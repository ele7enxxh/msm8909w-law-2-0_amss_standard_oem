/*=============================================================================

                err_qdi_client_initialized.c

GENERAL DESCRIPTION
      ERR QDI Client Process Code that goes to island image, if island mode is
      enabled , otherwise goes into main image

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2015
by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_qdi_client_initialized.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/28/15   din     Adapted from err_qdi_client.c
=============================================================================*/
#include "com_dtypes.h"

boolean err_qdi_client_initialized = FALSE;

/*===========================================================================

FUNCTION    ERR_QDI_CLIENT_IS_INITIALIZED

DESCRIPTION
  Initialize the err client qdi service.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE
  
SIDE EFFECTS
  None

===========================================================================*/
boolean err_qdi_client_is_initialized(void) 
{
 
   return err_qdi_client_initialized;

}  /* err_qdi_client_is_initialized */


