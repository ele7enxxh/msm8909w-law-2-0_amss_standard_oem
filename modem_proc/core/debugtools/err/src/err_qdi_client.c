/*=============================================================================

                err_qdi_client.c

GENERAL DESCRIPTION
      ERR QDI Client Process Code that goes to main image

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2014
by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_qdi_client.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/31/14   din     Added qdi calls for err on User PD.
=============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "err_qdi_client.h"
#include "err.h"
#include "err_policy_pdr.h"

static int err_qdi_client_handle = -1;
extern boolean err_qdi_client_initialized;

/*===========================================================================

FUNCTION    ERR_QDI_CLIENT_INIT

DESCRIPTION
  Initialize the err client qdi service.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void err_qdi_client_init(void) 
{
 
   err_qdi_client_handle = qurt_qdi_open(ERR_DRIVER_NAME);
   
   if(err_qdi_client_handle < 0)
   {
     ERR_FATAL("err_qdi_client_init :qdi_open failed\n", 0, 0, 0);
     return;
   }

   err_qdi_client_initialized = TRUE;

   err_qdi_invoke_send_pdr_info(err_policy_pdr_is_pdr_allowed());

}  /* err_qdi_client_init */


/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_SEND_PDR_INFO

DESCRIPTION
  Sends Pid info after initialization.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_send_pdr_info
(
  boolean                       user_pdr_enabled
)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_SEND_PDR_INFO,
		                user_pdr_enabled);

} /* err_qdi_invoke_send_pid_info*/

/*===========================================================================

FUNCTION    ERR_QDI_CLIENT_SEND_PDR_INFO

DESCRIPTION
  Initialize the err client qdi service.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void err_qdi_client_send_pdr_info
(
  boolean                       user_pdr_enabled
)
{
  if ( err_qdi_client_is_initialized())
    err_qdi_invoke_send_pdr_info(user_pdr_enabled);

}  /* err_qdi_client_send_pdr_info */

/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_INFORM_PFR

DESCRIPTION
  Sends PFR after a crash . Sends the info in expception handler.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_inform_pfr
(
  uint32                        pfr_len,
  char *                        pfr
)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_INFORM_PFR, pfr_len, pfr );

} /* err_qdi_invoke_inform_pfr */

/*===========================================================================

FUNCTION    ERR_QDI_INVOKE_ERR_HANDLING_DONE

DESCRIPTIONT
  Tells QDI on ROOT PD that err completion routine on User PD has completed.
  Root PD checks if PD restart is enabled. If so, it returns. Otherwise, 
  it brings the system down.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/

err_qdi_e err_qdi_invoke_err_handling_done(void)
{
  return qurt_qdi_handle_invoke(err_qdi_client_handle, ERR_QDI_ERR_HANDLING_DONE);

} /* err_qdi_invoke_err_handling_done */

