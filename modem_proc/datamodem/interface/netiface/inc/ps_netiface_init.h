#ifndef PS_NETIFACE_INIT_H
#define PS_NETIFACE_INIT_H

/*=========================================================================*/
/*!
  @file 
  ps_netiface_init.h

  @brief
  This file provides functions that are used to perform initializations 
  of NETIFACE modules.

  @detail
  This file provides methods to initialize the NETIFACE module. For AMSS
  environments the PS task initialization module calls these methods. 
  For 3rd party platforms, these methods need to be called by the platform
  specific initialization module before using NETIFACE.
 
  Exported functions:
  ps_netiface_powerup() 
  Initialization of NETIFACE libraries that happen before task start is
  signalled to the task main controller.

  ps_netiface_init()
  Initialization of NETIFACE that require other modules/tasks to be 
  initialized. This function gets called after the caller's task start is 
  signalled.

  @note
  Both these functions can be called at-most once.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_netiface_init.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-07-14 hm  Created module.

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
  Provides powerup initialization of NETIFACE modules.

  @detail
  This function initializes the NETIFACE modules during task powerup.
  The initializations that happen here are not dependent upon any other
  tasks being initialized. After this function NETIFACE modules are in
  ready state to recieve requests from other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void ps_netiface_powerup
(
  void
);

/*!
  @brief
  Provides powerdown cleanup of NETIFACE modules.

  @detail
  This function cleans up the NETIFACE modules during task powerdown.

  @param      None.
  @see        ps_netiface_powerup()
  @return     None.
*/
void ps_netiface_powerdown
(
  void
);

/*!
  @brief
  This function performs power-after initialization of NETIFACE moudules.

  @detail
  This function initializes the NETIFACE modules after the task-start 
  has been signalled. This means that we can perform initialization of 
  NETIFACE that are dependent upon other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void ps_netiface_init
(
  void
);

/*!
  @brief
  This function performs powerdown-before clean up of NETIFACE moudules.

  @detail
  This function cleans up the NETIFACE modules after the task-end 
  has been signalled.

  @param      None.
  @see        ps_netiface_init()
  @return     None.
*/
void ps_netiface_deinit
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_NETIFACE_INIT_H */




