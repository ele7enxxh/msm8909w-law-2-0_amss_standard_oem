#ifndef _PS_DPM_UL_OPT_H
#define _PS_DPM_UL_OPT_H
/*===========================================================================

                     PS_DPL_UL_OPT_HDLR.H

DESCRIPTION
Header file describing all UL data optimization Signal handlers

Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm_ul_opt.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/01/13    rk     Removed feature mobileap.
09/30/13    bvd    DPM and IPA filtering changes
09/27/13    wc     Support dynamic port configuration
07/11/2012  pgm    Added RNDIS sig hdlr.
09/22/2011  pgm    Added MBIM sig hdlr.
08/19/2011  AM     Code Cleanup
08/15/2011  AM     UL optimized funtion prototypes
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "ps_rm_defs.h"

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_DPM_UL_OPT_INIT

DESCRIPTION


PARAMETERS


RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_dpm_ul_opt_init
(
  void
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_IP_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_IP_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
  TRUE:
  FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_ETH_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH NO QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_ETH_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH QOS mode. It decides
  if the packet needs to take the legacy path or should be forwarded to Um
  iface.

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_eth_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_EMBMS_RX_SIG_HDLR

DESCRIPTION
  This function drops any data for embms call becuase for embms call
  there is no data expected on uplink path

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_embms_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

#ifdef FEATURE_DATA_PS_464XLAT
/*===========================================================================
FUNCTION ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr

DESCRIPTION
  This function is the optimized Signal handler for IP NO QOS mode with CLAT
  capability. It decides if the packet needs to take the legacy path or should 
  be forwarded to Um iface. If a v4 packet is received it is translated to v6
  pkt.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_IP_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for IP QOS mode with CLAT 
  capability. It decides if the packet needs to take the legacy path or 
  should be forwarded to Um iface.If a v4 packet is received it is translated 
  to v6 pkt.

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
  TRUE:
  FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_ETH_NO_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH NO QOS mode with CLAT 
  capability. It decides if the packet needs to take the legacy path or 
  should be forwarded to Um iface. If a v4 packet is received it is translated 
  to v6 pkt.

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION PS_DPM_UL_OPT_CLAT_ETH_QOS_RX_SIG_HDLR

DESCRIPTION
  This function is the optimized Signal handler for ETH QOS mode with CLAT 
  capability. It decides if the packet needs to take the legacy path or 
  should be forwarded to Um iface. If a v4 packet is received it is translated 
  to v6 pkt.

PARAMETERS
  rx_sig    : RmNet signal type.
  user_data : pointer to user data.

RETURN VALUE
 TRUE:
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

/*===========================================================================
FUNCTION ps_dpm_mbim_ul_opt_clat_ip_rx_sig_hdlr

DESCRIPTION
  This function is the optimized Signal handler for CLAT IP NO QOS mode. It 
  decides if the packet needs to take the legacy path or should be forwarded 
  to Um iface. If a v4 packet is received it is translated to v6 pkt.

PARAMETERS
 rx_sig    :  RmNet signal type
 user_data :  pointer to user data.

RETURN VALUE
 TRUE :
 FALSE:

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_mbim_ul_opt_clat_ip_rx_sig_hdlr
(
  ps_rm_sig_enum_type    rx_sig,
  void                 * user_data
);

#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* _PS_DPM_UL_OPT_H */
