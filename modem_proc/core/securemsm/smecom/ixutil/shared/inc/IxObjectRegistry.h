#ifndef __IXOBJECTREGISTRY_H__
#define __IXOBJECTREGISTRY_H__

/**
   @file IxObjectRegistry.h

   @brief 
  This header file defines the interface to IxObjectRegistry.
  This interface has methods for registering an object and for
  deregistering an object.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                  I X  O B J E C T  R E G I S T R Y
                     H E A D E R  F I L E

Copyright (c) 2008 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/shared/inc/IxObjectRegistry.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
04/11/11     nk      Added the C API's and debug enable methods.

===========================================================================*/
#include "IxErrno.h"

/*------------------------------------------------------------------------
                              DATA TYPES
------------------------------------------------------------------------*/
typedef void* REGISTRYHANDLE;

#ifdef __cplusplus

/**
   @brief The object map class is a placeholder and an associative map
   for registered objects. The object stores the registered object 
   pointer and an integer handle
*/
class IxObjectMap
{
   friend class IxObjectRegistry;

   private:
      IxObjectMap ()
      {
         m_handle  = 0;
         m_pObject = 0;
      }

      unsigned int m_handle;
      void        *m_pObject;
};

/**
   The MAX_OBJECTS constant below is used to prevent runaway registration
   of objects.
*/
#define IX_OBJECT_REGISTRY_MAX_OBJECTS 2048

/**
   The Default Object registry map size
*/
#define IX_OBJECT_REGISTRY_DEFAULT_MAP_SIZE 32

/**
   @brief The object registry class provides interface methods
   to register, unregister objects. It has methods to get integer
   handles of registered objects.
*/
class IxObjectRegistry
{

/*------------------------------------------------------------------------
                                  PUBLIC METHODS
 ------------------------------------------------------------------------*/

   public:

/**
   create an or get the instance handle of an 
   IxObjectRegistry object.

   @param
   None.

   @retval
   Returns a reference to IxObjectRegistry object.

   @dependencies 
   None.
*/
      static IxObjectRegistry* Instance ( void );

/**
         Register a specified object. If registration succeeds, a unique
         handle is returned using the pHandle parameter.

   @param[in]  pObject Pointer to the path string
   @param[out] pHandle Pointer to returned handle

   @retval
   Returns Error Code E_SUCCESS is returned if 
   there is no error.

   @dependencies 
   None.
*/
      IxErrnoType Register (void *pObject, unsigned int *pHandle);

/**
   Unregister a specified handle

   @param[in] handle handle to unregister

   @retval
   Returns Error Code E_SUCCESS is returned if 
   there is no error.

   @dependencies 
   None.
*/
      IxErrnoType UnRegister (unsigned int handle);

/**
         Checks if a specified handle is registered or not.

   @param[in] handle handle to check.

   @retval
   Returns TRUE if the specified handle is registered,
   FALSE otherwise.

   @dependencies 
   None.
*/
      bool IsRegistered (unsigned int handle);

/**
   Returns a pointer to an object that matches 
   the handle passed in.

   @param[in] handle handle to get the object reference.

   @retval
   Returns a pointer to the object whose handle matches 
   the input handle, or NULL if there is no match.

   @dependencies
   None.
*/
      void * GetObject (unsigned int handle);

/**
         Clears all registry information and frees all allocated memory.

   @param 
   None.

   @retval
   Returns Error Code E_SUCCESS is returned if 
   there is no error.

   @dependencies
   None.
*/
      IxErrnoType Clear( void );

/**
   Sets the internal debug message flag.

   @param[in] dflag Debug enable flag 
   None.

   @retval
   None.

   @dependencies
   None.
*/
void setDebug_Msg( unsigned char dflag ) { m_ordebug =  dflag; }

/**
   Gets the internal debug message flag value.

   @param[in] dflag Debug enable flag 
   None.

   @retval
   Returns
   0  when the debug message flag is not set
   1  when the debug message flag is set.

   @dependencies
   None.
*/
unsigned char getDebug_Msg() { return m_ordebug; }

   private:

