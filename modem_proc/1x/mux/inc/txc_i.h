#ifndef TXC_I_H
#define TXC_I_H
/*===========================================================================

          T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the transmit task that don't fit
  into one of the other H files such as CAI.H

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

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/txc.h_v   1.27   16 Oct 2002 16:35:20   randyp  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/txc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/12   srk     Mainlined FEATURE_IS2000_SCH and FEATURE_IS2000_P2
02/01/12   mca     Added txc_tx_is_on()
01/31/12   srk     Feature Clean Up.
12/13/11   srk     Mainlined FEATURE_RL_JUMP_ENHANCEMENT
12/01/11   srk     Added txc_set_ok_to_sleep().
11/22/11   srk     Feature clean up: Mainlined and Internalized Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/06/10   jtm     Remove deprecated RF file include.
12/15/10   jtm     Feature cleanup.
11/01/10   jtm     Private file clean up.
10/30/10   jtm     Modem 1x SU API changes.
02/04/10   jtm     Removed FEATURE_PLT.
11/05/09   jtm     Added functions to return information about various RL
                   channels.
08/05/09   jtm     Lint fixes.
04/08/09   jtm     Initial 1X Advanced feature support.
04/01/09   jtm     Added customer.h and featurized necessary includes.
03/20/09   jtm     Added txcmc_v.h include.
03/20/09   jtm     Cleaned up header file inclusions.
03/04/09   adw     Moved txc_chan_status_type to public api header.
11/20/08   jtm     Merged from //source/tip
11/19/08   jtm     Merged Common Modem Interface changes from CMI sandbox
07/21/08   jtm     Removed FEATURE_DELAYED_TX_CLOCK_SHUTDOWN code.
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
                   Allow function txc_tx_shutdown() to be visible for
                   FEATURE_RL_JUMP_ENHANCEMENT.
04/13/05   bn/fq   Moved all the common defines and structures to txcmc.h
01/25/05   vlc     Removed Feature Rel D from Jump to Hyperspace command.
01/17/05   ljl     Updated for Rel D interface changes by CP.
01/11/05   vlc     Added support for Rel D Fast Call Set Up - New Jump to
                   Hyperspace command feature.
09/27/04   ljl     Replaced pdch with fwd_pdch and added revd_pdch.
06/16/04   sr      Merged in changes from 6500 5004 build
05/21/04   jyw     introduce new RF PA backoff interface
04/22/04   jrp     Changes to txc_tc_cmd_type in support of Rev D.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
02/03/04   sab     Featurized confirmation_type under Release B.
09/15/03   jrp     Added txc_tx_shutdown().
08/06/03   sab     Added confirmation_type to payload of TXC_MSG_F.
04/15/03   jrp     Support for FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
04/07/03   jrp     Lint changes.
10/16/02   jrp     Added prototype for txc_write_pcbit_data().
08/01/02   jrp     Removed txc_flag_eib(), txc_flag_qib(), and txc_flag_sch_eib().
                   These are now modified directly by rxctraffic.
07/01/02   tc      Added support for rlgain_common_pilot, ic_thresh, ic_max.
06/18/02   jrp     Added txc_flag_qib() and txc_flag_sch_eib() in support of
                   FFPC Modes 4,5,6.
06/07/02   hrk     Added support to specify physical channel on which signaling
                   should be sent for VP2.
05/22/02   bn      Added new logics to support VP2 RPC
04/03/02   jrp     Added TXC_REACH_PROBE_F.
03/04/02   bgc     Changed features to IS2000_R_SCH where appropriate.
02/25/02   hjr     Fixed featurization of FEATURE_IS2000_SCH with
                   FEATURE_IS2000_DS_RS2 on.
02/25/02   bn      Added support for  MUX SVD signaling
02/22/02   tc      Supported FEATURE_IS2000_REL_A_CC.
02/11/02   tc      Supported Reverse Enhance Access Channel.
12/10/01   hrk     Renamed txc_flag_erasure txc_flag_eib.
08/22/01   bn      Support DCCH in FTM mode
08/21/01   sj      Changed rs1_20ms_incl -> rc3_rc5_20ms_incl  &
                           rs2_20ms_incl -> rc4_rc6_20ms_incl
07/12/01   vlc     Changed FEATURE_RC4_SCH to FEATURE_DS_IS2000_RS2 in order
                   to eliminate FEATURE_RC4_SCH.
06/15/01   snn     Added externs for new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_IS2000
                   Delete FEATURE_SPECIAL_MDR
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   snn     modified the prototype for txc_get_sch_info().
05/18/01   kk      RS2 TDSO support.
05/14/01   vlc     Removed test feature so SCH RC4 can be used in DMSS software.
04/26/01   vlc     Changed name of constants to accomodate new SCH RC4
                   development on MSM5100.  Added rev_sch_rc to txc_tc_cmd_type
                   under Factory Test Mode only.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2.
03/23/01   sr      Added support for 32X
03/20/01   ych     Merged JCDMA Features.
03/07/01   va      Added p_rev_in_use info in TC_F and RETURNED_F interfaces.
01/30/01   tc      Merged GPS support.
12/01/00   ak      Added function extern for txc_inform_sch_chng.
10/04/00   jq      Added FTM FCH and SCH loopback feature
09/06/00   bgc     Added TXC_FTM_SCH_F to setup SCH channels separate of
                   FCH for FEATURE_FACTORY_TESTMODE.
08/14/00   sr      Added support for pilot gating and FCH 1/8 rate gating in
                   PLT
07/17/00   va      Added support for 8th rate gating interface.
05/30/00   hrk     Changed prototype for txc_set_pch_fch_gain().
05/11/00   hrk     Added prototype for txc_set_fpc_mode().
04/24/00   lcc     Added TXC_MC_TRANS_F and related command structure.
04/11/00   na      Added prototype for txc_get_sch_info() under
                   FEATURE_DS_IS2000.
02/14/00   hrk     Added prototype for function txc_set_pch_fch_gain ()
02/10/00   hrk     Added prototype for function txc_get_fch_rate_set ()
1/31/00    sr      Added action time processing and implementation of
                   txc_ho_cmd, txc_hho_ok_cmd and txc_returned_cmd.
                   Parameters related to reverse link gain adjustment and
                   power control step size haven't yet been implemented
01/07/00   sr      Added num_preambles to txc_tc_cmd_type
12/22/99   hrk     Moved definition of txc_gain_type out of FEATURE_PLT
12/20/99   va      Added pwr_ctrl_step to the txc_pwr_ctrl_cmd_type,
                   txc_ho_cmd_type and txc_returned_cmd_type interfaces.
11/23/99   sr      Added prototype for txc_data_pump_init()
11/19/99   sr      Minimum implementation of new mc-txc interface
                   including support for new TXC_SO_F, TXC_CONNECT_SO_F
                   and TXC_RELEASE_F commands
                   Vanitha's new mc-txc interface
                   Added changes to support data pump and supplemental channels
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
                   Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
                   Updated the extern definition for txc_register_action_time()
                   Added extern definition for txc_get_time_in_1_25ms_unit()
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Added prototypes for txc_set_frame_offset(),
                   txc_hho_action_time_chk(), txc_flag_next_int() and
                   txc_int_occurred()
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added txc_suspend_tx function to suspend/enable Access
                   probes when T72M timer expires.
05/10/99   kmp     Merged in the IS-95B changes listed below
           fc      Put TXC_ACC_PROBE_F command under FEATURE_IS95B_PILOT_RPT.
           fc      Added TXC_ACC_PROBE_F command.
08/25/95   gb      Inform TX of the HHO parameters num_preamble and nom_pwr.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/13/95   gb      Changed mux option initialization.
06/14/95   gb      Added support for Rate set 2 service options.
03/29/95   jca     Added sigs and tcb_ptr to txc_tc_cmd_type.
12/23/94   jca     Added nom_pwr to TXC_ACCESS_F command parameters.
11/11/94   jca     Deleted TXC_ACC_KILL_F cmd.  Modified TXC_ACC_STOP_F cmd.
11/01/94   gb      Sector Lock test functionality added.
06/21/94   jca     Added TXC_FRAME_OFFSET_F command.
04/20/94   jca     Added Loopback service option.  Deleted support for
                   DIAG data services.
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
10/06/92   jai     Added support for access channel logging.
07/14/92   jai     First Revision

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
#include "txc.h"
#include "txc_v.h"
#include "ffpc_i.h"
#include "txcmc.h"
#include "txcmc_i.h"

//drivers
#include "enc.h"
#include "enc_v.h"
#include "enc_i.h"

//cp
#include "cai.h"
#include "caii.h"  //IS2000 change
#include "tx.h"

//other
#include "queue.h"
#include "qw.h"
#include "cmd.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef struct
{
  txc_sig_frame_type sig; /* Signaling mode */
  byte data [ENC_14400_FULL_RATE_SIZE] ALIGN(4);
} txc_tc_frame_type;

