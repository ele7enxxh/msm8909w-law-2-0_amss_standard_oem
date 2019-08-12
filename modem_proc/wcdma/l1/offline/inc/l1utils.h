#ifndef L1UTILS_H
#define L1UTILS_H
/*===========================================================================

                    L 1   U T I L I T I E S

DESCRIPTION
  This file contains global declarations and external references
  for the L1 utilities.

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2014 by Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1utils.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
11/15/16   vn      API to get slots to start CM
03/14/16   nd/ap   QSH changes to support SSIM on SUB2 when DUAL WCDMA is not defined
02/27/15   sr/ap   Changes to not post local command for on QSH Metric Config
02/24/15   ap      Initial check-in for WL1 QSH framework 
09/16/15   sr      Initial checkin for FR 26957 - TxAGC logging
09/11/15   stk     FR 30139: SVS+ and NOM+ frequency corner support in MCVS
11/05/14   kr      send w2g request with DRX request
11/13/14   kr      Corrt JO featurization
11/10/14   sl      Code changes for new f/w log packet 0x41C4 V1
11/07/14   sks     Redesign of MCVS algo.
10/10/14   pr      Added new API for Jolokia to find out the max clock speeds supported.
09/16/14   stk     JO SW-FW interface change to support Q6 clock speeds
09/15/14   ac      Enable/disable DSR state machine during init/cleanup
08/13/14   mit     FTM_RF/FTM_cal optimizations in WL1
08/08/14   ac      DSR F3 reduction
08/05/14   gsk     Changed Idle mode F3s to custom level
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
07/18/14   sad     RF api changes and inter freq tune changes
07/15/14   ac      NV reorganization
07/01/14   hk      Header file inclusion cleanup
05/20/14   gm      Changes to use TCXOMGR APIs irrespective of multi-SIM features
05/14/14   qj      Add change to support multi-carr for 0x41b1
05/09/14   gsk     Dont register gsm cleanup CB if gsm clk isnt enabled by search module
04/18/14   as      DSDS Code Reorganization.
02/12/14   sad     DB-3C API check in
03/27/14   gm      use of client, subscription and system RGS based on l1m_state
03/27/14   rc      Updated DSR state machine to handle concurrenrency issues
03/12/14   sks     MCVS check-in on BOLT
02/18/14   sks     Feature cleanup for offline clock speeds for BOLT.
12/16/13   as      Porting G2W TA from Triton to Dime.
12/06/13   ar      Added a function to count the set bit positions in a bitmask 
12/02/13   sks/kr  check for w2g_client_status while setting warm up time after sleep_drv_cancel_wakeup
08/21/13   sks     Compilation fixes for Bolt
07/29/13   dp      Allow Atuner period to be changed by NV
07/26/13   da      Added a generic function which handles log packet data
07/24/13   jd      Changes for Dual band Dual carrier 
06/06/13   vs      Extern all the globals for the wcdma periodic statictics log packet.
05/09/13   vs      added support for wcdma periodic statictics log pkt.
05/09/13   gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
02/27/13   rc      Added support for custom priority level F3s.
02/25/13   jd      Changes to write the RGS value to total frequency error when RGS is invalid during X2W.
01/28/13   dp      Added support for Antenna Tuner
01/24/13   zr      Initialize allocated heap memory to 0
01/21/13   jd      Frequency binning changes for dime.
01/21/13   pv      Added WL1_MSG7 and WL1_MSG9
01/15/12   pr      Changed wl1_mem_alloc to INLINE in order to catch mem heap exhaustion.
11/27/12   pv      API for FWS APP suspend status.
11/16/12   cc      IRAT feature for Dime
11/09/12   jd      Changes to make sure Frequency Binning algorithm is not triggered on Dime 
08/15/12   zr      Revising circular buffer API to prevent task starvation
08/15/12   zr      Increasing size of cumulative circular buffer counters
08/14/12   zr      Adding log packets to reduce F3 logging from searcher
08/01/12   pv      Created WL1_MSG8.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr      Added feature definitions for Dime
07/05/12   pv      Created WL1_MSG6.
05/29/12   saik    Use int32 type to store result of freq err computation.
05/23/12   dp      Parallelized the gl1 init process when wl1 boots up
03/07/12   zr      Added function to convert UARFCN to carrier frequency
02/13/12   pv      Support for sending NPA requests from WL1.
02/12/12   hk      Nikel feature cleanup
01/26/12   vs      Nikel feature cleanup.
01/20/12   bs      free floating changes.
01/19/12   zr      Removed FEATURE_POWER_COLLAPSE_WCDMA, FEATURE_WCDMA_TAXIS_SW, 
                   and FEATURE_WCDMA_OFFLINE_CLK_SPEED_CHANGE
12/27/11   mc      Added support for DSR and RxD/CM transitions
12/21/11   xl      Merge from MPSS.NI.1.1
12/09/11   pv      Fixed compilation warning.
12/09/11   pv      Fixed compilation error.
12/08/11   pv      Added support for sending offline clock speed to FW
                   if changed.
11/30/11   rc      Added support for FEATURE_WCDMA_DSR_SUPPORT
11/11/11   pv      Externed a variable for RTR power collapse.
10/24/11   pv      Removal of deprecated clkregim APIs.
10/21/11   sp      Nikel UL CM workaround under FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
09/12/11   pv      Removed FB_ANT_CLK check.
08/23/11   pv      Fixed compilation warning.
08/18/11   pv      Added new API to check for GSM frame boundary.
07/28/11   mmr     Externed a xo debug function.
05/27/11   mmr     Added XO fixes.
04/28/11   mmr     Added support for Feature XO.
02/09/11   pv      New enums and APIs to support offline clk speed switching.
11/19/10   sp      Added BIT/BYTE/WORD MACRO Operations
09/14/10   pv      Created an API that checks if FB_ANT0_CLK is under FW control in W.
05/21/10   yh      Featurized sample server config for TAXIS only
04/09/10   ms      Added support for dynamic memory allocation from Modem Heap.
03/30/10   rvs     IRAT sample-server config changes for Chetak and
                   general clean-up.
10/06/09   yh      Add function to toggle HW register when entering/exiting RxD.
09/17/09   yh      Use MCS API to configure SS to avoid GPS concurrency issues.
09/14/09   sv      Removed WCDMA_INTLOCK's.
08/20/09   ms      Added prototype for l1_copy_to_word function
07/17/09   yh      Add support for IRAT phase 2
04/17/09   ks      Added prototype for num_set_bitsXX functions
04/10/09   scm     Add WL1_TRK_LO_PDM_MIDPOINT definition.
03/06/09   yh      For IRAT phase 1, store the current mode of sample server 1.
11/05/08   scm     Utility functions for TCXO freeze and freq hypothesis usage.
10/03/08   hk      Bringing in 7k Mailine fixes
08/29/08   ks      Added wl1_swfw_sync()
08/08/08   ks      Added WL1 bit manipulation function prototype
01/29/08   scm     Move normalize_sfn() macro from dlstates.h to l1utils.h.
09/04/07   rgn     Added support for stable GSTMR reads at frame boundary
05/30/07   rmak    Corrected typo in wl1_deregister_devman_client extern
05/22/07   rmak    MDSP Low speed optimization support
04/10/06   mg      Power Collapse Support
11/15/06   mg      Extern wl1_enable_tx_and_clock / wl1_disable_tx_and_clock
08/16/05   mg      Remove wl1_gsm_clock_halted()
03/15/06   scm     Extern wl1_convert_ppm_to_freq_err().
03/07/06   scm     Extern wl1_convert_freq_err_to_ppm().
02/07/06   bbd     Added support for GSM clock shutdown voting mechanism.
12/16/05   mg      Add more wrapper functions
12/16/05   mg      Created L1 wrapper functions for enabling/disabling fee/modem clks
12/13/05   mc      Moved macro for UNNORMALIZE_CFN from l1cmmgr.h to here.
11/29/05   bbd     Created L1 wrapper functions for enabling/disabling GSM clock.
01/28/04   scm     Add extern to wl1_get_mdsp_debug_data().
07/30/03   m       Created file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "customer.h"
#include "l1const.h"
#include "l1mathutil.h"
#include "nv.h"
#include "tcxomgr.h"
#include "l1def.h"
#include "tlm.h"

#include "wfw_sw_cmd_proc_intf.h"
#include "ds3g_ext.h"
#include "wnv_l1.h"
#include  "npa.h"
#include "qsh.h"

#if defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC
#include "wl1_qsh_ext.h"
#endif /*defined FEATURE_QSH_DUMP || defined FEATURE_QSH_EVENT_METRIC*/

typedef enum
{
  WL1_TCXOMGR_NONE,       /* Dont ack or report restriction change to TCXO Mgr */
  WL1_TCXOMGR_ACK,        /* Ack restriction change to TCXO Mgr */
  WL1_TCXOMGR_REPORT      /* Report restriction change to TCXO Mgr */
} wl1_tcxomgr_ack_type;


#ifdef FEATURE_WCDMA_L1_ZI_OPT
#include "modem_mem.h"
#endif

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

#define WCDMA_NUM_QSH_SUBS 2

/* logging related */
#define WCDMA_PERIODIC_STAT_l1mstate_BMSK 0x0000000F
#define WCDMA_PERIODIC_STAT_l1mstate_SHFT 0

#define WCDMA_PERIODIC_STAT_calltypemask_BMSK 0x000000F0
#define WCDMA_PERIODIC_STAT_calltypemask_SHFT 4

#define WCDMA_PERIODIC_STAT_rxdState_BMSK 0x00000001
#define WCDMA_PERIODIC_STAT_rxdState_SHFT 0

#define WCDMA_PERIODIC_STAT_NumCarrs_BMSK 0x00000006
#define WCDMA_PERIODIC_STAT_NumCarrs_SHFT 1

