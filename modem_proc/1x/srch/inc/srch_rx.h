#ifndef SRCH_RX_H
#define SRCH_RX_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H  _ R X  ---  H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_rx.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005 - 2015 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_rx.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/16   pk      Always tune diveristy to same band/chan, primary is tuned to 
07/29/15   srk     Removed exporting static functions.
09/16/15   ab      Enable 1X Split Overheads for all MSIM combinations
05/04/15   pap     Adding log packets for IDLE DIV stats
04/21/15   srk     Unification of IDLE DIV algorithm for all CRAT modes.
04/09/15   sst     Add new PCH MER Log packet
03/05/15   srk     Remove TRM deprecated APIs.
02/23/15   pap     DR status will not be modified when next wakeup is too close
02/06/15   srk     Add additional warmup slacks when LTE is in traffic.
01/21/15   srk     TRM get info API call optimization.
01/21/15   bb      Changes to reserve with ACCESS when both PI bits detected as ONE
12/15/14   pap     Ensuring G in BG traffic is checked only during wakeup when
                   hybrid fallback has been issued
12/10/14   pap     Detecting diversity denials in cases of DTA for idle states
12/05/14   pap     Adding hysteresis for hybrid fallback mode.
11/25/14   srk     Consolidate ASDIV API export to a single header file.
11/17/14   pap     Cleanup logic to set conditions in IDLE DIV algorithm.
11/13/14   pk      Fixes for IDLE DIV in QPCH offtl timeline
11/11/14   pap     Updating IDLE DIV algorithm for DR cases.
11/07/14   srk     Fix a TRM assertion on enabling/disabling DR mode.
11/02/14   pap     Disabling DR when hybrid fallback is triggered by 1x.
10/13/14   jh      Remove srch_rx_int32_compare(), no longer needed
10/10/14   srk     Cleanup ASDIV related code for DR-DSDS.
10/08/14   jh      Move srch_rx_int32_compare to srch_rx_util.c
10/01/14   cjb     Use devices granted by trm_set_client_state for IRAT
09/09/14   pk      Cleanup for LTE compile out flavor.
09/04/14   cjb     Dynamically determine default PRx and DRx devices
08/29/14   sst     Updates to UESIN
08/27/14   as      Need to populate lte_device_id in 1xtoL
08/15/14   cjb     Diag command to enable/disable new idle div algo for DR mode
08/13/14   pap     Adding updated IDLE DIV algorithm for DR-DSDS cases.
08/08/14   as      Fix IRAT for Dual Receive
08/05/14   cjb     Treat chain_2 and chain_3 as independent devices in DR mode
07/22/14   ab      Changes to update TRM with correct ASD sleep information
07/10/14   as      For DR diversity the resource needs to be TRM_RX_DIVERSITY
07/07/14   ab      Add the PCH decode failure check for IDLE ASD
06/17/14   srk     Decouple Dynamic IDLE DIV from DIV NV.
06/02/14   srk     Fix IDLE DIV in QPCH OFFLINE timeline and SOOS cleanup.
05/30/14   srk     IDLE ASD fixes.
05/08/14   pk      Added API to check if primary rf tune is supported on a
                   particular band/chan
04/28/14   cjb     Renamed SRCH_RF_MAX_DEVICES to SRCH_RF_INVALID_DEVICE.
04/24/14   cjb     BOLT revisit cleanup
04/17/14   cjb     Dynamically handle RF_DEV assignment from TRM for TX case
03/25/14   bph     DSDS/DSDA Feature bring up. traffic+traffic
03/17/14   cjb     Dynamically handle RF_DEV assignment from TRM
03/10/14   bph     Add feature gaurd for ASDiv DSDS/DSDA code
02/18/14   cjb     Changes to support SHDR on BOLT modem with WTR3925
01/20/14   bph     IDLE AS-DIV support
01/16/14   pk      Added API to check if rf tune is supported on a particular
                   band/chan.
11/18/13   pap     To provide an API to provide the open-loop turnaround const.
10/21/13   srk     Remove unused APIs.
09/10/13   as      Reverting previous checkin to satisfy AsDIV feature
09/06/13   srk     Remove unused APIs.
08/20/13   dkb     Mainline FEATURE_INIT_2
08/16/13   vks     Mainline FEATURE_ZZ2_2, FEATURE_SRCH_DRX_SUPPORTS_ONE_BAND
07/23/13   srk     Replace RF API calls with mux API.
06/19/13   ab      New API added to return the RF device in use
04/16/13   bph     Avoid calling RF API w/ untuned RF device
02/19/13   cjb     Changes to support LTE->1x along with new LTE CA feature.
11/07/12   cjb     Updated RF device for SV-DIV operation.
10/31/12   bb      Changes to define srch_rx_get_owned_device()
10/19/12   srk     Remove unused function prototype.
10/15/12   dkb     Add mechanism to dynamically enable on the fly zz2_2 support
10/02/12   adw/dkb Add on the fly zz2_2 support.
09/19/12   vks     Div interlock support no longer required.
09/06/12   vks     Set wakeup chain to RX_ANY on chain denial for best possible
08/16/12   ab      Changes for Removing RX Manual Transition during Hard HO.
06/18/12   jj      Add wrapper function for rfm_cdma_get_sub_class,
                   rfm_cdma_is_band_chan_supported.
06/12/12   adw     Allow initial desired device to be specified in ZZ2_2 reset.
05/23/12   vks     Add wrapper function for rfm_get_band_mask.
05/15/12   srk     Removed redudant functions to return tx power.
04/13/12   mca     Added srch_rx_get_last_home_rx_agc()
03/01/12   sst     Added srch_rx_get_rin_enable
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/10/12   adw     Add APIs to get RF prep and exec overheads separately.
02/01/12   sst     Correct srch_rx_rin_div_ctrl prototype
01/27/12   cjb/vks Use SRCH_RF_RECEIVER_4 for second chain in SV mode and
                   SRCH_RF_RECEIVER_SV_DIV for diversity in SV mode.
                   Remove usage of SRCH_RF_RECEIVER_DUAL (no longer valid).
01/24/12   sst     Implementation of RIN
01/23/12   srk     Feature Cleanup
01/11/12   srk     Feature Cleanup.
12/18/11   adw     Add function to set fing carrier and demod.
12/15/11   vks     Add support to recheck div interlock when HDR/1x retunes
                   to different band.
11/28/11   sst/jtm Updates for Diversity in SV mode
11/15/11   vks     Add api to set reserve chain.
11/10/11   jtm     Integrate ZZ2_2 support.
10/31/11   adw     Integrate TRAM support.
10/31/11   jtm     Feature clean up.
10/03/11   adw/cjb Fix dynamic switching for common chain override api.
09/02/11   sk      Rebased the code to the NikeL modem package by mainlining
                   FEATURE_MODEM_1X_NIKEL.
08/18/11   vks     Clean up references to sample server which does not exist
                   anymore (since genesis).
                   Clean up references to fast agc loop which does not exist
                   anymore (since poseidon).
08/17/11   vks     Remove references to sample server in SRCH RX module (
                   sample server is not present since 9k).
08/08/11   cjb     Added IRAT measurement support for NikeL.
                   Mainline FEATURE_ONEX_USES_RF_SCRIPTS_FOR_IRAT.
08/05/11   vks     Fix compiler issues.
07/12/11   vks/bb  Add support for SHDR functionality.
07/11/11   vks     Mainline FEATURE_SRCH_RX_STM and FEATURE_SRCH_RX_SVDO.
06/30/11   vks     Fix MOB compiler error.
06/28/11   jtm     Fixed SRCH_RF_MAX_TX_DEVICES definition.
06/22/11   vks     Add an api to check if srch rx is in sv mode.
06/22/11   vks     Add support for srch_rx_owns_other_chain().
06/20/11   vks     Initial set of changes to get RX DIV working with RX STM.
06/16/11   adw     Added execute wakeup interface.
06/03/11   adw     SV support.
05/05/11   adw     Fix compilation errors when lto1x is enabled on NikeL.
04/27/11   adw     Initial SV support.
04/25/11   cjb     Added function to return whether sleep should be disabled,
                   while we are in second_chain_test mode.
04/15/11   pk      Added support for SRCH_RX_STATUS
04/12/11   bb      Changes to define srch_rx_update_acq_band_info() function
04/12/11   vks     Add a notification callback for releasing owned chains.
03/16/11   mca     Support to store the wakeup information (agc, device, etc)
02/25/11   cjb     Fixed the O-freq scan limitation when DRx supports one band.
02/17/11   cjb     Changes needed for "DRx supports only one band".
02/25/11   vks     Fix compiler issue arising out of undefining 1x supports
                   rf feature.
