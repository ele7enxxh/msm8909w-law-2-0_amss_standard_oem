/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              T E S T   D A T A   S E R V I C E   O P T I O N (TDSO)
                                                                          +


GENERAL DESCRIPTION
  This file contains the data structures and functions needed to perform the
  Test Data (TDSO) Service options.  For Simple TDSO and Full TDSO.

EXTERNALIZED FUNCTION :S

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc.
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/tdso.c_v   1.26   29 Aug 2002 19:28:40   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/tdso.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/13   srk     Use TCB_PTR MACRO instead of rex_self() or tcb_ptr to make
                   the code compatible with RCINIT.
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
01/03/13   nee     Replacement of memcpy() to memscpy() Phase 2
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
09/24/12   ppr     Added defencive checks for NULL ptr access
09/15/12   ppr     FR#2370 - 1x memory optimization changes
03/07/12   srk     Replaced MODEM_LEGACY_STARTED with RCINIT_RCEVT_INITGROUPS.
03/06/12   srk     Added RCInit Framework.
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
12/13/11   srk     Feature clean up: Mainlined FEATURE_1X_WDOG_DYNAMIC
09/28/11   srk     Mainlined Always On features and removed Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
01/27/11   azh     Lint and KW fixes.
01/07/11   ag      Included cai_i.h.
12/17/10   ssh     Changed the watchdog timout value for TDSO task to
                   DOG_DEFAULT_TIMEOUT.
12/15/10   ag      Added support for 32x rate for SCH for SO32.
11/15/10   jtm     Initial SU API split for CP.
11/02/10   ag      Added support for dynamic watchdog.
08/02/10   ssh     Fixed Klockwork warning.
07/15/10   ssh     Fixed Compiler Warnings.
07/13/10   ag      Added support for RC8/11 for FCH/SCH for TDSO.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
06/10/10   jtm     Eliminated dependency on log_dmss.h.
03/14/10   ag      Added RC(11,8) support to all SOs supporting Mux option 1.
03/04/10   jj      Replaced deprecated task_XXX APIs with tmc_XXX APIs.
11/02/09   jtm     Fixed compiler warnings.
10/09/09   ssh     Added array bound check for fwd_arr as part of KW error
                   fix.
09/22/09   ss      Removed redundant DS header include.
08/02/09   jtm     Lint fixes.
07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
                   have a single value.
06/30/09   ag      Diag decoupling changes for CMI
05/03/09   jj      CMI Phase-II update.
04/13/09   adw     Added tdso_v.h include for CMI.
03/20/09   jtm     Added txcmc_v.h include.
03/13/09   adw     Lint fixes.
03/03/09   jj      Featurized Q6 changes.
12/10/08   jj      QDSP6 migration porting effort, as a part of which replaced
                   INTLOCK and INTFREE with REX_ENABLE_PREMPTION and
                   REX_DISABLE_PREMPTION.
06/19/08   ag      Fixed compiler warning.
06/11/08   ag      Fixed more Klocwork errors
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/25/08   bb      Fix the Klocwork errors
12/14/07   an      Added Passport Mode
06/07/07   pg      Lint clean up.
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/19/07   pg      Lint Cleanup.
12/12/06   pg      Fixed Lint error.
08/01/06   fc      Merged the following from MSMSHARED:
08/01/06   fh      Made klokwork fixes.
06/23/06   pg      RPC cleanup.
05/30/06   fc      Fixed lint errors.
04/26/06   an      Fixed Lint Errors
10/10/05   fh      Removed including of dec.h. changed dec_rate_set_type to
                     cai_rate_set_type.
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of txcmc.h
07/05/05   sb      Added MEID support.
04/19/05   fc      Merged the following from MSMSHARED:
03/24/05   an       Replaced ds707.h by ds707_extif.h
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
12/10/04   sb      Fixed lint error.
08/09/04   fc      Service negotiation changes for Release C and D.
07/09/04   ht      Added TDSO support for Rel C DV channels
06/08/04   bkm     Fixed typos in feature names within comments.
03/05/04   bkm     Lint cleanup.
08/01/03   yll     Added support for Dynamic Feature.
06/13/03   bkm     Fixed P2 featurization error in
                   tdso_generate_rand_circ_buffer().
06/02/03   bkm     Lint cleanup.
01/10/03   lh      Added FEATURE_IS2000_REL_A around 32X related counters.
08/28/02   fc      Cleaned lint errors.
08/01/02   lh      Fixed Rel0 compilation problems.
06/28/02   lh      Reverse MuxPUD 5 support & Forward MuxPDU clean up.
06/20/02   az      Fixed compiler warning
06/19/02   lh      Fixed Rel0 compilation problems.
06/07/02   jqi     Updated with multimode task operation interface.
05/24/02   lh      Fixed FTDSO.
05/07/02   ph      Fixed compile errors when REL A is turned off.
04/08/02   va      Fixed the array size of tdso_data_len
04/01/02   ph      Look in to num_sch in addition to incl fields before using supplemental
                   channel.
02/12/02   yll     Merged from common archive CP VU 116.
                   01/28/02   fc
                   Corrected the update TDSO state when TX_ON_PERIOD is 0 or
                   TX_OFF_PERIOD is 0.
                   01/09/02   fc
                   Changed fwd and rev max mux option of stdso_neg_par and
                   ftdso_neg_par to CAI_MUX_WILDCARD.
                   12/13/01   fc
                   Corrected the update of DCCH FTDSO state.
                   Corrected the update of FTDSO test parameters from SOCM.
01/23/02   va      Added SO Group info  to neg par.
12/13/01   va      Merged from SVD branch
  09/26/01 kk      Secondary support for STDSO added.
  08/23/01 ph      Updated connection_initializer to use connection number.
12/07/01   fc      Corrected SCH NUM_RAND setting.
                   Changes to sync SCH PN with DCCH PN if DCCH full TDSO is on.
10/23/01   kk      Merge from common archive
                   10/18/01   kk      Fixed DTX reporting to QXDM for Forward DCCH.
                   10/05/01   lcc     Changes to use new interface for SCH callbacks.
                   09/24/01   kk      STDSO statistics fix when P2 mode is NOT defined.
10/15/01   kk      Fixed warning Release A is not defined.
09/13/01   kk      Carried over fixes from Main Archive.
                   09/13/01   kk      Data Pattern settings modified.
                   09/12/01   kk      Corrected uninitialized fields before reporting to QXDM.
08/10/01   kk      Added MuxPDU type 5 support.
08/09/01   kk      Merged from common archive.
                   08/06/01   kk      DCCH Full TDSO support.
                   07/26/01   fc      Removed unnecessary data pattern checking.
05/30/01   kk      Increased sync buffer size for DCCH and SCH.
05/29/01   kk      Fixed 1x problem.
05/22/01   kk      TDSO RS2 support.
05/18/01   fc      Changed negotiation parameters of FTDSO to allow FCH only.
05/11/01   fc      Added support of DCCH STDSO.
                   Cleaned lint errors.
04/10/01   lcc     1. Changes to accomodate new prototypes for snm_get_current_config.
                   2. Added P2 mode changes for snm_neg_par_type.
04/02/01   ks      Calling the INTFREE_SAV() to restore the interrupts before
                   returning the status in function tdso_build_tx_frames().
03/22/01   lcc     1. Rename function tdso_send_handoff_complete to
                      tdso_notify_hho_success and added checking of full TDSO in
                      function before proceeding.
                   2. Added function tdso_end_call to perform operation necessary
                      for ending TDSO call.
                   3. Added tdso_call_on to indicate if TDSO call is on.
03/12/01   fc      Added "#include msg.h" and "include err.h".
                   Removed the redundant declaration of tdso_log_stats.
03/09/01   fc      Featurized all reference to R-SCH functions or data
                   structures with FEATURE_IS2000_R_SCH.
03/07/01   ks      fixed the enum for tdso_e1_rerr value in FCH receive counter
                   retrival. Also setting tb_to value in SCH transmit counters.
03/06/01   fc      Added SOCM checking to reject any TDSO control directive if
                   it specifies full-rate frames with circular buffer size > 2.
02/08/01   ks      Added SOCM checking before 1sec of the SYNC frame. Also
                   TDSO now doesn't clear the CNTS after a SCH config change.
02/09/01   jal     Clear {F|S}TDSO statistics via DIAG.
01/20/01   ks      Added support for FULL TDSO functionality. Also added
                   support for TDSO task functions.
11/27/00   ks      resetting clr_cnts & cpy_cnts fields to 0 after clearing
                   or copying the counters at the sync_frame.
11/17/00   ks      Removed the code that was added for TESTING purpose.
11/13/00   ks      Added new function and set_num_rand_size and changed cleaned
                   up the debug statements.
11/05/00   ks      Added support for Full TDSO implementation
10/20/00   ak      Add call to ds_scrm_call_init at beginning of TDSO call.
09/20/00   fc      Added support for TDSO control directive and counter
                   retrieval directive.
08/11/00   va      Support for logging ERASURES on SCH.
07/10/00   ks      Added a null function to remove compiler warning.
07/10/00   ks      Added target.h file and check for FEATURE_IS2000_TDSO
07/06/00   ks      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "target.h"
#include "modem_1x_defs.h"
#include "log.h"
#include "cdma2kdiag.h"

#include "cai_i.h"
#include "dog.h"
#include "dog_hb_rex.h"
#include "mc_i.h"
#include "tdso.h"
#include "tdso_v.h"
#include "mar_i.h"
#include "martable.h"
#include "mccdma.h"
#include <memory.h>
#include "bit.h"
#include "rex.h"
#include "math.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "mcctcsup.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagbuf.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "mccap_v.h"
#include "txcmc.h"
#include "txcmc_i.h"
#include "rcinit.h"
#include "snm_v.h"
#include "snm_i.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* -------------------------------------------------------------------
** FCH or DCCH Transmitted Frame Enumerations when TDSO is ON or STDSO
** ------------------------------------------------------------------- */
enum
{
  FDCHTX_E1_TRAFFIC,
  FDCHTX_E1_DIM_BURST,
  FDCHTX_E1_BLANK
};

/* --------------------------------------------------------------------
** FCH or DCCH Transmitted Frame Enumerations when TDSO is OFF or STDSO
** -------------------------------------------------------------------- */
enum
{
  FDCHTX_BLANK_BLANK,
  FDCHTX_BLANK_OTHER
};

/* -----------------------------------------------------------------
** FCH or DCCH Received Frame Enumerations when TDSO is OFF or STDSO
** ----------------------------------------------------------------- */
enum
{
  FDCHRX_NULL_NULL,
  FDCHRX_NULL_BLANK,
  FDCHRX_NULL_OTHER
};

/* -----------------------------------------------------------
** SCH Transmitted Frame Enumerations when TDSO is ON or STDSO
** ----------------------------------------------------------- */
enum
{
  SCHTX_EN_TRAFFIC,
  SCHTX_EN_BLANK
};

/* --------------------------------------------------------
** SCH Received Frame Enumerations when TDSO is ON or STDSO
** -------------------------------------------------------- */
enum
{
  SCHRX_EN_TRAFFIC,
  SCHRX_EN_FRAME_ERROR,
  SCHRX_EN_ERASURE,
  SCHRX_EN_BLANK
};

/* ---------------------------------------------------------
** SCH Received Frame Enumerations when TDSO is OFF or STDSO
** --------------------------------------------------------- */
enum
{
  SCHRX_BLANK_BLANK,
  SCHRX_BLANK_OTHER
};

/* -----------------------------
** SCH PDU counters enumerations
** ----------------------------- */
enum
{
  TDSO_RATE_3_PDU,  /* Rate 3 MuxPDU  */
  TDSO_RATE_2_PDU,  /* Rate 2 MuxPDU  */
  TDSO_RATE_1A_PDU, /* Rate 1a MuxPDU */
  TDSO_RATE_1B_PDU  /* Rate 1b MuxPDU */
};

/*---------------------------------------------------------------------------
  Definitions for Service Option 0x8008(Simple TDSO ) and 0x0020 (Full TDSO)
---------------------------------------------------------------------------*/
static const snm_config_type  tdso_default_cfg =
{
  {
    /* Forward link defaults */
    0x1,                         /* Default forward multiplex option = 7 */
    CAI_RATE_9600,                 /* Forward rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All transmission rates used          */
#ifdef FEATURE_IS2000_REL_A_SVD
    CAI_PRI_OR_SEC_TRAF            /* Primary and Secondary ok */
#else
    CAI_PRI_TRAF                   /* Primary traffic okay    */
#endif /* FEATURE_IS2000_REL_A_SVD */
  },
  {
    /* REVERSE link defaults */
    0x1,                         /* Default REVERSE multiplex option = 7 */
    CAI_RATE_9600,                 /* REVERSE rate set = rate set 1        */
    CAI_RS1_ALL,                   /* All transmission rates used          */
#ifdef FEATURE_IS2000_REL_A_SVD
    CAI_PRI_OR_SEC_TRAF            /* Primary and Secondary ok */
#else
    CAI_PRI_TRAF                   /* Primary traffic okay    */
#endif /* FEATURE_IS2000_REL_A_SVD */
  }
};

snm_neg_par_type  stdso_neg_par = /* Simple TDSO negotiation parameters */
{
  6,                                    /* Minimum PREV allowed = 6        */

  {                                     /* Alternate service options       */
    CAI_SO_NULL,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL, 0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD}, /* Maximum fwd and rev_link mux options */
  {
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1x phys channel (preferred)     */
      CAI_RC_4,                         /* 1x phys channel                 */
      CAI_RC_5,
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0

    }
  },
  {                                     /* REVERSE LINK  radio channel     */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* 1x phys channel                 */
      CAI_RC_4,
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0

    }
  }
  ,{                                       /* # of mux options for fwd sch  */
    (byte)16,                              /* preferences for fwd sch mos   */
    {
      (word)0x921,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x911,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x821,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x909,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x811,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x905,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x809,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x03,                         /* MuxPDU 1,  1x, single sch     */
      /* Rate Set 2 mux Options */
      (word)0x922,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x912,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x822,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x90A,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x812,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x906,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x80A,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x04,                         /* MuxPDU 2,  1x, single sch     */
      0, 0

    }
  },
  {                                       /* # of mux options for fwd sch  */
    (byte)16,                              /* preferences for fwd sch mos   */
    {
      (word)0x921,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x911,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x821,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x909,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x811,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x905,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x809,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x03,                         /* MuxPDU 1,  1x, single sch     */
      /* Rate Set 2 mux Options */
      (word)0x922,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x912,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x822,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x90A,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x812,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x906,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x80A,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x04,                         /* MuxPDU 2,  1x, single sch     */
      0, 0
    }
  }
  /* Channels allowed for this SO */
  ,{ SNM_DCCH, SNM_FCH, SNM_RESERVED }
#ifdef FEATURE_IS2000_REL_A
  ,CAI_SO_GROUP_UNASSIGNED
  ,CAI_SO_BITMAP_POSITION_UNASSIGNED
#endif /* FEATURE_IS2000_REL_A */
};

snm_neg_par_type  ftdso_neg_par = /* Full TDSO negotiation parmeters */
{
  6,                                    /* Minimum PREV allowed = 6        */

  {                                     /* Alternate service options       */
    CAI_SO_NULL,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
  },
  {                                     /* Compatible service options      */
    CAI_SO_NULL, 0
  },
  {CAI_MUX_WILDCARD, CAI_MUX_WILDCARD}, /* Maximum fwd and rev_link mux options */
  {
    #ifdef FEATURE_IS2000_1X_ADV
    4,                                  /* number of rc's in the list      */
    #else
    3,                                  /* number of rc's in the list      */
    #endif
    {                                   /* forward radio channel           */
      CAI_RC_3,                         /* 1x phys channel (preferred)     */
      CAI_RC_4,                         /* 1x phys channel                 */
      CAI_RC_5,
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_11,                        /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0

    }
  },
  {                                     /* REVERSE LINK  radio channel     */
    #ifdef FEATURE_IS2000_1X_ADV
    3,                                  /* number of rc's in the list      */
    #else
    2,                                  /* number of rc's in the list      */
    #endif
    {
      CAI_RC_3,                         /* 1x phys channel                 */
      CAI_RC_4,
      #ifdef FEATURE_IS2000_1X_ADV
      CAI_RC_8,                         /* 1x Adv, RS 1                    */
      #endif
      (cai_radio_config_type)0,
      (cai_radio_config_type)0,
      (cai_radio_config_type)0

    }
  }
  ,{                                       /* # of mux options for fwd sch  */
    (byte)16,                              /* preferences for fwd sch mos   */
    {
      (word)0x921,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x911,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x821,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x909,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x811,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x905,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x809,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x03,                         /* MuxPDU 1,  1x, single sch     */
      /* Rate Set 2 mux Options */
      (word)0x922,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x912,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x822,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x90A,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x812,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x906,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x80A,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x04,                         /* MuxPDU 2,  1x, single sch     */
//      , 0xf20, 0
      0, 0                                /* MuxPDU 5 not supported for now*/

    }
  },
  {                                       /* # of mux options for fwd sch  */
    (byte)16,                              /* preferences for fwd sch mos   */
    {
      (word)0x921,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x911,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x821,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x909,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x811,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x905,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x809,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x03,                         /* MuxPDU 1,  1x, single sch     */
      /* Rate Set 2 mux Options */
      (word)0x922,                        /* MuxPDU 3, 16x, double sch     */
      (word)0x912,                        /* MuxPDU 3,  8x, double sch     */
      (word)0x822,                        /* MuxPDU 3,  8x, single sch     */
      (word)0x90A,                        /* MuxPDU 3,  4x, double sch     */
      (word)0x812,                        /* MuxPDU 3,  4x, single sch     */
      (word)0x906,                        /* MuxPDU 3,  2x, double sch     */
      (word)0x80A,                        /* MuxPDU 3,  2x, single sch     */
      (word)0x04,                         /* MuxPDU 2,  1x, single sch     */
      0, 0                                /* MuxPDU 5 not supported for now*/
    }
  }
  /* Channels allowed for this SO */
  ,{ SNM_DCCH, SNM_FCH, SNM_RESERVED }

#ifdef FEATURE_IS2000_REL_A
  ,CAI_SO_GROUP_UNASSIGNED
  ,CAI_SO_BITMAP_POSITION_UNASSIGNED
#endif /* FEATURE_IS2000_REL_A */
};

#ifdef FEATURE_IS2000_REL_A_SVD
word tdso_secondary_len = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */
/* ----------------------------------------
** Table of bit data length for TDSO frames
** ---------------------------------------- */
const word tdso_data_len[TDSO_MAX_RATE][TDSO_MAX_RATE_SET] =
{
#ifdef FEATURE_IS2000_REL_A
  {738,  546},       /* This is only the max for TDSO_RATE_3. */
#endif
  {346,  538},       /* TDSO_RATE_2 */
  {171,  266},       /* TDSO_RATE_1 */
  {80,   124},       /* TDSO_RATE_HALF */
  {40,   54},        /* TDSO_RATE_QUARTER */
  {16,   20},        /* TDSO_RATE_EIGHTH */
  {0,     0},        /* TDSO_BLANK */
  {0,     0},        /* TDSO_ERASURE */
  {0,     0},        /* TDSO_DIM_BURST */
  {0,     0},        /* TDSO_BIT_ERROR */
  {0,     0},        /* TDSO_INSUFFICIENT_FRAME */
  {0,     0},        /* TDSO_BLANK_BURST */
  {0,     0},        /* TDSO_NULL */
  {0,     0},        /* TDSO_OTHER */
};

#ifdef FEATURE_IS2000_REL_A
/* This table is for Rate 3 (MuxPDU 5) convolutional coding only */
/* See table 2-5 of sTDSO standard */
#define TDSO_SUPPORTED_SCH_RATES    6    /* Table supports 1X ~ 32X */

/* MuxPDU Header for sTDSO is 6 bits */
#define MUXPDU5_HEADER_LEN          6    /* bits */

/* LTU CRC Length */
#define MUXPDU5_LTU_CRC_LEN         16    /* bits */

/* Total overhead if LTU is used */
#define MUXPDU5_LTU_OVHD    (MUXPDU5_HEADER_LEN+ MUXPDU5_LTU_CRC_LEN)

/* Following table has the MuxPDU 5 length within each LTU. Notice that the overhead
** is not accounted.
*/
const word tdso_muxpdu5_conv_data_len[TDSO_SUPPORTED_SCH_RATES][TDSO_MAX_RATE_SET] =
{
  {168-MUXPDU5_HEADER_LEN,   264-MUXPDU5_HEADER_LEN},  /* 1X does not have LTU */
  {360-MUXPDU5_HEADER_LEN,   552-MUXPDU5_HEADER_LEN},  /* 2X does not have LTU */
  {368-MUXPDU5_LTU_OVHD,     560-MUXPDU5_LTU_OVHD},    /* 4X has LTU */
  {376-MUXPDU5_LTU_OVHD,     568-MUXPDU5_LTU_OVHD},    /* 8X has LTU */
  {376-MUXPDU5_LTU_OVHD,     568-MUXPDU5_LTU_OVHD},    /* 16X has LTU */
  {760-MUXPDU5_LTU_OVHD,     0                   }     /* 32X has LTU, only for RS1 */
};

const byte tdso_default_ltu_table[TDSO_SUPPORTED_SCH_RATES][TDSO_MAX_RATE_SET] =
{   //  RS1      RS2             RS1       RS2
    //-----------------     ----------------------
    {   0,        0   },    //  172       267    bits
    {   0,        0   },    //  360       552
    {   2,        2   },    //  744      1128
    {   4,        4   },    // 1512      2280
    {   8,        8   },    // 3048      4584
    {   8,        0   },    // 6120      5178
};

/* For turbo case, we use byte oriented processing for MuxPDU 5. The length in the
** following table includes MuxPDU header, in bytes.
*/
const word tdso_muxpdu5_turbo_data_len[TDSO_SUPPORTED_SCH_RATES][TDSO_MAX_RATE_SET] =
{
  {172/8,      267/8 },         /* 1X */
  {360/8,      552/8 },         /* 2X */
  {744/8,      1128/8},        /* 4X */
  {1512/8,     2280/8},        /* 8X */
  {3048/8,     4584/8},        /* 16X */
  {6120/8,     0     }         /* 32X */
};
#endif /* FEATURE_IS2000_REL_A */

/* Number of bytes in the received TDSO frame -1 */
/* For release A, there is new RATE 3 introduced for MuxPDU 5.
** The number in this table represents the maximum case.
*/
word const tdso_bytes[TDSO_MAX_RATE][TDSO_MAX_RATE_SET] =
{
#ifdef FEATURE_IS2000_REL_A
    {738/8,    546/8},   /* This is only the max for TDSO_RATE_3. */
#endif /* FEATURE_IS2000_REL_A */
    {346/8,    538/8},   /* TDSO_RATE_2 */
    {171/8,    266/8},   /* TDSO_RATE_1 */
    {80/8-1,   124/8},   /* TDSO_RATE_HALF */
    {40/8-1,    54/8},   /* TDSO_RATE_QUARTER */
    {16/8-1,    20/8},   /* TDSO_RATE_EIGHTH */
    {0,            0},   /* TDSO_BLANK */
    {0,            0},   /* TDSO_ERASURE */
    {0,            0},   /* TDSO_DIM_BURST */
    {0,            0},   /* TDSO_BIT_ERROR */
    {0,            0},   /* TDSO_INSUFFICIENT_FRAME */
    {0,            0},   /* TDSO_BLANK_BURST */
    {0,            0},   /* TDSO_NULL */
    {0,            0},   /* TDSO_OTHER */
};

/* Mask last byte to compare bits */
word const tdso_last_byte[TDSO_MAX_RATE][TDSO_MAX_RATE_SET] =
{
#ifdef FEATURE_IS2000_REL_A
    {0xC0,      0xC0},   /* TDSO_RATE_3. */
#endif /* FEATURE_IS2000_REL_A */
    {0xC0,      0xC0},   /* TDSO_RATE_2 */
    {0xE0,      0xC0},   /* TDSO_RATE_1 */
    {0xFF,      0xF0},   /* TDSO_RATE_HALF */
    {0xFF,      0xFC},   /* TDSO_RATE_QUARTER */
    {0xFF,      0xF0},   /* TDSO_RATE_EIGHTH */
    {0,            0},   /* TDSO_BLANK */
    {0,            0},   /* TDSO_ERASURE */
    {0,            0},   /* TDSO_DIM_BURST */
    {0,            0},   /* TDSO_BIT_ERROR */
    {0,            0},   /* TDSO_INSUFFICIENT_FRAME */
    {0,            0},   /* TDSO_BLANK_BURST */
    {0,            0},   /* TDSO_NULL */
    {0,            0}   /* TDSO_OTHER */
};

/* Current TDSO SO in use, either S-TDSO or F-TDSO */
static word tdso_so;

/* Indicates if TDSO call is up */
static boolean tdso_call_on = FALSE;

dword tdso_rx_seed = DEF_RX_SEED;  /* current seed for rx TDSO model */
dword tdso_tx_seed = DEF_TX_SEED ; /* current seed for tx TDSO model */

/* Current Rate Set of the Mobile during TDSO */
    /* For DCCH or FCH */
static tdso_mux_type fdch_tdso_rate_set;

/* --------------------------------------------------------------------------
** Forward and reverse circular buffers for FCH, DCCH and SCH0 channels
** synchronization. For Maximum N = 50, where N = size of number of Full Rate
** Frames. These circular buffers are currently limited to N = 2.
** -------------------------------------------------------------------------- */
static byte fch_rx_circ_buffer[50];
static byte fch_tx_circ_buffer[50];
static byte dcch_rx_circ_buffer[50];
static byte dcch_tx_circ_buffer[50];

static byte sch_rx_circ_buffer[3120];
static byte sch_tx_circ_buffer[3120];

/*---------------------------------------------------------------------
** The TDSO channel parameters for FCH, DCCH and SCH0 channels
** The '0' entry in the array is FWD link and '1' entry is reverse link
** -------------------------------------------------------------------- */
tdso_chan_parms fch_parm;
tdso_chan_parms dcch_parm;
tdso_chan_parms sch0_parm;

/* --------------
** TDSO SCH0 info
** -------------- */
static struct
{
  boolean            sch_is_active; /* Indicates whether F-SCH is active */
  tdso_sch_rate_type sch_rx_rate;     /* F-SCH rate */
  tdso_sch_rate_type sch_tx_rate;     /* R-SCH rate */
} tdso_sch;

#ifdef FEATURE_1X_CP_MEM_OPT
/* Pointer holds TDSO Pre-Synchronization info */
tdso_pre_sync_type *tdso_pre_sync_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
/* TDSO Pre-Synchronization info */
tdso_pre_sync_type tdso_pre_sync;

/* Pointer holds TDSO Pre-Synchronization info */
tdso_pre_sync_type *tdso_pre_sync_ptr = &tdso_pre_sync;
#endif /* FEATURE_1X_CP_MEM_OPT */


/* SOCM processing info */
boolean           socm_deferred;
  /* Flag indicates whether to defer the update of TDSO test
     parameters from SOCM which is received in the 1 sec boundary of SYNC
     frame */
boolean           socm_ignored;
  /* Flag indicates whether we have received SOCM in the 1 sec boundary of
     SYNC frame */

