#ifndef PS_SOFTAP_CONFIG_H
#define PS_SOFTAP_CONFIG_H
/*===========================================================================

                N A T _ I F A C E _ H D L R I . H

DESCRIPTION
   This is NAT iface handler internal header file.

Copyright (c) 2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/ifacectls/src/ps_softap_config.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef __cplusplus
extern "C" {
#endif


#define PS_SOFTAP_CONFIG_XML_STRING_MAX_SIZE 50


/*===========================================================================

                            DATA DECLARATIONS

===========================================================================*/
  /* First add support for one rule and then later we can extend this */
typedef struct
{
  boolean   firewall_enabled;
  boolean   are_pkts_allowed;
  uint16    firewall_port;
  uint8     firewall_protocol;
  uint16    nat_entry_timeout;
  uint32    port_fwding_private_ip;
  uint16    port_fwding_private_port;
  uint16    port_fwding_global_port;
  uint8     port_fwding_protocol;
  uint32    dmz_ip;
  uint8     enable_ipsec_vpn_pass_through;
  uint8     enable_pptp_vpn_pass_through;
  uint8     enable_l2tp_vpn_pass_through;
  uint8     initialized;
}ps_softap_config_params_type;

/*---------------------------------------------------------------------------
  Structure that holds the Static SOFTAP configuration
---------------------------------------------------------------------------*/
extern ps_softap_config_params_type   ps_softap_config;

/*===========================================================================

                            FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION 

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE
 None
===========================================================================*/
void ps_softap_config_init
( void );

#ifdef __cplusplus
}
#endif

#endif /* PS_SOFTAP_IFACE_CONFIG_H */
