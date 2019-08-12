#ifndef __MCPM_API_H__
#define __MCPM_API_H__

/*=========================================================================

           M O D E M   C L O C K   A N D   P O W E R   M A N A G E R

                E X T E R N A L   H E A D E R   F I L E


  GENERAL DESCRIPTION
    This file contains the external interface functions and definitions for
    Modem Clock and Power Manager (MCPM).

  EXTERNALIZED FUNCTIONS
    MCPM_Config_Modem

  INITIALIZATION AND SEQUENCING REQUIREMENTS
    Invoke the MCPM_Init function to initialize the MCPM.

        Copyright (c) 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.


==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mpower/api/mcpm_api.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/16/15   mh      Addition of MSS split rail related changes 
08/20/15   ne      add support for SVS_PLUS and NOM_PLUS corners
08/10/15   mh      Added MCPM_Config_Modem_Try() public API. 
10/17/14   mh      Remove WCDMA EDRX request enums
10/16/14   cl      Adding support for Q6 345.6MHz and 370MHz
10/14/14   mh      Fixed merge issues
09/04/14   sc      Added support for CMCC power optimization feature.
08/11/14   ss      Map LTE data to LTE FDD data
07/17/14   ss      Add MCVS parameters for Scalar and Vector Q6 processors
06/19/14   cl      Add API to support W-L1 team query for Q6 speed
05/04/14   bd      GPIO Protection API Support
05/05/14   cl      First Jolokia official release branch off Bolt tip
04/25/14   ls      FW_VPE MCVS client to support LTE NLIC.
03/21/14   sr      G + G DSDS multi-sim support.
02/13/14   ls      LTE VoLTE and CDRX support.
01/17/14   ls      Support for LTE's MCPM request for TDD and FDD DATA on Bolt.
09/27/13   sr      Corrected formatting of some comments.
09/03/13   ls      Added a new clk_mdm freq enum.
07/31/13   sr      Removed GSM apis
07/26/13   sr      Added enum MCPM_GSM_DL_NONEV.
07/08/13   ls      Added more APIs (request type, DL/UL/BW data rate).
02/11/13   vs      Ported from Dime release branch.


==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/


#include "customer.h"
#include "comdef.h"
#include "msg.h"
#include "npa.h"
#include "icbarb.h"
#include "rfm_mode_types.h"

/*=========================================================================
      Macros for the MCPM clk/pll dump
==========================================================================*/

#define MCPM_DUMP_CLK 0x00000001
#define MCPM_DUMP_BLK 0x00000002
#define MCPM_DUMP_PWRUP 0x00000004


#define MCPM_DO_NOP_REQ            MCPM_DO_PARMS_UPDATE_REQ
#define MCPM_DO_START_DATA_TX_REQ  MCPM_DO_START_DATA_REQ
#define MCPM_DO_STOP_DATA_TX_REQ   MCPM_DO_STOP_DATA_REQ

/*
 * Bit 0: "RECEIVER_CONFIG" field is updated
 * Bit 1: "NEIGHBOR_MEAS" field is updated
 * Bit 2: "DL_DATA_RATE" field is updated
 * Bit 3: "UL_DATA_RATE" field is updated
 * Bit 4: "RF_BANDWIDTH" field is updated
 */
#define MCPM_NO_UPDATE                 0x000
#define MCPM_RECEIVER_CONFIG_UPDATE    0x001
#define MCPM_NEIGHBOR_MEAS_UPDATE      0x002
#define MCPM_DL_DATA_RATE_UPDATE       0x004
#define MCPM_UL_DATA_RATE_UPDATE       0x008
#define MCPM_RF_BANDWIDTH_UPDATE       0x010
#define MCPM_WKTIME_RF_UPDATE          0x020
#define MCPM_WKTIME_RSRC_UPDATE        0x040
#define MCPM_FCW_DCO_RESET_UPDATE      0x080
#define MCPM_MODEM_PERF_MODE_UPDATE    0x100
#define MCPM_MCVS_SCALE_UPDATE         0x200

/*
 * MCVS Param update info
 */
#define MCPM_MCVS_VPE_UPDATE           0x001
#define MCPM_MCVS_CLKBUS_UPDATE        0x002
/* Requests for Scalar Q6 clock change */
#define MCPM_MCVS_SCALAR_PROC_CLK_UPDATE 0x004
#define MCPM_MCVS_Q6_CLK_UPDATE          MCPM_MCVS_SCALAR_PROC_CLK_UPDATE
/* Requests for Vector Q6 (Silver) clock change */
#define MCPM_MCVS_VECTOR_PROC_CLK_UPDATE 0x008
#define MCPM_MCVS_FW_VPE_UPDATE          0x010

/*
 * RECEIVER_CONFIG
 * Bit 0: RXD (ON/OFF)
 * Bit 1: EQ (ON/OFF)
 * Bit 2: QICE (ON/OFF)
 * Bit 3: W-MIMO (ON/OFF)
 */

