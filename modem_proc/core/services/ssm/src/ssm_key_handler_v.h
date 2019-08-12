/**
@file ssm_key_handler_v.h
@brief
This module implements the Secured Services key handler functionality.
*/
#ifndef SSM_KEY_HANDLER_V_H
#define SSM_KEY_HANDLER_V_H
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
  Implements SSM Key Handler Functionality

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_key_handler_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/05/12   din     File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_key_handler.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/





/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/**
Initializes key handler and its global variable

@param[in]  
None

@return 
None 

@dependencies
None

*/

void ssm_key_handler_init(void);



/**
Processes request queued to Key Handler in a serialized manner through a task.

@param[in]  
None

@return 
none

@dependencies
None

*/

void ssm_key_handler_process_request(void);




#endif /* SSM_KEY_HANDLER_V_H */


/** @} */ /* end_addtogroup ssm */
