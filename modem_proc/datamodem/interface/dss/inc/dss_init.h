#ifndef DSS_INIT_H
#define DSS_INIT_H

/*=========================================================================*/
/*!
  @file 
  dss_init.h

  @brief
  This file provides functions that are used to perform initializations 
  of DSS modules.

  @detail
  This file provides methods to initialize the dss module. For AMSS
  environments the PS task initialization module calls these methods. 
  For 3rd party platforms, these methods need to be called by the platform
  specific initialization module before using dss.
 
  Exported functions:
  dss_powerup() 
  Initialization of DSS libraries that happen before task start is
  signalled to the task main controller.

  dss_init()
  Initialization of DSS that require other modules/tasks to be 
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/inc/dss_init.h#1 $
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
  Provides powerup initialization of DSS modules.

  @detail
  This function initializes the DSS modules during task powerup.
  The initializations that happen here are not dependent upon any other
  tasks being initialized. After this function DSS modules are in
  ready state to recieve requests from other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void dss_powerup
(
  void
);

/*!
  @brief
  Provides powerdown cleanup of DSS modules.

  @detail
  This function cleansup the DSS modules during task powerdown.

  @param      None.
  @see        dss_powerup()
  @return     None.
*/
void dss_powerdown
(
  void
);

/*!
  @brief
  This function performs power-after initialization of DSS moudules.

  @detail
  This function initializes the DSS modules after the task-start 
  has been signalled. This means that we can perform initialization of 
  DSS that are dependent upon other tasks.

  @param      None.
  @see        psi_powerup() for reference task initializing this module.
  @see        psi_init()
  @return     None.
*/
void dss_init
(
  void
);

/*!
  @brief
  This function performs powerdown-before cleanup of DSS moudules.

  @detail
  This function cleansup the DSS modules after the task-end 
  has been signalled.

  @param      None.
  @see        dss_init()
  @return     None.
*/
void dss_deinit
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* DSS_INIT_H */



