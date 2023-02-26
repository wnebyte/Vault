#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include "vault.h"
#include "cmds/init.h"
#include "io.h"
#include "cmds/add.h"
#include "cmds/cp.h"

#define PROJ_PATH "/home/wne/dev/c/Pwm/"
#define VAULT_PATH "/home/wne/dev/c/Pwm/data/output/vault"
#define INIT_CMD "init"
#define ADD_CMD "add"
#define CP_CMD "cp"

static void usage() {
    fprintf(stdout, "Usage: <NAME> CMD [OPTIONS]\n");
}

int main(int argc, char *argv[]) {
    /*
    if (argc >= 2) {
        const char* cmd = argv[1];
        argc -= 2;
        argv += 2;
        if (strcmp(cmd, INIT_CMD) == 0) {
            init_exe(argc, argv);
        } else if (strcmp(cmd, ADD_CMD) == 0) {
            add_exe(argc, argv);
        } else if (strcmp(cmd, CP_CMD) == 0) {
            cp_exe(argc, argv);
        } else {
            usage();
        }
    } else {
        usage();
    }
    */

}
