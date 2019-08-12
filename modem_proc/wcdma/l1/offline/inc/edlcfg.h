#ifndef EDL_H
#define EDL_H

/*============================================================================
              E N H A N C E D   U P L I N K   H E A D E R   F I L E

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
downlink physical channel configuration  .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/edlcfg.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/08/16    raj     Fix for spurious AG decodes when secondary carrier is active
02/04/14    ash     Added Macro to determine if EUL 2ms TTI is active to be used by WMCVS, to bump up clock.
02/04/15    gp      Change ordered page for EDL channels valid after populating 
                    database to avoid race condition
11/18/14    cjz     Fix for 2ms NS-RGCH is not put to sleep with CPC-DRX configured after sleep condition is met
10/13/14    ymu     Fix for HICH CM mask populating incorrectly for DC-HSUPA
10/01/14    gv      Code cleanup. Removal of stale and unused code. 
07/28/14    pkg     Code Check-in for 16QAM feature.
06/23/14    pkg     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag.
06/10/14    yw      EDL config code cleanup: (needs CR658318) No need to delay CPHY CNF into EDL frm evt
05/11/14    yw      MCPM Opts for unification of HSDPA/HSUA Clk requests.
05/21/14    raj     Initial code checkin to allow CPC with QTA
05/21/14    sl      Changes for 0x4186 log packet version8
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
04/27/14    jkb     Add check for Feature dual sim with feature qta
12/10/13    raj     compilation fix for DCHSUPA: ASU cell add and drop support
11/22/13    ymu     0x4186 logging fix for secondary carrier
11/14/13    yw      DCHSUPA: ASU cell add and drop support
11/04/13    yw      CM support for DCHSUPA
10/14/13    pr/sr   Channel config optimizations
09/17/13    jkb     Porting Dual Sim from Triton to Dime
07/11/13    ymu     Support for secondary carrier OOS and recfg.
07/11/13    ash     Changes in HICH/RGCH Threshold calculation formula
07/02/13    yw      Fixed crash during TTI recfg with ASU
06/25/13    yw      Define UE cat 8 for DCHSUPA 
06/24/13    gp      Code changes for new False detection algo for RGCH detection
                    in a single Radio link scenario.
06/12/13    ymu     Fix KW errors, deadlock crash and 435B crash. 
06/12/13    ash     Fixed e_dl_populate_fing_log_info() to accept uint16 as arg for psc instead of uint8
06/07/13    at      EUL event reporting to Diag changes for L1M redesign
05/22/13    oh      Adding DC-HSUPA framework support
05/20/13    at      Support for Enhanced Layer-1 Manager
05/17/13    ash     Fixed race condition for HICH ordered page update
03/19/13    vr      HSRACH fix to retrieve updated cell_db_idx even for HSRACH 
                    suspend-resume operation
01/04/13    raj     Changes as a part of ZI memory optimization
12/10/12    ash     Delay setting the new EDL CM Bitmask from N+1 to N+2 during 
                    recfg with Timing Change, since the new config is active only in N+2
11/02/12    pr      Changes to fix Active HWCH in 0x4186 Log packet.
10/15/12    jd      Memory Optimization Changes
08/31/12    vr      Fix for 0x4309 logging specific to HSRACH seamless transition.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/03/12    raj     Mainlined the feature FEATURE_HSUPA_L1_LOGGING_ENABLED
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Feature cleanup.
02/01/12    gnk     Nikel feature cleanup
01/31/12    raj     Nikel feature cleanup. Mainlined the feature FEATURE_WCDMA_HSUPA_IRQ_SPLIT
01/26/11    vs      Nikel feature cleanup. Mainlined the feature FEATURE_L1_LOGGING_ENABLED.
11/29/11    gnk     Removed some unused features
08/11/11    oh      Fixed KW errors.
03/11/11    dp      Ported CPC DRX changes to offline Architecture
10/27/10    sp      Fixed compiler error 
10/27/10    sp      Added functionality for E-AGCH enhanced detection algorithm
                    under FEATURE_WCDMA_HSUPA_AGCH_DETECTION
10/26/10    gv/gnk  Validated DTX fixes and code review comments
09/02/10    gv      Code Checkin for CPC DTX
04/09/10    ksr     Changes to move logging variables to dynamic memory approach
08/25/09    sa      Including offset 'b' in HICH/RGCH threshold computation along with
                    QXDM command support.
07/31/09    gnk     Mainlining the feature FEATURE_WCDMA_DL_ENHANCED
07/22/09    gnk     Mainlining the feature FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT
07/16/09    hk      Reorganized the code to remove INC/SRC violations
04/30/09    sa      Use optimised threshold parameters for 2ms TTI EUL.
04/24/09    sa      Support to use optimised 10ms TTI HICH threshold paramters
04/21/09    vc      Modified e_dl_populate_fing_log_info definition.
03/18/09    gv      Fix for a very corner case of CPHY_IDLE_REQ following a 
                    CPHY_SETUP_REQ with EUL config for 2ms TTI.
02/06/09    sa      Fix for EDL channel log packet crash and double buffering changes
01/23/09    oh      Optimization HICH/RGCH threshold calculations.
11/19/08    gv      Changes to correct debug code to detect MDPS timing error.
10/08/08    oh      Adding support for dropping EDL channels before cell is dropped.
10/03/08    hk      Bringing in 7k Mailine fixes
09/29/08    sa      Added new field in EDL config DB to map the 
                    cell index to the config DB index.
09/28/08    sa      SW changes to support serving cell change
09/26/08    sa      Support for EUL DL reconfig/SHO
09/16/08    gv      Including function prototype in proper feature-part of lint fix
08/13/08    gv      EUL fixes for stop and other clean up.
07/31/08    oh      EUL changes for Taxis.
01/21/08    gv      Provided fix for EF when EUL DL was trying to de-register
                    svc from per RL triage, when that RL was already dropped
                    from R99 ASET.
01/04/08    oh/gv   Fix for EF in the AGCH fix/workaround
01/03/08    oh/gv   Support for AGCH detection issue workaround.
12/11/07    gv      Defined a bit mask for debug global for CM
11/15/07    gv      Fixed compilation warning for ADS1.2
10/26/07  gv/vp     Added functionality for EUL DL to co-exist with new R99
                    DL architecture. 
10/08/07    gv      Modified the threshold computation parameter for HICH NS RLS.
09/03/07    gv      Added prototype for function to get serving RLS change info.
06/18/07    gv      Added prototype for function to know whether PSC 
                    is in serving RLS. This is used by triage.
06/13/07    gv      Added code to support new threshold algortihm
05/21/07    gv      Functionality to support new 4302 log packet structure.
05/04/07    gv      Added macro to access the serv RLS change global and the
                    added global for SYNC A procedure during EDL. 
04/19/07    gv      Added macro to access the edl globals for serving RLS
                    change indications and check whether ref fing is valid
                    for a specific EDL RL.
03/02/07    gv      Added support for log on demand for 4301 log packet info.
02/20/07    gv      Added code for applying SG at activation time
02/22/07    gv      Added #define for new values of 'a' and 'b' for computation
                    of threshold for SHO/Non SHO cases.
02/16/07    gv      Added macro to access the serv_cell_idx in active RL info.
02/05/07    am      Added Macro to get the num of RG RLS and HICH RLS
02/02/07    oh      Enhanced interface to control various f3 messages for EUL.
02/02/07    gv      Updated the prototype of the function
                    e_dl_populate_fing_log_info. Also added updated the 
		    #define for HICH and RGCH threshold 'a' and 'b' values.
02/01/07    gv      Added the extern e_dl_edch_serv_cell_and_rls_change_ind.
                    This flag is used to indicate to MAC that a serving
	            cell has changed and also new serving cell is not in the
	            previous serving RLS.
12/29/06    gv      Added a flag to in per RL info field of e_dl_config_db,
                    to indicate whether the RL is participating in soft 
                    combining, either HICH or RGCH.
12/21/06  bbd/gv    Added prototype for functions modified/added. This 
                    change is to support the re-org of per RL triage for 
                    enhanced DL.
12/20/06    gv      Added config CFN to e_dl_config_db structure. This is 
                    updated every time a config command is sent to FW. This
                    is used when logging the end_fn for a channel stop.
12/13/06    gv      Added the #define for immediate stop sub FN value
12/06/06    am      Fix EUL L1/MAC log packet
11/16/06    gv      Added a extern definition of a global to be used in
                    srchdch.c. This is to allow the SC weights to be computed
                    only if needed even though there are multiple RLs active.
11/02/06    am      Added macro to get rg comb idx for E_UL use
10/17/06    gv      Added support for EUL finger info logging
10/16/06    ub      Added support for EUL DL CM
10/12/06    ub      Added support for finger lock status update
10/12/06    gv      Defined structures and #defines used in code for
                    EUL events, threshold scaling and soft combining weights.
09/25/06    gv      Added a few fixes from code review and code cleanup.
09/15/06    gv      Added the #define for DEBUG bit masks and also a structure
                    prototype for the active RL information in EDL.
08/14/06    yiz     Mainline HSUPA code
01/14/06    am      file created.
                    
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "l1rrcif.h"
#include "eul.h"
#include "edllog.h"
#include "eulsg.h"
#include "mdspasync.h"
#include "dltriparm.h"
#include "dlphch.h"
#include "mcalwcdma_edl.h"

#ifdef FEATURE_WCDMA_HS_RACH
#include "wl1hsrachcntrlr.h"
#endif

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/*
#define E_DL_CONFIG_DB_INVALID_PAGE 0xFF
*/

#if 0
#ifdef  FEATURE_WCDMA_DC_HSUPA 
/*max num UL CARRs with DC HSUPA*/
#define WL1_MAX_UL_CARR_IDX 2
#define WL1_PRI_UL_CARR_IDX 0
#define WL1_SEC_UL_CARR_IDX 1
#else
#define WL1_MAX_UL_CARR_IDX 1
#define WL1_PRI_UL_CARR_IDX 0
#endif /*FEATURE_WCDMA_DC_HSUPA */
#endif

//DEBUGGING
/* To debug the IRQ split feature */
#define DEBUG_EUL_IRQ_SPLIT     0x800   
#ifdef FEATURE_WCDMA_CPC_DTX
#define DEBUG_EUL_CPC_DTX_LOGGING 0x1000
#define DEBUG_CPC_DTX_CM_LOGGING       0x2000 
#define DEBUG_CPC_DTX_CQI_PRIORITY_MSG 0x4000
#endif

#define DEBUG_EUL_DL_LOGGING_HICH     0x1
#define DEBUG_EUL_DL_LOGGING_HICH_ALL 0x2
#define DEBUG_EUL_DL_LOGGING_AGCH     0x4
#define DEBUG_EUL_DL_LOGGING_AGCH_ALL 0x8
#define DEBUG_EUL_DL_LOGGING_RGCH     0x10
#define DEBUG_EUL_DL_LOGGING_RGCH_ALL 0x20
#define DEBUG_EUL_DL_LOGGING_SG       0x40
#define DEBUG_EUL_DL_LOGGING_BETA_ED  0x80
#define DEBUG_EUL_DL_LOGGING_ETFC_PR  0x100
#define DEBUG_EUL_DL_LOGGING_REL6_MPR 0x200
#define DEBUG_EUL_DL_LOGGING_CM       0x400
#define DEBUG_EUL_MAC_ETFCI_LOGGING   0x1000000
#define DEBUG_EUL_MAC_SI_LOGGING      0x2000000
#define DEBUG_EUL_MAC_HB_LOGGING      0x4000000
#define DEBUG_EUL_MAC_LOW_LOGGING     0x8000000

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
#define DEBUG_UL_QTA_LOGGING 0x8000
#endif /* FEATURE_QTA */

#define E_DL_HICH_LOGGING_BMSK 0x1
#define E_DL_RGCH_LOGGING_BMSK 0x2
#define E_DL_AGCH_LOGGING_BMSK 0x4


extern uint32 DEBUG_EUL_DL_LOGGING;

#define  INVALID_FINGER            0xFF
#define  INVALID_PSC_VAL           0xFFFF
#define  INVALID_INDEX             0xFF
#define  INVALID_PAGE_INDEX        0xFF
#define  INVALID_CELL_INDEX        0xFF
#define  INVALID_COMB_CHAN_INDEX   0xFF
#define  INVALID_FRAME_NUMBER      0x8000
#define MAX_NUM_RG_COMB_IDX 6
#define MAX_NUM_TPC_IDX 6


