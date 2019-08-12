#ifndef _CFA_V_H_
#define _CFA_V_H_
/*===========================================================================
 
                    Callflow Analysis Logging Header File
 
DESCRIPTION
   This header file includes the appropriate enums for CFA logging.
   It can be shared by the TARGET (MSM) and the analyzer (QCAT).
 
Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
 
                      EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/cfa.h_v   1.3   10 May 2002 15:18:46   amcintos  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/inc/cfa_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/02   ATM     Added GSM ANALYZE interface functions
05/10/02   ATM     Added cfa_log_ex function 
04/10/02   ATM     Added Front-End Filter to CFA
11/21/01   ATM     Removed enums - using defines from generic services (gs.h)
08/21/01   ATM     Initial Revision
===========================================================================*/

#include "mmcp_variation.h"
#include "cfa.h"   /* byte */
  
/*******************************************************************************
 *
 *  Function name: cfa_log_packet
 *  ------------------------------
 *  Description:
 *  ------------
 *  Writes a Callflow Analysis Packet to the log file (if so configured).
 *  MSG ID is embedded in the IMH_T header of msg_ptr.
 *
 *  Parameters:
 *  -----------
 *     queue     -- queueid of recipient (enum in gs.h)
 *     length    -- size of message pointed to by msg_ptr
 *     msg_ptr   -- pointer to IMH_T based message
 *
 *  Returns:
 *  --------
 *     None.
 ******************************************************************************/
void cfa_log_packet( byte   queue,
                     word   length,
                     byte  *msg_ptr );

#ifdef FEATURE_GSM
/*******************************************************************************
 *
 *  Function name: cfa_log_ota_msg
 *  ------------------------------
 *  Description:
 *  ------------
 *    HELPER FUNCTION that governs whether message should issue
 *    EVENT_GSM_MESSAGE_SENT or EVENT_GSM_MESSAGE_RECEIVED
 *    based on the message ID and direction.
 *
 *  Parameters:
 *  -----------
 *     direction -- TRUE for uplink, FALSE for downlink
 *     channel   -- channel used (l2_channel_type_T)
 *     pd        -- octet containing pd
 *     msgid     -- octet containing m_id
 *
 *     NOTE: unused bits in pd/id will be masked to zero
 *           example if given pd=0x83 m_id=0x47
 *           pd will be masked with '00001111' to get 0x03 (PD_CC)
 *           id will be masked with '10111111' to get 0x07 (CONNECT_ACK)
 *
 *  Returns:
 *  --------
 *     None.
 ******************************************************************************/
void cfa_log_ota_msg        ( boolean isUplink, 
                              byte channel, 
                              byte pd, 
                              byte msgid );
/*******************************************************************************
 *
 *  Function name: cfa_log_timer_expired
 *  ------------------------------
 *  Description:
 *  ------------
 *      HELPER FUNCTION that governs whether message should issue
 *      EVENT_GSM_TIMER_EXPIRED based on the timer ID.
 *
 *  Parameters:
 *  -----------
 *     queue     -- queueid component of the id (enum in gs.h)
 *     timer     -- queue-specific component of timer
 *
 *  Returns:
 *  --------
 *     None.
 ******************************************************************************/
void cfa_log_timer_expired  ( byte queue, byte timer );

/*******************************************************************************
 *
 *  Function name: cfa_log_counter_expired
 *  ------------------------------
 *  Description:
 *  ------------
 *      HELPER FUNCTION that governs whether message should issue
 *      EVENT_GSM_COUNTER_EXPIRED based on the ID.
 *
 *  Parameters:
 *  -----------
 *     queue     -- queueid component of the id (enum in gs.h)
 *     counter   -- queue-specific component of counter
 *
 *  Returns:
 *  --------
 *     None.
 ******************************************************************************/
void cfa_log_counter_expired( byte queue, byte counter );
#endif /* FEATURE_GSM */


#endif /*ifndef _CFA_H_ */

