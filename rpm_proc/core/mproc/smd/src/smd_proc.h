#ifndef SMD_PROC_H
#define SMD_PROC_H

/*---------------------------------------------------------------------------
            Shared Memory Driver Processor-Specific Definitions
---------------------------------------------------------------------------*/
/*!
  @file
    smd_proc.h

  @brief
    This file contains the SMD processor-specific definitions.
*/

/*--------------------------------------------------------------------------
     Copyright  2012-2013 Qualcomm Technologies Incorporated.
     All rights reserved.
---------------------------------------------------------------------------*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smd_proc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/13   rv      MSM8916 RPM needs CSR register configuration to access DDR.
                   Corresponding access configuration macros are added.
01/28/13   bt      Add TZ interrupt definitions.
10/03/12   bt      Several fixes for compiler problems, updating to tip.
08/17/12   bt      Add SMEM access macros, from smd_os.h.
05/24/12   bm      Use smem_host_type to identify hosts
07/17/12   bt      Fixed typo breaking ADSP->RPM intr definition.
04/27/12   bm      Created processor-specific file for B-family RPM.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_v.h"
#include "DDIIPCInt.h"
#include "HALhwio.h"
#include "rpm_hwio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/
/** \name SMD Host
 *
 * This macro defines the local processor in which SMD runs.
 *
 * @{
 */

#define SMD_THIS_HOST   ( SMEM_RPM )
#define SMD_NUM_CHANNEL_INFO 5
#define SMD_RPM_PROC
/*@}*/

/** RPM's lock type does not matter, they are all INTLOCKs. */
#define SMD_EVENT_MUTEX_TYPE DALSYS_SYNC_ATTR_RESOURCE

/*---------------------------------------------------------------------------
  Interrupt Defines
---------------------------------------------------------------------------*/
/** NULL Interrupt definitions */
#define SMD_NULL_IN_INTERRUPT  ( DALIPCINT_INT_32BITS )
#define SMD_NULL_OUT_INTERRUPT ( (DalIPCIntInterruptType)SMD_NULL_IN_INTERRUPT)


/** \name SMD Target RPM
 *
 * These values represent the target specific IRQs and ISRs supported.
 *
 * @{
 */
/* RPM<-Apps incoming  rpm_interrupts[8];kpss_to_rpm_ipc0 */
#define SMD_APPS_IN           8  /*TRAMP_APSS_CPU0_GP_HIGH_IRQ*/
#define SMD_APPS_OUT          DALIPCINT_GP_HIGH /* rpm_ipc[4] - RPM_IPC */
#define SMD_APPS_IPCPROC      DALIPCINT_PROC_ACPU

/* RPM<-MPSS incoming rpm_interrupts[26]; mss_sw_to_rpm_ipc_irq0 */
#define SMD_MPSS_IN           26  /*mss_sw_to_rpm_ipc_irq0*/
#define SMD_MPSS_OUT          DALIPCINT_GP_HIGH /* rpm_ipc[12] - RPM_IPC */
#define SMD_MPSS_OUT_M        0 /* Not used with DAL. */
#define SMD_MPSS_IPCPROC      DALIPCINT_PROC_MDSPSW

/* RPM<-ADSP incoming rpm_interrupts[56]; lpass_to_rpmss_ipc_irq0 */
#define SMD_ADSP_IN           56  /*TRAMP_LPASS_GP_HIGH_IRQ*/
#define SMD_ADSP_OUT          DALIPCINT_GP_HIGH /* rpm_ipc[8] - RPM_IPC */
#define SMD_ADSP_OUT_M        0 /* Not used with DAL. */
#define SMD_ADSP_IPCPROC      DALIPCINT_PROC_ADSP

