#include <stdlib.h>
#include <stdio.h>
#include "module3.h"

#define MODNAME  "MODULE_3"

int init_module_3(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    return x;
}

int start_module_3(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    return x;
}

int stop_module_3(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    return x;
}
