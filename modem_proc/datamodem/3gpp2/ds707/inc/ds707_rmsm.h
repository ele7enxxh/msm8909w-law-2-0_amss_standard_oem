#ifndef DS707_RMSM_H
#define DS707_RMSM_H
/*===========================================================================

                          D S 7 0 7 _ R M S M . H

DESCRIPTION

  The Data Services IS707 Rm State Machine source file. This state machine 
  manages the Um and Rm interfaces for certain IS707 packet calls.  
  Specifically RMSM is used to support traditional Relay, SIP Network Model 
  and Mobile IP type IS707 calls which are a special case of the 
  generic multimode PS architecture.

EXTERNALIZED FUNCTIONS
  ds707_rmsm_init()
    Initialize the Rm State Machine.
  ds707_rmsm_post_event()
    Post an event to Rm State Machine.
  ds707_rmsm_process_cmd()  
    Process a cmd posted to Rm State Machine.
  ds707_rmsm_is_packet_call()
    Is RM in IS707 laptop pkt data call.
  ds707_rmsm_is_in_network_model_call()
    Is RM in IS707 laptop network model pkt data call.
  ds707_rmsm_dial_str_cb()
    Dial string handler for IS707 pkt data calls.  
  ds707_rmsm_notify_ppp_up
    Notifies RMSM that end to end PPP is up for SIP network model call.

Copyright (c) 2001 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rmsm.h_v   1.6   23 Jan 2003 16:34:28   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_rmsm.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
10/08/13    vm     Added argument to ps_iface_down_1x_v4_ind() to support
                   call end reason  
09/05/12    sid    Increase DO Rev A RM FWD DNE  
01/16/12    ack    Fix to post a cmd for iface bringup/teardown
11/21/11    msh    Changed Rev link wmk lvls 
08/15/11    ms     Fix graceful tear down in LPM
07/29/11    vpk    Add support for memory flow control logging
03/29/11    ack    Fix to gracefully tear down calls before phone 
                   goes to LPM mode
12/07/10    ls     Adjust WM value for A2 data path
06/25/10    vs     Moving the constants defining watermark levels
                   from ds707_rmsm.c to ds707_rmsm.h
10/22/09    ss     Adding prototype for the utility function 
                   ds707_rmsm_get_state().
03/04/09    sa     AU level CMI modifications.
02/09/09    spn    Moved RMSM PDN instance to the RMSM Module
06/13/07   ak/az   Added new function PS_IFACE_DOWN_V6_V4_IND() to facilitate
                   the tear down process for V4 and V6 ifaces
10/31/06    rsj    Added new function ds707_rmsm_is_relay_model_call() to 
                   indicate if a relay model call is active.
09/28/06    kvd    Added new function ds707_rmsm_get_sio_iface() to access RM
                   iface.
09/11/06    as     added new function ds707_rmsm_is_rm_iface_up to check if
                   the RM iface is up or not.
11/14/05    gr     Changes to throttle the um link when the rm link is slower
                   than the um link (specifically the BT scenario)
04/20/05    mct    Removed references to phys_link down as part of flow chgs.
05/25/04    vr     Fixed checkin error
04/30/04    vr     Added ds707_rmsm_is_in_network_model_call()
04/30/04    kvd    Removed  DS707_RMSM_UM_PPP_UP_EV (not used)
03/02/04    kvd    Added new event DS707_RMSM_UM_MIP_IP_ADDR_CHANGED_EV.
02/05/04    usb    Removed iface_ptr as an argument to init fn. Obtained
                   through routing. Removed unused variables/functions.
02/02/04    jd     Removed MIP DOWN event, renamed MIP UP as UM IFACE UP
10/31/03    usb    Added prototype for ds707_rmsm_notify_ppp_up().
07/20/03    usb    Removed ACL post proc function (which should not reside 
                   here), modified ds707_rmsm_is_iface_in_use().
05/05/03    usb    Incorporated Code review comments. Moved state enum and
                   RMSM info block to .c file since all that is private.
04/11/03    ak     Updated all vars/functions to include ds707_, as there
                   were naming collisions with GPRS.
01/20/03    ar     Added dsat_dial_modifier_info_type to 
                   ds707_rmsm_dial_str_cb interface.
11/19/02    usb    Added ACL processing and post processing functions to 
                   check if UM iface is in use
11/14/02    jd/usb Renamed RMSM_WAITING_FOR_RM_UM_DOWN_STATE to 
                   RMSM_RM_UP_WAITING_FOR_UM_DOWN_STATE 
            jd     Removed um_mip_changed, added rmsm_is_packet_call()
09/10/02    ak     Updated for new dial string handler.
08/28/02    usb    Added/renamed some event names
05/14/02    usb    Re-created module (original filename - dsmip_rmsm.h)
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"

#ifdef FEATURE_DATA_IS707
#include "dstask_v.h"
#include "dsat_v.h"
#include "ps_iface.h"

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
/*---------------------------------------------------------------------------
  Declaration of the events of the state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  DS707_RMSM_MIN_EV = -1,
  DS707_RMSM_RM_WANTS_PKT_CALL_EV = 0,     /* Rm wants packet call         */
  DS707_RMSM_RM_IFACE_DOWN_EV = 1,         /* Rm interface is down         */
  DS707_RMSM_UM_IFACE_DOWN_EV = 2,         /* Um interface is down         */
  DS707_RMSM_UM_PHYS_LINK_UP_EV = 3,       /* Traffic channel is up        */
  DS707_RMSM_UM_PHYS_LINK_GONE_EV = 4,
  DS707_RMSM_UM_PPP_DOWN_EV = 5,           /* Um PPP is down               */
  DS707_RMSM_RM_PPP_UP_EV = 6,             /* Rm PPP is up                 */
  DS707_RMSM_UM_IFACE_UP_EV = 7,           /* Um MIP is up                 */
  DS707_RMSM_UM_MIP_IP_ADDR_CHANGED_EV = 8,
  DS707_RMSM_E2E_PPP_UP_EV = 9,            /* End to End PPP is UP         */
  DS707_RMSM_UM_RESYNC_EV = 10,            /* Resync on UM                 */
  DS707_RMSM_RM_RESYNC_EV = 11,            /* Resync on RM                 */
  DS707_RMSM_MAX_EV
} ds707_rmsm_event_type;
           