/* --------------------------------------------------------------------
** FCH transmit counters.  The tables with suffix 'rpt' below, are used
** during counter retrieval when BS requests using a control derivative.
** Refer to the following tables in the TDSO standard for more info.
** Table 13, Table 14, Table 15, Table 16, Table 17.
** --------------------------------------------------------------------- */
static uint32 fch_tx_table[3],fch_tx_rpt[3] ;
static uint32 fch_tx_blank_table[2],fch_tx_blank_rpt[2];

/* ---------------------
** FCH receive counters.
** --------------------- */
static uint32 fch_rx_table[TDSO_MAX_RATE],fch_rx_rpt[TDSO_MAX_RATE];
static uint32 fch_rx_null_table[3],fch_rx_null_rpt[3];

/* ---------------------------------------------------------------------
** DCCH transmit counters.  The tables with suffix 'rpt' below, are used
** during counter retrieval when BS requests using a control derivative.
** Refer to the following tables in the TDSO standard for more info.
** Table 13, Table 14, Table 15, Table 16, Table 17.
** --------------------------------------------------------------------- */
static uint32 dcch_tx_table[3], dcch_tx_rpt[3] ;
static uint32 dcch_tx_blank_table[2],dcch_tx_blank_rpt[2];

/* ---------------------
** DCCH receive counters.
** --------------------- */
static uint32 dcch_rx_table[TDSO_MAX_RATE],dcch_rx_rpt[TDSO_MAX_RATE];
static uint32 dcch_rx_null_table[3],dcch_rx_null_rpt[3];

/* ----------------------
** SCH0 transmit counters
** ---------------------- */
static uint32 sch_tx_table[2][5], sch_tx_rpt[2][5];
static uint32 sch_tx_blank, sch_tx_blank_rpt ;

/* ---------------------
** SCH0 receive counters
** --------------------- */
#ifdef FEATURE_IS2000_REL_A
static uint32 sch_rx_table[4][6],sch_rx_rpt[4][6];
#else
static uint32 sch_rx_table[4][5],sch_rx_rpt[4][5];
#endif /* FEATURE_IS2000_REL_A */
static uint32 sch_rx_blank_table[2],sch_rx_blank_rpt[2];

/* ----------------------------------------------------------------
** SCH0 receive PDU counters (instantaneous use) with dimensions as
** Received MuxPDU type x Expected rate.
** ---------------------------------------------------------------- */
static uint32 sch_rx_pdu_table[4][3];

/* -----------------------------------------------------------------
** SCH0 receive PDU counters (buffer storage) with dimensions as
** Received MuxPDU type x Expected rate.
** ----------------------------------------------------------------- */
static uint32 sch_rx_pdu_buffer[4][3];

/* DOG report ID for DH task */
LOCAL dog_report_type tdso_dog_rpt_id = 0;

/*---------------------------------------------------------------------------
                  Variables pertaining to RCINIT
---------------------------------------------------------------------------*/
/* Create TCB pointer */
rex_tcb_type* tdso_tcb_ptr = NULL;

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/
void tdso_pre_sync_proc
(
  dword              time,
  tdso_link_dir_type direction
);
void tdso_sch_chng_cback
(
  mccsch_sch_info_type* sch_info
);
void tdso_sync_proc
(
  tdso_link_dir_type    direction,
  tdso_frame_block_type *frame_blk_ptr
);
void tdso_update_test_parameter
(
  tdso_tst_par_type tst_par
);
void tdso_sync_test_parameter
(
  tdso_link_dir_type direction
);
void tdso_generate_rand_circ_buffer
(
  tdso_buffer_info  *chan_pn_info_ptr,
  dword*            seed_ptr,
  tdso_channel_type channel
);
void tdso_pack_datablk_from_circ_buffer
(
  tdso_tx_frame_type* tx_frame_ptr,
  tdso_link_dir_type  direction
);
void tdso_set_num_rand
(
  tdso_channel_type  channel,
  tdso_link_dir_type direction
);
void tdso_update_pn_offset_info
(
  tdso_buffer_info *chan_pn_info_ptr
);
void tdso_update_on_off_state
(
  tdso_channel_type  chan,
  tdso_link_dir_type direction
);
void tdso_update_on_off_threshold
(
  tdso_chan_parms    *chan_parm_ptr,
  tdso_link_dir_type direction
);
void tdso_clear_counter_buffer
(
  tdso_link_dir_type direction
);
void tdso_copy_counter_buffer
(
  tdso_link_dir_type direction
);
tdso_status_type tdso_process_fch_rx_frame
(
  tdso_frame_block_type *frame_blk_ptr
);
tdso_status_type tdso_process_sch_rx_frame
(
  tdso_frame_block_type *frame_blk_ptr
);
void tdso_update_fch_tx_stats
(
  tdso_tx_frame_type *tx_frame_ptr,
  boolean            tdso_state
);
void tdso_update_sch_tx_stats
(
  tdso_tx_frame_type* tx_frame_ptr,
  boolean             tdso_state
);
void tdso_update_fch_rx_stats
(
  tdso_rx_frame_type *rx_frame_ptr,
  boolean            tdso_state
);
void tdso_update_sch_rx_stats
(
  tdso_rx_frame_type* rx_frame_ptr,
  boolean             tdso_state
);
void tdso_update_sch_pdu_stats
(
  tdso_rx_frame_type* rx_frame_ptr
);

void tdso_update_dcch_tx_stats
(
  tdso_tx_frame_type *tx_frame_ptr,
  boolean            tdso_state
);
tdso_status_type tdso_process_dcch_rx_frame
(
  tdso_frame_block_type *frame_blk_ptr
);
void tdso_log_stats(void);
void tdso_log_stdso_stats(void);
void tdso_log_ftdso_stats(void);


/*===========================================================================

FUNCTION TDSO_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tdso_dog_report ( void )
{
  dog_hb_report(tdso_dog_rpt_id);
  (void) rex_clr_sigs( TDSO_TCB_PTR, TDSO_DOG_RPT_SIG );

} /* tdso_dog_report */


/*===========================================================================

FUNCTION TDSO_TASK_INIT

DESCRIPTION
  This function initializes the tcb pointer and other pre initializations that
  needs to be done only after TCB_PTR is updated.

DEPENDENCIES
  Must be called from "tdso" task context.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tdso_task_init(void)
{
  /* Update the TDSO TCB pointer */
  TDSO_TCB_PTR = rex_self();

  /* -----------------------------------------------------------------
  ** Initialize DOG report ID and DOG report timeout.
  ** ----------------------------------------------------------------- */
  tdso_dog_rpt_id = dog_hb_register_rex( TDSO_DOG_RPT_SIG );

  /* ------------------------------------------------------------------
  ** Initially kick watchdog
  ** ------------------------------------------------------------------ */
  tdso_dog_report();

} /* tdso_task_init */

/*===========================================================================

FUNCTION tdso_task

DESCRIPTION
  This function is the entrance procedure for the TDSO Task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return

SIDE EFFECTS
  None

===========================================================================*/
void tdso_task
(
  dword ignored
    /* Parameter required for REX. Tell lint to ignore it */
    /*lint -esym(715,ignored) */
)
{
  rex_sigs_type sigs;


  /*---------------------*/
  /* Task initialization */
  /*---------------------*/
  tdso_task_init();

  /* Start handshake with RCINIT to start the init process */
  rcinit_handshake_startup();


  for (;;)   /* Infinite loop */
  {
    sigs = rex_wait( TDSO_DOG_RPT_SIG      |   /* Watchdog report timer */
                    TDSO_CMD_SIG          |   /* TDSO pre-sync         */
                    TASK_STOP_SIG         |   /* Power down            */
                    TASK_OFFLINE_SIG          /* Offline               */
                   );

    if ((sigs & TDSO_DOG_RPT_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      tdso_dog_report();
    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, process task stop procedure, and proceed.
    ** ------------------------------------------------------- */
    if ((sigs & TASK_STOP_SIG) != 0)
    {
      (void) rex_clr_sigs(TDSO_TCB_PTR, TASK_STOP_SIG);
    }

    /* -------------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, process task offline procedure, and
    ** proceed.
    ** ------------------------------------------------------- */
    if ((sigs & TASK_OFFLINE_SIG) != 0)
    {
      (void) rex_clr_sigs(TDSO_TCB_PTR, TASK_OFFLINE_SIG);
    }

    /* ------------------------------------------------------
    ** Check if TDSO pre-sync is requested. If set then clear
    ** signal and do pre-sync operations.
    ** ------------------------------------------------------ */
    if ((sigs & TDSO_CMD_SIG) != 0)
    {
      (void) rex_clr_sigs(TDSO_TCB_PTR, TDSO_CMD_SIG);

      /* ------------------------
      ** TDSO Pre-Synchronization
      ** ------------------------ */
      tdso_pre_sync_proc(tdso_pre_sync_ptr->time, tdso_pre_sync_ptr->direction);
    }
  } /* end for (;;) */

} /* tdso_task */

/*===========================================================================

FUNCTION : TDSO_INITIALIZER

DESCRIPTION:
  This function will initialize the TDSO On a per call basis.
  This initializer function is registered with SNM and will be
  called when the service option has been initialized.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void tdso_initializer
(
  const caii_srv_cfg_type* cfg, /* Service configuration info */
  word connect_info /*lint -esym(715,connect_info) Connection number not used*/
)
{
  // Initialize SCH callbacks to no callback
  mccsch_sch_callback_type sch_cb = { 0, 0, 0, 0 };
  /* -----------------------------------
  ** Set the Current TDSO service option
  ** ----------------------------------- */
    tdso_so = cfg->con[connect_info].so;
  M1X_MSG( DCP, LEGACY_MED,
    "SNM called TDSO initializer for SO %d ",
    tdso_so);

  /* ----------------------------------------------------------------------
  ** Register a call back with MCCSCH module to get notified of any changes
  ** in the SCH configuration as a result of processing ESCAM.
  ** Other callbacks are not used.
  ** ---------------------------------------------------------------------- */
  sch_cb.sch_config_change_cb = tdso_sch_chng_cback;
  mccsch_reg_sch_callback( &sch_cb );

  /* -----------------------------------------------
  ** Set the Radio Configuration information for FCH
  ** ----------------------------------------------- */
  if (cfg->fch_cc_incl)
  {
    fch_parm.pn_info[FWD_LINK].rc = cfg->for_fch_rc;
    fch_parm.pn_info[REV_LINK].rc = cfg->rev_fch_rc;
    M1X_MSG( DCP, LEGACY_MED,
      "TDSO Connect: F_FCH_RC %d, R_FCH_RC %d",
      fch_parm.pn_info[FWD_LINK].rc,
      fch_parm.pn_info[REV_LINK].rc);
  }

  /* ------------------------------------------------
  ** Set the Radio Configuration information for DCCH
  ** ------------------------------------------------ */
  if (cfg->dcch_cc_incl)
  {
    dcch_parm.pn_info[FWD_LINK].rc = cfg->for_dcch_rc;
    dcch_parm.pn_info[REV_LINK].rc = cfg->rev_dcch_rc;
    M1X_MSG( DCP, LEGACY_MED,
      "TDSO Connect: F_DCCH_RC %d, R_DCCH_RC %d",
      dcch_parm.pn_info[FWD_LINK].rc,
      dcch_parm.pn_info[REV_LINK].rc);
  }

  /* -----------------------------------
  ** Set the Radio Configuration for SCH
  ** ----------------------------------- */
  if (cfg->for_sch_cc_incl && (cfg->num_for_sch != 0))
  {
    sch0_parm.pn_info[FWD_LINK].rc = cfg->for_sch[0].sch_chn_cfg.sch_rc;
    M1X_MSG( DCP, LEGACY_MED,
      "FOR_SCH_RC = %d",
      sch0_parm.pn_info[FWD_LINK].rc );
  }
  if(cfg->rev_sch_cc_incl && (cfg->num_rev_sch != 0))
  {
    sch0_parm.pn_info[REV_LINK].rc = cfg->rev_sch[0].sch_chn_cfg.sch_rc;
    M1X_MSG( DCP, LEGACY_MED,
      "REV_SCH_RC = %d",
      sch0_parm.pn_info[REV_LINK].rc );
  }

  /* --------------------------------------------------
  ** For Simple TDSO, the TX and RX are always in SYNC.
  ** -------------------------------------------------- */
  if (tdso_so == CAI_SO_TDSO )
  {
    fch_parm.pn_info[FWD_LINK].tdso_sync = TRUE;
    fch_parm.pn_info[REV_LINK].tdso_sync = TRUE;

    dcch_parm.pn_info[FWD_LINK].tdso_sync = TRUE;
    dcch_parm.pn_info[REV_LINK].tdso_sync = TRUE;

    sch0_parm.pn_info[FWD_LINK].tdso_sync = TRUE;
    sch0_parm.pn_info[REV_LINK].tdso_sync = TRUE;
  }
  else
  {
    fch_parm.pn_info[FWD_LINK].tdso_sync = FALSE;
    fch_parm.pn_info[REV_LINK].tdso_sync = FALSE;

    dcch_parm.pn_info[FWD_LINK].tdso_sync = FALSE;
    dcch_parm.pn_info[REV_LINK].tdso_sync = FALSE;

    sch0_parm.pn_info[FWD_LINK].tdso_sync = FALSE;
    sch0_parm.pn_info[REV_LINK].tdso_sync = FALSE;
  }


  /* ---------------------------------------------
  ** Initialize FCH, DCCH and SCH0 test parameters
  ** --------------------------------------------- */
  memset(&fch_parm.pn_info[FWD_LINK].tst_par, 0, sizeof(tdso_tst_par_type));
  memset(&fch_parm.pn_info[REV_LINK].tst_par, 0, sizeof(tdso_tst_par_type));

  memset(&dcch_parm.pn_info[FWD_LINK].tst_par, 0, sizeof(tdso_tst_par_type));
  memset(&dcch_parm.pn_info[REV_LINK].tst_par, 0, sizeof(tdso_tst_par_type));

  memset(&sch0_parm.pn_info[FWD_LINK].tst_par, 0, sizeof(tdso_tst_par_type));
  memset(&sch0_parm.pn_info[REV_LINK].tst_par, 0, sizeof(tdso_tst_par_type));

  /* ----------------------------------------------------------------------
  ** Initialize the TX and RX counters (both instaneous and buffer storage)
  ** for FCH, DCCH and SCH0 to '0's
  ** ---------------------------------------------------------------------- */
  memset(fch_tx_table, 0, sizeof(fch_tx_table));
  memset(fch_tx_blank_table, 0, sizeof(fch_tx_blank_table));
  memset(fch_tx_rpt, 0, sizeof(fch_tx_rpt));
  memset(fch_tx_blank_rpt, 0, sizeof(fch_tx_blank_rpt));

  memset(dcch_tx_table, 0, sizeof(dcch_tx_table));
  memset(dcch_tx_blank_table, 0, sizeof(dcch_tx_blank_table));
  memset(dcch_tx_rpt, 0, sizeof(dcch_tx_rpt));
  memset(dcch_tx_blank_rpt, 0, sizeof(dcch_tx_blank_rpt));

  memset(sch_tx_table, 0, sizeof(sch_tx_table));
  sch_tx_blank = 0;
  memset(sch_tx_rpt, 0, sizeof(sch_tx_rpt));
  sch_tx_blank_rpt = 0;

  memset(fch_rx_table, 0, sizeof(fch_rx_table));
  memset(fch_rx_null_table, 0, sizeof(fch_rx_null_table));
  memset(fch_rx_rpt, 0, sizeof(fch_rx_rpt));
  memset(fch_rx_null_rpt, 0, sizeof(fch_rx_null_rpt));

  memset(dcch_rx_table, 0, sizeof(dcch_rx_table));
  memset(dcch_rx_null_table, 0, sizeof(dcch_rx_null_table));
  memset(dcch_rx_rpt, 0, sizeof(dcch_rx_rpt));
  memset(dcch_rx_null_rpt, 0, sizeof(dcch_rx_null_rpt));

  memset(sch_rx_table, 0, sizeof(sch_rx_table));
  memset(sch_rx_blank_table, 0, sizeof(sch_rx_blank_table));
  memset(sch_rx_rpt, 0, sizeof(sch_rx_rpt));
  memset(sch_rx_blank_rpt, 0, sizeof(sch_rx_blank_rpt));

  memset(sch_rx_pdu_table, 0, sizeof(sch_rx_pdu_table));
  memset(sch_rx_pdu_buffer, 0, sizeof(sch_rx_pdu_buffer));

  /* ------------------------------------------------------------------
  ** Default values for TX and RX test parameters of FCH, DCCH and SCH0
  ** ------------------------------------------------------------------ */

  /* Set data source to selectable data pattern */
  fch_parm.pn_info[FWD_LINK].data_src = 0;
  fch_parm.pn_info[REV_LINK].data_src = 0;

  dcch_parm.pn_info[FWD_LINK].data_src = 0;
  dcch_parm.pn_info[REV_LINK].data_src = 0;

  sch0_parm.pn_info[FWD_LINK].data_src = 0;
  sch0_parm.pn_info[REV_LINK].data_src = 0;

  /* Set frame activity to deterministic */
  fch_parm.pn_info[FWD_LINK].frame_act = 0;
  fch_parm.pn_info[REV_LINK].frame_act = 0;

  dcch_parm.pn_info[FWD_LINK].frame_act = 0;
  dcch_parm.pn_info[REV_LINK].frame_act = 0;

  sch0_parm.pn_info[FWD_LINK].frame_act = 0;
  sch0_parm.pn_info[REV_LINK].frame_act = 0;

  /* Set transmission ON/OFF period/count to 0 */
  fch_parm.pn_info[FWD_LINK].tx_on_period = 0;
  fch_parm.pn_info[FWD_LINK].tx_on_period_cnt = 0;
  fch_parm.pn_info[FWD_LINK].tx_off_period = 0;
  fch_parm.pn_info[FWD_LINK].tx_off_period_cnt = 0;
  fch_parm.pn_info[REV_LINK].tx_on_period = 0;
  fch_parm.pn_info[REV_LINK].tx_on_period_cnt = 0;
  fch_parm.pn_info[REV_LINK].tx_off_period = 0;
  fch_parm.pn_info[REV_LINK].tx_off_period_cnt = 0;

  dcch_parm.pn_info[FWD_LINK].tx_on_period = 0;
  dcch_parm.pn_info[FWD_LINK].tx_on_period_cnt = 0;
  dcch_parm.pn_info[FWD_LINK].tx_off_period = 0;
  dcch_parm.pn_info[FWD_LINK].tx_off_period_cnt = 0;
  dcch_parm.pn_info[REV_LINK].tx_on_period = 0;
  dcch_parm.pn_info[REV_LINK].tx_on_period_cnt = 0;
  dcch_parm.pn_info[REV_LINK].tx_off_period = 0;
  dcch_parm.pn_info[REV_LINK].tx_off_period_cnt = 0;

  sch0_parm.pn_info[FWD_LINK].tx_on_period = 0;
  sch0_parm.pn_info[FWD_LINK].tx_on_period_cnt = 0;
  sch0_parm.pn_info[FWD_LINK].tx_off_period = 0;
  sch0_parm.pn_info[FWD_LINK].tx_off_period_cnt = 0;
  sch0_parm.pn_info[REV_LINK].tx_on_period = 0;
  sch0_parm.pn_info[REV_LINK].tx_on_period_cnt = 0;
  sch0_parm.pn_info[REV_LINK].tx_off_period = 0;
  sch0_parm.pn_info[REV_LINK].tx_off_period_cnt = 0;

  /* Set circular buffer pointers */
  fch_parm.pn_info[FWD_LINK].circ_buffer = fch_rx_circ_buffer;
  fch_parm.pn_info[REV_LINK].circ_buffer = fch_tx_circ_buffer;

  dcch_parm.pn_info[FWD_LINK].circ_buffer = dcch_rx_circ_buffer;
  dcch_parm.pn_info[REV_LINK].circ_buffer = dcch_tx_circ_buffer;

  sch0_parm.pn_info[FWD_LINK].circ_buffer = sch_rx_circ_buffer;
  sch0_parm.pn_info[REV_LINK].circ_buffer = sch_tx_circ_buffer;
  sch0_parm.pn_info[FWD_LINK].pre_sync_circ_buffer =
    tdso_pre_sync_ptr->sch_rx_circ_buffer;
  sch0_parm.pn_info[REV_LINK].pre_sync_circ_buffer =
    tdso_pre_sync_ptr->sch_tx_circ_buffer;

  /* Set the TDSO state to ON */
  fch_parm.pn_info[FWD_LINK].tdso_state = 1;
  fch_parm.pn_info[REV_LINK].tdso_state = 1;

  dcch_parm.pn_info[FWD_LINK].tdso_state = 1;
  dcch_parm.pn_info[REV_LINK].tdso_state = 1;

  sch0_parm.pn_info[FWD_LINK].tdso_state = 1;
  sch0_parm.pn_info[REV_LINK].tdso_state = 1;

  /* Set the data pattern to be 0xff */
  fch_parm.pn_info[FWD_LINK].data_pattern = 0xff;
  fch_parm.pn_info[REV_LINK].data_pattern = 0xff;

  fch_parm.pn_info[FWD_LINK].tst_par.data_pattern = 0xff;
  fch_parm.pn_info[REV_LINK].tst_par.data_pattern = 0xff;

  dcch_parm.pn_info[FWD_LINK].data_pattern = 0xff;
  dcch_parm.pn_info[REV_LINK].data_pattern = 0xff;

  dcch_parm.pn_info[FWD_LINK].tst_par.data_pattern = 0xff;
  dcch_parm.pn_info[REV_LINK].tst_par.data_pattern = 0xff;

  sch0_parm.pn_info[FWD_LINK].data_pattern  = 0xff;
  sch0_parm.pn_info[REV_LINK].data_pattern  = 0xff;

  sch0_parm.pn_info[FWD_LINK].tst_par.data_pattern  = 0xff;
  sch0_parm.pn_info[REV_LINK].tst_par.data_pattern  = 0xff;

  /* Set PN value to 0 */
  fch_parm.pn_info[FWD_LINK].pn_valid = 0;
  fch_parm.pn_info[REV_LINK].pn_valid = 0;

  dcch_parm.pn_info[FWD_LINK].pn_valid = 0;
  dcch_parm.pn_info[REV_LINK].pn_valid = 0;

  sch0_parm.pn_info[FWD_LINK].pn_valid = 0;
  sch0_parm.pn_info[REV_LINK].pn_valid = 0;

  /* Initialize SOCM processing */
  socm_deferred = FALSE;
  socm_ignored = FALSE;

  /* Initialize TDSO SCH info */
  tdso_sch.sch_is_active = FALSE;
  tdso_sch.sch_tx_rate = TDSO_SCH_RATE_NONE;
  tdso_sch.sch_rx_rate = TDSO_SCH_RATE_NONE;

  /* Mark TDSO call up */
  tdso_call_on = TRUE;

} /* tdso_initializer */

/*===========================================================================

FUNCTION : TDSO_INIT_SO_CFG

DESCRIPTION
  Registers the TDSO service option with SNM.

 DEPENDENCIES
  An instance of snm_config_type (default configuration) and snm_neg_par_type
  (negotiation parameters) should have been defined for the TDSO service option
  being registered, and should have been initialized with meaningful values
  prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdso_init_so_cfg(void)
{
  #if defined FEATURE_IS2000_REL_A
  /* Adjust the mux_option before registering the TDSO with SNM */
  if (mccap_item_is_supported(MCCAP_DF_MUXPDU5))
  {
    stdso_neg_par.fwd_sch_mo_list.list[stdso_neg_par.fwd_sch_mo_list.num_mos]
        = CAI_MUX_VARIABLE;
    stdso_neg_par.fwd_sch_mo_list.num_mos ++;
    stdso_neg_par.rev_sch_mo_list.list[stdso_neg_par.rev_sch_mo_list.num_mos]
        = CAI_MUX_VARIABLE;
    stdso_neg_par.rev_sch_mo_list.num_mos ++;

    ftdso_neg_par.fwd_sch_mo_list.list[ftdso_neg_par.fwd_sch_mo_list.num_mos]
        = CAI_MUX_VARIABLE;
    ftdso_neg_par.fwd_sch_mo_list.num_mos ++;
    ftdso_neg_par.rev_sch_mo_list.list[ftdso_neg_par.rev_sch_mo_list.num_mos]
        = CAI_MUX_VARIABLE;
    ftdso_neg_par.rev_sch_mo_list.num_mos ++;
  }
  #endif /* FEATURE_IS2000_REL_A */

  if( snm_add_so( CAI_SO_TDSO, (snm_neg_par_type*)&stdso_neg_par,
      &tdso_default_cfg ) == SNM_FAILURE )
  {
    ERR_FATAL( "Registering SO %x Failed",  CAI_SO_TDSO, 0, 0);
  }

  if( snm_add_so( CAI_SO_FULL_TDSO, (snm_neg_par_type*)&ftdso_neg_par,
      &tdso_default_cfg ) == SNM_FAILURE )
  {
    ERR_FATAL( "Registering SO %x Failed",  CAI_SO_FULL_TDSO, 0, 0);
  }

  /* -----------------------------------------------------------------------
  ** Tell the service negotiation module what SO to respond with, when
  ** paged with these SOs. We respond with the Same SO, since this is test
  ** service options, we don't need to negotiate the SO.

  ** snm_set_page_response(  CAI_SO_TDSO,<respond with> );
  ** ----------------------------------------------------------------------*/

  /* Call the function to register the connection initilizer for TDSO */
  if (snm_reg_connection_initializer( CAI_SO_TDSO, tdso_initializer))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Registering SO Initializer %x Failed",
      CAI_SO_TDSO);
  }

  if (snm_reg_connection_initializer( CAI_SO_FULL_TDSO, tdso_initializer))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Registering SO Initializer %x Failed",
      CAI_SO_FULL_TDSO);
  }
} /* tdso_init_so_cfg */

/*===========================================================================

FUNCTION : TDSO_CHECK_SYNC_FRAME

DESCRIPTION:
  This function checks whether the specified frame is a SYNC frame for full
  TDSO.

DEPENDENCIES:
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean tdso_check_sync_frame
(
  dword              frame_time,
  tdso_link_dir_type direction
)
{
  dword fr_time;
  boolean status = FALSE;

  qword public_lcm;
  /* The public long code mask is used to determine if this is a sync frame
     (even if the mobile is currently using the private long code mask).
     Calling get_public_long_code_mask directly (instead of get_long_code_mask)
     avoids having messages printed each frame, which floods QXDM. */
  mcctcsup_get_public_long_code_mask(public_lcm);

  if (tdso_so == CAI_SO_FULL_TDSO)
  {
    /* Set the Frame time variable based on the Direction */
    fr_time = (direction == FWD_LINK) ?  (frame_time ^ DEF_RX_SEED) :
              (frame_time ^ DEF_TX_SEED);

    /* SYNC frame */
    if ((fr_time & 0x1FF) == (qw_lo(public_lcm) & 0x1FF))
    {
      status = TRUE;
    }
  }

  return status;

} /* tdso_check_sync_frame */

