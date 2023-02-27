/*
 * Includes.
 */
#include <stdio.h>
#include <string.h>
#include "vault.h"
#include "io.h"
#include "types.h"
#include "util.h"

/*
 * Definitions.
 */
#define VAULT_DIR_NAME "vault"
#define MASTER_DIR_NAME "_master"
#define NUM_PROTECTED_NAMES (1u)
#define VAULT_KEY (250lu)
#define HASH_MAX_LENGTH (200u)

/*
 * Typedefs.
 */
typedef char*(*Encryption)(const char*, size_t n, size_t key);
typedef char*(*Decryption)(const char*, size_t n, size_t key);
typedef void(*Cleanup)(void);

/*
 * Local variables.
 */
static const char* protectednames[NUM_PROTECTED_NAMES] = {MASTER_DIR_NAME};

/*
 * Local function declarations.
 */
static bool isprot(const char* name);
static uint64_t keygen();
static const char* hash(const char* data, size_t n);
static void fdump(const char* filename, const char* modes, const char* data, const Cleanup cleanupcb);
static void fundump(const char* filename,const char* modes, char* data, size_t n, const Cleanup cleanupcb);
static void newtf(Vault*);
static void loadff(Vault*);

/*
 * Global function declarations.
 */
Vault vltinit(const char*);

/*
 * Local function definitions.
 */