/*---------------------------------------------------------------------------
  Define different Watermark Levels for Forward Link Tx Watermark based on
  whether RevB is defined or not.
---------------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REVB 
  #ifdef FEATURE_HDR_REVB_ENH_RATES
    #define DS707_RMSM_REVB_FWD_LO_WATERMARK 350000
    #define DS707_RMSM_REVB_FWD_HI_WATERMARK 400000
  #else /* FEATURE_HDR_REVB_ENH_RATES */
    #define DS707_RMSM_REVB_FWD_LO_WATERMARK 500000
    #define DS707_RMSM_REVB_FWD_HI_WATERMARK 550000
  #endif /* FEATURE_HDR_REVB_ENH_RATES */

  #define DS707_RMSM_REVB_REV_LO_WATERMARK 150000
  #define DS707_RMSM_REVB_REV_HI_WATERMARK 200000
  #define DS707_RMSM_REVB_REV_DONT_EXCEED_CNT 500000

#endif

#define DS707_RMSM_REVA_FWD_LO_WATERMARK 15000
#define DS707_RMSM_REVA_FWD_HI_WATERMARK 40000
/* RMSM FWD link DNE = RLP NAK abort time ( 500 ms) * Rev A FWD link data rate ( 3.1 Mbps)/8*/
#define DS707_RMSM_REVA_FWD_DONT_EXCEED_CNT 200000

#define DS707_RMSM_REVA_REV_LO_WATERMARK 25000
#define DS707_RMSM_REVA_REV_HI_WATERMARK 50000
#define DS707_RMSM_REVA_REV_DONT_EXCEED_CNT 100000

/*---------------------------------------------------------------------------
 Watermark for RM FWD and REV links
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 ds707_rmsm_fwd_lo_wmk;
  uint32 ds707_rmsm_fwd_hi_wmk;
  uint32 ds707_rmsm_fwd_dont_exceed_cnt;
  uint32 ds707_rmsm_rev_lo_wmk;
  uint32 ds707_rmsm_rev_hi_wmk;
  uint32 ds707_rmsm_rev_dont_exceed_cnt;
} ds707_rmsm_wmk_type;
  
          
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DS707_RMSM_INIT()

  DESCRIPTION
    This function initializes the Rm state machine.
                                                                                        
  PARAMETERS
    None

  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean ds707_rmsm_init
(
  void
);


/*===========================================================================
  FUNCTION DS707_RMSM_POST_EVENT()

  DESCRIPTION
    This function receives posted event and queues the corresponding 
    command to be processed in the ps task.
    
  PARAMETERS
    event: the event that is being posted to the state machine.

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern void ds707_rmsm_post_event
(
  ds707_rmsm_event_type event
);


/*===========================================================================
  FUNCTION DS707_RMSM_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to RMSM in DS task and calls 
    an appropriate function to handle it.

  PARAMETERS
    ds_cmd_ptr: Cmd buffer passed with the cmd 

  RETURN VALUE
    None

  DEPENDENCIES
    RM State Machine should have already been initialized.

  SIDE EFFECTS
    None
===========================================================================*/
extern void ds707_rmsm_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
);


