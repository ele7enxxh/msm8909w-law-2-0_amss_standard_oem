#ifndef MCTRANS_I_H
#define MCTRANS_I_H
/*===========================================================================

          MAIN CONTROL TRANSACTION MANAGER MANAGER

GENERAL DESCRIPTION
  This task manages the transaction (Commands with action times) sent to the lower
  layers.  The lower layers always "confirm" a transaction with the functions
  in this module before going ahead with it. This module also makes sure if one task
  is given an "okay" for a transaction, that is true for all other tasks which hold
  the same transaction. A "confirm" can return "FALSE" if this transaction is either
  no longer valid or has been overidden by a different transaction.

DESCRIPTION
   This module has data structures to keep track of different transaction numbers
   and the transaction types they correspond to.

  Copyright (c) 1999 - 2016 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mctrans.h_v   1.15   02 Oct 2002 16:48:14   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mctrans_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/03/16   ppr     Get and update APIs for are_int_tickers_reg status
03/09/16   ppr     1x2GTA Feature changes
09/13/13   pap     Adding changes for the m1x_stmr module to be integrated.
03/29/13   ppr     Removed MCTRANS_MAX_TRANSACTIONS declaration
10/16/12   ppr     Fix for trans pending msg memory leakage
09/15/12   ppr     FR#2370 - 1x memory optimization changes
06/28/12   ssh     Cleared the MCTRANS queue when it's not used.
02/21/12   ppr     Feature Cleanup
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
07/22/10   ag      Removed RCPM from pending_action_msg structure.
09/28/09   ag      1X Advanced Signaling Support - Phase 2
08/02/09   jtm     Lint fixes.
07/01/09   ag      Added/Modified 1X adavnced related definitions
03/26/09   pk      1X Advanced Feature - added new transaction type for RCPM
03/20/09   jtm     Renamed mctrans_v.h to mctrans.h since au/api/ version was
                   removed.
06/19/08   ag      Fixed compiler warning.
05/14/08   ag      Watermarking for trans_queue.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
05/22/06   fc      Mainlined Rel A interface in mctrans_action_packet_type.
04/30/06   fc      Added rif_ind to mctrans_rxc_hho_begin_action_type.
04/24/06   fc      Fixed Rel A featurization.
03/14/06   fh      Added AES support
07/05/05   sb      Added MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/08/05   sb      Added a comment.
01/14/05   sb      Updated Rel D DV interfaces.
01/10/05   va      Merged from REL_B_C archive
11/22/04   sb      Miscellaneous fixes for Rel C DV Handoffs.
11/05/04   va      Merged following from REL_B_C archive:
10/20/04   an        Replaced inclusion of srch.h with srchmc.h
09/30/04   yll       Added the rc, sch_rate and code_type in F-SCH Action
                     Packet to Search.sch
10/27/04   va      Added ICCH update rate in CPCCH info interface
10/21/04   va      Interface changes for R-DV RLGC interface
09/29/04   va      Interface change for PLT
09/17/04   va      Fixed compilation error
09/16/04   fc      Added support for reverse DV.
09/16/04   fc      PLT changes for reverse DV.
08/10/04   fc      Added txc_r_dv_params to mctrans_action_packet_type.
08/09/04   fc      Service negotiation changes for Release C and D.
08/02/04   bkm     Added declaration for mctrans_despatch_ordered_transaction.
07/30/04   ht      Added TDSO support for Rel C DV channels
07/29/04   bkm     Changes for PDCCH PLT support.
06/21/04   va      Changes for HO/CFS/GPS clean up
06/15/04   bkm     Changes to r_dv_params types to allow reuse
                   with Power Control and Rate Change messages.
06/15/04   pg      Removed some unnecessary comments.
06/08/04   bkm     Fixed typos in feature names.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/24/04   fc      Fixed compilation error for
                     mctrans_function_pointer_initializer_type.
05/28/04   bkm/ll  Misc Rel D DV interface updates.
05/25/04   bkm     Pulled for_ackch, rcch and gch structs out into typedef structs.
05/25/04   az      Increased the array size by 1 since there are now
                   MCTRANS_MAX_ACTION + 1 number of actions that servers
                   register using mctrans_function_pointer_initializer_type
04/06/04   bkm     DV CP/MUX interface updates.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
02/06/03   az      Added/Modified structures and interfaces for new
                   transaction engine.
01/20/04   bkm     Misc DV interface updates.
01/13/04   bkm     Removed puncture_pilot from F_DV_PARAMS action packet.
01/05/04   bkm     Misc updates for DV transactions.
05/29/03   bkm     Changed return type of mctrans_record_pending_msg to void.
10/02/02   az      Mainlined FEATURE_IS2000
06/28/02   lh      Reverse MuxPUD 5 support.
05/31/02   lcc     Added support for control hold action for RXC.
05/31/02   va      Added action payload for SRCH_FFPC_ACTION &
                   SRCH_ASET_ACTION
1/21/02    ph      Sync up SCR interface with MUX, revert them to REL_A featurization.
12/13/01   va      Merged in from SVD branch
  11/29/01 ph      Made interface changes exclusive to SVD builds.
  11/29/01 ph      Restoring old SCR interface for non-rel A builds.
  08/23/01 ph      SVD updates for SCR transaction from MC to mux.
11/30/01   fc      Added MCTRANS_R_SCH_UPDATE transaction type.
                   Added mctrans_r_sch_operation_type.
                   Changed mctrans_txc_r_sch_action_type.
11/27/01   lcc     Modified mctrans_fpc_olpc_param_type to support setting min/max
                   set points to the current set points.
10/26/01   jrp     Added forward and reverse SCH0_MUX_OPTION to SCR message to driver.
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/05/01   lcc     Added MCTRANS_R_SCH0_EXTEND transaction type.
06/26/01   va      Merged from MSM_CP.05.00.81
           ht      Changed feature label to FEATURE_GPS_MODE around
                   function mctrans_is_okay_to_tune_away().
           jq      Wrapped some code in FEATURE_IS95B_MAHHO.
05/17/01   lh      Merged the following from common archive.
  05/03/01   jq      Added mctrans_is_trans_pending ().
  05/02/01   fc      Added suport for T_ADD_ABORT.
  04/10/01   jq      Added transcations to support P2/Control Hold
05/10/01   lh      Added interface change for F-SCH to support 307K.
04/17/01   va      Added pilot rec type in the CF NSET interface.
03/02/01   va      Merged the following from common archive:
  02/15/01 va      Added prototype for mctrans_is_okay_to_tune_away for GPS ONE
                   support.
  02/12/01 lcc     Added member trans_time to mctrans_info_type.
02/27/01   va      Added pilot rec types in CFS transaction to
                   SRCH, Added p_rev in use in the HO trns to RXC and TXC.
02/12/01   lcc     Added member trans_time to mctrans_info_type.
08/23/00   lcc     Removed some obsolete comments.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
07/21/00   fc      Added trans_type to mctrans_rpt_type.
                   Used enumerated types for FPC mode, SCH rate and coding type.
07/17/00   va      Added the CFS transaction types and new actions
                   to go with it.
06/22/00   bgc     Moved mctrans_cmd_type to its own header from mctrans.h
                   to simplify a circular include problem.
06/19/00   lcc     1. Changed byte to uint8 to be consistent.
                   2. Made SCH init_setpt optional in mctrans_ffpc_action_type.
06/15/00   lcc     Changed HO action interface to TXC such that preamble unit
                   is specified.  Also allows nom_pwr to be optional since HDM does
                   not include that field.
06/12/00   lcc     1. Update interfaces to FFPC and RLGC.
                   2. Rearranged some type definitions to circumvent a problem with
                      circular dependency between srch.h and mctrans.h.
                   3. Added function prototype for mctrans_void_trans.
06/02/00   lcc     1. Corrected some comments and some structure definition (not used
                      yet).
                   2. Obsoleted cai_msg in mctrans_info_type which is not needed.
                   3. Removed HO message as a member of
05/16/00   lcc     Added data structures to handle Ho and SCR transactions.
                   Changed data member "code_type" of SCH action to "turbo_code"
                   to better reflect it's meaning.
04/24/00   lcc     Added functions prototypes, data structures and types to support
                   client-server transaction engine.
12/20/99   va      Added new interface for getting "transaction type" from a
                   transaction num
11/15/99   va      Initial revision of the file.
                   Created for the IS2000 Changes. This file implements the
                   MC transaction module.
===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "cai.h"
#include "caii.h"
#include "caii_i.h"
#include "cmd.h"
#include "queue.h"
#include "mctrans_cmd.h"
#include "srchmc.h"
#include "srchmc_v.h"
#include "srchmc_i.h"
#include "mc_v.h"
#include "m1x_stmr_i.h"

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/

/* The structures below to be used for rel A accomodates concurrent services */
/* Currently, we'll define a max of 2 non-signaling-only SO connections */
#define MAX_NO_OF_SO_RECS    CAI_MAX_NUM_CONNECTIONS

