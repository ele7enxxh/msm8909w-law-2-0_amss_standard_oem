#ifndef MCCCCL_H
#define MCCCCL_H
/*===========================================================================

     C O D E   C H A N N E L   L I S T   M A I N T E N A N C E

GENERAL DESCRIPTION
  This module maintains the code channel list according to IS95B section 6.6.8.

DESCRIPTION
  This module contains definitions and interfaces for maintaining the code
  channel list.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccccl.h_v   1.9   01 Oct 2002 16:06:46   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccccl.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
11/15/10   jtm     Initial SU API split for CP.
07/22/10   ag      De-coupling of RCP data from CCL object.
09/28/09   ag      1X Advanced Signaling Support - Phase 2
05/13/09   ag      CMI Phase2 changes.
03/20/09   jtm     Removed unnecessary include of mctrans.h.
03/04/09   adw     Added mccdma_v.h include.
05/18/06   fc      Changed the function prototype of initialize.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/08/05   sb      Renamed functions as per code review comments.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h.
11/05/04   sb      Added support for Rel C DV handoffs.
10/07/04   fc      Updated the maximum allowed number of values for DV group.
09/16/04   fc      Added support for reverse DV.
04/22/04   bkm     Misc updates from DV Call Setup/PCNM code review.
04/14/04   az      Added support for ERM - Rel C DV chan cfg change
01/28/04   bkm     Changes signature on initialize to take chan_assign as
                   parameter and added support for soft/softer switch.
01/05/04   bkm     Misc updates for DV F-PDCH support.
12/02/03   bkm     Split internal chind into fwd_chan_mask and rev_chan_mask.
10/23/03   va      Changed caii_ext_ch_ind3_pilot_rec_type to
                   mccdma_pilot_info_type
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
05/28/03   va      Changed the prototype of pilot_is_in_active_set to also
                   include pilot_rec_type
10/01/02   az      Mainlined FEATURE_IS2000
07/19/02   va      Added support FCH storing and restoring when going to CH
                   if necessary, added prototype for set_fch_to_be_restored()
                   and get_fch_to_be_restored()
05/07/02   fc      Added function pilot_is_in_sch_active.
10/17/01   jrp     Made mccccl_submit_reduced_active_set_log() public.
05/16/01   fc      Merged the following from common archive:
                   04/10/01   jq/lcc  Added functions to support P2 and control
                   hold.
04/09/01   va      Added a new prototype for get_pilots function if
                   FEATURE_IS2000_REL_A is defined.
03/07/01   fc      Moved function pilot_is_in_active_set outside
                   FEATURE_IS2000_SCH.
03/06/01   jq      Added aset_info logging to replace aset_config loggin.
02/27/01   va      Added pilot_rec_type in mccccl_sccl_entry_type.
08/23/00   lcc     1. For IS2000 only, changed function generate_aset_default_sups
                      to include sup_state in parameter.
                   2. Added prototype for function mark_pilot_as_invalid.
07/20/00   fc      Used enumeration type for SCH rate.
06/12/00   lcc     Corrected some featurization on FEATURE_IS2000_SCH/FEATURE_IS2000.
04/13/00   lcc     Added support for SCH (forward and reverse).
03/12/00   ry      Added IS2000 featurization
03/06/00   lcc     Added partial support for SCH management.
11/15/99   va      IS2000 Changes. Added support for QOF.
11/03/99   lcc     Merged from GD4021:
           ry      Added members get_pilots, save, and restore to
                   mccccl_code_channel_list_object_type
           ry      Moved all occurrences of update_all's out of the
                   #ifdef(FEATURE_IS95B_MDR || FEATURE_SPECIAL_MDR) block
09/20/99   lcc     Mainlined update_all.
08/03/99   lcc     Added descriptions of functions and removed function returns
                   from some functions as a result of code review.
                   Also added sup_pilots_are_valid function and some miscellaneous
                   cleanup.
06/30/99   kmp     Removed #includes of msg.h and err.h since they are not
                   needed in this header file.
05/19/99   doj     Chaged GHDM pilot type (caii_gen_ho_pilot_type) reference
                   to generic handoff type (caii_ho_pilot_type).
03/26/99   lcc     Added member get_number_of_supplementals to
                   mccccl_code_channel_list_object_type.
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
#include "caii.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "log.h"
#include "srchmc.h"
#include "mclog_v.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"


/*===========================================================================

                      SOME CONSTANTS USED IN THIS FILE

===========================================================================*/
// Reserve twice the number of active pilots so that potential pilots can be
// stored (in case of SCAM linked to GHDM).
#define CCL_SIZE  (CAI_N6M*2)

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
// State of pilot entry in CCL.
// INVALID - pilot entry does not contain a valid pilot
// SUP_ONLY - pilot entry contains only supplemental code channel (SCCH) info.
//            Not used for SCH.
// ACTIVE - pilot entry contains a valid pilot (in active set, although supplemental channels
//          may not be up yet)
typedef enum
{
  MCCCCL_INVALID,
  MCCCCL_SUP_ONLY,
  MCCCCL_ACTIVE
} mccccl_pilot_state_type;

// This update mode is used for function update_all only.  It is used to indicate
// if all of the CCL or just the pilots are to be updated.
typedef enum
{
  MCCCCL_UPDATE_PILOTS,
  MCCCCL_UPDATE_ALL
} mccccl_update_mode_type;

