/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
               B R O A D C A S T   S M S   S E R V I C E S

GENERAL DESCRIPTION
  This module contains functions for the Broadcast SMS services.

EXTERNALIZED FUNCTIONS

  MCCBCSMS_INIT -
    Initialize the broadcast SMS service and its related variables.

  MCCBCSMS_PROC_SRCH_RPT -
    This function process the Searcher's report, the wakeup report. It
    updates the Broadcast SMS related variables.

  MCCBCSMS_PROC_MSG -
    This function process the System Param Message, Extended System Param
    Message, and the MCRR Param Message for the Broadcast SMS related
    parameters. It checks to see if the Broadcast SMS configuration has
    been changed. If so, it updates the broadcast slot cycle and resets
    the related broadcast schedules.

  MCCBCSMS_SCHEDULE_BCAST -
    This function schedules the Broadcast SMS by finding the next Broadcast
    wakeup slot and sending them to the Searcher through sleep command.

  MCCBCSMS_COMP_ADDR -
    This function does necessary processing when receiving a broadcast
    message. It compares the BC address and checks if this is an expected
    broadcast message corresponding to the outstanding schedules. It also
    checks if it's a duplicate. If it's unique, it then inserts the
    broadcast message into the unique bcast list so that later broadcast
    pages and messages can perform the duplicate detection agaist it.

  MCCBCSMS_UNIV_PAGE_PARTIAL_ADDR_MATCH
    This function performs the Mobile Station Broadcast partial address match
    operation for the Universal Page Message. The result of match is directly
    saved in the input partial address structure.

  MCCBCSMS_PAGE_MATCH -
    This function performs broadcast page match operation.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  mccbcsms_init() needs to be called before calling any of the other
  functions.

  Copyright (c) 2002 - 2013 Qualcomm Technologies, Inc.
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccbcsms.c_v   1.15   02 Oct 2002 22:52:28   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccbcsms.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/14   gga     1X_CP F3 redistribution
07/11/13   gga     Added event when there is a change in the broadcast index
01/03/13   nee     Replacement of memcpy() to memscpy()
09/15/12   ppr     FR#2370 - 1x memory optimization changes
05/09/12   ppr     Feature Cleanup
04/04/12   trc     Convert to m1x_time module
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
04/13/11   ag      Moved mccbcsms_is_enabled() outside FEATURE_BROADCAST_SMS.
01/20/11   ssh     Changed the variable name in partial_addr_match function.
01/20/11   ssh     Moved the partial_addr_match functionality from WMS to
                   1xCP.
12/22/10   ag      Fixed a bug in BCSMS duplicate detection. The timestamps
                   matching with the start and end of duplicate cycle
                   also need to be considered while duplicate detection.
12/10/10   ag      Lint Fixes.
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
07/19/10   ag      Replaced "PACKED <name>" with "PACK (<name>)".
07/13/10   ag      Fixes BCSMS duplicate detection
05/12/10   ssh     Cleaned up the lint issues.
02/25/10   ssh     Cleaned up the lint warnings.
10/14/09   ag      Fix to increment the BCSMS records index to index of the
                   next first BCSMS whose wakeup slot has not passed yet.
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
05/14/09   ag      CMI phase 2 changes
05/12/09   ag      Fixed the issue of missing BCSMS when MS transitions from
                   system access to idle state.
04/17/09   jtm     Added includes to fix compiler warnings.
09/10/07   yll     Fixed a problem that MS doesn't receive consecutive and
                   large broadcast messages that take 3 paging channel slots
                   to send.
05/11/07   pg      More Lint Cleanup.
03/09/07   pg      removed dependency on WMS and make sure it only depends on
                   CM for SMS.
05/30/06   fc      Fixed lint errors.
04/27/06   fc      Fixed lint errors.
04/26/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel A featurization.
04/18/06   fc      Changed mccbcsms_config_type to use
                   cdma_bcast_bcch_info_type.
02/07/06   fc      Removed duplicate function prototype.
01/06/06   fc      Cleanup on OOS changes.
07/15/05   fc      Fixed lint errors.
06/13/05   fc      Merged the following from MSMSHARED:
06/09/05   yll      Fixed the problem of not scheduling the repeat broadcast
                    slots.
05/10/05   fc      Merged the following from MSMSHARED:
04/13/05   yll      Fixed the problem of not scheduling the Enh. BCSMS after
                    releasing the call.
01/20/05   fh      Changed to check the duplication for each of the records in
                   BCSMS pages, before scheduling a wakeup for receiving it.
12/05/04   yll     Added interface change for apiOne.
11/11/04   yll     Changed the BCCH_INDEX field in GPM/UPM to BCN number,
                   based on the standard change.
10/03/04   yll     Handle the case of receiving of broadcast message before
                   initialization of the bcast_index.
08/19/04   bkm     Removed unnecessary extern function declaration.
08/18/04   yll     Clean up the MC/SRCH interface.
06/16/04   fc      Prototype change for mccidl_supervise_channel_monitoring.
04/06/04   yll     Calculate the start of the broadcast message to compare
                   against the scheduled broadcast data slot.
02/25/04   yll     After receiving the scheduled broadcast SMS, if there are
                   more broadcast messages on BCCH of same BCN, the mobile
                   should continue monitoring.
10/03/03   yll     When in fast RAHO, don't send the BC_INFO to Search.
05/30/03   yll     When bcsms_enable changes, update the bcast_index to Srch
                   by sending a parm cmd.
05/27/03   yll     Changes for simultaneous monitoring of FCCCH and 2nd BCCH.
03/14/03   yll     When PCH Broadcast Page buffer is full, remove the expired
                   entries.
02/14/03   yll     Fixed a problem in peeking the broadcast message address
                   due to a race condition between RXC and MC.
01/30/03   yll     Fixed a problem of missing broadcast scheduling on PCH.
01/09/03   yll     Changes to save the old bc schedule on PCH that is
                   previously sent to Search, so that it doesn't send
                   multiple times.
12/02/02   yll     Fixed the problem when RXC is configured to Non-pri BCCH
                   to match the msg header before CP receives the wakeup rpt.
11/26/02   ph      mcc_bc_match(), added a return for bad ovhd chan case or
                   when BCSMS not turned on in new common channels.
11/19/02   fc      Added support for FEATURE_CDSMS UPM partial broadcast
                   address match.
11/18/02   yll     Fixed a problem to reset bc pages when receiving (E)SPM.
10/28/02   yll     Moved the broadcast schedule info from the sleep command
                   to bc_info_f.
10/24/02   yll     Changed the mccbcsms_match to mccbcsms_pc_match and
                   mccbcsms_bcch_match functions.
10/02/02   yll     Linted the code.
09/23/02   yll     Fixed the problem where mobile awake for long duration
                   after receiving the Broadcast SMS message.
