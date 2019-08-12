 /*!
  @file
  rflte_msg.h

  @brief
  Message router UMIDs header for RF LTE subsystem. This file defines
  interfaces in the RF task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2009 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header  : //source/qcom/qct/modem/rfa3/main/3.0/api/lte/rflte_msg.h
$DateTime: 2016/12/13 07:59:52 $
$Author  : $

when       who     what, where, why
--------   ---     ------------------------------------------------------------------
09/23/16   as      Support for PRS measurement Gap to measure cells with lower BW compared to current
12/10/14   kab     ML1-RF interface changes for IRAT to support Single Rx
11/20/14   php     ML1-RF interface changes for IRAT to support HORxD/other fixes
10/20/14   vb      ASDiv immediate script support
09/30/14   kab     RF-ML1 Interface changes for Scouting APIs, FW Triggered RF Tune
09/09/14   svi     Interface changes for supporting Single Rx Tune and
                   higher order diversity.
08/18/14   kab     TxPL changes to support SGLTE Tx power backoff with Coex.
08/15/14   st      Alt Path Override Support
08/11/14   jf      Return timestamp at end of RX_tune/wakeup to ML1
08/12/14   jf      MCL LTE support
07/31/14   ndb     Added support for extended EARFCN for LTE
06/13/14   kab     Add 3xCA support
05/05/14   svi     Moved TXPLL update to Core Task
05/05/14   svi     Re-Organized the file structure
04/20/14   gvn     S-LTE Support
02/04/14   bsh     Porting CL 5493095
03/12/14   bsh     Adding NLIC support
02/26/14   kab     Added support for Slot1/SRS Scheduling thru RF FED LTE Task
02/04/14    pv     Removed support for FBRx Gain error from RFA and moved to RFLM.
02/04/14    pv     Added Support for FBRx Gain error update MSG Router command
01/23/14   sbm     MCS msgr interface changes to support SGLTE Tx power backoff
07/29/13   as      Added support for relative backoff on SAR limit based on 
                   TDD-LTE ULDL configuration
07/26/13   pl      Added support for setting ant switch position at enter mode and wakeup
07/18/13    pv     Added support for separate TDD Tx and Rx ASM scripts.
07/18/13   pl      Update interface to match with common AsDIV driver
07/17/13   pl      Added MSG support for Asdiv abort
06/06/13   pv      Added support for multple TDD ASM scripts.
06/05/13   php     Add MSG support for Ant Switch Diversity
04/08/13   pl      L2L IRAT abort support
03/15/13   gvn     Update sgsar, coex_WWAN, coex_WLAN limits using MSGR
01/29/13   aca     Tuner update message support
01/15/10   pl      redefine L2L IRAT message to support multiple SCELL
12/18/12   gvn     Change the structure passed to L1 during request to get Min Power. 
                   Only pass Min Power instead of entire Tx params structure.
12/12/12   gvn     Band Specific Min power NV support (Merge from Nikel)
11/15/12   adk     Backed out Qtuner code
11/09/12   pl      moved L2L IRAT interface to msg file
11/09/12   pl      Add CA L2L IRAT interface
11/08/12   adk     Featurized Qtuner code (FEATURE_RF_HAS_QTUNER)
10/31/12   pl      Change MAX CELL support to 2
09/14/12   jf      Qtuner LTE added flag for TDD bands
09/14/12   jf      Qtuner LTE added flag for TDD bands
08/27/12   jf      Add Qtuner interface msg between ML1 and RFSW 
08/27/12   jf      Add Qtuner interface msg between ML1 and RFSW 
05/23/12   as      LTE Tx power limit for SGLTE SAR and CoEx
05/14/12   pl      Revise interface to support Carrier Aggregation
04/17/12   jf      Fix to remove ML1 to call RFSW for Quiet Mode 
03/19/12   aca     API change to support XO correction during Tx PLL tune
03/13/12   can     Added FA_RF_LTE_INVALID_CAL_DATA.
10/12/11   pl      Support Offline DRX
04/28/11   pl      Adding support for RX/TX PLL adjustments
02/18/11   pl      remove duplicated  txlm buffer field, make naming consistent
02/09/11   pl      Update to add RXLM and TXLM support
01/13/11   aca	   L2L Message routing interface
07/01/10   jyu     Added support to power collapse   
06/23/10   jyu     Added support to manage rx chain 
05/03/10   can     HDET optimization.
10/14/09   can     Changes to support the L1 Interface.
10/13/09   mkv     Added MAX RF command ID to set length to uint16
10/07/09   can     Bug fix. 
04/02/09   can     Initial Checkin

===========================================================================*/

#ifndef RF_LTE_MSG_H
#define RF_LTE_MSG_H

/*===========================================================================

                  INCLUDE FILES

===========================================================================*/

#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "lm_types.h"


/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/


/*---------------------------------------------------------------*/
/*                    MSGR SUPERVISORY ID'S                      */
/*---------------------------------------------------------------*/  

/*!>
  @brief
  This module contains MSG IDS for the different MSGR
  messages and commands to be dispatched and
  received by RFLTE
  @{
*/

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to enter LTE mode of operation CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_ENTER_MODE                0xA1

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to exit LTE mode of operation CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_EXIT_MODE                 0xA2

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to configure RX CMD and RSP (this includes tuning RX device)
 */
#define RFA_RF_LTE_MSG_ID_FDD_RX_CONFIG             0xA3

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to configure TX CMD and RSP (this includes tuning TX device)
 */
#define RFA_RF_LTE_MSG_ID_FDD_TX_CONFIG             0xA4

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to configure TX CMD and RSP (this includes tuning TX device)
 */
#define RFA_RF_LTE_MSG_ID_TDD_RX_CONFIG             0xA5

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to disable TX CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_TDD_TX_CONFIG             0xA6

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to disable TX CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_FDD_TX_DISABLE            0xA7

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to disable TX CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_TDD_TX_DISABLE            0xA8

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to go to sleep CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_SLEEP                     0xA9

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to wake up CMD and RSP
 */ 
#define RFA_RF_LTE_MSG_ID_WAKEUP                    0xAA

/*-----------------------------------------------------------------------*/
/*! @brief ID to querry whether RF is sleeping CMD and RSP
 */
#define RFA_RF_LTE_MSG_ID_RF_IS_SLEEPING            0xAB

/*-----------------------------------------------------------------------*/
/*! @brief ID to indicate TX is ON CMD
 */
