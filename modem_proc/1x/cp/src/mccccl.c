/*===========================================================================

     C O D E   C H A N N E L   L I S T   M A I N T E N A N C E

GENERAL DESCRIPTION
  This module maintains the code channel list according to IS95B section 6.6.8.

DESCRIPTION
  This module contains definitions and interfaces for maintaining the code
  channel list.

  Copyright (c) 1999 - 2014 Qualcomm Technologies, Inc. 
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


$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccccl.c_v   1.22   02 Oct 2002 14:40:28   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccccl.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/14   agh     Reject UHDM/MEID_UHDM if num_pilots if equal to Zero
05/06/14   gga     1X_CP F3 redistribution
08/05/13   gga     Feature cleanup
04/04/12   trc     Convert to m1x_time module
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/04/11   ag      Fixed compiler and lint warnings.
11/15/10   jtm     Initial SU API split for CP.
10/20/10   azh     Fixed KW warnings.
09/15/10   azh     Fixed KlocWork warnings.
07/22/10   ag      De-coupling of RCP data from CCL object.
06/10/10   jtm     Eliminated dependency on log_dmss.h.
05/26/10   ag      Replaced PACKED with PACK() to support QDSP6.
04/23/10   ag      Support for 1x Modem statistical report.
10/12/09   ssh     Initialized timestamp variable to zero as part of KW error
                   fix.
09/29/09   ag      1X Advanced Signaling Support - Phase 2
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
08/02/09   jtm     Lint fixes.
05/03/09   jj      CMI Phase-II update.
04/29/09   jj      Replaced assert.h with amssassert.h
04/17/09   jtm     Added includes to fix compiler warnings.
03/20/09   jtm     Removed unnecessary include of mctrans.h.
06/11/08   ag      Fixed lint critical errors.
04/14/08   bb      To fix errors in Klocwork errors
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/20/08   bb      Fixed klocwork errors
05/11/07   pg      More Lint Cleanup.
03/19/07   pg      Lint Cleanup.
03/15/07   an      Changed reference_buf pointer to an array
03/08/07   an      Fixed Critical Lint Errors
05/18/06   fc      Changed the function prototype of initialize.
04/26/06   an      Fixed Lint Errors
07/26/05   fc      Fixed lint errors.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/01/05   fc      Merged the following from MSMSHARED:
02/23/05   sb       Move set_cur_chind from update_ccl_with_uhdm to mcctxns_ho_pre.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/16/05   sb      Fixed lint errors.
02/15/05   sb      Check if we are in DV before adding reverse pilot to chind.
02/09/05   va      Merge from MSMSHARED CP tip:
02/01/05   sb       Changed log_alloc_ex to log_alloc (log_alloc_ex is obsolete).
01/26/05   sb       Fixed bug in logging of reduced active set.
01/04/05   sb       Log code uses FPOS instead of sizeof, to account for padding.
02/08/05   sb      Renamed functions as per code review comments.
01/14/05   sb      Release D interface updates.
01/10/05   va      Merged lint fixes from REL_B_C archive
12/22/04   fc      Code review comment changes.
12/16/04   sb      Send DV group number to SRCH in aset.
11/22/04   sb      Miscellaneous fixes for Rel C DV Handoffs.
11/05/04   sb      Added support for Rel C DV handoffs.
11/01/04   sb      Check if dv info is valid before setting num pdcch in aset.
10/27/04   va      Corrected the setting of DCCH active flag in ASET for SRCH
10/07/04   fc      Removed redundant condition checks.
09/28/04   va      Fixed a featurization bug
09/27/04   va      Merged from REL B/C archive
09/16/04   fc      Added support for reverse DV.
08/20/04   bkm     Some lint cleanup.
08/09/04   fc      Service negotiation changes for Release C and D.
08/03/04   bkm     Set WALSH_TABLE_ID, NUM_PDCCH, FOR_PDCCH_WALSH from OVHD
                   if not included in ECAM.  Update set_chan_mask() to remove
                   unnecessary local variable.
06/17/04   bkm     Featurization changes to allowing DV_SIG_TEST build.
06/08/04   bkm     Featurization changes to allowing building w/o Rel B/C/D.
06/04/04   bkm     Lint cleanup.
05/28/04   bkm     Merged the following from IS2000_REL_B_C
  03/05/04   bkm     Lint cleanup.
04/22/04   bkm     Misc updates from DV Call Setup/PCNM code review.
04/14/04   az      Added support for ERM - Rel C DV chan cfg change
04/08/04   bkm     Channel mask updates.  PDCH grouping now follows OTA order
                   not CCL order.  Have group numbers cycle up to 255 and wrap.
04/01/04   bkm     Updates for more concise cai_ext_chind enums.
04/01/04   bkm     Init ota_chind and ota_extchind in mccccl_initialize.
03/25/04   ejv     Modified the PDCCH data format to SRCH.
01/28/04   bkm     Updates to support soft/softer switch.
01/05/04   bkm     Misc updates for DV F-PDCH support.
12/02/03   bkm     Split internal chind into fwd_chan_mask and rev_chan_mask.
10/23/03   va      Changed caii_ext_ch_ind3_pilot_rec_type to
                   mccdma_pilot_info_type
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
07/22/03   bkm     Fixed P2 featurization wrt fch_info.active variable.
06/02/03   bkm     Lint cleanup (indentation changes only).
05/28/03   sb      Removed redundant call to mclog_report_event_ch_ind.
05/28/03   va      Changed the prototype of pilot_is_in_active_set to also
                   include pilot_rec_type
01/27/03   lh      Deleted message error when log_submit returns FALSE because
                   of diabled log mask.
01/27/03   jrp     Fixed precalculation of reduced active set buffer requirements
                   to prevent ERR_FATAL.
11/07/02   az      Mainlined FEATURE_IS95B
10/02/02   az      Mainlined FEATURE_IS2000, FEATURE_IS2000_2_1_ADDENDUM
09/13/02   va      Moved assignment of ccl.chind in set_cur_chind
09/06/02   va      Cleaned up lint errors.
08/01/02   va        Fixed compiler errors when REL_A is turned off
07/24/02   sb      Added srch_offset_nghbr to ccl.
07/19/02   va      Added support FCH storing and restoring when going to CH
                   if necessary
05/31/02   lcc     Corrected update of chind in ccl with set_cur_chind in
                   update_ccl_with_uhdm.
05/12/02   ht      (cml) Changed FEATURE_IS2000 to FEATURE_IS2000_SCH.
05/07/02   fc      Added function pilot_is_in_sch_active_set.
05/01/02   lcc     In generate_aset, added clearing of SCCH when SCH is active
                   and vice versa.
04/01/02   ph      Look in to num_sch in addition to incl fields before using supplemental
                   channel.
10/17/01   jrp     Added support for active set and reduced active set logs.
10/02/01   ph      Merged from CP VU 96
                   08/27/01   lcc     Removed an invalid ASSERT statement in get_aset_info.
07/18/01   ph      Fixed bug with regard to updating pilot_rec_type in active set
06/26/01   sb      Merged the following from common archive:
                   05/24/01   jq      Added IS2000_SCH ifdef to fix a compile error.
05/16/01   fc      Merged the following from common archive.
                   05/11/01   jq      Added ControlHold setting call back for
                                      DS.
                   05/03/01   jq/lcc  Added sccl initialization in
                                      mccccl_initialize.
                   04/16/01   fc      Corrected the parameter passed to
                                      mclog_report_event in set_cur_chind.
                   04/12/01   fc      Added support of events report: CH_IND
                                      change.
                   04/10/01   jq/lcc  Added functions to support P2 and control
                                      hold.
04/09/01   va      Changed get_pilots function for FEATURE_IS2000_REL_A where it
                   returns more info than before (as needed by Ext. HOCM
                   message).
03/07/01   fc      Moved function pilot_is_in_active_set outside
                   FEATURE_IS2000_SCH.
03/06/01   jq      Added aset_info logging to replace aset_config loggin.
03/01/01   va      Merged the following changes from Common archive.
 01/08/01  jq      Made some naming changes to prepare for IS2000_P2.
 11/29/00  lcc     Added initialization of R-SCH Walsh IDs as mandated in IS2000.
02/27/01   va      Added pilot_rec type info in the ccl array for pilots.
                   Added support for populating this info.
08/23/00   lcc     1. For IS2000 only, changed function generate_aset_default_sups
                      to include sup_state in parameter.
                   2. Added function mark_pilot_as_invalid for marking a pilot in CCL
                      as invalid.
                   3. Suppressed lint warnings.
08/02/00   fc      Changed sch rate type to enumeration type in
                   ccl_object_type.
07/20/00   fc      Used enumeration type for SCH rate.
06/05/00   lad     Fixed SCH support for active set log.
06/02/00   lcc     Changes to support IS2000 Rev 0 published addendum.  Also
                   corrected some featurization.
04/25/00   lcc     Changed data structures and related functions to support forward
                   and reverse SCH
04/20/00   lad     Made minor renaming changes to active set log.
04/17/00   lad     Added active set/SCH configuration logging.
03/28/00   ry      Fixed compiler warnings introduced from the previous merge
03/12/00   ry      Added IS2000 featurization
03/06/00   lcc     Added partial support for SCH management.
12/10/99   lcc     Corrected function get_pilots to return pilots in their
                   original order.  This fixes CRs 12211 and 12284.
11/15/99   va      IS2000 Changes, Added support for QOF id and new assign mode
                   in ECAM.
11/03/99   lcc     Merged from GD4021:
           ry      Added member functions get_pilots, save, and restore to the
                   mccccl_code_channel_list object, and the actual functions
                   themselves
           kmp     Added #ifdef FEATURE_IS95B || FEATURE_SPECIAL_MDR to
                   update_all() to fix compilation serious errors when 95B and
                   MDR (any form of MDR) are not defined.
           ry      Moved all occurrences of update_all's out of the
                   #ifdef(FEATURE_IS95B_MDR || FEATURE_SPECIAL_MDR) block
09/20/99   lcc     Mainlined update_all.
08/18/99   ram     Enclosed the prototype definitions of generate_aset_sups
                   update_all, get_number_of_supplementals functions under the
                   defines FEATURE_IS95B_MDR || FEATURE_SPECIAL_MDR.
                   They are used only when either one is defined. The functions
                   are already enclosed in these feature defines.
08/03/99   lcc     Removed function returns from functions initialize,
                   update_all, update_sups, and update_pilots and handle the
                   error cases internally (per code review),  corrected
                   featurization with function update_sups, miscellaneous
                   code review cleanups, and added sup_pilots_are_valid
                   function.
05/19/99   doj     Changed GHDM pilot type (caii_gen_ho_pilot_type) reference
                   to generic handoff type (caii_ho_pilot_type).
05/14/99   lcc     In update_all, moved checking of num_sup to the processing of
                   "MCCCCL_UPDATE_ALL", which takes meaningful value only in such
                   case.
04/23/99   lcc     Added member pwr_comb_ind to each pilot so that CCL is
                   responsible for filling that field in the tc_aset command,
                   which was not set before.
03/26/99   lcc     Added member function get_number_of_supplementals in
                   mccccl_code_channel_list object, and the actual function
                   itself.
02/12/99   lcc     Added complete support for special MDR.
01/26/99   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_utils.h"

#include "caii_v.h"
#include "mccccl.h"
#include "srv.h"
#include "mccap_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mcctcho_i.h"
#include "log.h"
#include "m1x_time_i.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"
#include "mclog_v.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mccds_v.h"

/*lint -e655 -e818 */

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/
static byte get_pilots
(
  mccdma_pilot_info_type pilot_array[]
);

static byte generate_aset
(
  srch_tc_aset_type* aset,
    /* Pointer to the active set structure that needs to be
    updated by this function */

  mccccl_sup_state_type* sup_state, // Used to return whether SCH or SCCH info returned

  boolean use_sups
    /* Used to indicate TRUE if supplemental channels need to be filled into
       the structure */
);

static byte generate_aset_default_sups
(
  srch_tc_aset_type* aset
    /* Pointer to the active set structure that needs to be
    updated by this function */

  , mccccl_sup_state_type* sup_state // Used to return whether SCH or SCCH info returned

);

static void initialize
(
  byte band_class,
  word cdma_channel,
  byte num_pilots,
  mccdma_pilot_info_type pilot_array[],
  mccdma_chnasn_type *chnasn_ptr
);

static void report_channel_change
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_ext_chind
);


static void update_pilots
(
  byte band_class,
  word cdma_channel,
  byte num_pilots,
  mccdma_pilot_info_type pilot_array[]
);

// Function to update whole CCL (supplementals optional)
static void update_all
(
  byte band_class,
  word cdma_channel,
  byte num_pilots,
  mccccl_update_mode_type update_mode,
  byte num_sup,
  caii_ho_pilot_type pilot_info[],
  byte ch_ind
);

static void save( void );
static void restore( void );

static void update_ccl_with_uhdm
(
  caii_ho_msg_type *uhdm
);
static byte get_aset_info
(
  mclog_active_set_type *aset,
  byte *fsch_rate,
  mclog_aset_rsch_type *rsch
);

static void update_ccl_with_escam
(
  caii_escam_type* escam
);

static boolean generate_sch_aset
(
  mctrans_srch_f_sch_action_type* sch_aset
    /* Pointer to the SCH info structure that will stored the SCH aset
       generated by this function */
);

static boolean pilot_is_in_sch_aset
(
  uint16 pilot_pn
);

static void activate_f_sch_assign
(
  uint8   sch_id,          // SCH 0 or 1
  uint8   sccl_index
);

static void activate_r_sch_assign
(
  uint8               sch_id,          // SCH 0 or 1
  cai_sch_rate_type   sch_rate
);

static void deactivate_f_sch
(
  uint8 sch_id    // SCH 0 or 1
);

static void deactivate_r_sch
(
  uint8 sch_id    // SCH 0 or 1
);

static const mccccl_sccl_entry_type* get_sccl_entry
(
  uint8 sch_id,   // SCH 0 or 1
  uint8 index
);

static uint8 get_r_sch_walsh
(
  uint8 sch_id,   // SCH 0 or 1
  uint8 rate
);

static boolean pilot_is_in_active_set
(
  uint16 pilot_pn,
  cai_pilot_rec_type pilot_rec_type,
  boolean match_pilot_rec_type
);

static void mark_pilot_as_invalid
(
  uint8 cclp
);

static boolean fwd_fundicated_chan_is_active
(
  void
);

static void set_chan_mask
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
);

static mcc_fwd_chan_mask_type get_fwd_chan_mask
(
  void
);

static mcc_rev_chan_mask_type get_rev_chan_mask
(
  void
);

static mcc_fwd_chan_mask_type gen_fwd_chan_mask
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
);

static mcc_rev_chan_mask_type gen_rev_chan_mask
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
);

static mcc_fwd_chan_mask_type gen_fwd_chan_mask_erm
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
);

static mcc_rev_chan_mask_type gen_rev_chan_mask_erm
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
);

static void set_chan_mask_erm
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
);

static cai_ext_chind_type get_ota_extchind
(
  void
);

static boolean in_control_hold
(
  void
);

static void exit_control_hold
(
  boolean restore_fch
);

static void get_control_hold_info
(
  mclog_control_hold_type *ch
);

#ifdef FEATURE_IS2000_REL_A_SVD
static void set_fch_to_be_restored
(
  boolean value
);