09/04/02   AP      Added support for FEATURE_CDSMS.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
06/27/02   fc      Changed UPM function names.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/12/02   yll     Changes for the code review comments.
05/22/02   yll     Save the enhanced broadcast page when bcast_index is not
                   initialized after power up.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed the function names.
02/10/02   yll     Create the module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "comdef.h"


#include "cai_v.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"
#include "bit.h"
#include "qw.h"
#include "m1x_time_i.h"
#include "caix.h"
#include "caix_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccbcsms.h"
#include "mc_v.h"
#include "wms_modem.h"
#include "wms.h"
#include "caii_v.h"
#include "event.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* broadcast SMS enabled? It's set by MC_BCSMS_ENABLE_F from CM */
/* It is referenced in other modules. Will be obsolete.         */
boolean mcc_bc_sms_enable;

/* Broadcast SMS unique entries, for the duplicate detection.   */
LOCAL mccbcsms_unique_rec_type      mccbcsms_unique_recs;

/* Matched (enhanced) broadcast pages, for broadcast schedules  */
LOCAL mccbcsms_matched_page_type    mccbcsms_pages;

/* The broadcast config info. Updated w/ ESPM or MCRRPM message */
LOCAL mccbcsms_config_type          mccbcsms_config;

/* Previous bc page schedule on PCH sent to Search */
LOCAL qword             mccbcsms_old_pch_bc_slot;

/* Variable indicating whether a BCSMS is in progress or not */
boolean mcc_bcsms_in_progress = FALSE;

/* The starting slot of the BCSMS in progress */
qword mcc_bcsms_start_slot = {0, 0};

/*===========================================================================

                                 FUNCTIONS

===========================================================================*/

/************************** Start Internal Functions ***********************/

/*--------------------------------------------------------------------------
  The following functions are used only for the Broadcast SMS on the
  Paging Channel.
  -------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION MCCBCSMS_CAL_BC_REC_INDEX

DESCRIPTION
  This function calculates the index that the new broadcast schedule record
  should be inserted in the sorted list. It go through the existing list and
  compare the bc_wake_slot against the list entries to found where to insert.

DEPENDENCIES
  None.

RETURN VALUE
  The index where the entry is to be inserted.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e818*/
byte mccbcsms_cal_bc_rec_index
(
  bc_page_type * bc_rec,
  qword bc_wakeup_slot      /* In 80ms unit */
)
{
  /* the index where the entry is to be inserted */
  byte t_index = mccbcsms_pages.pages.bc.tot_bc_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if the list is empty then append */
  while ( t_index != 0 )
  {
    /* if wakeup_slot < bc_wakeup_slot */
    if ( qw_cmp(bc_rec[t_index-1].wakeup_slot,bc_wakeup_slot)<0 )
    {
      break;
    }
    else
    {
      bc_rec[t_index] = bc_rec[t_index-1];
      t_index--;
    }
  }

  return (t_index);

} /* mccbcsms_cal_bc_rec_index */
/*lint +e818*/

/*--------------------------------------------------------------------------
  The following functions are used for the Broadcast SMS on both the
  Paging Channel and IS2000 Rel A Common Channel (BCCH/FCCCH).
  -------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION MCCBCSMS_DETECT_DUPLICATE

DESCRIPTION
  This function checks for duplicate broadcast message in bc_unqiue_entries[]
  based on the BURST_TYPE and BC_ADDR. It will also remove the obsolete
  records, whose timestamps are older than the expiry cycle, from the list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a duplicate is found, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_detect_duplicate
(
  /* all the info needed to determine whether a dup is received */
  byte burst_type,
  const byte *bc_addr,
  dword slot_stamp,         /* slot stamp in 80ms */
  dword bc_expire_cycle     /* expiration cycle, in 80ms */
)
{
  uint32 t_index ; /* temp index */
  uint32 count;    /* temp counter */
  dword t_expire_slot; /* temp slot stamp for calculate the expire slot */
  dup_detect_type * list_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  /* First, remove all those records whose timestamps are older than the expiry
   * cycle which means they are not candidates for duplicate detection any more */
  count = 0;
  t_index = mccbcsms_unique_recs.list_begin;

  while ( count < mccbcsms_unique_recs.tot_entries )
  {
     /* At the end of the loop, list_begin will be moved to the first entry
      * that needs to be checked for duplicate.
      */

#ifdef FEATURE_1X_CP_MEM_OPT
    list_ptr = mccbcsms_unique_recs.unique_entries[t_index];
#else /* !FEATURE_1X_CP_MEM_OPT */
    list_ptr = &mccbcsms_unique_recs.unique_entries[t_index];
#endif /* FEATURE_1X_CP_MEM_OPT */
    t_expire_slot = list_ptr->slot_stamp + bc_expire_cycle ;

    /*
     *  O = old slot, E = Expiry slot, N = new slot,
     *  where E = old + 4*BC_Cycle
     *
     *  Since, we are using dword instead of qword, there can be wraparound
     *  cases for either/or both of Expiry and New slots.
     *
     *  There are total 6 possible combinations of O, E & N.
     *
     *  1) --- O --- E --- N ----
     *  2) --- E --- N --- O ---- (expiry and new slots wrapped)
     *  3) --- N --- O --- E ---- (new slot wrapped)
     *     New is after expiry slot. So, no need to check for duplication.
     *
     *  4) --- O --- N --- E ----
     *  5) --- E --- O --- N ---- (expiry slot wrapped)
     *  6) --- N --- E --- O ---- (new and expiry slots wrapped)
     *     New is before expiry slot. Need to check for duplication.
     *
     */

    if ( ((list_ptr->slot_stamp <= slot_stamp) && (slot_stamp <= t_expire_slot)) ||
         ((t_expire_slot <= list_ptr->slot_stamp) && (list_ptr->slot_stamp <= slot_stamp)) ||
         ((slot_stamp <= t_expire_slot) && (t_expire_slot <= list_ptr->slot_stamp)) )
    {
      /* Since the unique records list is sorted with time of arrival of a BCSMS,
       * so all the BCSMSs starting from this one should be checked for
       * duplication */
      M1X_MSG( DCP, LEGACY_LOW,
        "All BCSMS starting from index %d, slot stamp 0x%x should be checked for dup",
        t_index,
        list_ptr->slot_stamp);
      break;
    }
    t_index = (t_index + 1) % MAX_BC_DUP_DETECT;
    count++;
#ifdef FEATURE_1X_CP_MEM_OPT
    /* Free the list_ptr for BCSMS which is expired*/
    modem_mem_free( list_ptr, MODEM_MEM_CLIENT_1X );
    list_ptr = NULL;
#endif /* FEATURE_1X_CP_MEM_OPT */
  } /* while */

  /* Decrement the number of total entries by the number of obsolete records.
   * count contains the number of stale records in the unique records list */
  mccbcsms_unique_recs.tot_entries -= count;
  mccbcsms_unique_recs.list_begin = t_index;

  /* Now check for duplicates among the records received within the expiry duty cycle */
  count = 0;
  t_index = mccbcsms_unique_recs.list_begin;
  while ( count < mccbcsms_unique_recs.tot_entries )
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    list_ptr = mccbcsms_unique_recs.unique_entries[t_index];
#else /* !FEATURE_1X_CP_MEM_OPT */
    list_ptr = &mccbcsms_unique_recs.unique_entries[t_index];
#endif /* FEATURE_1X_CP_MEM_OPT */

      if (list_ptr->burst_type == burst_type &&
          memcmp(list_ptr->bc_addr, bc_addr, CAII_BC_ADDR_LEN) == 0)
      {
        /* we have determined a duplicate */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Duplicate BCSMS, old time stamp 0x%x, new time stamp 0x%x",
        list_ptr->slot_stamp,
        slot_stamp);
        return (TRUE);
      }
    t_index = (t_index+1) % MAX_BC_DUP_DETECT;
    count++;
    /* continue checking the rest of the list for dups */
  }

  return (FALSE);

} /* mccbcsms_detect_duplicate */

