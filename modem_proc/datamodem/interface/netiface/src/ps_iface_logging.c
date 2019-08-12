/*===========================================================================

                        P S _ I F A C E _ L O G G I N G . C

GENERAL DESCRIPTION
  This file defines external API used by mode handlers to enable logging and
  to provide description for various logging structures.

EXTERNAL FUNCTIONS
  ps_iface_dpl_get_iface_desc()
    Get ps_iface's description

  ps_iface_dpl_set_iface_desc()
    Set ps_iface's description

  ps_iface_dpl_get_flow_desc()
    Get a flow's(ps_phys_link) description

  ps_iface_dpl_set_flow_desc()
    Set a flow's(ps_phys_link) description

  ps_iface_dpl_set_phys_link_desc()
    Set a phys link's(for eg, RLP/RLC instance) description

  ps_iface_dpl_support_network_logging()
    Support network logging on the specified interface

  ps_iface_dpl_support_flow_logging()
    Support flow logging on the specified interface

  ps_iface_dpl_support_link_logging()
    Support link logging on the specified interface

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_iface_logging.c#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/15/10    hs     Added DPL support on two new LTE Ifaces
11/21/08    am     Fixed incorrect setting of link bit in IID of
                   iface/flow/phys_link. Using new macro names for IID flags.
10/17/08    am     Fixed ANSI C warnings for bit-field usage.
12/25/06    msr    Fixed broken secondary link logging
07/17/06    mp     Fixed logging of zero length DPL packets
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link and added defualt ppp logging settings
02/22/06    msr    Using single critical section
02/14/06    ks     fixed compile warnings.
02/13/06    msr    Remoed some of critical sections
02/06/06    msr    Updated for L4 tasklock/crit sections.
10/31/05    msr    Changed default snaplen to 80 for PDP context 0, 1, and 2
08/22/05    msr    Not allowing flow/link logging on logical ifaces
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    msr    Using ps_flows instead of ps_phys_links to support flow
                   logging
05/12/05    ks     Fixed lint errors.
03/14/05    ks     Fixed setting of link bit in
                   ps_iface_dpl_support_link_logging().
03/02/05    msr    Moved default settings to
                   ps_iface_dpl_set_network_logging().
02/03/05    msr    Settings IP logging by default for UMTS Um ifaces.
01/13/05    ks     fixed ps_iface_dpl_set_link_desc().
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
10/31/04   ks/msr  Created module.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "ps_iface.h"
#include "ps_utils.h"
#include "ps_logging_defs.h"
#include "ps_iface_logging.h"
#include "ps_loggingi.h"

#include <stringl/stringl.h>
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_logging_diag.h"

/*===========================================================================

                          EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION     PS_IFACE_DPL_SET_IFACE_DESC

DESCRIPTION
  Set ps_iface description in corresponding ps_iface structure

PARAMETERS
  ps_iface_ptr : pointer to ps_iface structure
  desc         : desc to be set

RETURN VALUE
  TRUE on sucess
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_dpl_set_iface_desc
(
  ps_iface_type  * ps_iface_ptr,
  char           * desc
)
{
  dpl_state_enum_type dpl_state = DPL_DISABLED;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_set_iface_desc(): "
                    "Invalid iface, 0x%p, passed", ps_iface_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    ifname must have been set if logging is enabled on this ps_iface
  -------------------------------------------------------------------------*/
  if (!(DPL_IID_IFNAME_MIN <= ps_iface_ptr->dpl_net_cb.tx_dpl_id.ifname &&
        DPL_IID_IFNAME_MAX > ps_iface_ptr->dpl_net_cb.tx_dpl_id.ifname))
  {
    LOG_MSG_INFO2_1("ps_iface_dpl_set_iface_desc(): "
                    "Network logging is not enabled on this iface 0x%p",
                    ps_iface_ptr);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If NULL is passed as parameter, get default description from
    dpl lookup table
  -------------------------------------------------------------------------*/
  if (NULL == desc)
  {
    desc = (char *) dpli_get_ps_iface_default_desc(
        (dpl_iid_ifname_enum_type)ps_iface_ptr->dpl_net_cb.tx_dpl_id.ifname);
  }

  /*-------------------------------------------------------------------------
    Ensure that desc is atmost DPL_IFNAME_DESC_S_LEN characters long and is
    NULL terminated
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ps_iface_ptr->dpl_net_cb.desc[DPL_IFNAME_DESC_S_LEN - 1] = '\0';
  (void) snprintf(ps_iface_ptr->dpl_net_cb.desc,
                        DPL_IFNAME_DESC_S_LEN - 1,
                        "%s:0x%X:%d",
                        desc,
                        ps_iface_ptr->name,
                        ps_iface_ptr->instance);
  
  dpl_state = dpl_get_dpl_state();

  if (DPL_DISABLED != dpl_state)
  {
    dpli_update_logging_config(ps_iface_ptr,dpl_state);
  }
  
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return TRUE;

} /* ps_iface_dpl_set_iface_desc() */



/*===========================================================================
FUNCTION     PS_IFACE_DPL_SET_FLOW_DESC

DESCRIPTION
  Set flow description in corresponding ps_flow structure

PARAMETERS
  flow_ptr : pointer to ps_flow structure
  desc     : desc to be set

RETURN VALUE
  TRUE on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_dpl_set_flow_desc
(
  ps_flow_type  * flow_ptr,
  char          * desc
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_1("ps_iface_dpl_set_flow_desc(): "
                    "Invalid flow ptr, 0x%p, passed", flow_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    ifname must have been set if logging is enabled on this ps_flow
  -------------------------------------------------------------------------*/
  if (!(DPL_IID_IFNAME_MIN <= flow_ptr->dpl_flow_cb.tx_dpl_id.ifname &&
        DPL_IID_IFNAME_MAX > flow_ptr->dpl_flow_cb.tx_dpl_id.ifname))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO2_1("ps_iface_dpl_set_flow_desc(): "
                    "Flow logging is not enabled on ps_flow 0x%p", flow_ptr);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Ensure that desc is atmost DPL_FLOW_DESC_S_LEN characters long and is
    NULL terminated
  -------------------------------------------------------------------------*/
  flow_ptr->dpl_flow_cb.desc[DPL_FLOW_DESC_S_LEN - 1] = 0;
  if (NULL != desc)
  {
    (void) snprintf(flow_ptr->dpl_flow_cb.desc,
                          DPL_FLOW_DESC_S_LEN - 1,
                          "%s:0x%p",
                          desc,
                          flow_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Use a default description
    -----------------------------------------------------------------------*/
    (void) snprintf(flow_ptr->dpl_flow_cb.desc,
                          DPL_FLOW_DESC_S_LEN - 1,
                          "%s %d:0x%p",
                          "Flow",
                          PS_FLOWI_GET_COOKIE(flow_ptr),
                          flow_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return TRUE;

} /* ps_iface_dpl_set_flow_desc() */


/*===========================================================================
FUNCTION     PS_IFACE_DPL_SET_PHYS_LINK_DESC

DESCRIPTION
  Set phys link description in corresponding phys link structure

PARAMETERS
  phys_link_ptr : pointer to phys link
  desc          : desc to be set

RETURN VALUE
  TRUE on success
  FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_dpl_set_phys_link_desc
(
  ps_phys_link_type  * phys_link_ptr,
  char               * desc
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!PS_PHYS_LINK_IS_VALID(phys_link_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_set_phys_link_desc(): "
                    "Invalid phys link ptr, 0x%p, passed", phys_link_ptr);
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    ifname must have been set if logging is enabled on this ps_iface
  -------------------------------------------------------------------------*/
  if (!(DPL_IID_IFNAME_MIN <= phys_link_ptr->dpl_link_cb.tx_dpl_id.ifname &&
        DPL_IID_IFNAME_MAX > phys_link_ptr->dpl_link_cb.tx_dpl_id.ifname))
  {
    LOG_MSG_INFO2_1("ps_iface_dpl_set_phys_link_desc(): "
                    "Link logging is not enabled on phys link 0x%p",
                    phys_link_ptr);
    return FALSE;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Ensure that desc is atmost DPL_LINK_DESC_S_LEN characters long and is
    NULL terminated
  -------------------------------------------------------------------------*/
  phys_link_ptr->dpl_link_cb.desc[DPL_LINK_DESC_S_LEN - 1] = '\0';
  if (NULL != desc)
  {
    (void) snprintf(phys_link_ptr->dpl_link_cb.desc,
                          DPL_LINK_DESC_S_LEN - 1,
                          "%s",
                          desc);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Use a default description
    -----------------------------------------------------------------------*/
    (void) snprintf(phys_link_ptr->dpl_link_cb.desc,
                          DPL_LINK_DESC_S_LEN - 1,
                          "Link %d",
                          phys_link_ptr->phys_private.instance);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return TRUE;

} /* ps_iface_dpl_set_phys_link_desc() */



/*===========================================================================
FUNCTION    PS_IFACE_DPL_SUPPORT_NETWORK_LOGGING

DESCRIPTION
  Populate ifname in logging control block in ps_iface to indicate that
  logging is supported on this interface.

PARAMETERs
  ps_iface_ptr : ptr to iface
  ifname       : IID ifname to be set

RETURN VALUE
  0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_dpl_support_network_logging
(
  ps_iface_type            * ps_iface_ptr,
  dpl_iid_ifname_enum_type   ifname
)
{
#ifdef FEATURE_DATA_PS_DEBUG
  ps_iface_type  ** if_ptr;
  int               i;
#endif /* FEATURE_DATA_PS_DEBUG */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DPL_IID_IFNAME_MIN > ifname || DPL_IID_IFNAME_MAX <= ifname)
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_support_network_logging(): "
                    "Invalid ifname (%d) passed", ifname);
    ASSERT(0);
    return -1;
  }

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_support_network_logging(): "
                    "Invalid iface, 0x%p, is passed", ps_iface_ptr);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Do nothing if ifname is already set. Same ifname is set in both tx_cb
    and recv_cb.
  -------------------------------------------------------------------------*/
  if (DPL_IID_IFNAME_MIN <= ps_iface_ptr->dpl_net_cb.tx_dpl_id.ifname)
  {
    LOG_MSG_INFO2_1("ps_iface_dpl_support_network_logging(): "
                    "Trying to set ifname for an already set iface_ptr 0x%p",
                    ps_iface_ptr);
    return 0;
  }

