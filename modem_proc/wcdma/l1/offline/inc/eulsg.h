#ifndef EULSG_H
#define EULSG_H

/*============================================================================
              WCDMA L1 HSUPA SERVING GRANT AND HARQ RELATED HEADER FILE

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
serving grant and harq information  .

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/eulsg.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
09/14/15    ssv     Enhancement to AGCH monitoring after ROTA gap
07/16/15    tsk     Save and restore activated HARQ Bitmask after HHO/LTA.
07/06/15    cjz     is_scheduled_tx needs to consider next_tti_dtx
06/09/15    ssv     Instead of crash, added handler if FIQ gets triggered very late
03/16/15    ssv     Removing clock speed from profile buffer due to performance impact
01/16/15    ssv     Including clock speed in profile buffer.
12/08/14    ssv     Handling MAC inactivity counter when CPC is in re-configuration state.
11/18/14    gp      Enabling EUL FIQ time profiling to investigate EUL ISR getting delayed
10/01/14    gv      Code cleanup. Removal of stale and unused code. 
09/09/14    ssv     Protecting scheduled e-dch transmission that happens to be falling in 
                    cycle1/2 while UE is in suspend state
07/28/14    pkg     Code Check-in for 16QAM feature.
06/06/14    ymu     Backup/Restore secondary DPCCH pwr and SG during LTA
05/28/14    yw      Secondary carrier starts with the SG from SG IE from the latest recfg msg. 
                    If SG IE is not present, secondary will start with invalid SG until AG decode.
05/27/14    ar      Redesign of EUL and R99 ISR profiling
05/22/14    raj     Graceful handling of using invalid ETFCI during Re-tx after MAC reconfig
05/20/14    raj     Use "FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW" instead of "FEATURE_WCDMA_MODEM_COEXISTENCE_SW" 
05/20/14    raj     Initial code checkin for cpc+dsda
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/31/14    ar      Enhanced UL time profiling and removed code not applicable to Bolt
03/27/14    ymu     Fix CPC preample tx on Primary carrier when only secondary is Tx EDCH.
02/14/14    gp      EUL FIQ Changes for Bolt : Fixes for 2ms TTI
01/08/14    yw      DCHSUPA: CPC DRX support
12/17/13    ash     Removed A2 Done ISR as well as A2 Done Sem Logic
12/02/13    gp      Fix for LOW GRANT state activated in CM gap and prevent updation of low grant SG during reTx
11/04/13    yw      CM support for DCHSUPA
07/12/13    yw      CPC DTX support for DC-HSUPA
07/11/13    ymu     Support for secondary carrier OOS and recfg.
06/24/13    gp      Code changes for new False detection algo for RGCH detection
                    in a single Radio link scenario.
05/28/13    ash     UL Unused Mutex Cleanup
05/22/13    oh      Adding DC-HSUPA framework support
05/09/13    gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
05/06/13    at      SW Workaround for TXHW-A2 issue and cleanup FEATURE_WCDMA_NIKEL_ERAMA_ACCESS_FIX
12/11/12    vr      HSRACH code cleanup
08/13/12    ash     Configuring HSDPCCH only on a valid AGCH decode
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
02/01/12    raj     Nikel Feature cleanup.
02/01/12    gnk     Nikel Feature cleanup.
02/01/12    gnk     Nikel feature cleanup
01/31/12    raj     Nickel feature cleanup. Mainlined the features FEATURE_WCDMA_HSUPA_INVALID_SUBFN_FIX,
                    FEATURE_WCDMA_HSUPA_IRQ_SPLIT
01/30/12    vs      feature cleanup.
01/26/12    vs      Nikel feature cleanup.Mainline the feature FEATURE_L1_LOGGING_ENABLED.
01/03/12    at      Complete support for Back up and revert of HS SCCH orders
                    during HHO failure and revert back.
11/29/11    gnk     Removed some unused features
10/21/11    sp      Nikel UL CM workaround under FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
10/19/11    oh      Enabling profiling code to capture timestamp for R99 ISR.
07/07/11    sb      Upmerge from offline/mainline
06/24/11    gv      Changes to allow the UE to move out of low grant state when
                    applying the default SG when the UE moves into DTX CYCLE 2
04/28/11    sj      upmerge from offline/main
04/15/11    oh      Removed empty structure to fix mob error. 
03/11/11    dp      Ported CPC DRX changes to offline Architecture
02/07/11    oh      Fixed compilation errors on Genesis.
01/31/11    oh      Fixed compiler warnings.
01/24/11    gv      Incorporate the support to allow the Three step and two 
                    step index change as part of reconfiguration.
01/17/11    oh      Removed PLT featurization from mutex definition.
09/15/10    ksr     Enhanced A2 logics with ERAM Readback, L1-L2 interace contents
                    dumping, Rd back profiling etc
09/02/10    gv      Code Checkin for CPC DTX
08/08/10    sj      Added unified PLT support
07/26/10    gv      Aligning the TTI Change in SW with FW. Basically changing
                    to newer TTI only when FW stops the old config.
07/16/10    ksr     Mainline the FEATURE_WCDMA_HSUPA_ENH
09/21/09    ka      Fixed the issue in which False Acks on HICH were applied
                    to HARQs that had been DTXed.
06/09/09    oh      Declaring e_ul_isr_l2_datapath_mutex under PLT feature.
04/22/09    gnk     Fixed compiler warnings
03/10/09    oh      Added changes for light weight mutex between EUL ISR and L2 data path.
02/06/09    sa      Fix for EDL channel log packet crash and double buffering changes
01/12/09    oh      Protecting cfn_update_handled flag using a local mutex.
12/01/08    asm     Correcting Compiler error in previous check-in
11/24/08    oh      Replacing INTLOCK() in EUL IRQ with REX_ISR_LOCK.
10/03/08    hk      Bringing in 7k Mailine fixes
07/31/08    oh      EUL changes for Taxis.
09/26/07    oh      Fix for SGP as per the Spec 25.321 CR 0325.
06/13/07    gv      Added code to support the square root combining algo.
05/21/07    gv      Added code to support new 4302 log packet implementation.
02/21/07    am      Added code for applying SG at activation time
02/05/07    am      4311 logpkt changes
12/20/06    am      Get "SI sent alone" info from edch module
11/16/06    yiz     Add HHO support for SG resume and HARQ flush
11/08/06    ub      Added support for Nslots field in FW HICH/RGCH res buf
11/02/06    am      Added code for 4309 log pkt HICH changes
09/25/06    gv      Defined new structures for storing the results read by 
                    L1 for HICH and RGCH every frame. Also modified the 
					ENUM definition for the HICH ACK/NACK
09/15/06    am      Added code for logging,IOT fixes,SI optimization and 
                    review comments
08/14/06    yiz     Mainline HSUPA code
03/22/06    am      Moved reading the async results from mDSP to async I/F
03/15/06    yiz     Add EULSG_SG_SCALE_FACTOR & EULSG_MAX_NUM_HARQ
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
#include "rex.h"
//#include "edlcfg.h"
#ifdef FEATURE_WCDMA_DC_HSUPA 
#include "l1macdata.h"
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
extern uint8 debug_tmng_idx; 
extern uint8 debug_timing_snapshot_idx;
extern uint16 eul_pre_fiq_delay_cnt;
extern rex_crit_sect_type e_ul_isr_l2_datapath_mutex; 

typedef struct{
  uint32 debug_timing_issue_array[10];
  uint16 cfn;
}debug_timing_issue_struct_type;

#define BPG_VAL_TX_FRAME_COUNT_CX8_SHIFT 11
/*bpg 35*/
#define UL_BPG_DELAY_THRESHOLD_DCH_ISR 45
/*bpg 40*/
#define UL_BPG_DELAY_THRESHOLD_ENC_TRIGGER 50