/*===========================================================================

FUNCTION :   TDSO_BUILD_TX_FRAME

DESCRIPTION

  This function is used by the Multiplex layer to fetch the FCH or SCH
  Channel transmission TDSO frame buffer.  For simple TDSO the buffer is padded
  with all '1's. For Full TDSO, the TDSO frame is packed with data pattern
  selected by the Control derivatives.

DEPENDENCIES
  This functin depends on the MUX layer to properly pass the correct number
  of PDUs based on the MUX rate of the SCH/FCH and also PDU type.

RETURN VALUE

  This function returns the FCH/SCH frame based on the frame type requested.
  During the TDSO OFF state, this function returns a BLANK frame instead of
  a requested TDSO frame type.

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_build_tx_frames
(
  tdso_link_dir_type    direction,     /* Forward or Reverse link */
  tdso_frame_block_type *frame_blk_ptr /* Pointer to frames block */
)
{
    tdso_tx_frame_type *tx_frame_ptr = NULL;     /* Pointer to TX frame */
    tdso_buffer_info   *chan_pn_info_ptr = NULL; /* Pointer to channel PN info */
    uint16             max_frame_size = 0;       /* Number of bits to pack */
    boolean            sch_thresh = FALSE;
    byte               fr_num;
    static word        log_count = 0;            /* Count for logging frames */
    tdso_status_type   status = TDSO_VALID;
    uint32 threshValue;

    REX_ISR_LOCK(&mc_crit_sect);

    log_count++; /* Increment Log Count */

    /* -------------------------------------------------------------------
    ** Check if the circular frame generation needs to start at this frame
    ** ------------------------------------------------------------------- */
    if (tdso_check_sync_frame(frame_blk_ptr->fr_time + PRE_SYNC_COUNT,
        direction))
    {
      /* Set the pre-sync procedure direction and kick the TDSO task */
      M1X_MSG( DCP, LEGACY_MED,
        "Pre TDSO sync point, Building Circular Buff");
      tdso_pre_sync_ptr->direction = direction;
      tdso_pre_sync_ptr->time = frame_blk_ptr->fr_time + PRE_SYNC_COUNT;
      (void) rex_set_sigs(TDSO_TCB_PTR, TDSO_CMD_SIG);
    }

    /* -----------------------------------------------------------------------
    ** Check if update of test parameters from SOCM has to deferred because it
    ** is received in the period of of 1sec before SYNC.
    ** ---------------------------------------------------------------------- */
    if (tdso_check_sync_frame(frame_blk_ptr->fr_time + 50, direction) &&
        (direction == FWD_LINK))
    {
      /* This frame is 1sec before the SYNC frame, se socm_valid to FALSE */
      M1X_MSG( DCP, LEGACY_MED,
        "1sec before SYNC: Defer SOCM processing");
      socm_deferred = TRUE;
    }

    /* ------------------------------------------------------------------
    ** Check if this frame is the actual SYNC frame.  Do SYNC procedures.
    ** ------------------------------------------------------------------ */
    if (tdso_check_sync_frame(frame_blk_ptr->fr_time, direction))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TDSO sync point, SYNC procedures ");
      tdso_sync_proc(direction, frame_blk_ptr);

      /*------------------------------------------------------------------------
      ** Check if the update of TDSO test parameters is ignored because SOCM is
      ** received in the 1sec boundary of a SYNC frame. If so, update TDSO test
      ** parameters now which will take effect from next SYNC frame
      ** -----------------------------------------------------------------------*/
      if ((socm_ignored) && (direction == FWD_LINK))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Process deferred SOCM, Effects next SYNC" );
        socm_ignored = FALSE;
        tdso_sync_test_parameter(FWD_LINK);
        tdso_sync_test_parameter(REV_LINK);
      }
    }

    /* Iterate through and generate PDU blocks for Transmission */
    for (fr_num =0; fr_num < frame_blk_ptr->num_frames; fr_num++)
    {
      /*------------------------------------------------------------------
       Get a pointer to the specific frame that is going to be build.
      -------------------------------------------------------------------*/
      tx_frame_ptr = &(frame_blk_ptr->frame_type.rev_arr[fr_num]);

      /* Make sure the Physical channel is supported before building frames */
      if ((tx_frame_ptr->fr_channel != TDSO_FCH)
          && (tx_frame_ptr->fr_channel != TDSO_DCCH)
          && (tx_frame_ptr->fr_channel != TDSO_SCH0)
         )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Phy %d Dir %d fr num %d",
          tx_frame_ptr->fr_channel,
          direction,
          fr_num);
        status = TDSO_INVALID_CHAN;
        break;
      }

      /* Set channel pn info pointer */
      if (tx_frame_ptr->fr_channel == TDSO_FCH)
      {
        chan_pn_info_ptr = &fch_parm.pn_info[direction];
      }
      else if (tx_frame_ptr->fr_channel == TDSO_DCCH)
      {
        chan_pn_info_ptr = &dcch_parm.pn_info[direction];
      }
      else
      {
        chan_pn_info_ptr = &sch0_parm.pn_info[direction];
      }

      max_frame_size = tx_frame_ptr->max_frame_size;

      if (tdso_so == CAI_SO_TDSO)
      {
        /* --------------------------------------------------------------------
        ** Round up the maximum number of bits to the nearest octet. For Simple
        ** TDSO, the bits in the last octet (if not a full octed) is filled
        ** with ones.  Whereas in Full TDSO, the bits in the last octet are
        ** packed with zeros.
        ** ------------------------------------------------------------------*/
         max_frame_size += (8 - tx_frame_ptr->max_frame_size%8);
      }

      /* ----------------------------------------------------------------------
      ** If the Frame activity is random, compute the threshold value using
      ** the PN value and then compare it with the given ON to OFF propability
      ** and OFF to ON probability values based on the current state.
      ** ---------------------------------------------------------------------*/
      if (chan_pn_info_ptr->frame_act)
      {
        if ((tx_frame_ptr->fr_channel == TDSO_FCH)
            || (tx_frame_ptr->fr_channel == TDSO_DCCH)
            || ((tx_frame_ptr->fr_channel == TDSO_SCH0) && (!sch_thresh))
           )
        {
          threshValue = chan_pn_info_ptr->pn >> 8;

          switch (chan_pn_info_ptr->tdso_state)
          {
            case FALSE:
              if (threshValue < chan_pn_info_ptr->tx_on_thresh)
              {
                chan_pn_info_ptr->tdso_state = TRUE;
              }
              break;

            case TRUE:
              if (threshValue < chan_pn_info_ptr->tx_off_thresh)
              {
                chan_pn_info_ptr->tdso_state = FALSE;
              }
              break;
          /*lint -e744*/
          } /* end switch */
          /*lint +e744*/

          if (tx_frame_ptr->fr_channel == TDSO_SCH0)
          {
            sch_thresh = TRUE; /* This will avoid changing states per SCH pdu */
          }
        }
      } /* end if (chan_pn_info_ptr->frame_act) */

      /* ------------------------------------------------------------------
      ** Generate the Full Rate TDSO Frame from the Circular Buffer,
      ** -----------------------------------------------------------------*/
      if ((chan_pn_info_ptr->data_src) && (chan_pn_info_ptr->tdso_sync))
      {
        /* Pack the FCH/DCCH TDSO data block From the Circular buffer */
        tdso_pack_datablk_from_circ_buffer(tx_frame_ptr, direction);
      }
      else
      {
        /* ----------------------------------------------------------------
        ** Pack data bytes with Selectable byte pattern, If TDSO is not
        ** synchronized or if the selectable byte pattern is enabled.
        ** ----------------------------------------------------------------*/
        memset(tx_frame_ptr->fr_item_ptr, chan_pn_info_ptr->data_pattern,
               max_frame_size/8 );
      }

      /* -----------------------------------------------------------------
      ** For FTDSO Mask 5 bits of the data block with CH_ID and PDU hearder.
      ** ---------------------------------------------------------------*/
      if (tdso_so == CAI_SO_FULL_TDSO)
      {
        if (tx_frame_ptr->fr_channel == TDSO_FCH)
        {
          /* For FCH, the Channel_ID = 00 and PDU sequence number = 000 */
          tx_frame_ptr->fr_item_ptr[0] &= 0x07;
        }
        else if (tx_frame_ptr->fr_channel == TDSO_DCCH)
        {
          /* For FCH, the Channel_ID = 01 and PDU sequence number = 000 */
          tx_frame_ptr->fr_item_ptr[0] &= 0x07;

          /* NNMMMXXX ; NN idicates channel id, MMM indicates PDU seq num */
          tx_frame_ptr->fr_item_ptr[0] |= 0x40;
        }
        else if (tx_frame_ptr->fr_channel == TDSO_SCH0)
        {
          /* For SCH0, the Channel_ID = 0x10 and PDU sequence number from 0x0 */
          tx_frame_ptr->fr_item_ptr[0] &= 0x07;
          if (direction == FWD_LINK)
          {
            tx_frame_ptr->fr_item_ptr[0] |= (0x80 | (uint8) ((fr_num -1)*8));
          }
          else
          {
            tx_frame_ptr->fr_item_ptr[0] |= (0x80 | (uint8) ((fr_num)*8));
          }
        }
        /* Set the Last Bits (incomplete last octet) of the data block to 0 */
        tx_frame_ptr->fr_item_ptr[max_frame_size/8 ] = 0x0;
      } /* end if (tdso_so == CAI_SO_FULL_TDSO) */

      /* For FCH, update the PN offset information after every PDU */

      if (((tx_frame_ptr->fr_channel == TDSO_FCH)

           || (tx_frame_ptr->fr_channel == TDSO_DCCH)
          ) && (chan_pn_info_ptr->tdso_sync)
         )
      {
        /* Update seed value and data offset value */
        tdso_update_pn_offset_info(chan_pn_info_ptr);
      } /* end if (tdso_so == CAI_SO_FULL_TDSO) */

      /* Check if TDSO state is OFF */
      if ((!chan_pn_info_ptr->tdso_state) && (direction == REV_LINK))
      {
        /* TDSO state is OFF, set the data block as BLANK and clear data */
        tx_frame_ptr->fr_rate = TDSO_BLANK;
        frame_blk_ptr->num_frames = 0; /* set number of data blocks to zero */
      }

      /* Update the FCH transmission counters */
      if ((tx_frame_ptr->fr_channel == TDSO_FCH) && (direction == REV_LINK))
      {
        tdso_update_fch_tx_stats(tx_frame_ptr, chan_pn_info_ptr->tdso_state);

        /* Update the TDSO state */
        tdso_update_on_off_state(TDSO_FCH, direction);
      }

      /* Update the DCCH transmission counters */
      if ((tx_frame_ptr->fr_channel == TDSO_DCCH) && (direction == REV_LINK))
      {
        tdso_update_dcch_tx_stats(tx_frame_ptr,
          chan_pn_info_ptr->tdso_state);

        /* Update the TDSO state */
        tdso_update_on_off_state(TDSO_DCCH, direction);

      }

    } /* end for */

    /* -----------------------------------------------------------------
    ** Looped through all the PDUs, Update the SCH statistics and change
    ** state after all the PDUs are processed (This is one SCH frame).
    ** ---------------------------------------------------------------*/
    /* Last PDU in the SCH frame */
    if ( (status == TDSO_VALID) && (tx_frame_ptr != NULL) && (tx_frame_ptr->fr_channel == TDSO_SCH0))
    {
      chan_pn_info_ptr = &sch0_parm.pn_info[direction];

      if (direction == REV_LINK)
      {
        tdso_update_sch_tx_stats(tx_frame_ptr, chan_pn_info_ptr->tdso_state);

        /* Update the TDSO state */
        tdso_update_on_off_state(TDSO_SCH0, direction);
      }

      /* Update seed value and data offset value */
      if (chan_pn_info_ptr->tdso_sync)
      {
        tdso_update_pn_offset_info(chan_pn_info_ptr);
      }
    }

    if ( (status == TDSO_VALID) && (log_count == 200) ) /* Logging the Statistics every 200 data blocks */
    {
      tdso_log_stats();
      log_count = 0;
    }

    REX_ISR_UNLOCK(&mc_crit_sect);

    return status;

} /* tdso_build_tx_frame */

/*===========================================================================

FUNCTION : tdso_log_stats

DESCRIPTION:
  This function calls either the log_stdso_stats() function or the
  ftdso_log_stats() function based on the type of Service option.
  Also this is called by MC task during the call tear down. This
  provides an abstration to the type of SO in action and the logging
  dependencies.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/
void tdso_log_stats(void)
{
  if(tdso_so == CAI_SO_TDSO)
  {
    /* Log STDSO statistics on to the QXDM */
    tdso_log_stdso_stats();
  }
  else
  {
    /* Log FTDSO statistics on to the QXDM */
    tdso_log_ftdso_stats();
  }
} /* tdso_log_stats */

/*===========================================================================

FUNCTION : TDSO_PROCESS_RX_FRAMES


DESCRIPTION
  This function processes the TDSO frame(s) received over the air. This is the
  interface between TDSO receive and the multiplex sublayer. The multiplex
  sublayer can send all the frames it received in one frame time to TDSO or
  split it up into multiple calls of this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_process_rx_frames
(
  tdso_frame_block_type *frame_blk_ptr /* Pointer to rxed frame block */
)
{
  tdso_rx_frame_type *rx_frame_ptr;
  tdso_tx_frame_type *tx_frame_ptr;
  tdso_status_type   status = TDSO_VALID;
  word i;

  fdch_tdso_rate_set  = frame_blk_ptr->fdch_fr_mux;  /* set the Mux rate */

  if (frame_blk_ptr->num_frames > 0) /* Loop until all PDUs */
  {
    /* Get the Fundamental frame and process it */
    rx_frame_ptr = &(frame_blk_ptr->frame_type.fwd_arr[0]);

    if (
#ifdef FEATURE_IS2000_REL_A_SVD
        /*--------------------------------------------------------------------
        ** Return INVALID status if the frame is not PRIMARY or SECONDARY
        ** or if it is received on un-supported TDSO channel.
        --------------------------------------------------------------------*/
         (rx_frame_ptr->srvc_type  != TDSO_PRIMARY_SRVC) &&
         (rx_frame_ptr->srvc_type  != TDSO_SECONDARY_SRVC)
#else
         /*--------------------------------------------------------------------
         ** Return INVALID status if the frame is not PRIMARY or if it received
         ** on un-supported TDSO channel.
         --------------------------------------------------------------------*/
        (rx_frame_ptr->srvc_type  != TDSO_PRIMARY_SRVC)
#endif /*  FEATURE_IS2000_REL_A_SVD */
       )

    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Service %d",
        rx_frame_ptr->srvc_type);
      return TDSO_INVALID_SRVC;
    }




    if ((rx_frame_ptr->fr_channel != TDSO_FCH)
         && (rx_frame_ptr->fr_channel != TDSO_DCCH)
       )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Channel TDSO %d",
        rx_frame_ptr->fr_channel);
      return TDSO_INVALID_SRVC;
    }



#ifdef FEATURE_IS2000_REL_A_SVD
    tdso_secondary_len = 0;

    if (rx_frame_ptr->srvc_type == TDSO_SECONDARY_SRVC)
    {
      if (frame_blk_ptr->sec_len == TDSO_NON_FULL_FRAME_ON_SEC)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Non-Full Frames not supported.");
        return TDSO_INVALID_SRVC;
      }
      else if (frame_blk_ptr->sec_len == 0)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Zero length frame received on Secondary!");
        return TDSO_INVALID_SRVC;
      }
      else
      {
        tdso_secondary_len = frame_blk_ptr->sec_len;
      }
    }
#endif /*  FEATURE_IS2000_REL_A_SVD */

    if (!tdso_sch.sch_is_active)
    {
      frame_blk_ptr->num_frames = 1;
    }

    /* ---------------------------------------------------------------------
    ** Set the REVERSE Link PDUs in the Block same as the FORWARD link PDUs
    ** and call the tdso_build_tx_frames().  This function generated the
    ** expected PDU data blocks and packs in the REVERSE link. Note: Only
    ** the Channel parameters are set but not the data block contents.
    ** -------------------------------------------------------------------*/
    for (i=0; i<frame_blk_ptr->num_frames; i++)
    {
      rx_frame_ptr = &frame_blk_ptr->frame_type.fwd_arr[i];
      tx_frame_ptr = &frame_blk_ptr->frame_type.rev_arr[i];

      tx_frame_ptr->valid_frame = rx_frame_ptr->valid_frame;
      tx_frame_ptr->fr_channel= rx_frame_ptr->fr_channel;
      tx_frame_ptr->srvc_type = rx_frame_ptr->srvc_type;
      tx_frame_ptr->mux_pdu_type = rx_frame_ptr->mux_pdu_type;
      tx_frame_ptr->fr_rate = rx_frame_ptr->fr_rate;
      tx_frame_ptr->sch_rate = rx_frame_ptr->sch_rate;

      if (rx_frame_ptr->fr_rate >= TDSO_MAX_RATE)
      {
        /* Something horrible happened */
        ERR_FATAL("Rate out of range %d", (int) rx_frame_ptr->fr_rate, 0, 0);
      }

      /* check the valid range of forward rate set value */
      if (frame_blk_ptr->fdch_fr_mux >= TDSO_MAX_RATE_SET)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "forward rate out of range %d",
          frame_blk_ptr->fdch_fr_mux);
        return TDSO_INVALID_SRVC;
      }

      /* check the valid range of forward rate set value */
      if (frame_blk_ptr->sup_fr_mux >= TDSO_MAX_RATE_SET)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid TDSO rate set= %d",
          frame_blk_ptr->sup_fr_mux);
        return TDSO_INVALID_SRVC;
      }


      if (i == 0)
      {
#ifdef FEATURE_IS2000_REL_A_SVD
        if (rx_frame_ptr->srvc_type == TDSO_SECONDARY_SRVC)
        {
          tx_frame_ptr->max_frame_size = (word) tdso_secondary_len;
        }
        else  /* primary */
        {
#endif /* FEATURE_IS2000_REL_A_SVD */
          tx_frame_ptr->max_frame_size =
              tdso_data_len[rx_frame_ptr->fr_rate][frame_blk_ptr->fdch_fr_mux];
#ifdef FEATURE_IS2000_REL_A_SVD
        }
#endif /* FEATURE_IS2000_REL_A_SVD */
      }
      else
      {
        /* SCH frame size */
#ifdef FEATURE_IS2000_REL_A
        if (rx_frame_ptr->mux_pdu_type == TDSO_MUX_PDU_5)
        {
          /*
          ** MuxPDU 5 payload is different from
          ** that of MuxPDU 3. Separate talbe is used.
          */
          if (rx_frame_ptr->sch_rate < TDSO_SUPPORTED_SCH_RATES) /*lint !e641 */
          {
            tx_frame_ptr->max_frame_size =
              tdso_muxpdu5_conv_data_len[rx_frame_ptr->sch_rate][frame_blk_ptr->sup_fr_mux];
          }
          else
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "Invalid SCH RATE");
            tx_frame_ptr->max_frame_size = 0;
          }
        }
        else
#endif /* FEATURE_IS2000_REL_A */
        {
          /* MuxPDU 1, 2, 3 uses old table */
          tx_frame_ptr->max_frame_size =
            tdso_data_len[rx_frame_ptr->fr_rate][frame_blk_ptr->sup_fr_mux];
        }
      }
    } /* End of FOR loop */

    /* Build the Circular buffers for the Forward Link */
    (void) tdso_build_tx_frames(FWD_LINK, frame_blk_ptr);

    if (frame_blk_ptr->frame_type.fwd_arr[0].fr_channel == TDSO_FCH)
    {
      /* Process the received FCH tdso frame */
      status = tdso_process_fch_rx_frame(frame_blk_ptr);
    }

    if (frame_blk_ptr->frame_type.fwd_arr[0].fr_channel == TDSO_DCCH)
    {
      /* Process the received FCH tdso frame */
      status = tdso_process_dcch_rx_frame(frame_blk_ptr);
    }

    /* Check if the TDSO block contains SCH frames */
    if((frame_blk_ptr->num_frames > 1) && (tdso_sch.sch_is_active))
    {
      /* process the received SCH tdso frame */
      status = tdso_process_sch_rx_frame(frame_blk_ptr);
    }
  }

  return status;

} /* tdso_process_rx_frames */

/*===========================================================================

FUNCTION : TDSO_PROCESS_FCH_RX_FRAME

DESCRIPTION
  This function processes the Received FCH TDSO frame(s).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_process_fch_rx_frame
(
    tdso_frame_block_type *tdso_rx_block  /* Ptr to rcvd TDSO frames    */
)
{
  tdso_status_type status = TDSO_VALID;  /* set default status value */
  word i;                         /* Used for counting... */
  byte err_mask;
  size_t copy_size;      /*To store the value returned by memscpy */
  word frame_rate =0;             /* tdso frame rate & mux rate*/
  tdso_rx_frame_type  *frame_ptr = NULL;
//  uint8 local_fch_data[22];        /* maximum RS1 data length in bytes */
uint8 local_fch_data[34];        /* maximum RS1 & RS2 data length in bytes */

  /* set the frame pointer and frame rate of the FCH frame */
  frame_ptr = &(tdso_rx_block->frame_type.fwd_arr[0]);
  frame_rate = frame_ptr->fr_rate;  /*lint !e641 */


  /*--------------------------------------------------------------------
  ** Check if the MUX_PDU_TYPE is valid.
  ** -----------------------------------------------------------------*/
  if (frame_ptr->mux_pdu_type > TDSO_MUX_PDU_2)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid FCH MUX_PDU_TYPE type = %d",
      frame_ptr->mux_pdu_type);
    return TDSO_INVALID_SRVC;
  }

  /*------------------------------------
  ** Check if the TDSO rate set is valid.
  ** ----------------------------------- */
  if (fdch_tdso_rate_set>= TDSO_MAX_RATE_SET)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid TDSO rate set= %d",
      fdch_tdso_rate_set);
    return TDSO_INVALID_SRVC;
  }

  /* ---------------------------------------------------------------------
  ** Validate the TDSO frame only if it is a Full Rate TDSO frame.  If a
  ** HALF, QUARTER, EIGHT rate is received, no need to validate the frame
  ** errors.  But the statistics of the DIM and BURST are updated
  ** --------------------------------------------------------------------*/
  if((fch_parm.pn_info[FWD_LINK].tdso_state) && (frame_rate == TDSO_RATE_1)) /*lint !e641 */
  {
#ifdef FEATURE_IS2000_REL_A_SVD
    if (tdso_secondary_len) //* if this value is set Service is on Secondary
    {
      /* Copy the generated expected frame into the local buffer */
       copy_size = memscpy(local_fch_data, sizeof(local_fch_data),
                           tdso_rx_block->frame_type.rev_arr[0].fr_item_ptr,
            (int)((tdso_secondary_len/8) - 1));
       if( copy_size != (int)((tdso_secondary_len/8) - 1) )
       {
         M1X_MSG( DCP, LEGACY_ERROR,
           "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
           copy_size,
           (int)((tdso_secondary_len/8)-1),
           sizeof(local_fch_data));
       }

      /* Compare the Received Frame with that of the locally generated one */
      for (i=0; i < (int)((tdso_secondary_len/8) - 1); i++)
      {
        err_mask = frame_ptr->fr_item_ptr[i] ^ local_fch_data[i];
        if (err_mask != 0)
        {
          frame_ptr->fr_rate = TDSO_BIT_ERROR;
        }
      }
    }
    else
    {
#endif /* FEATURE_IS2000_REL_A_SVD  */
      /* Copy the generated expected frame into the local buffer */
      copy_size = memscpy(local_fch_data, sizeof(local_fch_data),
                          tdso_rx_block->frame_type.rev_arr[0].fr_item_ptr,
             tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set]);
      if( copy_size != tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set] )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set],
          sizeof(local_fch_data));
      }

      /* Compare the Received Frame with that of the locally generated one */
      for (i=0; i < tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set]; i++)
      {
        err_mask = frame_ptr->fr_item_ptr[i] ^ local_fch_data[i];
        if (err_mask != 0)
        {
          frame_ptr->fr_rate = TDSO_BIT_ERROR;
        }
      }

      /*---------------------------------------------------------------------
      ** Check the validity of the Last byte in the frame. The Full TDSO has
      ** bits in the last bytes (after the last octet) padded with 0's
      ** -------------------------------------------------------------------*/
      if (tdso_so == CAI_SO_FULL_TDSO)
      {
        err_mask =  (frame_ptr->fr_item_ptr[i] &
                      tdso_last_byte[frame_rate][fdch_tdso_rate_set]);
      }
      else
      {
        /*----------------------------------------------------------------------
        ** Check the validity of the Last byte in the frame. The Simple TDSO has
        ** bits in the last bytes (after the last octet) padded with 1's
        ** -------------------------------------------------------------------*/
        err_mask = (uint8) ((frame_ptr->fr_item_ptr[i] &
                    tdso_last_byte[frame_rate][fdch_tdso_rate_set]) ^
                    tdso_last_byte[frame_rate][fdch_tdso_rate_set]);
      }
#ifdef FEATURE_IS2000_REL_A_SVD
    } /* for secondary processing */
#endif /* FEATURE_IS2000_REL_A_SVD */
  }

  /* Update the FCH RX statistics */
  tdso_update_fch_rx_stats(frame_ptr, fch_parm.pn_info[FWD_LINK].tdso_state);

  /* Update state of TDSO */
  tdso_update_on_off_state(TDSO_FCH, FWD_LINK);

  return status;

} /* tdso_process_fch_rx_frame */

#if defined FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION : TDSO_PROCESS_MUXPDU5_RX_TURBO_FRAME

DESCRIPTION
  This function processes the Received SCH when MO is 0xf20, and coding type
  is TURBO.

DEPENDENCIES
  None

RETURN VALUE
  tdso_status_type

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_process_muxpud5_rx_turbo_frame
(
  tdso_frame_block_type *frame_blk_ptr /* Pointer to rxed frame blocks */
)
{
  word i;
  tdso_rx_frame_type *rx_frame_ptr = NULL; /* Pointer to rxed frame */
  tdso_status_type status = TDSO_VALID;


  rx_frame_ptr = &(frame_blk_ptr->frame_type.fwd_arr[1]);

  if ((rx_frame_ptr->srvc_type  != TDSO_PRIMARY_SRVC) ||
      (rx_frame_ptr->fr_channel != TDSO_SCH0 ))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid SRVC or CHAN TDSO %d",
      rx_frame_ptr->srvc_type,
      rx_frame_ptr->fr_channel);
    return TDSO_INVALID_SRVC;
  }

  /*----------------------------------------------------------------------
  ** Check if the received frame RATE is valid for SCH.
  ----------------------------------------------------------------------*/
  if ((rx_frame_ptr->fr_rate == TDSO_RATE_HALF) ||
      (rx_frame_ptr->fr_rate == TDSO_RATE_QUARTER) ||
      (rx_frame_ptr->fr_rate == TDSO_RATE_EIGHTH))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid SCH TDSO Rate type = %d",
      rx_frame_ptr->fr_rate);
    return TDSO_INVALID_SRVC;
  }

  /*----------------------------------------------------------------------
  ** Check if the frame rate set is valid.
  ----------------------------------------------------------------------*/
  if (frame_blk_ptr->sup_fr_mux >= TDSO_MAX_RATE_SET)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid TDSO rate set= %d",
      frame_blk_ptr->sup_fr_mux);
    return TDSO_INVALID_SRVC;
  }

  /*
  ** Now compare each byte to see if it is FF. This implementation only
  ** works for sTDSO.
  */
  if (rx_frame_ptr->fr_rate == TDSO_RATE_3)
  {
    /* First set the 6 bit header to '1's, because we don't care about them */
    rx_frame_ptr->fr_item_ptr[0] |= 0xFC;

    for (i=0;
         i<tdso_muxpdu5_turbo_data_len[rx_frame_ptr->sch_rate][frame_blk_ptr->sup_fr_mux];
         i++)
    {
      /* Check array bound for fwd_arr. */
      if (1+i/MAX_PDU_LEN > TDSO_MAX_PDU-1)
      {
        break;
      }

      if (frame_blk_ptr->frame_type.fwd_arr[1+i/MAX_PDU_LEN].fr_item_ptr[i%MAX_PDU_LEN] != 0xFF)
      {
        rx_frame_ptr->fr_rate = TDSO_BIT_ERROR;
        break;
      }
    }

    /* Update PDU status */
    tdso_update_sch_pdu_stats(rx_frame_ptr);
  }

  /* Update the SCH frame statistics, after all PDUs are processed */
  tdso_update_sch_rx_stats(rx_frame_ptr,
    sch0_parm.pn_info[FWD_LINK].tdso_state);


  return status;

}
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION : TDSO_PROCESS_SCH_RX_FRAME

