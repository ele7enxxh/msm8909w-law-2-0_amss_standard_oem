#ifndef TZBSP_CRYPTO_H
#define TZBSP_CRYPTO_H

/**
@file tzbsp_crypto.h
@brief Lock the hardware so that only TZ can use it
       The crypto hardware is shared by the HLOS. This means that
       there are all kinds of race conditions if the hardware access
       is not properly managed. The HLOS must ask TZ to use the hardware
       before it can be used. TZ must also lock access to this resource
       if the hardware is used.
*/

/*===========================================================================
   Copyright (c) 2010-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/crypto/tzbsp_crypto.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/04/11   cap      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

 /**
 * @brief Intialize the HW crypto engine
 *
 * @param
 *
 * @return 0 on success, negative on failure
 *
 */
int tzbsp_hw_crypto_init(void);

/**
 * @brief Lock or unlock the crypto hardware by XPU protecting the resource.
 *        This function may do nothing based if the chipset supports dedicated
 *        TZ crypto engine.
 *
 * @param[in] lock  Set to TRUE to lock and FALSE to unlock
 *
 * @return TRUE if hardware can be locked; otherwise FALSE is returned.
 */
boolean tzbsp_crypto_lock_engine(boolean lock);

#endif
