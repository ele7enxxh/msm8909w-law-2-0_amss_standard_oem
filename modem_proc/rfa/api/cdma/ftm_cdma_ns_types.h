#ifndef FTM_CDMA_NS_TYPES_H
#define FTM_CDMA_NS_TYPES_H

/*!
  @file
  Common CDMA (1x and HDR) Factory Test Mode non-signaling test interface
  types.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/cdma/ftm_cdma_ns_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/22/12   APU     Added FTM support for setting the individual pilot carrier 
                   power.
01/28/11   bmg     Consolidated CDMA non-signaling type definitions.
==============================================================================*/

#include "comdef.h"   /* Needed for feature definitions */
#include "cmd.h"      /* Command Queue types */
#include "test_cmd.h" /* Just for ftm_cmd_status_type */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum 
{
  FTM_CMD_INIT_CDMA,
  FTM_CMD_PILOT_ACQ,      
  FTM_CMD_DEMOD_SYNC, 
  FTM_CMD_MOD_FCH, 
  FTM_CMD_MOD_DCCH,
  FTM_CMD_MOD_SCH,
  FTM_CMD_DEMOD_FCH,
  FTM_CMD_DEMOD_FDCCH,
  FTM_CMD_DEMOD_FSCH, 
  FTM_CMD_FCH_LOOPBACK, 
  FTM_CMD_DCCH_LOOPBACK,
  FTM_CMD_SCH_LOOPBACK,
  FTM_CMD_RELEASE,
  FTM_CMD_FWD_HHO,
  FTM_CMD_SET_REV_LNK_PWR,

  HDR_FTM_CMD_PILOT_ACQ,
  HDR_FTM_CMD_SYNC,
  HDR_FTM_CMD_DEMOD_FTC,
  HDR_FTM_CMD_MOD_ACC,
  HDR_FTM_CMD_MOD_RTC,
  HDR_FTM_CMD_DEASSIGN,
  HDR_FTM_CMD_IDLE_STATE,
  HDR_FTM_CMD_DEMOD_FW_LINK_REV0,

  HDR_FTM_CMD_RELA_MAC_CONFIG,
  HDR_FTM_CMD_DEMOD_FW_LINK_REVA,
  HDR_FTM_CMD_MOD_ACC_REVA,
  HDR_FTM_CMD_MOD_RTC_REVA,

  FTM_CMD_MOD_FCH_DCCH, 
  FTM_CMD_DEMOD_FFCH_FDCCH, 

	HDR_FTM_CMD_MOD_RTC_REVB,
	HDR_FTM_CMD_CONFIG_RTC_REVB,

  FTM_CMD_NUM_CMDS    /* must be the last member */
}
ftm_cmd_enum_type;


/*******************************************************************************
                                    1X TYPES
*******************************************************************************/

/*--------------------------------------------------------------------------
                        ACQUISITION MODE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  FTM_ACQ_MODE_FULL,
  FTM_ACQ_MODE_MINI,
  FTM_ACQ_MODE_MICRO,
  ftm_acq_mode_sizer = 0x7FFFFFFF
} ftm_acq_mode_type;

/*--------------------------------------------------------------------------
                        BAND CLASS ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  FTM_BAND_CLASS_CELLULAR,
  FTM_BAND_CLASS_PCS,
  ftm_band_class_sizer = 0x7FFFFFFF
} ftm_band_class_type;

/*--------------------------------------------------------------------------
                        RADIO CONFIGURATION ENUM TYPES
--------------------------------------------------------------------------*/
typedef enum 
{
  FTM_FWD_RC_1=0x01,
  FTM_FWD_RC_2,
  FTM_FWD_RC_3,
  FTM_FWD_RC_4,
  FTM_FWD_RC_5,
  FTM_FWD_RC_1X_MAX,
  ftm_fwd_rc_sizer = 0x7FFFFFFF
} ftm_fwd_rc_type;

typedef enum 
{
  FTM_REV_RC_1=0x01,
  FTM_REV_RC_2,
  FTM_REV_RC_3,
  FTM_REV_RC_4,
  FTM_REV_RC_1X_MAX,
  ftm_rev_rc_sizer = 0x7FFFFFFF
} ftm_rev_rc_type;

