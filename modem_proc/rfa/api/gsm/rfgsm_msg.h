
/*!
  @file
  rfgsm_msg.h

  @brief
  Message router UMIDs header for RF GSM subsystem. This file defines
  interfaces in the RF task.

  @note
  This file must be named with a _msg.h suffix for the build environment
  to correctly process the messages defined within.

*/

/*===========================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/rfgsm_msg.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/15   sml     Add probe burst variable for Drx chain to maintain correct 
                   tuner state during probe burst
11/27/14   sc      Add Rx override flag when TRM blanks GL1 Rx bursts
11/25/14   sw      Disable therm read during WTR dedicated hopping
10/01/14   tsr     GSM IDLE Mode RXD support
09/30/14   tws     Remove the wakeup callback function pointer
08/19/14   zbz     Add RF GSM CCS execution time profiling
08/07/14   ggs     Add PRX and DRX request result to enter mode and wakeup 
                   confirmation messages
07/30/14   cj      Added Tx sharing changes
07/30/14   hoh     Add enhanced rx feature support 
07/21/14   sw      Replace gas_id with sub_id in the ASD callback function
07/09/14   tsr     Add Support for IDLE Mode GSM RX Diversity
07/09/14   tws     Add new RX burst type PCH
07/08/14   ec      Add message definitions to support SAWless features
06/30/14   tsr     Add idle mode rx priority flag to RX burst payload structure
06/10/14   hk      edge code changes removing the featurization
05/15/14   sw      Return enter mode and wakeup execution times to GL1
04/30/14   sw      Move prepare therm read settings from enter mode to TX enable
04/16/14   tsr     GFW/Interface update for GSM RX Diversity
04/16/14   sb      Add umids for SUB3 for IP2 cal 
03/26/14   sb      Add support for IP2 cal for Dime PM
03/12/14   sw      For ASD, use gas_id inline with GL1 code
02/26/14   sw      Updates for Antenna Switch Diversity
02/17/13   ec      Add type definitions for HAL_VOTE REQ/CNF messages and wakeup cb pointer/edge core to unblock new project development
01/03/13   sc      Clean up comments and add sub ID payload member
10/11/13   sc      Add back removed buffer ID and RFLM handle
08/14/13   sc      Add default Rx burst type for error handling
08/14/13   sc      Revert Rx burst type variable name
08/01/13   pl      remove featurization on L1 interface header file
07/29/13   sml/svi ASD support for GSM tech
07/29/13   tws     Initial support for RX diversity commands
07/22/13   sc      Add more specific Rx burst type definition
07/11/13   sc      Remove unused burst variables and add required payload variables
04/07/13   sc      Clean-up and doxygen-compatible comments
06/20/13   ec      Add support for GSM Tune Away (GTA) enter mode
05/09/13   tws     Add SUB3 UMIDs
04/09/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp to 
                   make temp comp data thread safe
02/25/13   sc      Added iRAT cm enter and exit MSGR payloads
01/31/13   sc      Added cnf required to allow L1 to control confirmation from RF
01/11/13   aka     Added support for simultaneous Tx SAR
01/10/13   av      Create new REQ and CNF ids for subscriber 2 and handle them in rf_task_2
01/03/12   sc      Added include to geran_msg header for access to MSGR_GERAN_FW
12/18/12   sc      Added therm read event generation framework
12/18/12   sc      Remove unused tx dynamic ptr
12/14/12   sc      Added support for Exit Mode
12/14/12   sc      Removed support for RF Task iRAT startup and cleanup
10/12/12   sc      Added support for RF Task iRAT startup and cleanup
10/11/12   sc      Cleanup Rx definitions for iRAT readiness
08/07/12   sc      Place header at top of payload structures to fix mem mismatch
07/24/12   sc      Added RFM_DEVICE to the payloads
06/19/12   sc      Added ccs event payload support
04/23/12   sc      Support for Rx/Tx Burst, Tx Band, Tx Power shared memory interface
04/23/12   sc      Change Tx band type to reduce need for casting
03/20/12   sc      Edited Rx/Tx Burst structures to remove burst buffers due
                   to payload size limit issue.
                   Changed Enter mode and Wakeup Init SBI buffer struct types
02/27/12   ggs     Added Tx power control buffer placeholder.
                   Removed temporary spi buffer definition.
                   Remove Set Tx power CMD ID
                   Use IRAT buffer for enter mode, wakeup and tx band change
                   sbi list
02/15/12   ggs     Added SBI buffer for set tx band payload
02/10/12   sc      Added lna_range to Rx channel paramters and changed Rx and
                   Tx band type to rfgsm_band_type
02/08/12   ggs     Removed special characters. Updated type definitions
01/19/12   sc      Initial version

===========================================================================*/

#ifndef RF_GSM_MSG_H
#define RF_GSM_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "appmgr_msg.h"
#include "msgr.h"
#include "msgr_types.h"
#include "rfa_msgr.h"
#include "rfcom.h"
#include "rfgsm_core_types.h"
#include "lm_types.h"
#include "sys_type.h"
#include "mdsp_intf.h"
#include "gfw_sw_intf.h"
#include "geran_msg.h"
#include "rfmeas_types.h"

/*===========================================================================

                  EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*=============================================================================
                            MSGR SUPERVISORY ID'S
=============================================================================*/
/*! @define MSGR SUB1 IDs                              */
/*! @brief SUB1 IDs for all of the commands to RF GSM  */
#define RFA_RF_GSM_MSG_ID_ENTER_MODE                 0x01
#define RFA_RF_GSM_MSG_ID_RX_ENTER_MODE              0x02   
#define RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE             0x03
#define RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE         0x04
#define RFA_RF_GSM_MSG_ID_WAKEUP                0x05
#define RFA_RF_GSM_MSG_ID_SLEEP                 0x06
#define RFA_RF_GSM_MSG_ID_TX_ENABLE             0x07
#define RFA_RF_GSM_MSG_ID_TX_DISABLE            0x08
#define RFA_RF_GSM_MSG_ID_RX_BURST              0x09
#define RFA_RF_GSM_MSG_ID_SET_TX_BAND           0x0A
#define RFA_RF_GSM_MSG_ID_TX_BURST              0x0B
#define RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR      0x0C
#define RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING 0x0D
#define RFA_RF_GSM_MSG_ID_VREG_ON               0x0E
#define RFA_RF_GSM_MSG_ID_VREG_OFF              0x0F
#define RFA_RF_GSM_MSG_ID_EXIT_MODE             0x10 
#define RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT         0x11
#define RFA_RF_GSM_MSG_ID_CM_ENTER              0x12
#define RFA_RF_GSM_MSG_ID_CM_EXIT               0x13
#define RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP      0x14
#define RFA_RF_GSM_MSG_ID_SET_ANTENNA           0x15
#define RFA_RF_GSM_MSG_ID_HAL_VOTE              0x16
#define RFA_RF_GSM_MSG_ID_START_IP2_CAL         0x17
#define RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL      0x18
#define RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS 0x19

/* **********Place Next SUB1 Message Here************* */
#define RFA_RF_GSM_MSG_ID_MAX_SUB1_MSGS              0x1F

/*! @define MSGR SUB2 IDs                              */
/*! @brief SUB2 IDs for all of the commands to RF GSM  */
#define RFA_RF_GSM_MSG_ID_ENTER_MODE_SUB2            0x21
#define RFA_RF_GSM_MSG_ID_RX_ENTER_MODE_SUB2         0x22
#define RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE_SUB2        0x23
#define RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE_SUB2         0x24
#define RFA_RF_GSM_MSG_ID_WAKEUP_SUB2                0x25
#define RFA_RF_GSM_MSG_ID_SLEEP_SUB2                 0x26
#define RFA_RF_GSM_MSG_ID_TX_ENABLE_SUB2             0x27
#define RFA_RF_GSM_MSG_ID_TX_DISABLE_SUB2            0x28
#define RFA_RF_GSM_MSG_ID_RX_BURST_SUB2              0x29
#define RFA_RF_GSM_MSG_ID_SET_TX_BAND_SUB2           0x2A
#define RFA_RF_GSM_MSG_ID_TX_BURST_SUB2              0x2B
#define RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR_SUB2      0x2C
#define RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING_SUB2 0x2D
#define RFA_RF_GSM_MSG_ID_VREG_ON_SUB2               0x2E
#define RFA_RF_GSM_MSG_ID_VREG_OFF_SUB2              0x2F
#define RFA_RF_GSM_MSG_ID_EXIT_MODE_SUB2             0x30
#define RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT_SUB2         0x31
#define RFA_RF_GSM_MSG_ID_CM_ENTER_SUB2              0x32
#define RFA_RF_GSM_MSG_ID_CM_EXIT_SUB2               0x33
#define RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP_SUB2      0x34
#define RFA_RF_GSM_MSG_ID_SET_ANTENNA_SUB2           0x35
#define RFA_RF_GSM_MSG_ID_HAL_VOTE_SUB2              0x36
#define RFA_RF_GSM_MSG_ID_START_IP2_CAL_SUB2         0x37
#define RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL_SUB2      0x38
#define RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS_SUB2 0x39

/* **********Place Next SUB2 Message Here************* */
#define RFA_RF_GSM_MSG_ID_MAX_SUB2_MSGS              0x3F

