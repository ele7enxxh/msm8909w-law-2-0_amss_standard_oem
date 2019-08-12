/*!
  @file
  ds_bearer_context.h

  @brief
  Lower layer/Bearer Context interface

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/ds_3gpp_bearer_context.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/13   ss      Increasing WCDMA UL DNE to 450K to buffer packets on
                   modem when QoS is enabled in the QMI link from AP.
11/20/12   ss      Fixed the issue of BP removal NV read failure.
08/10/12   ss      Delegating RAT specific handling during call_end.
08/08/12   ss      Optimization to trigger Dynamic BP only when threshold
                   num of acks are received.
06/07/12   ss      Enabled Dynamic BP by default and set the default tmr_val
                   to 2 seconds.
03/22/12   ss      Addition of RAB_FAILURE and ESM local cause codes.
01/19/12   ss      Modified default LTE UL WM level.
12/21/11   ss      Added support for watermark tuning.
10/20/11   ss      Dynamic back pressure for TCP throughput optimization.
10/05/11   ttv     Added DSDS changes for new mode handler.
09/09/11   rr      Added support for fallback from Dual IP to single IP when nw rejects 
                   with specific cause codes in UMTS mode
06/21/11   dvk     Fixed compilation warnings.
04/07/11   ttv     Addec changes for IOCTL PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG.
03/24/11   hs      Increased DNE to be 50K over HIGH for LTE calls.
02/10/11   ss      Added support for configurable RAB_REESTAB RETRY.
02/23/11   ttv     Added support for watermark tracing.
01/07/11   ss      Changed watermark levels.
05/17/10   hs      Added APN disable and PDN inactivity feature support.
04/23/10   vk      Use of MSGR for UL flow control event processing in LTE
04/02/10   vrk     UMTS interRAT support
01/23/10   vk      Added helper macros for virtual function dispatch
===========================================================================*/

#ifndef DS_3GPP_BEARER_CONTEXT_H
#define DS_3GPP_BEARER_CONTEXT_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "dstask_v.h"
#include "ds_3gpp_hdlr.h"
#include "ds_3gpp_pdn_context.h"
#include "cm.h"
#include "ps_iface.h"
#include "dsm_queue.h"
#include "dsumtspdpreg.h"
#include "ps_ppp.h"
#include "ds3gmgr.h"
#include "ds_3gppi_utils.h"
#include "sys.h"
#include "ps_iface_defs.h"
#include "cm.h"
#include "emm_irat_if_msg.h"
#include "cm_gw.h"
#include "ps_comp_logging_helper.h"
#include "ps_wmk_logging.h"
#ifdef FEATURE_QXDM_WM_TRACING
#include "fc_wm.h"
#endif /* FEATURE_QXDM_WM_TRACING */

#include "pstimer.h"

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_sys.h"
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define DS_BEARER_CTXT_VF_FPN(func) DS_3GPPI_VF_FPN(func)

#define DS_BEARER_CTXT_VF_REG(mode, func, fp) \
          DS_3GPPI_VF_REG(ds_bearer_context_ftbl, mode, func, fp)

#define DS_BEARER_CTXT_VOBJ_REG(ds_bearer_ctxt_p, mode, objp) \
          DS_3GPPI_VOBJ_REG((ds_bearer_ctxt_p)->ds_bearer_context_dyn_p->\
           vobj_data_ptr_tbl, mode, (void*)(objp))

#define DS_BEARER_CTXT_VOBJ(ds_bearer_ctxt_p, mode) \
          DS_3GPPI_VOBJ((ds_bearer_ctxt_p)->ds_bearer_context_dyn_p->\
          vobj_data_ptr_tbl, mode)


#define DS_BEARER_CTXT_VF_CALL(func, subs_id, ...)  \
            DS_3GPPI_VF_CALL(ds_bearer_context_ftbl, func, subs_id, __VA_ARGS__)


#define DS_BEARER_CTXT_VFR_CALL(rval, func, subs_id, ...)  \
            DS_3GPPI_VFR_CALL(ds_bearer_context_ftbl, rval, func, subs_id, \
                               __VA_ARGS__)


#define  RAB_REESTAB_RETRY_CNT_INVALID  0x7fffffff
#define  RAB_REESTAB_RETRY_CNT_DEFAULT  3
/*---------------------------------------------------------------------------
  Default Watermark Levels
---------------------------------------------------------------------------*/

#define DSUMTSPS_UL_WM_DEFAULT_LO        4500
#define DSUMTSPS_UL_WM_DEFAULT_HI        7500
#define DSUMTSPS_UL_WM_DEFAULT_DNE       13500
#define DSUMTSPS_UL_WM_DEFAULT_DNE_QCNT  27   //QCNT = DNE/500

#define DSUMTSPS_DL_WM_DEFAULT_LO        1600
#define DSUMTSPS_DL_WM_DEFAULT_HI        40000
#define DSUMTSPS_DL_WM_DEFAULT_DNE       49000
#define DSUMTSPS_DL_WM_DEFAULT_DNE_QCNT  98   //QCNT = DNE/500
/*---------------------------------------------------------------------------
  Default Watermark Levels for DC_HSUPA
---------------------------------------------------------------------------*/
#define DSWPSD_UL_WM_DEFAULT_DC_HSUPA_LO       48000 
#define DSWPSD_UL_WM_DEFAULT_DC_HSUPA_HI       120000
#define DSWPSD_UL_WM_DEFAULT_DC_HSUPA_DNE      350000
#define DSWPSD_UL_WM_DEFAULT_DC_HSUPA_DNE_QCNT 700

/*---------------------------------------------------------------------------
  Default Watermark Levels for HSPA+/Release7  
---------------------------------------------------------------------------*/

#define DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_LO       40000
#define DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_HI       65000
#define DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_DNE      450000 //AP QoS enabled
#define DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT 900 //QCNT = DNE/500
/*--------------------------------------------------------------------------
  Default Watermark levels for 3C HSDPA
--------------------------------------------------------------------------*/
#define  DSWPSD_DL_WM_DEFAULT_3C_HSDPA_LO       216000
#define  DSWPSD_DL_WM_DEFAULT_3C_HSDPA_HI       540000
#define  DSWPSD_DL_WM_DEFAULT_3C_HSDPA_DNE      675000
#define  DSWPSD_DL_WM_DEFAULT_3C_HSDPA_DNE_QCNT 1350

#if defined(FEATURE_WCDMA_REL8)
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_LO       400000//500000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_HI       800000 //1000000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE      1000000 //1100000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT 2000 //QCNT = DNE/500

#elif defined(FEATURE_WCDMA_MIMO) 
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_LO        200000//150000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_HI        800000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE       900000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT  1800 //QCNT = DNE/500

#else
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_LO        200000//150000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_HI        600000 // 300000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE       700000// 350000
#define DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT  1400 //QCNT = DNE/500

#endif

/*---------------------------------------------------------------------------
  Default Watermark Levels for LTE  
---------------------------------------------------------------------------*/
#define DSLTEPS_UL_WM_DEFAULT_LO        160000 
#define DSLTEPS_UL_WM_DEFAULT_HI        265000 
#define DSLTEPS_UL_WM_DEFAULT_DNE       345000
#define DSLTEPS_UL_WM_DEFAULT_DNE_QCNT  690 //QCNT = DNE/500

#define DSLTEPS_DL_WM_DEFAULT_LO        175000 //10ms*100Mbps
#define DSLTEPS_DL_WM_DEFAULT_HI        350000 //20ms*100Mbps
#define DSLTEPS_DL_WM_DEFAULT_DNE       400000 //HI+50k
#define DSLTEPS_DL_WM_DEFAULT_DNE_QCNT  800 //QCNT = DNE/500

/*---------------------------------------------------------------------------
  Default Watermark Levels for EUL
  These can be overridden for each link (e.g. dswpsdhdlr.h may override
  value for PS->RLC WM used for EUL)
---------------------------------------------------------------------------*/

#define DSWPSD_UL_WM_DEFAULT_EUL_LO       38000
#define DSWPSD_UL_WM_DEFAULT_EUL_HI       40000
#define DSWPSD_UL_WM_DEFAULT_EUL_DNE      54000
#define DSWPSD_UL_WM_DEFAULT_EUL_DNE_QCNT 108  //QCNT = DNE/500

/*---------------------------------------------------------------------------
  Default Watermark Levels for EUL with 2ms TTI
---------------------------------------------------------------------------*/

#define DSWPSD_UL_WM_DEFAULT_EUL_2MS_LO       45000
#define DSWPSD_UL_WM_DEFAULT_EUL_2MS_HI       65000
#define DSWPSD_UL_WM_DEFAULT_EUL_2MS_DNE      82000
#define DSWPSD_UL_WM_DEFAULT_EUL_2MS_DNE_QCNT 164  //QCNT = DNE/500

/*---------------------------------------------------------------------------
  Default Watermark Levels for HSDPA Phase 2
---------------------------------------------------------------------------*/
#define DSWPSD_DL_WM_DEFAULT_HS_P2_LO        120000
#define DSWPSD_DL_WM_DEFAULT_HS_P2_HI        170000
#define DSWPSD_DL_WM_DEFAULT_HS_P2_DNE       300000
#define DSWPSD_DL_WM_DEFAULT_HS_P2_DNE_QCNT  600 //QCNT = DNE/500


/*---------------------------------------------------------------------------
  Default Watermark Levels for HSDPA Phase 1
---------------------------------------------------------------------------*/
#define DSWPSD_DL_WM_DEFAULT_HS_P1_LO       60000
#define DSWPSD_DL_WM_DEFAULT_HS_P1_HI       85000
#define DSWPSD_DL_WM_DEFAULT_HS_P1_DNE      150000
#define DSWPSD_DL_WM_DEFAULT_HS_P1_DNE_QCNT  300 //QCNT = DNE/500

/*---------------------------------------------------------------------------
  Default Watermark Levels for HSDPA 
---------------------------------------------------------------------------*/
#define DSWPSD_DL_WM_DEFAULT_HS_LO        40000
#define DSWPSD_DL_WM_DEFAULT_HS_HI        80000
#define DSWPSD_DL_WM_DEFAULT_HS_DNE       125000
#define DSWPSD_DL_WM_DEFAULT_HS_DNE_QCNT  250 //QCNT = DNE/500

/*---------------------------------------------------------------------------
  WCDMA PS<->PDCP Watermark Levels
---------------------------------------------------------------------------*/
#if defined (FEATURE_WCDMA_DC_HSUPA) || defined (FEATURE_WCDMA_16_QAM)  
  #define DSWPSD_UL_WM_LO          DSWPSD_UL_WM_DEFAULT_DC_HSUPA_LO
  #define DSWPSD_UL_WM_HI          DSWPSD_UL_WM_DEFAULT_DC_HSUPA_HI
  #define DSWPSD_UL_WM_DNE         DSWPSD_UL_WM_DEFAULT_DC_HSUPA_DNE
  #define DSWPSD_UL_WM_DNE_QCNT    DSWPSD_UL_WM_DEFAULT_DC_HSUPA_DNE_QCNT

#elif defined(FEATURE_WCDMA_REL7) || defined(FEATURE_WCDMA_REL8)
  #define DSWPSD_UL_WM_LO          DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_LO
  #define DSWPSD_UL_WM_HI          DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_HI
  #define DSWPSD_UL_WM_DNE         DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_DNE
  #define DSWPSD_UL_WM_DNE_QCNT    DSWPSD_UL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT 

#elif defined(FEATURE_WCDMA_HSUPA)

#if defined(FEATURE_WCDMA_HSUPA_2MS_TTI)
  #define DSWPSD_UL_WM_LO          DSWPSD_UL_WM_DEFAULT_EUL_2MS_LO
  #define DSWPSD_UL_WM_HI          DSWPSD_UL_WM_DEFAULT_EUL_2MS_HI
  #define DSWPSD_UL_WM_DNE         DSWPSD_UL_WM_DEFAULT_EUL_2MS_DNE
  #define DSWPSD_UL_WM_DNE_QCNT    DSWPSD_UL_WM_DEFAULT_EUL_2MS_DNE_QCNT
#else
  #define DSWPSD_UL_WM_LO          DSWPSD_UL_WM_DEFAULT_EUL_LO
  #define DSWPSD_UL_WM_HI          DSWPSD_UL_WM_DEFAULT_EUL_HI
  #define DSWPSD_UL_WM_DNE         DSWPSD_UL_WM_DEFAULT_EUL_DNE
  #define DSWPSD_UL_WM_DNE_QCNT    DSWPSD_UL_WM_DEFAULT_EUL_DNE_QCNT
#endif /* FEATURE_WCDMA_HSUPA_2MS_TTI */

#elif defined(FEATURE_WCDMA)
  #define DSWPSD_UL_WM_LO          DSUMTSPS_UL_WM_DEFAULT_LO
  #define DSWPSD_UL_WM_HI          DSUMTSPS_UL_WM_DEFAULT_HI
  #define DSWPSD_UL_WM_DNE         DSUMTSPS_UL_WM_DEFAULT_DNE
  #define DSWPSD_UL_WM_DNE_QCNT    DSUMTSPS_UL_WM_DEFAULT_DNE_QCNT
#endif /* FEATURE_WCDMA_DC_HSUPA */


#ifdef FEATURE_WCDMA_3C_HSDPA
  #define DSWPSD_DL_WM_LO          DSWPSD_DL_WM_DEFAULT_3C_HSDPA_LO
  #define DSWPSD_DL_WM_HI          DSWPSD_DL_WM_DEFAULT_3C_HSDPA_HI
  #define DSWPSD_DL_WM_DNE         DSWPSD_DL_WM_DEFAULT_3C_HSDPA_DNE
  #define DSWPSD_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_3C_HSDPA_DNE_QCNT

