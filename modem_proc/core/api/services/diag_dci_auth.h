#ifndef DIAG_DCI_AUTH_H
#define DIAG_DCI_AUTH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  This file contains the definitions of all the functions, enums, structures which
  are used in the external APIs defined diag_dci_auth.c file

Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/api/services/diag_dci_auth.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/12   sr      File created - Support for DCI Override feature(OCH) and 
                   On Board Diagnostics(OBD)- Secure Sim feature 
          
===========================================================================*/      

#include "comdef.h"
#include "ssm_och.h"


/*Call back type used by clients while registration. 
  This callback will be called by Diag while polling the clients*/

typedef uint32 (*diag_dci_cb_type)(void);

/*Enum definitions*/

typedef enum
{
    /*Client- User Identity Module*/
	DIAG_DCI_AUTH_CLIENT_UIM = 0,

    DIAG_DCI_AUTH_LAST_CLIENT_ID = 0  
}diag_dci_auth_client_id_type;

typedef enum
{
	 /*Registration failed*/
	 DIAG_DCI_REG_FAILED = 0,

	 /*Registration successful*/
  	 DIAG_DCI_REG_SUCCESS = 1
    
}diag_dci_auth_reg_err_t;

typedef enum
{

  /** Authentication Not Successful */
  DIAG_OBD_AUTHENTICATION_NOT_SUCCESSFUL,

      /** Authentication Successful */
  DIAG_OBD_AUTHENTICATION_SUCCESSFUL

} diag_obd_auth_status_t;

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

/*!
@ingroup diag_security
@brief
  This function is the callback called by ssm when there is a change in
  DCI status.

@dependencies
None
@sideeffects
None
@return
None
*/
	
void diag_och_ssm_cb (ssm_och_ovrr_status_t och_stat); 

/*!
@ingroup diag_security
@brief
  This function is called by diag task to register callback with ssm. The
  callback is called by ssm to notify diag about change in DCI status.

@dependencies
None
@sideeffects
None
@return
None
*/
	
void diag_och_ssm_init(void);

/*!
@ingroup diag_security
@brief
  This is the function that Diag provides to register clients so that they can
  notify Diag about change of DCI status

@dependencies
None
@sideeffects
None
@return
DIAG_DCI_REG_SUCCESS if registration was successful else DIAG_DCI_REG_FAIL
*/

diag_dci_auth_reg_err_t diag_dci_auth_reg_client(diag_dci_auth_client_id_type client_id, diag_dci_cb_type cb);

/*!
@ingroup diag_security
@brief
  This function is called by clients to notify Diag about change in DCI
  status.

@dependencies
None
@sideeffects
None
@return
None
*/

void diag_dci_auth_notify (diag_dci_auth_client_id_type client_id, uint32 status); 


#endif /*DIAG_DCI_AUTH_H*/
