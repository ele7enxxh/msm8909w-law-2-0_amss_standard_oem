#ifndef TXCMC_I_H
#define TXCMC_I_H
/*===========================================================================

          T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the transmit task that don't fit
  into one of the other H files such as CAI.H

  Copyright (c) 2005 - 2014 Qualcomm Technologies, Inc. 
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

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/txcmc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   eye     1x2GTA Feature changes
08/21/14   eye     Added MVS_STANDBY during HHO with frame_offset change.
12/13/11   srk     Mainlined FEATURE_RL_JUMP_ENHANCEMENT
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/06/10   jtm     Remove deprecated RF file include.
11/15/10   jtm     Initial SU API split for CP.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
11/01/10   jtm     Modem 1x SU API changes.
07/07/10   jtm     Removed obsolete command definition from txc_cmd_type.
06/07/10   jtm     Added 1xA ACK/NAK test mode support.
02/04/10   jtm     Removed FEATURE_PLT.
07/29/09   jtm     Added FTM support for 1x Advanced RCPM.
07/06/09   jtm     Update to use cai_rcp_rxc_txc_type.
04/08/09   jtm     Initial 1X Advanced feature support.
04/01/09   jtm     Added customer.h and featurize necessary includes.
03/20/09   jtm     Split txcmc.h into public (.h) and private (_v.h)
03/20/09   jtm     Cleaned up header file inclusions.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
09/12/08   vlc     Added support for FEATURE_MVS_MIGRATE.
07/21/08   jtm     Removed FEATURE_IS2000_REL_D_DV and
                   FEATURE_DELAYED_TX_CLOCK_SHUTDOWN code.
12/04/06   trc     MSM7600 support
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
08/02/06   trc     Mainline FEATURE_WINCE support
10/17/05   rkc     Remove prototype for wrapper function txc_enc_set_frame_off().
10/12/05   rkc     Add prototype for wrapper function txc_enc_set_frame_off().
09/15/05   rkc     Add boolean hold_enc_clk_on to txc_jump_cmd_type.
07/08/05   bn      Added new interface function to get RLGC parameters for MC
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
                   Allow function txc_ok_to_sleep() to be visible for
                   FEATURE_RL_JUMP_ENHANCEMENT.
                   Added new parameter required in Jump to hyperspace to
                   txc_jump_cmd_type under FEATURE_RL_JUMP_ENHANCEMENT.
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

//mux
#include "txcmc.h"

//drivers
#include "enc.h"

//cp
#include "cai.h"
#include "cai_v.h"
#include "caii.h"  //IS2000 change
#include "mctrans_i.h"
#include "mc.h"
#include "mc_v.h"

//other
#include "queue.h"
#include "qw.h"
#include "cmd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


#ifdef FEATURE_IS2000_1X_ADV
typedef enum
{
  RACKCH1 = 0,
  RACKCH2
} txc_rackch_type;
#endif /* FEATURE_IS2000_1X_ADV */

