#ifndef MMCP_LOCAL_DEFINES_H
#define MMCP_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <mmcp> */

/* MMODE */
#define FEATURE_DUAL_SERVICE_IN_DATA
#define FEATURE_DUAL_SERVICE_IN_VOICE
#define FEATURE_MMGSDI_SESSION_READ_PRL
#define FEATURE_MULTIPLE_PRIMARY_PDP
#define FEATURE_OTASP_HOLD_CMD_IN_COMMIT
#define FEATURE_QMI_SERVICE_SPLIT
#define FEATURE_SS_L3_DATA

/* NAS */
#define FEATURE_GSM_GPRS_MSC34_SUPPORTED
#define FEATURE_LTE_3GPP_REL8_DEC09
#define FEATURE_REL_4_SUPPORT
#define FEATURE_REL_6_SUPPORT
#ifdef FEATURE_WCDMA
#define FEATURE_UMTS_BMC_DRX_SUPPORTED
#endif

/* WMS */
#define FEATURE_CDSMS_RUIM
#define FEATURE_CPHS


/* includes for local cust files
 * Note - these files are only visible to mmcp.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
#include "custmmode.h"
#include "custnas.h"
#include "custwms.h"

#endif /* MMCP_LOCAL_DEFINES_H */
