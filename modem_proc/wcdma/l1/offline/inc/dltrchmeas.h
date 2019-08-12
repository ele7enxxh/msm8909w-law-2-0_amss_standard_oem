#ifndef DL_TRCH_MEAS_H
#define DL_TRCH_MEAS_H

/*============================================================================
                           D L _ D E C . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dltrchmeas.h_v   1.13   01 May 2002 13:18:22   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dltrchmeas.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/16/15    ap      Initial check-in for FR 30856
02/24/15    ap      Initial check-in for WL1 QSH framework 
11/21/14    sl      Changes to freeze TxAccum on Secondary until Primary comes back in sync after LTA
10/23/14    sl      Changes for 0x0423 Log Packet version 7
06/30/14    pr      Dynamic adjustment of Q values during FDPCH add/reconfig scenarios 
                    and HS repointing cases.
06/06/14    pr      Enhancement for x4023 log pkt v6
05/30/14    vr      Changes to skip SYNC-A only if SYNC-AA is completed, using new API.
05/26/14    nd      Added support for LTA with DC-HSUPA
03/18/14    mit     F3 cleanup, enabling via NV's
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
12/04/13    kcm     Changes to Qin phase2 and Qout thresholds for E-FDPCH
01/06/14    vn      Added bler reporting to support Field Test Mode Feature.
06/27/13    sad      Added support for secondary Tx RL failure/Phychan estb. failure
06/12/13    pr      Force set the CLTD init Counter flag for first 10 frames.
05/21/13    sad     Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
09/06/12    sr      New log pkt 0x4198 WCDMA_ULTPC_DEBUG_PKT
04/01/13    hk      Mainlined FEATURE_WCDMA_EFDPCH
02/20/13    hk      Removed FEATURE_INTER_SIB_SLEEP
03/30/12    pr      Modified the QIN_PH2 to -3dB and QOUT to -6.5dB 
12/12/12    pr      Workaround to ensure that we do not declare Out of Sync for 40 frames 
                    after we detect a dip in RxAGC of more than 18dBm
12/11/12    vn      Enabled reporting SIR PATHLOSS from l1_get_rssi().
10/18/12    pr      Mainlined FEATURE_UMTS_SIR_PATHLOSS.
08/14/12    kcm     Changing the Phase 1 filter value to -3dB for first 3 
                    frames.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
05/23/12    kcm     L/G to W handover optimizations
02/10/12    rgn     Nikel feature cleanup
01/31/12    gnk     Nikel feature cleanup
01/26/11    vs      Nikel feature cleanup. Mainlined the features FEATURE_WCDMA_L1_BCH_HIGH_BLER_OOS
                    & FEATURE_L1_LOGGING_ENABLED
11/06/11    gnk     Removed some unused features
11/06/11    sv      Added extern for dl_trch_meas_get_sync_state API.
10/11/11    ks      Changed Qin(-2dB -> -2.5dB) & Qout(-5dB -> -5.5dB)
06/23/11    dm      Added enhanced power control log packet(0x4023) support
04/28/11    sj      upmerge from offline/main
04/01/11    ks      Added boolean to indicate if spill energy is used
04/01/11    ks      Added structure member to store partial frame energy in sync
                    detect structure
03/14/11    ms      Mainlined FEATURE_6275_COMPILE
09/02/10    gv      Code Checkin for CPC DTX
05/03/10    hk      Fixed compiler warnings
04/06/10    ks      Added Enhanced FDPCH support
09/30/09    hk      Replaced INTLOCK with Mutex
10/03/08    hk      Bringing in 7k Mailine fixes
02/21/07    ms      Added F-DPCH logging support.
12/22/06    ksr     Rel-6 post-verification, skip sync A changes
10/17/06    ms      Added support for FDPCH.
04/11/06    scm     Add BLER stats for inter-SIB sleep that don't reset.
09/12/05    gs      Added support for Auto bler meas and removed support for 
                    event and periodic bler measurements.
08/23/05    vb      removed cltd timer variable to the dl sync det struct
08/22/05    vb      Added cltd timer variable to the dl sync det struct
07/06/05    vp      Code changes to support new DL sync parms received in a 
                    CPHY_UPD_DL_SYNC_PARMS message. RRC sends this message
                    when it has received a UTRAN Mobility Information message.
05/20/05    vb      Defined MAX_UL_DPCCH_CONFIG_DELAY from 2 to 3
12/02/04    vb      Added constant and variable to track number of UL DCCH
                    frames before updating DL DPCCH pilot amp related parms.
11/04/04    vb      Code changes for flushing the periodic logpakts to diag
                    after tearing down the DPCH
02/11/04    gs      Added macros for Out of service area declaration for high
                    BCH BLER in BCH state
11/12/03    m       DL OLPC related code change to cleanup existing
                    implementation
09/24/03    m       Fixed delayed PA on problem
08/03/03    gs      Externalized function dl_set_ul_in_sync_tx_enable_flag
07/31/03    m       Removes deprecated OLPC floor declaration
07/23/03    m       externalize dl_olpc_target_ebnt to be used to print the
                    value when wind-up happens
07/17/03    m       Here the  Init period was  extended from 20  to 50
                    frames to  mask the problem caused  by TFCI decode
                    errors just  after INIT period ended.  May need to
                    be reverted once the root cause is identified.
05/30/03    m       Added TRK mode improvements related code
05/08/03    m       Added new down step size to be used in REACQ_MODE for OLPC
05/01/03    m       ACQ mode step size reduced to 0.125db from 0.5dB
04/17/03    m       Defines few enumeration used in SIR optimization procedure
02/27/03    gs      Added members to bler stats for CM BLER logging
                    Changed MAX SIR target to unsigned 16 bit MAX.
                    Externalized function dl_trch_cm_bler_info_update
02/26/03    m       Increased Init mode period for OLPC from 80msec to 200msec
01/28/03    m       Linted code
11/14/02    m       Implemented OLPC and windup algorithms
05/01/02    gs      1. Changed the value of DL_OLPC_TGT_EBNT_N_VAL_INIT_Q12
                       corresponding to 9 dB init value.
04/26/02    gs      1. Removed macro for target EbNt max. Added macros for
                       log2Mval min and max value.
04/15/02    gs      1. Changed the outer loop initial set point to 9 dB
02/04/02    gs      1. Added the feature FEATURE_L1_DL_OLSP_FLOOR for flooring
                       the outer loop set point.
01/25/02    gs      1. Added the macro DL_OLPC_TGT_EBNT_MAX_VAL
                    2. Backed out the change to floor the OLPC set point to 0 dB
12/18/01    gs      1. Set the OLPC min setpoint to 0 dB
12/03/01    gs      1. Added the structure element for DL OLPC stats in
                       structure dl_olpc_trch_vars_struct_type for feature
                       FEATURE_DL_OLPC_STATS.
11/29/01    gs      1. Externalized the function dl_manual_set_olpc_target_val.
11/13/01    gs      1. Changed the BLER qual target MAX to 0
10/15/01    gs      1. Added the structure member variable for RRC timer T312
                       and T313 to dl_sync_det_car_struct_type.
09/18/01    gs      1. Updated the QIN and QOUT value for in-sync and
                       out-of-sync indication
09/07/01    gs      1. Added the parameter reporting time interval in function
                       dl_bler_periodic_report so that it can be logged.
08/14/01    gs      1. Added the macro definition, structure definition and
                       function declaration for the outer loop power control.
                    2. Updated the BLER measurement setup structure type.
                    3. Added the bler measurement Id in BLER setup strcuture.
                    
07/17/01    gs      Created this file.
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "l1def.h"
#include "l1dlcctrchcfg.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* BLER measurement related defines */
/* -------------------------------- */

