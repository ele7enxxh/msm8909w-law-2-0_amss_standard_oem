#ifndef UL_H
#define UL_H

/*============================================================================
              U P L I N K   H E A D E R   F I L E

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
physical channel configuration database and CCTrCH configuration database.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/ul.h_v   1.13   10 Jul 2002 19:40:44   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/ul.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/14/13    tsk     Slot formats 5, 5a and 5b are removed as per spec 25.211 v8.
05/09/13    gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
01/04/13    raj     Changes as a part of ZI memory optimization
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12    vr      HSRACH fixes
02/02/12    raj     Nikel feature cleanup.
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
01/27/12    vs      Nikel feature cleanup.
10/21/11    sp      Nikel UL CM workaround under FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
01/17/11    sp      Added support for UL_RACH_DEBUG_LOG_PKT (0x4025)
01/17/11    sp      Featurized UL RACH FW Log Pkt (0x4026) under FEATURE_WCDMA_GENESIS_SW_FW
01/07/11    sp      Added support for new UL RACH FW Log Pkt (0x4026)
                    Removed support for 0x4180, 0x4181, 0x4182
11/02/10    ksr     Moving LOG_STATUS into periodic framework
04/22/09    gnk     Fixed compiler warnings
10/03/08    hk      Bringing in 7k Mailine fixes
01/28/08    gnk     Maintaining a copy of RRC config parameters in UL
12/08/06    am      Add HSUPA CM support
08/14/06    yiz     Mod to mainline HSUPA code
01/11/06   asm      Changes to remove lint errors
05/31/05    gv      Changes to remove Lint errors
10/15/04    yiz     Change ul_rm_parms_calc() return type to boolean.
10/11/04    gs      Put maro to define max DPCCH slot format out of
                    feature define
02/02/04    yiz     Permanently enable FEATURE_REL_99_JUNE_2001
01/07/04    yiz     Delete ul_ceil() and use math_ceil() in UL.
05/27/03    gs      Added header files l1macdata.h and macl1rrcif.h
                    Defined structure l1_ul_tfcs_restrict_info_struct_type
                    Externalized functions for feature compressed mode
                    TFCS restriction for HLS.
07/25/02    scm     Move L1_UL_MAX_DPCCH_SLOT_FMT from ul.c to ul.h.
07/10/02    gs      Added the enum definition for DPCH sub states.
05/16/02    scm     Moving 3 defines from ul.c to ul.h and adding a
                    function prototype to share with other modules.
02/06/02    sh      Added function ul_get_longest_tti().
01/18/02    gs      1. Externalized functions ul_build_sf_data_tab_cm and
                       ul_get_ul_cm_dpcch_slot_fmt for compressed mode support.
                    2. Changed the structure type ul_sf_data_struct_ary_type
                    3. Externalized globals ul_sf_data_tab_nm, ul_sf_data_tab_cm
                       and ul_sf_data_tab.
03/29/01    sk      Definitions for supporting turbo interleaving.
01/15/01    sk      Updated comments.
09/14/00    sk      file created.
                    
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "queue.h"
#include "l1ulcfg.h"
#include "macl1rrcif.h"
#include "l1macdata.h"

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/

/* See 25.212/4.2.2.2 for these definitions */
#define UL_BLOCK_SIZE_CONV         504
#define UL_BLOCK_SIZE_TURBO        5114
#define UL_TURBO_CODE_BLK_MIN_SIZE 40

/* utility macros to create bit masks */
#define UL_LOW_ZEROS(n)  (-1 << (n))
#define UL_LOW_ONES(n)   (~(uint32)(UL_LOW_ZEROS(n)))

/* maximum number of CCTrCH configurations maintained */
#define UL_CFG_MAX            2
/* uplink CCTrCH data base */
extern l1_ul_cctrch_cfg_struct_type  ul_cctrch_buf[UL_CFG_MAX];

/* The current CCTrCH pointer will always point to the initial 
 * configuration at channel set up. It will be updated whenever a 
 * re-configuration happens at the desired CFN.
 */
