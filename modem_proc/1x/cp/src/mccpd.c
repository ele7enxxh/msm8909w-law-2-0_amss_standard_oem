/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          M A I N  C O N T R O L

        P O S I T I O N   D E T E R M I N A T I O N   S E R V I C E S

GENERAL DESCRIPTION

  This file contains the support for Position Determination services.

EXTERNALIZED FUNCTIONS
  mcc_pd_init - Register the position determination service options with
                service negotiation manager (SNM).

INITIALIZATION AND SEQUENCING REQUIREMENTS
  mcc_pd_init needs to be called during the powerup.

  Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccpd.c_v   1.14   02 Oct 2002 13:40:22   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccpd.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/14   bb      Changes to pass stored information from mccpd_get_bs_info
                   if cur_bs_ptr is null                   
04/30/13   nee     Changed the return type of mcc_pd_get_bs_info() from void
                   to boolean.
01/28/13   gga     Change from MSG_FATAL to MSG_MED in mcc_pd_get_bs_info()
09/17/12   jj      Added defensive Null pointer check while retrieving
                   latest base station information by GPS.
04/04/12   trc     Convert to m1x_time module
02/21/12   ppr     Feature Cleanup
01/19/12   ppr     KDDI req FR1665 - Reject incoming SO35/36 call
                   if it is not supported
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
01/27/11   azh     Lint and KW fixes.
11/15/10   jtm     Initial SU API split for CP.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
03/14/10   ag      Added RC(11,8) support to all SOs supporting Mux option 1
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
                   have a single value.
05/14/09   ag      CMI phase 2 changes
04/17/09   jtm     Added includes to fix compiler warnings.
03/03/08   jj      Featurized Q6
12/10/08   jj      QDSP6 migration porting effort, as a part of which replaced
                   INTLOCK and INTFREE with REX_ENABLE_PREMPTION and
                   REX_DISABLE_PREMPTION.
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
12/14/07   an      Added Passport Mode
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
12/12/06   pg      Fixed Lint error.
10/15/06   fc      Code review comments.
07/17/06   fc      Optimize the implementation of pseudo bs.
07/10/06   fc      Added support for pseudo bs.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
12/10/04   sb       Fixed lint error.
01/10/05   va      Merged lint fixes from REL_B_C archive
09/27/04   va      Merged following from REL B/C archive
07/08/04   bkm      Correct setting of so_bitmap for SO23 (RS1 PD).
07/01/04   va       Corrected lint errors
06/18/04   ank      Mainlined FEATURE_GPSONE_DBM.
06/30/04   bkm     Lint cleanup.
05/19/04   sb      Made following merge official via integrate.
05/14/04   sb      Merged the following from msmshared:
                   03/05/04   bkm     Lint cleanup.
10/14/03   is      Added task lock on critical data in FEATURE_GPSONE_BS_INFO
10/10/03   ht      Included ts.h for ts_get().
08/01/03   yll     Added Dynamic Feature support.
11/25/02   sk      Merged force dormancy changes from MSM archive,  new logic
                   for deciding the anchors.
10/02/02   az      Mainlined FEATURE_IS2000
03/09/02   lh      Merged from common archive.
  02/07/02   hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                     Base Station ID.
02/22/02   fc      Added support for Release A common channels.
02/10/02   yll     Merged from common archive CP VU 116
                   01/31/02   hxw
                   Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                   Base Station ID.
10/01/01   kk      Merge from common archive CP VU 96.
                   08/14/01   jrp     Changed references to DEC_RATE...
                   to CAI_RATE...
08/16/01   kk      Merge from common archive CP VU 89.
                   08/10/01   ph    Include CHIND_LIST in neg pars for SO0x23
                   and 0x24.
08/10/01   kk      SO initialization modified.
06/26/01   sb      Merged following from common archive:
                   05/31/01   abh     Send the SID/NID from paging channel.
04/19/01   kk      Fixed typo
03/14/01   kk      IS2000 related initialization.
02/27/00   kk      SO initialization added for group/bitmap info.
11/30/00   yll     Changed the min_p_rev for SO 35/36.
10/06/00   yll     Fixed the compiler warning.
06/28/00   yll     Initial revision.
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"
#include "m1x_time.h"

#include "msg.h"
#include "m1x_diag.h"