#define L1_MAX_BLER_INFO_SIZE 512

/* DCH In-sync/out-of sync related defines */
/* --------------------------------------- */

/* Qin and Qout thresholds.  In log2_x100 */
#define Q_IN_PH1  -33  /* -1 dB */
#define Q_IN_PH2  -99  /* -3.0 dB */
#define Q_OUT     -216 /* -6.5 dB */
#define Q_IN_PH1_INIT_VAL -99 /*-3 dB*/

#define Q_OUT_EFDPCH -165   /* -5 dB */
#define Q_IN_PH2_EFDPCH -66  /* -2 dB */

#define SIR_NOT_VALID (0xFF)
#define SIR_MIN_VAL (-10)
#define SIR_MAX_VAL (20)

/* x dB = 10 log y => y = 10 ^ (x / 10)
 * -10 dB = (-10/10) * (log10/log2) * 10
*  -6dB = (-6/10) * (log10/log2) * 10 = -199.3
 */
#define SIR_THRESH_FOR_ULTPC_FREEZE -199 /* -6dB */

/* Macro required for energy filtering */
#define DPCH_SAMPLE_DUR_MS 10 /* Duration of each sample */

#define DPCCH_ENGY_FILTER_MS_PH1 40
#define DPCCH_ENGY_FILTER_MS_PH2 160

#define DPCCH_ENG_FILTER_LEN_PH1 (DPCCH_ENGY_FILTER_MS_PH1/DPCH_SAMPLE_DUR_MS)
#define DPCCH_ENG_FILTER_LEN_PH2 (DPCCH_ENGY_FILTER_MS_PH2/DPCH_SAMPLE_DUR_MS)

#define DPCCH_ENERGY_LOWER_BOUND -330 /*-10dB*/

#define DPCH_SYNC_MAX_CRC_ERR_TB 20
#define DPCH_SYNC_MAX_CRC_ERR_TIME 160

#define DPCH_SYNC_MAX_CRC_ERR_FRAME (DPCH_SYNC_MAX_CRC_ERR_TIME/DPCH_SAMPLE_DUR_MS)

#define DPCH_SYNC_PH1_DUR_UNIT_FR 16

/* Maximum number of frames that UL DPCCH setup takes so that f/w starts updating 
the DL DPCCH pilot Amp related params efgh */
#define MAX_UL_DPCCH_CONFIG_DELAY 3

/* Out of servive area declaration for high BCH BLER in BCH state */
/* -------------------------------------------------------------- */

/* Minimum BCH block errors in a given accumulation */
#define DL_BCH_HIGH_BLER_OOS_MIN_ERR_COUNT    25
/* Minimum percentage BLER on BCH TrCh */
#define DL_BCH_HIGH_BLER_OOS_MIN_BLER_PERCENT 50

 /* Part of FDPCH SIRE (1 / SF) can be calculated as following
    = log2(1 / SF)
    = log2(1 / 256) = -8
  */
 #define FDPCH_SIR_ESTIMATE_LOG2_1_DIV_SF -8

 /*Minimum frames to set CLTD Skip counter to default TRUE in FW*/
 #define DL_CLTD_SET_FLAG_TRUE_COUNT 10

 /*Max counter value to maintain DPCH Q values during init FDPCH*/
 #define DL_FDPCH_REPOINT_QVALUE_MAX_COUNTER 200


#define WL1_DL_SYNC_MSG(str, a, b, c) \
if (dl_sync_debug_msg_enable) \
{ \
  MSG_HIGH(str, a, b, c); \
}\
else \
{ \
  MSG_LOW(str, a, b, c); \
}

#define WL1_DL_SYNC_MSG_4(str, a, b, c, d) \
if (dl_sync_debug_msg_enable) \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH, str, a, b, c, d);\
}\
else \
{ \
  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_LOW, str, a, b, c, d);\
}

