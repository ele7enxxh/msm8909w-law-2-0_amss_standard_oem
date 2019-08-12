/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Session Manager NV Items Interface Header File

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes for 
  the SM Non-Volatile Memory interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2005, 2006 - 2011, 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Copyright (c) 2013-2014 QUALCOMM Atheros, Inc.
All Rights Reserved.
QCA Proprietary and Confidential.
 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/inc/sm_nv.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $ 

=============================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/15   rh      Added support for MultiSIM NV API  
11/04/14    ah     Support SUPL TD-SCDMA LocID Config through OEM Feature Mask NV
10/27/14    gk     NV item for terminating SUPL after final fix 
04/17/14   skm     NV Async Read changes 
01/31/14    ss     Supporting Max acceptable GF PUNC confguration from QMI
10/08/13    ah     Moved Tunnel Detection enablement bit to resolve bit conflict
05/21/10    gk     RC thresolds were set incorrectly from defaults
04/13/10   atien   NV_AAGPS_POSITIONING_MODES_SUPPORTED_V support all except GSM NMR
08/22/08   jlp     Added support of emergency call simulation for simulating
                   E911 A-GPS
09/14/07    gk     NMEA CONFIG set parameters support
04/17/07    lt     Changed default aagps_positioning_modes_supported to "all".
04/12/07    ah     Added default read values for all SM-related NV items.
11/14/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef SM_NV_H
#define SM_NV_H

/*
 * Include files
*/

#include "comdef.h"

#include "aries_os_api.h"
#include "aries_os_globals.h"

#include "nv.h"
#include "pdapi.h"
#include "queue.h"

#include "custcgps.h"
#include "sys.h"

/*
 * Constant definitions
*/
#define SM_NV_CLIENT_HANDLE_MAX      5
#define SM_NV_CLIENT_HANDLE_ERROR    0xFF

#define NV_AAGPS_APP_TRACKING_GPSON_THSLD_V          5 
#define NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_V        15
#define NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_V  60

#define PDSM_MAX_PDDM_DEFAULT_VAL                   200

#define LCS_AGENT_DEFAULT_ALL_DISABLED                0
#define LCS_AGENT_DEFAULT_TELESERVICE_ID          65001
#define LCS_AGENT_DEFAULT_MAX_DURATION       0xFFFFFFFF
#define PDSM_LCS_AGENT_DISABLE_GPS_DURING_VOICE_CALL  0
#define NV_CGPS_NMEA_DEFAULT_PORT_TYPE PDSM_PA_NMEA_PORT_TYPE_NONE
#define NV_CGPS_NMEA_DEFAULT_REPORTING_TYPE PDSM_PA_NMEA_REPORT_AT_1HZ_INTERVAL

/*Enable only "legacy" nmea sentences*/
#define NV_GNSS_NMEA_DEFAULT_GNSS_SENTENCE_TYPE  0x3F
#define NV_GNSS_NMEA_DEFAULT_GNSS_EXTENDED_SENTENCE_TYPE  0x0

enum 
{
  AAGPS_NV_EMERGENCY_SUPPORT_REJECT_ALL,    /* if in E911, reject all location requests */
  AAGPS_NV_EMERGENCY_SUPPORT_HIGH_PRIORITY, /* if in E911, only accept high priority location requests */
  AAGPS_NV_EMERGENCY_SUPPORT_LOW_PRIORITY   /* if in E911, accept even low priority location requests */
};

enum 
{
  AAGPS_NV_GPS_UNLOCKED,
  AAGPS_NV_GPS_LOCKED
};


/* Default VALUES for variables that are normally stored in NV as AAGPS related items,
   to be used in case NV read fails */
