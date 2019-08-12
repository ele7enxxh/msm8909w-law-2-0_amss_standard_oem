#include "../config/devcfg_lsm_target_devcfg.h"
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int int16;
typedef unsigned short int uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int int64;
typedef unsigned long long int uint64;
typedef unsigned char boolean;
typedef struct
{
   uint32 sw_revision;
   uint32 bw[10];
   uint32 cpp[10];
} lpasshwio_prop_lsm_mmpm_struct_t;
lpasshwio_prop_lsm_mmpm_struct_t lsm_mmpm_prop =
{ 1,
        {22300 , 27000, 31000, 34000, 65000, 100000, 111000, 134000, 160000, 175000},
        {100 , 100 , 100 , 100 , 100 , 100 , 100 , 100 , 100 , 100 },
};