#define MCPM_RECEIVER_CONFIG_RXD       0x01  /* 1 rxd ON, 0 rxd OFF */
#define MCPM_RECEIVER_CONFIG_EQ        0x02  /* 1 EQ ON, 0 EQ OFF */
#define MCPM_RECEIVER_CONFIG_QICE      0x04  /* 1 QICE ON, 0 QICE OFF */
#define MCPM_RECEIVER_CONFIG_W_MIMO    0x08  /* 1 W MIMO CALL enabled */


#define MCPM_RECEIVER_CONFIG_RXD_SHIFT    0
#define MCPM_RECEIVER_CONFIG_EQ_SHIFT     1
#define MCPM_RECEIVER_CONFIG_QICE_SHIFT   2
#define MCPM_RECEIVER_CONFIG_W_MIMO_SHIFT 3

#define MCPM_MODEM_WCDMA_PERF_NONE      0x0
#define MCPM_MODEM_WCDMA_HIGH_PERF_MODE 0x1 /* OFFLINE clock to 144 for Nikel */


/*=========================================================================
      Typedefs
==========================================================================*/

/*
 * mcpm_request_type
 *
 * Requests to MCPM
 *
 * START_REQ should be sent when a technology is activated.
 *
 * STOP_REQ should be sent when a technology is deactivated.
 *
 * WAKE_UP_REQ should be sent when coming out sleep to decode a page indicator
 *
 * GO_TO_SLEEP_REQ should be sent when going into sleep.
 *
 * GO_TO_PSEUDO_SLEEP should be sent by 1X tech if RTC needs to be kept ON during sleep
 *
 * IDLE_REQ should be sent when the UE moves from page indicator decode
 * to setup paging channel. 1X technology also calls this to move from
 * 1X VOICE and 1X DATA calls.
 *
 * VOICE_START_REQ should be sent on start of a voice call.
 *
 * VOICE_STOP_REQ should be sent on end of a voice call.
 *
 * DATA_START_REQ should be sent on start of a data call
 *
 * DATA_STOP_REQ should be sent on end of a data call
 *
 */

