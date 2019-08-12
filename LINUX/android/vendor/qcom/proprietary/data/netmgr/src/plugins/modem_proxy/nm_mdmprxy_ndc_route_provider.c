/******************************************************************************

      N M _ M D M P R X Y _ N D C _ R O U T E _ P R O V I D E R . C

******************************************************************************/

/******************************************************************************

  @file    nm_mdmproxy_ndc_route_provider.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Modem proxy module is responsible for managing routing rules and packet
  marking rules for forwarding packets to modem.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <sys/socket.h>
#include <linux/if.h>
#include <string.h>

#include "netmgr.h"
#include "netmgr_util.h"
#include "netmgr_kif.h"
#include "netmgr_kif_cb.h"

#include "nm_mdmprxy.h"
#include "nm_mdmprxy_route_provider_factory.h"
#include "nm_mdmprxy_iptables_helper.h"
#include "nm_mdmprxy_common.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define NM_MDMPRXY_CUSTOM_OEM_NETWORK  "oem9"

static boolean oem_network_created = FALSE;

static int iiface_refcount;
static int iiface_epdg_refcount;
static int iiface_handle;

static boolean iptables_inited = FALSE;

typedef struct nm_mdmprxy_global_cfg
{
  /* Currently ndc interface does not offer a mechanism to install routing rules per
     IP family however the callbacks from netmgr are per IP family ex. for dual-IP calls
     we will receive one callback for V4 and one for V6. We need to install the routing
     rules only once but there is a possibility of one of the IP families going down
     in the middle of a call. We need to ensure that the rules are removed only after
     both address families of the link are down. For this purpose we need to maintain
     two flags per link, one for V4 and one for V6 */
  boolean is_v4_rule_installed;
  boolean is_v6_rule_installed;
} nm_mdmprxy_cfg_t;

static boolean nm_mdmprxy_tunnel_doze_installed = FALSE;

#define NM_MDMPRXY_GET_TUNNEL_DOZE_RULES_INSTALLED()                         \
  nm_mdmprxy_tunnel_doze_installed

#define NM_MDMPRXY_SET_TUNNEL_DOZE_RULES_INSTALLED(status)                   \
  nm_mdmprxy_tunnel_doze_installed = status

static nm_mdmprxy_cfg_t nm_mdmprxy_global_link_status[NM_MDMPRXY_MAXDEVSTR];

#define NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V4(link)                      \
  nm_mdmprxy_global_link_status[link].is_v4_rule_installed

#define NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V6(link)                      \
  nm_mdmprxy_global_link_status[link].is_v6_rule_installed

#define NM_MDMPRXY_SET_ROUTING_RULES_INSTALLED(link, af, status)             \
do                                                                           \
{                                                                            \
  if (AF_INET == af)                                                         \
  {                                                                          \
    *(&nm_mdmprxy_global_link_status[link].is_v4_rule_installed) = status;   \
  }                                                                          \
  else if (AF_INET6 == af)                                                   \
  {                                                                          \
    *(&nm_mdmprxy_global_link_status[link].is_v6_rule_installed) = status;   \
  }                                                                          \
} while(0)

/* Global OOS and IS handlers */
static void
nm_mdmprxy_ndc_prov_oos_hndlr(void);

static void
nm_mdmprxy_ndc_prov_is_hndlr(void);

static void
nm_mdmprxy_ndc_prov_init(void);

static int
nm_mdmprxy_ndc_prov_link_oos_hndlr(void*);

static int
nm_mdmprxy_ndc_prov_link_is_hndlr(void*);

static int
nm_mdmprxy_ndc_prov_netd_is_hdlr(void);

static int
nm_mdmprxy_ndc_prov_netd_link_is_hdlr(void*);

static int
nm_mdmprxy_ndc_prov_create_network(void);

static int
nm_mdmprxy_ndc_prov_add_route
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
);

static int
nm_mdmprxy_ndc_prov_remove_route
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
);

static int
nm_mdmprxy_ndc_prov_install_iiface_rules
(
  int                  link,
  char*                link_name,
  int                  tech_type,
  int                  valid4,
  uint32_t             daddr4,
  int                  valid6,
  struct in6_addr      *daddr6,
  uint32_t             mask6,
  ipsec_tunnel_info_t  *tunnel
);

static int
nm_mdmprxy_ndc_prov_remove_iiface_rules
(
  int                            link,
  char                           *link_name,
  int                            tech_type,
  int                            ip_family,
  ipsec_tunnel_info_t            *tunnel,
  struct netmgr_kif_addrstorage  *link_addr
);

nm_mdmprxy_route_prov_t nm_mdmprxy_ndc_route_provider =
{
  /* Initialize the function table for
   * the ndc route provider */
  .add_route               = nm_mdmprxy_ndc_prov_add_route,
  .remove_route            = nm_mdmprxy_ndc_prov_remove_route,
  .install_rule            = nm_mdmprxy_ndc_prov_install_iiface_rules,
  .remove_rule             = nm_mdmprxy_ndc_prov_remove_iiface_rules,
  .init                    = nm_mdmprxy_ndc_prov_init,
  .is_hdlr                 = nm_mdmprxy_ndc_prov_is_hndlr,
  .oos_hdlr                = nm_mdmprxy_ndc_prov_oos_hndlr,
  .is_hdlr_per_link        = nm_mdmprxy_ndc_prov_link_is_hndlr,
  .oos_hdlr_per_link       = nm_mdmprxy_ndc_prov_link_oos_hndlr,
  .netd_is_hdlr            = nm_mdmprxy_ndc_prov_netd_is_hdlr,
  .netd_is_hdlr_per_link   = nm_mdmprxy_ndc_prov_netd_link_is_hdlr
};

