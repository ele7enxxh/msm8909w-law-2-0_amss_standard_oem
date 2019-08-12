/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 G S T K   N V  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to interact with NV and EFS.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_nv.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      Make NV cache read print less lines
05/06/16   gs      F3 logging improvements
02/03/16   gm      Fix incorect typecasting of NV index
02/02/16   vdc     Migrated to new MCFG Refresh Done API
09/07/15   gs      Read Default APN NV using GSTK NV API
07/13/15   gm      Add support for updated UIM_COMMON_EFS_DEVICE value
04/27/15   vr      Non UI and non NW proactive command parallel processing
01/14/15   vr      Enable GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND for GCF
01/13/15   vr      Enable GSTK_CFG_FEATURE_SETUP_CALL_REL7 if cat version REL7 and above
12/19/14   gm      Added support to read envelope retry data from NV
12/10/14   vr      Disable CC env during SEND/RECEIVE DATA based on NV config
11/24/14   shr     Use gcf_gstk EFS item instead of NV_GPRS_ANITE_GCF_I
11/11/14   shr     Fixed compilation warnings
10/30/14   bd      Removed feature GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS
10/28/14   gs      Removed GSTK_CFG_FEATURE_SLOT_STOP_SMS_PP_ON_PENDING_TR support
10/16/14   shr     Add GSTK NV support to control IMS bits in TP
10/14/14   tl      Fix compiler warnings
10/08/14   shr     Do not cache IMEI and IMEI_SVN
10/08/14   bd      Changes to control eHRPD loc info feature with NV
10/08/14   shr     Fixed KW error
10/07/14   shr     Memory optmization to the GSTK NV logic
10/01/14   bd      Removed feature GSTK_CFG_FEATURE_ME_ESTK_ICON
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/09/14   vr      Reduce stack usage
09/09/14   bd      Features clean up
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/22/14   shr     Disable continous read into BIP Rx buffer by default
08/15/14   shr     Always invoke MCFG Refresh done API
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
08/06/14   dy      Fixing KW errors
08/05/14   bd      Enable GSTK_CFG_FEATURE_ESTK_BIP_IP_DEFAULT when GCF NV is set
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
07/29/14   shr     Fixed compiler warning
07/16/14   shr     Fix IMEI_SVN read
07/11/14   shr     Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "intconv.h"
#include "customer.h"
#include "err.h"
#include "queue.h"
#include "dsm.h"
#include "memheap.h"
#include "mcfg_refresh.h"
#include "nv.h"
#include "uim_msg.h"
#include "uim_common_efs.h"
#include "gstk.h"
#include "gstk_tal.h"
#include "gstk_nv.h"
#include "gstkutil.h"
#include "qmi_cat.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/
#define GSTK_NV_RAT_PRIO_CONFIG_LEN      50

/*=============================================================================

                   ENUMERATED DATA DECLARATIONS

=============================================================================*/

/*===========================================================================

  ENUM: GSTK_NV_OP_CATEG_ENUM_TYPE

  DESCRIPTION:
    This enum describes the various operations that can be requested
    on GSTK NV items
==========================================================================*/
typedef enum {
  GSTK_NV_OP_CATEG_INIT = 0x0,
    /* GSTK NV initalization request due to task/card power-up */
  GSTK_NV_OP_CATEG_NV_REFRESH,
    /* NV Refresh request from MCFG */
  GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD,
    /* NV Refresh request from MCFG for a non-standard card */
  GSTK_NV_OP_CATEG_ACCESS_READ,
    /* NV/EFS Read request */
  GSTK_NV_OP_CATEG_ACCESS_WRITE
    /* NV/EFS Write request */
}
gstk_nv_op_categ_enum_type;

/*===========================================================================

  ENUM: GSTK_NV_CACHE_STATE_ENUM_TYPE

  DESCRIPTION:
    This enum describes all the states that a cached NV/EFS item can have
==========================================================================*/
typedef enum {
  GSTK_NV_STATE_NOT_INIT = 0x0,
    /* NV /EFS item is not yet read */
  GSTK_NV_STATE_INIT,
    /* NV/EFS item is successfully read and value cached */
  GSTK_NV_STATE_NOT_FOUND,
    /* NV/EFS item is unsuccesfully read - either default value
       is cached (in which case subsequent NV item reads shall suceeed,
       with default value being returned from cache) or cache is
       invalid (in which case subsequent NV item reads shall
       return error) */
  GSTK_NV_STATE_CHANGED
    /* NV/EFS item is changed in the NV due to a non-standard
       NV Refresh, but NV item cannot be re-read in current card
       power-cycle. NV item will be read during next card power-cycle */
}
gstk_nv_cache_state_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*===========================================================================
  STRUCT: GSTK_NV_APN_MAPPING_TYPE
==========================================================================*/
typedef struct
{
  uint8                            version;
  uint8                            input_apn[ESTK_APN_MAX_LENGTH];
  uint8                            output_apn[ESTK_APN_MAX_LENGTH];
}
gstk_nv_apn_mapping_type;

