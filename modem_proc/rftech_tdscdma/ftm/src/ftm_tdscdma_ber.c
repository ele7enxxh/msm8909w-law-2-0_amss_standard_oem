/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  Copyright (c) 2011 - 2014 by Qualcomm Technologies INCORPORATED.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_ber.c#1 $ 
  $DateTime: 2016/12/13 07:59:50 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/15   jak     Offtarget fix 
07/30/14   qzh     Enable FTM NS new intf and remove the dependency
04/26/14   jyu     KW fix
11/20/13   qzh     Featurized FTM NS new interface to disable it on BO 0.1 
09/20/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
09/11/13   qzh     Add command FTM_TDSCDMA_BER_TPC_CONTROL_REQ to support TPC override feature
09/02/13   qzh     Add "meas_occasion_info_incl" initialization in FTM NS end call
08/26/13   ms      Add diversity support for ftm_tdscdma_ber_rx_level   
06/30/13   qzh     Replace memcpy() with safer memscpy().
06/07/13   qzh     Add support for FTM HSUPA 16QAM on 8820C
02/27/13   ms      ftm_tdscdma_ber_get_rxagc merge
01/03/13   ms      NS handover (Nikel merge) 
12/06/12   ms      HSUPA support (merge from Nikel)
11/19/12   ms      KW fix 
07/17/12   hy      Fixed Rx Level dBm unit
07/16/12   hy/sk   Fixed DCH ReConfig (HHO) failure
                           Changed state machine to force Stop TDS after drop all
                           Implement RxLevel Request command
                           Implement Tx Pwr Override command
05/23/12   hy      Enabled All 1 BER calculation to print out CRC status
04/24/12   jfc     Use TASK_HANDLE instead of direct tcb access
04/24/12   hy      Fix NS TPC step size not taking effect; fix a crash at poor downlink signals
03/26/12   hy      TD-SCDMA Cal time optimization
03/22/12   hy      Fix an incorrect buffer size used for ftm_se_ber(bit to byte)
03/22/12   hy      NS SE BER for PN9 and PN15 data pattern(PN15 requires invert PN stream)
03/21/12   hy      NS API clean-up and redesign.
03/16/12   hy/sk   NS BER/TxLoopback/HHO/Release
                   Remove the temp fix to tx pwr because it's fixed in TFW
03/03/12   ycl     Fix compiler error.
02/02/12   hy      Fix Tx pwr being 15dB lowe
02/16/12   hy      Added support for TDSCDMA Non-signaling
05/19/11   hy      Created
===========================================================================*/


/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#define FEATURE_RF_TL1_FTM_NEW_INTF
#endif

#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"

#ifdef FTM_HAS_TDSCDMA_BER

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <stringl/stringl.h>

#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_v.h"
#include "rf_test_task_v.h"
#include "rfumts.h"
#include "ftm_tdscdma_ber.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_mdsp_types.h"
#include "rf_tdscdma_mdsp_smem_intf.h"
#include "rf_tdscdma_mdsp_util.h"
#include "rf_tdscdma_mc.h"
#include "ftm_tdscdma_ctl.h"
#include "ftm_common_control.h"
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tdsl1_ftm_msg.h"
#include "msgr.h"
#include "rf_tdscdma_msg.h"
#endif

//hy todo: needed cust feature.
#if defined(FTM_HAS_TDS_HSDPA)
#include "ftm_tdshsdpa_bler.h"
#endif /*FTM_HAS_TDS_HSDPA*/

#include "err.h"
#include "msg.h"
#include "rex.h"
#include "task.h"
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tdsl1mtask.h"
#include "tdsl2ultask.h"
#include "tdsl2dltask.h"
#include "tdsmacrrcif.h"
#include "tdsl1m.h"
#include "tdsl1def.h"
#include "tdsl1mcmd.h"
#include "tdsl1msetup.h"
#include "tdsl1macdata.h"
#include "tdsulutil.h"
#endif

#include "ftm_log.h"


#ifdef FTM_HAS_SE_BER
#include "ftm_se_ber.h"
#endif

#if defined(FEATURE_FTM_GSM_BER)
#include "ftm_umts_ber.h"
#include "ftm_gsm_ber.h"
#endif

#include "ftm_tdscdma_hsupa.h"

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tds_l1_api.h"
#else
#include "tdseulsg.h"
#endif

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                               MACROS

===========================================================================*/
#define FTM_TDSCDMA_BER_SET_STATE(s)   (ftm_tdscdma_ber_set_state(s))
#define FTM_TDSCDMA_BER_GET_STATE()    (ftm_tdscdma_ber_current_state)
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#define TDSL1_NS_CPHY_DROP_ALL_INCL             0x8000
#endif

/*This local feature is used to calculate data pattern = all 1s BER, 
since FTM_SE_BER only supports PN9 and PN15(with or without invert stream).
if this feature is defined, then CRC pass will be used for all data patterns.
if CRC isn't passing, it assumes all 1s data pattern to calculate BER.*/
#define FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER 

/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/

/* Queue for external messages (e.g. from RRC) */
static q_type                ftm_tdscdma_ber_q;

/* Queue and Free Queue for internal message */
static q_type                ftm_tdscdma_ber_int_q;
static q_type                ftm_tdscdma_ber_int_free_q;

/* Allocated buffer for internal messages */
static ftm_tdscdma_ber_int_cmd_type          ftm_tdscdma_ber_int_cmd_buf[10];

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
/* Buffers to hold all dl config */
ftm_tdscdma_ber_dl_db_type                  ftm_tdscdma_dl_db = {0, FALSE};
ftm_tdscdma_ber_state_type                         ftm_tdscdma_ber_current_state = FTM_TDSCDMA_BER_STATE_NULL;
ftm_tdscdma_ber_dch_config_type                    ftm_tdscdma_dch_config = {TDSL1_FTM_BER_RMC_TYPE_12_2_SC};

/* Buffers to hold all ul tx data  */

uint8                                       ftm_tdscdma_dl_cctrch_id = 0;

#else
/* Buffers to hold all dl config */
ftm_tdscdma_ber_dl_db_type                         ftm_tdscdma_dl_db = {NULL, NULL, NULL, 0, FALSE, FALSE};
ftm_tdscdma_ber_ul_db_type                         ftm_tdscdma_ul_db = {NULL, NULL, NULL, FALSE};
ftm_tdscdma_ber_state_type                         ftm_tdscdma_ber_current_state = FTM_TDSCDMA_BER_STATE_NULL;
ftm_tdscdma_ber_dch_config_type                    ftm_tdscdma_dch_config = {FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC};
static tdsl1dlcctrchcfg_cctrch_info_struct_type    ftm_tdscdma_dl_cctrch_info;
static tdsl1dlcctrchcfg_dl_ctfc_info_struct_type   ftm_tdscdma_ctfc_entry_array[TDSL1_DL_MAX_TFC];
static tdsl1_dl_phycfg_phychan_db_struct_type      ftm_tdscdma_dl_phychan_db;
static tdsl1_dl_cctrchcfg_trch_info_struct_type    ftm_tdscdma_dl_trch[TDSL1_DL_MAX_TRCH];
static tdsl1_dl_cctrchcfg_tf_info_struct_type      ftm_tdscdma_dl_tf_info[TDSL1_DL_MAX_TRCH][TDSL1_DL_MAX_TF_PER_TRCH];
static tdsl1_dl_cctrchcfg_ctfc_info_struct_type    ftm_tdscdma_dl_ctfc_info;
static mcaltds_dec_trch_info_struct_type           ftm_tdscdma_trch_info[TDSL1_DL_MAX_TRCH];
static mcaltds_dec_tf_info_struct_type             ftm_tdscdma_tf_info[TDSL1_DL_MAX_TRCH][TDSL1_DL_MAX_TF];
static mcaltds_dec_ctfc_entry_info_struct_type     ftm_tdscdma_ctfc_entry_info[TDSL1_DL_MAX_TFC];
static uint8                                       ftm_tdscdma_tfi_table[TDSL1_DL_MAX_TFC][TDSL1_DL_MAX_TRCH];

/* Buffers to hold all ul config */
static tdsl1_ul_phych_cfg_struct_type              ftm_tdscdma_ul_phychan_cfg;
static tdsl1_ul_cctrch_cfg_struct_type             ftm_tdscdma_ul_cctrch_cfg;
static tdsl1_ul_semi_static_parms_struct_type      ftm_tdscdma_ul_semi_static_parms[TDSL1_MAX_NUM_TRCH];
static tdsl1_ul_gain_factor_parms_struct_type      ftm_tdscdma_ul_gain_factor_parms[TDSL1_UL_TFC_MAX];
static tdsmac_ul_tfcs_type                         ftm_tdscdma_ul_mac_tfcs;

/* Buffers to hold all ul tx data  */
static ftm_tdscdma_ber_ul_tx_data_type             ftm_tdscdma_ber_ul_tx_data;
static boolean                                     ftm_tdscdma_ber_max_tti_found = FALSE;
static uint8                                       ftm_tdscdma_dl_cctrch_id = 0;

#endif

/* Structure for events that will be reported synchronously */
typedef struct 
{
	uint8 has_updated;
	ftm_tdscdma_ber_sync_status_type status;

} ftm_tdscdma_ber_sync_status_struct;

/* array to record the events that will be reported synchronously 
   via FTM_TDSCDMA_BER_EVENT_STATUS_REQ
*/
ftm_tdscdma_ber_sync_status_struct TDSCDMA_BER_Sync_Status[  FTM_TDSCDMA_BER_SYNC_STATUS_MAX ];

ftm_tdscdma_ber_rmc_dch_setup_type reconfig_cmd;

#ifdef FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER     
// Data structure for BER calculation
static uint32 ftm_tdscdma_bits_per_second    = 0;
static uint32 ftm_tdscdma_ber_per_second     = 0;
static uint8  ftm_tdscdma_tti_cnt_per_second = 50;
static uint8  ftm_tdscdma_tti_curr_cnt       = 0;
static uint32 ftm_tdscdma_total_bits         = 0; 
static uint32 ftm_tdscdma_ber_total          = 0; 
static uint8 ftm_tdscdma_ber_calc_nibble_tbl[16] = {4,3,3,2,4,2,2,1,3,2,2,1,2,1,1,0};
static uint8 ftm_tdscdma_ber_calc_1_bit_tbl[2] = {1,0};
static uint8 ftm_tdscdma_ber_calc_2_bit_tbl[4] = {2,1,1,0};
static uint8 ftm_tdscdma_ber_calc_3_bit_tbl[8] = {3,2,2,1,2,1,1,0};
#endif //FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_ber_allocate_dl_db( void);
void ftm_tdscdma_ber_free_dl_db( void);
void ftm_tdscdma_ber_allocate_ul_db(void);
void ftm_tdscdma_ber_free_ul_db( void);
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
void ftm_tdscdma_ber_process_msgs( void);
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_ber_process_l1_messages( tdsrrc_cmd_type *);
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
void ftm_tdscdma_ber_send_CPHY_START_TDSCDMA_MODE_REQ(void);
void ftm_tdscdma_ber_send_CPHY_STOP_TDSCDMA_MODE_REQ(void);
void ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ(void);
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_ber_send_CPHY_IDLE_REQ(void);
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
void ftm_tdscdma_ber_send_CPHY_ACQ_REQ(ftm_tdscdma_ber_acquire_config_type *);
void ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ(void);
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_ber_dl_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_ber_ul_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_ber_drop_dl_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_ber_drop_ul_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_set_dl_cctrch_db_information( void );
void ftm_tdscdma_set_dl_phychan_db_information( tdsl1_dl_phychan_enum_type phych_type,
  uint8 cctrch_id);
void ftm_tdscdma_set_ul_cctrch_db_information( void);
void ftm_tdscdma_set_ul_phychan_db_information(tdsl1_ul_phychan_enum_type phych_type);
void ftm_tdscdma_ber_set_ul_tx_data (tdsl1_dl_tb_data_struct_type *dl_data_ptr);

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

void ftm_tdscdma_ber_send_int_cmd( ftm_tdscdma_ber_int_cmd_type *msg);
void ftm_tdscdma_ber_loopback_handler(ftm_tdscdma_ber_int_cmd_rx_data_type* rx_data_msg);

void ftm_tdscdma_ber_clear_sync_status( void);
ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_set_ul_pwr( byte override_enable,int2 tx_power_10th_dbm);
ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_get_rx_level(ftm_tdscdma_ber_rx_chain_type rx_chain, ftm_tdscdma_ber_rx_level_rpt_type*  ftm_tdscdma_ber_rx_lev_rpt);
ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_tpc_control_req(int2 override_enable, uint32 tpc_pattern);
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
uint8 ftm_tdscdma_ber_get_max_tti_frm_num (ftm_tdscdma_ber_ul_tx_data_type *mac_data_ptr);
void ftm_tdscdma_ber_trblk_data_ind 
(
  tdsl1_dl_tb_data_struct_type *data_ptr,
  uint32 crc_status [][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH]
);
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
#ifdef FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER
void ftm_tdscdma_ber_calc_dl_ber (uint8 *tb_data_ptr, uint16 tb_size, boolean err);
#endif //FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER

extern ftm_tdscdma_hsupa_db_type ftm_tdscdma_hsupa_db;
extern ftm_rsp_pkt_type ftm_tdscdma_hsupa_dispatch( ftm_tdscdma_ber_pkt_type *cmd_ptr );
void ftm_tdscdma_ber_set_state(ftm_tdscdma_ber_state_type s)
  {
    if(ftm_tdscdma_ber_current_state != s)
  {
       FTM_MSG_HIGH("TDS NS: BER State [%d -> %d]",ftm_tdscdma_ber_current_state, s, 0);       
  }

    ftm_tdscdma_ber_current_state = s;
}

static ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_rmc_dch_setup_req (ftm_tdscdma_ber_pkt_type *cmd_ptr );
/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_ASSIGN_CCTRCH_ID

DESCRIPTION    Used when reconfiguring a DCH channel.  This "allocates" a
               new CCTrCH ID based on the current ID in use.  The current one
               will get dropped while the new one gets added.


DEPENDENCIES
               DCH should be active

===========================================================================*/
void ftm_tdscdma_ber_assign_cctrch_id( void)
{
  if (ftm_tdscdma_dl_cctrch_id == 0)
  {
     ftm_tdscdma_dl_cctrch_id = 1;
  }
  else 
  {
     ftm_tdscdma_dl_cctrch_id = 0;
  }

  FTM_MSG_HIGH("Assigned new CCTrCH ID: %d", ftm_tdscdma_dl_cctrch_id,0,0);
}


/*===========================================================================

FUNCTION        FTM_TDSCDMA_BER_CREATE_GEN_RESPONSE

DESCRIPTION     Creates a generic pass/fail type of response for a given
                dispatched TDSCDMA command.


DEPENDENCIES
   None

RETURN VALUE
                ftm_rsp_pkt_type -- FTM packet to send via diag

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_tdscdma_ber_create_gen_response( ftm_tdscdma_ber_pkt_type *cmd_ptr, uint16 status,
                                                    uint16 cid, uint16 scid)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_tdscdma_ber_gen_rsp_pkt_type   *gen_rsp_payload;


  gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_tdscdma_ber_gen_rsp_pkt_type));

  if( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_tdscdma_ber_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_TDSCDMA_BER_GEN_RESPONSE;
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;

  }

  return gen_rsp;
}

/*===========================================================================

FUNCTION        FTM_TDSCDMA_BER_CREATE_RX_LEV_RESPONSE

DESCRIPTION     Creates a rx level response for rx level request.


DEPENDENCIES
   None

RETURN VALUE
                ftm_rsp_pkt_type -- FTM packet to send via diag

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_tdscdma_ber_create_rx_lev_response( ftm_tdscdma_ber_pkt_type *cmd_ptr, uint16 status,
                                                    uint16 cid, uint16 scid, ftm_tdscdma_ber_rx_level_rpt_type ftm_tdscdma_ber_rx_lev_rpt)
{

  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_tdscdma_ber_rx_lev_rsp_pkt_type   *gen_rsp_payload;

  gen_rsp = ftmdiag_create_new_pkt( sizeof(ftm_tdscdma_ber_rx_lev_rsp_pkt_type));

  if( gen_rsp.pkt_payload != NULL)
  {
    gen_rsp_payload = ( ftm_tdscdma_ber_rx_lev_rsp_pkt_type *) gen_rsp.pkt_payload;
	
    gen_rsp_payload->header                  = cmd_ptr->header;
    gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_TDSCDMA_BER_GEN_RESPONSE;  // Need to check on this
    gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
    gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
    gen_rsp_payload->status                  = status;
    gen_rsp_payload->rx_lev_rpt              = ftm_tdscdma_ber_rx_lev_rpt;
  }

  return gen_rsp;
}


/*===========================================================================

FUNCTION     FTM_TDSCDMA_BER_LOG_GEN_CNF

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed,
             but no payload needs to be sent.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_tdscdma_ber_log_gen_cnf( ftm_log_tdscdma_ber_id_type  id)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_tdscdma_ber_gen_cnf        *gen_cnf_log;


  gen_cnf_log = ( ftm_log_tdscdma_ber_gen_cnf *)
                  ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_BER,
                                  sizeof( ftm_log_tdscdma_ber_gen_cnf) );

  if( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;

    log_commit( gen_cnf_log);
  }
  #endif
}


/*===========================================================================

FUNCTION     FTM_TDSCDMA_BER_LOG_GEN_CNF_WITH_STATUS

DESCRIPTION
             Logs a generic confirmation message to the user (PC side).  This
             is used when the indication is that a command was completed, and
             pass a generic status byte to the user.

DEPENDENCIES
  none

===========================================================================*/
LOCAL void ftm_tdscdma_ber_log_gen_cnf_with_status( ftm_log_tdscdma_ber_id_type  id, uint8 status)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_tdscdma_ber_gen_cnf_with_status        *gen_cnf_log;

  gen_cnf_log = ( ftm_log_tdscdma_ber_gen_cnf_with_status *)
                  ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_BER,
                                  sizeof( ftm_log_tdscdma_ber_gen_cnf_with_status) );

  if( gen_cnf_log != NULL)
  {
    gen_cnf_log->log_id = id;
    gen_cnf_log->status = status;

    log_commit( gen_cnf_log);
  }
  #endif

}


/*===========================================================================

FUNCTION    FTM_GSM_BER_LOG_ACQ_BCCH_CNF

DESCRIPTION
            Logs a select specific bcch confirmation message to the user (PC
            side).  Payload is an indication if acquisition was successful,
            and the SCH data block.  SCH data will only be valid if the
            confirmation indicates the acquisition was successful.

DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_log_acq_cnf( boolean status, uint16 freq)
{

  #ifdef FTM_HAS_LOGGING
  ftm_log_tdscdma_ber_acq_cnf_type   *tdscdma_ber_log;

  tdscdma_ber_log = (ftm_log_tdscdma_ber_acq_cnf_type *)
                  ftm_log_malloc( (uint16)FTM_LOG_TDSCDMA_BER,
                  sizeof( ftm_log_tdscdma_ber_acq_cnf_type));

  if( tdscdma_ber_log != NULL)
  {
    tdscdma_ber_log->log_id   = FTM_LOG_TDSCDMA_BER_ACQ_CNF;
    tdscdma_ber_log->status   = status;
    tdscdma_ber_log->freq     = freq;

    log_commit( tdscdma_ber_log);
  }
  #endif
}

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_ALLOCATE_UL_DB

DESCRIPTION    Sets up static database for handling all UL L1 configuration


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_allocate_ul_db( void)
{

  int i = 0;

  ftm_tdscdma_ul_db.ul_phychan_db = &ftm_tdscdma_ul_phychan_cfg;
  memset(&ftm_tdscdma_ul_phychan_cfg, 0x00, sizeof(tdsl1_ul_phych_cfg_struct_type));

  ftm_tdscdma_ul_db.ul_cctrch_info    = &ftm_tdscdma_ul_cctrch_cfg;
  memset(&ftm_tdscdma_ul_cctrch_cfg, 0x00, sizeof(tdsl1_ul_cctrch_cfg_struct_type));

  for( i = 0; i < TDSL1_MAX_NUM_TRCH; i++)
  {
    ftm_tdscdma_ul_db.ul_cctrch_info->semi_static_parms_ptr[i] = &ftm_tdscdma_ul_semi_static_parms[i];
  }

  for( i = 0; i < TDSL1_UL_TFC_MAX; i++)
  {
    ftm_tdscdma_ul_db.ul_cctrch_info->gain_factor_parms_ptr[i] = &ftm_tdscdma_ul_gain_factor_parms[i];
  }

  ftm_tdscdma_ul_db.tfcs_ptr = &ftm_tdscdma_ul_mac_tfcs;
  memset(&ftm_tdscdma_ul_mac_tfcs, 0x00, sizeof(tdsmac_ul_tfcs_type));  

  memset(&ftm_tdscdma_ber_ul_tx_data, 0x00, sizeof(ftm_tdscdma_ber_ul_tx_data_type));  

  ftm_tdscdma_ul_db.isInitialized = TRUE;
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_FREE_UL_DB

DESCRIPTION


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_free_ul_db( void)
{

  ftm_tdscdma_ul_db.isInitialized = FALSE;
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_ALLOCATE_DL_DB

DESCRIPTION    Sets up static database for handling all DL L1 configuration


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_allocate_dl_db( void)
{

  int i = 0;
  int j = 0;


  ftm_tdscdma_dl_db.dl_phychan_db = &ftm_tdscdma_dl_phychan_db;

  ftm_tdscdma_dl_db.dl_cctrch_info    = &ftm_tdscdma_dl_cctrch_info;

  for( i = 0; i < TDSL1_DL_MAX_TRCH; i++)
  {

    ftm_tdscdma_dl_db.dl_cctrch_info->trch_info_array[i] = &ftm_tdscdma_dl_trch[i];

    /* for each transport channel, allocate space for the array of Transport Format info structures */

    for( j = 0; j < TDSL1_DL_MAX_TF_PER_TRCH; j++)
    {
      ftm_tdscdma_dl_db.dl_cctrch_info->trch_info_array[i]->tf_info_array[j] = &ftm_tdscdma_dl_tf_info[i][j];
    }

  }

  /* allocate memory for the dl_ctfc_info structure */

  ftm_tdscdma_dl_db.dl_ctfc_info      = &ftm_tdscdma_dl_ctfc_info;

  /* for each tfc possible, allocate space for a mcaltds_dec_ctfc_entry_info_struct_type */


  for( i = 0; i < TDSL1_DL_MAX_TFC; i++)
  {
    ftm_tdscdma_dl_db.dl_ctfc_info->ctfc_entry_array[i] = &ftm_tdscdma_ctfc_entry_info[i];

#ifndef FEATURE_RRC_L1_INF_USE_CTFC
    ftm_tdscdma_dl_db.dl_ctfc_info->ctfc_entry_array[i]->tfi_table = &ftm_tdscdma_tfi_table[i][0];
#endif
  }


  ftm_tdscdma_dl_db.isInitialized = TRUE;
}


