#ifndef __FTM_CMD_H
#define __FTM_CMD_H

/*===========================================================================

                  FACTORY TEST COMMAND INTERFACE
DESCRIPTION

  This file contains prototypes, #defines, and enums which constitute
  the public interface for FTM functions intended to be called via
  the diag mechanism.

    Copyright (c) 1999 - 2012
                  by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/ftm_cmd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/13   sar     Updates for APQ, GNSS only target.
09/22/12   APU     Added FTM support for setting the individual pilot carrier 
                   power.
05/28/12   lp      Removed rfa_variation.h include file
04/24/12   aro     Interface to return randomized payload for DOrB
09/16/11   Saul    HDR non-sig support.
06/16/11   sar     Relocated private definitions.
06/15/11   sar     Removing deprecated function prototypes.
05/09/11   sar     Removed ftmicap.h for cmi-4 cleanup.
04/08/11   sar     Removed condition #def's for CMI-4.
04/04/11   aro     Added support for 1xNS
02/04/11   jtm/bmg Added support for 1xA testing.
10/08/10   anr     1x/DO Factory Features
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
03/06/08   aak     Added support for FTM HHO 
01/25/07   ak      Merged changes from version 9 in the main branch
08/17/06   ak      Augmented the sturctures "hdr_ftm_cmd_mod_rtc_param_type" and
                   "hdr_ftm_cmd_mod_rtc_rela_param_type" by adding to them the
                   parameter "byte hdr_ack_mode"
12/12/05   vm      Fixed the parameters ordering for structure 
                   hdr_ftm_cmd_mod_rtc_rela_param_type.
                   Removed the ambiguous function prototype.
12/06/05   ra      Added hdr_ftm_rela_mac_config - DO Reva API prototype
10/30/05   vm      Added support for new FTM DO Reva APIs
09/20/05   jac     Added enums, structures and prototypes for new API for 
                   1x EVDO Rev A testing.
09/08/05   ra      Added ftmicap.h
07/19/05   ra      Added more DO REV A support
05/03/05   ra      Added DO REV A support
07/06/04   bmg     Added prototypes for release A functions
05/18/04   rsr     Removed WINNT and SWIG support. Not being used
11/11/03   bmg     Merge Cougar release A
09/23/03   wd      Added new types and defs for simultaneous FCH and DCCH (Release A)
09/09/03   bmg     Added ftm_mc_cmd_resp_type for FTM MC command response
07/23/03   bmg     Converted test_* names to ftm_* names
02/10/03   wd      Marge files for FTM Task, Also added p4 info and clean up.
01/24/03   wd      Decouple legacy FTM from FTM_HWTC and FTM_RF
02/21/02   rjr     Support for RF commands and dependancy on rftest.h.
08/22/01   bn      Support DCCH in FTM mode
02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to enable a runtime choice.
11/15/00   bgc     ftm_cmd_status has been changed to test_cmd_status.
08/22/00   bgc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "test_cmd.h"
#include "ftm_cdma_ns_types.h" /* 1x and HDR non-signaling command types */
#include "diagpkt.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "hdrovhd.h"
#include "onex_non_sig.h"
#endif
#include "rfm_cdma_carrier_types.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*--------------------------------------------------------------------------
                        FTM MODE ENUM TYPE
--------------------------------------------------------------------------*/

typedef enum
{
  FTM_MODE_DMSS,
  FTM_MODE_FTM,
  ftm_mode_sizer = 0x7FFFFFFF
} ftm_cmd_mode_type;

/* ftm_packed_param_type is a union of all the above command 
 * parameters.  All ftm commands take this type as the argument */

typedef PACK(union)
{
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  onex_nonsig_packed_param_type           onex;  /* union of packed 1x commands */
  hdr_ftm_cmd_hho_type                    hdr_hho;
  hdr_ftm_cmd_acq_pilot_param_type        hdr_acq;
  hdr_ftm_cmd_demod_ftc_param_type        hdr_demod_ftc;
  hdr_ftm_cmd_mod_acc_param_type          hdr_mod_acc;
  hdr_ftm_cmd_mod_rtc_param_type          hdr_mod_rtc;
  hdr_ftm_cmd_fwd_link_rel0_param_type    hdr_fwd_link_rel0;
  hdr_ftm_rela_mac_config_param_type      hdr_rela_mac_config;
  hdr_ftm_cmd_fwd_link_rela_param_type    hdr_fwd_link_rela;
  hdr_ftm_cmd_mod_acc_rela_param_type     hdr_mod_acc_rela;
  hdr_ftm_cmd_mod_rtc_rela_param_type     hdr_mod_rtc_rela;
  hdr_ftm_cmd_mod_rtc_relb_param_type     hdr_mod_rtc_relb;
  hdr_ftm_cmd_config_rtc_relb_param_type  hdr_config_rtc_relb;
#endif
  ftm_cmd_rx_level_param_type             rx_level_req_params;  /*parameters for Rx power request*/
  ftm_cmd_rev_link_pwr_setting_param_type r_tx_power; /* reverse power setting in dBm*/
  ftm_cdma_carrier_tx_pwr_cmd_param_type  hdr_tx_carrier_power ; /* reverse carrier power setting in dBm*/
} ftm_packed_param_type;

