#ifndef RF_HAL_H
#define RF_HAL_H

/*=============================================================================

                R F _ H A L   H E A D E R   F I L E 
       
DESCRIPTION
  This file contains declarations and definitions to support a Hardware
  Abstraction Layer.

Copyright (c) 2006, 2007   by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/hal/common/inc/rf_hal.h#1 $
$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/12   SAR     Feature reduction changes.
06/07/11   aro     Mainlined FEATURE_RF_SVDO_API
03/19/10   lcl     Pull in changes from QSC6295
01/28/10   sar     Replaced msmhwio.h with msm.h per CMI.
10/24/08   rmd     QSC7X30 code clean up: Removed: T_MSM7600A.
09/22/08   Vish    Added "msmhwioreg.h" to make this the only header file that
                   needs to be included to gain access to all MSM HW registers.
07/14/08   adk     Ported DORB changes from the 7800 branch
11/15/07   ems     Support for QSC1100
08/13/07   vm      More updates for Sc2x
06/25/07   Vish    Changes for DOrB using RF_HAS_DO_REV_B (????)
02/27/07   dyc     Added QSC60X5 support
11/16/06   jfc     Added RFH_SHDW() macro
11/15/06   jfc     Renamed RF_TODO_7600 with RF_MSM7600_HWIO
11/14/06   adk     Added macros HWIO_CDMA_SHADOW_* for shadow registers.
10/31/06   ycl     Added HWIO_SAMPSRV_TEST_RAM_DATA_ADDR for ftm files.
10/31/06   ycl     Delete macros in msm_help.h.
10/26/06   jfc     Initial revision

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "msm.h"
#include "msmhwioreg.h"

/*===========================================================================

                         GLOBAL DEFINIATIONS and VARIABLES

===========================================================================*/

/* Ideally RFH_SHDWI() should be mapped to HWIO_SHDWI() but the msmhwioreg.h 
   file does not contain the shadow register names for RXF registers
*/

#define RFH_SHDW(hwiosym)                    __rfh_shdw(hwiosym)
#define RFH_SHDWI(hwiosym, index)            __rfh_shdwi(hwiosym, index)
#define RFH_SHDWI2(hwiosym, index1, index2)  __rfh_shdwi2(hwiosym, index1, index2)

#define __rfh_shdw(hwiosym)                    HWIO_##hwiosym##_shadow
#define __rfh_shdwi(hwiosym, index)            HWIO_##hwiosym##_shadow[index]
#define __rfh_shdwi2(hwiosym, index1, index2)  HWIO_##hwiosym##_shadow[index1][index2]

#define RFHAL_MODEM_CLK_MISC_CTL  MODEM_CLK_MISC_CTL_2
#define RXF_UMTS(hwiosym)         UMTS_##hwiosym
#define RXF_CDMA(hwiosym)         CDMA_##hwiosym
#define RXF_MODEM(hwiosym)        MODEM_##hwiosym
#define RFH(hwiosym)              RFHAL_##hwiosym

#endif /* RF_HAL_H */


