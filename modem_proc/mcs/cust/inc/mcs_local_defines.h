#ifndef MCS_LOCAL_DEFINES_H
#define MCS_LOCAL_DEFINES_H
/*===========================================================================

DESCRIPTION
  Local defines for MCS.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cust/inc/mcs_local_defines.h#1 $
===========================================================================*/

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <mcs> */
#define FEATURE_IS2000_REL_A_TD


/* includes for local cust files
 * Note - these files are only visible to mcs.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custmcs.h"

#endif /* MCS_LOCAL_DEFINES_H */
