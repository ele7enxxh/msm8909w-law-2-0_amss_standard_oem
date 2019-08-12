#ifndef OEMDRSYNC_H
#define OEMDRSYNC_H
/*=============================================================================

                        OEM DR Sync Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the OEM DR Enablement.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/oem/inc/sm_oemdrsync.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/10   ans     Initial version.

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "gnss_common.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros

=============================================================================*/

/*=============================================================================

       Typedefs

=============================================================================*/


/*=============================================================================

       Functions

=============================================================================*/

/*****************************OEM DRE API functions***********************/

/*===========================================================================

FUNCTION oemDrSyncSetConfig()

DESCRIPTION
  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemDrSyncSetConfig(gnss_DRSyncConfigType *pOemDrSyncConfigType);

/*===========================================================================

FUNCTION oemDrSyncGetConfig()

DESCRIPTION
  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemDrSyncGetConfig(void);

/*===========================================================================

FUNCTION oemDrSyncGetConfig()

DESCRIPTION
  
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean oemDrSyncArm(void);

/*===========================================================================

FUNCTION oemDrSyncConfig()

DESCRIPTION
  SM calls this function to provide DR Sync configuration to the OEM layer.
  This function is called called as a result of oemDrSyncGetConfig() call.
  
DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void oemDrSyncConfig(const gnss_DRSyncConfigType* pOemDrSyncConfig);

/*===========================================================================

FUNCTION oemDrSyncPulseDesc()

DESCRIPTION
  SM calls this function to provide DR Sync pulse description when a 
  pulse is generated.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void oemDrSyncPulseDescriptor(const gnss_DRSyncPulseDescType *pOemDrSyncPulseDesc);

#endif /* OEMDRSYNC_H */

