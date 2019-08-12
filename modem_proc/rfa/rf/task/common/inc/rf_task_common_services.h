#ifndef RF_TASK_CMN_SERVICES_H
#define RF_TASK_CMN_SERVICES_H
/*!
  @file
  RF Task header file.

  @defgroup RF_TASK_header
  @ingroup RF_TASK_CMN_SERVICES_H
  @{
*/
/*===========================================================================

                 R F   T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with RF task module.

REFERENCES
  None

Copyright (c) 2009 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/common/inc/rf_task_common_services.h#1 $
   
when         who     what, where, why
----------   ---     ------------------------------------------------------- 
03/04/14    sw       Remove GFW MSGR framework as its now replaced with RFLM MSGR
01/30/14    sw       Introduce framework for receiving RFLM msgs (therm read)
05/30/13    tws      Move the RF task num enum to rfcom.h
01/31/13    tsr      Added framework for recieving therm read response from GFW 
10/03/12    av       Initial revision
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "rfcom.h"
#include "comdef.h"
#include "msgr_types.h"
#include "dog.h"

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/



#define RF_CMD_Q_SIG          0x0001    /*!< Signal for when commands available 
                                             in  RF command queue. */

#define RF_RPT_TIMER_SIG      0x0002    /*!<  Signal for RF task watchdog. */

#define RF_NV_CMD_SIG         0x0004    /*!< Signal for RF NV CMD Reads.   */

/*!<  Signal to indicate ftm_wcdma enter/exit complete  */
#define FTM_WCDMA_ENTER_EXIT_COMPLETE_SIG    0x0008

#define RF_RFLM_CMD_Q_SIG      0x0010   /*!< Signal for RFLM -> RFSW CMDs   */

typedef struct 
{
    boolean is_rfa_task_running;
    /*< status of the rf task*/
    rex_tcb_type   *tcb_ptr;
    /*< tcb ptr*/
    rex_timer_type rf_rpt_timer; 
    rex_timer_type rf_wait_timer;
    /*<Timers used for watchdog reporting and generic purposes*/
    dog_report_type   rf_task_dog_rpt_var;      /* Initial Safety */
    uint32            rf_task_dog_rpt_time_var; /* Initial Safety */


}rf_task_data_type;

boolean rfa_is_task_running(rf_task_num_type task_num);

void rfa_set_task_status(rf_task_num_type task_num, boolean active);

void rf_task_initialize_data(rf_task_num_type task_num);

void rf_task_kick_watchdog( rf_task_num_type task_num );

void rf_task_init_watchdog( rf_task_num_type task_num );

void rf_task_deinit_watchdog( rf_task_num_type task_num );

rex_timer_type* rfa_get_watchdog_timer(rf_task_num_type task_num);

rex_timer_type* rfa_get_wait_timer(rf_task_num_type task_num);

#endif
