#ifndef SSM_SMDL_V_H
#define SSM_SMDL_V_H
/**
@file ssm_smdl_v.h
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
  Copyright (c) 2011 - 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                 S E C U R E   S E R V I C E S  M O D U L E

GENERAL DESCRIPTION
  Implements SSM IPC

EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_smdl_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/29/13   din     Changes pertaining to ADSP were done.
09/03/11   din     File created.

=============================================================================*/



/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "smd_lite.h"
#include "ssm_v.h"

/*===========================================================================
                        EXPORTED FUNCTION PROTOTYPES
===========================================================================*/


/* Length of Buffer passed to read function */
#define SSM_SMDL_RX_LENGTH 1000


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Get SMDL Port Connection Status

@param[in] proc                    Proc whose connection status needs to be determined

@return
TRUE or FALSE

@dependencies
None.

*/

boolean ssm_is_smdl_ready
( 
  ssm_proc_enum_t proc 
);

/** 
Handle SMDL read information

@param[in] proc                    Proc from which buffer should be read

@return
ssm_err_t -- E_SSM_SUCCESS if success, E_SSM_FAIL if failure

@dependencies
None.

*/

ssm_err_t ssm_smdl_handle_rx(void);

/** 
Transfer message through smdl

@param[in] proc                    Proc to  which buffer should be transferred
@param[in] write_buffer            Buffer to be written
@param[in] length                  length of the buffer

@return
ssm_err_t -- E_SSM_SUCCESS if success, E_SSM_FAIL if failure

@dependencies
None.

*/

ssm_err_t ssm_smdl_handle_tx
( 
  ssm_proc_enum_t proc,
  const void * write_buffer, 
  uint32 length
);


/** 
Initialize smdl port

@return
ssm_err_t  -- E_SSM_SUCCESS if success, E_SSM_FAIL if failure

@dependencies
None.

*/

ssm_err_t ssm_smdl_init(void);

#endif /* SSM_SMDL_V_H */
