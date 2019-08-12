#ifndef HDRHIT_MSG_H
#define HDRHIT_MSG_H

/*===========================================================================
      H D R    HIT    M E S S A G E S    D E F I N I T I O N

GENERAL DESCRIPTION
      This file contains hdr hit messages definitions

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/hdrhit_msg.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
----------   ---     -------------------------------------------------------- 
08/06/2013   rmg     Added support to skip DAC toggle in modulator ctl cmd.
07/27/2012   rmg     Added support for configuring RL funnel mode.
06/08/2012   dsp/rmg Changed HDR_HIT_CMD_ENABLE_MODULATOR 
                     to HDR_HIT_CMD_MODULATOR_CTL
05/14/2012   dsp/rmg Added these commands: 
                     HDR_HIT_CMD_ENABLE_MODULATOR, 
                     HDR_HIT_CMD_GET_FWD_LINK_STATS,
                     HDR_HIT_CMD_RESET_FWD_LINK_STATS,
                     Added hdrhitmsg_get_fwd_lnk_stats().
05/04/2012   smd     Exposed hdrhitmsg_get_response_status(). 
04/06/2012   dsp     Updated hdrhit_hard_handoff_cmd_msg_t structure
02/27/2012   smd     Added updates of FTM msgr interface changes.
06/22/2011   smd     Created this file.
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include <sys.h>
#include "hdrmsg.h"

#include "sys_v.h"

#include "hdrhit_api.h"
#ifdef __cplusplus
}  // extern "C"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Commands the HDR HIT module receives. */
enum
{
  HDR_HIT_MSG_CMD_FIRST = MSGR_UMID_BASE(MSGR_HDR_HIT, MSGR_TYPE_CMD),    
  MSGR_DEFINE_UMID(HDR, HIT, CMD, PIL_ACQ,                 0x0,  hdrhit_pil_acq_cmd_msg_t),             /* hdr_ftmi_pil_acq */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, SYNC,                    0x1,  hdrhit_sync_cmd_msg_t),                /* hdr_ftmi_sync */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, DEMOD_FTC,               0x2,  hdrhit_demod_ftc_cmd_msg_t),           /* hdr_ftmi_demod_ftc */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, MOD_ACC,                 0x3,  hdrhit_mod_acc_cmd_msg_t),             /* hdr_ftmi_mod_acc */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, MOD_RTC,                 0x4,  hdrhit_mod_rtc_cmd_msg_t),             /* hdr_ftmi_mod_rtc */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, DEASSIGN,                0x5,  hdrhit_deassign_cmd_msg_t),            /* hdr_ftmi_deassign */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, IDLE_STATE,              0x6,  hdrhit_idle_state_cmd_msg_t),          /* hdr_ftmi_idle_state */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, DEMOD_FW_LINK_REV0,      0x7,  hdrhit_demod_fw_link_rev0_cmd_msg_t),  /* hdr_ftmi_demod_fw_link_rev0 */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, RELA_MAC_CONFIG,         0x8,  hdrhit_rela_mac_config_cmd_msg_t),     /* hdr_ftmi_rela_mac_config */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, DEMOD_FW_LINK_REVA,      0x9,  hdrhit_demod_fw_link_reva_cmd_msg_t),  /* hdr_ftmi_demod_fw_link_reva */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, MOD_ACC_REVA,            0xa,  hdrhit_mod_acc_reva_cmd_msg_t),        /* hdr_ftmi_mod_acc_reva */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, MOD_RTC_REVA,            0xb,  hdrhit_mod_rtc_reva_cmd_msg_t),        /* hdr_ftmi_mod_rtc_reva */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, MOD_RTC_REVB,            0xc,  hdrhit_mod_rtc_revb_cmd_msg_t),        /* hdr_ftmi_mod_rtc_revb */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, CONFIG_RTC_REVB,         0xd,  hdrhit_config_rtc_revb_cmd_msg_t),     /* hdr_ftmi_config_rtc_revb */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, HARD_HANDOFF,            0xe,  hdrhit_hard_handoff_cmd_msg_t),        /* hdr_ftmi_hard_handoff */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, GET_FWD_LNK_STATS,       0xf,  hdrhit_get_fwd_lnk_stats_cmd_msg_t),   /* hdr_ftmi_get_fwd_link_stats */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, RESET_FWD_LNK_STATS,     0x10, hdrhit_reset_fwd_lnk_stats_cmd_msg_t), /* hdr_ftmi_reset_fwd_link_stats */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, MODULATOR_CTL,           0x11, hdrhit_modulator_ctl_cmd_msg_t),       /* hdr_ftmi_modulator_ctl */
  MSGR_DEFINE_UMID(HDR, HIT, CMD, CONFIG_FUNNEL_MODE,      0x12, hdrhit_config_funnel_mode_cmd_msg_t),  /* hdr_ftmi_config_funnel_mode */
  HDR_HIT_MSG_CMD_LAST = HDR_HIT_CMD_CONFIG_FUNNEL_MODE
};


