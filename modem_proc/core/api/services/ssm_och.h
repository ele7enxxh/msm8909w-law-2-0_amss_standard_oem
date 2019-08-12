/**
@file ssm_och.h
@brief
This module implements the Secured Services override Command handler 
functionality.
*/
#ifndef SSM_OCH_H
#define SSM_OCH_H
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
  Implements SSM Override Command Handler Functionality

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/ssm_och.h#1 $

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



/** SSM OCH override Status enumeration */
typedef enum
{
  /** Override Successful */
  SSM_OCH_OVERRIDE_SUCCESSFUL,

  /** Override Not Successful */
  SSM_OCH_OVERRIDE_NOT_SUCCESSFUL

}ssm_och_ovrr_status_t;


/** Callback to reflect ssm och command status */
typedef void (*ssm_och_register_cb_type)
(
  /**Depicts status of override command handler */
  ssm_och_ovrr_status_t och_stat
);


/** SSM OCH error codes */
typedef enum 
{
  /** Operation succeeded */
  E_SSM_OCH_SUCCESS,

  /** Operation in progress */
  E_SSM_OCH_IN_PROGRESS,

  /** Operation failed */
  E_SSM_OCH_FAILED,

  /** FIle Open Operation failed */
  E_SSM_OCH_FILE_OPEN_FAILED,

  /** File Write Operation failed */
  E_SSM_OCH_FILE_WRITE_FAILED,

  /** File Remove Operation failed */
  E_SSM_OCH_FILE_REMOVE_FAILED,

  /** Signature Verfication failed */
  E_SSM_OCH_SIGNATURE_VERIFICATION_FAILED,

  /** Signature Generation failed */
  E_SSM_OCH_SIGNATURE_GENERATION_FAILED,

  /** Dynamic Allocation failed */
  E_SSM_OCH_DYNAMIC_ALLOCATION_FAILED,

  /** Secure Api Operation failed */
  E_SSM_OCH_SEC_API_FAILED,

  /** Random Nonce not valid */
  E_SSM_OCH_RANDOM_NONCE_INVALID,

  /** Invalid key returned by Key Handler */
  E_SSM_OCH_INVALID_KEY,

  /** Override suceeded but cookie create failed */
  E_SSM_OCH_OVERRIDE_SUCCEEDED_BUT_COOKIE_CREATE_FAILED,

  /**  Override suceeded but cookie write failed */
  E_SSM_OCH_OVERRIDE_SUCCEEDED_BUT_COOKIE_WRITE_FAILED,

  /** Invalid Error */
  E_SSM_OCH_INVALID= 0xFFFFFFFF

}ssm_och_err_t;





/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/**
Registers for updates for override handler through callback 

@param[in] cb - Callback to be registered

@return 
E_SSM_OCH_SUCCESS -  Registration Successful
E_SSM_OCH_FAILED  -  Registration Failed

@dependencies
None

*/

ssm_och_err_t ssm_och_register_cb
(
  ssm_och_register_cb_type cb
);


/**
Gets override status 

@param[in]  
None

@return 
SSM_OCH_OVERRIDE_SUCCESSFUL    -   Override Successful 
SSM_OCH_OVERRIDE_NOT_SUCCESSFUL -  Override not Successful 

@dependencies
None

*/

ssm_och_ovrr_status_t ssm_och_get_override_status(void);


#endif /* SSM_OCH_H */


/** @} */ /* end_addtogroup ssm */