/*===========================================================================

FUNCTION     FTM_TDSCDMA_BER_FREE_DL_DB

DESCRIPTION  "Frees" static database for handling all DL L1 configuration


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_free_dl_db( void)
{
  ftm_tdscdma_dl_db.isInitialized = FALSE;
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_INIT

DESCRIPTION    Initializes memory and data structures for FTM TDSCDMA BER
               functionality.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_init( void)
{

  int i;

  /* External message Q */
  (void)q_init( &ftm_tdscdma_ber_q);

  /* Internal message Q..statically allocated, so uses q and free q */
  (void)q_init( &ftm_tdscdma_ber_int_q);
  (void)q_init(&ftm_tdscdma_ber_int_free_q);

  for( i = 0; i < 10; i++)
  {
    q_put( &ftm_tdscdma_ber_int_free_q,
           q_link(  &ftm_tdscdma_ber_int_cmd_buf[i],
                    &ftm_tdscdma_ber_int_cmd_buf[i].cmd_hdr.link ));
  }

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
  ftm_tdscdma_dl_cctrch_info.cctrch_id = 0;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
  /* This table remaps the TFCS table.  This is used as follows:
   * 0xN( DTCH), 0xN( DCCH) => ctfc = 0 => has TFCI stored in ftm_tdscdma_dch_config.tfcs_config[0]
   * 0xN( DTCH), 1xN( DCCH) => ctfc = 2 => has TFCI stored in ftm_tdscdma_dch_config.tfcs_config[1]
   * 1xN( DTCH), 0xN( DCCH) => ctfc = 1 => has TFCI stored in ftm_tdscdma_dch_config.tfcs_config[2]
   * 1xN( DTCH), 1xN( DCCH) => ctfc = 3 => has TFCI stored in ftm_tdscdma_dch_config.tfcs_config[3]
   *
   * The user may use an FTM command to change the value of the TFCS->TFCI mapping.  The
   * default mapping is the same as defined in the 3GPP spec.  However, boxes may use 
   * a non-standard mapping so this can provide flexibility.  The same TFCS is used for UL and
   * DL
   */

  ftm_tdscdma_dch_config.ul_tfcs_config[0] = 0;
  ftm_tdscdma_dch_config.ul_tfcs_config[1] = 1;
  ftm_tdscdma_dch_config.ul_tfcs_config[2] = 2;
  ftm_tdscdma_dch_config.ul_tfcs_config[3] = 3;
  
  ftm_tdscdma_dch_config.dl_tfcs_config[0] = 0;
  ftm_tdscdma_dch_config.dl_tfcs_config[1] = 1;
  ftm_tdscdma_dch_config.dl_tfcs_config[2] = 2;
  ftm_tdscdma_dch_config.dl_tfcs_config[3] = 3;

  //link the ptrs
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
  ftm_tdscdma_dl_db.dl_phychan_db  = &ftm_tdscdma_dl_phychan_db;
  ftm_tdscdma_dl_db.dl_cctrch_info = &ftm_tdscdma_dl_cctrch_info;
  ftm_tdscdma_dl_db.dl_ctfc_info   = &ftm_tdscdma_dl_ctfc_info;
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
  #ifdef FTM_HAS_TDS_HSDPA
  ftm_tdshsdpa_bler_init();
  #endif

  #ifdef FTM_HAS_TDS_HSUPA
  ftm_tdscdma_hsupa_init();
  #endif
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_CLEANUP

DESCRIPTION    This function is used to force a teardown of the TDSCDMA 
               non-signaling mode.  If we are in DCH, or a DCH config is
               pending, then the RLC config must be torn down also.

               The IDLE request should automatically tear down HS channels
               if they are supported and enabled.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_cleanup( void)
{
  if (FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_NULL)//never visit tds ber
  {
    return;
  }

  FTM_MSG_ERROR("TDS NS: clean up. This function may take up to 100ms! "
   "Please stop NS before entering other modes.",0,0,0);

  /* Set L1 to to drop all */
  ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ();  
  /* wait for TDS L1 completion. usually around 20ms*/  
  DALSYS_BusyWait(30000);
  
  /* Issue Stop TDSCDMA mode command.*/
  ftm_tdscdma_ber_send_CPHY_STOP_TDSCDMA_MODE_REQ();
  /* wait for TDS L1 completion. usually around 50ms*/  
  DALSYS_BusyWait(60000);

}


#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
/*===========================================================================

FUNCTION      ftm_tdscdma_ber_send_CPHY_START_TDSCDMA_MODE_REQ

DESCRIPTION   Call TL1 API to send start cmd to TL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_START_TDSCDMA_MODE_REQ(void)
{
  boolean return_val = FALSE;
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM)))
  return_val = tdsl1_api_send_start_cmd();
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);
}


/*===========================================================================

FUNCTION     ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ

DESCRIPTION   Sends a CPHY_STOP_TDSCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ(void)
{
  tdsl1_api_cphy_setup_req_t msg;
  tdsl1_api_ftm_ns_cphy_setup_req_t *ftm_msg = &(msg.ftm_msg);
  boolean return_val = FALSE;

  ftm_msg->req_mask = TDSL1_NS_CPHY_DROP_ALL_INCL;
  ftm_msg->primary_freq = ftm_tdscdma_dl_db.current_rf_chan;
  ftm_msg->cell_parm_id = ftm_tdscdma_dch_config.cell_parm_id;
  ftm_msg->max_tx_pwr = ftm_tdscdma_dch_config.ul_max_tx_pwr;
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM)))
  return_val = tdsl1_api_send_setup_cmd(&msg);
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);
}

/*===========================================================================

FUNCTION     FTM_TDSCDMA_BER_SEND_CPHY_STOP_TDSCDMA_MODE_REQ

DESCRIPTION   Sends a CPHY_STOP_TDSCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_STOP_TDSCDMA_MODE_REQ(void)
{
  boolean return_val = FALSE;
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM)))
  return_val = tdsl1_api_send_stop_cmd();
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_CPHY_ACQ_REQ

DESCRIPTION    Sends a CPHY_ACQ_REQ primitive to WL1 with the passed in
               acquisition parameters.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_ACQ_REQ( ftm_tdscdma_ber_acquire_config_type *acq_config)
{
  tdsl1_api_acq_req_t acq_msg;
  boolean return_val = FALSE;

  acq_msg.freq = acq_config->freq;

  FTM_MSG_HIGH("TDS NS: Sending TDSL1_CPHY_ACQ_REQ to L1 with freq =%d ",acq_msg.freq ,0,0);
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM)))
  return_val = tdsl1_api_send_acq_cmd(&acq_msg);
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);
         }

           

/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_ULDL_CPHY_SETUP_REQ

DESCRIPTION    Sets up and configures WL1 for UL and DL physical channel
               setups.  Uses the static FTM TDSCDMA BER database for actual
               configuration parameters.


DEPENDENCIES
               FTM TDSCDMA database needs to be set up with L1 configuration.

===========================================================================*/
void ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ(void)
{
  tdsl1_api_cphy_setup_req_t msg;
  tdsl1_api_ftm_ns_cphy_setup_req_t *ftm_msg = &(msg.ftm_msg);
  boolean return_val = FALSE;

  ftm_msg->req_mask                  = ftm_tdscdma_dch_config.req_mask;
  ftm_msg->freq_incl                 = ftm_tdscdma_dch_config.freq_incl;
  ftm_msg->primary_freq              = ftm_tdscdma_dl_db.current_rf_chan;
  ftm_msg->cell_parm_id              = ftm_tdscdma_dch_config.cell_parm_id;
  ftm_msg->max_tx_pwr                = ftm_tdscdma_dch_config.ul_max_tx_pwr;
  ftm_msg->working_freq_incl         = ftm_tdscdma_dch_config.working_freq_incl;
  
  ftm_msg->rmc_setup                 = (tdsl1_ftm_ns_rmc_setup_type)ftm_tdscdma_dch_config.rmc_setup;             
  ftm_msg->dl_slot                   = ftm_tdscdma_dch_config.dl_slot;                 
  ftm_msg->ul_slot                   = ftm_tdscdma_dch_config.ul_slot;                 

  ftm_msg->ul_tpc_step               = ftm_tdscdma_dch_config.ul_tpc_step;           

  ftm_msg->dl_cctrch_id              = ftm_tdscdma_dl_cctrch_id;
  ftm_msg->working_freq              = ftm_tdscdma_dl_db.current_rf_chan;
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM)))
  return_val = tdsl1_api_send_setup_cmd(&msg);
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);
}

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
/*===========================================================================

FUNCTION      FTM_TDSCDMA_BER_SEND_CPHY_START_TDSCDMA_MODE_REQ

DESCRIPTION   Sends a CPHY_START_TDSCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_START_TDSCDMA_MODE_REQ(void)
{
  tdsl1_ext_cmd_type *cmd_ptr;

  /*Register L1-L3 command call back functions*/
  /*No de-registration needed*/
  //hyang: register these to avoid crashes if boot in ftm directly
  if(tdsl1_register_upper_layer_controller(TDSL1_CONTROLLER_RRC,
    &tdsrrc_get_l1_cmd_buf, 
    &tdsrrc_put_l1_cmd,
    &tdsrrc_malloc_for_external_cmd ) == FALSE)
  {
    ERR_FATAL("Could not register L1 call back function for L1-L3 commands",0,0,0);
  }

  cmd_ptr = tdsl1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id = TDSL1_CPHY_START_TDSCDMA_MODE_REQ;

  cmd_ptr->cmd_hdr.act_time_type = TDSL1_ACTIVATION_TIME_NONE;
  cmd_ptr->cmd_hdr.act_time      = 0;
  cmd_ptr->cmd.start.mdsp_image = TDSL1_MDSP_IMAGE_WCDMA_GSM; 

  FTM_MSG_HIGH("TDS NS: Sending TDSL1_CPHY_START_TDSCDMA_MODE_REQ to L1",0 ,0,0);

  tdsl1_put_cmd( cmd_ptr);
  
  /* Wait until L1 transitions to the idle state */
  while(tdsl1m_get_state() != TDSL1M_IDLE)
  {
    rex_yield( ); 
  }
  
  FTM_MSG_HIGH("TDS NS: Sent TDSL1_CPHY_START_TDSCDMA_MODE_REQ to L1",0 ,0,0);
}


/*===========================================================================

FUNCTION     ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ

DESCRIPTION   Sends a CPHY_STOP_TDSCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ(void)
{
  tdsl1_ext_cmd_type *cmd_ptr;

  cmd_ptr = tdsl1_get_cmd_buf();


  /* Fill in the Command Header */
  cmd_ptr->cmd_hdr.cmd_id                      = TDSL1_CPHY_SETUP_REQ;
  cmd_ptr->cmd_hdr.act_time_type               = TDSL1_ACTIVATION_TIME_NONE;
  cmd_ptr->cmd_hdr.act_time                    = 0;
  cmd_ptr->cmd_hdr.seq_num                     = 0;

  cmd_ptr->cmd.setup.req_mask                  = TDSL1_CPHY_DROP_ALL_INCL;
  cmd_ptr->cmd.setup.ho_setup_mask             = 0;
  cmd_ptr->cmd.setup.freq_incl                 = FALSE;

  cmd_ptr->cmd.setup.primary_freq              = ftm_tdscdma_dl_db.current_rf_chan;
  cmd_ptr->cmd.setup.cell_parm_id              = ftm_tdscdma_dch_config.cell_parm_id;
  cmd_ptr->cmd.setup.signal_ul_mac_at_act_time = FALSE;
	cmd_ptr->cmd.setup.signal_dl_mac_at_act_time = FALSE;
  cmd_ptr->cmd.setup.max_tx_pwr                = ftm_tdscdma_dch_config.ul_max_tx_pwr;

#ifdef FEATURE_TDSCDMA_DMO_SUPPORT
  cmd_ptr->cmd.setup.meas_occasion_info_incl   = 0;
#endif

  FTM_MSG_HIGH("TDS NS: Sending TDSL1_CPHY_SETUP_REQ (Drop All) to L1",0 ,0,0);

  tdsl1_put_cmd( cmd_ptr);
}

/*===========================================================================

FUNCTION     FTM_TDSCDMA_BER_SEND_CPHY_STOP_TDSCDMA_MODE_REQ

DESCRIPTION   Sends a CPHY_STOP_TDSCDMA_MODE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
   void ftm_tdscdma_ber_send_CPHY_STOP_TDSCDMA_MODE_REQ(void)
   {
     tdsl1_ext_cmd_type *cmd_ptr;
   
     cmd_ptr = tdsl1_get_cmd_buf();
   
   
     /* Fill in the Command Header */
     cmd_ptr->cmd_hdr.cmd_id = TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ;
     cmd_ptr->cmd_hdr.act_time_type = TDSL1_ACTIVATION_TIME_NONE;
     cmd_ptr->cmd_hdr.act_time      = 0;
     cmd_ptr->cmd.stop.cause     = TDSL1_RAT_CHANGE;
   
     FTM_MSG_HIGH("TDS NS: Sending TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ to L1",0 ,0,0);
   
     tdsl1_put_cmd( cmd_ptr);
   }


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_CPHY_IDLE_REQ

DESCRIPTION    Sends a CPHY_IDLE_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_IDLE_REQ(void)
{
    /* Copied from custtdscdma.h
    Send a CPHY_SETUP_REQ w/ Drop all chan flag to L1 to put it into SYNC state
     * which will still maintain the sync time of the camped cell.
     * RRC will no longer to send IDLE_REQ to L1 to send it to IDLE mode. 
     * The counter part L1 feature is FEATURE_TDSCDMA_SUPPORT_IDLE_REQ.
     */
#ifdef FEATURE_TDSCDMA_SUPPORT_IDLE_REQ

  tdsl1_ext_cmd_type *cmd_ptr;

  cmd_ptr = tdsl1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id = TDSL1_CPHY_IDLE_REQ;

  tdsl1_put_cmd( cmd_ptr);
#else
   FTM_MSG_ERROR("TDS NS: CPHY_IDLE_REQ not implemented for TDS. please use Drop All.",0,0,0);
#endif   
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_CPHY_ACQ_REQ

DESCRIPTION    Sends a CPHY_ACQ_REQ primitive to WL1 with the passed in
               acquisition parameters.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_ACQ_REQ( ftm_tdscdma_ber_acquire_config_type *acq_config)
{

  tdsl1_ext_cmd_type *cmd_ptr;

  cmd_ptr = tdsl1_get_cmd_buf();

 
      cmd_ptr->cmd_hdr.cmd_id = TDSL1_CPHY_ACQ_REQ;
      cmd_ptr->cmd_hdr.seq_num       = 0;
      cmd_ptr->cmd.acq.acq_mode = 0;
      cmd_ptr->cmd.acq.cpi = 0;
      cmd_ptr->cmd.acq.num_freqs = 1;
      cmd_ptr->cmd.acq.freq_list[0] =  acq_config->freq;

      FTM_MSG_HIGH( "TDS NS: acq Freq = %d, acq Mode = %d, cpi = %d", 
                   cmd_ptr->cmd.acq.freq_list[0],
                   cmd_ptr->cmd.acq.acq_mode,
                   cmd_ptr->cmd.acq.cpi );
      
   FTM_MSG_HIGH("TDS NS: Sending TDSL1_CPHY_ACQ_REQ to L1",0 ,0,0);
       
  tdsl1_put_cmd(cmd_ptr);
}

/*===========================================================================

FUNCTION     ftm_tdscdma_set_dl_cctrch_db_information

DESCRIPTION
             This function adds the cctrch

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_dl_cctrch_db_information( void)
{
  memset(&ftm_tdscdma_dl_cctrch_info, 0x00, sizeof(tdsl1dlcctrchcfg_cctrch_info_struct_type));
  memset(&ftm_tdscdma_dl_ctfc_info, 0x00, sizeof(tdsl1_dl_cctrchcfg_ctfc_info_struct_type));  

  ftm_tdscdma_dl_cctrch_info.num_trch = 2; 
  ftm_tdscdma_dl_cctrch_info.trch_enable_bf = 0x4; 
  ftm_tdscdma_dl_cctrch_info.cctrch_id = ftm_tdscdma_dl_cctrch_id;
  ftm_tdscdma_dl_cctrch_info.trch_ref_type_bf = 0x4;

  ftm_tdscdma_trch_info[0].ref_type = TDSL1_DL_TRCH_DCH_TYPE;
  ftm_tdscdma_trch_info[0].tti_value = TDSL1_TTI_20MS; 
  ftm_tdscdma_trch_info[0].crc_length = TDSL1_CRC_LENGTH_16;    
  ftm_tdscdma_trch_info[0].coding_type = TDSL1_DL_CONV_1_BY_3_CODING;
  ftm_tdscdma_trch_info[0].trch_id = 2; /* 10 */      
  ftm_tdscdma_trch_info[0].num_tf = 2; 
  ftm_tdscdma_trch_info[0].rate_match_attrib = 160; 
  ftm_tdscdma_trch_info[0].bler_quality_val = -127; // stuart       
  ftm_tdscdma_trch_info[0].loopback_mode = TDSL1_LOOPBACK_MODE_1;
  
  if(
   ( ftm_tdscdma_dch_config.rmc_setup == FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC)
   ||( ftm_tdscdma_dch_config.rmc_setup == FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC)
   )   
  {     
     ftm_tdscdma_tf_info[0][0].tb_size = 244;
     ftm_tdscdma_tf_info[0][0].num_tb  = 0;
     ftm_tdscdma_tf_info[0][1].tb_size = 244;
     ftm_tdscdma_tf_info[0][1].num_tb  = 1;
     ftm_tdscdma_tf_info[1][0].tb_size = 100;
     ftm_tdscdma_tf_info[1][0].num_tb  = 0;
     ftm_tdscdma_tf_info[1][1].tb_size = 100;
     ftm_tdscdma_tf_info[1][1].num_tb  = 1;
  }
  //hyang todo: add other rmc here
  else
  {
  ftm_tdscdma_tf_info[0][0].tb_size = 244;
  ftm_tdscdma_tf_info[0][0].num_tb  = 0;
  ftm_tdscdma_tf_info[0][1].tb_size = 244;
  ftm_tdscdma_tf_info[0][1].num_tb  = 1;
     ftm_tdscdma_tf_info[1][0].tb_size = 100;
     ftm_tdscdma_tf_info[1][0].num_tb  = 0;
     ftm_tdscdma_tf_info[1][1].tb_size = 100;
     ftm_tdscdma_tf_info[1][1].num_tb  = 1;

     FTM_MSG_ERROR("TDS NS: unsupported RMC setup %d",ftm_tdscdma_dch_config.rmc_setup,0,0);    
  }

  ftm_tdscdma_trch_info[0].tf_info_array[0] = &ftm_tdscdma_tf_info[0][0];  
  ftm_tdscdma_trch_info[0].tf_info_array[1] = &ftm_tdscdma_tf_info[0][1];  
  ftm_tdscdma_dl_cctrch_info.trch_info_array[0] = &ftm_tdscdma_trch_info[0];

  ftm_tdscdma_trch_info[1].ref_type = TDSL1_DL_TRCH_DCH_TYPE;
  ftm_tdscdma_trch_info[1].tti_value = TDSL1_TTI_40MS; 
  ftm_tdscdma_trch_info[1].crc_length = TDSL1_CRC_LENGTH_12;       
  ftm_tdscdma_trch_info[1].coding_type = TDSL1_DL_CONV_1_BY_3_CODING;
  ftm_tdscdma_trch_info[1].trch_id = 12;      
  ftm_tdscdma_trch_info[1].num_tf = 2; 
  ftm_tdscdma_trch_info[1].rate_match_attrib = 160; 
  ftm_tdscdma_trch_info[1].bler_quality_val = -127;  // stuart        
  ftm_tdscdma_trch_info[1].loopback_mode = TDSL1_LOOPBACK_MODE_1;//hyang
//  ftm_tdscdma_trch_info[1].loopback_mode = TDSL1_LOOPBACK_NA;//stuart
  ftm_tdscdma_trch_info[1].tf_info_array[0] = &ftm_tdscdma_tf_info[1][0];  
  ftm_tdscdma_trch_info[1].tf_info_array[1] = &ftm_tdscdma_tf_info[1][1];    
  ftm_tdscdma_dl_cctrch_info.trch_info_array[1] = &ftm_tdscdma_trch_info[1];    

  ftm_tdscdma_dl_ctfc_info.num_ctfc_entry = 4;
  ftm_tdscdma_dl_ctfc_info.ctfc_entry_array[0] = &ftm_tdscdma_ctfc_entry_array[0];  
  ftm_tdscdma_dl_ctfc_info.ctfc_entry_array[1] = &ftm_tdscdma_ctfc_entry_array[1];  
  ftm_tdscdma_dl_ctfc_info.ctfc_entry_array[2] = &ftm_tdscdma_ctfc_entry_array[2];  
  ftm_tdscdma_dl_ctfc_info.ctfc_entry_array[3] = &ftm_tdscdma_ctfc_entry_array[3];  

  ftm_tdscdma_ctfc_entry_array[0].valid_ctfc = TRUE;  
  ftm_tdscdma_ctfc_entry_array[0].tfi_table = &ftm_tdscdma_tfi_table[0][0];
  ftm_tdscdma_tfi_table[0][0] = 0;
  ftm_tdscdma_tfi_table[0][1] = 0;
  
  ftm_tdscdma_ctfc_entry_array[1].valid_ctfc = TRUE;
  ftm_tdscdma_ctfc_entry_array[1].tfi_table = &ftm_tdscdma_tfi_table[1][0];
  ftm_tdscdma_tfi_table[1][0] = 0;
  ftm_tdscdma_tfi_table[1][1] = 1;

  ftm_tdscdma_ctfc_entry_array[2].valid_ctfc = TRUE;
  ftm_tdscdma_ctfc_entry_array[2].tfi_table = &ftm_tdscdma_tfi_table[2][0];
  ftm_tdscdma_tfi_table[2][0] = 1;
  ftm_tdscdma_tfi_table[2][1] = 0;

  ftm_tdscdma_ctfc_entry_array[3].valid_ctfc = TRUE;
  ftm_tdscdma_ctfc_entry_array[3].tfi_table = &ftm_tdscdma_tfi_table[3][0];
  ftm_tdscdma_tfi_table[3][0] = 1;
  ftm_tdscdma_tfi_table[3][1] = 1;
}   