/* Response of HDR HIT  */
enum
{
  HDR_HIT_RSP_FIRST = MSGR_UMID_BASE(MSGR_HDR_HIT, MSGR_TYPE_RSP),
  MSGR_DEFINE_UMID(HDR, HIT, RSP, PIL_ACQ,                 0x0,  hdrhit_pil_acq_rsp_msg_t),             /* hdr_ftmi_pil_acq */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, SYNC,                    0x1,  hdrhit_sync_rsp_msg_t),                /* hdr_ftmi_sync */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, DEMOD_FTC,               0x2,  hdrhit_demod_ftc_rsp_msg_t),           /* hdr_ftmi_demod_ftc */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, MOD_ACC,                 0x3,  hdrhit_mod_acc_rsp_msg_t),             /* hdr_ftmi_mod_acc */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, MOD_RTC,                 0x4,  hdrhit_mod_rtc_rsp_msg_t),             /* hdr_ftmi_mod_rtc */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, DEASSIGN,                0x5,  hdrhit_deassign_rsp_msg_t),            /* hdr_ftmi_deassign */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, IDLE_STATE,              0x6,  hdrhit_idle_state_rsp_msg_t),          /* hdr_ftmi_idle_state */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, DEMOD_FW_LINK_REV0,      0x7,  hdrhit_demod_fw_link_rev0_rsp_msg_t),  /* hdr_ftmi_demod_fw_link_rev0 */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, RELA_MAC_CONFIG,         0x8,  hdrhit_rela_mac_config_rsp_msg_t),     /* hdr_ftmi_rela_mac_config */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, DEMOD_FW_LINK_REVA,      0x9,  hdrhit_demod_fw_link_reva_rsp_msg_t),  /* hdr_ftmi_demod_fw_link_reva */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, MOD_ACC_REVA,            0xa,  hdrhit_mod_acc_reva_rsp_msg_t),        /* hdr_ftmi_mod_acc_reva */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, MOD_RTC_REVA,            0xb,  hdrhit_mod_rtc_reva_rsp_msg_t),        /* hdr_ftmi_mod_rtc_reva */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, MOD_RTC_REVB,            0xc,  hdrhit_mod_rtc_revb_rsp_msg_t),        /* hdr_ftmi_mod_rtc_revb */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, CONFIG_RTC_REVB,         0xd,  hdrhit_config_rtc_revb_rsp_msg_t),     /* hdr_ftmi_config_rtc_revb */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, HARD_HANDOFF,            0xe,  hdrhit_hard_handoff_rsp_msg_t),        /* hdr_ftmi_hard_handoff */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, GET_FWD_LNK_STATS,       0xf,  hdrhit_get_fwd_lnk_stats_rsp_msg_t),   /* hdr_ftmi_get_fwd_link_stats*/
  MSGR_DEFINE_UMID(HDR, HIT, RSP, RESET_FWD_LNK_STATS,     0x10, hdrhit_reset_fwd_lnk_stats_rsp_msg_t), /* hdr_ftmi_reset_fwd_link_stats*/
  MSGR_DEFINE_UMID(HDR, HIT, RSP, MODULATOR_CTL,           0x11, hdrhit_modulator_ctl_rsp_msg_t),       /* hdr_ftmi_modulator_ctl */
  MSGR_DEFINE_UMID(HDR, HIT, RSP, CONFIG_FUNNEL_MODE,      0x12, hdrhit_config_funnel_mode_rsp_msg_t),   /* hdr_ftmi_config_funnel_mode */
  HDR_HIT_MSG_RSP_LAST = HDR_HIT_RSP_CONFIG_FUNNEL_MODE
};