/* put the new DPCCH energy sample into the sample array  Add the new DPCCH energy sample to the accumulation 
Adjust the data array head pointer, wrapping to 0 if necessary 
*/
// kcm to put f3s for sec carr (include carr idx in the f3s)
#define DL_TRCH_FILTER_ENERGY_FOR_PH1(carr_idx,temp_dpcch_eng) \
  do { \
  WL1_DL_SYNC_MSG("sync_detect: PH1 accum %d, head_eng: %d, dpcch_eng: %d",                     \
                               dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph1,                                     \
                               dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph1[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph1],  \
                               temp_dpcch_eng);                                                              \
  \
  dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph1 -=                                                                  \
    dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph1[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph1];                             \
  \
  dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph1[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph1] = temp_dpcch_eng;             \
  \
  dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph1 += temp_dpcch_eng;                                                  \
  \
  if (++dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph1 == DPCCH_ENG_FILTER_LEN_PH1)                               \
    dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph1 = 0;                                                           \
  } while(0)

#define DL_TRCH_FILTER_ENERGY_FOR_PH2(carr_idx,temp_dpcch_eng) \
  do { \
    WL1_DL_SYNC_MSG("sync_detect: PH2 accum %d, head_eng: %d, dpcch_eng: %d",                               \
    						  dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph2,                                          \
    						  dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph2[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph2],   \
    						  temp_dpcch_eng);                                                               \
    dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph2 -=                                                                 \
      dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph2[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph2];                          \
    \
    dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph2[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph2] = temp_dpcch_eng;          \
    \
    dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph2 += temp_dpcch_eng;                                                \
    \
    if (++dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph2 == DPCCH_ENG_FILTER_LEN_PH2)                              \
      dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph2 = 0;                                                         \
  } while(0)

#define DL_TRCH_POST_PROCESS_ENERGY_PH1_FOR_FPDCH_WITH_CPC(carr_idx) \
  do { \
      dl_dch_sync_vars.per_carrier_info[carr_idx].spill_dpcch_eng =     \
       ((dl_dch_sync_vars.per_carrier_info[carr_idx].slot_level_accum_eng) - ((dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph1[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph1])*dl_dch_sync_vars.per_carrier_info[carr_idx].num_valid_slots))/SLOTS_PER_FRAME;  \
      dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph1 += dl_dch_sync_vars.per_carrier_info[carr_idx].spill_dpcch_eng;   \
      dl_dch_sync_vars.per_carrier_info[carr_idx].is_spill_eng_used  = TRUE;   \
      WL1_DL_SYNC_MSG("sync_detect: PH1 partial frame's num slots %d accum energy: %d",dl_dch_sync_vars.per_carrier_info[carr_idx].num_valid_slots, dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph1, 0);   \
  } while(0)

#define DL_TRCH_POST_PROCESS_ENERGY_PH2_FOR_FPDCH_WITH_CPC(carr_idx) \
  do { \
    dl_dch_sync_vars.per_carrier_info[carr_idx].spill_dpcch_eng =        \
      ((dl_dch_sync_vars.per_carrier_info[carr_idx].slot_level_accum_eng) - ((dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_data_ph2[dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_filter_head_ph2])*dl_dch_sync_vars.per_carrier_info[carr_idx].num_valid_slots))/SLOTS_PER_FRAME;  \
    dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph2 += dl_dch_sync_vars.per_carrier_info[carr_idx].spill_dpcch_eng ;   \
    dl_dch_sync_vars.per_carrier_info[carr_idx].is_spill_eng_used = TRUE; \
    WL1_DL_SYNC_MSG("sync_detect: PH2 partial frame's num slots: %d accum energy: %d",dl_dch_sync_vars.per_carrier_info[carr_idx].num_valid_slots, dl_dch_sync_vars.per_carrier_info[carr_idx].dpcch_eng_accum_ph2, 0); \
  } while(0)

  /* update the TB count with CRC and the errored TB blks. Adjust the data array head pointer, wrapping to 0 if necessary 
   If all the TB received are with the CRC error then update the number of TB received consectively with CRC error.
  */
#define DL_TRCH_UPDATE_CRC_ERR_CNT(nblks,nerr) \
  do {  \
    dl_dch_sync_vars.num_tb_rece_with_crc_accum -=   \
      dl_dch_sync_vars.num_tb_with_crc[dl_dch_sync_vars.tb_with_crc_cnt_head];  \
    dl_dch_sync_vars.num_tb_with_crc[dl_dch_sync_vars.tb_with_crc_cnt_head] = nblks;  \
    dl_dch_sync_vars.num_tb_rece_with_crc_accum += nblks;  \
    \
    dl_dch_sync_vars.num_tb_rece_with_crc_err_accum -=  \
    dl_dch_sync_vars.num_tb_with_crc_err[dl_dch_sync_vars.tb_with_crc_cnt_head];  \
    dl_dch_sync_vars.num_tb_with_crc_err[dl_dch_sync_vars.tb_with_crc_cnt_head] = nerr;  \
    dl_dch_sync_vars.num_tb_rece_with_crc_err_accum += nerr;  \
    \
    if (++dl_dch_sync_vars.tb_with_crc_cnt_head == DPCH_SYNC_MAX_CRC_ERR_FRAME)  \
      dl_dch_sync_vars.tb_with_crc_cnt_head = 0;                                 \
    \
    if ((nblks > 0) && (nerr == nblks))   \
    {  \
      dl_dch_sync_vars.consec_bad_trblk_cnt += nerr;  \
    } \
    WL1_DL_SYNC_MSG("LLSYNC2:CRC accum %u %u consec %u",  \
               dl_dch_sync_vars.num_tb_rece_with_crc_accum,        \
               dl_dch_sync_vars.num_tb_rece_with_crc_err_accum,    \
               dl_dch_sync_vars.consec_bad_trblk_cnt);     \
  } while(0)

