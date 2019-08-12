#ifndef HDRPACQNMI_H
#define HDRPACQNMI_H
/*===========================================================================

            HDR QOS NEGOTIATION MANAGER INTERNAL

GENERAL DESCRIPTION
  This contains the declarations for the HDR QOS Negotiation Manager.

EXTERNALIZED FUNCTIONS
  hdrpacqnmi_init()
    Called only once at mobile power-up to initialize variables and 
    data structures.

  hdrpacqnmi_process_qos_req_status_ev()
    State machine event handling when a qos request's status has been
    accepted or rejected.

  hdrpacqnmi_process_qos_resp_ev()
    State machine event handling when a qos response has arrived specifying
    outcome of qos request.

  hdrpacqnmi_process_resv_bound_ev()
    State machine event handling when a reservation has been bound to a
    particular RLP flow.

  hdrpacqnmi_process_resv_unbound_ev()
    State machine event handling when a reservation has been unbound from a
    particular RLP flow.

  hdrpacqnmi_process_rlp_active_ev()
    State machine event handling when an RLP is activated.

  hdrpacqnmi_process_rlp_deactive_ev()
    State machine event handling when an RLP is deactivated.

  hdrpacqnmi_process_rlp_bound_ev()
    State machine event handling when an RLP is bound to an RTCMAC flow

  hdrpacqnmi_process_rlp_unbound_ev()
    State machine event handling when an RLP is unbound from an RTCMAC flow

  hdrpacqnmi_process_rtcmac_activated_ev()
    State machine event handling when an RTCMAC is activated

  hdrpacqnmi_process_rtcmac_deactivated_ev()
    State machine event handling when an RTCMAC is deactivated

  hdrpacqnmi_release_flow_resources()
    This is a cleanup method used to release the resources for a particular 
    flow.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrpacqnmi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
03/26/09   sk      eHRPD: UE initiated QoS multi pdn support
02/19/09   sk      eHRPD: UE initiated QoS
11/15/07   sk      Handle QoS modify Reject
05/03/07   sk      Support to process bundle response message from AN
07/06/06   sk      QoS 2.0 Support
05/23/06   spn     The secondary flows are bounded by (DS707_N_RESV_MAX_FWD-1)
                   instead of DS707_N_RESV_MAX_FWD. Similarly for Reverse 
                   direction also. Did the required changes.
03/29/06   spn     Code Cleanup: Removed Redundant Variables
03/13/06   spn     Changed the Array size to account for RLP Flow Numbers 
                   instead of the number of RLPs
11/11/05   spn     Changed the Data Structure which represents the internal
                   state of the QNM. 
10/24/05   spn     Modified the code for revA
06/30/05   atp     Created module.

===========================================================================*/

/*===========================================================================
                      INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "IxErrno.h"
#include "ds707.h"

#ifdef FEATURE_HDR_QOS
#include "ps_qos_defs.h"
#include "hdrpacrlpctl.h"
#include "ds707_qosdefs.h"
#include "dsrlp_v.h"

#include "hdrcom_api.h"
#include "hdrpacqnm.h"


/*===========================================================================
      Constants & Macros
===========================================================================*/

/*===========================================================================
      Typedefs
===========================================================================*/
/*---------------------------------------------------------------------------
  Struct holding misc info about qnm such as callbacks, etc.
---------------------------------------------------------------------------*/
typedef struct
{
  hdrpacqnm_qos_status_notify_cb_type qos_status_notify_cb;
  hdrpacqnm_resv_bound_cb_type resv_bound_notification_cb;
  hdrpacqnm_resv_unbound_cb_type resv_unbound_notification_cb;
} hdrpacqnm_info_type;


/*---------------------------------------------------------------------------
  States of QNM.

  HDRPACQNM_QOS_REQ_PENDING --> When a QoS Request is made, the 
      state transitions to QoS Req Pending
  HDRPACQNM_QOS_REQ_ACCEPTED --> If we receive a GAUP Accepted the
      state transitions to QoS Req Accepted
  HDRPACQNM_QOS_REQ_PENDING_GRANTED --> If we receive a Response
      before we receive a GAUP Accept from the AN, we transition to an 
      intermediate state.
  HDRPACQNM_QOS_REQ_GRANTED --> Once we receive a non-null profile
      ID from the AN in QoS Response
  HDRPACQNM_QOS_REL_PENDING --> Once we send a NULL KKQoSRequest
---------------------------------------------------------------------------*/
typedef enum
{
  HDRPACQNM_NONE,
  HDRPACQNM_QOS_REQ_PENDING,
  HDRPACQNM_QOS_REQ_PENDING_GRANTED,
  HDRPACQNM_QOS_REQ_ACCEPTED,
  HDRPACQNM_QOS_REQ_GRANTED,
  HDRPACQNM_QOS_REL_PENDING,
  HDRPACQNM_QOS_MOD_PENDING,
  HDRPACQNM_QOS_MOD_PENDING_GRANTED,
  HDRPACQNM_QOS_MOD_REJECTED
} hdrpacqnm_resv_state_enum;

