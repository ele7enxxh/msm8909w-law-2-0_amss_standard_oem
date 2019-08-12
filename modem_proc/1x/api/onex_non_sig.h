#ifndef ONEX_NONSIG_H
#define ONEX_NONSIG_H
/*===========================================================================

         1 X   N O N S I G N A L I N G   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations to be used by the test_mc during 1x
  non-signaling factory test mode.

  Copyright (c) 2011 - 2014 Qualcomm Technologies, Inc.
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
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/onex_non_sig.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/14   dkb     Feature guard Triton UTX code under FEATURE_MODEM_1X_UTX
03/05/13   srk     Remove unused function.
08/14/12   cjb     Added DIAG interface for SLPC verification in non-sig mode.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/23/11   adw     Fixes in onex_nonsig to FTM plumbing.
02/04/11   jtm     Removed unnecessary includes. Pulled some function declarations
                   within the C file. Added new API declarations. Added new
                   types and definitions.
01/31/11   jtm     Create onex_nonsig_mode_type and replace ftm_mode_type use.
01/28/11   jtm     Moved types and variables local to onex_non_sig.c. Added
                   move functions to aid in decoupling from FTM further.
01/21/11   jtm     Initial Revision.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
/* Common */
#include "comdef.h"

/* SRCH */
#include "srch_rx_t.h"

/* Other */
#include "db.h"
#include "cmd.h"

/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
                            SIGNAL MASKS
--------------------------------------------------------------------------*/

/* This signal is set when an item is placed on any FTM_MCC queue */
#define  ONEX_NONSIG_MC_CMD_Q_SIG         MC_CMD_Q_SIG

/* This signal is set when FTM reports to DIAG */
#define  ONEX_NONSIG_MC_CMD_RSP_SIG       MC_CMD_SIG

/* This signal is set to indicate that the searcher has
   completed an rx tune */
#define  ONEX_NONSIG_MC_CMD_SRCH_COMPLETE MC_CMD_SIG << 2

/* This signal is set to indicate that the onex nonsig timer has expired */
#define  ONEX_NONSIG_TMO_SIG              MC_CMD_SIG << 3

/* This is the FTM command mapping used internal to onex non sig.
   Note that the values *must* match ftm_subcmd_type. */
typedef enum {
  ONEX_FTM_FIRST_CMD             = 100,
  ONEX_FTM_ACQUIRE_SC            = 100,
  ONEX_FTM_SYNC_SC               = 101,
  ONEX_FTM_FWD_FCH_SC            = 102,
  ONEX_FTM_FWD_SCH_SC            = 103,
  ONEX_FTM_REV_FCH_SC            = 104,
  ONEX_FTM_REV_SCH_SC            = 105,
  ONEX_FTM_FCH_LOOP_SC           = 106,
  ONEX_FTM_SCH_LOOP_SC           = 107,
  ONEX_FTM_RELEASE_SC            = 108,
  ONEX_FTM_SET_MODE_SC           = 109,
  ONEX_FTM_FWD_DCCH_SC           = 110,
  ONEX_FTM_REV_DCCH_SC           = 111,
  ONEX_FTM_DCCH_LOOP_SC          = 112,
  ONEX_FTM_FWD_FCH_DCCH_SC       = 120,
  ONEX_FTM_REV_FCH_DCCH_SC       = 121,
  ONEX_FTM_FWD_HHO_SC            = 128,
  ONEX_FTM_1XA_RCPM_DATA_SC      = 136,
  ONEX_FTM_PAGING_SC             = 137,
  ONEX_FTM_ACCESS_PROBE_SC       = 138,
  ONEX_FTM_REV_SET_PWR_SC        = 140,
  ONEX_FTM_FCH_RESET_STATS_SC    = 141,
  ONEX_FTM_FCH_GET_STATS_SC      = 142,
  ONEX_FTM_1X_RX_LEVEL_REQ       = 146,
  ONEX_SLEEP_SC                  = 200, /* REVISIT: Not in FTM */
  ONEX_FTM_MAX_CMD
} onex_ftm_subcmd_type;

/*--------------------------------------------------------------------------
                      ONEX NONSIG MODE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum {
  NS1X_DMSS_MODE,
  NS1X_FTM_MODE,
  NS1X_nonsig_mode_sizer = 0x7FFFFFFF  /* enforces 32 bits */
} onex_nonsig_mode_type;