#define NV_AAGPS_EMERGENCY_SERVICES_SPPRT_V         AAGPS_NV_EMERGENCY_SUPPORT_HIGH_PRIORITY
#define NV_AAGPS_POSITIONING_MODES_SUPPORTED_V      0xFF7F /* default to support all except GSM NMR */
#define NV_AAGPS_DEFAULT_ALLOW_RRC_V                     1 /* 0 is false, 1 is TRUE */
#define NV_AAGPS_DEFAULT_MTLR_GUARD_TIMER_V				10 /* seconds */
#define NV_AAGPS_DEFAULT_SMLC_COMM_TIMEOUT_V            20 /* seconds */
#define NV_AAGPS_DEFAULT_REF_TIME_UNC_V				   160 /* default to 8000ms; note unit is 50ms */
#define NV_AAGPS_DEFAULT_REF_POSITION_UNC_V          10000 /* meters */
#define NV_AAGPS_DEFAULT_GPS_LOCK_CONTROL_V          AAGPS_NV_GPS_UNLOCKED  
#define NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER1_VALUE_V      20 /* seconds */
#define NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER2_VALUE_V      20 /* seconds */
#define NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER3_VALUE_V      20 /* seconds */
#define NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_V            '\0'
#define NV_AAGPS_DEFAULT_USE_TRANSPORT_SECURITY_V        1
#define NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_V            32
#define NV_GNSS_SUPL_VERSION_V                        0x00020000 /* from MSB to right: rsvd, maj, min, servInd */
#define NV_GNSS_RRLP8_RRC8_SUPPORTED_V                0 /* RRLP/RRC Version 8 (GANSS portion) not supported */

#define NV_AAGPS_RTI_VALIDITY_DUR_V						 14400000 /* 4 hours: in msec */

#define NV_AAGPS_2G_MO_LRSUPPORT_V						 7
#define NV_AAGPS_3G_MO_LRSUPPORT_V						 3
#define NV_AAGPS_DEFAULT_QOS_TIME_V            64 /* seconds */
#define NV_AAGPS_DEFAULT_QOS_UNC_V             50 /* meters */

/* 	NV_ITEM aagps_2g_mo_lr_support : byte */
#define NV_AAGPS_2G_MO_LR_LOW_ACCURACY  0x01  /* legacy: support of non-GPS positioning only, obsolete */
#define NV_AAGPS_2G_MO_LR_HIGH_ACCURACY 0x02  /* if not set, block 2G MO AGPS */
#define NV_AAGPS_2G_MO_LR_ASSIST_DATA   0x04  /* if set, use AD call flow; otherwise use LocEst */

/* 	NV_ITEM aagps_3g_mo_lr_support : byte */
#define NV_AAGPS_3G_MO_LR_LOW_ACCURACY  0x01  /* legacy: support of non-GPS positioning only, obsolete */
#define NV_AAGPS_3G_MO_LR_HIGH_ACCURACY 0x02  /* if not set, block 3G MO AGPS */
#define NV_AAGPS_3G_MO_LR_ASSIST_DATA   0x04  /* if set, use AD call flow; otherwise use LocEst */

/* NV_ITEM aagps_mt_lrsupport: byte */
#define NV_AAGPS_MT_LR_CP_GSM_WCDMA_SUPPORTED 0x1 /* inform network of value_added supported if set */
#define NV_AAGPS_MT_LR_UP_GSM_WCDMA_BLOCKED   0x2 /* ignore G/W UP NI trigger if set */
#define NV_AAGPS_MT_LR_UP_LPP_SUPPORTED       0x04
#define NV_AAGPS_MT_LR_CP_LPP_SUPPORTED       0x08

#define SM_NV_XSPI_DEFAULT_INJECTION_TIMEOUT    (0) /* 0 = wait forever */
#define SM_NV_XSPI_DEFAULT_STREAM_TIMEOUT       (1000) /* in msec = 1 sec */

/* GPS1_CALL_RELATED NV bitmask definitions: uint8 */
#define NV_GPS1_CALL_RELATED_MASK_1_ENABLE_FUNC  0x01  /* Enable GPS call functionality, default is ON */
#define NV_GPS1_CALL_RELATED_MASK_2_END_MT_CALL  0x02  /* End CDMA MT Call immediately, default is Off */