#define WCDMA_PERIODIC_STAT_qiceState_BMSK 0x00000003
#define WCDMA_PERIODIC_STAT_qiceState_SHFT 0

#define WCDMA_PERIODIC_STAT_qiceReason_BMSK 0x000000FC
#define WCDMA_PERIODIC_STAT_qiceReason_SHFT 2

#define WCDMA_PERIODIC_STAT_hsRab_BMSK 0x000000008
#define WCDMA_PERIODIC_STAT_hsRab_SHFT 3

#define WCDMA_PERIODIC_STAT_rxdTrigger_BMSK 0x0000FFFF
#define WCDMA_PERIODIC_STAT_rxdTrigger_SHFT 0

#define WCDMA_PERIODIC_STAT_rxdpm_BMSK 0x00FF0000
#define WCDMA_PERIODIC_STAT_rxdpm_SHFT 16

#define WCDMA_PERIODIC_STAT_ard_BMSK 0xFF000000
#define WCDMA_PERIODIC_STAT_ard_SHFT 24

#define WCDMA_PERIODIC_STAT_SET_VAL(buf, field, val) \
  (buf) &= (~WCDMA_PERIODIC_STAT_##field##_BMSK); \
  (buf) |= (((val) << WCDMA_PERIODIC_STAT_##field##_SHFT) & \
            (WCDMA_PERIODIC_STAT_##field##_BMSK))


#ifndef WCDMA_PERIODIC_STAT_LOG_PKT
#define WCDMA_PERIODIC_STAT_LOG_PKT WCDMA_PERIODIC_STAT_0x41B1_LOG_PKT
#else
#error "Redefinition of the log command code: WCDMA_PERIODIC_STAT_LOG_PKT"
#endif

#define WCDMA_PERIODIC_STAT_INVALID_VAL 0xFF
#define WCDMA_PERIODIC_STAT_LOG_NUM_BUF 2
#define NUM_MAX_WCDMA_PERIODIC_STAT_SAMPLES 50
#define WCDMA_PERIODIC_STAT_SAMPLE_SIZE sizeof(wcdma_periodic_stat_info_struct_type)

#define WCDMA_PERIODIC_STAT_LOG_MAX_BUF_SIZE  ((sizeof(wcdma_periodic_stat_info_struct_type)) * \
                                           NUM_MAX_WCDMA_PERIODIC_STAT_SAMPLES)

#define WL1_PRIMARY_CARRIER_IDX 0
#define WL1_SECONDARY_CARRIER_IDX 1
#define WL1_TERTIARY_CARRIER_IDX 2
#define WL1_MAX_CARRIERS 3

/* Header definition since the header definition is defined in log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).      */
} wcdma_periodic_log_hdr_struct_type;


typedef PACKED struct PACKED_POST
{
  uint8 cfn_info;
  /* l1 state and call type mask */
  uint8 l1m_state_and_call_type_mask;
  /* status of RxD,Num of Sec Carriers and HS RAB */
  uint8 advanced_receiver_status;
  /* Gives the reason for RxD to be ON */
  uint32 rxd_reason;
  /* Full QICE reason/QPC Reason for carrier*/
  uint8 qice_state_reason[WL1_MAX_CARRIERS];
} wcdma_periodic_stat_info_struct_type;

/* structure for the log packet wcdma_periodic_stat */
LOG_RECORD_DEFINE(WCDMA_PERIODIC_STAT_LOG_PKT)
  /* Version field */
  uint8 version;
  /* Number of samples */
  uint8 num_samples;
  /* wcdma periodic stat samples */
  uint8 info[WCDMA_PERIODIC_STAT_LOG_MAX_BUF_SIZE];
LOG_RECORD_END


/* Flag to store whether a HS log submit local command is pending or not */
extern boolean wcdma_periodic_stat_log_cmd_pending;
/* Flag for the status of the log code in QXDM */
extern boolean wcdma_periodic_stat_logging_enabled_qxdm;
/* Buffer index where samples are accumulated */
extern uint8 wcdma_periodic_stat_log_info_buf_idx;
/* Log buffer availability status database */
extern boolean wcdma_periodic_stat_log_buf_avail_status[WCDMA_PERIODIC_STAT_LOG_NUM_BUF];
/* The log buffer where samples are accumulated */
extern WCDMA_PERIODIC_STAT_LOG_PKT_type wcdma_periodic_stat_log_pkt_buf[WCDMA_PERIODIC_STAT_LOG_NUM_BUF];
/* Flag indicating whether a packet has been submitted to diag */
extern boolean wcdma_periodic_stat_log_submit;
/* The log buffer index to be submitted */
extern uint8 wcdma_periodic_stat_log_buf_idx_to_submit;
/* offset value in the log buffer*/
extern uint16 wcdma_periodic_stat_sample_buf_offset;

#ifdef FEATURE_DUAL_SIM
extern boolean wl1_use_backed_up_tcxo_mgr_rot;
#endif

#ifndef FEATURE_WCDMA_MCPM_DRIVER
/* GSM clock enable state */
extern boolean wl1_util_gsm_clk_ena_state;
#else
extern boolean wl1_mcpmdrv_w2g_client_status;
#endif

extern boolean wl1_enable_rtr_power_collapse_at_slp;

extern boolean wl1_fw_app_enabled;

extern boolean wl1_fw_app_suspended;

/*===========================================================================

                     MACROS

===========================================================================*/

#define UNNORMALIZE_CFN(this_cfn, w_r_t_cfn) \
  UNNORMALIZE((this_cfn), (w_r_t_cfn), MAX_CFN_COUNT)

#define normalize_sfn(a) ((uint16)normalize((int16)(a), MAX_SFN_COUNT))

#define WL1_UTIL_INTERF_BMSK      0x01
#define WL1_UTIL_GSM_MEAS_BMSK    0x02
#define WL1_UTIL_ALL_MEAS_BMSK    0x03

/* Various votes for deciding MDSP clk speed (medium or low) */
#define WL1_NON_DCH_OR_VOICE_ONLY       0x01 /* If L1 is not DCH or call is not voice only */
#define WL1_DCH_GSM_NEIGHBOURS          0x02 /* If GSM Neighbours are present */
#define WL1_DCH_HIGH_NUM_ACTIVE_FINGERS 0x04 /* If assigned fingers are more than MSMHW_MAX_NUM_FINGERS_FOR_LOW_MDSP_CLK_SPEED */
#define WL1_DCH_CLTD                    0x08 /* If CLTD is enabled */
#define WL1_MDSP_OPTIMIZATION_ALL_VOTES 0x0f
#define MOD_8_MASK                      0x7

/* Various threholds for ADC switching */
#define WL1_DSR_AGC_THRESH_HI         -55
#define WL1_DSR_AGC_THRESH_LOW_MIN    -80
#define WL1_DSR_AGC_THRESH_LOW_MAX    -70
#define WL1_DSR_JAMMER_THRESH_LOW      20
#define WL1_DSR_JAMMER_THRESH_HIGH     18
#define WL1_DSR_BAD_FRAME_CNT_THRESH   96 

/* DSR callback mask */
#define Wl1_DSR_CALLBACK_NONE   0x00
#define WL1_DSR_CALLBACK_RXD    0x01
#define WL1_DSR_CALLBACK_HS     0x02
#define WL1_DSR_CALLBACK_CM     0x04
#define WL1_DSR_CALLBACK_VOICE  0x08

/* Maximum size of circular buffer */
#define WL1_CIRC_BUFF_HISTORY_LENGTH 60

/* Do not dump if there are fewer than this many elements in the buffer */
#define WL1_CIRC_BUFF_MIN_THRESH 20

/* Force a dump if inserted past this point */
#define WL1_CIRC_BUFF_PANIC_THRESH 50

/* MUTEX for DSR configuration */
extern rex_crit_sect_type dsr_crit_sec;

/* the various events the Atuner module can receive */
typedef enum
{
  /* perform periodic timer expiration processing */
  WL1_ATUNER_PULSE_TIMER_EXPIRE,
  /* perform processing occuring at CM pattern start */
  WL1_ATUNER_CM_MEAS_PERIOD_START,
  /* perform processing occuring at FMO pattern start */
  WL1_ATUNER_FMO_MEAS_PERIOD_START,
  /* perform processing occuring at FMO or CM pattern end */
  WL1_ATUNER_MEAS_PERIOD_END,
  /* # events */
  WL1_ATUNER_NUM_EVENTS
} wl1_atuner_event_type;

/* this is the command type used by Atuner with wl1's local command system */
typedef struct
{
  l1_local_cmd_hdr_type hdr;
  wl1_atuner_event_type evt;
} wl1_atuner_local_cmd_type;

#define WL1_NPA_NUM_NODES 1

typedef enum
{
  WL1_NPA_CLK_CPU_Q6SW,
  WL1_NPA_NUM_RESOURCES 
}wl1_npa_resource_type;

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
#endif /*FEATURE_WCDMA_JOLOKIA_MODEM*/

/**************************************
**  WL1 BIT MANIPULATION MACROS     ***
***************************************/
 
/* transform a bit count into a byte count (add 7, divide by 8) */
#define WL1_BITS_TO_BYTES(x)      (((x) + 0x7) >> 3)

/* transform a bit count into a word count (add 31, divide by 32) */
#define WL1_BITS_TO_WORDS(x)      (((x) + 0x1F) >> 5)

/* transform a byte count into a bit count (multiply by 8) */
#define WL1_BYTES_TO_BITS(x) ((x)<<3)  

/* transform a byte count into a word count (add 3, divide by 4) */
#define WL1_BYTES_TO_WORDS(x)     (((x) + 0x3) >> 2)

/* transform a word count into a byte count (multiply by 4) */
#define WL1_WORDS_TO_BYTES(x)     ((x) << 2)

/* transform a word count into a bit count (multiply by 32) */
#define WL1_WORDS_TO_BITS(x)      ((x) << 5)





#define WCDMA_WL1_COMMON_MANAGER_CUSTOM_MSG       MSG_MASK_5  
#define WCDMA_WL1_UTILS_CUSTOM_MSG                MSG_MASK_6               
#define WCDMA_WL1_IDLE_MODE_CUSTOM_MSG            MSG_MASK_7        
#define WCDMA_WL1_SLEEP_MODE_CUSTOM_MSG           MSG_MASK_8          
#define WCDMA_WL1_NON_DCH_SEARCH_CUSTOM_MSG       MSG_MASK_9   
#define WCDMA_WL1_DCH_SEARCH_CUSTOM_MSG           MSG_MASK_10                         
#define WCDMA_WL1_DL_TLM_CUSTOM_MSG               MSG_MASK_11       
#define WCDMA_WL1_DL_CH_MGR_CUSTOM_MSG            MSG_MASK_12                     
#define WCDMA_WL1_TRIAGE_CUSTOM_MSG               MSG_MASK_13                                          
#define WCDMA_WL1_POWER_CNTRL_CUSTOM_MSG          MSG_MASK_14                              
#define WCDMA_WL1_HS_CUSTOM_MSG                   MSG_MASK_15                               
#define WCDMA_WL1_DL_DATA_PATH_CUSTOM_MSG         MSG_MASK_16                                     
#define WCDMA_WL1_ADV_RECEIVER_CUSTOM_MSG         MSG_MASK_17                                       
#define WCDMA_WL1_UL_CHNL_MGR_CUSTOM_MSG          MSG_MASK_18
#define WCDMA_WL1_HSPA_EUL_CFG_CUSTOM_MSG         MSG_MASK_19                                      
#define WCDMA_WL1_EUL_DATA_PATH_CUSTOM_MSG        MSG_MASK_20                    
#define WCDMA_WL1_CONN_MODE_IRAT_CUSTOM_MSG       MSG_MASK_21                
#define WCDMA_WL1_NON_CONN_MODE_IRAT_CUSTOM_MSG   MSG_MASK_22       



#define WL1_COMMON_MANAGER                        WCDMA_WL1_COMMON_MANAGER_CUSTOM_MSG 
#define WL1_UTILS                                 WCDMA_WL1_UTILS_CUSTOM_MSG                            

#ifdef FEATURE_CUST_1
#define WL1_IDLE_MODE                             WCDMA_WL1_IDLE_MODE_CUSTOM_MSG                 
#define WL1_SLEEP_MODE                            WCDMA_WL1_SLEEP_MODE_CUSTOM_MSG                   
#else
#define WL1_IDLE_MODE                             MSG_LEGACY_HIGH                 
#define WL1_SLEEP_MODE                            MSG_LEGACY_HIGH
#endif

#define WL1_NON_DCH_SEARCH                        WCDMA_WL1_NON_DCH_SEARCH_CUSTOM_MSG       
#define WL1_DCH_SEARCH                            WCDMA_WL1_DCH_SEARCH_CUSTOM_MSG                                 
#define WL1_DL_TLM                                WCDMA_WL1_DL_TLM_CUSTOM_MSG                   
#define WL1_DL_CH_MGR                             WCDMA_WL1_DL_CH_MGR_CUSTOM_MSG                               
#define WL1_TRIAGE                                WCDMA_WL1_TRIAGE_CUSTOM_MSG                                                      
#define WL1_POWER_CNTRL                           WCDMA_WL1_POWER_CNTRL_CUSTOM_MSG                                      
#define WL1_HS                                    WCDMA_WL1_HS_CUSTOM_MSG                                                
#define WL1_DL_DATA_PATH                          WCDMA_WL1_DL_DATA_PATH_CUSTOM_MSG                                           
#define WL1_ADV_RECEIVER                          WCDMA_WL1_ADV_RECEIVER_CUSTOM_MSG                                              
#define WL1_UL_CHNL_MGR                           WCDMA_WL1_UL_CHNL_MGR_CUSTOM_MSG          
#define WL1_HSPA_EUL_CFG                          WCDMA_WL1_HSPA_EUL_CFG_CUSTOM_MSG                                             
#define WL1_EUL_DATA_PATH                         WCDMA_WL1_EUL_DATA_PATH_CUSTOM_MSG                          
#define WL1_CONN_MODE_IRAT                        WCDMA_WL1_CONN_MODE_IRAT_CUSTOM_MSG                    
#define WL1_NON_CONN_MODE_IRAT                    WCDMA_WL1_NON_CONN_MODE_IRAT_CUSTOM_MSG        



#define WL1_CUSTOM_MSG0(CUSTOM_LEVEL, FMT) \
        MSG(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT)

#define WL1_CUSTOM_MSG1(CUSTOM_LEVEL, FMT, A) \
        MSG_1(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A)

#define WL1_CUSTOM_MSG2(CUSTOM_LEVEL, FMT, A, B) \
        MSG_2(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B)

#define WL1_CUSTOM_MSG3(CUSTOM_LEVEL, FMT, A, B, C) \
        MSG_3(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C)

#define WL1_CUSTOM_MSG4(CUSTOM_LEVEL, FMT, A, B, C, D) \
        MSG_4(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C, D)

#define WL1_CUSTOM_MSG5(CUSTOM_LEVEL, FMT, A, B, C, D, E) \
        MSG_5(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C, D, E)

#define WL1_CUSTOM_MSG6(CUSTOM_LEVEL, FMT, A, B, C, D, E, F) \
        MSG_6(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C, D, E, F)

#define WL1_CUSTOM_MSG7(CUSTOM_LEVEL, FMT, A, B, C, D, E, F, G) \
        MSG_7(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C, D, E, F, G)

#define WL1_CUSTOM_MSG8(CUSTOM_LEVEL, FMT, A, B, C, D, E, F,G,H) \
        MSG_8(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C, D, E, F,G,H)

#define WL1_CUSTOM_MSG9(CUSTOM_LEVEL, FMT, A, B, C, D, E, F, G, H, I) \
        MSG_9(MSG_SSID_WCDMA_L1, CUSTOM_LEVEL, FMT, A, B, C, D, E, F,G, H, I)

/* Enum for different types of RGS */
typedef enum
{
  WL1_RGS_TYPE_SUBSCRIPTION,
  WL1_RGS_TYPE_CLIENT
} wl1_rgs_type;

#ifdef FEATURE_WCDMA_OFFLINE_MODEM_W2G
/** Enum for the mode to set the sample server */
typedef enum
{
  /* Mode GSM */
  WL1_SAMPLE_SERVER_MODE_GSM,
  /* Mode QICE */
  WL1_SAMPLE_SERVER_MODE_QICE,
  /* Mode Diversity */
  WL1_SAMPLE_SERVER_MODE_DIVERSITY,
  /* Mode Max */
  WL1_SAMPLE_SERVER_MODE_MAX
} wl1_sample_server_mode_enum_type;

/** Enum for the RxD mode to set the sample server */
typedef enum
{
  /* Entering RxD */
  WL1_SAMPLE_SERVER_ENTER_RXD,
  /* Exiting RxD */
  WL1_SAMPLE_SERVER_EXIT_RXD,
} wl1_sample_server_rxd_mode_enum_type;
#endif

/* The 12-bit, unsigned representation of trk_lo varies from 0 to 4095, so
** this is its midpoint. */
#define WL1_TRK_LO_PDM_MIDPOINT  2048


/* FW expects only the searcher clock = VPE clock to be updated. Change absolutely everything to VPE type.*/
#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
/* no code */
#else
typedef enum
{
  /* VPE speed of 285 MHz, offline of MCPM_CLK_143M */
  WL1_SRCH_CLK_LOW_STATE  = WFW_SRCH_CLK_LOW_STATE,
  /* VPE speed of 499 MHz, offline of MCPM_CLK_250M*/
  WL1_SRCH_CLK_MED_STATE  = WFW_SRCH_CLK_MED_STATE,
  /* VPE speed of 666 MHz, offline of MCPM_CLK_333M */
  WL1_SRCH_CLK_HIGH_STATE = WFW_SRCH_CLK_HIGH_STATE,
  /* VPE speed of 799 MHz, offline of MCPM_CLK_399M */
  WL1_SRCH_CLK_TURBO_STATE = WFW_SRCH_CLK_TURBO_STATE,
  /* Offline clk speed when stack is not active or during sleep.*/
  WL1_SRCH_CLK_INVALID_STATE = WFW_NUM_OFFLINE_CLK_STATES
} wl1_srch_clk_speed_state_type;
#endif



typedef enum 
{
  DSR_LPM_MODE_ENABLE,
  DSR_LPM_MODE_DISABLE,
  DSR_LPM_MODE_INACTIVE
}wl1_dsr_rf_mode_type;


typedef struct 
{
  /* is voice call only active */
  boolean voice_call_only_active;
  uint16 bad_frame;
  uint16 bad_frame_cnt;
  int32 rx_agc;
  int32 filt_rx_agc;
  boolean filt_init;
  boolean power_savings_enable_state;
  boolean hs_active;
  boolean cm_active;
  boolean rxd_active;
  /* jammer presence detected */
  boolean jammer_present;
  uint16  frame_timer_val;
  uint16 consecutive_bounce_back_cnt;
  boolean start_frame_timer;
  wl1_dsr_rf_mode_type wl1_dsr_last_comit_action;
  uint16 dsr_cmd_sent_cfn;
  boolean state_machine_active;
}wl1_dsr_db_type;


/* Type of F3 replacement messages to dump*/
typedef enum
{
  WL1_CIRC_BUFF_SRCH_HIST, 
  WL1_CIRC_BUFF_NASTT_HIST,
  WL1_CIRC_BUFF_NASTT_DETECT_HIST,
  WL1_CIRC_BUFF_CELL_HIST,
  WL1_CIRC_BUFF_ASET_FILT_HIST,
  WL1_CIRC_BUFF_SIRT_FILT_HIST,
  WL1_CIRC_BUFF_FILT_ECIO_HIST,
  WL1_CIRC_BUFF_RXD_FILT_HIST
} wl1_circ_buff_msg_enum_type;

/* Structure to define the srch_hist fields*/
typedef PACKED struct PACKED_POST
{
  /* Variable to indicate the type of search performed */
  uint8 srch_id;              
  /* Free-running frame counter */
  uint32 fn;
  /* Variable to indicate the frame reference counter */
  uint16 frc;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Variable to indicate the timestick of the system */
  uint32 timestamp;
} wl1_circ_buff_srch_hist_struct_type;

/* Structure to define nastt_hist fields */ 
typedef PACKED struct PACKED_POST
{
  /* Variable to indicate the frame number the search began on */
  uint8 start_cfn;
  /* The current frame number as determined by the search heartbeat handler */
  uint8 cur_cfn;
  /* The current frame number from the sequencer API */
  uint8 seq;
  /* The number of frames reserved for NASTT searches */
  uint8 num_frames;
  /* Type of search done */
  uint8 type;
  /* Milliseconds since UE was powered on at the time of the search event */
  uint32 timestamp;
} wl1_circ_buff_nastt_hist_struct_type;

/* Structure to define nastt_detect_hist fields */
typedef PACKED struct PACKED_POST
{
  /* The primary scrambling code of the cell that was searched */
  uint16 psc;
  /* The cell's raw energy */
  uint32 eng;
  /* The cell's peak position, in cx8 */
  uint32 pos;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Milliseconds since UE was powered on at the time of the search event */
  uint32 timestamp;
} wl1_circ_buff_nastt_detect_hist_struct_type;

/* Structure to define cell_hist fields */
typedef PACKED struct PACKED_POST
{
  /* The number of cells in the union of the ASET, DC_ASET, ADD_CSET, DROP_CSET, and CAND_USET */
  uint8 num_cells;
  /* The number of cells in the union of the SYNC_NSET, USET, and DC_SYNC_ALL_SET */
  uint8 sync_cells;
  /* Horse power utilization by compressed mode searches */
  uint32 d_cm;
  /* D_CM normalized per cell */
  uint8 hp;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Milliseconds since UE was powered on at the time of the event */
  uint32 timestamp;
} wl1_circ_buff_cell_hist_struct_type;

/* Structure to define aset_filt_hist fields */
typedef PACKED struct PACKED_POST
{
  /* The primary scrambling code of the cell that was filtered */
  uint16 psc;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Milliseconds since UE was powered on at the time of the event */
  uint32 timestamp;
} wl1_circ_buff_aset_filt_hist_struct_type;

/* Structure to define sirt_filt_hist */
typedef PACKED struct PACKED_POST
{
  /* The length of the filter used to calculate Ec/I0 */
  uint16 filt_len;
  /* The filtered Ec/I0. Print as (-0.5 dB * FILT_ECIO) */
  uint16 filt_ecio;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Milliseconds since UE was powered on at the time of the event */
  uint32 timestamp;
} wl1_circ_buff_sirt_filt_hist_struct_type;

/* Structure to define filt_ecio_hist fields */
typedef PACKED struct PACKED_POST
{
  /* The primary scrambling code of the cell that was filtered */
  uint16 psc;
  /* The previous filtered Ec/I0. Print as (-0.5 dB * OLD_ECIO) */
  uint8 old_ecio;
  /* The filtered Ec/I0. Print as (-0.5 dB * FILT_ECIO) */
  uint8 filt_ecio;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Milliseconds since UE was powered on at the time of the event */
  uint32 timestamp;
} wl1_circ_buff_filt_ecio_hist_struct_type;

/* Structure to define rxd_filt_hist fields */
typedef PACKED struct PACKED_POST
{
  /* The primary scrambling code of the cell that was filtered */
  uint16 psc;
  /* The set that the cell belongs to */
  uint8 set;
  /* RSCP of the current cell */
  uint16 path_thresh;
  /* Raw energy of the current cell */
  uint32 eng;
  /* Instantaneous peak position of the current cell */
  uint32 pos;
  /* RSSI of the current cell */
  uint16 rssi;
  /* RSCP of the current cell */
  uint16 rscp;
  /* Reference position of the current cell */
  uint32 ref_pos;
  /* Position of the start of the current cell's window */
  uint32 start_win;
  /* Variable to indicate current frame number*/
  uint8 cfn;
  /* Milliseconds since UE was powered on at the time of the event */
  uint32 timestamp;
} wl1_circ_buff_rxd_filt_hist_struct_type;

/* Declares structures corresponding to the type of F3
   replacement message */
typedef union 
{
  wl1_circ_buff_srch_hist_struct_type srch_hist;
  wl1_circ_buff_nastt_hist_struct_type nastt_hist;
  wl1_circ_buff_nastt_detect_hist_struct_type nastt_detect_hist;
  wl1_circ_buff_cell_hist_struct_type cell_hist;
  wl1_circ_buff_aset_filt_hist_struct_type aset_filt_hist;
  wl1_circ_buff_sirt_filt_hist_struct_type sirt_filt_hist;
  wl1_circ_buff_filt_ecio_hist_struct_type filt_ecio_hist;
  wl1_circ_buff_rxd_filt_hist_struct_type rxd_filt_hist;
} wl1_circ_buff_diff_event_hist_type;

/* struct for circular buffer */
typedef struct 
{
  /* An index variable for pointing to the next position where element needs to be inserted */
  uint8 write_idx;
  /* An index variable for pointing to the next element which needs to be dumped */
  uint8 read_idx;
  /* A counter to keep a track of the number of elements inserted into the cicular buffer */
  uint64 write_cnt;
  /* A counter to keep a track of the number of elements dumped from the circular buffer */
  uint64 read_cnt;
  /* Counter to keep track of number of times an element has been overwritten due to overflow */
  uint64 skip_cnt;                             
  /* Generating an array to be used as a circular buffer */ 
  wl1_circ_buff_diff_event_hist_type cb[WL1_CIRC_BUFF_HISTORY_LENGTH]; 
} wl1_circ_buff_struct_type;

/* stores time on GSM multiframe timeline */
typedef struct
{
  uint16 qs;
  uint8 gfn;  
} tlm_gfn_struct_type;

/* the types of snapshots available and a bitmask for specifying a group of 
   those types */
typedef uint8 wl1_snapshot_selection_bmsk_type;
typedef enum
{
  WL1_SNAP_G,
  WL1_SNAP_W_FRC,
  WL1_SNAP_W_DL,
  WL1_SNAP_W_UL,
  WL1_SNAP_L,
} wl1_snapshot_types;

/* Used to store a timing snapshot between the 3 RAT types (GSM/WCDMA/LTE) */
typedef struct
{
  /* determines which of the snapshot fields are valid in the rest of this 
     structure */
  wl1_snapshot_selection_bmsk_type valid_bmsk;

  /* stores GSM time taken at snapshot */
  tlm_gfn_struct_type g;
  /* stores the frc wcdma time taken at snapshot. */
  tlm_tfn_struct_type w_frc;
  /* stores the wcdma serving-cell combiner value taken at the snapshot.
     In DCH state this combiner will be aligned to the DPCH frame boundary
     Otherwise it's aligned to the serving cell's pccpch. */
  tlm_tfn_struct_type w_dl;
  /* stores the uplink wcdma time taken at the snapshot */
  tlm_tfn_struct_type w_ul;
  /* stores the lte ostmr time taken at the snapshot */
  uint32 l;
} wl1_gwl_snapshot_type;

#ifdef FEATURE_QSH_EVENT_METRIC 

/*Action Config Enum*/
typedef enum {
  START_METRIC,
  STOP_METRIC,
  LOG_METRIC,
  NO_OP
}wl1_qsh_metric_action_enum_type;

/*Metric table structure*/
typedef boolean wl1_metric_cb_type(wl1_qsh_metric_e metric_id, uint8* addr_ptr, wl1_qsh_metric_action_enum_type metric_action);

typedef struct
{
/*address of the metric*/
  uint8* metric_addr;
/*corresponding cb to call*/
  wl1_metric_cb_type* metric_cb_ptr;
  void* metric_context_id;
}wl1_qsh_metric_db_struct_type;

#endif

#ifdef FEATURE_QSH_DUMP 
typedef void wl1_dump_cb_type(void* write_ptr);

typedef struct
{
/*address of the dump*/
  uint32 dump_struct_size;
  /*corresponding cb to call*/
  wl1_dump_cb_type* dump_cb_ptr;
}wl1_qsh_dump_db_struct_type;

extern wl1_common_mini_debug_info_struct_type *wl1_qsh_dump_tag_0_ptr;
extern wl1_dl_mini_struct_type* wl1_qsh_dump_tag_1_ptr;
extern wl1_srcher_mini_debug_info_struct_type* wl1_qsh_dump_tag_2_ptr;
extern wl1_drx_sleep_mini_debug_info_struct_type* wl1_qsh_dump_tag_3_ptr;
extern wl1_cell_resel_mini_debug_info_struct_type* wl1_qsh_dump_tag_4_ptr;
extern wl1_ul_mini_debug_info_struct_type* wl1_qsh_dump_tag_5_ptr;
extern wl1_rxd_mini_debug_info_struct_type* wl1_qsh_dump_tag_6_ptr;
extern wl1_cme_mini_debug_info_struct_type* wl1_qsh_dump_tag_7_ptr;
extern wl1_irat_mini_debug_info_struct_type* wl1_qsh_dump_tag_8_ptr;
extern wl1_datapath_mini_debug_info_struct_type* wl1_qsh_dump_tag_9_ptr;
#ifdef FEATURE_DUAL_SIM
extern wl1_dsds_mini_debug_struct_type* wl1_qsh_dump_tag_10_ptr;
#endif /*FEATURE_DUAL_SIM*/

#endif /* FEATURE_QSH_DUMP */
/* Structure type declaration */
/* -------------------------- */
/* This structure keeps track of Tx AGC for logging */
typedef struct{
  boolean enable_tx_duration_logging;
  uint32 wl1_tx_bin[DS3G_TX_POWER_BIN_MAX];
} wl1_log_tx_duration_info_struct_type;

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_WCDMA_L1_ZI_OPT
/*===========================================================================

FUNCTION WL1_MEM_ALLOC

DESCRIPTION
  Allocates memory dynamically from the Modem HEAP.
  
DEPENDENCIES
  None

RETURN VALUE
  Pointer to the allocated memory chunk

SIDE EFFECTS
  None
  
===========================================================================*/
INLINE void* wl1_mem_alloc
(
  size_t               size       /*!< Number of bytes to allocate */
)
{
  void *ptr;
  /* Allocate 'size' bytes from Modem Heap */
  ptr = modem_mem_alloc (size, MODEM_MEM_CLIENT_WCDMA_CRIT);

  if (ptr == NULL)
  {
    ERR_FATAL("modem_mem_alloc failed for WL1, requested size %d bytes", size, 0, 0);
  }

  memset(ptr, 0, size);

  return ptr;
}

/*===========================================================================

FUNCTION WL1_MEM_FREE

DESCRIPTION
  De-allocates the dynamically allocated memory from the heap
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
INLINE void wl1_mem_free
(
  void                *ptr        /*!< Memory to free */
)
{
  /* Free the heap memory pointed to by 'ptr' */
  modem_mem_free (ptr, MODEM_MEM_CLIENT_WCDMA_CRIT);
}

#endif /* FEATURE_WCDMA_L1_ZI_OPT */

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION WL1_MEM_ALLOC_EXT_CMD

DESCRIPTION
  Allocates memory dynamically from the Modem HEAP.
  
DEPENDENCIES
  None

RETURN VALUE
  Pointer to the allocated memory chunk

SIDE EFFECTS
  None
  
===========================================================================*/
extern void* wl1_mem_alloc_ext_cmd
(
  size_t               size       /*!< Number of bytes to allocate */
);

/*===========================================================================

FUNCTION WL1_MEM_FREE_EXT_CMD

DESCRIPTION
  De-allocates the dynamically allocated memory from the heap
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void wl1_mem_free_ext_cmd
(
  void                *ptr        /*!< Memory to free */
);
#endif /*FEATURE_WCDMA_FREE_FLOATING_TASK*/