extern l1_ul_cctrch_cfg_struct_type *ul_curr_cctrch_ptr;

/* RRC may modify the pointer values so in order to avoid that
   we maintain a local copy of cctrch cfg parameters */
extern l1_ul_semi_static_parms_struct_type  ul_semi_static_local_parms_buf[L1_UL_TRCH_MAX][UL_CFG_MAX];
extern l1_ul_gain_factor_parms_struct_type  ul_gain_factor_local_parms_buf[L1_UL_TFC_MAX][UL_CFG_MAX];
extern mac_ul_tfcs_type ul_local_tfcs_buf[UL_CFG_MAX];

/* Pointer to new CCTrCH configuration */
extern l1_ul_cctrch_cfg_struct_type *ul_new_cctrch_ptr;

/* Some useful macros to access CCTrCH items */
/* Semistatic parameters */
  /* TTI */
#define CCTRCH_TTI(trch) (ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->tti)
  /* Coding Type */
#define CCTRCH_CODING_TYPE(trch) \
  (ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->coding)
  /* Static RM */
#define CCTRCH_STATIC_RM(trch) \
  (ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->staticRM)
  /* CRC */
#define CCTRCH_CRC(trch) (ul_curr_cctrch_ptr->semi_static_parms_ptr[trch]->crc)

/* Gain Factors */
  /* GF calculation type */
#define CCTRCH_GF_CALC_TYPE(tfc_id) \
  (ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->algo)
  /* Control Channel gain */
#define CCTRCH_GF_BETAC(tfc_id) \
  (ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->BetaC)
  /* Data Channel gain */
#define CCTRCH_GF_BETAD(tfc_id) \
  (ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->BetaD)
  /* PRACH power offset valid flag */
#define CCTRCH_PRACH_PWR_OFF_VALID(tfc_id) \
  (ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->valid_Pp_m)
  /* PRACH power offset value */
#define CCTRCH_PRACH_PWR_OFF_VAL(tfc_id) \
  (ul_curr_cctrch_ptr->gain_factor_parms_ptr[tfc_id]->pwr_offset_Pp_m)

/* maximum number of physical channel configurations maintained */
#define UL_PHYCHAN_CFG_MAX           2

/* uplink physical channel configuration data buffer */
extern l1_ul_phychan_cfg_struct_type  ul_phychan_buf[UL_CFG_MAX];

/* current uplink physical channel configuration pointer */
extern l1_ul_phychan_cfg_struct_type *ul_curr_phychan_ptr;

/* new uplink physical channel configuration pointer */
extern l1_ul_phychan_cfg_struct_type *ul_new_phychan_ptr;

/* Uplink status types */
typedef enum {
  UL_SUCCESS,
  UL_FAILURE
} ul_status_enum_type;

/* Spreading Factor Vs (Data Sizes, Min(RM) * Ndata) table.
 * Ndata is defined as number of bits in a radio frame 15*(10 * 2**k).
 * Note that SF = 256/2**k, 0<=k<=6. See 25.211/5.2.1.
 */
typedef struct {
  /* DPDCH slot format */
  uint8   slot_fmt;
  /* data size per frame */
  uint16  Ndata;
  /* Product of Minimum RM among all CCTrCHs and data size per frame */ 
  uint32  MinRM_X_Ndata;
} ul_sf_data_struct_ary_type;

/* SF table */
extern ul_sf_data_struct_ary_type ul_sf_data_tab_nm[];
#define L1_UL_MAX_DPCCH_SLOT_FMT 5
extern ul_sf_data_struct_ary_type ul_sf_data_tab_cm[];

extern ul_sf_data_struct_ary_type *ul_sf_data_tab;

/* Pre-RM bits and Filler bits for the <TrCH Semi-Static parms, TFC>
 * will be computed every time there is data to be transmitted. This
 * can be optimized so that we store them for every TFC in which case
 * we do not need to re-calculate them each time. It is FFS if we need
 * this optimization.
 */ 
