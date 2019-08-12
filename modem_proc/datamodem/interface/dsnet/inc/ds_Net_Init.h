#ifndef DS_NET_INIT_H
#define DS_NET_INIT_H

/*=========================================================================*/
/*!
  @file 
  ds_Net_Init.h

  @brief
  This file provides functions that are used to perform initializations 
  of DSNET modules.

  @detail
  This file provides methods to initialize the DSNET modules. 
  Currently, these initializations happen during PS task init. However,
  these functions can be ported to be called from any other task/library 
  as well. 
 
  Exported functions:
  DSNetPowerup() 
  Initialization of DSNET libraries that happen before task start is
  signalled to the task main controller.

  DSNetInit()
  Initialization of DSNET that require other modules/tasks to be 
  initialized. This function gets called after the caller's task start is 
  signalled.

  @note
  Both these functions can be called at-most once.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/inc/ds_Net_Init.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-07-16 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Provides powerup initialization of DSNET modules.

  @detail
  This function initializes the DSNET modules during task powerup.
  The initializations that happen here are not dependent upon any other
  tasks being initialized. After this function DSNET modules are in
  ready state to recieve requests from other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void DSNetPowerup
(
  void
);

/*!
  @brief
  Provides powerup cleanup of DSNET modules.

  @detail
  This function cleansup the DSNET modules during task powerdown.

  @param      None.
  @see        DSNetPowerup()
  @return     None.
*/
void DSNetPowerdown
(
  void
);

/*!
  @brief
  This function performs power-after initialization of DSNET moudules.

  @detail
  This function initializes the DSNET modules after the task-start 
  has been signalled. This means that we can perform initialization of 
  DSNET that are dependent upon other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void DSNetInit
(
  void
);

/*!
  @brief
  This function performs powerown-before cleanup of DSNET moudules.

  @detail
  This function initializes the DSNET modules after the task-start 
  has been signalled. This means that we can perform initialization of 
  DSNET that are dependent upon other tasks.

  @param      None.
  @see        DSNetInit()
  @return     None.
*/
void DSNetDeinit
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* DS_NET_INIT_H */


