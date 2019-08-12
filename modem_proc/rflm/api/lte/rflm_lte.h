/*!
  @file
  rflm_lte.h

  @brief
  RF LM API definitions for LTE to interact with SW and LM
  
  @detail
  
  
  @todo
 

*/

/*==============================================================================

  Copyright (c) 2013-16 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/lte/rflm_lte.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/03/16   as      WTR special writes support for 4FMOD performance improvement
02/04/16   am      Apply WTR PAR correction values in LTE while running Autopin
01/26/16   tk      add PA switch point hysteresis timer
01/06/16   as      Carry autopin support flag in fed static
11/19/15   as      Reduce max cell supported to 3 for Jolokia
10/30/15   kg      PAR based Auto-pin support
03/18/15   vb      Bug fix for Use source TQ for L2L tune away
02/19/15   bsh     Use source TQ for L2L tune away
01/20/15   gvn     Separate DRx scripts and don't execute during DTA
01/13/15   stw     Intraband CA PCC + SCC2 support
01/12/15   qzh     Add num_pa_range_grfc in fed to indicate the number of PA range GRFC
12/30/14    JJ     Added support for DTR rotator Tx Freq offset for Intra-band ULCA only.
12/07/14   ndb     Added FBRx CL feature
11/26/14   ndb     Added Coupler Forward/reverse Event
12/16/16   lhb     Populate script for each PA state and will be used in TX AGC
11/12/14   gvn     Increase LTE tune trigger script buffer size
10/21/14   gvn     Re-tune Tx and Rx in case split band changes on intra scell add or delete
10/27/14   pv      Added support for single TQ operation for all DL CA.
10/27/14   cri     TxAGC optimizations - ccs conv functions
10/27/14   cri     TxAGC optimizations - Interface for split txagc
10/22/14   ars     Updated RFLM_LTE_NUM_RX_PATHS to 6 to account for SCC2
10/21/14   bsh     Fix OFF target
10/13/14   bsh     MUTEX support while scheduling LNA tasks
10/01/14   ska     Adding GRFC based PA state toggle functionality
09/26/14   bsh     Support new API to check RF status
10/03/14   kab     Enable Rx Tune Scripts Execution by FW
09/30/14   gvn     3xCA gaps support
09/30/14   kab     RF-FW Interface changes for FW Triggered RF Tune
09/30/14   svi     Changes to execute Rx Tune Script as separate Preload and 
                   Trigger events
09/30/14   svi     Changes to execute Rx Tune Script as separate Preload and 
                   Trigger events
09/29/14   kab     Update DM with Rx LO spacing from Carrier Center Frequency
09/18/14   ars     Fix to use correct Rx chain mask while doing RSSI measurements for Intra band CA.
09/17/14   bsh     support rx enable/disable and delete obsolete ENUMS
09/11/14   vb      RSB/IQMC per LNA gain state support
09/09/14   ars     Moved Tx AGC override parameters to Tx static instead of using Tx dynamic data in FTM RF verification and NS  
08/21/14   svi     Added Support for Rx Config Script Execution
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
08/15/14   st      CA Alt Path Reconfiguration Support
08/15/14   cri      Added support for 16bit ICQ and DPD idx
08/14/14   pv      Added support for FBRx + GPS concurrency based scripts.
08/14/14   bsh     Support for 3DLCA
08/12/14   vb      Added Rx LNA gain states
08/13/14   jf      Reset QPOET to standby mode at DTX and PRACH
08/12/14   kab     Increase the GRFC Buffer space to 20
08/07/14    pv     Added support for TDD DRx eLNA ASM programming during Rx burst.
08/01/14   sk      Remove Common Txagc Seq creation in non-cal mode for LTE
08/01/14   asr     Added API to query for LTE DM
07/30/14    pv     Increased the RFFE Buffer space to 80 to accomodate TDD PortB settings.
07/30/14   gvn     Intra-band fixes: pass force_lna flag to fw when intra-status changes
07/18/14   jf      Return TX power in linear region to ML1
07/18/14   jf      ET Delay DM programming on SW side
07/15/14   bsh     Remove tech DM dependency for irat and rxagc
07/14/14   sbm     Added therm flag to indicate if therm read is done.
07/10/14   jf      TM8 + SRS switching for TDD
07/02/14   jf      AsDiv support
07/01/14   jf      Reduce CCS memory for AFC
06/26/14   bar     Therm read support through rflm.
06/23/14   bsh     Use events for LNA updates and other clean up
06/18/14   sbm     Added support for WTR HDET
06/18/14   php     Move CCS memory definitions into modem specific component
06/16/14   gvn     Create seperate event for GNSS blanking used for TDD
06/16/14   jf      Use CMN API to get TQ
06/13/14   kab     Add 3xCA support
06/03/14   jf      Only update DM for PCELL and TX 
05/14/14   kab     Clear AFC Gating History - During Tx_Config and Tx_Disable
05/13/14   jf      Add CL/AOL support
04/30/14    pv     Lower the FBRx Power threshold to -40dB.
04/29/14   svi     Fixed PA state Switch Points and active state update
04/28/14   kab     Update FED UL Scheduling Thread
04/24/14   jf      Change RGI timing delay offset
04/24/14   cri     wmore clipper enablement and nv support
04/17/14   svi     PA static NV and code redesign
04/15/14    pv     FBRx SM defined.
04/11/14   cri     DAC management changes for LTE TxAGC
04/10/14    pv     Added the common MACRO for FBRx to header
04/09/14   gvn     Changes to gain calculation for intra band CA
04/08/14   kab     Add support for Slot Based scheduling
04/08/14   jf      Support TCM memory reduction and variable ICQ addr for QPA
04/07/14   jf      Make sure LTE debug buffer index is maintained in the range
04/07/14   svi     Fixed RxAGC Online QRCT Logging
04/02/14   kab     DL CP Enablement
03/21/14    pv     Enabling FBRx over dynamic range for Gain-state 2.
03/21/14   cri     RGI timing delay when transitioning PA states
03/19/14   jf      ASM fix for for CELL SRS and UpPTS SFs
03/17/14   jf      UPPTS timing fix
03/19/14   jf      Separate RF event memory for TX and RX
03/19/14   bsh    NlIC changes: BOLT feature removed
03/17/14    pv     Added support for FBRx Setup/Enable/Disable RF/CCS scripts in DM
03/14/14   bsh     NlIC changes: BOLT feature added
03/11/14   bsh     Adding NLIC support
03/12/14   svi     Placed Missing "}" for extern "C" { to enable CPP inclusion
03/09/14   kab     Modify CCS memory allocation to increase Tx Off memory usage 
03/06/14    pv     Added support for Trigger power level for FBRx.
03/05/14   gvn     Change in TQP allocation and release implementation
03/04/14   kab     Add support for PRACH Tx path off Scheduling
03/04/14   cri     LTE specific CCS commit for Tx Override
02/26/14   bsh     Check previous task index status before scheduling LNA update
01/28/14   jf      Append RGI_0 script to TX_ON off script, and trigger HW write 
                   RGI_0 at tx_config
02/24/14    pv     Added support for FBRx Gain error processing with MTPL.
02/24/14   bsh     Rx PLL time adjustment for Rx ON
02/18/14    pv     Added support for FBRx Gain error processing with Common FBRx Trigger.
02/18/14   bsh     Flag to check the status of rx config and F3 to print LNA enqueue time
02/14/14   svi     Removed usaged of "on_off_event_counter" for 
                   indexing the debug buffer
02/13/14   bsh     Updated ENUM for LNA script size
02/13/14    pv     Added Support for FBRx trigger for MTPL.
02/12/14   jf      Add xpt logging for ENH TXAGC
02/05/14   kab     Support for AFC Issue Seq Gating
02/05/14   cri     Freq comp optimization for DM population
02/05/14   kab     Reduce memory used for Debug logging
02/05/14   kab     Insert padding memory between RF Scripts in DM
01/31/14   kab     LTE Timing Fixes
01/30/14   jf      Reorganize CCS Memory
01/30/14   kab     AFC RF Event and AFC Gating
01/28/14  svi/kab  RFLM LTE TxAGC Log Packets addition
01/24/14   cri     DRIF/IREF/LUT enablement
01/23/14   bsh     Corrected the CCS memory write location and offset calculator for Rx
01/21/14   bsh     Use the global ccs mem offset ptr instead of one from DM
01/22/14   jf      Fix comment
01/18/14   gvn     Interface change to store tq_ptr in handle_pool
01/08/14   jf      CCS memory re-organization for additional ASM writes
01/06/14   bsh     RxAGC support for BOLT
01/06/14   gvn     Update timing info for LTE
12/18/13   svi     CCS Memory Management Rearchitecture
12/17/13   svi     Merge Tx and Rx RF event creation wrapper
                   Both types of events to use RF event CCS Memory
                   Created new events for Rx EN High and Rx EN Low
12/13/13   svi     TxAGC Wrapper rearchitecture
12/10/13   svi     RFLM FED TxAGC Logging checkin 2 : Added freq hopping
12/10/13   svi     Removing RFLM FED TxAGC Logging from this file
12/10/13   svi     RFLM FED TxAGC Logging checkin 1 : 
                   removed unnecessary logging
12/10/13   svi     RFLM FED TxAGC Logging checkin 1
12/09/13   can     Separate RF CCS Memory from TX Events
12/03/13   pl      Remove unused IRAT CCS EVENT enum
12/02/13   cri     Support for RFLM Tx override for LTE
11/27/13   pl      Move RX events into new rx specific enum type
11/25/13   svi     Reduced SF turnaround time to 150us
11/25/13   Saul    CCS. New CSS Data Buffer Allocation For All Techs.
11/18/13   cri     RFLM based Tx override hookup for LTE
11/15/13   jf      AFC fix
11/15/13   npi     Define PCELL IDX to remove non RFLM external dependencies
11/14/13   npi     Changes to track TQ allocation for each cell in CA
11/14/13   cri     Enable digital settings and ET DAC
11/08/13   jf      Release TQ at exit mode
11/08/13   svi     FED Rx Dynamic DM Clean Up - 1
11/06/13   svi     Added FED Rx Dynamic DM
10/30/13   svi     Added Buffers for TxPLL and RxPLL AFC DM
10/28/13/  pl      Added LTE IRAT events
10/24/13   cri     Initial programming of digital settings. Iref and IQ gain.
10/09/13   nrk     Added initial DM population changes
10/08/13   jf      Add addtional WTR header to support WXE on different slave id 
10/04/13   jf      Add timing profile
09/30/13   jf      Fix EXT RFFE ccs convert API
09/27/13   jf      Add TDD ASM support (RFFE only)
09/27/13   jf      Change API input structure to re-use CCS convert function
09/23/13   jf      Add script_execute support
09/04/13   ra      Support interface to CCS_RF_INTF
08/30/13   jf      Add more CCS scripts
08/22/13   jf      Add CCS scripts     
08/20/13   jf      Add FBRX support 
07/30/13   jf      Initial Version
==============================================================================*/

#ifndef RFLM_LTE_SW_INTF_H
#define RFLM_LTE_SW_INTF_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_lte.h"
#include "rflm_txagc_api.h"
#include "rflm_rxagc_api.h"
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rflm_time_profile.h"
#include "rflm_api_fbrx.h"
#include <qurt.h>
#include "rflm_thread.h"
#include "rflm_cmn_intf.h"
#include "lm_types.h"

#ifdef FEATURE_BOLT_MODEM
#include "rflm_nlic_typedef_ag.h"
#endif 
/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES -- Fix Numbers

==============================================================================*/

/*! Define the PCELL IDX - required to be in sync with the RFLTE_MC_PCELL_IDX definition 
*/
#define RFLM_LTE_MC_PCELL_IDX                             RFLM_LTE_MC_CELL0

#define RFLM_LTE_MC_SCELL_1_IDX                             	RFLM_LTE_MC_CELL1


/*! Max cells supported - required to be in sync with the *rflte_mc_cell_idx_type* 
      maximum value of RFLTE_MC_MAX_CELL_IDX_SUPPORTED
*/
#define RFLM_LTE_MC_MAX_CELL_IDX_SUPPORTED               3

/*! Time interval for FBRX reads 
*/
#define RFLM_LTE_DEFAULT_FBRX_TIMER                       154000 // 5ms

/*! Minimum time needed for FED API to be called 
*/
#define RFLM_LTE_TX_SF_UPDATE_MIN_TURN_AROUND_TIME        4650 // 4650 ustmr counts ~= 242us

/*! Trigger Time for Tuner AOL event from SLOT 1 action time 
*/
#define RFLM_LTE_TX_AOL_SLOT_1_AFFIXED_TIME        9600 // 9600 ustmr counts = 500us
/*! Minimum time needed for FED API to be called 
*/
#define RFLM_LTE_TX_SCHEDULE_OUTSTANDING_TASKS_WAKEUP_AFFIXED_TIME        960 //50us

#define RFLM_LTE_TX_SCHEDULE_PRACH_OFF_WAKEUP_AFFIXED_TIME        -9600 //500us

#define RFLM_LTE_ONE_MS_IN_USTMR_COUNTS        19200 //1 ms

#define RFLM_LTE_ONE_P_SEVEN_MS_IN_USTMR_COUNTS  1.7 * RFLM_LTE_ONE_MS_IN_USTMR_COUNTS

/* RFLM_SLOT_SCHEDULE Thread parameters */
/*  Priority. This has to be the same as FW UL Thread. */
#define RFLM_SLOT_SCHEDULE_THREAD_PRI           25

/*  Stack Size. This is same as RFLM_DIAG_LOG stack size.                  *
 *  Need to reduce after Issue Seq optimization(Seq create once) is done   */
#define RFLM_SLOT_SCHEDULE_THREAD_STACK_SZ        RFLM_DEFAULT_STACK_SZ

#define RFLM_SLOT_SCHEDULE_NUM_CONTEXT            5

#define RFLM_SLOT_SCHEDULE_SIG_SHUTDOWN_BIT      RFLM_SLOT_SCHEDULE_NUM_CONTEXT