static boolean get_fch_to_be_restored
(
  void
);
#endif //FEATURE_IS2000_REL_A_SVD

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
// Internal private portion of CCL object
typedef struct
{
  uint8   band_class;                    /* Band class and cdma channel will always */
  uint16  cdma_channel;                  /* the same for all pilots - can't soft */
                                         /* handoff across multiple frequencies */
  mccccl_sup_state_type sup_state;       /* Records whether SCH or SCCH or none active */

  cai_chind_type            ota_chind;   /* OTA CH_IND field */
  cai_ext_chind_type     ota_extchind;   /* OTA EXT_CH_IND field */
  mcc_fwd_chan_mask_type fwd_chan_mask;  /* Bit mask of active fwd chans */
  mcc_rev_chan_mask_type rev_chan_mask;  /* Bit mask of active rev chans */

#ifdef FEATURE_IS2000_REL_A_SVD
  boolean fch_to_be_restored; /* Indicates if CCL has FCH info stored
                              for restoration upon transition from CHS to
                              active */
#endif //FEATURE_IS2000_REL_A_SVD

  boolean sups_active;                   /* Indicates if we are current processing supa */

  struct
  {
    uint8 order;                          // The order of pilots in the ?HDM message,
                                          // which is important because we need to
                                          // preserve order of pwr_comb_ind
    mccccl_pilot_state_type pilot_state;  // See explanation on type definition
    uint16  pilot_pn;                     // this pilot's PN

    cdma_walsh_fn_type fch_code_chan;     // Code channel and QOF of FCH
    cdma_walsh_fn_type dcch_code_chan;    // Code channel and QOF of DCCH
    cai_pilot_rec_type pilot_rec_type;    // Pilot record type
    caii_add_pilot_rec_type pilot_rec;    // Pilot rec spec fields
                                          // if it is not a COMMON pilot

    boolean pwr_comb_ind;                 // Power combining indicator
    byte srch_offset_nghbr;               // Neighbor Window Search Offset

    // This contains per pilot SCH and SCCH info.  Although they are mutually exclusive and
    // could have been a union, it's best to have both sets of info here so that they can be
    // both initialized.  Otherwise if RC changes on the traffic channel, what's initialized
    // for one member of the union may not work for the other.
    struct
    {
      struct
      {
        uint8 num_sup_chan;                    // number of supplementals for this pilot
        uint8 sup_code_chan[CAI_MAX_NUM_SUP];
      } scch;

      struct
      {
        boolean           active;   // Indicates if this SCH is active
        cai_sch_rate_type sch_rate; // This is same for all pilots, put here for convenience
        uint16            code_chan_sch;
        uint8             qof_mask_id_sch;
      } for_sch[CAI_MAX_NUM_FOR_SCH];

    } sup_info;

  } pilots[CCL_SIZE];

  // Current Walsh assignment for reverse SCH
  struct
  {
    boolean   active;           // Indicates if this SCH is active
    uint8     walsh_id;         // Can be 0 or 1 only
    cai_sch_rate_type rate;
  } rev_sch[CAI_MAX_NUM_REV_SCH];

  // Configuration information for forward SCH
  struct
  {
    mccccl_sccl_entry_type sccl[CAI_MAX_NUM_PER_SCH_CFG];
  } for_sch_cfg[CAI_MAX_NUM_FOR_SCH];

  // Reverse supplemental configuration, not per pilot info.
  struct
  {
    uint8 walsh_id[CAI_MAX_NUM_PER_SCH_CFG];  // Can be 0 or 1 only
  } rev_sch_cfg[CAI_MAX_NUM_REV_SCH];

} ccl_object_type;

ccl_object_type ccl;

ccl_object_type ccl_old;
// Public view of CCL object
const mccccl_code_channel_list_object_type mccccl_code_channel_list =
{
  get_pilots,
  generate_aset,
  generate_aset_default_sups,
  initialize,
  update_pilots,
  update_all
  ,save,
  restore
  ,update_ccl_with_uhdm
  ,get_aset_info
  ,update_ccl_with_escam
  ,generate_sch_aset
  ,pilot_is_in_sch_aset
  ,activate_f_sch_assign
  ,activate_r_sch_assign
  ,deactivate_f_sch
  ,deactivate_r_sch
  ,get_sccl_entry
  ,get_r_sch_walsh
  ,pilot_is_in_active_set
  ,fwd_fundicated_chan_is_active
  ,set_chan_mask
  ,get_fwd_chan_mask
  ,get_rev_chan_mask
  ,gen_fwd_chan_mask
  ,gen_rev_chan_mask
  ,gen_fwd_chan_mask_erm
  ,gen_rev_chan_mask_erm
  ,set_chan_mask_erm
  ,get_ota_extchind

  ,in_control_hold
  ,exit_control_hold
  ,get_control_hold_info

#ifdef FEATURE_IS2000_REL_A_SVD
  ,set_fch_to_be_restored
  ,get_fch_to_be_restored
#endif

};

#ifdef FEATURE_IS2000_REL_A
////////////////////////////////
// Logging types and variables
////////////////////////////////

// Active set types

/*lint -e754 Lint complains that the reserved fields are not used, but they are
  needed so that the structures are 32 bits (and so they match the tools interface). */

/* Please note that words should always be aligned to an even address.  Add reserved
   bytes to ensure this (and the reserved bytes must also be in the tools interface). */

typedef struct
{
  uint16  walsh;  // Walsh code for auxiliary pilot
  uint8   qof;    // Quasi-orthogonal function index for the Auxiliary Transmit Diversity Pilot
} aux_pilot_fields_type;

/* Maximum buffer size required */
#define PILOT_TYPE_RECORD_TYPE_BUF_SIZE ( sizeof(caii_add_pilot_rec0_type) + \
                                        sizeof(aux_pilot_fields_type) )
typedef struct
{
  uint8    reference_buf[PILOT_TYPE_RECORD_TYPE_BUF_SIZE];

  /*
   *  Fill the Reference Buffer with the following:
   *
   *  - If PILOT_REC_TYPE is 000 or 010, include 1 instance of caii_add_pilot_rec0_type
   *    (This is named td_pilot_fields_type in interface documentation)
   *
   *  - If PILOT_REC_TYPE is 001 or 010, include 1 instance of AUX_PILOT_FIELDS_TYPE.
   *
   * - Then, include as many reserved bytes as necessary to complete a 32 bit boundary.
   */

} pilot_type_record_type;


typedef struct
{
  uint16  code_chan;    // Code channel
  uint8   qof_mask_id;  // Quasi-orthogonal function index
  uint8   reserved;
} channel_configuration_type;

/* Maximum buffer size required */
#define ACTIVE_SET_PILOT_RECORD_TYPE_BUF_SIZE ( sizeof(pilot_type_record_type) + \
                         sizeof(channel_configuration_type) * (2 + CAI_MAX_NUM_SUP) )

typedef struct
{
  uint16  pilot_pn;        // Pilot PN sequence offset index
  uint8   pilot_rec_type;  // Pilot record type
  uint8   reserved;

  /************************************/

  uint8   reference_buf[ACTIVE_SET_PILOT_RECORD_TYPE_BUF_SIZE];

  /*
   *  Fill the Reference Buffer with the following:
   *
   *  - If the pilot is not a common pilot, include 1 instance of
   *    PILOT_TYPE_RECORD_TYPE
   *
   *  - If the FCH is active, include one instance of
   *    CHANNEL_CONFIGURATION_TYPE for the FCH
   *
   *  - If the DCCH is active, include one instance of
   *    CHANNEL_CONFIGURATION_TYPE for the DCCH
   *
   *  - If NUM_SCH > 0, include NUM_SCCH instances of
   *    CHANNEL_CONFIGURATION_TYPE for SCCH.
   *
   */

} active_set_pilot_record_type;

typedef PACK (union)
{
  /*lint -e46 */
  PACK (struct)
  {
    uint8    fch        :1;  // FCH is active
    uint8    dcch       :1;  // DCCH is active
    uint8    num_f_scch :3;  // Number of supplimental code channels active [0..7]
    uint8    reserved   :3;
  } individual_channels;
  /*lint +e46 */

  uint8  mask;  /* Use to init entire mask. */

} channels_info_mask;


/* Maximum buffer size required */
#define LOG_ASET_INFO_VER_2_C_BUF_SIZE ( sizeof(active_set_pilot_record_type) * CCL_SIZE )

LOG_RECORD_DEFINE(LOG_ASET_INFO_VER_2_C)

  uint8   rf_mode;     // RF mode (analog, cdma...)
  uint8   band_class;  // Band class
  uint16  cdma_chan;   // CDMA channel

  /************************************/

  channels_info_mask channels;
  uint8              num_pilots;  // Number of pilots described in this log packet
  uint16             reserved;

  /************************************/

  uint8    reference_buf[LOG_ASET_INFO_VER_2_C_BUF_SIZE];

  /*
   *  Fill the Reference Buffer with NUM_PILOTS instances of
   *    ACTIVE_SET_PILOT_RECORD_TYPE
   */

LOG_RECORD_END




//  Reduced active set types

typedef union
{
  /*lint -e46 */
  struct
  {
    uint8 f_sch    :1;  // F-SCH is active
    uint8 r_sch    :1;  // R-SCH is active
    uint8 reserved :6;
  } individual_channels;
  /*lint +e46 */

  uint8  mask;  /*  This is only used to initialize values  */

} active_sch_type;

typedef struct
{
  uint16 sch_mux_option;    // R-SCH mux option
  uint8  sch_num_bits_idx;  // R-SCH rate
  uint8  sch_walsh_id;      // R-SCH walsh channel
} r_sch_info_type;

typedef struct
{
  uint16    pilot_pn;        // Pilot PN sequence offset index
  uint16    sch_mux_option;  // F-SCH mux option

  /************************************/

  uint8   num_bits_sch;  // SCH rate enum
  uint8   qof_mask_id;   // Quasi-orthogonal function index
  uint16  code_chan;
} reduced_active_set_pilot_type;

/* Maximum buffer size required */
#define REDUCED_ACTIVE_SET_SUB_RECORD_TYPE_BUF_SIZE (sizeof(r_sch_info_type) + \
                              sizeof(reduced_active_set_pilot_type) * CCL_SIZE + 1 )

typedef struct
{
  uint8            time_offset;
  // Indicates the number of frames (20 ms) between this and last sub_record
  //  If this is the first sub_record of this packet, this shall be set to "0"

  active_sch_type    active_sch;

  uint8    num_pilots;  // Number of pilots for this time_offset

  /************************************/

  uint8    reference_buf[REDUCED_ACTIVE_SET_SUB_RECORD_TYPE_BUF_SIZE];

  /*
   *  Fill the Reference Buffer with the following:
   *
   *  - If R_SCH is active, include 1 inscance of a R_SCH_INFO_TYPE
   *
   * - Then, if F_SCH is active, include NUM_PILOTS instances of a
   *    REDUCED_ACTIVE_SET_PILOT_TYPE
   *
   *  - Then, include 1 reserved byte.
   */

} reduced_active_set_sub_record_type;


/*
 * Reduced active set log is an accumulated log.  It periodically is
 * appended.  Once the buffer is full, it is flushed to the log thread,
 * and new records can be added again to the static buffer.
 *
 * Each pilot requires 16 bytes.  So a buffer of 512 can hold approx 32
 * pilots before having to be flushed.
 */
#define REDUCED_ACTIVE_SET_LOG_BUFFER_SIZE (512)

LOG_RECORD_DEFINE(LOG_REDUCED_ASET_INFO_C)

  uint8  num_sub_records;  // Number of pilot sets given in this log packets

  uint8  reference_buf[REDUCED_ACTIVE_SET_LOG_BUFFER_SIZE];

  /*
   *  Fill the Reference Buffer with NUM_SUB_RECORDS instances
   *    of a REDUCED_ACTIVE_SET_SUB_RECORD_TYPE
   */

LOG_RECORD_END


static uint8 raw_reduced_active_set_log_buffer[ REDUCED_ACTIVE_SET_LOG_BUFFER_SIZE ];
LOG_REDUCED_ASET_INFO_C_type * reduced_active_set_log_buffer =
  (LOG_REDUCED_ASET_INFO_C_type *) raw_reduced_active_set_log_buffer;

static uint8  reduced_active_set_num_sub_records = 0;

static uint16 reduced_active_set_offset = 0;

static qword reduced_active_set_last_timestamp = {0,0};

#endif // FEATURE_IS2000_REL_A



#ifdef FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION mccccl_submit_reduced_active_set_log

DESCRIPTION
  Submits the current accumulated contents of the reduced active set log to
  the log manager.  This is done automatically by
  append_reduced_active_set_log() when the buffer is full.  It should also
  be done at call termination.

DEPENDENCIES
  Shares global variables with append_reduced_active_set_log.  Only one
  of these two functions may be executing at any time.  Concurrent execution
  will lead to race condition defects.

RETURN VALUE
  None.

SIDE EFFECTS
  Resets counter and offset global variables that are used by
  append_reduced_active_set_log().
  Adds a log packet to the logging thread's buffer.

===========================================================================*/

void mccccl_submit_reduced_active_set_log ( void )
{
  // If the log is empty, do not submit to the log thread.
  if ( reduced_active_set_num_sub_records == 0)
  {
     return;
  }

  // Set the number of sub records in this log.
  reduced_active_set_log_buffer->num_sub_records =
    reduced_active_set_num_sub_records;

  // Write the header information required by the log thread
  log_set_code
  (
    raw_reduced_active_set_log_buffer,
    LOG_REDUCED_ASET_INFO_C
  );
  log_set_length
  (
    raw_reduced_active_set_log_buffer,
    FPOS(LOG_REDUCED_ASET_INFO_C_type, reference_buf) + reduced_active_set_offset
  );

  (void)log_submit( raw_reduced_active_set_log_buffer );

  // Re-initialize the offset and counter for the next log packet
  reduced_active_set_num_sub_records = 0;
  reduced_active_set_offset = 0;
  qw_set( reduced_active_set_last_timestamp, 0, 0 );

}

/*===========================================================================

FUNCTION append_reduced_active_set_log

DESCRIPTION
  Appends a new log sub-record to the end of the log buffer containing
  current supplemental channel information.  If the log buffer is full,
  calls mccccl_submit_reduced_active_set_log().

DEPENDENCIES
  Requires that the ccl.pilots data structure is not updated during the
  execution of this function.  Otherwise, race condition defects may
  occur.

RETURN VALUE
  None.

SIDE EFFECTS
  Modifies counter and offset global variables, and writes into the global
  reduced active set log buffer.

===========================================================================*/

