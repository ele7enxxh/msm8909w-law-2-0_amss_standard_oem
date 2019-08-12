#ifndef RXCMC_I_H
#define RXCMC_I_H
/*===========================================================================

                R X C D M A   S U B T A S K    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for the main
  task to interface with the RXC subtask.

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

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:   $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/rxcmc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   eye     1x2GTA Feature changes
02/12/15   jh      Changes for SW Demback
01/28/14   pap     Aggregating the variables into global structures.
09/13/13   pap     Adding changes for the m1x_stmr module to be integrated.
07/16/12   srk     Removed FEATURE_IS2000_REL_A_CC
06/15/12   srk     Removed FEATURE_IS2000_REL_A_CC_SMMC
03/04/12   jj      CSFB: Added new LTE OTA channel over which 1x can receive
                   messages over tunnel.
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/22/11   jtm     Added RXC_TC_TT_F for jumping directly to FL traffic.
01/25/11   jtm     Added Mux Vocoder interface module.
12/03/10   jtm     Removed MDR/SCCH support.
11/15/10   jtm     Initial SU API split for CP.
11/01/10   jtm     Moved additional symbols to rxcmc.h.
11/01/10   jtm     Modem 1x SU API changes.
10/30/10   jtm     Modem 1x SU API changes.
08/19/10   jtm     Decouple inhibit vocoder enable from FEATURE_MVS_MIGRATE.
07/07/10   jtm     Removed obsolete command definition from rxc_cmd_msg_type.
06/17/10   jtm     Wrap mvs inhibit and RXC_MVS_ENABLE_F definitions with
                   FEATURE_MODEM_1X_INHIBIT_MVS_ENABLE.
06/15/10   jtm     Added support for RXC_MVS_ENABLE_F and added
                   the inhibit_mvs_enable flag to the tci command from CP
                   to allow CP control to delay the vocoder configuration.
06/07/10   jtm     Added 1xA Ack/Nak test mode support.
05/05/10   jtm     Added support for new RLP API using FEATURE_IS95_RLP_MUX_COMMON_API
02/04/10   jtm     Removed FEATURE_PLT.
12/01/09   jtm     Added rxc_get_current_atmr_handle().
09/14/09   adw     Changes to support modem data header CMI refactoring.
08/05/09   jtm     Lint fixes.
07/29/09   jtm     Added FTM support for 1x Advanced RCPM.
07/06/09   jtm     Update to use cai_rcp_rxc_txc_type.
06/06/09   vlc     Added support for 1x Advanced (RC11.)
04/01/09   jtm     Added customer.h
03/20/09   jtm     Removed mctrans_v.h include.
03/04/09   adw     Added mc_v.h and mctrans_v.h includes.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
09/12/08   vlc     Added support for FEATURE_MVS_MIGRATE.
07/21/08   jtm     Removed FEATURE_IS2000_REL_C_DV code.
12/04/06   trc     MSM7600 support
08/02/06   trc     Mainline FEATURE_WINCE support
06/09/06   trc     Remove T_MSM6700 featurization
10/11/05   rkc     Add function prototype rxc_dec_version().
07/08/05   bn      Added new interface function to get FFPC parameters for MC
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/13/05   bn      First Revision

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

//mux
#include "mux1xdiag.h"
#include "muxvoc.h"

//drivers
#include "dec5000.h"
#include "deint.h"
#include "deint_i.h"

//cp
#include "cai.h"
#include "caix.h"
#include "caix_i.h"
#include "mctrans_i.h"
#include "mc.h"
#include "mc_v.h"

//other
#include "qw.h"
#include "queue.h"
#include "cmd.h"
#include "mdrrlp_api.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*  Possible commands to the RXC subtask                  */
typedef enum
{
  RXC_CDMA_F,          /* Switch to CDMA mode */
  RXC_SC_F,            /* Sync Channel */
  RXC_IDLE_F,          /* Begin Idle state operation */
  RXC_PC_F,            /* Paging Channel */
  RXC_CHAN_CONFIG_F,   /* Change channel monitoring requirement */
  RXC_TCI_F,           /* Traffic Channel initialization processing */
  RXC_TC_F,            /* Traffic Channel */
  RXC_SCH_F,            /* IS95C Supplemental Channel */
  RXC_SCCH_F,          /* IS95B Supplemental Code Channel */
  RXC_EXIT_F,          /* Exit to main RX task */
  RXC_VOX_ON_F,        /* Turn on vocoder vox */
  RXC_VOX_OFF_F,       /* Turn off vocoder vox */
  RXC_PWR_F,           /* Change power measurement parameters */
  RXC_ERASE_F,         /* Treat blank and burst as erasures */
  RXC_SO_F,            /* Change service options */
  RXC_RESET_F,         /* Reset fade timer */
  RXC_PWR_ON_F,        /* Resume power control reporting */
  RXC_PWR_RESET_F,     /* Reinitialize power control reporting counters */
  RXC_PWR_REP_DELAY_F, /* Commence PWR_REP_DELAY countdown */
  RXC_HHO_BEGIN_F,     /* Perform hard handoff processing, Replaces old RXC_HO_F
                          which is now used for sending HO indication to RXC */
  RXC_SLOT_F,          /* Check for a valid message to be received in 2 slots*/
  RXC_SLEEP_F,         /* Perform pre-sleep processing */
  RXC_ACC_FADE_F,      /* Use Access Channel fade timer */
  RXC_IDL_FADE_F,      /* Use Idle fade timer */
  RXC_WAKE_F,          /* Perform wakeup processing */
  RXC_IDLE_HO_F,       /* Perform idle handoff processing */
  RXC_TUNE_RF_F,       /* About to tune to TF/CF - start ignoring good frame */
  RXC_RIFHHO_CFS_GPS_F,/* Perform Return-If-Fail HHO or CFS or GPS */
  RXC_RETURNED_F,      /* Return to the SF for a failed Return-If-Fail HHO */
  RXC_RESUME_F,        /* Resume RX */
  RXC_PWR_CTRL_F,      /* Change to the new fwd power control parameters */
  RXC_HO_F,            /* Indicates a HO */
  RXC_FPC_SUBCHAN_GAIN_F, /* Change fpc sun chan gain value */
  RXC_RELEASE_SO_F,    /* rxc release service option */
  RXC_RELEASE_F,       /* Equivalent to sending a NULL SO to RXC */
  RXC_MC_TRANS_F,      /* Transaction request from MC */
  RXC_OLR_F,           /* Outer Loop Report request from MC */
  RXC_FTM_FCH_LOOPBACK,/* FCH loopback */
  /* Support FEATURE_FTM_DCCH */
  RXC_FTM_DCCH_LOOPBACK,    /* DCCH loopback */
  /* End of FEATURE_FTM_DCCH */
  RXC_FTM_SCH_LOOPBACK,/* SCH loopback */
  RXC_FTM_RCP_DATA_F,      /* Used to update Radio Config parameters for 1x Advanced in RXC traffic */
  RXC_GET_SETPT_F,     /* Get current setpoints */
  RXC_START_GPS_F,
  RXC_MVS_ENABLE_STATUS_F, /* Status of voc config command (now mvs_enable) */
  RXC_VOC_ENABLE_F,    /* Indication from MC to enable vocoder */
  RXC_TC_TT_F,         /* Indication from MC to jump directly to Traffic */
  RXC_TA_START_F,      /* Indication from MC to start tune away */
  RXC_TA_COMP_F,       /* Indication from MC to that tune away completed */

  RXC_COMMAND_COUNT    /* End of RXC commands */
} rxc_cmd_name_type;