#define RFLM_SLOT_SCHEDULE_SIG_LAST          RFLM_SLOT_SCHEDULE_SIG_SHUTDOWN_BIT
                                        // Last Signal.

// Max # of signals & the corresponding signal mask
#define RFLM_SLOT_SCHEDULE_MAX_SIGNALS           (RFLM_SLOT_SCHEDULE_SIG_LAST + 1)
#define RFLM_SLOT_SCHEDULE_SIG_MASK              (BITL(RFLM_SLOT_SCHEDULE_MAX_SIGNALS) - 1)

/*! Max number of transactions for SSBI/RFFE/GRFC buffer
*/
#define RFLM_LTE_GRFC_TRANS_MAX  20         /*Was 20 earlier, changed during memory reduction*/
#define RFLM_LTE_SSBI_TRANS_MAX  5         /*Was 40 earlier, changed during memory reduction*/
#define RFLM_LTE_RFFE_TRANS_MAX  80
#define RFLM_LTE_PLL_TRANS_MAX   10
#define RFLM_MEAS_MAX_LTE_TRIG_TRANS 9
#define RFLM_LTE_PLL_SETTLING_TIME        150

/*! Max number of LNA Gain word (GW) SSBI to program to tune to a Carrier. 
    This is to incorporate LNA + PLL split for CA on WTR1625 + WFR1620 */
#define RFLM_LTE_MAX_LNA_GW_SSBI_PER_PATH 2


#define RFLM_LTE_CCS_MEM_PER_LNA_UPDATE             128
#define RFLM_LTE_CCS_EVENT_MEM_PER_LNA_UPDATE       8


#define RFLM_LTE_THERM_WR_MAX_INIT_SIZE 60
#define RFLM_LTE_THERM_WR_MAX_CONV_DISABLE_SIZE 60
#define RFLM_LTE_TDET_TEMP_COMP_SIZE 70
#define RFLM_LTE_THERM_RD_SIZE 60
#define RFLM_LTE_RX_ON_WR_SIZE 20
#define RFLM_LTE_TX_ON_WR_SIZE 20
#define RFLM_LTE_GNSS_WR_SIZE 20
#define RFLM_LTE_AFC_UPDATE_SIZE 35
#define RFLM_LTE_FBRX_LDO_SETUP_SIZE 20
#define RFLM_LTE_FBRX_LDO_DISABLE_SIZE 20
#define RFLM_LTE_TX_EN_WR_SIZE 20
#define RFLM_LTE_SMPS_EN_WR_SIZE 20
#define RFLM_LTE_ASM_RX_WR_SIZE 35
#define RFLM_LTE_ASM_TX_WR_SIZE 35
#define RFLM_LTE_RX_EN_WR_SIZE 20
#define RFLM_LTE_RX_EN_RX_ON_WR_SIZE 30
#define RFLM_LTE_TX_EN_TX_ON_WR_SIZE 30
#define RFLM_LTE_PAPM_ON_SIZE 60
#define RFLM_LTE_PAPM_STANDBY_SIZE 60
#define RFLM_LTE_HDET_RD_RESULT_NUM_TRANS 8
#define RFLM_LTE_THERM_RD_RESULT_NUM_TRANS 8
#define RFLM_LTE_MAX_NUM_SINGLE_EVENT_SCRIPT   20
#define RFLM_LTE_MAX_NUM_SINGLE_TASK_SCRIPT 10
#define RFLM_LTE_SINGLE_EVENT_WR_SIZE 20
#define RFLM_LTE_ADC_HDET_ENABLE_SIZE 80
#define RFLM_LTE_CA_RECONFIG_SIZE 60
#define RFLM_LTE_ADC_HDET_PAR_ENABLE_SIZE 120
#define RFLM_LTE_FBRX_CONFIG_COUPLER_SIZE 20

#define RFLM_LTE_NUM_RX_PATHS 6
#define RFLM_LTE_ASDIV_BUF_SIZE 80

#define RFLM_LTE_WRITE 0
#define RFLM_LTE_READ  1

/* This macro is the rolling buffer size for RFLM FED Txagc Logging */
#define RFLM_LTE_DEBUG_LM_BUFFER_SIZE 10

/* This macro is the Number of Tx ON OFF events for FDD
Tx ON --> Tx ON and Tx EN HIGH 
TX OFF --> TX OFF and TX EN LOW */
#define RFLM_LTE_DEBUG_NUM_TX_ON_OFF_EVENTS_IN_FDD 2

/* This macro is the Number of Tx ON OFF events for TDD
Tx ON --> Tx ON only 
TX OFF --> TX OFF off */
#define RFLM_LTE_DEBUG_NUM_TX_ON_OFF_EVENTS_IN_TDD 1

/* This macro helps control the total number of ASM allowed per Path on FE */
#define RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED                3

/* This macro helps control the total allowed unique settings per PATH on each FE */
#define RFLM_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH             2

#define RFLM_LTE_MAX_PA_SCRIPTS_SUPPORTED_PER_PATH              2

/* Number of buffers for dynamic data */
#define RFLM_LTE_NUM_DYNAMIC_BUFF                               2

/*--------------------------------------------------------------------------*/
/* PA State Definitions */
/*--------------------------------------------------------------------------*/

/*!>
  @brief
  This module contains PA State Macro Definitons :
  @{
*/

/*--------------------------------------------------------------------------*/
/*!>
  @brief
  This Macro defines the number of Max PA ranges in the new NV desgin
  
*/
#define RFLM_LTE_MAX_NUM_PA_STATES RFLM_TXAGC_PA_STATE_MAX
/*!> 8 */

/*--------------------------------------------------------------------------*/
/*!>
  @brief
  This Macro defines the number of Max PA ranges in the legacy NV desgin
  
*/
#define RFLM_LTE_LEGACY_MAX_NUM_PA_STATES 4
/*!> 4 */

/*--------------------------------------------------------------------------*/

/*! @} */


/* -------------------------------- CCS related --------------------------------------*/
/* Make the # even so that data are 32bit aligned to avoid issue_seq generation issue */
#define RFLM_LTE_CCS_TX_CONT_MAX_TRANS 40
#define RFLM_LTE_CCS_TX_CONT_MAX_TASK 20
#define RFLM_LTE_CCS_TX_OFF_MAX_TRANS 28
#define RFLM_LTE_CCS_TX_OFF_MAX_TASK 16
#define RFLM_LTE_CCS_TXAGC_NUM_BUF 4
#define RFLM_LTE_CCS_RXAGC_MAX_TRANS 16
#define RFLM_LTE_CCS_RXAGC_MAX_TASK 8
#define RFLM_LTE_CCS_TXAFC_MAX_TRANS 10
#define RFLM_LTE_CCS_RXAFC_MAX_TRANS 10
#define RFLM_LTE_CCS_SINGLE_TASK_MAX_TRANS 5
#define RFLM_LTE_CCS_ANT_SW_MAX_TASK 8
#define RFLM_LTE_TX_SSBI_MAX_TRANS   8
#define RFLM_LTE_RX_SSBI_MAX_TRANS   8
#define RFLM_LTE_TX_GRFC_MAX_TRANS   8
#define RFLM_LTE_TX_RFFE_MAX_TRANS   8
#define RFLM_LTE_RX_RFFE_MAX_TRANS   8
#define RFLM_LTE_CCS_NUM_DATA_PER_EXT_RFFE  4

/* Convert delta time in MSTMR (30.72MHz) to USTMR (19.2MHz) */
#define RFLM_LTE_CCS_USTME_CONV_MUL 5
#define RFLM_LTE_CCS_USTME_CONV_SHIFT 3

#define RFLM_LTE_TIMED     0
#define RFLM_LTE_IMMEDIATE 1

#define RFLM_LTE_RFFE_WRITE_DURATION               -19
#define RFLM_LTE_RFFE_EXTRA_PA_DELAY                 0 
#define RFLM_LTE_RFFE_WRITE_DELAY_0x0                0 
#define RFLM_LTE_RFFE_WRITE_DELAY_0x1_0x20           0
#define RFLM_LTE_RFFE_WRITE_DELAY_0x20               0

#define RFLM_LTE_GRFC_EXTRA_DELAY                    0 

#define RFLM_LTE_RFFE_NON_TRIGGER_SCRIPT_DELAY       0 
#define RFLM_LTE_WTR_NON_TRIGGER_SCRIPT_DELAY        0 
#define RFLM_LTE_WXE_NON_TRIGGER_SCRIPT_DELAY        0 

/*! @brief Size of each TQ data buffer. The common size is for two TQs. Devided by 2 to get size per TQ  */
#define RFLM_LTE_CCS_AUX_DATA_BUFFER_SIZE      (uint32)( CCS_RF_TASKQ_PAIRS_DATA_BUF_NUM_WORDS / 2 )
/*! @brief CCS RF auxiliary buffer idx, 
    - buffer idx (0-5) for two pair of TQ plus IRAT 
    - buffer idx (6) for RFSW TQ
    - buffer idx (7) for auxiliary buffer */
#define RFLM_LTE_CCS_RF_AUX_BUF_IDX                  7

/* Hal Buffer Sizes for Rx Tune Scripts */
#define RFLM_LTE_CCS_RX_TUNE_NON_TRX_MAX_SBI   0
#define RFLM_LTE_CCS_RX_TUNE_NON_TRX_MAX_RFFE  50
#define RFLM_LTE_CCS_RX_TUNE_NON_TRX_MAX_GRFC  20

#define RFLM_LTE_CCS_RX_TUNE_PRELOAD_TRX_MAX_SBI       0
#define RFLM_LTE_CCS_RX_TUNE_PRELOAD_TRX_MAX_RFFE      300
#define RFLM_LTE_CCS_RX_TUNE_PRELOAD_TRX_MAX_GRFC      0

#define RFLM_LTE_CCS_RX_TUNE_TRIGGER_TRX_MAX_SBI       0
#define RFLM_LTE_CCS_RX_TUNE_TRIGGER_TRX_MAX_RFFE      40
#define RFLM_LTE_CCS_RX_TUNE_TRIGGER_TRX_MAX_GRFC      0

#define RFLM_LTE_CCS_INVALID_RF_TASK_CAL_BACK_API NULL

/* Therm Max Transactions */
#define RFLM_LTE_THERM_MAX_SSBI_TRANSACTIONS        60
#define RFLM_LTE_THERM_MAX_RFFE_TRANSACTIONS        40
#define RFLM_LTE_THERM_MAX_GRFC_TRANSACTIONS         0

#define RFLTE_LM_MAX_USTMR_COUNT (1<<USTMR_COUNT_BITS)-1

/* Dime partition */
//#define RFLM_LTE_CCS_TX_MEM_SIZE                  1224
//#define RFLM_LTE_CCS_TX_MEM_OFFSET                   0
//#define RFLM_LTE_CCS_RX_MEM_SIZE                   150
//#define RFLM_LTE_CCS_RX_MEM_OFFSET                1224
//#define RFLM_LTE_CCS_RF_SCRIPT_MEM_SIZE            674
//#define RFLM_LTE_CCS_RF_SCRIPT_MEM_OFFSET         1374


#define RFLTE_DM_RX_DEFAULT_CELL_ID 0
#define RFLM_DM_RX_INTRA_BAND_CELL_ID 1
#define RFLM_DM_RX_ALT_PATH_CELL_ID 2  
#define RFLM_DM_RX_DEFAULT_NUM_CELLS 1

/* FBRx Default Trigger level, currently upto gain-state 2 dynamic range */
#define RFLTE_DM_FBRX_TRIGGER_PWR_LVL_DB10             -400
#define RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES             1
#define RFLTE_DM_FBRX_SUPPORTED_TX_DIVICE_INDEX        0
/* This is macro used for identifying how many unique FBRx
   scripts per/ transceiver can be supported by TxAGC */
#define RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES             2

#define RFLM_LTE_ALL_CHAIN_ENABLED_MASK             0x3F        /*Both PCELL and SCELLs enabled*/
#define RFLM_LTE_CA_CHAIN_ENABLED_MASK              0x0F        /*Intra Band is enabled (Both PCELL + SCELL1)*/
#define RFLM_LTE_PCELL_CHAIN_ENABLED_MASK           0x03        /*Only PCELL enabled*/
#define RFLM_LTE_SCELL1_CHAIN_ENABLED_MASK          0x0C        /*Only SCELL1 enabled*/
#define RFLM_LTE_SCELL2_CHAIN_ENABLED_MASK          0x30        /*Only SCELL2 enabled*/

#define RFLM_LTE_PRX_BIT_MASK                 0x01
#define RFLM_LTE_DRX_BIT_MASK                 0x02

#define RFLM_LTE_PRX_PATH                     0
#define RFLM_LTE_DRX_PATH                     1

/* FBRX Symbol level trigger macro 
   LTE sub-frame has 13 potential symbols as mentioned below:
   Slot0 - 0-6, Slot1 - 7 - 12, SRS - 13 (only). Each symbol is 70 us long */
#define RFLM_LTE_FBRX_NO_TRIGGER               0
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_0   1
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_1   2
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_2   4
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_3   8
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_4   16
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_5   32
#define RFLM_LTE_SLOT1_FBRX_TRIGGER_SYMBOL_6   64
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_7   128
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_8   256
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_9   512
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_10 1024
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_11 2048
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_12 4096
#define RFLM_LTE_SLOT2_FBRX_TRIGGER_SYMBOL_13 8192

/* For UPPTS SF we use 12th symbol as SRS 1*/
#define RFLM_LTE_SRS1_FBRX_TRIGGER_SYMBOL_12  4096
#define RFLM_LTE_SRS1_FBRX_TRIGGER_SYMBOL_13  8192
#define RFLM_LTE_SRS2_FBRX_TRIGGER_SYMBOL_13  8192

/* TX power range: Lowest = -70dBm */
#define RFLM_LTE_LOWEST_TX_POWER_DB10         -700
#define RFLM_LTE_CCS_IRAT_MEM_OFFSET 0
/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES -- ENUMs