/*! @define MSGR SUB3 IDs                              */
/*! @brief SUB3 IDs for all of the commands to RF GSM  */
#define RFA_RF_GSM_MSG_ID_ENTER_MODE_SUB3            0x41
#define RFA_RF_GSM_MSG_ID_RX_ENTER_MODE_SUB3         0x42
#define RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE_SUB3        0x43
#define RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE_SUB3         0x44
#define RFA_RF_GSM_MSG_ID_WAKEUP_SUB3                0x45
#define RFA_RF_GSM_MSG_ID_SLEEP_SUB3                 0x46
#define RFA_RF_GSM_MSG_ID_TX_ENABLE_SUB3             0x47
#define RFA_RF_GSM_MSG_ID_TX_DISABLE_SUB3            0x48
#define RFA_RF_GSM_MSG_ID_RX_BURST_SUB3              0x49
#define RFA_RF_GSM_MSG_ID_SET_TX_BAND_SUB3           0x4A
#define RFA_RF_GSM_MSG_ID_TX_BURST_SUB3              0x4B
#define RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR_SUB3      0x4C
#define RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING_SUB3 0x4D
#define RFA_RF_GSM_MSG_ID_VREG_ON_SUB3               0x4E
#define RFA_RF_GSM_MSG_ID_VREG_OFF_SUB3              0x4F
#define RFA_RF_GSM_MSG_ID_EXIT_MODE_SUB3             0x50
#define RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT_SUB3         0x51
#define RFA_RF_GSM_MSG_ID_CM_ENTER_SUB3              0x52
#define RFA_RF_GSM_MSG_ID_CM_EXIT_SUB3               0x53
#define RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP_SUB3      0x54
#define RFA_RF_GSM_MSG_ID_HAL_VOTE_SUB3              0x55
#define RFA_RF_GSM_MSG_ID_START_IP2_CAL_SUB3         0x56
#define RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL_SUB3      0x57
#define RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS_SUB3 0x58

/* **********Place Next SUB3 Message Here************* */
#define RFA_RF_GSM_MSG_ID_MAX_SUB3_MSGS              0x5F
#define RFA_RF_GSM_MSG_MAX_ID                        0xFF

/*! @todo This will need to reside in L1 */
#define RFA_RF_GSM_MAX_RX_ACTIVITIES            MDSP_MAX_STANDALONE_RSSI_MEASUREMENTS

#define MAX_MOD_TYPE    2

#define RFGSM_MAX_METRICS_ENTRIES 12
/*=============================================================================
                            RF Task GSM messages
=============================================================================*/
/*! @brief GSM subsystem message IDs the RF task sends. 
 * This enum is never called by name. It just defines symbols.
 */
/*! MSGR_DEFINE_UMID calculation below for reference:*/
/*! RFA_RF_GSM_<command>_CNF = (MSGR_RFA_RF_GSM & 0xFFFF) << 16 | 
 *                             (MSGR_TYPE_CNF & 0xFF) << 8 | 
 *                             (RFA_RF_GSM_MSG_ID_<command> & 0xFF)
 *
 *  RFA_RF_GSM_CNF_<command> = (MSGR_RFA_RF_GSM & 0xFFFF) << 16 | 
 *                             (MSGR_TYPE_CNF & 0xFF) << 8 | 
 *                             (RFA_RF_GSM_MSG_ID_<command> & 0xFF)
*/

/*! @brief SUB1 confirmation UMIDs */
enum
{
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, ENTER_MODE,       RFA_RF_GSM_MSG_ID_ENTER_MODE,         rfa_rf_gsm_enter_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, RX_ENTER_MODE,    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE,      rfa_rf_gsm_rx_enter_mode_cnf_s),  
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GTA_ENTER_MODE,   RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE,     rfa_rf_gsm_gta_enter_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GTA_EXIT_MODE,    RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE,      rfa_rf_gsm_gta_exit_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, WAKEUP,           RFA_RF_GSM_MSG_ID_WAKEUP,             rfa_rf_gsm_wakeup_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SLEEP,            RFA_RF_GSM_MSG_ID_SLEEP,              rfa_rf_gsm_sleep_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_ENABLE,        RFA_RF_GSM_MSG_ID_TX_ENABLE,          rfa_rf_gsm_tx_enable_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_DISABLE,       RFA_RF_GSM_MSG_ID_TX_DISABLE,         rfa_rf_gsm_tx_disable_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, RX_BURST,         RFA_RF_GSM_MSG_ID_RX_BURST,           rfa_rf_gsm_rx_burst_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SET_TX_BAND,      RFA_RF_GSM_MSG_ID_SET_TX_BAND,        rfa_rf_gsm_tx_band_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_BURST,         RFA_RF_GSM_MSG_ID_TX_BURST,           rfa_rf_gsm_tx_burst_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GET_TIMING_INFOR, RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR,   rfa_rf_gsm_get_timing_infor_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, IDLE_FRAME_PROCESSING, RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING, rfa_rf_gsm_idle_frame_processing_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, VREG_ON,          RFA_RF_GSM_MSG_ID_VREG_ON,            rfa_rf_gsm_vreg_on_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, VREG_OFF,         RFA_RF_GSM_MSG_ID_VREG_OFF,           rfa_rf_gsm_vreg_off_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, HAL_VOTE,         RFA_RF_GSM_MSG_ID_HAL_VOTE,           rfa_rf_gsm_hal_vote_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, EXIT_MODE,        RFA_RF_GSM_MSG_ID_EXIT_MODE,          rfa_rf_gsm_exit_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, CM_ENTER,         RFA_RF_GSM_MSG_ID_CM_ENTER,           rfa_rf_gsm_cm_enter_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, CM_EXIT,          RFA_RF_GSM_MSG_ID_CM_EXIT,            rfa_rf_gsm_cm_exit_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, UPDATE_TEMP_COMP, RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP,   rfa_rf_gsm_temp_comp_update_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SET_ANTENNA,      RFA_RF_GSM_MSG_ID_SET_ANTENNA,        rfa_rf_gsm_set_antenna_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, START_IP2_CAL,    RFA_RF_GSM_MSG_ID_START_IP2_CAL,      rfa_rf_gsm_start_ip2_cal_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, INIT_MSM_IP2_CAL, RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL,   rfa_rf_gsm_ip2_cal_msm_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, PROCESS_BURST_METRICS,  RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS,    rfa_rf_gsm_process_burst_metrics_cnf_s),
  
};

/*! @brief SUB2 confirmation UMIDs */

enum
{
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, ENTER_MODE_SUB2,       RFA_RF_GSM_MSG_ID_ENTER_MODE_SUB2,         rfa_rf_gsm_enter_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, RX_ENTER_MODE_SUB2,    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE_SUB2,      rfa_rf_gsm_rx_enter_mode_cnf_s),  
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GTA_ENTER_MODE_SUB2,   RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE_SUB2,     rfa_rf_gsm_gta_enter_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GTA_EXIT_MODE_SUB2,    RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE_SUB2,      rfa_rf_gsm_gta_exit_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, WAKEUP_SUB2,           RFA_RF_GSM_MSG_ID_WAKEUP_SUB2,             rfa_rf_gsm_wakeup_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SLEEP_SUB2,            RFA_RF_GSM_MSG_ID_SLEEP_SUB2,              rfa_rf_gsm_sleep_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_ENABLE_SUB2,        RFA_RF_GSM_MSG_ID_TX_ENABLE_SUB2,          rfa_rf_gsm_tx_enable_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_DISABLE_SUB2,       RFA_RF_GSM_MSG_ID_TX_DISABLE_SUB2,         rfa_rf_gsm_tx_disable_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, RX_BURST_SUB2,         RFA_RF_GSM_MSG_ID_RX_BURST_SUB2,           rfa_rf_gsm_rx_burst_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SET_TX_BAND_SUB2,      RFA_RF_GSM_MSG_ID_SET_TX_BAND_SUB2,        rfa_rf_gsm_tx_band_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_BURST_SUB2,         RFA_RF_GSM_MSG_ID_TX_BURST_SUB2,           rfa_rf_gsm_tx_burst_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GET_TIMING_INFOR_SUB2, RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR_SUB2,   rfa_rf_gsm_get_timing_infor_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, IDLE_FRAME_PROCESSING_SUB2, RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING_SUB2, rfa_rf_gsm_idle_frame_processing_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, VREG_ON_SUB2,          RFA_RF_GSM_MSG_ID_VREG_ON_SUB2,            rfa_rf_gsm_vreg_on_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, VREG_OFF_SUB2,         RFA_RF_GSM_MSG_ID_VREG_OFF_SUB2,           rfa_rf_gsm_vreg_off_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, HAL_VOTE_SUB2,         RFA_RF_GSM_MSG_ID_HAL_VOTE_SUB2,           rfa_rf_gsm_hal_vote_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, EXIT_MODE_SUB2,        RFA_RF_GSM_MSG_ID_EXIT_MODE_SUB2,          rfa_rf_gsm_exit_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, CM_ENTER_SUB2,         RFA_RF_GSM_MSG_ID_CM_ENTER_SUB2,           rfa_rf_gsm_cm_enter_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, CM_EXIT_SUB2,          RFA_RF_GSM_MSG_ID_CM_EXIT_SUB2,            rfa_rf_gsm_cm_exit_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, UPDATE_TEMP_COMP_SUB2, RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP_SUB2,   rfa_rf_gsm_temp_comp_update_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SET_ANTENNA_SUB2,      RFA_RF_GSM_MSG_ID_SET_ANTENNA_SUB2,        rfa_rf_gsm_set_antenna_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, START_IP2_CAL_SUB2,    RFA_RF_GSM_MSG_ID_START_IP2_CAL_SUB2,      rfa_rf_gsm_start_ip2_cal_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, INIT_MSM_IP2_CAL_SUB2, RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL_SUB2,   rfa_rf_gsm_ip2_cal_msm_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, PROCESS_BURST_METRICS_SUB2,  RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS_SUB2,    rfa_rf_gsm_process_burst_metrics_cnf_s),
};

