/*===========================================================================

         S E R V I C E   O P T I O N S   D E F I N I T I O N S

GENERAL DESCRIPTION
  This module contains definitions of service options other than data.

  Copyright (c) 2000 - 2014 Qualcomm Technologies, Inc.
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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccso.c_v   1.13   01 Oct 2002 16:49:36   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccso.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/30/15   sjo     Added NULL check to avoid the KW errors
09/09/15   agh     Use COP rate macros for sending COP rate to Voice Adapter
                   instead of directly sending absolute COP values
09/09/15   agh     Changes to support Voice Adapter based Audio architecture
11/12/14   eye     Added API to get and set MVS variable & reset in call end.
12/02/14   agh     Support for new customer specific voice service option
08/21/14   gga     Added handling to send the stanndby status to MUX for
                   reconfiguring the VFR.
05/06/14   gga     1X_CP F3 redistribution
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
01/30/13   gga     Changes to stop advertisings SO73 if 1xA and SO73 NV
                   is disabled
12/28/12   gga     Klocwork Reviews
11/19/12   ssh     Added SMS over CSFB support.
02/21/12   ppr     Feature Cleanup
02/15/12   srk     Feature Cleanup: Removed FEATURE_NO_13K_VOICE_SUPPORT
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/04/12   ssh     Added critical section for mc_cmd_q.
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
06/17/11   jj      For SO3, wrapped non-voice SO's under FEATURE_PASSPORT_MODE.
03/15/11   ag      Reinstated usage of NV item to enable or disable SO73.
03/07/11   ag      Fix to release MVS before MUX is commanded to release SO.
02/18/11   ag      Removed usage of NV item to enable or disable SO73.
01/24/11   ag      Changes to ignore SO73 NV item for SO73 capability when
                   FEATURE_IS2000_1X_ADV is enabled.
01/05/11   ag      Added SO75 to ALT SO lists of legacy loopback SOs and
                   legacy loopback SOs to ALT SO list of SO75. This will
                   assist in SO change during traffic state.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/28/10   ag      Decoupling of SO73 from 1xAdv.
11/19/10   ss      Moving SO = 1 related code outside FEATURE_MODEM_1X_VOICE_SUPPORT.
11/15/10   jtm     Initial SU API split for CP.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
06/15/10   ag      Added support for enhanced loopback service option.
05/21/10   jtm     Featurized MVS support check for SO73 under
                   FEATURE_VOC_4GV_NW.
05/19/10   ss      Added support to remove the redundant mvs_acquire() calls.
04/20/10   ssh     Enabled voice SOs on data card for otasp procedure.
04/19/10   ag      Changed the order of service options in alt_so lists.
03/14/10   ag      Added RC(11,8) support to all SOs supporting Mux option 1
03/02/10   ag      Added support for MVS mode for EVRC-NW SO.
02/26/10   ag      Added Mux2 support to SO74 and set preferred RCs to (4,3)
02/10/10   ag      Removed Mux option 2 support from SO74.
01/13/10   ag      Featurized RCs 11 and 8 in negotiation parameters
12/29/09   ag      Changed default RCs for SO73 and SO74 to 4 and 3
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
10/31/09   ag      Added check for MVS capability for SO73
09/29/09   ag      1X Advanced Signaling Support - Phase 2
07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
                   have a single value.
05/14/09   ag      CMI phase II update
03/13/09   adw     Lint fixes. Added mc_v.h include.
07/30/08   bb      Code changes for Voice Call Continuity (VCC) feature
05/15/08   bb      Changes for "MVS API Migration" feature
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
12/14/07   an      Added Passport Mode
04/24/07   an      Changed the min P_REV for 4GV from 1 to 6
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
12/12/06   pg      Fixed Lint error.
10/12/06   pg      Added F-TDSO(SO32) and Markov Loopback(SO55) to alt_so of
                   SO = 3 under FEATURE_JCDMA_2.
11/17/05   an      Changed 4GV_WB Bitmap Pos/SO Group
11/03/05   an      Changed 4GV SO value, moved EVRC above 4GV_WB in 4GV_NB alt
                   so list. Changed certain 4GV featurizations.
08/20/05   va      Allowed SO negotiation b/w test SOs
06/13/05   fc      Merged the following from MSMSHARED:
06/09/05   an       Made EVRC the first alt so for 4GV
05/23/05   fc      Merged the following from MSMSHARED:
05/24/05   an       Added 4GV_NB as an alt so to 4GV_WB and vice versa
04/27/05   an       Added support for 4GV service option
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
12/10/04   sb      Fixed lint warning.
08/09/04   fc      Service negotiation changes for Release C and D.
08/06/04   va      Removed voc_config from here as this is anyway done
                   in MUX
10/30/03   bkm     Added SO2 as alternate SO for EVRC to support legacy
                   JCDMA test equipment.
09/29/03   bkm     Added SO2 as alternate SO for IS-96A to support legacy
                   JCDMA test equipment.
08/13/03   sb      Added OTAPA and SMS on DCCH.
08/01/03   yll     Added support for Dynamic Feature.
07/30/03   az      Added support for FEATURE_SO_8026
10/01/02   az      Mainlined FEATURE_IS2000.
08/13/02   az      Changed the minimum PREV allowed to 1 for SMV. This is due to
                   a request from Lucent interop that SMV be able to originate
                   IS-95 BS
08/13/02   HQ      Added FEATURE_CDSMS.
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
04/17/02   lh      Added voc_acquire() before voc_config().
03/09/02   lh      Merged from common archive.
  02/20/02   az      Featurized 13K voice service option
10/01/01   kk      Merge from common archive.
                   08/16/01   sj      Registered the voice service options with
                   connection_intialization() call back.
                   Defined mccso_so_voice_con_initializer() to configure
                   vocoder.
08/10/01   kk      Increase mux option size.
08/09/01   kk      Merge from mainline.
                   08/02/01   jrp     Added SO54, SO55.  Expanded RC list from 3 elements to 5.
                   Removed dependence on dec.h by using cai_rate_set_type.
05/17/01   fc      Merged the following from common archive:
                   05/03/01   lcc     Correct SMS and OTAPA to use NULL traffic.
                   04/10/01   lcc     Changes to accomodate P2 mode.
11/20/00   lcc     Adding disabling of SMS service options if the feature is not
                   enabled.
11/02/00   lcc     Initial release.  Contains definitions for all non-data
                   service options.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"

#include "cai_v.h"
#include "snm_v.h"
#include "snm_i.h"

#include "err.h"
#include "msg.h"
#include "m1x_diag.h"

#include "mc_v.h"

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mvs.h"
#include "rxccommon_i.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/*===========================================================================

                 NEGOTIATION PARAMETERS FOR SERVICE OPTIONS

===========================================================================*/