==============================================================================*/
typedef enum
{
  RFLM_LTE_INVALID_DATA,
  RFLM_LTE_FED_STATIC_DATA,
  RFLM_LTE_TX_DYNAMIC_DATA,
  RFLM_LTE_RX_DYNAMIC_DATA,
  RFLM_LTE_FED2SW_DATA
}rflm_lte_data_type_e;

typedef enum
{
  RFLM_LTE_AFC_BUFFER_0,
  RFLM_LTE_AFC_BUFFER_1
}rflm_lte_afc_buffer_e;

typedef enum
{
  RFLM_LTE_POWER_CTL_MODE_FBRX,
  RFLM_LTE_POWER_CTL_MODE_HKADC,
  RFLM_LTE_POWER_CTL_MODE_NONE
}rflm_lte_tx_pwr_ctrl_type;

/* The list of the enums for non-time critical (i.e. non issue seq) CCS events */
typedef enum
{
  RFLM_LTE_THERM_WR_CCS_INIT,
  RFLM_LTE_THERM_WR_CCS_CONV_DISABLE,
  RFLM_LTE_THERM_RD_CCS,
  RFLM_LTE_TX_ON_WR_ON_CCS,
  RFLM_LTE_TX_ON_WR_OFF_CCS,
  RFLM_LTE_TX_EN_WR_ON_CCS,
  RFLM_LTE_TX_EN_WR_OFF_CCS,
  RFLM_LTE_SMPS_EN_WR_ON_CCS,       /* obsolete */
  RFLM_LTE_SMPS_EN_WR_OFF_CCS,      /* obsolete */
  RFLM_LTE_TDD_ASM_RX_WR_CCS,       /* obsolete */
  RFLM_LTE_TDD_ASM_TX_WR_CCS,       /* obsolete */
  RFLM_LTE_TX_EN_TX_ON_WR_ON_CCS,
  RFLM_LTE_TX_EN_TX_ON_WR_OFF_CCS,
  RFLM_LTE_PAPM_ON_CCS,
  RFLM_LTE_PAPM_STANDBY_CCS,
  RFLM_LTE_TDET_TEMP_COMP_CCS,
  RFLM_LTE_AFC_UPDATE_BUF_CCS,
  RFLM_LTE_LDO_SETUP_CCS,
  RFLM_LTE_LDO_DISABLE_CCS,
  RFLM_LTE_LDO_ALWAYS_ON_CCS,
  RFLM_LTE_CONFIG_COUPLER_FWD_CCS,
  RFLM_LTE_CONFIG_COUPLER_REV_CCS,
  RFLM_LTE_AOL_RF_EVENT_CCS,
  RFLM_LTE_GNSS_BLANKING_CCS,
  RFLM_LTE_FBRX_ADC_HDET_ENABLE_CCS,
  RFLM_LTE_HDET_PAR_ENABLE_GS1_CCS,
  RFLM_LTE_HDET_PAR_ENABLE_GS2_CCS,
  RFLM_LTE_CA_RECONF_CCS,
  RFLM_LTE_CSS_EVENT_NUM
}rflm_lte_ccs_event_type_e;

/* The list of the enums for non-time critical (i.e. non issue seq) CCS events */
typedef enum
{

  RFLM_LTE_RX_ON_WR_ON_CCS,    
  RFLM_LTE_RX_ON_WR_OFF_CCS,  
   
  RFLM_LTE_RX_EN_WR_ON_CCS,    
  RFLM_LTE_RX_EN_WR_OFF_CCS,   

  RFLM_LTE_IRAT_PRELOAD_CCS,
  RFLM_LTE_IRAT_TRIGGER_CCS,
  RFLM_LTE_IRAT_NON_TRX_CCS,
  RFLM_LTE_IRAT_RESET_CCS,
  RFLM_LTE_RX_LNA_SCRIPT_CCS,
  RFLM_LTE_RX_LNA_G0_CCS,
  RFLM_LTE_RX_LNA_G1_CCS,
  RFLM_LTE_RX_LNA_G2_CCS,
  RFLM_LTE_RX_LNA_G3_CCS,
  RFLM_LTE_RX_LNA_G4_CCS,
  RFLM_LTE_RX_LNA_G5_CCS,
  RFLM_LTE_RX_TUNE_PRELOAD_TRX_CCS,  
  RFLM_LTE_RX_TUNE_TRIGGER_TRX_CCS,    
  RFLM_LTE_RX_TUNE_NON_TRX_CCS,  
  RFLM_LTE_RX_CCS_EVENT_NUM
}rflm_lte_rx_ccs_event_type_e;

typedef enum 
{
  RFLM_LTE_RX_LNA_G0 = 0,
  RFLM_LTE_RX_LNA_G1,
  RFLM_LTE_RX_LNA_G2,
  RFLM_LTE_RX_LNA_G3,
  RFLM_LTE_RX_LNA_G4,
  RFLM_LTE_RX_LNA_G5
}rflm_lte_rx_lna_state_type_e;

typedef enum 
{
  RFLM_LTE_RF_EVENT_GENERAL = 0,
  RFLM_LTE_RF_EVENT_TX_AFC,
  RFLM_LTE_RF_EVENT_TX_ON,
  RFLM_LTE_RF_EVENT_TX_OFF,
  RFLM_LTE_RF_EVENT_TX_EN_HIGH,
  RFLM_LTE_RF_EVENT_TX_EN_LOW,
  RFLM_LTE_RF_EVENT_RX_ON,
  RFLM_LTE_RF_EVENT_RX_OFF,
  RFLM_LTE_RF_EVENT_THERM_MEAS,
  RFLM_LTE_RF_EVENT_THERM_READ,
  RFLM_LTE_RF_EVENT_TDET_TEMP_COMP,
  RFLM_LTE_RF_EVENT_RX_EN_HIGH,
  RFLM_LTE_RF_EVENT_RX_EN_LOW,
  RFLM_LTE_RF_EVENT_FBRX_LDO_SETUP,
  RFLM_LTE_RF_EVENT_FBRX_LDO_DISABLE,
  RFLM_LTE_RF_EVENT_FBRX_LDO_ALWAYS_ON,
  RFLM_LTE_RF_EVENT_GNSS_BLANK,
  RFLM_LTE_RF_EVENT_FBRX_ADC_HDET_ENABLE,
  RFLM_LTE_RF_EVENT_HDET_PAR_ENABLE_GS1,
  RFLM_LTE_RF_EVENT_HDET_PAR_ENABLE_GS2,
  RFLM_LTE_RF_EVENT_CA_RECONFG,
  RFLM_LTE_RF_EVENT_QPOET_STANDBY,
  RFLM_LTE_RF_EVENT_FBRX_CONFIG_COUPLER_FWD,
  RFLM_LTE_RF_EVENT_FBRX_CONFIG_COUPLER_REV,
  RFLM_LTE_RF_NUM_EVENT
}rflm_lte_rf_event_type_e;

/* The list of the enums for time critical (i.e. issue seq) CCS events */
typedef enum 
{
  RFLM_LTE_RF_SCRIPT_PA_0,                /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_1,                /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_2,                /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_3,                /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_ON,               /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_OFF,              /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_BOOST,            /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_TRIGGER,          /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_TX_ON_OFF,           /* TDD */
  RFLM_LTE_RF_SCRIPT_TX_EN_OFF,           /* TDD */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_TX,          /* TDD RFFE Tx */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_PRX,          /* TDD RFFE Rx */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_DRX,         /* TDD RFFE Rx */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_TX_TRIGGER,  /* TDD RFFE Tx */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_PRX_TRIGGER,  /* TDD RFFE Rx */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_DRX_TRIGGER, /* TDD RFFE DRx */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_TX_GRFC,     /* TDD GRFC */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_PRX_GRFC,     /* TDD GRFC */
  RFLM_LTE_RF_SCRIPT_TDD_ASM_DRX_GRFC,     /* TDD GRFC */
  RFLM_LTE_RF_SCRIPT_TXPLL_AFC,           /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PAPM_OFF,            /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PAPM_TRIGGER,        /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX0,           /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX1,           /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_EN_ANT_S,            /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_DIS_ANT_S,           /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX0_GRFC_BUF0, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX1_GRFC_BUF0, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX0_RFFE_BUF0, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX1_RFFE_BUF0, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX0_GRFC_BUF1, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX1_GRFC_BUF1, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX0_RFFE_BUF1, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_ANT_S_TX1_RFFE_BUF1, /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_FBRX_ENABLE,         /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_PA_4,                /* FDD and TDD */  
  RFLM_LTE_RF_SCRIPT_PA_5,                /* FDD and TDD */    
  RFLM_LTE_RF_SCRIPT_PA_6,                /* FDD and TDD */      
  RFLM_LTE_RF_SCRIPT_PA_7,                /* FDD and TDD */    
  RFLM_LTE_RF_SCRIPT_WTR_4FMOD_LHS,       /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_WTR_4FMOD_RHS,       /* FDD and TDD */
  RFLM_LTE_RF_SCRIPT_WTR_4FMOD_DEFAULT,   /* FDD and TDD */  
  RFLM_LTE_RF_SCRIPT_EVENT_MAX
}rflm_lte_rf_script_id_type_e;

typedef enum 
{
  RFLM_LTE_RF_INVALID,
  RFLM_LTE_RF_GRFC,
  RFLM_LTE_RF_SSBI,
  RFLM_LTE_RF_RFFE,
  RFLM_LTE_RF_RFFE_EXT,
  RFLM_LTE_RF_TYPE_MAX
}rflm_lte_rf_trans_type_e;

typedef enum
{
  RFLM_LTE_CCS_MEM_ALLOC_TX=0,
  RFLM_LTE_CCS_MEM_ALLOC_RX,
  RFLM_LTE_CCS_MEM_ALLOC_RF_SCRIPT,
  RFLM_LTE_CCS_MEM_ALLOC_TX_AUX,
  RFLM_LTE_CCS_MEM_ALLOC_RF_EVENT,
  RFLM_LTE_CCS_MEM_ALLOC_TX_RF_EVENT,
  RFLM_LTE_CCS_MEM_ALLOC_RX_RF_EVENT,
  RFLM_LTE_CCS_MEM_ALLOC_RX_CONFIG_EVENT,  
  RFLM_LTE_CCS_MEM_ALLOC_COUNT
}rflm_lte_ccs_mem_alloc_e;

typedef enum
{
  RFLM_LTE_THERM_READY,
  RFLM_LTE_THERM_READ_DONE,
  RFLM_LTE_THERM_MEAS_DONE
}rflm_lte_therm_status_e;

typedef enum
{
  RFLM_LTE_UPDATE_TX_SF_TIME,
  RFLM_LTE_TX_PATH_CTRL_TIME,
  RFLM_LTE_TXAGC_UPDATE_TIME,
  RFLM_LTE_COMMIT_ISS_SEQ_TIME,
  RFLM_LTE_COMMIT_SINGLE_TASK_TIME,
  RFLM_LTE_COMMIT_RF_EVENT_TIME,
  RFLM_LTE_RXAGC_UPDATE_TIME,
  RFLM_LTE_MAX_TIMING_PROFILE_MAX
}rflm_lte_time_logging_e;

typedef enum
{
  RFLM_LTE_CALL_BACK_INTERRUPT_DISABLED  = 0,
  RFLM_LTE_CALL_BACK_INTERRUPT_ENABLED   = 1,
  RFLM_LTE_CALL_BACK_INTERRUPT_INVALID,
  RFLM_LTE_CALL_BACK_INTERRUPT_MAX = RFLM_LTE_CALL_BACK_INTERRUPT_INVALID
}rflm_lte_ccs_cb_int_enable_e;

typedef enum
{
  RFLM_LTE_SET_TX_OFF  = 0,
  RFLM_LTE_SET_TX_ON   = 1,
  RFLM_LTE_INVALID_TX_ACTION,
  RFLM_LTE_MAX_TX_ACTION = RFLM_LTE_INVALID_TX_ACTION
}rflm_lte_tx_on_off_action_type_e;

typedef enum
{
  RFLM_LTE_TXAGC_TASK,
  RFLM_LTE_TX_PATH_OFF_TASK,
  RFLM_LTE_TXPLL_AFC_TASK,
  RFLM_LTE_INVALID_TASK,
  RFLM_LTE_MAX_TASK = RFLM_LTE_INVALID_TASK
}rflm_lte_issue_seq_ccs_task_ret_type_e;