/*! @brief SUB3 confirmation UMIDs */
enum
{
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, ENTER_MODE_SUB3,       RFA_RF_GSM_MSG_ID_ENTER_MODE_SUB3,         rfa_rf_gsm_enter_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, RX_ENTER_MODE_SUB3,    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE_SUB3,      rfa_rf_gsm_rx_enter_mode_cnf_s),  
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GTA_ENTER_MODE_SUB3,   RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE_SUB3,     rfa_rf_gsm_gta_enter_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GTA_EXIT_MODE_SUB3,    RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE_SUB3,      rfa_rf_gsm_gta_exit_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, WAKEUP_SUB3,           RFA_RF_GSM_MSG_ID_WAKEUP_SUB3,             rfa_rf_gsm_wakeup_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SLEEP_SUB3,            RFA_RF_GSM_MSG_ID_SLEEP_SUB3,              rfa_rf_gsm_sleep_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_ENABLE_SUB3,        RFA_RF_GSM_MSG_ID_TX_ENABLE_SUB3,          rfa_rf_gsm_tx_enable_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_DISABLE_SUB3,       RFA_RF_GSM_MSG_ID_TX_DISABLE_SUB3,         rfa_rf_gsm_tx_disable_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, RX_BURST_SUB3,         RFA_RF_GSM_MSG_ID_RX_BURST_SUB3,           rfa_rf_gsm_rx_burst_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, SET_TX_BAND_SUB3,      RFA_RF_GSM_MSG_ID_SET_TX_BAND_SUB3,        rfa_rf_gsm_tx_band_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, TX_BURST_SUB3,         RFA_RF_GSM_MSG_ID_TX_BURST_SUB3,           rfa_rf_gsm_tx_burst_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, GET_TIMING_INFOR_SUB3, RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR_SUB3,   rfa_rf_gsm_get_timing_infor_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, IDLE_FRAME_PROCESSING_SUB3, RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING_SUB3, rfa_rf_gsm_idle_frame_processing_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, VREG_ON_SUB3,          RFA_RF_GSM_MSG_ID_VREG_ON_SUB3,            rfa_rf_gsm_vreg_on_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, VREG_OFF_SUB3,         RFA_RF_GSM_MSG_ID_VREG_OFF_SUB3,           rfa_rf_gsm_vreg_off_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, HAL_VOTE_SUB3,         RFA_RF_GSM_MSG_ID_HAL_VOTE_SUB3,           rfa_rf_gsm_hal_vote_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, EXIT_MODE_SUB3,        RFA_RF_GSM_MSG_ID_EXIT_MODE_SUB3,          rfa_rf_gsm_exit_mode_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, CM_ENTER_SUB3,         RFA_RF_GSM_MSG_ID_CM_ENTER_SUB3,           rfa_rf_gsm_cm_enter_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, CM_EXIT_SUB3,          RFA_RF_GSM_MSG_ID_CM_EXIT_SUB3,            rfa_rf_gsm_cm_exit_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, UPDATE_TEMP_COMP_SUB3, RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP_SUB3,   rfa_rf_gsm_temp_comp_update_cnf_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, START_IP2_CAL_SUB3,    RFA_RF_GSM_MSG_ID_START_IP2_CAL_SUB3,      rfa_rf_gsm_start_ip2_cal_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, INIT_MSM_IP2_CAL_SUB3, RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL_SUB3,   rfa_rf_gsm_ip2_cal_msm_cnf_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, CNF, PROCESS_BURST_METRICS_SUB3,  RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS_SUB3,    rfa_rf_gsm_process_burst_metrics_cnf_s),

};

/*! @brief GSM subsystem message IDs the RF task receives. 
 * This enum is never called by name. It just defines symbols.
 */
 //RFA_RF_GSM_<command>_REQ = (MSGR_RFA_RF_GSM & 0xFFFF) << 16 | (MSGR_TYPE_REQ & 0xFF) << 8 | (RFA_RF_GSM_MSG_ID_<command> & 0xFF)
 //RFA_RF_GSM_REQ_<command> = (MSGR_RFA_RF_GSM & 0xFFFF) << 16 | (MSGR_TYPE_REQ & 0xFF) << 8 | (RFA_RF_GSM_MSG_ID_<command> & 0xFF)
 
 /*! @brief SUB1 requests */
enum
{
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, ENTER_MODE,       RFA_RF_GSM_MSG_ID_ENTER_MODE,         rfa_rf_gsm_enter_mode_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, RX_ENTER_MODE,    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE,      rfa_rf_gsm_rx_enter_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GTA_ENTER_MODE,   RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE,     rfa_rf_gsm_gta_enter_mode_req_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GTA_EXIT_MODE,    RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE,      rfa_rf_gsm_gta_exit_mode_req_s), 
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, WAKEUP,           RFA_RF_GSM_MSG_ID_WAKEUP,             rfa_rf_gsm_wakeup_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SLEEP,            RFA_RF_GSM_MSG_ID_SLEEP,              rfa_rf_gsm_sleep_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_ENABLE,        RFA_RF_GSM_MSG_ID_TX_ENABLE,          rfa_rf_gsm_tx_enable_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_DISABLE,       RFA_RF_GSM_MSG_ID_TX_DISABLE,         rfa_rf_gsm_tx_disable_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, RX_BURST,         RFA_RF_GSM_MSG_ID_RX_BURST,           rfa_rf_gsm_rx_burst_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SET_TX_BAND,      RFA_RF_GSM_MSG_ID_SET_TX_BAND,        rfa_rf_gsm_tx_band_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_BURST,         RFA_RF_GSM_MSG_ID_TX_BURST,           rfa_rf_gsm_tx_burst_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GET_TIMING_INFOR, RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR,   rfa_rf_gsm_get_timing_infor_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, IDLE_FRAME_PROCESSING, RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING, rfa_rf_gsm_idle_frame_processing_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, VREG_ON,          RFA_RF_GSM_MSG_ID_VREG_ON,            rfa_rf_gsm_vreg_on_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, VREG_OFF,         RFA_RF_GSM_MSG_ID_VREG_OFF,           rfa_rf_gsm_vreg_off_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, HAL_VOTE,         RFA_RF_GSM_MSG_ID_HAL_VOTE,           rfa_rf_gsm_hal_vote_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, EXIT_MODE,        RFA_RF_GSM_MSG_ID_EXIT_MODE,          rfa_rf_gsm_exit_mode_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, IND, SET_SAR_LIMIT,    RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT,      rfa_rf_gsm_set_sar_limit_ind_s),      
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, CM_ENTER,         RFA_RF_GSM_MSG_ID_CM_ENTER,           rfa_rf_gsm_cm_enter_req_s),
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, CM_EXIT,          RFA_RF_GSM_MSG_ID_CM_EXIT,            rfa_rf_gsm_cm_exit_req_s),      
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, UPDATE_TEMP_COMP, RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP,   rfa_rf_gsm_update_temp_comp_req_s),      
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SET_ANTENNA,      RFA_RF_GSM_MSG_ID_SET_ANTENNA,        rfa_rf_gsm_set_antenna_req_s),      
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, START_IP2_CAL,    RFA_RF_GSM_MSG_ID_START_IP2_CAL,      rfa_rf_gsm_start_ip2_cal_req_s), 
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, INIT_MSM_IP2_CAL,    RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL,  rfa_rf_gsm_ip2_cal_msm_req_s) ,
   MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, PROCESS_BURST_METRICS,  RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS,    rfa_rf_gsm_process_burst_metrics_req_s),

  
};

/*! @brief SUB2 requests */