void append_reduced_active_set_log ( void )
{
  qword  timestamp = { 0, 0};          // Current timestamp
  qword  timestamp_diff = { 0, 0};     // Difference between current timestamp
                             // and previous timestamp
  uint8  num_pilots = 0;     // Number of active pilots, counted in loop below
  uint16 sub_record_offset;  // Counts bytes written to within the current
                             // sub_record being appended to the log
  uint16 pilot_index;        // Index into ccl.pilots[]
  uint16 for_sch_index;      // Index into ...sup_info.for_sch[]

  // Pointers within variable length portion of log packet
  reduced_active_set_sub_record_type  *sub_record_ptr;
  r_sch_info_type  *r_sch_info_ptr;
  reduced_active_set_pilot_type *pilot_info_ptr;

  // Pointer to current configuration
  const caii_srv_cfg_type* cfg_ptr;

  // Get pointer to current configuration
  cfg_ptr = snm_get_current_config();

  /* Initialized timestamp to zero. */
  qw_set(timestamp, 0, 0);

  /* Count F-SCH pilots.  This is used in the length calculation below. */
  for (pilot_index = 0; pilot_index < CCL_SIZE; pilot_index++)
  {
    if (ccl.pilots[pilot_index].pilot_state == MCCCCL_ACTIVE)
    {
      for
      (
        for_sch_index = 0;
        for_sch_index < CAI_MAX_NUM_FOR_SCH;
        for_sch_index++
      )
      {
        if (ccl.pilots[pilot_index].sup_info.for_sch[for_sch_index].active)
        {
          num_pilots++;
        } // if sch active
      } // loop over sch
    } // if pilot active
  } // loop over pilots

  /* Compute the length of the record and check if it can fit within the max
     log size. */
  if ((FPOS(LOG_REDUCED_ASET_INFO_C_type, reference_buf)
       + reduced_active_set_offset  /* This accounts for previous subrecords
                                       that were already appended
                                       Remainder of sum is for new record being added. */
       + FPOS(reduced_active_set_sub_record_type, reference_buf)
       + (cfg_ptr->rev_sch_cc_incl ? cfg_ptr->num_rev_sch : 0) * sizeof(r_sch_info_type)
       + (cfg_ptr->for_sch_cc_incl ? cfg_ptr->num_for_sch : 0) * num_pilots
       * sizeof(reduced_active_set_pilot_type)
       /* Add one for the reserved byte at the end of each record. */
       + 1
      )
      > REDUCED_ACTIVE_SET_LOG_BUFFER_SIZE )
  {
    /* If this new record would exceed the log, send the log first. */
    mccccl_submit_reduced_active_set_log();
  }

  /* Initialize sub_record_ptr pointer */
  sub_record_ptr = (reduced_active_set_sub_record_type *) &(reduced_active_set_log_buffer->reference_buf[ reduced_active_set_offset ]); /*lint !e826 */

  // Get current count of 20ms frames
  m1x_time_get_20ms_frame_time( timestamp );

  // Compute the time_offset field.
  // This value shall only be set to zero in the first sub_record in this
  // log packet.  For other sub_records, it contains the number of frames
  // elapsed between the current and last sub_record.
  if ( (qw_hi(reduced_active_set_last_timestamp) == 0)
     && (qw_lo(reduced_active_set_last_timestamp) == 0) )
  {
    // This conditional succeeds only if this is the first subrecord
    // of the current log packet.  The time is initialized to 0 at
    // application start, and is reset to 0 each time the log is
    // submitted to the log thread.
    sub_record_ptr->time_offset = 0; // first sub_record of this log

    // Log gets the timestamp of the first sub_record in the log.
    log_set_timestamp( raw_reduced_active_set_log_buffer );
  }
  else // Not the first subrecord of this log packet
  {
    qw_sub(timestamp_diff, timestamp, reduced_active_set_last_timestamp);
    sub_record_ptr->time_offset = (uint8) qw_lo(timestamp_diff);
  }

  // Save timestamp for next record's calculation
  qw_equ(reduced_active_set_last_timestamp, timestamp);

  sub_record_ptr->active_sch.mask = 0;
  sub_record_ptr->active_sch.individual_channels.f_sch =
    (num_pilots > 0) ? 1 : 0;
  sub_record_ptr->active_sch.individual_channels.r_sch =
    ccl.rev_sch[0].active ? 1 : 0;

  sub_record_ptr->num_pilots = num_pilots;

  /*  Update offsets  */
  reduced_active_set_offset += (uint16) FPOS(reduced_active_set_sub_record_type, reference_buf);

  /*  Initialize offset to fill buffer  */
  sub_record_offset = 0;

  /*  Check if R-SCH is active  */
  if (sub_record_ptr->active_sch.individual_channels.r_sch)
  {
    /*  Initialize r_sch_info_ptr pointer  */
    r_sch_info_ptr = (r_sch_info_type *)
      &sub_record_ptr->reference_buf[sub_record_offset];

    r_sch_info_ptr->sch_mux_option = cfg_ptr->rev_sch[0].rev_sch_mux;
    r_sch_info_ptr->sch_num_bits_idx = ccl.rev_sch[0].rate;  /*lint !e641 */
    r_sch_info_ptr->sch_walsh_id = ccl.rev_sch[0].walsh_id;

    /*  Update offset  */
    sub_record_offset += sizeof(r_sch_info_type);
  }


  /*  If F_SCH is active, store the reduced active set  */
  if (sub_record_ptr->active_sch.individual_channels.f_sch)
  {
    for (pilot_index = 0; pilot_index < CCL_SIZE; pilot_index++)
    {
      if (ccl.pilots[pilot_index].pilot_state == MCCCCL_ACTIVE)
      {
        for
        (
          for_sch_index = 0;
          for_sch_index < CAI_MAX_NUM_FOR_SCH;
          for_sch_index++
        )
        {
          if (ccl.pilots[pilot_index].sup_info.for_sch[for_sch_index].active &&
              sub_record_offset < REDUCED_ACTIVE_SET_SUB_RECORD_TYPE_BUF_SIZE)
          {


            /*  Initialize pilot_info_ptr pointer  */
            pilot_info_ptr = (reduced_active_set_pilot_type *)
              &sub_record_ptr->reference_buf[sub_record_offset];

            pilot_info_ptr->pilot_pn = ccl.pilots[pilot_index].pilot_pn;
            pilot_info_ptr->sch_mux_option = cfg_ptr->for_sch[for_sch_index].for_sch_mux;
            pilot_info_ptr->num_bits_sch =ccl.pilots[pilot_index].sup_info.for_sch[for_sch_index].sch_rate; /*lint !e641 */
            pilot_info_ptr->qof_mask_id =ccl.pilots[pilot_index].sup_info.for_sch[for_sch_index].qof_mask_id_sch;
            pilot_info_ptr->code_chan = ccl.pilots[pilot_index].sup_info.for_sch[for_sch_index].code_chan_sch;

            sub_record_offset += sizeof(reduced_active_set_pilot_type);
          } // if sch active
        } // loop over sch
      } // if pilot active
    } // loop over pilots
  }  /*  End of if F_SCH is active  */

  /*  Update sub_record_offset with reserved fields  */
  sub_record_offset ++;

  /*  Update offset with sub_record_offset  */
  reduced_active_set_offset += sub_record_offset;

  // Verify that the buffer has not been exceeded.
  // This should only occur in the case of a race.
  if ( (FPOS(LOG_REDUCED_ASET_INFO_C_type, reference_buf) + reduced_active_set_offset)
       > REDUCED_ACTIVE_SET_LOG_BUFFER_SIZE )
  {
    ERR_FATAL("Exceeded reduced active set buffer",0,0,0);
  }

  /*  Update number of sub_records  */
  reduced_active_set_num_sub_records ++;
}

/*===========================================================================

FUNCTION mccccl_log_aset_info

DESCRIPTION
  If enabled, this procedure logs active set and channel configuration. This
  function replaces the previous two mccccl_log_aset.

DEPENDENCIES
   Requires that the set of active pilots is unchanging during the execution
   of this function.  Memory is allocated based on the number of pilots, and
   their types.

RETURN VALUE
  None.

SIDE EFFECTS
  Adds a log packet to the logging thread's buffer.

===========================================================================*/

void mccccl_log_aset_info ( void )
{
  unsigned int buffer_size;               // Size of allocated buffer
  unsigned int  provisional_size;   // Number of bytes written to buffer
  unsigned int offset;              // Tracks position in variable sized
                                    // reference buffer
  uint16  pilot_info_offset;
  uint8 num_pilots;                 // Number of active pilots
  unsigned int pilot_record_offset;
  int pilot_index;                  // Loop index for ccl.pilots[]
  uint8 sup_code_chan_index;          // Loop index for ...scch.sup_code_chan[];

  // Pointers into variable length portions of the log buffer
  active_set_pilot_record_type *pilot_record_ptr;
  pilot_type_record_type       *pilot_info_ptr;
  channel_configuration_type   *chan_conf_ptr;
  caii_add_pilot_rec0_type     *td_pilot_fields_ptr;
  aux_pilot_fields_type        *aux_pilot_fields_ptr;

  LOG_ASET_INFO_VER_2_C_type *active_set_ptr;  // Pointer to allocated log buffer

  // Current RF mode (analog, CDMA, PCN)
  db_items_value_type diag_db;

  /*
   * Preallocate memory for log buffer.
   * This requires a calculation of the memory that will
   * be required.
   */

  num_pilots=0;
  buffer_size = FPOS(LOG_ASET_INFO_VER_2_C_type, reference_buf);

  // Now, add requirements for each active pilot
  for ( pilot_index = 0; pilot_index < CCL_SIZE; pilot_index++ )
  {
    if ( ccl.pilots[pilot_index].pilot_state == MCCCCL_ACTIVE )
    {
      // -1 below accounts for reference_buf
      buffer_size += FPOS(active_set_pilot_record_type, reference_buf)

      /* Code channel and qof mask id apply to forward link channels */
      + ( (ccl.fwd_chan_mask & MCC_F_FCH) ? sizeof(channel_configuration_type) : 0 )
      + ( (ccl.fwd_chan_mask & MCC_F_DCCH) ? sizeof(channel_configuration_type) : 0 )
      + ccl.pilots[pilot_index].sup_info.scch.num_sup_chan
        * (unsigned) sizeof(channel_configuration_type);

      if ( ccl.pilots[pilot_index].pilot_rec_type != 0xFF ) /*lint !e641 */
      {
        buffer_size += 8;  // This is the max size of a pilot_type_record.
      }
      num_pilots++;
    }
  }

  // Allocate memory for the log buffer
  active_set_ptr = ( LOG_ASET_INFO_VER_2_C_type *) log_alloc
  (
    LOG_ASET_INFO_VER_2_C,
    buffer_size
  );

  // If log buffer could not be allocated, return without writing to the
  // pointer.
  if (!active_set_ptr)
  {
     return;
  }

  // Retrieve current RF mode (analog, CDMA, PCN)
  db_get ( DB_RF_MODE, &diag_db );

  /*  Fill log record  */
  active_set_ptr->rf_mode = diag_db.rf_mode; /*lint !e641 */
  active_set_ptr->band_class = ccl.band_class;
  active_set_ptr->cdma_chan = ccl.cdma_channel;

  active_set_ptr->channels.mask = 0;
  /* Forward channels are relevant for active set information */
  active_set_ptr->channels.individual_channels.fch = ( (ccl.fwd_chan_mask & MCC_F_FCH) ? TRUE : FALSE );
  active_set_ptr->channels.individual_channels.dcch =( (ccl.fwd_chan_mask & MCC_F_DCCH) ? TRUE : FALSE );
  if (ccl.sups_active)
  {
    // Find the max num_f_scch over all pilots.
    active_set_ptr->channels.individual_channels.num_f_scch = 0; // Initialize
    for ( pilot_index = 0; pilot_index < CCL_SIZE; pilot_index++ )
    {
      if ( ( ccl.pilots[pilot_index].pilot_state == MCCCCL_ACTIVE )
         && ( active_set_ptr->channels.individual_channels.num_f_scch < ccl.pilots[pilot_index].sup_info.scch.num_sup_chan ) )
      {
        active_set_ptr->channels.individual_channels.num_f_scch = ccl.pilots[pilot_index].sup_info.scch.num_sup_chan;
      }
    }
  }
  else
  {
    active_set_ptr->channels.individual_channels.num_f_scch = 0;
  }

  // num_pilots was calculated in the loop above
  active_set_ptr->num_pilots = num_pilots;

  /*  Update provisional size (including reserved fields)  */
  provisional_size = FPOS(LOG_ASET_INFO_VER_2_C_type, reference_buf);

  /*  Initialize offset to fill reference buffer  */
  offset = 0;

  /*  Fill variable length fields  */
  // loop through all pilot indecies
  for ( pilot_index = 0; pilot_index < CCL_SIZE; pilot_index++ )
  {
    if ( ccl.pilots[pilot_index].pilot_state != MCCCCL_ACTIVE )
    {
       // Continue is used to avoid placing this whole loop body
       // within the context of a conditional as well.
       continue;
    }

    /*  Initialize pilot_record_ptr pointer  */
    pilot_record_ptr = (active_set_pilot_record_type *)
      &active_set_ptr->reference_buf[offset];

    pilot_record_ptr->pilot_pn = ccl.pilots[pilot_index].pilot_pn;
    pilot_record_ptr->pilot_rec_type = ccl.pilots[pilot_index].pilot_rec_type; /*lint !e641 */

    offset += FPOS(active_set_pilot_record_type, reference_buf);

    // Initialize pilot_record_offset to fill reference buffer in pilot record
    pilot_record_offset = 0;

    /*********************************/
    /*  Fill pilot_record structure  */
    /*********************************/

    /* If it's not a common pilot in the list. */
    if ( pilot_record_ptr->pilot_rec_type != 0xFF )
    {
      /*  Initialize pilot_info_ptr pointer  */
      pilot_info_ptr = (pilot_type_record_type *)
        &pilot_record_ptr->reference_buf[pilot_record_offset];

      /*  Initialize pilot_info_offset to fill buffer in pilot info  */
      pilot_info_offset = 0;

      /*  Fill fields in pilot record structure  */
      if (pilot_record_ptr->pilot_rec_type == 0x00
         || pilot_record_ptr->pilot_rec_type == 0x02)
      {
        /*  Initialize td_pilot_fields_ptr pointer  */
        td_pilot_fields_ptr = ( caii_add_pilot_rec0_type *)
          &pilot_info_ptr->reference_buf[pilot_info_offset];

        if (pilot_record_ptr->pilot_rec_type == 0x00)
        {
          td_pilot_fields_ptr->td_power_level =
            ccl.pilots[pilot_index].pilot_rec.rec0.td_power_level;
          td_pilot_fields_ptr->td_mode =
            ccl.pilots[pilot_index].pilot_rec.rec0.td_mode;
        }
        else // pilot_rec_type == 0x02
        {
          td_pilot_fields_ptr->td_power_level =
            ccl.pilots[pilot_index].pilot_rec.rec2.aux_td_power_level;
          td_pilot_fields_ptr->td_mode =
            ccl.pilots[pilot_index].pilot_rec.rec2.td_mode;
        }

        /*  Increment offset  */
        pilot_info_offset += sizeof(caii_add_pilot_rec0_type);
      }

      if (pilot_record_ptr->pilot_rec_type == 0x01
         || pilot_record_ptr->pilot_rec_type == 0x02)
      {
        /*  Initialize aux_pilot_fields_ptr pointer  */
        aux_pilot_fields_ptr = (aux_pilot_fields_type *)
          &pilot_info_ptr->reference_buf[pilot_info_offset];

        if (pilot_record_ptr->pilot_rec_type == 0x01)
        {
          aux_pilot_fields_ptr->walsh =
            ccl.pilots[pilot_index].pilot_rec.rec1.aux_pilot_walsh;
          aux_pilot_fields_ptr->qof =
            ccl.pilots[pilot_index].pilot_rec.rec1.qof;
          // Place the walsh length in the high 3 bits of the walsh.
          aux_pilot_fields_ptr->walsh |=
            ((uint16)ccl.pilots[pilot_index].pilot_rec.rec1.walsh_length) << 13; /*lint !e734*/
        }
        else // pilot_rec_type == 0x02
        {
          aux_pilot_fields_ptr->walsh =
            ccl.pilots[pilot_index].pilot_rec.rec2.aux_walsh;
          aux_pilot_fields_ptr->qof =
            ccl.pilots[pilot_index].pilot_rec.rec2.qof;
          // Place the walsh length in the high 3 bits of the walsh.
          aux_pilot_fields_ptr->walsh |=
            ((uint16) ccl.pilots[pilot_index].pilot_rec.rec2.walsh_length) << 13; /*lint !e734*/
        }

        /*  Increment offset  */
        pilot_info_offset += sizeof(aux_pilot_fields_type);
      }

      // Check if number of bytes in the pilot record is multiple
      // of 4 (32 bits) and add as many reserved bytes as necessary
      if (pilot_info_offset & 0x0003)
      {
        pilot_info_offset = (pilot_info_offset & 0xFFFC) + 0x0004;
      }

      /*  Update pilot_record_offset with pilot_info_offset value  */
      pilot_record_offset += pilot_info_offset;
    }

    /*********************************/
    /*  Fill channel configuration    */
    /*********************************/
    if (active_set_ptr->channels.individual_channels.fch)
    {
      /*  Initialize chan_conf_ptr pointer to fill fch channel configuration  */
      chan_conf_ptr = (channel_configuration_type *)
        &pilot_record_ptr->reference_buf[pilot_record_offset];

      chan_conf_ptr->code_chan = ccl.pilots[pilot_index].fch_code_chan.code_chan;
      chan_conf_ptr->qof_mask_id =
        ccl.pilots[pilot_index].fch_code_chan.qof_mask_id;

      pilot_record_offset += sizeof(channel_configuration_type);
    }

    if (active_set_ptr->channels.individual_channels.dcch)
    {
      /*  Initialize chan_conf_ptr pointer to fill DCCH channel configuration  */
      chan_conf_ptr = (channel_configuration_type *)
        &pilot_record_ptr->reference_buf[pilot_record_offset];

      chan_conf_ptr->code_chan = ccl.pilots[pilot_index].dcch_code_chan.code_chan;
      chan_conf_ptr->qof_mask_id =
        ccl.pilots[pilot_index].dcch_code_chan.qof_mask_id;

      pilot_record_offset += sizeof(channel_configuration_type);
    }

    for
    (
      sup_code_chan_index=0;
      ((sup_code_chan_index<ccl.pilots[pilot_index].sup_info.scch.num_sup_chan) &&
       (sup_code_chan_index<CAI_MAX_NUM_SUP));
      sup_code_chan_index++
    )
    {
      /*  Initialize chan_conf_ptr pointer to fill fch channel configuration  */
      chan_conf_ptr = (channel_configuration_type *)
        &pilot_record_ptr->reference_buf[pilot_record_offset];

      chan_conf_ptr->code_chan = ccl.pilots[pilot_index].sup_info.scch.sup_code_chan[sup_code_chan_index];
      chan_conf_ptr->qof_mask_id = 0; // qof does not apply for scch

      pilot_record_offset += sizeof(channel_configuration_type);
    }

    /*  Update offset with pilot_record offset  */
    offset += pilot_record_offset;

  }  /*  End of loop through pilots in the active set  */

  provisional_size += offset;

  // Verify that the buffer has not been overrun.
  if (provisional_size > buffer_size)
  {
     ERR_FATAL("Log exceeds buffer length", 0,0,0);
  }
  else if (provisional_size != buffer_size)
  {
    log_shorten
    (
      active_set_ptr,
      provisional_size
    );
    log_set_length
    (
      active_set_ptr,
      provisional_size
    );
  }

  // Mark log packet as ready to be sent to the server
  log_commit(active_set_ptr);
}

