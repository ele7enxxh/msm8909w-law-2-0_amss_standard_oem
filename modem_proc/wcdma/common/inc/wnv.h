/*============================================================================
                            W N V . H
DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/common/inc/wnv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/28/16    sr      FR 38167: Changes to restrict the code to specific PLMN
01/12/16   sn      Changes for FR30881
10/16/15    as      Added NV for LTA during activation time
10/12/15    svh     CME features NV.
09/10/15    ac      Port MSIM support from TH 2.0
02/09/15    svh     Added NV for MDSP RECOVERY stats.
12/29/14    bc      Made changes to control the range of frequencies to be skipped around a WCDMA cell by an nv.
11/24/14    svh     Added NV for MDSP RECOVERY.
09/22/14    amj     NV for NASTT Power Optimizations and other search debug.
09/17/14    ac      Added WNV_CONF_ID_WSRCH_NBREJ and WNV_CONF_ID_QTA_ROTA
08/26/14    svh     Added changes for CIR enhancement NV.
07/21/14    ac      Rename RRC_DSDS_CONCURRENCY_CELLFACH to DEFAULT_CONFIG_IN_CELL_FACH
07/15/14    ac      Initial checkin 
===========================================================================*/

#ifndef _WNV_H_
#define _WNV_H_

#include "comdef.h"
#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include <string.h>
#include "msg.h"
#include "err.h"
#include "nv_items.h"
#include "uecomdef.h"
#include "rrcmmif.h"
/*************************************************************************** 
                   Exported structure definitions
****************************************************************************/

/* ID for each CONF file. Associated with a conf_func. */
typedef enum 
{
  WNV_CONF_ID_RXD               = 0,      /* WL1 RxD */
  WNV_CONF_ID_FET               = 1,      /* WL1 FET */
  WNV_CONF_ID_CME               = 2,      /* WL1 QICE */
  WNV_CONF_ID_DL                = 3,      /* WL1 DL */
  WNV_CONF_ID_SAR               = 4,      /* WL1 UL SAR */
  WNV_CONF_ID_ANT_SWITCH        = 5,      /* WL1 antenna switch  */
  WNV_CONF_ID_ANT_SWITCH_BAND   = 6,      /* WL1 antenna switch bands */
  WNV_CONF_ID_SRCH              = 7,      /* WL1 searcher  */
  WNV_CONF_ID_WSRCH_NBREJ       = 8,      /* WL1 NBR */
  WNV_CONF_ID_SRCH_GLOBAL       = 9,      /* WL1 searcher  */
  WNV_CONF_ID_WENC              = 10,      /* WL1 UL */
  WNV_CONF_ID_MULTISIM          = 11,     /* WL1 multi-SIM */
  WNV_CONF_ID_QTA_ROTA          = 12,     /* WL1 QTA ROTA */
  WNV_CONF_ID_DSR               = 13,     /* WL1 DSR */
  WNV_CONF_ID_ATUNER            = 14,     /* WL1 ATUNER */
  WNV_CONF_ID_W_IDLE_MODE       = 15,     /* WL1 idle mode optimizations */
  WNV_CONF_ID_DC_W2L_MODE       = 16,     /* WL1 DC+W2L settings */
  WNV_CONF_ID_L1_HSCFG          = 17,     /* WL1 HS CFG */
  WNV_CONF_ID_IDLE_DEBUG        = 18,     /* WL1 idle debug */
  WNV_CONF_ID_MDSP_RECOVERY     = 19,     /* WL1 MDSP RECOVERY */

  WNV_CONF_ID_RRC               = 20,     /* WRRC */
  WNV_CONF_ID_RRC_DEVICE_TYPE   = 21,     /* WRRC Device type */
  WNV_CONF_ID_RRC_FEATURE       = 22,     /* WRRC feature*/

  WNV_CONF_ID_MAX
} wnv_conf_id_enum;

/* ID for each NV file. EFS files are associated with a default function and CONF file.
   Legacy NV items do not have either. */
