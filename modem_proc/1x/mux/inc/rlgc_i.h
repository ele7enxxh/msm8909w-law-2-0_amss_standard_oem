#ifndef RLGC_I_H
#define RLGC_I_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  REVERSE LINK POWER CONTROL PROCESSING

GENERAL DESCRIPTION
  This module is responsible for low level processing of data received
  from the base station in CDMA mode of operation for Reverse Link
  Power Control.

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/rlgc.h_v   1.8   20 Aug 2002 16:51:40   baon  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/rlgc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/13   eye     Unifying Device only DTX for RC3/4 and RC8.
08/02/12   srk     Moved RF_NON_LINEAR_BIAS_CORRECTION from rlgc.c
02/24/12   srk     Mainlined FEATURE_IS2000_SCH and FEATURE_IS2000_P2
11/22/11   srk     Feature clean up: Mainlined and Internalized Features
09/28/11   srk     Mainlined Always On features and removed Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/28/11   vks     Remove unnecessary ftm include.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
10/30/10   jtm     Modem 1x SU API changes.
04/29/10   trc     Add rpc_set_scrm_allowed() to allow external SCRM
                   inhibition based upon arbitrary parameters (e.g. thermal)
04/12/10   jtm     Allow the use of sch_rc when FEATURE_IS2000_1X_ADV is defined.
02/04/10   jtm     Removed FEATURE_PLT. Created txc_rlgc_data_type.
11/07/09   vks     Only return the correct resolution for tx_rate_adj and not
                   set the mdsp register. Renamed rpc_set_tx_rate_adj() to
                   rpc_calculate_tx_rate_adj() to better reflect the
                   functionality.
08/22/09   jtm     Lint Fixes.
08/05/09   jtm     Lint fixes.
07/06/09   jtm     Changed rpc_rcpm_cmd to accept no parameters.
04/08/09   jtm     Initial 1X Advanced feature support.
04/01/09   jtm     Added customer.h and featurized necessary includes.
03/20/09   jtm     Additional headers included.
07/21/08   jtm     Removed FEATURE_IS2000_REL_D_DV and FEATURE_IS2000_REL_C_DV code.
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
06/09/06   trc     Remove T_MSM6700 featurization
07/25/05   rkc     Fixed lint errors.
07/08/05   bn      Moved the function rpc_reach_rate_word_satisfies_gain_limit to txcmc.h
10/20/04   ljl     Updated for Rev D channels.
09/16/04   jrp     Added MAC support.
09/01/04   jrp     Allows for DTX of R-REQCH when writing gain ratios.
07/28/04   jrp     Added interface for programming linear gains to mDSP.
07/28/04   jrp     Removed interfaces that do not need to be exported.
06/24/03   jrp     Added rpc_set_tx_rate_adj() to conditionally convert from
                   1/12dB units to 1/10dB units before writing to TX_RATE_ADJ.
04/07/03   jrp     Lint changes.
08/20/02   bn      Added funtion prototype that supports new SCRM rate logic
04/03/02   jrp     Added rpc_rate_word_satisifes_gain_limit(),
                   rpc_get_reach_pilot_gain(), and rpc_get_reach_data_gain().
06/15/01   day     Mainlined FEATURE_RLGC
06/12/01   vlc     Added new value for Max_Num_Of_High_Rates.  Added
                   RPC_32X_RATE (inside FEATURE_32X_SCH_DATA_RATE.)
04/26/01   vlc     Changed name of a few constants to accomodate new SCH RC4
                   development on MSM5100.
04/20/01   sr      Merged in from MSM5100 archive
04/10/01   hrk     Added support for FEATURE_IS2000_P2
11/22/00   hrk     Removed inclusion of err.h and msg.h
09/19/00   hrk     Added prototype for rpc_plt_get_rl_gain_adj (),
                   rpc_plt_get_fch_rate (), rpc_fch_gating_cmd().