DESCRIPTION
  This function processes the Received SCH TDSO frame(s).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_process_sch_rx_frame
(
  tdso_frame_block_type *frame_blk_ptr /* Pointer to rxed frame blocks */
)
{
  word i,j;
  byte err_mask = 0;
  tdso_rx_frame_type *rx_frame_ptr = NULL; /* Pointer to rxed frame */
  size_t copy_size;
  /* Return value of memscpy function */
#ifdef FEATURE_IS2000_REL_A
  byte expected_data[95];               /* Maximum PDU length in SCH */
  word num_pdu = 0;             /* Number of LTUs for convolutional case */
#else
  byte expected_data[71];                  /* Expected SCH data */
#endif /* FEATURE_IS2000_REL_A */
  word byte_len;
  tdso_status_type status = TDSO_VALID;

  /*------------------------------------
  ** Check if the TDSO rate set is valid.
  ** ----------------------------------- */
  if (frame_blk_ptr->sup_fr_mux >= TDSO_MAX_RATE_SET)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Invalid TDSO rate set= %d",
      frame_blk_ptr->sup_fr_mux);
    return TDSO_INVALID_SRVC;
  }

#ifdef FEATURE_IS2000_REL_A
  /* MuxPDU 5 and Turbo needs special treatment */
  if (frame_blk_ptr->frame_type.fwd_arr[1].mux_pdu_type == TDSO_MUX_PDU_5 &&
      frame_blk_ptr->frame_type.fwd_arr[1].sch_coding == TDSO_CODING_TURBO)
  {
    status = tdso_process_muxpud5_rx_turbo_frame(frame_blk_ptr);
  }
  else /* MUXPDU 3 or MuxPDU 5 Convolutional case below */
#endif /* FEATURE_IS2000_REL_A */
  {
    /* Iterate through all the SCH PDUs in the Received frame block */
    for (i=1; i<frame_blk_ptr->num_frames && i < TDSO_MAX_PDU; i++ )
    {
      rx_frame_ptr = &(frame_blk_ptr->frame_type.fwd_arr[i]);

      /* check the valid range of fr_rate value */
      if (rx_frame_ptr->fr_rate >= TDSO_MAX_RATE)
      {
        continue;
      }

#ifdef FEATURE_IS2000_REL_A
      /* set the length */
      if (rx_frame_ptr->mux_pdu_type == TDSO_MUX_PDU_5)
      {
        if (rx_frame_ptr->sch_rate < TDSO_SUPPORTED_SCH_RATES)  /*lint !e641 */
        {
          byte_len = tdso_muxpdu5_conv_data_len[rx_frame_ptr->sch_rate][frame_blk_ptr->sup_fr_mux]/8;
        }
        else
        {
          byte_len = 0;
        }
      }
      else
#endif
      {
        byte_len = tdso_bytes[rx_frame_ptr->fr_rate][frame_blk_ptr->sup_fr_mux];
      }

      // Compare the Received TDSO frame if the state is ON and if the frame
      // is either a TDSO_RATE_1 or TDSO_RATE_2 frame.

      if ((sch0_parm.pn_info[FWD_LINK].tdso_state) &&
          ((rx_frame_ptr->fr_rate == TDSO_RATE_2) ||
           (rx_frame_ptr->fr_rate == TDSO_RATE_1)
#ifdef FEATURE_IS2000_REL_A
           || (rx_frame_ptr->fr_rate == TDSO_RATE_3)
#endif
           ))
      {
        /* Copy the generated expected data block into local buffer */
        copy_size = memscpy(expected_data, sizeof(expected_data),
          frame_blk_ptr->frame_type.rev_arr[i].fr_item_ptr,
          byte_len);
        if( copy_size != byte_len )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            byte_len,
            sizeof(expected_data));
        }

        /* Compare each bit until the last packed full octet */
        for (j=0; j < byte_len; j++)

        {
          if ((err_mask = (rx_frame_ptr->fr_item_ptr[j] ^
              expected_data[j])) != 0)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Error");
            break;
          }
        }

        if (!err_mask)
        {
          /* --------------------------------------------------------------
          ** check last bits for errors. If the the frame is of type
          ** TDSO_RATE_1 and MUX_PDU_3, the number of bits is 1 less than
          ** that of the MUX_PDU_1 type.
          ** -----------------------------------------------------------*/
          if ((rx_frame_ptr->fr_channel == TDSO_SCH0) &&
              (rx_frame_ptr->mux_pdu_type == TDSO_MUX_PDU_3) &&
              (rx_frame_ptr->fr_rate == TDSO_RATE_1))
          {
            /* -------------------------------------------------------------
            ** For SCH and MUX_PDU_TYPE = 3, the number of bits in the
            ** last byte is 1 less than that of MUX_PDU_TYPE = 1.
            **-------------------------------------------------------------*/
            if (tdso_so == CAI_SO_FULL_TDSO)
            {
              err_mask = (uint8) (rx_frame_ptr->fr_item_ptr[j] &
                tdso_last_byte[rx_frame_ptr->fr_rate-1][frame_blk_ptr->sup_fr_mux]); /*lint !e641 */

            }
            else
            {
              /* For SCH and MUX_PDU_TYPE = 3, the number of bits in the
              ** last byte is 1 less than that of MUX_PDU_TYPE = 1 */
              err_mask =  (uint8) ((rx_frame_ptr->fr_item_ptr[j] &
                tdso_last_byte[rx_frame_ptr->fr_rate-1][frame_blk_ptr->sup_fr_mux]) ^ /*lint !e641 */
                tdso_last_byte[rx_frame_ptr->fr_rate-1][frame_blk_ptr->sup_fr_mux]);  /*lint !e641 */

            }
          }
          else
          {
            if (tdso_so == CAI_SO_FULL_TDSO)
            {
              err_mask = (uint8) (rx_frame_ptr->fr_item_ptr[j] &
                tdso_last_byte[rx_frame_ptr->fr_rate][frame_blk_ptr->sup_fr_mux]);

            }
            else
            {
              /* Check the validity of the Last byte in the frame */
              err_mask = (uint8) ((rx_frame_ptr->fr_item_ptr[j] &
                tdso_last_byte[rx_frame_ptr->fr_rate][frame_blk_ptr->sup_fr_mux]) ^
                tdso_last_byte[rx_frame_ptr->fr_rate][frame_blk_ptr->sup_fr_mux]);

            }
          }
        }

        /* Update the BIT error statistics in case of any error */
        if (err_mask)
        {
          rx_frame_ptr->fr_rate = TDSO_BIT_ERROR;
          M1X_MSG( DCP, LEGACY_ERROR,
            "Bit Error");

        }

        /* This will update the SCH PDU statistics */
        tdso_update_sch_pdu_stats(rx_frame_ptr);
      }
    } /* end for */

#ifdef FEATURE_IS2000_REL_A   //MuxPDU 5
    /*
    ** When MuxPDU 5 is used, there are some rare bit error conditions,
    ** assuming frame CRC is good (not erasure frame):
    ** (1) Bit errors inside LTU, bad LTU CRC
    **       This probably is the most common case.
    **       Because MUX is designed primarily for data service. It drops
    **     LTUs with bad CRC. So in here, we will check if the expected
    **       number of LTU is received. If the number is less, we mark
    **       the difference as bit-error LTUs and count the whole frame
    **       as bit error.
    ** (2) Bit error inside MuxPDU 5 data portion, good LTU CRC
    **       This is much rarer than (1), but possible. The bit error will
    **       be caught by the normal procedure. Nothing needs to be done.
    ** (3) Bit error inside MuxPDU 5 header portion, good LTU CRC
    **       This is extremely rare condition. Corrupted header is only
    **       a problem if it leads to mis-calculation of LTU length, and
    **       the wrong length is less than it should be. Otherwise, it falls
    **       under condition (2). If the length is less, it could make
    **       this PDU, next PDU and the whole frame to be bit error.
    **       That should be OK.
    **
    ** So the following checking is accounting for condition #1.
    */

    if ((rx_frame_ptr != NULL) &&
        (rx_frame_ptr->mux_pdu_type == TDSO_MUX_PDU_5))
    {
      /* Notice that we use the info in the last SCH PDU to make judgement */
      num_pdu = tdso_default_ltu_table[rx_frame_ptr->sch_rate]
                                      [frame_blk_ptr->sup_fr_mux];

      if (num_pdu == 0)
      {
        /* 0 in the table means no LTU, but there should be one PDU */
        num_pdu++;
      }

      /* num_frames contains one frame for FDCH */
      if (frame_blk_ptr->num_frames - 1 < num_pdu)
      {
        /* find the difference, which is number of bad PDU dropped by MUX */
        num_pdu -= ( frame_blk_ptr->num_frames - 1 );
        M1X_MSG( DCP, LEGACY_ERROR,
          "%d PDUs dropped by MUX",
          num_pdu);

        /* Adjust counter to compensate for the dropped PDU */
        sch_rx_pdu_table[TDSO_RATE_3_PDU][SCHRX_EN_FRAME_ERROR] += num_pdu;

        /* Mark the last frame as bit error so that the whole frame will
        ** be treated as bit error too.
        */
        rx_frame_ptr->fr_rate = TDSO_BIT_ERROR;
        M1X_MSG( DCP, LEGACY_ERROR,
          "Frame marked as BIT_ERROR");
      }
    }
#endif /* FEATURE_IS2000_REL_A */

    /* Check for non-null value of rx_frame_ptr to fix the Klocwork error */
    if (rx_frame_ptr != NULL)
    {
      /* Update the SCH frame statistics, after all PDUs are processed */
      tdso_update_sch_rx_stats(rx_frame_ptr,
                               sch0_parm.pn_info[FWD_LINK].tdso_state);
    }

  }


  /* Update the state of TDSO */
  tdso_update_on_off_state(TDSO_SCH0, FWD_LINK);

  return status;

} /* tdso_process_sch_rx_frame */

/*===========================================================================

FUNCTION : TDSO_UPDATE_ON_OFF_STATE

DESCRIPTION
  This function updates the TDSO state in case of a deterministic activity.
  It is used only for full TDSO.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdso_update_on_off_state
(
  tdso_channel_type  chan,
  tdso_link_dir_type direction
)
{
  tdso_chan_parms *chan_parm_ptr;

  if (tdso_so == CAI_SO_TDSO)
  {
    return;
  }

  switch (chan)
  {
    case TDSO_FCH:
      chan_parm_ptr = &fch_parm ;
      break ;

    case TDSO_DCCH:
      chan_parm_ptr = &dcch_parm ;
      break ;

    case TDSO_SCH0:
      chan_parm_ptr = &sch0_parm ;
      break ;

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Channel!! %d",
        chan );
      return;
  }

  if (chan_parm_ptr->pn_info[direction].frame_act)
  {
    return;   // No need to change state for Random activity here.
  }

  if ((chan_parm_ptr->pn_info[direction].tdso_state) &&
      ((chan_parm_ptr->pn_info[direction].tx_on_period == 0) ||
       (chan_parm_ptr->pn_info[direction].tx_on_period_cnt-- == 1)))
  {
    if (chan_parm_ptr->pn_info[direction].tx_off_period > 0)
    {
      /* Change State to Off and Load the Off period */
      chan_parm_ptr->pn_info[direction].tdso_state = FALSE;

      /* load the FCH_TX_OFF_PERIOD from the test parameters */
      chan_parm_ptr->pn_info[direction].tx_off_period_cnt =
      chan_parm_ptr->pn_info[direction].tx_off_period;
    }
    else
    {
      chan_parm_ptr->pn_info[direction].tx_on_period_cnt =
      chan_parm_ptr->pn_info[direction].tx_on_period;
    }
  }
  else if ((!chan_parm_ptr->pn_info[direction].tdso_state) &&
           ((chan_parm_ptr->pn_info[direction].tx_off_period == 0) ||
            (chan_parm_ptr->pn_info[direction].tx_off_period_cnt-- == 1)))
  {
    if (chan_parm_ptr->pn_info[direction].tx_on_period  > 0)
    {
      /* Change State to On and Load the On period */
      chan_parm_ptr->pn_info[direction].tdso_state = TRUE;

      /* load the FCH_TX_OFF_PERIOD from the test parameters */
      chan_parm_ptr->pn_info[direction].tx_on_period_cnt =
      chan_parm_ptr->pn_info[direction].tx_on_period;
    }
    else
    {
      chan_parm_ptr->pn_info[direction].tx_off_period_cnt =
      chan_parm_ptr->pn_info[direction].tx_off_period;
    }
  }
} /* tdso_update_on_off_state */

/*===========================================================================

FUNCTION : TDSO_LOG_STDSO_STATS

DESCRIPTION:
  This function allocates memory for the stdso_log_type using the
  LOG services and commits the counters log to be displayed on the
  DM.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:


===========================================================================*/
void tdso_log_stdso_stats(void)
{
  const caii_srv_cfg_type *config = snm_get_current_config();
    /* Get current configuration */

  log_stdso_p2_type *stdso = NULL;

  stdso = (log_stdso_p2_type*) log_alloc(LOG_STDSO_P2_C,
          sizeof(log_stdso_p2_type));

  /* Update Simple TDSO statistics */
  if (stdso != NULL)
  {
    /* ------------------
    ** Update MUX Options
    ** ------------------ */
    stdso->for_mux_opt = config->fwd_mux;
    stdso->rev_mux_opt = config->rev_mux;

    /* ------------------------------
    ** Update FCH Transmit statistics
    ** ------------------------------ */
    stdso->e1_t1 = fch_tx_table[FDCHTX_E1_TRAFFIC];
    stdso->e1_td = fch_tx_table[FDCHTX_E1_DIM_BURST];
    stdso->e1_tb = fch_tx_table[FDCHTX_E1_BLANK];
    stdso->eb_tb = fch_tx_blank_table[FDCHTX_BLANK_BLANK];
    stdso->eb_to = fch_tx_blank_table[FDCHTX_BLANK_OTHER];

    /* We don't have rate 1/2, 1/4 or 1/8 frame */
    stdso->e2_t2 = 0;
    stdso->e4_t4 = 0;
    stdso->e8_t8 = 0;
    stdso->e2_td = 0;
    stdso->e4_td = 0;
    stdso->e8_td = 0;
    stdso->e2_tb = 0;
    stdso->e4_tb = 0;
    stdso->e8_tb = 0;

    /* ------------------------------
    ** Update DCCH Transmit statistics
    ** ------------------------------ */
    stdso->dcch_e1_t1 = dcch_tx_table[FDCHTX_E1_TRAFFIC];
    stdso->dcch_e1_td = dcch_tx_table[FDCHTX_E1_DIM_BURST];
    stdso->dcch_e1_tb = dcch_tx_table[FDCHTX_E1_BLANK];
    stdso->dcch_eb_tb = dcch_tx_blank_table[FDCHTX_BLANK_BLANK];
    stdso->dcch_eb_to = dcch_tx_blank_table[FDCHTX_BLANK_OTHER];

    /* We don't have rate 1/2, 1/4 or 1/8 frame */
    stdso->dcch_e2_t2 = 0;
    stdso->dcch_e4_t4 = 0;
    stdso->dcch_e8_t8 = 0;
    stdso->dcch_e2_td = 0;
    stdso->dcch_e4_td = 0;
    stdso->dcch_e8_td = 0;
    stdso->dcch_e2_tb = 0;
    stdso->dcch_e4_tb = 0;
    stdso->dcch_e8_tb = 0;

    /* ------------------------------
    ** Update SCH Transmit statistics
    ** ------------------------------ */
    stdso->es1x_t1x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_1X];
    stdso->es2x_t2x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_2X];
    stdso->es4x_t4x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_4X];
    stdso->es8x_t8x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_8X];
    stdso->es16x_t16x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_16X];
    stdso->es1x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_1X];
    stdso->es2x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_2X];
    stdso->es4x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_4X];
    stdso->es8x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_8X];
    stdso->es16x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_16X];
    stdso->esb_tb = sch_tx_blank;

    /* -----------------------------
    ** Update FCH Receive statistics
    ** ----------------------------- */
    stdso->e1_r1   = fch_rx_table[TDSO_RATE_1];
    stdso->e1_rerr = fch_rx_table[TDSO_BIT_ERROR];
    stdso->e1_rd   = fch_rx_table[TDSO_DIM_BURST];
    stdso->e1_rb   = fch_rx_table[TDSO_BLANK];
    stdso->e1_r2   = fch_rx_table[TDSO_RATE_HALF];
    stdso->e1_r4   = fch_rx_table[TDSO_RATE_QUARTER];
    stdso->e1_r8   = fch_rx_table[TDSO_RATE_EIGHTH];
    stdso->e1_rfl  = fch_rx_table[TDSO_INSUFFICIENT_FRAME];
    stdso->e1_re   = fch_rx_table[TDSO_ERASURE];
    stdso->en_rn   = fch_rx_null_table[FDCHRX_NULL_NULL];
    stdso->en_rb   = fch_rx_null_table[FDCHRX_NULL_BLANK];
    stdso->en_ro   = fch_rx_null_table[FDCHRX_NULL_OTHER];

    /* We don't have rate 1/2, 1/4 or 1/8 frame */
    stdso->e2_r2   = 0;
    stdso->e2_rerr = 0;
    stdso->e2_rd   = 0;
    stdso->e2_rb   = 0;
    stdso->e2_r1   = 0;
    stdso->e2_r4   = 0;
    stdso->e2_r8   = 0;
    stdso->e2_rfl  = 0;
    stdso->e2_re   = 0;

    stdso->e4_r4   = 0;
    stdso->e4_rerr = 0;
    stdso->e4_rd   = 0;
    stdso->e4_rb   = 0;
    stdso->e4_r1   = 0;
    stdso->e4_r2   = 0;
    stdso->e4_r8   = 0;
    stdso->e4_rfl  = 0;
    stdso->e4_re   = 0;

    stdso->e8_r8   = 0;
    stdso->e8_rerr = 0;
    stdso->e8_rd   = 0;
    stdso->e8_rb   = 0;
    stdso->e8_r1   = 0;
    stdso->e8_r2   = 0;
    stdso->e8_r4   = 0;
    stdso->e8_rfl  = 0;
    stdso->e8_re   = 0;

    /* -----------------------------
    ** Update DCCH Receive statistics
    ** ----------------------------- */
    stdso->dcch_e1_r1   = dcch_rx_table[TDSO_RATE_1];
    stdso->dcch_e1_rerr = dcch_rx_table[TDSO_BIT_ERROR];
    stdso->dcch_e1_rd   = dcch_rx_table[TDSO_DIM_BURST];
    stdso->dcch_e1_rb   = dcch_rx_table[TDSO_BLANK];
    stdso->dcch_e1_r2   = dcch_rx_table[TDSO_RATE_HALF];
    stdso->dcch_e1_r4   = dcch_rx_table[TDSO_RATE_QUARTER];
    stdso->dcch_e1_r8   = dcch_rx_table[TDSO_RATE_EIGHTH];
    stdso->dcch_e1_rfl  = dcch_rx_table[TDSO_INSUFFICIENT_FRAME];
    stdso->dcch_e1_re   = dcch_rx_table[TDSO_ERASURE];
    stdso->dcch_en_rn   = dcch_rx_null_table[FDCHRX_NULL_NULL];
    stdso->dcch_en_rb   = dcch_rx_null_table[FDCHRX_NULL_BLANK];
    stdso->dcch_en_ro   = dcch_rx_null_table[FDCHRX_NULL_OTHER];

    /* We don't have rate 1/2, 1/4 or 1/8 frame */
    stdso->dcch_e2_r2   = 0;
    stdso->dcch_e2_rerr = 0;
    stdso->dcch_e2_rd   = 0;
    stdso->dcch_e2_rb   = 0;
    stdso->dcch_e2_r1   = 0;
    stdso->dcch_e2_r4   = 0;
    stdso->dcch_e2_r8   = 0;
    stdso->dcch_e2_rfl  = 0;
    stdso->dcch_e2_re   = 0;

    stdso->dcch_e4_r4   = 0;
    stdso->dcch_e4_rerr = 0;
    stdso->dcch_e4_rd   = 0;
    stdso->dcch_e4_rb   = 0;
    stdso->dcch_e4_r1   = 0;
    stdso->dcch_e4_r2   = 0;
    stdso->dcch_e4_r8   = 0;
    stdso->dcch_e4_rfl  = 0;
    stdso->dcch_e4_re   = 0;

    stdso->dcch_e8_r8   = 0;
    stdso->dcch_e8_rerr = 0;
    stdso->dcch_e8_rd   = 0;
    stdso->dcch_e8_rb   = 0;
    stdso->dcch_e8_r1   = 0;
    stdso->dcch_e8_r2   = 0;
    stdso->dcch_e8_r4   = 0;
    stdso->dcch_e8_rfl  = 0;
    stdso->dcch_e8_re   = 0;

    /* -----------------------------
    ** Update SCH Receive Statistics
    ** ----------------------------- */
    stdso->es1x_r1x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_1X];
    stdso->es2x_r2x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_2X];
    stdso->es4x_r4x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_4X];
    stdso->es8x_r8x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_8X];
    stdso->es16x_r16x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_16X];
    stdso->es1x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_1X];
    stdso->es2x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_2X];
    stdso->es4x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_4X];
    stdso->es8x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_8X];
    stdso->es16x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_16X];
    stdso->es1x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_1X];
    stdso->es2x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_2X];
    stdso->es4x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_4X];
    stdso->es8x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_8X];
    stdso->es16x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_16X];
    stdso->es1x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_1X];
    stdso->es2x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_2X];
    stdso->es4x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_4X];
    stdso->es8x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_8X];
    stdso->es16x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_16X];
    stdso->esb_rb = sch_rx_blank_table[SCHRX_BLANK_BLANK];
    stdso->esb_ro = sch_rx_blank_table[SCHRX_BLANK_OTHER];

#ifdef FEATURE_IS2000_REL_A
    stdso->es32x_r32x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_32X];
    stdso->es32x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_32X];
    stdso->es32x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_32X];
    stdso->es32x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_32X];
#endif /* FEATURE_IS2000_REL_A  */

    /* Commit the log with the LOG facility */
    log_commit(stdso);
  }
  else
  {
    /* No buffer, log STDSO statistics later */
    return;
  }
} /* tdso_log_stdso_stats */

/*===========================================================================

FUNCTION : TDSO_LOG_FTDSO_STATS

DESCRIPTION:
  This function allocates memory for the ftdso_log_type using the
  LOG services and commits the counters log to be displayed on the
  DM.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/
void tdso_log_ftdso_stats( void )
{
  const caii_srv_cfg_type  *config = snm_get_current_config();
    // Get current configuration

  log_ftdso_p2_type *tdso = NULL;

  tdso = (log_ftdso_p2_type*) log_alloc(LOG_FTDSO_P2_C,
          sizeof(log_ftdso_p2_type));

  /* Update Full TDSO statistics */
  if (tdso != NULL)
  {
    /* ------------------
    ** Update MUX Options
    ** ------------------ */
    tdso->for_mux_opt = config->fwd_mux;
    tdso->rev_mux_opt = config->rev_mux;

    /* ------------------------------
    ** Update FCH Transmit statistics
    ** ------------------------------ */
    tdso->tdso_e1_t1 = fch_tx_table[FDCHTX_E1_TRAFFIC];
    tdso->tdso_e1_td = fch_tx_table[FDCHTX_E1_DIM_BURST];
    tdso->tdso_e1_tb = fch_tx_table[FDCHTX_E1_BLANK];
    tdso->tdso_eb_tb = fch_tx_blank_table[FDCHTX_BLANK_BLANK];
    tdso->tdso_eb_to = fch_tx_blank_table[FDCHTX_BLANK_OTHER];

    /* ------------------------------
    ** Update DCCH Transmit statistics
    ** ------------------------------ */
    tdso->dcch_e1_t1 = dcch_tx_table[FDCHTX_E1_TRAFFIC];
    tdso->dcch_e1_td = dcch_tx_table[FDCHTX_E1_DIM_BURST];
    tdso->dcch_e1_tb = dcch_tx_table[FDCHTX_E1_BLANK];
    tdso->dcch_eb_tb = dcch_tx_blank_table[FDCHTX_BLANK_BLANK];
    tdso->dcch_eb_to = dcch_tx_blank_table[FDCHTX_BLANK_OTHER];

    /* ------------------------------
    ** Update SCH Transmit statistics
    ** ------------------------------ */
    tdso->tdso_e1x_t1x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_1X];
    tdso->tdso_e2x_t2x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_2X];
    tdso->tdso_e4x_t4x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_4X];
    tdso->tdso_e8x_t8x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_8X];
    tdso->tdso_e16x_t16x = sch_tx_table[SCHTX_EN_TRAFFIC][TDSO_SCH_RATE_16X];

#ifdef FEATURE_IS2000_REL_A

    tdso->tdso_e32x_t32x = 0;
#endif

    tdso->tdso_e1x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_1X];
    tdso->tdso_e2x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_2X];
    tdso->tdso_e4x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_4X];
    tdso->tdso_e8x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_8X];
    tdso->tdso_e16x_tb = sch_tx_table[SCHTX_EN_BLANK][TDSO_SCH_RATE_16X];

#ifdef FEATURE_IS2000_REL_A

    tdso->tdso_e32x_tb = 0;
