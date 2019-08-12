#ifndef DRX_CMD_H
#define DRX_CMD_H

/*===========================================================================

                         DRX Local Commands

GENERAL DESCRIPTION This file contains the description of local commands 
for DRX/Sleep.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) (2002-2007) by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/drxcmd.h_v   1.4   13 May 2002 18:23:16   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/drxcmd.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/08/05  gv/nd     Added code for WtoW BPLMN feature.
03/21/05    scm     Add BMC_SLEEP to drx_sleep_cmd_sender_enum.
03/15/05    scm     Add BMC_WAKE_UP to drx_sleep_cmd_sender_enum.
11/03/04    asr     Eliminated SEQ_PIPN DRX local command. It is subsumed by SEQ_PICH.
07/30/03    asr     New DRX_KICK_START command to kick start a new cycle.
09/18/02    gsc     Renamed RESERVED to DRXCMD_RESERVED in drx_sleep_cmd_sender_enum
05/12/02    asr     Added SEQ_PICH and SEQ_PIPN to drx_sleep_cmd_sender_enum
04/21/02    asr     Added DRX_TIMER_1_ISR to drx_sleep_cmd_sender_enum
02/21/02    asr     Added DRX_PICH_ISR to drx_sleep_cmd_sender_enum
02/31/02    asr     Created file


===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "l1def.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum {
    SRCH_SLEEP,
    SRCH_WAKE_UP,
    RF_WARM_UP,
    DL_SLEEP,
    DL_WAKE_UP,
#ifdef FEATURE_UMTS_BMC
    BMC_SLEEP,
    BMC_WAKE_UP,
#endif
#ifdef FEATURE_WTOW_BACKGROUND_PLMN_SEARCH
    SRCH_BPLMN,
#endif
    DRX_PICH_ISR,
    DRX_TIMER_1_ISR,
    SEQ_PICH,
    DRX_KICK_START,
    DRXCMD_RESERVED
} drx_sleep_cmd_sender_enum;

/*--------------------------------------------------------------------------
                        DRX SLEEP COMMAND
--------------------------------------------------------------------------*/

typedef struct
{
  l1_local_cmd_hdr_type hdr;
    /* generic command header */

  boolean success; // TRUE indicates success, FALSE failure.

  drx_sleep_cmd_sender_enum sender; // indicates who is sending this command

} drx_sleep_cmd_type;


#endif // DRX_CMD_H