/*===========================================================================
FUNCTION     L1_NV_PROCESS_NV_CMD_SIG

DESCRIPTION
  Processes L1_NV_CMD_SIG from NV

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void l1_nv_process_nv_cmd_sig(void);


/*===========================================================================
FUNCTION     WL1_GSM_CLK_CTRL_LOCK_MUTEX

DESCRIPTION
  This function locks the "wl1_gsm_clk_ctrl_crit_sect" mutex

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_gsm_clk_ctrl_lock_mutex( void );

/*===========================================================================
FUNCTION     WL1_GSM_CLK_CTRL_UNLOCK_MUTEX

DESCRIPTION
  This function unlocks the "wl1_gsm_clk_ctrl_crit_sect" mutex

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_gsm_clk_ctrl_unlock_mutex( void );

/*===========================================================================
FUNCTION     WL1_REGISTER_DEVMAN_CLIENT

DESCRIPTION
  Register WL1 client to influence MDSP clk speed

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  void

RETURN VALUE
  void

SIDE EFFECTS
  May increase MDSP clk speed
===========================================================================*/
extern void wl1_register_devman_client(void);

/*===========================================================================
FUNCTION     WL1_DEREGISTER_MDSP_CLIENT

DESCRIPTION
  De-register WL1 client to no longer influence MDSP clk speed

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  void

RETURN VALUE
  void

SIDE EFFECTS
  May decrease MDSP clk speed
===========================================================================*/
extern void wl1_deregister_devman_client(void);