09/06/00   bgc     Added FEATURE_FACTORY_TESTMODE.
08/24/00   hrk     Mainlined FEATURE_CS_ACTION_TIME_TRANS.
06/09/00   hrk     Initial revision.

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
#include "rxc.h"
#include "rlgc.h"
#include "txc.h"
#include "txcmc.h"
#include "txcmc_i.h"

//drivers
#include "enci.h"
#include "enci_i.h"             /* Encoder register & macro definitions */
#include "enc.h"
#include "deint.h"

//cp
#include "cai.h"
#include "caii.h"
#include "mctrans_i.h"

//other
#include "qw.h"

/*===========================================================================
                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

typedef struct
{
  int16  nom_att_gain;
  byte  pilot_ref_lvl;
} rl_att_gain_type;

#ifdef FEATURE_IS2000_REL_A_CC
/* Gain values output to MDSP has 15 bit resolution as result of TX
** gain enhancement on Dora target.
*/
typedef struct
{
  word pch_gain;
  word each_gain;
} rpc_reach_gain_type;

extern const int rpc_nominal_RCCH_attribute_gain_table[];
#endif // FEATURE_IS2000_REL_A_CC


typedef struct
{
  /********************/
  /* Enabled Channels */
  /********************/

  boolean fch_on;
  boolean dcch_on;
  boolean sch_on;

  #ifdef FEATURE_IS2000_1X_ADV
  boolean fch_ack_on;
  boolean sch_ack_on;
  #endif /* FEATURE_IS2000_1X_ADV */

  /*********************/
  /* RLGAIN Parameters */
  /*********************/

  int8 rlgain_traffic_pilot; /* unit : 0.125 dB */
  int8 rlgain_sch0_pilot;

  /**********************************/
  /* Channel Adjustments Parameters */
  /**********************************/

  int8 fch_chan_adj_gain;    /* unit : 0.125 dB */
  int8 dcch_chan_adj_gain;
  int8 sch0_chan_adj_gain;
  int8 sch1_chan_adj_gain;

  #ifdef FEATURE_IS2000_1X_ADV
  int8 fch_ack_chan_adj_gain;
  int8 sch_ack_chan_adj_gain;
  #endif /* FEATURE_IS2000_1X_ADV */

  /***************************************/
  /* Multichannel Adjustments Parameters */
  /***************************************/

  int16 fch_multi_chan_adj_gain;   /* valid only if sch_on is TRUE */
  int16 dcch_multi_chan_adj_gain;  /* valid only if sch_on is TRUE */
  int16 sch_multi_chan_adj_gain;
  #ifdef FEATURE_IS2000_1X_ADV
  int16 fch_ack_multi_chan_adj_gain;  /* valid only if sch_on is TRUE */
  int16 sch_ack_multi_chan_adj_gain;  /* valid only if sch_on is TRUE */
  #endif /* FEATURE_IS2000_1X_ADV */

  /*********************/
  /*   FCH Parameters  */
  /*********************/

  cai_radio_config_type fch_rc;
  enc_rate_set_type  fch_rate_set;
  boolean rev_fch_gating_on;  /* TRUE indicates R-FCH 1/8 gating is ON/OFF */

  /*********************/
  /*  DCCH Parameters  */
  /*********************/

  cai_radio_config_type dcch_rc;
  enc_rate_set_type  dcch_rate_set;

  /*********************/
  /*   SCH Parameters  */
  /*********************/

  cai_radio_config_type sch_rc;  /* R-SCH rc.    valid only if sch_on is TRUE */
  cai_sch_coding_type sch_code_type;      /* TURBO or CONVO. valid only if sch_on is TRUE */
  enc_sch_rate_type sch_rate; /* R-SCH rate.     valid only if sch_on is TRUE */

} txc_rlgc_data_type;

/* 2x, 4x, 8x, 16x, 32x (32x has its own feature) */
#define Max_Num_Of_High_Rates     5