#define RFA_RF_LTE_MSG_ID_TX_ON                     0xAD

/*-----------------------------------------------------------------------*/
/*! @brief ID to request L2L measurement
 */
#define RFA_RF_LTE_MSG_ID_L2L_BUILD_SCRIPTS         0xAE

/*-----------------------------------------------------------------------*/
/*! @brief ID to request L2L measurement
 */
#define RFA_RF_LTE_MSG_ID_PLL_ADJUST                0xAF

/*-----------------------------------------------------------------------*/
/*! @brief ID to request connection mode status
 */
#define RFA_RF_LTE_MSG_ID_CONNECT_MODE              0xB0

/*-----------------------------------------------------------------------*/
/*! @brief ID to Update TXPL parameters
 */
#define RFA_RF_LTE_MSG_ID_TXPL_PARAM_UPDATE         0xB1

/*-----------------------------------------------------------------------*/
/*! @brief ID to set TX power limit
 */
#define RFA_RF_LTE_MSG_ID_SET_TX_PLIMIT	            0xB2

/*-----------------------------------------------------------------------*/
/*! @brief ID to get UE min Tx power limit
 */
#define RFA_RF_LTE_MSG_ID_GET_UE_MIN_POWER          0xB3

/*-----------------------------------------------------------------------*/
/*! @brief ID for ML1 ticks for ant tuning 
*/
#define RFA_RF_LTE_MSG_ID_ANT_TUNER_TICK            0xB4

/*-----------------------------------------------------------------------*/
/*! @brief ID for Ant SW Req from ML1 
*/
#define RFA_RF_LTE_MSG_ID_ANT_SW                    0xB5

/*-----------------------------------------------------------------------*/
/*! @brief ID for Ant SW Req from ML1 
*/
#define RFA_RF_LTE_MSG_ID_ANT_SW_ABORT              0xB6

/*-----------------------------------------------------------------------*/
/*! @brief 
*/
#define  RFA_RF_LTE_MSG_ID_TDD_ASM_SCRIPT_UPDATE    0xB7

/*-----------------------------------------------------------------------*/
/*! @brief Set the Timer for FED task for TxAGC
*/
#define RFA_RF_LTE_MSG_ID_FED_SET_TIMER             0xB8

/*-----------------------------------------------------------------------*/
/*! @brief 
*/
#define  RFA_RF_LTE_MSG_ID_FBRX_GAIN_ERR_UPDATE     0xB9

/*-----------------------------------------------------------------------*/
/*! @brief Schedule Wakeup for FED task for TxAGC
*/
#define RFA_RF_LTE_MSG_ID_FED_SCHEDULER_WAKEUP      0xBA

/*-----------------------------------------------------------------------*/
/*! @brief Schedule Wakeup for FED task for PRACH TxAGC
*/
#define RFA_RF_LTE_MSG_ID_FED_PRACH_WAKEUP          0xBB

/*-----------------------------------------------------------------------*/
/*! @brief Schedue PCMAX Update to Core Task
*/
#define RFA_RF_LTE_MSG_ID_P_CMAX_UPDATE             0xBC

/*-----------------------------------------------------------------------*/
/*! @brief 
*/
#define RFA_RF_LTE_MSG_ID_UL_POWER_UPDATE           0xBD

/*-----------------------------------------------------------------------*/
/*! @brief 
*/
#define  RFA_RF_LTE_MSG_ID_PATH_SEL_OVERRIDE        0xBE

/*-----------------------------------------------------------------------*/
/*! @brief ID for RF to do Rx1 Chain config
 */
#define RFA_RF_LTE_MSG_ID_RX1_CHAIN_CONFIG          0xBF

/*-----------------------------------------------------------------------*/
/*! @brief Schedule TxPLL Script Update to DM to Core Task
*/
#define RFA_RF_LTE_MSG_ID_TXPLL_SCRIPTS_UPDATE      0xC0

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*! @brief 
*/
#define  RFA_RF_LTE_MSG_ID_ALT_PATH_OVERRIDE        0xC1

/*-----------------------------------------------------------------------*/
/*! @brief ID for Script Based Rx Tune 
*/
#define RFA_RF_LTE_MSG_ID_RX_SCRIPT_BASED_CONFIG    0xC2

/*-----------------------------------------------------------------------*/
/*! @brief ID for Script Based Tx Tune 
*/
#define RFA_RF_LTE_MSG_ID_TX_SCRIPT_BASED_CONFIG    0xC3

/*-----------------------------------------------------------------------*/
/*! @brief ID for getting Single Chain Support
*/
#define RFA_RF_LTE_MSG_ID_GET_SINGLE_CHAIN_SUPPORT  0xC4

/*-----------------------------------------------------------------------*/
/*! @brief ID for determining Re-Tune requirement of already tuned Cells
*/
#define RFA_RF_LTE_MSG_ID_CHECK_CONFIG              0xC5

/*-----------------------------------------------------------------------*/
/*! @brief Max Number of MSG ID
*/
#define RFA_RF_LTE_MSG_MAX_ID                       0xFF

/*! @} */

/*-----------------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*                 OTHER CONSTANT DEFINITIONS                    */
/*---------------------------------------------------------------*/

/*!>
  @brief
  This module contains the constant definitions used in RFLTE
  @{
*/

/*-----------------------------------------------------------------------*/
/*! @brief Max Number of Cells Supported
 */
#define RFA_RF_LTE_MAX_CELL_SUPPORTED               3

/*-----------------------------------------------------------------------*/
/*! @brief applies to Simultaneous Transmit SAR power limit
*/
#define RFA_RF_LTE_ST_SAR_PLIMIT_MASK               0x01

/*-----------------------------------------------------------------------*/
/*! @brief applies to WWAN Coexistence power limit
*/
#define RFA_RF_LTE_WWAN_COEX_PLIMIT_MASK            0x02 

/*-----------------------------------------------------------------------*/
/*! @brief applies to WCN (WLAN & BT) Coexistence power limit
*/
#define RFA_RF_LTE_WCN_COEX_PLIMIT_MASK             0x04

/*-----------------------------------------------------------------------*/
/*! @brief applies to WWAN Coexistence subframe level power limit
*/
#define RFA_RF_LTE_SUBFRAME_COEX_PLIMIT_MASK        0x08

/*-----------------------------------------------------------------------*/
/*! @brief defines the bit mask on the path bit mask.
*/