typedef enum 
{
  FTM_REV_FCH_FULL_RATE=0x00,
  FTM_REV_FCH_HALF_RATE=0x01,
  FTM_REV_FCH_QTR_RATE=0x02,
  FTM_REV_FCH_8TH_RATE=0x03,
  ftm_rev_fch_rate_sizer = 0x7FFFFFFF
} ftm_rev_fch_rate_type;

typedef enum 
{
  FTM_REV_SCH_1_5_RATE=0x00,
  FTM_REV_SCH_2_7_RATE=0x01,
  FTM_REV_SCH_4_8_RATE=0x02,
  FTM_REV_SCH_9_6_RATE=0x03,
  FTM_REV_SCH_19_2_RATE=0x04,
  FTM_REV_SCH_38_4_RATE=0x05,
  FTM_REV_SCH_76_8_RATE=0x06,
  FTM_REV_SCH_153_6_RATE=0x07,
  ftm_rev_sch_rate_sizer = 0x7FFFFFFF
} ftm_rev_sch_rate_type;

/*--------------------------------------------------------------------------
                        CHANNEL SPREAD RATE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  FTM_RATE_1X,
  FTM_RATE_2X,
  FTM_RATE_4X,
  FTM_RATE_8X,
  FTM_RATE_16X,
  ftm_spread_rate_sizer = 0x7FFFFFFF
} ftm_spread_rate_type;

/*--------------------------------------------------------------------------
                        CODING TYPE ENUM TYPE
--------------------------------------------------------------------------*/

typedef enum
{
  FTM_DECODE_CONVOLUTIONAL,
  FTM_DECODE_TURBO,
  ftm_decoding_sizer = 0x7FFFFFFF
} ftm_coding_type;

/*--------------------------------------------------------------------------
                        WALSH CODE/QOF  SPECIFIER TYPE
--------------------------------------------------------------------------*/
typedef PACK(struct) 
{
  byte walsh;    /* walsh channel number (0-63 for 95A/B, 0-127 for cdma2000) */

  byte qof_mask_id;  /* quasi orthoganal function mask (0-3) where 0 is 
                        true walsh codes */
} ftm_walsh_type;

/*--------------------------------------------------------------------------
             Structure for the FTM Fast Forward Power Control Parameters
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  byte  target_fer;
  byte  min_setpt;
  byte  max_setpt;
} ftm_fpc_olc_param_type;


typedef PACK(struct)
{
  ftm_band_class_type band_class;
  /* the band class, Cellular or PCS */

  word cdma_freq;
  /* the cdma frequency channel */

  ftm_acq_mode_type acq_mode;
  /* the acquisition mode, Full, Mini, Micro */
} ftm_cmd_acq_pilot_param_type;

typedef PACK(struct)
{
  ftm_fwd_rc_type radio_cfg;
  /* the radio configuration of this FCH */

  ftm_walsh_type walsh;
  /* walsh index */

  boolean bypass_lc;
  /* bypass long code (long code set to all zero) */

  byte frame_offset;
  /* frame offset */

  byte subchan_gain;
  /* Power control subchannel gain */

  boolean ffpc_enable;
  /* fast forward power control enable */

  dword fpc_mode;
  /* forward power control mode */

  byte init_setpt;
  /* initial set point for inner loop FPC on FCH */

  ftm_fpc_olc_param_type  olc_params;
  /* outer loop power control parameters */

} ftm_cmd_demod_fch_param_type;

typedef PACK(struct)
{

  ftm_fwd_rc_type radio_cfg;
  /* the radio configuration of this FCH */

  ftm_walsh_type walsh;
  /* walsh index */

  boolean bypass_lc;
  /* bypass long code (long code set to all zero) */

  word so_pri;
  /* Primary service option */

  word so_sec;
  /* Secondary service option */

  byte frame_offset;
  /* frame offset */

  boolean ilpc_enabled;
  /* to indicate if inner loop power control is enabled or not */

  boolean olpc_enabled;
  /* to indicate if outer loop power control is enabled or not */

  byte init_setpt;
  /* initial set point for inner loop FPC on DCCH */

  byte fpc_subchan_gain;
  /* subchan gain value for inner loop FPC */

  ftm_fpc_olc_param_type  dcch_olc_params;
  /* outer loop power control parameters */

} ftm_cmd_demod_dcch_param_type;

