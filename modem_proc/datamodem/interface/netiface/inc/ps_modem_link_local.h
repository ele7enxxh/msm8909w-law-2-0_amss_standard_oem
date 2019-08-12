#ifndef PS_MODEM_LINK_LOCAL_H
#define PS_MODEM_LINK_LOCAL_H
/*!
  @file
  ps_modem_link_local.h

  @brief
  This file provides function definitions of modem link local interface.

  @see
  ps_iface_defs.h

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR FILE

  Please notice that the changes are signaled in reverse chronological order.

  $Header:
  $DateTime:

  when        who     what, where, why
  --------    ---     -------------------------------------------------------
  15/03/2012  sun      Created Module.

===========================================================================*/


/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_PS_IPV6

/**
  @brief power up initialization.

  This method is called as part of PS task powerup initialization to create
  the modem link local interface

  @return
     None

  @dependencies
    None
*/
void ps_modem_link_local_powerup_init
(
  void
);

/**
  @brief Init function.

  This funtion performs task init function

  @return
     None

  @dependencies
    None
*/
void ps_modem_link_local_init
(
  void
);

#endif
#endif /* PS_MODEM_LINK_LOCAL_H */
