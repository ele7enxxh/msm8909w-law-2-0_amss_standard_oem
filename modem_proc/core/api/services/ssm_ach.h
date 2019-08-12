/**
@file ssm_ach.h
@brief
This module implements the Secured Services override Command handler 
functionality.
*/
#ifndef SSM_ACH_H
#define SSM_ACH_H
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

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/ssm_ach.h#1 $

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



/** SSM ACH override Status enumeration */
typedef enum
{
  /** Authentication Cookie Present */
  SSM_ACH_AUTHENTICATION_COOKIE_PRESENT,

  /** Authentication Cookie Not Present */
  SSM_ACH_AUTHENTICATION_COOKIE_NOT_PRESENT

}ssm_ach_authentication_coookie_status_t;


/** Callback to reflect ssm ach command status */
typedef void (*ssm_ach_register_cb_type)
(
  /**Depicts status of override command handler */
  ssm_ach_authentication_coookie_status_t ach_stat
);


/** SSM ACH error codes */
typedef enum 
{
  /** Operation succeeded */
  E_SSM_ACH_SUCCESS,

  /** Operation in progress */
  E_SSM_ACH_IN_PROGRESS,

  /** Operation failed */
  E_SSM_ACH_FAILED,

  /** FIle Open Operation failed */
  E_SSM_ACH_FILE_OPEN_FAILED,

  /** File Write Operation failed */
  E_SSM_ACH_FILE_WRITE_FAILED,

  /** File Remove Operation failed */
  E_SSM_ACH_FILE_REMOVE_FAILED,

  /** Dyanmic Allocation failed */
  E_SSM_ACH_DYNAMIC_ALLOCATION_FAILED,

  /** Authentication Cookie Already present */
  E_SSM_ACH_AUTHENTICAION_COOKIE_ALREADY_PRESENT,

  /** Invalid Error */
  E_SSM_ACH_INVALID= 0xFFFFFFFF

}ssm_ach_err_t;





/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/**
Registers for updates for override handler through callback 

@param[in] cb - Callback to be registered

@return 
E_SSM_ACH_SUCCESS -  Registration Successful
E_SSM_ACH_FAILED  -  Registration Failed

@dependencies
None

*/

ssm_ach_err_t ssm_ach_register_cb
(
  ssm_ach_register_cb_type cb
);




/**
Gets Authentication Cookie status 

@param[in]  
None

@return 
SSM_ACH_AUTHENTICATION_COOKIE_PRESENT     -  Authentication Cookie Present 
SSM_ACH_AUTHENTICATION_COOKIE_NOT_PRESENT -  Authentication Cookie Not Present

@dependencies
None

*/

ssm_ach_authentication_coookie_status_t ssm_ach_get_authentication_cookie_status(void);


#endif /* SSM_ACH_H */


/** @} */ /* end_addtogroup ssm */