typedef struct
{
  byte frame_cat;        /* frame category */
  byte data [ENC_14400_FULL_RATE_SIZE] ALIGN(4);
  boolean dtx;
} txc_tc_dcch_frame_type;

typedef struct
{
  byte data [ENC_SCH_RC3_32X_SIZE] ALIGN(4);
  word size; /* for FACTORY_TESTMODE, the amount of data in */
             /* the data buffer */
} txc_tc_supp_frame_type;

typedef struct {                  /* TXC_VOC_F */
  txc_hdr_type   hdr;
} txc_voc_cmd_type;

/* Gain values output to MDSP has 15 bit resolution as result of TX
** gain enhancement on Dora target.
*/
typedef struct
{
   word pch_gain;
   word fch_gain;
   word dcch_gain;
   word sch_gain;
} txc_gain_type;

#ifdef FEATURE_IS2000_1X_ADV
/* Linear gain ratios output to MDSP as needed for 1x Advance
   enabled targets. Since software only calculates gains every
   20ms frame and the MDSP needs to adjust gains every
   half PCG,the mdsp will perform the gain calculations using these
   ratios.
*/
typedef struct
{
   word fch_gain_ratio;

   word fch_ack_gain_ratio;
   word sch_gain_ratio;
   word sch_ack_gain_ratio;
} txc_linear_gain_ratio_type;

