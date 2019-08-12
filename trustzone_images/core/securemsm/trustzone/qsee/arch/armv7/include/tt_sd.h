#ifndef TZBSP_MMU_CONFIG_H
#define TZBSP_MMU_CONFIG_H

/*===========================================================================
                    Boot Scorpion MMU Definitions Header File

GENERAL DESCRIPTION
    Definitions specific to the Scorpion Memory Management Unit.  Borrowed
    from //source/qcom/qct/core/pkg/modem/mp/arm11/rel/1.0/modem_proc/core/boot/secboot3/common/scorpion_mmu.h#2

Copyright 2011 by Qualcomm Technologies Incorporated. All Rights Reserved.

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/include/tt_sd.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/25/11   pre     initial revision
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "tzbsp_target.h"

/*
   The mmu configuration is composed of 4096 word entries which each map
   one megabyte of memory.
   4GB/1MB -> 4096 word entries
   The page table contains either a section descriptor or a coarse page
   table descriptor.

   A section descriptor for L1 is composed as follows (Descriptor = 0b10):
 ______________________________________________________________________________
|              |     |  |  |     |    |      | |      |   |         |          |
|31----------20|19-18|17|16|-15-|14-12|11--10|9|8----5|-4-|3-------2|1------- 0|
|P Address base| SBZ |nG| S| APX| TEX |Access|P|DOMAIN| XN|Cach/Buff|Descriptor|
|______________|_____|__|__|____|_____|______|_|______|___|_________|__________|


   A coarse page table desc is composed as follows (Descriptor = 0b01):
 ______________________________________________________________________________
|                                                                              |
|31----------------------------------------------10|9|8------5|4|3 2|1 ------ 0|
|         coarse PT Address base                   |0| DOMAIN |0|0 0|Descriptor|
|______________________________________________________________________________|

  A level 2 page descriptor is composed as follows :
 ______________________________________________________________________
|                               |  |  |   |     |       |         |    |
|31---------------------------12|11|10| 9 |8 7 6|5 --- 4|3-------2|1  0|
|     Phys Address base         |nG|S |APX| TEX | Access|Cach/Buff|1 XN|
|_______________________________|__|__|___|_____|_______|_________|____|

*/


/*
   Bits [1:0] define the entry type
*/
/* Translation Fault */
#define INVALID_DESCRIPTOR                      0x0

/* Use 2nd Table for Fine sections */
#define L1_SECONDARY_TTE_DESCRIPTOR             0x1

/* L1 page is 1MB or 16MB */
#define L1_SECTION_DESCRIPTOR                   0x2

/* L2 page is 64KB */
#define L2_LARGE_PAGE_DESCRIPTOR                0x1

/* L2 page is 4KB */
#define L2_PAGE_DESCRIPTOR                      0x2

/* Reserved */
#define RESERVED_DESCRIPTOR                     0x3

/*
  Bits [14:13] are TEX bits [2:1] for L1 tables and are not used by
  the MMU hardware when TEX remap is enabled, and thus are called "OS
  managed"
 */
#define L1_OS_MANAGED_0                         (0 << 13)
#define L1_OS_MANAGED_1                         (1 << 13)
#define L1_OS_MANAGED_2                         (2 << 13)
#define L1_OS_MANAGED_3                         (3 << 13)

/*
  Bits [8:7] are TEX bits [2:1] for L2 tables and are not used by the
  MMU hardware when TEX remap is enabled, and thus are called "OS
  managed"
 */
#define L2_OS_MANAGED_0                         (0 << 7)
#define L2_OS_MANAGED_1                         (1 << 7)
#define L2_OS_MANAGED_2                         (2 << 7)
#define L2_OS_MANAGED_3                         (3 << 7)


/*
  Bit [16] specifies the S bit for L1 and Bit [10] for L2 - if a
  normal memory region is shared.
*/
#define L1_SHARED                               (1 << 16)
#define L1_NONSHARED                            (0 << 16)
#define L2_SHARED                               (1 << 10)
#define L2_NONSHARED                            (0 << 10)


/*
  Bits [3:2] are used with TEX bit 0 to index into PRRR/NMRR or MAIR0/MAIR1
*/
#define B_SET                                   (1 << 2)
#define B_CLEAR                                 (0 << 2)
#define C_SET                                   (1 << 3)
#define C_CLEAR                                 (0 << 3)

/*
  Bit 12 is the TEX bit for L1.  Together with B/C bits they specify
  an index into PRRR/NMRR or MAIR0/MAIR1
 */
#define L1_TEX_0                                (0 << 12)
#define L1_TEX_1                                (1 << 12)