typedef enum 
{
  /*!> NO EVENT */
  RFLM_LTE_DEBUG_TX_NO_EVENT = -1, 
  
  /*!> Tx Related EVENTs */  
  RFLM_LTE_DEBUG_TX_ON_RF_EVENT,                      /* TX PATH */
  RFLM_LTE_DEBUG_TX_OFF_RF_EVENT,                     /* TX PATH */
  RFLM_LTE_DEBUG_TX_EN_HIGH_RF_EVENT,                 /* TX PATH */
  RFLM_LTE_DEBUG_TX_EN_LOW_RF_EVENT,                  /* TX PATH */
  RFLM_LTE_DEBUG_TXAGX_ISSUE_SEQ,                     /* TXAGC */
  RFLM_LTE_DEBUG_TX_PATH_OFF_ISSUE_SEQ,               /* TX_PATH_OFF */
  RFLM_LTE_DEBUG_PA_ON_ISSUE_SEQ,                     /* PA ON OFF */
  RFLM_LTE_DEBUG_PA_OFF_ISSUE_SEQ,                    /* PA ON OFF */
  RFLM_LTE_DEBUG_PA_ON_TRIGGER_ISSUE_SEQ,             /* PA ON OFF */
  RFLM_LTE_DEBUG_PA_OFF_TRIGGER_ISSUE_SEQ,            /* PA ON OFF */
  RFLM_LTE_DEBUG_ASM_TX_TO_RX_ISSUE_SEQ,              /* ASM */
  RFLM_LTE_DEBUG_ASM_RX_TO_TX_ISSUE_SEQ,              /* ASM */
  RFLM_LTE_DEBUG_ASM_TX_TO_RX_TRIGGER_ISSUE_SEQ,      /* ASM */
  RFLM_LTE_DEBUG_ASM_RX_TO_TX_TRIGGER_ISSUE_SEQ,      /* ASM */
  RFLM_LTE_DEBUG_ASM_AUX_TX_TO_RX_ISSUE_SEQ,          /* ASM AUX */
  RFLM_LTE_DEBUG_ASM_AUX_RX_TO_TX_ISSUE_SEQ,          /* ASM AUX */
  RFLM_LTE_DEBUG_ASM_AUX_TX_TO_RX_TRIGGER_ISSUE_SEQ,  /* ASM AUX */
  RFLM_LTE_DEBUG_ASM_AUX_RX_TO_TX_TRIGGER_ISSUE_SEQ,  /* ASM AUX */
  RFLM_LTE_DEBUG_ANT_SWAP_TX_0_LO_ISSUE_SEQ,          /* ANT SWAP */
  RFLM_LTE_DEBUG_ANT_SWAP_TX_0_HI_ISSUE_SEQ,          /* ANT SWAP */
  RFLM_LTE_DEBUG_ANT_SWAP_TX_1_LO_ISSUE_SEQ,          /* ANT SWAP */
  RFLM_LTE_DEBUG_ANT_SWAP_TX_1_HI_ISSUE_SEQ,          /* ANT SWAP */
  RFLM_LTE_DEBUG_QPOET_ISSUE_SEQ,                     /* QPOET */
  RFLM_LTE_DEBUG_QPOET_TRIGGER_ISSUE_SEQ,             /* QPOET */
  RFLM_LTE_DEBUG_WXE_ISSUE_SEQ,                       /* WXE */
  RFLM_LTE_DEBUG_WTR_ISSUE_SEQ,                       /* WTR */
  RFLM_LTE_DEBUG_WTR_TRIGGER_ISSUE_SEQ,               /* WTR */
  RFLM_LTE_DEBUG_AFC_ISSUE_SEQ,                       /* AFC */
  RFLM_LTE_DEBUG_RGI0_WXE_ISSUE_SEQ,                  /* RGI0_WXE */
  RFLM_LTE_DEBUG_RGI0_WTR_ISSUE_SEQ,                  /* RGI0_WTR */
  RFLM_LTE_DEBUG_TX_MAX_EVENTS,                       /* Max Number of Tx Debug Events */

}rflm_lte_debug_tx_task_t; 

typedef enum 
{
  /*!> NO EVENT */
  RFLM_LTE_DEBUG_RX_NO_EVENT = -1, 

  /*!> Rx Related EVENTs */    
  RFLM_LTE_DEBUG_RX_ON_RF_EVENT,                      /* RX PATH */
  RFLM_LTE_DEBUG_RX_OFF_RF_EVENT,                     /* RX PATH */
  RFLM_LTE_DEBUG_RX_MAX_EVENTS,                       /* Max Number of Rx Debug Events */  

}rflm_lte_debug_rx_task_t; 

typedef enum 
{
  /*!> Initial value */
  RFLM_LTE_TX_RETUNE_DEFAULT = 0, 

  /*!> Tx re-tune progress */    
  RFLM_LTE_TX_RETUNE_REQUEST,                      /* Request for tx re-tune*/
  RFLM_LTE_TX_RETUNE_PERMISSION,                   /* Permission granted */
  RFLM_LTE_TX_RETUNE_PERMISSION_OVERRIDE,          /* Permission override */  

}rflm_lte_tx_retune_progress_t; 

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES -- Type and Structures

==============================================================================*/

typedef struct
{
  /*! < taskq handle for doing Script based Tuning */
  rflm_ccs_rf_tq_handle_t *script_tune_tq_ptr;

  /*! < flag to indicate whether use source TQ or IRAT TQ*/
  boolean                 use_source_tq;

  /*!<  LM buffer index for source Prx handle */
  lm_handle_type         source_rxlm_buf_idx_ant0;

}rflm_lte_rx_dm_buf_type;

typedef struct
{
  /*! < taskq handle for doing Script based Tuning */
  void *script_tune_tq_ptr;

}rflm_lte_tx_dm_buf_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  LTE FED DM interface structure
 
  @details
  This is the meta structure of RF MDSP-FW data structure. Since most of the
  pamrameters are moved to FED DM, the data structure will be copied to DM as
  is (excluding device scripts) based on LM handle
  
*/
typedef struct
{
  rflm_lte_rx_dm_buf_type rx_data;

  rflm_lte_tx_dm_buf_type tx_data;

  /*! RFLM handle */ 
}rflm_lte_dm_buf_type;


/* Logging Structure for TxPLL */
typedef struct
{

  uint32  fw_action_time;

  uint32  action_time;

  boolean afc_update;

  uint32 ccs_tq_data_offset;

}rflm_lte_debug_txpll_s;

/* Logging Structure for CCS Event Action Time */
typedef struct
{

  rflm_lte_debug_tx_task_t type;

  uint32 action_time;

}rflm_lte_debug_tx_ccs_action_time; 

