#ifndef DS707_QOS_FLOW_MGRI_H
#define DS707_QOS_FLOW_MGRI_H

/*===========================================================================

          DS707 QOS FLOW MANAGER INTERNAL

GENERAL DESCRIPTION
  This module is a helper to the Flow Manager module. It handles 
  events/notifications from other modules (resv Module, QNM Module).

EXTERNALIZED FUNCTIONS
  ds707_qos_flow_mgri_init ()
    Called to initialize variables and data structures.

  ds707_qos_flow_mgri_process_qos_granted_modified_ev()
    State machine event handling when a Qos has been granted or modified.

  ds707_qos_flow_mgri_process_qos_released_ev()
    State machine event handling when a Qos has been released.

  ds707_qos_flow_mgri_process_resv_on_ev()
    State machine event handling when a reservation has been turned on by AN.

  ds707_qos_flow_mgri_process_resv_off_ev()
    State machine event handling when a reservation has been turned off by AN.

  ds707_qos_flow_mgri_search_flow()
    Searches flow table for ps_flow passed in as parameter.

  ds707_qos_flow_mgri_search_label()
    Checks if resv label passed in as parameter has already been taken in
    reservation table.

  ds707_qos_flow_mgri_get_next_avail_resv_label()
    Gets next available free reservation label for a particular direction 
    that can be allotted.
  
  ds707_qos_flow_mgri_process_qos_status_notif()
    This function is registered with the QNM module to receive notification
    regarding the QoS State of the flow.
  
  ds707_qos_flow_mgri_free_resv_label()
    This function will free up a used resv label. 

  ds707_qos_flow_mgri_release_flow_resources()
    This function is called to release the resources for a particular 
    ps_flow_ptr.

  ds707_qos_flow_mgri_consolidate_commands()
    This function is called to either start consolidating the 
    commands, or add a command to be consolidated command, if 
    consolidation has already started

  ds707_qos_flow_mgri_is_consolidation_running()
    Returns true if the module has already started to
    consolidate commands

  ds707_qos_flow_mgri_process_consolidated_cmd()
    This function is called to process the consolidated command, 
    in DS task context.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_qos_flow_mgri.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/11   ms      Global variables cleanup.
04/07/11   ms      Global variables cleanup.
03/26/09   sk      eHRPD: UE initiated QoS multi pdn support
03/09/09   sa      AU level CMI modifications.
10/31/06   spn     Changes to consolidate a burst of commands in flow mgr
07/06/06   sk      QoS 2.0 Support
05/23/06   spn     The secondary flows are bounded by (DS707_N_RESV_MAX_FWD-1)
                   instead of DS707_N_RESV_MAX_FWD. Similarly for Reverse 
                   direction also. Did the required changes.
11/11/05   spn     Changed the notification from QNM to give only resv_label
                   and direction. 
10/31/05   spn     Re-structured the module, around the ds_flow_ptr, removed
                   the table that flow_mgr used to maintain. Re-wrote the 
                   module get_next_avail_resv_label. 
10/24/05   spn     Changed the module so that it interacts with the QNM.
                   It doesnt maintain the state of the reservation, instead 
                   queries the resv module for the state. And also doesnt 
                   maintain the state of the QoS, it queries the QNM for the 
                   state of the QoS
07/26/05   spn     Modified the method 
                   ds707_qos_flow_mgri_get_next_avail_resv_label(), to return
                   reservation label depending on the direction.
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
#include "errno.h"

#ifdef FEATURE_HDR_QOS
#include "ps_ipfltr_defs.h"

#include "ds707_pkt_mgri.h"
#include "dstask_v.h"

/*===========================================================================
      Constants & Macros
===========================================================================*/
/*---------------------------------------------------------------------------
  This is the count of number of commands that we can consolidate. The
  number 60 comes from 

  1. A Max of 15 Reservations
  2. 2 as there are two directions
  3. And the commands that need to consolidated are "ReservationOn/Off" and 
     ReservationOnSentNotification. 
  
  And hence 60 = ( 15 * 2 ) * 2

  There is a possibility for ReservationOn and ReservationOff for each 
  reservation label to be outstanding, but thats a corner case, and 
  in case it happens, then this number needs to be increased.
---------------------------------------------------------------------------*/
#define DS707_QOS_FLOW_MGR_MAX_CMDS_CONSOLIDATED  60

