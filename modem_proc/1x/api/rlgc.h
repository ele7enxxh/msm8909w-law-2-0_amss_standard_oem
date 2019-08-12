#ifndef RLGC_H
#define RLGC_H

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/rlgc.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/14   jh      remove qw.h include
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/18/10   jtm     Removed include of customer.h.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
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
#include "comdef.h"

//mux
#include "rxc.h"
#include "txc.h"
#include "txcmc.h"

//drivers
#include "enci.h"
#include "enc.h"
#include "deint.h"

//cp
#include "cai.h"
#include "caii.h"

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/


/*===========================================================================
FUNCTION RPC_GET_SCH_SCRM_RATE

DESCRIPTION
  For the given condition, this function returns the suitable R_SCH rate which
  can used in the SCRM.

DEPENDENCIES

RETURN VALUE
  R_SCH rate. A rate of ENC_NUM_SCH_RATES indicates invalid rate

SIDE EFFECTS
  None.
===========================================================================*/
enc_sch_rate_type rpc_get_sch_scrm_rate
(
  enc_sch_rate_type curr_sch_rate
);


/*===========================================================================
FUNCTION RPC_SET_SCRM_ALLOWED

DESCRIPTION
  Data Services requests available SCRM rate from 1x MUX via the function:

    rpc_get_sch_scrm_rate()

  This API allows the caller to trigger the above function to unconditionally
  return no SCRM is allowed when the is_scrm_allowed parameter is FALSE.  If
  the parameter passed is TRUE, the function will return to normal max SCRM
  rate determination based upon Tx power available.

  Note that the internal flag allowing SCRM will automatically be set to
  TRUE upon every new call origination.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void rpc_set_scrm_allowed( boolean is_scrm_allowed );

#endif /* RLGC_H */
