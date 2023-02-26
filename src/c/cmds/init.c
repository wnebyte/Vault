#include <stdio.h>
#include <getopt.h>
#include "cmds/init.h"
#include "vault.h"
#include "types.h"
#include "util.h"

static struct option long_options[] =
{
        {}
};

int init_exe(int argc, char *argv[]) {
    char cwd[PATH_MAX_LENGTH];
    if (GET_CWD(cwd, PATH_MAX_LENGTH) == FALSE) {
        fprintf(stderr, "Could not retrieve cwd.\n");
    }
    vltinit(cwd);
    return EXIT_SUCCESS;
}
