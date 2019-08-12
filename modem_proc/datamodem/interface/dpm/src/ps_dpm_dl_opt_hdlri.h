#ifndef _PS_DL_OPT_HDLRI_H
#define _PS_DL_OPT_HDLRI_H
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
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_dl_opt_hdlri.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_MBIM_SUPPORT

#include "dsm.h"
#include "ds_rmnet_smi.h"

boolean ps_dl_opt_add_mbim_header
(
  rmnet_smi_info_type  * rmnet_info,
  dsm_item_type       ** pkt_ptr,
  uint16                 total_len
);

#endif /*FEATURE_DATA_PS_MBIM_SUPPORT*/
#endif /* _PS_DL_OPT_HDLRI_H */