#elif defined(FEATURE_WCDMA_REL7) || defined(FEATURE_WCDMA_REL8)
  #define DSWPSD_DL_WM_LO          DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_LO
  #define DSWPSD_DL_WM_HI          DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_HI
  #define DSWPSD_DL_WM_DNE         DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE
  #define DSWPSD_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HSPA_PLUS_DNE_QCNT

#elif defined(FEATURE_6280_HSDPA_PHASE2_CAPABILITY)

  #define DSWPSD_DL_WM_LO          DSWPSD_DL_WM_DEFAULT_HS_P2_LO
  #define DSWPSD_DL_WM_HI          DSWPSD_DL_WM_DEFAULT_HS_P2_HI
  #define DSWPSD_DL_WM_DNE         DSWPSD_DL_WM_DEFAULT_HS_P2_DNE
  #define DSWPSD_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HS_P2_DNE_QCNT

#elif defined(FEATURE_6280_HSDPA_PHASE1_CAPABILITY)
  #define DSWPSD_DL_WM_LO          DSWPSD_DL_WM_DEFAULT_HS_P1_LO
  #define DSWPSD_DL_WM_HI          DSWPSD_DL_WM_DEFAULT_HS_P1_HI
  #define DSWPSD_DL_WM_DNE         DSWPSD_DL_WM_DEFAULT_HS_P1_DNE
  #define DSWPSD_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HS_P1_DNE_QCNT

#elif defined(FEATURE_HSDPA)
  #define DSWPSD_DL_WM_LO          DSWPSD_DL_WM_DEFAULT_HS_LO
  #define DSWPSD_DL_WM_HI          DSWPSD_DL_WM_DEFAULT_HS_HI
  #define DSWPSD_DL_WM_DNE         DSWPSD_DL_WM_DEFAULT_HS_DNE
  #define DSWPSD_DL_WM_DNE_QCNT    DSWPSD_DL_WM_DEFAULT_HS_DNE_QCNT

#elif defined(FEATURE_WCDMA)
  #define DSWPSD_DL_WM_LO          DSUMTSPS_DL_WM_DEFAULT_LO
  #define DSWPSD_DL_WM_HI          DSUMTSPS_DL_WM_DEFAULT_HI
  #define DSWPSD_DL_WM_DNE         DSUMTSPS_DL_WM_DEFAULT_DNE
  #define DSWPSD_DL_WM_DNE_QCNT    DSUMTSPS_DL_WM_DEFAULT_DNE_QCNT
#endif /* FEATURE_WCDMA_3C_HSDPA */

#define DSGPSD_UL_WM_DEFAULT_DNE       500000
#define DSGPSD_UL_WM_DEFAULT_HI        75000
#define DSGPSD_UL_WM_DEFAULT_LO        45000
#define DSGPSD_UL_WM_DEFAULT_DNE_QCNT  1000 //QCNT = DNE/500

#define DSGPSD_DL_WM_DEFAULT_DNE       49000
#define DSGPSD_DL_WM_DEFAULT_HI        40000
#define DSGPSD_DL_WM_DEFAULT_LO        1600
#define DSGPSD_DL_WM_DEFAULT_DNE_QCNT  98 //QCNT = DNE/500


#define DS_UMTS_MAX_VOBJ_PER_CONTEXT 2 /* WCDMA, GPRS */

/*---------------------------------------------------------------------------
  TDSCDMA PS<->PDCP Watermark Levels
---------------------------------------------------------------------------*/
#define DSTPSD_DL_WM_LO        33000 /* ~32Kbytes*/
#define DSTPSD_DL_WM_HI        66000 /* ~64Kbytes*/
#define DSTPSD_DL_WM_DNE       80000 /* ~78Kbytes*/
#define DSTPSD_DL_WM_DNE_QCNT  160 //QCNT = DNE/500


#define DSTPSD_UL_WM_LO        120000 /* ~120Kbytes*/
#define DSTPSD_UL_WM_HI        300000 /* ~300Kbytes*/
#define DSTPSD_UL_WM_DNE       450000 /* ~450 Kbytes to handle BP removal */
#define DSTPSD_UL_WM_DNE_QCNT  900 //QCNT = DNE/500

/*data rates in kbps*/
#define DSTPSD_UL_RATE_16          16000
#define DSTPSD_UL_RATE_32          32000
#define DSTPSD_UL_RATE_64          64000
#define DSTPSD_UL_RATE_128        128000
#define DSTPSD_UL_RATE_256        256000
#define DSTPSD_UL_RATE_384        384000

/*UL WM Levels 16kbps*/
#define DSTPSD_UL_WM_LO_16        14000 
#define DSTPSD_UL_WM_HI_16        28000
#define DSTPSD_UL_WM_DNE_16       34000

/*UL WM Levels 32kbps*/
#define DSTPSD_UL_WM_LO_32        30000
#define DSTPSD_UL_WM_HI_32        60000
#define DSTPSD_UL_WM_DNE_32       72000

 /*UL WM Levels 64kbps*/
#define DSTPSD_UL_WM_LO_64        45000 
#define DSTPSD_UL_WM_HI_64        90000
#define DSTPSD_UL_WM_DNE_64      108000 

/*UL WM Levels 128kbps*/
#define DSTPSD_UL_WM_LO_128       60000 
#define DSTPSD_UL_WM_HI_128      120000 
#define DSTPSD_UL_WM_DNE_128     146000 

/*UL WM Levels 256kbps*/
#define DSTPSD_UL_WM_LO_256       76000
#define DSTPSD_UL_WM_HI_256      152000
#define DSTPSD_UL_WM_DNE_256     184000

/*UL WM Levels 384kbps*/
#define DSTPSD_UL_WM_LO_384       85000 
#define DSTPSD_UL_WM_HI_384      170000
#define DSTPSD_UL_WM_DNE_384     205000

#define DS3GPP_THROUGHPUT_TMR_DEFAULT_VAL 2 /* in seconds */

/*-------------------------------------------------------------------------
 TCP ACK threshold
---------------------------------------------------------------------------*/
#define TCP_THRESHOLD_ACK_CNT_DEF        70 /* Default number of ACKs on uplink which 
                                                would trigger BP removal */

#define BEARER_CNTXT_VALID_COOKIE  0xbeabeabe

/*---------------------------------------------------------------------------
Access barring time remaining
-----------------------------------------------------------------------------*/
#define ACCESS_BARRING_INFINITE_RETRY_TIME 0


/*---------------------------------------------------------------------------
  Struct to read Back_pressure_removal in EFS
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean   back_pressure_removal; /* Back Pressure removal flag */
  boolean   um_wm_params_valid;    /* Whether UM WM params are valid */
  uint32    ul_wm_high_level;      /* UL WM high count */
  uint32    ul_wm_low_level;       /* UL WM low count */
  uint32    ul_wm_dne_level;       /* UL WM dne count */
  uint32    dl_wm_high_level;      /* DL WM high count */
  uint32    dl_wm_low_level;       /* DL WM low count */
  uint32    dl_wm_dne_level;       /* UL WM dne count */
  uint32    timer_val;             /* Dynamic BP timer value */
}ds_bearer_cntxt_bp_removal_struct_type;

/*---------------------------------------------------------------------------
  Struct to store throughput optimization parameters
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  boolean   back_pressure_removal; /* Back Pressure removal flag */
  uint32    timer_val;             /* Dynamic BP timer value */
  uint16    tcp_threshold_ack_cnt; /* Threshold ack_cnt for triggerring BP removal */
}ds_bearer_cntxt_tput_opt_struct_type;

extern ds_bearer_cntxt_tput_opt_struct_type ds_bearer_cntxt_tput_opt_params;

typedef PACKED struct PACKED_POST
{
  boolean  gprs_wm_levels_valid;
  uint32   gprs_ul_dne;
  uint32   gprs_ul_hi;
  uint32   gprs_ul_lo;
  uint32   gprs_dl_dne;
  uint32   gprs_dl_hi;
  uint32   gprs_dl_lo;
  boolean  wcdma_wm_levels_valid;
  uint32   wcdma_ul_dne;
  uint32   wcdma_ul_hi;
  uint32   wcdma_ul_lo;
  uint32   wcdma_dl_dne;
  uint32   wcdma_dl_hi;
  uint32   wcdma_dl_lo;
  boolean  lte_wm_levels_valid;
  uint32   lte_ul_dne;
  uint32   lte_ul_hi;
  uint32   lte_ul_lo;
  uint32   lte_dl_dne;
  uint32   lte_dl_hi;
  uint32   lte_dl_lo;
}ds_bearer_cntxt_um_wm_threshold_struct_type;


typedef PACKED struct PACKED_POST
{
  boolean  tds_wm_levels_valid;
  uint32   tds_ul_dne;
  uint32   tds_ul_hi;
  uint32   tds_ul_lo;
  uint32   tds_dl_dne;
  uint32   tds_dl_hi;
  uint32   tds_dl_lo;
}ds_bearer_cntxt_tds_um_wm_threshold_struct_type;


#define DS_UMTS_MAX_VOBJ_PER_CONTEXT 2 /* WCDMA, GPRS */ 
/*---------------------------------------------------------------------------
  Macro for RAB Reestablish retry time. This variable is used as the
  minimum interval to resend the RAB Reestablish Request when in dormant
  mode. When RAB Reestablish reject is received a timer for a random time
  between DSUMTS_RAB_REESTABLISH_MIN_RETRY_TIME and 
  DSUMTS_RAB_REESTABLISH_MIN_RETRY_TIME * 4 is started.
---------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_RAB_REESTABLISH_MIN_RETRY_TIME 100

/*----------------------------------------------------------------------------
This is the defualt number of attempts when a rab reject is received with 
ACCESS_BARRING_CAUSE_CODE
-----------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_ACCESS_BARR_RAB_RETRY_COUNTER 128

/*! @brief structure to hold WM queue ptr and the WM
*/
typedef struct
{
  q_type             wm_queue;       /*!< Transmit Queue          */
  dsm_watermark_type wm_item;        /*!< Transmit watermark      */
} ds_bearer_context_wm_s;

/* structure to hold fast dormancy related info */
typedef struct
{
  uint32     timer_1_value;
  uint32     timer_2_value;
  boolean    is_FD_enabled;
}ds_3gpp_fd_info_s;

/*-----------------------------------------------------------------------
 This is the eunm that defines the various states of the bearer.
 DS_BEARER_CONTEXT_STATE is a common prefix added to all the states.
 Omitting that, the states are explained as follows:

 DOWN                   - Bearer not in use
 COMING_UP              - Call has been initiated on this bearer
 PEND_REDIAL            - Call was internally rejected due to RAT change
                          DS will redial on the new RAT.
 UP                     - Call is connected on this bearer
                          Traffic can be assumed to be active now.
 UP_DORMANT             - RAB has been released due to inactivity
 UP_DORMANT_REESTAB     - RAB reestablish request has been sent out
                          Bearer will remain in this state until the 
			  network returns a result for the bearer estab
			  request.
 GOING_DOWN             - Call hangup has been initiated on this bearer
-----------------------------------------------------------------------*/

typedef enum
{
  DS_BEARER_CONTEXT_STATE_DOWN                  = 0,
  DS_BEARER_CONTEXT_STATE_COMING_UP             = 1,
  DS_BEARER_CONTEXT_STATE_PEND_REDIAL           = 2,  
  DS_BEARER_CONTEXT_STATE_PEND_RAB_REG          = 3,
  DS_BEARER_CONTEXT_STATE_UP                    = 4,
  DS_BEARER_CONTEXT_STATE_UP_DORMANT            = 5,
  DS_BEARER_CONTEXT_STATE_UP_DORMANT_REESTAB    = 6,
  DS_BEARER_CONTEXT_STATE_GOING_DOWN            = 7,
  DS_BEARER_CONTEXT_STATE_ANSWER                = 8,
  DS_BEARER_CONTEXT_STATE_MAX
} ds_bearer_context_state_e;


/*-----------------------------------------------------------------------
  Forward declaration needed for the structures being defined next.
-----------------------------------------------------------------------*/
struct ds_bearer_context_type;

/* Enhanced ps_iface_net_down_reason_type to hold additional information required 
   for readial */

typedef enum
{
  DS_3GPP_PS_IFACE_NET_DOWN_REASON_T,
  DS_3GPP_EXTENDED_NET_DOWN_REASON_T
} ds_3gpp_down_reason_e_type;

typedef union {
  ps_iface_net_down_reason_type ps_iface_down_reason;
  sm_cm_status_T                internal_reason;
} ds_3gpp_down_reason_u_type;

typedef struct 
{ 
  ds_3gpp_down_reason_u_type reason;
  ds_3gpp_down_reason_e_type  reason_T;
  ps_extended_info_code_enum_type  ps_eic;
} ds_3gpp_down_reason_t;

typedef struct
{
  uint64          actual_ul_rate;
  /** Actual UL throughput in Kbps */
  uint64          actual_dl_rate;
  /** Actual DL throughput in Kbps */
  uint64          total_prev_recd_rx_wm_cnt;
  /** Cached Total prev recd RX WM cnt (in bytes). This is
   *  required to calculate actual DL throughput  */
  uint64          total_prev_recd_tx_wm_cnt;
  /** cached Total prev received TX WM cnt (in bytes). This is
   *  required to calculate the actual UL throughput */
}ds_3gpp_report_bearer_throughput_stats_s;

