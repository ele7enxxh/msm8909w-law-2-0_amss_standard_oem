#ifndef MCCBCSMS_H
#define MCCBCSMS_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          M A I N  C O N T R O L

        P O S I T I O N   D E T E R M I N A T I O N   S E R V I C E S

GENERAL DESCRIPTION

  This module contains functions for the Broadcast SMS services.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccbcsms.h_v   1.7   27 Jun 2002 19:49:36   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccbcsms.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/12   ppr     FR#2370 - 1x memory optimization changes
05/09/12   ppr     Feature Cleanup
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/13/11   ag      Moved mccbcsms_is_enabled() outside FEATURE_BROADCAST_SMS.
11/15/10   jtm     Initial SU API split for CP.
07/13/10   ag      Fixed lint errors.
05/12/09   ag      Fixed the issue of missing BCSMS when MS transitions from
                   system access to idle state.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
09/10/07   yll     Fixed a problem that MS doesn't receive consecutive and
                   large broadcast messages that take 3 paging channel slots
                   to send.
04/18/06   fc      Changed mccbcsms_config_type to use
                   cdma_bcast_bcch_info_type.
                   structure on ebcsms info.
05/21/04   fc      Lint clean up.
03/12/04   fc      Moved the definition of MCC_PRI_BCCH_BCN to mccdma.h.
10/03/03   yll     Added mccbcsms_post_fraho_proc().
01/09/02   yll     Changes to save the old bc schedule on PCH that is
                   previously sent to Search, so that it doesn't send
                   multiple times.
10/28/02   yll     Moved the broadcast schedule info from the sleep command
                   to bc_info_f.
10/24/02   yll     Changed the mccbcsms_match to mccbcsms_pc_match and
                   mccbcsms_bcch_match functions.
06/27/02   fc      Changed UPM function names.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/12/02   yll     Updates after code review.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed the function names.
02/10/02   yll     Initial revision.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "cai.h"
#include "caii.h"
#include "caix.h"
#include "caix_i.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"




/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* ------------------------------------------------------------------------
   Broadcast SMS global definations, and variables
   ------------------------------------------------------------------------*/
// MAX_BC_PER_PAGE_MSG ~= 40
#define MAX_BC_PER_PAGE_MSG  \
         (((CAI_PC_BODY_SIZE - sizeof(cai_page_fix_type)) - 12) \
         / (sizeof (cai_pc_3_sc_0_fix_type) + CAII_BC_ADDR_LEN*8))
/* All the done bits plus reserved and add_length bits is 12 */

#define MAX_BC_DUP_DETECT (MAX_BC_PER_PAGE_MSG * 4)

typedef struct
{
  byte burst_type;
  byte bc_addr[CAII_BC_ADDR_LEN];
  dword slot_stamp; /* the slot stamp the dup is compared against, in 80ms */
} dup_detect_type;

typedef struct
{
  /* a list to keep all the unique broadcast msgs with time stamps for
   * duplication detection */
#ifdef FEATURE_1X_CP_MEM_OPT
  dup_detect_type   *unique_entries[MAX_BC_DUP_DETECT];
#else /* !FEATURE_1X_CP_MEM_OPT */
  dup_detect_type   unique_entries[MAX_BC_DUP_DETECT];
#endif /* FEATURE_1X_CP_MEM_OPT */


  /* the beginning of the above list, the oldest entry*/
  uint32 list_begin;

  /* the total number of entries to be compared in the list */
  uint32 tot_entries;

} mccbcsms_unique_rec_type;


/* the record to hold BC_ADDRs and BURST_TYPEs, once BC pages pass the
 * filter, to schedule SRCH to receive BC Msgs. It's also needed again to
 * varify the BURST_TYPE and BC_ADDR when BC Msgs arrives. */
typedef struct
{
  byte bc_addr[CAII_BC_ADDR_LEN];
  byte burst_type;
  /* the slot to wakeup, it's used for dup detection in PCH, in 80ms */
  qword wakeup_slot;
} bc_page_type;

typedef struct
{
  /* a record array to keep track of matched Broadcast pages */
  bc_page_type  page_recs[MAX_BC_PER_PAGE_MSG];

  /* the next BC Msg index in page_recs to be received */
  byte          nxt_rec_index;

  /* total number of msg bursts to be received */
  byte          tot_bc_msg;

} mccbcsms_bc_type;

