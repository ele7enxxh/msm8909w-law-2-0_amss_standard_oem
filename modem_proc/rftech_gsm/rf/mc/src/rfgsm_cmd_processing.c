
/*!
  @file rfgsm_cmd_processing.c 

  @brief
  This file dispatches and sends responses for any REQ that GSM RF driver receives.

*/

/*===========================================================================


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
Copyright (c) 1999 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

/*===========================================================================


                EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/mc/src/rfgsm_cmd_processing.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $ 2

when       who     what, where, why
--------   ---     ----------------------------------------------------------

05/02/16   krk     Adding debugging code to check rfm_mode is not rfcom_gsm_mode in rfgsm_dispatch_rx_burst_req
05/04/16   bp      RFHW information in F3 messages during entermode of GSM
02/23/16   par     Add MC logging buffer for failing sequences
12/23/15   bp      Stop further processing of therm_read_rsp if temp. compensation is stopped through GSM Tx Disable.
11/25/15   bp      FR25072: for device wakeup, vote for lowest/first supported band
02/18/15   sml     Add FEATURE_JOLOKIA_MODEM featurization
02/05/15   par     Move logging under sawless logging flag
01/22/15   piy     Fixed bug in Device status read feature for Mon Burst
1/13/15    sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
12/16/14   sml     Added NULL condition for ASD respponse payload data 
12/16/14   sml     Set variable set_antenna_position to default state if 
                   requested position is already set in set_antenna_req
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/25/14   sc      Err Fatal on bad input from GL1
11/25/14   sw      Disable therm read during WTR dedicated hopping
11/18/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa
11/12/14   tsr     Fix bug for Idle mode RXD
10/21/14   sc      Reduce code execution limiting use of get qsym API
10/06/14   sc      Deprecate unused functionality
10/10/14   tsr     Removed hardcoding of GLI lower CCS prio flag to false
09/30/14   tsr     GSM IDLE Mode RXD support
09/30/14   tws     Remove the wakeup callback function pointer
09/19/14   sc      Force RxLM update for every wakeup
09/11/14   sml     Adding start API Pcycle to know about pre-emption done to RF task
09/03/14   tws     Replace rfgsm_rfm_status_type with rfa_rf_gsm_cmd_outcome_t
08/19/14   zbz     Add RF GSM CCS execution time profiling
08/11/14   shb     Fixed offtarget compiler error
08/11/14   sw      Print the sub ID as part of the error message (improved logging)
08/11/14   sw      GL1 only expects ASD callback. Do not send Antenna CNF message
08/07/14   ggs     Add PRX and DRX request result to enter mode and wakeup 
                   confirmation messages
07/30/14   cj      Added changes for Tx sharing feature
07/28/14   hoh     Detect failure case for invalid Tx device passed from L1
07/28/14   ec      Re-enable burst metrics processing with corrected API
07/21/14   sw      Replace gas_id with sub_id in the ASD callback function
07/14/14   ec      Comment out burst metrics processing until API can be corrected
07/14/14   ec      Fix critical static analysis error due to out of bounds array reference
07/14/14   ec      Create and register interface to receive burst metrics from GL1
07/12/14   sw      Replace RFGC_T_ERR macro with a function (to reduce memory footprint)
07/09/14   sw      Introduce new macro to either error fatal or print message depending on nv bit
07/01/14   tsr     DR-DSDS: Lower RX CCS event priority when GSM sub is in Idle mode
07/01/14   jr      Enable therm read on sub-2
06/30/14   ec      Mainline Rx Diversity
06/30/14   sc      Do not place rfm mode to parked on exit mode
06/27/14   sc      RxLM and dev ID per sub and minimised call-flow for RxLM update logic
06/26/14   sw      Fix small bug where incorrect task_id is printed
06/18/14   sw      Remove calls to internal geran function gl1_get_FN() from RF
                   (port CR609471 from Dime)
06/12/14   sw      Use GL1 api to map sub_id to as_id
06/11/14   sc/ggs  Add flag to check if rxlm settings update is required during
                   wake up
06/10/14   hk      Edge core parameter from GL1.
09/06/14   sw      Port from Dime - GSM RF needs to send a command response to GL1 indicating that asdiv switch is already in requested position (CR640557)
06/06/14   ec      Do not update RFM state to parked when exiting GSM
06/06/14   sw      Fix ASD KW error + ASD code optimisation
06/02/14   hk      Adding wakeup call back to L1.
05/28/14   sw      Move therm read from enter mode to tx enable
05/20/14   sw      Fix OFT compiler errors
05/15/14   sw      Return enter mode and wakeup execution times to GL1
05/02/14   sc      Default as_id to same as sub_id preventing g_view multiSIM crash
02/12/14   sw      Move prepare therm read settings from enter mode to TX enable
04/24/14   sw      Small correction to ASD F3 message
04/23/14   tsr     KW Fixes
04/16/14   tsr     GFW/Interface update for GSM RX Diversity
04/16/14   sb      IP2 cal Interface and functionality changes
04/09/14   sc      Manage rfm_mode state machine
04/07/14   ggs     KW fixes
03/31/14   tsr     Added support for handling combined Enter Mode for PRX and DRX
03/26/14   tsr     Add support to query rfm device for TX capability
03/26/14   sb      Add support for IP2 cal for Dime PM
03/25/14   sc      Use subscription from GL1
03/24/14   sc      Remove dynamic task priority manipulation
03/12/14   tsr     Added RXD support for RX burst 
03/12/14   sw      ASD tidyup
03/07/14   sw      Fix OFT compilation error (no need for UMID featurisation)
03/04/14   sw      Introduce ASD message router definitions and remove GFW framework
02/28/14   sc      Pass core data through mc to core layers
02/28/14   sw      Therm read error handling
02/26/14   sc      Pass core data through cmd processing and mc layer
02/20/14   tsr     GSM RX Diversity
02/20/14   sc      Remove call to rfm_wakeup and call mc directly
02/20/14   ec	   Feature guard therm read response structure for development build compilation
02/11/14   sw      Remove therm read Triton featurisation
30/01/14   sc      Give cnf required control to GL1 and correct Rx enter callflow
01/28/14   sw      Block registration of RFLM RSP UMIDS for all RF SUBS
12/10/13   sc      Switch to use get timing info from enter mode and cm enter
11/05/13   ec	   Remove empty parameters from F3 macros to save memory
10/15/13   sc      Mainline L1-RF interface changes
09/20/13   par     Update RF task priority setting
08/16/13   ec	   Fix incorrect Task ID
08/16/13   ec	   Fix error in GTA Exit Mode; response UMID set incorrectly for tasks 2 and 3;
                   wrong return value in success case; incorrect type used for sizing;
                   request type missing from union
08/08/13   sml     Fixing KW warning
07/30/13   pl      Enable new therm read interface
07/30/13   sml     Adding ASD RSP from GFW
07/29/13   pl      use FEATURE_RF_ASDIV instead of FEATURE_GSM_ASD_RSP
07/29/13   sml     Generic RFSW-FW interaction to generate an ACK for a CCS event
07/29/13   sml/svi ASD support for GSM tech
07/29/13   tws     Initial support for RX diversity commands
07/29/13   tws     Use new RF_TASK_1 for GSM.
07/24/13   ec      Map rf_tasks to subscriptions at msg registration. Pass rf_task
                   into relevant MC layer functions
07/22/13   sc      Check triple buffer ID passed from L1
07/23/13   ec	   Do not always return in case of NULL pointer, to preserve dispatcher call-flow
07/22/13   ec	   Use new F3s/Null pointer checks
07/17/13   sc      Merge Vreg On/Off to Enter/Exit Wakeup/Sleep and merge get timing
07/16/13   sc      Move mc/core functionality out of dispatcher and into mc/core layer
07/15/13   sc      Add struct and enum that only used by this module to remove header
07/12/13   sc      Improve the dispatch variable checking and error handling
07/12/13   av      GTA EXIT mode
07/10/13   sc      Clean up result variables and add definitions for dispatcher verifications
07/10/13   sc      Use msg ptr instead of echo response
07/10/13   sml     Fixing off-target error
07/09/13   sml     Block registration of GFW RSP UMIDS for 2 SUBS
06/28/13   ec      Fix incomplete porting of GSM Tune Away (GTA) enter mode
06/28/13   ec      Log the rf_task context commands are executing in.
06/25/13   ec      Make changes to support Therm Read Cancellation
06/20/13   ec      Fix compiler error on single sim targets
06/20/13   ec      Add support for GSM Tune Away (GTA) enter mode
05/30/13   tws     Add support for GSM sub 3
05/30/13   tws     Make the GFW queue task specific.
04/09/13   sc      Handle return from mc exit mode
04/08/13   sc      Reassign the core RXLM variable during wakeup from GL1
04/09/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
03/18/13   svi     Replaced memcpy() by safer memscpy().
03/12/13   tsr     Added to tag to each therm read event handle to identify the successful retrieval of that handle
03/05/13   sb      Call Vbatt read/Vbatt comp APIs in online mode
02/26/13   sc      Register the REQ cmds for CM enter/exit with RF task
02/25/13   sc      Added cm enter/exit to init/de-init iRAT during gl1 cm init/terminate 
02/15/13   sc      Call rfgsm_mc_exit_mode
02/11/13   sc      Clean up F3s adding meaningful details and add logging
02/11/13   sc      Add GSM mode set for enter standalone and exit standalone
02/07/13   sc      Fix compiler warnings due to rfm_exit_mode check-in
02/06/13   sc      Call rfm_exit_mode to ensure previous tech is cleaned up
02/04/13   tsr     Fix Klockwork Errors from previous check-in.
01/31/13   tsr     Added framework for recieving therm read response from GFW
01/31/13   sc      Added cnf required flag. Temp default TRUE avoiding coordinated check-in
01/29/13   sc      Dynamically adjust the RF Task Priority
01/22/13   av      Removing the contraint that each rf_task processes messages based on rfm_device
01/11/13   aka     Added support for simultaneous Tx SAR
01/10/13   av      Create new REQ and CNF ids for subscriber 2 and handle them in rf_Task_2
01/09/13   sr      Made changes to run vbatt/temp comp in rf app task context.
01/07/12   sc      Fix not setting Tx enable result to success
01/03/12   sc      Added RF<->FW msgr interface CMD/RSP and framework
12/18/12   sc      Added therm read event generation framework
12/18/12   sc      Remove unused tx dynamic ptr
12/17/12   sc      Fix compiler warning due to unused variable in exit mode
12/14/12   sc      Remove unrequired iRAT req and add exit mode framework
12/11/12   sc      Ensure error handling of NULL ptr from L1 for Set Tx band
12/07/12   tws     Fix Klocwork errors.
12/06/12   tws     Fix Klocwork errors.
12/05/12   aka     Removed some hardcoding for device id
11/28/12   av      Enabling the rf_task to process only sub0 and rf_task_2 to process to ony sub1
                   enabling it now as gl1 has fixed passing the right rfm_device to rf in all the messages.
11/26/12   aka     Compiler warning fixes
11/26/12   aka     Changes to use a path specific gsm handle
11/23/12   tws     Remove deprecated GERAN includes.
11/21/12   av      Added rfm_device parameters to mc/core/mdsp/msm layers.
11/06/12   sc      Cleanup unrequired features/definitions
10/12/12   sc      Added support for iRAT startup cleanup RF Task implementation
10/10/12   sc      Move all Rx burst mc API calls to new prepare Rx CCS event mc API
10/05/12   av      Expanded rf task architecture to include more tasks. 
10/04/12   sc      Move to new RF Task dispatcher method to avoid UMID REX conflicts
10/03/12   tsr     First cut of RF time profile.
08/28/12   ggs     Remove unnecessary wrong device ID F3 messages  
08/24/12   sb/tsr  Made changes to clear the NON SBIs only during band change and SBIs during setup TX/RX burst
08/16/12   sc      Added bringup debug messages and code 
08/08/12   sc      Check device from GL1 before passing to RFGSM MC APIs
08/07/12   sc      Clearing seperate Rx and Tx scripts for CCS buffer tracability
08/06/12   sc      Added clearing of rf hal buffers before use
08/06/12   sc      Edit API to update CCS header FW vars
08/02/12   sc      Add num slots to setup Tx burst API
07/26/12   sr      Changes to APIS to take rfm_dev as parameter.
07/24/12   sc      Added RFM_DEVICE to the payloads
07/06/12   sc      Edit API parameters to support CCS buffer format
07/06/12   sc      Remove unrequired num transactions init functionality
06/27/12   sc      Remove no longer required parameter from Set Tx Band function call
06/07/12   sc      Cleaned up dispatcher verification and results
06/06/12   sc      Remove RF Manager calls and replace with RF Main Control calls
04/26/12   sc      Added shared memory interface support, integrating buffer ptrs
04/24/12   ggs     Replaced FEATURE_RF_HAS_GSM with FEATURE_GSM
03/20/12   sc      Edited dispatch functions' process-flow
03/13/12   ggs     Fixed typo in rfgsm_dispatch_outcome_t
02/27/12   ggs     Removed Set Tx power request. This will be added as part 
                   of the set TX burst payload.
                   Added Tx power control buffer to enter mode confirmation message.
02/13/12   sc      Removed dispatch init/de-init prototypes
02/08/12   ggs     Updated header comments info and response parameters
01/27/12   sc      Initial version
  

===========================================================================*/

/*===========================================================================

                  INCLUDE FILES

===========================================================================*/
#include "amssassert.h"
#include "appmgr.h"
#include "rex.h"
#include "msgr.h"
#include "rf_cap.h" /*Contains FEATURE_RF_TASK */
#include "rfa_msgr.h"
#include "rf_task.h"
#include "rf_cmd_interface.h"
#include "msgr_rex.h"
#include "rf_cmd_dispatcher.h"
#include "rfm.h"
#include "rfm_types.h"
#include "rfmeas_types.h"
#include "stringl.h"
#include "rfcom.h"
#include "rfgsm_data_manager.h"
#ifdef FEATURE_GSM
/*Need to place gsm specific msgr headers here*/
#include "rfgsm_mc.h"
#include "rfgsm_core.h"
#include "rfgsm_core_log.h"
#include "rf_hal_buffer.h"
#include "rfm_internal.h"
#include "gtmrs.h"
#include "qurt.h"
#include "rflm_gsm_msg.h"
#endif
#include "rf_task_common_services.h"
#include "rflm_gsm_dm.h"
#include "gl1_hw.h"
#include "rfgsm_nv_cmn.h"

#ifdef FEATURE_RF_ASDIV
#include "modem_mem.h"
#include "rfcommon_asdiv_manager.h"
#endif
#include "rfcommon_nv_mm.h"

#include "geran_test_mode_api.h"



/*===========================================================================

                                DEFINITIONS

===========================================================================*/
/*!
  @brief
  Maintains the information about what transceivers are used by what
  technologies.
*/
extern rfcom_mode_enum_type* rfgsm_mode;

/*! @brief Union of all RF receiving msgs that RF task can receive */
typedef union
{
  rfa_rf_gsm_enter_mode_req_s               rfa_rf_gsm_enter_mode;
  rfa_rf_gsm_rx_enter_mode_req_s            rfa_rf_gsm_rx_enter_mode;  
  rfa_rf_gsm_gta_enter_mode_req_s           rfa_rf_gsm_gta_enter_mode;
  rfa_rf_gsm_gta_exit_mode_req_s            rfa_rf_gsm_gta_exit_mode;
  rfa_rf_gsm_tx_enable_req_s                rfa_rf_gsm_tx_enable;
  rfa_rf_gsm_tx_disable_req_s               rfa_rf_gsm_tx_disable;
  rfa_rf_gsm_wakeup_req_s                   rfa_rf_gsm_wakeup;
  rfa_rf_gsm_sleep_req_s                    rfa_rf_gsm_sleep;
  rfa_rf_gsm_rx_burst_req_s                 rfa_rf_gsm_rx_burst;
  rfa_rf_gsm_set_tx_band_req_s              rfa_rf_gsm_set_tx_band;
  rfa_rf_gsm_tx_burst_req_s                 rfa_rf_gsm_tx_burst;
  rfa_rf_gsm_get_timing_infor_req_s         rfa_rf_gsm_get_timing_infor;
  rfa_rf_gsm_idle_frame_processing_req_s    rfa_rf_gsm_idle_frame_processing;
  rfa_rf_gsm_vreg_on_req_s                  rfa_rf_gsm_vreg_on;
  rfa_rf_gsm_vreg_off_req_s                 rfa_rf_gsm_vreg_off;
  rfa_rf_gsm_exit_mode_req_s                rfa_rf_gsm_exit_mode;
  rfa_rf_gsm_cm_enter_req_s                 rfa_rf_gsm_cm_enter;
  rfa_rf_gsm_cm_exit_req_s                  rfa_rf_gsm_cm_exit;
  rfa_rf_gsm_update_temp_comp_req_s         rfa_rf_gsm_update_temp_comp;
#ifdef FEATURE_RF_ASDIV
  rfa_rf_gsm_set_antenna_req_s              rfa_rf_gsm_set_antenna; 
#endif
  rfa_rf_gsm_process_burst_metrics_req_s    rfa_rf_gsm_process_burst_metrics;
} rfgsm_msg_u;

/*! @brief Outcome of the dispatch. 
 *  @details The different outcomes of the dispatch to RF task used by GSM MC
 *           Dispatcher
 */ 
typedef enum
{
  RFGSM_DISPATCH_SUCCESSFUL=1,
  RFGSM_DISPATCH_UNSUCCESSFUL=2,
  RFGSM_DISPATCH_NULL_PTR=3,
  RFGSM_DISPATCH_INVALID_PARAM =4,
  RFGSM_DISPATCH_INVALID_DEVICE = 5,
  RFGSM_DISPATCH_NULL_INPUT_ARG = 6,
  RFGSM_DISPATCH_STATE_MACHINE_ERR = 7,
  RFGSM_DISPATCH_MAX
}rfgsm_dispatch_outcome_t;

/* RFLM interface variables */
typedef union
{
  msgr_hdr_struct_type   hdr;
  rflm_gsm_therm_read_rsp_t rflm_gsm_therm_read_rsp; 
  rflm_gsm_asd_rsp_t        rflm_gsm_asd_rsp;
  rflm_gsm_device_status_read_rsp_t rflm_gsm_device_status_read_rsp;
} rfgsm_rflm_msg_u;

typedef struct ALIGN(8)
{
  q_link_type     link;
  rfgsm_rflm_msg_u msg;
} rfgsm_rflm_msgr_t;

#define RFGSM_RFLM_MSGR_QUEUE_SIZE 6
q_type  rfgsm_rflm_cmd_intf_msgr_q[RF_MAX_TASK];
q_type  rfgsm_rflm_cmd_intf_msgr_free_q[RF_MAX_TASK];
rfgsm_rflm_msgr_t  rfgsm_rflm_msgr[RF_MAX_TASK][RFGSM_RFLM_MSGR_QUEUE_SIZE];

/*! @brief This array holds the request (REQ) command list used to register
 *  the whole list with MSGR.
 */
static msgr_umid_type rfgsm_umid_req_list[] = 
{
  RFA_RF_GSM_ENTER_MODE_REQ,
  RFA_RF_GSM_RX_ENTER_MODE_REQ,
  RFA_RF_GSM_GTA_ENTER_MODE_REQ,
  RFA_RF_GSM_GTA_EXIT_MODE_REQ,
  RFA_RF_GSM_WAKEUP_REQ,
  RFA_RF_GSM_SLEEP_REQ,
  RFA_RF_GSM_TX_ENABLE_REQ,
  RFA_RF_GSM_TX_DISABLE_REQ,
  RFA_RF_GSM_RX_BURST_REQ,
  RFA_RF_GSM_SET_TX_BAND_REQ,
  RFA_RF_GSM_TX_BURST_REQ,
  RFA_RF_GSM_GET_TIMING_INFOR_REQ,
  RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQ,
  RFA_RF_GSM_VREG_ON_REQ,
  RFA_RF_GSM_VREG_OFF_REQ,
  RFA_RF_GSM_EXIT_MODE_REQ,
  RFA_RF_GSM_SET_SAR_LIMIT_IND,
  RFA_RF_GSM_CM_ENTER_REQ,
  RFA_RF_GSM_CM_EXIT_REQ,
  RFA_RF_GSM_UPDATE_TEMP_COMP_REQ,
#ifdef FEATURE_RF_ASDIV
  RFA_RF_GSM_SET_ANTENNA_REQ, 
#endif
  RFA_RF_GSM_START_IP2_CAL_REQ,
  RFA_RF_GSM_INIT_MSM_IP2_CAL_REQ,
  RFA_RF_GSM_PROCESS_BURST_METRICS_REQ,
};


/*! @brief This array holds the request (REQ) command list used to register
 *  the whole list with MSGR for rf_task_2
 */
