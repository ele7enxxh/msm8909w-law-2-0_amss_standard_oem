#ifndef _PS_PPP_EVENTS_H
#define _PS_PPP_EVENTS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ P P P _ E V E N T . H

GENERAL DESCRIPTION
  This header file contains the definitions of the enums that are used to
  generate PPP events.

Copyright (c) 2003-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_events.h_v   1.1   27 Feb 2003 15:43:00   jysong  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/api/ps_ppp_events.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/11    op     Data SU API cleanup
10/05/09    mga    Merged changes related to diag events
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
03/22/05    ssh    Added PPP_RESYNC state to ppp_event_state_type
04/02/03    mvl    Complete modularization of the authentication code which
                   entailed adding PPP_EV_INVALID_PROTO to event_proto type
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added include files, and inline documentations etc. 
02/27/03    js     initial revision.
02/25/03    mvl    created file
===========================================================================*/
#include "comdef.h"
#include "event.h"
#include "event_defs.h"
#include "ps_ppp_ext.h"
#include "ps_ppp_defs.h"
#include "ps_ppp_vsncp_3gpp2.h"

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_PROTOCOL_TYPE

DESRIPTION
  enum that defines the various supported PPP protocols - the event mechanism
  will map these to the corresponding strings
---------------------------------------------------------------------------*/
typedef enum 
{
     PPP_EV_LCP,
     PPP_EV_PAP,
     PPP_EV_CHAP,
     PPP_EV_IPCP,
     PPP_EV_IPv6CP,
     PPP_EV_EAP,
     PPP_EV_VSNCP,
     PPP_EV_INVALID_PROTO = 255   /* this should NEVER be used as payload! */
} ppp_event_protocol_type;


/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_STATE_TYPE

DESCRIPTION
  enum that defines the states of the protocol for event repporting.  
---------------------------------------------------------------------------*/
typedef enum
{
   PS_PPP_EVENT_UNKNOWN,
   PS_PPP_EVENT_STARTING, 
   PS_PPP_EVENT_OPEN,
   PS_PPP_EVENT_CLOSING, 
   PS_PPP_EVENT_CLOSED,
   PS_PPP_EVENT_RESYNC
} ppp_event_state_type;

/*--------------------------------------------------------------------------
TYPEDEF PPP_EVENT_AUTH_TYPE

DESCRIPTION
  Indicates the auth mode.      
--------------------------------------------------------------------------*/
typedef enum
{
  PPP_EVENT_AUTH_MIN       = 1,
  PPP_EVENT_AUTH_NONE      = PPP_EVENT_AUTH_MIN,
  PPP_EVENT_AUTH_PAP       = 2,
  PPP_EVENT_AUTH_CHAP      = 3,
  PPP_EVENT_AUTH_EAP       = 4,
  PPP_EVENT_AUTH_INVALID   = 255
}ppp_event_auth_proto_type;

/*--------------------------------------------------------------------------
TYPEDEF PPP_EVENT_PAYLOAD_TYPE

DESCRIPTION
    PPP Event Payload Type
--------------------------------------------------------------------------*/
typedef struct 
{
   int                      ppp_event_device;     /* PPP on dev0 or dev1  */
   ppp_event_protocol_type  ppp_event_protocol;   /* protocol type        */ 
   ppp_event_state_type     ppp_event_state;      /* Current state        */
   ppp_event_state_type     ppp_event_prev_state; /* previous state       */ 
} ppp_event_payload_type;

/*--------------------------------------------------------------------------
TYPEDEF PPP_EVENT_AUTH_PROTO_PAYLOAD_TYPE

DESCRIPTION
    Auth mode negotiated during LCP.
--------------------------------------------------------------------------*/
typedef struct 
{
  uint8                     ppp_event_device;   /* PPP on dev0 or dev1  */
  ppp_event_auth_proto_type ppp_event_auth;     /* Auth_Mode negotiated */
}ppp_event_auth_proto_payload_type;


/*--------------------------------------------------------------------------
TYPEDEF PPP_EVENT_VSNCP_PAYLOAD_TYPE

DESCRIPTION
    VSNCP Event Information.
--------------------------------------------------------------------------*/
typedef struct 
{
  uint8                        ppp_event_device;     /* PPP on dev0 or dev1*/
  uint8                        pdn_id ;              /* PDN ID             */
  vsncp_3gpp2_pdn_type         pdn_type;             /* PDN type           */
  ppp_event_state_type         ppp_event_state;      /* Current state      */
} ppp_event_vsncp_payload_type;


/*--------------------------------------------------------------------------
TYPEDEF PPP_EVENT_IID_PAYLOAD_TYPE

DESCRIPTION
    IPv6 IID Event Information.
--------------------------------------------------------------------------*/
typedef struct 
{
  uint8                        ppp_event_device;     /* PPP on dev0 or dev1*/
  char                         iid[20];              /* V6 IID             */
} ppp_event_iid_payload_type;



/*===========================================================================
FUNCTION PPP_REPORT_EVENT_TO_DIAG()

DESCRIPTION
  Creates the ppp event payload per the information passed in, and sends the
  event to DIAG via event_report_payload()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pppi_report_event_to_diag
(
  event_id_enum_type event_id,
  ppp_dev_enum_type device,
  ppp_protocol_e_type protocol,
  ppp_event_state_type prev_state,
  ppp_event_state_type state
);

#endif /* _PS_PPP_EVENTS_H */