typedef enum {
  TXC_CDMA_F,                      /* Switch to CDMA mode */
  TXC_PC_F,                        /* Move to Paging Channel */
  TXC_TC_F,                        /* Move to Traffic Channel */
  TXC_EXIT_F,                      /* Exit CDMA operation */
  TXC_MSG_F,                       /* Send a message */
  TXC_ACCESS_F,                    /* Make an access attempt */
  TXC_ACC_STOP_F,                  /* Stop access attempts immediately */
  TXC_OFF_TX_F,                    /* Stop transmitting */
  TXC_ON_TX_F,                     /* Start transmitting */
  TXC_NULLS_F,                     /* Transmit null data on TC */
  TXC_VOC_F,                       /* Wait for voc initialization */
  TXC_SO_F,                        /* Switch service options */
  TXC_RDR_F,                       /* Reverse data rate limit parameters */
  TXC_FRAME_OFFSET_F,              /* Set frame offset */
  TXC_ACC_PROBE_F,                 /* Transmit access probe */
  TXC_ACC_SUSPEND_F,
  TXC_PWR_CTRL_F,                 /* Pass along new power ctrl parameters */
  TXC_CONNECT_SO_F,               /* Indicates we are in CONV substate,
                                     Connect stored SO with vocoder if applicable */
  TXC_HO_F,                       /* HO command */
  TXC_HHO_SUCCESS_F,              /* Indicates that a HHO was successful, Can program
                                     necessary parameters */
  TXC_RETURNED_F,                 /* Indicates a return back to SF after a failed HHO,
                                     Reprogram necessary params back to old values */
  TXC_RELEASE_SO_F,               /* release the service option */
  TXC_RELEASE_F,                  /* Sent after a release, Equivalent to old command of
                                     sending a NULL SO command */
  TXC_MC_TRANS_F,                 /* Transaction from MC */

  TXC_FTM_SCH_F,                  /* Command to start SCH */
  TXC_FTM_FCH_LOOPBACK,           /* Start FCH loopback */
  /* Support FEATURE_FTM_DCCH */
  TXC_FTM_DCCH_LOOPBACK,           /* Start DCCH loopback */
  /* End of FEATURE_FTM_DCCH */
  TXC_FTM_SCH_LOOPBACK,           /* Start SCH loopback */

  TXC_FTM_RCP_DATA_F,      /* Used to update Radio Config parameters for 1x Advanced in TXC traffic */

//txc_reach_on
  TXC_REACH_F,                    /* Make an access attempt */
  TXC_REACH_PROBE_F,              /* Change R-EACH probe parameters */
//txc_reach_on
  TXC_JUMP_F,                     /* Jump to Hyperspace */
  TXC_TT_F,                       /* go the timing transfer state*/

  TXC_MVS_ENABLE_STATUS_F, /* Status of voc config command (now mvs_enable) */
  TXC_MVS_STANDBY_STATUS_F, /* Status of mvs_standby */
  TXC_SCR_F,                /* Service configuration data */
  TXC_TA_START_F,           /* Indication from MC to start tune away */
  TXC_TA_COMP_F,           /* Indication from MC to that tune away completed */

  TXC_COMMAND_COUNT                /* End of the enum */
} txc_cmd_name_type;

/* Possible status value for a command */
typedef enum {
  TXC_DONE_S,                      /* Generic OK return */
  TXC_BUSY_S,                      /* Command is being processed */
  TXC_BADCMD_S,                    /* Invalid command */
  TXC_ERR_S,                       /* Error attempting to do something */
  TXC_MAXIMUM_S                    /* always last, this counts statuses */
} txc_status_type;

/* Signaling mode for txc_tc_frame_type */
typedef enum
{
  NORM  = 0,                         /* normal mode */
  DIM   = 1,                         /* dim and burst frame */
  BLANK = 2                          /* blank and burst frame */
} txc_sig_frame_type;

/* The common Header for all the commands */
typedef struct {
  cmd_hdr_type         cmd_hdr;        /* command header */
  txc_cmd_name_type    command;        /* The command */
  txc_status_type       status;        /* Space for message status */
} txc_hdr_type;

typedef struct {                  /* TXC_CDMA_F */
  txc_hdr_type   hdr;
} txc_cdma_cmd_type;

typedef struct {                  /* TXC_PC_F */
  txc_hdr_type   hdr;
} txc_pc_cmd_type;

typedef struct {                  /* TXC_TT_F */
  txc_hdr_type   hdr;
} txc_tt_cmd_type;