/*===========================================================================
FUNCTION     WL1_MDSP_CLK_SPEED_VOTE_IS_VALID

DESCRIPTION
  Confirm if client bitmask is valid
  (i.e. has only one bit set and is one of the valid clients)

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  uint8 vote

RETURN VALUE
  TRUE  if vote is valid
  FALSE if vote is invalid

SIDE EFFECTS
  none
===========================================================================*/
extern boolean wl1_mdsp_clk_speed_vote_is_valid(uint8 vote);

/*===========================================================================
FUNCTION     WL1_REQUIRE_MDSP_MED_SPEED

DESCRIPTION
  Add Vote for MDSP to run at Medium speed

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  uint8 vote

RETURN VALUE
  TRUE  if vote was successfully added
  FALSE if vote was not recognized or not needed

SIDE EFFECTS
  May increase MDSP clk speed
===========================================================================*/
extern boolean wl1_require_mdsp_med_speed(uint8 vote);

/*===========================================================================
FUNCTION     WL1_RELINQUISH_MDSP_MED_SPEED

DESCRIPTION
  Remove Vote for MDSP to run at Medium Speed

DEPENDENCIES
  Clk regime support for Lower MDSP speed

PARAMETERS
  uint8 vote

RETURN VALUE
  TRUE  if vote was successfully removed
  FALSE if vote was not recognized or not already set

SIDE EFFECTS
  May decrease MDSP clk speed
===========================================================================*/
extern boolean wl1_relinquish_mdsp_med_speed(uint8 vote);