/*--------------------------------------------------------------------------
                      ONEX NONSIG STATE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_ENTER,                  /* Entry state */
  NS1X_INIT_CDMA,              /* CDMA initialization */
  NS1X_PIL_ACQ,                /* Pilot channel acquisition */
  NS1X_SYNC,                   /* Sync channel processing, combines
                                  CDMA_SYNC_ACQ, CDMA_SYNC_MSG */
  NS1X_IDLE,                   /* Idle state, we have acquired system
                                  time and are demodulating the paging
                                  channel. */
  NS1X_TRAFFIC,                /* Traffic state */
  NS1X_SYS_ACC,                /* System Access state */
  NS1X_REV_TRAFFIC,            /* Reverse traffic state */
  NS1X_EXIT,                   /* Exit state - Somebody determined that this number meant exit */
  NS1X_INVALID                 /* 1x Non Signaling is not supported. */

} onex_ns_state_enum_type;

/*--------------------------------------------------------------------------
                      ONEX NONSIG COMMAND ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_CMD_PILOT_ACQ,
  NS1X_CMD_DEMOD_SYNC,
  NS1X_CMD_MOD_FCH,
  NS1X_CMD_MOD_DCCH,
  NS1X_CMD_MOD_SCH,
  NS1X_CMD_DEMOD_FCH,
  NS1X_CMD_DEMOD_FDCCH,
  NS1X_CMD_DEMOD_FSCH,
  NS1X_CMD_FCH_LOOPBACK,
  NS1X_CMD_DCCH_LOOPBACK,
  NS1X_CMD_SCH_LOOPBACK,
  NS1X_CMD_RELEASE,
  NS1X_CMD_FWD_HHO,
  NS1X_CMD_SET_REV_LNK_PWR,
  NS1X_CMD_MOD_FCH_DCCH,
  NS1X_CMD_DEMOD_FFCH_FDCCH,
  NS1X_CMD_DEMOD_PAGING,
  NS1X_CMD_ACCESS,
  NS1X_CMD_SLEEP,
  NS1X_CMD_1XA_RC_PARAM,

  NS1X_CMD_NUM_CMDS,
  NS1X_CMD_INVALID = NS1X_CMD_NUM_CMDS

} onex_nonsig_cmd_enum_type;

/*--------------------------------------------------------------------------
                       TEST COMMAND STATUS ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_TEST_SUCCESS,
  NS1X_TEST_INVALID_CMD,
  NS1X_TEST_INVALID_STATE,
  NS1X_TEST_NO_SUCCESS,
  NS1X_test_cmd_status_sizer = 0x7FFFFFFF  /* enforces 32 bits */

} onex_nonsig_cmd_status_type;


