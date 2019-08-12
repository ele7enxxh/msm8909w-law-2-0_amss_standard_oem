#ifndef RXC_I_H
#define RXC_I_H
/*===========================================================================

                R X C D M A   S U B T A S K    H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for the main
  task to interface with the RXC subtask.

  Copyright (c) 1990 - 2016 Qualcomm Technologies, Inc.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/rxc_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/16   srk     Use locally stored/passed in RC instead of global variable
                   to avoid race condition between RX Task and TC ISR.
02/12/15   jh      Changes for SW Demback
03/03/14   srk     Dynamically specify stack size for ISRs.
10/25/13   cjb     Avoid registering for RX ISR when FW is disabled
07/16/12   srk     Removed FEATURE_IS2000_REL_A_CC
06/15/12   srk     Removed FEATURE_IS2000_REL_A_CC_SMMC
02/23/12   srk     Mainlined FEATURE_IS2000_SCH and FEATURE_IS2000_P2
01/17/12   jtm     Revert REL_A clean up.
12/13/11   srk     Feature clean up: Mainlined FEATURE_GPSONE_OVERRIDE_RDA and
                   changed all FEATURE_IS2000_REL_A_* to FEATURE_IS2000_REL_A
11/22/11   srk     Feature clean up: Mainlined and Internalized Features
09/28/11   srk     Mainlined Always On features and removed Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
12/15/10   jtm     Feature cleanup.
12/03/10   jtm     Removed MDR/SCCH support.
11/15/10   jtm     Initial SU API split for CP.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
11/01/10   jtm     Modem 1x SU API changes.
10/30/10   jtm     Modem 1x SU API changes.
06/09/10   jtm     Relocated some 1xA specific structure members to rxc_rc11
                   declared in rxctraffic.h.
05/05/10   jtm     Added support for new RLP API using FEATURE_IS95_RLP_MUX_COMMON_API
04/12/10   jtm     Updated rxc_dec_tc_type to include 1xA parameters and changed
                   valid_ffpc_frame to valid_frame.
04/02/10   jtm     Changed included rlp header file.
10/28/09   jtm     Added early_decode_pcg data member to rxc_dec_tc_type.
09/14/09   adw     Changes to support modem data header CMI refactoring.
08/05/09   jtm     Lint fixes.
06/06/09   vlc     Added support for 1x Advanced (RC11.)
04/01/09   jtm     Added customer.h
03/19/09   jtm     Updated included headers.
11/20/08   jtm     Merged from //source/tip
11/19/08   jtm     Merged Common Modem Interface changes from CMI sandbox
07/17/07   vlc     Moved rxc_get_chan_tc_demod_config_param outside
                   FEATURE_IS2000_REL_A_CC.
01/09/07   vlc     Moved rxc_chan_demod_config_param_type outside
                   FEATURE_IS2000_REL_A_CC.  This type is used by SRCH
                   outside the REL_A feature.
06/09/06   trc     Remove T_MSM6700 featurization
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/13/05   bn/fq   Moved all the common defines and structures to rxcmc.h
12/07/04   ejv     Added rxc_tc_chan_id_type, rxc_sch_info_type,
                   rxc_tc_chan_info_type, and prototype for
                   rxc_get_chan_tc_demod_config_param.
11/09/05   vlc     Added support for Rel D Fast Call Set Up Bypass 2 Good Frame
                   feature.
09/28/04   sr      Merged in changes from 6550 2009 build
08/09/04   bn/kwo  Added new interfaces to expose chan mod setup parameters
05/26/04   vlc     Added RXC_FOR_PDCH to rxc_dsch_type.
05/07/04   vlc     Implemented new RLP interface for EV-DV.  The new interface
                   replaces the current interface for 1x channels as well.
                   Also, modified RLP logging extensively to work with new
                   interface.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/03/04   sr      Changes for DV plus included changes to MSMSHARED tip
01/13/04   jrp     Moved descramble bypass variables in RXC_TCI_F within
                   FEATURE_IS2000_REL_C_DV.
11/04/03   bn      Defined FCCCH deint task in first slot and BCCH deint task in second slot
10/09/03   bn      Corrected the FCCCH data type in rxc_fccch_info_type
04/18/03   bn      Removed JCDMA Features in FER so that it can apply to DIAG
03/20/03   bn,sr   Added NCCCH_STATE to support simultaneously monitor BCCH and FCCCH
02/10/03   fc      Added page_match in rxc_wake_type.
11/19/02   hxw     Added rxc_gps_cmd_type support for gpsOne.
10/29/02   bn      Added new fields in the rxc_chan_config_cmd_type and chan_info
                   to support BC BCCH
08/13/02   sr      Removed rxc_idle_chan_config(). New SRCH-RXC interface for
                   Fast RAHO replaces this.
07/11/02   bn      Modified function prototype to support new QIB logic
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
04/19/02   jrp     Removed fch_incl from rxc_get_setpt_cmd_type.
04/01/02   lad     Added members needed for BCCH/FCCCH frame info logging.
03/27/02   ems     Modified sizes of ser and energy fields, featurized with
                   MSMHW_EXTENDED_ENERGY_MATRIX.
02/27/02   lad     Merged IS2000 Rel A Common Channels:
                   Added rxc_idle_chan_config().
                   Added rxc_idle_activate().
                   Assigned members of rxc_chan_type to the values of
                   caix_chan_type to eliminate need for translation.  Updated
                   rxc_idle_cmd_type to contain chan_id.  Added 'length' and
                   'frame' fields to rxc_dec_type.
                   Added the complete BCCH/FCCCH cmd interfaces & unused code.
02/25/02   bn      Added support for  MUX SVD signaling
01/21/02   bgc     Merged data changes for disabling MDR.
01/03/02   bgc     Added SCH featurization to turn off for MSM6000.
12/10/01   hrk     Renamed RXC_FULL_FRAME to RXC_GOOD_FRAME.
11/13/01   bn      Added the enhance DCCH DTX detection and channel supervision logics
08/22/01   bn      Support DCCH in FTM mode
06/15/01   snn     Added extern for new functions for DCCH UI display.
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_IS95B, FEATURE_RXC_ACTION_TIME_PROCESSING,
                   FEATURE_IS2000
                   Deleted FEATURE_FFPC_OLD, FEATURE_SPECIAL_MDR
06/11/01   fc      Removed #ifdef for rxc_pc_fade_type.
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
06/01/01   snn     Modified the rxc_get_sch_info to return SCH RC. Added
                   prototype for rxc_get_fch_rate_set()
05/11/01   jq      Added RXC_NO_FRAME.
05/03/01   lcc     Changed rxc_returned_type to include "included" field for
                   rev_pwr_cntl_delay.  Also made some minor restructuring.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
03/20/01   ych     Merged JCDMA Features
03/08/01   va      Added rxc_get_setpt_cmd_type data structure.
03/07/01   va      ASdded p_rev_in_use in the TCI_F and RETURNED_F interface.
02/01/01   tc      Updated RXC_RIF_HHO_OR_CFS_F to RXC_RIFHHO_CFS_GPS_F.
01/29/01   tc      Merged GPS support.
11/29/00   jq      Moved defination of rxc_wake_type.slot out of BROADCAST_SMS.
10/04/00   jq      Added FTM FCH and SCH loopback feature
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
07/19/00   hrk     Moved member rev_pwr_cntl_delay in rxc_tci_cmd_type out of
                   FEATURE_FFPC.
07/17/00   va      Added support for rev pwr cntl delay.
07/14/00   rm      Added suuport for multiframe interleaving.
06/05/00   fc      Added rxc_olr_cmd_type.
06/02/00   lad     Changed order of rxc_frame_type enum for logging.
05/31/00   fc      Added command RXC_OLR_F.
05/26/00   jq      Added support for the new rxc design to support BCSMS
05/18/00   et      added support for dtx in sch
04/24/00   hrk     Added new transaction processing support.
04/12/00   hrk     Added prototype for rxc_send_rlp_frames_log().
04/10/00   dgy     Removed FEATURE_IS2000_QPCH.
04/07/00   va      Added RXC_RF_TUNE_F (HHO fix from PP)
03/27/00   sr      Changed the type of scc_energy[] from word to int2
03/24/00   bgc     Added enable_skip_tci in tci_cmd_type for PLT.
03/24/00   na      Added prototype for rxc_init_sch_mux().
03/16/00   na      Included prototypes for rxc_get_sch_info().
02/02/00   hrk     Merged with version GV40401 rxc.h from ASW archive:
           dna     Added timeout and is_hho as payload to the
                   RXC_RIF_HHO_OR_CFS_F command.
           aaj     Added FEATURE_DETECT_SUP_MUTING for MDR sup muting detection
01/31/00   rm      Clean up TEMP_HACKS_FOR_TIGER.
01/24/00   rm      Added in support for ACTION TIME processing.
12/17/99   va      Added transaction_num to rxc_pwr_ctrl_cmd_type,
                   Removed return_from_hho from rxc_returned_type interface.
12/13/99   hrk     Introduced new RXC-FFPC interface. Older implementation
                   of FFPC is featurized to FEATURE_FFPC_OLD
12/01/99   hrk     SOme FFPC parameters expanded to quadword range.
11/22/99   va      Expanded the following MC-RXC interfaces for the IS2000
                   support:
                   RXC_TCI_F
                   RXC_SO_F
                   RXC_RETURNED_F
                   RXC_HO_F (new)
                   RXC_PWR_CTRL_F (new)
                   RXC_FPC_SUBCHAN_GAIN_F (new)
                   The old RXC_HO_F has been replaced by RXC_HHO_BEGIN_F.
11/09/99   rm      Added decode type as part of RXC_SCH command to distinguish
                   between Convolutional / Turbo coding.
11/04/99   lcc     Corrected FFPC featurization in txc_tci_cmd_type.
                   Merged from GD4021:
           kmp     Mainlained the FEATURE_ENHANCED_STANDBY_II feature by
                   removing the #ifdef around the rxc_load_pc_isr() prototype.
           ry      Added extern declaration for rxc_flag_good_frame()
10/27/99   ajn     Added QPCH support
10/21/99   rm      Added new command interface RXC_SCCH_F to support MDR
                   for MSM5000.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Added support for F3 message control for PLT demod
           hrk     Added support for Outer Loop on F-FCH
           rm      Added new Interface for SCH.
09/17/99   lcc     Merged in new interface definitions for IS2000 and MSM5000
                   support from PLT archive:
                   1. Fixed problem in tci struct when building for MSM3000
                   2. Removed T_MSM5000PLT.
                   3. Made changes in TCI command to add RC parameter for
                      IS2000. Also  changes dec_tc_type to dec_fch_type.
08/26/99   aaj     Merged Panther Branch 3.1 to Mainline
08/25/99   aaj     Added new definition for sup channel SER threshold for RS2
08/09/99   ry      Combined FEATURE_IS95B_HARD_HO and FEATURE_IS95B_CFS into
                   FEATURE_IS95B_MAHHO
07/09/99   ry      Added RXC_RIF_HHO_OR_CFS_F and RXC_RETURNED_F
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added rxc_pc_fade_type enum for various fade types.
04/06/99   aaj     Removed functions prototypes for RXDSP clock changes.
                   Added function prototype for rxc_voc_xchg()
02/26/99   ejv     Added prototype for rxc_load_pc_isr, part of FEATURE_
                   ENHANCED_STANDBY_II.
02/03/99   aaj     Added protoype declarations for RXDSP clk change routines
01/15/99   aaj     Merged MDR branch to Panther mainline
11/20/98   aaj     Added support for Medium (High) data rate sup channels
11/03/98   abh     Changes for frame no. correction after frame offset change.
09/17/98   bgc     Added rxc_set_frame_offset().
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
09/06/96   gb      Added dec_date rate determination field to dec buffer type.
04/09/96   rdh     Added funct to return whether or not we're in Paging slot.
03/06/96   jca     Added RXC_PWR_REP_DELAY_F to rxc_cmd_name_type.
12/12/95   jca     Changed (there is no) rx_cmd_q to rxc_cmd_q.
08/10/95   gb      Added New Markov Service Options and made all service
                     options CAI_SO_*.
07/26/95   rah     Added rxc_reset_voc_init call.
07/13/95   gb      Changed mux option initialization.
06/14/95   gb      Added support for Rate set 2 service options.
08/09/94   jca     Added support for IS96/IS96A dual-capability vocoder.
03/24/94   gb      Added called interface to set isr.
06/24/92   jai     First Revision
10/10/90   rrd     created file

===========================================================================*/

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
#include "rxc.h"
#include "rxcmc_i.h"