/*---------------------------------------------------------------------------
  Information for the Sub Blob
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 start_set_id;           /* The Starting SetID for the QoS Request  */        
  uint8 end_set_id;             /* The End SetID for the QoS Request       */        
  ip_flow_type   *ip_flow[DS707_MAX_PROFILE_PER_FLOW];
  uint8 ip_flow_count;
} hdrpacqnm_sub_blob_info_type;

/*---------------------------------------------------------------------------
  Information for each Reservation Label
---------------------------------------------------------------------------*/
typedef struct
{
  boolean allocated;                   /* Entry is in use                  */
  uint8   resv_label;                  /* reservation label                */
  hdrpacqnm_resv_state_enum  state;    /* State of reservation             */
  uint8 granted_setid;            /* Index to qos choice the AN granted    */ 
  boolean is_resv_bound;          /* Is reservation bound to active RLP yet*/
  uint8 rlp_id;                   /* RLP id to which resv is bound         */
  hdrpacqnm_sub_blob_info_type sub_blob_info; /*Info about sub-blob        */
  uint8 transaction_id;         /* Transaction id returned by hdrmrlpc     */
} hdrpacqnm_resv_table_entry_type;


/*---------------------------------------------------------------------------
  Information for each RLP 
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean is_active; /* Specifies if the rlp is active or not    */ 
#ifdef FEATURE_HDR_REVA_L1
  boolean is_bound;  /* Specifies if the rlp is bound or not     */ 
  uint8 rtcmac_id;   /* Specifies the RMAC to which RLP is bound */ 
#endif /* FEATURE_HDR_REVA_L1 */
} hdrpacqnm_rlp_info_type;

/*---------------------------------------------------------------------------
  State information of QNM as a whole
---------------------------------------------------------------------------*/
typedef struct
{
  hdrpacqnm_info_type info;  /* Handle to the callbacks registered      */
  hdrpacqnm_resv_table_entry_type fwd_resv_table[DS707_N_RESV_MAX_FWD-1];
                             /* Current state of the allowed Forward 
                                Reservations.                           */
  hdrpacqnm_resv_table_entry_type rev_resv_table[DS707_N_RESV_MAX_REV-1];
                             /* Current state of the allowed Reverse 
                                Reservations.                           */
  hdrpacqnm_rlp_info_type fwd_rlp_table [DSRLP_MAX_HDR_MRLP_FLOW_NUMBER];
                             /* Current state of the allowed Forward 
                                RLP                                     */
  hdrpacqnm_rlp_info_type rev_rlp_table [DSRLP_MAX_HDR_MRLP_FLOW_NUMBER];
                             /* Current state of the allowed Reverse 
                                RLP                                     */
#ifdef FEATURE_HDR_REVA_L1
  boolean rtcmac_active_table [HDRL1_RMAC3_MAX_NUM_MAC_FLOWS ];
                             /* Current state of the allowed RMAC       */ 
#endif /* FEATURE_HDR_REVA_L1 */
} hdrpacqnm_state_info_type;



/*===========================================================================
      Variables
===========================================================================*/

/*===========================================================================
      Functions
===========================================================================*/

/*===========================================================================
FUNCTION      GET_HDRPACQNM_STATE_PTR

DESCRIPTION   Accessor function for hdrpacqnm_state
  
DEPENDENCIES  None

RETURN VALUE  Address of hdrpacqnm_state

SIDE EFFECTS  None
===========================================================================*/
hdrpacqnm_state_info_type* get_hdrpacqnm_state_ptr 
(
  void
);

/*===========================================================================

FUNCTION      HDRPACQNMI_INIT

DESCRIPTION   Called only once at mobile power-up to initialize variables
              and data structures.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type hdrpacqnmi_init ( void );

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_QOS_REQ_STATUS_EV

DESCRIPTION   State machine event handling when a qos request's status has
              been accepted or rejected.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_qos_req_status_ev
(
  uint8 trans_id,                        /* Transaction id                 */
  ds707_qos_status_enum_type gaup_ind   /* GAUP indication                */
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_QOS_RESP_EV