#define MAX_TAU_DPCH_VAL 149


/*Offset of AGCH from PCCPCH*/
#define AGCH_PCCPCH_OFFSET_IN_TICKS 20
/*Initial valus for Energy, Value, dirty bit and Sub FN*/
#define INVALID_HICH_RGCH_ENERGY 0xFFFFFFFF
/*This specific invalid HICH/RGCH Value of 3 will be used by EUL for some logging
** purpose*/
#define INVALID_HICH_RGCH_VAL 0x03
#define INVALID_DIRTY_BIT_VAL 0xFF
#define INVALID_SUB_FN_VAL 0xFFFF

#define E_DL_CHAN_MASK_BMSK 0x0FFF
#define E_DL_INIT_SOFT_COMB_WEIGHT 1

/*Initialize the threshold value to 5000 .. TBD*/
#define HICH_RGCH_THRESHOLD_MAX_VALUE 5000

/* Error count thresholds used in EAGCH detection algorithm */
#define AGCH_CRC_PASS_HIGH_EM_ERR_CNT_THRESH 16
#define AGCH_CRC_PASS_LOW_EM_ERR_CNT_THRESH 6
#define AGCH_CRC_FAIL_ERR_CNT_THRESH_1 8
#define AGCH_CRC_FAIL_ERR_CNT_THRESH_2 14


/* This is to limit the agch threshold value calculated 
   not to overflow. The maximum agch threshold doesn't exceed 21 bits.*/
#define AGCH_THRESHOLD_MAX_LIMIT_ALLOWED 0x00FFFFFF

/* The param a=(1/128) used in EAGCH detection algorithm.
 * This macro is used to right shift by 7, i.e., divide by 128 */
#define AGCH_THRESH_PARAM_A 7 

/* The parameter 1.3 to account for 30% headroom on energy metric when CRC fails
 * Converted to Q10 format 1.3 << 10 */
#define AGCH_THRESH_PARAM_B 1331

#define EDL_HICH_RGCH_SCALE_FACTOR (1 << 8)
#define EDL_HICH_RGCH_DESCALE_FACTOR 8

#define UE_CAT_DC_HSUPA 8
#define UE_CAT_HSUPA_MAX 6
#define UE_CAT_HSUPA_INIT 0

typedef enum{
  /* CRC Failure and nothing sent by network (dtx case) */
  EDL_AGCH_DECODE_CRC_FAIL_DTX,
  /* CRC Pass but possibly False Alarm */
  EDL_AGCH_DECODE_FALSE_ALARM,
  /* CRC valid for us or for other users */
  EDL_AGCH_DECODE_VALID,
  /* CRC Fail but not sure if it is because of DTX or bad channel */
  EDL_AGCH_DECODE_CRC_FAIL,
  /* MAX Value for enum */
  EDL_AGCH_DECODE_CRC_NUM,
 }e_dl_agch_decode_result_type;

 /* Used for EDL CM bitmask states during reconfig with Timing Change */
typedef enum {
    /* Indicates that we have not yet received Regonfig with Timing Change */
    EDL_CM_BITMASK_RCFG_WITH_TC_INVALID,
    /* Indicates that we received reconfig with Timing change but haven't 
       hit the frame boudnary yet and we are still in frame N*/
    EDL_CM_BITMASK_RCFG_WITH_TC_IN_PROGRESS,
    /* Indicates that we have passed the frame boundary and can start using 
       new config when the maintenance event handler fires in N+2*/
    EDL_CM_BITMASK_RCFG_WITH_TC_COMPLETE
} e_dl_cm_bitmask_rcfg_with_tc_enum_type;


/* Num pages per channel in EUL DL CFG */
typedef enum {
E_DL_CONFIG_DB_PAGE_ONE,
E_DL_CONFIG_DB_PAGE_TWO,
  E_DL_CONFIG_DB_MAX_PAGES,
  E_DL_CONFIG_DB_INVALID_PAGE = 0xFF
} e_dl_config_db_page_enum_type;

#ifdef FEATURE_WCDMA_CPC_DRX
typedef enum
{
  E_DL_CHAN_AGCH,
  E_DL_CHAN_HICH,
  E_DL_CHAN_S_RGCH,
  E_DL_CHAN_NS_RGCH,
  E_DL_NUM_CHAN
} e_dl_chan_enum_type;

#define WL1_EDL_INVALID_EDL_TO_DPCH_OFFSET 0xFF
#define WL1_EDL_INVALID_ENTITY_ID 0XFF
#define WL1_EDL_INVALID_SUBFR_OFFSET 0XFF

/* DCHSUPA Carrier State Indication */
typedef enum
{
  SINGLE_CARRIER,
  DUAL_CARRIER_CONFIGURED,
  DUAL_CARRIER_ACTIVE,
  CARRIER_STATE_INVALID
} carrier_state_enum_type;

/* Global variable which maintains the CARRIER_STATE */
carrier_state_enum_type carr_state_ind;

#define E_DL_GET_DC_HSUPA_DL_STATE() e_dl_config_db.e_dl_secondary_state

#define GET_CURR_CARR_STATE_IND() \
        (((e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_TX)||(e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_DISABLED) \
             || (e_ul_sec_edpch_recfg_state == E_UL_SEC_CFG_WHEN_ACTIVE)) ? DUAL_CARRIER_ACTIVE : \
         (( \
           (e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_SETUP) || \
           (e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_CFGD) \
           ) ? DUAL_CARRIER_CONFIGURED : SINGLE_CARRIER \
          ))

#define WL1_EDL_IS_E_DL_CHAN_VALID(x) (x < E_DL_NUM_CHAN)
#define IS_AGCH_ENTITY_VALID() WL1_EDLDRX_ENTITY_ID_VALID(e_dl_config_db.e_dl_drx_params.agch_entity_id) 
#define IS_S_RGCH_ENTITY_VALID() WL1_EDLDRX_ENTITY_ID_VALID(e_dl_config_db.e_dl_drx_params.s_rgch_entity_id)
#define IS_NS_RGCH_ENTITY_VALID() WL1_EDLDRX_ENTITY_ID_VALID(e_dl_config_db.e_dl_drx_params.ns_rgch_entity_id)
#define IS_HICH_ENTITY_VALID() WL1_EDLDRX_ENTITY_ID_VALID(e_dl_config_db.e_dl_drx_params.hich_entity_id)
#define IS_EDL_TO_DPCH_OFFSET_VALID(x) WL1_EDLDRX_CH_2_DPCH_OFFSET_10MS_VALID(x)
#else
typedef enum
{
  E_DL_CHAN_AGCH,
  E_DL_CHAN_HICH,
  E_DL_CHAN_RGCH
} e_dl_chan_enum_type;
#endif /*FEATURE_WCDMA_CPC_DRX*/

#define E_DL_STATE_INACTIVE MCALWCDMA_E_DL_STATE_INACTIVE
#define E_DL_STATE_ACTIVE MCALWCDMA_E_DL_STATE_ACTIVE
#define E_DL_STATE_CONFIG_START MCALWCDMA_E_DL_STATE_CONFIG_START
#define E_DL_STATE_CONFIG_RECFG MCALWCDMA_E_DL_STATE_CONFIG_RECFG
#define E_DL_STATE_CONFIG_STOP MCALWCDMA_E_DL_STATE_CONFIG_STOP

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define E_DL_SECONDARY_STATE_INIT                MCALWCDMA_E_DL_SECONDARY_STATE_INIT
#define E_DL_SECONDARY_STATE_CONFIG_START        MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_START
#define E_DL_SECONDARY_STATE_CFGD                MCALWCDMA_E_DL_SECONDARY_STATE_CFGD
#define E_DL_SECONDARY_STATE_ACTIVE              MCALWCDMA_E_DL_SECONDARY_STATE_ACTIVE
#define E_DL_SECONDARY_STATE_CONFIG_RECONFIG     MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_RECONFIG
#define E_DL_SECONDARY_STATE_CONFIG_STOP         MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_STOP
#endif /* FEATURE_WCDMA_DC_HSUPA */

#define E_DL_CHAN_DISABLE MCALWCDMA_E_DL_CHAN_DISABLE
#define E_DL_CHAN_ENABLE MCALWCDMA_E_DL_CHAN_ENABLE
#define E_DL_CHAN_RECONFIG_WITH_TC MCALWCDMA_E_DL_CHAN_RECONFIG_WITH_TC
#define E_DL_CHAN_RECONFIG_WITHOUT_TC MCALWCDMA_E_DL_CHAN_RECONFIG_WITHOUT_TC
#define E_DL_CHAN_NO_ACTION MCALWCDMA_E_DL_CHAN_NO_ACTION
#define MAX_E_DL_CELL_DB_INDEX DL_MAX_NUM_CELLS -1

/* Constants for RGCH decoding:: For the new False Detection Algorithm for a single link
** EDCH ASET only*/
#define EUL_RGCH_SYMBOLS_PER_SLOT 40
#define EDL_RGCH_FA_ALGO_SCALE_FACTOR 100
#define EUL_RGCH_DECODE_MULTIPLIER(n_slots) (EUL_RGCH_SYMBOLS_PER_SLOT * (n_slots))

/* The Serving Cell Change (SCC) type when EDL is active
** This will be used to fill information in the EUL_SERVING_CELL_CHANGE event*/
typedef enum
{
  E_DL_SCC_TYPE_TIMING_INIT_HHO = 0,
  E_DL_SCC_TYPE_TIMING_MAINT_HHO = 1,
  E_DL_SCC_TYPE_NOT_IN_HHO = 2
}e_dl_scc_during_hho_enum_type;

/* This ENUM will be used to fill in information about the action related to
** a serving cell change. The HHO has a global that needs to set in l1msetup.c
** and hence not a part of this ENUM*/
typedef enum
{
  SERV_CELL_NO_CHANGE = 0,
  SERV_CELL_CHANGE_RECFG_SOFT = 1,
  SERV_CELL_CHANGE_RECFG_SOFTER = 2
}e_dl_serving_cell_change_type_enum_type;

/* Enum to indicate the HICH/RGCH channel type for which the threshold needs to be set.*/
typedef enum
{
  E_DL_SINGLE_LINK_THRESH,
  E_DL_SHO_NON_SERV_THRESH,
  E_DL_SHO_SERV_THRESH,
  E_DL_THRESH_NUM_OPS
} eul_debug_e_dl_thresh_type;

/* Enum to indicate the TTI info and the threshold parameter (a or b) to be set.*/
typedef enum
{
  E_DL_TTI_10MS_THRESH_PARAM_A,
  E_DL_TTI_10MS_THRESH_PARAM_B,
  E_DL_TTI_2MS_THRESH_PARAM_A,
  E_DL_TTI_2MS_THRESH_PARAM_B,
  E_DL_TTI_THRESH_PARAM_NUM_OPS
} eul_debug_e_dl_tti_thresh_param_type;

#if defined(FEATURE_WCDMA_EUL_DL_THRESHOLD_PROGRAMMING) || !defined(FEATURE_WCDMA_JOLOKIA_MODEM)
#define GET_FIRST_BYTE(val) (val & 0xFF) /* Gets first byte of 4 byte val*/
#define GET_SECOND_BYTE(val) ((val & 0xFF00) >> 8) /* Gets 2nd byte of val*/
#define GET_THIRD_BYTE(val) ((val & 0xFF0000) >> 16) /* Gets 3rd byte of val*/
#define GET_FOURTH_BYTE(val) ((val & 0xFF0000) >> 32) /* Gets 4th byte of val*/
#endif


/* Structure to store the per frame CM info for EUL use */
typedef struct
{
  /* CM info for the frame */
  l1_cm_ul_param_struct_type    param;
  /* slot bitmask showing CM slots in the frame */
  uint16 dpch_slot_bitmask; 
}eul_cm_param_struct_type;

/* Structure to store EUL CM info for current and next frame */
typedef struct
{
  eul_cm_param_struct_type prev;
  eul_cm_param_struct_type cur;
  eul_cm_param_struct_type next;
}eul_cm_frames_param_struct_type;