//drivers
#include "dalint_1x.h"
#include "dec5000.h"
#include "deint_i.h"

//cp
#include "cai.h"
#include "caix.h"
#include "caix_i.h"
#include "mctrans_i.h"

//other
#include "qw.h"
#include "queue.h"
#include "cmd.h"
#include "mdrrlp_api.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
typedef struct
{
  uint8   spr_rate;     /* spreading rate in chip*/
  uint8   rnd_factor;   /* rounding factor 2 or 3 */
  uint8   rep_factor;   /* repeat factor: 1, 2 or 4 */
} rxc_chan_demod_config_param_type;

extern boolean rxc_sup_dec_int_enable;
extern boolean rxc_turbo_dec_enabled;

/* RXC Traffic channel identifiers */
typedef enum
{
  RXC_F_FCH,
  RXC_F_SCH,
  RXC_F_DCCH,
  RXC_F_PDCCH0,
  RXC_F_PDCCH1,
  RXC_F_RCCH,
  RXC_F_ACKCH,
  RXC_F_GCH0,
  RXC_F_GCH1
} rxc_tc_chan_id_type;

// Definitions for physical channels carrying signalling
typedef enum
{
  RXC_FOR_FCH  = 0x1,
  RXC_FOR_DCCH = 0x2,
  RXC_FOR_PDCH = 0x3

} rxc_dsch_type;

