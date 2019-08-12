#ifndef TDSEULSG_H
#define TDSEULSG_H

/*============================================================================
              WCDMA L1 HSUPA SERVING GRANT AND HARQ RELATED HEADER FILE

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
serving grant and harq information  .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2010-2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdseulsg.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
05/13/13   yuw      Merged triton code base to dime
02/02/11   ps       Moving strucr=t  tdsl1_eul_supported_etfci_struct_type to tdsl1macdata.h
01/11/10   zdang    File created.

                    
===========================================================================*/

/* <EJECT> */

/*===========================================================================

                     DEFINE/UNDEFINE LOCAL FEATURES  

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_TDSCDMA_HSUPA

#include "tdsl1rrcif.h"
#include "tdseul.h"
#include "rex.h"
#include "tfw_sw_intf_msg.h"
#include "tdsl1macdata.h"
#include "tdssrch.h"
#include "tdseulcfg.h"
//#include "tdsedlcfg.h"
#include "tdsl1_nv.h"
#include "tdsl1m_rcinit.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* normalization sacle factopr for available serving grant */
#define TDSEULSG_SG_SCALE_FACTOR  225
/*When the non sg timer is expired or when no DOWN is received for an HARQ RTT,
  then the max available sg value should not be considered */
#define TDSEULSG_SG_INVALID_VAL   0xFFFF

/* MAX number of HAR processes */
#define TDSEULSG_MAX_NUM_HARQ  8

/*max no of active SG values possible 0..37*/
#define TDSEULSG_MAX_NUM_ACTIVE_SG 38  


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*========================================================================*
        Declarations for structures used in the module, for TD-SCDMA
*========================================================================*/


#define TDSEULSG_MAX_NUM_HARQ_PROCESS                   ( 8 )
#define TDSEULSG_MAX_NUM_AGCH_RESULT_BUF_SIZE           ( 2 )
#define TDSEULSG_MAX_NUM_SCHLD_EHICH    ( 4 )

#define TDSEULSG_TPC_ACCUM_LOW_BOND                     ( -190 )
#define TDSEULSG_TPC_ACCUM_HIGH_BOND                    ( +190 )

#define TDSEULSG_MIN_REFERENCE_BETA_DB                  ( -15 )
#define TDSEULSG_MAX_REFERENCE_BETA_DB                  ( +16 )

#define TDSEULSG_BETA_TB_LOW_BOND                       ( -32768 )
#define TDSEULSG_BETA_TB_HIGH_BOND                      ( +32767 )

#define TDSEULSG_MAX_EAGCH_SIR_TARGET                   ( 15360 )   // max E-AGCH SIR target, 15 dB, Q10
#define TDSEULSG_MIN_EAGCH_SIR_TARGET                   ( 2663 ) // min E-AGCH SIR target, 2.6 dB, Q10
#define TDSEULSG_EAGCH_OLPC_DELTA_SIR                   ( 512 )    // 0.5 dB, Q10
#define TDSEULSG_EAGCH_INIT_SIR_TARGET_1                ( 4096 ) // E-AGCH SIR target for BLER 10%~100%, 4 dB, Q10
#define TDSEULSG_EAGCH_INIT_SIR_TARGET_2                ( 7680 ) // E-AGCH SIR target for BLER 1%~10%, 7.5 dB, Q10
#define TDSEULSG_EAGCH_INIT_SIR_TARGET_3                ( 10752 ) // E-AGCH SIR target for BLER 0.1%~1%, 10.5 dB, Q10
#define TDSEULSG_EAGCH_INIT_SIR_TARGET_4                ( 13824 ) // E-AGCH SIR target for BLER 0~0.1%, 13.5 dB, Q10
#define TDSEULSG_EAGCH_OLPC_KMAX                        ( 3 )   // E-AGCH OLPC param, Kmax, 3


#define TDSEULSG_EAGCH_SIR_TARGET                       ( 1e7 )   // TBD, need system input to set the final value

#ifdef FEATURE_TDSCDMA_DSDS_QTA
#define TDSEULSG_EAGCH_OLPC_DSDS_QTA_DURATION_INIT_VAL  (0xFFFF)
#define TDSEULSG_EAGCH_OLPC_DSDS_QTA_DURATION_THRESHOLD (1000)
#define TDSEULSG_EAGCH_OLPC_DSDS_QTA_ALG_LEN_THRESHOLD_MS  (35)     //for QTA lenght as 35ms take as the thresh hold
#endif


#define TDSEULSG_MIN_EPUCH_TX_PWR_IN_DB                  -55  // E-PUCH min TX power

/** number of sub frame for UPH measurement filter */
#define TDSEULSG_NUM_UPH_MEAS_FILT_SUB_FR               10  

/* For RRC <--> L1 interface of Plus flag*/
#define TDSEULSG_PLUS_FLAG_ALPHA                        (102) // IIR filter coef, 0.1, Q10
#define TDSEULSG_PLUS_FLAG_BETA                         (205) // Weight value when large gap of EAGCH (long time EAGCH not received), 0.2, Q10
#define TDSEULSG_PLUS_FLAG_EAGCH_GAP_THRESHOLD          (40)  // how many unit of EAGCH not received, In unit of subframe
#define TDSEULSG_PLUS_FLAG_THRESHOLD                    (922) // threshold of reporting Plus_flag to be TRUE, 0.9, Q10

extern boolean tdseulsg_schld_tx_is_cfg;
extern boolean tdseulsg_non_schld_tx_is_cfg;

#define EDL_RESULT_INTLOCK() REX_ISR_LOCK(&tdseulsg_agch_hich_crit_section)
#define EDL_RESULT_INTFREE() REX_ISR_UNLOCK(&tdseulsg_agch_hich_crit_section)

#define ERUCCH_TX_INFO_INTLOCK()  REX_ISR_LOCK(&tdseulsg_erucch_tx_info_crit_section)
#define ERUCCH_TX_INFO_INTFREE()  REX_ISR_UNLOCK(&tdseulsg_erucch_tx_info_crit_section)

#define EUL_SG_STATE_INTLOCK() REX_ISR_LOCK(&tdseulsg_sg_type_state_crit_section)
#define EUL_SG_STATE_INTFREE() REX_ISR_UNLOCK(&tdseulsg_sg_type_state_crit_section)

#define TDSEULSG_EAGCH_RVD(agch_pos) (TRUE == tdseulsg_eagch_dec_result[agch_pos].valid_flag)

//#define TDSEULSG_SCHLD_TX_CFGED (TDSEDLCFG_IS_EUL_ACTIVE())
#define TDSEULSG_SCHLD_TX_CFGED  tdseulsg_schld_tx_is_cfg

//#define TDSEULSG_NON_SCHLD_TX_CFGED ( tdsedl_non_sched_grant_config_db.current_state == TDSEDL_NON_SCHED_GRANT_ACTIVE )
#define TDSEULSG_NON_SCHLD_TX_CFGED  tdseulsg_non_schld_tx_is_cfg

#define TDSEULSG_RDI_PRESENT ( tdsedl_config_db.agch_info.agch_info_page[TDSEDLCFG_GET_AGCH_CUR_PAGE()].rdi_present == TRUE )  // TBD, add RDI present determination