/*--------------------------------------------------------------------------
                        ACQUISITION MODE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_ACQ_MODE_FULL,
  NS1X_ACQ_MODE_MINI,
  NS1X_ACQ_MODE_MICRO,
  NS1X_acq_mode_sizer = 0x7FFFFFFF

} onex_nonsig_acq_mode_type;

/*--------------------------------------------------------------------------
                        BAND CLASS ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_BAND_CLASS_CELLULAR,
  NS1X_BAND_CLASS_PCS,
  NS1X_band_class_sizer = 0x7FFFFFFF

} onex_nonsig_band_class_type;


/*--------------------------------------------------------------------------
                 FORWARD RADIO CONFIGURATION ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_FWD_RC_INVALID,
  NS1X_FWD_RC_1,
  NS1X_FWD_RC_2,
  NS1X_FWD_RC_3,
  NS1X_FWD_RC_4,
  NS1X_FWD_RC_5,
  NS1X_FWD_RC_11 = 11,

  NS1X_FWD_RC_1X_MAX,
  NS1X_fwd_rc_sizer = 0x7FFFFFFF

} onex_nonsig_fwd_rc_type;

/*--------------------------------------------------------------------------
                 REVERSE RADIO CONFIGURATION ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_REV_RC_INVALID,
  NS1X_REV_RC_1,
  NS1X_REV_RC_2,
  NS1X_REV_RC_3,
  NS1X_REV_RC_4,
  NS1X_REV_RC_8 = 8,

  NS1X_REV_RC_1X_MAX,
  NS1X_rev_rc_sizer = 0x7FFFFFFF

} onex_nonsig_rev_rc_type;

/*--------------------------------------------------------------------------
                   REVERSE FCH FRAME RATE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_REV_FCH_FULL_RATE,
  NS1X_REV_FCH_HALF_RATE,
  NS1X_REV_FCH_QTR_RATE,
  NS1X_REV_FCH_8TH_RATE,
  NS1X_rev_fch_rate_sizer = 0x7FFFFFFF

} onex_nonsig_rev_fch_rate_type;

/*--------------------------------------------------------------------------
                   REVERSE SCH FRAME RATE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_REV_SCH_1_5_RATE,
  NS1X_REV_SCH_2_7_RATE,
  NS1X_REV_SCH_4_8_RATE,
  NS1X_REV_SCH_9_6_RATE,
  NS1X_REV_SCH_19_2_RATE,
  NS1X_REV_SCH_38_4_RATE,
  NS1X_REV_SCH_76_8_RATE,
  NS1X_REV_SCH_153_6_RATE,
  NS1X_rev_sch_rate_sizer = 0x7FFFFFFF

} onex_nonsig_rev_sch_rate_type;


/*--------------------------------------------------------------------------
                        CODING TYPE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_DECODE_CONVOLUTIONAL,
  NS1X_DECODE_TURBO,
  NS1X_decoding_sizer = 0x7FFFFFFF

} onex_nonsig_sch_coding_type;


/*--------------------------------------------------------------------------
                        CHANNEL SPREAD RATE ENUM TYPE
--------------------------------------------------------------------------*/
typedef enum
{
  NS1X_RATE_1X,
  NS1X_RATE_2X,
  NS1X_RATE_4X,
  NS1X_RATE_8X,
  NS1X_RATE_16X,
  NS1X_spread_rate_sizer = 0x7FFFFFFF

} onex_nonsig_spread_rate_type;

/*===========================================================================

                    COMMAND DEFINITIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------
                        COMMAND HEADER STRUCT TYPE
--------------------------------------------------------------------------*/

typedef struct
{
  cmd_hdr_type cmd_hdr;                 /* A header for queue control */
  onex_nonsig_cmd_enum_type cmd;        /* Which command to run */
  onex_nonsig_cmd_status_type status;   /* The return status of the command */

} onex_nonsig_cmd_hdr_type;

/*--------------------------------------------------------------------------
                          PERFORM PILOT ACQUISITION
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_band_class_type band_class;
  word cdma_freq;
  onex_nonsig_acq_mode_type acq_mode;

} onex_nonsig_cmd_acq_pilot_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type hdr;
  onex_nonsig_cmd_acq_pilot_param_type param;

} onex_nonsig_pilot_acq_cmd_type;


/*--------------------------------------------------------------------------
                          PERFORM PAGING DEMOD
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  byte  channel_type;        /* Paging channel type - PCH, FCCCH, BCCH */
  byte  wait_for_pc_msg;     /* Flag telling FTM to wait for valid page */
  byte  walsh_code;          /* Walsh code for paging channel in use */
  byte  pc_rate;             /* Bit rate - 9600 or 4800 */
  dword long_code_mask;      /* Long code mask */
  byte  fccch_id;            /* f-ccch channel id */
  byte  fccch_rate;          /* f-ccch data rate */
  byte  bcch_rate;           /* f-bcch data rate */
  byte  code_rates;          /* code rates for either f-bcch or f-ccch */

} onex_nonsig_cmd_1x_paging_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_1x_paging_param_type param;

} onex_nonsig_demod_1x_paging_cmd_type;


/*--------------------------------------------------------------------------
                          PERFORM ACCESS PROBE SEQUENCE
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  byte  use_reach;        /* Access channel to use, TRUE = R-EACH, FALSE = R-ACH */
  byte  pam_sz;           /* Access Channel preamble length */
  byte  max_cap_sz;       /* Max Access Channel msg capsule size */
  byte  pagech;           /* Current Paging Channel */
  word  base_id;          /* Base station identification */
  word  pilot_pn;         /* Pilot PN sequence offset index */
  byte  init_pwr;         /* initial power */
  byte  nom_pwr;          /* nominal power */
  byte  pwr_step;         /* probe power step in dB */
  byte  num_step;         /* number of steps */
  byte  seq_num;          /* number of probe sequences */

} onex_nonsig_cmd_access_probe_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_access_probe_type param;

} onex_nonsig_acc_probe_cmd_type;