typedef enum {
  RXC_GOOD_FRAME    = 0,
  RXC_DTX_FRAME     = 1,
  RXC_ERASURE_FRAME = 2,
  RXC_NO_FRAME      = 3
} rxc_frame_type;

/* Indicates whether DCCH channel is good or bad quality  */
typedef enum
{
  RXC_GOOD_CHANNEL,
  RXC_POOR_CHANNEL
}
rxc_dcch_quality_type;

/* -----------------------------------------------------
** This represents the state of the mobile while it
** it is monitoring the Paging channel for Fade reports.
** ----------------------------------------------------- */
typedef enum
{
  CDMA_IDLE_FADE,
  CDMA_ACCESS_FADE
} rxc_pc_fade_type;

typedef enum
{
  RXC_GPS_STATE_NONE,
  RXC_GPS_STATE_STARTED,
  RXC_GPS_STATE_RETURNED
} rxc_gps_state_type;

#define RXC_BC_PEEK_BOUNDRY 7
/* the number of frames left before RXC stop peeking into a BC SMS slot for the
 * beginning of a Data Burst Message */
typedef struct
{
     byte sch_id;
     byte sch_init_setpt;
     byte sch_FER;
     byte sch_min_setpt;
     byte sch_max_setpt;
}  rxc_fpc_sch_params_type;