/*===========================================================================

FUNCTION     ftm_tdscdma_set_dl_cctrch_db_information

DESCRIPTION
             This function adds the cctrch

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_dl_cctrch_db_information_8820C_UPA( void)
{
  memset(&ftm_tdscdma_dl_cctrch_info, 0x00, sizeof(tdsl1dlcctrchcfg_cctrch_info_struct_type));
  memset(&ftm_tdscdma_dl_ctfc_info, 0x00, sizeof(tdsl1_dl_cctrchcfg_ctfc_info_struct_type));

  ftm_tdscdma_dl_cctrch_info.num_trch = 1;
  ftm_tdscdma_dl_cctrch_info.trch_enable_bf = 0x4;
  ftm_tdscdma_dl_cctrch_info.cctrch_id = ftm_tdscdma_dl_cctrch_id;
  ftm_tdscdma_dl_cctrch_info.trch_ref_type_bf = 0x4;

  ftm_tdscdma_trch_info[0].ref_type = TDSL1_DL_TRCH_DCH_TYPE;
  ftm_tdscdma_trch_info[0].tti_value = TDSL1_TTI_40MS;
  ftm_tdscdma_trch_info[0].crc_length = TDSL1_CRC_LENGTH_12;
  ftm_tdscdma_trch_info[0].coding_type = TDSL1_DL_CONV_1_BY_3_CODING;
  ftm_tdscdma_trch_info[0].trch_id = 2; /* 10 */
  ftm_tdscdma_trch_info[0].num_tf = 2;
  ftm_tdscdma_trch_info[0].rate_match_attrib = 160;
  ftm_tdscdma_trch_info[0].bler_quality_val = -127; // stuart
  ftm_tdscdma_trch_info[0].loopback_mode = TDSL1_LOOPBACK_MODE_1;


  ftm_tdscdma_tf_info[0][0].tb_size = 100;
  ftm_tdscdma_tf_info[0][0].num_tb  = 0;
  ftm_tdscdma_tf_info[0][1].tb_size = 100;
  ftm_tdscdma_tf_info[0][1].num_tb  = 1;


  ftm_tdscdma_trch_info[0].tf_info_array[0] = &ftm_tdscdma_tf_info[0][0];
  ftm_tdscdma_trch_info[0].tf_info_array[1] = &ftm_tdscdma_tf_info[0][1];
  ftm_tdscdma_dl_cctrch_info.trch_info_array[0] = &ftm_tdscdma_trch_info[0];

  ftm_tdscdma_dl_ctfc_info.num_ctfc_entry = 2;
  ftm_tdscdma_dl_ctfc_info.ctfc_entry_array[0] = &ftm_tdscdma_ctfc_entry_array[0];
  ftm_tdscdma_dl_ctfc_info.ctfc_entry_array[1] = &ftm_tdscdma_ctfc_entry_array[1];


  ftm_tdscdma_ctfc_entry_array[0].valid_ctfc = TRUE;
  ftm_tdscdma_ctfc_entry_array[0].tfi_table = &ftm_tdscdma_tfi_table[0][0];
  ftm_tdscdma_tfi_table[0][0] = 0;

  ftm_tdscdma_ctfc_entry_array[1].valid_ctfc = TRUE;
  ftm_tdscdma_ctfc_entry_array[1].tfi_table = &ftm_tdscdma_tfi_table[1][0];
  ftm_tdscdma_tfi_table[1][0] = 1;

}
/*===========================================================================

FUNCTION     tdsutils_set_dl_phychan_db_information

DESCRIPTION
    This function add internal pccpch .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_dl_phychan_db_information( 
  tdsl1_dl_phychan_enum_type phych_type,
  uint8 cctrch_id)
{
   switch(phych_type)
   {
      case TDSL1_DL_PHYCHAN_PCCPCH_S:
         {
           FTM_MSG_MED("TDS NS: set TDSL1_DL_PHYCHAN_PCCPCH_S.",0,0,0);
           ftm_tdscdma_dl_phychan_db.pccpch.tstd_indicator = FALSE;

              /* [Yong Hou] sib_sfn_bit_mask in PCCPCH configuration
             It is a pointer to an array of 512 bytes (or 4096 bits)
             to handle all 4096 SFNs. When BCH setup, 
             this bit array identifies SFN where RRC wants to receive SIB data.  
             If bit is 1, it means L1SW need receive that frame.  
             If bit is 0, it means L1SW need skip it.  
             L1SW can use this information to sleep between SIB frames and 
             conserve power. If you want to simulate this table, 
             you should define an array of 512 bytes.Now, in QTF test code, 
             I set it to NULL.*/
           ftm_tdscdma_dl_phychan_db.pccpch.sib_sfn_bit_mask = NULL; //fake 64 bits all 1s             
         }
         break;
         
      case TDSL1_DL_PHYCHAN_PICH:
        {
           FTM_MSG_MED("TDS NS: set TDSL1_DL_PHYCHAN_PICH.",0,0,0);  
           ftm_tdscdma_dl_phychan_db.pich.ts_num = 0;/* TDSL1_TIMESLOT0 */
           ftm_tdscdma_dl_phychan_db.pich.ch_code_bitmap = 3; /*3 */
           ftm_tdscdma_dl_phychan_db.pich.midamble_and_burst.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;/* TDSL1_DEFAULT_MIDAMBLE */
           ftm_tdscdma_dl_phychan_db.pich.midamble_and_burst.config = TDSL1_MIDAMBLE_CONFIG_4; /*TDSL1_MIDAMBLE_CONFIG_4*/
           ftm_tdscdma_dl_phychan_db.pich.midamble_and_burst.shift = 0; /*0*/
           ftm_tdscdma_dl_phychan_db.pich.repetition_period_length = TDSL1_PICH_REPEAT_16_2;/*TDSL1_REPETITION_16_2*/;
           ftm_tdscdma_dl_phychan_db.pich.offset = 0; /* 0 */
           ftm_tdscdma_dl_phychan_db.pich.paging_indicator_length = TDSL1_DL_PI_LEMGTH_8;/* TDSL1DLPHYCFG_PI_LEMGTH_8 */
           ftm_tdscdma_dl_phychan_db.pich.num_paging_group = 0;//hyang??
           // Need to check with the rrc team; update this value then
           ftm_tdscdma_dl_phychan_db.pich.imsi_div_8192 = 2530;
         }
         break;

      case TDSL1_DL_PHYCHAN_SCCPCH0:
         {       
           FTM_MSG_ERROR("TDS NS: Not supported CPHY_SETUP_REQ cmd to L1 for SCCPCH phychan add.",0,0,0);   
             
         }
         break;

      case TDSL1_DL_PHYCHAN_DPCH:
         {
           FTM_MSG_MED("TDS NS: set TDSL1_DL_PHYCHAN_DPCH.",0,0,0);   
           ftm_tdscdma_dl_phychan_db.dpch.opt_parm_presence_bitmask = 0x7; /*0x7*/
           ftm_tdscdma_dl_phychan_db.dpch.doff = 0;
           
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.opt_parm_present_mask = 0x1;    
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.tstd_indicator = TRUE;
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.tpc_step_size = ftm_tdscdma_dch_config.ul_tpc_step;
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.timing_ind = TDSL1_DL_PHYCFG_TIMING_IND_DPCH_ESTABLISH;//stuartk

           // this should be the same as the cctrch id ( check )
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.tfcs_id = cctrch_id;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.time_info.activation_time = 0; /* 206 */
           
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.punct_limit = 60;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.repetition_length = 0;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.repetition_period = 1;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.second_interleaving_mode = TDSL1_SEC_INTLV_FRAME_BASED;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.tfci_coding = 16;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.num_ts = 1; /* 1 */

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].tfci_exists = TRUE;
            
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].additional_ss_tpc_symbols = 0;
            
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].ts_num = ftm_tdscdma_dch_config.dl_slot;
            
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].ss_tpc_symbols = TDSL1_SS_TPC_SYMBOL_1 ;
                        
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].midamble_ss_burst_type.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].midamble_ss_burst_type.config = TDSL1_MIDAMBLE_CONFIG_16;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].midamble_ss_burst_type.shift = 15;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].chan_code_bitmap = 3; 

           ftm_tdscdma_dl_phychan_db.sync_det.N312 = ftm_tdscdma_dch_config.n_312;
           ftm_tdscdma_dl_phychan_db.sync_det.N313 = ftm_tdscdma_dch_config.n_313;
           ftm_tdscdma_dl_phychan_db.sync_det.N315 = ftm_tdscdma_dch_config.n_315;
#ifdef FEATURE_SYNC_DET_TIMER
           ftm_tdscdma_dl_phychan_db.sync_det.T312 = ftm_tdscdma_dch_config.t_312;
           ftm_tdscdma_dl_phychan_db.sync_det.T313 = ftm_tdscdma_dch_config.t_313;
#endif
         }
         break;

   case TDSL1_DL_PHYCHAN_PCCPCH_N:
         {
           FTM_MSG_MED("TDS NS: Set TDSL1_DL_PHYCHAN_PCCPCH_N",0,0,0);
           ftm_tdscdma_dl_phychan_db.pccpch.tstd_indicator = FALSE;
    
           /* [Yong Hou] sib_sfn_bit_mask in PCCPCH configuration
           It is a pointer to an array of 512 bytes (or 4096 bits)
           to handle all 4096 SFNs. When BCH setup, 
           this bit array identifies SFN where RRC wants to receive SIB data.  
           If bit is 1, it means L1SW need receive that frame.  
           If bit is 0, it means L1SW need skip it.  
           L1SW can use this information to sleep between SIB frames and 
           conserve power. If you want to simulate this table, 
           you should define an array of 512 bytes.Now, in QTF test code, 
           I set it to NULL.*/
           ftm_tdscdma_dl_phychan_db.pccpch.sib_sfn_bit_mask = NULL; //fake 64 bits all 1s         
         }
         break;
      
   default:
     FTM_MSG_ERROR("TDS NS DL CONFIG: invalid physical channel type!\n",0,0,0);
     break;         
        
   }
}

/*===========================================================================

FUNCTION     tdsutils_set_dl_phychan_db_information

DESCRIPTION
    This function add internal pccpch .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_dl_phychan_db_information_8820C_UPA(
  tdsl1_dl_phychan_enum_type phych_type,
  uint8 cctrch_id)
{
   switch(phych_type)
   {
      case TDSL1_DL_PHYCHAN_DPCH:
         {
           FTM_MSG_MED("TDS NS: set TDSL1_DL_PHYCHAN_DPCH for 8820C.",0,0,0);
           ftm_tdscdma_dl_phychan_db.dpch.opt_parm_presence_bitmask = 0x7; /*0x7*/
           ftm_tdscdma_dl_phychan_db.dpch.doff = 0;

           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.opt_parm_present_mask = 0x1;
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.tstd_indicator = FALSE;
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.tpc_step_size = ftm_tdscdma_dch_config.ul_tpc_step;
           ftm_tdscdma_dl_phychan_db.dpch.dpch_common.timing_ind = TDSL1_DL_PHYCFG_TIMING_IND_DPCH_ESTABLISH;//stuartk

           // this should be the same as the cctrch id ( check )
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.tfcs_id = cctrch_id;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.time_info.activation_time = 0; /* 206 */

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.punct_limit = 100;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.repetition_length = 0;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.repetition_period = 1;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.second_interleaving_mode = TDSL1_SEC_INTLV_FRAME_BASED;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.common_ts_info.tfci_coding = 8;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.num_ts = 1; /* 1 */

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].tfci_exists = TRUE;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].additional_ss_tpc_symbols = 0;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].ts_num = ftm_tdscdma_dch_config.dl_slot;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].ss_tpc_symbols = TDSL1_SS_TPC_SYMBOL_1 ;

           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].midamble_ss_burst_type.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].midamble_ss_burst_type.config = TDSL1_MIDAMBLE_CONFIG_8;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].midamble_ss_burst_type.shift = 15;
           ftm_tdscdma_dl_phychan_db.dpch.dl_cctrch_list.individual_ts_info[0].chan_code_bitmap = 0x1000;  //cc16-13

           ftm_tdscdma_dl_phychan_db.sync_det.N312 = ftm_tdscdma_dch_config.n_312;
           ftm_tdscdma_dl_phychan_db.sync_det.N313 = ftm_tdscdma_dch_config.n_313;
           ftm_tdscdma_dl_phychan_db.sync_det.N315 = ftm_tdscdma_dch_config.n_315;
#ifdef FEATURE_SYNC_DET_TIMER
           ftm_tdscdma_dl_phychan_db.sync_det.T312 = ftm_tdscdma_dch_config.t_312;
           ftm_tdscdma_dl_phychan_db.sync_det.T313 = ftm_tdscdma_dch_config.t_313;
#endif
         }
         break;

   default:
     FTM_MSG_ERROR("TDS NS DL CONFIG: invalid physical channel type!\n",0,0,0);
     break;
   }
}


/*===========================================================================

FUNCTION     ftm_tdscdma_set_ul_cctrch_db_information

DESCRIPTION
             This function adds the cctrch

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_ul_cctrch_db_information( void)
{
  uint16 i;

  tdsl1_ul_cctrch_cfg_struct_type *cctrch = &ftm_tdscdma_ul_cctrch_cfg;
  tdsmac_ul_tfcs_type             *tfcs   = &ftm_tdscdma_ul_mac_tfcs;

/* UL CCTRCH configuration */
//  cctrch->num_trch                            = 2;

  cctrch->chan            = TDSL1_UL_PHYCHAN_DPCH;
  cctrch->filler_val      = TDSL1_UL_RF_EQ_FILLER_0;

  cctrch->semi_static_parms_ptr[0]->trch_id          = 1;
  cctrch->semi_static_parms_ptr[0]->staticRM         = 160;
  cctrch->semi_static_parms_ptr[0]->coding           = TDSL1_CONV_1_BY_3_CODING;
  cctrch->semi_static_parms_ptr[0]->crc              = TDSL1_CRC_LENGTH_16;

  cctrch->semi_static_parms_ptr[1]->trch_id          = 12;
  cctrch->semi_static_parms_ptr[1]->staticRM         = 160;
  cctrch->semi_static_parms_ptr[1]->coding           = TDSL1_CONV_1_BY_3_CODING;
  cctrch->semi_static_parms_ptr[1]->crc              = TDSL1_CRC_LENGTH_12;

  cctrch->gain_factor_parms_ptr[0]->algo             = TDSL1_UL_GF_NONE;
  cctrch->gain_factor_parms_ptr[0]->ref_tfc_id       = 0;
  cctrch->gain_factor_parms_ptr[0]->ref_tfc_id_valid = FALSE;
  cctrch->gain_factor_parms_ptr[0]->BetaD            = ftm_tdscdma_dch_config.beta_d;

  cctrch->gain_factor_parms_ptr[1]->algo             = TDSL1_UL_GF_NONE;
  cctrch->gain_factor_parms_ptr[1]->ref_tfc_id       = 0;
  cctrch->gain_factor_parms_ptr[1]->ref_tfc_id_valid = TRUE;
  cctrch->gain_factor_parms_ptr[1]->BetaD            = ftm_tdscdma_dch_config.beta_d;

  cctrch->gain_factor_parms_ptr[2]->algo             = TDSL1_UL_GF_NONE;
  cctrch->gain_factor_parms_ptr[2]->ref_tfc_id       = 0;
  cctrch->gain_factor_parms_ptr[2]->ref_tfc_id_valid = FALSE;
  cctrch->gain_factor_parms_ptr[2]->BetaD            = ftm_tdscdma_dch_config.beta_d;

  cctrch->gain_factor_parms_ptr[3]->algo             = TDSL1_UL_GF_SIGNALLED;
  cctrch->gain_factor_parms_ptr[3]->ref_tfc_id       = 0;
  cctrch->gain_factor_parms_ptr[3]->ref_tfc_id_valid = FALSE;
  cctrch->gain_factor_parms_ptr[3]->BetaD            = ftm_tdscdma_dch_config.beta_d;

  cctrch->ref_tfc_to_real_tfc[0]                     = 0xFF;
  cctrch->ref_tfc_to_real_tfc[1]                     = 0xFF;
  cctrch->ref_tfc_to_real_tfc[2]                     = 0xFF;
  cctrch->ref_tfc_to_real_tfc[3]                     = 0xFF;

/* UL TFCS configuration */

  tfcs->nchan = 2;
  tfcs->ntfc  = 4;

  tfcs->tfs_info[0].tti_info           = TDSL1_TTI_20MS;
  tfcs->tfs_info[0].ntf                = 2;

  tfcs->tfs_info[0].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[0].tf_info[1].ntblks  = 1;

  tfcs->tfs_info[1].tti_info           = TDSL1_TTI_40MS;
  tfcs->tfs_info[1].ntf                = 2;
  tfcs->tfs_info[1].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[1].tf_info[1].ntblks  = 1;

  if(
   ( ftm_tdscdma_dch_config.rmc_setup == FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC)
   ||( ftm_tdscdma_dch_config.rmc_setup == FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC)
   )   
  {
    tfcs->tfs_info[0].tf_info[0].tb_size = 244;
    tfcs->tfs_info[0].tf_info[1].tb_size = 244;

    tfcs->tfs_info[1].tf_info[0].tb_size = 96;
    tfcs->tfs_info[1].tf_info[1].tb_size = 96;
  }
  //hyang todo: add other rmc here
  else
  {
    tfcs->tfs_info[0].tf_info[0].tb_size = 244;
    tfcs->tfs_info[0].tf_info[1].tb_size = 244;

    tfcs->tfs_info[1].tf_info[0].tb_size = 96;
    tfcs->tfs_info[1].tf_info[1].tb_size = 96;

     FTM_MSG_ERROR("TDS NS: unsupported RMC setup %d",ftm_tdscdma_dch_config.rmc_setup,0,0);    
  }

  /* Flag all transport format combinations as invalid, and setup the
  ** ones we want afterwards. */
  for (i = 0; i < TDSUE_MAX_TFC; i++)
  {
    tfcs->tfc_info[i].control_flag = TDSTFC_NOT_ALLOWED;
  }

  tfcs->tfc_info[0].control_flag = TDSTFC_ALLOWED; 
  tfcs->tfc_info[0].ctfc         = 0;  
  tfcs->tfc_info[0].tfi[0]       = 0;
  tfcs->tfc_info[0].tfi[1]       = 0;
  tfcs->tfc_info[1].control_flag = TDSTFC_ALLOWED;
  tfcs->tfc_info[1].ctfc         = 1;  
  tfcs->tfc_info[1].tfi[0]       = 0;
  tfcs->tfc_info[1].tfi[1]       = 1;
  tfcs->tfc_info[2].control_flag = TDSTFC_ALLOWED;  
  tfcs->tfc_info[2].ctfc         = 2;  
  tfcs->tfc_info[2].tfi[0]       = 1;
  tfcs->tfc_info[2].tfi[1]       = 0;
  tfcs->tfc_info[3].control_flag = TDSTFC_ALLOWED;
  tfcs->tfc_info[3].ctfc         = 3;  
  tfcs->tfc_info[3].tfi[0]       = 1;
  tfcs->tfc_info[3].tfi[1]       = 1;
 
}   


