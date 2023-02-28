#include <stdlib.h>
#include "vault.h"
#include "assert.h"

#define VLT_PATH "/home/wne/dev/c/Vault"

static Vault vlt;

int main(int argc, char *argv[]) {
    vltinit(&vlt, VLT_PATH);
    vltadd(&vlt, "unix");
    return EXIT_SUCCESS;
}