typedef struct
{
  boolean included;               /* Indicates if channel is included */
  cai_radio_config_type rc;       /* RC of channel */
} chan_cfg_type;

typedef struct
{
  uint8     target_fer;
  struct
  {
    boolean set_to_current;   // Indicates if value should be set to current setpoint
    uint8   value;            // Value if set_to_current is not TRUE
  } min_setpt, max_setpt;
} mctrans_fpc_olpc_param_type;

/* Enumeration for the different types of transactions */
typedef enum
{
  MCTRANS_HO=0,
  MCTRANS_SCR,
  MCTRANS_FPC_SUBCHAN_GAIN,
  MCTRANS_PWR_CTRL_MSG,
  MCTRANS_F_SCH0_START,     // SCH1 not supported, no need to include here
  MCTRANS_F_SCH0_STOP,
  MCTRANS_R_SCH0_START,
  MCTRANS_R_SCH0_STOP,
  MCTRANS_R_SCH0_EXTEND,    // Used for extending R-SCH
  MCTRANS_R_SCH_UPDATE,    // Used for updating DTX duration and T_ADD_ABORT
  MCTRANS_CFS_NSET,
  MCTRANS_CFS_CTRL,
  MCTRANS_EXTENDED_RELEASE, // trans  for release of subset of phy chanels
  MCTRANS_ACTIVE,          // trans from control hold to active
  MCTRANS_LONG_CODE_MASK,  /* transaction to change the long code mask */
  MCTRANS_DV_SWITCH,       // Rel C DV Switch trans
  MCTRANS_DV_RATE_CHANGE,  // Rel C DV Rate Change Msg trans
  MCTRANS_DV_TDSO,         // Rel C/D DV TDSO SOCM trans
  MCTRANS_TX_ON_OFF,       // TXN for turning TX on or off
  MCTRANS_HHO_BEGIN,       // Txn for beginning good frame srch for HHO
  MCTRANS_RETURN,          // Txn for "return" after a failed HHO
  MCTRANS_CFS_GPS_PREP,    // Txn for CFS GPS Prepartion
  #ifdef FEATURE_IS2000_1X_ADV
  MCTRANS_RCP,             // Txn for RC parameters message
  #endif /* FEATURE_IS2000_1X_ADV */
  MCTRANS_COUNT
} mctrans_transaction_type;

/* Enumeration for the different modes of transactions
*/
typedef enum
{
  MCTRANS_MODE_EXPLICIT,
  MCTRANS_MODE_IMMEDIATE,
  MCTRANS_MODE_UNDEFINED

} mctrans_trans_mode_type;

typedef enum
{
  MCTRANS_FFPC_START,       // Start outer loop, may come with parameters
  MCTRANS_FFPC_STOP,        // Stop outer loop, may come with parameters
  MCTRANS_FFPC_CONTINUE     // Continue with current FFPC operation
} mctrans_ffpc_operation_type;

typedef enum
{
  MCTRANS_RLGC_START,       // Start RLGC
  MCTRANS_RLGC_STOP,        // Stop RLGC
  MCTRANS_RLGC_CONTINUE     // Continue with current RLGC operation
} mctrans_rlgc_operation_type;

typedef enum
{
  MCTRANS_PREAMBLE_FRAME,   // Spceifies preamble units in frames
  MCTRANS_PREAMBLE_PCG      // Spceifies preamble units in PCGs
} mctrans_preamble_unit_type;