enum
{
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, ENTER_MODE_SUB2,       RFA_RF_GSM_MSG_ID_ENTER_MODE_SUB2,         rfa_rf_gsm_enter_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, RX_ENTER_MODE_SUB2,    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE_SUB2,      rfa_rf_gsm_rx_enter_mode_req_s),         
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GTA_ENTER_MODE_SUB2,   RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE_SUB2,     rfa_rf_gsm_gta_enter_mode_req_s),     
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GTA_EXIT_MODE_SUB2,    RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE_SUB2,      rfa_rf_gsm_gta_exit_mode_req_s),     
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, WAKEUP_SUB2,           RFA_RF_GSM_MSG_ID_WAKEUP_SUB2,             rfa_rf_gsm_wakeup_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SLEEP_SUB2,            RFA_RF_GSM_MSG_ID_SLEEP_SUB2,              rfa_rf_gsm_sleep_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_ENABLE_SUB2,        RFA_RF_GSM_MSG_ID_TX_ENABLE_SUB2,          rfa_rf_gsm_tx_enable_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_DISABLE_SUB2,       RFA_RF_GSM_MSG_ID_TX_DISABLE_SUB2,         rfa_rf_gsm_tx_disable_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, RX_BURST_SUB2,         RFA_RF_GSM_MSG_ID_RX_BURST_SUB2,           rfa_rf_gsm_rx_burst_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SET_TX_BAND_SUB2,      RFA_RF_GSM_MSG_ID_SET_TX_BAND_SUB2,        rfa_rf_gsm_tx_band_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_BURST_SUB2,         RFA_RF_GSM_MSG_ID_TX_BURST_SUB2,           rfa_rf_gsm_tx_burst_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GET_TIMING_INFOR_SUB2, RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR_SUB2,   rfa_rf_gsm_get_timing_infor_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, IDLE_FRAME_PROCESSING_SUB2, RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING_SUB2, rfa_rf_gsm_idle_frame_processing_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, VREG_ON_SUB2,          RFA_RF_GSM_MSG_ID_VREG_ON_SUB2,            rfa_rf_gsm_vreg_on_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, VREG_OFF_SUB2,         RFA_RF_GSM_MSG_ID_VREG_OFF_SUB2,           rfa_rf_gsm_vreg_off_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, HAL_VOTE_SUB2,         RFA_RF_GSM_MSG_ID_HAL_VOTE_SUB2,           rfa_rf_gsm_hal_vote_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, EXIT_MODE_SUB2,        RFA_RF_GSM_MSG_ID_EXIT_MODE_SUB2,          rfa_rf_gsm_exit_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, IND, SET_SAR_LIMIT_SUB2,    RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT_SUB2,      rfa_rf_gsm_set_sar_limit_ind_s),      
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, CM_ENTER_SUB2,         RFA_RF_GSM_MSG_ID_CM_ENTER_SUB2,           rfa_rf_gsm_cm_enter_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, CM_EXIT_SUB2,          RFA_RF_GSM_MSG_ID_CM_EXIT_SUB2,            rfa_rf_gsm_cm_exit_req_s),     
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, UPDATE_TEMP_COMP_SUB2, RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP_SUB2,   rfa_rf_gsm_update_temp_comp_req_s),       
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SET_ANTENNA_SUB2,      RFA_RF_GSM_MSG_ID_SET_ANTENNA_SUB2,        rfa_rf_gsm_set_antenna_req_s),      
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, START_IP2_CAL_SUB2,    RFA_RF_GSM_MSG_ID_START_IP2_CAL_SUB2,      rfa_rf_gsm_start_ip2_cal_req_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, INIT_MSM_IP2_CAL_SUB2, RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL_SUB2,   rfa_rf_gsm_ip2_cal_msm_req_s) ,
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, PROCESS_BURST_METRICS_SUB2,  RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS_SUB2,    rfa_rf_gsm_process_burst_metrics_req_s),
};

/*! @brief SUB3 requests */
enum
{
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, ENTER_MODE_SUB3,       RFA_RF_GSM_MSG_ID_ENTER_MODE_SUB3,         rfa_rf_gsm_enter_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, RX_ENTER_MODE_SUB3,    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE_SUB3,      rfa_rf_gsm_rx_enter_mode_req_s),  
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GTA_ENTER_MODE_SUB3,   RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE_SUB3,     rfa_rf_gsm_gta_enter_mode_req_s),      
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GTA_EXIT_MODE_SUB3,    RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE_SUB3,      rfa_rf_gsm_gta_exit_mode_req_s),      
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, WAKEUP_SUB3,           RFA_RF_GSM_MSG_ID_WAKEUP_SUB3,             rfa_rf_gsm_wakeup_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SLEEP_SUB3,            RFA_RF_GSM_MSG_ID_SLEEP_SUB3,              rfa_rf_gsm_sleep_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_ENABLE_SUB3,        RFA_RF_GSM_MSG_ID_TX_ENABLE_SUB3,          rfa_rf_gsm_tx_enable_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_DISABLE_SUB3,       RFA_RF_GSM_MSG_ID_TX_DISABLE_SUB3,         rfa_rf_gsm_tx_disable_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, RX_BURST_SUB3,         RFA_RF_GSM_MSG_ID_RX_BURST_SUB3,           rfa_rf_gsm_rx_burst_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, SET_TX_BAND_SUB3,      RFA_RF_GSM_MSG_ID_SET_TX_BAND_SUB3,        rfa_rf_gsm_tx_band_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, TX_BURST_SUB3,         RFA_RF_GSM_MSG_ID_TX_BURST_SUB3,           rfa_rf_gsm_tx_burst_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, GET_TIMING_INFOR_SUB3, RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR_SUB3,   rfa_rf_gsm_get_timing_infor_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, IDLE_FRAME_PROCESSING_SUB3, RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING_SUB3, rfa_rf_gsm_idle_frame_processing_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, VREG_ON_SUB3,          RFA_RF_GSM_MSG_ID_VREG_ON_SUB3,            rfa_rf_gsm_vreg_on_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, VREG_OFF_SUB3,         RFA_RF_GSM_MSG_ID_VREG_OFF_SUB3,           rfa_rf_gsm_vreg_off_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, HAL_VOTE_SUB3,         RFA_RF_GSM_MSG_ID_HAL_VOTE_SUB3,           rfa_rf_gsm_hal_vote_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, EXIT_MODE_SUB3,        RFA_RF_GSM_MSG_ID_EXIT_MODE_SUB3,          rfa_rf_gsm_exit_mode_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, IND, SET_SAR_LIMIT_SUB3,    RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT_SUB3,      rfa_rf_gsm_set_sar_limit_ind_s),      
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, CM_ENTER_SUB3,         RFA_RF_GSM_MSG_ID_CM_ENTER_SUB3,           rfa_rf_gsm_cm_enter_req_s),
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, CM_EXIT_SUB3,          RFA_RF_GSM_MSG_ID_CM_EXIT_SUB3,            rfa_rf_gsm_cm_exit_req_s),     
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, UPDATE_TEMP_COMP_SUB3, RFA_RF_GSM_MSG_ID_UPDATE_TEMP_COMP_SUB3,   rfa_rf_gsm_update_temp_comp_req_s),    
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, START_IP2_CAL_SUB3,    RFA_RF_GSM_MSG_ID_START_IP2_CAL_SUB3,      rfa_rf_gsm_start_ip2_cal_req_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, INIT_MSM_IP2_CAL_SUB3, RFA_RF_GSM_MSG_ID_INIT_MSM_IP2_CAL_SUB3,  rfa_rf_gsm_ip2_cal_msm_req_s), 
  MSGR_DEFINE_UMID(RFA, RF_GSM, REQ, PROCESS_BURST_METRICS_SUB3,  RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS_SUB3,    rfa_rf_gsm_process_burst_metrics_req_s),  
};


/*! @enum rfa_rf_gsm_cmd_outcome_t */
/*! An enum for the result of a particular GSM RF command.*/
typedef enum
{
  RFA_RF_GSM_SUCCESS             =1, /**< Command result for success.*/
  RFA_RF_GSM_STATE_MACHINE_ERROR =2, /**< Command result for machine error.*/
  RFA_RF_GSM_EXECUTION_ERROR     =3, /**< Command result for an execution error.*/
  RFA_RF_GSM_L1_PARAM_ERROR      =4, /**< Command result for a payload error. */
  RFA_RF_GSM_CMD_OUTCOME_MAX     =5, /**< Maximum number of outcomes.*/
} rfa_rf_gsm_cmd_outcome_t;


/*! @enum rfgsm_rx_burst_type */
/*! An enum for the specific Rx burst type for processing.*/
typedef enum
{
  RFGSM_RX_DEFAULT_RX_BURST,
  RFGSM_RX_SYNC_BURST,
  RFGSM_RX_SCH_OR_ACQ_BURST,
  RFGSM_RX_PCH_BURST,
  RFGSM_RX_MON_BURST,
  RFGSM_RX_PROBE_BURST,
  RFGSM_MAX_RX_BURST_TYPES
} rfgsm_rx_burst_type;

/*! @enum  rfgsm_enter_mode_type */
/*! An enum for the specific Enter mode type for processing.*/
typedef enum
{
  RFGSM_PRX_ENTER_MODE,
  RFGSM_DRX_ENTER_MODE,
  RFGSM_PRX_DRX_ENTER_MODE,
  RFGSM_PRX_WTR_DED_HOP_ENTER_MODE,
  RFGSM_INVALID_ENTER_MODE 
} rfgsm_enter_mode_type;



/*! @enum  rfgsm_exit_mode_type */
/*! An enum for the specific Exit mode type for processing.*/
typedef enum
{
  RFGSM_PRX_EXIT_MODE,
  RFGSM_DRX_EXIT_MODE,
  RFGSM_PRX_DRX_EXIT_MODE,
  RFGSM_INVALID_EXIT_MODE 
} rfgsm_exit_mode_type;

/*! @enum  rfgsm_wakeup_mode_type */
/*! An enum for the specific Wake up mode type for processing.*/
typedef enum
{
  RFGSM_PRX_WAKEUP_MODE,
  RFGSM_DRX_WAKEUP_MODE,
  RFGSM_PRX_DRX_WAKEUP_MODE,
  RFGSM_INVALID_WAKEUP_MODE 
} rfgsm_wakeup_mode_type;



