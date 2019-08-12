/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file port_net_dev.h

Contains the Network Device Upper Layer API.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/port_net_dev.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _PORT_NET_DEV_H_
#define _PORT_NET_DEV_H_

#include <jtypes.h>


/** @ingroup hsusb_jungo_api 
@{ */

typedef void *netdev_h;
typedef void *app_ctx_h;

#define UW_MAX_ETHER_SIZE 1514
#define UW_MAC_ADDR_LEN 6

struct net_macaddr {
    juint8_t bytes[UW_MAC_ADDR_LEN];
};

typedef void (*net_complete_cb_t)(netdev_h dev, void *net_priv, 
    void *buffer);
typedef void (*net_transfer_cb_t)(app_ctx_h app_ctx, void *buffer, 
    juint32_t size, jresult_t status, void *priv);

jresult_t net_rx_complete(netdev_h dev, void *buffer, juint32_t size, 
    net_transfer_cb_t cb, void *priv);

void net_detach(netdev_h dev);

jresult_t net_enable(netdev_h dev);
void net_disable(netdev_h dev);

void net_enable_tx(netdev_h dev);
void net_disable_tx(netdev_h dev);

jresult_t net_dev_init(void);
void net_dev_uninit(void);

/** @} */ /* end_group hsusb_jungo_api */

#endif /*_PORT_NET_DEV_H_*/