/*-------------------------------------------------------------------------*/
/*  Physical Channel Constants for use in rxc/txc_scr_action_type          */
/*-------------------------------------------------------------------------*/
#define MCTRANS_PHY_CHAN_FCH  0x01
#define MCTRANS_PHY_CHAN_DCCH 0x02
#define MCTRANS_PHY_CHAN_SCH0 0x04
#define MCTRANS_PHY_CHAN_SCH1 0x08
#define MCTRANS_PHY_CHAN_PDCH 0x10

typedef enum
{
  MCTRANS_R_SCH_START, // Start R-SCH
  MCTRANS_R_SCH_STOP,  // Stop R-SCH
  MCTRANS_R_SCH_DTX    // Change R-SCH DTX duration
} mctrans_r_sch_operation_type;

// Temporary definition.  To be updated
// Action information for r_sch action
typedef struct
{
  uint8                        sch_id;
  mctrans_r_sch_operation_type mode; // Indicates what to do with R-SCH
  cai_radio_config_type        rc;
  cai_sch_rate_type            sch_rate;
  boolean double_size_pdu;  // Indicates if MUX PDU (Type 3) is single or double sized
  cai_sch_coding_type turbo_code;
  uint8   walsh_id;
  uint8   rev_sch_dtx_duration;
  cai_mux_pdu_type_type mux_pdu_type; // 1,2, 3, 5
  uint8             num_ltu;          // Number of LTUs in an SDU
  uint16            ltu_len;          // Length of LTU in bits
} mctrans_txc_r_sch_action_type;

typedef struct
{
  boolean use_t_add_abort;
    /* Used for SRCH R-SCH action, indicates whether T_ADD_ABORT is enabled */
  uint8   t_mulchan;
    /* SCRM pilot strength reporting offset */
} mctrans_srch_r_sch_action_type;

/* This action packet is used to give reverse link gain parameters to TXC.
 * RC for PDCH and gain parameters for R-CQICH and R-ACKCH are added to
 * support DV. */
typedef struct
{
  struct
  {
    mctrans_rlgc_operation_type mode;
    cai_radio_config_type       rc;    /* The rc is only valid if the mode is "start" */
  } fch_rlgc_op, dcch_rlgc_op;

  struct
  {
    mctrans_rlgc_operation_type mode;
    cai_radio_config_type       rc;    /* The rc is only valid if the mode is "start" */
    cai_sch_coding_type sch_code_type;
    /* 0 - conv., 1 - turbo, included only when starting */
    cai_sch_rate_type sch_rate;
  } sch_rlgc_op;
  boolean use_default_rlag;

  struct
  {
    boolean included;
    uint8   value;
  } pwr_cntl_step;

  struct
  {
    boolean included;
    int8    value;
  } rlgain_traffic_pilot;

  struct
  {
    boolean included;
    uint8   value;
  } rlgain_sch_pilot[CAI_MAX_NUM_REV_SCH];

  /* With RATCHGM, num_of_rpc_records is 0 or 1 */
  /* If there is 1 record it is type 3, see below. */
  uint8         num_of_rpc_records;                 /* Number of RPC records */
  caii_rpc_type rpc_records[CAI_MAX_NUM_RPC_RECS];  /* RPC records */
} mctrans_rlgc_action_type;

/*
** This action to TXC instructs TXC to enable or disable the transmitter.
** This action to SRCH is to inform that module of the transmitter state.
*/
typedef struct
{
  boolean tx_on; /* TRUE means on */
} mctrans_tx_on_action_type;


typedef struct
{
  dword  search_period;
    /* Time between successive searches on the CF, in millisecond unit */

  byte  band_class;
    /* Band class of the CF */

  word  cdma_freq;
    /* CDMA frequency for the CF */

  int1  diff_rx_pwr_thresh;
    /* Minimum difference in received power */

  byte  min_total_pilot_ec_io;
    /* Minimum total pilot Ec/Io */

  byte  sf_total_ec_thresh;
    /* Ec threshold for the Serving Frequency */

  byte  sf_total_ecio_thresh;
    /* Ec/Io threshold for the Serving frequency */

  byte  cf_t_add;
    /* Pilot detection threshold for th CF */

  byte  pilot_inc;
    /* Pilot Inc to be used on th CF after handoff */

  byte  cf_srch_win_n;
    /* Default search window size for the CF Search Set */

  byte  cf_srch_win_r;
    /* Search window size for the Remaining Set on the CF */

  boolean pilot_update;
    /* Indicates if a CF set update is included, the following fields make
       sense only if this is set to TRUE */

  word  cf_nset_cnt;
    /* Number of pilots to be searched */

  byte  cf_nghbr_srch_mode;
    /* Search mode for the CF Search Set */

  struct
  {
    int2 pilot;
      /* Pilot PN offset */

    byte srch_flag;
      /* Flag to incidate if the corresponding pilot is to be searched */

    byte srch_pri;
      /* Search priority */

    byte srch_win;
      /* Search window size */

    byte srch_offset_nghbr;
      /* Search Window offset */
    cai_pilot_rec_type pilot_rec_type;
      /* Pilot rec type */
    caii_add_pilot_rec_type pilot_rec;
    /* Pilot rec type specific fields*/

  }cf_nset[ CAI_CFSREQ_REC_MAX ];
} mctrans_cfs_nset_action_type;

// Action information for ffpc action
typedef struct
{
  struct
  {
    mctrans_ffpc_operation_type mode; // Indicates what to do with FFPC for the channel
    cai_radio_config_type       rc;   // Indicates RC to use, used for starting and continuing only
  }

  fch_ffpc_op, sch_ffpc_op, dcch_ffpc_op;

  struct
  {
    boolean           included;
    cai_fpc_mode_type value;
  } fpc_mode;
  struct
  {
    boolean   included;
    uint8     value;
  } fpc_subchan_gain;
  struct
  {
    boolean included;
    uint8   value;                  // 0-fch, 1-DCCH
  } fpc_pri_chan;
  struct
  {
    boolean   included;
    mctrans_fpc_olpc_param_type param;
  } fch_olpc, dcch_olpc;
  struct    // Threshold for FCH-DCCH
  {
    boolean included;
    uint8   value;
  } fpc_setpt_thresh;
  struct
  {
    boolean   included;               // Indicates if any of the fields below is included
    boolean   fpc_sch_init_setpt_op;  // 0 = absolute, 1 = relative to fpc_pri_chan
    struct
    {
      boolean included;
      uint8   value;
    } fpc_sec_chan, fpc_thresh_sch;
    struct
    {
      boolean   included;
      struct
      {
        boolean included;
        uint8    value;
      } init_setpt;
      mctrans_fpc_olpc_param_type param;
    } sch_olpc[CAI_MAX_NUM_FOR_SCH];
  } sch;
} mctrans_ffpc_action_type;