/* Bit Mask per Path 
   <Reserved 15:8> | DRx3 | DRx2 | DRx | PRx | <Reserved 3:1> | Tx | 
*/
#define RFA_RF_LTE_TX_PATH_BIT_MASK                 0x01
#define RFA_RF_LTE_PRX_PATH_BIT_MASK                0x10
#define RFA_RF_LTE_DRX_PATH_BIT_MASK                0x20
#define RFA_RF_LTE_DRX2_PATH_BIT_MASK               0x40
#define RFA_RF_LTE_DRX3_PATH_BIT_MASK               0x80

/*! @} */

/*-----------------------------------------------------------------------*/
/*! @brief Defines 
*/
/*  Single Chain Support Bit Mask           
    Rx_SC_Bit_Mask [8 bits] | Tx_SC_Bit_Mask [8 bits]
*/
#define RFA_RF_LTE_SINGLE_CHAIN_TX_BM_START_BIT  0
#define RFA_RF_LTE_SINGLE_CHAIN_RX_BM_START_BIT  8

/*-----------------------------------------------------------------------*/



/*===========================================================================

                  EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*---------------------------------------------------------------*/
/*                    RF Task LTE messages                       */
/*---------------------------------------------------------------*/

/*!>
  @brief
  This module contains the constant definitions of the symbols
  of the different MSGR commands and the associated Payloads.

  MSGR Commands in the LTE SUBSYSTEM can be of three types:
  1. Messages received by the LTE SUBSYSTEM as REQUESTS
  2. Messages sent back by the LTE SUBSYSTEM as CONFIRMATION to REQUESTS
  3. Messages received by the LTE SUBSYSTEM as INDICATORS

  The enums defined below are never called by name.
  They just define the symbols.
  
  @{
*/

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This enum defines the symbols of the MSGR Message ID's to be sent back
  by the LTE Task (LTE SUBSYSTEM) as confirmation responses

  The enum defined below is never called by name.
  It just define the confirmation symbols.
  
*/
enum
{
   /*! Enter LTE mode message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, ENTER_MODE, RFA_RF_LTE_MSG_ID_ENTER_MODE, rfa_rf_lte_enter_mode_cnf_s),

   /*! Exit LTE mode message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, EXIT_MODE, RFA_RF_LTE_MSG_ID_EXIT_MODE, rfa_rf_lte_exit_mode_cnf_s),

   /*! Configure FDD RX message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, FDD_RX_CONFIG, RFA_RF_LTE_MSG_ID_FDD_RX_CONFIG, rfa_rf_lte_fdd_rx_config_cnf_s),

   /*! Configure FDD TX message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, FDD_TX_CONFIG, RFA_RF_LTE_MSG_ID_FDD_TX_CONFIG, rfa_rf_lte_fdd_tx_config_cnf_s),

   /*! Configure TDD RX message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, TDD_RX_CONFIG, RFA_RF_LTE_MSG_ID_TDD_RX_CONFIG, rfa_rf_lte_tdd_rx_config_cnf_s),

   /*! Config TDD TX  message*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, TDD_TX_CONFIG, RFA_RF_LTE_MSG_ID_TDD_TX_CONFIG, rfa_rf_lte_tdd_tx_config_cnf_s),

   /*! Disable TDD TX disable message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, FDD_TX_DISABLE, RFA_RF_LTE_MSG_ID_FDD_TX_DISABLE, rfa_rf_lte_fdd_tx_disable_cnf_s),

   /*! Disable TDD TX disable message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, TDD_TX_DISABLE, RFA_RF_LTE_MSG_ID_TDD_TX_DISABLE, rfa_rf_lte_tdd_tx_disable_cnf_s),

   /*! RF sleep message*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, SLEEP, RFA_RF_LTE_MSG_ID_SLEEP, rfa_rf_lte_sleep_cnf_s),

   /*! RF wakeup message*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, WAKEUP, RFA_RF_LTE_MSG_ID_WAKEUP, rfa_rf_lte_wakeup_cnf_s),

   /*! RF is sleeping querry*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, RF_IS_SLEEPING, RFA_RF_LTE_MSG_ID_RF_IS_SLEEPING, rfa_rf_lte_rf_is_sleeping_cnf_s),

   /*! RF TX_ON confirmation message*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, TX_ON, RFA_RF_LTE_MSG_ID_TX_ON, rfa_rf_lte_tx_on_cfn_s),   
   
   /*! RF L2L measurement request message confirmation*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, L2L_BUILD_SCRIPTS, RFA_RF_LTE_MSG_ID_L2L_BUILD_SCRIPTS, rfa_rf_lte_l2l_build_scripts_cnf_s),

   /*! RF PLL adjustment request message confirmation*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, PLL_ADJUST, RFA_RF_LTE_MSG_ID_PLL_ADJUST, rfa_rf_lte_pll_adjust_cnf_s),

   /*! UE min Tx power limit message confirmation */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, GET_UE_MIN_POWER, RFA_RF_LTE_MSG_ID_GET_UE_MIN_POWER, rfa_rf_lte_get_ue_min_power_cnf_s),

   /*! Antenna switch request confirmation */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, ANT_SW, RFA_RF_LTE_MSG_ID_ANT_SW, rfa_rf_lte_ant_sw_cnf_s),

   /*! Rx1 chain config LTE mode message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, RX1_CHAIN_CONFIG, RFA_RF_LTE_MSG_ID_RX1_CHAIN_CONFIG, rfa_rf_lte_rx1_chain_config_cnf_s),
   
   /*! ID for Script Based Rx Tune  */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, RX_SCRIPT_BASED_CONFIG, RFA_RF_LTE_MSG_ID_RX_SCRIPT_BASED_CONFIG, rfa_rf_lte_rx_script_based_config_cnf_s),

   /*! ID for Script Based Tx Tune. */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, TX_SCRIPT_BASED_CONFIG, RFA_RF_LTE_MSG_ID_TX_SCRIPT_BASED_CONFIG, rfa_rf_lte_tx_script_based_config_cnf_s),

   /*! ID to obtain Single Chain Support */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, GET_SINGLE_CHAIN_SUPPORT, RFA_RF_LTE_MSG_ID_GET_SINGLE_CHAIN_SUPPORT, rfa_rf_lte_get_single_chain_support_cnf_s),

   /*! Identify Re-Tune requirement for already tuned Cells. */
   MSGR_DEFINE_UMID(RFA, RF_LTE, CNF, CHECK_CONFIG, RFA_RF_LTE_MSG_ID_CHECK_CONFIG, rfa_rf_lte_check_config_cnf_s),

};/* Confirmation Resposes */


