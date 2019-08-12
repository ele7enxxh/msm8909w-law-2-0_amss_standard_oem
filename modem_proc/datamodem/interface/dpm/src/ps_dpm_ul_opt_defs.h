#ifndef PS_DPM_UL_OPT_DEFS_H
#define PS_DPM_UL_OPT_DEFS_H
/*===========================================================================

                     PS_DPM_UL_OPT_HDLRI_DEFS.H
DESCRIPTION
  Header file describing all Optimized data path definitions used by
    both UL/DL.

Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_ul_opt_defs.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/27/13    wc     Support dynamic port configuration
01/29/13    vb     Fix to handle the GRE packet processing in
                   NAT optimized path.
09/14/12    tw     IPv6 prefix missmatch errors fixed
05/24/2012  pgm    RNDIS defns.
05/07/2012  mp     Fix to send TCP Reset for exhausted NAT entry.
08/15/2011  AM     created module.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#define PS_DPM_UL_OPT_IP_VERSION_MASK  0xF0000000
#define PS_DPM_UL_OPT_IP_PKT_LEN_MASK  0x0000FFFF
#define PS_DPM_UL_OPT_IPV4_VERSION     0x40000000
#define PS_DPM_UL_OPT_IPV6_VERSION     0x60000000

/*---------------------------------------------------------------------------
  ps optimized hdlr path types
  PS_DPM_UL_OPT_NO_PATH: Error cases, packet drops, Flow_controlled
  PS_DPM_UL_OPT_OPT_PATH: Um tx_cmd
  PS_DPM_UL_OPT_QOS_PATH: When Filters installed-ps_iface_tx_cmd()
  PS_DPM_UL_OPT_LEGACY_PATH: fallback_rx_sig_hdlr()
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DPM_UL_OPT_NO_PATH            = 0,
  PS_DPM_UL_OPT_OPT_PATH           = 1,
  PS_DPM_UL_OPT_QOS_PATH           = 2,
  PS_DPM_UL_OPT_LEGACY_PATH        = 3,
  PS_DPM_UL_OPT_FLOW_CONTROL_PATH  = 4
} ps_dpm_ul_opt_path_type;

#endif /* PS_DPM_UL_OPT_DEFS_H */
