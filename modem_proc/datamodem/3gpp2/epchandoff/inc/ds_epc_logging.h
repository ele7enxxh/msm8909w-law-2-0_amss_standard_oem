#ifndef DS_EPC_LOGGING_H
#define DS_EPC_LOGGING_H
/*===========================================================================

                      DS_EPC_LOGGING. H
                   
DESCRIPTION
  

EXTERNALIZED FUNCTIONS

 Copyright (c) 2009-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $Author: 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/07/14    sd    Changes to update EPC diag event EVENT_DS_EPC_PDN
                  to accommodate iWLAN_S2B.
04/14/09    sc    Created Module
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_EPC_HANDOFF
#include "comdef.h"
#include "customer.h"
#include "ps_iface.h"
//#include "ds_epc_hdlri.h"
#include "ds_epc_pdn_ctl.h"
#include "log_dmss.h"
#include "log.h"
#include "log_codes.h"

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/* will be defined in log_codes.h */
/*#define LOG_DS_EPC_PDN_INFO_C ((0x53B) + LOG_1X_BASE_C)*/

#define DS_EPC_PDN_INFO_LOG_VERSION        1

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*---------------------------------------------------------------------------
Struct: ds_epc_srat_cleanup_timer_struct
Event Name: EVENT_DS_EPC_SRAT_CLEANUP_TIMER_EVENT
Event Description: This event is triggered when the srat cleanup timer is 
                   started, stopped or expired. The payload will carry 
                   the timer state and the duration of the timer.
Event Fields Type: ds_epc_srat_cleanup_timer_enum_type (1 byte) and 
                   dword (4 bytes)
Event Payload Size: 5 bytes
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_SRAT_CLEANUP_EVENT_MIN =                0,
  DS_EPC_SRAT_CLEANUP_EVENT_TIMER_START =        DS_EPC_SRAT_CLEANUP_EVENT_MIN,
  DS_EPC_SRAT_CLEANUP_EVENT_TIMER_STOP =         1,
  DS_EPC_SRAT_CLEANUP_EVENT_TIMER_EXPIRY =       2,
  DS_EPC_SRAT_CLEANUP_EVENT_MAX
}ds_epc_srat_cleanup_timer_state_enum_type;

typedef struct
{
  uint32                                         timer_value;
  uint8                                          timer_state;
  uint8                                          epc_iface_instance;
} ds_epc_srat_cleanup_timer_event_type;

typedef PACK(struct)
{
  uint8                                          subs_id:4;
  uint8                                          reserved:4;
  uint32                                         timer_value;
  uint8                                          timer_state;
  uint8                                          epc_iface_instance;
}ds_epc_srat_cleanup_timer_ex_event_type; 

