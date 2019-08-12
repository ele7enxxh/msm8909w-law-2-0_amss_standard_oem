#ifndef FTM_TASK_H
#define FTM_TASK_H
/*===========================================================================

           F T M    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/ftm_task.h#1 $
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/16/12   ans/ars Changed messages to MSG_# interface to capture logs in crash 
                   dumps.  
01/06/10   sar     Migrated most definitions to new file, rf_test_task_v.h.
01/31/03   xw      created
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "rex.h"

#define FTM_SIG_WAIT_SIG2      0x00010000    /* Used generic for ftm_sig_rex_wait    */

#define FTM_MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define FTM_MSG_ERROR(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define FTM_MSG_HIGH(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define FTM_MSG_MED(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_MED, x_fmt, a, b, c)

#define FTM_MSG_LOW(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_FTM, MSG_LEGACY_LOW, x_fmt, a, b, c)

extern rex_tcb_type     ftm_tcb;

extern void ftm_sig_rex_set ( rex_sigs_type mask );

extern rex_sigs_type ftm_sig_rex_wait ( rex_sigs_type mask );

extern void ftm_free (void *mem_ptr);

#endif /* #ifndef FTM_TASK_H */