/* Timing information to be captured for last 2^4 TTI's */
#define MAX_R99_SZ_TIMING_ARRAY (0x1 << 4)
#define MAX_EUL_SZ_TIMING_ARRAY (0x1 << 4)

/*if R99 encoder is busy*/
#define UL_ENC_IN_PROGRESS  0x2
/*if EUL encoder is busy*/
#define EUL_ENC_IN_PROGRESS 0x100

typedef enum {
  R99_ISR_PROFILE = 0x0,
  EUL_ISR_PROFILE = 0x1, /* 2ms and 10ms EUL */
  EUL_PRE_FIQ = 0x2,     /* 2ms EUL only */
  EUL_POST_FIQ = 0x4,    /* 2ms EUL only */
  ISR_PROFILE_END
} tx_profile_enum_type;

typedef enum
{
  /* R99 isr timing profile points */
  ENC_GET_DATA_START,
  ENC_GET_DATA_END,
  ENC_PROC_TX_DATA_START,
  ENC_PROC_TX_DATA_END,
  ENC_BUILD_FRAME_START,
  ENC_BUILD_FRAME_END,
  ENC_ERAMA_DATA_START,
  ENC_ERAMA_DATA_END,

  /* EUL isr timing profile points */
  EUL_ISR_START,
  EUL_ISR_END,
  EUL_ISR_ASYNC_PARAM_START,
  EUL_ISR_ASYNC_PARAM_END_BFI_START,
  EUL_ISR_BFI_END_A2_START,
  EUL_ISR_A2_END,
  EUL_ISR_A2_READBACK_START,
  EUL_ISR_A2_READBACK_END,
  
  NUM_IDX_TX_STATUS
} tx_status_enum_type;
   
/* ISR Timining profile information struct for R99 */
typedef struct{
  uint8 enc_get_data_start_bpg;
  uint8 enc_proc_tx_data_start_bpg;
  uint8 enc_proc_tx_data_end_bpg;
  uint8 enc_erama_data_start_bpg;
  uint8 enc_erama_data_end_bpg;
  uint8 enc_build_frame_start_bpg;
  uint8 enc_build_frame_end_bpg;
  uint8 enc_get_data_end_bpg;
  uint8 cfn;
} debug_r99_isr_timing_struct_type;

/* ISR Timining profile information struct for EUL */
typedef struct{
  uint16 eul_start_bpg;
  uint16 eul_end_bpg;
  uint16 eul_async_param_start_bpg;
  uint16 eul_async_param_end_bfi_start_bpg;
  uint16 eul_bfi_end_a2_start_bpg;
  uint16 eul_a2_end_bpg;
  uint16 eul_a2_rb_start_bpg;
  uint16 eul_a2_rb_end_bpg;
  tx_profile_enum_type  pre_or_post_fiq;
  e_tti_enum_type tti;
  uint8 cfn;
  uint8 sub_fn;
  uint16 sfn;
} debug_eul_isr_timing_struct_type;

extern void eulsg_timeprofile_alloc_heapmem(void);
extern void eulsg_timeprofile_dealloc_heapmem(void);
extern void eulsg_capture_timing_info(tx_profile_enum_type
            tx_profile_type, tx_status_enum_type tx_profile_pt);

/*This structure maintains data specific to each harq process*/
typedef struct
{

  /*the maximum number of transmissions to perform on this harq*/
  uint32  cur_rsn;
  /*reference_ETPR holds the E-DPDCH to DPCCH power ratio used as reference for
    relative grant commands. This variable is set to the E-DPDCH to DPCCH power
    ratio used for the E-TFC selected for the previous TTI on this HARQ process,
    excluding non-scheduled transmissions, excluding any scaling applied according
    to subclause 5.1.2.6 of 25.214.In case no scheduled transmission took place
    on a HARQ process in the previous TTI, reference_ETPR shall be set to
    Minimum_Grant for this HARQ process
  */
  uint32  ref_ETPR;
  #ifdef FEATURE_WCDMA_REL8
  /* reference_ETPR2 holds the E-DPCCH to DPCCH power ratio and this is copied from 
  ** the reference ETPR before a new value is stored in the refernce ETPR. This is needed 
  ** to update the serving grant due to NS RGCH DOWN as per the new SPEC CR. This
  ** ref_ETPR2 is stored  for every HARQ Process and the reference ETPR used for
  ** SG_LUPR2 computation is max(ref ETPR2 for all active HARQs). DOWN is applied on that
  ** SG_LUPR2*/
  uint32  ref_ETPR2;
  #endif /*FEATURE_WCDMA_REL8*/
  /*Each HARQ process maintains the state variable CURRENT_TX_NB, which indicates
    the number of transmissions that have taken place for the MAC-e PDU currently
    in the buffer. When the HARQ process is established, CURRENT_TX_NB shall be
    initialized to 0.
  */
  uint32  cur_tx_nb;
  /* this buffer maintains the HARQ process state for each harq*/
  uint32 harq_process_state;
  /* boolean buffer to maintain whether there is schedule transmission in the
  prev tti for each harq*/
  boolean  sched_tx_in_prev_tti;
  /* This variable gives the maximum number fo transmissions  indicated in the
     transmission HARQ profile
  */
  uint32 max_num_retrans_allowed;
 /*initially the TRUE and once the data is sent on an harq the flag is set to false*/
  boolean first_harq_tx;
 /*if si alone is sent then it is TRUE else FALSE*/
  boolean eul_si_sent_alone;

  /* Whether harq was Transmitted */
  boolean is_harq_txed;

}eul_sg_harq_param_struct_type;

extern uint32 cur_harq_process_id;

extern uint8 debug_hsrach_call_cnt;
extern uint8 debug_hsrach_call_cnt_val;
extern uint8 eul_fiq_cnt;
extern boolean debug_eagch_result_read;
extern uint32 available_sg_idx[WL1_MAX_UL_CARR_IDX];
extern uint32 sec_init_sg_idx;

/* normalization sacle factopr for available serving grant */
#define EULSG_SG_SCALE_FACTOR  225
/*When the non sg timer is expired or when no DOWN is received for an HARQ RTT,
  then the max available sg value should not be considered */
#define EUL_SG_INVALID_VAL   0xFFFF

/* MAX number of HAR processes */
#define EULSG_MAX_NUM_HARQ  8

/*max no of active SG values possible 0..37*/
#define EUL_MAX_NUM_ACTIVE_SG 38  

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

/* The result structures are now re-organized to have results
** on a per target sub frame/frame basis.*/
#define      EUL_MAX_AGCH_RESULT_BUF_LEN         4
#define      EUL_MAX_NON_SRV_RGCH_RESULT_BUF_LEN 6

/* Define invalid values to DL_CHAN_RES_VAL, this is for invalid AG, NS_RG val.
** The per target sub FN structures will be initialized to this value, the AG and
** the NS_RG_VAL. This will be later used to check whether a valid AG or NS_RG
** exists to be considered for a specific target sub frame number*/
#define INVALID_EDL_CHAN_RES_VAL 0xFF
 
#define E_DL_VALID_CRC_PASS_AGCH 1
#define E_DL_INVALID_CRC_FAIL_AGCH 0
#define E_DL_VALID_CRC_PASS_INVALID_SCOPE_AGCH 2

/* This value indicates the maximum results that can be obtained per channel
** in an active RL.*/
#define       EUL_DL_MAX_RES_PER_RL  2

/* Macro to set the act time for reconfig/config of SG*/
#define EUL_SG_SET_ACT_TIME(act_time, carr_idx) \
    eul_sg_recfg_act_time[carr_idx] = act_time

/*This value will be checked only when SG ord page is valid and this special 
  value is used by edl to indicate activation time now case to SG module*/
#define EUL_SG_ACT_TIME_NONE  0xFFFF

/*This value is default value and no action is taken if it is EUL_SG_ACT_TIME_NONE*/
#define EUL_SG_ACT_TIME_INVALID  0xFFFE

