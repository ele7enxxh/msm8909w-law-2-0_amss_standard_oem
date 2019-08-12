#ifndef TDS_L1_API_H
#define TDS_L1_API_H


/*===========================================================================
                      L1 External APIs

DESCRIPTION

  This module contains protoypes of APIs that TD-SCDMA L1 provides to external modules



Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_l1_api.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/02/14   cliu    Add API tdsl1_api_get_tds_band
10/10/12   kguo    Added api for rx chain selection
02/11/09   rm      Added API to return W band

===========================================================================*/

#include "tdsl1_ftm_msg.h"
//#include "msgr_msg.h"
/*chunxial for CR 369090 */

/*chunxial for CR 369090 */
#define TDSSRCH_NUM_ANTENNAS                  2
#define TDSSRCH_UL_MAX_NUM_TS                6
#define TDSSRCH_TX_PWR_DEFAULT               0x7800


typedef enum
{
  TDSSRCH_NO_ANNTENNA,    
  TDSSRCH_ANNTENNA_ZERO,      
  TDSSRCH_ANNTENNA_ONE,     
  TDSSRCH_BOTH_ANNTENNA,    
  TDSSRCH_INVALID_ANNTENNA
} tds_anntenna_selection_enum_type;
/*chunxial for CR 369090 */

/* Define the midRssi structure of ervery frequency*/

typedef struct
{
  uint16 primary_frequency;
  int16 serving_cell_primary_freq_Rssi;
  int16 serving_cell_primary_freq_Rssi_rx1;
} tdssrch_freq_syncdlRssi_type;

/*Define the tx power for uplink timeslots*/
typedef struct
{
  int16 tdssrch_uplink_tx_pwr[TDSSRCH_UL_MAX_NUM_TS];
}tdssrch_uplink_tx_power_type;


#define TDSSRCH_NUM_ANTENNAS   2
#define TL1_DL_TIME_SLOT_MAX          7
#define TL1_UL_TIME_SLOT_MAX          6

typedef struct
{

    /* RxAgc*256. RRC gets raw data from lower layer and passes to upper layer
     in order to keep the accuracy. Conversion (divide 256 and store as float number)
     needs to be done at upper layer.
  */
  int32       AgcValue;

  /* -15 to 127 dBm
     -15 dBm is sent to upper layer if actual SINR is less than -15 dBm.
     When UE is not in CELL_DCH, SINR is not measured. A mid-value of 5 dBm                                                .
     is sent to upper layer.
  */
  uint8 timeslot;/*for TS0 to TS6 (TS1 is used by E-FTM)*/

  boolean Isinterfreq;

} tds_l1_rx_agc_type;

typedef struct
{

    /* RxAgc*256. RRC gets raw data from lower layer and passes to upper layer
     in order to keep the accuracy. Conversion (divide 256 and store as float number)
     needs to be done at upper layer.
  */
  int16       AgcValue;

  /* -15 to 127 dBm
     -15 dBm is sent to upper layer if actual SINR is less than -15 dBm.
     When UE is not in CELL_DCH, SINR is not measured. A mid-value of 5 dBm                                                .
     is sent to upper layer.
  */
  uint8 timeslot;/*Ts[0] is for UPPCH, and ts[1~5] for TS1 to TS5*/


} tds_l1_tx_agc_type;
/*----------------------------------------------------------------------------
                         TDSCDMA_L1_SCELL_SIGNAL_STATUS_IND
This primitive is sent from TRRC to upper layer to indicate signal status of 
serving cell. 
This structure contains: 
     - RSSI
     - RSCP 
     - EcIo (RSCP/RSSI)
     - TS0 SINR (CELL_DCH only, default value 5 dBm)
     - BLER (CELL_DCH only, default value 0)
     - AS_ID (TBD - will be added after DSDS/DSDA are supported)  
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */

    /* RxAgc*256. RRC gets raw data from lower layer and passes to upper layer
     in order to keep the accuracy. Conversion (divide 256 and store as float number)
     needs to be done at upper layer.
  */
  tds_l1_rx_agc_type       RxAgc[TDSSRCH_NUM_ANTENNAS][TL1_DL_TIME_SLOT_MAX];
  tds_l1_tx_agc_type       TxAgc[TDSSRCH_NUM_ANTENNAS][TL1_UL_TIME_SLOT_MAX];

  /* -15 to 127 dBm
     -15 dBm is sent to upper layer if actual SINR is less than -15 dBm.
     When UE is not in CELL_DCH, SINR is not measured. A mid-value of 5 dBm                                                .
     is sent to upper layer.
  */
  int16        sinr[TDSSRCH_NUM_ANTENNAS][TL1_DL_TIME_SLOT_MAX];
  /*
  TDSSRCH_NO_ANNTENNA(0):    no anntenna tuned 
  TDSSRCH_ANNTENNA_ZERO(1):     anntenna 0 tuned 
  TDSSRCH_ANNTENNA_ONE(2),        anntenna 1 tuned 
  TDSSRCH_BOTH_ANNTENNA(3),     both anntenna 0 and 1 tuned 
  TDSSRCH_INVALID_ANNTENNA(4)  error status for anntenna tuned
  */
  tds_anntenna_selection_enum_type   antennaSelect;
  boolean tx_enabled;
  int16 max_txpwr;

} tds_l1_timeslot_based_signal_status_ind_type;