02/11/11   sst     Add pass through functions for script based tuning
02/14/11   vks     First set of changes to get SRCH RX STM working.
01/27/11   vks     Add wrapper function for vbatt_read in srch_rx module.
01/26/11   vks     Featurize RF interface with FEATURE_MODEM_1X_SUPPORTS_RF.
                   Add wrapper functions to the rf functions that other modules
                   in SRCH are calling.
01/21/11   vks     Replace FEATURE_SRCH_RF_SVDO_API with FEATURE_1X_RF_SVDO_API
11/30/10   cjb     Second_Chain_Test [Option-2] implementation.
11/01/10   jtm     Modem 1x SU API changes.
07/20/10   bb      Code changes to support RF measurement scripts
                   for IRAT
06/24/10   bb      Code changes to fix the page fault issue
05/28/10   adw     Eliminated discrepancies between 1x and RF device type enums.
04/23/10   adw     Removed deprecated spectral inversion API.
04/12/10   sst     Added srch_rx_mdsp_pause() api
01/26/10   adw     Added srch_rx_set_sample_server_mode().
11/27/09   bb      Code changes to support "LTE to 1X IRAT" feature
                   implementation
10/28/09   sst     Update to RX_STM to work with legacy RF API
10/21/09   adw     Removed _chain_assn() as it should not be exported.
10/02/09   sst     TRMv3 & SVDO RF development (featurized)
06/10/09   sst     Initial changes to make trm v3 compile
04/24/09   adw     Changed srch_rx_enter_mode() paramater type.
04/06/09   mca     Changes for the RF SVDO API
04/01/09   adw     Categorized included header files and standardized format.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
03/03/09   adw     Lint fixes.
02/10/09   adw     Merge from 2h08 branch, based on VU_MODEM_1X_SRCH.13.00.01
01/30/09   sst     Added srch_rx_release_owned_chains()
12/01/08   adw     Commented out _v.h includes from subsystems outside 1x.
11/19/08   adw     Merge from main/latest, based on VU_MODEM_1X_SRCH.12.00.24
11/10/08   mca     Added new RF warmup API
10/27/08   aps     Removed featurization of srch_rx_mdsp_chain_assn()
09/08/08   aps     srch_rx_enter_mode() api change
08/18/08   adw     Added SINGLE_ANTENNA to featurize alt chain for ULC.
                   Added FEATURE_SRCH_HAS_NO_TRM to remove TRM for ULC.
06/23/08   aps     FTS - support for band configurability
06/20/08   mca     Merge from //depot
05/01/08   aps     Added parameter to srch_rx_exchange_devices() api
03/17/08   pa      Pass on origination pending to srch_rx_request_both_chains()
03/13/08   sst     Modify FTS version to be determined at runtime
02/22/08   aps     FTS3 - added srch_rx_request_and_notify_last_failed()
02/11/08   aps     Full Time SHDR 3/4 changes
04/27/07   aps     Wrapper function for SRCH_RX_MDSP_CHAIN_ASSN()
07/17/06   aps     Fix for rf sub packet being wrongly built in diversity
                   mode.
05/23/06   rkc     Add bypass_rtc_sync parameter to srch_rx_init_and_tune()
04/09/06   grl     Converted TRM_RX_BEST_POSSIBLE to TRM_RX_ANY if not enough
                   time from reservation to request.
04/04/06   awj     Added translation for TRM_RX_BEST and TRM_RX_BEST_POSSIBLE
02/07/06   bt      Added srch_rx_prepare_to_sleep.
02/02/06   awj     Added srch_rx_chain_is_ready
10/25/05   awj     Added default chain selection NV support
10/18/05   awj     Removed deprecated type
10/17/05   kwo     Lint Cleanup
09/20/05   bt      Use last valid AGC on home band/channel for SOOS and added
                   srch_rx_get_home_rx_agc.
08/29/05   kwo     Added srch_rx_complete_tune()
08/19/05   bt      Added srch_rx_bypass_rtc_sync.
08/19/05   ejv     Use srch_rx_chan_type for chan references.
08/09/05   grl     Added SOOS algorithm support.
07/19/05   grl     Simplified RF chain initialization API.
06/22/05   sfm     Added several new functions and updated function headers
06/09/05   sfm     Added srch_rx_change_priority()
06/07/05   sfm     removed srch_rx_tune_wait_cb()
06/03/05   sfm     srch_rx naming changes
06/01/05   grl     Added functions to get the current band and channel.
05/31/05   rng     Merged from SHDR Sandbox.
01/05/05   bt      Implementation, first cut.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* Srch */
#include "srch_genlog.h"
#include "srch_genlog_i.h"
#include "srch_nv.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"

/* Other */
#include "lm_types.h"
#include "mcccsfbsup.h"
#include "srch4drv_t.h"
#include "timetick.h"
#include "trm.h"

/* Feature Dependent */
#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "rfm_1x.h"
#include "rfm_device_types.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "rfm_meas.h"
#include "rfmeas_lte.h"
#include "rfmeas_types.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#include "gl1_hw.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* Callback function definition for RF granted events. */
typedef void (*srch_rx_granted_cb_t)( void );

/* Callback function definition for tune completions. */
typedef void (*srch_rx_tune_cb_t)( void );

/* This is emulating the rf device type provided by rfcom_device_enum_type */
typedef enum
{
  /*---------- non-SV chains ----------*/
  SRCH_RF_TRANSCEIVER_0 = 100,  /* Primary Radio transceiver */
  SRCH_RF_RECEIVER_1,           /* Secondary Receiver        */
  SRCH_RF_RECEIVER_2,           /* Dual Receive: Chain_2     */
  SRCH_RF_RECEIVER_3,           /* Dual Receive: Chain_3     */

  /*------------ SV chains ------------*/
  SRCH_RF_TRANSCEIVER_1,        /* 2nd-ary Radio transceiver */
  SRCH_RF_RECEIVER_4,           /* Secondary Receiver Dev2   */

  /*--------- DIV ---------*/
  SRCH_RF_TRANSCEIVER_DIV,         /* TxRx Diversity */
  SRCH_RF_RECEIVER_DIV,
  SRCH_RF_RECEIVER_SV_DIV,      /* SV Recieve Diversity      */
  SRCH_RF_TRANSCEIVER_PREF_DIV,

  /*------- DEFAULT/INVALID chain -------*/
  SRCH_RF_INVALID_DEVICE        /* Invalid device            */

} rx_sm_device_enum_t;

typedef struct
{
  boolean wakeup_seen; /* marks that we've seen at least one wakeup */
  int8    rx0_agc;     /* receive power (chain 0) */
  int8    rx1_agc;     /* receive power (chain 1) */
  int16   tx_pwr;      /* transmit power */
  rx_sm_device_enum_t dev; /* which antenna collected the data */
} srch_rx_wakeup_info_type;

typedef struct
{
  uint8 good_pages;
  uint8 bad_pages;
}srch_paging_stats_type;
extern srch_paging_stats_type srch_paging_stats;

/* Flag to check whether diversity was requested and not granted */
extern boolean is_div_req_and_not_grant;

