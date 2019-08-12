#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "module3.h"

#define MODNAME  "MODULE_2"

int init_module_2(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    assert(2 == init_module_3(2));
    return x;
}

int start_module_2(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    printf("Calling MODULE_3's start function\n");
    assert(2 == start_module_3(2));
    return x;
}

int stop_module_2(int x)
{
    printf(MODNAME ": In function %s. Got param %d\n", __func__, x);
    printf("Calling MODULE_3's stop function\n");
    assert(2 == stop_module_3(2));
    return x;
}