#ifdef FEATURE_DATA_PS_DEBUG
  /*-------------------------------------------------------------------------
    Ensure that there is no iface with the same ifname as the one passed as
    parameter. Since Ifaces are never destroyed, there is no need to traverse
    global_iface_ptr_array once we hit a NULL.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  if_ptr = global_iface_ptr_array;
  for (i = 0; (i < MAX_SYSTEM_IFACES) && (NULL != *if_ptr); i++, if_ptr++)
  {
    if (ifname == (*if_ptr)->dpl_net_cb.tx_dpl_id.ifname)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION (&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_dpl_support_network_logging(): "
                      "Duplicate ifname (%d) passed to iface_ptr 0x%p. The other "
                      "iface_ptr is 0x%p", ifname, ps_iface_ptr, *if_ptr);
      ASSERT(0);
      return -1;
    }
  }
  PS_LEAVE_CRIT_SECTION (&global_ps_crit_section);
#endif /* FEATURE_DATA_PS_DEBUG */

  /*-------------------------------------------------------------------------
    Once logging is supported, the cached IID values are initialized in the
    log flags structs.  Hence, cached IID != 0 will indicate support.
  -------------------------------------------------------------------------*/
  ps_iface_ptr->dpl_net_cb.tx_dpl_id.ifname     = ifname;
  SET_DPL_IID_L_BIT_NETWORK(ps_iface_ptr->dpl_net_cb.tx_dpl_id);
  SET_DPL_IID_F_BIT_NONFLOW(ps_iface_ptr->dpl_net_cb.tx_dpl_id);
  SET_DPL_IID_DIR_TX(ps_iface_ptr->dpl_net_cb.tx_dpl_id);

  ps_iface_ptr->dpl_net_cb.recv_dpl_id.ifname     = ifname;
  SET_DPL_IID_L_BIT_NETWORK(ps_iface_ptr->dpl_net_cb.recv_dpl_id);
  SET_DPL_IID_F_BIT_NONFLOW(ps_iface_ptr->dpl_net_cb.recv_dpl_id);
  SET_DPL_IID_DIR_RX(ps_iface_ptr->dpl_net_cb.recv_dpl_id);

  dpli_set_ps_iface_ptr(ps_iface_ptr, ifname);

  return 0;
} /* ps_iface_dpl_support_network_logging() */



