#ifndef TZBSP_KRAIT_CP15_H
#define TZBSP_KRAIT_CP15_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_krait_cp15.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/02/12   tk      First version.
============================================================================*/

/** Macro to generate L0/L1 i-cache line word selection argument for ICRSW. */
#define L1_ICRSW_ARG(xxset, xxway, xxword) \
    (((xxset) << 6) | ((xxway) << 0x1e) | ((xxword) << 2))

/** Macro to generate L0/L1 d-cache line word selection argument for DCRSW. */
#define L1_DCRSW_ARG(xxset, xxway, xxword) \
    (((xxset) << 6) | ((xxway) << 0x1e) | ((xxword) << 2))

/** Macro to generate L2 d-cache line word selection argument for DCRSW. Note
 * that xxword is indexing double words (64 bit). */
#define L2_DCRSW_ARG(xxset, xxway, xxword) \
    (((xxset) << 7) | ((xxway) << 0x1d) | ((xxword) << 3) | 0x2)

/** L2 register offsets for L2CPUCPSELR, @see qctps_common_macros.inc. */
#define L2ESR_IA                0x204
#define L2ESYNR0_IA             0x208
#define L2ESYNR1_IA             0x209
#define L2EAR0_IA               0x20C
#define L2EAR1_IA               0x20D
#define L2DCRDR0_IA             0x300
#define L2DCRDR1_IA             0x301
#define L2DCRTR0_IA             0x302
#define L2DCRTR1_IA             0x303

void wcp15_dcrsw(uint32 val);
void wcp15_icrsw(uint32 val);

uint32 rcp15_dcrtr0(void);
uint32 rcp15_dcrdr0(void);

uint32 rcp15_icrtr0(void);
uint32 rcp15_icrdr0(void);
uint32 rcp15_icrdr1(void);

uint32 rcp15_l0dcrtr0(void);
uint32 rcp15_l0dcrdr0(void);

uint32 rcp15_l0icrtr0(void);
uint32 rcp15_l0icrdr0(void);
uint32 rcp15_l0icrdr1(void);

uint32 read_l2_data(uint32 offset);

uint32 rcp15_pvr2f0(void);
void wcp15_pvr2f0(uint32 val);

uint32 rcp15_vbar(void);
void wcp15_vbar(uint32);

#endif /* TZBSP_KRAIT_CP15_H */
