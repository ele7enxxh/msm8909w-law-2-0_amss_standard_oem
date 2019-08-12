#ifndef SSM_H
#define SSM_H

/**
@file ssm.h
@brief
This module implements the Secured Services Functionality.
*/
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
  Copyright (c) 2011 - 2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM framework functionality

EXTERNALIZED FUNCTIONS
  ssm_client_init_func
    Register a client with SSM

  ssm_get_perm
    Gets permission for a particular set of attributes

INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/ssm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   din     Added new apis to get mode and to write to permission file. 
                   Also added diag command to get ssm mode.
09/13/12   din     New SSM apis to get the size of perm data and copy perm data.
09/03/11   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <stdarg.h>
#include "comdef.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/**
@mainpage SSM - Documentation
Secure subsystem Module (SSM) refers to a framework to which a client can 
register itself specifying different attributes and defining various permission 
level associated with different combination of atribute values. SSM parses the 
input file ( referred by client during initialization and depends on mode 
information, discussed below in detail ) and returns the associated permission 
level for a combination of associated value .


*/


/**
@defgroup SSM - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup SSM Functions
@file ssm.h
This file contains public interface defined by this module
*/

/** SSM_DEFAULT_MODE indicates the default mode when phone starts or if the mode 
 * isn't set */
#if (!defined(SSM_DEFAULT_MODE))
#define SSM_DEFAULT_MODE            0
#endif

/** ssm_attr_data_t is typedefed to uint32. It resembles entries for attributtes */
typedef uint32 ssm_attr_data_t;

/** ssm_perm_data_t is typedefed to uint32. It resembles entries for permission */
typedef uint32 ssm_perm_data_t;

/** SSM_INIT_FAILED indicates that initialization of a client with SSM has failed */
//#define SSM_INIT_FAILED            -1

/** Invalid Client ID */
#define SSM_CLIENT_INVALID_ID      -1

/** SSM_GET_PERM_ERR reflects invalid client id, or invalid number of arguments 
  or call cannot be processed by SSM */
#define SSM_GET_PERM_ERR           -1

/** Various type of attributes .
*/

typedef enum ssm_attr
{
  /** Signifies uint32 type attributes */
  SSM_UINT32,
  /** Signifies string type attributes */
  SSM_STRING
} ssm_attr_t;

/** Error Codes */
typedef enum ssm_err_type
{
  /** SSM operation succeeded */
  E_SSM_SUCCESS,
  /** SSM operation failed */
  E_SSM_FAIL,
  /** Client yet to be initialized */
  E_SSM_CLIENT_NOT_INITIALIZED,
  /** No more clients can be allowed */
  E_SSM_CLIENT_MAX_ID_REACHED,
  /** Attribute Type is not Supported */
  E_SSM_CLIENT_ATTR_TYPE_NOT_SUPPORTED,
  /** Client Initialization is in process */
  E_SSM_CLIENT_INIT_IN_PROCESS,
  /** Permission file operation failed */
  E_SSM_FILE_OPR_FAILED,
  /** Overflow is seen with values in current data type */
  E_SSM_DATA_OVERFLOW_ERROR,
  /** Dynamic memory allocation failed */
  E_SSM_DATA_MALLOC_FAILED,
  /** No more simultaneous ssm_get_perm can be supported */
  E_SSM_GET_PERM_MAX_COUNTER_REACHED,
  /** Id passed as argument is invalid */
  E_SSM_GET_PERM_INVALID_ID,
  /** Client reflected with "id" is not initialized */
  E_SSM_GET_PERM_CLIENT_NOT_INITIALIZED,
  /** Invalid number of arguments passed to ssm_get_perm */
  E_SSM_GET_PERM_INVALID_NUM_ARGS,
  /** Functionality failed since mode change occurred in between */
  E_SSM_GET_PERM_MODE_CHANGE_OCCURRED,
  /** Invalid is passed to function */
  E_SSM_INVALID_ID,
  /** Invalid data */
  E_SSM_INVALID_DATA,
  /** Deny Request */
  E_SSM_GET_PERM_CLIENT_DENIED,
  /** Message Decode failed */
  E_SSM_MSG_DECODE_ERR,
  /** Mode Changed */
  E_SSM_MODE_CHANGED,
  /** File Operation Completed */
  E_SSM_FILE_COMPLETE,
  /** Secapis failed */
  E_SSM_SECAPI_FAILED,

  E_SSM_UINT32_MAX = 0xFFFFFFFF

}ssm_err_t;