#endif

    tdso->tdso_esb_tb = sch_tx_blank;

    /* -----------------------------
    ** Update FCH Receive statistics
    ** ----------------------------- */
    tdso->tdso_e1_r1   = fch_rx_table[TDSO_RATE_1];
    tdso->tdso_e1_rerr = fch_rx_table[TDSO_BIT_ERROR];
    tdso->tdso_e1_rd   = fch_rx_table[TDSO_DIM_BURST];
    tdso->tdso_e1_ro   = fch_rx_table[TDSO_OTHER];
    tdso->tdso_e1_rb   = fch_rx_table[TDSO_BLANK];
    tdso->tdso_e1_rfl  = fch_rx_table[TDSO_INSUFFICIENT_FRAME];
    tdso->tdso_e1_re   = fch_rx_table[TDSO_ERASURE];
    tdso->tdso_en_rn   = fch_rx_null_table[FDCHRX_NULL_NULL];
    tdso->tdso_en_rb   = fch_rx_null_table[FDCHRX_NULL_BLANK];
    tdso->tdso_en_ro   = fch_rx_null_table[FDCHRX_NULL_OTHER];

    /* -----------------------------
    ** Update DCCH Receive statistics
    ** ----------------------------- */
    tdso->dcch_e1_r1   = dcch_rx_table[TDSO_RATE_1];
    tdso->dcch_e1_rerr = dcch_rx_table[TDSO_BIT_ERROR];
    tdso->dcch_e1_rd   = dcch_rx_table[TDSO_DIM_BURST];
    tdso->dcch_e1_ro   = dcch_rx_table[TDSO_OTHER];
    tdso->dcch_e1_rb   = dcch_rx_table[TDSO_BLANK];
    tdso->dcch_e1_rfl  = dcch_rx_table[TDSO_INSUFFICIENT_FRAME];
    tdso->dcch_e1_re   = dcch_rx_table[TDSO_ERASURE];
    tdso->dcch_en_rn   = dcch_rx_null_table[FDCHRX_NULL_NULL];
    tdso->dcch_en_rb   = dcch_rx_null_table[FDCHRX_NULL_BLANK];
    tdso->dcch_en_ro   = dcch_rx_null_table[FDCHRX_NULL_OTHER];

    /* -----------------------------
    ** Update SCH Receive Statistics
    ** ----------------------------- */
    tdso->tdso_e1x_r1x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_1X];
    tdso->tdso_e2x_r2x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_2X];
    tdso->tdso_e4x_r4x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_4X];
    tdso->tdso_e8x_r8x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_8X];
    tdso->tdso_e16x_r16x = sch_rx_table[SCHRX_EN_TRAFFIC][TDSO_SCH_RATE_16X];

#ifdef FEATURE_IS2000_REL_A

    tdso->tdso_e32x_r32x = 0;
#endif

    tdso->tdso_e1x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_1X];
    tdso->tdso_e2x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_2X];
    tdso->tdso_e4x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_4X];
    tdso->tdso_e8x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_8X];
    tdso->tdso_e16x_rerr = sch_rx_table[SCHRX_EN_FRAME_ERROR][TDSO_SCH_RATE_16X];

#ifdef FEATURE_IS2000_REL_A

    tdso->tdso_e32x_rerr = 0;
#endif

    tdso->tdso_e1x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_1X];
    tdso->tdso_e2x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_2X];
    tdso->tdso_e4x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_4X];
    tdso->tdso_e8x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_8X];
    tdso->tdso_e16x_re = sch_rx_table[SCHRX_EN_ERASURE][TDSO_SCH_RATE_16X];

#ifdef FEATURE_IS2000_REL_A

    tdso->tdso_e32x_re = 0;
#endif

    tdso->tdso_e1x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_1X];
    tdso->tdso_e2x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_2X];
    tdso->tdso_e4x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_4X];
    tdso->tdso_e8x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_8X];
    tdso->tdso_e16x_rb = sch_rx_table[SCHRX_EN_BLANK][TDSO_SCH_RATE_16X];

#ifdef FEATURE_IS2000_REL_A

    tdso->tdso_e32x_rb = 0;
#endif

    tdso->tdso_esb_rb = sch_rx_blank_table[SCHRX_BLANK_BLANK];
    tdso->tdso_esb_ro = sch_rx_blank_table[SCHRX_BLANK_OTHER];

    /* -----------------------------
    ** Update PDU Receive Statistics
    ** ----------------------------- */
    tdso->tdso_e3_r3 = sch_rx_pdu_table[TDSO_RATE_3_PDU][SCHRX_EN_TRAFFIC];
    tdso->tdso_e3_rerr =
      sch_rx_pdu_table[TDSO_RATE_3_PDU][SCHRX_EN_FRAME_ERROR];
    tdso->tdso_e3_re = sch_rx_pdu_table[TDSO_RATE_3_PDU][SCHRX_EN_ERASURE];
    tdso->tdso_e2_r2 = sch_rx_pdu_table[TDSO_RATE_2_PDU][SCHRX_EN_TRAFFIC];
    tdso->tdso_e2_rerr =
      sch_rx_pdu_table[TDSO_RATE_2_PDU][SCHRX_EN_FRAME_ERROR];
    tdso->tdso_e2_re = sch_rx_pdu_table[TDSO_RATE_2_PDU][SCHRX_EN_ERASURE];
    tdso->tdso_e1a_r1a = sch_rx_pdu_table[TDSO_RATE_1A_PDU][SCHRX_EN_TRAFFIC];
    tdso->tdso_e1a_rerr =
      sch_rx_pdu_table[TDSO_RATE_1A_PDU][SCHRX_EN_FRAME_ERROR];
    tdso->tdso_e1a_re = sch_rx_pdu_table[TDSO_RATE_1A_PDU][SCHRX_EN_ERASURE];
    tdso->tdso_e1b_rerr =
      sch_rx_pdu_table[TDSO_RATE_1B_PDU][SCHRX_EN_FRAME_ERROR];
    tdso->tdso_e1b_r1b = sch_rx_pdu_table[TDSO_RATE_1B_PDU][SCHRX_EN_TRAFFIC];
    tdso->tdso_e1b_re = sch_rx_pdu_table[TDSO_RATE_1B_PDU][SCHRX_EN_ERASURE];

    /* Commit the log with the LOG facility */
    log_commit(tdso);
  }
  else
  {
    /* No buffer, log TDSO statistics later */
    return;
  }
} /* tdso_log_ftdso_stats */

/*===========================================================================

FUNCTION : tdso_clear_stats

DESCRIPTION:
  This function clears the FCH and SCH transmit/Receive counters

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/

/*lint -e818*/
diag_rsp_type * tdso_clear_stats(
  DIAG_TDSO_STAT_RESET_F_req_type *req_ptr, /*lint -esym(715,req_ptr)*/
    /* Pointer to request packet, not used */
  word pkt_len /*lint -esym(715,pkt_len)*/
    /* Length of request packet, not used */
    /*lint !e715*/
)
{
  DIAG_TDSO_STAT_RESET_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof( DIAG_TDSO_STAT_RESET_F_rsp_type );

  /* ----------------------------------------------------------------------
  ** Clear the FCH transmit statistics from the internal FCH TX tables.
  ** ---------------------------------------------------------------------*/
  memset(fch_tx_table, 0, sizeof(fch_tx_table));
  memset(fch_tx_blank_table, 0, sizeof(fch_tx_blank_table));

  /* ----------------------------------------------------------------------
  ** Clear the DCCH transmit statistics from the internal FCH TX tables.
  ** ---------------------------------------------------------------------*/
  memset(dcch_tx_table, 0, sizeof(dcch_tx_table));
  memset(dcch_tx_blank_table, 0, sizeof(dcch_tx_blank_table));

  /* ----------------------------------------------------------------------
  ** Clear the SCH transmit statistics from the internal SCH TX tables.
  ** ---------------------------------------------------------------------*/
  memset(sch_tx_table, 0, sizeof(sch_tx_table));
  sch_tx_blank = 0;

  /* ----------------------------------------------------------------------
  ** Clear the FCH Receive statistics from the internal FCH RX tables.
  ** ---------------------------------------------------------------------*/
  memset(fch_rx_table, 0, sizeof(fch_rx_table));
  memset(fch_rx_null_table, 0, sizeof(fch_rx_null_table));

  /* ----------------------------------------------------------------------
  ** Clear the DCCH Receive statistics from the internal FCH RX tables.
  ** ---------------------------------------------------------------------*/
  memset(dcch_rx_table, 0, sizeof(dcch_rx_table));
  memset(dcch_rx_null_table, 0, sizeof(dcch_rx_null_table));

  /* ----------------------------------------------------------------------
  ** Clear the SCH transmit Receive from the internal SCH TX tables.
  ** ---------------------------------------------------------------------*/
  memset(sch_rx_table, 0, sizeof(sch_rx_table));
  memset(sch_rx_blank_table, 0, sizeof(sch_rx_blank_table));

  /* ----------------------------------------------------------------------
  ** Clear the PDU Receive statistics from the internal PDU table.
  ** ---------------------------------------------------------------------*/
  memset(sch_rx_pdu_table, 0, sizeof(sch_rx_pdu_table));

  /* Create response packet */
  rsp_ptr = (DIAG_TDSO_STAT_RESET_F_rsp_type *) diagbuf_pkt_alloc( rsp_len );
  if( rsp_ptr != NULL ) {
    rsp_ptr->cmd_code = DIAG_TDSO_STAT_RESET_F;
  }
  return ((diag_rsp_type *) rsp_ptr );  /*lint !e826 */

} /* tdso_clear_stats */
/*lint +e818*/

/*===========================================================================

FUNCTION : TDSO_PRE_SYNC_PROC

DESCRIPTION:
  This function prepares circular buffer for supplemental channel at the Pre
  Sync frame.

DEPENDENCIES:
  None

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/
void tdso_pre_sync_proc
(
  dword              time_val,
  tdso_link_dir_type direction
)
{

  dword frame32;                      /* Current frame number */
  dword* seed_ptr;                    /* Seed pointer for PN generation */
  tdso_buffer_info *chan_pn_info_ptr; /* points to the channels PN info */
  const caii_srv_cfg_type *config;    /* Point to current config info */


  /* Get the Current Configuration information from the SNM */
  config = snm_get_current_config();

  /* --------------------------------------------------------
  ** Prepare the Circular buffer for the Supplemental Channel
  ** -------------------------------------------------------- */
  if (((config->num_for_sch > 0) && (direction == FWD_LINK)) ||
      ((config->num_rev_sch > 0) && (direction == REV_LINK)))
  {
    frame32 = time_val;
    if (direction == REV_LINK) /* Reverse link Pre Sync frame */
    {
      seed_ptr = &tdso_tx_seed;   /* point to TX seed */
      frame32 ^= DEF_TX_SEED;     /* XOR frame with TX SEED */
    }
    else  /* Forward Link Pre Sync frame */
    {
      seed_ptr = &tdso_rx_seed;   /* point to RX seed */
      frame32 ^= DEF_RX_SEED;     /* XOR frame with RX SEED */
    }

    tdso_set_num_rand(TDSO_SCH0, direction);
    chan_pn_info_ptr = &sch0_parm.pn_info[direction];


    /* Change NUM_RAND based on the Frame source value */
    *seed_ptr = frame32; /* Reset seed to current */
    *seed_ptr <<= 1;

    /* Generate circular buffer */
    tdso_generate_rand_circ_buffer(chan_pn_info_ptr, seed_ptr, TDSO_SCH0);
    M1X_MSG( DCP, LEGACY_MED,
      "Pre sync: dir %d, fr %l, seed %l",
      direction,
      time_val,
      *seed_ptr);
  }

} /* tdso_pre_sync_proc */

/*===========================================================================

FUNCTION : TDSO_SYNC_PROC

DESCRIPTION:
  All pseudo-random number generators associated with the channel are
  reinitialized prior to TDSO frame processing for each synchronization
  frame. All service option initialization and control operations also
  take effect prior to TDSO frame processing for a synchronization frame
  for each physical channel. Sync rame occurs every 10.24 seconds.

  Note: This function is used to sync on FWD and LINK based on the
        DIRECTION parameter.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/

/*lint -e818*/
void tdso_sync_proc
(
  tdso_link_dir_type    direction,
  tdso_frame_block_type *frame_blk_ptr
)
{
  dword frame32, pntime; /* Current frame number */
  dword* seed_ptr;       /* Seed pointer used during PN generation */
  dword  pnval = 0;      /* PN value */
  tdso_buffer_info *chan_pn_info_ptr; /* Pointer to channels PN info */
  const caii_srv_cfg_type *config;    /* Pointer to current config info */
  size_t copy_size;     /*To store value returned by memscpy*/


  if (tdso_pre_sync_ptr->prev_sync_time != frame_blk_ptr->fr_time)
  {
    /* Get the Current Configuration information from the SNM */
    config = snm_get_current_config();

    frame32 = frame_blk_ptr->fr_time;

    if (direction == REV_LINK)  /* Reverse link sync */
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "REV Link Sync: %lx ",
        frame32);
      seed_ptr = &tdso_tx_seed;  /* Point to TX seed */
      frame32 ^= DEF_TX_SEED;    /* XOR frame with TX SEED */
    }
    else  /* Forward Link Sync frame */
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "FOR Link Sync: %lx ",
        frame32);
      seed_ptr = &tdso_rx_seed; /* Point to RX seed */
      frame32 ^= DEF_RX_SEED;   /* XOR frame with RX SEED */
    }

    /* This function copies the counters if the copy counters flag is set */
    tdso_copy_counter_buffer(direction);

    /* This function clears the counters if the clear counter flag is set */
    tdso_clear_counter_buffer(direction);

    if (config->dcch_cc_incl)
    {
      /* Set the Num Rand for DCCH */
      tdso_set_num_rand(TDSO_DCCH, direction);
      /* Update the ON and OFF periods based on the Frame activity */
      tdso_update_on_off_threshold(&dcch_parm, direction);

    }
    else
    {
      /* Set the Num Rand for FCH */
      tdso_set_num_rand(TDSO_FCH, direction);
      /* Update the ON and OFF periods based on the Frame activity */
      tdso_update_on_off_threshold(&fch_parm, direction);
    }

    tdso_update_on_off_threshold(&sch0_parm, direction);

    /* -------------------
    ** FCH / DCCH Synchronization
    ** ------------------- */

    if (config->dcch_cc_incl)
    {
      /* Pointer to the DCCH pn info */
      chan_pn_info_ptr = &dcch_parm.pn_info[direction];
    }
    else
    {
      /* Pointer to the FCH pn info */
      chan_pn_info_ptr = &fch_parm.pn_info[direction];
    }

    // Set the FCH synchronization field
    chan_pn_info_ptr->tdso_sync = TRUE;

    /* Change NUM_RAND based on the Frame source value */
    *seed_ptr = frame32; /* Reset seed to current */
    *seed_ptr <<= 1;

    M1X_MSG( DCP, LEGACY_HIGH,
      "PN = %lx",
      (chan_pn_info_ptr->pn)/2 );

    /* First Synchronization frame */
    if (!chan_pn_info_ptr->pn_valid)
    {
      /* Save seed value */
      pntime = *seed_ptr;

      *seed_ptr = mar_ran_next(*seed_ptr);
      *seed_ptr = mar_ran_next(*seed_ptr);
      *seed_ptr = mar_ran_next(*seed_ptr);
      *seed_ptr = mar_ran_next(*seed_ptr);
      *seed_ptr = mar_ran_next(*seed_ptr);

      /* Save the PN value */
      pnval = *seed_ptr;

      /* Set the PN value */
      chan_pn_info_ptr->pn = pnval;
      chan_pn_info_ptr->pn_valid = TRUE;

      if (config->dcch_cc_incl)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DCCH PN = %lx, direction = %d",
          chan_pn_info_ptr->pn,
          direction);

        /* Reset the Counters */
        if (direction == REV_LINK)
        {
          memset(dcch_tx_table, 0, sizeof(dcch_tx_table));
          memset(dcch_tx_blank_table, 0, sizeof(dcch_tx_blank_table));
        }
        else
        {
          memset(dcch_rx_table, 0, sizeof(dcch_rx_table));
          memset(dcch_rx_null_table, 0, sizeof(dcch_rx_null_table));
        }
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "FCH PN = %lx, direction = %d",
          chan_pn_info_ptr->pn,
          direction);

        /* Reset the Counters */
        if (direction == REV_LINK)
        {
          memset(fch_tx_table, 0, sizeof(fch_tx_table));
          memset(fch_tx_blank_table, 0, sizeof(fch_tx_blank_table));
        }
        else
        {
          memset(fch_rx_table, 0, sizeof(fch_rx_table));
          memset(fch_rx_null_table, 0, sizeof(fch_rx_null_table));
        }
      }

      /* Restore seed value */
      *seed_ptr = pntime;
    }

    if (config->dcch_cc_incl)
    {
      /* Generate Circular buffer */
      if (dcch_parm.pn_info[direction].tst_par.data_src)
      {
        tdso_generate_rand_circ_buffer(chan_pn_info_ptr, seed_ptr, TDSO_DCCH);

        M1X_MSG( DCP, LEGACY_MED,
          "DC: src: %d, dir %d, Yn1 %lx",
          dcch_parm.pn_info[direction].tst_par.data_src,
          direction,
          *seed_ptr);
      }
    }
    else
    {

      /* Generate Circular buffer */
      if (fch_parm.pn_info[direction].tst_par.data_src)
      {
        tdso_generate_rand_circ_buffer(chan_pn_info_ptr, seed_ptr, TDSO_FCH);

        M1X_MSG( DCP, LEGACY_MED,
          "F: src: %d, dir %d, Yn1 %lx",
          fch_parm.pn_info[direction].tst_par.data_src,
          direction,
          *seed_ptr);
      }
    }

    /* Save the current Seed ptr to be used in the next PN generation */
    chan_pn_info_ptr->last_seed = *seed_ptr;
    chan_pn_info_ptr->data_offset =
      (uint16) ((chan_pn_info_ptr->circ_buffer[0] & 0x3F) %
      (chan_pn_info_ptr->num_rand * 3));

    /* -------------------
    ** SCH Synchronization
    ** ------------------- */

    if (((config->num_for_sch > 0) && (direction == FWD_LINK)) ||
        ((config->num_rev_sch > 0) && (direction == REV_LINK)))
    {
      /* Set the Num Rand */
      tdso_set_num_rand(TDSO_SCH0, direction);

      chan_pn_info_ptr = &sch0_parm.pn_info[direction];

      // Set the SCH0 synchronization field
      chan_pn_info_ptr->tdso_sync = TRUE;

      /* First Synchronization frame */
      if (!chan_pn_info_ptr->pn_valid)
      {
        // Always Set the SCH PN same as that of the FCH/DCCH PN for 20ms frame
        // types.
        /* Set the PN value */
        chan_pn_info_ptr->pn = pnval;
        chan_pn_info_ptr->pn_valid = TRUE;
        M1X_MSG( DCP, LEGACY_ERROR,
          "SCH0 PN = %lx, direction = %d",
          chan_pn_info_ptr->pn,
          direction);

        /* Reset the SCH counters */
        if (direction == REV_LINK)
        {
          memset(sch_tx_table, 0, sizeof(sch_tx_table));
          sch_tx_blank = 0;
        }
        else
        {
          memset(sch_rx_table, 0, sizeof(sch_rx_table));
          memset(sch_rx_blank_table, 0, sizeof(sch_rx_blank_table));
          memset(sch_rx_pdu_table, 0, sizeof(sch_rx_pdu_table));
        }
      }

      if (config->dcch_cc_incl)
      {
        /* SCH PN should always in sync with DCCH PN */
        chan_pn_info_ptr->pn = dcch_parm.pn_info[direction].pn;
      }
      else
      {
        /* SCH PN should always in sync with FCH PN */
        chan_pn_info_ptr->pn = fch_parm.pn_info[direction].pn;
      }

      if (sch0_parm.pn_info[direction].tst_par.data_src)
      {
        //Copy pre_synchronization circular buffer to the actual buffer
        copy_size = memscpy(chan_pn_info_ptr->circ_buffer, sizeof(sch_rx_circ_buffer),
                            chan_pn_info_ptr->pre_sync_circ_buffer, sizeof(sch_rx_circ_buffer));
        if( copy_size != sizeof(sch_rx_circ_buffer) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(sch_rx_circ_buffer),
            sizeof(sch_rx_circ_buffer));
        }
      }

      /* Save the current Seed ptr to be used in the next PN generation */
      chan_pn_info_ptr->last_seed = chan_pn_info_ptr->pre_sync_last_seed;
      chan_pn_info_ptr->data_offset =
        (uint16) ((chan_pn_info_ptr->circ_buffer[0] & 0x3F) %
        (chan_pn_info_ptr->num_rand * 3));
    }

    /* Store synchronization time */
    tdso_pre_sync_ptr->prev_sync_time = frame_blk_ptr->fr_time;

    /* We are no longer in 1 sec boundary of SYNC frame */
    if (direction == FWD_LINK)
    {
      socm_deferred = FALSE;
    }
  }

} /* tdso_sync_proc */
/*lint +e818*/

/*===========================================================================

FUNCTION : TDSO_GENERATE_RAND_CIRC_BUFFER

DESCRIPTION:
  This function generates the Random circular buffer. Pseudo-random number
  generators are utilized for frame generation. These generators are
  associated with a particular physical channel (forward or reverse) and are
  initialized at each synchronization frame. The pseudo-random number
  generators are iterated one or more times for every frame. Iterations of
  the pseudo-random number generators are used for information bit
  generation, enough to fill two maximum rate physical layer frames (per the
  configured RC). Bits are stored in circular buffers.

DEPENDENCIES:
  Depends on the NUM_RAND for FCH or SCH. Should be called only at the
  Synchronization frame.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/
void tdso_generate_rand_circ_buffer
(
  tdso_buffer_info  *chan_pn_info_ptr,
  dword*            seed_ptr,
  tdso_channel_type channel
)
{
  dword rand_data;
  byte  *buffer;
  int   i, j=0, dog_cnt=0;
  /* Return value of memscpy function */
  size_t copy_size;

  /* Size of the array which circ_buffer/pre_sync_circ_buffer are pointing to.*/
  size_t dest_size;


  if ((channel == TDSO_FCH)
      || (channel == TDSO_DCCH)
     )
  {
    buffer = chan_pn_info_ptr->circ_buffer ;
    dest_size = sizeof(fch_rx_circ_buffer);
  }
  else
  {
    buffer = chan_pn_info_ptr->pre_sync_circ_buffer;
    dest_size = sizeof(sch_rx_circ_buffer);
  }

  /* -------------------------------------------------------------------------
  ** Before frame generation for every Forward Traffic Channel synchronization
  ** frame, the service option shall initialize the Traffic Channel
  ** pseudo-random number generator as follows: Refer to Section 3.7.2.1 of
  ** the TDSO standard for more information.
  ** -----------------------------------------------------------------------*/
  *seed_ptr = mar_ran_next(*seed_ptr);
  *seed_ptr = mar_ran_next(*seed_ptr);
  *seed_ptr = mar_ran_next(*seed_ptr);

  for (i=0; i<chan_pn_info_ptr->num_rand; i++)
  {
    if (dog_cnt++ == 8)
    {
      tdso_dog_report();
      dog_cnt = 0;
    }
    *seed_ptr = mar_ran_next(*seed_ptr); /* Get next ran number */
    rand_data = *seed_ptr >> 8;          /* Store 24 bits in frame */
    copy_size = memscpy(&buffer[j], dest_size - j, &rand_data, 3);   /* Store in Circular buffer*/
    if( copy_size != 3 )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, dest_size:%d, src_size: 3",
        copy_size,
        dest_size - j);
    }

    j += 3;                              /* Increment index by 3 */
  }

  if (channel == TDSO_SCH0)
  {
    // Set the Last Seed for future YN1 generations
    chan_pn_info_ptr->pre_sync_last_seed = *seed_ptr;
  }

} /* tdso_generate_rand_circ_buffer */