/* Check for in sync CRC criteria.
 If no TB are received then check in the last 160 ms. If any TB received in last 160 ms is with CRC 
 pass or there is no TB received in last 160ms then this criteria is fullfilled. If at least one block with CRC attached 
 and CRc pass is received then this criteria is fullfilled. Check for out of sync CRC criteria. If all the last 20 blocks 
 received have CRC failed and TB received in last 160 ms (16 radio frame) are with CRC fail
 */	
#define DL_TRCH_GENERATE_CRC_SYNC_SIG(nblks,nerr,crc_in_sync_sig,crc_out_of_sync_sig)  \
  do {  \
    if ((nblks == 0) &&                                                    \
    	(((dl_dch_sync_vars.num_tb_rece_with_crc_accum -                    \
    	   dl_dch_sync_vars.num_tb_rece_with_crc_err_accum) > 0) ||          \
    	 (dl_dch_sync_vars.num_tb_rece_with_crc_accum == 0)))                \
    {                                                                         \
      crc_in_sync_sig = TRUE;                                                 \
    }                                                                          \
    else if ((nblks > 0) && (nerr < nblks))                                    \
    {                                                                           \
      dl_dch_sync_vars.consec_bad_trblk_cnt = 0;                                 \
      crc_in_sync_sig = TRUE;                                                    \
    }                                                                            \
    if ((dl_dch_sync_vars.consec_bad_trblk_cnt >= DPCH_SYNC_MAX_CRC_ERR_TB) &&  \
    	(dl_dch_sync_vars.num_tb_rece_with_crc_accum > 0) &&                    \
    	(dl_dch_sync_vars.num_tb_rece_with_crc_accum ==                         \
    	 dl_dch_sync_vars.num_tb_rece_with_crc_err_accum) &&                  \
    	 (in_sync_forty_frames == FALSE))                     \
    {                                                                           \
      crc_out_of_sync_sig = TRUE;                                               \
    }                                                                              \
    if(in_sync_forty_frames == TRUE)                           \
    { \
      crc_in_sync_sig = TRUE;                                                          \
    }                                                                            \
  } while(0)

#define DL_TRCH_UPDATE_SYNC_CNT(carr_idx,in_sync_ind,out_sync_ind)            \
  do {                                                                \
    if (in_sync_ind && out_sync_ind)                                   \
    {                                                                  \
      MSG_ERROR("LLSYNC:Both in sync and out of sync ind", 0, 0, 0);   \
    }                                                                  \
    else if (!(in_sync_ind || out_sync_ind))                           \
    {                                                                   \
      WL1_DL_SYNC_MSG("LLSYNC:no indication", 0, 0, 0);                 \
    }                                                                   \
    else                                                                \
    {                                                                   \
      if (in_sync_ind)                           \
      {                                                                 \
        WL1_DL_SYNC_MSG("LLSYNC:in sync indication", 0, 0, 0);          \
        if ((dl_dch_sync_vars.per_carrier_info[carr_idx].in_sync_cnt <= dl_dch_sync_vars.N312) ||  \
           (dl_dch_sync_vars.per_carrier_info[carr_idx].in_sync_cnt <= dl_dch_sync_vars.N315))     \
        {                                                               \
          dl_dch_sync_vars.per_carrier_info[carr_idx].in_sync_cnt++;                               \
        }                                                               \
        dl_dch_sync_vars.per_carrier_info[carr_idx].out_sync_cnt = 0;                              \
      }                                                                 \
      else                                                              \
      {                                                                 \
        WL1_DL_SYNC_MSG("LLSYNC:out of sync indication", 0, 0, 0);      \
        dl_dch_sync_vars.per_carrier_info[carr_idx].in_sync_cnt = 0;                               \
        if (dl_dch_sync_vars.per_carrier_info[carr_idx].out_sync_cnt <= dl_dch_sync_vars.N313)     \
        {                                                               \
          dl_dch_sync_vars.per_carrier_info[carr_idx].out_sync_cnt++;                              \
        }                                                               \
      }                                                                 \
    }                                                                   \
  }while(0)


  #define SET_RTPC_DECISION_NONE(bmsk, slot) (bmsk) = (((uint32)(bmsk)) | ((uint32) (0<<(2*slot))))
  #define SET_RTPC_DECISION_DSDX(bmsk, slot) (bmsk) = (((uint32)(bmsk)) | ((uint32) (1<<(2*slot))))
  #define SET_RTPC_DECISION_UNDECIDED(bmsk, slot) (bmsk) = (((uint32)(bmsk)) | ((uint32) (2<<(2*slot))))
  #define SET_RTPC_DECISION_REJECTED(bmsk, slot) (bmsk) = (((uint32)(bmsk)) | ((uint32) (3<<(2*slot))))
  #define GET_RTPC_DECISION_REJECTED(bmsk, slot) (((WL1_GET_BIT_U32(bmsk, (2 * slot)) == 0) && (WL1_GET_BIT_U32(bmsk, ((2 * slot) + 1)) == 0)) ? 1: 0)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* BLER measurement related type definition */
/* ---------------------------------------- */

/* This enum defines the BLER measurement criteria. */
typedef enum
{
  DL_BLER_NO_REPORT,           /* No reporting required, it is disabled */
  DL_BLER_REPORT_AUTO_PERIODIC_MEAS /* Periodic meas. criteria based on time */
} dl_bler_report_type_enum_type;

/* BLER measurement SETUP structure.
   This structure defines the parameter for given set of TrCh Ids.
   If the measurement criteria is Quality measurement then the
   quality measurement parameters are also given. Other reporting
   criteria does need parameters */
