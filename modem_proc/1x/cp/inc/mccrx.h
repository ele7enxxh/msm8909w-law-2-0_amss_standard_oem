#ifndef MCCRX_H
#define MCCRX_H
/*===========================================================================

                M C C R X   H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface between
  the Main Control CDMA subtask and the Receive task.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
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


$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccrx.h_v   1.3   02 Oct 2002 13:38:30   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccrx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/28/16   sjo     Debug buffer enhancements. Added more confirmation rpts to
                   mmoc_cnf_dbg_buf. Added mc_dbg_msgr_q debug buffer also.
11/15/10   jtm     Initial SU API split for CP.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
12/28/04   va      Featurized num_lost_frames under DV
11/05/04   sb      Added num_lost_frames to mccrx_tx_on_type.
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
08/13/04   bkm     Added T79 Expired report from RXC to MC for Rel D Bypass
                   two good frames feature.
11/07/02   az      Mainlined FEATURE_IS95B
10/02/02   az      Mainlined FEATURE_IS2000
06/26/01   fc      Merged the following from common archive:
           06/11/01   fc      Added RXC_PC_T40M_TMO_R and renamed RXC_PC_TMO_R
                              to RXC_PC_T30M_TMO_R to support the split of fade
                              report between idle and system access substates.
04/09/01   lcc     Added support for DCCH.  Removed some used definitions.
06/05/00   fc      Modified mccrx_ol_type.
05/30/00   fc      Included the header file cai.h.
05/30/00   fc      Added RXC_OL_R report.
05/26/00   jq      Added cur_slot_mask into mccsrch_wakeup_rpt_type
04/24/00   lcc     Added SCH FER field to mccrx_pwr_type.
03/13/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ks      Removed FEATURE_ENHANCED_STANDBY_II definition.
01/26/00   va      Merged the GV40401 Changes
                   Added RXC_HHO_TIMEOUT_R report
12/20/99   va      Changed RX action time report to "RXC_AT_R" to make it
                   more general
11/15/99   va      IS2000 Changes.
                   Added RXC_SCR_AT_R report for RXC to inform when
                   action time for a SCR change has arrived.
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      RXC_PC_T72M_TMO_R & RXC_VALID_PC_MSG_R two reports are
                   defined to support T72M/T40M timers processing.
02/26/99   ejv     Merged the fix for timestamp SLOT_R message.
09/04/98   pms     Neared the variables to save ROM space.
10/27/94   rdh     Removed slotted tmo warning message field.
10/24/94   rdh     Addition for PN autoswitch/error correction.
07/20/92   jai     Created file.

===========================================================================*/

#include "cmd.h"
#include "qw.h"
#include "cai.h"
#include "cai_v.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* REPORTS from CDMA Receive task to CDMA Main Control task */

/* Report Codes */
typedef enum
{
/* 0 */
  RXC_PC_T30M_TMO_R,  /* No valid message with valid CRC received in T30M secs */
  RXC_PC_T40M_TMO_R,  /* No valid PC message with valid CRC in T40M secs */
  RXC_PC_T72M_TMO_R,  /* No valid PC message with valid CRC in T72M secs */
  RXC_VALID_PC_MSG_R, /* Valid PC message after T72M expires */
  RXC_TX_OFF_R,       /* Timeout on receiving valid frames occured on the
                         Forward Traffic Channel indicating transmitter should
                         be disabled */
  RXC_TX_ON_R,        /* Valid frames received which indicate that transmitter
                         should be re-enabled */
  RXC_TC_TMO_R,       /* All received traffic channel frames placed into
                         category 9 or 10  */
  RXC_VALID_R,        /* A Valid frame has been received on the Traffic Channel
                         during the Traffic Channel Initialization substate */
  RXC_PWR_R,          /* Power Measurement report */
  RXC_SLOT_R,         /* A valid message was received and in at least 2 slots */

/* 10 */
  RXC_PWR_ON_R,       /* Resume FPC after CDMA to CDMA hard handoff */
  RXC_HHO_TIMEOUT_R,  /* Return-on-failure hard handoff timed out, so return */
  RXC_AT_R,           /* Action time for SCR has arrived */
  RXC_OL_R,           /* Outer Loop report */

  RXC_T79_EXPIRED_R,  /* Timeout to receive 2 good frames while Tx enabled */

  RXC_PC_FS_TMO_R,    /* No valid message on either BCCH or FCCCH n 4 secs */
  RXC_TOTAL_COMMANDS_R
} mccrx_rpt_name_type;

