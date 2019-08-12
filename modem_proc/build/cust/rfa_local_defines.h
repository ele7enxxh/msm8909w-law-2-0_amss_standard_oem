#ifndef RFA_LOCAL_DEFINES_H
#define RFA_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <rfa> */
#define FEATURE_BARRED_CELL_DURING_CELL_SELECTION
#define FEATURE_HDR_NON_SIGNALING_MSGR_INTF
#define FEATURE_OPTIMIZE_SEARCH_FOR_HHO
#define FEATURE_RFA_CMI_API
#define FEATURE_RF_TASK
#define FEATURE_TX_POWER_BACK_OFF
#define FEATURE_WCDMA_MIN_INIT_TX_PWR
#define FEATURE_XO_FACTORY_CAL
#define GERAN_NIKEL_FW_INTERFACE
#define RF_FEATURE_LTOG
#define RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
#define RF_HAS_CDMA_DIME_SUPPORT


/* includes for local cust files
 * Note - these files are only visible to rfa.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */

#endif /* RFA_LOCAL_DEFINES_H */