/*! @enum  rfgsm_sleep_mode_type */
/*! An enum for the specific sleep mode type for processing.*/
typedef enum
{
  RFGSM_PRX_SLEEP_MODE,
  RFGSM_DRX_SLEEP_MODE,
  RFGSM_PRX_DRX_SLEEP_MODE,
  RFGSM_INVALID_SLEEP_MODE 
} rfgsm_sleep_mode_type;


/*! @struct  rfgsm_rxlm_buf_index_type */
/*! An structure for the handling rxlm buffer index type for Enter mode.*/

typedef struct
{
  uint32 prx_idx; 			
  uint32 drx_idx;
} rfgsm_rxlm_buf_index_type;



/*=============================================================================
                 TYPE DEFINITIONS FOR MSGR PAYLOAD STRUCTURES
=============================================================================*/
/*! @struct rfa_rf_gsm_set_antenna_callback_s */
/*! @brief This structure contains members for L1 callback function 
 * which informs TRM that antenna switch is complete (for ASD)
 *  @param  device - RFM device type to be passed
 *  @param  ant_pos - set antenna payload to be updated in rf task
 *  @param  req_results - Result detailing the success/failure of the command
 *  @param  sub_id - Subscription ID to be returned to GL1
 */
typedef struct
{
  rfcom_device_enum_type        device;
  uint8                         ant_pos;
  rfa_rf_gsm_cmd_outcome_t      req_result;
  uint8                         gas_id;
  uint8                         sub_id;
} rfa_rf_gsm_set_antenna_callback_s;

typedef struct
{
  boolean 		enh_rx_enable_flag;
  int32			enh_rx_offset_freq;
} rfgsm_enh_rx_params_type;

/*! Type defintion for L1 Call back function */
typedef void(*rfgsm_l1_cb_type)(rfa_rf_gsm_set_antenna_callback_s  *cb_data);

/*! @struct rfgsm_rx_chan_params_type */
/*! @brief This structure contains members for GSM Rx frame parameters that 
 *  will not vary between the bursts within that frame.
 *  @param  band - RF GSM Rx Band
 *  @param  arfcn - RF GSM Rx channel number
 *  @param  freq_err - frequency error for RF to apply during pll tuning
 *  @param  wbee - reg value for wide band energy estimator read by FW
 *  @param  rx_lvl_dbm - expected rx power level in 16ths dBm
 *  @param  gain_ptr - Gain range, upper/lower limits and value in 16ths dBm
 *  @param  lna_range - Expected rx lna range
 *  @param  burst_type - RF GSM Rx burst type
 *  @param  rx_burst_type - RF GSM Rx burst type, Sync, Acq or SCH, Monitor
 *  @param  high_lin_mode_active - used to inform GL1 if High Linearity mode is active
 *  @param  enh_rx_params - enhanced Rx enable flag and offset freq from L1
 */
typedef struct
{
  rfgsm_band_type band;
  uint16          arfcn;
  int32           freq_err;
  uint32          wbee;
  dBx16_T         rx_lvl_dbm;
  rf_gain_T       *gain_ptr;
  uint8           lna_range;
  rf_burst_type   burst_type;
  rfgsm_rx_burst_type rx_burst_type;
  rfgsm_rx_burst_type probe_burst;
  uint16          *high_lin_mode_active;  //GL1 need to log the state on SAWless devices, RF will update this
  rfgsm_enh_rx_params_type enh_rx_params;
  boolean         blanked; //Has TRM blanked this Rx, therefore do not create
}rfgsm_rx_chan_params_type;

/*===========================================================================*/
/*! @struct rfgsm_rx_diversity_chan_params_type */
/*! @brief This structure contains members for GSM Rx diversity frame parameters that 
 *  will not vary between the bursts within that frame.
 *  @param rx_lvl_dbm - Expected rx power level in 16ths dBm
 *  @param  gain_ptr - Gain range, upper/lower limits and value in 16ths dBm
 *  @param  lna_range - Expected rx lna range
 *  @param  high_lin_mode_active - used to inform GL1 if High Linearity mode is active
 */
typedef struct
{
  dBx16_T       rx_lvl_dbm;
  rf_gain_T     *gain_ptr;
  uint8         lna_range;
  uint16        *high_lin_mode_active;  //GL1 need to log the state on SAWless devices, RF will update this
  boolean       blanked; //Has TRM blanked this Rx, therefore do not create (potential for RxD-only blanking)
  rfgsm_rx_burst_type probe_burst;
}rfgsm_rx_diversity_chan_params_type;

/*===========================================================================*/
/*! @struct rfgsm_tx_chan_params_type */
/*! @brief This structure contains members for GSM Tx frame channel parameters that 
 *  will not vary between the bursts within that frame.
 *  @param  band - RF GSM Tx Band
 *  @param  arfcn - RF GSM Tx Channel number
 *  @param  freq_err - frequency error for RF to apply during pll tuning
 */
typedef struct
{
  rfgsm_band_type band;
  uint16          arfcn;
  int32           freq_err;
}rfgsm_tx_chan_params_type;



/*===========================================================================*/
/*! @struct rfa_rf_gsm_rx_burst_type*/
/*! @brief This structure contains all the information, for a given GSM Rx 
 *  burst type, required to handle up to RFA_RF_GSM_MAX_RX_ACTIVITIES Rx bursts.
 *  RF populate the L1-specified buffers with SSBI, GRFC, RFFE and MODEM Register
 *  HW (RXTXLM) write data.
 *  Buffer memory management is handled by GL1 in a ping-pong fashion.
 *  @param  num_rx_bursts - Number of Rx bursts to handle
 *  @param  rx_chan_params - parameters to configure for each Rx burst
 *  @param  rx_diversity_chan_params - parameters to configure for each DRx burst
 *  @param  gfw_rf_burst_event - array of pointers to GFW shared memory
 */
typedef struct
{
  uint8                     num_rx_bursts;
  boolean                   gl1_lower_rx_ccs_prio;
  rfgsm_rx_chan_params_type rx_chan_params[RFA_RF_GSM_MAX_RX_ACTIVITIES];
  rfgsm_rx_diversity_chan_params_type rx_diversity_chan_params[RFA_RF_GSM_MAX_RX_ACTIVITIES];
  void                      *gfw_rf_burst_event[RFA_RF_GSM_MAX_RX_ACTIVITIES];
}rfa_rf_gsm_rx_burst_type;



/*===========================================================================*/
/*! @struct rfa_rf_gsm_tx_burst_type*/
/*! @brief This structure contains all the information required to handle GSM
 *  Tx parameters for a single frame for up to RFGSM_MAX_TX_SLOTS_PER_FRAME per frame.
 *  RF populate the L1-specified buffers with SSBI, GRFC, RFFE and MODEM Register
 *  HW (RXTXLM) write data.
 *  @param  num_tx_slots - Number of Tx slots
 *  @param  num_assigned_uplink_slots - Number of assigned Tx slots
 *  @param  tx_chan_params - RF GSM Tx channel parameters
 *  @param  tx_pwr_ctl_params - RF GSM Tx power control parameters per slot
 *  @param  tx_burst_event_ptr - Pointer to FW shared memory for Tx
 */