#define  TDSEULSG_DPCH_SB_BIT_SHT            0
#define  TDSEULSG_DPCH_1_CODE_BIT_SHT        1
#define  TDSEULSG_DPCH_2_CODE_BIT_SHT        2
#define  TDSEULSG_UPPCH_BIT_SHT              3
#define  TDSEULSG_ERUCCH_BIT_SHT             4
#define  TDSEULSG_HSSICH_BIT_SHT             5
#define  TDSEULSG_SCHLD_EPUCH_BIT_SHT        6
#define  TDSEULSG_NON_SCHLD_EPUCH_BIT_SHT    7

#define TDSEULSG_IS_DPCH_SB_PRE(bitmask)          ((bitmask>>TDSEULSG_DPCH_SB_BIT_SHT)&0x1)
#define TDSEULSG_IS_DPCH_1_CODE_PRE(bitmask)      ((bitmask>>TDSEULSG_DPCH_1_CODE_BIT_SHT)&0x1)
#define TDSEULSG_IS_DPCH_2_CODE_PRE(bitmask)      ((bitmask>>TDSEULSG_DPCH_2_CODE_BIT_SHT)&0x1)
#define TDSEULSG_IS_UPPCH_PRE(bitmask)            ((bitmask>>TDSEULSG_UPPCH_BIT_SHT)&0x1)
#define TDSEULSG_IS_ERUCCH_PRE(bitmask)           ((bitmask>>TDSEULSG_ERUCCH_BIT_SHT)&0x1)
#define TDSEULSG_IS_HSSICH_PRE(bitmask)           ((bitmask>>TDSEULSG_HSSICH_BIT_SHT)&0x1)
#define TDSEULSG_IS_SCHLD_EPUCH_PRE(bitmask)      ((bitmask>>TDSEULSG_SCHLD_EPUCH_BIT_SHT)&0x1)
#define TDSEULSG_IS_NON_SCHLD_EPUCH_PRE(bitmask)  ((bitmask>>TDSEULSG_NON_SCHLD_EPUCH_BIT_SHT)&0x1)

#define TDSEULSG_SET_DPCH_SB_PRE(bitmask)          (bitmask |= (0x1<<TDSEULSG_DPCH_SB_BIT_SHT))
#define TDSEULSG_SET_DPCH_1_CODE_PRE(bitmask)      (bitmask |= (0x1<<TDSEULSG_DPCH_1_CODE_BIT_SHT))
#define TDSEULSG_SET_DPCH_2_CODE_PRE(bitmask)      (bitmask |= (0x1<<TDSEULSG_DPCH_2_CODE_BIT_SHT))
#define TDSEULSG_SET_UPPCH_PRE(bitmask)            (bitmask |= (0x1<<TDSEULSG_UPPCH_BIT_SHT))
#define TDSEULSG_SET_ERUCCH_PRE(bitmask)           (bitmask |= (0x1<<TDSEULSG_ERUCCH_BIT_SHT))
#define TDSEULSG_SET_HSSICH_PRE(bitmask)           (bitmask |= (0x1<<TDSEULSG_HSSICH_BIT_SHT))
#define TDSEULSG_SET_SCHLD_EPUCH_PRE(bitmask)      (bitmask |= (0x1<<TDSEULSG_SCHLD_EPUCH_BIT_SHT))
#define TDSEULSG_SET_NON_SCHLD_EPUCH_PRE(bitmask)  (bitmask |= (0x1<<TDSEULSG_NON_SCHLD_EPUCH_BIT_SHT))


#define TDSEULSG_IS_ABORT_DPCH(bitmask)    (((bitmask>>TDSEULSG_DPCH_1_CODE_BIT_SHT)&0x1) | ((bitmask>>TDSEULSG_DPCH_2_CODE_BIT_SHT)&0x1) | ((bitmask>>TDSEULSG_DPCH_SB_BIT_SHT)&0x1))
#define TDSEULSG_IS_ABORT_UPPCH(bitmask)   ((bitmask>>TDSEULSG_UPPCH_BIT_SHT)&0x1)
#define TDSEULSG_IS_ABORT_ERUCCH(bitmask)  ((bitmask>>TDSEULSG_ERUCCH_BIT_SHT)&0x1)
#define TDSEULSG_IS_ABORT_HSSICH(bitmask)  ((bitmask>>TDSEULSG_HSSICH_BIT_SHT)&0x1)
#define TDSEULSG_IS_ABORT_EPUCH(bitmask)   (((bitmask>>TDSEULSG_SCHLD_EPUCH_BIT_SHT)&0x1) | (((bitmask>>TDSEULSG_NON_SCHLD_EPUCH_BIT_SHT)&0x1)))

#define TDSEULSG_SET_DPCH_ABORT_BITMASK(bitmask)   (bitmask |=((0x1<<TDSEULSG_DPCH_1_CODE_BIT_SHT)|(0x1<<TDSEULSG_DPCH_2_CODE_BIT_SHT)|(0x1<<TDSEULSG_DPCH_SB_BIT_SHT)) )
#define TDSEULSG_SET_UPPCH_ABORT_BITMASK(bitmask)  (bitmask |=((0x1<<TDSEULSG_UPPCH_BIT_SHT)) )
#define TDSEULSG_SET_ERUCCH_ABORT_BITMASK(bitmask) (bitmask |=((0x1<<TDSEULSG_ERUCCH_BIT_SHT)) )
#define TDSEULSG_SET_HSSICH_ABORT_BITMASK(bitmask) (bitmask |=((0x1<<TDSEULSG_HSSICH_BIT_SHT)) )
#define TDSEULSG_SET_EPUCH_ABORT_BITMASK(bitmask)  (bitmask |=((0x1<<TDSEULSG_SCHLD_EPUCH_BIT_SHT)|(0x1<<TDSEULSG_NON_SCHLD_EPUCH_BIT_SHT)) )

#define TDSEULSG_CLEAR_DPCH_SB_BITMASK(bitmask)           (bitmask &=~ (0x1<<TDSEULSG_DPCH_SB_BIT_SHT))
#define TDSEULSG_CLEAR_DPCH_1_CODE_BITMASK(bitmask)       (bitmask &=~ (0x1<<TDSEULSG_DPCH_1_CODE_BIT_SHT))
#define TDSEULSG_CLEAR_DPCH_2_CODE_BITMASK(bitmask)       (bitmask &=~ (0x1<<TDSEULSG_DPCH_2_CODE_BIT_SHT))
#define TDSEULSG_CLEAR_UPPCH_BITMASK(bitmask)             (bitmask &=~ (0x1<<TDSEULSG_UPPCH_BIT_SHT))
#define TDSEULSG_CLEAR_HSSICH_BITMASK(bitmask)            (bitmask &=~ (0x1<<TDSEULSG_HSSICH_BIT_SHT))
#define TDSEULSG_CLEAR_ERUCCH_BITMASK(bitmask)            (bitmask &=~ (0x1<<TDSEULSG_ERUCCH_BIT_SHT))
#define TDSEULSG_CLEAR_SCHLD_EPUCH_BITMASK(bitmask)       (bitmask &=~ (0x1<<TDSEULSG_SCHLD_EPUCH_BIT_SHT))
#define TDSEULSG_CLEAR_NON_SCHLD_EPUCH_BITMASK(bitmask)   (bitmask &=~ (0x1<<TDSEULSG_NON_SCHLD_EPUCH_BIT_SHT))

/** target BLER, 0~1, step 0.05, This number ranges from 0 to 63, 0 => 0, 63 => -3.15, step size = 0.05,
     the actual BLER is 10^[], eg. 10^-3.15 */
