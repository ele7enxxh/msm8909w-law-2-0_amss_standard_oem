/*=============================================================================

             S E C U R E  S E R V I C E S   M O D U L E

GENERAL DESCRIPTION
  Implements SSM Mode transfer functionality.

EXTERNALIZED FUNCTIONS

  INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011 - 2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_mode.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
04/29/13   din     Changes pertaining to ADSP were done.
01/18/13   din     Accomodated sec api function change.
11/01/12   din     Fixed a security bug with mode updates.
09/25/12   din     Featured out secure channel initialization for 9x25.
08/09/12   din     Added Secure Channel Initialization .
07/20/12   din     Removed key exchange code from SSM.
01/20/12    ab     Changed ssm_timer_group to deferrable.
09/01/12   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_mode_v.h"
#include "ssm_ipc_v.h"
#include "ssm_v.h"



/*=============================================================================

                           DATA ATTRIBUTES

=============================================================================*/


#define SSM_MODE_MAX_LENGTH           4


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
)
{
  /* Mode Value */
  uint32 mode=0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED (" Function ssm_mode_update_mode " );

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

  if ( src_proc != SSM_PROC_APPS )
  {
    ssm_ipc_encode_send ( SSM_PROC_APPS,
                          SSM_MTOA_MODE_UPDATE_STATUS,
			  E_SSM_FAIL,
                          NULL,
                          0 );
  }

  if ( error_code != E_SSM_SUCCESS )
  {
    ssm_ipc_encode_send ( SSM_PROC_APPS,
		          SSM_MTOA_MODE_UPDATE_STATUS,
                          E_SSM_FAIL,
                          NULL,
                          0 );
  }

  #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  if ( src_proc != SSM_PROC_MODEM )
  {
    ssm_ipc_encode_send ( src_proc,
		          SSM_PTOM_MODE_UPDATE_STATUS,
                          E_SSM_FAIL,
                          NULL,
                          0 );
  }

  if ( error_code != E_SSM_SUCCESS )
  {
    ssm_ipc_encode_send ( src_proc,
                          SSM_PTOM_MODE_UPDATE_STATUS,
                          E_SSM_FAIL,
                          NULL,
                          0 );
  }


  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


 
  /* Get  Mode */
  (void) ssm_ipc_decode_break_ipc_helper( message, message_len, 0, &mode) ;

  ssm_parser_update_mode( mode );
  
  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

  if ( E_SSM_SUCCESS != ssm_ipc_encode_send ( SSM_PROC_ADSP,
		                              SSM_MTOP_MODE_UPDATE,
                                              E_SSM_SUCCESS,
                                              message,
                                              message_len ))
  {
    /* If SSM on ADSP is not present, directly send it to Apps */
    return ssm_ipc_encode_send ( SSM_PROC_APPS,
                               SSM_MTOA_MODE_UPDATE_STATUS,
                               E_SSM_SUCCESS,
		               NULL,
                               0 );
  }

  #else

  (void) ssm_ipc_encode_send ( SSM_PROC_MODEM,
                               SSM_PTOM_MODE_UPDATE_STATUS,
                               E_SSM_SUCCESS,
                               NULL,
                               0 );

    
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  return E_SSM_SUCCESS;
    
} /* ssm_mode_update_mode */


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
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED (" Function ssm_mode_ptom_mode_update_status " );

  if ( src_proc != SSM_PROC_ADSP )
  {
    return E_SSM_FAIL;
  }

  return ssm_ipc_encode_send ( SSM_PROC_APPS,
                               SSM_MTOA_MODE_UPDATE_STATUS,
                               error_code,
		               NULL,
                               0 ); 
  
} /* ssm_mode_ptom_mode_update_status */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