/* Types for channels.  For the sake of simplicity, these are mapped
   to the enumeration in caix.h. */
typedef enum
{
  RXC_CDMA_SC = CAIX_SC,   /* Message received on CDMA Sync Channel */
  RXC_CDMA_PC = CAIX_PC,   /* Message received on CDMA Paging Channel */
  RXC_CDMA_BCCH = CAIX_BCCH,  /* Message received on CDMA BCCH Channel */
  RXC_CDMA_FCCCH = CAIX_FCCCH,/* Message received on CDMA FCCCH Channel */
  RXC_CDMA_TC = CAIX_TC,   /* Message received on CDMA Traffic Channel */

  RXC_CDMA_LTE = CAIX_LTE  /* Messages  received on the LTE air interface */

} rxc_chan_type;

/* RXC_IDLE_F */
/* IS-95 Paging Channel (PCH) config info */
typedef struct {
  cai_pch_rate_type rate; /* Data rate */
} rxc_pch_info_type;

/* Broadcast Control Channel (BCCH) Config Info */
typedef struct {
  cai_bcch_rate_type rate;  /* Data rate */
  cai_code_rate_type crate; /* Code rate */
  byte code_chan;           /* Walsh code channel */
} rxc_bcch_info_type;

/* Forward Common Control Channel (F-CCCH) Config Info */
typedef struct {
  cai_fccch_data_rate_type rate;         /* Data rate */
  cai_code_rate_type crate;             /* Code rate */
  cai_fccch_frame_size_type frame_size; /* Frame size */
  byte code_chan;                       /* Walsh code channel */
} rxc_fccch_info_type;