static msgr_umid_type rfgsm_umid_req_sub2_list[] = 
{
  RFA_RF_GSM_ENTER_MODE_SUB2_REQ,
  RFA_RF_GSM_RX_ENTER_MODE_SUB2_REQ,
  RFA_RF_GSM_GTA_ENTER_MODE_SUB2_REQ,
  RFA_RF_GSM_GTA_EXIT_MODE_SUB2_REQ,
  RFA_RF_GSM_WAKEUP_SUB2_REQ,
  RFA_RF_GSM_SLEEP_SUB2_REQ,
  RFA_RF_GSM_TX_ENABLE_SUB2_REQ,
  RFA_RF_GSM_TX_DISABLE_SUB2_REQ,
  RFA_RF_GSM_RX_BURST_SUB2_REQ,
  RFA_RF_GSM_SET_TX_BAND_SUB2_REQ,
  RFA_RF_GSM_TX_BURST_SUB2_REQ,
  RFA_RF_GSM_GET_TIMING_INFOR_SUB2_REQ,
  RFA_RF_GSM_IDLE_FRAME_PROCESSING_SUB2_REQ,
  RFA_RF_GSM_VREG_ON_SUB2_REQ,
  RFA_RF_GSM_VREG_OFF_SUB2_REQ,
  RFA_RF_GSM_EXIT_MODE_SUB2_REQ,
  RFA_RF_GSM_SET_SAR_LIMIT_SUB2_IND,
  RFA_RF_GSM_CM_ENTER_SUB2_REQ,
  RFA_RF_GSM_CM_EXIT_SUB2_REQ,
  RFA_RF_GSM_UPDATE_TEMP_COMP_SUB2_REQ,
#ifdef FEATURE_RF_ASDIV
  RFA_RF_GSM_SET_ANTENNA_SUB2_REQ,
#endif
  RFA_RF_GSM_PROCESS_BURST_METRICS_SUB2_REQ,
};


/*! @brief This array holds the request (REQ) command list used to register
 *  the whole list with MSGR for rf_task_2
 */
static msgr_umid_type rfgsm_umid_req_sub3_list[] = 
{
  RFA_RF_GSM_ENTER_MODE_SUB3_REQ,
  RFA_RF_GSM_RX_ENTER_MODE_SUB3_REQ,
  RFA_RF_GSM_GTA_ENTER_MODE_SUB3_REQ,
  RFA_RF_GSM_GTA_EXIT_MODE_SUB3_REQ,
  RFA_RF_GSM_WAKEUP_SUB3_REQ,
  RFA_RF_GSM_SLEEP_SUB3_REQ,
  RFA_RF_GSM_TX_ENABLE_SUB3_REQ,
  RFA_RF_GSM_TX_DISABLE_SUB3_REQ,
  RFA_RF_GSM_RX_BURST_SUB3_REQ,
  RFA_RF_GSM_SET_TX_BAND_SUB3_REQ,
  RFA_RF_GSM_TX_BURST_SUB3_REQ,
  RFA_RF_GSM_GET_TIMING_INFOR_SUB3_REQ,
  RFA_RF_GSM_IDLE_FRAME_PROCESSING_SUB3_REQ,
  RFA_RF_GSM_VREG_ON_SUB3_REQ,
  RFA_RF_GSM_VREG_OFF_SUB3_REQ,
  RFA_RF_GSM_EXIT_MODE_SUB3_REQ,
  RFA_RF_GSM_SET_SAR_LIMIT_SUB3_IND,
  RFA_RF_GSM_CM_ENTER_SUB3_REQ,
  RFA_RF_GSM_CM_EXIT_SUB3_REQ,
  RFA_RF_GSM_UPDATE_TEMP_COMP_SUB3_REQ,
  RFA_RF_GSM_PROCESS_BURST_METRICS_SUB3_REQ,
};

static msgr_umid_type rfgsm_rflm_rsp_umids[] =
{
  RFLM_GSM_THERM_READ_RSP,    
  RFLM_GSM_ASD_RSP,
  RFLM_GSM_DEVICE_STATUS_READ_RSP,
};

static msgr_umid_type rfgsm_rflm_rsp_sub2_umids[] =
{
  RFLM_GSM_THERM_READ_SUB2_RSP,
  RFLM_GSM_ASD_SUB2_RSP,
  RFLM_GSM_DEVICE_STATUS_READ_SUB2_RSP,
};



/*===========================================================================

                               Macros

===========================================================================*/
/*! @brief Definitions of the RF GSM L1 message count for the mailboxes.
*/
#define RF_GSM_REQ_CNT(x) \
    (sizeof(x)/sizeof(msgr_umid_type))

/*! @brief Definitions of the RF GSM GFW message count for the mailboxes.
*/
#define RF_GSM_RFLM_RSP_CNT(x) \
    (sizeof(x)/sizeof(msgr_umid_type))



/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
rfgsm_dispatch_outcome_t rfgsm_req_dispatch(rfgsm_msg_u* msg_ptr);
rfgsm_dispatch_outcome_t rfgsm_req_dispatch_2(rfgsm_msg_u* msg_ptr);
rfgsm_dispatch_outcome_t rfgsm_req_dispatch_3(rfgsm_msg_u* msg_ptr);