/* Master Logging Structure */
typedef struct
{

  /*********************************************************/
  /*!               LOG SUB FRAME SETTINGS                 */
  /*********************************************************/ 

  uint32 frame_num;

  uint8  sf_number;

  rflm_lte_ul_chan_type_e subframe_type;

  boolean                first_sf_after_dtx;

  boolean                freq_hopping_enable;

  /*********************************************************/
  /*!                 LOG TXAGC SETTINGS                   */
  /*********************************************************/ 

  int8    rgi_index
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  int8    pa_state
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  boolean active_slot
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  int32   pwr_db10
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  uint32  action_time
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  rflm_lte_debug_txpll_s txpll_afc;

  rflm_lte_debug_tx_ccs_action_time ccs_tasks_timing_capture
                                   [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
                                   [RFLM_LTE_DEBUG_TX_MAX_EVENTS];

  uint8 count_ccs_tasks[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];


  /* Tx ON OFF || Tx EN LOW HIGH Params */

  uint32 index_for_on_off_event;

  uint32 tx_on_off_ccs_tq_data_offset
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  boolean tx_on_off_enqueued
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  boolean tx_on_off_serviced
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint32 tx_on_wr_on_bytes_before_ccs[RFLM_LTE_TX_ON_WR_SIZE];

  uint32 tx_on_wr_off_bytes_before_ccs[RFLM_LTE_TX_ON_WR_SIZE];

  uint32 tx_on_wr_on_bytes_after_ccs[RFLM_LTE_TX_ON_WR_SIZE];

  uint32 tx_on_wr_off_bytes_after_ccs[RFLM_LTE_TX_ON_WR_SIZE];

  uint32 en_high_low_rf_event_index;  

  uint32 tx_en_high_low_ccs_tq_data_offset  
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  boolean tx_en_high_low_enqueued  
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  boolean tx_en_high_low_serviced  
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  uint32 tx_en_wr_on_bytes_before_ccs[RFLM_LTE_TX_EN_WR_SIZE];

  uint32 tx_en_wr_off_bytes_before_ccs[RFLM_LTE_TX_EN_WR_SIZE];  

  uint32 tx_en_wr_on_bytes_after_ccs[RFLM_LTE_TX_EN_WR_SIZE];

  uint32 tx_en_wr_off_bytes_after_ccs[RFLM_LTE_TX_EN_WR_SIZE];  

  uint32 txagc_tx_off_ccs_tq_data_offset
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  /*********************************************************/
  /*!             LOG DIGITAL TXAGC SETTINGS               */
  /*********************************************************/  

  uint32 env_scale [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint32 iq_gain [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  rflm_txagc_xpt_mode_t xpt_mode [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint8 iref [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint8 dpd_lut_lmem_idx [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint8 dpd_hw_tbl_idx [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint8 drif_group [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint8 lin_tbl_idx [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  
  uint8 first_sweep_lin_tbl_idx [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  uint32 do_fbrx [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];  
  
  uint32 fbrx_correction [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];  

  uint32 pin_comp [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  uint32 pout_comp [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];  

  uint32 dac_set_point [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];  
  
}rflm_lte_debug_per_sf_s;

typedef struct
{

  /* Keeps a count of the number of ON OFF events sent to CCS */
  uint32 on_off_event_counter;

  /* Rolling Logging Buffer Index */
  uint32 rflm_debug_index;
   
  rflm_lte_debug_per_sf_s rflm_debug[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
  
}rflm_lte_debug_s;

typedef enum
{
  RFLM_LTE_AFC_EVENT_NO_UPDATE,
  RFLM_LTE_AFC_EVENT_PENDING,
  RFLM_LTE_AFC_EVENT_EXECUTING,
  RFLM_LTE_AFC_EVENT_COMPLETE,
} rflm_lte_afc_event_status_e;

typedef struct
{
  rflm_lte_afc_event_status_e status;
  uint32 data_bytes[RFLM_LTE_AFC_UPDATE_SIZE];
}rflm_lte_afc_ccs_event_s;

typedef struct
{
  rflm_ccs_rf_event_status_type status;
  uint32 data_bytes[RFLM_LTE_CA_RECONFIG_SIZE];
}rflm_lte_ca_reconf_event_s;


typedef struct
{
  uint32 therm_wr_init_bytes[RFLM_LTE_THERM_WR_MAX_INIT_SIZE];
  uint32 therm_wr_conv_disable_bytes[RFLM_LTE_THERM_WR_MAX_CONV_DISABLE_SIZE];
  uint32 therm_rd_bytes[RFLM_LTE_THERM_RD_SIZE];
  uint32 rx_on_wr_on_bytes[RFLM_LTE_RX_ON_WR_SIZE];
  uint32 rx_on_wr_off_bytes[RFLM_LTE_RX_ON_WR_SIZE];
  uint32 tx_on_wr_on_bytes[RFLM_LTE_TX_ON_WR_SIZE];
  uint32 tx_on_wr_off_bytes[RFLM_LTE_TX_ON_WR_SIZE];
  uint32 gnss_blanking_bytes[RFLM_LTE_GNSS_WR_SIZE];
  uint32 tx_en_wr_on_bytes[RFLM_LTE_TX_EN_WR_SIZE];
  uint32 tx_en_wr_off_bytes[RFLM_LTE_TX_EN_WR_SIZE];
  uint32 smps_en_wr_on_bytes[RFLM_LTE_SMPS_EN_WR_SIZE];
  uint32 smps_en_wr_off_bytes[RFLM_LTE_SMPS_EN_WR_SIZE];
  uint32 tdd_asm_rx_wr_bytes[RFLM_LTE_ASM_RX_WR_SIZE];
  uint32 tdd_asm_tx_wr_bytes[RFLM_LTE_ASM_TX_WR_SIZE];
  uint32 rx_en_wr_on_bytes[RFLM_LTE_RX_EN_WR_SIZE];
  uint32 rx_en_wr_off_bytes[RFLM_LTE_RX_EN_WR_SIZE];
  uint32 rx_en_rx_on_wr_off_bytes[RFLM_LTE_RX_EN_RX_ON_WR_SIZE];
  uint32 tx_en_tx_on_wr_on_bytes[RFLM_LTE_TX_EN_TX_ON_WR_SIZE];
  uint32 tx_en_tx_on_wr_off_bytes[RFLM_LTE_TX_EN_TX_ON_WR_SIZE];
  uint32 papm_on_bytes[RFLM_LTE_PAPM_ON_SIZE];
  uint32 papm_standby_bytes[RFLM_LTE_PAPM_STANDBY_SIZE];
  uint32 scell_rx_on_wr_on_bytes[RFLM_LTE_RX_ON_WR_SIZE];
  uint32 scell_rx_on_wr_off_bytes[RFLM_LTE_RX_ON_WR_SIZE];
  uint32 tdet_temp_comp_bytes[RFLM_LTE_TDET_TEMP_COMP_SIZE];
  uint32 fbrx_ldo_setup_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_FBRX_LDO_SETUP_SIZE];
  uint32 fbrx_ldo_disable_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_FBRX_LDO_DISABLE_SIZE];
  uint32 fbrx_ldo_always_on_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_FBRX_LDO_SETUP_SIZE];
  uint32 fbrx_adc_hdet_enable_bytes[RFLM_LTE_ADC_HDET_ENABLE_SIZE];
  uint32 fbrx_config_coupler_fwd_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_FBRX_CONFIG_COUPLER_SIZE];
  uint32 fbrx_config_coupler_rev_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_FBRX_CONFIG_COUPLER_SIZE];
  rflm_lte_afc_ccs_event_s afc_event_buf;
  rflm_lte_ca_reconf_event_s ca_reconf_event_buf;
 uint32 fbrx_hdet_par_gs1_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_ADC_HDET_PAR_ENABLE_SIZE];
 uint32 fbrx_hdet_par_gs2_bytes[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_LTE_ADC_HDET_PAR_ENABLE_SIZE];

  uint32 event_ptr[RFLM_LTE_CSS_EVENT_NUM];
}rflm_lte_ccs_events_types_s;

/* ------------------------- NLIC Data structures ---------------------*/

typedef rflm_lte_nlic_return_s rflm_lte_nlic_sf_data_type;

/* ------------------------- HDET and THERM events --------------------*/
typedef struct
{
  uint16 addr; /*only 8bits for now*/
  uint8 value;
}rflm_lte_hw_rd_data_type;

typedef struct
{
    rflm_lte_hw_rd_data_type hdet_rd[RFLM_LTE_HDET_RD_RESULT_NUM_TRANS];
    rflm_lte_rf_trans_type_e trans_type;
  uint8 num_trans;
}rflm_lte_hdet_rd_result_data_type;

typedef struct
{
    rflm_lte_hw_rd_data_type therm_rd[RFLM_LTE_THERM_RD_RESULT_NUM_TRANS];
    rflm_lte_rf_trans_type_e trans_type;
  uint8 num_trans;
}rflm_lte_therm_rd_result_data_type;

typedef struct                                      // ALIGN(32)    /*Removed during memory reduction*/
{
  //rflm_lte_hdet_rd_result_data_type hdet_rd_event_result;       /*Removed during memory reduction*/
  rflm_lte_therm_rd_result_data_type therm_rd_event_result;
}rflm_lte_ccs_rd_events_result_data_type;

/* ------------------------- RF script buffer  --------------------*/
/*! @brief GRFC transaction data structure */
 
typedef struct
{
  uint16 num_trans;            /*!< Number of transaction data */
}rflm_lte_grfc_hdr;

/*! @brief SSBI transaction data structure */

typedef struct
{
  uint8 ssbi_bus;              /*!< The SSBI bus number */
  uint8 rd_wr;                /*!< Read or write transaction. */
  uint16 num_trans;            /*!< Number of transaction data */
}rflm_lte_ssbi_hdr;

typedef struct
{
  uint8 addr;                 /*!< The 8 bit SSBI address */
  uint8 data;                 /*!< The 8 bit SSBI data. Undefined if transaction is read */
  boolean timed_imm;          /*!< Timed or immediate. If immediate, start_delta will be ignored */
  int16 start_delta;          /*!< Start time in TBD units */
}rflm_lte_ssbi_data_type;


/*! @brief RFFE transaction data structure */

typedef struct
{
  uint8 rd_wr;                /*!< Read or write transaction. */
  uint8 channel;              /*!< The RFFE channel (bus) number */
  uint8 slave_id;             /*!< The RFFE slave ID */
  boolean rd_delay;           /*!< The read delay (N/A for writes) */
  boolean half_rate;          /*!< Use half rate reads (N/A for writes) */
  uint16 num_trans;            /*!< Number of transactions to write or read */
}rflm_lte_rffe_hdr;

typedef struct
{
  boolean timed_imm;          /*!< Timed or immediate. If immediate, start_delta will be ignored */
  boolean extended_cmd;       /*!< Whether this transaction uses extended command format or not (must be TRUE if addr is 16 bits or num_bytes > 1 */
  uint8 num_bytes;            /*!< Number of bytes to write or read */
  uint8 data;                 /*!< Data bytes to write or space for holding read data */
  uint16 addr;                /*!< The RFFE register address. 8 bits for RFFE*/
  int16 start_delta;          /*!< Start time in TBD units */
}rflm_lte_rffe_data_type;


typedef struct
{
  union{
     uint8 data[RFLM_LTE_CCS_NUM_DATA_PER_EXT_RFFE];   /*!< Data bytes to write or space for holding read data */
     uint32 data_word;
   };
  boolean timed_imm;          /*!< Timed or immediate. If immediate, start_delta will be ignored */
  boolean extended_cmd;       /*!< Whether this transaction uses extended command format or not (must be TRUE if addr is 16 bits or num_bytes > 1 */
  uint8 num_bytes;            /*!< Number of bytes to write or read, upto 4 bytes*/
  uint32 addr;                /*!< The EXT RFFE register address. 16 bits */
  int16 start_delta;          /*!< Start time in TBD units */
  uint32 mode;                /*!< 1 - read, 2 - write */
  uint32 type;                /*!< Reg0 - 0, Reg - 1, Ext - 2, Ext_long - 3 */
  uint32 transaction_word[2];
}rflm_lte_ext_rffe_data_type;

typedef struct
{
  rflm_lte_rf_trans_type_e type;
  uint8 index;
  union
  {
    rflm_lte_ssbi_hdr ssbi_hdr;
    rflm_lte_grfc_hdr grfc_hdr;
    rflm_lte_rffe_hdr rffe_hdr;
  } hdr;
}rflm_lte_rf_script_type;


typedef struct
{
  rflm_lte_rf_script_type  tdd_asm_tx_script;
  rflm_lte_rf_script_type  tdd_asm_tx_trigger_script;
  /* To program multiple ASM settings for Tx */
  rflm_lte_rf_script_type  tdd_aux_asm_tx_script[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_rf_script_type  tdd_aux_asm_tx_trigger_script[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_rf_script_type  tdd_asm_prx_script;
  rflm_lte_rf_script_type  tdd_asm_prx_trigger_script;
  rflm_lte_rf_script_type  tdd_asm_drx_script;
  rflm_lte_rf_script_type  tdd_asm_drx_trigger_script;
  /* To program multiple ASM settings for Rx */
  rflm_lte_rf_script_type  tdd_aux_asm_prx_script[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_rf_script_type  tdd_aux_asm_prx_trigger_script[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_rf_script_type  tdd_aux_asm_drx_script[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_rf_script_type  tdd_aux_asm_drx_trigger_script[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_rf_script_type  tdd_asm_tx_grfc_script;
  rflm_lte_rf_script_type  tdd_asm_prx_grfc_script;
  rflm_lte_rf_script_type  tdd_asm_drx_grfc_script;
}rflm_lte_rf_tdd_asm_scripts_type;

typedef struct
{
  rflm_lte_rf_script_type  pa_state[RFLM_TXAGC_PA_STATE_MAX];
  rflm_lte_rf_script_type  pa_on; //This is only used for GRFC PA
  rflm_lte_rf_script_type  pa_off;
  rflm_lte_rf_script_type  pa_trigger;
 
}rflm_lte_rf_pa_scripts_type;

/*----------------------------------------------------------------------------*/
/*!
  Captures Transceiver scripts needed to be programmed for 4F mod
  in LTE (not all the WTR devices might have such requirements)
*/
typedef struct
{
  /*! Boolean to let the caller know if the 4fmod related writes are
    needed for the current WTR*/
  boolean is_4fmod_writes_valid;
 
  /*! Reference RB location to choose one of the lhs or rhs 
    4fmod config scripts */
  uint8 ref_rb_location;

  /*! For the config with number of RB below this, use  
    the LHS and/or RHS scripts */
  uint8 num_rb_threshold;
 
  /*! WXE trigger writes when number of RB <= num_rb_threshold
    and if these RBs location is on the right of reference RB */
  rflm_lte_rf_script_type wtr_4fmod_script_rhs_of_ref;
 
  /*! WXE trigger writes when number of RB <= num_rb_threshold
    and if these RBs location is on the left of reference RB */
  rflm_lte_rf_script_type wtr_4fmod_script_lhs_of_ref;

  /*! WXE trigger writes when number of RB > num_rb_threshold */
  rflm_lte_rf_script_type default_4fmod_script;
 
} rflm_lte_wtr_4fmod_scripts_type;

/* Structure for scripts*/
typedef struct
{
  rflm_lte_rf_script_type  qpoet;
  int32  pad1;
  rflm_lte_rf_script_type  txpll_afc_script;
  int32  pad2;
  //rflm_lte_rf_script_type  qpoet_trigger_script;
  int32  pad3;
  rflm_lte_rf_script_type  wtr_wxe;
  int32  pad4;
  rflm_lte_rf_script_type  wtr;
  int32  pad5;
  rflm_lte_rf_script_type  fbrx;
  int32  pad6;
  rflm_lte_rf_script_type  wxe;
  int32  pad7;
  rflm_lte_rf_script_type  ant_swap_tx_0;
  int32  pad8;
  rflm_lte_rf_script_type  ant_swap_tx_1;
  int32  pad9;
  rflm_lte_rf_script_type gnss;
  int32  pad10;
  rflm_lte_rf_script_type  rgi_0_wxe;
  rflm_lte_rf_script_type  rgi_0_wtr;


  /* We can support RFLM_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH many 
     unique ASM scripts per LTE Band specific path */
  rflm_lte_rf_tdd_asm_scripts_type tdd_asm_script[RFLM_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH];
  int32  pad11;

  /*Variable which will specify which TDD Tx ASM port settings to be used
  We need to make sure the value of this variable is always
  less than RFLM_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH */
  uint8 tdd_tx_pa_asm_script_index;
  int32  pad12;

  /*Variable which will specify which TDD Rx ASM port settings to be used
  We need to make sure the value of this variable is always
  less than RFLM_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH */
  uint8 tdd_rx_asm_script_index;
  int32  pad13;

  /* Enable script for FBRx WTR path and gain state programming for WTR */
  rflm_lte_rf_script_type fbrx_enable[RFLTE_DM_FBRX_SUPPORTED_TX_DEVICES][RFLM_FBRX_NUM_GAIN_STATES];

  /* TX off script */
  rflm_lte_rf_script_type tx_off;

  /* FBRX LDO script */
  rflm_lte_rf_script_type ldo_setup;
  rflm_lte_rf_script_type ldo_disable;

  rflm_lte_rf_script_type hdet_enable;

  /* PA Scripts*/
  rflm_lte_rf_pa_scripts_type pa_script[RFLM_LTE_MAX_PA_SCRIPTS_SUPPORTED_PER_PATH];
  uint8 pa_script_index;

  rflm_lte_wtr_4fmod_scripts_type wtr_wxe_4fmod;

}rflm_lte_rf_scripts;

/* ------------------------- Structure for CCS timings  -------------------- */

typedef struct
{
  /* Treat post ue_srs timng the same as tx_cont */

  /* WTR RGI Write Timing from Start of UL SF for both FDD and TDD */

  int16 tx_on_ul_sf_agc_timing;                    /* DTx/Sleep--UL_SF*/
  int16 tx_on_prach_or_srs_only_agc_timing;        /* Sleep--PRACH or Sleep--SRS */

  int16 tx_cont_ul_sf_agc_timing;                  /* UL_SF--UL_SF */
  int16 tx_cont_pre_ue_srs_agc_timing;             /* UL_SF--SRS */
  int16 tx_cont_post_ue_srs_agc_timing;            /* SRS---UL_SF */

  /* PA Control Timings from Start of UL SF for PA ON and end of UL SF for PA OFF - both FDD and TDD*/

  int16 tx_on_ul_sf_pa_ctl_on_timing;              /* DTx/Sleep--UL_SF*/
  int16 tx_on_prach_or_srs_only_pa_ctl_on_timing;  /* Sleep--PRACH or Sleep--SRS */
  int16 tx_on_qpoet_nontr_to_qpoet_tr;             /* QPoet Non Trigger to QPoet Trigger Delta*/

  int16 tx_cont_ul_sf_pa_ctl_on_timing;            /* UL_SF--UL_SF */
  int16 tx_cont_pre_ue_srs_pa_ctl_on_timing;       /* UL_SF--SRS */
  int16 tx_cont_post_ue_srs_pa_ctl_on_timing;      /* SRS---UL_SF */

  int16 tx_off_null_sf_pa_ctl_off_timing;          /* UL_SF--Sleep/DTx */
  int16 tx_off_cell_srs_pa_ctl_off_timing;         /* UL_SF--Blank_SRS_Sym */

  /* PA Current Timings from Start of UL SF for PA ON and end of UL SF for PA OFF - both FDD and TDD*/

//  int16 tx_on_ul_sf_pa_current_timing;              /* DTx/Sleep--UL_SF*/
//  int16 tx_on_prach_or_srs_only_pa_current_timing;  /* Sleep--PRACH or Sleep--SRS */
//
//  int16 tx_cont_ul_sf_pa_current_timing;            /* UL_SF--UL_SF */
//  int16 tx_cont_pre_ue_srs_pa_current_timing;       /* UL_SF--SRS */
//  int16 tx_cont_post_ue_srs_pa_current_timing;      /* SRS---UL_SF */

  /* ASM, Tx ON OFF and Tx EN OFF Timings for TDD */

  int16 tdd_tx_on_ul_sf_asm_tx_timing;             /* DTx/Sleep--UL_SF or DL_SF--UL_SF */
  int16 tdd_tx_on_prach_or_srs_only_asm_tx_timing; /* Sleep/DL_SF--PRACH or Sleep/DL_SF--SRS */

  int16 tdd_tx_off_null_sf_asm_rx;                 /* UL_SF/PRACH--DL_SF OR UL_SF/PRACH--<Empty>*/
  int16 tdd_tx_off_cell_srs_asm_rx;                /* UL_SF--Blank_SRS_Sym */

   int16 tdd_tx_on_low_timing;                   /* UL_SF--DL_SF OR UL_SF--<Empty> */
   int16 tdd_tx_en_low_timing;                   /* UL_SF--DL_SF OR UL_SF--<Empty> */

  /* TxPLL AFC Timings for FDD and TDD */

  int16 tx_on_ul_sf_txpll_afc_timing;               /* DTx/Sleep--UL_SF */

  int16 tx_cont_ul_sf_txpll_afc_timing;             /* UL_SF--UL_SF */
  int16 tx_cont_post_ue_srs_txpll_afc_timing;       /* SRS---UL_SF */

  /* RxPLL AFC Timings for FDD and TDD */
  int16 rx_on_ul_sf_rxpll_afc_timing;               /* DRx/Sleep--DL_SF */

  int16 rx_cont_ul_sf_rxpll_afc_timing;             /* DL_SF--DL_SF */

  /* QPOET config timing for FDD and TDD */

  int16 tx_on_ul_sf_bias_config;
  int16 tx_on_prach_or_srs_only_bias_config;
  
  int16 tx_cont_ul_sf_bias_config;
  int16 tx_cont_pre_ue_srs_bias_config;
  int16 tx_cont_post_ue_srs_bias_config;

  int16 common_tx_bias_off;

  /* Tx ON/Tx EN timings for FDD and TDD */

   int16 fdd_tx_on_high_timing;
   int16 fdd_tx_en_high_timing;

   int16 fdd_tx_on_low_timing;
   int16 fdd_tx_en_low_timing;

   int16 tdd_tx_on_high_timing;

  /* Tx Antenna Switch for TxAGC events for FDD and TDD */

  int16 ant_s_tx0_timing;
  int16 ant_s_tx1_timing;

  /* General Antenna Swap for features like AsDiv for FDD and TDD */

  int16 en_ant_s_timing;
  int16 dis_ant_s_timing;

  /* FBRX timing */
  int16 fbrx_ldo_en_offset_to_rgi_timing;

  /* RGI 0 timing */
  int16 tx_off_ul_sf_rgi0_timing;
  int16 tx_cell_srs_ul_sf_rgi0_timing;

  /* TM8 timing */
  int16 tx_on_tm8_xsw_timing;
  int16 tx_cont_tm8_post_ue_srs_xsw_timing;
  int16 tx_cont_tm8_pre_ue_srs_xsw_timing;
  int16 tx_cont_tm8_post_ue_srs_smps_pgm_timing;
  int16 tx_cont_tm8_post_ue_srs_pa_ctl_on_timing;
  int16 tx_cont_tm8_pre_ue_srs_smps_pgm_timing;
  int16 tx_cont_tm8_pre_ue_srs_pa_ctl_on_timing;
  int16 tx_cont_tm8_post_ue_srs_agc_timing;
  int16 tx_cont_tm8_pre_ue_srs_agc_timing;
  int16 tdd_tx_off_cell_srs_tm8_xsw;
  int16 tdd_tx_off_null_sf_tm8_xsw;

}rflm_lte_rf_timing;

typedef struct
{
  rflm_lte_ssbi_hdr hdr;           
  rflm_lte_ssbi_data_type data[RFLM_LTE_TX_SSBI_MAX_TRANS];
} rflm_lte_ssbi_buf;

typedef struct
{
  uint32 grfc_mask[2]; 
  int16 start_delta;
}rflm_lte_grfc_data_type;

typedef struct
{
  rflm_lte_grfc_hdr hdr;
  rflm_lte_grfc_data_type data;
} rflm_lte_grfc_buf;

typedef struct
{
  rflm_lte_rffe_hdr hdr;           
  rflm_lte_rffe_data_type data[RFLM_LTE_TX_RFFE_MAX_TRANS];
} rflm_lte_rffe_buf;

typedef struct
{
  rflm_lte_rffe_hdr hdr;           
  rflm_lte_ext_rffe_data_type data[RFLM_LTE_TX_RFFE_MAX_TRANS];
} rflm_lte_ext_rffe_buf;

typedef struct
{
  boolean active_buff;
  int32  ctrl_data[2];
}rflm_lte_dynamic_event_buf_ctl_data_s;

/*!  @brief structure of txagc PA static info. */
typedef struct
{
  /*! @brief The kind of NV from which the PA Static data is loaded to LM */
  uint16 num_max_pa_states_supported;

  /*! @brief The max number of active states for PA */
  uint16 num_active_pa_states;
  
  /*number of pa range GRFC*/
  int32  num_pa_range_grfc;
  
}rflm_txagc_pa_static_info_t;

/* ----------------------------- CCS structure ----------------------- */
typedef struct
{
  rflm_lte_ccs_events_types_s  ccs_events;
  rflm_lte_rf_scripts          script_type;
  rflm_lte_ssbi_data_type      ssbi_buff[RFLM_LTE_SSBI_TRANS_MAX];
  rflm_lte_grfc_data_type      grfc_buff[RFLM_LTE_GRFC_TRANS_MAX];
  rflm_lte_rffe_data_type      rffe_buff[RFLM_LTE_RFFE_TRANS_MAX];
  rflm_lte_ext_rffe_data_type  ext_rffe_buff[RFLM_LTE_RFFE_TRANS_MAX];
  rflm_lte_ccs_rd_events_result_data_type ccs_rd_events_result;
  rflm_lte_rf_timing           rf_timing;
  rflm_lte_dynamic_event_buf_ctl_data_s     afc_ctl_data; /* The active_buff flag indicates the bing-bong buffer that should be used by FED
                                                    The ctrl_data for AFC stores the center frequency that is being tuned to currently */
}rflm_lte_script_buf_s; /* buffer are filled in by SW */

typedef struct 
{
  /*! @brief CCS RF TQ 0 taskQ pointer */
  rflm_ccs_rf_tq_handle_t *rx_tq_handle;
  /*! @brief CCS RF TQ 1 taskQ pointer */
  rflm_ccs_rf_tq_handle_t *tx_tq_handle;
  /*! @brief CCS RF TQ data memory pointer. The 2 tasks Q above
  need to share the data memory below, caller can partition it appropriately. */
  uint32 *tq_data_ptr;
  /*! @brief Length (in bytes) of the data memory */
  uint16 tq_data_bytes;
  /*! @brief TQ index corresponding to rx_tq_handle */
  uint8 rx_tq_index;
  /*! @brief TQ index corresponding to tx_tq_handle */
  uint8 tx_tq_index;
} rflm_lte_ccs_rf_pair_info_s;


typedef struct 
{
  /*! @brief CCS RF auxiliary data buffer memory pointer. */
  uint32 *aux_data_ptr;
  /*! @brief Length (in bytes) of the aux data memory */
  uint16 aux_data_bytes;
} rflm_lte_ccs_aux_buf_info_s;

typedef struct
{
  uint16 ccs_mem_size_byte;
  uint16 ccs_mem_offset;
}rflm_lte_ccs_rf_pair_mem_alloc_t;

/* ------------------------- Tech data structure  -------------------- */

/*! @brief  txagc control data structure */
typedef struct
{
  /*! @brief  flag to indicate the buffer index has SW latest update */
  boolean last_buff_updated_by_sw;

  /*! @brief current txagc active buffer */
  boolean fed_active_buff;

  /*! @brief SW update is ready */
  boolean sw_update_ready;

}rflm_lte_dynamic_ctl_s;

/*----------------------------------------------------------------------------*/
/*!
  @name Structure to hold LTE RxAGC specific data unique for each WB 
 
  @brief
  This structure holds LTE data to be accessed through scratch mem provided by
  RxAGC common module
*/

/*! @{ */
typedef struct
{
  /*! Boolean to indicate if the IQMC vals are valid or not */
  boolean is_iqmc_vals_valid;

  /*! Boolean to indicate: 
      TRUE : If the IQMC values stored are calculated based on one default
             LNA gain state
      FALSE: were they calculated per LNA gain state  */
  boolean is_iqmc_vals_default;

  /*! IQMC compensation value for each of the LNA gain states */
  uint32 iqmc_vals[RFLM_CMN_RXAGC_MAX_LNA_STATES];

}rflm_lte_rxagc_tech_data_s;

typedef struct
{
  int dummy ;	
}rflm_lte_rx_ccs_events_types_s;

typedef union
{
  rflm_lte_grfc_buf grfc;
  rflm_lte_ssbi_buf ssbi;
  rflm_lte_rffe_buf rffe;
  rflm_lte_ext_rffe_buf ext_rffe;
}rflm_lte_buf_union_s;

typedef struct
{
  /* Type of RF transaction */
  rflm_lte_rf_trans_type_e script_type;

  /* Index where the transaction is present in script data */  
  uint8 script_index;

  /* Transaction Script Buffer */
  rflm_lte_buf_union_s    script_data;
  
}rflm_lte_pll_afc_dynamic_script_s;

typedef struct
{

  rflm_lte_rf_script_type          rx_gain_ctrl
                                    [RFLM_LTE_MAX_NUM_RX_PATHS]
                                    [RFLM_LTE_MAX_NUM_RX_GAIN_RANGES];
}rflm_lte_rx_script_type;

typedef struct
{

  rflm_lte_rx_script_type          script_type;
  rflm_lte_ssbi_data_type          ssbi_buff[RFLM_LTE_SSBI_TRANS_MAX];
  rflm_lte_grfc_data_type          grfc_buff[RFLM_LTE_GRFC_TRANS_MAX];
  rflm_lte_rffe_data_type          rffe_buff[RFLM_LTE_RFFE_TRANS_MAX];
  rflm_lte_ext_rffe_data_type      ext_rffe_buff[RFLM_LTE_RFFE_TRANS_MAX];
  
}rflm_lte_rx_dynamic_script_s;

typedef struct
{
  int8    lna_gain_state
            [RFLM_LTE_MAX_NUM_RX_PATHS]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE]; 

  uint8   rx_gain_force_oride_flag
            [RFLM_LTE_MAX_NUM_RX_PATHS];  

  uint8   rx_gain_force_write
            [RFLM_LTE_MAX_NUM_RX_PATHS];  
  	
  int32   rx_gain_expected_rssi
            [RFLM_LTE_MAX_NUM_RX_PATHS];  

  rflm_lte_rx_dynamic_script_s script;

  /*! @brief  Rx PLL AFC Script */
  rflm_lte_pll_afc_dynamic_script_s  rxpll_afc_script;

}rflm_lte_rx_dynamic_s;

typedef struct
{
  /* */
  int16 mpr_array_for_pa_swpts
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int32 tpl [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int32 tpl_srs [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];  

  int32 tpl_coex [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  rflm_lte_nlic_sf_data_type nlic_sf_data           
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int32 dtpl[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int32 dtpl_srs [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

   int32 dtpl_coex [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int8 temp_bin;
  int8 temp_rem;

  int32 next_therm_state_read_offset;

  /* Tx PLL AFC Script */
  rflm_lte_pll_afc_dynamic_script_s txpll_afc_script;
  
  /* XPT Data */
  /*! @brief et delay table */
  int16 et_delay[RFLM_TXAGC_XPT_NUM_SLOT]
                [RFLM_LTE_NUM_SF_FOR_INTERFACE];  
  /* ET path delay register value */
  rflm_txagc_et_path_delay_t et_delay_reg[RFLM_TXAGC_XPT_NUM_SLOT]
                [RFLM_LTE_NUM_SF_FOR_INTERFACE];
  
}rflm_lte_tx_dynamic_s; 

typedef struct
{
  boolean   tx_off_on_txagc_cb;
  boolean   txagc_cb_enabled;
  boolean   txagc_update_needed[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  rflm_lte_ul_chan_type_e current_sf_type;
}rflm_lte_txagc_cb_log_s;

typedef struct
{
  rflm_lte_ul_chan_type_e subframe_type          
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int8    rgi_index_const
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int8    pa_state
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  boolean active_slot
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int32   pwr_db10
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  uint32  action_time
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  uint32  action_time_tx_on_en
            [RFLM_LTE_TXAGC_MAX_TX_ON_EN_TYPE]
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int16  wtr_trigger_offset
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  boolean afc_update
            [RFLM_LTE_NUM_SF_FOR_INTERFACE];

  int16  qpoet_trigger_offset
            [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  rflm_lte_ccs_rf_pair_info_s         tq_handle;

  rflm_lte_ccs_aux_buf_info_s         aux_buff_handle;

  rflm_lte_ccs_rf_pair_mem_alloc_t    aux_mem_alloc;

  rflm_lte_active_cell                active_cell[RFLM_LTE_NUM_SF_FOR_INTERFACE];

  boolean                             const_row_idx_changed;
  boolean                             smps_row_idx_changed;
  boolean                             first_sf_after_dtx;
  boolean                             asm_toggled_to_rx;
  boolean                             uppts_last_sym_blank;

  rflm_lte_therm_status_e             therm_event_status;
  int8                                tdet_event_status;
  uint8                               curr_tx_ant_index;

  /* Time profiling */
  rflm_time_profile_meas_s            update_tx_sf;
  rflm_time_profile_meas_s            tx_path_ctrl;
  rflm_time_profile_meas_s            txagc_update;
  rflm_time_profile_meas_s            commit_iss_seq;
  rflm_time_profile_meas_s            commit_single_task;
  rflm_time_profile_meas_s            commit_rf_event;
  rflm_time_profile_meas_s            rxagc_update;
    

  /* CCS buffer related */
  uint32                ccs_tq_data_wr_offset;
  uint32                ccs_tq_data_rd_offset;
  uint32                ccs_tq_data_aux_wr_offset;
  uint8                 single_task_buffer_idx;
  uint32                rx_ccs_tq_data_wr_offset;
  uint32                ccs_tq_data_tx_rf_event_wr_offset;
  uint32                ccs_tq_data_tx_rf_event_rd_offset;
  uint32                ccs_tq_data_rx_rf_event_wr_offset;
  uint32                ccs_tq_data_rx_rf_event_rd_offset;
  //boolean               tq_is_requested;

  /* Data needed when TxAGC Wake Up for Slot1 happens */
  rflm_lte_txagc_cb_log_s  txagc_cb_info[RFLM_LTE_MAX_NUM_SF];

  /* ET DAC management */
  boolean                     sf_requires_et;
  boolean                     et_dac_enabled;
  boolean                     schedule_et_dac_disable;

  /* TX DAC management */
  boolean                     tx_dac_enabled;

  /* Current Sub Frame Number Info */
  uint8                       sf_num_current;

  /* Ant Swap tracking */
  boolean              ant_swap_enabled[RFLM_LTE_NUM_SF_FOR_INTERFACE]
                                       [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  /* TXLM handle index */
  uint32                      txlm_handle_id;

  /* Flag to indicate if TX and ET DAC ON has been scheduled through TX_ON RF event CB */
  boolean              et_dac_on_scheduled;
  /* PA state from the very last TXAGC update */
  uint8                 prev_pa_state;

  /* TxAGC input and ouput locations for running txagc on the semaphore thread */
  rflm_lte_update_tx_sf_input_s   *current_tx_sf_input;
  rflm_lte_update_tx_sf_return_s  *current_tx_sf_return;
}rflm_lte_lm_params_s;

typedef struct
{
  int16 txpwr_dbm10[RFLM_TXAGC_TX_LIN_APT_SIZE]; 
  uint8 index_to_const_bias_tbl[RFLM_TXAGC_TX_LIN_APT_SIZE];
  int32 timer_for_conts_bias_tbl[RFLM_TXAGC_TX_LIN_APT_SIZE];
}rflm_lte_lin_per_pa_state_s;

typedef struct
{
  rflm_lte_lin_per_pa_state_s lin_tbl[RFLM_TXAGC_PA_STATE_MAX];
}rflm_lte_lin_s; 

typedef struct
{
  int8 num_rb_threshold; // this is for the 920 env_scale ET change
  int8 rb_based_pwr_adjustment;
  int16 pa_fall_sp[RFLM_TXAGC_PA_STATE_MAX]; 
  int16 pa_rise_sp[RFLM_TXAGC_PA_STATE_MAX];

  /*! @brief hyst timer for PA switch point*/
  int16 hysteresis_fall_timer;  
  
  rflm_lte_script_buf_s  scripts;
  boolean                is_tdd[RFLM_LTE_MAX_NUM_CARRIER];
  boolean                ftm_rf_enabled;
  boolean                is_cal_mode;
  /* XPT Static Data */
  rflm_txagc_xpt_mode_t  xpt_mode[RFLM_TXAGC_PA_STATE_MAX];
  uint32                 default_iq_gain;
  uint32                 default_xpt_iq_gain;
  uint32                 default_ept_env_scale;
  uint32                 default_et_env_scale;
  uint32                 clipper_thr_en_level;
  uint32                 clipper_thr_mpr0;
  uint32                 clipper_thr_mpr1;

  /* This variabled tells which type of power cotrol mode is being used in lm
     Based on this we ca schedule either FBRx or hkadc*/
  rflm_lte_tx_pwr_ctrl_type lte_power_contol_mode;

  /* This Flag is to control whether we want turn ON/OFF the
     FBRx gain correction feature. This is completely controlled
     by LTE RFSW */
  boolean                fbrx_gain_corr_master_flag;
  boolean   therm_read_active;
  boolean   therm_read_done;
    
  /* A flag to indicate whether to enable or not the FBRx Gain correction for
    Tx power , if FALSE, TxAGC sub-routine will trigger
    the common FBRx get the Gain correction error and before passing it to RFSW
    it will set it to TRUE. This flag will only be of relavance
    if fbrx_gain_corr_master_flag is enabled else, FBRx gain correction is disabled */
  boolean fbrx_gain_corr_active; 

  /* Variable to track the trigger power level for FBRx */
  int16 trigger_pwr_lvl_for_fbrx[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];

  /* Variable to track the trigger power level for FBRx */
  int16 trigger_pwr_lvl_for_hkadc;

  boolean hkadc_active;

  boolean hkadc_read_done;

  /* HDET PAR tracking */
  boolean   hdet_par_inprogress;

  /* Autopin support flag*/
  boolean autopin_supported; 

#ifdef FEATURE_BOLT_MODEM
  rflm_nlic_data_type      nlic_data;

#endif 

  /* PA static info */
  rflm_txagc_pa_static_info_t pa_static;

  /* AsDiv LM Index */
  uint32 asdiv_lm_idx;

  /* A flag to indicate whether to override TXAGC,
     if TRUE, override the target TXAGC pwr with values from SW */
  boolean txagc_override; 

  /* A flag to indicate whether TX path needs to be turned
   * on/off for FTM RF test */
  boolean ftm_tx_path_init;

  /* Override TXAGC value from SW per SF, for each SF, TXAGC is
     same for all slots */
  int32 txagc_override_dB10[RFLM_LTE_NUM_SF_FOR_INTERFACE];
  /* Flags to control tx re-tune operation in case of split intra-band CA*/
  rflm_lte_tx_retune_progress_t tx_retune_scenario_detected;

  /* Flag to indicate DTA is in progress. In this case don't write DRx ASM settings during TDD UL-DL transitions */
  boolean rx_dta_in_progress;

  /* Rx LO Offset from Center Freq(KHz). */
  int32 rx_lo_offset_from_center_freq_in_KHz[RFLM_LTE_MAX_NUM_CARRIER];

  /* Tx LO Offset from Center Freq(KHz) for Intra-band ULCA case */
  int32 tx_lo_offset_from_center_freq_in_KHz[RFLM_LTE_MAX_NUM_CARRIER];

}rflm_lte_fed_static_s; 

typedef struct
{
  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_tx_cont_transactions
                                        [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
                                        [RFLM_LTE_CCS_TX_CONT_MAX_TRANS];

  ccs_rf_transaction_rffe_ext_rw_t rflm_lte_ccs_tx_cont_rffe_ext_transac
                                        [RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL]
                                        [RFLM_LTE_CCS_TX_CONT_MAX_TRANS];

  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_tx_off_transactions
                                        [RFLM_LTE_CCS_TX_OFF_MAX_TRANS];

  ccs_rf_transaction_rffe_ext_rw_t rflm_lte_ccs_tx_off_rffe_ext_transac
                                        [RFLM_LTE_CCS_TX_OFF_MAX_TRANS];

  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_rxagc_transactions
                                        [RFLM_LTE_CCS_RXAGC_MAX_TRANS];

  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_txafc_transactions
                                        [RFLM_LTE_CCS_TXAFC_MAX_TRANS];

  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_rxafc_transactions
                                        [RFLM_LTE_CCS_RXAFC_MAX_TRANS];

  ccs_rf_transaction_rffe_ext_rw_t rflm_lte_ccs_txafc_rffe_trans
                                        [RFLM_LTE_CCS_TXAFC_MAX_TRANS];

  ccs_rf_transaction_rffe_ext_rw_t rflm_lte_ccs_rxafc_rffe_trans
                                        [RFLM_LTE_CCS_RXAFC_MAX_TRANS];

  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_rxagc_ncell_transactions
                                        [RFLM_LTE_CCS_RXAGC_MAX_TRANS];

  ccs_rf_transaction_ssbi_rw_t rflm_lte_ccs_single_task_transactions
                                        [RFLM_LTE_CCS_SINGLE_TASK_MAX_TRANS];

  ccs_rf_transaction_rffe_ext_rw_t rflm_lte_ccs_single_task_ext_transactions
                                        [RFLM_LTE_CCS_SINGLE_TASK_MAX_TRANS];

  ccs_rf_task_t rflm_lte_ccs_tx_cont_tasks
                                        [RFLM_LTE_CCS_TXAGC_NUM_BUF]
                                        [RFLM_LTE_CCS_TX_CONT_MAX_TASK];
  
  ccs_rf_task_t rflm_lte_ccs_tx_off_tasks[RFLM_LTE_CCS_TX_OFF_MAX_TASK];
  
  ccs_rf_task_t rflm_lte_ccs_rxagc_tasks[RFLM_LTE_CCS_RXAGC_MAX_TASK];

  ccs_rf_task_t rflm_lte_ccs_txafc_task[RFLM_LTE_CCS_TXAFC_MAX_TRANS];

  ccs_rf_task_t rflm_lte_ccs_rxafc_task[RFLM_LTE_CCS_RXAFC_MAX_TRANS];  

  ccs_rf_task_t rflm_lte_ccs_single_task;
  
  ccs_rf_task_t rflm_lte_ccs_rxagc_ncell_tasks[RFLM_LTE_CCS_RXAGC_MAX_TASK];

  ccs_rf_task_t rflm_lte_ccs_ant_sw_task[RFLM_LTE_CCS_ANT_SW_MAX_TASK];

  uint8 rflm_lte_ccs_rxagc_carrier_index[RFLM_LTE_CCS_RXAGC_MAX_TASK];

  uint8 rflm_lte_ccs_rxagc_ncell_carrier_index[RFLM_LTE_CCS_RXAGC_MAX_TASK];
}rflm_lte_ccs_buff_s;

typedef struct 
{
  ccs_rf_task_t  *ccs_task_ptr;
  uint8               num_tasks;
} rflm_lte_ccs_task_s; 

typedef struct
{
  rflm_lte_buf_union_s pa;
  rflm_lte_buf_union_s qpoet;
  rflm_lte_buf_union_s wtr_wxe;
  rflm_lte_buf_union_s wtr;
  rflm_lte_buf_union_s fbrx;
  rflm_lte_buf_union_s wxe;
  rflm_lte_buf_union_s ant_swap;
  rflm_lte_buf_union_s tdd_asm;
  rflm_lte_buf_union_s tdd_asm_aux[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
}rflm_lte_txagc_buf_s;

typedef struct
{
  rflm_lte_buf_union_s pa;
  rflm_lte_buf_union_s ant_swap;
  rflm_lte_buf_union_s rgi_0_wxe;
  rflm_lte_buf_union_s rgi_0_wtr;
  rflm_lte_buf_union_s tdd_asm;
  rflm_lte_buf_union_s tdd_asm_drx;
  /* Aux is common for both prx and drx*/
  rflm_lte_buf_union_s tdd_asm_aux[RFLM_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rflm_lte_buf_union_s tdd_tx_off;
}rflm_lte_tx_off_buf_s;

typedef struct
{
  uint32 event_script[RFLM_LTE_SINGLE_EVENT_WR_SIZE];
}rflm_lte_event_script_s;

typedef struct
{
  /* RXAGC */
  int32         current_rx_agc_value[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
  int32         current_rx_agc_inband_value[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
  int8          rx_gain_state_from_fw[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
  int8          pa_state;
  int8          rgi_index_const;
  int8          rgi_index_smps;
  uint16        num_rbs;
  uint16        start_rbs;
  int32         rflm_lte_pwr_db10;
  uint8         current_xpt_mode;
  int32         p_out_comp;
  int32         p_in_comp;
  uint32        env_scale_backoff;
  uint32        iq_backoff;
  int32         delay;
  uint32         rflm_lte_linear_pwr;
  uint32         rflm_lte_linear_pwr_srs1;
  uint32         rflm_lte_linear_pwr_srs2;
  uint32         dpd_idx;
}rflm_lte_fed2sw_s;

/*! 
  @brief  
  RFLM LTE Log context ID's
*/
typedef struct
{

  /*! @brief rflm logpack context id for lte txagc internal log */
  int txagc_internal_log;

  /*! @brief rflm logpack context id for lte txagc external log*/
  int txagc_external_log;  

}rflm_lte_context_id_s;

typedef struct
{
  //rflm_lte_rx_dynamic_s    rx_dynamic[RFLM_LTE_NUM_DYNAMIC_BUFF];                     /*Removed during memory reduction*/
  rflm_lte_tx_dynamic_s    tx_dynamic[RFLM_LTE_NUM_DYNAMIC_BUFF];
  rflm_lte_fed_static_s    fed_static;
  //rflm_lte_event_script_s  single_event_script[RFLM_LTE_MAX_NUM_SINGLE_EVENT_SCRIPT];  /*Removed during memory reduction*/
  //rflm_lte_buf_union_s     single_task_script[RFLM_LTE_MAX_NUM_SINGLE_TASK_SCRIPT];   /*Removed during memory reduction*/
  rflm_lte_lm_params_s     lm_params;
  rflm_lte_lin_s           lin_data;
  rflm_lte_fed2sw_s        fed2sw;
  rflm_lte_txagc_buf_s     txagc_buf[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  rflm_lte_tx_off_buf_s    tx_off_buf;
  rflm_lte_ccs_buff_s      ccs_buff;
  rflm_lte_ccs_task_s      ccs_task_ret[RFLM_LTE_TXAGC_MAX_NUM_SLOT_PER_CELL];
  rflm_lte_ccs_task_s      afc_ccs_task_ret;
  rflm_lte_ccs_task_s      pa_off_ccs_task_ret;
  rflm_lte_dynamic_ctl_s   tx_ctl_data;
  rflm_lte_dynamic_ctl_s   rx_ctl_data;
  boolean                  is_tq_allocated;
  rflm_lte_context_id_s    context_id;
  rflm_lte_update_tx_sf_return_s prev_tx_sf_return;
}rflm_lte_tech_data_s;

typedef struct
{
  rflm_lte_tech_data_s data_per_cell[RFLM_LTE_MAX_NUM_CELL];
  void (*txagc_uk_population_fn_ptr)(rflm_lte_tx_update_s*);
}rflm_lte_dm_for_all_cell_s;


/*Structure to hold intra band ca status and rxlm handles*/
typedef struct
{
    /*Future: intra band ca mask. Bit0 - PCELL, Bit1 - SCELL1, Bit2 - SCELL2... 
      If CA between PCELL and SCELL1 - mask = 0x03, between SCELL1 and SCELL2 - mask = 0x06
      For now: we will set it as boolean*/
    uint8       intra_band_ca_mask; 
    /*Adding Chain mask per carrier for HORxD*/
    uint8       horxd_chain_mask[RFLM_LTE_MAX_NUM_CARRIER];
    /* Flag to track when intra status changes and inform FW about the same, so that they can trigger LNA settings change from SCA to LGY and vice-versa*/
    boolean     force_lna_settings_if_intra_status_change;
	  /* Bit Mask to indicate if RXAGC is required to pick Cal data from Alt Path Cell. Bit0 - PCELL, Bit1 - SCELL1, Bit2 - SCELL2...  */
	  uint8 alt_path_ca_mask[RFLM_LTE_MAX_NUM_RX_PATHS];
    /*rxlm buffer index for each path */
    uint8       rxlm_buffer_index[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
    /*Flags to check if rxconfig is active per path*/
    boolean     rflm_lte_rxagc_rx_config_active[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];
    /*CCS memory pointer for each carrier*/
    uint32      rflm_lte_rxagc_ccs_mem_offset[RFLM_LTE_MAX_NUM_CARRIER];
    /*DM update in progress flag: TRUE = in progress, FALSE = done*/
    boolean     rflm_lte_rxagc_dm_update_in_progress[RFLM_LTE_MAX_NUM_CARRIER][RFLM_LTE_MAX_NUM_RX_PATHS];

}rflm_lte_dm_rx_info_s;

/*==============================================================================

                     Input/Output structure definitions

==============================================================================*/
typedef struct
{
  uint8                      sf_num;
  rflm_lte_ul_chan_type_e    current_sf_type;
}rflm_lte_update_tx_path_input_s;


/*==============================================================================
                DM related input/output structure definitions
==============================================================================*/


/*==============================================================================
                RXAGC related input/output structure definitions
==============================================================================*/

/*==============================================================================
                Other LTE input/output structure definitions
==============================================================================*/


typedef struct
{
  uint32     time;
  boolean    immediate_write;
  int8       script_id;  
  rflm_lte_rf_trans_type_e  trans_type;                   
}rflm_lte_run_single_ccs_task_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
              API input/output function definitions -- TXAGC 
==============================================================================*/
//boolean rflm_lte_is_tdd(void);
uint32 rflm_lte_ostmr_read_mstmr_count(void);
int16 rflm_lte_get_rffe_delay(uint16 addr);

void rflm_lte_log_timing_profile
( rflm_lte_cell_id_e       cell_index,
  uint8                    sf_num,
  rflm_time_profile_meas_s *profile_meas,
  rflm_lte_time_logging_e  logging_type
);

rflm_err_t rflm_lte_txagc_init
( 
   rflm_handle_tx_t handle_id
);

rflm_ustmr_t rflm_lte_util_ts_to_ustmr(int16 ts);

/*==============================================================================
              API input/output function definitions -- LTE DM
==============================================================================*/

rflm_lte_tech_data_s* rflm_lte_dm_get_data_ptr( rflm_lte_cell_id_e cell_index );

rflm_lte_dm_rx_info_s* rflm_lte_dm_rx_get_data_ptr( void );
rflm_lte_dm_for_all_cell_s* rflm_lte_dm_get_ptr(void );

boolean rflm_lte_dm_init( void );

boolean rflm_lte_dm_de_init( void );

boolean rflm_lte_dm_rx_info_init(void);

boolean rflm_lte_dm_rx_info_de_init(void);

boolean rflm_lte_dm_set_data
(
  rflm_lte_tech_data_s* rflm_lte_data_mem_ptr,
  void *user_input_data,
  rflm_lte_data_type_e data_t 
);

boolean rflm_lte_dm_get_data
(
  rflm_lte_tech_data_s* rflm_lte_data_mem_ptr,
  void *user_input_data,
  rflm_lte_data_type_e data_t
);

void rflm_lte_dm_sync_dynamic_data_buffer
(
  rflm_lte_cell_id_e cell_index,
  rflm_lte_data_type_e data_t
);

void
rflm_lte_dm_init_log_packet
(
  void
);

void
rflm_lte_dm_deinit_log_packet
(
  void
);


/*==============================================================================
              API input/output function definitions -- CCS
==============================================================================*/


void rflm_lte_ccs_create_grfc_task( ccs_rf_task_t * ccs_grfc_task, int16 task_start_delta,  uint8 level, 
                                    uint32 grfc_bitmask  );

uint8 rflm_lte_ccs_conv_ssbi_tasks
(
  ccs_rf_transaction_ssbi_rw_t *ccs_transaction_buff,
  ccs_rf_task_t                *ccs_task_buff,
  rflm_lte_ssbi_buf            *rf_buf, 
  uint8                        trans_idx,
  rflm_lte_ccs_task_s          *ccs_task
);

uint8 rflm_lte_ccs_conv_rffe_tasks
(
  ccs_rf_transaction_ssbi_rw_t *ccs_transaction_buff,
  ccs_rf_task_t                *ccs_task_buff,
  rflm_lte_rffe_buf            *rf_buf, 
  uint8                        trans_idx,
  rflm_lte_ccs_task_s          *ccs_task
);

uint8 rflm_lte_ccs_conv_ext_rffe_tasks
(
  ccs_rf_transaction_rffe_ext_rw_t *ccs_transaction_buff,
  ccs_rf_task_t                    *ccs_task_buff,
  rflm_lte_ext_rffe_buf            *rf_buf, 
  uint8                            trans_idx,
  rflm_lte_ccs_task_s              *ccs_task
);

uint8 rflm_lte_ccs_txagc_conv_rffe_tasks_pa_off
(
  rflm_lte_rffe_buf          *rf_buf, 
  uint8                      trans_idx,
  rflm_lte_ccs_buff_s        *ccs_buff,
  rflm_lte_ccs_task_s        *ccs_task
);

uint32
rflm_lte_ccs_commit_txagc_seq
(
  rflm_ccs_rf_seq_t *issue_seq_ptr,
  rflm_ccs_rf_tq_handle_t *rf_tq,
  uint32 ccs_mem_offset,
  uint32 ustmr_action_time,
  uint32 handle_id
);

void rflm_lte_ccs_commit_issue_seq
(
  rflm_lte_tech_data_s                     *rflm_lte_dm_ptr,
  rflm_lte_update_tx_sf_input_s            *tx_sf_input,
  rflm_lte_ccs_mem_alloc_e                  ccs_buf_ind,
  uint32                                   *ccs_tq_data_offset,
  uint32                                   *issue_seq_size,
  rflm_lte_ccs_cb_int_enable_e              int_cb_en,
  rflm_ccs_rf_task_callback_t               int_cb_func,
  rflm_lte_issue_seq_ccs_task_ret_type_e    ccs_task_type,
  rflm_lte_slot_type_s                      slot_type
);

void rflm_lte_ccs_commit_rf_event
(
  uint32                              action_time,
  uint32                              *ccs_tq_data_offset,
  rflm_lte_ccs_cb_int_enable_e        int_cb_en,
  rflm_ccs_rf_task_callback_t         int_cb_func,
  rflm_lte_tech_data_s                *rflm_lte_dm_ptr,
  rflm_lte_rf_event_type_e            event_type,
  rflm_lte_cell_id_e                  carrier_index
);

void  rflm_lte_ccs_commit_single_ccs_task
(
  uint32                         action_time_ustmr,
  int32                          delta_offset_ustmr,
  ccs_rf_task_t                  *ccs_task_ptr,
  boolean                        int_cb_en,
  rflm_ccs_rf_task_callback_t    int_cb_func,
  rflm_lte_tech_data_s           *rflm_lte_dm_ptr,
  rflm_lte_ccs_rf_pair_info_s    *lte_rf_pair,
  rflm_lte_rf_trans_type_e       trans_type,
  rflm_lte_handle_type_e         handle_type,
  rflm_lte_ccs_event_type_e      event_type
);

void rflm_lte_ccs_de_init_tq_handle
(
   rflm_lte_cell_id_e cell_idx
);

void rflm_lte_ccs_init_tx_tq_handle
(
   uint8 device,
   lm_handle_type txlm_handle_idx,
   rflm_lte_cell_id_e cell_idx
);

rflm_lte_ccs_task_s rflm_lte_ccs_txafc_conv
( 
   void                       *txpll_array, 
   int16                      task_start_delta,
   rflm_lte_ccs_buff_s        *ccs_buff,
   rflm_lte_ccs_task_s        *ccs_task
);

void rflm_lte_txagc_rf_event_done_cb
(
   struct rflm_ccs_rf_tq_handle_s *tq, 
   uint32 task_idx, 
   ccs_rf_task_status_t status, 
   void *arg
);


void rflm_lte_txagc_issue_seq_sf_done_cb
(
   struct rflm_ccs_rf_tq_handle_s *tq, 
   uint32 task_idx, 
   ccs_rf_task_status_t status, 
   void *arg
);

void rflm_lte_txagc_et_dac_done_cb
(
   struct rflm_ccs_rf_tq_handle_s *tq, 
   uint32 task_idx, 
   ccs_rf_task_status_t status, 
   void *arg
);


rflm_err_t rflm_lte_execute_single_ccs_task
(
  rflm_lte_cell_id_e                cell_index,
  rflm_lte_handle_type_e            handle_type,
  rflm_lte_run_single_ccs_task_s    *execute_script
);

uint32 rflm_lte_ccs_get_mem_wr_offset_for_all(uint32 seq_size_bytes,
                                              uint32 mem_start_offset,
                                              uint32 mem_buffer_size,
                                              uint32 *rflm_lte_ccs_mem_offset_ptr );

rflm_err_t rflm_lte_txpll_afc_update_ccs_buffer
(
   rflm_lte_update_tx_sf_input_s     *tx_sf_input
);
rflm_ccs_rf_tq_handle_t* rflm_lte_get_tx_tq(rflm_lte_cell_id_e cell_idx);
rflm_ccs_rf_tq_handle_t* rflm_lte_get_rx_tq(rflm_lte_cell_id_e cell_idx);
uint8 rflm_lte_get_tx_tq_idx(rflm_lte_cell_id_e cell_idx);

uint32
rflm_lte_ccs_get_mem_addr
(
  uint16 cell_idx,
  rflm_lte_ccs_mem_alloc_e ccs_mem_alloc_type
);

uint32 rflm_lte_create_dummy_task
(
  rflm_ccs_rf_tq_handle_t *tq_handle,
  uint32 action_time,
  void* callback_api,
  void* callback_params
);

boolean rflm_lte_rxagc_create_task(rflm_handle_rx_t rx_handle, uint32 carrier_idx, uint8 tq_index, cell_id_t cell_id);

void rflm_lte_txagc_settings_slot2_srs(void);

extern void rflm_slot1_srs_schedule(void);

extern void rflm_ca_reconfig_schedule(void);

extern void rflm_lte_task_init(void);

void rflm_slot_schedule_thread(void *arg);

void rflm_lte_txagc_turn_tx_path_off_on_wakeup
(
   uint32                sf_num,
   rflm_lte_tech_data_s *rflm_lte_dm_ptr,
   rflm_lte_slot_type_s  slot_type
);

int32 rflm_lte_get_afc_error(void);

boolean rflm_lte_set_afc_error(int32 freq_error_in_hz);

void rflm_lte_ccs_inc_ccs_mem_wr_offset(uint32 data_size_bytes, rflm_lte_tech_data_s *rflm_lte_dm_ptr);

uint32 rflm_lte_ccs_get_ccs_mem_wr_offset(uint32 data_size_bytes, rflm_lte_tech_data_s *rflm_lte_dm_ptr);

boolean rflm_lte_dm_tuner_de_init(uint32 carrier_idx, rflm_handle_tx_t handle_id);

void rflte_dm_rx_force_lna_settings_if_intra_status_change( boolean force_flag );

//void rflte_read_crit_wtr_regs(void);

uint32 rflm_lte_ccs_get_ccs_mem_wr_offset_rf_event ( boolean is_tx_rf_event,
                                                     uint32 data_size_bytes,
                                                     rflm_lte_tech_data_s *rflm_lte_dm_ptr );

void rflm_lte_ccs_inc_ccs_mem_wr_offset_rf_event ( boolean is_tx_rf_event,
                                                   uint32 data_size_bytes,
                                                   rflm_lte_tech_data_s *rflm_lte_dm_ptr );

void rflm_lte_ccs_get_rf_pair_mem_alloc ( rflm_lte_tech_data_s *rflm_lte_dm_ptr,
                                          rflm_lte_ccs_mem_alloc_e ccs_mem_alloc_type,
                                          rflm_lte_ccs_rf_pair_mem_alloc_t *mem_alloc,
                                          uint16 cell_idx );

void rflm_lte_pa_off_issue_seq_cb( struct rflm_ccs_rf_tq_handle_s *tq,
                                   uint32 task_idx,
                                   ccs_rf_task_status_t status,
                                   void *arg);
void rflte_dm_tx_set_tx_retune_detected (rflm_lte_tx_retune_progress_t tx_retune_detected_progress);
rflm_lte_tx_retune_progress_t rflte_dm_tx_get_tx_retune_detected ( void );

void rflte_dm_rx_set_rx_dta_in_progress( boolean rx_dta_flag );
boolean rflte_dm_rx_get_rx_dta_in_progress ( void );

boolean rflm_lte_util_calculate_effect_time(uint16 size_transaction_data_bytes, 
                                            rflm_ustmr_t *effect_time, 
                                            rflm_ustmr_t desired_action_time, 
                                            boolean immediate_flag, 
                                            boolean last_transaction_ext_rffe);

void rflm_lte_post_process_fbrx_vswr_measurements( rflm_handle_tx_t handle_id,
                                                   rflm_fbrx_vswr_meas_data_t vswr_meas_data);


/***********************************************************************/
/*                   RFLM LTE FED TXAGC/RXAGC LOGGING                        */
/***********************************************************************/
extern boolean enable_lm_debug_logging;
extern rflm_lte_debug_s *rflm_lte_debug_params_ptr;

/*RxAGC*/
typedef struct
{
    uint8 rxagc_set_lna_cnt;
    uint8 rxagc_get_lna_cnt;
    uint8 rxagc_rssi_cnt;
    uint8 rxagc_forced_set_lna_cnt;
    uint32 rxagc_forced_flag_ustmr_cnt;

    rflm_lte_get_lna_settings_return_s rxagc_get_lna_val[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
    rflm_lte_set_lna_state_input_s rxagc_set_lna_val[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
    rflm_lte_set_lna_state_input_forced_s rxagc_forced_set_lna_val[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
    rflm_lte_update_rx_sf_input_s rxagc_update_input_rx_sf_val[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
    rflm_lte_update_rx_sf_return_s rxagc_update_return_rx_sf_val[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
    rflm_lte_ustmr_pair_s forced_flag_ustmr_pair[RFLM_LTE_DEBUG_LM_BUFFER_SIZE];
}rflm_lte_rxagc_log_s;

extern boolean rflm_lte_rxagc_log_data_flag;
extern rflm_lte_rxagc_log_s *rflm_lte_rxagc_log_data;

typedef struct
{
  boolean task_idx_valid;
  uint8 task_idx;
}rflm_lte_rxagc_task_index_status_s;


boolean rflm_lte_check_debug_buf_index(void);

/* FBRx SM and - Lin Update structure, Update once per SF */
typedef struct
{
  rflm_handle_tx_t        handle_id;
  boolean                 keep_LDO_on; /* TRUE:ON, FALSE:OFF */
  boolean                 LDO_is_on; /* TRUE:ON, FALSE:OFF */
  boolean                 LDO_slow_ramp_done; /* TRUE:ON, FALSE:OFF */
  rflm_fbrx_run_counter_e run_ctr;
  boolean                 fbrx_enabled; /* TRUE:ON, FALSE:OFF */
  boolean                 gps_state;
  rflm_fbrx_meas_req_type_e fbrx_req;  /* VSWR or POWER CONTROL */
} rflm_lte_fbrx_sm_type;

/* HDET struct to enable setup and read, Update once per SF */
typedef struct
{
  uint32 carrier_idx;
  uint32 start_rb;
  uint32 rb_block;
  int16 gain_err;
  int32 txagc_val;
} rflm_lte_hdet_sm_type;
/* HDET PAR struct to enable setup and read, Update once per SF */
typedef struct
{
  uint32 carrier_idx;
  rflm_handle_tx_t handle_id;
  uint16 num_rb;
} rflm_lte_hdet_par_sm_type;
#ifdef __cplusplus
}
#endif /* extern "C" */ 

#endif /* RFLM_LTE_SW_INTF_H */