#define EUL_IRQ1_IRQ2_CNT_INIT_VAL_ZERO                       0
#define EUL_IRQ1_IRQ2_CNT_ONE                                            1
#define EUL_IRQ1_IRQ2_CNT_TWO                                           2

#define EUL_IRQ_SPLIT_PRE_FIQ_IND                   -1
#define EUL_IRQ_SPLIT_SECOND_FIQ_IND             1

extern boolean e_ul_dtx_invalid_subfn;

#ifdef FEATURE_WCDMA_CPC_DTX
/* Hs-scch order to disable DTX */
#define EULSG_FW_DTX_ORDER_DISABLE 0

/* Hs-scch order to enable DTX */
#define EULSG_FW_DTX_ORDER_ENABLE  1

/* DTX order MASK */
#define E_UL_CPC_DTX_ORDER_MASK 0x01

/* No-op HS-scch order */
#define EULSG_FW_DTX_NO_ORDER_PEND 0xFFFF

#ifdef FEATURE_CPC_DTX_HS_SCCH_REVERT_HHO_RECOVERY /* RRC Support is needed*/
#define EUL_CPC_DTX_SET_CURR_HS_SCCH_ORDER(order) \
	e_ul_cpc_dtx_scch_ord_bkup_rstr_info.current_hs_scch_order = order
#endif
#endif /*FEATURE_WCDMA_CPC_DTX*/


#ifdef FEATURE_WCDMA_HS_RACH
extern uint16 eul_sg_hs_rach_dpcch_only_tx_cnt;

extern uint8 eul_sg_hs_rach_harq_tx_bmsk;

#define EULSG_SET_HS_RACH_DPCCH_ONLY_TX_CNT(val) \
	 eul_sg_hs_rach_dpcch_only_tx_cnt = val

#define EULSG_GET_HS_RACH_DPCCH_ONLY_TX_CNT() eul_sg_hs_rach_dpcch_only_tx_cnt

#define EULSG_SET_HS_RACH_HARQ_TX_BMSK(harq_id) \
 eul_sg_hs_rach_harq_tx_bmsk |= (uint8)(1 << harq_id)

#define EULSG_RESET_HS_RACH_HARQ_TX_BMSK(harq_id) \
 eul_sg_hs_rach_harq_tx_bmsk &= ~((uint8)(1 << harq_id))

#define EULSG_GET_HS_RACH_HARQ_TX_BMSK() eul_sg_hs_rach_harq_tx_bmsk

#define EULSG_INIT_HS_RACH_HARQ_TX_BMSK() eul_sg_hs_rach_harq_tx_bmsk = 0

#define EUL_SG_HS_RACH_GET_EUL_TX_STATE() eul_sg_hs_rach_eul_tx_state
#define EUL_SG_HS_RACH_SET_EUL_TX_STATE(var) eul_sg_hs_rach_eul_tx_state = var

#endif /*FEATURE_WCDMA_HS_RACH*/

#if defined(FEATURE_WCDMA_CPC_DTX) && defined(FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW)
    /* This is in bpg. This is to make sure SW protect e-dch only when pre-fiq
     * gets finished by 24th bpg. 
     */

#define EUL_CPC_DTX_EDCH_PROTECT_START_TIMELINE  19     

#define EUL_CPC_DTX_EDCH_PROTECT_TIMELINE 24
#endif

/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/
#ifdef FEATURE_WCDMA_HS_RACH
typedef enum
{
  /*This ENUM indicates that the DPCCH Only Tx period is ongoing*/
  EUL_SG_HS_RACH_DPCCH_ONLY_TX,
  /* This ENUM indicates that the UE is Preparing the PDU for EUL Tx*/
  EUL_SG_HS_RACH_EUL_BUILD_PDU,
  /* This ENUM indicates that the UE has done the first EUL Tx*/
  EUL_SG_HS_RACH_FIRST_EUL_TX_DONE,
  /* Default and invalid ENUM*/
  EUL_SG_HS_RACH_EUL_TX_MAX_NUM
}eul_sg_hs_rach_eul_tx_enum_type;

#endif /*FEATURE_WCDMA_HS_RACH*/
/*Information field mapping of the Absolute Grant Scope  as given in the spec
 25.212 table 16C. The values give the applicability of the Absolute Grant to
 the harq process.The agch scope takes two different values,"Per HARQ process"
 or "All HARQ processes", allowing to indicate whether the HARQ process
 activation/de-activation will affect one or all processes.The Absolute Grant 
 Scope is encoded in 1 bit. When the E-DCH is configured with 10ms TTI, only 
 the value "All HARQ processes" is valid.In case Identity Type is "Secondary",
 only the value "All HARQ processes" is valid .
*/
typedef enum
{
  /*The HARQ process is active*/
  EUL_ALL_HARQ_PROCESS,
  /*The HARQ process is Inactive*/
  EUL_PER_HARQ_PROCESS
}eul_sg_agch_scope_enum_type;

/* The HARQ acknowledgement indicator  as given in the spec 25.212 table 18.The
   result after decoding the HICH (combined across cell in active set) would be
   a non serving NACK (2)/ACK(1)/serving NACK(2).
*/
typedef enum
{
  /* HICH ack value returned by FW*/
  EUL_HICH_ACK_VAL = 1,
  /*HICH NACK value returned by FW .NACK (RLS containing the serving E-DCH 
    cell) For serving and non serving HICH, we use 2 for indicating NACK
  */
  EUL_HICH_NACK_VAL
}eul_sg_hich_ack_ind_enum_type;

/*Transmitted on downlink on the E-RGCH from a non-serving E-DCH RL, The non-
  serving relative grant allows neighboring Node Bs to adjust the transmitted
  rate of UEs that are not under their control in order to avoid overload 
  situations.By definition, there could be multiple non-serving relative grant
  commands received at any time. This indication can take two different values,
  "DOWN" or "HOLD". "UP" is not allowed from non serving RGCH.Result values 
  "HOLD"/"DOWN"is mapped to 0/2*/
typedef enum
{
  /*Non serving RG hold value returned by FW*/
  EUL_NON_SRV_RG_HOLD_VAL = 0,
  /*Non serving RG down value returned by FW*/
  EUL_NON_SRV_RG_DOWN_VAL = 2
}eul_sg_non_srv_rg_enum_type;

/*Transmitted on downlink on the E-RGCH from all cells in the serving E-DCH RLS,
  the serving relative grant allows the Node B scheduler to incrementally adjust
  the serving grant of UEs under its control. By definition, there can only be 
  one serving relative grant command received at any one time. This indication 
  can take three different values, "UP", "DOWN" or "HOLD".Result values HOLD"/
  "UP"/"DOWN" is mapped to 0/1/2
*/
typedef enum
{
  /*serving RG hold value returned by FW*/
  EUL_SRV_RG_HOLD_VAL ,
  /*serving RG up value returned by FW*/
  EUL_SRV_RG_UP_VAL ,
  /*serving RG down value returned by FW*/
  EUL_SRV_RG_DOWN_VAL 
}eul_sg_srv_rg_enum_type;

/*Downlink module uses this structure to pass  serving grant config information 
  received as part of cphy setup req to the serving grant module
*/
typedef struct
{
  /* The operation related to EUL*/
  uint8  eul_operation;
  /* TRUE if srv cell changes else FALSE*/
  boolean is_srv_cell_changed;
  /* TRUE if tti changes else FLASE*/
  boolean is_tti_changed;
  /*the tti value*/
  e_tti_enum_type tti;
  /* TRUE if ernti  changes else FALSE*/
  boolean is_ernti_changed;
  /* the E-RNTI(s)  configured for the UE*/
  eul_ernti_cfg_info_enum_type   available_ernti;
  /* the sg info received from N/W*/
  l1_e_grant_config_info_struct_type sg_info;
  /*This variable indicates the offset of FIQ from uplink frame boundary (subframe
    boundary if it is 2ms). The interrupt is issued by firmware to ARM to indicate
    that EUL DL results are ready for pick up.  The offset is in 256 chips unit. 
    Firmware expects a negative number to disable the FIQ.
  */
  uint16 fiq_offset;
  /*flag to know that the srv rgch is cfged*/
  boolean srv_rgch_cfged;
   
}eul_dl_sg_info_struct_type;