typedef struct {                  /* TXC_TC_F */
  txc_hdr_type   hdr;
  uint16         sigs;       /* Signal to set when TX task has acquired
                              * timing and moved to Traffic state */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal */
  qword          mask;            /* Long mask */

  /* Note that the rev_fch.included or rev_dcch.included flags above may
   * be set to TRUE without the corresponding forward channels in RXC_TCI_F
   * being TRUE. */
  chan_cfg_type  rev_fch;
  chan_cfg_type  rev_dcch;

  uint8 frame_offset;  /* Fundicated channel frame offset. */

  uint8 rl_gain_adj;   /*In 8-bit 2's compliment, units of 1db*/
  uint8 rlgain_traffic_pilot; /* In 8-bit 2's compliment */
  boolean rev_fch_gating_on; /* Indicates if 8th rate gating is on
                               * or off on R_FCH */
  uint16 signaling_phy_channel; /* physical channel(s) for signaling */

  enc_rate_type     fch_rate;

  /* Support FEATURE_FTM_DCCH */
  enc_rate_type     dcch_rate;
  /* End of FEATURE_FTM_DCCH */

  boolean           sch_on;
  cai_radio_config_type rev_sch_rc;
  boolean           sch_turbo;
  uint16             sch_ltu_size;
  enc_sch_rate_type sch_rate;
  uint16             num_preambles;
  uint32             tx_pattern;

  uint8 p_rev_in_use; /* Needed for frame formatting */

  struct
  {
    /* Included=TRUE only if RTC_NOM_PWR_USE_IND=TRUE */
    boolean included;

    /* Reverse Traffic Channel Nominal Power.
     * The nominal power to be used by the mobile station for its initial
     * transmission on the Reverse Traffic Channel if RTC_NOM_PWR_USE_IND
     * is set to '1'.
     * 5 bit two's complement signed, in units of 1dB. */
    uint8 rtc_nom_pwr;
  } rtc_nom_pwr;

  boolean turn_on_transmitter;

  cai_rcp_rxc_txc_type rcp_init;
} txc_tc_cmd_type;

typedef struct {
  txc_hdr_type      hdr;
  cai_radio_config_type radio_cfg;
  boolean           sch_on;
  enc_sch_rate_type sch_rate;
  boolean           sch_turbo;
  dword             tx_pattern;
} txc_ftm_sch_cmd_type;

typedef struct {
  txc_hdr_type      hdr;
  boolean           loopback_on;
} txc_ftm_loopback_cmd_type;

typedef struct {
  txc_hdr_type      hdr;
  cai_rcp_rxc_txc_type  rcp_data;
} txc_ftm_rcp_cmd_type;

typedef struct {                  /* TXC_EXIT_F */
  txc_hdr_type   hdr;
} txc_exit_cmd_type;

typedef enum {
  CONFIRM_MSG_SENT,     /* rxtx wants the confirmation when the msg is sent */
  CONFIRM_MSG_BUILT     /* rxtx wants the confirmation when the msg is built */
} txc_confirmation_type;

typedef struct {                  /* TXC_MSG_F */
  txc_hdr_type   hdr;
  txc_confirmation_type confirmation_type;  /* indicates when rxtx is
                           expecting to receive the
                           confirmation. */
  word          len;              /* number of bits in message */
  byte          *msg_ptr;         /* the message to send */
} txc_msg_cmd_type;

typedef struct {                  /* TXC_ACCESS_F */
  txc_hdr_type  hdr;
  word          len;              /* Number of bits in message */
  boolean       req;              /* True if access request */
  byte          seq_num;          /* Max number of access probe sequences */
  byte          pagech;           /* Current Paging Channel */
  word          base_id;          /* Base station identification */
  word          pilot_pn;         /* Pilot PN sequence offset index */
  byte          pam_sz;           /* Access Channel preamble length */
  byte          max_cap_sz;       /* Max Access Channel msg capsule size */
  byte          bkoff;            /* Probe sequence backoff range */
  byte          probe_bkoff;      /* Probe backoff range */
  byte          acc_tmo;          /* Acknowledgement timeout */
  byte          init_pwr;         /* Initial power offset for access */
  byte          nom_pwr;          /* Nominal transmit power offset */
  byte          pwr_step;         /* Power increment */
  byte          num_step;         /* Number of access probes */
  byte          acc_chan;         /* Number of access channels */
  boolean       use_chan;         /* Test feature to use specific acc chan */
  word          rn;               /* Random number of chips backoff - ignored now */
  dword         p;                /* Persistence probability */
  byte          *msg_ptr;         /* Access Channel message to send */
  word          sigs;             /* Signal to set when access attempt is
                                     done */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal when access
                                     attempt is done */
} txc_acc_cmd_type;


