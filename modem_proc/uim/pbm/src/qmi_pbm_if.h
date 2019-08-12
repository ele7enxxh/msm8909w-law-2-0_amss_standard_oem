#ifndef _QMI_PBM_IF_H
#define _QMI_PBM_IF_H

/*===========================================================================

                         Q M I _ P B M  _ I F . H

DESCRIPTION

 The Qualcomm PhoneBook service PBM interface header file.


EXTERNALIZED FUNCTIONS



Copyright (c) 2014 QUALCOMM Technologies, Inc(QTI).
All Rights Reserved.
QUALCOMM Technologies Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

    $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/qmi_pbm_if.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/25/16    sp     F3 improvements
09/18/14   stv     Fix for logging strings in F3s 
07/25/14    NR     Use v2.0 diag macros
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
03/08/14    stv    Move F3 print buffer from stack to heap 
01/21/14    NR     PBM/QMI_PBM valid_pb_mask is not resetting after REFRESH EVENT
03/23/10    sk     Initial revision.
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "pbmlib.h"
#include  "log.h"
#include "cm.h"
#include "mmgsdilib.h"
#include "qmi_svc_defs.h"
#include "pbm.h"
#include "pbmutils.h"

#define QMI_PBM_IF_MAX_LOG_MSG_SIZE   150
#define QMI_PBM_IF_MAX_SESSIONS       9
#define QMI_PBM_IF_MAX_PBS            8
#define QMI_PBM_IF_INVALID_PB_CAT  0xFF
#define QMI_PBM_IF_INVALID_SLOT    0
#define QMI_PBM_IF_REFRESH_START 1
#define QMI_PBM_IF_REFRESH_END   2
#define QMI_PBM_IF_READ_RECORDS_MAX 10
/* Max num length in ascii from amss pbm.
   This includes + for international number, null terminating char */
#define QMI_PBM_IF_MAX_NUM_LEN  42
#define QMI_PBM_IF_MAX_SIM_NUM_LEN 40
/* Max text length in ucs2 from amss pbm.
   This includes the terminating nul char */
#define QMI_PBM_IF_MAX_TEXT_LEN 243
//below doesnt includes NULL char , 
//and is used for accessing 255 UTF chars
#define QMI_PBM_IF_MAX_TEXT_EXT_LEN 255
#define QMI_PBM_IF_MAX_SIM_NAME_LEN 241
#define QMI_PBM_IF_MAX_AD_NUM  4
#define QMI_PBM_IF_MAX_GRP 10
#define QMI_PBM_IF_MAX_EMAIL 2
#define QMI_PBM_IF_MAX_SEARCHED_RECORDS       1000
#define QMI_PBM_IF_MAX_NAME_LEN QMI_PBM_IF_MAX_TEXT_LEN

#define QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE 4
#define QMI_PBM_PB_SET_MAX 4
#define QMI_PBM_IF_SNPRINTF snprintf
#define QMI_PBM_IF_MEM_ALLOC(siz)  modem_mem_alloc(siz, MODEM_MEM_CLIENT_QMI_PBM)
//lint -emacro(774,PBM_MEM_FREEIF) doesn't matter if the if is always true here
#define QMI_PBM_IF_MEM_FREE(mem)                                     \
  {                                                             \
    if ((mem))                                                  \
    {                                                           \
      	modem_mem_free((mem),MODEM_MEM_CLIENT_QMI_PBM);         \
      	(mem) = NULL;                                           \
    }                                                           \
  }


/* Max Emergency number length as per 3gpp 31.102 */
#define QMI_PBM_IF_MAX_ECC_LEN      6
#define QMI_PBM_IF_MAX_HCODED_ECC   10
#define QMI_PBM_IF_MAX_HCODED_ECC_EXT 50
#define QMI_PBM_IF_MAX_CARD_ECC     255

#define QMI_PBM_IF_MAX_GRP_SESSIONS  QMI_PBM_IF_MAX_SESSIONS
#define QMI_PBM_IF_MAX_GRP_RECS      40
#define QMI_PBM_IF_MAX_GRP_NAME_LEN  QMI_PBM_IF_MAX_SIM_NAME_LEN
#define QMI_PBM_IF_MAX_AAS_SESSIONS  QMI_PBM_IF_MAX_SESSIONS
#define QMI_PBM_IF_MAX_AAS_RECS      40
#define QMI_PBM_IF_MAX_AAS_NAME_LEN  QMI_PBM_IF_MAX_SIM_NAME_LEN


#define QMI_PBM_IF_MAX_SEQ_NUM  0xFFFF


typedef enum
{
  QMI_PBM_IF_IND_REG_REC_UPDATE       = 0x00000001,
  QMI_PBM_IF_IND_REG_PB_READY         = 0x00000002,
  QMI_PBM_IF_IND_REG_EMER_LIST        = 0x00000004,
  QMI_PBM_IF_IND_REG_HIDDEN_STATUS    = 0x00000008,
  QMI_PBM_IF_IND_REG_AAS_UPDATE       = 0x00000010,
  QMI_PBM_IF_IND_REG_GAS_UPDATE       = 0x00000020
}qmi_pbm_if_ind_reg_e_type;

typedef enum
{
  QMI_PBM_IF_SESSION_GW_PRIMARY     = 0,
  QMI_PBM_IF_SESSION_1X_PRIMARY     = 1,
  QMI_PBM_IF_SESSION_GW_SECONDARY   = 2,
  QMI_PBM_IF_SESSION_1X_SECONDARY   = 3,
  QMI_PBM_IF_SESSION_GPB_SLOT1      = 6,
  QMI_PBM_IF_SESSION_GPB_SLOT2      = 7,
  QMI_PBM_IF_SESSION_GW_TERTIARY   = 8,
  QMI_PBM_IF_SESSION_1X_TERTIARY   = 9,
  QMI_PBM_IF_SESSION_GPB_SLOT3      = 10,
  QMI_PBM_IF_SESSION_MAX
}qmi_pbm_if_session_type;

typedef enum
{
  QMI_PBM_SLOT_NONE=0,
  QMI_PBM_SLOT_1,
  QMI_PBM_SLOT_2,
  QMI_PBM_SLOT_3,
  QMI_MAX_SLOTS = QMI_PBM_SLOT_3
} qmi_pbm_slot_type;