/*===========================================================================
FUNCTION    PS_IFACE_DPL_SUPPORT_FLOW_LOGGING

DESCRIPTION
  Populate ifname in logging control block in default ps_flow of an ps_iface
  to indicate that flow logging is supported on this ps_iface

PARAMETERs
  ps_iface_ptr : ptr to iface
  ifname       : IID ifname to be set

RETURN VALUE
   0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_dpl_support_flow_logging
(
  ps_iface_type            * ps_iface_ptr,
  dpl_iid_ifname_enum_type   ifname
)
{
  ps_flow_type   * flow_ptr;
#ifdef FEATURE_DATA_PS_DEBUG
  ps_iface_type  ** if_ptr;
  int              i;
#endif /* FEATURE_DATA_PS_DEBUG */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DPL_IID_IFNAME_MIN > ifname || DPL_IID_IFNAME_MAX <= ifname)
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_support_flow_logging(): "
                    "Invalid ifname (%d) passed", ifname);
    ASSERT(0);
    return -1;
  }

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_support_flow_logging(): "
                    "Invalid iface_ptr, 0x%p, is passed", ps_iface_ptr);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    As QoS is not supported on logical ifaces, they do not have secondary
    flows
  -------------------------------------------------------------------------*/
  if (PS_IFACEI_IS_LOGICAL(ps_iface_ptr))
  {
    LOG_MSG_ERROR_2("ps_iface_dpl_support_flow_logging(): "
                    "Flow logging is not supported on logical iface, 0x%x:%d",
                    ps_iface_ptr->name, ps_iface_ptr->instance);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Do nothing if ifname is already set.
  -------------------------------------------------------------------------*/
  flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(ps_iface_ptr);
  ASSERT(PS_FLOW_IS_VALID(flow_ptr));

  if (DPL_IID_IFNAME_MIN <= flow_ptr->dpl_flow_cb.tx_dpl_id.ifname)
  {
    LOG_MSG_INFO2_2("ps_iface_dpl_support_flow_logging(): "
                    "Trying to enable flow logging on an iface (0x%x:%d) which "
                    "already supports it",
                    ps_iface_ptr->name, ps_iface_ptr->instance);
    return 0;
  }

#ifdef FEATURE_DATA_PS_DEBUG
  /*-------------------------------------------------------------------------
    Ensure that there is no iface with the same ifname as the one passed as
    parameter. Since Ifaces are never destroyed, there is no need to traverse
    global_iface_ptr_array once we hit a NULL.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  if_ptr = global_iface_ptr_array;
  for (i = 0; (i < MAX_SYSTEM_IFACES) && (NULL != *if_ptr); i++, if_ptr++)
  {
    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(*if_ptr);
    ASSERT(PS_FLOW_IS_VALID(flow_ptr));

    if (ifname == flow_ptr->dpl_flow_cb.tx_dpl_id.ifname)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_dpl_support_flow_logging(): "
                      "Duplicate ifname (%d) passed to iface_ptr 0x%p. The other "
                      "iface_ptr is 0x%p", ifname, ps_iface_ptr, *if_ptr);
      ASSERT(0);
      return -1;
    }
  }
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
#endif /* FEATURE_DATA_PS_DEBUG */

  /*-------------------------------------------------------------------------
    Once logging is supported, the cached IID values are initialized in the
    log flags structs.  Hence, cached IID != 0 will indicate support.
  -------------------------------------------------------------------------*/
  flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(ps_iface_ptr);
  ASSERT(PS_FLOW_IS_VALID(flow_ptr));

  flow_ptr->dpl_flow_cb.tx_dpl_id.ifname          = ifname;
  SET_DPL_IID_L_BIT_NETWORK(flow_ptr->dpl_flow_cb.tx_dpl_id);
  SET_DPL_IID_F_BIT_FLOW(flow_ptr->dpl_flow_cb.tx_dpl_id);
  flow_ptr->dpl_flow_cb.tx_dpl_id.link_instance   = 0;
  SET_DPL_IID_DIR_TX(flow_ptr->dpl_flow_cb.tx_dpl_id);

  dpli_set_ps_iface_ptr(ps_iface_ptr, ifname);
  return 0;

} /* ps_iface_dpl_support_flow_logging() */



