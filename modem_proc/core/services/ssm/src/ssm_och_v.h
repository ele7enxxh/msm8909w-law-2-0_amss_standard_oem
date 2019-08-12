/**
@file ssm_och_v.h
@brief
This module implements the Secured Services override Command handler 
functionality.
*/
#ifndef SSM_OCH_V_H
#define SSM_OCH_V_H
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
  Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM Override Command Handler Functionality

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_och_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/05/12   din     File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "ssm_och.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/





/** SSM OCH Commands */
typedef enum 
{
  /** Command to request Temporary override   */
  SSM_OCH_TEMP_OVERRIDE,

  /** Command to request Permanent override   */
  SSM_OCH_PERM_OVERRIDE,

  /** Command to request Cancel override   */
  SSM_OCH_CANCEL_OVERRIDE,

  /** Command to sign random nonce */
  SSM_OCH_SIGN_RANDOM_NONCE

} ssm_och_cmd_t ;

typedef void * ssm_och_cb_data_type;

/** Callback to reflect ssm och command status */
typedef void (*ssm_och_cb_type)
(
  /** Error type : Depicts status of OCH command */
  ssm_och_err_t         err,

  /** Length of data */
  uint16                len_data,

  /** Output Data type */
  uint8 *               output_data,

  /** User data */
  ssm_och_cb_data_type user_data

);


/** SSM OCH Override Command structures */
typedef struct
{
  /** Command to be proceesed */
  ssm_och_cmd_t        cmd;

  /** Callback to be called after command is processed */
  ssm_och_cb_type      cb;

  /** Optional member : To be used only in some comamnds */
  /** Usage for the key */
  uint32               usage;

  /** Optional member : To be used only in some comamnds */
  /** Source for the key , to be used to extract key from 
   * Key Handler*/
  uint32              source;

  /** Length of the signature */
  uint32              len_sig;

  /** Signature */
  uint8                * signature;

  /** User data */
  ssm_och_cb_data_type  user_data;

} ssm_och_override_cmd_struct_t;


/** SSM OCH Override Command structures */
typedef struct
{
  /** Command to be proceesed */
  ssm_och_cmd_t        cmd;

  /** Callback to be called after command is processed */
  ssm_och_cb_type      cb;

  /* Indicator for temp/ permanent override */
  boolean              temp_override;

  /** Mod length */
  uint16               mod_len;

  /** Exp length */
  uint16               exp_len;

  /** Modulus */
  uint8                * mod;

  /** Exponent */
  uint8                * exp;

  /** User data */
  ssm_och_cb_data_type  user_data;

} ssm_och_signature_cmd_struct_t;




/** SSM OCH Command structures */
typedef union
{
  /** Override Command to be processed */
  ssm_och_override_cmd_struct_t   ovrr_cmd;

  /** Signature Command to be processed */
  ssm_och_signature_cmd_struct_t  sign_cmd;

} ssm_och_cmd_struct_t;




/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/




/**
Enqueues OCH Command  request to the queue

@param[in]  
cmd  -  Structure allocated by the client for OCH commands
        Should be deallocated after the call to the function.
        Only Deallocate the cmd structure and not the memory pointed by 
	uint8 * signature within cmd.

@return 
ssm_och_err_t  - E_SSM_OCH_FAILED        Operation failed
                 E_SSM_OCH_IN_PROGRESS   Operation in Progress 

@dependencies
None

*/

ssm_och_err_t ssm_och_request
(
  /** Callback to inform about command status */
  ssm_och_cmd_struct_t      * cmd
);



/**
Processes request queued to OCH in a serialized manner through a task.
To be called from SSM task.

@param[in]  
None

@return 
none

@dependencies
None

*/

void ssm_och_process_request(void);



/**
Gets random nonce of size 256 bits 

@param[out]  
random nonce  -  Structure allocated by the client to get the random nonce and 
                 pass the address of the string for the random nonce to be copied

@return 
ssm_och_err_t  - E_SSM_OCH_FAILED        Operation failed
                 E_SSM_OCH_SUCCESS    Operation in Progress 

@dependencies
None

*/

ssm_och_err_t ssm_och_get_random_nonce
(
  /* Random Nonce */
   uint8  * random_nonce_ptr
);



/**
Reset random nonce 

@param[in]  
None

@return 
ssm_och_err_t  - E_SSM_OCH_FAILED     Operation failed
                 E_SSM_OCH_SUCCESS    Operation in Progress 

@dependencies
None

*/

#ifdef FEATURE_SSM_TEST_DEBUG
ssm_och_err_t ssm_och_reset_random_nonce(void);
#endif /* FEATURE_SSM_TEST_DEBUG */


/**
Initializes SSM override command handler module and its global variable

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_och_init(void);




#endif /* SSM_OCH_V_H */


/** @} */ /* end_addtogroup ssm */