/*---------------------------------------------------------------------------
Struct: ds_epc_edct_timer_struct
Event Name: EVENT_DS_EPC_EDCT_EVENT
Event Description: This event is triggered when the epc data context timer is
                   started, stopped or expired. The payload will carry 
                   the timer state and the duration of the timer.
Event Fields Type: ds_epc_edct_enum_type (1 byte) and 
                   dword (4 bytes)
Event Payload Size: 5 bytes
---------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_EDCT_EVENT_MIN =                        0,
  DS_EPC_EDCT_EVENT_TIMER_START =                DS_EPC_EDCT_EVENT_MIN,
  DS_EPC_EDCT_EVENT_TIMER_STOP =                 1,
  DS_EPC_EDCT_EVENT_TIMER_EXPIRY =               2,
  DS_EPC_EDCT_EVENT_MAX                       
}ds_epc_edct_timer_state_enum_type;

typedef struct
{
  uint32                                         timer_value;
  uint8                                          timer_state;
} ds_epc_edct_timer_event_type;

typedef enum                                       
{                                                  
  DS_EPC_PDN_STATE_MIN =                         0,
  DS_EPC_PDN_STATE_DOWN =                        DS_EPC_PDN_STATE_MIN,
  DS_EPC_PDN_STATE_COMING_UP =                   1,
  DS_EPC_PDN_STATE_UP =                          2,
  DS_EPC_PDN_STATE_HANDING_OVER =                3,
  DS_EPC_PDN_STATE_GOING_DOWN =                  4,
  DS_EPC_PDN_STATE_RECONFIGURING =               5,
  DS_EPC_PDN_STATE_WAITING_FOR_SRAT_CLEANUP =    6,
  DS_EPC_PDN_STATE_CONFIGURING_ADDRESS =         7,
  DS_EPC_PDN_STATE_MAX 
}ds_epc_pdn_state_enum_type;                       

typedef enum
{
  DS_EPC_IFNAME_MIN =                          0,
  DS_EPC_IFNAME_INVALID =                      DS_EPC_IFNAME_MIN,
  DS_EPC_IFNAME_3GPP2_IFACE =                  1,
  DS_EPC_IFNAME_3GPP_IFACE =                   2,
  DS_EPC_IFNAME_EPC_IFACE =                    3,
  DS_EPC_IFNAME_IWLAN_S2B_IFACE =              4,
  DS_EPC_IFNAME_MAX
}ds_epc_ifname_enum_type;

typedef enum
{
  DS_EPC_CALL_TYPE_MIN =                         0,
  DS_EPC_CALL_TYPE_V4 =                          0,
  DS_EPC_CALL_TYPE_V6 =                          1,
  DS_EPC_CALL_TYPE_MAX
} ds_epc_call_type;

#define DS_EPC_PDN_EVENT_PDN_IP_TYPE_MAX         6

#define DS_EPC_PDN_EVENT_IFACE_INSTANCE_MAX      32

#define DS_EPC_LOGGING_MAX_APN_LEN               12
typedef struct                                               
{                                                                   
  struct                                                     
  {                                                                            
    unsigned int epc_iface_instance            :5;            
    unsigned int pdn_ip_type                   :2;
    unsigned int call_type                     :1;            
    unsigned int pdn_state                     :4;            
    unsigned int current_data_serving_system   :4;            
    unsigned int srat_iface_name               :3;            
    unsigned int srat_iface_instance           :5;            
    unsigned int trat_iface_name               :3;            
    unsigned int trat_iface_instance           :5;            
  } ds_epc_pdn_state;                                               
  char                                           apn_string[12];
}ds_epc_pdn_event_type;

typedef PACK(struct)                                              
{                                                                   
  PACK(struct)                                                     
  {                                                                            
    unsigned int subs_id                       :4;   
    unsigned int reserved                      :4;                                                                          
    unsigned int epc_iface_instance            :5;            
    unsigned int pdn_ip_type                   :2;
    unsigned int call_type                     :1;            
    unsigned int pdn_state                     :4;            
    unsigned int current_data_serving_system   :4;            
    unsigned int srat_iface_name               :3;            
    unsigned int srat_iface_instance           :5;            
    unsigned int trat_iface_name               :3;            
    unsigned int trat_iface_instance           :5;            
  } ds_epc_pdn_state;                                               
  char                                           apn_string[12];
}ds_epc_pdn_ex_event_type;                                             

/* CAUTION: API Type, Do not modify */
typedef enum
{
  DS_EPC_PDN_CONN_STATE_DISCONNECTED = 0,
  DS_EPC_PDN_CONN_STATE_BRINGUP_RT_POST_PROC = 1,
  DS_EPC_PDN_CONN_STATE_CONNECTED = 2,
  DS_EPC_PDN_CONN_STATE_HANDOFF_START = 3,
  DS_EPC_PDN_CONN_STATE_HANDOFF_STARTED = 4,
  DS_EPC_PDN_CONN_STATE_HANDOFF_SUCCESS = 5,
  DS_EPC_PDN_CONN_STATE_RESYNC_SRAT = 6,
  DS_EPC_PDN_CONN_STATE_EDCT_TIMER_START = 7,
  DS_EPC_PDN_CONN_STATE_EDCT_TIMER_STOP = 8,
  DS_EPC_PDN_CONN_STATE_EDCT_TIMER_EXPIRED = 9,
  DS_EPC_PDN_CONN_STATE_SRAT_CLEANUP_TIMER_START = 10,
  DS_EPC_PDN_CONN_STATE_SRAT_CLEANUP_TIMER_STOP = 11,
  DS_EPC_PDN_CONN_STATE_SRAT_CLEANUP_TIMER_EXPIRED = 12,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_APN_PDN_NO_MATCH = 13,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_SRAT_CONTEXT = 14,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_INIT = 15,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_SAME_SRAT_TRAT = 16,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_TRAT_DOWN = 17,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_IP6_IID_TRANS = 18,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_SWAP_RAT = 19,
  DS_EPC_PDN_CONN_STATE_HANDOFF_FAILURE_IP6_ADDR = 20,
} ds_epc_pdn_connection_state_type;