/*===========================================================================
FUNCTION    PS_IFACE_DPL_SUPPORT_LINK_LOGGING

DESCRIPTION
  Populate ifname in logging control block in link_cb_ptr to indicate that
  logging is supported on this interface.

PARAMETERs
  ps_iface_ptr : ptr to iface
  ifname       : IID ifname to be set

RETURN VALUE
  0 on success
  -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_dpl_support_link_logging
(
  ps_iface_type            * ps_iface_ptr,
  dpl_iid_ifname_enum_type   ifname
)
{
  ps_phys_link_type  * phys_link_ptr;
  uint8                i;
#ifdef FEATURE_DATA_PS_DEBUG
  ps_iface_type     ** if_ptr;
#endif /* FEATURE_DATA_PS_DEBUG */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DPL_IID_IFNAME_MIN > ifname || DPL_IID_IFNAME_MAX <= ifname)
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_support_link_logging(): "
                    "Invalid ifname (%d) passed", ifname);
    ASSERT(0);
    return -1;
  }

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_iface_dpl_support_link_logging(): "
                    "Invalid iface, 0x%p, is passed", ps_iface_ptr);
    ASSERT(0);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Logical ifaces do not have any phys links
  -------------------------------------------------------------------------*/
  if (PS_IFACE_IS_LOGICAL(ps_iface_ptr))
  {
    LOG_MSG_ERROR_2("ps_iface_dpl_support_link_logging(): "
                    "Link logging is not supported on logical iface, 0x%x:%d",
                    ps_iface_ptr->name, ps_iface_ptr->instance);
    return -1;
  }

  phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(ps_iface_ptr);
  ASSERT(PS_PHYS_LINK_IS_VALID(phys_link_ptr));

  /*-------------------------------------------------------------------------
    Do nothing if ifname is already set.
  -------------------------------------------------------------------------*/
  if (DPL_IID_IFNAME_MIN <= phys_link_ptr->dpl_link_cb.tx_dpl_id.ifname)
  {
    LOG_MSG_INFO2_2("ps_iface_dpl_support_link_logging(): "
                    "Trying to enable link logging on an iface (0x%x:%d) which "
                    "already supports it",
                    ps_iface_ptr->name, ps_iface_ptr->instance);
    return 0;
  }