#else // ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION mccccl_log_aset_info

DESCRIPTION
  If enabled, this procedure logs active set and channel configuration. This
  function replaces the old mccccl_log_aset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccl_log_aset_info (void)
{
  db_items_value_type diag_db;
  log_aset_info_type log;
  byte fsch_rate;
  mclog_aset_rsch_type rsch;

  if (log_status(LOG_ASET_INFO_C))
  {
    /* Fill in header of log. */
    log.hdr.len  =  sizeof(LOG_ASET_INFO_C_type);
    log.hdr.code = LOG_ASET_INFO_C;
    (void) m1x_time_get(log.hdr.ts);

    db_get ( DB_RF_MODE, &diag_db );
    log.rf_mode = diag_db.rf_mode;
    log.band_class = ccl.band_class;
    log.curr_chan  = ccl.cdma_channel;  //lint !e734
    log.num_pilots = mccccl_code_channel_list.get_aset_info (log.aset, &fsch_rate, &rsch) ;
    log.fsch_rate = fsch_rate;
    log.rsch=rsch;

    /* Copy log buffer to log output buffer */
    (void)log_submit(&log);
  }
}

#endif // FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION mccccl_initialize

DESCRIPTION
  Performs some initialization for the code channel list object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccl_initialize
(
  void
)
{
  uint8 cclp;
  uint8 sch, index;

  // Clear out chind and ext_chind fields
  ccl.ota_chind = CAI_CHIND_RESERVED_1;
  ccl.ota_extchind = CAI_EXT_CHIND_UNSET;

  // Mark all the pilots as invalid...
  for ( cclp = 0; cclp < CCL_SIZE; ++cclp )
  {
    mark_pilot_as_invalid( cclp );
  }

  // Clear out all F-SCH configuration
  for ( sch = 0; sch < CAI_MAX_NUM_FOR_SCH; ++sch )
  {
    for ( index = 0; index < CAI_MAX_NUM_PER_SCH_CFG; ++index )
    {
      ccl.for_sch_cfg[sch].sccl[index].num_pilots = 0;
    }
  }

} /* mccccl_initialize */

/*===========================================================================

FUNCTION initialize

DESCRIPTION
  Initialize the code channel list with pilots and fundamental code channels.
  This function is called after receiving channel assignment (CAM or ECAM) from
  the base station.

DEPENDENCIES
  None.

RETURN VALUE
  MCCCCL_SUCCESS if CCL initialized.  MCCCCL_FAILURE if invalid num_pilots passed in.

SIDE EFFECTS
  None.

===========================================================================*/

static void initialize
(
  byte band_class,
  word cdma_channel,
  byte num_pilots,                      /* Total number of pilots */
  mccdma_pilot_info_type pilot_array[], /* Pilot PNs and code channels */
  mccdma_chnasn_type *chnasn_ptr        /* The received ECAM */
)
{
  uint8 bn;  // general purpose byte
  uint8 sch, index;

  mccdma_ca_ord_type *ca_ptr;
  cai_chind_type ota_chind;
  cai_ext_chind_type ota_extchind = CAI_EXT_CHIND_UNSET;

  ca_ptr = &chnasn_ptr->mode;

  ota_chind = ca_ptr->am0.ch_ind;
  /* If the chind is 0, we are in DV, and the chind should stay 0. */
  if (ota_chind != CAI_CHIND_RESERVED_1)
  {
    /* Set rev pilot bit even when RC<3.  When RC<3 the bit is meaningless. */
    ota_chind |= CAI_CHIND_REV_PILOT;
  }

  // Although CCl has N6m*2 entries, it can only be initialized with up to
  // N6m active pilots.
  ONEX_ASSERT( num_pilots > 0 && num_pilots <= CAI_N6M );

  // Just in case ASSERT didn't catch all problem during development, fix
  // num_pilots here.
  if (num_pilots > CAI_N6M)
  {
    num_pilots = CAI_N6M;
  }

  /* Initialize CCL to nothing first */
  mccccl_initialize();

  /* Update band class and CDMA channel number */
  ccl.band_class = band_class;
  ccl.cdma_channel = cdma_channel;

  /* Update channel indicator mask */
  set_chan_mask(ota_chind, ota_extchind);

  M1X_MSG( DCP, LEGACY_MED,
           "Update CCL with CAM/ECAM, no of pilots = %d",
            num_pilots );

  /* For each pilot, update the fundamental code channel, and/or DCCH code
     channel and/or DV channels and clear out SCCH. */
  for (bn = 0; bn < num_pilots; ++bn)
  {
    ccl.pilots[bn].pilot_state = MCCCCL_ACTIVE;
    ccl.pilots[bn].order = bn;
    ccl.pilots[bn].pilot_pn = pilot_array[bn].pilot_pn;
    ccl.pilots[bn].pwr_comb_ind = pilot_array[bn].pwr_comb_ind;
    ccl.pilots[bn].srch_offset_nghbr = 0;

    /* The add_pilot_rec_incl is always inited correctly irrespective
    of the p_rev_in_use */
    if (pilot_array[bn].add_pilot_rec_incl)
    {
      ccl.pilots[bn].pilot_rec_type = (cai_pilot_rec_type) pilot_array[bn].pilot_rec_type;
      ccl.pilots[bn].pilot_rec = pilot_array[bn].add_pilot_rec;
    }
    else
    {
      ccl.pilots[bn].pilot_rec_type = CAI_PILOT_1X_COMMON;
    }

    if (ccl.fwd_chan_mask & MCC_F_DCCH)
    {
      /* Update DCCH code_chan/qof info if forward DCCH is assigned */
      ccl.pilots[bn].dcch_code_chan.code_chan = pilot_array[bn].code_chan_dcch;
      ccl.pilots[bn].dcch_code_chan.qof_mask_id = pilot_array[bn].qof_mask_id_dcch;
    }

    if (ccl.fwd_chan_mask & MCC_F_FCH)
    {
      /* Update FCH info if forward FCH is assigned */
      ccl.pilots[bn].fch_code_chan.code_chan = pilot_array[bn].code_chan_fch;
      /* QOF mask id introduced for FEATURE_IS2000 */
      ccl.pilots[bn].fch_code_chan.qof_mask_id = pilot_array[bn].qof_mask_id_fch;
    }
  }

  /* Spec says PWR_COMB_IND for first pilot is zero */
  if (ccl.pilots[0].pwr_comb_ind != 0)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "First pilot should have PWR_COMB_IND=0");
  }

  // Clear out all F-SCH configuration and initialize R-SCH config.
  // We can use the same loop here for convenience since CAI_MAX_NUM_FOR_SCH and
  // CAI_MAX_NUM_REV_SCH are the same.
  for ( sch = 0; sch < CAI_MAX_NUM_FOR_SCH; ++sch )
  {
    for ( index = 0; index < CAI_MAX_NUM_PER_SCH_CFG; ++index )
    {
      // Clear out F-SCH config
      ccl.for_sch_cfg[sch].sccl[index].num_pilots = 0;
      // Initialize R-SCH config.  Hardcode numbers used here since that's some magic
      // number used in the standard.  Refer to IS-2000 Vol. 5 section 2.6.4.2.
      if ( sch == 0 )
      {
        ccl.rev_sch_cfg[sch].walsh_id[index] = ( index <= 3 ) ? 1 : 0;
      }
      else
      {
        ccl.rev_sch_cfg[sch].walsh_id[index] = ( index <= 2 ) ? 1 : 0;
      }
    }
  }

  // Clear out R-SCH current assignment
  for ( sch = 0; sch < CAI_MAX_NUM_REV_SCH; ++sch )
  {
    ccl.rev_sch[sch].active = FALSE;
  }

  // Mark sup_info as carrying SCCH as default...
  ccl.sup_state = MCCCCL_SCCH;

  // ... and they are not active
  ccl.sups_active = FALSE;

  /* Log the active set */
  mccccl_log_aset_info();
#if defined FEATURE_IS2000_REL_A_SVD
  ccl.fch_to_be_restored = FALSE;
#endif /* FEATURE_IS2000_REL_A_SVD */

} /* initialize */

