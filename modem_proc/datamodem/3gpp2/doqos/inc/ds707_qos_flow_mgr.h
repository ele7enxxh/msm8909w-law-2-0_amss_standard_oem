#ifndef DS707_QOS_FLOW_MGR_H
#define DS707_QOS_FLOW_MGR_H

/*===========================================================================

          DS707 QOS FLOW MANAGER

GENERAL DESCRIPTION
  This module get notifications from the reservation module regarding the
  state of the reservation for a particular reservation label. It 
  consolidates the reservation state for the whole flow, and notifies the 
  upper layers.

  It also gets notifications from the QoS Negotiation Manager regarding the 
  current state of the QoS. It consolidates the QoS state for the whole flow
  and notifies the upper layers.

  This module also assigns the reservation labels for each QoS Request; Creates
  the flow when the QoS Request IOCTL is called from the Application context.

EXTERNALIZED FUNCTIONS

  ds707_qos_flow_mgr_register_for_notifications()
    Sec_pkt_mgr registers callbacks with flow_mgr if it wants to be
    informed about flow events like resv on/off, qos granted/released, etc.

  ds707_qos_flow_mgr_deregister_notifications()
    Sec_pkt_mgr deregisters callbacks with flow_mgr if it's no longer 
    interested in them.

  ds707_qos_flow_mgr_process_cmd()
    Processes commands issued to flow mgr.

  ds707_qos_flow_mgr_request_qos()
    Called to request Qos for the first time or to modify an
    existing Qos.
    
  ds707_qos_flow_mgr_release_qos()
    Called to release Qos.

  ds707_qos_flow_mgr_release_qos_resv_labels()
    Called to release qos for the particular resv labels.

  ds707_qos_flow_mgr_request_resv_on() 
    Called to request turning on reservation for a particular flow.
    
  ds707_qos_flow_mgr_request_resv_off()
    Called to request turning off reservation for a particular flow.
  
  ds707_qos_flow_mgr_create_flow()
    Creates a ps flow.
    
  ds707_qos_flow_mgr_powerup_init()
    Called only once at mobile power-up to initialize variables
    and data structures.

  ds707_qos_flow_mgr_is_qos_granted_for_flow()
    Queries the QNM and determines if QoS is granted for the 
    flow on the whole

  ds707_qos_flow_mgr_get_info_resv_label()    
    Returns information for a given resv label and direction. 
    
  ds707_qos_flow_mgr_granted_setid_non_zero()  
    Returns if for a given QoS Request if either on of the
    directions has been granted a SetID ZERO. 

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_qos_flow_mgr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/11   ms      Global variables cleanup.
03/26/09   sk      eHRPD: UE initiated QoS multi pdn support
03/09/09   sa      AU level CMI modifications.
02/19/09   sk      eHRPD: UE initiated QoS
05/01/07   sk      QoS Modify support
10/23/06   spn     Changes to flush all the pending Reservation Messages OTA.
07/06/06   sk      QoS 2.0 Support
05/23/06   spn     The secondary flows are bounded by (DS707_N_RESV_MAX_FWD-1)
                   instead of DS707_N_RESV_MAX_FWD. Similarly for Reverse 
                   direction also. Did the required changes.
03/29/06   spn     Code Cleanup: Removed Redundant Variables
02/14/06   spn     Added a method which pass's through to QNM to release
                   QOS given only ResvLabels.
11/11/05   spn     Added method to return information if a flow_ptr is given
11/04/05   spn     Added a Method get_info() to return the information for a 
                   particular resv_label and direction.
10/31/05   spn     Re-structured the module, around the ds_flow_ptr, removed
                   the table that flow_mgr used to maintain.
10/24/05   spn     Changed the module so that it interacts with the QNM.
                   It doesnt maintain the state of the reservation, instead 
                   queries the resv module for the state. And also doesnt 
                   maintain the state of the QoS, it queries the QNM for the 
                   state of the QoS
07/20/05   spn     Included the parameter notify_AN in the ds707_qos_flow_mgr
                   _request_resv_on() /off() methods. 
07/05/05   atp     Fixes for compilation.
06/28/05   atp     Modified interfaces to sec_pkt_mgr, resv_module, ps.
05/25/05   atp     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_HDR_QOS
#include "errno.h"
#include "dstask_v.h"
#include "ps_qos_defs.h"
#include "ps_flow.h"
#include "ds707_qosdefs.h"
#include "hdrmrlpresv.h"
#include "ds707_pkt_mgri.h"
#include "ds707_qos_flow_mgri.h"

/*===========================================================================
      Constants & Macros
===========================================================================*/