/*===========================================================================

FUNCTION MCCBCSMS_INSERT_UNIQUE

DESCRIPTION
  This function inserts the broadcast msg time stamp into the
  bc_unqiue_entries[] based on BURST_TYPE and BC_ADDR.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  bc_unqiue_entries[] is updated

===========================================================================*/
void mccbcsms_insert_unique
(
  /* all the info needed to determine whether a dup is received */
  byte burst_type,
  const byte *bc_addr,
  dword slot_stamp      /* slot stamp, in 80ms */
)
{
  uint32 t_index ; /* temp index */
  dup_detect_type * list_ptr;
  size_t copy_size;
  /* Return value of memscpy function */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  if (mccbcsms_unique_recs.tot_entries == MAX_BC_DUP_DETECT) /*lint !e737 !e834 */
  {
    /* We have wrapped around. Douh! List overflow should be very
     * unlikely if the BC traffic model is correct. */
    mccbcsms_unique_recs.list_begin =
        (mccbcsms_unique_recs.list_begin + 1) % MAX_BC_DUP_DETECT;/*lint !e573 !e737 !e834 */
    mccbcsms_unique_recs.tot_entries--;
    M1X_MSG( DCP, LEGACY_LOW,
      "BCDUP list wrapped around. BCDUP minus.");
#ifdef FEATURE_1X_CP_MEM_OPT
    t_index =
        (mccbcsms_unique_recs.list_begin + mccbcsms_unique_recs.tot_entries)
          % MAX_BC_DUP_DETECT;
    list_ptr = mccbcsms_unique_recs.unique_entries[t_index];
#endif /* FEATURE_1X_CP_MEM_OPT */
  }
#ifdef FEATURE_1X_CP_MEM_OPT
  else
  {
    t_index =
        (mccbcsms_unique_recs.list_begin + mccbcsms_unique_recs.tot_entries)
          % MAX_BC_DUP_DETECT;
    if( (list_ptr = (dup_detect_type *) modem_mem_alloc( sizeof(dup_detect_type),
                                                         MODEM_MEM_CLIENT_1X )) != NULL )
    {
      memset( list_ptr, 0,sizeof(dup_detect_type) );
      mccbcsms_unique_recs.unique_entries[t_index] = list_ptr;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Buf allocation failed");
      return;
    }
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  t_index =
      (mccbcsms_unique_recs.list_begin + mccbcsms_unique_recs.tot_entries)
      % MAX_BC_DUP_DETECT;                                /*lint !e573 !e737 !e834 */
  list_ptr = &mccbcsms_unique_recs.unique_entries[t_index];
#endif /* FEATURE_1X_CP_MEM_OPT */

  list_ptr->burst_type = burst_type;

  copy_size = memscpy(list_ptr->bc_addr, CAII_BC_ADDR_LEN*sizeof(byte),
                      bc_addr, CAII_BC_ADDR_LEN);
  if( copy_size != CAII_BC_ADDR_LEN )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size",
      copy_size,
      CAII_BC_ADDR_LEN,
      CAII_BC_ADDR_LEN*sizeof(byte));
  }

  list_ptr->slot_stamp = slot_stamp;
  mccbcsms_unique_recs.tot_entries++;
  M1X_MSG( DCP, LEGACY_LOW,
    "BCDUP add unique record, Slot=0x%lx, Total entries=%d",
    slot_stamp,
    mccbcsms_unique_recs.tot_entries);

} /* mccbcsms_insert_unique */

/************************** End Internal Functions *************************/

/*===========================================================================

FUNCTION MCCBCSMS_INIT

DESCRIPTION
  Initialize the broadcast SMS service and its related variables.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccbcsms_init (void)
{
  mccbcsms_pages.bc_type            = BROADCAST_PAGE;
  mcc_bc_sms_enable                 = FALSE;

  mccbcsms_config.bcsms_enable      = FALSE;
  mccbcsms_config.bcast_init        = FALSE;
  mccbcsms_config.ovhd_chan         = CAIX_SC;  /* Start with Sync */
  mccbcsms_config.bcast_index       = 0xFF;
  mccbcsms_config.bcast_slot_cycle  = 0;
  mccbcsms_config.cancel_bcast      = FALSE;

  qw_set(mccbcsms_old_pch_bc_slot, 0, 0);
  mcc_bcsms_in_progress = FALSE;
  qw_set(mcc_bcsms_start_slot, 0, 0);

} /* mccbcsms_init */

/*===========================================================================

FUNCTION MCCBCSMS_RESET_BC_PAGES

DESCRIPTION
  Reset the (enhanced) broadcast pages and schedules.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccbcsms_reset_bc_pages (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( mccbcsms_config.ovhd_chan )
  {
    case CAIX_SC:
      /* Just completed BCSMS initialization and not yet rxed ESPM or MCRRPM.
         Treat it as PC. */
    case CAIX_PC:
      mccbcsms_pages.bc_type  = BROADCAST_PAGE;
      /* the next BC Msg index in page_recs to be received */
      mccbcsms_pages.pages.bc.nxt_rec_index     = 0;
      /* total number of msg bursts to be received */
      mccbcsms_pages.pages.bc.tot_bc_msg        = 0;
      break;

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        " Unrecognized ovhd_chan %d",
        mccbcsms_config.ovhd_chan);
      break;

  } /* switch */

} /* mccbcsms_reset_bc_pages */