typedef enum
{
  /* 1X Requests */
  MCPM_1X_START_REQ              = 0,
  MCPM_1X_STOP_REQ               = 1,
  MCPM_1X_WAKE_UP_REQ            = 2,
  MCPM_1X_GO_TO_SLEEP_REQ        = 3,
  MCPM_1X_GO_TO_PSEUDO_SLEEP_REQ = 4,
  MCPM_1X_IDLE_REQ               = 5,
  MCPM_1X_VOICE_REQ              = 6,
  MCPM_1X_DATA_REQ               = 7,
  MCPM_1X_PARMS_UPDATE_REQ       = 8,

  /* DO Requests */
  MCPM_DO_START_REQ              = 9,
  MCPM_DO_STOP_REQ               = 10,
  MCPM_DO_WAKE_UP_REQ            = 11,
  MCPM_DO_GO_TO_SLEEP_REQ        = 12,
  MCPM_DO_IDLE_REQ               = 13,
  MCPM_DO_START_DATA_REQ         = 14,
  MCPM_DO_STOP_DATA_REQ          = 15,
  MCPM_DO_PARMS_UPDATE_REQ       = 16,

  /* GERAN Requests */
  MCPM_GERAN_START_REQ           = 17,
  MCPM_GERAN_STOP_REQ            = 18,
  MCPM_GERAN_WAKE_UP_REQ         = 19,
  MCPM_GERAN_GO_TO_SLEEP_REQ     = 20,
  MCPM_GERAN_IDLE_REQ            = 21,
  MCPM_GERAN_VOICE_START_REQ     = 22,
  MCPM_GERAN_VOICE_STOP_REQ      = 23,
  MCPM_GERAN_DATA_START_REQ      = 24,
  MCPM_GERAN_DATA_STOP_REQ       = 25,
  MCPM_GERAN_PARMS_UPDATE_REQ    = 26,

  /* GERAN1 Requests */
  MCPM_GERAN1_START_REQ                               = 27,
  MCPM_GERAN1_STOP_REQ                                = 28,
  MCPM_GERAN1_WAKE_UP_REQ                             = 29,
  MCPM_GERAN1_GO_TO_SLEEP_REQ                         = 30,
  MCPM_GERAN1_IDLE_REQ                                = 31,
  MCPM_GERAN1_VOICE_START_REQ                         = 32,
  MCPM_GERAN1_VOICE_STOP_REQ                          = 33,
  MCPM_GERAN1_DATA_START_REQ                          = 34,
  MCPM_GERAN1_DATA_STOP_REQ                           = 35,
  MCPM_GERAN1_PARMS_UPDATE_REQ                        = 36,

  /* GERAN2 Requests */
  MCPM_GERAN2_START_REQ                               = 37,
  MCPM_GERAN2_STOP_REQ                                = 38,
  MCPM_GERAN2_WAKE_UP_REQ                             = 39,
  MCPM_GERAN2_GO_TO_SLEEP_REQ                         = 40,
  MCPM_GERAN2_IDLE_REQ                                = 41,
  MCPM_GERAN2_VOICE_START_REQ                         = 42,
  MCPM_GERAN2_VOICE_STOP_REQ                          = 43,
  MCPM_GERAN2_DATA_START_REQ                          = 44,
  MCPM_GERAN2_DATA_STOP_REQ                           = 45,
  MCPM_GERAN2_PARMS_UPDATE_REQ                        = 46,

  /* LTE Requests */
  MCPM_LTE_START_REQ                                  = 47,
  MCPM_LTE_STOP_REQ                                   = 48,
  MCPM_LTE_INIT_REQ                                   = 49,
  MCPM_LTE_ACQ_REQ                                    = 50,
  MCPM_LTE_WAKE_UP_REQ                                = 51,
  MCPM_LTE_GO_TO_SLEEP_REQ                            = 52,
  MCPM_LTE_IDLE_REQ                                   = 53,
  MCPM_LTE_TDD_DATA_START_REQ                         = 54,
  MCPM_LTE_FDD_DATA_START_REQ                         = 55,
  MCPM_LTE_TDD_VOLTE_DATA_START_REQ                   = 56,
  MCPM_LTE_FDD_VOLTE_DATA_START_REQ                   = 57,
  /* To maintain backwards compatibility map data start to fdd data start - going forward LTE Data
   * has either FDD or TDD only state so the Data_Start is redundanct
   */
  MCPM_LTE_DATA_START_REQ                             = MCPM_LTE_FDD_DATA_START_REQ,
  MCPM_LTE_GO_TO_LIGHT_SLEEP_REQ                      = 58,
  MCPM_LTE_GO_TO_LONG_LIGHT_SLEEP_REQ                 = 59,
  MCPM_LTE_GO_TO_LIGHT_SLEEP_NO_MODEM_FREEZE_REQ      = 60,
  MCPM_LTE_DATA_STOP_REQ                              = 61,
  MCPM_LTE_PARMS_UPDATE_REQ                           = 62,

  /* TDSCDMA Requests */
  MCPM_TDSCDMA_START_REQ                              = 63,
  MCPM_TDSCDMA_STOP_REQ                               = 64,
  MCPM_TDSCDMA_ACQ_REQ                                = 65,
  MCPM_TDSCDMA_WAKE_UP_REQ                            = 66,
  MCPM_TDSCDMA_GO_TO_SLEEP_REQ                        = 67,
  MCPM_TDSCDMA_IDLE_REQ                               = 68,
  MCPM_TDSCDMA_VOICE_START_REQ                        = 69,
  MCPM_TDSCDMA_VOICE_STOP_REQ                         = 70,
  MCPM_TDSCDMA_DATA_START_REQ                         = 71,
  MCPM_TDSCDMA_DATA_STOP_REQ                          = 72,
  MCPM_TDSCDMA_PARMS_UPDATE_REQ                       = 73,

  /* WCDMA Requests */
  MCPM_WCDMA_START_REQ                                = 74,
  MCPM_WCDMA_STOP_REQ                                 = 75,
  MCPM_WCDMA_WAKE_UP_REQ                              = 76,
  MCPM_WCDMA_GO_TO_SLEEP_REQ                          = 77,
  MCPM_WCDMA_IDLE_REQ                                 = 78,
  MCPM_WCDMA_CDRX_GO_TO_LIGHT_SLEEP_REQ               = 79,
  MCPM_WCDMA_VOICE_START_REQ                          = 80,
  MCPM_WCDMA_VOICE_STOP_REQ                           = 81,
  MCPM_WCDMA_DATA_START_REQ                           = 82,
  MCPM_WCDMA_DATA_STOP_REQ                            = 83,
  MCPM_WCDMA_PARMS_UPDATE_REQ                         = 84,

  /* GPS Requests */
  MCPM_GPS_STOP_REQ                                   = 85,
  MCPM_GPS_ACQ_REQ                                    = 86,
  MCPM_GPS_NON_DPO_REQ                                = 87,
  MCPM_GPS_DPO_ON_REQ                                 = 88,
  MCPM_GPS_PARMS_UPDATE_REQ                           = 89,

/* RF Requests */
  MCPM_RF_START_REQ                                   = 90,
  MCPM_RF_STOP_REQ                                    = 91,

  /* A2 Requests */
  MCPM_A2_START_REQ                                   = 92,
  MCPM_A2_STOP_REQ                                    = 93,

  /* GSM CIPHERING Requests */
  MCPM_GSM_CIPHERING_START_REQ                        = 94,
  MCPM_GSM_CIPHERING_STOP_REQ                         = 95,

    /* GSM1 CIPHERING Requests */
  MCPM_GSM_CIPHERING1_START_REQ                       = 96,
  MCPM_GSM_CIPHERING1_STOP_REQ                        = 97,

      /* GSM2 CIPHERING Requests */
  MCPM_GSM_CIPHERING2_START_REQ                       = 98,
  MCPM_GSM_CIPHERING2_STOP_REQ                        = 99,

  MCPM_TECH_MAX_REQ
} mcpm_request_type;


/*----------------------------------------------------------------------------
  MODEM block register restore callback function prototype
----------------------------------------------------------------------------*/

typedef void (*mcpm_block_restore_callback_type) (void);


/*----------------------------------------------------------------------------
  L1 techs register sleep timeline callback function prototype
  Default 1X and DO will extend timeline
  TRUE in param implies Optimized timeline
  FALSE in param implies Extended timeline
----------------------------------------------------------------------------*/