/*--------------------------------------------------------------------------
                         DEMODULATE FCH TRAFFIC CHANNEL
--------------------------------------------------------------------------*/

typedef PACK(struct)
{
  byte walsh;         /* walsh channel number (0-63 for 95A/B, 0-127 for cdma2000) */
  byte qof_mask_id;   /* quasi orthoganal function mask (0-3) where 0 is
                         true walsh codes */
} onex_nonsig_walsh_type;

typedef PACK(struct)
{
  byte  target_fer;
  byte  min_setpt;
  byte  max_setpt;

} onex_nonsig_fpc_olc_param_type;

typedef PACK(struct)
{
  onex_nonsig_fwd_rc_type radio_cfg;           /* the radio configuration of this FCH */
  onex_nonsig_walsh_type walsh;                /* walsh index */
  boolean bypass_lc;                           /* bypass long code (long code set to all zero) */
  byte frame_offset;                           /* frame offset */
  byte subchan_gain;                           /* Power control subchannel gain */
  boolean ffpc_enable;                         /* fast forward power control enable */
  dword fpc_mode;                              /* forward power control mode */
  byte init_setpt;                             /* initial set point for inner loop FPC on FCH */
  onex_nonsig_fpc_olc_param_type  olc_params;  /* outer loop power control parameters */

} onex_nonsig_cmd_demod_fch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_demod_fch_param_type param;

} onex_nonsig_demod_fch_cmd_type;

/*--------------------------------------------------------------------------
                         DEMODULATE DCCH TRAFFIC CHANNEL
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_fwd_rc_type radio_cfg;  /* the radio configuration of this FCH */
  onex_nonsig_walsh_type walsh;       /* walsh index */
  boolean bypass_lc;                  /* bypass long code (long code set to all zero) */
  word so_pri;                        /* Primary service option */
  word so_sec;                        /* Secondary service option */
  byte frame_offset;                  /* frame offset */
  boolean ilpc_enabled;               /* to indicate if inner loop power control is enabled or not */
  boolean olpc_enabled;               /* to indicate if outer loop power control is enabled or not */
  byte init_setpt;                    /* initial set point for inner loop FPC on DCCH */
  byte fpc_subchan_gain;              /* subchan gain value for inner loop FPC */
  onex_nonsig_fpc_olc_param_type  dcch_olc_params;  /* outer loop power control parameters */

} onex_nonsig_cmd_demod_dcch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_demod_dcch_param_type param;
} onex_nonsig_demod_dcch_cmd_type;

/*--------------------------------------------------------------------------
                    DEMODULATE SIMULTANEOUS FCH & DCCH
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_cmd_demod_fch_param_type fch;
  onex_nonsig_cmd_demod_dcch_param_type dcch;

} onex_nonsig_cmd_demod_fch_dcch_param_type;

typedef struct
{
   onex_nonsig_cmd_hdr_type  hdr;
   onex_nonsig_cmd_demod_fch_dcch_param_type param;

} onex_nonsig_demod_fch_dcch_cmd_type;

/*--------------------------------------------------------------------------
                  DEMODULATE SUPPLEMENTAL CHANNEL (IS95C SCH)
--------------------------------------------------------------------------*/

typedef PACK(struct)
{
  onex_nonsig_fwd_rc_type radio_cfg;            /* the radio configuration of this SCH */
  onex_nonsig_walsh_type walsh;                 /* walsh index */
  onex_nonsig_spread_rate_type sch_rate;        /* from 1X - 16X */
  onex_nonsig_sch_coding_type coding_type;      /* Convolutional or Turbo */
  boolean ffpc_enable;                          /* fast forward power control enable */
  dword fpc_mode;                               /* forward power control mode */
  byte init_setpt;                              /* initial set point for inner loop FPC on FCH */
  onex_nonsig_fpc_olc_param_type  olc_params;   /* outer loop power control parameters */

} onex_nonsig_cmd_demod_sch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_demod_sch_param_type param;

} onex_nonsig_demod_sch_cmd_type;

