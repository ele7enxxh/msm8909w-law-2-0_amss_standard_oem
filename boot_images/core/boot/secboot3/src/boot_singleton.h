#ifndef BOOT_SINGLETON_H
#define BOOT_SINGLETON_H

/*===========================================================================

                          Boot Singleton Header File

GENERAL DESCRIPTION
  This header file contains the definitions for boot singletons.
  
Copyright 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/src/boot_singleton.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/08   MJS     Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_comdef.h"
#include "boot_error_if.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#define BOOT_SINGLETON_DECLARE(prefix) \
extern void prefix##_register_singleton( prefix##_type *singleton ); \
extern prefix##_type *prefix##_get_singleton( void ); \
extern boolean prefix##_singleton_exists( void ); \
extern void prefix##_lock_singleton( void )


#define BOOT_SINGLETON_DEFINE(prefix) \
static prefix##_type *prefix##_singleton = NULL; \
static boolean prefix##_singleton_is_locked = FALSE; \
void prefix##_register_singleton( prefix##_type *singleton ) \
{ \
  BL_VERIFY( ( !prefix##_singleton_is_locked && singleton != NULL ), \
             BL_ERR_NULL_PTR ); \
  prefix##_singleton = singleton; \
} \
prefix##_type *prefix##_get_singleton( void ) \
{ \
  BL_VERIFY( prefix##_singleton != NULL, BL_ERR_NULL_PTR ); \
  return prefix##_singleton; \
} \
boolean prefix##_singleton_exists( void ) \
{ \
  return ( prefix##_singleton != NULL ); \
} \
void prefix##_lock_singleton( void ) \
{ \
  BL_VERIFY( prefix##_singleton != NULL, BL_ERR_NULL_PTR ); \
  prefix##_singleton_is_locked = TRUE; \
} \
extern prefix##_type *prefix##_get_singleton( void )


#endif /* BOOT_SINGLETON_H */