typedef void (*mcpm_npa_slp_tmln_callback_type) (boolean);


/*----------------------------------------------------------------------------
  MODEM clock speed change callback function prototype
----------------------------------------------------------------------------*/

typedef void (*mcpm_clk_speed_change_callback_type) (void);


/*----------------------------------------------------------------------------
  A2 notify callback function prototype
----------------------------------------------------------------------------*/

typedef void (*mcpm_a2_notify_callback_type) (void);




/*
 * mcpm_nbr_meas_type
 *
 * The neighbor measurement type
 */

typedef enum
{
  /* 1X Neighbors */
  MCPM_1X2L_MEAS_START,
  MCPM_1X2L_MEAS_STOP,

  /* DO Neighbors */
  MCPM_DO2L_MEAS_START,
  MCPM_DO2L_MEAS_STOP,

  /* GERAN and GERAN1 Neighbors */
  MCPM_G2L_MEAS_START,
  MCPM_G2L_MEAS_STOP,
  MCPM_G2W_MEAS_START,
  MCPM_G2W_MEAS_STOP,
  MCPM_G2TDS_MEAS_START,
  MCPM_G2TDS_MEAS_STOP,

  /* LTE Neighbors */
  MCPM_L2G_MEAS_START,
  MCPM_L2G_MEAS_STOP,
  MCPM_L2W_MEAS_START,
  MCPM_L2W_MEAS_STOP,
  MCPM_L21XDO_MEAS_START,
  MCPM_L21XDO_MEAS_STOP,
  MCPM_L2TDSCDMA_MEAS_START,
  MCPM_L2TDSCDMA_MEAS_STOP,

  /* TDSCDMA Neighbors */
  MCPM_TDSCDMA2G_MEAS_START,
  MCPM_TDSCDMA2G_MEAS_STOP,
  MCPM_TDSCDMA2L_MEAS_START,
  MCPM_TDSCDMA2L_MEAS_STOP,

  /* WCDMA Neighbors */
  MCPM_W2G_MEAS_START,
  MCPM_W2G_MEAS_STOP,
  MCPM_W2L_MEAS_START,
  MCPM_W2L_MEAS_STOP

} mcpm_nbr_meas_type;


/*
 * mcpm_rf_bandwidth_type
 *
 * The configured RF bandwidth
 */

typedef enum
{
  /* DO */
  MCPM_DO,
  MCPM_MCDO,

  /* LTE */
  MCPM_LTE_1p4MHZ,
  MCPM_LTE_3MHZ,
  MCPM_LTE_5MHZ,
  MCPM_LTE_10MHZ,
  MCPM_LTE_15MHZ,
  MCPM_LTE_20MHZ,
  MCPM_LTE_5MHZ_5MHZ,
  MCPM_LTE_10MHZ_10MHZ,
  MCPM_LTE_20MHZ_20MHZ,

  /* WCDMA */
  MCPM_WCDMA,
  MCPM_DC_WCDMA,
  MCPM_3C_WCDMA,
  MCPM_4C_WCDMA,
  MCPM_BW_NA,
  MCPM_NUM_BW

} mcpm_rf_bandwidth_type;


/*
 * MCPM_dl_datarate_type
 *
 * Downlink data rate.
 */

typedef enum
{

  /* 1X */

  /* DO */
  MCPM_DO_DL_2P4_MBPS,
  MCPM_DO_DL_3P1_MBPS,
  MCPM_DO_DL_9P3_MBPS,
  MCPM_DO_DL_14P7_MBPS,

  /* GERAN and GERAN1*/
  MCPM_GSM_DL_EV,
  MCPM_GSM_DL_NONEV,

  /* GPS */

  /* LTE */
  MCPM_LTE_DL_RATE_L10,
  MCPM_LTE_DL_RATE_L36,
  MCPM_LTE_DL_RATE_L50,
  MCPM_LTE_DL_RATE_L73,
  MCPM_LTE_DL_RATE_L100,
  MCPM_LTE_DL_RATE_L110,
  MCPM_LTE_DL_RATE_L146,
  MCPM_LTE_DL_RATE_L150,
  MCPM_LTE_DL_RATE_L300,

  /* TDSCDMA */

  /* WCDMA */
  MCPM_W_DL_R99DATA,
  MCPM_W_DL_3p6_MBPS,
  MCPM_W_DL_7p2_MBPS,
  MCPM_W_DL_14p4_MBPS,
  MCPM_W_DL_21_MBPS,
  MCPM_W_DL_28_MBPS,
  MCPM_W_DL_42_MBPS,
  MCPM_W_DL_63_MBPS,
  MCPM_W_DL_84_MBPS,
  MCPM_DL_RATE_NA,
  MCPM_NUM_DL_RATES

} MCPM_dl_datarate_type;


/*
 * mcpm_ul_datarate_type
 *
 * uplink datarate
 */

