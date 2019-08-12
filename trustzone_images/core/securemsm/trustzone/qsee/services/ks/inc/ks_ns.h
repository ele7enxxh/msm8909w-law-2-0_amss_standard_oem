#ifndef KS_NS_H
#define KS_NS_H
/**
   @file ks_ns.h
   Generic keystore interface to the NS world
*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ks/inc/ks_ns.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
04/05/13   pre      Initial version.

===========================================================================*/
#include <comdef.h>

/**
   Encrypt the Keystore with a key derived from the CE hardware key

   @param[in] keystore_ptr        Pointer to length of keystore
   @param[in] keystore_len        Length of keystore

   @return 0 on success, negative on failure
*/
int ks_ns_encrypt_keystore_ns
(
  uint32** ks_ptr,
  uint32*  ks_len
);

#endif /* KS_NS_H */