/*===========================================================================

FUNCTION update_ccl_with_uhdm

DESCRIPTION
  This function updates the CCL upon receiving an UHDM, which updates the pilots
  and possibly the SCH assignment and configuration.  Note that UHDM cannot update
  SCCH.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void update_ccl_with_uhdm
(
  caii_ho_msg_type* uhdm
)
{
  uint8 cclp, uhdmp, bn, empty;  // Some loop variables, ccl pilot, uhdm pilot, etc
  boolean index_updated[CAI_MAX_NUM_PER_SCH_CFG][CAI_MAX_NUM_FOR_SCH];

  cai_ext_chind_type ota_extchind = CAI_EXT_CHIND_UNSET;
  mcc_fwd_chan_mask_type fwd_chan_mask;

  // Initialize array to indicate if an index have been updated
  for ( bn = 0; bn < CAI_MAX_NUM_PER_SCH_CFG; ++bn )
  {
    index_updated[bn][0] = FALSE;
    index_updated[bn][1] = FALSE;
  }

  // Just in case BS sends too many pilots, fix num_pilots here.
  if ( uhdm->num_pilots > CAI_N6M )
  {
    uhdm->num_pilots = CAI_N6M;
  }

  // Step 1:
  // First update the band class, channel, and chind
  M1X_MSG( DCP, LEGACY_MED,
           "Update ccl with UHDM, no. of uhdm pilots = %d",
            uhdm->num_pilots );
  if ( uhdm->extra_parms )
  {
    if ( ccl.band_class != uhdm->extra.band_class || ccl.cdma_channel != uhdm->extra.cdma_freq )
    {
      ccl.band_class = uhdm->extra.band_class;
      ccl.cdma_channel = uhdm->extra.cdma_freq;

      // Invalidate all pilot entries
      mccccl_initialize();

      // Note that we can leave the configuration entries alone.  If the
      // base station want to use them, they need to have them properly
      // updated.
    } // if ( ccl.band_class != ...
  }

  set_chan_mask( (cai_chind_type) uhdm->ch_ind, ota_extchind );
  fwd_chan_mask = get_fwd_chan_mask();

  // If SCH info. is included, mark CCL as such
  if ( uhdm->sch_incl )
  {
    ccl.sup_state = MCCCCL_SCH;
  }

  // Step 2:
  // Look at each pilot in the uhdm.  If the pilot does not match
  // anyone in ccl, then we locate a empty entry in CCL and put it there.
  // Otherwise, the entry is updated with the new information.  Bear in mind
  // that the SCH configuartion info. is additive - can't erase the old info. if
  // info for a specific entry is not given.
  for ( uhdmp = 0; uhdmp < uhdm->num_pilots; ++uhdmp )
  {
    for ( cclp = 0, empty = CCL_SIZE; cclp < CCL_SIZE; ++cclp )
    {
      // If we find the same pilot in CCL, break and process below
      if ( ccl.pilots[cclp].pilot_pn == uhdm->pilot[uhdmp].pilot_pn )
      {
        break;
      }
      // If we find an empty entry, and it's the first one, remember it
      if ( ccl.pilots[cclp].pilot_state != MCCCCL_ACTIVE &&
           empty == CCL_SIZE )
      {
        empty = cclp;
      }
    }
    if ( cclp == CCL_SIZE )     // New pilot not in CCL
    {
      if ( empty == CCL_SIZE )  // Can't find empty entry (should be impossible)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "CCL out of entries: Pilot %d",
          uhdm->pilot[uhdmp].pilot_pn );
        return;
      }
      else
      {
        cclp = empty;               // Point to empty entry to be updated
      }
    }

    // We can just update all the information as if this is a new entry
    // as long as we make sure that unspecified SCH config entries are not
    // overwritten.

    ccl.pilots[cclp].pilot_state = MCCCCL_ACTIVE;
    ccl.pilots[cclp].order       = uhdmp;
    ccl.pilots[cclp].pilot_pn    = uhdm->pilot[uhdmp].pilot_pn;

    if (uhdm->pilot[uhdmp].add_pilot_rec_incl)
    {
      ccl.pilots[cclp].pilot_rec_type = (cai_pilot_rec_type) uhdm->pilot[uhdmp].pilot_rec_type;
      ccl.pilots[cclp].pilot_rec = uhdm->pilot[uhdmp].add_pilot_rec;
    }
    else
    {
      ccl.pilots[cclp].pilot_rec_type = CAI_PILOT_1X_COMMON;
    }

    // If FCH is assigned in the UHDM, update FCH
    if ( fwd_chan_mask & MCC_F_FCH)
    {
      ccl.pilots[cclp].fch_code_chan.code_chan   = uhdm->pilot[uhdmp].code_chan_fch;
      ccl.pilots[cclp].fch_code_chan.qof_mask_id = uhdm->pilot[uhdmp].qof_mask_id_fch;
    }
    // If DCCH is assigned in the UHDM, update DCCH
    if ( fwd_chan_mask & MCC_F_DCCH )
    {
      ccl.pilots[cclp].dcch_code_chan.code_chan   = uhdm->pilot[uhdmp].code_chan_dcch;
      ccl.pilots[cclp].dcch_code_chan.qof_mask_id = uhdm->pilot[uhdmp].qof_mask_id_dcch;
    }
    ccl.pilots[cclp].pwr_comb_ind = uhdm->pilot[uhdmp].pwr_comb_ind;
    ccl.pilots[cclp].srch_offset_nghbr = uhdm->pilot[uhdmp].srch_offset;

    // If SCH config. info is included, proceed with updating the pilot
    // specific F-SCH config.  We still have to update the non-pilot specific
    // info later.
    if ( uhdm->sch_incl )
    {
      uint8 sch, index;

      for ( bn = 0; bn < uhdm->pilot[uhdmp].num_sch; ++bn )
      {
        // Retrive sch ID and SCCL index for ease of reading
        sch = uhdm->pilot[uhdmp].chan_rec.sch_chan_rec[bn].for_sch_id;
        index = uhdm->pilot[uhdmp].chan_rec.sch_chan_rec[bn].sccl_index;

        // This is a new entry to SCCL, we need to erase it first as mandated
        // by the Standard (in UHDM processing)
        if ( !index_updated[index][sch] )
        {
          ccl.for_sch_cfg[sch].sccl[index].num_pilots = 0;
          index_updated[index][sch] = TRUE;   // Remember this has been updated
        }

        // We can use num_pilots here directly as index to the pilots in SCCL
        // since the Standards mandates that the UHDM can only update all
        // pilots in a particular SCCL entry, but not some of them, i.e. we
        // will overwrite the whole entry
        if ( uhdm->pilot[uhdmp].chan_rec.sch_chan_rec[bn].pilot_incl )
        {
          uint8 scclp;

          // Retrive num_pilots for readability.  Note: post-increment below.
          scclp = ccl.for_sch_cfg[sch].sccl[index].num_pilots++;
          ccl.for_sch_cfg[sch].sccl[index].pilots[scclp].pilot_pn =
            uhdm->pilot[uhdmp].pilot_pn;
          if (uhdm->pilot[uhdmp].add_pilot_rec_incl)
          {
            ccl.for_sch_cfg[sch].sccl[index].pilots[scclp].pilot_rec_type =
               (cai_pilot_rec_type) uhdm->pilot[uhdmp].pilot_rec_type;
            ccl.for_sch_cfg[sch].sccl[index].pilots[scclp].pilot_rec =
               uhdm->pilot[uhdmp].add_pilot_rec;
          }
          else
          {
            ccl.for_sch_cfg[sch].sccl[index].pilots[scclp].pilot_rec_type =
                CAI_PILOT_1X_COMMON;
          }
          ccl.for_sch_cfg[sch].sccl[index].pilots[scclp].code_chan_sch =
            uhdm->pilot[uhdmp].chan_rec.sch_chan_rec[bn].code_chan_sch;
          ccl.for_sch_cfg[sch].sccl[index].pilots[scclp].qof_mask_id_sch =
            uhdm->pilot[uhdmp].chan_rec.sch_chan_rec[bn].qof_mask_id_sch;
        }
      } // for ( bn = 0...

      // It can happen that num_sch can be 0 for this pilot, if that's the case
      // the standard says that we need to delete the pilot from all SCCL entries
      if ( uhdm->pilot[uhdmp].num_sch == 0)
      {
        for ( index = 0; index < CAI_MAX_NUM_PER_SCH_CFG; ++index )
        {
          for ( sch = 0; sch < CAI_MAX_NUM_FOR_SCH; ++sch )
          {
            uint8 plt;

            for ( plt = 0; plt < ccl.for_sch_cfg[sch].sccl[index].num_pilots; ++plt )
            {
              // Found pilot in SCCL
              if ( ccl.for_sch_cfg[sch].sccl[index].pilots[plt].pilot_pn ==
                   uhdm->pilot[uhdmp].pilot_pn )
              {
                // Overwrite this pilot with last entry (can be same)
                ccl.for_sch_cfg[sch].sccl[index].pilots[plt] =
                  ccl.for_sch_cfg[sch].sccl[index].pilots[ccl.for_sch_cfg[sch].sccl[index].num_pilots-1];
                ccl.for_sch_cfg[sch].sccl[index].num_pilots--;
                break;
              }
            } // for ( plt = 0...
          } // for ( sch = 0...
        } // for ( index = 0...
      } // if ( uhdm->pilot[uhdmp].num_sch == 0)
    } // if ( uhdm->sch_incl )

  } // of for ( uhdmp = 0...

  // Step 3:
  // Remove any pilots that are not specified in the message
  for ( cclp = 0; cclp < CCL_SIZE; ++cclp )
  {
    // We can skip pilots that are already marked at not used
    if ( ccl.pilots[cclp].pilot_state != MCCCCL_ACTIVE )
    {
      continue;
    }

    for ( uhdmp = 0; uhdmp < uhdm->num_pilots; ++uhdmp )
    {
      if ( ccl.pilots[cclp].pilot_pn == uhdm->pilot[uhdmp].pilot_pn )
      {
        break;
      }
    }

    // Now if uhdmp is equals uhdm->num_pilots, that means this pilot
    // is not in the new active set, remove it
    if ( uhdmp == uhdm->num_pilots )
    {
      mark_pilot_as_invalid( cclp );
    }
  }

  // Step 4:
  // Update non-pilot specific F-SCH configuration, and R-SCH configuration
  // if given
  if ( uhdm->sch_incl )
  {
    uint8 sch, index;

    // Non-pilot specific F-SCH configuration
    for ( bn = 0; bn < uhdm->sch_cfg.num_for_sch; ++bn )
    {
      // Retrive sch ID and SCCL index for readability
      sch = uhdm->sch_cfg.for_sch_cfg[bn].for_sch_id;
      index = uhdm->sch_cfg.for_sch_cfg[bn].sccl_index;

      // Now update the rate
      ccl.for_sch_cfg[sch].sccl[index].sch_rate =
        (cai_sch_rate_type) uhdm->sch_cfg.for_sch_cfg[bn].for_sch_rate;
    }

    // R-SCH configuration
    for ( bn = 0; bn < uhdm->sch_cfg.num_rev_sch; ++bn )
    {
      // Retrive sch ID and SCCL index for ease of reading
      sch = uhdm->sch_cfg.rev_sch_cfg[bn].rev_sch_id;
      index = uhdm->sch_cfg.rev_sch_cfg[bn].rev_sch_rate;

      // Now copy Walsh_id
      ccl.rev_sch_cfg[sch].walsh_id[index] =
        uhdm->sch_cfg.rev_sch_cfg[bn].rev_walsh_id;
    }
  }

  /* Log the active set */
  mccccl_log_aset_info();

  // We are done now
  return;

} // update_ccl_with_uhdm

/*===========================================================================

FUNCTION update_ccl_with_escam

DESCRIPTION
  This function updates the CCL upon receiving an ESCAM, which updates the
  SCH (both forward and reverse) assignment and configuration.  Note that ESCAM
  cannot update SCCH.  Only information applicable to CCL is updated.  Other
  information, e.g. USE_T_ADD_ABORT, is not stored in CCL.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void update_ccl_with_escam
(
  caii_escam_type* escam
)
{
  uint8   bn, index, sch;

  // ESCAM is only used for SCH, mark CCL as such
  ccl.sup_state = MCCCCL_SCH;

  // If reverse SCH config. included, update it
  if ( escam->rev_cfg_included )
  {
    // Note: num_rev_cfg_recs + 1 occurrences
    for ( bn = 0; bn <= escam->num_rev_cfg_recs; ++bn )
    {
      // Rev rate is index to configuration
      index = escam->rev_cfg[bn].rev_sch_rate;
      sch = escam->rev_cfg[bn].rev_sch_id;

      // Now stored configuration data (just walsh id)
      ccl.rev_sch_cfg[sch].walsh_id[index] = escam->rev_cfg[bn].rev_walsh_id;
    }
  }

  // Update forward SCH config. if needed
  if ( escam->for_cfg_included )
  {
    for ( bn = 0; bn <= escam->num_for_cfg_recs; ++bn )
    {
      uint8  escamp;

      // Obtain sch ID
      sch = escam->for_cfg[bn].for_sch_id;

      // Retrive index for readability
      index = escam->for_cfg[bn].sccl_index;

      // Update rate for this SCCL entry
      ccl.for_sch_cfg[sch].sccl[index].sch_rate =
        (cai_sch_rate_type) escam->for_cfg[bn].for_sch_rate;

      // Process configuration for each pilot specified
      // Note: num_sup_sho + 1 pilots
      for ( escamp = 0; escamp <= escam->for_cfg[bn].num_sup_sho; ++escamp )
      {
        ccl.for_sch_cfg[sch].sccl[index].pilots[escamp].pilot_pn =
          escam->for_cfg[bn].sup_sho[escamp].pilot_pn;
        ccl.for_sch_cfg[sch].sccl[index].pilots[escamp].code_chan_sch =
          escam->for_cfg[bn].sup_sho[escamp].for_sch_cc_index;
        ccl.for_sch_cfg[sch].sccl[index].pilots[escamp].qof_mask_id_sch =
          escam->for_cfg[bn].sup_sho[escamp].qof_mask_id_sch;
        if (escam->for_cfg[bn].sup_sho[escamp].add_pilot_rec_incl)
        {
           ccl.for_sch_cfg[sch].sccl[index].pilots[escamp].pilot_rec_type =
             (cai_pilot_rec_type) escam->for_cfg[bn].sup_sho[escamp].active_pilot_rec_type;
           ccl.for_sch_cfg[sch].sccl[index].pilots[escamp].pilot_rec =
             escam->for_cfg[bn].sup_sho[escamp].add_pilot_rec;
        }
        else
        {
          ccl.for_sch_cfg[sch].sccl[index].pilots[escamp].pilot_rec_type =
             CAI_PILOT_1X_COMMON;
        }
      } // for ( escamp = 0...
      // Now update number of pilots for this SCCL entry.
      ccl.for_sch_cfg[sch].sccl[index].num_pilots = escamp;

      /* Update modem statistics */
      cdma_stats.channel_config_info.fsch0_num_pilots = ccl.for_sch_cfg[0].sccl[index].num_pilots;
      cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

    } // for ( bn = 0...
  }

  return;

} // update_ccl_with_escam

