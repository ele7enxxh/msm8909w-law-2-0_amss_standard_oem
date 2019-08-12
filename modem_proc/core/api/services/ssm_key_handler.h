/**
@file ssm_key_handler.h
@brief
This module implements the Secured Services key handler functionality.
*/
#ifndef SSM_KEY_HANDLER_H
#define SSM_KEY_HANDLER_H
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The SSM_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      SSM_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the SSM group 
      description in the SSM_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM Key Handler Functionality

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/ssm_key_handler.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/05/12   din     File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include<comdef.h>

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/** SSM Key Handler Requests */
typedef enum 
{
  /** Requests Key handler to add a key  */
  SSM_KEY_HANDLER_ADD_KEY = 0,

  /** Requests Key handler to get a key  */
  SSM_KEY_HANDLER_GET_KEY ,

  /** Requests Key handler to remove key  */
  SSM_KEY_HANDLER_REMOVE_KEY,

  /** Invlalid request */
  SSM_KEY_HANDLER_INVALID = 0xFFFFFFFF

} ssm_key_handler_req_t ;

/** SSM Key Handler error codes */
typedef enum 
{
  /** Operation succeeded */
  E_SSM_KEY_HANDLER_SUCCESS,

  /** Operation in progress */
  E_SSM_KEY_HANDLER_IN_PROGRESS,

  /** Operation failed due to file open failure*/
  E_SSM_KEY_HANDLER_FILE_OPEN_FAILED,

  /** Operation failed due to file read failure*/
  E_SSM_KEY_HANDLER_FILE_READ_FAILED,

  /** Operation failed due to file write failure*/
  E_SSM_KEY_HANDLER_FILE_WRITE_FAILED,

  /** Operation reset failed */
  E_SSM_KEY_HANDLER_FILE_RESET_FAILED,

  /** Operation failed due to invalid input */
  E_SSM_KEY_HANDLER_INVALID_INPUT,
  
  /** Operation failed since key not found */
  E_SSM_KEY_HANDLER_KEY_NOT_FOUND,
  
  /** Operation failed since key is alreay present */
  E_SSM_KEY_HANDLER_KEY_ALREADY_PRESENT,

  /** Operation Not Allowed */
  E_SSM_KEY_HANDLER_NOT_ALLOWED,

  /** Operation failed due to dynamic allocation failure */
  E_SSM_KEY_HANDLER_DYNAMIC_ALLOCATION_FAILED,

  /** Operation failed */
  E_SSM_KEY_HANDLER_FAILED,

  /** Invalid */
  E_SSM_KEY_HANDLER_INVALID = 0xFFFFFFFF


}ssm_key_handler_err_t;

/** SSM Key Handler Key's usage */
typedef enum 
{
  /** Usage override */
  SSM_KEY_HANDLER_USAGE_OVERRIDE = 0,

  /** Usage OBD Milenage */
  SSM_KEY_HANDLER_USAGE_OBD_MILENAGE,

  /** Usage Max */
  SSM_KEY_HANDLER_USAGE_MAX,

  /** Usage Invalid */
  SSM_KEY_HANDLER_USAGE_INVALID = 0xFFFFFFFF

}ssm_key_handler_usage_t;

/** SSM Key Handler Key's source */
typedef enum 
{
  /** Source QCOM */
  SSM_KEY_HANDLER_SRC_QCOM = 0,

  /** Source OEM */
  SSM_KEY_HANDLER_SRC_OEM,

  /** Source Max */
  SSM_KEY_HANDLER_SRC_MAX,

  /** Source Invalid */
  SSM_KEY_HANDLER_SRC_INVALID = 0xFFFFFFFF

}ssm_key_handler_source_t;



/** SSM Key Handler Crypto-Algo  */
typedef enum 
{
  /** RSA PKCS#1 PSS verification key */
  SSM_KEY_HANDLER_CRYPTO_ALGO_RSA_PKCS_1_PSS_VERIFICATION_KEY = 0,

  /** RSA PKCS#1 PSS signature key */
  SSM_KEY_HANDLER_CRYPTO_ALGO_RSA_PKCS_1_PSS_SIGNATURE_KEY ,

  /** Milenage Ks, OP and AID */
  SSM_KEY_HANDLER_CRYPTO_ALGO_MILENAGE_KS_OP_AID ,

  /** CRYPTO_ALGO_MAX */
  SSM_KEY_HANDLER_CRYPTO_ALGO_MAX,

  /** Crypto Algo Invalid */
  SSM_KEY_HANDLER_CRYPTO_ALGO_INVALID = 0xFFFFFFFF

}ssm_key_handler_crypto_algo_t;

/** Structure of the info about key */
typedef struct
{ 
   /** SSM Key Handler Key's usage */
   ssm_key_handler_usage_t         usage;

   /** SSM Key Handler Key's source */
   ssm_key_handler_source_t        source;

   /** SSM Key Handler Key's serial number */
   uint8                           serial_num[16];

   /** SSM Key Handler Crypto Algorithm that key will ne used for */
   ssm_key_handler_crypto_algo_t   crypto_algo;

   /** Length of the key */
   uint16                          len_key;

} ssm_key_handler_info_t;


typedef void * ssm_key_handler_cb_data_type;

/** Callback to reflect ssm key handler status */
typedef void (*ssm_key_handler_cb_type)
(
  /** Error type : Depicts status of key handler request */
  ssm_key_handler_err_t         err,

  /** SSM Key Handler Operation */
  ssm_key_handler_req_t         req,

  /** SSM Key Handler info */
  ssm_key_handler_info_t        *info,

  /** SSM Key Handler key */
  uint8                         *key,

  /** SSM key Handler cb data */
  ssm_key_handler_cb_data_type   user_data

);


/** Structure of the info about key */
/* Clients must free up the key once they use it otherwise this will result in a memory leak */
typedef struct
{ 
   /** SSM Key Handler Operation */
   ssm_key_handler_req_t         req;

   /** SSM Key Handler Callback .. to be called after the operation is done */
   ssm_key_handler_cb_type       cb;

   /** SSM Key Handler info */
   ssm_key_handler_info_t        info;

   /** SSM Key Handler key */
   uint8                         *key;

  /** SSM key Handler cb data */
  ssm_key_handler_cb_data_type   user_data;

} ssm_key_handler_struct_t;





/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/



/**
Enqueues Key Handler request to the queue

@param[in]  
* key_req   -  Structure allocated by the client pertaining to key based on key request.
             Should be deallocated after the call to the function.

@return 
ssm_key_handler_err_t  - E_SSM_KEY_HANDLER_FAILED        Operation failed
                         E_SSM_KEY_HANDLER_IN_PROGRESS   Operation in Progress 

@dependencies
None

*/

ssm_key_handler_err_t ssm_key_handler_request
(
   ssm_key_handler_struct_t   *key_req
);


/**
Get keys from the secure key handler. The function is meant for synchronous call

@param[in]  
key_req   -  Structure allocated by the caller and deallocated after the call 
             to the function. key ( part of key_req ) should be deallocated 
	     by the user. Its a blocking call and uses sfs operation and 
	     hence mutex shouldn't be used around the call.

@return 
None

@dependencies
None

*/

ssm_key_handler_err_t ssm_key_handler_get_key
(
   ssm_key_handler_struct_t   * key_req
);


#endif /* SSM_KEY_HANDLER_H */


/** @} */ /* end_addtogroup ssm */