typedef PACK(struct)
{
  byte rx_chain;
}ftm_cmd_rx_level_param_type;

typedef PACK(struct)
{
  ftm_cmd_demod_fch_param_type fch;
     /* FCH parameters */

  ftm_cmd_demod_dcch_param_type dcch;
     /* DCCH parameters */

} ftm_cmd_demod_fch_dcch_param_type;

typedef PACK(struct)
{
  ftm_fwd_rc_type radio_cfg;
  /* the radio configuration of this SCH */

  ftm_walsh_type walsh;
  /* walsh index */

  ftm_spread_rate_type sch_rate;
  /* from 1X - 16X */

  ftm_coding_type coding_type;
  /* Convolutional or Turbo */

  boolean ffpc_enable;
  /* fast forward power control enable */

  dword fpc_mode;
  /* forward power control mode */

  byte init_setpt;
  /* initial set point for inner loop FPC on FCH */

  ftm_fpc_olc_param_type  olc_params;
  /* outer loop power control parameters */

} ftm_cmd_demod_sch_param_type;

typedef PACK(struct)
{
  ftm_rev_rc_type radio_cfg;
  /* Reverse FCH radio config */

  byte frame_offset;
  /* frame offset */

  ftm_rev_fch_rate_type fch_rate;

  boolean enable_rpc;
  /* Enable/disable reverse power control */

  word num_preambles;
  /* Preamble count for traffic channel initialization */

  dword tx_pattern;
  /* 32 bit pattern of bytes to use in the traffic frames */
} ftm_cmd_mod_fch_param_type;

typedef PACK(struct)
{
  ftm_rev_rc_type dcch_rc;

  byte frame_offset;
  /* frame offset */

  boolean sch_on;

  ftm_rev_rc_type sch_rc;

  boolean sch_turbo;

  word    sch_ltu_size;

  ftm_rev_sch_rate_type sch_rate;

  word num_preambles;

  dword tx_pattern;
  /* 32 bit pattern of bytes to use in the traffic frames */

}  ftm_cmd_mod_dcch_param_type;

typedef PACK(struct)
{
  ftm_rev_rc_type radio_cfg;
  /* Reverse FCH radio config */

  ftm_rev_sch_rate_type sch_rate;
  /* Rate of reverse SCH from */

  ftm_coding_type coding_type;
  /* turbo or convolutional */

  dword tx_pattern;
  /* 32 bit pattern of bytes to use in the traffic frames */
} ftm_cmd_mod_sch_param_type;

typedef PACK(struct)
{
  ftm_cmd_mod_fch_param_type  fch;    /* reverse FCH */
  ftm_cmd_mod_dcch_param_type dcch;    /* reverse DCCH */

} ftm_cmd_mod_fch_dcch_param_type;

typedef PACK(struct)
{
    byte override_enable;
    int2 tx_power_10th_dbm;
} ftm_cmd_rev_link_pwr_setting_param_type; 

typedef PACK(struct)
{
    uint8 rfm_device ;
    /* A valid Tx device, either SV or NSV device */

    uint8 carrier_index ;
    /* HDR carrier inder whose pilot needs to be changed
       Can be 0 or 1 or 2 or 3 */

    uint8 override_enable ;
    /* override: Yes Openloop in effect. No: Clear Override */

    int16 tx_power_10th_dbm ;
    /* HDR carrier power in dBm10 */

} ftm_cdma_carrier_tx_pwr_cmd_param_type ; 

typedef PACK(struct)
{
  ftm_band_class_type band_class;
  /* the band class, Cellular or PCS */

  word cdma_freq;
  /* the cdma frequency channel */

  boolean enable_rpc;

} ftm_cmd_fwd_hho_param_type;

/*--------------------------------------------------------------------------
                        COMMAND HEADER STRUCT TYPE
--------------------------------------------------------------------------*/

typedef struct
{
  cmd_hdr_type         cmd_hdr;
    /* A header for queue control */
  ftm_cmd_enum_type    cmd;            
    /* Which command to run */
  ftm_cmd_status_type  status; 
    /* The return status of the command */
}
ftm_cmd_hdr_type;