typedef enum
{
  QMI_PBM_IF_EMER_CAT_POLICE          = 0x01,
  QMI_PBM_IF_EMER_CAT_AMBULANCE       = 0x02,
  QMI_PBM_IF_EMER_CAT_FIRE_BRIGADE    = 0x04,
  QMI_PBM_IF_EMER_CAT_MARINE_GUARD    = 0x08,
  QMI_PBM_IF_EMER_CAT_MOUNT_RESCUE    = 0x10,
  QMI_PBM_IF_EMER_CAT_MANUAL_ECALL    = 0x20,
  QMI_PBM_IF_EMER_CAT_AUTO_ECALL      = 0x40,
  QMI_PBM_IF_EMER_CAT_SPARE           = 0x80
}qmi_pbm_if_emer_cat_e_type;

typedef enum
{
  QMI_PBM_IF_NUM_PLAN_UNKNOWN,
  QMI_PBM_IF_NUM_PLAN_ISDN,
  QMI_PBM_IF_NUM_PLAN_DATA,
  QMI_PBM_IF_NUM_PLAN_TELEX,
  QMI_PBM_IF_NUM_PLAN_NATIONAL,
  QMI_PBM_IF_NUM_PLAN_PRIVATE
}qmi_pbm_if_num_plan_e_type;

typedef enum
{
  QMI_PBM_IF_AMSS_NPI_UNKNOWN,
  QMI_PBM_IF_AMSS_NPI_ISDN,
  QMI_PBM_IF_AMSS_NPI_DATA     = 3,
  QMI_PBM_IF_AMSS_NPI_TELEX,
  QMI_PBM_IF_AMSS_NPI_NATIONAL = 8,
  QMI_PBM_IF_AMSS_NPI_PRIVATE
}qmi_pbm_if_amss_npi_e_type;


typedef enum
{
  QMI_PBM_IF_NUM_TYPE_UNKNOWN,
  QMI_PBM_IF_NUM_TYPE_INTERNATIONAL,
  QMI_PBM_IF_NUM_TYPE_NATIONAL,
  QMI_PBM_IF_NUM_TYPE_NETWK_SPECIFIC,
  QMI_PBM_IF_NUM_TYPE_DED_ACCESS

}qmi_pbm_if_num_type_e_type;

typedef enum
{
  QMI_PBM_IF_PB_DEFAULT   = 0x0000,
  QMI_PBM_IF_PB_ADN       = 0x0001,
  QMI_PBM_IF_PB_FDN       = 0x0002,
  QMI_PBM_IF_PB_MSISDN    = 0x0004,
  QMI_PBM_IF_PB_MBDN      = 0x0008,
  QMI_PBM_IF_PB_SDN       = 0x0010,
  QMI_PBM_IF_PB_BDN       = 0x0020,
  QMI_PBM_IF_PB_LND       = 0x0040,
  QMI_PBM_IF_PB_MBN       = 0x0080,
  QMI_PBM_IF_PB_GAS       = 0x100,
  QMI_PBM_IF_PB_AAS       = 0x200,
  QMI_PBM_DEVICE_TYPE_ALL = 0x400   /* Value used to set the appropriate bit in uint16 valid_pb_mask */
}qmi_pbm_if_pb_type;

typedef enum
{
  QMI_PBM_IF_PB_REC_OP_DEFAULT = 0x0,
  QMI_PBM_IF_PB_REC_OP_ADD     = 0x1,
  QMI_PBM_IF_PB_REC_OP_MODIFY  = 0x2,
  QMI_PBM_IF_PB_REC_OP_DELETE  = 0x3
}qmi_pbm_if_pb_rec_update_op_type;
typedef enum
{
  PBMI_CMD_VAL_INDICATION_REGISTER           = 0x0001,
  PBMI_CMD_VAL_GET_PB_CAPABILITIES           = 0x0002,
  PBMI_CMD_VAL_GET_ALL_PB_CAPABILITIES       = 0x0003,
  PBMI_CMD_VAL_READ_RECORDS                  = 0x0004,
  PBMI_CMD_VAL_RECORD_READ_IND               = PBMI_CMD_VAL_READ_RECORDS,
  PBMI_CMD_VAL_WRITE_RECORD                  = 0x0005,
  PBMI_CMD_VAL_DELETE_RECORD                 = 0x0006,
  PBMI_CMD_VAL_DELETE_ALL_PB_RECORDS         = 0x0007,
  PBMI_CMD_VAL_SEARCH_RECORDS                = 0x0008,
  PBMI_CMD_VAL_RECORD_UPDATE_IND             = 0x0009,
  PBMI_CMD_VAL_REFRESH_IND                   = 0x000A,
  PBMI_CMD_VAL_PB_READY_IND                  = 0x000B,
  PBMI_CMD_VAL_EMERGENCY_LIST_IND            = 0x000C,
  PBMI_CMD_VAL_ALL_PB_INIT_DONE_IND          = 0x000D,
  PBMI_CMD_VAL_GET_EMERGENCY_LIST            = 0x000E,
  PBMI_CMD_VAL_GET_ALL_GROUPS                = 0x000F,
  PBMI_CMD_VAL_SET_GROUP_INFO                = 0x0010,
  PBMI_CMD_VAL_GET_PB_STATE                  = 0x0011,
  PBMI_CMD_VAL_READ_ALL_HIDDEN_RECORDS       = 0x0012,
  PBMI_CMD_VAL_HIDDEN_RECORD_STATUS_IND      = 0x0013,
  PBMI_CMD_VAL_GET_NEXT_EMPTY_RECORD_ID      = 0x0014,
  PBMI_CMD_VAL_GET_NEXT_NON_EMPTY_RECORD_ID  = 0x0015,
  PBMI_CMD_VAL_GET_ALL_AAS                   = 0x0016,
  PBMI_CMD_VAL_SET_AAS                       = 0x0017,
  PBMI_CMD_VAL_AAS_UPDATE_IND                = 0x0018,
  PBMI_CMD_VAL_GAS_UPDATE_IND                = 0x0019,
  PBMI_CMD_VAL_BIND_SUBSCRIPTION             = 0x001A,
  PBMI_CMD_VAL_GET_SUBSCRIPTION_BINDING      = 0x001B,
  PBMI_CMD_VAL_READ_PBSET_CAPS               = 0x001C,  
  PBMI_CMD_VAL_PBSET_CAPS_READ_IND           = PBMI_CMD_VAL_READ_PBSET_CAPS,
  PBMI_CMD_VAL_READ_RECORDS_EXT              = 0x001D,  
  PBMI_CMD_VAL_READ_RECORDS_EXT_IND          = PBMI_CMD_VAL_READ_RECORDS_EXT,  
  PBMI_CMD_VAL_WRITE_RECORD_EXT              = 0x001E,  
  PBMI_CMD_VAL_SEARCH_RECORDS_EXT            = 0x001F,
  PBMI_CMD_VAL_READ_ALL_HIDDEN_RECORDS_EXT   = 0x0020,
  PBMI_CMD_VAL_SIM_READY_IND                 = 0x0021,
  PBMI_CMD_VAL_READ_RECORDS_EXT_UNDECODED    = 0x0022,
  PBMI_CMD_VAL_READ_RECORDS_EXT_UNDECODED_IND = PBMI_CMD_VAL_READ_RECORDS_EXT_UNDECODED,

/* QC EXTERNAL QMI COMMAND RANGE IS 0x0000 - 0x5555. 
   Add the next external QMI Command here */

/* VENDOR SPECIFIC QMI COMMAND RANGE IS 0x5556 - 0xAAAA.
   IMPORTANT!
   Add the vendor specific QMI Commands within this range only to avoid 
   conflicts with QC QMI commands that would get released in future */

/* RESERVED QC QMI COMMAND RANGE IS 0xAAAB - 0xFFFE 
   Internal QMI Commands must be added in DECREASING ORDER from Below */
  PBMI_CMD_VAL_WIDTH                  = 0xFFFF
}qmi_pbmi_cmd_val_e_type;