/*=========================================================================

 FUNCTION     :tdssrch_get_timeslot_based_rxagc_info
 
 DESCRIPTION  : The function is used to get timeslot based RxAgc. 
              
 DEPENDENCIES : 

 INPUT VALUE:    tds_l1_timeslot_based_signal_status_ind_type
  
 RETURN VALUE : 
   TRUE:    The data in the cm_timeslot_based_signal_strength_info pointer is avaliable
   FALSE:   The data in the cm_timeslot_based_signal_strength_info pointer is not avaliable
   
 SIDE EFFECTS : None
==========================================================================*/
boolean tdssrch_get_timeslot_based_rxagc_info(tds_l1_timeslot_based_signal_status_ind_type *cm_timeslot_based_signal_strength_info);

/*=========================================================================

 FUNCTION     :tdssrch_get_timeslot_based_txagc_info
 
 DESCRIPTION  : The function is used to get timeslot based TxAgc. 
              
 DEPENDENCIES : 

 INPUT VALUE:    tds_l1_timeslot_based_signal_status_ind_type
  
 RETURN VALUE : 
   TRUE:    The data in the cm_timeslot_based_signal_strength_info pointer is avaliable
   FALSE:   The data in the cm_timeslot_based_signal_strength_info pointer is not avaliable
   
 SIDE EFFECTS : None
==========================================================================*/
boolean tdssrch_get_timeslot_based_txagc_info(tds_l1_timeslot_based_signal_status_ind_type *cm_timeslot_based_signal_strength_info);

/*----------------------------------------------------------------------------
                         TDSCDMA_L1_SCELL_SIGNAL_STATUS_IND
This primitive is sent from TRRC to upper layer to indicate signal status of 
serving cell. 
This structure contains: 
     - RSSI
     - RSCP 
     - EcIo (RSCP/RSSI)
     - TS0 SINR (CELL_DCH only, default value 5 dBm)
     - BLER (CELL_DCH only, default value 0)
     - AS_ID (TBD - will be added after DSDS/DSDA are supported)  
----------------------------------------------------------------------------*/
typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr;

  /* (-80 to -4)*256 dBm */
  int16       rssi[TDSSRCH_NUM_ANTENNAS];

  /* (-120 to -25)*256 dBm */
  int16       rscp[TDSSRCH_NUM_ANTENNAS];

  /* (RSCP/RSSI(primary atanna))*256. RRC gets raw data from lower layer and passes to upper layer
     in order to keep the accuracy. Conversion (divide 256 and store as float number)
     needs to be done at upper layer.
  */
  int16       ecio[TDSSRCH_NUM_ANTENNAS];


  /* -15 to 127 dBm
     -15 dBm is sent to upper layer if actual SINR is less than -15 dBm.
     When UE is not in CELL_DCH, SINR is not measured. A mid-value of 5 dBm                                                .
     is sent to upper layer.
  */
  int16        sinr;

  /* This is a percentage rate range from 0 - 63 (%)
     0 is sent to upper layer if UE is not in CELL_DCH.
  */
  uint8       bler;
  
} tds_l1_cm_scell_signal_status_ind_type;

