#ifndef _DS_QMI_VOICE_CMD_LIST_H
#define _DS_QMI_VOICE_CMD_LIST_H
/*===========================================================================

                         D S _ Q M I _ V O I C E _ C M D _ L I S T . H

DESCRIPTION

 The Data Services Qualcomm Voice command list management header.


EXTERNALIZED FUNCTIONS



Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_cmd_list.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/25/09    sk     Initial revision.
===========================================================================*/

#include "IxErrno.h"
#include "qmi_voice_cm_if.h"




/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef enum
{
  QMI_VOICE_CMD_INDICATION_REGISTER        = 0x0003,
  QMI_VOICE_CMD_DIAL_CALL                  = 0x0020,
  QMI_VOICE_CMD_END_CALL                   = 0x0021,
  QMI_VOICE_CMD_ANSWER_CALL                = 0x0022,
  QMI_VOICE_CMD_CALL_STATUS_IND            = 0x0023,
  QMI_VOICE_CMD_GET_CALL_INFO              = 0x0024,
  QMI_VOICE_CMD_OTASP_STATUS               = 0x0025,
  QMI_VOICE_CMD_INFO_REC_IND               = 0x0026,
  QMI_VOICE_CMD_SEND_FLASH                 = 0x0027,
  QMI_VOICE_CMD_BURST_DTMF                 = 0x0028,
  QMI_VOICE_CMD_START_CONT_DTMF            = 0x0029,
  QMI_VOICE_CMD_STOP_CONT_DTMF             = 0x002A,
  QMI_VOICE_CMD_DTMF_IND                   = 0x002B,
  QMI_VOICE_CMD_SET_PREFERRED_PRIVACY      = 0x002C,
  QMI_VOICE_CMD_PRIVACY_IND                = 0x002D,
  QMI_VOICE_CMD_MANAGE_CALLS               = 0x0031,
  QMI_VOICE_CMD_SUPS_NOTIFICATION_IND      = 0x0032,
  QMI_VOICE_CMD_SET_SUPS_SERVICE           = 0x0033,
  QMI_VOICE_CMD_GET_CALL_WAITING           = 0x0034,
  QMI_VOICE_CMD_GET_CALL_BARRING           = 0x0035,
  QMI_VOICE_CMD_GET_CLIP                   = 0x0036,
  QMI_VOICE_CMD_GET_CLIR                   = 0x0037,
  QMI_VOICE_CMD_GET_CALL_FORWARDING        = 0x0038,
  QMI_VOICE_CMD_SET_CALL_BARRING_PASSWORD  = 0x0039
}qmi_voice_cmd_e_type;

/* For simplicity just use a C union to hold the subsystem specific data
   that is to be kept in the cmdlist */
typedef enum
{
  QMI_VOICE_CMD_FREE                      = 0x0001,  /*!< This cmd buffer is not in use */
  QMI_VOICE_CMD_AWAITING_CALLBACK         = 0x0002,  /*!< Cmd is waiting for RPC callback */
  QMI_VOICE_CMD_AWAITING_MORE_AMSS_EVENTS = 0x0004   /*!< Cmd completion depends on more 
                                                     AMSS events being received. */
} qmi_voice_cmd_state_e_type;

typedef union
{
  qmi_voice_cm_if_cmd_info_voice_type voice;
  qmi_voice_cm_if_cmd_info_sups_type  sups;
} qmi_voice_cmd_list_u_type;

typedef struct
{
  uint32                      hdl;  /* Command Handle */
  uint16                      cmd;  /* VOICE cmd */
  uint32                      sub_id;  /* Subsystem ID (e.g Call ID, SS Ref) */
  qmi_voice_cmd_state_e_type  state;  /* State of cmd list Entry */
  qmi_voice_cm_if_evt_e_type  pending_event_id;  /* Pending Event ID */
  qmi_voice_cmd_list_u_type   sub;  /*!< Union of subsystem specific information */
  uint8                       as_id; /* Subscription ID */
} qmi_voice_cmd_list_public_type;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/


void qmi_voice_cmd_list_init
( 
  void
);

IxErrnoType qmi_voice_cmd_list_new
( 
  int cmd,                     
  uint32 hdl,                    
  qmi_voice_cmd_state_e_type state,      
  qmi_voice_cm_if_evt_e_type pending_event_id, 
  uint32 sub_id,                    
  qmi_voice_cmd_list_u_type *sub_ptr,   
  uint32 sub_size,                 
  uint8  as_id
);

void qmi_voice_cmd_list_free_all
( 
  void
);

IxErrnoType qmi_voice_cmd_list_free
( 
  uint32 hdl
);

IxErrnoType qmi_voice_cmd_list_update_state
( 
  uint32 hdl,
  qmi_voice_cmd_state_e_type state
);

IxErrnoType qmi_voice_cmd_list_update_pending_event_id
( 
  uint32 hdl,
  qmi_voice_cm_if_evt_e_type pending_event_id
);

IxErrnoType qmi_voice_cmd_list_update_sub_id
( 
  uint32 hdl,
  uint32 sub_id
);

IxErrnoType qmi_voice_cmd_list_query
( 
  uint32 hdl,
  qmi_voice_cmd_list_public_type *info_ptr /*!< Output parameter */
);

IxErrnoType qmi_voice_cmd_list_query_by_event
( 
  qmi_voice_cm_if_evt_e_type pending_event_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8                          as_id
);

IxErrnoType qmi_voice_cmd_list_query_by_event_and_sub_id
( 
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8  as_id
);

IxErrnoType qmi_voice_cmd_list_query_by_event_and_sub_id_all_subs
( 
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr /*!< Output parameter */
);

IxErrnoType qmi_voice_cmd_list_query_by_event_and_sub_id_all_states
( 
  uint32 pending_event_id,
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8  as_id
);

IxErrnoType qmi_voice_cmd_list_query_by_event_all_states
( 
  qmi_voice_cm_if_evt_e_type     pending_event_id,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8                          as_id
);

IxErrnoType qmi_voice_cmd_list_query_by_sub_id
( 
  uint32 sub_id,
  qmi_voice_cmd_list_public_type *info_ptr /*!< Output parameter */
);

IxErrnoType qmi_voice_cmd_list_query_by_cmd_all_states
( 
  uint16 cmd,
  qmi_voice_cmd_list_public_type *info_ptr, /*!< Output parameter */
  uint8  as_id
);

IxErrnoType qmi_voice_cmd_list_update_pending_event_and_state
(
  uint32  hdl,
  qmi_voice_cm_if_evt_e_type pending_event_id,
  qmi_voice_cmd_state_e_type state
);

boolean qmi_voice_cmd_list_is_empty(void);

#endif /* _DS_QMI_VOICE_CMD_LIST_H */
