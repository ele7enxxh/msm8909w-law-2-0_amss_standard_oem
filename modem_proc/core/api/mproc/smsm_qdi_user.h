#ifndef SMSM_QDI_USER_H
#define SMSM_QDI_USER_H
/*===========================================================================

                            SMSM QDI Driver Header

  @file
    smsm_qdi_driver.h

  This is the QDI Driver header for SMSM in the ADSP Guest OS.

  Copyright (c) 2013 Qualcomm Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/mproc/smsm_qdi_user.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/13   bt      Unify GuestOS and Userspace SMSM client interfaces.
01/17/13   bt      Initial version of QDI implementation for SMSM driver.
==========================================================================*/
/*===========================================================================
                   INCLUDE FILES
===========================================================================*/

/*===========================================================================
                   CONSTANT AND MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                   TYPE DECLARATIONS
===========================================================================*/


/*===========================================================================
                   PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smsm_user_init
===========================================================================*/
/**
  Initializes the Userspace QDI interface to use SMSM functions from 
  userspace.  Increments the SMSM Userspace QDI Driver reference count.
  
  When the reference count goes from 0 to 1 (first initialization), it will
  launch the userspace callback worker thread.

  @return
  0 if successful.
  A negative error code if the function failed.
  
  @dependencies
  SMSM GuestOS driver must have already been initialized.
  SMSM GuestOS QDI layer must have been registered.
*/
/*=========================================================================*/
int smsm_user_init(void);

/*===========================================================================
  FUNCTION  smsm_user_deinit
===========================================================================*/
/**
  Decrements the SMSM Userspace QDI Driver reference count.  If the reference
  count reaches 0, it deinitializes the userspace driver, and halts the 
  callback worker thread.

  @return
  None
*/
/*=========================================================================*/
void smsm_user_deinit(void);

#endif /* SMSM_QDI_USER_H */
