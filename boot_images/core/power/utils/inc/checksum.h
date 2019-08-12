#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "DALStdDef.h"

typedef struct
{
    uint8 *data;
    uint32 size;
} checksum_data_t;

/******************************************************************************
 *
 * This function returns a 16-bit Fletcher checksum.
 *
 *  data_arr - an array of checksum_data_t which contains 8-bit data array and
 *             a number of 8-bit data
 *  arr_size - a number of checksum_data_t
 *
 */
uint16 checksum16(const checksum_data_t *data_arr, uint32 arr_size);

/******************************************************************************
 *
 * This functions returns a 32-bit Fletcher checksum.
 *
 *  data - an array of 16-bit data
 *  size - a number of 16-bit data
 *
 */
uint32 checksum32(const uint16 *data, uint32 size);

#endif /* CHECKSUM_H */