typedef enum
{
  QMI_PBM_IF_CMD_MIN                                = 0,

  /* Pbm commands */
  QMI_PBM_IF_CMD_INDICATION_REGISTER                = QMI_PBM_IF_CMD_MIN,
  QMI_PBM_IF_CMD_GET_PB_CAPABILITIES                ,
  QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES            ,
  QMI_PBM_IF_CMD_READ_RECORDS                       ,
  QMI_PBM_IF_CMD_WRITE_RECORD                       ,
  QMI_PBM_IF_CMD_DELETE_RECORD                      ,
  QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS              ,
  QMI_PBM_IF_CMD_SEARCH_RECORDS                     ,
  QMI_PBM_IF_CMD_GET_EMERGENCY_LIST                 ,
  QMI_PBM_IF_CMD_GET_PB_STATE                       ,
  QMI_PBM_IF_CMD_GET_ALL_GROUPS                     ,
  QMI_PBM_IF_CMD_SET_GROUP_INFO                     ,
  QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS            , 
  QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID           ,
  QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID       ,
  QMI_PBM_IF_CMD_GET_ALL_AAS                        ,
  QMI_PBM_IF_CMD_SET_AAS                            ,
  QMI_PBM_IF_CMD_READ_PBSET_CAPS                    ,/* 17 */  
  QMI_PBM_IF_CMD_WRITE_RECORD_EXT,     
  QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED         ,

  /* Responses */
  QMI_PBM_IF_CMD_INDICATION_REGISTER_RESP           ,
  QMI_PBM_IF_CMD_GET_PB_CAPABILITIES_RESP           ,
  QMI_PBM_IF_CMD_GET_ALL_PB_CAPABILITIES_RESP       ,
  QMI_PBM_IF_CMD_READ_RECORDS_RESP                  ,
  QMI_PBM_IF_CMD_WRITE_RECORD_RESP                  ,
  QMI_PBM_IF_CMD_DELETE_RECORD_RESP                 ,
  QMI_PBM_IF_CMD_DELETE_ALL_PB_RECORDS_RESP         ,
  QMI_PBM_IF_CMD_SEARCH_RECORDS_RESP                ,
  QMI_PBM_IF_CMD_GET_EMERGENCY_LIST_RESP            ,
  QMI_PBM_IF_CMD_GET_PB_STATE_RESP                  ,
  QMI_PBM_IF_CMD_GET_ALL_GROUPS_RESP                ,
  QMI_PBM_IF_CMD_SET_GROUP_INFO_RESP                ,
  QMI_PBM_IF_CMD_READ_ALL_HIDDEN_RECORDS_RESP       ,
  QMI_PBM_IF_CMD_GET_NEXT_EMPTY_RECORD_ID_RESP      ,
  QMI_PBM_IF_CMD_GET_NEXT_NON_EMPTY_RECORD_ID_RESP  ,
  QMI_PBM_IF_CMD_GET_ALL_AAS_RESP                   ,
  QMI_PBM_IF_CMD_SET_AAS_RESP                       ,
  QMI_PBM_IF_CMD_READ_PBSET_CAPS_RESP               ,/* 38 */
  QMI_PBM_IF_CMD_SEARCH_RECORDS_EXT_RESP            ,
  QMI_PBM_IF_CMD_READ_RECORDS_EXT_UNDECODED_RESP    ,

  /* Indications */
  QMI_PBM_IF_CMD_RECORD_UPDATE_IND                  ,
  QMI_PBM_IF_CMD_REFRESH_IND                        ,
  QMI_PBM_IF_CMD_PB_READY_IND                       ,
  QMI_PBM_IF_CMD_EMERGENCY_LIST_IND                 ,
  QMI_PBM_IF_CMD_ALL_PB_INIT_DONE_IND               ,
  QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_IND           ,
  QMI_PBM_IF_CMD_INTERNAL_RECORD_READ_EXT_IND       ,
  QMI_PBM_IF_CMD_RECORD_READ_IND                    ,
  QMI_PBM_IF_CMD_RECORD_READ_EXT_IND                    ,
  QMI_PBM_IF_CMD_INTERNAL_PBSET_CAPS_READ_IND       ,
  QMI_PBM_IF_CMD_PBSET_CAPS_READ_IND                , 
  QMI_PBM_IF_CMD_HIDDEN_RECORD_STATUS_IND           ,
  QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_IND   ,
  QMI_PBM_IF_CMD_INTERNAL_HIDDEN_RECORDS_READ_EXT_IND   ,
  QMI_PBM_IF_CMD_AAS_UPDATE_IND                     ,
  QMI_PBM_IF_CMD_GAS_UPDATE_IND                     ,/* 49 */
  QMI_PBM_IF_CMD_SIM_READY_IND                       ,
  QMI_PBM_IF_CMD_INTERNAL_READ_RECORDS_EXT_UNDECODED_IND     ,

  /* PBM COMMAND, EVENT Callbacks */
  QMI_PBM_IF_CMD_WRITE_CB                           ,
  QMI_PBM_IF_CMD_EVT_CB                             ,
  QMI_PBM_IF_CMD_MAX                                ,
  QMI_PBM_IF_CMD_WIDTH                              = 0xFFFF
} qmi_pbm_if_cmd_e_type;