#define TDSEULSG_EPUCH_OLPC_TARGET_BLER_HIGH               20   // this means 10%
#define TDSEULSG_EPUCH_OLPC_TARGET_BLER_LOW                40   // this means 1%
#define TDSEULSG_EPUCH_OLPC_PWR_LEV_HIGH_TH                (20 * 512)   // if E-PUCH TX power > this one, switch to low BLER target, format Q9
#define TDSEULSG_EPUCH_OLPC_PWR_LEV_LOW_TH                 (17 * 512)   // if E-PUCH TX power < this one, switch to high BLER target, format Q9
#define TDSEULSG_EPUCH_OLPC_ADJ_STEP_SIZE                  512   // default step size, Q9, 0.5 dB
#define TDSEULSG_EPUCH_OLPC_MIN_TPC_ADJ                    0    // min TPC ADJ, Q9, 0 dB
#define TDSEULSG_EPUCH_OLPC_MAX_TPC_ADJ                    512  // max TPC ADJ, Q9, 1 dB

#define TDSEULSG_SNPL_OPTIMIZE_ALPHA               3276  // 0.1, Q15 format
#define TDSEULSG_SNPL_OPTIMIZE_ONE_MINUS_ALPHA     29491 // 0.9, Q15 format

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/* For each cell in the E-DCH active set, the UE shall  receive data  in the 
   E-HICH frame associated with SFN i for  the data transmitted in the E DPDCH 
   frame associated with SFN i-3 for 10 ms TTI. Since HARQ RTT is 4,we need not
   to store the HICH results as we can simply apply the results received in the
   current TTI from FW to the next TTI

   For each cell which belongs to the serving E-DCH radio link set, the UE shall
   apply the  E-RGCH data received in SFN i to E-DPDCH frame associated with SFN
   i+1 for 10 ms TTI. So we need not to store the serving RGCH results as we can
   simply apply the results received in the current TTI from FW to the next TTI

   For each cell which does not belong to the serving E-DCH radio link set,the 
   UE shall apply the E-RGCH data received in SFN i to E-DPDCH frame associated 
   with SFN i+1+s for 10 ms TTI.
   s= ceil{ (160-(tau DPCH/256))/150}. So we need  to store the non serving RGCH 
   results as we can apply the results received in current TTI from FW as given 
   below
   if (tau DPCH/256) is 0-9      
      then  s= 2 so we need to apply resultsto the   current TTI +3
   if (tau DPCH/256) is 10-149   
      then  s= 1 so we need to apply resultsto the   current TTI +2
   So we need to store the non serving RGCH results for three frames for three 
   cells (if the three non serving cells belong to different rg comb idx) at max

   UE shall apply the  E-AGCH data received in SFN i to E-DPDCH frame associated
   with SFN i+1+s for 10 ms TTI.
   s= ceil{ (100-(tau DPCH/256))/150}. So we need  to store the AGCH results as 
   we can apply the results received in the current TTI from FW as given below
   if (tau DPCH/256) is 0-99      
     then  s= 1 so we need to apply results to the   current TTI +2
   if (tau DPCH/256) is 100-149  
     then  s= 0 so we need to apply results to the   current TTI +1
   so we need to store the agch results for two frames at max
*/
//#ifndef FEATURE_WCDMA_HSUPA_2MS_TTI
//#define      TDSEULSG_MAX_AGCH_RESULT_BUF_LEN         2

//#define      TDSEULSG_MAX_NON_SRV_RGCH_RESULT_BUF_LEN 9
//#else

/* The result structures are now re-organized to have results
** on a per target sub frame/frame basis.*/
#define      TDSEULSG_MAX_AGCH_RESULT_BUF_LEN         4
#define      TDSEULSG_MAX_NON_SRV_RGCH_RESULT_BUF_LEN 6

//#endif /* FEATURE_WCDMA_HSUPA_2MS_TTI*/

/* Define invalid values to DL_CHAN_RES_VAL, this is for invalid AG, NS_RG val.
** The per target sub FN structures will be initialized to this value, the AG and
** the NS_RG_VAL. This will be later used to check whether a valid AG or NS_RG
** exists to be considered for a specific target sub frame number*/
#define TDSEULSG_INVALID_EDL_CHAN_RES_VAL 0xFF
#if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
#define TDSEULSG_E_DL_VALID_CRC_PASS_AGCH 1
#define TDSEULSG_E_DL_INVALID_CRC_FAIL_AGCH 0
#define TDSEULSG_E_DL_VALID_CRC_PASS_INVALID_SCOPE_AGCH 2
#endif


/* This value indicates the maximum results that can be obtained per channel
** in an active RL.*/
#define       TDSEULSG_DL_MAX_RES_PER_RL  2


/*This value will be checked only when SG ord page is valid and this special 
  value is used by edl to indicate activation time now case to SG module*/
#define TDSEULSG_ACT_TIME_NONE  0xFFFF

/*This value is default value and no action is taken if it is TDSEULSG_ACT_TIME_NONE*/
#define TDSEULSG_ACT_TIME_INVALID  0xFFFE

/* Macro to check whether the tdseulsg_dl_dpch_in_sync is TRUE*/
#define TDSEULSG_IS_EUL_SG_DL_DPCH_IN_SYNC() (tdseulsg_dl_dpch_in_sync == TRUE)

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/





/*===========================================================================

                     DATA TYPE DEFINITIONS,  for TD-SCDMA

===========================================================================*/


typedef enum
{
  TPC_DOWN = 0, /** increase Pebase by 1 step */
  TPC_UP,   /** decrease Pebase by 1 step */
  TPC_HOLD  /** don't change Pebse */
}tdseul_tpc_cmd_enum_type;

typedef enum
{
  BUILD_TBL_INACTIVE = 0,
  BUILD_TBL_SETUP,
  BUILD_TBL_IN_PROGRESS
}tdseul_build_ss_tbl_state;

typedef enum
{
  BUILD_SS_TBL_FROM_L1M = 0,
  BUILD_SS_TBL_FROM_HSUPA
}tdsl1_eul_build_ss_tbl_src;

typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type       hdr;

  tdsl1_eul_build_ss_tbl_src  build_tbl_src_id;
} tdsl1_eul_build_ss_table_cmd_type;


typedef struct
{
  /** E-PUCH minimum allowed code rate, Integer (0..63), Maps 0.055 to 1.0 in steps of 0.015  */
  uint8 min_allowed_code_rate;
  /** E-PUCH max allowed code rate , Integer (0..63), Maps 0.055 to 1.0 in steps of 0.015 */
  uint8 max_allowed_code_rate;
  /** Reference Beta Information QPSK list length, valid 1 ~ 8 */
  uint8 ref_beta_qpsk_list_len;
  /** reference code rate for QPSK, valid 0~10, uinit range 0~1, step size 0.1 */
  uint8 ref_code_rate_qpsk[8];
  /** reference beta for QPSK, valid -15~16, unit range -15dB ~ 16 dB */
  int8  ref_beta_qpsk[8];
  
  /** Reference Beta Information 16QAM list length, valid 1 ~ 8 */
  uint8 ref_beta_16qam_list_len;
  /** reference code rate for 16QAM, valid 0~10, uinit range 0~1, step size 0.1 */
  uint8 ref_code_rate_16qam[8];
  /** reference beta for 16QAM, valid -15~16, unit range -15dB ~ 16 dB */
  int8  ref_beta_16qam[8];

  /** beta_0_e, in unit 0.001 dB, 5 dimentation table, order are:
      <0> number of E-PUCH timeslot - 1; 0:1 TS; 2:2 TS; ...
      <1> E-PUCH SF index, 0:SF1; 1:SF2; 2:SF4; 3:SF8; 4:SF16
      <2> E-UCCH instance number index, 0:1 E-UCCH; 1: 2 E-UCCH,...
      <3> E-PUCH modulation type index, 0:QPSK, 1:16QAM
      <4> E-TFCI
  */
  int16 beta_0_e[5][5][8][2][64];
} tdseulsg_beta_gain_tab_struct_type;

