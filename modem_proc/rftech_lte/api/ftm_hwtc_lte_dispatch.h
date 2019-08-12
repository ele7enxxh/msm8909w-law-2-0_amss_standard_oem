/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

					 F T M  H W T C  L T E  D I S P A T C H
                              

GENERAL DESCRIPTION
  This is the header file for the embedded FTM LTE

   Dispatch commands 
  comming from the diag service 

Copyright (c) 1994, 1995, 1996 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000, 2001       by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/ftm_hwtc_lte_dispatch.h#1 $                                  

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/27/10   tnt     Add IM2 support for band 17/4/2/7/11/3/20 
04/22/10   tnt     Initial check-in
===========================================================================*/

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_FTM_HWTC
#include "comdef.h"
#include "target.h"
#include "ftm_hwtc_cmd.h"

typedef enum {
  FTM_HWTC_LTE_SLEEP_MODE      = 0,
  FTM_HWTC_LTE_WAKEUP_MODE     = 1,
  FTM_HWTC_LTE_IRAT_TO_WCDMA   = 2,
  FTM_HWTC_LTE_IRAT_TO_1X      = 3,
  FTM_HWTC_LTE_IRAT_TO_DO      = 4,
  FTM_HWTC_LTE_IRAT_TO_GSM     = 5,
  FTM_HWTC_WCDMA_RETURN_TO_LTE = 6,
  FTM_HWTC_1X_RETURN_TO_LTE    = 7,
  FTM_HWTC_DO_RETURN_TO_LTE    = 8,
  FTM_HWTC_GSM_RETURN_TO_LTE   = 9,
  FTM_HWTC_WCDMA_IRAT_TO_LTE   = 10,
  FTM_HWTC_1X_IRAT_TO_LTE      = 11,
  FTM_HWTC_DO_IRAT_TO_LTE      = 12,
  FTM_HWTC_GSM_IRAT_TO_LTE     = 13,
  FTM_HWTC_LTE_RETURN_TO_WCDMA = 14,
  FTM_HWTC_LTE_RETURN_TO_1X    = 15,
  FTM_HWTC_LTE_RETURN_TO_DO    = 16,
  FTM_HWTC_LTE_RETURN_TO_GSM   = 17,
  FTM_HWTC_LTE_B13_IM2_PRX_CONTOUR = 18,
  FTM_HWTC_LTE_B13_IM2_DRX_CONTOUR = 19,
  FTM_HWTC_LTE_B1_IM2_PRX_CONTOUR = 20,
  FTM_HWTC_LTE_B1_IM2_DRX_CONTOUR = 21,
  FTM_HWTC_LTE_B17_IM2_PRX_CONTOUR = 22,
  FTM_HWTC_LTE_B17_IM2_DRX_CONTOUR = 23,
  FTM_HWTC_LTE_B4_IM2_PRX_CONTOUR = 24,
  FTM_HWTC_LTE_B4_IM2_DRX_CONTOUR = 25,
  FTM_HWTC_LTE_B2_IM2_PRX_CONTOUR = 26,
  FTM_HWTC_LTE_B2_IM2_DRX_CONTOUR = 27,
  FTM_HWTC_LTE_B7_IM2_PRX_CONTOUR = 28,
  FTM_HWTC_LTE_B7_IM2_DRX_CONTOUR = 29,
  FTM_HWTC_LTE_B11_IM2_PRX_CONTOUR = 30,
  FTM_HWTC_LTE_B11_IM2_DRX_CONTOUR = 31,
  FTM_HWTC_LTE_B3_IM2_PRX_CONTOUR = 32,
  FTM_HWTC_LTE_B3_IM2_DRX_CONTOUR = 33,
  FTM_HWTC_LTE_B20_IM2_PRX_CONTOUR = 34,
  FTM_HWTC_LTE_B20_IM2_DRX_CONTOUR = 35,
  FTM_HWTC_LTE_MAX_ENUM        =0xFFFF
} ftm_hwtc_lte_dispatch_enum;

/*--------------------------------------------------------------------------*/
void ftm_hwtc_lte_dispatch(ftm_hwtc_pkt_type *ftm_hwtc);

#endif /* FEATURE_FTM_HWTC */
#endif /* FEATURE_FACTORY_TESTMODE */