typedef enum
{
  QMI_PBM_IF_EMERG_CAT_TYPE_NONE = 0x0,
  QMI_PBM_IF_EMERG_CAT_TYPE_POLICE_MASK= 0x01,
  QMI_PBM_IF_EMERG_CAT_TYPE_AMBULANCE_MASK= 0x02,
  QMI_PBM_IF_EMERG_CAT_TYPE_FIRE_BRIGADE_MASK= 0x4,
  QMI_PBM_IF_EMERG_CAT_TYPE_MARINE_GUARD_MASK= 0x08,
  QMI_PBM_IF_EMERG_CAT_TYPE_MOUNTAIN_RESCUE_MASK= 0x10,
  QMI_PBM_IF_EMERG_CAT_TYPE_MANUAL_ECALL_MASK = 0x20,
  QMI_PBM_IF_EMERG_CAT_TYPE_AUTO_ECALL_MASK = 0x40,
  QMI_PBM_IF_EMERG_CAT_TYPE_SPARE = 0x80
} qmi_pbm_if_emer_category_e_type;

typedef enum{
  QMI_PBM_IF_PB_STATE_READY              = 0x00,
  QMI_PBM_IF_PB_STATE_NOT_READY          = 0x01,
  QMI_PBM_IF_PB_STATE_NOT_AVAILABLE      = 0x02,
  QMI_PBM_IF_PB_STATE_PIN_RESTRICTION    = 0x03,
  QMI_PBM_IF_PB_STATE_PUK_RESTRICTION    = 0x04,
  QMI_PBM_IF_PB_STATE_INVALIDATED        = 0x05,
  QMI_PBM_IF_PB_STATE_SYNC               = 0x06,
  QMI_PBM_IF_PB_STATE_MAX
}qmi_pbm_if_pb_state_e_type;

typedef enum{
  QMI_PBM_IF_GRP_OPER_ADD,
  QMI_PBM_IF_GRP_OPER_MODIFY,
  QMI_PBM_IF_GRP_OPER_DELETE
}qmi_pbm_if_grp_oper_e_type;

typedef enum{
  QMI_PBM_IF_AAS_OPER_ADD,
  QMI_PBM_IF_AAS_OPER_MODIFY,
  QMI_PBM_IF_AAS_OPER_DELETE
}qmi_pbm_if_aas_oper_e_type;

/* --------------------------log pkt-----------------*/

/* Subsystem type of the call flow */
typedef enum
{
  LOG_QMI_PBM_SUBSYSTEM_NONE, /* Used for banners e.g. state change that go across all subsystems */
  LOG_QMI_PBM_SUBSYSTEM_PBM_AMSS,
  LOG_QMI_PBM_SUBSYSTEM_PBM_SVC,
  LOG_QMI_PBM_SUBSYSTEM_CP,
  LOG_QMI_PBM_SUBSYSTEM_PBM_IF
} qmi_pbm_if_call_flow_subsystem_e_type;

/* Arrow type of the call flow */
typedef enum
{
  /* Bit 2 - Line type [0=Solid line, 1=Dashed line],
     Bit 1 - Arrow direction [0=One direction, 1=Bi-directional],
     Bit 0 - Arrow head type [0=Solid arrow head, 1=Open arrow head]
  */
  LOG_QMI_PBM_S_LINE_S_HEAD_S_DIR = 0x00, /* Solid line with solid arrow head pointing at destination */
  LOG_QMI_PBM_S_LINE_O_HEAD_S_DIR = 0x01, /* Solid line with open arrow head pointing at destination */
  LOG_QMI_PBM_S_LINE_S_HEAD_B_DIR = 0x02, /* Solid line with solid arrow head pointing at both source and destination */
  LOG_QMI_PBM_S_LINE_O_HEAD_B_DIR = 0x03, /* Solid line with open arrow head pointing at both source and destination */
  LOG_QMI_PBM_D_LINE_S_HEAD_S_DIR = 0x04, /* Dashed line with solid arrow head pointing at destination */
  LOG_QMI_PBM_D_LINE_O_HEAD_S_DIR = 0x05, /* Dashed line with open arrow head pointing at destination */
  LOG_QMI_PBM_D_LINE_S_HEAD_B_DIR = 0x06, /* Dashed line with solid arrow head pointing at both source and destination */
  LOG_QMI_PBM_D_LINE_O_HEAD_B_DIR = 0x07  /* Dashed line with open arrow head pointing at both source and destination */
} qmi_pbm_if_call_flow_arrow_e_type;


typedef struct
{
  log_hdr_type hdr;     /* Log header (length, code, timestamp) */
  uint8 src_subsystem;  /* Subsystem generating this call flow event */
  uint8 dest_subsystem; /* Subsystem this call flow event is being sent to */
  uint8 arrow;          /* Bit mask describing the line and arrow type to be drawn on the call flow */
  uint8 label[ 1 ];     /* Used to locate the first character of the Text to be displayed for the banner or arrow */
} qmi_pbm_if_call_flow_log_packet_type;

/* --------------------------log pkt-----------------*/
typedef enum
{
  QMI_PBM_IF_CARD_PIN1    = 0x00000001,
  QMI_PBM_IF_CARD_PIN2    = 0x00000002,
  QMI_PBM_IF_CARD_ADM1    = 0x00000004,
  QMI_PBM_IF_CARD_ADM2    = 0x00000008
}qmi_pbm_if_pin_e_type;

typedef enum
{
  QMI_PBM_IF_ALLOWED,
  QMI_PBM_IF_NEVER_ALLOWED,
  QMI_PBM_IF_AND_ALLOWED,
  QMI_PBM_IF_OR_ALLOWED,
  QMI_PBM_IF_SINGLE_ALLOWED,
  QMI_PBM_IF_NOT_KNOWN
}qmi_pbm_if_protection_e_type;

typedef struct {
  uint8 max_records;
  uint8 used_records;
  uint8 max_record_len;
}pbm_record_capability_s_type;

typedef struct {
  pbm_record_capability_s_type     record_caps;
  uint8                            mapping_type;
}pbm_usim_capability_ext_s_type;  

typedef struct {
  uint8 max_adn_records;
  uint8 used_adn_records;
  uint8 max_adn_num_len;
  uint8 max_adn_name_len;
}pbm_adn_caps_info_s_type; 

