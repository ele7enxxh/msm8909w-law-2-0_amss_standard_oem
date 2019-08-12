#ifndef IXBASE_H
#define IXBASE_H

/**
   @file IxBase.h

   @brief 
   Generic defination of base class.

   Base class from which all the Ix classes are derived.
   It contains just pure virtual functions for AddRef and Release.

   Externalized Functions:
   AddRef,
   Release,
   GetState,
   SetState.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                                 I X  B A S E
                            H E A D E R  F I L E

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxBase.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $


when       who      what, where, why
--------   ---      ------------------------------------
01/11/11   leo      (Tech Pubs) Edited Doxygen comments and markup.
12/07/10   nk       Doxygenated.
10/21/05    gr       Created

===========================================================================*/

#include "comdef.h"

/** @addtogroup SMECOMUTIL_CLASS 
@{
*/

/**
   @brief Class to provide a virtual interface for dervied classes.

   IxBase provides the generic base class that is used for any other dervied class.
   The class provides get/set object state interfaces and add/release references.
*/
class IxBase {

public:

/**
   Adds a reference count so the destructor does not delete shallow copied
   objects.

   This is used if the client wants to retain multiple pointers in this object
   that are shared, but does not want to remove the underlying references this
   object is holding. This is used for preventing deletion of shallow copied
   objects.

   @return
   Returns the current reference count + 1.

   @dependencies
   None.

   @sa
   Release()
*/
virtual unsigned long AddRef( void ) = 0;

/**
   Releases the reference count. This is automatically called by the
   destructor, so it must be used with caution.

   This operation is not normally called by the client.

   @return
   Returns the current reference count - 1.

   @dependencies
   None.

   @sa
   AddRef()
*/
virtual unsigned long Release( void ) = 0;

/**
   Gets the internal serialized state or data representing the object.

   @param[out] stateDataPtr    Pointer to the serialized state.
   @param[out] stateDataLenPtr Pointer to the length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   SetState()
*/
virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr ) = 0;

/**
   Sets the internal serialized state or data for the object.

   @param[in] stateDataPtr Pointer to the serialized state.
   @param[in] stateDataLen Length of the serialized state.

   @return
   None.

   @dependencies
   None.

   @sa
   GetState()
*/
virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen ) = 0;

};

/** @} */  /* end_addtogroup SMECOMUTIL_CLASS */

#endif /* IXBASE_H */

