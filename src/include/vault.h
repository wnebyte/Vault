#ifndef VAULT_VAULT_H
#define VAULT_VAULT_H

#include "types.h"

#define PATH_MAX_LENGTH (100u)

typedef struct Vault {
    char path[PATH_MAX_LENGTH];
    uint64_t key;
    char masterpasswordhash[PATH_MAX_LENGTH];
    uint64_t masterkey;
} Vault;

extern void vltinit(Vault*, const char* path);

extern bool vltadd(const Vault*, const char* name);

extern void vltget(const Vault*, const char* name, char* pw);

extern void vltcp(const Vault*, const char* name);

#endif //VAULT_VAULT_H
