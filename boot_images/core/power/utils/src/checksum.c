#include "checksum.h"


uint16 checksum16(const checksum_data_t *data_arr, uint32 arr_size)
{
    uint16 sum1 = 0;
    uint16 sum2 = 0;

    while(arr_size--)
    {
        uint8 *data = data_arr->data;
        uint32 size = data_arr->size;

        while(size--)
        {
            sum1 = 0xff & (sum1 + *data++);
            sum2 = 0xff & (sum1 + sum2);
        }

        ++data_arr;
    }

    return (sum2 << 8) | sum1;
}

uint32 checksum32(const uint16 *data, uint32 size)
{
    uint32 sum1 = 0;
    uint32 sum2 = 0;

    while(size--)
    {
        sum1 = 0xffff & (sum1 + *data++);
        sum2 = 0xffff & (sum1 + sum2);
    }

    return (sum2 << 16) | sum1;
}

