#ifndef VAULT_IO_H
#define VAULT_IO_H

#include <bits/types/FILE.h>
#include "types.h"

#define PASSWORD_MAX_LENGTH (100u)

extern void freadpw(char* password, size_t n);

extern bool fexists(const char* path);

extern bool fisdir(const char* path);

extern bool fmkdir(const char* path);

#endif //VAULT_IO_H