typedef struct {
  uint8 pb_set_id;
  pbm_adn_caps_info_s_type  adn_caps;
  uint32 email_caps_len; 
  pbm_usim_capability_ext_s_type email_caps[QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE];
  uint32 ad_num_caps_len; 
  pbm_usim_capability_ext_s_type ad_num_caps[QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE];
  uint32 sne_caps_len; 
  pbm_usim_capability_ext_s_type sne_caps[QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE];
  uint32 grp_caps_len; 
  pbm_usim_capability_ext_s_type grp_caps[QMI_PBM_MAX_FIELDS_IN_SAME_USIM_TYPE];
}pbm_pb_set_cap_info_s_type;

typedef struct {
  uint8 pb_sets;
  pbm_record_capability_s_type aas_caps;
  pbm_record_capability_s_type gas_caps;
  uint32 pb_set_info_len;
  pbm_pb_set_cap_info_s_type pb_set_info[QMI_PBM_PB_SET_MAX];
}pbm_pbr_cap_info_s_type;

typedef struct
{
  uint16      pb_type;
  uint16      used_recs;
  uint16      max_recs;
  uint8       max_num_len;
  uint8       max_name_len;
  uint8       is_sec_name_supported;
  uint8       max_sec_name_len;
  boolean     is_hidden_entry_supported;
  boolean     is_grp_valid;
  struct
  {
    uint8       max_grps;
    uint8       max_grp_tag_len;
  }grp_cap;

  boolean     is_ad_num_valid;
  struct
  {
    uint8       max_ad_nums;
    uint8       max_ad_num_len;
    uint8       max_ad_num_tag_len;
  }ad_num_cap;

  boolean     is_email_valid;
  struct
  {
    uint8       max_emails;
    uint8       max_email_len;
  }email_cap;

  boolean     is_gas_cap_valid;
  struct
  {
    uint8      max_recs;
    uint8      used_recs;
    uint8      max_gas_len;
  }gas_cap;

  boolean     is_aas_cap_valid;
  struct
  {
    uint8      max_recs;
    uint8      used_recs;
    uint8      max_aas_len;
  }aas_cap;  
  
  struct
  {
    uint64 pbm_protection_type;
    uint32       pbm_pin_mask;
  }write_access_cap;

  uint16 num_pb_sets;
  boolean pbset_info_valid;
  pbm_pbr_cap_info_s_type pb_set_caps; 
}qmi_pbm_if_pb_caps_s;

typedef struct
{
  uint8                   session_type;
  uint8                   num_of_pbs;
  qmi_pbm_if_pb_caps_s    pb_caps[QMI_PBM_IF_MAX_PBS];
}qmi_pbm_if_session_caps_s;

typedef struct
{
  uint8         num_type;
  uint8         num_plan;
  uint8         len;
  uint8         num[QMI_PBM_IF_MAX_NUM_LEN];
}qmi_pbm_if_num_info_s;

typedef struct
{
  uint8 len;
  uint8 text[QMI_PBM_IF_MAX_TEXT_EXT_LEN*2];
}qmi_pbm_if_text_info_s;

typedef struct
{
  uint8 len;
  uint8 text[QMI_PBM_IF_MAX_TEXT_EXT_LEN];
}qmi_pbm_if_text_info_undecoded_s;

typedef struct
{
  qmi_pbm_if_num_info_s  ad_num;
  uint8 tag_id; /* Record number in the EF-AAS */
}qmi_pbm_if_ad_num_data_s;

typedef struct
{
  uint16                      rec_id;
  qmi_pbm_if_num_info_s       num_info;
  qmi_pbm_if_text_info_s      name_info;
  boolean                     is_hidden;
  qmi_pbm_if_text_info_s      sname_info;
  uint8                       ad_num_cnt;
  qmi_pbm_if_ad_num_data_s    ad_num_data[QMI_PBM_IF_MAX_AD_NUM];
  uint8                       grp_cnt;
  uint8                       grp_id[QMI_PBM_IF_MAX_GRP];
  uint8                       email_cnt;
  qmi_pbm_if_text_info_s      email_data[QMI_PBM_IF_MAX_EMAIL];
}qmi_pbm_if_rec_data_s;

typedef struct
{
  uint16                      		rec_id;
  qmi_pbm_if_num_info_s       		num_info;
  qmi_pbm_if_text_info_undecoded_s  name_info;
  boolean                     		is_hidden;
  qmi_pbm_if_text_info_undecoded_s  sname_info;
  uint8                             ad_num_cnt;
  qmi_pbm_if_ad_num_data_s          ad_num_data[QMI_PBM_IF_MAX_AD_NUM];
  uint8                             grp_cnt;
  uint8                             grp_id[QMI_PBM_IF_MAX_GRP];
  uint8                             email_cnt;
  qmi_pbm_if_text_info_undecoded_s  email_data[QMI_PBM_IF_MAX_EMAIL];
}qmi_pbm_if_rec_data_undecoded_s;

typedef struct
{
  uint8 ecc_len;
  char  ecc[QMI_PBM_IF_MAX_ECC_LEN];
}qmi_pbm_if_ecc_data_type;

typedef struct
{
  uint8 category; /* Mask of the qmi_pbm_if_emer_cat_e_type values */
  uint8 ecc_len;
  char  ecc[QMI_PBM_IF_MAX_ECC_LEN];
}qmi_pbm_if_cat_ecc_data_type;


typedef struct{
  uint8  sess_type;
  uint8  ecc_count;
  qmi_pbm_if_cat_ecc_data_type  ecc_info[QMI_PBM_IF_MAX_CARD_ECC];
}qmi_pbm_if_ecc_session_card_data;

typedef struct{
  uint8  sess_type;
  uint8  ecc_count;
  qmi_pbm_if_cat_ecc_data_type  ecc_info[CM_MAX_EMERGENCY_NUM_COUNT ];
}qmi_pbm_if_ecc_session_ota_data;

typedef struct {
  int num_hcoded_ecc_entries;
  qmi_pbm_if_cat_ecc_data_type  hcoded_ecc[ QMI_PBM_IF_MAX_HCODED_ECC_EXT ];

  int  num_nv_ecc_entries;
  qmi_pbm_if_cat_ecc_data_type nv_ecc[ PBM_NV_EMERGENCY_NUMBERS ];

  int num_of_card_sessions;
  qmi_pbm_if_ecc_session_card_data card_ecc[ QMI_PBM_IF_MAX_SESSIONS ];

  int num_of_ota_sessions;
  qmi_pbm_if_ecc_session_ota_data ota_ecc[ QMI_PBM_IF_MAX_SESSIONS ];
} qmi_pbm_if_ecc_list_type;
typedef struct
{
  uint8 sess_type;
  boolean is_initialized;
  uint16 valid_pb_mask; /* Bit mask of the pbs which are ready/valid */
  uint16 del_all_pb_mask; /* Bit mask of the pbs currently being deleted */
  struct
  {
    uint16 max_adn_recs;
    uint8  max_fdn_recs;
    uint8  max_msisdn_recs;
    uint8  max_mbdn_recs;
    uint8  max_mbn_recs;
    uint8  max_sdn_recs;
    uint8  max_bdn_recs;
    uint8  max_lnd_recs;
    uint8  max_gas_recs;
    uint8  max_aas_recs;
  }pb_data;
}qmi_pbm_if_sess_data_type;

