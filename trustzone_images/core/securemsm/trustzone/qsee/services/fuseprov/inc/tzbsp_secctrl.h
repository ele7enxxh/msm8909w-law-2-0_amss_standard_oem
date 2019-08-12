#ifndef TZBSP_SECCTRL_H
#define TZBSP_SECCTRL_H

/**
@file tzbsp_secctrl.h
@brief TZ security level/state
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM Technology, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/29/14    hw      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

typedef struct
{
  uint32 value[2];
} tzbsp_secctrl_secure_status_t;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/**
 * check security status on the device
 *
 * @param [out] status   the security status returned from the call.
 *
 * @return 0 on call success
 */
int tz_get_secure_state(tzbsp_secctrl_secure_status_t* state);
#endif /* TZBSP_SECCTRL_H */