/*--------------------------------------------------------------------------
                          FTM COMMAND STRUCT TYPES
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                          PERFORM CDMA INITIALIZATION

This command instructs the FTM to initialize CDMA hardware and go to a
stable state for further commands.  
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
ftm_init_cdma_cmd_type;


/*--------------------------------------------------------------------------
                          PERFORM PILOT ACQUISITION

This command instructs the FTM to perform pilot acquisition.  FTM will
continue to attempt to acquire a pilot until it does so successfully or
until new behavior is commanded.  There are no timeouts.
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_acq_pilot_param_type param;
}
ftm_pilot_acq_cmd_type;


/*--------------------------------------------------------------------------
                         DEMODULATE SYNC CHANNEL
                         
This command instructs FTM to demodulate the sync channel                         
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
ftm_demod_sync_cmd_type;

/*--------------------------------------------------------------------------
                         DEMODULATE TRAFFIC CHANNEL
                         
This command instructs FTM to demodulate a 95A/B traffic channel
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_demod_fch_param_type param;
}
ftm_demod_fch_cmd_type;

/* DEMODULATE DEDICATED CONTROL CHANNEL */
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_demod_dcch_param_type param;
} ftm_demod_dcch_cmd_type;

/*
** Reverse link commands
*/

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_fch_param_type param;
}
ftm_mod_fch_cmd_type;

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_dcch_param_type param;
} ftm_mod_dcch_cmd_type;

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_sch_param_type param;
}
ftm_mod_sch_cmd_type;
/*--------------------------------------------------------------------------
                         DEMODULATE SUPPLEMENTAL CHANNEL (IS95C SCH)

This command instructs FTM to demodulate a 95C Supplemental Channel
--------------------------------------------------------------------------*/
typedef struct
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_demod_sch_param_type param;
}
ftm_demod_sch_cmd_type;

/*--------------------------------------------------------------------------
                         DEMODULATE SIMULTANEOUS FCH & DCCH 

This command instructs FTM to demodulate FCH & DCCH channels (Release A)
--------------------------------------------------------------------------*/
typedef struct
{
   ftm_cmd_hdr_type  hdr;
   /* Common header */

   ftm_cmd_demod_fch_dcch_param_type param;
}
ftm_demod_fch_dcch_cmd_type;

/*--------------------------------------------------------------------------
                         MODULATE SIMULTANEOUS FCH & DCCH 

This command instructs FTM to modulate FCH & DCCH channels (Release A)
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_mod_fch_dcch_param_type param;
}
ftm_mod_fch_dcch_cmd_type;

/*--------------------------------------------------------------------------
                          PERFORM HARD HANDOFF

This command instructs the FTM to perform Hard handoff.  FTM will
continue to attempt to complete hard handoff until it does so successfully or
until new behavior is commanded.  There are no timeouts.
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_fwd_hho_param_type param;
}
ftm_fwd_hho_cmd_type;

/*--------------------------------------------------------------------------
                          PERFORM REVERSE POWER CONTROL

This command instructs the FTM to set the Tx power to a secific dBm level. 
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  ftm_cmd_rev_link_pwr_setting_param_type param;
}
ftm_cmd_rev_link_pwr_setting_cmd_type;


/*--------------------------------------------------------------------------
                          PERFORM REVERSE POWER CONTROL

This command instructs the FTM to set the Tx power for a given carrier 
to a specific dBm level. 
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr ;
  /* Common header */

   ftm_cdma_carrier_tx_pwr_cmd_param_type param ;
}  ftm_cmd_hdr_tx_pwr_setting_cmd_type ;

/*******************************************************************************
                                   HDR TYPES
*******************************************************************************/
/*--------------------------------------------------------------------------
                        ACQUISITION MODE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  HDR_FTM_ACQ_MODE_FULL,
  HDR_FTM_ACQ_MODE_MINI,
  HDR_FTM_ACQ_MODE_MICRO,
  hdr_ftm_acq_mode_sizer = 0x7FFFFFFF
} hdr_ftm_acq_mode_type;

/*--------------------------------------------------------------------------
                        BAND CLASS ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum 
{
  HDR_FTM_BAND_CLASS_CELLULAR,
  HDR_FTM_BAND_CLASS_PCS,
  hdr_ftm_band_class_sizer = 0x7FFFFFFF
} hdr_ftm_band_class_type;

typedef enum 
{
  HDR_FTM_DIVERSITY_DISABLE,
  HDR_FTM_DIVERSITY_ENABLE,
  hdr_ftm_diversity_size = 0x7FFFFFFF
} hdr_ftm_diversity_mode_type;

typedef PACK(struct)
{
    dword    band_class;
    word     cdma_freq;
    boolean  rpc_enable;
} hdr_ftm_cmd_hho_type;

typedef PACK(struct)
{
   hdr_ftm_band_class_type band_class;
     /* the band class, Cellular or PCS */

   word cdma_freq;
     /* the cdma frequency channel */

   hdr_ftm_acq_mode_type acq_mode;
     /* the acquisition mode, Full, Mini, Micro */

   hdr_ftm_diversity_mode_type  diversity_mode;
    /* the diversity mode of the acquisition    */

} hdr_ftm_cmd_acq_pilot_param_type;