typedef struct {                  /* TXC_REACH_F */
  txc_hdr_type  hdr;
  word          len;              /* Number of bits in message */
  boolean       req;              /* True if access request */
  byte          seq_num;          /* Max number of access probe sequences */
  byte          fccch;            /* Current Paging Channel */
  word          base_id;          /* Base station identification */
  word          pilot_pn;         /* Pilot PN sequence offset index */
  byte          bkoff;            /* Probe sequence backoff range */
  byte          probe_bkoff;      /* Probe backoff range */
  byte          acc_tmo;          /* Acknowledgement timeout */
  byte          init_pwr;         /* Initial power offset for access */
  byte          nom_pwr;          /* Nominal transmit power offset */
  byte          pwr_step;         /* Power increment */
  byte          num_step;         /* Number of access probes */
  byte          num_each_ba;        /* Number of enhanced access channels */

  txc_each_rate_type   rate;

  boolean       preamble_enabled;
  dword         p;                /* Persistence probability */
  byte          slot;
  byte          slot_offset1;
  byte          slot_offset2;
  byte          preamble_num_frac;
  byte          preamble_frac_duration;
  byte          preamble_off_duration;
  byte          preamble_add_duration;
  byte rlgain_common_pilot;   /* Gain adjustment of EACH or RCCCH relative to reverse pilot */
  byte ic_thresh;             /* interference correction threshhold */
  byte ic_max;                /* maximum interference correction that can be applied */
  byte          *msg_ptr;         /* Access Channel message to send */
  word          sigs;             /* Signal to set when access attempt is
                                     done */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal when access
                                     attempt is done */
} txc_reach_cmd_type;


typedef struct {                  /* TXC_ACC_STOP_F  */
  txc_hdr_type  hdr;
  word          sigs;             /* Signal to set when access attempt is
                                     done */
  rex_tcb_type  *tcb_ptr;         /* Pointer to task to signal when access
                                     attempt is done */
} txc_stop_cmd_type;

typedef struct {                  /* TXC_ON_TX_F */
  txc_hdr_type   hdr;
} txc_on_cmd_type;

typedef struct {                  /* TXC_OFF_TX_F */
  txc_hdr_type   hdr;
} txc_off_cmd_type;

typedef struct {                  /* TXC_NULLS_F */
  txc_hdr_type   hdr;
} txc_nulls_cmd_type;


typedef struct {
   boolean change_pri;
   word pri;
   boolean change_sec;
   word sec;
   boolean connect_so;             /* Indicates if this SO can be connected with the vocoder
                                      1 if we are in CONV state already; 0 otherwise */
  struct
  {
    boolean included;               /* Indicates if channel is included */
    cai_radio_config_type rc;       /* RC of channel */
  }
  rev_fch, rev_dcch;

  uint8 num_of_so_recs;

  struct
  {
    uint16  so;                 // SO for this connection
    boolean connect_so;         // Indicates if this SO can be connected with the vocoder
                                // 1 if we are in CONV state already; 0 otherwise
    byte    sr_id;
    byte    traffic_type;       // CAI_PRI_TRAF or CAI_SEC_TRAF
    word    phy_channel;        // combined Bit mask of physical channels associated with SO
  } so_cfgs[MAX_NO_OF_SO_RECS];

  word signaling_phy_channel;  //Bit mask of signaling physical channel
} txc_scr_info_type;

typedef struct {                  /* TXC_SO_F */
  txc_hdr_type   hdr;
  qword action_time;              /* Action time when the new params take effect */
  txc_scr_info_type scr_info;     /* The new service config info */
  byte transaction_num;           /* transaction number for this MC-TXC transaction*/
} txc_so_cmd_type;

