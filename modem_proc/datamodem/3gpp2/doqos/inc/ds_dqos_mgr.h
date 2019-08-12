#ifndef DS_DQOS_MGR_H
#define DS_DQOS_MGR_H

/*===========================================================================

                        DYNAMIC QOS MANAGER

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds_dqos_mgr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/07/11    dvk    Merged the changes related to setting the dqos_state for 
                   ds_flows to deleting state while cleaning up QoS silently, 
                   if the tft_cb_ptr is deleted already as part of PPP 
                   RESYNC (HR->1X)
02/19/09    sk     eHRPD: UE initiated QoS
09/14/07    az     Created a new API to inform Sec Pkt Mgr that flows 
                   need to be suspended
05/08/06    az     Changes for code re-structuring so that failure cases 
                   in request/release of QoS are handled appropriately
03/16/06    az     Changed the RESV message retransmit timeout value from
                   25 seconds to 3 seconds.
12/08/05    msr    Code clean up
11/29/05   az/msr  Initial version

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_HDR_QOS

#include "timer.h"
#include "ds_tft_bldr.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nw_init_qos_mgr.h"
#include "ps_rsvp.h"

/*===========================================================================

                           EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Maximum number of TFT control blocks supported in AMSS. There should be
  one TFT CB for each iface on which AT supports QoS. 
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_MAX_TFT_CB  DS707_MAX_IFACES

/*---------------------------------------------------------------------------
  AT can keep track of these many outstanding RESV msgs for each TFT CB, one
  RESV msg for each ds_flow

  PLEASE DO NOT CHANGE THIS VALUE. THERE MAY BE ASSUMPTIONS IN CODE ABOUT
  THESE TWO BEING THE SAME
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_RESV_MSG_MAX_PENDING  DS707_MAX_DS_FLOWS

/*---------------------------------------------------------------------------
  Lower and Upper bounds on transaction ID sent in each RESV msg.
  Transaction ID is 32 bits and is defined in IS-835D

  Since AT can have only one outstanding RESV msg for a ds_flow, it needs to
  check if AT already has an outstanding RESV msg. If ts_id for a ds_flow is
  0, it means that there is no outstanding RESV msg for this ds_flow
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_3GPP2_OBJ_MIN_TS_ID      1
#define DS_DQOS_MGR_3GPP2_OBJ_MAX_TS_ID      0xEFFFFFFFUL
#define DS_DQOS_MGR_3GPP2_OBJ_INVALID_TS_ID  0

/*---------------------------------------------------------------------------
  Lower and Upper bounds on timer ID, which is used to uniquely identify
  a timer
  Refer to "rexmit_timer_handle" in "ds_dqos_mgr_pending_resv_msg_info_type"
  for more details
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_MIN_REXMIT_TIMER_ID  0
#define DS_DQOS_MGR_MAX_REXMIT_TIMER_ID  0x00FFFFFFUL

/*---------------------------------------------------------------------------
  Value of TIME_VALUE object of RESV message (IS835-D defined value)
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_RSVP_TIME_VALUE  0xFFFFFFFFUL

/*---------------------------------------------------------------------------
  Maximum # of filters that can be sent in one 3GPP2 object. This is a
  implementaion defined value. Each V4 filter takes up to 38 bytes and
  RESV msg is limited by MTU
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_3GPP2_OBJ_MAX_FLTRS  32


/*---------------------------------------------------------------------------
  The following definitions are eHRPD specific

  The QoS Check contains filters plus the sub blob. 
    - V4 filter can take up to 38 bytes
    - Each flow can have 7 profiles. The max size of blob (verbose mode) is
      30 bytes each direction.
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_3GPP2_OBJ_V4_FLTRS_SIZE   38
#define DS_DQOS_MGR_MAX_PROFILE_SIZE          30
#define DS_DQOS_MGR_3GPP2_OBJ_MAX_SIZE        1216

#define DS_DQOS_MGR_3GPP2_OBJ_MAX_LEN  2600

/*---------------------------------------------------------------------------
  Time in ms for which mobile waits for PDSN's RESV response before retrying
  RESV msg again
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_RESV_RESP_TIMEOUT_VAL  3000

/*---------------------------------------------------------------------------
  Maximum # of times a RESV msg is trasmitted to PDSN before bailing out
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_RESV_MSG_MAX_XMITS  3

/*---------------------------------------------------------------------------
  Src/Dst port used for RSVP communication (IS-835D defined)
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_RSVP_PORT  (dss_htons(3455))


/*---------------------------------------------------------------------------
  Events that are posted upon completion or beginning of TFT operation. 
  RESV_MSG_SUCCEEDED_EV is posted when a TFT operation is successful
  RESV_MSG_FAILED_EV is posted when a TFT operation is not successful
  RESV_MSG_INSTALLING_EV is posted when filters are in the process of installing
  RESV_MSG_TIMEOUT_EV is posted when time out occurs
---------------------------------------------------------------------------*/
#define DS_DQOS_MGR_RESV_MSG_SUCCEEDED_EV 0
#define DS_DQOS_MGR_RESV_MSG_FAILED_EV 1
#define DS_DQOS_MGR_RESV_MSG_INSTALLING_EV 2
#define DS_DQOS_MGR_RESV_MSG_TIMEOUT_EV 3

