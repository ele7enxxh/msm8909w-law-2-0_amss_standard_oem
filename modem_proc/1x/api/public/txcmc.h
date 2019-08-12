#ifndef TXCMC_H
#define TXCMC_H
/*===========================================================================

          T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the transmit task that don't fit
  into one of the other H files such as CAI.H

  Copyright (c) 2005 - 2013 Qualcomm Technologies, Inc. 
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/txcmc.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/14   jh      remove qw.h include
08/28/09   adw     Removed unnecessary inclusion of private rfnv.h header.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
04/08/09   adw     Added temporary CMI-lite featurization.
04/02/09   jtm     Removed featurization.
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
#include "comdef.h"

//1X
#include "cai.h"
#include "caii.h"  //IS2000 change
#include "mc.h"
#include "enc.h"

//other
#include "queue.h"
#include "cmd.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  TXC_EACH_RATE_9600_20_V = 0,         /* 9600 bps,  20 ms frame */
  TXC_EACH_RATE_19200_20_V,            /* 19200 bps, 20 ms frame */
  TXC_EACH_RATE_19200_10_V,            /* 19200 bps, 10 ms frame */
  TXC_EACH_RATE_38400_20_V,            /* 38400 bps, 20 ms frame */
  TXC_EACH_RATE_38400_10_V,            /* 38400 bps, 10 ms frame */
  TXC_EACH_RATE_38400_5_V              /* 38400 bps,  5 ms frame */
} txc_each_rate_type;


#endif /* TXC_H */