#ifdef FEATURE_DATA_PS_DEBUG
  /*-------------------------------------------------------------------------
    Ensure that there is no phys link with the same ifname as the one passed
    as parameter. Since ifaces are never destroyed, we know we've reached the
    last interface in global_iface_ptr_array when the value is NULL.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if_ptr = global_iface_ptr_array;
  for (i = 0; (i < MAX_SYSTEM_IFACES) && (NULL != *if_ptr); i++, if_ptr++)
  {
    phys_link_ptr = PS_IFACEI_GET_PHYS_LINK(*if_ptr);
    ASSERT(PS_PHYS_LINK_IS_VALID(phys_link_ptr));

    if (ifname == phys_link_ptr->dpl_link_cb.tx_dpl_id.ifname)
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      LOG_MSG_ERROR_3("ps_iface_dpl_support_link_logging(): "
                      "Duplicate ifname (%d) passed to iface_ptr 0x%p. The other "
                      "iface_ptr is 0x%p", ifname, ps_iface_ptr, *if_ptr);
      ASSERT(0);
      return -1;
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
#endif /* FEATURE_DATA_PS_DEBUG */

  for (i = 0; i < PS_IFACE_GET_NUM_PHYS_LINKS(ps_iface_ptr); i++)
  {
    phys_link_ptr = PS_IFACEI_GET_PHYS_LINK_BY_INST(ps_iface_ptr, i);
    ASSERT(PS_PHYS_LINK_IS_VALID(phys_link_ptr));

    phys_link_ptr->dpl_link_cb.tx_dpl_id.ifname          = ifname;
    SET_DPL_IID_L_BIT_LINK(phys_link_ptr->dpl_link_cb.tx_dpl_id);
    SET_DPL_IID_DIR_TX(phys_link_ptr->dpl_link_cb.tx_dpl_id);
    phys_link_ptr->dpl_link_cb.tx_dpl_id.link_instance   =
      phys_link_ptr->phys_private.instance;

    phys_link_ptr->dpl_link_cb.recv_dpl_id.ifname          = ifname;
    SET_DPL_IID_L_BIT_LINK(phys_link_ptr->dpl_link_cb.recv_dpl_id);
    SET_DPL_IID_DIR_RX(phys_link_ptr->dpl_link_cb.recv_dpl_id);
    phys_link_ptr->dpl_link_cb.recv_dpl_id.link_instance   =
      phys_link_ptr->phys_private.instance;

    (void) ps_iface_dpl_set_phys_link_desc(phys_link_ptr, NULL);
  }

  dpli_set_ps_iface_ptr(ps_iface_ptr, ifname);
  return 0;

} /* ps_iface_dpl_support_link_logging() */