typedef struct {
  /* number of code segments */
  uint8  num_code_segs;
  /* number of bits per code segment */
  uint32 num_bits_per_code_seg;
  /* number of pre-rate matched bits for a given TFCI */
  uint32 pre_rm_bits;
  /* number of filler bits to be used in the first code segment */
  uint32 num_filler_bits;
} ul_trch_coding_struct_type;

/* Transport Channel coding parameters */
extern ul_trch_coding_struct_type ul_trch_coding_parms[L1_UL_TRCH_MAX];

/* Turbo Interleaving parameters */
typedef struct {
  /**** TX_TURBO_INTLV_PARAM0_TRCHn parameters ****/
  /* C - 1, where C is the #columns */
  uint8  num_columns_minus1;
  /* last column in the last row */
  uint8  last_column;
  /* #rows in the interleaver table */
  uint8  num_row_index;
  /* exchange indicator (1st column cell with last column cell in last row */
  uint8  last_row_bit_exch;
  /* index of last row with valid data */
  uint8  last_row;

  /**** TX_TURBO_INTLV_PARAM1_TRCHn parameters ****/
  /* index of prime number p used in the index table */
  uint8  prime_num_index;
  /* prime number p */
  uint16 prime_number;
} ul_trch_turbo_ileav_parms_type;

extern ul_trch_turbo_ileav_parms_type  ul_trch_turbo_ileav_parms[L1_UL_TRCH_MAX];

/* Nx,j -> pre-rate matched bits */
#define Nx_j(trch_id) ul_trch_coding_parms[trch_id].pre_rm_bits

/* RM error parameters */
typedef struct {
  /* flag to indicate if puncturing is required */
  boolean punct_flag;
  /* error variable eini1 */
  uint16  eini1;
  /* error variable eini2 */
  uint16  eini2;
  /* error variable eminus1 */
  uint16  eminus1;
  /* error variable eminus2 */
  uint16  eminus2;
  /* error variable eplus1 */
  uint16  eplus1;
  /* error variable eplus2 */
  uint16  eplus2;
} ul_rm_parms_struct_type;

/* TrCH state in a TFC */
typedef enum {
  /* TrCH active */
  TRCH_ACTIVE,
  /* TrCH not active */
  TRCH_INACTIVE
} ul_trch_state_enum_type;

extern ul_rm_parms_struct_type ul_rm_parms[L1_UL_TRCH_MAX];

extern const uint8 tti_Fi_map[L1_NUM_TTI]; 

/* Total # of bits available for a CCTrCH in a 10 ms radio frame with TFC j.
 * This is derived from SF.
 */
extern uint32 Ndata_j;

/* Bit masks in mDSP format for available signatures and sub-channels */
typedef struct {
  /* bit mask of available signature */
  uint16  avail_sig;
  /* bit mask of available sub-channels */
  uint16  avail_sub_chan;
  /* start subchannel number */
  uint8   start_sub_chan;
} ul_asc_avail_mask_struct_type;

extern ul_asc_avail_mask_struct_type ul_asc_mask[L1_UL_ASC_MAX];

typedef enum
{
  UL_DPCH_STATE_SETUP,
  UL_DPCH_STATE_PCP_WAIT,
  UL_DPCH_STATE_TX,
  UL_DPCH_STATE_DISABLED
} ul_dpch_state_enum_type;



/* This structure defines the TFCS restriction info for each TFC */
typedef struct
{
  /* number of total TFCI */
  uint16 num_tfc;

  /* indicate that normal mode Tx is not possible on this TFCS */
  boolean normal_mode_tx_possible[L1_UL_TFC_MAX];
  /* normal mode SF */
  l1_sf_enum_type normal_mode_sf[L1_UL_TFC_MAX];
  /* number of minimum slots needed for transmission based on min SF */
  uint8 num_min_slots_to_tx[L1_UL_TFC_MAX];

  /* number of frames in min and max TTI */
  uint8 num_fr_min_tti;
  uint8 num_fr_max_tti;
} l1_ul_tfcs_restrict_info_struct_type;