/*---------------------------------------------------------------------------
  Struct used by RSVP cmd callback function to notify DS of RESV response
---------------------------------------------------------------------------*/
typedef struct
{
  void                     * user_data_ptr;
  int32                      sid;
  uint32                     msg_id;
  ps_rsvp_event_enum_type    event;
} ds_dqos_mgr_resv_resp_notify_cmd_type;


/*---------------------------------------------------------------------------
  State related to RESV msg sent to PDSN but for which we haven't received
  response yet

  Fields are:
    ds_flow_ptr_list    : list of ds_flows which are manipulated in this msg
    rexmit_timer        : timer which is used to trigger re-transmission
    rexmit_timer_handle : handle which uniquely identifies "rexmit_timer".
                          First 8 bits indicate the TFT CB's instance and
                          last 24 bits constitute "timer_id" which is defined
                          above. Embedding TFT CB instance in handle minimizes
                          amount of traversal that is required to identify
                          "resv_msg_info" given a handle
    ts_id               : transaction ID used in this msg
    tft_opcode          : operation performed in this msg
    num_xmits           : number of times, this msg has been sent to PDSN so
                          far
---------------------------------------------------------------------------*/
typedef struct
{
  list_link_type     link;
  ds707_flow_type  * ds_flow_ptr;
} ds_dqos_mgr_ds_flow_ptr_list_type;

typedef struct
{
  list_link_type               link;
  list_type                    ds_flow_ptr_list;
  timer_type                   rexmit_timer;
  timer_cb_data_type           rexmit_timer_handle;
  uint32                       ts_id;
  ds_tft_bldr_tft_opcode_type  tft_opcode;
  uint8                        num_xmits;
} ds_dqos_mgr_pending_resv_msg_info_type;

/*---------------------------------------------------------------------------
  TFT states
  TFT always starts from TFT_NULL state
---------------------------------------------------------------------------*/
typedef enum
{
  DS_DQOS_MGR_TFT_STATE_MIN   = 0,
  DS_DQOS_MGR_TFT_NULL        = DS_DQOS_MGR_TFT_STATE_MIN,
  DS_DQOS_MGR_TFT_CREATING    = 1,
  DS_DQOS_MGR_TFT_CREATED     = 2,
  DS_DQOS_MGR_TFT_GOING_NULL  = 3,
  DS_DQOS_MGR_TFT_STATE_MAX
} ds_dqos_mgr_tft_state_enum_type;

/*---------------------------------------------------------------------------
  TFT control block definition
  Fields are
    ps_iface_ptr         : iface to which this TFT corresponds to
    resv_msg_info_list   : list of RESV msgs for which response isn't received
                           yet from PDSN
    rsvp_session_id      : ID of RSVP session associated with this TFT
    num_active_qos_flows : number of QoS flows still being serviced by TFT
                           This variable is incremented whenever
                           dqos_request() is called and is decremented
                           whenever dqos_release() is called
    instance             : instance of this TFT
    is_available         : indicated if this TFT CB is already in use
    state                : TFT CB's stae
    tft_ie_type          : IE type sent in each RESV msg for this TFT CB
---------------------------------------------------------------------------*/
struct ds_dqos_mgr_tft_cb_s
{
  ps_iface_type                    * ps_iface_ptr;
  list_type                          resv_msg_info_list;
  int32                              rsvp_session_id;
  int32                              rsvp_hsgw_session_id; 
  uint8                              num_active_qos_flows;
  uint8                              instance;
  boolean                            is_available;
  ds_dqos_mgr_tft_state_enum_type    state;
  ds_tft_bldr_tft_ie_type            tft_ie_type;
};

