#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <bits/stat.h>
#include <termios.h>
#include <unistd.h>
#include "io.h"
#include "util.h"

void freadpw(char* password, size_t n) {
    static struct termios ogt;
    static struct termios newt;

    // get settings of current terminal
    tcgetattr(STDIN_FILENO, &ogt);

    // turn off echo
    newt = ogt;
    newt.c_lflag &= ~ECHO;

    // set this a new terminal options
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // get the password
    if (fgets(password, n, stdin) == NULL) {
        password[0] = '\0';
    } else {
        n = strlen(password);
        password[n - 1] = '\0';
    }

    // reset terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &ogt);
}

bool fexists(const char* path) {
    struct stat statbuf;
    return (stat(path, &statbuf) == 0);
}

bool fisdir(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return FALSE;
    }
    return S_ISDIR(statbuf.st_mode);
}

bool fmkdir(const char* path) {
#if _LINUX
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        int32_t result = mkdir(path, 0700);
        return (result == 0);
    }
#endif
    return FALSE;
}