typedef struct
{
  uint8                            num_of_sessions;
  qmi_pbm_if_sess_data_type        sess_data[QMI_PBM_IF_MAX_SESSIONS];
  boolean                          sim_ready[QMI_MAX_SLOTS];
}qmi_pbm_info_type;


typedef struct
{
  uint8  grp_id;
  uint8  grp_name_len;
  uint8  grp_name[QMI_PBM_IF_MAX_GRP_NAME_LEN];
}qmi_pbm_if_grp_data_s;

typedef struct
{
  uint8  sess_type;
  uint8  grp_cnt;
  qmi_pbm_if_grp_data_s grp_data[QMI_PBM_IF_MAX_GRP_RECS];
}qmi_pbm_if_session_grps_s;

typedef struct
{
  uint8  aas_id;
  uint8  aas_len;
  uint8  alpha[QMI_PBM_IF_MAX_AAS_NAME_LEN];
}qmi_pbm_if_aas_data_s;

typedef struct
{
  uint8  sess_type;
  uint8  aas_cnt;
  qmi_pbm_if_aas_data_s aas_data[QMI_PBM_IF_MAX_AAS_RECS];
}qmi_pbm_if_session_aas_s;


/*===========================================================================

                    REQUESTS

===========================================================================*/
typedef struct
{
  boolean send_pb_ready_ind;
  boolean send_emer_list_ind;
} qmi_pbm_if_indication_register_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
} qmi_pbm_if_get_pb_capabilities_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
  uint16 start_id;
  uint16 end_id;
} qmi_pbm_if_read_records_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
  uint16 start_id;
  uint16 end_id;
} qmi_pbm_if_read_records_ext_undecoded_s;

typedef struct
{
  uint8 session_type;
  uint16 start_id;
  uint16 end_id;
} qmi_pbm_if_read_pbset_caps_s;


typedef struct
{
  uint8                  session_type;
  uint16                 pb_type;
  qmi_pbm_if_rec_data_s  rec_data;
} qmi_pbm_if_write_record_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
  uint16 record_id;
}qmi_pbm_if_delete_record_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
}qmi_pbm_if_delete_all_pb_records_s;


typedef struct
{
  uint8 session_type;
  uint16 pb_type;
  uint8 number_info_valid;
  struct
  {
    uint8 num_len;
    uint8 number[QMI_PBM_IF_MAX_NUM_LEN];
  }num_info;
  uint8 name_info_valid;
  struct
  {
    uint8 name_len;
    uint8 name[QMI_PBM_IF_MAX_TEXT_EXT_LEN*2];
  }name_info;
}qmi_pbm_if_search_records_s;


typedef struct
{
  uint8 session_type;
  uint16 pb_type;
}qmi_pbm_if_get_pb_state_s;

typedef struct
{
  uint8  sess_type;
  uint8  operation;
  uint8  grp_id;
  uint8  grp_name_len;
  uint8  grp_name[QMI_PBM_IF_MAX_NAME_LEN];
}qmi_pbm_if_set_grp_info_s;

typedef struct
{
  uint8  sess_type;
  uint8  operation;
  uint8  aas_id;
  uint8  alpha_len;
  uint8  alpha[QMI_PBM_IF_MAX_NAME_LEN];
}qmi_pbm_if_set_aas_s;

typedef struct
{
  uint8 sess_type;
}qmi_pbm_if_read_all_hidden_recs_s;

typedef struct 
{
  uint8 session_type;
  uint16 pb_type;
  uint16 record_id;
}qmi_pbm_if_get_next_empty_record_id_s;

typedef struct 
{
  uint8 session_type;
  uint16 pb_type;
  uint16 record_id;
}qmi_pbm_if_get_next_non_empty_record_id_s;


/*===========================================================================

                    RESPONSES

===========================================================================*/

typedef struct
{
  qmi_error_e_type        error;
  uint8                   sess_type;
  qmi_pbm_if_pb_caps_s    pb_caps;
}qmi_pbm_if_get_pb_capabilities_resp_s;


typedef struct
{
  qmi_error_e_type            error;
  uint8                       num_of_sessions;
  qmi_pbm_if_session_caps_s   sess_caps[QMI_PBM_IF_MAX_SESSIONS];
}qmi_pbm_if_get_all_pb_capabilities_resp_s;

typedef struct
{
  qmi_error_e_type        error;
  uint8 sess_type;
  uint16 pb_type;
  uint16 rec_cnt;
}qmi_pbm_if_read_records_resp_s;

typedef struct
{
  qmi_error_e_type        error;
  uint8 sess_type;
  uint16 pb_type;
  uint16 rec_cnt;
}qmi_pbm_if_read_records_ext_undecoded_resp_s;


typedef struct
{
  qmi_error_e_type        error;
  uint8 sess_type;
  uint16 pbset_cnt;
}qmi_pbm_if_read_pbset_caps_resp_s;



typedef struct
{
  qmi_error_e_type        error;
  uint16                  rec_id;
}qmi_pbm_if_write_record_resp_s;

typedef struct
{
  qmi_error_e_type        error;
  uint16 record_id;
}qmi_pbm_if_delete_record_resp_s;

typedef struct
{
  qmi_error_e_type        error;
}qmi_pbm_if_delete_all_pb_records_resp_s;

typedef struct
{
  qmi_error_e_type   error;
  uint16    num_rec_ids;
  uint16   rec_ids[QMI_PBM_IF_MAX_SEARCHED_RECORDS];
}qmi_pbm_if_search_records_resp_s;

typedef struct
{
  qmi_error_e_type         error;
  qmi_pbm_if_ecc_list_type *emer_data;
}qmi_pbm_if_get_emerg_list_resp_s;

typedef struct
{
  qmi_error_e_type    error;
  uint8               sess_type;
  uint16              pb_type;
  uint8               state;
}qmi_pbm_if_get_pb_state_resp_s;