typedef struct
{
  uint8                     num_tx_slots;
  uint8                     num_assigned_uplink_slots;
  rfgsm_tx_chan_params_type tx_chan_params;
  rfgsm_power_profile_type  tx_pwr_ctl_params[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  void                      *tx_burst_event_ptr;
}rfa_rf_gsm_tx_burst_type;



/*===========================================================================*/
/*! @struct rfa_rf_gsm_sar_limit_params_type*/
/*! @brief This structure contains all the information required for  GSM SAR limit 
 *  for different modulations and different number of frame upto RFGSM_MAX_TX_SLOTS_PER_FRAME per frame.
 *  @param  device - RFM device
 *  @param  sar_tx_limit - SAR Tx pwr limit for each modulation and number of slots in a frame
 */
typedef struct
{
  rfcom_device_enum_type device;
  int16                  sar_tx_limit[MAX_MOD_TYPE][RFGSM_MAX_TX_SLOTS_PER_FRAME];
}rfa_rf_gsm_sar_limit_params_type;



/*===========================================================================*/
/*! @struct rfa_rf_gsm_rx_burst_metrics_type*/
/*! @brief This structure is used by GL1 to pass burst metrics in to RFSW for HL/LL decision 
 *  on SAWless targets.
 *  @param  band - GSM band for which the metrics apply
 *  @param  arfcn - GSM ARFCN for which the metrics apply
 *  @param  rx_lvl_dbm - dBm signal level measured (RSSI)
 *  @param  snr - Signal to Noise Ratio (dB)
 *  @param  jdet_value - Raw jammer detector reading
 */

typedef struct
{
  rfgsm_band_type            band;
  uint16                     arfcn;
  dBx16_T                    rx_lvl_dbm;
  uint16                     snr;
  uint16                     jdet_value;
}rfa_rf_gsm_rx_burst_metrics_type;

/*=============================================================================
                    RFA_RF_GSM_MSG_ID_ENTER_MODE
=============================================================================*/
/*RFA_RF_GSM_ENTER_MODE_REQ*/
/*! @struct rfa_rf_gsm_enter_mode_req_s*/
/*! @brief  This structure is the payload for initialising the RF Driver for 
 *  a specific radio transceiver for GSM.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for enter mode)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  init_writes_buff_ptr - pointer to GFW shared memory for enter mode
 *  @param  therm_read_buff_ptr - pointer to GFW shared memory for therm read
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;    //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_enter_mode_type         enter_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx; //legacy support
  rfgsm_rxlm_buf_index_type     rxlm_buf_index;
  void                          *init_writes_buff_ptr;
  void                          *therm_read_buff_ptr;
  rfgsm_timing_info_type        *timing_info;
#ifdef FEATURE_TX_SHARING
  boolean                       tx_sharing_mode;
  uint32                        txlm_buf_idx;
#endif
} rfa_rf_gsm_enter_mode_req_s;

/*===========================================================================*/
/*RFA_RF_GSM_ENTER_MODE_CNF*/
/*! @struct rfa_rf_gsm_enter_mode_cnf_s*/
/*! @brief  This structure is the payload for confirming the completion of 
 *  initialisation of the RF Driver for a specific radio transceiver for GSM.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for enter mode)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  init_writes_buff_ptr - pointer to GFW shared memory for enter mode
 *  @param  therm_read_buff_ptr - pointer to GFW shared memory for therm read
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 *  @param  sawless_sup - 2d array of booleans indicating SAWless support across
 *          bands for all logical devices.
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;   //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_enter_mode_type         enter_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx; //legacy support
  rfgsm_rxlm_buf_index_type     rxlm_buf_index;
  void                          *init_writes_buff_ptr;
  void                          *therm_read_buff_ptr;
  rfgsm_timing_info_type        *timing_info;
  boolean                       ip2_cal_supported; //Indicates device is SAWless if true
  unsigned long long            exec_time;      /* Execution Time (in pcycles) */
  unsigned long long            avg_exec_time;  /* Average Case Execution Time (in pcycles) */
  /* It is necessary to report the sawless support for all devices during an enter mode in case of WTR hopping */
  boolean                       sawless_sup[RFM_MAX_WAN_DEVICES][RFGSM_MAX_UNIQUE_RFCOM_BANDS]; /* Indicate which bands on all devices are sawless */
  rfa_rf_gsm_cmd_outcome_t      req_result;
  rfa_rf_gsm_cmd_outcome_t      prx_device_req_result;
  rfa_rf_gsm_cmd_outcome_t      drx_device_req_result;
} rfa_rf_gsm_enter_mode_cnf_s;

/*===========================================================================*/
/* This function pointer type is used so that GL1 can pass in a callback to RF.
   This allows RF to immediately signal when processing is complete and allows
   for wakeup parallelisation in GL1 */
typedef void (*rfgsm_wakeup_cb_fp_t)(uint32, uint32);
/*=============================================================================
                          RFA_RF_GSM_MSG_ID_WAKEUP
=============================================================================*/
/*  RFA_RF_GSM_WAKEUP_REQ      */
/*! @struct rfa_rf_gsm_wakeup_req_s*/
/*! @brief This structure is the payload for waking up the RF hardware from 
 *  sleep mode, setting the hardware to the same band it was in before sleep.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for wakeup)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  sbi_init_writes_buff_ptr - Pointer to GFW shared memory
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;   //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_wakeup_mode_type        wakeup_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;  //legacy support
  rfgsm_rxlm_buf_index_type     rxlm_buf_index;
  void                          *sbi_init_writes_buff_ptr;
  rfgsm_timing_info_type        *timing_info;
} rfa_rf_gsm_wakeup_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_WAKEUP_CNF*/
/*! @struct rfa_rf_gsm_wakeup_cnf_s*/
/*! @brief This structure is the payload for confirming the RF hardware has
 *  woken up.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for wakeup)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  sbi_init_writes_buff_ptr - Pointer to GFW shared memory
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;    //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_wakeup_mode_type        wakeup_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;  //legacy support
  rfgsm_rxlm_buf_index_type     rxlm_buf_index;
  void                          *sbi_init_writes_buff_ptr;
  rfgsm_timing_info_type        *timing_info;
  unsigned long long            exec_time;      /* Execution Time (in pcycles) */
  unsigned long long            avg_exec_time;  /* Average Case Execution Time (in pcycles) */
  rfa_rf_gsm_cmd_outcome_t      req_result;
  rfa_rf_gsm_cmd_outcome_t      prx_device_req_result;
  rfa_rf_gsm_cmd_outcome_t      drx_device_req_result;
  uint32                        ccs_time_us;
} rfa_rf_gsm_wakeup_cnf_s;

/*===========================================================================*/



/*=============================================================================
                       RFA_RF_GSM_MSG_ID_SLEEP
=============================================================================*/
/*RFA_RF_GSM_SLEEP_REQ*/
/*! @struct rfa_rf_gsm_sleep_req_s*/
/*! @brief This structure is the payload for placing the RF hardware to sleep
 *  for GSM mode.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for sleep)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;       //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_sleep_mode_type         sleep_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;  //legacy support
} rfa_rf_gsm_sleep_req_s;

/*===========================================================================*/
/*RFA_RF_GSM_SLEEP_CNF*/
/*! @struct rfa_rf_gsm_sleep_cnf_s*/
/*! @brief This structure is the payload for confirming that the RF hardware
 *  has been triggered to go to sleep.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for sleep)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;       //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_sleep_mode_type         sleep_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx; //legacy support
  rfa_rf_gsm_cmd_outcome_t      req_result;
  rfa_rf_gsm_cmd_outcome_t      prx_device_req_result;
  rfa_rf_gsm_cmd_outcome_t      drx_device_req_result;
} rfa_rf_gsm_sleep_cnf_s;

/*===========================================================================*/



/*=============================================================================
                    RFA_RF_GSM_MSG_ID_TX_ENABLE
=============================================================================*/
/*RFA_RF_GSM_TX_ENABLE_REQ*/
/*! @struct rfa_rf_gsm_tx_enable_req_s*/
/*! @brief This structure is the payload for enabling the transmitter for GSM
 *  mode.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for Tx enable)
 *  @param  txlm_buf_idx - Tx Link Manager buffer index
 *  @param  therm_read_buff_ptr - pointer to GFW shared memory for therm read 
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        txlm_buf_idx;
  void                          *therm_read_buff_ptr;
  uint8                         edge_core;
} rfa_rf_gsm_tx_enable_req_s;

/*===========================================================================*/
/*RFA_RF_GSM_TX_ENABLE_CNF*/
/*! @struct rfa_rf_gsm_tx_enable_cnf_s*/
/*! @brief This structure is the payload for confirming the transmitter for GSM
 *  has been enabled.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for Tx enable)
 *  @param  txlm_buf_idx - Tx Link Manager buffer index
 *  @param  therm_read_buff_ptr - pointer to GFW shared memory for therm read
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        txlm_buf_idx;
  void                          *therm_read_buff_ptr;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_tx_enable_cnf_s;

/*===========================================================================*/



/*=============================================================================
                       RFA_RF_GSM_MSG_ID_TX_DISABLE
=============================================================================*/
/*RFA_RF_GSM_TX_DISABLE_REQ*/
/*! @struct rfa_rf_gsm_tx_disable_req_s*/
/*! @brief This structure is the payload for disabling the transmitter for GSM
 *  mode.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for Tx disable)
 *  @param  txlm_buf_idx - Tx Link Manager buffer index
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        txlm_buf_idx;
} rfa_rf_gsm_tx_disable_req_s;

/*===========================================================================*/
/*RFA_RF_GSM_TX_DISABLE_CNF*/
/*! @struct rfa_rf_gsm_tx_disable_cnf_s*/
/*! @brief This structure is the payload for confirming the transmitter for GSM
 *  has been disabled.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for Tx disable)
 *  @param  txlm_buf_idx - Tx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        txlm_buf_idx;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_tx_disable_cnf_s;

/*===========================================================================*/



/*=============================================================================
                      RFA_RF_GSM_MSG_ID_RX_BURST
=============================================================================*/
/*  RFA_RF_GSM_RX_BURST_REQ         */
/*! @struct rfa_rf_gsm_rx_burst_req_s*/
/*! @brief This structure is the payload of the Rx burst request, handling all
 *  the GSM Rx burst information based on type of Rx burst.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  rx_burst_params - RF GSM Rx burst parameters for each of the bursts
 *  @param  rx_diversity_device - RFM device type to be passed from L1 to RF device
 *  @param  enable_rx_diversity - Enable Rx diversity flag
 *  @param  buffer_id - Triple Buffer ID
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_rx_burst_type      *rx_burst_params;
  rfcom_device_enum_type        rx_diversity_device;
  boolean                       enable_rx_diversity;
  uint8                         buffer_id;
  rfa_rf_gsm_cmd_outcome_t      req_result;
}rfa_rf_gsm_rx_burst_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_RX_BURST_CNF         */
/*! @struct rfa_rf_gsm_rx_burst_cnf_s*/
/*! @brief This structure is the payload of the confirmation to L1 indicating
 *  completion of RF populating the buffers for the GSM Rx burst/s.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  rx_burst_params - RF GSM Rx burst parameters for each of the bursts
 *  @param  rx_diversity_device - RFM device type to be passed from L1 to RF device
 *  @param  enable_rx_diversity - Enable Rx diversity flag
 *  @param  buffer_id - Triple Buffer ID
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_rx_burst_type      *rx_burst_params;
  rfcom_device_enum_type        rx_diversity_device;
  boolean                       enable_rx_diversity;
  uint8                         buffer_id;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_rx_burst_cnf_s;

/*===========================================================================*/



