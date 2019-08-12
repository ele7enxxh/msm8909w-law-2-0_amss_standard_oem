#ifndef CORE_LOCAL_DEFINES_H
#define CORE_LOCAL_DEFINES_H

/* FEATUREs previously defined in a target-owned cust file or
 * another SUs cust file but only used by <core> */
#define FEATURE_16BIT_SHADOW_REG
#define FEATURE_4PAGE_TX_LINEARIZER
#define FEATURE_AGC_LOGGING
#define FEATURE_BROADCAST_SMS
#define FEATURE_COMPLETE_SNM
#define FEATURE_DATA_SERIALIZER
#define FEATURE_DH
#define FEATURE_DH_EXP
#define FEATURE_DSM_MEM_CHK_NO_FILL_AA
#define FEATURE_DS_CHAP_NV
#define FEATURE_DS_CM
#define FEATURE_DS_ENABLE_RM_DMNCY_IF_CTA_0
#define FEATURE_DS_ORIG_IS707_SO
#define FEATURE_DS_PAP_NV
#define FEATURE_DS_SOCKETS
#define FEATURE_EVRC
#define FEATURE_EVRC_RDA
#define FEATURE_EVRC_SO_MGMT
#define FEATURE_FASTPRL
#define FEATURE_FAST_WARMUP
#define FEATURE_FFPC
#define FEATURE_GPIO_GROUP_INT
#define FEATURE_GSRM_SVC_IND
#define FEATURE_INT_125_FIQ
#define FEATURE_IS2000_CAGC
#define FEATURE_IS2000_F_SCH
#define FEATURE_IS2000_LTU_CRCS
#define FEATURE_IS2000_QPCH
#define FEATURE_IS2000_QPCH_CCI
#define FEATURE_IS2000_SCH_STATS
#define FEATURE_IS95B_ACCESS_ENTRY_HO
#define FEATURE_IS95B_ACCESS_HO
#define FEATURE_IS95B_ACCESS_PROBE_HO
#define FEATURE_IS95B_EXT_DISP
#define FEATURE_IS95B_INC_CALL_FWD_ENH
#define FEATURE_IS95B_MAHHO
#define FEATURE_IS95B_NDSS
#define FEATURE_IS95B_NGHBR_SRCH_ENH
#define FEATURE_IS95B_PILOT_RPT
#define FEATURE_IS95B_PPSMM
#define FEATURE_IS95B_RELEASE_ON_AC
#define FEATURE_IS95B_SOFT_HO
#define FEATURE_IS95B_SYS_RESELEC
#define FEATURE_MAX_PWR_PROTECTION
#define FEATURE_MC_TIMER_FIX
#define FEATURE_MINIMIZE_ISR_LOADING
#define FEATURE_OTASP_OTAPA
#define FEATURE_PA_RANGE_TEMP_FREQ_COMP
#define FEATURE_POWER_CLASS_III
#define FEATURE_RDA_CORRECTION
#define FEATURE_RELATIVE_DMOD_POS
#define FEATURE_RFR3100
#define FEATURE_RLGC
#define FEATURE_RUIM_CHV_FROM_NV
#define FEATURE_SCH_TRIAGE
#define FEATURE_SEC_MEM_TUNING
#define FEATURE_SLEEP_SLOW_CLOCK
#define FEATURE_SUBPCG_PA_WARMUP_DELAY
#define FEATURE_TMSI
#define FEATURE_TX_ACCESS_POWER_FIX
#define FEATURE_UART_RATE1
#define T_SLEEP


/* includes for local cust files
 * Note - these files are only visible to core.  Any new FEATURE used by multiple SUs must be 
 * provisioned in the FEATURE database, after which it will be included in the <su>_variation.h 
 * file generated for each SUs that uses the FEATURE. */
//#include "custdiag.h"
#include "custipsec.h"
//#include "custrex.h"
#include "custsec.h"
//#include "custsio_8960.h"

#endif /* CORE_LOCAL_DEFINES_H */
