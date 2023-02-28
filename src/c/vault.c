/*************************************************************
 * Includes
 *************************************************************
 */
#include <stdio.h>
#include <string.h>
#include "vault.h"
#include "io.h"
#include "types.h"
#include "util.h"

/*************************************************************
 * Definitions
 *************************************************************
 */
#define VAULT_DIR_NAME      "vault"
#define MASTER_DIR_NAME     "_master"
#define PASSWORD_FILE_NAME  "password.txt"
#define KEY_FILE_NAME       "key.txt"
#define WRITE_MODE          "w"
#define READ_MODE           "r"
#define NUM_PROTECTED_NAMES (1u)
#define VAULT_KEY           (250lu)
#define HASH_MAX_LENGTH     (200lu)

/*************************************************************
 * Typedefs
 *************************************************************
 */
typedef char* Encryption(const char*, size_t, sbyte*, size_t);
typedef char* Decryption(const sbyte*, size_t, char*, size_t);
typedef void Cleanup(void);

/*************************************************************
 * Local variables
 *************************************************************
 */
static const char* protectednames[NUM_PROTECTED_NAMES] = {MASTER_DIR_NAME};

/*************************************************************
 * Local function declarations
 *************************************************************
 */
static bool isprot(const char* name);
static uint64_t keygen();
static char* hash(const char*, size_t, char*, size_t);
static void fdump(const char* filename, const char* modes, const char* data, Cleanup cleanupcb);
static void fundump(const char* filename, const char* modes, char* data, size_t n, Cleanup cleanupcb);
static void newtf(Vault*);
static void loadff(Vault*);

/*************************************************************
 * Global function declarations
 *************************************************************
 */
void vltinit(Vault*, const char*);
bool vltadd(const Vault*, const char*);

/*************************************************************
 * Local function definitions
 *************************************************************
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

// Todo: impl
static size_t keygen() {
    return 20lu;
}

// TODO: impl
static char* hash(const char* str, size_t str_n, char* hash, size_t hash_n) {
    strncpy(hash, str, str_n);
    return hash;
}

static void fdump(const char* filename, const char* modes, const char* data, Cleanup cleanupcb) {
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

static void fundump(const char* filename, const char* modes, char* data, size_t n, Cleanup cleanupcb) {
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
    topath(masterpasswordpath, masterpath, 1, PASSWORD_FILE_NAME);

    // read master-password
    char masterpassword[PASSWORD_MAX_LENGTH];
    fprintf(stdout, "Select a Master Password: ");
    freadpw(masterpassword, PASSWORD_MAX_LENGTH);

    // hash and write _master-password to file (tmp unencrypted)
    char masterpasswordhash[HASH_MAX_LENGTH];
    hash(masterpassword, PASSWORD_MAX_LENGTH, masterpasswordhash, HASH_MAX_LENGTH);
    fdump(masterpasswordpath, WRITE_MODE, masterpasswordhash, NULL);

    // create _master-key path
    char masterkeypath[PATH_MAX_LENGTH];
    topath(masterkeypath, masterpath, 1, KEY_FILE_NAME);

    // generate master-key
    uint64_t masterkey = keygen();

    // write _master-key to file (tmp unencrypted)
    char str[256];
    sprintf(str, "%zu", masterkey);
    fdump(masterkeypath, WRITE_MODE, str, NULL);

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
    topath(masterpasswordpath, masterpath, 1, PASSWORD_FILE_NAME);

    // check that _master-password exists
    if (fexists(masterpasswordpath) == FALSE) {
        EXIT("File: '%s' does not exists\n", masterpasswordpath);
    }

    // read master-password from file
    char masterpasswordhash[HASH_MAX_LENGTH];
    fundump(masterpasswordpath, READ_MODE, masterpasswordhash, HASH_MAX_LENGTH, NULL);

    // create _master-key path
    char masterkeypath[PATH_MAX_LENGTH];
    topath(masterkeypath, masterpath, 1, KEY_FILE_NAME);

    // check that _master-key exists
    if (fexists(masterkeypath) == FALSE) {
        EXIT("File: '%s' does not exist\n", masterkeypath);
    }

    // read master-key from file
    char str[256];
    fundump(masterkeypath, READ_MODE, str, 256, NULL);
    uint64_t masterkey = (uint64_t)atoll(str);

    // update vault struct
    vlt->key = VAULT_KEY;
    strcpy(vlt->masterpasswordhash, masterpasswordhash);
    vlt->masterkey = masterkey;
}

/*************************************************************
 * Global function definitions
 *************************************************************
 */
void vltinit(Vault* vlt, const char* path) {
    // create top-level/root path
    topath(vlt->path, path, 1, VAULT_DIR_NAME);

    if (fexists(vlt->path) == TRUE) {
        // vault already exists
        loadff(vlt);
    } else {
        // new vault should be created
        newtf(vlt);
    }
}

bool vltadd(const Vault* vault, const char* name) {
    // check precondition
    if (isprot(name)) {
        EXIT("Name: '%s' is protected\n", name);
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

    // read password, and generate key
    char password[PASSWORD_MAX_LENGTH];
    fprintf(stdout, "Password: ");
    freadpw(password, PASSWORD_MAX_LENGTH);
    uint64_t key = keygen();
    
    // read master-password
    // Todo: afford user n auth attempts
    char masterpassword[PASSWORD_MAX_LENGTH];
    fprintf(stdout, "Master-Password: ");
    freadpw(masterpassword, PASSWORD_MAX_LENGTH);
    char masterpasswordhash[HASH_MAX_LENGTH];
    hash(masterpassword, PASSWORD_MAX_LENGTH, masterpasswordhash, HASH_MAX_LENGTH);

    if (strcmp(vault->masterpasswordhash, masterpasswordhash) != 0) {
        EXIT("Could not authenticate using Master-Password\n", NULL);
    }

    // create password path
    char passwordpath[PATH_MAX_LENGTH];
    topath(passwordpath, namepath, 1, PASSWORD_FILE_NAME);

    // write password to file
    fdump(passwordpath, WRITE_MODE, password, NULL);

    // create key path
    char keypath[PATH_MAX_LENGTH];
    topath(keypath, namepath, 1, KEY_FILE_NAME);

    // write key to file
    char str[256];
    sprintf(str, "%zu", key);
    fdump(keypath, WRITE_MODE, str, NULL);

    return TRUE;
}
