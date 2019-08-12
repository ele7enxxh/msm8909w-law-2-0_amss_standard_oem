#ifndef MSM_MM_H
#define MSM_MM_H
/*========================================================================================

                                    M S M  M M  I N T E R F A C E

DESCRIPTION

  This file contains the interface commands and stuctures between MM and MBMS Service Manager


  Copyright (c) 2005 by Qualcomm Technologies Incorporated.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mbmsm/inc/msm_mm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------    ---     ----------------------------------------------------------
01/11/07    ajt     Created file

===========================================================================*/
                   

/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/
/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/


/*===========================================================================
**                  CONSTANT/DEFINE DECLARATIONS
**=========================================================================*/

/*===========================================================================
**                       ENUMERATED TYPES
**=========================================================================*/

/*===========================================================================
**                      TYPE DECLARATIONS
**=========================================================================*/

/****************************************************************************

STRUCTURE: MM_MSM_SUPP_IND_S_TYPE

This structure tells if MBMS is supported or not at RA level.

*****************************************************************************/
typedef struct
{
  /* MBMS Supported in RA or Not */
  boolean mbms_support;

  // PLMN byte 0 = MCC digit 2 | MCC digit 1
  // PLMN byte 1 = MNC digit 3 | MCC digit 3
  // PLMN byte 2 = MNC digit 2 | MNC digit 1
  byte plmn_id[3];
}mm_msm_supp_ind_s_type;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/

#endif