      IxObjectMap *m_pMap;

      unsigned int m_mapSize;

      unsigned int m_nextHandle;

      unsigned int m_nRegisteredObjs;

unsigned char m_ordebug;

      /* Prevent direct construction, copy construction and assignment.
      */
      IxObjectRegistry ()
      {
         m_nextHandle      = 1;
         m_pMap            = 0;
         m_mapSize         = 0;
         m_nRegisteredObjs = 0;
         m_ordebug         = 0;
      }

      IxObjectRegistry (const IxObjectRegistry &);

      IxObjectRegistry& operator = (const IxObjectRegistry &);

      ~IxObjectRegistry () {}
};

#else


/**
   Definitions for "C" files. Since the C++ API cannot be called from a C file, we
   provide C wrappers. See the comments for the corresponding C++ methods for
   information on what each function does.
*/

/**
   call_IxObjectRegistry_Instance is a c call to create 
   an or get the instance handle of an IxObjectRegistry object.

   @param
   None.

   @retval
   Returns the pointer to an IxObjectRegistry object.

   @dependencies
   None.
*/
REGISTRYHANDLE call_IxObjectRegistry_Instance( void );

/**
   call_IxObjectRegistry_Register is a c call to register 
   an or get the interger handle of registered object.

   @param[in] REGISTRYHANDLE Pointer to IxObjectRegistry object.
   @param[in] pObject Pointer to object to be registered.
   @param[in,out] pHandle Pointer to integer handle 
                  for the registered object.

   @retval
   None.

   @dependencies
   None.
*/
IxErrnoType call_IxObjectRegistry_Register
(
REGISTRYHANDLE cRegistryHandle,
void *pObject, 
unsigned int *pHandle
);

/**
   call_IxObjectRegistry_UnRegister is a c call to un register 
   the previously registered object.

   @param[in] REGISTRYHANDLE Pointer to IxObjectRegistry object.
   @param[in] iHandle Integer handle for the registered object.

   @retval 
   None.

   @dependencies 
   None.
*/
IxErrnoType call_IxObjectRegistry_UnRegister
(
REGISTRYHANDLE cRegistryHandle, 
unsigned int iHandle
);

/**
   call_IxObjectRegistry_IsRegistered is a c call to check if 
   the object is registered or not.

   @param[in] REGISTRYHANDLE Pointer to IxObjectRegistry object.
   @param[in] iHandle Integer handle for the registered object.

   @retval
   None.

   @dependencies
   None.
*/
unsigned int call_IxObjectRegistry_IsRegistered
(
REGISTRYHANDLE cRegistryHandle, 
unsigned int iHandle
);

/**
   call_IxObjectRegistry_GetObject is a c call to get the real pointer 
   of the object that is already registered .

   @param[in] REGISTRYHANDLE Pointer to IxObjectRegistry object.
   @param[in] iHandle Integer handle for the registered object.

   @retval
   None.

   @dependencies
   None.
*/
void* call_IxObjectRegistry_GetObject
(
REGISTRYHANDLE cRegistryHandle,
unsigned int iHandle
);

/**
   C call to clear the object registry state

   @param[in] REGISTRYHANDLE Pointer to IxObjectRegistry object.

   @retval
   None.

   @dependencies
   None.
*/
IxErrnoType call_IxObjectRegistry_Clear
(
REGISTRYHANDLE cRegistryHandle
);

/**
   C call to set debug message .

   @param[in]  REGISTRYHANDLE Pointer to IxObjectRegistry object.

   @retval
   None.

   @dependencies
   None.
*/
void call_IxObjectRegistry_setDebug_Msg
(
REGISTRYHANDLE cRegistryHandle,
unsigned char dflag
);

/**
   C call to get debug status

   @param[in] REGISTRYHANDLE Pointer to IxObjectRegistry object.

   @retval
   None.

   @dependencies
   None.
*/
unsigned char call_IxObjectRegistry_getDebug_Msg
(
REGISTRYHANDLE cRegistryHandle
);

#endif /* __cplusplus */
#endif /* __IXOBJECTREGISTRY_H__ */
