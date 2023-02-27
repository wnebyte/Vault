#ifndef VAULT_ASSERT_H
#define VAULT_ASSERT_H

#include <stdio.h>
#include "util.h"
#include "types.h"

#define ASSERT(exp, format, ...) do { if (exp == FALSE) { EXIT(format, __VA_ARGS__); } } while (0)

#endif //VAULT_ASSERT_H