/*-----------------------------------------------------------------------*/
/*!
  @brief
  This enum defines the symbols of the MSGR Message ID's to be received
  by the LTE Task (LTE SUBSYSTEM) as requests or indicators

  The enum defined below is never called by name.
  It just define the request / indicator symbols.
  
*/
enum
{
   /*! Enter LTE mode message, payload is the device type, sent by L1 */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, ENTER_MODE, RFA_RF_LTE_MSG_ID_ENTER_MODE, rfa_rf_lte_enter_mode_req_s),

   /*! Exit LTE mode message, payload is the device type, sent by L1 */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, EXIT_MODE, RFA_RF_LTE_MSG_ID_EXIT_MODE, rfa_rf_lte_exit_mode_req_s),

   /*! Configure FDD RX message, payload is defined in lte_config_s, sent by L1 */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, FDD_RX_CONFIG, RFA_RF_LTE_MSG_ID_FDD_RX_CONFIG, rfa_rf_lte_fdd_rx_config_req_s),

   /*! Configure FDD TX message, payload is defined in lte_config_s, sent by L1 */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, FDD_TX_CONFIG, RFA_RF_LTE_MSG_ID_FDD_TX_CONFIG, rfa_rf_lte_fdd_tx_config_req_s),

   /*! Configure TDD message, payload is defined in lte_config_s, sent by L1 */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, TDD_RX_CONFIG, RFA_RF_LTE_MSG_ID_TDD_RX_CONFIG, rfa_rf_lte_tdd_rx_config_req_s),

   /*! Disable TDD TX Enable message*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, TDD_TX_CONFIG, RFA_RF_LTE_MSG_ID_TDD_TX_CONFIG, rfa_rf_lte_tdd_tx_config_req_s),

   /*! Disable TDD TX disable message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, FDD_TX_DISABLE, RFA_RF_LTE_MSG_ID_FDD_TX_DISABLE, rfa_rf_lte_fdd_tx_disable_req_s),

   /*! Disable TDD TX disable message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, TDD_TX_DISABLE, RFA_RF_LTE_MSG_ID_TDD_TX_DISABLE, rfa_rf_lte_tdd_tx_disable_req_s),

   /*! Sleep message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, SLEEP, RFA_RF_LTE_MSG_ID_SLEEP, rfa_rf_lte_sleep_req_s),

   /*! Wakeup message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, WAKEUP, RFA_RF_LTE_MSG_ID_WAKEUP, rfa_rf_lte_wakeup_req_s),

   /*! Sleep querry message, */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, RF_IS_SLEEPING, RFA_RF_LTE_MSG_ID_RF_IS_SLEEPING, rfa_rf_lte_rf_is_sleeping_req_s),

   /*! TX ON Indicator message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, TX_ON, RFA_RF_LTE_MSG_ID_TX_ON, rfa_rf_lte_tx_on_ind_s),

   /*! L2L Measurement request message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, L2L_BUILD_SCRIPTS, RFA_RF_LTE_MSG_ID_L2L_BUILD_SCRIPTS, rfa_rf_lte_l2l_build_scripts_req_s),

   /*! PLL adjustment request message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, PLL_ADJUST, RFA_RF_LTE_MSG_ID_PLL_ADJUST, rfa_rf_lte_pll_adjust_req_s),

    /*! Notify connection mode request message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, CONNECT_MODE, RFA_RF_LTE_MSG_ID_CONNECT_MODE, rfa_rf_lte_connect_mode_ind_s),

   /*! Notify connection mode request message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, TXPL_UPDATE, RFA_RF_LTE_MSG_ID_TXPL_PARAM_UPDATE, rfa_rf_lte_txpl_update_ind_s),

   /*! Set TX power limit for SGLTE, payload is TX power limit in dB10, sent by MCS */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, SET_TX_PLIMIT, RFA_RF_LTE_MSG_ID_SET_TX_PLIMIT, rfa_rf_lte_set_tx_plimit_ind_s),

   /*! UE min Tx power limit request message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, GET_UE_MIN_POWER, RFA_RF_LTE_MSG_ID_GET_UE_MIN_POWER, rfa_rf_lte_get_ue_min_power_s),

   /*! ML1 provide the RFSW ticks for ant tuning */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, ANT_TUNER_TICK, RFA_RF_LTE_MSG_ID_ANT_TUNER_TICK, rfa_rf_lte_ant_tuner_tick_ind_s),

   /*! Antenna switch request message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, ANT_SW, RFA_RF_LTE_MSG_ID_ANT_SW, rfa_rf_lte_ant_sw_s),

   /*! Antenna switch abort message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, ANT_SW_ABORT, RFA_RF_LTE_MSG_ID_ANT_SW_ABORT, rfa_rf_lte_ant_sw_abort_ind_s),

   /*! Set TX power limit for SGLTE, payload is TX power limit in dB10, sent by MCS */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, TDD_ASM_SCRIPT_UPDATE, RFA_RF_LTE_MSG_ID_TDD_ASM_SCRIPT_UPDATE, rfa_rf_lte_dynamic_asm_update_ind_s),

    /* Set Timer to Wake Up FED Task */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, FED_SET_TIMER, RFA_RF_LTE_MSG_ID_FED_SET_TIMER, rfa_rf_lte_fed_ind_s),

     /* Commit CCS buffers for slots other than Slot0 */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, FED_SCHEDULER_WAKEUP, RFA_RF_LTE_MSG_ID_FED_SCHEDULER_WAKEUP, rfa_rf_lte_fed_ind_s),
     
    /* Commit Tx Path Off writes for Prach */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, FED_PRACH_WAKEUP , RFA_RF_LTE_MSG_ID_FED_PRACH_WAKEUP  , rfa_rf_lte_fed_ind_s),

   /*! FBRx Gain Error update in dB10, sent by LTE FED */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, P_CMAX_UPDATE, RFA_RF_LTE_MSG_ID_P_CMAX_UPDATE, rfa_rf_lte_p_cmax_update_ind_s),

    /*UL power every SF*/
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, UL_POWER_UPDATE, RFA_RF_LTE_MSG_ID_UL_POWER_UPDATE, rfa_rf_lte_ul_power_update_s),

   /*! Force TX/Rx Path for a given LTE Band */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, PATH_SEL_OVERRIDE, RFA_RF_LTE_MSG_ID_PATH_SEL_OVERRIDE, rfa_rf_lte_path_sel_override_update_ind_s),
   
   /*! Rx1 chain config LTE mode message */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, RX1_CHAIN_CONFIG, RFA_RF_LTE_MSG_ID_RX1_CHAIN_CONFIG, rfa_rf_lte_rx1_chain_config_req_s),

   /*! TxPLL Script generation and update to DM */
   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, TXPLL_SCRIPTS_UPDATE, RFA_RF_LTE_MSG_ID_TXPLL_SCRIPTS_UPDATE, rfa_rf_lte_txpll_scripts_update_s), 

   MSGR_DEFINE_UMID(RFA, RF_LTE, IND, ALT_PATH_OVERRIDE, RFA_RF_LTE_MSG_ID_ALT_PATH_OVERRIDE, rfa_rf_lte_ftm_alt_path_oride_ind_s),
   
   /*! ID for Script Based Rx Tune  */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, RX_SCRIPT_BASED_CONFIG, RFA_RF_LTE_MSG_ID_RX_SCRIPT_BASED_CONFIG, rfa_rf_lte_rx_script_based_config_req_s),

   /*! ID for Script Based Tx Tune. */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, TX_SCRIPT_BASED_CONFIG, RFA_RF_LTE_MSG_ID_TX_SCRIPT_BASED_CONFIG, rfa_rf_lte_tx_script_based_config_req_s),

   /*! ID to obtain Single Chain Support */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, GET_SINGLE_CHAIN_SUPPORT, RFA_RF_LTE_MSG_ID_GET_SINGLE_CHAIN_SUPPORT, rfa_rf_lte_get_single_chain_support_req_s),
   
   /*! Identify Re-Tune requirement for already tuned Cells. */
   MSGR_DEFINE_UMID(RFA, RF_LTE, REQ, CHECK_CONFIG, RFA_RF_LTE_MSG_ID_CHECK_CONFIG, rfa_rf_lte_check_config_req_s),

};/* Requests and Indicators */


