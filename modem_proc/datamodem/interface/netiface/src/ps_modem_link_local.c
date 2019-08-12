/*==========================================================================*/
/*!
  @file
  ps_modem_link_local.cpp

  @brief
  This file provides functionality of modem link local interface.

  @see
  ps_iface_defs.h

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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
  06/21/2013  ash      Support MTU upto 2000
  21/02/2012  sun      Created Module.

===========================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_PS_IPV6

#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_tx_meta_info.h"

#include "ps_in.h"
#include "ps_utils.h"

#include "ps_acl.h"
#include "ps_aclrules.h"

#include "ps_iface_logging.h"
#include "ps_logging.h"
#include "ps_logging_defs.h"

#include "ds_Utils_DebugMsg.h"
#include "ps_modem_link_local.h"
#include "ps_lo.h"

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/
/*!
  @brief
  Modem link local MTU size.

  Modem link local interface MTU size.
  PS_MODEM_LINK_LOCAL_MTU  -  MTU size
*/
#define  PS_MODEM_LINK_LOCAL_MTU  15000

/*!
  @brief
  link local prefix length.

  Defines link local prefix length.
  PS_MODEM_LINKLOCAL_PREFIX_LENGTH  -  prefix length
*/
#define  PS_MODEM_LINKLOCAL_PREFIX_LENGTH  64

/*---------------------------------------------------------------------------
  Create the v6 modem link local interface and required physical link.
---------------------------------------------------------------------------*/
static ps_iface_type      ps_modem_link_local_iface6;
static acl_type           ps_modem_link_local_v6_acl;
static ps_phys_link_type  ps_modem_link_local_v6_phys_link;

/*===========================================================================

                    ACCESS CONTROL LIST DEFINITION

===========================================================================*/
ACL_DEF( ps_modem_link_local_v6_rt_acl )
ACL_START
{
  DENY( IP_POL_FAMILY_IS( IFACE_IPV4_ADDR_FAMILY ) );
  DENY( !REQUIRED_IFACE_TYPE_IS( MODEM_LINK_LOCAL_IFACE ) );
  PASS_DEFAULT( ACL_LOOPBACK_CLASS);
}
ACL_END


/*===========================================================================

                         INTERNAL FUNCTIONS

===========================================================================*/
/**
  @brief out_use_cmd_f_ptr
    
  This function is the callback for out_of_use event for link local iface
    
  @param[in]  iface_ptr       Pointer to the link local interface
  @param[in]  user_data_ptr   User data pointer (not used)
    
  @return       0                 Success
                -1                Failure
  @dependencies
    None
*/
int ps_modem_link_local_out_of_use_cmd_hdlr
(
  ps_iface_type          * iface_ptr,
  void                   * user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-------------------------------------------------------------------------
  We are always returning TRUE in this callback to make sure that this link 
  local iface is reset when tear down is happening and all the external/
  public v6 addresses cached are freed
  -------------------------------------------------------------------------*/
  return 0;
} /* ps_modem_link_local_out_of_use_cmd_hdlr() */

/**
  @brief Transmit data.

  This method is used as transmit command for the modem link local interface.
  It discards any meta info and send the IP datagramm back to the ip stack

  @param[in]  iface_ptr       Pointer to transmitting interface
  @param[in]  pkt_chain_ptr   Reference pointer to IP datagramm
  @param[in]  meta_info_ptr   Pointer to packet's meta info
  @param[in]  tx_cmd_info     User data pointer (not used)

  @return
     None

  @dependencies
    None
*/
int ps_modem_link_local_tx_cmd
(
  ps_iface_type          *iface_ptr,
  dsm_item_type          **pkt_ref_ptr,
  ps_tx_meta_info_type   *tx_meta_info_ptr,
  void                   *tx_cmd_info
)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return lo_tx_cmd(iface_ptr,
                   pkt_ref_ptr,
                   tx_meta_info_ptr,
                   tx_cmd_info);
 
} /* ps_modem_link_local_tx_cmd() */



