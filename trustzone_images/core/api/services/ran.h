#ifndef RAN_H
#define RAN_H
/*===========================================================================

         P S E U D O - R A N D O M    N U M B E R    S E R V I C E S

DESCRIPTION
  This file contains declarations for use with the Pseudo-random Number
  Services.

Copyright (c) 1991, 1992 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1998,1999  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/ran.h_v   1.0   Aug 21 2000 11:51:08   lpetrucc  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/ran.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/23/04   ty      Added function prototype for ran_init()
12/16/98   jct     Removed 80186 support
09/21/98   jct     Updated copyright, now includes comdef.h
09/01/98   jct     Added cdecl declaration
05/06/91   rdb     Created module.

===========================================================================*/

#include "comdef.h"

#if(defined FEATURE_L4 || defined FEATURE_BLAST || defined FEATURE_BLAST_DEMAND_PAGE || defined FEATURE_BMPOS_QNK) 
#include "rex.h"
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern void   ran_init ( rex_tcb_type  *task_ptr,
                         rex_sigs_type task_wait_sig,
                         void (*task_wait_handler)(rex_sigs_type) );
extern void   ran_seed ( dword seed );
extern dword  ran_next ( void );
extern word   ran_dist ( dword  ran_num, word lo_val, word hi_val );

extern word   hash     ( dword hash_key, word  n, word  decorr );

#endif /* RAN_H */

