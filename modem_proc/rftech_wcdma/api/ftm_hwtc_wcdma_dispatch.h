/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

					 F T M  H W T C  W C D M A  D I S P A T C H
                              

GENERAL DESCRIPTION
  This is the header file for the embedded FTM WCDMA Dispatch commands 
  comming from the diag service 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/ftm_hwtc_wcdma_dispatch.h#1 $ $DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/12   Saul    WCDMA. Added APIs to set LTE PA state and RGI.
07/10/12   Saul    WCDMA. Added tune_with_lte_settings API.
10/19/07   dw      Added command to set c0_c1 delays 
10/02/06   ka      Added command to set rxf digital filter coefficients.
01/29/03   xw      Created 
===========================================================================*/

#ifndef FTM_HWTC_WCDMA_DISPATCH_H
#define FTM_HWTC_WCDMA_DISPATCH_H

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC
#include "comdef.h"
#include "target.h"
#include "ftm_hwtc_cmd.h"

typedef enum {
  FTM_HWTC_SET_IM2_I_DAC,
  FTM_HWTC_SET_IM2_Q_DAC,
  FTM_HWTC_SET_TRANSCONDUCTOR,
  FTM_HWTC_ENABLE_DISABLE_IM2,
  FTM_HWTC_GET_IM2_POWER,
  FTM_HWTC_SET_RXF_FILTER_COEFFICIENTS,
  FTM_HWTC_SET_C0_C1_DELAY,
  FTM_HWTC_WCDMA_TUNE_WITH_LTE_SETTINGS = 7,
  FTM_HWTC_WCDMA_TUNE_SET_LTE_PA_STATE  = 8,
  FTM_HWTC_WCDMA_TUNE_SET_LTE_RGI       = 9,
  FTM_HWTC_WCDMA_MAX_ENUM=0xFFFF
} ftm_hwtc_wcdma_dispatch_enum;

void ftm_hwtc_wcdma_dispatch(ftm_hwtc_pkt_type *ftm_hwtc);

#endif /* FEATURE_FTM_HWTC */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_HWTC_WCDMA_DISPATCH_H */
