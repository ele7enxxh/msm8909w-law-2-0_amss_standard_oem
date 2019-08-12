/*
===========================================================================
*/
/**
  @file DALHwio.h
  @brief Semi-auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TCSR_TCSR_MUTEX
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/dal/sys/inc/DALHwio.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  ===========================================================================
*/

#ifndef __DAL_HWIO_H__
#define __DAL_HWIO_H__

#ifdef CORE_TOP_CSR_BASE
#undef CORE_TOP_CSR_BASE
#endif
extern uint8 *HWMutexBases;
#define CORE_TOP_CSR_BASE                           HWMutexBase

/* Needed only for the DAL MapRegion function, on procs that use DAL. */
#define HW_MUTEX_BASE_NAME ("CORE_TOP_CSR")

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/

#if((defined(FEATURE_MSM8916)) || (defined(FEATURE_MSM8936)) || (defined(FEATURE_MSM8909)) || (defined(FEATURE_MSM8937)) || (defined(FEATURE_MSM8952)) || (defined(FEATURE_MDM9607)))
#define TCSR_TCSR_MUTEX_REG_BASE                  (CORE_TOP_CSR_BASE      + 0x00005000)

#define HWIO_TCSR_MUTEX_REG_n_ADDR(n)             (TCSR_TCSR_MUTEX_REG_BASE      + 0x00000000 + 0x1000 * (n))
#define HWIO_TCSR_MUTEX_REG_n_RMSK                      0xff
#define HWIO_TCSR_MUTEX_REG_n_MAXn                        31
#define HWIO_TCSR_MUTEX_REG_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), HWIO_TCSR_MUTEX_REG_n_RMSK)
#define HWIO_TCSR_MUTEX_REG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), mask)
#define HWIO_TCSR_MUTEX_REG_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_MUTEX_REG_n_ADDR(n),val)
#define HWIO_TCSR_MUTEX_REG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_n_ADDR(n),mask,val,HWIO_TCSR_MUTEX_REG_n_INI(n))
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_BMSK                0xff
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_SHFT                 0x0

#else
#define TCSR_TCSR_MUTEX_REG_BASE                                            ( CORE_TOP_CSR_BASE      + 0x00004000 )
#define TCSR_TCSR_MUTEX_REG_BASE_PHYS                                       ( CORE_TOP_CSR_BASE_PHYS + 0x00004000 )
#define TCSR_TCSR_MUTEX_REG_BASE_SIZE                                       0x00005380

#define HWIO_TCSR_MUTEX_REG_n_ADDR(n)                                       ( TCSR_TCSR_MUTEX_REG_BASE      + 0x00000000 + 0x80 * (n) )
#define HWIO_TCSR_MUTEX_REG_n_RMSK                                                0xff
#define HWIO_TCSR_MUTEX_REG_n_MAXn                                                  31
#define HWIO_TCSR_MUTEX_REG_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), HWIO_TCSR_MUTEX_REG_n_RMSK)
#define HWIO_TCSR_MUTEX_REG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), (mask))
#define HWIO_TCSR_MUTEX_REG_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_MUTEX_REG_n_ADDR(n),val)
#define HWIO_TCSR_MUTEX_REG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_n_ADDR(n),mask,val,HWIO_TCSR_MUTEX_REG_n_INI(n))
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_BMSK                                          0xff
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_SHFT                                           0x0
#endif

#endif /* __DAL_HWIO_H__ */