/*! @} */

/*-----------------------------------------------------------------------*/


/*---------------------------------------------------------------*/
/*                EXTERNAL STRUCTURES AND ENUMS                  */
/*---------------------------------------------------------------*/

/*!>
  @brief
  This module contains structures and enums that define the
  payloads of the different MSGR commands to be received
  and dispatched back by the LTE SUBSYSTEM

  @{
*/

/*-----------------------------------------------------------------------*/
typedef enum
{
  RFM_L2L_TUNE_AWAY_TUNE_BACK_SCRIPT,
  RFM_L2L_TUNE_AWAY_SCRIPT,
  RFM_L2L_ABORT_SCRIPT,
  RFM_L2L_INVALID_TUNE_SCRIPT
} rfm_l2l_setup_and_build_scripts_type;

/*! @todo: This might need to be moved to a common file so that L1 can see it 
           and it might be shared with other targets.*/

/*-----------------------------------------------------------------------*/
typedef enum
{
  RFA_RF_LTE_SUCCESS=1,
  RFA_RF_LTE_STATE_MACHINE_ERROR=2,
  RFA_RF_LTE_EXECUTION_ERROR=3,
  RFA_RF_LTE_INVALID_CAL_DATA=4,
  RFA_RF_LTE_CMD_OUTCOME_MAX=5,
} rfa_rf_lte_cmd_outcome_t;

/*-----------------------------------------------------------------------*/
/* This data structure can be used for holding the RF 
Tx related parameters to be updated to ML1*/
typedef struct
{  
  int16   rf_min_tx_power;
} rfa_rf_lte_tx_config_cnf_params_type;

/*-----------------------------------------------------------------------*/
/*!> Structure is 32 bit alligned to save memory */
typedef struct
{
   rfm_device_enum_type device;
   uint8                 scenario_val;
   boolean               is_scenario_valid;
   lm_handle_type       lm_buffer_index;
   rfm_tuner_handle_type tuner_handle;
} rfa_rf_chain_param_type;

/*-----------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_earfcn_type            channel;
  rfcom_lte_bw_type bw;
} rfa_rf_rx_param_type;

/*-----------------------------------------------------------------------*/
/* Indicates TDD UL/DL configurations as specified in 
the 3GPP 36.211 V11.3.0, Table 4.2-2 */
typedef enum
{
  RFA_RF_LTE_TDD_UL_DL_CFG_0 = 0,
  RFA_RF_LTE_TDD_UL_DL_CFG_1 = 1,
  RFA_RF_LTE_TDD_UL_DL_CFG_2 = 2,
  RFA_RF_LTE_TDD_UL_DL_CFG_3 = 3,
  RFA_RF_LTE_TDD_UL_DL_CFG_4 = 4,
  RFA_RF_LTE_TDD_UL_DL_CFG_5 = 5,
  RFA_RF_LTE_TDD_UL_DL_CFG_6 = 6,
  RFA_RF_LTE_TDD_UL_DL_CFG_INVALID = 0xFF,  
} rfa_rf_lte_tdd_ul_dl_cfg_type;

/*-----------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_earfcn_type            channel;
  rfcom_lte_bw_type bw;
  rfa_rf_lte_tdd_ul_dl_cfg_type tdd_ul_dl_cfg;  /* Cfg#0, Cfg#1....Cfg#6*/
  int32             rx_freq_error;  
} rfa_rf_tx_param_type;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  uint8 ant_pos;
} rfa_rf_lte_enter_mode_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  uint8 ant_pos;
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_enter_mode_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_rx_param_type rx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_fdd_rx_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_rx_param_type rx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;
  boolean                  antenna_tuner_present;
  uint64 ustmr_timestamp;
} rfa_rf_lte_fdd_rx_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_rx_param_type rx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_tdd_rx_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_rx_param_type rx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;
  boolean                  antenna_tuner_present;
  uint64 ustmr_timestamp;
} rfa_rf_lte_tdd_rx_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_tx_param_type tx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_fdd_tx_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_tx_param_type tx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_tx_config_cnf_params_type tx_config_cnf_params;
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_fdd_tx_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_tx_param_type tx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_tdd_tx_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_tx_param_type tx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_tx_config_cnf_params_type tx_config_cnf_params;
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_tdd_tx_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_fdd_tx_disable_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_fdd_tx_disable_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_tdd_tx_disable_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_tdd_tx_disable_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];   
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_sleep_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];   
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_sleep_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  uint8 ant_pos;
} rfa_rf_lte_wakeup_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  uint8 ant_pos;
  rfa_rf_lte_cmd_outcome_t req_result;
  uint64 ustmr_timestamp;
} rfa_rf_lte_wakeup_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
} rfa_rf_lte_exit_mode_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint16 path_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_exit_mode_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32                 carrier_idx;
} rfa_rf_lte_rf_is_sleeping_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  boolean                  rf_is_sleeping_val;
  /*echo back the input*/
  uint32                 carrier_idx;
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_rf_is_sleeping_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32                 carrier_idx;
  rfcom_lte_earfcn_type channel;
} rfa_rf_lte_tx_on_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  /*echo back the input*/
  uint32                 carrier_idx;
  rfcom_lte_earfcn_type channel;
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_tx_on_cfn_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type  hdr;
  uint8                 sub_frame;
  int16                 tx_power;
  uint8                 num_rbs;
  uint8                 rb_start_index;

} rfa_rf_lte_ul_power_update_s;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This structure defines the Payload for the MSGR MDS ID
  RFA_RF_LTE_MSG_ID_TXPLL_SCRIPTS_UPDATE
  