// Action information for f_sch action to RXC
typedef struct
{
  uint8             sch_id;
  boolean           start;            // Indicate starting or stopping SCH
  boolean           for_sch_fer_rep;  // Indicates if FER report is to be generated at end of burst
  // Note: following fields are not used if stopping
  cai_radio_config_type rc;
  cai_sch_rate_type sch_rate;         // 1X, 2X, 4X, 8X, 16X
  boolean           double_size_pdu;  // Indicates if MUX PDU (Type 3) is single or double sized
  cai_sch_coding_type code_type;      // 0 - convolutional, 1 - turbo
  cai_mux_pdu_type_type mux_pdu_type; // 1,2, 3, 5
  uint8             num_ltu;          // Number of LTUs in an SDU
  uint16            ltu_len;          // Length of LTU in bits
} mctrans_rxc_f_sch_action_type;

// Action information for f_sch action to SRCH
typedef struct
{
  // For each member of the new SCH
  struct
  {
    uint8 num_pilots;
    // 0 if SCH not active

    cai_radio_config_type rc;
    cai_sch_rate_type     sch_rate;     // 1X, 2X, 4X, 8X, 16X
    cai_sch_coding_type   code_type;    // 0 - convolutional, 1 - turbo

    // Pilot offset indices and walsh channels for active set members
    struct
    {
      uint16  pilot_pn;
      // Pilot PN index of this Active Set member

      uint16  walsh_chn;
      // Walsh channel to be used on the traffic channel with the pilot

      uint8   qof_mask_id;
      // QOF mask ID (0-3), where 0 gives true walsh fns)

#ifdef FEATURE_IS2000_REL_A
      cai_pilot_rec_type pilot_rec_type; /* Pilot rec type */
      caii_add_pilot_rec_type pilot_rec; /* Pilot rec spec fields */
#endif

    } pilots[CAI_N6M];
  } sch[CAI_MAX_NUM_FOR_SCH];  // maximum of 2 SCH in IS95C. TIGER2 supports only 1
} mctrans_srch_f_sch_action_type;

/* Action information for HO action to RXC */
typedef struct
{
  boolean hho_ind;    /* Indicates if this is a HHO (To wait for sync points) */
  boolean rif_ind;    /* Indicates if this is Return on Failure HHO */
  boolean reset_fpc;  /* Indicates if FPC is to be reset after HO */
  struct
  {
    boolean included;
    uint8   value;
  } rev_pwr_cntl_delay;    /* Rev power control delay value */
#ifdef FEATURE_IS2000_REL_A
  byte p_rev_in_use;  /* P REV info needed for frame formatting */
#endif  /* Do we still need this? */

} mctrans_rxc_ho_action_type;

/* Action information for HO action to TXC */
typedef struct
{
  boolean hho_ind;      /* HHO Indication */
  boolean rif_ind;      /* Return on Failure Indication */
  uint8   frame_offset; /* Frame offset, change only if different */
  struct
  {
    boolean included;
    uint8   value;
  } nom_pwr;            /* nom_pwr is not included in HDM */

  struct
  {
    mctrans_preamble_unit_type  unit;
    uint8                       value;
  } num_preamble;

#ifdef FEATURE_IS2000_REL_A
  byte p_rev_in_use;  /* P REV info needed for frame formatting */
#endif  /* Do we still need this */
} mctrans_txc_ho_action_type;

/* Action information for long code action to TXC */
typedef struct
{
  qword long_code_mask;
} mctrans_txc_lcm_action_type;

/* Action information for long code action to SRCH */
typedef struct
{
  qword long_code_mask;
} mctrans_srch_lcm_action_type;

/* Action information for SCR action to RXC */
typedef struct
{

#ifndef FEATURE_IS2000_REL_A
  struct
  {
    boolean included;           /* Indicates if primary SO is included */
    uint16  value;              /* Primary SO to switch to, if included */
  } pri_so;

  struct
  {
    boolean included;           /* Indicates if secondary SO is included */
    uint16  value;              /* Secondary SO to switch to, if included */
  } sec_so;
#else
  uint8 num_of_so_recs;

  struct
  {
    uint16  so;                 /* SO for this connection */
    uint8   sr_id;
    uint8   traffic_type;      /* CAI_PRI_TRAF or CAI_SEC_TRAF */
    #ifdef FEATURE_IS2000_REL_A_AES
    uint8   ui_encrypt_mode;
    #endif /* FEATUER_IS2000_REL_A_AES */
  } so_cfgs[MAX_NO_OF_SO_RECS];
#endif // FEATURE_IS2000_REL_A

  struct
  {
    boolean included;           /* Indicates if DCCH/FCH is included */
    cai_radio_config_type rc;
  }

  fch, dcch;                    /* Indicates if channel is included */

  /* Rev pwr cntl delay value, This can change if the RC class changes. */
  struct
  {
    boolean included;           /* Indicates if rev pwr cntl delay is included */
    uint8 value;                /* Rev pwr cntl delay value */
  } rev_pwr_cntl_delay;

  /* F-SCH 0 MUX option */
  struct
  {
    boolean included; /* Indicates if f-sch0 mux option is included */
    uint16 value;     /* The f-sch0 mux option value */
  } for_sch0_mux_option;

} mctrans_rxc_scr_action_type;