/* This struct contains fields to carry out execution of hdr_ftmi_pil_acq() */
typedef struct {
    msgr_hdr_struct_type               hdr;
	sys_band_class_e_type			   band_class;
	uint16							   chan_num;			/* cdma frequency */
    hdrhit_hdr_ftm_diversity_mode_type diversity_mode;
} hdrhit_pil_acq_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_sync) */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
} hdrhit_sync_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_demod_ftc() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
	sys_channel_type				hdr_chan;
    uint8                           arq_enable;
    uint8                           drc_lockperiod;
    uint8                           drc_locklength;
    uint8                           mac_index;
    uint8                           rab_length;
    uint8                           rab_offset;
} hdrhit_demod_ftc_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_mod_acc() */
typedef struct {
    msgr_hdr_struct_type            hdr;                         /* Message header*/
    int8                            open_loop_adjust;
    int8                            probe_initial_adjust;
    uint8                           access_cycle_duration;
    uint8                           power_step;
    uint8                           probe_num_step;
    uint8                           preamble_len;
    uint32                          aclc_mask_i_hi;
    uint32                          aclc_mask_i_lo;
    uint32                          aclc_mask_q_hi;
    uint32                          aclc_mask_q_lo;
    uint32                          ac_txpattern;
} hdrhit_mod_acc_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_mod_rtc() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
    sys_channel_type                hdr_chan;
    uint8                           drc_value;
    uint8                           drc_length;
    boolean                         drc_gating;
    uint8                           drc_cover;
    int8                            drc_gain;
    int8                            ack_gain;
    uint8                           frame_offset;
    uint8                           hdr_ack_mode;
    uint8                           dataoffset_nom;
    uint8                           dataoffset_9k6;
    uint8                           dataoffset_19k2;
    uint8                           dataoffset_38k4;
    uint8                           dataoffset_76k8;
    uint8                           dataoffset_153k6;
    uint8                           rtc_txduty_cycle;
    uint8                           rri;
    uint32                          rtc_txpattern;
    uint8                           rpc_enable;
    uint8                           rpc_step;
    uint32                          rtclc_mask_i_hi;
    uint32                          rtclc_mask_i_lo;
    uint32                          rtclc_mask_q_hi;
    uint32                          rtclc_mask_q_lo;
} hdrhit_mod_rtc_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_deassign() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
} hdrhit_deassign_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_idle_state() */
typedef struct {
     msgr_hdr_struct_type            hdr;                       /* Message header*/
} hdrhit_idle_state_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_demod_fw_link_rev0() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
	sys_channel_type				hdr_chan;
    uint8                           arq_enable;
    uint8                           drc_lockperiod;
    uint8                           drc_locklength;
    uint8                           drc_cover;
    uint8                           drc_value;
    uint8                           mac_index;
    uint8                           rab_length;
    uint8                           rab_offset;
} hdrhit_demod_fw_link_rev0_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_rela_mac_config() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
    uint8                           mup_enabled;
    uint8                           sht_pkt_enabled;
} hdrhit_rela_mac_config_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_demod_fw_link_reva() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
	sys_channel_type				hdr_chan;
    uint8                           arq_enable;
    uint8                           drc_lockperiod;
    uint8                           drc_locklength;
    uint8                           drc_cover;
    uint8                           drc_value;
    uint8                           dsc_value;
    uint8                           mac_index;
    uint8                           rab_length;
    uint8                           rab_offset;    
} hdrhit_demod_fw_link_reva_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_mod_acc_reva() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
    int8                            open_loop_adjust;
    int8                            probe_initial_adjust;
    uint8                           probe_num_step;
    uint8                           access_cycle_duration;
    uint8                           power_step;    
    uint8                           preamble_len;
    uint32                          aclc_mask_i_hi;
    uint32                          aclc_mask_i_lo;
    uint32                          aclc_mask_q_hi;
    uint32                          aclc_mask_q_lo; 
    uint8                           enhanced_access_enable;
    uint8                           preamble_length_slots;    
    uint8                           sector_access_rate_max;
    uint8                           terminal_access_rate_max;
    uint8                           access_offset;
    uint32                          ac_txpattern;
} hdrhit_mod_acc_reva_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_mod_rtc_reva() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
    sys_channel_type                hdr_chan;
    uint8                           drc_value;
    int8                            drc_gain;
    uint8                           drc_length;
    boolean                         drc_gating;
    uint8                           drc_cover;
    uint8                           frame_offset;
    uint8                           hdr_ack_mode;
    int8                            ack_gain;    
    uint8                           dsc_gain;
    uint8                           dsc_value;
    uint8                           dsc_length;
    uint8                           hicap_lolatency;
    uint8                           gain_transition_point;
    uint8                           t2p_pre_transition;
    uint8                           t2p_post_transition;
    int8                            rri_gain_pre_transition;
    int8                            rri_gain_post_transition;
    uint8                           arq_mode;
    uint8                           auxpil_min_payload;
    uint8                           auxpil_gain;
    uint32                          rtc_txpattern;
    uint8                           rri;    
    uint8                           rpc_enable;
    uint8                           rpc_step;
    uint32                          rtclc_mask_i_hi;
    uint32                          rtclc_mask_i_lo;
    uint32                          rtclc_mask_q_hi;
    uint32                          rtclc_mask_q_lo;
} hdrhit_mod_rtc_reva_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_mod_rtc_revb() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
	sys_channel_type				hdr_chan;
    uint8                           drc_value;
    int8                            drc_gain;
    int8                            ack_gain;
    uint8                           dsc_gain;
    uint8                           auxpil_gain;
    uint8                           num_carriers;
    uint8                           rpc_step;
    uint8                           rpc_enable;
    uint8                           single_fwd_link_mode;
    uint8                           hicap_lolatency;
    uint16                          chan_c1;
    uint16                          chan_c2;
    uint8                           rri_c0;
    uint8                           rri_c1;
    uint8                           rri_c2;
    uint8                           power_offset_c0;
    uint8                           power_offset_c1;
    uint8                           power_offset_c2;
} hdrhit_mod_rtc_revb_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_config_rtc_revb() */
typedef struct {
    msgr_hdr_struct_type            hdr;                        /* Message header*/
    uint8                           drc_cover; 
    uint8                           drc_length;  
    uint8                           drc_gating; 
    uint8                           dsc_value;
    uint8                           dsc_length;  
    uint8                           frame_offset;
    uint32                          rtc_txpattern;
    uint32                          rtclc_mask_i_hi;
    uint32                          rtclc_mask_i_lo;
    uint32                          rtclc_mask_q_hi;
    uint32                          rtclc_mask_q_lo;
    uint8                           arq_mode;
    uint8                           hdr_ack_mode; 
    uint8                           auxpil_min_payload; 
    uint8                           rri;
    uint8                           hicap_lolatency;
    uint8                           gain_transition_point;
    uint8                           t2p_pre_transition;
    uint8                           t2p_post_transition;
    int8                            rri_gain_pre_transition;
    int8                            rri_gain_post_transition;
} hdrhit_config_rtc_revb_cmd_msg_t;