typedef struct ds_dqos_mgr_tft_cb_s ds_dqos_mgr_tft_cb_type;

/*--------------------------------------------------------------------
  Events that are posted upon completion of TFT operation.
----------------------------------------------------------------------*/
typedef enum
{
  DS_DQOS_MGR_FLTR_INSTALL_SUCCEEDED_EV   = 0,
  DS_DQOS_MGR_FLTR_RELEASE_SUCCEEDED_EV   = 1,

  DS_DQOS_MGR_FLTR_OPERATION_FAILED_EV    = 2,

  DS_DQOS_MGR_FLTR_INSTALLING_EV          = 3,
  DS_DQOS_MGR_QOS_CHECK_SUCCEEDED_EV      = 4,
  DS_DQOS_MGR_QOS_CHECK_FAILED_EV         = 5,
  DS_DQOS_MGR_QOS_CHECK_TIMEOUT_EV         = 6,
  DS_DQOS_MGR_FLTR_OPERATION_TIMED_OUT_EV    = 7,
  DS_DQOS_MGR_MAX_EV

} ds_dqos_mgr_event_enum_type;


/*---------------------------------------------------------------------------
TYPEDEF DS_DQOS_MGR_EVENT_CBACK_TYPE

DESCRIPTION
  Defines prototype for event callback function

PARAMETERS
  ds_flow_ptr_arr : array of ds_flow_ptrs
  num_ds_flow_ptr : number of ds_flow_ptrs in above array
  event           : the event for which callbacks need to be called
  user_data_ptr   : data to be passed back to the user
---------------------------------------------------------------------------*/
typedef void (* ds_dqos_mgr_event_cback_type)
(
  ds707_flow_type             ** ds_flow_ptr_arr,
  uint8                          num_ds_flow_ptr,
  ds_dqos_mgr_event_enum_type    event,
  ps_iface_type                * ps_iface_ptr

);

/*---------------------------------------------------------------------------
  Type used to register events with DQOS module
---------------------------------------------------------------------------*/
typedef struct
{
  void                          * user_data_ptr;
  ds_dqos_mgr_event_cback_type    event_cback_f_ptr;
} ds_dqos_mgr_event_buf_type;



/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION DS_DQOS_MGR_INIT()

DESCRIPTION
  Initializes DQOS module

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_init
(
  void
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_CREATE_GLOBAL_FILTER()

DESCRIPTION
    Creates Globlal RSVP Filter as part of powerup

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_create_global_filter
(
   void
);

/*===========================================================================
FUNCTION ds_dqos_mgr_process_iface_up_event()

DESCRIPTION

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_process_iface_up_event(
  
  ps_iface_type               * this_iface_ptr,
  ps_iface_event_info_u_type    event_info

  );

/*===========================================================================
FUNCTION DS_DQOS_MGR_GET_DS_FLOWS_FOR_IFACE()

DESCRIPTION
  Given a ps_iface_ptr, returns the number of valid ds_flows.

PARAMETERS
  ds_flow_output_arr : an array of ds_flow ptrs
  ps_iface_ptr       : ps_iface_ptr in which caller is interested in
  ds_flow_input_arr  : an array of ds_flow ptrs
  num_ds_flow_ptr    : number of ds_flow ptrs

RETURN VALUE
  Number of DS flows

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 ds_dqos_mgr_get_ds_flows_for_iface(
/* This is an OUTPUT parameter */
  ds707_flow_type         ** ds_flow_output_arr,
  /* These are INPUT parameters */
  ps_iface_type            * ps_iface_ptr,
  ds707_flow_type         ** ds_flow_input_arr,
  uint8                      num_ds_flow_ptr
);

