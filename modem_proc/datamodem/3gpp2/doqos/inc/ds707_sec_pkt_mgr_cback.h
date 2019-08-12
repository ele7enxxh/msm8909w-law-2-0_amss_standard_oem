/*===========================================================================

                        D S 7 0 7 _ S E C_P K T _ M G R _ C B A C K . H
GENERAL DESCRIPTION
  This file contains functionality secondary packet manager call backs

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

 
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_sec_pkt_mgr_cback.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
03/26/07   sk      Added EMPA MPA handoff 
05/03/07   sk      Support to process bundle response message from AN

===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR_QOS
#include "rex.h"
#include "amssassert.h"
#include "msg.h"
#include "ds_dqos_mgr.h"

/*===========================================================================
                           FORWARD DECLARATIONS
===========================================================================*/
/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                       FLOW MANAGER CALLBACKS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_FLOW_MGR_CBACK    

DESCRIPTION   This function routes the flow manger callbacks to appropriate 
              handler
     
DEPENDENCIES  None
 
RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sec_pkt_mgr_flow_mgr_cback(
  ds707_flow_type *ds_flow_pt,
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier
  );

/*===========================================================================
                       DQOS CALLBACKS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_SEC_PKT_MGRI_DQOS_EV_CBACK

DESCRIPTION   Called by DQOS module to notify response to a RESV msg

DEPENDENCIES  Should be called only from the DS task

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sec_pkt_mgr_dqos_ev_cback(
  ds707_flow_type              ** ds_flow_ptr,
  uint8                           num_ds_flow,
  ds_dqos_mgr_event_enum_type     cback_qualifier,
  ps_iface_type                 * ps_iface_ptr
);

/*==========================================================================
                       HDR PROTOCOL CALLBACKS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_PROTOCOL_CBACK

DESCRIPTION   This is the callback function registered with HDR PROTOCOL. 
              Any change on HDR PROTOCOL relevant to QoS module is notified
              through this function
     
DEPENDENCIES  None
 
RETURN VALUE  None

SIDE EFFECTS  Action is taken based on callback
===========================================================================*/
void ds707_sec_pkt_mgr_protocol_cback(
  ds707_sys_qos_state_enum_type sys_qos_state,
  hdrcom_hai_app_subtype_enum_type  old_app_subtype,
  hdrcom_hai_app_subtype_enum_type  new_app_subtype,
  dsrlp_rlp_identity_type rlp,
  boolean is_default,
  boolean activated,
  uint8  resv_count,
  uint8  resv_label[],
  boolean resv_bound,
  ds707_sec_pkt_protocol_cb_enum_type cback_qualifier
);

/*==========================================================================
                      TIMER CALLBACK
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_SEC_PKT_MGR_QOS_GRANT_TIMER_CB

DESCRIPTION    Function registered when the Timer for a QoS Grant has been 
               expired.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_sec_pkt_mgr_qos_grant_timer_cback(unsigned long param);

#endif  /* FEATURE_HDR_QOS  */
#endif /*FEATURE_DATA_IS707 */