/*--------------------------------------------------------------------------
                         MODULATE FCH TRAFFIC CHANNEL
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_rev_rc_type radio_cfg;        /* Reverse FCH radio config */
  byte frame_offset;                        /* frame offset */
  onex_nonsig_rev_fch_rate_type fch_rate;
  boolean enable_rpc;                       /* Enable/disable reverse power control */
  word num_preambles;                       /* Preamble count for traffic channel initialization */
  dword tx_pattern;                         /* 32 bit pattern of bytes to use in the traffic frames */

} onex_nonsig_cmd_mod_fch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_mod_fch_param_type param;

} onex_nonsig_mod_fch_cmd_type;

/*--------------------------------------------------------------------------
                         MODULATE DCCH TRAFFIC CHANNEL
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_rev_rc_type dcch_rc;
  byte frame_offset;                  /* frame offset */
  boolean sch_on;
  onex_nonsig_rev_rc_type sch_rc;
  boolean sch_turbo;
  word sch_ltu_size;
  onex_nonsig_rev_sch_rate_type sch_rate;
  word num_preambles;
  dword tx_pattern;                   /* 32 bit pattern of bytes to use in the traffic frames */

} onex_nonsig_cmd_mod_dcch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_mod_dcch_param_type param;

} onex_nonsig_mod_dcch_cmd_type;

/*--------------------------------------------------------------------------
                         MODULATE SIMULTANEOUS FCH & DCCH
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_cmd_mod_fch_param_type  fch;
  onex_nonsig_cmd_mod_dcch_param_type dcch;

} onex_nonsig_cmd_mod_fch_dcch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_mod_fch_dcch_param_type param;

} onex_nonsig_mod_fch_dcch_cmd_type;

/*--------------------------------------------------------------------------
                         MODULATE SCH TRAFFIC CHANNEL
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  onex_nonsig_rev_rc_type radio_cfg;        /* Reverse FCH radio config */
  onex_nonsig_rev_sch_rate_type sch_rate;   /* Rate of reverse SCH from */
  onex_nonsig_sch_coding_type coding_type;  /* turbo or convolutional */
  dword tx_pattern;                         /* 32 bit pattern of bytes to use in the traffic frames */

} onex_nonsig_cmd_mod_sch_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_mod_sch_param_type param;

} onex_nonsig_mod_sch_cmd_type;

/*--------------------------------------------------------------------------
                          PERFORM HARD HANDOFF
--------------------------------------------------------------------------*/

typedef PACK(struct)
{
  onex_nonsig_band_class_type band_class;  /* the band class, Cellular or PCS */
  word cdma_freq;                          /* the cdma frequency channel */
  boolean enable_rpc;

} onex_nonsig_cmd_fwd_hho_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_fwd_hho_param_type param;

} onex_nonsig_fwd_hho_cmd_type;

/*--------------------------------------------------------------------------
                     PERFORM REVERSE POWER CONTROL
--------------------------------------------------------------------------*/
typedef PACK(struct)
{
  byte override_enable;
  int2 tx_power_10th_dbm;

} onex_nonsig_cmd_rev_link_pwr_setting_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_rev_link_pwr_setting_param_type param;

} onex_nonsig_cmd_rev_link_pwr_setting_cmd_type;

/*--------------------------------------------------------------------------
                          PERFORM RCPM
--------------------------------------------------------------------------*/
#define ONEX_NOSIG_RC_PARM_MAX 8          /* Maximum number of RC parameter records */

typedef PACK(struct)
{
  byte qof_set_in_use;                    /* QOF Set in use */
  byte rev_sch0_ack_ch_walsh;             /* Rev SCH0 ACK Chn Walsh Code */
  byte rev_sch0_ack_bit;                  /* Rev SCH0 ACK Bit */
  byte rev_sch1_ack_ch_walsh;             /* Rev SCH1 ACK Chn Walsh Code */
  byte rev_sch1_ack_bit;                  /* Rev SCH1 ACK Bit */
  byte for_fch_ccsh_interleaver_type;     /* Interleaver type used for Fwd FCH RC 12 */

} onex_nonsig_rc_pilot_params_type;

