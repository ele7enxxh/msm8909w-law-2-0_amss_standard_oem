#ifndef DIAG_DCI_AUTH_V_H
#define DIAG_DCI_AUTH_V_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  This file contains the definitions of all the functions, enums, structures which
  are internal and declared in diag_dci_auth.c file

Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag_dci_auth_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   sr      File created
      
      
===========================================================================*/      

#include "comdef.h"
#include "ssm_och.h"

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

/*!
@ingroup diag_security
@brief
   This function is called by diag task to query if the dci port should be
  opened or not.It is also called when either ssm or one of the
  registered clients notifies diag to close the DCI port. This is an internal
  function and is always called in diag's context.

@dependencies
None
@sideeffects
None
@return
If the return value is TRUE, then DCI port is opened. Else, DCI port is
  not opened. 
*/

boolean diag_dci_open_query(void);

/*!
@ingroup diag_security
@brief
   Return SSM DCI Override init error code.

@dependencies
None
@sideeffects
None
@return
 SSM DCI Override init error code.
*/
	
ssm_och_err_t diag_och_ssm_get_error( void );

#endif /*DIAG_DCI_AUTH_V_H*/

