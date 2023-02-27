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

extern Vault vltinit(const char* path);

extern void vltadd(const Vault*, const char* name, const char* pw);

extern void vltget(const Vault*, const char* name, char* pw);

extern void vltcp(const Vault*, const char* name);

#endif //VAULT_VAULT_H
