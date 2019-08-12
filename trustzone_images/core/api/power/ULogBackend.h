#ifndef ULOGBACKEND_H
#define ULOGBACKEND_H

/*============================================================================
  @file ULogBackend.h

  Frontend interfaces for ULog Core.  Simplified access to the logging mechanism.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ULogBackend.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 @brief ULogBackend_Init - Initialize all the ULog Backends.
 
 Initialize any and all ULog backends.  This function will set
 up and enable all backend methods for data retrieval from the
 ULog system.
 
*/
void ULogBackend_Init (void);


#endif // ULOGBACKEND_H