static const struct nm_mdmprxy_cmd static_rule_list[] =
{
  { "%s %s -t mangle -A nm_mdmprxy_pkt_marker -m mark --mark %u"
                          " -j nm_mdmprxy_pkt_forwarder",
    "%s %s -t mangle -D nm_mdmprxy_pkt_marker -m mark --mark %u"
                          " -j nm_mdmprxy_pkt_forwarder",
    nm_mdmprxy_markjump_formatter },

  { "%s %s -t mangle -A nm_mdmprxy_pkt_forwarder -j DROP",
    "%s %s -t mangle -D nm_mdmprxy_pkt_forwarder -j DROP",
    nm_mdmprxy_basic_formatter }
};

static const struct nm_mdmprxy_cmd per_iiface_rule_list[] =
{
  { "%s %s -t mangle -I PREROUTING -i %s -j nm_mdmprxy_pkt_marker",
    "%s %s -t mangle -D PREROUTING -i %s -j nm_mdmprxy_pkt_marker",
    nm_mdmprxy_ifacejump_formatter },

  /* nm_mdmprxy_icmp_pkt_marker chain hosts marking rules for icmp ping packets
     originating from the modem. The mark provider will install the marking rules. The
     route provider just needs to install the jump rule in PREROUTING chain to the
     nm_mdmprxy_icmp_pkt_marker chain */
  { "%s %s -t mangle -A PREROUTING -j nm_mdmprxy_icmp_pkt_marker",
    "%s %s -t mangle -D PREROUTING -j nm_mdmprxy_icmp_pkt_marker",
    nm_mdmprxy_basic_formatter }
};

static const struct nm_mdmprxy_cmd per_oiface_rule_list[] =
{
  { "%s %s -t mangle -I nm_mdmprxy_pkt_forwarder -d %s"
                          " -j TEE --gateway %s --oif %s",
    "%s %s -t mangle -D nm_mdmprxy_pkt_forwarder -d %s"
                          " -j TEE --gateway %s --oif %s",
    nm_mdmprxy_tee_formatter }
};

