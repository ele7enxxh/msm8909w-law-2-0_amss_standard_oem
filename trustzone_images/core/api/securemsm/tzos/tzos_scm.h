#ifndef TZOS_SCM_H
#define TZOS_SCM_H

/**
@file TZOS_SCM.h
@brief Trustzone Secure Channel Manager

The Secure Channel Manager is used to communicate between HLOS and TZOS.
This file prototypes the Secure Channel Manager on TZOS.  It
accomplishes this by means of a control queue which is a shared
memory buffer between HLOS SCM and TZOS SCM. All methods in this file
are synchronous.

Contains the public methods and related data types.

*/
/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_scm.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
02/16/10   ssm      Created

===========================================================================*/


/*===========================================================================

          INCLUDE FILES

============================================================================*/

#include "comdef.h"
#include "IxErrno.h"
#include "tzos_scm_defs.h"


/*===========================================================================

          PUBLIC DATA TYPES

============================================================================*/

/*------------------------------------------------------------------------
                              PUBLIC METHODS
------------------------------------------------------------------------*/

/*!
* @brief
   Method to notify HLOS that a response has been placed in MSG_Q

* @details
  This method is used to notify the HLOS that a response to a
  ReqCmd msg has been placed in the MSG_Q

  @param[in] cmd        - Command for which response is being sent
  @param[in] session_id - Session for the response msg
  @param[in] user_data  - User data which allows HLOS SCM to map
                          requests to responses

  @dependencies
   None

  @sideeffects
   None.

  @return
   None.
*/
IxErrnoType tzos_scm_send_rsp ( tzos_scm_cmd_enum_type   cmd,
                                uint32                   session_id,
                                uint32                   user_data );


#endif /* TZOS_SCM_H */
