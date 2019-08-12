#ifndef SRCH_GENLOG_H
#define SRCH_GENLOG_H
/*===========================================================================

                  S R C H L O G    H E A D E R    F I L E

DESCRIPTION
  This module contains functions pertaining to generalized
  searcher log packets

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_genlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   pk      1x2GTA Feature changes
05/04/15   pap     Adding log packets for IDLE DIV stats
04/09/15   sst     Add new PCH MER Log packet
11/30/12   vks     Add asdiv log support
02/01/12   mca     Added srch_genlog_rf_report() support
11/04/11   mca     Added TSYNC support
05/10/11   jtm     Remove include for srch_afc_t.h.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
02/18/10   bb      Added code changes to support QOF functionality
09/18/09   adw     Mainlined FEATURE_ACQ2 to support dynamic memory allocation.
04/23/09   tjc     Added power control logging subpacket
04/01/09   adw     Categorized included header files and standardized format.
03/31/09   adw     Eliminated implicit includes.
12/15/08   vks     Created version 2 of RDDS log packet
10/06/08   vks     Reverted previous change
09/08/08   vks     Updated srch_genlog_rf_sub_v1 to include
                   FEATURE_SRCH_SINGLE_ANTENNA
08/21/08   vks     Added FEATURE_SRCH_HAS_NO_INTERNAL_LOG_PACKETS to remove
                   internal log packets
04/12/07   sst     Added support for QLIC logging
02/13/07   pa      Fixed typo in variable name.
11/10/06   tjc     Update RX diversity autoswitching log packet
11/08/06   tjc     Created RX diversity autoswitching log packet
05/01/06   tjc     Stripped FEATURE_SEARCH1 and mainlined FEATURE_SRCH_PC2
03/24/06   kwo     Renamed frame ser to frame sec
03/22/06   kwo     Added srch_idle_util_build_demod_stats_sub
01/30/05   pa/kwo  Changed timeline profile subpacket to version 2
01/16/06   awj     Moved to srch_stm interface
09/22/05   sst     Update Acq2.0 log subpacket to version 2
07/15/05   bt      Added profiling for ZZ2_1 timeline.
07/14/05   sfm     Mainlined FEATURE_1X_SRCH_USES_ROTATOR_AFC
06/20/05   ejv     Added srch_genlog_tc_set_maint_sub_v1 and
                   srch_genlog_tc_set_maint.
05/19/05   ejv     Moved SRCH_LOG_TNG_TC_STATS_C from 0x11A3 to 0x11A9.
05/13/05   ejv     Replaced srch_genlog_sub_id_type with uint8.
05/04/05   sst     File re-organization
05/02/05   ejv     Added set maint log packet and sub-packet IDs.
04/01/05   kwo     Added SRCH_LOG_NS_SRCH_C
04/01/05   sfm     Added srch_genlog_triage() and related definitions
03/31/05   sfm     Added timeline profiling.
03/31/05   sst     Updated comment for position in srch1_results to Cx2
03/22/05   sst     Removed references to srch4drv_t.h, changed reference to
                   srch4_srch_type to uint32 in logging functions
03/11/05   kwo     Modified field names for radio config in srch status sub
03/10/05   ddh     Added srch4drv_t.h include and changed variable type in
                   srch_genlog_1x_srch4_dump
11/29/04   sst     Added local define acq log pkt buffer size
10/29/04   ejv     Added srch_genlog_tc_stats and IDs for TC call stats
                   sub-packet and TC stats log code.  Added TC call stats
                   struct srch_genlog_tc_call_stats_sub_v1 from srchtc.c.
10/18/04   kwo     Updated sleep stats packet to include RF band and chan
10/15/04   bt      Added FEATURE_ZZ2_1
10/13/04   dmw     Added support for QPCH genlog sub-packet.
10/08/04   sst     Modified Acq log packet
10/07/04   kwo     Renamed gen log packets to SRCH_LOG_TNG...
10/06/04   kwo     Added support for finger channel estimate packet
10/06/04   jcm     Added sleep stats subpacket
10/01/04   sst     Added support for Acq2 logging
09/23/04   kwo     Added generalized demod status subpacket
09/16/04   jcm     Added generalized AFC subpacket
09/08/04   jcm     Rename exported genlog types to include srch_
09/08/04   jcm     Added generalized srch1 subpacket
09/03/04   jcm     Added generalized srch4 subpacket
09/03/04   jcm     Modified genlog to log multiple packet simultaneously
08/27/04   dmw     Added structs for active and finger info
08/17/04   kwo     Updated fields in rf subpacket (again)
08/09/04   kwo     Update fields for rf subpacket
08/03/04   kwo     Initial revision.

===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"

/* Srch */
#include "srch_hw_t.h"
#include "srchmc.h"

