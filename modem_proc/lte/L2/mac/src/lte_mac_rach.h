/*!
  @file
  lte_mac_rach.h

  @brief
  This file contains all the define structures/function prototypes 
  for Random Access Procedure.


*/

/*===========================================================================

  Copyright (c) 2009 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_rach.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/12/15   el      CR 884865: FR 29101 Fast RLF Declaration for fast resync with eNB
12/09/14   pw      CR749213: Handle DTA
08/03/14   mg      CR702710: QSH analysis and reset
10/07/13   ta      CR 529514: DSDS feature. Support for RF Available/Unavailable
                   message
04/30/13   st      CR 454099: FR 2999 RACH stats reporting to RRC
04/10/11   st      CR 305227: Fixed a HARQ retx bug
12/27/11   st      CR #327536: Fix for race condition leading to an unwanted 
                   re-rach(MSG4 arrives just before we abort RACH due to recfg)
09/01/11   bn      CR 303654:Only process RAR when in RACH MSG2 state
12/19/10   sm      Extend the CT timer to 5ms from 3ms
11/23/10   bn      Extend the CT timer for 3 ms to account for delay in ML1
06/09/10   bn      Changed the RC from uint16 to int16
03/29/10   bn      Added in function to send abort RACH CNF
11/06/09   bn      Moved LTE_MAC_RACH_PDU_HEADER_MAX_LENGTH to lte_l2_common.h
08/07/09   bn      Fixed Mar09 spec. CR 091679
05/15/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_RACH_H
#define LTE_MAC_RACH_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include "lte_mac_log.h"
#include "lte_mac_ul.h"
#include "qsh.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief defined maximum number overload indicator index */
#define LTE_MAC_RACH_MAX_NUM_OI_INDEX   16

/*! @brief defined Extend CT timer for 5 ms to account for delay in ML1*/
#define LTE_MAC_RACH_CONTENTION_TIMER_DELAY_IN_MS   5

/*! @brief defined MAC RACH state emumerations */
typedef enum
{
  LTE_MAC_RACH_INIT_STATE,
  LTE_MAC_RACH_SEND_PREAMBLE_STATE,
  LTE_MAC_RACH_PREAMBLE_RESP_STATE,
  LTE_MAC_RACH_SEND_MSG3_STATE,
  LTE_MAC_RACH_CONTENTION_RESOLUTION_STATE,
  LTE_MAC_RACH_BACKOFF_WAIT_STATE,
  LTE_MAC_RACH_ABORT_STATE
} lte_mac_rach_state_e;

/*! @brief defined MAC RACH procedure status emumerations */
typedef enum
{
  /*! currently doing RACH */
  LTE_MAC_RACH_IN_PROGRESS,
  /*! abort due to RRC request*/
  LTE_MAC_RACH_ABORT,
  /*! RACH success and done*/
  LTE_MAC_RACH_SUCCESS,
  /*! MAC internally stopping RACH */
  LTE_MAC_RACH_STOP
} lte_mac_rach_status_e;

/*! @brief Global structure for RACH statistics
*/
typedef struct
{
  uint8    num_preambles_sent;
  uint8    num_contentions;
  /*! The last succeed rach procedure preamble sent count*/
  uint8    last_num_preambles_sent;
    /*! The last failure rach procedure preamble sent count*/
  uint8   last_failed_num_preambles_sent;
  uint32   num_rach_sent; /*Total number of rach preambles including retry sent*/
  uint32   num_rach_retry_sent; /*Number of retry rach preambles*/
  uint32   num_rach_fails_rep; /*Number of rach failure indications sent to ML1*/
} lte_mac_rach_stats_s;

/*! @brief Global structure for RACH procedure 
*/
typedef struct
{
  /*! internal access procedure state */
  lte_mac_rach_state_e state;
  /*! access reason */
  lte_mac_access_reason_e  access_reason;
  /*! current access attempt number */
  uint16               attempt_number;
  /*! RACH sequence number, increase by one when the 
  attempt number reach the maximum */
  uint8                sequence_number;
  /*! contention based or contention free */
  boolean              contention_based_rach;
  /*! Current used RA index, it can be computed 
  or from configured RA index */
  uint8                current_raid;
  /*!  RA index mask */
  uint8                raid_mask;
  /*! contention free RA index. This is configured */
  uint8                contention_free_raid;
  /*! actual lenght of MSG3 */
  uint16               msg3_length;
  /*! local buffer pointer to store MSG3 */
  uint8                *msg3_buf_ptr;
  /*! in HO RACH case, MSG3 needs to store for 
  subsequence usage */
  boolean              request_duplication_msg3;
  /*! current power level in dB */
  int16                power_level;
  /* RA group, 0 or 1 */
  uint8                ra_group;
  /*! backoff duration in ms from MSG2*/
  uint16               backoff_duration;
  /*! current backoff duration in ms. Randomize value between 0->backoff_duration */
  uint16               curr_backoff_duration;
  /*! TEMP RNTI  received in RAR response */
  uint16               temp_rnti;
  /*! the UE C-RNTI */
  uint16               c_rnti;   
  /*! current RACH configuration pointer*/
  lte_mac_rach_cfg_s   *cfg_ptr;
  /*! RACH procedure status */
  lte_mac_rach_status_e rach_status;
  /*! estimated MAC PDU length */
  uint16  estimated_macpdu_length;
  /*! buffer pointer to build MAC PDU header */
  uint8   *pdu_header_ptr;
  /*! MAC PDU header in bits */
  uint16   pdu_header_in_bits;
  /*! radio condition */
  int16                 rc; 
  /*! The system frame number */
  lte_sfn_s             sfn;   
  /*! Grant size is sufficient or not */
  boolean  invalid_grant;
  /*! Pointer to global MAC Log structure */
  lte_mac_log_s*      log;     
  lte_mac_rach_stats_s stats;
  /*! SCellIndex(0 .. 7 ). Value 0 is for Pcell */
  uint8 scell_id;
  /*! Current RACH shift factor */
  uint8 shift_factor;
} lte_mac_rach_s;
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_mac_rach_init