typedef struct 
{
  boolean                    new_cfg;

  mcalwcdma_e_dl_chan_action_enum_type action;

  boolean                    pri_ernti_present;
  uint16                     pri_ernti;
  boolean                    sec_ernti_present;
  uint16                     sec_ernti;
  boolean                    agch_sttd;
  uint16                     ovsf_code;
  uint16                     cell_idx;
  uint16                     agch2dpch_offset;
  uint16                     start_sub_fn;
  uint8 correction_factor;
  uint16                     final_sub_fn;
  mcalwcdma_e_dl_channel_cm_info       agch_cm_info;
uint16 num_sub_frms_within_dpch_frm_bdry;
uint8 intf_buf_idx;
#ifdef FEATURE_WCDMA_HS_RACH
wl1_hs_rach_edl_hs_rach_info_struct_type hs_rach_info;
#endif /* FEATURE_WCDMA_HS_RACH*/
} e_dl_agch_info_page_type;


/*AGCH info maintained in DL config db*/
typedef struct 
{
  uint8 cur_page;
  uint8 ord_page;
  e_dl_agch_info_page_type agch_info_page[E_DL_CONFIG_DB_MAX_PAGES];
} e_dl_agch_info_struct_type;

typedef struct 
{
  boolean new_cfg;
  mcalwcdma_e_dl_chan_action_enum_type action;
  boolean                    e_sttd;
  uint16                     ovsf_code;
  uint16                     cell_idx;
  int16                      tau_hich;
  uint16                     hich2dpch_offset;
  uint16                     start_sub_fn;
  uint8 correction_factor;
  uint16                     final_sub_fn;
  uint16                     sig_seq_idx;
  mcalwcdma_e_dl_channel_cm_info       hich_cm_info;

uint16 num_sub_frms_within_dpch_frm_bdry;
uint8 intf_buf_idx;
#ifdef FEATURE_WCDMA_HS_RACH
wl1_hs_rach_edl_hs_rach_info_struct_type hs_rach_info;
#endif /* FEATURE_WCDMA_HS_RACH*/

} e_dl_hich_info_page_type;

typedef struct
{
  boolean new_cfg;
  mcalwcdma_e_dl_chan_action_enum_type action;
  boolean                    srv_rls_flag;
  uint16                     rg_comb_idx;
  uint16                     rgch2dpch_offset;
  uint16                     start_sub_fn;
  uint8 correction_factor;
  uint16                     final_sub_fn;
  uint16                     sig_seq_idx;
  mcalwcdma_e_dl_channel_cm_info       rgch_cm_info;
uint16 num_sub_frms_within_dpch_frm_bdry;
uint8 intf_buf_idx;
#ifdef FEATURE_WCDMA_HS_RACH
wl1_hs_rach_edl_hs_rach_info_struct_type hs_rach_info;
#endif /* FEATURE_WCDMA_HS_RACH*/
} e_dl_rgch_info_page_type;

/*RL info maintained in DL config db*/
typedef struct 
{
  boolean recfg_serv_rls_change;
  boolean recfg_tau_dpch_change;
  uint16 e_rl_psc;
  uint8 e_rl_tpc_idx;
  uint8 e_rl_rg_comb_idx;
  uint16 log_rssi_sum_val;
  uint16 tau_dpch;
  uint8  hich_cur_page;
  uint8  hich_ord_page;
  uint8 cell_idx;
  boolean rl_dropped_from_r99_aset;
  e_dl_hich_info_page_type  hich_info_page[E_DL_CONFIG_DB_MAX_PAGES];
  uint8  rgch_cur_page;
  uint8  rgch_ord_page;
  e_dl_rgch_info_page_type  rgch_info_page[E_DL_CONFIG_DB_MAX_PAGES];
} e_dl_rl_info_struct_type;

/*EDL RLS info*/
typedef struct{
  uint8 rg_comb_idx;
  /*Index of the e_dl_rl_db that have this rg_comb_idx*/
  uint8 e_dl_rl_db_idx[EUL_MAX_RL];
  uint8 num_cells;
}e_dl_rls_info_struct_type;

#ifdef FEATURE_WCDMA_CPC_DRX
/*Parameters related to EDL DRX updated at every EDL config*/
typedef struct{
  uint8 agch_entity_id;
  uint8 s_rgch_entity_id;
  uint8 ns_rgch_entity_id;
  uint8 hich_entity_id;
  uint8 agch_to_dpch_offset;
  uint8 min_s_rgch_to_dpch_offset;
  uint8 min_ns_rgch_to_dpch_offset;
  uint8 min_hich_to_dpch_offset;
  uint8 min_edch_tx_to_hich_rslt_offset;
}e_dl_drx_params_struct_type;
#endif /*FEATURE_WCDMA_CPC_DRX*/

/*EDL TPC Set Info*/
typedef struct{
  uint8 tpc_idx;
  /*Index of the e_dl_rl_db that have this tpc_idx*/
  uint8 e_dl_rl_db_idx[EUL_MAX_RL];
  uint8 num_cells;
}e_dl_tpc_set_info_struct_type;

/*dl config db */
typedef struct 
{
  mcalwcdma_e_dl_state_enum_type e_dl_state;
  mcalwcdma_e_dl_state_enum_type e_dl_next_state;
  dl_phch_cfg_handle_type dpch_cfg_handle[WL1_MAX_UL_CARR_IDX];
  e_dl_agch_info_struct_type agch_info[WL1_MAX_UL_CARR_IDX];
  uint8 dpch_comb_chan_id[WL1_MAX_UL_CARR_IDX];
  uint8 e_serv_rl_idx[WL1_MAX_UL_CARR_IDX];
  e_dl_serving_cell_change_type_enum_type e_serv_cell_changed;
  e_tti_enum_type curr_eul_tti;
  e_tti_enum_type ord_eul_tti;
  e_dl_rls_info_struct_type e_dl_rls_info[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][MAX_NUM_RG_COMB_IDX];
  e_dl_tpc_set_info_struct_type e_dl_tpc_set_info[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][MAX_NUM_TPC_IDX];
  uint8 num_rls[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES];
  uint8 num_tpc_set[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES];
  boolean non_srv_rgch_cfged[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES];
  uint8 rg_tpc_info_curr_page[WL1_MAX_UL_CARR_IDX];
  uint8 rg_tpc_info_ord_page[WL1_MAX_UL_CARR_IDX];
  uint16 hich_cfg_active_sub_fn[WL1_MAX_UL_CARR_IDX];
  uint16 rgch_cfg_active_sub_fn[WL1_MAX_UL_CARR_IDX];
  uint16 agch_cfg_active_sub_fn[WL1_MAX_UL_CARR_IDX];
  boolean eul_tti_changed;
  uint8 num_e_rl[WL1_MAX_UL_CARR_IDX];
  e_dl_rl_info_struct_type e_dl_rl_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
  uint16 farthest_ch2dpch_offset[WL1_MAX_UL_CARR_IDX];
  uint16 edl_config_cmd_cfn;
  uint8 cell_idx_e_rl_idx_mapping[WL1_MAX_UL_CARR_IDX][DL_MAX_NUM_CELLS];
  #ifdef FEATURE_WCDMA_CPC_DRX
  e_dl_drx_params_struct_type e_dl_drx_params;
  #endif /*FEATURE_WCDMA_CPC_DRX*/
  #ifdef FEATURE_WCDMA_HS_RACH
  /* This holds the information of the RL info index 
   ** which corresponds to the HS RACH cell*/
  uint16 hs_rach_e_dl_rl_idx;
  #endif /*FEATURE_WCDMA_HS_RACH*/
  #ifdef  FEATURE_WCDMA_DC_HSUPA 
  mcalwcdma_e_dl_secondary_state_enum_type e_dl_secondary_state;
  mcalwcdma_e_dl_secondary_state_enum_type e_dl_next_secondary_state;
  #endif /*FEATURE_WCDMA_DC_HSUPA */
} e_dl_config_db_struct_type;

/* This structure is defined so that the number of RLs in the e_dl_config_db
** are arranged consecutively with no holes in the array.*/
typedef struct
{
  uint16 psc;
  uint8 tpc_idx;
  uint8 rg_comb_idx;
  uint8 hich_sc_idx;
  uint8 rg_sc_idx;
  /* Boolean to indicate if this is a serving Cell.*/
  boolean serv_cell;
}e_dl_active_rl_info_struct_type;

/* Structure type to hold HICH and RGCH threshold related paramters for each RL*/
typedef struct
{
uint8 max_num_slots;    /* Max number of slots for decoding the channel result*/
  uint16 thresh_param_a;  /* factor 'a' in the threshold equation T= a*cpich_rssi +b */
  uint16 thresh_param_b;  /* The fixed offset 'b' given for threshold T */
}e_dl_thresh_param_info_struct_type;

extern e_dl_thresh_param_info_struct_type hich_thresh_params[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][EUL_MAX_RL];
extern e_dl_thresh_param_info_struct_type rgch_thresh_params[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][EUL_MAX_RL];

/******************************************************************************
                     COMMAND: DL_CPHY_SETUP_LOCAL_CMD_TYPE
******************************************************************************/

/******************************************************************************
                     Defining structures that can be filled to store samples of AGCH/HICH and RGCH
                     and they can be assigned to log packet and flushed when needed.
******************************************************************************/
typedef  struct{
  uint8 new_agch;
  uint8 agch_predec_sym_err;
  uint8 agch_valid;
  uint8 agch_val;
  uint8 ag_scope;
  uint8 pri_sec_ernti;
  uint16 agch_dec_energy;
}e_dl_agch_per_sample_info_struct_type;


typedef struct{
  boolean new_hich;
  int32 eng;
  uint16 thresh;
  uint32 sc_wts;
  uint8 n_slots;
}e_dl_hich_per_sample_info_struct_type;

typedef struct{
  uint8 num_hich_samples;
  e_dl_hich_per_sample_info_struct_type hich_info[EDL_DECODE_LOGGING_NUM_SAMPLES];
}e_dl_hich_per_rl_info_struct_type;

typedef struct{
  boolean new_rgch;
  int32 eng;
  uint16 thresh;
  uint8 n_slots;
}e_dl_rgch_per_sample_info_struct_type;

typedef struct{
  e_dl_rgch_per_sample_info_struct_type rgch_info[EDL_DECODE_LOGGING_NUM_SAMPLES];
  uint8 num_rg_samples;
}e_dl_rgch_per_rl_info_struct_type;


/* This structure stores the delta sub frames to start sub FN and also
** the correction factor. The correction factor has been introduced so that
** the start sub FN can be always a integral multiple of 5 and the correction
** factor will be the offset to the correct sub FN*/
typedef struct{
uint16 config_delta_sub_frames;
uint8 correction_factor;
}e_dl_delta_sub_frames_info_struct_type;

typedef struct{
uint16 psc;
uint8 comb_index;
}e_dl_index_psc_info_struct_type;

typedef struct{
e_dl_index_psc_info_struct_type e_dl_index_psc_info[EUL_MAX_RL];
uint8 num_cells;
uint8 e_tti;
uint8 serv_cell_idx;
}e_dl_chan_config_info_struct_type;

typedef struct{
  int32 energy;
  int16 threshold;
  uint8 num_slots;
  uint8 sign_bit;

}e_dl_chan_ps_info_struct_type;

typedef struct{
e_dl_chan_ps_info_struct_type e_dl_chan_ps_info[E_DL_CHAN_RES_MAX_SAMPLES];
uint16 sub_fn_first_result;
uint8 num_samples;
}e_dl_chan_results_info_struct_type;

typedef struct{
e_dl_chan_config_info_struct_type e_dl_chan_config_info;
e_dl_chan_results_info_struct_type chan_info[EUL_MAX_RL];
}e_dl_chan_logging_struct_type;

typedef struct{
uint8 e_dl_submit_buf_idx;
uint8 e_dl_sample_buf_idx;
boolean e_dl_log_pkt_dirty_bit[E_DL_LOG_PKT_SAMPLE_BUF_MAX];
  e_dl_chan_logging_struct_type e_dl_chan_info[WL1_MAX_UL_CARR_IDX][E_DL_LOG_PKT_SAMPLE_BUF_MAX];
}e_dl_chan_dbl_buf_logging_struct_type;

