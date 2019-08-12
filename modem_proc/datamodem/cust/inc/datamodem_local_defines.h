#ifndef DATAMODEM_LOCAL_DEFINES_H
#define DATAMODEM_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <datamodem> */
#define FEATURE_32X_SCH_DATA_RATE
#define FEATURE_6280_HSDPA_PHASE1_CAPABILITY
#define FEATURE_6280_HSDPA_PHASE2_CAPABILITY
#define FEATURE_CM_MULTIMODE
#define FEATURE_DATA_QMI_UIM
#define FEATURE_DATA_V80
#define FEATURE_DS_IS2000_RS2_16X_F_SCH
#define FEATURE_DS_IS2000_RS2_16X_R_SCH
#define FEATURE_EXTENDED_PRL
#define FEATURE_HDRRLP_LOGGING
#define FEATURE_HDR_AT_TEST_ONLY
#define FEATURE_IS2000_DYNAMIC_FEATURE
#define FEATURE_IS2000_REL_A_MUXPDU5
#define FEATURE_MEID_SUPPORT
#define FEATURE_MMGSDI_ATCSIM
#define FEATURE_MM_SUPERSET
#define FEATURE_RUIM_CDMA_REFRESH
#define FEATURE_RUIM_FUNC_INTERFACE
#define FEATURE_UIM_SUPPORT_3GPD
#define FEATURE_UIM_SUPPORT_HRPD_AN

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/* includes for local cust files
 * Note - these files are only visible to datamodem.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
//#include "custmodemdata.h"
#include "custdatamodem.h" // New Consolidated file

#endif /* DATAMODEM_LOCAL_DEFINES_H */
