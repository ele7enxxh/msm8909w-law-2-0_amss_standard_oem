/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=======*

            TM Periodic PPM Sub-module

General Description
  This file contains implementations for TM Periodic PPM Module

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=======*/

/*==============================================================================

                           Edit History
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_periodic_ppm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
==============================================================================*/

#ifndef TM_PERIODIC_PPM_H
#define TM_PERIODIC_PPM_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

FUNCTION: tm_periodic_ppm_init

DESCRIPTION:
  This function is called by TM-Core to initialize tm_periodic_ppm at bootup
   
PARAMETERS:
    
===========================================================================*/
void tm_periodic_ppm_init(void);

#ifdef __cplusplus
}
#endif

#endif /* TM_PERIODIC_PPM_H */