/* Action information for SCR action to TXC */
typedef struct
{
#ifndef FEATURE_IS2000_REL_A
  struct
  {
    boolean included;           /* Indicates if primary SO is included */
    uint16  value;              /* Primary SO to switch to, if included */
    boolean connect_so;         /* Indicates if this SO can be connected */
                                /* 1 if we are in CONV state, else 0 */
  } pri_so;

  /* Secondary SO not used now. */
  struct
  {
    boolean included;           /* Indicates if secondary SO is included */
    uint16  value;              /* Secondary SO to switch to, if included */
    boolean connect_so;         /* Indicates if this SO can be connected
                                 * with the vocoder.  1 if we are in CONV
                                 * state already; 0 otherwise */
  } sec_so;
#else
  uint8 num_of_so_recs;

  struct
  {
    uint16  so;                 /* SO for this connection */
    boolean connect_so;         /* Indicates if this SO can be connected with
                                 * the vocoder.  1 if we are in CONV state
                                 * already; 0 otherwise */
    uint8    sr_id;
    uint8    traffic_type;      /* CAI_PRI_TRAF or CAI_SEC_TRAF */
    uint16   phy_channel;       /* combined Bit mask of phy chans associated
                                 * with SO */

#ifdef FEATURE_IS2000_REL_A_AES
    uint8 ui_encrypt_mode;
#endif /* FEATURE_IS2000_REL_A_AES */

  } so_cfgs[MAX_NO_OF_SO_RECS];

  uint16 signaling_phy_channel; /* Bit mask of signaling physical channel */

#endif /* FEATURE_IS2000_REL_A */

  struct
  {
    boolean included;
    cai_radio_config_type rc;
  }

  fch, dcch;                    /* Indicates if channel is included */

  /* R-SCH 0 MUX option */
  struct
  {
    boolean included;  /* Indicates if r-sch0 mux option is included */
    uint16 value;      /* The r-sch0 mux option value */
  } rev_sch0_mux_option;

} mctrans_txc_scr_action_type;

typedef struct
{
  boolean rev_fch_gating_on; /* Indicates if 8th rate gating should be on */
} mctrans_txc_gating_action_type;

typedef struct
{
  cai_pilot_gating_rate_type gating_rate;
} mctrans_txc_ctrl_hld_gating_action_type;

typedef struct
{
  cai_pilot_gating_rate_type gating_rate;
  uint8                      rev_pwr_control_delay;
} mctrans_rxc_ctrl_hld_gating_action_type;

typedef struct
{
  cai_fpc_pri_chan_type fpc_pri_chan;   /* This will be set to CPCCH (255)
                                         * when F-fundicated not assigned */
} mctrans_srch_ffpc_action_type;

typedef struct {
  srch_tc_aset_info_type  tc_aset;
} mctrans_srch_aset_action_type;

typedef union
{
  caii_gen_type           gen;
  caii_pwr_ctrl_type      pwr_ctrl;
  caii_escam_type         escam;
  caii_cfs_req_type cfs_req;
  caii_cfs_ctrl_type cfs_ctrl;
  caii_erm_type erm;  // Extended Release Msg
  caii_ram_type ram;  // Resource Allocation Msg

  caii_ftc_ord_type order;
#ifdef FEATURE_IS2000_REL_C
  caii_srv_con_type service_connect;
#endif /* FEATURE_IS2000_REL_C */
} mctrans_pending_action_msg_type;

/* Queues to manage transactions buffer pool
*/
typedef q_type   trans_q_type;


// Some data structures for the transaction report queue.
// Transaction report is sent when a transaction is completed.
typedef struct
{
  cmd_hdr_type             rpt_hdr;      // report header - includes q_link
  uint16                    trans_num;
  mctrans_transaction_type trans_type;
} mctrans_rpt_type;
extern q_type  mctrans_rpt_q;

#ifndef FEATURE_1X_CP_MEM_OPT
/* Transaction Free Queue */
extern trans_q_type trans_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

// This header file inclusion has to be placed here to resolve a circular
// dependency problem between srch.h and mctrans.h.
struct mctrans_action_packet_struct;

/* Action information for HO action to SRCH */
typedef struct
{
  boolean hho_ind;        /* Indicates if this is a HHO */
  boolean rif_ind;        /* Return-on-failure indicator */

  struct
  {
    boolean included;     /* Indicates if following parameters are included. */
    caii_ho_srch_type values;
  } srch_parms;
} mctrans_srch_ho_action_type;

/* Action Packet for TX ON action for TXC*/
typedef struct
{
  boolean tx_on; /* 1 means TX is to be turned ON, 0 means OFF */
} mctrans_txc_tx_on_off_action_type;

/* Action Packet for TX ON action for SRCH*/
typedef struct
{
  boolean tx_on; /* 1 means TX is to be turned ON, 0 means OFF */
} mctrans_srch_tx_on_off_action_type;

/* Action Packet for RXC HHO BEGIN action */
typedef struct
{
  boolean rif_ind;  /* Indicates if this is Return on Failure HHO */
  byte hho_timeout; /* Time before which RXC shd rx a good frame */
} mctrans_rxc_hho_begin_action_type;

/* Action Packet for RXC CFS/GPS Prep action */
typedef struct
{
  boolean is_gps; /* Indicates to RXC that this is for GPS */
} mctrans_rxc_cfs_gps_prep_action_type;

/* Action Packet for SRCH CFS Meas action */
typedef struct
{
  byte cfs_seq;
    /* CFS Sequence num */
} mctrans_srch_cfs_meas_action_type;

/* Action types for action packets for MCTRANS_RCP transaction */
#ifdef FEATURE_IS2000_1X_ADV
typedef cai_rcp_rxc_txc_type mctrans_rxc_rcp_action_type;
typedef cai_rcp_rxc_txc_type mctrans_txc_rcp_action_type;
typedef cai_rcp_srch_type mctrans_srch_rcp_action_type;
#endif /* FEATURE_IS2000_1X_ADV */

// Structure to hold any information related to an action
typedef struct mctrans_action_packet_struct
{
  mctrans_action_type action_type;
  union
  {
    mctrans_ffpc_action_type        ffpc;
    mctrans_rlgc_action_type        rlgc;
    mctrans_txc_ho_action_type      txc_ho;
    mctrans_rxc_ho_action_type      rxc_ho;
    mctrans_srch_ho_action_type     srch_ho;
    mctrans_rxc_scr_action_type     rxc_scr;
    mctrans_txc_scr_action_type     txc_scr;
    mctrans_rxc_f_sch_action_type   rxc_f_sch;
    mctrans_srch_f_sch_action_type  srch_f_sch;
    mctrans_txc_r_sch_action_type   txc_r_sch;
    mctrans_srch_r_sch_action_type  srch_r_sch;
    mctrans_cfs_nset_action_type    srch_cfs_nset;
    mctrans_txc_gating_action_type  txc_gating;
    mctrans_txc_ctrl_hld_gating_action_type txc_ctrl_hld_gating;
    mctrans_rxc_ctrl_hld_gating_action_type rxc_ctrl_hld_gating;
    mctrans_srch_aset_action_type srch_aset;
    mctrans_srch_ffpc_action_type srch_ffpc;
    mctrans_txc_lcm_action_type     txc_lcm;
    mctrans_srch_lcm_action_type    srch_lcm;
    mctrans_txc_tx_on_off_action_type txc_tx_on_off;
    mctrans_srch_tx_on_off_action_type srch_tx_on_off;
    mctrans_rxc_hho_begin_action_type rxc_hho_begin;
    mctrans_srch_cfs_meas_action_type srch_cfs_meas;
    mctrans_rxc_cfs_gps_prep_action_type cfs_gps_prep;

    #ifdef FEATURE_IS2000_1X_ADV
    mctrans_rxc_rcp_action_type       rxc_rcp;
    mctrans_txc_rcp_action_type       txc_rcp;
    mctrans_srch_rcp_action_type      srch_rcp;
    #endif /* FEATURE_IS2000_1X_ADV */

  } action;

} mctrans_action_packet_type;


