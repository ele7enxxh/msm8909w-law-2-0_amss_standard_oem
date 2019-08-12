/*!
  @file   msgr_rex.h

  @brief  REX specific message router header file

  @detail
  Defines REX-specific Message Router related types and interfaces.

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

===========================================================================*/

/*===========================================================================

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/msgr_rex.h#1 $

===========================================================================*/

#ifndef MSGR_REX_H
#define MSGR_REX_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "msgr.h"
#include "msgr_types.h"
#include <rex.h>      /* REX */
#include <queue.h>    /* Queue services */
#include <IxErrno.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

errno_enum_type msgr_client_set_rex_q
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  rex_tcb_type              *tcb,              //!< REX TCB pointer
  rex_sigs_type              sig,              //!< REX signal to set
  q_type                    *msg_queue,        //!< queue to place the msg
  q_type                    *free_msg_queue,   //!< queue to get empty buffer
  uint16                     msgr_hdr_offset,  //!< offset to msgr_hdr
  uint16                     cmd_type_offset,  //!< offset to legacy cmd_type
  uint16                     max_msg_size      //!< max size of msgs
);

errno_enum_type msgr_client_add_rex_q
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  rex_tcb_type              *tcb,              //!< REX TCB pointer
  rex_sigs_type              sig,              //!< REX signal to set
  q_type                    *msg_queue,        //!< queue to place the msg
  q_type                    *free_msg_queue,   //!< queue to get empty buffer
  uint16                     msgr_hdr_offset,  //!< offset to msgr_hdr
  uint16                     cmd_type_offset,  //!< offset to legacy cmd_type
  uint16                     max_msg_size,     //!< max size of msgs
  msgr_id_t                 *queue_id          //!< queue identifier
);

void msgr_init_client_rex_q
(
  q_type                    *msg_queue,        //!< queue to place the msg
  q_type                    *free_msg_queue    //!< queue to get empty buffer
);

errno_enum_type msgr_client_add_rex_q_ext
(
  msgr_client_t             *msgr_client_id,   //!< Message Client Handle
  rex_tcb_type              *tcb,              //!< REX TCB pointer
  rex_sigs_type              sig,              //!< REX signal to set
  q_type                    *msg_queue,        //!< queue to place the msg
  q_type                    *free_msg_queue,   //!< queue to get empty buffer
  uint16                     msgr_hdr_offset,  //!< offset to msgr_hdr
  uint16                     cmd_type_offset,  //!< offset to legacy cmd_type
  uint16                     max_msg_size,     //!< max size of msgs
  msgr_id_t                 *queue_id,         //!< queue identifier
  boolean                    destroy_queues    //!< Flag to indicate whether MSGR should 
                                               //!< destroy client queues upon client deletion  
);

void msgr_rex_free_msg_buf
(
  q_link_type               *rex_msg_link_ptr  //!< pointer to msg buffer
);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* MSGR_REX_H */