#include "db_v.h"
#include "cai_v.h"
#include "caii_v.h"
#include "caix.h"
#include "caix_i.h"
#include "mc_i.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mccpd.h"
#include "mccdma_v.h"
#include "mccdma_i.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_GPSONE
/*--------------------------------------------------------------------------
            Position Determination Service Option Initializations.
--------------------------------------------------------------------------*/

/* These structures are declared const because these parameters will never
   change at run time */

/* Initialize the service option configuration for Position Determination
   Rate Set 1 */
const snm_config_type default_cfg_so23 =
{
  {
    1,                           /* Mux Option 1 */
    CAI_RATE_9600,               /* Forward Rate set = 1             */
    CAI_RS1_ALL,                 /* All transmission rates used for
                                    Fundamental channel              */
    CAI_NUL_TRAF                 /* Null Traffic allowed   */
  },
  {
    1,                           /* Mux Option 1  */
    CAI_RATE_9600,               /* Reverse Rate set = 1              */
    CAI_RS1_ALL,                 /* All transmission rates used for
                                    Fundamental channel               */
    CAI_NUL_TRAF                 /* Null Traffic allowed   */
  },
};


/* Initialize the service option configuration for Position Determination
   Rate Set 2 */
const snm_config_type default_cfg_so24 =
{
  {
    2,                           /* Mux Option 2 */
    CAI_RATE_14400,              /* Forward Rate set = 2             */
    CAI_RS1_ALL,                 /* All transmission rates used for
                                    Fundamental channel              */
    CAI_NUL_TRAF                 /* Null Traffic allowed   */
  },
  {
    2,                           /* Mux Option 2  */
    CAI_RATE_14400,              /* Reverse Rate set = 2              */
    CAI_RS1_ALL,                 /* All transmission rates used for
                                    Fundamental channel               */
    CAI_NUL_TRAF                 /* Null Traffic allowed   */
  },
};



/* Initialize the service negotiation parameters for Position Determination
   Rate Set 1 */
const snm_neg_par_type neg_par_so23 =
{
  1,                             /* Normal min. PREV for this SO */
                                 /* Even though IS-801 is an adjunct
                                  * standard to IS-95B (P_REV 4), there
                                  * is no reason why we cannot do
                                  * the SO 35/36 calls on JSTD-008 system,
                                  * since all the activities are just
                                  * data burst messages after the call is
                                  * setup.
                                  */
  {
    CAI_SO_RS2_PD, CAI_SO_NULL, 0, 0, 0, 0, 0
                                 /* Alternative Service Options. There is
                                 only one for PD Rate Set 1, which is
                                 PD Rate Set 2                     */
    ,0,0
  },
  {
    CAI_SO_NULL, 0               /* Compatible Service Options. There are
                                 None for PD Rate Set 1            */
  },
  {
    CAI_MUX1, CAI_MUX1 /* Maximum fwd and rev mux options */
  }
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list     */
    #else
    3,                                  /* number of rc's in the list     */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1xADV, RS = 1                   */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list     */
    #else
    2,                                  /* number of rc's in the list     */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_1,                         /* IS-95, RS 1                     */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1xADV, RS = 1                   */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0

    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
    /* Channels allowed for this SO */
#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_LOCATION
  , CAI_SO_BITMAP_SO_RS1_PD
#endif /* FEATURE_IS2000_REL_A */
};



/* Initialize the service negotiation parameters for MDT Data Pattern
Test Rate Set 2 */
const snm_neg_par_type neg_par_so24 =
{
  1,                             /* Normal min. PREV for this SO */
                                 /* Even though IS-801 is an adjunct
                                  * standard to IS-95B (P_REV 4), there
                                  * is no reason why we cannot do
                                  * the SO 35/36 calls on JSTD-008 system,
                                  * since all the activities are just
                                  * data burst messages after the call is
                                  * setup.
                                  */
  {
    CAI_SO_RS1_PD, CAI_SO_NULL, 0, 0, 0, 0, 0
                                 /* Alternative Service Options. There is
                                 only one for PD Rate Set 2, which is
                                 PD Rate Set 1                     */
    ,0,0
  },
  {
    CAI_SO_NULL, 0               /* Compatible Service Options. There are
                                 None for PD Rate Set 2            */
  },
  {
    CAI_MUX2, CAI_MUX2 /* Maximum fwd and rev mux options */
  }
 ,{
    2,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_5,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    2,                                  /* number of rc's in the list      */
    {
      CAI_RC_4,                         /* IS-2000, RS 2                   */
      CAI_RC_2,                         /* IS-95, RS 2                     */
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  ,{ 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
    /* Channels allowed for this SO */
#ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_LOCATION
  , CAI_SO_BITMAP_SO_RS2_PD
#endif /* FEATURE_IS2000_REL_A */

};
#endif /* FEATURE_GPSONE */

/*===========================================================================

                                 FUNCTIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION MCC_PD_GET_BS_INFO

DESCRIPTION
  This is an external function called by other modules (mainly by Position
  Determination Session Manager) to get the latest base station information
  (BAND_CLASS, CDMA_CHAN, BASE_ID, SID, NID) where the mobile has registered.

DEPENDENCIES
  This function return FALSE if the mobile station does not have service, or
  it is not in CDMA.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_pd_get_bs_info
(
  mcc_pd_bs_info_type  *info_ptr
    /* pointer to hold the base station information */
)
{
  #ifdef FEATURE_GPSONE

  REX_ISR_LOCK(&mc_crit_sect);

  info_ptr->stale_info = mcc_pd_ft_dm_info.dorm_ind;

  /* get basid, sid and nid from PD data stucture.
  */
  info_ptr->base_id    = mcc_pd_ft_dm_info.base_id;
  info_ptr->sid        = mcc_pd_ft_dm_info.sid;
  info_ptr->nid        = mcc_pd_ft_dm_info.nid;

  qw_equ(info_ptr->ts, mcc_pd_ft_dm_info.ts);
  info_ptr->still_active = mcc_pd_ft_dm_info.still_active;
  info_ptr->pilot_pn = mcc_pd_ft_dm_info.pilot_pn;
  info_ptr->base_lat = mcc_pd_ft_dm_info.base_lat;
  info_ptr->base_long = mcc_pd_ft_dm_info.base_long;

  /* Defensive check to see if curr_bs_ptr is NULL. If it is error out and
   * return.
   */
  if(cur_bs_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "ERROR:: In get PD, curr_bs_ptr is NULL. Returning.."); 
    info_ptr->band_class = mcc_pd_ft_dm_info.band_class;
    info_ptr->cdma_ch    = mcc_pd_ft_dm_info.cdma_ch;
    info_ptr->stale_info = FALSE;
    REX_ISR_UNLOCK(&mc_crit_sect);
    return TRUE;
  }
  else
  {
  /* Get the Band class, CDMA channel from Cur bs */
  if ( cur_bs_ptr->pilot_pn == PSEUDO_PILOT_PN )
  {
    /* Pseudo BS info is in use, refer to saved cur_bs_ptr for BS info that
       MS was idling on. */
    info_ptr->band_class = mcc_saved_current_bs_ptr->band_class;
    info_ptr->cdma_ch = mcc_saved_current_bs_ptr->cdma_ch;
  }
  else
  {
    info_ptr->band_class = cur_bs_ptr->band_class;
    info_ptr->cdma_ch    = cur_bs_ptr->cdma_ch;
  }
  }

  REX_ISR_UNLOCK(&mc_crit_sect);

  #else
  MODEM_1X_UNUSED(info_ptr);
  #endif /* FEATURE_GPSONE */
  return TRUE;

} /* mccpd_get_bs_info */


/* <EJECT> */
/*===========================================================================

FUNCTION MCC_PD_FT_DM_NEEDED

DESCRIPTION
  This is an external function called by other modules (mainly by Position
  Determination Session Manager) to decide whether a fast dormancy is needed.

DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_pd_ft_dm_needed(void)
{
  #if defined(FEATURE_GPSONE) && defined(FEATURE_GPSONE_UPDATE_BSID)
  /* Return Fast Dormancy indicator.
  */
  return mcc_pd_ft_dm_info.dorm_ind;
  #else
  return FALSE;
  #endif /* FEATURE_GPSONE && FEATURE_GPSONE_UPDATE_BSID */

}

#ifdef FEATURE_GPSONE
/*===========================================================================

FUNCTION MCC_PD_INIT

DESCRIPTION
  This function initializes the service option descriptors for
  Position Determination Service Options. These descriptors are
  used throughout an Position Determination call for service
  configuration and service negotiation.

DEPENDENCIES
  FEATURE_GPSONE_DBM must be turned on.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_pd_init( void )
{
  /* Initialize the Position Determination SOs by calling the registration
  functions */

  if( (snm_add_so( CAI_SO_RS1_PD,   //Service Option Number
                   (snm_neg_par_type*)&neg_par_so23,
                                    //Negotiation parameters
                   &default_cfg_so23    //Default configuration
                 ) ) == SNM_FAILURE )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Registering SO %d Failed",
      CAI_SO_RS1_PD);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "call snm_add_so[%d]",
      CAI_SO_RS1_PD);
  }

  if( (snm_add_so( CAI_SO_RS2_PD,   //Service Option Number
                   (snm_neg_par_type*)&neg_par_so24,
                                    //Negotiation parameters
                   &default_cfg_so24    //Default configuration
                 ) ) == SNM_FAILURE )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Registering SO %d Failed",
      CAI_SO_RS2_PD);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "call snm_add_so[%d]",
      CAI_SO_RS2_PD);
  }

  /*
   * Disable service options SO35 and SO36
   * if ( KDDI specific requirements supported &&
   *      SO35/SO36 service options not supported )
   */
  if( mcc_is_jcdma_supported() && mcc_is_so35_so36_disabled() )
  {
    /* Disable IS-801 Position Determination Service Option 35 for rate set 1  */
    (void)snm_disable_so( CAI_SO_RS1_PD );

    /* Disable IS-801 Position Determination Service Option 36 for rate set 2  */
    (void)snm_disable_so( CAI_SO_RS2_PD );

    M1X_MSG( DCP, LEGACY_MED,
      "Disabled SO35 and SO36 service options");
  }

  /* Tell the service negotiation module what SO to respond with, when
  paged with these SOs. We respond with the Same SO, since these are test
  service options, and we don't need to negotiate the SO.

  Note these functions below are needed if the Service Option to respond
  with is not the same as the paged Service Option. In this case, they
  are not needed

  snm_set_page_response( CAI_SO_RS1_PD,
                         CAI_SO_RS1_PD );

  snm_set_page_response( CAI_SO_RS2_PD,
                         CAI_SO_RS2_PD );
  */

  /* Call the function to register the connection initilizer
  for both Position Determination SOs

  Note these functions below are needed when the PD needs to initialize
  the connection. In this case, they are not needed
  */

//  if( (snm_reg_connection_initializer( CAI_SO_RS1_PD, /* Service Option */
//                                  mcc_pd_initializer
//                                                /* Function to be
//                                                   called by SN Manager */
//                                     ) ) == SNM_FAILURE )
//  {
//    MSG_HIGH( "Registering SO %x Failed", CAI_SO_RS1_PD, 0, 0);
//  }
//
//
//  if( (snm_reg_connection_initializer( CAI_SO_RS2_PD, /* Service Option */
//                                  mcc_pd_initializer
//                                                /* Function to be
//                                                   called by SN Manager */
//                                     ) ) == SNM_FAILURE )
//  {
//    MSG_HIGH( "Registering SO %x Failed", CAI_SO_RS2_PD, 0, 0);
//  }

}

/*===========================================================================

FUNCTION MCC_PD_SET_BS_INFO

DESCRIPTION
  This function initializes PD BS info data structure.
  use.
DEPENDENCIES

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
void mcc_pd_set_bs_info
(
  word pilot_pn,
  word sid,
  word nid,
  word base_id,
  uint32 base_lat,
  uint32 base_long
)
{
  /* Init pd relevant info from latest SPM.
  */

  mcc_pd_ft_dm_info.base_id        = base_id;
  mcc_pd_ft_dm_info.nid            = nid;
  mcc_pd_ft_dm_info.sid            = sid;
  mcc_pd_ft_dm_info.pilot_pn       = pilot_pn;

#ifdef FEATURE_GPSONE_UPDATE_BSID
  /* Always reset the dormancy indicator when SPM is handled.
  */
  mcc_pd_ft_dm_info.dorm_ind       = FALSE;
#endif /* UPDATE_BSID */

  (void) m1x_time_get( mcc_pd_ft_dm_info.ts );

  /*still_active parameter indicates that the serving BS is still
  **in the active set. The default value is 1 (TRUE) and should
  **be modified along with dorm_ind bit
  */
  mcc_pd_ft_dm_info.still_active   = TRUE;
  mcc_pd_ft_dm_info.base_lat = base_lat;
  mcc_pd_ft_dm_info.base_long = base_long;
}

#endif /* FEATURE_GPSONE */