/*-------------------------------------------------------------------------
      Macros
-------------------------------------------------------------------------*/
#define SRCH_RX_SETUP_MSG_CLT( rx_client, command )                     \
  {                                                                     \
    rx_sm_req_t *rx_cmd_payload;                                        \
                                                                        \
    if ( rx_client == RX_CLIENT_1X_PRI)                                 \
    {                                                                   \
      /* grab a outgoing message buffer for srch rx */                  \
      rx_cmd_payload = (rx_sm_req_t *)                                  \
        STM_ALLOC_MSG_BUF( &SRCH_RX_SM_P,                               \
                           &SRCH_COMMON_GROUP,                          \
                           command,                                     \
                           sizeof(rx_sm_req_t));                        \
    }                                                                   \
    else                                                                \
    {                                                                   \
      /* grab a outgoing message buffer for srch rx */                  \
      rx_cmd_payload = (rx_sm_req_t *)                                  \
        STM_ALLOC_MSG_BUF( &SRCH_RX_SM_S,                               \
                           &SRCH_COMMON_GROUP,                          \
                           command,                                     \
                           sizeof(rx_sm_req_t));                        \
    }                                                                   \
                                                                        \
    srch_rx_init_stm_cmd( rx_cmd_payload );

#define SRCH_RX_SETUP_MSG( sm, command )                                \
  {                                                                     \
    rx_sm_req_t *rx_cmd_payload;                                        \
                                                                        \
    /* grab a outgoing message buffer for srch rx */                    \
    rx_cmd_payload = (rx_sm_req_t *)                                    \
      STM_ALLOC_MSG_BUF( sm,                                            \
                         &SRCH_COMMON_GROUP,                            \
                         command,                                       \
                         sizeof(rx_sm_req_t));                          \
                                                                        \
    srch_rx_init_stm_cmd( rx_cmd_payload );

#define SRCH_RX_SEND_MSG()                                              \
    stm_put_internal_cmd_payload( (void *) rx_cmd_payload );            \
  }

// For DR mode the resource for diversity needs to be TRM_RX_DIVERSITY
#ifdef FEATURE_MODEM_1X_DUAL_RECEIVE
/* changed because TRM_DIV is used for chain #1 only*/
#define SRCH_RX_TRM_DIVERSITY TRM_RX_ANY
#else
#define SRCH_RX_TRM_DIVERSITY TRM_RX_SECONDARY
#endif /* FEATURE_MODEM_1X_DUAL_RECEIVE */

/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_RX_GET_DEFAULT_PRX_DEVICE

DESCRIPTION    This function return the PRX device (currently also TX capable)
               used by 1x in the current mode of operation (SV v/s non-SV modes)

DEPENDENCIES   None.

RETURN VALUE   PRx rf_dev if any. Else, RFM_INVALID_DEVICE

SIDE EFFECTS   None.

===========================================================================*/
extern rfm_device_enum_type srch_rx_get_default_prx_device
(
  sys_channel_type           band_chan      /* CDMA band & channel */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_DEFAULT_DRX_DEVICE

DESCRIPTION    This function return the DRX device (RX only) used by 1x in
               the current mode of operation (SV v/s non-SV modes)

DEPENDENCIES   None.

RETURN VALUE   DRx rf_dev if any. Else, RFM_INVALID_DEVICE

SIDE EFFECTS   None.

===========================================================================*/
extern rfm_device_enum_type srch_rx_get_default_drx_device
(
  sys_channel_type           band_chan      /* CDMA band & channel */
);


/*===========================================================================

FUNCTION       SRCH_RX_GET_DEVICE

DESCRIPTION    This function returns the RF device for the specified chain.
               RFCOM_RECEIVER_DIV will be returned if diversity is enabled.

DEPENDENCIES   None.

RETURN VALUE   The rf device.

SIDE EFFECTS   None.

===========================================================================*/
extern rx_sm_device_enum_t srch_rx_get_device
(
  rx_sm_client_enum_t chain_name    /* chain */
);

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
/*===========================================================================

FUNCTION       SRCH_RX_GET_DEVICE_IN_USE

DESCRIPTION    This function returns the RF device for the specified chain.

DEPENDENCIES   None.

RETURN VALUE   The rf device.

SIDE EFFECTS   None.

===========================================================================*/
extern rfm_device_enum_type srch_rx_get_device_in_use
(
  rx_sm_client_enum_t        rx_client      /* RX State Machine */
);

#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

/*===========================================================================

FUNCTION      SRCH_RX_GET_TUNED_DEVICE

DESCRIPTION   This function returns a tuned RF device.

DEPENDENCIES  This function is not to be called if no chains are tuned

RETURN VALUE  The tuned rf device, or if none tuned then RFM_INVALID_DEVICE

SIDE EFFECTS  None

===========================================================================*/
extern rfm_device_enum_type srch_rx_get_tuned_device( void );

/*===========================================================================

FUNCTION      SRCH_RX_GET_TX_DEVICE

DESCRIPTION   This function returns the RF Tx device.

DEPENDENCIES  None

RETURN VALUE  The RF Tx device in use.

SIDE EFFECTS  None

===========================================================================*/
extern rfm_device_enum_type srch_rx_get_tx_device( void );

/*===========================================================================

FUNCTION      SRCH_RX_GET_CARRIER

DESCRIPTION   This function returns which carrier is in use.

DEPENDENCIES  None

RETURN VALUE  The carrier in use.

SIDE EFFECTS  None

===========================================================================*/
extern srch4_carrier_type srch_rx_get_carrier( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_TRM_INFO

DESCRIPTION    This function is the wrapper for trm_get_info API.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_get_trm_info
(
  trm_client_enum_t         query_client_id,
  trm_band_t                query_band,
  trm_get_info_type         info_type,
  trm_get_info_return_type* output
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_RF_DEVICE

DESCRIPTION    This function gets the RF device information of the client
               being queried.

DEPENDENCIES   None

RETURN VALUE   RF device information of the client being queried.

SIDE EFFECTS   None

===========================================================================*/
extern rfm_device_enum_type srch_rx_get_rf_device
(
  trm_client_enum_t query_client_id
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_GRANTED

DESCRIPTION    This function indicates if a client holds an RF resource.
               Ideally, the client should already know and maintain knowledge
               of whether or not it holds an RF resource.

DEPENDENCIES   None.

RETURN VALUE   TRM_GRANTED_X: The granted TRM chain for the queried client.
               TRM_DENIED:    If the queried client doesn't own any RF device.

SIDE EFFECTS   None.

===========================================================================*/
extern trm_grant_event_enum_t srch_rx_get_granted
(
  trm_client_enum_t query_client_id
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_CLIENT_TRM_REASON

DESCRIPTION    This function gets the TRM reason of the client being queried.

DEPENDENCIES   None

RETURN VALUE   TRM reason of the client being queried

SIDE EFFECTS   None

===========================================================================*/
extern trm_reason_enum_t srch_rx_get_client_trm_reason
(
  trm_client_enum_t query_client_id
);

/*===========================================================================

FUNCTION       SRCH_RX_GSM_IS_IN_BG_TRAFFIC

DESCRIPTION    This function will check and see if G is in a data call.

DEPENDENCIES   None

RETURN VALUE   TRUE:  G is in  BG TRAFFIC (data call)
               FALSE: Otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_rx_gsm_is_in_bg_traffic( void );

/*===========================================================================

FUNCTION       SRCH_RX_TECH2_IS_IN_BG_TRAFFIC

DESCRIPTION    This function will check if a tech is in data call.

DEPENDENCIES   None

RETURN VALUE   TRUE:  Any Tech is in  BG TRAFFIC (data call)
               FALSE: Otherwise

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_rx_tech2_is_in_bg_traffic( void );

/*===========================================================================

FUNCTION      SRCH_RX_LTE_IN_TRAFFIC

DESCRIPTION   This function checks if LTE is in Connected state

DEPENDENCIES  None

RETURN VALUE  True:  LTE/LTE CA is in Connected State
              False: Otherwise

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_lte_in_traffic
(
  boolean query_trm
);

/*===========================================================================

FUNCTION      SRCH_RX_DUAL_RECEIVE_IS_ENABLED

DESCRIPTION   This function returns TRUE if Dual Receive is enabled

DEPENDENCIES  None

RETURN VALUE  TRUE  : Dual Receive ENABLED
              FALSE : Dual Receive DISABLED

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_dual_receive_is_enabled
(
  boolean query_trm
);

/*===========================================================================

FUNCTION      SRCH_RX_CHAIN_IS_IN_SV_MODE

DESCRIPTION   This function determines if the SRCH RX module is in SV mode

DEPENDENCIES  None

RETURN VALUE  True if the specified chain is RX1, FALSE otherwise

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_is_in_sv_mode
(
  boolean query_trm
);

/*===========================================================================

FUNCTION      SRCH_RX_CHAIN_IS_IN_SRLTE_MODE

DESCRIPTION   This function determines if the SRCH RX module is in SRLTE mode

DEPENDENCIES  None

RETURN VALUE  True if it is in SRLTE mode otherwise FALSE

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_is_in_srlte_mode( void );

/*===========================================================================

FUNCTION      SRCH_RX_IN_DUAL_RX_MODE

DESCRIPTION   This function determines if the SRCH RX module is not second chain
              and in DR mode.

DEPENDENCIES  None

RETURN VALUE  True if in DR mode and not second chain FTM, FALSE otherwise

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_in_dual_rx_mode
(
  boolean query_trm
);

#ifdef FEATURE_MODEM_1X_DUAL_RECEIVE
/*===========================================================================

FUNCTION       SRCH_RX_HANDLE_CSFB_DONE

DESCRIPTION    Handler for CP to indicate Pass/Fail

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_handle_csfb_done
(
  boolean status
);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION       SRCH_RX_GET_CSFB_REASON

DESCRIPTION    This function determines if the current state is CSFB

DEPENDENCIES   None.

RETURN VALUE   True if current state is CSFB

SIDE EFFECTS   None.

===========================================================================*/
extern trm_reason_enum_t srch_rx_get_csfb_reason
(
  mcc_csfb_status_e status
);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION       SRCH_RX_IS_CSFB

DESCRIPTION    This function determines if the current state is CFSB

DEPENDENCIES   None.

RETURN VALUE   True if current state is CFSB

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_csfb(void);

/*===========================================================================

FUNCTION       SRCH_RX_GET_IRAT_DEV_GRANT_PTR

DESCRIPTION    This function returns a ptr to IRAT device grant data structure

DEPENDENCIES   None.

RETURN VALUE   Pointer to device grant data structure

SIDE EFFECTS   None.

===========================================================================*/
extern trm_set_irat_return_data* srch_rx_get_irat_dev_grant_ptr( void );
#endif /* FEATURE_MODEM_1X_DUAL_RECEIVE */

/*===========================================================================

FUNCTION       SRCH_RX_IS_DEVICE_RXTX

DESCRIPTION    This function determines if the specified device is RXTX

DEPENDENCIES   None.

RETURN VALUE   True if specified device is RXTX, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_device_rxtx
(
  rx_sm_device_enum_t rf_device
);

/*===========================================================================

FUNCTION       SRCH_RX_IS_DEVICE_RX_ONLY

DESCRIPTION    This function determines if the specified device is RX only

DEPENDENCIES   None.

RETURN VALUE   True if specified device is RX only, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_device_rx_only
(
  rx_sm_device_enum_t rf_device
);

/*===========================================================================

FUNCTION       SRCH_RX_IS_DEVICE_DIVERSITY

DESCRIPTION    This function determines if the specified device is diversity

DEPENDENCIES   None.

RETURN VALUE   True if specified device is diversity, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_device_diversity
(
  rx_sm_device_enum_t rf_device
);

/*===========================================================================

FUNCTION       SRCH_RX_CHAIN_IS_RXTX

DESCRIPTION    This function determines if the specified chain is RXTX

DEPENDENCIES   None.

RETURN VALUE   True if specified chain is RXTX, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_chain_is_rxtx
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_CHAIN_IS_RX_ONLY

DESCRIPTION    This function determines if the specified chain is RX

DEPENDENCIES   None.

RETURN VALUE   True if the specified chain is RX1, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_chain_is_rx_only
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_STATUS

DESCRIPTION    This function returns the status of a chain

DEPENDENCIES   None.

RETURN VALUE   Chain status.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_status_type srch_rx_get_status
(
  rx_sm_client_enum_t chain_name    /* chain */
);

/*===========================================================================
FUNCTION      SRCH_RX_GET_PRIMARY_CLIENT_BAND_CHAN

DESCRIPTION   If primary Rx client owns RF chain, it returns TRUE, and also
              updates the band/chan stored in primary client struct.

DEPENDENCIES  None

RETURN VALUE  TRUE, if primary Rx clients owns RF chain, else FALSE.

SIDE EFFECTS  None
===========================================================================*/

extern boolean srch_rx_get_primary_client_band_chan
(
  srch_rx_band_type *band,   /* returned band class */
  srch_rx_chan_type *channel /* returned channel number */	
);


/*===========================================================================

FUNCTION       SRCH_RX_GET_BAND

DESCRIPTION    Returns the current band for the specified chain

DEPENDENCIES   None.

RETURN VALUE   The current band.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_band_type srch_rx_get_band
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_CHAN

DESCRIPTION    Returns the current channel for the specified chain

DEPENDENCIES   None.

RETURN VALUE   The current channel.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_chan_type srch_rx_get_chan
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION      SRCH_RX_CHAIN_IS_DIV_CAPABLE

DESCRIPTION   This function determines if the specified chain is div capable

DEPENDENCIES  None.

RETURN VALUE  True if specified chain is div capable, FALSE otherwise

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_chain_is_div_capable
(
  rx_sm_client_enum_t        rx_client      /* RX State Machine */
);

/*===========================================================================

FUNCTION       SRCH_RX_IS_DIVERSITY_REQUIRED

DESCRIPTION    This function determines if the current primary device is same
               as expected while determining next wakeup chain otherwise it
               returns FALSE.

DEPENDENCIES   None.

RETURN VALUE   True if conditions are met, FALSE otherwise

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_diversity_required(void);

/*===========================================================================

FUNCTION       SRCH_RX_GET_HOME_BAND

DESCRIPTION    This function returns the home band

DEPENDENCIES   None.

RETURN VALUE   The home band.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_band_type srch_rx_get_home_band( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_HOME_CHAN

DESCRIPTION    This function returns the home channel

DEPENDENCIES   None.

RETURN VALUE   The home channel.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_chan_type srch_rx_get_home_chan( void );

/*===========================================================================

FUNCTION      SRCH_RX_IS_BAND_CHAN_SUPPORTED

DESCRIPTION   This function checks with RF is the band and channel passed is
              supported or not.

DEPENDENCIES  None.

RETURN VALUE  TRUE if band_chan supported, FALSE otherwise.

SIDE EFFECTS  None.

===========================================================================*/
extern boolean srch_rx_is_band_chan_supported
(
  srch_rx_band_type         band,           /* returned band class */
  srch_rx_chan_type         channel         /* returned channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_CURR_BAND_CHAN

DESCRIPTION    This function returns the home band and channel

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern void srch_rx_get_curr_band_chan
(
  rx_sm_client_enum_t      chain_name,    /* chain */
  srch_rx_band_type       *band,          /* returned band class */
  srch_rx_chan_type       *channel        /* returned channel number */
);

/*===========================================================================

FUNCTION      SRCH_RX_GET_BAND_MASK

DESCRIPTION   SRCH wrapper function for rfm_get_band_mask. The rf api is
              used to know the bands supported by a given device. The upper
              layers can use this srch wrapper to know the bands supported
              and SRCH uses the device in use to get the information. This
              way the upper layers need not worry about mapping the correct
              rf device.

DEPENDENCIES  None

RETURN VALUE  RF band mask - bands supported by RF

SIDE EFFECTS  None

===========================================================================*/
extern sys_band_mask_type srch_rx_get_band_mask( void );


/*===========================================================================

FUNCTION      SRCH_RX_GET_BAND_SUB_CLASS_MASK

DESCRIPTION   SRCH wrapper function for rfm_cdma_get_sub_class. The rf api is
              used to know the band subclass supported by a given device for a
              given band class.
              The upper layers can use this srch wrapper to know the bands
              subclass supported by a band class and SRCH uses the device in
              use to get the information. This way the upper layers need not
              worry about mapping the correct rf device.

DEPENDENCIES  None

RETURN VALUE  RF band subclass mask for a given band supported by RF

SIDE EFFECTS  None

===========================================================================*/
extern uint32 srch_rx_get_band_sub_class_mask( rf_card_band_type band_class );

/*===========================================================================

FUNCTION      SRCH_RX_IS_BAND_CHAN_SUPPORTED

DESCRIPTION   SRCH wrapper function for rfm_cdma_is_band_chan_supported.
              This function checks if the (band, chan) is valid by consulting
              this RF API.
              The upper layers can use this srch wrapper to know if a
              band/chan is supported in the device.This way the upper layers
              need not worry about mapping the correct rf device.

DEPENDENCIES  None

RETURN VALUE  RF band/chan which needs to be checked.

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_is_rf_band_chan_supported( sys_channel_type band_chan );

/*===========================================================================
FUNCTION      SRCH_RX_IS_RF_TUNE_SUPPORTED_ON_BAND_CHAN

DESCRIPTION   SRCH wrapper function for rfm_is_band_chan_supported_v2.
              This function consults RF API to determine if 1x can tune a
              channel, without requiring rfm_enter_mode() to be called first.

DEPENDENCIES  None

RETURN VALUE  is_tune_supported
                  TRUE if rf tune to band/channel is supported.
                  FALSE if rf tune to band/channel is not supported.

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_is_rf_tune_supported_on_band_chan
(
  srch_rx_band_type band,
  srch_rx_chan_type chan
);

/*===========================================================================
FUNCTION      SRCH_RX_IS_PRIMARY_RF_TUNE_SUPPORTED_ON_BAND_CHAN

DESCRIPTION   SRCH wrapper function for rfm_is_band_chan_supported.
              This function consults RF API to determine if 1x priamary RF
              client can tune to a channel.

DEPENDENCIES  None

RETURN VALUE  is_tune_supported
                TRUE if rf tune to band/channel is supported.
                FALSE if rf tune to band/channel is not supported.

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_is_primary_rf_tune_supported_on_band_chan
(
  srch_rx_band_type band,
  srch_rx_chan_type chan
);


/*===========================================================================

FUNCTION       SRCH_RX_GET_RX_CARRIER_FREQ

DESCRIPTION    This function returns the carrier frequency for a given band
               class and channel.

DEPENDENCIES   None.

RETURN VALUE   Frequency in KHz.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_rx_get_rx_carrier_freq
(
  srch_rx_band_type         band,           /* returned band class */
  srch_rx_chan_type         channel         /* returned channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_FREQ_RANGE

DESCRIPTION    This function returns the frequency range corresponding to
               a given band class.

DEPENDENCIES   None.

RETURN VALUE   Frequency range.

SIDE EFFECTS   none

===========================================================================*/
extern srch_rx_freq_range_type srch_rx_get_freq_range
(
  srch_rx_band_type band_class  /* Band class */
);

/*===========================================================================

FUNCTION       SRCH_RX_OWNS_CHAIN

DESCRIPTION    This function determines if 1X owns a chain associated with
               the specified chain_name.

DEPENDENCIES   None.

RETURN VALUE   True if we own an antenna, otherwise FALSE.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_owns_chain
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION      SRCH_RX_OWNS_OTHER_CHAIN

DESCRIPTION   This function determines if 1X owns a chain associated with
               the chain other than the specified chain_name.
               (TUNE, OWNED, OWNED_DISABLED, MODIFY_PENDING,
                MODIFY_TUNE_PENDING)

DEPENDENCIES  None

RETURN VALUE  True if we own an antenna, otherwise FALSE.

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_owns_other_chain
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION      SRCH_RX_OWNS_BOTH_CHAINS

DESCRIPTION   This function determines if 1X owns both the primary and
              secondary chain.
              (TUNE, OWNED, OWNED_DISABLED, MODIFY_PENDING,
               MODIFY_TUNE_PENDING)

DEPENDENCIES  None.

RETURN VALUE  True if we own both antennae, otherwise FALSE.

SIDE EFFECTS  None.

===========================================================================*/
extern boolean srch_rx_owns_both_chains( void );

/*===========================================================================

FUNCTION      SRCH_RX_OWNS_ANY_CHAIN

DESCRIPTION   This function determines if 1X owns any chain.
              (TUNE, OWNED, OWNED_DISABLED, MODIFY_PENDING,
               MODIFY_TUNE_PENDING)

DEPENDENCIES  None.

RETURN VALUE  True if we own an antenna, otherwise FALSE.

SIDE EFFECTS  None.

===========================================================================*/
extern boolean srch_rx_owns_any_chain( void );

/*===========================================================================

FUNCTION       SRCH_RX_TUNED

DESCRIPTION    This function determines if 1X has an active antenna that
               is tuned and ready to demodulate.

DEPENDENCIES   None.

RETURN VALUE   True if we are active on an antenna, otherwise FALSE.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_tuned
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION      SRCH_RX_USES_OTHER_CHAIN

DESCRIPTION   This function determines if 1X has an active antenna on the
              other chain that is tuned and ready to demodulate.
               (OWNED, MODIFY_PENDING, MODIFY_TUNE_PENDING)

DEPENDENCIES  None

RETURN VALUE  True if we are active on an antenna, otherwise FALSE.

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_uses_other_chain
(
  rx_sm_client_enum_t  chain_name    /* chain */
);

/*===========================================================================

FUNCTION      SRCH_RX_USES_BOTH_CHAINS

DESCRIPTION   This function determines if 1X has an active antenna on both
              the chains that is tuned and ready to demodulate.
              (OWNED, MODIFY_PENDING, MODIFY_TUNE_PENDING)

DEPENDENCIES  None.

RETURN VALUE  True if we are active on both antennae, otherwise FALSE.

SIDE EFFECTS  None.

===========================================================================*/
extern boolean srch_rx_uses_both_chains( void );

/*===========================================================================

FUNCTION      SRCH_RX_USES_ANY_CHAIN

DESCRIPTION   This function determines if 1X has an active antenna on any of
              the chain that is tuned and ready to demodulate.
              (OWNED, MODIFY_PENDING, MODIFY_TUNE_PENDING)

DEPENDENCIES  None.

RETURN VALUE  True if we are active on any antenna, otherwise FALSE.

SIDE EFFECTS  None.

===========================================================================*/
extern boolean srch_rx_uses_any_chain( void );

/*===========================================================================

FUNCTION       SRCH_RX_SM_EXCHANGE_DEVICES

DESCRIPTION    This function exchanges the devices used by the
               DEMOD and ALT chains.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_sm_exchange_devices
(
 boolean fing_setup
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_RX_AGC

DESCRIPTION    This function gets the AGC for the specified receive chain

DEPENDENCIES   None.

RETURN VALUE   AGC for the specified receive chain.

SIDE EFFECTS   None.

===========================================================================*/
extern int8 srch_rx_get_rx_agc
(
  rx_sm_client_enum_t  chain_name   /* chain name */
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_HOME_RX_AGC

DESCRIPTION    This function gets the AGC for the specified receive chain
               when it is or was last tuned to the home band/channel

DEPENDENCIES   None.

RETURN VALUE   AGC for the demod chain when it is or was last tuned to the
               home band/channel.

SIDE EFFECTS   Stores the AGC for both the primary and secondary chains.

===========================================================================*/
extern int8 srch_rx_get_home_rx_agc(void);

/*===========================================================================

FUNCTION      SRCH_RX_GET_LAST_HOME_RX_AGC

DESCRIPTION   This function gets the last good AGC for the primary and
               secondary receive chains when they were last tuned to the
               home band/channel while in the online state.  If not active,
               -127 is returned instead.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Updates the parameters with the last RX AGC for the respective
               chains when they were last tuned to the home band/channel in
               0.334 dB units, centered at -63.248dBm on success, and -127
               on failure.

===========================================================================*/
extern void srch_rx_get_last_home_rx_agc( int8* primary, int8* secondary );

/*===========================================================================

FUNCTION       SRCH_RX_IS_TUNED_HOME

DESCRIPTION    This function determines whether we are tuned to the home band
               and channel.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the specified chain is already tuned to the given
               band class and channel.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_tuned_home
(
  rx_sm_client_enum_t chain_name    /* chain name */
);

/*===========================================================================

FUNCTION       SRCH_RX_IS_BAND_CHAN

DESCRIPTION    This function determines if the band class and channel for a
               specified chain match the passed in parameters.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the band class and channel match the parameters.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_band_chan
(
  rx_sm_client_enum_t     chain_name,    /* chain name */
  srch_rx_band_type       band,          /* band_class */
  srch_rx_chan_type       channel        /* channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_CHAIN_OWNED

DESCRIPTION    Returns the current chain owned by 1x

DEPENDENCIES   None.

RETURN VALUE   Chain Name

SIDE EFFECTS   None.

===========================================================================*/
extern srch_rx_chain_owned_type srch_rx_chain_owned ( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_TX_PWR_LIMIT

DESCRIPTION    This function returns the CDMA Tx power limit setting in 10 bit
               binary format.  The returned value is interpreted as follows:

               Returned value               0........1023
               Tx power limit          -52.25........+31dBm
               Tx power limit register   +511........-512

DEPENDENCIES   None.

RETURN VALUE   10 bit Tx power limit value in dBm12

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_rx_sm_get_tx_pwr_limit( void );

/*===========================================================================

FUNCTION      SRCH_RX_SM_GET_FILTERED_TX_PWR_DBM10

DESCRIPTION   This function returns Tx power reported by the FW Tx AGC filter.

DEPENDENCIES  None

RETURN VALUE  10 bit Filtered Tx power value in dBm10

SIDE EFFECTS  None

===========================================================================*/

extern int16 srch_rx_sm_get_filtered_tx_pwr_dbm10( void );

/*===========================================================================

FUNCTION       SRCH_RX_SM_GET_TX_AGC

DESCRIPTION    This function returns the CDMA Tx AGC setting in 10 bit
               binary format.

DEPENDENCIES   None.

RETURN VALUE   10 bit Tx AGC value in 1/10th dBm

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_rx_sm_get_tx_agc( void );

/*========================================================================

FUNCTION       SRCH_RX_GET_WAKEUP_CHAIN

DESCRIPTION    This function returns the TRM chain resource that is required
               based on the last RX AGC reading.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern trm_resource_enum_t srch_rx_get_wakeup_chain( void );

/*========================================================================

FUNCTION       SRCH_RX_GET_RESERVED_CHAIN

DESCRIPTION    This function returns the TRM chain that was last reserved.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern trm_resource_enum_t srch_rx_get_reserved_chain( void );

/*========================================================================

FUNCTION      SRCH_RX_SET_RESERVED_CHAIN

DESCRIPTION   This function sets the TRM chain for next request.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

========================================================================*/
extern void srch_rx_set_reserved_chain( trm_resource_enum_t resource );

/*========================================================================

FUNCTION       SRCH_RX_SET_RX_CHAIN_SEL_THRESH

DESCRIPTION    This function writes the RX chain selection thresholds that
               are read from NV.

DEPENDENCIES   Must call srch_rx_init prior to
               srch_rx_set_rx_chain_sel_thresh.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern void srch_rx_set_rx_chain_sel_thresh
(
  int16                 upper_thresh, /* Above, transition to any RX chain */
  int16                 lower_thresh  /* Below, transition to best RX chain */
);

/*========================================================================

FUNCTION      SRCH_RX_GET_WAKEUP_CHAIN

DESCRIPTION   This function returns the TRM chain resource that is required
               for acquisition based on chain sensitivity information.

DEPENDENCIES  None

RETURN VALUE  Wakeup chain name

SIDE EFFECTS  None

========================================================================*/
trm_resource_enum_t srch_rx_get_init_resource( void );

/*========================================================================

FUNCTION      SRCH_RX_GET_INIT_DEFAULT_RESOURCE

DESCRIPTION   This function returns the default TRM chain resource that is
               to be used for acquisition based on chain sensitivity
               information.

DEPENDENCIES  None

RETURN VALUE  Wakeup chain name

SIDE EFFECTS  None

========================================================================*/
trm_resource_enum_t srch_rx_get_init_default_resource( void );

/*========================================================================

FUNCTION       SRCH_RX_DET_WAKEUP_CHAIN

DESCRIPTION    This function determines the next wakeup chain based on
               the current RX AGC and the current demod chain.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern void srch_rx_det_wakeup_chain
(
  boolean going_to_sleep
);

/*========================================================================

FUNCTION      SRCH_RX_SET_WAKEUP_CHAIN

DESCRIPTION   This function sets the next wakeup chain with the value
              passed. Note that this overrides the srch_rx_det_wakeup_chain
              selection

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

========================================================================*/
extern void srch_rx_set_wakeup_chain
(
  trm_resource_enum_t chain
);

/*========================================================================

FUNCTION       SRCH_RX_GET_PRIMARY_RESOURCE

DESCRIPTION    Return the RXTX capable resource from TRM list.

DEPENDENCIES   None

RETURN VALUE   RXTX capable resource

SIDE EFFECTS   None

========================================================================*/
extern trm_resource_enum_t srch_rx_get_primary_resource( void );

/*===========================================================================

FUNCTION       SRCH_RX_SET_RX_CHAIN_TRANS

DESCRIPTION    This function writes the RX chain translation as set in NV.

DEPENDENCIES   Must call srch_rx_init prior to
               srch_rx_set_rx_chain_trans.

RETURN VALUE   None.

SIDE EFFECTS   Updates static structure with trm_resource_enum_t translation
               of TRM_RX_ANY.

===========================================================================*/
extern void srch_rx_set_rx_chain_trans
(
  srch_nv_dbg_mask_type    rx_chain_trans,
  trm_resource_enum_t      request_type
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_RX_CHAIN_TRANS

DESCRIPTION    This function reads the RX chain translation as set in NV.

DEPENDENCIES   Must call srch_rx_init prior to
               srch_rx_get_rx_chain_trans.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern trm_resource_enum_t srch_rx_get_rx_chain_trans
(
  trm_resource_enum_t   request_type
);

/*========================================================================

FUNCTION       SRCH_RX_DRX_IS_BAND_TUNABLE

DESCRIPTION    Returns if 1x's secondary RX path can be tuned to the given band.
               This API is normally used before acquiring the lock on the
               secondary RX antenna.

DEPENDENCIES   None

RETURN VALUE   TRUE if DRx can be tuned to the input "band"

SIDE EFFECTS   None

========================================================================*/
extern boolean srch_rx_drx_is_band_tunable
(
 srch_rx_band_type       band
);

/*========================================================================

FUNCTION       SRCH_RX_RESERVE_PRIMARY_FOR_NEXT_WAKEUP

DESCRIPTION    Sets a flag to indicate reserving of primary chain
               for next wakeup.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

========================================================================*/
extern void srch_rx_reserve_primary_for_next_wakeup( void );

/*===========================================================================

FUNCTION       SRCH_RX_BUILD_RF_SUB

DESCRIPTION    This function builds the rf sub packet.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_build_rf_sub
(
  srch_genlog_packet_id_type   id,       /* id of packet to commit sub */
  sub_commit_func_type         commit_fp /* function to call to commit the
                                            subpacket */
);

/*===========================================================================

FUNCTION      SRCH_RX_GET_RF_PREP_WARMUP_TIME

DESCRIPTION   This function returns the RF prep warmup overhead in us.

DEPENDENCIES  None.

RETURN VALUE  The RF prep warmup overhead in us.

SIDE EFFECTS  None.

===========================================================================*/
extern uint16 srch_rx_get_rf_prep_warmup_time( void );

/*===========================================================================

FUNCTION      SRCH_RX_GET_RF_EXEC_WARMUP_TIME

DESCRIPTION   This function returns the RF exec warmup overhead in us.

DEPENDENCIES  None.

RETURN VALUE  The RF exec warmup overhead in us.

SIDE EFFECTS  None.

===========================================================================*/
extern uint16 srch_rx_get_rf_exec_warmup_time( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_CDMA_TX_AGC

DESCRIPTION    Wrapper function for rf function rfm_1x_get_cdma_tx_agc

DEPENDENCIES   None.

RETURN VALUE   The current CDMA Tx AGC setting.

SIDE EFFECTS   None.

===========================================================================*/
extern uint16 srch_rx_get_cdma_tx_agc( void );

/*===========================================================================

FUNCTION       SRCH_RX_FREEZE_CAGC

DESCRIPTION    Wrapper function for rf function rf_freeze_cagc

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_freeze_cagc( void );

/*===========================================================================

FUNCTION       SRCH_RX_UNFREEZE_CAGC

DESCRIPTION    Wrapper function for rf function rf_unfreeze_cagc

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_unfreeze_cagc( void );

/*===========================================================================

FUNCTION       SRCH_RX_SLEEP_TX

DESCRIPTION    Wrapper function for rf function rfm_tx_stop

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_sleep_tx( rx_sm_client_enum_t rx_client );

/*===========================================================================

FUNCTION       SRCH_RX_WAKEUP_TX

DESCRIPTION    Wrapper function for rf function rfm_tx_start

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_wakeup_tx( rx_sm_client_enum_t rx_client );

/*===========================================================================

FUNCTION       SRCH_RX_GET_CDMA_TURNAROUND_CONST

DESCRIPTION    This function returns the CDMA turnaround constant value for current RF band.

DEPENDENCIES   None

RETURN VALUE   The CDMA turn around constant in dBm for the current RF band.

SIDE EFFECTS   None

===========================================================================*/
extern int32 srch_rx_get_cdma_turnaround_const( void );

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
/*===========================================================================

FUNCTION       SRCH_RX_MEAS_COMMON_SCRIPT_BUILD_SCRIPTS

DESCRIPTION    This function calls rf API to set up and build scripts for IRAT
               pilot measurements

DEPENDENCIES   LTE has to call rfm_meas_common_enter() first

RETURN VALUE   rfm_meas_result_type from RF pilot measurement script

SIDE EFFECTS   None.

===========================================================================*/
extern rfm_meas_result_type srch_rx_meas_common_script_build_scripts
(
  /* RF header (common info for L21x meas) */
  rfm_meas_header_type                              *p_rf_header,

  /* source tech parameters */
  rfm_meas_lte_setup_and_build_scripts_param_type   *p_source_param,

  /* target tech parameters */
  rfm_meas_1x_setup_and_build_scripts_param_type    *p_target_param
);
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*==========================================================================
FUNCTION      SRCH_RX_GET_CLIENT_ASID_MAPPING 

DESCRIPTION  Gets the GSM client ASID mapping.
             TRM_GSM1 will always map to Multi-mode subscription.
    
DEPENDENCIES  None

INPUTS        TRM Client id

RETURN VALUE  ASID corresponding to a TRM client, currently this is being
              handled only for GSM client.

SIDE EFFECTS  None
==========================================================================*/
extern sys_modem_as_id_e_type srch_rx_get_client_asid_mapping
(
  /* target tech id */
  trm_client_enum_t        client_id
);

/*===========================================================================
FUNCTION       SRCH_RX_UPDATE_TRM_FOR_1X2X_QTA_OPERATION

DESCRIPTION    This function calls trm_set_client_state() to Start/End QTA.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
extern void srch_rx_update_trm_for_1x2x_qta_operation
(
  trm_client_enum_t target_tech,
  boolean           start_qta
);

/*===========================================================================
FUNCTION       SRCH_RX_CHECK_AND_UPDATE_RF_FOR_1X2X_QTA_START

DESCRIPTION    This is wrapper function for RF API rfm_1x_prep_qta()

DEPENDENCIES   None.

RETURN VALUE   TRUE if call to rfm_1x_prep_qta() is successful
               FALSE otherwise

SIDE EFFECTS   None.
===========================================================================*/
extern boolean srch_rx_check_and_update_rf_for_1x2x_qta_start( void );

/*===========================================================================
FUNCTION       SRCH_RX_UPDATE_RF_FOR_1X2X_QTA_END

DESCRIPTION    This is wrapper function for RF API rfm_1x_end_qta()

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
extern void srch_rx_update_rf_for_1x2x_qta_end( void );

/*===========================================================================
FUNCTION       SRCH_RX_UPDATE_RF_FOR_1X2X_QTA_OPERATION

DESCRIPTION    This function calls RF APIs to Prep/End QTA.

DEPENDENCIES   None.

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
extern void srch_rx_update_rf_for_1x2x_qta_operation
(
  gl1_hw_cm_rf_info_t *qta_param,
  boolean             start_qta
);

/*===========================================================================
FUNCTION       SRCH_RX_UPDATE_MEAS_PARAMS_FOR_1X2X_QTA

DESCRIPTION    This function updates rfm_meas_common_param_type structure with
               1x realted params.

DEPENDENCIES   1x must own the RX & TX chains.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
extern void srch_rx_update_meas_params_for_1x2x_qta
(
  gl1_hw_cm_rf_info_t *rf_meas_param
);

/*===========================================================================

FUNCTION       SRCH_RX_SM_TUNE_AND_WAIT

DESCRIPTION    This function tunes the specified chain to the band and channel
               requested. It also tunes successfully when diversity is turned
               on.  It will inform the user via a callback once the tune has
               completed.

               The currently owned receiver will be automatically enabled
               if necessary.  The sample server will be started in continuous
               mode.  The sample ram will be flushed if the demod chain is
               tuned.

               This is a synchronous call that will return after the RF is
               tuned and ready.

DEPENDENCIES   The chain_name must be associated with a RF chain owned
               by 1x.

RETURN VALUE   True if successfull.

SIDE EFFECTS   srch_rx is updated.

===========================================================================*/
extern boolean srch_rx_sm_tune_and_wait
(
  srch_rx_band_type                    band,              /* band class */
  srch_rx_chan_type                    chan               /* channel number */
);

/*===========================================================================

FUNCTION       SRCH_RX_INIT_STM_CMD

DESCRIPTION    This function initializes the command buffer that is to be
               passed to RX_SM

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_init_stm_cmd
(
  rx_sm_req_t                          *rx_cmd_payload
);

/*===========================================================================

FUNCTION       SRCH_RX_GET_WAKEUP_INFO

DESCRIPTION    Copies the last wakeup values into the parameter.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_get_wakeup_info( srch_rx_wakeup_info_type* dest );

/*===========================================================================

FUNCTION       SRCH_RX_UPDATE_WAKEUP_INFO

DESCRIPTION    Updates the wakeup struct with the current values

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   Wakeup struct is modified.

===========================================================================*/
extern void srch_rx_update_wakeup_info( void );

/*===========================================================================

FUNCTION       SRCH_RX_UPDATE_ACQ_BAND_INFO

DESCRIPTION    This function updates band information passed in the
               Acquisition command

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   srch_rx structure gets modified

===========================================================================*/
extern void srch_rx_update_acq_band_info
(
  srch_rx_band_type band
);

/*===========================================================================

FUNCTION       SRCH_RX_SET_SECONDARY_CHAIN_TRANS

DESCRIPTION    This function updates the chain translation to return
               SECONDARY chain[DRx].

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_set_secondary_chain_trans( void );

/*===========================================================================

FUNCTION       SRCH_RX_BACKUP_CHAIN_TRANS

DESCRIPTION    This function backs-up the chain translation value into
               srch_rx.chain_trans_bkup.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_backup_chain_trans( void );

/*===========================================================================

FUNCTION       SRCH_RX_RESTORE_CHAIN_TRANS

DESCRIPTION    This function restores the chain translation value from
               srch_rx.chain_trans_bkup.

DEPENDENCIES   srch_rx_backup_chain_trans() must have been called earlier.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_restore_chain_trans( void );

/*===========================================================================

FUNCTION       SRCHRDIV_DISABLE_SLEEP_SECOND_CHAIN_TEST

DESCRIPTION    Returns whether SLEEP should be disabled during
               FTM second chain test mode.

DEPENDENCIES   None.

RETURN VALUE   TRUE if the sleep should be disabled in FTM second chain test.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srchrdiv_disable_sleep_second_chain_test( void );

/*===========================================================================

FUNCTION       SRCH_RX_CFS_TUNE_WAIT

DESCRIPTION    Wrapper function for rf macro RF_CFS_TUNE_WAIT

DEPENDENCIES   None.

RETURN VALUE   RF CFS tune time.

SIDE EFFECTS   None.

===========================================================================*/
extern uint32 srch_rx_cfs_tune_wait( void );

/*===========================================================================

FUNCTION       SRCH_RX_VBATT_READ

DESCRIPTION    Wrapper function for vbatt_read.

DEPENDENCIES   None.

RETURN VALUE   A scaled version of the batter level where empty is VBATT_SCALED_MIN,
               and full is VBATT_SCALE.

SIDE EFFECTS   None.

===========================================================================*/
extern byte srch_rx_vbatt_read( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_RXLM_BUFFER_HANDLE

DESCRIPTION    This function returns RXLM handle for the given chain.

DEPENDENCIES   None.

RETURN VALUE   RxLM handle for the input chain.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_get_rxlm_buffer_handle
(
  rx_sm_client_enum_t        rx_client,     /* RX State Machine */
  lm_handle_type             *rxlm_buf      /* RxLM handle */
);

/*===========================================================================

FUNCTION       SRCH_RX_SET_ANT_CARRIER_AND_DEMOD

DESCRIPTION    This function sets the fingers to the appropriate antenna,
               carrier, and demod path based on which RF device(s) are
               currently owned.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch_rx_set_ant_carrier_and_demod( void );

/**************************************************************************
  ZZ_2_2 defines
**************************************************************************/

typedef enum
{
  SRCH_DYN_RID_QPCH_ERASURE,
  SRCH_DYN_RID_QPCH_PI_DETECTED,
  SRCH_DYN_RID_CHAN_EST_FAIL,
  SRCH_DYN_RID_REACQ_FAIL,
  SRCH_DYN_RID_PAGE_DECODE_FAIL,
  SRCH_DYN_RID_RX0_AGC_FAIL,
  SRCH_DYN_RID_RX1_AGC_FAIL,
  SRCH_DYN_RID_RX0_ECIO_FAIL,
  SRCH_DYN_RID_RX1_ECIO_FAIL,
  SRCH_DYN_RID_DESIRED_CHAIN_FAIL,

  SRCH_DYN_RID_QPCH_PI_NOT_DETECTED,
  SRCH_DYN_RID_CHAN_EST_PASS,
  SRCH_DYN_RID_REACQ_SUCCESS,
  SRCH_DYN_RID_PAGE_DECODE_SUCCESS,
  SRCH_DYN_RID_RX0_AGC_PASS,
  SRCH_DYN_RID_RX1_AGC_PASS,
  SRCH_DYN_RID_RX0_ECIO_PASS,
  SRCH_DYN_RID_RX1_ECIO_PASS,
  SRCH_DYN_RID_DESIRED_CHAIN_PASS,

  SRCH_DYN_RID_RUDE_WAKEUP,
  SRCH_DYN_RID_NON_QPCH_TL,
  SRCH_DYN_RID_SEL_DIV_EN,
  SRCH_DYN_RID_OTF_DIV_EN,
  SRCH_DYN_RID_ASDIV_EN,
  SRCH_DYN_RID_PAGE_DECODE_FAIL_HYBRID,
  SRCH_DYN_RID_RX0_AGC_FAIL_HYBRID,
  SRCH_DYN_RID_RX1_AGC_FAIL_HYBRID,
  SRCH_DYN_RID_RX0_ECIO_FAIL_HYBRID,
  SRCH_DYN_RID_RX1_ECIO_FAIL_HYBRID,
  SRCH_DYN_RID_PAGE_DECODE_SUCCESS_SIMUP,
  SRCH_DYN_RID_RX0_AGC_PASS_SIMUP,
  SRCH_DYN_RID_RX1_AGC_PASS_SIMUP,
  SRCH_DYN_RID_RX0_ECIO_PASS_SIMUP,
  SRCH_DYN_RID_RX1_ECIO_PASS_SIMUP

} srch_dyn_rid_bad_cond_enum_type;

/* PCH MER Reset Reasons */
typedef enum
{
  SRCH_PCH_REAS_TUNE         = 0x1,          /* Tune to new band/chan  */
  SRCH_PCH_REAS_HO           = 0x2,          /* Idle handoff performed */
  SRCH_PCH_REAS_SLEEP        = 0x3,          /* Idle sleep entered     */
  SRCH_PCH_REAS_STATE_CHANGE = 0x4,          /* State change from idle */
  SRCH_PCH_REAS_MAX
} srch_pch_mer_reset_type;

/*===========================================================================

FUNCTION       SRCH_RX_INIT_ZZ2_2

DESCRIPTION    Initialize the ZZ2_2 mechanism.

               - resets the data structure
               - reloads the thresholds
               - reloads the selection params
               - resets the desired chain

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_init_zz2_2 ( void );

/*===========================================================================

FUNCTION       SRCH_RX_IS_ZZ2_2_ENABLED

DESCRIPTION    Return whether or not 1x ZZ2_2 should be
               used at this time.

DEPENDENCIES   None.

RETURN VALUE   TRUE if we use it this time, FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_is_zz2_2_enabled ( void );

/*===========================================================================

FUNCTION       SRCH_RX_DYN_ZZ2_2_ENABLED

DESCRIPTION    Return if dynamic ZZ2_2 is enabled based on the conditions
               mask.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_rx_dyn_zz2_2_enabled( void );

/*===========================================================================

FUNCTION       SRCH_RX_PRINT_ZZ2_2_PARMS

DESCRIPTION    Print the current thresholds and selection parameters used by
               ZZ2_2.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_print_zz2_2_parms ( void );

/*===========================================================================

FUNCTION       SRCH_RX_SET_ZZ2_2_COND

DESCRIPTION    This allows the caller to specify a specific good/bad
               condition that has been met.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_set_zz2_2_cond
(
  srch_dyn_rid_bad_cond_enum_type condition,
  boolean                     going_to_sleep
);

/*===========================================================================

FUNCTION       SRCH_RX_CHECK_ZZ2_2_CHAN_EST

DESCRIPTION    Check the channel estimate against the ZZ2_2 thresholds.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The corresponding condition flag will be set if the thresholds
               are exceeded.

===========================================================================*/
extern void srch_rx_check_zz2_2_chan_est
(
  uint32 threshold,
  uint32 chan_est
);

/*===========================================================================

FUNCTION       SRCH_RX_CHECK_DYN_RID_ECIO

DESCRIPTION    Check the Ec/Io against the dynamic idle div thresholds.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The corresponding condition flag will be set if the thresholds
               are exceeded.

===========================================================================*/
extern void srch_rx_check_dyn_rid_ecio
(
  boolean going_to_sleep
);

/*===========================================================================

FUNCTION       SRCH_RX_SET_DYN_RID_PCH_DECODE_COND

DESCRIPTION    The function sets/stores the pch decode result.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_set_dyn_rid_pch_decode_cond
(
  boolean good_frame
);

/*===========================================================================

FUNCTION       SRCH_RX_CHECK_ZZ2_2_RXAGC

DESCRIPTION    Check the Rx AGC of a specified chain against the ZZ2_2 thresholds.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   The corresponding condition flag will be set if the thresholds
               are exceeded.

===========================================================================*/
extern void srch_rx_check_zz2_2_rxagc( void );

/*========================================================================

FUNCTION       SRCH_RX_ZZ2_2_RESET

DESCRIPTION    This function resets ZZ2_2 and initializes the desired
               device as specified by the caller.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

========================================================================*/
extern void srch_rx_zz2_2_reset
(
  rx_sm_device_enum_t desired_device  /* initial desired device */
);

/*===========================================================================

     D Y N A M I C    R E C E I V E R    I N I T    D I V E R S I T Y

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH_RX_GET_RIN_ENABLE
DESCRIPTION    This function returns RIN enable mask
DEPENDENCIES   None

RETURN VALUE   uint8
SIDE EFFECTS   None

===========================================================================*/
extern uint8 srch_rx_get_rin_enable( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_RIN_PRIMARY/SEC_AGC_CK
DESCRIPTION    This function returns the threshold for the Primary/Secondar
               RxAGC check
DEPENDENCIES   None

RETURN VALUE   AGC value
SIDE EFFECTS   None

===========================================================================*/
extern int16 srch_rx_get_rin_primary_agc_ck( void );
extern int16 srch_rx_get_rin_sec_agc_ck( void );

/*===========================================================================

FUNCTION       SRCH_RX_SET/GET_RIN_AGC
DESCRIPTION    This function records the AGC values read during the last check
               A passed in value of "0" indicates "not reported"
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_set_rin_agc
(
  int16 pri_agc,
  int16 sec_agc
);
extern void srch_rx_get_rin_agc
(
  int16 *pri_agc,
  int16 *sec_agc
);

/*===========================================================================

FUNCTION       SRCH_RX_INIT_RIN
DESCRIPTION    This function initializes acquisition diversity
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_init_rin( void );

/*===========================================================================

FUNCTION       SRCH_RX_RIN_DIV_CTRL
DESCRIPTION    This function sets initial states diversity modes
               Thresholds may be passed as "0" to use default thresholds
DEPENDENCIES   None

RETURN VALUE   TRUE if successfully applied
SIDE EFFECTS   None

===========================================================================*/
extern boolean  srch_rx_rin_div_ctrl
(
  uint8                        en_mask,          /* enable mask */
  int16                        th0,              /* th0 threshold in dB */
  int16                        th1               /* th1 threshold in dB */
);

/*===========================================================================

FUNCTION       SRCH_RX_VOTE_ON_DUAL_RECEIVE_MODE

DESCRIPTION    This function will vote to enable/disable DR based on parameter.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_vote_on_dual_receive_mode
(
  boolean enable_dr
);

/*===========================================================================

FUNCTION       SRCH_RX_CHANGE_DUAL_RECEIVE_MODE

DESCRIPTION    This function determines if we are currently in hybrid fallback
               mode and enables/disables DR accordingly.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_change_dual_receive_mode
(
  boolean going_to_sleep     /* set when func called just before sleep */
);

/*===========================================================================

FUNCTION       SRCH_RX_RESET_HYBRID_FALLBACK_MODE

DESCRIPTION    This function resets the hybrid fallback related variables.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_rx_reset_hybrid_fallback_mode( void );

/*===========================================================================

FUNCTION       SRCH_RX_GET_HYBRID_FALLBACK_STATUS

DESCRIPTION    This function gets the hybrid fallback variable status.

DEPENDENCIES   None

RETURN VALUE   Value of hybrid_fallback flag. TRUE/FALSE

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_rx_get_hybrid_fallback_status( void );

/*===========================================================================

FUNCTION      SRCH_RX_BUILD_IDLE_DIV_STATS_SUB

DESCRIPTION   This function builds the PCH MER sub packet.

DEPENDENCIES  None

RETURN VALUE  boolean, TRUE is packet contains valid information

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_build_idle_div_stats_sub
(
  srch_genlog_packet_id_type id,            /* id of packet to commit sub */
  sub_commit_func_type       commit_fp      /* function to call to commit the
                                               subpacket */
);

/*===========================================================================

FUNCTION       SRCH_RX_RESET_DYN_RID_PCH_DECODE_COND

DESCRIPTION    The function resets the pch decode paramters.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_rx_reset_dyn_rid_pch_decode_cond
(
  srch_pch_mer_reset_type  reset_reason
);

/*===========================================================================

FUNCTION      SRCH_RX_BUILD_PCH_MER_SUB

DESCRIPTION   This function builds the PCH MER sub packet.

DEPENDENCIES  None

RETURN VALUE  boolean, TRUE if packet contains valid information

SIDE EFFECTS  None

===========================================================================*/
extern boolean srch_rx_build_pch_mer_sub
(
  srch_genlog_packet_id_type id,            /* id of packet to commit sub */
  sub_commit_func_type       commit_fp      /* function to call to commit the
                                               subpacket */
);
#endif /* SRCH_RX_H */