typedef struct
{
  /* message header type */
  msgr_hdr_s  msg_hdr;

  /* (-80 to -4)*256 dBm */
  int16       instance_rssi[TDSSRCH_NUM_ANTENNAS];

  /* (-120 to -25)*256 dBm */
  int16       instance_rscp[TDSSRCH_NUM_ANTENNAS];

  /* (RSCP/RSSI(primary atanna))*256. RRC gets raw data from lower layer and passes to upper layer
     in order to keep the accuracy. Conversion (divide 256 and store as float number)
     needs to be done at upper layer.
  */
  int16       instance_ecio[TDSSRCH_NUM_ANTENNAS];


  /* -15 to 127 dBm
     -15 dBm is sent to upper layer if actual SINR is less than -15 dBm.
     When UE is not in CELL_DCH, SINR is not measured. A mid-value of 5 dBm                                                .
     is sent to upper layer.
  */
  int16        instance_sinr;

  /* This is a percentage rate range from 0 - 63 (%)
     0 is sent to upper layer if UE is not in CELL_DCH.
  */
  uint8       instance_bler;
  
} tds_l1_cm_scell_signal_instance_info_type;



extern void tdssrch_cm_get_serving_cell_signal_instance_info(tds_l1_cm_scell_signal_instance_info_type *cm_serving_cell_signal_strength_info);



extern void tdssrch_cm_get_serving_cell_signal__info(tds_l1_cm_scell_signal_status_ind_type *cm_serving_cell_signal_strength_info);

extern boolean tdssrch_get_timeslot_based_rxagc_info(tds_l1_timeslot_based_signal_status_ind_type *cm_timeslot_based_signal_strength_info);
extern boolean tdssrch_get_timeslot_based_txagc_info(tds_l1_timeslot_based_signal_status_ind_type *cm_timeslot_based_signal_strength_info);



//#ifdef FEATURE_TDSCDMA_FTM_NS_NEW
/* Data structures for FTM NS */

/* TDSL1_CPHY_SETUP_REQ Sub-primitive mask: */
#define TDSL1_API_CPHY_UL_PHYCHAN_DROP_INCL      0x0001
#define TDSL1_API_CPHY_DL_PHYCHAN_DROP_INCL      0x0002
#define TDSL1_API_CPHY_DL_CCTRCH_INCL            0x0004
#define TDSL1_API_CPHY_UL_CCTRCH_INCL            0x0008
#define TDSL1_API_CPHY_DL_PHYCHAN_ADD_INCL       0x0010
#define TDSL1_API_CPHY_UL_PHYCHAN_ADD_INCL       0x0020
#define TDSL1_API_CPHY_DL_PHYCHAN_CFG_INCL       0x0040
#define TDSL1_API_CPHY_UL_PHYCHAN_CFG_INCL       0x0080
#define TDSL1_API_CPHY_HS_CHAN_CFG_INCL          0x0100
#define TDSL1_API_CPHY_E_DL_CHAN_CFG_INCL        0x0200
#define TDSL1_API_CPHY_E_UL_CHAN_CFG_INCL        0x0400
#define TDSL1_API_CPHY_NON_SCHED_GRANT_CFG_INCL  0x0800

typedef enum
{
  TDSL1_FTM_BER_RMC_TYPE_12_2_SC      = 0,
  TDSL1_FTM_BER_RMC_TYPE_12_2_MC      = 1,
  TDSL1_FTM_BER_RMC_TYPE_64           = 2,    
  TDSL1_FTM_BER_RMC_TYPE_144          = 3,
  TDSL1_FTM_BER_RMC_TYPE_384          = 4,    
  TDSL1_FTM_BER_RMC_TYPE_MAX
} tdsl1_ftm_ns_rmc_setup_type;

typedef enum 
{
  TDSL1_FTM_TPC_STEP_SIZE_0DB, /* If TPC Step Size is absent in OTA message */
  TDSL1_FTM_TPC_STEP_SIZE_1DB,
  TDSL1_FTM_TPC_STEP_SIZE_2DB,
  TDSL1_FTM_TPC_STEP_SIZE_3DB,
  TDSL1_FTM_NUM_TPC_STEP_SIZE
} tdsl1_ftm_tpc_step_size_enum_type;

typedef enum
{
  TDSL1_CMAPI_IF_CHAIN_0_ON = 0,
  TDSL1_CMAPI_IF_CHAIN_1_ON = 1 ,
  TDSL1_CMAPI_IF_BOTH_CHAIN_ON = 2,
  //'Prx-Only-Both-Chain-On' special case--use Prx, but enable both chains at FED. 
  TDSL1_CMAPI_IF_CHAIN_0_BOTH_ON = 3,
  TDSL1_CMAPI_IF_CHAIN_STATUS_INVALID
}tdsl1_cmapi_if_rx_status_type;