/*
  Bit 6 is the TEX bit for L2.  Together with B/C bits they specify an
  index into PRRR/NMRR or MAIR0/MAIR1
 */
#define L2_TEX_0                                (0 << 6)
#define L2_TEX_1                                (1 << 6)

/*
  Memory Attribute Indirection Register (MAIR) settings
 */
#define L1_MAIR_0                               (L1_TEX_0 | C_CLEAR | B_CLEAR)
#define L1_MAIR_1                               (L1_TEX_0 | C_CLEAR | B_SET)
#define L1_MAIR_2                               (L1_TEX_0 | C_SET   | B_CLEAR)
#define L1_MAIR_3                               (L1_TEX_0 | C_SET   | B_SET)
#define L1_MAIR_4                               (L1_TEX_1 | C_CLEAR | B_CLEAR)
#define L1_MAIR_5                               (L1_TEX_1 | C_CLEAR | B_SET)
#define L1_MAIR_6                               (L1_TEX_1 | C_SET   | B_CLEAR)
#define L1_MAIR_7                               (L1_TEX_1 | C_SET   | B_SET)

#define L2_MAIR_0                               (L2_TEX_0 | C_CLEAR | B_CLEAR)
#define L2_MAIR_1                               (L2_TEX_0 | C_CLEAR | B_SET)
#define L2_MAIR_2                               (L2_TEX_0 | C_SET   | B_CLEAR)
#define L2_MAIR_3                               (L2_TEX_0 | C_SET   | B_SET)
#define L2_MAIR_4                               (L2_TEX_1 | C_CLEAR | B_CLEAR)
#define L2_MAIR_5                               (L2_TEX_1 | C_CLEAR | B_SET)
#define L2_MAIR_6                               (L2_TEX_1 | C_SET   | B_CLEAR)
#define L2_MAIR_7                               (L2_TEX_1 | C_SET   | B_SET)


#define PRR_TR_STRONGLY_ORDERED                 0
#define PRR_TR_DEVICE                           1
#define PRR_TR_NORMAL                           2
#define PRR_TR_RESERVED                         3

#define SET_PRRR_MEM(I, NOS, TR)                ((NOS & 1UL) << (24 + I) | \
                                                 ((TR & 3)   << (2 * I)))
#define SET_PRRR_NS(I, S)                       ((S & 1) << (18 + I))
#define SET_PRRR_DS(I, S)                       ((S & 1) << (16 + I))

#define NMRR_NON_CACHEABLE                      0
#define NMRR_WRITE_BACK_WA                      1
#define NMRR_WRITE_THROUGH_NWA                  2
#define NMRR_WRITE_BACK_NWA                     3

#define SET_NMRR_OR(I, OR)                      ((OR & 3) << (16 + (I * 2)))
#define SET_NMRR_IR(I, IR)                      ((IR & 3) << (I * 2))


#define L1_MEM_STRONGLY_ORDERED                  L1_MAIR_0
#define L1_MEM_DEVICE                            L1_MAIR_1
#define L1_MEM_NON_CACHEABLE                     L1_MAIR_2
#define L1_MEM_WRITE_THROUGH_WA                  L1_MAIR_3
#define L1_MEM_WRITE_THROUGH_NWA                 L1_MAIR_4
#define L1_MEM_WRITE_BACK_WA                     L1_MAIR_5
#define L1_MEM_WRITE_BACK_NWA                    L1_MAIR_6

#define L2_MEM_STRONGLY_ORDERED                  L2_MAIR_0
#define L2_MEM_DEVICE                            L2_MAIR_1
#define L2_MEM_NON_CACHEABLE                     L2_MAIR_2
#define L2_MEM_WRITE_THROUGH_WA                  L2_MAIR_3
#define L2_MEM_WRITE_THROUGH_NWA                 L2_MAIR_4
#define L2_MEM_WRITE_BACK_WA                     L2_MAIR_5
#define L2_MEM_WRITE_BACK_NWA                    L2_MAIR_6

/*
   Bits [4] is the eXecute Never bit for L1 and bit [0] is the eXecute
   Never bit for L2. The region is executable if set to 0 and not
   executable if set to 1.
*/
#define L1_EXECUTABLE                           (0 << 4)
#define L1_NON_EXECUTABLE                       (1 << 4)
#define L2_EXECUTABLE                           (0)
#define L2_NON_EXECUTABLE                       (1)

/*
   Bits [8:5] specify one of the 16 domains
   everything is mapped to one domain for simplicity:
   The default domain is set to 0
*/
#define L1_DEFAULT_DOMAIN                       (0 << 5)

