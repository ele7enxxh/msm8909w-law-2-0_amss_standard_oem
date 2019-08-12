#include <two_threads.h>

const int swapped_data[test_range] = {0xdeadbeef,{0}}; //256KB


int test_func(void)
{
    printf("enter test_func\n");
    return 0;
}
