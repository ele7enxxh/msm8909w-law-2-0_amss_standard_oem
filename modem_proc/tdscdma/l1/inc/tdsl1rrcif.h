#ifndef TDSL1RRCIF_H
#define TDSL1RRCIF_H
/*===========================================================================

             L 1   T A S K   S A P   D E F I N I T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the L1's control service access point (SAP).

  Copyright (c) 2000-2010 by Qualcomm Technologies Incorporated.
  All Rights Reserved.

TABLE OF CONTENTS

    +-Constant Declarations
    | |
    | |__Non-Specification Related Constants
    | |
    | |__Specification Related Constants
    |    |
    |    |__10.3.10  Multiplicity values and type constraint values
    |    |
    |    |__Constants declared directly in IEs
    |
    |
    +-Enumerated Type Declarations For Commands
    | |
    | |__Specification Related Message IEs (Information Elements)
    | |
    | |__Non-Specification Related L1 - RRC Message Information Structures
    |
    |
    +-Structure Type Declarations For Commands
    | |
    | |__Non-Specification Related L1 - RRC Message Information Structures
    | |
    | |__Specification Related Message IEs (Information Elements)
    |
    |
    +-Definition of L1 Commands from RRC
    | |
    | |__CPHY_CAPABILITY_REQ
    | |
    | |__CPHY_IDLE_REQ
    | |
    | |__CPHY_DEACTIVATE_REQ
    | |
    | |__CPHY_STOP_WCDMA_MODE_REQ
    | |
    | |__CPHY_FREQ_SCAN_REQ
    | |
    | |__CPHY_ACQ_REQ
    | |
    | |__CPHY_CELL_PARM_REQ
    | |
    | |__CPHY_IMMEDIATE_CELL_EVAL_REQ
    | |
    | |__CPHY_MEASUREMENT_REQ
    | |
    | |__CPHY_HARD_HANDOFF_REQ
    | |
    | |__CPHY_SET_SFN_REQ
    | |
    | |__CPHY_SET_CFN_REQ
    | |
    | |__CPHY_BCH_SCHEDULE_REQ
    | |
    | |__CPHY_SET_REQ
    | |
    | |__CPHY_ASET_UPDATE_REQ
    | |
    | |__CPHY_CELL_TRANSITION_REQ
    | |
    | |__CPHY_DRX_REQ
    | |
    | |__CPHY_CIPHERING_KEY_REQ
    | |
    | |__CPHY_CELL_SELECTION_REQ
    | |
    | |__CPHY_IMMEDIATE_MEAS_REQ
    | |
    | |__RRC_CPHY_MEAS_CTRL_CNF
    | |
    | |__CPHY_INTEGRITY_KEY_REQ
    | |
    | |__CPHY_INTEGRITY_KEY_MAUC_REQ
    | |
    | |__CPHY_IMMEDIATE_RESELECTION_REQ
    | |
    | |__CPHY_ACT_TIME_CANCEL_REQ
    | |
    | |__CPHY_ACTION_CANCEL_REQ
    |  
    |  
    +-Definition of L1 Indications and Confirmations to RRC
    | |
    | |__CPHY_MEASUREMENT_IND
    | |
    | |__CPHY_CELL_RESELECTION_RSP
    | |
    | |__CPHY_ERROR_IND
    | |
    | |__CPHY_IDLE_IND
    | |
    | |__CPHY_DEACTIVATE_IND
    | |
    | |__CPHY_FREQ_SCAN_CNF
    | |
    | |__L1_ACQ_CNF
    | |
    | |__L1_DRX_CNF
    | |
    | |__L1_SETUP_CNF
    | |
    | |__L1_CELL_SELECT_CNF
    | |
    | |__CPHY_ACT_TIME_CANCEL_CNF
    | |
    | |__L1_CELL_RESELEC_IND
    | |
    | |__L1_IMMEDIATE_CELL_EVAL_CNF
    | |
    | |__L1_SET_SFN_CNF
    | |
    | |__L1_SET_CFN_CNF
    | |
    | |__L1_MEASUREMENT_IND
    | |
    | |__L1_CAPABILITY_CNF
    | |
    | |__L1_INTG_KEY_MAUC_CNF
    | |
    | |__L1_OUT_SYNC_IND
    | |
    | |__L1_IN_SYNC_IND
    | |
    | |__L1_PHYCHAN_EST_IND
    | |
    | |__L1_IMMEDIATE_RESELEC_CNF
    | |
    | |__L1_IDLE_CNF
    | |
    | |__L1_CELL_TRANSITION_CNF
    | |
    | |__RRC_CPHY_COMPRESSED_MODE_IND
    |
    |
    +-RRC Interface Function Prototypes
    | |
    | |__l1_get_rssi()
    | |
    | |__l1_get_interrat_measurement_data()

    
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1rrcif.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/14/14   zl       Provide API for RRC to query barred cell and remaining barring time
07/31/14   zli     Use -2 as default invalid absolute priority value
04/08/14  zli       Set TDSL1_EUTRA_FILTER_COEF_INVALID to TDSL1_EUTRA_FILTER_COEF_MAX
11/22/13   sk      RxLM optimization phase 2 - CR56584 
12/17/12   zy      Change DMO interface: measurement_purpose and timeslot_bitmap, from uint8 to macro.
07/09/12   zl      add the interface changes to support T2L and T2T absolute priority.
02/29/12   owen    Change FEATURE_HSDPA_MAC_EHS to FEATURE_TDSCDMA_HSDPA_MAC_EHS 
02/27/12   hx      Change value of TDSL1_MAX_FREQ from 3 to 9; 
                   TDSL1_MAX_NON_USED_FREQ from 2 to 8. 
06/24/11   ttl     Add max_tx_pwr in tdsl1_setup_cmd_type.
05/06/11   cdf     Remove three redundant parameters:
                   1) allowed in tdsl1_dl_cctrchcfg_tf_info_struct_type
                   2) tti in tdsl1_ul_semi_static_parms_struct_type
                   3) num_trch in tdsl1_ul_cctrch_cfg_struct_type
02/16/11   cdf     Move structures from tdsl1ulcfg.h to here.
01/14/11   cxd     Remove the following L1 external commands
                   TDSL1_CPHY_CAPABILITY_REQ           =   0x0,
                   TDSL1_CPHY_ASET_UPDATE_REQ          =   0xA,
                   TDSL1_CPHY_HARD_HANDOFF_REQ         =   0xB,
                   TDSL1_CPHY_BCH_SCHEDULE_REQ         =   0xC,
                   TDSL1_CPHY_INTEGRITY_KEY_REQ        =   0x13,
                   TDSL1_CPHY_INTEGRITY_KEY_MAUC_REQ   =   0x14,
                   TDSL1_CPHY_IMMEDIATE_RESELECTION_REQ=   0x15,
                   TDSL1_CPHY_CELL_RESELECTION_RSP     =   0x16,
12/02/10   cyx     Remove tdsmvsif.h to clean up l2 interface
11/30/10   zwj/ttl Modify IEs for RRC_CPHY_MEASUREMENT_IND and CPHY_MEASUREMENT_REQ.
11/30/10   ysh/ttl Add Uplink Physical Channel Control. 3Gpp-8.2.10(3.2.6)
11/29/10   sq      Initial draft
                                     
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "queue.h"
#include "customer.h"
#include "msg.h"
#include "tdsl1const.h"

#include "tdsl1sapcommon.h"
#include "sys_plmn_selection.h"
#include "tdsmacl1rrcif.h"

#include "tdsrrccmd.h"

/*#include "tdsrrc_rr_types.h"*/
#include "tdsrrc_rr_if.h"
#include "rr_tdsrrc_if.h"

#include "tdsuecomdef.h"
//#include "tdsl1dlphychancfg.h"
//#include "tdsdldeccommon.h"
#include "nv_items.h"
#include "mcfg_fs.h"
/*yongh*/
//#include "tdsl1dlcctrchcfg.h"

#include "trm.h"


/*-------------------------------------------------------------------------*/
/* L1 - RRC   Interface Constant Definitions                               */
/*-------------------------------------------------------------------------*/
/* Max cells reported per EUTRA frequency */
#define TDSL1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY 4
/* Define reselection permission bitmask for TL1 to determine whether to trigger
 * certain type of reselection;  
 * bit 0 -- can trigger certain reselection
 * bit 1 -- can not trigger certain reselection
 */
#define TDS_INTRA_FREQ_RESEL_BIT   0X01
#define TDS_INTER_FREQ_RESEL_BIT   0X02
#define TDS_INTER_RAT_GSM_RESEL_BIT  0X04
#define TDS_INTER_RAT_LTE_RESEL_BIT  0X08
/* Define max window size of page pattern detect*/
#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
#define TDS_PAGE_PATTERN_DETECT_MAX_WINDOW_SIZE 8 
#endif
#define tdsrrc_l1_cmd_e_type tdsrrc_cmd_e_type

/*For SRB5 support*/
typedef tdsl1_dl_cctrchcfg_cctrch_info_struct_type tdsl1_dl_cctrch_info_struct_type; 

/*==========================================================================
**
**      DL&UL SHARE STRUCTURE    
**
**==========================================================================*/

typedef struct {
  /*Midamble allocation mode. (10.3.6.41)*/
  tdsl1_midamble_alloc_mode_enum_type allocation_mode;
  
  /*Midamble configuration, as defined in TS 25.221.*/
  tdsl1_midamble_config_enum_type config;
  
  /*Integer (0..15).*/
  uint8 shift;
}tdsl1_midamble_ss_burst_struct_type;   

/* zhouya add for 10.3.6.88 begin(merge UL & DL) */
typedef struct {
  /*Frame timeslot related interleaving.  Default value is "Frame".  (10.3.6.10)*/
  tdsl1_sec_intlv_mode_enum_type  second_interleaving_mode;

  /*Describes the amount of bits for the TFCI bits code word as described in Ts 25.222. Integer(4,8,16,32). Defaults is no TFCI bit.*/
  uint8 tfci_coding; 

  /*Puncturing limit (0.40..1.0 by step of 0.04) * 100. (10.3.6.10)*/
  uint8 punct_limit;

  /*Integer(1, 2,4,8,16,32,64). Default is continuous allocation. Value 1 indicate continuous. (10.3.6.10)*/
  uint8 repetition_period;

  /*Integer(1.. Repetition period -1 ). NOTE: This is empty if repetition period is set to 1.  (10.3.6.10)*/
  uint8 repetition_length;
}tdsl1_common_ts_info_struct_type;


/*==========================================================================
**
**      DL CONFIGURATION DATA STRUCTURE    
**
**==========================================================================*/
/***************************************************************************
**                                                                       
**     Downlink physical channel configuration parameters                   
**                                                                       
***************************************************************************/
typedef enum 
{
  TDSL1_DL_PHYCFG_TIMING_IND_INIT,
  TDSL1_DL_PHYCFG_TIMING_IND_MAINTAIN,
  TDSL1_DL_PHYCFG_TIMING_IND_RELAY_HANDOVER,
  TDSL1_DL_PHYCFG_TIMING_IND_DPCH_ESTABLISH,
  TDSL1_DL_PHYCFG_TIMING_IND_DPCH_MODIFY,
  TDSL1_DL_PHYCFG_TIMING_IND_HHO_FAILURE,
  TDSL1_DL_PHYCFG_NUM_TIMING_IND
} tdsl1_dl_phycfg_timing_ind_enum_type;

typedef enum 
{
  TDSL1_DL_PHYCFG_DL_TDM_MODE_DISABLE,
  TDSL1_DL_PHYCFG_DL_TDM_MODE_ENABLE,
  TDSL1_DL_PHYCFG_INVALID_DL_TDM_MODE
}tdsl1_dl_phycfg_dl_tdm_mode_enum_type; 

typedef enum 
{
  TDSL1_DL_PHYCFG_PCCPCH_KNOWN_TTI_BOUNDARY,
  TDSL1_DL_PHYCFG_PCCPCH_PHASE_BASED_DETECTION,
  TDSL1_DL_PHYCFG_PCCPCH_BLIND_TTI_DETECTION,
  TDSL1_DL_PHYCFG_INVALID_PCCPCH_TTI_DETECTION
}tdsl1_dl_phycfg_pccpch_tti_detection_enum_type;

/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_pccpch_parms_struct_type
This contains the configuration information of pccpch channel.
--------------------------------------------------------------------------*/
typedef struct 
{

/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
  /* scrambling code */
  uint8 cpid;

  boolean tstd_indicator;

  /* Pointer to an array of 512 bytes (or 4096 bits) to handle all 4096 SFN's.
  ** On N_BCH setup, this bit array identifies SFN's where RRC wants to receive
  ** SIB data.  A 1 bit tells WL1 to receive that frame.  A 0 means WL1 may skip
  ** it.  WL1 can use this info to sleep between SIB frames and conserve power. 
  */
  uint8  *sib_sfn_bit_mask;
} tdsl1_dl_phycfg_pccpch_parms_struct_type;


/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_common_ts_info_struct_type
This contains the configuration information of individual ts info.
--------------------------------------------------------------------------*/ 
typedef struct 
{
  /*Frame timeslot related interleaving.  Default value is "Frame".  (10.3.6.10)*/
  tdsl1_sec_intlv_mode_enum_type second_interleaving_mode;
  
  /*Describes the amount of bits for the TFCI bits code word 
  **as described in Ts 25.222. Integer(4,8,16,32). 
  **Defaults is no TFCI bit.
  */
  uint8 tfci_coding; 
  
  /*Puncturing limit (0.40..1.0 by step of 0.04) * 100. (10.3.6.10)*/
  uint8 punct_limit;
  
  /*Integer(1, 2,4,8,16,32,64). 
  **Default is continuous allocation. 
  **Value 1 indicate continuous. (10.3.6.10)
  */
  uint8 repetition_period;
  
  /*Integer(1.. Repetition period -1 ). (10.3.6.10)
  **NOTE:This is empty if repetition period is set to 1.  
  */
  uint8 repetition_length;
}tdsl1_dl_phycfg_common_ts_info_struct_type;

                                                                  
/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_individual_ts_info_struct_type
This contains the configuration information of individual ts info.
--------------------------------------------------------------------------*/                                                                                                             
typedef struct 
{
  /*Timeslot number.  Integer (0,2,3,4,5,6)*/
  uint8 ts_num;  
  
  /* set to TRUE if TFCI is used */
  boolean tfci_exists;
  
  /*Midamble shift and burst type (10.3.6.41)*/
  tdsl1_midamble_ss_burst_struct_type midamble_ss_burst_type;
  
  /*Denotes amount of SS and TPC bits send in this timeslot. (10.3.6.37)*/
  tdsl1_ss_tpc_symb_enum_type ss_tpc_symbols;
  
  /*Integer(1..15).Specifies the number of additional codes 
  **in this timeslot that carry TPC and SS symbols 
  **as specified in TS 25.224.(10.3.6.37)
  */
  uint8 additional_ss_tpc_symbols;
  
  /*1 in bit i (i=0,15) means SF16 Code #i is used in current timeslot.  
  **All zeros in this bitmap means using SF=1 code in current timeslot. (10.3.6.71)
  */
  uint16 chan_code_bitmap;
}tdsl1_dl_phycfg_individual_ts_info_struct_type;



/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_sccpch_parms_struct_type
This contains the configuration information of sccpch channel.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Number of frame offset.  Integer (0...Repetition Period -1).  
  ** SFN modulo Repetition period = offset. 
  ** Repetition period is the one indicated 
  **in the accompanying Common timeslot info IE.  (10.3.6.71)  
  */
  uint8 offset;
  
  /*Common timeslot info (10.3.6.71) */
  tdsl1_dl_phycfg_common_ts_info_struct_type common_ts_info;
  
  /* Individual timeslot info (10.3.6.71) */
  tdsl1_dl_phycfg_individual_ts_info_struct_type individual_ts_info;
} tdsl1_dl_phycfg_sccpch_parms_struct_type;


/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_pich_parms_struct_type
This contains the configuration information of pich channel.
--------------------------------------------------------------------------*/
typedef struct 
{
   /* Timeslot number.  Default value is 0. */
  uint8 ts_num;

  /* Midamble shift and burst type ( 10.3.6.41) */
  tdsl1_midamble_ss_burst_struct_type midamble_and_burst;

  /* Channelisation code bitmap.  (10.3.6.49) */
  uint16 ch_code_bitmap;
  
  /* Repetition period/length (Default value is "64/2".   (10.3.6.49)
  ** Here Repetition period represents the value of Paging Block Periodicity(PBP) 
  ** and Repetition length represents the value of Npich.
  */
  tdsl1_pich_repeat_prd_len_enum_type repetition_period_length;
  
  /*Integer (0...Repetition period -1).SFN mod Repetition period = Offset.*/
  uint8 offset;
  
  /*Length of one paging indicator in Bits. Default value is 4.  (10.3.6.49)*/
  tdsl1_dl_pi_length_enum_type paging_indicator_length;
  
  /* Number of frames between the last frame carrying PICH for this Paging Occasion 
  ** and the first frame carrying paging messages for this Paging Occasion. 
  ** Default value is 4. (10.3.6.49)
  */
  tdsl1_dl_pich_num_gap_enum_type num_gap;
  
  /*Number of PCH paging groups (Npch). Integer(1 .. 8). 
  **Default value is 2.  (10.3.6.49)
  */
  uint8 num_paging_group;
  
  /* IMSI divided by 8192, as per 8.3 in 25.304 for R'99, used in
  ** calculating PO. Note: This parameter is new in the transition to R'99 
  */
  uint64 imsi_div_8192;

  /* IMSI divided by K, as per 8.3 in 25.304, used in calculating PI,
  ** where K = number of SCCPCH that have a PCH mapped. 
  */
  uint64 imsi_div_pch_count;
  /* 10.3.6.62 -> 10.3.6.42 PICH power offset in EcIo relative
  ** to Primary CPICH 
  */
  int8 pwr_offset;
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/

} tdsl1_dl_phycfg_pich_parms_struct_type;



/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_dpch_common_info_struct_type
This contains the common configuration information of dpch channel.
--------------------------------------------------------------------------*/
#define TDSL1_DL_PHYCFG_DPCH_COMMON_INFO_TPC_STEP_SIZE_PRESENT 0x01

typedef struct 
{
  /*TSTD indicator. (10.3.6.85a)*/
  boolean tstd_indicator;

  /* timing indication, init or maintain */
  tdsl1_dl_phycfg_timing_ind_enum_type timing_ind;

  /* optional parameter absence/presence bitmask.  
  ** Use the above #defines 
  */
  uint8 opt_parm_present_mask;

  /* Downlink DPCH power control info(10.3.6.23)  */
  tdsl1_tpc_step_size_enum_type tpc_step_size;
#if 0  
/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
  /* CFN <--> targetSFN frame offset */
  uint8 targ_frame_offset;

  /* Downlink DPCH power control info */
  tdsl1dlphycfg_dpc_mode_enum_type dpc_info;

  /* Downlink power offset between DPCCH pilot and DPDCH */
  uint8 po3;

  /* spreading factor */
  tdsl1_sf_enum_type sf;

  /* set to TRUE if TFCI is used */
  boolean tfci_exists;

  /* The number of pilot bits used in a slot */
  uint8 num_pilot_bits;

  /* Default value is FALSE */
  boolean  skip_sync_a_procedure;

/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/
#endif
} tdsl1_dl_phycfg_dpch_common_info_struct_type;

#define TDSL1_DL_DPCH_COMMON_INFO_TPC_STEP_SIZE_PRESENT 0x01


/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_time_info_struct_type
This contains the common configuration information of dpch channel.
--------------------------------------------------------------------------*/
typedef struct 
{
  /*CFN number start of the physical channel existence.
  **Integer(0..255). Default value is "Now".(10.3.3.31)
  */
  uint16 activation_time;
}tdsl1_dl_phycfg_time_info_struct_type;


/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_phycfg_cctrch_list_struct_type
This contains the cctrch information of dpch configuration parameter.
--------------------------------------------------------------------------*/
typedef struct 
{
  /*Identity of this CCTrCh.Integer(1..8).
  **Default value is 1.  (10.3.6.21)
  */
  uint8 tfcs_id;
  
  /*(10.3.6.83)*/
  tdsl1_dl_phycfg_time_info_struct_type time_info;

  /* zhouya modify for (10.3.6.88 & 10.3.6.21) begin (merge UL and DL) */
  /*Common timeslot info (10.3.6.20).*/
  tdsl1_common_ts_info_struct_type common_ts_info;
  /* zhouya modify for (10.3.6.88 & 10.3.6.21) end */

  /*Timeslot bitmap.  
  **From LSB, Bit 0 for TS#0, 
  **Bit 1 for TS#1, Bit 6 for TS#6.
  */
  uint8  num_ts;
  
  /*Individual timeslot info (10.3.6.71)*/
  tdsl1_dl_phycfg_individual_ts_info_struct_type individual_ts_info[TDSL1_DL_MAX_NUM_TS];
}tdsl1_dl_phycfg_cctrch_list_struct_type;



/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_phycfg_dpch_parms_struct_type
This contains the information of dpch configuration parameter.
--------------------------------------------------------------------------*/
//#define TDSL1_DL_PHYCFG_CCTRCH_LIST_PRESENT 0x0001
//#define TDSL1_DL_PHYCFG_DPCH_COMMON_INFO_PRESENT 0x0002
#define TDSL1_DL_PHYCFG_DEFAULT_DPCH_OFFSET_PRESENT 0x0001
typedef struct 
{
   /*DL physical channels to establish or reconfigure list.  (10.3.6.21)*/
  tdsl1_dl_phycfg_cctrch_list_struct_type dl_cctrch_list;

  /* Downlink DPCH info common for all radio links (10.3.6.18) */
  tdsl1_dl_phycfg_dpch_common_info_struct_type dpch_common;

  /* Optional parameter presence indication bitmask */
  uint16 opt_parm_presence_bitmask;

  /* Default DPCH offset value (DOFF).Integer(0..7) in number of frames.
  ** Indicates the default offset value within interleaving size 
  ** at a resolution of one frame to offset CFN in the UE.(10.3.6.16)
  */
  uint8 doff;
} tdsl1_dl_phycfg_dpch_parms_struct_type;


/*--------------------------------------------------------------------------
                   structure : tdsl1_dl_phycfg_dpch_sync_parms_struct_type
This contains the information dpch channel sync.
--------------------------------------------------------------------------*/
typedef struct 
{
  /* the N312 counter used for physical channel established detection */
  uint16 N312;

  /* the N313 counter used for out of sync detection */
  uint16 N313;

  /* the N315 counter used for in sync detection */
  uint16 N315;

  /* Timer count for physical chanel establishment in seconds */
  uint8 T312;

  /* Timer count for RL failure declaration in seconds */
  uint8 T313;
} tdsl1_dl_phycfg_dpch_sync_parms_struct_type;

#if 0
/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_phycfg_phychan_db_struct_type
This contains the information making up a page of the physical channel
configuration database
--------------------------------------------------------------------------*/
/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
typedef struct {
  /* Optional parameter presence indication bitmask */
  uint16 opt_parm_presence_bitmask;

  /* 10.3.6.20 -> 10.3.6.14 The downlink DPCH info common for all
     Radio Links */
  tdsl1_dl_phycfg_dpch_common_info_struct_type dpch_common;

  /* 10.3.6.20 -> 10.3.6.13 the default DPCH offset value */
  uint32 default_dpch_offset;
  
  /* 10.3.6.20 -> 10.3.6.74 The TX diversity mode */
  tdsl1dlphycfg_tx_div_mode_enum_type tx_div_mode;

  /* Default value is FALSE */
  boolean    post_verification_valid;

} tdsl1_dl_phycfg_common_info_struct_type;

/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/
#endif
typedef struct 
{
  /* the PCCPCH parameters */
  tdsl1_dl_phycfg_pccpch_parms_struct_type pccpch;

  /* array of TDSL1_DL_MAX_SCCPCH SCCPCH parameters */
  tdsl1_dl_phycfg_sccpch_parms_struct_type sccpch[TDSL1_DL_MAX_SCCPCH];

  /* PICH setup flag, the PICH will be added upon adding or 
  ** configuringan SCCPCH if this flag is set to TRUE 
  */
  boolean use_pich;

  /* the PICH parameters */
  tdsl1_dl_phycfg_pich_parms_struct_type pich;

  /* the DPCH parameters */
  tdsl1_dl_phycfg_dpch_parms_struct_type dpch;
  
  /* parameters for DPCCH sync detection and reporting */
  tdsl1_dl_phycfg_dpch_sync_parms_struct_type sync_det;

  /* the DPCH parameters common to all radio links */
  //tdsl1_dl_phycfg_common_info_struct_type dl_common;

  /* the number of individual radio links */
  uint16 num_rl;
/* zhouya del for 10.3.6.21 begin */

  /* array of DPCH info structures for parameters 
  ** unique to each radio link 
  */
  tdsl1dl_info_per_rl_struct_type dl_per_rl[TDSL1_MAX_RL];

/* zhouya del for 10.3.6.21 end */
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/

} tdsl1_dl_phycfg_phychan_db_struct_type;

/*+ Legacy for TDS UL/DL dev, to be deleted later ******************************/
typedef tdsl1_dl_phycfg_phychan_db_struct_type tdsl1dl_phychan_db_struct_type;
/*- Legacy for TDS UL/DL dev, to be deleted later ******************************/

/***************************************************************************
**                                                                       
**     Downlink cctrch configuration parameters                   
**                                                                       
***************************************************************************/
#if 0
/*tdsl1_dl_cctrchcfg_coding_enum_type*/
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_coding_enum_type  tdsl1_dl_cctrchcfg_coding_enum_type;
#else
typedef enum
{
  TDSL1_DL_CONV_1_BY_2_CODING, 
  TDSL1_DL_CONV_1_BY_3_CODING, 
  TDSL1_DL_TURBO_CODING,      
  TDSL1_DL_UNCODED             
} tdsl1_dl_cctrchcfg_coding_enum_type;
#endif
#endif

#if 0
/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_cctrchcfg_tf_info_struct_type
This contains the information making up tf info db for cctrch configuration
--------------------------------------------------------------------------*/
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_tf_info_struct_type  tdsl1_dl_cctrchcfg_tf_info_struct_type;
#else
typedef struct
{
  /* Number of transport block in TBS. */
  uint8  num_tb; 

  /* Transport block size. */
  uint16 tb_size; 
} tdsl1_dl_cctrchcfg_tf_info_struct_type;
#endif
#endif

/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_cctrchcfg_trch_info_struct_type
This contains the information making up trch info db for cctrch configuration
--------------------------------------------------------------------------*/

#define TDSL1_DL_MAX_TF_PER_TRCH 64

#if 0
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_trch_info_struct_type  tdsl1_dl_cctrchcfg_trch_info_struct_type;
#else
typedef struct
{
  /* Transport channel reference type */
  tdsl1_dl_trch_ref_type_enum_type ref_type;

  /* Transport channel Id. */
  uint8                         trch_id;

  /* Rate matching attribute. */
  uint16                        rate_match_attrib;

  /* Coding type (enumerated). */
  tdsl1_dl_cctrchcfg_coding_enum_type        coding_type;

  /* CRC length (enum or number of bits). */
  tdsl1_crc_length_enum_type       crc_length;

  /* TTI duration (enum or milliseconds). */
  tdsl1_tti_enum_type              tti_value;

  /* DCH BLER Quality Value.
  ** If not valid or not available, set it to -127.
  ** Valid range -63 to 0 
  */
  int8                          bler_quality_val;

#ifdef FEATURE_TEST_LOOPBACK
  /* Loop back mode for this TrCh. 
  ** Only Applicable if TrCh ref type is DCH 
  */
  tdsl1_loop_back_mode_type_enum_type loopback_mode;

  /* If set to TRUE, this flag indicates that this TrCh is mapped to
  ** Non-TM bearer.  Else if FALSE it is mapped to TM bearers.
  ** This flag is only applicable for TDSL1_LOOPBACK_MODE_1 
  */
  boolean                       non_tm_rb;
#endif /* FEATURE_TEST_LOOPBACK */

  /* Number of transport format. */
  uint8                         num_tf;

  /* Pointer to the array of transport format. */
  tdsl1_dl_cctrchcfg_tf_info_struct_type     *tf_info_array[TDSL1_DL_MAX_TF_PER_TRCH];
} tdsl1_dl_cctrchcfg_trch_info_struct_type;
#endif
#endif

/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_cctrchcfg_cctrch_info_struct_type
This contains the information making up cctrch info db for cctrch configuration
--------------------------------------------------------------------------*/
#if 0
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_cctrch_info_struct_type  tdsl1_dl_cctrchcfg_cctrch_info_struct_type;
#else
typedef struct
{
  /* CCTrCh Id for this CcTrCh information */
  uint8                       cctrch_id;

  /* TrCh ref type bit field indicating the TrCh ref type 
  ** presentin this CCTrCh 
  */
  uint8                       trch_ref_type_bf;

  /* TrCh ref type enable bit field indicating the TrCh ref type
  ** data will be passed up to MAC. If the bit for the TrCh is 0
  ** TB is dropped after it is received. 
  */
  uint8                       trch_enable_bf;

  /* Number of transport channel. */
  uint8                       num_trch;

  /* Pointer to the TrCh info array. */
  tdsl1_dl_cctrchcfg_trch_info_struct_type *trch_info_array[TDSL1_DL_MAX_TRCH];
 
} tdsl1_dl_cctrchcfg_cctrch_info_struct_type;
#endif
#endif

/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_cctrchcfg_ctfc_entry_info_struct_type
This contains the information making up ctfc entry db for cctrch configuration
--------------------------------------------------------------------------*/
#if 0
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_ctfc_entry_info_struct_type  tdsl1dlcctrchcfg_dl_ctfc_info_struct_type;
#else
typedef struct
{
  /* Is this CTFC is valid or not. */
  boolean            valid_ctfc;
  
  /** pointer to transport format index of each TrCh in CCTrCh array */ 
  uint8              *tfi_table;
} tdsl1dlcctrchcfg_dl_ctfc_info_struct_type;
#endif
#endif

#if 0
/*--------------------------------------------------------------------------
                   structure: tdsl1_dl_cctrchcfg_dl_ctfc_info_struct_type
This contains the information making up ctfc info db for cctrch configuration
--------------------------------------------------------------------------*/
#ifdef FEATURE_MCAL_TDSCDMA
typedef mcaltds_dec_ctfc_info_struct_type  tdsl1_dl_cctrchcfg_ctfc_info_struct_type;
#else
typedef struct
{
  /*Record the number of ctfc entry.*/
  uint16 num_ctfc_entry;

  /* Pointer to the array of TFC or CTFC entries. */
  tdsl1_dl_cctrchcfg_ctfc_entry_info_struct_type *ctfc_entry_array[TDSL1_DL_MAX_TFC];
} tdsl1_dl_cctrchcfg_ctfc_info_struct_type;
#endif
#endif


/* Maximum number of HARQ processes */
#ifdef FEATURE_TDSCDMA_MIMO
#define TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC 16
#define TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC_NO_MIMO 8
#else
#define TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC 8
#endif

/* Number of MAC HS queues */
#define TDSL1_DL_CCTRCHCFG_MAC_HS_Q_MAX_COUNT    8

/* maximum MAC-d PDU size */
#define TDSL1_DL_CCTRCHCFG_MACD_MAX_PDU_SZ 5000

/* 25.331 Rel-7 Spec - Maximum number of logical channels */
#define TDSL1_DL_CCTRCHCFG_MAX_LOGICAL_CHANNEL 16
#define TDSL1_DL_CCTRCHCFG_NON_DTX_MODE_LOGICAL_CHANNEL 15

/* This emum defines the expicit memory partitions 
 * among different HARQ processes 
 */
typedef enum
{
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS800, /* Start 800..16000 in step of 800 (enum value 0) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS1600,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS2400,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS3200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS4000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS4800,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS5600,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS6400,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS7200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS8000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS8800,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS9600,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS10400,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS11200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS12000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS12800,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS13600,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS14400,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS15200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS16000, /* End     800..16000 in step of 800  (enum index 18) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS17600, /* Start 17600..32000 in step of 1600 (enum index 19) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS19200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS20800,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS22400,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS24000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS25600,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS27200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS28800,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS30400,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS32000, /* End   17600..32000 in step of 1600 (enum index 28) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS36000, /* Start 36000..80000 in step of 4000 (enum index 29) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS40000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS43200,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS44000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS48000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS52000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS56000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS60000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS64000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS68000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS72000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS76000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS80000, /* End   36000..80000  in step of 4000 (enum index 40) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS88000, /* Start 88000..160000 in step of 8000 (enum index 41) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS96000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS104000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS112000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS120000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS128000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS136000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS144000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS152000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS160000, /* End    88000..160000 in step of 8000  (enum index 50) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS176000, /* Start 176000..304000 in step of 16000 (enum index 51) */
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS192000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS208000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS224000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS240000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS256000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS272000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS288000,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS304000  /* End 176000..304000 in step of 16000 (enum index 59) */
} tdsl1_dl_hs_dsch_harq_mem_sz_enum_type;

#define TDSL1DLCCTRCHCFG_HARQ_SZ_ENUM_TO_VAL(sz_enum) \
  ((sz_enum == TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS43200) ? (43200) : \
   ((sz_enum) <= TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS16000)  ? (800    + ((int32) (800   * ((int32)(sz_enum) - (int32)TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS800   )))) : \
   ((sz_enum) <= TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS32000)  ? (17600  + ((int32) (1600  * ((int32)(sz_enum) - (int32)TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS17600 )))) : \
   ((sz_enum) <= TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS80000)  ? (36000  + ((int32) (4000  * ((int32)(sz_enum) - (int32)TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS36000 )))) : \
   ((sz_enum) <= TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS160000) ? (88000  + ((int32) (8000  * ((int32)(sz_enum) - (int32)TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS88000 )))) : \
   ((sz_enum) <= TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS304000) ? (176000 + ((int32) (16000 * ((int32)(sz_enum) - (int32)TDSL1_DL_CCTRCHCFG_HS_DSCH_HMS176000)))) : \
   304000)

/* HS-DSCH TB-size Table */
typedef enum
{
  TDSL1_DL_CCTRCHCFG_HS_DSCH_TB_TABLE_OCTET_UNALIGNED,
  TDSL1_DL_CCTRCHCFG_HS_DSCH_TB_TABLE_OCTET_ALIGNED
} tdsl1_dl_cctrchcfg_hs_dsch_tb_size_alignment_enum_type;

/* This structure defines HARQ process memort partition information */
typedef struct
{
  /* Number of HARQ processes 
   * With MIMO, the possible values can be 1..8 or 12 or 14 or 16 
   * Without MIMO, the possible values can be 1..8 
   */
  uint8 num_proc;
  
  /* indicate if memory partitioning is specified explicit by N/W */
  boolean mem_part_info_valid;
  
  /* HARQ process mem partition information
   * Valid only if mem_part_info_valid is valid 
   */
  tdsl1_dl_hs_dsch_harq_mem_sz_enum_type
    mem_size_type[TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC];

 #if defined(FEATURE_TDSCDMA_MIMO)
  /* Indicate if additional memory partitioning 
   * is specified explicitly by N/W for MIMO 
   */
  boolean mimo_additional_mem_part_info_valid;
 
  /* HARQ process additional mem partition information
   * Valid only if mimo_additional_mem_part_info_valid is valid 
   */
  tdsl1_dl_hs_dsch_harq_mem_sz_enum_type
    mimo_additional_mem_size_type[TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC];
 #endif
} tdsl1_dl_cctrchcfg_hs_dsch_harq_info_struct_type;

/* This structure defines MAC-d PDU size information for given MAC HS queue */
typedef struct
{
  /* PDU size index in MAC HS header */
  uint8 pdu_sz_idx;
  
  /* PDU size */
  uint16 pdu_sz;
} tdsl1_dl_cctrchcfg_hs_dsch_mac_d_pdu_sz_info_struct_type;

/* This structure defines single MAC HS queue information */
typedef struct
{
  /* MAC HS queue Id */
  uint8 mac_hs_q_id;
  
  /* number of MAC-d PDU sizes for this queue */
  uint8 num_mac_d_pdu_sz;
  
  /* MAC PDU size type information for this queue */
  tdsl1_dl_cctrchcfg_hs_dsch_mac_d_pdu_sz_info_struct_type
    mac_d_pdu_sz_info[TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT];
} tdsl1_dl_cctrchcfg_hs_dsch_mac_hs_q_info_struct_type;

/* This structure defines MAC HS queue information */
typedef struct
{
  /* Number of MAC HS queues */
  uint8 num_mac_hs_q;
  
  /* Each MAC HS queue information */
  tdsl1_dl_cctrchcfg_hs_dsch_mac_hs_q_info_struct_type
    mac_hs_q_info[TDSL1_DL_CCTRCHCFG_MAC_HS_Q_MAX_COUNT];
} tdsl1_dl_cctrchcfg_hs_dsch_mac_d_flow_info_struct_type;

/* This structure defines the HS DSCH TrCh information */
typedef struct
{
  /* CTrCh Id for HS DSCH TrCh. 
   * The possible value can be 6 and 7 
   */
  uint8 cctrch_id;
  
  /* H-RNTI, this is 16 bits in length */
  uint16 h_rnti;
  
  /* HARQ information */
  tdsl1_dl_cctrchcfg_hs_dsch_harq_info_struct_type harq_info;

 #ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /*Informs MAC if MAC HS is configured or MAC EHS */
  mac_hs_e_type mac_hs_or_ehs;

  /* Indicates if the HS-DSCh TB table to be chosen 
   * is octet aligned or not
   */
  tdsl1_dl_cctrchcfg_hs_dsch_tb_size_alignment_enum_type  hsdsch_tb_size_alignment_type;

  /* If config is EHS, then only interpret the following EHS info.
   * Otherwise, HS info is valid for this cmd 
   */

  /* This field contains the queue id for each logical channel mapped 
   * to MAC-Ehs. If the logical channel is not in use, then the corresponding
   * queue mapping should be set by RRC to TDSINVALID_QUEUE_ID(value 8). 
   */
  uint8 mac_ehs_dlchan_q_info[TDSL1_DL_CCTRCHCFG_MAX_LOGICAL_CHANNEL];
 #endif
 
 #ifdef FEATURE_TDSCDMA_64QAM
  boolean hs_64_qam_configured;
 #endif

  /* MAC-d information. This is used for deciding mac HS header only */
  tdsl1_dl_cctrchcfg_hs_dsch_mac_d_flow_info_struct_type macd_info;
} tdsl1_dl_cctrchcfg_hs_dsch_info_struct_type;


#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
typedef enum 
{
   PATTERN_DETECT_NONE,
   PATTERN_DETECT_START,
   PATTERN_DETECT_CONTINUE,
   PATTERN_DETECT_STOP
} tdsrrc_pg_pattern_detect_operation_enum;
#endif
/*===========================================================================

FUNCTION     tdsl1m_is_connected

DESCRIPTION
  This function is called to check whether the UE is in a call (tdsl1m_global.l1_state is TDSL1M_DCH)

DEPENDENCIES
  None

RETURN VALUE
  TRUE if tdsl1m_global.l1_state is TDSL1M_DCH and FALSE otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tdsl1m_is_connected(void);


/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*==========================================================================
**
**      CONSTANT DECLARATIONS   
**
**==========================================================================*/
/***************************************************************************
**
**     L1 - RRC   Non-Specification Related Information                                                               
**
***************************************************************************/

/*-------------------------------------------------------------------------
**       L1 - RRC   Interface Constant Definitions                              
**-------------------------------------------------------------------------*/

/* Maximum number of cells that can be in the active set */
#define TDSL1_ASET_MAX   6

/* Maximum number of RF channels to acquire on at one time */
#define TDSL1_MAX_ACQ_CHANNEL_NUM   32

/*Maximum number of MSET list*/
#define TDSL1_MAX_MSET_LIST_NUM	    15

/* Maximum number of JDS cells L1SRCH return to RRC */
#define TDSL1_MAX_JDS_CELL_NUM      4

/* Maximum number of cells that can currently be triaged from neighbour set
** Actually we need only one for now but keep two 
*/
#define TDSL1_NSET_MAX   2 

#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
/* MACROs needed for L1_FREQ_SCAN_CNF declaration 
** In UOOS raw scan bin size increased to 3.2MHz from 2MHz
*/
#define TDSL1_MAX_RAW_SCAN_FREQ   21
#else 
/* MACROs needed for L1_FREQ_SCAN_CNF declaration */
#define TDSL1_MAX_RAW_SCAN_FREQ   32
#endif

/* Maximum of the center frequencies scanned through a RAW scan. 
** The value "32" is defined for 30MHz frequency band and 2MHz RAW
** scan incremental step. "31" should be enough, but "32" is more
** conservative.
*/  
#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
#define TDSL1_MAX_FINE_SCAN_FREQ  32
#else
/* Fine Scan within 2MHz */
#define TDSL1_MAX_FINE_SCAN_FREQ  10
#endif


#ifdef FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
/* Maximum of the center frequencies scanned through a FINE scan. 
** The value "11" is defined for 2MHz bandwith and 200KHz FINE scan
** ncremental step. RRC signal when tune is done
*/
#define TDSL1_RRC_CSP_UOOS_LFS_SIG 0x8000
#endif

/* Maximum of the center frequencies scanned through any type of scan. */
#define TDSL1_MAX_SCAN_FREQ  MAX(TDSL1_MAX_RAW_SCAN_FREQ, TDSL1_MAX_FINE_SCAN_FREQ)

/* djm - should this be maxCNdomains ? */
#define TDSL1_MAX_NUM_KEYS        10

/* Cell Info List Structure for Remove All Command Value for use in num_cell_rmv parameter.*/
#define TDSL1_REMOVE_ALL_CELLS    0xFF

/* 10.3.7.64 - Time to Trigger */
#define TDSL1_TTT_MAX             5000

#define TDSL1_TTT_NOT_SET         (TDSL1_TTT_MAX+5)
  
#define TDSL1_MAX_TRANSPORT_CHANNELS  32

/* this is used by RRC to indicate L1 that no confirmation is needed for the MCM sent */
#define TDSL1_NO_CNF_TRANSACTION_ID  100

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
/* for NMR upto 6 cells need to be sent */
#define TDSL1_MAX_CELLS_FOR_NMR 6
#endif

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
/* Default priority set by L1 when priority is not provided from SIB 19 */
#define TDSL1_LOWEST_PRIORITY 0XFF
/* Max number of inter frequency layers that can be sent in by 3rd_MEAS_REQ */
#define TDSL1_MAX_INTERF_PRIORITY_LAYERS 8
/* Max number of GSM layers that can be sent in by 3rd_MEAS_REQ */
#define TDSL1_MAX_GSM_PRIORITY_LAYERS 32
#ifdef FEATURE_TDSCDMA_SUPPORT_LTE_DEPRIORITIZATION
#define TDSL1_DEPRIORITIZED_PRIORITY (-1) 
#endif
#endif

#ifdef FEATURE_TDSCDMA_TO_LTE 
/* For EUTRA frequency up to 8 frequencies  */
#define TDSL1_MAX_EUTRA_FREQ 8
/* Max number of LTE layers that can be sent in by 3rd_MEAS_REQ */
#define TDSL1_MAX_EUTRA_PRIORITY_LAYERS 8
/* Max cells supported by EUTRA frequency */
#define TDSL1_MAX_EUTRA_CELLS_PER_FREQUENCY 16
#define TDSL1_REMOVE_ALL_EUTRA_FREQ    0xFF
/* MAX EUTRA reporting frequencies */
#define TDSL1_MAX_REP_EUTRA_FREQ 4
/* Max cells reported per EUTRA frequency */
#define TDSL1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY 4
#endif

/***************************************************************************
**
**     25.331  Specification Related Information                                                                                              
**
***************************************************************************/

/*-------------------------------------------------------------------------
**      25.331   10.3.10  Multiplicity Values and Type Contraint Values                          
**-------------------------------------------------------------------------*/
/* maxCNdomains - Maximum number of CN domains.*/
#define TDSL1_MAX_CN_DOMAINS          4

/* maxOtherRAT - Maximum number of other Radio Access Technologies.*/
#define TDSL1_MAX_OTHER_RAT           15

/* maxRAT - Maximum number of Radio Access Technologies */
#define TDSL1_MAX_RAT                 (TDSL1_MAX_OTHER_RAT + 1)

/* maxRB - Maximum number of RBs (Radio Bearers).*/
#define TDSL1_MAX_RB            32

/* maxRL - Maximum number of Radio Links
** also defined in tdsl1sapcommon.h(should pull those here).
*/
#undef TDSL1_MAX_RL  
#define TDSL1_MAX_RL                  6

/* maxMeasId */
#define TDSL1_MAX_MEAS_ID             16
#define TDSL1_MEAS_ID_INVALID         (0)
#define TDSL1_MEAS_ID_VALID(measid)     \
           ( ( ((uint16) (measid)) > 0 ) && ((uint16) (measid)) <= TDSL1_MAX_MEAS_ID )
           
/* maxTGPS - Maximum number of transmission gap pattern sequences */
#define TDSL1_MAX_TGPS                6
#define TDSL1_TGPSI_VALID(tgpsi)       \
           ( (((uint16) (tgpsi)) > 0 ) && (((uint16) (tgpsi)) <= TDSL1_MAX_TGPS) )

/*     maxAdditionalMeas - Maximum number of additional measurements for   *
 *                         a given measurement id                          */
#define TDSL1_MAX_ADDITIONAL_MEAS     4
/*     maxMeasEvent - Maximum number of events that can be listed in       *
 *                    measurement reporting criteria                       */
#define TDSL1_MAX_MEAS_EVENT          8
#define TDSL1_MAX_EUTRA_MEAS_EVENT    8
/*     maxMeasParEvent - Maximum number of measurement parameters          *
 *                       (eg. thresholds) per event                        */
#define TDSL1_MAX_MEAS_PAR_EVENT      2
/*     maxMeasIntervals - Maximum number of intervals that define the      *
 *                        mapping function between the measurements for    *
 *                        the cell quality Q of a cell and representing    *
 *                        quality value                                    */
#define TDSL1_MAX_MEAS_INTERVALS      1
/*     maxCellMeas  - Maximum Number of Cells Measured                     */
#define TDSL1_MAX_CELL_MEAS           32
/*     maxReportedGSMCells - Maximum number of GSM cells to be reported    */
#define TDSL1_MAX_REPORTED_GSM_CELLS  6
/*     maxReportedLTECells - Maximum number of LTE cells to be reported    */
#define TDSL1_MAX_REPORTED_LTE_CELLS  4
/*     maxFreq - Maximum number of frequencies to measure                  */
#define TDSL1_MAX_FREQ                12

/*refer to 10.3.7.20a,Maximum number of inter-frequency RACH reporting cells integer(1..8)*/
#ifdef FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING
#define TDSL1_RACH_INTERF_NSET_MAX 8
#endif

/* maxNonUsedFreq - Maximum number of non-used frequencies to measure   */
#define TDSL1_MAX_NON_USED_FREQ       (TDSL1_MAX_FREQ - 1)


#define TDSL1_MAX_FREQ_NUM             TDSL1_MAX_FREQ

#define TDSL1_MAX_FREQ_REPORT_NUM             9

#define TDSL1_MAX_NON_USED_FREQ_TMP       (TDSL1_MAX_FREQ_NUM - 1)

/*Contains the maximum number of intra-frequency cells to be reported on RACH--10.3.7.43*/
#define TDSL1_INTRA_MAX_IN_RACH        6

/*maxNumGSMFreqRanges - Maximum number of GSM Frequency Ranges to store */
#define TDSL1_MAX_NUM_GSM_FREQ_RANGES 32

/*maxNumFDDFreqs - Maximum number of FDD centre frequencies to store  */
#define TDSL1_MAX_NUM_FDD_FREQS       8


/*-------------------------------------------------------------------------
**      25.331   Constants declared directly in IEs                           
**-------------------------------------------------------------------------*/
/* first use, 10.3.7.26 -   BCCH ARFCN  (0..1023)*/
#define TDSL1_MAX_BCCH_ARFCN     1023


/* noted in 8.6.7.2 - Required Filter Coefficient Storage 
** note: specification says "UE shall support 2 filter coefficients per measurement type" 
**       I am assuming "measurement type" is Intra-Freq,Inter-Freq,Inter-Rat, etc.
**       I am assuming "support 2" implies 2 that are non-zero, hence with zero support is 2+1.
*/
#define TDSL1_MAX_FILTER_COEF_STORAGE     (2+1)

/*--DJM--TO BE REMOVED... use TDSL1_MAX_MEAS_EVENT */
#define TDSL1_INTRA_FREQ_MAX_NUM_EVENT  TDSL1_MAX_MEAS_EVENT
/*--DJM--TO BE REMOVED (END) */

/*--DJM--TO BE REMOVED... use TDSL1_MAX_CELL_MEAS */
#define TDSL1_MAX_NUM_MEAS     TDSL1_MAX_CELL_MEAS
/*--DJM--TO BE REMOVED (END) */

/*--DJM--Where does this come from? */
/* Second, maximum number of measurement mapping intervals */
#define TDSL1_MAX_MAP_INTERVAL 3
/*--DJM-- (End) */

/*--DJM--Where does this come from? */
#define TDSL1_MAX_RPT_CELLS 12
/*--DJM-- (End) */

/*--DJM--Where does this come from? */
/* 25.331 constant - djm where does this come from... should it be maxFreq = 8? */
#define TDSL1_MAX_NUM_FREQ    3
/*--DJM-- (End) */

/* used as array index (0,1,2,3) for events 3A, B, C, D to see if the rssi value
** has been used atleast once and then start aging.
*/
#define TDSL1_MAX_INTERRAT_EVENTS 4

#define TDSL1_MBMS_MAX_PL_PER_REQ 4

/* Macro for invalid PSC */
#define TDSL1_NO_PSC 0x0FFF
#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
#define TDSL1_MAX_MONITORED_CELLS_PER_FREQ 8
#define TDSL1_MAX_MONITORED_GSM_CELLS 8
#endif

/* Maximum number of logical channels that can be mapped
** onto the same priority queue. 
*/
#define TDSL1_MAX_LOGCHAN_PER_QUEUE 15

/* L1 will add begin */
/* 10.3.7.59, quality report quantity SIR */
#define L1_MAX_CCTRCH 8

#define L1_RACH_MAX_MEAS_LIST_NO 2

#define L1_MAX_TS 6

/* 10.3.7.38 Intra-frequency measurement quantity, Timeslot is a list, list number is up to 4 */ 
#define  L1_MAX_MEAS_LIST_NO 4
/* L1 will add end */

#define TDSL1_MAX_EPUCH_TS 5

/*==========================================================================
**
**      Enumerated Type Declarations    
**
**==========================================================================*/
/***************************************************************************
**
**     L1 - RRC   Non-Specification Related Enumerated Types
**
***************************************************************************/
/*Frequency Scan Step Mode*/
/*
 * Frequency Scan Step Mode
 */
typedef enum
{
  /* 800 KHz step for raw scan */
  TDSL1_FREQ_RAW_SCAN,    
  /* 200 KHz step for fine scan */
  TDSL1_FREQ_FINE_SCAN,
  /* only scan listed frequencies */
  TDSL1_FREQ_LIST_SCAN
} tdsl1_freq_scan_step_enum_type;


/*
 * The acquisition mode is used as a parameter of 
 * function tdsl1_srch_acq_init().
 */
typedef enum
{
  /* Normal Acquisition Mode, L1 do shallow search then deep search */
  TDSL1_ACQ_MODE_NORMAL,
  /* Short Acquisition Mode, use in G2TD PLMN search scenario */
  TDSL1_ACQ_MODE_SHORT,
  /* Shallow Acquisition Mode, L1 do shallow search only */
  TDSL1_ACQ_MODE_SHALLOW,
  /* Deep Acquisition Mode, L1 do deep search only */
  TDSL1_ACQ_MODE_DEEP,
  /* ReACQ Acquisition Mode, L1 do MEASURE_ALL only, use in TD2TD PLMN search scenario */
  TDSL1_ACQ_MODE_REACQ,
  /* Synchronized IRAT ReACQ Acquisition Mode, L1 skip step 1, 2 and directly do P-CCPCH 
     decoding and SFN reading */
  TDSL1_ACQ_MODE_IRAT_TIMING,
  /* List Frequency Scan Acquisition Mode, L1 do list frequency scan first and sort the frequencies before start ACQ */
  TDSL1_ACQ_MODE_LFS,
  /* Shorter Acquisition Mode, use in G2T/L2T PLMN search with shorter DRX timer */
  TDSL1_ACQ_MODE_SHORTER,
  TDSL1_ACQ_MODE_MAX
} tdsl1_acq_mode_enum_type;


/*
 * Identifies whether SIB, or Measurement Control Parameters are
 * Contained within the measurement command
 */
typedef enum
{
  /* Measurement parameters from SIB_11/12 */
  TDSL1_SIB_PARMS,
  /* Measurement parameters from Measurement Control Message */   
  TDSL1_MEAS_CTRL_PARMS,
  /* Measurement parameters from RRC on state transition from FACH<-->DCH */  
  TDSL1_RRC_STATE_CHANGE_PARMS,
  /* Measurement parameters from RRC for inter freq SIB */ 
  TDSL1_INTER_FREQ_SIB_PARMS 
  /* With Deferred reading feature RRC will defer measurement req to L1*/
  ,TDSL1_DEFERRED_MEASUREMENT_FROM_SIB
 #ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
   /* Measurement parameters and priority information from SIB_19 */
  ,TDSL1_PRIORITY_EUTRA_CELL_LIST_INFO
 #endif
} tdsl1_meas_ctrl_enum_type;

/*
 * Error Indication for use in TDSRRC_CPHY_ERROR_IND message
 */
typedef enum
{
  /* Invalid command in current state*/
  TDSL1_INVALID_CMD_IND,        
  /* Invalid parameters in command*/
  TDSL1_PARM_ERR_IND,           
  /* Unable to process command*/
  TDSL1_UNABLE_PROC_CMD_IND,    
  /* Invalid frequency selection*/
  TDSL1_FREQ_ERR_IND,           
  /* Hardware failure in L1*/
  TDSL1_HARDWARE_FAILURE_IND,   
  /* Firmware failure in L1*/
  TDSL1_FIRMWARE_FAILURE_IND,   
  /* Last error indication*/
  TDSL1_LAST_ERR_IND            
} tdsl1_error_enum_type;

/*
 * Cell Reselection Indication Status
 */
typedef enum
{
  TDSL1_RESEL_FAILURE,
  TDSL1_RESEL_SUCCESS   
  
} tdsl1_cell_resel_status_enum_type;

/*
 * Cell Reselection Indication Cell System Selected
 */
typedef enum
{  
  /*+ huichunc add + */ 
  TDSL1_RESEL_TDSCDMA,
  /*- huichunc add - */   
  TDSL1_RESEL_WCDMA,  
  TDSL1_RESEL_GSM,      
  TDSL1_RESEL_IS2000
#ifdef FEATURE_TDSCDMA_TO_LTE
  ,TDSL1_RESEL_LTE
#endif
} tdsl1_cell_resel_cell_enum_type;

/*
 * Meas Reconfiguration type defined
 */
typedef enum 
{
  TDSL1_MEAS_STOP_AND_OVERWRITE, /* Stop Rpt & Overwrite on current meas */
  TDSL1_MEAS_STOP_AND_CONFIG_CELL_LIST, /* Stop Rpt & Config the cell info list in measurement identity */
  TDSL1_MEAS_DELETE, /* Delete the measurement */
  TDSL1_MEAS_DELETE_ALL
} tdsl1_meas_trans_enum_type;

/*
 * When RRC issues the TDSL1_CPHY_START_TDSCDMA_MODE_REQ command to WL1, it may specify
 * the cause to start TDS L1 stack
 */
typedef enum
{
  TDSL1_START_CAUSE_OTHER,
  TDSL1_START_CAUSE_IRAT_TIMING,
  TDSL1_START_CAUSE_BPLMN_G2T,
  TDSL1_START_CAUSE_BPLMN_L2T,
  TDSL1_START_CAUSE_MAX
}tdsl1_start_cause_enum_type;

/*
 * MM sends RRC_STOP_TDS_MODE_REQ to RRC with a cause field.  Then RRC sends
 * TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ to L1 with cause field below.
 *
 * - RAT Change means we are leaving WCDMA to activate another RAT.
 * - Deep Sleep means we have not found service for some period of time and we now
 *   quit searching and enter a power conservation mode.
 * - TRM (Transceiver Resource Manager) stop means WL1 was either unable to
 *   reserve the primary antenna, or TRM asked WL1 to give up the primary
 *   antenna (and WL1 is giving it up).  These can happen if another RAT has
 *   priority over WCDMA for the antenna at that time.
 */
typedef enum
{
  TDSL1_RAT_CHANGE,
  TDSL1_DEEP_SLEEP,
  TDSL1_TRM_STOP,
  TDSL1_STOP_CAUSE_MAX
} tdsl1_stop_cause_enum_type;

/*
 * When RRC issues the TDSL1_CPHY_START_TDSCDMA_MODE_REQ command to WL1, it may specify
 * the mDSP firmware image to be loaded in the mDSP.  These are the various
 * mDSP image types available.
 */
typedef enum
{
  TDSL1_MDSP_IMAGE_WCDMA_ONLY,
  TDSL1_MDSP_IMAGE_WCDMA_GSM,
  TDSL1_MDSP_IMAGE_WCDMA_1X,
  TDSL1_MDSP_IMAGE_MAX
} tdsl1_mdsp_image_enum_type;

/*RRC sends a TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ to L1 with a specific cause.*/
typedef enum
{
  TDSL1_IRAT_REASON_OTHER,
  TDSL1_IRAT_RESELECTION,
  TDSL1_IRAT_REDIRECTION,
  TDSL1_IRAT_OOS,
  TDSL1_IRAT_BPLMN,
  TDSL1_IRAT_IHO,
  TDSL1_IRAT_CCO,
  TDSL1_IRAT_DUAL_SIM_TUNEAWAY,
  TDSL1_IRAT_NONE
}tdsl1_suspend_resume_cause_enum_type;

typedef enum
{
  TUNEAWAY_DISABLED,                  /*!< \brief TUNEAWAY_DISABLED */
  TUNEAWAY_ALREADY_IN_PROGRESS,       /*!< \brief TUNEAWAY_ALREADY_IN_PROGRESS */
  TIME_NOT_ENOUGH,                    /*!< \brief TIME_NOT_ENOUGH */
  RRC_TASK_DELAY,                     /*!< \brief RRC_TASK_DELAY */
  STATE_INCORRECT,                    /*!< \brief STATE_INCORRECT */
  RESELECTION_IN_PROGRESS,            /*!< \brief RESELECTION_IN_PROGRESS */
  OC_SETTING_IN_PROGRESS,             /*!< \brief OC_SETTING_IN_PROGRESS */
  NOT_CAMP_ON,                        /*!< \brief NOT_CAMP_ON */
  CELL_UPDATE_IN_PROGRESS,            /*!< \brief CELL_UPDATE_IN_PROGRESS */
  RCR_IN_PROGRESS,                    /*!< \brief RCR_IN_PROGRESS */
  TD2G_CCO_IN_PROGRESS,               /*!< \brief TD2G_CCO_IN_PROGRESS */
  G2TD_CCO_IN_PROGRESS,               /*!< \brief G2TD_CCO_IN_PROGRESS */
  CS_DOMAIN_OPEN,                     /*!< \brief CS_DOMAIN_OPEN */
  PEDNING_DL_SRB_DATA_EXIST,          /*!< \brief PEDNING_DL_SRB_DATA_EXIST */
  PENDING_L2_ACK_EXIST,               /*!< \brief PENDING_L2_ACK_EXIST */
  LOOP_BACK_MODE_OPEN,                /*!< \brief LOOP_BACK_MODE_OPEN */
  CPC_DRX_ACTIVE,                     /*!< \brief CPC_DRX_ACTIVE */
  SUSPEND_TD_MODE_IN_PROGRESS,        /*!< \brief SUSPEND_TD_MODE_IN_PROGRESS */
  RRC_CONN_NOT_EST,                   /*!< \brief RRC_CONN_NOT_EST */
  CS_RAB_PRESENT,                     /*!< \brief CS_RAB_PRESENT */
  CS_IDT_IN_PROGRESS,                 /*!< \brief CS_IDT_IN_PROGRESS */
  PS_IDT_IN_PROGRESS,                 /*!< \brief PS_IDT_IN_PROGRESS */
  CONVERSATION_STREAMING_PS_PRESENT,  /*!< \brief CONVERSATION_STREAMING_PS_PRESENT */
  LLC_NOT_IDLE,                       /*!< \brief LLC_NOT_IDLE */
  NEW_UNLOCK_REQEUST_COMES_LATER,	  /*!< \brief NEW_UNLOCK_REQEUST_COMES_LATER*/ 
  NOT_BG_TRAFFIC,                     /*!< \brief NOT_BG_TRAFFIC */
  NO_PS_RB,                           /*!< \brief NO_PS_RB>*/
  BLOCK_REASON_MAX                    /*!< \brief BLOCK_REASON_MAX */
}tdsrrc_tuneaway_block_reason_type;


/*RRC sends a TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ to L1 with the specified target rat.*/
typedef enum
{
  TDSL1_INTERRAT_GSM,
  TDSL1_INTERRAT_LTE,
  TDSL1_INTERRAT_WCDMA,
  TDSL1_INTERRAT_MAX
}tdsl1_interrat_enum_type;

/*
 * When TL1 returns acq_cnf with failure status to RRC, it can specify the 
 * cause of failure
 */
typedef enum
{
  TDSL1_ACQ_FAILURE_CAUSE_NONE,
  TDSL1_ACQ_FAILURE_CAUSE_CELL_BARRED,
  TDSL1_ACQ_FAILURE_CAUSE_MAX
}tdsl1_acq_failure_cause_enum_type;

/*--------------------------------------------------------------------------
                          ENUM: ASET_UPD_ENUM_TYPE
                          
This enum lists the various types of ASET update.

1) Cell Selection - used during BCH state. Note this is not really an ASET 
   update; it is more demod set updated.
2) Cell Reselection - used to demodulate a neighbor BCH for cell reselection 
   during BCH, PCH and FACH state. Note this is not really an ASET update;
   it is more demod set updated. In this mode, the new ASET info in the 
   local command only contains the the neighbor cell and does not contain
   the camp cell. The camp cell will be unchanged.
3) Cell Camp - Used to update actual ASET during BCH, PCH and FACH state. 
4) Macro Diversity - used for soft-handoff during DCH state.
5) Hard Handoff - Used for hard handoff during DCH state.
--------------------------------------------------------------------------*/
typedef enum
{
  TDSL1_ASET_UPD_CELL_RESELECTION,
  TDSL1_ASET_UPD_CELL_CAMP,
  TDSL1_ASET_UPD_CELL_RESELECTION_SUCCESS,
  TDSL1_ASET_UPD_CELL_RESELECTION_FAILURE
} tdsl1_aset_update_enum_type;

/*--------------------------------------------------------------------------
     List of bands for frequency scan
--------------------------------------------------------------------------*/
typedef enum
{
  TDSL1_FREQ_BAND_A,
  TDSL1_FREQ_BAND_E,
  TDSL1_FREQ_BAND_F,
} tdsl1_freq_scan_band_enum_type;


/*--------------------------------------------------------------------------
     Type of search requested by RRC 
--------------------------------------------------------------------------*/

//#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH

typedef enum
{
  TDSL1_SERVICE_SRCH_TYPE,
  TDSL1_MPLMN_LIST_SRCH_TYPE,
  TDSL1_BPLMN_SRCH_TYPE

} tdsl1_wcdma_srch_enum_type;

//#endif

/***************************************************************************
**                                                                       
**     25.331  Specification Related Enumerated Types                   
**                                                                       
***************************************************************************/

/**** 10.2... ****/

/*
 * derived from 10.2.17  - Measurement Control,  Measurement Type Selector
 */
typedef enum
{
  TDSL1_MEAS_TYPE_INVALID,
  TDSL1_INTRA_FREQ,
  TDSL1_INTER_FREQ,
  TDSL1_INTER_SYS,
  TDSL1_LCS,
  TDSL1_TRAF_VOL,
  TDSL1_QUALITY,
  TDSL1_UE_INTERNAL,
  TDSL1_INTER_SYS_EUTRA,
  TDSL1_MEAS_TYPE_MAX
} tdsl1_meas_type_enum_type;

#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
typedef struct
{
  /*Cell Primary Scrambling Code*/
  uint8 cpid;
  
  /*RSCP*/
  int16 rscp;
  
  /*EC/Io*/
  int16 ecio;
} tdsl1_cell_power_info_type;


typedef struct
{
  boolean is_valid;
  
  tdsl1_cell_power_info_type cell_info;
}tdsl1_serving_cell_power_info_type;

typedef struct
{
  /*Number of RLs in active set*/
  uint8 num_rls;
  
  /*Primary Scrambling codes*/
  uint8 cpid[TDSL1_ASET_MAX];
  
  /*RSCP*/
  int16 rscp[TDSL1_ASET_MAX];
  
  /*EC/Io*/
  int16 ecio[TDSL1_ASET_MAX];
} tdsl1_active_set_info_type;


typedef struct
{
  /*UARFCN*/
  uint16 uarfcn;
  
  /*Number of cells in the frequency*/
  uint8 num_of_cells;
  
  /*Cells info*/
  tdsl1_cell_power_info_type cell_info[TDSL1_MAX_MONITORED_CELLS_PER_FREQ];
} tdsl1_monitored_freq_info_type;

typedef struct
{
  /*Number of frequencies*/
  uint8 num_of_freqs;
  
  /*Freq info*/
  tdsl1_monitored_freq_info_type freq[TDSL1_MAX_FREQ_NUM];
  

  rrc_rr_interrat_measurement_data_type gsm_cells_info;

}tdsl1_monitored_set_info_type;


extern boolean tdssrch_modem_stat_call;

#endif /*FEATURE_TDSCDMA_MODEM_STATISTICS*/


#if 0
#if defined(FEATURE_CM_MEASUREMENT)

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif  
tdsl1_meas_type_enum_type TDSL1_TGPS_TYPE( tdsl1_cm_tg_meas_purpose_enum_type tgmp )
{
  tdsl1_meas_type_enum_type ret=TDSL1_MEAS_TYPE_MAX;


#endif
#endif

/**** 10.3.2... ****/

/*
 * derived from 10.3.2.5  - Mapping Information,   Function Type
 */
typedef enum
{
  TDSL1_MAP_FUN_LINEAR,
    /* Supported initially */
  TDSL1_MAP_FUN_TYPE_2,
    /* Not supported */
  TDSL1_MAP_FUN_TYPE_3,
    /* Not supported */
  TDSL1_MAP_FUN_TYPE_4,
    /* Not supported */
  TDSL1_MAP_FUN_TYPE_MAX
} tdsl1_map_function_type_enum_type;


/*
 * derived from 10.3.2.5  - Mapping Information,   RAT
 */
typedef enum
{
  /* RAT is WCDMA FDD.  Supported. */
  TDSL1_UTRA_FDD,
  /* RAT is WCDMA TDD.  Not supported */
  TDSL1_UTRA_TDD,
  /* RAT is GSM.        Supported (soon) */  
  TDSL1_GSM,
  /* RAT is CDMA2000.   Not supported */
  TDSL1_CDMA2000
} tdsl1_rat_enum_type;


/**** 10.3.3... ****/

/*
 * 10.3.3.1  Activation Time (CFN)
 * 10.3.6.75 SFN Time Info, Activation Time (SFN)
 */
typedef enum
{
  TDSL1_ACTIVATION_TIME_NONE,
  TDSL1_ACTIVATION_TIME_CFN
} tdsl1_act_time_enum_type;


/*
 * 10.3.3.13  - Failure cause
 */
typedef enum 
{
  TDSL1_INVALID_CONFIGURATION,
  TDSL1_INCOMPLETE_CONFIGURATION,
  TDSL1_UNSUPPORTED_MEASUREMENT
} tdsl1_failure_cause_enum_type;
typedef tdsl1_failure_cause_enum_type tdsl1_error_cause_enum_type;


/*
 * 10.3.6.36  Frequency Info  Choice
 */
typedef enum
{
  TDSL1_FREQ_MODE_FDD
} tdsl1_freq_mode_enum_type;


/**** 10.3.7... ****/

/*
 * 10.3.2.3  RAT Identifier
 */
typedef enum 
{
  /* GSM system */
  TDSL1_GSM_SYSTEM,    
  /* IS2000 system (currently not supported) */
  TDSL1_IS2000_SYSTEM, 
  TDSL1_EUTRA_SYSTEM,
  /* Neither GSM or IS2000 */
  TDSL1_SYSTEM_NONE    
} tdsl1_rat_type_enum_type;


/*
 * 10.3.7.5  SFN-SFN observed time difference reporting indicator
 */
typedef enum
{
  TDSL1_NO_REPORT,
  TDSL1_REPORT_TYPE_1,
  TDSL1_REPORT_TYPE_2
} tdsl1_sfn_report_enum_type;


/*
 * 10.3.7.9  - Filter Coefficient
 */
typedef enum 
{
  TDSL1_GSM_FILTER_COEF_0 = 0,  
  TDSL1_GSM_FILTER_COEF_1 = 1,  
  TDSL1_GSM_FILTER_COEF_2 = 2,  
  TDSL1_GSM_FILTER_COEF_3 = 3,  
  TDSL1_GSM_FILTER_COEF_4 = 4,  
  TDSL1_GSM_FILTER_COEF_5 = 5,  
  TDSL1_GSM_FILTER_COEF_6 = 6,  
  TDSL1_GSM_FILTER_COEF_7 = 7,  
  TDSL1_GSM_FILTER_COEF_8 = 8,  
  TDSL1_GSM_FILTER_COEF_9 = 9,  
  TDSL1_GSM_FILTER_COEF_11 = 11,  
  TDSL1_GSM_FILTER_COEF_13 = 13,  
  TDSL1_GSM_FILTER_COEF_15 = 15,  
  TDSL1_GSM_FILTER_COEF_17 = 17,  
  TDSL1_GSM_FILTER_COEF_19 = 19,
  TDSL1_GSM_FILTER_COEF_MAX,
  TDSL1_GSM_FILTER_COEF_INVALID
} tdsl1_gsm_filter_coef_enum_type;

/*
 * 10.3.7.9  - Filter Coefficient
 */
typedef enum 
{
  TDSL1_TDS_FILTER_COEF_0 = 0,  
  TDSL1_TDS_FILTER_COEF_1 = 1,  
  TDSL1_TDS_FILTER_COEF_2 = 2,  
  TDSL1_TDS_FILTER_COEF_3 = 3,  
  TDSL1_TDS_FILTER_COEF_4 = 4,  
  TDSL1_TDS_FILTER_COEF_5 = 5,  
  TDSL1_TDS_FILTER_COEF_6 = 6,  
  TDSL1_TDS_FILTER_COEF_7 = 7,  
  TDSL1_TDS_FILTER_COEF_8 = 8,  
  TDSL1_TDS_FILTER_COEF_9 = 9,  
  TDSL1_TDS_FILTER_COEF_11 = 11,  
  TDSL1_TDS_FILTER_COEF_13 = 13,  
  TDSL1_TDS_FILTER_COEF_15 = 15,  
  TDSL1_TDS_FILTER_COEF_17 = 17,  
  TDSL1_TDS_FILTER_COEF_19 = 19,
  TDSL1_TDS_FILTER_COEF_MAX,
  TDSL1_TDS_FILTER_COEF_INVALID
} tdsl1_wcdma_filter_coef_enum_type;

#ifdef FEATURE_TDSCDMA_TO_LTE
/*
 * 10.3.7.9  - Filter Coefficient
 */
typedef enum 
{
  TDSL1_EUTRA_FILTER_COEF_0 = 0,  
  TDSL1_EUTRA_FILTER_COEF_1 = 1,  
  TDSL1_EUTRA_FILTER_COEF_2 = 2,  
  TDSL1_EUTRA_FILTER_COEF_3 = 3,  
  TDSL1_EUTRA_FILTER_COEF_4 = 4,  
  TDSL1_EUTRA_FILTER_COEF_5 = 5,  
  TDSL1_EUTRA_FILTER_COEF_6 = 6,  
  TDSL1_EUTRA_FILTER_COEF_7 = 7,  
  TDSL1_EUTRA_FILTER_COEF_8 = 8,  
  TDSL1_EUTRA_FILTER_COEF_9 = 9,  
  TDSL1_EUTRA_FILTER_COEF_11 = 11,  
  TDSL1_EUTRA_FILTER_COEF_13 = 13,  
  TDSL1_EUTRA_FILTER_COEF_15 = 15,  
  TDSL1_EUTRA_FILTER_COEF_17 = 17,  
  TDSL1_EUTRA_FILTER_COEF_19 = 19,
  TDSL1_EUTRA_FILTER_COEF_MAX,
  TDSL1_EUTRA_FILTER_COEF_INVALID = TDSL1_EUTRA_FILTER_COEF_MAX
} tdsl1_eutra_filter_coef_enum_type;
#endif

/*
 * 10.3.7.14  - Inter-frequency event identity
 */
typedef enum
{
  /* Change of Best Frequency */
  TDSL1_INTER_FREQ_EVENT_2A,
  /* The estimated quality of the currently used frequency is below a threshold *and* the 
  ** estimated quality of a non-used frequency is above a certain threashold              
  */
  TDSL1_INTER_FREQ_EVENT_2B,
  /* The estimated quality of a non-used frequency is above a certain threshold */
  TDSL1_INTER_FREQ_EVENT_2C,
  /* The estimated quality of the currently used frequency falls below a certain threshold */
  TDSL1_INTER_FREQ_EVENT_2D,
  /* The quality of a non-used frequency is below a certain threshold */
  TDSL1_INTER_FREQ_EVENT_2E,
  /* The estimated quality of the currently used frequency is above a certain threshold */
  TDSL1_INTER_FREQ_EVENT_2F  
} tdsl1_inter_freq_event_enum_type;


/*
 * 10.3.7.22  - Set update mode
 */
typedef enum
{
  TDSL1_SET_UPDATE_ON,
  TDSL1_SET_UPDATE_ON_NO_RPT,
  TDSL1_SET_UPDATE_OFF
} tdsl1_set_update_mode_enum_type;


/*
 * 10.3.7.23  - Inter-RAT Cell Info List,  Band Indicator
 */
typedef enum 
{
  TDSL1_DCS_1800_BAND,    /* DCS 1800 band used */
  TDSL1_PCS_1900_BAND     /* PCS 1900 band used */
} tdsl1_gsm_band_indicator_enum_type;


/*
 * 10.3.7.26  - CHOICE BSIC verified or non-verified
 */
typedef enum
{
  TDSL1_BSIC_VERIFIED,
  TDSL1_BSIC_NOT_VERIFIED
} tdsl1_bsic_choice_enum_type;



/*
 * 10.3.7.28  Inter-RAT Event Identity
 */
typedef enum
{
  /* The estimated quality of the currently used UTRAN frequency is below a certain         
  ** threshold *and* the estimated quality of the other system is above a certain threshold 
  */
  TDSL1_INTER_RAT_EVENT_3A,
  /* The estimated quality of the other system is below a certain threshold */
  TDSL1_INTER_RAT_EVENT_3B,
  /* The estimated quality of the other system is above a certain threshold */  
  TDSL1_INTER_RAT_EVENT_3C,
  /* Change of the best cell in other system */
  TDSL1_INTER_RAT_EVENT_3D
} tdsl1_inter_sys_event_enum_type;


/*
 * derived from 10.3.7.29  - Inter-RAT measurement quantity,  Measurement Quantitiy
 */
typedef enum
{
    TDSL1_GSM_CARRIER_RSSI
} tdsl1_gsm_meas_quantity_enum_type;

/*
 * 10.3.7.34  - Intra-frequency event identity 
 */
typedef enum
{
  /* A primary CPICH enters the reporting range. not supported; for FDD only */
  TDSL1_INTRA_FREQ_EVENT_1A,
  /* A primary CPICH leaves the reporting range. not supported; for FDD only */
  TDSL1_INTRA_FREQ_EVENT_1B,
  /* A non-active P-CPICH becomes better than an active P-CPICH. not supported; for FDD only*/
  TDSL1_INTRA_FREQ_EVENT_1C,
  /* Change of the best cell. not supported; for FDD only */
  TDSL1_INTRA_FREQ_EVENT_1D,
  /* A P-CPICH becomes better than an absolute threshold. not supported; for FDD only */
  TDSL1_INTRA_FREQ_EVENT_1E,
  /* A P-CPICH becomes worse than an absolute threshold. not supported; for FDD only */
  TDSL1_INTRA_FREQ_EVENT_1F,
  /* Change of best cell (TDD) */
  TDSL1_INTRA_FREQ_EVENT_1G,
  /* Timeslot ISCP below a certain threshold (TDD) */
  TDSL1_INTRA_FREQ_EVENT_1H,
  /* Timeslot ISCP above a certain threshold (TDD) */  
  TDSL1_INTRA_FREQ_EVENT_1I,
 #ifdef FEATURE_TDSCDMA_HSUPA
  /* A non EDCH aset cell becomes better than EDCH aset cell */
  TDSL1_INTRA_FREQ_EVENT_1J
#endif
} tdsl1_intra_freq_event_enum_type;

/*
 * 10.3.7.38  - Measurement quantity
 *   For cell selection and reselection, only Ec/No and RSCP are used.
 */
typedef enum
{
/* L1 will del  begin */
  TDSL1_EC_NO,
/* L1 will del  end */
/* L1 will add begin */
  TDSL1_ISCP,
/* L1 will add end */
  TDSL1_RSCP,
  TDSL1_PATHLOSS,
  TDSL1_MEAS_NONE
} tdsl1_meas_quan_enum_type;

/*
 * 10.3.7.39  - Triggering Condition(s) 1 & 2 
 */
typedef enum
{
  /* Aset cell triggers event */
  TDSL1_MEAS_EVENT_ASET_TRIG,
  /* Monitored (neighbor) set cell triggers event */  
  TDSL1_MEAS_EVENT_MSET_TRIG,
  /* Aset and Monitored (Neighbor) cell triggers event */
  TDSL1_MEAS_EVENT_ASET_MSET_TRIG,
  /* Detected Set triggers event */
  TDSL1_MEAS_EVENT_DSET_TRIG,
  /* Detected and Monitored (Neighbor) cell triggers event */
  TDSL1_MEAS_EVENT_DSET_MSET_TRIG,
  /* defined for 1D only as it allows all cells to trigger 
  ** trigg condn2 is optional in rel5 and not needed in rel99 
  */
  TDSL1_MEAS_EVENT_ASET_MSET_DSET_TRIG
} tdsl1_meas_event_trigger_enum_type;


/*
 * 10.3.7.43  - Maximum number of reported cells on RACH
 */
typedef enum
{
  /* No Report */
  TDSL1_RACH_RPT_NONE, 
  /* Current cell */  
  TDSL1_RACH_RPT_0,
  /* Current cell + best neighbor */
  TDSL1_RACH_RPT_1,
  /* Current cell + 2 best neighbors */
  TDSL1_RACH_RPT_2,
  /* Current cell + 3 best neighbors */
  TDSL1_RACH_RPT_3,
  /* Current cell + 4 best neighbors */
  TDSL1_RACH_RPT_4,
  /* Current cell + 5 best neighbors */
  TDSL1_RACH_RPT_5,
  /* Current cell + 6 best neighbors */
  TDSL1_RACH_RPT_6
} tdsl1_intra_freq_rach_rpt_max_enum_type;


/*
 * 10.3.7.46  - Measurement Command for measurement control message
 */
typedef enum
{
  TDSL1_MEAS_SETUP,
  TDSL1_MEAS_MODIFY,
  TDSL1_MEAS_RELEASE
} tdsl1_meas_cmd_enum_type;


/*
 * derived from 10.3.7.49  - Measurement Reporting Mode,
 *                              Measurement Report Transfer Mode
 */
typedef enum 
{
  TDSL1_ACKNOWLEDGED_MODE,
  TDSL1_UNACKNOWLEDGED_MODE
} tdsl1_meas_report_trans_mode_enum_type;


/*
 * derived from 10.3.7.49  - Measurement Reporting Mode,
 *                              Periodical Reporting / Event Trigger Reporting Mode
 */
typedef enum
{
  TDSL1_PERIODIC_RPT,
  TDSL1_EVENT_TRIGGER_RPT,
  TDSL1_NO_RPT,
  TDSL1_NO_CHANGE,
  TDSL1_INVALID_RPT_MODE
} tdsl1_meas_report_mode_enum_type;


/*
 * 10.3.7.49  - Measurement reporting mode
 */
typedef struct
{
  /* event based, or periodic */
  tdsl1_meas_report_mode_enum_type        rpt_mode;
  /* measurement transfer mode */    
  tdsl1_meas_report_trans_mode_enum_type  rpt_trans_mode;  
} tdsl1_meas_reporting_mode_struct_type;


/*
 * 10.3.7.51  - Measurement Validity
 */
typedef enum
{
  TDSL1_STATE_DCH,
  TDSL1_ALL_STATES_BUT_DCH,
  TDSL1_ALL_STATES
} tdsl1_meas_validity_enum_type;


/*
 * 10.3.7.53  - Periodical reporting criteria, Amount of reporting
 *                  The default value is infinity
 */
typedef enum
{
  TDSL1_RPT_AMOUNT_INFINITY = 0,
  TDSL1_RPT_AMOUNT_1  = 1,
  TDSL1_L1_RPT_AMOUNT_2  = 2,
  TDSL1_RPT_AMOUNT_4  = 4,
  TDSL1_RPT_AMOUNT_8  = 8,
  TDSL1_RPT_AMOUNT_16 = 16,
  TDSL1_RPT_AMOUNT_32 = 32,
  TDSL1_RPT_AMOUNT_64 = 64
} tdsl1_amount_of_reporting_enum_type;

/*
 * 10.3.7.53  - Periodical reporting criteria, Reporting Interval
 *                  Indicates the interval of periodical report
 *                  interval in milliseconds.
 */
typedef enum
{
  TDSL1_RPT_INTERVAL_INVALID = 0,
  TDSL1_RPT_INTERVAL_250MS   = 250,
  TDSL1_RPT_INTERVAL_500MS   = 500,
  TDSL1_RPT_INTERVAL_1000MS  = 1000,
  TDSL1_RPT_INTERVAL_2000MS  = 2000,
  TDSL1_RPT_INTERVAL_3000MS  = 3000,
  TDSL1_RPT_INTERVAL_4000MS  = 4000,
  TDSL1_RPT_INTERVAL_5000MS  = 5000,
  TDSL1_RPT_INTERVAL_6000MS  = 6000,
  TDSL1_RPT_INTERVAL_8000MS  = 8000,
  TDSL1_RPT_INTERVAL_12000MS = 12000,
  TDSL1_RPT_INTERVAL_16000MS = 16000,
  TDSL1_RPT_INTERVAL_20000MS = 20000,
  TDSL1_RPT_INTERVAL_24000MS = 24000,
  TDSL1_RPT_INTERVAL_28000MS = 28000,
  TDSL1_RPT_INTERVAL_32000MS = 32000,
  TDSL1_RPT_INTERVAL_64000MS = 64000
} tdsl1_reporting_interval_enum_type;


/*
 * 10.3.7.60  - Time Difference Accuracy enumerate type
 */
typedef enum
{
  /* Cell timing accuracy is 40 chips */
  TDSL1_CELL_TIME_ACC_40_CHIPS = 40,     
  /* Cell timing accuracy is 256 chips */
  TDSL1_CELL_TIME_ACC_256_CHIPS = 256,   
  /* Cell timing accuracy is 2560 chips */
  TDSL1_CELL_TIME_ACC_2560_CHIPS = 2560  
} tdsl1_cell_time_acc_enum_type;

/*
 * 10.3.7.61  - Reporting cell status possible combonations
 */
typedef enum
{
  /* Report cells within Active Set */
  TDSL1_RPT_ASET,
  /* Report cells within Monitored Set on used Freq. */  
  TDSL1_RPT_MSET_USED_FREQ,
  /* Report cells within Aset and MSet on used Freq. */
  TDSL1_RPT_ASET_MSET_USED_FREQ,
  /* Report cells within Detected Set */
  TDSL1_RPT_DSET_USED_FREQ,
  /* Report cells within Detected Set on used Freq. */
  TDSL1_RPT_MSET_DSET_USED_FREQ,
  /* Report All Aset + MSet on used Freq. */
  TDSL1_RPT_ALL_ASET_MSET_USED_FREQ,
  /* Report All Aset + DSet on used Freq. */
  TDSL1_RPT_ALL_ASET_DSET_USED_FREQ,
  /* Report All Aset + MSet and/or DSet on used Freq. */
  TDSL1_RPT_ALL_ASET_MSET_DSET_USED_FREQ,
  /* Report cells within Virtual Active Set */
  TDSL1_RPT_VASET,
  /* Report cells within Monitored Set on Non-used Freq. */
  TDSL1_RPT_MSET_NUSED_FREQ,
  /* Report cells within MSet and/or Aset on Non-used Freq. */
  TDSL1_RPT_ASET_MSET_NUSED_FREQ,
  /* Report cells within Virtual ASet + Mset on Non-used Freq. */
  TDSL1_RPT_ALL_VASET_AND_MSET_NUSED_FREQ,
  TDSL1_RPT_ASET_AND_VASET,
  /* Report cells within Active Set + Virtual ASet  or of the Other RAT */
  TDSL1_RPT_OTHER_RAT = TDSL1_RPT_ASET_AND_VASET,
  /* Report cells within MSet and/or Aset on Used and Non-used Freq.*/ 
  TDSL1_RPT_ASET_AND_MSET_USED_NUSED
} tdsl1_meas_report_cell_status_enum_type;


/*
 * 10.3.7.67  - RLC Buffers Payload, Average and Variance
 */
typedef enum
{
  TDSL1_0_BYTES       = 0,
  TDSL1_4_BYTES       = 4,
  TDSL1_8_BYTES       = 8,
  TDSL1_16_BYTES      = 16,
  TDSL1_32_BYTES      = 32,
  TDSL1_64_BYTES      = 64,
  TDSL1_128_BYTES     = 128,
  TDSL1_256_BYTES     = 256,
  TDSL1_512_BYTES     = 512,
  TDSL1_1K_BYTES      =   1*1024,
  TDSL1_2K_BYTES      =   2*TDSL1_1K_BYTES,
  TDSL1_4K_BYTES      =   4*TDSL1_1K_BYTES,
  TDSL1_8K_BYTES      =   8*TDSL1_1K_BYTES,
  TDSL1_16K_BYTES     =  16*TDSL1_1K_BYTES,
  TDSL1_32K_BYTES     =  32*TDSL1_1K_BYTES,
  TDSL1_64K_BYTES     =  64*TDSL1_1K_BYTES,
  TDSL1_128K_BYTES    = 128*TDSL1_1K_BYTES,
  TDSL1_256K_BYTES    = 256*TDSL1_1K_BYTES,
  TDSL1_512K_BYTES    = 512*TDSL1_1K_BYTES,
  TDSL1_1024K_BYTES   = 1024*TDSL1_1K_BYTES
} tdsl1_num_bytes_enum_type ;


/*
 * 10.3.7.75  - Internal measurement event identity 
 */
typedef enum
{
  /* The UE Transmitted power becomes larger than an absolute threshold */
  TDSL1_INTERNAL_MEAS_EVENT_6A = 0,
  /* The UE Transmitted power becomes less than an absolute threshold */  
  TDSL1_INTERNAL_MEAS_EVENT_6B,
  /* The UE Transmitted power reaches its minimum value */
  TDSL1_INTERNAL_MEAS_EVENT_6C,
  /* The UE Transmitted power reaches its maximum value */
  TDSL1_INTERNAL_MEAS_EVENT_6D,
  /* The UE RSSI reaches the UEs dynamic receiveer range */
  TDSL1_INTERNAL_MEAS_EVENT_6E,
  /* The time difference indicated by TADV becomes larger than an
  ** absolute threshold 
  */
  TDSL1_INTERNAL_MEAS_EVENT_6F,
  /* The time difference indicated by TADV becomes less than an
  ** absolute threshold 
  */
  TDSL1_INTERNAL_MEAS_EVENT_6G
} tdsl1_internal_meas_event_enum_type;

#ifdef FEATURE_TDSCDMA_TO_LTE
typedef enum
{
  TDSL1_EUTRA_QUAN_RSRP,
  TDSL1_EUTRA_QUAN_RSRQ,
  TDSL1_EUTRA_QUAN_BOTH,
  TDSL1_EUTRA_QUAN_NONE
} tdsl1_eutra_meas_rpt_quantity_enum_type;
#endif

/*==========================================================================
**
**       Sub-Structure Type Declarations
**
**==========================================================================*/
/***************************************************************************
**
**    25.331  Specification Related Sub-Structure Types
**
***************************************************************************/

/*
** NOTE: Where Possible These IEs are listed in the order found in 25.331
**       The message section, 10.2.xxx is dependent on the IEs defined
**       in 10.3.7.xxx.  A number of IEs within 10.3.7.xxx are dependent
**       on IEs from higher section numbers within the spec.  These IEs
**       will be out of order below.
*/

/*
 * 10.3.2.3  - The contents of this structure are derived from section
 *             10.3.2.3  RAT List sub-structure
 */
#if 0
typedef struct
{  
  tdsl1_rat_type_enum_type   rat_type;  

  /* Inter-RAT (inter-system) measurement threshold. Value (in dB) 
  ** ranges (-32..20) by step of 2. 0xFFFF indicates the parameter
  ** is not applicable.      
  */
  int16                   s_interrat;  
  
  /* Flag indicating whether s_srchhcs is included */ 
  boolean                 s_hcsrat_incl;  

  /* HCS measurement threshold. Value (in dB) ranges (-105..91) 
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */ 
  int16                   s_hcsrat;   
  
  /* Value (in dB) ranges (-32..20) by step of 2. 0xFFFF indicates
  ** the parameter is not applicable.   
  */
  int16                   s_limit_srchrat;     

} tdsl1_interrat_resel_info_type;
#endif
typedef struct
{  
  /* HCS measurement threshold. Value (in dB) ranges (-105..91)  
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */      
  int16                   s_hcsrat;   

  /* Value (in dB) ranges (-105..91) by step of 2. 0xFFFF indicates 
  ** the parameter is not applicable.   
  */
  int16                   s_limit_srchrat;   
  
  /* Inter-RAT (inter-system) measurement threshold. Value (in dB) 
  ** ranges (-105..91) by step of 2. 0xFFFF indicates the parameter
  ** is not applicable.      
  */      
  int16                   s_interrat;  

  tdsl1_rat_type_enum_type   rat_type;  
  
  /* Flag indicating whether s_interrat is absent */
  boolean                 s_interrat_absent;  
    
  /* Flag indicating whether s_srchhcs is included */
  boolean                 s_hcsrat_incl;   

} tdsl1_interrat_resel_info_type_tdstemp;

/*
 * 10.3.2.4  - Cell Selection Re-Selection Info Structure Type
 */
typedef struct
{
  /* Offset used for CCPCH RSCP. Integer (-50..50), Default 0 */
  int16        qoffset1_s_n;

  /* L1 will del begin */
  /* Offset used for CPICH Ec/No. Integer (-50..50), Default 0  
  ** Only required for WCDMA-FDD Cells
  */  
  int16        qoffset2_s_n;
  /* L1 will del end */

  /* Maximum allowed ULTX power. UE_TXPWR_MAX_RACH dBm */
  int16        max_tx_pwr;
    
  /* L1 will del begin */
  /* Q-qualmin for Ec/Io. Int (-24..0) dBm.Default Q-qualmin for serving cell.  
  ** Only valid for WCDMA - FDD Cells 
  */  
  int16        qual_min;
  /* L1 will del end */

  /* For FDD & TDD Cells: Q-rxlevmin for RSCP (dBm). 
  ** For GSM: Q-rxlevmin for GSM RSSI (dBm). Int (-115..-25 by step of 2).
  ** Default value is Q-rxlevmin for serving cell.   
  */
  int16        rxlev_min;
} tdsl1_cell_sel_resel_info_struct_type;

/*
 * 10.3.7.10 & 10.3.7.11  - HCS neighbor cell information
 */
typedef struct
{
  /* Hierachical Cell Structrue Priority */
  uint16        hcs_prio;
  
  /* Quality Threshold level for HCS cell reselection */    
  uint16        q_hcs;

  /* Penalty_time. 0 means not used. next two not allowed for 0 */    
  uint16        penalty_time;

  /* Temporary_offset used for PCCPCH RSCP*/    
  uint16        tmp_offset1;
 
  /* L1 wll del begin */
  /* Temporary_offset used for CPICH Ec/No */
  uint16        tmp_offset2;
  /* L1 wll del end */  

} tdsl1_nbr_cell_hcs_info_struct_type;

/*
 * 10.3.2.5  - Parameters pertinent to measurement mapping function
 */
typedef struct
{
  /* The number of RATs to be measured */
  int16 num_rat;

  /* Parameters for each RAT */
  struct
  {
    /* Number of mapping intervals */ 
    int16  num_intvl;
  
    /* parameters for each mapping interval */      
    struct
    {

    
      /* Parameter used by the mapping function indicated above */        
      int16   map_parm_1;
    
      /* Another parameter used by the mapping function indicated above */
      int16   map_parm_2;
    
      /* Upper limit, as specified by 10.3.2.5 of TS25.331 */        
      int16   up_limit;        

      /* Mapping function type, as specified by 10.3.2.5 of TS25.331 */
      tdsl1_map_function_type_enum_type   func_type;
    } intvl[TDSL1_MAX_MEAS_INTERVALS];
  } rat[TDSL1_MAX_RAT];
} tdsl1_mapping_info_struct_type;

/*
 * 10.3.2.5  - Parameters pertinent to the LCR R4 mapping function for TDD 1.28Mcps
 */
typedef struct
{
   /* Number of mapping intervals */
   int16  num_intvl;

   /* parameters for each mapping interval */      
   struct
   {
    
      /* Parameter used by the mapping function indicated above */        
      int16   map_parm_1;
    
      /* Another parameter used by the mapping function indicated above */       
      int16   map_parm_2;
        
      /* Upper limit, as specified by 10.3.2.5 of TS25.331 */	  
      int16   up_limit;
       
      /* Mapping function type, as specified by 10.3.2.5 of TS25.331 */
      tdsl1_map_function_type_enum_type   func_type;
      
      boolean up_limit_incl;

   } intvl[TDSL1_MAX_MEAS_INTERVALS];
} tdsl1_mapping_LCR_struct_type;


/**** 10.3.8... ****/

/*
 * 10.3.8.2  BSIC - Base Station Identifier Code
 *           from 3GPP 23.003  section 4.3.2
 */
#define TDSL1_BSIC_COLOR_CODE_MASK               0x07
typedef struct
{
  /* PLMN Colour Code low 3 bits are valid {b00000111}*/
  uint8 ncc;
  
  /* Base Station Colour Code low 3 bits are valid {b00000111} */    
  uint8 bcc;   
} tdsl1_gsm_bsic_struct_type;



/**** 10.3.7... ****/
/*
 * 10.3.7.2  - Cell Info Structure Type
 */
typedef struct
{
  /* Cell Individual Offset, Real (-10..10 by step of 0.5) dB, default 0  */
  int32        cell_offset;
  
  /* Flag indicating whether Ref time diff to the serving cell is included */
  boolean       ref_tm_diff_incl;
  
  /* Reference Time difference to Serving Cell, integer (0..38400) */ 
  uint16        ref_tm_diff;
  
  /* Neighbor Cell timing accuracy relative to the serving cell */    
  tdsl1_cell_time_acc_enum_type time_acc;
  
  /* L1 will del begin */
  boolean      pri_scr_code_incl;
  
  /* Primary Scrambling Code. Not needed if measuring RSSI only. (0..511) */ 
  uint16        pri_scr_code;
  
  boolean       pri_cpich_txpwr_incl;
  
  /* Primary CPICH Tx power. Needed if calculating patchloss
  ** Units: Int  -10... +50 dBm 
  */
  int32        pri_cpich_txpwr;
  /* L1 will del end */    
  
  /* L1 will add begin */
  boolean      cpi_incl;

  /* Cell Prarameter ID. (0..127) */ 
  uint16       cpi;
  
  boolean       pccpch_txpwr_incl;
  
  /* PCCPCH Tx power. Needed if calculating patchloss
  ** Units: Int  -10... +50 dBm 
  */
  int32         pccpch_txpwr;
  
  uint16        no_ts;
  
  uint8  ts_list[L1_MAX_TS];
  
  boolean       tstd_ind; /* ?? need? */
  /* L1 will add end */

  /* Read SFN Indicator.TRUE means read of SFN requested for target cell*/  
  boolean       rd_sfn_ind;
  
  /* L1 will del begin */
  /* TX Diversity Indicator */
  boolean       tx_diver_ind;
  /* L1 will del end */

  /* Flag to indicate if cell selection and reselection info is    
  ** specified for this cell or not. If TRUE, the following fields  
  ** are meaningful; otherwise, ignore the following fields.        
  ** if HCS is not used and if all of the values are defaults, then 
  ** this field can be absent. 
  */  
  boolean       cell_sel_resel_info_incl;

  tdsl1_cell_sel_resel_info_struct_type  cell_sel_info;
  
  boolean hcs_nbr_incl;

  /* Neighbor cell HCS information */  
  tdsl1_nbr_cell_hcs_info_struct_type  hcs_info;  
  
  /* Flag to indicate if cell is high-cost cell.
  ** If either LAC or RAC of the cell is different from the serving cell,
  ** set to TRUE.
  ** otherwise, set to FALSE to mark the cell as normal.
  */
  boolean       high_cost_cell;
} tdsl1_cell_info_struct_type;


/*
 * 10.3.7.6  - Cell synchronization information
 */
typedef struct 
{
  /* choice FDD(supported), TDD(not-supported) */
  tdsl1_freq_mode_enum_type         freq_mode;

  /* FDD */
  boolean  count_c_sfn_diff_incl;

  /* Integer(0..3840 by step of 256) in frames */  
  uint16 count_c_sfn_high;

  /* 5.1.9 of TS25.215 
  ** OFF is defined as (SFN-CFNtx) mod 256 given in no. of frames 
  ** Has range:0...255  
  */    
  uint16 off ;

  /* defined in 5.1.9 of TS 25.215  Range:0..38399     
  ** Given both Tm and OFF , the SFN-CFN observed time 
  ** difference to cell is defined as: OFFx38400 + Tm  
  */
  uint32 tm;

  /* TDD - unsupported */
  /* uint16 tdd; */
} tdsl1_intra_meas_rpt_cell_sync_info_struct_type ;


/*
 * 10.3.7.3  - measured results for each cell 
 */
typedef struct
{
  /* SFN-SFN observed time diff reporting indicator */
  tdsl1_sfn_report_enum_type sfn_rpt_ind;
    
  /* Flag indicating if SFN-SFN observed time diff is included 
  ** 10.3.7.63 SFN-SFN observed time difference                 
  **               Range: 0....9830399 if Type 1 (chips)           
  **               -20480..0..20480 if type 2 (1/16 of chips) 
  */
  uint32 sfn_sfn_tm_diff ;

  /* Flag indicating if cell synchronization info is included */    
  boolean cell_sync_info_incl;
    
  /* 10.3.7.6 cell synchronization information */
  tdsl1_intra_meas_rpt_cell_sync_info_struct_type cell_sync_info;
    
  /* L1 will del begin */
  /* 10.3.6.60 Primary scriambling code 0...511 */
  uint16 pri_scr_code ;
  /* L1 will del end */
  
  /* L1 will add begin */
  uint16 cpi ;
  /* L1 will add end */
  
  /* The type of measurement quantity */
  tdsl1_meas_quan_enum_type meas_quan_type;

  /* value of the quantity  measured */    
  uint16      meas_quan_value;    

  /*   
  ** The values will be returned as per the mapping specified in 25.133
  ** V3.6.0    
  **  
  ** For RSCP : Table 9-4   (eg., < -115 --> 00
  **                                           -115 -->01
  **                                                ...
  **                                        >= -25 -->91
  **                               
  ** For ECNO:  Table 9-9  (eg:   < -24   -->00
  **                                           -24   -->01
  **                                           -23.5 -->02
  **                                                   ...
  **                                            >= 0   --->49
  */
  /* Flag indicating if the EcNo is included      */ 
  /* L1 will del begin */
  boolean ec_no_included;
  
  uint16 ec_no_val;
  /* L1 will del end */
  
  /* L1 will add begin */
  boolean ts_list_included;
  
  uint8   no_ts;
  
  struct
  {
    uint8   ts_no;
  
    uint16  iscp_value;
  }iscp_ts_list[L1_MAX_TS];
  /* L1 will add end */

  /* Flag indicating if the RSCP value is included */
  boolean rscp_included;
  
  uint16 rscp_val;

  /* Flag indicating if the pathloss is included  */
  boolean pathloss_included;
  
  uint16 pathloss_val;

  boolean delta_rscp_included;

  int16 delta_rscp;
} tdsl1_intra_meas_rpt_cell_info_struct_type ;

/*
 * 10.3.7.5  - Report quantities
 */
typedef struct
{
  /* SFN-SFN observed time diff reporting indicator */
  tdsl1_sfn_report_enum_type sfn_rpt;

  /* Cell synchronization information reporting indicator */
  boolean   cfn_sfn_delta_rpt;

  /* Cell Identity reporting indicator */
  boolean   cell_id_rpt ;
  
  /* L1 will del begin */
  boolean   ec_no_rpt;
  /* L1 will del end */

  /* L1 will add begin */
  /* PCCPCH ISCP reporting indicator */
  boolean   iscp_rpt;
  /* L1 will add end */

  /* PCCPCH RSCP reporting indicator */
  boolean   rscp_rpt;

  /* Pathloss reporting indicator */
  boolean   pathloss_rpt;
} tdsl1_intra_dch_rpt_quan_struct_type;

/* 
 * 10.3.7.8 FACH measurement occasion info
 */
#define TDSL1_NUM_OTHER_RATS 2
typedef struct 
{  
  /* Indicates if FACH measurement occasion cycle length coefficient is present */
  boolean k_ind;

  /*FACH measurement occasion cycle length coefficient */ 
  uint32        k; 

  /* Inter-frequency FDD Measurement indicator, N_FDD */ 
  boolean       inter_f_ind; 

  /* Number of different RATS to be measured. */ 
  uint16        num_inter_sys; 

  /* What RATs to perform measurments on. N_GSM, and future N_2000 */ 
  tdsl1_rat_type_enum_type        inter_sys[TDSL1_NUM_OTHER_RATS]; 
} tdsl1_fach_meas_info_struct_type;


/* Enum defining all possible values for Tcrmax */
typedef enum
{
  TDSL1_T_CR_MAX_NOT_USED,
  TDSL1_T_CR_MAX_30S  = 30,
  TDSL1_T_CR_MAX_60S  = 60,
  TDSL1_T_CR_MAX_120S = 120,
  TDSL1_T_CR_MAX_180S = 180,
  TDSL1_T_CR_MAX_240S = 240
} tdsl1_t_cr_max_enum_type;

/* Enum defining all possible values for Tcrmaxhyst */
typedef enum
{
  TDSL1_T_CR_MAX_HYST_NOT_USED,
  TDSL1_T_CR_MAX_HYST_10S = 10,
  TDSL1_T_CR_MAX_HYST_20S = 20,
  TDSL1_T_CR_MAX_HYST_30S = 30,
  TDSL1_T_CR_MAX_HYST_40S = 40,
  TDSL1_T_CR_MAX_HYST_50S = 50,
  TDSL1_T_CR_MAX_HYST_60S = 60,
  TDSL1_T_CR_MAX_HYST_70S = 70
} tdsl1_t_cr_max_hyst_enum_type;

/*
 * 10.3.7.11  - HCS serving cell information
 */
typedef struct
{
  /* Serving cell HCS priority. Value ranges (0..7). Default value is 0. */
  uint16   hcs_prio;

  /* Quality threshold level for HCS cell reselection. Value ranges (0..99). 
  ** Default value is 0. 
  */
  uint16   q_hcs;

  /* maximum number fo cell reselections. Value ranges (1..16). 
  ** Default value is 8. 
  */  
  uint16   n_cr;

  /* Duration for evaluating alowed amount of cell reselections. 
  ** Default value is 0, which means the parameter is not used. When used, 
  ** the values are (0, 30, 60, 120, 180, 240), (in second). 
  */  
  tdsl1_t_cr_max_enum_type   t_crmax;

  /* Additional time period before UE recert to low-mobility measurement. 
  ** Default value is 0, which means the parameter is not used. When used, 
  ** the value ranges (10..70) by step of 10, (in second). 
  */  
  tdsl1_t_cr_max_hyst_enum_type   t_crmaxhyst;
} tdsl1_serv_cell_hcs_info_struct_type;

/*
 * 10.3.6.36  Frequency Info
 */
typedef struct
{
  /* Flag indicationg UL UARFCN info is included. */
  boolean uarfcn_ul_incl;

  /* Uplink UARFCN, Nu (0..16383) */

  uint16  uarfcn_ul;

  /* Downlink UARFCN, Nd (0..16383) */
  uint16  uarfcn_dl;  
} tdsl1_freq_info_struct_type;

typedef struct
{
  uint8   num_freq;
  uint16  freq[TDSL1_MAX_MSET_LIST_NUM];
}tdsl1_mset_list_type;

typedef struct
{
  uint8   num_cell;
  uint8   cell_id[TDSL1_MAX_JDS_CELL_NUM];
}tdsl1_jds_cell_list_type;

/*
 * 10.3.7.13, 10.3.7.23  - Cell Info List Remove Command
 */
typedef enum
{
  TDSL1_CELL_LIST_REMOVE_ALL_CELLS,
  TDSL1_CELL_LIST_REMOVE_SOME_CELLS,
  TDSL1_CELL_LIST_REMOVE_NO_CELLS
} tdsl1_inter_sys_cell_list_remove_enum_type;

/*
 * 10.3.7.13  - Inter-frequency cell list information
 */
typedef struct
{
  /* Removed inter-freqency cells 
  ** Number of inter-freq cells removed 0=none, 1..TDSL1_MAX_CELL_MEAS 
  ** TDSL1_REMOVE_ALL_CELLS(=0xFF)=remove all 
  */
  uint16       num_cell_rmv;

  struct 
  {
    uint16    freq;
    
    /* L1 will del begin */
    /* Primary Scrambling Code  */  
    uint16    psc;
    /* L1 will del end */
  
    /* L1 will add begin for TDD */
    boolean   cpi_incl;
  
    uint16   cpi;
    /* Cell Parameter ID */
    /* L1 will add end for TDD */  
  } rmv_cell[ TDSL1_MAX_CELL_MEAS ];
  
  /* New inter-frequency cells 
  ** Number of inter-frequency cells Added   
  */
  uint16       num_cell_add;
  
  struct  
  {
    /* L1 will del begin */
    /* Primary Scrambling Code  */
    uint8    cpid;
    /* L1 will del end */
  
    /* L1 will add begin */
    /* Cell Parameter ID */
    boolean   cpi_incl;   
  
    uint16   cpi;
    
    /* If NW config the IE 'intraSecondaryFreqIndicator', will set TRUE. */
    boolean   sec_freq_ind_incl;  
    
    boolean sec_freq_ind;
    /* L1 will add end */
  
    boolean measure_in_idle_pch_fach;
    
    tdsl1_freq_info_struct_type freq_info;
  
    tdsl1_cell_info_struct_type  cell_info;
  } add_cell[ TDSL1_MAX_CELL_MEAS ];

  /* 10.3.7.47  measurement cotrol system information  
  ** Flag indicating if serving cell belongs to a HCS structure 
  */  
  boolean       hcs_used;

  
  /* If cells_for_meas_incl is set to TRUE, then cells mentioned in this 
  ** list should only be measured. For SIB's, its always set to FALSE
  */
  boolean cells_for_meas_incl;
  
  /* No of cells to measure */
  uint16 num_cell_meas;
  
  /* Only cells mentioned in this list should be measured. This list 
  ** contains the psc and frequency for the cell 
  */
  struct
  { 
    /* L1 will del begin */
    uint16 psc;
    /* L1 will del end */
  
    uint16 dl_freq;
  
    /* L1 will add begin */
    uint16  cpi;
    /* If NW config the IE 'intraSecondaryFreqIndicator', will set TRUE. */
    boolean   sec_freq_ind_incl;  
    
    boolean sec_freq_ind;
    /* L1 will add end */    
  } meas_cells_list[ TDSL1_MAX_CELL_MEAS ]; 
} tdsl1_inter_freq_cell_list_struct_type;

/*
 * 10.3.7.15  - Inter-Frequency measured results list
 */
typedef struct 
{
  uint16 num_cells;
  
  tdsl1_intra_meas_rpt_cell_info_struct_type cell[TDSL1_MAX_CELL_MEAS];
  
} tdsl1_intra_freq_addl_meas_rpt_struct_type;


/*
 * 10.3.7.15  Inter-Frequency Measured Results List
 */
typedef struct 
{
  /* number of inter-frequency measurement results included spec[1..maxFreq], 0=>no info */
  uint16 num_msmt_results;
    
  struct
  {
    /* frequency information */  
    tdsl1_freq_info_struct_type  freq_info;

    /* Flag indicationg UTRA Carrier RSSI info is included. */
    boolean            utra_carrier_rssi_incl;

    /* UTRA_carrier_RSSI_LEV from 25.133 */
    uint16             utra_carrier_rssi;
    
    /*Inter-Frequency Cell Measurement Results */
    /* Number of Cell Measured Results Included[1..maxCellMeas], 0=>no info */
    uint16 num_cells;

    /* Cell Measured Results */    
    tdsl1_intra_meas_rpt_cell_info_struct_type* cell[TDSL1_MAX_CELL_MEAS];    
  } msmt_results[TDSL1_MAX_FREQ_NUM];
} tdsl1_inter_freq_meas_results_struct_type;

/*
 * 10.3.7.17  Inter-Frequency Measurement Event Results
 */
typedef struct
{
  /* event identity, 2a, 2b, etc. */
  tdsl1_inter_freq_event_enum_type  evt_id;

  /* Inter-Frequency Cells  spec[1..maxFreq], 0=>no info */  
  uint16 num_freq;
  
  struct 
  {
    /* frequency information */  
    tdsl1_freq_info_struct_type freq_info;
    
    /* 10.3.7.4  Cell Measurement Event Results 
    ** choice, FDD(supported)/TDD(not supported) 
    ** FDD 
    */
    /* L1 will del begin */
    uint16 num_psc;

    /* Primary Scrambling Code Index */ 
    uint16   prim_scr_code[TDSL1_MAX_CELL_MEAS];
    /* L1 will del end */
  
    /* L1 will add begin */
    uint16 num_cpi;
    
    uint16   cpi[TDSL1_MAX_CELL_MEAS];
    /* L1 will add end */
  } freq [TDSL1_MAX_FREQ_NUM] ;  
} tdsl1_inter_freq_event_results;

/*
 * 10.3.7.*15*  - Inter-frequency measured results list *plus* inter-freq event results
 */
typedef struct 
{
  /* 10.3.7.17 - event results for 10.3.7.7, from msmt report msg 10.2.19 */
  tdsl1_inter_freq_event_results  event_results;
   
  boolean meas_results_included;

  /* Inter-Frequency measurements */  
  tdsl1_inter_freq_meas_results_struct_type inter_freq_measurements;  
} tdsl1_inter_freq_meas_rpt_struct_type ;

/*
 * 10.3.7.38  - Intra-frequency measurement quantity
 */
typedef struct
{
  /* Filter coefficient as in 10.3.7.9 */
  uint16    filter_coef;
  
  /* L1 interface del begin */
  tdsl1_meas_quan_enum_type   quan_type;
  /* L1 interface del end */

  /* Measurement quantity. Note that, for TDSCDMA TDD cells, we currently 
  ** support ISCP or RSCP Measurement Quantity for DCH Measurement report.
  */
  /* L1 will add begin */
  uint8         meas_quan_list_no;

  tdsl1_meas_quan_enum_type   meas_quan[L1_MAX_MEAS_LIST_NO];  
  /* L1 will add end */  
} tdsl1_intra_freq_meas_quan_struct_type;

/*
 * 10.3.7.18  - Inter-frequency measurement quantity
 */
typedef struct
{
  /* Can only be TDSL1_INTRA_FREQ or TDSL1_INTER_FREQ */  
  tdsl1_meas_type_enum_type rpt_choice;

  /* Inter-frequency measurment quantity as defined in 10.3.7.18    
  ** (Currently matches the intra-frequency definition in 10.3.7.18 
  ** For TDSL1_INTER_FREQ, interf_meas_quan.tdsl1_meas_quan_enum_type can L1_EC_N0 or TDSL1_RSCP only 
  ** For TDSL1_INTRA_FREQ, interf_meas_quan.tdsl1_meas_quan_enum_type can be any value 
  */  
  tdsl1_intra_freq_meas_quan_struct_type   interf_meas_quan;
} tdsl1_inter_freq_meas_quan_struct_type;


/*
 * 10.3.7.61
 */
typedef enum 
{
  /* Max number that can be reported is 6 */
  TDSL1_CELL_REPORTING_TYPE1,    
  /* Max number that can be reported is 12 */
  TDSL1_CELL_REPORTING_TYPE2,     
  /* Max number that can be reported is 6 */ 
  TDSL1_CELL_REPORTING_TYPE3      
} tdsl1_meas_report_cell_reporting_enum_type;


/*
 * 10.3.7.61  - Reporting cell status
 */
typedef struct
{
  tdsl1_meas_report_cell_status_enum_type rpt_cell_stat;
  
  tdsl1_meas_report_cell_reporting_enum_type rpt_cell_type;  
  
  /* Maximum number of reported cells (1..6) */
  uint16 max_num_rpt_cells ;     
} tdsl1_meas_rpt_cell_status_struct_type;


/*
 * 10.3.7.19  - Inter-Frequency measurement reporting criteria
 */
typedef struct
{
  /* Event ID */
  tdsl1_inter_freq_event_enum_type  evt_id;

  /* Threshold used for frequency for event 2B, 2D, and 2F. range (-115..0). 
  ** Range used depend on measurement quantity: 
  **      CPICH RSCP -115..-25 dBm; 
  **      CPICH Ec/Io -24..0 dB; 
  */
  int16  freq_thresh;

  /* Weight used for events 2A, 2B, 2D and 2F, value set to 0xFFFF for all 
  ** other events. Range (0..2.0) by step of 0.1, actual value passed is w_used * 10. 
  */
  uint16  w_used;

  /* Hysteresis. range (0..19) dBm  in 0.5 dBm steps */
  uint16  hyst;
 
  /* Time duration for which the event triggering condition needs to 
  ** be fulfilled before the report can be sent, in ms. 
  */  
  uint16  time_to_trig;
  
  boolean   rpt_cell_status_incl;

  /* Reporting cell status */
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status;
  
  /* Parameters Required for each non-used Frequency
  ** optional parameter, 0 => none, else 1..maxFreq included 
  */  
  uint16    num_non_used_f_parm;
  
  struct
  {
    /* Threshold for non used frequency. Used for events 2A, 2B, 2C and 2E 
    ** range (-115..0) 
    ** Range used depend on measurement quantity: 
    **      CPICH RSCP -115..-25 dBm; 
    **      CPICH Ec/Io -24..0 dB; 
    */
    int16     non_used_f_thresh;

    /* Weight used for non-used frequency for events 2A, 2B, 2C and 2E, 
    ** value set to 0xFFFF for all other events.  
    ** Range (0..2.0) by step of 0.1, actual value passed is w_non_used_f * 10. 
    */  
    uint16    w_non_used_f;
  } non_used_f_parm_array[TDSL1_MAX_NON_USED_FREQ_TMP];

} tdsl1_inter_freq_event_crit_struct_type;


/*
 * 10.3.7.22  - Inter-frequency set update info
 */
typedef struct
{
  /* Autonmous update mode  or 
  ** Non-Autonmous update mode, only used if auto_update_mode is set to OFF 
  */
  tdsl1_set_update_mode_enum_type       auto_update_mode;

  /* Flag indicationg radio link info is included. */
  boolean  radio_link_info_incl;

  /* Number of additional radio links to add. */
  uint16   num_add_radio_link;

  /* Primary Scrambling Code for radio links to add. */
  uint16   add_psc[TDSL1_MAX_RL];

  /* Number of additional radio links to remove. */
  uint16   num_rmv_radio_link;

  /* Primary Scrambling Code for radio links to remove. */
  uint16   rmv_psc[TDSL1_MAX_RL];
} tdsl1_inter_freq_set_update_struct_type;


/*
 * 10.3.7.21  - Inter-frequency reporting quantity
 */
typedef struct
{
  /* TRUE means report of the UTRA Carrier RSSI is requested */
  boolean          rpt_rssi;

  /* TRUE means report of the frequency quality estimat is requested
  ** Currently not used, should be ignored 
  */
  boolean          freq_qual_est;
  
  tdsl1_intra_dch_rpt_quan_struct_type  other_rpt_quan;
} tdsl1_inter_freq_rpt_quan_struct_type;


/*
 * 10.3.7.39  - criteria per event as defined in 10.3.7.39 of TS25.331.
 */
typedef struct
{
  /* Event ID */
  tdsl1_intra_freq_event_enum_type  evt_id;

  /* 0..29 by step of 0.5dB for event 1A and 1B. Set to 0xFFFF for all
  ** other events. 
  */
  uint16   rpt_range;

  /* Number of cells foebidden to affect report range for event 1A and 1B. 
  ** set to 0xFFFF for all other events. 
  */   
  uint16  num_cell_forbidded;

  /* Primary Scrambling Code for each forbidden cell */
  uint16  forbidden_cell_scr_code[ TDSL1_MAX_CELL_MEAS ];

  /* Weight for even 1A and 1B. 0..20 by step of 0.1. set to 0xFFFF
  ** for all other events. 
  */
  uint16  w;

  /* Hysteresis. 0..7.5 by step of 0.5 dB, actual value is hyst*2. */
  uint16  hyst;

  /* Threshold used for frequency for event 1E, 1F, 1H and 1I. -125..165. 
  ** Range used depend on measurement quantity: 
  **      PCCPCH ISCP -115~-25dbm;             
  */  
  int16  freq_thresh;

  /* Reporting deactivation threshold for event 1A, indicating        
  ** the maximum number of cells allowed in the active set in order   
  ** for event 1A to occur. value range 0..7. 0 means not applicable. 
  */  
  uint16  rpt_deact_thresh;

  /* Reporting activation thresh for 1c. Indicates min. cells      
  ** needed in ASET in order for 1C to occur. 0...7 , 0 means none 
  */    
  uint16 rpt_act_thresh;

  /* Time duration for which the event triggering condition      
  ** needs to be fulfilled before the report can be sent, in ms. 
  */    
  uint16  time_to_trig;

  /* Measurement is "released" after the indicated amount (number) of   
  ** reporting(s) from the UE itself. 0 means "infinity". For 1A or 1C. 
  */  
  uint16  num_rpt;

  /* Interval of periodical reporting, in ms, for 
  ** 1A or 1C. 0 means no periodical reporting.   
  */    
  uint16  rpt_intvl;
  
  boolean   rpt_cell_status_incl;

  /* Reporting cell status */
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status;
  
  /* TRUE means use CIO for Event 1D */
  boolean evt_1d_cio_incl; 

} tdsl1_intra_freq_event_crit_struct_type;


/*
 * 10.3.7.39  - Intra-frequency measurement reporting criteria
 */
typedef struct
{
  uint16 num_event;
  
  tdsl1_intra_freq_event_crit_struct_type evt[TDSL1_MAX_MEAS_EVENT];
} tdsl1_intra_freq_report_crit_struct_type;


/*
 * 10.3.7.19  - Inter-frequency report criteria
 */
typedef struct
{
  uint16 num_event;
  
  tdsl1_inter_freq_event_crit_struct_type evt[TDSL1_MAX_MEAS_EVENT];
} tdsl1_inter_freq_report_crit_struct_type;


/*
 * 10.3.7.53  - Periodical reporting criteria
 */
#define TDSL1_PERIODIC_RPT_AMOUNT_INFINITY      0

typedef struct
{
  /* Amount of reporting (1,2,4,8,16,32,64, Infinity 
  ** Measurement is "released" after the indicated amount (number) of 
  ** report(s) from the UE itself. 0 means "infinity".   
  */
  tdsl1_amount_of_reporting_enum_type rpt_amount;

  /* Reporting Interval  (250,500,1000,2000, ... ) in ms */
  tdsl1_reporting_interval_enum_type  rpt_interval;
} tdsl1_per_rpt_crit_struct_type;


typedef enum
{
  TDSL1_INTRA_FREQ_EVT_TRIG,
  TDSL1_INTER_FREQ_EVT_TRIG,
  TDSL1_INTER_FREQ_PERIODIC,
  TDSL1_INTER_FREQ_NO_REPORTING,
  TDSL1_INTER_FREQ_NO_CHANGE
} tdsl1_inter_freq_meas_report_mode_enum_type;
/*
 * 10.3.7.16  - Inter-frequency info
 */
typedef struct
{
  /* flag to indicate if cell list is present  */
  boolean cell_list_incl;

  /* cell information 10.3.7.13 */
  tdsl1_inter_freq_cell_list_struct_type  cell_list;

  /* flag to indicate if meas quantity is present  */
  boolean   meas_quan_incl;

  /* Inter-frequency measurement quantity as in 10.3.7.18 of TS25.331 */
  tdsl1_inter_freq_meas_quan_struct_type  meas_quan;

  /* flag indicating if the reporting quantities are given. If yes, next four 
  ** fields are valid 
  */
  boolean rpt_quan_incl ;

  /* Inter-frequency reporting quantity as defined in 10.3.7.21 of TS25.331 */  
  tdsl1_inter_freq_rpt_quan_struct_type    rpt_quan;
  
  boolean rpt_cell_status_incl;  


  /* Reporting cell status as in 10.3.7.61 
  ** Optional field if reporting criteria is "Periodic reporting" or "No report" 
  ** otherwise not needed 
  */  
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status; 
  
  /* flag indicating if the inter-frequency set update is included. */
  boolean  inter_freq_set_update_incl;

  /* Inter-frequency set update info as defined in 10.3.7.22 of TS25.331 */
  tdsl1_inter_freq_set_update_struct_type  update;

  /* Boolean because this can be optional for MODIFY */
  boolean rpt_mode_incl;

  /* Measurement reporting mode as in 10.3.7.49  */
  tdsl1_inter_freq_meas_report_mode_enum_type   rpt_mode;

  union 
  {
    /* Event Triggered Measurement reporting criteria as in 10.3.7.39  
    ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT 
    */ 
    tdsl1_intra_freq_report_crit_struct_type    intra_rpt_crit;

    /* Event Triggered Measurement reporting criteria as in 10.3.7.19  
    ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT 
    */
    tdsl1_inter_freq_report_crit_struct_type    inter_rpt_crit;

    /* Periodical Measurement reporting criteria as in 10.3.7.53  
    ** Meaningful if "rpt_mode" is TDSL1_PERIODIC_RPT 
    */
    tdsl1_per_rpt_crit_struct_type periodic_crit;
  } u;
} tdsl1_inter_freq_meas_ctrl_struct_type;


/*
 * derived from 10.3.7.23  - Inter-RAT Cell Info List,  GSM Information
 */
typedef struct  
{
  /* Cell Individual Offset value 
  ** Value used to offset the measured quantity in dB, 
  ** range (-50..50) 
  */
  int16                           cio;

  /* Band, DCS 1800 or PCS 1900 */
  tdsl1_gsm_band_indicator_enum_type band_indicator;

  /* BSIC 10.3.8.2 */  
  tdsl1_gsm_bsic_struct_type         bsic;



  /* ARFCN  Integer (0..1023) */
  W_ARFCN_T                          bcch_arfcn;
} tdsl1_gsm_cell_info_type;


/*
 * derived from 10.3.7.23  - Inter-RAT cell info list,  GSM Cell Info
 */
typedef struct 
{
  /* Flag indicationg Cell Re-Selection Info SIB 11/12 info is included. */
  boolean cell_sel_info_incl;

  /* Cell Selection and Re-Selection Info SIB 11/12 */
  tdsl1_cell_sel_resel_info_struct_type  cell_sel_info;

  /* cell identification information */
  tdsl1_gsm_cell_info_type    gsm_info;

  /* Neighbor cell HCS information */
  tdsl1_nbr_cell_hcs_info_struct_type hcs_info;
} tdsl1_gsm_cell_sel_info_struct_type;


/*
 * 10.3.7.23  - Inter-RAT (Inter-Stystem) Cell Info List as in 10.3.7.23 of TS25.331
 */
typedef struct
{
  /* Remove inter-RAT cells */
  /* Number of inter-RAT cells removed 0=none, 1..TDSL1_MAX_CELL_MEAS
  ** TDSL1_REMOVE_ALL_CELLS(=0xFF)=remove all 
  */
  uint16       num_cell_rmv;

  struct 
  {
    /* Inter-RAT cell id, unique value in the range (0..<maxCellMeas-1>) */
    uint16    intersys_cell_id;
  } rmv_cell[ TDSL1_MAX_CELL_MEAS ];

  /* Add inter-system cells */
  /* Number of inter-system cells Added  */
  uint16       num_cell_add;

  struct
  {
    /* Cell Id, Unique value in the range (0..<maxCellMeas-1>) 
    ** Cell Id is an optional parameter for add cell, but L1 will require 
    ** RRC to select a Cell Id for use, so the choice will be made on 
    ** the UE in one place. 
    */
    uint16  intersys_cell_id;

    boolean measure_in_idle_pch_fach;

    tdsl1_rat_type_enum_type  cell_info_type;

    union
    {
      tdsl1_gsm_cell_sel_info_struct_type  gsm_cell;

      /* IS2000 unsupported */
      uint16  is2000_cell;
    } u;
  } add_cell[ TDSL1_MAX_CELL_MEAS ];

  /* If cells_for_meas_incl is set to TRUE, then cells   
  ** mentioned in this list should only be measured. For 
  ** SIB's, its always set to FALSE.  
  */
  boolean cells_for_meas_incl;

  /* Number of cells to measure */
  uint16 num_cell_meas;

  /* Only cells mentioned in this list should be measured. 
  ** This list contains the inter-system cell id            
  */  
  uint16 meas_cells_list[ TDSL1_MAX_CELL_MEAS ]; 

} tdsl1_inter_sys_cell_list_struct_type;


/*
 * 10.3.7.26  Inter-RAT Measured Results List, Measured Cells
 */
typedef struct
{
  /* Flag indicationg GSM Carrier RSSI info is included. */
  boolean gsm_carrier_rssi_incl;

  /* GSM Carrier RSSI */
  int16   gsm_carrier_rssi;

  /* BSIC Choice, Verified or Non-Verified */
  tdsl1_bsic_choice_enum_type bsic_choice;

  union
  {
    /* Verified BSIC Information */
    struct
    {
      /* Inter-RAT cell id  spec[0..(maxCellMeas-1)] */
      uint16  inter_sys_cell_id;
    } bsic_verified;

    /* Non-Verified BSIC Information */
    struct
    {
      /* BCCH ARFCN  spec[0..1023] */
      W_ARFCN_T bcch_arfcn;
    } bsic_not_verified;
  } u;

  #if 0
  /*Xiaoning note: the following 4 fields will be deleted*/

  /* Flag indicationg UTRA Carrier RSSI info is included. */
  boolean gsm_time_diff_incl;

  uint16  fn_offset;

  uint16  qbit_offset;

  /* observed time difference to GSM cell, GSM_TIME in 25.133 spec[0..4095]*/
  uint16  gsm_time_diff;
  #endif
} tdsl1_measured_cells_struct_type;

/*
 * 10.3.7.26  Inter-RAT Measured Results List
 */
typedef struct 
{
  /* number of inter-RAT measurement results included spec[1..maxOtherRAT], 0=>no info */
  uint16 num_msmt_results;

  struct
  {
    /* System Type, GSM is only supported choice currently */
    tdsl1_rat_enum_type system;
  
    union
    {
      /** GSM **/
      struct
      {
        /* Number of GSM Cells Measured */
        uint16 measured_gsm_cells;

        /* Measured Cells Information */
        tdsl1_measured_cells_struct_type measured_cells[TDSL1_MAX_REPORTED_GSM_CELLS];
      } gsm;
    } u;
  } inter_sys_msmt_results [TDSL1_MAX_OTHER_RAT];
} tdsl1_inter_sys_meas_result_struct_type;


/*
 * 10.3.7.28  - Inter-RAT Measured Results List
 */
typedef struct
{
  /* Inter-RAT Event Identity 3a,3b,3c,3d */
  tdsl1_inter_sys_event_enum_type  event_id;

  /* Inter-RAT number of cells to report spec[1..maxCellMeas], 0=>no info */
  uint16 num_cells;

  struct 
  {
    /* BSIC Verified or Non-Verified */
    tdsl1_bsic_choice_enum_type bsic_choice;
  
    union 
    {
      struct 
      {
        /* Inter-RAT Cell Id spec[0..(maxCellMeas-1)] */
        uint16 intersys_cell_id;
      } bsic_verified;

      struct 
      {
        /* BCCH ARFCH spec[0..1023] */
    W_ARFCN_T bcch_arfcn;
      } bsic_nonverified;
    } u;
  } cell [TDSL1_MAX_CELL_MEAS] ;
} tdsl1_inter_sys_event_results; 


/*
 * 10.3.7.*26*  - Inter-RAT Measured Results *plus* Measurement Event Results
 */
typedef struct 
{
  /* 10.3.7.28  Inter-rat  Event Results for 10.3.7.7, from msmt report msg 10.2.19 */
  tdsl1_inter_sys_event_results event_results;

  /* 10.3.7.26  Inter RAT Measurements */
  tdsl1_inter_sys_meas_result_struct_type measurements;
} tdsl1_inter_sys_meas_rpt_struct_type ;


/*
 * derived from 10.3.7.32  - Inter-RAT reporting quanity, CHOICE system:GSM
 */
/* TRUE for either of these Boolean types means that inclusion
** of the indicated field in the report is requested
*/
typedef struct
{  
  /*Xiaoning note: this field should be removed*/
  /* Observed time difference to GSM cell Reporting indicator */  
  boolean        time_diff;  

  /* GSM Carrier RSSI Reporting indicator */
  boolean        rssi_rpt_ind;  
} tdsl1_dch_gsm_rpt_quan_struct_type;


/*
 * 10.3.7.32  - Inter-RAT Reporting quantity
 */
typedef struct
{  
  /* Not used in this release should always be FALSE */  
  boolean                  utran_est_qual;    

  /* Currently only GSM is defined */  
  tdsl1_rat_type_enum_type             sys_choice;  
#ifdef FEATURE_TDSCDMA_TO_LTE
  union
  {
    tdsl1_eutra_meas_rpt_quantity_enum_type eutra_rpt_quan;
  }u;
#endif
  tdsl1_dch_gsm_rpt_quan_struct_type   gsm_rpt_quan;
} tdsl1_inter_sys_dch_rpt_quan_struct_type;

#ifdef FEATURE_TDSCDMA_TO_LTE
typedef struct 
{
  /* EUTRA measurement quantity*/
  tdsl1_eutra_meas_rpt_quantity_enum_type    meas_quan;
  /* Filter Coefficient: DEFAULT fc0 */
  tdsl1_eutra_filter_coef_enum_type      filter_coef;
} tdsl1_eutra_meas_quan_struct_type;
#endif

/*
 * 10.3.7.29  - Inter-RAT Measurement Quantity
 */
typedef struct 
{
  /* GSM_CARRIER_RSSI */
  tdsl1_gsm_meas_quantity_enum_type    meas_quan;

  /* Filter Coefficient */
  tdsl1_gsm_filter_coef_enum_type      filter_coef;

  /* True indicates BSIC Verification Required */
  boolean   bsic_ver_req;
} tdsl1_gsm_meas_quan_struct_type;


/*
 * 10.3.7.29  - Inter-system measurement quantity
 */
typedef struct
{
  /* Flag indicationg Intra-Freq Meas Quantity info is included. */
  boolean intra_meas_quan_incl;

  /* measurement quantity for UTRAN quality estimate */
  tdsl1_intra_freq_meas_quan_struct_type intra_meas_quan;

  /* Inter-system choice for measurements */
  tdsl1_rat_type_enum_type               system_choice; 

  /* Measurement quantity. Note that, for WCDMA FDD cells, we currently     
  ** support Ec/Io or RSCP Measurement Quantity for DCH Measurement report. 
  */
  union  
  {
    tdsl1_gsm_meas_quan_struct_type      gsm_meas_quan;
    uint16   is2000_meas_quan;
#ifdef FEATURE_TDSCDMA_TO_LTE
    tdsl1_eutra_meas_quan_struct_type    eutra_meas_quan;
#endif
  } u;
} tdsl1_inter_sys_meas_quan_struct_type;


/*
 * derived from 10.3.7.30  - Inter-RAT Measurement Report Criteria
 */
typedef struct
{
  /* Event ID */  
  tdsl1_inter_sys_event_enum_type  evt_id;  

  /* Threshhold for own system, range (-115..0). Only applies to event 3A. */  
  int16   own_thresh;   
  
  /* Used for event 3A, the actual value of W is a real number in the range
  ** (0.1..2.0) by 0.1 steps, value passed here is W*10  
  */  
  uint16   w;

  /* Threshhold for other system, range (-115..0). Only applies to events   
  ** 3A, 3B and 3C.   
  */    
  int16   other_thresh;   

  /* Hysteresis. 0..7.5 by .5 dB steps, value passed is Hysteresis*2 */   
  uint16  hyst;   

  /* Time duration for which the event triggering condition needs to   
  ** be fulfilled before the report can be sent, in ms.     
  */  
  uint16  time_to_trig;
  
  boolean   rpt_cell_status_incl;  

  /* Reporting cell status */  
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status;    
} tdsl1_inter_sys_event_crit_struct_type;


/*
 * 10.3.7.30  - Inter-Rat Measurement report criteria
 */
typedef struct
{ 
  uint16         num_event;  
  
  tdsl1_inter_sys_event_crit_struct_type    evt[TDSL1_MAX_MEAS_EVENT];
}  tdsl1_inter_sys_report_crit_struct_type;


/*
 * 10.3.7.27  - Inter-RAT Measurements, Report Criteria
 */
typedef struct
{
  /* CHOICE report criteria in 10.3.7.27 */
  tdsl1_meas_report_mode_enum_type  rpt_mode;  

  union
  {
    /* Event Triggered Measurement reporting criteria as in 10.3.7.30  
    ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT 
    */  
    tdsl1_inter_sys_report_crit_struct_type    evt_rpt_crit; 

    /* Periodical Measurement reporting criteria as in 10.3.7.53
    ** Meaningful if "rpt_mode" is TDSL1_PERIODIC_RPT 
    */      
    tdsl1_per_rpt_crit_struct_type periodic_crit;
  } u;
} tdsl1_inter_sys_reporting_criteria_struct_type;


/*
 * 10.3.7.27  Inter-RAT Measurement
 */
typedef struct
{
  /* flag to indicate if cell info is present  */
  boolean cell_list_incl;  

  /* cell information 10.3.7.23 */    
  tdsl1_inter_sys_cell_list_struct_type  cell_list;  

  /* flag to indicate if meas quantity is present  
  ** If true, the next field is valid 
  */
  boolean meas_quan_incl;

  /* Intra-frequency measurement quantity as in 10.3.7.29 of TS25.331 */    
  tdsl1_inter_sys_meas_quan_struct_type  meas_quan;

  /* flag indicating if the reporting quantities are given. */
  boolean rpt_quan_incl;

  /* Rpt quantity for Inter-RAT set as defined in 10.3.7.32 of TS25.331 */  
  tdsl1_inter_sys_dch_rpt_quan_struct_type   inter_sys_quan;  

  boolean rpt_cell_status_incl;  

  /* Reporting cell status as in 10.3.7.61 
  ** Optional field if reporting criteria is "Periodic reporting" or "No report" 
  ** otherwise not needed 
  */  
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status; 

  /* reporting criteria, periodic, event, and parameters */
  tdsl1_inter_sys_reporting_criteria_struct_type  rpt_criteria;
} tdsl1_inter_sys_meas_ctrl_struct_type;


/*
 * 10.3.7.33  - Intra-Frequency Cell Info List
 */
typedef struct
{
  /* Removed intra-freqency cells */
  /* Number of intra-frequency cells removed */  
  uint16       num_cell_rmv;
  
  struct 
  {
    /* L1 will del begin */
    /* Primary Scrambling Code  */
    uint16    psc;
    /* L1 will del end */
  
    /* L1 will add begin for TDD */

    /* Cell Parameter ID */
    boolean   cpi_incl;    

    uint16   cpi;
    /* L1 will add end for TDD */
  } rmv_cell[ TDSL1_MAX_CELL_MEAS ];
  
  /* New intra-frequency cells */
  /* Number of intra-frequency cells Added   */
  uint16       num_cell_add;
  
  struct  
  {
    /* L1 will del begin */
    /* Primary Scrambling Code  */
    uint16    psc;
    /* L1 will del end */
  
    /* L1 will add begin for TDD */
    /* Cell Parameter ID */
    boolean   cpi_incl;   

    uint16   cpi;

    /* L1 will add end for TDD */
    boolean measure_in_idle_pch_fach;
    
    tdsl1_cell_info_struct_type  cell_info;
  } add_cell[ TDSL1_MAX_CELL_MEAS ];
  
  /* If cells_for_meas_incl is set to TRUE, then cells mentioned in this list should
  ** only be measured. For SIB's, its always set to FALSE 
  */
  boolean cells_for_meas_incl;
  
  /* No of cells to measure */
  uint16 num_cell_meas;
  
  /* Only cells mentioned in this list should be measured. This list contains psc */
  uint16 meas_cells_list[ TDSL1_MAX_CELL_MEAS ];

} tdsl1_intra_freq_cell_list_struct_type;

/*
 * 10.3.7.35 Intra-frequency measured results list
 */
typedef struct 
{
  /* --change, intra-freq code to use rpt_crit in 10.2.19 struct 
  **  Describes the criterion of the  report Periodic/event triggered 
  */
  tdsl1_meas_report_mode_enum_type rpt_crit;

  /* flag indicating whether measured_results are included  
  ** If FALSE, ignore next two fields  
  */
  boolean cell_meas_result_incl;

  /* num of cells with measured results */
  uint16 num_cells ;

  /* measured results for each cell 10.3.7.3  */   
  tdsl1_intra_meas_rpt_cell_info_struct_type cell[TDSL1_MAX_CELL_MEAS];

  /* The event that triggered this report. Applicable only if
  ** the rpt_crit is TDSL1_EVENT_TRIGGER_RPT 
  */  
  uint16 evt_id ;

  uint16 num_cells_for_evt;

/* L1 will del begin */
  uint16 cell_psc[TDSL1_MAX_CELL_MEAS];
/* L1 will del end */

/* L1 will add begin */
  uint16 cpi[TDSL1_MAX_CELL_MEAS];
/* L1 will add end */
} tdsl1_intra_freq_meas_rpt_struct_type ;

/*
 * Intra-frequency info 10.3.7.36
 */
typedef struct
{
  /* flag to indicate if cell info is present  */
  boolean cell_list_incl;

  /* cell information 10.3.7.33 */
  tdsl1_intra_freq_cell_list_struct_type  cell_list;

  /* flag to indicate if meas quantity is present
  ** If true, the next two fields are valid       
  */  
  boolean meas_quan_incl ;

  /* Intra-frequency measurement quantity as in 10.3.7.38 of TS25.331 */
  tdsl1_intra_freq_meas_quan_struct_type  meas_quan;

  /* flag indicating if the reporting quantities are given. 
  ** If yes, next four fields are valid 
  ** Intra-frequency reporting quantity as defined in 10.3.7.5 of TS25.331 
  */
  boolean intra_f_rpt_quan_incl ;

  /* Rpt quantity for active set as defined in 10.3.7.41 of TS25.331 */
  tdsl1_intra_dch_rpt_quan_struct_type    aset_quan;

  /* Rpt quantity for monitored set as defined in 10.3.7.41 of TS25.331 */
  tdsl1_intra_dch_rpt_quan_struct_type    mset_quan;

  boolean       dset_quan_incl;

  /* Rpt quantity for detected set as defined in 10.3.7.41 of TS25.331 */
  tdsl1_intra_dch_rpt_quan_struct_type    dset_quan;

  /* Measurement reporting mode as in 10.3.7.49  */
  tdsl1_meas_report_mode_enum_type   rpt_mode;

  /* Event Triggered Measurement reporting criteria as in 10.3.7.39  
  ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT 
  */
  tdsl1_intra_freq_report_crit_struct_type    rpt_crit;

  /* Measurement is "released" after the indicated amount (number) of 
  ** reporting(s) from the UE itself. 0 means "infinity".   
  */
  uint16  num_rpt;

  /* Interval of periodical reporting, in ms. */
  uint16  rpt_intvl;  
  
  boolean rpt_cell_status_incl;

  /* Reporting cell status. Valid only if reporting criteria is
  ** "Periodic reporting" or "No report"  
  */
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status;
} tdsl1_intra_freq_meas_ctrl_struct_type;

/*
 * 10.3.7.42  - Intra-frequency reporting quantity for RACH reporting
 */
typedef struct
{
  /* SFN-SFN observed time difference */
  tdsl1_sfn_report_enum_type    rach_sfn_tm_delta;

  /* L1 will del begin */
  /* RACH Reporting Quantity. Note that, for WCDMA FDD cells, we currently 
  ** support Ec/Io or RSCP Measurement Quantity for DCH Measurement report.
  */
  tdsl1_meas_quan_enum_type     rach_rpt_quan;
  /* L1 will del end */
  
  /* L1 will add begin */
  uint8                           rach_meas_quan_list_no;
  
  /* RACH Reporting Quantity. Note that, for TDSCDMA TDD cells, we currently 
  ** support Iscp or RSCP Measurement Quantity for DCH Measurement report.
  */
  tdsl1_meas_quan_enum_type   rach_rpt_quan_list[L1_RACH_MAX_MEAS_LIST_NO];
  /* L1 will add end */
} tdsl1_intra_freq_rach_rpt_quan_struct_type;

#if 0
/*10.3.7.42  - Intra-frequency reporting quantity for RACH reporting*/
typedef struct
{
  tdsl1_sfn_report_enum_type    rach_sfn_tm_delta;
    /* SFN-SFN observed time difference */
  uint8 rach_meas_quan_list_no;
  tdsl1_meas_quan_enum_type     rach_rpt_quan_list[L1_RACH_MAX_MEAS_LIST_NO];
    /* RACH Reporting Quantity. Note that, for WCDMA FDD cells, we currently 
       support Ec/Io or RSCP Measurement Quantity for DCH Measurement report.
     */
} tdsl1_intra_freq_rach_rpt_quan_struct_type;
#endif
/*
 * 10.3.7.62  - Reporting information in state CELL_DCH
 */
typedef struct
{
  /* Intra-frequency reporting quantity as defined in 10.3.7.5 of TS25.331 */
  tdsl1_intra_dch_rpt_quan_struct_type    aset_quan;
  
  /* Rpt quantity for active set as defined in 10.3.7.41 of TS25.331 */
  tdsl1_intra_dch_rpt_quan_struct_type    mset_quan;
  
  /* Rpt quantity for monitored set as defined in 10.3.7.41 of TS25.331 */
  boolean       dset_quan_incl;

  /* Rpt quantity for detected set as defined in 10.3.7.41 of TS25.331 */ 
  tdsl1_intra_dch_rpt_quan_struct_type    dset_quan;

  /* Measurement reporting mode as in 10.3.7.49  */
  tdsl1_meas_report_mode_enum_type   rpt_mode;

  /* Measuremnt Report transfer criteria to UTRAN */
  tdsl1_meas_report_trans_mode_enum_type  rpt_trans_mode;

  /*Event Triggered Measurement reporting criteria as in 10.3.7.39
  ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT
  */
  tdsl1_intra_freq_report_crit_struct_type    rpt_crit;
  
   /* --change, intra-freq to use
   ** Periodical Measurement reporting criteria as in 10.3.7.53
   ** Meaningful if "rpt_mode" is TDSL1_PERIODIC_RPT
   */

  /* Measurement is "released" after the indicated amount (number) of
  ** reporting(s) from the UE itself. 0 means "infinity".  
  */   
  uint16  num_rpt;

  /* Interval of periodical reporting, in ms. 0 means no periodical reporting.*/
  uint16  rpt_intvl;
} tdsl1_intra_freq_dch_rpt_info_struct_type;


/*
 * 10.3.7.40  - Intra-frequency measurement system information
 */
typedef struct
{
  /* Intra-frequency measurement identity number, default as 1 */
  uint16       meas_id;

  /* Flag indicating whether the cell list is included  */
  boolean cell_list_incl;

  /* Intra-Frequency Cell Info List as in 10.3.7.33 of TS25.331 */
  tdsl1_intra_freq_cell_list_struct_type  cell_list;

  /* Flag indicating whether the measurement quantity is included  */    
  boolean meas_quan_incl;

  /* Intra-frequency measurement quantity as in 10.3.7.38 of TS25.331 */  
  tdsl1_intra_freq_meas_quan_struct_type  meas_quan;

  /* Flag indicating whether the RACH reporting quantity is included  */
  boolean rach_rpt_quan_incl;

  /* Intra-frequency reporting quantity for RACH reporting as in 10.3.7.42 */
  tdsl1_intra_freq_rach_rpt_quan_struct_type  rach_rpt_quan;

  /* Flag indicating whether the max no of reported cells on RACH info is included */
  boolean rach_rpt_max_incl;

  /* Maximum number of reported cells on RACH as in 10.3.7.43 */
  tdsl1_intra_freq_rach_rpt_max_enum_type    rach_rpt_max;

  /* Reporting info for CELL_DCH included  */
  boolean   intra_f_dch_rpt_info_incl;

  /* Reporting information in state CELL_DCH, as in 10.3.7.62 */
  tdsl1_intra_freq_dch_rpt_info_struct_type  dch_rpt_info;
} tdsl1_intra_freq_sib_struct_type;

/* Bler per transport channel */
typedef struct
{
  /* Tr Channel Identifier 1..32 */
  uint8 transport_ch_identity;

  /* Has bler been measured on this transport channel.
  ** If false ignore the next field 
  */
  boolean bler_present;

  /* BLER Value 0..63 used only for periodic reports*/
  uint8 bler_value;
} tdsl1_transport_channel_bler_struct_type;

/*
 * 10.3.7.55  - Quality Measured Results List
 */

/* Using the same structure for both periodic and event reporting. For event
** reporting the no_of_tr_ch is set as 1 and only one channel is included in 
** the array 
*/
/* L1 will add begin */
typedef struct 
{
 /* No of Transport Channels 1..32 for both event triggered and periodic reports*/
  uint8 tfcs_id;
 
  uint8 no_ts;
  
  uint8 sir[L1_MAX_TS];
} l1_sir_result_type;
/* L1 will add end */

typedef struct 
{
 /* No of Transport Channels 1..32 for both event triggered and periodic reports*/
  uint8 no_of_tr_ch;
  
  tdsl1_transport_channel_bler_struct_type l1_trch_bler[TDSL1_MAX_TRANSPORT_CHANNELS];
  
/* L1 will add begin */
  uint8 no_of_cctrch;

  l1_sir_result_type l1_sir_result[L1_MAX_CCTRCH];
/* L1 will add end */
} tdsl1_qual_meas_rpt_struct_type;

/* For qual_meas additional measurements */

/* Event Triggered Cirteria for Quality Measurements */
typedef struct
{
  /* 1 ..32 */
  uint8 no_of_transport_channels;

  struct
  {
    /* Tranpsort Channel Identifier */
    uint8 tr_ch_id;

    /* Total No of CRC's before 5A could be generated 1..512 */
    uint16 total_crc;

    /* BAD CRC's during the window, 1..512 */
    uint16 bad_crc;

    /* Pending time after trigger CRC's, 1..512 */
    uint16 pending_time_after_trigger;
  } tr_ch[TDSL1_MAX_TRANSPORT_CHANNELS];
} tdsl1_qual_event_crit_struct_type;

/*
 * 10.3.7.56  - Quality Measurement
 */
typedef struct
{
  /* Meas Quantity is always BLER. No need to indicate explicitly 
  ** Reporting Quantity to be included. 
  */
  boolean rpt_quan_included;

  /* BLER reporting is requested? If this is yes, next two fields are Mandatory */
  boolean bler_rpt_requested;
  
  /* True specifies that BLER to be reported for all DL transport channels. If set to TRUE
  ** Ignore the next two 
  */
  boolean rpt_all_transport_ch;

  /* No of Transport Channels. 1..32 */
  uint8 no_of_tr_ch;

  uint8 reported_transport_channels[TDSL1_MAX_TRANSPORT_CHANNELS];

  /* 10.3.7.59, L1 will add begin */
  uint8 no_of_cctrch;
  
  uint8 tfcs_id[L1_MAX_CCTRCH];
  /* 10.3.7.59, L1 will add end */
  
  /* Reporting Mode */
  tdsl1_meas_report_mode_enum_type rpt_mode;

  union 

  {
    /* Event Triggered Measurement reporting criteria as in 10.3.7.80 
    ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT 
    */
    tdsl1_qual_event_crit_struct_type    evt_trig_crit;

    /* Periodical Measurement reporting criteria as in 10.3.7.53  
    ** Meaningful if "rpt_mode" is TDSL1_PERIODIC_RPT 
    */  
    tdsl1_per_rpt_crit_struct_type periodic_crit;
  } u;
} tdsl1_qual_meas_ctrl_struct_type;
/*
 * 10.3.7.67  - Traffic Volume Measured Results List
 */
typedef struct 
{
    /* djm - to be finished */
    /* Traffic Volume Measurement Results spec[1..maxRB], 0=>no info */
    uint16 num_msmt_results;

    struct 
    {
      /* RB Identity 10.3.4.16  spec[1..32] */
      uint16 rb_id;

      /* op - RLC Bufferes Payload, in bytes */
      tdsl1_num_bytes_enum_type  rlc_buffers_payload;

      /* op - Average of RLC Buffer Payload */
      tdsl1_num_bytes_enum_type  average_rlc_buffers_payload;

      /* op - Variance of RLC Buffer Payload */
      tdsl1_num_bytes_enum_type  variance_rlc_buffers_payload;
    } msmt_info [TDSL1_MAX_RB] ;
} tdsl1_traf_vol_meas_rpt_struct_type ;


/*
 * 10.3.7.67  - Traffic Volume Measured Results List
 */
typedef struct 
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_traf_vol_addl_meas_rpt_struct_type;
/* For traf_vol additional measurements */


/*
 * 10.3.7.68  - Traffic Volume Measurement
 */
typedef struct
{
  /********** To Be completed *************/
  uint16 dummy;
} tdsl1_traf_vol_meas_ctrl_struct_type;


/* L1 will del begin */
typedef struct
{
  /* Primary CPICH info (0...511) */
  uint16 pri_scr_code;

  /* 10.3.7.83 Range (768...1280) */
  uint16 rx_tx_diff_type_1;
} tdsl1_internal_meas_rx_tx_rpt_struct_type;

/* L1 will del end */

typedef struct
{
  /* UE Tx power info included ? */
  boolean ue_tx_pwr_included;
  
  /* L1 will del begin */
  /* Range (0...85) according to 25.133*/
  uint16  ue_tx_pwr;
  
  /* Rx-Tx diff info included */
  boolean ue_rx_tx_time_diff_included;
  
  /* Num RL for which Rx-Tx diff info included */
  uint16  num_rl;
  
  tdsl1_internal_meas_rx_tx_rpt_struct_type rx_tx_rpt[TDSL1_MAX_RL];
  /* L1 will del end */
  
  /* L1 will add begin */
  /* 10.3.7.85, UE Transmitted Power, Range [0...104]*/
  uint8   no_pwr;

  uint16  ue_tx_pwr_list[TDSL1_UL_MAX_NUM_TS];

  /* 10.3.7.112, TADV */
  boolean t_adv_included;

  uint16  t_adv;

  uint16  sfn;
  /* L1 will add end */
  
} tdsl1_internal_cell_measured_results_struct_type;

/*
 * 10.3.7.76  - UE Internal Measured Results
 */
typedef struct 
{
  /* Indicates whether cell measured results included */
  boolean cell_measured_results_included;

  tdsl1_internal_cell_measured_results_struct_type cell_measured_result_info;

  /* The event that triggered this report. Applicable only if 
  ** the rpt_crit is TDSL1_EVENT_TRIGGER_RPT
  ** Only valid if report is for event 6f/6g 
  */
  tdsl1_internal_meas_event_enum_type evt_id ;
  
  /* L1 will del begin */
  uint16 cell_psc;
  /* L1 will del end */
  
  /* L1 will add begin */
  uint16 cpi;
  /* L1 will add end */
} tdsl1_ue_internal_meas_rpt_struct_type;
/* For ue_internal additional measurements */


typedef enum
{
  TDSL1_UE_TX_PWR,
  TDSL1_UTRA_RSSI,
  /*TDSL1_RX_TX_DIFF should be deleted*/
  TDSL1_RX_TX_DIFF,
  /*add TDSL1_TADV for 1.28 TDD*/
  TDSL1_TADV,
  TDSL1_MEAS_NO_MEAS
} tdsl1_internal_meas_quan_type;

/*  10.3.7.82 */
typedef struct
{
  boolean ue_tx_pwr_rpt;

/* L1 will del begin */
  boolean ue_rx_tx_diff_rpt;
/* L1 will del ends */

/* L1 will add begin */
  boolean ue_t_adv;
/* L1 will add end */
} tdsl1_internal_meas_rpt_quan_info_struct_type;

/* 10.3.7.80 */
typedef struct
{
  /* event id */
  tdsl1_internal_meas_event_enum_type event_id;

  /*  Time To Trigger */
  uint16 ttt;

  /* Tx power threshold dbm units. ( -50 ... +33) valid for 6A/6B */
  int16 ue_tx_pwr_thresh;

  /* L1 will del begin */
  /* Rx-Tx time diff thresh. in chips ( 768...1280) valid for 6F/6G */
  uint16 rx_tx_diff_thresh;
  /* L1 will del end */
  
  /* L1 will add begin */
  /* Time difference ( 0...504). In event 6F/6G 
  ** 10.3.7.80, real (0..63), step 0.125, RRC send L1 is (0, 504), L1 will translate to real value 
  */
  uint16 t_adv_thresh;
  /* L1 will add end */
} tdsl1_internal_meas_evt_crit_struct_type;

typedef struct
{
  /* Num events */
  uint16 num_event;

  tdsl1_internal_meas_evt_crit_struct_type evt[TDSL1_MAX_MEAS_EVENT];
}tdsl1_internal_meas_rpt_crit_struct_type;

/* Internal measurements SIB info */
typedef struct 
{
  /* measurement id */
  uint16 meas_id ;

  /* Measurement quantity 10.3.7.79 */
  tdsl1_internal_meas_quan_type meas_quan;
}tdsl1_internal_meas_sib_struct_type;


/*
 * 10.3.7.77  - UE Internal Measurement
 */
typedef struct
{
  /* If this is yes, next two fields are Mandatory */
  boolean meas_quan_included;

  /* Measurement quantity 10.3.7.79 */
  tdsl1_internal_meas_quan_type meas_quan;

  boolean filter_coeff_incl;

  tdsl1_wcdma_filter_coef_enum_type filt_idx;

  boolean rpt_quan_included;

  /* 10.3.7.82 */
  tdsl1_internal_meas_rpt_quan_info_struct_type rpt_quan;

  tdsl1_meas_report_mode_enum_type  rpt_mode;

  union 
  {
    /* Event Triggered Measurement reporting criteria as in 10.3.7.80  
    ** Meaningful if "rpt_mode" is TDSL1_EVENT_TRIGGER_RPT 
    */
    tdsl1_internal_meas_rpt_crit_struct_type    evt_trig_crit;

    /* Periodical Measurement reporting criteria as in 10.3.7.53  
    ** Meaningful if "rpt_mode" is TDSL1_PERIODIC_RPT 
    */  
    tdsl1_per_rpt_crit_struct_type periodic_crit;  
  } u;
} tdsl1_ue_internal_meas_ctrl_struct_type;


/*
 * 10.3.7.99  - UE Positioning Measured Results
 */
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_ue_positioning_measured_results_type;


/*
 * 10.3.7.100  - UE Positioning Measurement
 */
typedef struct
{
  /********** To Be completed *************/
  uint16 dummy;
} tdsl1_ue_pos_meas_ctrl_struct_type;


/*
 * 10.3.7.44  Measured Results
 */
typedef struct
{
  /* Describes the type of measurement  */
  tdsl1_meas_type_enum_type  meas_type;

  union 
  {
    /* Intra-frequency measurement report info 10.3.7.35  */
    tdsl1_intra_freq_meas_rpt_struct_type intra_f_rpt ;

    /* Inter-frequency measurement report info 10.3.7.15  */
    tdsl1_inter_freq_meas_rpt_struct_type inter_f_rpt ;

    /* Inter-RAT measurement report info       10.3.7.26 */
    tdsl1_inter_sys_meas_rpt_struct_type inter_sys_rpt ;

    /* Traffic volume measurement report info  10.3.7.67 */
    tdsl1_traf_vol_meas_rpt_struct_type traf_vol_rpt ;

    /* Quality measurement report info         10.3.7.55 */
    tdsl1_qual_meas_rpt_struct_type qual_meas_rpt ;

    /* UE internal measurement report info     10.3.7.76 */
    tdsl1_ue_internal_meas_rpt_struct_type ue_internal_meas_rpt;

    /* UE positioning measured results list    10.3.7.99 */
    tdsl1_ue_positioning_measured_results_type ue_pos_meas_results;
  } u;
} tdsl1_measured_results_struct_type;



/**** 10.2... ****/

/*
 * Measurement Parameters from SIB_11/12, as in 10.2.48.8.14 and 
 * 10.2.48.8.15 of TS25.331.
 */
/* DJM - THIS DOESN'T seem correct... should pull Measurement Control System Info Struct
 * from 10.3.7.47...
 * and then need Use of HCS enum, Cell selection and reselection quality measure
 * and then all of the measurement information
 *
 * need SIB 12 indicator
 *      10.3.7.8  FACH measurement occasion info
 *      10.3.7.47 Measurement Control System Information
 *         use of HCS (used, not used)
 *         cell selection and reselection quality measure (CPICH Ec/No, CPICH RSCP)
 *         10.3.7.40  Intra-Freq
 *         10.3.7.20  Inter-Freq
 *         10.3.7.31  Inter-RAT
 *         10.3.7.73  Traffic Volume Measurement System Info
 *         10.3.7.81  UE Internal System Measurement Info
 *
 */
typedef struct
{
  /* FACH Measurement occasion info included if yes,10.3.7.8 */
  boolean       fach_meas_incl;

  /* FACH measurement info */
  tdsl1_fach_meas_info_struct_type   fach;

  /* UTRAN DRX cycle length coefficient, valid only if fach_meas_incl is true */  
  uint32        k_utra;

  /* 10.3.7.47  measurement cotrol system information  
  ** Flag indicating if serving cell belongs to a HCS structure 
  */  
  boolean       hcs_used;

  /* Choice of measurement to use as quality measure Q */
  tdsl1_meas_quan_enum_type  cell_sel_resel_quan_type;

  /* Flag indicating if intra-frequency measurement info is included */
  boolean      intra_freq_meas_info_incl;

  /* Intra-frequency measurement system information */
  tdsl1_intra_freq_sib_struct_type intra_f;

  
  /* Inter freq info is removed from SIB part of MEASUREMENT_REQ.
  ** A new command CPHY_INTER_FREQ_MEAS_REQ will contain the info 
  ** Flag indicating if inter-system measurement info is included 
  */ 
  boolean  inter_sys_meas_info_incl;

  /* Inter-system cell information, as defined in 10.3.7.31 */    
  tdsl1_inter_sys_cell_list_struct_type   inter_s;

  /******** Other measurement information. See 10.3.7.72  ******************/
  /********           To be completed                     ******************/
} tdsl1_meas_sib_parm_struct_type;

/*
 * T2L PS HO:  EUTRA
*/
#ifdef FEATURE_TDSCDMA_TO_LTE

/* Ref: 3GPP TS 25.331 version 8.8.0 Release 8 - Section: 10.3.7.115  
** Measurement bandwidth information common for all neighbouring cells on the carrier
** frequency. It is defined by the parameter Transmission Bandwidth Configuration, NRB
** [36.104]. The values indicate the number of resource blocks over which the UE could 
** measure. Default value is 6.   
*/
typedef enum
{
  TDSL1_MBW6,
  TDSL1_MBW15,
  TDSL1_MBW25,
  TDSL1_MBW50,
  TDSL1_MBW75,
  TDSL1_MBW100,
  TDSL1_MBW_INVALID
} tdsl1_eutra_meas_bandwidth_type;

/* Struct to store the LTE priority info per EARFCN received from SIB19/3rd MEAS_REQ */
typedef struct
{
  /* EARFCN of the downlink carrier frequency Range: (0..65535)*/
  uint32 EARFCN;  
  
  /* Priority of the LTE freq, Range: (0..7) */
  int8 priority;
  
  /* Threshold to be used when calculating suitability criteria for that LTE EARFCN 
  ** Range: 2*(-70 to -22) 
  */
  int16 q_rxlevmin_eutra; 
  
  /* Max threshold value RSRP, dB
  ** Range: 2*(0 to 31) 
  */
  uint8 threshx_high; 
  
  /* Min threshold value RSRP, Range: 2*(0 to 31) dB*/
  uint8 threshx_low;  

  /*add new threshold_2*/
  /* Threshold to be used when calculating suitability criteria for that LTE EARFCN */
  /* as per spec RSRQ, dB default value is negative infinity, L1 should treat 0xFF as default */
  int16 q_qualmin_eutra;   //
  boolean high2_incl;    // whether threhold_high2 is included or not.
  uint8 threshx_high2;   //Threshold_high2. 
  boolean low2_incl;
  uint8 threshx_low2;  
} tdsl1_inter_rat_eutra_priority_info_type;

/* struct to store the LTE blacklisted cell id's and Bandwidth per EARFCN */
typedef struct
{
  /* EARFCN of the downlink carrier frequency 
  ** Range: (0..65535) 
  */
  uint32 EARFCN;  
  
  /* See tdsl1_eutra_meas_bandwidth_type comments*/
  tdsl1_eutra_meas_bandwidth_type meas_band_width;

  /* Number of blacklisted cell for that LTE EARFCN */
  uint8 num_blacklisted_cells;

  /* List of cell ids that are blacklised from LTE EARFCN */
  uint16 blacklisted_cells[TDSL1_MAX_EUTRA_CELLS_PER_FREQUENCY];
} tdsl1_eutra_frequency_info_type;

/* Main LTE Freq level struct */
typedef struct
{
  /* Number of LTE frequencies recieved from SIB19 */
  uint8 num_eutra_frequencies;

  /* List of LTE frequencies, look at tdsl1_eutra_frequency_info_type for more details */
  tdsl1_eutra_frequency_info_type eutra_frequency[TDSL1_MAX_EUTRA_FREQ];
} tdsl1_meas_eutra_frequency_list_type;

/* Main LTE cell level struct per Freq */
typedef struct
{
  /* EARFCN of the downlink carrier frequency */
  uint32 EARFCN;   

  /* Cell id of a cell that is from LTE EARFCN */
  uint16 physical_cell_identity; /*Range 0-503*/

  /* See tdsl1_eutra_meas_bandwidth_type comments*/
  tdsl1_eutra_meas_bandwidth_type band_width;
} tdsl1_eutra_cell_info_type;

/* LTE information passed to RRC in CEL RESEL IND */
typedef struct
{
  /* E-UTRA Cell Reselection Cell Information */
  tdsl1_eutra_cell_info_type    cell_info;    

  /* Cell selection measurement made by SRCH, to be reported to RRC */  
  int16                    rxlev_meas;  
} tdsl1_eutra_cell_resel_ind_info_type;

typedef struct
{
  uint16       num_freq_rmv;
  /* Number of EUTRA freqs removed 0=none, 1..L1_MAX_EUTRA_FREQ */
  /* L1_REMOVE_ALL_FREQ(=0xFF)=remove all */
  uint32  rmv_freq[ TDSL1_MAX_EUTRA_FREQ ];

  tdsl1_meas_eutra_frequency_list_type add_freq_list;
}tdsl1_inter_sys_eutra_freq_list_struct_type;

#ifdef FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT
/* * 10.3.7.12a  - Idle Interval Information */
typedef struct{
/* the coefficient parameter to calculate the idle interval period. */
/* Default value is 2. The actual idle interval period equal to 2^k radio frames. */
  uint8                 k;

/* The idle interval position in the period.Default value is 0. */
  uint8                 Offset;
} tdsl1_inter_sys_idle_interval_info_type;
#endif

typedef struct
{
  boolean eutra_freq_list_incl;  /* E-UTRA freq list info */
  tdsl1_inter_sys_eutra_freq_list_struct_type eutra_freq_list;

  boolean eutra_meas_quantity_incl;  /* E-UTRA measurement quantity 8.6.7.5 */
  tdsl1_inter_sys_meas_quan_struct_type  meas_quan;

  boolean eutra_rpt_quantity_incl;  /* E-UTRA measurement quantity 8.6.7.6 */
  tdsl1_inter_sys_dch_rpt_quan_struct_type rpt_quan;

  boolean rpt_cell_status_incl;
  tdsl1_meas_rpt_cell_status_struct_type  rpt_cell_status;

  tdsl1_inter_sys_reporting_criteria_struct_type  rpt_criteria;

#ifdef FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT
    /* IdleIntervalInfo OPTIONAL */
  boolean idle_interval_info_incl;

  tdsl1_inter_sys_idle_interval_info_type  idle_interval_info;
#endif

}tdsl1_inter_sys_eutra_meas_ctrl_struct_type;

#endif

/*
 * 10.2.17  Measurement Control Message, CHOICE Measurement Type
 */
typedef struct
{
  tdsl1_meas_type_enum_type  meas_type;
  union
  {
    tdsl1_intra_freq_meas_ctrl_struct_type  intra_f;
    tdsl1_inter_freq_meas_ctrl_struct_type  inter_f;
    tdsl1_inter_sys_meas_ctrl_struct_type   inter_s;
#ifdef FEATURE_TDSCDMA_TO_LTE
    tdsl1_inter_sys_eutra_meas_ctrl_struct_type  inter_s_eutra;
#endif
    tdsl1_ue_pos_meas_ctrl_struct_type      ue_pos;
    tdsl1_traf_vol_meas_ctrl_struct_type    traf_v;
    tdsl1_qual_meas_ctrl_struct_type        qual;
    tdsl1_ue_internal_meas_ctrl_struct_type ue_internal;
  } u;
} tdsl1_meas_ctrl_union_struct_type;
#ifdef FEATURE_TDSCDMA_DMO_SUPPORT
/* * 10.3.7.27  Inter-RAT Measurement */
/*  * 10.3.7.126  - CELL_DCH measurement occasion info LCR  */
#define TDSL1_MAX_OCCASION_PATTERN    5

typedef enum{
  TDSL1_OCCASION_PATTERN_ACTIVE,
  TDSL1_OCCASION_PATTERN_DEACTIVE
}tdsl1_occa_pattern_status_enum_type;

/* For Timeslot Bitmap: Bit string (7), Bitmap indicating which of the timeslot(s) is/are allocated for measurement. 
        Bit 0 is for timeslot 0.
        Bit 1 is for timeslot 1.
        Bit 2 is for timeslot 2.
        Bit 3 is for timeslot 3.
        Bit 4 is for timeslot 4.
        Bit 5 is for timeslot 5.
        Bit 6 is for timeslot 6.
        The value 0 of a bit means the corresponding timeslot is not used for measurement.
        The value 1 of a bit means the corresponding timeslot is used for measurement.
        Bit 0 is the first/leftmost bit of the bit string. */
#define      TDS_DMO_timeslot_bitmap_timeslot0    0x80
#define      TDS_DMO_timeslot_bitmap_timeslot1    0x40
#define      TDS_DMO_timeslot_bitmap_timeslot2    0x20
#define      TDS_DMO_timeslot_bitmap_timeslot3    0x10
#define      TDS_DMO_timeslot_bitmap_timeslot4    0x08
#define      TDS_DMO_timeslot_bitmap_timeslot5    0x04
#define      TDS_DMO_timeslot_bitmap_timeslot6    0x02

typedef struct{
  /* CELL_DCH measurement occasion cycle length coefficient. 1~9 */
  uint8                              k;
  
  /* In frames. The measurement occasion position in the measurement period. 0~511 */
  uint16                             Offset;
  
  /* The measurement occasion length in frames starting from the Offset. 1~512 */
  uint16                             m_length;
  
  /* Bitmap indicating which of the timeslot(s) is/are allocated for measurement. Bit string (7) */
  /* Bit 0 is the first/leftmost bit of the bit string. */
  uint8                              timeslot_bitmap;
}tdsl1_occasion_pattern_seq_para_type;

/* For measurement_purpose: measurementPurpose BIT STRING (SIZE (5)) OPTIONAL */
/* Bit 0 is for Inter-frequency measurement. */
/* Bit 1 is for GSM carrier RSSI measurement. */
/* Bit 2 is for Initial BSIC identification. */
/* Bit 3 is for BSIC reconfirmation. */
/* Bit 4 is for E-UTRA measurement. */
/* The value 1 of a bit means that the measurement occasion pattern 
       sequence is applicable for the corresponding type of measurement.
       Bit 0 is the first/leftmost bit of the bit string. */
#define      TDS_DMO_meas_purpose_Inter_frequency_measurement    0x80
#define      TDS_DMO_meas_purpose_GSM_carrier_RSSI_measurement   0x40
#define      TDS_DMO_meas_purpose_Initial_RSSI_identification    0x20
#define      TDS_DMO_meas_purpose_BSIC_reconfirmation            0x10
#define      TDS_DMO_meas_purpose_EUTRA_measurement              0x08

typedef struct
{
  /* measurement-Occasion-Coeff INTEGER (1..5) */
  uint8                    occasion_pattern_number;
  struct
  {
    /* patternIdentifier INTEGER(0..maxMeasOccasionPattern-1), 0~4 */
    uint8                  pattern_sequence_identifier;

    /* activate, deactivate */
    tdsl1_occa_pattern_status_enum_type      status_flag;

    boolean                     measurement_purpose_incl;

    /* measurementPurpose BIT STRING (SIZE (5)) OPTIONAL */
    uint8                       measurement_purpose;

    /* measurementOccasionPatternParameter MeasurementOccasionPatternParameter OPTIONAL */
    boolean                  occasion_pattern_seq_para_incl;
    tdsl1_occasion_pattern_seq_para_type  occasion_pattern_seq_para;
  } occasion_pattern[TDSL1_MAX_OCCASION_PATTERN];
} tdsl1_meas_occasion_info_struct_type;
#endif


/*
 * 10.2.17  Measurement Control Message
 */
typedef struct
{
  /* If trans_id is TDSL1_NO_CNF_TRANSACTION_ID, RRC doesn't need a confirmation for the MCM sent */
  uint32  trans_id;
    
  /* Measurement Identity Number */
  uint16  meas_id;
    
  /* Measurement Command 10.3.7.46 */
  tdsl1_meas_cmd_enum_type  meas_cmd;
  

  /* DJM - this block is not needed by L1.  Periodic vs/ Event is found below 
  **       in the meas_object.rpt_criteria 
  **       the AM vs UM RLC is an RRC bit of info, L1 doesn't need 
  */
  boolean  rpt_mode_incl;
  
  /* flag indicating if reporting mode has been included 
  ** If TRUE, then the following variable is valid       
  */
  tdsl1_meas_reporting_mode_struct_type  reporting_mode;
  
  /* periodic or event based, AM or UM RLC transfer */

  /* --change, intra-freq/inter freq  to make use of the reporting_mode above, 
  ** remove use of these, event based, or periodic. 
  */
  tdsl1_meas_report_mode_enum_type        rpt_mode;

  /* measurement transfer mode */
  tdsl1_meas_report_trans_mode_enum_type  rpt_trans_mode;

  /* Number of additional measurements */
  uint16  num_add_meas;

  /* 10.3.7.48 Measurement Identity for each addl measurement */
  uint16  add_meas[ TDSL1_MAX_CELL_MEAS ];

  /*  A boolean to check if the following info is included   
  **  It is mandatory if the meas_cmd is "setup", optional 
  **  if it is "modify" and not needed if it is "release"  
  */
  boolean  meas_object_info_included;
  
  /* Measurement object  */  
  tdsl1_meas_ctrl_union_struct_type  meas_object;
  
#ifdef FEATURE_TDSCDMA_DMO_SUPPORT
/* cellDCHMeasOccasionInfo-TDD128 CellDCHMeasOccasionInfo-TDD128-r9 OPTIONAL */
  boolean                        meas_occasion_info_incl;
  tdsl1_meas_occasion_info_struct_type    meas_occasion_info;
#endif
} tdsl1_meas_ctrl_parm_struct_type;

#define TDSCLEAR_CELL_INFO_LIST   0x01
#define TDSCLEAR_MEAS_IDENTITY    0x02

/* Empty  Measurement Control Message*/
typedef struct
{
  tdsl1_intra_freq_meas_quan_struct_type meas_quan;
  uint8  l1_deferred_meas_from_sib_action;
} tdsl1_deferred_meas_from_sib_struct_type;

/***************************************************************************
**                                                                       
**     L1 - RRC   Non-Specification Related Sub-Structure Types                   
**                                                                       
***************************************************************************/

/*
 * Frequency Scan Parameters Structures, Raw & Fine
 */
/* Frequency Scan, raw scan parameters */
typedef struct
{
  /* The first center frequency on the frequency band. The value is  
  ** specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) 
  ** of downlink carrier frequency. 
  */
  uint16 start_ctr_freq;

  /* The last center frequency on the frequency band. The value is  
  ** specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) 
  ** of downlink carrier frequency.  
  */
  uint16 end_ctr_freq;
} tdsl1_freq_raw_scan_parm_struct;

/*
 * Frequency Scan, fine scan parameters
 */
typedef struct
{
  /* Center frequency for fine (200KHZ incremental) scan. The value is  
  ** specified as the UTRA Absolute Radio Frequency Channel Number (UARFCN) 
  ** of downlink carrier frequency. 
  */
  uint16 ctr_freq;
} tdsl1_freq_fine_scan_parm_struct;

/*FEATURE_LFS_COOS mainlined*/
/*#if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2) || defined(FEATURE_LFS_COOS)*/
typedef struct
{
    uint16  freq[MAX_ACQ_DB_ENTRIES];
  
    uint8   plmn_type[MAX_ACQ_DB_ENTRIES];
}tdsl1_freq_list_scan_parm_struct;
/*#endif*/

/*
 * Cell Reselection WCDMA System Information
 */
typedef struct
{
  /* Frequency of the IHO target cell */
  uint16    freq;

  /* Scrambling code of the IHO target cell */
  uint8    cpid;
} tdsl1_cell_resel_wcdma_info_type;

/*
 * Cell Reselection TD-SCDMA System Information
 */
typedef struct
{
  /* Frequency of the target cell */
  uint16    freq;

  /* Cell parameters ID of the target cell */
  uint8     cpid;
} tdsl1_cell_resel_tds_info_type;


/*
 * Cell Reselection GSM System Information
 */
typedef struct
{
  /* GSM Cell Reselection Cell Information */
  tdsl1_gsm_cell_info_type    cell_info;    

  /* Cell selection measurement made by SRCH, to be reported to RRC */     
  int16                    rxlev_meas;  
} tdsl1_gsm_cell_resel_ind_info_type;

/*
 * Cell Reselection IS2000 Information
 */
typedef struct
{   
  /***** to be completed *****/
  /* Needs to be filled in when this option is implemented */     
  uint16    dummy;
} tdsl1_cell_resel_is2000_info_type;


/* New Cell List type used in state transition meas*/
typedef struct
{
  boolean cell_list_incl;

  tdsl1_intra_freq_cell_list_struct_type cell_list;
} tdsl1_intra_freq_new_cell_list_struct_type; 


/* State Transition (DCH-->FACH) parameters */ 
typedef struct
{
 /* Measurement Id */
  uint16 meas_id;

  /* What to do with this Meas i.e DELETE, STOP */
  tdsl1_meas_trans_enum_type  meas_action;

  /* Meas Type */
  tdsl1_meas_type_enum_type   meas_type; 

  /* flag to indicate if HCS is used */
  boolean hcs_used;

  /* New Cell List obtained from SIB 11/12 after DCH-->FACH transition */
  union 
  {
    /* TDSL1_MEAS_STOP_AND_CONFIG_CELL_LIST */
    tdsl1_intra_freq_new_cell_list_struct_type l1_new_intra_freq_cell_list; 
    
    /* TDSL1_MEAS_STOP_AND_OVERWRITE */
    tdsl1_meas_sib_parm_struct_type l1_new_intra_freq_sib_meas_params;

    /* TDSL1_MEAS_STOP_AND_CONFIG_CELL_LIST */
    tdsl1_inter_freq_cell_list_struct_type l1_inter_freq_cell_list;

    /* INTER RAT cell info list */
    tdsl1_inter_sys_cell_list_struct_type   inter_s;
  } u; 

  /* FACH measurement information */
  boolean       fach_meas_incl;

  /* FACH Measurement occasion info included if yes,10.3.7.8 */
  /* FACH measurement info */
  tdsl1_fach_meas_info_struct_type   fach;

} tdsl1_meas_trans_type;

/* timing info for synchronized inter-RAT HO or redirection */
typedef struct
{
  uint16            uarfcn;
  uint16            ts0_position; /* ts0 start position from step0 result */
  uint8             lna_state; /* current LNA state */
  uint8             num_of_jds_cells; /* number of JDS cells for interference cancellation. Zero indicates invalid timing */
  uint8             cell_list[TDSIRAT_MAX_TDS_CELLS_PER_FREQ]; /* cell parameter id */
} tdsl1_acq_timing_type;

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN

/* intra, inter-freq, inter-rat are supported */
typedef enum
{
  TDSL1_RRC_UTRAN_NMR_INTRA_FREQ,
  TDSL1_RRC_UTRAN_NMR_INTER_FREQ,
  TDSL1_RRC_UTRAN_NMR_IRAT
} tdsl1_rrc_utran_nmr_report_type;

/* cell info parms for intra and inter-freq */
typedef struct
{
  /* L1 will del begin */
  uint16 psc;
  
  boolean cpich_ec_no_present;
  
  uint16 cpich_ec_no;
  
  boolean cpich_rscp_present;
  
  uint16 cpich_rscp;
  /* L1 will del end */
  
  /* L1 will add begin */
  /* 10.3.6.9 Cell parameters Id, (0, 127) */
  uint16 cpi;
  
  boolean pccpch_iscp_present;
  
  /* 10.3.7.65 Timeslot ISCP info */
  uint8  pccpch_iscp_no;
  
  uint8 pccpch_iscp[L1_MAX_TS];
  
  boolean pccpch_rscp_present;
  
  uint16 pccpch_rscp;
  /* L1 will add end */
  
  boolean pathloss_present;
  
  uint16 pathloss;
}tdsl1_meas_utran_nmr_cell_info_type;

/* cell info parms for inter-rat */
typedef struct
{
    boolean gsm_carrier_rssi_present;
  
    int16 gsm_carrier_rssi;
  
    tdsl1_bsic_choice_enum_type bsic_choice;
  
    union
    {
      /* Verified BSIC Information */
      struct
      {
        /* Inter-RAT cell id  spec[0..(maxCellMeas-1)] */
        uint16  inter_sys_cell_id;
      } bsic_verified;

      /* Non-Verified BSIC Information */
      struct
      {
        /* BCCH ARFCN  spec[0..1023] */
        W_ARFCN_T bcch_arfcn;
      } bsic_not_verified;
    } u;
}tdsl1_meas_utran_nmr_irat_cell_info_type;

/* inter-freq */
typedef struct
{
  uint16 num_cells;

  uint16 freq;

  tdsl1_meas_utran_nmr_cell_info_type nmr_cell_info[TDSL1_MAX_CELLS_FOR_NMR];
}tdsl1_meas_utran_nmr_info_type;

/* intra-freq */
typedef struct
{
  uint16 num_cells;

  tdsl1_meas_utran_nmr_cell_info_type nmr_cell_info[TDSL1_MAX_CELLS_FOR_NMR];
}tdsl1_meas_utran_nmr_intra_info_type;

/* inter-freq */
typedef struct
{
  uint16 num_freq;

  tdsl1_meas_utran_nmr_info_type l1_meas_utran_nmr_info[TDSL1_MAX_NON_USED_FREQ];
}tdsl1_meas_utran_nmr_inter_info_type;

/* inter-rat */
typedef struct
{
  uint16 num_cells;
  
  tdsl1_meas_utran_nmr_irat_cell_info_type irat_cell_info[TDSL1_MAX_CELLS_FOR_NMR];
}tdsl1_meas_utran_nmr_irat_info_type;

typedef union
{
  /* UTRAN NMR information for intra frequency */
  tdsl1_meas_utran_nmr_intra_info_type intra_freq_info;
  
  /* UTRAN NMR information for inter frequency */
  tdsl1_meas_utran_nmr_inter_info_type inter_freq_info;
  
  /* UTRAN NMR information for IRAT frequency */
  tdsl1_meas_utran_nmr_irat_info_type irat_info;
}tdsl1_meas_utran_nmr_info_union_type;

#endif 

//#ifdef FEATURE_TDSCDMA_DSDS
typedef enum
{
  TDSL1_FAILED_NORMAL,
  TDSL1_FAILED_NO_TRM_LOCK,
  TDSL1_FAILED_SPLIT_NO_TRM_LOCK,
  TDSL1_FAILED_INVALID_MAX
} tdsl1_common_fail_enum_type;
//#endif

/*==========================================================================
**
**      Commands    
**
**==========================================================================*/
/***************************************************************************
**                                                                       
**     DEFINITION OF ALL L1 COMMANDS FROM RRC                    
**                                                                       
***************************************************************************/

/*--------------------------------------------------------------------------
          TDSL1_CPHY_IDLE_REQ

Used by RRC to command Physical Layer into a known state of Idle with 
no UL or DL physical channels active.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} tdsl1_idle_cmd_type;

/*--------------------------------------------------------------------------
          TDSL1_CPHY_DEACTIVATE_REQ

Used by RRC to command Physical Layer into a Deactivated state.
--------------------------------------------------------------------------*/
typedef struct
{
  sys_stop_mode_reason_e_type  deact_reason;
} tdsl1_deactivate_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ

Used by RRC to command Physical Layer into a Stopped state.  Means WCDMA
is no longer the active RAT.
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_stop_cause_enum_type  cause;
} tdsl1_stop_cmd_type;

/*--------------------------------------------------------------------------
          TDSL1_CPHY_START_TDSCDMA_MODE_REQ

Used by RRC to command Physical Layer out of Stopped state and to start and
go idle.  Means WCDMA is now the active RAT.
--------------------------------------------------------------------------*/
	
typedef enum
{
  TDSL1_MEAS_MODE_NORMAL,
  TDSL1_MEAS_MODE_SGLTE
} tdsl1_meas_mode_enum_type;

typedef struct
{
  tdsl1_mdsp_image_enum_type  mdsp_image;
  tdsl1_start_cause_enum_type cause;
  tdsl1_meas_mode_enum_type meas_mode;
} tdsl1_start_cmd_type;

/*--------------------------------------------------------------------------
          TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ

Used by RRC to command Physical Layer into a Suspended state.  Means we're
attempting handover or cell reselection to another RAT.
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_suspend_resume_cause_enum_type cause;
  tdsl1_interrat_enum_type             rat;
} tdsl1_suspend_cmd_type;

/*--------------------------------------------------------------------------
          TDSL1_CPHY_RESUME_TDSCDMA_MODE_REQ

Used by RRC to command Physical Layer out of Suspended state and to resume
in the state it was in earlier.  Means the attempted handover or cell
reselection to another RAT failed, so we're coming back to WCDMA.
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_suspend_resume_cause_enum_type cause;
} tdsl1_resume_cmd_type;

/*--------------------------------------------------------------------------
          TDSL1CPHY_START_QTA_REQ

Indication from RRC that we are to start T2G quick tuneaway process.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
} tdsl1_start_qta_cmd_type;

/*--------------------------------------------------------------------------
          TDSL1CPHY_STOP_QTA_REQ

Indication from RRC that we are to stop T2G quick tuneaway process,
meaning resume WCDMA mode.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} tdsl1_stop_qta_cmd_type;

/*--------------------------------------------------------------------------
          TDSRRC_CPHY_FREQ_SCAN_CNF

Used by Layer 1 to report frequency scan results back to RRC.
--------------------------------------------------------------------------*/

/* The results of frequency scan reported to RRC for each of the scan types
**   are as follows:
**   
**       1. For a RAW scan, the report consists of all center frequencies
**          whose measured AGC RSSI values are above the pre-specified 
**         threshold, along theire AGC RSSI's. The frequecy list is sorted 
**          on AGC RSSI, with the strongest first.
**          
**       2. For a FINE scan, the report consists of all center frequencies
**          whose measured AGC RSSI values are above the pre-specified 
**          threshold, along theire AGC RSSI's. The frequecy list is sorted 
**          on AGC RSSI, with the strongest first.
*/

/* First, scan result structure for both raw and fine scan */
typedef struct
{
  uint16  ctr_freq;
  
  int16  rssi;
/*FEATURE_LFS_COOS mainlined*/  
/*#if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2) || defined(FEATURE_LFS_COOS)*/
  uint8  result_plmn_type;
/*#endif*/
} tdsl1_freq_scan_result_struct;

/* Finally, Frequency Scan Confirmation Type */
#ifdef FEATURE_TDSCDMA_DSDS
typedef struct
{
  boolean              status;
    /* Indicates if the acquisition succeeds. */
  tdsl1_common_fail_enum_type fail_type;
    /* Indicates the type of acq failure. */
  uint16  num_freq;
    /* Number of center frequencies reported */
  tdsl1_freq_scan_step_enum_type  step;
    /* Indicates which type of scan (RAW or FINE) the result is for */
  tdsl1_freq_scan_result_struct  freq_result[TDSL1_MAX_SCAN_FREQ];
    /* Center frequencies and their AGC RSSI's */
} tdsl1_freq_scan_cnf_type;
#else
typedef struct
{

  /* Number of center frequencies reported */
  uint16  num_freq;

  /* Indicates which type of scan (RAW or FINE) the result is for */
  tdsl1_freq_scan_step_enum_type  step;

  /* Center frequencies and their AGC RSSI's */
  tdsl1_freq_scan_result_struct  freq_result[TDSL1_MAX_SCAN_FREQ];
} tdsl1_freq_scan_cnf_type;
#endif

/*--------------------------------------------------------------------------
          TDSL1_CPHY_FREQ_SCAN_REQ

Used by RRC to command Layer 1 to perform WCDMA frequency scan on the
specified frequency band and with the specified incremental step.
--------------------------------------------------------------------------*/
/*
 * The frequency scan Step is used as a parameter of a frequency scan command.
 * The scan Step specifies the incremental step used for frequency band scan.
 * Currently, two incremental steps are used -- 2MHz(Raw) and 200KHz(Fine) steps.
 * 
 *     1. Initially, RRC requests 2MHz incremental frequency scan, with Step 
 *        "TDSL1_FREQ_RAW_SCAN", on WCDMA frequency band covering as much as 
 *        60MHz.
 *        
 *     2. Based on "raw" scan results, RRC requests L1 to perform a 200KHz 
 *        fine scan, with "TDSL1_FREQ_FINE_SCAN", on the +/-1 MHz band around 
 *        the center frequency specified by RRC.
 */
typedef struct
{
/*FEATURE_LFS_COOS mainlined*/
/*#if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2) || defined(FEATURE_LFS_COOS)*/
      uint16 num_freq;
    
      int16 agc;
/*#endif*/

  /* Indicates which type of scan (RAW or FINE or ADDITIONAL CHANNEL)
  ** will be performed 
  */
  tdsl1_freq_scan_step_enum_type  step;

  /* Indicates the type of band on which additional channels need 
  ** to be scanned 
  */    
  tdsl1_freq_scan_band_enum_type  band;

//#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  /* Indicates the search requested; service search, manual plmn or background plmn */
  tdsl1_wcdma_srch_enum_type  srch;
//#else
//  boolean                      is_list_search;
//#endif
  /* timer in ms, "0" means unlimited. L1 needs to send a response to RRC before this timer expires */
  uint32 timer;

  union
  {
    /* Raw Scan Parameters */
    tdsl1_freq_raw_scan_parm_struct   raw_scan_parm;

    /* Fine Scan Parameters */
    tdsl1_freq_fine_scan_parm_struct  fine_scan_parm;

/*#if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2) || defined(FEATURE_LFS_COOS)*/
    /* List AGC scan parameters*/
    tdsl1_freq_list_scan_parm_struct  list_scan_parm; 
/*#endif*/
  } u;
  
#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  boolean do_freq_scan_split;
#endif
} tdsl1_freq_scan_cmd_type;
  /* TDSL1_CPHY_FREQ_SCAN_REQ */
  /*--------------------------------------------------------------------------
			CPHY_ACQ_REQ
  
  Used by RRC to command Physical Layer into Acquisition state and request 
  cell search.
  --------------------------------------------------------------------------*/

typedef enum 
{
  /* No change */
  TDSL1_MULTISIM_DEVICE_MODE_NONE,
  /* Single active (including single-SIM, dual-SIM-single-active, tri-SIM-single-active) */
  TDSL1_MULTISIM_DEVICE_MODE_SA,
  /* Dual standby (including dual-SIM-dual-standby, tri-SIM-dual-standby if it exists) */  
  TDSL1_MULTISIM_DEVICE_MODE_DS,
  /* Dual active (including dual-SIM-dual-active, tri-SIM-dual-active if it exists) */
  TDSL1_MULTISIM_DEVICE_MODE_DA,
  /* Tri standby (including tri-SIM-tri-standby, reserved for future) */
  TDSL1_MULTISIM_DEVICE_MODE_TS,
  /* Tri active (including tri-SIM-tri-active, reserved for future) */
  TDSL1_MULTISIM_DEVICE_MODE_TA
} tdsl1_multisim_device_mode_info_enum_type;

  typedef struct
  {
	  /* TRUE: UE will be operating in dual SIM mode
	  ** FALSE: UE will be operating in single SIM mode*/
  boolean dual_standby_mode; /* obselete */
  tdsl1_multisim_device_mode_info_enum_type multisim_device_mode;
  } tdsl1_ds_status_change_ind_type;
  

/* ZZTBD: to be removed */
#if 0
/*--------------------------------------------------------------------------
          TDSL1_CPHY_ACQ_REQ

Used by RRC to command Physical Layer into Acquisition state and request 
cell search.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates the acquisition mechanism to be used, i.e. frequency
  ** only or frequency and scrambling code 
  */
  tdsl1_acq_mech_type  acq_type;

  /* Indicates which RF channel to be scanned. The value is  
  ** the UTRA Absolute Radio Frequency Channel Number (UARFCN) 
  ** of downlink carrier frequency, as defined in TS25.101. 
  */
  uint16                  freq;

  /* the number of individual radio links */
  uint16                  num_rl;

  /* Requested scrambling code.  Can be 0 to (16*511)+15 = 8191. */
  uint16                  scr_code[TDSL1_MAX_RL];
 
  /* Indicates how deep the reference RF channel shall be scanned.*/
  tdsl1_acq_mode_type  acq_mode;
  
#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
  /* Indicates the search requested; service search, manual plmn or background plmn */
  tdsl1_wcdma_srch_enum_type  srch;
#else
  boolean                 is_list_search;
#endif
  
  /* Indicates if AGC thresh needs to be compared before proceeding
  ** with acquisition path.  
  */
  boolean                 apply_agc_thresh;

  /* If present, PN position of the cell in chipx1 units. */
  uint32                  pn_pos;
  
  /* TRUE means "sttd_indicator" value is present.  FALSE otherwise. */
  boolean                 sttd_indicator_present;

  /* If present, TRUE means the cell is using Space Time Transmit Diversity.
  ** FALSE means it is not.  Described in TS 25.331, 10.3.6.78. 
  */
  boolean                 sttd_indicator;
 
  /* This variable indicates whether acquisition is being attempted
  ** for limited service (=TRUE) or automatic service (=FALSE)
  ** For limited service search, all T-barred timers should be stopped.
  */
  boolean                 unbar_freqs;

#if defined(FEATURE_FLEXIBLE_DEEP_SLEEP) || defined( FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2)
  /*The agc threshold value is set if the apply_agc_thresh (= TRUE)
  **Otherwise the value is not set and L1 will not look into this value
  **in that case, (= -1) indicates L1 shall decide the value
  */
  int16 agc;
#endif
} tdsl1_acq_cmd_type;
#endif

/*--------------------------------------------------------------------------
          TDSL1_CPHY_ACQ_REQ

Used by RRC to command Physical Layer into Acquisition state and request 
cell search.
--------------------------------------------------------------------------*/
typedef enum
{
  /* ACQ not under FS, L1 clears RSCP DB every time */
  TDSL1_ACQ_FS_NONE,
  /* First ACQ under FS, L1 creates new RSCP DB */
  TDSL1_ACQ_FS_START,
  /* Subsequent ACQ under FS, L1 updates RSCP DB without clearing it */
  TDSL1_ACQ_FS_CONTINUE,
  /* FS stops, L1 returns the strongest frequency in the RSCP DB */
  TDSL1_ACQ_FS_STOP,

  TDSL1_ACQ_FS_MAX
} tdsl1_acq_fs_enum_type;

typedef struct
{
 
  /* a list of RF channels that L1 should attempt acquisition on
        This array is used for acquisition without timing */
  uint16                    freq_list[TDSL1_MAX_ACQ_CHANNEL_NUM];

#ifdef FEATURE_TDSCDMA_L2TD_PS_HANDOVER
  /* a list of RF channel timing info that L1 should use for acq_mode type
     TDSL1_ACQ_MODE_ IRAT_TIMING. 
     For handover, only the first entry is valid; for redirection, multiple entries
     may be valid, depending on num_freqs below */
  tdsl1_acq_timing_type	freq_timing_list[TDSL1_MAX_ACQ_CHANNEL_NUM];
#endif

  /* Indicates number of RF channels in the freq_list or freq_timing_list  */
  uint8                     num_freqs;

   /* Refer to the single_cell description above. Valid range 0..127
  */
  uint8                     cpi;

  /* pre-measured rscp by other RAT for the given cpi, only used for acq_mode type
     TDSL1_ACQ_MODE_ IRAT_TIMING */
  int16                     rscp;

  /* When this parameter is TRUE, L1 should only verify if the cell
  ** freq_list[0] and cpi can be acquired. If it is FALSE, the cpi
  ** field is ignored by L1.
  */
  boolean                   single_cell;
  
 /* TRUE indicates this ACQ req is due to inter-RAT ho from other RAT
      FALSE means this ACQ is normal ACQ for cell camping */
  boolean                   is_ho;


 /* TRUE indicates this ACQ req is due to inter-RAT reselection from other RAT
      FALSE means this ACQ is normal ACQ for cell camping */
  boolean                   is_irat;

  /* This variable indicates whether acquisition is being attempted
  ** for limited service (=TRUE) or automatic service (=FALSE)
  ** For limited service search, all T-barred timers should be stopped.           
  */
  boolean                 unbar_freqs;
  
  /* timer in ms, "0" means unlimited. L1 needs to send a response to RRC before this timer expires */
  uint32                  timer;

  /* Acquisition mode */
  tdsl1_acq_mode_enum_type  acq_mode;
#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  boolean do_acq_split;
#endif
  /* Frequency scan status */
  tdsl1_acq_fs_enum_type acq_fs_status;

} tdsl1_acq_cmd_type;

#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY

/* Default value for invalid priority. Actual value range from 0 to 7.
                                                     After depriotization, priority could be -1. 
  */
#define TDSL1_INVALID_PRIORITY -2

/* Struct to store the Serving Cell priority info received from SIB19/3rd MEAS_REQ */
typedef struct
{
  /*Range: (0 to 7) */
  int8 priority;          

  /*Range: 2*(0 to 31) */
  uint8 s_prioritysearch1; 

  /*Range: (0 to 7), default is 0*/
  uint8 s_prioritysearch2; 

  /*Range: (0 to 31)*/
  uint8 thresh_servinglow; 
} tdsl1_serving_cell_priority_info_type;

/* Struct to store the inter-f priority info received from SIB19/3rd MEAS_REQ */
typedef struct
{
  uint16 UARFCN;

  /*Range: (0 to 7) */
  int8 priority;     

  /*Range: 2*(0 to 31)*/
  uint8 threshx_high;

  /*Range: 2*(0 to 31)*/
  uint8 threshx_low; 

  /*Range: -24 to 0, Default : -24 (Spec 25.331, 13.4.15c)*/
  int16 q_qualmin_fdd; 

  /*Range: 2*(-60 to -13)+1, Default : -119 (Spec 25.331, 13.4.15c)*/
  int16 q_rxlevmin_fdd; 
  /* Add a new interface for TDD LTE measurement */
  int16 q_rxlevmin_tdd;
} tdsl1_inter_freq_priority_info_type;

/* Struct to store the GSM priority info received from SIB19/3rd MEAS_REQ */
typedef struct
{
  W_ARFCN_T ARFCN;

  /*Range: (0 to 7) */
  int8 priority;       

  /*Range: 2*(-58 to -13)+1 */
  int16 q_rxlevmin_gsm; 

  /*Range: 2*(0 to 31)*/
  uint8 threshx_high;

  /*Range: 2*(0 to 31)*/
  uint8 threshxlow;  
} tdsl1_inter_rat_gsm_priority_info_type;

/* Struct to store the priority info for all neighbors carried in 3rd MEAS_REQ */
typedef struct
{
  tdsl1_serving_cell_priority_info_type serving_cell_priority;

  boolean inter_freq_priority_info_incl;

  uint8 num_inter_freq_priorities;

  tdsl1_inter_freq_priority_info_type inter_freq_priority[TDSL1_MAX_INTERF_PRIORITY_LAYERS];

  boolean inter_rat_gsm_priority_info_incl;

  uint8 num_gsm_priorities;

  tdsl1_inter_rat_gsm_priority_info_type gsm_priority[TDSL1_MAX_GSM_PRIORITY_LAYERS];

  boolean inter_rat_eutra_priority_info_incl;

#ifdef FEATURE_TDSCDMA_TO_LTE
  uint8 num_eutra_priorities;

  tdsl1_inter_rat_eutra_priority_info_type eutra_priority[TDSL1_MAX_EUTRA_PRIORITY_LAYERS];
#endif
} tdsl1_meas_priority_info_type;

/* Main parent struct for 3rd MEAS_REQ that carries all SIB-19 parameters */
typedef struct
{
#ifdef FEATURE_TDSCDMA_TO_LTE
  tdsl1_meas_eutra_frequency_list_type eutra_cell_list;
#endif

  tdsl1_meas_priority_info_type priority_info;
} tdsl1_meas_priority_eutra_cell_list_info_type;

#endif

/*--------------------------------------------------------------------------
          TDSL1_CPHY_CELL_PARM_REQ

Used by RRC to configure the Cell Selection and Reselection Parameters of 
the Serving cell in either Idle Mode or Connected Mode. The information is 
conveyed via SIB3/4 by WCDMA base station.
--------------------------------------------------------------------------*/
#if 0
/*
 * 10.3.2.3  - Serving cell parameters, mostly related to Cell selection
 *             and reselection, from SIB_3/4.
 */
typedef struct
{
  /* Flag indicating if mapping functionality is needed. 
  ** FALSE indicates no further mapping functionality is needed, or using 
  ** implicit mapping: Q_map = Q_meas_LEV. See section 7 of TS25.304. 
  */
  boolean  map_info_included;

  /* Measurement mapping information. It is used when the above flag is TRUE */
  tdsl1_mapping_info_struct_type  meas_map_info;

  /* Measurement quantity -- Ec/Io or RSCP. Note that, for Cell Selection      
  ** and Reselection, we only support Ec/Io Measurement Quantity on FDD cells, 
  ** as required by TS25.304. 
  */
  tdsl1_meas_quan_enum_type   quan_type;

  /* whether s_intrasrch is included */
  boolean s_intrasrch_incl;

  /* Intra-frequency measurement threshold. Value (in dB) ranges (-32..20) 
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */
  int16   s_intrasrch;

  /* whether s_intersrch is included */
  boolean s_intersrch_incl;

  /* Inter-frequency measurement threshold. Value (in dB) ranges (-32..20) 
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */  
  int16   s_intersrch;

  /* whether s_srchhcs is included */  
  boolean s_srchhcs_incl;

  /* HCS measurement threshold. Value (in dB) ranges (-105..91)      
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */  
  int16   s_srchhcs;
    
  /* DDH THIS NOT NEEDED num_rats should be and is expected to be zero if 
  ** there are no other rats. Flag indicating whether info is included for any 
  ** interrat neighbors  
  */
  boolean s_interrat_incl; 

  /* Number of additional RATs included */    
  uint16  num_rats;

  /* Inter-RAT reselection info, as defined in 10.3.2.3 of TS25.331 */
  tdsl1_interrat_resel_info_type     interrat_info[TDSL1_MAX_OTHER_RAT];

  /* Hysteresis value of the serving cell for RSCP (?), It is used for    
  ** calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2. 
  */
  uint16   qhyst1_s;

  /* Hysteresis value of the serving cell for Ec/Io (?), It is used for     
  **  calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2. 
  **  Default value is qhyst1_s. 
  */
  uint16   qhyst2_s;

  /* Cell reselection timer, integer (0..31000) in milli seconds */    
  uint16   trselc_ms;

  /* Flag indicating if HCS info below is applicable */
  boolean hcs_info_incl;

  /* Serving cell HCS information */
  tdsl1_serv_cell_hcs_info_struct_type  hcs_info;

  /* Maximum allowed UL TX power, UE_TXPWR_MAX_RACH (dBm) */
  int16   max_tx_pwr;

  /* Minimum required quality level for Ec/Io measurement (in dB).
  ** Value ranges (-24..0) 
  */
  int16   q_qualmin;

  /* Minimum required received level for RSCP measurement (in dBm).
  ** Value ranges (-115..-25) by step of 2. 
  */  
  int16   q_rxlevmin;

  /* maximum number fo cell reselections. Value ranges (1..16). Default
  ** value is 8. 
  */  
  uint16   nhcs_n_cr;

  /* Duration for evaluating alowed amount of cell reselections. 
  ** Default value is 0, which means the parameter is not used. When used, 
  ** the values are (0, 30, 60, 120, 180, 240), (in second). 
  */
  tdsl1_t_cr_max_enum_type   nhcs_t_crmax;

  /* Additional time period before UE recert to low-mobility measurement. 
  ** Default value is 0, which means the parameter is not used. When used, 
  ** the value ranges (10..70) by step of 10, (in second). 

  */
  tdsl1_t_cr_max_hyst_enum_type   nhcs_t_crmaxhyst;

  /* 0 to 10;  coresponds  speed dependent scale factor/10 */
  uint16 tresel_scale;

  /* 4 to 19. corresponds to interf scale factor / 4*/    
  uint16 tresel_interf_scale ;

  /* 4 to 19. corresponds to inter-rat scalefactor/ 4*/    
  uint16 tresel_irat_scale; 
} tdsl1_cell_parm_cmd_type;
#endif

typedef struct
{
  /* Flag indicating if mapping LCR functionality is needed. */
  boolean  mapping_lcr_included;
  
  /* Measurement mapping LCR R4. It is used when the above flag is TRUE */  
  tdsl1_mapping_LCR_struct_type  mapping_lcr;

  /* Measurement quantity -- Ec/Io or RSCP. Note that, for Cell Selection      
  ** and Reselection, we only support Ec/Io Measurement Quantity on FDD cells, 
  ** as required by TS25.304. 
  */  
  tdsl1_meas_quan_enum_type   quan_type;

  /* whether s_intrasrch is included */
  boolean s_intrasrch_incl;

  /* Intra-frequency measurement threshold. Value (in dB) ranges (-105..91) 
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */  
  int16   s_intrasrch;

  /* whether s_intersrch is included */
  boolean s_intersrch_incl;

  /* Inter-frequency measurement threshold. Value (in dB) ranges (-105..91) 
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */  
  int16   s_intersrch;

  /* whether s_srchhcs is included */
  boolean s_srchhcs_incl;
  
  /* HCS measurement threshold. Value (in dB) ranges (-105..91) 
  ** by step of 2. 0xFFFF indicates the parameter is not applicable. 
  */  
  int16   s_srchhcs;
  
  /* Flag indicating whether info is included for any interrat
  ** neighbors  
  */
  boolean s_interrat_incl; 


  /* DDH THIS NOT NEEDED num_rats should be and is expected to be zero if 
  ** there are no other rats.  Number of additional RATs included
  */
  uint16  num_rats;

  /* Inter-RAT reselection info, as defined in 10.3.2.3 of TS25.331 */
  tdsl1_interrat_resel_info_type_tdstemp   interrat_info[TDSL1_MAX_OTHER_RAT];
  
  /* Hysteresis value of the serving cell for RSCP (?), It is used for 
  ** calculating cell-ranking R in HCS. Value ranges (0..40) by step of 2. 
  */  
  uint16   qhyst1_s;

  /* Cell reselection timer, integer (0..31000) in milli seconds */
  uint16   trselc_ms;
 
  /* Flag indicating if HCS info below is applicable */
  boolean hcs_info_incl;

  /* Serving cell HCS information */
  tdsl1_serv_cell_hcs_info_struct_type  hcs_info;
  
  /* Maximum allowed UL TX power, UE_TXPWR_MAX_RACH (dBm) */
  int16   max_tx_pwr;
  
  /* Minimum required received level for RSCP measurement (in dBm). 
  ** Value ranges (-115..-25) by step of 2. 
  */
  int16   q_rxlevmin;

  /* Offset to the signalled Qrxlevmin taken into account in the Srxlev 
  ** evaluation as a result of a periodic search for a higher priority  
  ** PLMN while camped normally in a VPLMN. SIB3 only in v770 Ext.      
  ** Value ranges (2..16) by step of 2. Default to 0 
  */  
  uint16   q_rxlevmin_offset;

  /* maximum number fo cell reselections. Value ranges (1..16).  
  ** Default value is 8. 
  */  
  uint16   nhcs_n_cr;

  /* Duration for evaluating alowed amount of cell reselections. 
  ** Default value is 0, which means the parameter is not used. When used, /
  ** the values are (0, 30, 60, 120, 180, 240), (in second). 
  */
  tdsl1_t_cr_max_enum_type   nhcs_t_crmax;

  /* Additional time period before UE recert to low-mobility measurement. 
  ** Default value is 0, which means the parameter is not used. When used, 
  ** the value ranges (10..70) by step of 10, (in second). 
  */  
  tdsl1_t_cr_max_hyst_enum_type   nhcs_t_crmaxhyst;

  /* 0 to 10;  coresponds  speed dependent scale factor/10 */
  uint16 tresel_scale;

  /* 4 to 19. corresponds to interf scale factor / 4*/
  uint16 tresel_interf_scale ;

  /* 4 to 19. corresponds to inter-rat scalefactor/ 4*/
  uint16 tresel_irat_scale;   
} tdsl1_cell_parm_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_IMMEDIATE_CELL_EVAL_REQ

Used by RRC to request the Physical Layer to perform a quick evaluation of 
the quality of the intra-frequency cells prior to RACH transmission.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} tdsl1_immediate_cell_eval_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_MEASUREMENT_REQ

Used by RRC to configure L1 measurement parameters, including neighbor 
information, in Idle Mode or Connected Mode. In Idle Mode, the measurement 
parameters are based on SIB_11. In Connected Mode, the measurement parameters 
are based on either SIB_12 (SIB_11 when SIB_12 is absent) or Measurement 
Control Message. 

All sections adhere to 2002-03 version of 25.331
--------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING
typedef struct 
{ 
  /* L1 will del begin */
  tdsl1_meas_quan_enum_type interf_rach_rpt_quan;
  /* L1 will del end */
  
  /* L1 will add begin */
  uint8                           rach_meas_quan_list_no;
  
  /* RACH Reporting Quantity. Note that, for TDSCDMA TDD cells, we currently 
  ** support Iscp or RSCP Measurement Quantity for DCH Measurement report.
  */  
  tdsl1_meas_quan_enum_type   rach_rpt_quan_list[L1_RACH_MAX_MEAS_LIST_NO];
  /* L1 will add end */
  
  /* -115.0 */
  int16 interf_rach_reporting_thresh; 

  /* 18 */
  uint8 interf_rach_max_reporting_cells; 
} tdsl1_meas_rach_interf_struct_type;
#endif /*FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING*/

/* 
 * 10.3.7.47  Measurement Control System Information
 * Measurement Control Parameter Type for both SIB_11/12 and 
 * Measurement Control Message 
 */
typedef struct
{
  tdsl1_meas_ctrl_enum_type parm_type;
  
  union
  {
    tdsl1_meas_sib_parm_struct_type sib;
  
    tdsl1_meas_ctrl_parm_struct_type ctrl;
  
    /* This cmd comes to L1 only when RRC transitions from FACH<-->DCH */
    tdsl1_meas_trans_type trans_meas;
  
    tdsl1_inter_freq_cell_list_struct_type inter_freq_sib;
   /*L1 to look at this structure only when parm_type is set 
    to TDSL1_DEFERRED_MEASUREMENT_FROM_SIB */
    tdsl1_deferred_meas_from_sib_struct_type deferred_meas_from_sib;
#ifdef FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
    tdsl1_meas_priority_eutra_cell_list_info_type priority_eutra_cell_list;
#endif
  } u;
  
#ifdef FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING
  /* If yes, next field is valid */ 
  boolean inter_freq_rach_info_incl;

  tdsl1_meas_rach_interf_struct_type interf_rach_rpt_inf;
#endif /*FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING*/
} tdsl1_meas_parm_type;


/*
 * 10.2.17  Measurement Control Message (RRC command)
 */
typedef struct
{
  /* Pointer to the parameter storage memory */ 
  tdsl1_meas_parm_type *meas_ptr;
  #ifdef FEATURE_SGLTE
  tdsl1_meas_mode_enum_type meas_mode;
  #endif
} tdsl1_meas_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_SET_SFN_REQ

Used by RRC to set SFN based on current BCH frame count.
--------------------------------------------------------------------------*/
typedef struct
{
  /* SFN from received BCH frame */
  uint32 cur_sfn;

  /* SFN on reference L1 BCH frame */    
  uint32 ref_sfn;    
} tdsl1_sfn_cmd_type;


/*--------------------------------------------------------------------------
          CPHY_SET_CFN_REQ

Used by RRC to set current CFN.
--------------------------------------------------------------------------*/
typedef struct
{
  /* Frame offset in number of frames */
  uint32 tdsframe_offset;    
} tdsl1_cfn_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_SIB_SCHED_CHANGE_IND

Used by RRC to request certain frames to be read from the NBCH
--------------------------------------------------------------------------*/

typedef struct 
{
  /* Pointer to an array of 512 bytes (or 4096 bits) to handle all 4096 SFN's.
  ** On N_BCH setup, this bit array identifies SFN's where RRC wants to receive
  ** SIB data.  A 1 bit tells WL1 to receive that frame.  A 0 means WL1 may skip
  ** it.  WL1 can use this info to sleep between SIB frames and conserve power. 
  */
  uint8  *sib_sfn_bit_mask;
	/* Tell L1 if they needs to release lock during SIB gap */ //xlong 0410
  boolean hold_trm_lock_for_sib_gap;
} tdsl1_sib_sched_change_ind_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_SETUP_REQ
Used by RRC to send setup commands to L1.
--------------------------------------------------------------------------*/
/* Operation types for physical channel setup */
//move to L1 internal interface .h xuqiangz ????
typedef enum {
  TDSL1_SETUP_OPS_NOOP,
  TDSL1_SETUP_OPS_DROP,
  TDSL1_SETUP_OPS_CFG,
  TDSL1_SETUP_OPS_ADD,
  TDSL1_SETUP_OPS_DPCH_DROP_ADD,//no use
  TDSL1_SETUP_OPS_EN_DIS_TRCH,//no use
  TDSL1_SETUP_OPS_SET_LOOPBACK_MODE
} tdsl1_setup_ops_enum_type;

/* TDSL1_CPHY_SETUP_REQ Sub-primitive mask: */
#define TDSL1_CPHY_UL_PHYCHAN_DROP_INCL      0x0001
#define TDSL1_CPHY_DL_PHYCHAN_DROP_INCL      0x0002
#define TDSL1_CPHY_DL_CCTRCH_INCL            0x0004
#define TDSL1_CPHY_UL_CCTRCH_INCL            0x0008
#define TDSL1_CPHY_DL_PHYCHAN_ADD_INCL       0x0010
#define TDSL1_CPHY_UL_PHYCHAN_ADD_INCL       0x0020
#define TDSL1_CPHY_DL_PHYCHAN_CFG_INCL       0x0040
#define TDSL1_CPHY_UL_PHYCHAN_CFG_INCL       0x0080

#define TDSL1_CPHY_HS_CHAN_CFG_INCL          0x0100

#ifdef FEATURE_TDSCDMA_HSUPA

/* If following bitmask is set, it indicates that L1 shall look into 
 * tdsl1_e_dl_info_struct_type present in tdsl1_e_info_struct_type in setup cmd 
 * sent by RRC i.e if this bit is NOT set then it indicates NOOP for DL configuration.
 * This bit should not be set by RRC when one of the following is met:
 *   1. Config message wont result in change in EUL(E-DCH) state and config
 *   2. When there is only change of UL config
 *   3. CphySetupFailure(functional failure) - 
 *       a) reverting back to old config (DCH->E-DCH)
 *       b) reverting to old config (E-DCH->E-DCH) when config message
 *          dont result in deletion of any E-DCH active set RL. */
#define TDSL1_CPHY_E_DL_CHAN_CFG_INCL        0x0200

/* If following bitmask is set, it indicates that L1 shall look into 
 * tdsl1_e_ul_info_struct_type present in tdsl1_e_info_struct_type in setup cmd 
 * sent by RRC i.e if this bit is NOT set then it indicates NOOP for UL configuration.
 * This bit should not be set by RRC when one of the following is met:
 *   1. Config message wont result in change in EUL(E-DCH) state and config
 *   2. When there is only change of DL config
 *   3. CphySetupFailure(functional failure) - 
 *       a) reverting back to old config (DCH->E-DCH)
 *       b) reverting to old config (E-DCH->E-DCH) when config message
 *          dont result in deletion of any E-DCH active set RL. */
#define TDSL1_CPHY_E_UL_CHAN_CFG_INCL        0x0400

/*Added for TD-SCDMA non scheduled grant configuration. 
** RRC set this bit if non scheduled grant info is includ in CPHY_SETUP_REQ CMD 
**/

#define TDSL1_CPHY_NON_SCHED_GRANT_CFG_INCL  0x0800

#endif /* FEATURE_TDSCDMA_HSUPA */


#define TDSL1_CPHY_DL_TRCH_EN_DIS_INCL       0x1000
#define TDSL1_CPHY_ASET_UPDATE_INCL          0x2000
#define TDSL1_CPHY_DL_SET_LOOPBACK_MODE_INCL 0x4000
#define TDSL1_CPHY_DROP_ALL_INCL             0x8000

/* Need to consolidate with the constants below */
#define TDSL1_DCH_SETUP_REQ_RECOVER_FROM_HHO  0x0002
#define TDSL1_DCH_SETUP_REQ_RECOVER_OLD_CFG   0x0100

/*+TDSCDMA New bitmask for handover case*/
#define TDSL1_CPHY_HHO_NOT_BATON_INCL 0x0002        /* bit1  .........1 HHO; 0 Baton HO */
#define TDSL1_CPHY_HHO_TI_INCL 0x0004                       /* bit2  .........1 for Timing Reinit; 0 for Timing Maintainened */
#define TDSL1_CPHY_HHO_REVERT_OLD_INCL 0x0100       /* bit8  .........1 revert back to old configuration; 0 not revert back */
/*-TDSCDMA New bitmask for handover case*/
 

/* The following enum values correspond to the CPHY setup commands
 * defined in the Request Mask field of the CPHY setup command packet.
 * Enum value 0 corresponds to bit position 0, enum value 1 corresponds
 * to bit position 1, and so on...
 */
typedef enum
{
  TDSL1_CPHY_UL_PHYCHAN_DROP_REQ,
  TDSL1_CPHY_DL_PHYCHAN_DROP_REQ,
  TDSL1_CPHY_DL_CCTRCH_REQ,
  TDSL1_CPHY_UL_CCTRCH_REQ,
  TDSL1_CPHY_DL_PHYCHAN_ADD_REQ,
  TDSL1_CPHY_UL_PHYCHAN_ADD_REQ,
  TDSL1_CPHY_DL_PHYCHAN_RECFG_REQ,
  TDSL1_CPHY_UL_PHYCHAN_CFG_REQ,

  TDSL1_CPHY_HS_CHAN_CFG_REQ,

 #ifdef FEATURE_TDSCDMA_HSUPA
  TDSL1_CPHY_EUL_CHAN_DL_CFG_REQ,
  TDSL1_CPHY_EUL_CHAN_UL_CFG_REQ,
  TDSL1_CPHY_EUL_NON_SCHED_GRANT_CFG_REQ,
 #endif
 

  /* request mask is zero, or all requests have been processed */
  TDSL1_CPHY_SETUP_DONE,


  TDSL1_CPHY_SETUP_NOOP,

  TDSL1_CPHY_SETUP_NUM_CMDS
} tdsl1_cphy_setup_cmd_id_enum_type;


/* Operation types for physical channel setup */
typedef enum {
  TDSL1_HSDPA_SETUP_OPS_NOOP,
  TDSL1_HSDPA_SETUP_OPS_START,
  TDSL1_HSDPA_SETUP_OPS_RECFG,
  TDSL1_HSDPA_SETUP_OPS_STOP
} tdsl1_hsdpa_setup_ops_enum_type;


/*--------------------------------------------------------------------------
MACROS FOR DETERMINING WHICH SUB-PRIMITIVES ARE INCLUDED IN A TDSL1_CPHY_SETUP_REQ

Use these macros to determine whether a given sub-primitive is included
in a TDSL1_CPHY_SETUP_REQ.  Pass in the req_mask field from the command packet.
--------------------------------------------------------------------------*/
#define TDSL1_CPHY_SETUP_REQ_IS_DL_CCTRCH_INCL(mask) \
  (((mask) & TDSL1_CPHY_DL_CCTRCH_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_UL_CCTRCH_INCL(mask) \
  (((mask) & TDSL1_CPHY_UL_CCTRCH_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_DROP_INCL(mask) \
  (((mask) & TDSL1_CPHY_UL_PHYCHAN_DROP_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_DROP_ALL_INCL(mask) \
  (((mask) & TDSL1_CPHY_DROP_ALL_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_DROP_INCL(mask) \
  (((mask) & TDSL1_CPHY_DL_PHYCHAN_DROP_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_CFG_INCL(mask) \
  (((mask) & TDSL1_CPHY_DL_PHYCHAN_CFG_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_CFG_INCL(mask) \
  (((mask) & TDSL1_CPHY_UL_PHYCHAN_CFG_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(mask) \
  (((mask) & TDSL1_CPHY_DL_PHYCHAN_ADD_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_ADD_INCL(mask) \
  (((mask) & TDSL1_CPHY_UL_PHYCHAN_ADD_INCL) ? TRUE : FALSE)
  
#define TDSL1_CPHY_SETUP_REQ_IS_HS_CHAN_CONFIG_INCL(mask) \
  (((mask) & TDSL1_CPHY_HS_CHAN_CFG_INCL) ? TRUE : FALSE)

#ifdef FEATURE_TDSCDMA_HSUPA
#define TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_DL_CONFIG_INCL(mask) \
  (((mask) & TDSL1_CPHY_E_DL_CHAN_CFG_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_UL_CONFIG_INCL(mask) \
  (((mask) & TDSL1_CPHY_E_UL_CHAN_CFG_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_NON_SCHED_GRANT_CONFIG_INCL(mask) \
  (((mask) & TDSL1_CPHY_NON_SCHED_GRANT_CFG_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_START_INCL(mask) \
  (((mask) & TDSL1_SCHED_EUL_START) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_NON_SCHED_GRANT_START_INCL(mask) \
  (((mask) & TDSL1_NON_SCHED_GRANT_START) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_STOP_INCL(mask) \
  (((mask) & TDSL1_EUL_STOP) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_NOOP(mask) \
  (((mask) == TDSL1_EUL_NOOP) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EDL_CFG_INCL(mask) \
  (((mask) & TDSL1_SCHED_EUL_RECFG_DL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_NON_SCHED_GRANT_CFG_INCL(mask) \
  (((mask) & TDSL1_NON_SCHED_GRANT_RECFG) ? TRUE : FALSE)


  #define TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_CFG_INCL(mask) \
  (((mask) & TDSL1_EUL_RECFG_UL) ? TRUE : FALSE)

#define TDSL1_CPHY_E_DL_CHAN_MASK_REQ_IS_OP_INCL(mask, chan, op) \
  (((mask) & L1_E_##chan##_INFO_##op##) ? TRUE : FALSE)

#define TDSL1_CPHY_E_DL_CHAN_MASK_REQ_IS_ANY_OP_INCL(mask, chan) \
  ( (((mask) & L1_E_##chan##_INFO_ADD) || ((mask) & L1_E_##chan##_INFO_RECFG) || ((mask) & L1_E_##chan##_INFO_DROP)) ? TRUE : FALSE)


#endif /* FEATURE_TDSCDMA_HSUPA */


#define TDSL1_CPHY_SETUP_REQ_IS_DL_TRCH_EN_DIS_INCL(mask) \
  (((mask) & TDSL1_CPHY_DL_TRCH_EN_DIS_INCL) ? TRUE : FALSE)
 
#define TDSL1_CPHY_SETUP_REQ_IS_DL_LOOPBACK_MODE_INCL(mask) \
  (((mask) & TDSL1_CPHY_DL_SET_LOOPBACK_MODE_INCL) ? TRUE : FALSE)

#define TDSL1_CPHY_SETUP_REQ_IS_ACT_TIME_INCL(act_time) \
  ((act_time) != TDSL1_ACTIVATION_TIME_NONE)

/*The rf channel mask.  This denotes that the actual RF channel is specified
**in the 14 LSB of the rf_chan field in the TDSL1_CPHY_SETUP_REQ command packet 
*/
#define TDSL1_CPHY_SETUP_REQ_RF_MASK    0x3FFF

/***************************************************************************
**                                                                       
**   R4 UL related configuration                  
**                                                                       
***************************************************************************/
 
typedef struct {
  /* Indicate what UL sync codes are available */
  uint8     avail_ul_sync_code_mask;

  /* The number of sub-channels (1, 2, 4, 8) */
  uint8     sub_chan_size;

  /* Indicate what sub-channels are available */
  uint8     avail_ul_sub_chan_mask;
} tdsl1_ul_asc_struct_type;


/* Access Service Class (ASC) configuration
 * See 25.331 v9.1.0, 10.3.6.53 for PRACH partitioning
 */
typedef struct {
  /* Number of Acess Service Classes (1..8) */
  uint8                               num_asc;

  /* access service class */
  tdsl1_ul_asc_struct_type           *asc_info_ptr[TDSL1_UL_ASC_MAX];
} tdsl1_ul_asc_cfg_struct_type;


typedef struct {
  /* Channelisation code (1..16) */
  uint8                       chan_code;

  /* Spreading factor. (1, 2, 4, 8, 16) for DPCH, (4, 8, 16) for PRACH, (16) for FPACH */
  tdsl1_sf_enum_type          sf;
} tdsl1_ul_chan_code_struct_type;


typedef struct {
  /* FPACH timeslot number (0..6) */
  uint8                                 fpach_ts_num;

  /* FPACH channelisation code */
  tdsl1_ul_chan_code_struct_type        fpach_chan_code;

  /* FPACH midamble shift and burst. See 25.331 v9.1.0, 10.3.6.41 */
  tdsl1_midamble_ss_burst_struct_type   midamble_and_burst;

  /* WAIT TIME. The number of sub-frames to wait for FPACH ACK, following
   * the sub-frame in which the SYNC_UL is transmitted. (1..4) */
  uint8                                 wt;
} tdsl1_ul_fpach_info_struct_type;


/* Structure of FPACH and associated PRACH 
 * See 25.331 v9.1.0, 10.3.6.52
 */
typedef struct {
  /* PRACH time slot number */
  uint8                             prach_ts_num;

  /* The number of PRACH channelisation codes */
  uint8                             num_prach_codes;

  /* PRACH channelisation code list. See 25.331 v9.1.0, 10.3.6.51a */
  tdsl1_ul_chan_code_struct_type    prach_chan_codes[TDSL1_UL_PRACH_CODE_MAX];

  /* PRACH midamble shift and burst. See 25.331 v9.1.0, 10.3.6.41 */
  tdsl1_midamble_ss_burst_struct_type  midamble_and_burst;

  /* FPACH info. See 25.331 v9.1.0, 10.3.6.35a */
  tdsl1_ul_fpach_info_struct_type   fpach_info;
} tdsl1_ul_prach_fpach_struct_type;

typedef struct {
  /* Indicate what UL sync codes are available */
  uint8                             ul_sync_code_mask;

  /* FPACH info for re-sync */
  tdsl1_ul_fpach_info_struct_type   ul_fpach_info;

  /* Desired UpPCH received power, in dBm (-120..-58) 
   * See 25.331, 10.3.6.52 and 10.3.6.78a 
   */
  int8                              prx_upPch_des;

  /* UL sync code power ramp step when no FPACH received after sent SYNC_UL. 
   * The value is (0, 1, 2, 3) in dB. Default value is 2. See 25.331: 10.3.6.96 */
  uint8                             pwr_ramp_step;

  /* Maximum numbers of SYNC_UL transmissions.
   * The value is (1, 2, 4, 8). Default value is 2.
   * See 25.331: 10.3.6.96 
   */
  uint8                             max_sync_ul_tx;

  /* Frequency for FPACH */
  int16                             fpach_freq;

  /* UpPCH shift, in 16chips (0..127)
   * See 25.331, 10.3.2.5 and 10.3.6.39a 
   */
  uint8                             upPch_shift;
} tdsl1_ul_sync_parms_struct_type;


typedef struct {
  /* Timeslot number, (1..5) */
  uint8                             ts_num;

  /* Whether TFCI exist */
  boolean                           tfciExist;

  /* DPCH midamble shift and burst */
  tdsl1_midamble_ss_burst_struct_type   midamble_and_burst;

  /* Modulation type. See 25.331 10.3.6.37 */
  tdsl1_mod_enum_type      modType;

  /* Denotes amount of SS and TPC symbols send in this timeslot
   * See 25.221: 5A.2.2.2 and 25.331: 10.3.6.37 
   */
  tdsl1_ss_tpc_symb_enum_type       ss_tpc_symbols;

  /* Specifies the number of additional codes in this timeslot that
   * carry TPC and SS symbols. 0-not used.
   * See 25.331: 10.3.6.37
   */
  uint8                             add_ss_tpc_symbols;

  /* The number of channelisation codes */
  uint8                             num_chan_codes;

  /* DPCH channelisation code list. */
  tdsl1_ul_chan_code_struct_type    chan_code_list[TDSL1_MAX_NUM_UL_CODE_PER_SLOT];
} tdsl1_ul_dpch_ts_struct_type;


/* Uplink PRACH configuration.
 * See 25.331 v9.1.0, 10.3.6.55 for PRACH System Info List. 
 */
typedef struct {
  /* The number of PRACH-FPACH pairs (maximum pairs=8) */
  uint8                             num_prach_fpach;

  /* PRACH and FPACH mapping info
   * See 25.331 v9.1.0, 10.3.6.52
   */
  tdsl1_ul_prach_fpach_struct_type  prach_fpach_info[TDSL1_UL_PRACH_FPACH_MAX];

  /* Trch ID of the RACH which mapped to this PRACH */
  uint8                             trch_id;

  /* Bitmap of SYNC_UL codes. Each bit indicates availability of a SYNC_UL code,
   * where the SYNC_UL codes are numbered "code0" to "code7". The value 1 of a 
   * bit indicates that the corresponding SYNC_UL code can be used, vice versa.
   * See 25.331 v9.1.0, 10.3.6.52 and 10.3.6.78a 
   */
  uint8                             sync_ul_codes_bit_map;

  /* Desired UpPCH receiving power, in dBm (-120..-58) 
   * See 25.331 v9.1.0, 10.3.6.78a */
  int8                              prx_upPch_des;

  /* UL sync code power ramp step when no ACK, in dB (0..3) */
  uint8                             pwr_ramp_step;

  /* Maximum UL sync code transmission, (1, 2, 4, 8) */
  uint8                             max_sync_ul_tx;

  /* Tx Power of PCCPCH, in dBm (6..43) */
  uint8                             pccpch_tx_pwr;

  /* UpPCH shift, in 16chips (0..127)
   * See 25.331 v9.1.0, 10.3.2.5 and 10.3.6.39a */
  uint8                             upPch_shift;

  /* Access Service Class (PRACH partitioning) */
  tdsl1_ul_asc_cfg_struct_type      asc_cfg;
} tdsl1_ul_prach_cfg_struct_type;


/* UL DPCH Info.
 * See 25.331 v9.1.0, 10.3.6.88
 */
typedef struct {

  /* TRUE the field existed and always TRUE; FALSE the field not present. See 25.331:10.3.6.91 */
  boolean  beacon_pl_est;
  
  /* TPC step size. See 25.331 v9.1.0, 10.3.6.91 */
  tdsl1_tpc_step_size_enum_type     tpc_step_size;

  /* PCCPCH tx power, for pathloss calculation. In dBm (6..43) or 0 if not specified.
   * See 25.331 v9.1.0, 10.3.6.91
   */
  uint8                             pccpch_tx_pwr;

  /* Whether UL timing advance control is present */
  boolean                           ul_timing_advance_ctrl_present;
  
  /* Whether UL timing advance control is enabled */
  boolean                           ul_timing_advance_ctrl_enabled;

  /* Uplink synchronisation step size. (1..8) default 1
   * See 25.331 v9.1.0, 10.3.6.96
   */
  uint8                             ul_sync_step_size;

  /* Uplink synchronisation frequency. (1..8) default 1
   * See 25.331 v9.1.0, 10.3.6.96
   */
  uint8                             ul_sync_freq;
/* zhouya add for 10.3.6.88 begin */
  boolean                           ul_sync_parm_present;
/* zhouya add for 10.3.6.88 end */
  /* UL sync parameters */
  tdsl1_ul_sync_parms_struct_type   ul_sync_parms;

  /* DPCH desired received power, in dBm (-120..-58) */
  int8                              prx_dpch_des;

  /* See 25.331 v9.1.0, 10.3.6.88/10.3.6.83 */
  uint16                            act_time;
/* zhouya add for 10.3.6.88 begin(merge UL & DL) */
  tdsl1_common_ts_info_struct_type common_ts_info;
  /* zhouya add for 10.3.6.88 end */

  /* whether use dynamic SF */
  boolean                           dynamicSfUse;

  /* The number of timeslots, (1..5) */
  uint8                             num_ts;

  /* Time slot list */
  tdsl1_ul_dpch_ts_struct_type      ts_list[TDSL1_MAX_NUM_TS];
} tdsl1_ul_dpch_cfg_struct_type;


/* uplink physical channel configuration structure */
typedef struct {
  /* Uplink PRACH configuration */
  tdsl1_ul_prach_cfg_struct_type  prach_cfg;

  /* Uplink DPCH configuration */
  tdsl1_ul_dpch_cfg_struct_type   dpch_cfg;
} tdsl1_ul_phych_cfg_struct_type;


/* Semi-static Parameters */
typedef struct {
  /* Coding type: No-coding, 1/2 Convolution, 1/3 convolution and Trubo */
  tdsl1_coding_enum_type          coding;

  /* Static Rate Matching attribute 1..256 (see 25.222) */
  uint16                          staticRM;

  /* CRC size, in bit */
  tdsl1_crc_length_enum_type      crc;

  /* TrCH id */
  uint8                           trch_id;
} tdsl1_ul_semi_static_parms_struct_type;


/* Gain Factor Parameters - indexed by TFC id.
 * See 25.331 v9.1.0, 10.3.5.8.
 */
typedef struct {
  /* gain factor calculation method - signalled or calculated */
  tdsl1_ul_gain_factor_calc_enum_type algo;

  /* Reference TFC ID. Mandatory for computed. Optional for signalled */
  uint8                               ref_tfc_id;

  /* Used only for signalled. TRUE if ref_tfc_id above is valid. */
  boolean                             ref_tfc_id_valid;

  /* Data channel gain. Valid if signalled.
   * See 25.331 v9.1.0, 10.3.5.8 */
  uint8                               BetaD;
} tdsl1_ul_gain_factor_parms_struct_type;


/* UL CCTrCH items */
typedef struct {
  /* uplink physical channel type */
  tdsl1_ul_phychan_enum_type                chan;

  /* semi-static parameters for valid transport channels */
  tdsl1_ul_semi_static_parms_struct_type   *semi_static_parms_ptr[TDSUE_MAX_TRCH];

  /* gain factor parameters, indexed by TFC id */
  tdsl1_ul_gain_factor_parms_struct_type   *gain_factor_parms_ptr[TDSL1_UL_TFC_MAX];

  /* Maps reference TFC IDs to real TFC IDs for computed gain factors. */
  uint8                                     ref_tfc_to_real_tfc[TDSL1_UL_REF_TFC_MAX];

  /* filler value for code segments after radio frame equalization */
  tdsl1_ul_filler_polarity_enum_type        filler_val;
} tdsl1_ul_cctrch_cfg_struct_type;


/***************************************************************************
**                                                                       
**     HSDPA related configuration                  
**                                                                       
***************************************************************************/
 
 
#define TDSL1_CPHY_HS_DSCH_CFG_INCL              0x0001
#define TDSL1_CPHY_HS_SCCH_SICH_CFG_INCL         0x0002
#define TDSL1_CPHY_HS_PDSCH_MIDAMBLE_CFG_INCL    0x0004
   
#define TDSL1_CPHY_HS_CFG_REQ_IS_CFG_INCL(mask, cfg) \
  (((mask) & TDSL1_CPHY_HS_##cfg##_CFG_INCL) ? TRUE : FALSE)

/* zhouya add for 10.3.6.23a begin */
#define TDS_MAX_HS_SCCH_SICH_COUNT 4

#define TDSL1_CPHY_HS_CFG_ALL_INCL \
  (TDSL1_CPHY_HS_DSCH_CFG_INCL | \
  TDSL1_CPHY_HS_SCCH_SICH_CFG_INCL | \
  TDSL1_CPHY_HS_PDSCH_MIDAMBLE_CFG_INCL)
  
/*HS-DSCH struct information*/
typedef struct
{
  /*brief Number of HARQ processes,1..8 */
  uint8   num_proc;
  
  /*brief indicate if memory partitioning is specified explicit by N/W */
  boolean mem_part_info_valid;

  /*brief HARQ process mem partition information, Valid only if mem_part_info_valid is valid */
  tdsl1_dl_hs_dsch_harq_mem_sz_enum_type   mem_size_type[TDSL1_DL_CCTRCHCFG_HS_DSCH_HARQ_MAX_PROC];
} tdsl1_hs_dsch_harq_info_struct_type;

/*This structure defines MAC-d PDU size information for given MAC HS queue */
typedef struct
{
  /*brief PDU size index in MAC HS header */
  uint8  pdu_sz_idx;

  /*brief PDU size */
  uint16 pdu_sz;
} tdsl1_hs_dsch_mac_d_pdu_sz_info_struct_type;

/*This structure defines single MAC HS queue information */
typedef struct
{
  /*brief MAC HS queue Id */
  uint8 mac_hs_q_id;

  /*brief number of MAC-d PDU sizes for this queue */
  uint8 num_mac_d_pdu_sz;

  /*brief MAC PDU size type information for this queue */
  tdsl1_hs_dsch_mac_d_pdu_sz_info_struct_type   mac_d_pdu_sz_info[TDSL1_DL_MACD_PDU_SZ_PER_Q_MAX_COUNT];
} tdsl1_hs_dsch_mac_hs_q_info_struct_type;

/*This structure defines MAC HS queue information */
typedef struct
{
  /*brief Number of MAC HS queues */
  uint8 num_mac_hs_q;

  /*brief Each MAC HS queue information */
  tdsl1_hs_dsch_mac_hs_q_info_struct_type mac_hs_q_info[TDSL1_DL_CCTRCHCFG_MAC_HS_Q_MAX_COUNT];
} tdsl1_hs_dsch_mac_d_flow_info_struct_type;


/* This structure defines hs dsch information */
typedef struct
{
  /*brief  CTrCh Id for HS DSCH TrCh. The possible value can be 6 and 7 */
  uint8  cctrch_id;

  /*brief  H-RNTI, this is 16 bits in length */
  uint16 h_rnti;

  /*brief  HARQ information */
  tdsl1_hs_dsch_harq_info_struct_type       harq_info;

  /*brief  octet aligned or bit-aligned for TB */
  tdsl1_dl_cctrchcfg_hs_dsch_tb_size_alignment_enum_type hsdsch_tb_size_alignment_type;

  /*brief  MAC-d information. This is used for deciding mac HS header only */
  tdsl1_hs_dsch_mac_d_flow_info_struct_type macd_info;

 #ifdef FEATURE_TDSCDMA_HSDPA_MAC_EHS
  /*Informs MAC if MAC HS is configured or MAC EHS */
  mac_hs_e_type mac_hs_or_ehs;


  /* If config is EHS, then only interpret the following EHS info.
   * Otherwise, HS info is valid for this cmd 
   */

  /* This field contains the queue id for each logical channel mapped 
   * to MAC-Ehs. If the logical channel is not in use, then the corresponding
   * queue mapping should be set by RRC to TDSINVALID_QUEUE_ID(value 8). 
   */
  uint8 mac_ehs_dlchan_q_info[TDSL1_DL_CCTRCHCFG_MAX_LOGICAL_CHANNEL];
 #endif
 
 #ifdef FEATURE_TDSCDMA_64QAM
  boolean hs_64_qam_configured;
 #endif

} tdsl1_hs_dsch_info_struct_type;


/*--------------------------------------------------------------------------
          HS-SCCH (HS-SICH) Info
--------------------------------------------------------------------------*/
/** HS-SICH channel info */
typedef struct
{
  /*brief time slot number of HS-SICH */
  uint8 ts_num_sich;
  
  /*brief channelisation code for HS-SICH */
  tdsl1_dl_ch_code_enum_type sich_chcode;
  
  /*brief midamble configuration number HS-SICH */
  tdsl1_midamble_ss_burst_struct_type midamble_config_sich;
  
  /*brief Ack-Nack power offset, -7..8, applied to REL-5 */
  int16  ack_nack_powoffset;
  
  /*brief in dBm. desired power level for HS-SICH, -120.. -58, applied to REL-5 */
  int16 prx_sich;
  
  /*brief TPC Step Size, 1,2,3, applied to REL-5 */
  tdsl1_tpc_step_size_enum_type tpc_ssize;  
} tdsl1_hs_sich_config_struct_type;


/* HS-SCCH channel info */
typedef struct
{
  /*brief time slot number of HS-SCCH */
  uint8  ts_num_scch;
  
  /*brief first channelisation code for HS-SCCH */
  tdsl1_dl_ch_code_enum_type           scch_chcode_1st;
  
  /*brief second channelisation code for HS-SCCH */
  tdsl1_dl_ch_code_enum_type           scch_chcode_2nd;
  
  /*brief HS-PDSCH midamble configuration  */
  tdsl1_midamble_ss_burst_struct_type          hs_scch_midamble_cfg;
  
  /*brief HS-SCCH BLER target, -63..0, actual value = 0.05 * blertarget, applied to REL-5 */
  int32  bler_target;
  
  /*brief Corresponding HS-SICH info per HS-SCCH*/
  tdsl1_hs_sich_config_struct_type             l1_sich_config;  
}tdsl1_hs_scch_set_config_struct_type;

/** HS-SCCH Info: up to 4 HS-SCCH/SICH pairs */
typedef struct
{
  /*brief Ack-Nack power offset, -7..8, applied to REL-6 */
  int16   ack_nack_powoffset;
  
  /*brief desired power level for HS-SICH in dBm, -120.. -58, applied to REL-6 */
  int16   prx_sich;
  
  /*brief TPC Step Size, 1,2,3, applied to REL-6 */
  tdsl1_tpc_step_size_enum_type tpc_ssize;
  
  /*brief HS-SCCH BLER target, -63..0, actual value = 0.05 * blertarget, applied to REL-6 */
  int32   bler_target;
  
  /*brief power control gap in subfrmae numbers, 1..255, applied to REL-6 */
  uint16  pow_control_gap;
  
  /*brief pathloss compensation swich, applied to REL-7 */
  boolean do_pathloss_compens;
  
  /*brief number of HS-SCCH and HS-SICH pairs */
  uint8  num_hs_scch_sich;
  
  /*brief HS-SCCH and HS-SICH physical channel set configuration info */
  tdsl1_hs_scch_set_config_struct_type l1_dl_hs_scch_sich_config[TDS_MAX_HS_SCCH_SICH_COUNT];  
} tdsl1_hs_scch_sich_info_common_params_struct_type;

/* HSDPA new data struct for TDS */
typedef struct
{
  /*brief req mask for HS configuration */
  uint8  hs_req_mask;
  
  /*brief UE category information */
  uint8  hs_cat;
  
  /*brief Indicator whether the current IE using shared ck_nack_powoffset, tpc_step_size, bler_target, prx_sich etc */
  boolean  common_scch_sich_params_present;
  
  /*brief HS-PDSCH midamble configuration  */
  tdsl1_midamble_ss_burst_struct_type  hs_pdsch_midamble_cfg;
  
  /*brief L1 DL HS DSCH TrCh info */
  tdsl1_hs_dsch_info_struct_type  hs_dsch_info;
  
  /*brief L1 DL HS-SCCH/SICH physical channel combo info for Rel 5 and Rel 6 above */
  tdsl1_hs_scch_sich_info_common_params_struct_type  hs_scchset_info ;
}tdsl1_hs_info_struct_type;


/***************************************************************************
**                                                                       
**     HSUPA related configuration                     
**                                                                       
***************************************************************************/

#ifdef FEATURE_TDSCDMA_HSUPA


/* e_req_mask bit positions/value */

/* No operation needed for E_DL and E_UL*/
#define  TDSL1_EUL_NOOP                 0x00

/* START: Indicates that its the first time EUL channels are setup. RRC should 
** indicate all the necessary UL/DL configuration. 
** RRC will set START when atleast one of the following is met:
**  1. when EUL is setup for the first time
**  2. CphyEstablishmentFailure - reverting back to old config (E-DCH->DCH)
**  3. CphySetupFailure(functional failure) - reverting back to old config (E-DCH->DCH) 
*/

#define  TDSL1_SCHED_EUL_START                0x01

/* RECFG_UL: If EUL is up, RRC will set bit L1_HSUPA_RECFG_UL whenever there
** is any change in E-UL config.
** RRC will set RECFG-UL when atleast one of the following is met:
**  1. Atleast one of E-UL parameter is modified (parameters include TTI)
**  2. CphyEstablishmentFailure - reverting back to old config (E-DCH->E-DCH)
**  3. CphySetupFailure(functional failure) - reverting back to old config (E-DCH->E-DCH) 
*/
#define  TDSL1_EUL_RECFG_UL             0x02

/* RECFG_DL: If EUL is up, RRC will set bit L1_HSUPA_RECFG_DL whenever there
** is any change in E-DL config. 
** RRC will set RECFG-DL when atleast one of the following is met:
**  1. Whenever any E-DCH active set RL goes out of E-DCH active set,
**  2. New RL gets added to E-DCH active set
**  3. Simultaneous add and remove of RL to E-DCH active set
**  4. Change of E-DCH Serving RL
**  5. CphyEstablishmentFailure - reverting back to old config (E-DCH->E-DCH)
**  6. CphySetupFailure(functional failure) - reverting to old config (E-DCH->E-DCH), 
**     when config message results in deletion of one or more E-DCH active set RL.
**  7. Whenever E-DCH active set RL is removed from Active Set 
**  8. Whenever E-channel(AGCH/RGCH/HICH) parameters associated with E-DCH
**      active set RL is modified
**  9. Whenever any of the E-channels associated with E-DCH Active Set RL 
**      is added/removed
** 10. TTI reconfiguration
** 11. (P/S)E-RNTI is added or modified

** NOTE: When E-HICH is removed from any of E-DCH active set RL and/or
**          E-DCH active set RL is removed from the active set, then 
**          E-DCH RL info for that RL(s) will be removed from e_rl_info[TDSL1_MAX_EDCH_RL] 
**          and added in e_rl_release_info[] 
*/

#define  TDSL1_SCHED_EUL_RECFG_DL       0x04
/* STOP: Layer1 is expected to release/stop all E-DCH Channels. 
**   RRC will set STOP whenever one of following is met:
**     1. UE moves from E-DCH to non-DCH(FACH/Cell-PCH/URA-PCH) state
**     2. NW explicitly indicates UE to stop E-DCH
**     3. CphyEstablishmentFailure: reverting back to old config (DCH->E-DCH) 
*/
#define  TDSL1_EUL_STOP                 0x08
/*
**RRC set TDSL1_NON_SCGED_GRANT_START when first time configure non sched grant to L1   
** If non sched grant is rconfigured or stop. RRC set it same as E-UL    
*/
#define TDSL1_NON_SCHED_GRANT_START     0x10

#define TDSL1_NON_SCHED_GRANT_RECFG     0x20

/*This macro gives the bits that are valid for EUL check in a uint8*/
#define TDSL1_EUL_MASK                  0x3F



/* e_dl_channel_mask bit position definition */
#define TDSL1_E_AGCH_INFO_INCL             0x01
#define TDSL1_E_HICH_INFO_INCL             0x02
#define TDSL1_ERNTI_PRESENT                0x04


/*e_ul_info_mask bit position definition*/
#define TDSL1_EPUCH_INFO_INCL             0X01
#define TDSL1_ERUCCH_INFO_INCL            0X02
#define TDSL1_HARQ_RV_INFO_INCL           0X04
  


#define TDSL1_MAX_REF_E_TFCI_LIST         8

typedef enum
{
  TDSL1_E_GRANT_SELECTOR_NOT_PRESENT,
  TDSL1_E_PRIMARY_GRANT,
  TDSL1_E_SECONDARY_GRANT
} tdsl1_e_pri_sec_grant_selector_enum_type;

/* HARQ RV configuration: Ref: 10.3.5.7d in 25.331*/
typedef enum
{
  /* initialized with this value */
  TDSL1_RV_NOT_PRESENT, 
  /* use RSN based RV index */
  TDSL1_RV_TABLE, 
  /* UE shall only use RV index 0 */
  TDSL1_RV_0
} tdsl1_harq_rv_config_enum_type;

/*E_DL channel channelisation code enum type*/
typedef enum
{
  TDSL1_EDL_SF16_CH_CODE_1,
  TDSL1_EDL_SF16_CH_CODE_2,
  TDSL1_EDL_SF16_CH_CODE_3,
  TDSL1_EDL_SF16_CH_CODE_4,
  TDSL1_EDL_SF16_CH_CODE_5,
  TDSL1_EDL_SF16_CH_CODE_6,
  TDSL1_EDL_SF16_CH_CODE_7,
  TDSL1_EDL_SF16_CH_CODE_8,
  TDSL1_EDL_SF16_CH_CODE_9,
  TDSL1_EDL_SF16_CH_CODE_10,
  TDSL1_EDL_SF16_CH_CODE_11,
  TDSL1_EDL_SF16_CH_CODE_12,
  TDSL1_EDL_SF16_CH_CODE_13,
  TDSL1_EDL_SF16_CH_CODE_14,
  TDSL1_EDL_SF16_CH_CODE_15,
  TDSL1_EDL_SF16_CH_CODE_16,
  TDSL1_EDL_INVALID_CH_CODE = 0xff
} tdsl1_edl_ch_code_enum_type;

typedef enum
{
  TDSEDL_TPC_STEP_SIZE_1 = 1 ,
  TDSEDL_TPC_STEP_SIZE_2 ,
  TDSEDL_TPC_STEP_SIZE_3 
} tdsl1_edl_tpc_step_size_enum_type;

/*E-AGCH set configuration type*/
typedef struct
{
  /*time slot number (0..6)*/
  uint8 ts_num ;
  
  /*first channelisation code*/
  tdsl1_edl_ch_code_enum_type first_code ;
  
  /*second channelisation code*/
  tdsl1_edl_ch_code_enum_type second_code ;
  
  tdsl1_midamble_ss_burst_struct_type eagch_midamble ;
} tdsl1_eagch_set_info_struct_type ;

#define TDSL1_EDL_MAX_NUM_EAGCH 4

typedef struct
{
  

  /*TDSCDMA HSUPA. Refer to 3GPP 25.331 10.3.6.100
  **If TRUE, RDI will present in E-AGCH payload
  */
  boolean rdi_pre ;
  
  /*TPC step size. 1,2,3*/
  tdsl1_edl_tpc_step_size_enum_type tpc_step ;
  
  /*E-AGCH Bler Target. -3.15 to 0 step 0.05. 0 mapped to bler target and -63 mapped to -3.15*/
  int8 bler_target ;
  
  /*Number of E-AGCH Channels. 1...4*/
  uint8 num_eagch ;
  
  /*E-AGCH time slot info */
  tdsl1_eagch_set_info_struct_type eagch_slot_info[TDSL1_EDL_MAX_NUM_EAGCH] ;
} tdsl1_e_agch_info_struct_type;



typedef struct
{
  /*time slot number (0..6)*/
  uint8 ts_num ;
  
  /*E-HICH index*/
  uint8 ei ;
  
  /*channelisation code*/
  tdsl1_edl_ch_code_enum_type chan_code ;
  
  tdsl1_midamble_ss_burst_struct_type ehich_midamble ;
} tdsl1_sch_ehich_set_info_struct_type ;

#define TDSL1_EDL_MAX_NUM_SCH_EHICH 4
typedef struct
{
  
  /*TDSCDMA HSUPA. Refer to 3GPP 25.331 10.3.6.101
  **Minimum number of time slots between E-PUCH and E-HICH. 4...15 
  */
  uint8 n_e_hich ;
  
  /*Number of scheduled E-HICH channels. 1...4*/
  uint8 num_ehich ;
  
  tdsl1_sch_ehich_set_info_struct_type sched_e_hich_slot_info[TDSL1_EDL_MAX_NUM_SCH_EHICH] ;
} tdsl1_sched_e_hich_info_struct_type;

typedef struct
{

  tdsl1_e_agch_info_struct_type e_agch_info;
  
  tdsl1_sched_e_hich_info_struct_type e_hich_info;
} tdsl1_e_dl_per_rl_info_struct_type;


/*TD-SCDMA non scheduled grant info*/
typedef struct
{
  /*Refer to 3GPP 25.331 10.3.6.41c*/
  /*Time slot number*/
  uint8 ts_num ;
  
  /*Channelisation code*/
  tdsl1_edl_ch_code_enum_type chan_code ;
  
  /*Midamble code*/
  tdsl1_midamble_ss_burst_struct_type mid_code ;
  
  /*E-HICH signature group index. 0...19*/
  uint8 sig_group_index ;
} tdsl1_non_sched_e_hich_info_struct_type ;

typedef struct
{
  /*Refer to 3GPP 25.331 10.3.6.41c
  **Number of E-UCCH instance in one TTI . 1...8
  */
  uint8 num_e_ucch;

  /*Minimum of time slot  number between E-PUCH and E-HICH for non scheduled transmission. 4..15*/
  uint8 n_e_hich ;

  /*time slot resource related info. bit 4 indicated if time slot 1 is used. 
  **bit 3 indicates time slot 2.... bit 0 indicates if time slot 5 is used 
  */
  uint8 trri ;

  /*power resource related info. this is  grant power index 1...32*/
  uint8 prri ;

  /*code resource related info. channelization code index */
  uint8 crri ;

  /*activation time. CFN number*/
  uint8 activation_time ;

  /*sub frame number 0 or 1*/
  uint8 subframe_num ;

  /*repetition period . 1,2,4,8,16,32,64. I means grant is continuous*/
  uint8 rep_period ;

  /*repetition length*/
  uint8 rep_length ;

  /*non scheduled E-HICH info*/
  tdsl1_non_sched_e_hich_info_struct_type non_sched_e_hich_info ;
} tdsl1_non_sched_grant_info_struct_type ;

typedef struct
{

  /*TDSCDMA HSUPA*/
  uint16 e_rnti ;

 /* e_dl_channel_mask bit position definition 
#define TDSL1_E_AGCH_INFO_INCL             0x01
#define TDSL1_E_HICH_INFO_INCL             0x02   
#define TDSL1_ERNTI_PRSENT                 0x04 
*/
  uint16 e_dl_channel_mask;
  tdsl1_e_dl_per_rl_info_struct_type e_dl_per_rl_info ;

} tdsl1_e_dl_info_struct_type;

typedef struct
{
 
  /*Added for TDSCDMA HSUPA
  **reference code rate. 0...10. Mapped to code rate from 0 to 1.0 in step 0.1
  */
  uint8 ref_code_rate ;

  /*reference beta. from -15dB to 16dB*/
  int8 ref_gain_factor ;
} tdsl1_ref_e_tfci_struct_type;

typedef enum
{
  TDSL1_SNPL_REP_TYPE_1,
  TDSL1_SNPL_REP_TYPE_2,
  TDSL1_SNPL_REP_TYPE_NONE 

} tdsl1_snpl_rep_enum_type;

typedef struct
{
  /*time slot number. 1...5*/
  uint8 ts_num ;
  
  /*Middleamble info*/
  tdsl1_midamble_ss_burst_struct_type midamble_info ;
} tdsl1_epuch_ts_info_struct_type ;

typedef struct
{
  /*Number of reference ETFCI for QPSK. 1..8*/
  uint8 num_ref_e_tfci_qpsk ;
  
  /*ETFCI refeence table for QPSK*/
  tdsl1_ref_e_tfci_struct_type ref_e_tfci_list_qpsk[TDSL1_MAX_REF_E_TFCI_LIST] ;
  
  /*Number of reference ETFCI for 16QAM. 1..8*/
  uint8 num_ref_e_tfci_16qam ;
  
  /*ETFCI refeence table for QPSK*/
  tdsl1_ref_e_tfci_struct_type ref_e_tfci_list_16qam[TDSL1_MAX_REF_E_TFCI_LIST] ;
  
  /*SNPL report type*/
  tdsl1_snpl_rep_enum_type snpl_rep_type ;
  
  /*PRX_DES_BASE. -112dbm ...-50dbm in step of 1dbm*/
  int8 prx_des_base ;
  
  /*if Beacon path loss estimation is used or not. */
  boolean beacon_pl_est ;
  
  /*E-PUCH TPC step size.1,2,3*/
  tdsl1_edl_tpc_step_size_enum_type tpc_step;
  
  /*Pebase power control  gap. 1...255. 255 means infinite and closed loop power control is always used*/
  uint8 p_base_contr_gap ;
  
  /*UL synchronization step size.1...8*/
  uint8 ul_sync_step ;
  
  /*UL synchronization frequency. 1...8*/
  uint8 ul_sync_freq ;
  
  /*Minimumm code rate. index 0..63. mapped to 0.055 to 1.0 in step of 0.055*/
  uint8 min_code_rate ;
  
  /*Maximumm code rate. index 0..63. mapped to 0.055 to 1.0 in step of 0.055*/
  uint8 max_code_rate ;
  
  /*Number of E-PUCH time slots*/
  uint8 num_epuch_ts ;
  
  tdsl1_epuch_ts_info_struct_type epuch_ts_cfg_list[TDSL1_MAX_EPUCH_TS] ;
} tdsl1_e_puch_info_struct_type ;


/*SYNC_UL info for E-RUCCH, refer to 25.331 10.3.6.78d */
typedef struct
{
  /* Indicate what UL sync codes are available, 25.331 10.3.6.78o */
  uint8                             ul_sync_code_mask;
  
  /* Desired UpPCH received power, in dBm (-120..-58) 
  ** See 25.331, 10.3.6.78d 
  */
  int8                              prx_upPch_des;
  
  /* UL sync code power ramp step when no FPACH received after sent SYNC_UL. 
  ** The value is (0, 1, 2, 3) in dB.  See 25.331: 10.3.6.78d 
  */
  uint8                             pwr_ramp_step;
  
  /* Maximum numbers of SYNC_UL transmissions.
  ** The value is (1, 2, 4, 8). 
  ** See 25.331: 10.3.6.78d 
  */
  uint8                             max_sync_ul_tx;
} tdseulcfg_sync_ul_info_erucch;


/*E-RUCCH info */
typedef struct
{
  /*E-RUCCH access service class setting */
  tdsl1_ul_asc_cfg_struct_type asc_cfg ;

  /*sync UL code info setting*/
  tdseulcfg_sync_ul_info_erucch sync_ul_code_info_erucch ;

  /*number of PRACH_FPACH*/
  uint8 num_prach_fpach ;
  
  /* UpPCH shift, in 16chips (0..127)
   * See 25.331 v9.1.0, 10.3.2.5 and 10.3.6.39a */
  uint8 upPch_shift;

  /*PRACH_FPACH info array . Max Array size is 8*/
  tdsl1_ul_prach_fpach_struct_type prach_fpach_info[TDSL1_UL_PRACH_FPACH_MAX] ;
} tdsl1_e_rucch_info_struct_type ;

typedef struct
{
 
/*e_ul_info_mask bit position definition
#define TDSL1_EPUCH_INFO_INCL             0X01
#define TDSL1_ERUCCH_INFO_INCL            0X02
#define TDSL1_HARQ_RV_INFO_INCL             0X04
*/
  uint16 e_ul_info_mask ;

  tdsl1_harq_rv_config_enum_type harq_rv_config;

  tdsl1_e_puch_info_struct_type e_puch_info ;
  
  tdsl1_e_rucch_info_struct_type e_rucch_info ;
} tdsl1_e_ul_info_struct_type;

typedef struct
{
  tdsl1_e_ul_info_struct_type e_ul_info;

  tdsl1_e_dl_info_struct_type e_dl_info;

  tdsl1_non_sched_grant_info_struct_type non_sched_grant_info ;
} tdsl1_e_info_struct_type;

#endif /* FEATURE_TDSCDMA_HSUPA */



#define TDSL1_CPHY_HHO_NOT_A_HHO_INCL 0x0001
#define TDSL1_CPHY_HHO_INTERF_TI_HHO_INCL 0x0002
#define TDSL1_CPHY_HHO_INTERF_TM_HHO_INCL 0x0004
#define TDSL1_CPHY_HHO_INTRAF_TI_HHO_INCL 0x0008
#define TDSL1_CPHY_HHO_INTRAF_TM_HHO_INCL 0x0010
#define TDSL1_CPHY_HHO_FACH_TO_DCH_INTERF_TI_HHO_INCL 0x0020
#define TDSL1_CPHY_HHO_RESUME_TO_OLD_CONFIG_HHO_INCL 0x0100

  
#define TDSL1_CPHY_VOICE_CALL_ACTIVE 0x0001
#define TDSL1_CPHY_R99_PS_CALL_ACTIVE 0x0002
#define TDSL1_CPHY_HS_CALL_ACTIVE 0x0004
#ifdef FEATURE_TDSCDMA_DSDA
#define TDSL1_CPHY_HIGH_PRIORITY_SIGNALING_ON_DSDA  0x0010
#endif

typedef struct
{
  /* TRUE means L1 should call l1_post_ul_mac_config_sig() and/or
  ** l1_post_dl_mac_config_sig() at the activation time, so MAC can start
  ** its configuration process too. 
  */
  boolean  signal_ul_mac_at_act_time;

  boolean  signal_dl_mac_at_act_time;

  /* a bit mask that indicates which of the sub-primitives are included 
  ** in the CPHY_SETUP_REQ.  See the comments for the mask bit defines 
  ** for more details. 
  */
  uint16 req_mask;
#ifdef FEATURE_TDSCDMA_DSDS
  /* Indicate TL1 whether to release lock;
   * TRUE -- need to release lock
   * FALSE -- no need to release lock
   */
  boolean release_lock;
#endif
  /* This mask is used by RRC to indicate whethere this CPHY_SETUP_REQ 
     is used to restore old configuration from failed HO attempt. if yes
     more detail information about the previous HO attempt

  ** bit15(MSB) bit14 bit13 .........bit0(LSB)
  **
  ** bit0 (LSB)  .........1 HO 
  **                          0 No HO
  ** bit1 ...................1 HHO
  **                          0 Baton HO
  ** bit2 ...................1 for Timing Reinit
  **                           0 for Timing Maintainened
  ** bit3 - bit 7 .........reserved
  ** bit8 ...................1 revert back to old configuration
  **                          0 not revert back
  */
  uint16 ho_setup_mask;

  /* RRC will set the bit based on what type of call is active and 
  ** what type of call is being setup.
  ** bit7(MSB) bit6 bit5 .........bit0(LSB)
  ** bit0 ...................1 Voice call  
  ** bit1 ...................1 Release 99 PS data call
  ** bit2 ...................1 HS call 
  ** bit3 ...................1 MBMS channel
  ** bit4-7 ...................reserved 
  */
  uint8 call_type_mask;

  /*Primary frequency - cell's primary frequency. See  25.331:10.3.6.36 and 25.102*/
  uint16  primary_freq; 
  
  /* TRUE - working frequency included*/
  boolean working_freq_incl;
  
  /*Working frequency. If working frequency is included, cell shall work at the frequency.*/
  uint16  working_freq; 
  
  /*Cell parameters ID (see 25.223:7.3)*/
  uint8  cell_parm_id;

  /*X2TD IRAT HHO flag*/
  boolean is_x2td_irat_hho;

  /* Maximum allowed Tx power in dBm (-50..33)
   * See 25.331 v9.1.0 10.3.6.39*/
  int8   max_tx_pwr;

  /* TRUE if the recfg doesn't cause any Trch config, Rb Config or any CFN change. */
  boolean seamless_recfg_ind;

  /* data for CPHY_DL_CCTRCH_REQ */
  tdsl1_dl_cctrchcfg_cctrch_info_struct_type   *dl_cctrch_info;
  
  tdsl1_dl_cctrchcfg_ctfc_info_struct_type     *dl_ctfc_info;

  /* data for CPHY_UL_CCTRCH_REQ */
  tdsl1_ul_cctrch_cfg_struct_type *ul_cctrch_info;

  /* data for CPHY_UL_PHYCHAN_DROP_REQ */
  tdsl1_ul_phychan_enum_type drop_ul_phychan;

  /* data for CPHY_DL_PHYCHAN_DROP_REQ */
  tdsl1_dl_phychan_enum_type drop_dl_phychan;

  /* data for CPHY_DL_PHYCHAN_CFG_REQ.  cfg_dl_phychan is the downlink
  ** physical channel to be reconfigured.  It is assumed that it will
  ** be reconfigured to use the CCTrCH referred to in the CPHY_DL_CCRTCH_REQ
  ** also included in the command packet
  */
  tdsl1_dl_phychan_enum_type cfg_dl_phychan;

  tdsl1_dl_phycfg_phychan_db_struct_type *cfg_dl_phychan_db;

  /* data for CPHY_UL_PHYCHAN_CFG_REQ.  cfg_ul_phychan is the uplink physical
  ** channel to be reconfigured.  It is assumed that it will be reconfigured
  ** to use the CCTrCH referred to in the CPHY_UL_CCTRCH_REQ also included
  ** in the command packet
  */
  tdsl1_ul_phychan_enum_type cfg_ul_phychan;

  tdsl1_ul_phych_cfg_struct_type  *cfg_ul_phychan_db;

  /* data for CPHY_DL_PHYCHAN_ADD_REQ */
  tdsl1_dl_phychan_enum_type add_dl_phychan;
  
  tdsl1_dl_phycfg_phychan_db_struct_type *add_dl_phychan_db;

  /* data for CPHY_UL_PHYCHAN_ADD_REQ */
  tdsl1_ul_phychan_enum_type add_ul_phychan;
  
  tdsl1_ul_phych_cfg_struct_type  *add_ul_phychan_db;

  tdsmac_ul_tfcs_type  *tfcs_ptr;

  /* --------------------------- 
  ** HSDPA related configuration 
  ** --------------------------- */
  /* HSDPA operation type for L1 */
  tdsl1_hsdpa_setup_ops_enum_type hsdpa_l1_ops;
  
  /* HSDPA information. If NULL then it is assumed that HSDPA information is
  ** not present 
  */
  tdsl1_hs_info_struct_type *hsdpa_l1_info;

#ifdef FEATURE_TDSCDMA_HSUPA
  /* Indicates L1 to flush all HARQs.
  ** RRC will set this to TRUE under following conditions:
  **    - NW explicitly indicate in the reconfig message
  **    - whenever TTI reconfiguration happens 
  **  NOTE: L1 is expected to look into this variable ir-respective of the value
  **        of 'e_req_mask'. 
  */
  boolean mac_e_reset_indicator;

  /* req mask for EUL configuration 
  ** This request mask will outline the action related to HSUPA UL and DL 
  ** bitmask:
  ** TDSL1_SCHED_EUL_START                0x01
  ** TDSL1_EUL_RECFG_UL                   0x02
  ** TDSL1_SCHED_EUL_RECFG_DL             0x04
  ** TDSL1_EUL_STOP                       0x08 
  ** TDSL1_NON_SCHED_GRANT_START          0X10 
  ** TDSL1_NON_SCHED_GRANT_RECFG          0X20 
  */
  uint8 e_req_mask;
 
  tdsl1_e_info_struct_type *e_info_ptr;
#endif /* FEATURE_TDSCDMA_HSUPA */

  /*+ Legacy for TDS UL/DL dev, to be deleted later */
  /* whether rf frequency information is included or not */
  boolean freq_incl;
  
  /* the rf channel if included. */
  uint16 rf_chan;
 
#ifdef FEATURE_TDSCDMA_DMO_SUPPORT
/* cellDCHMeasOccasionInfo-TDD128 CellDCHMeasOccasionInfo-TDD128-r9 OPTIONAL */
  boolean                        meas_occasion_info_incl;
  tdsl1_meas_occasion_info_struct_type    *meas_occasion_info;
#endif

#ifdef FEATURE_TDSCDMA_MTPL_THERMAL_BACKOFF
 boolean                         is_emergency_call;
#endif

} tdsl1_setup_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_DRX_REQ

Used by RRC to command physical layer to begin DRX operations.  
--------------------------------------------------------------------------*/
typedef struct
{
  /* The 2^K value of the DRX cycle length. 
  ** The UE will sleep for 2^K frames.      
  */
  uint32       drx_cycle_len;
} tdsl1_drx_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_CIPHERING_KEY_REQ
                       
Parameter structure for the TDSL1_CPHY_CIPHERING_KEY_REQ primitive.                       
--------------------------------------------------------------------------*/
typedef struct 
{
  tdsl1_ciphering_key_req_type cipher_key_req;
} tdsl1_ciphering_key_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_CELL_SELECTION_REQ

Used by RRC to request Cell Selection values S for specified cells.   
--------------------------------------------------------------------------*/
typedef struct
{
  /* Frequency specification */
  uint16 freq;
  /* Primary Scrambling Code Index */
  uint16 prim_code_inx;
} tdsl1_cell_selec_cmd_type;



/*--------------------------------------------------------------------------
          TDSL1_CPHY_IMMEDIATE_MEAS_REQ

Used by RRC to request an immediate measurement report at FACH state. 
This is implemented as a function.
--------------------------------------------------------------------------*/
/*it will replace l1_immediate_meas_type in future*/
typedef struct
{
  /* Scrambling code of the serving cell */
  uint16  aset_cpi;
    
  uint8    meas_quan_list_no;

  /* measurement quantity type */
  tdsl1_meas_quan_enum_type meas_quan_list[L1_RACH_MAX_MEAS_LIST_NO];

  /* measurement value for the serving cell 
  **add new fields for ISCP
  */
  int16 aset_rscp_val;

  uint8  ts_num;

  struct
  {
    uint8   ts_no;

    uint16  iscp_val;
  }iscp_ts_list[L1_MAX_TS];

  /* Number of neighbors being reported  Upto 6 --10.3.7.43*/
  uint16 num_ncells;
    
  struct
  {
    /* scrambling code for the cell  
    **in fach state,only serve cell's ISCP will be measured
    **so for neighbor cell,only RSCP can be reported
    */
    uint16 cpi_code ;
  
    /* measurement quantity type */
    tdsl1_meas_quan_enum_type meas_quan;

    /* measured value for the cell  */
    uint16 rscp_val;

    /* SFN-SFN observed time diff reporting indicator */
    tdsl1_sfn_report_enum_type sfn_rpt;
  
    /*  SFN-SFN difference  */
    uint32 sfn_sfn_diff;    
  } n_cells[ TDSL1_INTRA_MAX_IN_RACH ];
  
#ifdef FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING
  /* Add the inter freq cell info here */ 
  uint16 num_interf_freqs ;

  struct 
  { 
    uint16 freq;

    uint16 num_cells;

    uint16 cell_psc[TDSL1_RACH_INTERF_NSET_MAX];
  } n_interf_cells [TDSL1_MAX_NON_USED_FREQ_TMP];
#endif /*FEATURE_TDSCDMA_INTER_FREQ_RACH_REPORTING*/
} tdsl1_immediate_meas_type_tmp;

/*--------------------------------------------------------------------------
          TDSRRC_CPHY_MEAS_CTRL_CNF
Used to indicate if meas ctrl message command from RRC is valid in response 
to a TDSL1_CPHY_MEASUREMENT_REQ. Status is true or false
---------------------------------------------------------------------------*/

typedef struct
{
   /* Transaction Identifier */
   uint32 trans_id;

   /* Measurement Identity Number */
   uint16 meas_id;

   /* Status (Valid?), true or false */
   boolean status;

   /* Reason Invalid */
   tdsl1_failure_cause_enum_type error_cause;  
}  tdsl1_meas_ctrl_cnf_type;



/*--------------------------------------------------------------------------
          TDSL1_CPHY_CELL_BAR_REQ

Used by RRC to bar a cell.
--------------------------------------------------------------------------*/

/* This is used to inform L1 if the GSM cell is barred, or WCDMA 
** cell is barred, or WCDMA frequency is barred, or the active
** set is barred
*/
typedef enum
{
  /* ZZTBD: TDSL1_WCDMA_xxx not to be used for TDD */
  //TDSL1_WCDMA_NOT_BARRED,
  TDSL1_GSM_CELL_BARRED
  //TDSL1_WCDMA_PSC_BARRED,
  //TDSL1_WCDMA_FREQ_BARRED,
  //TDSL1_WCDMA_ASET_BARRED
#ifdef FEATURE_TDSCDMA_TO_LTE
  ,TDSL1_EUTRA_CELL_BARRED
  ,TDSL1_EUTRA_FREQ_BARRED
#endif
  ,TDSL1_TDS_NOT_BARRED
  ,TDSL1_TDS_CPID_BARRED
  ,TDSL1_TDS_FREQ_BARRED
  ,TDSL1_TDS_ASET_BARRED /* May not be needed for TDD */
}tdsl1_cell_bar_info_e_type;


typedef struct 
{
  /* ARFCN and BSIC uniquely identify GSM cell*/
  W_ARFCN_T                   bcch_arfcn;
  
  tdsl1_gsm_bsic_struct_type     bsic;
}tdsl1_gsm_cell_identity_struct;

/*
typedef struct
{
  uint16 freq;

  uint16 scr_code;
}tdsl1_fdd_cell_identity_struct;
*/

typedef struct
{
  /* Frequency and CPID uniquely identify a TDD cell*/
  uint16 freq;

  uint8 cpid;
}tdsl1_tdd_cell_identity_struct;

typedef struct
{

  /* T barred timer value */
  uint16 t_barred;      
  /* This is used to inform L1 if the GSM cell is barred, or WCDMA 
  ** cell is barred, or WCDMA frequency is barred, or the active
  ** set is barred
  */
  tdsl1_cell_bar_info_e_type l1_cell_bar_info;   

  /* Bar the frequency because of forbidden lai/non-eplmn*/
  boolean bar_for_resel_only;

  boolean non_eq_plmn_barred;

  /* GSM cell identity or FDD cell identity*/
  union
  {
    tdsl1_gsm_cell_identity_struct gsm;
  
    /* ZZTBD: tdsl1_fdd_cell_identity_struct not to be used for TDD */
    //tdsl1_fdd_cell_identity_struct fdd;
  
#ifdef FEATURE_TDSCDMA_TO_LTE
    tdsl1_eutra_cell_info_type eutra;
#endif

    tdsl1_tdd_cell_identity_struct tdd;
  }id;
}tdsl1_cell_bar_cmd_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_ACT_TIME_CANCEL_REQ
Activation time cancel primitive command
--------------------------------------------------------------------------*/
typedef struct 
{
  /* sequence number to cancel */
  uint8 seq_num;
} tdsl1_act_time_cancel_cmd_type;

#ifdef FEATURE_TDSCDMA_DATA_FOR_UI_BAR_DISPLAY
/*--------------------------------------------------------------------------
          L1_UI_DISPLAY_INFO
This structure will be used by L1 to fill RSSI, RSCP, Ec/Io and its validity
when BER timer is expied
--------------------------------------------------------------------------*/
typedef struct
{
   /* this flag will hold the value false when L1 is doing ACQ (or) Freq scan */
  boolean     is_data_valid;

  /* Rx AGC value */
  int16       rssi_value;

  /* Ec/Io value 
  ** In DCH HS call Ec/Io, RSCP is of the HS cell only 
  */
  int16       ecio_value; 

  /* RSCP value */
  int16       rscp_value;

  /* this flag will be set during a data call */
  boolean       use_ecio_for_display;
  
#ifdef FEATURE_UMTS_SIR_PATHLOSS
  /*Holds the Pathloss value. It is valid only for DCH state*/
  int16 pathloss; 

  /*Holds the Sir value. It is valid only for DCH state*/
  int16 sir;
#endif  

} tdsl1_ui_display_info_struct_type;

#endif
/*--------------------------------------------------------------------------
          L1_RSSI_INFO
This structure will be used by L1 to fill RSSI and its validity when BER timer is expied
--------------------------------------------------------------------------*/

typedef struct
{
 /* this flag will hold the value false when L1 is doing ACQ (or) Freq scan */
  boolean       is_valid_rssi;

  uint8           rssi_value;

#ifdef FEATURE_UMTS_SIR_PATHLOSS
  /*Holds the Pathloss value. It is valid only for DCH state*/
  int16 pathloss; 

  /*Holds the Sir value. It is valid only for DCH state*/
  int16 sir;
#endif

 } tdsl1_rssi_info_struct_type;

/*==========================================================================
**
**      DEFINITIONS OF ALL L1 INDICATIONS AND CONFIRMATIONS TO RRC    
**
**==========================================================================*/

/*--------------------------------------------------------------------------
          CPHY_MEASUREMENT_IND

Used by L1 to send RRC a measurement report message

All sections adhere to 03-2002 version of 25.331
--------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA_TO_LTE
/* event result structure */
typedef  struct
{
  tdsl1_inter_sys_event_enum_type  event_id;
  uint8 num_freq;
  struct
  {
    uint32 earfcn;  
    uint8 num_cell;
    uint16 phy_cell_id[TDSL1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY];
  }freqs[TDSL1_MAX_REP_EUTRA_FREQ];  
}tdsl1_eutra_event_results_struct_type;
#endif
 
/* Measurement result structure */
/* Either rsrp_incl or rsrq_incl should be set */
typedef struct
{
      uint16 phy_cell_id;
      boolean rsrp_incl;
      uint8 rsrp;
      boolean rsrq_incl;
      uint8 rsrq;
	  uint16 dummy;	/*make it word aligned to avoid crash in tdsmath_sort_exchange() func*/
} tdsl1_eutra_measured_result_cell_struct_type;
/* eutra measured cell structure */

#ifdef FEATURE_TDSCDMA_TO_LTE
typedef struct
{
  uint32 earfcn;  
  uint8 num_cell;
  uint8 dummy[3]; /*make it 4 bytes aligned to avoid crash in tdsmath_bbsort->tdsmath_sort_exchange, as CR606832*/
  tdsl1_eutra_measured_result_cell_struct_type cells[TDSL1_MAX_REP_EUTRA_CELLS_PER_FREQUENCY];
} tdsl1_eutra_measured_result_freq_struct_type;
/* eutra measured frequency structure */	

typedef struct
{
  uint8 measured_eutra_freqs;
  tdsl1_eutra_measured_result_freq_struct_type freqs[TDSL1_MAX_REP_EUTRA_FREQ];
}tdsl1_eutra_measured_results_struct_type;
/* eutra event results structure */

typedef struct
{
  /* event results*/
  tdsl1_eutra_event_results_struct_type  evt_res;
  /* measurement results*/
  tdsl1_eutra_measured_results_struct_type meas_res;
}tdsl1_inter_sys_eutra_rpt_struct_type;
#endif

/*
 * 10.2.19  - Measurement Report Message
 */
typedef struct
{
  /* Measurement id: described in 10.3.7.48 range 1...16 */
  uint16  meas_id;  
  
  /* DJM - this info should be maintained by RRC, not L1??? */
  /* Describes report transfer criteria to UTRAN */
  tdsl1_meas_report_trans_mode_enum_type meas_transfer_mode;

  /*  Describes the criterion of the  report Periodic/event triggered */  
  tdsl1_meas_report_mode_enum_type rpt_crit;
  
  /* valid if rpt_crit is Periodic, ignored otherwise             
  ** TRUE - Indicates to RRC that this is the last periodic report 
  **        for this type of measurement, RRC should delete the    
  **        corresponding measurement id                           
  */
  boolean last_periodic_report;
  
  /* Describes the type of measurement  */
  tdsl1_meas_type_enum_type  meas_type;
    
 /* Measured Results */
  union 
  {
    /* Intra-frequency measurement report info 10.3.7.35  */
    tdsl1_intra_freq_meas_rpt_struct_type intra_f_rpt ;

    /* Inter-frequency measurement report info 10.3.7.15  */   
    tdsl1_inter_freq_meas_rpt_struct_type inter_f_rpt ;

    /* Inter-RAT measurement report info       10.3.7.26 */   
    tdsl1_inter_sys_meas_rpt_struct_type inter_sys_rpt ;
#ifdef FEATURE_TDSCDMA_TO_LTE
    tdsl1_inter_sys_eutra_rpt_struct_type inter_sys_eutra_rpt ;
#endif

    /* Traffic volume measurement report info  10.3.7.67 */   
    tdsl1_traf_vol_meas_rpt_struct_type traf_vol_rpt ;

    /* Quality measurement report info         10.3.7.55 */
    tdsl1_qual_meas_rpt_struct_type qual_meas_rpt ;

    /* UE internal measurement report info     10.3.7.76 */
    tdsl1_ue_internal_meas_rpt_struct_type ue_internal_meas_rpt;

     /* UE positioning measured results list    10.3.7.99 */      
    tdsl1_ue_positioning_measured_results_type ue_pos_meas_results;      
  } u;

  /* flag indicating if additional measured results are appended, always false at present */
  boolean addl_meas_info_incl;
    
  /* number of additional measurements */
  uint16 num_addl_meas;
} tdsl1_meas_rpt_struct_type ;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_ERROR_IND

Used by L1 to report errors in configuration, processing or sequencing.
--------------------------------------------------------------------------*/
typedef struct
{
    
  /* Mask indication error type */  
  uint8              err_type;    
  /* Error Indication */
  tdsl1_error_enum_type err;  
} tdsl1_error_ind_type;

/* Return true if any of the STMR interrupts have failed status */
#define TDSL1_ERROR_IND_STMR_FAILED( err_type )   err_type

/* Macro to find out the error type in the Error Indication for a particular STMR interrupt. */
#define TDSL1_ERROR_IND_STMR_INT_FAILED( err_type, tdsintr ) \
  ( err_type & (1 << tdsintr) )


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_IDLE_CNF 
                         
Used to confirm L1 is in Idle state with no active channels in response to 
a TDSL1_CPHY_IDLE_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  boolean status;
} tdsl1_idle_cnf_type; 


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_DEACTIVATE_CNF
                         
Used to confirm that L1 has transitioned to the deactivated state in
response to the TDSL1_CPHY_DEACTIVATE_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_deactivate_cnf_type; 

/*--------------------------------------------------------------------------
                         tdsl1_stop_cnf_type 
                         
Used to confirm L1 is in STOPPED state in response to
TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16 dummy;
} tdsl1_stop_cnf_type;

/*--------------------------------------------------------------------------
                         tdsl1_start_cnf_type 
                         
Used to confirm L1 has started and is in IDLE state in response to
TDSL1_CPHY_START_TDSCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{

#ifdef FEATURE_TDSCDMA_DSDS
	  boolean			   status;
		/* Indicates if the acquisition succeeds. */
	
	  tdsl1_common_fail_enum_type fail_type;
		/* Indicates the type of acq failure. */
#endif /*FEATURE_TDSCDMA_DSDS*/

  uint16 dummy;
} tdsl1_start_cnf_type;

/*--------------------------------------------------------------------------
                         tdsl1_suspend_cnf_type 
                         
Used to confirm L1 is in SUSPENDED state in response to
TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{
  /* TRUE indicates suspend was successful. */
  boolean status;
} tdsl1_suspend_cnf_type;

/*--------------------------------------------------------------------------
                         tdsl1_resume_cnf_type 
                         
Used to confirm L1 is in RESUMED state in response to
TDSL1_CPHY_RESUME_TDSCDMA_MODE_REQ from RRC.
--------------------------------------------------------------------------*/
typedef struct
{
#ifdef FEATURE_TDSCDMA_DSDS
  /* Indicates the type of acq failure. */
  tdsl1_common_fail_enum_type fail_type;
#endif /*FEATURE_TDSCDMA_DSDS*/

  /* TRUE indicates resume was successful. */
  boolean status;
} tdsl1_resume_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_ACQ_CNF

Used to indicate acquisition results during initial cell search in response 
to a TDSL1_CPHY_ACQ_REQ.
---------------------------------------------------------------------------*/
 

/* Structure containing the scrambling code and pn position of
** the cell that was also detected on the same frequency as the
** cell that was currently acquired.
**
** Used only if acquisition mode type is TDSL1_WCDMA_ACQ_DET_LIST.
*/
/* This data structure is still used by TDD RRC */
typedef struct
{
  /* Scrambling code of detected cell */
  uint8                cpid;

  /* PN position of the cell in chipx1 units. */
  uint32                  pn_pos;
} tdsl1_acq_scr_code_det;

/* Number of CPIDs to be reported in the CPHY_ACQ_CNF to RRC */
#define TDSL1_NUM_CPID_DETECTED_MAX                           8

#ifdef FEATURE_TDSCDMA_DSDS
typedef struct
{

  /* Returns the RF channel on which acquisition is successful.
  ** In case of acquisition failure, returns the last RF channel 
  ** on which acquisition is attempted                       
  */
  uint16               freq;

  /* Indicates if the acquisition succeeds. */
  boolean              status;

  /* Indicates the cause if the acquisition fails. */
  tdsl1_acq_failure_cause_enum_type failure_cause;  

  tdsl1_common_fail_enum_type fail_type;/* Indicates the type of acq failure. */
  
  /* Indicates if it is a single cell acquisition. Matches
  ** the same field of CPHY_ACQ_REQ. If it is TRUE,  
  ** cpid[0] must be equal to the cpi field of CPHY_ACQ_REQ. 
  ** Multiple CPID is still allowed when it is TRUE.
  */
  boolean              single_cell;

  /* Number of CPIDs detected. */
  uint8                num_cpid_detected;
  
  /* Cells that are detected during acquisitions */
  uint8                cpid[TDSL1_NUM_CPID_DETECTED_MAX];
  
  uint8                num_tried_freq;
  
  uint16               tried_freq_list[TDSL1_MAX_ACQ_CHANNEL_NUM];
} tdsl1_acq_cnf_type;
#else
typedef struct
{

  /* Returns the RF channel on which acquisition is successful.
  ** In case of acquisition failure, returns the last RF channel 
  ** on which acquisition is attempted                       
  */
  uint16               freq;

  /* Indicates if the acquisition succeeds. */
  boolean              status;
  
  /* Indicates the cause if the acquisition fails. */
  tdsl1_acq_failure_cause_enum_type failure_cause;  


  /* Indicates if it is a single cell acquisition. Matches
  ** the same field of CPHY_ACQ_REQ. If it is TRUE,  
  ** cpid[0] must be equal to the cpi field of CPHY_ACQ_REQ. 
  ** Multiple CPID is still allowed when it is TRUE.
  */
  boolean              single_cell;

  /* Number of CPIDs detected. */
  uint8                num_cpid_detected;
  
  /* Cells that are detected during acquisitions */
  uint8                cpid[TDSL1_NUM_CPID_DETECTED_MAX];
  
  uint8                num_tried_freq;
  
  uint16               tried_freq_list[TDSL1_MAX_ACQ_CHANNEL_NUM];
} tdsl1_acq_cnf_type;
#endif

/*--------------------------------------------------------------------------
          TDSRRC_CPHY_DRX_CNF

Used to indicate results from drx setup in response to a TDSL1_CPHY_DRX_REQ
---------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates if the drx parameters were valid */
  boolean              status;
} tdsl1_drx_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_SETUP_CNF

Used to confirm that L1 has completed all of the requested physical channel 
setup in the last TDSL1_CPHY_SETUP_REQ.
--------------------------------------------------------------------------*/
/*
 * TDSRRC_CPHY_SETUP_CNF setup status identifier
 */
typedef enum
{
  TDSL1_CPHY_SETUP_CNF_STAT_TYPE_DL_TRCH,
  TDSL1_CPHY_SETUP_CNF_STAT_TYPE_DL_PHYCHAN,
  TDSL1_CPHY_SETUP_CNF_STAT_TYPE_UL_TRCH,
  TDSL1_CPHY_SETUP_CNF_STAT_TYPE_UL_PHYCHAN,
  TDSL1_CPHY_SETUP_CNF_NUM_STAT_TYPE
} tdsl1_cphy_setup_cnf_stat_type_enum_type;


#define TDSL1_CPHY_SETUP_CNF_OPS_MASK 0x00000003
#define TDSL1_CPHY_SETUP_CNF_OPS_STAT_MASK 0x00000004

#define TDSL1_CPHY_SETUP_CNF_DL_PHYCHAN_OFFSET(phch) \
  tdsl1_cphy_setup_cnf_dl_phychan_offset[phch]
  
#define TDSL1_CPHY_SETUP_CNF_DL_TRCH_OFFSET(trch) \
  tdsl1_cphy_setup_cnf_dl_trch_offset[trch]
  
#define TDSL1_CPHY_SETUP_CNF_UL_PHYCHAN_OFFSET(phch) \
  tdsl1_cphy_setup_cnf_ul_phychan_offset[phch]
  
#define TDSL1_CPHY_SETUP_CNF_UL_TRCH_OFFSET(trch) \
  tdsl1_cphy_setup_cnf_ul_trch_offset[trch]
  
#define TDSL1_CPHY_SETUP_CNF_SET_OP(type, op, offset) \
  (type = (type & ~(TDSL1_CPHY_SETUP_CNF_OPS_MASK << offset)) | \
  (((uint32) (op & TDSL1_CPHY_SETUP_CNF_OPS_MASK)) << offset))
  
#define TDSL1_CPHY_SETUP_CNF_SET_OP_STAT(type, offset, stat) \
  (type = (type & ~(TDSL1_CPHY_SETUP_CNF_OPS_STAT_MASK << offset)

typedef enum
{
  TDSL1_SETUP_STATUS_VALIDATION_FAIL,
  TDSL1_SETUP_STATUS_SETUP_FAIL,
  #ifdef FEATURE_TDSCDMA_DSDS
  TDSL1_SETUP_STATUS_TRM_LOCK_FAIL,
  #endif
  TDSL1_SETUP_STATUS_SUCCESS,
  TDSL1_SETUP_STATUS_DROP_ALL,
  TDSL1_SETUP_STATUS_SETUP_FAIL_INTRAF_HHO
} tdsl1_setup_cnf_status_enum_type;


typedef struct
{
  /* status of the DL phychan ops and channels */
  uint32 dl_phychan_status;
    
  /* status of the DL TrCH ops and channels */
  uint32 dl_trch_status;
    
  /* status of the UL phychan ops and channels */
  uint32 ul_phychan_status;
    
  /* status of the UL TrCH ops and channels */
  uint32 ul_trch_status;

  boolean hs_chan_status;

#ifdef FEATURE_TDSCDMA_HSUPA
  /* Combined status of the E_DL and E_UL operations;
  ** Takes a value of FALSE if E_DL or E_UL operation fails; Else, TRUE
  */
  boolean eul_chan_status;
#endif


  /* did the command pass or fail overall */
  boolean status;
    
  /* default value FALSE. 
  ** While CPHY_SETUP is not yet completed, if PHY_EST_IND need to be sent, this
  ** flag will be set to TRUE. This will help in sending the PHY_EST_IND after
  ** CPHY_SETUP_CNF to RRC 
  */
  boolean send_phy_est_ind;

  /* valid only if send_phy_est_ind is TRUE
  ** type of result to be sent as part of PHY_EST_IND
  ** TRUE for success and FALSE for unsuccessful establishment 
  */
  boolean phy_est_ind_result;

  /* Indicate the status type validation failures or 
  ** setup failures or setup success 
  */
  tdsl1_setup_cnf_status_enum_type status_type;


} tdsl1_setup_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_CELL_SELECTION_CNF

Used to indicate S values in response to a TDSL1_CPHY_CELL_SELECTION_REQ.
--------------------------------------------------------------------------*/

typedef struct
{
  /* Frequency specification */
  uint16     freq;
    

  
  uint16 cpi;

  /*zy--it will be delted later*/
  /* Cell Selection quality Value */
  int16    s_qual;
    
  /* Cell Selection RX Level Value */
  int16    s_rxlev;
    
  /* Quality measured during Cell Selection Evaluation.
  ** A bigger number means a higher energy meaurement.  
  */
  uint16   q_map;
/*zy--it will be delted later*/
  /* Primary Scrambling Code Index */
  uint8   pri_cpid;
  /* Flag indicating if the cell is searched or not.    
  ** Currently, if the cell timing is unknown, the cell 
  ** will not be searched for cell selection evaluation 
  */
  boolean  evaluated;
} tdsl1_cell_select_cnf_type;



/*--------------------------------------------------------------------------
          CPHY_ACT_TIME_CANCEL_CNF

Activation time cancel confirm primitive
--------------------------------------------------------------------------*/
typedef struct 
{
  /* sequence number of the activation time command */
  uint8 seq_num;

  /* whether cancellation was successful */ 
  boolean status;    
} tdsl1_act_time_cancel_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_CELL_RESELECTION_IND

Used to indicate cell reselection results.
--------------------------------------------------------------------------*/
/*
 * Cell Reselection Indication Information
 */
typedef struct
{
  tdsl1_cell_resel_status_enum_type     status;  
  
  tdsl1_cell_resel_cell_enum_type       cell;  
  
  /* Cell selection values evaluated by SRCH and to be reported to RRC */
  int16    s_rxlev;  

  union
  {
    /* WCDMA Cell Reselection Cell Information */
    tdsl1_cell_resel_wcdma_info_type   wcdma_info;  

    /* GSM Cell Reselection Cell Information */   
    tdsl1_gsm_cell_resel_ind_info_type  gsm_info;    

    /* IS2000 Cell Reselection Cell Information */      
    tdsl1_cell_resel_wcdma_info_type   is2000_info;
      
#ifdef FEATURE_TDSCDMA_TO_LTE
    /*E-UTRA cell reselection information*/
    tdsl1_eutra_cell_resel_ind_info_type eutra_info;      
#endif

    /* TD-SCDMA Cell Reselection Cell Information */
    tdsl1_cell_resel_tds_info_type   tds_info;  
  } u;
} tdsl1_cell_reselec_ind_type;


/*--------------------------------------------------------------------------
          TDSL1_CPHY_CELL_TRANSITION_REQ

Used by RRC to send a cell transition command
--------------------------------------------------------------------------*/
typedef struct
{
  /* cell type */
  tdsl1_cell_resel_cell_enum_type   cell_type;

  /* GSM cell info -- for cell transition failure case */
  tdsl1_gsm_cell_identity_struct    gsm_info;

  /* type of ASET update */
  tdsl1_aset_update_enum_type       select_type;
    
  /* a bit mask that indicates which of the sub-primitives 
  ** are included in the TDSL1_CPHY_SETUP_REQ.  See the comments 
  ** for the mask bit defines for more details. 
  */
  uint16 req_mask;
    
  /*it will be deleted later*/
  /* scrambling code */
  uint16 scr_code;

  /*+ huichunc add +*/
  /* cell parameter id */
  uint8 cpi;
  /*- huichunc  add -*/

  int8 max_tx_pwr;

  /* whether rf frequency information is included or not */
  boolean freq_incl;

  /* the rf channel if included. */
  uint16 rf_freq;
  
  /* data for TDSL1_CPHY_DL_CCTRCH_REQ */
  tdsl1_dl_cctrchcfg_cctrch_info_struct_type   *dl_cctrch_info;

  tdsl1_dl_cctrchcfg_ctfc_info_struct_type     *dl_ctfc_info;

  /* data for TDSL1_CPHY_DL_PHYCHAN_ADD_REQ */
  tdsl1_dl_phychan_enum_type add_dl_phychan;
  
  tdsl1_dl_phycfg_phychan_db_struct_type *add_dl_phychan_db;
    
  /* data for TDSL1_CPHY_UL_CCTRCH_REQ */
  tdsl1_ul_cctrch_cfg_struct_type *ul_cctrch_info;

  /* data for TDSL1_CPHY_UL_PHYCHAN_ADD_REQ */
  tdsl1_ul_phychan_enum_type add_ul_phychan;
  
  tdsl1_ul_phych_cfg_struct_type  *add_ul_phychan_db;

  tdsmac_ul_tfcs_type  *tfcs_ptr;

  /* Only applicable during reselection failure 
  ** Indicates if the target frequency is to be 
  ** excluded from future cell reselection evaluation  
  */
  boolean tg_freq_excl;

  /* Indicates if the target cell is to be excluded 
  ** from future cell reselection evaluation        
  */
  boolean tg_cell_excl;
} tdsl1_cell_transition_cmd_type;

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*--------------------------------------------------------------------------
           CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP
 
 Used by RRC to send a CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP
--------------------------------------------------------------------------*/
typedef struct
{
  /*TRUE:  RRC accepted RRC_NUDGE_FOR_SFN_MISS_MATCH_REQ from L1
   *FALSE: RRC is not in position to accept RRC_NUDGE_FOR_SFN_MISS_MATCH_REQ from L1
   */
  boolean status;
} tdsl1_rrc_nudge_update_cmd_type;

/*--------------------------------------------------------------------------
           TDSCPHY_GET_MODEM_STATISTICS_REQ
 
 Used by RRC to send a TDSCPHY_GET_MODEM_STATISTICS_REQ
--------------------------------------------------------------------------*/
#define TDSCDMA_GET_SERVING_CELL_POWER_INFO 0x01
#define TDSCDMA_GET_ACTIVE_SET_INFO 0x02
#define TDSCDMA_GET_MONITORED_SET_INFO 0x04


typedef struct
{
  /*GET_SERVING_CELL_POWER_INFO:  RRC request SERVING_CELL_POWER_INFO
   *GET_ACTIVE_SET_INFO: RRC request ACTIVE_SET_INFO
   *GET_MONITORED_SET_INFO: RRC request MONITORED_SET_INFO
   */
  uint8 modem_stat_req_bmsk;
} tdsl1_get_modem_stat_req_cmd_type;
#endif

#ifdef FEATURE_TDSCDMA_DSDS
typedef struct
{
  tdsl1_common_fail_enum_type oos_cause;
} tdsl1_out_of_service_type;
#endif

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
typedef struct
{
  boolean is_g2t_ta_mode; //TRUE: is in g2t ta mode; FALSE: isn't in g2t ta mode
} tdsl1_g2t_ta_status_type;
#endif

/*--------------------------------------------------------------------------
           TDSRRC_CPHY_OPERATIONAL_ERROR_IND
Used to indicate an error condition within Layer 1 to upper layers

--------------------------------------------------------------------------*/
typedef enum 
{
  TDSL1_START_AGC_MDSP_BAD_STATUS = 0,
  TDSL1_START_AGC_MDSP_TIMEOUT,
  TDSL1_GOTO_SLEEP_MDSP_BAD_STATUS,
  TDSL1_GOTO_SLEEP_MDSP_TIMEOUT,
  TDSL1_MDSP_SYNC_SEND_BUF_OVERFLOW,
  TDSL1_SRCH_CMD_TIMEOUT, 
  TDSL1_STUCK_IN_L1M_IDLE,
  TDSL1_NUM_OP_ERROR_SOURCES
} tdsl1_op_err_source_enum_type;

typedef struct 
{
  tdsl1_op_err_source_enum_type src;
} tdsl1_op_err_ind_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_IMMEDIATE_CELL_EVAL_CNF
Used to confirm results from immediate cell evaluation in response to a 
CPHY_IMMEDIATE_CELL_EVAL_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_immediate_cell_eval_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_SET_SFN_CNF

This confirms that SFN synchronization has been performed by L1 and 
indicates the SFN that frames should show synchronization by. Sent in 
response to a CPHY_SET_SFN_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  uint32  sfn;
} tdsl1_set_sfn_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_SET_CFN_CNF

This confirms that CFN synchronization has been performed by L1 and indicates 
the CFN that frames should show synchronization by. Sent in response to a 
CPHY_SET_CFN_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_set_cfn_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_MEASUREMENT_IND

Used to report Measurement results.
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_meas_rpt_struct_type *meas_rpt;
} tdsl1_measurement_ind_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_CAPABILITY_CNF

Used by the Physical Layer to report L1 capability.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_capability_cnf_type;


/*--------------------------------------------------------------------------
          RRC_CPHY_INTG_KEY_MAUC_CNF

Used by the Physical Layer to report the Message Authentication Code 
(MAC/XMAC for Uplink/Downlink) of the signaling message for which an 
integrity check was performed upon request from RRC.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_intg_key_mauc_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_OUT_OF_SYNC_IND

Used by L1 to report that the radio link has gone out of sync according to 
the criteria dictated by 25.214 section 4.3.1.2.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_out_sync_ind_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_IN_SYNC_IND

Used by L1 to report that the radio link has gone in sync according to the 
criteria dictated by 25.214 section 4.3.1.2.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_in_sync_ind_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND

Used by L1 to fulfil criteria of N312 and section 8.5.4 of RRC spec. Should 
only be expected when the DPCCH is first setup.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
    /* physical channel establish indication */
} tdsl1_phychan_est_ind_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_IMMEDIATE_RESELECTION_CNF

Used by L1 to return cell reselection results in response to a 
TDSL1_CPHY_IMMEDIATE_RESELECTION_REQ.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_immediate_reselec_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_IMMEDIATE_MEAS_CNF

Used by L1 to respond to RRCs TDSL1_CPHY_IMMEDIATE_MEAS_REQ primitive.
--------------------------------------------------------------------------*/
typedef struct
{
  /***** to be completed *****/
  uint16 dummy;
} tdsl1_immediate_meas_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_CELL_TRANSITION_CNF

 Used by L1 to respond to RRCs TDSL1_CPHY_CELL_TRANSITION_REQ primitive
--------------------------------------------------------------------------*/
typedef struct
{
  /* Status indicating whether the cell transition was successful or not  */
  boolean status;
    
  /* Reselection Indication Piggyback */
  tdsl1_cell_reselec_ind_type        cell_rsel_ind;
} tdsl1_cell_transition_cnf_type;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_COMPRESSED_MODE_IND
Used to indicate if meas ctrl message command from RRC is valid in response 
to a TDSL1_CPHY_MEASUREMENT_REQ. Status is true or false
---------------------------------------------------------------------------*/
typedef struct
{
  /* Measurement Identity Number */
  uint16 meas_id;
  

  /* Transmission Gap Pattern Identifier */ 
  uint16 tgpsi;
}  tdsl1_compressed_mode_ind_type;

/*--------------------------------------------------------------------------
          TDSRRC_CPHY_DL_WEAK_IND

Used to indicate a weak DL (for serving or neighbor PCCPCH)
--------------------------------------------------------------------------*/
typedef struct
{
  /* Indicates whether the indication is for serving or neighbor cell */
  boolean     serving_cell;  
  /* the cell parameter id in which DL weak happens */
  uint8 cell_parm_id;
  
  /* the primary frequency in which DL weak happens */
  uint16 primary_freq;

#ifdef FEATURE_TDSCDMA_DSDS
  /* no lock indicator */
  boolean no_resource;
#endif

} tdsl1_dl_weak_ind_type;

/*--------------------------------------------------------------------------
          TDSRRC_RL_FAILURE_IND

Used to indicate a RL FAILURE IN DCH
--------------------------------------------------------------------------*/
#ifdef FEATURE_TDSCDMA_L1M_SELF_RESET_FW
typedef struct
{
  /* Indicates whether the indication is triggered by FW */
  boolean     trigger_by_fw;  
}tdsl1_rl_failure_recover_fw_type;
#endif

#define TDSL1_CPHY_HIGH_MOBILITY_INCL 0x01
typedef struct
{
  uint8 bit_mask;
  
  boolean high_mobility_ind;
} tdsl1_upd_ota_parms_ind_cmd_type;

/*--------------------------------------------------------------------------
          RRC_CPHY_UPD_DL_SYNC_PARMS_CMD
Used to send the latest DL sync parameters received in a UTRAN Mobility 
Information message.
--------------------------------------------------------------------------*/
typedef struct {
  uint16 N315; 
  
  uint16 N312; 
  
  uint16 N313;
  
  uint8   T312; 
  
  uint8   T313; 
} tdsl1_upd_dl_sync_parms_cmd_type;

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
#ifdef FEATURE_TRITON_MODEM 
#else
#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
/*--------------------------------------------------------------------------
          RRC_CPHY_MODEM_STATISTICS_RSP

  Confirmation of MODEM_STATISTICS_RSP from L1
--------------------------------------------------------------------------*/

typedef struct {
  tdsl1_serving_cell_power_info_type *l1_serving_cell_power_info;
  tdsl1_active_set_info_type *l1_active_set_info;
  tdsl1_monitored_set_info_type *l1_monitored_set_info;
} tdsl1_modem_stat_rsp_cmd_type;
#endif
#endif/*FEATURE_TRITON_MODEM */

/*--------------------------------------------------------------------------
          CPHY_CHECK_S_CRIT_REQ

Used to check if S criteria is satisfied
--------------------------------------------------------------------------*/
typedef struct
{
  int16 q_qualmin;
  int16 q_rxlevmin;
  int16 ue_tx_max_pwr;
} tdsl1_s_criteria_req_type;
/*--------------------------------------------------------------------------
          RRC_CPHY_CHECK_S_CRIT_CNF

Used to indicate status if S Criteria is valid in response to CPHY_CHECK_S_CRIT_REQ
Status is TRUE or FALSE.
--------------------------------------------------------------------------*/
typedef struct
{
  boolean status;
} tdsl1_s_criteria_cnf_type;

/*--------------------------------------------------------------------------
          TDSCPHY_CHECK_SLEEP_RSP

Used to indicate if L1 can go to sleep in response to TDSRRC_CPHY_CHECK_SLEEP_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  boolean go_to_sleep; /* TRUE indicates that L1 can go to sleep */
  boolean wtog_bplmn_in_progress;
  boolean wtow_bplmn_in_progress;
} tdsl1_sleep_rsp_type;

/*--------------------------------------------------------------------------
          TDSRRC_CPHY_CHECK_SLEEP_REQ

Used to check if L1 can go to sleep
--------------------------------------------------------------------------*/
typedef struct
{
  uint32 timer_val;
}tdsl1_sleep_req_type;

/*--------------------------------------------------------------------------
           CPHY_ADDITIONAL_MEAS_DATA_REQ
 
 Used by RRC to send a CPHY_ADDITIONAL_MEAS_DATA_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  uint32                  meas_id;
  tdsl1_meas_type_enum_type  meas_type;
} tdsl1_additional_meas_info_req_type;

/*--------------------------------------------------------------------------
          CPHY_IMMEDIATE_MEAS_REQ

Used by RRC to request an immediate measurement report at FACH state. 
This is implemented as a function.
--------------------------------------------------------------------------*/
typedef struct
{
  uint16  aset_scr_code;
    /* Scrambling code of the serving cell */
  tdsl1_meas_quan_enum_type meas_quan;
    /* measurement quantity type */
  uint16 aset_meas_val;
    /* measurement value for the serving cell */

  uint16 num_ncells;
    /* Number of neighbors being reported  Upto 6 */
  struct
  {
      uint16 scr_code ;
        /* scrambling code for the cell  */
      tdsl1_meas_quan_enum_type meas_quan;
        /* measurement quantity type */
      uint16 meas_val;
        /* measured value for the cell  */
      tdsl1_sfn_report_enum_type sfn_rpt;
        /* SFN-SFN observed time diff reporting indicator */
      uint32 sfn_sfn_diff;
        /*  SFN-SFN difference  */
  } n_cells[ TDSL1_ASET_MAX ];
} tdsl1_immediate_meas_type;

/*--------------------------------------------------------------------------
           CPHY_GET_IMM_MEAS_REQ
 
 Used by RRC to send a CPHY_GET_IMM_MEAS_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_intra_freq_rach_rpt_quan_struct_type   meas_quan;
  tdsl1_meas_type_enum_type   meas_type;
} tdsl1_process_imm_meas_req_type;

/*--------------------------------------------------------------------------
           RRC_CPHY_GET_IMM_MEAS_RSP
 
 Used by L1 to send a RRC_CPHY_GET_IMM_MEAS_RSP
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_immediate_meas_type_tmp  *immed_meas;
} tdsl1_process_imm_meas_cnf_type;

#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
/*--------------------------------------------------------------------------
           CPHY_NMR_INFO_REQ
 
 Used by RRC to send a CPHY_NMR_INFO_REQ
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_rrc_utran_nmr_report_type report_type;
} tdsl1_utran_nmr_info_req_type;

/*--------------------------------------------------------------------------
           RRC_CPHY_UTRAN_NMR_INFO_CNF
 
 Used by L1 to send a RRC_CPHY_UTRAN_NMR_INFO_CNF
--------------------------------------------------------------------------*/
typedef struct
{
  tdsl1_meas_utran_nmr_info_union_type *l1_meas_utran_nmr_info;
  boolean                            status;
} tdsl1_utran_nmr_info_cnf_type;
#endif

#endif /*EATURE_TDSCDMA_FREE_FLOATING_TASK*/

/*--------------------------------------------------------------------------
          TDSL1_CPHY_UL_PHY_CHANNEL_CTRL_REQ

Used by RRC to send a Uplink Physical Channel Control command
--------------------------------------------------------------------------*/
typedef struct {

  /* CCTrCH power control info included. If TRUE, the following 3 fields are valid; otherwise ignore them. */
  boolean  cctrch_pwr_ctrl_info_incl;
  
  /* INT8 (-120..-58). In dBm. PRXdpch_des. See 25.331:10.3.6.91 */
  int8     prx_dpch_des;
  
  /* TRUE the field existed and always TRUE; FALSE the field not present. See 25.331:10.3.6.91 */
  boolean  beacon_pl_est;
  
  
  
  /* (0, 6..43). In dBm. PCCPCH tx power. For Pathloss Calculation. See 25.331:10.3.6.91 */
  uint8    pccpch_tx_pwr;
  
  /* Special burst scheduling included. See 25.331:10.2.59. */
  boolean  sb_scheduling_incl;
  
  /* (0..7). Value represents number of radio frames */
  /* 0 = 2 frames, 1 = 4 frames,
  ** 2 = 8 frames, 3 = 16 frames,
  ** 4 = 32 frames, 5 = 64 frames,
  ** 6 = 128 frames, 7 =256 frames 
  */
  uint8    sb_gen_prd;
  
  /* (1..8). Uplink sync step size. See 25.331:10.2.59. Default value is 1. */
  uint8    ul_sync_step_size;
  
  /* (1..8). Uplink sync frequency. See 25.331:10.2.59. Default value is 1. */
  uint8    ul_sync_frequency;
  
  /* (-120..-58, 0). In dBm. Desired power level for HS-SICH. 0 no such parameter. See 25.331:10.2.59 */
  int8     prx_hssich;
  
  /* HS-SICH TPC step size present flag */
  boolean  hssich_tpc_step_size_incl;
  /* TPC step size in dB. See 25.331:10.3.6.91 */
  tdsl1_tpc_step_size_enum_type  tpc_step_size;
  /* (1,2,3). In dB. For HS-SICH. See 25.331:10.2.59. */
  tdsl1_tpc_step_size_enum_type  hssich_tpc_step_size;
} tdsl1_ul_phych_control_cmd_type;


typedef struct
{
  /* DRX Cycle Length in 10 ms radio-frame units */
  uint16 drx_cycle_len;

  /* log to base 2 of the drx_cycle_len (for logging) */
  uint8 log_drx_cycle_len;

  /* number of elapsed DRX Cycles */
  uint32 num_drx_cycles;

  /* Total number of DRX Cycles. This value is only reset by diag. */
  uint32 total_drx_cycles;

  /* the next paging occasion (sfn of frame that may contain a PI for this UE) */
  uint16 next_paging_occasion;

  /* SFN of PCCPCH frame that contains the (1st of the) PI bits */
  uint16 pich_sfn; 

  /* Paging Indicator */
  uint32 PI;

  /* 10.3.6.41 Number of PI per frame - obtained from RRC during SCCPCH setup; 
  */
  uint8 pi_per_frame;

  /* IMSI div K, as per 8.3 in 25.304 (version 2000-12), used in calculating paging
     occasion, where K is the number of SCCPCHs that carry a PCH.
     Obtained from RRC during SCCPCH setup*/
  uint64 imsi_div_pch_cnt;

  /* IMSI div 8192, as per 8.3 in 25.304 (version 2001-06), used in calculating PI.
     Obtained from RRC during SCCPCH setup*/
  uint64 imsi_div_8192;

 /*
  * "imsi_div_K modulo drx_cycle_length" which is the fixed term of the 
  * paging occasion; the other term used in calculating paging occasion
  * is a variable term. Paging occasions are (fixed term) + n * drx_cycle_len
  * for n = 0,1,2,...
  * this corresponds to the case of n=0.
  */
  uint16 min_paging_occasion;

  /* CCTrCh table index of the SCCPCH that carries the paging blocks */
  uint8 sccpch_cctrch_idx;

  /* Timing Offset of the SCCPCH that is associated with this PICH */
  uint16 sccpch_offset;

  /* the offset of the PICH relative to the PCCPCH (units of 256 chips)*/
  int16 pich_offset;

  /* Threshold for detecting PI bits; required by the mdsp to provide a Y/N decision whether
     the PI bits were +1 or -1 */
  int16 mdsp_pi_threshold;

  /* Number of bits allocated to a paging indicator = 288/Np, where
   * Np is the PI per frame. Range (2, 4, 8, or 16). This parameter
   * is programmed to the mDSP.
   */
  uint8 mdsp_bits_per_pi;

  /* the time difference (in units of 256 chips) from the start of the pich frame to
     when the page indicator bits start in the pich frame
   * Range (0...143).
   */
  uint8 pi_offset_from_pich;

  /* the time difference between the start of the PI bits to the start of the
    previous PCCPCH frame. In other words, 
    the sum of pi_offset_from_pich and pich_offset parameters described above, modulo 150.
    An mdsp parameter*/
  uint16 mdsp_pi_pn_count;

  /* the count, in 256 chip units, at which pich tear down should occur */
  uint16 mdsp_pich_tear_down_pi_pn_count;

  /* whether sccpch with pich was setup before drx i.e. whether to trust the values of
  bits_per_pi etc ..*/
  boolean sccpch_with_pich_setup;

  /* 25.331 10.3.6.50 : power transmitted on PICH minus power transmitted on CPICH
     in dB (range -10..5) */
  int8 pich_pwr_offset;

  uint8 mdsp_sccpch_setup_cfn;

  /* number of DRX commands received from RRC, and accepted by L1 */
  uint32 num_drx_cmds;

  /* number of pi sum zeros */
  uint32 num_zero_pisum;

  /* number of pi sum non zeros */
  uint32 num_non_zero_pisum;

  /* number of po overruns */
  uint32 num_po_overruns;
} tdsdrx_parms_struct_type;
/* Holds all the parameters necessary for calculating parameters
 * for PICH demodulation.
 */
extern tdsdrx_parms_struct_type tds_drx_parms;

typedef struct
{
  /* 0 to 16383 */
  uint16      uarfcn;

  /* 0 to 127 */
  uint8       cpid;

  /* 46 to 158 dB */
  uint8       pathloss;

  /* 0 to 8191 cx8, same value for all timeslot */
  uint16      timing_advance;

  /* (-25 to -128)*256 RSCP dBm in Q8 L3 filtered. */
  int32       rscp;
} tdsl1_cell_meas_result;


/*--------------------------------------------------------------------------
          TDSRRC_CPHY_ACTIVATION_TIME_IND 
Used to send the TDSRRC_CPHY_ACTIVATION_TIME_IND to RRC, at the 
CFN activation - 1. 
--------------------------------------------------------------------------*/



typedef struct
{
  /* Ordered Activation CFN -1 */
  uint8 cfn;

  /* ho_type
  ** 0x01 ................... BHO 
  ** 0x02 ................... HHO
  ** 0x80 ................... Failure Recovery
  */
  uint8 ho_type;


} tdsl1_activation_time_ind_type;

typedef enum {
  /* Tx total power < 0dBm */
  TDSL1_UL_TX_POWER_LOW,
  
  /* Tx total power within 0dBm - 10dBm (both included) */
  TDSL1_UL_TX_POWER_MEDIUM,
  
  /* Tx total power > 10dBm */
  TDSL1_UL_TX_POWER_HIGH,
  
  /* Initial/Invalid value */
  TDSL1_UL_TX_POWER_MAX
} tdsl1_ul_tx_power_range_e_type;

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_TDSCDMA_DATA_FOR_UI_BAR_DISPLAY
/*===========================================================================
FUNCTION        tdsl1_get_data_for_ui_display

FILE NAME       dldem.c

DESCRIPTION     This function returns the RSSI, RSCP, Ec/Io
                for use by the UI and Call Manager subsystems

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    Ec/Io, RSSI, RSCP  values (signed).  
               

SIDE EFFECTS    None
===========================================================================*/
void tdsl1_get_data_for_ui_display(tdsl1_ui_display_info_struct_type *l1_data_for_ui_display);
#else
/*===========================================================================
FUNCTION        tdsl1_get_rssi

FILE NAME       dldem.c

DESCRIPTION     This function returns the Received Signal Strength Indication
                for use by the UI and Call Manager subsystems

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    The measured signal level shall be sent with no sign

SIDE EFFECTS    None
===========================================================================*/
void tdsl1_get_rssi(tdsl1_rssi_info_struct_type *rssi);
#endif

/*===========================================================================

FUNCTION       tdsl1_get_mobility_info

DESCRIPTION
   This function returns the mobility state of UE to RRC.
   
DEPENDENCIES
  None.

RETURN VALUE   
  TRUE if UE is in High mobility state
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1_get_mobility_info(void);

#ifdef FEATURE_TDSCDMA_HSUPA

/*=========================================================================

FUNCTION     : tdsl1_rrc_get_hsupa_category

DESCRIPTION  : RRC returns the HSUPA Category to L1. RRC reads NV to determine 
               UE CAT (valid NV value, invalid NV value, NV not present, 
               NV reading failure)  

DEPENDENCIES : None

RETURN VALUE : HSUPA Category value 1..6

SIDE EFFECTS : None

=========================================================================*/

extern uint16 tdsrrc_get_hsupa_category(void);

/*===========================================================================
FUNCTION  tdsl1_max_allowed_upa_rate

DESCRIPTION This function calculate the Tput of last second

DEPENDENCIES called in RRC

RETURN VALUE Tput of last second, in unit of bps

SIDE EFFECTS None
===========================================================================*/

extern uint32 tdsl1_max_allowed_upa_rate(void);


/*===========================================================================
FUNCTION  tdsl1_get_ul_plus_flag

DESCRIPTION  Calculate the whether L1 could support being provide extra more data from higher layer

DEPENDENCIES called in RRC

RETURN VALUE  Return TRUE when L1 support transferring extra data from higher layer

SIDE EFFECTS None
===========================================================================*/

extern boolean tdsl1_get_ul_plus_flag(void);


#endif /*FEATURE_TDSCDMA_HSUPA*/

/*=========================================================================

FUNCTION     : tdsl1_get_interrat_measurement_data

DESCRIPTION  : This function will accept an inter-rat measurement data
               structure pointer.  It will fill in the structure with
               all relevant inter-rat cell measurement information.

DEPENDENCIES : None

RETURN VALUE : Contents of rrc_rr_interrat_measurement_data_type struct

SIDE EFFECTS : None

=========================================================================*/
void tdsl1_get_interrat_measurement_data( tdsrr_interrat_measurement_data_type * p_msmt_data);

/*===========================================================================
FUNCTION     tdsl1_get_periodic_inter_rat_msmt_results

DESCRIPTION
  This function helps to build a measurement report message when
  periodic reporting triggers that a report should be sent.  This function
  provides the Inter-RAT addl_measurement information.

DEPENDENCIES
  None

RETURN VALUE
  True if information is available, False otherwise

SIDE EFFECTS
  TDS_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation, remove int
  blocking when L1 performs the call, in future.
===========================================================================*/
boolean tdsl1_get_periodic_inter_rat_msmt_results
          (
            uint32  meas_id,
            tdsl1_inter_sys_meas_result_struct_type *p_msmtres
          );
/* jiangbing 3.4.10 delete begin */
/*====================================================================
FUNCTION: tdsrrc_ul_tx_pwr_ind()

DESCRIPTION:
  This function places the values of cpich tx power, ul interference,
  and a constant value defined in PRACH sys info, into the addresses
  passed into this function.

DEPENDENCIES:
  Implemented in RRC

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void tdsrrc_ul_tx_pwr_ind
(
  int8    *cpich_tx_pwr,
  int8    *ul_if,
  int8    *const_val
);
/* jiangbing 3.4.10 delete end */

/*====================================================================
FUNCTION: tdsl1_get_stored_intra_freq_meas()

DESCRIPTION:
  This function indicates l1 to save intra freq measurement criteria
  based on the value of measurement identity. 

DEPENDENCIES:
  -

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
extern void tdsl1_get_stored_intra_freq_meas
(
 uint16 meas_id
);

/*====================================================================
FUNCTION: tdsl1_srchmeas_set_inter_freq_measdb_status_during_state_transition()

DESCRIPTION:
  Through this function RRC indicates to Layer1 whether to save inter freq 
  measurement database during a state transition. 
  Measurements should only be saved during a (DCH -> FACH -> DCH) 
  transition.
  
DEPENDENCIES:
  -

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
extern void tdsl1_srchmeas_set_inter_freq_measdb_status_during_state_transition(boolean status);

/*===========================================================================
FUNCTION     tdsl1_get_additional_intra_freq_msmt_results

DESCRIPTION
  This function returns the intra freq measurement results as part
  of additional measurement info.
  
DEPENDENCIES
  None

RETURN VALUE
  False if meas id not present or meas_id is for non-intra freq
        or if the reporting crit is event triggered
  True otherwise.
  
SIDE EFFECTS
  TDS_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation.
===========================================================================*/
extern boolean tdsl1_get_additional_intra_freq_msmt_results
          (
            uint32  meas_id,
            tdsl1_intra_freq_meas_rpt_struct_type *p_msmtres
          );

/*===========================================================================
FUNCTION     tdsl1_get_additional_interf_msmt_results

DESCRIPTION
  This function returns the inter freq measurement results as part
  of additional measurement info.

DEPENDENCIES
  None

RETURN VALUE
  False if meas id not present or meas_id is for non-inter freq
        or if the reporting crit is event triggered
  True otherwise.

SIDE EFFECTS
  TDS_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation.
===========================================================================*/
extern boolean tdsl1_get_additional_interf_msmt_results(
            uint32  meas_id,
            tdsl1_inter_freq_meas_rpt_struct_type *p_msmtres );


/*===========================================================================
FUNCTION     tdsl1_get_additional_internal_msmt_results


DESCRIPTION
  This function returns the internal measurement results as part
  of additional measurement info.


DEPENDENCIES
  None


RETURN VALUE
  False if meas id not present or meas_id is for non-internal
        or if the reporting crit is event triggered
  True otherwise.


SIDE EFFECTS
  TDS_INTFREE()/WCDMA_INTLOCK() while RRC performs this operation.
===========================================================================*/
extern boolean tdsl1_get_additional_internal_msmt_results(
            uint32  meas_id,
            tdsl1_ue_internal_meas_rpt_struct_type *p_msmtres );

/*===========================================================================
FUNCTION     tdsl1_rrc_is_gsm_cell_present_in_sibs


DESCRIPTION
  This function returns TRUE if GSM cell are present in SIB11 else
  return FALSE. Called only by L1 in its context.


DEPENDENCIES
  None


RETURN VALUE
  True/False


SIDE EFFECTS
  TDS_INTFREE()/WCDMA_INTLOCK() while L1 performs this operation.
===========================================================================*/
extern boolean tdsl1_rrc_is_gsm_cell_present_in_sibs(void);

/*===========================================================================
FUNCTION        tdsl1m_get_trk_lo_adj_pdm

DESCRIPTION
  This function may be invoked by RRC to grab the mDSP's current VCTCXO
  adjustment PDM value.  It returns the value from the register
  tcxoAdjAccumMsw.  The tcxoAdjAccumLsw register is not read.

  It may not be able to read the tcxoAdjAccumMsw register because the WCDMA
  mDSP image is not active, or the mDSP clock is not enabled.  In this case
  it returns either a previously read value if there is one or a default
  value if there is not.  It also returns a previously read or default value
  if we do not have WCDMA service.

  This value may then get passed into another RAT, like GSM, so the VCTCXO
  trk_lo_adj PDM can start at this previously good, tracking PDM value.  If
  VCTCXO has never tracked, the return value is unlikely to be of much help.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  uint16 - Latest VCTCXO adjustment PDM value, read straight from mDSP's
           tcxoAdjAccumMsw register.  If it can.  Otherwise, it returns
           a default or previous PDM value.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint16 tdsl1m_get_trk_lo_adj_pdm(void);

/*===========================================================================

FUNCTION     tdsl1_get_reference_cell_info

DESCRIPTION
  This function gives subsystems outside L1 (or inside) access to
  information about the current reference cell in DCH or serving cell in
  other states.  It only provides the reference or serving cell's
  frequency and primary scrambling code if the mobile is in a state where
  it actually has a reference or serving cell.

DEPENDENCIES
  None.

PARAMETERS
  uint16 *freq - Reference or serving cell's frequency.
  uint16 *psc  - Reference or serving cell's primary scrambling code.

RETURN VALUE
  TRUE  - Mobile has a reference or serving cell.
          *freq and *psc are valid.
  FALSE - Mobile has no reference or serving cell.
          *freq and *psc are invalid.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1_get_reference_cell_info(uint16 *freq, uint8 *cpid);

/*====================================================================
FUNCTION: tdsl1_rrc_query_crnti

DESCRIPTION:
  This function queries RRC for C_RNTI existence and value. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  returns TRUE if C_RNTI is valid and the value is *c_rnti

SIDE EFFECTS:
  None.
====================================================================*/
boolean tdsrrc_query_crnti
(
  uint16 *c_rnti
);


/*====================================================================
FUNCTION: tdsl1_check_cell_barred

DESCRIPTION:
  This function is used to check if a WCDMA cell is barred or not

DEPENDENCIES:
  NONE

RETURN VALUE:
  WCDMA cell is not barred, or only the PSC is barred, or the frequency
  is barred

SIDE EFFECTS:
  NONE
====================================================================*/
tdsl1_cell_bar_info_e_type tdsl1_check_cell_barred
(
  uint16 freq, uint8 cpid
);

#ifdef FEATURE_TDSCDMA_PWR_TFC_ELMN

/*====================================================================
FUNCTION: tdsl1_disable_ul_tfc_pwr_restriction

DESCRIPTION:
  This function disables UL TFC power restriction dynamically when
  AMR is setup or added for DCH. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  L1 will not restrict any UL TFC for MAX TX power limitation in DCH.
====================================================================*/
void tdsl1_disable_ul_tfc_pwr_restriction(void);

/*====================================================================
FUNCTION: tdsl1_enable_ul_tfc_pwr_restriction

DESCRIPTION:
  This function allows UL TFC power restriction dynamically when
  CS/PS data call is setup or AMR is dropped from concurrent services
  for DCH. 

DEPENDENCIES:
  NONE

RETURN VALUE:
  NONE

SIDE EFFECTS:
  L1 will start to restrict UL TFC for MAX TX power limitation in DCH.
====================================================================*/
void tdsl1_enable_ul_tfc_pwr_restriction(void);

#endif /* FEATURE_TDSCDMA_PWR_TFC_ELMN */

//#ifdef FEATURE_WCDMA_PRACH_SELECTION
typedef enum
{
  TDSL1_PRACH_INVALID,       /* PRACH configuration is invalid */
  /* jiangbing 3.4.14 add begin */
  TDSL1_PRACH_5MS_ALLOWED,  /* allows 5ms RACH TTI */
  /* jiangbing 3.4.14 add end */
  TDSL1_PRACH_10MS_ALLOWED,  /* allows 10ms RACH TTI */
  TDSL1_PRACH_20MS_ALLOWED   /* allows 20ms RACH TTI */
} tdsl1_rach_tti_allowed_enum_type;

typedef struct
{
  /* Interested TFC */
  uint8   tfci;

  /* UE MAX TX power in dBm in SIB */
  int8    max_tx_pwr;    

  /* Primary CPICH TX power in dBm in SIB */
  int8    pr_cpich_tx_pwr; 

  /* PRACH constant value in dB in SIB */
  int8    const_val;      

  /* UL interference in dB in SIB 7 */
  int8    ul_if;           
  
  tdsl1_ul_cctrch_cfg_struct_type*  rach_cctrch_cfg_ptr;
  
  tdsmac_ul_tfcs_type*              rach_ul_tfcs_ptr;
} tdsl1_prach_selection_info_struct_type;
//#endif /* FEATURE_WCDMA_PRACH_SELECTION */

#if 0
#if defined( FEATURE_WCDMA_DYNAMIC_POWER_CLASS ) || defined( FEATURE_WCDMA_LOWER_POWER_FOR_AMR )
/*====================================================================
FUNCTION: tdsl1_lower_power_class_for_amr

DESCRIPTION:
  This function notifies L1 to lower TX power class for AMR at next
  phychan config or re-config. 

DEPENDENCIES:
  To be called by RRC when AMR is going to be added in DCH state
  before CPH_SETUP_REQ.

RETURN VALUE:
  NONE

SIDE EFFECTS:
  L1 will lower TX power class at next phychan config or re-config. 
====================================================================*/
void tdsl1_lower_power_class_for_amr(void);

/*====================================================================
FUNCTION: tdsl1_resume_normal_power_class

DESCRIPTION:
  This function notifies L1 to resume normal TX power class at next
  phychan config or re-config. 

DEPENDENCIES:
  To be called by RRC when AMR is going to be deleted in DCH state
  before CPH_SETUP_REQ.

RETURN VALUE:
  NONE

SIDE EFFECTS:
  L1 to resume normal power class at next phychan config or re-config.
====================================================================*/
void tdsl1_resume_normal_power_class(void);

#endif /* ( FEATURE_WCDMA_DYNAMIC_POWER_CLASS || FEATURE_WCDMA_LOWER_POWER_FOR_AMR ) */

/*===========================================================================

FUNCTION tdsl1_get_best_psc_in_aset

DESCRIPTION
  This function returns the best cell in the active set. This function is called
  by RRC on DCH to Idle transition so that acquisition is first attempted on the
  best cell in the active set before RRC tries acquisition on frequencies in the
  acq db.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: at least one cell was found in the active set (which should generally be the case)
  FALSE: no cell was found in the active set (failure case).

SIDE EFFECTS
  None.

===========================================================================*/
boolean tdsl1_get_best_psc_in_aset(uint16 *scr_code, uint32 *pn_pos);
#endif

/*===========================================================================
FUNCTION tdshscfg_get_cur_state

DESCRIPTION
  This function returns the current state of HSDPA channel in L1. 
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active.
  FALSE if HSDPA channel is inactive.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean tdsl1_get_hs_cur_state(void);


/*===========================================================================
FUNCTION        DL_PHYCHAN_CCTRCH_ID_IN_USE

FILE NAME       dl_dem.c

DESCRIPTION     This function retrieves the currently used DL CCTrCH id.

DEPENDENCIES    The physical channel database

RETURN VALUE    CCTrCH that DL is currently working on, if DL DPCH 
                channel is established
                0xFF if DL DPCH channel is inactive

SIDE EFFECTS    None
===========================================================================*/
extern uint8 tdsdlchmgr_dl_phychan_cctrch_id_in_use(void);

/*===========================================================================
FUNCTION        UL_PHYCHAN_IS_UL_ACTIVE

DESCRIPTION     This function returns the state of UL phy channel,
                whether it is active or not.

DEPENDENCIES    none

RETURN VALUE    TRUE if either PRACH or DPCH channels are active
                FALSE if neither PRACH or DPCH channels are active

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsulcmd_phychan_is_ul_active (void);

/*===========================================================================
FUNCTION        tdsl1_ul_phychan_is_ul_dpch_active

DESCRIPTION     This function returns the state of UL DPCH channel,
                whether it is active or not.

DEPENDENCIES    none

RETURN VALUE    TRUE if UL DPCH channel is active
                FALSE if UL DPCH channel is active

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsl1_ul_phychan_is_ul_dpch_active (void);

/*===========================================================================
FUNCTION        UL_PHYCHAN_IS_UL_PRACH_ACTIVE

DESCRIPTION     This function returns the state of UL PRACH channel,
                whether it is active or not.

DEPENDENCIES    none

RETURN VALUE    TRUE if UL PRACH channel is active
                FALSE if UL PRACH channel is inactive

SIDE EFFECTS    None
===========================================================================*/
extern boolean tdsulcmd_phychan_is_ul_prach_active (void);

#ifndef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION          tdsl1_rrc_can_l1_go_to_sleep

DESCRIPTION       This function is called by L1 in every DRX cycle to determine
                  if L1 should go to sleep or not. These function call was 
                  introduced due to WTOW BPLMN search. If search needs to be
                  done during the DRX sleep time, then L1 should NOT go to sleep.

DEPENDENCIES      NONE

RETURN VALUE      
                  TRUE:  L1 can go to sleep.
                  FALSE: Otherwise
                                  
SIDE EFFECTS

===========================================================================*/
extern boolean tdsrrc_can_l1_go_to_sleep(uint32 timer_val, boolean *wtog_bplmn_in_progress);

/*===========================================================================

FUNCTION tdsrrcpg1_modify_sibs_for_sfn_update

DESCRIPTION
   Call back function for L1 to induce S-PCCPCH setup


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
#endif

/*===========================================================================

FUNCTION     tdsl1m_state_is_fach

DESCRIPTION
  This function is called to check whether the UE is in TDSL1M_FACH state

DEPENDENCIES
  None

RETURN VALUE
  TRUE if tdsl1m_global.l1_state is TDSL1M_FACH and FALSE otherwise

SIDE EFFECTS

===========================================================================*/
extern boolean tdsl1m_state_is_fach(void);

/*===========================================================================
FUNCTION tdshscfg_tdshscfg_is_supported

DESCRIPTION
  This function returns whether HSDPA is supported on this chip.

DEPENDENCIES
  HS cfg init must have been called earlier to set its value.

RETURN VALUE
  TRUE if HSDPA is supported.
  FALSE if HSDPA is not supported.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdshscfg_tdshscfg_is_supported(void);


/*===========================================================================
FUNCTION        L1_GET_RSSI_DURING_WTOW_BPLMN

FILE NAME       srchbplmn.c

 DESCRIPTION     This function is used by RRC to get the signal strength of the PLMNs during
                        a BPLMN search.The measured signal level shall be sent with no sign 

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    

SIDE EFFECTS    None
===========================================================================*/
void tdssrchbplmn_l1_get_rssi_during_wtow_bplmn(tdsl1_rssi_info_struct_type *rssi);


#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
/*===========================================================================
FUNCTION tdsl1_get_utran_nmr_info

DESCRIPTION
  This function sends upto 6 best intra/inter-freq/inter-rat cells
  in response to a NMR request.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1_get_utran_nmr_info
(
  tdsl1_meas_utran_nmr_info_union_type *l1_meas_utran_nmr_info, 
  tdsl1_rrc_utran_nmr_report_type report_type
);
#endif /* FEATURE_TDSCDMA_PLI_NMR_UTRAN */

#if 0
#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
/*===========================================================================
FUNCTION L1_GET_SERVING_CELL_INFO

DESCRIPTION
  This function populates the serving cell power info in the arguments
  This API should be used  in non DCH state.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
void tds_l1_get_serving_cell_power(tdsl1_serving_cell_power_info_type* l1_serving_cell_info);

/*===========================================================================
FUNCTION tds_l1_get_active_set_info

DESCRIPTION
  This function populates the active set info in the arguments provided
  This function should be called only in the DCH state.
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
void tds_l1_get_active_set_info(tdsl1_active_set_info_type* l1_active_set_info);

/*===========================================================================
FUNCTION  tds_l1_get_monitored_set_info

DESCRIPTION
  This function populates the monitores set info in the structure provided

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/

void tds_l1_get_monitored_set_info(tdsl1_monitored_set_info_type* l1_monitored_set_info);

#endif /*FEATURE_TDSCDMA_MODEM_STATISTICS*/
#endif
/*===========================================================================

FUNCTION tdsseq_get_cfn

DESCRIPTION
  This function returns the value of the current CFN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 tdsseq_get_cfn( void );


/*============================================================================
FUNCTION: tdsl1_clear_barred_cells()

DESCRIPTION:
  This function clears all barred cell structures and timers. This function will 
  be called by RRC when the UE is soft-reset, or enters LPM.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void tdsl1_clear_barred_cells(void);

/*===========================================================================
FUNCTION      tdsl1_check_cell_barred_and_get_bar_time

DESCRIPTION
  This function is used to check if a TDS cell is barred or not. 
  If its barred, get the remaining barred time, and return the bar_type. 
DEPENDENCIES
  None.

RETURN VALUE
TDSL1_TDS_NOT_BARRED: cell/freq not barred
TDSL1_TDS_CPID_BARRED:  only cell is barred
TDSL1_TDS_FREQ_BARRED: the frequency is also barred. 


SIDE EFFECTS
  None.
===========================================================================*/
tdsl1_cell_bar_info_e_type tdsl1_check_cell_barred_and_get_bar_time
(
  uint16 freq, uint8 cpid, uint16 *rem_bar_time_in_secs, boolean *is_bar_resel
);



/*===========================================================================

FUNCTION     DRX_BCH_SETUP

DESCRIPTION
  This function is called when a BCH is setup. It sets a flag in the drx module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_bch_setup(void);



/* ------------------------------------------------------------------- */
/* These are the channel numbers used by each band class. Note that    */
/* they do match those specified in 3GPP 25.102 v5.2.                */
/* ------------------------------------------------------------------- */

/* --------------------------------- */
/*  TDSCDMA BAND A                   */
/* --------------------------------- */

/* Downlink channel boundary for BAND A */
#define TDSL1_BAND_A_CHAN_MIN              10054
#define TDSL1_BAND_A_CHAN_MAX              10121

/* --------------------------------- */
/*  TDSCDMA BAND E                   */                  
/* --------------------------------- */ 

/* Downlink channel boundary for BAND E */
#define TDSL1_BAND_E_CHAN_MIN              11504
#define TDSL1_BAND_E_CHAN_MAX              11996

/* --------------------------------- */
/*  TDSCDMA BAND F                   */                  
/* --------------------------------- */ 

/* Downlink channel boundary for BAND F */
#define TDSL1_BAND_F_CHAN_MIN              9404
#define TDSL1_BAND_F_CHAN_FS_LOWER_MAX        9500
#define TDSL1_BAND_F_CHAN_FS_UPPER_MIN        9501
#define TDSL1_BAND_F_CHAN_MAX              9596
/*--------------------------------------------------------*/
/*                 RRC COMMAND IDs                        */
/*                                                        */
/*  Command Ids for commands sent from various tasks to   */
/*  RRC are defined here. All command ids, regardless of  */
/*  which command queue is used to queue them are defined */
/*  in a single enum type given below.                    */
/*                                                        */
/*  NOTE! When you add a new command ID please make sure  */
/*  the corrosponding command structure is added to the   */
/*  structure tdsrrc_cmd_type that contains all the commands.*/
/*                                                        */
/*--------------------------------------------------------*/

/*
                      
typedef enum{         
                      
}tdsrrc_l1_cmd_e_type;
*/

/* RRC starts QTA around 38ms 
** to allow for TDSCDMA to prepare QTA */
#define TDSRRC_UNLOCK_QTA_OVERHEAD_TIMER_IN_MS 38
#define TDSRRC_UNLOCK_QTA_MAX_OVERHEAD_TIMER_IN_MS 70 /* Let L1 uplink has enough time to predict 40ms TTI transmission*/
#define TDSRRC_UNLOCK_QTA_IMMEDIATE_TIMER_IN_MS 0 /*When QTA time is less than 70ms, RRC will send Start_QTA_REQ to L1 immediately */
//#define TDSRRC_MIN_GAP_ACROSS_TUNEAWAYS_IN_MS 4
#define TDSRRC_UNLOCK_BY_OVERHEAD_TIMER_IN_MS 48
#define TDSRRC_UNLOCK_BY_LTA_1X_PAGE_OVERHEAD_TIMER_IN_MS 35

/*
** RRC Command header. This header is a part of all commands sent
** to the RRC layer. This contains the command id for the command sent
** to RRC, and a Queue link in order to put the command in the
** appropriate queue
*/


/* Please keep the header to be the same size&structure as 
** Other RRC internal/external headers:
** i.e. tdsrrc_cmd_hdr_type in tdsrrccmd_v.h*/
typedef struct
{
  q_link_type   link;        /* Queue link     */
  tdsrrc_cmd_e_type cmd_id;     /* The command Id */
  uint8         seq_num;
}tdsrrc_l1_cmd_hdr_type;


/*--------------------------------------------------------*/
/*                 RRC COMMAND BUFFER                     */
/*                                                        */
/*  The Command buffer used for all RRC command queues is */
/*  defined here. The command buffer contains a RRC       */
/*  command header and a union of all possible commands   */
/*  for any RRC command queue                             */
/*--------------------------------------------------------*/



typedef union
{
    /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by L1          */
  /*--------------------------------------------------------*/
  tdsl1_idle_cnf_type                idle_cnf;
  tdsl1_acq_cnf_type                 acq_cnf;
  tdsl1_freq_scan_cnf_type           freq_scan_cnf;
  tdsl1_setup_cnf_type               setup_cnf;
  tdsl1_cell_select_cnf_type         cell_sel_cnf;
  tdsl1_error_ind_type               err_ind;
  tdsl1_cell_reselec_ind_type        cell_rsel_ind;
  tdsl1_measurement_ind_type         meas_ind;
  tdsl1_phychan_est_ind_type         phy_est_ind;
  tdsl1_cell_transition_cnf_type     cell_transition_cnf;
  tdsl1_drx_cnf_type                 drx_cnf;
  tdsl1_phychan_est_ind_type         l1_phychan_est_ind;
  tdsl1_meas_ctrl_cnf_type           meas_ctrl_cnf;
  tdsl1_act_time_cancel_cnf_type     act_cancel_cnf;


  tdsl1_deactivate_cnf_type          deactivate_cnf;
  tdsl1_stop_cnf_type                stop_cnf;
  tdsl1_start_cnf_type               start_cnf;
  tdsl1_suspend_cnf_type             suspend_cnf;
  tdsl1_resume_cnf_type              resume_cnf;

  tdsl1_dl_weak_ind_type             dl_weak_ind;
  #ifdef FEATURE_TDSCDMA_L1M_SELF_RESET_FW
  tdsl1_rl_failure_recover_fw_type   rl_failure_ind;
  #endif


#ifdef FEATURE_TDSCDMA_DSDS
  tdsl1_out_of_service_type  oos_ind;
#endif
  tdsl1_activation_time_ind_type act_time_ind;

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
    /* Free Floating */
#ifdef FEATURE_TRITON_MODEM 
#else
#ifdef FEATURE_TDSCDMA_MODEM_STATISTICS
  tdsl1_modem_stat_rsp_cmd_type      modem_stat_rsp;
#endif
#endif /*FEATURE_TRITON_MODEM*/
  tdsl1_s_criteria_cnf_type          s_criteria_cnf;
  tdsl1_rssi_info_struct_type     rssi_cnf;
#ifdef FEATURE_TDSCDMA_DATA_FOR_UI_BAR_DISPLAY
  tdsl1_ui_display_info_struct_type  data_for_ui_disp_cnf;
#endif
  tdsl1_sleep_req_type               l1_sleep_req;
  tdsl1_measured_results_struct_type add_meas_info_rsp;
  tdsl1_process_imm_meas_cnf_type    imm_meas_cnf;
#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
  tdsl1_utran_nmr_info_cnf_type      nmr_cnf;
#endif
#endif /* FEATURE_TDSCDMA_FREE_FLOATING_TASK */

  tdsl1_start_qta_cmd_type           start_qta_cnf;

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  tdsl1_g2t_ta_status_type            g2t_ta_mode_ind;
#endif
 /* This dummy field has been added , so that both external and the 
 * RRC internal cmds have the same size for the cmd union. 
 * As the size of the command unoin is determined by the highest data type
 * present in the structure in ARM.*/
  /*Currently disabled for Q6 build*/
  /*uint64 dummyPadding;*/
}tdsrrc_l1_cmd_data_u_type;



/* Please keep this command type to be the same structure as 
** Other RRC internal/external command types:
** i.e. tdsrrc_cmd_type in tdsrrccmd_v.h*/
typedef struct
{
  /* Command header common for all commands */
  tdsrrc_l1_cmd_hdr_type    cmd_hdr;

  tdsrrc_l1_cmd_data_u_type cmd;

}tdsrrc_l1_cmd_type;

typedef enum
{
  TDSL1_CONTROLLER_RRC = 0,
  TDSL1_CONTROLLER_PLT,
  TDSL1_NUM_CONTROLLERS
} tdsl1_conroller_enum_type;


typedef tdsrrc_l1_cmd_type *(*get_controller_cmd_buf_func_type)
(
  void
);

typedef void (*send_msg_to_controller_func_type)
(
  tdsrrc_l1_cmd_type *tdsrrc_l1_cmd_type
);

typedef void *(*controller_malloc_func_type)
(
  size_t size
);

/*CR376929 supply the interface for signal strength to RRC*/
/*----------------------------------------------------------------------------
                         TDSCDMA_L1_SCELL_SIGNAL_STATUS_IND
This primitive is sent from TRRC to upper layer to indicate signal status of 
serving cell. 
This structure contains: 
     - RSSI
     - RSCP 
     - EcIo (RSCP/RSSI)
     - TS0 SINR (CELL_DCH only, default value 5 dBm)
     - BLER (CELL_DCH only, default value 0)
     - AS_ID (TBD - will be added after DSDS/DSDA are supported)  
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr;

  /* (-80 to -4)*256 dBm */
  int16       rssi;

  /* (-120 to -25)*256 dBm */
  int16       rscp;

  /* (RSCP/RSSI)*256. RRC gets raw data from lower layer and passes to upper layer
     in order to keep the accuracy. Conversion (divide 256 and store as float number)
     needs to be done at upper layer.
  */
  int16       ecio;

  /* -15 to 127 dBm
     -15 dBm is sent to upper layer if actual SINR is less than -15 dBm.
     When UE is not in CELL_DCH, SINR is not measured. A mid-value of 5 dBm                                                .
     is sent to upper layer.
  */
  int16        sinr;

  /* This is a percentage rate range from 0 - 63 (%)
     0 is sent to upper layer if UE is not in CELL_DCH.
  */
  uint8       bler;

} tds_l1_scell_signal_status_ind_type;


extern void tdssrch_get_serving_cell_signal_strength_info(tds_l1_scell_signal_status_ind_type *rrc_serving_cell_signal_strength_info);
/*CR376929 supply the interface for signal strength to RRC*/


/*===========================================================================

FUNCTION     TDSL1_REGISTER_UPPER_LAYER_CONTROLLER

DESCRIPTION
  This function is called by upper layer protocol entity, i.e. RRC  or PLT  
  to register with L1 the command buffer allocation function pointer and command 
  transmit function pointer 
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean 

SIDE EFFECTS

===========================================================================*/
extern boolean tdsl1_register_upper_layer_controller
(
  tdsl1_conroller_enum_type upper_layer_controller,
  get_controller_cmd_buf_func_type get_controller_cmd_buf_func,
  send_msg_to_controller_func_type send_msg_to_controller_func,
  controller_malloc_func_type controller_malloc_func
);

/*===========================================================================

FUNCTION RRC_GET_L1_CMD_BUF

DESCRIPTION

  This function returns a L1 command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_l1_cmd_type *tdsrrc_get_l1_cmd_buf( void );



/*===========================================================================

FUNCTION RRC_PUT_L1_CMD

DESCRIPTION

  This function puts a command buffer into RRC's l1
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_l1_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_l1_cmd
(
  tdsrrc_l1_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION rrc_put_l1_ff_cmd

DESCRIPTION

  This function puts a command buffer into RRC's l1 FF
  command queue. Note that the command buffer must have been
  requested through a call to rrc_get_l1_ff_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_l1_ff_cmd
(
  tdsrrc_l1_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);
#endif

/*===========================================================================

FUNCTION TDSL1_MEM_ALLOC

DESCRIPTION
  Allocates memory dynamically from the Modem HEAP.
  
DEPENDENCIES
  None

RETURN VALUE
  Pointer to the allocated memory chunk

SIDE EFFECTS
  None
  
===========================================================================*/
extern void* tdsl1_mem_alloc
(
  size_t               size       /*!< Number of bytes to allocate */
);

/*===========================================================================

FUNCTION TDSL1_MEM_FREE

DESCRIPTION
  De-allocates the dynamically allocated memory from the heap
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void tdsl1_mem_free
(
  void                *ptr        /*!< Memory to free */
);

/*===========================================================================
FUNCTION        L1_GET_RSSI_DURING_WTOW_BPLMN


 DESCRIPTION     This function is used by RRC to get the signal strength of the PLMNs during
                        a BPLMN search. The measured signal level shall be sent with no sign

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    

SIDE EFFECTS    None
===========================================================================*/
void tdsl1_get_rssi_during_wtow_bplmn(tdsl1_rssi_info_struct_type *bplmn_rscp);


/*===========================================================================
FUNCTION        tdsl1_get_scell_meas_result


DESCRIPTION     This function is used by RRC to get the measurement result of
                the serving cell from L1, including RSCP, pathloss and Tadv.
                L1 should also fill in primary freq and cpid of the serving cell.

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    If anything blocks L1 to fill in all the results, return FALSE;
                otherwise return TRUE.

SIDE EFFECTS    None
===========================================================================*/
boolean tdsl1_get_scell_meas_result(tdsl1_cell_meas_result *scell_result);

/*===========================================================================
FUNCTION        tdsl1_get_ncell_meas_result


DESCRIPTION     This function is used by RRC to get the measurement result of
                top 8 neighbor cells (MSET&DSET) from L1, including RSCP and 
                pathloss.
                L1 should update num_ncell according to actual cell numbers,
                and fill in the results one by one to the array ncell_result[]
                with RSCP descending order.
                L1 should also fill in primary freq and cpid of all cells.
                Note: timing_advance is not required to set.

DEPENDENCIES    The SRCH module of Layer 1

RETURN VALUE    If anything blocks L1 to fill in all the results, return FALSE;
                otherwise return TRUE.

SIDE EFFECTS    None
===========================================================================*/
boolean tdsl1_get_ncell_meas_result(uint8 *num_ncell, tdsl1_cell_meas_result ncell_result[]);

/*=========================================================================

 FUNCTION     :TDSL1_GET_MSET_LIST
 
 DESCRIPTION  : get freq list from mset in the RSCP descending order.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   tdsl1_mset_list_type *tdsl1_mset_list : use to save freq set from mset.

 RETURN VALUE : 
   boolean: if no mset, return FALSE. Otherwise, return TRUE.
       
 SIDE EFFECTS : 
==========================================================================*/
boolean tdsl1_get_mset_list(tdsl1_mset_list_type *tdsl1_mset_list);


/*=========================================================================

 FUNCTION     : TDSL1_GET_JDS_CELL_LIST
 
 DESCRIPTION  : RRC get latest JDS cells on specific frequency and continue
   to setup P-CCPCH on these cells. It's used when any cell is barred in initial
   camping phase. When L1SRCH receives the function call, it returns up to
   4 cells in the JDS on the requested frequency.
              
 DEPENDENCIES : 

 INPUT PARAMETERS :
   freq  : frequency to get JDS cells.
   tdsl1_jds_cell_list : number of JDS cells and their CPID.

 RETURN VALUE : 
   boolean: if no JDS, return FALSE. Otherwise, return TRUE.
       
 SIDE EFFECTS : 
   N/A
==========================================================================*/
boolean tdsl1_get_jds_cell_list(uint16 freq, tdsl1_jds_cell_list_type *tdsl1_jds_cell_list);


/*=========================================================================

 FUNCTION     : TDSL1_GET_ORDERED_CONFIG_ACT_TIME
 
 DESCRIPTION  : This is for RRC to get calculated activation time of ordered 
                configuration.
                In order to cancel ordered config that is sent to L1 but not
                activated, RRC need to know the exact activation time.
              
 DEPENDENCIES : RRC invokes this function when a CPHY_SETUP_REQ is pending or
                being performed in L1.

 INPUT PARAMETERS :
   act_time - the calculated activation time (0~255).

 RETURN VALUE : 
   boolean  - TRUE if L1 has an ongoing CPHY_SETUP_REQ for dedicated channels;
              FALSE otherwise.
       
 SIDE EFFECTS : 
   N/A
==========================================================================*/
boolean tdsl1_get_ordered_config_act_time(int16 *act_time);

/*===========================================================================

FUNCTION tdsrrc_get_oos_timer_value

DESCRIPTION This function will be called by TL1 in idle/pch
                state to get the OOS timer value

DEPENDENCIES
                None

RETURN VALUE
                12000/24000 ms

SIDE EFFECTS
                None

===========================================================================*/
uint32 tdsrrc_get_oos_timer_value(void);

/*===========================================================================

FUNCTION        TDSRRC_GET_RESELECTION_PERMISSION_BITMASK

DESCRIPTION     This function will get reselection permission bitmask for TL1

DEPENDENCIES
                None
                bit 0 -- can trigger certain reselection
                bit 1 -- can not trigger certain reselection
RETURN VALUE
                uint8

SIDE EFFECTS
                None

===========================================================================*/

uint8  tdsrrc_get_reselection_permission_bitmask(void);

/*===========================================================================

FUNCTION tdsrrc_get_num_prach_power_increasing_times

DESCRIPTION

  This function is called by L1 to either get V300 value during connection
  setup or V302 value during cell update
  
DEPENDENCIES


RETURN VALUE

  Number of times UE increases PRACH power

SIDE EFFECTS

  None

===========================================================================*/
extern uint8 tdsrrc_get_num_prach_power_increasing_times(void);

/*===========================================================================
FUNCTION tdsl1_write_nv_items

DESCRIPTION
  This function write nv items into efs files.
  This function is directly called by RRC when power-off in both active and inactive mode

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void  tdsl1_write_nv_items(void);
/*===========================================================================

FUNCTION  RRC_IMMEDIATE_PREEMPTION_CB

DESCRIPTION

  This is a call back function to be called in case of pre-emption.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrcwrm_immediate_preemption_cb 
(
  /* The client which requested the unlock event a.k.a. the winning client */
  trm_client_info_t               client_info,

  /* The event being sent to the client */
  trm_unlock_event_enum_t         event,

  /* Sclk timestamp for TRM_UNLOCK_BY */
  uint32                          unlock_by_sclk
);

/*===========================================================================

FUNCTION  TDSRRCWRM_RELEASE_RRC_LOCK_ON_L1_REQ

DESCRIPTION

  This function is called to release RRC lock by TDRM during split ACQ and inter-SIB sleep
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE - If RRC lock is released
  FALSE - Otherwise

SIDE EFFECTS

  None.

===========================================================================*/
boolean tdsrrcwrm_release_rrc_lock_on_l1_req(void);

/*===========================================================================

FUNCTION  TDSRRCWRM_UPDATE_RRC_LOCK_STATUS

DESCRIPTION

  This function is called by TDRM to update RRC lock status if needed. Function is supposed to be called
  after getting TRM lock if TDRM happened to release RRC lock before for split ACQ or inter-SIB sleep

DEPENDENCIES

  None.

RETURN VALUE

  None

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrcwrm_update_rrc_lock_status(void);

/*=========================================================================
FUNCTION tdsl1_get_ul_tx_power_range

DESCRIPTION
  RRC use this interface to query ul tx power range
  
  TxTotalPwr (dBm)	Cost of modem power
  < 0dBm	LOW
  = 0~10dBm	MEDIUM
  >10dBm	HIGH
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
tdsl1_ul_tx_power_range_e_type tdsl1_get_ul_tx_power_range(void);

/*===========================================================================

FUNCTION     tdssrchzz_is_tresel_running_for_any_lte_cells

DESCRIPTION
  This function checks whether Tresel is running for any detected LTE cells 
  across all LTE EARFCNs.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if Tresel is running on any LTE cell
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean tdssrchzz_is_tresel_running_for_any_lte_cells(void);

/*=========================================================================
FUNCTION tdsl1_set_bandF_disabled

DESCRIPTION
  RRC use this interface to tell L1 if bandF is enabled or disabled
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=========================================================================*/
void tdsl1_set_bandF_disabled(boolean is_disabled);

#ifdef FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
/*===========================================================================

FUNCTION tdsrrcpg1_pg_pattern_detect

DESCRIPTION
  Page pattern detect operation function.
  START, CONTINUE or STOP page detection and return page cycle back.
  return SUCCESS -- page detect operation success;
  return FAILURE -- page detect operation fail;
DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS/FAILURE

===========================================================================*/
extern boolean tdsrrcpg1_pg_pattern_detect(tdsrrc_pg_pattern_detect_operation_enum l1_req, uint8 *detect_value_ptr);

#endif

/*===========================================================================

FUNCTION  TDSRRC_GET_EFS_STORAGE_SUB_ID

DESCRIPTION

  This function returns the MCFG sub id value for EFS file access using MCFG APIs. It converts current as_id from
  sys_modem_as_id_e_type to mcfg_fs_sub_id_e_type and returns the result.

DEPENDENCIES

  None.

RETURN VALUE

  sub id of type mcfg_fs_sub_id_e_type for EFS file access

SIDE EFFECTS

  None.

===========================================================================*/
mcfg_fs_sub_id_e_type tdsrrc_get_efs_storage_sub_id(void);

#endif /* TDSL1INF_H */