/* Transaction object that stores all the details of the transaction
*/
typedef struct trans_info
{
  q_link_type           link;
    /* Q linking element

       This element has to be the first
       element in this data structure
       because of removal of self_ptr
       in q_link_type
     */

  uint16 trans_num;


  /* Data structure that holds the state of each active transaction
  */
  struct
  {
    /* Whether preprocessor has been executed
    */
    boolean preproc_done;
  mctrans_transaction_type  transaction_type;

    /* Time transaction to be executed
    */
    qword   trans_time;

    /*  Next server to execute actions for the transaction
    */
    mctrans_server_token_type  next_server;

    /*  Order of servers for ordered transactions
    */
    mctrans_server_token_type  server_order[MCTRANS_SERVER_COUNT];

    struct
    {
      mctrans_action_type actions[MCTRANS_MAX_ACTION_IN_TRANS];
      boolean  all_actions_done;

    } server[MCTRANS_SERVER_COUNT];

    void (* preprocessor)(struct trans_info * curr_trans);
    void (* postprocessor)(struct trans_info * curr_trans);
    void (* action_packet_gen)(struct trans_info * , mctrans_action_type,
                            mctrans_server_token_type, mctrans_action_packet_type*);
  } trans;

} mctrans_trans_info_type;


// This structure is used by transaction client (MC) to prepare transaction
// to be despatched.  The intent is that all static information related to
// a transaction is stored here which can be initialized with static initializer.
typedef struct
{
  mctrans_transaction_type  transaction_type;

  /* Whether transaction is Explicit or Immediate.
  */
  mctrans_trans_mode_type    trans_mode;

  void (*preprocessor)(mctrans_trans_info_type * curr_trans);
  void (*postprocessor)(mctrans_trans_info_type * curr_trans);
  void (*action_packet_gen)(mctrans_trans_info_type * , mctrans_action_type,
                            mctrans_server_token_type, mctrans_action_packet_type* );

} mctrans_despatch_info_type;


/* This stores the action type and associated action handlers
*/
typedef struct
{
  mctrans_action_type action_type;
  void (*action_processor)(mctrans_action_packet_type* action_packet);
} mctrans_action_handler_type;


typedef struct
{
  /* This function pointer is used to inform the servers that the action time
     for some transaction has arrived.
  */
  void (* action_notifier)(void);


  /* Custom interrupt registration function pointer
  */
  void (* interrupt_reg_func)(void (* interrupt_func)(mctrans_server_token_type, qword));


  /* During initializing of this array please make sure that all actions
     are initialized.
  */
  mctrans_action_handler_type action_handler[(uint32) MCTRANS_MAX_ACTION+1];

} mctrans_function_pointer_initializer_type;

/*===========================================================================

FUNCTION mctrans_trans_free_q_init

DESCRIPTION
  Initializes the free queue

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void mctrans_trans_free_q_init( void );

/*===========================================================================

FUNCTION mctrans_initialize

DESCRIPTION
  Performs necessary initialization for the transaction engine.  This function
  is called when first get onto traffic channel.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void mctrans_initialize ( void );

/*===========================================================================

FUNCTION mctrans_initialize_actions

DESCRIPTION
  Clear all actions for this transaction. Also initialize all_actions_done
  to FALSE

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void  mctrans_initialize_actions(mctrans_trans_info_type * trans_info);


/*===========================================================================

FUNCTION mctrans_confirm_transaction

DESCRIPTION
  Checks to see if its OK to go ahead and put the transaction in the active
  queue.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if transaction can be put in active queue.
  FALSE otherwise.

SIDE EFFECTS
===========================================================================*/

boolean mctrans_confirm_transaction(
    mctrans_trans_info_type * curr_active_trans);

/*===========================================================================

FUNCTION mctrans_get_transaction_type

DESCRIPTION
  This function returns the transaction type for a given transaction
  number.

DEPENDENCIES
  None

RETURN VALUE
  Transaction type for this trans num

SIDE EFFECTS
===========================================================================*/

extern mctrans_transaction_type mctrans_get_transaction_type(
  mctrans_trans_info_type * curr_trans );

/*===========================================================================

FUNCTION mctrans_despatch_transaction

DESCRIPTION
  This function despatches a transaction based on information in "despatch_info".
  Callers has the option of passing in a message (IS2000 message) to be stored
  internally which can be used later when processing the actions.

DEPENDENCIES
  None.

RETURN VALUE
  Transaction number that was despatched

SIDE EFFECTS
===========================================================================*/
uint16 mctrans_despatch_transaction
(
  qword   trans_time,
  mctrans_despatch_info_type* despatch_info,
  mctrans_pending_action_msg_type* msg
);

/*===========================================================================

FUNCTION mctrans_process_action

DESCRIPTION
  This function is called by mctrans_process_given_transaction to process an
  action in a transaction.  It is called each time an action in a
  transaction is to be processed.  This function runs in the server's
  context and must be brief and non blocking (except, of course, what's
  in action_processor, which is controlled by the transaction server).

DEPENDENCIES
  None

RETURN VALUE
  TRUE if transaction is valid and action executed.  False if transaction is
  expired.

SIDE EFFECTS
===========================================================================*/

boolean mctrans_process_action
(
  mctrans_action_type action,
  mctrans_trans_info_type * curr_active_trans,
  mctrans_server_token_type token,
  void (*action_processor)(mctrans_action_packet_type* action_packet)
);

