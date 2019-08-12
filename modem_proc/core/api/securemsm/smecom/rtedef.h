#ifndef RTEDEF_H
#define RTEDEF_H

/**
   @file rtedef.h

   @brief  defines common definations

   @detail
   This header file contains general types and macros that are of use
   to all modules.  

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                                R T E D E F
                           H E A D E R  F I L E


Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                         EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/rtedef.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/17/11   leo     Edited Doxygen comments and markup.
12/14/10   nk      Doxygenated.
09/30/10   nk      Removed RTE_EXPORT, not used anymore
02/06/09   qxu     fixed PACKED problem for MDM8200. 
05/22/08   avm     Reverting back to version 4, conditionally excluding targets
                   to avoid BREW version conflict needs to be revisited
10/04/06    rv     Fixed compilation errors
05/03/05   jct     Recreated from comdef.h

===========================================================================*/
/** @addtogroup SMECOMUTIL_COMDEF 
@{
*/

/** @name C ++ Guard Defines 
@{
*/

#define __CPLUSPLUS_GUARD_BEGIN  extern "C" {  /**< C++ guard begin. */
#define __CPLUSPLUS_GUARD_END    }             /**< C++ guard end.   */

/** @} */ /* end_name c plus plus guard */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif
/**
   @name RTE Standard Types
   @{
*/

/**
   Opaque address type.
*/
typedef void* rte_opaque_address_type; //{ Opaque address

/**
   Defines the possible status codes that can be returned from a method
   conforming to the conventions of the aIDunno interface. Normally,
   aIDunno-style methods should return a status code and send any results
   from the function callback to the caller via out parameters.
*/
enum rte_result_type
{
RTE_OK,             /**< Operation succeeded.                       */
RTE_FAIL,           /**< General failure.                           */
RTE_NO_INTERFACE,   /**< Requested interface not found.             */
RTE_NULL_POINTER,   /**< NULL pointer provided as an out parameter. */
RTE_NO_MEMORY,      /**< Memory allocation failure.                 */
RTE_UNAVAILABLE,    /**< Requested service is unavailable.          */
RTE_UNSUPPORTED     /**< Requested service is not supported.        */
};

/** @} */ /* end_namegroup */

/** @} */  /* end_addtogroup SMECOMUTIL_COMDEF */

#ifdef __cplusplus
/* Namespaces defines */
#ifdef __arm
#define NAMESPACE( ns )
#else
#define NAMESPACE( ns ) using namespace ns;
#endif

#endif

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif


#endif // RTEDEF_H