/*===========================================================================

FUNCTION : TDSO_PACK_DATABLK_FROM CIRC_BUFFER

DESCRIPTION:
  The bits from the circular buffer for a particular channel are
  packed serially into data blocks corresponding to the available
  MuxPDUs as determined by the connected multiplex option. The multiplex
  option indicates the size of the data block or data block(s), which is
  equal to the number of bits to be copied from the circular buffer to the
  last whole octet to form a data block or data blocks.

DEPENDENCIES:
  This function needs the Number of PDUs in the block.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/
void tdso_pack_datablk_from_circ_buffer
(
 tdso_tx_frame_type* tx_frame_ptr,
 tdso_link_dir_type  direction
)
{
  tdso_buffer_info *chan_pn_info_ptr; /* Pointer to channel PN info */
  word byte_count, frame_size, num_erasure_pdu ; /* Requested Frame size */
  word             sch_pdu_type;      /* Type of PDU being used on the SCH */
  const caii_srv_cfg_type  *config;
  word for_mux;
  size_t copy_size;      /*To store the value returned by memscpy */

  /* Set the MUX Options */
  config = snm_get_current_config();  // Get current configuration
  for_mux = config->for_sch[0].for_sch_mux; // Set FOR SCH mux option

  num_erasure_pdu = 1;
  frame_size = tx_frame_ptr->max_frame_size;    /* Set the frame size */


  if ((tx_frame_ptr->fr_channel == TDSO_FCH)
      || (tx_frame_ptr->fr_channel == TDSO_DCCH)
     )
  {
    if (tx_frame_ptr->fr_channel == TDSO_DCCH)
    {
      chan_pn_info_ptr = &dcch_parm.pn_info[direction];
    }
    else
    {
      chan_pn_info_ptr = &fch_parm.pn_info[direction];
    }

    if (tx_frame_ptr->fr_rate != TDSO_RATE_1)
    {
      /* TDSO only generates FULL rate frames to transmit on FCH */
      frame_size = tdso_data_len[TDSO_RATE_1][TDSO_RATE_SET_1];
    }

    /* ------------------------------------------------------------------
    ** Pack the data block with Circular buffer data only if TDSO is ON
    ** ----------------------------------------------------------------*/
    if (chan_pn_info_ptr->tdso_state)
    {
      /* Number of bytes from reference point to the end of Circular buffer */
      byte_count =
        (word) (chan_pn_info_ptr->num_rand*3 - chan_pn_info_ptr->data_offset);

      if (frame_size/8 > byte_count)
      {
        /* Pack the FCH TDSO data block from the Circular buffer */
        copy_size = memscpy(tx_frame_ptr->fr_item_ptr,
                            MAX_PDU_LEN*sizeof(uint8),
                            &chan_pn_info_ptr->circ_buffer[chan_pn_info_ptr->data_offset],
                            byte_count);
        if( copy_size != byte_count )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            byte_count,
            MAX_PDU_LEN*sizeof(uint8));
        }

        copy_size = memscpy(&tx_frame_ptr->fr_item_ptr[byte_count],
                            (MAX_PDU_LEN-byte_count)*sizeof(uint8),
               chan_pn_info_ptr->circ_buffer, (frame_size/8 - byte_count));
        if( copy_size != (frame_size/8 - byte_count) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            (frame_size/8-byte_count),
            (MAX_PDU_LEN-byte_count)*sizeof(uint8));
        }
      }
      else
      {
        /* Pack the FCH TDSO data block from the Circular buffer */
        copy_size = memscpy(tx_frame_ptr->fr_item_ptr,
                            MAX_PDU_LEN*sizeof(uint8),
                            &chan_pn_info_ptr->circ_buffer[chan_pn_info_ptr->data_offset],
                            frame_size/8 );
        if( copy_size != (frame_size/8) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            frame_size/8,
            MAX_PDU_LEN*sizeof(uint8));
        }
      }

      /* update the offset for circular buffer for the next frame */
      chan_pn_info_ptr->data_offset += frame_size/8;
    }
  }
  else if (tx_frame_ptr->fr_channel == TDSO_SCH0)
  {
    chan_pn_info_ptr = &sch0_parm.pn_info[direction];

    /* Make sure the requested tdso frame rate is valid for SCHO */
    if ((tx_frame_ptr->fr_rate != TDSO_RATE_2) &&
        (tx_frame_ptr->fr_rate != TDSO_RATE_1) &&
#ifdef FEATURE_IS2000_REL_A
        (tx_frame_ptr->fr_rate != TDSO_RATE_3) &&
#endif
        (tx_frame_ptr->fr_rate != TDSO_BLANK) &&
        (tx_frame_ptr->fr_rate != TDSO_ERASURE))
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid SCH TDSO Rate type = %d",
        tx_frame_ptr->fr_rate);
      return;
    }

    if (chan_pn_info_ptr->tdso_state)
    {
      byte_count =
        (word) (chan_pn_info_ptr->num_rand*3 - chan_pn_info_ptr->data_offset);

      if (tx_frame_ptr->max_frame_size/8 > byte_count)
      {
        /* Pack the SCH TDSO data block from the Circular buffer */
        copy_size = memscpy(tx_frame_ptr->fr_item_ptr, MAX_PDU_LEN*sizeof(uint8),
                            &chan_pn_info_ptr->circ_buffer[chan_pn_info_ptr->data_offset],
                            byte_count);
        if( copy_size != byte_count )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            byte_count,
            MAX_PDU_LEN*sizeof(uint8));
        }

        copy_size = memscpy(&tx_frame_ptr->fr_item_ptr[byte_count],
                            (MAX_PDU_LEN-byte_count)*sizeof(uint8),
               chan_pn_info_ptr->circ_buffer,
               (tx_frame_ptr->max_frame_size/8 - byte_count));
        if( copy_size != (tx_frame_ptr->max_frame_size/8 - byte_count) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            (tx_frame_ptr->max_frame_size/8-byte_count),
            (MAX_PDU_LEN-byte_count)*sizeof(uint8));
        }
      }
      else
      {
        /* Pack the SCH TDSO data block from the Circular buffer */
        copy_size = memscpy(tx_frame_ptr->fr_item_ptr, MAX_PDU_LEN*sizeof(uint8),
               &chan_pn_info_ptr->circ_buffer[chan_pn_info_ptr->data_offset],
               tx_frame_ptr->max_frame_size/8 );
        if( copy_size != tx_frame_ptr->max_frame_size/8  )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            tx_frame_ptr->max_frame_size/8,
            MAX_PDU_LEN*sizeof(uint8));
        }
      }

      /* This is a SCH frame and an Erasure, Set the maximum length
      ** to either Single PDU Full Rate or Double PDU Full Rate frame
      ** based on the Current MUX configuration
          */
      if ((direction == FWD_LINK) &&
          ((tx_frame_ptr->fr_rate  == TDSO_ERASURE) ||
           (tx_frame_ptr->fr_rate == TDSO_BLANK)))
      {

        M1X_MSG( DCP, LEGACY_MED,
          "For_MUX = %d",
          for_mux);

        if ((for_mux >> 8) == 0x8)
        {
          tx_frame_ptr->max_frame_size =
            tdso_data_len[TDSO_RATE_1][TDSO_RATE_SET_1];

          M1X_MSG( DCP, LEGACY_MED,
            "SCH Erasure, Single PDU");
          sch_pdu_type = 0x1;
        }
        else if ((for_mux >> 8) == 0x9)
        {
          tx_frame_ptr->max_frame_size =
            tdso_data_len[TDSO_RATE_2][TDSO_RATE_SET_1];
          sch_pdu_type = 0x2;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid for MUX");
          return;
        }

        num_erasure_pdu = (word) ((pow((double)2,
          (double) tx_frame_ptr->sch_rate) / sch_pdu_type));
        M1X_MSG( DCP, LEGACY_ERROR,
          "Moving Ref By PDU %d, size %d",
          num_erasure_pdu,
          num_erasure_pdu*tx_frame_ptr->max_frame_size/8);
      }

      /* Update the data block byte offset pointer in Circular Buffer */
      chan_pn_info_ptr->data_offset +=
        (uint16) (num_erasure_pdu * (tx_frame_ptr->max_frame_size/8));
    }

    /* Wrap it up if the PDU length exceeds the maximum SCH buffer */
    chan_pn_info_ptr->data_offset = (uint16) (chan_pn_info_ptr->data_offset %
                                    (chan_pn_info_ptr->num_rand * 3));
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Unsupported TDSO Physical Channel ");
  }

} /* tdso_pack_datablk_from_circ_buffer */

 /*===========================================================================

FUNCTION : TDSO_UPDATE_PN_OFFSET_INFO

DESCRIPTION:
  This function generates the Random Number for the next frame, computes the
  offset based on the yn1 value and sets the the data offset pointer
  (reference pointer) in the circular buffer from where the new frame is
  packed. It is only used for full TDSO.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void tdso_update_pn_offset_info
(
  tdso_buffer_info *chan_pn_info_ptr
)
{
  if (tdso_so == CAI_SO_TDSO)
  {
    return;
  }

  /* Genereate the PN for the next frame for ON/OFF determination */
  chan_pn_info_ptr->pn = mar_ran_next (chan_pn_info_ptr->pn);

  chan_pn_info_ptr->last_seed = mar_ran_next (chan_pn_info_ptr->last_seed);
  chan_pn_info_ptr->yn1 = chan_pn_info_ptr->last_seed >> 8;
  chan_pn_info_ptr->pn_offset = (uint16) (((byte)chan_pn_info_ptr->yn1 & 0x3f)
                                % (chan_pn_info_ptr->num_rand * 3));
  chan_pn_info_ptr->data_offset += chan_pn_info_ptr->pn_offset;
  chan_pn_info_ptr->data_offset = (uint16) (chan_pn_info_ptr->data_offset %
                                  (chan_pn_info_ptr->num_rand * 3));

} /* tdso_update_pn_offset_info */

/*===========================================================================

FUNCTION : TDSO_SET_NUM_RAND

DESCRIPTION:
  Sets the NUM_RAND value for FCH and SCH0 based on the RC Values.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void tdso_set_num_rand
(
  tdso_channel_type channel,
  tdso_link_dir_type direction
)
{
  tdso_buffer_info *chan_pn_info_ptr;   /* Pointer to channel pn info */
  uint16           num_circ_buffer = 2; /* By default, Two full Frames */
  uint16           num_circ_bits = 0;   /* Number of circular buffer bits */

  /* ---------------------
  ** FCH / DCCH processing
  ** --------------------- */

  if ((channel == TDSO_FCH)
      || (channel == TDSO_DCCH)
     )
  {
    if (channel == TDSO_DCCH)
    {
      chan_pn_info_ptr = &dcch_parm.pn_info[direction];
    }
    else
    {
      chan_pn_info_ptr = &fch_parm.pn_info[direction];
    }

    if (chan_pn_info_ptr->tst_par.frame_src == 0x1)
    {
      if (chan_pn_info_ptr->tst_par.num_circ_buf_frames <= 50)
      {
        num_circ_buffer = chan_pn_info_ptr->tst_par.num_circ_buf_frames;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unsupported # of Circ Buffer frames %d",
          chan_pn_info_ptr->tst_par.num_circ_buf_frames);
      }
    }

    /* Forward Link */
    if (direction == FWD_LINK)
    {
      /* Check RC and set the NUM RAND properly */
      /*lint -e641 */
      if ((chan_pn_info_ptr->rc == CAI_RC_1) ||
          (chan_pn_info_ptr->rc == CAI_RC_3) ||
          (chan_pn_info_ptr->rc == CAI_RC_4)
          #ifdef FEATURE_IS2000_1X_ADV
          || (chan_pn_info_ptr->rc == CAI_RC_11)
          #endif
         )
      /*lint +e641 */
      {
        /* Set the Maximum number of circular frame buffer bits */
        num_circ_bits = (uint16) (FCH_RC1_FRAME_BITS * num_circ_buffer);
      }
      else if ((chan_pn_info_ptr->rc == CAI_RC_2) || /*lint !e641 */
               (chan_pn_info_ptr->rc == CAI_RC_5))   /*lint !e641 */
      {
        /* Set the Maximum number of circular frame buffer bits */
        num_circ_bits = (uint16) (FCH_RC2_FRAME_BITS * num_circ_buffer);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unsupported TDSO FWD RC %d",
          chan_pn_info_ptr->rc);
      }
    }
    else /* Reverse Link */
    {
      /* Check RC and set the NUM RAND properly */
      /*lint -e641 */
      if ((chan_pn_info_ptr->rc == CAI_RC_1) ||
          (chan_pn_info_ptr->rc == CAI_RC_3) ||
          (chan_pn_info_ptr->rc == CAI_RC_5)
          #ifdef FEATURE_IS2000_1X_ADV
          || (chan_pn_info_ptr->rc == CAI_RC_8)
          #endif
         )
      /*lint +e641 */
      {
        /* Set the Maximum number of circular frame buffer bits */
        num_circ_bits = (uint16) (FCH_RC1_FRAME_BITS * num_circ_buffer);
      }
      /*lint -e641 */
      else if ((chan_pn_info_ptr->rc == CAI_RC_2) ||
               (chan_pn_info_ptr->rc == CAI_RC_4))
      /*lint +e641 */
      {
        /* Set the Maximum number of circular frame buffer bits */
        num_circ_bits = (uint16) (FCH_RC1_FRAME_BITS * num_circ_buffer);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unsupported TDSO REV RC %d",
          chan_pn_info_ptr->rc);
      }
    }

    /* Update the NUM_RAND */
    chan_pn_info_ptr->num_rand = num_circ_bits / NUM_PN_BITS;
    if (num_circ_bits % NUM_PN_BITS > 0 )
    {
      ++chan_pn_info_ptr->num_rand;
    }
    M1X_MSG( DCP, LEGACY_MED,
      "FCH: dir %d circ fr %d NUM_RAND %d",
      direction,
      num_circ_buffer,
      chan_pn_info_ptr->num_rand);
  }

  /* --------------
  ** SCH processing
  ** -------------- */
  else if (channel == TDSO_SCH0)
  {
    chan_pn_info_ptr = &sch0_parm.pn_info[direction];

    if ((chan_pn_info_ptr->tst_par.frame_src == 0x1) &&
        (chan_pn_info_ptr->tst_par.num_circ_buf_frames > 0))
    {
      num_circ_buffer = chan_pn_info_ptr->tst_par.num_circ_buf_frames;
    }

    switch( chan_pn_info_ptr->rc )
    {
      case 3:
        num_circ_bits = (direction == FWD_LINK) ?
                        (uint16) (FSCH_RC3_FRAME_BITS * num_circ_buffer):
                        (uint16) (RSCH_RC3_FRAME_BITS * num_circ_buffer);
        break;
      case 4:
        num_circ_bits = (direction == FWD_LINK) ?
                        (uint16) (FSCH_RC4_FRAME_BITS * num_circ_buffer):
                        (uint16) (RSCH_RC4_FRAME_BITS * num_circ_buffer);

        break;
      case 5:
        num_circ_bits = (direction == FWD_LINK) ?
                        (uint16) (FSCH_RC5_FRAME_BITS * num_circ_buffer):
                        (uint16) (RSCH_RC5_FRAME_BITS * num_circ_buffer);
        break;

      #ifdef FEATURE_IS2000_1X_ADV
      /* For REV RC8, the number of bits/frame is same as that for REV RC3 */
      case 8:
        if (direction == REV_LINK)
        {
          num_circ_bits = (uint16) (RSCH_RC3_FRAME_BITS * num_circ_buffer);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unsupported RC %d for SCH",
            chan_pn_info_ptr->rc);
        }
        break;

      /* For FWD RC11, the number of bits/frame is same as that for FWD RC4 */
      case 11:
        if (direction == FWD_LINK)
        {
          num_circ_bits = (uint16) (FSCH_RC4_FRAME_BITS * num_circ_buffer);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unsupported RC %d for SCH",
            chan_pn_info_ptr->rc);
        }
        break;
      #endif /* FEATURE_IS2000_1X_ADV */

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unsupported RC %d for SCH",
          chan_pn_info_ptr->rc);
        break;
    }

    /* Update the NUM_RAND */
    chan_pn_info_ptr->num_rand = num_circ_bits / NUM_PN_BITS;
    if (num_circ_bits % NUM_PN_BITS > 0)
    {
      ++chan_pn_info_ptr->num_rand;
    }

    M1X_MSG( DCP, LEGACY_MED,
      "SCH: dir %d circ fr %d NUM_RAND %d",
      direction,
      chan_pn_info_ptr->tst_par.num_circ_buf_frames,
      chan_pn_info_ptr->num_rand);
  }
} /* tdso_set_num_rand */

/*===========================================================================

FUNCTION : TDSO_COPY_COUNTER_BUFFER

DESCRIPTION:
  If the COPY_COUNTERS field is equal to '1', the mobile station
  shall copy the counters associated with the specified reverse
  Traffic Channel/Forward Traffic Channels as determined by the
  channel configuration.

DEPENDENCIES:
  This copy occurs at the Synchronization time.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/
void tdso_copy_counter_buffer(tdso_link_dir_type direction)
{

  const caii_srv_cfg_type *config;    /* Pointer to current config info */
  /* Get the Current Configuration information from the SNM */
  size_t copy_size;
  /* Return value of memscpy function */

  config = snm_get_current_config();

  if (config->dcch_cc_incl)
  {
    /* Copy the DCCH counters if cpy_cnts field is set */
    if (dcch_parm.pn_info[direction].tst_par.cpy_cnts)
    {
      /* -----------------------------------------------------------------
      ** Copy the Reverse Link counters if the channel direction is either
      ** REV_LNK_ONLY or FWD_AND_REV_LNK
      ** ----------------------------------------------------------------- */
      if (direction == REV_LINK)
      {
        /* Copy the DCCH reverse Link counters */
        copy_size = memscpy(dcch_tx_rpt, sizeof(dcch_tx_rpt),
                            dcch_tx_table, sizeof(dcch_tx_table));
        if( copy_size != sizeof(dcch_tx_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(dcch_tx_table),
            sizeof(dcch_tx_rpt));
        }

        copy_size = memscpy(dcch_tx_blank_rpt, sizeof(dcch_tx_blank_rpt),
                            dcch_tx_blank_table, sizeof(dcch_tx_blank_table));
        if( copy_size != sizeof(dcch_tx_blank_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(dcch_tx_blank_table),
            sizeof(dcch_tx_blank_rpt));
        }

      }
      else
      /* -------------------------------------------------------------------
      ** Copy the Forward Link counters if the channel direction is either
      ** FWD_LNK_ONLY or FWD_AND_REV_LNK
      ** ------------------------------------------------------------------*/
      {
        copy_size = memscpy(dcch_rx_rpt, TDSO_MAX_RATE*sizeof(uint32),
                            dcch_rx_table, sizeof(dcch_rx_table));
        if( copy_size != sizeof(dcch_rx_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(dcch_rx_table),
            TDSO_MAX_RATE*sizeof(uint32));
        }

        copy_size = memscpy(dcch_rx_null_rpt, sizeof(dcch_rx_null_rpt),
                            dcch_rx_null_table, sizeof(dcch_rx_null_table));
        if( copy_size != sizeof(dcch_rx_null_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(dcch_rx_null_table),
            sizeof(dcch_rx_null_rpt));
        }

      }

      /* Reset the cpy_cnts filed to 0 */
      dcch_parm.pn_info[direction].tst_par.cpy_cnts = 0;
    }
  }
  else   /* must be FCH */
  {

    /* Copy the FCH counters if cpy_cnts field is set */
    if (fch_parm.pn_info[direction].tst_par.cpy_cnts)
    {
      /* -----------------------------------------------------------------
      ** Copy the Reverse Link counters if the channel direction is either
      ** REV_LNK_ONLY or FWD_AND_REV_LNK
      ** ----------------------------------------------------------------- */
      if (direction == REV_LINK)
      {
        /* Copy the FCH reverse Link counters */
        copy_size = memscpy(fch_tx_rpt, sizeof(fch_tx_rpt),
                            fch_tx_table, sizeof(fch_tx_table));
        if( copy_size != sizeof(fch_tx_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(fch_tx_table),
            sizeof(fch_tx_rpt));
        }

        copy_size = memscpy(fch_tx_blank_rpt, sizeof(fch_tx_blank_rpt),
                            fch_tx_blank_table, sizeof(fch_tx_blank_table));
        if( copy_size != sizeof(fch_tx_blank_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(fch_tx_blank_table),
            sizeof(fch_tx_blank_rpt));
        }


      }
      else
      /* -------------------------------------------------------------------
      ** Copy the Forward Link counters if the channel direction is either
      ** FWD_LNK_ONLY or FWD_AND_REV_LNK
      ** ------------------------------------------------------------------*/
      {
        copy_size = memscpy(fch_rx_rpt, TDSO_MAX_RATE*sizeof(uint32),
                            fch_rx_table, sizeof(fch_rx_table));
        if( copy_size != sizeof(fch_rx_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(fch_rx_table),
            TDSO_MAX_RATE*sizeof(uint32));
        }
        copy_size = memscpy(fch_rx_null_rpt, sizeof(fch_rx_null_rpt),
                            fch_rx_null_table, sizeof(fch_rx_null_table));
        if( copy_size != sizeof(fch_rx_null_table) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            sizeof(fch_rx_null_table),
            sizeof(fch_rx_null_rpt));
        }
      }

      /* Reset the cpy_cnts filed to 0 */
      fch_parm.pn_info[direction].tst_par.cpy_cnts = 0;
    }
  }

  /* Copy the SCH0 counters if cpy_cnts field is set */
  if (sch0_parm.pn_info[direction].tst_par.cpy_cnts)
  {
    /* -------------------------------------------------------------------
    ** Copy the Reverse Link counters if the channel direction is either
    ** REV_LNK_ONLY or FWD_AND_REV_LNK
    ** ------------------------------------------------------------------*/
    if (direction == REV_LINK)
    {
      /* Copy the FCH reverse Link counters */
      copy_size = memscpy(sch_tx_rpt, sizeof(sch_tx_rpt),
                          sch_tx_table, sizeof(sch_tx_table));
      if( copy_size != sizeof(sch_tx_table) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(sch_tx_table),
          sizeof(sch_tx_rpt));
      }
      sch_tx_blank_rpt = sch_tx_blank;
    }

    /* -------------------------------------------------------------------
    ** Copy the Forward Link counters if the channel direction is either
    ** FWD_LNK_ONLY or FWD_AND_REV_LNK
    ** ------------------------------------------------------------------*/
    else
    {
      /* Copy the PDU counter table to PDU buffer */
      copy_size = memscpy(sch_rx_pdu_buffer, sizeof(sch_rx_pdu_buffer),
                          sch_rx_pdu_table, sizeof(sch_rx_pdu_table));
      if( copy_size != sizeof(sch_rx_pdu_table) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(sch_rx_pdu_table),
          sizeof(sch_rx_pdu_buffer));
      }

      /* Copy the SCH receive frame counters from the table */
      copy_size = memscpy(sch_rx_rpt, sizeof(sch_rx_rpt),
                          sch_rx_table, sizeof(sch_rx_table));
      if( copy_size != sizeof(sch_rx_table) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(sch_rx_table),
          sizeof(sch_rx_rpt));
      }
      copy_size = memscpy(sch_rx_blank_rpt, sizeof(sch_rx_blank_rpt),
                          sch_rx_blank_table, sizeof(sch_rx_blank_table));
      if( copy_size != sizeof(sch_rx_blank_table) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(sch_rx_blank_table),
          sizeof(sch_rx_blank_rpt));
      }
    }

    /* Reset the cpy_cnts filed to 0 */
    sch0_parm.pn_info[direction].tst_par.cpy_cnts = 0;
  }
} /* tdso_copy_counter_buffer */

/*===========================================================================

FUNCTION : TDSO_CLEAR_COUNTER_BUFFER

DESCRIPTION:
  If the CLEAR_COUNTERS field is equal to '1', the mobile station
  shall clear the counters associated with the specified reverse
  Traffic Channel/Forward Traffic Channels as determined by the
  channel configuration.

DEPENDENCIES:
  This clear occurs at the Synchronization time.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/
void tdso_clear_counter_buffer(tdso_link_dir_type direction)
{

  /* Clear the DCCH counters if clr_cnts field is set */
  if (dcch_parm.pn_info[direction].tst_par.clr_cnts)
  {
    /* -------------------------------------------------------------------
    ** Clear the Reverse Link counters if the channel direction is either
    ** REV_LNK_ONLY or FWD_AND_REV_LNK
    ** ------------------------------------------------------------------*/
    if (direction == REV_LINK)
    {
      memset(dcch_tx_table, 0, sizeof(dcch_tx_table));
      memset(dcch_tx_blank_table, 0, sizeof(dcch_tx_blank_table));
    }
    /* -------------------------------------------------------------------
    ** Clear the Forward Link counters if the channel direction is either
    ** ------------------------------------------------------------------*/
    else
    {
      memset(dcch_rx_table, 0, sizeof(dcch_rx_table));
      memset(dcch_rx_null_table, 0, sizeof(dcch_rx_null_table));
    }

    // Reset the clr_cnts field back to 0
    dcch_parm.pn_info[direction].tst_par.clr_cnts = 0;
  }

  /* Clear the FCH counters if clr_cnts field is set */
  if (fch_parm.pn_info[direction].tst_par.clr_cnts)
  {
    /* -------------------------------------------------------------------
    ** Clear the Reverse Link counters if the channel direction is either
    ** REV_LNK_ONLY or FWD_AND_REV_LNK
    ** ------------------------------------------------------------------*/
    if (direction == REV_LINK)
    {
      memset(fch_tx_table, 0, sizeof(fch_tx_table));
      memset(fch_tx_blank_table, 0, sizeof(fch_tx_blank_table));
    }
    /* -------------------------------------------------------------------
    ** Clear the Forward Link counters if the channel direction is either
    ** ------------------------------------------------------------------*/
    else
    {
      memset(fch_rx_table, 0, sizeof(fch_rx_table));
      memset(fch_rx_null_table, 0, sizeof(fch_rx_null_table));
    }

    // Reset the clr_cnts field back to 0
    fch_parm.pn_info[direction].tst_par.clr_cnts = 0;
  }

  /* Clear the SCH0 counters if clr_cnts field is set */
  if (sch0_parm.pn_info[direction].tst_par.clr_cnts)
  {
    /* -------------------------------------------------------------------
    ** Clear the Reverse Link counters if the channel direction is either
    ** REV_LNK_ONLY or FWD_AND_REV_LNK
    ** ------------------------------------------------------------------*/
    if (direction == REV_LINK)
    {
      memset(sch_tx_table, 0, sizeof(sch_tx_table));
      sch_tx_blank = 0;
    }
    else
    /* -------------------------------------------------------------------
    ** Clear the Forward Link counters if the channel direction is either
    ** FWD_LNK_ONLY or FWD_AND_REV_LNK
    ** ------------------------------------------------------------------*/
    {
      memset(sch_rx_table, 0, sizeof(sch_rx_table));
      memset(sch_rx_blank_table, 0, sizeof(sch_rx_blank_table));
      // Clear the Received PDU counters.
      memset(sch_rx_pdu_table, 0, sizeof(sch_rx_pdu_table));
    }
    // Reset the clr_cnts field back to 0 */
    sch0_parm.pn_info[direction].tst_par.clr_cnts = 0;
  }

} /* tdso_clear_counter_buffer */

/*===========================================================================

FUNCTION : TDSO_UPDATE_ON_OFF_THRESHOLD

DESCRIPTION:
  It updates the ON/OFF threshholds for frame activity on both the FCH and
  SCH0 channels. It is only used for full TDSO.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

===========================================================================*/
void tdso_update_on_off_threshold
(
  tdso_chan_parms    *chan_parm_ptr,
  tdso_link_dir_type direction
)
{
  uint32 scaledVal;
  const uint32 threshScaler = 16777215;

  if (tdso_so == CAI_SO_TDSO)
  {
    return;
  }

  //Set the Source field
  chan_parm_ptr->pn_info[direction].data_src =
    chan_parm_ptr->pn_info[direction].tst_par.data_src;

  if (!chan_parm_ptr->pn_info[direction].data_src)
  {
    /* Fixed Pattern selected, set the Fixed Pattern */
    chan_parm_ptr->pn_info[direction].data_pattern =
      chan_parm_ptr->pn_info[direction].tst_par.data_pattern;
  }

  if (!chan_parm_ptr->pn_info[direction].tst_par.frame_act)
  {
    if ((chan_parm_ptr->pn_info[direction].tst_par.tx_on_period == 0) &&
        (chan_parm_ptr->pn_info[direction].tst_par.tx_off_period > 0))
    {
      // Set the TDSO state to OFF at sync frame
      chan_parm_ptr->pn_info[direction].tdso_state = FALSE;
    }
    else
    {
      // Set the TDSO state to ON at sync frame
      chan_parm_ptr->pn_info[direction].tdso_state = TRUE;
    }
    // Set Random Activity to Zero
    chan_parm_ptr->pn_info[direction].frame_act = 0;

    /* Deterministic frame activity */
    chan_parm_ptr->pn_info[direction].tx_on_period_cnt =
    chan_parm_ptr->pn_info[direction].tx_on_period =
    chan_parm_ptr->pn_info[direction].tst_par.tx_on_period;

    chan_parm_ptr->pn_info[direction].tx_off_period_cnt =
    chan_parm_ptr->pn_info[direction].tx_off_period =
    chan_parm_ptr->pn_info[direction].tst_par.tx_off_period;

    M1X_MSG( DCP, LEGACY_MED,
      "Direction %d, ON/OFF cnts : ON %d, OFF %d ",
      direction,
      chan_parm_ptr->pn_info[direction].tx_on_period_cnt,
      chan_parm_ptr->pn_info[direction].tx_off_period_cnt );
  }
  else
  {
    if (!chan_parm_ptr->pn_info[direction].pn_valid)
    {
      chan_parm_ptr->pn_info[direction].tdso_state = FALSE;
    }

    // Set Random Frame Activity
    chan_parm_ptr->pn_info[direction].frame_act = 1;

    /* Random activity with specified frame activity and burst length */
    scaledVal = chan_parm_ptr->pn_info[direction].tst_par.off_to_on_prob
                * threshScaler;
    chan_parm_ptr->pn_info[direction].tx_on_thresh = scaledVal / 100;
    if ((scaledVal % 100) >= 50)
    {
      ++chan_parm_ptr->pn_info[direction].tx_on_thresh;
    }
    scaledVal = chan_parm_ptr->pn_info[direction].tst_par.on_to_off_prob *
                threshScaler;
    chan_parm_ptr->pn_info[direction].tx_off_thresh = scaledVal / 100;
    if ((scaledVal % 100) >= 50)
    {
      ++chan_parm_ptr->pn_info[direction].tx_off_thresh;
    }

    M1X_MSG( DCP, LEGACY_MED,
      "Act Thresh: ON %x, OFF %x",
      chan_parm_ptr->pn_info[direction].tx_on_thresh,
      chan_parm_ptr->pn_info[direction].tx_off_thresh);
  }

} /* tdso_update_on_off_threshold */