/*===========================================================================

FUNCTION mctrans_record_pending_msg

DESCRIPTION
  Saves the pending message in the internal pending message buffer.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
void mctrans_record_pending_msg
(
  mctrans_pending_action_msg_type* action_msg,
  uint16 trans_num
);

/*===========================================================================

FUNCTION mctrans_get_pending_msg

DESCRIPTION
  Retrive the pending message for specified transaction number.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to pending message.  NULL if no message for transaction.

SIDE EFFECTS
===========================================================================*/
mctrans_pending_action_msg_type* mctrans_get_pending_msg
(
  uint16 trans_num
);





/*===========================================================================

FUNCTION mctrans_release_pending_msg

DESCRIPTION
  Release the pending message for the specified transaction number.  The message
  entry will be free to be used by other transactions.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_release_pending_msg
(
  uint16 trans_num
);

/*===========================================================================

FUNCTION mctrans_clearall_pending_msg

DESCRIPTION
  Clearall all of the pending messages for all the transactions. The message
  entry will be free to be used by other transactions.  Note that it's possible
  that the transaction becomes invalid before the message is released.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_clearall_pending_msg( void );

/*===========================================================================

FUNCTION mctrans_report_trans_done

DESCRIPTION
  This function sends a MCC_TRANS_DONE_SIG to notify MC that a transaction
  has just been completed.  This is used only for transactions that requires
  MC to perform some further processing after the transaction completes.  If
  no such processing is required, this function need not be called.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_report_trans_done
(
  mctrans_trans_info_type * curr_trans
);


/*===========================================================================

FUNCTION mctrans_add_action_to_server

DESCRIPTION
  Adds a specified action to the specified server.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
===========================================================================*/
extern void mctrans_add_action_to_server(
  mctrans_trans_info_type * curr_trans,
  mctrans_server_token_type server_token,
  mctrans_action_type action);

/*===========================================================================

FUNCTION mctrans_void_trans

DESCRIPTION
  This function invalidates the most recent transaction specified by trans_type
  if the transaction has not been committed.  Note that if multiple transactions
  of the same type are pending, this function can only be used to cancel the latest
  one.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_void_trans
(
  mctrans_transaction_type trans_type
);

/*===========================================================================

FUNCTION mctrans_is_trans_pending

DESCRIPTION
  This function checks whether there is a transaction pending.

DEPENDENCIES
  None

RETURN VALUE
  TRUE is assignment is pending

SIDE EFFECTS
  None.
===========================================================================*/
boolean mctrans_is_trans_pending ( mctrans_transaction_type trans );


/*===========================================================================

FUNCTION mctrans_register_action_handler

DESCRIPTION

  This registers the action handlers. It is called by
  mctrans_register_action_notifier_handler_interrupt_reg_func to register
  action handlers for each of the actions that each server processes.

  These action handlers are supplied by each Server to
  mctrans_register_action_notifier_handler_interrupt_reg_func.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : Successful in registering
  FALSE : Unsuccessful in registering

SIDE EFFECTS
  None
===========================================================================*/

boolean mctrans_register_action_handler(
  mctrans_server_token_type token,
  mctrans_action_type action_type,
  void (*action_proc)(mctrans_action_packet_type* action_packet)
);

/*===========================================================================

FUNCTION mctrans_init_action_handlers

DESCRIPTION
  This initializes the "action store". The "action store" stores pointers to
  action handler functions for each server for each action type that it
  supports.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/

void mctrans_init_action_handlers(void);


/*===========================================================================

FUNCTION mctrans_register_action_notifier_handler_interrupt_reg_func

DESCRIPTION
  This function registers 3 components:

  1] Action notifiers - Registers action notifier for the server.

  2] Custom ticker reg func - Registers the custom ticker registration function

  3] Action handlers - Registers function pointers for each of the actions that
     the server processes.

  All the above information is supplied in
  mctrans_function_pointer_initializer_type by the server calling this function

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mctrans_register_action_notifier_handler_interrupt_reg_func(
  mctrans_server_token_type token,
  mctrans_function_pointer_initializer_type action_func_pointer
);


/*===========================================================================

FUNCTION mctrans_process_default_action_time_tick

DESCRIPTION

  This is a default ticker function. RXC, TXC and SRCH provide a ticker
  registration function. However, if they do not provide a ticker
  registration function, then this default ticker function is used to keep
  track of action time of their transactions.

  This default ticker function, when registered with atmr, is called at
  every clock interrupt.

  This is called in interrupt context.

  It checks to see if a transaction's action time has arrived. If yes,
  it notifies all the servers who have not provided the registration function


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void mctrans_process_default_action_time_tick(m1x_stmr_event_type);

/*===========================================================================

FUNCTION mctrans_process_transactions

DESCRIPTION
  This function is called by the servers to process all the transactions
  whose action time has arrived.

  These transactions are sitting in the active queue.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mctrans_process_transactions(mctrans_server_token_type token);
/*===========================================================================

FUNCTION mctrans_despatch_ordered_transaction

DESCRIPTION
  This function despatches an ordered transaction based on information
  in "despatch_info".

  Callers has the option of passing in a message (IS2000 message) to be stored
  internally which can be used later when processing the actions.

DEPENDENCIES
  None.

RETURN VALUE
  Transaction number that was despatched

SIDE EFFECTS
  None

===========================================================================*/

extern uint16 mctrans_despatch_ordered_transaction
(
  /* Time transaction to be executed
  */
  qword trans_time,

  /* Information related to the transaction, including actions for each server
  */
  mctrans_despatch_info_type* despatch_info,

  /* Optional message to be stored internally to be used to generate transaction
     when transaction time comes
  */
  mctrans_pending_action_msg_type* msg,

  /* Order of servers
  */
  mctrans_server_token_type server_order[MCTRANS_SERVER_COUNT]

);

/*===========================================================================

FUNCTION MCTRANS_CLEAR_ALL_QUEUES

DESCRIPTION
  Clears Active, Outstanding queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_clear_all_queues(void);

/*===========================================================================

FUNCTION mctrans_print_active_queue

DESCRIPTION
  Prints the active queue.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

void mctrans_print_active_queue(void);

/*===========================================================================

FUNCTION MCTRANS_GET_ACTIVE_TRANS_DATA_Q_CNT

DESCRIPTION
  Get the count for the active transactions queue

DEPENDENCIES
  None

RETURN VALUE
  No of elements in active queue

SIDE EFFECTS
  None.
===========================================================================*/

