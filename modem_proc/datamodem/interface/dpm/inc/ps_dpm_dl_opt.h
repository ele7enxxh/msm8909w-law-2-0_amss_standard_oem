#ifndef _PS_DL_OPT_HDLR_H
#define _PS_DL_OPT_HDLR_H
/*===========================================================================

                     PS_OPTIMIZED_HDLR_H . H
DESCRIPTION


EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/inc/ps_dpm_dl_opt.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/30/13    svj    Increased PS_DL_OPT_MBIM_MAX_NDP_AGGR_DGRMS to 10.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "ps_dpmi.h"

#define PS_DL_OPT_AGGR_MORE_PACKETS     (0x0D81FACE)
#define PS_DL_OPT_AGGR_NO_MORE_PACKETS  (0x0)

#define PS_DL_OPT_MBIM_MAX_NDP_AGGR_DGRMS   (10)
#define PS_DL_OPT_MBIM_NDP_TERM             (1)
#define PS_DL_OPT_MBIM_MAX_NDP_ENTRIES                         \
	(PS_DL_OPT_MBIM_MAX_NDP_AGGR_DGRMS + PS_DL_OPT_MBIM_NDP_TERM)

boolean ps_dl_phys_opt_rx_pkt
(
  const ps_phys_link_type  * this_phys_link_ptr,
  dsm_item_type           ** pkt_ref_ptr,
  ps_rx_meta_info_type     * rx_mi_ptr,
  ps_dpm_bearer_info_type  * bearer_ptr
);

boolean ps_dl_iface_opt_rx_pkt
(
  ps_iface_type            * this_iface_ptr,
  dsm_item_type           ** pkt_ref_ptr
);

#endif /* _PS_DL_OPT_HDLR_H */