typedef struct{
uint8 agch_valid;
uint8 agch_val;
uint8 ag_scope;
uint8 pri_sec_ernti;
uint8 pre_dec_symb_err;
uint16 agch_dec_energy;
uint16 agch_vd_output;
uint16 agch_cm_slot_bmsk;
uint16 rssi_sum;
}e_dl_agch_ps_struct_type;

typedef struct{
uint16 num_samples;
uint16 sub_fn_first_result;
e_dl_agch_ps_struct_type agch_logging_ps_info[E_DL_CHAN_RES_MAX_SAMPLES];
}e_dl_agch_logging_struct_type;

typedef struct {
uint8 e_dl_submit_buf_idx;
uint8 e_dl_sample_buf_idx;
boolean e_dl_log_pkt_dirty_bit[E_DL_LOG_PKT_SAMPLE_BUF_MAX];
  e_dl_agch_logging_struct_type e_dl_agch_logging_info[WL1_MAX_UL_CARR_IDX][E_DL_LOG_PKT_SAMPLE_BUF_MAX];
}e_dl_agch_dbl_buf_logging_struct_type;

#define E_DL_LOGGING_HICH_PER_CELL_HDR_LEN 3
#define E_DL_LOGGING_HICH_PER_CELL_SAMPLE_LEN 4
#define E_DL_LOGGING_RGCH_PER_CELL_HDR_LEN 3
#define E_DL_LOGGING_RGCH_PER_CELL_SAMPLE_LEN 4
#define E_DL_LOGGING_AGCH_HDR_LEN             5
#define E_DL_LOGGING_AGCH_PER_CELL_SAMPLE_LEN 10

#ifdef FEATURE_WCDMA_HS_RACH
/* This structure holds the infromation needed on a RL specific basis and not channel basis
** for setting up the channels. The information held in this structure is going to be a common
** for all the channel setup*/
typedef struct{
uint16 cell_index;
uint16 e_dl_rl_index;
uint16 tau_dpch;
uint16 psc;
boolean rl_sttd;
}e_dl_rl_commonn_cfg_info_struct_type;

typedef struct{
uint16 curr_page;
uint16 ord_page;
mcalwcdma_e_dl_chan_action_enum_type chan_action;
}e_dl_chan_cfg_info_struct_type;
#endif /*FEATURE_WCDMA_HS_RACH*/
/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/
/* the agch threshold value used to detect false alarms */
extern uint32 agch_threshold[WL1_MAX_UL_CARR_IDX];

extern e_dl_config_db_struct_type  e_dl_config_db;

extern e_dl_chan_dbl_buf_logging_struct_type e_dl_hich_dbl_buf_logging;
extern e_dl_chan_dbl_buf_logging_struct_type e_dl_rgch_dbl_buf_logging;
extern e_dl_agch_dbl_buf_logging_struct_type e_dl_agch_dbl_buf_logging;

/* This structure is defined so that the number of RLs in the e_dl_config_db
** are arranged consecutively with no holes in the array.*/
extern e_dl_active_rl_info_struct_type e_dl_active_rl_info[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][EUL_MAX_RL];
/* The soft combining index for HICH and RGCH stored as an array. It is 
** two dimensional array with the dB pages also included*/
extern uint8 hich_soft_comb_idx[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][EUL_MAX_RL];
extern uint8 rg_soft_comb_idx[WL1_MAX_UL_CARR_IDX][E_DL_CONFIG_DB_MAX_PAGES][EUL_MAX_RL];
/* These global variables are for reading the per target sub FN result
** buffers. These have been defined as globals to reduce passing 
** these parameters to function and to keep things modular*/
extern uint8 e_dl_read_idx_ag_hich_buf[WL1_MAX_UL_CARR_IDX];
extern uint8 e_dl_read_idx_ns_rg_buf[WL1_MAX_UL_CARR_IDX];
extern uint16 e_dl_logging_bmsk;

/* These are the HICH and RGCH result buffer read pointers 
** used by the SW.*/
extern volatile uint16 * edl_hich_buf_read_ptr;
extern volatile uint16 * edl_rgch_buf_read_ptr;

/* The Serving Cell Change (SCC) type when EDL is active
** This will be used to fill information in the EUL_SERVING_CELL_CHANGE event*/
extern e_dl_scc_during_hho_enum_type e_dl_scc_during_hho;

/* Boolean that keeps track of a forced sync A when EUL is active*/
extern boolean e_dl_active_during_forced_sync_a;

/* This flag is an indication to MAC that there is a serving EDCH RLS change */
extern boolean e_dl_edch_serv_cell_and_rls_change_ind;

/* Flag to keep track of simultaneous UL and DL drop for HHO*/
extern boolean e_dl_simultaneous_ul_dl_drop;

/* This global will indicate whether or not to activate the false alarm detection, 
** based on number of serving rgch. If there are more than one serving RGCH,
** then the false alarm detection will be turned OFF*/
extern boolean e_dl_rgch_fa_algo_single_cell_active[WL1_MAX_UL_CARR_IDX];


#if defined(FEATURE_QTA) && defined(FEATURE_WCDMA_CPC_DRX)
extern boolean e_ul_cpc_drx_was_in_qta_gap;
extern boolean e_ul_cpc_drx_grant_ind;
#endif

/* =======================MACROS ============================*/


/* MACRO to Convert to Q10 format (Shift left by 10) */
#define AGCH_CONVERT_TO_Q10(x)  (x << 10)

#define AGCH_COMPUTE_THRESHOLD_AT_CRC_FAIL_DTX(agch_threshold, agch_energy, agch_param_a, agch_param_b)\
         ((agch_threshold - (agch_threshold >> agch_param_a)) + \
         ((agch_energy * agch_param_b) >> agch_param_a))

#define AGCH_COMPUTE_THRESHOLD_AT_DECODE_VALID(agch_threshold, agch_param_a)\
         (agch_threshold - (agch_threshold >> agch_param_a))


#define IS_EUL_ACTIVE() (e_dl_config_db.e_dl_state == E_DL_STATE_ACTIVE)
#define IS_EUL_ACTIVE_OR_RECFG() ((e_dl_config_db.e_dl_state == E_DL_STATE_ACTIVE) ||(e_dl_config_db.e_dl_state == E_DL_STATE_CONFIG_RECFG))
#define IS_EUL_INACTIVE() (e_dl_config_db.e_dl_state == E_DL_STATE_INACTIVE)

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define IS_DC_HSUPA_CFGD() ((e_dl_config_db.e_dl_secondary_state != MCALWCDMA_E_DL_SECONDARY_STATE_INIT) && \
                    (e_dl_config_db.e_dl_secondary_state != MCALWCDMA_E_DL_SECONDARY_STATE_CONFIG_STOP))
#else
#define IS_DC_HSUPA_CFGD() (FALSE)
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA
/*DC HSUPA is already cfgd or getting cfgd/re-cfgd*/
#define IS_DC_HSUPA_ACTIVE_OR_RECONFIG_ACTIVE() \
         ((e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_ACTIVE) || \
          (((e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CONFIG_RECONFIG) || \
           (e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CONFIG_START))&& \
            (e_dl_config_db.e_dl_next_secondary_state == E_DL_SECONDARY_STATE_ACTIVE)))

#define IS_DC_HSUPA_IN_CONFIG() \
          ((e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CONFIG_START) || \
            (e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CONFIG_RECONFIG) || \
              (e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CONFIG_STOP))


#define IS_SEC_EDL_ACTIVE_OR_CFG_RECFG() \
    ((e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CFGD) || \
            (e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_ACTIVE) || \
              (e_dl_config_db.e_dl_secondary_state == E_DL_SECONDARY_STATE_CONFIG_RECONFIG))
#endif /*FEATURE_WCDMA_DC_HSUPA */


/* If the E_DL_STATE is not active and if it is not inactive also, then EUL DL should be in config state*/
#define IS_EUL_IN_CONFIG() (e_dl_config_db.e_dl_state != E_DL_STATE_INACTIVE)&&(e_dl_config_db.e_dl_state != E_DL_STATE_ACTIVE)
#define E_DL_GET_EUL_DL_STATE() e_dl_config_db.e_dl_state

/* Macros to check whether the current TTI configured is for 10ms r 2ms*/
#define IS_E_CURR_TTI_10MS() (e_dl_config_db.curr_eul_tti == E_TTI_10MS)
#define IS_E_CURR_TTI_2MS() (e_dl_config_db.curr_eul_tti == E_TTI_2MS)

/* Macros to check whether the ordered TTI configured is for 10ms r 2ms*/
#define IS_E_ORD_TTI_10MS() (e_dl_config_db.ord_eul_tti == E_TTI_10MS)
#define IS_E_ORD_TTI_2MS() (e_dl_config_db.ord_eul_tti == E_TTI_2MS)

/* Macro to be called by WMCVS driver to see if EUL 2ms is Active 
** If EUL 2ms is active and CM is active, the Clock will be bumped up to 576 MHz
*/
#define IS_ACTIVE_E_TTI_2MS() \
          (((e_ul_edpch_state == E_UL_EDPCH_STATE_TX) ||  \
              (e_ul_edpch_state == E_UL_EDPCH_STATE_SETUP)) &&  \
                (IS_E_CURR_TTI_2MS()))

/* Macro to check whether the reconfig is because of TTI change*/
#define IS_EUL_IN_TTI_RECFG() (e_dl_config_db.eul_tti_changed == TRUE)

/* The method to compute the Write and Read index are based on sub FN 
** for 2ms and based on CFN in 10ms.*/
#define EDL_COMPUTE_RES_BUF_WRITE_IDX_10MS(e_dl_sw_buf_idx, target_sub_fn) \
           e_dl_sw_buf_idx = (((target_sub_fn+1)/5) & 0x3)
#define EDL_COMPUTE_RES_BUF_WRITE_IDX_2MS(e_dl_sw_buf_idx, target_sub_fn) \
           e_dl_sw_buf_idx = (target_sub_fn & 0x3)
#define EDL_COMPUTE_RES_BUF_WRITE_IDX_NS_RG_2MS(e_dl_sw_buf_idx, target_sub_fn) \
           e_dl_sw_buf_idx = (target_sub_fn % EUL_MAX_NON_SRV_RGCH_RESULT_BUF_LEN)

#define EDL_COMPUTE_RES_BUF_READ_IDX_10MS( eul_target_cfn) \
            (((eul_target_cfn)%256) & 0x3)

#define EDL_COMPUTE_RES_BUF_READ_IDX_2MS_AGCH_HICH( target_sub_fn)\
           (((target_sub_fn)%(256 * 5))& 0x3)

#define EDL_COMPUTE_RES_BUF_READ_IDX_2MS_NS_RG(target_sub_fn) \
           (((target_sub_fn) % (256 * 5)) % EUL_MAX_NON_SRV_RGCH_RESULT_BUF_LEN)

/* Macros to get the new HICH/RGCH CFG active sub fn. This sub fn is the time when
** ordered pages for soft combining indices should be used.*/
#define EDL_GET_NEW_HICH_CFG_ACTIVE_SUB_FN(carr_idx) e_dl_config_db.hich_cfg_active_sub_fn[carr_idx]
#define EDL_GET_NEW_RGCH_CFG_ACTIVE_SUB_FN(carr_idx) e_dl_config_db.rgch_cfg_active_sub_fn[carr_idx]
#define EDL_GET_NEW_AGCH_CFG_ACTIVE_SUB_FN(carr_idx) e_dl_config_db.agch_cfg_active_sub_fn[carr_idx]

#define EDL_SET_NEW_HICH_CFG_ACTIVE_SUB_FN(value,i) \
  (e_dl_config_db.hich_cfg_active_sub_fn[i] = value)
#define EDL_SET_NEW_RGCH_CFG_ACTIVE_SUB_FN(value,i) \
  (e_dl_config_db.rgch_cfg_active_sub_fn[i] = value)
#define EDL_SET_NEW_AGCH_CFG_ACTIVE_SUB_FN(value,i) \
  (e_dl_config_db.agch_cfg_active_sub_fn[i] = value)

/*Macro to check whether HICH is ACK or NACK*/
#define EDL_COMPUTE_HICH_COMB_VAL(value, eng, thresh) \
  if( eng > thresh) \
  { \
    value = EUL_HICH_ACK_VAL; \
  } \
  else \
  { \
    value = EUL_HICH_NACK_VAL; \
  }