#define DS_3GPP_PS_IFACE_NET_DOWN_REASON(x)                                            \
      ((ps_iface_net_down_reason_type)                                         \
      ((x.reason_T == DS_3GPP_PS_IFACE_NET_DOWN_REASON_T) ? x.reason.ps_iface_down_reason :\
        PS_NET_DOWN_REASON_NOT_SPECIFIED))


/*------------------------------------------------------------------------
  Enum used to track the state of UM WMs while they are being 
  flushed. While switching data path within the modem from SW to a2, we need 
  to flush the UM and SIO watermarks to guarantee in-order delivery
 ------------------------------------------------------------------------ */
typedef enum {
   DS_BEARER_CONTEXT_UM_WM_STATE_INVALID,
   DS_BEARER_CONTEXT_UM_WM_STATE_UNFLUSHED,
   DS_BEARER_CONTEXT_UM_WM_STATE_FLUSHED,   
}ds_bearer_context_wm_flush_state_e;

typedef enum
{
  DS_BEARER_CONTEXT_EV_MIN          = 0,
  DS_BEARER_CONTEXT_EV_DSM_MEM_CTRL = 1,
  DS_BEARER_CONTEXT_EV_MAX
} ds_bearer_context_event_enum_type;

typedef struct
{
  ds_bearer_context_event_enum_type event_type;
  union
  {
    struct
    {
      dsm_mempool_id_enum_type    pool_id;
      dsm_mem_level_enum_type     event;
      dsm_mem_op_enum_type        op;
    } dsm_info;
  } cmd_info;
} ds_bearer_context_event_type;

#ifdef FEATURE_DATA_WCDMA_PS
typedef struct
{
  void*                      bearer_context_p;
  boolean                    cnf_status;
} ds_bearer_context_purge_cnf_ev_type;
#endif /* FEATURE_DATA_WCDMA_PS */

typedef struct
{
  void*            bearer_context_p;
  dsm_item_type*   reorig_pkt_ptr;
}ds_bearer_context_reorig_cmd_type;

/*-----------------------------------------------------------------------
  This structure is used to hold function pointers to the bearer up and
  bearer down call backs. These functions are invoked by the bearer cntx
  to let the PDN context know about the bringing up or tearing down of
  bearers.

  The call back functions are implemented by PDN context and are passed
  to the bearer context during power up. Bearer context stores the address
  of these functions in this structure below and calls them on appropriate
  events.
-----------------------------------------------------------------------*/
typedef struct
{
    void (*bearer_up_ind_cb)(
                             void                           *object,
                             struct ds_bearer_context_type  *bearer_context_ptr,
                             const cm_call_mode_info_s_type *mode_info_ptr
                            );

   void (*bearer_down_ind_cb)(
                              void                          *object,
                              struct ds_bearer_context_type *bearer_context_ptr,
                              ds_3gpp_down_reason_t                 down_reason,
			                        boolean                       call_was_connected
                             );

} bearer_context_notifier_cb_tbl_s;

/*! @brief Bearer Context
  This structure defines the bearer context type. This is one single structure
  that contains information about the bearer under consideration. The need and 
  use of the various member fields are described inside the structure.
*/
typedef struct ds_bearer_context_dyn_type
{
  /*! Cookie to ensure dyn mem is valid */
  uint32                           cookie;   
  /*!< Bearer context state */
  ds_bearer_context_state_e        state;

  /*!< call id assigned by CM */
  cm_call_id_type                  cm_call_id;

  /*!< store the bearer cntx inst */
  void                             *client_data_ptr;

  //VRK:TODO rename nsapi to nas_bearer_id, get rid of EPS_VOBJ->bearer_id
  //Also check, if we need to save: nas_linked_bearer_id
  //Change testframe and tests too. 
  /*!< nsapi assigned by NAS */
  uint8                            nsapi;


  /*!< is this default bearer cntx */
  boolean                          is_default;
  
  


  /*!< whether hi (lo) wm cb was called last on tx wm*/
  boolean                          tx_hi;

  /*!< watermark state used to guarantee in_order delivery*/
  ds_bearer_context_wm_flush_state_e  wm_flush_state;


  /*!< Array of pointers to mode specific data blob*/
  void                             *vobj_data_ptr_tbl[DS_3GPPI_SYS_MODE_MAX];
  void                             *pdn_context_ptr;
  
  /*!< Ptr to CB tbl for notification of state changes to PdnContext. */
  bearer_context_notifier_cb_tbl_s *pdn_context_cb_tbl_ptr;
   
#ifdef FEATURE_DATA_IMS
  /*!< Flag to indicate if this bearer serves the purpose being the signaling 
   * link for IMS */
  boolean                          im_cn_subsystem_flag;
#endif

  /*!< Flag to indicate that a command to reorig has been enqueued.
   * This prevents multiple commands from being posted to DS when a bunch of
   * packets are enqueued into the WM */
  boolean                          reorig_cmd_enqueued;

  /*!< Flag to keep note of the bearer on which the reorig was sent out*/
  boolean                          srv_rsp_exp;

  /*!< SDF ID to keep track of the PDN connectivity request. EPS specific */
  uint16                           sdf_id;

  /*!< Holds the number of bytes transmitted upto last polling interval   */
  uint64                           tx_bytes;

  /*!< Holds the number of bytes received upto last polling interval      */
  uint64                           rx_bytes;

  /*!< call mode used in L-GW InterRAT, different from the system mode    */
  uint64                           call_mode;
  
  /*!<  If TI is provided for the LTE call then its transferable to GW    */
  boolean              is_transferable_to_GW; 

  boolean              is_registered_with_dpm; 

  /*!< rab_reestab_retry counter      */
  uint32                           rab_reestab_retry_cnt;

  /*!< PS timer handle for dynamic back pressure disable */
  ps_timer_handle_type             tmr_hndl_dyn_back_pressure;

  /*!< Current TCP DL Ack counter */
  uint64                           tcp_dl_ack_counter;

  /*!< Last TCP DL Ack count */
  uint64                           last_tcp_dl_ack_count;

  /*!< ps_iface_down reason. */
  ps_iface_net_down_reason_type    down_reason;

  /*!< ps_eic. */
  ps_extended_info_code_enum_type  down_reason_ps_eic;

  int                              index;

  /*!Used during Handover to GPRS.*/
  boolean                          is_ho2GPRS_in_progress;

  /*!Used to calculate bearer throughput statistics when required*/
  ds_3gpp_report_bearer_throughput_stats_s bearer_throughput_stats;

  uint32               total_bytes_received; /*Store pdcp rx_count for dl tput estimation*/

#ifdef FEATURE_DATA_REL10
  byte                             *apn;
#endif /*FEATURE_DATA_REL10*/
  boolean                           ul_registered_with_ltepdcp;

  sys_modem_as_id_e_type           subs_id;
  /*!<flag will be set if this bearer needs to deactivate on modify IND
       in UMTS NW init QoS case>*/
  boolean                           teardown_on_modify_ind;
} ds_bearer_context_dyn_s;

typedef struct ds_bearer_context_type
{
  /*!< phys link is made a part of the bearer 
    Aloocate this statically because it is being used by rmnet
    to register/deregister events*/
  ps_phys_link_type                phys_link;

  /*--------------------------------------------------------------------------
  Need to work with PDCP team to ensure we don't hit crashes and move this back
  to dyn_mem
  ----------------------------------------------------------------------------*/

    /*!< L2 to PS watermark, Umts DL data*/
  ds_bearer_context_wm_s           rx_wm;
      /*!< L2 to PS watermark, Lte DL data*/
  ds_bearer_context_wm_s           lte_rx_wm;
      /*!< TLB DL watermark, used in Mode a*/
  ds_bearer_context_wm_s           tlb_rx_wm;
  /*!< Function pointer for phys link enable flow cb*/
  void                           * uplink_flow_enable_buf_ptr;
  /*!< PS to L2 watermark, UL data*/
  ds_bearer_context_wm_s           tx_wm;

  /*!< RX Signal for Bearer context.*/
  ps_sig_enum_type                 rx_sig;
  
  /*!< Flag which indicated if RM is flow controlled*/
  boolean                          rm_flow_enabled; 
  
  ds_bearer_context_dyn_s *ds_bearer_context_dyn_p;

} ds_bearer_context_s;

/*===========================================================================
   This Structure is used to maintain all subscription specific global 
   variables of ds_3gpp_bearer_context
   PLEASE USE THIS STRUCTURE WHENEVER YOU WANT TO ADD A NEW SUBSCRIPTION 
   SPECIFIC GLOBAL VARIABLE.
===========================================================================*/
typedef struct
{
  /*-----------------------------------------------------------------------------
    Timer for sending out RAB request again. This timer is effective only from 
    the point when we get a RAB REESTAB reject.
  -----------------------------------------------------------------------------*/
  rex_timer_type         tmr_rab_reestab;
  
  /*-----------------------------------------------------------------------------
    Boolean flag that indicates if the timer is running. Although REX provides
    API which can be used to check if the timer is running, it is more expensive
    in terms of CPU cycles. Hence this boolean flag is maintained. It becomes
    extremely critical to set or reset this flag appropriately as and when the 
    timer is set or reset. Also this boolean should never be set/reset in any
    other task except the DS task.
    Also on timer expiry, this flag has to be explicitly reset in the function
    that handles the timer_expired command
  -----------------------------------------------------------------------------*/
  boolean                timer_running;
  
  /*-----------------------------------------------------------------------------
    This flag is used to keep track of any pending service requests
  -----------------------------------------------------------------------------*/
  boolean                service_request_pending_flag;

  uint32                 uplink_data_bitmask;
  
  boolean                rab_reject_local_cause;
  
  rex_timer_type         pending_redial_tmr;

  boolean                pending_redial_tmr_running;

  sys_modem_as_id_e_type subs_id;

  uint32                  cookie;


} ds_bearer_cntxt_per_subs_info_type;