typedef enum
{
  /* EFS files */
  /* L1 - RxD */
  WNV_ID_L1_RXD_RSCP_THRESH                       = 0,  /* NV 65726 */
  WNV_ID_L1_RXD_QUAL_THRESH                       = 1,  /* NV 65724 */
  WNV_ID_L1_RXD_TPR_THRESH                        = 2,  /* NV 65725 */
  WNV_ID_L1_RXD_ARD_THRESH                        = 3,  /* NV 67234 */
  WNV_ID_L1_RXD_ARD_MMT_TIME                      = 4,  /* NV 67235 */
  WNV_ID_L1_RXD_ARD_DWELL_TIME                    = 5,  /* NV 67236 */
  WNV_ID_L1_RXD_ARD_V2_DECISION_DELTA             = 6,  /* NV 67319 */
  WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH1              = 7,  /* NV 67320 */
  WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH2              = 8,  /* NV 67321 */
  WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH3              = 9,  /* NV 67322 */
  WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH4              = 10, /* NV 67323 */
  WNV_ID_L1_RXD_ARD_V2_MIN_DWELL_TIME_K           = 11, /* NV 67351 */
  WNV_ID_L1_RXD_ARD_V2_FILTER_SETTLE_TIME_L       = 12, /* NV 67352 */
  WNV_ID_L1_RXD_OPTS                              = 13, /* NV 71539 */
  WNV_ID_L1_RXD_PCH_FILTECIO_UPPER_THRESH         = 14, /* NV 66025 */
  WNV_ID_L1_RXD_PCH_FILTECIO_LOWER_THRESH         = 15, /* NV 66026 */
  WNV_ID_L1_RXD_PCH_PI_SUM_THRESH                 = 16, /* NV 66028 */
  WNV_ID_L1_RXD_PCH_MIN_FULL_DIV_CYCLES           = 17, /* NV 66027 */
  WNV_ID_L1_RXD_PCH_ORIDE_VAL                     = 18, /* NV 66029 */

  /* L1 FET */
  WNV_ID_L1_FET_CONTROL                           = 19, /* NV 67329 */

  /* L1 CME */
  WNV_ID_L1_CME_OPTS                              = 20, /* NV 70309 */
  WNV_ID_L1_CME_QPM_AVG_RSSI_VAR                  = 21, /* NV 67349 */
  WNV_ID_L1_CME_QPM_AVG_RSSI_VAR_RETAIN           = 22, /* NV 67350 */
  WNV_ID_L1_CME_CIR_ENH                           = 23, /* NV 73503 */
  WNV_ID_L1_CME_FEAT                              = 24, /* NV 73683 */

  /* L1 DL */
  WNV_ID_L1_DL                                    = 25, /* NV 70188 */
  WNV_ID_L1_HARQ_FLUSH_TIMER_MAX                  = 26, /* NV 72529 */

  /* L1 UL */
  WNV_ID_L1_UL_SAR                                = 27, /* NV 70355 */
  WNV_ID_L1_UL_RACH                               = 28, /* NV 70282 */

  /* L1 ANT_SWITCH*/
  WNV_ID_L1_ANT_SWITCH                            = 29, /* NV 70240 */
  WNV_ID_L1_ANT_SWITCH_BAND                       = 30, /* NV 70276 */

  /* L1 SRCH */
  WNV_ID_L1_SRCH_E1D                              = 31, /* NV 72516 */
  WNV_ID_L1_NB_REJECTION                          = 32, /* NV 72549 */
  WNV_ID_SRCH_GLOBAL                              = 33, /* NV  */

  /* L1 MULTISIM */
  WNV_ID_L1_MULTISIM_W2G_TUNEAWAY                 = 34, /* NV 67244 */
  WNV_ID_L1_QTA_ROTA                              = 35, /* NV 72562 */

  /* L1 DSR */
  WNV_ID_L1_DSR                                   = 36, /* NV 67250 */

  /* L1 ATUNER */
  WNV_ID_L1_ATUNER                                = 37, /* NV 71551 */

  /* L1 DC W2L */
  WNV_ID_L1_DC_W2L                                = 38, /* NV 69690 */

  /* L1 IDLE MODE */
  WNV_ID_L1_IDLE_MODE_OPT                         = 39, /* NV 69733 */
  WNV_ID_L1_IDLE_DEBUG                            = 40, /* NV 72595 */

  /* L1 MDSP RECOVERY */
  WNV_ID_L1_MDSP_RECOVERY                         = 41, /* NV 73560 */
  WNV_ID_L1_MDSP_REC_STAT                         = 42, /* NV 73636 */

  /* RRC*/
  WNV_ID_RRC_WTOL_CM_SUPPORT                      = 43, /* NV 67256 */	
  WNV_ID_RRC_PPAC_SUPPORT                         = 44, /* NV 67305 */
  WNV_ID_RRC_SAVE_ACQDB                           = 45, /* NV 67309 */
  WNV_ID_RRC_BAND_PRIORITY_CONFIG                 = 46, /* NV 67293 */
  WNV_ID_RRC_DISABLE_CPC_FDPCH                    = 47, /* NV 67333 */
  WNV_ID_RRC_DISABLE_CRASH_DEBUG_INFO             = 48, /* NV 72515 (may change)*/
  WNV_ID_RRC_WTOL_PS_HO_SUPPORT                   = 49, /* NV 67347 */
  WNV_ID_RRC_WTOL_TDD_PS_HO_SUPPORT               = 50, /* NV 69732 */
  WNV_ID_RRC_IGNORE_CELL_BAR_RESERVE_STATUS       = 51, /* NV 65782 */
  WNV_ID_RRC_CSFB_SKIP_SIBL1_OPT                  = 52, /* NV 65885 */
  WNV_ID_RRC_FREQ_LOCK_ITEM                       = 53, /* NV 65818 */
  WNV_ID_RRC_DO_NOT_FORWARD_PAGE                  = 54, /* NV 66047 */
  WNV_ID_RRC_FREQ_SCAN_TIMER_IN_MS                = 55, /* NV 70352 */
  WNV_ID_RRC_DEEP_SLEEP_NO_SVC_TIMER_IN_MS        = 56, /* NV 70351 */
  WNV_ID_RRC_BPLMN_FREQ_SCAN_TIMER_IN_MS          = 57, /* NV 70350 */
  WNV_ID_RRC_SIB_SLEEP_SB                         = 58, /* NV 72599 */
  WNV_ID_RRC_CONSTANT_320MS_DRX                   = 59, /* NV 72563 */
  WNV_ID_RRC_PRIORITIZE_850_BAND                  = 60, /* NV 72612 */
  WNV_ID_RRC_DISABLE_MEAS                         = 61, /* NV 72520 */
  WNV_ID_RRC_DORMANCY_ENABLE                      = 62, /* NV 73499 */
  WNV_ID_WCDMA_RRC_ENABLE_PSC_LOCK                = 63, /* NV 70241 */
  WNV_ID_WCDMA_RRC_CSG_PRUNE_COUNTER              = 64, /* NV ????? */
  WNV_ID_WCDMA_CSG_SRCH_CARRIER_SPACE             = 65, /* NV 70363 */
  WNV_ID_WCDMA_ASF_TIMER                          = 66, /* NV 72574 */
  WNV_ID_WCDMA_RRC_SNOW3G_ENABLED                 = 67, /* NV 65820 */
  WNV_ID_WCDMA_RRC_SET_SRB2_ACT_TIME              = 68, /* NV 65825 */
  WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB          = 69, /* NV 67219 */
  WNV_ID_WCDMA_RRC_FAST_RETURN_TO_LTE_AFTER_CSFB_TIMER    = 70, /* NV 67220 */
  WNV_ID_RRC_GANSS_SUPPORT_NV                             = 71, /* NV 71564 */
  WNV_ID_RRC_FE_FACH_SUPPORT                              = 72, /* NV 71571 */
  WNV_ID_RRC_DSDS_CONCURRENCY_EXTENDED                    = 73, /* NV 72581 */
  WNV_ID_RRC_DSDA_CONCURRENCY_EXTENDED                    = 74, /* NV 72582 */
  WNV_ID_RRC_TSTS_CONCURRENCY_EXTENDED                    = 75, /* NV 72583 */
  WNV_ID_RRC_NV_ENABLE_MFBI_SUPPORT                       = 76, /* NV 72548 */
  WNV_ID_RRC_UL_COMPR_CAP_SUPPORT                         = 77, /* NV 72576 */
  WNV_ID_RRC_FEATURE                                      = 78, /* NV 65693 */
  WNV_ID_RRC_DEFAULT_CONFIG_IN_CELL_FACH                  = 79, /* NV 72526 */
  WNV_ID_RRC_DEVICE_TYPE                                  = 80, /* NV 70310 */
  WNV_ID_RRC_SIB7_EXP_TIME                                = 81, /* NV 73514 */
  WNV_ID_RRC_ENABLE_MEAS_ID_EXTN                          = 82, /* NV 73526 */
  WNV_ID_RRC_IDLE_FEATURES                                = 83, /* NV 73584 */
  WNV_ID_RRC_DB_DC_BAND_COMB_NV_CONTROL                   = 84, /* NV 73595 */
  WNV_ID_RRC_HALF_DRX_LEN                                 = 85, /* NV 73619*/
  WNV_ID_RRC_FREQ_SKIP_RANGE_NV                           = 86, /* NV 73617*/
  WNV_ID_RRC_M2M_FEATURE_ENABLE_NV                        = 87, /* NV 73776*/
  WNV_ID_RRC_SUPPORT_LTA                                  = 88, /* NV 73774*/
  WNV_ID_RRC_M2M_BITMASK_NV                               = 89, /* NV 73778*/
  WNV_ID_RRC_PSUEDO_SIB19                                 = 90, /* NV 73818 */
  WNV_ID_RRC_BAND66_PLMN_SUPPORT                         = 91,  /* NV 73911 */
  WNV_ID_EFS_COUNT,

  /* Legacy NV items */
  WNV_ID_LEGACY_START                                     = 1000,  /* This must match next line */

  /* L1 */                                           
  WNV_ID_BTFD_OLPC_FLOOR_DB_I                             = 1000,  /* NV 939 */
  WNV_ID_NON_BTFD_OLPC_FLOOR_DB_I                         = 1001,  /* NV 940 */
  WNV_ID_GPRS_ANITE_GCF_I                                 = 1002,  /* NV 947  */
  WNV_ID_WCDMA_SPIKE_MITIGATION_ENABLED_I                 = 1003,  /* NV 1800 */
  WNV_ID_WCDMA_RRC_VERSION_I_FOR_L1                       = 1004,  /* NV 3649 */
  WNV_ID_WCDMA_RX_DIVERSITY_CTRL_I                        = 1005,  /* NV 3851 */
  WNV_ID_WCDMA_EQUALIZER_CTRL_I                           = 1006,  /* NV 3852 */
  WNV_ID_HSDPA_CAT_I                                      = 1007,  /* NV 4118*/
  WNV_ID_WCDMA_TEST_NV_1_I                                = 1008,  /* NV 5458 */
  WNV_ID_BOARD_CAPABILITIES_I                             = 1009,  /* NV 6800 */
  /* RRC */                                          
  WNV_ID_WCDMA_HSUPA_CM_CTRL_I                            = 1010,  /* NV 5090 */
  WNV_ID_WCDMA_HSUPA_DEFAULT_CATEGORY_I                   = 1011,  /* NV 4210 */
  WNV_ID_WCDMA_OPTIONAL_FEATURE_LIST_I                    = 1012,  /* NV 4211 */
  WNV_ID_INTERRAT_NACC_SUPPORT_I                          = 1013,  /* NV 4116 */
  WNV_ID_GERAN_FEATURE_PACK_1_I                           = 1014,  /* NV 2512 */
  WNV_ID_RRC_INTEGRITY_ENABLED_I                          = 1015,  /* NV 880 */
  WNV_ID_RRC_CIPHERING_ENABLED_I                          = 1016,  /* NV 881 */
  WNV_ID_HSDPA_COMPRESSED_MODE_ENABLED_I                  = 1017,  /* NV 4159 */
  WNV_ID_RRC_FAKE_SECURITY_ENABLED_I                      = 1018,  /* NV 882 */
  WNV_ID_WCDMA_RRC_PDCP_DISABLED_I                        = 1019,  /* NV 4173 */
  WNV_ID_RRC_VERSION_I_FOR_RRC                            = 1020,  /* NV 3649 */
  WNV_ID_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I               = 1021,  /* NV 6878 */
  WNV_ID_WCDMA_DL_FREQ_ENABLED_I                          = 1022,  /* NV 3525 */
  WNV_ID_WCDMA_DL_FREQ_I                                  = 1023,  /* NV 3524 */
  /* L2 */                                           
  WNV_ID_RRC_VERSION_I_FOR_L2                             = 1024,  /* NV 3649 */
  /* WPLT */
  WNV_ID_PREF_MODE_I                                      = 1025,  /* NV 10 */

  WNV_ID_LEGACY_END                                       = 1025   /* This value must match the previous line */
} wnv_nv_id_enum;

