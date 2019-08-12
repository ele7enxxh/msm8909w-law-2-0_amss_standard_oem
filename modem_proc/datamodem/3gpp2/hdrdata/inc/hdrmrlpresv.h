#ifndef HDRMRLPRESV_H
#define HDRMRLPRESV_H

/*===========================================================================
                                   H D R M R L P R E S V
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrmrlpresv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
09/04/09   ss      CMI SU level modifications.
10/23/06   spn     Changes to flush all the pending Reservation Messages OTA.
10/17/06   kvd     Added type definitions for functions used as args.
09/08/06   spn     Updated the interaction with RASM.
08/03/06   as      Added new type for function pointer to allow RPC for 7500
07/21/06   kvd      7500 re-arch.
07/06/06   sk      QoS 2.0 Support
10/21/05   spn     Added a new method, hdrmrlpresv_is_resv_on()
08/08/05   sk/sy   Merged semi-dynamic QOS changes from sandbox.
04/15/05   sy      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#ifdef FEATURE_HDR_QOS
#include "ds707_qosdefs.h"
#include "hdrcom_api.h"
#include "hdrmrlp_api.h"
#ifdef FEATURE_NIQ_EHRPD
  #include "ds707_nw_init_qos_mgr.h"
#endif
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

typedef enum
{
 HDRMRLP_RESV_STATE_NONE,      /* used internally */
 HDRMRLP_RESV_STATE_OPENING,   /* When a open request is sent to AN waiting for response */
 HDRMRLP_RESV_STATE_OPEN,      /* Received a response from AN for Resv On Request or AN
                                  sent Reservation On*/
 HDRMRLP_RESV_STATE_CLOSING,   /* When a close request is sent to AN waiting for response */
 HDRMRLP_RESV_STATE_CLOSE      /* Received a response from AN for Resv Off Request or AN
                                  sent Reservation Off */
}hdrmrlp_resv_state_enum_type;

typedef struct
{
  uint8 resv_label;                              /* Reservation label used to identify
                                                    each IP flow characteristics */
  ds707_direction_enum_type dirn;               /* Direction of the resv_label  */
  boolean notify_AN;                             /* Whether the request is implicit
                                                    or explicit*/
  errno_enum_type err;                           /* Status                       */
}hdrmrlp_resv_info_type;


/*---------------------------------------------------------------------------
TYPEDEF HDRMRLP_RESV_STATE_CHANGE_CB_TYPE

DESCRIPTION
   Type of callback function registered with RESV module on reservation
   change.
---------------------------------------------------------------------------*/

typedef void (*hdrmrlpresv_resv_state_change_cb_type) (uint8 resv_label,
                        ds707_direction_enum_type dirn,
                        ds707_sec_pkt_flow_mgr_cb_enum_type cback_qualifier,
                        hdrmrlp_resv_state_enum_type state);

/*---------------------------------------------------------------------------
TYPEDEF HDRMRLPRESV_RESV_ON_SENT_CB_TYPE

DESCRIPTION
   Type of callback function registered with RESV module on reservation
   sent.
---------------------------------------------------------------------------*/

typedef void (*hdrmrlpresv_resv_on_sent_cb_type)(uint8 resv_label,
                       ds707_direction_enum_type dirn);


