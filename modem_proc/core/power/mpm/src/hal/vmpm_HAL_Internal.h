#ifndef VMPM_HAL_INTERNAL_H
#define VMPM_HAL_INTERNAL_H
/*=========================================================================

FILE:         vmpm_HAL_Internal.h

DESCRIPTION:  This is the hardware abstraction layer internal interface for 
              the MPM interrupt controller block.

DEPENDENCIES: None

              Copyright (c) 2008-2014 QUALCOMM Technologies, Inc. (QTI).
              All Rights Reserved.
              Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/hal/vmpm_HAL_Internal.h#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "vmpm_HAL.h"

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
 * vmpm_HAL_PlatformType
 *
 * Platform (asic/processor) specific information filled in by the platform
 * code in HAL_pic_PlatformEarlyInit.
 *
 *  WakeupReg     : The base of the interrupt enable register array.
 *  nEnableReg    : The base of the interrupt enable register array.
 *  nDetectReg    : The base of the interrupt detect ctl register array.
 *  nDetect3Reg   : The base of the interrupt detect ctl register array.
 *  nPolarityReg  : The base of the interrupt polarity register array.
 *  nStatusReg    : The base of the interrupt status register array.
 *  nEnableRegPad : The base of TLMM's wakeup interrupt enable register array.
 *  nClearReg     : The base of the interrupt clear register array.
 */
typedef struct
{
  uint32 WakeupReg;
  uint32 nEnableReg;
  uint32 nDetectReg;
  uint32 nDetect3Reg;
  uint32 nPolarityReg;
  uint32 nStatusReg;
  uint32 nEnableRegPad;
  uint32 nClearReg;
} vmpm_HAL_PlatformType;

/* -----------------------------------------------------------------------
**                        VARIABLE DECLARATIONS
** ----------------------------------------------------------------------- */

/*
 * This object contains addresses of various vMPM configuration register
 * for the master. Various fields of this objects must be initialized before
 * using any mpm functionalities. 
 *
 * @see vmpm_HAL_InitializeRegisterLayout
 */
extern vmpm_HAL_PlatformType registerLayout;

/* -----------------------------------------------------------------------
**                        FUNCTION DECLARATIONS
** ----------------------------------------------------------------------- */

/**
 * Intializes the register layour for writing to shared memory. Its 
 * implementation will vary based on OS. For Apps (QNX or EA) we will
 * need virtual address for the base of shared memory address for mpm.
 *
 * @param master_msg_ram_base_ptr: Address of the memory region for this
 *                                 master reserved in shared message ram.
 *                                 It is determined by RPM team. Address
 *                                 can be virtual or physical based on
 *                                 target.
 */
void vmpm_HAL_InitializeRegisterLayout ( uint8 *master_msg_ram_base_ptr );

/**
 * Prepares mpm driver to perform IPC with RPM afterwards. This is just
 * a setup function and it does not perform any actual IPC. 
 *
 * @Note
 * This OS dependent function. For example,
 *  - in case where we use DAL IPC, it would be attaching to DAL IPC device.
 *  - On some target this function will map the addresses of IPC registers 
 *    for the process they are running in.
 */
void vmpm_HAL_SetupIPC( void );

/**
 * Sends an IPC interrupt to RPM whenver vMPM configuration for this
 * master has been changed. This happens when any interrupt/gpio 
 * configuration (MPM2 supported trigger type) is changed in DAL
 * which changes configuration in HAL (detection, polarity).
 *
 * @Note:
 * This function may have different implementation on different OSes.
 * - The default implementation is using DAL IPC interrupt interface.
 * - Alternative one would be to write directly to IPC registers.
 */
void vmpm_send_interrupt( void );

#endif /* VMPM_HAL_INTERNAL_H */

