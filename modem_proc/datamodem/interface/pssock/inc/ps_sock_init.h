#ifndef PS_SOCK_INIT_H
#define PS_SOCK_INIT_H

/*=========================================================================*/
/*!
  @file ps_sock_init.h

  @brief
  This file provides functions that are used to perform initializations
  of PSSOCK modules.

  @detail
  This file provides methods to initialize the PSSOCK module. For AMSS
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
  Provides powerup initialization of SOCK modules.

  @detail
  This function initializes the SOCK modules during task powerup.
  The initializations that happen here are not dependent upon any other tasks
  being initialized. After this function SOCK modules are in ready state to
  recieve requests from other tasks.

  @param      None

  @return     None

  @see        psi_powerup, psi_init
*/
void ps_sock_powerup
(
  void
);

/*!
  @brief
  Provides powerdown cleanup of SOCK modules.

  @detail
  This function cleans up after the SOCK modules during task powerdown.
  
  @param      None

  @return     None

  @see
*/
void ps_sock_powerdown
(
  void
);

/*!
  @brief
  This function performs power-after initialization of SOCK moudules.

  @detail
  This function initializes the SOCK modules after the task-start
  has been signalled. This means that we can perform initialization of
  SOCK that are dependent upon other tasks.

  @param      None

  @return     None

  @see        psi_powerup, psi_init
*/
void ps_sock_init
(
  void
);

/*!
  @brief
  This function performs power-down deinitialization of SOCK moudules.

  @detail
  This function deinitializes the SOCK modules

  @param      None

  @return     None

  @see
*/
void ps_sock_deinit
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* PS_SOCK_INIT_H */
