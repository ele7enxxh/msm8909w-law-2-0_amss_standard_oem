#ifndef __BATT_EFS_H__
#define __BATT_EFS_H__
/*!
  @file batt_efs.h

  @brief
   EFS Item support for the LMTSMGR BATT
*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/batt/inc/batt_efs.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/24/12   ag      EFS Restructuring
05/06/12   ag      Initial Revision

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "lmtsmgr_i.h"

/*=============================================================================

                       CONSTANTS AND DEFINES

=============================================================================*/

/*=============================================================================

                             TYPEDEFS

=============================================================================*/
typedef struct
{
  int16   battery_limit;
  boolean valid;
}batt_lut_type;

typedef struct 
{
  const char *batt_limit_fname;
} batt_efs_fname_type;

typedef struct
{
  batt_efs_fname_type           files;
  /* List of all LUTs for this band combination */
  void*                        luts;
  /* Max bands for this combination */
  uint32                       num_tech1_bands;
  uint32                       num_tech2_bands;
}batt_tech_comb_lut_type;

typedef PACK (struct)
{
  uint32   hprio_band;
  uint32   lprio_band;
  int16    hprio_limit;
}batt_efs_record_type;

typedef struct 
{
  cxm_tech_type hptech;
  uint32 hpband;
  cxm_tech_type lptech;
  uint32 lpband; 
} batt_efs_lookup_in;

typedef struct
{
  int16 hplimit;
  boolean hplimit_valid;
} batt_efs_lookup_out;



/*=============================================================================

                        FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================

  FUNCTION:  batt_efs_init

=============================================================================*/
/*!
    @brief
    Initializes BATT EFS
 
    @return
    None
*/
/*===========================================================================*/
extern void batt_efs_init (void);

/*=============================================================================

  FUNCTION:  batt_efs_lookup

=============================================================================*/
/*!
    @brief
    Performs a lookup to get the current Battery current Tx limits.
 
    @return
    None
*/
/*===========================================================================*/
extern boolean batt_efs_lookup
(
  batt_efs_lookup_in* input,
  batt_efs_lookup_out* output
);

#endif /* __BATT_EFS_H__ */