/* API return values */
typedef enum
{
  WNV_SUCCESS        = 0,
  WNV_ERROR          = 1,
  WNV_FILE_EXISTS    = 2,
  WNV_FILE_NOT_FOUND = 3
} wnv_api_status;

/*************************************************************************** 
                          Exported macros
****************************************************************************/

typedef sys_modem_as_id_e_type wsub_id_e_type;

/* Gets scalar default value for given NV ID */
#define WNV_GET_DEFAULT(nv_id) (nv_id ## _DEFAULT)

/* Number of legacy NVs */
#define WNV_LEGACY_NV_COUNT (WNV_ID_LEGACY_END - WNV_ID_LEGACY_START + 1)

/* Wrapper macros that hard-code sub_id to 0 */
#define wnv_init_efs_conf_and_nv_files(conf_id) wnv_init_efs_conf_and_nv_files_msim(conf_id, SYS_MODEM_AS_ID_1)
#define wnv_get_nv(nv_id, out_buffer, out_len) wnv_get_nv_msim(nv_id, out_buffer, out_len, SYS_MODEM_AS_ID_1)
#define wnv_get_default(nv_id, out_buffer, out_len) wnv_get_default_msim(nv_id, out_buffer, out_len, SYS_MODEM_AS_ID_1) 

/*************************************************************************** 
                          Exported functions
****************************************************************************/
/*===========================================================================
FUNCTION        wnv_init_efs_conf_and_nv_files_msim

DESCRIPTION     Creates conf file for given WNV_CONF_ID if it doesn't already 
                exist. Initializes NV files for this conf file to default
                values if they don't already exist.

DEPENDENCIES    None

RETURN VALUE    WNV_SUCCESS if conf file was written successfully 
                WNV_ERROR if something went wrong writing conf file
                WNV_FILE_EXISTS if conf file already exists
 
SIDE EFFECTS    ERR_FATAL if conf_id is invalid 
                             wsub_id is invalid 
                             conf file is too large to write in one efs_write
===========================================================================*/
wnv_api_status wnv_init_efs_conf_and_nv_files_msim (wnv_conf_id_enum conf_id, /* ID of CONF file*/
                                                    wsub_id_e_type wsub_id    /* Subscription ID */
                                                    );