typedef struct
{
  /* 0 or 1 for double buffer, TDSEDPCH_DBL_BUF_NEITHER_ACTIVE for no-active
   buffer */
  uint32  curr_tab_index;

  /* current inactive semi-static table of E-TFC coding */
  uint32  inactive_tab_index;

  /* buffer index for revert/resume */
  uint32 saved_idx;

  /* Whether need to switch buffers at activation */
  boolean need_to_switch;

  /* Is table built and ready for activation */
  boolean table_ready;

  tdseul_build_ss_tbl_state build_ss_tbl_state;
  
  uint8 build_table_total_steps;

  uint8 curr_build_table_step_cnt;

  

  /* double buffer of semi-static tables of beta_0_e + alpha */
  tdseulsg_beta_gain_tab_struct_type  etfc_beta_gain[2];
} tdseulsg_beta_gain_tab_buf_struct_type;






/** E-HICH for scheduled transmission decode result struct definition */
typedef struct
{
  /** indicator if this item is valid or not */
  boolean valid_flag;
  /** Received E-HICH sub frame number over the air, range 0~8191 */
  uint16 ehich_sub_fn;
  /** Each E-PUCH mapped to a E-HICH, this ie associated E-PUCH transmit sub frame number 
  over the air, range 0~8191 */
  uint16 epuch_sub_fn;
  /** E-HICH indicator */
  uint8 ei;
  /** E-HICH result, ACK or NACK. TRUE:ACK,  FALSE:NACK */
  boolean value;
#ifdef FEATURE_TDSCDMA_HSUPA_DSDA_PWR_BACKOFF
  boolean epuchBackoffFlag;
#endif 
}tdseulsg_schld_ehich_result_struct_type;


/** E-HICH for non-scheduled transmission decode result struct definition */
typedef struct
{
  /** indicator if this item is valid or not */
  boolean valid_flag;
  /** Received E-HICH sub frame number over the air, range 0~8191 */
  uint16 ehich_sub_fn;
  /** Each E-PUCH mapped to a E-HICH, this ie associated E-PUCH transmit sub frame number 
  over the air, range 0~8191 */
  uint16 epuch_sub_fn;
  /** TPC command carried in E-HICH */
  tdseul_tpc_cmd_enum_type tpc;
  /** Sync shift command, carried in non-scheduled E-HICH, 
  Valid [0,1, 2], 0: Down, 1: Up, 2: nothing */
  uint8 ss;
  /** E-HICH result, ACK or NACK. TRUE:ACK,  FALSE:NACK */
  boolean value;
}tdseulsg_non_schld_ehiche_result_struct_type;


/** E-HICH for non-scheduled transmission TPC decode result struct definition */
typedef struct
{
  /** indicator if this item is valid or not */
  boolean valid_flag;
  /** Received E-HICH sub frame number over the air, range 0~8191 */
  uint16 ehich_sub_fn;
  /** Each E-PUCH mapped to a E-HICH, this ie associated E-PUCH transmit sub frame number 
  over the air, range 0~8191 */
  uint16 epuch_sub_fn;
  /** TPC command carried in E-HICH */
  tdseul_tpc_cmd_enum_type tpc;
}tdseulsg_non_schld_ehich_tpc_cmd_dec_result_struct_type;


typedef enum
{
  TDSEULSG_NO_GRANT = 0,
  TDSEULSG_ALLOC_TTI,
  TDSEULSG_SPACING_TTI,
  TDSEULSG_CONFLICT
}tdseulsg_sg_status_enum_type;

typedef struct
{
  /** RDI result, if RRC configure RDI not present in E-AGCH, always set this to 0, otherwise
  use rdi carried in E-AGCH to update this IE */
  uint8 rdi;
  /** grant duration in unit of TTI, including allocated and spacing TTIs, valid 1~13*/
  uint8 duration_in_tti;
  /** indication if this TTI is allocated or spacing, TRUE:allocated TTI; FALSE:spacing TTI */
  boolean resource_allocate[13];
  /** a counter, indicate which one is using in resource_allocate array */
  uint8 tti_cnt;
}rdi_info_struct_type;

/** Serving grant of scheduled transmission for next TTI(5ms), this info 
comes from E-AGCH decode result */
typedef struct
{
  /** serving grant status */
  tdseulsg_sg_status_enum_type sg_status;
  /** power resource related info, unit in dB, valid range: -12 ~ +19 */
  int8 prri;
  /** code resource related info, bvalid range: 0 ~30 */
  uint8 crri;
  /** E-PUCH spreading factor, valid value is :1,2,4,8,16 */
  uint8 sf;
  /** E-PUCH channalisation code number */
  uint8 code;
  /** timeslot resource related infomation, Valid Bit[4:0] for TS1 to TS5, TS1 is MSB */
  uint8 trri;
  /** alpha e, it is related to E-PUCH SF, defined in 25.224, table 2a */
  uint8 alpha_e;
  /** number of E-PUCH timeslot in scheduled transmission serving grant */
  uint8 num_epuch_ts;
  /** timeslot # of E-PUCH in serving grant */
  uint8 epuch_timeslot[5];
  /** E-DCH cyclic sequence number. Valid [0..7] */
  uint8 ecsn;
  /** E-HICH indicator */
  uint8 ei;
  /** E-UCCH number instance number, range 1~8 */
  uint8 num_eucch;
  /** symbol number after modulation of E-PUCH */
  uint16 edch_sym_num;
  rdi_info_struct_type rdi_info;
#ifdef FEATURE_TDSCDMA_HSUPA_DSDA_PWR_BACKOFF
  boolean eagchMode; 
  int16   txBackoffMtplQ4;
  uint8   txBackoffSlotCnt;
  uint8   txBackoffBitMask;
#endif 
}tdseulsg_schld_grant_info_struct_type;