typedef PACK(struct)
{
  word pilot_pn;                                      /* Pilot PN of active pilot set*/
  onex_nonsig_rc_pilot_params_type rc_pilot_params;   /* Parameters specific to a pilot */

} onex_nonsig_rc_params_record_type;

typedef PACK(struct)
{
  word for_fch_ack_mask_rl_blanking;          /* Fwd FCH ACK Mask for RL Blanking */
  word for_fch_ack_mask_no_rl_blanking;       /* Fwd FCH ACK Mask for No RL Blanking */
  word for_sch_ack_mask_rl_blanking;          /* Fwd SCH ACK Mask for RL Blanking */
  word for_sch_ack_mask_no_rl_blanking;       /* Fwd SCH ACK Mask for No RL Blanking */
  word rev_fch_blanking_dutycycle;            /* Duty cycle value for Rev Link */
  byte rev_gain_adj_acs1;                     /* Chn adj gain for Rev Ack Chn (No Cells =1) */
  byte rev_gain_adj_acs2plus;                 /* Chn adj gain for Rev Ack Chn (No Cells >1)*/
  word power_control_mode;                    /* Power Control Mode */
  word power_control_step;                    /* Power control step size during RL Blanking */
  word rev_fch_ack_mask;                      /* Rev FCH ACK Mask */
  word rev_sch_ack_mask;                      /* Rev SCH ACK Mask */
  word for_n2m_ind;                           /* Fwd N2M Indicator */
  word for_fch_blanking_dutycycle;            /* Duty cycle value for Fwd Link */
  byte num_rc_params_rec;                     /* Number of RC Parameters records */

  onex_nonsig_rc_params_record_type rc_params_recs[ONEX_NOSIG_RC_PARM_MAX];

} onex_nonsig_cmd_radio_config_param_type;

typedef struct
{
  onex_nonsig_cmd_hdr_type  hdr;
  onex_nonsig_cmd_radio_config_param_type param;

} onex_nonsig_rcpm_cmd_type;


/*--------------------------------------------------------------------------
                          RETRIEVE FER STATS
--------------------------------------------------------------------------*/

typedef PACK(struct)
{
    uint32 fch_frame_total;
    uint32 fch_frame_errors;
    uint32 fch_sym_total;
    uint32 fch_sym_errors;
} onex_nonsig_cmd_rxc_fer_ser_type;

/*--------------------------------------------------------------------------
                             UNION OF ALL PACKED PARAMS
--------------------------------------------------------------------------*/
typedef PACK(union)
{
  onex_nonsig_cmd_acq_pilot_param_type            acq;        /* acquisition */
  onex_nonsig_cmd_demod_fch_param_type            f_fch;      /* forward FCH */
  onex_nonsig_cmd_demod_dcch_param_type           f_dcch;     /* forward DCCH */
  onex_nonsig_cmd_demod_sch_param_type            f_sch;      /* forward SCH */
  onex_nonsig_cmd_demod_fch_dcch_param_type       f_fch_dcch; /* forward fch and dcch */
  onex_nonsig_cmd_mod_fch_param_type              r_fch;      /* reverse FCH */
  onex_nonsig_cmd_mod_dcch_param_type             r_dcch;     /* reverse DCCH */
  onex_nonsig_cmd_mod_sch_param_type              r_sch;      /* reverse SCH */
  onex_nonsig_cmd_mod_fch_dcch_param_type         r_fch_dcch; /* reverse fch and dcch */
  onex_nonsig_cmd_rev_link_pwr_setting_param_type r_tx_power; /* reverse power setting in dBm*/
  onex_nonsig_mode_type                           mode;       /* change mode of phone */
  onex_nonsig_cmd_1x_paging_param_type            paging;     /* paging channel config */
  onex_nonsig_cmd_access_probe_type               access;     /* access probe parameters */
  onex_nonsig_cmd_radio_config_param_type         rcpm;       /* radio config param msg */
  onex_nonsig_cmd_fwd_hho_param_type              f_hho;      /* forward hard handoff*/
  onex_nonsig_cmd_rxc_fer_ser_type                fch_fer;    /* retrieve FL FER stats*/

} onex_nonsig_packed_param_type;