// Supplemental Channel (SCH or SCCH) state
typedef enum
{
  MCCCCL_SCCH,
  MCCCCL_SCH
} mccccl_sup_state_type;

// Information contained in a SCCL entry
typedef struct
{
  uint8 num_pilots;       // number of pilots in the active set of this SCH
  cai_sch_rate_type sch_rate;
  struct
  {
    uint16  pilot_pn;
    word    code_chan_sch;
    byte    qof_mask_id_sch;
    cai_pilot_rec_type pilot_rec_type;
    caii_add_pilot_rec_type pilot_rec;
  } pilots[CAI_N6M];
} mccccl_sccl_entry_type;


// Structure used by ECAM/CAM/HDM/EHDM to pass pilot information into CCL and
// to generate active set info to be passed to searcher.
// See below for description of the following "member" functions of
// mccccl_code_channel_list_object_type
typedef struct
{

  byte (*get_pilots)( mccdma_pilot_info_type pilot_array[] );
  byte (*generate_aset)( srch_tc_aset_type* aset,
                         mccccl_sup_state_type* sup_state,  // This parameter added for IS2000
                         boolean use_sups );
  byte (*generate_aset_default_sups)( srch_tc_aset_type* aset,
                                      mccccl_sup_state_type* sup_state );

  void (*initialize)( byte band_class, word cdma_channel, byte num_pilots,
                      mccdma_pilot_info_type pilot_array[],
                      mccdma_chnasn_type *msg_ptr  );
  void (*update_pilots)( byte band_class, word cdma_channel, byte num_pilots,
                         mccdma_pilot_info_type pilot_array[] );
  void (*update_all)( byte band_class, word cdma_channel,
                      byte num_pilots, mccccl_update_mode_type update_mode,
                      byte num_sup, caii_ho_pilot_type pilot_info[],
                      byte ch_ind );

  void (*save)( void );
  void (*restore)( void );

  void (*update_ccl_with_uhdm)( caii_ho_msg_type* uhdm );
  byte (*get_aset_info)(mclog_active_set_type *aset, byte *fsch_rate, mclog_aset_rsch_type *rsch);
  void (*update_ccl_with_escam)( caii_escam_type* escam );
  boolean (*generate_sch_aset)( mctrans_srch_f_sch_action_type* sch_aset );
  boolean (*pilot_is_in_sch_aset)( uint16 pilot_pn );
  void (*activate_f_sch_assign)( uint8 sch_id, uint8 sccl_index );
  void (*activate_r_sch_assign)( uint8 sch_id, cai_sch_rate_type sch_rate );
  void (*deactivate_f_sch)( uint8 sch_id );
  void (*deactivate_r_sch)( uint8 sch_id );
  const mccccl_sccl_entry_type* (*get_sccl_entry)( uint8 sch_id, uint8 index );
  uint8 (*get_r_sch_walsh)( uint8 sch_id, uint8 rate );
  boolean (*pilot_is_in_active_set)( uint16 pilot_pn,
            cai_pilot_rec_type pilot_rec_type, boolean match_pilot_rec_type );

  /* Determine if there is an active fwd fundicated channel */
  boolean (*fwd_fundicated_chan_is_active)(void);

  /* Store the channel indicator and channel masks */
  void (*set_chan_mask)(cai_chind_type ota_chind,
                        cai_ext_chind_type ota_extchind);

  /* Retrieve separate fwd and reverse channel masks */
  mcc_fwd_chan_mask_type (*get_fwd_chan_mask)(void);
  mcc_rev_chan_mask_type (*get_rev_chan_mask)(void);

  /* Convert from OTA to internal channel masks, do not store the value */
  mcc_fwd_chan_mask_type (*gen_fwd_chan_mask)(cai_chind_type ota_chind,
                                                 cai_ext_chind_type ota_extchind);
  mcc_rev_chan_mask_type (*gen_rev_chan_mask)(cai_chind_type ota_chind,
                                                 cai_ext_chind_type ota_extchind);

  /* For ERM, determine what channels to release */
  mcc_fwd_chan_mask_type (*gen_fwd_chan_mask_erm)(cai_chind_type ota_chind,
                                                     cai_ext_chind_type ota_extchind);
  mcc_rev_chan_mask_type (*gen_rev_chan_mask_erm)(cai_chind_type ota_chind,
                                                     cai_ext_chind_type ota_extchind);

  /* Update internal MCCCCL channel masks and masks based on ERM */
  void (*set_chan_mask_erm)(cai_chind_type ota_chind,
                            cai_ext_chind_type ota_extchind);

  /* Returns the ota value of extended channel indicator */
  cai_ext_chind_type (*get_ota_extchind)(void);

  boolean (*in_control_hold)(void);
  void (*exit_control_hold)(boolean restore_fch);
  void (*get_control_hold_info)(mclog_control_hold_type *ch);

#ifdef FEATURE_IS2000_REL_A_SVD
  void (*set_fch_to_be_restored)( boolean value);
  boolean (*get_fch_to_be_restored) (void);
#endif //FEATURE_IS2000_REL_A_SVD
} mccccl_code_channel_list_object_type;

extern const mccccl_code_channel_list_object_type mccccl_code_channel_list;


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

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
);


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

void mccccl_submit_reduced_active_set_log
(
  void
);

#endif // FEATURE_IS2000_REL_A
#endif // MCCCCL_H