typedef enum
{
  /* 1X */

  /* DO */
  MCPM_DO_UL_153KBPS,
  MCPM_DO_UL_1P8MBPS,
  MCPM_DO_UL_5P4MBPS,

  /* GERAN and GERAN1*/
  MCPM_GSM_UL_EV,

  /* GPS */

  /* LTE */
  MCPM_LTE_UL_RATE_L5,
  MCPM_LTE_UL_RATE_L11,
  MCPM_LTE_UL_RATE_L13,
  MCPM_LTE_UL_RATE_L23,
  MCPM_LTE_UL_RATE_L25,
  MCPM_LTE_UL_RATE_L40,
  MCPM_LTE_UL_RATE_L50,
  MCPM_LTE_UL_RATE_L100,
  MCPM_LTE_UL_RATE_L150,

  /* TDSCDMA */

  /* WCDMA */
  MCPM_W_UL_R99DATA,
  MCPM_W_UL_1P46_MBPS,
  MCPM_W_UL_2MBPS,
  MCPM_W_UL_2P93_MBPS,
  MCPM_W_UL_5P76_MBPS,
  MCPM_W_UL_11P5_MBPS,
  MCPM_W_UL_22_MBPS,
  MCPM_UL_RATE_NA,
  MCPM_NUM_UL_RATES

} mcpm_ul_datarate_type;


/*
 * mcvs_request_type
 *
 * mcvs request type
 */

typedef enum
{
  MCVS_REQUEST_NONE = 0,
  MCVS_PRE_SCALE_REQUEST,  /* to prescale Cx/Mx based on clock requests to come in future */
  MCVS_FULL_REQUEST,       /* to update clocks - clk bus/VPE/Q6 - as requested possibly even causing RPM interaction */
  MCVS_FAST_CAP_REQUEST,   /* to update clocks as far as possible but return quickly - No RPM interaction*/
  MCVS_RELEASE_REQUEST     /* To release any request made in past */
} mcvs_request_type;


/*
 * mcpm_mcvsrequest_parms_type
 *
 *  Structure containing the MCPM MCVS Request and Request Attributes.
 *  mcvs_req_type                 - Indicates if request is pre-scale (0) - slow (1) or fast(2), other fileds reserved for future
 *  mcvs_update_info              - Mask that indicates which of the attrbutes are updated.
 *
 *  modem_vpe_KHz                 - Provides the MP update Info
 *  modem_clk_bus_KHz             - Indicates the Clk bus update
 *  q6_clock_KHz                  - Indicates Q6 clock update
 */

typedef struct
{
  mcvs_request_type      mcvs_req_type;
  uint32                 mcvs_update_info;
  uint32                 modem_vpe_KHz;
  uint32                 modem_clk_bus_KHz;
  uint32                 q6_clock_KHz;
} mcpm_mcvsrequest_parms_type;


/*
 * mcpm_request_parms_type
 *
 * Structure containing the MCPM Request and Request Attributes.
 *
 *  update_info        - Indicates which of the attrbutes are updated.
 *  receiver_config    - Provides the receiver config information.
 *  neighbor_meas      - Indiactes the type of neighbor measurement
 *  dl_datarate        - Provides the downlink data rate.
 *  ul_datarate        - Provides the uplink data rate.
 *  rf_bandwidth       - Provides the RF bandwidth.
 *  wkup_time_for_rf   - Time when RF resource needed (in usec)
 *  wkup_time          - Time when bus or any other resource request needed.(in usec)
 *  modem_perf_mode    - Modem performance mode
 *  mcvs_request - L1s can send an MCVS request along with other parameter updates
 *                 MCVS request could be to pre-scale voltage rails based on clock frequency, or make immediate clk bus/Q6/VPE updates
 */
typedef struct
{

  uint32                        update_info;
  uint32                        receiver_config;
  mcpm_nbr_meas_type            neighbor_meas;
  MCPM_dl_datarate_type         dl_datarate;
  mcpm_ul_datarate_type         ul_datarate;
  mcpm_rf_bandwidth_type        rf_bandwidth;
  uint64                        wkup_time_for_rf;
  uint64                        wkup_time;
  uint32                        modem_perf_mode;
  mcpm_mcvsrequest_parms_type   mcvs_request;
} mcpm_request_parms_type;

/*
 * mcpm_modem_clk_type
 *
 * modem clock type.
 */

typedef enum
{

  /* MCPM Q6 Clk */
  MCPM_CLK_Q6,

  /* Clk Modem AXI */
  MCPM_CLK_MODEM_AXI,

  /* Clk bus */
  MCPM_CLK_BUS,

  /* MCPM TDEC clk */
  MCPM_CLK_TDEC,

  /* Clk bus internal to MODEM */
  MCPM_CLK_MODEM_MSSBUS,
  
  /* MCPM total number of clocks */
  MCPM_NUM_CLK

} mcpm_modem_clk_type;


/*
 * mcpm_clk_callback_trigger_type
 *
 * clock callback trigger type.
 */

typedef enum
{

  /* Trigger call back after clock change in MCPM */
  MCPM_CB_Trigger_AFTER_CLK_Change,

  /* Trigger call back before clock change in MCPM */
  MCPM_CB_Trigger_BEFORE_CLK_Change,

} mcpm_clk_callback_trigger_type;


/*
 * mcpm_tech_type
 *
 * MCPM supported technologies.
 */