/** Serving grant of non-scheduled transmission, thie infomation comes from 
RRC message when HSUPA is setup/reconfig */
typedef struct
{
  /* indicate if non-scheduled transmission is configured or not */
  boolean valid_flag;
  /** indicate if grant is available for next TTI or not */
  boolean grant_available;
  /** power resource related info, unit in dB, valid range: -12 ~ +19 */
  int8 prri;
  /** code resource related info, bvalid range: 0 ~30 */
  uint8 crri;
  /** E-PUCH spreading factor, valid value is :1,2,4,8,16 */
  uint8 sf;
  /** E-PUCH channalisation code number */
  uint8 code;
  /** timeslot resource related infomation, Valid Bit[4:0] for TS1 to TS5, TS1 is MSB */
  uint8 trri;
  /** alpha e, it is related to E-PUCH SF, defined in 25.224, table 2a */
  uint8 alpha_e;
  /** number of E-PUCH timeslot in scheduled transmission serving grant */
  uint8 num_epuch_ts;
  /** timeslot # of E-PUCH in serving grant */
  uint8 epuch_timeslot[5];
  /** E-UCCH number instance number, range 1~8 */
  uint8 num_eucch;
  /** symbol number after modulation of E-PUCH */
  uint16 edch_sym_num;
  /** Specifies the E-PUCH Offset in Radio Frame level, 0~4095 */
  uint16 activation_time;
  /** Specifies the E-PUCH Offset in subframe leve, 0 or 1 */
  uint8 subframe_num;
  /** Periodicity in terms of TTIs for which resource is allocated. 
  1= continuous allocation, 2 indicates every other TTI, 4 = every 4th, 8 every 8th etc.
  {1, 2, 4, 8, 16, 32, 64} */
  uint8 rep_period;
  /* TTI in which resource is assigned. If Repetition period = 1 then value is ignored */
  uint8 rep_length;
  /** calculated offset in TTI,
  offset_sub =(Activation time* 2+ Subframe number) mod Repetition period */
  uint8 offset_sub;
}tdseulsg_non_schld_grant_info_struct_type;


/** outer loop power control type */
typedef enum
{
  TDSEULSG_OLPC_FIX_BLER_TGT = 0,  // fix BLER target type outer loop power control
  TDSEULSG_OLPC_ADPT_BLER_TGT   // adpaptive BLER target type outer loop power control
}tdseulsg_sg_olpc_enum_type;


typedef struct
{
  boolean olpc_on;
  tdseulsg_sg_olpc_enum_type olpc_type;
  uint8 bler_tgt;
  uint8 bler_tgt_H;
  uint8 bler_tgt_L;
  int16 pwr_th_H;
  int16 pwr_th_L;
  uint16 adj_step_size_Q9;
  int32 min_tpc_adj_Q9;
  int32 max_tpc_adj_Q9; 
}tdseulsg_epuch_olpc_params_struct_type;

typedef struct
{
  /** Pe-base, unit of 1dBm */
  int16 pebase;
  /** TPC accumulate result, unit of 1dB */
  int16 tpc_accum;
  /** Pe-base + pass loss result, in unit of 1/16 dBm */
  int16 pebase_plus_L_Q4;
  /** PRXdes_base, this is configured by RRC message when HSUPA is setup/reconfigure, unit of 1dBm */
  int16 prx_des_base;
  /** First time E-PUCH transmit flag */
  boolean init_tx_flag;
  /** pass loss when first time of E-PUCH transmission, unit of 1/16 dB */
  uint16 init_tx_pathloss_Q4;
  /* current passloss, unit of 1/16 dB */
  uint16 cur_pathloss_Q4;
  /** Pe-base power control gap, Unit: Number of subframes,  Default value is 1.
  Value 255 represents infinite in which case closed loop power control shall always be used
  */
  uint16 pebase_pwr_ctl_gap;
  /** a counter, used to count how many sub frames between two consective TPC command, 
  for scheduled transmission, TPC command is carried in E-AGCH,
  for non-scheduled transmission, TPC command is carried in E-HICH */
  uint16 gap_cnt;
  /* TPC step size, 1: 1dB; 2: 2dB; 3: 3dB */
  uint8 tpc_step;
  /** Beacon PL Est, carried in RRC message, TRUE indicates that the UE may take into account 
  path loss estimated from beacon function physical channels. Default value is FALSE*/
  boolean pl_est_enable;
  /** Pe-base, in unit of 1/512 dBm */
  int32 pebase_Q9;
  /** Pe-base + pass loss result, in unit of 1/512 dBm */
  int32 pebase_plus_L_Q9;
  /** step size of increase TPC adjustment, unit is 1/512 dB */
  int32 delta_up_Q9;
  /** step size of decrease TPC adjustment, unit is 1/512 dB */
  int32 delta_down_Q9;
  /** TPC adjustment, unit is 1/512 dB */
  int32 tpc_adj_Q9;
  /** TPC adjustment, down step size 1, for low BLER target */
  int32 tpc_down_step_size_1;
  /** TPC adjustment, down step size 1, for high BLER target */
  int32 tpc_down_step_size_2;
  /** TPC adjustment, current BLER target */
  uint8 curr_bler_target;
} tdseulsg_epuch_pwr_ctrl_struct_type;

typedef enum
{
  /** Initial state of a HARQ process, indicate HARQ process is not used, 
  and could be selected to transmit new data */
  TDSEUL_INACTIVE_HARQ_PROC = 0,
  /** HARQ process is selected for new transmission, or call it initial transmission,
  and E-HICH is not received for that HARQ process yet */
  TDSEUL_INIT_TX_HARQ_PROC,
  /** HARQ process is selected for retransmission, HARQ data may or may not be sent,
  E-HICH is not received yet */
  TDSEUL_RE_TX_HARQ_PROC,
  /** NACK in E-HICH is received, and HARQ process is pending for retransmission */
  TDSEUL_NACK_RVD_HARQ_PROC,
  /** RTX_TIMER expires, HARQ process ID need to be reported to MAC, and reset to
  TDSEUL_INACTIVE_HARQ_PROC after that */
  TDSEUL_TIMEOUT_HARQ_PROC,
  /** This status is only used for the set report to MAC failed HARQ ID, indicate that 
  failed HARQ process is because of total number of transmission exceed limit */
  TDSEUL_EXCEED_TX_NUM_HARQ_PROC
} tdseulsg_harq_status_enum_type;

typedef enum
{
  TDSEULSG_NO_GRANT_FOR_NEXT_TTI = 0,
  TDSEULSG_SCHLD_TX_FOR_NEXT_TTI,
  TDSEULSG_SPACING_FOR_NEXT_TTI,
  TDSEULSG_NON_SCHLD_TX_FOR_NEXT_TTI
} tdseulsg_grant_for_next_tti_enum_type;

typedef enum
{
  TDSEULSG_ETFCI_LIMIT_BY_SG,
  TDSEULSG_ETFCI_LIMIT_BY_PWR
} tdseulsg_etfci_limit_reason_enum_type;

typedef struct
{
  /** max allowed UL transmit power, set by RRC, in unit of 1dBm */
  int16 max_allowed_ul_tx_pwr;
  /** max UE transmit power, get from FW interface, including power limit by PDET, 
      in unit of 1/16 dBm, 16 = 1dBm  */
  int16 pwr_max_adjusted_Q4;
  /** power headroom, which can be used by E-PUCH, 
      available_pwr = max_tx_pwr - dpch_pwr - hs_sich_pwr - erucch_pwr
      in unit of 1/16 dBm, 16 = 1dBm */
  int16 epuch_pwr_avail_Q4;
  /** max E-PUCH power by grant, which means E-PUCH power when use full grant,
      unit is Q4 dBm, +16 = 1dBm  */
  int16 max_pwr_by_grant_Q4;
  /** alpha e, related to E-PUCH SF, defined in 25.224, Table 2a */
  uint8 alpha_e;
  /** absolute power grant info, from E-AGCH or RRC cfg */
  int16 prri;
  /** max supported beta_0_e + harq power offset, unit is 0.001 dB, 1000 = 1 dB. 
      this is determined both by PRRI in E-AGCH or RRC cfg, 
      and also current power headroom */
  int32 max_beta_0_e_plus_po_db1k;
#ifdef FEATURE_TDSCDMA_HSUPA_DSDA_PWR_BACKOFF
  int32 max_beta_0_e_plus_po_db1k_bkoff;
#endif 
  /** if set to TURE, it means both QPSK and 16QAM can be supported, otherwise only QPSK
      is supported */
  boolean both_qpsk_and_16qam;
  /** E-TFCI limited reason, by serving grant or by available E-PUCH power */
  tdseulsg_etfci_limit_reason_enum_type etfci_limit_reason;
  /** UL power info, dump from FW */
  tfw_ul_pwr_info_t fw_ul_pwr_info;

  /** UE UPH measurement part */
  /** current UE transmission power headroom, unit of 1dB */
  uint16 curr_uph;
  /** current write index */
  uint16 update_idx;
  /** UPH measurement history */
  uint16 uph_history[TDSEULSG_NUM_UPH_MEAS_FILT_SUB_FR];

  /** UE max TX power limit when calculate UPH, unit is dBm */
  int8 mtpl_for_uph;
  /** UE max TX power limit when select E-TFCI, unit is dBm */
  int8 mtpl_for_etfci_sel;
  
} tdseulsg_epuch_pwr_headroom_struct_type;



