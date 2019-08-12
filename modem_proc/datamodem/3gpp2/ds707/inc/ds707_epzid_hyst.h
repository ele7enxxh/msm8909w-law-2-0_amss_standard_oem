#include "datamodem_variation.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#ifndef DS707_EPZID_HYSTERESIS_H
#define DS707_EPZID_HYSTERESIS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                     E P Z I D  H Y S T E R E S I S

GENERAL DESCRIPTION
  This file contains functions used to handle PZID Hysteresis feature.

EXTERNALIZED FUNCTIONS

 DS707_EPZID_HYSTERESIS_PROCESS_EVENT
   This function is the Hysteresis event machine. It is the access point 
   for all the external files. It is called whenever there is a change in 
   call state to either going active/dormant or if the signal is set for the 
   timer or BS sends an SOCM to turn on/off hysteresis machine.
   
 DS707_EPZID_HYSTERESIS_PROCESS_HT 
    Function to process the hysteresis timer (HT).
 
 DS707_EPZID_HYSTERESIS_ADD_NEW_PZID 
    Funciton to be called while in  hysteresis state to add new pzid to 
	the list and also starts a timer on all the existing entries.
 
 
 DS707_EPZID_HYSTERESIS_PRINT_PZID_LIST
    Funtion which prints the hysteresis list along with the HT timer value.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    Hysteresis state will be turned on by default.
   Copyright (c) 2003-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_epzid_hyst.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     --------------------------------------------------------
03/15/11   mg      Global variable cleanup
03/12/09   ms      Update the HAT timer value with the value read from NV
                   or RUIM card entry.
03/09/09   sa      AU level CMI modifications.
02/04/09   ss      Off Target Lint fixes
08/05/08   ms      IS707B addendum Feature support.
04/05/05   gr      Initial version of the file to support IS707-B

===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "ds707_epzid.h"
#include "ds707_cback.h"
#include "dstask_v.h"


/*===========================================================================
                                TYPEDEFS
===========================================================================*/

/*---------------------------------------------------------------------------
   Hysteresis activation timer (HAT) and hysteresis timer (HT) 
   as defined by the standard in seconds.
---------------------------------------------------------------------------*/
#define EPZID_HYSTERESIS_HAT_TIMER_IN_SEC 30 
#define EPZID_HYSTERESIS_HT_TIMER_IN_SEC  60

/*===========================================================================
                                 EXTERNS
===========================================================================*/

/*===========================================================================
FUNCTION       DS707_EPZID_HYST_DS707_EVENT_HDLR

DESCRIPTION    Event Handler for DS707 Events received  by the Hysteresis 
               engine

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_epzid_hyst_ds707_event_hdlr
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);

/*===========================================================================
FUNCTION 	DS707_EPZID_HYST_SOCM_ENABLE_HYST

DESCRIPTION	This function is called whenever BS sends SOCM to turn on
		hysteresis. All the hysteresis state variables are initialized.


DEPENDENCIES	NONE

RETURN VALUE	NONE

SIDE EFFECTS	NONE
===========================================================================*/
void ds707_epzid_hyst_socm_enable_hyst
(
  void
);
/*===========================================================================
FUNCTION 	DS707_EPZID_HYST_SOCM_DISABLE_HYST

DESCRIPTION	This function is called when the BS sends SOCM to disable the
		the hysteresis feature. Whenever hysteresis is disabled the
		hysteresis state variables are reset.

DEPENDENCIES	NONE

RETURN VALUE	NONE

SIDE EFFECTS	NONE
===========================================================================*/
void ds707_epzid_hyst_socm_disable_hyst
(
  void
);
/*===========================================================================

FUNCTION    : DS707_EPZID_HYSTERESIS_NULL_STATE

DESCRIPTION : Enters null state when packet call is closed or when 
              the PZID reconnection feature is disabled

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void ds707_epzid_hysteresis_null_state
(
  void
);
/*===========================================================================
FUNCTION 		DS707_EPZID_HYSTERESIS_INIT_STATE

DESCRIPTION		This function is called when Hysteresis SM enters INIT state. 
				The Hys SM enters init state in the following scenarios:
                1. A non-zero PZID is detected as part of EPZID change
                2. A SOCM msg to enable Hysteresis is received  
				3. PPP data transfer using RLP/SDB happens 

                Scenario (3) turns the HAI=TRUE and hence  forces the HAT to 
                be started after 30 secs.

DEPENDENCIES	None

RETURN VALUE	None

SIDE EFFECTS	None
===========================================================================*/
void ds707_epzid_hysteresis_init_state
(
   boolean data_transfer_flag
);
/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_PRINT_EPZID_LIST

DESCRIPTION
  This function prints the current visited EPZID list as a series of 
  MSG_HIGHs. This is for debug purposes.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void ds707_epzid_hysteresis_print_epzid_list
(
  void
);