typedef struct {                  /* TXC_RDR_F */
  txc_hdr_type hdr;
  byte rdr_limit;                 /* Reverse data rate limit */
  byte rdr_ena_thresh;            /* Reverse data rate limit enable
                                     threshold */
  byte rdr_dis_thresh;            /* Reverse data rate limit disable
                                     threshold */
} txc_rdr_cmd_type;

typedef struct {                  /* TXC_FRAME_OFFSET_F */
  txc_hdr_type hdr;
  byte frame_offset;              /* Traffic Channel frame offset */
  qword action_frame;             /* Frame when frame offset takes effect */
} txc_frame_offset_cmd_type;


typedef struct {
  txc_hdr_type hdr;
  qword action_time;
  byte transaction_num; /* Transaction num of this command */
  byte pwr_ctrl_step; /* power control step size */

  boolean rpc_incl; /* Indicates if RPC records are present */
  byte num_of_rpc_records; /* Number of RPC records */
  caii_rpc_type rpc_records[CAI_MAX_NUM_RPC_RECS]; /* RPC records */
} txc_pwr_ctrl_cmd_type;

typedef struct {                  /* TXC_CONNECT_SO_F */
  txc_hdr_type   hdr;
  byte   sr_id;                /* so service reference ID */
} txc_conn_so_cmd_type;

typedef struct {                  /* TXC_RELEASE_SO_F */
  txc_hdr_type   hdr;
  byte   sr_id;                /* so service reference ID */
} txc_release_so_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  byte transaction_num; /* Transaction num to validate the trans */
  qword action_time; /* Action time of this transaction */

  boolean hho_ind; /* HHO Indication */
  boolean rif_ind; /* Return on Failure Indication */

  boolean extra_params; /* Extra params indication */
  byte frame_offset; /* Valid only if extra_params is 1, Change FO only if different */
  byte nom_pwr; /* Valid only if extra_params is 1 */
  byte num_preamble; /* Valid only if extra_params is 1 */
  boolean lcm_change_ind; /* Valid only if extra_params is 1 */
  qword lcm;
  boolean rpc_incl;  /* Valid only if extra_params is 1 */
  int8 rlgain_traffic_pilot; /* Valid only if rpc_incl is 1 */
  boolean default_rlag; /* Valid only if rpc_incl is 1 */

  boolean pwr_ctrl_step_incl;  /* Pwr control step size is included in this cmd */
  byte pwr_ctrl_step;  /* pwr ctrl step size valid only if above flag is 1 */

  boolean scr_info_incl;  /* SCR info incl indicator */
  txc_scr_info_type scr_info;  /* SCR info valid only if above is true */

} txc_ho_cmd_type;

typedef struct {
  txc_hdr_type hdr;
} txc_hho_ok_cmd_type;

typedef struct {
  txc_hdr_type hdr;
} txc_rel_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  boolean frame_offset_incl; /* FO included indicator */
  byte frame_offset;  /* Frame offset , valid only if above is TRUE */
  boolean pwr_ctrl_step_incl;  /* Pwr control step size is included in this cmd */
  byte pwr_ctrl_step;  /* pwr ctrl step size valid only if above flag is 1 */
  boolean scr_incl; /* SCR included indicator */
  txc_scr_info_type scr_info; /* SCR info valid only if above is true*/
  boolean lcm_change_ind; /* LCM change indicator */
  qword lcm;  /* Valid only if above is TRUE */
  boolean rev_fch_gating_on; /* Indicates if 8th rate gating is on
                                     or off on R_FCH */
  byte p_rev_in_use; /* Needed for frame formatting */
} txc_returned_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  enc_gating_mode_type gating_mode;
  word gating_pattern;
  byte num_frames_gating_on;
  byte num_frames_gating_off;
} txc_pilot_gating_cmd_type;