/*=============================================================================
                        RFA_RF_GSM_MSG_ID_SET_TX_BAND
=============================================================================*/
/*  RFA_RF_GSM_SET_TX_BAND_REQ          */
/*! @struct rfa_rf_gsm_set_tx_band_req_s*/
/*! @brief This structure is the payload for initialising the hardware for the
 *  specified Tx band. There can be a significant number of RFI writes required
 *  for a band change, so GL1 should send this command at least 2 frames before
 *  the band is required to be changed.
 *  GFW must schedule these writes for after the last RX/TX activity before
 *  the Tx band changes
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for Tx band)
 *  @param  band - Tx band RF need to program the RF device to
 *  @param  sbi_set_tx_band_buff_ptr - Pointer to GFW shared memory
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfgsm_band_type               band;
  void                          *sbi_set_tx_band_buff_ptr;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_set_tx_band_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_SET_TX_BAND_CNF          */
/*! @struct rfa_rf_gsm_set_tx_band_cnf_s*/
/*! @brief This structure is the payload for confirming the hardware has been
 *  setup for a specified Tx band.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for Tx band)
 *  @param  band - Tx band RF need to program the RF device to
 *  @param  sbi_set_tx_band_buff_ptr - Pointer to GFW shared memory
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfgsm_band_type               band;
  void                          *sbi_set_tx_band_buff_ptr;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_set_tx_band_cnf_s;

/*===========================================================================*/



/*=============================================================================
                      RFA_RF_GSM_MSG_ID_TX_BURST
=============================================================================*/
/*  RFA_RF_GSM_TX_BURST_REQ          */
/*! @struct rfa_rf_gsm_tx_burst_req_s*/
/*! @brief This structure is the payload of the Tx burst request, handling all
 *  the GSM Tx burst information for a single frame.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0, 1, 2 for Tx burst)
 *  @param  tx_burst_params - pointer to parameters for all Tx's
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfa_rf_gsm_tx_burst_type      *tx_burst_params;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_tx_burst_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_TX_BURST_CNF          */
/*! @struct rfa_rf_gsm_tx_burst_cnf_s*/
/*! @brief This structure is the payload of the confirmation to L1 indicating
 *  completion of RF populating the buffers for the GSM Tx slots.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0, 1, 2 for Tx burst)
 *  @param  tx_burst_params - pointer to parameters for all Tx's
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfa_rf_gsm_tx_burst_type      *tx_burst_params;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_tx_burst_cnf_s;

/*===========================================================================*/



/*=============================================================================
                    RFA_RF_GSM_MSG_ID_GET_TIMING_INFOR
=============================================================================*/
/*  RFA_RF_GSM_GET_TIMING_INFOR_REQ          */
/*! @struct rfa_rf_gsm_get_timing_infor_req_s*/
/*! @brief This structure is the payload for L1 to retrieve RF timing related 
 *  information for monitor, Rx and Tx burst types.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfgsm_timing_info_type        *timing_info;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_get_timing_infor_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_GET_TIMING_INFOR_CNF          */
/*! @struct rfa_rf_gsm_get_timing_infor_cnf_s*/
/*! @brief This structure is the payload for confirming RF have retrieved the
 *  timing relating to monitor, Rx and Tx bursts.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfgsm_timing_info_type        *timing_info;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_get_timing_infor_cnf_s;

/*===========================================================================*/



/*=============================================================================
                  RFA_RF_GSM_MSG_ID_IDLE_FRAME_PROCESSING
=============================================================================*/
/*  RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQ          */
/*! @struct rfa_rf_gsm_idle_frame_processing_req_s*/
/*! @brief This structure is the payload for triggering the idle frame 
 *  functions. One example could be for Thermistor Reads.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_idle_frame_processing_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_IDLE_FRAME_PROCESSING_CNF          */
/*! @struct rfa_rf_gsm_idle_frame_processing_cnf_s*/
/*! @brief This structure is the payload for confirming the completion of the
 *  idle frame functionality.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       ip2_cal_req;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_idle_frame_processing_cnf_s;

/*===========================================================================*/



/*=============================================================================
                        RFA_RF_GSM_MSG_ID_VREG_ON
=============================================================================*/
/*  RFA_RF_GSM_VREG_ON_REQ          */
/*! @struct rfa_rf_gsm_vreg_on_req_s*/
/*! @brief This structure is the payload for triggering a particular Vreg On
 *  activity via MCPM.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_vreg_on_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_VREG_ON_CNF          */
/*! @struct rfa_rf_gsm_vreg_on_cnf_s*/
/*! @brief This structure is the payload for confirmation that RF has triggered
 *  a Vreg On activity.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - subscription command is for
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_vreg_on_cnf_s;
/*===========================================================================*/



/*=============================================================================
                        RFA_RF_GSM_MSG_ID_VREG_OFF
=============================================================================*/
/*  RFA_RF_GSM_VREG_OFF_REQ          */
/*! @struct rfa_rf_gsm_vreg_off_req_s*/
/*! @brief This structure is the payload for triggering a particular Vreg Off
 *  activity via MCPM.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_vreg_off_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_VREG_OFF_CNF        */
/*! @struct rfa_rf_gsm_vreg_off_cnf_s*/
/*! @brief This structure is the payload for confirmation that RF has triggered
 *  a Vreg Off activity.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_vreg_off_cnf_s;
/*=============================================================================
                        RFA_RF_GSM_MSG_ID_HAL_VOTE
=============================================================================*/
/*  RFA_RF_GSM_HAL_VOTE_REQ          */
/*! @struct rfa_rf_gsm_hal_vote_req_s*/
/*! @brief This structure isÙ the payload for enabling/disabling a HAL bus
    vote without a TRM lock.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  vote - boolean denoting enable (true) or disable (false)
 *  @param  req_result - Result detailing the success/failure of the command
 */

typedef struct
{
  msgr_hdr_struct_type          hdr;
  boolean                       cnf_required;
  boolean                       vote;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_hal_vote_req_s;


/*  RFA_RF_GSM_HAL_VOTE_CNF          */
/*! @struct rfa_rf_gsm_hal_vote_cnf_s*/
/*! @brief This structure is the response for enabling/disabling a HAL bus
    vote without a TRM lock.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  vote - boolean denoting enable (true) or disable (false)
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  boolean                       cnf_required;
  boolean                       vote;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_hal_vote_cnf_s;
/*===========================================================================*/



/*=============================================================================
                   RFA_RF_GSM_MSG_ID_EXIT_MODE
=============================================================================*/
/*  RFA_RF_GSM_EXIT_MODE_REQ          */
/*! @struct rfa_rf_gsm_exit_mode_req_s*/
/*! @brief This structure is the payload for triggering an exit mode activity 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for exit mode)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;      //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_exit_mode_type          exit_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;  //legacy support
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_exit_mode_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_EXIT_MODE_CNF        */
/*! @struct rfa_rf_gsm_exit_mode_cnf_s*/
/*! @brief This structure is the payload for triggering an exit mode activity 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for exit mode)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;      //legacy support
  rfcom_device_enum_type        prx_device;
  rfcom_device_enum_type        drx_device;
  rfgsm_exit_mode_type          exit_mode;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;  //legacy support
  rfa_rf_gsm_cmd_outcome_t      req_result;
  rfa_rf_gsm_cmd_outcome_t      prx_device_req_result;
  rfa_rf_gsm_cmd_outcome_t      drx_device_req_result;
} rfa_rf_gsm_exit_mode_cnf_s;
/*===========================================================================*/



/*=============================================================================
                   RFA_RF_GSM_MSG_ID_SET_SAR_LIMIT
=============================================================================*/
/*  RFA_RF_GSM_SET_SAR_LIMIT_IND  */
/*! @struct rfa_rf_gsm_set_sar_limit_ind_s*/
/*! @brief This structure is the payload for confirmation that RF has received
 *  a sar limit update
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  rfa_rf_gsm_sar_limit_params - SAR limit params from MCS to RF
 */
typedef struct
{
  msgr_hdr_struct_type		   hdr;
  rfa_rf_gsm_sar_limit_params_type rfa_rf_gsm_sar_limit_params;
}rfa_rf_gsm_set_sar_limit_ind_s;
/*===========================================================================*/



/*=============================================================================
                   RFA_RF_GSM_MSG_ID_CM_ENTER
=============================================================================*/
/*  RFA_RF_GSM_CM_ENTER_REQ          */
/*! @struct rfa_rf_gsm_cm_enter_req_s*/
/*! @brief This structure is the payload for triggering an cm enter for iRAT 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for cm enter mode)
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfgsm_timing_info_type        *timing_info;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_cm_enter_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_CM_ENTER_CNF        */
/*! @struct rfa_rf_gsm_cm_enter_cnf_s*/
/*! @brief This structure is the payload for confirmation of cm enter for iRAT 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for cm enter mode)
 *  @param  timing_info - Timings required to setup monitor, Rx and Tx bursts
 *  @param  sawless_sup - indicates sawless support for GSM bands on all logical devices
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfgsm_timing_info_type        *timing_info;
  /* It is necessary to report the sawless support for all devices during an enter mode in case of WTR hopping */
  boolean                       sawless_sup[RFM_MAX_WAN_DEVICES][RFGSM_MAX_UNIQUE_RFCOM_BANDS]; /* Indicate which bands on all devices are sawless */
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_cm_enter_cnf_s;
/*===========================================================================*/

/*=============================================================================
                    RFA_RF_GSM_MSG_ID_RX_ENTER_MODE
=============================================================================*/
/*RFA_RF_GSM_RX_ENTER_MODE_REQ*/
/*! @struct rfa_rf_gsm_rx_enter_mode_req_s*/
/*! @brief  This structure is the payload for initialising the RF Driver for 
 *  a specific radio receiver for GSM.
 *  @param hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param device - RFM device type to be passed from L1 to RF 
 *  @param cnf_required - L1 to request RF to send confirmation msg or not
 *  @param rxlm_buf_idx - Rx Link Manager Buffer Index
 *  @param init_writes_buff_ptr - smem location buffer for intf index
 */

typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint32                        rxlm_buf_idx;
  void                          *init_writes_buff_ptr;
} rfa_rf_gsm_rx_enter_mode_req_s;

