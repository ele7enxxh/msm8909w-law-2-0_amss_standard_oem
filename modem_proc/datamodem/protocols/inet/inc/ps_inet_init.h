#ifndef PS_INET_INIT_H
#define PS_INET_INIT_H

/*=========================================================================*/
/*!
  @file ps_inet_init.h

  @brief
  This file provides functions that are used to perform initializations
  of INET modules.

  @detail
  This file provides methods to initialize the INET module. For AMSS
  environments the PS task initialization module calls these methods.

  @note
  Both these functions can be called at-most once.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================

                     EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*!
  @brief
  Provides powerup initialization of INET modules.

  @detail
  This function initializes the INET modules during task powerup.
  The initializations that happen here are not dependent upon any other tasks
  being initialized. After this function INET modules are in ready state to
  recieve requests from other tasks.

  @param      None

  @return     None

  @see        psi_powerup, psi_init
*/
void ps_inet_powerup
(
  void
);

/*!
  @brief
  Provides powerdown cleanup of INET modules.

  @detail
  This function cleans up after the INET modules during task powerdown.
  
  @param      None

  @return     None

  @see
*/
void ps_inet_powerup
(
  void
);

/*!
  @brief
  This function performs power-after initialization of INET moudules.

  @detail
  This function initializes the INET modules after the task-start
  has been signalled. This means that we can perform initialization of
  INET that are dependent upon other tasks.

  @param      None

  @return     None

  @see        psi_powerup, psi_init
*/
void ps_inet_init
(
  void
);

/*!
  @brief
  This function performs power-down deinitialization of INET moudules.

  @detail
  This function deinitializes the INET modules

  @param      None

  @return     None

  @see
*/
void ps_inet_deinit
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_INET_INIT_H */