/*===========================================================================

FUNCTION MCCBCSMS_PROC_MSG

DESCRIPTION
  This function process the System Param Message, Extended System Param
  Message, and the MCRR Param Message for the Broadcast SMS related
  parameters. It checks to see if the Broadcast SMS configuration has
  been changed. If so, it updates the broadcast slot cycle and resets
  the related broadcast schedules.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccbcsms_proc_msg
(
  byte              msg_type,
  cdma_bs_type      *new_bs_config
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (msg_type)
  {
    case CAI_SYS_PARM_MSG:
      /* -------------------------
      ** System Parameters Message
      ** ------------------------- */
      /* This is called when the System Param Message indicates */
      /* that no Ext. System Param Message is available in this */
      /* Paging Channel (EXT_SYS_PARAMETER field set to '0'.    */
      /* According to IS-95A, 6.6.2.2.1.1, set the bcast_index  */
      /* to MAX_SLOT_CYCLE_INDEX.                               */

      if ( new_bs_config->csp.sp.ext_sys_parameter )
      {
        break;
      }
      else
      {
        new_bs_config->csp.esp.bcast_index =
            new_bs_config->csp.sp.max_slot_cycle_index;
        M1X_MSG( DCP, LEGACY_MED,
          "SPM w/o ESPM, bcast_index=max_sci");
      }

      /* Fall Through */
      /*lint -fallthrough */

    case CAI_EXT_SYS_PARM_MSG:
      /* ----------------------------------
      ** Extended System Parameters Message
      ** ---------------------------------- */

      if (mccbcsms_config.ovhd_chan != CAIX_PC ||
          !mccbcsms_config.bcast_init ||
          mccbcsms_config.bcast_index != new_bs_config->csp.esp.bcast_index)
      {
        mccbcsms_config.bcast_init  = TRUE;
        mccbcsms_config.ovhd_chan   = CAIX_PC;

        /* broadcast slot cycle = 2^bcast_index x 16 + 3 */
        if (new_bs_config->csp.esp.bcast_index != 0)
        {
          mccbcsms_config.bcast_slot_cycle =
              (16UL << (new_bs_config->csp.esp.bcast_index)) + 3;  /*lint !e734 */

          if(mccbcsms_config.bcast_index != new_bs_config->csp.esp.bcast_index)
          {
            event_report_payload(EVENT_BROADCAST_CYCLE_ENABLED,
                                 sizeof(mccbcsms_config.bcast_slot_cycle),
                                 &mccbcsms_config.bcast_slot_cycle);
          }
        }
        else
        {
          mccbcsms_config.bcast_slot_cycle = 0;
          if(mccbcsms_config.bcast_index != new_bs_config->csp.esp.bcast_index)
          {
            event_report(EVENT_BROADCAST_CYCLE_DISABLED);
          }
        }

        mccbcsms_config.bcast_index = new_bs_config->csp.esp.bcast_index;

        /* Reset the enhanced broadcast page structure. */
        mccbcsms_reset_bc_pages();

        /* Init the dup list */
        mccbcsms_unique_recs.list_begin     = 0;
        mccbcsms_unique_recs.tot_entries    = 0;

        if (mccidl_is_fast_raho())
        {
          /* Fast RAHO, remember to cancel the bcast */
          mccbcsms_config.cancel_bcast = TRUE;
          M1X_MSG( DCP, LEGACY_HIGH,
            "Fast RAHO: don't send cmd to search");
        }
        else
        {
          /* Cancel the broadcast schedule to Search */
          mccbcsms_cancel_bcast();
        }
      }
      M1X_MSG( DCP, LEGACY_MED,
        "(E)SPM, bcast_index=%d, bc_enable=%d",
        new_bs_config->csp.esp.bcast_index,
        mccbcsms_config.bcsms_enable);
      break;

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unrecognized msg type %d",
        msg_type);
      break;
  } /* end switch */

} /* mccbcsms_proc_msg */

/*===========================================================================

FUNCTION MCCBCSMS_ENABLE

DESCRIPTION
  This function enables or disables the broadcast SMS depends on the input.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mccbcsms_enable
(
  boolean bcsms_enable
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mccbcsms_config.bcsms_enable != bcsms_enable)
  {
    mccbcsms_config.bcsms_enable =
    mcc_bc_sms_enable            = bcsms_enable;

    if (mccbcsms_config.bcsms_enable)
    {
      mccbcsms_reset_bc_pages();

      /* Don't reset the unique bcast list variables, because   */
      /* they contain the time stamp, which might be valid if   */
      /* bcsms happens to be enabled again.                     */
    }
    else
    {
      /* BCSMS is disabled. Cancel the broadcast schedule */
      mccbcsms_cancel_bcast();
    }

    /* If bcsms_enable changed, CP may need to update the */
    /* bcast_index value to Search, by sending a parm cmd */
    mccidl_send_srch_parms();
  }
} /* mccbcsms_enable */

/*===========================================================================

FUNCTION MCCBCSMS_IS_BCAST_INIT

DESCRIPTION
  This procedure checks wheter broadcast info has been initialized.

DEPENDENCIES
  None

RETURN VALUE
  Returns bcast_init.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccbcsms_is_bcast_init(void)
{
  return (mccbcsms_config.bcast_init);

} /* mccbcsms_is_bcast_init */

/*===========================================================================

FUNCTION MCCBCSMS_IS_BC_PAGE_SLOT

DESCRIPTION
  This function returns whether the given frame is a broadcast page slot. A
  paging slot is a broadcast slot, if it's the first or second slot within
  the current broadcast cycle.
  It takes the frame when the message is received as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it's a broadcast page slot

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_is_bc_page_slot
(
  qword msg_frame_20        /* Msg frame, in 20 ms */
)
{
  qword tmp_qw;
  word  bc_slot_pos;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( mccbcsms_config.bcsms_enable && mccbcsms_config.bcast_slot_cycle != 0)
  {
    qw_equ(tmp_qw, msg_frame_20);
    qw_shift(tmp_qw, -2);       /* Convert to 80 ms */

    bc_slot_pos = qw_div(tmp_qw, tmp_qw, mccbcsms_config.bcast_slot_cycle);

    /* the current slot is a broadcast slot if it is within the first two
     * slots of a broadcast cycle */
    return (bc_slot_pos==0 || bc_slot_pos==1);
  }
  else
  {
    /* multi-slotted mode */
    return FALSE;
  }

} /* mccbcsms_is_bc_page_slot */