/*After the serving grant update procedure SG module will fill in appropriate
  entries in the structure below and pass it to E-DCH module to use the info
  for the upcoming transmission. Then E-DCH module will compute the maximum
  payload limits for scheduled data and total traffic respectively.E-DCH 
  module will pass this information to MAC also.
*/
typedef struct
{
  /* 0..3 for 10ms TTI; 
   * 0..7 for 2ms TTI 
   * L1_UPA_UL_HARQ_INACTIVE indicates HARQ is inactive
  */
  uint32  harq_id;

  /* TRUE if next TTI is new TX; FALSE if re-TX */
  boolean new_tx;

  /* RSN, valid for re-TX only */
  uint32  harq_rsn;

  /* sub frame number for next TTI in case of 2 ms TTI */
  uint32  sub_fr;

  /* one entry value in 25.321-670 Table 9.2.5.2.1.1 multiplied by 225
   * 169344 means (168/15)^2 * 6, which is index 37 
  */
  uint32  serv_grnt;

  /*----------------------------------------------------------------
  Indicates the power offset of the highest priority logical
  channel carrying data. Value in the range 0-6
  ----------------------------------------------------------------*/
  uint8     harq_power_offset;
  
} eul_edch_harq_sg_info_struct_type;

/* this structure gives the information about hich results received from FW. 
   Since UE receive data  in the E-HICH frame associated with SFN i for  the 
   data transmitted in the E DPDCH frame associated with SFN i-3 for 10 ms TTI, 
   we should apply the results on the data to be transmitted in the next TTI
*/
typedef struct
{
  /*the sub frame num at which the hich results will be applied*/
  uint16 sub_fn;
  /* the number of slots decoded by FW */
  uint16 n_slots;
  /* Energy received on the HICH/RGCH */
  int32  energy;
  /* UP/DOWN/HOLD for serving RGCH , DOWN/HOLD for non serving RGCH
  ** ACK/NACK for HICH*/
  uint8 val;
  /*Dirty bit gives whether we have a new result or not for this RL.*/
  uint8 dirty_bit;
}eul_dl_result_struct_type;
 
/*This is a common structure for HICH/RGCH, both serving and non-serving RG results.
** The results that need to be stored for further processing, i.e. for logging etc are actually 
** an array of size 2. This is needed atleast for non-serv RG*/
typedef struct
{
  /* The below array stores upto max two results per HICH/RGCH, per RL*/
  eul_dl_result_struct_type data[EUL_DL_MAX_RES_PER_RL];
  /*The num_results gives the number of valid results in above array*/
  uint8 num_results;
  /* Threshold value for HICH and RGCH*/
  uint16 thresh;

}eul_dl_hich_rgch_result_struct_type;

/*This structure is used for storing the soft combined, energy, threshold and Value
** for serving RGCH and HICH*/
typedef struct
{
  /*Soft combined received energy*/
  int32 eng;
  /* square of combined eng*/
  int64 square_eng;
  /*Soft combined threshold energy*/
  int64 thresh_eng;
  int32 thresh_ul_logging;
 
  /*Soft combined Val for serving RGCH and HICH*/
  uint8 val;
}eul_dl_soft_comb_data_struct_type;

/*serving grant module keeps double buffer data for maintaining serving grant
  information. This is needed because when there is reconfiguration due to  
  grant selector/E-RNTI/TTI/serving cell,the new information needs to be 
  applied after 2 frames from the reconfig cmd.
*/
typedef struct
{
  /*The number of HARQ processes is equal to HARQ round-trip-time(HARQ_RTT). 
    The HARQ_RTT is equal to 4 for 10ms TTI and 8 for 2ms TTI.  Each process 
    is associated  with a number from 0 to HARQ_RTT-1.
  */
  uint32 num_harq_rtt;
  /*this sub frame number tells when to switch to the reconfig data */
  uint32 recfg_sub_fn;
  /*received sg info from downlink*/
  eul_dl_sg_info_struct_type sg_cfg_info;
  
}eul_sg_cfg_db_struct_type;


typedef struct
{
  /* the ag value that should be used at the cfn */
  uint32 ag_val;
  /* the ag scope  that should be used at the cfn */
  eul_sg_agch_scope_enum_type ag_scope;
  /* the ag identity that should be used at the cfn */
  l1_e_pri_sec_grant_selector_enum_type ag_identity;
  /* flag to indicate that the structure is available for storing new results*/
  boolean ag_avail;
}eul_sg_agch_results_struct_type;

/* This is a generic structure that is used to store the energy and
** threshold. This will be used to define structures of this type
** and they may store the per cell values or also the soft combined
** values*/
typedef struct
{
  /* To store the square of the energy in case of square root combining*/
  int64 square_eng;
  /* The energy read on the DL channel - RGCH/HICH*/
  int32 eng;
  /* The threshold energy.*/
  int64 thresh_eng;
  /* INcluding this field for unscaled threshold for logging*/

  int32 thresh_ul_logging;
  uint16 thresh_unscaled;

  /* Store the Nslots*/
  uint8 num_slots;
}e_dl_eng_thresh_result_struct_type;

/* This structure gives the energy metric on the HICH channel along with 
** the threshold for comparision for that RL. This structure has information
** on a per cell basis and also on a per TPC set basis for ease in soft 
** combining*/
typedef struct
{
  /* This gives the per cell HICH energy and threshold contribution to the Soft combined
  ** result*/
  e_dl_eng_thresh_result_struct_type per_cell_info[EUL_MAX_RL];
  /* Gives the Soft combined hich_eng on a per TPC set basis with 
  ** the serving TPC set being the 0th entry. There can be max 4 TPC sets*/
  e_dl_eng_thresh_result_struct_type soft_eng_thresh_info[EUL_MAX_RL];
}eul_hich_results_struct_type;

/*This structure gives the energy metric on the RGCH channel along with the
** threshold for comparison for that RL. This structure also has information
** for the serving RLS to ease soft combining.*/
typedef struct
{
  /* This gives the per cell RGCH eng and threshold for the Soft combined
  ** result. This is filled only for cells those are in serving RLS*/
  e_dl_eng_thresh_result_struct_type per_cell_info[EUL_MAX_RL];
  /* The square of the soft combined energy for square root combining*/
  int64 square_s_rg_eng;
  /* This gives the soft comibined serv RGCH energy.*/
  int32 s_rg_eng;
  /* This gives the soft combined serv_rgch threshold*/
  int32 s_rg_thresh;
  /* This gives the CPich energy*/
  uint32 pilotEnergy;
  /* This gives the sum of the symbol energies*/
  uint32 symEnergy;
  int32 thresh_ul_logging;
  

}eul_serv_rgch_results_struct_type;

/* This structure is for storing the results received on the DL channels on 
** per target sub FN basis. An array of four of type 
** eul_dl_per_target_sub_fn_result_type have been defined to store the
** results for AGCH, HICH and Serving RGCH. */
typedef struct 
{
  /* The target sub FN for the results in these structure*/
  uint16 sub_fn;
  /*AGCH Information to be applied in the above sub_fn*/
  eul_sg_agch_results_struct_type agch_result_info;
  /*HICH Information to be applied in the above sub_fn*/
  eul_hich_results_struct_type hich_result_info;
  /*S-RGCH Information to be applied in the above sub_fn*/
  eul_serv_rgch_results_struct_type serv_rgch_result_info;
  /*The HICH page that is being used to store the information*/
  uint8 hich_page;
  /* The RGCH page that is being used to store information*/
  uint8 rgch_page;
}eul_dl_per_target_sub_fn_result_type;/*Buffer length - FOUR*/

