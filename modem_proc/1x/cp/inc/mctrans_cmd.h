#ifndef MCTRANS_CMD_H
#define MCTRANS_CMD_H
/*===========================================================================

          MAIN CONTROL TRANSACTION COMMAND MANAGER MANAGER

GENERAL DESCRIPTION
  This is the interface for the MC transaction command type only.

DESCRIPTION
   This module has data structures to keep track of different transaction numbers
   and the transaction types they correspond to.

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

===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mctrans_cmd.h_v   1.4   15 Aug 2002 14:28:54   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mctrans_cmd.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/09   pk      1X Advanced Feature - added the action type for RCP txn
08/05/04   va      Increased max actions/per server/per trans to 8
07/29/04   ht      Added TDSO support for Rel C DV channels
07/19/04   lcc     Temporary changes to redefine MCTRANS_SRCH/TXC_SWITCH_ACTION
                   so that SRCH and TXC compiles correctly.
06/21/04   va      Fixed compile issue
06/21/04   va      Added TX_ON_OFF, RETURN, HHO_BEGIN, CFS_GPS_PREP actions
05/28/04   bkm/ll  Added MCTRANS_R_DV_PARAMS_ACTION.
02/10/04   az      Removed mctrans_cmd_type structure. Added support for new
                   Transaction Engine
01/05/04   bkm     Added TX_ON action to mctrans_action_type.
08/15/02   va      Increased MCTRANS_MAX_ACTION_IN_TRANS to 5
05/31/02   va      Added MCTRANS_ASET_ACTION for SRCH
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/05/01   lcc     Added MCTRANS_NULL_ACTION.
04/10/01   jq      Added actions to support P2/Control Hold
07/17/00   va      Added support for CFS and gating actions. Changed num of max
                   actions per transaction to 4.
06/22/00   bgc     Moved mctrans_cmd_type to its own header from mctrans.h
                   to simplify a circular include problem.
===========================================================================*/


#include "1x_variation.h"
#include "comdef.h"
#include "qw.h"

/*===========================================================================
                        SOME DEFINES
===========================================================================*/
#define MCTRANS_MAX_ACTION_IN_TRANS   8

// Actions that may be included in a transaction
typedef enum
{
  MCTRANS_FFPC_ACTION = 0 ,
  MCTRANS_RLGC_ACTION,
  MCTRANS_F_SCH_ACTION,
  MCTRANS_R_SCH_ACTION,
  MCTRANS_HO_ACTION,
  MCTRANS_SCR_ACTION,
  MCTRANS_CFS_NSET_ACTION,
  MCTRANS_CFS_CTRL_ACTION,
  MCTRANS_GATING_ACTION,
  MCTRANS_CTRL_HLD_GATING_ACTION,
  MCTRANS_ASET_ACTION,
  MCTRANS_F_DV_PARAMS_ACTION,
  /* Temporary change so that SRCH and TXC compiles */
  MCTRANS_SWITCH_ACTION,
  MCTRANS_SRCH_SWITCH_ACTION = MCTRANS_SWITCH_ACTION,
  MCTRANS_TXC_SWITCH_ACTION = MCTRANS_SWITCH_ACTION,
  /* End temporary change */
  MCTRANS_TX_ON_ACTION,
  MCTRANS_LCM_ACTION,
  MCTRANS_TX_ON_OFF_ACTION ,
  MCTRANS_HHO_BEGIN_ACTION ,
  MCTRANS_RETURN_ACTION,
  MCTRANS_CFS_GPS_PREP_ACTION ,
  MCTRANS_CFS_MEAS_ACTION,
  MCTRANS_R_DV_PARAMS_ACTION,
  MCTRANS_DV_TDSO_ACTION,
  #ifdef FEATURE_IS2000_1X_ADV
  MCTRANS_RCP_ACTION,
  #endif /* FEATURE_IS2000_1X_ADV */
  MCTRANS_NULL_ACTION,
  MCTRANS_MAX_ACTION
} mctrans_action_type;

// Don't change the order of following servers
typedef enum
{
  MCTRANS_SERVER_RXC = 0,
  MCTRANS_SERVER_TXC,
  MCTRANS_SERVER_SRCH,
  MCTRANS_SERVER_COUNT,
  MCTRANS_SERVER_NULL
} mctrans_server_token_type;

#endif /* MCTRANS_CMD_H */