typedef struct {
  txc_hdr_type hdr;
  word          rn;                /* Random number of chips backoff */
  boolean       hold_enc_clk_on;   /* Hold encoder clock on, e.g., enhance call setup */

} txc_jump_cmd_type;               /* TXC_JUMP_F - Jump to Hyperspace */

typedef struct {                   /* TXC_MVS_ENABLE_STATUS_F */
  txc_hdr_type             hdr;
  mc_mvs_enable_enum_type  status;
    /* Status of mvs_enable() command (vocoder driver) */
} txc_mvs_enable_cmd_type;

typedef struct {              /* TXC_MVS_STANDBY_STATUS_F */
  txc_hdr_type                hdr;
  mc_mvs_standby_enum_type    status;
    /* Status of mvs_standby() command (vocoder driver) */
} txc_mvs_standby_cmd_type;

/* Type for sending SCR information to TXC to bypass scheduling SCR
 * transaction. This cmd will be sent by CP for cases where 
 * TX has been disabled and re-enabled during traffic call */
typedef struct { 
  txc_hdr_type hdr;
  mctrans_action_type action_type; /* Action for which this cmd is being sent */
  mctrans_txc_scr_action_type scr; /* SCR data within the cmd */
} txc_scr_params_cmd_type;

  typedef struct {              /* TXC_TA_START_F */
  txc_hdr_type                  hdr;
  srch_ta_rsn_type              ta_type;
    /* Tune away type : QTA or LTA */
} txc_ta_start_cmd_type;

  typedef struct {              /* TXC_TA_COMP_F */
  txc_hdr_type                  hdr;
  srch_ta_rsn_type              ta_type;
    /* Tune away type : QTA or LTA */
} txc_ta_comp_cmd_type;

typedef union {
  txc_hdr_type       hdr;          /* Generic header */
  txc_cdma_cmd_type  cdma;         /* CDMA init */
  txc_pc_cmd_type    pc;           /* page channel */
  txc_tt_cmd_type    tt;           /* Time transfer*/
  txc_tc_cmd_type    tc;           /* traffic channel */
  txc_exit_cmd_type  exit;         /* exit cdma  */
  txc_msg_cmd_type   msg;          /* message to transmit */
  txc_acc_cmd_type   acc;          /* access attempt */
  txc_stop_cmd_type  stop;         /* stop access attempt and TX clock */
  txc_on_cmd_type    on;           /* start transmitting */
  txc_off_cmd_type   off;          /* stop transmitting */
  txc_nulls_cmd_type nulls;        /* transmit null traffic data */
  txc_so_cmd_type    so;           /* switch service options */
  txc_rdr_cmd_type   rdr;          /* reverse data rate limit parameters */
  txc_frame_offset_cmd_type fo;    /* Set frame offset                   */
  txc_pwr_ctrl_cmd_type pwr_ctrl;        /* Change Power Control Parameters */
  txc_conn_so_cmd_type conn_so;    /* Connect SO with vocoder if necessary */
  txc_release_so_cmd_type  release_so; /* release service option */
  txc_ho_cmd_type ho;              /* Ho command */
  txc_hho_ok_cmd_type hho_ok;      /* HHO Okay Command*/
  txc_returned_cmd_type ret;       /* HHO returned command */
  txc_rel_cmd_type      rel;       /* Equivalent to sending a SO cmd with null SO */
  txc_ftm_sch_cmd_type  ftm_sch;   /* FTM turn on SCH command */
  txc_ftm_loopback_cmd_type loopback; /* FTM loopback command */
  txc_ftm_rcp_cmd_type  rcp;
  txc_reach_cmd_type   reach;          /* access attempt */
  txc_jump_cmd_type jump;          /* TXC_JUMP_F - Jump to Hyperspace */
  txc_mvs_enable_cmd_type    mvs_enable; /* MC reports status of mvs_enable */
  txc_mvs_standby_cmd_type   mvs_standby; /* MC reports status of mvs_standby */
  txc_scr_params_cmd_type    scr_params;  /* SCR params sent to TXC directly */
  txc_ta_start_cmd_type      ta_start;
  txc_ta_comp_cmd_type       ta_comp;

} txc_cmd_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TXC_SUSPEND_TX

