#ifndef MMAL_CIQ_PL_H
#define MMAL_CIQ_PL_H
/*!
  @file mmal_ciq_pl.h

  @brief
   The messages sent to the mmal task.

  @ingroup mmal CIQ Porting Layer Header file
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/inc/mmal_ciq_pl.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/03/11   ars     Fix for CR 286318. Moved IQBackend_Initialize to porting 
                   layer
03/25/11   ars     Initial checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "mcs_variation.h"
#include <mmal_cfg.h>
#ifdef FEATURE_MMAL_CIQ
#include <dsm_pool.h>
#include <sys.h>

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/



/*=============================================================================

  FUNCTION:  mmal_ciq_pl_init

=============================================================================*/
/*!
    @brief
    Init routine of this file. This is called during MMAL task init and 
    it does the initialization of timers.

*/
/*===========================================================================*/
boolean mmal_ciq_pl_init( void );

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_process_sio_rx_cb_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_PL_SIO_RX_CB_REQ message.
    It sends notification to IQ Agent that data is waiting to be read.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_sio_rx_cb_req 
( 
  dsm_item_type*   dsm_item_ptr
);

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_process_bkend_timer_exp_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_BKEND_EXECUTE_TIMER_EXP_REQ message.
    It invokes IQBackend_Execute and restarts the timer.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_bkend_timer_exp_req ( void );

/*=============================================================================

  FUNCTION:  IQPorting_SerialWrite

=============================================================================*/
/*!
    @brief
    This function calls IQBackend_Execute to give the IQ Agent a chance to run
    When finished running, a timer value will be provided and the IQ Agent 
    expects another call to IQBackend_Execute upon timer expiry
*/
/*===========================================================================*/
void mmal_ciq_pl_invoke_bkend_exec( void );

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_process_time_zone_chg_req

=============================================================================*/
/*!
    @brief
    This function processes the MMAL_CIQ_PL_TIME_ZONE_CHG_REQ message.
    It updates the locally stored time zone. Note that CIQ states to exclude 
    any daylight saving offset in time zone. Offset should always correspond 
    to Standard time.

*/
/*===========================================================================*/
void mmal_ciq_pl_process_time_zone_chg_req
(
  int                             time_zone_offset,
  boolean                         daylight_saving_adj_avail,
  sys_daylight_saving_adj_e_type  daylight_saving_adj
);

/*=============================================================================

  FUNCTION:  mmal_ciq_pl_get_iq_agent_init_flag

=============================================================================*/
/*!
    @brief
    This function is used to get the iq agent init flag

   @return		True: If Init was successful
                False: Otherwise.
*/
/*===========================================================================*/
boolean mmal_ciq_pl_get_iq_agent_init_flag ( void );
#endif /* FEATURE_MMAL_CIQ */
#endif /* MMAL_CIQ_PL_H */