*/

typedef struct
{

  /*!> Carrier Index of the Tx Cell */
  uint32 carrier_idx; 

  /*!> Rx Freq Error for the Tx Cell */  
  int32 rx_freq_error_in_hz;

  /*!> TxPPL SBI Script Buffer Ptr for the Tx Cell */  
  void *txpll_sbi_script_buf_ptr;

} rfa_rf_lte_txpll_scripts_update_params_s;

/*-----------------------------------------------------------------------*/
/*!
  @brief
  This structure defines the Header and Payload for the MSGR MDS ID
  RFA_RF_LTE_MSG_ID_TXPLL_SCRIPTS_UPDATE
  
*/
typedef struct
{

  /*!> MSG header */
  msgr_hdr_struct_type  hdr;

  /*!> MSG Params */  
  rfa_rf_lte_txpll_scripts_update_params_s txpll_params;

} rfa_rf_lte_txpll_scripts_update_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
   rfcom_device_enum_type device; 
   /*!<  Device on which LTE will be tuned  */

   rfcom_device_enum_type div_device; 
   /*!<  Diversity Device on which LTE will be tuned  */

   uint32                band;
   /*!< Specify technology band*/

   rfcom_lte_earfcn_type                rx_channel;
   /*!<  Specify technology DL channel */

   rfcom_lte_bw_type     bw;  
   /*!<  Specify technology channel bandwidth, 
         populate only for LTE  */

   lm_handle_type        rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx, 
         populate only for target tech */

   lm_handle_type        rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx, 
         populate only for target tech */

   uint16                div_en;
   /*!<  source diversity enable/disable indicator  */  
} rfm_meas_lte_rx_chan_param_type;

/*-----------------------------------------------------------------------*/
typedef struct 
{
   rfcom_device_enum_type device; 
   /*!<  Source Device on which LTE will be tuned  */

   rfcom_device_enum_type div_device; 
   /*!<  Source Diversity Device on which LTE will be tuned  */

   rfcom_device_enum_type div2_device; 
   /*!<  Source Diversity2 Device on which LTE will be tuned  */

   rfcom_device_enum_type div3_device; 
   /*!<  Source Diversity3 Device on which LTE will be tuned  */

   rfcom_mode_enum_type  source_tech;
   /*!<  Specify source technology for IRAT */

   uint32                source_band;
   /*!< Specify source technology band*/

   rfcom_lte_earfcn_type                source_rx_channel;
   /*!<  Specify source technology DL channel */

   /*! @brief  source diversity enable/disable indicator  */
   uint16                source_div_en;

   rfcom_lte_bw_type     source_bw;  
   /*!<  Specify source technology channel bandwidth  */

   uint32                source_buffer_index; 
   /*!<  Buffer index for clean-up scripts.*/

   lm_handle_type        source_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        source_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for drx*/

   lm_handle_type        source_rxlm_buf_idx_ant2;
   /*!<  LM buffer index for drx2*/

   lm_handle_type        source_rxlm_buf_idx_ant3;
   /*!<  LM buffer index for drx3*/

   rfcom_device_enum_type target_device; 
   /*!<  Target Device on which LTE will be tuned  */

   rfcom_device_enum_type target_div_device; 
   /*!<  Target Diversity Device on which LTE will be tuned  */
      
   rfcom_mode_enum_type  target_tech;
   /*!<  Specify target technology for IRAT */

   uint32                target_band;
   /*!<  Specify target technology band */

   rfcom_lte_earfcn_type                target_rx_channel;
   /*!<  Specify source technology DL channel */

   /*! @brief  target diversity enable/disable indicator  */
   uint16                target_div_en;

   rfcom_lte_bw_type     target_bw;  
   /*!<  Specify source technology channel bandwidth  */

   uint32                target_buffer_index; 
   /*!<  Buffer index for clean-up scripts.*/   

   lm_handle_type        target_rxlm_buf_idx_ant0;
   /*!<  LM buffer index for prx*/

   lm_handle_type        target_rxlm_buf_idx_ant1;
   /*!<  LM buffer index for prx*/

   rfm_l2l_setup_and_build_scripts_type tune_script_type;
   /*!<   Type of IRAT script requested*/

   uint32                num_src_scell;
   /*!<   Number of SCELL configured at source tech */

   rfm_meas_lte_rx_chan_param_type src_scell_param[(RFA_RF_LTE_MAX_CELL_SUPPORTED-1)];
   /*!<   SCELL configuration parameters */

   uint8 src_chain_mask[RFA_RF_LTE_MAX_CELL_SUPPORTED];
   /*!<  Chain mask indicating active Rx chains*/

   uint16 single_chain_bitmask[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
   /*!<  Rx_SC_Bit_Mask [8 bits] | Tx_SC_Bit_Mask [8 bits] */ 

   uint32                active_carrier_idx_mask; 
   /*!<  Carrier index mask: which carriers is the request for on source.*/ 

   boolean  is_l2l_wideband_meas;
   /*!<  Flag to convey wideband L2L measurement.*/ 

} rfm_l2l_setup_and_build_scripts_param_type;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfm_l2l_setup_and_build_scripts_param_type l2l_build_script_request_params;
} rfa_rf_lte_l2l_build_scripts_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  /*echo back the input*/
  rfm_l2l_setup_and_build_scripts_param_type l2l_build_script_request_params;
  // result of the request
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_l2l_build_scripts_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  boolean tx_pll_adjust_flag;
  int64   pll_freq_adjust;
} rfa_rf_lte_pll_adjust_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_pll_adjust_params_s pll_adjust_params;
} rfa_rf_lte_pll_adjust_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_pll_adjust_params_s pll_adjust_params;
  // result of the request
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_pll_adjust_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  rfcom_device_enum_type device;
  boolean switch_init_mode;
  boolean param_update;
  boolean srs_update;
  uint8 mpr;
  uint8 ampr;
  uint8 ns_x;
  uint8 mod;
  uint8 rb_block_start_1;
  uint8 rb_block_start_2;
  uint8 rb_block_num;
  uint16 power_pcmax;
  uint16 power_pemax;
  int16  dtpl;
  int16  mtpl;
  int16  dtpl_coex;
  int16  mtpl_coex;
} rfa_rf_lte_txpl_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_txpl_params_s txpl_params;
} rfa_rf_lte_txpl_update_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  rfcom_device_enum_type device;
  boolean enable;
} rfa_rf_lte_connect_mode_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_connect_mode_params_s connect_mode_params;
} rfa_rf_lte_connect_mode_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  uint32 plimit_type_mask; /* Bit mask which tells which limits are being updated*/
  int16 sar_plimit; /* SAR Tx Power limit */  
  int16 coex_plimit; /* Coex desense Tx Power limit */
  int16 wcn_coex_plimit; /* WLAN and BT coex tx power liimit*/
  int16 subframe_coex_plimit;  /* this new limit is for subframe level backoff… so MCS will use this for SGLTE, G-SLTE and 1X-SLTE  and any other combinations in future that require subframe level backoff */
} rfa_rf_lte_sg_txpl_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_sg_txpl_params_s sg_txpl_params; 
} rfa_rf_lte_set_tx_plimit_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
} rfa_rf_lte_get_ue_min_power_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  int16   rf_min_tx_power; /* Units are in db10 */
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_get_ue_min_power_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  uint64 timestamp; 
  uint32 transaction_id; 
} rfa_rf_lte_transaction_hdr_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
	/* this transaction_id should be reset only during LTE exit and is maintained by ML1 */
  rfa_rf_lte_transaction_hdr_s tick_ind_params;
} rfa_rf_lte_ant_tuner_tick_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  rfm_device_enum_type device;
  /* Antenna switch position */
  uint32 pos;
  void* cb_func;
  void* cb_data;
  boolean immediate;
} rfa_rf_lte_ant_sw_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_ant_sw_params_s ant_sw_params;
} rfa_rf_lte_ant_sw_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_ant_sw_params_s ant_sw_params;
  /* Result of the request */
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_ant_sw_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  boolean config;
} rfa_rf_lte_rx1_chain_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  boolean config;
  rfa_rf_lte_cmd_outcome_t req_result;
} rfa_rf_lte_rx1_chain_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_ant_sw_params_s ant_sw_params;
} rfa_rf_lte_ant_sw_abort_ind_s;

