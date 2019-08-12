#ifndef HDRSAP_H
#define HDRSAP_H

/*===========================================================================

            S I G N A L I N G   A D A P T A T I O N  P R O T O C O L
                           D E F I N I T I O N S

DESCRIPTION
  This contains the external declarations for the Signaling Adaptation
  Protocol (SAP)

Copyright (c) 2011 - 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrsap.h#1 $DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/26/13   vko     Do not start SAP backoff timer in all cases of AC failure
02/04/13   cnx     Don't reset tunnel info if AT comes back to the same cell
                   after OOS. 
10/07/12   cnx     Clean up RASM before idle handoff to native HDR network.
09/11/12   cnx     Handle TX and RX messages during active handoff
06/19/12   cnx     AT moves back to tunnel after getting CGI if tunnel mode 
                   is enabled.
03/01/12   cnx     Added hdrsap_stop_backoff_timer().
01/27/11   cnx     Support pre-Registration status update.
01/05/12   sju     Added verbose debug f3 support for SAP protocols. 
12/05/11   cnx     Support hdrcp_config_info.conf.
11/28/11   cnx     Support connection abort;
                   Support NV for set preReg status.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/28/11   pba     Optimized handoff phase 2 support
10/03/11   cnx     Added HDRSAP_TUNNEL_MODE_NO_SRV.
09/14/11   cnx     Support retrieving CDMA system time from LTE via message
                   interface during LTE to HDR Optimized Handoff.
08/18/11   cnx     Added hdrsap_connection_is_open(). 
04/11/11   cnx     Fixed compiler errors.
01/01/11   pba     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "hdrhai.h"
#include "dsm.h"
#include "hdrmc_v.h"
#include "comdef.h"
#include "hdrerrno.h"
#include "hdrind.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "emm_irat_if_msg.h"
#include "lte_rrc_ext_msg.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define HDRSAP_BACKOFF_TIME_TABLE_L2_FAILED "/nv/item_files/modem/hdr/cp/sap/backofftimel2"
#define HDRSAP_SET_PREREG_STATUS "/nv/item_files/modem/hdr/cp/sap/setpreregstatus"

/* AlternateLinkOpenReq open reason */
#define HDRSAP_ALT_LINK_OPEN_REQ_REASON_AT_INIT 0x00
#define HDRSAP_ALT_LINK_OPEN_REQ_REASON_AN_INIT 0x01

/* connect reason bit pattern */
#define HDRSAP_CR_USER       0x01 /* user requests to open a connection */
#define HDRSAP_CR_MSG        0x02 /* open a connection to send RTC messages */
#define HDRSAP_CR_AN         0x04 /* AN requests to open a connection */ 
#define HDRSAP_CR_ABORT      0x08 /* user abort the open conn request */

typedef enum
{
  HDRSAP_ACTIVATE_CMD           = 0, /* Activate SAP */
  HDRSAP_DEACTIVATE_CMD         = 1, /* Deactivate SAP */
  HDRSAP_OPEN_CONNECTION_CMD    = 2, /* Open Connection */
  HDRSAP_CLOSE_CONNECTION_CMD   = 3, /* Close connection */
  HDRSAP_IDLE_HANDOFF_CMD       = 4, /* Idle handoff */
  HDRSAP_ACTIVE_HO_CMD          = 5, /* Active handoff */
  HDRSAP_CONTINUE_ACTIVE_HO_CMD = 6  /* Continue active handoff */

} hdrsap_cmd_name_enum_type;

typedef enum
{
  HDRSAP_DEACT_ACQ_HDR      = 0, /* Will exit tunnel and acquire HDR */
  HDRSAP_DEACT_NO_SRV       = 1  /* Deactivate because no LTE service */

} hdrsap_deact_reason_enum_type;

typedef struct
{
  hdrsap_deact_reason_enum_type reason; /* Deactivate reason */

} hdrsap_deactivate_cmd_type;

typedef struct
{
  hdrhai_open_reason_enum_type reason; /* Open connection reason */

} hdrsap_open_connection_cmd_type;

