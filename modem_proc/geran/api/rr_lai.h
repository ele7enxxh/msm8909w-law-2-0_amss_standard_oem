#ifndef RR_LAI_H
#define RR_LAI_H
/*============================================================================
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_lai.h#1 $$DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $

 @file rr_lai.h

 Header file containing the definition of the LAI_T type

 Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_plmn.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

// represents an LAI
typedef struct
{
  PLMN_id_T plmn_id;
  word      location_area_code;
} LAI_T;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 *--------------------------------------------------------------------------*/


#endif /* #ifndef RR_LAI_H */

/* EOF */

