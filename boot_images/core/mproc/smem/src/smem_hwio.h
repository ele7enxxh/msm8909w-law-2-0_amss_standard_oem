#ifndef __SMEM_HWIO_H__
#define __SMEM_HWIO_H__
/*
===========================================================================
*/
/**
  @file smem_hwio.h
  @brief Semi-auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TCSR_TCSR_MUTEX
    TCSR_TCSR_REGS

  'Include' filters applied: MUTEX_REG_n[TCSR_TCSR_MUTEX] TZ_WONCE[TCSR_TCSR_REGS] 
  'Exclude' filters applied: RESERVED DUMMY
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/src/smem_hwio.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/13   bt      Added HWIO_TCSR_TZ_WONCE_n macros.
10/18/13   bt      Reformat macros, remove unnecessary _REG_n_PHYS(n).
09/26/13   sm      Initial revision for 8916.
===========================================================================*/

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/

extern uint8 *smem_hw_mutex_base_ptr;
#define CORE_TOP_CSR_BASE_MUTEX         smem_hw_mutex_base_ptr
#define smem_hw_mutex_reg_base_phys     CORE_TOP_CSR_BASE_PHYS
#define smem_hw_mutex_reg_base_size     CORE_TOP_CSR_BASE_SIZE

/* Needed only for the DAL MapRegion function, on procs that use DAL. */
#define SMEM_HW_MUTEX_BASE_NAME         ("CORE_TOP_CSR")

#define TCSR_TCSR_MUTEX_REG_BASE        (CORE_TOP_CSR_BASE_MUTEX + 0x00005000)
#define TCSR_TCSR_MUTEX_REG_BASE_PHYS   (CORE_TOP_CSR_BASE_PHYS  + 0x00005000)

#define HWIO_TCSR_MUTEX_REG_n_ADDR(n)   (TCSR_TCSR_MUTEX_REG_BASE + \
                                          0x00000000 + 0x1000 * (n))
#define HWIO_TCSR_MUTEX_REG_n_RMSK                      0xff
#define HWIO_TCSR_MUTEX_REG_n_MAXn                        31
#define HWIO_TCSR_MUTEX_REG_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), \
                        HWIO_TCSR_MUTEX_REG_n_RMSK)
#define HWIO_TCSR_MUTEX_REG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), mask)
#define HWIO_TCSR_MUTEX_REG_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_MUTEX_REG_n_ADDR(n),val)
#define HWIO_TCSR_MUTEX_REG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_n_ADDR(n),mask,val, \
          HWIO_TCSR_MUTEX_REG_n_INI(n))
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_BMSK                0xff
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_SHFT                 0x0

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#ifdef SMEM_BOOT_IMAGE
#include "msmhwiobase.h"
#define CORE_TOP_CSR_BASE_TARG_INFO_ADDR  CORE_TOP_CSR_BASE
#else
extern uint8 *smem_targ_info_addr_base_ptr;
#define CORE_TOP_CSR_BASE_TARG_INFO_ADDR  smem_targ_info_addr_base_ptr
#endif

/* Needed only for the DAL MapRegion function, on procs that use DAL. */
#define SMEM_TARG_INFO_ADDR_BASE_NAME     ("CORE_TOP_CSR")

#define TCSR_TCSR_REGS_REG_BASE           (CORE_TOP_CSR_BASE_TARG_INFO_ADDR + \
                                            0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS      (CORE_TOP_CSR_BASE_PHYS + 0x00037000)

#define HWIO_TCSR_TZ_WONCE_n_ADDR(n)      (TCSR_TCSR_REGS_REG_BASE + \
                                            0x00006000 + 0x4 * (n))
#define HWIO_TCSR_TZ_WONCE_n_RMSK                           0xffffffff
#define HWIO_TCSR_TZ_WONCE_n_MAXn                           15
#define HWIO_TCSR_TZ_WONCE_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_TZ_WONCE_n_ADDR(n), HWIO_TCSR_TZ_WONCE_n_RMSK)
#define HWIO_TCSR_TZ_WONCE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_TZ_WONCE_n_ADDR(n), mask)
#define HWIO_TCSR_TZ_WONCE_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_TZ_WONCE_n_ADDR(n),val)
#define HWIO_TCSR_TZ_WONCE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_TZ_WONCE_n_ADDR(n),mask,val, \
                            HWIO_TCSR_TZ_WONCE_n_INI(n))
#define HWIO_TCSR_TZ_WONCE_n_TZ_WONCE_ADDRESS_BMSK          0xffffffff
#define HWIO_TCSR_TZ_WONCE_n_TZ_WONCE_ADDRESS_SHFT          0x0

#endif /* __SMEM_HWIO_H__ */