/*===========================================================================
FUNCTION  DS_DQOS_MGRI_GET_TFT_CB_PTR()

DESCRIPTION
  Given a ps_iface_ptr, returns corresponding TFT CB

PARAMETERS
  ps_iface_ptr : ps_iface_ptr in which caller is interested in

RETURN VALUE
  ptr to TFT CB : on success
  NULL          : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds_dqos_mgr_tft_cb_type *ds_dqos_mgri_get_tft_cb_ptr(

  ps_iface_type  * ps_iface_ptr
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_CREATE_TFT_CB()

DESCRIPTION
  Creates a TFT control block to manage TFT associated with a particular IP
  address of MS

PARAMETERS
  ps_iface_ptr : iface for which TFT needs to be created

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  A RSVP session is created for this TFT
===========================================================================*/
ds_dqos_mgr_tft_cb_type * ds_dqos_mgr_create_tft_cb
(
  ps_iface_type  * ps_iface_ptr
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_DELTE_RSVP_SESSIONS()

DESCRIPTION
  Deletes open RSVP sessions that are associated with the control block that
  is passed.

PARAMETERS
  tft_cb_ptr : tft control block pointer with which the sessions are
               associated
 
RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  A RSVP session is created for this TFT
===========================================================================*/
int ds_dqos_mgr_delete_rsvp_sessions
(
  ds_dqos_mgr_tft_cb_type  *tft_cb_ptr
);
/*===========================================================================
FUNCTION DS_DQOS_MGR_DELETE_TFT_CB()

DESCRIPTION
  Deletes a TFT control block which was created to manage TFT associated
  with a particular IP address of MS

PARAMETERS
  ps_iface_ptr : iface whose corresponding TFT needs to be deleted

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  RSVP session associated with TFT is deleted
===========================================================================*/
int32 ds_dqos_mgr_delete_tft_cb
(
  ps_iface_type  * ps_iface_ptr
);

#ifdef FEATURE_EHRPD
/*===========================================================================
FUNCTION        DS_DQOS_MGR_QOS_CHECK()

DESCRIPTION     Sends RESV message to HSGW

PARAMETERS      ds_flow_ptr_arr : an array of ds_flow ptrs
                num_ds_flow_ptr : number of ds_flow ptrs in the above array

RETURN VALUE
                 0 : on success
                -1 : on failure

DEPENDENCIES
  Only those "ds_flows" that belong to a single "ps_iface" must be used
  as argument. Client can't mix one "ds_flows" from different ifaces

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_dqos_mgr_qos_check(
  ds707_flow_type  ** ds_flow_ptr_arr,
  uint8               num_ds_flow_ptr
);
#endif
/*===========================================================================
FUNCTION DS_DQOS_MGR_QOS_REQUEST()

DESCRIPTION
  Sends filters to PDSN

PARAMETERS
  ds_flow_ptr_arr : an array of ds_flow ptrs
  num_ds_flow_ptr : number of ds_flow ptrs in the above array
  is_ue_force_op  : indicate if it is ue force operation 

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_dqos_mgr_qos_request
(
  ps_iface_type     * ps_iface_ptr,
  ds707_flow_type  ** ds_flow_ptr_arr,
  uint8               num_ds_flow_ptr,
  boolean             is_ue_force_op
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_QOS_RELEASE()

DESCRIPTION
  Removes filters from PDSN

PARAMETERS
  ds_flow_ptr_arr : an array of ds_flow ptrs
  num_ds_flow_ptr : number of ds_flow ptrs in the above array
  is_ue_force_op  : indicate if it is ue force operation 

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_dqos_mgr_qos_release
(
  ds707_flow_type  ** ds_flow_ptr_arr,
  uint8               num_ds_flow_ptr,
  boolean             is_ue_force_op
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_QOS_CLEAN_UP()

DESCRIPTION
  Cleans resources locally without sending RESV DELETE to PDSN. This function
  must only be used when AT is in QOS UNAWARE system

PARAMETERS
  ds_flow_ptr_arr : an array of ds_flow ptrs
  num_ds_flow_ptr : number of ds_flow ptrs in the above array

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_qos_clean_up
(
  ds707_flow_type  ** ds_flow_ptr_arr,
  uint8               num_ds_flow_ptr
);

/*===========================================================================
FUNCTION  DS_DQOS_MGR_PROCESS_RSVP_MSG_CMD

DESCRIPTION
  Processes RSVP msg from PDSN

PARAMETERS
  ds_cmd_ptr : DS cmd that has info about PDSN's response

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  DS_DQOS_MGR_RESV_MSG_SUCCEEDED_EV is posted if RESV CONF is received and
  DS_DQOS_MGR_RESV_MSG_FAILED_EV is posted if RESV ERR is received.
  In certain cases AT may try to get in sync with PDSN if it thinks that
  AT and PDSN are not in sync
===========================================================================*/
void ds_dqos_mgr_process_rsvp_msg_cmd
(
  ds_cmd_type  * ds_cmd_ptr
);

/*===========================================================================
FUNCTION  DS_DQOS_MGR_PROCESS_REXMIT_TIMER_CMD()

DESCRIPTION
  Processes expiry of re-transmission timer. Based on TFT state and number
  of times a msg is already transmitted, this function either retransmits
  again or bails out

PARAMETERS
  user_data : data passed back by timer

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  DS_DQOS_MGR_RESV_MSG_FAILED_EV is posted if AT's retransmission mechanism
  failed. In certain cases AT may try to get in sync with PDSN if it thinks
  that AT and PDSN are not in sync
===========================================================================*/
void ds_dqos_mgr_process_rexmit_timer_cmd
(
  ds_cmd_type  * ds_cmd_ptr
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_REG_EV_CBACK()

DESCRIPTION
  Registers an event callback with DQOS module

PARAMETERS
  event             : event in which client is intereseted in
  event_cback_f_ptr : callback function to be registered
  user_data_ptr     : data to be passed back to callback function

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_dqos_mgr_reg_ev_cback
(
  ds_dqos_mgr_event_enum_type     event,
  ds_dqos_mgr_event_cback_type    event_cback_f_ptr,
  void                          * user_data_ptr
);

/*===========================================================================
FUNCTION DS_DQOS_MGR_DEREG_EV_CBACK()

DESCRIPTION
  Deregisters an event callback with DQOS module

PARAMETERS
  event : event in which client is intereseted in

RETURN VALUE
   0 : on success
  -1 : on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_dqos_mgr_dereg_ev_cback
(
  ds_dqos_mgr_event_enum_type  event
);

#ifdef FEATURE_DATA_PS_IPV6 
/*===========================================================================
FUNCTION ds_dqos_mgr_process_ipv6_prefix_update_event()

DESCRIPTION  This module handles change of IPv6 Prefix.

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_process_ipv6_prefix_update_event(
  ps_iface_type               * this_iface_ptr,
  ps_iface_event_info_u_type    event_info
  );
#endif /* #ifdef FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION DS_DQOS_MGRI_PROCESS_HANDOFF_EVENT()

DESCRIPTION

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_process_handoff_event(
  
  /* number of flow labels released while on an unaware system */
  uint8 label_count

  );


/*===========================================================================
FUNCTION       DS_DQOS_MGR_SEND_DELETE_TFT

DESCRIPTION    
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dqos_mgr_send_delete_tft(

  ps_iface_type  * ps_iface_ptr

  );


/*===========================================================================
FUNCTION ds_dqos_mgr_process_iface_unavailable_event()

DESCRIPTION

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgr_process_iface_unavailable_event(
  ps_iface_type               * this_iface_ptr,
  ps_iface_event_info_u_type    event_info

);

/*===========================================================================
FUNCTION DS_DQOS_MGR_GET_DS_FLOWS_PTR ()

DESCRIPTION 
  Returns the pointer to global variable ds_dqos_mgr_ds_flows

RETURN VALUE
  ds707_flow_type ** - pointer to global variable ds_dqos_mgr_ds_flows

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ds707_flow_type ** ds_dqos_mgr_get_ds_flows_ptr( void );
/*===========================================================================
FUNCTION  DS_DQOS_MGRI_PROC_RSVP_hsgw_MSG_CB()

DESCRIPTION
  Posts a cmd to DS on receiving RSVP message from PDSN on a RSVP session
  with HSGW IP address

PARAMETERS
  sid           : session ID
  user_data_ptr : user data passed back by PS
  event         : type of PDSN's response
  obj_mask      : mask indicating what objects are present in PDSN's response
  msg_id        : PDSN's message ID. Used to fetch objects from the response

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgri_proc_rsvp_hsgw_msg_cb
(
  int32                      sid,
  void                     * user_data_ptr,
  ps_rsvp_event_enum_type    event,
  uint32                     obj_mask,
  uint32                     msg_id
);
/*===========================================================================
FUNCTION  DS_DQOS_MGRI_PROC_RSVP_MSG_CB()

DESCRIPTION
  Posts a cmd to DS on receiving RSVP message from PDSN on a RSVP session
  with UE IP address

PARAMETERS
  sid           : session ID
  user_data_ptr : user data passed back by PS
  event         : type of PDSN's response
  obj_mask      : mask indicating what objects are present in PDSN's response
  msg_id        : PDSN's message ID. Used to fetch objects from the response

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_dqos_mgri_proc_rsvp_msg_cb
(
  int32                      sid,
  void                     * user_data_ptr,
  ps_rsvp_event_enum_type    event,
  uint32                     obj_mask,
  uint32                     msg_id
);

#ifdef FEATURE_NIQ_EHRPD

/*===========================================================================
FUNCTION      DS_DQOS_MGR_NW_CREATE_APP_QOS

DESCRIPTION   Call clean_qos_pkg_memory after using this function to free the 
              memory used by the qos_package. This function converts a recently
              unpacked ds707_nw_init_3gpp2_obj into a package that contains an array
              of qos_spec_types that can be passed on to create flows.

DEPENDENCIES  Valid ds707_nw_init_3gpp2_obj.

RETURN VALUE  Pointer to a dynamically sized qos_package structure.

SIDE EFFECTS  Uses memory for the structure, must be freed.
===========================================================================*/
ds707_nw_init_qos_rcvd_resv_msg_type * ds_dqos_mgr_nw_create_app_qos
(
  ds707_nw_init_3gpp2_obj                    * object,
  ds_tft_bldr_tft_error_code             error_code
);

/*===========================================================================
FUNCTION      DS_DQOS_MGR_CLEAN_QOS_PKG_MEMORY

DESCRIPTION   Frees all memory allocated for a qos_package structure.

DEPENDENCIES  Assumes the package has been initialized properly in the 
              nw_create_app_qos function 

RETURN VALUE  None

SIDE EFFECTS  Will crash if unused pointers are not properly initialized to 
              NULL. 
===========================================================================*/
void ds_dqos_mgr_clean_qos_pkg_memory
(
  ds707_nw_init_qos_rcvd_resv_msg_type * dqos_pkg_ptr
);

/*===========================================================================
FUNCTION      DS_DQOS_MGR_IS_MATCHING_PACKET

DESCRIPTION   This function compares two ds707_nw_init_qos_rcvd_resv_msg_type 
              structures that have been properly initialized. The first parameter
              should be the original packet, and the second should contain the
              new packet. The function will determine if the packets contain
              the same information

DEPENDENCIES  Properly initialized ds707_nw_init_qos_rcvd_resv_msg_type structures.

RETURN VALUE  TRUE if the packets contain the same information. 
              FALSE if they contain >1 differences, or if there is an error
                    during the comparison.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dqos_mgr_is_matching_packet
(
  ds707_nw_init_qos_rcvd_resv_msg_type * new_packet,
  ds707_nw_init_qos_rcvd_resv_msg_type * original_packet
);

/*===========================================================================
FUNCTION  DS_DQOS_MGR_SEND_RESV_ERR_MSG()

DESCRIPTION
  Builds a 3GPP2 object and sends a RESV_ERR msg to PDSN

PARAMETERS
  tft_cb_ptr      : TFT CB for which RESV msg needs to be sent
  transaction_id  : Transaction ID to be used in RESV msg
  error_code      : Error code
  ds_errno        : error code returned in case of failure

RETURN VALUE
   0 : on success
  -1 : on failure
===========================================================================*/
int32 ds_dqos_mgr_send_resv_err_msg(
  ps_iface_type                *ps_iface_ptr,
  uint32                        transaction_id,
  ds_tft_bldr_tft_error_code    error_code,
  int16                        *ds_errno
);

/*===========================================================================
FUNCTION      CONTAINS_MATCHING_FILTERS

DESCRIPTION   This function compares one ds707_nw_init_qos_resv_flow_type structure 
              with a list of structures. If it finds a match it will return
              immediately, if no match is found it will iterate through the
              entire list. 

DEPENDENCIES  None.

RETURN VALUE  TRUE if no match is found
              FALSE if there is a match

SIDE EFFECTS  If a match is found, match_index will be updated to point to 
              the index in the flow list where the match occurred. 
===========================================================================*/
boolean ds_dqos_mgr_contains_matching_filters 
(
  ds707_nw_init_qos_resv_flow_type    new_flow,
  ds707_nw_init_qos_resv_flow_type  * original_flow_list,
  uint8                         num_flows,
  uint8                       * match_index,
  uint8                         curr_index
);

#endif /*FEATURE_NIQ_EHRPD*/

#endif /* FEATURE_HDR_QOS */
#endif /* DS_DQOS_MGR_H */