/* CAUTION: API Type, Do not modify */
/*---------------------------------------------------------------------------
  DS EPC PDN INFO
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_DS_EPC_PDN_INFO_C)
  uint8    pkt_version;                    /* 1   len = 1 */
  uint8    curr_rat;                       /* 1   len = 2 */
  uint8    pdn_rat;                        /* 1   len = 3 */
  uint8    connection_state;               /* 1   len = 4 */
  uint16   iface_name_epc;                 /* 2   len = 6 */
  uint8    iface_inst_epc;                 /* 1   len = 7 */
  uint8    iface_state_epc;                /* 1   len = 8 */
  uint16   iface_name_srat;                /* 2   len = 10 */
  uint8    iface_inst_srat;                /* 1   len = 11 */
  uint8    iface_state_srat;               /* 1   len = 12 */
  uint16   iface_name_trat;                /* 2   len = 14 */
  uint8    iface_inst_trat;                /* 1   len = 15 */
  uint8    iface_state_trat;               /* 1   len = 16 */
  uint8    pdn_call_type;                  /* 1   len = 17 */
  uint8    srat_bearer_type;               /* 1   len = 18 */
  uint16   reserved;                       /* 2   len = 20 */
  uint32   handoff_failure_trat_down_reason;/* 4   len = 24 */
  uint8    edct_timer_status;              /* 1   len = 25 */
  uint8    srat_cleanup_timer_status;      /* 1   len = 26 */
  uint16   edct_timer_remaining_value;     /* 2   len = 28 */
  uint16   srat_cleanup_timer_remaining_value;/* 2   len = 30 */
  uint16   profile_id_3gpp;                /* 2   len = 32 */
  uint16   profile_id_3gpp2;               /* 2   len = 34 */
  uint8    profile_pdp_type_3gpp;          /* 1   len = 35 */
  uint8    profile_pdp_type_3gpp2;         /* 1   len = 36 */
  uint8    profile_apn_len_3gpp;           /* 1   len = 37 */
  uint8    profile_apn_len_3gpp2;          /* 1   len = 38 */
  uint8    profile_apn_names[DS_PROFILE_3GPP_MAX_APN_STRING_LEN +
                             DS_PROFILE_3GPP2_APN_MAX_VAL_LEN +
                             2];  /* APN names, with NULL end */
LOG_RECORD_END


#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS_EPC_LOGGING_PDN_EVENT

DESCRIPTION   Logs DS_EPC_PDN_EVENT

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_logging_pdn_event
(
  ds_epc_cb_type*             ds_epc_cb_ptr,
	 ds_epc_pdn_state_enum_type  pdn_state
);

/*===========================================================================
FUNCTION      DS_EPC_LOGGING_SRAT_CLEANUP_TIMER_EVENT

DESCRIPTION   Logs DS_EPC_SRAT_CLEANUP_TIMER_EVENT

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_logging_srat_cleanup_timer_event
(
  ds_epc_srat_cleanup_timer_state_enum_type timer_state,
  uint32                                    duration,
  ps_iface_type*                            epc_iface_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_LOGGING_EDCT_TIMER_EVENT

DESCRIPTION   Logs DS_EPC_EDCT_TIMER_EVENT

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_epc_logging_edct_timer_event
(
  ds_epc_edct_timer_state_enum_type timer_state,
  uint32                            duration
);

/*===========================================================================
FUNCTION      DS_EPC_LOGGING_LOG_PDN_INFO

DESCRIPTION   This function logs PDN info packet

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_logging_log_pdn_info( ds_epc_cb_type       * epc_iface_ctrl_ptr,
                                  ds_epc_pdn_connection_state_type conn_state);


//extern void l2_log_state (byte sapi, byte l2_state, l2_event_T l2_event);

#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_LOGGING_H */