typedef struct
{
  uint16 freq;

} tdsl1_api_acq_req_t;

typedef struct
{
  boolean freq_incl;
  uint16 primary_freq;
  boolean working_freq_incl;
  uint16 working_freq;
  uint8 dl_slot;
  uint8 ul_slot;
  int8 max_tx_pwr;
  tdsl1_ftm_ns_rmc_setup_type rmc_setup;
  tdsl1_ftm_tpc_step_size_enum_type ul_tpc_step;
  
  uint16 req_mask;
        
  uint8 cell_parm_id;
  uint8 dl_cctrch_id;

  uint8 epuch_slot;
  uint8 epuch_code;
  int8  epuch_tx_power;
  uint8 mod_type;
  uint8 num_eucch;
} tdsl1_api_ftm_ns_cphy_setup_req_t;

typedef union
{
  tdsl1_api_ftm_ns_cphy_setup_req_t ftm_msg;
} tdsl1_api_cphy_setup_req_t;
//#endif

/*===========================================================================

FUNCTION tdssrch_get_freq_midRssi_tx_pwr_info

DESCRIPTION
  Reports RSSI and txpower to cm layer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdssrch_get_freq_syncdlRssi_tx_pwr_info (tdssrch_freq_syncdlRssi_type* cm_freq_DwPTSRssi,tdssrch_uplink_tx_power_type* cm_tx_pwr_info);

/*===========================================================================
FUNCTION tdsrxd_set_active_rx_chain

DESCRIPTION
  API used to indicate the Rx chain setting to be used for next traffic state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsrxd_set_rx_chain_status(tdsl1_cmapi_if_rx_status_type cmapi_rx_chain_config);

/*===========================================================================

FUNCTION tdsrxd_set_rx_chain_status

DESCRIPTION
  Set rx chain selection status

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdsrxd_set_rx_chain_status(tdsl1_cmapi_if_rx_status_type cmapi_rx_chain_config);

/*Add for timetag cmd API*/
boolean tdsl1_api_send_timetag_cmd(boolean allow_sleep_timetag);

//#ifdef FEATURE_TDSCDMA_FTM_NS_NEW
/*===========================================================================

FUNCTION  TDSL1_API_SEND_START_CMD

DESCRIPTION
  Sends TDSL1_CPHY_START_TDSCDMA_MODE_REQ to TL1

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdsl1_api_send_start_cmd(void);

/*===========================================================================

FUNCTION  TDSL1_API_SEND_STOP_CMD

DESCRIPTION
  Sends TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ to TL1

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdsl1_api_send_stop_cmd(void);

/*===========================================================================

FUNCTION  TDSL1_API_SEND_ACQ_CMD

DESCRIPTION
  Sends TDSL1_CPHY_ACQ_REQ to TL1

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdsl1_api_send_acq_cmd(tdsl1_api_acq_req_t *msg);

/*===========================================================================

FUNCTION  TDSL1_API_SEND_SETUP_CMD

DESCRIPTION
  Sends TDSL1_CPHY_SETUP_REQ to TL1

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdsl1_api_send_setup_cmd(tdsl1_api_cphy_setup_req_t *msg);
//#endif /*FEATURE_TDSCDMA_FTM_NS_NEW*/

/*===========================================================================

FUNCTION  tdsl1_api_get_tds_band

DESCRIPTION
  Convert TDS channel to system band

DEPENDENCIES
  None

RETURN VALUE
  sys_band_class_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_band_class_e_type tdsl1_api_get_tds_band(uint16 freq);


/*===========================================================================

FUNCTION  tdsl1ulcfg_config_modem_power_stats_tx

DESCRIPTION
Enables/disables capturing of tx power stats

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
extern void tdsl1ulcfg_config_modem_power_stats_tx(boolean en_pwr_stats);

/*===========================================================================

FUNCTION  tdsl1ulcfg_get_modem_power_stats_tx

DESCRIPTION
Fetches the tx power stats and clears old stats

RETURN VALUE
Tx power stats in ms

SIDE EFFECTS
None

===========================================================================*/
extern void tdsl1ulcfg_get_modem_power_stats_tx(uint32 *tx_power_bin);



#endif /*TDS_L1_API_H*/
