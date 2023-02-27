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

/*
 * Typedefs.
 */
typedef char*(*Encryption)(const char*, size_t n, size_t key);
typedef char*(*Decryption)(const char*, size_t n, size_t key);
typedef void(*Cleanup)(void);

/*
 * Local variables.
 */
static const char* protectedNames[NUM_PROTECTED_NAMES]={MASTER_DIR_NAME };

/*
 * Local function declarations.
 */
static bool isnameprotected(const char* name);
static uint64_t keygen();
static const char* hash(const char* data, size_t n);
static void fdump(const char* filename, const char* modes, const char* data, const Cleanup cleanupcb);
static void fundump(const char* filename,const char* modes, char* data, size_t n, const Cleanup cleanupcb);
static void newtf(Vault *vlt, const char *vpath);
static void loadff(Vault*, const char*);

/*
 * Global function declarations.
 */
Vault vltinit(const char*);

/*
 * Local function definitions.
 */
static bool isnameprotected(const char* name) {
    uint32_t i;
    for (i = 0; i < NUM_PROTECTED_NAMES; ++i) {
        if (strcmp(name, protectedNames[i]) == 0) {
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

static void newtf(Vault* vlt, const char* vpath) {
    // create new top-level directory
    if (fmkdir(vpath) == FALSE) {
        EXIT("Could not create dir: '%s'\n", vpath);
    }

    // read master-password, and generate key
    fprintf(stdout, "Select a Master Password: ");
    char masterpassword[PASSWORD_MAX_LENGTH];
    freadpw(masterpassword, PASSWORD_MAX_LENGTH);
    fprintf(stdout, "\n");
    uint64_t masterkey = keygen();

    // create _master dir path
    char masterpath[PATH_MAX_LENGTH];
    topath(masterpath, vpath, 1, MASTER_DIR_NAME);

    // create _master dir
    if (fmkdir(masterpath) == FALSE) {
        // Todo: cleanup
        EXIT("Could not create dir: '%s'\n", masterpath);
    }

    // create _master-password path
    char masterpasswordpath[PATH_MAX_LENGTH];
    topath(masterpasswordpath, masterpath, 1, "password.txt");

    // create _master-key path
    char masterkeypath[PATH_MAX_LENGTH];
    topath(masterkeypath, masterpath, 1, "key.txt");

    // hash and write _master-password to file (tmp unencrypted)
    const char* masterpasswordhash = hash(masterpassword, PATH_MAX_LENGTH);
    fdump(masterpasswordpath, "w", masterpasswordhash, NULL);

    // write _master-key to file (tmp unencrypted)
    char str[256];
    sprintf(str, "%zu", masterkey);
    fdump(masterkeypath, "w", str, NULL);

    // update vault struct data members
    strcpy(vlt->path, vpath);
    vlt->key = VAULT_KEY;
    strcpy(vlt->masterpasswordhash, masterpasswordhash);
    vlt->masterkey = masterkey;
}

static void loadff(Vault* vlt, const char* path) {
    // create vault dir path
    char vpath[PATH_MAX_LENGTH];
    strcpy(vpath, path);

    // create _master dir path
    char masterpath[PATH_MAX_LENGTH];
    topath(masterpath, vpath, 1, MASTER_DIR_NAME);

    // check that _master dir exists
    if (fisdir(masterpath) == FALSE) {
        EXIT("Directory: '%s' does not exists.\n", masterpath);
    }

    // create _master-password path
    char masterpasswordpath[PATH_MAX_LENGTH];
    topath(masterpasswordpath, masterpath, 1, "password.txt");

    // create _master-key path
    char masterkeypath[PATH_MAX_LENGTH];
    topath(masterkeypath, masterpath, 1, "key.txt");

    // check that _master-password exists
    if (fexists(masterpasswordpath) == FALSE) {
        EXIT("File: '%s' does not exists.\n", masterpasswordpath);
    }

    // check that _master-key exists
    if (fexists(masterkeypath) == FALSE) {
        EXIT("File: '%s' does not exist.\n", masterkeypath);
    }

    // read master-password from file
    char masterpasswordhash[PASSWORD_MAX_LENGTH];
    fundump(masterpasswordpath, "r", masterpasswordhash, PASSWORD_MAX_LENGTH, NULL);

    // read master-key from file
    char str[256];
    fundump(masterkeypath, "r", str, 256, NULL);
    uint64_t masterkey = (uint64_t)atoll(str);

    // update vault struct data members
    strcpy(vlt->path, vpath);
    vlt->key = VAULT_KEY;
    strcpy(vlt->masterpasswordhash, masterpasswordhash);
    vlt->masterkey = masterkey;
}


/*
 * Global function definitions.
 */
Vault vltinit(const char* path) {
    Vault vlt;
    // create cwd path
    char cwd[PATH_MAX_LENGTH];
    strcpy(cwd, path);

    // create vault dir path
    char vpath[PATH_MAX_LENGTH];
    topath(vpath, cwd, 1, VAULT_DIR_NAME);

    if (fexists(vpath) == TRUE) {
        // vault already exists
        loadff(&vlt, vpath);
    } else {
        // new vault should be created
        newtf(&vlt, vpath);
    }

    return vlt;
}
