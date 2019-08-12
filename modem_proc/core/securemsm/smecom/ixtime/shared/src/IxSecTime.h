#ifndef IXSECTIME_H
#define IXSECTIME_H

/*===========================================================================

                            
             I X S E C U R E   T I M E   H E A D E R    F I L E

GENERAL DESCRIPTION
  This header file should be used only when the FEATURE_SZEC_TIME is NOT 
  enabled on a target

EXTERNALIZED FUNCTIONS
  sectime_get_sys_time
    This function gets the current system time in milliseconds since the
    epoch. 

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2005, 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixtime/shared/src/IxSecTime.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
21/03/07    rv     Compiler errors
14/03/07    rv     Initial revision for targets without "sectime"

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#include "clk.h"

/*===========================================================================

FUNCTION      SECTIME_GET_SYS_TIME

DESCRIPTION
  This function gets the current system time in milliseconds since the
  epoch. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ret_val is filled with the system time.
===========================================================================*/

#define time_sec_get(ret_val) \
    clk_read_ms(*((qword*)ret_val));


/* all the unsuported functions */
void time_local_get (uint64 *ret_val)
{
    *ret_val = 0;
    return;
}

uint64 time_local_get_offset()
{
    return 0;
}

void time_user_get(uint64 *ret_val)
{
    *ret_val = 0;
    return;
}

void time_sec_set
(
   uint32                        new_sys_time,
   rex_tcb_type                  *task_ptr,
   rex_sigs_type                 wait_sig,
   void                          (*wait_handler)(rex_sigs_type)
)
{
    (void)new_sys_time;
    (void)task_ptr;
    (void)wait_sig;
    (void)wait_handler;
    return;
}

void time_local_set_offset
(
  int32                         offset,
  rex_tcb_type                  *task_ptr,
  rex_sigs_type                 task_wait_sig,
  void                          (*task_wait_handler)(rex_sigs_type)
)
{
    (void)offset;
    (void)task_ptr;
    (void)task_wait_sig;
    (void)task_wait_handler;
    return;
}

void time_user_set
(
  uint32                        new_user_time,
  rex_tcb_type                  *task_ptr,
  rex_sigs_type                 wait_sig,
  void                          (*wait_handler)(rex_sigs_type)
)
{
    (void)new_user_time;
    (void)task_ptr;
    (void)wait_sig;
    (void)wait_handler;
    return;
}



#endif /* IXSECTIME_H */

