#ifndef TDSL1MSM_H
#define TDSL1MSM_H
/*===========================================================================

                    L 1 C O N S T . H

DESCRIPTION
  This file contains constants MSM dependent defines for WCDMA L1.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2003 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1const.h_v   1.16   11 Jul 2002 21:26:14   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1msm.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/04   bbd      Changed feature for HW CPP to FEATURE_SET_HW_CPP_PF_GAINS
02/11/04   bbd      Added pilot filter gains for HW CPP block
11/11/03   yus      Replace FEATURE_6250_COMPILE with FEATURE_MAP_6250_DEFINES 
11/05/03   yus      Remove rf_log_single_agc_data()
10/14/03   abp      Removed empty macro hw_power_down_micro()
10/10/03   yus      Add temp define for rf_log_single_agc_data()  
07/15/03   yus      Created File
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "msm.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*************************************************/
/*                   MSM6250                     */
/************************************************/
#if 0
#ifdef FEATURE_SET_HW_CPP_PF_GAINS
/* Pilot filter gains for HW CPP block */
#define TDSL1MSM_CPP_PF_FILT_GAIN1           13107
#define TDSL1MSM_CPP_PF_FILT_GAIN2           10486
#define TDSL1MSM_CPP_PF_FILT_GAIN3           6554
/* Pilot filter gains for HW CPP block, non-STTD only */
#define TDSL1MSM_CPP_NON_STTD_PF_FILT_GAIN1  6554
#define TDSL1MSM_CPP_NON_STTD_PF_FILT_GAIN2  10486
#define TDSL1MSM_CPP_NON_STTD_PF_FILT_GAIN3  11796
#endif /* FEATURE_SET_HW_CPP_PF_GAINS */
#endif

#endif