/** Callback to reflect ssm_clients status after initialization */
typedef void (*ssm_cb_type)
(
  /** Error type : Depicts status of ssm _client after initialization is complete */
  ssm_err_t         err
);




/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/** 
Registers a client with ssm module. 

@param[in] filename                       Filename to be parsed.
@param[in] num_of_attr                    Number of attributes in the date file
@param[in] default_perm                   Default permission to be returned when 
                                          entry not found  
@param[in] attr_type                      attribute  types - uint32 or string  
@param[in] ssm_cb_type                    Callback which will be called reporting the status of 
                                          ssm clients
@param[out] uint32 *                      ID of the client
@param[in]  char *                        Fallback buffer if permission file is not present
@param[in] uint32                         Length of flabback buffer

@return
ssm_err_t                                 E_SSM_SUCCESS if client already initialized 
                                          or updated with fallback buffer
                                          E_SSM_CLIENT_INIT_IN_PROCESS in process and not 
					  updated with fallback buffer
                                          E_SSM_FAIL failed
					  E_SSM_CLIENT_MAX_ID_REACHED max id reached
					  E_SSM_CLIENT_ATTR_TYPE_NOT_SUPPORTED attribute not supported
					  E_SSM_DATA_MALLOC_FAILED Dynamic memory allocation failed

@dependencies
None.
*/


ssm_err_t ssm_client_init_func
( 
  char *        filename, 
  uint32        num_of_attr, 
  uint32        default_perm,
  ssm_attr_t    attr_type,
  ssm_cb_type   ssm_cb,
  uint32 *      client_id,
  char   *      fallback_buffer,
  uint16        len_fallback_buffer
);

/** 
Checks if the client is initialized or not

@param[in] filename                       Filename of the client. Identifies client.

@return
boolean                                   TRUE if initialized 
                                          FALSE otherwise

@dependencies
None.
*/


boolean ssm_parser_is_client_initialized
( 
  char *        filename
);


/** 
Get permission set for a particular ssm attribute 

@param[out] ssm_perm_t                    Corresponding permission
@param[in] arg1                           Number of argument
@param[in] ...                            ellipsis - variable args
                                          in the order of client id, 
					  and argument set 

@return
ssm_err_t    E_SSM_SUCCESS if success
             E_SSM_CLIENT_NOT_INITIALIZED if client not initialized
	     E_SSM_INVALID_ID if arguments are invalid
	     E_SSM_GET_PERM_MAX_COUNTER_REACHED max simulatneous get perm 
	     E_SSM_GET_PERM_CLIENT_DENIED request denied
	     E_SSM_GET_PERM_INVALID_NUM_ARGS invalid num of arguments
	     E_SSM_GET_PERM_MODE_CHANGE_OCCURRED mode change occurred;
					  
@dependencies
None.
*/

ssm_err_t ssm_get_perm
(
  ssm_perm_data_t * perm,
  uint32 arg1,
  ...
);

/** 
Gets the number of entries of perm data if client is initialized

@param[in]  client_id                Id of the client
@param[out] num_ent                     number of entries of perm data 
  
@return
ssm_err_t  - E_SSM_SUCCESS if success
             E_SSM_CLIENT_NOT_INITIALIZED if client not initialized
	     E_SSM_INVALID_ID if arguments are invalid
	     

@dependencies
num_ent will be set to zero if the operation failed

*/

ssm_err_t ssm_get_perm_data_size
( 
  uint32 client_id,
  uint32 * num_ent
);

/** 
Copies the perm data to buffer allocated by the client and de initailizes 
the client.

@param[in]  client_id                Id of the client
@param[in]  size                     size of the buffer in bytes
@param[in]  perm_data_ptr            address of the buffer
  
@return
ssm_err_t  - E_SSM_SUCCESS if success
             E_SSM_CLIENT_NOT_INITIALIZED if client not initialized
	     E_SSM_INVALID_ID if arguments are invalid

@side_effects
SSM client will be deinitialized from ssm subsystem.

*/

ssm_err_t ssm_copy_perm_data 
( 
  uint32 client_id,
  uint32 size,
  void * perm_data_ptr
);


/** 
Returns ssm mode 

@param[in] None                          

@return
uint32                                     SSM mode

@dependencies
None.
*/

uint32 ssm_parser_get_mode( void );

#endif /* SSM_H */