typedef struct
{
    byte for_sch_id;
    qword start_time;
     byte for_sch_duration;
    boolean for_sch_fer_rep;
    byte for_sch_rate; //This will eventually be an enum
}  rxc_sch_params_type;

typedef struct
{
     rxc_hdr_type hdr;
     byte transaction_num;
     byte num_for_schs_assigned;
     rxc_fpc_sch_params_type fwd_sch_params[2];  //Real size is same as above
     boolean fpc_incl;
     byte fpc_mode;
     boolean fpc_olpc_sch_master_incl;
     byte fpc_sec_chan;
     byte num_schs;
     rxc_sch_params_type  sch_params[2]; //Real size is as above
     boolean  fch_sch_setpt_thresh_incl;
     byte  fch_sch_setpt_thresh;
}  rxc_escam_cmd_type;

/* Supplemental Channel (SCH) Config Info */
typedef struct {
  cai_radio_config_type rc;
  cai_sch_rate_type     sch_rate;       // 1X, 2X, 4X, 8X, 16X
  cai_sch_coding_type   code_type;      // 0 - convolutional, 1 - turbo
} rxc_sch_info_type;

/* A union of all TC channels */
typedef struct {
  rxc_tc_chan_id_type chan_id; /* Type of channel */

  union {
    rxc_sch_info_type sch;
 } config;
} rxc_tc_chan_info_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Decoder Signalling Data buffer types */

/* Sync Channel buffer type */
typedef byte rxc_dec_sc_type [ DEC_96_BITS / 8 ];

/* Paging Channel buffer type */
typedef struct
{
  dec_pc_type bits;
  qword frame;
} rxc_dec_pc_type;

/* ------------------------------------------------------------------------------------ */
#define RXC_TOTAL_TC (DEC_192_BITS + DEC_96_BITS + DEC_48_BITS + DEC_24_BITS)

/* Traffic Channel buffer type */
typedef struct
{
  dec_fch_type bits;
  qword frame;
} rxc_dec_tc_type;

/* ------------------------------------------------------------------------------------ */

/* Type to be placed on rxc_dec_q */
typedef struct
{
  q_link_type    link;     /* queue link */
  rxc_state_type state;    /* State of RXC state machine when bits were
                                received */
  cai_data_rate_type dec_rate;      /* rate determination for F-FCH this frame  */
  #ifdef FEATURE_IS2000_REL_A_SVD
  cai_data_rate_type dcch_dec_rate; /* rate determination for F-DCCH this frame */
  #endif /* FEATURE_IS2000_REL_A_SVD */

rxc_dcch_quality_type  dcch_chan_quality;

  union
  {
    rxc_dec_sc_type sc_buf;  /* Bits received on Sync Channel */
    rxc_dec_pc_type pc_buf;  /* Bits received on Paging Channel */
    rxc_dec_tc_type tc_buf;  /* Bits received on Traffic Channel */
  } buf;
} rxc_dec_type;

