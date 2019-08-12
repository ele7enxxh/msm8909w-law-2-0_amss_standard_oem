#ifndef SRCHMC_V_H
#define SRCHMC_V_H
/*===========================================================================

             S E A R C H   to   M C   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations to be used by the MC task.

  Copyright (c) 2004 - 2013 Qualcomm Technologies, Inc. 
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

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srchmc_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   pk      1x2GTA Feature changes
11/26/14   jh      remove qw.h include
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/07/11   bb      Changes to support quick LTE to 1X Reselection scenarios
01/19/11   jtm     Eliminated dependency on FTM.
11/18/10   jtm     Moved SRCH_CFNSET_MAX here from srchmc_i.h. Removed
                   featurization.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
11/01/10   bb      Changes to support eCSFB feature
09/30/10   bb      Change the cdma_sys_time type to uint64
06/18/10   bb      Change the lte_ostmr value type uint32
03/19/10   pk      Modem Statistics Report Feature - Enable Reporting of
                   Pilots info,Rx pwr and Tx pwr to Call Processing
02/04/10   jtm     Removed FEATURE_PLT.
10/20/09   bb      Added support for "LTE to 1X IRAT Core" feature
09/14/09   bb      Added slew_opt field to the srch_slew_cmd_type structure
                   for slew state otimization feature
08/29/09   bb      Added nghbr_srch_mode field to the srch_nset_cmd_type
08/04/09   mca     Added support for 1x Advanced
04/24/09   adw     Lint fixes.
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Cleaned up incs and eliminated implicit includes.
11/19/08   adw     Merge from main/latest, based on VU_MODEM_1X_SRCH.12.00.24
11/19/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_SRCH.12.00.13
09/22/08   sst     Increase MAX_SYS_MEAS_LIST_SIZ
10/18/06   bn      Added new structure to support timing transfering from
                   DO to 1x
10/12/06   awj     Added featurized pseudo-idle support for UMTS/1x
06/05/06   rkc     Added srch_agc_to_dbm.
02/02/06   awj     Added SRCH_MAX_SETS
09/21/05   pa      Changed value of NO_SYS_MEAS_RESULTS
09/20/05   pa      Added AGC gated ecio measurement enum.
08/29/05   grl     Added header to SRCH_ACCESS_F message.
08/08/05   kwo     Added pgslot field to idle_cmd_type
08/03/05   ejv     Mainline FEATURE_TRANS_MANAGER.
08/02/05   ejv     Removed unnecessary includes.
07/24/05   fh      Renamed SRCH_FIND_STRONGEST_PILOT_F, SRCH_STRONG_PILOT_F
                   and related report names, data types, and data structures
07/13/05   ejv     Remove srch_access_inform.  Added srch_notify_block_gps.
06/24/05   ejv     Remove srch_tune_rfm.
06/01/05   grl     Temporarily added srch_tune_rfm for calls from mccidl.c
05/31/05   rng     Merged in from SHDR Sandbox.
04/12/05   fc/ejv  Added support for DCA.
03/21/05   ejv     Removed runtime Cset sizes.
02/07/05   ejv     Added srch_get_rx_agc from srch.h.
01/24/05   ejv     Added srch_get_pilot_ecio from srch.h.
12/10/04   ejv     Updates to the RelD interface.
12/06/04   kwo     Added SRCH_ACCESS_F and SRCH_FIND_STRONGEST_PILOT_F
11/12/04   ejv     Replaced magic number.  Remove use_sch flag.
11/10/04   ejv     Added srch_tune_rfm and srch_get_rx_div_cap from srch.h.
11/03/04   kwo     Removed obsolete fade and unslew command support
10/15/04   ejv     Initial version.  Moved MC-specific data from srch.h.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Cp */
#include "cai.h"
#include "cai_v.h"
#include "caii_v.h"
#include "db.h"

/* Srch */
#include "srch_rx_t.h"
#include "srchmc.h"

/* Other */
#include "cmd.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Maximum number of Supplemental Channels(SCHs) in IS-2000 */
#define  SRCH_SCH_MAX             CAI_MAX_NUM_FOR_SCH


/*--------------------------------------------------------------------------
                       SEARCH COMMAND STATUS VALUES

The following status codes are for use with the 'status' field of the
search command header type defined below and as the return values for
function srch_cmd.
--------------------------------------------------------------------------*/

typedef enum
{
  SRCH_DONE_S,      /* Function completed successfully */
  SRCH_BUSY_S,      /* Command queued, but not yet completed */
  SRCH_BADPARM_S,   /* Illegal parameter(s) specified for command */
  SRCH_BADSEQ_S,    /* Illegal sequencing of commands */
  SRCH_TMO_S,       /* Command could not be completed in time */
  SRCH_ERR_S,       /* Internal error occurred */
  SRCH_TOOLATE_S,   /* Now is not a good time for that command */
  SRCH_TUNE_S,      /* Command queued, but tuning in progress  */
  SRCH_SLEEP_S,     /* We are asleep. Please come back in the morning */

  SRCH_MAX_S        /* Tells number of status codes. MUST be last item
                       in enum */
}
srch_cmd_status_type;