/*---------------------------------------------------------------------------
  Table of handler functions for bearer_context. Each mode-specific handler 
  maintains such a table, and registers this table with bearer context.
  In fact only three such tables are populated. The three tables correspond to
  the three modes GPRS, WCDMA and LTE.

  Based on the current mode the function belonging to the mode is invoked.
  In some cases these function pointers could be NULL which means that the
  mode does not need such functionality.

  These functions first do the part that is specific to either UMTS or EPS
  (depending on the mode it is invoked in). As the next step, the UMTS impl
  of these functions may invoke either the GPRS or the WCDMA functions. The
  functions that will end up calling a second level of handlers will be 
  marked so.

  The same functions can be registered in different modes if the procedures
  are the same across them.
---------------------------------------------------------------------------*/
typedef struct
{
  /*---------------------------------------------------------------------------
    This is the function that is called from 3gpp_hdlr on call connected.
    This in turn invoked GPRS or WCDMA bearer level functions.
 
    Returns failure if errors occur in setting up the stack. Success is returned
    otherwise.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(activateCnf))(
                           ds_bearer_context_s             *bearer_context_p,
                           const cm_call_mode_info_s_type  *mode_info_p,
                           cm_end_params_s_type            *end_parms_p
                          );

  /*---------------------------------------------------------------------------
    Populates applicable call origination params in ds_3gpp_orig_params_s, 
    including QOS and default (match all) TFT for default bearer. Note that 
    ds_3gpp_orig_params_s is a new type that aggregates all different types
    of cm orig params (for different modes). 
    
    Returns failure if an unexpected error occurs, success otherwise.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(getOrigParams))(
                             ds_bearer_context_s           *bearer_context_p,
                             ds_3gpp_orig_params_s         *mm_orig_params_p,                          
                             const ds_3gpp_pdn_call_info_type 
                                                           *ds3gi_call_info
                            );

  /*---------------------------------------------------------------------------
    Populates call end params to use for bringing down the data call, 
    including reason code.
  ---------------------------------------------------------------------------*/
  
  void (*DS_BEARER_CTXT_VF_FPN(getEndParams))(
                         ds_bearer_context_s          *bearer_context_p,
                         cm_end_params_s_type         *end_params,
                         ds_3gpp_pdn_call_info_type   *ds3gi_call_info,
			 boolean                                        *delay_disconnect
                        );
  /*---------------------------------------------------------------------------
   Called to indicate deactivate of Bearer Context. Tears down the data path
   including deregistration with lower layers if needed, and changes state to
   DOWN. Notifies PdnContext of state change. This is a virtual function and 
   calls mode specific function.
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(deactivateCnf))(
                          ds_bearer_context_s            *bearer_context_p,
                          const ds3g_call_info_cmd_type  *call_info_p, 
                          uint8                           end_type,
                          boolean                         call_was_connected
                       );

  /*---------------------------------------------------------------------------
    Dormancy reorig actions happen here. This function pointer could be set to
    NULL if dormancy is not supported in some modes.
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(dormancyReorig))(
                           ds_bearer_context_s        *bearer_context_p,
                           dsm_item_type*              reorig_pkt_ptr
                          );

  /*--------------------------------------------------------------------------
    Called to indicate berarer activation request indication.
    NULL if we are in non-LTE and pre W REL 8.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(bearerActivateInd))(
                               cm_call_id_type                  call_id,
                               const cm_call_mode_info_s_type   *mode_info_ptr_i
                                );

  /*--------------------------------------------------------------------------
    This function is used to initiate a call. Since the CM API is different 
    for LTE and UMTS, this function pointer will be used to invoke the mode
    specific function to initiate a call in the corresponding mode.

    For LTE calls, the use of Dial string is uet to be evaluated. For now, in
    the EPS specific function, this will be an unused parameter
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(bring_up))(
                        void                         *info_p,
                        ds_3gpp_pdn_call_info_type   *call_info,
                        boolean                      useDefaultDialStr
                      );

  /*--------------------------------------------------------------------------
  This function is used to redial a call. Since the CM API is different 
  for LTE and UMTS, this function pointer will be used to invoke the mode
  specific function to initiate a call in the corresponding mode.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(redial))(
                        ds_bearer_context_s     *bearer_context_p
                      );                            
   
  /*--------------------------------------------------------------------------
   This function is used to free up the bearer when redialing is not performed.
  --------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(go_null)) (
                            ds_bearer_context_s     *bearer_context_p,
                            ds_3gpp_down_reason_t    down_reason,
                            boolean                  call_was_connected
                      );
   /*--------------------------------------------------------------------------
    This function is used to initiate the tear down of a call. Since the APIs
    are different in UMTS and LTE modes, we need mode specific handlers. These
    function pointers are initialized in the corresponding mode specific files
    and are accessed while processing phys_link_down command
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(tear_down))(
                          ds_bearer_context_s          *bearer_context_p,
                          ds_3gpp_pdn_call_info_type   *call_info,
                          void                         *info_ptr
                        );

  /*--------------------------------------------------------------------------
    This function is to perform any mode specific operations on Rab re-estab
    indication.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(rab_re_estab_ind))(
                          ds_bearer_context_s* bearer_context_p,
                          cm_call_mode_info_s_type  *mode_info_ptr);

    /*--------------------------------------------------------------------------
    This function is to perform any mode specific operations on Rab release
    indication.
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(rab_rel_ind))(
                          ds_bearer_context_s* bearer_context_p);

 
  /*---------------------------------------------------------------------------
    This function is used to get the bearer tech info from the bearer context
  ---------------------------------------------------------------------------*/
  int (*DS_BEARER_CTXT_VF_FPN(get_bearer_tech_type))(
                          ps_iface_bearer_technology_type    *bearer_tech_p,
                          sint15                             *ps_errno,
                          sys_modem_as_id_e_type              subs_id);

#ifdef FEATURE_DATA_WLAN_MAPCON
  /*---------------------------------------------------------------------------
    This function is used to get the bearer tech info from the bearer context
  ---------------------------------------------------------------------------*/
  int (*DS_BEARER_CTXT_VF_FPN(get_bearer_tech_type_ex))(
                          ds_sys_system_status_info_type    *bearer_tech_ex_p,
                          sys_modem_as_id_e_type             subs_id);

   /*---------------------------------------------------------------------------
    This function is used to get the SO mask info based on capability
  ---------------------------------------------------------------------------*/
  uint64 (*DS_BEARER_CTXT_VF_FPN(getSoMask))(
                          sys_modem_as_id_e_type             subs_id);
#endif /* FEATURE_DATA_WLAN_MAPCON */

  /*--------------------------------------------------------------------------
    Mode specific Suspend indication handler
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(suspend_ind))(                        
                        ds_bearer_context_s* bearer_context_p
                        );
  /*--------------------------------------------------------------------------
    Mode specific Resume indication handler
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(resume_ind))(ds_bearer_context_s* bearer_context_p);

  /*--------------------------------------------------------------------------
    Mode specific Resume wm flushing handler
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(empty_watermark))(ds_bearer_context_s* bearer_context_p);

  /*--------------------------------------------------------------------------
    This function is used to report data activity on each bearer. The mode
    specific function is dispatched which in turn queries the corresponding
    lower layers to figure out the amount of data tx or rx
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(get_data_trfr_activity))(
                          ds_bearer_context_s     *bearer_context_p);

 /*-------------------------------------------------------------------------
   This function is used to get the bearer rate
  --------------------------------------------------------------------------*/
  int (*DS_BEARER_CTXT_VF_FPN(get_bearer_rate))(
                          uint32                   call_inst,
                          void                    *argval_ptr,
                          sint15                  *ps_errno,
                          sys_modem_as_id_e_type   subs_id);
   
  /*---------------------------------------------------------------------------
    This function gets the timer value that should be used to attempt re-estab
    of DRB
  ---------------------------------------------------------------------------*/
  boolean (*DS_BEARER_CTXT_VF_FPN(examine_if_throt_needed))(
                              dsgwl_ps_call_info_u_type *call_info_ptr,
                              sys_modem_as_id_e_type subs_id);

  /*-------------------------------------------------------------------------
    These functions are used to transfer contexts across during LTE-GW interRAT.
  -------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(cntxt_transfer_ind_setup))(
                                   ds_bearer_context_s*    bearer_context_p);

  void (*DS_BEARER_CTXT_VF_FPN(cntxt_transfer_ind_release))(
                                   ds_bearer_context_s*    bearer_context_p);

  void (*DS_BEARER_CTXT_VF_FPN(dereg_pdcp))(
                                   ds_bearer_context_s*    bearer_context_p);

  void (*DS_BEARER_CTXT_VF_FPN(resume_validate))(
                                     sys_sys_mode_e_type           new_mode,
                                     sys_sys_mode_e_type           old_mode,
                                     sys_modem_as_id_e_type        subs_id);

  boolean (*DS_BEARER_CTXT_VF_FPN(free_umts_bearer))(
                                  ds_bearer_context_s*    bearer_context_p);

  boolean (*DS_BEARER_CTXT_VF_FPN(allocate_umts_bearer))(
                                  int    loop_index);

  boolean (*DS_BEARER_CTXT_VF_FPN(allocate_eps_bearer))(
                                  int    index);

  boolean (*DS_BEARER_CTXT_VF_FPN(free_eps_bearer))
                                    (ds_bearer_context_s*    bearer_context_p);

  ps_iface_type* (*DS_BEARER_CTXT_VF_FPN(complogfetchiface))(
                           ps_comp_logging_instance_config_s *logging_info_ptr,
                           ip_version_enum_type ip_version);

#ifdef FEATURE_QXDM_WM_TRACING
  void (*DS_BEARER_CTXT_VF_FPN(report_wm_stats)) (
                        uint32 call_instance,
                        fc_wm_stat_type *wm_stat_ptr
                       );
#endif /* FEATURE_QXDM_WM_TRACING */
  uint32 (*DS_BEARER_CTXT_VF_FPN(getratMaskforHs))(
                         sys_hs_ind_e_type             hs_status,
                         sys_modem_as_id_e_type        subs_id);

  uint32 (*DS_BEARER_CTXT_VF_FPN(getratMaskforEgprsSupport))(
                         sys_egprs_support_e_type      egprs_support,
                         sys_modem_as_id_e_type        subs_id);

  cm_cell_srv_ind_s_type 
         (*DS_BEARER_CTXT_VF_FPN(getCellSrvInd))(
                         sys_modem_as_id_e_type  subs_id);

  void   (*DS_BEARER_CTXT_VF_FPN(setCellSrvInd))(
                         cm_cell_srv_ind_s_type  cell_srv_ind,
                         sys_modem_as_id_e_type  subs_id);

#ifdef FEATURE_WCDMA
  void	 (*DS_BEARER_CTXT_VF_FPN(purgecmdcbhdr))(
  	        ds_bearer_context_purge_cnf_ev_type *ds3gpp_purge_cnf_ev);
#endif /*FEATURE_WCDMA*/
  sys_div_duplex_e_type
         (*DS_BEARER_CTXT_VF_FPN(get_div_duplex))(
                         sys_modem_as_id_e_type  subs_id);

  void   (*DS_BEARER_CTXT_VF_FPN(set_div_duplex))(
                         sys_div_duplex_e_type   div_duplex,
                         sys_modem_as_id_e_type  subs_id);

  boolean(*DS_BEARER_CTXT_VF_FPN(refresh_actual_throughput))
                                (ds_bearer_context_s       *bearer_context_p,
                                 sys_modem_as_id_e_type     subs_id,
                                 uint32                     sampling_interval);

  void (*DS_BEARER_CTXT_VF_FPN (init_bearer_throughput_stats))
                                (ds_bearer_context_s       *bearer_context_p);
/*--------------------------------------------------------------------------
    This function is to perform any mode specific operations on mode_pref_change.
  ---------------------------------------------------------------------------*/
  void (*DS_BEARER_CTXT_VF_FPN(mode_pref_change))(                          
                          ds_bearer_context_s* bearer_context_p);
} ds_bearer_context_ftbl_s;

/*----------------------------------------------------------------------------
  EPS/umts specific function tables. We allocate memory for SYS_SYS_MODE_MAX
  table. However, we will be using only those corresponding to WCDMA, GPRS
  and EPS.
----------------------------------------------------------------------------*/
extern ds_bearer_context_ftbl_s      ds_bearer_context_ftbl[DS_3GPPI_SYS_MODE_MAX];


#define DS_3GPPI_VFR_CALL_PER_BEARER(ftbl, rval, func, bearer_context_p, ...) \
        {                                                                     \
          sys_sys_mode_e_type mode;                                           \
          int index;                                                          \
                                                                              \
          mode = (sys_sys_mode_e_type)bearer_context_p->                      \
          ds_bearer_context_dyn_p->call_mode;                                 \
          if (mode < SYS_SYS_MODE_MAX && mode > SYS_SYS_MODE_NONE) {          \
            index = DS_3GPPI_MODE_INDEX(mode);                                \
                                                                              \
            ASSERT(DS_3GPPI_VERIFY_MODE_INDEX(index));                        \
            if (ftbl[index].DS_3GPPI_VF_FPN(func)) {                          \
              rval = ftbl[index].DS_3GPPI_VF_FPN(func)( bearer_context_p,     \
                                                        __VA_ARGS__ );        \
            } else {                                                          \
              DS_3GPP_MSG3_HIGH(                                              \
                "DS_3GPPI_VFR_CALL: No reg fnc in tbl %x for index %d",       \
                ftbl, index, 0);                                              \
            }                                                                 \
          } else {                                                            \
            DS_3GPP_MSG3_HIGH("Invalid mode %x",mode,0,0);                    \
          }                                                                   \
        }

#define DS_3GPPI_VF_CALL_PER_BEARER(ftbl, func, bearer_context_p, ...)        \
        {                                                                     \
          sys_sys_mode_e_type mode;                                           \
          int index;                                                          \
                                                                              \
          mode = (sys_sys_mode_e_type)bearer_context_p->                      \
          ds_bearer_context_dyn_p->call_mode;                                 \
          if (mode < SYS_SYS_MODE_MAX && mode > SYS_SYS_MODE_NONE) {          \
            index = DS_3GPPI_MODE_INDEX(mode);                                \
                                                                              \
            ASSERT(DS_3GPPI_VERIFY_MODE_INDEX(index));                        \
            if (ftbl[index].DS_3GPPI_VF_FPN(func)) {                          \
              ftbl[index].DS_3GPPI_VF_FPN(func)( bearer_context_p,            \
                                                 __VA_ARGS__ );               \
            } else {                                                          \
              DS_3GPP_MSG3_HIGH(                                              \
                "DS_3GPPI_VF_CALL: No reg fnc in tbl %x for index %d",        \
                ftbl, index, 0);                                              \
            }                                                                 \
          } else {                                                            \
            DS_3GPP_MSG3_HIGH(                                                \
                  "DS_3GPPI_VF_CALL:Invalid mode %x",mode,0,0);               \
          }                                                                   \
        }

#define DS_3GPPI_VF_CALL_NO_ARG_PER_BEARER(ftbl, func, bearer_context_p)      \
        {                                                                     \
          sys_sys_mode_e_type mode;                                           \
          int index;                                                          \
                                                                              \
          mode = (sys_sys_mode_e_type) bearer_context_p->                     \
          ds_bearer_context_dyn_p->call_mode;                                 \
          if(mode < SYS_SYS_MODE_MAX && mode > SYS_SYS_MODE_NONE) {           \
            index = DS_3GPPI_MODE_INDEX(mode);                                \
                                                                              \
            ASSERT(DS_3GPPI_VERIFY_MODE_INDEX(index));                        \
            if (ftbl[index].DS_3GPPI_VF_FPN(func)) {                          \
              ftbl[index].DS_3GPPI_VF_FPN(func)(bearer_context_p);            \
            } else {                                                          \
              DS_3GPP_MSG3_HIGH(                                              \
                "DS_3GPPI_VF_CALL: No reg fnc in tbl %x for index %d",        \
                ftbl, index, 0);                                              \
            }                                                                 \
          } else {                                                            \
            DS_3GPP_MSG3_HIGH("Invalid mode %x",mode,0,0);                    \
          }                                                                   \
        }


/*-----------------------------------------------------------------------------
  Macro to start the timer with a given value. It also sets the timer_running
  flag
-----------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_SET_TIMER(hndl, time_value)\
  {\
    (void)rex_set_timer(&(hndl->tmr_rab_reestab), time_value);\
    hndl->timer_running = TRUE;\
  }

/*-----------------------------------------------------------------------------
  Macro to stop the timer. It also resets the timer_running flag
-----------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_CLR_TIMER(hndl)\
  {\
    (void)rex_clr_timer(&(hndl->tmr_rab_reestab));\
     hndl->timer_running = FALSE;\
  }

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*-----------------------------------------------------------------------------
  This macro is used to set the field of bitmask that represents the bearer
  passed in to it.
-----------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_SET_BITMASK(hndl, bearer_context_p)\
  {\
    hndl->uplink_data_bitmask = \
  	hndl->uplink_data_bitmask | ((uint32)0x1 << (uint32)bearer_context_p->\
    ds_bearer_context_dyn_p->client_data_ptr);\
  }

/*-----------------------------------------------------------------------------
  This macro is used to unset the field of bitmask that represents the bearer
  passed in to it.
-----------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_UNSET_BITMASK(hndl, bearer_context_p)\
  {\
    hndl->uplink_data_bitmask = \
  	hndl->uplink_data_bitmask & ~((uint32)0x1 << (uint32)bearer_context_p->\
    ds_bearer_context_dyn_p->client_data_ptr);\
  }

/*-----------------------------------------------------------------------------
  This macro is used to query the field of bitmask that represents the bearer
  passed in to it.
-----------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_QUERY_BITMASK(hndl, bearer_context_p)\
    ((hndl->uplink_data_bitmask & \
     ((uint32)0x1 << (uint32)bearer_context_p->ds_bearer_context_dyn_p->\
       client_data_ptr)) != 0)?TRUE:FALSE;

/*---------------------------------------------------------------------------
  These macros are used for incrementing and resetting the rab_reestab_retry
  counter for a particular bearer
---------------------------------------------------------------------------*/
#define DS_BEARER_CNTXT_INC_RAB_REESTAB_RETRY_CNT(bearer_context_p)\
        bearer_context_p->ds_bearer_context_dyn_p->rab_reestab_retry_cnt++;