int mctrans_get_active_trans_data_q_cnt(void);

/*===========================================================================

FUNCTION MCTRANS_CHECK_IF_TRANS_PENDING

DESCRIPTION
  This API checks if a given type of transaction exists in the deferred transactions queue.
  
DEPENDENCIES
  Should be invoked after LTA Gap end before restoring any transactions
  back from deferred queue.

  No need for mutex protection here and at the LTA gap deferred queue can
  only be accessed by MC task.

RETURN VALUE
  True if transaction was found
  False otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean mctrans_check_if_trans_pending
(
  mctrans_transaction_type trans_type
);

/*===========================================================================

FUNCTION MCTRANS_CHECK_AND_RESTORE_DEFERRED_TRANSACTIONS

DESCRIPTION
  Place the transactions from the deferred transactions queue to outstanding
  transactions queue based on whether they will be able to execute before 
  tuneaway start. 
  This will be needed if G changed the reservation time and now the 
  reservation time is greater than it was previously.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_check_and_restore_deferred_transactions(void);

/*===========================================================================

FUNCTION MCTRANS_RESTORE_HHO_DEFERRED_TRANSACTIONS

DESCRIPTION
  Place the hho transactions from the deferred transactions queue to outstanding
  transactions queue, to execute HHO trasactions immediately. 
  This API will get called if the HHO defer timer got expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_restore_hho_deferred_transactions(void);

/*===========================================================================

FUNCTION MCTRANS_RESTORE_SHO_DEFERRED_TRANSACTIONS

DESCRIPTION
  Place the sho transactions from the deferred transactions queue to outstanding
  transactions queue, to execute SHO trasactions immediately. 
  This API will get called if the SHO defer timer got expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_restore_sho_deferred_transactions(void);

/*===========================================================================

FUNCTION MCTRANS_RESTORE_CFS_DEFERRED_TRANSACTIONS

DESCRIPTION
  Place the cfs transactions from the deferred transactions queue to outstanding
  transactions queue, to execute CFS trasactions immediately. 
  This API will get called if the CFS defer timer got expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_restore_cfs_deferred_transactions(void);

/*===========================================================================

FUNCTION MCTRANS_CHECK_AND_DEFER_OUTSTANDING_TRANSACTIONS

DESCRIPTION
  Place the transactions in the outstanding transactions queue to deferred
  transactions queue based on whether they will be able to execute before 
  tuneaway start. 
  This will ensure that none of the oustanding transactions
  are triggered by any of the servers during the QTA/LTA gap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_check_and_defer_outstanding_transactions(void);

/*===========================================================================

FUNCTION MCTRANS_DEFER_ALL_OUTSTANDING_TRANSACTIONS

DESCRIPTION
  Place all the transactions in the outstanding transactions queue to deferred
  transactions queue. This will ensure that none of the oustanding transactions
  are triggered by any of the servers during the QTA/LTA gap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_defer_all_outstanding_transactions(void);

/*===========================================================================

FUNCTION MCTRANS_RESTORE_DEFERRED_TRANSACTIONS

DESCRIPTION
  Place all the transactions in the deferred transactions queue back to the
  outstanding transactions queue. The transactions can be triggered by the
  servers only after they have been placed back to the oustanding transactions 
  queue.
  This API should be invoked after QTA/LTA gap end before any new transactions
  are queued on to the outstanding transactions queue.

DEPENDENCIES
  Should be invoked before any new transactions are queued on outstanding trans 
  queue after QTA/LTA gap end.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_restore_deferred_transactions
(
  boolean is_ta_cancelled
);

/*===========================================================================

FUNCTION MCTRANS_ADJUST_TRANSACTION_BEFORE_REQUEUE

DESCRIPTION
  Makes necessary adjustments to a transaction before queuing it back on the 
  outstanding queue after QTA gap end.
  The adjustments can include alignment of transaction time in accordance 
  with current time and will depend on the type of transaction being considered.

  This API should is only called in MC task context. But it operates on data
  structures that may also be accessed concurrently by other tasks.
  The calling API should ensure this API is called under mutex protection.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -  if transaction needs to be queued back in outstanding transactions 
           queue
  FALSE - if transaction does need to be queued back

SIDE EFFECTS
  None.
===========================================================================*/
boolean mctrans_adjust_transaction_before_requeue
( 
  mctrans_trans_info_type * trans_obj,
  boolean is_ta_cancelled
);

/*===========================================================================

FUNCTION MCTRANS_IS_TRANSACTION_DEFERRED

DESCRIPTION
  This function decides if a particular transaction should be despatched or 
  deferred. 
  This decision is taken by taking into account the estimated time the
  transaction requires for completion and the time at which next QTA/LTA 
  is scheduled.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Defer transaction and put it in deferred transactions queue
  FALSE - Continue with despatching the transaction

SIDE EFFECTS
  None.
===========================================================================*/
boolean mctrans_is_transaction_deferred
( 
  mctrans_trans_info_type * trans_obj
);

/*===========================================================================

FUNCTION MCTRANS_GET_INT_TICKERS_REG_STATUS

DESCRIPTION
  This function decides if interrupt tickers (both custom and default)
   have been registered or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If interrupt tickers (both custom and default)
         have been registered
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean mctrans_get_int_tickers_reg_status( void );

/*===========================================================================

FUNCTION MCTRANS_UPDATE_INT_TICKERS_REG_STATUS

DESCRIPTION
  This function will update the interrupt tickers (both custom and default)
  registered status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mctrans_update_int_tickers_reg_status
( 
  boolean int_tickers_reg_status
);

/*===========================================================================

FUNCTION MCTRANS_GET_TUNEAWAY_REMAINING_TIME

DESCRIPTION
  This function returns the time remaining for the start of registered tuneaway.
  If no tuneaway is registered this API will return an invalid value.
  The time is returned in ms units.

DEPENDENCIES
  None

RETURN VALUE
  Remaining time in ms if TA is registered
  ONEX2GTA_TUNEAWAY_NOT_REGISTERED - is no TA is registered

SIDE EFFECTS
  None.
===========================================================================*/
uint32 mctrans_get_tuneaway_remaining_time(void);

#endif /*MCTRANS_I_H*/
