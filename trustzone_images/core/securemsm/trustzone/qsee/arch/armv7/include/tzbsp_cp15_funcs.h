#ifndef TZBSP_CP15_FUNCS_H
#define TZBSP_CP15_FUNCS_H

/**
@file tzbsp_cp15_funcs.h
@brief C functions for CP15 register read/write access.
*/

/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/include/tzbsp_cp15_funcs.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
08/12/12   tk       Initial version.
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
#define CNTP_CTL_ISTATUS  (1<<2) /* The status of the timer interrupt. */
#define CNTP_CTL_IMASK    (1<<1) /* Timer interrupt mask bit. */
#define CNTP_CTL_ENABLE   (1<<0) /* Enables the timer. */

uint32 rcp15_cntfrq(void);
uint64 rcp15_cntpct(void);
uint32 rcp15_cntkctl(void);
uint32 rcp15_cntp_tval(void);
uint32 rcp15_cntp_ctl(void);
uint32 rcp15_cntv_tval(void);
uint32 rcp15_cntv_ctl(void);
uint64 rcp15_cntvct(void);
uint64 rcp15_cntp_cval(void);
uint64 rcp15_cntv_cval(void);
uint64 rcp15_cntvoff(void);
/* Virtualized QTimer is not implemented.
uint32 rcp15_cnthctl(void);
uint32 rcp15_cnthp_tval(void);
uint32 rcp15_cnthp_ctl(void);
uint64 rcp15_cnthp_cval(void);
*/
uint32 rcp15_isr(void);

void wcp15_cntfrq(uint32 val);
void wcp15_cntkctl(uint32 val);
void wcp15_cntp_tval(uint32 val);
void wcp15_cntp_ctl(uint32 val);
void wcp15_cntv_tval(uint32 val);
void wcp15_cntv_ctl(uint32 val);
void wcp15_cntp_cval(uint64 val);
void wcp15_cntv_cval(uint64 val);
void wcp15_cntvoff(uint64 val);
/* Virtualized QTimer is not implemented.
void wcp15_cnthctl(uint32 val);
void wcp15_cnthp_tval(uint32 val);
void wcp15_cnthp_ctl(uint32 val);
void wcp15_cnthp_cval(uint64 val);
*/

#endif /* TZBSP_CP15_FUNCS_H */