/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
void ps_modem_link_local_powerup_init
(
  void
)
{
  int                     result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the modem link local iface ACL
  -------------------------------------------------------------------------*/
  ps_modem_link_local_v6_acl.acl_fptr       = ps_modem_link_local_v6_rt_acl;
  ps_modem_link_local_v6_acl.post_proc_fptr = NULL;

  /*-------------------------------------------------------------------------
    Create the modem link local interface.
  -------------------------------------------------------------------------*/
  result =  ps_iface_create(&ps_modem_link_local_iface6,
                            MODEM_LINK_LOCAL_IFACE,
                            &ps_modem_link_local_v6_acl,
                            NULL,
                            &ps_modem_link_local_v6_phys_link,
                            1);
  if (-1 == result)
  {
    DATA_ERR_FATAL("ps_modem_link_local_powerup_init(): Unable to create modem link local iface");
    return;
  }

  /*-------------------------------------------------------------------------
  Set iface to ACTIVE so that iface is reset during tear down
  -------------------------------------------------------------------------*/

  PS_IFACE_SET_IS_ACTIVE( &ps_modem_link_local_iface6, TRUE);
  ps_modem_link_local_iface6.out_use_cmd_f_ptr  = 
    ps_modem_link_local_out_of_use_cmd_hdlr;

  /*-------------------------------------------------------------------------
    Set the dad function pointers to null
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_ADDR_MGMT
  ps_modem_link_local_iface6.dad_f_ptr = NULL;
  ps_modem_link_local_iface6.addr_cb_f_ptr = NULL;
#endif

  /*-------------------------------------------------------------------------
    Enable DPL
  -------------------------------------------------------------------------*/
  (void) ps_iface_dpl_set_iface_desc(&ps_modem_link_local_iface6,
                                     "MODEM_LINK_LOCAL_IFACE");

  (void) ps_iface_dpl_support_network_logging(&ps_modem_link_local_iface6,
                                              DPL_IID_IFNAME_MODEM_LINK_LOCAL_V6_IFACE);

  return;
} /* ps_modem_link_local_powerup_init() */


void ps_modem_link_local_init
(
  void
)
{
  ps_flow_type           * flow_ptr;
  int16                    ps_errno;
  int                      result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the modem link local iface addr family to ipv6
  -------------------------------------------------------------------------*/
  result = ps_iface_set_addr_family(&ps_modem_link_local_iface6,
                                    IFACE_IPV6_ADDR_FAMILY);
  if (-1 == result)
  {
    LOG_MSG_ERROR_2("ps_modem_link_local_powerup_init(): "
                    "Unable to set addr family to IPv6, iface 0x%x:%d",
                    ps_modem_link_local_iface6.name,
                    ps_modem_link_local_iface6.instance);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Set the modem link local iface tx function
  -------------------------------------------------------------------------*/
  ps_iface_set_tx_function(&ps_modem_link_local_iface6,
                           ps_modem_link_local_tx_cmd,
                           NULL);
                           
  /*-------------------------------------------------------------------------
    Assign prefix and iid to modem link local
    Send the life time as infinite to avoid ps_timer_start()
  -------------------------------------------------------------------------*/
  result = ps_iface_apply_v6_prefix(&ps_modem_link_local_iface6,
                                    0,
                                    ps_htons(0xfe80),
                                    PS_IPV6_INFINITE_LIFE_TIME,
                                    PS_IPV6_INFINITE_LIFE_TIME,
                                    PS_MODEM_LINKLOCAL_PREFIX_LENGTH);
  if (-1 == result)
  {
    LOG_MSG_ERROR_2("ps_modem_link_local_powerup_init(): "
                    "Failed to apply v6 prefix, iface 0x%x:%d",
                    ps_modem_link_local_iface6.name,
                    ps_modem_link_local_iface6.instance);
    ASSERT(0);
    return;
  }

  result = ps_iface_generate_ipv6_iid(&ps_modem_link_local_iface6,
                                      &(ps_modem_link_local_iface6.iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->iid),
                                      &ps_errno);
  if (-1 == result)
  {
    LOG_MSG_ERROR_2("ps_modem_link_local_powerup_init(): "
                    "Failed to generate random iid, iface 0x%x:%d",
                    ps_modem_link_local_iface6.name,
                    ps_modem_link_local_iface6.instance);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Set MTU
  -------------------------------------------------------------------------*/
  PS_IFACE_SET_MTU(&ps_modem_link_local_iface6, PS_MODEM_LINK_LOCAL_MTU);

  flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(&ps_modem_link_local_iface6);
  ps_flow_bind_phys_link(flow_ptr, &ps_modem_link_local_v6_phys_link);
  ps_flow_activate_ind(flow_ptr, PS_EIC_NOT_SPECIFIED);
  ps_phys_link_up_ind(&ps_modem_link_local_v6_phys_link);
  ps_iface_up_ind(&ps_modem_link_local_iface6);

  return;
} /* ps_modem_link_local_init() */

#endif