/* Other */
#include "srch4trans_t.h"
#include "stm.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Log Code IDs
-------------------------------------------------------------------------*/
/* Searcher has log codes in the range
   LOG_SEARCHER_RESERVED_CODES_BASE_C (0x119A) - LOG_SEARCHER_LAST_2_C (0x11AF)
   (0x18C9) - 0x18CA
   The log codes are generic to the diag interface and their usage is defined
   here by searcher.  Any of those 22 codes not defined here or in genlog.h
   are considered unused. Internal log codes should be taken starting from the
   last and working toward the first. */

#define SRCH_LOG_TNG_FINGER_STATUS_C          (0x119A)
#define SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)
#define SRCH_LOG_TNG_GENERAL_STATUS_C         (0x119C)
#define SRCH_LOG_TNG_SRCH_DEMOD_INFO_C        (0x119D)
#define SRCH_LOG_TNG_SLEEP_STATS_C            (0x119E)
#define SRCH_LOG_TNG_QPCH_C                   (0x119F)
#define SRCH_LOG_TNG_AFC_INFO_C               (0x11A0)
#define SRCH_LOG_TNG_ACQ_INFO_C               (0x11A1)
#define SRCH_LOG_TNG_FING_CH_EST_C            (0x11A2)
#define SRCH_LOG_TNG_QLIC_INFO_C              (0x11A3)
#define SRCH_LOG_TNG_TSYNC_C                  (0x11A4)
#define SRCH_LOG_TNG_RF_REPORT_C              (0x11A5)
#define SRCH_LOG_TNG_ASDIV_C                  (0x11A6)
#define SRCH_LOG_TNG_PCH_MER                  (0x18C9)
#define SRCH_LOG_IDLE_DIV_STATS_C             (0x18CC)
#define SRCH_LOG_TRAFFIC_TA_STATS_C           (0x18CD) //REVISIT


/* internal searcher log codes */
#define SRCH_LOG_TNG_RX_DIV_AUTO_C            (0x11A7)
#define SRCH_LOG_TNG_TC_STATS_C               (0x11A9)
#define SRCH_LOG_TNG_TC_SET_MAINT_C           (0x11AA)
#define SRCH_LOG_TNG_ZZ_PROF_C                (0x11AB)
#define SRCH_LOG_TNG_TRIAGE_C                 (0x11AC)
#define SRCH_LOG_NS_SRCH_C                    (0x11AD)
#define SRCH_LOG_SECT_DUMP_C                  (0x11AE)
#define SRCH_LOG_FING_DRIVER_C                (0x11AF)

/*-------------------------------------------------------------------------
      Sub-packet IDs
-------------------------------------------------------------------------*/
enum
{
  SRCH_SUB_ID_GENERAL_STATUS  = 1,
  SRCH_SUB_ID_RF,
  SRCH_SUB_ID_ACTIVE_INFO,
  SRCH_SUB_ID_FINGER_INFO,
  SRCH_SUB_ID_FILT_PILOT_SYMBOL,
  SRCH_SUB_ID_SEARCHER_1,
  SRCH_SUB_ID_SEARCHER_4,
  SRCH_SUB_ID_SLEEP_STATS,
  SRCH_SUB_ID_QPCH,
  SRCH_SUB_ID_AFC,
  SRCH_SUB_ID_TC_DEMOD_STATUS,
  SRCH_SUB_ID_CH_EST,
  SRCH_SUB_ID_ACQ,
  SRCH_SUB_ID_TC_CALL_STATS,
  SRCH_SUB_ID_ZZ_PROF,
  SRCH_SUB_ID_TRIAGE,
  SRCH_SUB_ID_TC_SET_MAINT,
  SRCH_SUB_ID_IDLE_DEMOD_STATUS,
  SRCH_SUB_ID_IDLE_SRCH_FILTER,
  SRCH_SUB_ID_TC_SRCH_FILTER,
  SRCH_SUB_ID_RX_DIV_AUTO,
  SRCH_SUB_ID_QLIC_INFO,
  SRCH_SUB_ID_TC_POWER_CTL,
  SRCH_SUB_ID_TSYNC,
  SRCH_SUB_ID_ASDIV,
  SRCH_SUB_ID_PCH_MER,
  SRCH_SUB_ID_IDLE_DIV_STATS,
  SRCH_SUB_ID_TRAFFIC_TA_STATS,
};

