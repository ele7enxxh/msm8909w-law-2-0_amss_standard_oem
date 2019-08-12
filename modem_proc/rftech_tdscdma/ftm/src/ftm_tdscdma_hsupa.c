/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 by Qualcomm Technologies INCORPORATED.  
  Copyright (c) 2012 by Qualcomm Technologies INCORPORATED.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_hsupa.c#1 $ 
  $DateTime: 2016/12/13 07:59:50 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/30/14   qzh     Enable FTM NS new intf and remove the dependency
11/20/13   qzh     Featurized FTM NS new interface to disable it on BO 0.1 
07/06/13   qzh     Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
06/07/13   qzh     Add support for FTM HSUPA 16QAM on 8820C
12/06/12   ms      Fix linker error 
12/06/12   ms      Merged
11/07/12   qzh     Add support for HSUPA freq reconfiguration and merge HSUPA FTM related msg to only one setup msg.
10/19/12   qzh     Fix the building warnings.
10/11/12   qzh     Created and add support for TDSCDMA FTM HSUPA.
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
#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftm_v.h"
#include "rf_test_task_v.h"
#include "rfumts.h"
#include "ftm_tdscdma_ber.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_mdsp_types.h"
#include "rf_tdscdma_mc.h"
#include "ftm_tdscdma_ctl.h"
#include "ftm_common_control.h"
//hy todo: needed cust feature.
#if defined(FTM_HAS_TDS_HSDPA)
#include "ftm_tdshsdpa_bler.h"
#endif /*FTM_HAS_TDS_HSDPA*/
#include "err.h"
#include "msg.h"
#include "rex.h"
#include "task.h"


#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tdsl1_ftm_msg.h"
#include "msgr.h"
#include "rf_tdscdma_msg.h"
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
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
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
#include "ftm_log.h"

#ifdef FTM_HAS_SE_BER
#include "ftm_se_ber.h"
#endif

#if defined(FEATURE_FTM_GSM_BER)
#include "ftm_umts_ber.h"
#include "ftm_gsm_ber.h"
#endif

#ifdef FTM_HAS_TDS_HSUPA
#include "ftm_tdscdma_hsupa.h"
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tds_l1_api.h"
#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
#include "tdseulsg.h"
#include "tdsl1macif.h"
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/*===========================================================================

                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                               MACROS

===========================================================================*/


/*===========================================================================

                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
ftm_tdscdma_hsupa_db_type ftm_tdscdma_hsupa_db;


extern ftm_tdscdma_ber_dch_config_type ftm_tdscdma_dch_config;
extern ftm_tdscdma_ber_dl_db_type ftm_tdscdma_dl_db;

extern ftm_tdscdma_ber_state_type ftm_tdscdma_ber_current_state;
extern uint8 ftm_tdscdma_dl_cctrch_id;

extern ftm_rsp_pkt_type ftm_tdscdma_ber_create_gen_response( ftm_tdscdma_ber_pkt_type *cmd_ptr, uint16 status,
                                                    uint16 cid, uint16 scid);

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
// HSUPA
uint32 ftm_tdscdma_hsupa_edch_tx_data_qpsk[30] = 
{
0x7A1194F8,
0xB05A1586,
0x3DF230F7,
0x3BEEE4C0,
0xCD3E803F,
0xFA86C761,
0x8BB8A7F1,
0x421DDEB6,
0x810C5950,
0x1B07CD2A,
0x9E08BD9C,
0x54FFFF20,
0xCC650728,
0x9BD89F21,
0x71A82D68,
0xFA10C8E8,
0x7EA37B42,
0xB1257DE4,
0xA4CE8ACC,
0xA7C2ABD0,
0x7283A9D4,
0xB58ACA06,
0x8512E9D9,
0x2E60D099,
0x927B9DAC,
0x12522A3C,
0x44BE948D,
0xDDA5C0E9,
0xF12CBBBA,
0x4C400000
};

uint32 ftm_tdscdma_hsupa_edch_tx_data_16qam[59] = 
{
0x1EE8C598,
0xA378AD41,
0x9BC8FB00,
0x7F50DDD7,
0x64E39752,
0x24DBF0A7,
0x2DC364D3,
0x6D10F384,
0x9D712E5F,
0xE2DF6529,
0xFC9A0601,
0xF75A0FFD,
0x719C8AF4,
0x44E71C3F,
0xA96B055D,
0xC8DFAD1B,
0x385B5C9C,
0xC6CB1341,
0x7DBB3155,
0x9059747B,
0x1B8C3756,
0x8C69C1B8,
0x5CAC31F4,
0xA562294E,
0x6729618F,
0xA0C08FDA,
0x18CEC9F0,
0x8FD9B010,
0x292FF6BE,
0xBAB9EB50,
0x8304D392,
0x86172636,
0xB3F1AB91,
0x4C547F9A,
0x157E45C8,
0x63391C1F,
0xE0C86D1F,
0x0A5544DA,
0x6C2EC667,
0xB6DEB247,
0x5B92EB64,
0x6C535F70,
0x37D23F65,
0x592DF83C,
0x5D34149E,
0xA64FFA26,
0x1EB1054E,
0x803733B1,
0x32BBB513,
0x6A5944E4,
0xD492067A,
0x2DCD58F6,
0x8E127E78,
0x5D350000
};

tdsl1_eul_edch_ctrl_info_struct_type  ftm_tdscdma_hsupa_l1_mac_etfc_sel_info;


ftm_tdscdma_hsupa_db_type ftm_tdscdma_hsupa_db;
extern ftm_tdscdma_ber_dch_config_type ftm_tdscdma_dch_config;
extern ftm_tdscdma_ber_dl_db_type ftm_tdscdma_dl_db;
extern ftm_tdscdma_ber_ul_db_type ftm_tdscdma_ul_db;
extern ftm_tdscdma_ber_state_type ftm_tdscdma_ber_current_state;


extern ftm_rsp_pkt_type ftm_tdscdma_ber_create_gen_response( ftm_tdscdma_ber_pkt_type *cmd_ptr, uint16 status,
                                                    uint16 cid, uint16 scid);
extern void ftm_tdscdma_ber_set_state(ftm_tdscdma_ber_state_type s);
extern void ftm_tdscdma_ber_assign_cctrch_id( void);
extern void ftm_tdscdma_ber_dl_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
extern void ftm_tdscdma_ber_ul_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
extern void ftm_tdscdma_ber_send_CPHY_DROP_ALL_REQ(void);
extern void ftm_tdscdma_ber_dl_cphy_setup_cmd_8820C_UPA( tdsl1_setup_cmd_type *cmd);
extern void ftm_tdscdma_ber_ul_cphy_setup_cmd_8820C_UPA( tdsl1_setup_cmd_type *cmd);

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

// HSUPA
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
void ftm_tdscdma_hsupa_set_edch_info_db(void);
void ftm_tdscdma_hsupa_init_edch_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
void ftm_tdscdma_hsupa_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd);
tdsl1_eul_edch_ctrl_info_struct_type *
ftm_tdscdma_hsupa_tdsmac_eul_etfci_selection
(
  tdsl1_eul_sg_upd_info_struct_type * ftm_sg_info
);
void ftm_tdscdma_hsupa_eul_phy_edch_build_frame_ind
(
  tdseul_l1_build_frame_struct_type * ftm_build_frame_info
);
void ftm_tdscdma_hsupa_sim_schld_ehich(tdseulsg_schld_ehich_result_struct_type *ftm_schld_ehich_ptr, uint16 ftm_curr_sub_fn);

int16 ftm_tdscdma_hsupa_get_epuch_tx_pwr(void);

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
ftm_rsp_pkt_type ftm_tdscdma_hsupa_dispatch( ftm_tdscdma_ber_pkt_type *cmd_ptr );

#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_SET_EDCH_INOF_DB

DESCRIPTION    this function set HSUPA configuration data base, for HSUPA 
               parameters in CPHY_SETUP message


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_set_edch_info_db(void)
{
  FTM_MSG_MED("TDS NS: set E-DCH db for hsupa",0,0,0);
  
  memset(&ftm_tdscdma_hsupa_db.hsupa_edch_cfg, 0x0, sizeof(ftm_tdscdma_hsupa_db.hsupa_edch_cfg));
  
  /** E-DL config */
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_rnti = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_channel_mask = TDSL1_E_HICH_INFO_INCL | TDSL1_E_AGCH_INFO_INCL | TDSL1_ERNTI_PRESENT;
  
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.rdi_pre = FALSE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.tpc_step = TDSEDL_TPC_STEP_SIZE_1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.bler_target = -50;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.num_eagch = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.eagch_slot_info[0].ts_num = 6;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.eagch_slot_info[0].first_code = TDSL1_EDL_SF16_CH_CODE_13;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.eagch_slot_info[0].second_code = TDSL1_EDL_SF16_CH_CODE_14;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.eagch_slot_info[0].eagch_midamble.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.eagch_slot_info[0].eagch_midamble.config = TDSL1_MIDAMBLE_CONFIG_8;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_agch_info.eagch_slot_info[0].eagch_midamble.shift = 0;
  
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.n_e_hich = 12;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.num_ehich = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.sched_e_hich_slot_info[0].ts_num = 6;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.sched_e_hich_slot_info[0].ei = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.sched_e_hich_slot_info[0].chan_code = TDSL1_EDL_SF16_CH_CODE_15;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.sched_e_hich_slot_info[0].ehich_midamble.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.sched_e_hich_slot_info[0].ehich_midamble.config = TDSL1_MIDAMBLE_CONFIG_8;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_dl_info.e_dl_per_rl_info.e_hich_info.sched_e_hich_slot_info[0].ehich_midamble.shift = 0;
   
  /** E-UL config */
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_ul_info_mask = TDSL1_EPUCH_INFO_INCL | TDSL1_HARQ_RV_INFO_INCL;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.harq_rv_config = TDSL1_RV_0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.num_ref_e_tfci_qpsk = 8;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[0].ref_code_rate = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[0].ref_gain_factor = -9;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[1].ref_code_rate = 2;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[1].ref_gain_factor = -6;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[2].ref_code_rate = 3;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[2].ref_gain_factor = -5;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[3].ref_code_rate = 4;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[3].ref_gain_factor = -4;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[4].ref_code_rate = 5;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[4].ref_gain_factor = -3;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[5].ref_code_rate = 6;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[5].ref_gain_factor = -2;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[6].ref_code_rate = 9;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[6].ref_gain_factor = 2;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[7].ref_code_rate = 10;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_qpsk[7].ref_gain_factor = 6;
  
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.num_ref_e_tfci_16qam = 8;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[0].ref_code_rate = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[0].ref_gain_factor = -2;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[1].ref_code_rate = 2;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[1].ref_gain_factor = -1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[2].ref_code_rate = 3;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[2].ref_gain_factor = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[3].ref_code_rate = 4;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[3].ref_gain_factor = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[4].ref_code_rate = 7;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[4].ref_gain_factor = 6;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[5].ref_code_rate = 8;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[5].ref_gain_factor = 7;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[6].ref_code_rate = 9;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[6].ref_gain_factor = 9;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[7].ref_code_rate = 10;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ref_e_tfci_list_16qam[7].ref_gain_factor = 14;
  
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.snpl_rep_type = TDSL1_SNPL_REP_TYPE_1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.prx_des_base = -90;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.beacon_pl_est = FALSE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.tpc_step = TDSEDL_TPC_STEP_SIZE_1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.p_base_contr_gap = 255;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ul_sync_step = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.ul_sync_freq = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.min_code_rate = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.max_code_rate = 63;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.num_epuch_ts = 5;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[0].ts_num = 1;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[0].midamble_info.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[0].midamble_info.config = TDSL1_MIDAMBLE_CONFIG_16;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[0].midamble_info.shift = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[1].ts_num = 2;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[1].midamble_info.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[1].midamble_info.config = TDSL1_MIDAMBLE_CONFIG_16;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[1].midamble_info.shift = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[2].ts_num = 3;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[2].midamble_info.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[2].midamble_info.config = TDSL1_MIDAMBLE_CONFIG_16;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[2].midamble_info.shift = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[3].ts_num = 4;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[3].midamble_info.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[3].midamble_info.config = TDSL1_MIDAMBLE_CONFIG_16;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[3].midamble_info.shift = 0;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[4].ts_num = 5;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[4].midamble_info.allocation_mode = TDSL1_DEFAULT_MIDAMBLE;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[4].midamble_info.config = TDSL1_MIDAMBLE_CONFIG_16;
  ftm_tdscdma_hsupa_db.hsupa_edch_cfg.e_ul_info.e_puch_info.epuch_ts_cfg_list[4].midamble_info.shift = 0;
  
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_INIT_EDCH_CPHY_SETUP_CMD

