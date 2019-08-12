#ifndef MODEM_XPU_INT_H
#define MODEM_XPU_INT_H

/**
* @file modem_xpu_int.h
* @brief Modem XPU Interrupt Handler
*
* This file implements the XPU interrupt Handling. All XPU violations are reported
* over DIAG (Subsystem QDSP6) and to a ring buffer referenced by 'modem_sec_xpu_buffer'.
*
*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/modem_sec/inc/modem_xpu_int.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
01/04/2013 rs       Initial version
===========================================================================*/
#include <comdef.h>

/**
 * @brief      Configure XPU Violation Interrupts and start IST
 *             
 *
 * @param[in]  none
 * @param[out] none
 *
 * @return     none
 */
void modem_sec_config_xpu_error_reporting (void);

/**
 * @brief      Initialize QURT Interrupt Service Thread 
 *             
 *
 * @param[in]  none
 * @param[out] none
 *
 * @return     none
 */
void modem_sec_xpu_IST_init (void);

/**
 * @brief      Thread that registers XPU Violation interrupts 
 *             
 *
 * @param[in]  none
 * @param[out] none
 *
 * @return     none
 */
void modem_sec_xpu_IST (void *arg);

/**
 * @brief      Interrupt Service Routine for XPU Violation 
 *             
 *
 * @param[in]  none
 * @param[out] none
 *
 * @return     none
 */
void modem_sec_xpu_isr(void);

#endif