/*===========================================================================
  FUNCTION DS707_RMSM_PROCESS_RM_IFACE_FLOW_CMD()

  DESCRIPTION
    This function processes the IFACE flow commands for the RM interface

  PARAMETERS
    ds_cmd_ptr: Cmd buffer passed with the cmd 

  RETURN VALUE
    None

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void ds707_rmsm_process_rm_iface_flow_cmd
(
  ds_cmd_type  *cmd_buf
);

/*===========================================================================
  FUNCTION DS707_RMSM_IS_PACKET_CALL()

  DESCRIPTION
    Returns true if we're in a packet call

  PARAMETERS
    None

  RETURN VALUE
    TRUE - If RM is in IS707 laptop pkt data call
    FALSE - Otherwise

  DEPENDENCIES
    ds707_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
extern boolean ds707_rmsm_is_packet_call
(
  void
);


/*===========================================================================
  FUNCTION DS707_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function processes the dial string it has registered for and
    initiates the appropriate data call over IS707_PKT_IFACE.

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only
    modifiers: Flags passed as part of the dial string

  RETURN VALUE
    Abort handler callback.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
extern dsat_call_abort_cb_type ds707_rmsm_dial_str_cb
(
  const byte *dial_string,
  boolean    digit_mode,
  dsat_dial_modifier_info_type  * modifiers,
  sys_modem_as_id_e_type        subs_id
);


/*===========================================================================
FUNCTION DS707_RMSM_NOTIFY_PPP_UP

DESCRIPTION
  This function notifies RMSM that the end to end PPP is complete during a 
  SIP network model call.  This means that a SIP network model is 
  actually "UP" and hence RMSM sets the iface states appropriately.
    - Move SIO iface to UP state since nothing special needs to be done. 
    - Move IS707 to Routeable state since we are in a laptop call. 
    - Put the phys link state on SIO iface to UP so that PPP can 
      gracefully terminate during call  termination.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ds707_rmsm_notify_ppp_up
(
  void
);

/*===========================================================================
FUNCTION DS707_RMSM_IS_IN_NETWORK_MODEL_CALL

DESCRIPTION
  This function returns whether we are currently in a simple IP network
  model laptop call
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if we are in a SIP network model call, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_rmsm_is_in_network_model_call
( 
  void
);

/*===========================================================================
FUNCTION      DS707_PKT_IS_RM_IFACE_UP

DESCRIPTION   Returns TRUE if the RM iface is up.

DEPENDENCIES  phys_link should be up or coming up.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_rmsm_is_rm_iface_up( void );

/*===========================================================================
FUNCTION DS707_RMSM_IS_NULL()

DESCRIPTION
  This function  returns the state of PPP on tethered device.
  Returns TRUE if RMSM is in NULL state, else returns FALSE.

PARAMETERS
  None

RETURN VALUE
   TRUE if RMSM is NULL state, else returns FALSE.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean ds707_rmsm_is_null
(
  void
);

/*===========================================================================
FUNCTION DS707_RMSM_GET_UM_IFACE()

DESCRIPTION
  This function  returns the UM iface associaetd with the current SN PPP cal

PARAMETERS
  None

RETURN VALUE
   return rmsmi-info.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

ps_iface_type *ds707_rmsm_get_um_iface
(
  void
);
/*===========================================================================
FUNCTION DS707_RMSM_GET_SIO_IFACE()

DESCRIPTION
  This function  returns the SIO iface associaetd with the current SN PPP cal
  Used by BCRM moduel to register for events on RMIface.

PARAMETERS
  None

RETURN VALUE
   return rmsmi-info.sio_pkt_iface.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

ps_iface_type *ds707_rmsm_get_sio_iface
(
  void
);

/*===========================================================================
FUNCTION DS707_RMSM_IS_RELAY_MODEL_CALL()

DESCRIPTION
  This function indicates if a relay model call is active.

PARAMETERS
  None

RETURN VALUE
   Returns True if Relay Call is active, False otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_rmsm_is_relay_model_call
(
  void
);

boolean ds707_rmsm_will_be_relay_model_call
(
  void
);

/*===========================================================================
FUNCTION PS_IFACE_DOWN_V6_V4_IND()

DESCRIPTION
  The given interface has been stopped.     
  THIS FUNCTION SHOULD NOT BE CALLED IN A ISR.

PARAMETERS
  this_iface_ptr: Ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_down_v6_v4_ind
(
  ps_iface_type *iface4_ptr
);

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*===========================================================================
FUNCTION DS707_RMSM_IS_NETMDL_CALL_UP() 

DESCRIPTION
  This function returns whether a network model call is up or not.

PARAMETERS
  None

RETURN VALUE
   TRUE - if network model call is up
   FALSE - if no network model call

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean ds707_rmsm_is_netmdl_call_up(void);
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

/*===========================================================================
FUNCTION PS_IFACE_DOWN_1X_V4_IND()

DESCRIPTION
Posting a down indication on the IPv4 1x interface


PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  down_reason   : Call end reason to be given in IFACE DOWN indication 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_down_1x_v4_ind
(
  ps_iface_type                   *iface4_ptr,
  ps_iface_net_down_reason_type    down_reason
);

/*===========================================================================
FUNCTION DS707_RMSM_GET_CURRENT_STATE()

DESCRIPTION
  This function  returns the current state of the RMSM

PARAMETERS
  None

RETURN VALUE
   Returns RMSM state (integer).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds707_rmsm_get_state(void);

/*===========================================================================
FUNCTION DS707_IS_RMSM_IN_SIP_STATE()

DESCRIPTION
  This function returns TRUE if RMSM is in one of the SIP states

PARAMETERS
  None

RETURN VALUE
   Returns TRUE if RMSM is in one of the SIP states

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_is_rmsm_in_SIP_state(void);

/*===========================================================================
FUNCTION DS707_IS_RMSM_IN_NON_MIP_SIP_STATE()

DESCRIPTION
  This function returns TRUE if RMSM is in any state other than MIP/SIP states

PARAMETERS
  None

RETURN VALUE
   Returns TRUE if RMSM is in any state other than MIP/SIP states

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_is_rmsm_in_non_MIP_SIP_state(void);

/*===========================================================================
FUNCTION DS707_RMSM_UPDATE_WMK_VAULE()

DESCRIPTION
  This function updates the RM FWD/REV link Watermark value based on 
  technology info getting from HDR 
  
PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_update_wmk_value
(
  void
);

/*===========================================================================
FUNCTION   DS707_RMSM_PKT_BRING_UP_CMD()

DESCRIPTION 
  Function that processes DS_707_RMSM_BRING_UP_CMD. This is the cmd handler 
  for bringing up 707 iface during a Relay or Network Model call.  
  Since a special handling is required for bringing up Relay or Netmodel 
  SIP call, RMSM provides its own cmd handler rather than using the 
  one provided by IS707 controller
 
DEPENDENCIES
  None

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 
   
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_pkt_bring_up_cmd
(
  ds_cmd_type  *cmd_ptr
);
/*===========================================================================
FUNCTION   DS707_RMSM_GRACEFUL_PPP_TEAR_DOWN_CMD()

DESCRIPTION 
 This callback function is the cmd handler for tearing down 707 iface
 in Low Power Mode. When the tear down is due to inactivity, we cant to handle
 the case differently because we want graceful close. In other cases, the
 ds707_rmsm_pkt_tear_down_cmd() is called.
 
DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr: iface pointer
  user_data: user data pointer
 
RETURN VALUE
  -1 for failure, 0 for success

SIDE EFFECTS
  None
===========================================================================*/
int ds707_rmsm_graceful_ppp_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void          *user_data
);