#define DS_BEARER_CNTXT_RESET_RAB_REESTAB_RETRY_CNT(bearer_context_p)\
        bearer_context_p->ds_bearer_context_dyn_p->rab_reestab_retry_cnt = 0;

/*===========================================================================
FUNCTION ds_bearer_cntxt_get_subs_id_from_bearer_internal

DESCRIPTION
  This function fetches the subs id to which the bearer belongs. There will
  be no F3 if bearer context is invalid
 
PARAMETERS
  bearer_context_p       - bearer_context
  subs_id_p              - subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if successfully fetched subs Id
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntxt_get_subs_id_from_bearer_internal
(
  ds_bearer_context_s    *bearer_context_p,
  sys_modem_as_id_e_type *subs_id_p
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_get_subs_id_from_bearer

DESCRIPTION
  This function fetches the subs id from the PDN Cntxt to which the bearer
  belongs.

PARAMETERS
  bearer_context_p       - bearer_context
  subs_id_p              - subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if successfully fetched subs Id
  FALSE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntxt_get_subs_id_from_bearer
( 
  ds_bearer_context_s    *bearer_context_p,
  sys_modem_as_id_e_type *subs_id_p
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_INST_TO_REORIG

DESCRIPTION
  This function is used to get a bearer instance on which reorigination is
  to be requested. This is used in the timer_expired command processing to
  get a bearer to work on.

  This uses the bit mask to determine a bearer that is still waiting on RAB
  reestablishment and returns it to the caller.

PARAMETERS
  Subs Id: Subscription Id

DEPENDENCIES
  Depends on whether the uplink_data_bitmask is null or non-null

RETURN VALUE
  Pointer to the bearer context awaiting RAB reestab if bitmask is non null
  NULL if bitmask is NULL

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_inst_to_reorig
(
  sys_modem_as_id_e_type subs_id
);

#if 0 //Not used anywhere. Have to evaluate if this is needed

ds_umts_umts_qos_params_type* ds_bearer_cntxt_get_umts_qos_val(ds_bearer_context_s*);

ds_umts_gprs_qos_params_type* ds_bearer_cntxt_get_gprs_qos_val(ds_bearer_context_s*);

#endif

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_3GPP_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each bearer context and initializes the
  parameters for each bearer context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_3gpp_init( void );

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_DATA_TRFR_ACTIVITY

DESCRIPTION
  This function is used to find out if any data was transferred in the UL
  or DL direction in the previous polling period.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
   TRUE - if any data was Tx or Rx in the polling interval
   FALSE - otherwise
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_bearer_cntxt_get_data_trfr_activity
(
  ds_bearer_context_s  *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_FREE_BEARER_INST

DESCRIPTION
  This function cleans up the bearer instance on a call end.

PARAMETERS
  IN
    bearer_context_p - Pointer to the bearer context to be cleaned up.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_free_bearer_inst
(
  ds_bearer_context_s* bearer_context_p
);


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_REGISTER_PDN_CONTEXT

DESCRIPTION
  This fucntion is called by PDN context to register itself with the bearers
  associated with it. This function attaches the PDN, bearer contexts and the
  phys links associated with them. The bearer callbacks to the PDN context is
  also stored in the bearer context. This function is called per bearer.

PARAMETERS
  IN
    pdn_context_p     - pointer to the pdn context stored as void* in bearer.
    bearer_context_p  - pointer to bearer context
    pdn_context_cb_tbl_p
                      - pointer to the call call back table. Stored in bearer
		        to be called later when bearer goes up / down
    is_default        - Boolean that tells if the bearer is default or not.
DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_register_pdn_context( 
  void                             *pdn_context_ptr,
  ds_bearer_context_s              *bearer_context_p,
  boolean                          is_default
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PHYS_LINK_DOWN_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to a DS_BEARER_CNTXT_PHYS_LINK_DOWN 
  command on the UMTS/EPS iface. From the new implementation, it looks like
  this function need not be handled. There is no one who calls PHYS_LINK_DOWN
  Even the function body has been left empty for now. This function will be 
  removed if it is deemed unnecessary.

PARAMETERS
  call_instance - dummy variable.
  info_ptr      - void pointer

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_phys_link_down_cmd_hdlr
(
  uint32 call_instance,
  void *info_ptr
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PHYS_LINK_UP_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to a phys up command on
  the UMTS iface

  This function, for now, only handles the reorigination from dormancy.

PARAMETERS
  call_instance  - call index to be able to locate the bearer
  info_ptr       - Not used.

DEPENDENCIES
  Called in DS TASK context.  Assumes that PS_IFACE has already
  set phys_link state to COMING_UP.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_phys_link_up_cmd_hdlr
(
  uint32      call_instance,
  void        *info_ptr
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PHYS_LINK_FLOW_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to DS3GPP_PHYS_LINK_FLOW_ENABLE_CMD
  or DS3GPP_PHYS_LINK_FLOW_DISABLE_CMD. Need to flow control the phys link
  appropriately.

PARAMETERS
   cmd_ptr - information relating to the DS comand.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_phys_link_flow_cmd_hdlr
(
  const ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_INVOKE_BEARER_BRIGUP

DESCRIPTION
  This function calles the bearer bring_up function. It also cleans up the
  bearer and calls bearer_down_ind_cb() if bringup fails.

PARAMETERS
  call_instance  - call index to be able to locate the bearer
  info_ptr       - ptr to network_params_info_type

DEPENDENCIES
  Assumes bearer is already allocated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_invoke_bearer_bringup
(
  uint32              bearer_instance,
  void                *info_ptr
);
/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CONTEXT_REFRESH_ACTUAL_THROUGHPUT

DESCRIPTION
  This utility function invokes the mode specific functions to refresh the
  current actual UL throughput, based on the current system mode
 

PARAMETERS
  sys_modem_as_id_e_type  - subscription ID 
  uint32                  - sampling interval to calculate the actual
                            throughput rate
DEPENDENCIES
  Assumes bearer is already allocated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_bearer_context_refresh_actual_throughput
(
  sys_modem_as_id_e_type    cm_subs_id,
  uint32                    sampling_interval
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_IS_DEFAULT

DESCRIPTION
  This function is an accessor function that tells if a given bearer context
  is default or not.

PARAMETERS
  IN
    bearer_context_p - pointer to the bearer context that we are interested in

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the bearer is default
  FALSE if the bearer is not default.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_bearer_cntxt_is_default(ds_bearer_context_s* bearer_context_p);
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_IS_DEFAULT_CALL_ID
DESCRIPTION
  This function is an accessor function that tells if a given call id maps to 
  default bearer or not.

PARAMETERS
    call ID  - call ID  that we are interested in

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the bearer is default
  FALSE if the bearer is not default.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_is_default_call_id(cm_call_id_type   call_id);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_FROM_CALL_ID

DESCRIPTION
  This function is used to find the bearer context, given a CM call id

PARAMETERS
  IN
    cm_call_id_type - CM call ID

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the bearer context if there is a CM call ID match
  NULL if there is no match

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_from_call_id(cm_call_id_type call_id);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_CNTXT

DESCRIPTION
  This function is used to get the bearer context from the call info. It
  gets the call instance from the call info and returns a pointer to the
  bearer context.

PARAMETERS
  IN
    const void* mode_specific_info_p - This is type cast into the call info

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the bearer context if the call id is valid.
  NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_cntxt(const void* mode_specific_info_p);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GETPDNCNTXT

DESCRIPTION
  This function is used to get the PDN context from the bearer context.

PARAMETERS
  IN
    bearer_context_p - Pointer to the bearer context.

DEPENDENCIES
  None.

RETURN VALUE
  Void* - Pointer to the PDN context on Success
          NULL on failure

SIDE EFFECTS
  None.

===========================================================================*/
void* ds_bearer_cntxt_get_pdn_cntxt(ds_bearer_context_s* bearer_context_p);

