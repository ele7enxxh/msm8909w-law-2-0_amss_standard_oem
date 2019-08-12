/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               PDCOMMSNV Items using EFS API Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the PDCOMMS Non-Volatile Memory interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/pd_comms/inc/pd_comms_nv_efs.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/10   ssheshad     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef PD_COMMS_NV_EFS_H
#define PD_COMMS_NV_EFS_H

/*
 * Include files
*/
#include "comdef.h"
#include "customer.h"
#include "gps_nv_efs.h"
#include "nv.h"

/* All the NV EFS values are initialzed here */
/* default value for the RSA key length */
#define CGPS_NV_EFS_PDCOMM_SSL_RSA_CERTIFICATE_KEY_LENGTH_DEFAULT (256)


/* add all the default values for NV items here */
/*
 * Structure definitions
*/
/*===========================================================================

FUNCTION sm_nv_efs_init

DESCRIPTION
  This function is used to initialize internal PDCOMM-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void pd_comms_nv_efs_init (void);

/*===========================================================================

FUNCTION pd_comms_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read NV item. If the read fails, it sets it to
  default value.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean pd_comms_nv_efs_reg_item_read(cgps_nv_efs_reg_item_type const item, 
                                      uint32 data_size,
                                      void *data_ptr,
                                      boolean force_read_from_hw                                      
                                      );

/*===========================================================================

FUNCTION pd_comms_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an NV item. 
 
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean pd_comms_nv_efs_reg_item_write(cgps_nv_efs_reg_item_type const item, 
                                       uint32 data_size,
                                       void *data_ptr
                                      );

#endif /* PD_COMMS_NV_EFS_H */