/*---------------------------------------------------------------------------
  The following definitions are as per x.0057 standard. 
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
The PDN ID is is 4 bit value. Hence 0x1F is invaid PDN ID
----------------------------------------------------------------------------*/
#define DS707_INVALID_PDN_ID   0x1F

/*---------------------------------------------------------------------------
The PDN ID is is 4 bit value. Hence 0x1F is invaid PDN ID
----------------------------------------------------------------------------*/
#define DS707_MAX_FLOW   0xF


/*========================================================================
      Typedefs
===========================================================================*/
/*---------------------------------------------------------------------------
  Struct holding info about flow_mgr callbacks, etc.
---------------------------------------------------------------------------*/
/*- - -  - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - 
 Command types used to send Qos notifications to flow mgr.
 Struct used to pass info about reservation on notification to flow mgr
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
typedef struct
{
  uint8 resv_label;                      /* Reservation label              */
  ds707_direction_enum_type dirn;        /* Direction                      */
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier;  /* Call Back Reason */
} ds707_qos_flow_mgr_resv_on_notify_type;

typedef struct
{
  uint8 resv_label;                    /* Reservation label                */
  ds707_direction_enum_type dirn;      /* Direction                        */
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier;  /* Call Back Reason */
} ds707_qos_flow_mgr_resv_off_notify_type;

/*---------------------------------------------------------------------------
Represents the entire state of the Module of Flow Mgr and allocates 
resources as per AT/UE implementation.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 resv_label;                    /* Reservation label                */
  ds707_direction_enum_type dirn;      /* Direction                        */
} ds707_qos_flow_mgr_reservation_on_sent_notify_type;

typedef struct
{
  void (* qos_modified_notification_cb) /* Call back function   */
        (ds707_flow_type * flow_ptr,
         ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier);
} ds707_qos_flow_mgr_info_type;

/*---------------------------------------------------------------------------
  For management of resv Labels and binding.
  If there is entry in the table, the reservation label is bound.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   resv_label;       /* reservation label associationed */
} ds707_qos_flow_mgr_resv_bind;


/*---------------------------------------------------------------------------
  For management of resv Labels. 
---------------------------------------------------------------------------*/
typedef struct
{
  boolean allocated;       /* Whether this block is assigned or not   */
  uint8   resv_label;       /* reservation label associationed */
  boolean is_turned_on;   /* Indicates whether the resv is turned on or not*/
  boolean is_resv_on_sent; /* Indicates whether the reservation on sent successfully */
  ds707_flow_type *ds_flow_ptr;/* The DS flow ptr which is using this label*/
} ds707_qos_flow_mgr_resv;

/*---------------------------------------------------------------------------
  For management of next resv Labels for a PDN.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
  Totoal number of flows for one PDN
  -------------------------------------------------------------------------*/
  uint8 total_fwd_flows;  /* Total fwd flows */ 
  uint8 total_rev_flows;  /* Total rev flows  */ 

  /*-------------------------------------------------------------------------
  This is to keep track of next possible reservation lable to use. This
  will save run time search of available reservation label. However, the
  reservation label's usability needs to be varified.
  -------------------------------------------------------------------------*/
  uint8         next_fwd_resv; 
  uint8         next_rev_resv; 

  /*-------------------------------------------------------------------------
  The PDN ID is is 4 bit value. Hence 0x1F is invaid PDN ID
  -------------------------------------------------------------------------*/
  uint8         pdn_id;             /* actual PDN id */
  void          *iface_ptr;         /* iface pointer */

} ds707_qos_flow_mgr_pdn;

/*---------------------------------------------------------------------------
  Union of commands that can be originated from the Flow Manager, 
  This is used to consolidate the commands in case we run out of ds_cmds
---------------------------------------------------------------------------*/
typedef struct 
{
  ds_cmd_enum_type cmd_hdr;
  union
  {
    ds707_qos_flow_mgr_resv_on_notify_type on_notify;
    ds707_qos_flow_mgr_resv_off_notify_type off_notify;
    ds707_qos_flow_mgr_reservation_on_sent_notify_type on_sent_notify;
  } cmd_info;
} ds707_qos_flow_mgri_command_type;

