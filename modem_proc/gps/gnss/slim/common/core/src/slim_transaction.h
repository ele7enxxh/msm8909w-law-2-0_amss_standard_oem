#ifndef SLIM_TRANSACTION_H
#define SLIM_TRANSACTION_H
/**
@file
@brief Module for handling SLIM transactions.

  Requests made by SLIM clients are stored as transactions in SLIM
  transaction registry. Registry keeps count which transactions client
  has initiated and keeps track of the transaction ids. This module provides
  functions for adding and removing of theses transactions.

@ingroup slim_CoreTransactionRegistry
*/
/*============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

               Copyright (c) 2013-2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
============================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/slim/common/core/src/slim_transaction.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "slim_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup slim_CoreTransactionRegistry
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
@brief Maximum number of opened transactions.

This constant defines a hard limit of simultaneously opened transactions. These
transactions include operations requested by client, as well as internal
operations (like time synchronization request).
*/
#define SLIM_MAX_TXN_COUNT (10)
/**
 * @brief Invalid transaction identifier
 */
#define SLIM_INVALID_TXN_ID (0)
/**
 * @brief Timeout value for transactions that can't expire
 */
#define SLIM_TXN_TIMEOUT_INFINITE_MS (UINT32_MAX)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
/** Enum for SLIM transaction types */
typedef enum
{
  SLIM_TXN_NONE = 0,  /**< Internal SLIM transaction */
  SLIM_TXN_ENABLE,    /**< Enable reporting transaction */
  SLIM_TXN_DISABLE,   /**< Disable reporting transaction */
  SLIM_TXN_GET,       /**< Get transaction */
  SLIM_TXN_SET,       /**< Set transaction */
  SLIM_TXN_GET_TIME,  /**< Get provider time transaction */
  //! @cond
  SLIM_TXN_MAX = 2147483647 /* To force a 32 bit enum. Do not change or use */
  //! @endcond
} slim_TxnEnumType;

/*! Struct for SLIM transaction data */
typedef struct slim_TxnDataStructType
{
  slim_ClientTxnDataStructType z_ClientData; /*!< Client data */
  slim_TxnEnumType             e_TxnType;    /*!< Transaction type */
  slimServiceEnumT             e_Service;    /*!< SLIM service type */
  slim_ProviderEnumType        e_Provider;   /*!< SLIM provider type */
  boolean                      v_Reserved;   /*!< TRUE if slot is reserved. */
  uint64                       t_ReferenceTime_ms; /*!< Time when transaction was
                                                     added or reference time
                                                     provided by SLIM client. */
  uint64                       t_ExpireTime_ms; /**< Expiration time */
} slim_TxnDataStructType;

/*! Struct for SLIM transaction registry */
typedef struct slim_TxnRegistryStructType
{
  slim_TxnDataStructType z_Txns[SLIM_MAX_TXN_COUNT]; /*!< Transaction array */
  slim_MutexStructType z_TxnMutex;                   /*!< Transaction mutex */
} slim_TxnRegistryStructType;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
@brief Adds transaction to registry.

SLIM core should add transactions to transaction registry using this
function before sending the request to the actual sensor data provider.
If maximum transaction count is reached, function returns.
eSLIM_ERROR_MAX_TXN_COUNT_REACHED.

@param[in]  pz_Registry           Pointer to the transaction registry.
@param[in]  cz_Txn                Pointer to the client transaction data.
@param[in]  e_Type                Transaction type
@param[in]  e_Service             Service type
@param[in]  t_ReferenceTime_ms    Time when transaction was added or reference
                                  time provided by SLIM client.
@param[in]  q_Timeout_ms          Transaction expiration time in milliseconds.
                                  Can be SLIM_TXN_TIMEOUT_INFINITE_MS.
@param[out] pl_ServiceTxnIdOut    Pointer to the generated service transaction
                                  id if the function is successful.

@return Operation result
@retval eSLIM_SUCCESS if adding succeeds
*/
slimErrorEnumT slim_TransactionAdd
(
  slim_TxnRegistryStructType         *pz_Registry,
  const slim_ClientTxnDataStructType *cz_Txn,
  slim_TxnEnumType                    e_Type,
  slimServiceEnumT                    e_Service,
  slim_ProviderEnumType               e_Provider,
  uint64                              t_ReferenceTime_ms,
  uint32                              q_Timeout_ms,
  int32                              *pl_ServiceTxnIdOut
);