/*===========================================================================
FUNCTION     WL1_QUERY_GSM_CLOCK_STATE

DESCRIPTION
  Returns current state of GSM clock

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  GSM clock enabled (TRUE) or disabled (FALSE).

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_query_gsm_clock_state(void);

/*===========================================================================
FUNCTION     WL1_ENABLE_FEE_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to enable FEE clock.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  FEE clock is enabled.
===========================================================================*/
extern void wl1_enable_fee_clock(void);

/*===========================================================================
FUNCTION     WL1_DISABLE_FEE_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to disable FEE clock.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  FEE clock is disabled.
===========================================================================*/
extern void wl1_disable_fee_clock(void);

/*===========================================================================
FUNCTION     WL1_ENABLE_TX_AND_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to enable TX clock and to 
  turn ON RF TX chain.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  TX clock is enabled.
===========================================================================*/
extern void wl1_enable_tx_and_clock(void);

/*===========================================================================
FUNCTION     WL1_DISABLE_TX_AND_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to disable TX clock and to
  turn OFF RF TX chain.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  TX clock is disabled.
===========================================================================*/
extern void wl1_disable_tx_and_clock(void);

/*===========================================================================
FUNCTION     WL1_ENTERING_SLEEP

DESCRIPTION
  Wrapper function for turning TCXO and TRK_LO off.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_entering_sleep(void);

/*===========================================================================
FUNCTION     WL1_EXITING_SLEEP

DESCRIPTION
  Wrapper function for turning TCXO and TRK_LO on.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_exiting_sleep(void);

/*===========================================================================
FUNCTION     WL1_ENABLE_CLOCKS_EXITING_SLEEP

DESCRIPTION
  Wrapper function for  calling clock regime APIs to enable clocks when 
  coming out of sleep.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_enable_clocks_exiting_sleep(void);

/*===========================================================================
FUNCTION     WL1_DISABLE_CLOCKS_ENTERING_SLEEP

DESCRIPTION
  Wrapper function for  calling clock regime APIs to disable clocks when 
  entering sleep.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_disable_clocks_entering_sleep(void);

/*===========================================================================
FUNCTION     WL1_ENABLE_CLOCKS

DESCRIPTION
  Wrapper function for calling clock regime API to enable WCDMA clocks.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_enable_clocks(void);

/*===========================================================================
FUNCTION     WL1_DISABLE_CLOCKS

DESCRIPTION
  Wrapper function for calling clock regime API to disable WCDMA clock.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_disable_clocks(void);

/*===========================================================================
FUNCTION     WL1_DISABLE_SLP_FAST_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to disable SLP FAST CLOCK.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_disable_slp_fast_clock(void);

/*===========================================================================
FUNCTION     WL1_ENABLE_SLP_FAST_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to enable SLP FAST CLOCK.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_enable_slp_fast_clock(void);

/*===========================================================================
FUNCTION     WL1_WRITE_FBIBER_THRESHOLD

DESCRIPTION
  Function to write to mdsp variable fbiBER_Threshold

DEPENDENCIES

PARAMETERS
  uint16 threshold - value to write to mdsp variable fbiBER_Threshold.

RETURN VALUE
  boolean - returns whether write was successful (TRUE) or not (FALSE)

SIDE EFFECTS

===========================================================================*/
extern boolean wl1_write_fbiber_threshold(uint16 threshold);

/*===========================================================================
FUNCTION     WL1_READ_FBIBER_THRESHOLD

DESCRIPTION
  Function to read from mdsp variable fbiBER_Threshold

DEPENDENCIES

PARAMETERS
  uint16 *threshold - pointer to store value read from mdsp variable 
                               fbiBER_Threshold.

RETURN VALUE
  boolean - returns whether read was successful (TRUE) or not (FALSE)

SIDE EFFECTS

===========================================================================*/
extern boolean wl1_read_fbiber_threshold(uint16 *threshold);

/*===========================================================================
FUNCTION     WL1_CONVERT_FREQ_ERR_TO_PPM

DESCRIPTION
  This function takes the VCTCXO frequency error in units of 7.3242 Hz as a
  parameter.  Then, based upon the current carrier frequency, it converts
  that error to units of 2^-10 ppm.

DEPENDENCIES
  None.

PARAMETERS
  raw_freq_error - From mDSP freqCommon register, units of 7.3242 Hz.

RETURN VALUE
  int16 - VCTCXO frequency error in units of 2^-10 ppm.

SIDE EFFECTS
  None.
===========================================================================*/
extern int32 wl1_convert_freq_err_to_ppm(int32 raw_freq_error);