/* A union of all idle commands */
typedef struct {
  cai_rx_channel_id_type chan_id; /* Type of channel */

  union {
    rxc_bcch_info_type  bcch;
    rxc_fccch_info_type fccch;
    rxc_pch_info_type   pch;
 } config;
 boolean ebcsms;
} rxc_idle_chan_info_type;

typedef struct {
  byte fwd_sch_id;
  cai_radio_config_type fwd_sch_rc;
  word fwd_sch_mux_option;
  boolean fwd_sch_coding_type;
  byte fwd_sch_multi_fo;
} rxc_fwd_sch_cfg_info_type;

typedef struct {
  /* pre rel-A */
  boolean change_pri;           /* Indicator whether to switch primary SOs */
  word pri;                     /* Primary SO to switch to */
  boolean change_sec;           /* Indicator whether to switch sec SO */
  word sec;                     /* Secondary SO to switch to */

  /* rel-A */
  uint8 num_of_so_recs;
  struct
  {
    uint16  so;                 // SO for this connection
    byte    sr_id;
    byte    traffic_type;       // CAI_PRI_TRAF or CAI_SEC_TRAF
  } so_cfgs[MAX_NO_OF_SO_RECS];

  // Structure for forward power control parameters
  struct
  {
    boolean               included;     // When 0, none of following is included
    cai_fpc_mode_type     fpc_mode;
    cai_fpc_pri_chan_type fpc_pri_chan;
    // Parameters for FCH/DCCH OLPC
    struct
    {
      boolean included;
      mctrans_fpc_olpc_param_type params;
    }
    fch_olpc, dcch_olpc;
  } fpc;

  // Structure for channel configuration parameters
  struct
  {
    boolean included;               /* Indicates if channel is included */
    cai_radio_config_type rc;       /* RC of channel */
  }
  for_fch, for_dcch;

  // Fwd-supp information
  byte num_fwd_sch;
  rxc_fwd_sch_cfg_info_type fwd_sch_info[2]; /*Actual size is num_fwd_sch */
} rxc_scr_info_type;

/* Possible status value for a command */
typedef enum {
  RXC_DONE_S,         /* Generic OK return */
  RXC_BUSY_S,         /* Command is being processed */
  RXC_BADCMD_S,       /* Invalid command */
  RXC_BADPARM_S,      /* Bad Parameters */
  RXC_BADSEQ_S,       /* Command is out of sequence */
  RXC_NOLOCK_S,       /* Couldn't tune synthesizer */
  RXC_MAX_S           /* Number of status codes -- MUST be last entry */
} rxc_status_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is the big one - All the possible commands, unionized */