/*===========================================================================

FUNCTION MCCBCSMS_IS_BC_DATA_SLOT

DESCRIPTION
  This function returns whether the given frame is a scheduled broadcast
  slot to receive data burst.
  It takes the frame when the message is received as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it's a broadcast data slot

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_is_bc_data_slot
(
  qword msg_frame_20        /* Msg frame, in 20 ms */
)
{
  boolean   is_bc_slot = FALSE;
  qword     tmp_qw;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if a broadcast msg has been scheduled and the current slot is
   * the first slot of the scheduled wakeup slots. */
  if (mccbcsms_config.ovhd_chan == CAIX_PC)
  {
    byte      nxt_rec_index;

    if (mccbcsms_config.bcsms_enable &&
        mccbcsms_pages.pages.bc.tot_bc_msg != 0 &&
        mccbcsms_pages.pages.bc.nxt_rec_index != 0 &&
        mccbcsms_config.bcast_slot_cycle != 0 )
    {
      /* Set nxt_rec_index to the index of the scheduled BCSMS */
      nxt_rec_index = mccbcsms_pages.pages.bc.nxt_rec_index - 1;

      qw_equ(tmp_qw, msg_frame_20);
      qw_shift(tmp_qw, -2);       /* 80 ms slot */

      /* We need to skip the BCSMS whose wakeup slot has passed */
      while ((nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg)
             &&(nxt_rec_index < MAX_BC_PER_PAGE_MSG)
             &&(qw_cmp(tmp_qw, mccbcsms_pages.pages.bc.page_recs
                 [nxt_rec_index].wakeup_slot) > 0)
            )
      {
        nxt_rec_index++;
        M1X_MSG( DCP, LEGACY_MED,
          "Wakeup slot for BCSMS num %d already passed before slot 0x%x",
          nxt_rec_index,
          qw_lo(msg_frame_20));
      }
      /* The message pointed to by nxt_rec_index is
       * the one for which current slot could be a start slot */

      if (nxt_rec_index < MAX_BC_PER_PAGE_MSG)
      {
        is_bc_slot =
            !qw_cmp(mccbcsms_pages.pages.bc.page_recs
                        [nxt_rec_index].wakeup_slot, tmp_qw );
      }

      /* Set the index to the page number for the next possible BCSMS.
       * Add 1 because mccbcsms_pages.pages.bc.nxt_rec_index always
       * points to 1 ahead of scheduled BCSMS. */
      mccbcsms_pages.pages.bc.nxt_rec_index = nxt_rec_index + 1;

    }
    else
    {
      is_bc_slot = FALSE;
    }
  }

  return (is_bc_slot);

} /* mccbcsms_is_bc_data_slot */

/*===========================================================================

FUNCTION MCCBCSMS_PC_MATCH

DESCRIPTION
  This function parse the *bcast_hdr received on the Paging channel to see
  if it is a BC message header that contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *pc_hdr contains
  enough useful bits (at least CAII_BC_HDR_LEN bits). And the current slot
  is the scheduled BC SMS slot.

RETURN VALUE
  TRUE if it contains the expected BC_ADDR, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_pc_match
(
  byte * bcast_hdr
)
{
  word msg_pos;
  caii_pc_hdr_type bc_hdr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( b_unpackb( bcast_hdr,
                  FPOS( cai_gen_type, msg_type ), /*lint !e734 */
                  FSIZ( cai_gen_type, msg_type )
                 ) != CAI_PC_BURST_MSG )
  {
    /* if the MSG_TYPE is not Data Burst Message, it's not a BC SMS */
    return FALSE;
  }
  msg_pos = FSIZ(cai_pc_hdr_type, msg_type);
  xlate_ext_pc_hdr(&msg_pos, bcast_hdr, &bc_hdr);
  /* now we have the parsed header, we need to look the contents */
  if ((bc_hdr.addr_type == CAI_BCAST_ADDR_TYPE) &&
      (cm_wms_bc_addr_match(bc_hdr.addr.type5.bc_addr))
     )
  {
    /* we have a BCAST address type and BC_ADDR also matches, so it's the one */
    M1X_MSG( DCP, LEGACY_MED,
      "BCSMS hdr match");
    return TRUE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "BCSMS hdr not match");
    return FALSE;
  }

} /* mccbcsms_pc_match */

/*===========================================================================

FUNCTION MCC_BC_MATCH

DESCRIPTION
  This function parse the *bcast_hdr to see if it is a BC message header that
  contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *bcast_hdr contains
  enough useful bits (at least CAII_BC_HDR_LEN bits). And the current slot
  is the scheduled BC SMS slot.

RETURN VALUE
  TRUE if it contains the expected BC_ADDR, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
uint16 mcc_bc_match(byte * bcast_hdr)
{
    mcc_bcsms_in_progress = mccbcsms_pc_match(bcast_hdr);

    if(mcc_bcsms_in_progress)
    {
      m1x_time_get_20ms_frame_time( mcc_bcsms_start_slot); // 20ms frame
      qw_shift(mcc_bcsms_start_slot, -2);  // 80ms slot
      M1X_MSG( DCP, LEGACY_MED,
        "BCSMS started in slot 0x%x:%x",
        qw_hi(mcc_bcsms_start_slot),
        qw_lo(mcc_bcsms_start_slot));
    }
    else
    {
      qw_set(mcc_bcsms_start_slot, 0, 0);
    }
    return (mcc_bcsms_in_progress);

} /* mcc_bc_match */