/* This struct contains fields to carry out execution of hdr_ftmi_hard_handoff() */
typedef struct {
    msgr_hdr_struct_type                hdr;
    sys_channel_type                    target_hdr_chan;
    boolean                             rpc_enable;
} hdrhit_hard_handoff_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_get_fwd_lnk_stats */
typedef struct {
    msgr_hdr_struct_type                hdr;
} hdrhit_get_fwd_lnk_stats_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_reset_fwd_lnk_stats */
typedef struct {
    msgr_hdr_struct_type                hdr;
} hdrhit_reset_fwd_lnk_stats_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_modulator_ctl */
typedef struct {
    msgr_hdr_struct_type                hdr;
    uint8                               carrier_mask;
    boolean                             skip_dac_toggle;
} hdrhit_modulator_ctl_cmd_msg_t;

/* This struct contains fields to carry out execution of hdr_ftmi_config_funnel_mode */
typedef struct {
    msgr_hdr_struct_type                hdr;
    boolean                             enable;
    uint8                               data_active_carrier_idx;
} hdrhit_config_funnel_mode_cmd_msg_t;

/* ENUM to send back status in response messages */
typedef enum
{
	HDRHIT_SUCCESS = 0,
	HDRHIT_INVALID_STATE = 2,
	HDRHIT_NO_SUCCESS = 3
} hdrhit_msg_status_type;