typedef PACK(struct)
{
   byte     arq_enable;
   byte     drc_lockperiod;
   byte     drc_locklength;
   byte     mac_index;
   byte     rab_length;
   byte     rab_offset;
} hdr_ftm_cmd_demod_ftc_param_type;

typedef PACK(struct)
{
   dword    aclc_mask_i_hi;
   dword    aclc_mask_i_lo;
   dword    aclc_mask_q_hi;
   dword    aclc_mask_q_lo;
   dword    ac_txpattern;
   byte     access_cycle_duration;
   int8     open_loop_adjust;
   int8     probe_initial_adjust;
   byte     probe_num_step;
   byte     power_step;
   byte     preamble_length;
} hdr_ftm_cmd_mod_acc_param_type;

typedef PACK(struct)
{
   byte     ack_gain;
   byte     dataoffset_nom;
   byte     dataoffset_9k6;
   byte     dataoffset_19k2;
   byte     dataoffset_38k4;
   byte     dataoffset_76k8;
   byte     dataoffset_153k6;
   byte     drc_value;
   byte     drc_cover;
   byte     drc_gain;
   byte     drc_length;
   byte     drc_gating;
   byte     frame_offset;
   byte     rpc_enable;
   byte     rpc_step;
   byte     rri;
   dword    rtclc_mask_i_hi;
   dword    rtclc_mask_i_lo;
   dword    rtclc_mask_q_hi;
   dword    rtclc_mask_q_lo;
   dword    rtc_txpattern;
   byte     rtc_txduty_cycle;
   byte     hdr_ack_mode;  // hdrfmac_ack_mode_enum_type
} hdr_ftm_cmd_mod_rtc_param_type;

typedef PACK(struct)
{
   byte     drc_value;
   byte     drc_cover;
   byte     arq_enable;
   byte     drc_lockperiod;
   byte     drc_locklength;
   byte     mac_index;
   byte     rab_length;
   byte     rab_offset;
} hdr_ftm_cmd_fwd_link_rel0_param_type;

typedef PACK(struct)
{
   byte sht_pkt;    /* Short packet */
   byte mup;        /* Multi-user packet */
} hdr_ftm_rela_mac_config_param_type;

typedef PACK(struct)
{
   byte     drc_value;
   byte     drc_cover;
   byte     dsc_value;
   byte     arq_enable;
   byte     drc_lockperiod;
   byte     drc_locklength;
   byte     mac_index;
   byte     rab_length;
   byte     rab_offset;
} hdr_ftm_cmd_fwd_link_rela_param_type;

typedef PACK(struct)
{
   dword    aclc_mask_i_hi;
   dword    aclc_mask_i_lo;
   dword    aclc_mask_q_hi;
   dword    aclc_mask_q_lo;
   dword    ac_txpattern;
   byte     access_cycle_duration;
   int8     open_loop_adjust;
   int8     probe_initial_adjust;
   byte     probe_num_step;
   byte     power_step;
   byte     preamble_length;
   byte     enhanced_access_enable;
   byte     preamble_length_slots;
   byte     access_offset;
   byte     sector_access_rate_max;
   byte     terminal_access_rate_max;
} hdr_ftm_cmd_mod_acc_rela_param_type;