/*===========================================================================

FUNCTION     ftm_tdscdma_set_ul_cctrch_db_information

DESCRIPTION
             This function adds the cctrch

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_ul_cctrch_db_information_8820C_UPA( void)
{
  uint16 i;

  tdsl1_ul_cctrch_cfg_struct_type *cctrch = &ftm_tdscdma_ul_cctrch_cfg;
  tdsmac_ul_tfcs_type             *tfcs   = &ftm_tdscdma_ul_mac_tfcs;

/* UL CCTRCH configuration */
//  cctrch->num_trch                            = 2;

  cctrch->chan            = TDSL1_UL_PHYCHAN_DPCH;
  cctrch->filler_val      = TDSL1_UL_RF_EQ_FILLER_0;

  cctrch->semi_static_parms_ptr[0]->trch_id          = 1;
  cctrch->semi_static_parms_ptr[0]->staticRM         = 160;
  cctrch->semi_static_parms_ptr[0]->coding           = TDSL1_CONV_1_BY_2_CODING;
  cctrch->semi_static_parms_ptr[0]->crc              = TDSL1_CRC_LENGTH_12;

  cctrch->gain_factor_parms_ptr[0]->algo             = TDSL1_UL_GF_NONE;
  cctrch->gain_factor_parms_ptr[0]->ref_tfc_id       = 0;
  cctrch->gain_factor_parms_ptr[0]->ref_tfc_id_valid = FALSE;
  cctrch->gain_factor_parms_ptr[0]->BetaD            = ftm_tdscdma_dch_config.beta_d;

  cctrch->gain_factor_parms_ptr[1]->algo             = TDSL1_UL_GF_NONE;
  cctrch->gain_factor_parms_ptr[1]->ref_tfc_id       = 0;
  cctrch->gain_factor_parms_ptr[1]->ref_tfc_id_valid = TRUE;
  cctrch->gain_factor_parms_ptr[1]->BetaD            = ftm_tdscdma_dch_config.beta_d;


  cctrch->ref_tfc_to_real_tfc[0]                     = 0xFF;
  cctrch->ref_tfc_to_real_tfc[1]                     = 0xFF;
  cctrch->ref_tfc_to_real_tfc[2]                     = 0xFF;
  cctrch->ref_tfc_to_real_tfc[3]                     = 0xFF;

/* UL TFCS configuration */

  tfcs->nchan = 1;
  tfcs->ntfc  = 2;

  tfcs->tfs_info[0].tti_info           = TDSL1_TTI_40MS;
  tfcs->tfs_info[0].ntf                = 2;

  tfcs->tfs_info[0].tf_info[0].ntblks  = 0;
  tfcs->tfs_info[0].tf_info[1].ntblks  = 1;

    tfcs->tfs_info[0].tf_info[0].tb_size = 96;
    tfcs->tfs_info[0].tf_info[1].tb_size = 96;


  /* Flag all transport format combinations as invalid, and setup the
  ** ones we want afterwards. */
  for (i = 0; i < TDSUE_MAX_TFC; i++)
  {
    tfcs->tfc_info[i].control_flag = TDSTFC_NOT_ALLOWED;
  }

  tfcs->tfc_info[0].control_flag = TDSTFC_ALLOWED;
  tfcs->tfc_info[0].ctfc         = 0;
  tfcs->tfc_info[0].tfi[0]       = 0;

  tfcs->tfc_info[1].control_flag = TDSTFC_ALLOWED;
  tfcs->tfc_info[1].ctfc         = 1;
  tfcs->tfc_info[1].tfi[0]       = 1;


}

/*===========================================================================

FUNCTION     tdsutils_set_ul_phychan_db_information

DESCRIPTION
    This function add internal dpch.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_ul_phychan_db_information(tdsl1_ul_phychan_enum_type phych_type)
{
   uint8 i,j;
   tdsl1_ul_phych_cfg_struct_type *phychan = &ftm_tdscdma_ul_phychan_cfg;
  
   switch(phych_type)
   {
      case TDSL1_UL_PHYCHAN_DPCH:
         {
           FTM_MSG_MED("TDS NS: set TDSL1_UL_PHYCHAN_DPCH",0,0,0);  
           phychan->dpch_cfg.act_time = 0;
           phychan->dpch_cfg.beacon_pl_est = FALSE;

           phychan->dpch_cfg.common_ts_info.punct_limit = 60;
           phychan->dpch_cfg.common_ts_info.repetition_length = 0;
           phychan->dpch_cfg.common_ts_info.repetition_period = 1;
           phychan->dpch_cfg.common_ts_info.second_interleaving_mode = TDSL1_SEC_INTLV_FRAME_BASED;
           phychan->dpch_cfg.common_ts_info.tfci_coding = 16;


           phychan->dpch_cfg.dynamicSfUse = FALSE;
           phychan->dpch_cfg.num_ts = 1;

           /* Leave "for" loop for futher */
           for(i = 0; i < phychan->dpch_cfg.num_ts; i++)
           {
             phychan->dpch_cfg.ts_list[i].ts_num = ftm_tdscdma_dch_config.ul_slot;
             phychan->dpch_cfg.ts_list[i].tfciExist = 1;
             phychan->dpch_cfg.ts_list[i].ss_tpc_symbols = TDSL1_SS_TPC_SYMBOL_1;
             phychan->dpch_cfg.ts_list[i].add_ss_tpc_symbols = 0;
             phychan->dpch_cfg.ts_list[i].modType = TDSL1_MOD_QPSK;

             phychan->dpch_cfg.ts_list[i].midamble_and_burst.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
             phychan->dpch_cfg.ts_list[i].midamble_and_burst.config = TDSL1_MIDAMBLE_CONFIG_16;
             phychan->dpch_cfg.ts_list[i].midamble_and_burst.shift = 0;

             if( ftm_tdscdma_dch_config.rmc_setup == FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC)
             {
               phychan->dpch_cfg.ts_list[i].num_chan_codes = 2;
               for(j = 0; j < phychan->dpch_cfg.ts_list[i].num_chan_codes; j++)
               {
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].chan_code = j + 1;
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].sf = TDSL1_SF_16;
               }             
             }
             else if ( ftm_tdscdma_dch_config.rmc_setup == FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC)
             {
               phychan->dpch_cfg.ts_list[i].num_chan_codes = 1;
               for(j = 0; j < phychan->dpch_cfg.ts_list[i].num_chan_codes; j++)
               {
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].chan_code = j + 1;
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].sf = TDSL1_SF_8;
               }
             }
             //hyang todo: add other rmc here
             else
             {
               phychan->dpch_cfg.ts_list[i].num_chan_codes = 1;
               for(j = 0; j < phychan->dpch_cfg.ts_list[i].num_chan_codes; j++)
               {
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].chan_code = j + 1;
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].sf = TDSL1_SF_8;
               }
               FTM_MSG_ERROR("TDS NS: unsupported RMC setup %d",ftm_tdscdma_dch_config.rmc_setup,0,0);
             }
           }

           phychan->dpch_cfg.pccpch_tx_pwr = 24;

           phychan->dpch_cfg.prx_dpch_des = -112; // stuart

           phychan->dpch_cfg.tpc_step_size = ftm_tdscdma_dch_config.ul_tpc_step;

           phychan->dpch_cfg.ul_sync_freq = 0;

           phychan->dpch_cfg.ul_sync_parm_present = 0;

           phychan->dpch_cfg.ul_timing_advance_ctrl_enabled = TRUE;
           phychan->dpch_cfg.ul_timing_advance_ctrl_present = TRUE;
           phychan->dpch_cfg.ul_sync_step_size              = 1;
           phychan->dpch_cfg.ul_sync_freq                   = 1;
         }
         break;


   default:
     FTM_MSG_ERROR("TDS NS DL CONFIG: invalid physical channel type!\n",0,0,0);
     break;

   }
}

/*===========================================================================

FUNCTION     ftm_tdscdma_set_ul_phychan_db_information_8820C_UPA

DESCRIPTION
    This function add internal dpch.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_ul_phychan_db_information_8820C_UPA(tdsl1_ul_phychan_enum_type phych_type)
{
   uint8 i,j;
   tdsl1_ul_phych_cfg_struct_type *phychan = &ftm_tdscdma_ul_phychan_cfg;

   switch(phych_type)
   {
      case TDSL1_UL_PHYCHAN_DPCH:
         {
           FTM_MSG_MED("TDS NS: set TDSL1_UL_PHYCHAN_DPCH for 8820C",0,0,0);
           phychan->dpch_cfg.act_time = 0;
           phychan->dpch_cfg.beacon_pl_est = FALSE;

           phychan->dpch_cfg.common_ts_info.punct_limit = 100;
           phychan->dpch_cfg.common_ts_info.repetition_length = 0;
           phychan->dpch_cfg.common_ts_info.repetition_period = 1;
           phychan->dpch_cfg.common_ts_info.second_interleaving_mode = TDSL1_SEC_INTLV_FRAME_BASED;
           phychan->dpch_cfg.common_ts_info.tfci_coding = 8;


           phychan->dpch_cfg.dynamicSfUse = FALSE;
           phychan->dpch_cfg.num_ts = 1;

           /* Leave "for" loop for futher */
           for(i = 0; i < phychan->dpch_cfg.num_ts; i++)
           {
             phychan->dpch_cfg.ts_list[i].ts_num = ftm_tdscdma_dch_config.ul_slot;
             phychan->dpch_cfg.ts_list[i].tfciExist = 1;
             phychan->dpch_cfg.ts_list[i].ss_tpc_symbols = TDSL1_SS_TPC_SYMBOL_1;
             phychan->dpch_cfg.ts_list[i].add_ss_tpc_symbols = 0;
             phychan->dpch_cfg.ts_list[i].modType = TDSL1_MOD_QPSK;

             phychan->dpch_cfg.ts_list[i].midamble_and_burst.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
             phychan->dpch_cfg.ts_list[i].midamble_and_burst.config = TDSL1_MIDAMBLE_CONFIG_8;
             phychan->dpch_cfg.ts_list[i].midamble_and_burst.shift = 0;

               phychan->dpch_cfg.ts_list[i].num_chan_codes = 1;
               for(j = 0; j < phychan->dpch_cfg.ts_list[i].num_chan_codes; j++)
               {
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].chan_code = j + 1;
                 phychan->dpch_cfg.ts_list[i].chan_code_list[j].sf = TDSL1_SF_16;
               }

           }

           phychan->dpch_cfg.pccpch_tx_pwr = 24;

           phychan->dpch_cfg.prx_dpch_des = -112; // stuart

           phychan->dpch_cfg.tpc_step_size = ftm_tdscdma_dch_config.ul_tpc_step;

           phychan->dpch_cfg.ul_sync_freq = 0;

           phychan->dpch_cfg.ul_sync_parm_present = 0;

           phychan->dpch_cfg.ul_timing_advance_ctrl_enabled = TRUE;
           phychan->dpch_cfg.ul_timing_advance_ctrl_present = TRUE;
           phychan->dpch_cfg.ul_sync_step_size              = 1;
           phychan->dpch_cfg.ul_sync_freq                   = 1;
         }
         break;

      
   default:
     FTM_MSG_ERROR("TDS NS DL CONFIG: invalid physical channel type!\n",0,0,0);
     break;         
        
   }
}


/*===========================================================================

FUNCTION     ftm_tdscdma_ber_set_ul_tx_data

DESCRIPTION
    This function sets Uplink TX data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_ber_set_ul_tx_data (tdsl1_dl_tb_data_struct_type *dl_data_ptr)
{
    uint8 i,j,k;
   ftm_tdscdma_ber_ul_tx_data_type *tx_data_ptr = &ftm_tdscdma_ber_ul_tx_data;

   switch( ftm_tdscdma_dch_config.rmc_setup )
   {
          
      case FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC:
      case FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC:
      {
        tx_data_ptr->TFCI     = 2;
        tx_data_ptr->num_trch = 1;
        tx_data_ptr->mac_busy = FALSE;
          i = 0;

          tx_data_ptr->tx_data[i].tti = TDSL1_TTI_20MS;
          tx_data_ptr->tx_data[i].num_tb  = 1;
          tx_data_ptr->tx_data[i].tb_size = 244;
            j = 0;
            if( tx_data_ptr->tx_data[i].tb_size == dl_data_ptr->tb_set_data[j].trblk_size)
            {
              tx_data_ptr->tx_data[i].trch_data[j].tb_data = dl_data_ptr->tb_set_data[j].tb_data_info->tb_data_buffer;
            }
            else
            {
              if( dl_data_ptr->tb_set_data[j].tb_data_info->tb_data_buffer != NULL )
              {
                (void)dsm_free_buffer(dl_data_ptr->tb_set_data[j].tb_data_info->tb_data_buffer);
              }

              tx_data_ptr->num_trch = 0;
            }

            FTM_MSG_MED( "TDS NS: ul_tx_data_config_cmd_proc - trch = %d , tti = %d, num_tb = %d ", 
                      i, tx_data_ptr->tx_data[i].tti, tx_data_ptr->tx_data[i].num_tb); 

            tx_data_ptr->tx_data[i].trch_data[j].mac_hdr_size = 0;
            for (k = 0; k < ((tx_data_ptr->tx_data[i].trch_data[j].mac_hdr_size + 7)/8); k++ )
            {
              tx_data_ptr->tx_data[i].trch_data[j].mac_hdr[k] = 0;
            }

            /* Ciphering */
            tx_data_ptr->tx_data[i].trch_data[j].ciphering_on = FALSE;
      }
      break;

      case FTM_TDSCDMA_BER_RMC_TYPE_64:
      case FTM_TDSCDMA_BER_RMC_TYPE_144:
      case FTM_TDSCDMA_BER_RMC_TYPE_384:
      
      default:
        FTM_MSG_ERROR("TDS NS DL CONFIG: invalid rmc_setup type = %d!\n",ftm_tdscdma_dch_config.rmc_setup,0,0);
        break;         
        
   }
   
  ftm_tdscdma_ber_max_tti_found = FALSE;
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_INIT_CPHY_SETUP_CMD

DESCRIPTION    Initializeds L1 setup command structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_init_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd, boolean signal_mac)
{
   cmd->req_mask                  = ftm_tdscdma_dch_config.req_mask;
	 cmd->ho_setup_mask             = ftm_tdscdma_dch_config.ho_setup_mask;
   cmd->freq_incl                 = ftm_tdscdma_dch_config.freq_incl;
   cmd->primary_freq              = ftm_tdscdma_dl_db.current_rf_chan;
   cmd->cell_parm_id              = ftm_tdscdma_dch_config.cell_parm_id;
   cmd->signal_ul_mac_at_act_time = FALSE;
	 cmd->signal_dl_mac_at_act_time = FALSE;
   cmd->max_tx_pwr                = ftm_tdscdma_dch_config.ul_max_tx_pwr;
   cmd->working_freq_incl         = ftm_tdscdma_dch_config.working_freq_incl;

   if( cmd->working_freq_incl == TRUE )
   {
     cmd->working_freq = ftm_tdscdma_dch_config.working_freq;
   }
#ifdef FEATURE_TDSCDMA_DMO_SUPPORT
   cmd->meas_occasion_info_incl = 0;
#endif
}


/*===========================================================================

FUNCTION       ftm_tdscdma_ber_dl_cphy_setup_cmd

DESCRIPTION    Adds a DL Phychan and DL CCTrCH configurations to an
               existing L1 setup command structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_dl_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd)
{
  //CCTRCH configuration  
  if (TDSL1_CPHY_SETUP_REQ_IS_DL_CCTRCH_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for dl cctrch Add",0,0,0);
    ftm_tdscdma_set_dl_cctrch_db_information();    

    cmd->dl_cctrch_info = ftm_tdscdma_dl_db.dl_cctrch_info;
    cmd->dl_ctfc_info   = ftm_tdscdma_dl_db.dl_ctfc_info;
  }

  //Add
  if (TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(cmd->req_mask))
  {  
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for dl phy channel Add",0,0,0);
    ftm_tdscdma_set_dl_phychan_db_information(ftm_tdscdma_dch_config.add_dl_phychan,
                                              ftm_tdscdma_dl_cctrch_info.cctrch_id);

    cmd->add_dl_phychan    = ftm_tdscdma_dch_config.add_dl_phychan;
    cmd->add_dl_phychan_db = ftm_tdscdma_dl_db.dl_phychan_db;
  }

   //Reconfig
  if (TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_CFG_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for phy channel reconfig",0,0,0);
    ftm_tdscdma_set_dl_phychan_db_information(ftm_tdscdma_dch_config.cfg_dl_phychan,
                                              ftm_tdscdma_dl_cctrch_info.cctrch_id);

    cmd->cfg_dl_phychan    = ftm_tdscdma_dch_config.cfg_dl_phychan;
    cmd->cfg_dl_phychan_db = ftm_tdscdma_dl_db.dl_phychan_db;
  }

  if(TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_DROP_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for phy channel drop",0,0,0);
    cmd->drop_dl_phychan = ftm_tdscdma_dch_config.drop_dl_phychan;
  }
}

/*===========================================================================

FUNCTION       ftm_tdscdma_ber_dl_cphy_setup_cmd_8820C_UPA

DESCRIPTION    Adds a DL Phychan and DL CCTrCH configurations to an
               existing L1 setup command structure when running UPA on 8820C.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_dl_cphy_setup_cmd_8820C_UPA( tdsl1_setup_cmd_type *cmd)
{
  //CCTRCH configuration
  if (TDSL1_CPHY_SETUP_REQ_IS_DL_CCTRCH_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for dl cctrch Add",0,0,0);
    ftm_tdscdma_set_dl_cctrch_db_information_8820C_UPA();

    cmd->dl_cctrch_info = ftm_tdscdma_dl_db.dl_cctrch_info;
    cmd->dl_ctfc_info   = ftm_tdscdma_dl_db.dl_ctfc_info;
  }

  //Add
  if (TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_ADD_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for dl phy channel Add",0,0,0);
    ftm_tdscdma_set_dl_phychan_db_information_8820C_UPA(ftm_tdscdma_dch_config.add_dl_phychan,
                                              ftm_tdscdma_dl_cctrch_info.cctrch_id);

    cmd->add_dl_phychan    = ftm_tdscdma_dch_config.add_dl_phychan;
    cmd->add_dl_phychan_db = ftm_tdscdma_dl_db.dl_phychan_db;
  }

   //Reconfig
  if (TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_CFG_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for phy channel reconfig",0,0,0);
    ftm_tdscdma_set_dl_phychan_db_information_8820C_UPA(ftm_tdscdma_dch_config.cfg_dl_phychan,
                                              ftm_tdscdma_dl_cctrch_info.cctrch_id);

    cmd->cfg_dl_phychan    = ftm_tdscdma_dch_config.cfg_dl_phychan;
    cmd->cfg_dl_phychan_db = ftm_tdscdma_dl_db.dl_phychan_db;
  }

  if(TDSL1_CPHY_SETUP_REQ_IS_DL_PHYCHAN_DROP_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for phy channel drop",0,0,0);
    cmd->drop_dl_phychan = ftm_tdscdma_dch_config.drop_dl_phychan;
  }
}


/*===========================================================================

FUNCTION       ftm_tdscdma_ber_ul_cphy_setup_cmd

DESCRIPTION    Adds a UL Phychan and UL CCTrCH configurations to an
               existing L1 setup command structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_ul_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd)
{

  //CCTRCH configuration  
  if (TDSL1_CPHY_SETUP_REQ_IS_UL_CCTRCH_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for ul cctrch Add",0,0,0);
    ftm_tdscdma_set_ul_cctrch_db_information();    

    cmd->ul_cctrch_info = ftm_tdscdma_ul_db.ul_cctrch_info;
    cmd->tfcs_ptr       = ftm_tdscdma_ul_db.tfcs_ptr;
  }

  //Add
  if (TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_ADD_INCL(cmd->req_mask))
  {  
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for ul phy channel Add",0,0,0);
    ftm_tdscdma_set_ul_phychan_db_information(ftm_tdscdma_dch_config.add_ul_phychan);

    cmd->add_ul_phychan    = ftm_tdscdma_dch_config.add_ul_phychan;
    cmd->add_ul_phychan_db = ftm_tdscdma_ul_db.ul_phychan_db;
  }

   //Reconfig
  if (TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_CFG_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for UL phy channel reconfig",0,0,0);
    ftm_tdscdma_set_ul_phychan_db_information(ftm_tdscdma_dch_config.add_ul_phychan);

    cmd->cfg_ul_phychan    = ftm_tdscdma_dch_config.cfg_ul_phychan;
    cmd->cfg_ul_phychan_db = ftm_tdscdma_ul_db.ul_phychan_db;
  }

  // Drop
  if(TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_DROP_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for phy channel drop",0,0,0);
    cmd->drop_ul_phychan = ftm_tdscdma_dch_config.drop_ul_phychan;
  }
}

/*===========================================================================

FUNCTION       ftm_tdscdma_ber_ul_cphy_setup_cmd_8820C_UPA

DESCRIPTION    Adds a UL Phychan and UL CCTrCH configurations to an
               existing L1 setup command structure when running UPA on 8820C.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_ul_cphy_setup_cmd_8820C_UPA( tdsl1_setup_cmd_type *cmd)
{

  //CCTRCH configuration
  if (TDSL1_CPHY_SETUP_REQ_IS_UL_CCTRCH_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for ul cctrch Add",0,0,0);
    ftm_tdscdma_set_ul_cctrch_db_information_8820C_UPA();

    cmd->ul_cctrch_info = ftm_tdscdma_ul_db.ul_cctrch_info;
    cmd->tfcs_ptr       = ftm_tdscdma_ul_db.tfcs_ptr;
  }

  //Add
  if (TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_ADD_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for ul phy channel Add",0,0,0);
    ftm_tdscdma_set_ul_phychan_db_information_8820C_UPA(ftm_tdscdma_dch_config.add_ul_phychan);

    cmd->add_ul_phychan    = ftm_tdscdma_dch_config.add_ul_phychan;
    cmd->add_ul_phychan_db = ftm_tdscdma_ul_db.ul_phychan_db;
  }

   //Reconfig
  if (TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_CFG_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for UL phy channel reconfig",0,0,0);
    ftm_tdscdma_set_ul_phychan_db_information_8820C_UPA(ftm_tdscdma_dch_config.add_ul_phychan);

    cmd->cfg_ul_phychan    = ftm_tdscdma_dch_config.cfg_ul_phychan;
    cmd->cfg_ul_phychan_db = ftm_tdscdma_ul_db.ul_phychan_db;
  }

  // Drop
  if(TDSL1_CPHY_SETUP_REQ_IS_UL_PHYCHAN_DROP_INCL(cmd->req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for phy channel drop",0,0,0);
    cmd->drop_ul_phychan = ftm_tdscdma_dch_config.drop_ul_phychan;
  }
}


/*===========================================================================

FUNCTION      FTM_TDSCDMA_BER_DROP_DL_CPHY_SETUP_CMD

DESCRIPTION   Requests a PhyChan Removal, using an existing L1 setup command
              structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_drop_dl_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd)
{

  cmd->drop_dl_phychan                   = TDSL1_DL_PHYCHAN_DPCH;
  cmd->req_mask                          |= TDSL1_CPHY_DL_PHYCHAN_DROP_INCL;
}


/*===========================================================================

FUNCTION      FTM_TDSCDMA_BER_DROP_UL_CPHY_SETUP_CMD

DESCRIPTION   Requests a PhyChan Removal, using an existing L1 setup command
              structure.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_drop_ul_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd)
{

  cmd->drop_ul_phychan                   = TDSL1_UL_PHYCHAN_DPCH;
  cmd->req_mask                          |= TDSL1_CPHY_UL_PHYCHAN_DROP_INCL;
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_CPHY_SETUP_REQ

DESCRIPTION    Sends a CPHY_SETUP_REQ primitive to WL1.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_CPHY_SETUP_REQ( tdsl1_setup_cmd_type *cmd)
{

  tdsl1_ext_cmd_type *cmd_ptr;

  cmd_ptr = tdsl1_get_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id                 = TDSL1_CPHY_SETUP_REQ;

  cmd_ptr->cmd_hdr.act_time_type          = TDSL1_ACTIVATION_TIME_NONE;
  cmd_ptr->cmd_hdr.act_time               = 0;
  cmd_ptr->cmd_hdr.seq_num                = 0;

  cmd_ptr->cmd.setup                      = *cmd;

  FTM_MSG_HIGH("TDS NS: Sending TDSL1_CPHY_SETUP_REQ to L1",0 ,0,0);

  tdsl1_put_cmd( cmd_ptr);
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_ULDL_CPHY_SETUP_REQ

DESCRIPTION    Sets up and configures WL1 for UL and DL physical channel
               setups.  Uses the static FTM TDSCDMA BER database for actual
               configuration parameters.


DEPENDENCIES
               FTM TDSCDMA database needs to be set up with L1 configuration.

===========================================================================*/
void ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ(void)
{

  tdsl1_setup_cmd_type                       cmd;
  
  memset(&cmd, 0x0, sizeof(cmd));

  ftm_tdscdma_ber_init_cphy_setup_cmd( &cmd, TRUE);
  ftm_tdscdma_ber_dl_cphy_setup_cmd( &cmd);
  ftm_tdscdma_ber_ul_cphy_setup_cmd( &cmd);
  ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_ULDL_DROP_CPHY_SETUP_REQ

DESCRIPTION    Drops the UL/DL Phychan setups.  Also forces HSDPA stopped
               if it is enabled.


DEPENDENCIES
               DCH should be active

===========================================================================*/
void ftm_tdscdma_ber_send_ULDL_RECFG_CPHY_SETUP_REQ( void)
{
  ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ ();
}

/*===========================================================================
FUNCTION     FTM_PUT_TDSL1_CMD

DESCRIPTION
  This function is called by TDSCDMA L1 to send a message to FTM.  In normal
  phone operation these messages go to RRC.  But in Factory Test Mode,
  FTM is commanding WL1, so FTM gets WL1 messages.

DEPENDENCIES
  None.

PARAMETERS
  rrc_cmd_type *cmd_buf - Pointer to command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  Posts a WL1 message to FTM.
===========================================================================*/
void ftm_put_tdsl1_cmd(tdsrrc_cmd_type *cmd_ptr)
{

  if (ftm_mode == FTM_MODE)
  {
    /* First initialize the queue link.  This is always done
    ** before putting an item in a queue. */
    (void)q_link(cmd_ptr, &(cmd_ptr->cmd_hdr.link));

    /* Then put the buffer in the queue. */
    q_put(&ftm_tdscdma_ber_q, &(cmd_ptr->cmd_hdr.link));

    /* Then set the signal for this queue. */
    (void)rex_set_sigs(TASK_HANDLE(ftm), FTM_TDSCDMA_BER_CMD_Q_SIG);
  }
  else
  {
    /* This is pretty awful.  WL1 sent a command to FTM because it thinks
    ** the phone is in FTM.  But it's not in FTM!  Toss the command. */
    FTM_MSG_ERROR("TDSL1 cmd sent to FTM. UE mode:%d not FTM!", ftm_mode, 0, 0);
    tdsrrc_free_cmd_buf(cmd_ptr);

    /* Cleanup state, since this is a "fatal" error. */
    
    FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_NULL);
  }
}


