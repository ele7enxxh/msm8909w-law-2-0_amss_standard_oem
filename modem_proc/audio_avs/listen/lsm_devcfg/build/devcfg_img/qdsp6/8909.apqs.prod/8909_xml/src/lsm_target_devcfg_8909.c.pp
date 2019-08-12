# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/src/lsm_target_devcfg_8909.c"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/src/lsm_target_devcfg_8909.c" 1
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 136 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/src/lsm_target_devcfg_8909.c" 2
# 10 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/src/lsm_target_devcfg_8909.c"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/inc/lsm_target_devcfg.h" 1
# 12 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/inc/lsm_target_devcfg.h"
# 1 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/afe/drivers/hw/lpasshwio/hal/common/inc/typedef_drivers.h" 1
# 38 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/afe/drivers/hw/lpasshwio/hal/common/inc/typedef_drivers.h"
typedef signed char int8;




typedef unsigned char uint8;




typedef signed short int int16;




typedef unsigned short int uint16;




typedef signed int int32;




typedef unsigned int uint32;




typedef signed long long int int64;




typedef unsigned long long int uint64;




typedef unsigned char boolean;
# 13 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/inc/lsm_target_devcfg.h" 2
# 22 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/inc/lsm_target_devcfg.h"
typedef struct
{
   uint32 sw_revision;
   uint32 bw[10];
   uint32 cpp[10];
} lpasshwio_prop_lsm_mmpm_struct_t;
# 11 "/local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/audio_avs/listen/lsm_devcfg/target/src/lsm_target_devcfg_8909.c" 2

lpasshwio_prop_lsm_mmpm_struct_t lsm_mmpm_prop =
{ 1,
        {22300 , 27000, 31000, 34000, 65000, 100000, 111000, 134000, 160000, 175000},
        {100 , 100 , 100 , 100 , 100 , 100 , 100 , 100 , 100 , 100 },
};