/*--------------------------------------------------------------------------
                             UNION OF ALL FTM COMMANDS
--------------------------------------------------------------------------*/
typedef union
{
  onex_nonsig_cmd_hdr_type                      hdr;
  onex_nonsig_pilot_acq_cmd_type                pilot_acq;
  onex_nonsig_demod_1x_paging_cmd_type          demod_paging;
  onex_nonsig_acc_probe_cmd_type                access_probe;
  onex_nonsig_rcpm_cmd_type                     rcpm_data;
  onex_nonsig_demod_fch_cmd_type                demod_fch;
  onex_nonsig_demod_dcch_cmd_type               demod_dcch;
  onex_nonsig_demod_sch_cmd_type                demod_sch;
  onex_nonsig_mod_fch_cmd_type                  mod_fch;
  onex_nonsig_mod_dcch_cmd_type                 mod_dcch;
  onex_nonsig_mod_sch_cmd_type                  mod_sch;
  onex_nonsig_demod_fch_dcch_cmd_type           demod_fch_dcch;
  onex_nonsig_mod_fch_dcch_cmd_type             mod_fch_dcch;
  onex_nonsig_fwd_hho_cmd_type                  fwd_hho;
  onex_nonsig_cmd_rev_link_pwr_setting_cmd_type rev_link_pwr_setting;

} onex_nonsig_cmd_type;

/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION onex_nonsig_check_cmd_queue

DESCRIPTION
   This function checks the onex_ns_cmd_q for any pending commands. It then
   calls the appropriate function to handle the command. Once the command
   processing has completed, the command status is updated and the command
   is placed on the requested queue and the requested signal is set for the
   client task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void onex_nonsig_check_cmd_queue(void);

/*===========================================================================

FUNCTION onex_nonsig_queue_cmd

DESCRIPTION
   This function handles requests to run tests and  other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
onex_nonsig_cmd_status_type onex_nonsig_queue_cmd
(
  onex_nonsig_packed_param_type *cmd_ptr,
  onex_ftm_subcmd_type command
);

/*============================================================================

FUNCTION      onex_nonsig_cmd_dispatcher

DESCRIPTION   This function will call the appropriate subcommand
              for a given FTM command.
DEPENDENCIES

RETURN VALUE  TRUE if there was a buffer available and the
              command type is in range.  FALSE otherwise.

SIDE EFFECTS  A command will be placed on the FTM_MCC command queue

============================================================================*/
onex_nonsig_cmd_status_type onex_nonsig_cmd_dispatcher
(
  byte sub_cmd_code,
  word length,
  onex_nonsig_packed_param_type *param
);

/*===========================================================================

FUNCTION onex_nonsig_is_ftm_mode

DESCRIPTION
  Function used to return the whether or not the mobile is in FTM mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if mode is FTM, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean onex_nonsig_is_ftm_mode (void);

/*===========================================================================

FUNCTION onex_nonsig_set_ftm_mode

DESCRIPTION
  Used to change the ftm mode if the mode provided is in the valid range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_set_ftm_mode (onex_nonsig_mode_type mode);

/*===========================================================================

FUNCTION onex_nonsig_ftm_init

DESCRIPTION
  Wrapper used to call into FTM to intialize the RF side. Placing this here
  to provide some seperation from RF for the 1x tasks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_ftm_init (void);

/*===========================================================================

FUNCTION onex_nonsig_get_rev_state

DESCRIPTION
  This function returns the current reverse link test state.

DEPENDENCIES

RETURN VALUE
   Current reverse link test state.

SIDE EFFECTS
  None.

===========================================================================*/

onex_ns_state_enum_type onex_nonsig_get_rev_state (void);

/*===========================================================================

FUNCTION onex_nonsig_get_fwd_state

DESCRIPTION
  This function returns the current forward link test state.

DEPENDENCIES

RETURN VALUE
   Current forward link test state.

SIDE EFFECTS
  None.

===========================================================================*/

onex_ns_state_enum_type onex_nonsig_get_fwd_state (void);

