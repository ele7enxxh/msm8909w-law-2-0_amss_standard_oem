#ifndef __MCPM_API_H__
#define __MCPM_API_H__

/*=========================================================================

           M O D E M   C L O C K   A N D   P O W E R   M A N A G E R 
               
                E X T E R N A L   H E A D E R   F I L E


  GENERAL DESCRIPTION
    This file contains the external interface functions and definitions for
    Modem Clock and Power Manager (MCPM).
  
  EXTERNALIZED FUNCTIONS
    MCPM_Init
    MCPM_Config_Modem
  
  INITIALIZATION AND SEQUENCING REQUIREMENTS
    Invoke the MCPM_Init function to initialize the MCPM.
  
        Copyright (c) 2010 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.


==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/stubs/api/mcpm_api.h#1 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
02/02/12   yz       Added support of High Offline clk Cfg
12/09/11   yz       Type define for offline clk speed
09/29/11   ps       Added a new emum for MCPM_Send_RF_Req API
09/22/11   ps       Fixed compilation issue after reverting the a version
                    Removed extern for header declarations
09/22/11   ps       MCPM support for RF-SW to allow direct NPA calls. 
                    This is needed to choose beween 
                    scheduled NPA requests or direct requests
08/21/11   ps       Add support for GSTMR-MCPM mutex,
                    GSM clk src always at PL11 Disable DCO in sleep and
                    Disable SW PC for LTE
08/10/11   mg       Added MCPM_FCW_DCO_RESET_UPDATE update field
06/20/11   ps       Added callback registration mechanism in SHDR/Hybrid modes
06/09/11   yz       Added MCPMDRV_enable_all_pwron_api so the tech team can call it to enable 
05/31/11   mg       Added MCPM_TDSCDMA_ACQ_REQ request for TDS 
05/16/11   ps       Added UIM support for RPM optimization
05/16/11   ps       Add support for UIM 
04/28/11   ps       Added 
                    1. Call flows to get RF settings/Bus reg
                    2. Added calls to TRM
                    3. Added logic to calculate minimum wakeup time
                    4. Added functions to set RFM_TECH_INFO
04/26/11   ps       Adding initial support for RPM optimization - 
                    minimal API needs for RF/GPS/L1tech to integrate their software
                    1. Modified "params" for MCPM_Config_Modem
                    2. Added MCPM_Send_Bus_Request API
4/26/11    mg       Add MCPM_1X_GO_TO_PSEUDO_SLEEP_REQ request
11/17/10   mg       Add define MCPM_NO_UPDATE for no update
11/15/10   mg       Initial Revision.


==========================================================================*/ 


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/


#include "customer.h"
#include "comdef.h"
#include "msg.h"
//#include "npa.h"
//#include "rfm_mode_types.h"
//#include "icbarb.h"

/*=========================================================================
      Macros for the MCPM clk/pll dump
==========================================================================*/
#define MCPM_DUMP_CLK	0x00000001
#define MCPM_DUMP_BLK	0x00000002
#define MCPM_DUMP_PWRUP	0x00000004

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

  /* LTE Requests */
  MCPM_LTE_START_REQ             = 27,
  MCPM_LTE_STOP_REQ              = 28,
  MCPM_LTE_WAKE_UP_REQ           = 29,
  MCPM_LTE_GO_TO_SLEEP_REQ       = 30,
  MCPM_LTE_IDLE_REQ              = 31,
  MCPM_LTE_DATA_START_REQ        = 32,
  MCPM_LTE_DATA_STOP_REQ         = 33,
  MCPM_LTE_PARMS_UPDATE_REQ      = 34,

  /* TDSCDMA Requests */
  MCPM_TDSCDMA_START_REQ         = 35,
  MCPM_TDSCDMA_STOP_REQ          = 36,
  MCPM_TDSCDMA_ACQ_REQ           = 37,
  MCPM_TDSCDMA_WAKE_UP_REQ       = 38,
  MCPM_TDSCDMA_GO_TO_SLEEP_REQ   = 39,  
  MCPM_TDSCDMA_IDLE_REQ          = 40,
  MCPM_TDSCDMA_VOICE_START_REQ   = 41,
  MCPM_TDSCDMA_VOICE_STOP_REQ    = 42,
  MCPM_TDSCDMA_DATA_START_REQ    = 43,
  MCPM_TDSCDMA_DATA_STOP_REQ     = 44,
  MCPM_TDSCDMA_PARMS_UPDATE_REQ  = 45,
  
  /* WCDMA Requests */
  MCPM_WCDMA_START_REQ           = 46,
  MCPM_WCDMA_STOP_REQ            = 47,
  MCPM_WCDMA_WAKE_UP_REQ         = 48,
  MCPM_WCDMA_GO_TO_SLEEP_REQ     = 49,
  MCPM_WCDMA_IDLE_REQ            = 50,
  MCPM_WCDMA_VOICE_START_REQ     = 51,
  MCPM_WCDMA_VOICE_STOP_REQ      = 52,
  MCPM_WCDMA_DATA_START_REQ      = 53,
  MCPM_WCDMA_DATA_STOP_REQ       = 54,
  MCPM_WCDMA_PARMS_UPDATE_REQ    = 55,

  /* GPS Requests */ 
  MCPM_GPS_STOP_REQ              = 56,
  MCPM_GPS_ACQ_REQ               = 57,
  MCPM_GPS_NON_DPO_REQ           = 58,
  MCPM_GPS_DPO_ON_REQ            = 59,
  MCPM_GPS_DPO_IDLE              = 60,
  MCPM_GPS_PARMS_UPDATE_REQ      = 61,
  
  /* A2 Requests */
  MCPM_A2_START_REQ              = 62,
  MCPM_A2_STOP_REQ               = 63,

  /* UIM Requests */
  MCPM_UIM_START_REQ             = 64,
  MCPM_UIM_STOP_REQ              = 65,
  /* GPS Requests */
  MCPM_GPS_START_REQ = 66,
   /* GPS Requests */


  MCPM_GPS_WAKE_UP_REQ,
  MCPM_GPS_GO_TO_PSEUDO_SLEEP_REQ,
  MCPM_TECH_MAX_REQ
} mcpm_request_type;

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
#define MCPM_MCVS_PRE_SCALE_UPDATE     0x200
#define MCPM_MCVS_MP_UPDATE            0x001
#define MCPM_MCVS_CLKBUS_UPDATE        0x002
#define MCPM_MCVS_Q6_CLK_UPDATE        0x004

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
  
  /* GERAN Neighbors */
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
  MCPM_LTE_10MHZ_10MHZ,
  
  /* WCDMA */
  MCPM_WCDMA,
  MCPM_DC_WCDMA,
  MCPM_3C_WCDMA,
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
    
  /* GERAN */
  MCPM_GSM_DL_EV,
   
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
 
  /* GERAN */
  MCPM_GSM_UL_EV,

  /* GPS */

  /* LTE */
  MCPM_LTE_UL_RATE_L5,
  MCPM_LTE_UL_RATE_L13,
  MCPM_LTE_UL_RATE_L23,
  MCPM_LTE_UL_RATE_L25,
  MCPM_LTE_UL_RATE_L40,
  MCPM_LTE_UL_RATE_L50,
  
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