/*-----------------------------------------------------------------------*/
typedef enum
{
  PORT_A = 0,/* this is the default setting */
  PORT_B = 1,/* this is the port B setting */
  PORT_MAX = 2,/* this is the Max allowed setting */
} rfa_rf_lte_port_state_type;

/*-----------------------------------------------------------------------*/
typedef enum
{
  RX_PORT = 0,/* this enables the Rx Port Update */
  TX_PORT = 1,/* this enables the Tx Port Update */
  RXTX_PORT = 2,/* this enables both Rx and Tx Port Update */
  RXTX_PORT_MAX = 3,
} rfa_rf_lte_rx_tx_chain_type;

/*-----------------------------------------------------------------------*/
typedef struct
{
  rfcom_lte_earfcn_type tx_channel; /* LTE  Tx channel to identify the band */
  rfcom_lte_earfcn_type rx_channel; /* LTE  Rx channel to identify the band */
  rfa_rf_lte_rx_tx_chain_type rx_tx_chain; /* State Update for Rx/Tx/Both */
  uint8 cell_idx; /* Cell Index */
  rfa_rf_lte_port_state_type tx_asm_port_state; /* The Tx port state needs to be preconfigured */
  rfa_rf_lte_port_state_type rx_asm_port_state; /* The Rx port state needs to be preconfigured */
} rfa_rf_lte_asm_update_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_asm_update_params_s lte_tdd_asm_params; 
} rfa_rf_lte_dynamic_asm_update_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  uint32              wake_up_time;   /* In ustmr time.*/
  uint32              sf_num;         /* Subframe number.*/
  uint32              cell_index;     /* Cell Index */
  void               *lte_dm_ptr;      /* LTE DM memory pointer*/
  msgr_umid_type      umid;  /* UMID MSG call to invoke*/
} rfa_rf_lte_fed_params_s;

/*-----------------------------------------------------------------------*/  
typedef struct
{
  msgr_hdr_struct_type     hdr;              /* MSGR Message Header */
  rfa_rf_lte_fed_params_s  fed_lte_params;    
} rfa_rf_lte_fed_ind_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  /*! */
  uint32 carrier_idx;
  /*! */
  uint8 subframe_num;
  /*! */
  rfa_rf_lte_txpl_params_s tpl_params;
  /*! */
  uint8 pa_swpt_backoff;

} rfa_rf_lte_p_cmax_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  rfa_rf_lte_p_cmax_params_s tx_params; 
} rfa_rf_lte_p_cmax_update_ind_s;

/*-----------------------------------------------------------------------*/
/* This structure is used to support forcing a Tx/Rx path on a given
   band irrespective of its Channel/BW mapping
   We support upto 254 splits on a given band */
typedef PACK(struct)
{
  /*! Defines the Carrier_index/Cell_idx */
  uint8 carrier_id;
  /*! Defines the LTE Band. Always provide the full-band value */
  uint8 band;
  /*! Defines the Tx Path index */
  uint8 tx_path_index;
  /*! Defines the PRx Path index */
  uint8 prx_path_index;

  /*! Defines the PRx Path index */
  uint8 drx_path_index;

} rfa_rf_lte_path_sel_override_params_s;

/*-----------------------------------------------------------------------*/
typedef PACK(struct)
{ 
  msgr_hdr_struct_type hdr;
  rfa_rf_lte_path_sel_override_params_s lte_path_sel_override_params;
} rfa_rf_lte_path_sel_override_update_ind_s; 