/*===========================================================================

FUNCTION UPDATE_PILOTS

DESCRIPTION
  This function updates the pilots in CCL according to IS95B.  The supplemental
  channels for pilots that are not deleted are not affected.  This function is used
  by EHDM or HDM.  Note that this function is used to support old non-IS95B builds.
  Note used for IS2000 at all.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void update_pilots
(
  byte band_class,
  word cdma_channel,
  byte num_pilots,
  mccdma_pilot_info_type pilot_array[]
)
{
  byte cclp, newp, bn;  // bn is a general purpose byte
  boolean pilot_done[CAI_N6M], replace_all = FALSE;

  // Note: maximum number of pilots is limited by CAI_N6M but not
  // CAI_MAX_SUP_PILOTS, although they are defined to be the same value.
  ONEX_ASSERT( num_pilots <= CAI_N6M );

  // Just in case ASSERT didn't catch all problem during development, fix
  // num_pilots here.
  if ( num_pilots > CAI_N6M )
  {
    num_pilots = CAI_N6M;
  }

  // Initialize working scrachpad stuff
  for ( bn = 0; bn < num_pilots; ++bn )
  {
    pilot_done[bn] = FALSE;
  }

  // See if band class or CDMA channel are different.  If so, all pilots are
  // different regardless of the PN offset.
  if ( band_class != ccl.band_class || cdma_channel != ccl.cdma_channel )
  {
    replace_all = TRUE;
  }

  // Now update band class and CDMA channel number
  ccl.band_class = band_class;
  ccl.cdma_channel = cdma_channel;

  // First of all, clean out any pilots in CCL not in pilot_array, processing
  // any pilots in CCL that are also in pilot_array.  Note: if frequency changed
  // none of the pilots in CCL are in pilot_array.
  for ( cclp = 0; cclp < CCL_SIZE; ++cclp )
  {
    if ( replace_all )  // Frequency changed, replace all pilots
    {
      mark_pilot_as_invalid( cclp );
    }
    else
    {
      // Do it for each pilot that is not marked as MCCCCL_INVALID
      if ( ccl.pilots[cclp].pilot_state != MCCCCL_INVALID )
      {
        for ( newp = 0; newp < num_pilots; ++newp )
        {
          // Found a pilot that matches one in pilot_array
          if ( ccl.pilots[cclp].pilot_pn == pilot_array[newp].pilot_pn )
          {
            // Update fundamental code channel, power combining indicator, and order
            ccl.pilots[cclp].order          = newp;
            ccl.pilots[cclp].fch_code_chan.code_chan = pilot_array[newp].code_chan_fch;
            ccl.pilots[cclp].fch_code_chan.qof_mask_id = pilot_array[newp].qof_mask_id_fch;
            ccl.pilots[cclp].pwr_comb_ind   = pilot_array[newp].pwr_comb_ind;
            ccl.pilots[cclp].srch_offset_nghbr = 0;
            // Mark it as MCCCCL_ACTIVE
            ccl.pilots[cclp].pilot_state = MCCCCL_ACTIVE;
            pilot_done[newp] = TRUE;
            break;
          }
        } // for ( newp = 0...
        // If pilot is not found in pilot_array, the above loop will exit with
        // newp = num_pilots
        if ( newp == num_pilots )
        {
          // This pilot (cclp) is not in pilot_array, need to delete
          mark_pilot_as_invalid( cclp );
        }
      }  // of if ( ccl.pilots[cclp].pilot_state != MCCCCL_INVALID )
    }  // of else replace_all
  }  // for ( cclp = 0; cclp < CCL_SIZE; ++cclp )

  // Now, time to process any pilot in pilot_array that's not processed yet
  // Those are pilots that are not in CCL, i.e. new pilots
  for ( bn = newp = 0; newp < num_pilots; ++newp )
  {
    // This pilot is not processed...
    //lint -e771 : pilot_done conceivably not initialized, which is not the case
    if ( !pilot_done[newp] )
    //lint +e771
    {
      // ... Go ahead and find a slot for it in CCL
      // bn is the beginning point to start searching
      for ( cclp = bn; cclp < CCL_SIZE; ++cclp )
      {
        // This slot is empty
        if ( ccl.pilots[cclp].pilot_state == MCCCCL_INVALID )
        {
          // Update new pilot info.
          ccl.pilots[cclp].pilot_state    = MCCCCL_ACTIVE;
          ccl.pilots[cclp].order          = newp;
          ccl.pilots[cclp].fch_code_chan.code_chan = pilot_array[newp].code_chan_fch;
          ccl.pilots[cclp].fch_code_chan.qof_mask_id = pilot_array[newp].qof_mask_id_fch;
          if (pilot_array[newp].add_pilot_rec_incl)
          {
            ccl.pilots[cclp].pilot_rec_type = (cai_pilot_rec_type) pilot_array[newp].pilot_rec_type;
            ccl.pilots[cclp].pilot_rec= pilot_array[newp].add_pilot_rec;
          }
          else
          {
            ccl.pilots[cclp].pilot_rec_type = CAI_PILOT_1X_COMMON;
          }
          ccl.pilots[cclp].pilot_pn       = pilot_array[newp].pilot_pn;
          ccl.pilots[cclp].pwr_comb_ind   = pilot_array[newp].pwr_comb_ind;
          ccl.pilots[cclp].srch_offset_nghbr = 0;
          ccl.pilots[cclp].sup_info.scch.num_sup_chan   = 0;

          // Move starting point of empty slot search up
          bn = cclp + 1;

          break;
        }
      }
      // If we couldn't find a slot for the new pilot, we can stop and exit and
      // and return.  It shouldn't have happened though.
      if ( cclp == CCL_SIZE )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "CCL out of entries: Pilot %d",
          pilot_array[newp].pilot_pn );
        return;
      }
    }
  }

} // update_pilots

/*===========================================================================

FUNCTION update_all

DESCRIPTION
  This function is used by ?HDM to update the complete CCL (not SCH) or just the
  pilots in CCL (in this case it it similar to update_pilots).  This function
  does not include SCH assignment.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void update_all
(
  byte band_class,
  word cdma_channel,
  byte num_pilots,                  /* Total number of pilots */
  mccccl_update_mode_type mode,     /* update all or just the pilots */
  byte num_sup,                     /* Number of supplementals for pilots carrying sups */
  caii_ho_pilot_type pilot_info[],  /* Pilot PNs, code channels, etc. */
  byte ch_ind
)
{
  byte  bn;  // bn is a general purpose byte
  mccdma_pilot_info_type pilot_array[CAI_N6M];

  /* This function is called for HDM, EHDM and GHDM, none of which carry ext_ch_ind. */
  set_chan_mask((cai_chind_type) ch_ind, CAI_EXT_CHIND_UNSET);

  // Some error checking
  ONEX_ASSERT( num_pilots <= CAI_N6M );

  // Just in case ASSERT didn't catch all problem during development, fix
  // num_pilots here.
  if ( num_pilots > CAI_N6M )
  {
    num_pilots = CAI_N6M;
  }

  M1X_MSG( DCP, LEGACY_MED, 
           "CCL - Update all, no of pilots = %d, mode = %d",
            num_pilots, mode );
  
  if ( mode == MCCCCL_UPDATE_PILOTS )
  {
    // If supplementals are not to be updated, then this function is the same as
    // update_pilots except that the parameters are different.  So let's do the
    // re-packaging here and call update_pilots.
    for ( bn = 0; bn < num_pilots; ++bn )
    {
      pilot_array[bn].pilot_pn = pilot_info[bn].pilot_pn;
      pilot_array[bn].pwr_comb_ind = pilot_info[bn].pwr_comb_ind;
      pilot_array[bn].code_chan_fch = pilot_info[bn].code_chan_fch;
      pilot_array[bn].qof_mask_id_fch = pilot_info[bn].qof_mask_id_fch;
      pilot_array[bn].add_pilot_rec_incl = pilot_info[bn].add_pilot_rec_incl;
      if (pilot_array[bn].add_pilot_rec_incl)
      {
        pilot_array[bn].pilot_rec_type = pilot_info[bn].pilot_rec_type;
        pilot_array[bn].add_pilot_rec = pilot_info[bn].add_pilot_rec;
      }
    }
    update_pilots( band_class, cdma_channel, num_pilots, pilot_array );
  }
  else
  {
    // If not IS95B or special MDR, we'll never be updating supplementals.
    // If we are asked to do so, it's an error.
    // Some error checking
    ONEX_ASSERT ( num_sup <= CAI_MAX_NUM_SUP );

    // Just in case ASSERT didn't catch all problem during development, fix
    // num_sup here.
    if ( num_sup > CAI_MAX_NUM_SUP )
    {
      num_sup = CAI_MAX_NUM_SUP;
    }

    // Here, just update the whole CCL with info. in pilot_info
    // First the band class and channel
    ccl.band_class = band_class;
    ccl.cdma_channel = cdma_channel;

    // GHDM can only change SCCH assignment (not SCH), so mark as such
    ccl.sup_state = MCCCCL_SCCH;

    // Now the information for each pilot
    for ( bn = 0; bn < num_pilots; ++bn )
    {
      ccl.pilots[bn].pilot_state      = MCCCCL_ACTIVE;
      ccl.pilots[bn].order            = bn;
      ccl.pilots[bn].pilot_pn         = pilot_info[bn].pilot_pn;
      ccl.pilots[bn].fch_code_chan.code_chan   = pilot_info[bn].code_chan_fch;
      ccl.pilots[bn].fch_code_chan.qof_mask_id   = 0;
      if (pilot_info[bn].add_pilot_rec_incl)
      {
        ccl.pilots[bn].pilot_rec_type = (cai_pilot_rec_type) pilot_info[bn].pilot_rec_type;
        ccl.pilots[bn].pilot_rec = pilot_info[bn].add_pilot_rec;
      }
      else
      {
        ccl.pilots[bn].pilot_rec_type = CAI_PILOT_1X_COMMON;
      }
      ccl.pilots[bn].pwr_comb_ind     = pilot_info[bn].pwr_comb_ind;
      ccl.pilots[bn].srch_offset_nghbr = 0;

      // No supplementals for this pilot
      ccl.pilots[bn].sup_info.scch.num_sup_chan = 0;

    }  // of for ( bn = 0; bn < num_pilots; ++bn )
    // bn is now = num_pilots.  Clear out the rest of the entries in CCL.
    for (; bn < CCL_SIZE; ++bn )
    {
      mark_pilot_as_invalid( bn );
    }

    /* Log active set */
    mccccl_log_aset_info();
  }

} // update_all

/*===========================================================================

FUNCTION GET_PILOTS

DESCRIPTION
  This function reads the code channel list and returns all the active pilots.
  Note that the pilot_array that is passed in is assumed to be big enough
  (CAI_N6M) to store all the returned pilots; otherwise, memory scribbling
  will occur

DEPENDENCIES
  NONE

RETURN VALUE
  The number of active pilots currently in the ccl.

SIDE EFFECTS
  None.

===========================================================================*/
static byte get_pilots
(
  mccdma_pilot_info_type pilot_array[]   // Pilot PNs and code channels
)
{
  byte i, num_pilots;

  for ( i = num_pilots = 0; i < CCL_SIZE; i++ )
  {
    if ( ccl.pilots[i].pilot_state == MCCCCL_ACTIVE )
    {
      // Make sure order of pilots are maintained.  We are sure that "order" is less than or equal
      // to CAI_N6M
      pilot_array[ccl.pilots[i].order].pilot_pn = ccl.pilots[i].pilot_pn;
      pilot_array[ccl.pilots[i].order].pwr_comb_ind = ccl.pilots[i].pwr_comb_ind;
      if (ccl.pilots[i].pilot_rec_type == CAI_PILOT_1X_COMMON)
      {
         pilot_array[ccl.pilots[i].order].add_pilot_rec_incl = FALSE;
      }
      else
      {
        pilot_array[ccl.pilots[i].order].add_pilot_rec_incl = TRUE;
        pilot_array[ccl.pilots[i].order].pilot_rec_type = (cai_pilot_rec_type) ccl.pilots[i].pilot_rec_type; /*lint !e641 */
        pilot_array[ccl.pilots[i].order].add_pilot_rec = ccl.pilots[i].pilot_rec;
      }
      pilot_array[ccl.pilots[i].order].code_chan_fch = (uint8)ccl.pilots[i].fch_code_chan.code_chan;

      num_pilots++;
    }
  }
  M1X_MSG( DCP, LEGACY_MED,
           "Getting active pilots form CCL, no. of pilots = %d",
            num_pilots );
  return( num_pilots );

}

/*===========================================================================

FUNCTION generate_aset

DESCRIPTION

  This function reads the code channel list and generates the active
  set information which can be sent to SRCH. For SCCH, this fucntion
  should be called only if the entire active set needs to be updated.
  If only supplemental channel information is needed use generate_aset_sups.
  For SCH, this function is always called to generate aset.


DEPENDENCIES
  None.

RETURN VALUE
  The number of active pilots in the code channel list. (Will be zero if
  no pilots are active).

SIDE EFFECTS
  Once this function is called, the SCCH status (sups_active) will be updated.

===========================================================================*/
static byte generate_aset
(
  srch_tc_aset_type* aset,
    /* Pointer to the active set structure that needs to be
    updated by this function */
  mccccl_sup_state_type* sup_state, // Used to indicate whether SCH or SCCH info returned
  boolean use_sups
    /* Used to indicate TRUE if supplemental channels need to be filled into
       the structure, applicable for SCCH only */
)
{
  byte pilot_order[CAI_N6M] = {0, 0, 0, 0, 0, 0}; /* To store the order of pilots in aset */
  byte num_pilots;           /* To store the number of pilots       */
  byte count1;               /* loop counter                        */
  byte this_entry;           /* Points to entry in pilot array to use now */

  if (sup_state)
  {
    *sup_state = ccl.sup_state;
  }

  // First find out total pilot in CCL and order them so that we pack them
  // in the right order in aset.  It is important because the pwr_comb_ind
  // bit requires the pilots in the right order.
  for ( count1 = num_pilots = 0; count1 < CCL_SIZE; count1++ )
  {
    if ( ccl.pilots[count1].pilot_state == MCCCCL_ACTIVE )
    {
      ++num_pilots;
      // This puts the pilot entries in the order we want
      pilot_order[ccl.pilots[count1].order] = count1;
    }
  }

  // Now, it's time to fill in the information into aset
  for ( count1 = 0; (count1 < num_pilots) &&
        (count1 < CAI_N6M); count1++ )
  {
    // Get the entry to use from pilot_order.  We know that the pilot is already
    // active because pilot_order contains only active pilots
    this_entry = pilot_order[count1];

    /* Now copy this pilot's information to the given pointer */
    aset[count1].pilot         = (int2)ccl.pilots[this_entry].pilot_pn; /* Pilot's PN*/

    aset[count1].pilot_rec_type = CAI_PILOT_1X_COMMON;
#ifdef FEATURE_IS2000_REL_A
    aset[count1].pilot_rec_type = ccl.pilots[this_entry].pilot_rec_type;
    aset[count1].pilot_rec      = ccl.pilots[this_entry].pilot_rec;
#endif

    aset[count1].srch_offset_nghbr = ccl.pilots[this_entry].srch_offset_nghbr;

    if ( ccl.fwd_chan_mask & MCC_F_DCCH )  /* DCCH included? */
    {
      aset[count1].dcch_info.active      = TRUE;
      aset[count1].dcch_info.walsh_chn   = ccl.pilots[this_entry].dcch_code_chan.code_chan;
      aset[count1].dcch_info.qof_mask_id = ccl.pilots[this_entry].dcch_code_chan.qof_mask_id;
    }
    else
    {
      aset[count1].dcch_info.active = FALSE;
    }

    if ( ccl.fwd_chan_mask & MCC_F_FCH )  /* FCH included? */
    {
      aset[count1].fch_info.active      = TRUE;
      aset[count1].fch_info.walsh_chn   = ccl.pilots[this_entry].fch_code_chan.code_chan;
      aset[count1].fch_info.qof_mask_id = ccl.pilots[this_entry].fch_code_chan.qof_mask_id;
    }
    else
    {
      aset[count1].fch_info.active = FALSE;
    }

    aset[count1].pwr_comb_ind  = ccl.pilots[this_entry].pwr_comb_ind;

    /* Check if supplemental channel information is requested
    by the calling function. */
    if ( ccl.sup_state == MCCCCL_SCCH )
    {
      uint8 sch;

      // SCCH is active, so SCH must not be
      for ( sch = 0; sch < CAI_MAX_NUM_FOR_SCH; ++sch )
      {
        aset[count1].sch_info[sch].active = FALSE;
      }
    }
    else // This must be SCH
    {
      uint8 sch;

      for ( sch = 0; sch < CAI_MAX_NUM_FOR_SCH; ++sch )
      {
        if ( ccl.pilots[this_entry].sup_info.for_sch[sch].active )
        {
          // SCH is marked as active for this pilot, include that in aset
          aset[count1].sch_info[sch].active = TRUE;
          aset[count1].sch_info[sch].qof_mask_id =
            ccl.pilots[this_entry].sup_info.for_sch[sch].qof_mask_id_sch;
          aset[count1].sch_info[sch].walsh_chn =
            ccl.pilots[this_entry].sup_info.for_sch[sch].code_chan_sch;
        }
        else
        {
          // SCH is not active
          aset[count1].sch_info[sch].active = FALSE;
        }
      }
    }

  }/* end for (pilot count) */

  return( num_pilots ); /* Return the number of active pilots */

} // generate_aset

/*===========================================================================

FUNCTION generate_sch_aset

DESCRIPTION
  This function generates the active set for SCH according to information stored
  in the CCL.  The information in put in a SRCH command structure to be sent to
  SRCH.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if aset generated successfully.  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean generate_sch_aset
(
  mctrans_srch_f_sch_action_type* sch_aset
    /* Pointer to the SCH info structure that will stored the SCH aset
       generated by this function */
)
{
  uint8 ccl_plt, aset_plt, sch;

  // Some sanity check here
  if ( ccl.sup_state == MCCCCL_SCCH )
  {
    return FALSE;
  }

  // Do it for all SCH's (MSM5000 supports only 1 - SCH 0)
  sch_aset->sch[1].num_pilots = 0;
  for ( sch = 0; (sch < mccap_num_f_sch_supported()) &&
        (sch < CAI_MAX_NUM_FOR_SCH); ++sch )
  {
    for ( ccl_plt = aset_plt = 0; (ccl_plt < CCL_SIZE) &&
          (aset_plt < CAI_N6M); ++ccl_plt )
    {
      if ( ccl.pilots[ccl_plt].pilot_state == MCCCCL_ACTIVE &&
           ccl.pilots[ccl_plt].sup_info.for_sch[sch].active )
      {
        // Found pilot carrying SCH, update info.
        sch_aset->sch[sch].pilots[aset_plt].pilot_pn =
          ccl.pilots[ccl_plt].pilot_pn;
#ifdef FEATURE_IS2000_REL_A
        sch_aset->sch[sch].pilots[aset_plt].pilot_rec_type =
          ccl.pilots[ccl_plt].pilot_rec_type;
        sch_aset->sch[sch].pilots[aset_plt].pilot_rec =
          ccl.pilots[ccl_plt].pilot_rec;
#endif
        sch_aset->sch[sch].pilots[aset_plt].qof_mask_id =
          ccl.pilots[ccl_plt].sup_info.for_sch[sch].qof_mask_id_sch;
        sch_aset->sch[sch].pilots[aset_plt].walsh_chn =
          ccl.pilots[ccl_plt].sup_info.for_sch[sch].code_chan_sch;
        aset_plt++;
      }
    }
    // Update num_pilots with total aset pilots found (can be 0)
    sch_aset->sch[sch].num_pilots = aset_plt;
  }

  return TRUE;

}   // generate_sch_aset