/*! @brief Function prototypes for the dispatching functionality.          */
rfgsm_dispatch_outcome_t rfgsm_dispatch_enter_mode_req( 
           rfa_rf_gsm_enter_mode_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_rx_enter_mode_req( 
           rfa_rf_gsm_enter_mode_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );		   

rfgsm_dispatch_outcome_t rfgsm_dispatch_tx_enable_req( 
           rfa_rf_gsm_tx_enable_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_tx_disable_req( 
           rfa_rf_gsm_tx_disable_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_wakeup_req( 
           rfa_rf_gsm_wakeup_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_sleep_req( 
           rfa_rf_gsm_sleep_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_rx_burst_req( 
           rfa_rf_gsm_rx_burst_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_set_tx_band_req( 
           rfa_rf_gsm_set_tx_band_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_tx_burst_req( 
           rfa_rf_gsm_tx_burst_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_idle_frame_processing_req( 
           rfa_rf_gsm_idle_frame_processing_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_exit_mode_req( 
           rfa_rf_gsm_exit_mode_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_sar_limit_ind( 
           rfa_rf_gsm_set_sar_limit_ind_s* rfgsm_msg_ptr, rf_task_num_type rf_task );

rfgsm_dispatch_outcome_t rfgsm_dispatch_therm_read_rsp( 
           rfgsm_rflm_msg_u* rfgsm_msg_ptr, rf_task_num_type rf_task );

rfgsm_dispatch_outcome_t rfgsm_dispatch_device_status_read_rsp( rfgsm_rflm_msg_u* rfgsm_msg_ptr,
                                                                rf_task_num_type rf_task );

#ifdef FEATURE_RF_ASDIV
rfgsm_dispatch_outcome_t rfgsm_dispatch_asd_read_rsp( rfgsm_rflm_msg_u* rfgsm_msg_ptr,rf_task_num_type rf_task );
#endif
rfgsm_dispatch_outcome_t rfgsm_dispatch_cm_enter_req( 
           rfa_rf_gsm_cm_enter_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_cm_exit_req( 
           rfa_rf_gsm_cm_exit_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

rfgsm_dispatch_outcome_t rfgsm_dispatch_update_temp_comp_req( 
           rfa_rf_gsm_update_temp_comp_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );

boolean rfgsm_dispatch_check_device( rfcom_device_enum_type device, uint32 umid );

extern int32 rfm_exit_mode (rfcom_device_enum_type device,
                     rfcom_mode_enum_type to_rfmode);
#ifdef FEATURE_RF_ASDIV
rfgsm_dispatch_outcome_t rfgsm_dispatch_set_antenna_req(
           rfa_rf_gsm_set_antenna_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );
#endif

rfgsm_dispatch_outcome_t rfgsm_dispatch_process_burst_metrics( 
            rfa_rf_gsm_process_burst_metrics_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf );
/*===========================================================================

                      FUNCTIONS

===========================================================================*/
/*===========================================================================
  FUNCTION:  rfgsm_send_msgr_cnf()
===========================================================================*/
/*! @fn rfgsm_send_msgr_cnf(msgr_hdr_struct_type* msg_ptr, uint32 umid, uint32 size){}
 *  @brief This function is called by any function that requires to send a 
 *  confirmation (CNF) MSG.
 *  @param msg_ptr a pointer to message router header structure type
 *  @param umid a uint32 of the Unique Message ID
 *  @param size the payload size
 *  @return errno_enum_type.
 */
errno_enum_type rfgsm_send_msgr_cnf( msgr_hdr_struct_type* msg_ptr, 
                                     uint32 umid, 
                                     uint32 size )
{
  errno_enum_type msgr_send_status = E_SUCCESS;
  
  if( msg_ptr == NULL)
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "MSGR Send NULL Message UMID 0x%X, error_no %d",umid);
    return (E_BAD_ADDRESS);
  }

  /* Send the CNF MSG back here */
  msgr_init_hdr( msg_ptr, MSGR_RFA_RF_GSM, umid );
    
  msgr_send_status = msgr_send( msg_ptr, size );
  return msgr_send_status;
}


/*===========================================================================
FUNCTION  rfgsm_send_msgr_rf_fw_intf_cmd
===========================================================================*/
errno_enum_type rfgsm_send_msgr_rf_fw_intf_cmd( msgr_hdr_struct_type* msg_ptr, 
                                                uint32 umid, 
                                                uint32 size )
{
  errno_enum_type msgr_send_status = E_SUCCESS;

  if( msg_ptr == NULL)
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "MSGR Send NULL Message UMID 0x%X, error_no %d",umid);
    return (E_BAD_ADDRESS);
  }

  /* initialise msgr header */
  msgr_init_hdr( msg_ptr, MSGR_GERAN_FW, umid);

  /* send msgr message to GFW */
  msgr_send_status = msgr_send( msg_ptr, size );

  return msgr_send_status;
}



/*===========================================================================
FUNCTION  rfgsm_dispatch_check_device
===========================================================================*/
/*! @fn rfgsm_dispatch_check_device
 *  @brief This funtion checks the validity of the device ID passed from L1
 *  @param device - rfm device ID
 *  @param umid - a uint32 of the Unique Message ID
 *  @return boolean - Pass or Fail indication
 */
boolean rfgsm_dispatch_check_device( rfcom_device_enum_type rfm_dev, uint32 umid )
{
  /* Check the device ID passed to RF is valid */
  if( rfm_dev >= RFM_MAX_WAN_DEVICES )
  {
    RFGC_MSG_1(MSG_LEGACY_ERROR,"Device invalid for UMID 0x%x", umid);
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================

  FUNCTION:  rfgsm_error_handler

===========================================================================*/
static void rfgsm_error_handler( rfgsm_dispatch_outcome_t dispatch_result, 
                                 uint32 umid, 
                                 rf_task_num_type rf_task,
                                 rfcom_device_enum_type rfm_dev )
{
  /* Note: that rf_task is used to determine which subscription the call was */
  /*       for because it is a 1-2-1 mapping.                                */

  /* Display meaningful error depending on dispatch result enum */
  switch( dispatch_result )
  {
    case RFGSM_DISPATCH_INVALID_DEVICE:
      ERR_FATAL("GL1 passed RFGSM invalid device ID %d for UMID 0x%08x on sub %d", rfm_dev, umid, rf_task);
      break;

    case RFGSM_DISPATCH_NULL_INPUT_ARG:
      ERR_FATAL("GL1 passed RFGSM NULL input pointer for UMID 0x%08x on sub %d device ID %d", umid, rf_task, rfm_dev);
      break;

    case RFGSM_DISPATCH_NULL_PTR:
      ERR_FATAL("Null pointer! GL1 sent command UMID 0x%08x on sub %d device ID %d", umid, rf_task, rfm_dev);
      break;
    
    case RFGSM_DISPATCH_INVALID_PARAM:
      ERR_FATAL("GL1 passed RFGSM invalid input args for UMID 0x%08x on sub %d device ID %d", umid, rf_task, rfm_dev);
      break;

    case RFGSM_DISPATCH_STATE_MACHINE_ERR:
      ERR_FATAL("GL1 called RF in an invalid sequence with UMID 0x%08x on sub %d device ID %d", umid, rf_task, rfm_dev);
      break;

    default:
      MSG_4(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "GL1->RFGSM dispatch UMID 0x%08x unhandled error %d on sub %d device ID %d", 
            umid, dispatch_result, rf_task, rfm_dev);
      break;
  }
}  



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_enter_mode_req

===========================================================================*/
/*! @fn rfgsm_dispatch_enter_mode_req( rfa_rf_gsm_enter_mode_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for GSM.
 *  @param rfgsm_msg_ptr a pointer to the enter_mode structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_enter_mode_req( rfa_rf_gsm_enter_mode_req_s* rfgsm_msg_ptr, 
                                                        rf_task_num_type rf_task,
                                                        uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_enter_mode_cnf_s   rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr_drx = NULL;
  rfgsm_core_data_type          rfgsm_core_data;
  rfgsm_prof_type               *prof_buff = NULL;
  uint8                         prof_buff_idx = 0;
  #ifdef FEATURE_TX_SHARING
  rflm_dm_handle_meta_s         handle_meta;
  boolean                       dm_status = FALSE;
  #endif

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  rfgsm_core_data.enable_rx_diversity = FALSE;
  /*core variable to indicate whether we received combined enter mode from GL1*/
  rfgsm_core_data.prx_drx_combined_mode =FALSE;

  /*To get the HWID of the RF card*/
   rfcommon_nv_tbl_type* common_nv_tbl_ptr = NULL;
  
   /*----------------------Print out HWID of RF card-----------------*/
   common_nv_tbl_ptr = rfcommon_nv_get_tbl_ptr();
   if (common_nv_tbl_ptr != NULL) 
   {
     MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "HWID of the RF Card is %d", common_nv_tbl_ptr->rf_hw_config);
   }
   else
   {
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,  "Common NV table pointer is NULL. Cannot get HWID of RF Card");
   }

  
  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_enter_mode_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if (rfgsm_msg_ptr->enter_mode == RFGSM_PRX_ENTER_MODE ||
      rfgsm_msg_ptr->enter_mode == RFGSM_PRX_DRX_ENTER_MODE ||
      rfgsm_msg_ptr->enter_mode == RFGSM_PRX_WTR_DED_HOP_ENTER_MODE ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->prx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->prx_device );

      if( rfgsm_core_handle_ptr == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
             "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->prx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      if( rfgsm_msg_ptr->init_writes_buff_ptr == NULL || 
          rfgsm_msg_ptr->timing_info == NULL )
      {
        RFGC_T_MSG( MSG_LEGACY_ERROR, 
           "RF GSM Enter Mode dispatcher failed L1 payload variable verification");
       dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    /* ----------------------------------------------------------------------- */
    /* Execution for PRX Enter Mode                                            */
    /* Verify payload parameters. */
    if ( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_3( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Enter Mode, dev_id=%d, shared_mem_ptr=0x%x, cnf=%d",
                    rfgsm_msg_ptr->prx_device,
                    rfgsm_msg_ptr->init_writes_buff_ptr,
                    rfgsm_msg_ptr->cnf_required );

      /* If the last mode was not GSM ensure to exit that mode first */
      if(rfgsm_mode[rfgsm_msg_ptr->prx_device] != RFCOM_GSM_MODE)
      {
        (void)rfm_exit_mode(rfgsm_msg_ptr->prx_device, RFCOM_GSM_MODE);
      }

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->prx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->prx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                       RFA_RF_GSM_ENTER_MODE_REQUEST_START, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       0,                     //duration us
                       start_pcycle,          //start time pcycles
                       0 );                   //duration pcycles

      #ifdef FEATURE_TX_SHARING
      /*Store device and Sub in handle meta */
      dm_status = rflm_dm_get_handle_info(rfgsm_msg_ptr->txlm_buf_idx, &handle_meta);
      if (dm_status == TRUE)
      {
        handle_meta.sub_id = rfgsm_msg_ptr->sub_id;
        handle_meta.devide_id = rfgsm_msg_ptr->prx_device;
        (void)rflm_dm_set_handle_info(rfgsm_msg_ptr->txlm_buf_idx, &handle_meta);
      }
      else
      {
        RFGC_T_MSG_3( MSG_LEGACY_ERROR, 
                      "Get meta info failed for TX handle: handle_id %d, sub_id %d, dev_id %d",
                      rfgsm_msg_ptr->txlm_buf_idx,
                      rfgsm_msg_ptr->sub_id, 
                      rfgsm_msg_ptr->prx_device );
      }
      RFGC_T_MSG_3( MSG_LEGACY_HIGH, 
                    "RF GSM Enter Mode TX handle meta info updated: handle_id %d, sub_id %d, dev_id %d",
                    rfgsm_msg_ptr->txlm_buf_idx, 
                    handle_meta.sub_id, 
                    handle_meta.device_id );
      #endif

      /* If WTR dedicated HOP, then sub is already in traffic so disable therm read on old device before hopping to new device */
      if ( rfgsm_msg_ptr->enter_mode == RFGSM_PRX_WTR_DED_HOP_ENTER_MODE) 
      {
        (void)rfgsm_mc_stop_therm_read( rfgsm_msg_ptr->prx_device );
      }

      /* Enter GSM mode */
      if( rfgsm_mc_enter_mode( &rfgsm_core_data,
                               rfgsm_msg_ptr->rxlm_buf_index.prx_idx, 
                               (void * const)rfgsm_msg_ptr->init_writes_buff_ptr,
                               rfgsm_msg_ptr->timing_info ) == RFGSM_MC_SUCCESS )
      {
        /* RFM mode is now GSM */
        rfgsm_mode[rfgsm_msg_ptr->prx_device] = RFCOM_GSM_MODE;

        /* Enter mode was successful */
        cmd_result = RFA_RF_GSM_SUCCESS;
        prx_device_cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        RFGC_T_MSG_2( MSG_LEGACY_ERROR, "Failed rfgsm_mc_enter_mode to mode for rfm device %d", 
                      RFCOM_GSM_MODE, rfgsm_msg_ptr->prx_device);
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {           
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Enter Mode dispatcher failed variable verification for PRX device %d", rfgsm_msg_ptr->prx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->prx_device );
    }
  }


  /*Validity check for DRX device*/
  if(rfgsm_msg_ptr->enter_mode == RFGSM_DRX_ENTER_MODE ||
    rfgsm_msg_ptr->enter_mode == RFGSM_PRX_DRX_ENTER_MODE ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->drx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
      rfgsm_core_data.enable_rx_diversity = FALSE;
      rfgsm_core_data.rxd_rfm_dev = RFM_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr_drx = rfgsm_core_handle_get( rfgsm_msg_ptr->drx_device );
      if( rfgsm_core_handle_ptr_drx == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                     "rfgsm_core_handle_ptr DRX is NULL for device %d!", rfgsm_msg_ptr->drx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
        rfgsm_core_data.enable_rx_diversity = FALSE;
        rfgsm_core_data.rxd_rfm_dev = RFM_INVALID_DEVICE;
      }
    }
    /*Need to check this in case there is DRX enter mode only*/
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      if( rfgsm_msg_ptr->init_writes_buff_ptr == NULL)
      {
        RFGC_T_MSG( MSG_LEGACY_ERROR, 
           "RF GSM Enter Mode dispatcher failed L1 payload variable verification");
       dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_2( MSG_LEGACY_HIGH, 
                   "RF GSM Cmd Proc Enter Mode, drx dev_id=%d, cnf=%d",
                   rfgsm_msg_ptr->drx_device,
                   rfgsm_msg_ptr->cnf_required );

      /* If the last mode was not GSM ensure to exit that mode first */
      if(rfgsm_mode[rfgsm_msg_ptr->drx_device] != RFCOM_GSM_MODE)
      {
        (void)rfm_exit_mode(rfgsm_msg_ptr->drx_device, RFCOM_GSM_MODE);
      }

      /* Set the core data */
      rfgsm_core_data.enable_rx_diversity = TRUE;
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->drx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr_drx;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->drx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle
                       RFA_RF_GSM_RX_ENTER_MODE_REQUEST_START, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       0,                         //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       0,                         //duration us
                       start_pcycle,              //start time pcycles
                       0 );                       //duration pcycles

      /* Enter GSM mode */
      if( rfgsm_mc_rx_enter_mode( &rfgsm_core_data,
                                  rfgsm_msg_ptr->rxlm_buf_index.drx_idx,
                                  (void * const)rfgsm_msg_ptr->init_writes_buff_ptr ) == RFGSM_MC_SUCCESS )
      {
        rfgsm_mode[rfgsm_msg_ptr->drx_device] = RFCOM_GSM_MODE;
        cmd_result = RFA_RF_GSM_SUCCESS;
        drx_device_cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR, "Failed rfgsm_mc_enter_mode to mode %d", RFCOM_GSM_MODE);
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {      
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Enter Mode dispatcher failed variable verification for DRX device %d", rfgsm_msg_ptr->drx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->drx_device );
    }
  }
  
  if (rfgsm_msg_ptr->enter_mode == RFGSM_PRX_DRX_ENTER_MODE) 
  {
    /*If their is combined mode then device driver scripts those were obtained 
    using prx and drx rfm_dev needs to be programmed to same CCS Event*/
    rfgsm_core_data.rfgsm_core_handle_ptr= rfgsm_core_handle_ptr;
    rfgsm_core_data.rxd_rfm_dev =  rfgsm_msg_ptr->drx_device;
    rfgsm_core_data.rfgsm_core_handle_ptr_rxd = rfgsm_core_handle_ptr_drx;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->prx_device;
    rfgsm_core_data.prx_drx_combined_mode = TRUE;
  }


  if (cmd_result==RFA_RF_GSM_SUCCESS) 
  {
    if (rfgsm_mc_build_enter_mode_ccs_events(&rfgsm_core_data,
                                            (void * const)rfgsm_msg_ptr->init_writes_buff_ptr)== RFGSM_MC_SUCCESS)
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }
    else
    {
       cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
       prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
       drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
    }
  }

  if(rfgsm_msg_ptr->enter_mode == RFGSM_PRX_ENTER_MODE ||
     rfgsm_msg_ptr->enter_mode == RFGSM_PRX_DRX_ENTER_MODE ||
     rfgsm_msg_ptr->enter_mode == RFGSM_PRX_WTR_DED_HOP_ENTER_MODE )
  {
    if(dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      /* RF execution time returned to GL1 */
      /* Take a snapshot of the pcycles buffer index before it is incremented */
      prof_buff = &rfgsm_core_handle_ptr->cmd_buffs.time_prof[RFA_RF_GSM_ENTER_MODE_REQUEST_STOP];
      prof_buff_idx = prof_buff->index;

      /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles to the PRX device log */
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr,     //core handle
                       RFA_RF_GSM_ENTER_MODE_REQUEST_STOP, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       rfgsm_msg_ptr->prx_device, //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                       start_pcycle,              //start time pcycles
                       qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
    }
  }

  if(rfgsm_msg_ptr->enter_mode == RFGSM_DRX_ENTER_MODE ||
     rfgsm_msg_ptr->enter_mode == RFGSM_PRX_DRX_ENTER_MODE )
  {
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      /* RF execution time returned to GL1 */
      /* Take a snapshot of the pcycles buffer index before it is incremented */
      prof_buff = &rfgsm_core_handle_ptr_drx->cmd_buffs.time_prof[RFA_RF_GSM_RX_ENTER_MODE_REQUEST_STOP];
      prof_buff_idx = prof_buff->index;

      /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles to the DRX device log */
      RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr_drx, //core handle
                        RFA_RF_GSM_RX_ENTER_MODE_REQUEST_STOP, //api 
                        rf_task,                   //rf task
                        start_time_qs,             //command proc start qs
                        rfgsm_msg_ptr->drx_device, //arg1
                        0,                         //arg2
                        geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                        start_ustmr,               //start time USTMR
                        rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                        start_pcycle,              //start time pcycles
                        qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.prx_device               =     rfgsm_msg_ptr->prx_device;
    rfgsm_resp_msg.drx_device               =     rfgsm_msg_ptr->drx_device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.rxlm_buf_index.prx_idx   =     rfgsm_msg_ptr->rxlm_buf_index.prx_idx;
    rfgsm_resp_msg.rxlm_buf_index.drx_idx   =     rfgsm_msg_ptr->rxlm_buf_index.drx_idx;
    rfgsm_resp_msg.init_writes_buff_ptr     =     rfgsm_msg_ptr->init_writes_buff_ptr;
    rfgsm_resp_msg.ip2_cal_supported        =     rfgsm_core_get_sawless_status(rfgsm_msg_ptr->prx_device);
    rfgsm_resp_msg.req_result               =     cmd_result;
    rfgsm_resp_msg.prx_device_req_result    =     prx_device_cmd_result;
    rfgsm_resp_msg.drx_device_req_result    =     drx_device_cmd_result;

    if(rfgsm_mc_retrieve_sawless_support(rfgsm_resp_msg.sawless_sup) == FALSE)
    {
      RFGC_T_MSG(MSG_LEGACY_ERROR, "Failed to retrieve SAWless status for logical devices during enter mode!");
    }

    /* Return actual and running average execution times to GL1 */
    if ( prof_buff != NULL ) {
       rfgsm_resp_msg.avg_exec_time            =   prof_buff->avg_case.pcycles;       /* Running average of pcycles for PRX path */
       rfgsm_resp_msg.exec_time                =   prof_buff->pcycles[prof_buff_idx]; /* Actual pcycle for this call for PRX path */
    }

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_enter_mode_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM RX Enter Mode MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_rx_enter_mode_req

===========================================================================*/
/*! @fn rfgsm_dispatch_rx_enter_mode_req( rfa_rf_gsm_rx_enter_mode_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific RX path for GSM. Intended to be used for diversity RX support or
 *  any other application that requires a standalone Rx path.
 *  @param rfgsm_msg_ptr a pointer to the enter_mode structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_rx_enter_mode_req( rfa_rf_gsm_enter_mode_req_s* rfgsm_msg_ptr, 
                                                           rf_task_num_type rf_task,
                                                           uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_enter_mode_cnf_s   rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /*core variable to indicate whether we received combined enter mode from GL1*/
  rfgsm_core_data.prx_drx_combined_mode = FALSE;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_rx_enter_mode_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->drx_device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->drx_device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->drx_device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    if( rfgsm_msg_ptr->init_writes_buff_ptr == NULL )
    {
      RFGC_T_MSG( MSG_LEGACY_ERROR, 
          "RF GSM Rx Enter Mode dispatcher failed L1 payload variable verification");
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    MSG_3( MSG_SSID_RF, MSG_LEGACY_HIGH, 
           "RF GSM Cmd Proc Rx Enter Mode, dev_id=%d, shared_mem_ptr=0x%x, cnf=%d",
           rfgsm_msg_ptr->drx_device, 
           rfgsm_msg_ptr->init_writes_buff_ptr,
           rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->drx_device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
    rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->drx_device);

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_RX_ENTER_MODE_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    /* Enter GSM mode */
    if( rfgsm_mc_rx_enter_mode( &rfgsm_core_data,
                                rfgsm_msg_ptr->rxlm_buf_index.drx_idx,
                                (void * const)rfgsm_msg_ptr->init_writes_buff_ptr ) == RFGSM_MC_SUCCESS )
    {
      rfgsm_mode[rfgsm_msg_ptr->drx_device] = RFCOM_GSM_MODE;
      cmd_result = RFA_RF_GSM_SUCCESS;
    }
    else
    {
      RFGC_T_MSG_2( MSG_LEGACY_ERROR, "Failed rfgsm_mc_rx_enter_mode to mode %d", RFCOM_GSM_MODE, cmd_result);
    }

    if (rfgsm_mc_build_enter_mode_ccs_events(&rfgsm_core_data,
											 (void * const)rfgsm_msg_ptr->init_writes_buff_ptr)== RFGSM_MC_SUCCESS)
    {
        cmd_result = RFA_RF_GSM_SUCCESS;
    }
    else
    {
      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      RFGC_T_MSG_2( MSG_LEGACY_ERROR, "Failed rfgsm_mc_rx_enter_mode to mode %d", RFCOM_GSM_MODE, cmd_result);
    }

    /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles */
    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_RX_ENTER_MODE_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {    
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Rx Enter Mode dispatcher failed variable verification for device %d", rfgsm_msg_ptr->drx_device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->drx_device );
  }

  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for functionality usage. */
    rfgsm_resp_msg.drx_device               =     rfgsm_msg_ptr->drx_device;
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.rxlm_buf_index.drx_idx   =     rfgsm_msg_ptr->rxlm_buf_index.drx_idx;
    rfgsm_resp_msg.init_writes_buff_ptr     =     rfgsm_msg_ptr->init_writes_buff_ptr;
    rfgsm_resp_msg.req_result               =     cmd_result;

    if(rfgsm_mc_retrieve_sawless_support(rfgsm_resp_msg.sawless_sup) == FALSE)
    {
      RFGC_T_MSG(MSG_LEGACY_ERROR, "Failed to retrieve SAWless status for logical devices during Rx enter mode!");
    }

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_enter_mode_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Enter Mode MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_tx_enable_req

===========================================================================*/
/*! @fn rfgsm_dispatch_tx_enable_req( rfa_rf_gsm_tx_enable_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the enabling of the transmitter for GSM
 *  mode.
 *  @param rfgsm_msg_ptr a pointer to the tx_enable structure variable 
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_tx_enable_req( rfa_rf_gsm_tx_enable_req_s* rfgsm_msg_ptr, 
                                                       rf_task_num_type rf_task,
                                                       uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t 	  	dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_tx_enable_cnf_s    rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_tx_enable_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) || !rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->device) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }

    if( rfgsm_msg_ptr->therm_read_buff_ptr == NULL )
    {
      RFGC_T_MSG( MSG_LEGACY_ERROR, 
          "RF GSM Tx enable Mode dispatcher failed L1 payload therm read variable verification");
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {   
    RFGC_T_MSG_5( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd Proc Tx Enable, dev_id=%d, txlm idx=%d, them_read_buff_ptr=0x%x, cnf=%d, edge_core=%d",
                  rfgsm_msg_ptr->device, 
                  rfgsm_msg_ptr->txlm_buf_idx,
                  rfgsm_msg_ptr->therm_read_buff_ptr,
                  rfgsm_msg_ptr->cnf_required,
                  rfgsm_msg_ptr->edge_core);

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
    rfgsm_core_handle_ptr->edge_core = rfgsm_msg_ptr->edge_core;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_TX_ENABLE_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if (rfgsm_core_is_device_prx(rfgsm_core_data.rfm_dev))
    {
      /* Update the therm read script and store the UMIDs for this device */
      if ( rfgsm_mc_prepare_therm_read( &rfgsm_core_data, 
                                            (void * const)rfgsm_msg_ptr->therm_read_buff_ptr ) == RFGSM_MC_SUCCESS )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
      }    
    }

    if( rfgsm_mc_enable_tx( &rfgsm_core_data,
                            rfgsm_msg_ptr->txlm_buf_idx ) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

#ifdef FEATURE_TX_SHARING
    if (cmd_result==RFA_RF_GSM_SUCCESS) 
    {
      if (rfgsm_mc_build_tx_enable_ccs_events(&rfgsm_core_data, rfgsm_core_data.rfm_dev)== RFGSM_MC_SUCCESS)
      {
	    cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }
    }
#endif

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_TX_ENABLE_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Tx enable dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr            =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device         =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required   =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.txlm_buf_idx   =     rfgsm_msg_ptr->txlm_buf_idx;
    rfgsm_resp_msg.therm_read_buff_ptr = rfgsm_msg_ptr->therm_read_buff_ptr;
    rfgsm_resp_msg.req_result     =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_tx_enable_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RFGSM Tx Enable MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_tx_disable_req

===========================================================================*/
/*! @fn rfgsm_dispatch_tx_disable_req( rfa_rf_gsm_tx_disable_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the disabling of the transmitter for GSM
 *  mode.
 *  @param rfgsm_msg_ptr a pointer to the tx_disable structure variable 
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_tx_disable_req( rfa_rf_gsm_tx_disable_req_s* rfgsm_msg_ptr, 
                                                        rf_task_num_type rf_task,
                                                        uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_tx_disable_cnf_s   rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_tx_disable_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) || !rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->device) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
  	RFGC_T_MSG_3( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd Proc Tx Disable, dev_id=%d, txlm idx=%d, cnf=%d",
                  rfgsm_msg_ptr->device, 
                  rfgsm_msg_ptr->txlm_buf_idx,
                  rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_TX_DISABLE_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_disable_tx(&rfgsm_core_data,
                            rfgsm_msg_ptr->txlm_buf_idx) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

#ifdef FEATURE_TX_SHARING
    if (cmd_result==RFA_RF_GSM_SUCCESS) 
    {
      if (rfgsm_mc_build_tx_disable_ccs_events(&rfgsm_core_data, rfgsm_core_data.rfm_dev)== RFGSM_MC_SUCCESS)
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }
    }
#endif	

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_TX_DISABLE_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {    
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Tx disable dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr            =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device         =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required   =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.txlm_buf_idx   =     rfgsm_msg_ptr->txlm_buf_idx;
    rfgsm_resp_msg.req_result     =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_tx_disable_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
       RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Tx Disable MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_wakeup_req

===========================================================================*/
/*! @fn rfgsm_dispatch_wakeup_req( rfa_rf_gsm_wakeup_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the waking up of the RF hardware from 
 *  sleep mode, setting the hardware to the same band it was in before sleep.
 *  @param rfgsm_msg_ptr a pointer to the wakeup structure variable payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_wakeup_req( rfa_rf_gsm_wakeup_req_s* rfgsm_msg_ptr, 
                                                    rf_task_num_type rf_task,
                                                    uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_wakeup_cnf_s       rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr_drx = NULL;
  rfgsm_core_data_type          rfgsm_core_data;
  rfgsm_prof_type               *prof_buff = NULL;
  uint8                         prof_buff_idx = 0;
  uint32                        ccs_exe_time_us = 0;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;
  unsigned long long stop_pcycle = 0;
  int32 duration_us = 0;

  rfgsm_core_data.enable_rx_diversity = FALSE;
  /*core variable to indicate whether we received combined Wake up mode from GL1*/
  rfgsm_core_data.prx_drx_combined_mode = FALSE;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_wakeup_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if (rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_WAKEUP_MODE 
      || rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_DRX_WAKEUP_MODE  ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->prx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->prx_device );

      if( rfgsm_core_handle_ptr == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                     "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->prx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      if( rfgsm_msg_ptr->sbi_init_writes_buff_ptr == NULL )
      {
        RFGC_T_MSG( MSG_LEGACY_ERROR, 
                   "RF GSM Enter Mode dispatcher failed L1 payload variable verification");
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    /* ----------------------------------------------------------------------- */
    /* Execution                                                               */
    /* Verify payload parameters. */
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {

      RFGC_T_MSG_4( MSG_LEGACY_HIGH, 
                   "RF GSM Cmd Proc Wakeup, dev_id=%d, rxlm idx=%d, shared_mem_ptr=0x%x, cnf=%d",
                    rfgsm_msg_ptr->prx_device, 
                    rfgsm_msg_ptr->rxlm_buf_index.prx_idx, 
                    rfgsm_msg_ptr->sbi_init_writes_buff_ptr,
                    rfgsm_msg_ptr->cnf_required );

      /* If the last mode was not GSM ensure to exit that mode first */
      if(rfgsm_mode[rfgsm_msg_ptr->prx_device] != RFCOM_GSM_MODE)
      {
        (void)rfm_exit_mode(rfgsm_msg_ptr->prx_device, RFCOM_GSM_MODE);
      }

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->prx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->prx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                       RFA_RF_GSM_WAKEUP_REQUEST_START,  //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       0,                     //duration us
                       start_pcycle,          //start time pcycles
                       0 );                   //duration pcycles

      /* Store the rxlm index in core handles for reference */
      rfgsm_core_handle_ptr->rxlm_buf_index = rfgsm_msg_ptr->rxlm_buf_index.prx_idx;

      /* Execute GSM wakeup */
      if( rfgsm_mc_wakeup( &rfgsm_core_data,
                           (const void *)rfgsm_msg_ptr->sbi_init_writes_buff_ptr ) == RFGSM_MC_SUCCESS )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
        prx_device_cmd_result = RFA_RF_GSM_SUCCESS;
        rfgsm_mode[rfgsm_msg_ptr->prx_device] = RFCOM_GSM_MODE;

        /* Get CCS execution time in microseconds */
        ccs_exe_time_us = rfgsm_mc_get_ccs_exe_us( rfgsm_msg_ptr->prx_device,
                                                   (rf_buffer_intf*)rfgsm_core_handle_ptr->scripts.enter_mode_script);
      } /* rfgsm_mc_wakeup */
      else 
      {
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Wakeup dispatcher failed variable verification for device %d", rfgsm_msg_ptr->prx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->prx_device );
    }
  }

  if (rfgsm_msg_ptr->wakeup_mode == RFGSM_DRX_WAKEUP_MODE 
      || rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_DRX_WAKEUP_MODE  ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->drx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr_drx = rfgsm_core_handle_get( rfgsm_msg_ptr->drx_device );

      if( rfgsm_core_handle_ptr_drx == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                      "rfgsm_core_handle_ptr is NULL for drx device %d!", rfgsm_msg_ptr->drx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      if( rfgsm_msg_ptr->sbi_init_writes_buff_ptr == NULL )
      {
          RFGC_T_MSG( MSG_LEGACY_ERROR, 
                      "RF GSM wakeup Mode dispatcher failed L1 payload variable verification");
          dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
    }

    /* ----------------------------------------------------------------------- */
    /* DRX Execution                                                            */
    /* Verify payload parameters. */
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_4( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Wakeup, dev_id=%d, rxlm idx=%d, shared_mem_ptr=0x%x, cnf=%d",
                    rfgsm_msg_ptr->drx_device, 
                    rfgsm_msg_ptr->rxlm_buf_index.drx_idx, 
                    rfgsm_msg_ptr->sbi_init_writes_buff_ptr,
                    rfgsm_msg_ptr->cnf_required );

      /* If the last mode was not GSM ensure to exit that mode first */
      if(rfgsm_mode[rfgsm_msg_ptr->drx_device] != RFCOM_GSM_MODE)
      {
          (void)rfm_exit_mode(rfgsm_msg_ptr->drx_device, RFCOM_GSM_MODE);
      }

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->drx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr_drx;
      rfgsm_core_data.enable_rx_diversity = TRUE;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->drx_device);
      /* Store the rxlm index in core handles for reference */
      rfgsm_core_handle_ptr_drx->rxlm_buf_index = rfgsm_msg_ptr->rxlm_buf_index.drx_idx;

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle
                       RFA_RF_GSM_RX_WAKEUP_REQUEST_START, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       0,                         //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       0,                         //duration us
                       start_pcycle,              //start time pcycles
                       0 );                       //duration pcycles

      /* Execute GSM wakeup */
      if( rfgsm_mc_wakeup( &rfgsm_core_data,
                           (const void *)rfgsm_msg_ptr->sbi_init_writes_buff_ptr ) == RFGSM_MC_SUCCESS )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
        drx_device_cmd_result = RFA_RF_GSM_SUCCESS;
        rfgsm_mode[rfgsm_msg_ptr->drx_device] = RFCOM_GSM_MODE;
      }
      else
      {
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Wakeup dispatcher failed variable verification for device %d", rfgsm_msg_ptr->drx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->drx_device );
    }
  }

  /*Combined wakeup_mode*/
  if (rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_DRX_WAKEUP_MODE) 
  {
    /*If their is combined mode then device driver scripts those were obtained 
    using prx and drx rfm_dev needs to be programmed to same CCS Event*/
    rfgsm_core_data.rfgsm_core_handle_ptr= rfgsm_core_handle_ptr;
    rfgsm_core_data.rfgsm_core_handle_ptr_rxd = rfgsm_core_handle_ptr_drx;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->prx_device;
    rfgsm_core_data.rxd_rfm_dev = rfgsm_msg_ptr->drx_device;
    rfgsm_core_data.prx_drx_combined_mode = TRUE;
  }

  if (cmd_result==RFA_RF_GSM_SUCCESS) 
  {
    if (rfgsm_mc_build_wakeup_mode_ccs_events(&rfgsm_core_data,
                                              (void * const)rfgsm_msg_ptr->sbi_init_writes_buff_ptr)== RFGSM_MC_SUCCESS)
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
      prx_device_cmd_result = RFA_RF_GSM_SUCCESS;
      prx_device_cmd_result = RFA_RF_GSM_SUCCESS;
    }
    else
    {
      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
    }
  }

  if(rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_WAKEUP_MODE ||
     rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_DRX_WAKEUP_MODE )
  {
    if(dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      /* RF execution time returned to GL1 */
      /* Take a snapshot of the pcycles buffer index before it is incremented */
      prof_buff = &rfgsm_core_handle_ptr->cmd_buffs.time_prof[RFA_RF_GSM_WAKEUP_REQUEST_STOP];
      prof_buff_idx = prof_buff->index;
      stop_pcycle = qurt_get_core_pcycles();
      duration_us = rfgsm_ustmr_count_diff_us_rtn(start_ustmr);

      /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles to the PRX device log */
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr,     //core handle
                       RFA_RF_GSM_WAKEUP_REQUEST_STOP, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       rfgsm_msg_ptr->prx_device, //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       duration_us, //duration us
                       start_pcycle,              //start time pcycles
                       stop_pcycle-start_pcycle ); //duration pcycles
    }
  }

  if(rfgsm_msg_ptr->wakeup_mode == RFGSM_DRX_WAKEUP_MODE || 
     rfgsm_msg_ptr->wakeup_mode == RFGSM_PRX_DRX_WAKEUP_MODE )
  {
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      /* Take a snapshot of the pcycles buffer index before it is incremented */
      prof_buff = &rfgsm_core_handle_ptr_drx->cmd_buffs.time_prof[RFA_RF_GSM_RX_WAKEUP_REQUEST_STOP];
      prof_buff_idx = prof_buff->index;
      stop_pcycle = qurt_get_core_pcycles();
      duration_us = rfgsm_ustmr_count_diff_us_rtn(start_ustmr);
      /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles to the DRX device log */
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle
                       RFA_RF_GSM_RX_WAKEUP_REQUEST_STOP, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       rfgsm_msg_ptr->drx_device, //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       duration_us, //duration us
                       start_pcycle,              //start time pcycles
                       stop_pcycle-start_pcycle ); //duration pcycles
    }
  }

  RFGC_T_MSG_4( MSG_LEGACY_HIGH,
                "RF GSM Cmd Proc Wakeup start time USTMR 0x%08x, start time qs=%d, duration pcycles=%d, duration us=%d",
                start_ustmr, start_time_qs, stop_pcycle-start_pcycle, duration_us );


  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =   rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.prx_device               =   rfgsm_msg_ptr->prx_device;
    rfgsm_resp_msg.drx_device               =   rfgsm_msg_ptr->drx_device;
    rfgsm_resp_msg.cnf_required             =   rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.rxlm_buf_index.prx_idx   =   rfgsm_msg_ptr->rxlm_buf_index.prx_idx;
    rfgsm_resp_msg.rxlm_buf_index.drx_idx   =   rfgsm_msg_ptr->rxlm_buf_index.drx_idx;
    rfgsm_resp_msg.sbi_init_writes_buff_ptr =   rfgsm_msg_ptr->sbi_init_writes_buff_ptr;
    rfgsm_resp_msg.req_result               =   cmd_result;
    rfgsm_resp_msg.prx_device_req_result    =   prx_device_cmd_result;
    rfgsm_resp_msg.drx_device_req_result    =   drx_device_cmd_result;
    rfgsm_resp_msg.ccs_time_us              =   ccs_exe_time_us;

    /* Return actual and average execution times to GL1 */
    if ( prof_buff != NULL ) 
    {
       rfgsm_resp_msg.avg_exec_time         =   prof_buff->avg_case.pcycles;         /* Running average of pcycles */
       rfgsm_resp_msg.exec_time             =   prof_buff->pcycles[prof_buff_idx];   /* Actual pcycle for this call */
    }

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_wakeup_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RFGSM Wakeup MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_sleep_req

===========================================================================*/
/*! @fn rfgsm_dispatch_sleep_req( rfa_rf_gsm_sleep_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the placing of the RF hardware to sleep
 *  for GSM mode. 
 *  @param rfgsm_msg_ptr a pointer to the sleep structure variable payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_sleep_req( rfa_rf_gsm_sleep_req_s* rfgsm_msg_ptr, 
                                                   rf_task_num_type rf_task,
                                                   uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_sleep_cnf_s        rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr_drx = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;
  unsigned long long stop_pcycle = 0;
  int32 duration_us = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_sleep_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if (rfgsm_msg_ptr->sleep_mode == RFGSM_PRX_SLEEP_MODE 
      || rfgsm_msg_ptr->sleep_mode == RFGSM_PRX_DRX_SLEEP_MODE ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->prx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->prx_device );

      if( rfgsm_core_handle_ptr == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                      "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->prx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
      /* Device has been manipulated outside of GSM control exit with error */
      else if(rfgsm_mode[rfgsm_msg_ptr->prx_device] != RFCOM_GSM_MODE)
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                      "rfm_mode %d is not GSM cannot go to sleep!", rfgsm_mode[rfgsm_msg_ptr->prx_device]);
        dispatch_result = RFGSM_DISPATCH_STATE_MACHINE_ERR;
      }
    }

    /* ----------------------------------------------------------------------- */
    /* PRX SLEEP Execution                                                     */
    /* Verify payload parameters. */
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_3( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Sleep, dev_id=%d, rxlm idx=%d, cnf=%d",
                    rfgsm_msg_ptr->prx_device,
                    rfgsm_msg_ptr->rxlm_buf_idx,
                    rfgsm_msg_ptr->cnf_required );

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->prx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->prx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                       RFA_RF_GSM_SLEEP_REQUEST_START, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       0,                     //duration us
                       start_pcycle,          //start time pcycles
                       0 );                   //duration pcycles

      if( rfgsm_mc_sleep(&rfgsm_core_data) == RFGSM_MC_SUCCESS )
      {
        /*rfgsm_mc function successful*/
        cmd_result = RFA_RF_GSM_SUCCESS;
        prx_device_cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        cmd_result =   RFA_RF_GSM_EXECUTION_ERROR;
        prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        RFGC_T_MSG(MSG_LEGACY_ERROR, "RF GSM Sleep cmd failed for prx device");
      }
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Sleep dispatcher failed variable verification for device %d", rfgsm_msg_ptr->prx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->prx_device );
    }
  }/*PRX Sleep execution*/
  

  if( rfgsm_msg_ptr->sleep_mode == RFGSM_DRX_SLEEP_MODE ||
      rfgsm_msg_ptr->sleep_mode == RFGSM_PRX_DRX_SLEEP_MODE ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->drx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr_drx = rfgsm_core_handle_get( rfgsm_msg_ptr->drx_device );

      if( rfgsm_core_handle_ptr_drx == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                      "rfgsm_core_handle_ptr is NULL for drx device %d!", rfgsm_msg_ptr->drx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
      }
      /* Device has been manipulated outside of GSM control exit with error */
      else if(rfgsm_mode[rfgsm_msg_ptr->drx_device] != RFCOM_GSM_MODE)
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                      "rfm_mode %d is not GSM cannot go to sleep!", rfgsm_mode[rfgsm_msg_ptr->drx_device]);
        dispatch_result = RFGSM_DISPATCH_STATE_MACHINE_ERR;
      }
    }

    /* ----------------------------------------------------------------------- */
    /* Execution                                                               */
    /* Verify payload parameters. */
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_3( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Sleep, dev_id=%d, rxlm idx=%d, cnf=%d",
                    rfgsm_msg_ptr->drx_device,
                    rfgsm_msg_ptr->rxlm_buf_idx,
                    rfgsm_msg_ptr->cnf_required );

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->drx_device;
      rfgsm_core_data.rxd_rfm_dev = rfgsm_msg_ptr->drx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr_drx;
      rfgsm_core_data.rfgsm_core_handle_ptr_rxd = rfgsm_core_handle_ptr_drx;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->drx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle
                       RFA_RF_GSM_RX_SLEEP_REQUEST_START, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       0,                         //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       0,                         //duration us
                       start_pcycle,              //start time pcycles
                       0 );                       //duration pcycles

      if( rfgsm_mc_sleep(&rfgsm_core_data) == RFGSM_MC_SUCCESS )
      {
        /*rfgsm_mc function successful*/
          cmd_result = RFA_RF_GSM_SUCCESS;
          drx_device_cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        cmd_result =   RFA_RF_GSM_EXECUTION_ERROR;
        drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        RFGC_T_MSG(MSG_LEGACY_ERROR, "RF GSM Sleep cmd failed for drx device");
      }
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Sleep dispatcher failed variable verification for device %d", rfgsm_msg_ptr->drx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->drx_device );
    }

  }/*DRX Sleep execution*/

  if(rfgsm_msg_ptr->sleep_mode == RFGSM_PRX_SLEEP_MODE ||
     rfgsm_msg_ptr->sleep_mode == RFGSM_PRX_DRX_SLEEP_MODE )
  {
    if(dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      stop_pcycle = qurt_get_core_pcycles();
      duration_us = rfgsm_ustmr_count_diff_us_rtn(start_ustmr);
      /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles to the PRX device log */
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr,     //core handle
                       RFA_RF_GSM_SLEEP_REQUEST_STOP, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       rfgsm_msg_ptr->prx_device, //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       duration_us, //duration us
                       start_pcycle,              //start time pcycles
                       stop_pcycle-start_pcycle ); //duration pcycles
    }
  }

  if(rfgsm_msg_ptr->sleep_mode == RFGSM_DRX_SLEEP_MODE ||
     rfgsm_msg_ptr->sleep_mode == RFGSM_PRX_DRX_SLEEP_MODE )
  {
    if(dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      stop_pcycle = qurt_get_core_pcycles();
      duration_us = rfgsm_ustmr_count_diff_us_rtn(start_ustmr);

      /* Log the RFM_DEVICE, RXLM BUFFER IDX and execution time in pcycles to the PRX device log */
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle
                       RFA_RF_GSM_RX_SLEEP_REQUEST_STOP, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       rfgsm_msg_ptr->drx_device, //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       duration_us,//duration us
                       start_pcycle,              //start time pcycles
                       stop_pcycle-start_pcycle ); //duration pcycles
    }
  }

  RFGC_T_MSG_4( MSG_LEGACY_HIGH,
                "RF GSM Cmd Proc Sleep start time USTMR 0x%08x, start time qs=%d, duration pcycles=%d, duration us=%d",
                start_ustmr, start_time_qs, stop_pcycle-start_pcycle, duration_us );

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr            =   rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.prx_device     =   rfgsm_msg_ptr->prx_device;
    rfgsm_resp_msg.drx_device     =   rfgsm_msg_ptr->drx_device;
    rfgsm_resp_msg.cnf_required   =   rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result     =   cmd_result;
    rfgsm_resp_msg.prx_device_req_result = prx_device_cmd_result ;
    rfgsm_resp_msg.drx_device_req_result = drx_device_cmd_result ;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_sleep_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RFGSM Sleep MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_rx_burst_req