/*===========================================================================
FUNCTION        wnv_get_nv

DESCRIPTION     Retrieves contents of EFS NV for given WNV_NV_ID. If it does not 
                exist, default will be returned. 

DEPENDENCIES    None

RETURN VALUE    WNV_SUCCESS Successfully read from EFS 
                WNV_ERROR   Failed to read from EFS - default returned 

SIDE EFFECTS    ERR_FATAL if nv_id is invalid 
                          wsub_id is invalid 
                          out_buffer is null
                          out_len smaller than size of default data
===========================================================================*/
wnv_api_status wnv_get_nv_msim (wnv_nv_id_enum nv_id,   /* ID of NV */
                                void* out_buffer,       /* Buffer where NV contents will be copied */
                                uint32 out_len,         /* Length of buffer */
                                wsub_id_e_type wsub_id  /* Subscription ID */
                                 );

/*===========================================================================
FUNCTION        wnv_get_nv_default_msim

DESCRIPTION     Retrieves default value for given WNV_NV_ID. 

DEPENDENCIES    None

RETURN VALUE    WNV_SUCCESS always

SIDE EFFECTS    ERR_FATAL if nv_id is invalid 
                             wsub_id is invalid 
                             out_buffer is null
                             out_len smaller than size of default data
===========================================================================*/
uint32 wnv_get_nv_default_msim (wnv_nv_id_enum nv_id,    /* ID of NV */
                                void* out_buffer,        /* Buffer where NV contents will be copied */
                                uint32 out_len,          /* Length of NV contents */
                                wsub_id_e_type wsub_id   /* Subscription ID */
                                );


