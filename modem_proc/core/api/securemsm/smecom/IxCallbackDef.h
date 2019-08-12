#ifndef IXCALLBACKDEF_H
#define IXCALLBACKDEF_H

/**
   @file  IxCallbackDef.h

   @brief 
   Interface definition for generic callback header class.

   This header file defines the interface for Generic callback header. 
   This may be used for various interfaces where memory allocation for 
   callbacks is prohibitive.

   The common handlers are
   pfnCancel - Filled by the handler. It can be called at any time to cancel the callback
   pfnNotify - Filled by the caller.  It is called when the callback is issued.
   pNotifyData - Filled by the caller.  It is passed to the callback.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================

                     I X  C A L L B A C K D E F
                        H E A D E R  F I L E

Copyright (c) 2010-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxCallbackDef.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/11   leo     Edited Doxygen comments and markup.
12/10/10   nk      Doxygenated.

===========================================================================*/

#include "comdef.h"
#include "IxErrno.h"
#include "rtedef.h"

#ifdef __cplusplus
__CPLUSPLUS_GUARD_BEGIN
#endif

/** @addtogroup SMECOMUTIL_CLASS 
    @{
*/

/**
   @brief Placeholder for Application Execution Environment (AEE) callback data.
*/
typedef struct _AEECallbackIx AEECallbackIx;


/**
   @brief Placeholder for AEE callback data and notify status error conditions.
   IxCallback uses AEECallback  
*/
typedef struct _IxCallback IxCallback;


/**
   Function pointer to notify of a callback cancel. Takes the IxCallback pointer
   as an input argument.

   @param[in] pcb Pointer to IxCallback.
*/
typedef void (*PFNCBCANCELIx)(IxCallback * pcb);


/**
   Function pointer to notify of a callback. Takes the IxCallback pointer as an
   input argument.

   @param[in] pcb Pointer to IxCallback.
*/
typedef void (*PFNNOTIFYIx)(IxCallback * pcb);

/**
   @brief Placeholder for AEE callback data.
*/
struct _AEECallbackIx
{
   AEECallbackIx *pNext;               /**< Reserved.                         */
   void          *pmc;                 /**< Reserved.                         */
   PFNCBCANCELIx  pfnCancel;           /**< Filled by the callback handler.   */
   void          *pCancelData;         /**< Filled by the callback handler.   */
   uint32         cancelDataSize;      /**< Filled by the callback handler.   */
   PFNNOTIFYIx    pfnNotify;           /**< Filled by the caller.             */
   void          *pNotifyData;         /**< Filled by the caller.             */
   uint32         notifyDataSize;      /**< Filled by the caller.             */
   uint32         pUserData;           /**< Filled by the caller; not to be
                                            handled by the callback.          */
   uint32         pClientData;         /**< Filled by the caller; not to be
                                            handled by the callback.          */
   uint32         pServerData;         /**< Filled by the callback handler.   */
                                       /* Leave as uint32 for now */
   uint32         pServerCancelCB;     /**< Filled by the callback handler.   */
   uint32         pServerCancelCBData; /**< Filled by the callback handler.   */
   void         **ppStream;            /**< Reserved.                         */
   void          *pReserved;           /**< Reserved; used by the handler.    */
};

/**
   @brief Placeholder for AEE callback data and notify status error conditions.
   IxCallback uses AEECallback.
*/
struct _IxCallback
{
struct _AEECallbackIx  AEECallback;  /**< AEE callback structure. */
IxErrnoType           NotifyStatus;  /**< Filled by the caller.   */
};

/** @} */  /* end_addtogroup SMECOMUTIL_CLASS */

#ifdef __cplusplus
__CPLUSPLUS_GUARD_END
#endif

#endif