typedef struct
{
  hdrhai_abort_reason_enum_type reason; /* Close connection reason */
  boolean silent_close;

} hdrsap_close_connection_cmd_type;

typedef struct
{
  hdrmc_lte_to_hdr_active_ho_info_type *ho_msg_ptr; /* Pointer to HO message */

} hdrsap_continue_active_ho_cmd_type;

typedef union
{
  hdrsap_deactivate_cmd_type deactivate;
  hdrsap_open_connection_cmd_type open_connection;
  hdrsap_close_connection_cmd_type close_connection;
  hdrsap_continue_active_ho_cmd_type continue_active_ho;

} hdrsap_cmd_params_union_type;

typedef struct
{
  hdrsap_cmd_name_enum_type      name;    /* Name of the command */
  hdrhai_protocol_name_enum_type sender;  /* Protocol giving command */
  hdrsap_cmd_params_union_type   params;  /* Command parameters */

} hdrsap_cmd_type;

typedef enum
{
  HDRSAP_STATE_TIMER,      /* SAP state timer */
  HDRSAP_BACKOFF_TIMER     /* SAP backoff timer */

} hdrsap_timer_enum_type;

typedef enum
{
  HDRSAP_NO_MSG = 0,
  HDRSAP_UATI_REQ = 1,       
  HDRSAP_LINK_OPEN_REQ = 2  
} hdrsap_ac_msg_enum_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRSAP_POWERUP_INIT

DESCRIPTION
  This function commands SAP to Initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_powerup_init ( void );


