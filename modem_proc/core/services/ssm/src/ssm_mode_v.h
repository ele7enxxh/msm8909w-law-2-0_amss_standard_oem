/**
@file ssm_mode_v.h
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
  Copyright (c) 2012 - 2013 by QUALCOMM Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM Mode transfer functionality.

EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS



=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_mode_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/29/13   din     Changes pertaining to ADSP were done.
08/09/12   din     Added Secure Channel Initialization.
07/20/12   din     Removed key exchange code from SSM.
09/03/11   din     File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_v.h"
#include "ssm_ipc_v.h"

/*=============================================================================

                           DATA ATTRIBUTES

=============================================================================*/


/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/** 
Update Mode status on modem

@param[in] src_proc                 Source from which the command was sent
@param[in] error_code               error code sent with ipc
@param[in] message                  message
@param[in] message_len              Message Length

@return
ssm_mode_status_t -- Status of Secure Channel 

@dependencies
None.

*/

ssm_err_t ssm_mode_update_mode
(
  ssm_proc_enum_t   src_proc,
  ssm_err_t         error_code,
  char *            message,
  uint32            message_len
);

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/** 
Update Mode status on modem

@param[in] src_proc                 Source from which the command was sent
@param[in] error_code               error code sent with ipc

@return
ssm_mode_status_t -- Status of Secure Channel 

@dependencies
None.

*/

ssm_err_t ssm_mode_ptom_mode_update_status
(
  ssm_proc_enum_t   src_proc,
  ssm_err_t         error_code
);

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

/** @} */ /* end_addtogroup ssm */
