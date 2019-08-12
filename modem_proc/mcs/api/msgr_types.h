/*!
  @file   msgr_types.h

  @brief  Message Router header file

  @detail
  Defines Message Router related types and interfaces.

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/msgr_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/11   ckk     Add 2nd port support for xproc messaging
12/22/10   awj     Update header file inclusions.
12/13/10   ejv     Update header file inclusions.
12/08/10   zm      Feature cleanup - SU API effort.
11/09/10   zm      Moved file into mcs/api folder - SU API effort.
07/02/10   ejv     Some qube cleanup.
04/02/10   ck      Add MSGR_NO_QUEUE support for legacy REX clients
06/04/09   ejv     Add "proc" field to msgr_hdr_s.
03/12/09   ejv     Updates to MSGR for supporting SMD-Lite.
11/21/08   ejv     Move attachment types & defines into msgr_attach.h.
11/14/08   ejv     Featurize DSM code.
11/14/08   ejv     Remove MSGR_MAX_MSG_SIZE.
11/05/08   ejv     Add MSGR_NO_OFFSET.
10/31/08   ejv     Add FEATURE_MSGR_REX.
10/29/08   awj     Merged in from dev/posix/
10/14/08   ejv     Move client details internal to MSGR.
07/23/08   ejv     Port CS to new OSI MSGR APIs.
05/16/08   ejv     Update for QDSP6 beta release.
03/31/08   djm     Add support for OSI operating system independent msgr funcs
01/25/08   sah     Add support for CS message router
01/14/08   enj     Cleanup msgr/msgr_qube split
01/14/08   enj     Initial Revision

===========================================================================*/

#ifndef MSGR_TYPES_H
#define MSGR_TYPES_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>    /* Definition of basic types and macros */
#include <IxErrno.h>
#include "msgr_umid.h"
#include <smd_lite.h>       /* SMD Port ID */

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef int                     msgr_fifo_handle_t;
typedef int                     msgr_client_t;
#define MSGR_MQ_NO_Q            (mqd_t)-1
#define MSGR_MQ_MAX_NAME_SIZE   16

/*! @brief MSGR ID is used to identify the mq being that is being referenced */
typedef int                     msgr_id_t;
#define MSGR_ID_NONE            (msgr_id_t)0
#define MSGR_ID_REX             MSGR_ID_NONE

/*! @brief "NO OFFSET" is used to indicate that the offset to the REX
    cmd_type in the msg data structure does not need to be set by MSGR */
#define MSGR_NO_OFFSET          (uint16)-1

/*! @brief "NO QUEUE" is passed to msgr_client_set/add_rex_q in the free_msg_queue
    param to indicate that MSGR should allocate message buffers from the heap
    rather than a specified free queue */
#define MSGR_NO_QUEUE           (q_type *)-1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef uint8   msgr_proc_t;

typedef struct {
  msgr_proc_t       proc;
  const char *      name;
  uint32            unused1;
  uint32            unused2;
} msgr_init_entry_s;

typedef enum {
  SMDL_PORT_UNKNOWN = 0x0,
  SMDL_PORT_CLOSED  = 0x1,
  SMDL_PORT_OPEN    = 0x2
} msgr_smd_port_e;

typedef struct {
  msgr_proc_t       proc1;
  msgr_proc_t       proc2;
  const char *      name;       /*!< Size dictated by SMD-Lite... limited to 20 chars */
  smdl_handle_type  smd_port_id;
  msgr_smd_port_e   status;
  void *            rcv_buf;    /*!< Buffer for receiving from SMDLite */
  smdl_callback_t   service_cb;
} msgr_init_smd_entry_s;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef uint8            msgr_priority_t;
typedef msgr_priority_t  msgr_priority_type;

#define MSGR_PRIORITY_LOW   0x00
#define MSGR_PRIORITY_HIGH  0x7F

/* Default variant for messages */
#define MSGR_NO_VARIANT 0

/* Variant field is 3 bits wide so the allowed variants are 0-7 */
#define MSGR_MAX_VARIANT 7

/* Number of variants supported in this build */
#ifdef FEATURE_MSGR_VARIANTS
#define MSGR_NUM_VARIANT 8
#else
#define MSGR_NUM_VARIANT 1
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for message header.
*/
#ifdef __GNUC__
typedef struct ALIGN(8)
#else
typedef struct
#endif // __GNUC__
{
  msgr_umid_type              id;           /*!< Unique Message ID (UMID) */
  msgr_tech_module_type       src;          /*!< Sending module */
  msgr_proc_t                 proc;         /*!< Sending proc */
  uint8                       num_attach;   /*!< Number of attachments */
  msgr_priority_type          priority;     /*!< Priority of the message */
  uint8                       inst_id;      /*!< Generic unsigned user data field.
                                                 Appropriate conversion to be 
                                                 performed by user to pass data 
                                                 like sys_modem_as_id_e_type 
                                                 inside this field */
  uint8                       options_mask; /*!< Bit field to set optional params */
} msgr_hdr_s;
typedef msgr_hdr_s  msgr_hdr_struct_type;

/*! @brief Optional params in the header */
typedef enum {
  NO_OPT = 0x0,
  NO_SIG  = 0x1    // Do not signal after send
} msgr_hdr_opts_e;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The following supervisory messages should be defined by every component using
 * the MSGR_UMID macro.
 * e.g. MSGR_UMID( MSGR_UMB_SRCH, MSGR_TYPE_SPR, MSGR_ID_LOOPBACK )
 */

/*! @brief Supervisory Loopback Message */
#define MSGR_ID_LOOPBACK           0x00

/*! @brief Supervisory Loopback Reply */
#define MSGR_ID_LOOPBACK_REPLY     0x01

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for supervisory loopback command.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type   hdr;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Message payload */
  
  /*! Reply message header.  Used internally to reply to the loopback.  The
   *  payload in this message overlays exactly with the loopback reply below.
   *  As such, it is used as the reply message.
   */
  msgr_hdr_struct_type   reply_hdr;                                         
  /*! Token to be included in reply */
  unsigned int           reply_token;
  
} msgr_spr_loopback_s;
typedef msgr_spr_loopback_s  msgr_spr_loopback_struct_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*! @brief Type for loopback reply.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type   hdr;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*! Message payload - Reply token received in loopback command */
  unsigned int           reply_token;

} msgr_spr_loopback_reply_s;
typedef msgr_spr_loopback_reply_s  msgr_spr_loopback_reply_struct_type;

/*! @brief Type for callback for msgr_send notifications
 */
typedef void (*msgr_send_cb_t)(msgr_tech_module_type,
                               msgr_tech_module_type,
                               msgr_hdr_struct_type*);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* MSGR_TYPES_H */
