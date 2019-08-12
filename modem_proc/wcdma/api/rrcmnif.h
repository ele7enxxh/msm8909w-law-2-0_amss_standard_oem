#ifndef RRCMNIF_H
#define RRCMNIF_H
/*===========================================================================
              R R C M N I F  H E A D E R  F I L E

DESCRIPTION
  This file contains data types and declarations associated with RRC-MN 
  interface.
  
Copyright (c) 2001,2003,2007,2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/rrcmnif.h_v   1.0   26 Mar 2001 13:12:28   vtawker  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rrcmnif.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/13   ad         Providing value of rrciho_mvs_control_flag to NAS if as_id matches with 
                      the NAS provided as_id through an API
03/14/13   md         Made changes to provide active subscription ID to NAS APIs for DSDA
02/15/12    rl          Defeaturization changes
11/30/11   md         Added prototypes of funs rrc_vocoder_control_enabled and rrc_vocoder_acquired. 
07/29/11   ad         Made changes for CMI Phase 4
02/10/11   rl         Merged with VU_MODEM_WCDMA_RRC.01.90.51
02/03/11   ad         Added changes to refeaturize the API files
01/28/11   rl         Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad         Added changes for RRC SW decoupling
05/08/09   ss         Updated Copyright Information
03/23/01   rj/vt      Created file.
01/08/08   ps/gkg     Added declaration for rrcmn_is_rab_modified()

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/
#include "sys.h"

/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/


/*===========================================================================
**                  ENUMERATED TYPES
**=========================================================================*/

typedef enum
{
  RRCMN_CS_DATA_CALL,                       /* Indicates CS Data Call */
  RRCMN_CS_VOICE_CALL,                      /* Indicates CS Voice Call */ 
  RRCMN_INVALID_RAB_ID                      /* Invalid Radio Access Bearer */

}rrcmn_rab_status_T;


/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

typedef uint8 radio_access_bearer_T;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION rrcmn_verify_rab

DESCRIPTION
  This function validates the received RAB and identifies the call type 
  for which the RAB belongs to.

DEPENDENCIES


RETURN VALUE
  rrcmn_rab_status_T : If the RAB is valid, it returns either 
  RRCMN_CS_DATA_CALL or RRCMN_CS_VOICE_CALL. Otherwise it returns 
  RRCMN_INVALID_RAB_ID.


SIDE EFFECTS
  None
===========================================================================*/

rrcmn_rab_status_T rrcmn_verify_rab
(    
  radio_access_bearer_T  rab_id            /* RAB Id received from OTA 
                                              messsage */
);

/*===========================================================================

FUNCTION rrcmn_is_rab_modified

DESCRIPTION
  This function will indicate whether Rab type has changed or not
  

DEPENDENCIES

PARAMETERS
  none

RETURN VALUE
  boolean 
  TRUE(1)  - RAb type has changed 
  FALSE(0) - Rab type has not changed


SIDE EFFECTS
  None
===========================================================================*/

boolean rrcmn_is_rab_modified
(
  void
);

/*===========================================================================

FUNCTION rrcmn_verify_rab_subs

DESCRIPTION
  When DSDA is enabled this function validates the received RAB and identifies the call type 
  for which the RAB belongs to.

DEPENDENCIES


RETURN VALUE
  rrcmn_rab_status_T : If the RAB is valid, it returns either 
  RRCMN_CS_DATA_CALL or RRCMN_CS_VOICE_CALL. Otherwise it returns 
  RRCMN_INVALID_RAB_ID.


SIDE EFFECTS
  None
===========================================================================*/

rrcmn_rab_status_T rrcmn_verify_rab_subs 
(    
  radio_access_bearer_T  rab_id,            /* RAB Id received from OTA messsage */
  sys_modem_as_id_e_type as_id

);

/*===========================================================================

FUNCTION rrcmn_is_rab_modified_subs

DESCRIPTION
  This function will indicate whether Rab type has changed or not when DSDA is enabled
  

DEPENDENCIES

PARAMETERS
  none

RETURN VALUE
  boolean 
  TRUE(1)  - RAb type has changed 
  FALSE(0) - Rab type has not changed


SIDE EFFECTS
  None
===========================================================================*/

boolean rrcmn_is_rab_modified_subs
(
  sys_modem_as_id_e_type as_id
);
#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION   rrc_vocoder_control_enabled_sub()

DESCRIPTION

  Interface to let other procedures know that rrc is controlling the vocoder if W has the as id 
  same as the as_id passed by NAS

DEPENDENCIES

  None.

RETURN VALUE

  TRUE if rrciho is controlling the vocoder if as id matches,FALSE otherwise.

SIDE EFFECTS

  None.
 ===========================================================================*/
boolean rrc_vocoder_control_enabled_sub(sys_modem_as_id_e_type as_id);
#endif
/*===========================================================================
FUNCTION   rrc_vocoder_control_enabled()

DESCRIPTION

  Interface to let other procedures know that rrc is controlling the vocoder
  
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE if rrciho is controlling the vocoder,FALSE otherwise.

SIDE EFFECTS

  None.
 ===========================================================================*/
boolean rrc_vocoder_control_enabled(void);

/*===========================================================================
FUNCTION   rrc_vocoder_acquired()

DESCRIPTION

  This is a callback function for NAS when the mvs_acquire commmand issued
  by RRC was succesful
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.
 ===========================================================================*/
void rrc_vocoder_acquired(void);


#endif /* RRCMNIF_H */
