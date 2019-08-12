#ifndef UIM_LOCAL_DEFINES_H
#define UIM_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <uim> */
#define FEATURE_UIM_TOOLKIT


/* includes for local cust files
 * Note - these files are only visible to uim.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custuim.h"
#include "custpbm.h"

#endif /* UIM_LOCAL_DEFINES_H */