/*===========================================================================
FUNCTION   DS707_RMSM_PKT_TEAR_DOWN_CMD()

DESCRIPTION 
  This callback function is the cmd handler for tearing down 707 iface
  during a Relay or Network Model call.  Since a special handling is
  required for tearing down Relay or Netmodel SIP call, RMSM provides its
  own cmd handler rather than using the one provided by IS707 controller
 
DEPENDENCIES
  None

PARAMETERS
  this_iface_ptr: iface pointer
  user_data: user data pointer
 
RETURN VALUE
  -1 for failure, 0 for success

SIDE EFFECTS
  None
===========================================================================*/
int ds707_rmsm_pkt_tear_down_cmd
(
  ps_iface_type *this_iface_ptr,
  void *user_data
);

/*===========================================================================
FUNCTION   DS707_RMSM_TEAR_DOWN_CMD()

DESCRIPTION 
  Function that processes DS_707_RMSM_TEAR_DOWN_CMD. The lpm_mode is used to 
  determine whether Low Power mode is enabled. If it is enabled we
  need to check whether the tear_down is because of inactivity or normal
  teardown and take action accordingly. This is done by calling the function
  ds707_rmsm_graceful_ppp_tear_down_cmd(). In all other cases the normal
  ds707_rmsm_pkt_tear_down_cmd() is called.
 
DEPENDENCIES
  None

PARAMETERS
  ds_cmd_ptr: Cmd buffer passed with the cmd 
   
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_tear_down_cmd
(
  ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION DS707_RMSM_GET_QCMIP_VAL

DESCRIPTION
  This function returns the qcmip val.

PARAMETERS
  None

RETURN VALUE
   UINT32 : qcmip value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 ds707_rmsm_get_qcmip_val(void);


/*===========================================================================
FUNCTION DS707_RMSM_GET_WMK_VAL

DESCRIPTION
  This function returns watermarks low and high levele value

PARAMETERS
  None

RETURN VALUE
   None 

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_rmsm_get_wmk_val
(
  ds707_rmsm_wmk_type *rmsm_wmk_type_ptr
);
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_RMSM_H */