/*===========================================================================

                 TEST (MARKOV AND LOOPBACK) SERVICE OPTIONS

===========================================================================*/

// CAI_SO_MARKOV_SO54
const snm_neg_par_type mccso_so54_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV,
    CAI_SO_MARKOV_SO74,
    CAI_SO_RS1_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_NULL,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD} /* WILDCARDED Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    6,                                  /* number of rc's in the list      */
    #else
    5,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_5,
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1
      #ifdef FEATURE_IS2000_1X_ADV
      ,CAI_RC_11                        /* 1x Adv, RS 1                    */
      #endif
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    5,                                  /* number of rc's in the list      */
    #else
    4,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1,
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

#ifdef FEATURE_IS2000_1X_ADV
// CAI_SO_MARKOV_SO74
const snm_neg_par_type mccso_so74_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV_SO54,
    CAI_SO_MARKOV,
    CAI_SO_RS1_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_NULL,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD} /* WILDCARDED Maximum fwd and rev mux options */
 ,{
    6,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_4,
      CAI_RC_5,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1,
      CAI_RC_11                         /* 1x Adv, RS 1                    */
    }
  },
  {                                     /* reverse radio channel           */
    5,                                  /* number of rc's in the list      */
    {
      CAI_RC_3,
      CAI_RC_4,
      CAI_RC_2,
      CAI_RC_1,
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};
#endif /* FEATURE_IS2000_1X_ADV */


// CAI_SO_MARKOV
const snm_neg_par_type mccso_so8002h_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_RS1_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_MARKOV_SO54,
    CAI_SO_MARKOV_SO74,
    CAI_SO_NULL,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_1,                         /* IS-95, RS 1                     */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS1_MARKOV
const snm_neg_par_type mccso_so801eh_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV,
    CAI_SO_RS2_MARKOV,
    CAI_SO_MARKOV_13K,
    CAI_SO_MARKOV_SO54,
    CAI_SO_MARKOV_SO74,
    CAI_SO_NULL,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_1,                         /* IS-95, RS 1                     */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_MARKOV_13K
const snm_neg_par_type mccso_so801ch_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_RS2_MARKOV,
    CAI_SO_RS1_MARKOV,
    CAI_SO_MARKOV,
    CAI_SO_MARKOV_SO54,
    CAI_SO_MARKOV_SO74,
    CAI_SO_NULL,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS2_MARKOV
const snm_neg_par_type mccso_so801fh_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_MARKOV_13K,
    CAI_SO_RS1_MARKOV,
    CAI_SO_MARKOV,
    CAI_SO_MARKOV_SO54,
    CAI_SO_MARKOV_SO74,
    CAI_SO_NULL,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_LOOPBACK_SO55
const snm_neg_par_type mccso_so55_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK_13K,
    CAI_SO_LOOPBACK,
    CAI_SO_LOOPBACK_SO75,
    CAI_SO_NULL,
    0,0,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD} /* WILDCARDED Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    6,                                  /* number of rc's in the list      */
    #else
    5,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_5,
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1
      #ifdef FEATURE_IS2000_1X_ADV
      ,CAI_RC_11                        /* 1x Adv, RS 1                    */
      #endif
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    5,                                  /* number of rc's in the list      */
    #else
    4,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_2,
      CAI_RC_1,
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};



// CAI_SO_LOOPBACK
const snm_neg_par_type mccso_so2_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK_13K,
    CAI_SO_LOOPBACK_SO55,
    CAI_SO_LOOPBACK_SO75,
    CAI_SO_NULL,
    0,0,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, RS 1                   */
      CAI_RC_4,                         /* IS-95, RS 1                     */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1xADV, RS = 1                   */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,
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
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_LOOPBACK_13K
const snm_neg_par_type mccso_so9_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK,
    CAI_SO_LOOPBACK_SO55,
    CAI_SO_LOOPBACK_SO75,
    CAI_SO_NULL,
    0,0,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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

  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }

  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