/*===========================================================================*/
/*RFA_RF_GSM_RX_ENTER_MODE_CNF*/
/*! @struct rfa_rf_gsm_rx_enter_mode_cnf_s*/
/*! @brief  This structure is the payload for confirming the completion of 
 *  initialisation of the RF Driver for a specific radio transceiver for GSM.
 *  @param hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param device - RFM device type to be passed from L1 to RF 
 *  @param cnf_required - L1 to request RF to send confirmation msg or not
 *  @param rxlm_buf_idx - Rx Link Manager Buffer Index
 *  @param init_writes_buff_ptr - smem location buffer for intf index
 *  @param req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint32                        rxlm_buf_idx;
  void                          *init_writes_buff_ptr;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_rx_enter_mode_cnf_s;

/*=============================================================================
                   RFA_RF_GSM_MSG_ID_CM_EXIT
=============================================================================*/
/*  RFA_RF_GSM_CM_EXIT_REQ          */
/*! @struct rfa_rf_gsm_cm_exit_req_s*/
/*! @brief This structure is the payload for triggering an cm exit for iRAT 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for cm exit mode)
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_cm_exit_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_CM_EXIT_CNF        */
/*! @struct rfa_rf_gsm_cm_exit_cnf_s*/
/*! @brief This structure is the payload for confirmation of cm exit for iRAT 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for cm exit mode)
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_cm_exit_cnf_s;
/*===========================================================================*/



/*=============================================================================
                   RFA_RF_GSM_MSG_ID_TEMP_COMP_UPDATE
=============================================================================*/
/*  RFA_RF_GSM_TEMP_COMP_UPDATE_REQ          */
/*! @struct rfa_rf_gsm_update_temp_comp_req_s*/
/*! @brief This structure is the payload for updating the temp compensation information 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from RF Apps to RF Task
 *  @param  cnf_required - RF Apps to request RF to send confirmation msg or not
 *  @param  temp_comp_scaled_value - temp comp payload to be updated in rf task
 *  @param  temp_bin_16 - current temp bin index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  int16                         temp_comp_scaled_value;
  uint8                         temp_bin_16;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_update_temp_comp_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_TEMP_COMP_UPDATE_CNF        */
/*! @struct rfa_rf_gsm_temp_comp_update_cnf_s*/
/*! @brief This structure is the payload for confirmation of temp comp update 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from RF Apps to RF Task
 *  @param  cnf_required - RF Apps to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_update_temp_comp_cnf_s;
/*===========================================================================*/



/*=============================================================================
                    RFA_RF_GSM_MSG_ID_GTA_ENTER_MODE
=============================================================================*/
/*RFA_RF_GSM_GTA_ENTER_MODE_REQ*/
/*! @struct rfa_rf_gsm_gta_enter_mode_req_s*/
/*! @brief  This structure is the payload for initialising the RF Driver for 
 *  a specific radio transceiver for GSM.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for gta enter mode)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;
} rfa_rf_gsm_gta_enter_mode_req_s;

/*===========================================================================*/
/*RFA_RF_GSM_GTA_ENTER_MODE_CNF*/
/*! @struct rfa_rf_gsm_gta_enter_mode_cnf_s*/
/*! @brief  This structure is the payload for confirming the completion of 
 *  initialisation of the RF Driver for a specific radio transceiver for GSM.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  buffer_id - which RF buffer to use (should be 0 for gta enter mode)
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         buffer_id;
  uint32                        rxlm_buf_idx;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_gta_enter_mode_cnf_s;


/*=============================================================================
                   RFA_RF_GSM_MSG_ID_GTA_EXIT_MODE
=============================================================================*/
/*  RFA_RF_GSM_GTA_EXIT_MODE_REQ          */
/*! @struct rfa_rf_gsm_gta_exit_mode_req_s*/
/*! @brief This structure is the payload for triggering a GTA exit mode activity 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */ 
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint32                        rxlm_buf_idx;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_gta_exit_mode_req_s;


/*===========================================================================*/
/*RFA_RF_GSM_GTA_EXIT_MODE_CNF*/
/*! @struct rfa_rf_gsm_gta_enter_mode_cnf_s*/
/*! @brief  This structure is the payload for confirming the completion of 
            gsm tune away exit command.
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  rxlm_buf_idx - Rx Link Manager buffer index
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint32                        rxlm_buf_idx;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_gta_exit_mode_cnf_s;

/*=============================================================================
                   RFA_RF_GSM_MSG_ID_SET_ANTENNA
=============================================================================*/
/*  RFA_RF_GSM_SET_ANTENNA_REQ          */
/*! @struct rfa_rf_gsm_set_antenna_req_s*/
/*! @brief This structure is the payload for updating the antenna position information 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - Subscription ID to be returned to GL1
 *  @param  ant_pos - set antenna payload to be updated in rf task
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 *  @param  l1_cb_func - Function to call when switch complete ack comes from FW
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  uint8                         ant_pos;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
  rfgsm_l1_cb_type              l1_cb_func;
} rfa_rf_gsm_set_antenna_req_s;

/*===========================================================================*/
/*  RFA_RF_GSM_SET_ANTENNA_CNF        */
/*! @struct rfa_rf_gsm_set_antenna_cnf_s*/
/*! @brief This structure is the payload for confirmation of set antenna update 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_set_antenna_cnf_s;
/*===========================================================================*/

/*=============================================================================
                   RFA_RF_GSM_MSG_ID_START_IP2_CAL
=============================================================================*/


typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  //uint8                         num_ip2_cal_steps;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_start_ip2_cal_req_s;


/*===========================================================================*/
/*  RFA_RF_GSM_START_IP2_CAL_CNF        */

typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_start_ip2_cal_cnf_s;
/*===========================================================================*/

typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  uint32                        ip2_rxlm_buffer_idx;
  uint32                        ip2_txlm_buffer_idx;
  boolean                       cnf_required;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_ip2_cal_msm_req_s;

typedef struct
{
  msgr_hdr_struct_type          hdr;
  rfcom_device_enum_type        device;
  uint8                         sub_id;
  boolean                       cnf_required;
  uint8                         num_cal_steps;
  rfa_rf_gsm_cmd_outcome_t      req_result;
} rfa_rf_gsm_ip2_cal_msm_cnf_s;

/*===========================================================================*/
/*=============================================================================
                   RFA_RF_GSM_MSG_ID_PROCESS_BURST_METRICS
=============================================================================*/

/*! @struct rfa_rf_gsm_process_burst_metrics_req_s*/
/*! @brief This structure is the header used to update RFSW with burst metrics for 
 *         use in determining the High/Low linearity state required on a SAWless device 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - The subscription ID associated with the metrics
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  num_metrics_entries - The number of seperate metrics entries to process
 *  @param  metrics - Array of pointers (up to RFGSM_MAX_METRICS_ENTRIES) to burst metrics
 *          structures to be processed
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type              hdr;
  rfcom_device_enum_type            device;
  uint8                             sub_id;
  boolean                           cnf_required;
  uint8                             num_metrics_entries;
  rfa_rf_gsm_rx_burst_metrics_type* metrics[RFGSM_MAX_METRICS_ENTRIES];
  rfa_rf_gsm_cmd_outcome_t          req_result;
} rfa_rf_gsm_process_burst_metrics_req_s;

/*===========================================================================*/

/*! @struct rfa_rf_gsm_process_burst_metrics_cnf_s*/
/*! @brief This structure is the header used to update RFSW with burst metrics for 
 *         use in determining the High/Low linearity state required on a SAWless device 
 *  @param  hdr - MSGR details for each UMID i.e priority, sending module etc
 *  @param  device - RFM device type to be passed from L1 to RF device
 *  @param  sub_id - The subscription ID associated with the metrics
 *  @param  cnf_required - L1 to request RF to send confirmation msg or not
 *  @param  req_result - Result detailing the success/failure of the command
 */
typedef struct
{
  msgr_hdr_struct_type              hdr;
  rfcom_device_enum_type            device;
  uint8                             sub_id;
  boolean                           cnf_required;
  rfa_rf_gsm_cmd_outcome_t          req_result;
} rfa_rf_gsm_process_burst_metrics_cnf_s;

#endif /* RF_GSM_MSG_H */