/* The common header for all the commands */
typedef struct {
    cmd_hdr_type       cmd_hdr;      /* command header - includes q_link */
    rxc_cmd_name_type  command;      /* The command */
    rxc_status_type    status;       /* Space for message status */
} rxc_hdr_type;

typedef struct {                  /* RXC_CDMA_F */
  rxc_hdr_type hdr;
} rxc_cdma_cmd_type;

typedef struct {          /* RXC_SC_F */
  rxc_hdr_type hdr;
} rxc_sc_cmd_type;

/* RXC_IDLF_F command type */
typedef struct {
   rxc_hdr_type            hdr;
   qword                   frame;     /* Frame number for jump to Hyperspace */
   cai_rx_channel_id_type  chan_id;   /* Type of channel */
   rxc_idle_chan_info_type chan_info; /* channel configuration */
} rxc_idle_cmd_type;

typedef struct {          /* RXC_PC_F */
  rxc_hdr_type hdr;
  cai_pch_rate_type rate; /* Paging Channel data rate */
  qword frame;            /* Frame number for jump to Hyperspace */
} rxc_pc_cmd_type;

typedef struct {                  /* RXC_CHAN_CONFIG_F */
  rxc_hdr_type             hdr;
  byte                     num_chan;
    /* Number of channels specified in priority list */
  boolean                  skip_frame;
  rxc_idle_chan_info_type  chan_info[CAI_MAX_NUM_CHAN_MONITOR];
} rxc_chan_config_cmd_type;

typedef struct {          /* RXC_TCI_F */
  rxc_hdr_type hdr;

  /* Structure for passing forward power control parameters */
  struct
  {
    /* For DV Channel Configurations with no Fwd-Fundicated channel, the
     * boolean included is set to TRUE, but fpc_mode and fpc_pri_chan are
     * set to RESERVED and CPCCH respectively indicating no FFPC in use and
     * to ignore rest of fpc structure. */
    boolean               included; /* When 0, none of following is included */
    cai_fpc_mode_type     fpc_mode;
    cai_fpc_pri_chan_type fpc_pri_chan; /* 0 = FCH, 1 = DCCH, 255=CPCCH */
    uint8                 fpc_subchan_gain;
    /* Parameters for FCH/DCCH OLPC */
    struct
    {
      boolean included;
      uint8   init_setpt;
      mctrans_fpc_olpc_param_type params;
    }
    fch_olpc, dcch_olpc;
  } fpc;

  /* Following 2 members temporarily included here per Harsha -- MUX TEAM
   * TO DETERMINE IF THESE CAN BE REMOVED */
  boolean ilpc_enabled;
  boolean olpc_enabled;

  /* Structure for passing channel configuration parameters */
  /* For DV Channel Configurations with no Fwd-Fundicated channel, the
   * for_fch.included and for_dcch.included fields are set to FALSE. */

  chan_cfg_type for_fch;
  chan_cfg_type for_dcch;

  uint8 frame_offset;            /* Fundicated channel frame offset. */

  uint8 rev_pwr_cntl_delay;  /* Rev FCH Pwr Ctrl delay */

  boolean enable_skip_tci;   /* disables check for good frames in TCI */
  uint8 p_rev_in_use;  /* Current p_rev in use,Needed for frame formatting */

  /* Parameters used in 1x Advance mode.  If MC has not received the RCPM by the
  ** time this command is sent, MC sends RXC the default value for these parameters.
  ** MC will update the parameters when it receives the RCPM from BS using
  ** the transaction engine interface.
  */
  cai_rcp_rxc_txc_type rcp_init;

  boolean inhibit_voc_enable;

} rxc_tci_cmd_type;