#ifdef FEATURE_IS2000_1X_ADV
// CAI_SO_LOOPBACK_SO75
const snm_neg_par_type mccso_so75_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_LOOPBACK,
    CAI_SO_LOOPBACK_SO55,
    CAI_SO_LOOPBACK_13K,
    CAI_SO_NULL,
    0,0,
    0,0,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    1,                                  /* number of rc's in the list      */
    {                                   /* forward radio channel           */
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  },
  {                                     /* reverse radio channel           */
    1,                                  /* number of rc's in the list      */
    {
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC,
      CAI_INIT_RC
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};
#endif /* FEATURE_IS2000_1X_ADV */


/*===========================================================================

                         VOICE SERVICE OPTIONS

===========================================================================*/

/* The order of service options in the ALT_SO lists is decided by the following
 * constrainsts and goals:
 * 1. Generally carriers set MAX_NUM_ALT_SO to 1 or 2.
 * 2. 1xA MS should be connected to the 1x legacy networks with the servcice
 *    option SO3 or SO68.
 * 3. 1xADV MS should be connected to 1xADV networks with SO73.
 * 4. BS can use the support of RC8/11 in RC bit map to imply the support of
 *    SO73 by the MS.
 * 5. SO3 is the most widely deployed SO, so SO3 is at top of the ALT_SO
 *    lists for all service options.
 * 6. SO70 is towards the very end of ALT_SO list because no network has so far
 *    deployed SO70 and the ones that may want to have the similar
 *    functionality will now use SO73.
 * 7. For the purpose of lowering call drop because of handoff (from 1xADV
 *    capable BS to legacy BS), ALT_SO lists will have SO68 ahead of SO73.
 * 8. The position of SO 73 in the ALT_SO list is decided in such a way that,
 *    a network with MAX_NUM_ALT_SO = 2, will for sure see SO 73 in the
 *    suggested alternative SOs, for all primary SOs.
 * 9. Considering 13K can be enabled in the mobile, to have SO 73 show-up
 *    in ALT_SO list for 13k SOs while the MAX_NUM_ALT_SO = 2, SO73 is above
 *    SO 68 for 13K SOs.
 */

// CAI_SO_VOICE_13K
const snm_neg_par_type  mccso_so8000h_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  // Since there's no bitmap position assigned for 13K, it will
  // never be reported in the bitmap, do not set the group.
  , CAI_SO_GROUP_UNASSIGNED
  , CAI_SO_BITMAP_POSITION_UNASSIGNED
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_IS96A
const snm_neg_par_type  mccso_so1_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_4GV_WB,
    #ifdef FEATURE_JCDMA_TEST_SUPPORT
    CAI_SO_LOOPBACK                    /* Allow SO2 to support test equip. */
    #else
    CAI_SO_NULL
    #endif
    ,0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_IS96A
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_13K_IS733
const snm_neg_par_type  mccso_so17_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  , { 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  , { 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_13K_IS733
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_EVRC
snm_neg_par_type  mccso_so3_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_IS96A,
    CAI_SO_FULL_TDSO,
    CAI_SO_LOOPBACK_SO55,
  #if !defined(FEATURE_1X_CUST_VOICE_SO) && defined(FEATURE_JCDMA_TEST_SUPPORT)
    CAI_SO_LOOPBACK                    /* Allow SO2 to support test equip. */
  #else
    CAI_SO_NULL
  #endif
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_EVRC
  #endif /* FEATURE_IS2000_REL_A */
};

/* Negotation parameters for customer specific Service option */
const snm_neg_par_type  mccso_cust_so_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_NULL,
    0, 0, 0, 0, 0, 0, 0, 0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_EVRC
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_SMV
const snm_neg_par_type  mccso_so56_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_SMV
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_4GV_NB
const snm_neg_par_type  mccso_so4gv_nb_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate Service Options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL,
    0,0
  },
  {
    CAI_SO_NULL,                        /* Compatible service options      */
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_4GV_NB
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_VOICE_4GV_WB
const snm_neg_par_type  mccso_so4gv_wb_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_EVRC_NW,
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL,
    0,0
  },
  {
    CAI_SO_NULL,                        /* Compatible service options      */
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                          /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }

  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_VOICE
  , CAI_SO_BITMAP_SO_VOICE_4GV_WB
  #endif /* FEATURE_IS2000_REL_A */
};