===========================================================================*/
/*! @fn rfgsm_dispatch_rx_burst_req( rfa_rf_gsm_rx_burst_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the Rx burst request, handling all
 *  the GSM Rx burst information, based on type of Rx burst, for a single
 *  frame.
 *  @param rfgsm_msg_ptr a pointer to the rx_burst structure variable payload
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_rx_burst_req( rfa_rf_gsm_rx_burst_req_s* rfgsm_msg_ptr, 
                                                      rf_task_num_type rf_task,
                                                      uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_rx_burst_cnf_s     rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_mc_status_type          result = RFGSM_MC_ERROR;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr_rxd = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_rx_burst_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    if( rfgsm_msg_ptr->rx_burst_params == NULL )
    {
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
    else if( (rfgsm_msg_ptr->rx_burst_params->num_rx_bursts <= 0) || (rfgsm_msg_ptr->rx_burst_params->num_rx_bursts > RFA_RF_GSM_MAX_RX_ACTIVITIES) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_PARAM;
    }
    if( dispatch_result != RFGSM_DISPATCH_SUCCESSFUL )
    {
        RFGC_T_MSG(MSG_LEGACY_ERROR, 
            "RF GSM Rx burst dispatcher failed L1 payload variable verification");
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    if( rfgsm_msg_ptr->enable_rx_diversity )
    {
      RFGC_T_MSG_6( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Rx Burst prx_dev=%d, drx_dev=%d, num bursts=%d, rfm_mode=%d, GL1 lower ccs prio=%d, cnf=%d",
                    rfgsm_msg_ptr->device,
                    rfgsm_msg_ptr->rx_diversity_device,
                    rfgsm_msg_ptr->rx_burst_params->num_rx_bursts,
                    rfgsm_mode[rfgsm_msg_ptr->rx_diversity_device],
                    rfgsm_msg_ptr->rx_burst_params->gl1_lower_rx_ccs_prio,
                    rfgsm_msg_ptr->cnf_required );
    }
    else
    {
      RFGC_T_MSG_5( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Rx Burst prx_dev=%d num bursts=%d, rfm_mode=%d, GL1 lower ccs prio=%d, cnf=%d", 
                    rfgsm_msg_ptr->device,
                    rfgsm_msg_ptr->rx_burst_params->num_rx_bursts,
                    rfgsm_mode[rfgsm_msg_ptr->device],
                    rfgsm_msg_ptr->rx_burst_params->gl1_lower_rx_ccs_prio,
                    rfgsm_msg_ptr->cnf_required );
    }

    if( rfgsm_msg_ptr->enable_rx_diversity == FALSE )
    {
      /* RXD is disabled for this burst */
      rfgsm_msg_ptr->rx_diversity_device = RFM_INVALID_DEVICE;
    }
    /* RXD is enabled, check the RXD device ID */
    else 
    {
      if( ( rfgsm_msg_ptr->device == RFM_DEVICE_0 && rfgsm_msg_ptr->rx_diversity_device != RFM_DEVICE_1 ) ||
          ( rfgsm_msg_ptr->device == RFM_DEVICE_2 && rfgsm_msg_ptr->rx_diversity_device != RFM_DEVICE_3 ) )
      {
        RFGC_T_MSG_2( MSG_LEGACY_ERROR, "Invalid RXD device for dev %d: %d", rfgsm_msg_ptr->device, rfgsm_msg_ptr->rx_diversity_device);
        rfgsm_msg_ptr->rx_diversity_device = RFM_INVALID_DEVICE;
        rfgsm_msg_ptr->enable_rx_diversity = FALSE;
      }

      if ( rfgsm_mode[rfgsm_msg_ptr->rx_diversity_device] != RFCOM_GSM_MODE ) 
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR, "dev %d is not in GSM mode", rfgsm_msg_ptr->rx_diversity_device);
        rfgsm_msg_ptr->rx_diversity_device = RFM_INVALID_DEVICE;
        rfgsm_msg_ptr->enable_rx_diversity = FALSE;
      }
      if( rfgsm_msg_ptr->enable_rx_diversity )
      {
        /*Get the rfgsm core handle ptr for DRX*/
        rfgsm_core_handle_ptr_rxd = rfgsm_core_handle_get( rfgsm_msg_ptr->rx_diversity_device );
        RF_NULL_CHECK_RTN(rfgsm_core_handle_ptr_rxd, "rfgsm_core_handle_ptr is NULL for DRX device", FALSE);
      }
    }

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = rfgsm_msg_ptr->buffer_id;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
    /*To indicate rxd is enabled or not*/
    rfgsm_core_data.enable_rx_diversity = rfgsm_msg_ptr->enable_rx_diversity;
    rfgsm_core_data.rfgsm_core_handle_ptr_rxd = rfgsm_core_handle_ptr_rxd;
    rfgsm_core_data.rxd_rfm_dev = rfgsm_msg_ptr->rx_diversity_device;
    rfgsm_core_data.gl1_lower_rx_ccs_prio = rfgsm_msg_ptr->rx_burst_params->gl1_lower_rx_ccs_prio;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_RX_BURST_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     rfgsm_mode[rfgsm_msg_ptr->device],                     // mode on prx device
                     rfgsm_mode[rfgsm_msg_ptr->rx_diversity_device],        // mode on drx device
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_msg_ptr->enable_rx_diversity )
    {
      RFGSM_DEBUG_EN_Q( rfgsm_core_handle_ptr_rxd, //core handle
                        RFA_RF_GSM_DRX_BURST_REQUEST_START, //api
                        rf_task,                   //rf task
                        start_time_qs,             //command proc start qs
                        rfgsm_mode[rfgsm_msg_ptr->device],                   // mode on prx device
                        rfgsm_mode[rfgsm_msg_ptr->rx_diversity_device],      // mode on drx device
                        geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                        start_ustmr,               //start time USTMR
                        0,                         //duration us
                        start_pcycle,              //start time pcycles
                        0 );                       //duration pcycles
    }

    result = rfgsm_mc_process_rx_burst(&rfgsm_core_data,
                                       rfgsm_msg_ptr->rx_burst_params->num_rx_bursts,
                                       rfgsm_msg_ptr->rx_burst_params->rx_chan_params,
                                       rfgsm_msg_ptr->rx_burst_params->rx_diversity_chan_params,
                                       &rfgsm_msg_ptr->rx_burst_params->gfw_rf_burst_event[0]);
    if( result == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }
    else
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "PRx prepare bursts failed with result %d", result);
    }
    
    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr,             //core handle
                     RFA_RF_GSM_RX_BURST_REQUEST_STOP,  //api
                     rf_task,                           //rf task
                     start_time_qs,                     //command proc start qs
                     rfgsm_core_handle_ptr->rfgsm_mode, //arg1
                     ( (rfgsm_msg_ptr->rx_burst_params->num_rx_bursts & 0xF) | 
                       (rfgsm_msg_ptr->rx_diversity_device & 0xF) << 4 | 
                       (rfgsm_msg_ptr->enable_rx_diversity & 0xF) << 8 ), //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,                       //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,                      //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle); //duration pcycles

    if( rfgsm_msg_ptr->enable_rx_diversity )
    {
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_rxd,         //core handle
                       RFA_RF_GSM_DRX_BURST_REQUEST_STOP, //api
                       rf_task,                           //rf task
                       start_time_qs,                     //command proc start qs
                       rfgsm_core_handle_ptr->rfgsm_mode, //arg1
                       ( (rfgsm_msg_ptr->rx_burst_params->num_rx_bursts & 0xF) | 
                       (rfgsm_msg_ptr->rx_diversity_device & 0xF) << 4 | 
                       (rfgsm_msg_ptr->enable_rx_diversity & 0xF) << 8 ), //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,                       //start time USTMR
                       rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                       start_pcycle,                      //start time pcycles
                       qurt_get_core_pcycles()-start_pcycle); //duration pcycles
    }
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {    
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Rx burst dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr              =   rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device           =   rfgsm_msg_ptr->device;
    rfgsm_resp_msg.rx_diversity_device = rfgsm_msg_ptr->rx_diversity_device;
    rfgsm_resp_msg.enable_rx_diversity = rfgsm_msg_ptr->enable_rx_diversity;
    rfgsm_resp_msg.cnf_required     =   rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.rx_burst_params  =   rfgsm_msg_ptr->rx_burst_params;
    rfgsm_resp_msg.req_result       =   cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_rx_burst_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RFGSM Rx Burst MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_set_tx_band_req

===========================================================================*/
/*! @fn rfgsm_dispatch_set_tx_band_req( rfa_rf_gsm_set_tx_band_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the initialisation of the hardware for 
 *  the specified Tx band. 
 *  @param rfgsm_msg_ptr a pointer to the set_tx_band structure variable 
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_set_tx_band_req( rfa_rf_gsm_set_tx_band_req_s* rfgsm_msg_ptr, 
                                                         rf_task_num_type rf_task,
                                                         uint32 umid_cnf)
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_set_tx_band_cnf_s  rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfgsm_dispatch_set_tx_band_req is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) || !rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->device) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    if( rfgsm_msg_ptr->sbi_set_tx_band_buff_ptr == NULL )
    {
      RFGC_T_MSG(MSG_LEGACY_ERROR, 
          "RF GSM Enter Mode dispatcher failed L1 payload variable verification");
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG_4( MSG_LEGACY_HIGH,
                  "RF GSM Cmd Proc Tx Band, dev_id=%d, tx band=%d, shared_mem_ptr=0x%x, cnf=%d",
                  rfgsm_msg_ptr->device, 
                  rfgsm_msg_ptr->band, 
                  rfgsm_msg_ptr->sbi_set_tx_band_buff_ptr,
                  rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_SET_TX_BAND_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_set_tx_band(&rfgsm_core_data, 
                             rfgsm_msg_ptr->band, 
                             rfgsm_msg_ptr->sbi_set_tx_band_buff_ptr) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_SET_TX_BAND_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {    
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Set Tx band dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }


  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                        =   rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                     =   rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required               =   rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.band                       =   rfgsm_msg_ptr->band;
    rfgsm_resp_msg.sbi_set_tx_band_buff_ptr   =   rfgsm_msg_ptr->sbi_set_tx_band_buff_ptr;
    rfgsm_resp_msg.req_result                 =   cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_set_tx_band_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RFGSM Set Tx Band MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_tx_burst_req

===========================================================================*/
/*! @fn rfgsm_dispatch_tx_burst_req( rfa_rf_gsm_tx_burst_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the Tx burst request, handling all
 *  the GSM Tx burst information for a single frame.
 *  @param rfgsm_msg_ptr a pointer to the tx_burst structure variable payload
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_tx_burst_req( rfa_rf_gsm_tx_burst_req_s* rfgsm_msg_ptr, 
                                                      rf_task_num_type rf_task,
                                                      uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_tx_burst_cnf_s     rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */  
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_tx_burst_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) || !rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->device) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    if( rfgsm_msg_ptr->tx_burst_params == NULL )
    {
      RFGC_T_MSG(MSG_LEGACY_ERROR, 
          "RF GSM Enter Mode dispatcher failed L1 payload variable verification");
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
    else if( rfgsm_msg_ptr->tx_burst_params->num_tx_slots > RFGSM_MAX_TX_SLOTS_PER_FRAME )
    {
      RFGC_T_MSG_2(MSG_LEGACY_ERROR,"num_tx_slots %d too large. Max Tx slots is %d", 
            rfgsm_msg_ptr->tx_burst_params->num_tx_slots, RFGSM_MAX_TX_SLOTS_PER_FRAME);
      dispatch_result = RFGSM_DISPATCH_INVALID_PARAM;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG_7( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd Proc Tx Burst, dev_id=%d, num slots=%d, band=%d, channel=%d, freq err=%d, shared_mem_ptr=0x%x, cnf=%d",
                  rfgsm_msg_ptr->device, 
                  rfgsm_msg_ptr->tx_burst_params->num_tx_slots,
                  rfgsm_msg_ptr->tx_burst_params->tx_chan_params.band, 
                  rfgsm_msg_ptr->tx_burst_params->tx_chan_params.arfcn,
                  rfgsm_msg_ptr->tx_burst_params->tx_chan_params.freq_err,
                  rfgsm_msg_ptr->tx_burst_params->tx_burst_event_ptr,
                  rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = rfgsm_msg_ptr->buffer_id;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_TX_BURST_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_process_tx_burst( &rfgsm_core_data,
                                   rfgsm_msg_ptr->tx_burst_params ) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_TX_BURST_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Tx Burst dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */   
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }


  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr               =   rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device            =   rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required      =   rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.tx_burst_params   =   rfgsm_msg_ptr->tx_burst_params;
    rfgsm_resp_msg.req_result        =   cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_tx_burst_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Tx Burst MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_idle_frame_processing_req

===========================================================================*/
/*! @fn rfgsm_dispatch_idle_frame_processing_req( rfa_rf_gsm_idle_frame_processing_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the triggering of the idle frame 
 *  functionality i.e therm read.
 *  @param rfgsm_msg_ptr a pointer to the idle_frame_processing structure 
 *  variable payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_idle_frame_processing_req( rfa_rf_gsm_idle_frame_processing_req_s* rfgsm_msg_ptr, 
                                                                   rf_task_num_type rf_task,
                                                                   uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_idle_frame_processing_cnf_s rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_idle_frame_processing_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG_2( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd Proc Idle Frame Proc, dev_id=%d, cnf=%d", 
                  rfgsm_msg_ptr->device,
                  rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_do_idle_frame_processing(&rfgsm_core_data) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle );   //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Idle frame proc dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr          =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device       =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result   =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_idle_frame_processing_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Idle Frame Processing MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_exit_mode_req

===========================================================================*/
/*! @fn rfgsm_dispatch_exit_mode_req( rfa_rf_gsm_exit_mode_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for GSM.
 *  @param rfgsm_msg_ptr a pointer to the exit_mode structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_exit_mode_req( rfa_rf_gsm_exit_mode_req_s* rfgsm_msg_ptr, 
                                                       rf_task_num_type rf_task,
                                                       uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cmd_outcome_t      drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_exit_mode_cnf_s    rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr_drx = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_exit_mode_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if (rfgsm_msg_ptr->exit_mode == RFGSM_PRX_EXIT_MODE ||
      rfgsm_msg_ptr->exit_mode == RFGSM_PRX_DRX_EXIT_MODE ) 
  {
  
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->prx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->prx_device );

      if( rfgsm_core_handle_ptr == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                     "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->prx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;      
      }
    }

    /* NOTE:                                                                   */
    /* RF sleep command is followed by RF exit during deactivation             */
    /* Therefore during exit mode the rfm_mode could be any of the following:  */
    /* o  RFCOM_GSM_MODE - active on device or after sleep during deactivation */
    /* o  RFCOM_<TECH>_MODE - other tech has come between GSM sleep and exit   */
    /*                        during deactivation                              */
    /* o  RFCOM_PARKED_MODE - other tech has come between and exited during GSM*/
    /*                        sleep and exit during deactivation               */
    /* Therefore all rfm_modes are valid when GL1 call exit mode               */

    /* ----------------------------------------------------------------------- */
    /* Execution for PRX device                                                 */
    /* Verify payload parameters. */
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_2( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Exit Mode, dev_id=%d, cnf=%d", 
                    rfgsm_msg_ptr->prx_device,
                    rfgsm_msg_ptr->cnf_required );

      /* No longer in GSM standalone mode */
      /* Important to ensure therm read not executed during iRAT */
      rfgsm_core_handle_ptr->rfgsm_mode = RFGSM_DEFAULT_MODE;

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->prx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->prx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_EXIT_MODE_REQUEST_START, //api
                     rf_task,                 //rf task
                     start_time_qs,           //command proc start qs
                     0,                       //arg1
                     0,                       //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,             //start time USTMR
                     0,                       //duration us
                     start_pcycle,            //start time pcycles
                     0 );                     //duration pcycles

      if( rfgsm_mc_exit_mode(&rfgsm_core_data) == RFGSM_MC_SUCCESS )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
        prx_device_cmd_result = RFA_RF_GSM_SUCCESS;
        
        /*-------------------------------------------------------------------*/
        /* NOT placing the rfm_mode to RFCOM_PARKED_MODE                     */
        /* RFM mode cannot be altered at this point because another tech     */
        /* could have TRM lock on this same device.                          */
        /* Deactivation of a subscription will trigger exit mode but this    */
        /* could come some time following a sleep from which time another    */
        /* tech could have required this device, therefore GSM should not    */
        /* alter the device mode to not corrupt the state.                   */
      }
      else
      {
        RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RF GSM Exit Mode cmd failed for dev : %d", rfgsm_core_data.rfm_dev );
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        prx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
      }

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                       RFA_RF_GSM_EXIT_MODE_REQUEST_STOP, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                       start_pcycle,          //start time pcycles
                       qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Exit Mode dispatcher failed variable verification for PRX device %d", rfgsm_msg_ptr->prx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->prx_device );
    }
  }

  /*Validity check for DRX device*/
 
  if (rfgsm_msg_ptr->exit_mode == RFGSM_DRX_EXIT_MODE ||
     rfgsm_msg_ptr->exit_mode == RFGSM_PRX_DRX_EXIT_MODE ) 
  {
    if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->drx_device, rfgsm_msg_ptr->hdr.id ) )
    {
      dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
      rfgsm_core_data.rxd_rfm_dev = RFM_INVALID_DEVICE;
    }
    else
    {
      rfgsm_core_handle_ptr_drx = rfgsm_core_handle_get( rfgsm_msg_ptr->drx_device );

      if( rfgsm_core_handle_ptr_drx == NULL )
      {
        RFGC_T_MSG_1( MSG_LEGACY_ERROR,
                   "rfgsm_core_handle_ptr DRX is NULL for device %d!", rfgsm_msg_ptr->drx_device);
        dispatch_result = RFGSM_DISPATCH_NULL_PTR;
        rfgsm_core_data.rxd_rfm_dev = RFM_INVALID_DEVICE;
      }
      rfgsm_core_data.rxd_rfm_dev =  rfgsm_msg_ptr->drx_device;
    }

    /* ----------------------------------------------------------------------- */
    /* Execution for DRX device                                                   */
    /* Verify payload parameters. */
    if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Exit Mode, dev_id=%d", 
                    rfgsm_msg_ptr->drx_device);

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->drx_device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr_drx;
      rfgsm_core_data.device_is_tx = rfgsm_core_get_device_tx_capability(rfgsm_msg_ptr->drx_device);

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle
                       RFA_RF_GSM_RX_EXIT_MODE_REQUEST_START, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       0,                         //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       0,                         //duration us
                       start_pcycle,              //start time pcycles
                       0 );                       //duration pcycles

      if( rfgsm_mc_exit_mode(&rfgsm_core_data) == RFGSM_MC_SUCCESS )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
        drx_device_cmd_result = RFA_RF_GSM_SUCCESS;
        #ifndef FEATURE_JOLOKIA_MODEM
        rfgsm_mode[rfgsm_msg_ptr->drx_device] = RFCOM_PARKED_MODE;
        #endif
      }
      else
      {
        cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        drx_device_cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
        RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RF GSM Exit Mode cmd failed for dev : %d", rfgsm_core_data.rfm_dev );
      }

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr_drx, //core handle 
                       RFA_RF_GSM_RX_EXIT_MODE_REQUEST_STOP, //api
                       rf_task,                   //rf task
                       start_time_qs,             //command proc start qs
                       0,                         //arg1
                       0,                         //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,               //start time USTMR
                       rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                       start_pcycle,              //start time pcycles
                       qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
    }
    else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Exit Mode dispatcher failed variable verification for RDX device %d", rfgsm_msg_ptr->drx_device);

      /* Handle dispatch errors. */
      rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->drx_device );
    }

  }
  
  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.prx_device               =     rfgsm_msg_ptr->prx_device;
    rfgsm_resp_msg.drx_device               =     rfgsm_msg_ptr->drx_device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result               =     cmd_result;
    rfgsm_resp_msg.prx_device_req_result    =     prx_device_cmd_result;
    rfgsm_resp_msg.prx_device_req_result    =     drx_device_cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_exit_mode_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Exit Mode MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_therm_read_rsp