typedef PACK(struct) 
{   
  byte  drc_value; //DRC value
  byte  drc_cover; 
  byte  drc_length;  // 1, 2, 4 or 8 slots
  byte  drc_gating; 
  int8  drc_gain; // relative gain of DRC to Pilot in 0.5 DB              
  int8  ack_gain; // relative gains of ACK to Pilot in 0.5 DB  
  byte  dsc_value;
  byte  dsc_length;  //should be a multiple of 8
  byte  dsc_gain; //dsc channel gain w.r.t the pilot signed with 0.5 dB resolution
  byte  rpc_step;     // reverse power control step in 0.5 DB 
  byte  frame_offset;
  byte  rpc_enable;
  dword rtc_txpattern;
  dword  rtclc_mask_i_hi;
  dword  rtclc_mask_i_lo;
  dword  rtclc_mask_q_hi;
  dword  rtclc_mask_q_lo;
  byte  rri;
  byte  hicap_lolatency;
  byte  gain_transition_point;
  byte  t2p_pre_transition;
  byte  t2p_post_transition;
  byte  arq_mode;
  byte  auxpil_min_payload;
  byte  auxpil_gain;
  int8  rri_gain_pre_transition;
  int8  rri_gain_post_transition;  
  byte  hdr_ack_mode;  // hdrfmac_ack_mode_enum_type
} hdr_ftm_cmd_mod_rtc_rela_param_type;
 
typedef PACK(struct) 
{   
  byte  single_fwd_link_mode;
  byte  num_carriers;
  byte  drc_value; //DRC value
  int8  drc_gain; // relative gain of DRC to Pilot in 0.5 DB              
  int8  ack_gain; // relative gains of ACK to Pilot in 0.5 DB  
  byte  dsc_gain; //dsc channel gain w.r.t the pilot signed with 0.5 dB resolution
  byte  rpc_step;     // reverse power control step in 0.5 DB 
  byte  rpc_enable;
  byte  hicap_lolatency;
  byte  auxpil_gain;
  byte  rri_c0;
  int8  power_offset_c0;
  word  chan_c1;
  byte  rri_c1;
  int8  power_offset_c1;
  word  chan_c2;
  byte  rri_c2;
  int8  power_offset_c2;
} hdr_ftm_cmd_mod_rtc_relb_param_type;

typedef PACK(struct) 
{   
  byte  drc_cover; 
  byte  drc_length;  // 1, 2, 4 or 8 slots
  byte  drc_gating; 
  byte  dsc_value;
  byte  dsc_length;  //should be a multiple of 8
  byte  frame_offset;
  dword rtc_txpattern;
  dword  rtclc_mask_i_hi;
  dword  rtclc_mask_i_lo;
  dword  rtclc_mask_q_hi;
  dword  rtclc_mask_q_lo;
  byte  arq_mode;
  byte  hdr_ack_mode;  // hdrfmac_ack_mode_enum_type
  byte  auxpil_min_payload;
  byte  rri;
  byte  hicap_lolatency;
  byte  gain_transition_point;
  byte  t2p_pre_transition;
  byte  t2p_post_transition;
  int8  rri_gain_pre_transition;
  int8  rri_gain_post_transition;  
  
} hdr_ftm_cmd_config_rtc_relb_param_type;


/*--------------------------------------------------------------------------
                          PERFORM PILOT ACQUISITION

This command instructs the FTM to perform pilot acquisition.  FTM will
continue to attempt to acquire a pilot until it does so successfully or
until new behavior is commanded.  There are no timeouts.
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  hdr_ftm_cmd_acq_pilot_param_type param;
}
hdr_ftm_pilot_acq_cmd_type;

/*--------------------------------------------------------------------------
                          PERFORM SYSTEM SYNCHRONIZATION

--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
hdr_ftm_sync_cmd_type;

/*--------------------------------------------------------------------------
                        DEMOD the forward channels
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  hdr_ftm_cmd_demod_ftc_param_type param;
}
hdr_ftm_demod_ftc_cmd_type;

/*--------------------------------------------------------------------------
                        Modulate the access channel
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  hdr_ftm_cmd_mod_acc_param_type param;
}
hdr_ftm_mod_acc_cmd_type;

/*--------------------------------------------------------------------------
                        Modulate the reverse channels
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  hdr_ftm_cmd_mod_rtc_param_type param;
}
hdr_ftm_mod_rtc_cmd_type;

/*--------------------------------------------------------------------------
                        Deassign the channels
--------------------------------------------------------------------------*/
typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
hdr_ftm_deassign_cmd_type;

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */
}
hdr_ftm_idle_state_cmd_type;