/* OEM_FEATURE_MASK NV bitmask definitions: uint32 */
#define NV_GNSS_OEM_FEATURE_MASK_1_OEMDRE                        0x00000001 /* for OEM DRE Data Enablement (used in LM) */
#define NV_GNSS_OEM_FEATURE_MASK_2_OEMDATA                       0x00000002 /* for PRM/PPM/SA/AA OEM data (used in LM) */
#define NV_GNSS_OEM_FEATURE_MASK_3_1XCPMT                        0x00000004 /* To accept 1X CP MT even if MO is active (used in TM) */
#define NV_GNSS_OEM_FEATURE_MASK_4_SUPL_NI_NOTIF_LOOPBK          0x00000008 /* unconditional acceptance of SUPL_INIT by pdsm */
#define NV_GNSS_OEM_FEATURE_MASK_5_CP_SS_NOTIF_LOOPBK            0x00000010 /* unconditional acceptance of CP SS notif/verif. by pdsm */
#define NV_GNSS_OEM_FEATURE_MASK_6_SUPLINIT_NOTIFY               0x00000020 /* always notify AP of SUPL INIT even if GPS is locked */
#define NV_GNSS_OEM_FEATURE_MASK_7_AUTO_ACCEPT_MTLR_IN_EMERGENCY 0x00000040 /* always accept MTLR during emergency even if GPS is locked */
#define NV_GNSS_OEM_FEATURE_MASK_8_OEMDRE_TUNNEL_DETECT          0x00000080 /* for OEM DRE Tunnel Detection Enablement (used in LM) */
#define NV_GNSS_OEM_FEATURE_MASK_9_KILL_PERIODIC_AT_EMERG_END    0x00000100 /* End ongoing LPP Periodic session at end of emergency state */
#define NV_GNSS_OEM_FEATURE_MASK_10_REQ_XTRA_DURING_SUPL         0x00000200 /* requesting XTRA download during MO MSB SUPL */
#define NV_GNSS_OEM_FEATURE_MASK_11_DISABLE_GF_OEM_CONF          0x00000400 /* Disable GF OEM Conf */
#define NV_GNSS_OEM_FEATURE_MASK_12_REQ_XTRA_DURING_1XUP         0x00000800 /* requesting XTRA download during MO MSB 1x UP (V2 etc) */
#define NV_GNSS_OEM_FEATURE_MASK_13_END_SUPL_AFTER_FINAL_FIX     0x00001000 /* End SUPL session with server if final fix is reported to the AP */
#define NV_GNSS_OEM_FEATURE_MASK_14_SUPL_TDS_LOCID_CFG           0x00002000 /* Configure TD-SCDMA LocationID depending on SUPL server used */


/* SM may define lower max-subs configuration if necessary in order to save NV cache memory (default 3 subs) */
#define SM_CONFIG_MIN_SUB_ID    SYS_MODEM_AS_ID_1
#define SM_CONFIG_MAX_SUB_ID    SYS_MODEM_AS_ID_3
#define SM_CONFIG_MAX_SUBS     (SM_CONFIG_MAX_SUB_ID+1)  /* max number of subscriptons */

/* current Subs ID for carrier-specific legacy/EFS NV, default is 0 (subscription independent)
   This value must NOT be -1, should always be valid 0~1 or 0~2 (even if phone is OOS)
   For run-time value sm_nv_max_sub, if it's single SIM, value should be 0, otherwise 1 or 2 (DSDS/TSTS) */
extern sys_modem_as_id_e_type sm_nv_current_sub, sm_nv_max_sub;


/*
 * Structure definitions
*/

/* Type for SM NV write command callback function
 *
 * A client that wants to be notified of the result of a SM NV write
 * command needs to supply a pointer to a function of this type as the
 * only parameter to the sm_nv_init() function.
 */
typedef void (sm_nv_cmd_cb_f_type) (

  /* NV item that this write command status is for */
  nv_items_enum_type    item,

  /* Pointer to SM NV module provided write data */
  void*                 data,

  /* Command status code indicating write pass/fail */
  nv_stat_enum_type     status
);

typedef struct
{
  q_link_type           link;
  nv_items_enum_type    item;
  nv_item_type          data;
  uint32                handle;
  sys_modem_as_id_e_type sub; /* subscription */
  nv_func_enum_type     cmd;
} sm_nv_item_s_type;

typedef struct
{
  boolean               active;
  uint32                thread_id;
  sm_nv_cmd_cb_f_type*  nv_write_cb_fn;
  sm_nv_cmd_cb_f_type*  nv_read_cb_fn;
} sm_nv_clnt_s_type;


/*
 * Function definitions
*/

