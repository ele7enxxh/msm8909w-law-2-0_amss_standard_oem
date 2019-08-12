#ifndef MCCDS_H
#define MCCDS_H

/*===========================================================================

            MAIN CONTROL DATA SERVICE SUPPORT FUNCTIONS

GENERAL DESCRIPTION
  This module contains utilities to handle requests from Data Service Task.

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccds.h_v   1.1.2.0   30 Nov 2001 17:11:44   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mccds.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/09   jj      Meta-Comments cleanup for API's exported to DS.
08/31/09   adw     Removed unnecessary customer.h and target.h includes.
07/01/09   jj      Included cai.h to resolve a compile issue.
05/14/09   jj      Split this file as part of CMI phase2.
12/13/07   pg      Fixed featurization problem on FEATURE_IS2000_CHS and
                   FEATURE_IS2000_R_SCH.
05/09/06   TMR     Removed remoting of mccds_reg_chs_notifier()... now being
                   shimmed as callback is called in interrupt context.
02/01/06   pg      RPC related changes.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
06/26/01   sb      Merged the following from common archive:
		   06/03/01   ht      Added $Header line for revision control.
05/11/01   jq      Added mccds_reg_chs_notifier.
05/02/01   fc      Added support of T_ADD_ABORT.
04/10/01   jq      Moved all SCRM related data definitions and function
                   prototypes from mccsch.h to here.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "cai.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

// Type for requesting channel resources
typedef enum
{
  MCCDS_RES_NONE,
  MCCDS_RES_SCRM,
  MCCDS_RES_RRM
} mccds_resource_req_type;


/*===========================================================================

FUNCTION mccds_reg_resource_generator

DESCRIPTION
  This function binds a SCRM blob generator to a function pointer. If an old
  SCRM blob generator already exists, it overwrites it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef mccds_resource_req_type (*mccds_resource_req_func_type) (uint8* sz_blob, 
                                                                 uint8* blob, 
                                                                 uint16 so);
/*~ PARAM OUT sz_blob POINTER */
/*~ PARAM OUT blob VARRAY CAI_MAX_SCRM_BLOB_SIZE LENGTH CAI_MAX_SCRM_BLOB_SIZE */

void mccds_reg_resource_generator
(
  mccds_resource_req_func_type ds_resource_generator
);

/*===========================================================================

FUNCTION mccds_reg_chs_notifier

DESCRIPTION
  This function binds a DS function to modify the Control Hold State status
  inside DS to synchronize state changes in MC. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*mccds_chs_notifier_type) (boolean chs_on);

void mccds_reg_chs_notifier
(
  mccds_chs_notifier_type ds_chs_notifier
);

#endif // MCCDS_H