===========================================================================*/
/*! @fn rfgsm_dispatch_therm_read_req( rfgsm_rflm_msg_u* rfgsm_msg_ptr )
 *  @brief This function dispatches the processing of the thermistor read data
 *  @param rfgsm_rflm_msg_u a pointer to the therm_read variable payload.
 *  @param rf_task_enum_type RF task number which the message is received on
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_therm_read_rsp( rfgsm_rflm_msg_u* rfgsm_msg_ptr,
                                                        rf_task_num_type rf_task )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      rsp_cnf = RFGSM_DISPATCH_UNSUCCESSFUL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfcom_device_enum_type        device = RFM_INVALID_DEVICE;
  uint8                         event_handle_tag = 0;
  uint8                         therm_read_ok;
  rfgsm_core_data_type          rfgsm_core_data;
  rflm_gsm_resp_status          status;
  
  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();

  /* Verify payload ptr is not NULL */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfgsm_msg_ptr is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Process payload */
  device =(rfcom_device_enum_type)((rfgsm_msg_ptr->rflm_gsm_therm_read_rsp.payload>>16) & 0xFFFF );
  event_handle_tag = (uint8)( rfgsm_msg_ptr->rflm_gsm_therm_read_rsp.payload & 0xFF );
  status = rfgsm_msg_ptr->rflm_gsm_therm_read_rsp.status;

  RFGC_T_MSG_3( MSG_LEGACY_HIGH, 
                "RF GSM RSP Proc therm read, dev_id=%d, therm event_handle tag %d, status %d",
                                      device, 
                                      event_handle_tag,
                                      status);

  /* At this point, status should ONLY be one of the following:     
     RFLM_GSM_STATUS_OK     (successful)
     RFLM_GSM_STATUS_ERROR  (no appropriate status set by GFW - this should never happen!)
     RFLM_GSM_STATUS_THERM_READ_ABORTED (cancellation e.g. when GFW is going to sleep)
     RFLM_GSM_STATUS_THERM_READ_TIMEOUT (GFW was not able to schedule therm read within 4 frames)
  */
  /* @todo Replace ASSERT with appropriate error handling once code is mature */
  ASSERT( (status != RFLM_GSM_STATUS_DEFAULT) || (status != RFLM_GSM_STATUS_MAX) || (status != RFLM_GSM_STATUS_ERROR) );

  /* Therm read OK => process therm read response and do the house-keeping */
  /* Therm read NOK => only do the house-keeping */
  therm_read_ok = (status == RFLM_GSM_STATUS_OK) ? TRUE : FALSE;

  /* Get the handle*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(device);
  
  if (rfgsm_core_handle_ptr == NULL)
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    rsp_cnf = RFGSM_DISPATCH_NULL_PTR;
    return rsp_cnf;
  }

  rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;

  /* Set the core data */
  rfgsm_core_data.rf_task = rf_task;
  rfgsm_core_data.rfm_dev = device;

  // task to umid mapping
  switch( rf_task )
  {
  case RF_TASK_2:                     
    rfgsm_core_data.sub_id = RFLM_GSM_SUBSCRIPTION_2;
    break;
  case RF_TASK_1:
  case RF_TASK_3:
  default:
  rfgsm_core_data.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION;
    break;
  }
  
  rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_core_data.sub_id);
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  if( rfgsm_mc_process_therm_read_rsp(&rfgsm_core_data, 
                                      event_handle_tag, 
                                      therm_read_ok) == RFGSM_MC_SUCCESS )
  {
    rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;
  }

  RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                   RFA_RF_GSM_THERM_READ_RESPONSE, //api
                   rf_task,               //rf task
                   0,                     //command proc start qs
                   0,                     //arg1
                   0,                     //arg2
                   geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                   start_ustmr,           //start time USTMR
                   rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                   start_pcycle,          //start time pcycles
                   qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return(rsp_cnf);
}

/*===========================================================================

  FUNCTION:  rfgsm_dispatch_device_status_read_rsp

===========================================================================*/
/*! @fn rfgsm_dispatch_device_status_read_rsp( rfgsm_rflm_msg_u* rfgsm_msg_ptr )
 *  @brief This function dispatches the processing of the Device Status read data
 *  @param rfgsm_rflm_msg_u a pointer to the de variable payload.
 *  @param rf_task_enum_type RF task number which the message is received on
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_device_status_read_rsp( rfgsm_rflm_msg_u* rfgsm_msg_ptr,
                                                        rf_task_num_type rf_task )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      rsp_cnf = RFGSM_DISPATCH_UNSUCCESSFUL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  void                          *event_handle;
  rfcom_device_enum_type        device = RFM_INVALID_DEVICE;
  uint32                        buffer_id;
  uint8                         sub_id;
  rf_burst_type                 burst_type;
  rfgsm_core_dm_script_type 	script_type;
  uint8                         event_tag;
  uint8                         burst_num;
    
  /* Verify payload ptr is not NULL */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfgsm_msg_ptr is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Process payload */
  device =(rfcom_device_enum_type)((rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload>>28) & 0x0F );
  script_type = (rfgsm_core_dm_script_type)((rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload>>24) & 0x0F );
  event_tag = (uint8)((rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload>>16) & 0xFF );
  buffer_id = (uint8)((rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload>>12) & 0x0F );
  sub_id = (uint8)((rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload>>8) & 0x0F );
  burst_num = (uint8)((rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload>>4) & 0x0F );
  burst_type = (uint8)(rfgsm_msg_ptr->rflm_gsm_device_status_read_rsp.payload & 0x0F );

  if( device >= RFM_MAX_WAN_DEVICES )
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,"Device id is wrong. Its %d", device);
    rsp_cnf = RFGSM_DISPATCH_INVALID_PARAM;
	return rsp_cnf;
  }

  /* Check the burst buffer ID is valid */
  if((buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED))
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RF GSM core DM burst_buffer_id %d invalid", buffer_id);
    rsp_cnf = RFGSM_DISPATCH_INVALID_PARAM;
	return rsp_cnf;
  }
  
  /* Get the handle*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(device);
  
  if (rfgsm_core_handle_ptr == NULL)
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL! for device %d", device);
    rsp_cnf = RFGSM_DISPATCH_NULL_PTR;
    return rsp_cnf;
  }
  
  switch ( script_type )
  {
    case RFLM_GSM_TX_STOP_SCRIPT:
      if(rfgsm_mc_process_device_status_read_rsp(sub_id, buffer_id, burst_type,0, event_tag, burst_num) == RFGSM_MC_SUCCESS)
      {
        rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;
      }
      break;

    case RFLM_GSM_MON_STOP_SCRIPT:  
    case RFLM_GSM_RX_STOP_SCRIPT:
      if(rfgsm_mc_process_device_status_read_rsp(sub_id, buffer_id, burst_type,RFLM_GSM_SYNC_RX_EVENT, event_tag, burst_num) == RFGSM_MC_SUCCESS)
      {
        rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;
      } 
      break;

    case RFLM_GSM_RX_ACQ_STOP_SCRIPT:
      if(rfgsm_mc_process_device_status_read_rsp(sub_id, buffer_id, burst_type,RFLM_GSM_SCH_OR_ACQ_RX_EVENT, event_tag, burst_num) == RFGSM_MC_SUCCESS)
      {
        rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;
      } 
      break;

    case RFLM_GSM_RX_PCH_STOP_SCRIPT:
      if(rfgsm_mc_process_device_status_read_rsp(sub_id, buffer_id, burst_type,RFLM_GSM_PCH_RX_EVENT, event_tag, burst_num) == RFGSM_MC_SUCCESS)
      {
        rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;
      } 
      break;

    default:
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid Script Type for Device Status Read. Its %d ", script_type);
	  rsp_cnf = RFGSM_DISPATCH_INVALID_PARAM;
      break;
  }
  
  return(rsp_cnf);
}

#ifdef FEATURE_RF_ASDIV
/*===========================================================================

  FUNCTION:  rfgsm_dispatch_asd_read_rsp

===========================================================================*/
/*! @fn rfgsm_dispatch_asd_read_req( rfgsm_rflm_msg_u* rfgsm_msg_ptr )
 *  @brief This function dispatches the processing of the ASD read data
 *  @param rfgsm_msg_ptr a pointer to the ASD_read structure variable
 *  payload.
 *  @param rf_task_enum_type RF task number which the message is received on
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_asd_read_rsp( rfgsm_rflm_msg_u* rfgsm_msg_ptr,rf_task_num_type rf_task  )
{
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      rsp_cnf = RFGSM_DISPATCH_UNSUCCESSFUL;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfcom_device_enum_type        device = RFM_INVALID_DEVICE ;
  uint8                         ant_postion;
  rfgsm_core_data_type          rfgsm_core_data;
  rfgsm_core_asd_switch_event_info_type *asd_info;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* Verify payload ptr is not NULL */
  if( rfgsm_msg_ptr == NULL )
  {
    RFGC_T_MSG(MSG_LEGACY_ERROR, "RF GSM RSP Proc ASD Read: Pointer argument into rfgsm_msg_ptr is NULL");
    rsp_cnf = RFGSM_DISPATCH_NULL_PTR;
    return rsp_cnf;
  }

  /* Process the payload */
  asd_info = (rfgsm_core_asd_switch_event_info_type *) rfgsm_msg_ptr->rflm_gsm_asd_rsp.payload;
  if( asd_info == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM RSP Proc ASD Read: payload data is NULL");
    rsp_cnf = RFGSM_DISPATCH_NULL_PTR;
    return rsp_cnf;
  }
  device = asd_info->rfm_dev;
  ant_postion = asd_info->set_antenna_position;

  MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM RSP Proc ASD Read Response, "
                                      "dev_id=%d, umid=0x%x, ant_position=%d",
                                      device, rfgsm_msg_ptr->hdr.id, ant_postion);

  /* Get the handle*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(device);
  
  if (rfgsm_core_handle_ptr == NULL)
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
    return rsp_cnf;
  }

   /*asdiv response error case timer clear */
  rfgsm_mc_asd_rsp_check_stop(device);
  
  /* Set the core data */
  rfgsm_core_data.rf_task = rf_task;
  rfgsm_core_data.rfm_dev = device;
  rfgsm_core_data.sub_id = asd_info->sub_id;
  rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(asd_info->sub_id);
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                   RFA_RF_GSM_ASD_READ_RESPONSE_START, //api
                   rf_task,               //rf task
                   start_time_qs,         //command proc start qs
                   0,                     //arg1
                   0,                     //arg2
                   geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                   start_ustmr,           //start time USTMR
                   0,                     //duration us
                   start_pcycle,          //start time pcycles
                   0 );                   //duration pcycles

  /* Process the response from RFLM. RFLM only sends repsonse back when the RX start  
    event (and antenna switch) has been processed by CCS */
  if( rfgsm_mc_process_asd_read_rsp(&rfgsm_core_data, ant_postion) == RFGSM_MC_SUCCESS )
  {
    rsp_cnf = RFGSM_DISPATCH_SUCCESSFUL;
  }

  /* Antenna switch has occurred => call the GL1 callback function to confirm this */
  /* The callback function in turn informs TRM the switch has occurred */
  if (asd_info->l1_cb != NULL) 
  {
    rfa_rf_gsm_set_antenna_callback_s l1_cb_data;

    /* Populate the L1 callback data */
    l1_cb_data.req_result = (rfgsm_msg_ptr->rflm_gsm_asd_rsp.status==RFLM_GSM_STATUS_OK)?RFA_RF_GSM_SUCCESS:RFA_RF_GSM_EXECUTION_ERROR;
    l1_cb_data.ant_pos = ant_postion;
    l1_cb_data.device = device;
    l1_cb_data.sub_id = asd_info->sub_id;

    /* Call the L1 callback function */
    asd_info->l1_cb( &l1_cb_data );
  }

  RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                   RFA_RF_GSM_ASD_READ_RESPONSE_STOP, //api
                   rf_task,               //rf task
                   start_time_qs,         //command proc start qs
                   device,                //arg1
                   0,                     //arg2
                   geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                   start_ustmr,           //start time USTMR
                   rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                   start_pcycle,          //start time pcycles
                   qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return(rsp_cnf);
}
#endif
/*===========================================================================

  FUNCTION:  rfgsm_dispatch_update_temp_comp_req

===========================================================================*/
/*! @fn rfgsm_dispatch_update_temp_comp_req( rfa_rf_gsm_update_temp_comp_req_s* rfgsm_msg_ptr, uint32 umid_cnf )
 *  @brief This function dispatches the processing of temp comp update request
 *  @param rfgsm_msg_ptr a pointer to the temp comp update structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_update_temp_comp_req( rfa_rf_gsm_update_temp_comp_req_s* rfgsm_msg_ptr,
                                                              rf_task_num_type rf_task,
                                                              uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_update_temp_comp_cnf_s rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );
  
  if( rfgsm_core_handle_ptr == NULL )
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
    dispatch_result = RFGSM_DISPATCH_NULL_PTR;
	return dispatch_result;
  }

  /* Check if Tempcomp was stopped*/
  if(FALSE == rfgsm_core_handle_ptr->rfgsm_core_temp_comp_started)
  {
    RFGC_T_MSG_1( MSG_LEGACY_HIGH, "[dev %d] Tempcomp is stopped already, not updating temp_comp data any more ", rfgsm_msg_ptr->device);
    dispatch_result = RFGSM_DISPATCH_UNSUCCESSFUL;
    return dispatch_result;
  }

  rf_task_num_type rfgsm_rf_task = rfgsm_core_get_task_id_from_sub_id(rfgsm_msg_ptr->sub_id);

  if ( RF_TASK_1 == rfgsm_rf_task)
  {
    if (rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid != rfgsm_sub_to_therm_umid_map[RF_TASK_1].therm_read_umid)/*GERAN_FW_RFM_THERM_READ_CMD*/
    {
      RFGC_T_MSG_2( MSG_LEGACY_HIGH,"Not updating temp comp data because of mismatch between UMID [0x%x] and SUB_ID [%d] ", rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid, rfgsm_msg_ptr->sub_id);
      dispatch_result = RFGSM_DISPATCH_STATE_MACHINE_ERR;
    }
  }

  if ( RF_TASK_2 == rfgsm_rf_task)
  {
    if (rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid != rfgsm_sub_to_therm_umid_map[RF_TASK_2].therm_read_umid)/*GERAN_FW2_RFM_THERM_READ_CMD)*/
    {
      RFGC_T_MSG_2( MSG_LEGACY_HIGH,"Not updating temp comp data because of mismatch between UMID [0x%x] and SUB_ID [%d] ", rfgsm_core_handle_ptr->therm_read.fw_therm_read_umid, rfgsm_msg_ptr->sub_id);
      dispatch_result = RFGSM_DISPATCH_STATE_MACHINE_ERR;
    }
  }

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_update_temp_comp_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG_1( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd update temp comp flag Proc, dev_id=%d", 
                  rfgsm_msg_ptr->device );

    /* Triple buffer should be zero unless burst event */
    rfgsm_core_handle_ptr->buffer_id = 0;

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_TEMP_COMP_UPDATE_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_update_temp_comp_req(&rfgsm_core_data, 
                                      rfgsm_msg_ptr->temp_comp_scaled_value, 
                                      rfgsm_msg_ptr->temp_bin_16) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_TEMP_COMP_UPDATE_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     (qurt_get_core_pcycles()-start_pcycle) ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Update temp comp dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  rfgsm_msg_ptr->cnf_required = FALSE;
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                   =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result               =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_update_temp_comp_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM update Temp Comp cnf UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }
   
  return(dispatch_result);
}