/*===========================================================================

FUNCTION sm_nv_rdrw_reg

DESCRIPTION
  This function is used to register a client that will use this NV module.
  It will register the client's callback function after an Async Read/NV Write
  command has completed, and return a client handle that is needed for future
  read/writes.
  
RETURN VALUE
  Client handle value (or 0xFF if error)

DEPENDENCIES
 
===========================================================================*/

uint32 sm_nv_rdrw_reg (uint32 thread_id, sm_nv_cmd_cb_f_type nv_clnt_write_cb_fn,
                       sm_nv_cmd_cb_f_type nv_clnt_read_cb_fn);

/*===========================================================================

FUNCTION sm_nv_write

DESCRIPTION
  This function is used to queue NV Write commands to NV. The writes to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  SM NV write callback needs to be registered via call to sm_nv_write_reg

===========================================================================*/

boolean sm_nv_write (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle);

/*===========================================================================

FUNCTION sm_nv_read

DESCRIPTION
  This function is used to perform a synchronous read from NV.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_read (nv_items_enum_type nv_item, void* item_ptr);

/*===========================================================================

FUNCTION sm_nv_async_read

DESCRIPTION
  This function is used to queue NV Read commands to NV. The Reads to NV
  via this function are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV Async Read command successfully sent to NV
  FALSE - NV Async Read failed to be sent to NV

DEPENDENCIES
  SM NV read callback needs to be registered via call to sm_nv_async_read_reg

===========================================================================*/

boolean sm_nv_async_read (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle);

/*===========================================================================

FUNCTION   sm_nv_rdrw_done

DESCRIPTION
  This function is used to handle NV write/read callback status. If there are any
  clients who issued write/async-read requests, they will be notified and any pending
  async-read/write requests will be subsequently handled.

RETURN VALUE
  TRUE  - NV write/read success
  FALSE - NV write/read failure

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_rdrw_done (void);

/*===========================================================================

FUNCTION sm_nv_read_ex

DESCRIPTION
  This function is used to perform a read from cache. And if cached value not present, read from Hw.
  This function is used to perform a read from cache. And if cached value not present, read from Hw.
  GPS SHALL CALL THIS API INSTEAD OF sm_nv_read AFTER BOOT UP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_read_ex (nv_items_enum_type nv_item, void* item_ptr);

/*===========================================================================

FUNCTION sm_nv_init

DESCRIPTION
  This function is used to initialize internal SM-NV data structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/

void sm_nv_init (void);


/*===========================================================================

FUNCTION sm_nv_set_current_sub_id

DESCRIPTION
  This function is used to set the current default subscription ID (usually
  Dedicated-Data sub). When EFS NV read/write is performed without giving
  specific Subs ID, this current Subs ID will be used.
 
  Note: When DD sub is out of service, EFS NV read/write will continue on last Subs
  ID used (if no Subs ID is specified)
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_set_current_sub_id(sys_modem_as_id_e_type sub);

/*===========================================================================

FUNCTION sm_nv_set_max_sub_id

DESCRIPTION
  This function is used to set the max subscription ID
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_set_max_sub_id(sys_modem_as_id_e_type sub);

/*===========================================================================

FUNCTION sm_nv_write_to_sub 
 
DESCRIPTION
  This function is used to queue NV Write commands to a subscription-specific NV.
  The writes to NV are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV write command successfully sent to NV
  FALSE - NV write command failed to be sent to NV

DEPENDENCIES
  SM NV write callback needs to be registered via call to sm_nv_write_reg

===========================================================================*/

boolean sm_nv_write_to_sub (nv_items_enum_type nv_item, const nv_item_type* item_ptr, 
                            uint32 handle, sys_modem_as_id_e_type sub);

/*===========================================================================

FUNCTION sm_nv_read_from_sub

DESCRIPTION
  This function is used to perform a synchronous read from a subscription-specific NV.
  GPS SHALL CALL THIS ONLY DURING BOOTUP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_read_from_sub (nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub);

/*===========================================================================

FUNCTION sm_nv_read_default

DESCRIPTION
  This function is used to return the default value for the NV item to be
  read. This should be used only when failing to read from NV.

RETURN VALUE
  None

DEPENDENCIES
 
===========================================================================*/

void sm_nv_read_default (nv_items_enum_type nv_item, void *item_ptr);

#endif /* SM_NV_H */