/*=============================================================================
FUNCTION DS_BEARER_CNTXT_SET_RX_SIG

DESCRIPTION
  This function is used to set the RX signal on a particular bearer

PARAMETERS
  IN
    uint32 - This is the bearer instance that is needed to identify the bearer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void ds_bearer_cntxt_set_rx_sig(uint32 bearer_instance);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_TEARDOWN

DESCRIPTION
  This function is used to bring down a call on a bearer. The iface down cmd
  is processed in the PDN context and in turn calls this function. 

PARAMETERS
  bearer_context_p - pointer to the bearer on which call is to be ended
  info_ptr         - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS (TRUE) on Successful call hang up with CM
  DS3G_FAILURE (FALSE) on any failure

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_tear_down(ds_bearer_context_s* bearer_cntx_p, void *info_ptr);

/*===========================================================================
FUNCTION  DS_3GPP_REPORT_BEARER_THROUGHPUT_STATS_FOR_PDN

DESCRIPTION 
 This utility function reports the bearer throughput statistics for all the
 bearers associated with the given PDN

PARAMETERS 
  
  ds_pdn_context_s*                        - Pointer to PDN context 
 
  ds_sys_bearer_throughput_info_list_type* - Pointer to bearer_info_list
 
DEPENDENCIES 
  This utility function will be used only when
  * There is atleast one bearer associated with the PDN
        &
  * Throughput Info Indication timer is running
 
RETURN VALUE
  TRUE   - If the bearer throughput stats is reported successfully for
           the Input PDN
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds_3gpp_report_bearer_throughput_stats_for_pdn
(
   ds_pdn_context_s                          *pdn_context_p,
   ds_sys_bearer_throughput_info_list_type   *bearer_info_list_p
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PROCESSPSCMD

DESCRIPTION
  This function processes the bearer context specific PS commands received 
  by the DS task

PARAMETERS
  IN
    const ds_cmd_type* - Pointer to the command buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_process_ps_cmd(const ds_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ISIMCNCALLUP

DESCRIPTION
  This fn is used during call orig to find out if there is already a call
  with the IM_CN_subsystem flag set to TRUE

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the IMCN call is up, False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_is_IMCN_call_up
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_GET_IMCN_SUBSYSTEM_FLAG_FOR_IFACE

DESCRIPTION
   This function returns the value of IM CN subsystem flag for an iface. It
   looks up the call table to get the IM CN Subsystem flag.
   
PARAMETERS   :
  iface_ptr      - Pointer to iface on which query is made
  im_cn_status   - boolean pointer to return the status of IM CN Flag.
  ps_errno       - Error if any
  
DEPENDENCIES
  None.

RETURN VALUE
   0  - If the IM CN flag is successfully retrieved.
   -1 - If the iface is not valid.

SIDE EFFECTS
  None.

===========================================================================*/
int ds_bearer_cntxt_get_IMCN_subsystem_flag_for_iface
(
  const ps_iface_type      *iface_ptr,
  boolean                  *im_cn_status,
  sint15                   *ps_errno
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ALLOC

DESCRIPTION
  This function marks a bearer context to be in use by allocating a CM call
  id to it. There is no other processing done here. This is mainly used for
  MT calls in UMTS and in the default bearer activation in EPS.

PARAMETERS
  IN
    ds_bearer_context_s* - Pointer to the bearer that has to be allocated.
    cm_call_id_type      - This is the call id that is to be stored in bearer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_alloc(ds_bearer_context_s*,cm_call_id_type);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_INCOMINGCALL

DESCRIPTION
  This function results in a transition of the bearer context state.
  This is called from 3gpp handler if USER_ALERT is given to it by
  PDN context.

PARAMETERS
  IN
    ds_bearer_context_s* - The bearer whose state is to be changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_incoming_call(ds_bearer_context_s* bearer_context_p);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ANSWERCALL

DESCRIPTION
  This function is used to answer an incoming call. This is the point of
  convergence of the two different paths that an MT call could take (Auto
  answer and Application answer).

PARAMETERS
  IN
    ds_bearer_context_s* - The bearer on which the incoming call is to be
                           answered.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if the call was initiated successfully or if hang up was
                 initiated successfully on a failed call initiation
  DS3G_FAILURE - if the hang up was unsuccessful. Local clean up happens in
                 this case.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_answer_call(ds_bearer_context_s* bearer_context_p);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_UPDATECALLID 

DESCRIPTION
  This function finds the bearer context based on the call info pointer
  passed into it and updates it with the call id. This function is called
  after we initiate a call with CM. CM gets back to 3gpp handler with
  call_id handler and this function is inturn called by 3gpp hdlr to
  update the bearer with the CM call id

PARAMETERS
  IN
    mode_specific_info_p - call info to identify the bearer context
    call_id              - CM call id to update the bearer with

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the bearer was found and the call Id was stored in it.
  FALSE if the bearer could not be found

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntxt_update_call_id
(
  const void      *mode_specific_info_p,
  cm_call_id_type call_id
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CONTEXT_INIT_BEARER_THROUGHPUT_STATS

DESCRIPTION
  This function initializes the bearer throughput statistics parameters
  for all active bearer contexts. This will be required to get a better
  estimate of the actual throughput rates on all active bearers
 
PARAMETERS  
  subs_id: Subscription Id
    
DEPENDENCIES 
  The bearer throughput stats are initialized before the timer that is
  used to refresh actual throughout timer starts.
  
RETURN VALUE 
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_bearer_context_init_bearer_throughput_stats
(
  sys_modem_as_id_e_type subs_id
);
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_STORE_CALL_ID 

DESCRIPTION
  This function stores the CM call id in the bearer context struct pointed to
  by bearer_context_p

PARAMETERS
  IN
    bearer_context_p     - Pointer to the bearer context
    call_id              - CM call id to update the bearer with

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_store_call_id
(
  ds_bearer_context_s *bearer_context_p,
  cm_call_id_type     call_id
);


/*===========================================================================
FUNCTION DSWCDMABEARERCNTXT_PROC_REORIG_DORMTMR_EXP_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of dormancy timer expiry. This simply calls the function to reestab RAB.
      
PARAMETERS
  client_data_ptr: Subs Id passed as Client Data Ptr

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_proc_reorig_dormtmr_exp_cmd
(
  void* client_data_ptr 
);

/*===========================================================================
FUNCTION DSWCDMABEARERCNTXT_REORIG_DORM_TIMER_CB()

DESCRIPTION
  This function is called when the call is in the dormant mode and the timer 
  to call the RAB reestablish expires. It puts a command to DS and exits.
      
PARAMETERS
   callback_data : Bearer context pointer registered with the timer

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_dorm_timer_cb
(
  void*              callback_data /* unused */
);


/*===========================================================================
FUNCTION  DSWCDMABEARERCNTXT_PROC_DORM_REORIG_CMD

DESCRIPTION
  This function is called when we process the command to reestablish RAB
  due to enqueuing data into the watermarks.

PARAMETERS
   *cmd_ptr - ds cmd ptr

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_cntxt_proc_dorm_reorig_cmd
(
  const ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION  ds_bearer_cntxt_proc_dereg_dpm_cmd

DESCRIPTION
  This function is called when we process the command to deregister with
  dpm as part of context transfer.

PARAMETERS   
  *cmd_ptr - ds cmd ptr

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_bearer_cntxt_proc_dereg_dpm_cmd
(
  const ds_cmd_type *cmd_ptr
);

/*===========================================================================
  FUNCTION DSWCDMABEARERCNTXT_REORIG_DORM

  DESCRIPTION
    This function is responsible for reoriginating the call from dormancy.
    This function ends up calling the DSMGR function for RAB reestab.
    The logic to check if there is a Rab-reestab request already pending is
    also present in this function.
  
  PARAMETERS   
   *bearer_context_p - Pointer to the bearer context that we are working on
   dsm_item_type*    -  Reorig packet pointer
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    DS3G_SUCCESS : Successful setup
    DS3G_FAILURE : Setup error
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_dorm
(
  ds_bearer_context_s* bearer_context_p,
  dsm_item_type*       reorig_pkt_ptr
);

/*===========================================================================
  FUNCTION DS_BEARER_CNTXT_REORIG_ALL_DORM_BEARERS

  DESCRIPTION
    This function is responsible for reoriginating all the dormant calls.
  
  PARAMETERS   
    None.
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_bearer_cntxt_reorig_all_dorm_bearers
(
  void
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_FROM_EPS_ID

DESCRIPTION
  This function is used to find the bearer context, given a EPS bearer id

PARAMETERS
  IN
    eps_bearer_id_T - eps bearer id
    subs_id         - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the bearer context if there is a EPS bearer ID match
  NULL if there is no match

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_from_eps_id
(
  eps_bearer_id_T        eps_bearer_id,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ALLOCATE_NEW_BEARER

DESCRIPTION
  This function is used to return a bearer from the pool of available bearer
  contexts. The allocation and deallocation details are completely hidden from
  the caller. We could change the implementation of this function at a later
  point and make bearer allocations completely dynamic instead of allocating
  a free bearer from the pool of statically created bearers.

PARAMETERS
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_context_s* - This is the pointer to the bearer context.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_allocate_new_bearer
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_DEREGISTER_PDN_CONTEXT

DESCRIPTION
  This fucntion is called by PDN context to deregister itself with the bearer
  associated with it. This function detaches the PDN, bearer contexts and the
  phys links associated with them. This function is called per bearer.

PARAMETERS
  IN
    bearer_context_p  - pointer to bearer context that is to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_deregister_pdn_context
( 
  ds_bearer_context_s              *bearer_context_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_RETURN_BEARER_TO_FREE_POOL

DESCRIPTION
  This function is used to return a bearer to the pool of available bearer
  contexts. This function just resets the in_use flag inside the bearer.

PARAMETERS
  IN
    bearer_context_p - pointer to the bearer to be marked as free.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_return_bearer_to_free_pool
(
  ds_bearer_context_s              *bearer_context_ptr     
);

/*===========================================================================
FUNCTION      DS_BEARER_CNTXT_PHYS_LINK_DOWN

DESCRIPTION
  Called by the embedded application.  Brings down the EPS/UMTS traffic 
  channel. It is also called on IFace Down command.

PARAMETERS
  phys_link_ptr - phys link ptr for the call
  info_ptr      - not used

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int ds_bearer_cntxt_phys_link_down
(
  ps_phys_link_type *phys_link_ptr_i,
  void              *info_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_BEARER_FROM_SDF_ID

DESCRIPTION
  This function is used to fetch the bearer context using the SDF Id.
  
PARAMETERS
  sdf_id - SDF ID to use to locate the bearer

DEPENDENCIES
  None.

RETURN VALUE
 bearer_context_p  - Pointer to Bearer context
 NULL if the bearer cannot be located

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_bearer_from_sdf_id(uint16 sdf_id);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_SET_BEARER_STATE

DESCRIPTION
  This function is used to set the bearer to a particular state
  
PARAMETERS
  bearer_context_p  - Pointer to Bearer context
  state             - State to set the bearer context to

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_set_bearer_state
(
  ds_bearer_context_s* bearer_context_p,
  ds_bearer_context_state_e state
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_CALL_RAB_RE_ESTAB_IND_HDLR

DESCRIPTION
  This function handles rab reject indication for 3GPP modes. This will in
  turn dispatch any mode specific functions
  
PARAMETERS  
  call_id         -   CM call id
  mode_info_ptr   -   mode specific call info
  end_params      -   Call end param
  subs_id         -   Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_re_estab_ind_hdlr
(
  cm_call_id_type            call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_CALL_RAB_RE_ESTAB_REJ_HDLR

DESCRIPTION
  This function handles rab reject indication for 3GPP modes
  
PARAMETERS  
  call_id     -   CM call id
  call_info   -   mode specific call info
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_re_estab_rej_hdlr
(
  cm_call_id_type            call_id,
  dsgwl_ps_call_info_u_type *call_info
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_CALL_RAB_RE_ESTAB_FAIL_HDLR

DESCRIPTION
  This function is called by 3G DSManager when the RAB re-establishment
  failure event is received.
  RABM signalling procedures succeeded but NO RAB was established. Spec
  does not have any procedures to recover from this. So go ahead and
  start call release now by returning an error: 3G DS manager will initiate
  call release when this fn returns an error.

PARAMETERS   
  call_id,        - Call ID 
  *end_params     - CM specific call end parameters
  subs_id         - Subscription Id
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS : RAB reject handled correctly
  DS3G_FAILURE : RAB reject cannot be handled 
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_re_estab_fail_hdlr
(
  cm_call_id_type            call_id,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_CALL_RAB_RELEASE_HDLR

DESCRIPTION
  This function handles rab release procedure for 3GPP modes.
  
PARAMETERS  
  call_id    - CM call id
  end_param  - call end parameters
  subs_id    - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_call_rab_release_hdlr
(
  cm_call_id_type            call_id,
  cm_end_params_s_type      *end_params,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_REORIG_DORM_CALL_CB

DESCRIPTION
  This function sends a command to DS task to trigger dormant re-orig. Called
  when data is enqueued into the Um Tx watermark. Since it runs in PS task,
  we are queuing a command to DS.
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/
void ds_bearer_cntxt_reorig_dorm_call_cb
(
  dsm_watermark_type *wm_p,
  void               *call_back_data
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_ACTIVATE_CNF

DESCRIPTION
  This is the function that is called from 3gpp_hdlr on call connected.
  This in turn invoked GPRS or WCDMA bearer level functions.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  mode_info_p       - mode info params
  end_params_p      - call end parameters
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.

===========================================================================*/
boolean 
ds_bearer_cntxt_activate_cnf
(
  ds_bearer_context_s             *bearer_context_p,
  const cm_call_mode_info_s_type  *mode_info_p,
  cm_end_params_s_type            *end_parms_p,
  sys_modem_as_id_e_type           subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTX_GET_BEARER_TYPE

DESCRIPTION
  This function is used to get the underlying bearer type. This makes a call
  to the mode specific bearer function and populates the incoming param.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  bearer_type_p     - bearer_type structure that has to be filled in
  ps_errno          - error code to be returned to PS in case of failure.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
   Integer value is returned.
   0 - SUCCESS
  -1 - FAILURE
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_3gpp_bearer_cntxt_get_bearer_type
(
  ds_bearer_context_s              *bearer_context_p,
  ps_iface_bearer_technology_type  *bearer_type_p,
  sint15                           *ps_errno
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTX_GET_BEARER_TYPE_EX

DESCRIPTION
  This function is used to get the underlying bearer type. 
  
PARAMETERS
  network_mode      - network sys mode
  bearer_type_ex_p  - bearer_type structure that has to be filled in
  ps_errno          - error code to be returned to PS in case of failure
  subs_id           - subscription id

DEPENDENCIES 
  None.
  
RETURN VALUE 
   Integer value is returned.
   0 - SUCCESS
  -1 - FAILURE
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
int ds_3gpp_bearer_cntxt_get_bearer_type_ex
(
  sys_sys_mode_e_type              network_mode,
  ds_sys_system_status_info_type  *bearer_type_ex_p,
  sint15                          *ps_errno,
  sys_modem_as_id_e_type           subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTX_GET_SO_MASK

DESCRIPTION
  This function is used to fetch the SO mask info based on
  capability information such hsdpa_call_status for WCDMA or 
  egprs_support for GSM. 
  
PARAMETERS  
  subs_id: Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  SO mask
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint64 ds_3gpp_bearer_cntxt_get_so_mask
(
  sys_modem_as_id_e_type  subs_id
);
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*==========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_BEARER_RATE

DESCRIPTION This function returns the tx and rx rates of current call's
            channel.
    
PARAMETERS
   call_inst        - index into the call table
  *argval_ptr       - Bearer rate info will be put in here
  *ps_errno         - Ptr to error code (if any)
   subs_id          - Subscription Id
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE

SIDE EFFECTS 
  None
===========================================================================*/
int ds_3gpp_bearer_cntxt_get_bearer_rate
(
  uint32                   call_inst,
  void                    *argval_ptr,
  sint15                  *ps_errno,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_EXAMINE_IF_THROT_NEEDED

DESCRIPTION
  This function is used to examine the cause code of rejection and determine
  if we need to perform throttling. This function in turn dispatches the mode
  specific handler to figure out if throttling is needed

PARAMETERS
  call_info_ptr  - Pointer to the call info to find out rejection cause
  subs_id        - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if throttling is needed
  FALSE otherwise. 
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_examine_if_throt_needed
(
  dsgwl_ps_call_info_u_type *call_info_ptr,
  sys_modem_as_id_e_type     subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_GETORIGPARAMS

DESCRIPTION
  Populates applicable call origination params in ds_3gpp_orig_params_s, 
  including QOS and default (match all) TFT for default bearer. 
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  mm_orig_params_p  - orig params
  ds3gi_call_info   - call info
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_get_orig_params
(
  ds_bearer_context_s              *bearer_context_p,
  ds_3gpp_orig_params_s            *mm_orig_params_p,                          
  const ds_3gpp_pdn_call_info_type *ds3gi_call_info
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_GETENDPARAMS

DESCRIPTION
  Populates call end params to use for bringing down the data call, 
  including reason code.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  end_params_p      - call end parameters
  ds3gi_call_info   - call info
  delay_disconnect  - disconnect to be delayed or not
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void 
ds_bearer_cntxt_get_end_params
(
  ds_bearer_context_s              *bearer_context_p,
  cm_end_params_s_type             *end_params,
  ds_3gpp_pdn_call_info_type       *ds3gi_call_info,
  boolean                          *delay_disconnect
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_DEACTIVATECNF

DESCRIPTION
   Called to indicate deactivate of Bearer Context. Tears down the data path
   including deregistration with lower layers if needed, and changes state to
   DOWN. Notifies PdnContext of state change. This is a virtual function and 
   calls mode specific function.
  
PARAMETERS  
  bearer_context_p   - bearer context obj ptr
  call_info_p        - DSMGR input which also contains CM mode info   
  end_type           - 0:regular,1:abort,2:reject
  call_was_connected - was call connected
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void 
ds_bearer_cntxt_deactivate_cnf
(
  ds_bearer_context_s            *bearer_context_p,
  const ds3g_call_info_cmd_type  *call_info_p,
  uint8                           end_type,
  boolean                         call_was_connected
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_DORMANCYREORIG

DESCRIPTION
  Dormancy reorig actions happen here. This function pointer could be set to
  NULL if dormancy is not supported in some modes.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void 
ds_bearer_cntxt_dormancy_reorig
(
  ds_bearer_context_s        *bearer_context_p
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_BEARERACTIVATEIND

DESCRIPTION
  Called to indicate berarer activation request indication.
  
PARAMETERS  
  call_id           - call ID
  mode_info_ptr_i   - mode info ptr
  subs_id           - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_bearer_activate_ind
(
  cm_call_id_type                  call_id,
  const cm_call_mode_info_s_type   *mode_info_ptr_i,
  sys_modem_as_id_e_type           subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CONTEXT_GO_NULL

DESCRIPTION
  This function is used to cleanup the bearer. It calls the mode specific bearer
  to clean itself up.
  
PARAMETERS  
  bearer_cntx_p      - bearer context obj ptr
  down_reason        - reason for bearer deactivation
  call_was_connected - whether the call was connected or not.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
 None 
===========================================================================*/
void
ds_3gpp_bearer_cntxt_go_null
(
  ds_bearer_context_s * bearer_cntx_p,
  ds_3gpp_down_reason_t down_reason,
  boolean               call_was_connected
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_BRING_UP

DESCRIPTION
  This function is used to initiate a call. Since the CM API is different 
  for LTE and UMTS, this function pointer will be used to invoke the mode
  specific function to initiate a call in the corresponding mode.
  
PARAMETERS  
  info_p            - bearer context obj ptr
  call_info         - mode info params
  useDefaultDialStr - whether to use default dial string
  subs_id           - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_bring_up
(
  void                       *info_p,
  ds_3gpp_pdn_call_info_type *call_info,
  boolean                     useDefaultDialStr,
  sys_modem_as_id_e_type      subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_REDIAL

DESCRIPTION
  This function is used to redial a call. Since the CM API is different 
  for LTE and UMTS, this function pointer will be used to invoke the mode
  specific function to initiate a call in the corresponding mode.
  
PARAMETERS    
  call_info         - mode info params  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_redial
(  
  ds_bearer_context_s           *bearer_context_p 
);
/*===========================================================================
FUNCTION ds_bearer_cntxt_TEAR_DOWN

DESCRIPTION
  This function is used to initiate the tear down of a call. Since the APIs
  are different in UMTS and LTE modes, we need mode specific handlers. These
  function pointers are initialized in the corresponding mode specific files
  and are accessed while processing phys_link_down command
 
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
  call_info         - call info params
  info_ptr          - void pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_tear_down_call
(
  ds_bearer_context_s        *bearer_context_p,
  ds_3gpp_pdn_call_info_type *call_info,
  void                       *info_ptr
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed HS status.

PARAMETERS
  hs_status - the HS status
  subs_id   - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  RAT mask 

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32 ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status
(
  sys_hs_ind_e_type       hs_status,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_cell_srv_ind

DESCRIPTION
  This function returns the cached cell srv ind.

PARAMETERS
  subs_id      - subscription id

DEPENDENCIES
  None.

RETURN VALUE
  cell srv ind 

SIDE EFFECTS
  None.
  
===========================================================================*/
cm_cell_srv_ind_s_type ds_3gpp_bearer_cntxt_get_cell_srv_ind
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_cell_srv_ind

DESCRIPTION
  This function sets the cached cell srv ind.

PARAMETERS  
  cell_srv_ind - passed cell srv ind
  subs_id      - subscription id

DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_cell_srv_ind
(
  cm_cell_srv_ind_s_type  cell_srv_ind,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================

FUNCTION ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support

DESCRIPTION
  This function returns the RAT MASK corresponding to the passed egprs support.

PARAMETERS
  egrprs_support - egprs supported or not
 
DEPENDENCIES
  None.

RETURN VALUE
  RAT mask

SIDE EFFECTS
  None.
  
===========================================================================*/
uint32 ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support
(
  sys_egprs_support_e_type  egprs_support,
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_get_prev_div_duplex

DESCRIPTION
  This function returns the previous lte div duplex reported by CM.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Last div duplex reported by CM.

SIDE EFFECTS
  None.
===========================================================================*/
sys_div_duplex_e_type ds_3gpp_bearer_cntxt_get_prev_div_duplex
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_prev_div_duplex

DESCRIPTION
  This function returns the previous lte div duplex reported by CM.

PARAMETERS
  div_duplex - div_duplex value
  subs_id - subscription id
  
DEPENDENCIES
  None.

RETURN VALUE
  Last lte div duplex reported by CM.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_set_prev_div_duplex
( 
  sys_div_duplex_e_type   div_duplex,
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_RAB_RE_ESTAB_IND

DESCRIPTION
  This function is to perform any mode specific operations on Rab re-estab
  indication.
  
PARAMETERS  
  bearer_context_p  - bearer context obj ptr
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  DS3G_SUCCESS if success, DS3G_FAILURE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_rab_re_estab_ind
(
  ds_bearer_context_s       *bearer_context_p,
  cm_call_mode_info_s_type  *mode_info_ptr
  
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_RETRY_TIMER_VAL

DESCRIPTION
  This function gets the retry timer value for rab re-establishment
  
PARAMETERS  
  consult_throt_sm - boolean flag indicating if we should consult throt sm
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  timer val (uint32)
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_3gpp_bearer_cntxt_get_retry_timer_val
(
  boolean                consult_throt_sm,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_UPDATE_PHYS_LINK_FLOW_STATUS

DESCRIPTION
  This function enables/disables phys link flows for all active PDP contexts
  This function is called during InterRAT scenarios.  

PARAMETERS
  boolean is_enabled.
  subs_id Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_update_phys_link_flow_status
(
  boolean                is_enabled,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PS_TO_L2_FLOW_ENABLE_CB

DESCRIPTION
  This function sends a flow enable message to ds task
  
PARAMETERS  
  bearer_context_p   - bearer context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_ps_to_L2_flow_enable_cb
(
 dsm_watermark_type *wm,  
 void*              callback_data
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PS_TO_L2_FLOW_DISABLE_CB

DESCRIPTION
  This function sends a flow disable message to ds task
  
PARAMETERS  
      
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_ps_to_L2_flow_disable_cb
(
 dsm_watermark_type *wm, 
 void*              callback_data
);

/*===========================================================================
FUNCTION DSBEARERCNTXT_IS_PS_DATA_AVAILABLE

DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.


PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsBearerCntxt_is_ps_data_available
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================

FUNCTION DS_BEARER_CNTXT_LOCAL_CLEANUP

DESCRIPTION
  This function peforms local call cleanup

PARAMETERS
  Bearer context pointer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds_bearer_cntx_local_cleanup
(
  ds_bearer_context_s *bearer_context_p
);


/*===========================================================================
FUNCTION ds_bearer_cntxt_transfer_ind

DESCRIPTION
  This function is to perform any mode specific operations on cntxt transfer
  indication.
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE,if atleast one Bearer was successfully transferred to target rat
  FALSE, otherwise 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean 
ds_bearer_cntxt_transfer_ind
(
  sys_sys_mode_e_type    target_rat,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_RESET_SRV_REQ_THROT

DESCRIPTION
  This function resets service request throttling SM. This is called during
  RAT change. 

PARAMETERS
  sys_modem_as_id_e_type  - subscription ID. 
  sys_sys_mode_e_type     - new mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_bearer_cntxt_reset_srv_req_throt
(
  sys_modem_as_id_e_type subs_id,
  sys_sys_mode_e_type    target_rat
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_SUSPEND_IND

DESCRIPTION
  This function is called during InterRAT scenarios and calls the mode 
  specific handler.

PARAMETERS
allowed_to_send_data_avail - boolean dicteted by serv status, full_srv/
                             ltd srv/power save mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_suspend_ind
(  
  boolean                allowed_to_send_data_avail,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntxt_RESUME_IND

DESCRIPTION
  This function is to perform any mode specific operations on data resume
  indication.
  
PARAMETERS  
  subs_id - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_resume_ind
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_SYS_CHANGE_HANDLER

DESCRIPTION
   This function updates the DSMGR call table with the new sys mode

PARAMETERS
   new_mode
   subscription id

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_sys_change_handler
(
  sys_sys_mode_e_type    new_mode,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_BEARER_CNTXT_PS_DATA_FAIL_IND_HDLR

DESCRIPTION
  This function enables phys link flows for all active PDP contexts, if we 
  are allowed to send data_available_ind based on the srv status.
  This function is called during InterRAT scenarios.  

PARAMETERS
  boolean allowed_to_send_data_avail.
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_bearer_cntxt_ps_data_fail_ind_hdlr
(
  boolean                allowed_to_send_data_avail,
  sys_modem_as_id_e_type subs_id
);


#ifdef FEATURE_QXDM_WM_TRACING
/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_CALL_ID

DESCRIPTION
  This function gives the call id.

PARAMETERS
  bearer_context_p - bearer context pointer

DEPENDENCIES
  None.

RETURN VALUE
  uint32 - call id.
  
SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_bearer_cntxt_get_call_id
(
  ds_bearer_context_s* bearer_context_p
);

/*===========================================================================

FUNCTION DS3GPP_REPORT_WM_STATS_CB

DESCRIPTION
  This function internally call bearer context function to report wm status. 

PARAMETERS
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void ds3gpp_report_wm_stats_cb
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_REPORT_WM_STATS

DESCRIPTION
  This function internally call umts or lte specific function to report wm 
  status. 
  
PARAMETERS  
  bearer_instance - bearer context instance
  wm_stat_ptr     - pointer where we store the wm values
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_report_wm_stats
(
  uint32          bearer_instance,
  fc_wm_stat_type *wm_stat_ptr
);

#endif /* FEATURE_QXDM_WM_TRACING */

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_GET_PHYS_LINK_PTR_FROM_NSAPI

DESCRIPTION
  This function returns the UMTS PHYS Link ptr corresponding to the NSAPI.
  This is used for the test framework. 
  
PARAMETERS
  nsapi
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  Phys Link Ptr

SIDE EFFECTS
  None.

===========================================================================*/
ps_phys_link_type *ds_3gpp_bearer_cntxt_get_phys_link_ptr_from_nsapi
(
  uint8                  nsapi,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS3GPP_GET_V4_IFACE_PTR_FROM_NSAPI

DESCRIPTION
  This function returns the v4 PS IFace ptr corresponding to the NSAPI.
  This is used for the test framework.

PARAMETERS
  nsapi
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_3gpp_bearer_cntxt_get_v4_iface_ptr_from_nsapi
(
  uint8                  nsapi,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS3GPP_GET_V6_IFACE_PTR_FROM_NSAPI

DESCRIPTION
  This function returns the v6 PS IFace ptr corresponding to the NSAPI.
  This is used for the test framework.

PARAMETERS
  nsapi
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ps_iface_type *ds_3gpp_bearer_cntxt_get_v6_iface_ptr_from_nsapi
(
  uint8                  nsapi,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
  FUNCTION DS_3GPP_BEARER_CNTXT_REORIG_ALL_DORM_BEARERS

  DESCRIPTION
    This function is responsible for reoriginating all the dormant calls.
  
  PARAMETERS   
    subs_id: Subscription Id
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
void ds_3gpp_bearer_cntxt_reorig_all_dorm_bearers
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_START_BP_TIMER

DESCRIPTION
  This function starts the BP removal timer
  
PARAMETERS  
  *bearer_cntxt_p  - bearer_context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_start_bp_timer
(
  ds_bearer_context_s  *bearer_cntxt_p,
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_TMR_BACK_PRESSURE_DISABLE_EXP_HDLR

DESCRIPTION
  This is the timer exp hdlr for dynamic back pressure disable. This would be 
  called in PS task context.

PARAMETERS
  user_data_ptr - user data.

DEPENDENCIES
  None.

RETURN VALUE
  None 

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_tmr_dyn_back_pressure_exp_hdlr
(
  void              *user_data_ptr
);

/*===========================================================================
FUNCTION DS_WCDMA_BEARER_CNTXT_DISABLE_BACK_PRESSURE

DESCRIPTION
  This function forwards pkts received to the lower layers for 
  pdp-ip call.
  This function is registered with ps_iface for a PDP-IP call 
  
PARAMETERS  
  *bearer_cntxt_p  - bearer_context pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_disable_back_pressure
(
  ds_bearer_context_s  *bearer_cntxt_p
);


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_CLEANUP_CALL_RESOURCES

DESCRIPTION
  This function clears bearer resources when call end is initiated.
  
PARAMETERS  
  pdn_cntx_p - PDN context for which the bearer needs to be cleaned.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_bearer_cntxt_cleanup_call_resources
(
  ds_pdn_context_s  *pdn_cntx_p
);

/*===========================================================================
FUNCTION ds_bearer_cntx_validate_bearer_context

DESCRIPTION
  This function checks if the bearer context has been allocated dynamic memory

PARAMETERS
  bearer_context_p       - bearer_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_validate_bearer_context
(
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_CNTX_VALIDATE_BEARER_CONTEXT_INTERNAL

DESCRIPTION
  This function checks if the bearer context has been allocated dynamic memory
  but no report for sucess or failure

PARAMETERS
  bearer_context_p       - bearer_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_validate_bearer_context_internal
(
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION ds_bearer_cntx_register_dpm

DESCRIPTION
  This function registers the bearer with dpm module

PARAMETERS
  bearer_context_p       - bearer_context to be registered
    
DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/

void ds_bearer_cntx_register_dpm
(
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION ds_bearer_cntx_register_dpm_tlb_mode

DESCRIPTION
  This function registers the bearer WM with dpm module in TLB Mode A

PARAMETERS
  bearer_context_p       - bearer_context to be registered
    
DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntx_register_dpm_tlb_mode
(
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION ds_bearer_cntx_deregister_dpm

DESCRIPTION
  This function deregisters the bearer with dpm module
  If it is a dual ip bearer; both the ifaces will be
  derigistered for the correspodning bearer
PARAMETERS
  bearer_context_p       - bearer_context to be registered
    
DEPENDENCIES
  None.

RETURN VALUE
 None.
 
SIDE EFFECTS
  None

===========================================================================*/

void ds_bearer_cntx_deregister_dpm
(
  ds_bearer_context_s *bearer_context_p
);
/*===========================================================================
FUNCTION ds_bearer_cntx_deregister_dpm_iface

DESCRIPTION
  This function deregisters the iface with dpm module
  for the associated bearers
PARAMETERS
  bearer_context_p       - bearer_context to be registered
  iface_ptr              - iface to be derigstered     
DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None

===========================================================================*/


void ds_bearer_cntx_deregister_dpm_iface
(
  ds_pdn_context_s                 *pdn_cntx_p,
  ps_iface_type                    *iface_ptr
);


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_IS_ACTIVE_BEARER_CNTXT

DESCRIPTION
  This function returns TRUE if there is an active bearer context in the
  system. Else, it returns FALSE.
.
  
PARAMETERS  
  pdn_cntx_p - PDN context for which the bearer needs to be cleaned.
  subs_id    - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_is_active_bearer_cntxt
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_BEARERS_ON_LTE

DESCRIPTION
  This function returns TRUE if all the active bearers are in LTE mode.
  Else, it returns FALSE.
  
PARAMETERS  
  subs_id - Subscription Id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  boolean
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_bearer_cntxt_bearers_on_lte
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_bearer_cntx_free_bearer_context

DESCRIPTION
  This function frees dynamic bearer context

PARAMETERS
  bearer_context_p       - bearer_context to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntx_free_dyn_mem
(
  ds_bearer_context_s *bearer_context_p
);
/*===========================================================================
FUNCTION  DS_3GPP_CC_SP_THROTTLING

DESCRIPTION
  This function is used for cause code specific throttling for UMTS 

PARAMETERS
rejected_pdn_cntx_p     - pointer to pdn_cntxt that got rejected
pdn_throt_sm_ptr        - pointer to throttle sm
current_bearer_mode     - current mode of the bearer
down_reason             - pdn reject cause code given by the network

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_bearer_cntxt_cc_sp_throttling
(
  ds_pdn_context_s		*rejected_pdn_cntx_p,
  void				*pdn_throt_sm_ptr,
  sys_sys_mode_e_type		 current_bearer_mode,
  ps_iface_net_down_reason_type  down_reason
);

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_GLOBAL_THROTTLING

DESCRIPTION
  This function is used for global throttling for UMTS as well as LTE  

PARAMETERS
rejected_pdn_cntx_p     - pointer to pdn context that got rejected
pdn_throt_sm_ptr        - pointer to throttle sm
current_bearer_mode     - current mode of the bearer
down_reason             - pdn reject cause code given by the network

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_bearer_cntxt_global_throttling
(
  ds_pdn_context_s              *rejected_pdn_cntx_p,
  void                          *pdn_throt_sm_ptr,
  sys_sys_mode_e_type            current_bearer_mode,
  ps_iface_net_down_reason_type  down_reason
);

/*===========================================================================
FUNCTION  DS_3GPP_BEARER_CNTXT_GET_NUM_FREE_BEARERS

DESCRIPTION
    This function returns the total number free of bearers in the bearer
    resource pool
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  uint8     - total number of bearers in use

SIDE EFFECTS
  None.

===========================================================================*/
uint8 ds_3gpp_bearer_cntxt_get_num_free_bearers(void);

/*===========================================================================

FUNCTION DS_3GPP_BEARER_CNTXT_MEM_CTRL_EV_CB

DESCRIPTION
  This function posts cmd to DS task to notify DSM Memory control event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_mem_ctrl_ev_cb
(
  dsm_mempool_id_enum_type pool_id,            /* memory pool ID */
  dsm_mem_level_enum_type  event,              /* type of memory event */
  dsm_mem_op_enum_type     op
);


/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb

DESCRIPTION
  This function is the nonempty callback function for the bearer's downlink
  watermark. As part of nonempty callback function processing we set the rx_sig

PARAMETERS
  bearer_context_p       - bearer_context to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/

void ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb
(
 dsm_watermark_type *wm,
 void*              callback_data
);

/*=============================================================================
FUNCTION ds_bearer_cntxt_clr_pending_calls

DESCRIPTION
  This function will clear the calls in Pending Redial state if conditions are met.

PARAMETERS
  client_data_ptr: Subs Id passed as Client Data Ptr

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void ds_bearer_cntxt_clr_pending_calls
(
  void* client_data_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_COMP_LOG_FETCH_IFACE

DESCRIPTION
  This Functions returns the iface corresponding to link control id

PARAMETERS
  logging_info_ptr - the Log information 
  ip_version       - ip version v4 or v6

DEPENDENCIES
  None.

RETURN VALUE
  Iface.
  
SIDE EFFECTS
  None.
=============================================================================*/

ps_iface_type *ds_3gpp_bearer_cntxt_comp_log_fetch_iface
( 
  ps_comp_logging_instance_config_s *logging_info_ptr,
  ip_version_enum_type               ip_version
);

/*===========================================================================
FUNCTION ds_bearer_cntx_report_ps_wmk_logging

DESCRIPTION
  This function reports PS watermark per IFace

PARAMETERS
  bearer_context_p       - bearer_context to be freed
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
void ds_bearer_cntxt_report_ps_wmk_logging
(
  ds_bearer_context_s             *bearer_context_p,
  ps_wmk_logging_dorm_state_type   ps_event
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_MODE_PREF_CHANGE

DESCRIPTION
  This Handler is triggered while mode_pref  changes. This is required when 
  IRAT happens due to mode change.

PARAMETERS
  subs_id             -- Subscription ID where mode_pref_change occurred.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_mode_pref_change
(
  sys_modem_as_id_e_type subs_id
);

 /*===========================================================================
FUNCTION DS_BEARER_CNTXT_TD_PROC_CMD

DESCRIPTION
  This function handles the request to update ul wm levels from
  TDRRC 

PARAMETERS
  rab_id  - Identifier used to uniquely identify a data bearer
  ul_data_rate - Updated data rate of the bearer
  subs_id      - Subscription on which this call is made

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds_bearer_cntxt_td_proc_cmd
(
   uint32 rab_id,
   uint32 ul_data_rate,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PREPARE_FOR_TEARDOWN

DESCRIPTION
  This function is used to prepare the bearer context for teardown, Certain
  values would be set and appropriate teardown parameters will be populated.
 

PARAMETERS
  bearer_context_p - pointer to the bearer on which call is to be ended
  info_ptr         - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS (TRUE)
  DS3G_FAILURE (FALSE) on any failure

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_bearer_cntxt_prepare_for_teardown
(
  ds_3gpp_pdn_call_info_type    *mode_sp_info,
  ds_bearer_context_s           *bearer_context_p
);


/*===========================================================================
FUNCTION DS_BEARER_CNTXT_VALIDATE_BEARER_SUBSCRIPTION_INTERNAL

DESCRIPTION
  This function validates whether the subs id of the bearer matches the
  subs id that was passed in as parameter. No F3 if bearer context is invalid
  or does not belong to subs

PARAMETERS
  bearer_context_p : Bearer Context Ptr
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer Subscription Id is valid
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntx_validate_bearer_subscription_internal
(
  ds_bearer_context_s     *bearer_context_p,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_VALIDATE_BEARER_SUBSCRIPTION

DESCRIPTION
  This function validates whether the subs id of the bearer matches the
  subs id that was passed in as parameter.

PARAMETERS
  bearer_context_p : Bearer Context Ptr
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Bearer Subscription Id is valid
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntxt_validate_bearer_subscription
(
  ds_bearer_context_s     *bearer_context_p,
  sys_modem_as_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches a handle to the Bearer Context Information
  corresponding to the given Subscription.

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the Bearer Context Information for the given Subscription.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_cntxt_per_subs_info_type* ds_bearer_cntxt_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_VALIDATE_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function validates the handle to the Subscription Specific
  Bearer Context Information. 
  
PARAMETERS
  hndl  : Handle to the Subscription specific
          Bearer Context Information

DEPENDENCIES
  None.

RETURN VALUE
  TRUE,  if the handle is valid
  FALSE, if invalid

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_bearer_cntxt_validate_per_subs_info_hndl
(
  ds_bearer_cntxt_per_subs_info_type *hndl
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_GET_INSTANCE_PTR

DESCRIPTION
  This function fetches the Bearer Context Pointer corresponding to the
  given Bearer Instance.
 
PARAMETERS
  Bearer Instance

DEPENDENCIES
  None.

RETURN VALUE
  Bearer Context Pointer.

SIDE EFFECTS
  None.
===========================================================================*/
ds_bearer_context_s* ds_bearer_cntxt_get_instance_ptr
(
  uint8 instance
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes the Bearer Context Information for the given
  subscription.
 
PARAMETERS
  subs_id : Subscriptio Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_bearer_cntxt_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_is_modem_fd_enabled

DESCRIPTION
  This function returns if modem fast dormancy is enabled in EFS or not

PARAMETERS
None
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_is_modem_fd_enabled(void);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag

DESCRIPTION
  This function sets modem fast dormancy is enabled flag

PARAMETERS
boolean value to enable/disable
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_modem_fd_enabled_flag
(
  boolean enabled_flag
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_modem_fd_timer_1_value

DESCRIPTION
  This function sets modem fast dormancy timer value

PARAMETERS
value 
    
DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_modem_fd_timer_1_value
(
  uint32 timer_value
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_cntxt_set_modem_fd_timer_2_value

DESCRIPTION
  This function sets modem fast dormancy timer value

PARAMETERS
value 
    
DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_bearer_cntxt_set_modem_fd_timer_2_value
(
  uint32 timer_value
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_HANDLE_CONNECTION_RELEASE_REQUEST

DESCRIPTION
  This function handles connection release SYS IOCTL from PS and sends
  Signalling Connection Release Request to CM if applicable

PARAMETERS
  arg_val_ptr - carrys required information
  ps_errno    - type of error
    
DEPENDENCIES
  None.

RETURN VALUE
 0- Success
 -1- Failure

SIDE EFFECTS
  None

===========================================================================*/
int ds_3gpp_bearer_cntxt_handle_connection_release_request
(
  void                      *arg_val_ptr,
  int16                     *ps_errno
);

/*===========================================================================

FUNCTION DS_BEARER_CNTXT_IS_ANY_OTHER_BEARER_PER_SUB_ACTIVE

DESCRIPTION
  This function checks if any other bearer is in UP state only for the SUBS
  of the bearer context

PARAMETERS  
  Bearer Context ptr

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_bearer_cntx_is_any_other_bearer_per_sub_active
(
  ds_bearer_context_s *bearer_context_p
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_RESET_ACCESS_BARRING_RETRY

DESCRIPTION
  This function resets access_barring_retry 

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_bearer_cntxt_reset_access_barring_retry
(
   void
);
/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_IS_ANY_PDN_CLEAN_UP_IN_PROGRESS

DESCRIPTION
  This function checks if any PDN clean up is in progress

PARAMETERS
None

DEPENDENCIES
  None.

RETURN VALUE
  TRUE-- if YES
  FALSE- if NO

SIDE EFFECTS
  None

===========================================================================*/

boolean ds_3gpp_bearer_cntxt_is_any_pdn_clean_up_in_progress
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION ds_3gpp_bearer_gen_fast_dormancy_event
DESCRIPTION
  This function is called to send fast dormancy status event to PS/QMI.
 
PARAMETERS 
  None.
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_bearer_gen_fast_dormancy_event
(
  ds_bearer_context_s               *bearer_context_ptr,
  sys_modem_as_id_e_type             subs_id,
  void                              *event_data_ptr
);

/*===========================================================================
FUNCTION DS_BEARER_CNTXT_DEACTIVATE_LAST_BEARER_IF_NEEDED

DESCRIPTION
  This function checks 
  1. If all bearers are down
  2. If all PDN's are down

  If one of the above is not fulfilled a fres attach is not tried
 
PARAMETERS 
 
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void ds_bearer_cntxt_deactivate_last_bearer_if_needed
(
   sys_modem_as_id_e_type  subs_id 
);

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_GET_EPS_ID_FROM_PHYSLINK

DESCRIPTION
  This function gets the LTE EPS beater ID of the bearer corresponding to the 
   physlink information passed

PARAMETERS
 eps_id_p - pointer for eps_id info
 phys_link_ptr - physlink pointer for whuch corresponding eps_id is required
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_bearer_cntxt_get_eps_id_from_physlink
(
  uint8*             eps_id_p,
  ps_phys_link_type* phys_link_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_BEARER_CNTXT_DPM_BEARER_ERR_CB

DESCRIPTION
  This is a callback function called from PS Task when malformed 
  number of packets on particular bearer reaches threshhold.

PARAMETERS
  phys_link_ptr - physical_link_ptr
  prev_resync_passed -  previous resync flag value passed
  
DEPENDENCIES
  None.

RETURN VALUE
  NoneE.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void ds_3gpp_bearer_cntxt_dpm_bearer_err_cb
(
  ps_phys_link_type* phys_link_ptr,
  boolean            prev_resync_passed
);

#endif /* FEATURE_DATA_LTE */

#endif /* FEATURE_DATA_3GPP */
#endif /* DS_3GPP_BEARER_CONTEXT_H */