/*-------------------------------------------------------------------------*/
/* Type for sending storing symbol error rates */
typedef struct
{
  dword frame_cnt;
  dword ser;
} rxc_sc_ser_type;

typedef rxc_sc_ser_type rxc_pc_ser_type;

extern boolean rxc_get_chan_tc_demod_config_param
(
  /* pointer to channel info (typically from a chan config msg) */
  rxc_tc_chan_info_type            *chan_info,

  /* pointer to struct to return info in */
  rxc_chan_demod_config_param_type *demod_params
);

extern void rxc_clear_isr (void);
  /* set the DEC_INT isr to NULL for now */

extern void rxc_set_pc_isr
(
  boolean use_sw_demback,
  cai_pch_rate_type pc_rate
);
  /* setup the DEC_INT isr as the paging isr */

extern void rxc_voc_xchg (void);
  /* Writes the decoded over the air voice bits to the vocoder */

/*===========================================================================

FUNCTION RXC_REGISTER_ISR

DESCRIPTION
  This function registers for the DEC DONE ISR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_register_isr
(
  dalint_1x_isr         isr_handler,
  dalint_1x_stack_size  stack_size
);

/*===========================================================================

FUNCTION RXC_LOAD_PC_ISR

DESCRIPTION
  This externally-called function installs the PC ISR and controls
  whether or not the decoder should have its state metrics
  cleared until the first frame in the slot.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_load_pc_isr ( boolean blank_frame );

/*===========================================================================

FUNCTION RXC_FLAG_GOOD_FRAME

DESCRIPTION
  This function is called by SRCH during a Return-If-Fail HHO, after
  verifying none of the HHO failure conditions is met.  This is to
  inform RXC that it's okay to start processing good frame.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXC will start processing good frame

===========================================================================*/
extern void rxc_flag_good_frame( void );

/*========================================================================

FUNCTION RXC_POWER_COMB_CONFIG

DESCRIPTION This function configures the power control puncturing position
            and PUNCT_POS_SEL based on Radio Configuration

DEPENDENCIES Power control and puncturing must be enabled before.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_power_comb_config
(
  deint_rc_type rc
);

/*========================================================================

FUNCTION RXC_SET_MAC_RND

DESCRIPTION This function sets the MAC Engine Rounding factors based on the
             spreading Rate.

DEPENDENCIES MAC Engine should be enabled.

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_set_mac_rnd
(
  deint_rc_type rc,
  dec_sch_rate_type sch_rate,
  dec_coding_type code_type
);

/*========================================================================

FUNCTION RXC_SERVICE_TRANS_CMD

DESCRIPTION
   This function services a group of transactions at action time

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxc_service_trans_cmd( rxc_cmd_msg_type *cmd_ptr );

/*========================================================================
FUNCTION RXC_FWD_SCH_TRANS_CMD

DESCRIPTION
   The fwd. SCH transaction callback

DEPENDENCIES
  None

SIDE EFFECTS
  None

RETURN VALUE
  None
========================================================================*/
void rxc_fwd_sch_trans_cmd( mctrans_action_packet_type *fwd_sch_packet );

/*========================================================================

FUNCTION RXC_DTX_DET_SCH

DESCRIPTION This function determines if the current frame is a DTX frame,
an erasure frame, or a full frame.

DEPENDENCIES None

RETURN VALUE enum, either RXC_GOOD_FRAME, RXC_DTX_FRAME, or RXC_ERASURE_FRAME


SIDE EFFECTS None

=========================================================================*/
rxc_frame_type rxc_dtx_det_sch( dec_rc_type rc, boolean crc_pass );

/*===========================================================================
FUNCTION      RXC_GET_AVERAGED_FER

DESCRIPTION   This function gets erasure frames for poor FER alarm.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
extern void rxc_get_averaged_fer
(
  mux1xdiag_fer_s_type *fer                          /* pointer to store FER data */
);

/*========================================================================

FUNCTION RXCRDA_DCCH_INIT

DESCRIPTION This function returns the initial threshole frame quality value.

DEPENDENCIES Shoud be called after the receiving of the ECAM

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
void rxcrda_dcch_init( void );

/*========================================================================

FUNCTION RXC_DCCH_QUALITY_DET

DESCRIPTION This function returns the quality of the channel to assist in
channel setup and maintenance.

DEPENDENCIES None

RETURN VALUE enum rxc_dcch_quality_type: RXC_GOOD_CHANNEL, RXC_POOR_CHANNEL

SIDE EFFECTS None

=========================================================================*/
rxc_dcch_quality_type rxc_dcch_quality_det
(
  cai_radio_config_type curr_rc,
  rxc_frame_type        rda_decision
);
#endif /*RXC_I_H*/