/* 0 : Convolutional; 1 : Turbo */
#define Max_Num_Of_Coding_Types   2

/* First entri in rpc_gain_adj_tab corresponds to PCH gain of 30 */
#define TABLE_OFFSET    30

#define RPC_32X_RATE  ((uint32) ENC_SCH_32X_RATE - (uint32) ENC_SCH_2X_RATE)
#define RPC_16X_RATE  ((uint32) ENC_SCH_16X_RATE - (uint32) ENC_SCH_2X_RATE)
#define RPC_8X_RATE   ((uint32) ENC_SCH_8X_RATE  - (uint32) ENC_SCH_2X_RATE)
#define RPC_4X_RATE   ((uint32) ENC_SCH_4X_RATE  - (uint32) ENC_SCH_2X_RATE)
#define RPC_2X_RATE   ((uint32) ENC_SCH_2X_RATE  - (uint32) ENC_SCH_2X_RATE)


/* Total digital baseband power = 255
*/
#define RPC_TOT_DIG_PWR_255  255

/* Reduce pilot channel digital gain in from 15 to 8 bit resolution.  The 8
** bit resolution digital gain value is used to calculate TX Rate Adjust.  The
** 15 bit resolution digital gain value is used to output to MDSP.
*/
#define PCH_GAIN_TRUNCATE_TO_8BITS(gain_15bits) ((byte) ((gain_15bits) >> 7))

/* Part of the TX gain enhancements is to replace the dB2lin lookup table with
** calls to convert_dbq8_to_lin() for better gain ratio accuracy.  This
** change results in higher precision with proper Q format.
** Under normal operations, call this function with bias of -0.0089 (-2 in Q8)
** to remove any positive bias inherent in the dB2lin function due to
** RF non-linear effects.
*/
#define RF_NON_LINEAR_BIAS_CORRECTION -2

#ifdef FEATURE_IS2000_1X_ADV
typedef enum
{
  RACK_CHAN_MOD_BPSK,
  RACK_CHAN_MOD_OOK,
  NUM_RACK_CHAN_MOD_TYPES
} rack_chan_mod_type;

typedef enum
{
  RLGC_R_FCH_ACK,
  RLGC_R_SCH0_ACK,
  RLGC_R_SCH1_ACK,

  RLGC_NUM_RACK_CHANNELS = 2

  /* There are really 3 channels
     RLGC_R_FCH_ACK (FCH), RLGC_R_SCH0_ACK (SCH0), RLGC_R_SCH1_ACK (SCH1)
     We only use FCH and SCH0 */
 } rlgc_rack_chan_types;

typedef enum {
  TXC_ACS_IS_1,
  TXC_ACS_IS_2_OR_MORE,
  TXC_NUM_ACS_TYPES
} txc_active_cell_size_types;

#endif /* FEATURE_IS2000_1X_ADV */
/* <EJECT> */

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_TC_CMD

DESCRIPTION
  Performs initialization for Reverse Link Gain Control processing.

DEPENDENCIES
  Rate Set should be initialized in txc_so before invoking this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_tc_cmd (txc_cmd_type  *cmd_ptr);

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_RELATIVE_GAINS

DESCRIPTION
  Returns the adjusted gain value for PCH.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_get_relative_gains
(
  int16 *fch2pich,
  int16 *dcch2pich,
  int16 *sch02pich,
  enc_rate_type rate,
  txc_chan_status_type sch_chan_status,
  txc_chan_status_type dcch_chan_status
);

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================
FUNCTION RPC_RCPM_CMD

DESCRIPTION
  This function process the reception of new RCPM data from MC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_rcpm_cmd ( void );
#endif /* FEATURE_IS2000_1X_ADV */

