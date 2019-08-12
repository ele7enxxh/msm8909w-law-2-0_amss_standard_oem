#ifndef L1MSM_H
#define L1MSM_H
/*===========================================================================

                    L 1 C O N S T . H

DESCRIPTION
  This file contains constants MSM dependent defines for WCDMA L1.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2003 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1const.h_v   1.16   11 Jul 2002 21:26:14   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1msm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/13   pj       Wrapped HWIO access with WL1_HWIO macro
01/19/12   zr       Mainlined FEATURE_MAP_6250_DEFINES
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
#include "customer.h"
#include "msm.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define MICRO_CLK_CTL    ARM_CLK_CTL
// shall define in tramp_6250.h
#define KEYSESNSE_INT    TRAMP_KEY_SENSE_ISR

//#define hw_power_down_micro()  
// need to remove those later 
//#define HWIO_TEST_MEM_SEL_OUT(x) 0
//#define HWIO_TEST_MEM_ADDR_OUT(x) 0
//#define HWIO_TEST_MEM_READ0_IN 0
  
#ifdef FEATURE_WCDMA_HWIO_STUB

#define WL1_HWIO_INM(A, B)                   (0)
#define WL1_HWIO_OUT(A, B)                   MSG_HIGH("WL1_HWIO : " #A " = 0x%x ",(B),0,0) 
#define WL1_HWIO_IN(A)                       (0)
#define WL1_HWIO_FMSK(A, B)                  (0)
#define WL1_HWIO_SHFT(A, B)                  (0)
#define WL1_HWIO_OUTI(A, B, C)               MSG_HIGH("WL1_HWIO%d : " #A " = 0x%x",B,C,0)  
#define WL1_HWIO_ADDR(A)                     (0)
#define WL1_HWIO_ADDRI(A, B)                 (0)
#define WL1_HWIO_INI(A, B)                   (0)
#define WL1_HWIO_OUTM(A, B, C)               
#define WL1_HWIO_RMSK(A)                     (0)
#define WL1_HWIO_INMI(A, B, C)               (0)
#define WL1_HWIO_RSHFT(A)                    (0)

#else

#define HWIO_DEC_DOB_DECIPH_STATUS_TRBLK_NUM_BMSK   HWIO_DBACK_DECIPH_STATUS_DECIPH_TRBK_NUM_BMSK
#define HWIO_DEC_DOB_DECIPH_STATUS_TRBLK_NUM_SHFT   HWIO_DBACK_DECIPH_STATUS_DECIPH_TRBK_NUM_SHFT
#define WL1_HWIO_INM(A, B)                   HWIO_INM(A, B)
#define WL1_HWIO_OUT(A, B)                   HWIO_OUT(A, B)
#define WL1_HWIO_IN(A)                       HWIO_IN(A)
#define WL1_HWIO_FMSK(A, B)                  HWIO_FMSK(A, B)
#define WL1_HWIO_SHFT(A, B)                  HWIO_SHFT(A, B)
#define WL1_HWIO_OUTI(A, B, C)               HWIO_OUTI(A, B, C)
#define WL1_HWIO_ADDR(A)                     HWIO_ADDR(A)
#define WL1_HWIO_ADDRI(A, B)                 HWIO_ADDRI(A, B)
#define WL1_HWIO_INI(A, B)                   HWIO_INI(A, B)
#define WL1_HWIO_OUTM(A, B, C)               HWIO_OUTM(A, B, C)
#define WL1_HWIO_RMSK(A)                     HWIO_RMSK(A)
#define WL1_HWIO_INMI(A, B, C)               HWIO_INMI(A, B, C)
#define WL1_HWIO_RSHFT(A)                    HWIO_RSHFT(A)

#endif

#endif