/*===========================================================================

FUNCTION pilot_is_in_sch_aset

DESCRIPTION
  This function checks if the pilot passed into the function is in the SCH
  active set.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if aset generated successfully.  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean pilot_is_in_sch_aset
(
  uint16 pilot_pn
)
{
  uint8 ccl_plt, sch;
  boolean pilot_found = FALSE;

  // Some sanity check here
  if ( ccl.sup_state == MCCCCL_SCCH )
  {
    return FALSE;
  }

  // Do it for all SCH's (MSM5000 supports only 1 - SCH 0)
  for ( sch = 0; (sch < mccap_num_f_sch_supported()) &&
        (sch < CAI_MAX_NUM_FOR_SCH) && (!pilot_found); ++sch )
  {
    for ( ccl_plt = 0; ccl_plt < CCL_SIZE && !pilot_found; ++ccl_plt )
    {
      if ((ccl.pilots[ccl_plt].pilot_state == MCCCCL_ACTIVE) &&
          ccl.pilots[ccl_plt].sup_info.for_sch[sch].active &&
          (ccl.pilots[ccl_plt].pilot_pn == pilot_pn))
      {
        /* Found pilot carrying SCH */
        pilot_found = TRUE;
      }
    }
  }

  return (pilot_found);

}   /* pilot_is_in_sch_aset */

/*===========================================================================

FUNCTION generate_aset_default_sups

DESCRIPTION
  This function is similar to generate_aset except that the parameter "use_sups"
  is defaulted to the internal stored value of sups_active in CCL.  Essentially,
  it updates the active set without affecting the processing of SCCH/SCH.
  It is used by HDM, GHDM, and UHDM.

DEPENDENCIES
  NONE

RETURN VALUE
  The number of active pilots currently in the ccl.

SIDE EFFECTS
  None.

===========================================================================*/
static byte generate_aset_default_sups
(
  srch_tc_aset_type* aset
    /* Pointer to the supplemental channel info structure
    that needs to be updated by this function */
  , mccccl_sup_state_type* sup_state
)
{
  return generate_aset( aset,
                        sup_state,
                        ccl.sups_active );

} // generate_aset_default_sups

/*===========================================================================

FUNCTION SAVE

DESCRIPTION
  This function saves the code channel list by copying the structure CCL
  into a separate structure called CCL_OLD

DEPENDENCIES
  NONE

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void save( void )
{
  ccl_old = ccl;
}

/*===========================================================================

FUNCTION RESTORE

DESCRIPTION
  This function restores the code channel list by copying CCL_OLD into CCL

DEPENDENCIES
  NONE

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void restore( void )
{
  ccl = ccl_old;
}

/*===========================================================================

FUNCTION activate_f_sch_assign

DESCRIPTION
  This function reads from the f-sch pending assignment in CCL.  If it finds
  any assignment that's pending, it'll make it active by updating the f-sch
  current assignment in the ccl.pilot array.  It'll also reset the pending
  flag.  This is used to both start and stop f-sch.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void activate_f_sch_assign
(
  uint8   sch_id,         // SCH 0 or 1
  uint8   sccl_index
)
{
  uint8 plt, cclp;

  // First make sure SCH ID is valid
  ONEX_ASSERT( sch_id < CAI_MAX_NUM_FOR_SCH );

  /*lint -save -e661 -e662 ASSERT check is there for sch_id */
  // First of all, clear out any old assignment for this SCH
  deactivate_f_sch( sch_id );

  // Loop through each pilot in sch_assign and find a matching pilot in CCL.  Then do
  // the assignment update.
  for ( plt = 0; plt < ccl.for_sch_cfg[sch_id].sccl[sccl_index].num_pilots; ++plt )
  {
    for ( cclp = 0; cclp < CCL_SIZE; ++cclp )
    {
      if ( ccl.pilots[cclp].pilot_state != MCCCCL_ACTIVE )
      {
        continue;
      }
      if ( ccl.pilots[cclp].pilot_pn == ccl.for_sch_cfg[sch_id].sccl[sccl_index].pilots[plt].pilot_pn )
      {
        // Found matching pilot pn, update current assignment for this pilot
        ccl.pilots[cclp].sup_info.for_sch[sch_id].active = TRUE;
        ccl.pilots[cclp].sup_info.for_sch[sch_id].code_chan_sch =
          ccl.for_sch_cfg[sch_id].sccl[sccl_index].pilots[plt].code_chan_sch;
        ccl.pilots[cclp].sup_info.for_sch[sch_id].qof_mask_id_sch =
          ccl.for_sch_cfg[sch_id].sccl[sccl_index].pilots[plt].qof_mask_id_sch;
        ccl.pilots[cclp].sup_info.for_sch[sch_id].sch_rate =
          ccl.for_sch_cfg[sch_id].sccl[sccl_index].sch_rate;
        break;  // Done for this pilot pn
      } // if ( ccl.pilots[cclp].pilot_pn...
    } // for ( cclp = 0...
    // If we couldn't find a matching pilot in CCL, just skip it.  This should have
    // happened since error error checking would have precluded that.
  } // for ( plt = 0...
  /*lint -restore */

#ifdef FEATURE_IS2000_REL_A
  /* Log the reduced active set */
  append_reduced_active_set_log();
#else
  /* Log the active set */
  mccccl_log_aset_info();
#endif // ifdef FEATURE_IS2000_REL_A
} // activate_f_sch_assign

/*===========================================================================

FUNCTION deactivate_f_sch

DESCRIPTION
  This function marks the specified F-SCH as inactive in CCL.  It is used for
  stopping F-SCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void deactivate_f_sch
(
  uint8 sch_id      // SCH 0 or 1
)
{
  uint8 cclp;

  /* First make sure SCH ID is valid */
  ONEX_ASSERT( sch_id < CAI_MAX_NUM_FOR_SCH );

  /*lint -save -e661 -e662 ASSERT check is there for sch_id */
  // Find all active pilots and then mark the specified SCH as inactive
  for ( cclp = 0; cclp < CCL_SIZE; ++cclp )
  {
    if ( ccl.pilots[cclp].pilot_state == MCCCCL_ACTIVE )
    {
      ccl.pilots[cclp].sup_info.for_sch[sch_id].active = FALSE;
    }
  } // for ( cclp = 0...
  /*lint -restore */
} // deactivate_f_sch

/*===========================================================================

FUNCTION activate_r_sch_assign

DESCRIPTION
  This function reads from the r-sch pending assignment in CCL.  If it finds
  any assignment that's pending, it'll make it active by updating the r-sch
  current assignment in CCL.  It'll also reset the pending flag.  This is used
  to both start and stop r-sch.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void activate_r_sch_assign
(
  uint8             sch_id,          // SCH 0 or 1
  cai_sch_rate_type sch_rate
)
{
  // First make sure SCH ID and SCH rate are valid
  ONEX_ASSERT( (sch_id < CAI_MAX_NUM_FOR_SCH) &&
          ((int)sch_rate < CAI_MAX_NUM_PER_SCH_CFG) );

  /*lint -save -e661 -e662 ASSERT check is there for sch_id */
  // Now R-SCH update assignment in CCL
  ccl.rev_sch[sch_id].rate = sch_rate;
  ccl.rev_sch[sch_id].walsh_id = ccl.rev_sch_cfg[sch_id].walsh_id[sch_rate];
  ccl.rev_sch[sch_id].active = TRUE;
  /*lint -restore */

#ifdef FEATURE_IS2000_REL_A
  /* Log the reduced active set */
  append_reduced_active_set_log();
#else
  /* Log the active set */
  mccccl_log_aset_info();
#endif // ifdef FEATURE_IS2000_REL_A
} // activate_r_sch_assign

/*===========================================================================

FUNCTION deactivate_r_sch

DESCRIPTION
  This function marks the specified R-SCH as inactive in CCL.  It is used for
  stopping R-SCH.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void deactivate_r_sch
(
  uint8 sch_id    // SCH 0 or 1
)
{
  // First make sure SCH ID is valid
  ONEX_ASSERT( sch_id < CAI_MAX_NUM_FOR_SCH );

  /*lint -save -e661 -e662 ASSERT check is there for sch_id */
  ccl.rev_sch[sch_id].active = FALSE;
  /*lint -restore */
} // deactivate_r_sch

/*===========================================================================

FUNCTION get_scch_entry

DESCRIPTION
  This function returns a point a a SCCL entry stored in the CCL.

DEPENDENCIES
  None.

RETURN VALUE
  0 if error, pointer to SCCL entry otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static const mccccl_sccl_entry_type* get_sccl_entry
(
  uint8 sch_id,   // SCH 0 or 1
  uint8 index
)
{
  // First make sure SCH ID/index is valid
  ONEX_ASSERT( sch_id < CAI_MAX_NUM_FOR_SCH && index < CAI_MAX_NUM_PER_SCH_CFG );

  /*lint -save -e661 -e662 ASSERT check is there for sch_id and index */
  return &ccl.for_sch_cfg[sch_id].sccl[index];
  /*lint -restore */
} // get_scch_entry

/*===========================================================================

FUNCTION get_r_sch_walsh

DESCRIPTION
  This function returns the r-sch walsh ID for the specified rate.

DEPENDENCIES
  None.

RETURN VALUE
  Walsh ID (0 or 1).

SIDE EFFECTS
  None.

===========================================================================*/
static uint8 get_r_sch_walsh
(
  uint8 sch_id,   // SCH 0 or 1
  uint8 rate
)
{
  // First some checking for programming error
  ONEX_ASSERT( sch_id < CAI_MAX_NUM_REV_SCH && rate < CAI_MAX_NUM_PER_SCH_CFG );

  /*lint -save -e661 -e662 ASSERT check is there for sch_id and index */
  return ccl.rev_sch_cfg[sch_id].walsh_id[rate];
  /*lint -restore */
} // get_r_sch_walsh


/*===========================================================================

FUNCTION pilot_is_in_active_set

DESCRIPTION

  This function checks if the pilot passed into the function is in the active
  active.

DEPENDENCIES

  None.

RETURN VALUE

  FALSE if pilot is not in active set, TRUE otherwise.

SIDE EFFECTS

  None.

===========================================================================*/
static boolean pilot_is_in_active_set
(
  uint16 pilot_pn,
  cai_pilot_rec_type pilot_rec_type,
  boolean match_pilot_rec_type
)
{
  int cclp;

  // Loops through each member in CCL
  for ( cclp = 0; cclp < CCL_SIZE; ++cclp )
  {
    // We don't need to look at non-active entries,
    // so just continue in those cases
    if ( ccl.pilots[cclp].pilot_state != MCCCCL_ACTIVE )
    {
      continue;
    }

    // The entry must be in the active set if we are here
    if ( ccl.pilots[cclp].pilot_pn == pilot_pn )
    {
      if (!match_pilot_rec_type)
      {
        // Found it, we can return
        return TRUE;
      }
      else
      {
        if (ccl.pilots[cclp].pilot_rec_type == pilot_rec_type)
        {
          // Found it, we can return
          return TRUE;
        }
        else
        {
          // pilot_rec_type is not matching
          return FALSE;
        }
      }
    }
  } // of for ( cclp = 0; ...

  // If we are here, pilot is not found
  return FALSE;

} // pilot_is_in_active_set

/*===========================================================================

FUNCTION mark_pilot_as_invalid

DESCRIPTION
  Marks the specified pilot entry in ccl as invalid.  Also clears appropriate
  status in the entry.  This function is only used locally.

DEPENDENCIES
  The ccl pilot index supplied is assumed to be valid.  The caller is responsible
  for making sure that's the case.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mark_pilot_as_invalid
(
  uint8 cclp
)
{
  uint8 sch;
  
  if( ccl.pilots[cclp].pilot_pn != 0 || 
      ccl.pilots[cclp].pilot_state != MCCCCL_INVALID)
  {
    M1X_MSG( DCP, LEGACY_MED, 
             "CCL - Mark pilot as invalid, index = %d, state = %d, pn = %d", 
              cclp, ccl.pilots[cclp].pilot_state, ccl.pilots[cclp].pilot_pn );
  }
  // Mark pilot state as invalid
  ccl.pilots[cclp].pilot_state = MCCCCL_INVALID;

  // And clear out and SCCH or SCH assignment.
  ccl.pilots[cclp].sup_info.scch.num_sup_chan = 0;

  // And clear out F-SCH current assignment
  for ( sch = 0; sch < CAI_MAX_NUM_FOR_SCH; ++sch )
  {
    ccl.pilots[cclp].sup_info.for_sch[sch].active = FALSE;
  }

} /* mark_pilot_as_invalid */

/*===========================================================================

FUNCTION get_aset_info

DESCRIPTION
  Returns current info on aset.

DEPENDENCIES
  Only valid on traffic channel.  Value is undefined otherwise.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/
static byte get_aset_info
(
  mclog_active_set_type *aset,
  byte *fsch_rate,
  mclog_aset_rsch_type *rsch
)
{
  int i,j=0;

  for (i=0; i<CCL_SIZE; i++)
  {
    if (ccl.pilots[i].pilot_state == MCCCCL_ACTIVE)
    {
      aset[j].pilot_pn = ccl.pilots[i].pilot_pn;
      aset[j].pilot_rec_type = ccl.pilots[i].pilot_rec_type; /*lint !e641 */

      /* fill in the FCH info */
      if (ccl.fwd_chan_mask & MCC_F_FCH)
      {
        aset[j].fch.enable= TRUE;
        aset[j].fch.code_chan = ccl.pilots[i].fch_code_chan.code_chan;
        aset[j].fch.qof_mask_id = ccl.pilots[i].fch_code_chan.qof_mask_id;
      }
      else
      {
        aset[j].fch.enable= FALSE;
      }

      /* fill in the DCCH info */
      if (ccl.fwd_chan_mask & MCC_F_DCCH)
      {
        aset[j].dcch.enable= TRUE;
        aset[j].dcch.code_chan = ccl.pilots[i].dcch_code_chan.code_chan;
        aset[j].dcch.qof_mask_id = ccl.pilots[i].dcch_code_chan.qof_mask_id;
      }
      else
      {
        aset[j].dcch.enable= FALSE;
      }

      if (ccl.pilots[i].sup_info.for_sch[0].active)
      {
        aset[j].sch.enable = TRUE;
        aset[j].sch.code_chan = ccl.pilots[i].sup_info.for_sch[0].code_chan_sch;
        aset[j].sch.qof_mask_id = ccl.pilots[i].sup_info.for_sch[0].qof_mask_id_sch;
        /* all fsch_rate should be the same */
        *fsch_rate = ccl.pilots[i].sup_info.for_sch[0].sch_rate; /*lint !e641 */
      }
      else
      {
        aset[j].sch.enable = FALSE;
      }
      j++;
    }
  }

  if (ccl.rev_sch[0].active)
  {
    rsch->active = TRUE;
    rsch->walsh_id = ccl.rev_sch[0].walsh_id;
    rsch->rate = ccl.rev_sch[0].rate; /*lint !e641 */
  }
  else
  {
    rsch->active = FALSE;
  }

  return j;  /*lint !e734 there is no loss of precision */

}

