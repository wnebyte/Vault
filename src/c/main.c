#include <stdlib.h>
#include "vault.h"
#include "assert.h"

#define VLT_PATH "/home/wne/dev/c/Vault"

int main(int argc, char *argv[]) {
    Vault vlt = vltinit(VLT_PATH);
    vltadd(&vlt, "unix");
    return EXIT_SUCCESS;
}