/*===========================================================================
  STRUCT: GSTK_NV_ENV_RETRY_EXT_DATA_TYPE
==========================================================================*/
typedef struct
{
  gstk_env_retry_ext_data_type  value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_env_retry_ext_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_ENV_RETRY_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_env_retry_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_TP_CLIENTS_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint32                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_tp_clients_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_FEAT_ENABLE_PROACT_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint32                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_proact_feat_enable_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_TAL_CONF_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint32                        value[GSTK_TAL_MAX_CMD_CONFIG];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_tal_conf_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_REDIAL_CFG_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint32                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_redial_cfg_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_QMI_CAT_SVC_INIT_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_qmi_cat_svc_init_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_GPRS_ANITE_GCF_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_gprs_anite_gcf_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_QMI_CAT_MODE_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_qmi_cat_mode_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_GCF_GSTK_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_gcf_gstk_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_ESN_ME_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value[GSTK_ESN_LEN];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_esn_me_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_MEID_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value[GSTK_MEID_LEN];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_meid_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_QMI_CAT_DEFAULT_LANG_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value[2];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_qmi_cat_default_lang_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_CAT_VERSION_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_cat_version_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_CS_PS_PARALLEL_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_cs_ps_parallel_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_RAT_PRIO_DATA_TYPE
==========================================================================*/
typedef struct
{
  char                          value[GSTK_NV_RAT_PRIO_CONFIG_LEN + 1];
  gstk_rat_enum_type            prio_list[GSTK_MAX_NUM_RATS];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_rat_prio_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_BIP_APN_MAPPING_TBL_DATA_TYPE
==========================================================================*/
typedef struct
{
  gstk_nv_apn_mapping_type      value[ESTK_APN_MAPPING_MAX];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_bip_apn_mapping_tbl_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_DEFAULT_APN_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value[ESTK_APN_MAX_LENGTH + 2];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_default_apn_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_UE_IMEI_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value[GSTK_IMEI_LEN];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_ue_imei_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_UE_IMEISVN_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint8                         value[GSTK_IMEI_SV_LEN];
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_ue_imeisvn_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_BIP_OTASP_CONCURRENCY_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_bip_otasp_concurrency_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_ATTACH_CTRL_TIMER_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint32                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_attach_ctrl_timer_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_DTM_FEAT_SUPPORT_DATA_TYPE
==========================================================================*/
typedef struct
{
  boolean                       value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_dtm_feat_support_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_FEATURE_BMASK_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint32                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_feature_bmask_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_FEATURE_BMASK_2_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint64                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_feature_bmask_2_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_ME_FEATURE_BMASK_DATA_TYPE
==========================================================================*/
typedef struct
{
  uint64                        value;
  gstk_nv_cache_state_enum_type state;
}
gstk_nv_me_feature_bmask_data_type;

/*===========================================================================
  STRUCT: GSTK_NV_PARAMS_TYPE
==========================================================================*/
typedef struct
{
  uint32                         item_num;
    /* NV /EFS item number */
  boolean                        is_item_nv;
    /* Is the item in NV? */
  uint8                          cache_index;
    /* Index within the Slot/Sub cache corresponding to the current item */
  int8                           nv_index;
    /* Index in NV/EFS w.r.t. the current Slot/Sub item */
  gstk_nv_cache_state_enum_type *cache_state_ptr;
    /* Pointer to the state of the item in the NV cache */
  void                          *cache_data_ptr;
    /*Pointer to the location where the NV/EFS item is cached in NV cache */
  uint32                         cache_data_len;
    /* Size of the NV item in the NV cache */
  boolean                        is_item_read_from_fs;
    /* Is the NV item attempted to be read from the File System
       (i.e. not the GSTK NV cache) */    
}gstk_nv_params_type;

/*===========================================================================

  STRUCT: GSTK_NV_ME_CACHE_DATA_TYPE

  DESCRIPTION:
    The structure is to hold GSTK NV/EFS cache data for ME
    based NV items
==========================================================================*/
typedef struct
{
  gstk_nv_env_retry_data_type            env_retry;
  gstk_nv_tp_clients_data_type           tp_clients;
  gstk_nv_proact_feat_enable_data_type   proact_feat_enable;
#ifdef FEATURE_GSTK_TAL
  gstk_nv_tal_conf_data_type             tal_conf;
#endif /* FEATURE_GSTK_TAL */
  gstk_nv_redial_cfg_data_type           redial_cfg;
  gstk_nv_qmi_cat_svc_init_data_type     qmi_cat_svc_init;
  gstk_nv_gprs_anite_gcf_data_type       gprs_anite_gcf;
  gstk_nv_qmi_cat_mode_data_type         qmi_cat_mode;
  gstk_nv_gcf_gstk_data_type             gcf_gstk;
  gstk_nv_esn_me_data_type               esn_me;
  gstk_nv_meid_data_type                 meid;
  gstk_nv_qmi_cat_default_lang_data_type qmi_cat_default_lang;
  gstk_nv_feature_bmask_data_type        me_feature_bmask;
  gstk_nv_me_feature_bmask_data_type     me_spec_feature_bmask;
}
gstk_nv_me_cache_data_type;

/*===========================================================================

  STRUCT: GSTK_NV_SLOT_CACHE_DATA_TYPE

  DESCRIPTION:
    The structure is to hold GSTK NV/EFS cache data for slot
    based NV items
==========================================================================*/
typedef struct
{
  gstk_nv_cat_version_data_type          cat_version;
  gstk_nv_cs_ps_parallel_data_type       cs_ps_parallel;
  gstk_nv_rat_prio_data_type             rat_prio;
  gstk_nv_bip_apn_mapping_tbl_data_type *bip_apn_mapping_tbl_ptr;
  gstk_nv_default_apn_data_type          default_apn;
  gstk_nv_ue_imei_data_type              ue_imei;
  gstk_nv_ue_imeisvn_data_type           ue_imeisvn;
  gstk_nv_feature_bmask_data_type        feature_bmask;
  gstk_nv_feature_bmask_2_data_type      feature_bmask_2;
  gstk_nv_env_retry_ext_data_type        env_retry_ext;
}
gstk_nv_slot_cache_data_type;

/*===========================================================================

  STRUCT: GSTK_NV_SUB_CACHE_DATA_TYPE

  DESCRIPTION:
    The structure is to hold GSTK NV/EFS cache data for subscription
    based NV items
==========================================================================*/
typedef struct
{
  gstk_nv_bip_otasp_concurrency_data_type bip_otasp_concurrency;
  gstk_nv_attach_ctrl_timer_data_type     attach_ctrl_timer;
  gstk_nv_dtm_feat_support_data_type      dtm_feat_support;
}
gstk_nv_sub_cache_data_type;

/*===========================================================================

  STRUCT: GSTK_NV_CACHE_DATA_TYPE

  DESCRIPTION:
    The structure is to hold GSTK NV/EFS cache data

  NOTE:
    Memory for the slot and sub caches are allocated at run-time from
    the heap, based on number of slots supported by the device
==========================================================================*/
typedef struct
{
 gstk_nv_me_cache_data_type    me_items;
 gstk_nv_slot_cache_data_type *slot_items_ptr[GSTK_MAX_NUM_OF_INSTANCES];
 gstk_nv_sub_cache_data_type  *sub_items_ptr[GSTK_MAX_NUM_OF_INSTANCES];
}
gstk_nv_cache_data_type;

/* GSTK NV Cache */
static gstk_nv_cache_data_type gstk_nv_cache;

/*===========================================================================

                        STATIC FUNCTION PROTOTYPES

===========================================================================*/
static void gstk_nv_dump_cache_info(
  const gstk_nv_params_type *item_params_ptr,
  boolean                    dump_cache_data
);

static gstk_status_enum_type gstk_nv_item_read_write(
  nv_func_enum_type  nv_op,
  nv_items_enum_type item,
  nv_item_type     * data_ptr,
  uint16             context
);

static void gstk_nv_access_post_process_meid(
  nv_item_type nv_item
);

static void gstk_nv_access_post_process_feature_bmask(
 uint8 cache_index
);

static gstk_status_enum_type gstk_nv_access_post_process_rat_prio(
  uint8                    cache_index,
  boolean                  is_item_read,
  gstk_nv_items_data_type *data_ptr
);

static gstk_status_enum_type gstk_nv_access_post_process_bip_apn_mapping_tbl(
  uint8                    cache_index,
  gstk_nv_items_data_type *data_ptr
);

static void gstk_nv_access_post_process_ue_imei(
  uint8        cache_index,
  nv_item_type nv_item
);

static void gstk_nv_access_post_process_ue_imeisvn(
  uint8        cache_index,
  nv_item_type nv_item
);

static gstk_status_enum_type gstk_nv_access_int_get_item_params(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_op_categ_enum_type  op_categ,
  gstk_slot_id_enum_type      slot_id,
  sys_modem_as_id_e_type      sub_id,
  gstk_nv_params_type        *item_params_ptr
);

static gstk_status_enum_type gstk_nv_access_int_read(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_op_categ_enum_type  op_categ,
  gstk_nv_params_type        *item_params_ptr,
  gstk_nv_items_data_type    *data_ptr
);

static gstk_status_enum_type gstk_nv_access_int_write(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_params_type        *item_params_ptr,
  gstk_nv_items_data_type    *data_ptr
);

static gstk_status_enum_type gstk_nv_access_int(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_op_categ_enum_type  op_categ,
  gstk_slot_id_enum_type      slot_id,
  sys_modem_as_id_e_type      sub_id,
  gstk_nv_items_data_type    *data_ptr
);

static void gstk_nv_init_me_feature_bmask_cache(void);

static void gstk_nv_init_cache(
  gstk_nv_items_enum_type    item_type_start,
  uint32                     nv_item_count,
  gstk_nv_op_categ_enum_type op_categ,
  gstk_slot_id_enum_type     slot_id,
  sys_modem_as_id_e_type     sub_id
);

static void gstk_nv_init_me_cache(void);

#ifdef FEATURE_MODEM_CONFIG_REFRESH
static boolean gstk_nv_mcfg_refresh_cb(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
);
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

                        GSTK NV UTILITIES

===========================================================================*/

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
FUNCTION gstk_nv_dump_cache_info

DESCRIPTION
  This function prints the NV Cache info

PARAMETERS
  item_params_ptr: [Input] The pointer to NV params to be dumped
  dump_cache_data: [Input] Indicates if NV cache data (part of item_params_ptr)
                           should be dumped or not

RETURN VALUE
  Void

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_dump_cache_info(
  const gstk_nv_params_type *item_params_ptr,
  boolean                    dump_cache_data
)
{
  uint32  i                = 0;
  uint32  cache_data_index = 0;

  if (item_params_ptr == NULL)
  {
    return;
  }

  UIM_MSG_HIGH_4("******** GSTK NV: item_num: %d, is_item_nv: %d, cache_index: %d, nv_index: %d ********",
                 item_params_ptr->item_num,
                 item_params_ptr->is_item_nv,
                 item_params_ptr->cache_index,
                 item_params_ptr->nv_index);

  if(item_params_ptr->cache_state_ptr != NULL)
  {
    UIM_MSG_HIGH_2("cache_state: 0x%x, cache_len: 0x%x, cache_data:",
                   *item_params_ptr->cache_state_ptr,
                   item_params_ptr->cache_data_len);
  }

  if(dump_cache_data && item_params_ptr->cache_data_ptr != NULL)
  {
    const byte *byte_array_ptr = item_params_ptr->cache_data_ptr;

    while (i < (item_params_ptr->cache_data_len / 9))
    {
      cache_data_index = i * 9;
      UIM_MSG_HIGH_9("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                     byte_array_ptr[cache_data_index],
                     byte_array_ptr[cache_data_index+1],
                     byte_array_ptr[cache_data_index + 2],
                     byte_array_ptr[cache_data_index + 3],
                     byte_array_ptr[cache_data_index + 4],
                     byte_array_ptr[cache_data_index + 5],
                     byte_array_ptr[cache_data_index + 6],
                     byte_array_ptr[cache_data_index + 7],
                     byte_array_ptr[cache_data_index + 8]);
      i++;
    }

    cache_data_index = i * 9;
    switch(item_params_ptr->cache_data_len - cache_data_index)
    {
      case 9:
        /* have 9 items left in last line */
        UIM_MSG_HIGH_9("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index + 1],
                       byte_array_ptr[cache_data_index + 2],
                       byte_array_ptr[cache_data_index + 3],
                       byte_array_ptr[cache_data_index + 4],
                       byte_array_ptr[cache_data_index + 5],
                       byte_array_ptr[cache_data_index + 6],
                       byte_array_ptr[cache_data_index + 7],
                       byte_array_ptr[cache_data_index + 8]);
        break;
      case 8:
        /* have 8 items left in last line */
        UIM_MSG_HIGH_8("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index + 1],
                       byte_array_ptr[cache_data_index + 2],
                       byte_array_ptr[cache_data_index + 3],
                       byte_array_ptr[cache_data_index + 4],
                       byte_array_ptr[cache_data_index + 5],
                       byte_array_ptr[cache_data_index + 6],
                       byte_array_ptr[cache_data_index + 7]);
        break;
      case 7:
        /* have 7 items left in last line */
        UIM_MSG_HIGH_7("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index + 1],
                       byte_array_ptr[cache_data_index + 2],
                       byte_array_ptr[cache_data_index + 3],
                       byte_array_ptr[cache_data_index + 4],
                       byte_array_ptr[cache_data_index + 5],
                       byte_array_ptr[cache_data_index + 6]);
        break;
      case 6:
        /* have 6 items left in last line */
        UIM_MSG_HIGH_6("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index + 1],
                       byte_array_ptr[cache_data_index + 2],
                       byte_array_ptr[cache_data_index + 3],
                       byte_array_ptr[cache_data_index + 4],
                       byte_array_ptr[cache_data_index + 5]);
        break;
      case 5:
        /* have 5 item left in last line */
        UIM_MSG_HIGH_5("0x%x 0x%x 0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index + 1],
                       byte_array_ptr[cache_data_index + 2],
                       byte_array_ptr[cache_data_index + 3],
                       byte_array_ptr[cache_data_index + 4]);
        break;
      case 4:
        /* have 4 items left in last line */
        UIM_MSG_HIGH_4("0x%x 0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index+1],
                       byte_array_ptr[cache_data_index + 2],
                       byte_array_ptr[cache_data_index + 3]);
        break;
      case 3:
        /* have 3 items left in last line */
        UIM_MSG_HIGH_3("0x%x 0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index+1],
                       byte_array_ptr[cache_data_index + 2]);
        break;
      case 2:
        /* have 2 items left in last line */
        UIM_MSG_HIGH_2("0x%x 0x%x",
                       byte_array_ptr[cache_data_index],
                       byte_array_ptr[cache_data_index + 1]);
        break;
      case 1:
        /* have 1 item left in last line */
        UIM_MSG_HIGH_1("0x%x",
                       byte_array_ptr[cache_data_index]);
        break;
      default:
        break;
    }
  }
} /* gstk_nv_dump_cache_info */

/*===========================================================================
FUNCTION gstk_nv_item_read_write

DESCRIPTION
  Access an item in nonvolatile memory

PARAMETERS
  nv_op:    [Input] NV operation requested
  item:     [Input] NV item to access
  data_ptr: [Input/Output] Pointer to the data buffer
  context:  [Input] NV context

RETURN VALUE
  gstk_status_enum_type
    GSTK_SUCCESS : if NV_DONE
    GSTK_ERROR   : otherwise.

DEPENDENCIES
  None

COMMENTS
  Only supports NV read and write operations

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_nv_item_read_write(
  nv_func_enum_type  nv_op,
  nv_items_enum_type item,
  nv_item_type     * data_ptr,
  uint16             context
)
{
  rex_tcb_type    *current_task_tcb_ptr = NULL;
  nv_cmd_ext_type  gstk_nv_cmd_ext;
  nv_cmd_type      gstk_nv_cmd;

  if(data_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  if(nv_op != NV_READ_F &&
     nv_op != NV_WRITE_F)
  {
    return GSTK_UNSUPPORTED_COMMAND;
  }

  current_task_tcb_ptr = rex_self();

  /* Sync NV access can only be done from task context */
  if(current_task_tcb_ptr != UIM_GSTK_TCB)
  {
    return GSTK_ERROR;
  }

  gstk_nv_cmd_ext.nvcmd = &gstk_nv_cmd;
  gstk_nv_cmd_ext.context = context;

  /* Notify GSTK when done */
  gstk_nv_cmd.tcb_ptr = current_task_tcb_ptr;
  gstk_nv_cmd.sigs = GSTK_NV_SIG;

  /* Command goes on no queue when done */
  gstk_nv_cmd.done_q_ptr = NULL;

  /* Item to access */
  gstk_nv_cmd.item = item;
  gstk_nv_cmd.cmd = nv_op;

  /* Set the Data Ptr for NV ITEM to read */
   gstk_nv_cmd.data_ptr = data_ptr;

  /* Clear REX Signals */
  (void) rex_clr_sigs(rex_self(), GSTK_NV_SIG);

  /* Send Command to NV task */
  nv_cmd_ext(&gstk_nv_cmd_ext);

  /* Wait for Signal from NV that it's complete */
  (void)gstk_wait(GSTK_NV_SIG);

  /* Clear Signal */
  (void)rex_clr_sigs(current_task_tcb_ptr, GSTK_NV_SIG);

  UIM_MSG_HIGH_3("In gstk_nv_item_read_write, item: 0x%x, context: 0x%x, NV Status: 0x%x",
                 item, context, gstk_nv_cmd.status);

  if (gstk_nv_cmd.status != NV_DONE_S)
  {
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstk_nv_item_read_write */

/*===========================================================================

                    GSTK NV ITEM ACCESS - INTERNAL POST_PROCESSING FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION gstk_nv_access_post_process_meid

DESCRIPTION
  This function parses the raw MEID read from NV and updates the NV cache

PARAMETERS
  nv_item: [Input] The raw MEID read from NV

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_access_post_process_meid(
  nv_item_type nv_item
)
{
  uint8                 index       = 0;
  uint8                 temp        = 0;
  uint32                meid_lo     = 0;
  uint32                meid_hi     = 0;

  /* Parse MEID only if the raw NV read was successful */
  if(gstk_nv_cache.me_items.meid.state == GSTK_NV_STATE_INIT)
  {
    meid_lo = qw_lo(nv_item.meid);
    meid_hi = qw_hi(nv_item.meid);

    do
    {
      (void)gstk_memcpy(gstk_nv_cache.me_items.meid.value,
                        &meid_lo,
                        GSTK_MEID_LEN/2,
                        GSTK_MEID_LEN,
                        sizeof(uint32));

      (void)gstk_memcpy(gstk_nv_cache.me_items.meid.value + 4,
                        &meid_hi,
                        GSTK_MEID_LEN/2,
                        GSTK_MEID_LEN/2,
                        sizeof(uint32));

      for(index = 0; index < (GSTK_MEID_LEN/2); index++)
      {
        temp = gstk_nv_cache.me_items.meid.value[index];
        gstk_nv_cache.me_items.meid.value[index] =
          gstk_nv_cache.me_items.meid.value[GSTK_MEID_LEN - index - 1];
        gstk_nv_cache.me_items.meid.value[GSTK_MEID_LEN - index - 1] = temp;
      }
    }while(0);
  }
} /* gstk_nv_access_post_process_meid */

/*===========================================================================
FUNCTION gstk_nv_access_post_process_feature_bmask

DESCRIPTION
  This function parses the raw feature bitmask read from NV and updates
  the NV cache

PARAMETERS
  cache_index: [Input] The slot NV cache index to be updated

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_access_post_process_feature_bmask(
 uint8 cache_index
)
{
  gstk_status_enum_type   gstk_status = GSTK_ERROR;
  uint8                   index       = 0;
  nv_item_type            nv_item;
  gstk_nv_items_data_type read_data;

  if(cache_index >= GSTK_MAX_NUM_OF_INSTANCES ||
     gstk_nv_cache.slot_items_ptr[cache_index] == NULL)
  {
    return;
  }

  /* Set default value */
  if(gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.state == GSTK_NV_STATE_NOT_FOUND)
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value = 0;
  }

  UIM_MSG_HIGH_2("GSTK NV feature_bmask, raw value: 0x%x, gstk_num_of_instances: 0x%x",
                 gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value,
                 gstk_shared_data.gstk_num_of_instances);

  /* If device supports only a single slot... */
  if((gstk_shared_data.gstk_num_of_instances == 1) && (cache_index == 0))
  {
    memset(&nv_item, 0x00, sizeof(nv_item));

    /* Disable VPE feature on CDMA only device */
    gstk_status = gstk_nv_item_read_write(
                    NV_READ_F,
                    NV_PREF_MODE_I,
                    &nv_item,
                    0);

    if (gstk_status == GSTK_SUCCESS)
    {
      switch (nv_item.pref_mode.mode)
      {
        case NV_MODE_DIGITAL_ONLY:
        case NV_MODE_PCS_CDMA_ONLY:
        case NV_MODE_CELL_CDMA_ONLY:
          /* Disable loc_info and its timer at power-up if device is set to CDMA only mode */
          UIM_MSG_HIGH_1("Device in CDMA only mode: 0x%x, disabled GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS",
                         nv_item.pref_mode.mode);
          gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value &=
            ~(GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS);
          break;

        default:
          break;
      }
    }
  }
  /* Always disable VPE feature on multi-SIM tagets */
  else
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value &= ~(GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS);
    UIM_MSG_HIGH_1("Disabled GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS on slot_id: 0x%x",
                   (cache_index + 1));
  }

  (void)gstk_nv_access_read(GSTK_NV_EXT_ME_GCF_GSTK,
                            GSTK_SLOT_ID_MAX,
                            &read_data);

  if(read_data.gcf_gstk == TRUE)
  {
    /* Disable proprietary behavior if
       /nv/item_files/modem/uim/mmgsdi/gcf_gstk is set */
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value &= ~(GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH);
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value &= ~(GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS);
    /* Enable GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT and GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND if
       /nv/item_files/modem/uim/mmgsdi/gcf_gstk is set */
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value |= GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT;
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value |= GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND;
  }

  /* Envelope retry should always be enabled if VPE feature is enabled */
  if(gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value & GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS)
  {
    for(index = 0; index < gstk_shared_data.gstk_num_of_instances; index++)
    {
      gstk_instances_ptr[index]->gstk_curr_envelope_info.retry_info.retry_flag = TRUE;
    }
  }
  /* Else, use the value from NV_TOOLKIT_ENV_RETRY_FLAG_I */
  else
  {
    (void)gstk_nv_access_read(GSTK_NV_ME_TOOLKIT_ENV_RETRY_FLAG_I,
                              GSTK_SLOT_ID_MAX,
                              &read_data);                    

    for(index = 0; index < gstk_shared_data.gstk_num_of_instances; index++)
    {
      gstk_instances_ptr[index]->gstk_curr_envelope_info.retry_info.retry_flag = 
        read_data.env_retry;        
    }
  }

  /* Enable GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7 based on cat version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            cache_index + 1,
                            &read_data);
  if(read_data.cat_version >= (uint8)GSTK_CFG_CAT_VER7)
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value |= GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7;
  }
  else
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->feature_bmask.value &= ~(GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7);
  }
} /* gstk_nv_access_post_process_feature_bmask */

/*===========================================================================
FUNCTION gstk_nv_access_post_process_rat_prio

DESCRIPTION
  This function parses the raw RAT Priority read from NV and
  updates the NV cache.
  It also copies the parsed RAT priority into the data buffer passed in

PARAMETERS
  cache_index:  [Input] The slot NV cache index to be updated
  is_item_read: [Input] Indicates if item has been newly read from EFS.
                        If so, the NV item needs to be parsed.
  data_ptr:     [Output] Data buffer to copy the parsed RAT priority into

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_nv_access_post_process_rat_prio(
  uint8                    cache_index,
  boolean                  is_item_read,
  gstk_nv_items_data_type *data_ptr
)
{
  if(cache_index >= GSTK_MAX_NUM_OF_INSTANCES ||
     gstk_nv_cache.slot_items_ptr[cache_index] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* If the raw NV item is newly read, Parse the same */
  if(is_item_read)
  {
    /* Cache state being init indicates that the raw NV read was successful */
    if(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.state == GSTK_NV_STATE_INIT)
    {
      uint8  i = 0;
      uint8  j = 0;
      uint8  k = 0;

      /* Parse the raw RAT priority */
      for(i=0, j=0, k=0;
          i <= GSTK_NV_RAT_PRIO_CONFIG_LEN && j < GSTK_NV_RAT_PRIO_CONFIG_LEN && k < GSTK_MAX_NUM_RATS;
          i++)
      {
        /* Format - GSM, UMTS, LTE, 1X, HRPD */
        if(i == GSTK_NV_RAT_PRIO_CONFIG_LEN ||
           gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value[i] == 0x2C)
        {
          if (((i-j) == (sizeof("GSM")-1)) &&
			  (j <= (GSTK_NV_RAT_PRIO_CONFIG_LEN - (i - j)))&&
              (!memcmp(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value+j,
                       "GSM",
                       i-j)))
          {
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_GSM;
          }
          else if (((i-j) == (sizeof("UMTS")-1)) &&
                   (j <= (GSTK_NV_RAT_PRIO_CONFIG_LEN - (i - j)))&&
                   (!memcmp(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value+j,
                            "UMTS",
                            i-j)))
          {
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_UMTS;
          }
          else if (((i-j) == (sizeof("LTE")-1)) &&
                   (j <= (GSTK_NV_RAT_PRIO_CONFIG_LEN - (i - j)))&&
                   (!memcmp(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value+j,
                            "LTE",
                            i-j)))
          {
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_LTE;
          }
          else if (((i-j) == (sizeof("1X")-1)) &&
                   (j <= (GSTK_NV_RAT_PRIO_CONFIG_LEN - (i - j)))&&
                   (!memcmp(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value+j,
                            "1X",
                            i-j)))
          {
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_1X;
          }
          else if (((i-j) == (sizeof("EHRPD")-1)) &&
                   (j <= (GSTK_NV_RAT_PRIO_CONFIG_LEN - (i - j)))&&
                   (!memcmp(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value+j,
                            "EHRPD",
                            i-j)))
          {
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_EHRPD;
          }
          else if (((i-j) == (sizeof("HRPD")-1)) &&
                   (j <= (GSTK_NV_RAT_PRIO_CONFIG_LEN - (i - j)))&&
                   (!memcmp(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value+j,
                            "HRPD",
                            i-j)))
          {
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_HRPD;
          }
          else
          {
            UIM_MSG_ERR_0("Unrecognized RAT");
          }
          if(i != GSTK_NV_RAT_PRIO_CONFIG_LEN &&
            gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.value[i] == 0x2C)
          {
            j=i+1;
          }
          UIM_MSG_HIGH_2("Stored rat_priority[%d] = 0x%x",
                         k, gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k]);
          k++;
        }
      }
      /* Reset the remaining rat priority values to Invalid */
      while(k < GSTK_MAX_NUM_RATS)
      {
        gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[k] = GSTK_RAT_INVALID;
        k++;
      }
    }
    else
    {
      /* Cache of default RAT priority */
      gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[0] = GSTK_RAT_LTE;
      gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[1] = GSTK_RAT_EHRPD;
      gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[2] = GSTK_RAT_HRPD;
      gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[3] = GSTK_RAT_1X;
      gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[4] = GSTK_RAT_UMTS;
      gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list[5] = GSTK_RAT_GSM;
    }
  }

  /* Copy the parsed RAT priority to the output buffer */
  if(data_ptr != NULL)
  {
    if(gstk_memcpy(data_ptr->rat_prio,
                   gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list,
                   sizeof(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list),
                   sizeof(data_ptr->rat_prio),
                   sizeof(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list)) <
       sizeof(gstk_nv_cache.slot_items_ptr[cache_index]->rat_prio.prio_list))
    {
      return GSTK_ERROR;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_nv_access_post_process_rat_prio */

/*===========================================================================
FUNCTION gstk_nv_access_bip_apn_mapping_tbl

DESCRIPTION
  This function parses copies the raw BIP APN mapping table read from NV
  into the data buffer passed in

PARAMETERS
  cache_index: [Input] The slot NV cache index to be updated
  data_ptr:    [Output] Data buffer to copy the parsed BIP APN mapping table into

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
static gstk_status_enum_type gstk_nv_access_post_process_bip_apn_mapping_tbl(
  uint8                    cache_index,
  gstk_nv_items_data_type *data_ptr
)
{
  uint8   i            = 0;
  uint8   apn_index    = 0;

  if(cache_index >= GSTK_MAX_NUM_OF_INSTANCES ||
     gstk_nv_cache.slot_items_ptr[cache_index] == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  if((gstk_nv_cache.slot_items_ptr[cache_index]->bip_apn_mapping_tbl_ptr == NULL) ||
     (data_ptr == NULL) || (data_ptr->apn_mapping_table_ptr == NULL))
  {
    UIM_MSG_ERR_2("NULL ptr, bip_apn_mapping_tbl_ptr: 0x%x, data_ptr: 0x%x",
                  gstk_nv_cache.slot_items_ptr[cache_index]->bip_apn_mapping_tbl_ptr,
                  data_ptr);
    return GSTK_BAD_PARAM;
  }

  /* If NV is present and cached, copy to output buffer */
  if(gstk_nv_cache.slot_items_ptr[cache_index]->bip_apn_mapping_tbl_ptr->state == GSTK_NV_STATE_INIT)
  {
    gstk_nv_bip_apn_mapping_tbl_data_type *nv_raw_value_ptr =
      gstk_nv_cache.slot_items_ptr[cache_index]->bip_apn_mapping_tbl_ptr;

    memset(data_ptr->apn_mapping_table_ptr->value,
           0x00,
           sizeof(data_ptr->apn_mapping_table_ptr->value));

    for (apn_index = 0; apn_index < ESTK_APN_MAPPING_MAX; apn_index++)
    {
      data_ptr->apn_mapping_table_ptr->value[apn_index].version =
        nv_raw_value_ptr->value[apn_index].version;

      i = 0;
      while (nv_raw_value_ptr->value[apn_index].input_apn[i] != 0 &&
             i < (ESTK_APN_MAX_LENGTH - 1))
      {
        data_ptr->apn_mapping_table_ptr->value[apn_index].input_apn[i] =
          nv_raw_value_ptr->value[apn_index].input_apn[i];
        i++;
      }
      i = 0;
      while (nv_raw_value_ptr->value[apn_index].output_apn[i] != 0 &&
             i < (ESTK_APN_MAX_LENGTH - 1))
      {
        data_ptr->apn_mapping_table_ptr->value[apn_index].output_apn[i] =
          nv_raw_value_ptr->value[apn_index].output_apn[i];
        i++;
      }
    }
  }
  else
  {
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstk_nv_access_post_process_bip_apn_mapping_tbl */

/*===========================================================================
FUNCTION gstk_nv_access_post_process_ue_imei

DESCRIPTION
  This function parses the raw IMEI read from NV and updates the NV cache

PARAMETERS
  cache_index: [Input] The slot NV cache index to be updated
  nv_item:     [Input] The raw IMEI read from NV

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_access_post_process_ue_imei(
  uint8        cache_index,
  nv_item_type nv_item

)
{
  if(cache_index >= GSTK_MAX_NUM_OF_INSTANCES ||
     gstk_nv_cache.slot_items_ptr[cache_index] == NULL)
  {
    return;
  }

  if((gstk_nv_cache.slot_items_ptr[cache_index]->ue_imei.state == GSTK_NV_STATE_INIT) &&
     (nv_item.ue_imei.ue_imei[0] != 0))
  {
    if(gstk_memcpy(gstk_nv_cache.slot_items_ptr[cache_index]->ue_imei.value,
                   &nv_item.ue_imei.ue_imei[1],
                   GSTK_IMEI_LEN,
                   GSTK_IMEI_LEN,
                   NV_UE_IMEI_SIZE - 1) !=
       GSTK_IMEI_LEN)
    {
      gstk_nv_cache.slot_items_ptr[cache_index]->ue_imei.state = GSTK_NV_STATE_NOT_FOUND;
    }
  }
  else
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->ue_imei.state = GSTK_NV_STATE_NOT_FOUND;
  }
} /* gstk_nv_access_post_process_ue_imei */

/*===========================================================================
FUNCTION gstk_nv_access_post_process_ue_imeisvn

DESCRIPTION
  This function parses the raw IMEI_SVN read from NV and updates the NV cache

PARAMETERS
  cache_index: [Input] The slot NV cache index to be updated
  nv_item:     [Input] The raw IMEI_SVN read from NV

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_access_post_process_ue_imeisvn(
  uint8        cache_index,
  nv_item_type nv_item
)
{
  gstk_nv_items_data_type read_data;
  uint8                   digit1      = 0;
  uint8                   digit2      = 0;
  gstk_status_enum_type   gstk_status = GSTK_ERROR;

  if(cache_index >= GSTK_MAX_NUM_OF_INSTANCES ||
     gstk_nv_cache.slot_items_ptr[cache_index] == NULL)
  {
    return;
  }

  if((gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.state == GSTK_NV_STATE_INIT) &&
     (nv_item.ue_imeisv_svn <= 98))
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.state = GSTK_NV_STATE_NOT_FOUND;

    gstk_status = gstk_nv_access_read(GSTK_NV_EXT_SLOT_UE_IMEI_I,
                                      (gstk_slot_id_enum_type)(cache_index + 1),
                                      &read_data);

    if(gstk_status == GSTK_SUCCESS)
    {
      if(gstk_memcpy(gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value,
                     read_data.ue_imei,
                     GSTK_IMEI_LEN,
                     GSTK_IMEI_SV_LEN,
                     GSTK_IMEI_LEN) ==
         GSTK_IMEI_LEN)
      {
        /* The number of digits is now even and not odd,
           so clear the odd/even bit to indicate even   */
        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value[0] &= 0xF7;

        /* The type of the identity is now IMEISV and not IMEI */
        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value[0] |= 0x01;

        /* Add the SVN (Software Version Number)  */
        digit1 = nv_item.ue_imeisv_svn / 10;
        digit1 = (byte) (digit1 << 4);

        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value[7] &= 0x0f;
        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value[7] |= digit1;

        digit2 = nv_item.ue_imeisv_svn % 10;
        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value[8] = digit2;
        /* Set top nibble to all 1's */
        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.value[8] |= 0xF0;
        gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.state = GSTK_NV_STATE_INIT;
      }
    }
  }
  else
  {
    gstk_nv_cache.slot_items_ptr[cache_index]->ue_imeisvn.state = GSTK_NV_STATE_NOT_FOUND;
  }
} /* gstk_nv_access_post_process_ue_imeisvn */

/*===========================================================================

                        GSTK NV ITEM ACCESS - INTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION gstk_nv_access_int_get_item_params

DESCRIPTION
  This function retrieves the NV/EFS item parameters that can then be
  used to access the NV/EFS item

PARAMETERS:
  item_type:       [Input] NV/EFS item to be accesses
  op_categ:        [Input] Category of NV access
  slot_id:         [Input] Slot ID on which access is requested
  sub_id:          [Input] Subscription ID on which access is requested
  item_params_ptr: [Output] NV/EFS parameters

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
static gstk_status_enum_type gstk_nv_access_int_get_item_params(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_op_categ_enum_type  op_categ,
  gstk_slot_id_enum_type      slot_id,
  sys_modem_as_id_e_type      sub_id,
  gstk_nv_params_type        *item_params_ptr
)
{
  uint32                          item_class     = item_type & GSTK_NV_CLASS_MASK;
  gstk_status_enum_type           gstk_status    = GSTK_SUCCESS;
  uim_common_efs_status_enum_type uim_efs_status = UIM_COMMON_EFS_ERROR;

  if(item_params_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  memset(item_params_ptr, 0x00, sizeof(gstk_nv_params_type));

  /*---------------------------------------------------------------------------
    Get the cache index and NV index corresponding to the NV/EFS item
  ---------------------------------------------------------------------------*/
  switch(item_class)
  {
    /* ME based NV/EFS items */
    case GSTK_NV_ME_START:
    case GSTK_NV_EXT_ME_START:
      /* ME based NV items do not support NV Refresh */
      if(op_categ == GSTK_NV_OP_CATEG_NV_REFRESH ||
         op_categ == GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD)
      {
        UIM_MSG_ERR_2("Item Class: 0x%x, Op Category: 0x%x not supported",
                      item_class, op_categ);
        return GSTK_BAD_PARAM;
      }

      /* Note: ME cache index and NV index are 0 - already set
         via memset on item_params_ptr*/
      break;

    /* Slot based NV/EFS items */
    case GSTK_NV_SLOT_START:
    case GSTK_NV_EXT_SLOT_START:
      /* Validate Slot ID */
      if(!GSTK_IS_VALID_SLOT_ID(slot_id))
      {
        UIM_MSG_ERR_1("Invalid slot id 0x%x", slot_id);
        return GSTK_BAD_PARAM;
      }

      /* Validate NV cache corresponding to the slot */
      if(gstk_nv_cache.slot_items_ptr[(uint8)(slot_id - 1)] == NULL)
      {
        UIM_MSG_ERR_1("Invalid NV cache w.r.t. slot id 0x%x",
                       slot_id);
        return GSTK_BAD_PARAM;
      }

      /* Slot cache index and NV index are same for slot based NV items */
      item_params_ptr->cache_index = (uint8)(slot_id - 1);
      item_params_ptr->nv_index = (int8)(slot_id - 1);
      break;

    /* Sub based NV/EFS items */
    case GSTK_NV_SUB_START:
    case GSTK_NV_EXT_SUB_START:
      /* Validate Slot ID */
      if(!GSTK_IS_VALID_SLOT_ID(slot_id))
      {
        UIM_MSG_ERR_1("Invalid slot id 0x%x", slot_id);
        return GSTK_BAD_PARAM;
      }

      /* Validate NV cache corresponding to the Sub */
      if(gstk_nv_cache.sub_items_ptr[(uint8)(slot_id - 1)] == NULL)
      {
        UIM_MSG_ERR_1("Invalid NV cache w.r.t. slot id 0x%x",
                       slot_id);
        return GSTK_BAD_PARAM;
      }

      /* For op categories where Sub ID is not passed by the caller,
         retrieve the same using the slot ID */
      if(op_categ == GSTK_NV_OP_CATEG_ACCESS_READ ||
         op_categ == GSTK_NV_OP_CATEG_ACCESS_WRITE ||
         op_categ == GSTK_NV_OP_CATEG_INIT)
      {
        sys_modem_as_id_e_type sub_id_local = SYS_MODEM_AS_ID_MAX;

        gstk_status = gstk_util_get_sub_id_from_slot(
                        slot_id, &sub_id_local);

        if(gstk_status != GSTK_SUCCESS)
        {
          return GSTK_ERROR;
        }

        /* Update NV index based on SUB ID */
        switch (sub_id_local)
        {
          case SYS_MODEM_AS_ID_1:
            item_params_ptr->nv_index = 0;
            break;
          case SYS_MODEM_AS_ID_2:
            item_params_ptr->nv_index = 1;
            break;
          case SYS_MODEM_AS_ID_3:
            item_params_ptr->nv_index = 2;
            break;
          default:
            return GSTK_ERROR;
        }
      }
      else
      {
        item_params_ptr->nv_index = (int8)sub_id;
      }

      /* Validate the Sub ID */
      if((item_params_ptr->nv_index < (int8)SYS_MODEM_AS_ID_1) ||
         (item_params_ptr->nv_index > (int8)SYS_MODEM_AS_ID_3))
      {
        UIM_MSG_ERR_1("Invalid sub_id: %d",
                      item_params_ptr->nv_index);
        return GSTK_BAD_PARAM;
      }

      item_params_ptr->cache_index = (uint8)(slot_id - 1);
      break;

    default:
      UIM_MSG_ERR_1("Invalid item_class: 0x%x", item_class);
      return GSTK_BAD_PARAM;
  }

  /*---------------------------------------------------------------------------
    1. Get the NV/EFS item number, cache state pointer, cache
       data pointer and cache length.
    2. For an NV item, also mark that the item is an NV.
  ---------------------------------------------------------------------------*/
  switch (item_type)
  {
    case GSTK_NV_ME_TOOLKIT_ENV_RETRY_FLAG_I:
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num =
        (uint32)NV_TOOLKIT_ENV_RETRY_FLAG_I;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.env_retry.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.env_retry.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.env_retry.state;
      break;
      
    case GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->env_retry_ext.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->env_retry_ext.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->env_retry_ext.state;        
      break;      

    case GSTK_NV_ME_TP_CLIENTS:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_ME_TP_CLIENTS;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.tp_clients.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.tp_clients.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.tp_clients.state;
      break;

    case GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_ME_PROACTIVE_FEATURE_ENABLE_CFG;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.proact_feat_enable.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.proact_feat_enable.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.proact_feat_enable.state;
      break;

#ifdef FEATURE_GSTK_TAL
    case GSTK_NV_ME_TAL_CONF:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_ME_TAL_CONF;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.me_items.tal_conf.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.tal_conf.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.tal_conf.state;
      break;
#endif /* FEATURE_GSTK_TAL */

    case GSTK_NV_ME_REDIAL_CFG:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_ME_REDIAL_CFG;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.redial_cfg.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.redial_cfg.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.redial_cfg.state;
      break;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

    case GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I:
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num = (uint32)NV_GPRS_ANITE_GCF_I;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.gprs_anite_gcf.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.gprs_anite_gcf.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.gprs_anite_gcf.state;
      break;

    case GSTK_NV_EXT_ME_QMI_CAT_MODE:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_UIMQMI_QMI_CAT_MODE;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.qmi_cat_mode.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.qmi_cat_mode.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.qmi_cat_mode.state;
      break;

    case GSTK_NV_EXT_ME_GCF_GSTK:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_MMGSDI_GCF_GSTK_FILE;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.gcf_gstk.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.gcf_gstk.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.gcf_gstk.state;
      break;

    case GSTK_NV_EXT_ME_ESN_ME_I:
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num = (uint32)NV_ESN_ME_I;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.me_items.esn_me.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.esn_me.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.esn_me.state;
      break;

    case GSTK_NV_EXT_ME_MEID_I:
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num = (uint32)NV_MEID_ME_I;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.me_items.meid.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.meid.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.meid.state;
      break;

    case GSTK_NV_EXT_ME_QMI_CAT_DEFAULT_LANG:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_UIMQMI_QMI_CAT_DEFAULT_LANG;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.me_items.qmi_cat_default_lang.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.qmi_cat_default_lang.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.qmi_cat_default_lang.state;
      break;

    case GSTK_NV_ME_FEATURE_BMASK:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_ME_FEATURE_BMASK;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.me_items.me_spec_feature_bmask.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.me_items.me_spec_feature_bmask.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.me_items.me_spec_feature_bmask.state;
      break;

    case GSTK_NV_SLOT_CAT_VERSION:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SLOT_CAT_VERSION;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->cat_version.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->cat_version.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->cat_version.state;
      break;

    case GSTK_NV_SLOT_FEATURE_BMASK:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->feature_bmask.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->feature_bmask.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->feature_bmask.state;
      break;

    case GSTK_NV_SLOT_FEATURE_BMASK_2:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SLOT_FEATURE_BMASK_2;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->feature_bmask_2.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->feature_bmask_2.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->feature_bmask_2.state;
      break;

    case GSTK_NV_SLOT_TOOLKIT_CS_PS_PARALLEL_I:
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num =
        (uint32)NV_TOOLKIT_CS_PS_PARALLEL_I;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->cs_ps_parallel.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->cs_ps_parallel.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->cs_ps_parallel.state;
      break;

    case GSTK_NV_SLOT_RAT_PRIORITY_CONFIG:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SLOT_RAT_PRIORITY_CONFIG;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->rat_prio.value;
      item_params_ptr->cache_data_len =
        GSTK_NV_RAT_PRIO_CONFIG_LEN;
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->rat_prio.state;
      break;

    case GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE:
      item_params_ptr->item_num = (uint32)UIM_COMMON_EFS_GSTK_SLOT_BIP_APN_MAPPING_TABLE;

      /* Allocate memory for BIP APN mapping table cache */
      if(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr == NULL)
      {
        gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr =
          (gstk_nv_bip_apn_mapping_tbl_data_type *) GSTK_CALLOC(sizeof(gstk_nv_bip_apn_mapping_tbl_data_type));
        if (gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr == NULL)
        {
          return GSTK_MEMORY_ERROR;
        }
        memset(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr,
               0x00,
               sizeof(gstk_nv_bip_apn_mapping_tbl_data_type));
      }

      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr->value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr->value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->bip_apn_mapping_tbl_ptr->state;
      break;

    case GSTK_NV_SLOT_DEFAULT_APN:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SLOT_DEFAULT_APN;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->default_apn.value;

      uim_efs_status = uim_common_efs_get_file_size(
                         (uim_common_efs_item_type)item_params_ptr->item_num,
                         (uim_common_efs_context_type)item_params_ptr->nv_index,
                         &item_params_ptr->cache_data_len);

      if((uim_efs_status != UIM_COMMON_EFS_SUCCESS) ||
         (item_params_ptr->cache_data_len == 0) ||
         (item_params_ptr->cache_data_len > (ESTK_APN_MAX_LENGTH + 1)))
      {
        UIM_MSG_ERR_2("Invalid Default APN, Read file size status: 0x%x, file size: 0x%x",
                      uim_efs_status, item_params_ptr->cache_data_len);
        return GSTK_ERROR;
      }
      else
      {
        UIM_MSG_HIGH_1("Default APN file size: 0x%x",
                       item_params_ptr->cache_data_len);
      }

      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->default_apn.state;
      break;

    case GSTK_NV_EXT_SLOT_UE_IMEI_I:
#ifndef FEATURE_DUAL_IMEI
       /* When multiple IMEI is not supported, always use
          IMEI from NV index 0 */
      if(gstk_nv_cache.slot_items_ptr[0] == NULL)
      {
        UIM_MSG_HIGH_1("Invalid NV cache w.r.t. slot id 0x%x",
                       GSTK_SLOT_1);
        return GSTK_BAD_PARAM;
      }
      item_params_ptr->cache_index = 0;
      item_params_ptr->nv_index = 0;
#endif /* FEATURE_DUAL_IMEI */
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num =
      (uint32)NV_UE_IMEI_I;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->ue_imei.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->ue_imei.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->ue_imei.state;
      break;

    case GSTK_NV_EXT_SLOT_UE_IMEISVN_I:
#ifndef FEATURE_DUAL_IMEI
      /* When multiple IMEI is not supported, always use
         IMEISVN from NV index 0 */
      if(gstk_nv_cache.slot_items_ptr[0] == NULL)
      {
        UIM_MSG_HIGH_1("Invalid NV cache w.r.t. slot id 0x%x",
                       GSTK_SLOT_1);
        return GSTK_BAD_PARAM;
      }
      item_params_ptr->cache_index = 0;
      item_params_ptr->nv_index = 0;
#endif /* FEATURE_DUAL_IMEI */
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num =
      (uint32)NV_UE_IMEISV_SVN_I;
      item_params_ptr->cache_data_ptr =
        (void*)gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->ue_imeisvn.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->ue_imeisvn.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.slot_items_ptr[item_params_ptr->cache_index]->ue_imeisvn.state;
      break;

    case GSTK_NV_SUB_BIP_OTASP_CONCURRENCY:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SUB_BIP_OTASP_CONCURRENCY;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->bip_otasp_concurrency.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->bip_otasp_concurrency.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->bip_otasp_concurrency.state;
      break;

    case GSTK_NV_SUB_ATTACH_CTRL_TIMER:
      item_params_ptr->item_num =
        (uint32)UIM_COMMON_EFS_GSTK_SUB_ATTACH_CTRL_TIMER;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->attach_ctrl_timer.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->attach_ctrl_timer.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->attach_ctrl_timer.state;
      break;

    case GSTK_NV_EXT_SUB_DTM_FEATURE_SUPPORT_I:
      item_params_ptr->is_item_nv = TRUE;
      item_params_ptr->item_num =
        (uint32)NV_DTM_FEATURE_SUPPORT_I;
      item_params_ptr->cache_data_ptr =
        (void*)&gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->dtm_feat_support.value;
      item_params_ptr->cache_data_len =
        sizeof(gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->dtm_feat_support.value);
      item_params_ptr->cache_state_ptr =
        &gstk_nv_cache.sub_items_ptr[item_params_ptr->cache_index]->dtm_feat_support.state;
      break;

    default:
      UIM_MSG_ERR_1("NV item: 0x%x access not supported", item_type);
      return GSTK_BAD_PARAM;
  }

  /* For ME specific EFS based NV items, EFS context of
     UIM_COMMON_EFS_DEVICE needs to be passed */
  if((item_class == GSTK_NV_ME_START ||
      item_class == GSTK_NV_EXT_ME_START) &&
     (item_params_ptr->is_item_nv == FALSE))
  {
    item_params_ptr->nv_index = UIM_COMMON_EFS_DEVICE;
  }

  return GSTK_SUCCESS;
} /* gstk_nv_access_int_get_item_params */

/*===========================================================================
FUNCTION gstk_nv_access_int_read

DESCRIPTION
  This function, depending on the operation category requested, handles
  NV/EFS read/init/refresh and updates the NV cache

PARAMETERS:
  item_type:        [Input] NV/EFS item to be accesses
  op_categ:         [Input] Category of NV access
  item_params_ptr:  [Input/Output] NV/EFS parameters
  data_ptr:         [Input/Output] Data buffer

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
static gstk_status_enum_type gstk_nv_access_int_read(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_op_categ_enum_type  op_categ,
  gstk_nv_params_type        *item_params_ptr,
  gstk_nv_items_data_type    *data_ptr
)
{
  gstk_status_enum_type           gstk_status    = GSTK_SUCCESS;
  uim_common_efs_status_enum_type uim_efs_status = UIM_COMMON_EFS_ERROR;
  boolean                         is_item_read   = FALSE;
  uint32                          item_categ     = item_type & GSTK_NV_CATEG_MASK;
  nv_item_type                    nv_item;
  gstk_nv_items_data_type         write_data;

  if(item_params_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  if(item_params_ptr->cache_data_ptr == NULL ||
     item_params_ptr->cache_state_ptr == NULL)
  {
    UIM_MSG_ERR_2("cache_data_ptr: 0x%x or cache_state_ptr: 0x%x is NULL",
                  item_params_ptr->cache_data_ptr,
                  item_params_ptr->cache_state_ptr);
    return GSTK_BAD_PARAM;
  }

  memset(&nv_item, 0x00, sizeof(nv_item));
  memset(&write_data, 0x00, sizeof(gstk_nv_items_data_type));

  /*---------------------------------------------------------------------------
     Based on item category, operation category and cache state, decide
     if the NV/EFS item needs to be read from FS or not
  ---------------------------------------------------------------------------*/
  if(item_categ == GSTK_NV_INIT_NO_REREAD_START ||
     item_categ == GSTK_NV_ONDEMAND_NO_REREAD_START)
  {
    switch (op_categ)
    {
      case GSTK_NV_OP_CATEG_ACCESS_READ:
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_INIT)
        {
          is_item_read = TRUE;
        }
        break;

      case GSTK_NV_OP_CATEG_INIT:
        if((*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_INIT) ||
           (*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_CHANGED))
        {
          is_item_read = TRUE;
        }
        break;

      case GSTK_NV_OP_CATEG_NV_REFRESH:
        is_item_read = TRUE;
        break;

      case GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD:
        *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_CHANGED;
        break;

      case GSTK_NV_OP_CATEG_ACCESS_WRITE:
        return GSTK_BAD_PARAM;
    }
  }
  else if(item_categ == GSTK_NV_INIT_REREAD_START||
          item_categ == GSTK_NV_ONDEMAND_REREAD_START)
  {
    switch (op_categ)
    {
      case GSTK_NV_OP_CATEG_ACCESS_READ:
      case GSTK_NV_OP_CATEG_INIT:
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_INIT)
        {
          is_item_read = TRUE;
        }
        break;

      case GSTK_NV_OP_CATEG_NV_REFRESH:
      case GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD:
        *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_INIT;
        break;

      case GSTK_NV_OP_CATEG_ACCESS_WRITE:
        return GSTK_BAD_PARAM;
    }
  }
  else
  {
    UIM_MSG_ERR_1("invalid item_categ: 0x%x",
                  item_categ);
    return GSTK_BAD_PARAM;
  }

  /*-------------------------------------------------------------------------
     Pre-processing before NV access operation
  -------------------------------------------------------------------------*/
  switch (item_type)
  {
    case GSTK_NV_SUB_ATTACH_CTRL_TIMER:
      if(is_item_read == TRUE)
      {
        /* If VPE feature is disabled, do not read attach control timer */
        if(gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
             (gstk_slot_id_enum_type)item_params_ptr->cache_index + 1) == FALSE)
        {
          is_item_read = FALSE;

          /* Set efault value */
          *(uint32*)item_params_ptr->cache_data_ptr = 0;
          *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_FOUND;
        }
      }
      break;
    default:
      break;
  }

  item_params_ptr->is_item_read_from_fs = is_item_read;

  /*-------------------------------------------------------------------------
     Read the NV/EFS item if its required
  -------------------------------------------------------------------------*/
  if(is_item_read == TRUE)
  {
    /* Clear the NV cache prior to the read */
    memset(item_params_ptr->cache_data_ptr,
           0x00,
           item_params_ptr->cache_data_len);

    /*-------------------------------------------------------------------------
       If NV based item, read from NV and update cache status
    -------------------------------------------------------------------------*/
    if(item_params_ptr->is_item_nv)
    {
      gstk_status = gstk_nv_item_read_write(
                      NV_READ_F,
                      (nv_items_enum_type)item_params_ptr->item_num,
                      &nv_item,
                      (uint16)item_params_ptr->nv_index);

      if(gstk_status == GSTK_SUCCESS)
      {
        *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_INIT;
      }
      else
      {
        *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_FOUND;
      }

      /* Reset status to Success */
      gstk_status = GSTK_SUCCESS;
    }
    /*-------------------------------------------------------------------------
       If EFS based item, read from EFS and update cache status
    -------------------------------------------------------------------------*/
    else
    {
      uim_efs_status = uim_common_efs_read(
                         (uim_common_efs_item_type)item_params_ptr->item_num,
                         UIM_COMMON_EFS_ITEM_FILE_TYPE,
                         (uim_common_efs_context_type)item_params_ptr->nv_index,
                         (uint8*)item_params_ptr->cache_data_ptr,
                         item_params_ptr->cache_data_len);

      if(uim_efs_status == UIM_COMMON_EFS_SUCCESS)
      {
        *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_INIT;
      }
      else
      {
       *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_FOUND;
      }
    }
  }

  /*---------------------------------------------------------------------------
     Post-processing
     1. If the the item was attempted to be read (is_item_read is TRUE):
        a. If item was read and not found: update default value to cache
           if needed (for ex: NV_TOOLKIT_ENV_RETRY_FLAG_I defaults to FALSE)
        b. For NV based items successfully read (for ex: NV_UE_IMEI_I),
           since the item is read into a temporary variable, copy the value
           into the NV cache
           (unlike EFS based items where its read directly into the cache)
        a. If any processing is needed on the raw NV item value read,
            perform the same (for ex: rat_priority_config needs to be parsed)
     2. Copy the data from the NV cache into data_ptr (w.r.t. NV read request)
        if the pointer is not NULL
  ---------------------------------------------------------------------------*/
  switch (item_type)
  {
    case GSTK_NV_ME_TOOLKIT_ENV_RETRY_FLAG_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
        {
          *(boolean*)item_params_ptr->cache_data_ptr =
            nv_item.toolkit_env_retry_flag;
        }
        /* Write default value into cache if item not found in FS */
        else if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
        {
          *(boolean*)item_params_ptr->cache_data_ptr = FALSE;
        }
      }

      if(data_ptr != NULL)
      {
        data_ptr->env_retry = *(boolean*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA:
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        write_data.env_retry_ext.max_retries = GSTK_DEFAULT_NUM_OF_ENV_RETRY;
        write_data.env_retry_ext.duration = GSTK_ENV_RETRY_CMD_WAIT_TIME;
        gstk_memcpy(item_params_ptr->cache_data_ptr,
                    &(write_data.env_retry_ext),
                    sizeof(data_ptr->env_retry_ext),
                    item_params_ptr->cache_data_len,
                    sizeof(data_ptr->env_retry_ext));
      }
      
      if(data_ptr != NULL)
      {        
        gstk_memcpy(&(data_ptr->env_retry_ext),
                    item_params_ptr->cache_data_ptr,
                    item_params_ptr->cache_data_len,
                    sizeof(data_ptr->env_retry_ext),
                    item_params_ptr->cache_data_len);
      }
      break;      

    case GSTK_NV_ME_TP_CLIENTS:
      /* Write default value into cache if item not found in FS */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        *(uint32*)item_params_ptr->cache_data_ptr = 0;
      }

      if(data_ptr != NULL)
      {
        data_ptr->tp_clients = *(uint32*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG:
      /* Write default value into FS if the item is read and not found */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        write_data.proact_feat_enable =
          GSTK_FEATURE_ENABLE_MAX & ~GSTK_FEATURE_ENABLE_UICC_SERVER_MODE;

        (void)gstk_nv_access_int_write(
                item_type,
                item_params_ptr,
                &write_data);
      }

      if(data_ptr != NULL)
      {
        data_ptr->proact_feat_enable = *(uint32*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_ME_TAL_CONF:
#ifdef FEATURE_GSTK_TAL
      /* Only if EF item is found in file system, copy value into data
         buffer and return success */
      if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        if(data_ptr != NULL)
        {
          if(gstk_memcpy(data_ptr->tal_conf,
                         item_params_ptr->cache_data_ptr,
                         item_params_ptr->cache_data_len,
                         sizeof(data_ptr->tal_conf),
                         item_params_ptr->cache_data_len) <
             item_params_ptr->cache_data_len)
          {
            gstk_status = GSTK_ERROR;
          }
        }
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }
#else
      UIM_MSG_ERR_0("GSTK TAL not enabled");
      gstk_status = GSTK_ERROR;
#endif /* FEATURE_GSTK_TAL */
      break;

    case GSTK_NV_ME_REDIAL_CFG:
      /* Write default value into FS if the item is read and not found */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        write_data.redial_cfg =
          ESTK_DEFAULT_REDIAL_INTERVAL_MS;

        (void)gstk_nv_access_int_write(
                item_type,
                item_params_ptr,
                &write_data);
      }

      if(data_ptr != NULL)
      {
        data_ptr->redial_cfg = *(uint32*)item_params_ptr->cache_data_ptr;
      }
      break;

#ifdef FEATURE_UIM_TEST_FRAMEWORK
    #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

    case GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
        {
          *(boolean*)item_params_ptr->cache_data_ptr =
            nv_item.gprs_anite_gcf;
        }
        /* Write default value into cache if item not found in FS */
        else if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
        {
          *(boolean*)item_params_ptr->cache_data_ptr = FALSE;
        }
      }

      if(data_ptr != NULL)
      {
        data_ptr->gprs_anite_gcf = *(boolean*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_EXT_ME_QMI_CAT_MODE:
      /* Write default value (CATI_CONFIG_MODE_DECODED) into cache if item
         not found in FS */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        *(uint8*)item_params_ptr->cache_data_ptr = CATI_CONFIG_MODE_DECODED;
      }

      if(data_ptr != NULL)
      {
        data_ptr->qmi_cat_mode = *(uint8*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_EXT_ME_GCF_GSTK:
      /* Write default value into cache if item not found in FS */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        *(boolean*)item_params_ptr->cache_data_ptr = FALSE;
      }

      if(data_ptr != NULL)
      {
        data_ptr->gcf_gstk = *(boolean*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_EXT_ME_ESN_ME_I:
      /* Copy read NV value from temp. variable into NV cache */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        (void)gstk_memcpy(item_params_ptr->cache_data_ptr,
                          &nv_item.esn,
                          GSTK_ESN_LEN,
                          item_params_ptr->cache_data_len,
                          sizeof(nv_item.esn));
      }

     /* Only if NV item is found in file system, copy value into data
         buffer and return success */
      if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        if(data_ptr != NULL)
        {
          if(gstk_memcpy(data_ptr->esn_me,
                         item_params_ptr->cache_data_ptr,
                         item_params_ptr->cache_data_len,
                         sizeof(data_ptr->esn_me),
                         item_params_ptr->cache_data_len) <
             item_params_ptr->cache_data_len)
          {
            gstk_status = GSTK_ERROR;
          }
        }
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }
      break;

    case GSTK_NV_EXT_ME_MEID_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        gstk_nv_access_post_process_meid(nv_item);
      }

      /* Only if NV item is found in file system, copy value into data
               buffer and return success */
      if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        if(data_ptr != NULL)
        {
          if(gstk_memcpy(data_ptr->meid,
                         item_params_ptr->cache_data_ptr,
                         item_params_ptr->cache_data_len,
                         sizeof(data_ptr->meid),
                         item_params_ptr->cache_data_len) <
             item_params_ptr->cache_data_len)
          {
            gstk_status = GSTK_ERROR;
          }
        }
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }
      break;

    case GSTK_NV_EXT_ME_QMI_CAT_DEFAULT_LANG:
      /* Write default value into cache if item not found in FS */
      if(is_item_read &&
          *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        ((uint8*)item_params_ptr->cache_data_ptr)[0] = 0x65; /* ASCII for 'e' */
        ((uint8*)item_params_ptr->cache_data_ptr)[1] = 0x6E; /* ASCII for 'n' */
      }

      if(data_ptr != NULL)
      {
        data_ptr->qmi_cat_default_lang[0] = ((uint8*)item_params_ptr->cache_data_ptr)[0];
        data_ptr->qmi_cat_default_lang[1] = ((uint8*)item_params_ptr->cache_data_ptr)[1];
      }
      break;

    case GSTK_NV_ME_FEATURE_BMASK:
      /* Write default value into cache if item not found in FS */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        *(uint64*)item_params_ptr->cache_data_ptr = 0;
      }

      if(data_ptr != NULL)
      {
        data_ptr->me_spec_feature_bmask = *(uint64*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SLOT_CAT_VERSION:
      /* Write default value into cache if item not found in FS */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        *(uint8*)item_params_ptr->cache_data_ptr = GSTK_CFG_CAT_VER8;
      }

      if(data_ptr != NULL)
      {
        data_ptr->cat_version = *(uint8*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SLOT_FEATURE_BMASK:
      /* If the NV item was read, perform post-processing */
      if(is_item_read)
      {
        gstk_nv_access_post_process_feature_bmask(item_params_ptr->cache_index);
      }

      if(data_ptr != NULL)
      {
        data_ptr->feature_bmask = *(uint32*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SLOT_FEATURE_BMASK_2:
      /* Write default value into cache if item not found in FS */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        *(uint8*)item_params_ptr->cache_data_ptr = 0;
      }

      if(data_ptr != NULL)
      {
        data_ptr->feature_bmask_2 = *(uint32*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SLOT_TOOLKIT_CS_PS_PARALLEL_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
        {
          *(boolean*)item_params_ptr->cache_data_ptr =
            nv_item.toolkit_cs_ps_parallel;
        }
        /* Write default value into cache if item not found in FS */
        else if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
        {
          *(boolean*)item_params_ptr->cache_data_ptr = FALSE;
        }
      }

      if(data_ptr != NULL)
      {
        data_ptr->cs_ps_parallel = *(boolean*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SLOT_RAT_PRIORITY_CONFIG:
      gstk_status = gstk_nv_access_post_process_rat_prio(
                      item_params_ptr->cache_index,
                      is_item_read,
                      data_ptr);
      break;

    case GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE:
      gstk_status = gstk_nv_access_post_process_bip_apn_mapping_tbl(
                      item_params_ptr->cache_index,
                      data_ptr);
      break;

    case GSTK_NV_SLOT_DEFAULT_APN:
      /* Only if EF item is found in file system, copy value into data
         buffer and return success */
      if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        if((data_ptr != NULL) && (data_ptr->default_apn_ptr != NULL))
        {
          memset(data_ptr->default_apn_ptr->apn,
                 0x00,
                 sizeof(data_ptr->default_apn_ptr->apn));

          if(gstk_memcpy(data_ptr->default_apn_ptr->apn,
                         item_params_ptr->cache_data_ptr,
                         (item_params_ptr->cache_data_len),
                         sizeof(data_ptr->default_apn_ptr->apn),
                         (item_params_ptr->cache_data_len)) <
             (item_params_ptr->cache_data_len))
          {
            gstk_status = GSTK_ERROR;
          }
        }
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }
      break;

    case GSTK_NV_EXT_SLOT_UE_IMEI_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        gstk_nv_access_post_process_ue_imei(item_params_ptr->cache_index,
                                            nv_item);
      }

      /* Only if NV item is found in file system, copy value into data
               buffer and return success */
      if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        if(data_ptr != NULL)
        {
          if(gstk_memcpy(data_ptr->ue_imei,
                         item_params_ptr->cache_data_ptr,
                         item_params_ptr->cache_data_len,
                         sizeof(data_ptr->ue_imei),
                         item_params_ptr->cache_data_len) <
             item_params_ptr->cache_data_len)
          {
            gstk_status = GSTK_ERROR;
          }
        }
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }

      /* Clear IMEI Cache if caching IMEI and IMEI_SVN is disabled */
#ifndef FEATURE_GSTK_NV_CACHE_IMEI
      memset(item_params_ptr->cache_data_ptr, 0x00, item_params_ptr->cache_data_len);
      *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_INIT;
#endif /* FEATURE_GSTK_NV_CACHE_IMEI */
      break;

    case GSTK_NV_EXT_SLOT_UE_IMEISVN_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        gstk_nv_access_post_process_ue_imeisvn(item_params_ptr->cache_index,
                                               nv_item);
      }

      /* Only if NV item is found in file system, copy value into data
               buffer and return success */
      if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
      {
        if(data_ptr != NULL)
        {
          if(gstk_memcpy(data_ptr->ue_imeisvn,
                         item_params_ptr->cache_data_ptr,
                         item_params_ptr->cache_data_len,
                         sizeof(data_ptr->ue_imeisvn),
                         item_params_ptr->cache_data_len) <
             item_params_ptr->cache_data_len)
          {
            gstk_status = GSTK_ERROR;
          }
        }
      }
      else
      {
        gstk_status = GSTK_ERROR;
      }

      /* Clear IMEI_SVN Cache if caching IMEI and IMEI_SVN is disabled */
#ifndef FEATURE_GSTK_NV_CACHE_IMEI
      memset(item_params_ptr->cache_data_ptr, 0x00, item_params_ptr->cache_data_len);
      *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_INIT;
#endif /* FEATURE_GSTK_NV_CACHE_IMEI */
      break;

    case GSTK_NV_SUB_BIP_OTASP_CONCURRENCY:
      if(is_item_read)
      {
        /* Reset block variables */
        if(gstk_instances_ptr[item_params_ptr->cache_index] != NULL)
        {
          gstk_instances_ptr[item_params_ptr->cache_index]->bip_blocked = FALSE;
          gstk_instances_ptr[item_params_ptr->cache_index]->otasp_blocked = FALSE;
        }
        else
        {
          UIM_MSG_ERR_1("gstk_instances_ptr[%d] is NULL",
                        item_params_ptr->cache_index);
          return GSTK_BAD_PARAM;
        }

        /* Write default value into cache if item not found in FS */
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
        {
          *(boolean*)item_params_ptr->cache_data_ptr = FALSE;
        }
      }

      if(data_ptr != NULL)
      {
        data_ptr->bip_otasp_concurrency = *(boolean*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_SUB_ATTACH_CTRL_TIMER:
      /* Write default value into FS if the item is read and not found */
      if(is_item_read &&
         *item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
      {
        write_data.attach_ctrl_timer_val =
          GSTK_ATTACH_CTRL_TIME;

        (void)gstk_nv_access_int_write(
                item_type,
                item_params_ptr,
                &write_data);
      }

      if(data_ptr != NULL)
      {
        data_ptr->attach_ctrl_timer_val = *(uint32*)item_params_ptr->cache_data_ptr;
      }
      break;

    case GSTK_NV_EXT_SUB_DTM_FEATURE_SUPPORT_I:
      if(is_item_read)
      {
        /* Copy read NV value from temp. variable into NV cache */
        if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_INIT)
        {
          *(boolean*)item_params_ptr->cache_data_ptr =
            nv_item.dtm_feature_support;
        }
        /* Write default value into cache if item not found in FS */
        else if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
        {
          *(boolean*)item_params_ptr->cache_data_ptr = TRUE;
        }
      }

      if(data_ptr != NULL)
      {
        data_ptr->dtm_feat_support = *(boolean*)item_params_ptr->cache_data_ptr;
      }
      break;

    default:
      UIM_MSG_ERR_1("Unknown NV/EFS item 0x%x", item_type);
      gstk_status = GSTK_BAD_PARAM;
  }

  return gstk_status;
} /* gstk_nv_access_int_read */

/*===========================================================================
FUNCTION gstk_nv_access_int_write

DESCRIPTION
     This function writes into the NV/EFS item requested and updates the
     NV cache

PARAMETERS:
  item_type:        [Input] NV/EFS item to be accesses
  op_categ:         [Input] Category of NV access
  item_params_ptr:  [Input/Output] NV/EFS parameters
  data_ptr:         [Input/Output] Data buffer

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
static gstk_status_enum_type gstk_nv_access_int_write(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_params_type        *item_params_ptr,
  gstk_nv_items_data_type    *data_ptr
)
{
  gstk_status_enum_type           gstk_status    = GSTK_SUCCESS;
  uim_common_efs_status_enum_type uim_efs_status = UIM_COMMON_EFS_ERROR;
  nv_item_type                    nv_item;

  if(item_params_ptr == NULL || data_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  if(item_params_ptr->cache_data_ptr == NULL ||
     item_params_ptr->cache_state_ptr == NULL)
  {
    UIM_MSG_ERR_2("cache_data_ptr: 0x%x or cache_state_ptr: 0x%x is NULL",
                  item_params_ptr->cache_data_ptr,
                  item_params_ptr->cache_state_ptr);
    return GSTK_BAD_PARAM;
  }

  memset(&nv_item, 0x00, sizeof(nv_item));

  /*-------------------------------------------------------------------------
     Pre-processing before write operation (retrieve value to be
     written from input buffer and update the NV cache with the value)
  -------------------------------------------------------------------------*/
  switch (item_type)
  {
    case GSTK_NV_ME_TP_CLIENTS:
      *(uint32*)item_params_ptr->cache_data_ptr = data_ptr->tp_clients;
      break;

    case GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG:
      *(uint32*)item_params_ptr->cache_data_ptr = data_ptr->proact_feat_enable;
      break;

    case GSTK_NV_ME_REDIAL_CFG:
      *(uint32*)item_params_ptr->cache_data_ptr = data_ptr->redial_cfg;
      break;

    case GSTK_NV_SUB_ATTACH_CTRL_TIMER:
      *(uint32*)item_params_ptr->cache_data_ptr = data_ptr->attach_ctrl_timer_val;
      break;

    case GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I:
      *(boolean*)item_params_ptr->cache_data_ptr = data_ptr->gprs_anite_gcf;
      /* Since this is an NV based item, copy the value to be written into
         the temp. variable */
       nv_item.gprs_anite_gcf = data_ptr->gprs_anite_gcf;
      break;

    default:
      UIM_MSG_ERR_1("Write operation not supported for item_type: 0x%x",
                    item_type);
      return GSTK_BAD_PARAM;
  }

  /*-------------------------------------------------------------------------
     Write into NV for NV based item and update cache status
  -------------------------------------------------------------------------*/
  if(item_params_ptr->is_item_nv)
  {
    gstk_status = gstk_nv_item_read_write(
                    NV_WRITE_F,
                    (nv_items_enum_type)item_params_ptr->item_num,
                    &nv_item,
                    item_params_ptr->nv_index);

    if(gstk_status == GSTK_SUCCESS)
    {
      *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_INIT;
    }
    else
    {
      *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_FOUND;
    }

    /* Reset status to Success */
    gstk_status = GSTK_SUCCESS;
  }
  /*-------------------------------------------------------------------------
     Write into EFS for EFS based item and update cache status
  -------------------------------------------------------------------------*/
  else
  {
    uim_efs_status = uim_common_efs_write(
                       (uim_common_efs_item_type)item_params_ptr->item_num,
                       UIM_COMMON_EFS_ITEM_FILE_TYPE,
                       (uim_common_efs_context_type)item_params_ptr->nv_index,
                       item_params_ptr->cache_data_ptr,
                       item_params_ptr->cache_data_len);

    if(uim_efs_status == UIM_COMMON_EFS_SUCCESS)
    {
      *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_INIT;
    }
    else
    {
      *item_params_ptr->cache_state_ptr = GSTK_NV_STATE_NOT_FOUND;
    }
  }
  /*-------------------------------------------------------------------------
     Post-processing after write operation (set default values if write
     operation failed)
  -------------------------------------------------------------------------*/
  if(*item_params_ptr->cache_state_ptr == GSTK_NV_STATE_NOT_FOUND)
  {
    switch (item_type)
    {
      case GSTK_NV_ME_TP_CLIENTS:
        *(uint32*)item_params_ptr->cache_data_ptr = 0;
        break;

      case GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG:
        *(uint32*)item_params_ptr->cache_data_ptr = GSTK_FEATURE_ENABLE_MAX &
                                                    ~GSTK_FEATURE_ENABLE_UICC_SERVER_MODE;
        break;

      case GSTK_NV_ME_REDIAL_CFG:
        *(uint32*)item_params_ptr->cache_data_ptr = ESTK_DEFAULT_REDIAL_INTERVAL_MS;
        break;

      case GSTK_NV_SUB_ATTACH_CTRL_TIMER:
        /* If VPE feature is disabled, attach control timer is disabled */
        if(gstk_nv_get_feature_status(
             GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
             (gstk_slot_id_enum_type)(item_params_ptr->cache_index + 1) == TRUE))
        {
          *(uint32*)item_params_ptr->cache_data_ptr = GSTK_ATTACH_CTRL_TIME;
        }
        else
        {
          *(uint32*)item_params_ptr->cache_data_ptr = 0;
        }
        break;

      case GSTK_NV_EXT_ME_GPRS_ANITE_GCF_I:
        *(boolean*)item_params_ptr->cache_data_ptr = FALSE;
        break;

      default:
        UIM_MSG_ERR_1("Write operation not supported for item_type: 0x%x",
                      item_type);
        gstk_status = GSTK_BAD_PARAM;
    }
  }

  return gstk_status;
} /* gstk_nv_access_int_write */

/*===========================================================================
FUNCTION gstk_nv_access_int

DESCRIPTION
  This function accesses the NV/EFS item requested

PARAMETERS:
  item_type:[Input] NV/EFS item to be accesses
  op_categ: [Input] Category of NV access
  slot_id:  [Input] Slot ID on which access is requested
  sub_id:   [Input] Subscription ID on which access is requested
  data_ptr: [Input/Output] Data buffer

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
static gstk_status_enum_type gstk_nv_access_int(
  gstk_nv_items_enum_type     item_type,
  gstk_nv_op_categ_enum_type  op_categ,
  gstk_slot_id_enum_type      slot_id,
  sys_modem_as_id_e_type      sub_id,
  gstk_nv_items_data_type    *data_ptr
)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  gstk_nv_params_type   item_params;

  /* Retrieve the paramters corresponding to the NV/EFS item */
  gstk_status = gstk_nv_access_int_get_item_params(item_type,
                                                   op_categ,
                                                   slot_id,
                                                   sub_id,
                                                   &item_params);

  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* Perform the NV access operation */
  if(op_categ == GSTK_NV_OP_CATEG_ACCESS_WRITE)
  {
    gstk_status = gstk_nv_access_int_write(item_type,
                                           &item_params,
                                           data_ptr);
  }
  else
  {
    gstk_status = gstk_nv_access_int_read(item_type,
                                          op_categ,
                                          &item_params,
                                          data_ptr);
  }

  /* If Full logging is enabled, or its an NV write operation or when NV is
     attempted to be read from file system - log the NV access */
  if((gstk_nv_cache.me_items.me_spec_feature_bmask.value & GSTK_CFG_FEATURE_ME_ENABLE_FULL_NV_LOGGING) ||
     (op_categ == GSTK_NV_OP_CATEG_ACCESS_WRITE) ||
     (item_params.is_item_read_from_fs == TRUE))
  {
    UIM_MSG_HIGH_6("gstk_nv_access_int, item_type: 0x%x, op_categ: 0x%x, slot_id: 0x%x, sub_id: 0x%x, data_ptr: 0x%x, status: 0x%x",
                   item_type, op_categ, slot_id, sub_id, data_ptr, gstk_status);

    if (gstk_status != GSTK_SUCCESS)
    {
      gstk_nv_dump_cache_info(&item_params,FALSE);
    }
    else
    {
      /* Dump the NV/EFS item params post access */
      gstk_nv_dump_cache_info(&item_params,TRUE);
    }
  }

  return gstk_status;
} /* gstk_nv_access_int */

/*===========================================================================

                        GSTK NV CACHE INIT FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION gstk_nv_init_me_feature_bmask_cache

DESCRIPTION
  Function caches the ME specific sub-features of NV 65674 by ORing the
  values from each NV context of the item

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_init_me_feature_bmask_cache(void)
{
  uint32                  nv_index = 0;
  gstk_nv_items_data_type read_data;

  /* Derive ME based sub-feature values as composite of values from
     each slot context */
  for (nv_index = 0; nv_index < gstk_shared_data.gstk_num_of_instances; nv_index++)
  {
    gstk_nv_cache.me_items.me_feature_bmask.value |=
      gstk_nv_cache.slot_items_ptr[nv_index]->feature_bmask.value;
  }

  if(!(gstk_nv_cache.me_items.me_feature_bmask.value & GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2))
  {
    (void)gstk_nv_access_read(GSTK_NV_EXT_ME_QMI_CAT_MODE,
                              GSTK_SLOT_ID_MAX,
                              &read_data);

    /* If QMI CAT not disabled */
    if(read_data.qmi_cat_mode > 0)
    {
      gstk_nv_cache.me_items.me_feature_bmask.value |= GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2;
    }
  }

  gstk_nv_cache.me_items.me_feature_bmask.state = GSTK_NV_STATE_INIT;

  UIM_MSG_HIGH_2("GSTK NV me_feature_bmask, value: 0x%x, state: 0x%x",
                 gstk_nv_cache.me_items.me_feature_bmask.value,
                 gstk_nv_cache.me_items.me_feature_bmask.state);
} /* gstk_nv_init_me_feature_bmask_cache */

/*===========================================================================
FUNCTION gstk_nv_init_cache

DESCRIPTION
  This function triggers the initalization for the requested range of NV items

PARAMETERS:
  item_type_start: [Input] First NV/EFS item to be initalized
  nv_item_count:   [Input] Number of NV/EFS items to be initalized
  op_categ:        [Input] Category of NV initalization
  slot_id:         [Input] Slot ID on which initalization is requested
  sub_id:          [Input] Subscription ID on which initalization is requested

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_init_cache(
  gstk_nv_items_enum_type    item_type_start,
  uint32                     nv_item_count,
  gstk_nv_op_categ_enum_type op_categ,
  gstk_slot_id_enum_type     slot_id,
  sys_modem_as_id_e_type     sub_id
)
{
  gstk_nv_items_enum_type item_type = GSTK_NV_MAX;
  uint32                  i         = 0;

  /* Initalize the GSTK NV cache for the range of NV items requested */
  for(i= 0; i < nv_item_count; i++)
  {
    item_type = (gstk_nv_items_enum_type)(item_type_start + i);

    (void)gstk_nv_access_int(item_type,
                             op_categ,
                             slot_id,
                             sub_id,
                             NULL);
  }
} /* gstk_nv_init_cache */

/*===========================================================================
FUNCTION gstk_nv_init_me_cache

DESCRIPTION
  This function triggers the initalization for the ME specific NV items

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_nv_init_me_cache(void)
{
  gstk_nv_items_enum_type me_item_start = GSTK_NV_MAX;
  uint32                  nv_item_count = 0;

  /* Read all ME based NV items owned by GSTK */
  me_item_start = GSTK_NV_ME_CATEG_INIT_NOREREAD_START + 1;
  nv_item_count = (uint32)(GSTK_NV_ME_CATEG_INIT_NOREREAD_END - me_item_start);

  gstk_nv_init_cache(me_item_start,
                     nv_item_count,
                     GSTK_NV_OP_CATEG_INIT,
                     GSTK_SLOT_ID_MAX,
                     SYS_MODEM_AS_ID_MAX);

  /* Read all ME based NV items not owned by GSTK */
  me_item_start = GSTK_NV_EXT_ME_CATEG_INIT_NOREREAD_START + 1;
  nv_item_count = (uint32)(GSTK_NV_EXT_ME_CATEG_INIT_NOREREAD_END - me_item_start);

  gstk_nv_init_cache(me_item_start,
                     nv_item_count,
                     GSTK_NV_OP_CATEG_INIT,
                     GSTK_SLOT_ID_MAX,
                     SYS_MODEM_AS_ID_MAX);
} /* gstk_nv_init_me_cache */

/*===========================================================================
FUNCTION gstk_nv_init_slot_cache

DESCRIPTION
  This function triggers the initalization for the slot specific NV items

PARAMETERS:
  init_categ: [Input] Category of Slot NV cache initalization
  slot_id:    [Input] Slot ID on which initalization is requested

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_init_slot_cache(
  gstk_nv_init_categ_enum_type init_categ,
  gstk_slot_id_enum_type       slot_id
)
{
  gstk_nv_items_enum_type    slot_item_start = GSTK_NV_MAX;
  uint32                     nv_item_count   = 0;
  gstk_nv_op_categ_enum_type op_categ;

  switch(init_categ)
  {
    case GSTK_NV_INIT_CATEG_POWER_UP_INIT:
      op_categ = GSTK_NV_OP_CATEG_INIT;
      break;
    case GSTK_NV_INIT_CATEG_NV_REFRESH:
      op_categ = GSTK_NV_OP_CATEG_NV_REFRESH;
      break;
    case GSTK_NV_INIT_CATEG_NV_REFRESH_NON_STANDARD:
      op_categ = GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD;
      break;
  }

  /* Attempt to re-read all slot based NV/EFS items in init. category */
  if((op_categ == GSTK_NV_OP_CATEG_INIT) ||
     (op_categ == GSTK_NV_OP_CATEG_NV_REFRESH) ||
     (op_categ == GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD))

  {
    slot_item_start = GSTK_NV_SLOT_CATEG_INIT_NOREREAD_START + 1;
    nv_item_count = (uint32)(GSTK_NV_SLOT_CATEG_INIT_NOREREAD_END - slot_item_start);

    gstk_nv_init_cache(slot_item_start,
                       nv_item_count,
                       op_categ,
                       slot_id,
                       SYS_MODEM_AS_ID_MAX);

    slot_item_start = GSTK_NV_EXT_SLOT_CATEG_INIT_NOREREAD_START + 1;
    nv_item_count = (uint32)(GSTK_NV_EXT_SLOT_CATEG_INIT_NOREREAD_END - slot_item_start);

    gstk_nv_init_cache(slot_item_start,
                       nv_item_count,
                       op_categ,
                       slot_id,
                       SYS_MODEM_AS_ID_MAX);
  }

  /* Attempt to clear the cache for all slot based NV/EFS items in on-demand read category */
  if((op_categ == GSTK_NV_OP_CATEG_NV_REFRESH) ||
     (op_categ == GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD))

  {
    slot_item_start = GSTK_NV_SLOT_CATEG_ONDEMAND_REEAD_START + 1;
    nv_item_count = (uint32)(GSTK_NV_SLOT_CATEG_ONDEMAND_REEAD_END - slot_item_start);

    gstk_nv_init_cache(slot_item_start,
                       nv_item_count,
                       op_categ,
                       slot_id,
                       SYS_MODEM_AS_ID_MAX);

    slot_item_start = GSTK_NV_EXT_SLOT_CATEG_ONDEMAND_REEAD_START + 1;
    nv_item_count = (uint32)(GSTK_NV_EXT_SLOT_CATEG_ONDEMAND_REEAD_END - slot_item_start);

    gstk_nv_init_cache(slot_item_start,
                       nv_item_count,
                       op_categ,
                       slot_id,
                       SYS_MODEM_AS_ID_MAX);
  }
} /* gstk_nv_init_slot_cache */

/*===========================================================================
FUNCTION gstk_nv_init_sub_cache

DESCRIPTION
  This function triggers the initalization for the subscription specific
  NV items

PARAMETERS:
  init_categ:[Input] Category of Sub NV cache initalization
  slot_id:   [Input] Slot ID on which initalization is requested
  sub_id:    [Input] Subscription ID on which initalization is requested

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_init_sub_cache(
  gstk_nv_init_categ_enum_type init_categ,
  gstk_slot_id_enum_type       slot_id,
  sys_modem_as_id_e_type       sub_id
)
{
  gstk_nv_items_enum_type    sub_item_start = GSTK_NV_MAX;
  uint32                     nv_item_count  = 0;
  gstk_nv_op_categ_enum_type op_categ;

  switch(init_categ)
  {
    case GSTK_NV_INIT_CATEG_POWER_UP_INIT:
      op_categ = GSTK_NV_OP_CATEG_INIT;
      break;
    case GSTK_NV_INIT_CATEG_NV_REFRESH:
      op_categ = GSTK_NV_OP_CATEG_NV_REFRESH;
      break;
    case GSTK_NV_INIT_CATEG_NV_REFRESH_NON_STANDARD:
      op_categ = GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD;
      break;
  }

  /* Attempt to re-read all sub based NV/EFS items in init. category */
  if((op_categ == GSTK_NV_OP_CATEG_INIT) ||
     (op_categ == GSTK_NV_OP_CATEG_NV_REFRESH) ||
     (op_categ == GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD))

  {
    sub_item_start = GSTK_NV_SUB_CATEG_INIT_NOREREAD_START + 1;
    nv_item_count = (uint32)(GSTK_NV_SUB_CATEG_INIT_NOREREAD_END - sub_item_start);

    gstk_nv_init_cache(sub_item_start,
                       nv_item_count,
                       op_categ,
                       slot_id,
                       sub_id);
  }

  /* Attempt to clear the cache for all sub based NV/EFS items in on-demand read category */
  if((op_categ == GSTK_NV_OP_CATEG_NV_REFRESH) ||
     (op_categ == GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD))

  {
    sub_item_start = GSTK_NV_EXT_SUB_CATEG_ONDEMAND_REEAD_START + 1;
    nv_item_count = (uint32)(GSTK_NV_EXT_SUB_CATEG_ONDEMAND_REEAD_END - sub_item_start);

    gstk_nv_init_cache(sub_item_start,
                       nv_item_count,
                       op_categ,
                       slot_id,
                       sub_id);
  }
} /* gstk_nv_init_sub_cache */

/*===========================================================================
FUNCTION gstk_nv_init_task

DESCRIPTION
  This function triggers the initalization of ME and slot specific NV items during
  GSTK task initalization

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_init_task(void)
{
  uint32                 index   = 0;
  gstk_slot_id_enum_type slot_id = GSTK_SLOT_ID_MAX;

  memset(&gstk_nv_cache, 0x00, sizeof(gstk_nv_cache));

  /* Read all ME based NV items that need to be read during task init */
  gstk_nv_init_me_cache();

  /* For all valid slots... */
  for (index = 0; index < gstk_shared_data.gstk_num_of_instances; index++)
  {
    /* Allocate memory for Slot NV cache */
    gstk_nv_cache.slot_items_ptr[index] =
      (gstk_nv_slot_cache_data_type *) GSTK_CALLOC(sizeof(gstk_nv_slot_cache_data_type));
    if (gstk_nv_cache.slot_items_ptr[index] == NULL)
    {
      ERR_FATAL("Can't create GSTK slot NV instance 0x%x",
                index, 0, 0);
    }
    memset(gstk_nv_cache.slot_items_ptr[index],
           0x00,
           sizeof(gstk_nv_slot_cache_data_type));

    /* Allocate memory for Sub NV cache */
    gstk_nv_cache.sub_items_ptr[index] =
      (gstk_nv_sub_cache_data_type *) GSTK_CALLOC(sizeof(gstk_nv_sub_cache_data_type));
    if (gstk_nv_cache.sub_items_ptr[index] == NULL)
    {
      ERR_FATAL("Can't create GSTK sub NV instance 0x%x",
                index, 0, 0);
    }
    memset(gstk_nv_cache.sub_items_ptr[index],
           0x00,
           sizeof(gstk_nv_sub_cache_data_type));

    /* Read all Slot based NV items that need to be read during task init */
    slot_id = (gstk_slot_id_enum_type)(index + 1);
    gstk_nv_init_slot_cache(GSTK_NV_INIT_CATEG_POWER_UP_INIT,
                            slot_id);
  }

  /* Extract and cache ME specific NV 65674 sub-features */
  gstk_nv_init_me_feature_bmask_cache();
} /* gstk_nv_init_task */

/*===========================================================================

                        GSTK NV - MCFG INTERACTION FUNCTIONS

===========================================================================*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION gstk_nv_mcfg_refresh_cb

DESCRIPTION
  This function callback (reigstered with MCFG) queues the MCFG NV Refresh
  notification to GSTK

PARAMETERS:
  mcfg_refresh_info_ptr:[Input] NV refresh info from MCFG

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
static boolean gstk_nv_mcfg_refresh_cb(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
)
{
  gstk_task_cmd_type *task_cmd_ptr = NULL;

  UIM_MSG_HIGH_0("In gstk_nv_mcfg_refresh_cb()");

  if(mcfg_refresh_info_ptr == NULL)
  {
    UIM_MSG_ERR_0("mcfg_refresh_info_ptr is NULL");
    return FALSE;
  }

  task_cmd_ptr = gstk_task_get_cmd_buf();

  if(task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.subs_change_ind.message_header.command_group =
      GSTK_NV_REFRESH_CMD;

    /*store mcfg refresh data */
    task_cmd_ptr->cmd.nv_refresh_cmd.type         = (uint32)mcfg_refresh_info_ptr->type;
    task_cmd_ptr->cmd.nv_refresh_cmd.slot_index   = (uint32)mcfg_refresh_info_ptr->slot_index;
    task_cmd_ptr->cmd.nv_refresh_cmd.sub_index    = (uint32)mcfg_refresh_info_ptr->sub_index;
    task_cmd_ptr->cmd.nv_refresh_cmd.reference_id = mcfg_refresh_info_ptr->reference_id;

    gstk_task_put_cmd_priority_buf(task_cmd_ptr);
  }
  else
  {
    return FALSE;
  }
  return TRUE;
} /* gstk_nv_mcfg_refresh_cb */

/*===========================================================================
FUNCTION gstk_nv_mcfg_process_refresh

DESCRIPTION

  This function processes the NV refresh command from MCFG

PARAMETERS
  cmd_ptr: [Input] Pointer to the command to be processed

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
gstk_status_enum_type gstk_nv_mcfg_process_refresh(
  gstk_cmd_type *cmd_ptr
)
{
  gstk_nv_op_categ_enum_type op_categ = GSTK_NV_OP_CATEG_NV_REFRESH;
  gstk_slot_id_enum_type     slot_id  = GSTK_SLOT_ID_MAX;
  sys_modem_as_id_e_type     sub_id   = SYS_MODEM_AS_ID_MAX;

  if(cmd_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_4("NV refresh params: type: 0x%x, slot_index:0x%x, sub_index: 0x%x, reference_id: 0x%x",
                 cmd_ptr->nv_refresh_cmd.type,
                 cmd_ptr->nv_refresh_cmd.slot_index,
                 cmd_ptr->nv_refresh_cmd.sub_index,
                 cmd_ptr->nv_refresh_cmd.reference_id);

  switch (cmd_ptr->nv_refresh_cmd.slot_index)
  {
    case MCFG_REFRESH_INDEX_0:
      slot_id = GSTK_SLOT_1;
      break;
    case MCFG_REFRESH_INDEX_1:
      slot_id = GSTK_SLOT_2;
      break;
    case MCFG_REFRESH_INDEX_2:
      slot_id = GSTK_SLOT_3;
      break;
    default:
      (void)mcfg_refresh_done_w_status(cmd_ptr->nv_refresh_cmd.reference_id,
                                       MCFG_REFRESH_SUCCESS);
      return GSTK_BAD_PARAM;
  }

  if(gstk_instances_ptr[(uint32)slot_id - 1] == NULL)
  {
    (void)mcfg_refresh_done_w_status(cmd_ptr->nv_refresh_cmd.reference_id,
                                     MCFG_REFRESH_SUCCESS);
    return GSTK_BAD_PARAM;
  }

  /* If MMGSDI has already activated card apps, the NV refresh is non-standard */
  if(gstk_instances_ptr[(uint32)slot_id - 1]->apps_enabled != 0)
  {
    op_categ = GSTK_NV_OP_CATEG_NV_REFRESH_NON_STANDARD;
  }

  /* NV refresh procedure for Slot NV items */
  if (cmd_ptr->nv_refresh_cmd.type == MCFG_REFRESH_TYPE_SLOT ||
      cmd_ptr->nv_refresh_cmd.type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    gstk_nv_init_slot_cache((gstk_nv_init_categ_enum_type)op_categ,
                            slot_id);
  }

  /* NV refresh procedure for Subscription NV items */
  if (cmd_ptr->nv_refresh_cmd.type == MCFG_REFRESH_TYPE_SUBS ||
      cmd_ptr->nv_refresh_cmd.type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    switch (cmd_ptr->nv_refresh_cmd.sub_index)
    {
      case MCFG_REFRESH_INDEX_0:
        sub_id = SYS_MODEM_AS_ID_1;
        break;
      case MCFG_REFRESH_INDEX_1:
        sub_id = SYS_MODEM_AS_ID_2;
        break;
      case MCFG_REFRESH_INDEX_2:
        sub_id = SYS_MODEM_AS_ID_3;
        break;
      default:
        (void)mcfg_refresh_done_w_status(cmd_ptr->nv_refresh_cmd.reference_id,
                                         MCFG_REFRESH_SUCCESS);
        return GSTK_BAD_PARAM;
    }

    gstk_nv_init_sub_cache((gstk_nv_init_categ_enum_type)op_categ,
                           slot_id,
                           sub_id);
  }

  /* Notify MCFG that the NV Refresh procedure is done */
  if(mcfg_refresh_done_w_status(cmd_ptr->nv_refresh_cmd.reference_id,
                                MCFG_REFRESH_SUCCESS))
  {
    return GSTK_SUCCESS;
  }
  else
  {
    return GSTK_ERROR;
  }
} /* gstk_nv_mcfg_process_refresh */

/*===========================================================================
FUNCTION gstk_nv_mcfg_refresh_register

DESCRIPTION
  This function registers for NV refresh notification with MCFG

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_nv_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type mcfg_reg = {0};
  boolean                          reg_status = FALSE;

  /* MCFG Registration */
  mcfg_reg.cb = gstk_nv_mcfg_refresh_cb;
  /* RFU - defaulting it to 0*/
  mcfg_reg.group = 0;
  mcfg_reg.order = MCFG_REFRESH_ORDER_50;
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;

  reg_status = mcfg_refresh_register(&mcfg_reg);

  UIM_MSG_HIGH_1("MCFG Registration status: 0x%x", reg_status);
} /* gstk_nv_mcfg_refresh_register */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

/*===========================================================================

                        GSTK NV ITEM ACCESS FUNCTIONS

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
)
{
  gstk_nv_items_data_type read_data;

  if((proactive_cmd_enable_ptr == NULL)||
     (proactive_cmd >= GSTK_FEATURE_ENABLE_MAX))
  {
    UIM_MSG_ERR_2("ERR: gstk_nv_get_proactive_feature_enable, proactive_cmd_enable_ptr: 0x%x, proactive_cmd: 0x%x",
                  proactive_cmd_enable_ptr, proactive_cmd);
    return GSTK_ERROR;
  }

  (void)gstk_nv_access_read(
          GSTK_NV_ME_PROACTIVE_FEATURE_ENABLE_CFG,
          GSTK_SLOT_ID_MAX,
          &read_data);

  if((read_data.proact_feat_enable) & proactive_cmd)
  {
    *proactive_cmd_enable_ptr = TRUE;
  }
  else
  {
    *proactive_cmd_enable_ptr = FALSE;
  }

  return GSTK_SUCCESS;
} /* gstk_nv_get_proactive_feature_enable */

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
  gstk_nv_cfg_features_enum_type mask_value,
  gstk_slot_id_enum_type         slot_id
)
{
  boolean ret = FALSE;

  switch(mask_value)
  {
    /* ME specific sub-features */
    /*case GSTK_CFG_FEATURE_ME_ESTK_QMI:*/
    /*case GSTK_CFG_FEATURE_ME_ESTK_ICON:*/
    case GSTK_CFG_FEATURE_ME_DATA_CARD:
    case GSTK_CFG_FEATURE_ME_GSTK_CUST_ENH0:
    /*case GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS:*/
    case GSTK_CFG_FEATURE_ME_ESTK_IGNORE_ALPHA:
    case GSTK_CFG_FEATURE_ME_DYNAMIC_TP_CLIENTS:
    case GSTK_CFG_FEATURE_ME_REPORT_SW1_SW2:
    case GSTK_CFG_FEATURE_ME_TP_IMS_ENABLE:
    case GSTK_CFG_FEATURE_ME_TRANSCODING_ENABLE:
    case GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_VOICE_CALL_INTERFACE:
    case GSTK_CFG_FEATURE_ME_DISABLE_ESTK_CM_SUPPS_INTERFACE:
    case GSTK_CFG_FEATURE_ME_DISABLE_ESTK_WMS_INTERFACE:
    case GSTK_CFG_FEATURE_ME_DISABLE_ESTK_BIP_INTERFACE:
    case GSTK_CFG_FEATURE_ME_ESTK_QMI_SEND_NO_AND_NULL_ALPHA_IND:
      if(gstk_nv_cache.me_items.me_feature_bmask.value & mask_value)
      {
        ret = TRUE;
      }
      break;

    /* Slot specific sub-features */
    case GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH:
    case GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS:
    case GSTK_CFG_FEATURE_SLOT_ESTK_BIP_IP_DEFAULT:
    case GSTK_CFG_FEATURE_SLOT_ENH_RAT_AND_LOC_EVT:
    case GSTK_CFG_FEATURE_SLOT_SETUP_CALL_REL7:
    case GSTK_CFG_FEATURE_SLOT_TR_ADD_INFO_BYTE:
    case GSTK_CFG_FEATURE_SLOT_ESTK_BIP_OVER_WLAN:
    case GSTK_CFG_FEATURE_SLOT_ESTK_BIP_WLAN_PRIO:
    case GSTK_CFG_FEATURE_SLOT_CUST_TP_25_BYTE:
    case GSTK_CFG_FEATURE_SLOT_STOP_ENV_CMD_PROCESSING:
    /*case GSTK_CFG_FEATURE_SLOT_STOP_SMS_PP_ON_PENDING_TR:*/
    case GSTK_CFG_FEATURE_SLOT_ESTK_ENABLE_BIP_TCP_CONT_READ:
    case GSTK_CFG_FEATURE_SLOT_DISABLE_CALL_AND_SMS_CTRL_RETRY:
    case GSTK_CFG_FEATURE_SLOT_MODE_SWITCH_STK_SWITCH:
    case GSTK_CFG_FEATURE_SLOT_LTD_SVC_WITH_NW_REJ_ONLY:
    case GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO:
      if((!GSTK_IS_VALID_SLOT_ID(slot_id)) ||
         (gstk_nv_cache.slot_items_ptr[(uint8)(slot_id - 1)] == NULL))
      {
        UIM_MSG_ERR_2("gstk_nv_get_feature_status, mask: 0x%x, invalid slot id 0x%x",
                      mask_value, slot_id);
        return FALSE;
      }

      if(gstk_nv_cache.slot_items_ptr[(uint8)(slot_id - 1)]->feature_bmask.value & mask_value)
      {
        ret = TRUE;
      }
      break;

    default:
      ret = FALSE;
      break;
  }

  UIM_MSG_HIGH_3("gstk_nv_get_feature_status, mask: 0x%x, slot id: 0x%x, value: 0x%x",
                 mask_value, slot_id, ret);

  return ret;
} /* gstk_nv_get_feature_status */

/*===========================================================================
FUNCTION GSTK_NV_GET_FEATURE_2_STATUS

DESCRIPTION
  Function used to get the status of one of the GSTK features bmask 2
  listed in gstk_nv_cfg_features_2_enum_type

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
  gstk_nv_cfg_features_2_enum_type mask_value,
  gstk_slot_id_enum_type         slot_id
)
{
  boolean ret = FALSE;

  switch(mask_value)
  {
    /* Slot specific sub-features */
    case GSTK_CFG_FEATURE_2_SLOT_BLOCK_CALL_CTRL_DURING_SEND_RECEIVE_DATA:
      if((!GSTK_IS_VALID_SLOT_ID(slot_id)) ||
         (gstk_nv_cache.slot_items_ptr[(uint8)(slot_id - 1)] == NULL))
      {
        UIM_MSG_ERR_2("gstk_nv_get_feature_2_status, mask: 0x%x, invalid slot id 0x%x",
                      mask_value, slot_id);
        return FALSE;
      }

      if(gstk_nv_cache.slot_items_ptr[(uint8)(slot_id - 1)]->feature_bmask_2.value & mask_value)
      {
        ret = TRUE;
      }
      break;

    default:
      ret = FALSE;
      break;
  }

  UIM_MSG_HIGH_3("gstk_nv_get_feature_2_status, mask: 0x%x, slot id: 0x%x, value: 0x%x",
                 mask_value, slot_id, ret);

  return ret;
} /* gstk_nv_get_feature_2_status */

/*===========================================================================
FUNCTION GSTK_NV_GET_ME_FEATURE_STATUS

DESCRIPTION
  Function used to get the status of one of the GSTK ME specific features 
  bmask listed in gstk_nv_cfg_me_features_enum_type

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
  gstk_nv_cfg_me_features_enum_type mask_value
)
{
  boolean ret = FALSE;

  switch(mask_value)
  {
    /* ME specific sub-features */
    case GSTK_CFG_FEATURE_ME_DISABLE_PCMD_SELECTIVE_PARALLEL_PROCESSING:
    case GSTK_CFG_FEATURE_ME_DISABLE_DUP_ERR_TR_DELAY_PCMD_PARALLEL_PROCESSING:
    case GSTK_CFG_FEATURE_ME_ENABLE_FULL_NV_LOGGING:      
      if(gstk_nv_cache.me_items.me_spec_feature_bmask.value & mask_value)
      {
        ret = TRUE;
      }
      break;
    default:
      ret = FALSE;
      break;
  }

  UIM_MSG_HIGH_2("gstk_nv_get_me_feature_status, mask: 0x%x, value: 0x%x",
                 mask_value, ret);

  return ret;
} /* gstk_nv_get_me_feature_status */

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
)
{
  if(data_ptr == NULL)
  {
    UIM_MSG_ERR_0("gstk_nv_access_read, data_ptr is NULL");
    return GSTK_BAD_PARAM;
  }

  /* Since data elements corresponding to BIP APN mapping table
     and Default APN NV items are pointers, do not memset for these */
  if((item_type != GSTK_NV_SLOT_BIP_APN_MAPPING_TABLE) &&
     (item_type != GSTK_NV_SLOT_DEFAULT_APN))
  {
    memset(data_ptr, 0x00,sizeof(gstk_nv_items_data_type));
  }

  return gstk_nv_access_int(item_type,
                            GSTK_NV_OP_CATEG_ACCESS_READ,
                            slot_id,
                            SYS_MODEM_AS_ID_MAX,
                            data_ptr);
} /* gstk_nv_access_read */

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
)
{
  if(data_ptr == NULL)
  {
    UIM_MSG_ERR_0("gstk_nv_access_write, data_ptr is NULL");
    return GSTK_BAD_PARAM;
  }

  return gstk_nv_access_int(item_type,
                            GSTK_NV_OP_CATEG_ACCESS_WRITE,
                            slot_id,
                            SYS_MODEM_AS_ID_MAX,
                            data_ptr);
} /* gstk_nv_access_write */