/*===========================================================================

FUNCTION onex_nonsig_initialize

DESCRIPTION
  Perform any necessary initialization prior to begining 1x non signaling.
  This should be called prior to begining any non signaling operations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_initialize (void);

/*===========================================================================

FUNCTION onex_nonsig_enable_mdsp

DESCRIPTION
  Enable the mdsp and download the image.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_enable_mdsp (void);

/*===========================================================================

FUNCTION onex_nonsig_disable_mdsp

DESCRIPTION
  Disable the mdsp if it is currently running.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_disable_mdsp (void);

/*===========================================================================

FUNCTION onex_nonsig_exit

DESCRIPTION
  Perform any necessary cleanup prior to exiting 1x non signaling.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_exit (void);

/*===========================================================================

FUNCTION onex_nonsig_system_determination

DESCRIPTION
  This is a special simplified version of system determination used only in
  FTM (hardcoded conditions used).

DEPENDENCIES
  None.

RETURN VALUE
  The action code causing the exit.

SIDE EFFECTS
  None.

===========================================================================*/

void onex_nonsig_system_determination(void);


/*===========================================================================

FUNCTION onex_nonsig_get_mc_cmd_q_cnt

DESCRIPTION
  Retrieve the count of queued commands in mc_cmd_q.

DEPENDENCIES
  None

RETURN VALUE
  Number of queued commands.

SIDE EFFECTS
  None

===========================================================================*/
int onex_nonsig_get_mc_cmd_q_cnt(void);

/*===========================================================================

FUNCTION onex_nonsig_get_onex_ns_cmd_q_cnt

DESCRIPTION
  Retrieve the count of queued commands in onex_ns_cmd_q.

DEPENDENCIES
  None

RETURN VALUE
  Number of queued commands.

SIDE EFFECTS
  None

===========================================================================*/
int onex_nonsig_get_onex_ns_cmd_q_cnt (void);

/*===========================================================================

FUNCTION onex_nonsig_process_mc_cmd

DESCRIPTION
  Process commands present on the mc_cmd_q..

DEPENDENCIES
  None

RETURN VALUE
  True if non-signaling mode should be exited, False otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean onex_nonsig_process_mc_cmd(void);

/*===========================================================================

FUNCTION ONEX_NONSIG_GET_ACQ_MODE

DESCRIPTION
  This function returns the current non-signaling acquisition mode.

DEPENDENCIES
  None

RETURN VALUE
  current non-signaling acquisition mode

SIDE EFFECTS
  None.

===========================================================================*/

db_acq_mode_type onex_nonsig_get_acq_mode(void);

/*===========================================================================

FUNCTION ONEX_NONSIG_SET_ACQ_MODE

DESCRIPTION
  This function updates the current non-signaling acquisition mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void onex_nonsig_set_acq_mode(db_acq_mode_type mode);

/*===========================================================================

FUNCTION ONEX_NONSIG_GET_BAND_CLASS

DESCRIPTION
  This function returns the current non-signaling band_class.

DEPENDENCIES
  None

RETURN VALUE
  Current non-signaling CDMA band class

SIDE EFFECTS
  None.

===========================================================================*/

srch_rx_band_type onex_nonsig_get_band_class(void);

/*===========================================================================

FUNCTION ONEX_NONSIG_SET_BAND_CLASS

DESCRIPTION
  This function updates the current non-signaling band_class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void onex_nonsig_set_band_class(srch_rx_band_type band_class);


/*===========================================================================

FUNCTION ONEX_NONSIG_WAIT_FOR_PC_MSG

DESCRIPTION
  This function returns whether or not 1x should wait for a valid pc message
  before reporting successful paging demod.

DEPENDENCIES
  None

RETURN VALUE
  TRUE = wait for page, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean onex_nonsig_wait_for_pc_msg(void);

/*===========================================================================

FUNCTION onex_nonsig_reset_fch_stats

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or
  INVALID_STATE, indicating the command is issued at a wrong phone state.

SIDE EFFECTS
  None.

===========================================================================*/

onex_nonsig_cmd_status_type onex_nonsig_reset_fch_stats (void);


/*===========================================================================

FUNCTION onex_nonsig_get_fch_stats

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or
  INVALID_STATE, indicating the command is issued at a wrong phone state.

SIDE EFFECTS
  None.

===========================================================================*/

onex_nonsig_cmd_status_type onex_nonsig_get_fch_stats
(
  onex_nonsig_cmd_rxc_fer_ser_type *fer_ser_report
);

#endif /*ONEX_NONSIG_H*/
