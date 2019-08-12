#ifndef IPCMACROS_H
#define IPCMACROS_H

/**
   @file ipcmacros.h

   @brief 
   Generic IPC Macro Services Header File.

   Externalized Functions:

   Initialization and sequencing requirements:
   None.
*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               I P C    M A C R O S    S E R V I C E S

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                     EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/ipcmacros.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/07/10   nk      Doxygenated.
04/27/06   gr      Fixed a problem with the definition of the STRUCT macro 
12/07/05   ws      Removed semicolons and implemented more review comments
12/02/05   ws      Implemented Review comments
11/29/05   ws      Initial Revision.

===========================================================================*/

/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/

#include <string.h>
#include <memory.h>
#include "rtedef.h"

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif

/** @addtogroup SMECOMUTIL_IPC 
    @{
*/

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
   Defines ipcmacros_custom_f_type.

   This type define is a type of function that is to be passed into the
   function macro CUSTOM.
*/
typedef void (ipcmacros_custom_f_type) (
void             *req,
uint32            a
);

/**
   Defines ipcmacros_custom_f_type.

  This type define is a type of function that is to be passed into the
  function macro RETURN_CUSTOM.
*/
typedef void (ipcmacros_return_custom_f_type) (
uint32            b,   
void             *rsp
);

/**
   @name IPC Callback Macros
   @{ */

/**
   Populates the IPC message ID and the IPC object handle.
*/
#define PREP_MSG( req, id, handle )                \
(req)->msg_id = (ipc_msg_id_type) (id);         \
(req)->object_handle = (uint32) (handle)

/**
   Populates the IPC variable with an 8-bit unsigned integer.
*/
#define UINT8( req, a ) \
(req)->a = (uint8) (a)

/**
   Populates the IPC variable with a 16-bit unsigned integer.
*/
#define UINT16( req, a ) \
(req)->a = (uint16) (a)

/**
   Populates the IPC variable with a 32-bit unsigned integer.
*/
#define UINT32( req, a ) \
(req)->a = (uint32) (a)

/**
   Populates the IPC variable with an 8-bit signed integer.
*/
#define INT8( req, a ) \
(req)->a = (int8) (a)

/**
   Populates the IPC variable with a 16-bit signed integer.
*/
#define INT16( req, a ) \
(req)->a = (int16) (a)

/**
   Populates the IPC variable with a 32-bit signed integer.
*/
#define INT32( req, a ) \
(req)->a = (int32) (a)

/**
   Populates the IPC variable with a type long.
*/
#define LONG( req, a ) \
(req)->a = (long) (a)

/**
   Populates the IPC variable with a 32-bit unsigned integer.
*/
#define ENUM( req, a ) \
(req)->a = (uint32) (a)

/**
   Populates the IPC variable with a type Boolean.
*/
#define BOOL( req, a ) \
(req)->a = (boolean) (a)

/**
   Populates the IPC variable with a character array.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/
#define CHAR_ARRAY( req, a, size ) \
(void) ipc_memcpy( (char*) (req)->a, (char*) (a), (uint32)(size) )

/**
   Populates the IPC variable with a character array or string.
   Internally, it performs an IPC string copy (ipc_strcpy).
*/
#define STRING( req, a ) \
(void) ipc_strcpy( (char*) (req)->a, (const char*) (a) )

/**
   Populates the IPC variable with a character array or constant string.
   Internally, it performs an IPC string copy (ipc_strcpy).
*/
#define CONST_STRING( req, a ) \
(void) ipc_strcpy( (char*) (req)->a, (const char*) (a) )

/**
   Populates the IPC variable with a structure array.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/
#define STRUCT_ARRAY( req, a, size ) \
(void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size)*sizeof( a ) )

/**
   Populates the IPC variable with a constant structure array.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/  
#define CONST_STRUCT_ARRAY( req, a, size ) \
(void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size)*sizeof( a ) )

/**
   Populates the contents of the IPC variable with a void pointer.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define POINTER( req, a ) \
(void) ipc_memcpy( (void*) &((req)->a), (void*) a, (size_t) sizeof(*a))

/**
   Populates the contents of the IPC variable with a typed pointer.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define TYPED_POINTER( req, a, size ) \
(void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size) )

/**
   Populates the contents of the IPC variable with a constant pointer.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define CONST_POINTER( req, a ) \
(void) ipc_memcpy( (void*) &((req)->a), (void*) a, (size_t) sizeof(*a))

/**
   Populates the contents of the IPC variable with a constant void pointer.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define CONST_VOID_POINTER( req, a, size ) \
(void) ipc_memcpy( (void*) (req)->a, (void*) (a), (size_t) (size) )

/**
   Populates the contents of the IPC variable with a 32-bit unsigned integer,
   which acts as an opaque pointer. 
*/ 
#define OPAQUE_PTR( req, a ) \
(req)->a = (uint32) (a)

