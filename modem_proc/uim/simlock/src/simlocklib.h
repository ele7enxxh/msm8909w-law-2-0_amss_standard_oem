#ifndef SIMLOCKLIB_H
#define SIMLOCKLIB_H
/*===========================================================================


            S I M L O C K   L I B R A R Y   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     -----------------------------------------------------------
02/14/14   vv      Initial revision
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "comdef.h"

#ifdef FEATURE_SIMLOCK

#include "simlock_common.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_PROCESS_MESSAGE

DESCRIPTION
  This is the function which processes the requests to the SIMLock feature. It
  is the entry point to the various requests the SIMLock feature supports

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE  
===========================================================================*/
simlock_result_enum_type simlock_process_message(
  const simlock_message_request_data_type * msg_req_ptr,
  simlock_message_response_data_type      * msg_resp_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCKLIB_H */