/*===========================================================================

FUNCTION MCCBCSMS_SCHEDULE_BCAST

DESCRIPTION
  This function schedules the Broadcast SMS by finding the next Broadcast
  wakeup slot and sending them to the Searcher through SRCH_BC_INFO_F.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_schedule_bcast(void)
{
  qword     cur_slot;
  boolean   update_schedule = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do nothing if BCSMS is not enabled. */
  if ( !mccbcsms_config.bcsms_enable )
  {
    return;
  }

  /* get the current time */
  m1x_time_get_20ms_frame_time(cur_slot);

  if (mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    qw_shift(cur_slot, -2); /* cur_time >> 2 to get 80 ms slot number */

    while ((mccbcsms_pages.pages.bc.nxt_rec_index <
            mccbcsms_pages.pages.bc.tot_bc_msg     )
           &&
           (mccbcsms_pages.pages.bc.nxt_rec_index < MAX_BC_PER_PAGE_MSG)
           &&
            (qw_cmp(cur_slot, mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot) >= 0)
          )
    {
      /* We need to ignore the wakeup slots that already passed */
      mccbcsms_pages.pages.bc.nxt_rec_index++;
      M1X_MSG( DCP, LEGACY_MED,
        "Ignore BC SMS page num %d",
        mccbcsms_pages.pages.bc.nxt_rec_index);
    }

    if ((mccbcsms_pages.pages.bc.nxt_rec_index > 0)
        &&
        (qw_cmp(cur_slot, mccbcsms_pages.pages.bc.page_recs
             [mccbcsms_pages.pages.bc.nxt_rec_index - 1].wakeup_slot) < 0)
       )
    {
      /* if after scheduling the last bc, the MS didn't goto sleep,
       * start_sleep() will be repeatedly called. in this case, we can't
       * increment the record index */
      mccbcsms_pages.pages.bc.nxt_rec_index--;
    }

    /* If the current top records are duplicated with the received
     * ones, do not schedule the wake up for those records. Keeps searching
     * until a non-duplicated record is found, or no more records to be checked */
    if (mccbcsms_config.bcast_slot_cycle != 0)
    {
      /* the duplication cycle need be in unit of 80ms as required by
       * the fucntion mccbcsms_detect_duplicate */
      dword dup_cycle = (mccbcsms_config.bcast_slot_cycle << 2);

      /* get the current slot for duplication timing check */
      dword current_slot = qw_lo(cur_slot);

      while (mccbcsms_pages.pages.bc.nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg )
      {
        /* Check if the record pointed by the index is duplicated with the received ones */
        if (! mccbcsms_detect_duplicate(CAI_SHORT_MSG_SERVICES,
                                        mccbcsms_pages.pages.bc.page_recs[mccbcsms_pages.pages.bc.nxt_rec_index].bc_addr,
                                        current_slot,     /* 80ms unit */
                                        dup_cycle))       /* 80ms unit */
        {
          /* stop the loop once a non-duplicated record is found, and schedule this
           * record at the end */
          M1X_MSG( DCP, LEGACY_MED,
            "No duplication found for BC SMS page num=%d",
            (mccbcsms_pages.pages.bc.nxt_rec_index + 1));
          break;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "No wakeup schedule for duplicated BC SMS page num=%d",
            (mccbcsms_pages.pages.bc.nxt_rec_index + 1));
          /* if the current one is a duplicated one, continue to check the next one
           * until a non-duplicated record is found, or there is no more to check */
          mccbcsms_pages.pages.bc.nxt_rec_index++;
        }
      } /* while loop */
    } /* if (mccbcsms_config.bcast_slot_cycle != 0) */

    /* looking into mccbcsms_page_recs[] */
    if (mccbcsms_pages.pages.bc.nxt_rec_index <
        mccbcsms_pages.pages.bc.tot_bc_msg      )
    {
      if (qw_cmp(mccbcsms_old_pch_bc_slot,
                 mccbcsms_pages.pages.bc.page_recs
                   [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot) != 0)
      {
        /* If the old schedule changes, update Search by SRCH_BC_INFO_F */
        update_schedule = TRUE;
        mcc_srch_buf.bc.monitor_bcch = FALSE;
        qw_equ(mcc_srch_buf.bc.next_bcast_slot.time,
               mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot);
        M1X_MSG( DCP, LEGACY_MED,
          "BC_INFO 0x%x (40ms) 0x%x (80ms)",
          (qw_lo(mcc_srch_buf.bc.next_bcast_slot.time)&0xFF),
          ((qw_lo(mcc_srch_buf.bc.next_bcast_slot.time) >> 1) & 0xFF));

        /* Save the old bc schedule */
        qw_equ(mccbcsms_old_pch_bc_slot,
               mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot);
      }
      /* Update the index */
      mccbcsms_pages.pages.bc.nxt_rec_index++;
    }
    else
    {
      /* all the broadcast msgs are finished */
      mccbcsms_pages.pages.bc.tot_bc_msg = 0;
      mccbcsms_pages.pages.bc.nxt_rec_index = 0;
      M1X_MSG( DCP, LEGACY_MED,
        "All BC page done");
    }
  } /* ovhd_chan == PCH */

  /* Send the BC Info command to Search */
  if (update_schedule)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "BC_INFO to srch");
    mcc_srch_buf.bc.hdr.cmd = SRCH_BC_INFO_F;
    mcc_srch_cmd(&mcc_srch_buf);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "BC Schedule no change");
  }

} /* mccbcsms_schedule_bcast */



/*===========================================================================

FUNCTION MCCBCSMS_SCHED_BCAST_UPON_WAKEUP

DESCRIPTION
  This function schedules the Broadcast SMS upon wakeup, by finding the
  next Broadcast wakeup slot and sending them to the Searcher through
  SRCH_BC_INFO_F.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_sched_bcast_upon_wakeup
(
  qword cur_time_20ms /* in 20ms */
)
{
  qword     cur_slot;
  boolean   update_schedule = FALSE;
  byte      nxt_rec_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do nothing if BCSMS is not enabled. */
  if ( !mccbcsms_config.bcsms_enable )
  {
    return;
  }

  qw_equ(cur_slot, cur_time_20ms);

  if (mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    qw_shift(cur_slot, -2); /* cur_time >> 2 to get 80 ms slot number */

    nxt_rec_index = mccbcsms_pages.pages.bc.nxt_rec_index;

    while ((nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg )
           &&
           (nxt_rec_index < MAX_BC_PER_PAGE_MSG)
           &&
           (qw_cmp(cur_slot,
              mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot)
            >= 0)
          )
    {
      /* We need to ignore the wakeup slots that already passed */
      nxt_rec_index++;
    }

    /* looking into mccbcsms_page_recs[] */
    if ( (nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg) &&
         (nxt_rec_index < MAX_BC_PER_PAGE_MSG) )
    {
      if (qw_cmp(mccbcsms_old_pch_bc_slot,
                 mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot)
          != 0)
      {
        /* If the old schedule changes, update Search by SRCH_BC_INFO_F */
        update_schedule = TRUE;
        mcc_srch_buf.bc.monitor_bcch = FALSE;
        qw_equ(mcc_srch_buf.bc.next_bcast_slot.time,
               mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot);
        M1X_MSG( DCP, LEGACY_MED,
          "BC_INFO to srch 0x%x (40ms) 0x%x (80ms)",
          (qw_lo(mcc_srch_buf.bc.next_bcast_slot.time)&0xFF),
          ((qw_lo(mcc_srch_buf.bc.next_bcast_slot.time) >> 1) & 0xFF));

        /* Save the old bc schedule */
        qw_equ(mccbcsms_old_pch_bc_slot,
               mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot);
      }
    }
  } /* ovhd_chan == PCH */

  /* Send the BC Info command to Search */
  if (update_schedule)
  {
    mcc_srch_buf.bc.hdr.cmd = SRCH_BC_INFO_F;
    mcc_srch_cmd(&mcc_srch_buf);
  }

} /* mccbcsms_sched_bcast_upon_wakeup */



/*===========================================================================

FUNCTION MCCBCSMS_CANCEL_BCAST

DESCRIPTION
  This function cancels the previous broadcast schedule to Search.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_cancel_bcast(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mcc_srch_buf.bc.hdr.cmd = SRCH_BC_INFO_F;

  if (mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    mcc_srch_buf.bc.monitor_bcch = FALSE;
    qw_set(mcc_srch_buf.bc.next_bcast_slot.time, 0, 0);
  }

  mcc_srch_cmd(&mcc_srch_buf);

} /* mccbcsms_cancel_bcast */

