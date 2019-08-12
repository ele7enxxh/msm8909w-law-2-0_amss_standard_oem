#ifndef DS707_DRS_H
#define DS707_DRS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 
                           D S 7 0 7 _ D R S

GENERAL DESCRIPTION
  This file contains functions that are used to see if the DRS bit should be
  set on originations out of dormancy.

EXTERNALIZED FUNCTIONS
  ds707_drs_get_drs_bit()
    Called on a pkt re-origination to see if Mobile IP requires the DRS bit
    to be set, indicating that the mobile has data to send.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

 Copyright (c) 2002 - 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_drs.h_v   1.3   26 Nov 2002 08:20:34   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_drs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
11/11/05   vrk/gr     Avoiding ppp resync on failed ALN on MIP
10/21/03   vr         Added DS707_DRS_IS_PPP_RESYNC_ON_HANDDOWN_SET
10/13/03   vr/kvd     Added flags to determine if we need to do a PPP resync
                      in different handoff scenarios and functions to
                      set/clear/check those flags.
11/25/02   usb        Renamed functions, added drs_init, completed drs 
                      algorithm handling.
11/18/02   jd         Added ds707_drs_mip_algorithm() to turn optimization
                      code on or off
07/29/02   ak         Initial revision.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707


/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DRS_INIT

DESCRIPTION   Initializes the Data Send Request (DSR) algorithm handler.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_init(void);

/*===========================================================================
FUNCTION      DS707_DRS_GET_MIP_DRS_BIT

DESCRIPTION   This function returns TRUE if the DRS bit should be set for a
              Mobile IP Call. Else returns FALSE if the DRS bit should 
              not be set.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_get_mip_drs_bit(void);

/*===========================================================================
FUNCTION      DS707_DRS_HANDLE_DRS1_RECON

DESCRIPTION   This function handles the reconection due to SID/NID/PZID 
              change or IDM change. If the drs bit was set during 
              reconnection then it initiates a PPP resync on UM interface.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_drs_handle_drs1_recon(void);

#ifdef FEATURE_HDR

/*===========================================================================
FUNCTION      DS707_DRS_GET_DRS_BIT_ON_IDM_CHANGE

DESCRIPTION   This function returns TRUE if currently a mip session is active
              and either of dsi_do_ppp_resync_on-1XHandoff or dsi_use_aln_algorithm
              are checked in NV. 
              In the former case it also sets ds707_ppp_resync_on_1Xhandoff flag to TRUE,

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/

boolean ds707_drs_get_drs_bit_on_idm_change(sys_sys_mode_e_type new_network);

/*===========================================================================
FUNCTION      DS707_DRS_GET_DRS_BIT_ON_ALN

DESCRIPTION   This function returns TRUE if currently a mip session is active
              called when we INITIATE_ALN_CDM form HDRMC
              This sets ds707_drsi_ppp_resync_flags.handoff_ALN to TRUE, so
              that ppp_resync is initiated upon call_connect.

DEPENDENCIES  None.

RETURN VALUE  See description.

SIDE EFFECTS  None.
===========================================================================*/

boolean ds707_drs_get_drs_bit_on_ALN (void);

#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION      DS707_DRS_RESET_PPP_RESYNC_FLAGS

DESCRIPTION   This function is used to reset the ppp_resync flag on CALLCONNECT
              after ppp_resync(), if needed done. Called from
              ds707_call_conn_hdlr()

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_drs_reset_ppp_resync_flags(void);

/*===========================================================================
FUNCTION      DS707_PPP_RESYNC_REQUIRED

DESCRIPTION   This function  returns TRUE if we need to do a ppp resync
              followed by call_connect, typically for an orig with DRS bit 1,
              because we want to initiate ppp_resync on HDRto1X handoff or
              we are revisiting an old PDSN. 

DEPENDENCIES  None.

RETURN VALUE  Boolean

SIDE EFFECTS  None.
===========================================================================*/

boolean ds707_drs_ppp_resync_reqd(void);

/*===========================================================================
FUNCTION      DS707_DRS_MOVED_TO_1X

DESCRIPTION   set ds707_drsi_ppp_resync_flags.handoff_HDR_to_1X if MIP call
              UP

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/

void ds707_drs_moved_to_1X(void);

/*===========================================================================
FUNCTION      DS707_DRS_IS_IN_MIP_CALL

DESCRIPTION   Wrapper function that returns TRUE if we are in a mobile IP
              call

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_is_in_mip_call(void);

/*===========================================================================
FUNCTION      DS707_DRS_IS_PPP_RESYNC_ON_HANDDOWN_SET

DESCRIPTION   Wrapper function that returns TRUE if the flag to do PPP resync
              due to handdown from HDR to 1X is set.

DEPENDENCIES  None.

RETURN VALUE  handoff_HDR_to_1X flag.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_drs_is_ppp_resync_on_handdown_set( void );

/*===========================================================================
FUNCTION      DS707_DRS_RESET_ALN_PPP_RESYNC_FLAG

DESCRIPTION   This function RESETs the ALN PPP resync flag to FALSE
              This is to handle the scenario where ALN fails and the
              packet data session is connected on 1x

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/

void ds707_drs_reset_aln_ppp_resync_flag (void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_DRS_H        */