/* Ask Searcher for the active cell size and reduce it to an index we
   can use in the table.
   0 = <= 1 active cell
   1 = 2 or more active cells
*/
#define TXC_GET_ACTIVE_CELL_SIZE( acs ) \
 ( ( ( acs ) <= 1 ) ? 0 : 1 )

#endif /* FEATURE_IS2000_1X_ADV */

/*
** NOTE: when adding/deleting memebers to this enum,
** change appropriately the #define TXC_DCCH_SCH_CONFIG_TYPE
*/
typedef enum
{
   DCCH_ACTIVE_SCH_OFF = 0x0,
   DCCH_DTX_SCH_OFF,
   DCCH_ACTIVE_SCH_ACTIVE,
   DCCH_ACTIVE_SCH_DTX,
   DCCH_DTX_SCH_ACTIVE,
   DCCH_DTX_SCH_DTX
} txc_dcch_sch_config_type;

/* this is the number of modes as defined in txc_dcch_sch_config_type */
#define TXC_DCCH_SCH_CONFIG_MODES  6

typedef struct
{
  txc_gain_type gain;
  int16         pch_adj_gain;
} txc_dcch_sch_gain_type;

#ifdef FEATURE_IS2000_REL_A_SVD
typedef txc_dcch_sch_gain_type  txc_fch_dcch_sch_gain_type;
#endif /* FEATURE_IS2000_REL_A_SVD */

typedef enum
{
  TXC_RL_PILOT,
  TXC_RL_FCH,
  TXC_RL_DCCH,
  TXC_RL_SCH
} txc_rl_traffic_chan_type;

/* <EJECT> */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/




