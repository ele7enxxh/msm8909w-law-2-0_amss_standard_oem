#ifndef DHCP_CLIENT_EVENT_H
#define DHCP_ClIENT_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  D H C P _ C L I E N T _ C O R E _ V 4 . C

GENERAL DESCRIPTION
  DMSS Dynamic Host Configuration Protocol client core code.  This
  file provides the necessary the DHCP protocol logic, message
  generation, processing and so on.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005-2008 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/src/dhcp_client_event.h#1 $ 

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/26/08    am     RVCT compiler warning fixes.
09/09/05    clp    Split this file out of dhcp_client_core_v4.
===========================================================================*/

/*===========================================================================
  NOTE: changes to the following diag event payload enums and
  structures MUST be coordinated with the diag/tools team.  
===========================================================================*/

typedef enum {
  DHCP_CLIENT_EVENT_REQUEST,
  DHCP_CLIENT_EVENT_DISCOVER,
  DHCP_CLIENT_EVENT_OFFER,
  DHCP_CLIENT_EVENT_ACK,
  DHCP_CLIENT_EVENT_NAK,
  DHCP_CLIENT_EVENT_RELEASE,
  DHCP_CLIENT_EVENT_INFORM,
  DHCP_CLIENT_EVENT_DECLINE,
  DHCP_CLIENT_EVENT_LEASE_EXPIRE
} dhcp_client_diag_event_enum_type;

/* The types are explicit here to ensure proper sizing */
typedef struct {
  uint32 client_ip_addr;
  uint32 server_ip_addr;
  uint8  event_type; /* dhcp_client_diag_event_enum_type */
  uint8  state;      /* dhcp_client_state_enum */
} dhcp_client_event_payload_type;

#endif /* DHCP_ClIENT_EVENT_H */
