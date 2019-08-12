#ifndef RFM_TYPES_H
#define RFM_TYPES_H

/*===========================================================================


   R F  D r i v e r  M u l t i m o d e  T y p e s
                           H e a d e r  F i l e

DESCRIPTION
  This file contains all type definitions used by the RFM layer .


Copyright (c) 2008 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfm_types.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/18/14   sc      Remove unused code and dependencies 
03/11/13   sar     Updates for APQ, GNSS only target.
09/20/12   pl      Remove RFLTE_MC() macro and rflte_mc.h as LTE external interfaces
                   are message based
09/07/11   jhe     Removed #ifdef's to comply with CMI.
08/08/11   hy      Changed FEATURE_RF_HAS_TDSCDMA to FEATURE_TDSCDMA
07/20/11   jhe     Added Macros for TDSCDMA MC functions
04/05/11   sar     Removed conditional #def's to comply with CMI.
12/16/10   aro     Added Macros for CDMA MC functions
12/09/10   aro     Added Support to Compile SVDO Build
03/19/10   lcl     Pull in changes from QSC6295
03/01/10   tws     Add macros for new IRAT API.
02/03/10   kma     Added macros RFGSM_MC_V
01/08/10   ckl     Removed high level lint error.
12/09/09   ntn     Added macros RF1X_MC_V, RFHDR_MC_V and RFGNSS_MC_V  
11/25/09   ntn     Added macros RFMEAS_MC_GTOW_V and RFMEAS_MC_WTOG_V  
10/11/09   ckl     Added WCDMA macro for void function.
09/15/09   bmg     Added SVDO mode macro
08/17/09   gy      Removed FEATURE_CGPS definition.
03/14/09   gy      Updated RFGNSS_MC micro and added FEATURE_GPS temporarily.
03/14/09   sar     Added back rfm_hdr
03/13/09   sr      removed the FEATURE_MEAS define
03/12/09   sar     Added changes needed for 1x
10/14/08   sr      Initial version.

============================================================================*/

#include "rfcom.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfm_wcdma.h"
#include "rfwcdma_mc.h"
#include "rfm_gsm.h"
#include "rfm_1x.h"
#include "rfm_hdr.h"
#endif
#include "rfm_gnss.h"
#include "rfgnss_mc.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfm_meas.h"
#include "rfmeas_mc.h"
#endif

#define RFCOMMON_MC(func, ret_val) ret_val = rfcommon_mc_##func
#define RFWCDMA_MC(func, ret_val) ret_val = rfwcdma_mc_##func
#define RFWCDMA_MC_V(func) (void)rfwcdma_mc_##func
#define RFGSM_MC(func, ret_val) ret_val = rfgsm_mc_##func
#define RFGSM_MC_V(func) (void)rfgsm_mc_##func
#define RFMEAS_MC_GTOW(func, ret_val) ret_val = rfmeas_mc_gtow_##func
#define RFMEAS_MC_GTOW_V(func) rfmeas_mc_gtow_##func 
#define RFMEAS_MC_WTOG(func, ret_val) ret_val = rfmeas_mc_wtog_##func
#define RFMEAS_MC_WTOG_V(func) rfmeas_mc_wtog_##func 
#define RFMEAS_MC_UNIFY_WTOG(func, ret_val) ret_val = rfmeas_mc_unify_wtog_##func
#define RFMEAS_MC(func, ret_val) ret_val = rfmeas_mc_##func
#define RFCDMA_MC(func, ret_val) ret_val = rf_cdma_mc_##func
#define RF1X_MC(func, ret_val) ret_val = rf_1x_mc_##func
#define RF1X_MC_V(func) rf_1x_mc_##func
#define RFHDR_MC(func, ret_val) ret_val = rf_hdr_mc_##func
#define RFHDR_MC_V(func) rf_hdr_mc_##func
#define RFGNSS_MC(func, ret_val) ret_val = rfgnss_mc_##func
#define RFGNSS_MC_V(func) rfgnss_mc_##func
#define RFTDSCDMA_MC(func, ret_val) ret_val = rf_tdscdma_mc_##func

/*!Default temperature value for rf thermistors*/
/*the invalid temperature value is equivalent to -273 degree celcius (0 kelvin)*/
#define RFM_INVALID_TEMPERATURE 0xFEEF

#endif /* RFM_TYPES_H */