/*===========================================================================

  
===========================================================================*/
typedef enum {
  FTM_FIRST_CMD = 100,
  FTM_ACQUIRE_SC = 100,
  FTM_SYNC_SC =    101,
  FTM_FWD_FCH_SC = 102,
  FTM_FWD_SCH_SC = 103,
  FTM_REV_FCH_SC = 104,
  FTM_REV_SCH_SC = 105,
  FTM_FCH_LOOP_SC = 106,
  FTM_SCH_LOOP_SC = 107,
  FTM_RELEASE_SC = 108,
  FTM_SET_MODE_SC = 109,
  FTM_FWD_DCCH_SC = 110,
  FTM_REV_DCCH_SC = 111,
  FTM_DCCH_LOOP_SC = 112,
  HDR_FTM_ACQUIRE = 113,
  HDR_FTM_SYNC = 114,
  HDR_FTM_DEMOD_FTC = 115,
  HDR_FTM_MOD_ACC = 116,
  HDR_FTM_MOD_RTC = 117,
  HDR_FTM_DEASSIGN = 118,
  FTM_FWD_FCH_DCCH_SC = 120,
  FTM_REV_FCH_DCCH_SC = 121,
  
  HDR_FTM_DEMOD_FWLINK_REV0 = 122,
  HDR_FTM_IDLE_STATE = 123,
  HDR_FTM_CONFIG_SAME_CHANNEL = 133,
  HDR_FTM_RELA_MAC_CONFIG = 124,
  HDR_FTM_MOD_ACC_REVA  = 125,
  HDR_FTM_MOD_RTC_REVA  = 126,
  HDR_FTM_DEMOD_FWLINK_REVA  = 127,
  FTM_FWD_HHO_SC = 128,
  HDR_FTM_MOD_RTC_REVB  = 131,
  HDR_FTM_CONFIG_RTC_REVB  = 132,
  HDR_FTM_CONFIG_RTC_TX_PATTERN_REVB  = 134,
  HDR_FTM_REV_SET_PWR       = 135,
  FTM_1XA_RCPM_DATA_SC      = 136,
  FTM_PAGING_SC             = 137,
  FTM_ACCESS_PROBE_SC       = 138,
  FTM_REV_SET_PWR_SC        = 140,
  FTM_FCH_RESET_STATS_SC    = 141,
  FTM_FCH_GET_STATS_SC      = 142,
  HDR_FTM_HRD_HND_OFF       = 143,
  HDR_FTM_RST_FWD_LNK_STATS = 144,
  HDR_FTM_GET_FWD_LNK_STATS = 145,
  FTM_1X_RX_LEVEL_REQ       = 146,
  FTM_HDR_RX_LEVEL_REQ      = 147,
  HDR_FTM_SET_CARRIER_TX_PWR      = 148,
  FTM_MAX_CMD 
} ftm_subcmd_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type diag_hdr;
  word                       sub_cmd;
  uint8                      status;
} ftm_mc_cmd_resp_type;

typedef PACK(struct)
{
    uint32 fch_frame_total;
    uint32 fch_frame_errors;
    uint32 fch_sym_total;
    uint32 fch_sym_errors;
} ftm_rxc_fer_ser_report_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type  diag_hdr;
  word                        sub_cmd;
  uint8                       status;
  ftm_rxc_fer_ser_report_type ftm_rxc_fer_ser_report;
} ftm_fch_stats_rsp_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type  diag_hdr;
  word                        sub_cmd;
  uint8                       status;
  uint32                      total_tc_packets;
  uint32                      total_tc_errors;
} ftm_hdr_per_rsp_type;

typedef PACK(struct)
{
  byte  rx_chain;
  int16 rssi;
  int16 rx_pwr_dbm10;
}ftm_rx_level_report_type;

typedef PACK(struct)
{
  diagpkt_subsys_header_type  diag_hdr;
  word                        sub_cmd;
  ftm_cmd_status_type         status;
  ftm_rx_level_report_type    ftm_rx_report;
}ftm_rx_level_rsp_type;

/*===========================================================================

FUNCTION FTM_HDR_GET_RX_LEVEL_REPORT

DESCRIPTION
  This procedure processes a FTM command for getting HDR Rx power values.
          
DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, 
  TEST_INVALID_CMD,  indicating the command is invalid or has invalid parameters, or 
  TEST_NO_SUCCESS, indicating the command is issued at a wrong phone state.
  
SIDE EFFECTS
  None.

===========================================================================*/
ftm_cmd_status_type ftm_hdr_rx_level_report
(
  ftm_packed_param_type *p_param, 
  ftm_rx_level_report_type* ftm_rx_level_report
);

/*============================================================================

FUNCTION      FTM_COMMAND_DISPATCHER

DESCRIPTION   This function will call the appropriate subcommand
              for a given FTM command.
DEPENDENCIES   

RETURN VALUE  TRUE if there was a buffer available and the
              command type is in range.  FALSE otherwise.

SIDE EFFECTS  A command will be placed on the FTM_MCC command queue
      
============================================================================*/
ftm_cmd_status_type ftm_cmd_dispatcher
(
  byte sub_cmd_code,
  word length,
  ftm_packed_param_type *param
);

#ifdef T_WINNT  /* return to normal packing */
#error code not present
#endif
       
/*----------------------------------------------------------------------------*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
void hdrftm_acmac_get_ap_msg( hdrovhd_ap_msg_type * ap_msg);
#endif
/*----------------------------------------------------------------------------*/
uint16
ftm_hdr_get_rl_payload
(
  rfm_cdma_carrier_type carrier,
  uint8* rl_payload
);

#endif /* __FTM_CMD_H */
