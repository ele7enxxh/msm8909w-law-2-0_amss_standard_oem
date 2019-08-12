#ifndef CUST_LOCAL_DEFINES_H
#define CUST_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <cust> */
#define FEATURE_DBM
#define FEATURE_GSDI_MULTICLIENT_SUPPORT


/* includes for local cust files
 * Note - these files are only visible to cust.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custcdma2000.h"
#include "custmodem.h"
#include "custrela.h"
#include "custsrch.h"

#endif /* CUST_LOCAL_DEFINES_H */
