#ifndef GARB_TABLES_H
#define GARB_TABLES_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                   G L 1  A R B I T R A T O R  T A B L E S

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/gl1_arbitrator_tables.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who       what, where, why
--------   --------  ----------------------------------------------------------
04/011/14  ab        CR646584: GSM Timeline reporting for WLAN COEX
02/07/14   mc        CR.687340 : BOLT 2.0 : COEX : Mass Merge of COEX changes from DI4.0
14/01/14   og        CR589868. T+G DSDA support.
29/11/13   js        CR.574402 : COEX : VBATT : Accurate prediction of N+1 frame Tx power for CS (TCH and SDCCH)
02/10/13   pjr       CR.552448 : DDR Frequency plan implementation - l1_arbitration modification
09/11/13   mc        CR.572746 : COEX : DSDA G+G VBATT power back-offs for TxTx collisions are not correctly applied
11/10/13   pjr       CR.555300 : Changes to correctly register freqid during monitors  and acq
01/10/13   sk        CR552259 CR sync from 1.0/1.1
23/09/13   sk        2.0 syncup from 1.1
15/08/13   sk        CR524039 GBTA bringup changes
29/07/13   sk        CR519663 Partial QBTA bringup changes
25/03/13   mc        CR.464496 : COEX Support - GERAN arbitrator SW CXM integration
13/03/13   mc        CR.459487 : COEX Support - GERAN arbitrator framework
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gl1_arbitrator_interface.h"
#include "gl1_arbitrator_types.h"
#include "gl1_arbitrator_resolve.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
#define LUTX_MAX_IDX       10
#define LUTY_MAX_IDX       20
#define LUTX_MIN_STEP_SIZE 0.414
#define LUTY_BACKOFF_1     0
#define LUTY_BACKOFF_2     1
#define LUTY_STEPSIZE      2
#define LUTY_IDX_INVALID   (LUTY_MAX_IDX - 1)
#define MAX_TXLEV          31
#define MAX_DBM            33 /* 33 dBm */

/*===========================================================================

                        PUBLIC DECLARATIONS

===========================================================================*/
extern const uint8 garb_paging_frame[9];
extern const uint8 garb_ext_paging_frame[9];

extern const garb_channel_t garb_map_gl1_hw_channel_type_lut[];

extern void (*garb_resolve_conflict_lut[GARB_CHAN_MAX][GARB_CHAN_MAX])(uint8 ts, boolean is_rx, uint8 conflict_index, cxm_activity_type* conflict_priority_tier, gas_id_t);

#ifdef FEATURE_GSM_COEX_SW_CXM
extern const cxm_activity_type garb_map_channel_to_cxm_activity_gpg_lut[];
extern const cxm_activity_type garb_map_channel_to_cxm_activity_xpg_lut[];
extern const cxm_activity_type garb_map_channel_to_cxm_activity_tpg_dl_lut[];
extern const cxm_activity_type garb_map_channel_to_cxm_activity_tpg_ul_lut[];

extern const uint8 max_ul_tier_10_per_frame[];
extern const uint8 max_dl_tier_10_per_frame[];
#endif

#ifdef FEATURE_GSM_GPRS_TIMELINE_REPORTING
extern const cxm_slot_prio_e garb_map_channel_to_wlan_priority[];
#endif /* FEATURE_GSM_GPRS_TIMELINE_REPORTING */

/* VBATT : TXLEV to dBm */
const uint16 txlev_to_dBm[SYS_BAND_CELL_850+1][MAX_TXLEV+1];

/* VBATT : P1 (dBm) + P2 (dBm) = MAX(P1, P2) + LUTx[ABS(P1 - P2)] */
const double LUTx[LUTX_MAX_IDX];

/* VBATT : Proportion SUB1/SUB2 backoffs */
const double LUTy[LUTY_MAX_IDX+1][2];

/* VBATT : p1p2_dbm_to_backoff_index[] - if either P1 or P2 >= 25dBm total budgeted power may require distributing between SUBs according to priority */
const uint8 p1p2_dbm_to_backoff_index[MAX_DBM-25+1][MAX_DBM-25+1];

#endif /*GARB_TABLES_H*/