typedef struct {
    cmd_hdr_type        rpt_hdr;      /* report header - includes q_link */
    mccrx_rpt_name_type   rpt;          /* Report type */
} mccrx_rpt_hdr_type;

/* RXC_PC_TMO_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
}mccrx_pc_tmo_type;

/* RXC_TX_OFF_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
}mccrx_tx_off_type;

/* RXC_TX_ON_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
  word num_lost_frames;
} mccrx_tx_on_type;

/* RXC_TC_TMO_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
} mccrx_tc_tmo_type;

/* RXC_VALID_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
}mccrx_valid_type;

/* RXC_PWR_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
  word    pwr_meas_frames;
  byte    errors_detected;
  boolean sch_pwr_meas_included;
  byte    sch_id;
  word    sch_pwr_meas_frames;
  word    sch_errors_detected;
  boolean dcch_pwr_meas_incl;
  word    dcch_pwr_meas_frames;
  byte    dcch_errors_detected;
} mccrx_pwr_type;

/* RXC_SLOT_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
  qword rpt_time;
  byte slot_mask;
} mccrx_slot_type;

/* RXC_PWR_ON_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
} mccrx_pwr_on_type;

/* RXC_HHO_TIMEOUT_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
} mccrx_hho_timeout_type;

/* RXC_OL_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
  boolean            fch_incl;            /* FCH included indicator */
  byte               fpc_fch_curr_setpt;  /* FCH outer loop Eb/Nt setpoint */
  boolean            dcch_incl;           /* DCCH included indicator */
  byte               fpc_dcch_curr_setpt; /* F-DCCH outer loop Eb/Nt setpoint */
  byte               num_sup;             /* Number of SCHs */
  struct
  {
    boolean sch_id;                       /* SCH index */
    byte fpc_sch_curr_setpt;              /* SCH outer loop Eb/Nt setpoint */
  }                  sch[CAI_OUTER_RPT_MAX_SUP];
} mccrx_ol_type;

/* RXC_T79_EXPIRED_R */
typedef struct
{
  mccrx_rpt_hdr_type hdr;
} mccrx_t79_expired_type;

/* Union of reports */
typedef union
{
  mccrx_rpt_hdr_type    hdr;
  mccrx_pc_tmo_type  pc_tmo;
  mccrx_tx_off_type  tx_off;
  mccrx_tx_on_type   tx_on;
  mccrx_tc_tmo_type  tc_tmo;
  mccrx_valid_type   valid;
  mccrx_pwr_type       pwr;
  mccrx_slot_type    slot;
  mccrx_pwr_on_type  pwr_on;
  mccrx_hho_timeout_type hho_timeout;
  mccrx_ol_type ol;
  mccrx_t79_expired_type t79_expired;
}mccrx_rpt_type;

/* queue for free buffers */
extern q_type mcc_rx_free_q;

/* Queue for reports from RXC to MCC */
extern q_type mcc_rx_rpt_q;


/*===========================================================================

FUNCTION MCCRX_INIT

DESCRIPTION
  This function initializes the queues between the Receive task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mccrx_init( void );

/*===========================================================================

FUNCTION MCC_GET_CUR_MSG_SEQ

DESCRIPTION
  This function gets the current config message sequence number in use
  on this serving base station.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the current config_msg_seq.

SIDE EFFECTS
  None.

===========================================================================*/

extern word mcc_get_cur_msg_seq( void );

#endif /* MCCRX_H */