/* This struct is command fields of all responses */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_common_rsp_msg_t;

/* This struct is used to send response PIL_ACQ */
typedef struct{
    msgr_hdr_struct_type               	hdr;                     /* Message header*/
    hdrhit_msg_status_type              cmd_status;
    sys_channel_type                    hdr_chan;
} hdrhit_pil_acq_rsp_msg_t;

/* This struct is used to send response SYNC */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_sync_rsp_msg_t;

/* This struct is used to send response DEMOD_FTC */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_demod_ftc_rsp_msg_t;

/* This struct is used to send response MOD_ACC */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_mod_acc_rsp_msg_t;

/* This struct is used to send response MOD_RTC */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_mod_rtc_rsp_msg_t;

/* This struct is used to send response DEASSIGN */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_deassign_rsp_msg_t;

/* This struct is used to send response IDLE_STATE */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_idle_state_rsp_msg_t;

/* This struct is used to send response DEMOD_FW_LINK_REV0 */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_demod_fw_link_rev0_rsp_msg_t;

/* This struct is used to send response RELA_MAC_CONFIG */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_rela_mac_config_rsp_msg_t;

/* This struct is used to send response DEMOD_FW_LINK_REVA */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_demod_fw_link_reva_rsp_msg_t;

/* This struct is used to send response MOD_ACC_REVA */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_mod_acc_reva_rsp_msg_t;

/* This struct is used to send response MOD_RTC_REVA */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_mod_rtc_reva_rsp_msg_t;

/* This struct is used to send response MOD_RTC_REVB */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_mod_rtc_revb_rsp_msg_t;

/* This struct is used to send response CONFIG_RTC_REVB */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_config_rtc_revb_rsp_msg_t;

/* This struct is used to send response HARD_HANDOFF*/
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_hard_handoff_rsp_msg_t;

/* This struct is used to send  response GET_FWD_LNK_STATS  */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
    uint32                             total_tc_packets;
    uint32                             total_tc_errors;
} hdrhit_get_fwd_lnk_stats_rsp_msg_t;

/* This struct is used to send  response RESET_FWD_LNK_STATS  */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_reset_fwd_lnk_stats_rsp_msg_t;

/* This struct is used to send  response MODULATOR_CTL  */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_modulator_ctl_rsp_msg_t;

/* This struct is used to send  response CONFIG_FUNNEL_MODE  */
typedef struct{
    msgr_hdr_struct_type               hdr;                     /* Message header*/
    hdrhit_msg_status_type             cmd_status;
} hdrhit_config_funnel_mode_rsp_msg_t;