typedef struct
{
  qmi_error_e_type            error;
  uint8                       num_of_sessions;
  qmi_pbm_if_session_grps_s   sess_grps[QMI_PBM_IF_MAX_GRP_SESSIONS];
}qmi_pbm_if_get_all_grp_resp_s;

typedef struct
{
  qmi_error_e_type    error;
  uint8               sess_type;
  uint8               grp_id;
}qmi_pbm_if_set_grp_info_resp_s;

typedef struct
{
  qmi_error_e_type        error;
  uint8 sess_type;
  uint16 rec_cnt;
}qmi_pbm_if_read_all_hidden_recs_resp_s;

typedef struct
{
  qmi_error_e_type        error;
  uint16 record_id;
}qmi_pbm_if_get_next_empty_record_id_resp_s;

typedef struct
{
  qmi_error_e_type        error;
  uint16 record_id;
}qmi_pbm_if_get_next_non_empty_record_id_resp_s;

typedef struct
{
  qmi_error_e_type            error;
  uint8                       num_of_sessions;
  qmi_pbm_if_session_aas_s    sess_aas[QMI_PBM_IF_MAX_AAS_SESSIONS];
}qmi_pbm_if_get_all_aas_resp_s;

typedef struct
{
  qmi_error_e_type    error;
  uint8               sess_type;
  uint8               aas_id;
}qmi_pbm_if_set_aas_resp_s;


/*===========================================================================

                   INDICATIONS

===========================================================================*/

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
  uint8  refresh_status;
}qmi_pbm_if_refresh_ind_s;


typedef struct
{
  uint8 num_of_sessions;
  struct
  {
    uint8  sess_type;
    uint16 pb_mask;
  }sess_info[QMI_PBM_IF_MAX_SESSIONS];
}qmi_pbm_if_all_pb_init_done_ind_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
  uint8  operation;
  uint16 record_id;
}qmi_pbm_if_record_update_ind_s;

typedef struct
{
  uint8 session_type;
  uint16 pb_type;
}qmi_pbm_if_pb_ready_ind_s;

typedef struct
{
  uint32 slot_id;
}qmi_pbm_if_sim_ready_ind_s;

typedef struct
{
  qmi_pbm_if_ecc_list_type  *emer_data;
}qmi_pbm_if_emer_ind_s;

typedef struct
{
  uint16 seq_num;
  uint8 sess_type;
  uint16 pb_type;
  uint16 start_id;
  uint16 end_id; /* The last record to be read */
}qmi_pbm_if_internal_record_read_ind_s;

typedef struct
{
  uint16 seq_num;
  uint8 sess_type;
  uint16 pb_type;
  uint16 start_id;
  uint16 end_id; /* The last record to be read */
}qmi_pbm_if_internal_record_read_ext_undecoded_ind_s;

typedef struct
{
  uint8 sess_type;
  uint16 start_id;  
  uint16 end_id; /* The last pbset to be read */
  pbm_all_usim_field_capabilities_s_type *usim_cap_info;
}qmi_pbm_if_internal_pbset_caps_read_ind_s;



typedef struct
{
  uint8               sess_type;
  pbm_record_id_type  last_hidden_rec;
}qmi_pbm_if_internal_hidden_recs_read_ind_s;

typedef struct
{
  uint8 sess_type;
  uint16 pb_type;
  uint16 seq_num;
  uint8                   num_of_recs;
  qmi_pbm_if_rec_data_s   rec_data[QMI_PBM_IF_READ_RECORDS_MAX];
}qmi_pbm_if_record_read_ind_s;

typedef struct
{
  uint8 							sess_type;
  uint16 							pb_type;
  uint16 							seq_num;
  uint8                   			num_of_recs;
  qmi_pbm_if_rec_data_undecoded_s   rec_data[QMI_PBM_IF_READ_RECORDS_MAX];
}qmi_pbm_if_record_read_ext_undecoded_ind_s;

typedef enum {
  QMI_PBM_FIELD_EMAIL = 0x00,
  QMI_PBM_FIELD_ANR = 0x01,
  QMI_PBM_FIELD_SNE = 0x02,
  QMI_PBM_FIELD_GRP = 0x03,
  QMI_PBM_FIELD_NONE
} qmi_pbm_field_id_type;

//Enumeration for the usim file mapping type
typedef enum {
  QMI_PBM_MAPPING_TYPE_1 = 0x01,
  QMI_PBM_MAPPING_TYPE_2 = 0x02,
  QMI_PBM_MAPPING_TYPE_NONE
} qmi_pbm_mapping_type;

typedef struct
{
  qmi_pbm_mapping_type mapping_type;
  qmi_pbm_field_id_type field_id; 
  uint16   num_of_records;
  uint16   records_in_use;
  uint16   record_length;       
}usim_field_caps_type;



typedef struct
{
  uint32 mapping_type;
  uint32 field_id; 
  uint16   num_of_records;
  uint16   num_free_records;
  uint16   record_length;       
}qmi_pbm_if_pbset_cap_data_s;

typedef struct
{
  uint8 sess_type;
  uint8                   num_of_pbsets;
  qmi_pbm_if_pbset_cap_data_s   pbset_cap_data[QMI_PBM_IF_READ_RECORDS_MAX][PBM_MAX_USIM_FIELDS_IN_PB_SET];
  boolean is_last_ind;
}qmi_pbm_if_pbset_cap_read_ind_s;


typedef struct
{
  uint8 sess_type;
  uint8 status;
}qmi_pbm_if_hidden_record_status_ind_s;

typedef struct
{
  uint8 prim_subs_slot;
}qmi_pbm_if_internal_prim_subs_slot_ind_s;

typedef struct
{
  uint8 session_type;
  uint8  operation;
  uint8 aas_id;
  uint16  alpha_len;
  uint8  alpha[QMI_PBM_IF_MAX_TEXT_EXT_LEN*2];
}qmi_pbm_if_aas_update_ind_s;


typedef struct
{
  uint8 session_type;
  uint8 operation;
  uint8 gas_id;
  uint16 grp_name_len;
  uint8 grp_name[QMI_PBM_IF_MAX_TEXT_EXT_LEN*2];
}qmi_pbm_if_gas_update_ind_s;


/*===========================================================================

                   CALLBACKS

===========================================================================*/
typedef struct
{
  pbm_notify_data_s_type data;
}qmi_pbm_if_pbm_evt_cb_s;

typedef struct
{
  pbm_writecb_data_s_type data;
}qmi_pbm_if_pbm_write_cb_s;

typedef struct
{
  mmgsdi_cnf_type data;
}qmi_pbm_if_mmgsdi_client_id_reg_cb_s;

