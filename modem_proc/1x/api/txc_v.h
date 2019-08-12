#ifndef TXC_V_H
#define TXC_V_H
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/txc_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/14   jh      remove qw.h include
04/27/12   srk     Added an API to return PA state if TX is ON.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
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
#include "comdef.h"

//mux
#include "txc.h"
#include "txcmc.h"

//drivers
#include "enc.h"
#include "enc_v.h"

//cp
#include "cai.h"
#include "caii.h"  //IS2000 change
#include "tx.h"

//other
#include "queue.h"
#include "cmd.h"

typedef enum
{
  TXC_PA_GAIN_STATE_TX_OFF = -1,
  TXC_PA_GAIN_STATE_0      =  0,
  TXC_PA_GAIN_STATE_1,
  TXC_PA_GAIN_STATE_2,
  TXC_PA_GAIN_STATE_3,
  TXC_PA_GAIN_STATE_NUM
} txc_pa_gain_state_enum_type;

/*===========================================================================

FUNCTION TXC_SET_VOC_ACTIVITY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void txc_set_voc_activity
(
  boolean enable,   /* enable/disable voice activity test */
  word voc_1,       /* # of full rate */
  word voc_2,       /* # of 1/2 rate */
  word voc_8        /* # of 1/8 rate */
);

/*===========================================================================

FUNCTION TXC_GET_PA_STATE

DESCRIPTION
  Returns PA state if TX is ON.

DEPENDENCIES
  None.

RETURN VALUE
  txc_pa_gain_state_enum_type

SIDE EFFECTS
  None
===========================================================================*/
extern txc_pa_gain_state_enum_type txc_get_pa_state(void);

#endif /*TXC_V_H*/