/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/
/*==========================================================================
FUNCTION      HDRMRLP_REG_RESV_STATE_CHANGE_NOTIFY_FUNC

DESCRIPTION   This registers the function to notify if a new reservation
               label state change happens via the over the air messages
              If a NULL function is passed, the "notify Reservation state
              change" function is deregistered.

DEPENDENCIES  A valid reservation label must be passed into this function.

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
extern errno_enum_type hdrmrlpresv_reg_resv_state_change_notify_func
(
  uint8 resv_label,
  ds707_direction_enum_type dirn,
  hdrmrlpresv_resv_state_change_cb_type resv_state_change_func,
  hdrmrlpresv_resv_on_sent_cb_type resv_on_sent_func
);

/*==========================================================================
FUNCTION      HDRMRLP_DEREG_RESV_STATE_CHANGE_NOTIFY_FUNC

DESCRIPTION   This deregisters the function to notify the state change callback.

DEPENDENCIES  A valid reservation label must be passed into this function.

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
extern errno_enum_type hdrmrlpresv_dereg_resv_state_change_notify_func
(
  uint8 resv_label,
  ds707_direction_enum_type dir
);


/*===========================================================================

FUNCTION HDRMRLP_REQUEST_RESERVATION_OFF

DESCRIPTION
 This function is called to request for Reservation ON message to be sent
  transition to the "Open" state.

DEPENDENCIES

PARAMETERS
   num_resv : Total number of reservation entities that need to send
              reservation on request at one time.
   resv_info: Pointer to all the reservations that needs processing.
   send_all_pending_resv_msgs: flushes all the pending reservations 
              immediately.  

RETURN VALUE
  Whether  the funciton is able to process successfully.

SIDE EFFECTS
  None.

===========================================================================*/
extern errno_enum_type hdrmrlpresv_request_reservation_off
(
  int num_resv,
  hdrmrlp_resv_info_type resv_info[],
  boolean send_all_pending_resv_msgs
);


/*===========================================================================

FUNCTION HDRMRLP_REQUEST_RESERVATION_ON

DESCRIPTION
  This function is called to request for Reservation ON message to be sent
  transition to the "Open" state.

DEPENDENCIES

PARAMETERS
   num_resv : Total number of reservation entities that need to send
              reservation on request at one time.
   resv_info: Pointer to all the reservations that needs processing.
   send_all_pending_resv_msgs: flushes all the pending reservations 
              immediately.
RETURN VALUE
  Whether  the funciton is able to process successfully.

SIDE EFFECTS
  None.

===========================================================================*/
extern errno_enum_type hdrmrlpresv_request_reservation_on
(
  int num_resv,
  hdrmrlp_resv_info_type resv_info[],
  boolean send_all_pending_resv_msgs
);

/*===========================================================================
FUNCTION       HDRMRLPRESVI_BLD_SIG_MSGS

DESCRIPTION    Callback timer called whenever the timer expires. This function
               goes through all the reservations and checks if it needs to send
               a signalling message. If it is set then it builds a signalling 
               message.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
extern void hdrmrlpresv_bld_sig_msgs
(
  void
);
/*===========================================================================
FUNCTION  HDRMRLP_IS_RESV_ON

DESCRIPTION       
    This method returns the current stable state of the reservation. Whether 
	the reservation is on or off. 

DEPENDENCIES

RETURN VALUE
  TRUE implies that the reservation state is on
  FALSE implies that the reservation state is off

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrmrlpresv_is_resv_on
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

/*===========================================================================
FUNCTION  HDRMRLPRESV_IDLE_STATE_RESV_ATTRIB_EXECUTE

DESCRIPTION  This function is called whenever the traffic channel goes up/down.
             It will get the value of the ReservationKKIdle bit and apply
             it to all the reservation whenever a connection is gone up/down.

DEPENDENCIES  NONE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrcom_errno_enum_type hdrmrlpresv_idle_state_resv_attrib_execute
(
  boolean
);


/*===========================================================================
FUNCTION      HDRMRLPRESV_MSG_CB

DESCRIPTION   Called from hdrpac_mdm if there are signalling messages for 
              reservation module.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  NONE

===========================================================================*/
void hdrmrlpresv_msg_cb 
(
  dsm_item_type *msg_ptr
);

/*===========================================================================
FUNCTION      HDRMRLPRESV_IS_RESV_USED

DESCRIPTION   Utility function. 
              TRUE: if the reservation is in use or the reservation is out 
                    of bound
              FALSE: If the reservation is not in use

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  NONE

===========================================================================*/
boolean hdrmrlpresv_is_resv_used
(
  uint8 resv_label,
  ds707_direction_enum_type dirn
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /*TEST_FRAMEWORK*/
#endif /* FEATURE_HDR_QOS */

#endif /* HDRMRLPRESV_H */