/*===========================================================================
FUNCTION HDRSAP_MSGR_START

DESCRIPTION
  Initialize message interface.
 
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_msgr_start( void );


/*===========================================================================
FUNCTION HDRSAP_MSGR_STOP

DESCRIPTION
  Clean up Signaling Adaptation Protocol message interface by doing the following. 
  - De-register the registered messages. 
  - Delete the messenger Client ID for Decoder module. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_msgr_stop( void );


/*===========================================================================
FUNCTION HDRSAP_GET_80MS_TIME

DESCRIPTION
  This function gets CDMA system time in 80ms unit.
 
DEPENDENCIES
  hdrsap_msgr_start() has been called.

PARAMETERS
  sys_time - system time retrieved

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_get_80ms_time
( 
  qword sys_time
);


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_ACTIVATE

DESCRIPTION
  This function commands SAP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_activate
(
  hdrhai_protocol_name_enum_type sender
);


/*===========================================================================
FUNCTION HDRSAP_DEACTIVATE

DESCRIPTION
  This function commands HDRSAP to deactivate

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  reason - Deactivate reason

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_deactivate
(
  hdrhai_protocol_name_enum_type sender,
  hdrsap_deact_reason_enum_type reason
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to SAP.

DEPENDENCIES
  None

PARAMETERS
  sap_cmd_ptr - Pointer to the command for SAP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_process_cmd
(
  hdrsap_cmd_type * smp_cmd_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_PROCESS_IND

DESCRIPTION
  This function processes indications given to SAP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for SAP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);

/*===========================================================================
FUNCTION HDRSAP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to the SAP

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
                         
SIDE EFFECTS
  None.
===========================================================================*/
void hdrsap_msg_cb
(
  dsm_item_type *msg_ptr
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_PROCESS_MSG

DESCRIPTION
  This function processes messages for SAP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_process_msg
(
  dsm_item_type * item_ptr,
  hdrhai_protocol_instance_enum_type protocol_instance
);

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRSAP_PROCESS_TIMER

DESCRIPTION
  This function is an entry point for hdrmc to process the timer message
  that has been queued up.

DEPENDENCIES
  None.
  
PARAMETERS
  timer_id - The id of the expired timer triggering the function.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_process_timer
( 
  uint32 timer_id 
);

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAP_SET_TUNNEL_MODE

DESCRIPTION
  This function sets the tunnel mode value.

DEPENDENCIES
  None

PARAMETERS
  tunnel_mode - value to set the tunnel mode.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_set_tunnel_mode
( 
  hdrhai_tunnel_mode_type tunnel_mode
);

/*===========================================================================
FUNCTION HDRSAP_TUNNEL_MODE_IS_ENABLED

DESCRIPTION
  This function returns whether tunnel mode is enabled or not.

DEPENDENCIES
  None

PARAMETERS
  caller - The name of the calling protocol (used in software trace).

RETURN VALUE
  TRUE is tunnel mode is enabled, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrsap_tunnel_mode_is_enabled 
( 
  hdrhai_protocol_name_enum_type caller
);


/*===========================================================================
FUNCTION HDRSAP_OPEN_CONNECTION

DESCRIPTION
  This function commands SAP to open a connection.

DEPENDENCIES
  None

PARAMETERS 
  reason - The reason the connection is being opened 
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_open_connection
(
  hdrhai_open_reason_enum_type reason,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRSAP_ALT_LINK_OPEN_COMP_TX_CB_IS_RECEIVED

DESCRIPTION
  This function returns whether AltLinkOpenComp msg 
  tx outcome cb is received or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE is msg outcome cb is received, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsap_alt_link_open_comp_tx_cb_is_received( void );

/*===========================================================================
FUNCTION HDRSAP_CLOSE_CONNECTION

DESCRIPTION
  This function commands SAP to close a connection.

DEPENDENCIES
  None

PARAMETERS
  abort_reason - reason to abort connection 
  sender - The protocol giving this command (only for software trace)
  silent_close - whether to send AltLinkCloseReq or not

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_close_connection
(
  hdrhai_abort_reason_enum_type abort_reason,
  hdrhai_protocol_name_enum_type sender,
  boolean silent_close
);


/*=============================================================================

FUNCTION HDRSAP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SAP.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Indication to process, is a global enum for all
             possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdrsap_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr
);

/*===========================================================================
FUNCTION HDRSAP_IDLE_HANDOFF

DESCRIPTION
  This function commands SAP to initiate idle handoff procedure.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_idle_handoff
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================
FUNCTION HDRSAP_ACTIVE_HANDOFF

DESCRIPTION
  This function commands SAP to process active handoff from LTE.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_active_handoff
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================
FUNCTION HDRSAP_CONTINUE_ACTIVE_HANDOFF

DESCRIPTION
  This function commands SAP to continue processing active handoff after 
  ALMP has entered IDLE state.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)
  payload - pointer to dsm item that has the OLP + HSP + TCA messages.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_continue_active_handoff
(
  hdrhai_protocol_name_enum_type sender,
  hdrmc_lte_to_hdr_active_ho_info_type *active_ho_payload
);

/*===========================================================================
FUNCTION HDRSAP_CONNECTION_IS_OPEN

DESCRIPTION
  Boolean access function, can be called from any task to check if a
  SAP connection is open.  

DEPENDENCIES
  None

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE if a SAP connection is open, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean hdrsap_connection_is_open
( 
  hdrhai_protocol_name_enum_type caller
);


/*===========================================================================
FUNCTION HDRSAP_LTE_TO_HDR_ACTIVE_HO_IS_IN_PROGRESS

DESCRIPTION
  This function returns whether LTE to HDR active handoff is in progress or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean hdrsap_lte_to_hdr_active_ho_is_in_progress( void ) ;

/*===========================================================================

FUNCTION HDRSAP_GET_CURRENT_TUNNEL_MODE_VALUE

DESCRIPTION
  This function returns the current tunnel mode value.

DEPENDENCIES
  None

PARAMETERS
  caller - protocol that is invoking the function

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

hdrhai_tunnel_mode_type hdrsap_get_current_tunnel_mode_value 
( 
  hdrhai_protocol_name_enum_type caller 
);

/*===========================================================================
FUNCTION HDRSAP_GET_PILOTS

DESCRIPTION
  This function gets HRPD pilot measurement results from LTE ML1
 
DEPENDENCIES
  hdrsap_msgr_start() has been called.

PARAMETERS
  max_pilots - Less than or equal to size of array pointed to by pilots_ptr
  pilots_ptr - Pointer to array of structures comprising all the active and 
               candidate set pilots.  The first pilot is the reference pilot.
  caller - Protocol calling the function, for debug


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int hdrsap_get_pilots
( 
  int max_pilots,
  hdrsrch_pilot_rpt_type * pilots_ptr,
  hdrhai_protocol_name_enum_type caller
);

/*===========================================================================
FUNCTION HDRSAP_SEND_PRE_REG_STATUS_TO_LTE

DESCRIPTION
  This function sends current pre-reg status to LTE

DEPENDENCIES
  None

PARAMETERS
  ue_is_preregistered - current pre-reg status

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsap_send_pre_reg_status_to_lte
( 
  boolean ue_is_preregistered
);


/*===========================================================================
FUNCTION HDRSAP_SET_PREREG_STATUS

DESCRIPTION
  This function notifies HDR about any events that could change the
  preRegistration status.

DEPENDENCIES
  None

PARAMETERS
  hdrhai_prereg_status_event_enum_type - what type of event it is

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsap_set_prereg_status
( 
  hdrhai_prereg_status_event_enum_type prereg_status_event
);


/*===========================================================================

FUNCTION HDRSAP_GET_RUP_MSG

DESCRIPTION
  This function returns a pointer to DSM item that contains the stored RU
  message.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Pointer to DSM item containing RU message.

SIDE EFFECTS
  None
===========================================================================*/

dsm_item_type* hdrsap_get_rup_msg ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSAP_CLAER_RUP_MSG

DESCRIPTION
  This function clears RUP message pointer stored in HDRSAP.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsap_clear_rup_msg ( void );

/*===========================================================================
FUNCTION HDRSAP_GET_TIME_SEND_PREREG_STATUS

DESCRIPTION
  This function returns when UE considers itself to be pre-Registered and
  accordingly set pre-Registration status to TRUE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  At what point the UE considers itself to be pre-Registered.

SIDE EFFECTS
  None
===========================================================================*/
hdrhai_prereg_status_enum_type hdrsap_get_time_set_prereg_status( void );

/*===========================================================================
FUNCTION HDRSAP_RESET_ABORT_CONNECTION

DESCRIPTION
  This function resets the abort connection flag.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_reset_abort_connection( void );

/*===========================================================================
FUNCTION HDRSAP_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrsap_set_verbose_debug
(
  uint8 verbose_debug_setting
);

/*===========================================================================

FUNCTION HDRSAP_STOP_BACKOFF_TIMER

DESCRIPTION
  This function stops backoff timer.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_stop_backoff_timer( void );


/*===========================================================================

FUNCTION HDRSAP_STOP_ALT_LINK_OPEN_CONF_TIMER

DESCRIPTION
  This function stops the connection setup timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_stop_alt_link_open_conf_timer( void );


/*===========================================================================

FUNCTION HDRSAP_STOP_ALT_LINK_CLOSE_CONF_TIMER

DESCRIPTION
  This function stops the Alternate link close conf timer if it is active

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_stop_alt_link_close_conf_timer( void );

/* <EJECT> */
/*===========================================================================
FUNCTION HDRSAP_CGI_READY

DESCRIPTION
  This function moves protocols to correct state to get CGI

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsap_cgi_ready( void );

/*===========================================================================
FUNCTION HDRSAP_IDLE_HANDOFF_READY

DESCRIPTION
  This function closes virtual connection before idle handoff.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsap_idle_handoff_ready( void );

/*===========================================================================

FUNCTION HDRSAP_CHANGE_AC_MSG_TYPE

DESCRIPTION
  Called to set the access channel message, for which L2ACK 
  is failed

DEPENDENCIES
  None

PARAMETERS
  new_msg_type - hdrsap_ac_msg_enum_type

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsap_set_ac_msg_type
( 
  hdrsap_ac_msg_enum_type new_msg_type
);

/*===========================================================================

FUNCTION HDRSAP_GET_AC_MSG_TYPE

DESCRIPTION
  Called to retrieve the current access channel message, for which L2ACK
  is failed

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  new_msg_type - hdrsap_ac_msg_enum_type

SIDE EFFECTS
  None

===========================================================================*/

hdrsap_ac_msg_enum_type hdrsap_get_ac_msg_type( void );

#endif /* FEATURE_LTE_TO_HDR_OH */
#endif /* HDRSAP_H */