#ifdef FEATURE_RF_ASDIV
/*===========================================================================

  FUNCTION:  rfgsm_dispatch_set_antenna_req

===========================================================================*/
/*! @fn rfgsm_dispatch_set_antenna_req(rfa_rf_gsm_set_antenna_req_s* rfgsm_msg_ptr, uint32 umid_cnf)
 *  @brief This function dispatches the processing of set antenna request
 *  @param rfgsm_msg_ptr a pointer to the set antenna structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_set_antenna_req
( 
  rfa_rf_gsm_set_antenna_req_s* rfgsm_msg_ptr,
  rf_task_num_type rf_task,
  uint32 umid_cnf
)
{
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_set_antenna_cnf_s  rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;
  rfcom_device_enum_type        device;
  msgr_hdr_struct_type          hdr;
  boolean                       cnf_required;
  uint8                         ant_pos;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  /* ----------------------------------------------------------------------- */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_set_antenna_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);
 
  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  device = rfgsm_msg_ptr->device;
  hdr = rfgsm_msg_ptr->hdr;
  cnf_required = rfgsm_msg_ptr->cnf_required;
  ant_pos = rfgsm_msg_ptr->ant_pos;

  RFGC_T_MSG_3(MSG_LEGACY_HIGH, "RF GSM Cmd Set Antenna Request, "
    "dev_id=%d, umid=0x%x ant_pos=%d", device, hdr.id, ant_pos);

  if( !rfgsm_dispatch_check_device( device, hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else 
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( device );
    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR, "rfgsm_core_handle_ptr is NULL for device %d!", device);
      dispatch_result = RFGSM_DISPATCH_UNSUCCESSFUL;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* ----------------------------------------------------------------------- */
  if (dispatch_result == RFGSM_DISPATCH_SUCCESSFUL)
  {
    // Check to see if the antenna is already in the requested position
    if( rfcommon_asdiv_get_current_position(device) == ant_pos )
    {
      RFGC_T_MSG_1( MSG_LEGACY_HIGH, "Requested switch pos %d already set", ant_pos );
      rfgsm_core_handle_ptr->set_antenna_position = (uint8)RF_INVALID_VALUE_8BIT;

      if ( rfgsm_msg_ptr->l1_cb_func != NULL)
      {
        // Antenna is already in the requested position
        rfa_rf_gsm_set_antenna_callback_s l1_cb_data;
                         
        // Populate the L1 callback data
        l1_cb_data.req_result = RFA_RF_GSM_SUCCESS;
        l1_cb_data.device     = device;
        l1_cb_data.ant_pos    = ant_pos;
        l1_cb_data.sub_id     = rfgsm_msg_ptr->sub_id;

        // Call the L1 callback function 
        rfgsm_msg_ptr->l1_cb_func( &l1_cb_data ); 

        // Set result to SUCCESS for CNF message to GL1
        cmd_result = RFA_RF_GSM_SUCCESS;
      }
      else
      {
        RFGC_T_MSG( MSG_LEGACY_ERROR, "L1 callback function ptr is NULL" );
      }
    }
    else
    {
      /* These will be stored in event hdr and returned by RFLM so store them away */
      rfgsm_core_handle_ptr->asd_l1_cb_func = rfgsm_msg_ptr->l1_cb_func; 
      rfgsm_core_handle_ptr->asd_sub_id = rfgsm_msg_ptr->sub_id;

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = device;
      rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                       RFA_RF_GSM_SET_ANTENNA_REQUEST_START, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       0,                     //duration us
                       start_pcycle,          //start time pcycles
                       0 );                   //duration pcycles

      if( rfgsm_mc_set_antenna_req(&rfgsm_core_data, ant_pos) == RFGSM_MC_SUCCESS )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
      }

      /* Only profile if rfgsm_core_handle_ptr is valid */
      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                       RFA_RF_GSM_SET_ANTENNA_REQUEST_STOP, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                       start_pcycle,          //start time pcycles
                       qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
    }
  }
  else
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM Set Antenna Request dispatcher failed variable verification for device %d", device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  cnf_required = FALSE;
  if( cnf_required )
  {
    /*Set the payload result to the result of the above functionality*/
    rfgsm_resp_msg.req_result    = cmd_result;
    rfgsm_resp_msg.hdr           = hdr;
    rfgsm_resp_msg.device        = device;
    rfgsm_resp_msg.cnf_required  = cnf_required;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_set_antenna_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Set Antenna cnf UNSUCCESSFUL, error_no %d",msgr_send_status);
    }

  }

  return(dispatch_result);
}
#endif


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_populate_ip2_cal_script

===========================================================================*/
/*! @fn rfgsm_dispatch_build_ip2_cal_script( rfa_rf_gsm_start_ip2_cal_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf )
 *  @brief This function dispatches the processing of populate ip2 cal script
 *  @param rfgsm_msg_ptr a pointer to the ip2 cal structure variable
 *  payload.
 *  @param rf_task
 *  @param umid_cnf
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */

rfgsm_dispatch_outcome_t rfgsm_dispatch_build_ip2_cal_script
( 
  rfa_rf_gsm_start_ip2_cal_req_s* rfgsm_msg_ptr,
  rf_task_num_type rf_task,
  uint32 umid_cnf
)
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_res = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_start_ip2_cal_cnf_s  rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfgsm_dispatch_build_ip2_cal_script is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  /* Get the handle and device*/
  if(!rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ))
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,"Device ID passed is greater than or equal to %d ", RFM_MAX_WAN_DEVICES);
    dispatch_res = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_msg_ptr->device);

    if (rfgsm_core_handle_ptr == NULL)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
      dispatch_res = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */

  if( dispatch_res == RFGSM_DISPATCH_SUCCESSFUL )
  {
      MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM Cmd start IP2 cal, "
            "dev_id=%d, umid=0x%x, dispatch result:%d ", rfgsm_msg_ptr->device,rfgsm_msg_ptr->hdr.id,dispatch_res);

      /* Set the core data */
      rfgsm_core_data.rf_task = rf_task;
      rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
      rfgsm_core_data.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION; /*! @todo rfgsm_msg_ptr->sub_id; */
      rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_core_data.sub_id);
      rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
      rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                       RFA_RF_GSM_DBG_START_IP2_CAL_REQUEST_START, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       0,                     //duration us
                       start_pcycle,          //start time pcycles
                       0 );                   //duration pcycles

      if ( rfgsm_mc_build_ip2_cal_script(&rfgsm_core_data) )
      {
        cmd_result = RFA_RF_GSM_SUCCESS;
      }

      RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                       RFA_RF_GSM_DBG_START_IP2_CAL_REQUEST_STOP, //api
                       rf_task,               //rf task
                       start_time_qs,         //command proc start qs
                       0,                     //arg1
                       0,                     //arg2
                       geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                       start_ustmr,           //start time USTMR
                       rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                       start_pcycle,          //start time pcycles
                       qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_res != RFGSM_DISPATCH_SUCCESSFUL  */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM build IP2 cal script dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_res, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {

    /* Echo REQ message info to CNF message info for functionality usage. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                   =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result               =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_start_ip2_cal_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM START IP2 CAL cnf UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return dispatch_res;

}

/*===========================================================================

  FUNCTION:  rfgsm_dispatch_init_msm_ip2_cal

===========================================================================*/
/*! @fn rfgsm_dispatch_init_msm_ip2_cal( rfa_rf_gsm_ip2_cal_msm_req_s* rfgsm_msg_ptr, rf_task_num_type rf_task, uint32 umid_cnf )
 *  @brief This function dispatches the processing of populate ip2 cal script
 *  @param rfgsm_msg_ptr a pointer to the ip2 cal structure variable
 *  payload.
 *  @param rf_task
 *  @param umid_cnf
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */

rfgsm_dispatch_outcome_t rfgsm_dispatch_init_msm_ip2_cal
( 
  rfa_rf_gsm_ip2_cal_msm_req_s* rfgsm_msg_ptr,
  rf_task_num_type rf_task,
  uint32 umid_cnf
)
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_res = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_ip2_cal_msm_cnf_s  rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfgsm_dispatch_init_msm_ip2_cal is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  /* Get the handle and device*/
  if(!rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ))
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,"Device ID passed is greater than or equal to %d ", RFM_MAX_WAN_DEVICES);
    dispatch_res = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
     /* Get the handle*/
     rfgsm_core_handle_ptr = rfgsm_core_handle_get(rfgsm_msg_ptr->device);
      
     if (rfgsm_core_handle_ptr == NULL)
     {
        RFGC_T_MSG_1(MSG_LEGACY_ERROR,"rfgsm_core_handle_ptr is NULL!", 0);
        dispatch_res = RFGSM_DISPATCH_NULL_PTR;
     }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */

  if( dispatch_res == RFGSM_DISPATCH_SUCCESSFUL )
  {
     MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM Cmd start init IP2 cal msm, "
            "dev_id=%d, umid=0x%x, dispatch result:%d ", rfgsm_msg_ptr->device,rfgsm_msg_ptr->hdr.id,dispatch_res);

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION; /*! @todo rfgsm_msg_ptr->sub_id; */
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_core_data.sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_DBG_INIT_MSM_IP2_CAL_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_msm_init_ip2_cal(&rfgsm_core_data ,rfgsm_msg_ptr->ip2_rxlm_buffer_idx,rfgsm_msg_ptr->ip2_txlm_buffer_idx) )
    {
       cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_DBG_INIT_MSM_IP2_CAL_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  }
  else /* dispatch_res != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM init msm ip2 cal dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_res, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {

    /* Echo REQ message info to CNF message info for functionality usage. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                   =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result   =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                           umid_cnf,
                                           sizeof(rfa_rf_gsm_ip2_cal_msm_cnf_s));
    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM INIT MSM IP2 CAL cnf UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return dispatch_res;

}




/*===========================================================================

  FUNCTION:  rfgsm_dispatch_sar_limit_ind

===========================================================================*/
/*! @fn rfgsm_dispatch_sar_limit_ind( rfa_rf_gsm_set_sar_limit_ind_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the SAR limit update received from MCS
 *  @param rfgsm_msg_ptr a pointer to the sar limit structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_sar_limit_ind( rfa_rf_gsm_set_sar_limit_ind_s* rfgsm_msg_ptr,
                                                       rf_task_num_type rf_task )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t dispatch_result = RFGSM_DISPATCH_UNSUCCESSFUL;
  uint32                   index = 0;

  rfa_rf_gsm_sar_limit_params_type sar_limit_params;
  int16                    sar_limit_gmsk[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  int16                    sar_limit_8psk[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  rfgsm_core_handle_type   *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type     rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfgsm_dispatch_sar_limit_ind is NULL", RFGSM_DISPATCH_NULL_PTR);

  sar_limit_params = rfgsm_msg_ptr->rfa_rf_gsm_sar_limit_params;

  /* Get the handle*/
  rfgsm_core_handle_ptr = rfgsm_core_handle_get(sar_limit_params.device);
  
  RF_CMD_NULL_CHECK_RTN(rfgsm_core_handle_ptr, "rfgsm_core_handle_ptr is NULL!", RFGSM_DISPATCH_NULL_PTR);

  if(sar_limit_params.device >= RFM_MAX_WAN_DEVICES)
  {
    RFGC_T_MSG_1(MSG_LEGACY_ERROR,
          "RF Device %d from MCS is not as expected", sar_limit_params.device);
    sar_limit_params.device = RFCOM_TRANSCEIVER_0;
  }
  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Copy GMSK and 8PSK modulation t xpwr limts into local structures*/
  for (index = 0; index < RFGSM_MAX_TX_SLOTS_PER_FRAME; index++)
  {
    sar_limit_gmsk[index] = sar_limit_params.sar_tx_limit[0][index];
    sar_limit_8psk[index] = sar_limit_params.sar_tx_limit[1][index];
  }

  /* Set the core data */
  rfgsm_core_data.rf_task = rf_task;
  rfgsm_core_data.rfm_dev = sar_limit_params.device;
  rfgsm_core_data.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION;
  rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_core_data.sub_id);
  rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
  rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

  RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                   RFA_RF_GSM_SET_SAR_LIMIT_INDICATION_START, //api
                   rf_task,               //rf task
                   start_time_qs,         //command proc start qs
                   0,                     //arg1
                   0,                     //arg2
                   geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                   start_ustmr,           //start time USTMR
                   0,                     //duration us
                   start_pcycle,          //start time pcycles
                   0 );                   //duration pcycles

  if( rfgsm_mc_set_simult_sar_limit(&rfgsm_core_data, sar_limit_gmsk, sar_limit_8psk) == RFGSM_MC_SUCCESS )
  {
    dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  }

  RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                   RFA_RF_GSM_SET_SAR_LIMIT_INDICATION_STOP, //api
                   rf_task,               //rf task
                   start_time_qs,         //command proc start qs
                   0,                     //arg1
                   0,                     //arg2
                   geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                   start_ustmr,           //start time USTMR
                   rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                   start_pcycle,          //start time pcycles
                   qurt_get_core_pcycles()-start_pcycle ); //duration pcycles

  return(dispatch_result);
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_cm_enter_req

===========================================================================*/
/*! @fn rfgsm_dispatch_cm_enter_req( rfa_rf_gsm_cm_enter_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for GSM.
 *  @param rfgsm_msg_ptr a pointer to the exit_mode structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_cm_enter_req( rfa_rf_gsm_cm_enter_req_s* rfgsm_msg_ptr, 
                                                      rf_task_num_type rf_task,
                                                      uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cm_enter_cnf_s     rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_cm_enter_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG_2( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd Proc CM Enter, dev_id=%d, cnf=%d", 
                  rfgsm_msg_ptr->device,
                  rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_CM_ENTER_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_cm_enter( &rfgsm_core_data, 
                           rfgsm_msg_ptr->timing_info ) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_CM_ENTER_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM CM Enter Mode dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                   =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result               =     cmd_result;

    if ( rfgsm_mc_retrieve_sawless_support(rfgsm_resp_msg.sawless_sup) == FALSE )
    {
      RFGC_T_MSG(MSG_LEGACY_ERROR, "Failed to retrieve SAWless status for logical devices during CM enter mode!");
    }

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_cm_enter_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Compressed Mode Enter MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_cm_exit_req

===========================================================================*/
/*! @fn rfgsm_dispatch_cm_exit_req( rfa_rf_gsm_cm_exit_req_s* rfgsm_msg_ptr )
 *  @brief This function dispatches the initialisation of the RF Driver for 
 *  a specific radio transceiver for GSM.
 *  @param rfgsm_msg_ptr a pointer to the exit_mode structure variable
 *  payload.
 *  @return rfgsm_dispatch_outcome_t enum variable describing the success/
 *  failure of the function.
 */