/**
   Populates the contents of the IPC variable with a 32-bit unsigned integer,
   which acts as a constant opaque pointer. 
*/ 
#define CONST_OPAQUE_PTR( req, a ) \
(req)->a = (uint32) (a)

/**
   Populates the contents of the IPC variable with a typed opaque pointer.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define TYPED_OPAQUE_PTR( req, a, size ) \
(void) ipc_memcpy( (req)->a, (a), (size) )


/** @name Type ipcmacros_custom_f_type Functions
@{ */

/**
   Populates the contents of the IPC function pointer with 
   ipcmacros_custom_f_type. 
*/ 
#define CUSTOM( req, a, func ) \
func( (void*) (req), (uint32) (a) )

/**
   Populates the contents of the IPC structure of the specified type.
   Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define STRUCT( req, a ) \
(void) ipc_memcpy( (void*) &((req)->a), (void*) (&a), (size_t) sizeof( a ) )

/**
  VOID; not defined.
*/
#define VOID( req, a )

/**
  CALLBACK; not defined.
*/
#define CALLBACK( req, a )

/**
   Casts to Boolean and populates the IPC variable with Boolean data.
*/
#define RETURN_BOOL( b, rsp ) \
(b) = (boolean) (rsp)->b

/** @}  */ /* end_name Type ipcmacros_custom_f_type functions  */


/** @name Type ipcmacros_return_custom_f_type Functions
@{ */

/**
   Invokes the function with the specified ipcmacros_return_custom_f_type
   function pointer.
*/
#define RETURN_CUSTOM( b, rsp, func ) \
func( (uint32) (b), (void*) (rsp) )

/**
  RETURN_VOID; not defined.
*/
#define RETURN_VOID

/**
   Casts to an 8-bit unsigned integer and populates the IPC variable
   with uint8 data.
*/
#define RETURN_UINT8( b, rsp ) \
(b) = (uint8) (rsp)->b

/**
   Casts to a 16-bit unsigned integer and populates the IPC variable
   with uint16 data.
*/
#define RETURN_UINT16( b, rsp ) \
(b) = (uint16) (rsp)->b

/**
   Casts to a 32-bit unsigned integer and populates the IPC variable
   with uint32 data.
*/
#define RETURN_UINT32( b, rsp ) \
(b) = (uint32) (rsp)->b

/**
   Casts to an 8-bit signed integer and populates the IPC variable
   with int8 data.
*/
#define RETURN_INT8( b, rsp ) \
(b) = (int8) (rsp)->b

/**
   Casts to a 16-bit signed integer and populates the IPC variable
   with int16 data.
*/
#define RETURN_INT16( b, rsp ) \
(b) = (int16) (rsp)->b

/**
   Casts to a 32-bit signed integer and populates the IPC variable
   with int32 data.
*/
#define RETURN_INT32( b, rsp ) \
(b) = (int32) (rsp)->b

/**
   Casts to a 32-bit unsigned integer and populates the IPC variable
   with uint32 data.
*/
#define RETURN_ENUM( b, rsp ) \
(b) = (uint32) (rsp)->b

/**
   Populates the IPC return variable with character array data of a specified
   size. Internally, it performs an IPC memory copy (ipc_memcpy).
*/ 
#define RETURN_CHAR_ARRAY( b, rsp, size ) \
(void) ipc_memcpy( (void*) (b), (void*) (rsp)->b, (size_t) (size) )

/**
   Populates the IPC return variable with constant character pointer data.
   Internally, it performs an IPC string copy (ipc_strcpy).
*/
#define RETURN_STRING( b, rsp ) \
(void) ipc_strcpy( (char*) (b), (const char*) (rsp)->b )

/**
   Populates the IPC return variable with void pointer data.
   Internally it does an ipc memory copy (ipc_memcpy).
*/  
#define RETURN_POINTER( b, rsp, size ) \
(void) ipc_memcpy( (void*) (b), (void*) (rsp)->b, (size_t) (size) )

/**
   Casts to a 32-bit unsigned integer and populates the IPC variable with
   uint32 data. The variable acts like an opaque pointer.
*/
#define RETURN_OPAQUE_PTR( b, rsp ) \
(b) = (uint32) (rsp)->b

/** @} */ /* end_name Type ipcmacros_return_custom_f_type functions  */

/** @} */  /* end_name IPC Callback Macros*/

/** @} */  /* end_addtogroup SMECOMUTIL_IPC */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif /* IPCMACROS_H */