/*===========================================================================
FUNCTION     WL1_CONVERT_PPM_TO_FREQ_ERR

DESCRIPTION
  This function receives a parameter in units of (ppm * 100).  So for 3.5 ppm,
  the caller should pass 350.  Then, based upon the current carrier frequency,
  it converts it to units of 7.3242 Hz.  This corresponds to units used by
  the mDSP to manage VCTCXO.

DEPENDENCIES
  None.

PARAMETERS
  int16 ppm_times_one_hundred (actual ppm value * 100, i.e. 2.25 ppm -> 225).

RETURN VALUE
  int16 - VCTCXO frequency error in units of 7.3242 Hz for the mDSP.

SIDE EFFECTS
  None.
===========================================================================*/
extern int16 wl1_convert_ppm_to_freq_err(int16 ppm_times_one_hundred);

/*===========================================================================
FUNCTION     WL1_UPDATE_RGS_VALUES

DESCRIPTION
  This function is called periodically when we have service to update Recent
  Good System data in TCXO Manager.  It handles any tweaking or converting
  of the data to get it to a format that TCXO Manager expects.

DEPENDENCIES
  None.

PARAMETERS
  int32 mdsp_pdm - trk_lo PDM value read from mDSP.
  int32 mdsp_rot - Rotator value read from mDSP.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_update_rgs_values(int32 mdsp_pdm, int32 mdsp_rot);

/*===========================================================================
FUNCTION     WL1_GET_STORED_VCO

DESCRIPTION
  Function called to get trk_lo and rotator values from TCXO MGR.  Applies
  only to FEATURE_WCDMA_TCXOMGR30, where TCXO MGR handles Recent Good System
  validation, RGS aging, temperature table access, etc.  WL1 then uses these
  values when attempting acquisition.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  tcxomgr_vco_info_type - TCXO MGR VCTCXO information structure.

SIDE EFFECTS
  None.
===========================================================================*/
extern tcxomgr_vco_info_type wl1_get_stored_vco(void);


/*===========================================================================
FUNCTION     WL1_CHECK_GSTMR_RD_STATUS

DESCRIPTION
  This function does two successive gstmr reads and makes sure that the counters
  are being read when gstmr is stable

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Quarter symbol count from gstmr

SIDE EFFECTS
  None.
===========================================================================*/
extern uint32 wl1_check_gstmr_rd_status(void);

/*===========================================================================
FUNCTION l1utils_init

DESCRIPTION
  Function initializes the global variables in l1utils
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  Initializes global variables
  
===========================================================================*/
extern void l1utils_init (void);

/*===========================================================================
FUNCTION     __lmbd

DESCRIPTION
  This function finds the leftomost occurence(MSB) of "bit_val_to_match" in "datum". It does comparision till stop_pos bit

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  position of leftmost bit pattern. check return type for -ve value.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 __lmbd64(uint64 datum, uint8 stop_pos);

uint8 __lmbd32(uint32 datum, uint8 stop_pos);

uint8 __lmbd16(uint16 datum, uint8 stop_pos);

uint8 __lmbd8(uint8 datum, uint8 stop_pos);




/*===========================================================================
FUNCTION     __rmbd

DESCRIPTION
  This function finds the rightmost occurence of "bit_val_to_match" in "datum". It does comparision starting from bit pos
  start_pos 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  position of leftmost bit pattern. check return type for -ve value.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 __rmbd64(uint64 datum, uint8 start_pos);
  
uint8 __rmbd32(uint32 datum, uint8 start_pos);

uint8 __rmbd16(uint16 datum, uint8 start_pos);

uint8 __rmbd8(uint8 datum, uint8 start_pos);

/*===========================================================================
FUNCTION     NUM_SET_BITSX   x: 64/32/16/8

DESCRIPTION
  This function counts number of bits set in a given "datum"

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
 number of set bits in the datum

SIDE EFFECTS
  None.
===========================================================================*/
 uint8 num_set_bits64(uint64 datum);
 uint8 num_set_bits32(uint32 datum);
 uint8 num_set_bits16(uint16 datum);
 uint8 num_set_bits8(uint8 datum);

 /*===========================================================================
 FUNCTION     count_set_bit_indices   x: 8
 
 DESCRIPTION
   This function counts the index positions of the bits set in datum
 
 DEPENDENCIES
   None.
 
 PARAMETERS
   uint8 datum 
 
 RETURN VALUE
  sum of the indices of the bits set
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
 uint8 count_set_bit_indices(uint8 datum, uint8 start_pos);

 /*===========================================================================

FUNCTION L1_COPY_to_WORD

DESCRIPTION
  This function copies data from uint8 pointer into a word(uint32) byte-by-byte
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 l1_copy_to_word(uint8 *src_ptr);

/*===========================================================================
FUNCTION     WL1_WFW_SET_TCXO_FREEZE_FLAG

DESCRIPTION
  This function tells firmware whether or not to freeze TCXO.

DEPENDENCIES
  None.

PARAMETERS
  tcxo_frozen - TRUE means freeze TCXO.
              \ FALSE means unfreeze TCXO.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_wfw_set_tcxo_freeze_flag(boolean tcxo_frozen);

/*===========================================================================
FUNCTION     WL1_WFW_SET_USE_FREQ_HYPO_FLAG

DESCRIPTION
  This function tells firmware whether or not to use the frequency hypothesis
  value set previously by software.

DEPENDENCIES
  None.

PARAMETERS
  use_freq_hypo - TRUE means use frequency hypothesis.
                \ FALSE means do not use frequency hypothesis.
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_wfw_set_use_freq_hypo_flag(boolean use_freq_hypo);

/*===========================================================================
FUNCTION     WL1_SET_CURR_SRCH_CLK_SPEED

DESCRIPTION
  This sets the internal offline clock speed state to the specified value.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
extern void wl1_set_curr_srch_clk_speed(uint32 srch_clk_speed_khz);

/*===========================================================================
FUNCTION     WL1_GET_CURR_SRCH_CLK_SPEED

DESCRIPTION
  Returns the current offline clock speed state.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  Enum corresponding to one of the states - LOW,MED or HIGH state.

SIDE EFFECTS

===========================================================================*/
extern uint32 wl1_get_curr_srch_clk_speed(void);

#ifdef FEATURE_XO

/*===========================================================================
FUNCTION     WL1_GET_CARRIER_FREQUENCY

DESCRIPTION
  This function returns the carrier frequency in khz

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

uint32 wl1_get_carrier_frequency(uint8 carr_idx);

/*===========================================================================
FUNCTION     WL1_CONVERT_PPM_TO_FREQ_ERR_FOR_RXFE

DESCRIPTION
  This function receives a parameter in units of (ppm * 100).  So for 3.5 ppm,
  the caller should pass 350.  Then, based upon the current carrier frequency,
  it converts it to units of Q6.

DEPENDENCIES
  None.

PARAMETERS
  int16 ppm_times_one_hundred (actual ppm value * 100, i.e. 2.25 ppm -> 225).

RETURN VALUE
  int16 - VCTCXO frequency error in units of Q6.

SIDE EFFECTS
  None.
===========================================================================*/
extern int32 wl1_convert_ppm_to_freq_err_for_rxfe(int32 ppm_times_one_hundred);



/*===========================================================================
FUNCTION     WL1_AFC_TTL_INIT

DESCRIPTION
  This function initializes frontend rotator and MND counter for XO feature

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_afc_ttl_init(void);
/*===========================================================================
FUNCTION     WL1_GET_XO_PARAMS

DESCRIPTION
  This function prints the inner/outer/total freq error in the system.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_get_xo_params(void);
#endif /* FEATURE_XO */

#ifdef FEATURE_WCDMA_MCPM_DRIVER
boolean wl1_wait_for_gsm_frame_boundary(void);
#endif 

/*===========================================================================
FUNCTION  WL1_IS_FW_APP_ENABLED

DESCRIPTION
  This function returns if the FW APP is enabled for WCDMA.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if FW APP enabled, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_is_fw_app_enabled(void);

/*===========================================================================
FUNCTION  WL1_IS_FW_APP_SUSPENDED

DESCRIPTION
  This function returns if the FW APP is suspended for WCDMA.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if FW APP suspended, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_is_fw_app_suspended(void);


/*===========================================================================
FUNCTION 

DESCRIPTION
   
RETURN VALUE
   .
SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_dsr_controller(uint32 callback_mask);



/*===========================================================================
FUNCTION wl1_dsr_controller

DESCRIPTION
    API to return the state machine active status
 
RETURN VALUE
    Boolean.
SIDE EFFECTS
  None.
=============================================================================*/
extern boolean wl1_dsr_state_machine_active(void);

/*===========================================================================
FUNCTION wl1_dsr_cleanup

DESCRIPTION
    
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_dsr_cleanup(void);

/*===========================================================================
FUNCTION wl1_dsr_qual_meas_update

DESCRIPTION
    Controller for various DSR transitions
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_dsr_qual_meas_update(uint8 num_tbs, uint8 num_err);

/*===========================================================================
FUNCTION wl1_dsr_init

DESCRIPTION
    DSR init
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_dsr_init(void);

/*===========================================================================
FUNCTION wl1_dsr_read_nv_item

DESCRIPTION
    DSR init
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_dsr_read_nv_item(void);

#ifdef FEATURE_WCDMA_MCPM_DRIVER

/* New Voting bitmasks for GSM clocks */
#define WL1_UTIL_VOTE_GSM_CLK_FOR_INIT   0x01
#define WL1_UTIL_VOTE_GSM_CLK_FOR_SRCH   0x02
#define WL1_UTIL_VOTE_ALL_MEAS_BMSK      0x03