/* This structure is for storing the results received on the DL channels on 
** per target sub FN basis. An array of six of type 
** eul_dl_per_target_sub_fn_ns_rg_result_type have been defined to store the
** results for Non-serving RGCH. */
typedef struct 
{
  /*The target sub FN for the results in these structure*/
  uint16 sub_fn;
  /* This gives the energy and threshold for the non-serv
  ** RG cells that result in the total combined value for this
  ** target sub FN. */
  e_dl_eng_thresh_result_struct_type per_cell_info[EUL_MAX_RL];
  /* The combined OR of the DOWNS value for the target sub fn */
  uint8 ns_rg_val;   /* DOWN/HOLD */
} eul_dl_per_target_sub_fn_ns_rg_result_type;/* Buffer length SIX*/


typedef struct{
  /* This is the hich energy and threshold energy for this RL*/
  e_dl_eng_thresh_result_struct_type hich_info;
  /* This is the rgch energy and threshold energy for this RL*/
  e_dl_eng_thresh_result_struct_type rgch_info;
  /* HICH Value for this cell, ACK/NACK/DTX*/
  uint8 hich_val;
  /* RGCH Val for this cell UP/DOWN/HOLD*/
  uint8 rgch_val;
  /* Boolean indicating whether this RGCH is serv_rls*/
  boolean is_serv_rls;
}eul_logging_per_cell_info_struct_type;


/* The structure below will be used for getting information for the 4309 log packet.*/
typedef struct{
  /* This is the soft combined energy and threshold on a per TPC set basis*/
  e_dl_eng_thresh_result_struct_type hich_soft_eng_thresh_info[EUL_MAX_RL];
  /* This gives the soft comibined serv RGCH energy.*/
  int32 s_rg_eng;
  /* This gives the soft combined serv_rgch threshold*/
  int32 s_rg_thresh;
  int32 s_rg_thresh_ul_logging;
  /* AGCH quality parameters*/
  uint16 eng_word;
  uint16 err_cnt_word;
  /* Per cell information*/
  eul_logging_per_cell_info_struct_type per_cell_info[EUL_MAX_RL];
  /* Num cells..This can be fixed at the config time, but there 
  ** could be cases during reconfig when we start using new config with 
  ** more or fewer cells.*/
  uint8 num_cells;
}eul_logging_dl_results_struct_type;

typedef struct
{
  uint16 crc_pass;
  /* AGCH quality parameters*/
  uint16 eng_word;
  uint16 err_cnt_word;
  uint16 vd_output;
  uint16 cm_slot_bmsk;
}eul_logging_agch_info_struct_type;



/*the result struct for rg and hich which shall be applied in  next E-DCH frame
This is used for 10ms TTI
*/
typedef struct{
  eul_sg_agch_results_struct_type ag_info;
  /* flags to know that there is a new data available for the agch/hich/rgch
     from FW
  */
  boolean  non_srv_rg_avail;

  /*variables to know the soft combined result for serving rg/non serving rg */
  uint8 srv_rg_val;
  uint8 non_srv_rg_val;

  /*variables to know the soft combined HICH result for serving rls and soft
    combined HICH result for all the cells
  */
  uint8 srv_hich_val;
  uint8 comb_hich_val;
}eul_sg_applied_dl_results_struct_type;

typedef enum
{
  EUL_IRQ_1,
  EUL_IRQ_2
}eul_irq_cycle_enum_type;

extern eul_irq_cycle_enum_type eul_irq_cycle;
#ifdef FEATURE_WCDMA_BOLT_2_0
#else
extern boolean eul_skip_first_irq;
#endif
extern uint8 eul_irq1_irq2_count;

typedef struct
{
  boolean eul_skip_edch_tx_for_next_tti;
  uint32 previous_sg_idx;
}e_ul_sg_e_dl_res_return_params_struct_type;

#ifdef FEATURE_WCDMA_CPC_DTX

/* Enumeration describing EUL TX types */
typedef enum 
{
  /* None */
  EUL_CPC_DTX_NONE,

  /* Allow TX if MAC has data */
  EUL_CPC_DTX_ALLOW_TX_IF_MAC_HAS_DATA,

  /* ReTx, Allow TX even if MAC doesnt have data*/
  EUL_CPC_DTX_ALLOW_TX_FOR_RETX,

  /* There is already a pending Tx */
  EUL_CPC_DTX_PENDING_TX,

  /* Do not allow TX in next TTI */
  EUL_CPC_DTX_DONOT_ALLOW_TX

}eul_cpc_dtx_tx_status_enum_type;  


typedef struct
{

  /* TX decision for CPC DTX */
  eul_cpc_dtx_tx_status_enum_type tx_status;

  /* Sub frame number at which TX should happen */
  uint16                          tx_subfn;

  /* Whether next TTI has TX */
  boolean                         dtx_next_tti;

#if defined(FEATURE_WCDMA_CPC_DTX) && defined(FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW)
  /* Whether e-dch to be protected or not */
  boolean                         edch_tx_protect[WL1_MAX_UL_CARR_IDX];
#endif
  

} eul_cpc_dtx_tx_decision_type;

typedef struct{
  /* A boolean Array that will store the future transmission sub FN
  ** indexed using HARQ ID*/
  boolean ota_tx_present[EUL_MAX_HARQ_RTT];
  
  /*Inactivity timer for cycle_2*/
  uint16 inact_timer_for_cycle_2;

  /*MAC Inactivity timer*/
  uint16 mac_inact_threshold;

}eul_cpc_dtx_per_tti_upd_params_struct_type;

typedef enum
{
  /* Decrement the MAC inactivity timer if configured and non-zero
  ** Decrement the Cycle 2 inactivity timer if it is non-zero*/
  E_UL_CPC_DTX_DECREMENT_ALL_TIMERS,
  /* Set the value of the timers back to those configured by the OTA message.
  ** basically done when there has been an EDCH Tx completion*/
  E_UL_CPC_DTX_RESET_TIMERS,
 
  /* Decrement the MAC times during reconfig */
  E_UL_CPC_DTX_DECREMENT_MAC_TIMERS,

  /* RESET MAC inactivity timer during re-configuration */
  E_UL_CPC_DTX_RESET_MAC_TIMERS,
  
  E_UL_PER_TTI_NOOP
}e_ul_cpc_dtx_timers_state_update_enum_type;

typedef enum{

  /*Update default SG when we move to cycle 2*/
  EUL_SG_DEFAULT_SG_READY_FOR_CYCLE_2_TRANS,

  EUL_SG_APPLY_DEFAULT_SG,

  EUL_SG_DO_NOT_APPLY_DEFAULT_SG,

  EUL_SG_DEFAULT_SG_NOOP

}eul_sg_cpc_dtx_default_sg_apply_enum_type;


extern eul_cpc_dtx_per_tti_upd_params_struct_type eul_cpc_dtx_per_tti_upd_params[WL1_MAX_UL_CARR_IDX];
extern uint16 e_ul_cpc_dtx_cm_num_slots_burst[WL1_MAX_UL_CARR_IDX][2],e_ul_cpc_dtx_cm_start_slot_burst[WL1_MAX_UL_CARR_IDX][2];

/* Just used between the PRE FIQ and the SECOND FIQ*/
typedef struct{
  /* This parameter is set by the PRE FIQ in preparation to calling the
  ** HARQ PO function in the second FIQ. This parameter indicates
  ** whether HARQ PO function needs to proceed further or not. This
  ** is set to TRUE if Data can be OTAed as per MAC DTX cycle and other
  ** params -- EVERY TIME AT THE START OF PRE FIQ, THIS FLAG IS INITIALIZED 
  ** TO FALSE*/
  boolean is_etfci_sel_pdu_build_allowed;
  /* The param e_ul_cpc_cfg_active is used to know if the DTX has been
  ** activated just now. If that is the case then in the process_dtx_func
  ** we should not update the counters etc because the DTX is not yet active*/
  boolean e_ul_cpc_dtx_cfg_active;

}e_ul_cpc_dtx_eul_isr_params_struct_type;