typedef enum
{

  /* MCPM 1X technolgy definition */
  MCPM_1X_TECH,

  /* MCPM GERAN technolgy definition */
  MCPM_GERAN_TECH,

 /* MCPM GERAN1 technolgy definition */
  MCPM_GERAN1_TECH,

   /* MCPM GERAN2 technolgy definition */
  MCPM_GERAN2_TECH,

  /* MCPM DO technolgy definition */
  MCPM_DO_TECH,

  /* MCPM WCDMA technolgy definition */
  MCPM_WCDMA_TECH,

  /* MCPM LTE technolgy definition */
  MCPM_LTE_TECH,

  /* MCPM TDSCDMA technolgy definition */
  MCPM_TDSCDMA_TECH,

  /* Number of RAT technologies */
  MCPM_NUM_RAT_TECH,

  /* MCPM GPS technology definition */
  MCPM_GPS_TECH = MCPM_NUM_RAT_TECH,

  /* MCPM RF technology definition */
  MCPM_RF_TECH,

  /* MCPM GSM CIPHERING definition */
  MCPM_GSM_CIPHERING_TECH,

  /* MCPM GSM1 CIPHERING definition */
  MCPM_GSM1_CIPHERING_TECH,

    /* MCPM GSM2 CIPHERING definition */
  MCPM_GSM2_CIPHERING_TECH,

  /* MCPM A2 technolgy definition */
  MCPM_A2_TECH,

  /* number fo techs used for boundary checks  */
  MCPM_NUM_TECH
} mcpm_tech_type;


/*
 * MCPM_RCM_TRIGGER
 *
 * Supported RCM Triggers
 */

typedef enum
{
  /* Primary RCM Debug Toggle */
  RCM_TRIGGER_1,
  /* Secondary RCM Debug Toggle */
  RCM_TRIGGER_2,
  /* Tertiary RCM Debug Toggle */
  RCM_TRIGGER_3,
  /* RCM Debug toggle for L1 breakdowns */
  RCM_TRIGGER_4 = RCM_TRIGGER_3,
  /* Max number of RCM triggers */
  RCM_TRIGGER_MAX
} MCPM_RCM_TRIGGER;


/*----------------------------------------------------------------------------
  A2 resources need be setup as fast as possible. Pulling out of A2 from the
  normal path of muxing.
----------------------------------------------------------------------------*/
#define MCPMDRV_NUM_MUX_TECH  (MCPM_A2_TECH)

/*----------------------------------------------------------------------------
  Typedef of modem blocks.
----------------------------------------------------------------------------*/

typedef enum
{

  /* MCPM EDGE block */
  MCPM_EDGE_BLOCK,

  /* MCPM EDGE1 block */
  MCPM_EDGE_G1_BLOCK,

    /* MCPM EDGE2 block */
  MCPM_EDGE_G2_BLOCK,

  /* MCPM TX block */
  MCPM_TX_BLOCK,

  /* MCPM Turbo decoder block */
  MCPM_TDEC_BLOCK,

  /* MCPM DemBack block */
  MCPM_DEMBACK_BLOCK,

  /* MCPM VPE block */
  MCPM_VPE_BLOCK,

  /* MCPM MEMSS block */
  MCPM_MEMSS_BLOCK,

  /* MCPM DEMSS block */
  MCPM_DEMSS_BLOCK,

  /* MCPM RXFE block */
  MCPM_RXFE_BLOCK,

  /* MCPM RXFE block */
  MCPM_STMR_BLOCK,

  /* MCPM total number of blocks */
  MCPM_NUM_BLOCK

} mcpm_modem_block_type;

/* Return values to check for by RF/other SW modules using MCPM_NPA */
typedef enum
{
  MCPMNPA_STAT_NULL,
  MCPMNPA_STAT_OPT_SUCCESS = 1,
  MCPMNPA_STAT_OPT_FAILURE = 2,
  MCPMNPA_STAT_OPT_IMM_REQ_SENT = 3,
  MCPMNPA_STAT_OPT_ENABLE = 4
} MCPMNPA_StatusType;


/*
 * MCPM_ClkSpeedType
 *
 * MCPM clock speed type which controls the Q6 Clk speed for Jolokia.
 *
 */

typedef enum
{
  MCPM_CLK_NULL,
  MCPM_CLK_19_2M,
  MCPM_CLK_Q6_115M,
  MCPM_CLK_Q6_144M,
  MCPM_CLK_Q6_230M,
  MCPM_CLK_Q6_288M,
  MCPM_CLK_Q6_345M,
  MCPM_CLK_Q6_370M,
  MCPM_CLK_Q6_384M,
  MCPM_CLK_Q6_480M,
  MCPM_CLK_Q6_576M,
  MCPM_CLK_Q6_615M,
  MCPM_CLK_Q6_691M,
  MCPM_NUM_CLKSPEED
} MCPM_ClkSpeedType;

/*----------------------------------------------------------------------------
  Typedef of core cpu vdd vote.
----------------------------------------------------------------------------*/

typedef enum
{
MCPM_MODEM_PLUS_Q6_PC_VOTE,      /*vote = 0*/
MCPM_MODEM_PLUS_Q6_NOT_PC_VOTE,   /*vote = 1*/
MCPM_Q6_ONLY_PC_VOTE            /*MODEM FREEZE - vote =2*/
} MCPM_core_cpuvdd_vote_type;