typedef struct {          /* RXC_TC_F */
  rxc_hdr_type hdr;
  boolean enable_t79; /* Support for Fast Call Setup Bypass 2 Good Frame feature */
} rxc_tc_cmd_type;

typedef struct {        /* RXC_SCH_F */
  rxc_hdr_type hdr;
  deint_rc_type rc;
  dec_sch_rate_type sch_rate;     /* 2X, 4X, 8X, 16X */
  boolean double_size_pdu;   /* Indicates if MUX PDU (Type 3) is single
                               or double sized */
  dec_coding_type code_type;
  dec_frame_type frame_type;  /* Indicates 20ms/40ms/80ms frame */
} rxc_sch_cmd_type;

typedef struct         /* RXC_SCCH_F */
{
  rxc_hdr_type hdr;
  byte num_sup;
  /* Number of Supplemental Code Channels */
} rxc_scch_cmd_type;

typedef struct {          /* RXC_EXIT_F */
  rxc_hdr_type hdr;
} rxc_exit_cmd_type;

typedef struct {          /* RXC_VOX_ON_F */
  rxc_hdr_type hdr;
} rxc_vox_on_cmd_type;

typedef struct {          /* RXC_VOX_OFF_F */
  rxc_hdr_type hdr;
} rxc_vox_off_cmd_type;

typedef struct {          /* RXC_PWR_F */
  rxc_hdr_type hdr;
  boolean pwr_thresh_enable;
    /* Threshold report mode indicator */
  boolean pwr_period_enable;
    /* Periodic report mode indicator */
  byte pwr_rep_thresh;
    /* Power control reporting threshold */
  word pwr_rep_frames;
    /* Power control reporting frame count */
  word pwr_rep_delay;
    /* Power report delay */
} rxc_pwr_cmd_type;

typedef struct {          /* RXC_PWR_ON_F */
  rxc_hdr_type hdr;
  boolean pwr_thresh_enable;
    /* Threshold report mode indicator */
  boolean pwr_period_enable;
    /* Periodic report mode indicator */
} rxc_pwr_on_cmd_type;

typedef struct {          /* RXC_PWR_RESET_F */
  rxc_hdr_type hdr;
} rxc_pwr_reset_cmd_type;

typedef struct {          /* RXC_PWR_REP_DELAY_F */
  rxc_hdr_type hdr;
  boolean pwr_thresh_enable;
    /* Threshold report mode indicator */
  boolean pwr_period_enable;
    /* Periodic report mode indicator */
} rxc_pwr_rep_delay_cmd_type;

typedef struct {          /* RXC_ERASE_F */
  rxc_hdr_type hdr;
} rxc_erase_type;

typedef struct {          /* RXC_SO_F */
  rxc_hdr_type hdr;
  rxc_scr_info_type scr;
  qword action_time;
  byte transaction_num;

} rxc_so_cmd_type;

typedef struct {          /* RXC_RELEASE_SO_F */
  rxc_hdr_type hdr;
  byte         sr_id;
} rxc_release_so_type;

typedef struct {          /* RXC_RESET_F */
  rxc_hdr_type hdr;
} rxc_reset_type;

typedef struct {          /* RXC_SLEEP_F */
  rxc_hdr_type hdr;
} rxc_sleep_type;

typedef struct {          /* RXC_ACC_FADE_F */
  rxc_hdr_type hdr;
} rxc_acc_fade_type;

typedef struct {          /* RXC_IDL_FADE_F */
  rxc_hdr_type hdr;
} rxc_idl_fade_type;

typedef struct {          /* RXC_WAKE_F */
  rxc_hdr_type hdr;
  word idle_pgslot;       /* Hashed paging channel slot */
  word sci_mask;          /* Mask to get position-in-slot from pgslot */
  byte slot_mask;         /* indicate the type of slot */
  word slot;              /* the slot RXC is waking up to */
  boolean bcsms_rxd;      /* the scheduled bcsms received in the sched'd slot */
  boolean page_match;     /* Indicate if quick page match has to be performed */
} rxc_wake_type;