#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_SEND_INT_CMD

DESCRIPTION    Sends a command from FTM->FTM.


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_send_int_cmd( ftm_tdscdma_ber_int_cmd_type *msg)
{

  ftm_tdscdma_ber_int_cmd_type  *newmsg;

  newmsg = (ftm_tdscdma_ber_int_cmd_type *) q_get( &ftm_tdscdma_ber_int_free_q);

  if( newmsg != NULL)
  {
    *newmsg = *msg;
    q_put( &ftm_tdscdma_ber_int_q, q_link( newmsg, &newmsg->cmd_hdr.link));
    (void)rex_set_sigs( TASK_HANDLE(ftm), FTM_TDSCDMA_BER_INT_CMD_Q_SIG);
  }
  else
  {
      FTM_MSG_ERROR("No free buffers for FTM TDS BER",0,0,0);
  }
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_LOOPBACK_HANDLER

DESCRIPTION   calculate single ended ber


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_loopback_handler(ftm_tdscdma_ber_int_cmd_rx_data_type* rx_data_msg)
{
  #ifdef FTM_HAS_SE_BER
  ftm_se_ber_config_run_type              config;
      /* Submit the DL data to the Single Ended BER module */
      config.bit_ordering = FTM_SE_BER_BIT_ORDERING_LEFT_ALIGNED;
    config.run_mode     = FTM_SE_RUN_MODE_WCDMA_RMC;/*reuse wcdma mode*/
      config.bfi          = FALSE; /* doesn't apply in TDSCDMA mode */
      config.slot_num     = 0;

      switch( ftm_tdscdma_dch_config.rmc_setup)
      {
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
      case TDSL1_FTM_BER_RMC_TYPE_12_2_SC:
      case TDSL1_FTM_BER_RMC_TYPE_12_2_MC:
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
      case FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC:
      case FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC:
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


        if( rx_data_msg->trb_size >= 244)
          {
            config.bit_def.num_bits = 244;
          ftm_se_ber_run( rx_data_msg->rx_data, &config);
          }
        else
          {
           FTM_MSG_ERROR("TDS NS: discarding rx data in se ber: expected 244 bits but received only %d",rx_data_msg->trb_size,0,0);
          }
          break;

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
      case TDSL1_FTM_BER_RMC_TYPE_64:
      case TDSL1_FTM_BER_RMC_TYPE_144:
      case TDSL1_FTM_BER_RMC_TYPE_384:
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
      case FTM_TDSCDMA_BER_RMC_TYPE_64:
      case FTM_TDSCDMA_BER_RMC_TYPE_144:
        case FTM_TDSCDMA_BER_RMC_TYPE_384:
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


        default:
         FTM_MSG_ERROR("TDS NS: unsupported rmc type %d",ftm_tdscdma_dch_config.rmc_setup,0,0);
          break;
      }
      #endif /* FTM_HAS_SE_BER */
  
   /*clean up. should be after the featurization*/
   if (TRUE == rx_data_msg->delete_payload)
    {
      ftm_free(rx_data_msg->rx_data); 
    }
} /* ftm_tdscdma_ber_loopback_handler() */


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_PROCESS_INT_MSGS

DESCRIPTION    Message handler for FTM->FTM messages.  Currently this
               only processes messages sent from the watermark queuing 
               functions, so they can be processed in the FTM task context.


DEPENDENCIES
               ftm_tdscdma_ber_init() called.

===========================================================================*/
void ftm_tdscdma_ber_process_int_msgs( void)
{
  ftm_tdscdma_ber_int_cmd_type  *newmsg;

  /* Get all commands off the internal Q */

  while( q_cnt(&ftm_tdscdma_ber_int_q) > 0)
  {
    newmsg = q_get(&ftm_tdscdma_ber_int_q);

    if( newmsg != NULL)
    {
      switch( newmsg->cmd_hdr.cmd_id)
      {
        /* Rx data indication...process the watermark id */
        case FTM_TDSCDMA_BER_INT_CMD_RX_DATA:
          ftm_tdscdma_ber_loopback_handler( &(newmsg->msg.rx_data_stream_cmd));
          break;

        default:
         FTM_MSG_ERROR("TDS NS: unexpected int msg cmd id %d",newmsg->cmd_hdr.cmd_id,0,0);
          break;
      }
      /* Return the message buffer to the free Q */
      q_put( &ftm_tdscdma_ber_int_free_q, q_link( newmsg, &newmsg->cmd_hdr.link));
    }
  }
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_PROCESS_MSGS

DESCRIPTION    Message handler for commands that arrive from external tasks
               (e.g. RRC).


DEPENDENCIES
               ftm_tdscdma_ber_init() called.

===========================================================================*/
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_ber_process_msgs( void)
{
}

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
void ftm_tdscdma_ber_process_msgs( void)
{

  tdsrrc_cmd_type *msg;

  /* Loop through all messages in the Queue */

  while( q_cnt( &ftm_tdscdma_ber_q) > 0)
  {
    msg = q_get( &ftm_tdscdma_ber_q);

    if( msg != NULL)
    {

      /* Process RRC messages with TDSCDMA handler */

      ftm_tdscdma_ber_process_l1_messages( msg);

      /* Process RRC messages with HSDPA handler */

      #ifdef FTM_HAS_TDS_HSDPA
      ftm_tdshsdpa_bler_process_l1_messages( msg);
      #endif
      /* Discard the message */

      tdsrrc_free_cmd_buf( msg);
    }

  }
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_PROCESS_RRC_MESSAGES

DESCRIPTION


DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_process_l1_messages( tdsrrc_cmd_type *msg)
{
    switch( (tdsrrc_l1_cmd_e_type)(msg->cmd_hdr.cmd_id))
    {

/*************************************************************************
*                                                                        *
*              RRC_CPHY_START_TDSCDMA_MODE_CNF                             *
*                                                                        *
*                 -Normal response to CPHY_START_TDSCDMA_MODE_REQ          *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_START_TDS_MODE_CNF:
      {
      
        FTM_MSG_HIGH("Received RRC_CPHY_START_TDSCDMA_MODE_CNF",0,0,0);

        ftm_tdscdma_ber_allocate_dl_db();
        ftm_tdscdma_ber_allocate_ul_db();

        ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_BER_START_TDSCDMA_MODE_CNF);
        FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_IDLE);

        ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
        
        // Fill out the entry for the synchronous status message
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_START_TDSCDMA_MODE_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_START_TDSCDMA_MODE_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
        
        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_STOP_TDSCDMA_MODE_CNF                              *
*                                                                        *
*                 -Normal response to CPHY_STOP_TDSCDMA_MODE_REQ           *
*                                                                        *
**************************************************************************/

      case RRC_CPHY_STOP_TDS_MODE_CNF:
      {
      
        FTM_MSG_HIGH("Received RRC_CPHY_STOP_TDSCDMA_MODE_CNF",0,0,0);
        ftm_tdscdma_ber_free_dl_db();
        ftm_tdscdma_ber_free_ul_db();
        // Initialize UL tx data
        memset(&ftm_tdscdma_ber_ul_tx_data, 0x00, sizeof(ftm_tdscdma_ber_ul_tx_data_type));  

        ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_BER_STOP_TDSCDMA_MODE_CNF);
        FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_NULL);

        ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;

        // Fill out the entry for the synchronous status message
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_IDLE_CNF                                         *
*                                                                        *
*                 -Normal response to CPHY_START_IDLE_MODE_REQ           *
*                                                                        *
**************************************************************************/

      case TDSRRC_CPHY_IDLE_CNF:
      {
      
        ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_BER_IDLE_MODE_CNF);

        FTM_MSG_HIGH("Received RRC_CPHY_IDLE_CNF",0,0,0);

        FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_IDLE);

        ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
        
        // Fill out the entry for the synchronous status message
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_IDLE_MODE_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_IDLE_MODE_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
        
        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_ACQ_CNF                                          *
*                                                                        *
*                 -Normal response to CPHY_ACQ_REQ                       *
*                                                                        *
**************************************************************************/

      case TDSRRC_CPHY_ACQ_CNF:
      {
      
        FTM_MSG_HIGH("Received RRC_CPHY_ACQ_CNF",0,0,0);

        if( msg->cmd.acq_cnf.status == TRUE)
        {
           TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_ACQ_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
             
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ACQ);   /* Acquired Pilot, demod BCH for SFN read */
          FTM_MSG_MED( "Detect: Freq = %d",msg->cmd.acq_cnf.freq,0,0);

          ftm_tdscdma_dl_db.current_rf_chan_active = TRUE;
          ftm_tdscdma_dl_db.current_rf_chan        = msg->cmd.acq_cnf.freq;
        }
        else
        {
          TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_ACQ_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED;
             
          FTM_MSG_MED( "Acqusition on Freq = %d FAILED!",
                       msg->cmd.acq_cnf.freq,
                       0,
                       0);

          ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
        }

        ftm_tdscdma_ber_log_acq_cnf( msg->cmd.acq_cnf.status,
                                   msg->cmd.acq_cnf.freq
                                   );

        // Fill out the entry for the synchronous status message
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_ACQ_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }
      
/*************************************************************************
*                                                                        *
*              RRC_CPHY_SETUP_CNF                                        *
*                                                                        *
*                 -Normal response to CPHY_SETUP_REQ                     *
*                 -If parameters from CPHY_SETUP_REQ were invalid, the   *
*                  status will indicate a failure.                       *
*                 -For a normal ACQ->DCH setup, L1 will stay in ACQ      *
*                  state if Setup Req Fails.                             *
*                 -For a DCH->DCH reconfigure, if Setup Req fails        *
*                  then we tear down everything and attempt to go to     *
*                  IDLE to cleanup.                                      *
*                                                                        *
*                                                                        *
**************************************************************************/        
        
      case TDSRRC_CPHY_SETUP_CNF:
      {
      
        if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_PENDING)
        {
            
          if( msg->cmd.setup_cnf.status == TRUE )
          {

            /* RF chan is same as acq chan, still valid */

            /* Need to wait for PhyChan establishment before declaring DCH state */

            FTM_MSG_HIGH("RRC_CPHY_SETUP_CNF, Status: Success!",0,0,0);
          }
          else
          {

            /* RF chan still valid...should be tracking CPICH */            
            
            FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ACQ);
            FTM_MSG_ERROR("RRC_CPHY_SETUP_CNF, Status: Fail! still in ACQ state?",0,0,0);
          }
        }
        else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING)
        {

          if( msg->cmd.setup_cnf.status == TRUE )
          {
            if (ftm_tdscdma_dch_config.is_dch_refcg == FALSE)
            {
             /* Need to wait for PhyChan Established before declaring DCH state */
             FTM_MSG_HIGH("DCH Reconfig CPHY_SETUP_CNF : Drop channel success",0,0,0);
               
             // Set primary freq
             ftm_tdscdma_dl_db.current_rf_chan = ftm_tdscdma_dch_config.rf_chan;
             ftm_tdscdma_dl_db.current_rf_chan_active = TRUE;
             FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ACQ);
             
             // Add channel : send DCH config with a new freq
            (void) ftm_tdscdma_ber_rmc_dch_setup_req ((ftm_tdscdma_ber_pkt_type *)&reconfig_cmd);
               
             break;
          }
          }
          else
          {
            ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;

            FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING);
            FTM_MSG_ERROR("DCH Reconfig failure: Drop channel failed",0,0,0);                
          }
        }
        else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DROPALL_PENDING)
        {

          if( msg->cmd.setup_cnf.status == TRUE )
          {               
             /* No Need to wait for PhyChan Established before declaring DCH state */
             FTM_MSG_HIGH("DCH Drop all CPHY_SETUP_CNF success. setting BER state to Drop All Complete. Must Stop/Start TDS to Acq again",0,0,0);
             FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DROPALL_COMPLETE);

          }
          else
          {
            ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
            FTM_MSG_HIGH("DCH Drop all failure. setting BER state to Drop All Complete. Must Stop/Start TDS to Acq again",0,0,0);
            FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DROPALL_COMPLETE);
          }
        }

        ftm_tdscdma_ber_log_gen_cnf_with_status( FTM_LOG_TDSCDMA_BER_CPHY_SETUP_CNF,
                                               msg->cmd.setup_cnf.status);

        // Fill out the entry for the synchronous status message
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_CPHY_SETUP_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_CPHY_SETUP_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_PHYCHAN_ESTABLISHED_IND                          *
*                                                                        *
*                 -Normal response after a CPHY_SETUP_REQ, if the Setup  *
*                  request is valid.                                     *
*                 -Indicates if the physical channel could successfully  *
*                  be established.                                       *
*                 -For normal ACQ->DCH setup, if status is good, then    *
*                  we are in DCH state.  For DCH->DCH reconfig, we       *
*                  remain in DCH, and also attempt to restore the HS     * 
*                  setup if one was active prior to reconfig.            *
*                 -If status is bad, we attempt to go to IDLE to cleanup *
*                                                                        *
*                                                                        *
**************************************************************************/

      case TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND:
      {
      
        FTM_MSG_HIGH( "Received RRC_CPHY_PHYCHAN_ESTABLISHED_IND with status %d",
                      msg->cmd.phy_est_ind.status,0,0);

        ftm_tdscdma_ber_log_gen_cnf_with_status( FTM_LOG_TDSCDMA_PHYCHAN_ESTABLISHED_IND,
                                               msg->cmd.phy_est_ind.status);


        /* Physical Channel Established properly */

        if( msg->cmd.phy_est_ind.status == TRUE)
        {
          if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_PENDING)
          {

            FTM_MSG_HIGH("PhyChan Established: ACQ->DCH success",0,0,0);
            
            FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH);                
          }
          else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING)
          {

            FTM_MSG_HIGH( "Reconfig PhyChan Established: DCH->DCH success",0,0,0);
            
            /* Now set up on new RF channel */
            ftm_tdscdma_dl_db.current_rf_chan = ftm_tdscdma_dch_config.rf_chan;//hyang todo: rf_chan or working_freq??
          
            FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH); 

            /* Resume the HSDPA channel.  Function will check if HS channel was
             * active just before HHO, then attempt to start it back up
             */
            #ifdef FTM_HAS_TDS_HSDPA
            ftm_tdshsdpa_bler_request_resume_hs_channel();
            #endif

            if (ftm_tdscdma_dch_config.is_dch_refcg == TRUE)
            {
              ftm_tdscdma_dch_config.is_dch_refcg = FALSE;
            }
          }
          else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DROPALL_PENDING)
          {
             FTM_MSG_ERROR("TDS NS: unxpected TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND for drop all",0,0,0);
          }

          TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        }

        /* Physical Channel Establishement error */

        else if( msg->cmd.phy_est_ind.status == FALSE)
        {

          FTM_MSG_ERROR("PhyChan Establishment error",0,0,0);

          ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;

          if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DROPALL_PENDING)
          {
             FTM_MSG_ERROR("TDS NS: unxpected TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND for drop all",0,0,0);
          }
          else
          {

          }

          TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED;
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING);

        }
       

        // Fill out the entry for the synchronous status message
        
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].has_updated = TRUE;
        ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );

        break;

      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_RL_FAILURE_IND                                   *
*                                                                        *
**************************************************************************/

      case TDSRRC_CPHY_RL_FAILURE_IND:
      {
      
        ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_RL_FAILURE_IND);
        FTM_MSG_HIGH( "Received RRC_CPHY_RL_FAILURE_IND",0,0,0);
        
        break;
      }

/*************************************************************************
*                                                                        *
*              RRC_CPHY_ERROR_IND                                        *
*                                                                        *
**************************************************************************/


      case TDSRRC_CPHY_ERROR_IND:
      {
                  
        ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_CPHY_ERROR_IND);
        FTM_MSG_HIGH( "Received RRC_CPHY_ERROR_IND",0,0,0);
        
        break;
      }

/*************************************************************************
*                                                                        *
*              DEFAULT                                                   *
*                                                                        *
**************************************************************************/

      default:
      {
      
        FTM_MSG_HIGH( "Received Unknown message from L1: %d", msg->cmd_hdr.cmd_id,0,0);
        
        break;
      }
    }
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_DISPATCH

