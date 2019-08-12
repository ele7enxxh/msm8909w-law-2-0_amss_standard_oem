#ifndef DS707_P_REV_H
#define DS707_P_REV_H
/*===========================================================================

                         D S 7 0 7 _ P _ R E V
GENERAL DESCRIPTION

  This file contains functions relating the system P_REV, which is determined
  by MC/CM.
  
EXTERNAL FUNCTIONS
  DS707_P_REV_GET_INDEX
    Returns an enum indicating the current p_rev.
    
  DS707_P_REV_CHANGE_HANDLER
    Called by MC when the system P_REV changes.
  
  DS707_P_REV_REGISTER  
    Registers te P_REV change handler with MC.
    
INITIALIZATION
  Need to register P_REV handler with MC.
    
 Copyright (c) 2002-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_p_rev.h_v   1.1   18 Nov 2002 21:29:02   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_p_rev.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/14   sd      Added new API to fetch p_rev value from MC
03/04/14   sd      Changes made to notify DSD to indicate 1x service 
                   availability only after ESPM is received.
07/23/04   atp     Added P_REV 10 & 11 support for Rel D.
10/14/03   sy      Added P_REV 9 support for Rel C.
06/26/03   sy      Added P_REV 8 support for Rel B.
11/17/02   ak      Updated header comments.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cm.h"

/*===========================================================================
                     TYPEDEFS & VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Base station prev's that are meaningful to data.
  Since Prev_6 and 7 have same service options make their enum types of 
  same value.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_P_REV_3 = 0,        /* Normal, lowspeed data; uses RLP 1           */
  DS707_P_REV_5 = 1,        /* Base Station can do MDR, uses RLP 2         */
  DS707_P_REV_6 = 2,        /* Base Station cad do 3G/1x; uses RLP 3       */
  DS707_P_REV_7 = 2,        /* Release A                                   */
  DS707_P_REV_8 = 2,        /* Release B                                   */
  DS707_P_REV_9 = 2,        /* Release C                                   */
  DS707_P_REV_10= 2,        /* Release D                                   */
  DS707_P_REV_11= 2,        /* Release D                                   */
  DS707_P_REV_MAX           /* Max values of this enum                     */
} ds_bs_prev_enum_type;

#define DS707_NUM_P_REVS    DS707_P_REV_MAX


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_P_REV_GET_INDEX

DESCRIPTION   Based on the p_rev_in_use, returns the index into the enum list
              of p_revs.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
word ds707_p_rev_get_index(void);

/*===========================================================================
FUNCTION      DS707_P_REV_CHANGE_HANDLER

DESCRIPTION   Enable/Disable the appropriate Packet Data service options and 
              set up the originating Packet Data service options, based on
              the new base station protocol revision.  This allows the phone
              to intelligently know whether or not to try IS95B or 3G data
              service options.
  
              This function is a callback registered with CM in dsi_cm_init.
              It is called whenever the prev_in_use changes.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_p_rev_change_handler
(
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type  *ss_info_ptr
);

/*===========================================================================
FUNCTION      DS707_P_REV_REGISTER

DESCRIPTION   Registers the P_REV with MC/CM.  Called once, at startup.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_p_rev_register
(
  cm_client_id_type        cm_id
);

/*===========================================================================
FUNCTION       DS707_REGISTER_SYNC_ESPM_IND

DESCRIPTION    Registers a callback to get indication for SYNC and ESPM 
               messages. Called once at startup.
 
DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_register_sync_espm_ind
(
  void
);

/*===========================================================================
FUNCTION       DS707_GET_SO_MASK_FROM_P_REV

DESCRIPTION    Gets SO mask for a given p_rev
 
DEPENDENCIES   None.

RETURN VALUE   so_mask - based on p_rev value

SIDE EFFECTS   None
===========================================================================*/
uint32 ds707_get_so_mask_from_p_rev 
(
  byte p_rev
);

/*===========================================================================
FUNCTION       DS707_GET_ESPM_IND_RECVD

DESCRIPTION    Get API for espm_ind_recvd.
 
DEPENDENCIES   None.

RETURN VALUE   Booelan

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_get_espm_ind_recvd
(
  void
);

/*===========================================================================
FUNCTION      DS707_ORIG_P_REV_GET_INDEX

DESCRIPTION   Based on the p_rev_in_use, returns the index into the enum list
              of p_revs during call origination.
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
word ds707_orig_p_rev_get_index
(
  void
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_P_REV_H      */