===========================================================================*/
/*!
    @brief
    This function initializes the RACH variables during power up.

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_init
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_procedure_init

===========================================================================*/
/*!
    @brief
    This function initializes the RACH procedure.

    @detail
    - Store all the required parameters from the REQ command to local data
    - Decide if contention based or contention free
    - Prepare a local copy of MSG3 if needed 
    - Reset all the state variable related to RACH procedure

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_procedure_init
(
  lte_mac_intmsg_random_access_req_s* msg /*!< input message pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_main_handler

===========================================================================*/
/*!
    @brief
    This function is the main handler the RACH procedure.

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern lte_mac_rach_status_e lte_mac_rach_main_handler
(
  lte_mac_ul_msg_u* msg /*!< input message pointer */
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_reset

===========================================================================*/
/*!
    @brief
    This function resets the RACH variables in case of connection release,
    radio failure or HO

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_reset
(
  lte_mac_inform_action_e   inform_action 
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_deinit

===========================================================================*/
/*!
    @brief
    This function de-initializes the RACH variables during shutdown.

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_deinit
(
  void 
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_send_internal_abort_cnf

===========================================================================*/
/*!
    @brief
    This is function is send internal RACH Abort Confirmation

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_send_internal_abort_cnf
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_in_msg2_state

===========================================================================*/
/*!
    @brief
    This function indicates if MAC RACH is in MSG2 state

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern boolean lte_mac_rach_in_msg2_state
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_is_rach_success

===========================================================================*/
/*!
    @brief
    This function returns a pointer to the internal state structure of the
    lte_mac_rach module

    @detail

    @return
    Pointer to the lte_mac_rach structure

    @see related_function()

*/
/*=========================================================================*/
extern boolean lte_mac_rach_is_rach_success
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_get_access_ptr

===========================================================================*/
/*!
    @brief
    This function return a pointer to the data structure of lte_mac_rach
    module.

    @detail

    @return
    None.

    @see related_function()

*/
/*=========================================================================*/
extern lte_mac_rach_s* lte_mac_rach_get_access_ptr
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_retry_preamble

===========================================================================*/
/*!
    @brief
    This function performs a retry PRACH whent the last PRACH is
    failed

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_retry_preamble
(
  void
);

/*===========================================================================

  FUNCTION:  lte_mac_rach_send_msg3_state_handler

===========================================================================*/
/*!
    @brief
    This function is the handler for the send message 3  state

    @detail

    @return

    @see related_function()

*/
/*=========================================================================*/
extern void lte_mac_rach_send_msg3_state_handler
(
  lte_mac_ul_msg_u* msg /*!< input message pointer */
); 

/*==============================================================================

  FUNCTION:  lte_mac_rach_qsh_analysis_routine

==============================================================================*/
/*!
    @brief
    This API runs the MAC UL RACH QSH analysis routine

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC UL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_rach_qsh_analysis_routine 
(
  void
);

/*==============================================================================

  FUNCTION:  lte_mac_rach_qsh_reset_stats

==============================================================================*/
/*!
    @brief
    This API resets MAC UL RACH stats

    @detail
    This API can run either in A2 task context during error fatal mode OR
    in L2 task (MAC UL task) context during Active mode 

    @return
    None

*/
/*============================================================================*/
extern void lte_mac_rach_qsh_reset_stats
(
  void
); 

/*==============================================================================

  FUNCTION:  lte_mac_rach_set_failed_preamble_cnt

==============================================================================*/
/*!
    @brief
    This API sets MAC UL RACH failed preamble count

    @detail
    This API is called in MAC control task when handling rach_abort_req
    
    @return
    None

*/
/*============================================================================*/
EXTERN void lte_mac_rach_set_failed_preamble_cnt
(
  boolean conneted_state
);

/*==============================================================================

  FUNCTION:  lte_mac_rach_get_access_reason

==============================================================================*/
/*!
    @brief
    This API return MAC rach access reason

    @detail
    None
    
    @return
    None

*/
/*============================================================================*/
EXTERN lte_mac_access_reason_e lte_mac_rach_get_access_reason
(
  void
);
#endif /* LTE_MAC_RACH_H */