#define MCPM_BIMC_BOOST_MASK              0x001
#define MCPM_CPU_BOOST_MASK               0x002

typedef void (*mcpm_deboost_callback)(void);
typedef void (*mcpm_cpuboost_callback)(void);

/*==========================================================================
               FUNCTION DECLARATIONS FOR MODULE
==========================================================================*/

/* =========================================================================
 **  Function : MCPM_Service_Boost_Req
 ** =========================================================================*/
/**
  This function is the main API called by L1 techs to request for boost/deboost..


  @param client         [in] Current technology mcpm_tech.
  @param boostRequest         [in] uses 2 LSB bits as flags.
                                  MCPM_BIMC_BOOST_MASK(0x1) and MCPM_CPU_BOOST_MASK (0x2)
  @param callbackFunctionPtr  [in] function pointer to call, when deboost is complete.

  @return
  None.

  @dependencies
  TBD.

 */
void  MCPM_Service_Boost_Req
(
   mcpm_tech_type client,
   uint32  boostRequest,
   mcpm_deboost_callback callbackFunctionPtr
);

uint32  MCPM_Service_Boost_Request
(
   mcpm_tech_type client,
   uint32  boostRequest,
   mcpm_deboost_callback callbackFunctionPtr,
   mcpm_cpuboost_callback CpuBoostCbfcnPtr,
   uint32 l1boostTime
);


/*==========================================================================

  FUNCTION      MCPM_CONFIG_MODEM

  DESCRIPTION   This function is the main interface function.
                Modem SW should call this function when the modem mode is
                changed. MCPM expects the call made for the following mode
                changes.
                - Activate and deactivate the Portocal stack
                - Start and stop of voice call
                - Start and stop of data call
                - Change in RF bandwidth
                - Start and Stop of nbr measurement
                - Rxd/qice/EQ start and stop.

  PARAMETERS    req      - Modem SW mode change request.
                parms    - Attributes associated with req.

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  Clocks will be enabled and disabled during the test.

==========================================================================*/

void MCPM_Config_Modem(mcpm_request_type req, mcpm_request_parms_type *parms);

/*==========================================================================

  FUNCTION      MCPM_Config_Modem_Try

  DESCRIPTION   This function will first try to get MCPM top lock.  If the
                lock is already acquired then this function will return FALSE
                immediately without processing the request, otherwise it will
                proceed to get the lock, process the request by calling
                MCPM_CONFIG_MODEM and return TRUE when finished.

  PARAMETERS    req      - Modem SW mode change request.
                parms    - Attributes associated with this request.

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  TRUE     - Successfully acquired the lock and finished the request.
                FALSE    - MCPM top lock has been acquired at the time of requst, the
                           request is NOT processed.

  SIDE EFFECTS  If returned TRUE, system configuration is setup as expected.
                If returned FALSE, system configuration is not setup as expected.

==========================================================================*/

boolean MCPM_Config_Modem_Try(mcpm_request_type req, mcpm_request_parms_type *parms);


/*==========================================================================

  FUNCTION      MCPM_MCVSConfig_Modem

  DESCRIPTION   This function is the interface function for MCVS requests.
                Modem SW should call this function when the modem mode mcvs update is required.
                MCPM expects the call made for the following mode
                changes.
                - VPE Clock update
                - CLK BUS Clock update
                - Q6 Clock update

  PARAMETERS    tech      - Modem SW tech type.
                parms    - Attributes associated with req.

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  Clocks will be Updated during this without change in Config. If the request attribute type in parms is
                set to type immediate - then no RPM requests will be sent out to scale the voltages. If voltages have
                not been pre-scaled, then frequencies will be capped.If request type is type slow - this call will
                block if (mx,cx) voltage scaling is required and may take longer to finish. This call needs to be
                accompanied by a call to Release the MCVS Request

==========================================================================*/

void MCPM_MCVSConfig_Modem(mcpm_tech_type tech, mcpm_mcvsrequest_parms_type *parms);


/*==========================================================================

  FUNCTION      MCPM_MCVSConfig_Release
  DESCRIPTION   This function is the interface function for releasing the MCVS requests .
                Modem SW should call this function when the modem mode mcvs update request has been made.
                MCPM expects the call made for the following mode
                changes.
                - VPE Clock update
                - CLK BUS Clock update
                - Q6 Clock update

  PARAMETERS    tech      - Modem SW tech type.


  DEPENDENCIES  MCPM must be initialized. A prior call to MCPM_MCVSConfig_Modem from same tech type should have been made.


  RETURN VALUE  NONE.

  SIDE EFFECTS : This will bring down the clock configs to pre-mcvs level.

==========================================================================*/

void MCPM_MCVSConfig_Release(mcpm_tech_type tech);


/*============================================================================

FUNCTION MCPM_SET_BLOCK_RESTORE_CALLBACK

DESCRIPTION
  This function sets the callback function to call when modem block is bought
  out of power gating (power collapse)

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPM_Set_Block_Restore_Callback
(
  /* The tech that is registering the block */
  mcpm_tech_type tech,

  /* Modem block  */
  mcpm_modem_block_type block,

  /* register restore callback */
  mcpm_block_restore_callback_type restore_cb
);

