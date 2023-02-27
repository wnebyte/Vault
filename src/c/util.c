#include <stdarg.h>
#include <string.h>
#include "util.h"

void topath(char* path, const char* authority, size_t n, ...) {
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
