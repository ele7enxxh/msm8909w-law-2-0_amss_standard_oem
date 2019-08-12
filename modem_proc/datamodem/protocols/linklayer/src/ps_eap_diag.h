#ifndef PS_EAP_DIAG_H
#define PS_EAP_DIAG_H

/*===========================================================================

     E X T E N S I B L E  A U T H E N T I C A T I O N  P R O T O C O L

           D I A G   I N T E R F A C E   D E F I N I T I O N S


This file defines the DIAG interface (events, logs) used by EAP.

Changes to these enums must be coordinated with the diag/tools team.
As of the creation date mail had to be sent to asw.diag.request.

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_eap_diag.h#1 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/08   rnp     Fixed Compiler warnings
09/08/05   lyr     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_DATA_PS_EAP
#include "event.h"
#include "log.h"


/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
The following indicates the nature of the EAP_DIAG_EVENT.
--------------------------------------------------------------------------*/
typedef enum
{
  EAP_PEER_DIAG_REQ_EV                =  0,
  EAP_PEER_DIAG_RSP_EV                =  1,
  EAP_PEER_DIAG_AUTH_SERVER_IND_EV    =  2,
  EAP_PEER_DIAG_AUTH_LOCAL_IND_EV     =  3
  
} eap_peer_diag_event_enum_type;


/*---------------------------------------------------------------------------
  Status values
---------------------------------------------------------------------------*/
typedef enum
{
  EAP_DIAG_EVENT_SUCCESS = 0,
  EAP_DIAG_EVENT_FAILURE = 1
} eap_diag_status_enum_type;


/*--------------------------------------------------------------------------
The payload for the EAP_EVENT indicates:
 
 Event (which is being logged)
 State (in which event happened)
 Type : as per RFC 3748
 Status: success or failure

 The display order should be:
 "Receive EAP Event: , Type: , State: , Status: "
--------------------------------------------------------------------------*/
typedef struct
{                                           // Display:
  uint32  type;
  uint8   event_type;
  uint8   state;
  uint8   status;
}eap_diag_event_payload_type;


/*---------------------------------------------------------------------------
  Macros to make code more readable
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Allocate an event payload holder
---------------------------------------------------------------------------*/
  #define EAP_DIAG_EVENT_ALLOC( payload ) \
    eap_diag_event_payload_type  (payload)
     
/*---------------------------------------------------------------------------
  Set the event payload values
---------------------------------------------------------------------------*/
  #define EAP_DIAG_EVENT_SET( payload_ptr_, type_, ev_, state_, status_ )\
   do                                                    \
   {                                                     \
      (payload_ptr_)->type           = (type_);          \
      (payload_ptr_)->event_type     = (ev_);            \
      (payload_ptr_)->state          = (state_);         \
      (payload_ptr_)->status         = (status_);        \
   } while(0)
    
/*---------------------------------------------------------------------------
  Report the event
---------------------------------------------------------------------------*/
  #define EAP_DIAG_EVENT_REPORT( payload )                       \
   do                                               \
   {                                                \
     event_report_payload( EVENT_EAP,               \
       sizeof( payload ),                           \
       &payload );                                  \
   } while(0)
#endif /* FEATURE_DATA_PS_EAP */
#endif   /* PS_EAP_DIAG_H */