/*============================================================================

FUNCTION MCPM_Set_Clock_Change_Callback

DESCRIPTION
  This function sets the callback function to call when modem clock state
  changes based on the trigger type.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPM_Set_Clock_Change_Callback
(
  /* The tech that is registering the call-back */
  mcpm_tech_type tech,

  /* Modem clk  */
  mcpm_modem_clk_type clk_type,

  /* register clock change callback */
  mcpm_clk_speed_change_callback_type clk_speed_change_cb
);

/*============================================================================

FUNCTION MCPMDRV_CFG_Dump

DESCRIPTION
  This function dumps all the real time MCPM clk/registers values to F3 for
  now.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPMDRV_CFG_Dump( uint32 mcpm_dump_val);


/*==========================================================================

  FUNCTION      MCPM_Send_RF_Req

  DESCRIPTION   This function can be called by RF drivers to send the RF requests
                to MCPM.

  PARAMETERS    mode                - Tech/RF mode that is making this request.
                npa_id              - PAM Id that MCPM will use in NPA request
                npa_handle          - NPA handle that MCPM should use to
                pass this request to PMIC
                time_rsrc_needed    - Time when this resource is needed (not used for now)
                sleep_req           - Bool to indicate if this is a sleep or awake request

  DEPENDENCIES  MCPM must be initialized.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/
MCPMNPA_StatusType MCPM_Send_RF_Req
(
  /* Tech that is making this request. */
  rfm_mode_enum_type mode,
  /* NPA Id sent by RF driver */
  uint8 npa_id,
  /* NPA handle that is sent by RF driver */
  npa_client_handle npa_handle,
  /* Time when this resource is needed. */
  uint64 time_rsrc_needed,
  /* Sleep or Awake request. */
  boolean sleep_req
);


/*============================================================================

FUNCTION MCPM_NPA_SET_SLEEP_TIMELINE_CALLBACK

DESCRIPTION
  This function sets the callback function to call when sleep timeline has to
  changed from optimized to extended and vice versa

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPM_NPA_Set_Sleep_Timeline_Callback
(
  /* The tech that is registering the block */
  mcpm_tech_type tech,

  /* register sleep timeline callback */
  mcpm_npa_slp_tmln_callback_type slp_tmln_cb
);


/*==========================================================================
FUNCTION MCPMDRV_Get_Modem_Offline_Clock

DESCRIPTION:
    MCPM API to returen offline clock setting to the caller

DEPENDENCIES
  None.

RETURN VALUE
  clock speed type (enum).

SIDE EFFECTS
  None.

============================================================================*/

MCPM_ClkSpeedType MCPMDRV_Get_Modem_Offline_Clock(mcpm_tech_type eTech);


/*===========================================================================
FUNCTION     MCPM_Get_PwrOpt_Level

DESCRIPTION  This function is the interface function for L1 SW to get the
             level of power optimization from Modem SW.

DEPENDENCIES MCPM must be initialized.

PARAMETERS   eTech  - mcpm_tech_type.

RETURN VALUE The level of power optimization for that tech.

SIDE EFFECTS None.
===========================================================================*/

uint8 MCPM_Get_PwrOpt_Level(mcpm_tech_type eTech);

/*==========================================================================
FUNCTION MCPMDRV_Get_Modem_Q6_Clock

DESCRIPTION:
    MCPM API to return q6 clock setting to the caller.
    This is not the final Q6 clock speed in the NPA node, but it's only the
    Q6 clock speed that's voted by a particular tech indicated by eTech.
    Q6 clock speed is extracted from MCA value.

DEPENDENCIES
  None.

RETURN VALUE
  clock speed type (enum).

SIDE EFFECTS
  None.

============================================================================*/
MCPM_ClkSpeedType MCPMDRV_Get_Modem_Q6_Clock(mcpm_tech_type eTech);



/*==========================================================================

  FUNCTION      mcpm_gpio_profile_pwr_strobe_toggle

  DESCRIPTION   Used to toggle power strobe at profile points. Should not be used
                directly. Instead should be used through macro MCPM_GPIO_TOGGLE.

  PARAMETERS    strobe      - The RCM power strobe to toggle

  DEPENDENCIES  MCPM must be initialized, MCPM GPIO feature must be enabled

  RETURN VALUE  None.

  SIDE EFFECTS  Toggles a specific dedicated debug strobe on RCM HW

==========================================================================*/

extern void MCPM_Gpio_Profile_Pwr_Strobe_Toggle(MCPM_RCM_TRIGGER strobe);

/*
 * Macro for toggling a specific RCM debug strobe
 */

#define MCPM_GPIO_TOGGLE(RCM_STROBE) MCPM_Gpio_Profile_Pwr_Strobe_Toggle(RCM_STROBE);

/*============================================================================

FUNCTION MCPM_Set_A2_Tech_Callback

DESCRIPTION
  This function sets the callback function to notify A2 when only A2 is active

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void MCPM_Set_A2_Tech_Callback
(
  /* register clock change callback */
  mcpm_a2_notify_callback_type a2_notify_cb
);


#endif /* __MCPM_API_H__ */