DESCRIPTION    Dispatcher for handling commands sent from PC to direct the
               FTM TDSCDMA BER functionality.


DEPENDENCIES
               ftm_tdscdma_ber_init() has been called.

===========================================================================*/
ftm_rsp_pkt_type ftm_tdscdma_ber_dispatch( ftm_tdscdma_ber_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_tdscdma_ber_sync_status_type                cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;

  // Variables for synchronous status
  uint32        start_time     = 0;
  int32         time_to_wait   = 0;
  uint8         TimeOutExpired = FALSE;
  rex_sigs_type ftm_sig_clk_rex_wait_return_sig;

  ftm_tdscdma_ber_rx_level_rpt_type  ftm_tdscdma_ber_rx_lev_rpt={0,0};

  #ifdef FTM_HAS_TDS_HSDPA
  if( cmd_ptr->ftm_cmd_header.cmd_id >= FTM_TDSCDMA_BER_HSDPA_CMD_BASE && cmd_ptr->ftm_cmd_header.cmd_id < FTM_TDSCDMA_HSUPA_CMD_BASE)
  {

    rsp_pkt = ftm_tdshsdpa_bler_dispatch( ( ftm_tdshsdpa_bler_pkt_type *) cmd_ptr);
  }

  #ifndef FTM_HAS_TDS_HSUPA
  else
  #endif
  
  #endif
  
  #ifdef FTM_HAS_TDS_HSUPA

  #ifdef FTM_HAS_TDS_HSDPA
  else if( cmd_ptr->ftm_cmd_header.cmd_id >= FTM_TDSCDMA_HSUPA_CMD_BASE )
  #else
  if( cmd_ptr->ftm_cmd_header.cmd_id >= FTM_TDSCDMA_HSUPA_CMD_BASE )
  #endif
  {
    rsp_pkt = ftm_tdscdma_hsupa_dispatch( cmd_ptr);
  }
  else
  #endif
  {
    FTM_MSG_HIGH( "FTM TDSCDMA BER dispatch, Received cmd id: %d, curent BER state: %d", 
      cmd_ptr->ftm_cmd_header.cmd_id,FTM_TDSCDMA_BER_GET_STATE(),0);

    switch( cmd_ptr->ftm_cmd_header.cmd_id)
    {
          
/***********************************************************************
*            FTM_TDSCDMA_BER_START_TDSCDMA_MODE_REQ                        *
*                                                                      *
************************************************************************/
      case FTM_TDSCDMA_BER_START_TDSCDMA_MODE_REQ:

        /*call ftm_rfmode_exit to ensure we are not in tds cal or gsm or 1x mode*/
        ftm_rfmode_exit(RFM_DEVICE_0,FTM_STATE_PARK);

        /* Start TDSL1 again only if stopped */
        if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_NULL 
            #ifdef FEATURE_FTM_GSM_BER
            && ftm_gsm_ber_get_state() == FTM_GSM_BER_STATE_STOPPED
            #endif
          )
        {

          ftm_tdscdma_ber_send_CPHY_START_TDSCDMA_MODE_REQ();

          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        else
        {

          FTM_MSG_ERROR("TDS NS: start mode req: invalid state %d.",FTM_TDSCDMA_BER_GET_STATE(),0,0);
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        }

        ftm_set_rf_db_state( FTM_DB_RF_IDLE);

        break;


/***********************************************************************
*            FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_REQ                         *
*                                                                      *
************************************************************************/

      case FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_REQ:

        /* Stop TDSL1 only if started */
        if( FTM_TDSCDMA_BER_GET_STATE() != FTM_TDSCDMA_BER_STATE_NULL)
        {
          ftm_tdscdma_ber_send_CPHY_STOP_TDSCDMA_MODE_REQ(); 
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        else
        {
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        break;

/***********************************************************************
*            FTM_TDSCDMA_BER_DROP_ALL_REQ                         *
*                                                                      *
************************************************************************/

      case FTM_TDSCDMA_BER_DROP_ALL_REQ:

        /* Stop TDSL1 only if started */
        if( FTM_TDSCDMA_BER_GET_STATE() != FTM_TDSCDMA_BER_STATE_NULL)
        {                
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DROPALL_PENDING);
          ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ(); 
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;

          #ifdef FTM_HAS_TDS_HSUPA
            ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_INACTIVE;
          #endif
        }
        else
        {
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        break;

/***********************************************************************
*            FTM_TDSCDMA_BER_IDLE_REQ                                    *
*                                                                      *
************************************************************************/

      case FTM_TDSCDMA_BER_IDLE_REQ:
        {
          FTM_MSG_ERROR("TDS NS: CPHY_IDLE_REQ isn't enabled for TDS; please use Drop all instead.",0,0,0);            
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        }

        break;

/***********************************************************************
*            FTM_TDSCDMA_BER_ACQUIRE_REQ                                 *
*                                                                      *
************************************************************************/

      case FTM_TDSCDMA_BER_ACQUIRE_REQ:
        /* Acquire only if idle or acquired Pilot already */
        if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_IDLE ||
            FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_ACQ)
        {
          ftm_tdscdma_ber_send_CPHY_ACQ_REQ( &((ftm_tdscdma_ber_acquire_req_type *) cmd_ptr)->acq_config);
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        }
        else
        {
          FTM_MSG_ERROR("TDS NS: acq req: invalid state %d.",FTM_TDSCDMA_BER_GET_STATE(),0,0);
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        }
        break;

/***********************************************************************
*            FTM_TDSCDMA_BER_RMC_DCH_SETUP                               *
*                                                                      *
************************************************************************/
      case FTM_TDSCDMA_BER_RMC_DCH_SETUP_REQ:
      {
        cmd_status = ftm_tdscdma_ber_rmc_dch_setup_req (cmd_ptr);
        break;
      }

    case FTM_TDSCDMA_BER_RMC_DCH_TFCS_CONFIG_REQ:
    {
      ftm_tdscdma_ber_rmc_dch_tfcs_config_req_type  *config = (ftm_tdscdma_ber_rmc_dch_tfcs_config_req_type *) cmd_ptr;
      uint8      i;

        // Set to success flag and change status when failure is detected
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;

      /* Config must use unique values spanning 0..3 */
      /* Check if unique */
      if( ( config->tfcs_config[0] == config->tfcs_config[1]) ||
          ( config->tfcs_config[0] == config->tfcs_config[2]) ||
          ( config->tfcs_config[0] == config->tfcs_config[3]) 
        )
      {
         FTM_MSG_ERROR("TDS NS: tfcs config isn't unique",0,0,0);
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        break;
      }

      /* Check to make sure all values within range */
      if( config->tfcs_config[0] >= FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS ||
          config->tfcs_config[1] >= FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS ||
          config->tfcs_config[2] >= FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS ||
          config->tfcs_config[3] >= FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS 
        )
      {
         FTM_MSG_ERROR("TDS NS: tfcs config is out of range",0,0,0);

          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        break;
      }
      
      for( i = 0; i < FTM_TDSCDMA_BER_NUM_TRANSPORT_FORMAT_COMBINATIONS; i++ )
      {

        if( config->tfcs_select == FTM_TDSCDMA_BER_UL_TFCS_CONFIG)
        {
          ftm_tdscdma_dch_config.ul_tfcs_config[i] = config->tfcs_config[i];
        }
        else if( config->tfcs_select == FTM_TDSCDMA_BER_DL_TFCS_CONFIG)
        {
          ftm_tdscdma_dch_config.dl_tfcs_config[i] = config->tfcs_config[i];
        }
        else
        {
            cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;;
          break;
        }
      }

        FTM_MSG_HIGH( "Configuring TFCS: is_dl=%d [0]=%d [1]=%d ", config->tfcs_select, config->tfcs_config[0], config->tfcs_config[1]);
        FTM_MSG_HIGH( "Configuring TFCS: is_dl=%d [2]=%d [3]=%d", config->tfcs_select, config->tfcs_config[2], config->tfcs_config[3] );
      
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
        break;
      }

      case FTM_TDSCDMA_BER_EVENT_STATUS_CLEAR_REQ:
      {
        /* call the function to clear the status */
        ftm_tdscdma_ber_clear_sync_status();

        /* Setup the response value */
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;

        FTM_MSG_HIGH( "Clearing synchronous TDSCDMA events", 0,0,0);
    
      break;
    }
 
      case FTM_TDSCDMA_BER_EVENT_STATUS_REQ:
      {
        /* This comand will return the status of events that have occured
           these events are stored in the TDSCDMA_BER_Sync_Status */

        ftm_tdscdma_ber_event_status_req_type *request = (ftm_tdscdma_ber_event_status_req_type *) cmd_ptr;

        /* validate the ID of the event_type */
        if ( request->event_type < FTM_TDSCDMA_BER_SYNC_STATUS_MAX)
        {
          /* Record the start time */
          start_time      = timetick_get();

          /* Setup timeout loop
           * If the event has already reported then the timing loop will not execute 
           */
          while (	(  TDSCDMA_BER_Sync_Status[ request->event_type ].has_updated == FALSE )
                &&  (  !TimeOutExpired ))
          {
            /*
             * Status messages from L1 may not be the one we are looking for.
             * For this case we will wait again adjusting for the time remaining.
             * This is neccessary because all request status messages are sharing the 
             * same signal.
             * The first time through the loop the time to wait will be the value passed
             * in by from the user
             */
            time_to_wait = request->timeout_ms - timetick_get_elapsed( start_time, T_MSEC);

            /* Make sure we have more time remaining */
            if( time_to_wait > 0 )
            {
              /* Wait on a request status sig or a timeout. */
              ftm_sig_clk_rex_wait_return_sig = ftm_sig_clk_rex_wait(FTM_SEQ_STATUS_REQ_SIG,time_to_wait);

              /* Check to see if we timed out */
              if( (ftm_sig_clk_rex_wait_return_sig & FTM_WAIT_TIMEOUT) == FTM_WAIT_TIMEOUT )
              {
                TimeOutExpired = TRUE;
                cmd_status     = FTM_TDSCDMA_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
              }
            }
            else
            {
              TimeOutExpired = TRUE;
              cmd_status     = FTM_TDSCDMA_BER_SYNC_STATUS_TIMEOUT_EXPIRED;
            } /* if( time_to_wait > 0 ) */

          }/* End While timing loop */

          /* If message received then use the status from that message type */
          if(  TDSCDMA_BER_Sync_Status[ request->event_type ].has_updated == TRUE)
          {
            cmd_status = TDSCDMA_BER_Sync_Status[ request->event_type ].status;
          }
        }
        else
        {
          /* Error, invalid event_type */
          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;

          FTM_MSG_HIGH("FTM_TDSCDMA_BER_EVENT_STATUS_REQ: Invalid Event Type=%d", request->event_type,0,0);
        }/* if ( request->event_type < FTM_TDSCDMA_BER_SYNC_STATUS_MAX) */

        FTM_MSG_MED("FTM_TDSCDMA_BER_EVENT_STATUS_REQ: Event Type=%d cmd_status =%d",request->event_type,
                                                                                   cmd_status,
                                                                                   0);

      }
      break;

      case FTM_TDSCDMA_BER_SET_UL_POWER_REQ:
      {
        ftm_tdscdma_ber_set_ul_pwr_type *request = (ftm_tdscdma_ber_set_ul_pwr_type *) cmd_ptr;

        cmd_status = ftm_tdscdma_ber_set_ul_pwr( request->override_enable,
                                              request->tx_power_10th_dbm);
      }
      break;

      /***********************************************************************
      *            			FTM_TDSCDMA_BER_RX_LEVEL_REQ                                              *
      *                                                              									     *
      ************************************************************************/
    case FTM_TDSCDMA_BER_RX_LEVEL_REQ:
    {
      ftm_tdscdma_ber_rx_level_req_type *rx_level_req = (ftm_tdscdma_ber_rx_level_req_type *) cmd_ptr;

      cmd_status = ftm_tdscdma_ber_get_rx_level(rx_level_req->rx_chain, &ftm_tdscdma_ber_rx_lev_rpt);
    }
    break;

    /***********************************************************************
      *            	      FTM_TDSCDMA_BER_TPC_CONTROL_REQ                                           *
      *                                                                                                                      *
      ************************************************************************/
    case FTM_TDSCDMA_BER_TPC_CONTROL_REQ:
    {
      ftm_tdscdma_ber_tpc_control_req_type *request = (ftm_tdscdma_ber_tpc_control_req_type *) cmd_ptr;

      cmd_status = ftm_tdscdma_ber_tpc_control_req(request->override_enable,
                                                   request->tpc_pattern);
      break;
    }
    
/***********************************************************************
*            DEFAULT                                                   *
*                                                                      *
************************************************************************/

    default:
    {
      FTM_MSG_ERROR("Unknown FTM TDSCDMA BER Command: %d",cmd_ptr->ftm_cmd_header.cmd_id,0,0);
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_UNKNOWN_CMD;
     
      break;
    }
  }

    if( cmd_status == FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE ||
        cmd_status == FTM_TDSCDMA_BER_SYNC_STATUS_UNKNOWN_CMD )
  {

     FTM_MSG_ERROR( "FTM TDSCDMA BER, Command Failure: %d, State: %d", cmd_ptr->ftm_cmd_header.cmd_id,
                                                                    FTM_TDSCDMA_BER_GET_STATE(),
                                                                    0);
  }
    switch (cmd_ptr->ftm_cmd_header.cmd_id)
    {
        case FTM_TDSCDMA_BER_RX_LEVEL_REQ:
            rsp_pkt = ftm_tdscdma_ber_create_rx_lev_response(cmd_ptr,
                                                (uint16) cmd_status,
                                                cmd_ptr->ftm_cmd_header.cmd_id,
                                                0,
                                                ftm_tdscdma_ber_rx_lev_rpt);
            break;

        default:
  rsp_pkt = ftm_tdscdma_ber_create_gen_response(cmd_ptr,
                                                (uint16) cmd_status,
                                              cmd_ptr->ftm_cmd_header.cmd_id,
                                              0);
  }

  }
  return( rsp_pkt);
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_BER_GET_STATE

DESCRIPTION    Returns current state of FTM TDSCDMA BER operations.


DEPENDENCIES
  none

===========================================================================*/
ftm_tdscdma_ber_state_type ftm_tdscdma_ber_get_state( void)
{
  return (ftm_tdscdma_ber_current_state);
}

/*===========================================================================

FUNCTION       ftm_tdscdma_ber_clear_sync_status

DESCRIPTION    This function clears all entries in the synchronous status 
               array.  This behavior is called by the FTM_TDSCDMA_BER_EVENT_STATUS_CLEAR_REQ
               command.

DEPENDENCIES
  none

===========================================================================*/
void ftm_tdscdma_ber_clear_sync_status( void)
{
  uint8 Index;

  /* Interate through the array that stores the synchronous status items */
  for ( Index = 0; Index < FTM_TDSCDMA_BER_SYNC_STATUS_MAX; Index = Index + 1 )
  {
    /* Reset the updated flag */
    TDSCDMA_BER_Sync_Status[ Index ].has_updated = FALSE;

    /* Reset the event status */
    TDSCDMA_BER_Sync_Status[ Index ].status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
  }

}

/*===========================================================================

FUNCTION       ftm_tdscdma_ber_set_ul_pwr

DESCRIPTION    This function will set the tx power of the up link to a 
               specific dBm level.

DEPENDENCIES
  none

===========================================================================*/
ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_set_ul_pwr( byte override_enable,
                                                         int2 tx_power_10th_dbm)
{
  ftm_tdscdma_ber_sync_status_type cmd_status    = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;

// todo: need to override g0 g1 as well???
  switch( override_enable )
  {
    case FTM_TDSCDMA_TX_POWER_CONTROL_TYPE_AGC_SETPOINT:
    {
      if( (-570 > tx_power_10th_dbm) || (tx_power_10th_dbm > 240) )//todo: hardcode min/max for now.
      {
        FTM_MSG_ERROR("Warning ftm_tdscdma_ber_set_ul_pwr: Power setting out of range %d",\
          tx_power_10th_dbm,0,0);
      }

     rf_tdscdma_mdsp_update_override_val(RF_TDSCDMA_MDSP_CHAIN_0, \
    RF_TDSCDMA_MDSP_TXAGC_OVERRIDE, TRUE, rf_tdscdma_agc10_to_mdsp_16q9(tx_power_10th_dbm)); 
  
      break;
    }

    case FTM_TDSCDMA_TX_POWER_CONTROL_TYPE_RPC_ENABLE:
    {
       rf_tdscdma_mdsp_update_override_val(RF_TDSCDMA_MDSP_CHAIN_0, \
      RF_TDSCDMA_MDSP_TXAGC_OVERRIDE, FALSE, rf_tdscdma_agc10_to_mdsp_16q9(tx_power_10th_dbm));
      
      break;
    }

    default:
    {
      FTM_MSG_ERROR( "ftm_tdscdma_ber_set_ul_pwr: Unsupported override_enable value=%d, tx_power_10th_dbm =%d", override_enable,tx_power_10th_dbm,0); 
      cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_OVERRIDE_VALUE_UNSUPPORTED;
      break;
    }
  }

  FTM_MSG_HIGH( "ftm_tdscdma_ber_set_ul_pwr: override_enable_type=%d, tx_power_10th_dbm =%d", override_enable,tx_power_10th_dbm,0);
  return cmd_status;

}

/*===========================================================================

FUNCTION       ftm_tdscdma_ber_tpc_control_req

DESCRIPTION    This function will set the TPC of the up-link to all up or all down and ignore the network's TPC config.

DEPENDENCIES
  none

===========================================================================*/
ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_tpc_control_req(int2 override_enable, uint32 tpc_pattern)
{
  ftm_tdscdma_ber_sync_status_type cmd_status    = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;

  /*If override TPC is enabled*/
  if (override_enable == TRUE)
  {
    /*Check if TPC pattern is valid*/
    if((tpc_pattern != 0) && (tpc_pattern != 1) )
    {
      FTM_MSG_ERROR("Error ftm_tdscdma_ber_tpc_control_req: tpc_pattern out of range %d", tpc_pattern, 0, 0);
    }
    /*Config FW to enable TPC override*/
    rf_tdscdma_mdsp_update_override_val(RF_TDSCDMA_MDSP_CHAIN_0, RF_TDSCDMA_MDSP_TPC_OVERRIDE, TRUE, (int16)tpc_pattern); 
  }
  else
  {
    /*Config FW to disable TPC override*/
    rf_tdscdma_mdsp_update_override_val(RF_TDSCDMA_MDSP_CHAIN_0, RF_TDSCDMA_MDSP_TPC_OVERRIDE, FALSE, (int16)tpc_pattern); 
  }
  FTM_MSG_HIGH( "ftm_tdscdma_ber_tpc_control_req: override_enable_type=%d, tpc_pattern =%d", override_enable, tpc_pattern, 0);
  
  return cmd_status;
}

/*===========================================================================

FUNCTION       ftm_tdscdma_ber_get_rx_levl

DESCRIPTION    This function will read the rx level on DPCH

DEPENDENCIES
  none

===========================================================================*/
ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_get_rx_level(ftm_tdscdma_ber_rx_chain_type rx_chain, ftm_tdscdma_ber_rx_level_rpt_type*  ftm_tdscdma_ber_rx_lev_rpt)
{
  ftm_tdscdma_ber_sync_status_type cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
  tfw_agc_result_buf_t* tfw_agc_result = (tfw_agc_result_buf_t*) rf_tdscdma_mdsp_smem_get_smem_ptr(RF_TDSCDMA_MDSP_CHAIN_0, RF_TDSCDMA_MDSP_RF_AGC_LOG_SMEM_TYPE,0);
   uint32 slot_idx = 0;

   if ( FTM_TDSCDMA_BER_STATE_DCH == FTM_TDSCDMA_BER_GET_STATE())
   {
      /* Note: find the active timeslot from index 1 because we just care about the DPCH timeslots */
      for(slot_idx = 1; slot_idx < TFW_SLOTS_PER_SUBFRAME; ++slot_idx)
      {
         /* Just retrieve the 1st active slot in one subframe */
         if(tfw_agc_result->rx.slot[slot_idx].active)
         {
       if (rx_chain == FTM_TDSCDMA_PRIMARY_RX_CHAIN)
         {
           ftm_tdscdma_ber_rx_lev_rpt->rssi         = rf_tdscdma_mdsp_rxagc_mdsp_to_rf_unit(tfw_agc_result->rx.slot[slot_idx].chain[0].rxPowerdBm);
           ftm_tdscdma_ber_rx_lev_rpt->rx_agc_dbm10 = rf_tdscdma_mdsp_rx_mdsp_dbm10_to_rf(tfw_agc_result->rx.slot[slot_idx].chain[0].rxPowerdBm);

           cmd_status   = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
         }

//#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
       else if (rx_chain == FTM_TDSCDMA_SECONDARY_RX_CHAIN)
         {
          ftm_tdscdma_ber_rx_lev_rpt->rssi         = rf_tdscdma_mdsp_rxagc_mdsp_to_rf_unit(tfw_agc_result->rx.slot[slot_idx].chain[1].rxPowerdBm);
          ftm_tdscdma_ber_rx_lev_rpt->rx_agc_dbm10 = rf_tdscdma_mdsp_rx_mdsp_dbm10_to_rf(tfw_agc_result->rx.slot[slot_idx].chain[1].rxPowerdBm);
           cmd_status   = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
         }
//#endif //FTM_HAS_TDSCDMA_RX_DIVERSITY
       else
         {
           FTM_MSG_ERROR("FTM_TDSCDMA_BER_RX_LEVEL_REQ: Invalid Rx Chain: %d",rx_chain,0,0);
           cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
         }

            break;
         }
      }
     }
   else
     {
       FTM_MSG_ERROR("FTM_TDSCDMA_BER_RX_LEVEL_REQ: Wrong UE State:%d",FTM_TDSCDMA_BER_GET_STATE(),0,0);
       cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
     }

   FTM_MSG_HIGH( "ftm_tdscdma_ber_get_rx_level: rx_chain=%d, rssi =%d, rx_agc_dbm10=%d", 
      rx_chain,ftm_tdscdma_ber_rx_lev_rpt->rssi,ftm_tdscdma_ber_rx_lev_rpt->rx_agc_dbm10);

   return cmd_status;
}

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
/*=========================================================================

FUNCTION ftm_tdscdma_ber_get_max_tti_frm_num

DESCRIPTION
  This function is used to get the maximum TTI frame numbers for given
  tx data structure.

DEPENDENCIES
  None

RETURN VALUE
  the maximum TTI frame numbers

SIDE EFFECTS
  None.

=========================================================================*/
uint8 ftm_tdscdma_ber_get_max_tti_frm_num (ftm_tdscdma_ber_ul_tx_data_type *mac_data_ptr)
{
  tdsl1_tti_enum_type max_tti = TDSL1_TTI_5MS;
  uint8 i;

  for (i = 0; i < mac_data_ptr->num_trch; i++)
  {
    if (mac_data_ptr->tx_data[i].tti > max_tti)
    {
      max_tti = mac_data_ptr->tx_data[i].tti;
    }
  }

  FTM_MSG_MED("ftm_tdscdma_ber_get_max_tti_frm_num: max_tti = %d, max_frm_num = %d", 
               max_tti, tdsulutil_tti_Fi_map[max_tti], 0);

  return (tdsulutil_tti_Fi_map[max_tti]);
}

/*=========================================================================

FUNCTION ftm_tdscdma_ber_ul_build_frame_ind

DESCRIPTION
  This function build Uplink TrCH data or loopback data.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void ftm_tdscdma_ber_ul_build_frame_ind
(
  /* TTI required for data */
  tdsl1_tti_enum_type     tti,

  /* UL is ready to get data from MAC or not */
  boolean                 ul_tx_disabled,

  /* pointer to Tx data buffer */
  tdsl1_ul_tx_data_type  *tx_buf_ptr
)
{
  uint8 idx = 0;
  uint8 trch_idx;
  uint8 tb_idx;
  uint8 mac_hdr_size;
  uint8 active_trch;
  ftm_tdscdma_ber_ul_tx_data_type *mac_data_ptr = &ftm_tdscdma_ber_ul_tx_data;
  
  FTM_MSG_MED("ftm_tdscdma_ber_ul_build_frame_ind: tti = %d, ul_tx_disabled = %d", tti, ul_tx_disabled, 0);
  
  /* Return if the test vector num_trch is equal to 0 */
  if (mac_data_ptr->num_trch == 0)
  {
    FTM_MSG_HIGH(" Return since num_trch = 0", 0, 0, 0);	
    return;  
  }

  if(ul_tx_disabled)
  {
    tx_buf_ptr->num_trch = 0;
    FTM_MSG_HIGH("Tx is disable at required frame", 0, 0, 0);
    return;
  }

  if (!ftm_tdscdma_ber_max_tti_found)
  {
    if (tdsulutil_tti_Fi_map[tti] < ftm_tdscdma_ber_get_max_tti_frm_num (mac_data_ptr))
    {
      tx_buf_ptr->num_trch = 0;
      FTM_MSG_HIGH("Do not return data since not maximum TTI boundary", 0, 0, 0);
      return;
    }
    else
    {
      ftm_tdscdma_ber_max_tti_found = TRUE;
    }
  }
  
  active_trch = 0;
  for (trch_idx = 0; trch_idx < mac_data_ptr->num_trch; trch_idx++)
  {
    if (mac_data_ptr->tx_data[trch_idx].tti > tti)
    {
      /* No data for this TrCH */
      tx_buf_ptr->tx_data[trch_idx].num_tb = 0;
    }
    else
    {
      tx_buf_ptr->tx_data[trch_idx].tb_size = mac_data_ptr->tx_data[trch_idx].tb_size;
      tx_buf_ptr->tx_data[trch_idx].num_tb = mac_data_ptr->tx_data[trch_idx].num_tb;

      for(tb_idx = 0; tb_idx < tx_buf_ptr->tx_data[trch_idx].num_tb; tb_idx++)
      {
        /* Fill TB data */
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].data_ptr = 
                               mac_data_ptr->tx_data[trch_idx].trch_data[tb_idx].tb_data;
 
        /* Fill MAC header */
        mac_hdr_size = mac_data_ptr->tx_data[trch_idx].trch_data[tb_idx].mac_hdr_size;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].mac_hdr_size = mac_hdr_size;
        for ( idx = 0; idx < ((mac_hdr_size+7)/8) ; idx++)
        {
          tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].mac_hdr[idx]   = mac_data_ptr->tx_data[trch_idx].trch_data[tb_idx].mac_hdr[idx];
        }

        /* Fill ciphering info */
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].ciphering_on = mac_data_ptr->tx_data[trch_idx].trch_data[tb_idx].ciphering_on;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].tm_cipher_state = TDSL1_TB_TM_CIPHER_NOT_APPLICABLE;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].radio_bearer_id = 0;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].ciphering_key_id = 0;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].count = 0;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].offset = 0;
        tx_buf_ptr->tx_data[trch_idx].trch_data[tb_idx].cipher_algo = 0;
      }
      if(tx_buf_ptr->tx_data[trch_idx].num_tb != 0)
      {
        active_trch++;
      }
      FTM_MSG_MED("ul_build_frame_ind: active_trch = %d, trch_id = %d", active_trch, trch_idx, 0);
	  
    }
  }
  tx_buf_ptr->num_trch = active_trch;
  tx_buf_ptr->mac_busy = mac_data_ptr->mac_busy;
  tx_buf_ptr->TFCI     = mac_data_ptr->TFCI;

  mac_data_ptr->num_trch = 0;//this fixes crashes when DL decode is bad

  return;
}
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