rfgsm_dispatch_outcome_t rfgsm_dispatch_cm_exit_req( rfa_rf_gsm_cm_exit_req_s* rfgsm_msg_ptr, 
                                                     rf_task_num_type rf_task,
                                                     uint32 umid_cnf )
{
  /* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t      dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t      cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_cm_exit_cnf_s      rfgsm_resp_msg;
  errno_enum_type               msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type        *rfgsm_core_handle_ptr = NULL;
  rfgsm_core_data_type          rfgsm_core_data;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_cm_exit_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result == RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG_2( MSG_LEGACY_HIGH, 
                  "RF GSM Cmd Proc CM Exit, dev_id=%d, cnf=%d", 
                  rfgsm_msg_ptr->device,
                  rfgsm_msg_ptr->cnf_required );

    /* Set the core data */
    rfgsm_core_data.rf_task = rf_task;
    rfgsm_core_data.rfm_dev = rfgsm_msg_ptr->device;
    rfgsm_core_data.sub_id = rfgsm_msg_ptr->sub_id;
    rfgsm_core_data.as_id = gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id);
    rfgsm_core_data.triple_buffer_id = RFLM_GSM_BUFFER_ID_NOT_REQUIRED;
    rfgsm_core_data.rfgsm_core_handle_ptr = rfgsm_core_handle_ptr;

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle
                     RFA_RF_GSM_CM_EXIT_REQUEST_START, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     0,                     //duration us
                     start_pcycle,          //start time pcycles
                     0 );                   //duration pcycles

    if( rfgsm_mc_cm_exit(&rfgsm_core_data) == RFGSM_MC_SUCCESS )
    {
      cmd_result = RFA_RF_GSM_SUCCESS;
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr, //core handle 
                     RFA_RF_GSM_CM_EXIT_REQUEST_STOP, //api
                     rf_task,               //rf task
                     start_time_qs,         //command proc start qs
                     0,                     //arg1
                     0,                     //arg2
                     geran_test_mode_api_get_FN(rfgsm_core_data.as_id), //SW frame num
                     start_ustmr,           //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,          //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }
  else /* dispatch_result != RFGSM_DISPATCH_SUCCESSFUL */
  {
    RFGC_T_MSG_1( MSG_LEGACY_ERROR, "RF GSM CM Exit Mode dispatcher failed variable verification for device %d", rfgsm_msg_ptr->device);

    /* Handle dispatch errors. */
    rfgsm_error_handler( dispatch_result, rfgsm_msg_ptr->hdr.id, rf_task, rfgsm_msg_ptr->device );
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                   =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result               =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_cm_exit_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Compressed Mode Exit MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);
}
/*===========================================================================

  FUNCTION:  rfgsm_dispatch_process_burst_metrics

===========================================================================*/
rfgsm_dispatch_outcome_t rfgsm_dispatch_process_burst_metrics( rfa_rf_gsm_process_burst_metrics_req_s* rfgsm_msg_ptr, 
                                                         rf_task_num_type rf_task, 
                                                         uint32 umid_cnf )
{
/* ----------------------------------------------------------------------- */
  /* Initialise Response and Result Variables */
  rfgsm_dispatch_outcome_t          dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfa_rf_gsm_cmd_outcome_t          cmd_result = RFA_RF_GSM_EXECUTION_ERROR;
  rfa_rf_gsm_process_burst_metrics_cnf_s  rfgsm_resp_msg;
  errno_enum_type                   msgr_send_status = E_FAILURE;
  rfgsm_core_handle_type            *rfgsm_core_handle_ptr = NULL;

  /* Time Profiling */
  unsigned long long start_pcycle = qurt_get_core_pcycles();
  uint32 start_ustmr = rfgsm_mc_read_ustmr_count();
  uint32 start_time_qs = 0;

  /* ----------------------------------------------------------------------- */
  /* Check common msgr msg ptr, device id , and rfgsm core handle validity   */
  RF_CMD_NULL_CHECK_RTN( rfgsm_msg_ptr, "Pointer argument into rfa_rf_gsm_process_burst_metrics_req_s is NULL", RFGSM_DISPATCH_NULL_PTR);

  /* Get the current frame-relative qs time 0-4999 */
  start_time_qs = geran_test_mode_api_get_qsym_count( gl1_hw_rf_map_rf_sub_id_to_as_id(rfgsm_msg_ptr->sub_id) );

  if( !rfgsm_dispatch_check_device( rfgsm_msg_ptr->device, rfgsm_msg_ptr->hdr.id ) )
  {
    dispatch_result = RFGSM_DISPATCH_INVALID_DEVICE;
  }
  else
  {
    rfgsm_core_handle_ptr = rfgsm_core_handle_get( rfgsm_msg_ptr->device );

    if( rfgsm_core_handle_ptr == NULL )
    {
      RFGC_T_MSG_1( MSG_LEGACY_ERROR,
            "rfgsm_core_handle_ptr is NULL for device %d!", rfgsm_msg_ptr->device);
      dispatch_result = RFGSM_DISPATCH_NULL_PTR;
    }
  }

  /* ----------------------------------------------------------------------- */
  /* Execution                                                               */
  /* Verify payload parameters. */
  if( dispatch_result != RFGSM_DISPATCH_SUCCESSFUL )
  {
    RFGC_T_MSG(MSG_LEGACY_ERROR, "RF GSM Process Metrics dispatcher failed variable verification");
  }
  else
  {
    if(rfgsm_core_handle_ptr->sawless_linearity_control.enable_logging == TRUE)
    {
      RFGC_T_MSG_2( MSG_LEGACY_HIGH, 
                    "RF GSM Cmd Proc Process Metrics, dev_id=%d, cnf=%d", 
                    rfgsm_msg_ptr->device,
                    rfgsm_msg_ptr->cnf_required );
    }

    if ( rfgsm_msg_ptr->num_metrics_entries > RFGSM_MAX_METRICS_ENTRIES )
    {
      RFGC_T_MSG_2(MSG_LEGACY_ERROR, 
                   "RF GSM Process Metrics: too many entries passed! Max: %d, Passed: %d", 
                   rfgsm_msg_ptr->num_metrics_entries, 
                   RFGSM_MAX_METRICS_ENTRIES);
      cmd_result = RFA_RF_GSM_L1_PARAM_ERROR;
    }
    else
    {
        if ( (rfgsm_mc_process_burst_metrics(rfgsm_msg_ptr->device, rfgsm_msg_ptr->metrics, rfgsm_msg_ptr->num_metrics_entries)) == TRUE )
        {
          cmd_result = RFA_RF_GSM_SUCCESS;
        }
    }

    RFGSM_DEBUG_EN_Q(rfgsm_core_handle_ptr,              //core handle 
                     RFA_RF_GSM_PROCESS_BURST_METRICS_REQUEST, //api
                     rf_task,                            //rf task
                     start_time_qs,                      //command proc start qs
                     rfgsm_msg_ptr->num_metrics_entries, //arg1
                     0,                                  //arg2
                     geran_test_mode_api_get_FN(SYS_MODEM_AS_ID_NONE), //SW frame num
                     start_ustmr,                        //start time USTMR
                     rfgsm_ustmr_count_diff_us_rtn(start_ustmr), //duration us
                     start_pcycle,                       //start time pcycles
                     qurt_get_core_pcycles()-start_pcycle ); //duration pcycles
  }

  /* ----------------------------------------------------------------------- */
  /* Response                                                                */
  if(rfgsm_msg_ptr->cnf_required)
  {
    /* Echo REQ message info to CNF message info for message confirmation. */
    rfgsm_resp_msg.hdr                      =     rfgsm_msg_ptr->hdr;
    rfgsm_resp_msg.device                   =     rfgsm_msg_ptr->device;
    rfgsm_resp_msg.cnf_required             =     rfgsm_msg_ptr->cnf_required;
    rfgsm_resp_msg.req_result               =     cmd_result;

    /* Send a confirmation message */
    msgr_send_status = rfgsm_send_msgr_cnf((msgr_hdr_struct_type *)&rfgsm_resp_msg,
                                            umid_cnf,
                                            sizeof(rfa_rf_gsm_process_burst_metrics_cnf_s));

    if (msgr_send_status != E_SUCCESS)
    {
      RFGC_T_MSG_1(MSG_LEGACY_ERROR, "RFGSM Process Metrics MSGR Send UNSUCCESSFUL, error_no %d",msgr_send_status);
    }
  }

  return(dispatch_result);

}

/*===========================================================================

  FUNCTION:  rfgsm_l1_msgr_register

===========================================================================*/
/*!
  @fn boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to register for commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to regsiter
  @param id the RF TASK command signal RF_CMD_Q_SIG flagging the TASK that there
            are commands waiting in the queue
  @return boolean TRUE if all registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Register for the RF GSM L1 Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_GSM,
                                   client_id,
                                   id,
                                   rfgsm_umid_req_list,
                                   RF_GSM_REQ_CNT(rfgsm_umid_req_list));


  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_l1_msgr_register: Unable to register for RF GSM L1 message router register block." );
    rc = FALSE;
  }

  return(rc);
}

/*===========================================================================

  FUNCTION:  rfgsm_msgr_register_sub2

===========================================================================*/
/*!
  @fn boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to register for commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to regsiter
  @param id the RF TASK command signal RF_CMD_Q_SIG flagging the TASK that there
            are commands waiting in the queue
  @return boolean TRUE if all registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_msgr_register_sub2( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Register for the RF GSM L1 Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_GSM,
                                   client_id,
                                   id,
                                   rfgsm_umid_req_sub2_list,
                                   RF_GSM_REQ_CNT(rfgsm_umid_req_sub2_list));
  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_l1_msgr_register: Unable to register for RF GSM L1 message router register block." );
    rc = FALSE;
  }

  return(rc);
}


/*===========================================================================

  FUNCTION:  rfgsm_msgr_register_sub3

===========================================================================*/
/*!
  @fn boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to register for commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to regsiter
  @param id the RF TASK command signal RF_CMD_Q_SIG flagging the TASK that there
            are commands waiting in the queue
  @return boolean TRUE if all registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_msgr_register_sub3( msgr_client_t *client_id, msgr_id_t id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Register for the RF GSM L1 Commands that the RF task is interested in receiving */
  ret_status = msgr_register_block(MSGR_RFA_RF_GSM,
                                   client_id,
                                   id,
                                   rfgsm_umid_req_sub3_list,
                                   RF_GSM_REQ_CNT(rfgsm_umid_req_sub3_list));
  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_l1_msgr_register: Unable to register for RF GSM L1 message router register block." );
    rc = FALSE;
  }

  return(rc);
}

/*===========================================================================

  FUNCTION:  rfgsm_msgr_deregister

===========================================================================*/
/*!
  @fn boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to de-register commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to de-regsiter
  @return boolean TRUE if all de-registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_msgr_deregister( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF GSM L1 message RF task has registered for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_GSM,
                                     client_id,
                                     rfgsm_umid_req_list,
                                     RF_GSM_REQ_CNT(rfgsm_umid_req_list));

  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_msgr_deregister: Unable to deregister RF message router register block." );
    rc = FALSE;
  }
  return(rc);
}

/*===========================================================================

  FUNCTION:  rfgsm_msgr_deregister_sub2

===========================================================================*/
/*!
  @fn boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to de-register commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to de-regsiter
  @return boolean TRUE if all de-registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_msgr_deregister_sub2( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF GSM L1 message RF task has registered for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_GSM,
                                     client_id,
                                     rfgsm_umid_req_sub2_list,
                                     RF_GSM_REQ_CNT(rfgsm_umid_req_sub2_list));
  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_msgr_deregister: Unable to deregister RF message router register block." );
    rc = FALSE;
  }
  return(rc);
}


/*===========================================================================

  FUNCTION:  rfgsm_msgr_deregister_sub3

===========================================================================*/
/*!
  @fn boolean rfgsm_msgr_register( msgr_client_t *client_id, msgr_id_t id )
  @brief This function is used by RF Task to de-register commands sent by the
         L1 task via the message router. This function is called from
         rf_task_msgr_register function which is called from RF Task itself
  @param client_id a pointer to the RF task control block to de-regsiter
  @return boolean TRUE if all de-registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rfgsm_msgr_deregister_sub3( msgr_client_t *client_id )
{
  boolean         rc         = TRUE;
  errno_enum_type ret_status = E_SUCCESS;

  /* Deregister all the RF GSM L1 message RF task has registered for. */
  ret_status = msgr_deregister_block(MSGR_RFA_RF_GSM,
                                     client_id,
                                     rfgsm_umid_req_sub3_list,
                                     RF_GSM_REQ_CNT(rfgsm_umid_req_sub3_list));
  if (ret_status != E_SUCCESS)
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_msgr_deregister: Unable to deregister RF message router register block." );
    rc = FALSE;
  }
  return(rc);
}



/*===========================================================================

  FUNCTION:  rfgsm_req_dispatcher

===========================================================================*/
/*!
  @brief This is the common command handler for rf gsm RFM commands
  @param req_ptr A pointer to a structure containing tcb and msgr details
  @return void
*/
/*=========================================================================*/
void rfgsm_req_dispatcher( rf_cmd_type* req_ptr )
{
  rfgsm_dispatch_outcome_t dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfgsm_msg_u rfgsm_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if( req_ptr != NULL )
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfgsm_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (uint8 *) &rfgsm_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( (void *)cpy_dest, sizeof(rfgsm_msg_u), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *) &req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy( (void *)cpy_dest, (sizeof(rfgsm_msg_u)-sizeof(rf_msgr_hdr_struct_type)), cpy_src, cpy_size );

      /* Dispatch the message */
      dispatch_result = rfgsm_req_dispatch(&rfgsm_msgr_req);

      if (dispatch_result != RFGSM_DISPATCH_SUCCESSFUL)
      {
        MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM command received from rf command dispatcher. GSM Command"
              "processor could not process the command. Exited with error code"
              "%d", dispatch_result );
        dispatch_result = RFGSM_DISPATCH_UNSUCCESSFUL;
      }
  }
}


/*===========================================================================

  FUNCTION:  rfgsm_req_dispatcher_2

===========================================================================*/
/*!
  @brief This is the common command handler for rf gsm RFM commands
  @param req_ptr A pointer to a structure containing tcb and msgr details
  @return void
*/
/*=========================================================================*/

void rfgsm_req_dispatcher_2( rf_cmd_type* req_ptr )
{
  rfgsm_dispatch_outcome_t dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfgsm_msg_u rfgsm_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if( req_ptr != NULL )
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfgsm_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (uint8 *) &rfgsm_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( (void *)cpy_dest, sizeof(rfgsm_msg_u), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *) &req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy( (void *)cpy_dest, (sizeof(rfgsm_msg_u)-sizeof(rf_msgr_hdr_struct_type)), cpy_src, cpy_size );

      dispatch_result = rfgsm_req_dispatch_2(&rfgsm_msgr_req);

      if (dispatch_result != RFGSM_DISPATCH_SUCCESSFUL)
      {
        MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM command received from rf command dispatcher. GSM Command"
                  "processor could not process the command. Exited with error code"
                  "%d", dispatch_result );
        dispatch_result = RFGSM_DISPATCH_UNSUCCESSFUL;
      }

  }

}



/*===========================================================================

  FUNCTION:  rfgsm_req_dispatcher_3

===========================================================================*/
/*!
  @brief This is the common command handler for rf gsm RFM commands
  @param req_ptr A pointer to a structure containing tcb and msgr details
  @return void
*/
/*=========================================================================*/

void rfgsm_req_dispatcher_3( rf_cmd_type* req_ptr )
{
  rfgsm_dispatch_outcome_t dispatch_result = RFGSM_DISPATCH_SUCCESSFUL;
  rfgsm_msg_u rfgsm_msgr_req;
  uint8 * cpy_dest;
  void * cpy_src;
  size_t cpy_size;

  if( req_ptr != NULL )
  {
    /*Limit memcpy message copy size to max allowable size*/
    cpy_size = MIN(sizeof(rfgsm_msg_u)-sizeof(rf_msgr_hdr_struct_type), RF_MAX_CMD_PAYLOAD_SIZE);

    cpy_src = (void *) &req_ptr->msgr_hdr;
    cpy_dest = (uint8 *) &rfgsm_msgr_req;

    /*Copy the MSGR request packet header */
    memscpy( (void *)cpy_dest, sizeof(rfgsm_msg_u), cpy_src, sizeof(rf_msgr_hdr_struct_type));

    /* Set the destination and source pointers to the payload */
    cpy_src = (void *) &req_ptr->payload;
    cpy_dest += sizeof(rf_msgr_hdr_struct_type);

    /* Copy the payload */
    memscpy( (void *)cpy_dest, (sizeof(rfgsm_msg_u)-sizeof(rf_msgr_hdr_struct_type)), cpy_src, cpy_size );

      dispatch_result = rfgsm_req_dispatch_3(&rfgsm_msgr_req);

      if (dispatch_result != RFGSM_DISPATCH_SUCCESSFUL)
      {
        MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "GSM command received from rf command dispatcher. GSM Command"
                  "processor could not process the command. Exited with error code"
                  "%d", dispatch_result );
        dispatch_result = RFGSM_DISPATCH_UNSUCCESSFUL;
      }

  }

}



/*===========================================================================

  FUNCTION:  rfgsm_dispatch_init

===========================================================================*/
/*!
  @fn void rfgsm_dispatch_init(void)
  @brief Registers the GSM module and dispatch handlers with the RF task dispatcher.
         This function is called from rfgsm_mc_init before any RF GSM L1 command is
         sent to the RF Driver Task.
*/
/*=========================================================================*/
void rfgsm_dispatch_init(void)
{
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_1, 
                                    MSGR_TECH_RFA, 
                                    MSGR_MODULE_RF_GSM, 
                                    rfgsm_req_dispatcher ) != RF_CMD_REGISTRATION_SUCCESS)
  {
     MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_dispatch_init:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_GSM "
               "registration" );
     return;
  }
}

/*===========================================================================

  FUNCTION:  rfgsm_dispatch_init_2

===========================================================================*/
/*!
  @fn void rfgsm_dispatch_init(void)
  @brief Registers the GSM module and dispatch handlers with the RF task dispatcher.
         This function is called from rfgsm_mc_init before any RF GSM L1 command is
         sent to the RF Driver Task.
*/
/*=========================================================================*/
void rfgsm_dispatch_init_2(void)
{
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_2, 
                                    MSGR_TECH_RFA, 
                                    MSGR_MODULE_RF_GSM, 
                                    rfgsm_req_dispatcher_2 ) != RF_CMD_REGISTRATION_SUCCESS)
  {
     MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_dispatch_init_2:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_GSM "
               "registration" );
     return;
  }
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_init_3

===========================================================================*/
/*!
  @fn void rfgsm_dispatch_init(void)
  @brief Registers the GSM module and dispatch handlers with the RF task dispatcher.
         This function is called from rfgsm_mc_init before any RF GSM L1 command is
         sent to the RF Driver Task.
*/
/*=========================================================================*/
void rfgsm_dispatch_init_3(void)
{
  /*Register with rf module dispatch table using RF RFA as the module.*/
  if(rf_cmd_dispatch_register_tech( RF_TASK_3, 
                                    MSGR_TECH_RFA, 
                                    MSGR_MODULE_RF_GSM, 
                                    rfgsm_req_dispatcher_3 ) != RF_CMD_REGISTRATION_SUCCESS)
  {
     MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_dispatch_init_3:Failed MSGR_TECH_RFA / MSGR_MODULE_RF_GSM "
               "registration" );
     return;
  }
}


/*===========================================================================

  FUNCTION:  rfgsm_dispatch_deinit

===========================================================================*/
/*!
  @fn void rfgsm_dispatch_deinit(void)
  @brief This funtion has to be called whenever the RF task is killed. Frees allocated
         memory and registration tables.
  @return void
  @TODO this function is not called at the moment but need verify if required because
        rfm_init is only called once. Need to understand if is is possible to create 
        multiple instances of the same module and therefore not deregistering could 
        be a problem if the case where multiple inits occurs. (Shouldn't occur)
*/
/*=========================================================================*/
void rfgsm_dispatch_deinit(void)
{
   /*Clean up the rf command dispatch table which contains all the pointers to 
     individual command handler. The rf module dispatch table is cleaned by 
     rf_req_dispatcher when rf task is killed.
   */
   /*! @TODO the module dispatch de-registration returns a success/failure flag 
             which needs to be handled here.
   */
  rf_cmd_dispatch_deregister_tech( RF_TASK_1, MSGR_TECH_RFA, MSGR_MODULE_RF_GSM);
}

/*===========================================================================

  FUNCTION:  rfgsm_dispatch_deinit_2

===========================================================================*/
/*!
  @fn void rfgsm_dispatch_deinit(void)
  @brief This funtion has to be called whenever the RF task is killed. Frees allocated
         memory and registration tables.
  @return void
  @TODO this function is not called at the moment but need verify if required because
        rfm_init is only called once. Need to understand if is is possible to create 
        multiple instances of the same module and therefore not deregistering could 
        be a problem if the case where multiple inits occurs. (Shouldn't occur)
*/
/*=========================================================================*/
void rfgsm_dispatch_deinit_2(void)
{
   /*Clean up the rf command dispatch table which contains all the pointers to 
     individual command handler. The rf module dispatch table is cleaned by 
     rf_req_dispatcher when rf task is killed.
   */
   /*! @TODO the module dispatch de-registration returns a success/failure flag 
             which needs to be handled here.
   */
  rf_cmd_dispatch_deregister_tech( RF_TASK_2, MSGR_TECH_RFA, MSGR_MODULE_RF_GSM);
}

/*===========================================================================

  FUNCTION:  rfgsm_dispatch_deinit_3

===========================================================================*/
/*!
  @fn void rfgsm_dispatch_deinit(void)
  @brief This funtion has to be called whenever the RF task is killed. Frees allocated
         memory and registration tables.
  @return void
  @TODO this function is not called at the moment but need verify if required because
        rfm_init is only called once. Need to understand if is is possible to create 
        multiple instances of the same module and therefore not deregistering could 
        be a problem if the case where multiple inits occurs. (Shouldn't occur)
*/
/*=========================================================================*/
void rfgsm_dispatch_deinit_3(void)
{
   /*Clean up the rf command dispatch table which contains all the pointers to 
     individual command handler. The rf module dispatch table is cleaned by 
     rf_req_dispatcher when rf task is killed.
   */
   /*! @TODO the module dispatch de-registration returns a success/failure flag 
             which needs to be handled here.
   */
  rf_cmd_dispatch_deregister_tech( RF_TASK_3, MSGR_TECH_RFA, MSGR_MODULE_RF_GSM);
}