typedef struct 
{
  ftm_cmd_hdr_type  hdr;
  /* Common header */

  hdr_ftm_cmd_fwd_link_rel0_param_type param;
}
hdr_ftm_fwd_link_rel0_cmd_type;

typedef struct
{
  ftm_cmd_hdr_type  hdr;
  hdr_ftm_rela_mac_config_param_type param;
}
hdr_ftm_rela_mac_config_cmd_type;

typedef struct
{
  ftm_cmd_hdr_type  hdr;
  hdr_ftm_cmd_fwd_link_rela_param_type param;
}
hdr_ftm_fwd_link_rela_cmd_type;

typedef struct
{
  ftm_cmd_hdr_type  hdr;
  hdr_ftm_cmd_mod_acc_rela_param_type param;
}
hdr_ftm_mod_acc_rela_cmd_type;

typedef struct
{
  ftm_cmd_hdr_type hdr;
  hdr_ftm_cmd_mod_rtc_rela_param_type param;
}
hdr_ftm_mod_rtc_rela_cmd_type;

typedef struct 
{
  uint32 g0;
  uint32 g1;
  uint32 g2;
  uint32 g3;
  uint32 g4;
  uint32 g5;
  uint32 g6;
  uint32 g7;
  uint32 g8;
  uint32 g9;
  uint32 g10;
  uint32 g11;
} ftm_hdr_gain_array_type;

typedef struct
{
  ftm_cmd_hdr_type hdr;
  hdr_ftm_cmd_mod_rtc_relb_param_type param;
}
hdr_ftm_mod_rtc_relb_cmd_type;

typedef struct
{
  ftm_cmd_hdr_type hdr;
  hdr_ftm_cmd_config_rtc_relb_param_type param;
}
hdr_ftm_config_rtc_relb_cmd_type;



/*--------------------------------------------------------------------------
                             UNION OF ALL FTM COMMANDS
--------------------------------------------------------------------------*/
typedef union
{
  ftm_cmd_hdr_type      hdr;
  ftm_init_cdma_cmd_type init_cdma_cmd;
  ftm_pilot_acq_cmd_type pilot_acq;
  ftm_demod_sync_cmd_type demod_sync;
  ftm_demod_fch_cmd_type demod_fch;
  ftm_demod_dcch_cmd_type demod_dcch;
  ftm_demod_sch_cmd_type demod_sch;
  ftm_mod_fch_cmd_type mod_fch;
  ftm_mod_dcch_cmd_type mod_dcch;
  ftm_mod_sch_cmd_type mod_sch;
  ftm_demod_fch_dcch_cmd_type demod_fch_dcch;
  ftm_mod_fch_dcch_cmd_type mod_fch_dcch;
  ftm_fwd_hho_cmd_type fwd_hho;
  ftm_cmd_rev_link_pwr_setting_cmd_type rev_link_pwr_setting;
  ftm_cmd_hdr_tx_pwr_setting_cmd_type tx_carrier_pwr_setting;


  hdr_ftm_pilot_acq_cmd_type hdr_pilot_acq;
  hdr_ftm_sync_cmd_type hdr_sync;
  hdr_ftm_demod_ftc_cmd_type  hdr_demod_ftc;
  hdr_ftm_mod_acc_cmd_type  hdr_mod_acc;
  hdr_ftm_mod_rtc_cmd_type  hdr_mod_rtc;
  hdr_ftm_deassign_cmd_type  hdr_deassign;
  hdr_ftm_idle_state_cmd_type    hdr_idle_state;
  hdr_ftm_fwd_link_rel0_cmd_type  hdr_fwd_link_rel0;
  hdr_ftm_rela_mac_config_cmd_type  hdr_rela_mac_config;
  hdr_ftm_fwd_link_rela_cmd_type  hdr_fwd_link_rela;
  hdr_ftm_mod_acc_rela_cmd_type  hdr_mod_acc_rela;
  hdr_ftm_mod_rtc_rela_cmd_type  hdr_mod_rtc_rela;
  hdr_ftm_mod_rtc_relb_cmd_type  hdr_mod_rtc_relb;
  hdr_ftm_config_rtc_relb_cmd_type  hdr_config_rtc_relb;
}
ftm_cmd_type;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FTM_CDMA_NS_TYPES_H */

