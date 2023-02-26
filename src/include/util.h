#ifndef PWM_UTIL_H
#define PWM_UTIL_H

#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>

#ifdef _LINUX
#define FILE_SEP "/"
#define FILE_SEP_C '/'
#elifdef _WINDOWS
#define FILE_SEP "\\"
#define FILE_SEP_C '\\'
#endif

#define EXIT(format, ...) do { fprintf(stderr, format, __VA_ARGS__); exit(EXIT_FAILURE); } while (0)
#define GET_CWD(cwd, n) (getcwd(cwd, n) != NULL)

void pathify(char* path, const char* authority, size_t n, ...) {
    va_list ap;
    char* arg;
    strcpy(path, authority);

    va_start(ap, n);
    while (n > 0) {
        arg = va_arg(ap, char*);
        strcat(path, FILE_SEP);
        strcat(path, arg);
        n--;
    }
}

#endif //PWM_UTIL_H