/* CAI_SO_VOICE_EVRC_NW */
const snm_neg_par_type  mccso_so73_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6       */
  {                                     /* Alternate service options       */
    CAI_SO_VOICE_EVRC,
    CAI_SO_VOICE_4GV_NB,
    CAI_SO_VOICE_13K,
    CAI_SO_VOICE_13K_IS733,
    CAI_SO_VOICE_SMV,
    CAI_SO_VOICE_4GV_WB,
    CAI_SO_VOICE_IS96A,
    CAI_SO_NULL,
    0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
#ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
#else
    3,                                  /* number of rc's in the list      */
#endif
    {                                   /* forward radio channel           */
      CAI_RC_4,
      CAI_RC_3,
      CAI_RC_1,
#ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
#endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
#ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
#else
    2,                                  /* number of rc's in the list      */
#endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,
#ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
#endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  , { SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_ENHANCED_VOICE
  , CAI_SO_BITMAP_SO_ENHANCED_VOICE_EVRC_NW
  #endif /* FEATURE_IS2000_REL_A */
};


/*===========================================================================

                         DATA BURST SERVICE OPTIONS

===========================================================================*/
// CAI_SO_RS2_SMS
const snm_neg_par_type  mccso_so14_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_SMS,
    CAI_SO_NULL,
    0,0,0,0,0
    ,0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_SMS
  , CAI_SO_BITMAP_SO_RS2_SMS
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_SMS
const snm_neg_par_type  mccso_so6_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_RS2_SMS,
    CAI_SO_NULL,
    0,0,0,0,0
    ,0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_SMS
  , CAI_SO_BITMAP_SO_SMS
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS2_OTAPA
const snm_neg_par_type  mccso_so19_neg_par =
{
  3,                                    /* Minimum PREV allowed.  This allows 1 for BC 1 too */
  {                                     /* Alternate service options       */
    CAI_SO_RS1_OTAPA,
    CAI_SO_NULL,
    0,0,0,0,0
    ,0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX2, CAI_MUX2}                  /* Maximum fwd and rev mux options */
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
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_OTAPA
  , CAI_SO_BITMAP_SO_RS2_OTAPA
  #endif /* FEATURE_IS2000_REL_A */
};

// CAI_SO_RS1_OTAPA
const snm_neg_par_type  mccso_so18_neg_par =
{
  1,                                    /* Minimum PREV allowed = 1        */
  {                                     /* Alternate service options       */
    CAI_SO_RS2_OTAPA,
    CAI_SO_NULL,
    0,0,0,0,0
    ,0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_DCCH, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_OTAPA
  , CAI_SO_BITMAP_SO_RS1_OTAPA
  #endif /* FEATURE_IS2000_REL_A */
};

#ifdef FEATURE_SO_8026
// CAI_SO_PROPRIETARY_8026
const snm_neg_par_type  mccso_so8026_neg_par =
{
  6,                                    /* Minimum PREV allowed = 6        */
  {                                     /* Alternate service options       */
    CAI_SO_NULL,
    0,0,0,0,0,0
    ,0,0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL,
    0
  },
  {CAI_MUX1, CAI_MUX1}                  /* Maximum fwd and rev mux options */
 ,{
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_4,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  },
  {                                     /* reverse radio channel           */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* IS-2000, 9600 bps               */
      CAI_RC_1,                         /* IS-95, RS = 1                   */
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0
    }
  }
  // Following are forward and reverse mux options for SCH, not used for this
  // option.
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  ,{ 0, { 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
  /* Channels allowed for SO */
  ,{ SNM_FCH, SNM_RESERVED, SNM_RESERVED }
  #ifdef FEATURE_IS2000_REL_A
  , CAI_SO_GROUP_PROPRIETARY_8026
  , CAI_SO_BITMAP_SO_PROPRIETARY_8026
  #endif /* FEATURE_IS2000_REL_A */
};
#endif /* FEATURE_SO_8026 */




/*===========================================================================

                 DEFAULT CONFIGURATIONS SERVICE OPTIONS

===========================================================================*/
// Default configuration shared by some RS 1 service options
const snm_config_type  mccso_rs1_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

// Default configuration shared by some RS 2 service options
const snm_config_type  mccso_rs2_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_PRI_TRAF                   /* Primary traffic only                 */
  }
};

// Default configuration shared by RS 1 signaling service options
const snm_config_type  mccso_rs1_sig_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX1,                      /* Default forward multiplex option = 1 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  },
  {
    /* Reverse link defaults */
    CAI_MUX1,                      /* Default reverse multiplex option = 1 */
    CAI_RATE_9600,                 /* Reverse rate set = 1                 */
    CAI_RS1_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  }
};