#ifdef FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER
/*=========================================================================

FUNCTION ftm_tdscdma_ber_calc_dl_ber

DESCRIPTION
  This function calculate BER for a given TB data.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/
void ftm_tdscdma_ber_calc_dl_ber (uint8 *tb_data_ptr, uint16 tb_size, boolean err)
{
  uint16 i;
  uint16 ber_per_tb = 0;
  uint16 tb_size_byte  = tb_size / 8;
  uint8  tb_size_mod_8 = tb_size % 8;
  uint8  ber_1st_nibble = 0;
  uint8  ber_2nd_nibble = 0;

  // Calculate each bit for error only if there is CRC error happens
  // Otherwise, will consider as no bit error 
  if( err == TRUE )
  {
    /* get BER for tb bytes */   
    for( i = 0; i < tb_size_byte; ++i )
    {
      ber_1st_nibble = ftm_tdscdma_ber_calc_nibble_tbl [tb_data_ptr[i] >> 4];
      ber_2nd_nibble = ftm_tdscdma_ber_calc_nibble_tbl [tb_data_ptr[i] & 0x0F];
      ber_per_tb = ber_per_tb + ber_1st_nibble + ber_2nd_nibble;
  
      ber_1st_nibble = 0;
      ber_2nd_nibble = 0;
    }
   
    /* get BER from remaining (mod byte) */
    switch( tb_size_mod_8 )
    {
      case 1:
        ber_1st_nibble = ftm_tdscdma_ber_calc_1_bit_tbl [tb_data_ptr[i] >> (8 - tb_size_mod_8)];
        break;
  
      case 2:
        ber_1st_nibble = ftm_tdscdma_ber_calc_2_bit_tbl [tb_data_ptr[i] >> (8 - tb_size_mod_8)];
        break;
      case 3:
        ber_1st_nibble = ftm_tdscdma_ber_calc_3_bit_tbl [tb_data_ptr[i] >> (8 - tb_size_mod_8)];
        break;
  
      case 4:
        ber_1st_nibble = ftm_tdscdma_ber_calc_nibble_tbl [tb_data_ptr[i] >> (8 - tb_size_mod_8)];
        break;
  
      case 5:
        ber_1st_nibble = ftm_tdscdma_ber_calc_nibble_tbl [(*tb_data_ptr) >> 4];
        ber_2nd_nibble = ftm_tdscdma_ber_calc_1_bit_tbl [((*tb_data_ptr) & 0x0F) >> (8 - tb_size_mod_8)];
        break;
  
      case 6:
        ber_1st_nibble = ftm_tdscdma_ber_calc_nibble_tbl [tb_data_ptr[i] >> 4];
        ber_2nd_nibble = ftm_tdscdma_ber_calc_2_bit_tbl [(tb_data_ptr[i] & 0x0F) >> (8 - tb_size_mod_8)];
        break;
  
      case 7:
        ber_1st_nibble = ftm_tdscdma_ber_calc_nibble_tbl [tb_data_ptr[i] >> 4];
        ber_2nd_nibble = ftm_tdscdma_ber_calc_3_bit_tbl [(tb_data_ptr[i] & 0x0F) >> (8 - tb_size_mod_8)];
        break;
  
      default: 
        break;
  
    }
  
    ber_per_tb = ber_per_tb + ber_1st_nibble + ber_2nd_nibble;
    if(ber_per_tb != 0)
    {
    FTM_MSG_HIGH( "TDS NS, CRC error, add %d to BER(only valid if DL DPCH data pattern is all 1s.)", ber_per_tb, 0, 0);
    }
  }
  else
  {
    ber_per_tb = 0;
    FTM_MSG_HIGH( "TDS NS, CRC pass.", 0, 0, 0);
  }

  ftm_tdscdma_total_bits += tb_size;
  ftm_tdscdma_ber_total  += ber_per_tb;

  ftm_tdscdma_bits_per_second += tb_size;
  ftm_tdscdma_ber_per_second  += ber_per_tb;

  if( ++ftm_tdscdma_tti_curr_cnt >= ftm_tdscdma_tti_cnt_per_second )
  {
    FTM_MSG_HIGH( "TDS NS, BER per second = ber (%d) bits per second (%d) = %d (only valid if DL DPCH data pattern is all 1s.)",
                   ftm_tdscdma_ber_per_second, ftm_tdscdma_bits_per_second, 
                  (ftm_tdscdma_ber_per_second * 100)/ftm_tdscdma_bits_per_second );

    FTM_MSG_HIGH( "TDS NS, total BER = ber (%d) total bits (%d) = %d (only valid if DL DPCH data pattern is all 1s.)",
                   ftm_tdscdma_ber_total, ftm_tdscdma_total_bits, 
                  (ftm_tdscdma_ber_total * 100)/ftm_tdscdma_total_bits );

    ftm_tdscdma_tti_curr_cnt    = 0;
    ftm_tdscdma_bits_per_second = 0;
    ftm_tdscdma_ber_per_second  = 0;
  }
}

#endif //FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
/*=========================================================================

FUNCTION ftm_tdscdma_ber_trblk_data_ind

DESCRIPTION
  This function generates BER (Bit Error Rate) of downlink data.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/

void ftm_tdscdma_ber_trblk_data_ind 
(
  tdsl1_dl_tb_data_struct_type *data_ptr,
  uint32 crc_status [][TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH]
)
{
  uint8 i;
  uint8* tb_data_ptr;
  boolean tb_error = FALSE;
   ftm_tdscdma_ber_int_cmd_type int_cmd;

   if (data_ptr->num_tb_set == 0)
   {
      FTM_MSG_HIGH( "TDS NS, ftm_tdscdma_ber_trblk_data_ind: num_tb_set = 0", 0, 0, 0);
      // Need to re-visit to decide if the expected data be invalid or not
      return;
    }

   for(i = 0; i < data_ptr->num_tb_set; i++  )
   {
     if( data_ptr->tb_set_data[i].num_tb == 0)
     {
       FTM_MSG_HIGH( "TDS NS, num_tb = 0", 0, 0, 0);
       continue;
     }

     if (data_ptr->tb_set_data[i].trch_ref_type != TDSL1_DL_TRCH_DCH_TYPE)
     {
       FTM_MSG_HIGH( "TDS NS, trch_ref_type = %d", 
                        data_ptr->tb_set_data[i].trch_ref_type, 0, 0);
       if( data_ptr->tb_set_data[i].tb_data_info->tb_data_buffer != NULL )
     {
         (void)dsm_free_buffer(data_ptr->tb_set_data[i].tb_data_info->tb_data_buffer);
       }
       continue;
     }

     if( data_ptr->tb_set_data[i].trblk_size != ftm_tdscdma_dch_config.current_tb_size)
     {
       FTM_MSG_HIGH( "TDS NS, trblk_size = %d", data_ptr->tb_set_data[i].trblk_size, 0, 0);
       if( data_ptr->tb_set_data[i].tb_data_info->tb_data_buffer != NULL )
       {
         (void)dsm_free_buffer(data_ptr->tb_set_data[i].tb_data_info->tb_data_buffer);
       }
       continue;
     }

     FTM_MSG_MED( "TDS NS, trblk_size = %d, data_size = %d, buffer_size = %d",
                   data_ptr->tb_set_data[i].trblk_size, 
                   data_ptr->tb_set_data[i].tb_data_info->data_size,
                   data_ptr->tb_set_data[i].tb_data_info->buffer_size_bytes);

     // currently for 12.2 kbps, need to expand to cover 384 kbps
     if( crc_status[ftm_tdscdma_dl_cctrch_id][0] == 0 )
     {
       tb_error = TRUE;
     }
     // clean the CRC status 
     crc_status[ftm_tdscdma_dl_cctrch_id][0] = 0; 

     tb_data_ptr = data_ptr->tb_set_data[i].tb_data_info->tb_data_buffer->data_ptr; 

#ifdef FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER     
     ftm_tdscdma_ber_calc_dl_ber (tb_data_ptr, data_ptr->tb_set_data[i].trblk_size, tb_error);
#endif //FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER

     ftm_tdscdma_ber_set_ul_tx_data(data_ptr);

     int_cmd.cmd_hdr.cmd_id               = FTM_TDSCDMA_BER_INT_CMD_RX_DATA;
     int_cmd.msg.rx_data_stream_cmd.trb_size = data_ptr->tb_set_data[i].trblk_size; /*note: unit is bit*/
     int_cmd.msg.rx_data_stream_cmd.delete_payload = TRUE;/*notify task to delete payload after use*/
     
     int_cmd.msg.rx_data_stream_cmd.rx_data = 
            ftm_malloc(data_ptr->tb_set_data[i].tb_data_info->buffer_size_bytes);

     if (int_cmd.msg.rx_data_stream_cmd.rx_data != NULL)
     {
        /*copy the data*/
        memscpy(int_cmd.msg.rx_data_stream_cmd.rx_data,
                  data_ptr->tb_set_data[i].tb_data_info->buffer_size_bytes,
                  tb_data_ptr,
                  data_ptr->tb_set_data[i].tb_data_info->buffer_size_bytes);
        /*send internal msg to ftm task*/
        ftm_tdscdma_ber_send_int_cmd(&int_cmd);
     }
     else
     {
        FTM_MSG_ERROR( "TDS NS, FAILED TO ALLOCATE MEMORY!!!LEAKS??",0,0,0);
     }     
   }

}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
/*=========================================================================

FUNCTION ftm_tdscdma_ber_rmc_dch_setup_req

DESCRIPTION
  This function process FTM_TDSCDMA_BER_RMC_DCH_SETUP_REQ.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================*/               

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
static ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_rmc_dch_setup_req (ftm_tdscdma_ber_pkt_type *cmd_ptr )
{
   ftm_tdscdma_ber_rmc_dch_setup_config_type  *dch_config_ptr;
   ftm_tdscdma_ber_sync_status_type cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
   dch_config_ptr = &(( (ftm_tdscdma_ber_rmc_dch_setup_type *) cmd_ptr)->rmc_dch_config);

   // if reconfiguration
   if (FTM_TDSCDMA_BER_GET_STATE()== FTM_TDSCDMA_BER_STATE_DCH)
   {
     /* Reconfiguration:
        1. drop current channel
        2. add new channel after receiving drop confirm
     */

     FTM_MSG_HIGH( "Reconfiguring DCH from DCH state.",0,0,0);

     if (dch_config_ptr->rf_chan == 0) //this cmd comes from dch setup instead of hho.
     {
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        FTM_MSG_ERROR( "Skip Reconfiguring DCH from DCH state because rf_chan is 0.",0,0,0);
        return (cmd_status);         
     }
     else
     {
       FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);

       /* Send CPHY_SETUP_REQ with Drop (old config) */
      ftm_tdscdma_dch_config.req_mask          = TDSL1_API_CPHY_DL_PHYCHAN_DROP_INCL | 
                                                 TDSL1_API_CPHY_UL_PHYCHAN_DROP_INCL;

       ftm_tdscdma_dch_config.rf_chan           = dch_config_ptr->rf_chan; // save new RF channel for reconfig

      ftm_tdscdma_dch_config.working_freq_incl      = TRUE;
      ftm_tdscdma_dch_config.working_freq           = ftm_tdscdma_dl_db.current_rf_chan;
   
   
       reconfig_cmd = *((ftm_tdscdma_ber_rmc_dch_setup_type *) cmd_ptr);

       FTM_MSG_HIGH("  DCH ReCfg 1. drop channels (pri_freq = %d, new_freq = %d",
                       ftm_tdscdma_dl_db.current_rf_chan,dch_config_ptr->rf_chan, 0);

       /* Send config command to L1 */            
       ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ();
     }

     return (cmd_status);
   }

   /* Have we:
    *    - successfully acquired pilot already OR in DCH to do a reconfig 
    * If neither, then declare command invalid.
    */

   if( cmd_ptr->ftm_cmd_header.cmd_id == FTM_TDSCDMA_BER_RMC_DCH_SETUP_REQ)
   {
      /* Setup configuration for parameters as part of setup packet */
      ftm_tdscdma_dch_config.rmc_setup              = dch_config_ptr->rmc_setup;  
      ftm_tdscdma_dch_config.dl_slot                = dch_config_ptr->dl_slot;  
      ftm_tdscdma_dch_config.ul_slot                = dch_config_ptr->ul_slot;  
      ftm_tdscdma_dch_config.ul_max_tx_pwr          = dch_config_ptr->ul_max_tx_pwr;  
      ftm_tdscdma_dch_config.ul_tpc_step            = (tdsl1_ftm_tpc_step_size_enum_type)dch_config_ptr->ul_tpc_step;
      ftm_tdscdma_dch_config.rf_chan                = ftm_tdscdma_dl_db.current_rf_chan;

      /* Setup default configuration for parameters not part of setup packet */
      ftm_tdscdma_dch_config.req_mask               = TDSL1_API_CPHY_DL_CCTRCH_INCL | 
                                                      TDSL1_API_CPHY_DL_PHYCHAN_ADD_INCL |
                                                      TDSL1_API_CPHY_UL_CCTRCH_INCL | 
                                                      TDSL1_API_CPHY_UL_PHYCHAN_ADD_INCL;
   }
   else
   { 
      FTM_MSG_ERROR("  DCH Setup: V2 command not supported!!! ",0,0,0);
      cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
      return (cmd_status);
   }

   FTM_MSG_HIGH("  DCH Setup: rf_chan=%d, rmc_setup=%d, ul_max_tx_pwr=%d",
                   ftm_tdscdma_dch_config.rf_chan, 
                   ftm_tdscdma_dch_config.rmc_setup,
                   ftm_tdscdma_dch_config.ul_max_tx_pwr);

   FTM_MSG_HIGH("  DCH Setup: dl_slot=%d, ul_slot=%d, ul_tpc_step=%d",
                   ftm_tdscdma_dch_config.dl_slot,
                   ftm_tdscdma_dch_config.ul_slot,ftm_tdscdma_dch_config.ul_tpc_step);

   switch( ftm_tdscdma_dch_config.rmc_setup )
   {
      case TDSL1_FTM_BER_RMC_TYPE_12_2_SC:
      case TDSL1_FTM_BER_RMC_TYPE_12_2_MC:
      {
         ftm_tdscdma_dch_config.current_tb_size = 244;
         break;
      }

      case TDSL1_FTM_BER_RMC_TYPE_64:
      case TDSL1_FTM_BER_RMC_TYPE_144:
      case TDSL1_FTM_BER_RMC_TYPE_384:          
      default:
      {
         FTM_MSG_ERROR("  DCH Setup: RMC setup %d not supported!!! ",ftm_tdscdma_dch_config.rmc_setup,0,0);
         cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
         return (cmd_status);
       }
   } /* switch () */

   if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_ACQ)
   {
      FTM_MSG_MED( "Setting up DCH from ACQ state.",0,0,0);
      FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH_PENDING);

      /* Send config command to L1 */            
      ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ();
   }

#ifdef FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER
    // Initialize Dl BER data  
    ftm_tdscdma_bits_per_second    = 0;
    ftm_tdscdma_ber_per_second     = 0;
    ftm_tdscdma_tti_cnt_per_second = 50; 
    ftm_tdscdma_tti_curr_cnt       = 0;
    ftm_tdscdma_total_bits         = 0; 
    ftm_tdscdma_ber_total          = 0; 