typedef struct {          /* RXC_IDLE_HO_F */
  rxc_hdr_type hdr;
} rxc_idle_ho_type;

typedef struct {          /* RXC_TUNE_RF_F */
  rxc_hdr_type hdr;
} rxc_tune_rf_type;

typedef struct {          /* RXC_RIFHHO_CFS_GPS_F */
  rxc_hdr_type hdr;
  boolean is_hho;         /* Flag if this is a Hard Handoff or CFS */
  byte timeout;           /* Timeout (TF_WAIT_TIME) in 80ms units */

  byte gps;               /* GPS in progress */
} rxc_rif_hho_type;

typedef struct {          /* RXC_RETURNED_F */
  rxc_hdr_type hdr;
  struct
  {
    boolean             included;
    rxc_scr_info_type   data;
  } scr;                  /* SCR to be restored */
  struct
  {
    boolean included;
    uint8   value;
  } fpc_subchan_gain,     /* FPC subchan gain */
    rev_pwr_cntl_delay;   /* Rev FCH Pwr Ctrl delay  to be restored */
  byte p_rev_in_use;  /* Needed for frame formatting */
} rxc_returned_type;

typedef struct {          /* RXC_RESUME_F */
  rxc_hdr_type hdr;
} rxc_resume_type;

typedef struct {
  rxc_hdr_type hdr;
  qword action_time;        /* action time when the changes have to take place */
  byte transaction_num;     /* Transaction num for this command */
  byte fpc_mode;            /* Fwd Pwr Ctrl Mode */
  cai_fpc_pri_chan_type fpc_pri_chan;     /* FPC primary channel, 0-fch,1-DCCH */
  boolean fpc_fch_incl;     /* Indicator if FPC for FCH is included */
  boolean master_sec_incl;  /* Is master chan included for sec inner loop FPC */
  byte fpc_sec_chan;        /* Secondary channel */
  byte num_sups;            /* Num of supplementals for which FPC info is given */
  mctrans_fpc_olpc_param_type fch_olc_params; /* FCH FPC outer loop params */
  mctrans_fpc_olpc_param_type sup_olc_params[2]; /*Outer loop parameters for supp channels */
                            /* revisit */
  byte sch_ids[2];          /* SCH_IDs to which the above outer loop params apply */
                            /* revisit */
  boolean fpc_thresh_incl;
  byte fpc_setpt_thresh;
  boolean fpc_thresh_sch_incl;
  boolean fpc_setpt_thresh_sch;

} rxc_pwr_ctrl_cmd_type;


typedef struct
{
    rxc_hdr_type hdr;
    byte transaction_num; //Transaction num for this transaction
    qword action_time;    //Action time for this transaction
    boolean hho_ind;      //Indicates if this is a HHO (To wait for sync points)
    boolean rif_ind;      //Indicates if this Return on Failure HHO
    boolean reset_fpc;    // Indicates if FPC is to be reset after HO
    boolean fo_change_ind; // Indicates if FO has changed (so vocoder can be reset)
    byte frame_offset;
    boolean fpc_subchan_gain_incl;  //Indicates if fpc_subchan_gain is incl
    byte fpc_subchan_gain; //Units of 0.25 db
    boolean scr_incl;  // Indicates if SCR info is included
    rxc_scr_info_type scr;  // SCR info
} rxc_ho_cmd_type;

typedef struct
{
  rxc_hdr_type hdr;
  byte transaction_num;
  qword action_time;
  byte fpc_subchan_gain; // Units of 0.25 db
} rxc_fpc_subchan_gain_cmd_type;

typedef struct
{
  rxc_hdr_type hdr;
} rxc_hho_begin_cmd_type;