/*---------------------------------------------------------------------------
  This enum is used to know whether we are currently trying to 
  consolidate commands or not.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_FLOW_MGR_CONSOLIDATED_CMD_FREE,
  DS707_FLOW_MGR_CONSOLIDATED_CMD_IN_USE
} ds707_flow_mgri_consolidated_cmd_state_enum_type;


/*---------------------------------------------------------------------------
  This is the structure that we will be using to consolidate the commands

  next_available_index :  This is the index where we have to insert the 
                          new command.
  cmd_state   :  Whether the command is in use or not
  cmds:   This is the array where we will be consolidating the commands. 
          We choose 60, for a max of 15reservations per dirn, for the 
          sent_notify and the state_change_notify
          
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 next_available_index;
  ds707_flow_mgri_consolidated_cmd_state_enum_type consolidated_cmd_state;
  ds707_qos_flow_mgri_command_type cmds[DS707_QOS_FLOW_MGR_MAX_CMDS_CONSOLIDATED];
} ds707_qos_flow_mgri_consolidated_command_type;

/*===========================================================================
      Variables
===========================================================================*/


/*===========================================================================
      Functions
===========================================================================*/
/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGRI_INIT

DESCRIPTION   Called to initialize variables and data structures.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.

===========================================================================*/
errno_enum_type ds707_qos_flow_mgri_init ( void );

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGRI_PROCESS_QOS_RELEASED_EV