typedef enum
{
  MCPM_Q6_OFF   = 0,
  MCPM_Q6_115_2 = 115200,
  MCPM_Q6_144   = 144000,
  MCPM_Q6_230_4 = 230400,
  MCPM_Q6_288   = 288000,
  MCPM_Q6_384   = 384000,
  MCPM_Q6_576   = 576000, 
  MCPM_Q6_691_2 = 691200
} mcpm_q6_clkchange;

typedef enum
{
  MCPM_MP_OFF = 0,
  MCPM_MP_144,
  MCPM_MP_192,
  MCPM_MP_249_6,
  MCPM_MP_288
} mcpm_mp_clkchange;
typedef enum
{
  MCPM_CLKBUS_OFF  = 0,
  MCPM_CLKBUS_72,
  MCPM_CLKBUS_144
} mcpm_clkbus_clkchange;
 
#define MCPM_MODEM_WCDMA_PERF_NONE      0x0
#define MCPM_MODEM_WCDMA_HIGH_PERF_MODE 0x1 /* OFFLINE clock to 144 for Nikel */
 
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
 */
typedef struct
{

  uint32                  update_info;
  uint32                  receiver_config;
  mcpm_nbr_meas_type      neighbor_meas;
  MCPM_dl_datarate_type   dl_datarate;
  mcpm_ul_datarate_type   ul_datarate;
  mcpm_rf_bandwidth_type  rf_bandwidth;
  uint32                  wkup_time_for_rf;
  uint32                  wkup_time;
  uint32                  modem_perf_mode;
} mcpm_request_parms_type;


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
  Typedef of modem clocks.
----------------------------------------------------------------------------*/

typedef enum
{

  /* MCPM Offline clk */
  MCPM_CLK_MODEM,

  /* MCPM TDEC clk */
  MCPM_CLK_TDEC,

  /* MCPM total number of clocks */
  MCPM_NUM_CLK
}
mcpm_modem_clk_type;

/*----------------------------------------------------------------------------
  Typedef of MCPM how to trigger call-back.
----------------------------------------------------------------------------*/

typedef enum
{

  /* Trigger call back after clock change in MCPM */
  MCPM_CB_Trigger_AFTER_CLK_Change,

  /* Trigger call back before clock change in MCPM */
  MCPM_CB_Trigger_BEFORE_CLK_Change,
}
mcpm_clk_callback_trigger_type;

/*----------------------------------------------------------------------------
  Typedef of MCPM supported Technologies
----------------------------------------------------------------------------*/