/* Number of prime numbers in list for Turbo interleaving parameters */
#define UL_NUM_PRIME_LIST_SZ  52

/* Prime number list. See 25.212 V3.5.0 2000-12 Sec 4.2.3.2.3.1. */
extern const uint16 ul_prime_num_list[UL_NUM_PRIME_LIST_SZ];


extern mac_ul_tfcs_type *ul_local_tfcs_buf_ptr[UL_CFG_MAX];
/* Data structure which holds one boolean variable for each log packet
 * In the begining of WL1 task, during every Enable/Disable of PhyCh
 * regular intervals - LOG Status is checked and updated into this variable
 * In all other instances, the following vairable through GET_LOG_STATUS
 * is used to check the LOG Packet status
 */
typedef struct 
{
   /* RACH Log packets */
   boolean ul_rach_ram_dump;              //UL_RACH_RAM_DUMP_LOG_PKT
   boolean ul_fw_rach_debug;              //UL_FW_RACH_DEBUG_LOG_PKT
   boolean ul_rach_debug;                 //UL_RACH_DEBUG_LOG_PKT
   
   /* Uplink Tx Log packets */
   boolean uplink_tx_rf_debug;            //UPLINK_TX_RF_DEBUG_LOG_PKT
   boolean ul_data_dump;                  //UL_DATA_DUMP_LOG_PKT
   boolean ul_phychan_dpch;               //UL_PHYCHAN_DPCH_LOG_PKT
   
   #ifdef FEATURE_WCDMA_HS_RACH
   boolean ul_hsrach_config;
   boolean ul_hsrach_alloc_edch_rsrc;
   boolean ul_hsrach_chan_timing;
   boolean ul_hsrach_preamble_statue;
   #endif /* FEATURE_WCDMA_HS_RACH */
} uplink_log_status_type;

extern uplink_log_status_type  ul_log_status_info;

#define UL_GET_UL_RACH_RAM_DUMP_LOG_STATUS()                   (ul_log_status_info.ul_rach_ram_dump)
#define UL_SET_UL_RACH_RAM_DUMP_LOG_STATUS(value)              (ul_log_status_info.ul_rach_ram_dump = value)

#define UL_GET_UPLINK_TX_RF_DEBUG_LOG_STATUS()                 (ul_log_status_info.uplink_tx_rf_debug)
#define UL_SET_UPLINK_TX_RF_DEBUG_LOG_STATUS(value)            (ul_log_status_info.uplink_tx_rf_debug = value)

#define UL_GET_UL_DATA_DUMP_LOG_STATUS()                       (ul_log_status_info.ul_data_dump)
#define UL_SET_UL_DATA_DUMP_LOG_STATUS(value)                  (ul_log_status_info.ul_data_dump = value)

#define UL_GET_UL_PHYCHAN_DPCH_LOG_STATUS()                    (ul_log_status_info.ul_phychan_dpch)
#define UL_SET_UL_PHYCHAN_DPCH_LOG_STATUS(value)               (ul_log_status_info.ul_phychan_dpch = value)

#define UL_GET_UL_FW_RACH_DEBUG_LOG_STATUS()                   (ul_log_status_info.ul_fw_rach_debug)
#define UL_SET_UL_FW_RACH_DEBUG_LOG_STATUS(value)              (ul_log_status_info.ul_fw_rach_debug = value)

#define UL_GET_UL_RACH_DEBUG_LOG_STATUS()                      (ul_log_status_info.ul_rach_debug)
#define UL_SET_UL_RACH_DEBUG_LOG_STATUS(value)                 (ul_log_status_info.ul_rach_debug = value)

#ifdef FEATURE_WCDMA_HS_RACH
#define UL_GET_UL_HSRACH_CONFIG_LOG_STATUS()                   (ul_log_status_info.ul_hsrach_config)
#define UL_SET_UL_HSRACH_CONFIG_LOG_STATUS(value)              (ul_log_status_info.ul_hsrach_config = value)