DESCRIPTION   State machine event handling when a qos response is received
              telling outcome of qos request.

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_qos_resp_ev
(
  uint8 resv_label,                      /* Reservation label              */
  ds707_direction_enum_type dirn,        /* Reservation Direction          */
  hdrcp_mrlpc_res_qos_profile_config_type *profile_config /* Profile Configuration */
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RESV_BOUND_EV

DESCRIPTION   State machine event handling when a reservation is bound to
              an active RLP.

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_resv_bound_ev
(
  uint8 resv_label_count,
  uint8 resv_label[],                    /* Reservation label              */
  ds707_direction_enum_type dirn,        /* Reservation direction          */ 
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver             /* RLP Version                    */
);
  
/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RESV_UNBOUND_EV

DESCRIPTION   State machine event handling when a reservation is unbound 
              from an RLP.

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_resv_unbound_ev
(
  uint8 resv_label_count,                /* Reservation label              */
  uint8 resv_labels[],
  ds707_direction_enum_type dirn,        /* Reservation direction          */ 
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver             /* RLP Version                    */
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RLP_ACTIVE_EV

DESCRIPTION   State machine event handling when an RLP is activated.

DEPENDENCIES  Assumes that the rlp_id passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_rlp_active_ev
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver                /* RLP version                    */
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RLP_DEACTIVE_EV

DESCRIPTION   State machine event handling when an RLP is deactivated.

DEPENDENCIES  Assumes that the rlp_id passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_rlp_deactive_ev
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver                /* RLP version                    */
);

#ifdef FEATURE_HDR_REVA_L1

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RLP_BOUND_EV

DESCRIPTION   State machine event handling when an RLP is 
              bound to an RTCMAC

DEPENDENCIES  If this function is called, then it is assumed that REV-A
              Phys Layer is supported. Assumes that the rlp_id passed is a 
              valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_rlp_bound_ev
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver,               /* RLP version                    */
  uint8 rtcmac_id
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RLP_UNBOUND_EV

DESCRIPTION   State machine event handling when an RLP is 
              unbound from an RTCMAC

DEPENDENCIES  If this function is called, then it is assumed that REV-A
              Phys Layer is supported. Assumes that the rlp_id passed is a 
              valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_rlp_unbound_ev
(
  uint8 rlp_id,                          /* RLP id                         */
  dsrlp_ver_enum_type ver,               /* RLP version                    */
  uint8 rtcmac_id
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RTCMAC_ACTIVATED_EV

DESCRIPTION   State machine event handling when an RTCMAC 
              is activated.

DEPENDENCIES  If this function is called, then it is assumed that REV-A
              Phys Layer is supported. Assumes that the rtcmac_id passed is a 
              valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_rtcmac_activated_ev
(
  uint8 rtcmac_id
);

/*===========================================================================

FUNCTION      HDRPACQNMI_PROCESS_RTCMAC_DEACTIVATED_EV

DESCRIPTION   State machine event handling when an RTCMAC 
              is deactivated.

DEPENDENCIES  If this function is called, then it is assumed that REV-A
              Phys Layer is supported. Assumes that the rtcmac_id passed is a 
              valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void hdrpacqnmi_process_rtcmac_deactivated_ev
(
  uint8 rtcmac_id
);

#endif /* FEATURE_HDR_REVA_L1 */


/*===========================================================================
FUNCTION       HDRPACQNMI_RELEASE_FLOW_RESOURCES

DESCRIPTION    This function is called when we need to release the state 
               information with respect to a particular ps_flow_ptr

DEPENDENCIES   None. 

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void hdrpacqnmi_release_flow_resources
(
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================
FUNCTION       HDRPACQNMI_GET_FREE_RESV_TABLE_ENTRY

DESCRIPTION    Returns a handle to the resv table entry given a resv_label
               and direction.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
hdrpacqnm_resv_table_entry_type* hdrpacqnmi_get_free_resv_table_entry(
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       HDRPACQNMI_RESV_TABLE_ENTRY

DESCRIPTION    Checks if the RESV label is in use

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
boolean hdrpacqnmi_resv_label_in_use
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       HDRPACQNMI_RESV_TABLE_ENTRY

DESCRIPTION    Returns a handle to the resv table entry given a resv_label
               and direction.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
hdrpacqnm_resv_table_entry_type* hdrpacqnmi_resv_table_entry
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);  

/*===========================================================================
FUNCTION       HDRPACQNMI_SET_PREV_STATE

DESCRIPTION    Sets previous state

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
boolean hdrpacqnmi_set_prev_state(uint8 resv_label, ds707_direction_enum_type dirn);
#endif  /* FEATURE_HDR_QOS */
#endif  /*  HDRPACQNMI_H */