DESCRIPTION
  This functions enables/Disables the Transmitter and enables/disables the
  transmission of Access probes.

DEPENDENCIES
  Depends on the current state of the TX task.

RETURN VALUE
  None.

SIDE EFFECTS
  This function is called when the T72 fade timer expires to temporarily
  enable/disable the transmission of the Access Probes.

===========================================================================*/
void txc_suspend_tx (boolean);    /* Enable/Disable probe transmission */

/*===========================================================================

FUNCTION TXC_POWERUP_INIT

DESCRIPTION
  Initializes timers and queues for CDMA Transmit task processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_powerup_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_SUBTASK

DESCRIPTION
  Transmit subtask to process CDMA.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void txc_subtask( void );


/* <EJECT> */
/*===========================================================================

FUNCTION TXC_REGISTER_ACTION_TIME

DESCRIPTION
  This function is called by MC, upon receiving (E)HDM, to enable
  MCC_TXC_FRAME_SIGs for MC's HO action time (within the frame before
  System Action Time) and for RF tune time (within the frame after the
  System Action Time).

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_register_action_time
(
  boolean use_time,
  word action_time,
  boolean enable_second_frm_sig,
  boolean stop_tx_flag
);

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_SET_FRAME_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void txc_set_frame_offset( byte new_frame_offset );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_GET_TIME_IN_1_25MS_UNIT

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern dword txc_get_time_in_1_25ms_unit( void );

/*===========================================================================

FUNCTION TXC_OK_TO_SLEEP

DESCRIPTION
  This function returns true if MC can begin sleep.  This mechanism is in
  place to delay shutdown for a period after an access attempt to prevent
  the need to jump to hyperspace for another attempt that occurs soon
  thereafter.

DEPENDENCIES

RETURN VALUE
  True if it is okay to sleep.  False if sleep is to be delayed.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean txc_ok_to_sleep( void );

#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION rpc_reach_rate_word_satisfies_gain_limit

DESCRIPTION

  Determines if the passed rate_word would use more gain than allowed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if rate_word satisfies the gain limit.
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean rpc_reach_rate_word_satisfies_gain_limit
(
  txc_each_rate_type rate_word,
  int16 rlgain_common_pilot
);


/*===========================================================================

FUNCTION RLGC_GET_NOMINAL_RCCH_ATTRIBUTE_GAIN

DESCRIPTION
  The index must be between 0 and TXC_EACH_RATE_38400_5_V, or else it will return
  the value at index 0.

DEPENDENCIES

RETURN VALUE
 Return EACH Attribute Gains from the rpc_nominal_RCCH_attribute_gain_table.

SIDE EFFECTS
  None.
===========================================================================*/
int16 rlgc_get_nominal_rcch_attribute_gain(int16 index);
#endif

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION TXC_CONFIG_TEST_ACK_OR_NAK

DESCRIPTION
  API for CP task to use when configuring ACK or NAK transmissions for
  SO74 (Markov) and SO75 (Loopback).

  Parameters:
  channel: specifies which R-ACKCH this api is meant for.
  ack_or_nak: Bit mask signifying the test configuration
              for the specified R-ACKCH

  0x0 = Normal Operation
  0z1 = Send Ack every frame
  0x2 = Send Nak every frame

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void txc_config_test_ack_or_nak( txc_rackch_type channel, uint8 ack_or_nak );
#endif /* FEATURE_IS2000_1X_ADV */


/*===========================================================================

FUNCTION TXC_CMD

DESCRIPTION
  This is how the outside world interfaces with the tx task.  Call this
  procedure with the address of the txc_cmd_type and the command will be
  placed on the txc_cmd_q

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void txc_cmd
(
  txc_cmd_type *cmd_ptr
    /* The command to queue up */
);

#endif /* TXCMC_I_H */