#define UL_GET_UL_HSRACH_ALLOC_EDCH_RSRC_LOG_STATUS()          (ul_log_status_info.ul_hsrach_alloc_edch_rsrc)
#define UL_SET_UL_HSRACH_ALLOC_EDCH_RSRC_LOG_STATUS(value)     (ul_log_status_info.ul_hsrach_alloc_edch_rsrc = value)

#define UL_GET_UL_HSRACH_CHAN_TIMING_LOG_STATUS()              (ul_log_status_info.ul_hsrach_chan_timing)
#define UL_SET_UL_HSRACH_CHAN_TIMING_LOG_STATUS(value)         (ul_log_status_info.ul_hsrach_chan_timing = value)

#define UL_GET_UL_HSRACH_PREAMBLE_STATUS_LOG_STATUS()          (ul_log_status_info.ul_hsrach_preamble_statue)
#define UL_SET_UL_HSRACH_PREAMBLE_STATUS_LOG_STATUS(value)     (ul_log_status_info.ul_hsrach_preamble_statue = value)
#endif /* FEATURE_WCDMA_HS_RACH */

/*=========================================================================

FUNCTION UL_SET_LOG_STATUS_PERIODIC

DESCRIPTION
  This function checks the LOG_STATUS for various UL log codes and updates
  the global structure periodically, which can be used during the regular
  UL logics.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void ul_set_log_status_periodic(void);


/*=========================================================================

FUNCTION UL_BUILD_SF_DATA_TAB

DESCRIPTION

DEPENDENCIES
  CCTrCH configuration must have already been successfully completed

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void ul_build_sf_data_tab
(
  /* Min RM */
  uint16             rm_min
  /* include #slots as argument here for compressed mode ? */
);

/*=========================================================================

FUNCTION UL_BUILD_SF_DATA_TAB_CM

DESCRIPTION
  This function calculates min(RM) * Ndata for all legal Spreading Factors
  for compressed mode.

DEPENDENCIES
  CCTrCH configuration must have already been successfully completed

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void ul_build_sf_data_tab_cm(
  /* Min RM */
  uint16             rm_min);

/*=========================================================================

FUNCTION UL_GET_UL_CM_DPCCH_SLOT_FMT

DESCRIPTION
  This function calculates the DPCCH slot format in compressed mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern uint8 ul_get_ul_cm_dpcch_slot_fmt(void);
/*=========================================================================

FUNCTION UL_PRE_RM_BITS

DESCRIPTION
  This function calculates pre-rate matched bits for a given transport
  channel. The number of pre-rate matched bits depends on the CCTrCH
  configuration for the transport channel and TFC.

DEPENDENCIES
  None

RETURN VALUE
  length of concatenated transport blocks and CRC (both packed)

SIDE EFFECTS
  None

=========================================================================*/
extern uint32 ul_pre_rm_bits
(
  /* transport channel id */
  uint8 trch_id,
  /* Concatenated transport block size in bits */
  uint16 len_tb_concat,
  /* CRC size */
  uint8  crc,
  /* number of transport blocks */
  uint8  num_tb
);

/*=========================================================================

FUNCTION UL_SF_CALC

DESCRIPTION This function calculates the Spreading Factor to be used in
  transmitting data at the next frame boundary.

DEPENDENCIES
  Pre-Rate Matched bits must have already been calculated.

RETURN VALUE
  uint16 - Spreading Factor

SIDE EFFECTS
  None

=========================================================================*/
extern l1_sf_enum_type ul_sf_calc
(
  /* pointer to TrCH states in a tfc */
  ul_trch_state_enum_type *tfc_ptr
);

/*=========================================================================

FUNCTION UL_RM_PARMS_CALC

DESCRIPTION
  This function calculates the rate matching parameters for all TrCHs.

DEPENDENCIES
  Pre-Rate Matched bits must have already been calculated.

RETURN VALUE
  FALSE if encountered error

SIDE EFFECTS
  None

=========================================================================*/
extern boolean ul_rm_parms_calc
(
  /* next radio frame number 0..7 */
  uint8  nrf,
  /* pointer to TrCH states in a tfc */
  ul_trch_state_enum_type *tfc_ptr
);