static const struct nm_mdmprxy_cmd per_iface_forward_rule_list[] =
{
  { "%s %s -t filter -I nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    nm_mdmprxy_ifaceforward_incoming_formatter },

  { "%s %s -t filter -I nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_iface_pkt_fwder -i %s -o %s -j ACCEPT",
    nm_mdmprxy_ifaceforward_outgoing_formatter },
};

/* It is possible to have wlan0 address on the UE and DNS address on the epdg server to share
     the same subnet. In such situations packets originating on from the modem will be routed
     up the AP stack since the subnets are same. To prevent such situations we will TEE the
     incoming packet from the modem arriving from local-breakout interface and force the packet
     over wlan0 using a dummy gateway */
static const struct nm_mdmprxy_cmd local_breakout_rule_list[] =
{
  { "%s %s -t mangle -A PREROUTING -p tcp -i %s -j nm_mdmprxy_mngl_pre_tee",
    "%s %s -t mangle -D PREROUTING -p tcp -i %s -j nm_mdmprxy_mngl_pre_tee",
    nm_mdmprxy_lbo_ifacejump_formatter },

  { "%s %s -t mangle -A PREROUTING -p udp -i %s -j nm_mdmprxy_mngl_pre_tee",
    "%s %s -t mangle -D PREROUTING -p udp -i %s -j nm_mdmprxy_mngl_pre_tee",
    nm_mdmprxy_lbo_ifacejump_formatter },

  { "%s %s -t mangle -I nm_mdmprxy_mngl_pre_tee -j MARK --set-mark " strstr(NM_MDMPRXY_LBO_MARK),
    "%s %s -t mangle -D nm_mdmprxy_mngl_pre_tee -j MARK --set-mark " strstr(NM_MDMPRXY_LBO_MARK),
    nm_mdmprxy_basic_formatter },

  { "%s %s -t mangle -A nm_mdmprxy_mngl_pre_tee -j TEE --gateway %s --oif %s",
    "%s %s -t mangle -D nm_mdmprxy_mngl_pre_tee -j TEE --gateway %s --oif %s",
    nm_mdmprxy_lbo_formatter },

  { "%s %s -t mangle -A nm_mdmprxy_mngl_pre_tee -j DROP",
    "%s %s -t mangle -D nm_mdmprxy_mngl_pre_tee -j DROP",
    nm_mdmprxy_basic_formatter }
};

static const struct nm_mdmprxy_cmd local_breakout_doze_rule_list[] =
{
  { "%s %s -t filter -I nm_mdmprxy_doze_mode_skip -m mark --mark %u -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_doze_mode_skip -m mark --mark %u -j ACCEPT",
    nm_mdmprxy_doze_lbo_formatter }
};

static const struct nm_mdmprxy_cmd per_iface_doze_rule_list[] =
{
  { "%s %s -t filter -A nm_mdmprxy_doze_mode_skip -o %s -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_doze_mode_skip -o %s -j ACCEPT",
    nm_mdmprxy_doze_oif_formatter }
};

static const struct nm_mdmprxy_cmd tunnel_doze_rule_list[] =
{
  { "%s %s -t filter -A nm_mdmprxy_doze_mode_skip --src %s --dst %s -j ACCEPT",
    "%s %s -t filter -D nm_mdmprxy_doze_mode_skip --src %s --dst %s -j ACCEPT",
    nm_mdmprxy_doze_tunnel_formatter }
};

static const struct nm_mdmprxy_cmd vpn_skip_rule_list[] =
{
  { "%s add from %s to %s iif lo fwmark 0x0/0x20000 goto 13000 prio 11999",
    "%s del from %s to %s iif lo fwmark 0x0/0x20000 goto 13000 prio 11999",
    nm_mdmprxy_vpn_skip_formatter }
};

struct route_prov_iptables ndc_prov_iptables_cmd_tbl;

/*---------------------------------------------------------------------------
                            IPTABLES FUNCTIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_install_iiface_rules
===========================================================================*/
/*!
@brief
  Sets up rules to forward packets to the modem by using TEE target
  to clone packets

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_install_iiface_rules
(
  int                  link,
  char*                link_name,
  int                  tech_type,
  int                  valid4,
  uint32_t             daddr4,
  int                  valid6,
  struct in6_addr      *daddr6,
  uint32_t             mask6,
  ipsec_tunnel_info_t  *tunnel
)
{
  int rc;
  int ip_family;
  struct in6_addr gwaddr6;
  inet_pton(AF_INET6, NM_MDMPRXY_GATEWAY_V6_ADDR, &gwaddr6);

  /* If this is the first ePDG call being brought up install doze mode and VPN skip rules*/
  if (iiface_epdg_refcount == 0 &&
      NETMGR_KIF_CB_IF_TECH_TYPE_EPDG == tech_type)
  {
    if (NETMGR_SUCCESS != nm_mdmprxy_install_tunnel_doze_rules(link_name,
                                                               tech_type,
                                                               tunnel))
    {
      netmgr_log_err("%s(): Failed to install tunnel doze rule for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IP[%d] tunnel doze rule for link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);

      NM_MDMPRXY_SET_TUNNEL_DOZE_RULES_INSTALLED(TRUE);
    }

    if (NETMGR_SUCCESS != nm_mdmprxy_install_vpn_skip_rule(link_name,
                                                           tech_type,
                                                           tunnel))
    {
      netmgr_log_err("%s(): Failed to install vpn skip rule for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IP[%d] vpn skip rule for link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);
    }

    if (NETMGR_SUCCESS != nm_mdmprxy_common_install_dl_ping6_fwmark_rules(link_name,
                                                                          tech_type,
                                                                          tunnel))
    {
      netmgr_log_err("%s(): Failed to install DL ping6 fwmark rules for tunnel=%d, link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed DL ping6 fwmark rules for tunnel=%d, link=%s!",
                     __func__,
                     tunnel->tunnel_family,
                     link_name);
    }
  }

  iiface_refcount++;

  /* Install TEE rule for local breakout */
  if (NETMGR_KIF_CB_IF_TECH_TYPE_LBO == tech_type)
  {
    if (valid4)
    {
      ip_family = AF_INET;
    }
    else if (valid6)
    {
      ip_family = AF_INET6;
    }
    else
    {
      netmgr_log_err("%s(): bad ip_family!", __func__);
      return NETMGR_FAILURE;
    }

    if (NETMGR_SUCCESS
          != nm_mdmprxy_install_local_breakout_rules(link_name,
                                                     NM_MDMPRXY_IIFACE,
                                                     NM_MDMPRXY_GATEWAY_V4_ADDR,
                                                     &gwaddr6,
                                                     ip_family))
    {
      netmgr_log_err("%s(): Failed to install TEE rule for local-breakout iface!", __func__);
    }
    if (NETMGR_SUCCESS
          != nm_mdmprxy_install_local_breakout_doze_rules(ip_family))
    {
      netmgr_log_err("%s(): Failed to install TEE rule for local-breakout iface!", __func__);
    }
  }
  else if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG == tech_type)
  {
    iiface_epdg_refcount++;
  }

  /* Install the destination based rules, if any */
  if (valid4)
  {
    nm_mdmprxy_install_oiface_rules(iiface_handle,
                                    link,
                                    (const char*) link_name,
                                    TRUE,
                                    NM_MDMPRXY_GATEWAY_V4_ADDR,
                                    daddr4,
                                    FALSE,
                                    NULL,
                                    NULL,
                                    0);

    netmgr_log_med("%s(): Installed IPv4 oif rules for link [%s]",
                   __func__, link_name);

    if (NETMGR_SUCCESS != nm_mdmprxy_install_per_iface_doze_rules(link_name,
                                                                  AF_INET))
    {
      netmgr_log_err("%s(): Failed to install IPv4 iface doze rule for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IPv4 iface doze rule for link=%s!",
                     __func__,
                     link_name);
    }
  }
  else if (valid6)
  {
    nm_mdmprxy_install_oiface_rules(iiface_handle,
                                    link,
                                    (const char*) link_name,
                                    FALSE,
                                    NM_MDMPRXY_V4ADDR_INVALID,
                                    NM_MDMPRXY_V4ADDR_INVALID,
                                    TRUE,
                                    &gwaddr6,
                                    daddr6,
                                    mask6);

    netmgr_log_med("%s(): Installed IPv6 oif rules for link [%s]",
                   __func__, link_name);

    if (NETMGR_SUCCESS != nm_mdmprxy_install_per_iface_doze_rules(link_name,
                                                                  AF_INET6))
    {
      netmgr_log_err("%s(): Failed to install IPv6 iface doze rule for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IPv6 iface doze rule for link=%s!",
                     __func__,
                     link_name);
    }

    if (NETMGR_SUCCESS != nm_mdmprxy_common_install_per_iface_dl_ping6_rules(link_name,
                                                                             tech_type,
                                                                             daddr6,
                                                                             mask6))
    {
      netmgr_log_err("%s(): Failed to install IPv6 DL ping6 rules for link=%s!",
                     __func__,
                     link_name);
    }
    else
    {
      netmgr_log_med("%s(): Installed IPv6 DL ping6 rules for link=%s!",
                     __func__,
                     link_name);
    }
  }
  else
  {
    netmgr_log_err("%s(): Unhandled AF; skipping config", __func__);
  }

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_remove_iiface_rules
===========================================================================*/
/*!
@brief
  Cleans up iptables rules for the given link

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_remove_iiface_rules
(
  int                            link,
  char                           *link_name,
  int                            tech_type,
  int                            ip_family,
  ipsec_tunnel_info_t            *tunnel,
  struct netmgr_kif_addrstorage  *link_addr
)
{
  struct in6_addr gwaddr6;
  inet_pton(AF_INET6, NM_MDMPRXY_GATEWAY_V6_ADDR, &gwaddr6);

  /* Remove destination based rules, if any */
  nm_mdmprxy_uninstall_oiface_rules(iiface_handle, link, ip_family);

  /* Remove TEE rule for local breakout */
  if (NETMGR_KIF_CB_IF_TECH_TYPE_LBO == tech_type)
  {
    if (NETMGR_SUCCESS
          != nm_mdmprxy_remove_local_breakout_rules(link_name,
                                                    NM_MDMPRXY_IIFACE,
                                                    NM_MDMPRXY_GATEWAY_V4_ADDR,
                                                    &gwaddr6,
                                                    ip_family,
                                                    (int) nm_mdmprxy_rules_len(local_breakout_rule_list) - 1))
    {
      netmgr_log_err("%s(): Failed to install TEE rule for local-breakout iface!", __func__);
    }
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_remove_per_iface_doze_rules(link_name,
                                                               ip_family))
  {
    netmgr_log_err("%s(): Failed to remove IP[%d] iface doze rule for link=%s!",
                   __func__,
                   ip_family,
                   link_name);
  }
  else
  {
    netmgr_log_med("%s(): Removed IP[%d] iface doze rule for link=%s!",
                   __func__,
                   ip_family,
                   link_name);
  }

  /* TODO: The original design was assuming that the refcount being = 1 means that the iface
     being cleaned up was an iiface. However, there is no gurantee that the last iface being
     disconnected is the iiface. If there are other oifaces still up and we get a disconnect
     on the iiface earlier to getting disconnect on oiface then we  are not cleaning up  the
     associated oiface rules */
  if (iiface_refcount == 1)
  {
    /* If this is the last rev rmnet iface then remove the jump rule from PREROUTING */
    nm_mdmprxy_uninstall_iiface_rules(iiface_handle);
  }

  /* If this is the last ePDG call going down, remove tunnel doze rules */
  if (NETMGR_KIF_CB_IF_TECH_TYPE_EPDG == tech_type)
  {
    if (iiface_epdg_refcount == 1)
    {
      if (NETMGR_SUCCESS != nm_mdmprxy_remove_tunnel_doze_rules(link_name,
                                                                tech_type,
                                                                tunnel))
      {
        netmgr_log_err("%s(): Failed to remove tunnel[%d] iface doze rule for link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed tunnel[%d] iface doze rule for link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);

        NM_MDMPRXY_SET_TUNNEL_DOZE_RULES_INSTALLED(FALSE);
      }

      if (NETMGR_SUCCESS != nm_mdmprxy_remove_vpn_skip_rule(link_name,
                                                            tech_type,
                                                            tunnel))
      {
        netmgr_log_err("%s(): Failed to remove tunnel[%d] vpn skip rule for link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed tunnel[%d] vpn skip rule for link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }

      if (NETMGR_SUCCESS != nm_mdmprxy_common_remove_dl_ping6_fwmark_rules(link_name,
                                                                           tech_type,
                                                                           tunnel))
      {
        netmgr_log_err("%s(): Failed to remove DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
    }

    if (AF_INET6 == ip_family)
    {
      if (NETMGR_SUCCESS != nm_mdmprxy_common_remove_per_iface_dl_ping6_rules(link_name,
                                                                              tech_type,
                                                                              &link_addr->_in6.addr,
                                                                              link_addr->_in6.mask))
      {
        netmgr_log_err("%s(): Failed to remove DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
      else
      {
        netmgr_log_med("%s(): Removed DL ping6 rules for tunnel=%d, link=%s!",
                       __func__,
                       tunnel->tunnel_family,
                       link_name);
      }
    }

    iiface_epdg_refcount--;
  }

  iiface_refcount--;

  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_iptables_init
===========================================================================*/
/*!
@brief
  Installs static iptables rules
*/
/*=========================================================================*/
static void
nm_mdmprxy_ndc_prov_iptables_init()
{
  nm_mdmprxy_iptables_init();

  iptables_inited = TRUE;

  netmgr_log_low("%s(): Initialization finished\n", __func__);
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_iptables_deinit
===========================================================================*/
/*!
@brief
  Cleans up static iptables rules
*/
/*=========================================================================*/
static void
nm_mdmprxy_ndc_prov_iptables_deinit()
{
  /* This removes both static and iiface + oiface rules. This mechanism will be
     used till we fix the SSR handling within core */
  nm_mdmprxy_iptables_uninit();

  iptables_inited = FALSE;

  netmgr_log_low("%s(): De-initialization finished\n", __func__);
}

/*---------------------------------------------------------------------------
                            ROUTING FUNCTIONS
---------------------------------------------------------------------------*/

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_exec_cmd
===========================================================================*/
/*!
@brief
  Helper function to execute ndc commands

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_exec_cmd(char* cmd)
{
  int rc = NETMGR_FAILURE;
  char err_str[NETMGR_MAX_COMMAND_LENGTH] = "";
  char *saveptr = NULL;
  char *ret_str = NULL;

  NETMGR_LOG_FUNC_ENTRY;

  /* ndc commands log everything to stdout even when there is an error
     We need to pass in a valid stdout buffer for the return message
     to be captured so that we can parse the output */
  DS_SYSCALL_DEFAULTS(opts);
  opts.stdout = err_str;
  opts.stdout_length = sizeof(err_str);
  opts.log_options |= DS_EXEC_LOG_EXECTIME;

  if (!cmd)
  {
    netmgr_log_err("%s(): Invalid cmd!\n", __func__);
    goto bail;
  }

  rc = netmgr_ds_exec(cmd, &opts);

  if (DS_EXEC_OK != rc)
  {
    netmgr_log_err("%s(): cmd: %s failed!", __func__, cmd);
    rc = NETMGR_FAILURE;
    goto bail;
  }

  if (strlen(err_str) != 0)
  {
    /* ndc commands print the string 200 0 success on successful
     * execution of the command. We need so search
     * for this keyword to check if the command succeeded */
    ret_str = strtok_r(err_str, " ", &saveptr);

    while (ret_str)
    {
      if (strcmp(ret_str, "200")
            && NULL == strcasestr(ret_str, "200"))
      {
        /* Continue searching till the end of the error string is reached */
        ret_str = strtok_r(NULL, " ", &saveptr);
      }
      else
      {
        rc = NETMGR_SUCCESS;
        break;
      }
    }
  }

  if (NETMGR_SUCCESS != rc)
  {
    netmgr_log_err("%s(): cmd: %s failed! Err: %s", __func__, cmd, err_str);
  }

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_create_network
===========================================================================*/
/*!
@brief
  Creates network to manage custom routing rules

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_create_network(void)
{
  char ndc_create_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";
  int rc = NETMGR_FAILURE;

  NETMGR_LOG_FUNC_ENTRY;

  if (!oem_network_created)
  {
    snprintf(ndc_create_cmd,
             sizeof(ndc_create_cmd),
             "ndc network create %s",
             NM_MDMPRXY_CUSTOM_OEM_NETWORK);

    if (NETMGR_SUCCESS != nm_mdmprxy_ndc_prov_exec_cmd(ndc_create_cmd))
    {
      netmgr_log_err("%s(): Failed to create custom network!\n",
                     __func__);
      goto bail;
    }

    netmgr_log_med("%s(): Successfully created new custom network!\n",
                    __func__);
    oem_network_created = TRUE;
  }

  rc = NETMGR_SUCCESS;

bail:
  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_add_route
===========================================================================*/
/*!
@brief
  Setup routing rules for the provided link

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_add_route
(
  int                            link,
  char*                          link_name,
  int                            tech_type,
  sa_family_t                    af,
  ipsec_tunnel_info_t            *tunnel_info,
  struct netmgr_kif_addrstorage  *link_addr
)
{
  int rc = NETMGR_FAILURE;
  char cmd[NETMGR_MAX_COMMAND_LENGTH] = "";

  NETMGR_LOG_FUNC_ENTRY;

  NM_MDMPRXY_VAR_UNUSED(tech_type);
  NM_MDMPRXY_VAR_UNUSED(tunnel_info);
  NM_MDMPRXY_VAR_UNUSED(link_addr);

  if (!link_name)
  {
    netmgr_log_err("%s(): link_name cannot be NULL!", __func__);
    goto bail;
  }

  /* Currently ndc interface does not provide an option to enable routing rules per
     ip family. We will be installing rules for both V4 and V6 at the same time.
     To handle a dual-ip case where only one of the address families goes up and down
     we need to ensure routing rules installation and deletion is handled correctly */
  if ((AF_INET == af && TRUE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V6(link))
        || (AF_INET6 == af && TRUE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V4(link)))
  {
    /* Routing rules need to be installed only once */
    netmgr_log_med("%s(): Routing rules already installed for link [%s]", __func__, link_name);
    rc = NETMGR_SUCCESS;
    goto bail;
  }

  /* If netd had started after netmgr in the bootup sequence then the initial
     attempt to create the oem10 network might have failed. Attempt to create
     the network again here */
  if (NETMGR_SUCCESS != nm_mdmprxy_ndc_prov_create_network())
  {
    netmgr_log_err("%s(): Failed to create custom network!", __func__);
    goto bail;
  }

  /* Add link to custom network */
  snprintf(cmd,
           sizeof(cmd),
           "ndc network interface add %s %s",
           NM_MDMPRXY_CUSTOM_OEM_NETWORK,
           link_name);

  if (NETMGR_SUCCESS !=
        nm_mdmprxy_ndc_prov_exec_cmd(cmd))
  {
    netmgr_log_err("%s(): Failed to add interface [%s] to network!",
                   __func__, link_name);
    goto bail;
  }

  /* Add routing rule to send packets from the provided link over wlan0 */
  snprintf(cmd,
           sizeof(cmd),
           "ndc ipfwd add %s wlan0",
           link_name);

  if (NETMGR_SUCCESS !=
        nm_mdmprxy_ndc_prov_exec_cmd(cmd))
  {
    netmgr_log_err("%s(): Failed to add forwarding rule from link [%s] to wlan0!",
                   __func__, link_name);
    goto bail;
  }

  /* Add a default V4 route link's table via dummy gateway. The cloned packets
   * will be forwarded to modem over this route */
  snprintf(cmd,
           sizeof(cmd),
           "ndc network route add %s %s %s/32",
           NM_MDMPRXY_CUSTOM_OEM_NETWORK,
           link_name,
           NM_MDMPRXY_GATEWAY_V4_ADDR_STR);

  if (NETMGR_SUCCESS !=
        nm_mdmprxy_ndc_prov_exec_cmd(cmd))
  {
    netmgr_log_err("%s(): Failed to add default V4 gateway route for link [%s]!",
                   __func__, link_name);
    goto bail;
  }

  /* Add routing rule to send packets from the provided link over dummy0 */
  snprintf(cmd,
           sizeof(cmd),
           "ndc ipfwd add %s dummy0",
           link_name);

  if (NETMGR_SUCCESS !=
        nm_mdmprxy_ndc_prov_exec_cmd(cmd))
  {
    netmgr_log_err("%s(): Failed to add forwarding rule from link [%s] to dummy0!",
                   __func__, link_name);
    goto bail;
  }

  /* Add a default V6 route link's table via dummy gateway. The cloned packets
   * will be forwarded to modem over this route */
  snprintf(cmd,
           sizeof(cmd),
           "ndc network route add %s %s %s/128",
           NM_MDMPRXY_CUSTOM_OEM_NETWORK,
           link_name,
           NM_MDMPRXY_GATEWAY_V6_ADDR);

  if (NETMGR_SUCCESS !=
        nm_mdmprxy_ndc_prov_exec_cmd(cmd))
  {
    netmgr_log_err("%s(): Failed to add default V6 gateway route for link [%s]!",
                   __func__, link_name);
    goto bail;
  }

  rc = NETMGR_SUCCESS;

bail:
  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_ROUTING_RULES_INSTALLED(link, af, TRUE);
    netmgr_log_low("%s(): Routing rules intalled for link [%s], family [%d]",
                   __func__, link_name, af);
  }

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_remove_route
===========================================================================*/
/*!
@brief
  Remove routing rules for the provided link

@return
  NETMGR_SUCCESS/NETMGR_FAILURE
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_remove_route
(
  int                           link,
  char*                         link_name,
  int                           tech_type,
  sa_family_t                   af,
  ipsec_tunnel_info_t           *tunnel_info,
  struct netmgr_kif_addrstorage *link_addr
)
{
  int rc = NETMGR_FAILURE;
  char ndc_remove_cmd[NETMGR_MAX_COMMAND_LENGTH] = "";

  NM_MDMPRXY_VAR_UNUSED(tech_type);
  NM_MDMPRXY_VAR_UNUSED(tunnel_info);
  NM_MDMPRXY_VAR_UNUSED(link_addr);

  NETMGR_LOG_FUNC_ENTRY;

  if (!link_name)
  {
    netmgr_log_err("%s(): link_name cannot be NULL!", __func__);
    goto bail;
  }

  if ( AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): invalid ip family [%d]", __func__, af);
    rc = NETMGR_FAILURE;
    goto bail;
  }

  if ((AF_INET == af && TRUE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V6(link))
        || (AF_INET6 == af && TRUE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V4(link)))
  {
    /* If one of the IP families is still up, we need to skip rule deletion*/
    netmgr_log_med("%s(): Cannot remove routing rules for link [%s] as another IP family"
                   " is still UP on it!", __func__, link_name);
    rc = NETMGR_SUCCESS;
    goto bail;
  }

  if (FALSE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V4(link)
      && FALSE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V6(link))
  {
    netmgr_log_med("%s(): Routing rules already removed for link [%s]", __func__, link_name);
    rc = NETMGR_SUCCESS;
    goto bail;
  }

  snprintf(ndc_remove_cmd,
           sizeof(ndc_remove_cmd),
           "ndc network interface remove %s %s",
           NM_MDMPRXY_CUSTOM_OEM_NETWORK,
           link_name);

  if (NETMGR_SUCCESS != nm_mdmprxy_ndc_prov_exec_cmd(ndc_remove_cmd))
  {
    netmgr_log_err("%s(): failed to remove link [%s] from custom network",
                   __func__, link_name);
    goto bail;
  }

  rc = NETMGR_SUCCESS;

bail:
  if (NETMGR_SUCCESS == rc)
  {
    NM_MDMPRXY_SET_ROUTING_RULES_INSTALLED(link, af, FALSE);
    netmgr_log_med( "%s(): successfully removed link [%s], ip_family [%d]"
                    " from custom network\n",
                    __func__, link_name, af);
  }

  NETMGR_LOG_FUNC_EXIT;
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_netd_is_hdlr
===========================================================================*/
/*!
@brief
  Global IS handler for netd
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_netd_is_hdlr(void)
{
  int rc = NETMGR_SUCCESS;
  /* Reset global flag */
  oem_network_created = FALSE;

  rc = nm_mdmprxy_ndc_prov_create_network();

  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_netd_link_is_hdlr
===========================================================================*/
/*!
@brief
  Per link IS handler for netd restart
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_netd_link_is_hdlr(void* arg)
{
  struct netmgr_kif_cb_ifinfo *ifinfo = (struct netmgr_kif_cb_ifinfo *) arg;
  int rc = NETMGR_FAILURE;

  if (!ifinfo)
  {
    netmgr_log_err("%s(): Invalid params", __func__);
    goto bail;
  }

  if (ifinfo->flags & IFF_UP)
  {
    /* Reinstall routing rules based on ip family */
    if (TRUE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V4(ifinfo->link))
    {
      NM_MDMPRXY_SET_ROUTING_RULES_INSTALLED(ifinfo->link, AF_INET, FALSE);
      rc = nm_mdmprxy_ndc_prov_add_route(ifinfo->link,
                                         ifinfo->name,
                                         ifinfo->tech_type,
                                         AF_INET,
                                         NULL,
                                         NULL);
    }

    if (TRUE == NM_MDMPRXY_GET_ROUTING_RULES_INSTALLED_V6(ifinfo->link))
    {
      NM_MDMPRXY_SET_ROUTING_RULES_INSTALLED(ifinfo->link, AF_INET6, FALSE);
      rc = nm_mdmprxy_ndc_prov_add_route(ifinfo->link,
                                         ifinfo->name,
                                         ifinfo->tech_type,
                                         AF_INET6,
                                         NULL,
                                         NULL);
    }
  }

  netmgr_log_low("%s(): complete!", __func__);

bail:
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_link_oos_hndlr
===========================================================================*/
/*!
@brief
  Per link OOS handler
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_link_oos_hndlr(void* arg)
{
  struct in6_addr gwaddr6;
  netmgr_kif_cb_link_ssr_info_t *ifinfo =
         (netmgr_kif_cb_link_ssr_info_t *) arg;
  int rc = NETMGR_SUCCESS;

  if (!ifinfo)
  {
    netmgr_log_err("%s(): Invalid params...skipping deinit!", __func__);
    goto bail;
  }

  inet_pton(AF_INET6, NM_MDMPRXY_GATEWAY_V6_ADDR, &gwaddr6);

  if (NETMGR_KIF_CB_IF_TECH_TYPE_LBO == ifinfo->tech_type)
  {
    if (NETMGR_SUCCESS
          != nm_mdmprxy_remove_local_breakout_rules(ifinfo->name,
                                                    NM_MDMPRXY_IIFACE,
                                                    NM_MDMPRXY_GATEWAY_V4_ADDR,
                                                    &gwaddr6,
                                                    AF_INET,
                                                    (int) nm_mdmprxy_rules_len(local_breakout_rule_list) - 1))
    {
      netmgr_log_err("%s(): Failed to remove V4 TEE rule for local-breakout iface!",
                      __func__);
    }

    if (NETMGR_SUCCESS
          != nm_mdmprxy_remove_local_breakout_rules(ifinfo->name,
                                                    NM_MDMPRXY_IIFACE,
                                                    NM_MDMPRXY_GATEWAY_V4_ADDR,
                                                    &gwaddr6,
                                                    AF_INET6,
                                                    (int) nm_mdmprxy_rules_len(local_breakout_rule_list) - 1))
    {
      netmgr_log_err("%s(): Failed to remove V6 TEE rule for local-breakout iface!",
                      __func__);
    }
  }

  /* In modem SSR scenario where the kernel interface would have gone down
     when modem goes down, we need to ensure that the corresponding route is cleaned
     up as well */
  if ( NETMGR_KIF_IPF_MASK_IPV4 & ifinfo->addr_mask )
  {
    rc = nm_mdmprxy_ndc_prov_remove_route(ifinfo->link,
                                          ifinfo->name,
                                          ifinfo->tech_type,
                                          AF_INET,
                                          NULL,
                                          NULL);
    if ( NETMGR_FAILURE == rc )
    {
      netmgr_log_err("%s(): failed remove ndc route for AF_INET",__func__);
    }
  }

  if ( NETMGR_KIF_IPF_MASK_IPV6 & ifinfo->addr_mask )
  {
    rc = nm_mdmprxy_ndc_prov_remove_route(ifinfo->link,
                                          ifinfo->name,
                                          ifinfo->tech_type,
                                          AF_INET6,
                                          NULL,
                                          NULL);
    if ( NETMGR_FAILURE == rc)
    {
      netmgr_log_err("%s(): failed remove ndc route for AF_INET6",__func__);
    }
  }

bail:
  netmgr_log_low("%s(): complete", __func__);
  return rc;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_link_is_hndlr
===========================================================================*/
/*!
@brief
  Per link IS handler
*/
/*=========================================================================*/
static int
nm_mdmprxy_ndc_prov_link_is_hndlr(void* arg)
{
  /* Currently we don't have anything to handle in per link modem IS handler
     Eventually we want to move iiface + oiface rule + any other link specific
     rules to this handler */
  NM_MDMPRXY_VAR_UNUSED(arg);
  return NETMGR_SUCCESS;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_flush_doze_mode_rules
===========================================================================*/
/*!
@brief
  Global OOS doze rule clean-up
*/
/*=========================================================================*/
static void
nm_mdmprxy_ndc_prov_flush_doze_mode_rules(sa_family_t af)
{
  if (AF_INET != af && AF_INET6 != af)
  {
    netmgr_log_err("%s(): Unknown address family", __func__);
    return;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_flush_doze_rules(af))
  {
    netmgr_log_err("%s(): failed to remove doze rules for af=%d",
                   __func__, af);
  }
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_oos_hndlr
===========================================================================*/
/*!
@brief
  Global OOS handler
*/
/*=========================================================================*/
static void
nm_mdmprxy_ndc_prov_oos_hndlr(void)
{
  /* Un-initialize iptables */
  if (iptables_inited)
  {
    nm_mdmprxy_ndc_prov_iptables_deinit();
  }

  nm_mdmprxy_ndc_prov_flush_doze_mode_rules(AF_INET);

  nm_mdmprxy_ndc_prov_flush_doze_mode_rules(AF_INET6);

  nm_mdmprxy_flush_vpn_skip_rules();

  nm_mdmprxy_common_flush_dl_ping6_rules();
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_is_hndlr
===========================================================================*/
/*!
@brief
  Global IS handler

@note
  - Dependencies
    - Global IS handler for the common module has been called
*/
/*=========================================================================*/
static void
nm_mdmprxy_ndc_prov_is_hndlr(void)
{
  iiface_refcount = 0;
}

/*===========================================================================
  FUNCTION  nm_mdmprxy_ndc_prov_init
===========================================================================*/
/*!
@brief
  Init function

@note
  - Dependencies
    - Global nm_mdmprxy_common_init
*/
/*=========================================================================*/
static void
nm_mdmprxy_ndc_prov_init(void)
{
  iiface_refcount = 0;
  iiface_epdg_refcount = 0;

  memset(&ndc_prov_iptables_cmd_tbl, 0, sizeof(ndc_prov_iptables_cmd_tbl));

  /* Copy all the command arrays to the iptables helper */
  ndc_prov_iptables_cmd_tbl.static_rule_list         = static_rule_list;
  ndc_prov_iptables_cmd_tbl.static_rule_list_len     = (int) nm_mdmprxy_rules_len(static_rule_list);

  ndc_prov_iptables_cmd_tbl.per_iiface_rule_list     = per_iiface_rule_list;
  ndc_prov_iptables_cmd_tbl.per_iiface_rule_list_len = (int) nm_mdmprxy_rules_len(per_iiface_rule_list);

  ndc_prov_iptables_cmd_tbl.per_oiface_rule_list     = per_oiface_rule_list;
  ndc_prov_iptables_cmd_tbl.per_oiface_rule_list_len = (int) nm_mdmprxy_rules_len(per_oiface_rule_list);

  ndc_prov_iptables_cmd_tbl.per_iface_forward_rule_list     = per_iface_forward_rule_list;
  ndc_prov_iptables_cmd_tbl.per_iface_forward_rule_list_len = (int) nm_mdmprxy_rules_len(per_iface_forward_rule_list);

  ndc_prov_iptables_cmd_tbl.local_breakout_rule_list     = local_breakout_rule_list;
  ndc_prov_iptables_cmd_tbl.local_breakout_rule_list_len = (int) nm_mdmprxy_rules_len(local_breakout_rule_list);

  ndc_prov_iptables_cmd_tbl.local_breakout_doze_rule_list     = local_breakout_doze_rule_list;
  ndc_prov_iptables_cmd_tbl.local_breakout_doze_rule_list_len = (int)nm_mdmprxy_rules_len(local_breakout_doze_rule_list);

  ndc_prov_iptables_cmd_tbl.per_iface_doze_skip_rule_list     = per_iface_doze_rule_list;
  ndc_prov_iptables_cmd_tbl.per_iface_doze_skip_rule_list_len = (int) nm_mdmprxy_rules_len(per_iface_doze_rule_list);

  ndc_prov_iptables_cmd_tbl.tunnel_doze_skip_rule_list     = tunnel_doze_rule_list;
  ndc_prov_iptables_cmd_tbl.tunnel_doze_skip_rule_list_len = (int) nm_mdmprxy_rules_len(tunnel_doze_rule_list);

  ndc_prov_iptables_cmd_tbl.vpn_skip_rule_list     = vpn_skip_rule_list;
  ndc_prov_iptables_cmd_tbl.vpn_skip_rule_list_len = (int) nm_mdmprxy_rules_len(vpn_skip_rule_list);

  nm_mdmprxy_register_route_prov_cmd_tbl(&ndc_prov_iptables_cmd_tbl);

  /* Install static rules */
  if (!iptables_inited)
  {
      nm_mdmprxy_ndc_prov_iptables_init();
      /* iiface rule list is nothing but the jump rules.
         The jump rules are static and can be installed at bootup.*/
      iiface_handle = nm_mdmprxy_install_iiface_rules(NM_MDMPRXY_IIFACE, NM_MDMPRXY_MARK);
  }

  /* Create custom oem network */
  if (NETMGR_SUCCESS
       != nm_mdmprxy_ndc_prov_create_network())
  {
    netmgr_log_err("%s(): failed!", __func__);
  }
}