typedef enum
{

  /* MCPM 1X technolgy definition */
  MCPM_1X_TECH,

  /* MCPM GERAN technolgy definition */
  MCPM_GERAN_TECH,

  /* MCPM DO technolgy definition */
  MCPM_DO_TECH,

  /* MCPM WCDMA technolgy definition */
  MCPM_WCDMA_TECH,

  /* MCPM LTE technolgy definition */
  MCPM_LTE_TECH,

  /* MCPM TDSCDMA technolgy definition */
  MCPM_TDSCDMA_TECH,

  /* MCPM GPS technolgy definition */
  MCPM_GPS_TECH,

  /* MCPM A2 technolgy definition */
  MCPM_A2_TECH,

  /* MCPM UIM technolgy definition */
  MCPM_UIM_TECH,

  /* number fo techs used for boundary checks  */
  MCPM_NUM_TECH
}
mcpm_tech_type;


/*----------------------------------------------------------------------------
  Typedef of modem blocks.
----------------------------------------------------------------------------*/

typedef enum
{

  /* MCPM EDGE block */
  MCPM_EDGE_BLOCK,

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
}
mcpm_modem_block_type;

/* Return values to check for by RF/other SW modules using MCPM_NPA */
typedef enum
{
  MCPMNPA_STAT_NULL,
  MCPMNPA_STAT_OPT_SUCCESS = 1,
  MCPMNPA_STAT_OPT_FAILURE = 2,
  MCPMNPA_STAT_OPT_IMM_REQ_SENT = 3,
  MCPMNPA_STAT_OPT_ENABLE = 4
} MCPMNPA_StatusType;

/*----------------------------------------------------------------------------
  Typedef of modem offline clk speeds.
----------------------------------------------------------------------------*/

typedef enum
{
  MCPM_CLK_NULL,
  MCPM_CLK_OFF,  
  MCPM_CLK_19_2M,
  MCPM_CLK_48M,
  MCPM_CLK_57_6M,
  MCPM_CLK_72M,
  MCPM_CLK_76_8M,
  MCPM_CLK_115_2M,
  MCPM_CLK_144M,
  MCPM_NUM_CLKSPEED
} MCPM_ClkSpeedType;

/*==========================================================================
               FUNCTION DECLARATIONS FOR MODULE
==========================================================================*/


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

extern void MCPM_Config_Modem(mcpm_request_type req, mcpm_request_parms_type *parms);

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

/*----------------------------------------------------------------------------
  MODEM clock speed change callback function prototype
----------------------------------------------------------------------------*/

typedef void (*mcpm_clk_speed_change_callback_type) (mcpm_modem_clk_type, mcpm_clk_callback_trigger_type);

/*============================================================================

FUNCTION MCPM_Set_Clock_Change_Callback

DESCRIPTION   See mcpm_api.h

============================================================================*/

void MCPM_Set_Clock_Change_Callback
(
  /* The tech that is registering the call-back */
  mcpm_tech_type tech,

  /* Modem clk  */
  mcpm_modem_clk_type clk_type,

  /* register clock change callback */
  mcpm_clk_speed_change_callback_type clk_speed_change_cb,

  /* register clock change callback trigger type */
  mcpm_clk_callback_trigger_type clk_cb_trigger_point
);

/*==========================================================================

  FUNCTION      MCPM_INIT

  DESCRIPTION   intialisation function  

==========================================================================*/
void MCPM_Init( void);
/*==========================================================================
FUNCTION  MCPMDRV_LTE_POWERUP_debug

DESCRIPTION: 
  	The MCPM configuration API to suppor the bring-up effort.
	@params: prgm_seq 1 : we are upgrading from SVS to non-SVS mode or 
				0: we are downgrading from non-SVS mode to SVS mode 
	
==========================================================================*/
void MCPMDRV_LTE_POWERUP_debug(int prgm_seq);

/*==========================================================================
FUNCTION  MCPMDRV_enable_all_pwron_api

DESCRIPTION: 
  	The extern API to trigger MCPM to call into the debug API to config clk/pwr .
	@params: None
	
==========================================================================*/

void MCPMDRV_enable_all_pwron_api(void);

/*==========================================================================
FUNCTION  MCPMDRV_CFG_Dump

DESCRIPTION: 
  	The MCPM configuration API to dump all real time MCPM clk/registers
	
==========================================================================*/
void MCPMDRV_CFG_Dump( uint32 mcpm_dump_val);





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
FUNCTION  MCPMDRV_is_clk_on_api

DESCRIPTION: 
  	The extern API to be used by GSTMR to check GSM CLK On before processing ISR
	@params: None
	
==========================================================================*/
boolean MCPMDRV_ifclk_on_get_gstmr_mutex(void);

/*==========================================================================
FUNCTION  MCPMDRV_release_gstmr_mutex

DESCRIPTION: 
  	The extern API to be used by GSTMR to release MCPM-GSTMR mutex
	@params: None
	
==========================================================================*/
void MCPMDRV_release_gstmr_mutex(void);

/*==========================================================================
FUNCTION  MCPMDRV_Get_Modem_Offline_Clock

DESCRIPTION: 
    MCPM API to returen offline clock setting to the caller
  
==========================================================================*/
MCPM_ClkSpeedType MCPMDRV_Get_Modem_Offline_Clock(int param_input);

#endif /* __MCPM_API_H__ */