static bool isprot(const char* name) {
    uint32_t i;
    for (i = 0; i < NUM_PROTECTED_NAMES; ++i) {
        if (strcmp(name, protectednames[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

static size_t keygen() {
    return 20lu;
}

static const char* hash(const char* data, size_t n) {
    return data;
}

static void fdump(const char* filename, const char* modes, const char* data, const Cleanup cleanupcb) {
    FILE* fptr;
    if ((fptr = fopen(filename, modes)) == NULL) {
        if (cleanupcb != NULL) cleanupcb();
        EXIT("Could not open file: '%s'\n", filename);
    }
    if ((fputs(data, fptr)) == EOF) {
        if (cleanupcb != NULL) cleanupcb();
        EXIT("Could not write to file: '%s'\n", filename);
    }
    fclose(fptr);
}

static void fundump(const char* filename, const char* modes, char* data, size_t n, const Cleanup cleanupcb) {
    FILE* fptr;
    if ((fptr = fopen(filename, modes)) == NULL) {
        if (cleanupcb != NULL) cleanupcb();
        EXIT("Could not open file: '%s'\n", filename);
    }
    if ((fgets(data, n, fptr)) == NULL) {
        if (cleanupcb != NULL) cleanupcb();
        EXIT("Could not read from file: '%s'\n", filename);
    }
    fclose(fptr);
}

static void newtf(Vault* vlt) {
    // create top-level/root directory
    if (fmkdir(vlt->path) == FALSE) {
        EXIT("Could not create dir: '%s'\n", vlt->path);
    }

    // create _master dir path
    char masterpath[PATH_MAX_LENGTH];
    topath(masterpath, vlt->path, 1, MASTER_DIR_NAME);

    // create _master dir
    if (fmkdir(masterpath) == FALSE) {
        // Todo: cleanup
        EXIT("Could not create dir: '%s'\n", masterpath);
    }

    // create _master-password path
    char masterpasswordpath[PATH_MAX_LENGTH];
    topath(masterpasswordpath, masterpath, 1, "password.txt");

    // read master-password
    char masterpassword[PASSWORD_MAX_LENGTH];
    fprintf(stdout, "Select a Master Password: ");
    freadpw(masterpassword, PASSWORD_MAX_LENGTH);
    fprintf(stdout, "\n");

    // hash and write _master-password to file (tmp unencrypted)
    const char* masterpasswordhash = hash(masterpassword, HASH_MAX_LENGTH);
    fdump(masterpasswordpath, "w", masterpasswordhash, NULL);

    // create _master-key path
    char masterkeypath[PATH_MAX_LENGTH];
    topath(masterkeypath, masterpath, 1, "key.txt");

    // generate master-key
    uint64_t masterkey = keygen();

    // write _master-key to file (tmp unencrypted)
    char str[256];
    sprintf(str, "%zu", masterkey);
    fdump(masterkeypath, "w", str, NULL);

    // update vault struct
    vlt->key = VAULT_KEY;
    strcpy(vlt->masterpasswordhash, masterpasswordhash);
    vlt->masterkey = masterkey;
}

static void loadff(Vault* vlt) {
    // create _master dir path
    char masterpath[PATH_MAX_LENGTH];
    topath(masterpath, vlt->path, 1, MASTER_DIR_NAME);

    // check that _master dir exists
    if (fisdir(masterpath) == FALSE) {
        EXIT("Directory: '%s' does not exists\n", masterpath);
    }

    // create _master-password path
    char masterpasswordpath[PATH_MAX_LENGTH];
    topath(masterpasswordpath, masterpath, 1, "password.txt");

    // check that _master-password exists
    if (fexists(masterpasswordpath) == FALSE) {
        EXIT("File: '%s' does not exists\n", masterpasswordpath);
    }

    // read master-password from file
    char masterpasswordhash[HASH_MAX_LENGTH];
    fundump(masterpasswordpath, "r", masterpasswordhash, HASH_MAX_LENGTH, NULL);

    // create _master-key path
    char masterkeypath[PATH_MAX_LENGTH];
    topath(masterkeypath, masterpath, 1, "key.txt");

    // check that _master-key exists
    if (fexists(masterkeypath) == FALSE) {
        EXIT("File: '%s' does not exist\n", masterkeypath);
    }

    // read master-key from file
    char str[256];
    fundump(masterkeypath, "r", str, 256, NULL);
    uint64_t masterkey = (uint64_t)atoll(str);

    // update vault struct
    vlt->key = VAULT_KEY;
    strcpy(vlt->masterpasswordhash, masterpasswordhash);
    vlt->masterkey = masterkey;
}

/*
 * Global function definitions.
 */
Vault vltinit(const char* path) {
    // create vault and vault dir path
    Vault vlt;
    char root[PATH_MAX_LENGTH];
    topath(root, path, 1, VAULT_DIR_NAME);
    strcpy(vlt.path, root);

    if (fexists(root) == TRUE) {
        // vault already exists
        loadff(&vlt);
    } else {
        // new vault should be created
        newtf(&vlt);
    }

    return vlt;
}

bool vltadd(const Vault* vault, const char* name) {
    // check precondition
    uint32_t i;
    for (i = 0; i < NUM_PROTECTED_NAMES; ++i) {
        if (strcmp(name, protectednames[i]) == 0) {
            EXIT("Name: '%s' is protected\n", name);
        }
    }

    // create name dir path
    char namepath[PATH_MAX_LENGTH];
    topath(namepath, vault->path, 1, name);

    // check if name dir already exists
    if (fexists(namepath) == TRUE) {
        fprintf(stdout, "Entry for name: '%s' already exists\n", name);
        return FALSE;
    }

    // create name dir
    if (fmkdir(namepath) == FALSE) {
        EXIT("Could not create dir: '%s'\n", namepath);
    }

    /*
     * 1. specify password
     * 2. auth using master-password
     */

    // read password, and generate hash & key
    char password[PASSWORD_MAX_LENGTH];
    fprintf(stdout, "Password: ");
    freadpw(password, PASSWORD_MAX_LENGTH);
    fprintf(stdout, "\n");
    const char* passwordhash = hash(password, HASH_MAX_LENGTH);
    uint64_t key = keygen();

    // read master-password
    char masterpassword[PASSWORD_MAX_LENGTH];
    fprintf(stdout, "Master-Password: ");
    freadpw(masterpassword, PASSWORD_MAX_LENGTH);
    fprintf(stdout, "\n");
    const char* masterpasswordhash = hash(masterpassword, HASH_MAX_LENGTH);

    if (strcmp(vault->masterpasswordhash, masterpasswordhash) != 0) {
        EXIT("Could not authenticate using Master-Password\n", NULL);
    }

    // create password path
    char passwordpath[PATH_MAX_LENGTH];
    topath(passwordpath, namepath, 1, "password.txt");

    // write password to file
    fdump(passwordpath, "w", passwordhash, NULL);

    // create key path
    char keypath[PATH_MAX_LENGTH];
    topath(keypath, namepath, 1, "key.txt");

    // write key to file
    char str[256];
    sprintf(str, "%zu", key);
    fdump(keypath, "w", str, NULL);

    return TRUE;
}