/*--------------------------------------------------------------------------
              STRUCTURES FOR CHANNEL SPECIFIC INFO
--------------------------------------------------------------------------*/

typedef struct
{
  /* Walsh channel to be used on the traffic channel with the pilot */
  uint16  walsh_chn;

  /* QOF mask ID (0-3), where 0 gives true walsh functions */
  uint8   qof_mask_id;

  /* Indicates if the FCH active on this PN */
  boolean active;
}
/* Data for fundamental channel */
srch_fch_info_type;

typedef struct
{
  /* Walsh channel to be used on the DCCH with the pilot */
  uint16  walsh_chn;

  /* QOF mask ID (0-3), where 0 gives true walsh functions */
  uint8   qof_mask_id;

  /* Indicates if the DCCH active on this PN */
  boolean active;
}
/* Data for dedicated control channel */
srch_dcch_info_type;

typedef struct
{
  /* Number of supplemental channels, can vary from 0 to 7. */
  uint8  num;

  /* Array of Supplemental Walsh channel to be used on the traffic
     channel. Can vary from 0 to 7 */
  uint16 walsh_chn[ SRCH_SUP_CHANNEL_MAX ];
}
/* Data for supplemental code channels */
srch_scch_info_type;

typedef struct
{
  /* Number of SCH channels */
  uint8 num;

  /* Array of SCH walsh codes */
  uint8 walsh[ SRCH_SCH_MAX ];

  /* Array of SCH walsh code lengths */
  uint8 walsh_len[SRCH_SCH_MAX ];

  /* Array of SCH QOF mask id's */
  uint8 qof_mask_id[ SRCH_SCH_MAX ];
}
/* Data for supplemental channels - take 2 */
srch_sch_info_type2;

typedef struct
{
  /* Channel walsh code */
  uint16                   walsh_chn;

  /* Power control sub-channel */
  uint8                    sub_chn;

  /* Rate of CPCCH, always 800Hz for Rel C */
  cai_cpcch_rate_enum_type rate;

  /* Indicates if F-CPCCH is active on this PN */
  boolean                  active;
}
/* Data for common power control channel */
srch_cpcch_info_type;

/* SFM: may get replaced by a CAI #define later */
#define MAX_NUM_PDCCH  2          /* maximum number of PDCCHs per pilot */

typedef struct
{
  /* number of PDCCHs on this pilot (0, 1 or 2) */
  uint8 num;

  /* walsh channels for PDCCHs */
  uint8 walsh_chn[MAX_NUM_PDCCH];

  /* value of PDCH_GROUP_IND received OTA or value of PWR_COMB_IND
     if PDCH_GROUP_IND was not sent */
  /* Revisit - this parameter should be removed when MC updates their
     reference to the new parameter below */
  uint8 pdch_group_ind;

  /* value of the DV group number as calculated by MC */
  uint8 pdch_group_id;
}
/* Data for packet data control channel */
srch_pdcch_info_type;


typedef struct
{
  /* number of GCHs on this pilot (0, 1 or 2) */
  uint8 num;

  /* walsh channels for GCHs */
  uint8 walsh_chn[2];
}
/* Data for grant channel */
srch_gch_info_type;

typedef struct
{
  /* number of RCCHs on this pilot (0 or 1) */
  uint8 num;

  /* walsh channel for RCCH */
  uint8 walsh_chn;

  /* RCCH subchannel index */
  uint8 subchan_index;
}
/* Data for rate control channel */
srch_rcch_info_type;

typedef struct
{
  /* number of ACKCHs on this pilot (0 or 1) */
  uint8 num;

  /* walsh channel for ACKCH */
  uint8 walsh_chn;

  /* ACKCH subchannel index */
  uint8 subchan_index;
}
/* Data for ack channel */
srch_ackch_info_type;


typedef struct
{
  /* Data pertaining to F-PDCCH */
  srch_pdcch_info_type pdcch;

  /* Data pertaining to F-GCH */
  srch_gch_info_type   gch;

  /* Data pertaining to F-RCCH */
  srch_rcch_info_type  rcch;

  /* Data pertaining to F-ACKCH */
  srch_ackch_info_type ackch;
}
/* Super-structure of all DV forward channels */
srch_dv_pilot_info_type;

/*--------------------------------------------------------------------------
           1x TA (TUNE AWAY) REASONS
--------------------------------------------------------------------------*/
typedef enum
{
  SRCH_TA_NONE,
  SRCH_TA_QTA, /* Quick Tune Away */
  SRCH_TA_LTA  /* Long Tune Away */
} srch_ta_rsn_type;
#endif /*SRCHMC_V_H*/
