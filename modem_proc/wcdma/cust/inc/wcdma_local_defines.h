#ifndef WCDMA_LOCAL_DEFINES_H
#define WCDMA_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <wcdma> */
#define FEATURE_G2W_EVENTS_SUPPORTED
#define FEATURE_MDSP_MULTI_RF_CHAIN_INTERFACE
#define FEATURE_MMODE_CMI_4
#define MSMHW_GSM_INIT_IN_WL1_INIT


/* includes for local cust files
 * Note - these files are only visible to wcdma.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custwcdma.h"

#endif /* WCDMA_LOCAL_DEFINES_H */
