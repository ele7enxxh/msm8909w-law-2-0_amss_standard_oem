#ifndef TCXOMGR_MSGR_H
#define TCXOMGR_MSGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                           T C X O    M A N A G E R

                           TCXO Manager Header File

GENERAL DESCRIPTION

  This module maintains the internal definitions for the txcomgr subsystem

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_msgr.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     ---------------------------------------------------------
01/30/15     tl      Remove obsolete featurization
07/28/14     tl      Clean up resources when shutting down Tcxomgr
05/27/14     tl      Remove support for older modems
01/21/14     tl      Added LTE FW messages for XO cal
06/15/12     gs      Genericized init, recv, created register function
02/10/12     sr      TCXOMGR Feature Cleanup
10/04/11     ag      Initial version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"

#define MSGR_NOLIMIT -1

#include "tcxomgr.h"
#include "msg.h"
#include "tcxomgr_cal.h"
#include "tcxomgr_cal_i.h"
#include "mmoc_msgr.h"

#ifdef FEATURE_TDSCDMA
#include "tfw_sw_intf_rx_tx.h"
#include "tfw_sw_intf_msg.h"
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_CDMA
#include "cdmafw_msg.h"
#include "cfw_rf_intf.h"
#include "tcxomgr_msgr.h"
#endif /* FEATURE_CDMA  */

#ifdef FEATURE_LTE
#include "intf_sys.h"
#include "intf_dl.h"
#endif /* FEATURE_LTE */

#include "msgr.h"
#include "msgr_rex.h"


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/* Define the different clients for the tcxomgr_msgr interface */
typedef enum
{
  TCXOMGR_MSGR_TDSCDMA_CAL,
  TCXOMGR_MSGR_LTE_CAL,
  TCXOMGR_MSGR_TASK,
  TCXOMGR_MSGR_CDMA_CAL,
  TCXOMGR_MSGR_MAX
} tcxomgr_msgr_client;

/* Messsage router payload */
typedef union
{
  /* empty cmd */
  msgr_hdr_struct_type               hdr;

#ifdef FEATURE_TDSCDMA
  /* IQ capture cmd type */
  tfw_iq_capture_cmd_t               iqCaptureCmd;

  /* IQ capture response type */
  tfw_iq_capture_rsp_t               iqCaptureRsp;
#endif

  /* Offline indication msg */
  mmoc_offline_ind_s_type            offline_ind;

#ifdef FEATURE_LTE
  lte_LL1_sys_mempool_buf_details_req_msg_struct lteIqMempoolCmd;
    
  lte_LL1_sys_mempool_buf_details_cnf_msg_struct lteIqMempoolRsp;
#endif 

#ifdef FEATURE_CDMA
  cfw_iq_capture_msg_t cdmaIqCaptureCmd;
  
  cfw_iq_capture_rsp_msg_t cdmaIqCaptureRsp;  
  
#endif 

#ifdef FEATURE_LTE
  lte_LL1_dl_iq_sample_capture_req_msg_struct lteIqCaptureCmd;
  lte_LL1_dl_iq_sample_capture_cnf_msg_struct lteIqCaptureRsp;
#endif

} tcxomgr_msgr_msgs_u;

/* Message router queue item type */
typedef struct
{
  /* Item type for tcxomgr msgr queue */
  q_link_type                         qlink;

  /* The messsage payload */
  tcxomgr_msgr_msgs_u                 cmds;
  
} tcxomgr_msgr_msgs_t;

/* function pointer for recv msg handling callback function for
 * tcxomgr client technologies. */
typedef boolean (* tcxomgr_recv_msg_cb)
(
  tcxomgr_msgr_msgs_t *queue_ptr, /* pointer to the queued message */
  tcxomgr_msgr_msgs_u *recvd_msg /* buf for user to retrieve message contents */
);

/* Message router initialization structure type */
typedef struct
{

  /* Msgr Client handle    */
  msgr_client_t client;

  /* Msgr client queue     */
  q_type        cmd_q;

  /* Msgr queue ID         */
  msgr_id_t     queue_id;

  /* Is Msgr initialized ? */
  boolean       initialized;

  /* callback pointer for recv_msg */
  tcxomgr_recv_msg_cb msg_cb;

  /* msgr signal mask for this client */
  rex_sigs_type sigs;

} tcxomgr_msgr_type;

/*============================================================================

                   FUNCTIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*============================================================================

FUNCTION tcxomgr_msgr_init

DESCRIPTION
  This function initializes the msgr interface for a tcxomgr client

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the initialization is successful, FALSE otherwise.

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_msgr_init
( 
  rex_sigs_type sigs,                 /*!< Signal to indicate FW response  */
  tcxomgr_msgr_client client_type,    /* Client type for tcxomgr msgr */
  tcxomgr_recv_msg_cb client_msg_cb  /* Callback function for recvd message */
);
/*============================================================================

FUNCTION tcxomgr_msgr_term

DESCRIPTION
  This function terminates the msgr interface for a tcxomgr client

DEPENDENCIES
  msgr interface should have been initialized using tcxomgr_msgr_init()

RETURN VALUE
  Boolean - TRUE if the termination is successful, FALSE otherwise.

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_msgr_term
(
  tcxomgr_msgr_client client_type    /* Client type for tcxomgr msgr */
);
/*============================================================================

FUNCTION tcxomgr_register_msgs

DESCRIPTION
  This function registers a client to recieve messages

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the initialization is successful, FALSE otherwise.

SIDE EFFECTS
  None

============================================================================*/

boolean tcxomgr_register_msgs
(
  tcxomgr_msgr_client client_type, /* client to register the message with */
  msgr_umid_type umid /* UMID to register */
);

/*============================================================================

FUNCTION tcxomgr_msgr_send_msg

DESCRIPTION
  This function sends a message through msgr.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
errno_enum_type tcxomgr_msgr_send_msg
(
  msgr_hdr_struct_type *msg_hdr_ptr,  /*!< Pointer to MSG Header  */
  msgr_umid_type        umid,         /*!< UMID of the message    */
  int32                 size          /*!< Size of the message    */
);

/*============================================================================

FUNCTION tcxomgr_msgr_recv_msgs

DESCRIPTION
  This function receives messages through msgr.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
boolean tcxomgr_msgr_recv_msgs
(
  tcxomgr_msgr_client  client_type,
  tcxomgr_msgr_msgs_u* recv_msg,
  int                  dequeue_lim
);

#endif /* TCXOMGR_MSGR_H */
