#ifndef PS_SLIP_IFACE_HDLR_H
#define PS_SLIP_IFACE_HDLR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    P S _ S L I P _ I F A C E _ H D L R . H

                           
DESCRIPTION
  Header file defining the API for the slip iface mode handler. The slip 
  Iface Mode Hdlr is responsible for controlling the SLIP operation. 
  This module owns the SLIP ps_iface.

DEPENDENCIES
  The module MUST execute in the DS context.

EXTERNALIZED FUNCTIONS  
  
  slip_sm_init() - 
    This function is used to initialize the SLIP state machine.
    
  slip_sm_post_event() - 
    This function is used to post an event to the SLIP state machine
    
Copyright (c) 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ps_slip_iface_hdlr.h#1 $ 
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/19/09    am     DS Task De-coupling effort and introduction of DCC task.
01/29/09    pp     Initial release.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_SLIP
#include "ps_iface_defs.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF SLIP_SM_EVENT_TYPE

DESCRIPTION
  Type and static variable defining events supported by the slip iface 
  hdlr sm.
---------------------------------------------------------------------------*/

typedef enum
{
  SLIP_IFACE_HDLR_CP_UP_EV               = 0,
  SLIP_IFACE_HDLR_CP_DOWN_EV             = 1,
  SLIP_IFACE_HDLR_CP_DEACTIVATING_EV     = 2,
  SLIP_IFACE_HDLR_ADDR_CONFIG_SUCCESS_EV = 3,
  SLIP_IFACE_HDLR_ADDR_RELEASED_EV       = 4,
  SLIP_IFACE_HDLR_ADDR_FAILURE_EV        = 5,
  SLIP_IFACE_HDLR_SM_MAX_EV        
} slip_sm_event_type;

typedef union 
{
  slip_sm_event_type               slip_evt;
  ps_flow_type*                    flow_ptr;
} slip_sm_evt_info;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION SLIP_SM_INIT()

DESCRIPTION
  This function is used to initialize the SLIP state machine.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void slip_sm_init
(
  void
);

/*===========================================================================
FUNCTION SLIP_SM_POST_EV()

DESCRIPTION
   This function is used to post an event to the SLIP state machine.

PARAMETERS
  SLIP state machine event type
  SLIP event info which is event specific

RETURN VALUE
  -1 in case of failure
  0 in case of success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int slip_sm_post_ev
(
  slip_sm_event_type event,
  slip_sm_evt_info   *evt_info
);

/*===========================================================================

FUNCTION SLIP_IFACE_HDLR_IS_PREF_MODE

DESCRIPTION
  This function determines if the current mode is preferred mode for allowing
  any data to go over it
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean slip_iface_hdlr_is_pref_mode
( 
  void
);
#endif /* FEATURE_DATA_PS_SLIP */
#endif /* PS_SLIP_IFACE_HDLR_H */