/*===========================================================================

FUNCTION MCCBCSMS_POST_FRAHO_PROC

DESCRIPTION
  This function performes the actions suppressed by the fast RAHO.
  If the bcast_index or non-primary BCCH channel configuration has
  been changed during fast RAHO, it sends the canceling of bcast
  schedule to Search.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_post_fraho_proc(void)
{
  /* Check if we need to cancel the broadcast schedule */
  if (mccbcsms_config.cancel_bcast)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Wakeup, cancel bcast to search");
    mccbcsms_cancel_bcast();
    mccbcsms_config.cancel_bcast = FALSE;
  }
}

/*===========================================================================

FUNCTION MCCBCSMS_COMP_ADDR

DESCRIPTION
  This function does necessary processing when receiving a broadcast message.
  It compares the BC address and checks if this is an expected broadcast
  message corresponding to the outstanding schedules. It also checks if it's
  a duplicate. If it's unique, it then inserts the broadcast message into
  the unique bcast list so that later broadcast pages and messages can
  perform the duplicate detection agaist it.

  This function is called from mcc_comp_addr().

  Please note the processings for the Paging Ch and BCCH/FCCCH are different.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if a valid broadcast address is found and the broadcast message is
  unique. FALSE otherwise.

SIDE EFFECTS
  It updates the bc_unique_entries.

===========================================================================*/
boolean mccbcsms_comp_addr
(
  caii_pc_hdr_type *msg_hdr,
    /* Header of paging channel message to match address of */
  qword msg_frame_20
    /* Msg frame, in 20 ms */
)
{
  boolean addr_found = FALSE;
  qword tmp_qw;
  dword current_slot;       /* for inserting to the dup list */
  dword dup_cycle;          /* dup detection cycle, in 80ms  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!mccbcsms_config.bcsms_enable)
  {
    /* we don't look at the addr if the feature is disabled */
    return FALSE;
  }

  /* If we receive a broadcast SMS before initializing the bcast */
  /* index, don't perform the dup detection. Instead we simply   */
  /* perform the address matching and return                     */
  if ( !mccbcsms_config.bcast_init )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Received a BCSMS before Overhead");

    if (cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  /* Calculate the duplication detection cycle */
  if (mccbcsms_config.bcast_slot_cycle != 0)
  {
    dup_cycle = mccbcsms_config.bcast_slot_cycle << 2;
  }
  else
  {
    dup_cycle = 16UL<<cur_bs_ptr->csp.sp.max_slot_cycle_index;
  }

  /* Perform the broadcast address matching for the Paging channel */
  if ( mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    qw_equ(tmp_qw, msg_frame_20);
    qw_shift(tmp_qw, -2);       /* Convert to slot in 80ms */
    current_slot = qw_lo(tmp_qw);

    if ( ( mccbcsms_pages.pages.bc.tot_bc_msg > 0 )     &&
         ( mccbcsms_pages.pages.bc.nxt_rec_index != 0 ) &&
         ( mcc_is_ovhd_cfg_okay() )                         )
    {
      /* if we wakeup at the scheduled slot and match the bc_addr */
      if ( mccbcsms_config.bcast_slot_cycle != 0 &&
           mccbcsms_is_bc_data_slot(msg_frame_20) )
      {
        /* Multiple GPM may have the same broadcast data    */
        /* slot schedules. We need to check all of them.    */

        unsigned int i;

        for (i = mccbcsms_pages.pages.bc.nxt_rec_index;
             ((i <= mccbcsms_pages.pages.bc.tot_bc_msg) &&
              ((i-1) < MAX_BC_PER_PAGE_MSG)) &&
             (qw_cmp(mccbcsms_pages.pages.bc.page_recs[i-1].wakeup_slot,
                      tmp_qw) == 0); i++ )
        {
          if (memcmp(mccbcsms_pages.pages.bc.page_recs[i-1].bc_addr,
                     msg_hdr->addr.type5.bc_addr, CAII_BC_ADDR_LEN) == 0)
          {
            /* if the currently expected BC_ADDR is the same as the */
            /* one arrives, then we have a match                    */
            M1X_MSG( DCP, LEGACY_MED,
              "Rec'd scheduled BCSMS, page num=%d",
              i);

            /* the expiration cycle for periodic broadcast is
               4*bcast_slot_cycle */
            if (!mccbcsms_detect_duplicate( CAI_SHORT_MSG_SERVICES,
                                            msg_hdr->addr.type5.bc_addr,
                                            current_slot,     /* 80ms unit */
                                            dup_cycle))       /* 80ms unit */
            {
              mccbcsms_insert_unique( CAI_SHORT_MSG_SERVICES,
                                      msg_hdr->addr.type5.bc_addr,
                                      current_slot);          /* 80ms unit */
              addr_found = TRUE;
            }

            /* Update sleep slot mask */
            mccidl_update_sleep_slot_mask(BC_DATA_SLOT);

            if (mccidl_assert_sleep_ok(msg_frame_20))
            {
              M1X_MSG( DCP, LEGACY_MED,
                "BC SMS Rec'd, Sleep OK");
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Sleep_ok not asserted");
            }
            break;
          }
        } /* for */
      } /* if mccbcsms_config.bcast_slot_cycle != 0 */
      else
      {
        /* Originally we don't support multi-slotted and periodic mode
         * broadcast simultaneously, but per Lucent's request, I will put in
         * the code to support this, although even Lucent doesn't know if
         * they will eventually implement such broadcast mode */

        /* we received a BC that's not expected in periodic mode broadcast */
        /* for multislotted mode broadcast, the expiration cycle is the
         * maximum slot cycle */
        if ((cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr)) &&
            (!mccbcsms_detect_duplicate( CAI_SHORT_MSG_SERVICES,
                                         msg_hdr->addr.type5.bc_addr,
                                         current_slot,
                                         dup_cycle
                                 )
            ))
        {
          addr_found = TRUE;
          mccbcsms_insert_unique( CAI_SHORT_MSG_SERVICES,
                                  msg_hdr->addr.type5.bc_addr,
                                  current_slot);
          M1X_MSG( DCP, LEGACY_MED,
            "Rec'd mslotd BCSMS.");
          /* it's a multi-slotted bc sms */
        }
      }
    }
    else
    {
      if (cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr))
      {
        if (!mccbcsms_detect_duplicate( CAI_SHORT_MSG_SERVICES,
                                        msg_hdr->addr.type5.bc_addr,
                                        current_slot,
                                        dup_cycle))
        {
          addr_found = TRUE;
          mccbcsms_insert_unique( CAI_SHORT_MSG_SERVICES,
                                  msg_hdr->addr.type5.bc_addr,
                                  current_slot);
        }
        if (mccbcsms_config.bcast_slot_cycle != 0)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Rec'd nonsch'd BCSMS, BC_ADDR match=%d",
            addr_found);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Rec'd mslotd BCSMS, BC_ADDR match=%d",
            addr_found);
        }
      }
    }
  } /* ovhd_chan == PC */

  if (addr_found)
  {
    /* Received a broadcast message. Check if we need to update */
    /* Search with new broadcast info.                          */
    mccbcsms_schedule_bcast();
  }

  return addr_found;

} /* mccbcsms_comp_addr */