DESCRIPTION   State machine event handling when a Qos has been released.
              This event is notified on a direction basis. If
              QoS is granted for the flow as a whole, then the sec_pkt_mgr
              is notified of a QoS Grant

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_qos_flow_mgri_process_qos_released_ev
(
  ds707_flow_type *ds_flow_ptr,
  ds707_direction_enum_type dir,
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier   
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGRI_PROCESS_QOS_GRANTED_MODIFIED_EV

DESCRIPTION   State machine event handling when a Qos has been granted or
              modified. This event is notified on a direction basis. If
              QoS is granted for the flow as a whole, then the sec_pkt_mgr
              is notified of a QoS Grant

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_qos_flow_mgri_process_qos_granted_modified_ev
(
  ds707_flow_type  *ds_flow_ptr,
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier   
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGRI_PROCESS_RESV_ON_EV

DESCRIPTION   State machine event handling when a reservation has been 
              turned on by AN. This event is notified on a direction basis. 
              If Resv is on for the flow as a whole, then the sec_pkt_mgr
              is notified of a QoS Grant

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_qos_flow_mgri_process_resv_on_ev
(
  uint8 resv_label,                     /* Reservation Label               */
  ds707_direction_enum_type dirn,       /* Direction                       */
  ds707_sec_pkt_flow_mgr_cb_enum_type cb_reason  /* Call Back Reason               */
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGRI_PROCESS_RESV_OFF_EV

DESCRIPTION   State machine event handling when a reservation has been
              turned off by AN.

DEPENDENCIES  Assumes that the resv_label passed is a valid resv label.

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_qos_flow_mgri_process_resv_off_ev
(
  uint8 resv_label,                    /* Reservation Label                */
  ds707_direction_enum_type dirn,      /* Direction                        */
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier  /* Call Back Reason               */
);

/*===========================================================================

FUNCTION      DS707_QOS_FLOW_MGRI_PROCESS_RESERVATION_ON_SENT_EV

DESCRIPTION   This function sets the flag to indicate the reservation on 
              request was sent successfully. Also, queries the utility
              function to see if all the reservations belonging to this
              flow were sent successfully. If all the reseravtions were
              sent successfully, then inform secondary packet manager.

DEPENDENCIES  NONE

RETURN VALUE  None.

SIDE EFFECTS  None.

===========================================================================*/
void ds707_qos_flow_mgri_process_reservation_on_sent_ev
(
  uint8 resv_label,                    /* Reservation Label                */
  ds707_direction_enum_type dirn      /* Direction                        */
);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGRI_GET_NEXT_AVAIL_RESV_LABEL

DESCRIPTION   Gets next available free reservation label that can be allotted

DEPENDENCIES  None.

RETURN VALUE  Reservation label allotted.

SIDE EFFECTS  None.

===========================================================================*/
uint8 ds707_qos_flow_mgri_get_next_avail_resv_label
(
  uint8                     pdn_id,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_PROCESS_QOS_STATUS_NOTIF

DESCRIPTION    This function is registered with hdrpacqnm, to notify
               of events related to the QoS Request

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void ds707_qos_flow_mgri_process_qos_status_notif
(
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier,   
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_FREE_RESV_LABEL

DESCRIPTION    This function frees up a resv label. 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void ds707_qos_flow_mgri_free_resv_label
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_RESV_BOUND_EV

DESCRIPTION    This function is registered with QNM to reserve a label if 
               on reservation bound
               This would flag a given reservation label, bound and we make 
               sure that we dont assign this reservation label for a new 
               QoS Request.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void ds707_qos_flow_mgri_resv_bound_ev
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_RESV_UNBOUND_EV

DESCRIPTION    This function is registered with QNM to free up a label if 
               on reservation unbound
               This would flag the reservation as unbounded, and if this 
               reservation label is not allocated then this reservation 
               label is free to be assigned to a new QoS Request.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void ds707_qos_flow_mgri_resv_unbound_ev
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_RELEASE_FLOW_RESOURCES

DESCRIPTION    This function is called when we need to release the state 
               information with respect to a particular ps_flow_ptr

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
  
===========================================================================*/
void ds707_qos_flow_mgri_release_flow_resources
(
  ds707_flow_type *ds_flow_ptr
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_CONSOLIDATE_COMMANDS

DESCRIPTION    This function is called to either start consolidating the 
               commands, or add a command to be consolidated command, if 
               consolidation has already started

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_qos_flow_mgri_consolidate_commands
(  
  ds707_qos_flow_mgri_command_type *cmd
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_IS_CONSOLIDATION_RUNNING

DESCRIPTION    Returns true if the module has already started to
               consolidate commands.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds707_qos_flow_mgri_is_consolidation_running
(  
  void
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_PROCESS_CONSOLIDATED_CMD

DESCRIPTION    This function is called to process the consolidated command, 
               in DS task context.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_qos_flow_mgri_process_consolidated_cmd
( 
  void 
);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_FIND_FREE_RESV_INDEX

DESCRIPTION    This is a utility function to find the free reservation index in
               Flow manager state info.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint8 ds707_qos_flow_mgri_find_free_resv_index(ds707_direction_enum_type dirn);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGR_PDN_ALLOCATED_EVENT

DESCRIPTION    

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_qos_flow_mgr_pdn_allocated_event(ps_iface_type * iface_ptr, 
                                            uint8 pdn_id);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGR_IFACE_UNAVAILABLE_EVENT

DESCRIPTION    

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds707_qos_flow_mgr_pdn_deallocated_event(ps_iface_type * iface_ptr);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_FIND_PDN_INDEX

DESCRIPTION    This is a utility function to find the reservation index in
               Flow manager state info.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint8 ds707_qos_flow_mgri_find_pdn_index(uint8 pdn_id);

/*===========================================================================
FUNCTION       DS707_QOS_FLOW_MGRI_FIND_RESV_INDEX

DESCRIPTION    This is a utility function to find the reservation index in
               Flow manager state info.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint8 ds707_qos_flow_mgri_find_resv_index(uint8 resv_label, 
                                          ds707_direction_enum_type dirn);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGRI_REGISTER_FOR_NOTIFICATIONS

DESCRIPTION   Sec_pkt_mgr registers callbacks with flow_mgr if it wants to be
              informed about flow events like resv on/off, qos 
              granted/released, etc.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.
===========================================================================*/
errno_enum_type ds707_qos_flow_mgri_register_for_notifications 
( 
  void (* qos_modified_notification_cb) /* Call back function   */
        ( ds707_flow_type * flow_ptr, 
         ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier )
);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGRI_DEREGISTER_FOR_NOTIFICATIONS

DESCRIPTION   Sec_pkt_mgr De-registers callbacks with flow_mgr if it wants to be
              informed about flow events like resv on/off, qos 
              granted/released, etc.

DEPENDENCIES  None.

RETURN VALUE  E_SUCCESS/E_FAILURE.

SIDE EFFECTS  None.
==========================================================================*/
errno_enum_type ds707_qos_flow_mgri_deregister_notifications(void);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGR_INVOKE_QOS_MODIFIED_NOTIFICATION_CB

DESCRIPTION  Calls the associated QOS Modified notification callbakc function registered with
                    qos flow manager.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_qos_flow_mgr_invoke_qos_modified_notification_cb
(
  ds707_flow_type                     *flow_ptr,
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier
);

/*===========================================================================
FUNCTION      DS707_QOS_FLOW_MGRI_INVOKE_QOS_MODIFIED_NOTIFICATION_CB

DESCRIPTION  Calls the associated QOS Modified notification callbakc function registered with
             qos flow manager.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_qos_flow_mgri_invoke_qos_modified_notification_cb
(
  ds707_flow_type                     *flow_ptr,
  ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier
);
#endif  /* FEATURE_HDR_QOS */
#endif  /*  DS707_QOS_FLOW_MGRI_H  */