/*===========================================================================

FUNCTION : TDSO_UPDATE_FCH_TX_STATS

DESCRIPTION:
  This function updates the FCH transmission statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
 This function depends on the FCH TDSO STATE parameter.

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

/*lint -e818*/
void tdso_update_fch_tx_stats
(
  tdso_tx_frame_type *tx_frame_ptr,
  boolean            tdso_state
)
{
  if (tdso_state) /* TDSO state is ON */
  {
    if (tx_frame_ptr->fr_rate == TDSO_RATE_1)
    {
      fch_tx_table[FDCHTX_E1_TRAFFIC]++;
    }
    else if (tx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      fch_tx_table[FDCHTX_E1_BLANK]++;
    }
    else
    {
      fch_tx_table[FDCHTX_E1_DIM_BURST]++;
    }
  }
  else /* TDSO stat is off */
  {
    if (tx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      fch_tx_blank_table[FDCHTX_BLANK_BLANK]++;
    }
    else
    {
      fch_tx_blank_table[FDCHTX_BLANK_OTHER]++;
    }
  }

} /* tdso_update_fch_tx_stats */

/*===========================================================================

FUNCTION : TDSO_UPDATE_SCH_TX_STATS

DESCRIPTION:
  This function updates the SCH0 transmission statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
  This function depends on the SCH0 TDSO STATE parameter.

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void tdso_update_sch_tx_stats
(
  tdso_tx_frame_type* tx_frame_ptr,
  boolean             tdso_state
)
{
  /* Update the SCH PDU counters */
  tdso_sch.sch_tx_rate = tx_frame_ptr->sch_rate;

  /* check the valid SCH TX rates for SCH0 transmit statistics */
  if (tdso_sch.sch_tx_rate >= TDSO_SCH_RATE_BLANK)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid SCH TX RATE");
      return;
  }

  if (tdso_state) /* TDSO state is ON */
  {
    if (tx_frame_ptr->fr_rate != TDSO_BLANK)
    {
      sch_tx_table[SCHTX_EN_TRAFFIC][tdso_sch.sch_tx_rate]++;
    }
    else
    {
      sch_tx_table[SCHTX_EN_BLANK][tdso_sch.sch_tx_rate]++;
    }
  }
  else
  {
    sch_tx_blank++;
  }

} /* tdso_update_sch_tx_stats */

/*===========================================================================

FUNCTION : TDSO_UPDATE_FCH_RX_STATS

DESCRIPTION:
  This function updates the FCH Receive statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
  This function depends on the FCH TDSO STATE parameter.

RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/
void tdso_update_fch_rx_stats
(
  tdso_rx_frame_type *rx_frame_ptr,
  boolean            tdso_state
)
{
  if (tdso_state) /* TDSO state is ON */
  {
    /* check the valid range of foward rate */
    if  (rx_frame_ptr->fr_rate >= TDSO_MAX_RATE)
    {
        M1X_MSG( DCP, LEGACY_HIGH,
          "FR Rate is out of range");
        return;
    }

    if (tdso_so == CAI_SO_FULL_TDSO)
    {
      /* For FTDSO, 1/2, 1/4 and 1/8 rate frames are updated as DIM-BURST */
      if ((rx_frame_ptr->fr_rate == TDSO_RATE_HALF) ||
          (rx_frame_ptr->fr_rate == TDSO_RATE_QUARTER) ||
          (rx_frame_ptr->fr_rate == TDSO_RATE_EIGHTH))
      {
        fch_rx_table[TDSO_DIM_BURST]++;
        M1X_MSG( DCP, LEGACY_HIGH,
          "TDSO ON: Received DIM & BURST %d",
          rx_frame_ptr->fr_rate);
      }
      else
      {
        fch_rx_table[rx_frame_ptr->fr_rate]++;
      }
    }
    /* For STDSO, update counters individually for 1/2, 1/4 & 1/8th rates */
    else
    {
      fch_rx_table[rx_frame_ptr->fr_rate]++;
    }
  }
  else /* TDSO state is OFF */
  {
    if ((rx_frame_ptr->fr_rate == TDSO_NULL) ||
        (rx_frame_ptr->fr_rate == TDSO_RATE_EIGHTH))
    {
      fch_rx_null_table[FDCHRX_NULL_NULL]++;
    }
    else if (rx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      fch_rx_null_table[FDCHRX_NULL_BLANK]++;
    }
    else
    {
      fch_rx_null_table[FDCHRX_NULL_OTHER]++;
    }
  }

} /* tdso_update_fch_rx_stats */

/*===========================================================================

FUNCTION : TDSO_UPDATE_SCH_RX_STATS

DESCRIPTION:
  This function updates the SCH0 Receive statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
  This function depends on the SCH0 TDSO STATE parameter.

RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/
void tdso_update_sch_rx_stats
(
  tdso_rx_frame_type*  rx_frame_ptr,
  boolean              tdso_state
)
{
  tdso_sch.sch_rx_rate = rx_frame_ptr->sch_rate;

  /* check the valid SCH RX rates for SCH0 receive statistics */
   if (tdso_sch.sch_rx_rate >= TDSO_SCH_RATE_BLANK)
   {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid SCH RX RATE");
      return;
   }


  if (tdso_state) /* TDSO state is ON */
  {
    if ((rx_frame_ptr->fr_rate == TDSO_RATE_1) ||
#ifdef FEATURE_IS2000_REL_A
        (rx_frame_ptr->fr_rate == TDSO_RATE_3) ||
#endif
        (rx_frame_ptr->fr_rate == TDSO_RATE_2))
    {
      sch_rx_table[SCHRX_EN_TRAFFIC][tdso_sch.sch_rx_rate]++;
    }
    else if (rx_frame_ptr->fr_rate == TDSO_BIT_ERROR)
    {
      sch_rx_table[SCHRX_EN_FRAME_ERROR][tdso_sch.sch_rx_rate]++;
    }
    else if (rx_frame_ptr->fr_rate == TDSO_ERASURE)
    {
      sch_rx_table[SCHRX_EN_ERASURE][tdso_sch.sch_rx_rate]++;
    }
    else if (rx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      sch_rx_table[SCHRX_EN_BLANK][tdso_sch.sch_rx_rate]++;
    }
  }
  else /* TDSO state is OFF */
  {
    if (rx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      sch_rx_blank_table[SCHRX_BLANK_BLANK]++;
    }
    else
    {
      sch_rx_blank_table[SCHRX_BLANK_OTHER]++;
    }
  }

}  /* tdso_update_sch_rx_stats */

/*===========================================================================

FUNCTION : TDSO_UPDATE_SCH_PDU_STATS

DESCRIPTION:
  This function updates the SCH0 PDU statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
  This function depends on the SCH0 TDSO STATE parameter.

RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/
void tdso_update_sch_pdu_stats
(
  tdso_rx_frame_type* rx_frame_ptr
)
{
  word pdu_type;

  if ( rx_frame_ptr == NULL )
  {
    return;
  }

  if (tdso_so == CAI_SO_TDSO)
  {
    return;
  }

  /* Set the PDU type */
  if (rx_frame_ptr->fr_rate == TDSO_RATE_2)
  {
    pdu_type = TDSO_RATE_2_PDU;
  }
#ifdef FEATURE_IS2000_REL_A
  else if (rx_frame_ptr->fr_rate == TDSO_RATE_3)
  {
    pdu_type = TDSO_RATE_3_PDU;
  }
#endif
  else
  {
    pdu_type = TDSO_RATE_1B_PDU;
  }

  if (tdso_sch.sch_rx_rate == TDSO_SCH_RATE_1X)
  {
    pdu_type = TDSO_RATE_1A_PDU;
  }

  /* Update the PDU table based on the received SCH TDSO frame */

  if (rx_frame_ptr->fr_rate == TDSO_BIT_ERROR)
  {
    sch_rx_pdu_table[pdu_type][SCHRX_EN_FRAME_ERROR]++;
  }
  else if (rx_frame_ptr->fr_rate == TDSO_ERASURE)
  {
    sch_rx_pdu_table[pdu_type][SCHRX_EN_ERASURE]++;
  }
  else  /* MS has received a good SCH PDU frame */
  {
    sch_rx_pdu_table[pdu_type][SCHRX_EN_TRAFFIC]++;
  }

} /* tdso_update_sch_pdu_stats */
/*lint +e818*/

/*===========================================================================

FUNCTION : TDSO_UPDATE_TEST_PARAMETER

DESCRIPTION
  This function updates the corresponding channel's TDSO test parameters
  based on SOCM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdso_update_test_parameter
(
  tdso_tst_par_type tst_par   /* Test parameters */
)
{
  /* ----------------------------------------------------------------------
  ** Update FCH and/or SCH0 TDSO test parameters based on type of directive
  ** ---------------------------------------------------------------------- */
  switch (tst_par.ctl_type)
  {
    /* All physical channels */
    case TDSO_CTL_ALL:

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||  /*lint !e641 */
          (tst_par.ch_dir == FWD_LNK_ONLY))  /*lint !e641 */
      {
        if (socm_deferred)
        {
          fch_parm.pn_info[FWD_LINK].socm_deferred = TRUE;
          fch_parm.pn_info[FWD_LINK].pending_tst_par = tst_par;

          dcch_parm.pn_info[FWD_LINK].socm_deferred = TRUE;
          dcch_parm.pn_info[FWD_LINK].pending_tst_par = tst_par;

          sch0_parm.pn_info[FWD_LINK].socm_deferred = TRUE;
          sch0_parm.pn_info[FWD_LINK].pending_tst_par = tst_par;
        }
        else
        {
          fch_parm.pn_info[FWD_LINK].tst_par = tst_par;

          dcch_parm.pn_info[FWD_LINK].tst_par = tst_par;

          sch0_parm.pn_info[FWD_LINK].tst_par = tst_par;
        }
      }

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||  /*lint !e641 */
          (tst_par.ch_dir == REV_LNK_ONLY))  /*lint !e641 */
      {
        if (socm_deferred)
        {
          fch_parm.pn_info[REV_LINK].socm_deferred = TRUE;
          fch_parm.pn_info[REV_LINK].pending_tst_par = tst_par;

          dcch_parm.pn_info[REV_LINK].socm_deferred = TRUE;
          dcch_parm.pn_info[REV_LINK].pending_tst_par = tst_par;

          sch0_parm.pn_info[REV_LINK].socm_deferred = TRUE;
          sch0_parm.pn_info[REV_LINK].pending_tst_par = tst_par;
        }
        else
        {
          fch_parm.pn_info[REV_LINK].tst_par = tst_par;

          dcch_parm.pn_info[REV_LINK].tst_par = tst_par;

          sch0_parm.pn_info[REV_LINK].tst_par = tst_par;
        }
      }
      break;

    /* FCH */
    case TDSO_CTL_FCH:

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||   /*lint !e641 */
          (tst_par.ch_dir == FWD_LNK_ONLY))   /*lint !e641 */
      {
        if (socm_deferred)
        {
          fch_parm.pn_info[FWD_LINK].socm_deferred = TRUE;
          fch_parm.pn_info[FWD_LINK].pending_tst_par = tst_par;
        }
        else
        {
          fch_parm.pn_info[FWD_LINK].tst_par = tst_par;
        }
      }

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||    /*lint !e641 */
          (tst_par.ch_dir == REV_LNK_ONLY))  /*lint !e641 */
      {
        if (socm_deferred)
        {
          fch_parm.pn_info[REV_LINK].socm_deferred = TRUE;
          fch_parm.pn_info[REV_LINK].pending_tst_par = tst_par;
        }
        else
        {
          fch_parm.pn_info[REV_LINK].tst_par = tst_par;
        }
      }
      break;

    /* DCCH */
    case TDSO_CTL_DCCH:

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||   /*lint !e641 */
          (tst_par.ch_dir == FWD_LNK_ONLY))   /*lint !e641 */
      {
        if (socm_deferred)
        {
          dcch_parm.pn_info[FWD_LINK].socm_deferred = TRUE;
          dcch_parm.pn_info[FWD_LINK].pending_tst_par = tst_par;
        }
        else
        {
          dcch_parm.pn_info[FWD_LINK].tst_par = tst_par;
        }
      }

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||   /*lint !e641 */
          (tst_par.ch_dir == REV_LNK_ONLY))   /*lint !e641 */
      {
        if (socm_deferred)
        {
          dcch_parm.pn_info[REV_LINK].socm_deferred = TRUE;
          dcch_parm.pn_info[REV_LINK].pending_tst_par = tst_par;
        }
        else
        {
          dcch_parm.pn_info[REV_LINK].tst_par = tst_par;
        }
      }
      break;

    /* SCH 0 */
    case TDSO_CTL_SCH0:

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||   /*lint !e641 */
          (tst_par.ch_dir == FWD_LNK_ONLY))   /*lint !e641 */
      {
        if (socm_deferred)
        {
          sch0_parm.pn_info[FWD_LINK].socm_deferred = TRUE;
          sch0_parm.pn_info[FWD_LINK].pending_tst_par = tst_par;
        }
        else
        {
          sch0_parm.pn_info[FWD_LINK].tst_par = tst_par;
        }
      }

      if ((tst_par.ch_dir == FWD_AND_REV_LNK) ||  /*lint !e641 */
          (tst_par.ch_dir == REV_LNK_ONLY))   /*lint !e641 */
      {
        if (socm_deferred)
        {
          sch0_parm.pn_info[REV_LINK].socm_deferred = TRUE;
          sch0_parm.pn_info[REV_LINK].pending_tst_par = tst_par;
        }
        else
        {
          sch0_parm.pn_info[REV_LINK].tst_par = tst_par;
        }
      }
      break;

    default:
      break;
  }

} /* tdso_update_test_parameter */

/*===========================================================================

FUNCTION : TDSO_PROCESS_SOCM_MSG

DESCRIPTION:
  This function processes the received SOCM. It validates the content of the
  message. If the SOCM is received within the 1 sec boundary of SYNC frame
  and its content is valid, it saves the message content in TDSO_SOCM for
  later update of TDSO test parameters. Otherwise, it updates the TDSO test
  parameters right away if the content of SOCM is valid.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - valid SOCM.
  FALSE - invalid SOCM.

SIDE EFFECTS:
  None.

===========================================================================*/
boolean tdso_process_socm_msg
(
  uint8* tsf,
  uint8  tsf_len
)
{
  tdso_tst_par_type tst_par; /* Test parameters */
  uint8             i;       /* Index to test parameters array */

  memset(&tst_par, 0, sizeof(tst_par));

  /* ----------------------------------
  ** Retrieve Type of Control Directive
  ** ---------------------------------- */
  tst_par.ctl_type = tsf[TDSO_CTL_TYPE];

  /* ---------------------------------------------------
  ** Initialize index to byte index of channel direction
  ** --------------------------------------------------- */

  i = TDSO_CHN_DIR;

  if (i < tsf_len)
  {
    /* --------------------------------
    ** Retrieve Channel direction field
    ** -------------------------------- */
    tst_par.ch_dir = b_unpackb(&tsf[i], 0, 2);

    /* ----------------------------
    ** Retrieve Copy counters field
    ** ---------------------------- */
    tst_par.cpy_cnts = b_unpackb(&tsf[i], 2, 1);

    /* -----------------------------
    ** Retrieve Clear counters field
    ** ----------------------------- */
    tst_par.clr_cnts = b_unpackb(&tsf[i], 3, 1);

    /* Display the control parameters on DM */

    M1X_MSG( DCP, LEGACY_MED,
      "CH_DIR = %d,CPY_CNT= %d,CLR_CNT = %d",
      tst_par.ch_dir,
      tst_par.cpy_cnts,
      tst_par.clr_cnts);

    /* --------------------------
    ** Retrieve Data source field
    ** -------------------------- */
    tst_par.data_src = b_unpackb(&tsf[i], 4, 1);

    /* ---------------------------
    ** Retrieve Frame source field
    ** --------------------------- */
    tst_par.frame_src = b_unpackb(&tsf[i], 5, 2);

    /* -----------------------------
    ** Retrieve Frame activity field
    ** ----------------------------- */
    tst_par.frame_act = b_unpackb(&tsf[i], 7, 1);

    /* Display the control parameters on DM */
    M1X_MSG( DCP, LEGACY_MED,
      " RAND_SRC = %d, FRAME_SRC = %d, FRAME_ACT = %d ",
      tst_par.data_src,
      tst_par.frame_src,
      tst_par.frame_act);
  }

  if (i < (tsf_len - 1))
  {
    /* ---------------------------
    ** Retrieve Test options field
    ** --------------------------- */
    tst_par.tst_opt = tsf[++i];
  }

  /* ----------------------------------------------------------------
  ** Retrieve Number of full-rate frames in the circular buffer field
  ** ---------------------------------------------------------------- */
  if ((tdso_fr_src_type) tst_par.frame_src == N_FULL_RATE_FRAME)
  {
    if (i < (tsf_len - 1))
    {
      tst_par.num_circ_buf_frames = tsf[++i];
      M1X_MSG( DCP, LEGACY_MED,
        " NUM CIR BUFF = %d",
        tst_par.num_circ_buf_frames);
    }
  }
  else
  {
    tst_par.num_circ_buf_frames = 0;
  } /* end if (tst_par.data_src) */

  /* ------------------------------------------------------------------
  ** Retrieve "On" state to "Off" state transition probability, "Off"
  ** state to "On" state transition probability, Transmission on period
  ** and Transmission off period fields.
  ** ------------------------------------------------------------------ */
  if (tst_par.frame_act)
  {
    if (i < (tsf_len - 2))
    {
      tst_par.on_to_off_prob = tsf[++i];
      tst_par.off_to_on_prob = tsf[++i];
      M1X_MSG( DCP, LEGACY_MED,
        " ONtoOFF prob = %d, OFFtoON prob = %d ",
        tst_par.on_to_off_prob,
        tst_par.off_to_on_prob);
    }
  }
  else
  {
    if (i < (tsf_len - 2))
    {
      tst_par.tx_on_period = tsf[++i];
      tst_par.tx_off_period = tsf[++i];
      M1X_MSG( DCP, LEGACY_MED,
        " ON period= %d, OFF period = %d",
        tst_par.tx_on_period,
        tst_par.tx_off_period);
    }
  } /* end if (tst_par.frame_act) */

  /* ---------------------------
  ** Retrieve Data pattern field
  ** --------------------------- */
  tst_par.data_pattern = 0xff;
  if (!tst_par.data_src)
  {
    if (i < (tsf_len - 1))
    {
      tst_par.data_pattern = tsf[++i];
      M1X_MSG( DCP, LEGACY_MED,
        " Data Pattern = %d",
        tst_par.data_pattern);
    }
  }

  /* ---------------------------------------------------------------------
  ** If either one of the following condidition is true, fails the update.
  ** (1) Channel direction has value > 2.
  ** (2) Frame source has value > 2.
  ** (3) "On" state to "Off" state probability > 100.
  ** (4) "Off" state to "On" State probability > 100.
  ** (5) Circular buffer size > 2.
  ** (6) Excessive or insufficient test parameters specified.
  ** --------------------------------------------------------------------- */
  if ((tst_par.ch_dir > REV_LNK_ONLY) || (tst_par.frame_src > NEW_FRAME) ||  /*lint !e641 */
      (tst_par.on_to_off_prob > 100) || (tst_par.off_to_on_prob > 100) ||
      (((tdso_fr_src_type) tst_par.frame_src == N_FULL_RATE_FRAME) &&
       (tst_par.num_circ_buf_frames > 2)) ||
      (i != (tsf_len-1)))
  {
    return FALSE;
  }

  /* Rxed SOCM in 1 sec boundary of SYNC frame */
  if (socm_deferred)
  {
    /* Mark that we have deferred the update of TDSO test parameters from
       SOCM */
    socm_ignored = TRUE;
    M1X_MSG( DCP, LEGACY_MED,
      "Deferred update of TDSO test parameter");
  }
  else
  {
    /* Mark that we have updated the TDSO test parameters from SOCM */
    socm_ignored = FALSE;
  }

  /* Update TDSO test parameters */
  tdso_update_test_parameter(tst_par);

  return TRUE;

} /* tdso_process_socm_msg */

/*===========================================================================

FUNCTION : TDSO_SYNC_TEST_PARAMETER

DESCRIPTION
  This function updates the corresponding channel's TDSO test parameters
  based on deferred SOCM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdso_sync_test_parameter
(
  tdso_link_dir_type direction
)
{
  if (direction == FWD_LINK)
  {
    if (fch_parm.pn_info[FWD_LINK].socm_deferred)
    {
      fch_parm.pn_info[FWD_LINK].socm_deferred = FALSE;
      fch_parm.pn_info[FWD_LINK].tst_par = fch_parm.pn_info[FWD_LINK].pending_tst_par;
    }
    if (dcch_parm.pn_info[FWD_LINK].socm_deferred)
    {
      dcch_parm.pn_info[FWD_LINK].socm_deferred = FALSE;
      dcch_parm.pn_info[FWD_LINK].tst_par = dcch_parm.pn_info[FWD_LINK].pending_tst_par;
    }
    if (sch0_parm.pn_info[FWD_LINK].socm_deferred)
    {
      sch0_parm.pn_info[FWD_LINK].socm_deferred = FALSE;
      sch0_parm.pn_info[FWD_LINK].tst_par = sch0_parm.pn_info[FWD_LINK].pending_tst_par;
    }
  }
  else
  {
    if (fch_parm.pn_info[REV_LINK].socm_deferred)
    {
      fch_parm.pn_info[REV_LINK].socm_deferred = FALSE;
      fch_parm.pn_info[REV_LINK].tst_par = fch_parm.pn_info[REV_LINK].pending_tst_par;
    }
    if (dcch_parm.pn_info[REV_LINK].socm_deferred)
    {
      dcch_parm.pn_info[REV_LINK].socm_deferred = FALSE;
      dcch_parm.pn_info[REV_LINK].tst_par = dcch_parm.pn_info[REV_LINK].pending_tst_par;
    }
    if (sch0_parm.pn_info[REV_LINK].socm_deferred)
    {
      sch0_parm.pn_info[REV_LINK].socm_deferred = FALSE;
      sch0_parm.pn_info[REV_LINK].tst_par = sch0_parm.pn_info[REV_LINK].pending_tst_par;
    }
  }

} /* tdso_sync_test_parameter */

/*===========================================================================
FUNCTION     TDSO_NOTIFY_HHO_SUCCESS

DESCRIPTION
  This function is called by MC when a HHO has been successfully performed.
  If a full TDSO is in progress, this function sets the current TDSO state
  to FALSE so that all 1's are transmitted util the next SYNC frame.  The TDSO
  also sends the Service Option Control Message (SOCM) of the test parameters
  that were used before the Handoff.  This is a full TDSO requirement.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdso_notify_hho_success( void )
{
  const caii_srv_cfg_type  *config_ptr;

  if ( !(tdso_call_on && tdso_so == CAI_SO_FULL_TDSO) )
  {
    return;
  }

  config_ptr = snm_get_current_config();

  M1X_MSG( DCP, LEGACY_HIGH,
    "TDSO informed about HHO success");

  /* ------------------------------------------------------------------------
  ** Set the TDSO state for the FCH and SCH0 channels for both FWD/REV Link
  ** to FALSE, This forces the TDSO to send all 1's.  Also send the SOCM
  ** Control derivative with the test parameters that were used before the
  ** Hard handoff.
  ** ---------------------------------------------------------------------- */
  fch_parm.pn_info[FWD_LINK].tdso_sync = FALSE;
  fch_parm.pn_info[REV_LINK].tdso_sync = FALSE;
  sch0_parm.pn_info[FWD_LINK].tdso_sync = FALSE;
  sch0_parm.pn_info[REV_LINK].tdso_sync = FALSE;

  //Set the PNs as invalid, they will computed on the next Sync frame.

  fch_parm.pn_info[FWD_LINK].pn_valid = FALSE;
  fch_parm.pn_info[REV_LINK].pn_valid = FALSE;
  sch0_parm.pn_info[FWD_LINK].pn_valid = FALSE;
  sch0_parm.pn_info[REV_LINK].pn_valid = FALSE;

  // Send the SOCM to the Base station after a HHO */
  M1X_MSG( DCP, LEGACY_HIGH,
    " Sending SOCM to BS ");



  dcch_parm.pn_info[FWD_LINK].pn_valid = FALSE;
  dcch_parm.pn_info[REV_LINK].pn_valid = FALSE;

  if (config_ptr->dcch_cc_incl)
  {
    // SOCM for FWD link DCCH/SCH configuration parameters
    if (config_ptr->num_for_sch > 0)
    {
      send_srv_ctl_tdso_ctl_dir (dcch_parm.pn_info[FWD_LINK].tst_par,
                                 sch0_parm.pn_info[FWD_LINK].tst_par);
    }
    else
    {
      send_srv_ctl_tdso_ctl_dir (dcch_parm.pn_info[FWD_LINK].tst_par,
                                   fch_parm.pn_info[FWD_LINK].tst_par);
    }

    // SOCM for REV link DCCH/SCH configuration parameters
    if (config_ptr->num_rev_sch > 0)
    {
      send_srv_ctl_tdso_ctl_dir (dcch_parm.pn_info[REV_LINK].tst_par,
                                 sch0_parm.pn_info[REV_LINK].tst_par);
    }
    else
    {
      send_srv_ctl_tdso_ctl_dir (dcch_parm.pn_info[REV_LINK].tst_par,
                                   fch_parm.pn_info[REV_LINK].tst_par);
    }
  }
  else
  {

    // SOCM for FWD link FCH/SCH configuration parameters
    if (config_ptr->num_for_sch > 0)
    {
      send_srv_ctl_tdso_ctl_dir (fch_parm.pn_info[FWD_LINK].tst_par,
                               sch0_parm.pn_info[FWD_LINK].tst_par);
    }
    else
    {
      send_srv_ctl_tdso_ctl_dir (fch_parm.pn_info[FWD_LINK].tst_par,
                                 fch_parm.pn_info[FWD_LINK].tst_par);
    }

    // SOCM for REV link FCH/SCH configuration parameters
    if (config_ptr->num_rev_sch > 0)
    {
      send_srv_ctl_tdso_ctl_dir (fch_parm.pn_info[REV_LINK].tst_par,
                               sch0_parm.pn_info[REV_LINK].tst_par);
    }
    else
    {
      send_srv_ctl_tdso_ctl_dir (fch_parm.pn_info[REV_LINK].tst_par,
                                 fch_parm.pn_info[REV_LINK].tst_par);
    }
  }

} /* tdso_notify_hho_success */