typedef struct
{
  /* measurement criteria.
     If measurement is to be disabled send DL_BLER_NO_REPORT */
  dl_bler_report_type_enum_type meas_criteria;

  /* number of TrChs that follow above critera for BLER reporting */
  uint8 num_trch_id;
  /* Trch list */
  uint8 trch_id_list[L1_MAX_TRCH_ID];
} l1_dl_bler_meas_setup_req_struct_type;

/* BLER measurement parameter structure.
   It defines the measurement criteria. If the criteria is quality measurement
   then it defines its parameters too. Other criteria doesn't have parameters */
typedef struct
{
  /* BLER reporting criteria */
  dl_bler_report_type_enum_type report_type;
} dl_bler_report_param_struct_type;

/* BLER measurement stats structure */
typedef struct
{
  /* CRC result received */
  uint32 dl_bler_num_tb_w_crc_accum;
  /* CRC result with CRC failed received */
  uint32 dl_bler_num_tb_w_crc_err_accum;
  /* CRC result received */
  uint32 tb_w_crc_accum_cm;
  /* CRC result with CRC failed received */
  uint32 tb_w_crc_err_accum_cm;
} dl_bler_state_struct_type;

/* DCH in-sync/out-of-sync detection related type definition */
/* --------------------------------------------------------- */

/*--------------------------------------------------------------------------
                  ENUM: LOW_LEVEL_SYNC_IND_ENUM_TYPE

This enumerates the possible output indications from the DCH low level
sync detection sub-module.

IN_SYNC_IND     an in sync indication
OUT_SYNC_IND    an out of sync indication
NO_IND          neither indication was seen to be true, no indication made
ERR_IND         both the in and out of sync conditions were true
--------------------------------------------------------------------------*/
typedef enum {
  LL_SYNC_IN_SYNC_IND,
  LL_SYNC_OUT_SYNC_IND,
  LL_SYNC_NO_IND,
  LL_SYNC_ERR_IND
} low_level_sync_ind_enum_type;

/*--------------------------------------------------------------------------
                       ENUM: DCH_SYNC_STATE_ENUM_TYPE

This enumerates the two possible sync states, IN and OUT of sync.
--------------------------------------------------------------------------*/
typedef enum {
  IN_SYNC_STATE,
  OUT_OF_SYNC_STATE
} dch_sync_state_enum_type;

/*--------------------------------------------------------------------------
                       ENUM: DCH_SYNC_TYPE_ENUM_TYPE

This enumerates the state in which sync procedure is being run, DCH or FACH.
--------------------------------------------------------------------------*/

typedef enum {
  DCH_SYNC,
  FACH_SYNC,
  NO_SYNC
} dch_sync_type_enum_type;

/*--------------------------------------------------------------------------
                  STRUCTURE: DL_SYNC_DET_VAR_STRUCT_TYPE

This struture contains the persistent variables for DCH state sync
detection.
--------------------------------------------------------------------------*/
typedef struct {
  /* TRUE if Sync detection is enabled, this occurs at the 1st frame
  boundary of a newly setup DL DPCH. In case of DC-HSUPA, for primary carrier
  this will set based on dlphch add/recfg notify, where as for secondary
  it would be set at the reception of hs-scch order   */
  boolean enabled;
  /* TRUE when the DPCH is declared as established, FALSE before then */
  boolean dpch_estab;

  /* TRUE if in the 1st 160 ms of DCH sync detection operation */
  boolean in_phase_1;

  /* Counter to track number of frames after the Uplink DPCCH 
  establishment is done */
  uint8 ul_dpcch_config_delay_cnt;

  /* TRUE if in the 1st 40 ms of first 160 ms (Phase 1) of DCH sync
     detection operation */
  uint8 in_first_40ms;

  /* the filter buffer for DPCCH energy filtering */
  int16 dpcch_eng_data_ph1[DPCCH_ENG_FILTER_LEN_PH1];
  int16 dpcch_eng_data_ph2[DPCCH_ENG_FILTER_LEN_PH2];

  /* the DPCCH energy accumulator */
  int32 dpcch_eng_accum_ph1;
  int32 dpcch_eng_accum_ph2;

  /* the DPCCH filter data buffer head pointer */
  uint16 dpcch_filter_head_ph1;
  uint16 dpcch_filter_head_ph2;

  /* number of active slots so far, keep adding and once it become 15 reset it*/
  uint8  num_valid_slots;

  /* to store so far accumulated energy, reset when valid slot count reaches 15 */
  int32  slot_level_accum_eng;

  /* to store energy of partial frame */
  int32 spill_dpcch_eng;

  boolean  is_spill_eng_used;

  uint8 fdpch_active_Slots;


  /* frame count for the phase 1 */
  uint32 frame_cnt_ph1;
  uint32 frame_cnt_ph1_after_estab;

  /* the DCH sync state */
  dch_sync_state_enum_type sync_state;

  /* how many consecutive low level in sync indications have been received */
  uint16 in_sync_cnt;

  /* how many consecutive low level out of sync indications have been
  received */
  uint16 out_sync_cnt;

  /* Default value is FALSE */
  boolean    post_verification_valid;

  /* Default value is FALSE */
  boolean  skip_sync_a_procedure;

  /* indicate if T313 counters are active */  
  boolean t313_active;

  /* stores the currect timer count of the timers */
  uint16 t312_cur_count;
  uint16 t313_cur_count;
  
  /* dl in sync is percarrier..however pa would be off when both carrier's UL Tx is off */
  boolean dl_in_sync_tx_enabled;
  boolean dl_force_ul_tx_enable_at_in_sync;
   
  /* below are just added for debug code */         
  boolean in_sync_forty_frames;
  uint16 in_sync_forty_frames_counter;
  int16 debug_rxagc;

  /* Flag used to Freeze TPC if needed after resuming from LTA*/
  boolean dl_freeze_tpc_after_lta;

} dl_sync_det_var_per_carrier_struct_type;

