#ifndef VAULT_UTIL_H
#define VAULT_UTIL_H

#include <stdlib.h>
#include <unistd.h>

#ifdef _LINUX
#define FILE_SEP "/"
#define FILE_SEP_C '/'
#elif _WINDOWS
#define FILE_SEP "\\"
#define FILE_SEP_C '\\'
#endif

#define EXIT(format, ...) do { fprintf(stderr, format, __VA_ARGS__); exit(EXIT_FAILURE); } while (0)
#define GET_CWD(cwd, n) (getcwd(cwd, n) != NULL)

extern void topath(char* path, const char* authority, size_t n, ...);

#endif //VAULT_UTIL_H