/*=========================================================================

FUNCTION      UL_GET_LONGEST_TTI

DESCRIPTION
  This function gets the largest TTI from the current CCTrCh.

DEPENDENCIES
  ul_curr_cctrch_ptr should point to the current CCTrCh.

RETURN VALUE
  l1_tti_enum_type value of the longest TTI.

SIDE EFFECTS
  None

=========================================================================*/
l1_tti_enum_type ul_get_longest_tti
(
  void
);



/*=========================================================================
FUNCTION UL_CALC_CM_TFCS_RESTRICT_INFO

DESCRIPTION 
  This function generates the UL TFCS restriction info for complete TFCS
  table. It computes teh normal mode SF, normal mode Tx possible and
  minimum number of slots required for transmission.
  This information is primarily used for TFC restriction for compressed
  mode by SF reduction (to avoid SF 4) and for compressed mode by HLS.

DEPENDENCIES
  It assumes that normal mode SF table has already ben populated.

RETURN VALUE
  None

SIDE EFFECTS
  Effects ul_trch_coding_parms because it uses function ul_pre_rm_bits.
  ul_curr_tfcs_restrict_info_idx is incremented (and rolled over if
  necessary) by 1 to point to new TFCS restriction buffer.
=========================================================================*/

extern void ul_calc_cm_tfcs_restrict_info(
  /* UL CCTrCh info */
  l1_ul_cctrch_cfg_struct_type *cctrch_info,
  /* UL physical channel information */
  l1_ul_phychan_cfg_struct_type *phychan_info,
  /* TFC info */
  mac_ul_tfcs_type *tfcs_info);

/*=========================================================================
FUNCTION UL_QUERY_CM_TFCS_RESTRICTION

DESCRIPTION 
  This function is a call back function that updates the TFCI restriction
  information based on following
  - Number of frame in Max TTI for TrCh that starts Tx at given CFN
  - CM method
  - UL CM information for a given frame
  
  Error scenario are handled as following:
  1. UL current TFCI restriction information not valid (NULL)
  2. call with invalid CFN (CFN with TTI alignment less than min TTI)
     No action is taken. MSG_ERROR is output and just retun.
  3. Invalid TFCI (>= num TFC)
     TFCI set to restricted and MSG_ERROR is output.

DEPENDENCIES
  Current UL TFCI restriction information

RETURN VALUE
  None

SIDE EFFECTS
  Updates restricted field in tfci_list provided by MAC.
=========================================================================*/

extern void ul_query_cm_tfcs_restriction(
  /* Pointer to array of TFCS restriction */
  l1_ul_allowed_tfci_list_type *tfci_list);


/*===========================================================================
FUNCTION GET_NEXT_FRAME_PILOT_BITS

DESCRIPTION
  During normal mode, this function returns 
- Number of pilot bits per slot in normal mode.
  During compressed mode, this function returns 
  - Number of pilot bits per slot in compressed mode.
  
DEPENDENCIES
  Should only be called by EDPCH module in FIQ.

PARAMETERS
  uint8 num_slots_txed

RETURN VALUE
  Returns Number of pilot bits per slot.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 get_next_frame_pilot_bits( uint8 num_slots_txed);
/*===========================================================================
FUNCTION UL_GET_DPCCH_SLT_FMT

DESCRIPTION
  This function returns the dpcch normal mode slot format  
DEPENDENCIES

PARAMETERS

RETURN VALUE
  Returns normal mode dpcch slot format

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ul_get_dpcch_slt_fmt(void);

/*=========================================================================

FUNCTION UL_QUERY_SF_REDUCED_IN_CM_BY_HLS

DESCRIPTION
  This function tells if SF was reduced due to compressed mode by HLS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern boolean ul_query_sf_reduced_in_cm_by_hls(
  /* Potentially reduced SF to be evaluated */
  l1_sf_enum_type sf);

#endif // UL_H
