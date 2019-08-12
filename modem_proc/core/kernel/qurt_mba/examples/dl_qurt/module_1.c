#include <stdlib.h>
#include <stdio.h>

#define MODNAME  "MODULE_1"

int init_module_1(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    return x;
}

int start_module_1(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    return x;
}

int stop_module_1(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    return x;
}