/*
   Bit [9] Specifies the P bit - whether the memory
   region has ECC enabled. TRM p0r2 says "Should be Zero"
*/
#define L1_ECC_ENABLED                          (1 << 9)
#define L1_ECC_DISABLED                         (0 << 9)

/*
   Bits [11:10] Specifies the Access Permisions (AP bits) along with
   the APX bit [15] for L1. Bits [5:4] specifies the Access Permission
   (AP) along with the APX bit [9] for L2.  AP APX
*/
#define L1_READ_WRITE_PL1                       ((0 << 15) | (1 << 10))
#define L1_READ_WRITE_PL0                       ((0 << 15) | (3 << 10))
#define L1_READ_ONLY_PL1                        ((1 << 15) | (1 << 10))
#define L1_READ_ONLY_PL0                        ((1 << 15) | (3 << 10))
#define L2_READ_WRITE_PL1                       ((0 <<  9) | (1 <<  4))
#define L2_READ_WRITE_PL0                       ((0 <<  9) | (3 <<  4))
#define L2_READ_ONLY_PL1                        ((1 <<  9) | (1 <<  4))
#define L2_READ_ONLY_PL0                        ((1 <<  9) | (3 <<  4))

#define NUM_L1_TTES            (TZ_VAS_SIZE / TZBSP_1MB)
#define NUM_L2_TTS             17

#define TTE_SIZE 4

#define NUM_L2_TTES_PER_L1_TTE (TZBSP_1MB / TZBSP_4KB)

#define L1_MAP_SIZE  TZBSP_1MB
#define L2_MAP_SIZE  TZBSP_4KB
#define MIN_MAP_SIZE L2_MAP_SIZE

#define L1_SIZE_MSK ((uint64)(L1_MAP_SIZE - 1))
#define L2_SIZE_MSK ((uint64)(L2_MAP_SIZE - 1))

#define L1_ADDR_MSK ~L1_SIZE_MSK
#define L2_ADDR_MSK ~L2_SIZE_MSK

/* Right shifts are implementation defined, so define a mask that does
   not depend on right shifting the mask arithmetically.  Define size
   masks to be consistant.
 */
#define L1_KB_SIZE_MSK ((uint64)(B2KB(L1_MAP_SIZE) - 1))
#define L2_KB_SIZE_MSK ((uint64)(B2KB(L2_MAP_SIZE) - 1))

#define L1_KB_ADDR_MSK ((uint64)~(L1_KB_SIZE_MSK))
#define L2_KB_ADDR_MSK ((uint64)~(L2_KB_SIZE_MSK))

/* Alignment is determined by resultant page table size */
#define TZBSP_L1_PT_ALIGN_LOOKUP_0x00020000000     (11)
#define TZBSP_L1_PT_ALIGN_LOOKUP_0x00040000000     (12)
#define TZBSP_L1_PT_ALIGN_LOOKUP_0x00080000000     (13)
#define TZBSP_L1_PT_ALIGN_LOOKUP_0x00100000000     (14)

#define TZBSP_L1_PT_ALIGN_LOOKUP(xx)        (TZBSP_L1_PT_ALIGN_LOOKUP_IMPL(xx))
#define TZBSP_L1_PT_ALIGN_LOOKUP_IMPL(xx)   (TZBSP_L1_PT_ALIGN_LOOKUP_##xx##)

/* TTB0 is determined by page table size, Disallow TTBR1 TLB misses */
#define TZBSP_TTB0_LOOKUP_0x00020000000       ((1 << 5) | 3)
#define TZBSP_TTB0_LOOKUP_0x00040000000       ((1 << 5) | 2)
#define TZBSP_TTB0_LOOKUP_0x00080000000       ((1 << 5) | 1)
#define TZBSP_TTB0_LOOKUP_0x00100000000       (1 << 5)

#define TZBSP_TTB0_LOOKUP(xx)               (TZBSP_TTB0_LOOKUP_IMPL(xx))
#define TZBSP_TTB0_LOOKUP_IMPL(xx)          (TZBSP_TTB0_LOOKUP_##xx##)

/* TODO: This is here due to some crap in tzbsp_ext_os.c that should
   get fixed. */
#define B2KB(xx)     ((xx) >> 10)
#define B2L1TTE(xx)  ((xx) >> 20)
#define KB2L1TTE(xx) ((xx) >> 10)

#ifndef _ARM_ASM_

typedef uint32 sd_tte_t;

typedef struct
{
  sd_tte_t tt[NUM_L1_TTES];
} l1_tt_t;

typedef struct
{
  sd_tte_t tt[NUM_L2_TTES_PER_L1_TTE];
} l2_tt_t;

#endif /* _ARM_ASM_ */

#endif /* TZBSP_MMU_CONFIG_H */