/*===========================================================================
      Typedefs
===========================================================================*/

/*---------------------------------------------------------------------------
Represents the entire state of the Module of Flow Mgr and allocates 
resources as per AT/UE implementation.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
  This is for global counting
  -------------------------------------------------------------------------*/
  ds707_qos_flow_mgr_pdn pdn[DS707_MAX_PDN_INSTANCES];

  /*-------------------------------------------------------------------------
  This is global resource. 
  -------------------------------------------------------------------------*/
  ds707_qos_flow_mgr_resv fwd_resv_label[( DS707_N_RESV_MAX_FWD - 1 )];
  ds707_qos_flow_mgr_resv rev_resv_label[( DS707_N_RESV_MAX_REV - 1 )];

  /*-------------------------------------------------------------------------
  The following data structure is to track if the reservation is still bound
  to the RLP. It is a cached information. The reservation label can be bound
  and unbound by network at any point of time irrespective of whether the
  reservation is allocated or not. 

  The same information can be obtained from RLP's data base. But, the query
  may take long time as the search should go through all the activate RLPs
  in the system.

  -------------------------------------------------------------------------*/
  ds707_qos_flow_mgr_resv_bind fwd_resv_bound[(DS707_N_RESV_MAX_FWD - 1)];
  ds707_qos_flow_mgr_resv_bind rev_resv_bound[(DS707_N_RESV_MAX_REV - 1)];

} ds707_qos_flow_mgr_state_info_type;

/*===========================================================================
      Variables
===========================================================================*/

/*===========================================================================
      External Functions
===========================================================================*/

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_REGISTER_FOR_NOTIFICATIONS

DESCRIPTION   Sec_pkt_mgr registers callbacks with flow_mgr if it wants to be
              informed about flow events like resv on/off, qos
              granted/released, etc.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_register_for_notifications 