/*===========================================================================

FUNCTION MCCBCSMS_PAGE_MATCH

DESCRIPTION
  This function performs broadcast page match operation.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE broadcast or enhanced broadcast page match.
  Otherwise, FALSE.

SIDE EFFECTS

===========================================================================*/
boolean mccbcsms_page_match
(
  mccbcsms_bc_info_type *bc_ptr /* Pointer to broadcast info */
)
{
  qword    bc_wakeup_slot;     /* tmp variable to cal. the wakeup slot */
  boolean  bc_found = FALSE;
  qword    msg_slot;
  size_t copy_size;
  /* Return value of memscpy function */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_equ(msg_slot, bc_ptr->frame);
  qw_shift(msg_slot, -2);       /* msg slot in 80 ms */

  if (bc_ptr->bc_type == BROADCAST_PAGE)
  {
    /* Broadcast Page */

    if (bc_ptr->burst_type == CAI_SHORT_MSG_SERVICES)
    {
      if (!OVHD_CHAN_IS_PCH)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Received broadcast page on F-CCCH");
        return (FALSE);
      }

      if (mccbcsms_config.bcsms_enable && !mccbcsms_config.bcast_init)
      {
        /* bc_index is not initialized, we can't process the GPM right now */
        return (TRUE);
      }

      if (!mccbcsms_config.bcsms_enable || mccbcsms_config.bcast_slot_cycle == 0)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Broadcast SMS page ignored, feature not enabled");
        return (FALSE);
      }

      if (mccbcsms_pages.pages.bc.tot_bc_msg == MAX_BC_PER_PAGE_MSG) /*lint !e834 */
      {
        /* Check if any broadcast data slots have been expired. */
        while ((mccbcsms_pages.pages.bc.nxt_rec_index <
                mccbcsms_pages.pages.bc.tot_bc_msg     )
               &&
               (qw_cmp(msg_slot, mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot) > 0))
        {
          /* We need to ignore the wakeup slots that already passed */
          mccbcsms_pages.pages.bc.nxt_rec_index++;
          M1X_MSG( DCP, LEGACY_MED,
            "Ignore BC SMS page num %d",
            mccbcsms_pages.pages.bc.nxt_rec_index);
        }
        if ( mccbcsms_pages.pages.bc.nxt_rec_index
             ==
             mccbcsms_pages.pages.bc.tot_bc_msg )
        {
          mccbcsms_pages.pages.bc.nxt_rec_index = 0;
          mccbcsms_pages.pages.bc.tot_bc_msg    = 0;
        }

        if (mccbcsms_pages.pages.bc.tot_bc_msg == MAX_BC_PER_PAGE_MSG)  /*lint !e834 */
        {
          /* ------------------------------------------------------------------
          ** We should never get here with the current understanding of the
          ** standard, however if multiple GPMs that occupied more than 2 slots
          ** are allowed, MAX_BC_PER_PAGE_MSG might need to be changed.
          ** ------------------------------------------------------------------ */
          M1X_MSG( DCP, LEGACY_ERROR,
            "BCast page buffer overflow, discard remain pages");
          return (FALSE);
        }
      }

      if (bc_ptr->addr_len == CAII_BC_ADDR_LEN)
      {
        if (cm_wms_bc_addr_match(bc_ptr->bc_addr))
        {
          /* --------------------------------------------------------------
          ** We have a broadcast SMS match the next slot to wake up is
          ** calculated by adding 3 x bc_page_number to the reference slot.
          ** -------------------------------------------------------------- */
          qw_equ(bc_wakeup_slot, msg_slot);
          qw_inc(bc_wakeup_slot, (dword) (bc_ptr->page.bc.num_bc_page * 3));  //lint !e732

          /* ---------------------------------------------------------------
          ** We need to check for duplicates, it's different from the normal
          ** page msg duplicate detection, since BC page doesn't have
          ** msg_seq.
          **
          ** When check for duplicate, we should use the page slot of
          ** the future Data Burst not the current page slot.
          ** --------------------------------------------------------------- */
          if (!mccbcsms_detect_duplicate(bc_ptr->burst_type,
                                         bc_ptr->bc_addr,
                                         qw_lo(bc_wakeup_slot),     /* 80ms */
                                         4 * mccbcsms_config.bcast_slot_cycle))
                                                                    /* 80ms */
          {
            byte t_index;

            bc_found = TRUE;

            /* --------------------------------------
            ** If not duplicate, start sorted insert.
            ** -------------------------------------- */

            /* First, calculate the index where we should insert the record */
            t_index =
                mccbcsms_cal_bc_rec_index(mccbcsms_pages.pages.bc.page_recs,
                                          bc_wakeup_slot);

            if ( t_index >= MAX_BC_PER_PAGE_MSG )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "t_index (%d)is greater than %d",
                t_index,
                MAX_BC_PER_PAGE_MSG);
              return (FALSE);
            }

            copy_size = memscpy(mccbcsms_pages.pages.bc.page_recs[t_index].bc_addr,
                                CAII_BC_ADDR_LEN*sizeof(byte),
                                bc_ptr->bc_addr,CAII_BC_ADDR_LEN
                               );
            if( copy_size != CAII_BC_ADDR_LEN )
             {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                 copy_size,
                 CAII_BC_ADDR_LEN,
                 CAII_BC_ADDR_LEN*sizeof(byte));
             }


            mccbcsms_pages.pages.bc.page_recs[t_index].burst_type =
                bc_ptr->burst_type;

            /* Assign the next wake up slot */
            qw_equ(mccbcsms_pages.pages.bc.page_recs[t_index].wakeup_slot,
                   bc_wakeup_slot);
            mccbcsms_pages.pages.bc.tot_bc_msg++;

            /* Reset the next BC Msg index to be received */
            mccbcsms_pages.pages.bc.nxt_rec_index = 0;

            M1X_MSG( DCP, LEGACY_MED,
              "BC rec index=%d, wake_slot=0x%x",
              t_index,
              qw_lo(bc_wakeup_slot));
          }
        }
      }
      /* we do nothing if addr_len is not correct, since very unlikely. */
    }
    else
    {
      /* Unsupported burst type */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unsupported burst %d",
        bc_ptr->burst_type);
    }
  }   /* Broadcast Page */

  return (bc_found);

} /* mccbcsms_page_match */

/*===========================================================================

FUNCTION MCCBCSMS_IS_ENABLED

DESCRIPTION
  This function returns if the Broadcast SMS is enabled or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Broadcast SMS is enabled.
  FALSE, if disabled.

SIDE EFFECTS

===========================================================================*/
boolean mccbcsms_is_enabled( void )
{

  return mccbcsms_config.bcsms_enable;

} /* mccbcsms_is_enabled */