typedef struct {
  /* the N312 counter used for declaring the DPCH established */
  uint16 N312;

  /* the N313 counter used for detecting the OUT_OF_SYNC condition */
  uint16 N313;

  /* the N315 counter used for detecting the IN_SYNC condition */
  uint16 N315;

  /* keep track of CRC errors, number of TB with CRC count */
  uint16  num_tb_with_crc[DPCH_SYNC_MAX_CRC_ERR_FRAME];
  uint16  num_tb_with_crc_err[DPCH_SYNC_MAX_CRC_ERR_FRAME];
  uint8  tb_with_crc_cnt_head;
  uint16 num_tb_rece_with_crc_accum;
  uint16 num_tb_rece_with_crc_err_accum;
  uint16 consec_bad_trblk_cnt;

  /* Default value is FALSE */
  boolean  dpch_reslam_pending;
  /* State in which sync procedure is being run.*/
  dch_sync_type_enum_type sync_type;

  /* number of frames in which the T312 timer expires */
  uint16 t312_unit_frame;
  /* number of frames in which the T313 timer expires */
  uint16 t313_unit_frame;

  boolean T312_to_be_updated;
  uint8 T312_latest;
  boolean T313_to_be_updated;
  uint8 T313_latest;

  /*Status and counter for Dynamic FDPCH Qin/QOut Thresholds*/
  boolean dl_fdpch_repoint_qvalue_status;
  uint8 dl_fdpch_repoint_qvalue_counter;

  /* below flag indicates if x4023 log packet needs to be flushed or not*/
  boolean flush_sync_log_pkt;

  dl_sync_det_var_per_carrier_struct_type per_carrier_info[WL1_MAX_UL_CARR_IDX];

} dl_sync_det_var_struct_type;

/* COMMAND: DL_TRBLK_CRC_STATUS_CMD */
/* -------------------------------- */

/* This structure contains parameters for the DL_TRBLK_CRC_STATUS_CMD. */
/* --------------------------------------------------------------------*/
typedef struct {
  l1_local_cmd_hdr_type hdr;

  /* the number of transport blocks recieved this frame */
  uint8 ntrblks;

  /* the number of transport blocks received in error this frame */
  uint8 nerr;

} dl_trblk_crc_status_type;


extern boolean wenc_crash_phychan_fail;
extern boolean wenc_phychan_est_fail;
extern boolean dl_cltd_init_cnt_skip_condition_flag;


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

extern dl_sync_det_var_struct_type dl_dch_sync_vars;
extern boolean dl_in_reconfig_gap;


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#ifdef FEATURE_WCDMA_DC_HSUPA

/*===========================================================================
FUNCTION dl_trch_handle_sync_for_lta_on_sec

DESCRIPTION
  This function checks if LTA is to handled on secondary carrier

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/


extern boolean dl_trch_handle_sync_for_lta_on_sec(void);

#endif /* FEATURE_WCDMA_DC_HSUPA*/
/*===========================================================================
FUNCTION dl_trch_meas_init

DESCRIPTION
  This function initialize the trch related measurement.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_trch_meas_init(void);

/* BLER measurement related function */
/* --------------------------------- */

/*===========================================================================
FUNCTION dl_bler_setup_meas_critera

DESCRIPTION
  This function sets up the BLER measurement criteria. It can be used to
  set/modify/remove any BLER measurement for a TrCh Id. The BLER stats and
  parameters are reset when CCTrCh is setup for new PhyChan ADD. The BLER
  measurement is not cancelled when CCTrCh is dropped. It is assumed that
  next Phychan Add/CCTrCh setup will reset it or it will be turned off
  explicitly by using this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_bler_setup_meas_critera(
  /* bler meas req */
  l1_dl_bler_meas_setup_req_struct_type *bler_meas_setup_req);

/*===========================================================================
FUNCTION dl_bler_reset_stats_params

DESCRIPTION
  This function resets the BLER stats and params for the given TrCh Id.
  The measurement criteria is set to NO REPORT. All counters/values are
  set to 0. All flags are set to FALSE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_bler_reset_stats_params(
  /* TrCh id */
  uint8 trch_id);

/*===========================================================================
FUNCTION dl_trch_bler_info_update

DESCRIPTION
  This function is called for updating the BLER info update. This functions
  updates the number of CRC received and the number of CRC errors received.
  It assumes that number of TB received is only 1 (for example in BTFD TrCh
  pass/fail, RFCI decode fail)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_trch_bler_info_update(
  /* TrCh Id */
  uint8 trch_id,
  /* number of CRC results */
  uint8 crc_results,
  /* CRC errors */
  uint8 crc_errors);

#ifdef FEATURE_UMTS_UI_REPORT_BLER 
#error code not present
#endif

/*===========================================================================
FUNCTION dl_trch_bler_info_update_w_tb_info

DESCRIPTION
  This function is similar to dl_trch_bler_info_update except that it has
  explicit TB CRC pass/fail information passed to it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_trch_bler_info_update_w_tb_info(
  /* TrCh Id */
  uint8 trch_id,
  /* number of CRC results. This is equal to number of TBs */
  uint8 crc_results,
  /* CRC errors */
  uint8 crc_errors,
  /* CRC pass/fail result for each TB, LSB is last TB decoded */
  uint32 tb_crc_result);

/*===========================================================================
FUNCTION dl_trch_cm_bler_info_update

DESCRIPTION
  This function is called for updating the compressed mode BLER info.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_trch_cm_bler_info_update(
  /* TrCh Id */
  uint8 trch_id,
  /* number of CRC results */
  uint8 crc_results,
  /* CRC errors */
  uint8 crc_errors);

