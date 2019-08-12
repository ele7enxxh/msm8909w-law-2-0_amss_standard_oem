/**
 * @file   tlDriverApi.h
 * @brief  Contains trustlet API definitions
 *
 * <Copyright goes here> 
 */

#ifndef __TLDRIVERAPI_H__
#define __TLDRIVERAPI_H__

#include "tlStd.h"
#include "TlApi/TlApiError.h"


/**
 * Open session to the driver with given data
 *
 * @return  session id
 */
_TLAPI_EXTERN_C uint32_t tlApiOpenSession( void );


/**
 * Close session
 *
 * @param sid  session id
 *
 * @return  TLAPI_OK upon success or specific error
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiCloseSession( uint32_t sid );


/**
 * Inits session data (sample data 01)
 *
 * @param length  data length
 * @param address data address
 *
 * @return  TLAPI_OK upon success or specific error
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiInitSampleData01(
        uint32_t sid,
        uint32_t length,
        uint32_t address);


/**
 * Inits session data (sample data 02)
 *
 * @param data
 *
 * @return  TLAPI_OK upon success or specific error
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiInitSampleData02(
        uint32_t sid,
        uint32_t data);


/**
 * Executes command
 *
 * @param sid        session id
 * @param commandId  command id
 *
 * @return  TLAPI_OK upon success or specific error
 */
_TLAPI_EXTERN_C tlApiResult_t tlApiExecute(
        uint32_t sid,
        uint32_t commandId);


/** tlApi function to call driver via IPC.
 * Sends a MSG_RQ message via IPC to a MobiCore driver.
 *
 * @param driverID The driver to send the IPC to.
 * @param pMarParam MPointer to marshaling parameters to send to the driver.
 *
 * @return TLAPI_OK
 * @return E_TLAPI_COM_ERROR in case of an IPC error.
 */
_TLAPI_EXTERN_C tlApiResult_t tlApi_callDriver(
        uint32_t driver_ID,
        void* pMarParam);


#endif // __TLDRIVERAPI_H__