/*-----------------------------------------------------------------------*/
/* This structure is used to configure (tune\disable) single rx chain. */
typedef PACK(struct)
{
  /*! Defines the Carrier_info */
  //rflte_mc_carrier_info_type carrier_info;
  uint8 carrier_idx;
  rfa_rf_chain_param_type rx_chain_info;
  uint8 carrier_type;
  /*! Specifies chain to be enabled or disabled */
  boolean enable_chain;

} rfa_rf_lte_config_rx_chain_params_s;

/*-----------------------------------------------------------------------*/
typedef PACK(struct)
{ 
  msgr_hdr_struct_type hdr;
  rfa_rf_lte_config_rx_chain_params_s lte_rx_chain_params;
} rfa_rf_lte_config_rx_chain_update_ind_s; 

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;
  uint32 exit_carrier_idx_mask;

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

  /*  RF Cell Config Bit Mask                                                                                               *
   *  <Reserved 15:8> | DRX3 | DRX2 | DRX | PRX | <Reserved 3:1> | TX 
   */
  uint16 rf_cell_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Boolean to indicate if RF or FW should execute Trigger\Non-TRx writes. */
  boolean exectue_script;

  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_rx_param_type rx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];

}rfa_rf_lte_rx_script_based_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;
  uint32 exit_carrier_idx_mask;

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

  /*  RF Cell config Bit Mask                                                                                               *
   *  <Reserved 15:8> | DRX3 | DRX2 | DRX | PRX | <Reserved 3:1> | TX 
   */
  uint16 rf_cell_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Boolean to indicate if RF or FW should execute Trigger\Non-TRx writes. */
  boolean exectue_script;

  rfa_rf_chain_param_type prx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx2[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_chain_param_type drx3[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_rx_param_type rx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;

}rfa_rf_lte_rx_script_based_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  uint32 carrier_idx_mask;

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /*  RF Cell config Bit Mask                                                                                               *
   *  <Reserved 15:8> | DRX3 | DRX2 | DRX | PRX | <Reserved 3:1> | TX 
   */
  uint16 rf_cell_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  
  /* Boolean to indicate if RF or FW should execute Trigger\Non-TRx writes. */
  boolean exectue_script;  

  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_tx_param_type tx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];

}rfa_rf_lte_tx_script_based_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask;

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

  /*  RF Cell config Bit Mask                                                                                               *
   *  <Reserved 15:8> | DRX3 | DRX2 | DRX | PRX | <Reserved 3:1> | TX 
   */
  uint16 rf_cell_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
 
  /* Boolean to indicate if RF or FW should execute Trigger\Non-TRx writes. */
  boolean exectue_script;
 
  rfa_rf_chain_param_type tx[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_tx_param_type tx_param[RFA_RF_LTE_MAX_CELL_SUPPORTED];
  rfa_rf_lte_cmd_outcome_t req_result;

}rfa_rf_lte_tx_script_based_config_cnf_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  /*   Bit Mask per Path 
   *   <Reserved 15:8> | DRx3 | DRx2 | DRx | PRx | <Reserved 3:1> | Tx | 
   */
  uint16                       path_bitmask;
  rfa_rf_chain_param_type      tx;
  rfa_rf_chain_param_type      prx;
  rfa_rf_chain_param_type      drx;
  rfa_rf_chain_param_type      drx2;
  rfa_rf_chain_param_type      drx3;
  rfa_rf_tx_param_type         tx_param;
  rfa_rf_rx_param_type         rx_param;

}rfa_rf_lte_get_single_chain_support_params_s;
/*-----------------------------------------------------------------------*/
typedef struct
{

  msgr_hdr_struct_type   hdr;

  /* Current RF configuration info.*/
  rfa_rf_lte_get_single_chain_support_params_s current_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Requested RF configuration info.*/ 
  rfa_rf_lte_get_single_chain_support_params_s requested_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

}rfa_rf_lte_get_single_chain_support_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{

  msgr_hdr_struct_type   hdr;

  /* Current RF configuration info.*/
  rfa_rf_lte_get_single_chain_support_params_s current_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Requested RF configuration info.*/
  rfa_rf_lte_get_single_chain_support_params_s requested_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  
  
  rfa_rf_lte_cmd_outcome_t req_result;

}rfa_rf_lte_get_single_chain_support_cnf_s;


/*-----------------------------------------------------------------------*/
typedef struct
{
  /*   Bit Mask per Path 
   *   <Reserved 15:8> | DRx3 | DRx2 | DRx | PRx | <Reserved 3:1> | Tx | 
   */
  uint16                       path_bitmask;
  rfa_rf_chain_param_type      tx;
  rfa_rf_chain_param_type      prx;
  rfa_rf_chain_param_type      drx;
  rfa_rf_chain_param_type      drx2;
  rfa_rf_chain_param_type      drx3;
  rfa_rf_tx_param_type         tx_param;
  rfa_rf_rx_param_type         rx_param;

}rfa_rf_lte_check_config_params_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type   hdr;
  
  /* Current RF configuration info.*/
  rfa_rf_lte_check_config_params_s current_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Requested RF configuration info.*/
  rfa_rf_lte_check_config_params_s requested_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

} rfa_rf_lte_check_config_req_s;

/*-----------------------------------------------------------------------*/
typedef struct
{
  msgr_hdr_struct_type     hdr;
  uint32 carrier_idx_mask; 
  uint32 exit_carrier_idx_mask;

  /* Current RF configuration info.*/
  rfa_rf_lte_check_config_params_s current_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /* Requested RF configuration info.*/
  rfa_rf_lte_check_config_params_s requested_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  /*  Single Chain Support Bit Mask           
      RX_SC_BIT_MASK [8 bits] | TX_SC_BIT_MASK [8 bits]
   */
  uint16 single_chain_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];  

  /*  RF Cell config Bit Mask                                                                                               *
   *  <Reserved 15:8> | DRX3 | DRX2 | DRX | PRX | <Reserved 3:1> | TX 
   */
  uint16 rf_cell_config[RFA_RF_LTE_MAX_CELL_SUPPORTED];

  rfa_rf_lte_cmd_outcome_t req_result;

} rfa_rf_lte_check_config_cnf_s;

/*! @} */

/*-----------------------------------------------------------------------*/

#endif /* RF_LTE_MSG_H */

