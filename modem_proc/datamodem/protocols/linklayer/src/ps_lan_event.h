#ifndef PS_LAN_EVENT_H
#define PS_LAN_EVENT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  PS_LAN_EVENT . H

GENERAL DESCRIPTION
  This file contains definition of the events and payloads data structure
  to support DIAG for both ARP and 802.1X events.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

   $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $
   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_lan_event.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/09/05    hba    Created files
=========================================================================*/

/*===========================================================================
  NOTE: changes to the following diag event payload enums and
  structures MUST be coordinated with the diag/tools team.
===========================================================================*/


/*---------------------------------------------------------------------------
   Enumeration of the different supported ARP Events
---------------------------------------------------------------------------*/
typedef enum
{
  ARP_REQUEST_INCOMING_EVENT,
  ARP_REQUEST_OUTGOING_EVENT,
  ARP_RESPONSE_INCOMING_EVENT,
  ARP_RESPONSE_OUTGOING_EVENT
} arp_event_enum_type;


/*---------------------------------------------------------------------------
  Definition of the payload of an ARP event
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 ip_addr;
  char   mac_addr[6];
  uint8  event_type;  /* arp_event_enum_type */
  uint8  status;      /* arp_entry_state_enum */
} arp_event_payload_type;



/*---------------------------------------------------------------------------
  Enumeration of the differents 802.1X supported events
---------------------------------------------------------------------------*/
typedef enum
{
  LAN_1X_EAPOL_PKT_EVENT,
  LAN_1X_PAE_CREATED_EVENT,
  LAN_1X_PAE_DELETED_EVENT,
  LAN_1X_SUPPL_FSM_CHANGED_EVENT,
  LAN_1X_SUPPL_TIMEOUT_EVENT,
  LAN_1X_ERROR_EVENT
} lan_1x_event_enum_type;

/*---------------------------------------------------------------------------
 Definition of the payloa of a LAN_1X event!
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  event_type;        /* lan_1x_event_enum_type                      */
  uint8  eapol_pkt_type;
  uint8  port_id;
  uint8  state;             /* Supplicant current FSM state                */
  uint8  nb_retries;
  uint8  error_type;
} lan_1x_event_payload_type;

#endif /* PS_LAN_EVENT_H */

