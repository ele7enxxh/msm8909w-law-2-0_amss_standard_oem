#ifndef DRX_CMD_H
#define DRX_CMD_H

/*===========================================================================

                         DRX Local Commands

GENERAL DESCRIPTION This file contains the description of local commands 
for DRX/Sleep.
  
EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

Copyright (c) (2002-2007) by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/drxcmd.h_v   1.4   13 May 2002 18:23:16   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdrxcmd.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
11/08/05  gv/nd     Added code for WtoW BPLMN feature.
03/21/05    scm     Add TDSDRXCMD_BMC_SLEEP to tdsdrxcmd_sleep_sender_enum.
03/15/05    scm     Add TDSDRXCMD_BMC_WAKE_UP to tdsdrxcmd_sleep_sender_enum.
11/03/04    asr     Eliminated SEQ_PIPN DRX local command. It is subsumed by TDSDRXCMD_SEQ_PICH.
07/30/03    asr     New TDSDRXCMD_KICK_START command to kick start a new cycle.
09/18/02    gsc     Renamed RESERVED to TDSDRXCMD_RESERVED in tdsdrxcmd_sleep_sender_enum
05/12/02    asr     Added TDSDRXCMD_SEQ_PICH and SEQ_PIPN to tdsdrxcmd_sleep_sender_enum
04/21/02    asr     Added TDSDRXCMD_TIMER_1_ISR to tdsdrxcmd_sleep_sender_enum
02/21/02    asr     Added TDSDRXCMD_PICH_ISR to tdsdrxcmd_sleep_sender_enum
02/31/02    asr     Created file


===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "tdsl1def.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
typedef enum {
    TDSDRXCMD_RF_WARM_UP,
    TDSDRXCMD_SRCH_BPLMN,
    TDSDRXCMD_RESERVED
} tdsdrxcmd_sleep_sender_enum;

/*--------------------------------------------------------------------------
                        DRX SLEEP COMMAND
--------------------------------------------------------------------------*/

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
    /* generic command header */

  boolean success; // TRUE indicates success, FALSE failure.

  tdsdrxcmd_sleep_sender_enum sender; // indicates who is sending this command

} tdsdrxcmd_sleep_cmd_type;


#endif // DRX_CMD_H