typedef struct
{
  struct
  {
    uint16    cmd;
    uint32    handle;
  } header;

  union
  {
    /* requests */
    qmi_pbm_if_indication_register_s           ind_reg;
    qmi_pbm_if_get_pb_capabilities_s           get_pb_caps;
    qmi_pbm_if_read_records_s                  read_recs;
    qmi_pbm_if_read_pbset_caps_s               read_pbset_caps;
    qmi_pbm_if_write_record_s                  write_rec;
    qmi_pbm_if_delete_record_s                 delete_rec;
    qmi_pbm_if_delete_all_pb_records_s         delete_all_pb_recs;
    qmi_pbm_if_search_records_s            search_recs;
    qmi_pbm_if_get_pb_state_s                  get_pb_state;
    qmi_pbm_if_set_grp_info_s                  set_grp_info;
    qmi_pbm_if_read_all_hidden_recs_s          read_all_hidden_recs;
    qmi_pbm_if_get_next_empty_record_id_s      nxt_empty_rec_id;
    qmi_pbm_if_get_next_non_empty_record_id_s  nxt_non_empty_rec_id;
    qmi_pbm_if_set_aas_s                       set_aas;
    qmi_pbm_if_read_records_ext_undecoded_s    read_recs_ext_undecoded;

    /* responses */
    qmi_pbm_if_get_pb_capabilities_resp_s           get_pb_caps_resp;
    qmi_pbm_if_get_all_pb_capabilities_resp_s       get_all_pb_caps_resp;
    qmi_pbm_if_read_records_resp_s                  read_recs_resp;
    qmi_pbm_if_read_pbset_caps_resp_s               read_pbset_caps_resp;    
    qmi_pbm_if_write_record_resp_s                  write_rec_resp;
    qmi_pbm_if_delete_record_resp_s                 delete_rec_resp;
    qmi_pbm_if_delete_all_pb_records_resp_s         delete_all_pb_recs_resp;
    qmi_pbm_if_search_records_resp_s                *search_recs_resp;
    qmi_pbm_if_get_emerg_list_resp_s                emerg_list_resp;
    qmi_pbm_if_get_pb_state_resp_s                  get_pb_state_resp;
    qmi_pbm_if_get_all_grp_resp_s                   *get_all_grp_resp;
    qmi_pbm_if_set_grp_info_resp_s                  set_grp_info_resp;
    qmi_pbm_if_read_all_hidden_recs_resp_s          read_all_hidden_recs_resp;
    qmi_pbm_if_get_next_empty_record_id_resp_s      nxt_empty_rec_id_resp;
    qmi_pbm_if_get_next_non_empty_record_id_resp_s  nxt_non_empty_rec_id_resp;
    qmi_pbm_if_get_all_aas_resp_s                   *get_all_aas_resp;
    qmi_pbm_if_set_aas_resp_s                       set_aas_resp;
    qmi_pbm_if_read_records_ext_undecoded_resp_s  read_recs_ext_undecoded_resp;

    /* indications */
    qmi_pbm_if_record_update_ind_s              rec_update_ind;
    qmi_pbm_if_refresh_ind_s                    refresh_ind;
    qmi_pbm_if_pb_ready_ind_s                   pb_ready_ind;
    qmi_pbm_if_sim_ready_ind_s                  sim_ready_ind;    
    qmi_pbm_if_emer_ind_s                       emerg_list_ind;
    qmi_pbm_if_all_pb_init_done_ind_s           all_pb_init_done_ind;
    qmi_pbm_if_internal_record_read_ind_s       intr_rec_read_ind;
    qmi_pbm_if_internal_pbset_caps_read_ind_s   intr_pbset_cap_read_ind;
    qmi_pbm_if_internal_record_read_ext_undecoded_ind_s  intr_rec_read_ext_undecoded_ind;
    qmi_pbm_if_record_read_ind_s                *rec_read_ind;
    qmi_pbm_if_record_read_ext_undecoded_ind_s  *rec_read_ext_ind;
    qmi_pbm_if_pbset_cap_read_ind_s             *pbset_cap_read_ind;
    qmi_pbm_if_hidden_record_status_ind_s       hidden_status_ind;
    qmi_pbm_if_internal_hidden_recs_read_ind_s  intr_hidd_rec_ind;
    qmi_pbm_if_internal_prim_subs_slot_ind_s    intr_prim_subs_slot_ind;
    qmi_pbm_if_aas_update_ind_s                 aas_update_ind;
    qmi_pbm_if_gas_update_ind_s                 gas_update_ind;


    /* Event callbacks */
    qmi_pbm_if_pbm_evt_cb_s    pbm_evt_cb;

    cm_mm_ss_info_s_type    pbm_ss_emer_evt_cb;
    mmgsdi_event_data_type    pbm_mmgsdi_emer_evt_cb;

    /* PBM command callback */
    qmi_pbm_if_pbm_write_cb_s    write_cb;
    qmi_pbm_if_mmgsdi_client_id_reg_cb_s    client_id_reg_cb;
  } data;
} qmi_pbm_if_info_item_s;



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

void qmi_pbm_if_init
(
  void
);

const char *qmi_pbm_if_lookup_cmd_name
(
  uint8 cmd
);

void qmi_pbm_encode_and_send_msg
       (qmi_pbm_if_info_item_s *     pbm_if_data_ptr);


/*===========================================================================
  FUNCTION qmi_pbm_if_qmi_session_to_pbm_session()

  DESCRIPTION
    Map the AMSS-QMI session type to the PBM session type.

  PARAMETERS
    ret        : pbm_return_type

  RETURN VALUE
    state of the phonebook pbm_session_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

pbm_session_enum_type qmi_pbm_if_qmi_session_to_pbm_session
(
  qmi_pbm_if_session_type qmi_sess
);

/* ---Indications--- */
void qmi_pbm_if_send_all_pb_init_done_ind
(
  uint8 sess_type
);

void qmi_pbm_if_internal_record_read_ind
(
  qmi_pbm_if_info_item_s  *const params_ptr
);
void qmi_pbm_if_evt_cb_handler
(
  pbm_notify_data_s_type *notify_data
);

/* ---PBM event callback--- */
void qmi_pbm_if_event_callback
(
  void *user_data,
  pbm_notify_data_s_type *notify_data
);

/* ---PBM write callback--- */
void qmi_pbm_if_write_callback
(
  pbm_writecb_data_s_type *cb_data
);

#endif /* _QMI_PBM_IF_H */