/*===========================================================================

FUNCTION get_control_hold_info

DESCRIPTION
  Returns current info on control_hold.

DEPENDENCIES
  Only valid on traffic channel.  Value is undefined otherwise.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/
static void get_control_hold_info(mclog_control_hold_type *ch)
{
  /* ptr to retrieve the non negotiable srv config records */
  const caii_non_neg_srv_cfg_type *nnscr;

  if (in_control_hold())
  {
    // we are in control hold mode
    nnscr = snm_get_current_nnscr();

    ch->enable = TRUE;
    if (nnscr->gating_rate_incl)
    {
      ch->gating_rate = nnscr->pilot_gate_rate;
    }
    else
    {
      ch->gating_rate = CAI_GATING_RATE_NONE; /*lint !e641 */
    }
  }
  else
  {
    ch->enable = FALSE;
  }

} // get_control_hold_info(mclog_control_hold_type *ch)


/*===========================================================================

FUNCTION fwd_fundicated_chan_is_active

DESCRIPTION
  Check if there is an active forward-fundicated channel (i.e.
  F-FCH or F-DCCH).

DEPENDENCIES
  MCCCCL already initialized with OTA CH_IND and EXT_CH_IND.

RETURN VALUE
  Boolean - TRUE if F-FCH and/or F-DCCH is active, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean fwd_fundicated_chan_is_active()
{
  return ((ccl.fwd_chan_mask & (MCC_F_FCH|MCC_F_DCCH)) != 0); /*lint !e641 */

}

/*===========================================================================

FUNCTION set_chan_mask

DESCRIPTION
  Save the value of the CH_IND and EXT_CH_IND and store the internal
  forward and reverse channel masks based on OTA values received during
  TCI/ECAM and Handoff processing.

  In ECAM, for a DV assignment, EXT_CH_IND is set and no CH_IND is
  present in the message.

  In UHDM, for a DV assignment, EXT_CH_IND is set and CH_IND is '000'.

  No validity checking is done in this function.

DEPENDENCIES
  Parameters with OTA CH_IND and EXT_CH_IND are already validated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void set_chan_mask
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
  // If either CH_IND or EXT_CH_IND changes update information
  if ((ccl.ota_chind != ota_chind) || (ccl.ota_extchind != ota_extchind))
  {
    // Store the separate fwd/rev channel masks
    ccl.fwd_chan_mask = gen_fwd_chan_mask(ota_chind, ota_extchind);
    ccl.rev_chan_mask = gen_rev_chan_mask(ota_chind, ota_extchind);

    // Store the OTA values
    ccl.ota_chind = ota_chind;
    ccl.ota_extchind = ota_extchind;

    report_channel_change(ota_chind, ota_extchind);
  }
}


/*===========================================================================

FUNCTION report_channel_change

DESCRIPTION
  Generate the event report of channel indicator changes and notify
  DS of changes in control hold status.

DEPENDENCIES
  Internal MCCCCL forward and reverse channel masks are up to date.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void report_channel_change
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "OTA: chind: 0x%x  extchind: 0x%x",
    ota_chind,
    ota_extchind);
  M1X_MSG( DCP, LEGACY_MED,
    "MASK: fwd: 0x%x  rev: 0x%x ",
    ccl.fwd_chan_mask,
    ccl.rev_chan_mask);

  mclog_report_event_ch_ind(ota_chind, ota_extchind);

  // Only notify DS if call back function is registered
  if (mccds_notify_chs)
  {
    if (ccl.rev_chan_mask & MCC_R_PICH)
    {
      // if the pilot bit is on, then MS is in active state
      (*mccds_notify_chs)(FALSE);
    }
    else
    {
      // MS is in control hold
      (*mccds_notify_chs)(TRUE);
    }
  }
}

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION  set_fch_to_be_restored

DESCRIPTION
  If value is TRUE, Marks flag indicating there is stored info in
  CCL for FCH which
  is to be restored upon transition to active. This is set when we get an
  ERM with ch_ind=101
  If value is FALSE, Marks flag indicating there is no FCH to be
  restored. This typically
  happens when we get any HDMs which contains new pilot info anyway.

DEPENDENCIES
  Only valid on traffic channel.  Value is undefined otherwise.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void set_fch_to_be_restored( boolean value )
{
  ccl.fch_to_be_restored = value;
}

/*===========================================================================

FUNCTION  get_fch_to_be_restored

DESCRIPTION
  Retrives the value of the flag which indicates if FCH is to be
  restored.

DEPENDENCIES
  Only valid on traffic channel.  Value undefined otherwise.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean get_fch_to_be_restored( void )
{
  return ccl.fch_to_be_restored;
}

#endif //FEATURE_IS2000_REL_A_SVD


/*===========================================================================

FUNCTION get_fwd_chan_mask

DESCRIPTION
  Returns current value of Forward Channel Mask.

DEPENDENCIES
  Only valid on traffic channel.  Value is undefined otherwise.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/
static mcc_fwd_chan_mask_type get_fwd_chan_mask
(
  void
)
{
    return ccl.fwd_chan_mask;
}
/*===========================================================================

FUNCTION get_rev_chan_mask

DESCRIPTION
  Returns current value of Reverse Channel Mask

DEPENDENCIES
  Only valid on traffic channel.  Value is undefined otherwise.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/
static mcc_rev_chan_mask_type get_rev_chan_mask
(
  void
)
{
    return ccl.rev_chan_mask;
}

/*===========================================================================

FUNCTION gen_fwd_chan_mask

DESCRIPTION
  Convert the OTA CH_IND and EXT_CH_IND fields to the internal
  forward channel mask.

  Assumption is that only one of these values is actually
  meaningful at any time. Always test EXT_CH_IND first as it will have
  a well-defined "unset" value when it is not set.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of forward channels

SIDE EFFECTS
  None.

===========================================================================*/
static mcc_fwd_chan_mask_type gen_fwd_chan_mask
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
  mcc_fwd_chan_mask_type fwd_mask = MCC_F_RESERVED;

  if (ota_extchind == CAI_EXT_CHIND_UNSET)
  {
    // Convert CH_IND to Channel Mask
    fwd_mask |= ((ota_chind & CAI_CHIND_FCH_ONLY) ? MCC_F_FCH : 0);
    fwd_mask |= ((ota_chind & CAI_CHIND_DCCH_ONLY) ? MCC_F_DCCH : 0);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No DV support, EXT_CH_IND should not be in use");
  }
  return fwd_mask;
}

/*===========================================================================

FUNCTION gen_rev_chan_mask

DESCRIPTION
  Convert the OTA CH_IND and EXT_CH_IND fields to the internal
  reverse channel mask.

  Assumption is that only one of these values is actually
  meaningful at any time. Always test EXT_CH_IND first as it will have
  a well-defined "unset" value when it is not set.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of reverse channels

SIDE EFFECTS
  None.

===========================================================================*/
static mcc_rev_chan_mask_type gen_rev_chan_mask
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
  mcc_rev_chan_mask_type rev_mask = MCC_R_RESERVED;

  if (ota_extchind == CAI_EXT_CHIND_UNSET)
  {
    // Convert CH_IND to Channel Mask
    rev_mask |= ((ota_chind & CAI_CHIND_FCH_ONLY) ? MCC_R_FCH : 0);
    rev_mask |= ((ota_chind & CAI_CHIND_DCCH_ONLY) ? MCC_R_DCCH : 0);
    rev_mask |= ((ota_chind & CAI_CHIND_REV_PILOT) ? MCC_R_PICH : 0);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No DV support, EXT_CH_IND should not be in use");
  }
  return rev_mask;
}

/*===========================================================================

FUNCTION gen_fwd_chan_mask_erm

DESCRIPTION
  Given the CH_IND and possibly EXT_CH_IND values from the
  Extended Release Message (ERM) calculate the channel mask of
  what forward channels to release.

  ERM can caus a channel configuration change for Non-DV to Non-DV, DV to DV
  and DV to Non-DV, but cannot cause a Non-DV to DV transition.

  If EXT_CH_IND is set, then CH_IND is ignored.

DEPENDENCIES
  None.

RETURN VALUE
  The channel mask of what forward channels to release.

SIDE EFFECTS
  None.

===========================================================================*/
static mcc_fwd_chan_mask_type gen_fwd_chan_mask_erm
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
  mcc_fwd_chan_mask_type rel_mask;
  mcc_fwd_chan_mask_type tmp_mask;

  // EXT_CH_IND is not set, transition to Non-DV channel configuration
  if (ota_extchind == CAI_EXT_CHIND_UNSET)
  {
    // Which of the non-DV channels to release - AND together the current
    // mask and the channels indicated in the OTA CH_IND, this handles
    // the case of asymmetric channel usage, i.e. CH_IND says release DCCH
    // but only R-DCCH is active
    tmp_mask = gen_fwd_chan_mask(ota_chind, CAI_EXT_CHIND_UNSET);
    rel_mask = ccl.fwd_chan_mask & tmp_mask;

    // On DV to Non-DV transition must also release F-PDCH
    rel_mask |= ((ccl.fwd_chan_mask & MCC_F_PDCH) ? MCC_F_PDCH : 0);

    // On DV to Non-DV transition must also release F-CPCCH if allocated
    rel_mask |= ((ccl.fwd_chan_mask & MCC_F_CPCCH) ? MCC_F_CPCCH : 0);
  }
  else  // DV to DV channel configuration change
  {
    // AND together the current mask and the complement
    // of the channels indicated by the OTA EXT_CH_IND
    tmp_mask = gen_fwd_chan_mask(ota_chind, ota_extchind);
    rel_mask = ccl.fwd_chan_mask & ~tmp_mask;
  }

  return rel_mask;
}

/*===========================================================================

FUNCTION gen_rev_chan_mask_erm

DESCRIPTION
  Given the CH_IND and possibly EXT_CH_IND values from the
  Extended Release Message (ERM) calculate the channel mask of
  what reverse channels to release.

  ERM can caus a channel configuration change for Non-DV to Non-DV, DV to DV
  and DV to Non-DV, but cannot cause a Non-DV to DV transition.

  If EXT_CH_IND is set, then CH_IND is ignored.

DEPENDENCIES
  None.

RETURN VALUE
  The channel mask of what reverse channels to release.

SIDE EFFECTS
  None.

===========================================================================*/
static mcc_rev_chan_mask_type gen_rev_chan_mask_erm
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
  mcc_rev_chan_mask_type rel_mask, tmp_mask;

  // EXT_CH_IND is not set, transition to Non-DV channel configuration
  if (ota_extchind == CAI_EXT_CHIND_UNSET)
  {
    // Which of the non-DV channels to release (including Cont. Rev. Pilot)
    // For reverse, we don't do the extra AND with the current channels
    // because for a transition to Non-DV we are never releasing the forward
    // channel and not the reverse channel.
    rel_mask = gen_rev_chan_mask(ota_chind, CAI_EXT_CHIND_UNSET);

    // Until R-PDCH is supported no DV channels in rev ch mask to consider
    // for DV to Non-DV transitions

  }
  else // DV to DV channel configuration change
  {
   // AND together the current mask and the complement
   // of the channels indicated by the new OTA EXT_CH_IND
   tmp_mask = gen_rev_chan_mask(ota_chind, ota_extchind);
   rel_mask = ccl.rev_chan_mask & ~tmp_mask;
  }

  return rel_mask;
}

/*===========================================================================

FUNCTION set_chan_mask_erm

DESCRIPTION
  Save the value of the CH_IND and EXT_CH_IND and store the internal
  forward and reverse channel masks based on OTA values received during
  ERM processing.

  No validity checking is done in this function.

DEPENDENCIES
  Caller validates the OTA CH_IND and EXT_CH_IND values.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void set_chan_mask_erm
(
  cai_chind_type ota_chind,
  cai_ext_chind_type ota_extchind
)
{
   mcc_fwd_chan_mask_type fwd_rel_mask;
   mcc_rev_chan_mask_type rev_rel_mask;

   // Calculate and store the new forward and reverse channel masks
   fwd_rel_mask = gen_fwd_chan_mask_erm(ota_chind, ota_extchind);
   rev_rel_mask = gen_rev_chan_mask_erm(ota_chind, ota_extchind);
   ccl.fwd_chan_mask &= ~fwd_rel_mask;
   ccl.rev_chan_mask &= ~rev_rel_mask;

   // Store the OTA values

  /* DV to Non-DV
     Non-DV to Non-DV
  */

  ccl.ota_chind = CAI_CHIND_RESERVED_1;

  if ( (ccl.fwd_chan_mask & MCC_F_FCH)
       || (ccl.rev_chan_mask & MCC_R_FCH)
     )
  {
    ccl.ota_chind |= CAI_CHIND_FCH_ONLY;
  }

  if ( (ccl.fwd_chan_mask & MCC_F_DCCH)
       || (ccl.rev_chan_mask & MCC_R_DCCH)
     )
  {
    ccl.ota_chind |= CAI_CHIND_DCCH_ONLY;
  }

  if (ccl.rev_chan_mask & MCC_R_PICH)
  {
    ccl.ota_chind |= CAI_CHIND_REV_PILOT;
  }

  /* DV to DV
     Non-DV to DV
  */
  ccl.ota_extchind = ota_extchind;

  report_channel_change(ota_chind, ota_extchind);
}

/*===========================================================================
FUNCTION get_ota_extchind

DESCRIPTION
  Returns the ota value of extended channel indicator

DEPENDENCIES
  None

RETURN VALUE
  ext_chind ota value.

SIDE EFFECTS
  None.

===========================================================================*/
static cai_ext_chind_type get_ota_extchind(void)
{
    return ccl.ota_extchind;
}

/*===========================================================================

FUNCTION in_control_hold

DESCRIPTION
  Returns TRUE if MS is in control hold, FALSE otherwise.

DEPENDENCIES
  Only valid on traffic channel.  Value is undefined otherwise.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean in_control_hold(void)
{
  // If there is no Continuous Reverse Pilot, the MS is in Control Hold
  if ((ccl.rev_chan_mask & MCC_R_PICH) == 0) /*lint !e641 */
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================

FUNCTION exit_control_hold

DESCRIPTION
   Update internal MCCCCL fields on exit from control hold

DEPENDENCIES
  Only valid on traffic channel.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void exit_control_hold
(
  boolean restore_fch
)
{
  if (!in_control_hold())
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Exit Control Hold?, When MS is Active");
  }

  // Update OTA CH_IND
  ccl.ota_chind |= CAI_CHIND_REV_PILOT;

  // Update internal channel mask
  ccl.rev_chan_mask |= MCC_R_PICH;

  // Revisit for DV
  if (restore_fch)
  {
    ccl.ota_chind |= CAI_CHIND_FCH_ONLY;
    ccl.fwd_chan_mask |= MCC_F_FCH;
    ccl.rev_chan_mask |= MCC_R_FCH;
  }
}
/*lint +e655 +e818 */