/*===========================================================================
FUNCTION     WL1_DISABLE_GSM_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to disable GSM clock.
  Checks GSTMR to see if GFN rollover is about to happen.  If so, we
  need to delay the clock shutdown to ensure MDSP isn't still using
  anything.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  GSM clock is disabled.
===========================================================================*/
extern void wl1_vote_disable_gsm_clock(uint8 module_bmsk);

/*===========================================================================
FUNCTION     WL1_ENABLE_GSM_CLOCK

DESCRIPTION
  Wrapper function for calling clock regime API to enable GSM clock.

DEPENDENCIES
  Clock regime driver.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  GSM clock is enabled.
===========================================================================*/
extern void wl1_vote_enable_gsm_clock(uint8 module_bmsk);
#endif 

/*===========================================================================
FUNCTION wl1_npa_request_node_config

DESCRIPTION
    This function requests a specific config(speed)for a specific resource with NPA.
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_npa_request_node_config(wl1_npa_resource_type resource);

/*===========================================================================
FUNCTION wl1_npa_relinquish_node_config

DESCRIPTION
    This function relinquishes a specific config(speed)for a specific resource with NPA.
 
RETURN VALUE
   None.
SIDE EFFECTS
  None.
=============================================================================*/
extern void wl1_npa_relinquish_node_config(wl1_npa_resource_type resource);

/*===========================================================================
 FUNCTION     WL1_UARFCN_TO_FREQ_KHZ

 DESCRIPTION
   This converts the given UARFCN to its corresponding carrier frequency, in
   kHz.

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   Carrier frequency, in kHz

 SIDE EFFECTS
   None.
===========================================================================*/
extern uint32 wl1_uarfcn_to_freq_khz(uint16 uarfcn);

#ifdef FEATURE_XO
/*===========================================================================
 FUNCTION     WL1_FREEZE_XO_LOOPS

 DESCRIPTION
   This freezes XO loops in FW

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None.
===========================================================================*/
extern void wl1_freeze_xo_loops(void);
/*===========================================================================
 FUNCTION     WL1_UNFREEZE_XO_LOOPS

 DESCRIPTION
   This unfreezes XO loops in FW

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None.
===========================================================================*/
extern void wl1_unfreeze_xo_loops(void);
#endif /* FEATURE_XO */
/*===========================================================================
 FUNCTION     WCDMA_L1_GET_TCB

 DESCRIPTION
   This function returns a pointer to the WCDMA L1 TCB. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   A pointer to the WCDMA L1 TCB.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern rex_tcb_type* wcdma_l1_get_tcb(void);

/*===========================================================================
FUNCTION WL1_CIRC_BUFF_INSERT

DESCRIPTION
  Inserts elements into the circular buffer for logging purposes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The source will be copied into the destination.
===========================================================================*/
void wl1_circ_buff_insert(wl1_circ_buff_diff_event_hist_type* src, 
                          wl1_circ_buff_struct_type*          dest, 
                          wl1_circ_buff_msg_enum_type         type);

/*=========================================================================
FUNCTION WL1_CIRC_BUFF_DUMP

DESCRIPTION
  This function prints the most recent len number of elements from buff for 
  logging purposes. Also, it decides which type of log packet needs to be
  populated and allocates memory for the approriate log packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_circ_buff_dump(wl1_circ_buff_struct_type* buff, 
                        wl1_circ_buff_msg_enum_type type);

/*===========================================================================
FUNCTION WL1_POPULATE_LOG_PACKET

DESCRIPTION:
  This function replaces certain F3 messages by dumping the data from a 
  particular type of circular buffer into its corresponding log packet e.g. 
  wl1_circ_buff_srch_hist_struct_type data into its appropriate log packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A log packet will be submitted, if the approriate ID is enabled.
===========================================================================*/
void wl1_populate_log_packet
(
  wl1_circ_buff_struct_type* buff,
  uint8 num_elements,
  void* log_pkt, 
  uint16 buff_size
);

/*===========================================================================
FUNCTION wl1_advance_snapshot_to_cfn_time

DESCRIPTION
  Returns a g/w/l snapshot that is the result of an input snapshot shifted 
  forward to the specified dpch-aligned CFN-based time.

  Note gsm conversion can lose precision due to fixed point division. Therefore
  if two snapshots are needed from a starting reference snapshot, calculate 
  both using the starting reference position rather than calculating the second
  position using the first.

  It is ok to pass the same snapshot pointer as input and output.

DEPENDENCIES
  - The w_dl field of the input snapshot must be valid.
  - The target point should be ahead in time relative to the snapshot's w_dl field

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_advance_snapshot_to_cfn_time(
  /* the location we want the output snapshot at */
  tlm_cfn_struct_type *target,
  /* the input snapshot */
  wl1_gwl_snapshot_type *in_snapshot, 
  /* the returned snapshot, calculated correctly to match the new time */
  wl1_gwl_snapshot_type *out_snapshot);

/*===========================================================================
FUNCTION wl1_advance_snapshot_by_w_delta

DESCRIPTION
  Returns a g/w/l snapshot that is the result of an input snapshot shifted by a
  passed in adjustment time. 

  Note gsm conversion can lose precision due to fixed point division. Therefore
  if two snapshots are needed from a starting reference snapshot, calculate 
  both using the starting reference position rather than calculating the second
  position using the first.

  It is ok to pass the same snapshot pointer as input and output.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_advance_snapshot_by_w_delta(
  /* The amount of time to move the input snapshot by, in w-frames/cx8 */
  tlm_cfn_struct_type *adj,
  /* the input snapshot */
  wl1_gwl_snapshot_type *in_snapshot, 
  /* the returned snapshot, calculated correctly to match the new time */
  wl1_gwl_snapshot_type *out_snapshot);

/*===========================================================================
FUNCTION wl1_get_irat_snapshot

DESCRIPTION
  Returns timeline values all taken at the same time for GSM, LTE and WCDMA-DL 
  timelines.  Allows for timeline conversions.  For the returned wstmr field in
  the snapshot, its value is based on the FRC combiner.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_get_irat_snapshot(
  /* the snapshots that the caller cares about (others won't be updated) */
  wl1_snapshot_selection_bmsk_type selections,
  /* the snapshot structure that will be updated by this function */
  wl1_gwl_snapshot_type *snapshot);

/*===========================================================================
 FUNCTION     WCDMA_L1_INIT_TCB

 DESCRIPTION
   This function populates the cached WCDMA L1 TCB pointer through rcinit. 

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   An assertion will fail if the lookup fails.
===========================================================================*/
extern void wcdma_l1_init_tcb(void);



/*===========================================================================
FUNCTION wsrchlte_read_dc_w2l_nv_setting

DESCRIPTION
  This function determines how often to update the tuner (when not in CM/FMO). 
  It determines this from from the relevant NV item. If that NV does not exist,
  it creates it and initializes the value to the default value.
 
RETURN VALUE
   None.

SIDE EFFECTS
  Sets global flag wl1_atuner_proc_period_ms
=============================================================================*/
extern void wl1_read_atuner_nv_settings(void);


/*=========================================================================
FUNCTION wl1_atuner_state_init

DESCRIPTION
  This function initializes wl1's antenna tuner (Atuner) processing algorithms.
  It will 
  1) Cause atuner to register status functions with any wl1 modules it
     needs to know about
  2) Setup the timers that will fire regularly for periodic atuner processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_atuner_init(void);

/*=========================================================================
FUNCTION wl1_atuner_state_deinit

DESCRIPTION
  This function deinitializes wl1's antenna tuner (Atuner) processing algorithms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_atuner_deinit(void);

/*=========================================================================
FUNCTION wl1_atuner_evt_processor

DESCRIPTION
  This function handles all known events to the Atuner system.  It's the main
  state machine and starts whatever processing is needed.

DEPENDENCIES
  MUST be called in wl1_task context

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_atuner_evt_processor(wl1_atuner_event_type evt);

/*=========================================================================
FUNCTION wl1_atuner_fmo_state_change_handler

DESCRIPTION
  This function is intended to receive updates about the FMO measurement state.
  It will send  a MEAS_PERIOD_START or MEAS_PERIOD_END event to the main state
  event processing function depending on what the FACH meas module is doing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_atuner_fmo_state_change_handler(
  /* TRUE indicates the start of a cm measurement gap pattern, or FMO gap.
     FALSE indicates the end of a cm measurement gap pattern, or FMO gap. */
  boolean meas_start);


/*===========================================================================

FUNCTION SRCH_USE_RGS_ROTATOR

DESCRIPTION:
  Returns if RGS values can be used/discarded

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean srch_use_rgs_rotator(tcxomgr_vco_indicator_type vco_type);

/*===========================================================================
FUNCTION wcdma_periodic_stat_log_init
 
DESCRIPTION This function initializes the log states for wcdma periodic stat 
            log packet
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wcdma_periodic_stat_log_init(void);

/*===========================================================================
FUNCTION wcdma_periodic_stat_log_update

DESCRIPTION
  This function is used for the  wcdma_periodic_stat logging accumulation.
  
  It is called every frame to collect the data corresponding to the sample
  
DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None
===========================================================================*/
extern void wcdma_periodic_stat_log_update(void);

/*===========================================================================
FUNCTION wcdma_periodic_stat_stop_logging

DESCRIPTION
  This function prepares the wcdma periodic stat buffer to be submitted to diag
  It calls the function that posts a wcdma periodic stat log submit local command and then
  clears the state of wcdma periodic stat logging.
  
DEPENDENCIES
  wcdma_periodic_stat_log_info_buf_idx should be valid while making
  a call to this function.

RETURN VALUE
  None

SIDE EFFECTS
  ===========================================================================*/
void wcdma_periodic_stat_stop_logging (void);