/* <EJECT> */
/*===========================================================================
 FUNCTION RPC_TRANS_CMD

DESCRIPTION
  This function process the MCTRANS_RLGC_ACTION_CMD from MC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_trans_cmd (mctrans_action_packet_type *cmd_ptr);

/* <EJECT>^L */
/*===========================================================================
 FUNCTION RPC_FCH_GATING_MODE

DESCRIPTION
  If the R-FCH 1/8 Rate gating mode changes, this function re-calculates the
  gains.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void
rpc_fch_gating_cmd
(
 boolean rev_fch_gating_on
);

#ifdef FEATURE_IS2000_REL_A_CC

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_REACH_PILOT_GAIN

DESCRIPTION
  Returns the R_PICH digital gain that should be used with the R-EACH
  for the speicified rate_word.

DEPENDENCIES

RETURN VALUE
  Digital R-PICH gain.

SIDE EFFECTS
  None.
===========================================================================*/
word
rpc_get_reach_pilot_gain
(
  txc_each_rate_type rate_word
);


/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_REACH_DATA_GAIN

DESCRIPTION
  Returns the R_EACH digital gain that should be used for the speicified
  rate_word.

DEPENDENCIES

RETURN VALUE
  Digital R-EACH gain.

SIDE EFFECTS
  None.
===========================================================================*/
word
rpc_get_reach_data_gain
(
  txc_each_rate_type rate_word
);

#endif // FEATURE_IS2000_REL_A_CC

int16
rpc_get_dcch2pch_gain_ratio
(
 boolean dcch_only
);

/*===========================================================================
FUNCTION RPC_CALCULATE_TX_RATE_ADJ

DESCRIPTION
  This function converts from 1/12dB to 1/10dB resolution if necessary.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
word  rpc_calculate_tx_rate_adj
(
  int16  tx_adj
);

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_SCH2PICH_GAIN_RATIO

DESCRIPTION
  This function computes the gain ratio between R-SCH and R-PICH.
  See section 2.1.2.3.3.7 of the rev D physical layer standard.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  SCH/PICH Gain Ratio.

SIDE EFFECTS
  None.
===========================================================================*/
int2 rpc_get_sch2pich_gain_ratio
(
 enc_sch_rate_type rate,
 cai_sch_coding_type code_type
);

/* <EJECT> */
/*===========================================================================
FUNCTION RPC_GET_PCH_ADJ_GAIN

DESCRIPTION
  Returns the adjusted gain value for PICH.
  This is programmed as TX_RATE_ADJ to correct for the PICH gain not equal
  to 256.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
int16 rpc_get_pch_adj_gain
(
  byte pch_gain
);

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================
FUNCTION RPC_GET_FCH_ACK2PICH_GAIN_RATIO

DESCRIPTION
  This function computes the gain ratio between R-ACKCH and R-PICH for the R-FCH.
  See section 2.1.2.3.3.7 of the rev E physical layer standard.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  ACKCH/PICH Gain Ratios for the R-ACK Channel for R-FCH.

SIDE EFFECTS
  None.
===========================================================================*/
int2 rpc_get_fch_ack2pich_gain_ratio ( txc_active_cell_size_types acs );

/*===========================================================================
FUNCTION RPC_GET_SCH_ACK2PICH_GAIN_RATIO

DESCRIPTION
  This function computes the gain ratio between R-ACKCH and R-PICH for the R-SCH.
  See section 2.1.2.3.3.7 of the rev E physical layer standard.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  ACKCH/PICH Gain Ratios for the R-ACK Channel for R-SCH.

SIDE EFFECTS
  None.
===========================================================================*/
int2 rpc_get_sch_ack2pich_gain_ratio ( txc_active_cell_size_types acs );
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================
FUNCTION RPC_MEAN_CODE_CHAN_OUTPUT_POWER

DESCRIPTION
  Computes the Mean Code Channel Output Power.

DEPENDENCIES
  This function should be called after RLGC parameters have been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_mean_code_chan_output_power(void);
#endif /* RLGC_I_H */
