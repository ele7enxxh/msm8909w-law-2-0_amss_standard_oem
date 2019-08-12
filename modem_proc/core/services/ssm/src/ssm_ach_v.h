/**
@file ssm_ach_v.h
@brief
This module implements the Secured Services override Command handler 
functionality.
*/
#ifndef SSM_ACH_V_H
#define SSM_ACH_V_H
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

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_ach_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/05/12   din     File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "ssm_ach.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/





/** SSM ACH Commands */
typedef enum 
{
  /** Command to add authentication cookie */
  SSM_ACH_ADD_AUTHENTICATION_COOKIE,

  /** Command to remove Authetication cookie   */
  SSM_ACH_REMOVE_AUTHENTICATION_COOKIE,

} ssm_ach_cmd_t ;


typedef void * ssm_ach_cb_data_type;

/** Callback to reflect ssm ach command status */
typedef void (*ssm_ach_cb_type)
(
  /** Status of Authentication Cookie */
   ssm_ach_err_t        err,

  /** User data */
  ssm_ach_cb_data_type user_data
);


/** SSM ACH Override Command structures */
typedef struct
{
  /** Command to be proceesed */
  ssm_ach_cmd_t        cmd;

  /** Callback to be called after command is processed */
  ssm_ach_cb_type      cb;

  /** User data */
  ssm_ach_cb_data_type user_data;

} ssm_ach_cmd_struct_t;



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


ssm_ach_err_t ssm_ach_request
(
  /** Callback to inform about command status */
  ssm_ach_cmd_struct_t      * cmd
);



/**
Processes request queued to ACH in a serialized manner through a task.
To be called from SSM task.

@param[in]  
None

@return 
none

@dependencies
None

*/

void ssm_ach_process_request(void);



/**
Initializes SSM authentication cookie global variables

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_ach_init(void);


/**
Initializes SSM authentication cookie .
The Initialization sequence is delayed since it needs to be performed 
only once sfs initialies or is initialized from RCINIT Group 2

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_ach_delayed_init(void);


#endif /* SSM_ACH_V_H */


/** @} */ /* end_addtogroup ssm */
