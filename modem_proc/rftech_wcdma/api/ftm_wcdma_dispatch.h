#ifndef FTM_WCDMA_DISPATCH_H
#define FTM_WCDMA_DISPATCH_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      F T M  W C D M A  D I S P A T C H   

GENERAL DESCRIPTION
  This is the header file for the embedded FTM WCDMA Commands 


Copyright (c) 2002, 2003, 2012 - ? by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_wcdma_dispatch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/30/13   aro     Added WCDMA radio dispatch
01/30/13   ka      Updated FTM interfaces to logical device
07/10/12   Saul    WCDMA. Added tune_with_lte_settings API.
01/18/06   dp/bhas Add WCDMA diversity support
10-18-04   bhas    Removed FTM_HAS_ASYNC_PKT
08-21-02   rjr     Created
===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmdiag.h"
#include "ftm.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type 
ftm_wcdma_dispatch
(
  rfcom_device_enum_type device, 
  ftm_pkt_type *cmd_ptr
);


/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_wcdma_radio_dispatch
(
  void  * ftm_req_data
);

#ifdef __cplusplus
}
#endif

#endif  // FEATURE_FACTORY_TESTMODE
#endif  // FTM_WCDMA_DISPATCH_H