/* RPM<-WCNSS incoming rpm_interrupts[34]; RivaRpmGpHighIrq */
#define SMD_WCNSS_IN          34  /*TRAMP_RIVA_GP_HIGH_IRQ*/
#define SMD_WCNSS_OUT         DALIPCINT_GP_HIGH /* rpm_ipc[16] - RPM_IPC */
#define SMD_WCNSS_OUT_M       0 /* Not used with DAL. */
#define SMD_WCNSS_IPCPROC     DALIPCINT_PROC_WCN

/* RPM<-Self ; NULL */
#define SMD_RPM_IN            SMD_NULL_IN_INTERRUPT
#define SMD_RPM_OUT           SMD_NULL_OUT_INTERRUPT
#define SMD_RPM_OUT_M         0
#define SMD_RPM_IPCPROC       DALIPCINT_PROC_NULL

/* RPM<-TZ incoming rpm_interrupts[11]; APCS_rpmTzIPCInterrupt[0] */
#define SMD_TZ_IN             11  /* APCS_rpmTzIPCInterrupt[0] */
#define SMD_TZ_OUT            DALIPCINT_GP_HIGH /* rpm_ipc[24] - RPM_IPC */
#define SMD_TZ_OUT_M          0 /* Not used with DAL. */
#define SMD_TZ_IPCPROC        DALIPCINT_PROC_TZ
/*@}*/

/* -------------------------------------------------------------
   Shared memory access macros for RPM
   ------------------------------------------------------------- */
#define SMD_READ_SMEM_1( addr ) ( *( ( volatile uint8* )( addr ) ) )

#define SMD_READ_SMEM_2( addr ) ( *( ( volatile uint16* )( addr ) ) )

#define SMD_READ_SMEM_4( addr ) ( *( ( volatile uint32* )( addr ) ) )

#define SMD_WRITE_SMEM_1( addr, val ) \
  ( *( ( volatile uint8 *)( addr ) )  = ( uint8 )( val ) )

#define SMD_WRITE_SMEM_2( addr, val ) \
  ( *( ( volatile uint16 *)( addr ) ) = ( uint16 )( val ) )

#define SMD_WRITE_SMEM_4( addr, val ) \
  ( *( ( volatile uint32 *)( addr ) ) = ( uint32 )( val ) )

#define SMD_MEMSET_SMEM( addr, val, size )     memset( addr, val, size )

#define SMD_MEMCPY_FROM_SMEM( dst, src, size ) memcpy( dst, src, size )

#define SMD_MEMCPY_TO_SMEM( dst, src, size )   memcpy( dst, src, size )

#define SMD_MEMCPY_FROM_TO_SMEM( dst, src, size ) memcpy( dst, src, size )

/* Ensure the appropriate data copy implementation is used for copies in and
 * out of the SMD channels.  When the buffer resides in SMEM, the native
 * memcpy can be used.  For buffers in RPM MSG RAM, word accesses are strictly
 * required, but some implementations of memcpy do byte accesses under certain
 * conditions, even if src and dest are word-aligned.
 *
 * ARM memcpy is fine on all edges, so don't bother using info->copy_fn.
 */
#define SMD_GET_SMEM_COPY_FUNC(channel_type)   memcpy

/*-------------------------------------------------------------
   Shared and Ext Shared RAM access configuration
 ------------------------------------------------------------- */
/** RPM page select configuration needed to get DDR shared memory access
 *  CSR:0x02 <- DDR shared memory
 *  CSR:0x00 <- RPM message RAM */
#define SMD_ACCESS_CONFIGURE_SHARED_RAM()       HWIO_RPM_PAGE_SELECT_OUT(0x2)
#define SMD_ACCESS_CONFIGURE_EXT_SHARED_RAM()   HWIO_RPM_PAGE_SELECT_OUT(0x0)

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/
/** OS abstraction over DAL types */
typedef DalIPCIntProcessorType smd_os_proc_type;
typedef DalIPCIntInterruptType smd_os_intr_type;

#ifdef __cplusplus
}
#endif

#endif  /* SMD_PROC_H */