/*===========================================================================
  FUNCTION:  rfgsm_req_dispatch
===========================================================================*/
/*!
  @fn rfgsm_dispatch_outcome_t rfgsm_req_dispatch(rfgsm_msg_u* msg_ptr)
  @brief This function is the main dispatch function for all the REQs the RF
         task will receive. Once it receives a command, it basically dispatches
         it to the appropriate function again in this file. That function will
         basically send the command to the repective MC function and then once that 
         is executed, it will send back a the reponse. That function will also echo 
         the arguments of the original command.
  @param msg_ptr a pointer to the structure variable of the payload
  @return rfgsm_dispatch_outcome_t 
*/
/*=========================================================================*/
rfgsm_dispatch_outcome_t rfgsm_req_dispatch(rfgsm_msg_u* msg_ptr)
{
  rfgsm_dispatch_outcome_t req_cnf;
  msgr_hdr_s   *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 

  if (msg_ptr != NULL)
  {
    switch ( msgr_hdr_ptr->id )
    {
    case RFA_RF_GSM_ENTER_MODE_REQ:
      req_cnf = rfgsm_dispatch_enter_mode_req((rfa_rf_gsm_enter_mode_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_ENTER_MODE_CNF);
      break;
    case RFA_RF_GSM_RX_ENTER_MODE_REQ:
      req_cnf = rfgsm_dispatch_rx_enter_mode_req((rfa_rf_gsm_enter_mode_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_RX_ENTER_MODE_CNF);
      break;
    case RFA_RF_GSM_TX_ENABLE_REQ:
      req_cnf = rfgsm_dispatch_tx_enable_req((rfa_rf_gsm_tx_enable_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_TX_ENABLE_CNF);
      break;
    case RFA_RF_GSM_TX_DISABLE_REQ:
      req_cnf = rfgsm_dispatch_tx_disable_req((rfa_rf_gsm_tx_disable_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_TX_DISABLE_CNF);
      break;
    case RFA_RF_GSM_WAKEUP_REQ:
      req_cnf = rfgsm_dispatch_wakeup_req((rfa_rf_gsm_wakeup_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_WAKEUP_CNF);
      break;
    case RFA_RF_GSM_SLEEP_REQ:
      req_cnf = rfgsm_dispatch_sleep_req((rfa_rf_gsm_sleep_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_SLEEP_CNF);
      break;
    case RFA_RF_GSM_RX_BURST_REQ:
      req_cnf = rfgsm_dispatch_rx_burst_req((rfa_rf_gsm_rx_burst_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_RX_BURST_CNF);
      break;
    case RFA_RF_GSM_SET_TX_BAND_REQ:
      req_cnf = rfgsm_dispatch_set_tx_band_req((rfa_rf_gsm_set_tx_band_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_SET_TX_BAND_CNF);
      break;
    case RFA_RF_GSM_TX_BURST_REQ:
      req_cnf = rfgsm_dispatch_tx_burst_req((rfa_rf_gsm_tx_burst_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_TX_BURST_CNF);
      break;
    case RFA_RF_GSM_IDLE_FRAME_PROCESSING_REQ:
      req_cnf = rfgsm_dispatch_idle_frame_processing_req((rfa_rf_gsm_idle_frame_processing_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_IDLE_FRAME_PROCESSING_CNF);
      break;
    case RFA_RF_GSM_EXIT_MODE_REQ:
      req_cnf = rfgsm_dispatch_exit_mode_req((rfa_rf_gsm_exit_mode_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_EXIT_MODE_CNF);
      break;
    case RFA_RF_GSM_SET_SAR_LIMIT_IND:
      req_cnf = rfgsm_dispatch_sar_limit_ind((rfa_rf_gsm_set_sar_limit_ind_s*) msg_ptr, RF_TASK_1);
      break;
    case RFA_RF_GSM_CM_ENTER_REQ:
      req_cnf = rfgsm_dispatch_cm_enter_req( (rfa_rf_gsm_cm_enter_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_CM_ENTER_CNF);
      break;
    case RFA_RF_GSM_CM_EXIT_REQ:
      req_cnf = rfgsm_dispatch_cm_exit_req( (rfa_rf_gsm_cm_exit_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_CM_EXIT_CNF);
      break;
    case RFA_RF_GSM_UPDATE_TEMP_COMP_REQ:
      req_cnf = rfgsm_dispatch_update_temp_comp_req( (rfa_rf_gsm_update_temp_comp_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_UPDATE_TEMP_COMP_CNF);
      break;
    #ifdef FEATURE_RF_ASDIV
    case RFA_RF_GSM_SET_ANTENNA_REQ:
      req_cnf = rfgsm_dispatch_set_antenna_req( (rfa_rf_gsm_set_antenna_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_SET_ANTENNA_CNF);      
      break;
    #endif
    case RFA_RF_GSM_START_IP2_CAL_REQ:
      req_cnf = rfgsm_dispatch_build_ip2_cal_script((rfa_rf_gsm_start_ip2_cal_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_START_IP2_CAL_CNF);
      break;
    case RFA_RF_GSM_INIT_MSM_IP2_CAL_REQ:
      req_cnf = rfgsm_dispatch_init_msm_ip2_cal((rfa_rf_gsm_ip2_cal_msm_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_INIT_MSM_IP2_CAL_CNF);
      break;
    case RFA_RF_GSM_PROCESS_BURST_METRICS_REQ:
      req_cnf = rfgsm_dispatch_process_burst_metrics((rfa_rf_gsm_process_burst_metrics_req_s*) msg_ptr, RF_TASK_1, RFA_RF_GSM_PROCESS_BURST_METRICS_CNF);
      break;

    default :
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "Undefined GSM REQ 0x%08x for SUB1", msgr_hdr_ptr->id );
      req_cnf = RFGSM_DISPATCH_UNSUCCESSFUL;
      break;
    }
  } 
  else
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer NULL" );
    req_cnf = RFGSM_DISPATCH_NULL_PTR;
  }
  return(req_cnf);
}


/*===========================================================================*/
/*!
  @fn rfgsm_dispatch_outcome_t rfgsm_req_dispatch_2(rfgsm_msg_u* msg_ptr) for 
      task 2/subscriber2
  @brief This function is the main dispatch function for all the REQs the RF
         task will receive. Once it receives a command, it basically dispatches
         it to the appropriate function again in this file. That function will
         basically send the command to the repective MC function and then once that 
         is executed, it will send back a the reponse. That function will also echo 
         the arguments of the original command.
  @param msg_ptr a pointer to the structure variable of the payload
  @return rfgsm_dispatch_outcome_t 
*/
/*=========================================================================*/
rfgsm_dispatch_outcome_t rfgsm_req_dispatch_2(rfgsm_msg_u* msg_ptr)
{
  rfgsm_dispatch_outcome_t req_cnf;
  msgr_hdr_s   *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 

  if (msg_ptr != NULL)
  {
    switch ( msgr_hdr_ptr->id )
    {
    case RFA_RF_GSM_ENTER_MODE_SUB2_REQ:
      req_cnf = rfgsm_dispatch_enter_mode_req((rfa_rf_gsm_enter_mode_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_ENTER_MODE_SUB2_CNF);
      break;
    case RFA_RF_GSM_RX_ENTER_MODE_SUB2_REQ:
      req_cnf = rfgsm_dispatch_rx_enter_mode_req((rfa_rf_gsm_enter_mode_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_RX_ENTER_MODE_SUB2_CNF);
      break;	  
    case RFA_RF_GSM_TX_ENABLE_SUB2_REQ:
      req_cnf = rfgsm_dispatch_tx_enable_req((rfa_rf_gsm_tx_enable_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_TX_ENABLE_SUB2_CNF);
      break;
    case RFA_RF_GSM_TX_DISABLE_SUB2_REQ:
      req_cnf = rfgsm_dispatch_tx_disable_req((rfa_rf_gsm_tx_disable_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_TX_DISABLE_SUB2_CNF);
      break;
    case RFA_RF_GSM_WAKEUP_SUB2_REQ:
      req_cnf = rfgsm_dispatch_wakeup_req((rfa_rf_gsm_wakeup_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_WAKEUP_SUB2_CNF);
      break;
    case RFA_RF_GSM_SLEEP_SUB2_REQ:
      req_cnf = rfgsm_dispatch_sleep_req((rfa_rf_gsm_sleep_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_SLEEP_SUB2_CNF);
      break;
    case RFA_RF_GSM_RX_BURST_SUB2_REQ:
      req_cnf = rfgsm_dispatch_rx_burst_req((rfa_rf_gsm_rx_burst_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_RX_BURST_SUB2_CNF);
      break;
    case RFA_RF_GSM_SET_TX_BAND_SUB2_REQ:
      req_cnf = rfgsm_dispatch_set_tx_band_req((rfa_rf_gsm_set_tx_band_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_SET_TX_BAND_SUB2_CNF);
      break;
    case RFA_RF_GSM_TX_BURST_SUB2_REQ:
      req_cnf = rfgsm_dispatch_tx_burst_req((rfa_rf_gsm_tx_burst_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_TX_BURST_SUB2_CNF);
      break;
    case RFA_RF_GSM_IDLE_FRAME_PROCESSING_SUB2_REQ:
      req_cnf = rfgsm_dispatch_idle_frame_processing_req((rfa_rf_gsm_idle_frame_processing_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_IDLE_FRAME_PROCESSING_SUB2_CNF);
      break;
    case RFA_RF_GSM_EXIT_MODE_SUB2_REQ:
      req_cnf = rfgsm_dispatch_exit_mode_req((rfa_rf_gsm_exit_mode_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_EXIT_MODE_SUB2_CNF);
      break;
    case RFA_RF_GSM_SET_SAR_LIMIT_SUB2_IND:
      req_cnf = rfgsm_dispatch_sar_limit_ind((rfa_rf_gsm_set_sar_limit_ind_s*) msg_ptr, RF_TASK_2 );
      break;
    case RFA_RF_GSM_CM_ENTER_SUB2_REQ:
      req_cnf = rfgsm_dispatch_cm_enter_req( (rfa_rf_gsm_cm_enter_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_CM_ENTER_SUB2_CNF);
      break;
    case RFA_RF_GSM_CM_EXIT_SUB2_REQ:
      req_cnf = rfgsm_dispatch_cm_exit_req( (rfa_rf_gsm_cm_exit_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_CM_EXIT_SUB2_CNF);
      break;
    case RFA_RF_GSM_UPDATE_TEMP_COMP_SUB2_REQ:
      req_cnf = rfgsm_dispatch_update_temp_comp_req( (rfa_rf_gsm_update_temp_comp_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_UPDATE_TEMP_COMP_SUB2_CNF);
      break;
    #ifdef FEATURE_RF_ASDIV
    case RFA_RF_GSM_SET_ANTENNA_SUB2_REQ:
      req_cnf = rfgsm_dispatch_set_antenna_req( (rfa_rf_gsm_set_antenna_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_SET_ANTENNA_SUB2_CNF);      
      break;
    #endif
    case RFA_RF_GSM_START_IP2_CAL_SUB2_REQ:
      req_cnf = RFGSM_DISPATCH_SUCCESSFUL;
      break;
    case RFA_RF_GSM_PROCESS_BURST_METRICS_SUB2_REQ:
      req_cnf = rfgsm_dispatch_process_burst_metrics((rfa_rf_gsm_process_burst_metrics_req_s*) msg_ptr, RF_TASK_2, RFA_RF_GSM_PROCESS_BURST_METRICS_SUB2_CNF);
      break;

    default :
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "Undefined GSM REQ 0x%08x for SUB2", msgr_hdr_ptr->id );
      req_cnf = RFGSM_DISPATCH_UNSUCCESSFUL;
      break;
    }
  } 
  else
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer NULL" );
    req_cnf = RFGSM_DISPATCH_NULL_PTR;
  }
  return(req_cnf);
}


/*===========================================================================*/
/*!
  @fn rfgsm_dispatch_outcome_t rfgsm_req_dispatch_2(rfgsm_msg_u* msg_ptr) for 
      task 2/subscriber2
  @brief This function is the main dispatch function for all the REQs the RF
         task will receive. Once it receives a command, it basically dispatches
         it to the appropriate function again in this file. That function will
         basically send the command to the repective MC function and then once that 
         is executed, it will send back a the reponse. That function will also echo 
         the arguments of the original command.
  @param msg_ptr a pointer to the structure variable of the payload
  @return rfgsm_dispatch_outcome_t 
*/
/*=========================================================================*/
rfgsm_dispatch_outcome_t rfgsm_req_dispatch_3(rfgsm_msg_u* msg_ptr)
{
  rfgsm_dispatch_outcome_t req_cnf;
  msgr_hdr_s   *msgr_hdr_ptr = (msgr_hdr_s*) msg_ptr; 

  if (msg_ptr != NULL)
  {
    switch ( msgr_hdr_ptr->id )
    {
    case RFA_RF_GSM_ENTER_MODE_SUB3_REQ:
      req_cnf = rfgsm_dispatch_enter_mode_req((rfa_rf_gsm_enter_mode_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_ENTER_MODE_SUB3_CNF);
      break;
    case RFA_RF_GSM_RX_ENTER_MODE_SUB3_REQ:
      req_cnf = rfgsm_dispatch_rx_enter_mode_req((rfa_rf_gsm_enter_mode_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_RX_ENTER_MODE_SUB3_CNF);
      break;
    case RFA_RF_GSM_TX_ENABLE_SUB3_REQ:
      req_cnf = rfgsm_dispatch_tx_enable_req((rfa_rf_gsm_tx_enable_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_TX_ENABLE_SUB3_CNF);
      break;
    case RFA_RF_GSM_TX_DISABLE_SUB3_REQ:
      req_cnf = rfgsm_dispatch_tx_disable_req((rfa_rf_gsm_tx_disable_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_TX_DISABLE_SUB3_CNF);
      break;
    case RFA_RF_GSM_WAKEUP_SUB3_REQ:
      req_cnf = rfgsm_dispatch_wakeup_req((rfa_rf_gsm_wakeup_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_WAKEUP_SUB3_CNF);
      break;
    case RFA_RF_GSM_SLEEP_SUB3_REQ:
      req_cnf = rfgsm_dispatch_sleep_req((rfa_rf_gsm_sleep_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_SLEEP_SUB3_CNF);
      break;
    case RFA_RF_GSM_RX_BURST_SUB3_REQ:
      req_cnf = rfgsm_dispatch_rx_burst_req((rfa_rf_gsm_rx_burst_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_RX_BURST_SUB3_CNF);
      break;
    case RFA_RF_GSM_SET_TX_BAND_SUB3_REQ:
      req_cnf = rfgsm_dispatch_set_tx_band_req((rfa_rf_gsm_set_tx_band_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_SET_TX_BAND_SUB3_CNF);
      break;
    case RFA_RF_GSM_TX_BURST_SUB3_REQ:
      req_cnf = rfgsm_dispatch_tx_burst_req((rfa_rf_gsm_tx_burst_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_TX_BURST_SUB3_CNF);
      break;
    case RFA_RF_GSM_IDLE_FRAME_PROCESSING_SUB3_REQ:
      req_cnf = rfgsm_dispatch_idle_frame_processing_req((rfa_rf_gsm_idle_frame_processing_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_IDLE_FRAME_PROCESSING_SUB3_CNF);
      break;
    case RFA_RF_GSM_EXIT_MODE_SUB3_REQ:
      req_cnf = rfgsm_dispatch_exit_mode_req((rfa_rf_gsm_exit_mode_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_EXIT_MODE_SUB3_CNF);
      break;
    case RFA_RF_GSM_SET_SAR_LIMIT_SUB3_IND:
      req_cnf = rfgsm_dispatch_sar_limit_ind((rfa_rf_gsm_set_sar_limit_ind_s*) msg_ptr, RF_TASK_3);
      break;
    case RFA_RF_GSM_CM_ENTER_SUB3_REQ:
      req_cnf = rfgsm_dispatch_cm_enter_req( (rfa_rf_gsm_cm_enter_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_CM_ENTER_SUB3_CNF);
      break;
    case RFA_RF_GSM_CM_EXIT_SUB3_REQ:
      req_cnf = rfgsm_dispatch_cm_exit_req( (rfa_rf_gsm_cm_exit_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_CM_EXIT_SUB3_CNF);
      break;
    case RFA_RF_GSM_UPDATE_TEMP_COMP_SUB3_REQ:
      req_cnf = rfgsm_dispatch_update_temp_comp_req( (rfa_rf_gsm_update_temp_comp_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_UPDATE_TEMP_COMP_SUB3_CNF);
      break;
    case RFA_RF_GSM_PROCESS_BURST_METRICS_SUB3_REQ:
      req_cnf = rfgsm_dispatch_process_burst_metrics((rfa_rf_gsm_process_burst_metrics_req_s*) msg_ptr, RF_TASK_3, RFA_RF_GSM_PROCESS_BURST_METRICS_SUB3_CNF);
      break;

    default :
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "Undefined GSM REQ 0x%08x for SUB3", msgr_hdr_ptr->id );
      req_cnf = RFGSM_DISPATCH_UNSUCCESSFUL;
      break;
    }
  } 
  else
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Pointer NULL" );
    req_cnf = RFGSM_DISPATCH_NULL_PTR;
  }
  return(req_cnf);
}

/*===========================================================================
  FUNCTION:  rfgsm_rflm_rsp_dispatch
===========================================================================*/
/*!
  @fn void rfgsm_rflm_rsp_dispatch(rf_task_num_type task_num)
  @brief This function is the main dispatch function for all the RSPs the RF
         task will receive from RFLM TECH. Once it receives a command, it 
         dispatches it to the appropriate function in this file. That function 
         then sends the command to the respective MC function and then once
         that is executed, it returns a msg to free the msr router q
  @param task_num RF Task the message was received on
  @return void 
*/

void rfgsm_rflm_rsp_dispatch(rf_task_num_type task_num)
{
  rfgsm_rflm_msgr_t*  cmd_ptr  = NULL;
  rfgsm_dispatch_outcome_t result = RFGSM_DISPATCH_SUCCESSFUL;

  ASSERT(task_num < RF_MAX_TASK);

  /* get messages from the MSGR queue*/
  while ((cmd_ptr = (rfgsm_rflm_msgr_t *) q_get(&rfgsm_rflm_cmd_intf_msgr_q[task_num])) != NULL)
  {    
    switch (cmd_ptr->msg.hdr.id )
    {
    case RFLM_GSM_THERM_READ_RSP:
#ifdef FEATURE_DUAL_SIM
    case RFLM_GSM_THERM_READ_SUB2_RSP:
#endif
      result = rfgsm_dispatch_therm_read_rsp(&(cmd_ptr->msg), task_num);
     break;
#ifdef FEATURE_RF_ASDIV
    case RFLM_GSM_ASD_RSP:
#ifdef FEATURE_DUAL_SIM
    case RFLM_GSM_ASD_SUB2_RSP:
#endif
      result = rfgsm_dispatch_asd_read_rsp(&(cmd_ptr->msg), task_num);
    break;
#endif
    case RFLM_GSM_DEVICE_STATUS_READ_RSP:
#ifdef FEATURE_DUAL_SIM
    case RFLM_GSM_DEVICE_STATUS_READ_SUB2_RSP:
#endif
      result = rfgsm_dispatch_device_status_read_rsp(&(cmd_ptr->msg), task_num);
    break;

    default :
      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Undefined RFLM REQ" );
      result = RFGSM_DISPATCH_SUCCESSFUL;
      break;
    }
   /* Indicate error if RF Task confirmation message does not indicate success */
    if(result != RFGSM_DISPATCH_SUCCESSFUL)
    {  
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "RFGSM RFLM Response Dispatch failed: %d", result );
    }

    /* Return message to the free queue */
    q_put( &rfgsm_rflm_cmd_intf_msgr_free_q[task_num], &cmd_ptr->link);
  }
  return;
}

/*===========================================================================*/
/*!
  @fn void rfgsm_initialise_rflm_msg_router_q( void )
  @brief This function initialises the msg router q to handle all the RSPs the RF
         task will receive from RFLM. A new signal is registered which will be set
         whenever there is a response cmd from RFLM
  @param task_num RF Task the message was received on
  @return void
*/
/*=========================================================================*/
void rfgsm_initialise_rflm_msg_router_q( rf_task_num_type task_num )
{
   errno_enum_type  msgr_error;
   msgr_client_t    msgr_client;
   msgr_id_t        msgr_id;

   uint32  i = 0;

   ASSERT(task_num < RF_MAX_TASK);

   (void) q_init( &rfgsm_rflm_cmd_intf_msgr_q[task_num] );
   (void) q_init( &rfgsm_rflm_cmd_intf_msgr_free_q[task_num] );

   i = 0;

   while( i < RFGSM_RFLM_MSGR_QUEUE_SIZE )
   {
     msgr_hdr_struct_type*  tmp_ptr   = (msgr_hdr_struct_type *) &(rfgsm_rflm_msgr[task_num][i].msg.hdr);
     q_link_type*           link_item_ptr = q_link( tmp_ptr, &(rfgsm_rflm_msgr[task_num][i].link) );

     q_put( &rfgsm_rflm_cmd_intf_msgr_free_q[task_num], link_item_ptr );

     i++;
   }

   /* Register the queue and free queue with the message router */
   msgr_error = msgr_client_create(&msgr_client);

   ASSERT(msgr_error == E_SUCCESS);

   msgr_error = msgr_client_add_rex_q(
                                      &msgr_client,                              //!< Message Client Handle
                                      rex_self(),                                //!< REX TCB pointer
                                      RF_RFLM_CMD_Q_SIG,                          //!< REX TCB pointer
                                      &rfgsm_rflm_cmd_intf_msgr_q[task_num],      //!< queue to place the msg
                                      &rfgsm_rflm_cmd_intf_msgr_free_q[task_num], //!< queue to get empty buffer
                                      offsetof(rfgsm_rflm_msgr_t, msg),           //!< offset to msgr_hdr
                                      MSGR_NO_OFFSET,                            //!< offset to legacy cmd_type
                                      sizeof(rfgsm_rflm_msgr_t),                  //!< max size of msgs
                                      &msgr_id                                   //!< queue identifier
                                     );

   ASSERT(msgr_error == E_SUCCESS);
      
   if ( task_num == RF_TASK_1 )
   {
     msgr_error = msgr_register_block(MSGR_RFLM_GSM, &msgr_client, msgr_id, rfgsm_rflm_rsp_umids, 
                                      (RF_GSM_RFLM_RSP_CNT(rfgsm_rflm_rsp_umids)) );
   }
      
   if ( task_num == RF_TASK_2 )
   {
     msgr_error = msgr_register_block(MSGR_RFLM_GSM, &msgr_client, msgr_id, rfgsm_rflm_rsp_sub2_umids,
                                      (RF_GSM_RFLM_RSP_CNT(rfgsm_rflm_rsp_sub2_umids)) );
   } 

   ASSERT(msgr_error == E_SUCCESS);
}