/*===========================================================================
FUNCTION dl_bler_periodic_report

DESCRIPTION
  This function calculate the BLER log values for the list of TrCh ids
  passed to the function. It checks for the reporting type of the BLER
  and if it is not the periodic measurement then it set the value to zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_bler_periodic_report(
  /* number of TrCh Id in the list */
  uint8 num_trch,
  /* TrCh Id list for which to report the measurement */
  uint8 *trc_id_list,
  /* Array in which the BLER log values are reported */
  uint8 *bler_log_val,
  /* BLER calculation window */
  uint16 report_dur_ms);

/* DCH in-sync/out-of-sync detection related function */
/* -------------------------------------------------- */

/*===========================================================================
FUNCTION        DL_TRBLK_CRC_STATUS_RPT

FILE NAME       dl_dem.c

DESCRIPTION     This function is called by the downlink/decoder module at
                every decoder header event.  It indicates to the downlink/
                sync detection sub-module that on the CCTrCH referred to
                by the cctrch_table_index
                1) how many transport blocks were received
                2) how many transport blocks were received in error as
                   indicated by their CRC check status.

                This function then formats a L1 local command with this
                information and posts it to the L1 local command queue.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    A DL_TRBLK_CRC_STATUS_CMD will be posted to the L1
                local command queue.
===========================================================================*/

extern void dl_trblk_crc_status_rpt(
  /* how many transport blocks received this frame */
  uint8 ntrblks,
  /* how many of them failed their CRC check */
  uint8 nfail);

/*===========================================================================
FUNCTION        DL_DCH_SYNC_INIT

FILE NAME       dl_dem.c

DESCRIPTION     This function initializes the low and high level DCH sync
                detection variables and state.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The DCH sync variables are initialized
===========================================================================*/

extern void dl_dch_sync_init(void);

/*===========================================================================
FUNCTION dl_set_ul_force_ul_tx_enable_at_in_sync

DESCRIPTION
  This function set the UL in sync TX enable flag. This is useful to enable
  the UL when the UL is added and physical channel establishment is not
  carried out.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_set_ul_force_ul_tx_enable_at_in_sync(
  /* state of the UL in sync TX enable flag */
  boolean state,
  /*Primary or secondary carrier */
  wl1_ul_carr_id_enum_type carr_idx
    );

/*===========================================================================
FUNCTION        DL_LOW_LEVEL_SYNC_DETECT

FILE NAME       dl_dem.c

DESCRIPTION     This function performs the low level sync detection in the
                DCH state as described by TS 25.214.

DEPENDENCIES    None

RETURN VALUE    The low level sync indication, one of the following
                LL_SYNC_IN_SYNC_IND,
                LL_SYNC_OUT_SYNC_IND,
                LL_SYNC_NO_IND,
                LL_SYNC_ERR_IND

SIDE EFFECTS    The uplink controller may be instructed to enable or disable
                the transmitter
===========================================================================*/

extern low_level_sync_ind_enum_type dl_low_level_sync_detect(
  /* how many transport blocks received in total this frame */
  uint16 nblks,
  /* how many transport blocks received in error this frame */
  uint16 nerr,
  /* the current DPCCH energy sample */
  int16 dpcch_eng);

/*===========================================================================
FUNCTION        DL_HIGH_LEVEL_SYNC_DETECT

FILE NAME       dl_dem.c

DESCRIPTION     This function implements the high level sync detection

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Indications may be sent to RRC, one of the following
                CPHY_PHYCHAN_ESTABLISHED_IND,
                CPHY_IN_SYNC_IND,
                CPHY_OUT_OF_SYNC_IND
===========================================================================*/

extern void dl_high_level_sync_detect(
  /* the indication received from the low level sync detector */
  low_level_sync_ind_enum_type frame_ind);

/*===========================================================================
FUNCTION        DL_TRCH_SUBMIT_PWRCTRL_SYNC_LOGPKT

FILE NAME       dlcmd.c

DESCRIPTION     This function submits the power control log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_trch_submit_pwrctrl_sync_logpkt(void);

/*===========================================================================
FUNCTION        DL_TRCH_SUBMIT_ULTPC_DEBUG_LOGPKT

FILE NAME       dlcmd.c

DESCRIPTION     This function submits the ul tpc debug log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_trch_submit_ultpc_debug_logpkt(void);

/*===========================================================================
FUNCTION        DL_TRCH_SUBMIT_FDPCH_DEBUGPKT

FILE NAME       dltrchmeas.c

DESCRIPTION     This function submits the fdpch debug log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

extern void dl_trch_submit_fdpch_debug_logpkt(void);

/*===========================================================================
FUNCTION        dl_trch_init_multi_comb_tpc_logpkt

FILE NAME       Dltrchmeas.c

DESCRIPTION     This function initiats the Multi-combiner power control log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void dl_trch_init_multi_comb_tpc_logpkt(void);

/*===========================================================================
FUNCTION        dl_trch_init_l1_ultpc_debug_logpkt

FILE NAME       Dltrchmeas.c

DESCRIPTION     This function initiats the l1_ultpc_debug_logpkt

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void dl_trch_init_l1_ultpc_debug_logpkt(void);

/*===========================================================================
FUNCTION        DL_TRCH_SUBMIT_MULTI_COMB_TPC_LOGPKT

FILE NAME       Dltrchmeas.c

DESCRIPTION     This function submits the Multi-combiner power control log pkt.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

void dl_trch_submit_multi_comb_tpc_logpkt(void);

/*===========================================================================
FUNCTION        DL_TRBLK_CRC_STATUS_CMD

FILE NAME       dl_dem.c

DESCRIPTION     This function handles the DL_TRBLK_CRC_STATUS_CMD.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

/* COMMAND: DL_TRBLK_CRC_STATUS_CMD */
/* -------------------------------- */