typedef enum
{
  TDSEULSG_EPUCH_MOD_TYPE_QPSK = 0,
  TDSEULSG_EPUCH_MOD_TYPE_16QAM
} tdseulsg_mod_type_enum_type;

typedef struct
{
  tdseulsg_grant_for_next_tti_enum_type grant_next_tti;
  /** TRUE, next TTI is new transmission;
      FALSE, next TTI is retransmission */
  boolean new_tx;
  /** number of E-PUCH timeslot */
  uint8 num_epuch_ts;
  /** E-UCCH instance number, 1 ~ 8 */
  uint8 num_eucch;
  /** E-PUCH SF index, 0:SF1; 1:SF2; 2:SF4; 3:SF8; 4:SF16  */
  uint8 epuch_sf_idx;
  /** selected HARQ ID, for new tx or re-tx */
  uint8   harq_id;
  /** E-TFCI for next TTI, could be new TX, or RE-TX */
  uint8 etfci;
  /** modulation type, QPSK , 16QAM */
  tdseulsg_mod_type_enum_type   mod_type;
  /** E-PUCH TX power, unit Q4 dBm, +16 = 1dBm */
  int16 epuch_tx_pwr_Q4;
  /** next TTI DRX flag, TRUE: DTX  */
  boolean dtx_flag;
  /** E-PUCH TX power, Q9, unit is 1/512 dBm */
  int32 epuch_tx_pwr_Q9;
  /*Timeslot number */
  uint8  time_slot[TDSL1_UL_MAX_NUM_TS];
  uint32 buf_status;
} tdseulsg_next_tti_tx_info_struct_type;


/*This structure maintains data specific to each harq process*/
typedef struct
{
  /** HARQ process status */
  tdseulsg_harq_status_enum_type harq_proc_state;
  /** number of E-PUCH timeslot when this HARQ ID is selected for initial transmission, 
  if this HARQ need to retransmit, it can only be selected if in current grant, 
  number of E-PUCH timeslot is the same as that for initial transmission */
  uint8 num_epuch_ts;
  /** E-TFCI of this HARQ process */
  uint8 etfci;
  /** power offset, set by MAC */
  uint8 power_offset;
  /** only SI is carried in this HARQ process flag */
  boolean si_sent_alone;
  /** max number of transmission allowed, including initial transmission and retransmission, 
  set by MAC, in HARQ profile */
  uint8 max_num_trans_allowed;
  /**Each HARQ process maintains the state variable CURRENT_TX_NB, which indicates
    the number of transmissions that have taken place for the MAC-e PDU currently
    in the buffer. When the HARQ process is established, CURRENT_TX_NB shall be
    initialized to 0 */
  uint8 cur_tx_nb;
  /** CURRENT_RSN , initial value is 0, updated by CURRENT_TX_NB, valid range is from 0~3, 
  updated sequence patten is: 0,1,2,3,2,3,2,3...   */
  uint8 cur_rsn;
  /** RTX_TIMER expire value, unit of ms */
  uint16 rtx_timer_expire_value;
  /** retransmission time elapsed, unit of ms */
  uint16 rtx_time_elapsed;
  /** E-HICH indicator, valid range is from 0 ~ 3 */
  uint8 ei;
  /** Last HARQ process sent sub frame number, initial value is 0xFFFF */
  uint16 last_tx_sub_fn;
  /** HARQ DTX flag, if this HARQ is selected but not transmitted over the air, set to TRUE,
      otherwise FALSE */
  boolean dtx_flag;
  /** pe-base when send this HARQ */
  int16 pebase;
  /** epuch transmit power when send this HARQ */
  int16 tx_pwr_q4;

  /** E-PUCH TX power, Q9, unit is 1/512 dB */
  int32 tx_pwr_q9;
  /** Pe-base, Q9 */
  int32 pebase_q9;

} tdseulsg_harq_param_struct_type;

/** A set definition, contain all failed HARQ process ID, 
and these HARQ ID need to report to MAC */
typedef struct
{
  /** number of HARQ process need to report to MAC */
  uint8 num_harq_proc_rpt;
  /** HARQ ID array, need to report to MAC */
  uint8 harq_id[TDSEULSG_MAX_NUM_HARQ_PROCESS];
  /** indicate falid reason for each HARQ ID */
  tdseulsg_harq_status_enum_type harq_state[TDSEULSG_MAX_NUM_HARQ_PROCESS];
} tdseulsg_failed_harq_set_struct_type;

/** A set definition, contain all HARQ process pendind for retransmission, the struct type of 
scheduled and non-scheduled transmission are the same, but the set is defined individually */
typedef struct
{
  /** number of HARQ process need to report to MAC */
  uint8 num_harq_process;
  uint8 harq_id[TDSEULSG_MAX_NUM_HARQ_PROCESS/2];
  uint16 rtx_time_elapsed[TDSEULSG_MAX_NUM_HARQ_PROCESS/2];
} tdseulsg_pending_rtx_set_struct_type;

typedef struct
{
  /** this indicate if E-AGCH outer loop power control is applied or not */
  boolean valid;
  /** BLER quality target set by RRC.
     This number ranges from 0 to -63, 0 => 0, -63 => -3.15, step size = 0.05,
     the actual BLER is 10^[], eg. 10^-3.15 */
  int8 bler_quality_target;
  /** initial value of SIR target, dB, Q10 */ 
  int32 init_sir_target;
  /** SIR target, will passed to FW, Q10 */
  int32 sir_target;
  /** Delta SIR, dB, Q10 */
  int32 delta_sir;
  /** SIR target down adjust step size, dB, Q10 */
  int32 sir_down_step_size;
  /** SIR target up adjust step size, dB, Q10 */
  int32 sir_up_step_size;
  /** number of SIR target adjust up */
  uint32 target_up_count;
  /** number of SIR target adjust down */
  uint32 target_down_count;
  /** max SIR target limit, dB, Q10 */
  int32 max_target_sir;
  /** min SIR target limit, dB, Q10 */
  int32 min_target_sir;
  /** ECSN carried in last received E-AGCH */
  uint8 last_ecsn;
  /** ESCN carried in current received E-AGCH */
  uint8 current_ecsn;
  /** a flag to indicate if this is the first received E-AGCH */
  boolean first_rvd_eagch;
  /** a counter to indicate how many subframes eagch has not been received */
  uint8 continuous_no_agch;
  /** SIR target down adjust step size, inital value, dB, Q10 */
  int32 sir_down_step_init;
  /** SIR_down_step_size x (k - Kmax -1 ), all possible combination */
  uint16 sir_down_step_x_k_subs_Kmax[7];
#ifdef FEATURE_TDSCDMA_DSDS_QTA
  uint16 dsds_qta_eagch_oplc_dur_ms;
  uint8  dsds_qta_ignored_eagch_number;
#endif
}tdseulsg_eagch_olpc_vars_struct_type;

