#ifndef HDR_LOCAL_DEFINES_H
#define HDR_LOCAL_DEFINES_H

/*===========================================================================
  Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <hdr> */
#define FEATURE_TRM_API_V2


/* includes for local cust files
 * Note - these files are only visible to hdr.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custhdr.h"

#endif /* HDR_LOCAL_DEFINES_H */
