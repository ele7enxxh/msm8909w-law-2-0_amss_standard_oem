#ifndef DS_SOCK_INIT_H
#define DS_SOCK_INIT_H

/*=========================================================================*/
/*!
  @file 
  ds_Sock_Init.h

  @brief
  This file provides functions that are used to perform initializations 
  of DSSOCK modules.

  @detail
  This file provides methods to initialize the DSSOCK modules. 
  Currently, these initializations happen during PS task init. However,
  these functions can be ported to be called from any other task/library 
  as well. 
 
  Exported functions:
  DSSockPowerup() 
  Initialization of DSSOCK libraries that happen before task start is
  signalled to the task main controller.

  DSSockInit()
  Initialization of DSSOCK that require other modules/tasks to be 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_Init.h#1 $
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
  Provides powerup initialization of DSSOCK modules.

  @detail
  This function initializes the DSSOCK modules during task powerup.
  The initializations that happen here are not dependent upon any other
  tasks being initialized. After this function DSSOCK modules are in
  ready state to recieve requests from other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void DSSockPowerup
(
  void
);

/*!
  @brief
  Provides powerdown cleanup of DSSOCK modules.

  @detail
  This function cleans up the DSSOCK modules during task powerdown.

  @param      None.
  @see        DSSockPowerup()
  @return     None.
*/
void DSSockPowerdown
(
  void
);

/*!
  @brief
  This function performs power-after initialization of DSSOCK moudules.

  @detail
  This function initializes the DSSOCK modules after the task-start 
  has been signalled. This means that we can perform initialization of 
  DSSOCK that are dependent upon other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void DSSockInit
(
  void
);

/*!
  @brief
  This function performs powerdown-before cleanup of DSSOCK moudules.

  @detail
  This function cleans up the DSSOCK modules after the task-start 
  has been signalled.

  @param      None.
  @see        DSSockInit()
  @return     None.
*/
void DSSockDeinit
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* DS_SOCK_INIT_H */