/* <EJECT> */
/*===========================================================================

FUNCTION TXC_HHO

DESCRIPTION
  This function is called by MC on a hard handoff boundary to set the
  number of preamble frames to be transmitted and the new nominal power.

DEPENDENCIES
 Relies on the fact that TX_OFF_TX_F has already been sent and TXC is now
 waiting for TX_ON_TX_F.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void txc_hho(
word num_preambles,
word nom_pwr
);

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_FLAG_NEXT_INT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void txc_flag_next_int( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_INT_OCCURRED

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean txc_int_occurred( void );

/* <EJECT> */
/*===========================================================================

FUNCTION TXC_DATA_PUMP_INIT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_set_pch_fch_gain
(
 enc_rate_type,
 txc_gain_type,
 int16,
 txc_chan_status_type
);

/*===========================================================================
FUNCTION TXC_SET_FPC_MODE

DESCRIPTION
  Sets the value of FPC_MODE in txc_so struct.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_set_fpc_mode (fpc_mode_type fpc_mode);

/*===========================================================================

FUNCTION TXC_WRITE_PCBIT_DATA

DESCRIPTION
  This function is called by RXC to write the PCBIT data.  It must be called
  from RXC context because of timing constraints.  The power control data
  must be written so that it is transmitted two frames after the frame it
  is giving quality information about.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_write_pcbit_data( void );

/*===========================================================================

FUNCTION TXC_TX_SHUTDOWN

DESCRIPTION
  Turns off tx clock and disables TXC interrupt.  Allows MC to command SRCH to
  sleep.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_tx_shutdown( void );

/*===========================================================================

FUNCTION TXC_SET_OK_TO_SLEEP

DESCRIPTION
  This function sets the txc_sleep_allowed parameter of txc_delay_enc_shutdown.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void txc_set_ok_to_sleep( boolean ok_to_sleep );

/*===========================================================================

FUNCTION TXC_UPDATE_CHAN_CONFIG_TO_RF

DESCRIPTION This function calls by MUX to update the actual channel config to RF



DEPENDENCIES None


SIDE EFFECTS None

===========================================================================*/
extern void txc_update_chan_config_to_rf (boolean  valid_config);

/*===========================================================================

FUNCTION TXC_FCH_IS_ON

DESCRIPTION
  Returns if FCH is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If FCH is On
  FALSE: If FCH is Off

SIDE EFFECTS
  None
===========================================================================*/
boolean txc_fch_is_on(void);

/*===========================================================================

FUNCTION TXC_SCH_IS_ON

DESCRIPTION
  Returns if SCH is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If SCH is On
  FALSE: If SCH is Off

SIDE EFFECTS
  None
===========================================================================*/
boolean txc_sch_is_on(void);

/*===========================================================================

FUNCTION TXC_TX_IS_ON

DESCRIPTION
  Returns if TX is ON.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If TX is On
  FALSE: If TX is Off

SIDE EFFECTS
  None
===========================================================================*/
boolean txc_tx_is_on(void);

/*===========================================================================

FUNCTION TXC_GET_REV_CHAN_GAIN

DESCRIPTION
  Returns the current gain value of the specified channel.

DEPENDENCIES
  None

RETURN VALUE
  Digital gain of the specified channel.

SIDE EFFECTS
  None
===========================================================================*/
uint16 txc_get_rev_chan_gain(txc_rl_traffic_chan_type channel);

/*===========================================================================

FUNCTION TXC_GET_REV_SCH_RATE

DESCRIPTION
  Returns the current R-SCH rate.

DEPENDENCIES
  None

RETURN VALUE
  Rate of R-SCH.

SIDE EFFECTS
  None
===========================================================================*/
enc_sch_rate_type txc_get_rev_sch_rate(void);

/*===========================================================================

FUNCTION TXC_GET_FCH_RC

DESCRIPTION
  Returns the radio configuration for FCH.

DEPENDENCIES
  None

RETURN VALUE
  The radio configuration for the current R-FCH.

SIDE EFFECTS
  None
===========================================================================*/
cai_radio_config_type txc_get_fch_rc (void);

#endif /*TXC_I_H*/