( 
  void (* qos_modified_notification_cb) /* Call back function   */
        (ds707_flow_type * flow_ptr, 
         ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier)
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_DEREGISTER_NOTIFICATIONS

DESCRIPTION   Sec_pkt_mgr deregisters callbacks with flow_mgr if it's no longer
              interested in them.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_deregister_notifications 
( 
  void 
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_PROCESS_CMD

DESCRIPTION   Processes commands issued to flow mgr.

DEPENDENCIES  This should be called from DS task.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_qos_flow_mgr_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_REQUEST_QOS

DESCRIPTION   Called to request Qos for the first time or to modify an
              existing Qos. The flag is_new_request applies to all the 
              ps_flow_ptr's.

DEPENDENCIES  Assumes sec_pkt_mgr is calling this in DS context.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  This method fills in the data structure, ds_flow_ptr that  
              co-relates to the ps_flow_ptr.
===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_request_qos 
( 
  uint8           num_request,
  ds707_flow_type *ds_flow_ptr[],
  boolean         is_new_request, /* T- brand new qos request
                                   F- Modify existing qos request */
  boolean         is_ue_resync   /* T - qos request is triggered by UE to
                                        resync with AN*/
);


/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_RELEASE_QOS

DESCRIPTION   Called to release Qos on a number of flow ptrs. The goal of 
              this method being to release as many flows as possible. And 
              for the flows which were failed we clean up silently.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_release_qos
(
  uint8         num_request,
  ds707_flow_type *ds_flow_ptr[]
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_RELEASE_QOS_RESV_LABELS

DESCRIPTION   Called to release Qos, given resv labels. The function does 
              not update any book keeping for the requested releases.

              This function is to be used either on power cycle or when the 
              AT returns to a QoS Aware system, from an UnAware system, to 
              release Qos of ResvLabels, whose DS FLOWS have been deleted.

DEPENDENCIES  
              
RETURN VALUE  E_NOT_AVAILABLE: If MFPA is not negotiated
              E_DATA_TOO_LARGE:If such a big GAUP request is not supported
              E_NOT_ALLOWED:   If This request is not allowed
              E_SUCCESS:       Returned on Success

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type  ds707_qos_flow_mgr_release_qos_resv_labels
(
  uint8 label_count,
  ds707_resv_label_type resv_label[( DS707_N_RESV_MAX_FWD - 1 ) + ( DS707_N_RESV_MAX_REV - 1 )] 
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_REQUEST_RESV_ON

DESCRIPTION   Called to request turning on reservation for a particular flow.
              The flag notify_AN applies to all the resv On Requests.

              notify_AN Should be false only if we are in a QOS UNAWARE SYSTEM.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_request_resv_on
(
  uint8 num_flows,
  ds707_flow_type *ds_flow_ptr[],
  boolean        notify_AN,      /* Whether the request is implicit or 
                                   explicit                                */
  boolean        send_immediately                                   
);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_REQUEST_RESV_OFF

DESCRIPTION   Called to request turning off reservation for a particular flow.
              The flag notify_AN applies to all the resv On Requests. 

              notify_AN Should be false only if we are in a QOS UNAWARE SYSTEM.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_request_resv_off
(
  uint8 num_flows,
  ds707_flow_type *ds_flow_ptr[],
  boolean        notify_AN,      /* Whether the request is implicit or 
                                   explicit                                */
  boolean        send_immediately                                   
);
/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_POWERUP_INIT

DESCRIPTION   Called only once at mobile power-up to initialize variables
              and data structures.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_powerup_init 
( 
  void 
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_IS_QOS_GRANTED_FOR_FLOW

DESCRIPTION   Queries the QNM and concludes if the flow has been granted 
              QoS

DEPENDENCIES  None.

RETURN VALUE  TRUE - QOS has been granted
              FALSE - QOS has not been granted

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_qos_flow_mgr_is_qos_granted_for_flow
(
  ds707_flow_type *ds_flow       /* Ptr to ps_flow passed in by caller       */
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_IS_RESV_ON_FOR_FLOW

DESCRIPTION   Queries the resv_module and concludes if the flow has been 
              granted reservation on the whole

DEPENDENCIES  None.

RETURN VALUE  TRUE - Resv is on 
              FALSE - Resv is not on.

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_qos_flow_mgr_is_resv_on_for_flow
(
  ds707_flow_type *ds_flow_ptr    /* Ptr to ps_flow passed in by caller       */
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_GET_INFO_RESV_LABEL

DESCRIPTION   This is a utility function that returns information, given a 
              resv_label and direction.
             
PARAMETERS    resv_label: Reservation label Passed to query information
              direction : The direction which is applicable to the 
                          resv_label passed to us
              ps_flow_ptr, dirn, fwd_resv_label, rev_resv_label are the
              return values, described below. 

DEPENDENCIES  None.

RETURN VALUE  TRUE -  Information returned is valid
              FALSE - Information returned is invalid

              ps_flow_ptr: The ps_flow_ptr that corresponds to the query
              dirn      : The Consolidated direction of the ps_flow_ptr
              fwd_resv_label: returns a fwd_resv_label if it is applicable
              rev_resv_label: returns a rev_resv_lable if it is applicable

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_qos_flow_mgr_get_info_resv_label
(
  uint8 resv_label,                 
  ds707_direction_enum_type direction,
  ds707_flow_type **ds_flow_ptr_ptr /* Ptr to ps_flow passed in by caller  */
);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_GRANTED_SETID_ZERO

DESCRIPTION   Returns if for a given QoS Request if either on of the
              directions has been granted a SetID ZERO. 

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_qos_flow_mgr_granted_setid_non_zero
(
  ps_flow_type  * ps_flow_ptr,
  boolean * setid_is_non_zero
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGR_RELEASE_FLOW

DESCRIPTION    This method deregisters with the Reservation Module and then
               releases the resources for a particular flow pointer.
               This method does not trigger an OTA message for reservation 
               off or QOS Release.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void ds707_qos_flow_mgr_release_flow
(
  ds707_flow_type *ds_flow_ptr
);

/*==========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_AVAILABLE_RESV_LABEL

DESCRIPTION   This is an utility function. The caller will get the count of 
              forward free reservation labels on the system
             
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_qos_flow_mgr_available_resv_label(
  uint8 *fwd_available_resv_total,
  uint8 *rev_available_resv_total
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_IS_QOS_ACCEPTED_FOR_FLOW

DESCRIPTION   Queries the QNM and concludes if QOS has been accepted 
              for the flow by the an

DEPENDENCIES  None.

RETURN VALUE  TRUE - QOS has been accepted
              FALSE - QOS has not been accepted

SIDE EFFECTS  None.

===========================================================================*/
boolean ds707_qos_flow_mgr_is_qos_accepted_for_flow
(
  ds707_flow_type *ds_flow_ptr    /* Ptr to ps_flow passed in by caller       */
);

/*==========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_GET_GRANTED_SETID

DESCRIPTION   Returns the granted SetID
             

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_get_granted_setid(
  uint8 resv_label,
  ds707_direction_enum_type dirn,
  uint8 *granted_setid
);

/*==========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_GET_START_SETID

DESCRIPTION   Returns the start SetID
             

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None.
===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_get_start_setid(
  uint8 resv_label,
  ds707_direction_enum_type dirn,
  uint8 *granted_setid
);
/*==========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_SET_PREV_STATE

DESCRIPTION   
             
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_qos_flow_mgr_set_prev_state(
  uint8 resv_label, 
  ds707_direction_enum_type dir, 
  ds707_flow_type *ds_flow_ptr
);
#ifdef FEATURE_EHRPD
/*==========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_GET_NEXT_AVAIL_RESV_LABEL

DESCRIPTION   This is an utility function. Given a PDN ID, a reservation label
              is allocated.
             
DEPENDENCIES  None.

RETURN VALUE  TRUE: if reservation is allocated successfully
              FALSE: otherwise

SIDE EFFECTS  The reservation label from the pool is allocated
===========================================================================*/
boolean ds707_qos_flow_mgr_get_next_avail_resv_label( 
  uint8 pdn_id, 
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGR_REQUEST_QOS_EX

DESCRIPTION   This function is used to send ReservationKKQoSRequest over the
              air. The Reservation label is registered with reservation module
              to get the notification on time.

DEPENDENCIES  The assumption is that the reservation label is already 
              allocated and it is not registered with reservation label

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  DS module registers for reservation label with Reservation 
              Module
===========================================================================*/
errno_enum_type ds707_qos_flow_mgr_request_qos_ex 
( 
  uint8         num_request,
  ds707_flow_type *ds_flow_ptr_request[]
);

#endif /* FEATURE_EHRPD */

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGR_GET_STATE_INFO

DESCRIPTION    This is a utility function to get the Qos flow manager 
               state information. 

DEPENDENCIES   NONE

RETURN VALUE   Address of the global variable ds707_qos_flow_mgr_state_info

SIDE EFFECTS   NONE
===========================================================================*/
ds707_qos_flow_mgr_state_info_type * ds707_qos_flow_mgr_get_state_info(void);

#endif  /* FEATURE_HDR_QOS */
#endif  /*  DS707_QOS_FLOW_MGR_H  */