typedef enum
{
  SRCH_PKT_ID_FINGER_STATUS  = 0,
  SRCH_PKT_ID_1X_SRCH1_DUMP,
  SRCH_PKT_ID_1X_SRCH4_DUMP,
  SRCH_PKT_ID_GENERAL_STATUS,
  SRCH_PKT_ID_SRCH_DEMOD_INFO,
  SRCH_PKT_ID_SLEEP_STATS,
  SRCH_PKT_ID_QPCH,
  SRCH_PKT_ID_AFC,
  SRCH_PKT_ID_FING_CH_EST,
  SRCH_PKT_ID_ACQ,
  SRCH_PKT_ID_TC_STATS,
  SRCH_PKT_ID_ZZ_PROF,
  SRCH_PKT_ID_TRIAGE,
  SRCH_PKT_ID_TC_SET_MAINT,
  SRCH_PKT_ID_RX_DIV_AUTO,
  SRCH_PKT_ID_QLIC_INFO,
  SRCH_PKT_ID_TSYNC,
  SRCH_PKT_ID_RF_REPORT,
  SRCH_PKT_ID_ASDIV,
  SRCH_PKT_ID_PCH_MER,
  SRCH_PKT_ID_IDLE_DIV_STATS,
  SRCH_PKT_ID_TRAFFIC_TA_STATS,

  SRCH_NUM_GENLOG_PACKETS

} srch_genlog_packet_id_type;


/*-------------------------------------------------------------------------
      General Header Data Types
-------------------------------------------------------------------------*/
typedef struct
{
  uint8                    id;         /* sub packet id */
  uint8                    ver;        /* sub packet version */
  uint16                   size;       /* sub packet size */

} srch_genlog_sub_header_type;

/*-------------------------------------------------------------------------
      Variable Types
-------------------------------------------------------------------------*/

typedef void (*sub_commit_func_type)
(
 srch_genlog_packet_id_type   id,
 srch_genlog_sub_header_type *sub
);


/*-------------------------------------------------------------------------
      Types for External Log packets
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)                    */
/*-------------------------------------------------------------------------*/
typedef struct
{
  uint32   peak_pos;                   /* peak position in Cx16 resolution */
  uint16   peak_eng;                   /* peak energy */
  uint16   reserved;

} srch_genlog_srch4_result_type;

typedef struct
{
  uint16   pilot_ant_burst_len_qof;    /* pilot, antenna, burst length, QOF */
  uint16   coh_trunc_coh_len;          /* coherent truncation and length */
  uint8    noncoh_len;                 /* non-coherent length */
  uint8    phase;                      /* phase */
  uint16   walsh_index;                /* walsh index */
  uint16   win_start;                  /* window start position in Chips */
  uint16   win_size;                   /* window size position in Chips */
  uint32   pilot_set_mask;             /* pilot set mask */
  srch_genlog_srch4_result_type   results[ SRCH_MAX_PEAKS ];

} srch_genlog_srch4_task_type;

typedef struct
{
  srch_genlog_sub_header_type   hdr;   /* subpacket header */
  uint8    tech;                       /* srch4 technology */
  uint8    queue;                      /* srch4 queue used */
  uint8    priority;                   /* search priority */
  uint8    qpch_cntl;                  /* QPCH control */
  int16    freq_offset;                /* frequency offset */
  uint8    num_tasks;                  /* number of tasks programmed */
  uint8    num_peaks;                  /* number of peaks programmed */
  srch_genlog_srch4_task_type   tasks[ SRCH4_MAX_NUM_TASKS ];

} srch_genlog_srch4_sub_v1;


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Exported  Functions
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*       SRCH_LOG_TNG_1X_SEARCHER_DUMP_C       (0x119B)                    */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRCH_GENLOG_1X_SRCH4_DUMP

DESCRIPTION
  This function builds a 1x Searcher Dump packet for searcher 4.  This packet
  shall include the rf subpacket and the searcher4 subpacket plus any
  additional subpacket included by the srch4 driver.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void srch_genlog_1x_srch4_dump
(
  uint32 queue,                /* queue to log */
  uint32 num_tasks             /* number of tasks to log */
);

#endif /* SRCH_GENLOG_H */