/*===========================================================================

FUNCTION     TDSO_SCH_CHNG_CBACK

DESCRIPTION
  Called by MC when there is a configuration change to the
  SCH's.  TDSO then sends all 1's on the changed SCH until the next SYNC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

/*lint -e818*/
void tdso_sch_chng_cback( mccsch_sch_info_type* sch_info)
{
  if ((sch_info->f_sch_0.changed) && (sch_info->f_sch_0.active))
  {
    // Forward SCH has configuration changed. MC informs this change when
    // the Rate changes or the Burst assignment becomes OFF to ON.
    // Forward SCH is configured.
    tdso_sch.sch_is_active = TRUE;
    if (tdso_so == CAI_SO_FULL_TDSO)
    {
      sch0_parm.pn_info[FWD_LINK].tdso_sync = FALSE;
      sch0_parm.pn_info[FWD_LINK].tdso_state = TRUE;
      sch0_parm.pn_info[FWD_LINK].frame_act = 0;
    }
    M1X_MSG( DCP, LEGACY_MED,
      "F-SCH config changed: Reset SYNC ");
  }

  if ((sch_info->r_sch_0.changed) && (sch_info->r_sch_0.active))
  {
    // Reverse SCH has configuration changed. MC informs this change when
    // the Rate changes or the Burst assignment becomes OFF to ON.
    // Reverse SCH is configured.
    tdso_sch.sch_is_active = TRUE;
    if (tdso_so == CAI_SO_FULL_TDSO)
    {
      sch0_parm.pn_info[REV_LINK].tdso_sync = FALSE;
      sch0_parm.pn_info[REV_LINK].tdso_state = TRUE;
      sch0_parm.pn_info[REV_LINK].frame_act = 0;
    }
    M1X_MSG( DCP, LEGACY_MED,
      "R-SCH config changed: Reset SYNC ");
  }
} /* tdso_sch_chng_cback */
/*lint +e818*/

/*===========================================================================

FUNCTION : TDSO_GET_VECT

DESCRIPTION
  This function updates a given table with the appropiate TDSO statistics.

DEPENDENCIES
  None

RETURN VALUE
  Length of vector (vector elements are dword here).

SIDE EFFECTS
  None

===========================================================================*/
uint8 tdso_get_vect
(
  uint8  ctl_rec_type,    /* Control record type */
  uint8  vect_counter_id, /* Vector counter id */
  uint32 *tsf             /* Address of storage for vector */
)
{
  uint8 len=0;

  /* ----------------------
  ** FCH counters retrieval
  ** ---------------------- */
  if (ctl_rec_type == TDSO_CNT_FCH)  /*lint !e641 */
  {
    switch (vect_counter_id)
    {
      /* ---------------------------
      ** FER counter response on FCH
      ** --------------------------- */
      case TDSO_FDCH_FER_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving FCH FER CNT");

        /* ----------
        ** tdso_e1_r1
        ** ---------- */
        tsf[0] = fch_rx_rpt[TDSO_RATE_1];

        /* -------------------------------------------------------
        ** tdso_e1_rbad = tdso_e1_rerr + tdso_e1_rd + tdso_e1_re +
        **                tdso_e1_ro + tdso_e1_rb + tdso_e1_rfl)
        ** ------------------------------------------------------- */
        tsf[1] = fch_rx_rpt[TDSO_BIT_ERROR] + fch_rx_rpt[TDSO_DIM_BURST]
                 + fch_rx_rpt[TDSO_ERASURE] +fch_rx_rpt[TDSO_OTHER]
                 + fch_rx_rpt[TDSO_BLANK] + fch_rx_rpt[TDSO_INSUFFICIENT_FRAME];

        /* ----------
        ** tdso_en_rn
        ** ---------- */
        tsf[2] = fch_rx_null_rpt[FDCHRX_NULL_NULL];

        /* --------------------------------------
        ** tdso_en_rbad = tdso_en_rb + tdso_en_ro
        ** -------------------------------------- */
        tsf[3] = fch_rx_null_rpt[FDCHRX_NULL_BLANK] +
                 fch_rx_null_rpt[FDCHRX_NULL_OTHER];

        /* -----------------------------------------------------
        ** tdso_ex_rbad = tdso_e1_rbad + tdso_en_rb + tdso_en_ro
        ** ----------------------------------------------------- */
        tsf[4] = tsf[1] + tsf[3];

        len = 5;
        break;

      /* -------------------------------------
      ** FCH receive expected counter response
      ** ------------------------------------- */
      case TDSO_FDCH_RXEX_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving FCH RXEX CNT");

        /* ----------
        ** tdso_e1_r1
        ** ---------- */
        tsf[0] = fch_rx_rpt[TDSO_RATE_1];

        /* ----------
        ** tdso_e1_rd
        ** ---------- */
        tsf[1] = fch_rx_rpt[TDSO_DIM_BURST];

        /* ----------
        ** tdso_e1_ro
        ** ---------- */
        tsf[2] = fch_rx_rpt[TDSO_OTHER];

        /* ----------
        ** tdso_e1_rb
        ** ---------- */
        tsf[3] = fch_rx_rpt[TDSO_BLANK];

        /* -----------
        ** tdso_e1_rfl
        ** ----------- */
        tsf[4] =fch_rx_rpt[TDSO_INSUFFICIENT_FRAME];

        /* ----------
        ** tdso_e1_re
        ** ---------- */
        tsf[5] = fch_rx_rpt[TDSO_ERASURE];

        /* ------------
        ** tdso_e1_rerr
        ** ------------ */
        tsf[6] = fch_rx_rpt[TDSO_BIT_ERROR];

        /* ----------
        ** tdso_en_rn
        ** ---------- */
        tsf[7] = fch_rx_null_rpt[FDCHRX_NULL_NULL];

        /* ----------
        ** tdso_en_rb
        ** ---------- */
        tsf[8] = fch_rx_null_rpt[FDCHRX_NULL_BLANK];

        /* ----------
        ** tdso_en_ro
        ** ---------- */
        tsf[9] = fch_rx_null_rpt[FDCHRX_NULL_OTHER];

        len = 10;
        break;

      /* ----------------------------
      ** Transmitted counter response
      ** ---------------------------- */
      case TDSO_FDCH_TX_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving FCH TX CNT");

        /* ----------
        ** tdso_e1_t1
        ** ---------- */
        tsf[0] = fch_tx_rpt[FDCHTX_E1_TRAFFIC];

        /* ----------
        ** tdso_e1_td
        ** ---------- */
        tsf[1] = fch_tx_rpt[FDCHTX_E1_DIM_BURST];

        /* ----------
        ** tdso_e1_tb
        ** ---------- */
        tsf[2] = fch_tx_rpt[FDCHTX_E1_BLANK];

        /* ----------
        ** tdso_eb_tb
        ** ---------- */
        tsf[3] = fch_tx_blank_rpt[FDCHTX_BLANK_BLANK];

        /* ----------
        ** tdso_eb_to
        ** ---------- */
        tsf[4] = fch_tx_blank_rpt[FDCHTX_BLANK_OTHER];

        len = 5;
        break;

      default:
        break;
    } /* end switch (vect_counter_id) */
  }

  /* DCCH */
  else if (ctl_rec_type == TDSO_CNT_DCCH)  /*lint !e641 */
  {
    switch (vect_counter_id)
    {
      /* ---------------------------
      ** FER counter response on FCH
      ** --------------------------- */
      case TDSO_FDCH_FER_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving DCCH FER CNT");

        /* ----------
        ** tdso_e1_r1
        ** ---------- */
        tsf[0] = dcch_rx_rpt[TDSO_RATE_1];

        /* -------------------------------------------------------
        ** tdso_e1_rbad = tdso_e1_rerr + tdso_e1_rd + tdso_e1_re +
        **                tdso_e1_ro + tdso_e1_rb + tdso_e1_rfl)
        ** ------------------------------------------------------- */
        tsf[1] = dcch_rx_rpt[TDSO_BIT_ERROR] + dcch_rx_rpt[TDSO_DIM_BURST]
                 + dcch_rx_rpt[TDSO_ERASURE] + dcch_rx_rpt[TDSO_OTHER]
                 + dcch_rx_rpt[TDSO_BLANK] + dcch_rx_rpt[TDSO_INSUFFICIENT_FRAME];

        /* ----------
        ** tdso_en_rn
        ** ---------- */
        tsf[2] = dcch_rx_null_rpt[FDCHRX_NULL_NULL];

        /* --------------------------------------
        ** tdso_en_rbad = tdso_en_rb + tdso_en_ro
        ** -------------------------------------- */
        tsf[3] = dcch_rx_null_rpt[FDCHRX_NULL_BLANK] +
                 dcch_rx_null_rpt[FDCHRX_NULL_OTHER];

        /* -----------------------------------------------------
        ** tdso_ex_rbad = tdso_e1_rbad + tdso_en_rb + tdso_en_ro
        ** ----------------------------------------------------- */
        tsf[4] = tsf[1] + tsf[3];

        len = 5;
        break;

      /* -------------------------------------
      ** FCH receive expected counter response
      ** ------------------------------------- */
      case TDSO_FDCH_RXEX_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving DCCH RXEX CNT");

        /* ----------
        ** tdso_e1_r1
        ** ---------- */
        tsf[0] = dcch_rx_rpt[TDSO_RATE_1];

        /* ----------
        ** tdso_e1_rd
        ** ---------- */
        tsf[1] = dcch_rx_rpt[TDSO_DIM_BURST];

        /* ----------
        ** tdso_e1_ro
        ** ---------- */
        tsf[2] = dcch_rx_rpt[TDSO_OTHER];

        /* ----------
        ** tdso_e1_rb
        ** ---------- */
        tsf[3] = dcch_rx_rpt[TDSO_BLANK];

        /* -----------
        ** tdso_e1_rfl
        ** ----------- */
        tsf[4] = dcch_rx_rpt[TDSO_INSUFFICIENT_FRAME];

        /* ----------
        ** tdso_e1_re
        ** ---------- */
        tsf[5] = dcch_rx_rpt[TDSO_ERASURE];

        /* ------------
        ** tdso_e1_rerr
        ** ------------ */
        tsf[6] = dcch_rx_rpt[TDSO_BIT_ERROR];

        /* ----------
        ** tdso_en_rn
        ** ---------- */
        tsf[7] = dcch_rx_null_rpt[FDCHRX_NULL_NULL];

        /* ----------
        ** tdso_en_rb
        ** ---------- */
        tsf[8] = dcch_rx_null_rpt[FDCHRX_NULL_BLANK];

        /* ----------
        ** tdso_en_ro
        ** ---------- */
        tsf[9] = dcch_rx_null_rpt[FDCHRX_NULL_OTHER];

        len = 10;
        break;

      /* ----------------------------
      ** Transmitted counter response
      ** ---------------------------- */
      case TDSO_FDCH_TX_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving DCCH TX CNT");

        /* ----------
        ** tdso_e1_t1
        ** ---------- */
        tsf[0] = dcch_tx_rpt[FDCHTX_E1_TRAFFIC];

        /* ----------
        ** tdso_e1_td
        ** ---------- */
        tsf[1] = dcch_tx_rpt[FDCHTX_E1_DIM_BURST];

        /* ----------
        ** tdso_e1_tb
        ** ---------- */
        tsf[2] = dcch_tx_rpt[FDCHTX_E1_BLANK];

        /* ----------
        ** tdso_eb_tb
        ** ---------- */
        tsf[3] = dcch_tx_blank_rpt[FDCHTX_BLANK_BLANK];

        /* ----------
        ** tdso_eb_to
        ** ---------- */
        tsf[4] = dcch_tx_blank_rpt[FDCHTX_BLANK_OTHER];

        len = 5;
        break;

      default:
        break;
    } /* end switch (vect_counter_id) */
  }

  /* -----------------------
  ** SCH0 counters retrieval
  ** ----------------------- */
  else if (ctl_rec_type == TDSO_CNT_SCH0)   /*lint !e641 */
  {
    switch (vect_counter_id)
    {
      /* ---------------------
      ** FER counters response
      ** --------------------- */
      case TDSO_SCH_FER_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving SCH FER CNT");

        /* ----------------------------------------------------------
        ** Forward SCH is not active in the last copy of SCH counters
        ** ---------------------------------------------------------- */
        if ((tdso_sch.sch_rx_rate == TDSO_SCH_RATE_NONE) ||
            (tdso_sch.sch_rx_rate > TDSO_SCH_RATE_16X))
        {
          M1X_MSG( DCP, LEGACY_MED,
            "TDSO SCH Rx FER counters are all zeros");
          tsf[0] = tsf[1] = tsf[2] = tsf[3] = tsf[4] = 0;
        }
        /* ------------------------------------------------------
        ** Forward SCH is active in the last copy of SCH counters
        ** ------------------------------------------------------ */
        else
        {

          M1X_MSG( DCP, LEGACY_HIGH,
            "Retrieve F-SCH CNTS: Rate :%d",
            tdso_sch.sch_rx_rate);

          /* ------------
          ** tdso_enx_rnx
          ** ------------ */
          tsf[0] = sch_rx_rpt[SCHRX_EN_TRAFFIC][tdso_sch.sch_rx_rate];

          /* --------------------------------------------------------
          ** tdso_enx_rbad = tdso_enx_err + tdso_enx_re + tdso_enx_rb
          ** -------------------------------------------------------- */
          tsf[1] = sch_rx_rpt[SCHRX_EN_FRAME_ERROR][tdso_sch.sch_rx_rate] +
            sch_rx_rpt[SCHRX_EN_ERASURE][tdso_sch.sch_rx_rate] +
            sch_rx_rpt[SCHRX_EN_BLANK][tdso_sch.sch_rx_rate];

          /* ----------
          ** tdso_eb_rb
          ** ---------- */
          tsf[2] = sch_rx_blank_rpt[SCHRX_BLANK_BLANK];

          /* -------------------------
          ** tdso_eb_rbad = tdso_eb_ro
          ** ------------------------- */
          tsf[3] = sch_rx_blank_rpt[SCHRX_BLANK_OTHER];

          /* -----------------------------------------
          ** tdso_ex_rbad = tdso_enx_rbad + tdso_eb_ro
          ** ----------------------------------------- `*/
          tsf[4] = tsf[1] + tsf[3];
        } /* end if (tdso_sch.sch_rx_rate == TDSO_SCH_RATE_NONE) */

        len = 5;
        break;

      /* ---------------------
      ** PER counters response
      ** --------------------- */
      case TDSO_SCH_PER_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving SCH PER CNT");

        /* ----------------------------------------------------------
        ** Forward SCH is not active in the last copy of SCH counters
        ** ---------------------------------------------------------- */
        if ((tdso_sch.sch_rx_rate == TDSO_SCH_RATE_NONE) ||
            (tdso_sch.sch_rx_rate > TDSO_SCH_RATE_16X))
        {
          M1X_MSG( DCP, LEGACY_MED,
            "TDSO SCH Rx PER counters are all zeros");
          tsf[0] = tsf[1] = tsf[2] = tsf[3] = tsf[4] = tsf[5] = tsf[6] =
            tsf[7] = tsf[8] = tsf[9] = tsf[10] = tsf[11] = 0;
        }
        /* ------------------------------------------------------
        ** Forward SCH is active in the last copy of SCH counters
        ** ------------------------------------------------------ */
        else
        {
          /* ----------
          ** tdso_e3_r3
          ** ---------- */
          tsf[0] = sch_rx_pdu_buffer[TDSO_RATE_3_PDU][SCHRX_EN_TRAFFIC];

          /* ------------
          ** tdso_e3_rerr
          ** ------------ */
          tsf[1] = sch_rx_pdu_buffer[TDSO_RATE_3_PDU][SCHRX_EN_FRAME_ERROR];

          /* ----------
          ** tdso_e3_re
          ** ---------- */
          tsf[2] = sch_rx_pdu_buffer[TDSO_RATE_3_PDU][SCHRX_EN_ERASURE];

          /* ----------
          ** tdso_e2_r2
          ** ---------- */
          tsf[3] = sch_rx_pdu_buffer[TDSO_RATE_2_PDU][SCHRX_EN_TRAFFIC];

          /* ------------
          ** tdso_e2_rerr
          ** ------------ */
          tsf[4] = sch_rx_pdu_buffer[TDSO_RATE_2_PDU][SCHRX_EN_FRAME_ERROR];

          /* ----------
          ** tdso_e2_re
          ** ---------- */
          tsf[5] = sch_rx_pdu_buffer[TDSO_RATE_2_PDU][SCHRX_EN_ERASURE];

          /* ------------
          ** tdso_e1a_r1a
          ** ------------ */
          tsf[6] = sch_rx_pdu_buffer[TDSO_RATE_1A_PDU][SCHRX_EN_TRAFFIC];

          /* -------------
          ** tdso_e1a_rerr
          ** ------------- */
          tsf[7] = sch_rx_pdu_buffer[TDSO_RATE_1A_PDU][SCHRX_EN_FRAME_ERROR];

          /* -----------
          ** tdso_e1a_re
          ** ----------- */
          tsf[8] = sch_rx_pdu_buffer[TDSO_RATE_1A_PDU][SCHRX_EN_ERASURE];

          /* ------------
          ** tdso_e1b_r1b
          ** ------------ */
          tsf[9] = sch_rx_pdu_buffer[TDSO_RATE_1B_PDU][SCHRX_EN_TRAFFIC];

          /* -------------
          ** tdso_e1b_rerr
          ** ------------- */
          tsf[10] = sch_rx_pdu_buffer[TDSO_RATE_1B_PDU][SCHRX_EN_FRAME_ERROR];

          /* -----------
          ** tdso_e1b_re
          ** ----------- */
          tsf[11] = sch_rx_pdu_buffer[TDSO_RATE_1B_PDU][SCHRX_EN_ERASURE];
        } /* end if (tdso_sch.sch_rx_rate == TDSO_SCH_RATE) */

        len = 12;
        break;

      /* -----------------------------
      ** Transmitted counters response
      ** ----------------------------- */
      case TDSO_SCH_TX_CNT:

        M1X_MSG( DCP, LEGACY_HIGH,
          "Retrieving SCH TX CNT");

        /* ----------------------------------------------------------
        ** reverse SCH is not active in the last copy of SCH counters
        ** ---------------------------------------------------------- */
        if ((tdso_sch.sch_tx_rate == TDSO_SCH_RATE_NONE) ||
            (tdso_sch.sch_tx_rate > TDSO_SCH_RATE_16X))
        {
          M1X_MSG( DCP, LEGACY_MED,
            "TDSO SCH Tx counters are all zeros");
          tsf[0] = tsf[1] = tsf[2] = tsf[3] = 0;
        }
        /* ------------------------------------------------------
        ** reverse SCH is active in the last copy of SCH counters
        ** ------------------------------------------------------ */
        else
        {
          /* ------------
          ** tdso_enx_tnx
          ** ------------ */
          tsf[0] = sch_tx_rpt[SCHTX_EN_TRAFFIC][tdso_sch.sch_tx_rate];

          /* -----------
          ** tdso_enx_tb
          ** ----------- */
          tsf[1] = sch_tx_rpt[SCHTX_EN_BLANK][tdso_sch.sch_tx_rate];

          /* ----------
          ** tdso_eb_tb
          ** ---------- */
          tsf[2] = sch_tx_blank_rpt;

          /* ----------
          ** tdso_eb_to
          ** ---------- */
          tsf[3] = 0;

        } /* end if (tdso_sch.sch_tx_rate == TDSO_SCH_RATE_NONE) */

        len = 4;
        break;

      default:
        break;
    } /* end switch (vect_counter_id) */
  } /* end if (ctl_rec_type == TDSO_CNT_SCH0) */

  return len;

} /* tdso_get_vect */

/*===========================================================================
FUNCTION     TDSO_END_CALL

DESCRIPTION
  This function is called by MC in the release substate.  It does some
  cleanup needed when ending a TDSO call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void tdso_end_call( void )
{
  if ( tdso_call_on )
  {
    tdso_log_stats();
    tdso_call_on = FALSE;
  }
} /* tdso_end_call */

/*===========================================================================

FUNCTION : TDSO_UPDATE_DCCH_TX_STATS

DESCRIPTION:
  This function updates the DCCH transmission statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
 This function depends on the FCH TDSO STATE parameter.

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

/*lint -e818*/
void tdso_update_dcch_tx_stats
(
  tdso_tx_frame_type *tx_frame_ptr,
  boolean            tdso_state
)
{
  if (tdso_state) /* TDSO state is ON */
  {
    if (tx_frame_ptr->fr_rate == TDSO_RATE_1)
    {
      dcch_tx_table[FDCHTX_E1_TRAFFIC]++;
    }
    else if (tx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      dcch_tx_table[FDCHTX_E1_BLANK]++;
    }
    else
    {
      dcch_tx_table[FDCHTX_E1_DIM_BURST]++;
    }
  }
  else /* TDSO stat is off */
  {
    if (tx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      dcch_tx_blank_table[FDCHTX_BLANK_BLANK]++;
    }
    else
    {
      dcch_tx_blank_table[FDCHTX_BLANK_OTHER]++;
    }
  }

} /* tdso_update_dcch_tx_stats */

/*===========================================================================

FUNCTION : TDSO_UPDATE_DCCH_RX_STATS

DESCRIPTION:
  This function updates the DCCH Receive statistics (counters)
  When the TDSO is transmitting frames on the reverse Link.

DEPENDENCIES:
  This function depends on the DCCH TDSO STATE parameter.

RETURN VALUE:
  None.

SIDE EFFECTS:

===========================================================================*/
void tdso_update_dcch_rx_stats
(
  tdso_rx_frame_type *rx_frame_ptr,
  boolean            tdso_state
)
{
  /* check the valid range of fr_rate value */
  if (rx_frame_ptr->fr_rate >= TDSO_MAX_RATE)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "FR Rate is out of range");
      return;
  }

  if (tdso_state) /* TDSO state is ON */
  {
    if (tdso_so == CAI_SO_FULL_TDSO)
    {
      /* For FTDSO, 1/2, 1/4 and 1/8 rate frames are updated as DIM-BURST */
      if ((rx_frame_ptr->fr_rate == TDSO_RATE_HALF) ||
          (rx_frame_ptr->fr_rate == TDSO_RATE_QUARTER) ||
          (rx_frame_ptr->fr_rate == TDSO_RATE_EIGHTH))
      {
        dcch_rx_table[TDSO_DIM_BURST]++;
        M1X_MSG( DCP, LEGACY_HIGH,
          "TDSO ON: Received DIM & BURST %d",
          rx_frame_ptr->fr_rate);
      }
      else
      {
        dcch_rx_table[rx_frame_ptr->fr_rate]++;
      }
    }
    /* For STDSO, update counters individually for 1/2, 1/4 & 1/8th rates */
    else
    {
      dcch_rx_table[rx_frame_ptr->fr_rate]++;
    }
  }
  else /* TDSO state is OFF */
  {
    if ((rx_frame_ptr->fr_rate == TDSO_NULL) ||
        (rx_frame_ptr->fr_rate == TDSO_RATE_EIGHTH))
    {
      dcch_rx_null_table[FDCHRX_NULL_NULL]++;
    }
    else if (rx_frame_ptr->fr_rate == TDSO_BLANK)
    {
      dcch_rx_null_table[FDCHRX_NULL_BLANK]++;
    }
    else
    {
      dcch_rx_null_table[FDCHRX_NULL_OTHER]++;
    }
  }

} /* tdso_update_dcch_rx_stats */
/*lint +e818*/

/*===========================================================================

FUNCTION : TDSO_PROCESS_DCCH_RX_FRAME

DESCRIPTION
  This function processes the Received DCCH TDSO frame(s).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_process_dcch_rx_frame
(
  tdso_frame_block_type *frame_blk_ptr /* Ptr to rxed frame block */
)
{
  tdso_rx_frame_type *rx_frame_ptr = NULL; /* Pointer to rxed frame */
  uint8 expected_data[34];                 /* Expected FCH data     */
  size_t copy_size;      /*To store the value returned by memscpy */
  tdso_status_type status = TDSO_VALID;
  byte err_mask = 0;
  word i;

  /* set the frame pointer and frame rate of the FCH frame */
  rx_frame_ptr = &(frame_blk_ptr->frame_type.fwd_arr[0]);


  /*------------------------------------
  ** Check if the MUX_PDU_TYPE is valid.
  ** ----------------------------------- */
  if (rx_frame_ptr->mux_pdu_type > TDSO_MUX_PDU_2)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid DCCH MUX_PDU_TYPE type = %d",
      rx_frame_ptr->mux_pdu_type);
    return TDSO_INVALID_SRVC;
  }

  /*------------------------------------
  ** Check if the TDSO rate set is valid.
  ** ----------------------------------- */
  if (fdch_tdso_rate_set >= TDSO_MAX_RATE_SET)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid TDSO rate set= %d",
      fdch_tdso_rate_set);
    return TDSO_INVALID_SRVC;
  }


  /* ---------------------------------------------------------------------
  ** Validate the TDSO frame only if it is a Full Rate TDSO frame.  If a
  ** HALF, QUARTER, EIGHT rate is received, no need to validate the frame
  ** errors.  But the statistics of the DIM and BURST are updated
  ** --------------------------------------------------------------------*/
  if ((dcch_parm.pn_info[FWD_LINK].tdso_state) &&
      (rx_frame_ptr->fr_rate == TDSO_RATE_1))
  {
    /* Copy the generated expected frame into the local buffer */
    copy_size = memscpy(expected_data, sizeof(expected_data),
                        frame_blk_ptr->frame_type.rev_arr[0].fr_item_ptr,
           tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set]);
    if( copy_size != tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set] )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
        copy_size,
        tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set],
        sizeof(expected_data));
    }

    /* Compare the Received Frame with that of the locally generated one */
    for (i=0; i < tdso_bytes[TDSO_RATE_1][fdch_tdso_rate_set]; i++)
    {
      if ((err_mask = (rx_frame_ptr->fr_item_ptr[i] ^ expected_data[i])) != 0)
      {
        rx_frame_ptr->fr_rate = TDSO_BIT_ERROR;
        break;
      }
    }

    if (!err_mask)
    {
      /*---------------------------------------------------------------------
      ** Check the validity of the Last byte in the frame. The Full TDSO has
      ** bits in the last bytes (after the last octet) padded with 0's
      ** -------------------------------------------------------------------*/
      if (tdso_so == CAI_SO_FULL_TDSO)
      {
        err_mask = (uint8) (rx_frame_ptr->fr_item_ptr[i] &
          tdso_last_byte[TDSO_RATE_1][fdch_tdso_rate_set]);
      }
      else
      {
        /*----------------------------------------------------------------------
        ** Check the validity of the Last byte in the frame. The Simple TDSO has
        ** bits in the last bytes (after the last octet) padded with 1's
        ** -------------------------------------------------------------------*/
        err_mask = (uint8) (((rx_frame_ptr->fr_item_ptr[i] &
          tdso_last_byte[TDSO_RATE_1][fdch_tdso_rate_set]) ^
          tdso_last_byte[TDSO_RATE_1][fdch_tdso_rate_set]));
      }
    }
  }

  /* Update the DCCH RX statistics */
  tdso_update_dcch_rx_stats(rx_frame_ptr,
    dcch_parm.pn_info[FWD_LINK].tdso_state);

  /* Update state of TDSO */
  tdso_update_on_off_state(TDSO_DCCH, FWD_LINK);

  return status;

} /* tdso_process_dcch_rx_frame */


/*===========================================================================

FUNCTION : TDSO_DV_CALL_IS_ON

DESCRIPTION
  This function returns TRUE if the Full TDSO call is on.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None

===========================================================================*/
boolean tdso_dv_call_is_on()
{
    return FALSE;
}