typedef union {
    msgr_hdr_struct_type                        hdr;
    /* message header */
    hdrhit_pil_acq_cmd_msg_t                    pil_acq;
    /* pilot acquisition command*/
    hdrhit_sync_cmd_msg_t                       sync;

    hdrhit_demod_ftc_cmd_msg_t                  demod_ftc;

    hdrhit_mod_acc_cmd_msg_t                    mod_acc;

    hdrhit_mod_rtc_cmd_msg_t                    mod_rtc;
  
    hdrhit_deassign_cmd_msg_t                   deassign;
  
    hdrhit_idle_state_cmd_msg_t                 idle_state; 

    hdrhit_demod_fw_link_rev0_cmd_msg_t         demod_fw_link_rev0;

    hdrhit_rela_mac_config_cmd_msg_t            rela_mac_config;

    hdrhit_demod_fw_link_reva_cmd_msg_t         demod_fw_link_reva;

    hdrhit_mod_acc_reva_cmd_msg_t               mod_acc_reva;
 
    hdrhit_mod_rtc_reva_cmd_msg_t               mod_rtc_reva;

    hdrhit_mod_rtc_revb_cmd_msg_t               mod_rtc_revb;

    hdrhit_config_rtc_revb_cmd_msg_t            config_rtc_revb;

    hdrhit_hard_handoff_cmd_msg_t               hard_handoff;

    hdrhit_get_fwd_lnk_stats_cmd_msg_t          get_fwd_lnk_stats;

    hdrhit_reset_fwd_lnk_stats_cmd_msg_t        reset_fwd_lnk_stats;

    hdrhit_modulator_ctl_cmd_msg_t              modulator_ctl;

    hdrhit_config_funnel_mode_cmd_msg_t         config_funnel_mode;

} hdrhit_msg_cmd_u;


typedef union {
    msgr_hdr_struct_type                        hdr;
    /* message header */
    hdrhit_common_rsp_msg_t                     common_rsp;

    hdrhit_pil_acq_rsp_msg_t                    pil_acq;

    hdrhit_sync_rsp_msg_t                       sync;

    hdrhit_demod_ftc_rsp_msg_t                  demod_ftc;

    hdrhit_mod_acc_rsp_msg_t                    mod_acc;

    hdrhit_mod_rtc_rsp_msg_t                    mod_rtc;
  
    hdrhit_deassign_rsp_msg_t                   deassign;
  
    hdrhit_idle_state_rsp_msg_t                 idle_state; 

    hdrhit_demod_fw_link_rev0_rsp_msg_t         demod_fw_link_rev0;

    hdrhit_rela_mac_config_rsp_msg_t            rela_mac_config;

    hdrhit_demod_fw_link_reva_rsp_msg_t         demod_fw_link_reva;

    hdrhit_mod_acc_reva_rsp_msg_t               mod_acc_reva;
 
    hdrhit_mod_rtc_reva_rsp_msg_t               mod_rtc_reva;

    hdrhit_mod_rtc_revb_rsp_msg_t               mod_rtc_revb;

    hdrhit_config_rtc_revb_rsp_msg_t            config_rtc_revb;

    hdrhit_hard_handoff_rsp_msg_t               hard_handoff;

    hdrhit_get_fwd_lnk_stats_rsp_msg_t          get_fwd_lnk_stats;

    hdrhit_reset_fwd_lnk_stats_rsp_msg_t        reset_fwd_lnk_stats;

    hdrhit_modulator_ctl_rsp_msg_t              modulator_ctl;

    hdrhit_config_funnel_mode_rsp_msg_t         config_funnel_mode;

} hdrhit_msg_rsp_u;

/*=========================================================================

FUNCTION HDRHITMSG_GET_FWD_LNK_STATS

DESCRIPTION
  This function resets forward link stats.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_get_fwd_lnk_stats
(
   uint32 * total_tc_packets,
   uint32 * total_tc_errors 
);

/*===========================================================================

FUNCTION HDRHITMSG_GET_RESPONSE_STATUS

DESCRIPTION
  Provide response status.

DEPENDENCIES

RETURN VALUE
   Response message status
SIDE EFFECTS
  None.

===========================================================================*/ 
hdrhit_msg_status_type hdrhitmsg_get_response_status( void );

#endif /* HDRHIT_MSG_H */