/* Macro to check whether S-RG is UP/DOWN/HOLD*/
#define EDL_COMPUTE_S_RGCH_COMB_VAL(value, eng, thresh) \
  if(eng > thresh) \
  { \
    value = EUL_SRV_RG_UP_VAL;\
  }\
  else if(eng < -thresh) \
  { \
    value = EUL_SRV_RG_DOWN_VAL;\
  }\
  else\
  {\
    value = EUL_SRV_RG_HOLD_VAL;\
  }
/* Macro to compute the per cell HICH val*/
#define EDL_COMPUTE_HICH_PER_CELL_VAL(value, eng, thresh) \
  if( eng > thresh) \
  { \
    value = EUL_HICH_ACK_VAL; \
  } \
  else if(eng < -thresh)\
  { \
    value = EUL_HICH_NACK_VAL; \
  } \
  else \
  {\
    value = 0;\
  }
/* Macro to check whether NS-RG is DOWN/HOLD*/
#define EDL_COMPUTE_NS_RGCH_VAL(value, eng, thresh) \
  if(eng < -thresh) \
  { \
    value = EUL_SRV_RG_DOWN_VAL;\
  }\
  else\
  {\
    value = EUL_SRV_RG_HOLD_VAL;\
  }

/* Defining macros to extract the RG/TPC information current/ordered pages*/
#define  EDL_GET_RG_TPC_INFO_CUR_PAGE(carr_idx)  (e_dl_config_db.rg_tpc_info_curr_page[carr_idx])
#define  EDL_GET_RG_TPC_INFO_ORD_PAGE(carr_idx)  (e_dl_config_db.rg_tpc_info_ord_page[carr_idx])

#define  EDL_ALLOCATE_RG_TPC_INFO_ORD_PAGE(carr_idx)  \
           (e_dl_config_db.rg_tpc_info_ord_page[carr_idx] = (\
            (EDL_GET_RG_TPC_INFO_CUR_PAGE(carr_idx) == E_DL_CONFIG_DB_PAGE_TWO) ? \
             E_DL_CONFIG_DB_PAGE_ONE : E_DL_CONFIG_DB_PAGE_TWO))
#define  EDL_SET_RG_TPC_INFO_CUR_PAGE(page, carr_idx)  (e_dl_config_db.rg_tpc_info_curr_page[carr_idx] = page)
#define  EDL_SET_RG_TPC_INFO_ORD_PAGE(page, carr_idx)  (e_dl_config_db.rg_tpc_info_ord_page[carr_idx] = page)

#define EDL_GET_AGCH_CORRECTION_FACTOR(page, carr_idx) \
            e_dl_config_db.agch_info[carr_idx].agch_info_page[page].correction_factor

#define EDL_GET_HICH_CORRECTION_FACTOR(page, e_rl_idx, carr_idx) \
            e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].hich_info_page[page].correction_factor

#define EDL_GET_RGCH_CORRECTION_FACTOR(page, e_rl_idx, carr_idx) \
            e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].rgch_info_page[page].correction_factor

/*Defining Macros to compute the different channel offsets from PCCPCH and DPCH timeline*/
#define EDL_COMPUTE_TAU_HICH_10MS(tau_dpch)  \
            ((STMR_TICKS_PER_FRAME + (-70 + (((tau_dpch + 20) / 30) * 30))) % STMR_TICKS_PER_FRAME)

#define EDL_COMPUTE_TAU_HICH_2MS(tau_dpch)  \
            ((STMR_TICKS_PER_FRAME + (50 + (((tau_dpch + 20) / 30) * 30))) % STMR_TICKS_PER_FRAME)

  /*Macro to be used by EUL module to get the TPC index of an RL*/
  #define EUL_GET_ACTIVE_RL_TPC_IDX(rl_idx, carr_idx) \
                  e_dl_active_rl_info[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]][rl_idx].tpc_idx

  /*Macro to be used by EUL module to get the PSC of an RL*/
  #define EUL_GET_ACTIVE_RL_PSC(rl_idx, carr_idx) \
                  e_dl_active_rl_info[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]][rl_idx].psc

  /*Macro to be used by EUL module to get the RG comb idx */
  #define EUL_GET_ACTIVE_RL_RG_COMB_IDX(rl_idx, carr_idx) \
                  e_dl_active_rl_info[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]][rl_idx].rg_comb_idx

  /*Macro to be used by EUL module to get the idx into rgch result buf*/
  #define EUL_GET_ACTIVE_RL_RG_SC_IDX(rl_idx, carr_idx) \
                  e_dl_active_rl_info[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]][rl_idx].rg_sc_idx

  /*Macro to be used by EUL module to get the idx into hich result buf*/
  #define EUL_GET_ACTIVE_RL_HICH_SC_IDX(rl_idx, carr_idx) \
                  e_dl_active_rl_info[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]][rl_idx].hich_sc_idx

  /*Macro to be used by EUL module to get the num RG RLS */
  #define EUL_GET_NUM_RG_RLS(carr_idx) e_dl_config_db.num_rls[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]]

  /*Macro to be used by EUL module to get the num hich RLS */
  #define EUL_GET_NUM_HICH_RLS(carr_idx) e_dl_config_db.num_tpc_set[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]]

  /*Macro to be used by EUL module to get whether this RL is serving cell or not*/
  #define EUL_GET_ACTIVE_RL_IS_SERV_CELL(rl_idx, carr_idx) \
                  e_dl_active_rl_info[carr_idx][e_dl_config_db.rg_tpc_info_curr_page[carr_idx]][rl_idx].serv_cell



/* Macro to get the cell index using the e_rl_idx*/
#define EDL_GET_CELL_IDX(e_rl_idx,idx)  e_dl_config_db.e_dl_rl_info[idx][e_rl_idx].cell_idx
/* These MACROs will be called when the RL in EDXH ASET is dropped as a part
** of R99. The entry from triage will be removed and hence we need an indication
** within EUL DL Database to check and not query the triage for information*/
#define EDL_SET_RL_DROPPED_R99_ASET_FLAG(e_rl_idx, val, carr_idx) \
  e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].rl_dropped_from_r99_aset = val
#define EDL_GET_RL_DROPPED_R99_ASET_FLAG(e_rl_idx, carr_idx) \
  e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].rl_dropped_from_r99_aset

#define E_DL_GET_DPCH_STTD_FOR_CELL(dpch_handle, cell_db_idx)  \
  (dl_phch_get_txdiv_info_for_cell(dpch_handle, cell_db_idx) != MCALWCDMA_TX_DIV_NONE)

/* Macros to get/set current/ordered channel page and to allocate 
 * ordered channel page
 */
#define  EDL_GET_AGCH_CUR_PAGE(carr_idx)  (e_dl_config_db.agch_info[carr_idx].cur_page)
#define  EDL_GET_AGCH_ORD_PAGE(carr_idx)  (e_dl_config_db.agch_info[carr_idx].ord_page)


#define  EDL_ALLOCATE_LOCAL_AGCH_ORD_PAGE(carr_idx) \
           ((EDL_GET_AGCH_CUR_PAGE(carr_idx) == E_DL_CONFIG_DB_PAGE_TWO) ? \
             E_DL_CONFIG_DB_PAGE_ONE : E_DL_CONFIG_DB_PAGE_TWO)


#define  EDL_SET_AGCH_CUR_PAGE(page,carr_idx)  (e_dl_config_db.agch_info[carr_idx].cur_page = page)
#define  EDL_SET_AGCH_ORD_PAGE(page,carr_idx)  (e_dl_config_db.agch_info[carr_idx].ord_page = page)

#define  EDL_GET_HICH_CUR_PAGE(e_dl_rl_idx,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].hich_cur_page)
#define  EDL_GET_HICH_ORD_PAGE(e_dl_rl_idx,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].hich_ord_page)

#define  EDL_ALLOCATE_LOCAL_HICH_ORD_PAGE(e_dl_rl_idx, carr_idx) \
           ((EDL_GET_HICH_CUR_PAGE(e_dl_rl_idx, carr_idx) == E_DL_CONFIG_DB_PAGE_TWO) ? \
             E_DL_CONFIG_DB_PAGE_ONE : E_DL_CONFIG_DB_PAGE_TWO)

#define  EDL_SET_HICH_CUR_PAGE(e_dl_rl_idx,page,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].hich_cur_page = page)
#define  EDL_SET_HICH_ORD_PAGE(e_dl_rl_idx,page,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].hich_ord_page = page)

#define  EDL_GET_RGCH_CUR_PAGE(e_dl_rl_idx,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].rgch_cur_page)
#define  EDL_GET_RGCH_ORD_PAGE(e_dl_rl_idx,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].rgch_ord_page)


#define  EDL_ALLOCATE_LOCAL_RGCH_ORD_PAGE(e_dl_rl_idx, carr_idx) \
           ((EDL_GET_RGCH_CUR_PAGE(e_dl_rl_idx, carr_idx) == E_DL_CONFIG_DB_PAGE_TWO) ? \
             E_DL_CONFIG_DB_PAGE_ONE : E_DL_CONFIG_DB_PAGE_TWO)

#define  EDL_SET_RGCH_CUR_PAGE(e_dl_rl_idx,page,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].rgch_cur_page = page)
#define  EDL_SET_RGCH_ORD_PAGE(e_dl_rl_idx,page,carr_idx)  (e_dl_config_db.e_dl_rl_info[carr_idx][e_dl_rl_idx].rgch_ord_page = page)

/* MACRO to check whether EDL state is in config start, config reconfig*/
#define IS_EDL_STATE_CONFIG_START()  e_dl_config_db.e_dl_state == E_DL_STATE_CONFIG_START
#define IS_EDL_STATE_CONFIG_RECFG()  e_dl_config_db.e_dl_state == E_DL_STATE_CONFIG_RECFG

#define EDL_COMPUTE_AGCH_DPCH_OFFSET(tau_dpch)  \
            ((STMR_TICKS_PER_FRAME + AGCH_PCCPCH_OFFSET_IN_TICKS - tau_dpch) % STMR_TICKS_PER_FRAME)

#define EDL_COMPUTE_HICH_DPCH_OFFSET(tau_hich, tau_dpch)  \
            ((STMR_TICKS_PER_FRAME + tau_hich - tau_dpch) % STMR_TICKS_PER_FRAME)

#define EDL_COMPUTE_RGCH_DPCH_OFFSET(tau_dpch)  \
            ((STMR_TICKS_PER_FRAME + AGCH_PCCPCH_OFFSET_IN_TICKS - tau_dpch) % STMR_TICKS_PER_FRAME)


#define EDL_GET_WEIGHTED_AVERAGE(result, energy,weight) \
            (result+ = energy*weight)