// Default configuration shared by RS 2 signaling service options
const snm_config_type  mccso_rs2_sig_so_default_cfg =
{
  {
    /* Forward link defaults */
    CAI_MUX2,                      /* Default forward multiplex option = 2 */
    CAI_RATE_14400,                /* Forward rate set = rate set 2        */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  },
  {
    /* Reverse link defaults */
    CAI_MUX2,                      /* Default reverse multiplex option = 2 */
    CAI_RATE_14400,                /* Reverse rate set = 2                 */
    CAI_RS2_ALL,                   /* All rates allowed                    */
    CAI_NUL_TRAF                   /* No traffic used                      */
  }
};

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
/*===========================================================================

FUNCTION mccso_cdmacodec_mvs_cb_func

DESCRIPTION
  This method is used to register with MVS task along with mvs_acquire ()
  API. It will be called under MVS task context to post the CDMA vocoder
  acquisation enable status whenever (1) mvs_acquire (), (2) mvs_enable () 
  and (3) mvs_standby ()  APIs are getting called from CP/MUX layers. It 
  will also post the MC_MVS_ENABLE_STATUS_F and MC_MVS_STANDBY_STATUS_F 
  command to the MC task with MVS enable and standby status respectively.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccso_cdmacodec_mvs_cb_func(mvs_event_type *event)
{
  mc_msg_type *mc_cmd_ptr=NULL;

  switch (event->hdr.event)
  {
    case MVS_EV_COMMAND:

      if(event->cmd.cmd_status == MVS_CMD_SUCCESS)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "MVS command is successful");

        /* mvs_cdma_standby_in_progress is set by RXC task, when the hard
        ** handoff has happened and mvs_standby() has been called */
        if ( rxc_mvs_get_standby_progress_status() == TRUE )
        {
          /* Send the MVS Standby status to the MC */

          REX_ISR_LOCK(&mc_crit_sect);
          /* Send the MVS standby status to the MC */
          mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );
          if(mc_cmd_ptr == NULL)
          {
            ERR_FATAL("Can't get MC Buffer",0,0,0);
          } /* if(mc_cmd_ptr == NULL) */
          else
          {
            mc_cmd_ptr->hdr.cmd = MC_MVS_STANDBY_STATUS_F;
            mc_cmd_ptr->mvs_standby.standby_status = MVS_STANDBY_SUCCESS;

            mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;

            /* Initialize and link the command onto the MC command queue,
            * set a signal to the MC task and exit */
            M1X_MSG( DCP, LEGACY_MED,
              "Send MC_MVS_STANDBY_STATUS_F to MC from MVS");

            mc_cmd(mc_cmd_ptr);
          } /* else */
          REX_ISR_UNLOCK(&mc_crit_sect);
          break;
        }/* if ( rxc_mvs_get_standby_progress_status() ) */
      }
      else if ((event->cmd.cmd_status == MVS_CMD_FAILURE) ||
               (event->cmd.cmd_status == MVS_CMD_BUSY) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "MVS acquire command is failed");
      }
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "MVS invalid command = %d",
          event->cmd.cmd_status);
      }
      break;

    case MVS_EV_MODE:

      M1X_MSG( DCP, LEGACY_MED,
        "MVS_EV_MODE: type %d, status %d ",
        event->mode.mode,
        event->mode.mode_status);

      /* MVS Mode is initializing */
      if(event->mode.mode_status == MVS_MODE_INIT)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "MVS mode = MVS_MODE_INIT, %d",
          event->mode.mode_status);
      }
      else if ((event->mode.mode_status == MVS_MODE_READY) &&
               (event->mode.mode != MVS_MODE_NONE))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "MVS enable mode is successful = MVS_MODE_READY, %d",
          event->mode.mode_status);

        /* Send the MVS enable status to the MC */

        REX_ISR_LOCK(&mc_crit_sect);
        /* Send the MVS enable status to the MC */
        mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );
        if(mc_cmd_ptr == NULL)
        {
          ERR_FATAL("Can't get MC Buffer",0,0,0);
        }
        else
        {
          mc_cmd_ptr->hdr.cmd = MC_MVS_ENABLE_STATUS_F;
          mc_cmd_ptr->mvs_enable.enable_status = MVS_ENABLE_SUCCESS;

          mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;

          /* Initialize and link the command onto the MC command queue,
          * set a signal to the MC task and exit.
          */
          M1X_MSG( DCP, LEGACY_MED,
            "Send Cmd %d to MC from MVS",
            mc_cmd_ptr->hdr.cmd );
          mc_cmd(mc_cmd_ptr);
        }
        REX_ISR_UNLOCK(&mc_crit_sect);
      }
      else if ( (event->mode.mode_status != MVS_MODE_READY) &&
                (event->mode.mode != MVS_MODE_NONE))
      {
         M1X_MSG( DCP, LEGACY_HIGH,
           "MVS enable is failed and status is %d",
           event->mode.mode_status);

         REX_ISR_LOCK(&mc_crit_sect);

         /* Send the MVS enable status to the MC */
         mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );
         if(mc_cmd_ptr == NULL)
         {
           ERR_FATAL("Can't get MC Buffer",0,0,0);
         }
         else
         {
           mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;
           mc_cmd_ptr->hdr.cmd = MC_MVS_ENABLE_STATUS_F;
           mc_cmd_ptr->mvs_enable.enable_status = MVS_ENABLE_FAIL;

          /* Initialize and link the command onto the MC command queue,
          * set a signal to the MC task and exit.
          */
          M1X_MSG( DCP, LEGACY_MED,
            "Send Cmd %d to MC from MVS",
            mc_cmd_ptr->hdr.cmd );
          mc_cmd(mc_cmd_ptr);
         }
        REX_ISR_UNLOCK(&mc_crit_sect);

      }
      else
      {
        if ( (event->mode.mode_status == MVS_MODE_READY) &&
             (event->mode.mode == MVS_MODE_NONE))
        {
          M1X_MSG( DCP, LEGACY_MED,
            "MVS Release is successful");
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "MVS Release is failed");
        }
      }
      break;

    default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "MVS unknown event %d =",
          event->hdr.event);
        break;
  }
}

/*===========================================================================

FUNCTION MCCSO_IS_MVS_MODE_SUPPORTED

DESCRIPTION
  This function checks if a particular MVS mode is supported or not for
  old Audio architecture. 
  For new Audio architecture, all modes are supported on current targets
  as per inputs from Audio team so this API should always return TRUE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccso_is_mvs_mode_supported ( mvs_mode_type mode )
{
#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE  
  return TRUE;    
#else  
  return mvs_mode_supported(mode);
#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */
} /* mccso_is_mvs_mode_supported */