/*===========================================================================

FUNCTION DS707_EPZID_HYSTERESIS_ADD_NEW_EPZID

DESCRIPTION
  This funciton is called only in hysteresis state and this will add the
  new pzid to the list and also starts a timer on all the existing entries.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void ds707_epzid_hysteresis_add_new_epzid
(
  ds707_epzid_type *new_epzid 
);

/*===========================================================================
FUNCTION      DS707_EPZID_HYSTERESIS_PROCESS_CMD

DESCRIPTION   This function processes PZID specific commands.

DEPENDENCIES  None.

RETURN VALUE  boolean reorig_flag

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_hysteresis_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);
/*===========================================================================

FUNCTION       DS707_EPZID_HYST_SEND_CMD

DESCRIPTION	   Post a command to DS task whenever a SDB/PPP data tranfer
               happens
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void  ds707_epzid_hyst_send_cmd
(
   ds_cmd_enum_type cmd
);
/*===========================================================================
FUNCTION       DS707_EPZID_HYST_START_HT

DESCRIPTION    This function starts the hysteresis timer on the last 
               EPZID entry in the list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
extern void ds707_epzid_hyst_start_ht
(
  void
);

/*===========================================================================
FUNCTION       DS707_EPZID_SET_HYSTERESIS_ENABLED

DESCRIPTION    This function sets the hysteresis_enabled flag

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_set_hysteresis_enabled(
  boolean hysteresis_enabled
);

#ifdef FEATURE_IS707B_ADDENDUM
/*===========================================================================
FUNCTION       DS707_EPZID_SET_HAT_HDLR

DESCRIPTION    This function sets the HAT to user defined value

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
extern void ds707_epzid_set_hat_hdlr
(
  ds_cmd_type *ds_cmd_ptr
);
/*===========================================================================
FUNCTION      DS707_EPZID_HYST_REG_SUBSTATE_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called This call back gets called under MC task space whenever
              mobile station enters the registration access sub state due to 
              timer based registration, distance based registration, parameter change registration
              or use zone registration as registration type 
              
              Sends a command to DS, so that actual processing is done in 
              DS task context.
              
              This function is called from the context of the MC task.
       
              This function follows procedures from IS-707.B sec.2.2.2.1.2.4. 
              fetches the PZID list and check if HT is running for the current PZID. 
              If it is true, then DS will clear the HT and remove the current PZID from the list.
              And finally DS will call ps_physlink_up_cmd() to send DRS=0 origination. 
                                   
              This command handler is executed under the context of the DS task.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern boolean ds707_epzid_hyst_reg_substate_cb
(
  word sid,                         /* System   Identifier */
  word nid,                         /* Network  Identifier */
  byte pzid                         /* Pkt Zone Identifier */
);

/*===========================================================================
FUNCTION       DS707_EPZID_SET_NV_HAT_TIMER_IN_SEC

DESCRIPTION    This function sets the _nv_hat_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_set_nv_hat_timer_in_sec
(
  uint8 timer_val
);

/*===========================================================================
FUNCTION       DS707_EPZID_GET_HYSTERESIS_HAT_TIMER_IS_RUNNING

DESCRIPTION    This function returns TRUE if epzid_hysteresis hat timer is 
               running.  

DEPENDENCIES   None

RETURN VALUE   boolean

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_epzid_get_hysteresis_hat_timer_is_running(void);

/*===========================================================================
FUNCTION       DS707_EPZID_GET_HAT_START_TIME

DESCRIPTION    This function returns value of ds707_epzid_hat_start_time

DEPENDENCIES   None

RETURN VALUE   dword

SIDE EFFECTS   None
===========================================================================*/
dword ds707_epzid_get_hat_start_time(void);

#endif /* FEATURE_IS707B_ADDENDUM*/

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_SET_HT_TIMER_IN_SEC

DESCRIPTION    This function sets value of ds707_epzid_hysteresis_ht_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_hysteresis_set_ht_timer_in_sec(
  uint8 timer_val
);

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_GET_HT_TIMER_IN_SEC

DESCRIPTION    This function returns value of ds707_epzid_hysteresis_ht_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   uint8

SIDE EFFECTS   None
===========================================================================*/
uint8 ds707_epzid_hysteresis_get_ht_timer_in_sec(void);

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_SET_HAT_TIMER_IN_SEC

DESCRIPTION    This function sets value of ds707_epzid_hysteresis_hat_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   uint8

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_hysteresis_set_hat_timer_in_sec(
  uint8 timer_val
);

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_GET_HAT_TIMER_IN_SEC

DESCRIPTION    This function returns value of ds707_epzid_hysteresis_hat_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   uint8

SIDE EFFECTS   None
===========================================================================*/
uint8 ds707_epzid_hysteresis_get_hat_timer_in_sec(void);

#endif /* DS707_EPZID_HYSTERESIS_H */
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