/*===========================================================================
FUNCTION wcdma_periodic_stat_send_log_submit_cmd

DESCRIPTION
  This function makes a request to post a local command to submit the 
  currently accumulated log packet to diag. Before it does so, it checks if 
  a previous log packet that was submitted to diag has been  serviced or not.
  If not, it drops the current packet and proceeds further.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wcdma_periodic_stat_send_log_submit_cmd(uint8 wcdma_periodic_stat_log_info_buf_idx);

/*===========================================================================
FUNCTION wcdma_periodic_stat_submit_log_buffer

DESCRIPTION
  This function is called to post wcdma periodic stat submit command. If the 
  buffer is not ready to be submitted to diag the function returns without
  doing anything. This can so happen because multiple logs are instructed
  to be submitted to diag by the same L1 command.
  It checks for the log code in diag and submits the packet.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Flag indicating the log buffer is submitted for flushing is reset.
===========================================================================*/
void wcdma_periodic_stat_submit_log_buffer(void);

/*===========================================================================
FUNCTION hs_send_submit_log_cmd

DESCRIPTION
  This function send HS submit log packet local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wcdma_periodic_stat_submit_log_cmd(void);

#ifdef FEATURE_DUAL_SIM
extern void wl1_backup_freq_error_at_acq_succ(void);
#endif

/*===========================================================================
FUNCTION     WL1_GET_INV_DLO_FREQ

DESCRIPTION
  This function calculates inverse downlink frequency

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern uint64 wl1_get_inv_dlo_freq(uint32 h_carrier_khz);

/*===========================================================================
FUNCTION wl1_is_gsm_clk_enabled_by_module

DESCRIPTION
  This function returns whether the gsm clocks are enabled by a module
  that is passed as a parameter

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If clocks are enabled by the module requested
  FALSE - If clocks are enabled by any other module than the requested module.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_is_gsm_clk_enabled_by_module(uint8 module_bmask);

/*===========================================================================
FUNCTION     WL1_TCXO_SET_RESTRICTION

DESCRIPTION
  Invoked due to either a restriction change from TCXO Mgr, or when error becomes
  large and UMTS is breaking the TCXO Mgr restriction or we are freezing the trk_lo
  for debugging purposes. This simply posts a local command which does all the work.
  Necessary because we cant call tcxomgr_report() from interrupt context.

DEPENDENCIES
  None.

PARAMETERS
   tcxomgr_restriction_type    restriction  - (New) restriction type.
   wl1_tcxomgr_ack_type        ack          - Indicates whether the change needs to be
                                              acked or reported.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_set_restriction(tcxomgr_restriction_type restriction,
                                     wl1_tcxomgr_ack_type ack);

/*===========================================================================
FUNCTION     WL1_TCXO_SET_RESTRICTION_CMD

DESCRIPTION
  Invoked by the local command TCXO_SET_RESTRICTION_CMD  to actually perform the
  freezing or unfreezing of the TCXO. This function needs send acks (when TCXO
  Mgr gives us a new restriction) and reports (when breaking a restriction) to
  TCXO Mgr when necessary.

DEPENDENCIES
  None.

PARAMETERS
   tcxomgr_restriction_type    restriction  - (New) restriction type.
   wl1_tcxomgr_ack_type        ack          - Indicates whether the change needs to be
                                              acked or reported.

RETURN VALUE
  None.

SIDE EFFECTS
    None.
===========================================================================*/
extern void wl1_tcxo_set_restriction_cmd(tcxomgr_restriction_type restriction,
                                         wl1_tcxomgr_ack_type ack);

/*===========================================================================
FUNCTION     WL1_TCXO_RESUME_RESTRICTION

DESCRIPTION
  Resumes current restriction after WL1 diverges from the restriction due to error
  in the VCO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_resume_restriction(void);

/*===========================================================================
FUNCTION     WL1_TCXO_RELEASE

DESCRIPTION
  Releases control of the TCXO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_release(void);

/*===========================================================================
FUNCTION     WL1_TCXO_GET_VCO

DESCRIPTION
  This function returns the vco value from the mdsp.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  tcxomgr_vco_type - returns the PDM value.

SIDE EFFECTS
  None.
===========================================================================*/
extern tcxomgr_vco_type wl1_tcxo_get_vco(void);

/*===========================================================================
FUNCTION     WL1_TRM_SET_STARTING_TRK_LO

DESCRIPTION
  This function is called after WL1 has TCXO control from TCXO Manager, and
  after WCDMA firmware has been started.  These things happen when WCDMA is
  started up.  This function then sets the starting trk_lo value in firmware.
  WL1 received this value earlier from TCXO Manager.  It is the last known
  value of trk_lo, before WCDMA was activated.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_trm_set_starting_trk_lo(void);

/*===========================================================================
FUNCTION     WL1_TCXO_FROZEN

DESCRIPTION
  Returns if the tcxo is currently frozen or not.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - TRUE if the TCXO is frozen.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_tcxo_frozen(void);

/*===========================================================================
FUNCTION     WL1_TCXO_DISABLE_PDM_FOR_SLEEP

DESCRIPTION
  Disable TRK_LO PDM via tcxomgr, if there is no restriction on the TCXO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_disable_pdm_for_sleep(void);

/*===========================================================================
FUNCTION     WL1_TCXO_ENABLE_PDM_FOR_WAKE

DESCRIPTION
  Enable TRK_LO PDM via tcxomgr.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_enable_pdm_for_wake(void);

/*===========================================================================
FUNCTION     WL1_TCXO_RPUSH

DESCRIPTION
  Performs the actual rotator push when using TCXO Mgr 3.0

DEPENDENCIES
  None.

PARAMETERS
  tcxomgr_client_state_type state    - Indicates whether WL1 is in 
                                       Idle/Traffic/OOS.

RETURN VALUE
  boolean                            - Indicates whether the Rotator push
                                       succeeded. (It can fail if there are no
                                       samples in the accumulator)

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_tcxo_rpush(tcxomgr_client_state_type state);

/*===========================================================================
FUNCTION     WL1_TCXO_RPUSH_INIT

DESCRIPTION
  Initialize RPush while L1 is entering BCH, FACH and DCH states.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_rpush_init(void);

/*===========================================================================
FUNCTION     WL1_TCXO_RPUSH_CLEANUP

DESCRIPTION
  Clean up while L1 is leaving BCH, FACH and DCH states.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1_tcxo_rpush_cleanup(void);

/*===========================================================================
FUNCTION     WL1_TCXO_REQUEST

DESCRIPTION
  This function requests TCXO Mgr for control of the VCTCXO.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - FALSE if TCXO request times out, or TCXO Mgr denies control to WCDMA.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1_tcxo_request(void);

/*===========================================================================
FUNCTION wl1_is_mode_FTM

DESCRIPTION
  This function returns whether the UE is in FTM mode (FTM_CAL/FTM_RF) or not

DEPENDENCIES
  RF

RETURN VALUE
  TRUE - If mode is FTM (FTM_CAL/FTM_RF)
  FALSE - Otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_is_mode_FTM(void);

#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
/*===========================================================================
FUNCTION wl1_read_supported_clk_speeds

DESCRIPTION
  This function reads the max supported Q6 Clock speeds based on the NPA read

DEPENDENCIES
  None

RETURN VALUE
  
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_read_supported_clk_speeds(void);
/*===========================================================================
FUNCTION wl1_get_max_supported_q6_speeds

DESCRIPTION
  This function returns the max supported Q6 clock speeds.

DEPENDENCIES
  RF

RETURN VALUE
  0/1/2

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 wl1_get_max_supported_q6_speeds(void);

/*===========================================================================
FUNCTION wcdma_timeline_profiling_log

DESCRIPTION
  This function logs f/w time-line violations.

DEPENDENCIES
Before calling the below function status dump needs to be issued to f/w

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
extern void wcdma_timeline_profiling_log(void);

#endif /*FEATURE_WCDMA_JOLOKIA_MODEM*/

extern boolean wl1_set_gsm_clk_mask_status(uint8 module_bmask);
extern boolean wl1_reset_gsm_clk_mask_status(uint8 module_bmask);

/*===========================================================================
FUNCTION wl1_update_modem_power_stats

DESCRIPTION
  This function increments the tx value bins based on the TX agc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void wl1_update_modem_power_stats(int16 tx_agc_val_dBm);

/*===========================================================================
FUNCTION wl1_qsh_cb

DESCRIPTION


DEPENDENCIES


RETURN VALUE


SIDE EFFECTS

===========================================================================*/
extern void wl1_qsh_cb(qsh_client_cb_params_s* qsh_cb_params_ptr);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_dsr_debug_dump

DESCRIPTION
  This function dumps the DSR db into wl1_debug_fet_db_info_struct_type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_dsr_debug_dump(wl1_debug_dsr_db_info_struct_type *dump_ptr);
#endif /* FEATURE_QSH_DUMP */

#if defined FEATURE_QSH_EVENT_METRIC || defined FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_qsh_cb

DESCRIPTION
  This function is called by QSH module for dump collection and metric logging

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wl1_qsh_cb(qsh_client_cb_params_s* );
/*===========================================================================
FUNCTION wl1_qsh_cb_process

DESCRIPTION
  This function is called by wl1_qsh_cb for  metric logging

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wl1_qsh_cb_process(qsh_client_cb_params_s* );
/*===========================================================================
FUNCTION wl1_qsh_init

DESCRIPTION
  This function is called by wl1m.c for QSH initialization

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wl1_qsh_init(void );

#endif

/*===========================================================================
FUNCTION wl1_slot_to_start_cm

DESCRIPTION
  This function returns the number of slots leading upto the upcoming CM gap.

DEPENDENCIES
 NONE

RETURN VALUE
 Number of slots remaining till the next CM gap

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 wl1_slot_to_start_cm (void);
#endif /* L1UTILS_H */