/**
@brief Finds and provides transaction data.

Specific transaction details can be fetched with this function.
The client handle and transaction id is provided if a transaction
is found with the given service transaction id.

@param[in]  pz_Registry    Pointer to the transaction registry.
@param[in]  l_ServiceTxnId Service transaction id.
@param[out] pz_TxnOut      Found transaction data if the function succeeds.

@return TRUE if transaction is found. Otherwise FALSE.
*/
boolean slim_TransactionFind
(
  slim_TxnRegistryStructType *pz_Registry,
  int32                       l_ServiceTxnId,
  slim_TxnDataStructType     *pz_TxnOut
);

/**
@brief Checks if client has pending transactions.

The function checks if client has pending transactions in the registry.

@param[in] pz_Registry Pointer to the transaction registry.
@param[in] p_Handle    Client handle.

@retval TRUE  if transactions for client are found.
@retval FALSE The client doesn't have pending transaction
*/
boolean slim_TransactionHasPendingForClient
(
  slim_TxnRegistryStructType *pz_Registry,
  slimClientHandleT           p_Handle
);

/**
@brief Removes single transaction from registry.

Removes transaction from the registry with the given service transaction id.

@param[in] pz_Registry    Pointer to the transaction registry.
@param[in] l_ServiceTxnId Service transaction id.
*/
void slim_TransactionRemoveOne
(
  slim_TxnRegistryStructType *pz_Registry,
  int32                       l_ServiceTxnId
);

/**
@brief Removes all transactions initiated by a specific client.

Removes all transactions from the registry that were initiated by the
client corresponding to the given client handle.

@param[in] pz_Registry Pointer to the transaction registry.
@param[in] p_Handle    Client handle.
*/
void slim_TransactionRemoveAll
(
  slim_TxnRegistryStructType *pz_Registry,
  slimClientHandleT           p_Handle
);

/**
@brief Checks if a transaction id is valid.

Function returns TRUE if the given transaction id is valid. Id might be valid
even though a transaction matching the id does not exist.

@param[in] l_ServiceTxnId Transaction id.

@retval TRUE  The transaction number is valid.
@retval FALSE The transaction number is invalid.
*/
boolean slim_TransactionIsValidId
(
  int32 l_ServiceTxnId
);

/**
 * @brief Locates timed out transaction entry in table
 *
 * The method locates first timed out transaction entry.
 *
 * @param[in]  pz_Registry        Transaction registry.
 * @param[in]  t_ReferenceTime_ms SLIM time in milliseconds.
 * @param[out] pl_ServiceTxnIdOut Transaction identifier if found.
 * @param[out] pz_TxnOut          Transaction data if found. Optional.
 *
 * @retval TRUE  Transaction has been found and values are returned.
 * @retval FALSE Transaction has not been found.
 *
 * @sa slim_TransactionGetNextExpirationTime
 * @sa slim_TransactionSetExpirationTimeout
 */
boolean slim_TransactionFindExpired
(
  slim_TxnRegistryStructType *pz_Registry,
  uint64                      t_ReferenceTime_ms,
  int32                      *pl_ServiceTxnIdOut,
  slim_TxnDataStructType     *pz_TxnOut
);

/**
 * @brief Locates next earliest transaction expiration time
 *
 * The method locates transaction entry with the earliest expiration time and
 * returns expiration time in @a pt_ExpirationTimeMsOut
 *
 * @param[in]  pz_Registry             Transaction registry.
 * @param[out] pt_ExpirationTimeOut_ms SLIM time in milliseconds if found.
 *
 * @retval TRUE  Transaction has been found and expiration time is returned.
 * @retval FALSE Transaction has not been found.
 *
 * @sa slim_TransactionFindExpired
 * @sa slim_TransactionSetExpirationTimeout
 */
boolean slim_TransactionGetNextExpirationTime
(
  slim_TxnRegistryStructType *pz_Registry,
  uint64                     *pt_ExpirationTimeOut_ms
);

/**
 * @brief Changes transaction expiration time.
 *
 * @param[in] pz_Registry     Transaction registry.
 * @param[in] l_ServiceTxnId  Transaction identifier.
 * @param[in] q_Timeout_ms    SLIM expiration time in milliseconds. Can
 *                            be #SLIM_TXN_TIMEOUT_INFINITE_MS.
 *
 * @return None.
 *
 * @sa slim_TransactionFindExpired
 * @sa slim_TransactionGetNextExpirationTime
 */
void slim_TransactionSetExpirationTimeout
(
  slim_TxnRegistryStructType *pz_Registry,
  int32                       l_ServiceTxnId,
  uint32                      q_Timeout_ms
);

//! @}

#ifdef __cplusplus
}
#endif

#endif /* #ifndef SLIM_TRANSACTION_H */