typedef struct
{
  rxc_hdr_type hdr;
} rxc_rel_cmd_type;

typedef struct
{
  rxc_hdr_type hdr;
} rxc_olr_cmd_type;

typedef struct {
  rxc_hdr_type      hdr;
  boolean           loopback_on;
} rxc_ftm_loopback_cmd_type;

typedef struct {
  rxc_hdr_type      hdr;
  cai_rcp_rxc_txc_type  rcp_data;
} rxc_ftm_rcp_cmd_type;

typedef struct {
  rxc_hdr_type      hdr;
/* The following fields are populated by RXC before setting the
   CMD_DONE_SIG with MC, These fields are invalid when the command is
   sent from MC to RXC */
  boolean fch_incl;
  byte fpc_fch_curr_setpt;
  boolean dcch_incl;
  byte fpc_dcch_curr_setpt;
  byte num_sup;
  struct {
    byte sch_id;
    byte fpc_sch_curr_setpt;
  } sch_setpt_info[CAI_OUTER_RPT_MAX_SUP];
} rxc_get_setpt_cmd_type;

typedef struct {                  /* RXC_START_GPS_F */
  rxc_hdr_type             hdr;
} rxc_gps_cmd_type;

typedef struct {              /* RXC_MVS_ENABLE_STATUS_F */
  rxc_hdr_type                hdr;
  mc_mvs_enable_enum_type     status;
    /* Status of mvs_enable() command (vocoder driver) */
} rxc_mvs_enable_cmd_type;

typedef struct {              /* RXC_TA_START_F */
  rxc_hdr_type                hdr;
  srch_ta_rsn_type            ta_type;
    /* Tune away type : QTA or LTA */
} rxc_ta_start_cmd_type;

typedef struct {              /* RXC_TA_COMP_F */
  rxc_hdr_type                hdr;
  srch_ta_rsn_type            ta_type;
    /* Tune away type : QTA or LTA */
} rxc_ta_comp_cmd_type;

/* And, the union of them all.... */
typedef union {
  rxc_hdr_type               hdr;
  rxc_cdma_cmd_type          cdma;
  rxc_sc_cmd_type            sc;
  rxc_idle_cmd_type          idle;
  rxc_pc_cmd_type            pc;
  rxc_chan_config_cmd_type   chan_config;
  rxc_tci_cmd_type           tci;
  rxc_tc_cmd_type            tc;
  rxc_sch_cmd_type           sch;
  rxc_scch_cmd_type          scch;
  rxc_exit_cmd_type          exit;
  rxc_vox_on_cmd_type        vox_on;
  rxc_vox_off_cmd_type       vox_off;
  rxc_pwr_cmd_type           pwr;
  rxc_pwr_on_cmd_type        pwr_on;
  rxc_pwr_reset_cmd_type     pwr_reset;
  rxc_pwr_rep_delay_cmd_type pwr_rep_delay;
  rxc_erase_type             erase;
  rxc_so_cmd_type            so;
  rxc_release_so_type        release_so;
  rxc_reset_type             reset;
  rxc_sleep_type             sleep;
  rxc_acc_fade_type          acc_fade;
  rxc_idl_fade_type          idl_fade;
  rxc_wake_type              wake;
  rxc_idle_ho_type           idle_ho;
  rxc_tune_rf_type           tune_rf;
  rxc_rif_hho_type           rif_hho;
  rxc_returned_type          ret;
  rxc_resume_type            resume;
  rxc_pwr_ctrl_cmd_type      pwr_ctrl;
  rxc_ho_cmd_type            ho;
  rxc_fpc_subchan_gain_cmd_type fpc_subchan;
  rxc_hho_begin_cmd_type     hho_begin; // Replaces the old rxc_ho_type command
  rxc_rel_cmd_type           rel;       //Equivalent to sending SO cmd with null SO
  rxc_olr_cmd_type           olr;       // Outer Loop Report command from MC
  rxc_ftm_loopback_cmd_type  loopback;
  rxc_ftm_rcp_cmd_type  rcp;
  rxc_get_setpt_cmd_type     setpt;
  rxc_gps_cmd_type           gps;
  rxc_mvs_enable_cmd_type    mvs_enable; /* MC reports status of mvs_enable */
  rxc_ta_start_cmd_type      ta_start;
  rxc_ta_comp_cmd_type       ta_comp;

} rxc_cmd_msg_type;

 /* Possible values for requesting an interrupt in one of the 4 bins
    in a 20ms frame */