#endif /* FEATURE_WCDMA_CPC_DTX */

typedef struct{
  uint16 eul_cur_fn;
  uint32 sg_lupr;
  boolean eul_sg_harq_first_tx_acked;
  boolean eul_sg_harq_max_re_tx_nack;
}eul_logging_data_struct_type;


#ifdef FEATURE_WCDMA_CPC_DRX
typedef struct
{
  /* TRUE if the next TTI Tx has scheduled data and will be Tx'ed */
  boolean is_scheduled_tx[WL1_MAX_UL_CARR_IDX];

  /* TRUE if atleast one MAC-D flow is configured for scheduled tx and(TEBS>0)*/
  boolean is_tebs_non_zero[WL1_MAX_UL_CARR_IDX];

}eul_edch_edldrx_params_struct_type;

extern eul_edch_edldrx_params_struct_type eul_edch_edldrx_params;

#define IS_NEXT_TTI_SCHEDULED_TX(carr_idx) (eul_edch_edldrx_params.is_scheduled_tx[carr_idx])
#define IS_TEBS_NON_ZERO(carr_idx)(eul_edch_edldrx_params.is_tebs_non_zero[carr_idx])

#endif /*FEATURE_WCDMA_CPC_DRX*/

extern uint8 e_ul_cpc_dtx_debug_hs_scch_ord;
extern uint16 e_ul_cpc_dtx_debug_hs_scch_ord_sub_fn;

/* ==========================================================================
** Public Data Declarations
** ========================================================================*/
/* Variable to indicate whether downlink DCH is in sync or not*/
extern boolean e_ul_cm_func_called;

/* AGCH, HICH and Serving RGCH per target sub FN array of structures*/
extern eul_dl_per_target_sub_fn_result_type agch_serv_rgch_hich_results[WL1_MAX_UL_CARR_IDX][EUL_MAX_AGCH_RESULT_BUF_LEN];
/* Non-serving per target sub FN array of structures*/
extern eul_dl_per_target_sub_fn_ns_rg_result_type ns_rgch_results[WL1_MAX_UL_CARR_IDX][EUL_MAX_NON_SRV_RGCH_RESULT_BUF_LEN];

/* The below structure is to copy information to a global structure which
** can later be used to fill information in EUL log packets*/
extern eul_logging_dl_results_struct_type eul_logging_dl_results[WL1_MAX_UL_CARR_IDX];
extern eul_logging_agch_info_struct_type eul_logging_agch_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_AGCH_RESULT_BUF_LEN];

/*The subFN value and should be accessed only inside the EUL FIQ*/
extern uint16 eul_fiq_global_cur_subfn;
#ifdef FEATURE_WCDMA_16_QAM
#define EUL_MAX_NUM_E_TABS 2
#else
#define EUL_MAX_NUM_E_TABS 1
#endif /* FEATURE_WCDMA_16_QAM */

extern const uint32 sg_table[EUL_MAX_NUM_E_TABS][40];

/* This global value holds the activation time for the SG information during 
 reconfig/start*/
extern uint16 eul_sg_recfg_act_time[WL1_MAX_UL_CARR_IDX];
extern uint8 debug_eul_set_hich_val;
extern uint8 debug_eul_set_rgch_val;
extern uint8 debug_eul_set_beta_e_d;
extern uint8 debug_eul_set_beta_e_c ;
extern uint8 debug_eul_set_agch_val;
extern boolean debug_taxis_eul_betas;
#ifdef FEATURE_WCDMA_DC_HSUPA 
extern l1_eul_pwr_offset_sec_ul_non_sg_struct_type eul_pwr_offset_sec_ul_non_sg;
#endif 

#if defined(FEATURE_QTA) && defined(FEATURE_WCDMA_CPC_DRX)
extern boolean e_ul_cpc_drx_was_in_rota_gap;
#endif

/*The CFN value and should be accessed only inside the EUL FIQ*/
extern uint8 eul_fiq_global_cur_fn;
extern boolean eul_allow_etfci_sel_pdu_build_in_low_grant;

#ifdef FEATURE_WCDMA_HS_RACH
extern eul_sg_hs_rach_eul_tx_enum_type eul_sg_hs_rach_eul_tx_state;
extern uint16 hsrach_agch_decode_result;
#endif


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION    eul_hs_rach_preprocess_fiq

DESCRIPTION 

DEPENDENCIES 

RETURN VALUE 

SIDE EFFECTS 
===========================================================================*/
boolean eul_hs_rach_preprocess_fiq(void);
#endif /*FEATURE_WCDMA_HS_RACH*/