extern void dl_trblk_crc_status_cmd(
  /* DL DCH trblk CRC report command */
  dl_trblk_crc_status_type *cmd);

/*===========================================================================
FUNCTION dl_ul_freeze_tpc

DESCRIPTION
  This function freezes the TPC UL accumulation. It also sets the various flag
  assist the unfreezing the UL TPC accumulation.
  This is intended to be used for UL TPC accumulation freeze for DPCH reconfig.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dl_ul_freeze_tpc(void);

/*===========================================================================
FUNCTION        DL_UPD_SYNC_PARMS

FILE NAME       dltrchmeas.c

DESCRIPTION     This function updates the constants related to DL sync-N312,
                N313 and N315 right away. The timers-T312 and T313 are updated
                if they are not running. Else, they are marked to be updated when
                the timers are stopped. (Sec-8.3.3.3 in 25.331 v3.18.0)

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void dl_upd_sync_parms(l1_upd_dl_sync_parms_cmd_type *cmd);

/*===========================================================================
FUNCTION        DL_TRCH_SUBMIT_CLTD_LOG_PACKET

FILE NAME       dltrchmeas.c

DESCRIPTION     This function submits cltd log packet

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS     None
===========================================================================*/
void dl_trch_submit_cltd_log_packet(void);

/*===========================================================================
FUNCTION     DL_TRBLK_GET_SIR

DESCRIPTION     This function populates the maximum SIR value to be sent to CM module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
None
===========================================================================*/
extern void dl_trblk_get_sir(int16 *sir);

/*===========================================================================
FUNCTION        DL_TRCH_MEAS_GET_SYNC_STATE

FILE NAME       dltrchmeas.c

DESCRIPTION     This function returns the state of DL sync.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern boolean dl_trch_meas_get_sync_state(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION        dl_sync_detect

FILE NAME       dltrchmeas.c

DESCRIPTION     

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void dl_sync_detect(
  /* Primary or secondary carrier */
  wl1_ul_carr_id_enum_type carr_idx,
  /* how many transport blocks received in total this frame */
  uint16 nblks,
  /* how many transport blocks received in error this frame */
  uint16 nerr,
  /* the current DPCCH energy sample */
  int16 dpcch_eng);

/*===========================================================================
FUNCTION  dl_dch_dp_debug

DESCRIPTION     
      This function logs debug code.

DEPENDENCIES
      None
      
RETURN VALUE
      None
      
SIDE EFFECTS
      None
===========================================================================*/
extern void dl_trch_ilpc_debug_logpkt(void);

/*===========================================================================
FUNCTION  dl_dch_dp_debug_init

DESCRIPTION     
      This function logs debug code.

DEPENDENCIES
      None
      
RETURN VALUE
      None
      
SIDE EFFECTS
      None
===========================================================================*/
extern void dl_dch_dp_debug_init(void);

/*===========================================================================
FUNCTION  dl_get_sync_det_enable_status

DESCRIPTION     
      

DEPENDENCIES
      None
      
RETURN VALUE
      None
      
SIDE EFFECTS
      None
===========================================================================*/
extern boolean dl_get_sync_det_enable_status(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  dl_dch_per_carr_sync_params_init

DESCRIPTION     
      

DEPENDENCIES
      None
      
RETURN VALUE
      None
      
SIDE EFFECTS
      None
===========================================================================*/
extern void dl_dch_per_carr_sync_params_init(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  dl_set_sync_det_enable_status

DESCRIPTION   

DEPENDENCIES
      None

RETURN VALUE
      None

SIDE EFFECTS
      None
===========================================================================*/
extern void dl_set_sync_det_enable_status(boolean status, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  dl_trch_is_sync_established

DESCRIPTION     
      This function returns sync establishment status of carried id passed

DEPENDENCIES
      None
      
RETURN VALUE
      boolean
      
SIDE EFFECTS
      None
===========================================================================*/
extern boolean dl_trch_is_sync_established(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  dl_put_sec_carr_in_dormant_mode

DESCRIPTION
      This function triggers actions to put secondary carrier into dormant
      mode upon phychan establishment failure/rl failure.

DEPENDENCIES
      None

RETURN VALUE
      None

SIDE EFFECTS
      None
===========================================================================*/
extern void dl_put_sec_carr_in_dormant_mode(boolean);

 /*===========================================================================

FUNCTION dl_set_dltrchmeas_nv_values

DESCRIPTION
  Function controls enabling of debug F3s for dltrchmeas

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_set_dltrchmeas_nv_values(void);

 /*===========================================================================

FUNCTION dl_update_fdpch_sire_rep_status

DESCRIPTION
  This function updates the status of qvalue_status and qvalue_counter for the respective carrier.
  dl_fdpch_repoint_qvalue_status represents the status of whether Q_DPCH or Q_FDPCH needs to be used.
  dl_fdpch_repoint_qvalue_counter represents the counter will runs for 200 frames when status is TRUE

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
extern void dl_update_fdpch_sire_rep_status(boolean status);

#ifdef FEATURE_QSH_EVENT_METRIC
 /*===========================================================================

FUNCTION wl1_qsh_dl_metric_cb

DESCRIPTION
  This function is logs the SIRE metric for QSH

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
 ===========================================================================*/
extern boolean wl1_qsh_dl_metric_cb(wl1_qsh_metric_e metric_id, uint8* addr_ptr, wl1_qsh_metric_action_enum_type action);

/*===========================================================================
FUNCTION wl1_qsh_voice_bler_metric_cb

DESCRIPTION
  This function performs the requested QSH action

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_qsh_voice_bler_metric_cb(wl1_qsh_metric_e metric_id, uint8* addr_ptr, wl1_qsh_metric_action_enum_type action);

#endif /* FEATURE_QSH_EVENT_METRIC */
#endif /* DL_TRCH_MEAS_H */