typedef enum {
  RXC_ATMR_5MS_FRAME_1 = 0 ,
  RXC_ATMR_5MS_FRAME_2,
  RXC_ATMR_5MS_FRAME_3,
  RXC_ATMR_5MS_FRAME_4
} rxc_atmr_5ms_enum_type;

/* States of RXC state machine */
typedef enum
{
  RXC_CDMA_STATE,
  RXC_SC_STATE,
  RXC_PC_STATE,
  RXC_SC_STATE_SW,
  RXC_PC_STATE_SW,
  RXC_TCI_STATE,
  RXC_TC_STATE,
  RXC_TA_STATE,
  RXC_EXIT_STATE
} rxc_state_type;

/* Type for sending crc counters */
typedef struct
{
  dword good_crcs;  /* number of messages received with good CRCs */
  dword bad_crcs;   /* number of messages received with bad CRCs */
}rxc_crc_cnt_type;


/* The RXC command queue is declared external here so that the RX task
** can check to make sure an item is on the queue before passing control
** to the RXC subtask. */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void rxc_powerup_init( void );
  /* use this to perform powerup intialization */

extern void  rxc_cmd ( rxc_cmd_msg_type *cmd_ptr );
  /* use this to submit a command */

extern void  rxc_subtask( void );
  /* main rxc subtask */


extern rxc_state_type rxc_get_state(void);
  /* report current state of Receive task state machine */


extern word rxc_get_mode(void);
  /* report current mode (voice or Markov) */

extern void rxc_markov_reset( void );
  /* reset Markov test statistics */


extern void rxc_get_sc_crc( rxc_crc_cnt_type *crc_ptr);
  /* Get Sync Channel message crc counts */

extern void rxc_get_pc_crc( rxc_crc_cnt_type *crc_ptr);
  /* Get Paging Channel message crc counts */

extern void rxc_set_frame_offset ( byte frame_offset );
  /* Set the frame offset for use in time correction calculation */

/*===========================================================================

FUNCTION FFPC_GET_DEFAULT_FCH_INIT_SETPT_VAL

DESCRIPTION


DEPENDENCIES

RETURN VALUE
 Return FFPC_DEFAULT_FCH_INIT_SETPT_VAL.

SIDE EFFECTS
  None.
===========================================================================*/
int8 ffpc_get_default_fch_init_setpt_val(void);

/*===========================================================================

FUNCTION RXC_DEC_VERSION

DESCRIPTION
  Wrapper function for dev_version().
  Return the hardware version number for this circuit.

DEPENDENCIES
  None

RETURN VALUE
  Returns the version number of the circuit

SIDE EFFECTS
  None

===========================================================================*/
extern byte rxc_dec_version
(
  void
);


#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION RXC_CONFIG_TEST_ACK_OR_NAK

DESCRIPTION
  API for CP task to use when configuring the test directive for ACK or NAK
  reception for SO74 (Markov) and SO75 (Loopback). This function simply
  copies the provided paramter for use later during counter updates.

  Parameters:
  ack_or_nak: Bit mask signifying the test configuration

  0x0 = Normal Operation
  0x1 = Ack expected
  0x2 = Nak expected

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rxc_config_test_ack_or_nak( uint8 ack_or_nak );
#endif /* FEATURE_IS2000_1X_ADV */

#endif /* RXC_I_H */