#endif //          FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER

    return (cmd_status);
}

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
static ftm_tdscdma_ber_sync_status_type ftm_tdscdma_ber_rmc_dch_setup_req (ftm_tdscdma_ber_pkt_type *cmd_ptr )
{
   ftm_tdscdma_ber_rmc_dch_setup_config_type  *dch_config_ptr;
   ftm_tdscdma_ber_sync_status_type cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
   dch_config_ptr = &(( (ftm_tdscdma_ber_rmc_dch_setup_type *) cmd_ptr)->rmc_dch_config);

   // if reconfiguration
   if (FTM_TDSCDMA_BER_GET_STATE()== FTM_TDSCDMA_BER_STATE_DCH)
   {
     /* Reconfiguration:
        1. drop current channel
        2. add new channel after receiving drop confirm
     */

     FTM_MSG_HIGH( "Reconfiguring DCH from DCH state.",0,0,0);

     if (dch_config_ptr->rf_chan == 0) //this cmd comes from dch setup instead of hho.
     {
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        FTM_MSG_ERROR( "Skip Reconfiguring DCH from DCH state because rf_chan is 0.",0,0,0);
        return (cmd_status);         
     }
     else
     {
       FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);

       /* Send CPHY_SETUP_REQ with Drop (old config) */
       ftm_tdscdma_dch_config.req_mask          = TDSL1_CPHY_DL_PHYCHAN_DROP_INCL | 
                                                  TDSL1_CPHY_UL_PHYCHAN_DROP_INCL;
       ftm_tdscdma_dch_config.drop_dl_phychan   = TDSL1_DL_PHYCHAN_DPCH;
       ftm_tdscdma_dch_config.drop_ul_phychan   = TDSL1_UL_PHYCHAN_DPCH;

       ftm_tdscdma_dch_config.rf_chan           = dch_config_ptr->rf_chan; // save new RF channel for reconfig

       reconfig_cmd = *((ftm_tdscdma_ber_rmc_dch_setup_type *) cmd_ptr);

       FTM_MSG_HIGH("  DCH ReCfg 1. drop channels (pri_freq = %d, new_freq = %d",
                       ftm_tdscdma_dl_db.current_rf_chan,dch_config_ptr->rf_chan, 0);

       /* Send config command to L1 */            
       ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ();
     }

     return (cmd_status);
   }

   /* Have we:
    *    - successfully acquired pilot already OR in DCH to do a reconfig 
    * If neither, then declare command invalid.
    */

   if( !( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_ACQ &&
          ftm_tdscdma_dl_db.current_rf_chan_active == TRUE)
     )
   {
      cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
      FTM_MSG_ERROR("TDS NS: DCH Setup: invalid state, not in Acq or Dch state",0,0,0);
      return (cmd_status);
   }

   if (!ftm_tdscdma_dl_db.isInitialized || !ftm_tdscdma_ul_db.isInitialized)
   {
      cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
      FTM_MSG_ERROR("TDS NS: DCH Setup: dl or ul db not initialized. tds mode started?",0,0,0);
      return (cmd_status);
   }

   if( cmd_ptr->ftm_cmd_header.cmd_id == FTM_TDSCDMA_BER_RMC_DCH_SETUP_REQ)
   {
      /* Setup configuration for parameters as part of setup packet */
      ftm_tdscdma_dch_config.rmc_setup              = dch_config_ptr->rmc_setup;  
      ftm_tdscdma_dch_config.dl_slot                = dch_config_ptr->dl_slot;  
      ftm_tdscdma_dch_config.ul_slot                = dch_config_ptr->ul_slot;  
      ftm_tdscdma_dch_config.ul_max_tx_pwr          = dch_config_ptr->ul_max_tx_pwr;  
      ftm_tdscdma_dch_config.ul_tpc_step            = dch_config_ptr->ul_tpc_step;
      ftm_tdscdma_dch_config.rf_chan                = ftm_tdscdma_dl_db.current_rf_chan;

      /* Setup default configuration for parameters not part of setup packet */
      ftm_tdscdma_dch_config.req_mask               = TDSL1_CPHY_DL_CCTRCH_INCL | 
                                                      TDSL1_CPHY_DL_PHYCHAN_ADD_INCL |
                                                      TDSL1_CPHY_UL_CCTRCH_INCL | 
                                                      TDSL1_CPHY_UL_PHYCHAN_ADD_INCL;

      ftm_tdscdma_dch_config.add_dl_phychan         = TDSL1_DL_PHYCHAN_DPCH;
      ftm_tdscdma_dch_config.add_ul_phychan         = TDSL1_UL_PHYCHAN_DPCH;


      ftm_tdscdma_dch_config.ho_setup_mask          = 0;
      ftm_tdscdma_dch_config.beta_d                 = 1;
      ftm_tdscdma_dch_config.n_312                  = FTM_TDSCDMA_BER_N312_s1; //stuart
      ftm_tdscdma_dch_config.n_313                  = FTM_TDSCDMA_BER_N313_s20;
      ftm_tdscdma_dch_config.n_315                  = FTM_TDSCDMA_BER_N315_s1;
      ftm_tdscdma_dch_config.t_312                  = 1;
      ftm_tdscdma_dch_config.t_313                  = 3;

      ftm_tdscdma_dch_config.cell_parm_id           = 0;
      ftm_tdscdma_dch_config.freq_incl              = FALSE;
      ftm_tdscdma_dch_config.working_freq_incl      = TRUE;
      ftm_tdscdma_dch_config.working_freq           = ftm_tdscdma_dl_db.current_rf_chan;
   }
   else
   { 
      FTM_MSG_ERROR("  DCH Setup: V2 command not supported!!! ",0,0,0);
      cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
      return (cmd_status);
   }

   FTM_MSG_HIGH("  DCH Setup: rf_chan=%d, rmc_setup=%d, ul_max_tx_pwr=%d",
                   ftm_tdscdma_dch_config.rf_chan, 
                   ftm_tdscdma_dch_config.rmc_setup,
                   ftm_tdscdma_dch_config.ul_max_tx_pwr);

   FTM_MSG_HIGH("  DCH Setup: dl_slot=%d, ul_slot=%d, ul_tpc_step=%d",
                   ftm_tdscdma_dch_config.dl_slot,
                   ftm_tdscdma_dch_config.ul_slot,ftm_tdscdma_dch_config.ul_tpc_step);

   switch( ftm_tdscdma_dch_config.rmc_setup )
   {
      case FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC:
      case FTM_TDSCDMA_BER_RMC_TYPE_12_2_MC:
      {
         ftm_tdscdma_dch_config.current_tb_size = 244;
         break;
      }

      case FTM_TDSCDMA_BER_RMC_TYPE_64:
      case FTM_TDSCDMA_BER_RMC_TYPE_144:
      case FTM_TDSCDMA_BER_RMC_TYPE_384:          
      default:
      {
         FTM_MSG_ERROR("  DCH Setup: RMC setup %d not supported!!! ",ftm_tdscdma_dch_config.rmc_setup,0,0);
         cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
         return (cmd_status);
       }
   } /* switch () */

   if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_ACQ)
   {
      FTM_MSG_MED( "Setting up DCH from ACQ state.",0,0,0);
      FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH_PENDING);

//    ftm_tdscdma_ber_assign_cctrch_id();

      /* Send config command to L1 */            
      ftm_tdscdma_ber_send_ULDL_CPHY_SETUP_REQ();
   }

#ifdef FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER
    // Initialize Dl BER data  
    ftm_tdscdma_bits_per_second    = 0;
    ftm_tdscdma_ber_per_second     = 0;
    ftm_tdscdma_tti_cnt_per_second = 50; 
    ftm_tdscdma_tti_curr_cnt       = 0;
    ftm_tdscdma_total_bits         = 0; 
    ftm_tdscdma_ber_total          = 0; 
#endif //          FEATURE_FTM_TDSCDMA_BER_CALC_ALL_ONES_BER

    return (cmd_status);
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
rf_tdscdma_dispatch_outcome_t ftm_tdscdma_ber_process_l1_messages_msgr( msgr_hdr_s  *msg_ptr )
{
  rf_tdscdma_dispatch_outcome_t     ret_status = RF_TDSCDMA_DISPATCH_SUCESSFUL;
  ftm_tdscdma_ber_int_cmd_type int_cmd;

  switch (msg_ptr->id )
  {
    /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_START_TDSCDMA_MODE_CNF                             *
      *                                                                        *
      *                 -Normal response to CPHY_START_TDSCDMA_MODE_REQ          *
      *                                                                        *
      **************************************************************************/
    case TDSCDMA_L1NS_CPHY_START_CNF:
    {
      FTM_MSG_HIGH("Received RRC_CPHY_START_TDSCDMA_MODE_CNF",0,0,0);
     
      ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_BER_START_TDSCDMA_MODE_CNF);
      FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_IDLE);
     
      ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
      
      // Fill out the entry for the synchronous status message
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_START_TDSCDMA_MODE_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_START_TDSCDMA_MODE_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
      
      break;
    }
      
    /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_STOP_TDSCDMA_MODE_CNF                              *
      *                                                                        *
      *                 -Normal response to CPHY_STOP_TDSCDMA_MODE_REQ           *
      *                                                                        *
      **************************************************************************/
    case TDSCDMA_L1NS_CPHY_STOP_CNF:
    {
      FTM_MSG_HIGH("Received RRC_CPHY_STOP_TDSCDMA_MODE_CNF",0,0,0);
      
      ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_BER_STOP_TDSCDMA_MODE_CNF);
      FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_NULL);
     
      ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
     
      // Fill out the entry for the synchronous status message
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_STOP_TDSCDMA_MODE_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
     
      break;
    }

    /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_ACQ_CNF                                          *
      *                                                                        *
      *                 -Normal response to CPHY_ACQ_REQ                       *
      *                                                                        *
      **************************************************************************/
    case TDSCDMA_L1NS_CPHY_ACQ_CNF:
    { 
    
      FTM_MSG_HIGH("Received RRC_CPHY_ACQ_CNF",0,0,0);
    
      if( ((tdsl1_ns_cphy_acq_cnf_t*)msg_ptr)->status == TRUE)
      {
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_ACQ_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
           
        FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ACQ);   /* Acquired Pilot, demod BCH for SFN read */
        FTM_MSG_MED( "Detect: Freq = %d",((tdsl1_ns_cphy_acq_cnf_t*)msg_ptr)->freq,0,0);
  
        ftm_tdscdma_dl_db.current_rf_chan_active = TRUE;
        ftm_tdscdma_dl_db.current_rf_chan        = ((tdsl1_ns_cphy_acq_cnf_t*)msg_ptr)->freq;
      }
      else
      {
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_ACQ_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED;
           
        FTM_MSG_MED( "Acqusition on Freq = %d FAILED!",
                     ((tdsl1_ns_cphy_acq_cnf_t*)msg_ptr)->freq,
                     0,
                     0);
    
        ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
      }
    
      ftm_tdscdma_ber_log_acq_cnf( ((tdsl1_ns_cphy_acq_cnf_t*)msg_ptr)->status,
                                 ((tdsl1_ns_cphy_acq_cnf_t*)msg_ptr)->freq
                                 );
    
      // Fill out the entry for the synchronous status message
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_ACQ_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
    
      break;
    }
      
    /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_SETUP_CNF                                        *
      *                                                                        *
      *                 -Normal response to CPHY_SETUP_REQ                     *
      *                 -If parameters from CPHY_SETUP_REQ were invalid, the   *
      *                  status will indicate a failure.                       *
      *                 -For a normal ACQ->DCH setup, L1 will stay in ACQ      *
      *                  state if Setup Req Fails.                             *
      *                 -For a DCH->DCH reconfigure, if Setup Req fails        *
      *                  then we tear down everything and attempt to go to     *
      *                  IDLE to cleanup.                                      *
      *                                                                        *
      *                                                                        *
      **************************************************************************/        
    case TDSCDMA_L1NS_CPHY_SETUP_CNF:
    {
      if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_PENDING)
      { 
        if( ((tdsl1_ns_cphy_setup_cnf_t*)msg_ptr)->status == TRUE )
        {
          /* RF chan is same as acq chan, still valid */
          /* Need to wait for PhyChan establishment before declaring DCH state */
      
          FTM_MSG_HIGH("RRC_CPHY_SETUP_CNF, Status: Success!",0,0,0);
        }
        else
        {
      
          /* RF chan still valid...should be tracking CPICH */            
          
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ACQ);
          FTM_MSG_ERROR("RRC_CPHY_SETUP_CNF, Status: Fail! still in ACQ state?",0,0,0);
        }
      }
      else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING)
      {
        if( ((tdsl1_ns_cphy_setup_cnf_t*)msg_ptr)->status == TRUE )
        {
          if (ftm_tdscdma_dch_config.is_dch_refcg == FALSE)
          {
             /* Need to wait for PhyChan Established before declaring DCH state */
             FTM_MSG_HIGH("DCH Reconfig CPHY_SETUP_CNF : Drop channel success",0,0,0);
               
             // Set primary freq
             ftm_tdscdma_dl_db.current_rf_chan = ftm_tdscdma_dch_config.rf_chan;
             ftm_tdscdma_dl_db.current_rf_chan_active = TRUE;
             FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ACQ);
             
             // Add channel : send DCH config with a new freq
            (void) ftm_tdscdma_ber_rmc_dch_setup_req ((ftm_tdscdma_ber_pkt_type *)&reconfig_cmd);
          }
        }
        else
        {
          ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
      
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING);
          FTM_MSG_ERROR("DCH Reconfig failure: Drop channel failed",0,0,0);                
        }
      }
      else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DROPALL_PENDING)
      {
        if( ((tdsl1_ns_cphy_setup_cnf_t*)msg_ptr)->status == TRUE )
        {               
           /* No Need to wait for PhyChan Established before declaring DCH state */
           FTM_MSG_HIGH("DCH Drop all CPHY_SETUP_CNF success. setting BER state to Drop All Complete. Must Stop/Start TDS to Acq again",0,0,0);
           FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DROPALL_COMPLETE);
        }
        else
        {
          ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
          FTM_MSG_HIGH("DCH Drop all failure. setting BER state to Drop All Complete. Must Stop/Start TDS to Acq again",0,0,0);
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DROPALL_COMPLETE);
        }
      }
      
      ftm_tdscdma_ber_log_gen_cnf_with_status( FTM_LOG_TDSCDMA_BER_CPHY_SETUP_CNF,
                                             ((tdsl1_ns_cphy_setup_cnf_t*)msg_ptr)->status);
      
      // Fill out the entry for the synchronous status message
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_CPHY_SETUP_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_BER_CPHY_SETUP_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
      
      break;
    }

      /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_PHYCHAN_ESTABLISHED_IND                          *
      *                                                                        *
      *                 -Normal response after a CPHY_SETUP_REQ, if the Setup  *
      *                  request is valid.                                     *
      *                 -Indicates if the physical channel could successfully  *
      *                  be established.                                       *
      *                 -For normal ACQ->DCH setup, if status is good, then    *
      *                  we are in DCH state.  For DCH->DCH reconfig, we       *
      *                  remain in DCH, and also attempt to restore the HS     * 
      *                  setup if one was active prior to reconfig.            *
      *                 -If status is bad, we attempt to go to IDLE to cleanup *
      *                                                                        *
      *                                                                        *
      **************************************************************************/
    
    case TDSCDMA_L1NS_CPHY_ESTABLISHED_IND:
    {
      FTM_MSG_HIGH( "Received RRC_CPHY_PHYCHAN_ESTABLISHED_IND with status %d",
                    ((tdsl1_ns_cphy_phychan_established_ind_t*)msg_ptr)->phychan_est_ind_status,0,0);
      
      ftm_tdscdma_ber_log_gen_cnf_with_status( FTM_LOG_TDSCDMA_PHYCHAN_ESTABLISHED_IND,
                                             ((tdsl1_ns_cphy_phychan_established_ind_t*)msg_ptr)->phychan_est_ind_status);
      
      /* Physical Channel Established properly */
      if( ((tdsl1_ns_cphy_phychan_established_ind_t*)msg_ptr)->phychan_est_ind_status == TRUE)
      {
        if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_PENDING)
        {
          FTM_MSG_HIGH("PhyChan Established: ACQ->DCH success",0,0,0);
          
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH);                
        }
        else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING)
        {
          FTM_MSG_HIGH( "Reconfig PhyChan Established: DCH->DCH success",0,0,0);
          
          /* Now set up on new RF channel */
          ftm_tdscdma_dl_db.current_rf_chan = ftm_tdscdma_dch_config.rf_chan;//hyang todo: rf_chan or working_freq??
        
          FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_DCH); 
      
          /* Resume the HSDPA channel.  Function will check if HS channel was
               * active just before HHO, then attempt to start it back up
               */
          #ifdef FTM_HAS_TDS_HSDPA
          ftm_tdshsdpa_bler_request_resume_hs_channel();
          #endif
          
          if (ftm_tdscdma_dch_config.is_dch_refcg == TRUE)
          {
            ftm_tdscdma_dch_config.is_dch_refcg = FALSE;
          }
        }
        else if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DROPALL_PENDING)
        {
          FTM_MSG_ERROR("TDS NS: unxpected TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND for drop all",0,0,0);
        }
      
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
      }
      
      /* Physical Channel Establishement error */
      else if( ((tdsl1_ns_cphy_phychan_established_ind_t*)msg_ptr)->phychan_est_ind_status == FALSE)
      {
        FTM_MSG_ERROR("PhyChan Establishment error",0,0,0);
      
        ftm_tdscdma_dl_db.current_rf_chan_active = FALSE;
      
        if( FTM_TDSCDMA_BER_GET_STATE() == FTM_TDSCDMA_BER_STATE_DROPALL_PENDING)
        {
           FTM_MSG_ERROR("TDS NS: unxpected TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND for drop all",0,0,0);
        }
      
        TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].status = FTM_TDSCDMA_BER_SYNC_STATUS_EVENT_STATUS_FAILED;
        FTM_TDSCDMA_BER_SET_STATE( FTM_TDSCDMA_BER_STATE_ERROR_OCCURRED_IDLE_PENDING);
      }
      // Fill out the entry for the synchronous status message
      
      TDSCDMA_BER_Sync_Status[ FTM_TDSCDMA_PHYCHAN_ESTABLISHED_SYNC_STATUS ].has_updated = TRUE;
      ftm_sig_rex_set( FTM_SEQ_STATUS_REQ_SIG );
      
      break;
    }

    /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_RL_FAILURE_IND                                   *
      *                                                                        *
      **************************************************************************/
    case TDSCDMA_L1NS_CPHY_RL_FAILURE_IND:
    {
    
      ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_RL_FAILURE_IND);
      FTM_MSG_HIGH( "Received RRC_CPHY_RL_FAILURE_IND",0,0,0);
      
      break;
    }

    /*************************************************************************
      *                                                                        *
      *              RRC_CPHY_ERROR_IND                                        *
      *                                                                        *
      **************************************************************************/
    case TDSCDMA_L1NS_CPHY_ERROR_IND:
    {
                
      ftm_tdscdma_ber_log_gen_cnf( FTM_LOG_TDSCDMA_CPHY_ERROR_IND);
      FTM_MSG_HIGH( "Received RRC_CPHY_ERROR_IND",0,0,0);
      
      break;
    }
    
    case TDSCDMA_L1NS_DL_DATA_IND:
    {    
      ftm_tdscdma_ber_calc_dl_ber(((tdsl1_ns_dl_data_ind_t*)msg_ptr)->tb_data_ptr,((tdsl1_ns_dl_data_ind_t*)msg_ptr)->trblk_size,((tdsl1_ns_dl_data_ind_t*)msg_ptr)->tb_error);
      
      int_cmd.cmd_hdr.cmd_id               = FTM_TDSCDMA_BER_INT_CMD_RX_DATA;
      int_cmd.msg.rx_data_stream_cmd.trb_size = ((tdsl1_ns_dl_data_ind_t*)msg_ptr)->trblk_size; /*note: unit is bit*/
      int_cmd.msg.rx_data_stream_cmd.delete_payload = TRUE;/*notify task to delete payload after use*/
      
      int_cmd.msg.rx_data_stream_cmd.rx_data = 
          ftm_malloc(((tdsl1_ns_dl_data_ind_t*)msg_ptr)->buffer_size_bytes);
      
      if (int_cmd.msg.rx_data_stream_cmd.rx_data != NULL)
      {
        /*copy the data*/
        #if 0 
        memcpy(int_cmd.msg.rx_data_stream_cmd.rx_data,
                  ((tdsl1_ns_dl_data_ind_t*)msg_ptr)->tb_data_ptr,
                  ((tdsl1_ns_dl_data_ind_t*)msg_ptr)->buffer_size_bytes);
        #endif
        memscpy(int_cmd.msg.rx_data_stream_cmd.rx_data,
                      ((tdsl1_ns_dl_data_ind_t*)msg_ptr)->buffer_size_bytes,
                      ((tdsl1_ns_dl_data_ind_t*)msg_ptr)->tb_data_ptr,
                      ((tdsl1_ns_dl_data_ind_t*)msg_ptr)->buffer_size_bytes); 
        
        
        /*send internal msg to ftm task*/
        ftm_tdscdma_ber_send_int_cmd(&int_cmd);
      }
      else
      {
        FTM_MSG_ERROR( "TDS NS, FAILED TO ALLOCATE MEMORY!!!LEAKS??",0,0,0);
      }     
      
      FTM_MSG_HIGH( "Received TDSCDMA_L1NS_DL_DATA_IND",0,0,0);
      
      break;
    }

    /*************************************************************************
      *                                                                        *
      *              DEFAULT                                                   *
      *                                                                        *
      **************************************************************************/
    default:
    {
      FTM_MSG_HIGH( "Received Unknown message from L1: %d", msg_ptr->id,0,0);
      
      break;
    }
  }

  return (ret_status);
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

#endif /* FTM_HAS_TDSCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */
