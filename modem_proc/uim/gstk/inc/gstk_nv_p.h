#ifndef GSTK_NV_P_H
#define GSTK_NV_P_H

/*===========================================================================


                         G S T K _N V_P    H E A D E R

GENERAL DESCRIPTION
  This is the GSTK internal header file to be used by UIM modules.  It
  contains data types and functions that other UIM modules can use to
  access GSTK NV items.

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the
U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/inc/gstk_nv_p.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
09/07/15   gs      Read Default APN NV using GSTK NV API
04/27/15   vr      Non UI and non NW proactive command parallel processing
12/19/14   gm      Added support to read envelope retry data from NV
12/10/14   vr      Disable CC env during SEND/RECEIVE DATA based on NV config
11/24/14   shr     Use gcf_gstk EFS item instead of NV_GPRS_ANITE_GCF_I
10/30/14   bd      Removed feature GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS
10/28/14   gs      Removed GSTK_CFG_FEATURE_SLOT_STOP_SMS_PP_ON_PENDING_TR support
10/21/14   shr     Add GSTK NV support to control IMS bits in TP
10/08/14   bd      Changes to control eHRPD loc info feature with NV
10/07/14   shr     Memory optmization to the GSTK NV logic
10/01/14   bd      Removed feature GSTK_CFG_FEATURE_ME_ESTK_ICON
09/09/14   bd      Features clean up
08/22/14   shr     Disable continous read into BIP Rx buffer by default
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
07/11/14   shr     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gstk_p.h"
#include "estk.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Mask to extract if NV/EFS item is ME based/Slot based or Sub based */
#define GSTK_NV_CLASS_MASK               0xF000

/* Mask to extract the the NV/EFS item's category */
#define GSTK_NV_CATEG_MASK               0x0F00

/* Category of NV/EFS items that are:
   - Read during GSTK initalization
   - Not re-read when a non-standard NV Refresh occurs. Rather
     they are marked for re-read during the next card power-cycle */
#define GSTK_NV_INIT_NO_REREAD_START     0x0

/* Category of NV/EFS items that are:
   - Not read during GSTK initalization but whenever first access
     is requested
   - Not re-read when a non-standard NV Refresh occurs. Rather
     they are marked for re-read during the next card power-cycle */
#define GSTK_NV_ONDEMAND_NO_REREAD_START 0x100

/* Category of NV/EFS items that are:
   - Read during GSTK initalization
   - Re-read when a non-standard NV Refresh occurs */
#define GSTK_NV_INIT_REREAD_START        0x200

/* Category of NV/EFS items that are:
   - Not read during GSTK initalization but whenever first access
     is requested
   - Re-read whenever the first access post a non-standard
     NV Refresh occurs.  */
#define GSTK_NV_ONDEMAND_REREAD_START    0x300

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*===========================================================================

  ENUM: GSTK_NV_ITEMS_ENUM_TYPE

  DESCRIPTION:
    This enum lists the NV/EFS items accessed and cached
    by GSTK which can be acted upon using the GSTK NV APIs:
      1. gstk_nv_access_read  (NV read)
      2. gstk_nv_access_write (NV write)

  NOTE:
    gstk_nv_get_feature_status() GSTK NV API should be used to access
    NV 6564 sub-features.

  STEPS ADD A NEW NV ITEM:
    1. Classify the NV/EFS item to be added based on following criteria:
       a. owned by GSTK or not
       b. is NV/EFS item ME based/Slot based/Subsctiption based
       c. to be read during GSTK init or on-demand
       d. can be re-read during non-standard NV refresh ot not
    2. Create a new enumeration for the NV/EFS item and place it below
       in gstk_nv_items_enum_type based on the classification
    3. Update GSTK NV cache (gstk_nv_cache)
    4. Add logic to access the NV/EFS item in functions:
       a. gstk_nv_access_int_get_item_params
       b. gstk_nv_access_int_write: For NV/EFS write
       c. gstk_nv_access_int_read: For NV/EFS read/init/refresh
    5. Update gstk_nv_items_data_type
    6. For EFS based NV items
       a. Add support for accessing the item in UIM common EFS access module:
          - Enumeration in uim_common_efs_item_type
          - Associated path definition
          - Update to uim_common_efs_get_path_from_enum
      b. If EFS item is owned by GSTK, add support for QCN back-up
         in mmgsdi.conf
      c. If EFS item is owned by GSTK, add support for access via
         QXDM NV browser
    7. Update GSTK NV Wiki with the new NV item info
    8. If its a new EFS/NV item owned by GSTK and is carrier
       specific  - inform MCFG team
==========================================================================*/
  typedef enum {
  /*=============================================
    ME based EFS/NV items owned by GSTK
  =============================================*/
  GSTK_NV_ME_START                               = 0x0,

  /*---------------------------------------------
    ME based EFS/NV items owned by GSTK - read during
    GSTK initalization and not re-read
  ---------------------------------------------*/
  GSTK_NV_ME_CATEG_INIT_NOREREAD_START           = (GSTK_NV_ME_START|GSTK_NV_INIT_NO_REREAD_START),
  /* 0x1 - NV_TOOLKIT_ENV_RETRY_FLAG_I (NV 5593) */
  GSTK_NV_ME_TOOLKIT_ENV_RETRY_FLAG_I,
  /* 0x2 - /nv/item_files/modem/uim/gstk/tp_clients */
  GSTK_NV_ME_TP_CLIENTS,
  /* 0x3 - /nv/item_files/modem/uim/gstk/proactive_feature_enable_cfg (NV 66007) */
  GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG,
  /* 0x4 - /nv/item_files/modem/uim/gstk/tal_conf (NV 65815) */
  GSTK_NV_ME_TAL_CONF,
  /* 0x5 - /nv/item_files/modem/uim/gstk/me_feature_bmsk (NV 73670) */
  GSTK_NV_ME_FEATURE_BMASK,

  /* Category end */
  GSTK_NV_ME_CATEG_INIT_NOREREAD_END,

  /*---------------------------------------------
    ME based EFS/NV items owned by GSTK - read
     on-demand and not re-read
  ---------------------------------------------*/
  GSTK_NV_ME_CATEG_ONDEMAND_NO_RREEAD_START      = (GSTK_NV_ME_START|GSTK_NV_ONDEMAND_NO_REREAD_START),
  /* 0x101 - /nv/item_files/modem/uim/gstk/redial_cfg (NV 66006) */
  GSTK_NV_ME_REDIAL_CFG,
  /* 0x102 - /nv/item_files/modem/uim/gstk/qmi_cat_svc_init */
  GSTK_NV_ME_QMI_CAT_SVC_INIT,

  /* Category end */
  GSTK_NV_ME_CATEG_ONDEMAND_NO_REREAD_END,

  /*=============================================
    ME based EFS/NV items not owned by GSTK
  =============================================*/
  GSTK_NV_EXT_ME_START                           = 0x1000,

  /*---------------------------------------------
    ME based EFS/NV items not owned by GSTK - read
   during GSTK initalization and not re-read
  ---------------------------------------------*/
  GSTK_NV_EXT_ME_CATEG_INIT_NOREREAD_START       = (GSTK_NV_EXT_ME_START|GSTK_NV_INIT_NO_REREAD_START),
  /* 0x1001 - NV_GPRS_ANITE_GCF_I (NV 947) */
  GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I,
  /* 0x1002 - /nv/item_files/modem/qmi/cat/qmi_cat_mode (NV 65683) */
  GSTK_NV_EXT_ME_QMI_CAT_MODE,
  /* 0x1003 - /nv/item_files/modem/uim/mmgsdi/gcf_gstk */
  GSTK_NV_EXT_ME_GCF_GSTK,

  /* Category end */
  GSTK_NV_EXT_ME_CATEG_INIT_NOREREAD_END,

  /*---------------------------------------------
    ME based EFS/NV items not owned by GSTK - read
    on-demand and not re-read
  ---------------------------------------------*/
  GSTK_NV_EXT_ME_CATEG_ONDEMAND_NO_REEAD_START   = (GSTK_NV_EXT_ME_START|GSTK_NV_ONDEMAND_NO_REREAD_START),
  /* 0x1101 - NV_ESN_ME_I (NV 5597) */
  GSTK_NV_EXT_ME_ESN_ME_I,
  /* 0x1102 - NV_MEID_ME_I (NV 5598) */
  GSTK_NV_EXT_ME_MEID_I,
  /* 0x1103 - /nv/item_files/modem/qmi/cat/qmi_cat_default_lang (NV 69729) */
  GSTK_NV_EXT_ME_QMI_CAT_DEFAULT_LANG,

  /* Category end */
  GSTK_NV_EXT_ME_CATEG_ONDEMAND_NO_REEAD_END,

  /*=============================================
    Slot based EFS/NV items owned by GSTK
  =============================================*/
  GSTK_NV_SLOT_START                             = 0x2000,

  /*---------------------------------------------
    Slot based EFS/NV items owned by GSTK - read
    during GSTK initalization and not re-read
    during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SLOT_CATEG_INIT_NOREREAD_START         = (GSTK_NV_SLOT_START|GSTK_NV_INIT_NO_REREAD_START),
  /* 0x2001 - /nv/item_files/modem/uim/gstk/cat_version (NV 67331) */
  GSTK_NV_SLOT_CAT_VERSION,
  /* 0x2002 - /nv/item_files/modem/uim/gstk/feature_bmsk (NV 65674),
     Can be used to read the entire NV for the slot (instead of reading
     individual sub-features using gstk_nv_get_feature_status())
     Note that only slot sub-features will have valid values returned*/
  GSTK_NV_SLOT_FEATURE_BMASK,
  /* 0x2003 - /nv/item_files/modem/uim/gstk/feature_bmsk_2,
     Can be used to read the entire NV for the slot (instead of reading
     individual sub-features using gstk_nv_get_feature_2_status())
     Note that only slot sub-features will have valid values returned*/
  GSTK_NV_SLOT_FEATURE_BMASK_2,

  /* Category end */
  GSTK_NV_SLOT_CATEG_INIT_NOREREAD_END,

  /*---------------------------------------------
    Slot based EFS/NV items owned by GSTK - read
    during on-demand and not re-read during
    non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SLOT_CATEG_ONDEMAND_NO_REEAD_START     = (GSTK_NV_SLOT_START|GSTK_NV_ONDEMAND_NO_REREAD_START),

  /* Category end */
  GSTK_NV_SLOT_CATEG_ONDEMAND_NO_REEAD_END,

  /*---------------------------------------------
    Slot based EFS/NV items owned by GSTK - read
    during GSTK initalization and re-read during
    non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SLOT_CATEG_INIT_REREAD_START           = (GSTK_NV_SLOT_START|GSTK_NV_INIT_REREAD_START),

  /* Category end */
  GSTK_NV_SLOT_CATEG_INIT_REREAD_END,

  /*---------------------------------------------
    Slot based EFS/NV items owned by GSTK - read
    on-demand and allowed re-read after
    non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SLOT_CATEG_ONDEMAND_REEAD_START        = (GSTK_NV_SLOT_START|GSTK_NV_ONDEMAND_REREAD_START),
  /* 0x2301 - NV_TOOLKIT_CS_PS_PARALLEL_I (NV 5770) */
  GSTK_NV_SLOT_TOOLKIT_CS_PS_PARALLEL_I,
  /* 0x2302 - /nv/item_files/modem/uim/gstk/rat_priority_config (NV 72585) */
  GSTK_NV_SLOT_RAT_PRIORITY_CONFIG,
  /* 0x2303 - /nv/item_files/modem/uim/gstk/bip_apn_mapping_tbl (NV 70314) */
  GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE,
  /* 0x2304 - /nv/item_files/modem/uim/gstk/default_apn */
  GSTK_NV_SLOT_DEFAULT_APN,
  /* 0x5 - //nv/item_files/modem/uim/gstk/envelope_retry (NV 73618) */
  GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,

  /* Category end */
  GSTK_NV_SLOT_CATEG_ONDEMAND_REEAD_END,

  /*=============================================
    Slot based EFS/NV items not owned by GSTK
  =============================================*/
  GSTK_NV_EXT_SLOT_START                         = 0x3000,

  /*---------------------------------------------
    Slot based EFS/NV items not owned by GSTK - read
    during GSTK initalization andnot re-read
    during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SLOT_CATEG_INIT_NOREREAD_START     = (GSTK_NV_EXT_SLOT_START|GSTK_NV_INIT_NO_REREAD_START),

  /* Category end */
  GSTK_NV_EXT_SLOT_CATEG_INIT_NOREREAD_END,

  /*---------------------------------------------
    Slot based EFS/NV items not owned by GSTK - read
    during on-demand and not re-read during
    non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SLOT_CATEG_ONDEMAND_NO_REEAD_START = (GSTK_NV_EXT_SLOT_START|GSTK_NV_ONDEMAND_NO_REREAD_START),

  /* Category end */
  GSTK_NV_EXT_SLOT_CATEG_ONDEMAND_NO_REEAD_END,

  /*---------------------------------------------
    Slot based EFS/NV items not owned by GSTK - read
   during GSTK initalization and re-read during
   non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SLOT_CATEG_INIT_REREAD_START       = (GSTK_NV_EXT_SLOT_START|GSTK_NV_INIT_REREAD_START),

  /* Category end */
  GSTK_NV_EXT_SLOT_CATEG_INIT_REREAD_END,

  /*---------------------------------------------
    Slot based EFS/NV items not owned by GSTK - read
   on-demand and allowed re-read after
   non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SLOT_CATEG_ONDEMAND_REEAD_START    = (GSTK_NV_EXT_SLOT_START|GSTK_NV_ONDEMAND_REREAD_START),
  /* 0x3301 - NV_UE_IMEI_I (NV 550) */
  GSTK_NV_EXT_SLOT_UE_IMEI_I,
  /* 0x3302 - NV_UE_IMEISV_SVN_I (NV 5153) */
  GSTK_NV_EXT_SLOT_UE_IMEISVN_I,

  /* Category end */
  GSTK_NV_EXT_SLOT_CATEG_ONDEMAND_REEAD_END,

  /*=============================================
    Subscription based EFS/NV items owned by GSTK
=============================================*/
  GSTK_NV_SUB_START                              = 0x4000,

  /*---------------------------------------------
    Subscription based EFS/NV items owned by GSTK -
    read during GSTK initalization and not re-read
    during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SUB_CATEG_INIT_NOREREAD_START          = (GSTK_NV_SUB_START|GSTK_NV_INIT_NO_REREAD_START),
  /* 0x4001 - /nv/item_files/modem/uim/gstk/bip_otasp_concurrency (NV 70212) */
  GSTK_NV_SUB_BIP_OTASP_CONCURRENCY,
  /* 0x4002 - /nv/item_files/modem/uim/gstk/attach_ctrl_timer (NV 70299) */
  GSTK_NV_SUB_ATTACH_CTRL_TIMER,

  /* Category end */
  GSTK_NV_SUB_CATEG_INIT_NOREREAD_END,

  /*---------------------------------------------
    Subscription based EFS/NV items owned by GSTK -
    read during on-demand and not re-read during
    non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SUB_CATEG_ONDEMAND_NO_REEAD_START      = (GSTK_NV_SUB_START|GSTK_NV_ONDEMAND_NO_REREAD_START),

  /* Category end */
  GSTK_NV_SUB_CATEG_ONDEMAND_NO_REEAD_END,

  /*---------------------------------------------
    Subscription based EFS/NV items owned by GSTK -
    read during GSTK initalization and re-read
    during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SUB_CATEG_INIT_REREAD_START            = (GSTK_NV_SUB_START|GSTK_NV_INIT_REREAD_START),

  /* Category end */
  GSTK_NV_SUB_CATEG_INIT_REREAD_END,

  /*---------------------------------------------
    Subscription based EFS/NV items owned by GSTK -
    read on-demand and allowed re-read after
    non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_SUB_CATEG_ONDEMAND_REEAD_START         = (GSTK_NV_SUB_START|GSTK_NV_ONDEMAND_REREAD_START),

  /* Category end */
  GSTK_NV_SUB_CATEG_ONDEMAND_REEAD_END,

  /*=============================================
    Subscription based EFS/NV items not owned by GSTK
  =============================================*/
  GSTK_NV_EXT_SUB_START                          = 0x5000,


  /*---------------------------------------------
    Subscription based EFS/NV items not owned by
    GSTK - read during GSTK initalization and
    not re-read during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SUB_CATEG_INIT_NOREREAD_START      = (GSTK_NV_EXT_SUB_START|GSTK_NV_INIT_NO_REREAD_START),

  /* Category end */
  GSTK_NV_EXT_SUB_CATEG_INIT_NOREREAD_END,

  /*---------------------------------------------
    Subscription based EFS/NV items not owned by
    GSTK - read during on-demand and
    not re-read during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SUB_CATEG_ONDEMAND_NO_REEAD_START  = (GSTK_NV_EXT_SUB_START|GSTK_NV_ONDEMAND_NO_REREAD_START),

  /* Category end */
  GSTK_NV_EXT_SUB_CATEG_ONDEMAND_NO_REEAD_END,

  /*---------------------------------------------
    Subscription based EFS/NV items not owned by
    GSTK - read during GSTK initalization and
    re-read during non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SUB_CATEG_INIT_REREAD_START        = (GSTK_NV_EXT_SUB_START|GSTK_NV_INIT_REREAD_START),

  /* Category end */
  GSTK_NV_EXT_SUB_CATEG_INIT_REREAD_END,

  /*---------------------------------------------
    Subscription based EFS/NV items not owned by
    GSTK - read on-demand and allowed re-read
    after non-standard NV refresh
  ---------------------------------------------*/
  GSTK_NV_EXT_SUB_CATEG_ONDEMAND_REEAD_START     = (GSTK_NV_EXT_SUB_START|GSTK_NV_ONDEMAND_REREAD_START),
  /* 0x5301 - NV_DTM_FEATURE_SUPPORT_I (NV 3628) */
  GSTK_NV_EXT_SUB_DTM_FEATURE_SUPPORT_I,

  /* Category end */
  GSTK_NV_EXT_SUB_CATEG_ONDEMAND_REEAD_END,

  /*=============================================
    NV/EFS items end - Force an initialize size enumeration
  =============================================*/
  GSTK_NV_MAX = 0xFFFFFFFF
  }
gstk_nv_items_enum_type;

/*===========================================================================

  ENUM: GSTK_NV_CFG_FEATURES_ENUM_TYPE

  DESCRIPTION:
    This enum lists all the sub-features within
    /nv/item_files/modem/uim/gstk/feature_bmsk (NV 65674) whose values
    can be retrieved using the GSTK NV API: gstk_nv_get_feature_status

  STEPS ADD A NEW SUB-FEATURE:
    1. Classify the sub-feature as ME specific or slot specific
    2. Create a new enumeration for the sub-feature and place it below
       in gstk_nv_cfg_features_enum_type based on the classification
       Note: Always define feature such that the default value is disabled
    3. Add logic to access the NV item in function gstk_nv_get_feature_status
    4. Update GSTK NV Wiki with the new NV item info
==========================================================================*/
typedef enum {
  /* When enabled, turns-on OTASP IMSI_M enhancement */
  GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH                         = 0x00000001,
  /* Removed, DO NOT  reuse the mask */
  /* GSTK_CFG_FEATURE_ME_ESTK_QMI                             = 0x00000002,*/
  /* Removed, DO NOT  reuse the mask */
  /* GSTK_CFG_FEATURE_ME_ESTK_ICON                            = 0x00000004,*/
  /* Should be enabled for all data card targets */
  GSTK_CFG_FEATURE_ME_DATA_CARD                            = 0x00000008,
  /* When enabled, enables VZW proprietary location status envelope cmd */
  GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS                    = 0x00000010,
  /* When enabled, disables the GSTK response timer */
  GSTK_CFG_FEATURE_ME_GSTK_CUST_ENH0                       = 0x00000020,
  /* Removed, DO NOT  reuse the mask */
  /* GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS                   = 0x00000040,*/
  /* When enabled, will set pdn_type to v4 in the DS profile if the open
     channel has IPV4 address */
  GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT                = 0x00000080,
  /* When enabled, will ignore the alpha for ESTK handled commands
     (e.g Open channel, send data, receive data, close channel, set up call,
     send ussd, send sms, send ss) */
  GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA                    = 0x00000100,
  /* When enabled, will enable GSTK to generate Access Technology envelope and
     Location Status envelope when UE receives the first SETUP EVENT LIST
     command of each terminal profile */
  GSTK_CFG_FEATURE_SLOT_ENH_RAT_AND_LOC_EVT                = 0x00000200,
  /* When enabled, will prevent GSTK from writing a tp_clients file with
     smaller amount of clients than it already has */
  GSTK_CFG_FEATURE_ME_DYNAMIC_TP_CLIENTS                   = 0x00000400,
  /* When enabled, will make GSTK report the status words for terminal response
     and envelope commands as received from card, to the clients */
  GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2                       = 0x00000800,
  /* When enabled, will turn-on the supported IMS related bits in
     terminal profile */
  GSTK_CFG_FEATURE_ME_TP_IMS_ENABLE                        = 0x00001000,
  /* Enables SETUP CALL Cat Rel-7 features */
  GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7                    = 0x00002000,
  /* When enabled, will make sure the additional information byte is set when
     the result byte for terminal response is 0x20 and no additional
     information byte is present */
  GSTK_CFG_FEATURE_SLOT_TR_ADD_INFO_BYTE                   = 0x00004000,
  /* When enabled, will takes the text in ascii format and use the DCS value
     provided by client to do the conversion when packing the TR to the card */
  GSTK_CFG_FEATURE_ME_TRANSCODING_ENABLE                   = 0x00008000,
  /* When enabled will allow BIP over WLAN bearer */
  GSTK_CFG_FEATURE_SLOT_ESTK_BIP_OVER_WLAN                 = 0x00010000,
  /* When enabled, will prioritize BIP on WLAN bearer over
     Cellular (when both are available and BIP over WLAN is allowed) */
  GSTK_CFG_FEATURE_SLOT_ESTK_BIP_WLAN_PRIO                 = 0x00020000,
  /* When enabled will set 25th bit of TP to 0 */
  GSTK_CFG_FEATURE_SLOT_CUST_TP_25_BYTE                    = 0x00040000,
  /* When enabled, if there is pending proactive command to be processed, then
     the envelope requested is rejected with SIM Busy */
  GSTK_CFG_FEATURE_SLOT_STOP_ENV_CMD_PROCESSING            = 0x00080000,
  /* Removed, DO NOT  reuse the mask */
  /* GSTK_CFG_FEATURE_SLOT_STOP_SMS_PP_ON_PENDING_TR       = 0x00100000, */
  /* When enabled, disables the sending of Voice Call and DTMF
     capabilities to Card in TP */
  GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE = 0x00200000,
  /* When enabled, disables the sending of SS and USSD capabilities to Card
     in TP, and also disables ESTK to handle the SS and USSD
     request from the card */
  GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE      = 0x00400000,
  /* When enabled, disables the sending of SMS capabilities to Card in TP, and
     also disables ESTK to handle the SMS request from the card */
  GSTK_CFG_FEATURE_ME_DISABLE_ESTK_WMS_INTERFACE           = 0x00800000,
  /* When enabled, disables the sending of BIP capabilities to Card in TP */
  GSTK_CFG_FEATURE_ME_DISABLE_ESTK_BIP_INTERFACE           = 0x01000000,
  /* Removed, DO NOT  reuse the mask */
  /* GSTK_CFG_FEATURE_ME_ESTK_QMI_HOLD_NO_AND_NULL_ALPHA_IND  = 0x02000000,*/
  /* When enabled, enables continuous read of data from a TCP socket
     as part of a BIP session */
  GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ      = 0x04000000,
  /* When enabled, disables call control envelope retry */
  GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY    = 0x08000000,
  /* When enabled, limited service MM Idles are only sent to card if a
     NW reject was received for the same MCC, MNC, and LAC/TAC.
     This feature is meant for SINGLE RAT targets ONLY (non-SGLTE etc.) */
  GSTK_CFG_FEATURE_SLOT_LTD_SVC_WITH_NW_REJ_ONLY           = 0X10000000,
  /* When enabled, STK app will switch between STK/UTK in case there is
     mode change between 3GPP/3GPP2 radio access technology */
  GSTK_CFG_FEATURE_SLOT_MODE_SWITCH_STK_SWITCH             = 0x20000000,
  /* When enabled, Alpha indications for No Alpha and Null Alpha
     will be sent to display client */
  GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND  = 0x40000000,
  /* When enabled, (e)HRPD location information can be given to card */
  GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO                     = 0x80000000,
  /* To guarantee int32 */
  GSTK_CFG_FEATURE_MAX                                     = 0x7FFFFFFF
}
gstk_nv_cfg_features_enum_type;

/*===========================================================================

  ENUM: GSTK_NV_CFG_FEATURES_2_ENUM_TYPE

  DESCRIPTION:
    This enum lists all the sub-features within
    /nv/item_files/modem/uim/gstk/feature_bmsk_2 whose values
    can be retrieved using the GSTK NV API: gstk_nv_get_feature_2_status

  STEPS ADD A NEW SUB-FEATURE:
    1. Classify the sub-feature as ME specific or slot specific
    2. Create a new enumeration for the sub-feature and place it below
       in gstk_nv_cfg_features_2_enum_type based on the classification
       Note: Always define feature such that the default value is disabled
    3. Add logic to access the NV item in function gstk_nv_get_feature_2_status
    4. Update GSTK NV Wiki with the new NV item info
==========================================================================*/
typedef enum {
  /* When enabled, Blocks CC envelope during SEND and receive data */
  GSTK_CFG_FEATURE_2_SLOT_BLOCK_CALL_CTRL_DURING_SEND_RECEIVE_DATA = (SYS_BM_64BIT(0)),
  /* To guarantee int64 */
  GSTK_CFG_FEATURE_2_MAX                                           = 0x7FFFFFFFFFFFFFFF
}
gstk_nv_cfg_features_2_enum_type;

/*===========================================================================

  ENUM: GSTK_NV_CFG_ME_FEATURES_ENUM_TYPE

  DESCRIPTION:
    This enum lists all the sub-features within
    /nv/item_files/modem/uim/gstk/me_feature_bmsk (NV 73670) whose values
    can be retrieved using the GSTK NV API: gstk_nv_get_me_feature_status

  STEPS ADD A NEW SUB-FEATURE:
    1. Create a new enumeration for the sub-feature and place it below
       in gstk_nv_cfg_me_features_enum_type based on the classification
       Note: Always define feature such that the default value is disabled
    2. Add logic to access the NV item in function gstk_nv_get_me_feature_status
    3. Update GSTK NV Wiki with the new NV item info
==========================================================================*/
typedef enum {
  /* When enabled, NON UI and NON NW Proactive commands parallel processing will be disabled */
  GSTK_CFG_FEATURE_ME_DISABLE_PCMD_SELECTIVE_PARALLEL_PROCESSING        = (SYS_BM_64BIT(0)),
  /* When enabled, will not allow other slot proactive session, if GSTK delays the TR because of 
     continues duplicate proactive command and error TR */
  GSTK_CFG_FEATURE_ME_DISABLE_DUP_ERR_TR_DELAY_PCMD_PARALLEL_PROCESSING = (SYS_BM_64BIT(1)),
  /* When enabled, will log all accesses to GSTK NV items. When disabled,
     GSTK NV reads from the GSTK NV cache are not logged */
  GSTK_CFG_FEATURE_ME_ENABLE_FULL_NV_LOGGING                            = (SYS_BM_64BIT(14)),  
  /* To guarantee int64 */
  GSTK_CFG_ME_FEATURE_MAX                                           = 0x7FFFFFFFFFFFFFFF
}
gstk_nv_cfg_me_features_enum_type;

/*===========================================================================

  ENUM: GSTK_NV_PROACTIVE_CMD_ENABLE_ENUM_TYPE

  DESCRIPTION:
    This enum lists all the sub-features within
    /nv/item_files/modem/uim/gstk/proactive_feature_enable_cfg (NV 66007) whose values
    can be retrieved using the GSTK NV API: gstk_nv_get_proactive_feature_enable
==========================================================================*/
typedef enum {
  GSTK_FEATURE_ENABLE_EVT_LST_STATUS      = 0x00000001,
  GSTK_FEATURE_ENABLE_NET_LOC             = 0x00000002,
  GSTK_FEATURE_ENABLE_QUERY_SIM_ORIG_CALL = 0x00000004,
  GSTK_FEATURE_ENABLE_BIP                 = 0x00000008,
  GSTK_FEATURE_ENABLE_SIM_CALL_CTRL       = 0x00000010,
  GSTK_FEATURE_ENABLE_LAUNCH_BROWSER      = 0x00000020,
  GSTK_FEATURE_ENABLE_SETUP_CALL_SUBADDR  = 0x00000040,
  GSTK_FEATURE_ENABLE_UICC_SERVER_MODE    = 0x00000080,
  GSTK_FEATURE_ENABLE_MAX                 = 0x7FFFFFFF
}
gstk_nv_proactive_cmd_enable_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*===========================================================================

  STRUCT: GSTK_APN_MAPPING_TYPE

  PARAMETERS:
    input_apn    : Input APN string
    output_apn   : Output APN string

  DESCRIPTION:
    The structure is used to hold one entry of the BIP APN mapping
    table
===========================================================================*/
typedef struct
{
  uint8                            version;
  char                             input_apn[ESTK_APN_MAX_LENGTH + 1];
  char                             output_apn[ESTK_APN_MAX_LENGTH + 1];
}
gstk_apn_mapping_type;

/*===========================================================================

  STRUCT: GSTK_APN_MAPPING_TABLE_DATA_TYPE

  DESCRIPTION:
    The structure is used to cache the BIP APN mapping
    table (read from GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE)
===========================================================================*/
typedef struct
{
  gstk_apn_mapping_type value[ESTK_APN_MAPPING_MAX];
}
gstk_apn_mapping_table_data_type;

/*===========================================================================

  STRUCT: GSTK_APN_MAPPING_TYPE

  PARAMETERS:
    apn : Default APN string

  DESCRIPTION:
    The structure is used to hold the default APN string
===========================================================================*/
typedef struct
{
  uint8 apn[ESTK_APN_MAX_LENGTH + 2];
}
gstk_default_apn_data_type;

/*===========================================================================

  STRUCT: GSTK_ENV_RETRY_EXT_DATA_TYPE

  PARAMETERS:
    max_retries : Maximum number of envelope retries
    duration    : Time duration betweek two retries

  DESCRIPTION:
    The structure is used to hold the retry data
===========================================================================*/
typedef PACKED struct PACKED_POST
{
  uint32  duration;    /*Duration of timer to wait in between retries */
  uint8   max_retries; /* max number of retries to attempt */  
}
gstk_env_retry_ext_data_type;

/*===========================================================================
  
  UNION: GSTK_NV_ITEMS_DATA_TYPE

  DESCRIPTION:
    The union is used to access GSTK NV/EFS cache data.

    When an NV item is read via the gstk_nv_access_read API,
    the value can retrieved from the union item corresponding to
    the NV item.

    When an NV item is written into via the gstk_nv_access_write API,
    the value is passed in bt populating the union item corresponding to
    the NV item.

   NOTE:
     When reading GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE/
     GSTK_NV_SLOT_DEFAULT_APN, caller is expected to allocate memory for the
     corresponding union element:
     apn_mapping_table_ptr/default_apn_ptr before invoking the GSTK NV read API
==========================================================================*/
typedef union
{
  /* GSTK_NV_ME_TOOLKIT_ENV_RETRY_FLAG_I */
  boolean                           env_retry;
  /* GSTK_NV_ME_TP_CLIENTS */
  uint32                            tp_clients;
  /* GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG */
  uint32                            proact_feat_enable;
#ifdef FEATURE_GSTK_TAL
  /* GSTK_NV_ME_TAL_CONF */
  uint32                            tal_conf[GSTK_TAL_MAX_CMD_CONFIG];
#endif /* FEATURE_GSTK_TAL */
  /* GSTK_NV_ME_REDIAL_CFG */
  uint32                            redial_cfg;
  /* GSTK_NV_ME_QMI_CAT_SVC_INIT */
  boolean                           qmi_cat_svc_init;
  /* GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I */
  boolean                           gprs_anite_gcf;
  /* GSTK_NV_EXT_ME_QMI_CAT_MODE */
  uint8                             qmi_cat_mode;
  /* GSTK_NV_EXT_ME_GCF_GSTK */
  boolean                           gcf_gstk;
  /* GSTK_NV_EXT_ME_ESN_ME_I */
  uint8                             esn_me[GSTK_ESN_LEN];
  /* GSTK_NV_EXT_ME_MEID_I */
  uint8                             meid[GSTK_MEID_LEN];
  /* GSTK_NV_EXT_ME_MEID_I */
  uint8                             qmi_cat_default_lang[2];
  /* GSTK_NV_SLOT_CAT_VERSION */
  uint8                             cat_version;
  /* GSTK_NV_SLOT_FEATURE_BMASK */
  uint32                            feature_bmask;
  /* GSTK_NV_SLOT_FEATURE_BMASK_2 */
  uint64                            feature_bmask_2;
  /* GSTK_NV_SLOT_TOOLKIT_CS_PS_PARALLEL_I */
  boolean                           cs_ps_parallel;
  /* GSTK_NV_SLOT_RAT_PRIORITY_CONFIG */
  gstk_rat_enum_type                rat_prio[GSTK_MAX_NUM_RATS];
  /* GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE */
  gstk_apn_mapping_table_data_type *apn_mapping_table_ptr;
  /* GSTK_NV_SLOT_DEFAULT_APN */
  gstk_default_apn_data_type       *default_apn_ptr;
  /* GSTK_NV_EXT_SLOT_UE_IMEI_I */
  uint8                             ue_imei[GSTK_IMEI_LEN];
  /* GSTK_NV_EXT_SLOT_UE_IMEISVN_I */
  uint8                             ue_imeisvn[GSTK_IMEI_SV_LEN];
  /* GSTK_NV_SUB_BIP_OTASP_CONCURRENCY */
  boolean                           bip_otasp_concurrency;
  /* GSTK_NV_SUB_ATTACH_CTRL_TIMER */
  uint32                            attach_ctrl_timer_val;
  /* GSTK_NV_EXT_SUB_DTM_FEATURE_SUPPORT_I */
  boolean                           dtm_feat_support;
  /* GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA */
  gstk_env_retry_ext_data_type      env_retry_ext;
  /* GSTK_NV_ME_FEATURE_BMASK */
  uint64                            me_spec_feature_bmask;
}
gstk_nv_items_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION gstk_nv_get_proactive_feature_enable

DESCRIPTION
  This function returns if the proactive feature is enabled or not

PARAMETERS
  proactive_cmd:       [Input]  Proactive cmd to check
  proactive_cmd_enable [Output] boolean to determine if proactive cmd
                                is enabled or not

RETURN VALUE
  gstk_status_enum_type

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_nv_get_proactive_feature_enable(
  gstk_nv_proactive_cmd_enable_enum_type  proactive_cmd,
  boolean                                *proactive_cmd_enable_ptr
);

/*===========================================================================
FUNCTION GSTK_NV_GET_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of one of the GSTK features
  listed in gstk_nv_cfg_features_enum_type

PARAMETERS:
  mask_value: [Input] Single sub-feature to be read
  slot_id:    [Input] Slot ID on which initalization is requested

RETURN VALUE
  boolean

DEPENDENCIES
  None

COMMENTS
  For ME specific NV items, slot ID parameter does not apply

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_nv_get_feature_status(
  gstk_nv_cfg_features_enum_type mask,
  gstk_slot_id_enum_type         slot_id
);

/*===========================================================================
FUNCTION GSTK_NV_GET_FEATURE_2_STATUS

DESCRIPTION
  Function used to get the status of one of the GSTK features bmask 2
  listed in gstk_nv_cfg_features_enum_type

PARAMETERS:
  mask_value: [Input] Single sub-feature to be read
  slot_id:    [Input] Slot ID on which initalization is requested

RETURN VALUE
  boolean

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_nv_get_feature_2_status(
  gstk_nv_cfg_features_2_enum_type mask,
  gstk_slot_id_enum_type         slot_id
);

/*===========================================================================
FUNCTION GSTK_NV_GET_ME_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of one of the GSTK ME specific features 
  bmask listed in gstk_nv_cfg_features_enum_type

PARAMETERS:
  mask_value: [Input] Single sub-feature to be read

RETURN VALUE
  boolean

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_nv_get_me_feature_status(
  gstk_nv_cfg_me_features_enum_type mask
);

/*===========================================================================
FUNCTION gstk_nv_access_read

DESCRIPTION
  This function reads the NV/EFS item requested from NV.
  If item is already cached by GSTK, value is reutned from cache.

PARAMETERS:
  item_type:[Input]  NV/EFS item to be read
  op_categ: [Input]  Category of NV access
  slot_id:  [Input]  Slot ID on which access is requested
  sub_id:   [Input]  Subscription ID on which access is requested
  data_ptr: [Output] Data buffer with the data read from NV/EFS

RETURN VALUE
  gstk_status_enum_type

DEPENDENCIES
   When reading GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE, caller is expected
   to allocate memory for the corresponding union element:
   apn_mapping_table_ptr before invoking the GSTK NV read API

   When reading GSTK_NV_SLOT_DEFAULT_APN, caller is expected
   to allocate memory for the corresponding union element:
   default_apn_ptr before invoking the GSTK NV read API

COMMENTS
  For ME specific NV items, slot ID parameter does not apply

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_nv_access_read(
  gstk_nv_items_enum_type      item_type,
  gstk_slot_id_enum_type       slot_id,
  gstk_nv_items_data_type     *data_ptr
);

/*===========================================================================
FUNCTION gstk_nv_access_write

DESCRIPTION
  This function writes the data provided corresponding to the NV/EFS
  item requested into the NV.
  The GSTK NV cache is also updated with the new value.

PARAMETERS:
  item_type:[Input] NV/EFS item to be write
  op_categ: [Input] Category of NV access
  slot_id:  [Input] Slot ID on which access is requested
  sub_id:   [Input] Subscription ID on which access is requested
  data_ptr: [Input] Data buffer

RETURN VALUE
  gstk_status_enum_type

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_nv_access_write(
  gstk_nv_items_enum_type      item_type,
  gstk_slot_id_enum_type       slot_id,
  gstk_nv_items_data_type     *data_ptr
);

#endif /* GSTK_NV_P_H */
