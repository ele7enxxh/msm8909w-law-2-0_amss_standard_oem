#ifndef OEMDRE_H
#define OEMDRE_H
/*=============================================================================

                        OEM DR Enablement Header File

GENERAL DESCRIPTION
  This file contains contains the structure definitions and function prototypes for 
  the OEM DR Enablement.

 
EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/oem/inc/sm_oemdre.h#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/07/13   rh      Removed NO_FIX report handling (now BestAvailPos) 
02/10/10   vp      Initial version.

=============================================================================*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include "sm_api.h"
#include "gnss_common.h"
#include "sm_log.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Macros

=============================================================================*/

/*=============================================================================

       Typedefs

=============================================================================*/

/* Type of OEMDR Data reported from SM to OEM layer */
typedef enum
{
  OEMDRE_UNKNOWN,
  OEMDRE_MEAS,
  OEMDRE_POS_FIX,
  OEMDRE_SVPOLY,
  OEMDRE_POS_NO_FIX
} oemdre_dataType;

/* Type of OEMDRE Status reported from SM/LM to OEM layer */
typedef uint32 oemdre_statusType;
#define  FEATURE_OEMDRE_NONE          0
#define  FEATURE_OEMDRE_NOT_SUPPORTED 1
#define  FEATURE_OEMDRE_ON            2
#define  FEATURE_OEMDRE_OFF           3
#define  FEATURE_OEMDRE_ALREADY_ON    4
#define  FEATURE_OEMDRE_ALREADY_OFF   5


/* Full Data structures capable of reporting GNSS information is used should we 
decide to report non-GPS data also at some point in future */
typedef struct
{
  oemdre_dataType           oemdreDataType;
  union {
    sm_GpsMeasRptStructType   oemdreMeas;
    sm_GnssFixRptStructType   oemdrePosFix;
    gnss_SvPoly               oemdreSvPoly;
  } u;
} oemdre_data;

/*=============================================================================

       Functions

=============================================================================*/

/*****************************OEM DRE API functions***********************/


/*===========================================================================

FUNCTION oemdreData

DESCRIPTION
  This function receives data from PE and ME and sends it to OEM layer.
  
DEPENDENCIES
  None.

PARAMETERS
  p_OemdreData - OEM DR data

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void oemdreData(const oemdre_data* pOemdrData);

/*===========================================================================

FUNCTION oemdreOn

DESCRIPTION
  This function turns on OEMDR Data Reporting
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  oemdre status type

SIDE EFFECTS
  None.

===========================================================================*/
oemdre_statusType oemdreOn(void);

/*===========================================================================

FUNCTION oemdreOff

DESCRIPTION
  This function turns off OEMDR Data Reporting.
  
DEPENDENCIES
  None.

PARAMETERS
  None. 

RETURN VALUE
  uint32 - oemdre status type

SIDE EFFECTS
  None.

===========================================================================*/
oemdre_statusType oemdreOff(void);
#endif /* OEMDRE_H */