typedef enum
{
  TDSEUL_NO_ERUCCH,  /* initial state, no E-RUCCH is triggered */
  TDSEUL_ERUCCH_TX_START, /* MAC indicate L1 to send E-RUCCH, L1 havn't received FPACH yet */
  TDSEUL_ERUCCH_FPACH_ACK_RVD, /* FPACH is received */
  TDSEUL_DURING_ERUCCH_CANCEL /* when MAC build E-RUCCH PDU, if TEBS=0, MAC indicate L1 to cancel E-RUCCH, L1 is doing it now,
                               and not finished yet, if cancel is done, state will return to TDSEUL_NO_ERUCCH */
} tdseulsg_erucch_tx_status_enum_type;

typedef struct
{
  tdseulsg_erucch_tx_status_enum_type erucch_status;
  /* ASC index selected by MAC */
  uint8 asc_index;
  /* E-RUCCH configuration */
  tdsl1_e_rucch_info_struct_type      *erucch_cfg;
  tdsl1_eul_etfci_enum_type  erucch_type;
} tdseulsg_erucch_tx_info_struct_type;



typedef struct
{

  /** indicate if this item is valid or not */
  boolean valid_flag;
  /** sub frame number, indicate at what time E-AGCH is received over the air */
  uint16 sub_fn;
	/** power resource related info, unit in dB, valid range: -12 ~ +19 */
	int8 prri;
	/** code resource related info, bvalid range: 0 ~30 */
	uint8 crri;
	/** timeslot resource related infomation, bit[4:0] for TS1 to TS5, TS1 is MSB */
	uint8 trri;
	/** E-DCH cyclic sequence number. Valid [0..7] */
	uint8 ecsn;
	/** E-HICH indicator */
	uint8 ei;
	/** E-UCCH number indicator, range: 0~7, represent E-UCCH instance number 1~8 */
	uint8 eni;
	/** TPC command carried in E-AGCH */
	tdseul_tpc_cmd_enum_type tpc;
	/** RDI result */
	uint8 rdi;
	/** EAGCH Decode Status */
	uint8 errorCnt;
	/** The good/bad quality bit for coded data for E-AGCH */
	uint8 qualityBit;
	/** Sign-extended value of the accumulated energy metric for coded data for
	  E-AGCH */
	int16 energy_metric;
	/** SS to control E-PUCH, carried in E-AGCH */
	uint8 ss;
#ifdef FEATURE_TDSCDMA_HSUPA_DSDA_PWR_BACKOFF
  boolean eagchMode; 
  int16   txBackoffMtplQ4;
  uint8   txBackoffSlotCnt;
  uint8   txBackoffBitMask;
#endif 
}tdseulsg_agch_results_struct_type;



typedef struct
{
  boolean is_dpch_coexist;
  boolean is_uppch_coexist;
  boolean is_erucch_coexist;
  boolean is_hssich_coexist;
  uint8 dpch_type; /* 0: Special Burst, 1: 1 code; 2: 2 codes */
  uint8 num_epuch_ts;
  uint8 epuch_ts[5];
  boolean abort_uppch;
  boolean abort_hssich;
  boolean abort_epuch;
  uint8 ch_coexist_bitmask;
  uint8 ch_abort_bitmask;
  uint8 num_dpch_epuch_comm_ts;
  uint8 dpch_epuch_comm_ts[5];
} tdseulsg_ul_ch_conflict_db_struct_type;

typedef struct
{
  boolean is_schld_ehich_reg;
  uint16 schld_ehich_sub_fn;
  uint16 schld_ehich_ts_bitmap;
  boolean is_non_schld_ehich_reg;
  uint16 non_schld_ehich_sub_fn;
  uint16 non_schld_ehich_ts_bitmap;
} tdseulsg_reg_ehich_slot_struct_type;

typedef struct
{
  boolean snpl_opt_enable;
  boolean reset_delta_snpl;
  boolean acked_in_current_sub_frame;
  int16 beta_supported;
  int16 beta_grant;
  uint8 max_etfci_supported;
  uint8 max_etfci_grant;
  uint16 alpha;
  uint16 one_minus_alpha;
  uint32 delta_snpl; // Q16
  uint8 snpl_original;
  uint8 snpl_report;
} tdseulsg_snpl_optimize_struct_type;

#if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
typedef struct{
  /* This is the hich energy and threshold energy for this RL*/
//  tdseulsg_e_dl_eng_thresh_result_struct_type hich_info;
  /* This is the rgch energy and threshold energy for this RL*/
//  tdseulsg_e_dl_eng_thresh_result_struct_type rgch_info;
  /* HICH Value for this cell, ACK/NACK/DTX*/
  uint8 hich_val;
  /* RGCH Val for this cell UP/DOWN/HOLD*/
  uint8 rgch_val;
  /* Boolean indicating whether this RGCH is serv_rls*/
  boolean is_serv_rls;
}tdseulsg_logging_per_cell_info_struct_type;

/* Per RL configuration information for 4309/4311 log packet*/
typedef struct{
  /* PSC */
  uint16 psc;
  uint8 tpc_idx;
  uint8 rls_idx;
}tdseulsg_logging_edl_per_rl_config_info_struct_type;

/* This information does not change on a per TTI basis. So have this
** as a separate structure so that it is easy to fill at one before committing
** the log packet.*/
typedef struct{
  /*Num TPC sets*/
  uint8 num_tpc_sets;
  /*Num RLS*/
  uint8 num_rls;
  /* Num Cells*/
  uint8 num_cells;
  /* Serving Cell index*/
  uint8 serv_cell_idx;
  /* Per RL config_info like PSC etc*/
  tdseulsg_logging_edl_per_rl_config_info_struct_type per_rl_config_info[TDSEUL_MAX_RL];
}tdseulsg_logging_edl_config_info_struct_type;

/* The structure below will be used for getting information for the 4309 log packet.*/
typedef struct{
  /* This is the soft combined energy and threshold on a per TPC set basis*/
//  tdseulsg_e_dl_eng_thresh_result_struct_type hich_soft_eng_thresh_info[TDSEUL_MAX_RL];
  /* This gives the soft comibined serv RGCH energy.*/
  int32 s_rg_eng;
  /* This gives the soft combined serv_rgch threshold*/
  int32 s_rg_thresh;
  #if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
  int32 s_rg_thresh_ul_logging;
  #endif
  /* AGCH quality parameters*/
  uint16 eng_word;
  uint16 err_cnt_word;
  /* Per cell information*/
  tdseulsg_logging_per_cell_info_struct_type per_cell_info[TDSEUL_MAX_RL];
  /* Num cells..This can be fixed at the config time, but there 
  ** could be cases during reconfig when we start using new config with 
  ** more or fewer cells.*/
  uint8 num_cells;
}tdseulsg_logging_dl_results_struct_type;

