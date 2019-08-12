#ifndef DS707_EPZID_H
#define DS707_EPZID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               E P Z I D 

GENERAL DESCRIPTION
  This file contains functions used to handle EPZID changes.

EXTERNALIZED FUNCTIONS
  DS707_EPZID_DISABLE_RECON
    Disables dormant re-origination when EPZID changes.
    
  DS707_EPZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    EPZID reconnect parameters.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.h_v   1.1   18 Nov 2002 21:33:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_epzid.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
03/15/11   mg      Global variable cleanup
28/12/09   ms      Changes to expose ds707_epzid_get_current_epzid API at
                   VU Level.
08/05/08   ms      IS707B addendum Feature support.
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
04/05/05   gr      Initial version of the file to support IS707-B
===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "cm.h"

#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_IS707B_ADDENDUM
/*===========================================================================
                                Data Structure Definitions                               
===========================================================================*/

typedef struct 
{
   uint8 timer_val;
} ds707_epzid_hat_type;

typedef struct
{
 uint8 timer_val;
} ds707_epzid_get_hat_status_type;
#endif 

/*===========================================================================
                                TYPEDEFS
===========================================================================*/
/*---------------------------------------------------------------------------
  Type for the service option control message for Packet Zone ID 
  Reconnection Control.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  byte  cntl;                            /* Packet Zone Connection Control */
  byte  list_len;                        /* Length of visited EPZID list   */
} ds707_epzid_socm_msg_type;

/*---------------------------------------------------------------------------
  Type definition for the Enhanced Packet Zone Identifier
---------------------------------------------------------------------------*/
typedef struct
{
  byte  pzid;                    /*   Packet Zone Identifier               */
  word  sid;                     /*   System Identifier                    */ 
  word  nid;                     /*   Network Identifier                   */
#ifdef FEATURE_DS_EPZID_HYSTERESIS
  dword hyst_exp_time;
  boolean ht_expired;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
} ds707_epzid_type;


/*---------------------------------------------------------------------------
  Type definition to store the highest call fail reason and call end status
  from CM. This shall be used by EPZID to verify call status before adding
  the new EPZID onto the list
---------------------------------------------------------------------------*/
typedef struct
{
  cm_call_orig_fail_layer_e_type highest_call_fail_reason;
  cm_call_end_e_type end_status;
} ds707_cm_call_end_info_type;


/*===========================================================================
                       EXTERN VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  Mask to get the EPZID_CTL bits
---------------------------------------------------------------------------*/
#define DS707_EPZID_CNTL_MASK           0xE0

/*---------------------------------------------------------------------------
  Mask to get the LIST_LEN bits
---------------------------------------------------------------------------*/
#define DS707_EPZID_LIST_LEN_MASK       0x0F

/*---------------------------------------------------------------------------
  Values of the EPZID_CNTL field in the Packet Data Zone based Registration/
  Reconnection service option control message.
---------------------------------------------------------------------------*/
#define DS707_EPZID_CNTL_DISABLE          0   /* Disable EPZID recon       */
#define DS707_EPZID_CNTL_ENABLE        0x20   /* Enable EPZID recon        */
#define DS707_EPZID_CNTL_CLEAR_LIST    0x40   /* Clear EPZID list          */

/*---------------------------------------------------------------------------
  Mask to get the LIST_LEN bits
---------------------------------------------------------------------------*/
#define DS707_EPZID_MAX_LEN    16           /* Maximum size of array       */

/* Default Enhanced PZID contains just the PZID      */
#define DEFAULT_ACTIVE_EPZID_TYPE    NV_EPZID_PZID

/*===========================================================================
                                 EXTERNS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_EPZID_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns byte (SOCM Message Id)

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzid_parse_socm
(
  byte           cntl,            /* SOCM CNTL field                       */
  byte           list_len         /* list_len (needs to masked off)       */
);

/*===========================================================================
FUNCTION       DS707_EPZID_RECON_ENABLED

DESCRIPTION    Returns TRUE if reconnection is enabled for EPZID changes.

DEPENDENCIES   None

RETURN VALUE   TRUE if EPZID recon turned on.  Else FALSE.

SIDE EFFECTS   None
===========================================================================*/
extern boolean ds707_epzid_recon_enabled(void);

/*===========================================================================
FUNCTION      DS707_EPZID_DISABLE_RECON

DESCRIPTION   Disables packet zone id based reconnection for packet calls.
              This function deregisters the pzidi_active_change_cb()
              and the pzidi_idle_change_cb(). The list of visited
              packet zones is also cleared.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
             (a) BS sends a SOCTL message disable PZID reconnection
             (b) BS sends a PZID of 0.
             (c) MS comes out of the active state - PPP session has ended.
             (d) MS detects a change in SID.
  
             This function can be called from either the DS or the MC task
             (from the callbacks).

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void  ds707_epzid_disable_recon(void);

/*===========================================================================
FUNCTION      DS707_EPZID_PROCESS_DELAY_TIMER_EXPIRED

DESCRIPTION   This function calls the pzid change handler, passing the 
              pzid contained in element zero of the array, and the new
              pzid as told by MC.  May cause an origination.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void  ds707_epzid_process_delay_timer_expired(void);


/*===========================================================================
FUNCTION      DS707_EPZIDI_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns SOCM Message id

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzidi_parse_socm
(
  byte           cntl,            /* SOCM CNTL field                       */
  byte           list_len         /* list_len (needs to masked off)        */
);

/*===========================================================================
FUNCTION      DS707_EPZIDI_INIT_LIST

DESCRIPTION   This function initializes all the list element to invalid value

DEPENDENCIES  List is already created

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epizdi_init_list(void);

/*===========================================================================
FUNCTION      DS707_EPZID_CHANGE_HANDLER

DESCRIPTION   This function handles EZID change notification and manipulates
              the EPZID list. It also sets the trigger for the EPZID Mgr to
	            re-originate

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_change_handler
( 
  ds707_epzid_type *new_epzid_ptr
);

/*===========================================================================
FUNCTION      DS707_EPZID_GET_CURRENT_EPZID

DESCRIPTION   This function gets a copy of current epzid from MC

DEPENDENCIES  curr_epzid pointer is initialized

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_get_current_epzid
(
  ds707_epzid_type *curr_epzid_ptr
);

/*===========================================================================
FUNCTION      DS707_EPZID_SET_ACTIVE_EPZID_TYPE

DESCRIPTION   This function sets the value of ds707_active_epzid_type

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_set_active_epzid_type
(
  byte active_epzid_type
);

/*===========================================================================
FUNCTION      DS707_EPZID_GET_ACTIVE_EPZID_TYPE

DESCRIPTION   This function gets value of ds707_active_epzid_type

DEPENDENCIES  None

RETURN VALUE  Byte

SIDE EFFECTS  None
===========================================================================*/
byte ds707_epzid_get_active_epzid_type
(
  void
);
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_EPZID_H */