/*===========================================================================
FUNCTION     EUL_SG_HARQ_MODULE_INIT

DESCRIPTION  This function initalizes the gobal data maintained for serving
             grant and HARQ module.This function is called whenever EUL 
             is started.DL module calls this function to reset the data  

DEPENDENCIES Called by EUL DL channel setup module

RETURN VALUE None

SIDE EFFECTS SG information shall be reinitialized to default values
  
===========================================================================*/
extern void  eul_sg_harq_module_init(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SG_HARQ_MODULE_PROCESS_CFG_DATA

DESCRIPTION  This function gets the serving grant module config data from 
             downlink module and initialize the serving grant config db.
             This function is called when there is serving cell change or
             EUL start or TTI change or E-RNTI change 

DEPENDENCIES Called by EUL DL channel setup module as post callback function
             of mDSP config cmds for start/recfg of EUL

RETURN VALUE None

SIDE EFFECTS serving grant will be initialized to signalled value and 
             HARQ process are active
===========================================================================*/
extern void  eul_sg_harq_module_process_cfg_data
                  (eul_dl_sg_info_struct_type  *e_dl_sg_info_ptr, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SG_SET_HARQ_PARAMS

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
extern void eul_sg_set_harq_params
       (uint32 ref_etpr, boolean scheduled_data_txed,
        uint32 max_num_retrans,boolean etfci0_newtx,
        boolean is_harq_txed,
        wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION         E_DL_POPULATE_DL_CHAN_RESULTS_FOR_TX_NEXT_TTI

DESCRIPTION      The index that is used to read the info from the buffer is already
                 computed and is stored in a global variable. Using this index for the 
                 AGCH/HICH/S-RGCH and NS RGCH per target sub FN structures,
                 the EUL SG information structure is populated with needed info for the
                 SG functionality to proceed.

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     A EUL_SG structure is updated.

===========================================================================*/
extern void e_dl_populate_dl_chan_results_for_next_tti( eul_sg_applied_dl_results_struct_type* applied_dl_results, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SG_GET_CUR_SG_IDX

DESCRIPTION  This function gets the current available sg index in the SG table

DEPENDENCIES .None

RETURN VALUE  serving grant index

SIDE EFFECTS  None
===========================================================================*/

extern uint32 eul_sg_get_cur_sg_idx(void);

/*===========================================================================
FUNCTION     EUL_SG_GET_CUR_SG_CFG_INFO

DESCRIPTION  This function gets the current active SG cfg info

DEPENDENCIES .None

RETURN VALUE  current active SG cfg info

SIDE EFFECTS  None
===========================================================================*/
extern l1_e_grant_config_info_struct_type eul_sg_get_cur_sg_cfg_info(void);

/*===========================================================================
FUNCTION     EUL_SG_GET_SG_AVAILABLE_STATUS

DESCRIPTION  This function gets the sg availablity status

DEPENDENCIES .None

RETURN VALUE  TRUE if the SG is available else FALSE

SIDE EFFECTS  None
===========================================================================*/

extern boolean eul_sg_get_sg_available_status(void);

/*===========================================================================
FUNCTION     EUL_SG_SAVE_SG

DESCRIPTION  This function saves the current Serving Grant for HHO, reverting,
             or resume within FIQIORLOCK()

DEPENDENCIES To be called by e_ul_cleanup() only inside FIQIRQ lock

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void eul_sg_save_sg(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SAVE_STATE_INFO

DESCRIPTION  This function saves the current EUL State for HHO, reverting,
             or resume within FIQIORLOCK()

DEPENDENCIES To be called by e_ul_cleanup() only inside FIQIRQ lock

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void eul_save_state_info(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_RESTORE_STATE_INFO

DESCRIPTION  This function restores the EUL State after HHO/LTA, reverting,
             or resume.

DEPENDENCIES

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
extern void eul_restore_state_info(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SG_FLUSH_ALL_HARQ

DESCRIPTION  This function flushes all HARQ's in case of MAC_e_reset

DEPENDENCIES To be called by SEQ at activiation time inside FIQIRQ lock

RETURN VALUE  None

SIDE EFFECTS  As said above
===========================================================================*/
extern void eul_sg_flush_all_harq(wl1_ul_carr_id_enum_type carr_idx);


/*===========================================================================
FUNCTION     EUL_SG_GET_CUR_ERNTI_INFO

DESCRIPTION  This function gets the current E-RNTIs configured

DEPENDENCIES None

RETURN VALUE Current E-RNTI info

SIDE EFFECTS None
===========================================================================*/
eul_ernti_cfg_info_enum_type eul_sg_get_cur_ernti_info(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SG_GET_CUR_THREE_IDX_STEP_THRESH_INFO

DESCRIPTION  This function gets the current configured three step threshold index

DEPENDENCIES .None

RETURN VALUE  Current Configured three index step threshold

SIDE EFFECTS  None
===========================================================================*/
uint8  eul_sg_get_cur_three_idx_step_thresh_info(void);

/*===========================================================================
FUNCTION     EUL_SG_GET_CUR_TWO_IDX_STEP_THRESH_INFO

DESCRIPTION  This function gets the current configured two step threshold index

DEPENDENCIES .None

RETURN VALUE  Current Configured two index step threshold

SIDE EFFECTS  None
===========================================================================*/
uint8  eul_sg_get_cur_two_idx_step_thresh_info(void);

/*===========================================================================
FUNCTION     EUL_SG_IS_SERVING_RG_CFGED

DESCRIPTION  This function checks if serving RG is cfged or not

DEPENDENCIES None

RETURN VALUE TRUE if serving RG cfged else FALSE

SIDE EFFECTS None
===========================================================================*/
boolean eul_sg_is_serving_rg_cfged(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION     EUL_SG_TTI_RECFG_DONE

DESCRIPTION  This function resets the tti in progress info flag

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS In the upcoming EUL FIQ ,data can be processed after TTI recfg
===========================================================================*/
void eul_sg_tti_recfg_done(void);

/*===========================================================================
FUNCTION     EUL_SG_IS_TTI_RECFG_IN_PROGRESS

DESCRIPTION  This function returns a boolean flag whethe TTI reconfig
                    is in progress or not.

DEPENDENCIES None

RETURN VALUE Boolean

SIDE EFFECTS  
===========================================================================*/

extern boolean eul_sg_is_tti_recfg_in_progress(void);

/*===========================================================================
FUNCTION     EUL_SG_GET_CURR_HARQ_ID

DESCRIPTION  This function returns cur_harq_process_id

DEPENDENCIES None

RETURN VALUE Boolean

SIDE EFFECTS  
===========================================================================*/

uint32 eul_sg_get_curr_harq_id(void);

/*===========================================================================
FUNCTION     EUL_UPDATE_DL_RES_SG_CALL_MAC_IF_FUNC

DESCRIPTION  This function updates the DL results based on the results obtained based on Absolute grant
             serving grant and the non serving relative grants. When DTX feature is defined then a MAC IF function
             is called which gets to know the buffer status from the MAC and RLC. If the DTX feature is not defined,
             nothing is done. 
             If the IRQ SPLIT feature is not defined, then HARQ PO function will be called in this function. In this 
             case the MAC IF functio is the HARQ PO function call.
             If IRQ SPLIT is defined, then the harq po function call is done else where, i.e. in the second FIQ.

             VERY IMPORTANT: THIS FUNCTION IS DEFINED AND CALLED IRRESPECTIVE OF THE IRQ SPLIT FEATURE.

DEPENDENCIES  NONE

RETURN VALUE  None

SIDE EFFECTS  NONE
===========================================================================*/
extern void eul_update_dl_res_sg_call_mac_if_func(eul_edch_harq_sg_info_struct_type *edch_harq_sg_info,
                                                                       e_ul_sg_e_dl_res_return_params_struct_type *e_ul_sg_e_dl_res_sg_call_mac_if_func,
                                                                       #ifdef FEATURE_WCDMA_CPC_DTX
                                                                        e_ul_cpc_dtx_eul_isr_params_struct_type *e_ul_cpc_dtx_data_pre_fiq_to_fiq,
                                                                       #endif /* FEATURE_WCDMA_CPC_DTX */
                                                                       eul_logging_data_struct_type *eul_logging_data,
                                                                       wl1_ul_carr_id_enum_type carr_idx);

#ifdef FEATURE_WCDMA_CPC_DTX
/*===========================================================================
FUNCTION  E_UL_CPC_DTX_INIT_CM_GLOBALS

DESCRIPTION  Initializes DTX CM globals

DEPENDENCIES NONE

RETURN VALUE 

SIDE EFFECTS Setting the mdsp var cpccmFillNextFrame = 0
===========================================================================*/

void e_ul_cpc_dtx_init_cm_globals(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION    EUL_DTX_SEND_FW_LOG_PKT_TO_DIAG()

DESCRIPTION This function sends DTX FW log packet to diag

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void eul_dtx_send_fw_log_pkt_to_diag(void);

/*===========================================================================
FUNCTION    EULSG_CPC_DTX_COLLECT_FW_LOG_PKT_SAMPLE()

DESCRIPTION This function updates the sample info for the DTX FW log packet

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
void eulsg_cpc_dtx_collect_FW_log_pkt_sample(void);

/*===========================================================================
FUNCTION    E_UL_CPC_DTX_INIT

DESCRIPTION   Initialize UL DTX variables.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Clears global variables.
===========================================================================*/
extern void e_ul_cpc_dtx_init(void);

/*===========================================================================
FUNCTION     E_UL_CPC_DTX_ETFCI_SELECT_FOR_SUBFN

DESCRIPTION  This function finds if ETFCI selection is allowed for the given   
             subfn as per the MAC DTX Cycle. 

DEPENDENCIES  NONE

RETURN VALUE  BOOLEAN

SIDE EFFECTS  NONE
===========================================================================*/

boolean e_ul_cpc_dtx_etfci_select_for_subfn(uint32 subfr,  wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  EULSG_CHECK_AND_RET_TX_SUBFN_FOR_CPC_DTX

DESCRIPTION Returns a possible CFN where TX can begin within 10 subframes from
            current SFN.

DEPENDENCIES Should be called only in 2MS DTX_CYCLE_1 or DTX_CYCLE_2

RETURN VALUE
  TRUE   - tx_subfn carries a valid SUBFN in which TX is possible
  FALSE  - tx_subfn is not valid

SIDE EFFECTS
===========================================================================*/
boolean eulsg_check_and_ret_tx_subfn_for_cpc_dtx (uint16 *tx_subfn, wl1_ul_carr_id_enum_type carr_idx );


/*===========================================================================
FUNCTION  EULSG_PROCESS_CPC_DTX_PRE_MAC_BUFFER_QUERY

DESCRIPTION This funciton is called before querying mac for the buffer status. This function will fill in the next possible 
                      OTA subfn value if any and fill in the next tti is either a DTX or can be used for transmission.

DEPENDENCIES Should be called only in DTX_CYCLE_1 or DTX_CYCLE_2

RETURN VALUE  void

SIDE EFFECTS Fills in the structures that are passed as arguments.
===========================================================================*/
void eulsg_process_cpc_dtx_pre_mac_buffer_query (
     /* HARQ info */
   eul_edch_harq_sg_info_struct_type  *edch_harq_sg_info_ptr,

     /* EUL Edch transmit info */
   eul_cpc_dtx_tx_decision_type      *edch_cpc_dtx_tx_info,

    /*carrier index*/
   wl1_ul_carr_id_enum_type           carr_idx 
);


/*===========================================================================
FUNCTION  EULSG_PROCESS_CPC_DTX_POST_MAC_BUFFER_QUERY

DESCRIPTION

DEPENDENCIES Should be called only in DTX_CYCLE_1 or DTX_CYCLE_2

RETURN VALUE
  void

SIDE EFFECTS
===========================================================================*/
boolean eulsg_process_cpc_dtx_post_mac_buffer_query (
     /* HARQ related info */
   eul_edch_harq_sg_info_struct_type  *edch_harq_sg_info_ptr,

     /* Carries TX info during CPC DTX */
   eul_cpc_dtx_tx_decision_type       *edch_cpc_dtx_tx_info,

     /* Gives buffer status */
   boolean                             mac_buffer_status,

    /*carrier index*/
   wl1_ul_carr_id_enum_type           carr_idx 
);
/*===========================================================================
FUNCTION  E_UL_SG_PROCESS_SCCH_ORDER

DESCRIPTION

DEPENDENCIES  This function does the handling of the HS SCCH order processing in SW.
                       It handles the DTX ENABLE and DTX Disable commands on HS SCCH orders
                       and also performs the needed state transitions.

RETURN VALUE
  void

SIDE EFFECTS  Updates global variables including the EUL CPC DTX state
===========================================================================*/
void e_ul_sg_process_scch_order(void);

/*===========================================================================
FUNCTION  EULSG_CPC_DTX_UPD_CYCLE2_MAC_INACT_TIMERS

DESCRIPTION  This function updates the Cycle 2 timer and MAC inactivity timer
                       based on the ENUM passed. The enum can order the function to
                       reset all the timers, when there has been an EUL Tx completed.
                       The ENUM can indicate to decrement all cycle 2 inact and mac 
                       inact timers. It can also ask to decrement only the MAC inact timer
                       in the case when the CPC DTX cycle is CYCLE 2.

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS  The cycle 2 inactivity and MAC inactivity timers are updated.
===========================================================================*/

void eulsg_cpc_dtx_upd_cycle2_mac_inact_timers(
                                             e_ul_cpc_dtx_timers_state_update_enum_type update_decr_reset_timers,
                                             wl1_ul_carr_id_enum_type carr_idx
);
/*===========================================================================
FUNCTION  EULSG_PROCESS_CPC_DTX_STATE

DESCRIPTION  This function is one of the core functions of CPC DTX
                     which completes the critical functions like updating the
                     cycle 2 and mac inactivity timers and also taking care
                      of moving the DTX state from cycle 1 to 2 and vice versa.
                      This function also takes care of the scenario where the
                      cycle timers are updated even when the DTX is disabled
                      with a HS SCCH orders
 
 DEPENDENCIES
 
 RETURN VALUE void

 SIDE EFFECTS  The global variables reflecting the CPC DTX state,the
                        mac inactivity and cycle 2 timers.
===========================================================================*/

void eulsg_process_cpc_dtx_state(boolean tx_next_tti, 
                                           e_ul_cpc_dtx_eul_isr_params_struct_type *e_ul_cpc_dtx_data_pre_fiq_to_fiq,
                                           wl1_ul_carr_id_enum_type carr_idx);
/*===========================================================================
FUNCTION     E_UL_SG_SET_LOW_GRANT_SG_VAL

DESCRIPTION  This function sets the global variable to hold the SG value at which
                      the UE declared low grant. So if the default SG is applied and it is greater
                      than that value, the UE can come out of the low grant state

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS  
===========================================================================*/
extern void e_ul_sg_set_low_grant_sg_val(wl1_ul_carr_id_enum_type carr_idx);


#endif /*FEATURE_WCDMA_CPC_DTX*/

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION     EUL_SG_HS_RACH_PROC

DESCRIPTION  This function is called in the EUL ISR handler when the
             HS RACH state is IN PROGRESS. This is not called otherwise.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS  This function would set some globals like euledpch_state
             so that the build PDU might not happen. This is as per the HS RACH
             spec to gate EUL Tx and allow only UL DPCCH Tx for a specified duration.
===========================================================================*/
void eul_sg_hs_rach_proc(void);
/*===========================================================================
FUNCTION     EUL_SG_HS_RACH_SERV_GRANT_UPDATE_PROC

DESCRIPTION  This function is called in the EUL ISR handler when the
             HS RACH state is IN PROGRESS and only when the HS RACH
             EUL TX type is DTCH and HS RACH substate is EDCH Tx. This function
             should not be called for HS RACH for CCCH data and also during the
             collision resolution phase

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS  This function would update the global available_sg_idx, the
             variable that holds the serving grant for the UE.
===========================================================================*/
uint16 eul_sg_hs_rach_serv_grant_update_proc(uint16 eul_cur_fn);

#endif /*FEATURE_WCDMA_HS_RACH*/

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION  E_DL_SEC_TX_ENABLE

DESCRIPTION
  This function is called by EUL ISR to enable Sec E-DL after Sec UL has gone
  active

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  Sec E-DL starts
===========================================================================*/

extern void e_dl_sec_tx_enable(void);

/*===========================================================================
FUNCTION     E_UL_SEC_CPC_DTX_FLUSH_LOG_PKT

DESCRIPTION  This function is called to deactivate CPC DTX on secondary carrier
                        upon SCCH order deactivation of secondary carrier.  It should only be called when primary
                        is already in CPC DTX state

DEPENDENCIES  When secondary carrier is in CPC DTX active state

RETURN VALUE None

SIDE EFFECTS  None
===========================================================================*/
extern void e_ul_sec_cpc_dtx_flush_log_pkt(void);

/*===========================================================================
FUNCTION     E_UL_SEC_CPC_DTX_ORDER_CFG_STOP

DESCRIPTION  This function is called to deactivate CPC DTX on secondary carrier
                        upon SCCH order deactivation of secondary carrier.  It should only be called when primary
                        is already in CPC DTX state

DEPENDENCIES  When secondary carrier is in CPC DTX active state

RETURN VALUE None

SIDE EFFECTS  None
===========================================================================*/

extern void e_ul_sec_cpc_dtx_order_cfg_stop(void);

/*===========================================================================
FUNCTION     E_UL_SEC_CPC_DTX_ORDER_CFG_START

DESCRIPTION  This function is called to configure CPC DTX on secondary carrier
                        upon SCCH order activation of secondary carrier.  It should only be called when primary
                        is already in CPC DTX state

DEPENDENCIES  When Primary carrier is in CPC DTX active state

RETURN VALUE None

SIDE EFFECTS  None
===========================================================================*/

extern void e_ul_sec_cpc_dtx_order_cfg_start(uint32 dtx_cfg_db_idx);

/*===========================================================================
FUNCTION     e_ul_find_max_etfci_and_sgp

DESCRIPTION  This function is called to polulate max etfci and SG payload bits based on TTI and ETFCI table

DEPENDENCIES 

RETURN VALUE None

SIDE EFFECTS  None
===========================================================================*/

extern void e_ul_find_max_etfci_and_sgp(uint8 *max_etfci_pwr, uint16 *max_sgp);

#endif

#endif /* EULSG_H */