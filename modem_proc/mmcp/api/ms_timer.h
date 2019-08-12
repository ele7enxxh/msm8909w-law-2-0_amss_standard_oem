#ifndef MS_TIMER_H
#define MS_TIMER_H
/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/ms_timer.h_v   1.3   31 Oct 2001 13:02:30   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/ms_timer.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/01   jca     Fixed Lint errors.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "nas_exp.h"
#include "timers.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*******************************************************************************
 *
 * Message Ids
 *
 ******************************************************************************/

#define MID_START_TIMER_REQUEST      0x01
#define MID_MODIFY_TIMER_REQUEST     0x02
#define MID_TIMER_EXPIRY             0x03
#define MID_TIMER_ENQUIRY            0x04

/*******************************************************************************
 *
 * Message Structures
 *
 ******************************************************************************/

/*
 * MID_TIMER_EXPIRY
 */

typedef struct timer_expiry_S
{
   IMH_T                               IMH;
   timer_id_T                          timer_id;
   byte                                data;
} timer_expiry_T;


#endif /* MS_TIMER_H */