DESCRIPTION    this function initialize CPHY_SETUP message for HSUPA, including
               set req_mask and e_req_mask 


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_init_edch_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd)
{
  cmd->req_mask |= ftm_tdscdma_hsupa_db.hsupa_req_mask;
  cmd->e_req_mask = ftm_tdscdma_hsupa_db.hsupa_e_req_mask;
  
  ftm_tdscdma_hsupa_set_edch_info_db();
  
  FTM_MSG_MED("TDS NS: init CPHY_SETUP_REQ cmd for hsupa 0x%x, 0x%x",cmd->req_mask,cmd->e_req_mask,0);
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_CPHY_SETUP_CMD

DESCRIPTION    this function set e_info in CPHY_SETUP message


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_cphy_setup_cmd( tdsl1_setup_cmd_type *cmd)
{
  // add
  if(TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_DL_CONFIG_INCL(cmd->req_mask)
  && TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_UL_CONFIG_INCL(cmd->req_mask)
  && TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_START_INCL(cmd->e_req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for hsupa channel Add",0,0,0);
    cmd->mac_e_reset_indicator = TRUE;
    cmd->e_info_ptr = &ftm_tdscdma_hsupa_db.hsupa_edch_cfg;
  }
  
  // reconfig
  if(TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_DL_CONFIG_INCL(cmd->req_mask)
  && TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_UL_CONFIG_INCL(cmd->req_mask)
  && TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EDL_CFG_INCL(cmd->e_req_mask)
  &&TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_CFG_INCL(cmd->e_req_mask) )
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for hsupa channel Recfg",0,0,0);
    cmd->mac_e_reset_indicator = TRUE;
    cmd->e_info_ptr = &ftm_tdscdma_hsupa_db.hsupa_edch_cfg;
  }
  
  // drop
  if(TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_DL_CONFIG_INCL(cmd->req_mask)
  && TDSL1_CPHY_SETUP_REQ_IS_EUL_CHAN_UL_CONFIG_INCL(cmd->req_mask)
  && TDSL1_CPHY_SETUP_REQ_E_MASK_IS_EUL_STOP_INCL(cmd->e_req_mask))
  {
    FTM_MSG_MED("TDS NS: CPHY_SETUP_REQ cmd for hsupa channel Drop",0,0,0);
    cmd->mac_e_reset_indicator = TRUE;
    cmd->e_info_ptr = &ftm_tdscdma_hsupa_db.hsupa_edch_cfg;
  }
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_TDSMAC_EUL_ETFCI_SELECTION

DESCRIPTION    this function simulate HSUPA L1 and MAC interface for MAC to select
               E-TFCI, based on User seleted modulation type and E-PUCH code in
               Non-Signaling mode parameters in QRCT, this will select E-TFCI
               accordingly


DEPENDENCIES
   None.

RETURN VALUE
   E-TFCI info selected by MAC.

SIDE EFFECTS
   None.

===========================================================================*/

tdsl1_eul_edch_ctrl_info_struct_type *
ftm_tdscdma_hsupa_tdsmac_eul_etfci_selection
(
  tdsl1_eul_sg_upd_info_struct_type * ftm_sg_info
)
{
  tdsl1_eul_supported_etfci_struct_type support_etfci_info;
  
  memset(&ftm_tdscdma_hsupa_l1_mac_etfc_sel_info,0x0,sizeof(ftm_tdscdma_hsupa_l1_mac_etfc_sel_info));
  
  /** by default, do nothing */
  ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0xFF;
  ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_NO_MAC_E_PDU;
  
  if(ftm_sg_info->tx_status == EUL_NO_GRANT)
  {
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, L1->MAC, no grant",0,0,0);

    ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0xFF;
    ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_NO_MAC_E_PDU;
  
    return &ftm_tdscdma_hsupa_l1_mac_etfc_sel_info;
  }
  else if(ftm_sg_info->tx_status == EUL_NEW_TRANSMISSION)
  {
    support_etfci_info = tdsl1_eul_calc_supported_etfci_based_on_power_offset(0);
  
    if(support_etfci_info.max_etfci_to_select == 0xFF || support_etfci_info.min_etfci_to_select == 0xFF )
    {
      FTM_MSG_ERROR("FTM TDSCDMA HSUPA, max support E-TFCI from L1 is invalid, select E-TFCI=0",0,0,0);
      
      ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
      ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_SI_ALONE_ON_EPUCH;
      ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
      ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
      ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
      ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
      
      return &ftm_tdscdma_hsupa_l1_mac_etfc_sel_info;
    }
  
    if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_mod_type == TDSEULSG_EPUCH_MOD_TYPE_QPSK)
    {
      if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri == 0)
      {
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 40;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_MAC_E_PDU_ON_EPUCH;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
      }
      else
      {
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_MAC_E_PDU_ON_EPUCH;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
      }
      
      FTM_MSG_HIGH("FTM TDSCMDA HSUPA, NEW TX, MAC select E-TFCI = %d, QPSK",ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci,0,0);
    }
    else if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_mod_type == TDSEULSG_EPUCH_MOD_TYPE_16QAM)
    {
      if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri == 0)
      {
        if(support_etfci_info.max_etfci_to_select > 53)
        {
          // SF=1, 16QAM, E-TFCI = 53
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 53;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_MAC_E_PDU_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        else
        {
          // ERROR condition, E-TFCI = 0
          FTM_MSG_ERROR("FTM TDSCDMA HSUPA, max support E-TFCI from L1 = %d, crri = %d, can't use 16QAM, select E-TFCI=0 ",support_etfci_info.max_etfci_to_select,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri,0);
    
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_SI_ALONE_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        
      }
      else if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri >= 1 && ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri <=2)
      {
        if(support_etfci_info.max_etfci_to_select > 35)
        {
          // SF=2, 16QAM E-TFCI = 35
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 35;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_MAC_E_PDU_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        else
        {
          // ERROR condition, E-TFCI = 0
          FTM_MSG_ERROR("FTM TDSCDMA HSUPA, max support E-TFCI from L1 = %d, crri = %d, can't use 16QAM, select E-TFCI=0 ",support_etfci_info.max_etfci_to_select,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri,0);
    
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_SI_ALONE_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        
      }
      else if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri >= 3 && ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri <= 6)
      {
        if(support_etfci_info.max_etfci_to_select > 15)
        {
          // SF=4, 16QAM, E-TFCI = 15
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 15;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_MAC_E_PDU_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        else
        {
          // ERROR condition, E-TFCI = 0
          FTM_MSG_ERROR("FTM TDSCDMA HSUPA, max support E-TFCI from L1 = %d, crri = %d, can't use 16QAM, select E-TFCI=0 ",support_etfci_info.max_etfci_to_select,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri,0);
    
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_SI_ALONE_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        
      }
      else if(ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri >= 7 && ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri <=14)
      {
        if(support_etfci_info.max_etfci_to_select > 2)
        {
          // SF=8, 16QAM, E-TFCI = 2
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_MAC_E_PDU_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        else
        {
          // ERROR condition, E-TFCI = 0
          FTM_MSG_ERROR("FTM TDSCDMA HSUPA, max support E-TFCI from L1 = %d, crri = %d, can't use 16QAM, select E-TFCI=0 ",support_etfci_info.max_etfci_to_select,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri,0);
    
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_SI_ALONE_ON_EPUCH;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
          ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
        }
        
      }
      else
      {
        // ERROR condition, E-TFCI = 0
        FTM_MSG_ERROR("FTM TDSCDMA HSUPA, ERROR, CRRI out of valid range, select E-TFCI=0 ",support_etfci_info.max_etfci_to_select,0,0);
    
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci = 0;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_SI_ALONE_ON_EPUCH;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.asc_index = 0xFF;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.harq_retransmit_timer = 60;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.max_num_retransmit = 2;
        ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.harq_profile.power_offset = 0;
      }
      
      
      FTM_MSG_HIGH("FTM TDSCMDA HSUPA, NEW TX, MAC select E-TFCI = %d, 16QAM",ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci,0,0);
    }
    else
    {}
  
    return &ftm_tdscdma_hsupa_l1_mac_etfc_sel_info;
  }
  else if(ftm_sg_info->tx_status == EUL_RETRANSMISSION)
  {
    ftm_tdscdma_hsupa_l1_mac_etfc_sel_info.etfci_status = EUL_NO_MAC_E_PDU;
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, RE-TX",0,0,0);
    return &ftm_tdscdma_hsupa_l1_mac_etfc_sel_info;
  }
  else
  {
    ERR_FATAL("FTM TDSCMDA HSUPA L1->MAC, error param in ETFC sel intf, %d",ftm_sg_info->tx_status,0,0);
  }
  
  return &ftm_tdscdma_hsupa_l1_mac_etfc_sel_info;
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_EUL_PHY_EDCH_BUILD_FRAME_IND

DESCRIPTION    this function simulate HSUPA L1 and MAC interface for MAC to
               build E-DCH PDU


DEPENDENCIES
   None.

RETURN VALUE
   E-DCH PDU.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_eul_phy_edch_build_frame_ind
(
  tdseul_l1_build_frame_struct_type * ftm_build_frame_info
)
{
  /** first is error check */
  if(ftm_build_frame_info->etfci == ERUCCH_TX_ETFCI)
  {
    ERR_FATAL("FTM TD-SCDMA don't support E-RUCCH",0,0,0);
  }
  
  if(ftm_build_frame_info->etfci != 40 
  && ftm_build_frame_info->etfci != 53
  && ftm_build_frame_info->etfci != 35
  && ftm_build_frame_info->etfci != 15
  && ftm_build_frame_info->etfci != 2
  && ftm_build_frame_info->etfci != 0 )
  {
    ERR_FATAL("FTM TD-SCDMA don't support E-TFCI = %d, only support E-TFCI 40 and 53",ftm_build_frame_info->etfci,0,0);
  }
  
  /** E-PUCH is QPSK */
  if(ftm_build_frame_info->etfci == 40)
  {
    ftm_build_frame_info->data_buf->num_src_desc = 2;
  
    tdsl1_l2_build_copy_engine_ctrl_info(26,0,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(26, 0, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                        FALSE, FALSE, 0);
                    
    tdsl1_l2_write_hdr_section(4, (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_qpsk[0]);

    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->data_seg_list_ptr = (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_qpsk[1];
    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->length = 912 ;

    tdsl1_l2_build_copy_engine_ctrl_info(0,912,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(912, 9, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                         TRUE, FALSE, 0x0);
  
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, build EDCH PDU, E-TFCI = %d, QPSK",ftm_build_frame_info->etfci,0,0);
  }
  
  /** E-PUCH is 16QAM */
  else if(ftm_build_frame_info->etfci == 53)
  {
    ftm_build_frame_info->data_buf->num_src_desc = 2;
  
    tdsl1_l2_build_copy_engine_ctrl_info(32,0,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(32, 0, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                        FALSE, FALSE, 0);
                    
    tdsl1_l2_write_hdr_section(4, (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[0]);

    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->data_seg_list_ptr = (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[1];
    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->length = 1680 ;

    tdsl1_l2_build_copy_engine_ctrl_info(0,1680,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(1680, 9, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                         TRUE, FALSE, 0x0);
  
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, build EDCH PDU, E-TFCI = %d, 16QAM",ftm_build_frame_info->etfci,0,0);
  }
  else if(ftm_build_frame_info->etfci == 35)
  {
    ftm_build_frame_info->data_buf->num_src_desc = 2;
  
    tdsl1_l2_build_copy_engine_ctrl_info(25,0,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(25, 0, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                        FALSE, FALSE, 0);
                    
    tdsl1_l2_write_hdr_section(4, (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[0]);

    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->data_seg_list_ptr = (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[1];
    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->length = 720 ;

    tdsl1_l2_build_copy_engine_ctrl_info(0,720,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(720, 9, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                         TRUE, FALSE, 0x0);
  
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, build EDCH PDU, E-TFCI = %d, 16QAM",ftm_build_frame_info->etfci,0,0);
  }
  else if(ftm_build_frame_info->etfci == 15)
  {
    ftm_build_frame_info->data_buf->num_src_desc = 2;
  
    tdsl1_l2_build_copy_engine_ctrl_info(29,0,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(29, 0, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                        FALSE, FALSE, 0);
                    
    tdsl1_l2_write_hdr_section(4, (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[0]);

    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->data_seg_list_ptr = (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[1];
    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->length = 184 ;

    tdsl1_l2_build_copy_engine_ctrl_info(0,184,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(184, 9, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                         TRUE, FALSE, 0x0);
  
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, build EDCH PDU, E-TFCI = %d, 16QAM",ftm_build_frame_info->etfci,0,0);
  }
  else if(ftm_build_frame_info->etfci == 2)
  {
    ftm_build_frame_info->data_buf->num_src_desc = 2;
  
    tdsl1_l2_build_copy_engine_ctrl_info(26,0,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(26, 0, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                        FALSE, FALSE, 0);
                    
    tdsl1_l2_write_hdr_section(4, (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[0]);

    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->data_seg_list_ptr = (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_16qam[1];
    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->length = 136 ;

    tdsl1_l2_build_copy_engine_ctrl_info(0,136,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(136, 9, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                         TRUE, FALSE, 0x0);
  
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, build EDCH PDU, E-TFCI = %d, 16QAM",ftm_build_frame_info->etfci,0,0);
  }
  
  else if(ftm_build_frame_info->etfci == 0)
  {
    ftm_build_frame_info->data_buf->num_src_desc = 2;
  
    tdsl1_l2_build_copy_engine_ctrl_info(7,0,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(7, 0, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                        FALSE, FALSE, 0);
                    
    tdsl1_l2_write_hdr_section(1, (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_qpsk[0]);

    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->data_seg_list_ptr = (uint8*)&ftm_tdscdma_hsupa_edch_tx_data_qpsk[1];
    ftm_build_frame_info->data_buf->cur_ul_buf_ptr->length = 16 ;

    tdsl1_l2_build_copy_engine_ctrl_info(0,16,0);
    tdsl1_l2_build_cipher_engine_ctrl_info(16, 9, 
                                         UE_UMTS_CIPHER_ALGO_NONE, 0, 0,
                                         TRUE, FALSE, 0x0);
  
    FTM_MSG_HIGH("FTM TDSCMDA HSUPA, build EDCH PDU, E-TFCI = 0 !!!, QPSK",0,0,0);
  }
  else
  {}
  
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_SIM_EAGCH

DESCRIPTION    this function simulate HSUPA L1 and FW interface, simulate UE
               received E-AGCH every sub frame, serving grant item in E-AGCH
               is controlled by HSUPA non-signaling mode parameters in QRCT


DEPENDENCIES
   None.

RETURN VALUE
   E-AGCH data base pointer.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_sim_eagch(tdseulsg_agch_results_struct_type *ftm_eagch_info_ptr, uint16 ftm_curr_sub_fn)
{
  int16 i;

  memset(ftm_eagch_info_ptr, 0x0, sizeof(tdseulsg_agch_results_struct_type));

  if( ftm_tdscdma_ber_current_state == FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING )
  {
    return;
  }

  i = (ftm_curr_sub_fn-1) & 0x1;
  
  ftm_eagch_info_ptr[i].valid_flag = TRUE;
  ftm_eagch_info_ptr[i].sub_fn = (ftm_curr_sub_fn-1) & 8191;
  ftm_eagch_info_ptr[i].prri = 19;
  ftm_eagch_info_ptr[i].crri = ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri;
  ftm_eagch_info_ptr[i].trri = ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_ecsn++;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_ecsn &= 0x7;
  
  ftm_eagch_info_ptr[i].ecsn = ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_ecsn ;
  ftm_eagch_info_ptr[i].ei = 0;
  ftm_eagch_info_ptr[i].eni = (ftm_tdscdma_hsupa_db.eagch_epuch_cfg.num_e_ucch - 1) & 0x7;
  ftm_eagch_info_ptr[i].tpc = TPC_HOLD;
  ftm_eagch_info_ptr[i].rdi = 0;
  ftm_eagch_info_ptr[i].errorCnt = 0;
  ftm_eagch_info_ptr[i].qualityBit = 0;
  ftm_eagch_info_ptr[i].energy_metric = 0;
  ftm_eagch_info_ptr[i].ss = 0;
  
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_SIM_SCHLD_EHICH

DESCRIPTION    this function simulate HSUPA L1 and FW interface, simulate UE
               received scheduled E-HICH every sub frames, and always ACK


DEPENDENCIES
   None.

RETURN VALUE
   E-HICH data base pointer.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_sim_schld_ehich(tdseulsg_schld_ehich_result_struct_type *ftm_schld_ehich_ptr, uint16 ftm_curr_sub_fn)
{
   ftm_schld_ehich_ptr->valid_flag = TRUE;
   ftm_schld_ehich_ptr->ehich_sub_fn = (ftm_curr_sub_fn - 1) & 8191;
   ftm_schld_ehich_ptr->epuch_sub_fn = (ftm_curr_sub_fn - 3) & 8191;
   ftm_schld_ehich_ptr->ei = 0;
   ftm_schld_ehich_ptr->value = TRUE;
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_GET_EPUCH_TX_PWR

DESCRIPTION    this function return E-PUCH transmit power which is set in HSUPA
               non-signaling mode in QRCT


DEPENDENCIES
   None.

RETURN VALUE
   E-PUCH TX power in dBm.

SIDE EFFECTS
   None.

===========================================================================*/

int16 ftm_tdscdma_hsupa_get_epuch_tx_pwr(void)
{
  return ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_tx_pwr;
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/  
/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_INIT

DESCRIPTION    this function init database used for HSUPA non-signaling mode


DEPENDENCIES
   None.

RETURN VALUE
   E-PUCH TX power in dBm.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_hsupa_init(void)
{
  memset(&ftm_tdscdma_hsupa_db, 0x0, sizeof(ftm_tdscdma_hsupa_db));
  ftm_tdscdma_hsupa_db.isInitialized = TRUE;
}

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_START_OPS

DESCRIPTION    this function execute HSUPA non-signaling mode setup operation
               , update database, setup DL/UL DPCH and HSUPA channels accodingly,
               and send CPHY_SETUP message to L1


DEPENDENCIES
   None.

RETURN VALUE
   TRUE: setup successfully
   FALSE: error happens during setup, and HSUPA channel will not be setup.

SIDE EFFECTS
   None.

===========================================================================*/
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
boolean ftm_tdscdma_hsupa_start_ops(ftm_tdscdma_hsupa_setup_config_type *hsupa_config_ptr)
{
  tdsl1_api_cphy_setup_req_t msg;
  tdsl1_api_ftm_ns_cphy_setup_req_t *ftm_msg = &(msg.ftm_msg);
  boolean return_val = FALSE;
  
  if( ftm_tdscdma_hsupa_db.isInitialized == FALSE )
  {
    FTM_MSG_ERROR("New NS HSUPA Intf: ftm_tdshsdpa_bler_start_hs_channel: unitialized state",0,0,0);
  
      return FALSE;
  }

  FTM_MSG_HIGH("New NS HSUPA Intf: FTM TDSCDMA HSUPA start",0,0,0);
  
  ftm_tdscdma_ber_assign_cctrch_id();
  
  ftm_msg->req_mask                  = TDSL1_API_CPHY_DL_CCTRCH_INCL | 
                                       TDSL1_API_CPHY_DL_PHYCHAN_ADD_INCL |
                                       TDSL1_API_CPHY_UL_CCTRCH_INCL | 
                                       TDSL1_API_CPHY_UL_PHYCHAN_ADD_INCL | 
                                       TDSL1_API_CPHY_E_DL_CHAN_CFG_INCL | 
                                       TDSL1_API_CPHY_E_UL_CHAN_CFG_INCL;
  
  ftm_msg->freq_incl                 = FALSE;
  ftm_msg->primary_freq              = ftm_tdscdma_dl_db.current_rf_chan;
  ftm_msg->cell_parm_id              = 0;
  ftm_msg->max_tx_pwr                = hsupa_config_ptr->ul_max_tx_pwr;
  ftm_msg->working_freq_incl         = FALSE;
  
  ftm_msg->rmc_setup                 = TDSL1_FTM_BER_RMC_TYPE_12_2_SC;             
  ftm_msg->dl_slot                   = hsupa_config_ptr->dl_dpch_slot;                 
  ftm_msg->ul_slot                   = hsupa_config_ptr->ul_dpch_slot;                 

  ftm_msg->ul_tpc_step               = TDSL1_FTM_TPC_STEP_SIZE_1DB;           
  
  ftm_msg->dl_cctrch_id              = ftm_tdscdma_dl_cctrch_id;
  ftm_msg->working_freq              = ftm_tdscdma_dl_db.current_rf_chan;
         
  ftm_msg->epuch_slot = hsupa_config_ptr->epuch_slot;
  ftm_msg->epuch_code = hsupa_config_ptr->epuch_code;
  ftm_msg->epuch_tx_power = hsupa_config_ptr->epuch_tx_power;
  ftm_msg->mod_type = hsupa_config_ptr->mod_type;
  ftm_msg->num_eucch = hsupa_config_ptr->num_eucch; 

  ftm_tdscdma_dch_config.primary_freq   = ftm_tdscdma_dl_db.current_rf_chan;
  
   ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_PENDING);

  ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE;
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM)))
  return_val = tdsl1_api_send_setup_cmd(&msg);
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);    
  
   return TRUE;
}

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
boolean ftm_tdscdma_hsupa_start_ops(ftm_tdscdma_hsupa_setup_config_type *hsupa_config_ptr)
{
  
  tdsl1_setup_cmd_type          cmd;
  uint16 i;
  
  
  if( ftm_tdscdma_hsupa_db.isInitialized == FALSE )
  {
      FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: unitialized state",0,0,0);
  
      return FALSE;
  }

  FTM_MSG_HIGH("FTM TDSCDMA HSUPA start",0,0,0);
  
  /* Setup configuration for parameters as part of setup packet */
  //ftm_tdscdma_dch_config.rmc_setup        = dch_config_ptr->rmc_setup;
  ftm_tdscdma_dch_config.rmc_setup              = FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC;
  ftm_tdscdma_dch_config.dl_slot        = hsupa_config_ptr->dl_dpch_slot;  
  ftm_tdscdma_dch_config.ul_slot        = hsupa_config_ptr->ul_dpch_slot;  
  ftm_tdscdma_dch_config.ul_max_tx_pwr      = hsupa_config_ptr->ul_max_tx_pwr;  
  ftm_tdscdma_dch_config.ul_tpc_step      = TDSL1_TPC_STEP_SIZE_1DB;
  ftm_tdscdma_dch_config.rf_chan        = ftm_tdscdma_dl_db.current_rf_chan;
  ftm_tdscdma_dch_config.primary_freq   = ftm_tdscdma_dl_db.current_rf_chan;
  
  /* Setup default configuration for parameters not part of setup packet */
  ftm_tdscdma_dch_config.req_mask        = TDSL1_CPHY_DL_CCTRCH_INCL | 
                                           TDSL1_CPHY_DL_PHYCHAN_ADD_INCL |
                                           TDSL1_CPHY_UL_CCTRCH_INCL | 
                                           TDSL1_CPHY_UL_PHYCHAN_ADD_INCL;
  
  ftm_tdscdma_dch_config.add_dl_phychan     = TDSL1_DL_PHYCHAN_DPCH;
  ftm_tdscdma_dch_config.add_ul_phychan     = TDSL1_UL_PHYCHAN_DPCH;
  
  ftm_tdscdma_dch_config.ho_setup_mask      = 0;
  
  ftm_tdscdma_dch_config.beta_d       = 1;
  ftm_tdscdma_dch_config.n_312        = FTM_TDSCDMA_BER_N312_s1; //stuart
  ftm_tdscdma_dch_config.n_313        = FTM_TDSCDMA_BER_N313_s20;
  ftm_tdscdma_dch_config.n_315        = FTM_TDSCDMA_BER_N315_s1;
  ftm_tdscdma_dch_config.t_312        = 1;
  ftm_tdscdma_dch_config.t_313        = 3;
  
  ftm_tdscdma_dch_config.cell_parm_id      = 0;
  ftm_tdscdma_dch_config.freq_incl      = FALSE;
  ftm_tdscdma_dch_config.working_freq_incl  = FALSE;
         

  FTM_MSG_HIGH("DCH Setup: rf_chan=%d, rmc_setup=%d, ul_max_tx_pwr=%d",
                ftm_tdscdma_dch_config.rf_chan, 
                ftm_tdscdma_dch_config.rmc_setup,
                ftm_tdscdma_dch_config.ul_max_tx_pwr);
  
  FTM_MSG_HIGH("DCH Setup: dl_slot=%d, ul_slot=%d, ul_tpc_step=%d",
               ftm_tdscdma_dch_config.dl_slot,
               ftm_tdscdma_dch_config.ul_slot,ftm_tdscdma_dch_config.ul_tpc_step);

  ftm_tdscdma_dch_config.current_tb_size = 244;

  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_mod_type = hsupa_config_ptr->mod_type;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_tx_pwr = hsupa_config_ptr->epuch_tx_power;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.num_e_ucch = hsupa_config_ptr->num_eucch;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri= hsupa_config_ptr->epuch_code;

  switch(hsupa_config_ptr->epuch_slot)
  {
    case 1:
    {
      ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x10;
      break;
    }
    case 2:
    {
      ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x8;
      break;
    }
    case 3:
    {
      ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x4;
      break;
    }
    case 4:
    {
      ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x2;
      break;
    }
    case 5:
    {
      ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x1;
      break;
    }
    default:
    {
     
      FTM_MSG_ERROR( "error E-PUCH time slot %d",hsupa_config_ptr->epuch_slot,0,0);
      return FALSE;
      break;
    }
  }

   FTM_MSG_MED( "FTM TDSCDMA HSUPA, mod=%d,tx_pwr=%d,eucch=%d",ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_mod_type,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_tx_pwr,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.num_e_ucch);
   FTM_MSG_MED( "FTM TDSCDMA HSUPA, ts=%d,trri=%d,crri=%d",hsupa_config_ptr->epuch_slot,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri);

   ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_PENDING);

   ftm_tdscdma_hsupa_db.hsupa_req_mask = TDSL1_CPHY_E_DL_CHAN_CFG_INCL | TDSL1_CPHY_E_UL_CHAN_CFG_INCL;
   ftm_tdscdma_hsupa_db.hsupa_e_req_mask = TDSL1_SCHED_EUL_START;
  
   ftm_tdscdma_ber_assign_cctrch_id();

   ftm_tdscdma_ber_init_cphy_setup_cmd( &cmd, TRUE);
   /*DLDCH and ULDCH of HSUPA configuration on 8820C and CMW500 are different, need to be set separately
     DLDPCH timeslot of HSUPA configuration on 8820C is fixed on ts0, so using the timeslot of DLDPCH as the check point*/
   /*If 8820C*/
   if (hsupa_config_ptr->dl_dpch_slot == 0)
   {
     ftm_tdscdma_ber_dl_cphy_setup_cmd_8820C_UPA( &cmd);
     ftm_tdscdma_ber_ul_cphy_setup_cmd_8820C_UPA( &cmd);
   }
   /*If CMW500, using the common configuration*/   
   else
   {
     ftm_tdscdma_ber_dl_cphy_setup_cmd( &cmd);
     ftm_tdscdma_ber_ul_cphy_setup_cmd( &cmd);
   }

   /** use midamble K = 8 */
   for(i=0;i<cmd.add_dl_phychan_db->dpch.dl_cctrch_list.num_ts;i++)
   {
     cmd.add_dl_phychan_db->dpch.dl_cctrch_list.individual_ts_info[i].midamble_ss_burst_type.config = TDSL1_MIDAMBLE_CONFIG_8;
   }

   for(i=0;i<cmd.add_ul_phychan_db->dpch_cfg.num_ts;i++)
   {
     cmd.add_ul_phychan_db->dpch_cfg.ts_list[i].midamble_and_burst.config = TDSL1_MIDAMBLE_CONFIG_8;
   }
   
   ftm_tdscdma_hsupa_init_edch_cphy_setup_cmd( &cmd);
   ftm_tdscdma_hsupa_cphy_setup_cmd( &cmd);
  
   ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);

   ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE;

   return TRUE;
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/

/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_RECONFIG_OPS

DESCRIPTION    this function execute HSUPA non-signaling mode reconfig operations.
               support change non-signling mode parameters, and also turn on/off
               E-PUCH during the call


DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
boolean ftm_tdscdma_hsupa_reconfig_ops(ftm_tdscdma_hsupa_setup_config_type *hsupa_config_ptr)
{
    
  tdsl1_api_cphy_setup_req_t msg;
  tdsl1_api_ftm_ns_cphy_setup_req_t *ftm_msg = &(msg.ftm_msg);
  boolean return_val = FALSE;
  
  if( ftm_tdscdma_hsupa_db.isInitialized == FALSE )
  {
    FTM_MSG_ERROR("ftm tds hsupa: unitialized state",0,0,0);
  
    return FALSE;
  }


  FTM_MSG_HIGH("New NS HSUPA Intf: FTM TDSCDMA HSUPA reconfig",0,0,0);

  ftm_tdscdma_dch_config.is_dch_refcg = TRUE;
 
  if(hsupa_config_ptr->is_hsupa_on_flag == 0 )
  {
	/** turn OFF HSUPA NS */
	if(ftm_tdscdma_hsupa_db.current_state == FTM_TDSCDMA_HSUPA_CHAN_STATE_INACTIVE)
	{
	  /** HSUPA channel is already inactive, no need to config HSUPA channel */
	  FTM_MSG_HIGH("FTM TDSCDMA HSUPA is OFF now, do nothing",0,0,0);
  
	  return TRUE;
	}
  }
  else if(hsupa_config_ptr->is_hsupa_on_flag > 1)
  {
    FTM_MSG_ERROR("invalid FTM TDSCDMA HSUPA ON/OFF flag %d",hsupa_config_ptr->is_hsupa_on_flag,0,0);
    return FALSE;
  }
  
  ftm_tdscdma_ber_assign_cctrch_id();
  
  ftm_msg->req_mask                  = TDSL1_API_CPHY_DL_CCTRCH_INCL |
                                       TDSL1_API_CPHY_UL_CCTRCH_INCL |
                                       TDSL1_API_CPHY_DL_PHYCHAN_CFG_INCL | 
                                       TDSL1_API_CPHY_UL_PHYCHAN_CFG_INCL| 
                                       TDSL1_API_CPHY_E_DL_CHAN_CFG_INCL | 
                                       TDSL1_API_CPHY_E_UL_CHAN_CFG_INCL;
  
  ftm_msg->freq_incl                 = FALSE;
  ftm_msg->primary_freq              = ftm_tdscdma_dch_config.primary_freq;
  ftm_msg->cell_parm_id              = 0;
  ftm_msg->max_tx_pwr                = hsupa_config_ptr->ul_max_tx_pwr;
  ftm_msg->working_freq_incl         = TRUE;
  
  ftm_msg->rmc_setup                 = TDSL1_FTM_BER_RMC_TYPE_12_2_SC;             
  ftm_msg->dl_slot                   = hsupa_config_ptr->dl_dpch_slot;                 
  ftm_msg->ul_slot                   = hsupa_config_ptr->ul_dpch_slot;                 

  ftm_msg->ul_tpc_step               = TDSL1_FTM_TPC_STEP_SIZE_1DB;           
  
  ftm_msg->dl_cctrch_id              = ftm_tdscdma_dl_cctrch_id;
  ftm_msg->working_freq              = hsupa_config_ptr->rf_chan;
  
  ftm_msg->epuch_slot = hsupa_config_ptr->epuch_slot;
  ftm_msg->epuch_code = hsupa_config_ptr->epuch_code;
  ftm_msg->epuch_tx_power = hsupa_config_ptr->epuch_tx_power;
  ftm_msg->mod_type = hsupa_config_ptr->mod_type;
  ftm_msg->num_eucch = hsupa_config_ptr->num_eucch; 
           
  FTM_MSG_HIGH("FTM TDSCDMA HSUPA recfg to send to Tl1",0,0,0);
#if !(defined(TEST_FRAMEWORK)  && (defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_BOLT_MODEM))) 
  return_val = tdsl1_api_send_setup_cmd(&msg);
#else
  #error code not present
#endif
  ASSERT( return_val == TRUE);
  
  FTM_MSG_HIGH("FTM TDSCDMA HSUPA recfg been send to Tl1",0,0,0);
                                         
  if(hsupa_config_ptr->is_hsupa_on_flag == 1)
  {
      /** HSUPA NS is already running, use reconfig */
      ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);

    return TRUE;
  }
  else if(hsupa_config_ptr->is_hsupa_on_flag == 0)
  {
    /** turn OFF HSUPA NS */
	if(ftm_tdscdma_hsupa_db.current_state == FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE)
	{
	  /** HSUPA NS is already running, use stop */
	  ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);
	  
	  ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_INACTIVE;
	  
	  return TRUE;
	}
	else
	{
	  /** HSUPA channel is already inactive, no need to config HSUPA channel */
	  FTM_MSG_HIGH("FTM TDSCDMA HSUPA is OFF now, do nothing",0,0,0);

	  return TRUE;
	}
  }
  else
  {
    FTM_MSG_ERROR("invalid FTM TDSCDMA HSUPA ON/OFF flag %d",hsupa_config_ptr->is_hsupa_on_flag,0,0);
    return FALSE;
  }

  return TRUE;
}

#else /*FEATURE_RF_TL1_FTM_NEW_INTF*/
boolean ftm_tdscdma_hsupa_reconfig_ops(ftm_tdscdma_hsupa_setup_config_type *hsupa_config_ptr)
{
  tdsl1_setup_cmd_type          cmd;
  uint16                        i;
  
  if( ftm_tdscdma_hsupa_db.isInitialized == FALSE )
  {
    FTM_MSG_ERROR("ftm_tdshsdpa_bler_start_hs_channel: unitialized state",0,0,0);
  
    return FALSE;
  }


  FTM_MSG_HIGH("FTM TDSCDMA HSUPA reconfig",0,0,0);

  ftm_tdscdma_dch_config.is_dch_refcg = TRUE;
 
  if(hsupa_config_ptr->is_hsupa_on_flag == 0 )
  {
	/** turn OFF HSUPA NS */
	if(ftm_tdscdma_hsupa_db.current_state == FTM_TDSCDMA_HSUPA_CHAN_STATE_INACTIVE)
	{
	  /** HSUPA channel is already inactive, no need to config HSUPA channel */
	  FTM_MSG_HIGH("FTM TDSCDMA HSUPA is OFF now, do nothing",0,0,0);
  
	  return TRUE;
	}
  }
  else if(hsupa_config_ptr->is_hsupa_on_flag > 1)
  {
    FTM_MSG_ERROR("invalid FTM TDSCDMA HSUPA ON/OFF flag %d",hsupa_config_ptr->is_hsupa_on_flag,0,0);
    return FALSE;
  }
  
  /* Setup configuration for parameters as part of setup packet */
  //ftm_tdscdma_dch_config.rmc_setup        = dch_config_ptr->rmc_setup;
  ftm_tdscdma_dch_config.rmc_setup        = FTM_TDSCDMA_BER_RMC_TYPE_12_2_SC;
  ftm_tdscdma_dch_config.dl_slot        = hsupa_config_ptr->dl_dpch_slot;  
  ftm_tdscdma_dch_config.ul_slot        = hsupa_config_ptr->ul_dpch_slot;  
  ftm_tdscdma_dch_config.ul_max_tx_pwr      = hsupa_config_ptr->ul_max_tx_pwr;  
  ftm_tdscdma_dch_config.ul_tpc_step      = TDSL1_TPC_STEP_SIZE_1DB;
  ftm_tdscdma_dch_config.rf_chan        = hsupa_config_ptr->rf_chan;
  
  /* Setup default configuration for parameters not part of setup packet */
  ftm_tdscdma_dch_config.req_mask = TDSL1_CPHY_DL_CCTRCH_INCL |
                                    TDSL1_CPHY_UL_CCTRCH_INCL |
                                    TDSL1_CPHY_DL_PHYCHAN_CFG_INCL | 
                                    TDSL1_CPHY_UL_PHYCHAN_CFG_INCL;
  
  ftm_tdscdma_dch_config.cfg_dl_phychan = TDSL1_DL_PHYCHAN_DPCH;
  ftm_tdscdma_dch_config.cfg_ul_phychan = TDSL1_UL_PHYCHAN_DPCH;
  
  
  ftm_tdscdma_dch_config.ho_setup_mask    = 0;
  
  ftm_tdscdma_dch_config.beta_d        = 1;
  ftm_tdscdma_dch_config.n_312        = FTM_TDSCDMA_BER_N312_s1; //stuart
  ftm_tdscdma_dch_config.n_313        = FTM_TDSCDMA_BER_N313_s20;
  ftm_tdscdma_dch_config.n_315        = FTM_TDSCDMA_BER_N315_s1;
  ftm_tdscdma_dch_config.t_312        = 1;
  ftm_tdscdma_dch_config.t_313        = 3;
  
  ftm_tdscdma_dch_config.cell_parm_id     = 0;
  ftm_tdscdma_dch_config.freq_incl      = FALSE;
  ftm_tdscdma_dch_config.working_freq_incl  = FALSE;
           
  
  FTM_MSG_HIGH("  DCH reconfig: rf_chan=%d, rmc_setup=%d, ul_max_tx_pwr=%d",
           ftm_tdscdma_dch_config.rf_chan, 
           ftm_tdscdma_dch_config.rmc_setup,
           ftm_tdscdma_dch_config.ul_max_tx_pwr);
  
  FTM_MSG_HIGH("  DCH reconfig: dl_slot=%d, ul_slot=%d, ul_tpc_step=%d",
           ftm_tdscdma_dch_config.dl_slot,
           ftm_tdscdma_dch_config.ul_slot,ftm_tdscdma_dch_config.ul_tpc_step);
  
  ftm_tdscdma_dch_config.current_tb_size = 244;
  
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_mod_type = hsupa_config_ptr->mod_type;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_tx_pwr = hsupa_config_ptr->epuch_tx_power;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.num_e_ucch = hsupa_config_ptr->num_eucch;
  ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri= hsupa_config_ptr->epuch_code;
  
  switch(hsupa_config_ptr->epuch_slot)
  {
    case 1:
    {
     ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x10;
     break;
    }
    case 2:
    {
     ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x8;
     break;
    }
    case 3:
    {
     ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x4;
     break;
    }
    case 4:
    {
     ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x2;
     break;
    }
    case 5:
    {
     ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri = 0x1;
     break;
    }
    default:
    {
     FTM_MSG_ERROR( "error E-PUCH time slot %d",hsupa_config_ptr->epuch_slot,0,0);
     return FALSE;
     break;
    }
  }
  
  FTM_MSG_MED( "FTM TDSCDMA HSUPA, mod=%d,tx_pwr=%d,eucch=%d",ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_mod_type,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.epuch_tx_pwr,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.num_e_ucch);
  FTM_MSG_MED( "FTM TDSCDMA HSUPA, ts=%d,trri=%d,crri=%d",hsupa_config_ptr->epuch_slot,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_trri,ftm_tdscdma_hsupa_db.eagch_epuch_cfg.eagch_crri);
  
  /* Send CPHY_SETUP_REQ with reconfig commands */
  ftm_tdscdma_dch_config.working_freq_incl = TRUE;
  ftm_tdscdma_dch_config.working_freq      = hsupa_config_ptr->rf_chan; 
  
  FTM_MSG_HIGH("  DCH ReCfg: rf_chan=%d, working_freq=%d",
           ftm_tdscdma_dch_config.rf_chan,
           ftm_tdscdma_dch_config.working_freq,0);

  ftm_tdscdma_ber_assign_cctrch_id();
                                         
  ftm_tdscdma_ber_init_cphy_setup_cmd( &cmd, TRUE);

  /*DLDCH and ULDCH of HSUPA configuration on 8820C and CMW500 are different, need to be set separately
    DLDPCH timeslot of HSUPA configuration on 8820C is fixed on ts0, so using the timeslot of DLDPCH as the check point*/
  /*If 8820C*/
  if (hsupa_config_ptr->dl_dpch_slot == 0)
  {
    ftm_tdscdma_ber_dl_cphy_setup_cmd_8820C_UPA( &cmd);
    ftm_tdscdma_ber_ul_cphy_setup_cmd_8820C_UPA( &cmd);
  }
  /*If CMW500, using the common configuration*/   
  else
  {
    ftm_tdscdma_ber_dl_cphy_setup_cmd( &cmd);
    ftm_tdscdma_ber_ul_cphy_setup_cmd( &cmd);
  }
  cmd.primary_freq = ftm_tdscdma_dch_config.primary_freq;
    
  // use midamble K = 8
  for(i=0; i<cmd.cfg_dl_phychan_db->dpch.dl_cctrch_list.num_ts; i++)
  {
    cmd.cfg_dl_phychan_db->dpch.dl_cctrch_list.individual_ts_info[i].midamble_ss_burst_type.config = TDSL1_MIDAMBLE_CONFIG_8;
  }
                                         
  for(i=0; i<cmd.cfg_ul_phychan_db->dpch_cfg.num_ts; i++)
  {
    cmd.cfg_ul_phychan_db->dpch_cfg.ts_list[i].midamble_and_burst.config = TDSL1_MIDAMBLE_CONFIG_8;
  }
  
  if(hsupa_config_ptr->is_hsupa_on_flag == 1)
  {
    /** still use HSUPA NS */
    if(ftm_tdscdma_hsupa_db.current_state == FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE)
    {
      /** HSUPA NS is already running, use reconfig */
      ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);

      ftm_tdscdma_hsupa_db.hsupa_req_mask =   TDSL1_CPHY_E_DL_CHAN_CFG_INCL | TDSL1_CPHY_E_UL_CHAN_CFG_INCL;
      ftm_tdscdma_hsupa_db.hsupa_e_req_mask = TDSL1_EUL_RECFG_UL | TDSL1_SCHED_EUL_RECFG_DL;

      ftm_tdscdma_hsupa_init_edch_cphy_setup_cmd( &cmd);
      ftm_tdscdma_hsupa_cphy_setup_cmd( &cmd);
  
      ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);

      ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE;
      
      FTM_MSG_HIGH("TURN ON FTM TDSCDMA HSUPA, req_mask=0x%x,e_req_mask=0x%x,state=%d",
	                ftm_tdscdma_hsupa_db.hsupa_req_mask,ftm_tdscdma_hsupa_db.hsupa_e_req_mask,ftm_tdscdma_hsupa_db.current_state);
    }
    else
    {
      /** HSUPA NS is inactive, use setup/start */
      ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);

      ftm_tdscdma_hsupa_db.hsupa_req_mask =   TDSL1_CPHY_E_DL_CHAN_CFG_INCL | TDSL1_CPHY_E_UL_CHAN_CFG_INCL;
      ftm_tdscdma_hsupa_db.hsupa_e_req_mask = TDSL1_SCHED_EUL_START;

      ftm_tdscdma_hsupa_init_edch_cphy_setup_cmd( &cmd);
      ftm_tdscdma_hsupa_cphy_setup_cmd( &cmd);
  
      ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);

      ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE;
     
      FTM_MSG_HIGH("TURN ON FTM TDSCDMA HSUPA, req_mask=0x%x,e_req_mask=0x%x,state=%d",
	                ftm_tdscdma_hsupa_db.hsupa_req_mask,ftm_tdscdma_hsupa_db.hsupa_e_req_mask,ftm_tdscdma_hsupa_db.current_state);
    }

    return TRUE;
  }
  else if(hsupa_config_ptr->is_hsupa_on_flag == 0)
  {
    /** turn OFF HSUPA NS */
	if(ftm_tdscdma_hsupa_db.current_state == FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE)
	{
	  /** HSUPA NS is already running, use stop */
	  ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_RECONFIG_PENDING);
	  
	  ftm_tdscdma_hsupa_db.hsupa_req_mask =   TDSL1_CPHY_E_DL_CHAN_CFG_INCL | TDSL1_CPHY_E_UL_CHAN_CFG_INCL;
	  ftm_tdscdma_hsupa_db.hsupa_e_req_mask = TDSL1_EUL_STOP;
	
	  ftm_tdscdma_hsupa_init_edch_cphy_setup_cmd( &cmd);
	  ftm_tdscdma_hsupa_cphy_setup_cmd( &cmd);
	
	  ftm_tdscdma_ber_send_CPHY_SETUP_REQ( &cmd);
	
	  ftm_tdscdma_hsupa_db.current_state = FTM_TDSCDMA_HSUPA_CHAN_STATE_INACTIVE;
	  
	  FTM_MSG_HIGH("TURN OFF FTM TDSCDMA HSUPA, req_mask=0x%x,e_req_mask=0x%x,state=%d",
	                ftm_tdscdma_hsupa_db.hsupa_req_mask,ftm_tdscdma_hsupa_db.hsupa_e_req_mask,ftm_tdscdma_hsupa_db.current_state);

	  return TRUE;
	}
	else
	{
	  /** HSUPA channel is already inactive, no need to config HSUPA channel */
	  FTM_MSG_HIGH("FTM TDSCDMA HSUPA is OFF now, do nothing",0,0,0);

	  return TRUE;
	}
  }
  else
  {
    FTM_MSG_ERROR("invalid FTM TDSCDMA HSUPA ON/OFF flag %d",hsupa_config_ptr->is_hsupa_on_flag,0,0);
    return FALSE;
  }

  return TRUE;
}

#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/


/*===========================================================================

FUNCTION       FTM_TDSCDMA_HSUPA_DISPATCH

DESCRIPTION    this is the dispatch function for all HSUPA related command,
               include setup, and reconfig HSUPA


DEPENDENCIES
   None.

RETURN VALUE
   ftm_rsp_pkt_type, a struct include success/fail info when execute external command.

SIDE EFFECTS
   None.

===========================================================================*/

ftm_rsp_pkt_type ftm_tdscdma_hsupa_dispatch( ftm_tdscdma_ber_pkt_type *cmd_ptr )
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_tdscdma_ber_sync_status_type          cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
  
  // Variables for synchronous status
  ftm_tdscdma_hsupa_setup_config_type *hsupa_config_ptr;
  
  hsupa_config_ptr = &(( (ftm_tdscdma_hsupa_setup_type *) cmd_ptr)->hsupa_config);
  
  FTM_MSG_HIGH( "FTM TDSCDMA HSUPA dispatch, Received cmd id: %d, curent BER state: %d", 
                 cmd_ptr->ftm_cmd_header.cmd_id,ftm_tdscdma_ber_current_state,0);
  
  switch( cmd_ptr->ftm_cmd_header.cmd_id)
  {
    /***********************************************************************
    *       START / RECONFIG HSUPA FTM                                     *
    *                                                                      *
    ************************************************************************/
    case FTM_TDSCDMA_HSUPA_CMD_SETUP:
    {
    
      // Assume pass until failure is detected
      cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
    
      /* Have we:
       *    - successfully acquired pilot already OR in DCH to do a reconfig 
       * If neither, then declare command invalid.
       */
      
      if( !( ftm_tdscdma_ber_current_state == FTM_TDSCDMA_BER_STATE_ACQ &&
           ftm_tdscdma_dl_db.current_rf_chan_active == TRUE)
        && !( ftm_tdscdma_ber_current_state== FTM_TDSCDMA_BER_STATE_DCH)
        )
      {
      
        cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
        FTM_MSG_ERROR("TDS NS: DCH Setup: invalid state, not in Acq or Dch state",0,0,0);
        break;
      }
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
      if (!ftm_tdscdma_dl_db.isInitialized || !ftm_tdscdma_ul_db.isInitialized)
      {
         cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
         FTM_MSG_ERROR("TDS NS: DCH Setup: dl or ul db not initialized. tds mode started?",0,0,0);
         break;
      }
#endif /*FEATURE_RF_TL1_FTM_NEW_INTF*/
      if(hsupa_config_ptr->ul_dpch_slot == hsupa_config_ptr->epuch_slot)
      {
         cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
         FTM_MSG_ERROR("FTM TDSCDMA HSUPA, E-PUCH and UL DPCH timeslot must be different",0,0,0);
         break;
      }
       
      if( cmd_status != FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE )
      {
        if( ftm_tdscdma_ber_current_state == FTM_TDSCDMA_BER_STATE_ACQ)
        {
          FTM_MSG_MED( "Setting up HSUPA FTM from ACQ state.",0,0,0);
    
          ftm_tdscdma_ber_set_state( FTM_TDSCDMA_BER_STATE_DCH_PENDING);
    
          if(ftm_tdscdma_hsupa_start_ops(hsupa_config_ptr))
          {
            cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
          }
          else
          {
            cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
          }
        }
        else
        {
            if(ftm_tdscdma_hsupa_reconfig_ops(hsupa_config_ptr))
            {
              cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_SUCCESS;
            }
            else
            {
              cmd_status = FTM_TDSCDMA_BER_SYNC_STATUS_GEN_FAILURE;
            }
        }
      }

      break;
    }
  
    /***********************************************************************
    *       DEFAULT                                                        *
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
                   ftm_tdscdma_ber_current_state, 0);
   }

   rsp_pkt = ftm_tdscdma_ber_create_gen_response(cmd_ptr,
                          (uint16) cmd_status,
                          cmd_ptr->ftm_cmd_header.cmd_id,
                          0);

  return( rsp_pkt);
}

#endif /* FTM_HAS_TDS_HSUPA */
#endif /* FTM_HAS_TDSCDMA_BER */
#endif /* FEATURE_FACTORY_TESTMODE */