/*===========================================================================

FUNCTION MCCSO_SET_AVG_ENCODER_RATE

DESCRIPTION
  This function provides the average encoder to MVS or the CDMA Voice Adapter.
  Depending on the audio architecture being used, this function will invoke MVS
  API or provide required EVENT to CDMA Voice Adapter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccso_set_avg_encoder_rate ( byte rate )
{
  #ifdef FEATURE_VOICE_AGENT_ARCHITECTURE    
  /* To store the COP Macro defined corresponding to the absolute COP value */
  uint32 mrate; 
  /* Payload for Average encoder rate event */
  onex_voc_event_rqst_avg_enc_rate_t* enc_rate_params;
  /* CDMA Voice Adapter parameters */
  onex_voc_cmd_open_t voice_adapter_params = mcc_get_vadapter_params();
  
  /* Convert COP value to COP macro before passing it to MUX/Audio */
  switch (rate)
  {
    case 0:
      mrate = ONEX_VOC_COP_0;
      break;

    case 1:
      mrate = ONEX_VOC_COP_1;
      break;

    case 2:
      mrate = ONEX_VOC_COP_2;
      break;      
    
    case 3:
      mrate = ONEX_VOC_COP_3;
      break;
     
    case 4:
      mrate = ONEX_VOC_COP_4;
      break;  

    case 5:
      mrate = ONEX_VOC_COP_5;
      break;  

    case 6:
      mrate = ONEX_VOC_COP_6;
      break;  

    case 7:
      mrate = ONEX_VOC_COP_7;
      break;        
      
    default:
      mrate = ONEX_VOC_COP_0;
      M1X_MSG( DCP, LEGACY_ERROR,
               "Unexpected COP value %d, defaulting to COP_0", rate);      
      break;
  } /* switch (rate) */

  /* Set the global variable for avg encoder rate which may be used by MUX */
  mcc_set_avg_enc_rate( mrate );
  
  enc_rate_params = (onex_voc_event_rqst_avg_enc_rate_t*) modem_mem_alloc(
                     sizeof(onex_voc_event_rqst_avg_enc_rate_t), 
                     MODEM_MEM_CLIENT_1X );

  /* With new Audio architecture, 1X will send the unaltered rate. 
   * On MVS based old architecture 1X used to send the rate multiplied 
   * by 2 and MVS used to divide the rate value received from 1X by 2.  
   * This was done to handle the case of IS-96(A) TR-45 (13kbps).
   *
   * But now there is no such requirement of special handling of TR-45 
   * (13kbps) case. So on new Audio architecture, 1X won't multiply the 
   * rate by 2 and correspondingly MVS won't divide the value of rate 
   * received by 2 */

  if(enc_rate_params == NULL)
  {
    ERR_FATAL("Memory allocation failure!", 0, 0, 0);
  }
  else
  {
    enc_rate_params->avg_enc_rate = mrate;
  
    voice_adapter_params.event_cb( voice_adapter_params.session_context,
                                 ONEX_VOC_EVENT_RQST_AVG_ENC_RATE,
                                 enc_rate_params, 
                                 sizeof(onex_voc_event_rqst_avg_enc_rate_t)
                               );
    M1X_MSG( DCP, LEGACY_MED,
      "Sent average encoder rate to Audio, rate = 0x%x", mrate);
  }
  #else    
  /* To be compatible with the old rate reduction parameters in
   * the driver we need to multiply the new factor by 2. */    
  (void) mvs_voc_tx_rate_limit(rate * 2);
  
  #endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

}/* mccso_set_avg_encoder_rate */

#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/*===========================================================================

FUNCTION  mccso_so_voice_con_initializer

DESCRIPTION
  Called when a voice call is connected.  Goes ahead and configures up the
  vocoder capability.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void mccso_so_voice_con_initializer
(
  const caii_srv_cfg_type*  cfg_info,
  word                      so_index
)
{

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT

  #ifdef FEATURE_VOICE_AGENT_ARCHITECTURE

  mcc_is_mvs_acquired = TRUE;
  
  #else
  
  /* Acquire vocoder if we haven't done so */
  #ifdef FEATURE_VCC
  /* This feature supports WLANs <--> 1X handoffs.
  * For WLAN --> 1X handoff cases release VOIP vocoder
  * before acquiring the CDMA vocoder.
  */
  M1X_MSG( DCP, LEGACY_HIGH,
    "MC(1X) releases (VOIP) vocoder");
  mvs_release(MVS_CLIENT_VOIP);
  #endif

  /* If 1X has the MVS acquired already, just return from here
   * without calling mvs_acquire again. This avoids the duplicate mvs_acquire calls
   * which cause reset in the vocoder configuration.
   */
  if ( mcc_is_mvs_acquired )
  {
    return;
  }

  mvs_acquire(MVS_CLIENT_CDMA, mccso_cdmacodec_mvs_cb_func);

  /* mark the vocoder acquired status and this will remain TRUE till the end of
   * the voice SO connection.
   */
  mcc_is_mvs_acquired = TRUE;
  
  #endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

} /* mccso_so_voice_con_initializer */ /*lint !e715*/

/*===========================================================================

FUNCTION MCCSO_RELEASE_MVS

DESCRIPTION
  This function releases MVS if it was acquired for CDMA voice services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccso_release_mvs ( void )
{
 #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
 if (mcc_is_mvs_acquired)
 {
    #ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
    onex_voc_cmd_open_t voice_adapter_params = mcc_get_vadapter_params();
    voice_adapter_params.event_cb( voice_adapter_params.session_context,
                                   ONEX_VOC_EVENT_RQST_STOP,
                                   NULL, /* STOP Event has no parameters */
                                   0
                                 );
    M1X_MSG( DCP, LEGACY_MED,
             "Sent STOP Request to Audio" );
    #else
    mvs_release(MVS_CLIENT_CDMA);
    #endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */
   mcc_is_mvs_acquired = FALSE;
 }
 #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */
}

/*===========================================================================

FUNCTION mccso_initialize

DESCRIPTION
  This function initializes all the service options defined in this file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccso_initialize( void )
{

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
  nv_stat_enum_type status; /* status returned by NV */
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

#ifdef FEATURE_1X_CUST_VOICE_SO
  byte i;
#endif /* FEATURE_1X_CUST_VOICE_SO */

  // Test service options
  (void)snm_add_so( CAI_SO_MARKOV_SO54, (snm_neg_par_type*)&mccso_so54_neg_par, &mccso_rs1_so_default_cfg );
#ifdef FEATURE_IS2000_1X_ADV
  (void)snm_add_so( CAI_SO_MARKOV_SO74, (snm_neg_par_type*)&mccso_so74_neg_par, &mccso_rs1_so_default_cfg );
#endif /* FEATURE_IS2000_1X_ADV */
  (void)snm_add_so( CAI_SO_MARKOV, (snm_neg_par_type*)&mccso_so8002h_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_MARKOV_13K, (snm_neg_par_type*)&mccso_so801ch_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS1_MARKOV, (snm_neg_par_type*)&mccso_so801eh_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS2_MARKOV, (snm_neg_par_type*)&mccso_so801fh_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_LOOPBACK_SO55, (snm_neg_par_type*)&mccso_so55_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_LOOPBACK, (snm_neg_par_type*)&mccso_so2_neg_par, &mccso_rs1_so_default_cfg );
  (void)snm_add_so( CAI_SO_LOOPBACK_13K, (snm_neg_par_type*)&mccso_so9_neg_par, &mccso_rs2_so_default_cfg );
#ifdef FEATURE_IS2000_1X_ADV
  (void)snm_add_so( CAI_SO_LOOPBACK_SO75,  (snm_neg_par_type*)&mccso_so75_neg_par,&mccso_rs1_so_default_cfg);
#endif /* FEATURE_IS2000_1X_ADV */

  // Data burst service options
  (void)snm_add_so( CAI_SO_SMS, (snm_neg_par_type*)&mccso_so6_neg_par, &mccso_rs1_sig_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS2_SMS, (snm_neg_par_type*)&mccso_so14_neg_par, &mccso_rs2_sig_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS1_OTAPA, (snm_neg_par_type*)&mccso_so18_neg_par, &mccso_rs1_sig_so_default_cfg );
  (void)snm_add_so( CAI_SO_RS2_OTAPA, (snm_neg_par_type*)&mccso_so19_neg_par, &mccso_rs2_sig_so_default_cfg );
#ifdef FEATURE_SO_8026
  (void)snm_add_so( CAI_SO_PROPRIETARY_8026, (snm_neg_par_type*)&mccso_so8026_neg_par, &mccso_rs1_sig_so_default_cfg );
#endif

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
  (void)snm_add_so( CAI_SO_CSFB_TUNNELED_SMS, (snm_neg_par_type*)&mccso_so6_neg_par, &mccso_rs1_sig_so_default_cfg );
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

  // Voice Service Options
  (void)snm_add_so( CAI_SO_VOICE_13K, (snm_neg_par_type*)&mccso_so8000h_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_VOICE_13K_IS733, (snm_neg_par_type*)&mccso_so17_neg_par, &mccso_rs2_so_default_cfg );
  (void)snm_add_so( CAI_SO_VOICE_IS96A, (snm_neg_par_type*)&mccso_so1_neg_par, &mccso_rs1_so_default_cfg );

#ifdef FEATURE_1X_CUST_VOICE_SO
  if ( mcc_get_cust_so() != CAI_SO_NULL )
  {
    /* Adding customer specific voice SO in EVRC alt SO list. Since Cust SO
     * value is not fixed we could not add it while initializing alt SO list for EVRC */
    for (i = 0; i < MAX_ALT_SO; i++)
    {
      if( mccso_so3_neg_par.alternate_so[i] == CAI_SO_NULL )
      {
        mccso_so3_neg_par.alternate_so[i] = mcc_get_cust_so();  
        break;
      }
    } /* for (i = 0; */
    (void)snm_add_so( mcc_get_cust_so(), (snm_neg_par_type*)&mccso_cust_so_neg_par, &mccso_rs1_so_default_cfg );
  } /* if ( mcc_get_cust_so() != CAI_SO_NULL ) */ 
#endif /* FEATURE_1X_CUST_VOICE_SO */
  (void)snm_add_so( CAI_SO_VOICE_EVRC, (snm_neg_par_type*)&mccso_so3_neg_par, &mccso_rs1_so_default_cfg );
 
  (void)snm_add_so( CAI_SO_VOICE_SMV, (snm_neg_par_type*)&mccso_so56_neg_par, &mccso_rs1_so_default_cfg );

  (void)snm_add_so( CAI_SO_VOICE_4GV_NB, (snm_neg_par_type*)&mccso_so4gv_nb_neg_par, &mccso_rs1_so_default_cfg);
  (void)snm_add_so( CAI_SO_VOICE_4GV_WB, (snm_neg_par_type*)&mccso_so4gv_wb_neg_par, &mccso_rs1_so_default_cfg);

  (void)snm_add_so(CAI_SO_VOICE_EVRC_NW,  (snm_neg_par_type*)&mccso_so73_neg_par,&mccso_rs1_so_default_cfg);

  /*  Register the connection_initializer call back function for the
      CAI_SO_VOICE* service options to configure the vocoder on service
      connect
  */
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_13K,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_13K_IS733,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_IS96A,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_EVRC,
                                         mccso_so_voice_con_initializer);
#ifdef FEATURE_1X_CUST_VOICE_SO
  if ( mcc_get_cust_so() != CAI_SO_NULL )
  {  
    (void)snm_reg_connection_initializer(mcc_get_cust_so(),
                                         mccso_so_voice_con_initializer);  
  }    
#endif /* FEATURE_1X_CUST_VOICE_SO */
                                      
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_SMV,
                                         mccso_so_voice_con_initializer);

  (void)snm_reg_connection_initializer(CAI_SO_VOICE_4GV_NB,
                                         mccso_so_voice_con_initializer);
  (void)snm_reg_connection_initializer(CAI_SO_VOICE_4GV_WB,
                                         mccso_so_voice_con_initializer);

  (void)snm_reg_connection_initializer(CAI_SO_VOICE_EVRC_NW,
                                         mccso_so_voice_con_initializer);

  /* Keep SO = 1 outside of 1X_VOICE_SUPPORT feature.
   * MVS doesn't support half rate vocoder rates (IS96A). So disable IS96A
   * vocoder rate
   */
  (void)snm_disable_so( CAI_SO_VOICE_IS96A );

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT

  /* Initialize the common fields of the command we will send to NV */
  mcc_nv_buf.cmd        = NV_READ_F;        /* We will do reads        */
  mcc_nv_buf.data_ptr   = &mcc_nv_data;     /* Initialize data pointer */

#ifndef FEATURE_CDSMS
  (void)snm_disable_so( CAI_SO_SMS );
  (void)snm_disable_so( CAI_SO_RS2_SMS );
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
  (void)snm_disable_so( CAI_SO_CSFB_TUNNELED_SMS);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
#endif  /* FEATURE_CDSMS */

  if ( !mccso_is_mvs_mode_supported(MVS_MODE_IS127) )
  {
    (void)snm_disable_so( CAI_SO_VOICE_EVRC );
#ifdef FEATURE_1X_CUST_VOICE_SO
    if ( mcc_get_cust_so() != CAI_SO_NULL )
    {
      (void)snm_disable_so( mcc_get_cust_so() );
    }
#endif /* FEATURE_1X_CUST_VOICE_SO */
  }

  /* MVS doesn't support SMV vocoder rate. So disable SMV vocoder rate */
  (void)snm_disable_so( CAI_SO_VOICE_SMV );


  /* Read whether 4GV_NB is enabled */
  mcc_nv_buf.item = NV_CDMA_SO68_ENABLED_I;
  status = mcc_nv_cmd( &mcc_nv_buf );

  if( ((status == NV_DONE_S) && (!mcc_nv_data.cdma_so68_enabled)) ||
      (status == NV_NOTACTIVE_S) ||
      (!mccso_is_mvs_mode_supported(MVS_MODE_4GV_NB))
    )

  {
    (void)snm_disable_so( CAI_SO_VOICE_4GV_NB );
  }


  /* Read whether 4GV_WB is enabled */
  mcc_nv_buf.item = NV_CDMA_SO70_ENABLED_I;
  status = mcc_nv_cmd( &mcc_nv_buf );

  if( ((status == NV_DONE_S) && (!mcc_nv_data.cdma_so70_enabled)) ||
      (status == NV_NOTACTIVE_S) ||
      (!mccso_is_mvs_mode_supported(MVS_MODE_4GV_WB))
    )

  {
    (void)snm_disable_so( CAI_SO_VOICE_4GV_WB );
  }

  /* NV#7166 is applicable only for Non-1xAdvanced. 1xAdvanced guarantees SO73. */
#ifdef FEATURE_IS2000_1X_ADV
  if(cdma.onexadv_status.status == MC_1XADVANCED_DISABLED)
#endif /* FEATURE_IS2000_1X_ADV */
  {
  /* Read whether 4GV_NW is enabled */
  mcc_nv_buf.item = NV_CDMA_SO73_ENABLED_I;
  status = mcc_nv_cmd( &mcc_nv_buf );
  }

  if(
#ifdef FEATURE_IS2000_1X_ADV
      (cdma.onexadv_status.status ==  MC_1XADVANCED_DISABLED) &&
#endif /* FEATURE_IS2000_1X_ADV */
      (((status == NV_DONE_S) && (!mcc_nv_data.cdma_so73_enabled)) ||
      (status == NV_NOTACTIVE_S) ||
      (!mccso_is_mvs_mode_supported(MVS_MODE_4GV_NW)))
    )
  {
    (void)snm_disable_so( CAI_SO_VOICE_EVRC_NW );
  }

  if ( !mccso_is_mvs_mode_supported(MVS_MODE_IS733) )
  {
    (void)snm_disable_so( CAI_SO_VOICE_13K );
    (void)snm_disable_so( CAI_SO_VOICE_13K_IS733 );
  }

#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

} /* mccso_initialize */



