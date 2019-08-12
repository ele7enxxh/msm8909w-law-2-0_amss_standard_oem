/*==========================================================================*/
/*!
  @file 
  ps_netiface_init.c

  @brief
  This file provides functions that are used to perform initializations 
  of netiface module.

  Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_netiface_init.c#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-07-14 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "ps_netiface_init.h"
#include "ps_iface.h"
#include "ps_iface_flow.h"
#include "ps_ifacei.h"
#include "ps_flowi.h"
#include "ps_phys_linki.h"
#include "ps_phys_linki_event.h"
#include "ps_flowi_event.h"
#include "ps_policyi_mgr.h"
#include "ps_qos_net_initiated_req.h"
#include "ps_iface_ipfltr.h"


/*---------------------------------------------------------------------------
  Local declarations.
---------------------------------------------------------------------------*/

void ps_netiface_powerup
(
  void
)
{
  /*------------------------------------------------------------------------
    Initialize the global PS critical section
  -------------------------------------------------------------------------*/
  ps_iface_init();
#ifdef FEATURE_DATA_PS_QOS
  ps_iface_flow_init();

  ps_iface_qos_net_initiated_req_init();

#endif /* FEATURE_DATA_PS_QOS */

  ps_iface_ipfltr_powerup();

  /*-------------------------------------------------------------------------
    Initialize Phys Link, PS flow, and PS iface events module.
    Must be done in this order.
  -------------------------------------------------------------------------*/
  ps_phys_link_event_init();
  ps_flow_event_init();
  ps_iface_event_init();

  /*-------------------------------------------------------------------------
    Initialize the policy manager.
  -------------------------------------------------------------------------*/
  ps_policy_mgr_init();

  /*-------------------------------------------------------------------------
    Register command handlers for flow control commands
  -------------------------------------------------------------------------*/
  (void) ps_set_cmd_handler( PS_IFACE_FLOW_CTRL_CMD,
                             ps_iface_flow_ctrl_cmd_handler);
  (void) ps_set_cmd_handler( PS_FLOW_FLOW_CTRL_CMD,
                             ps_flowi_flow_ctrl_cmd_handler);
  (void) ps_set_cmd_handler( PS_PHYS_LINK_FLOW_CTRL_CMD,
                             ps_phys_link_flow_ctrl_cmd_handler);
  (void) ps_set_cmd_handler( PS_IFACE_GO_NULL_CTRL_CMD,
                             ps_iface_go_null_cmd_ex_handler);                             

  return;

} /* ps_netiface_powerup() */

void ps_netiface_powerdown
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // TODO what cleanup should take place here?

} /* ps_netiface_powerdown() */

void ps_netiface_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize IPFltr module
  -------------------------------------------------------------------------*/  
  ps_iface_ipfltr_init();
  
} /* ps_netiface_init() */

void ps_netiface_deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Currently a no-op
  -------------------------------------------------------------------------*/

} /* ps_netiface_deinit() */