typedef struct
{
  uint16 crc_pass;
  /* AGCH quality parameters*/
  uint16 eng_word;
  uint16 err_cnt_word;
  uint16 vd_output;
  uint16 cm_slot_bmsk;
}tdseulsg_logging_agch_info_struct_type;
#endif /* FEATURE_TDSCDMA_HSUPA_L1_LOGGING_ENABLED*/


#ifdef FEATURE_TDSCDMA_WLAN_COEX
extern boolean tdseulsg_hsupa_configured;
#endif

/*=========================================================================

FUNCTION tdseulsg_channel_rpt

DESCRIPTION
  This function can provide channel number related with UPA in the next subframe. Should be called at TS5

DEPENDENCIES
  None.
  
RETURN VALUE
  channel number
  
SIDE EFFECTS
  None.

=========================================================================*/
extern uint8 tdseulsg_channel_rpt(void);

extern const uint8 tdseulsg_crri_to_sf_table[31];

extern const uint8 tdseulsg_sf_to_sf_idx_table[17];

extern const uint8 tdseulsg_alpla_e_tb[5];


extern uint16 tdseulsg_cat_1_2_etfci_table[3][64];
extern uint16 tdseulsg_cat_3_to_6_etfci_table[5][64];

/*extern tdseulsg_beta_gain_tab_buf_struct_type  tdseulsg_beta_gain_buf;*/
extern tdseulsg_beta_gain_tab_buf_struct_type  *tdseulsg_beta_gain_buf_ptr;

extern tdseulsg_agch_results_struct_type tdseulsg_eagch_dec_result[TDSEULSG_MAX_NUM_AGCH_RESULT_BUF_SIZE];

extern tdseulsg_schld_ehich_result_struct_type tdseulsg_schld_ehich_result[2];

extern uint8 tdseulsg_num_ehich_rvd;

extern tdseulsg_non_schld_ehiche_result_struct_type tdseulsg_non_schld_ehich_reslt;

extern tdseulsg_non_schld_ehich_tpc_cmd_dec_result_struct_type tdseulsg_non_schld_echich_tpc_cmd_dec_result;

extern tdseulsg_schld_grant_info_struct_type tdseulsg_schld_sg_info;

extern tdseulsg_non_schld_grant_info_struct_type tdseulsg_non_schld_sg_info;

extern tdseulsg_epuch_pwr_ctrl_struct_type eul_epuch_pwr_ctl_info;

extern tdseulsg_epuch_pwr_headroom_struct_type tdseulsg_epuch_pwr_headroom_info;

extern uint8 max_sg_etfci, max_pwr_etfci;


extern tdseulsg_eagch_olpc_vars_struct_type tdseulsg_eagch_olpc_vars;

extern tdseulsg_erucch_tx_info_struct_type tdseulsg_erucch_tx_status;

extern tdseulsg_snpl_optimize_struct_type tdseulsg_snpl_opt;
extern boolean tdseulsg_enable_snpl_opt;


/** data struct used to register MEASUREMENT module about E-PUCH timeslot usage */
extern tdssrch_meas_epuch_config_struct_type schld_epuch_ts_usage_info;
extern tdssrch_meas_epuch_config_struct_type nonschld_epuch_ts_usage_info;


/* ==========================================================================
** Public Data Declarations
** ========================================================================*/
/* Variable to indicate whether downlink DCH is in sync or not*/
extern boolean tdseulsg_dl_dpch_in_sync;




/*The CFN value and should be accessed only inside the EUL FIQ*/
extern uint8 tdseulsg_eul_fiq_global_cur_fn;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */


extern void tdseulsg_build_gain_factor_ss_tbl ( tdseulsg_beta_gain_tab_struct_type *beta_gain_tbl_ptr);

extern void tdseulsg_schld_sg_databse_init(void);

extern void tdseulsg_init_harq_proc(boolean init_all_harq, uint8 harq_id);

extern void tdseulsg_init_all_rdi_type(void);

extern void tdseulsg_non_schld_sg_database_init(void);

#ifdef FEATURE_TDSCDMA_DSDS_QTA
/*===========================================================================
FUNCTION tdseulsg_eagch_olpc_set_dsds_qta_dur_ms

DESCRIPTION
  This function set the HSSCCH olpc duration time.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/

extern void tdseulsg_eagch_olpc_set_dsds_qta_dur_ms(uint16 qta_dur_ms);
#endif

extern void tdseulsg_eagch_olpc_enable(void);

extern void tdsedpch_etfc_restrict_log_pkt_hdr_info(void);

extern void tdsedpch_etfc_restrict_log_pkt_sample_info(boolean dtx_flag);

extern uint8* tdseul_erucch_fpach_ack_rvd(boolean fpach_ack);

extern void tdseul_erucch_tx_done(boolean send_success);
extern void tdseulsg_erucch_abort_done(void);

extern int16 tdseulsg_conv_2_betaD_div_to_dB_Q4(uint16 betaD_1_Q5, uint16 betaD_2_Q5);

extern void tdseul_erucch_with_tebs_zero_cancel_done(void);

/*===========================================================================
FUNCTION     TDSEULSG_SET_HARQ_PARAMS

DESCRIPTION  This function passes reference ETPR, max retransmissions allowed
             on the current HARQ process, whether there is a scheduled
             transmission in prev TTI of the harq process and whether current
             HARQ is TXed. This information is used in Serving Grant update
             for the same HARQ process at next time.

DEPENDENCIES Called by E-DCH module at the end of FIQ after UL E-DCH data path
             is completed.

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void tdseulsg_set_harq_params( uint8 harq_id, boolean dtx_flag );

extern void mcaltds_edl_send_eagch_sir_target_cmd(void);

#ifdef FEATURE_TDSCDMA_WLAN_COEX
extern int16 tdseul_get_epuch_tx_power(void);
#endif

/*===========================================================================
FUNCTION     TDSEULSG_FLUSH_ALL_HARQ

DESCRIPTION  This function flushes all HARQ's in case of MAC_e_reset

DEPENDENCIES To be called by SEQ at activiation time inside FIQIRQ lock

RETURN VALUE  None

SIDE EFFECTS  As said above
===========================================================================*/
extern void tdseulsg_flush_all_harq(void);

#ifdef FEATURE_TDSCDMA_WLAN_COEX
/*===========================================================================
FUNCTION   

DESCRIPTION 

DEPENDENCIES 

RETURN VALUE None

SIDE EFFECTS 
===========================================================================*/
extern void tdseulsg_ts3_event_handler(void);
#else
/*===========================================================================
FUNCTION    EUL_START_EDCH_DATA_PATH_FIQ_HANDLER

DESCRIPTION This function updates the serving grant and HARQ process  at every
            TTI boundary and shall take into account the Absolute Grant message,
            Serving Relative Grant and non-serving Relative Grants that apply 
            to the TTI. UEs configured with an E-DCH transport channel shall
            maintain a Serving Grant and the list of active HARQ processes 
            based on the absolute and relative grant commands decoded on the
            configured E-AGCH and E-RGCH(s).

DEPENDENCIES Called when FIQ is raised by FW

RETURN VALUE None

SIDE EFFECTS serving grant may change and HARQ process may become inactive
===========================================================================*/
extern void tdseulsg_start_edch_data_path_fiq_handler(void);
#endif


#endif /* FEATURE_TDSCDMA_HSUPA */

#endif /* EULSG_H */