/*************************************************************************** 
                          Internal functions
****************************************************************************/
/*===========================================================================
FUNCTION        wnv_default_func

DESCRIPTION     Function that will get the default value for the given NV.

DEPENDENCIES    None

RETURN VALUE    WNV_SUCCESS Default read successfully (when write_to_file == FALSE) 
                WNV_SUCCESS Default written successfully (when write_to_file == TRUE)
                WNV_FILE_EXISTS	NV file already exists – nothing written (when write_to_file == TRUE)
                WNV_ERROR	Error occurred during writing (when write_to_file == TRUE)

SIDE EFFECTS    ERR_FATAL if out_len smaller than size of default data 
                             nv_id is not handled by this function 
===========================================================================*/
typedef wnv_api_status (*wnv_default_func)(wsub_id_e_type wsub_id,  /* Subscription ID */
                                           wnv_nv_id_enum nv_id,    /* ID of NV */
                                           void* out_buffer,        /* Buffer where NV contents will be copied */
                                           uint32 output_len,       /* Length of NV contents */
                                           boolean write_to_file    /* TRUE to write to file (if it doesn't exist) */
                                           );

/*===========================================================================
FUNCTION        wnv_int_write_default

DESCRIPTION     Writes default value to file it if does not exist
 
DEPENDENCIES    None

RETURN VALUE    WNV_SUCCESS if default written
                WNV_ERROR if some EFS error occurred during writing

SIDE EFFECTS    None
===========================================================================*/
wnv_api_status wnv_int_write_default(wsub_id_e_type wsub_id,       /* Subscription ID */
                                     wnv_nv_id_enum nv_id,         /* ID of NV */
                                     void* default_buffer,         /* Buffer containing default data */
                                     uint32 default_len            /* Length of default data */
                                     );


/*************************************************************************** 
                      Internal structure definitions
****************************************************************************/

/* Mapping of CONF ID to CONF handlers */
typedef PACKED struct PACKED_POST
{
  wnv_default_func  default_func;     /* NV default function */
  const char       *filename;         /* EFS filename for CONF file */
} wnv_conf_id_map;

/* Mapping of EFS NV ID to NV handlers */
typedef PACKED struct PACKED_POST
{
  uint8             conf_id;          /* Associated conf file ID */
  uint8             path_prefix_idx;  /* Index of path prefix */
  const char       *filename;         /* EFS filename for NV */
} wnv_nv_id_efs_map;

/* Mapping of legacy NV ID to NV handlers */
typedef PACKED struct PACKED_POST
{
  uint16      default_func_idx; /* NV default function index */
  uint16      nv_item;          /* NV item number */
} wnv_nv_id_legacy_map;

#endif /* _WNV_H_*/