#define EDL_GET_NUM_CELLS_PER_TPCSI(num_cells, tpc_idx_val, idx) \
  for(idx = 0; idx < MAX_NUM_TPC_IDX; idx++) \
  {\
    if(tpc_idx_val == e_dl_config_db.e_dl_tpc_set_info[idx].tpc_idx)\
    {\
      num_cells = e_dl_config_db.e_dl_tpc_set_info[idx].num_cells;\
    }\
  }\

  #define EDL_GET_NUM_CELLS_S_RG(num_cells, rg_comb_idx_val, idx) \
  for(idx = 0; idx < MAX_NUM_TPC_IDX; idx++) \
  {\
    if(rg_comb_idx_val == e_dl_config_db.e_dl_rls_info[idx].rg_comb_idx)\
    {\
      num_cells = e_dl_config_db.e_dl_rls_info[idx].num_cells;\
    }\
  }\

  /* Macro to update the farthest Channel to DPCH offset in the e_dl_config db*/
  #define E_DL_UPDATE_FARTHEST_CH2DPCH_OFFSET(offset,carr_idx)\
      e_dl_config_db.farthest_ch2dpch_offset[carr_idx] =\
           (offset > e_dl_config_db.farthest_ch2dpch_offset[carr_idx]) ? offset : e_dl_config_db.farthest_ch2dpch_offset[carr_idx];

  /* Macro to update the ch to DPCH delta slots and if ch and DPCH are slot aligned */
  #define E_DL_UPDATE_CM_SLOT_INFO(ch2dpch_offset,ch_cm_info)\
    ch_cm_info.ch2dpch_delta_slots = (ch2dpch_offset / 10);\
    ch_cm_info.ch2dpch_slot_aligned = (((ch2dpch_offset % 10) == 0) ? TRUE : FALSE)

  /* Macro to update the ch to DPCH frame alignment flag */
  #define E_DL_UPDATE_CM_FRAME_ALIGNMENT_INFO(cfn,start_sub_fn,ch_cm_info)\
    ch_cm_info.ch2dpch_cfn_subfn_aligned = (((cfn & 0x1) == (start_sub_fn & 0x1)) ? TRUE : FALSE)

  /* Macro to get the TPC index for this RL*/
  #define E_DL_GET_THIS_RL_TPC_IDX(e_rl_idx, carr_idx) \
    (e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].e_rl_tpc_idx)

  /* Macro to get the RG COMB index for this RL*/
  #define E_DL_GET_THIS_RL_RG_COMB_IDX(e_rl_idx, carr_idx) \
    (e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].e_rl_rg_comb_idx)

  /*Macro to be used by EUL module  to get the serving RL index*/
  #define EUL_GET_SRV_CELL_IDX(carr_idx) (e_dl_config_db.e_serv_rl_idx[carr_idx])

  /*Macro to be used by EUL module to get the number of RLs in EUL*/
  #define EUL_GET_EDCH_NUM_CELLS(carr_idx) (e_dl_config_db.num_e_rl[carr_idx])

  /* Macro for reading the flag for serving RLS change that is indicated to MAC for
  ** SI trigger.*/
  #define EDL_GET_SERV_RLS_CHANGE_FLAG() e_dl_edch_serv_cell_and_rls_change_ind

  /*Macro to set the SERVING CELL CHANGE IND to MAC as FALSE*/
  #define EDL_SET_SERV_RLS_CHANGE_FLAG(value) \
      e_dl_edch_serv_cell_and_rls_change_ind = value


  /* MACRO to set the EDL SCC information during HHO*/
  #define EDL_SET_SCC_TYPE_DURING_HHO(scc_type) \
    e_dl_scc_during_hho = scc_type

  /* MACRO to set the FORCED SYNC A flag when EDL is active*/
  #define EDL_SET_FORCED_SYNC_FLAG(value) \
    e_dl_active_during_forced_sync_a = value

  /*These #defines are used to fill the action field for the RECONFIG OR ASU
  ** event  for EUL*/
  #define EUL_RECFG_OR_ASU_EVENT_ACTION_START                   0
  #define EUL_RECFG_OR_ASU_EVENT_ACTION_STOP                    1
  #define EUL_RECFG_OR_ASU_EVENT_ACTION_RECFG_ASET_CHANGE       2
  #define EUL_RECFG_OR_ASU_EVENT_ACTION_RECFG_NO_ASET_CHANGE    3

  /*These #defines are used to fill the action and type field of the EUL serving cell
  ** change event SCC -> Serving Cell Change*/
  #define EUL_SCC_EVENT_ACTION_HARD 0
  #define EUL_SCC_EVENT_ACTION_SOFT 1
  #define EUL_SCC_EVENT_ACTION_SOFTER 2

  #define EUL_SCC_EVENT_TYPE_TIMING_REINIT 0
  #define EUL_SCC_EVENT_TYPE_TIMING_MAINT 1

  #define E_DL_IS_CONFIG_DB_PAGE_VALID(page)  (page != E_DL_CONFIG_DB_INVALID_PAGE)

  #define E_DL_IS_CONFIG_DB_PAGE_INVALID(page)  (page == E_DL_CONFIG_DB_INVALID_PAGE)

  #ifdef FEATURE_WCDMA_HS_RACH
  #define EDL_SET_HS_RACH_E_DL_RL_IDX(rl_idx) \
  	        e_dl_config_db.hs_rach_e_dl_rl_idx = rl_idx

  #define EDL_GET_HS_RACH_E_DL_RL_IDX() e_dl_config_db.hs_rach_e_dl_rl_idx
  
  #define EDL_SET_HS_RACH_E_DL_AGCH_CELL_IDX(agch_cur_page, val, carr_idx) \
          e_dl_config_db.agch_info[carr_idx].agch_info_page[agch_cur_page].cell_idx = val
  #define EDL_SET_HS_RACH_E_DL_HICH_CELL_IDX(hs_rach_e_dl_rl_idx, hich_cur_page, val, carr_idx) \
          e_dl_config_db.e_dl_rl_info[carr_idx][hs_rach_e_dl_rl_idx].hich_info_page[hich_cur_page].cell_idx = val

  #define EDL_SET_CELL_IDX(e_rl_idx, val, carr_idx)  e_dl_config_db.e_dl_rl_info[carr_idx][e_rl_idx].cell_idx = val
  #endif /**/

/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION        E_DL_AGCH_THRESHOLD_INIT

DESCRIPTION     This function initializes the agch threshold value to zero.
                This function is called whenever EUL serving cell is established or changed.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_dl_agch_threshold_init (void);


/*===========================================================================
FUNCTION       E_DL_AGCH_THRESHOLD_UPDATE

DESCRIPTION     This function implements E-AGCH enhanced detection algorithm.
                Based on AGCH energy decoded, the function updates E-AGCH threshold
                whenever CRC passes or fails.
                CRC Fails - 1. Based on the observed error count, AGCH Result value is decided
                            2. Based on the AGCH Result value decided above, update agch_threshold
 
                CRC Pass  - 1. Based on observed error count, energy metric and the last calculated
                               agch_threshold, AGCH Result value is decided
                            2. Based on the AGCH Result value decided above, update agch_threshold

DEPENDENCIES    None

RETURN VALUE    AGCH RESULT VALUE (enum type)

SIDE EFFECTS    None
===========================================================================*/
extern e_dl_agch_decode_result_type e_dl_agch_threshold_update(boolean agch_crc_pass,
                                                               uint32 agch_energy, 
                                                               uint32 agch_err_cnt, 
                                                               wl1_ul_carr_id_enum_type carr_idx);