/* Type of broadcast page */
typedef enum
{
  BROADCAST_PAGE
} mccbcsms_bc_page_type;

typedef struct
{
  mccbcsms_bc_page_type     bc_type;
  union
  {
    mccbcsms_bc_type        bc;
  } pages;
} mccbcsms_matched_page_type;

typedef struct
{
  /* broadcast SMS enabled? It is set by MC_BCSMS_ENABLE_F from CM */
  boolean           bcsms_enable;

  /* Indicates if broadcast index initialized or not after powerup */
  boolean           bcast_init;

  /* Overhead channel, used when an idle HO between PCH and BCCH */
  caix_chan_type    ovhd_chan;

  /* Broadcast cycle index */
  byte              bcast_index;

  /* broadcast slot cycle in periodic mode */
  word              bcast_slot_cycle;

  /* */
  boolean           cancel_bcast;

} mccbcsms_config_type;


/* Specifc information for broadcast page */
typedef struct
{
  byte num_bc_page; /* Broadcast page number within GPM */
} mccbcsms_bc_page_info_type;

/* Broadcast information */
typedef struct
{
  qword frame;                    /* Frame message received */
  mccbcsms_bc_page_type bc_type;  /* Type of broadcast page */
  byte burst_type;                /* Burst type */
  byte addr_len;                  /* Length of broadcast address */
  byte bc_addr[CAII_BC_ADDR_LEN]; /* Broadcast address */
  union
  {
    mccbcsms_bc_page_info_type     bc; /* Broadcast page info */
  } page;
} mccbcsms_bc_info_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCCIDL_SAVE_BCAST_PAGE

DESCRIPTION
  This procedure stores broadcast page and set broadcast pending flag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void mccidl_save_bcast_page
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to received message */
);


/* <EJECT> */
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
void mccbcsms_init (void);

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
void mccbcsms_reset_bc_pages (void);

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
);

/* <EJECT> */
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
extern void mccbcsms_proc_msg
(
  byte              msg_type,
  cdma_bs_type      *new_bs_config
);


/* <EJECT> */
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
extern boolean mccbcsms_is_bcast_init(void);


/*===========================================================================

FUNCTION MCCBCSMS_IS_BC_PAGE_SLOT

DESCRIPTION
  This function returns whether the given slot is a broadcast page slot. A
  paging slot is a broadcast slot, if it's the first or second slot within
  the current broadcast cycle.
  It takes the slot when the message is received as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it's a broadcast page slot

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mccbcsms_is_bc_page_slot
(
  qword msg_frame_20    /* Msg frame, in 20 ms */
);


/*===========================================================================

FUNCTION MCCBCSMS_IS_BC_DATA_SLOT

DESCRIPTION
  This function returns whether the given slot is a scheduled broadcast
  slot to receive data burst.
  It takes the slot when the message is received as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it's a broadcast data slot

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mccbcsms_is_bc_data_slot
(
  qword msg_frame_20    /* Msg frame, in 20 ms */
);


/* <EJECT> */
/*===========================================================================

FUNCTION MCCBCSMS_SCHEDULE_BCAST

DESCRIPTION
  This function schedules the Broadcast SMS by finding the next Broadcast
  wakeup slot and sending them to the Searcher through sleep command.

  This function is called from mccidl_start_sleep().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void mccbcsms_schedule_bcast(void);


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
extern void mccbcsms_sched_bcast_upon_wakeup
(
  qword cur_time_20ms /* in 20ms */
);

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
extern void mccbcsms_cancel_bcast(void);

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
extern void mccbcsms_post_fraho_proc(void);

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
extern boolean mccbcsms_comp_addr
(
  caii_pc_hdr_type *msg_hdr,
    /* Header of paging channel message to match address of */
  qword msg_frame_20
    /* Msg frame, in 20 ms */
);

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
extern boolean mccbcsms_page_match
(
  mccbcsms_bc_info_type *bc_ptr /* Pointer to broadcast info */
);

/* Variable indicating whether a BCSMS is in progress or not */
extern boolean mcc_bcsms_in_progress;

/* The starting slot of the BCSMS in progress */
extern qword mcc_bcsms_start_slot;



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
boolean mccbcsms_is_enabled( void );

#endif /* MCCBCSMS_H */