/*===========================================================================

FUNCTION        EUL_VALIDATE_CONFIG

DESCRIPTION     This function validates the EUL DL and UL config as received
                in the CPHY_SETUP_REQ command from RRC

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern boolean eul_validate_config(l1_setup_cmd_type* setup);

/*===========================================================================

FUNCTION        E_DL_PHYCHAN_CFG_VALIDATE

DESCRIPTION     This function validates the EUL DL config as received
                in the CPHY_SETUP_REQ command from RRC

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern boolean e_dl_phychan_cfg_validate(l1_setup_cmd_type *setup);

/*===========================================================================

FUNCTION        E_DL_VALIDATE_CONFIG

DESCRIPTION     This function validates the EUL DL config as received
                in the CPHY_SETUP_REQ command from RRC

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern boolean e_dl_validate_rl(l1_setup_cmd_type *setup, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        EUL_DROP_UL_DL

DESCRIPTION     This function drops both EUL UL and DL channels 

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern void eul_drop_ul_dl(void);

/*===========================================================================

FUNCTION        E_DL_INIT_ON_POWER_UP

DESCRIPTION     This function initializes EUL DL data structures on power up

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database and other globals initialized

===========================================================================*/
extern void e_dl_init(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_CONFIG_FLUSH_LOG_PKTS

DESCRIPTION     This function flushed EUL log packets when EUL_STOP or 
                EUL_RECFG is received. This only includes 0x4324,
                0x4325, 0x4326

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void e_dl_config_flush_log_pkts(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_CONFIG

DESCRIPTION     This function configures the EUL DL info into ordered config
                EUL data base and sets up the EUL COMB CHAN CFG EVT to proceed 
                further 

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern void e_dl_config(uint8 e_req_mask, l1_setup_cmd_type *setup_cmd, wl1_ul_carr_id_enum_type carr_idx);


/*===========================================================================

FUNCTION        E_DL_CONFIG_START

DESCRIPTION     This function handles EUL DL start request

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
void e_dl_config_start(l1_e_dl_info_struct_type *e_dl_info_ptr, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_CONFIG_RECFG

DESCRIPTION     This function handles EUL DL recfg request

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
void e_dl_config_recfg(l1_e_dl_info_struct_type *e_dl_info_ptr, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_CONFIG_STOP

DESCRIPTION     This function handles EUL DL stop request

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
void e_dl_config_stop(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_UPDATE_EUL_CONFIG_IN_DB

DESCRIPTION     This function updates the EUL cell idx info in existing R99
                databases. This also registers a callback function with 
                triage for any finger changes.

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    R99 databases updated

===========================================================================*/
extern void e_dl_add_eul_cfg_in_r99_db(void);

/*===========================================================================

FUNCTION        E_DL_TIMING_INFO_UPDATE

DESCRIPTION     This function updates the start_sub_fn and final_sub_fn for 
                all the configured EUL DL channels based on their timing wrt 
                to nominal DPCH. 
                       
                NOTE: This function just calculates and stores the delta to
                      UL from the DL fn, for configuring the channels. The
                      actual cfn is added to this delta in the DL config
                      window.

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern void e_dl_timing_info_update(void);

/*===========================================================================

FUNCTION        E_DL_SWITCH_CONFIG

DESCRIPTION     This function switches EUL DL config db from current to 
                ordered for the configured channels

DEPENDENCIES    Unknown 

RETURN VALUE    Allocated cell id

SIDE EFFECTS    EUL DL mapping info table updated

===========================================================================*/
extern void e_dl_switch_config(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_ALLOCATE_CELL_ID

DESCRIPTION     This function allocates a new entry in the e_dl_mapping_info
                array and fills in the corresponding dl_rl_db_id and 
                triage psc db_id

DEPENDENCIES    Unknown 

RETURN VALUE    Allocated cell id

SIDE EFFECTS    EUL DL mapping info table updated

===========================================================================*/
extern uint16 e_dl_allocate_cell_id(uint16 psc);

/*===========================================================================

FUNCTION        E_DL_QUERY_CELL_IDX_FOR_PSC

DESCRIPTION     This function finds the cell_idx corresponding to the passed
                in PSC

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
uint16 e_dl_query_cell_idx_for_psc(uint16 psc);

/*===========================================================================

FUNCTION        E_DL_SCHEDULE_COMB_CHAN_CFG_EVT

DESCRIPTION     This function schedules the combiner channel configure event
                for EUL DL configuration 

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL combiner channel config event scheduled

===========================================================================*/
extern void e_dl_schedule_comb_chan_cfg_evt(void);

/*===========================================================================

FUNCTION        E_DL_COMB_CHAN_CFG_EVT_HANDLER

DESCRIPTION     This function handles the combiner channel configure event
                for EUL DL configuration 

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_comb_chan_cfg_evt_handler(void);

/*===========================================================================

FUNCTION        E_DL_CALC_START_FINAL_FN_AND_PATCH

DESCRIPTION     This function calculates the start and final fn for all
                configured channels and patches the calculated values into
                the mDSP comb cfg shadow buffer

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_calc_start_final_sub_fn_and_patch(void);

/*===========================================================================

FUNCTION        E_DL_SEND_COMB_CHAN_CFG_CMD

DESCRIPTION     This function sends the EUL COMB CHAN CFG local cmd

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_send_comb_chan_cfg_cmd (void);

/*===========================================================================

FUNCTION        E_DL_COMB_CHAN_CFG_CMD_HANDLER

DESCRIPTION     This function handles the combiner channel configure local
                command for EUL DL configuration 

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_comb_chan_cfg_cmd_handler(void);

/*===========================================================================

FUNCTION        E_DL_SCHEDULE_FRM_BDRY_EVT

DESCRIPTION     This function schedules the combiner channel configure event
                for EUL DL configuration 

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    EUL DL combiner channel config event scheduled

===========================================================================*/
extern void e_dl_schedule_frm_bdry_evt(void);

/*===========================================================================

FUNCTION        E_DL_COMB_CHAN_FRM_BDRY_EVT_HANDLER

DESCRIPTION     This function handles the combiner channel frame boundary
                event for EUL DL configuration

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_comb_chan_frm_bdry_evt_handler(void);

/*===========================================================================

FUNCTION        E_DL_SEND_FRM_BDRY_CMD

DESCRIPTION     This function sends the EUL FRM BDRY local cmd

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_send_frm_bdry_cmd (void);

/*===========================================================================

FUNCTION        E_DL_FRM_BDRY_CMD_HANDLER

DESCRIPTION     This function handles the combiner channel frame boundary
                local command for EUL DL configuration

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_comb_chan_frm_bdry_cmd_handler(void);

/*===========================================================================

FUNCTION        E_DL_CHECK_SERV_RLS_CHANGE

DESCRIPTION     This function checks whether there is a scope for some implicit
                reconfig in the form of Serving RLS change or a change in the tau DPCH
                values.

DEPENDENCIES    Unknown 

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/

extern void e_dl_check_serv_rls_change(l1_e_dl_info_struct_type *e_dl_info_ptr,
                                       uint8 new_serv_rg_comb_idx, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_DROP_RL

DESCRIPTION      This function drops the RL specified in the current EDL
               config db.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_drop_rl(uint8 rl_idx, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_CHECK_AND_UPDATE_TAU_DPCH_CHANGE

DESCRIPTION      This function is called if there is a tau DPCH change in an
                 RL. The parameters passed are the new tau DPCH value and 
                 PSC of the RL. This function loops through the EDL database 
                 and finds the whether this PSC exists in its database. Also 
                 checks whether this RL has some add or recfg information. 
                 And then sets the CPHY_SETUP_REQ to trigger a E_DL_RECFG, 
                 to impact the tau dpch change.


DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     The Higher level req_mask and e_req_mask are updated to
                  reflect the change in the Tau DPCH for triggering a reconfiguration.

===========================================================================*/
extern void e_dl_check_and_update_for_tau_dpch_change(uint16 psc, uint16 new_tau_dpch);

/*===========================================================================
FUNCTION         E_DL_CHECK_STTD_RECONFIG

DESCRIPTION      This function is called when there is a DL DPCH reconfiguration
                 in CPHY_SETUP_REQ command. The function checks if STTD setup of 
                 any of the existing RLs in EDCH active set needs to be reconfigured
                 due to DPCH tx diversity reconfig in RLs of R99 active set. If STTD 
                 needs to be reconfigured for any of the existing EDCH active set RLs,
                 it issues an implicit reconfig by setting req_mask, e_req_mask, and 
                 per-RL channel masks in CPHY_SETUP_REQ command approriately. 

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     The Higher level req_mask and e_req_mask are updated to
                 trigger a STTD reconfiguration.

===========================================================================*/
extern void e_dl_check_sttd_reconfig(void);

/*===========================================================================
FUNCTION         E_DL_ADD_RL

DESCRIPTION      This function adds a new RL to the EDCH RL list.
          

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_add_rl(uint8 recfg_idx, l1_e_dl_info_struct_type *e_dl_info_ptr,
                        uint8 serv_rg_comb_idx, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_RECFG_RL

DESCRIPTION      This function reconfigures the RL specified in the current EDL
               config db. This function also takes care of the tau DPCH change and also
               of the serving RLS change. rl_idx is the idx in the EDL config DB that 
               corresponds to the RL that needs to be reconfigured. recfg_idx, is the idx
               that corresponds to the entry in the RRC - L1 message, add and recfg info.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_recfg_rl(uint8 rl_idx, uint8 recfg_idx, 
                          l1_e_dl_info_struct_type *e_dl_info_ptr,
                          uint8 serv_rg_comb_idx, wl1_ul_carr_id_enum_type carr_idx);
/*===========================================================================
FUNCTION         E_DL_COMPLETE_CLEANUP

DESCRIPTION      This function completes the cleanup, if HSDPA is present,
          if not, R99 DCH cleanup.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_complete_cleanup(void);

/*===========================================================================
FUNCTION         E_DL_UPDATE_CELL_ID_MAPPING_INFO

DESCRIPTION      This function is called when there is an ASU and
                          EUL is also active. This function updates the cell id
                          mapping info database with the changed index to the
                          dl_rl_db after the ASU.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
void e_dl_update_cell_id_mapping_info(void);

/*===========================================================================
FUNCTION         E_DL_CHECK_ERNTI_CHANGED

DESCRIPTION      This functions checks whether the ERNTI (Primary or
                           Secondary) has changed and returns a boolean. This function
                           needs to called twice, once for primary and again for secondary
                           ERNTI changes.

DEPENDENCIES     None

RETURN VALUE     Boolean

SIDE EFFECTS     None

===========================================================================*/
extern boolean e_dl_check_ernti_changed(e_dl_agch_info_page_type* curr_page_ptr,
                                                                 e_dl_agch_info_page_type* ord_page_ptr);
/*===========================================================================
FUNCTION         E_DL_SUBMIT_DL_CHAN_INFO_LOG_PKT

DESCRIPTION      This function commits the DL CHAN INFO log packet 
                           This will be called from the function that is posted after the 
                           frame boundary command has been handled.

DEPENDENCIES     None

RETURN VALUE     Boolean

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_submit_dl_chan_info_log_pkt (void);

/*===========================================================================
FUNCTION         E_DL_SUBMIT_DL_DEC_INFO_LOG_PKT

DESCRIPTION      This function fills in the log pointer allocated for this log packet from the
                           global structure that has accumulated the information on a per sample basis.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_submit_dl_dec_info_log_pkt(void);

/*===========================================================================
FUNCTION         E_DL_UPDATE_RLS_RG_COMB_IDX_PER_RL

DESCRIPTION      This function updates the information about the RLS idx and the 
                           rg_comb_idx  for each of the RLs and also specifically the serv_rls_idx 
                           and serv_rg_comb_idx.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_update_rls_rg_comb_idx_per_rl(void);

/*===========================================================================
FUNCTION         E_DL_INIT_DEC_LOG_PKT

DESCRIPTION      This function initializes important fields of the dl_ded log packet 
                           per sample structure.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_init_dec_log_pkt(void);

/*===========================================================================
FUNCTION         E_DL_CALC_NUM_RLS_TPCSET_CURR_CONFIG

DESCRIPTION      This function computes the number of RLS and TPC sets in the
                           the number of active EUL RLs.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_calc_num_rls_tpcset_curr_config(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_CALC_INDEX_FOR_SOFT_COMBINING

DESCRIPTION      This function computes the soft combining index. These indices will be used to
                    populate the soft combined energy/val arrays when reading results from MDSP.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_calc_index_for_soft_combining(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_UPDATE_THRESH_PARAMS_DURING_CONFIG

DESCRIPTION      This function updates HICH and RGCH threshold parameters,i.e.,
                 scaling 'a' and number of slots, when EUL DL channels are 
                 configured/reconfigured. The update is based on various factors:
                 TTI, single RL or SHO case, serving or non-serving TPC/RL set.
                 'a' values are chosen so as to satisfy various GCF EUL DL
                 performance requirements.

DEPENDENCIES     None   

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_update_thresh_params_during_config(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION            E_DL_INVALIDATE_SOFT_COMBINE_INDEX

DESCRIPTION      This function is called to invalidate the soft combine index for 
                           HICH and RGCH if there is a RL drop, and RGCH combine index
                           if there is a RG drop only. There can be a case where we have 
                           issued a config command to drop an RGCH but in frame N we
                           get the results before we could drop the channel. Then this result
                           should not be used in the soft combining as this channel is being 
                           dropped once and for all.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_invalidate_soft_combine_index(wl1_ul_carr_id_enum_type carr_idx);
/*===========================================================================
FUNCTION         E_DL_POPULATE_FING_LOG_INFO

DESCRIPTION      This function is called by triage module to populate the fing
                 log info for EUL and the hwch information. The hwch info
				 is regarding which channels are configured on the EUL
				 cell that is being tracked by the finger.

DEPENDENCIES     None

RETURN VALUE     The fing_eul_info byte that will be used for eul finger logging

SIDE EFFECTS     None

===========================================================================*/
extern uint8 e_dl_populate_fing_log_info(uint16 psc, PACKED uint16 *chan, dl_tri_carr_id_enum_type demod_carr_idx);

/*===========================================================================

FUNCTION        EUL_SEC_VALIDATE_CONFIG

DESCRIPTION     This function validates the EUL DL and UL config as received
                in the CPHY_SETUP_REQ command from RRC for the Secondary UL

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    EUL DL Sec config database updated

===========================================================================*/
extern boolean eul_sec_validate_config(l1_setup_cmd_type* setup);
/*===========================================================================
FUNCTION         E_DL_SIGNAL_EUL_SG_INFO_CHANGE

DESCRIPTION      This function is used to singal a SG change to EUL.
                 The function is once the validation is done and so that the 
                 at the activation time CFN, the SG can be activated.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_signal_eul_sg_info_change(wl1_ul_carr_id_enum_type carr_idx);
#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION         E_DL_LOG_INIT

DESCRIPTION      This function registers with the log on demand and also
                 sets the global flag to indicate whether valid log packet info 
                 present or not. It is initialized to FALSE.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
void e_dl_log_init(void);

/*===========================================================================
FUNCTION         E_DL_CHAN_INFO_LOG_ON_DEMAND

DESCRIPTION      This function checks whether the valid flag is TRUE.
                 If it is TRUE and EUL is active, then commit the log packet 


DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_chan_info_log_on_demand(void);
#endif
/*===========================================================================

FUNCTION        E_DL_IS_SERV_RLS_CHANGE_INFO

DESCRIPTION     This function gets whether the RL with this PSC is getting
                reconfigured because of serving RLS change
                it would be, after the reconfig procedure has completed.

DEPENDENCIES    Unknown 

RETURN VALUE    boolean

SIDE EFFECTS    Unknown

===========================================================================*/
boolean e_dl_is_serv_rls_changed(l1_e_dl_info_struct_type *e_dl_info_ptr, uint8 cur_serv_rg_comb_idx,
                                 uint8 new_serv_rg_comb_idx, wl1_ul_carr_id_enum_type carr_idx);





/*===========================================================================
FUNCTION         E_DL_SET_NEXT_CFN_CM_GAP_PARAMS

DESCRIPTION      This function checks and sets the CM slot bitmasks for the
                 next_cfn for each of the configured EUL DL channels. FW uses
                 this bitmask to DTX the slots where the CM gaps occur.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     EUL DL CM slot bitmasks for each channel are updated

===========================================================================*/
extern void e_dl_set_next_cfn_cm_gap_params(uint8 num_carr);

/*===========================================================================
FUNCTION        E_DL_CB_NOTIFY_EUL_PARENT_SVC_REMOVAL

FILE NAME       edl.c

DESCRIPTION     This routine notifies the EUL DL module that the R99 ASET cell with
                a specific cell index has been dropped. Within this function the EUL DL Database
                is updated to reflect this for that specific RL. This RL will be eventually dropped
                from the EDCH ASET also, but the drop will happen a few frames after the R99 ASET
                drop. For the few frames in between, the EUL DL module should abstain from calling the
                per RL triage for information for that cell index.

                This is a call back function called from dltri.c, from the remove per RL triage function
                when the R99ASET service is being removed.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_dl_cb_notify_eul_parent_svc_removal(uint8 cell_idx);

extern boolean e_dl_drop_precedes_asu_cmd_cphy_setup[WL1_MAX_UL_CARR_IDX];

/*===========================================================================
FUNCTION         E_DL_PREPARE_MCAL_DATA

DESCRIPTION      This function prepares the EDL config data for the MCAL
                 so that the configuration procedure can be done within MCAL.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     An MCAL data structure is updated.

===========================================================================*/
extern void e_dl_prepare_mcal_data(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================

FUNCTION        E_DL_UPD_CFG_DB_START_FINAL_SUB_FN_FROM_MCAL

DESCRIPTION     This function updates the start sub FN and other needed
                information that is updated in the MCAL structure and needs
                to be copied into the e_dl_config_db structure.
                
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_upd_cfg_db_start_final_sub_fn_from_mcal( wl1_ul_carr_id_enum_type carr_idx );


/*===========================================================================
FUNCTION         E_DL_INIT_PER_TARGET_SUB_FN_RESULT_BUFFER

DESCRIPTION      This accepts a boolean as TRUE if all the buffers needs to be initialized. 
                 Not all the information elements need to be initialized, only the AG val 
                 and the NS_RG val, as these are used to check by the SG module for the 
                 presence of valid information. The structure that has DL channel info for 
                 FIQ processing for next TTI TX. 
                 If the boolean is FALSE, then only the entry into the per target sub fn
                 structure that was read out this TTI FIQ handler is initialized.
                 This is called before exiting the FIQ handler.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     The per target sub FN strucuture is initialized.

===========================================================================*/
extern void e_dl_init_per_target_sub_fn_result_buffer( boolean all_buffs, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_SET_SW_BUF_READ_IDX

DESCRIPTION      This function accepts the target_sub_fn, i.e, the sub frame
                 number of the next transmission. Using this information, the function
                 computes the read index into the SW buffers. This read index will be
                 used later to fill in the results that are needed by SG module for the
                 target sub FN. The read index is used to index into the target sub frame
                 structure.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/
extern void e_dl_set_sw_buf_read_idx(uint32 target_sub_fn);

/*===========================================================================
FUNCTION         EUL_DL_UL_LOGGING_RESULTS_UPDATE

DESCRIPTION      This accepts the target_sub_fn as the parameter and extracts
                 the DL channel results for this target sub frame number to populate
                 the structure that is later used by EUL logging code. This structure will
                 be accessed using macros for 4309 log packet

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     A logging structure gets updated

===========================================================================*/
extern void e_dl_ul_logging_results_update(void);

/*===========================================================================
FUNCTION         E_DL_UL_LOGGING_GET_NUM_RLS_TPC_SET

DESCRIPTION      This function returns the number of RLS and TPC set to EUL
                 logging module

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

============================================================================*/
extern void e_dl_ul_logging_get_num_rls_tpc_set(uint8 *num_rls, uint8 *num_tpc_set, wl1_ul_carr_id_enum_type carr_idx);


/*===========================================================================
FUNCTION         E_DL_UL_RESET_LOGGING_RESULTS

DESCRIPTION      This function resets the rg and hich per cell info

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

============================================================================*/
extern void e_dl_ul_reset_logging_results(void);
/*===========================================================================
FUNCTION         E_DL_SUBMIT_DL_CHAN_RESULTS_LOGPKT

DESCRIPTION     This function that submits the DL channel results based on the bit mask

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

============================================================================*/
extern void e_dl_submit_dl_chan_results_logpkt(void);

#if defined(FEATURE_WCDMA_EUL_DL_THRESHOLD_PROGRAMMING) || !defined(FEATURE_WCDMA_JOLOKIA_MODEM)
/*===========================================================================

FUNCTION        E_DL_SET_HICH_THRESH_THROUGH_QXDM

DESCRIPTION     This function updates the appropriate HICH threshold parameter
                with the value set via QXDM. The first byte in the 4 byte 'val' set
                via QXDM indicates the RL type - single link, SHO non-serving HICH
                or SHO serving HICH. The second byte of "val" gives the TTI 
                information as well as the parameter type ('a' or 'b') to be updated.
                The last two bytes of 'val' contain the actual value to be set.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_set_hich_thresh_through_qxdm( uint32 val);

/*===========================================================================

FUNCTION        E_DL_SET_RGCH_THRESH_THROUGH_QXDM

DESCRIPTION     This function updates the appropriate RGCH threshold parameter
                with the value set via QXDM. The first byte in the 4 byte 'val' set
                via QXDM indicates the RL type - single link, SHO non-serving RGCH
                or SHO serving RGCH. The second byte of "val" gives the TTI 
                information as well as the parameter type ('a' or 'b') to be updated.
                The last two bytes of 'val' contain the actual value to be set.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_set_rgch_thresh_through_qxdm( uint32 val);
#endif /* FEATURE_WCDMA_EUL_DL_THRESHOLD_PROGRAMMING*/
/*===========================================================================
FUNCTION            E_DL_SUBMIT_EUL_RECFG_OR_ASU_EVENT

DESCRIPTION      This function is called to submit the physical layer
                 reconfig or ASU event for EUL.This uses the new config and
                 commits the event as to which cells have been added and 
                 which ones are unchanged.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     None

===========================================================================*/

extern void e_dl_submit_eul_recfg_or_asu_event(uint8 e_req_mask);
/*===========================================================================

FUNCTION        E_DL_UPDATE_S_RGCH_FALSE_ALARM_DET

DESCRIPTION     This function takes in the many parameters for 
                the new FALSE ALARM detection algo for S-RGCH. The 
                pointer to the RGCH Decision is passed and overwritten as 
                hold only when the condition is satisfied. 

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
 extern void e_dl_update_s_rgch_false_alarm_det(eul_sg_srv_rg_enum_type *rgch_val,
                                    eul_serv_rgch_results_struct_type * s_rg_info, uint16 serv_rl_idx);      


/*===========================================================================

FUNCTION        E_DL_L1M_CONFIG_HANDLER

DESCRIPTION     This function checks if EDL config received from L1M needs to
                be handled or not depending upon entry in EDL DB. If this PSC
                exists in EDL database, it invokes e_dl_config() to process 
                further.

DEPENDENCIES    Unknown

RETURN VALUE    Returns TRUE if PSC is found in EDL DB. Otherwise FALSE.

SIDE EFFECTS    EUL DL config database updated

===========================================================================*/
extern boolean e_dl_l1m_config_handler(uint8 l1m_e_req_mask, l1_e_dl_info_struct_type *l1m_e_dl_info_ptr, wl1_ul_carr_id_enum_type carr_idx);

#ifdef FEATURE_WCDMA_CPC_DRX
/*===========================================================================

FUNCTION        E_DL_INIT_EDL_DRX_PARAMS

DESCRIPTION     Reset EDL DRX params provided by EDL config module. Called by e_dl_init()

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    EUL DL config database and other globals initialized

===========================================================================*/
extern void e_dl_init_edl_drx_params(void);
#endif /*FEATURE_WCDMA_CPC_DRX*/

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================

FUNCTION        E_DL_CALC_DELTA_FRAMES_DECODE_TO_APPLY_AGCH

DESCRIPTION     This function calculates the number of frames from the frame
                the AGCH is decoded by the UE to the frame where the AGCH 
                grant is applied by the UE for the UL transmission based
                on the passed in AGCH to DPCH channel offset value
                          
DEPENDENCIES    Unknown

RETURN VALUE    delta frames between AGCH decode and AGCH grant application

SIDE EFFECTS    Unknown

===========================================================================*/
extern uint16 e_dl_calc_delta_frames_decode_to_apply_agch (uint16 agch2dpch_offset);
 

/*===========================================================================

FUNCTION        E_DL_CALC_DELTA_FRAMES_DECODE_TO_APPLY_HICH

DESCRIPTION     This function calculates the number of frames from the frame
                the HICH is decoded by the UE to the frame where the HICH 
                result is applied by the UE for the next UL transmission based
                on the passed in HICH to DPCH channel offset value
                          
DEPENDENCIES    Unknown

RETURN VALUE    delta frames between HICH decode and HICH result application

SIDE EFFECTS    Unknown

===========================================================================*/
extern uint16 e_dl_calc_delta_frames_decode_to_apply_hich (uint16 hich2dpch_offset);
 

/*===========================================================================

FUNCTION        E_DL_CALC_DELTA_FRAMES_DECODE_TO_APPLY_RGCH

DESCRIPTION     This function calculates the number of frames from the frame
                the RGCH is decoded by the UE to the frame where the RGCH 
                grant is applied by the UE for the next UL transmission based
                on the passed in RGCH to DPCH channel offset value and if
                the RGCH channel under consideration is serving or non-serving
                          
DEPENDENCIES    Unknown

RETURN VALUE    delta frames between RGCH decode and RGCH grant application

SIDE EFFECTS    Unknown

===========================================================================*/
extern uint16 e_dl_calc_delta_frames_decode_to_apply_rgch (uint16 rgch2dpch_offset,
                                                    boolean serving_cell);
 

/*===========================================================================

FUNCTION        E_DL_CALC_DELTA_SUB_FRAMES_DECODE_TO_APPLY_AGCH

DESCRIPTION     This function is only for 2ms TTI.This function calculates the
                number of sub frames that needs to be added to the sub frame
                corresponding to the frame N in which the config command was sent
                for a channel activation. N*5+delta_sub_frames.

DEPENDENCIES    Unknown

RETURN VALUE    delta_sub_frames

SIDE EFFECTS    Unknown

===========================================================================*/
extern e_dl_delta_sub_frames_info_struct_type e_dl_calc_delta_sub_frames_decode_to_apply_agch (uint16 agch2dpch_offset);
 
/*===========================================================================

FUNCTION        E_DL_CALC_DELTA_SUB_FRAMES_DECODE_TO_APPLY_HICH

DESCRIPTION    This function is only for 2ms TTI.This function calculates the
                number of sub frames that needs to be added to the sub frame
                corresponding to the frame N in which the config command was sent
                for a channel activation. N*5+delta_sub_frames.

DEPENDENCIES    Unknown

RETURN VALUE    delta_sub_frames

SIDE EFFECTS    Unknown

===========================================================================*/
extern e_dl_delta_sub_frames_info_struct_type e_dl_calc_delta_sub_frames_decode_to_apply_hich (uint16 hich2dpch_offset);
 

/*===========================================================================

FUNCTION        E_DL_CALC_DELTA_SUB_FRAMES_DECODE_TO_APPLY_RGCH

DESCRIPTION     This function calculates the number of frames from the frame
                the RGCH is decoded by the UE to the frame where the RGCH
                grant is applied by the UE for the next UL transmission based
                on the passed in RGCH to DPCH channel offset value and if
                the RGCH channel under consideration is serving or non-serving

DEPENDENCIES    Unknown

RETURN VALUE    delta frames between RGCH decode and RGCH grant application

SIDE EFFECTS    Unknown

===========================================================================*/
extern e_dl_delta_sub_frames_info_struct_type e_dl_calc_delta_sub_frames_decode_to_apply_rgch(uint16 rgch2dpch_offset, boolean serving_cell);
 


/*===========================================================================
 
 FUNCTION        E_DL_CONFIGURE_MODEM_CLKS
 
 DESCRIPTION     
 
 DEPENDENCIES    Unknown
 
 RETURN VALUE    None
 
 SIDE EFFECTS    Call the clocks and modem pwr drivers to vote
              for higher speeds ad HSUPA is being setup
 
 ===========================================================================*/
extern void e_dl_configure_modem_clks(void);

/*===========================================================================
FUNCTION        E_DL_CONFIG_DB_CELL_IDX_UPDATE

DESCRIPTION     This function is used to update the cell index (edl db).
                Cell index is typically populated during CPHY_SETUP_REQ.
                When HSRACH suspend-resume operation happens we need to 
                update again, in case it has changed.
             
DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    Unknown

===========================================================================*/
extern void e_dl_config_db_cell_idx_update(uint8 hs_rach_eul_dl_cell_idx);

#endif /* #ifdef FEATURE_WCDMA_HS_RACH */

#if (!defined(FEATURE_QSBSCS))
/*===========================================================================
FUNCTION        E_UL_SUBMIT_START_RECFG_EVENTS_TO_DM

DESCRIPTION     This function will be called by L1M to commit the EUL events 
                based on the event bitmask for EUL START or RECFG.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void e_ul_submit_start_recfg_events_to_dm(void);

/*===========================================================================
FUNCTION        E_UL_SUBMIT_STOP_EVENTS_TO_DM

DESCRIPTION     This function will be called by L1M to commit the EUL events 
                based on the event bitmask for EUL STOP.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void e_ul_submit_stop_events_to_dm(void);

/*===========================================================================
FUNCTION        CPC_SUBMIT_START_RECFG_EVENTS_TO_DM

DESCRIPTION     This function will be called by L1M to commit the CPC events 
                based on the event bitmask for CPC START or RECFG.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void cpc_submit_start_recfg_events_to_dm(void);

/*===========================================================================
FUNCTION        CPC_SUBMIT_STOP_EVENTS_TO_DM

DESCRIPTION     This function will be called by L1M to commit the CPC events 
                based on the event bitmask for CPC STOP.

DEPENDENCIES    Unknown

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
void cpc_submit_stop_events_to_dm(void);
#endif

/*===========================================================================
FUNCTION        E_DL_DCH_CLEANUP

DESCRIPTION     This function will be called by L1M to cleanup EDL

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern boolean e_dl_dch_cleanup(void);

/*===========================================================================
FUNCTION        E_DL_ASU_DROP_HANDLER

DESCRIPTION     This function will be called by L1M to hanlder ASU RL drop

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None

===========================================================================*/
extern void e_dl_asu_drop_handler(void);


#endif /*end of EUL_H*/

