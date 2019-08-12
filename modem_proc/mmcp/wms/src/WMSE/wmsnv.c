/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsnv.c -- NV Operations

  This module provides the utility functions for the use by WMS for accessing
  NV items needed for SMS processing.

  -------------

    Copyright (c) 1999-2008,2010-2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsnv.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
EXTERNALIZED FUNCTIONS
  wms_nv_read_wait
    Read an item from NV, handle non-active items

  wms_nv_write_wait
    Write an item to NV
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "target.h"
#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include <memory.h>
#include <stdio.h>
#include "wmscl.h"
#include "wmsi.h"
#include "wmsnv.h"
#include "wmscfg.h"
#include "wmsmsgi.h"
#include "wmsts.h"
#include "wmsutils.h"
#include "err.h"
#include "nv.h"
#include "stringl.h"
#include "policyman.h"
#ifdef FEATURE_CDSMS
#include "wmstscdma.h"
#include "wmstsi.h"
#include "wmsdc.h"
#endif /* FEATURE_CDSMS */
#ifdef FEATURE_GWSMS_BROADCAST
#include "wmsbc.h"
#endif /* FEATURE_GWSMS_BROADCAST */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
#include "mcfg_fs.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_GWSMS
/*      --------------------------------------
 *      G W    E F S    F I L E    F O R M A T
 *      --------------------------------------
 *
 * SMS uses two files in EFS to store its messages, one to store
 * header information about storage and the other to store the actual data
 *
 *
 * SMS_NV_GW_MSG_STORE_MAJOR_VER
 *  - Version number of payload slot data format
 *
 * SMS_NV_GW_MSG_STORE_DATA
 *  - The actual SMS messages are stored in this file
 *
 * SMS_NV_GW_MSG_STORE_HEADER
 *  - Contains meta information.
 *    First 4 bytes - (Major version number) Incremented
 *                    everytime we change the payload data
 *    Next 4 bytes  - (Minor version number) Corresponds to
 *                    number of slots used to create SMS_NV_GW_MSG_DATA
 *    The rest of the header data is version dependent.
 *
 *    --------------------
 *    Version 1.xxx format
 *    --------------------
 *    First 4 bytes - 0x00000001
 *    Next 4 bytes  - Depends on value of max number of slots
 *    Next 4 bytes  - Bytes occupied by each individual slot
 *    Next n bytes  - Where "n" is the max number of slots.
 *                    Each byte corresponds to meta information about
 *                    the individual slot, which is the tag info.
 *
 * SMS_NV_GW_MSG_STORE_START_TAG / SMS_NV_GW_MSG_STORE_END_TAG
 *  - The actual data is wrapped between these tags
 *    which act as a basic protection against corruption.
 */

/* EFS Storage Version */
#define SMS_NV_GW_MSG_STORE_MAJOR_VER       0x00000001

/* Files used for storage in EFS */
#define SMS_NV_GW_MSG_STORE_DATA            "/sms/nv_gw_msg_data"
#define SMS_NV_GW_MSG_STORE_HEADER          "/sms/nv_gw_msg_header"

/* Tags around actual message in slots */
#define SMS_NV_GW_MSG_STORE_START_TAG       0xCA
#define SMS_NV_GW_MSG_STORE_END_TAG         0xFE

/* Slot consits of actual data and the start+end byte tag */
/* NV_SMS_GW_DATA_SIZ is defined in services/nv/nv_items.h */
#define SMS_NV_GW_MSG_STORE_SLOT_SIZ        (NV_SMS_GW_DATA_SIZ + 2)

#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*      --------------------------------------
 *      C D    E F S    F I L E    F O R M A T
 *      --------------------------------------
 *
 * SMS uses two files in EFS to store its messages, one to store
 * header information about storage and the other to store the actual data
 *
 *
 * SMS_NV_CD_MSG_STORE_MAJOR_VER
 *  - Version number of payload slot data format
 *
 * SMS_NV_CD_MSG_STORE_DATA
 *  - The actual SMS messages are stored in this file
 *
 * SMS_NV_CD_MSG_STORE_HEADER
 *  - Contains meta information.
 *    First 4 bytes - (Major version number) Incremented
 *                    everytime we change the payload data
 *    Next 4 bytes  - (Minor version number) Corresponds to
 *                    number of slots used to create SMS_NV_CD_MSG_DATA
 *    The rest of the header data is version dependent.
 *
 *    --------------------
 *    Version 1.xxx format
 *    --------------------
 *    First 4 bytes - 0x00000001
 *    Next 4 bytes  - Depends on value of max number of slots
 *    Next 4 bytes  - Bytes occupied by each individual slot
 *    Next n bytes  - Where "n" is the max number of slots.
 *                    Each byte corresponds to meta information about
 *                    the individual slot, which is the tag info.
 *
 * SMS_NV_CD_MSG_STORE_START_TAG / SMS_NV_CD_MSG_STORE_END_TAG
 *  - The actual data is wrapped between these tags
 *    which act as a basic protection against corruption.
 */

/* EFS Storage Version */
#define SMS_NV_CD_MSG_STORE_MAJOR_VER       0x00000001

/* Files used for storage in EFS */
#define SMS_NV_CD_MSG_STORE_DATA            "/sms/nv_cd_msg_data"
#define SMS_NV_CD_MSG_STORE_HEADER          "/sms/nv_cd_msg_header"

/* Tags around actual message in slots */
#define SMS_NV_CD_MSG_STORE_START_TAG       0xCA
#define SMS_NV_CD_MSG_STORE_END_TAG         0xFE

/* Slot consits of actual data and the start+end byte tag */
/* NV_SMS_DATA_SIZ is defined in services/nv/nv_items.h */
#define SMS_NV_CD_MSG_STORE_SLOT_SIZ        (NV_SMS_DATA_SIZ + 2)

static wms_dc_s_type                *dc_s_ptr;

#endif /* FEATURE_CDSMS */

/* Maximum length of file name for EFS files  (including full path)-
** to be increased if filepath length increases
*/
#define WMS_NV_EFS_FILENAME_MAX_LEN         60

/* Structure for EFS table
*/
typedef struct
{
   wms_nv_efs_id_e_type           efs_id;       /* EFS id */
   const char                     file_name[WMS_NV_EFS_FILENAME_MAX_LEN];    /* EFS file name */
} wms_nv_efs_tbl_entry_s_type;

#ifdef FEATURE_TRIPLE_SIM
static struct
{
  boolean has_been_read[MAX_AS_IDS];
  int     nv_value[MAX_AS_IDS];
} wms_nv_refresh_item = {{FALSE, FALSE, FALSE},{-1, -1, -1}};
#else
static struct
{
  boolean has_been_read[MAX_AS_IDS];
  int     nv_value[MAX_AS_IDS];
} wms_nv_refresh_item = {{FALSE, FALSE},{-1, -1}};
#endif

/*
 ** WMS EFS config filename
 */
#define WMS_EFS_CONF_F_NAME (char*)"/nv/item_files/conf/wms.conf"

/*
 ** WMS EFS config items
 */
char *wms_config_items = "/nv/item_files/modem/sms/mo_on_data_channel\n\
/nv/item_files/modem/sms/enable_fdn_control\n\
/nv/item_files/modem/sms/store_to_sim_if_nv_full\n\
/nv/item_files/ims/qipcall_1xsmsandvoice\n\
/nv/item_files/modem/sms/telecom_smsp_fallback\n\
/nv/item_files/modem/sms/sms_retry_limit\n\
/nv/item_files/modem/sms/disable_lte_cb_dup_detection\n\
/nv/item_files/modem/sms/disable_pres_bc_alert\n\
/nv/item_files/modem/sms/cs_domain_fallback\n\
/nv/item_files/modem/sms/stk_sms_retry_enable\n\
/nv/item_files/modem/sms/mmgsdi_refresh_vote_ok\n\
/nv/item_files/modem/sms/max_mt_msgs_allowed_in_ecbm\n";

/*
** EFS table that maps EFS Id to filename
*/
static const wms_nv_efs_tbl_entry_s_type wms_nv_efs_tbl[] =
{
  { WMS_NV_EFS_MO_ON_DATA_CHANNEL,           "/nv/item_files/modem/sms/mo_on_data_channel" },
  { WMS_NV_EFS_ENABLE_FDN_CONTROL,           "/nv/item_files/modem/sms/enable_fdn_control" },
  { WMS_NV_EFS_STORE_TO_SIM_IF_NV_FULL,      "/nv/item_files/modem/sms/store_to_sim_if_nv_full" },
  { WMS_NV_EFS_1x_SMS_AND_VOICE,             "/nv/item_files/ims/qipcall_1xsmsandvoice" },
  { WMS_NV_EFS_TELECOM_SMSP_FALLBACK,        "/nv/item_files/modem/sms/telecom_smsp_fallback" },
  { WMS_NV_EFS_SMS_RETRY_LIMIT,              "/nv/item_files/modem/sms/sms_retry_limit" },
  { WMS_NV_EFS_DISABLE_LTE_CB_DUP_DETECTION, "/nv/item_files/modem/sms/disable_lte_cb_dup_detection" },
  { WMS_NV_EFS_DISABLE_PRES_BC_ALERT,        "/nv/item_files/modem/sms/disable_pres_bc_alert" },
  { WMS_NV_EFS_CS_DOMAIN_FALLBACK,           "/nv/item_files/modem/sms/cs_domain_fallback"},
  { WMS_NV_EFS_STK_SMS_RETRY_ENABLE,         "/nv/item_files/modem/sms/stk_sms_retry_enable"},
  { WMS_NV_EFS_MMGSDI_REFRESH_VOTE_OK,       "/nv/item_files/modem/sms/mmgsdi_refresh_vote_ok"},
  { WMS_NV_EFS_MAX_MT_ALLOWED_IN_ECBM,       "/nv/item_files/modem/sms/max_mt_msgs_allowed_in_ecbm" },
  { WMS_NV_EFS_ALLOW_SMS_IN_ECBM,            "/nv/item_files/modem/mmode/allow_sms_in_ecbm" }
};

static wms_cfg_s_type               *cfg_s_ptr;
static wms_msg_s_type               *msg_s_ptr;


/* NV command buffer */
static nv_cmd_type                   nvcmd;
/* Extended NV command struct for dual sim */
static nv_cmd_ext_type               nvcmd_ext;

/* Critical Section for WMS accessing NV */
rex_crit_sect_type                   wms_nv_crit_sect;

#ifdef FEATURE_CDSMS
wms_cdma_template_s_type            *cdma_template_ptr = NULL;
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS
wms_gw_template_s_type              *gw_template_ptr = NULL;
#endif /* FEATURE_GWSMS */
//TBD

                            /**************************/
                            /* Local Helper Functions */
                            /**************************/
#ifdef FEATURE_GWSMS
static void  wms_nv_determine_max_gw_slots          ( void );

static uint8 wms_nv_gw_msg_store_authenticate       ( void );
static uint8 wms_nv_gw_msg_store_is_version_valid   ( void );
static uint8 wms_nv_gw_msg_store_init               ( void );
static uint8 wms_nv_gw_msg_store_create_header_file ( void );
static uint8 wms_nv_gw_msg_store_create_data_file   ( void );
static uint8 wms_nv_gw_msg_store_update_header_file ( wms_message_index_type index,
                                                      uint8 tag );
static uint8 wms_nv_gw_msg_store_read_tags          ( void );
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
static uint8 wms_nv_cd_msg_store_authenticate       ( void );
static uint8 wms_nv_cd_msg_store_is_version_valid   ( void );
static uint8 wms_nv_cd_msg_store_init               ( void );
static uint8 wms_nv_cd_msg_store_create_header_file ( void );
static uint8 wms_nv_cd_msg_store_create_data_file   ( void );
static uint8 wms_nv_cd_msg_store_update_header_file ( wms_message_index_type index,
                                                      uint8 tag );
static uint8 wms_nv_cd_msg_store_read_tags          ( void );
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
boolean wms_nv_refresh_cb(
  mcfg_refresh_info_s_type *p_info
);
#endif

/*===========================================================================
FUNCTION WMS_NV_WAIT

DESCRIPTION
  Wait on NV actions.  Handle signals as they come in just in case NV
  takes a while.  Only time critical watchdog signals are acknowledged.

DEPENDENCIES
  There is a command sent to NV, and this task will be signalled.  There
  are no other outstanding NV requests from this task.

RETURN VALUE
  None

SIDE EFFECT
  Some signals are delayed till we're done with NV

===========================================================================*/
void wms_nv_wait
(
  void
)
{
  rex_sigs_type  sigs;

  for (;;)
  {                     /* until signal back from NV */
    sigs = rex_wait( WMS_NV_SIG | WMS_RPT_TIMER_SIG );

    if( sigs & WMS_RPT_TIMER_SIG )
    {
      wms_kick_dog();
    }

    if( sigs & WMS_NV_SIG )
    {
      (void)rex_clr_sigs( rex_self(), WMS_NV_SIG );
      break;
    }
  }

  return;

} /* end of wms_nv_wait() */

uint16 wms_nv_convert_to_context(sys_modem_as_id_e_type as_id)
{
  if (SYS_MODEM_AS_ID_1 == as_id)
  {
    return 0;
  }

  if (SYS_MODEM_AS_ID_2 == as_id)
  {
    return 1;
  }

  if (SYS_MODEM_AS_ID_3 == as_id)
  {
    return 2;
  }

  return 0;
}

/*===========================================================================
FUNCTION WMS_NV_WRITE_WAIT_PER_SUBS

DESCRIPTION
  Write an item to NV memory per as id.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_write_wait_per_subs
(
  sys_modem_as_id_e_type  as_id,
  nv_items_enum_type      item,        /* which item */
  nv_item_type           *data_ptr     /* pointer to data for item */
)
{
  nv_stat_enum_type  ret_status;

  /* Entering WMS NV Critical Section */
  rex_enter_crit_sect(&wms_nv_crit_sect);

  MSG_HIGH_1( "wms_nv_write_wait_per_subs, item=%d", item);

  /* set up NV item */
  nvcmd.tcb_ptr    = rex_self();        /* Synchronous NV Operation */
  nvcmd.sigs       = WMS_NV_SIG;
  nvcmd.done_q_ptr = NULL;

  nvcmd.cmd        = NV_WRITE_F;
  nvcmd.item       = item;             /* item to write */
  nvcmd.data_ptr   = data_ptr;        /* the data to write */

  nvcmd_ext.nvcmd   = &nvcmd;
  nvcmd_ext.context = wms_nv_convert_to_context(as_id);

  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( rex_self(), WMS_NV_SIG );
  nv_cmd_ext(&nvcmd_ext);
  wms_nv_wait();

  ret_status = nvcmd.status;

  /* Leaving WMS NV Critical Section */
  rex_leave_crit_sect(&wms_nv_crit_sect);

  if( ret_status != NV_DONE_S )
  {
    MSG_ERROR_2( "NV Write Failed Item %d Code %d",
                 item,
                 ret_status );

    /* Set the global NV cmd err appropriately */
    if(ret_status == NV_FULL_S || ret_status == NV_NOMEM_S)
    {
      cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
    }
    else
    {
      cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
    }
  }
  else
  {
    /* Set the global NV cmd err to OK */
    cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_NONE;
  }

  return( ret_status );

} /* end of wms_nv_write_wait_per_subs() */

/*===========================================================================
FUNCTION WMS_NV_WRITE_WAIT

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_write_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr    /* pointer to data for item */
)
{
  nv_stat_enum_type  ret_status;

  /* Entering WMS NV Critical Section */
  rex_enter_crit_sect(&wms_nv_crit_sect);

  MSG_HIGH_1( "wms_nv_write_wait, item=%d", item);

  /* set up NV item */
  nvcmd.tcb_ptr    = rex_self();        /* Synchronous NV Operation */
  nvcmd.sigs       = WMS_NV_SIG;
  nvcmd.done_q_ptr = NULL;

  nvcmd.cmd        = NV_WRITE_F;
  nvcmd.item       = item;             /* item to write */
  nvcmd.data_ptr   = data_ptr;        /* the data to write */


  /* Clear the signal, call NV, wait for it to finish */
  (void) rex_clr_sigs( rex_self(), WMS_NV_SIG );
  nv_cmd( &nvcmd );
  wms_nv_wait();

  ret_status = nvcmd.status;

  /* Leaving WMS NV Critical Section */
  rex_leave_crit_sect(&wms_nv_crit_sect);

  if( ret_status != NV_DONE_S )
  {
    MSG_ERROR_2( "NV Write Failed Item %d Code %d",
                 item,
                 ret_status );

    /* Set the global NV cmd err appropriately */
    if(ret_status == NV_FULL_S || ret_status == NV_NOMEM_S)
    {
      cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_MEMORY_FULL;
    }
    else
    {
      cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_MSG_NV_WRITE;
    }
  }
  else
  {
    /* Set the global NV cmd err to OK */
    cfg_s_ptr->nv_cmd_err = WMS_CMD_ERR_NONE;
  }

  return( ret_status );

} /* end of wms_nv_write_wait() */

/*===========================================================================
FUNCTION WMS_NV_READ_WAIT_PER_SUBS

DESCRIPTION
  Get an item from the nonvolatile memory per as id.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_read_wait_per_subs
(
  sys_modem_as_id_e_type  as_id,
  nv_items_enum_type      item,        /* which item */
  nv_item_type           *data_ptr     /* pointer to space for item */
)
{
  nv_stat_enum_type  ret_status;

  /* Entering WMS NV Critical Section */
  rex_enter_crit_sect(&wms_nv_crit_sect);

  MSG_HIGH_1( "wms_nv_read_wait_per_subs, item=%d", item);

  /* set up NV item */
  nvcmd.tcb_ptr    = rex_self();        /* Synchronous NV Operation */
  nvcmd.sigs       = WMS_NV_SIG;
  nvcmd.done_q_ptr = NULL;

  nvcmd.cmd        = NV_READ_F;        /* command */
  nvcmd.item       = item;             /* item to read */
  nvcmd.data_ptr   = data_ptr;         /* where to put result */

  nvcmd_ext.nvcmd   = &nvcmd;
  nvcmd_ext.context = wms_nv_convert_to_context(as_id);

  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( rex_self(), WMS_NV_SIG );
  nv_cmd_ext(&nvcmd_ext);
  wms_nv_wait();

  ret_status = nvcmd.status;

  /* Leaving WMS NV Critical Section */
  rex_leave_crit_sect(&wms_nv_crit_sect);

  if( ret_status != NV_DONE_S )
  {
    MSG_ERROR_2( "NV Read Failed Item %d Code %d",
                 item,
                 ret_status );
  }

  return( ret_status );

} /* end of wms_nv_read_wait_per_subs() */

/*===========================================================================
FUNCTION WMS_NV_READ_WAIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
nv_stat_enum_type wms_nv_read_wait
(
  nv_items_enum_type item,        /* which item */
  nv_item_type       *data_ptr          /* pointer to space for item */
)
{
  nv_stat_enum_type  ret_status;

  /* Entering WMS NV Critical Section */
  rex_enter_crit_sect(&wms_nv_crit_sect);

  MSG_HIGH_1( "wms_nv_read_wait, item=%d", item);

  /* set up NV item */
  nvcmd.tcb_ptr    = rex_self();        /* Synchronous NV Operation */
  nvcmd.sigs       = WMS_NV_SIG;
  nvcmd.done_q_ptr = NULL;

  nvcmd.cmd        = NV_READ_F;        /* command */
  nvcmd.item       = item;             /* item to read */
  nvcmd.data_ptr   = data_ptr;         /* where to put result */

  /* Clear the return signal, call NV, and wait for a response */
  (void) rex_clr_sigs( rex_self(), WMS_NV_SIG );
  nv_cmd( &nvcmd );
  wms_nv_wait();

  ret_status = nvcmd.status;

  /* Leaving WMS NV Critical Section */
  rex_leave_crit_sect(&wms_nv_crit_sect);

  /* Handle a NV_NOTACTIVE_S or NV_FAIL_S status internally by replacing
  ** the random data returned with a default value of our own.
  */
  if( ret_status == NV_NOTACTIVE_S || ret_status == NV_FAIL_S )
  {
    MSG_ERROR_2( "NV Read Failed Item %d Code %d",
                 item,
                 ret_status );
    ret_status = NV_DONE_S;

    switch( item )
    {
#ifdef FEATURE_CDSMS
      case NV_SMS_CD_PARMS_I:
        data_ptr->sms_cd_parms.address = 0;
        data_ptr->sms_cd_parms.len = 0;
        memset( (void*) data_ptr->sms_cd_parms.data, 0, NV_SMS_CD_PARMS_SIZE );
        ret_status = wms_nv_write_wait( NV_SMS_CD_PARMS_I, data_ptr );
        break;

#ifdef FEATURE_CDSMS_BROADCAST
      case NV_SMS_BC_CONFIG_I:
        data_ptr->sms_bc_config = (uint8) WMS_BC_CONFIG_DISALLOW;
        break;

      case NV_SMS_BC_USER_PREF_I:
        data_ptr->sms_bc_user_pref = (uint8)WMS_BC_PREF_DEACTIVATE;
        break;

      case NV_SMS_BC_SERVICE_TABLE_SIZE_I:
        data_ptr->sms_bc_service_table_size = 0 ;
        break;

      case NV_SMS_BC_SERVICE_TABLE_I:
        data_ptr->sms_bc_service_table.active_service = FALSE;
        break;
#endif /* FEATURE_CDSMS_BROADCAST */
#endif /* FEATURE_CDSMS */

#ifdef FEATURE_GWSMS_BROADCAST
      case NV_SMS_GW_CB_CONFIG_I:
        data_ptr->sms_gw_cb_config    = (uint8) WMS_BC_CONFIG_DISALLOW;
        (void) wms_nv_write_wait( item, data_ptr );
        break;

      case NV_SMS_GW_CB_USER_PREF_I:
        data_ptr->sms_gw_cb_user_pref    = (uint8) WMS_BC_PREF_DEACTIVATE;
        (void) wms_nv_write_wait( item, data_ptr );
        break;

      case NV_SMS_GW_CB_SERVICE_TABLE_SIZE_I:
        data_ptr->sms_gw_cb_service_table_size = 0;
        (void) wms_nv_write_wait( item, data_ptr );
        break;
#endif /* FEATURE_GWSMS_BROADCAST */

      case NV_SMS_MO_RETRY_INTERVAL_I:
        data_ptr->sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
        (void) wms_nv_write_wait( item, data_ptr );
        break;

      case NV_SMS_MO_RETRY_PERIOD_I:
        data_ptr->sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
        (void) wms_nv_write_wait( item, data_ptr );
        break;

      /* If it's an unknown item, print an info message and pass on the */
      /* NV_NOTACTIVE_S status. */
      default:
        ret_status = NV_NOTACTIVE_S;
        break;
    }

#ifdef FEATURE_CDSMS_BROADCAST
    /* write back the default values to NV */
    MSG_HIGH_1( "Write BC default value to NV, item=%d", item);
    if ( item == NV_SMS_BC_CONFIG_I ||
         item == NV_SMS_BC_USER_PREF_I ||
         item == NV_SMS_BC_SERVICE_TABLE_SIZE_I ||
         item == NV_SMS_BC_SERVICE_TABLE_I )
    {
      (void) wms_nv_write_wait( item, data_ptr );
    }
#endif /* FEATURE_CDSMS_BROADCAST */
  }
  else
  {
    if( ret_status != NV_DONE_S )
    {
      MSG_ERROR_2( "NV Read Failed Item %d Code %d",
                   item,
                   ret_status );
    }
  }

  return( ret_status );

} /* end of wms_nv_read_wait() */


#ifdef FEATURE_CDSMS
/*===========================================================================
FUNCTION wms_nv_read_cdma_sms

DESCRIPTION
  Read the CDMA SMS from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int            fd = -1;
  int            bytes_read = 0;
  int            offset = 0;
  uint8          *packed_data = NULL;

  if ( WMS_MAX_NV_CDMA_MESSAGES <= index )
  {
    MSG_ERROR_1("Message Index %d Out of Bounds", index);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  if ( !wms_nv_cd_msg_store_authenticate() )
  {
    return FALSE;
  }

  /* Read the contents from EFS */
  /* Open message storage */
  fd =  mcfg_fopen( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

  if ( fd < 0 )
  {
    MSG_ERROR_1( "SMS Read failed because sms msg store file could not be opened for item %d", index);
    return FALSE;
  }
  else /* File opened successfully for reading ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_CD_MSG_STORE_SLOT_SIZ */
    offset = (int) index * (SMS_NV_CD_MSG_STORE_SLOT_SIZ);

    packed_data = (uint8*) wms_mem_malloc(SMS_NV_CD_MSG_STORE_SLOT_SIZ);

    if ( NULL == packed_data )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Go to index position in message store before reading ... */
    offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );

    if ( offset == -1 )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Read data from its correct slot in file */
    bytes_read = mcfg_fread( fd, packed_data, (uint32)SMS_NV_CD_MSG_STORE_SLOT_SIZ, MCFG_FS_TYPE_EFS);

    if ( bytes_read != (int) SMS_NV_CD_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR_0("Unable to read msg correctly from file");

      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Check if data is valid */
    if ( ( SMS_NV_CD_MSG_STORE_START_TAG != packed_data[0] ) ||
         ( SMS_NV_CD_MSG_STORE_END_TAG   != packed_data[SMS_NV_CD_MSG_STORE_SLOT_SIZ-1] ) ||
         ( (uint8) WMS_TAG_NONE          == packed_data[2] ) )
    {
      MSG_ERROR_0("Invalid start/end tag or TAG_NONE found in data");

      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* If we got here, sms data stored in slot is good */
    /* "Unpack" meta data before returning sms message data */
    (void)memscpy( data,
                   (SMS_NV_CD_MSG_STORE_SLOT_SIZ - 2),
                   packed_data + 1,
                   (SMS_NV_CD_MSG_STORE_SLOT_SIZ - 2) );

    wms_mem_free( packed_data );
    packed_data = NULL;
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
  }

  return TRUE;

} /* wms_nv_read_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_write_cdma_sms

DESCRIPTION
  Writes the CDMA SMS to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA SMS
===========================================================================*/
boolean wms_nv_write_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int       fd = -1;
  int       bytes_written = 0;
  int       offset = 0;
  uint8     *packed_data = NULL;

  if ( WMS_MAX_NV_CDMA_MESSAGES <= index )
  {
    MSG_ERROR_1("Message Index %d Out of Bounds", index);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  /* Sanity Check */
  if ( !wms_nv_cd_msg_store_authenticate() )
  {
    return FALSE;
  }

  /* Open message storage */
  fd =  mcfg_fopen( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_O_WRONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

  if ( fd < 0 )
  {
    MSG_ERROR_1( "SMS Write failed because sms msg store file could not be opened for item %d", index);
    return FALSE;
  }
  else /* File opened successfully for writing ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_CD_MSG_STORE_SLOT_SIZ */
    offset =  (int) index * (SMS_NV_CD_MSG_STORE_SLOT_SIZ);

    packed_data = (uint8*) wms_mem_malloc(SMS_NV_CD_MSG_STORE_SLOT_SIZ);

    if ( packed_data == NULL )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* "Pack" meta data message before writing to slot */
    packed_data[0] = SMS_NV_CD_MSG_STORE_START_TAG;
    (void)memscpy( packed_data + 1,
                   SMS_NV_CD_MSG_STORE_SLOT_SIZ - 2,
                   data,
                   SMS_NV_CD_MSG_STORE_SLOT_SIZ - 2 );
    packed_data[SMS_NV_CD_MSG_STORE_SLOT_SIZ-1] = SMS_NV_CD_MSG_STORE_END_TAG;

    /* Go to index position in message store before writing ... */
    offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );

    if ( offset == -1 )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Insert data into its correct slot in file */
    bytes_written =  mcfg_fwrite( fd, packed_data, (uint32)SMS_NV_CD_MSG_STORE_SLOT_SIZ, MCFG_FS_TYPE_EFS );

    if ( bytes_written != (int) SMS_NV_CD_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR_0("Unable to write msg to file");
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Update EFS storage header also */
    if ( ! wms_nv_cd_msg_store_update_header_file ( index, data[1] ) )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    wms_mem_free( packed_data );
    packed_data = NULL;
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

  }

  return TRUE;

} /* wms_nv_write_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_replace_cdma_sms

DESCRIPTION
  Replace the CDMA SMS from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA SMS
===========================================================================*/
boolean wms_nv_replace_cdma_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{

  /* This call will overwrite previously written data */
  if ( ! wms_nv_write_cdma_sms( index, data ) )
  {
    return FALSE;
  }


  return TRUE;

} /* wms_nv_replace_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_delete_cdma_sms

DESCRIPTION
  Deletes the CDMA SMS from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA SMS
===========================================================================*/
boolean wms_nv_delete_cdma_sms
(
  wms_message_index_type   index,
  const uint8            * data
)
{
  int       fd = -1;
  int       bytes_written = 0;
  int       offset = 0;
  int8      *clear_data = NULL;

  (void) data;

  if ( WMS_MAX_NV_CDMA_MESSAGES <= index )
  {
    MSG_ERROR_1("Message Index %d Out of Bounds", index);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  /* Sanity Check */
  if ( !wms_nv_cd_msg_store_authenticate() )
  {
    return FALSE;
  }

  /* Open message storage */
  fd =  mcfg_fopen( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_O_WRONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

  if ( fd < 0 )
  {
    MSG_ERROR_1( "SMS Delete failed because sms msg store file could not be opened for item %d", index);
    return FALSE;
  }
  else /* File opened successfully for deleting msg ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_CD_MSG_STORE_SLOT_SIZ */
    offset = (int) index * (SMS_NV_CD_MSG_STORE_SLOT_SIZ);

    /* Go to index position in message store before writing ... */
    offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );
    if ( offset == -1 )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Allocate memory for slot we want to clear */
    clear_data = (int8*) wms_mem_malloc(SMS_NV_CD_MSG_STORE_SLOT_SIZ);
    if ( NULL == clear_data )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Clear the slot in file */
    bytes_written =  mcfg_fwrite( fd, clear_data, (uint32)SMS_NV_CD_MSG_STORE_SLOT_SIZ, MCFG_FS_TYPE_EFS );

    if ( bytes_written != (int) SMS_NV_CD_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR_0("Unable to clear msg in storage file");
      wms_mem_free( clear_data );
      clear_data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Update the EFS storage header also, pass in the tag value (data[1])
    it has to be modified to */
    if ( ! wms_nv_cd_msg_store_update_header_file( index, data[1] ) )
    {
      MSG_ERROR_0("Unable to update header file with updated info");
      wms_mem_free( clear_data );
      clear_data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    wms_mem_free( clear_data );
    clear_data = NULL;
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
  }

  return TRUE;

} /* wms_nv_delete_cdma_sms() */

/*===========================================================================
FUNCTION wms_nv_read_cdma_params

DESCRIPTION
  Read the CDMA Params from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_cdma_params
(
  wms_message_index_type    index,
  uint8                     * data
)
{
  nv_item_type nvi;
  boolean res = TRUE;
  nv_stat_enum_type   nv_st;

  nvi.sms_cd_parms.address = (uint8)index;

  if( (nv_st=wms_nv_read_wait( NV_SMS_CD_PARMS_I, & nvi )) == NV_DONE_S )
  {
    (void)memscpy( (uint8 *)data,
                   NV_SMS_CD_PARMS_SIZE,
                   (void *)nvi.sms_cd_parms.data,
                   NV_SMS_CD_PARMS_SIZE );
  }
  else
  {
    if( nv_st != NV_NOTACTIVE_S )
    {
      MSG_ERROR_0("Read NV_SMS_CD_PARMS_I failed");
    }
    res = FALSE;
  }

  return res;

} /* wms_nv_read_cdma_params() */

/*===========================================================================
FUNCTION wms_nv_write_cdma_params

DESCRIPTION
  Write the CDMA Params to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV CDMA params
===========================================================================*/
boolean wms_nv_write_cdma_params
(
  wms_message_index_type    index,
  const uint8                     * data
)
{
  nv_item_type nvi;
  boolean res = TRUE;

  nvi.sms_cd_parms.address = (uint8)index;

  (void)memscpy( (void *)nvi.sms_cd_parms.data,
                 NV_SMS_CD_PARMS_SIZE,
                 (uint8 *) data,
                 NV_SMS_CD_PARMS_SIZE );

  if( wms_nv_write_wait( NV_SMS_CD_PARMS_I, & nvi ) != NV_DONE_S )
  {
    res = FALSE;
  }

  return res;

} /* wms_nv_write_cdma_params() */

#endif /* FEATURE_CDSMS */



#ifdef FEATURE_GWSMS

/*===========================================================================
FUNCTION wms_nv_read_gw_sms

DESCRIPTION
  Read the GW SMS NV Item

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_gw_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int            fd = -1;
  int            bytes_read = 0;
  int            offset = 0;
  uint8          *packed_data = NULL;

  if ( WMS_MAX_NV_GW_MESSAGES <= index )
  {
    MSG_ERROR_1("Message Index %d Out of Bounds", index);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  if ( !wms_nv_gw_msg_store_authenticate() )
  {
    return FALSE;
  }

  /* Read the contents from EFS */
  /* Open message storage */
  fd =  mcfg_fopen( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

  if ( fd < 0 )
  {
    MSG_ERROR_1( "SMS Read failed because sms msg store file could not be opened for item %d", index);
    return FALSE;
  }
  else /* File opened successfully for reading ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_GW_MSG_STORE_SLOT_SIZ */
    offset =  (int) index * (SMS_NV_GW_MSG_STORE_SLOT_SIZ);
    packed_data = (uint8*) wms_mem_malloc (SMS_NV_GW_MSG_STORE_SLOT_SIZ);

    if ( NULL == packed_data )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Go to index position in message store before reading ... */
    offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );

    if ( offset == -1 )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Read data from its correct slot in file */
    bytes_read =  mcfg_fread( fd, packed_data, (uint32)SMS_NV_GW_MSG_STORE_SLOT_SIZ, MCFG_FS_TYPE_EFS );

    if ( bytes_read != (int) SMS_NV_GW_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR_0("Unable to read msg correctly from file");

      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Check if data is valid */
    if ( ( SMS_NV_GW_MSG_STORE_START_TAG != packed_data[0] )  ||
         ( SMS_NV_GW_MSG_STORE_END_TAG   != packed_data[SMS_NV_GW_MSG_STORE_SLOT_SIZ-1] ) ||
         ( (uint8) WMS_TAG_NONE          == packed_data[1] ) )
    {
      MSG_ERROR_0("Invalid start/end tag or TAG_NONE found in data");

      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* If we got here, sms data stored in slot is good */
    /* "Unpack" meta data before returning sms message data */
    (void)memscpy( data,
                   (SMS_NV_GW_MSG_STORE_SLOT_SIZ - 2),
                   packed_data + 1,
                   (SMS_NV_GW_MSG_STORE_SLOT_SIZ - 2) );

    wms_mem_free( packed_data );
    packed_data = NULL;
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
  }

  return TRUE;

} /* wms_nv_read_gw_sms() */

/*===========================================================================
FUNCTION wms_nv_write_gw_sms

DESCRIPTION
  Write the GW SMS NV Item

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW SMS
===========================================================================*/
boolean wms_nv_write_gw_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int       fd = -1;
  int       bytes_written = 0;
  int       offset = 0;
  uint8     *packed_data = NULL;

  if (WMS_MAX_NV_GW_MESSAGES <= index )
  {
    MSG_ERROR_1("Message Index %d Out of Bounds", index);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  /* Sanity Check */
  if ( !wms_nv_gw_msg_store_authenticate() )
  {
    return FALSE;
  }

  /* Open message storage */
  fd =  mcfg_fopen( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_O_WRONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

  if ( fd < 0 )
  {
    MSG_ERROR_1( "SMS Write failed because sms msg store file could not be opened for item %d", index);
    return FALSE;
  }
  else /* File opened successfully for writing ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_GW_MSG_STORE_SLOT_SIZ */
    offset =  (int) index * (SMS_NV_GW_MSG_STORE_SLOT_SIZ);

    packed_data = (uint8*) wms_mem_malloc ( SMS_NV_GW_MSG_STORE_SLOT_SIZ );

    if ( NULL == packed_data )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* "Pack" meta data message before writing to slot */
    packed_data[0] = SMS_NV_GW_MSG_STORE_START_TAG;
    (void)memscpy( packed_data + 1,
                   (SMS_NV_GW_MSG_STORE_SLOT_SIZ - 2),
                   data,
                   SMS_NV_GW_MSG_STORE_SLOT_SIZ - 2 );
    packed_data[SMS_NV_GW_MSG_STORE_SLOT_SIZ-1] = SMS_NV_GW_MSG_STORE_END_TAG;

    /* Go to index position in message store before writing ... */
    offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );

    if ( offset == -1 )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Insert data into its correct slot in file */
    bytes_written =  mcfg_fwrite( fd, packed_data, (uint32)SMS_NV_GW_MSG_STORE_SLOT_SIZ, MCFG_FS_TYPE_EFS );

    if ( bytes_written != (int) SMS_NV_GW_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR_0("Unable to write msg to file");
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    /* Update EFS storage header also */
    if ( ! wms_nv_gw_msg_store_update_header_file( index, data[0] ) )
    {
      MSG_ERROR_0("Unable to update header file with updated info");
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      wms_mem_free( packed_data );
      packed_data = NULL;
      return FALSE;
    }

    wms_mem_free( packed_data );
    packed_data = NULL;
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

  }

  return TRUE;

} /* wms_nv_write_sms() */

/*===========================================================================
FUNCTION wms_nv_replace_gw_sms

DESCRIPTION
  Replace the GW SMS NV Item

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW SMS
===========================================================================*/
boolean wms_nv_replace_gw_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{

  /* This call will overwrite previously written data */
  if ( ! wms_nv_write_gw_sms( index, data ) )
  {
    return FALSE;
  }


  return TRUE;

} /* wms_nv_replace_gw_sms() */

/*===========================================================================
FUNCTION wms_nv_delete_gw_sms

DESCRIPTION
  Delete the GW SMS NV Item

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW SMS
===========================================================================*/
boolean wms_nv_delete_gw_sms
(
  wms_message_index_type   index,
  uint8                    * data
)
{
  int       fd = -1;
  int       bytes_written = 0;
  int       offset = 0;
  int8      *clear_data = NULL;

  (void) data;
  if (WMS_MAX_NV_GW_MESSAGES <= index )
  {
    MSG_ERROR_1("Message Index %d Out of Bounds", index);
    return FALSE;
  }

  /* Kick the Watchdog timer */
  wms_kick_dog();

  /* Sanity Check */
  if ( !wms_nv_gw_msg_store_authenticate() )
  {
    return FALSE;
  }

  /* Open message storage */
  fd =  mcfg_fopen( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_O_WRONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

  if ( fd < 0 )
  {
    MSG_ERROR_1( "SMS Delete failed because sms msg store file could not be opened for item %d", index);
    return FALSE;
  }
  else /* File opened successfully for deleting msg ... */
  {
    /* Data is stored in fixed positions of size SMS_NV_GW_MSG_STORE_SLOT_SIZ */
    offset = (int)(index * (SMS_NV_GW_MSG_STORE_SLOT_SIZ));

    /* Go to index position in message store before writing ... */
    offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );
    if ( offset == -1 )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Allocate memory for slot we want to clear */
    clear_data = (int8*) wms_mem_malloc( SMS_NV_GW_MSG_STORE_SLOT_SIZ );
    if ( NULL == clear_data )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Clear the slot in file */
    bytes_written =  mcfg_fwrite( fd, clear_data, (uint32)SMS_NV_GW_MSG_STORE_SLOT_SIZ, MCFG_FS_TYPE_EFS );

    if ( bytes_written != (int) SMS_NV_GW_MSG_STORE_SLOT_SIZ )
    {
      MSG_ERROR_0("Unable to clear msg in storage file");
      wms_mem_free( clear_data );
      clear_data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    /* Update the EFS storage header also */
    if ( ! wms_nv_gw_msg_store_update_header_file( index, data[0] ) )
    {
      MSG_ERROR_0("Unable to update header file with updated info");
      wms_mem_free( clear_data );
      clear_data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    wms_mem_free( clear_data );
    clear_data = NULL;
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
  }


  return TRUE;

} /* wms_nv_delete_sms() */

/*===========================================================================
FUNCTION wms_nv_read_gw_domain_pref

DESCRIPTION
  Read the GW Domain Preference from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_gw_domain_pref
(
  sys_modem_as_id_e_type      as_id,
  wms_gw_domain_pref_e_type * gw_domain_pref
)
{
  nv_item_type nvi;
  boolean res = TRUE;

  if( wms_nv_read_wait_per_subs(as_id, NV_SMS_GW_BEARER_PREF_I, & nvi ) == NV_DONE_S )
  {
    *gw_domain_pref = (wms_gw_domain_pref_e_type) nvi.sms_gw_bearer_pref;
  }
  else
  {
    MSG_ERROR_0("Read NV_SMS_GW_BEARER_PREF_I failed");
    res = FALSE;
  }

  return res;

} /* wms_nv_read_gw_domain_pref() */

/*===========================================================================
FUNCTION wms_nv_write_gw_domain_pref

DESCRIPTION
  Write the GW Domain Preference to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Domain Preference
===========================================================================*/
boolean wms_nv_write_gw_domain_pref
(
  sys_modem_as_id_e_type      as_id,
  wms_gw_domain_pref_e_type   gw_domain_pref
)
{
  nv_item_type nvi;
  boolean res = TRUE;

  nvi.sms_gw_bearer_pref = (uint8)gw_domain_pref;

  if(wms_nv_write_wait_per_subs(as_id, NV_SMS_GW_BEARER_PREF_I, & nvi) != NV_DONE_S)
  {
    MSG_ERROR_0("Write NV_SMS_GW_BEARER_PREF_I failed");
    res = FALSE;
  }

  return res;

} /* wms_nv_write_gw_domain_pref() */

/*===========================================================================
FUNCTION wms_nv_read_gcf_flag

DESCRIPTION
  Read the GCF Flag from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_gcf_flag
(
  boolean   * gcf_flag
)
{
  nv_item_type nvi;
  boolean res = TRUE;

  if( wms_nv_read_wait( NV_GPRS_ANITE_GCF_I, & nvi ) == NV_DONE_S )
  {
    * gcf_flag = nvi.gprs_anite_gcf;
  }
  else
  {
    MSG_ERROR_0("Read NV_GPRS_ANITE_GCF_I failed");
    res = FALSE;
  }

  return res;

} /* wms_nv_read_gcf_flag() */

/*===========================================================================
FUNCTION wms_nv_read_ens_flag

DESCRIPTION
  Read the ENS Flag from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_ens_flag
(
  boolean   * ens_flag
)
{
  nv_item_type nvi;
  boolean res = TRUE;

  if( wms_nv_read_wait( NV_ENS_ENABLED_I, & nvi ) == NV_DONE_S )
  {
    * ens_flag = nvi.ens_enabled;
  }
  else
  {
    MSG_ERROR_0("Read NV_ENS_ENABLED_I failed");
    res = FALSE;
  }

  return res;

} /* wms_nv_read_ens_flag() */


#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION wms_nv_read_routing

DESCRIPTION
  Read the Message Routing from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_routing
(
  wms_routes_s_type  *ptr
)
{
  int i;
  nv_item_type nvi;
  boolean res = TRUE;

  if( wms_nv_read_wait_per_subs(ptr->as_id, NV_SMS_CFG_ROUTING_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR_0("Read NV_SMS_CFG_ROUTING_I failed");
    res = FALSE;
  }
  else
  {
    for( i = 0; i<NV_SMS_MAX_ROUTES; i ++ )
    {
      ptr->pp_routes[i].route     = (wms_route_e_type) nvi.sms_cfg_routing.pp_routes[i];
      ptr->pp_routes[i].mem_store = (wms_memory_store_e_type) nvi.sms_cfg_routing.pp_mem_stores[i];

      //TBD bc_routes
    }

#ifdef FEATURE_GWSMS
    ptr->transfer_status_report = nvi.sms_cfg_routing.transfer_status_report;
#endif /* FEATURE_GWSMS */
  }

  return res;

} /* wms_nv_read_routing() */


/*===========================================================================
FUNCTION wms_nv_write_routing

DESCRIPTION
  Write the Message Routing to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV Message Routing
===========================================================================*/
boolean wms_nv_write_routing
(
  const wms_routes_s_type  *ptr
)
{
  int i;
  boolean res = TRUE;
  nv_item_type nvi;

  memset(&nvi, 0, sizeof(nv_item_type));
  for( i = 0; i<NV_SMS_MAX_ROUTES; i ++ )
  {
    nvi.sms_cfg_routing.pp_routes[i]     = (uint8) ptr->pp_routes[i].route;
    nvi.sms_cfg_routing.pp_mem_stores[i] = (uint8) ptr->pp_routes[i].mem_store;

    //TBD bc_routes
  }

#ifdef FEATURE_GWSMS
  nvi.sms_cfg_routing.transfer_status_report =   ptr->transfer_status_report;
#endif /* FEATURE_GWSMS */

  if( wms_nv_write_wait_per_subs(ptr->as_id, NV_SMS_CFG_ROUTING_I, & nvi) != NV_DONE_S )
  {
    MSG_ERROR_0("write NV_SMS_CFG_ROUTING_I failed");
    res = FALSE;
  }

  return res;

} /* wms_nv_write_routing() */



#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_nv_read_gw_params

DESCRIPTION
  Read the GW Params from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_gw_params
(
  wms_gw_template_s_type  * template_ptr,
  sys_modem_as_id_e_type  as_id
)
{
  nv_item_type nvi;
  boolean res = TRUE;
  nv_stat_enum_type   nv_st;

  if( (nv_st = wms_nv_read_wait_per_subs( as_id, NV_SMS_GW_PARMS_I, & nvi )) == NV_DONE_S )
  {
    if (wms_ts_decode_smsp((uint8 *)nvi.sms_gw_parms.data,
                           NV_SMS_GW_PARAMS_SIZE,
                           template_ptr) == FALSE)
    {
      res = FALSE;
    }
    /*
    memcpy( (uint8*) template_ptr,
            (void*) nvi.sms_gw_parms.data,
            NV_SMS_GW_PARAMS_SIZE );
    */
  }
  else
  {
    if( nv_st != NV_NOTACTIVE_S )
    {
      MSG_ERROR_1("Read NV_SMS_GW_PARMS_I failed as id = %d", as_id);
    }
    res = FALSE;
  }

  return res;

} /* wms_nv_read_gw_params() */

/*===========================================================================
FUNCTION wms_nv_write_gw_params

DESCRIPTION
  Write the GW Params to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Params
===========================================================================*/
boolean wms_nv_write_gw_params
(
  wms_gw_template_s_type  * template_ptr, 
  sys_modem_as_id_e_type  as_id
)
{
  boolean res = TRUE;
  nv_item_type nvi;

  memset(&nvi, 0, sizeof(nv_item_type));
  if (wms_ts_encode_smsp(template_ptr,
                         NV_SMS_GW_PARAMS_SIZE,
                         (uint8 *)nvi.sms_gw_parms.data) == FALSE)
  {
    res = FALSE;
  }

  /*
  memcpy((void*) nvi.sms_gw_parms.data,
          (uint8*) template_ptr,
          NV_SMS_GW_PARAMS_SIZE );
  */
  if( wms_nv_write_wait_per_subs(as_id, NV_SMS_GW_PARMS_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR_1("Write NV_SMS_GW_PARMS_I failed as id = %d", as_id);
    res = FALSE;
  }

  return res;

} /* wms_nv_write_gw_params() */

#endif /* FEATURE_GWSMS */


/*===========================================================================
FUNCTION WMS_NV_INIT_NVRUIM

DESCRIPTION
  Get certain NV items from the EFS or RUIM depending on NVRUIM's settings.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_init_nvruim
(
  void
)
{
  nv_item_type nvi;

  /* Read Retry Interval from NV */
  if (wms_nv_read_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi ) == NV_DONE_S)
  {
    msg_s_ptr->mo_retry_interval = nvi.sms_mo_retry_interval;
  }
  else
  {
    msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    nvi.sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    (void) wms_nv_write_wait( NV_SMS_MO_RETRY_INTERVAL_I, &nvi );
  }

  /* Read Retry Period from NV */
  {
    if (wms_nv_read_wait( NV_SMS_MO_RETRY_PERIOD_I, &nvi ) == NV_DONE_S)
    {
      msg_s_ptr->mo_retry_period = nvi.sms_mo_retry_period; /* seconds */
    }
    else
    {
      msg_s_ptr->mo_retry_period = WMS_MAX_RETRY_PERIOD; /* seconds */
    }
  }

  MSG_HIGH_2( "Retry Period, Interval = %d s, %d s,",
              msg_s_ptr->mo_retry_period,
              msg_s_ptr->mo_retry_interval );

#ifdef FEATURE_CDSMS

  /* Read Access or Traffic Channel from RUIM card */
  wms_nv_read_cdma_channel_setting();

  /* Read Service Option Setting from card */
  wms_nv_read_cdma_service_option();

#endif /* FEATURE_CDSMS */

} /* wms_nv_init_nvruim() */

/*===========================================================================
FUNCTION wms_nv_read_bearer_prefs

DESCRIPTION
  Read the Bearer Preferences

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
)
{
  boolean             res = TRUE;
  nv_stat_enum_type   nv_st;
  int                 i;
  nv_item_type        nvi;

  if( (nv_st=wms_nv_read_wait( NV_SMS_BEARER_PREFS_I, & nvi )) == NV_DONE_S )
  {
    for( i=0; i < (int) WMS_BEARER_MAX; i ++ )
    {
      bearer_prefs[i] = (wms_bearer_usage_e_type) nvi.sms_bearer_prefs[i].nv_sms_bearer_usage;
    }
  }
  else
  {
    MSG_ERROR_1("Read NV_SMS_BEARER_PREFS_I failed: %d", nv_st);
    res = FALSE;
  }

  return res;

} /* wms_nv_read_bearer_prefs() */

/*===========================================================================
FUNCTION wms_nv_write_bearer_prefs

DESCRIPTION
  Write the Bearer Preferences to NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  Modified NV GW Params
===========================================================================*/
boolean wms_nv_write_bearer_prefs
(
  wms_bearer_prefs_arr_type  bearer_prefs
)
{
  boolean      res = TRUE;
  int          i;
  nv_item_type nvi;

  memset(&nvi, 0, sizeof(nv_item_type));
  for( i=0; i < (int) WMS_BEARER_MAX; i++ )
  {
    nvi.sms_bearer_prefs[i].nv_sms_bearer_usage = (uint8) bearer_prefs[i];
  }

  if( wms_nv_write_wait( NV_SMS_BEARER_PREFS_I, & nvi ) != NV_DONE_S )
  {
    MSG_ERROR_0("Write NV_SMS_BEARER_PREFS_I failed");
    res = FALSE;
  }

  return res;

} /* wms_nv_write_bearer_prefs() */

/*===========================================================================
FUNCTION wms_nv_read_max_payload_len

DESCRIPTION
  Read the max payload length from NV

RETURN VALUE
  TRUE / FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_read_max_payload_len
(
  uint32   *max_payload_len
)
{
  nv_item_type nvi;
  boolean res = FALSE;

  if( wms_nv_read_wait( NV_SMS_MAX_PAYLOAD_LENGTH_I, & nvi ) == NV_DONE_S )
  {
    *max_payload_len = nvi.sms_max_payload_length;
    res = TRUE;
  }

  MSG_HIGH_2( "Read MAX_PAYLOAD_LEN=%d, Result=%d",
              *max_payload_len,
              res );
  return res;
} /* wms_nv_read_max_payload_len() */

/*===========================================================================
FUNCTION WMS_NV_INIT

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from one task.
===========================================================================*/
void wms_nv_init
(
  void
)
{
#ifdef FEATURE_CDSMS
  uint32       i;
  uint8       *nv_cdma_data = NULL;

#endif
  nv_item_type *nvi_ptr = NULL;
  struct       fs_stat temp_fs_buf;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  MSG_HIGH_0("wms_nv_init()");

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  {
    mcfg_refresh_registration_s_type mcfg_refresh_reg_data;

    mcfg_refresh_reg_data.order     = MCFG_REFRESH_ORDER_50;       
    mcfg_refresh_reg_data.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
    mcfg_refresh_reg_data.sub_mask  = MCFG_REFRESH_INDEX_MASK_ALL;
    mcfg_refresh_reg_data.group     = 0;                           
    mcfg_refresh_reg_data.cb        = wms_nv_refresh_cb;

    // Register for NV refresh event from MCFG module.
    if (FALSE == mcfg_refresh_register(&mcfg_refresh_reg_data))
    {
      MSG_ERROR_0("NV Refresh registration failed");
    }
  }
#endif  /* FEATURE_MODEM_CONFIG_REFRESH */

  cfg_s_ptr = wms_cfg_s_ptr();

  msg_s_ptr = wms_msg_s_ptr();

#ifdef FEATURE_CDSMS
  dc_s_ptr  = wms_dc_s_ptr();
#endif

  /* Initialize WMS NV Critical Section */
  rex_init_crit_sect(&wms_nv_crit_sect);

  /* Check and Create New Directory for SMS if not present */
  if ( MCFG_FS_STATUS_OK != mcfg_fs_stat( "/sms", &temp_fs_buf, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE ) )
  {
    if ( MCFG_FS_ERROR == mcfg_mkdir( "/sms", MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS ) )
    {
      MSG_ERROR_0("Cannot make SMS Folder for SMS Messages");
    }
  }


#ifdef FEATURE_CDSMS
  cfg_s_ptr->cdma_voice_mail_index  = (wms_message_index_type)WMS_DUMMY_MESSAGE_INDEX;
  cfg_s_ptr->cdma_voice_mail_active = FALSE;

  cfg_s_ptr->nv_cdma_max_slots = WMS_MAX_NV_CDMA_MESSAGES;

  /* Clear the duplicate detection list */
  for ( i = 0; i < WMS_MESSAGE_LIST_MAX; i++)
  {
#ifdef FEATURE_CDSMS_DUP_DETECTION
    /* Initialize the mask to 0 signifying this slot is empty. */
    cfg_s_ptr->dup_detect[i] = NULL;
#endif /* FEATURE_CDSMS_DUP_DETECTION */
  }

  if ( wms_nv_cd_msg_store_authenticate() )
  {
      /* Read meta information about slots */
      if ( wms_nv_cd_msg_store_read_tags( ) )
      {
          wms_client_message_s_type     *client_message_ptr = 
            (wms_client_message_s_type *) wms_mem_malloc( sizeof(wms_client_message_s_type) );
          wms_tl_message_type           *cdma_tl_ptr        =
            (wms_tl_message_type *) wms_mem_malloc( sizeof(wms_tl_message_type) );
          wms_OTA_message_type          *cdma_ota_ptr       = 
            (wms_OTA_message_type *) wms_mem_malloc( sizeof(wms_OTA_message_type) );
          nv_cdma_data = (uint8*) wms_mem_malloc ( sizeof(uint8) * NV_SMS_DATA_SIZ );

          if (NULL != nv_cdma_data && NULL != client_message_ptr && NULL != cdma_tl_ptr && NULL != cdma_ota_ptr)
          {
            for( i = 0; i < cfg_s_ptr->nv_cdma_max_slots; i ++ )
            {
              if ( ( WMS_TAG_NONE != cfg_s_ptr->nv_cdma_tags[i] ) &&
                   wms_nv_read_cdma_sms( i, nv_cdma_data ) )
              {
                /* ======= Structure =========
                              ** byte 0:    format
                              ** byte 1:    tag/status
                              ** byte 2:    len
                              ** remainder: data
                              ** =========================== */

                wms_cfg_cdma_update_voice_mail_index( (wms_format_e_type) nv_cdma_data[0],  /* format */
                                                      (wms_message_tag_e_type) nv_cdma_data[1],  /* tag */
                                                      nv_cdma_data[2],  /* len */
                                                      nv_cdma_data+3,
                                                      i );

                if ( WMS_IS_MT( cfg_s_ptr->nv_cdma_tags[i] ) )
                {
                  /* Do the proper decoding of the nv data to update the
                                 ** duplicate detection cache
                                 */

                  client_message_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
                  client_message_ptr->msg_hdr.tag          = (wms_message_tag_e_type) nv_cdma_data[1];
                  cdma_ota_ptr->format   = (wms_format_e_type) nv_cdma_data[0];
                  cdma_ota_ptr->data_len = nv_cdma_data[2];
                  (void)memscpy( (uint8 *)cdma_ota_ptr->data,
                                 cdma_ota_ptr->data_len,
                                 nv_cdma_data + 3,
                                 cdma_ota_ptr->data_len);
                  if(wms_ts_decode_CDMA_tl(cdma_ota_ptr, cdma_tl_ptr,
                                           &(client_message_ptr->u.cdma_message.raw_ts)) != WMS_OK_S)
                  {
                    MSG_ERROR_0("Decode tl FAILED, Dup list not updated");
                  }
                  else if(wms_ts_decode_CDMA_bd(&(client_message_ptr->u.cdma_message.raw_ts), FALSE, FALSE,
                                                &(cdma_tl_ptr->cl_bd)) != WMS_OK_S)
                  {
                    MSG_ERROR_0("Decode bd FAILED, Dup list not updated");
                  }
                  else if( (client_message_ptr->u.cdma_message.raw_ts.format != WMS_FORMAT_MWI) &&
                           (wms_ts_validate_bd_subparms( &(cdma_tl_ptr->cl_bd), cdma_tl_ptr->teleservice ) != WMS_OK_S ) )
                  {
                    MSG_ERROR_0("Decode bd Validation FAILED, Dup list not updated");
                  }
                  else
                  {
                    /* Update the duplicate detection list */
                    wms_cfg_update_dup_info_cache(WMS_MEMORY_STORE_NV_CDMA,
                                                  i,
                                                  cdma_tl_ptr);
                  }
                }
              }
            }
          }
          WMS_MEM_FREEIF(nv_cdma_data);
          WMS_MEM_FREEIF(client_message_ptr);
          WMS_MEM_FREEIF(cdma_tl_ptr);
          WMS_MEM_FREEIF(cdma_ota_ptr);
      }
  }
  /* Init for NV params */
  if (NULL == cdma_template_ptr)
  {
     cdma_template_ptr = wms_mem_malloc( sizeof(wms_cdma_template_s_type) );
  }
  if (NULL != cdma_template_ptr)
  {
    nv_cdma_data = (uint8*) wms_mem_malloc ( sizeof(uint8) * NV_SMS_DATA_SIZ );
    if (NULL != nv_cdma_data)
    {
      if( wms_nv_read_cdma_params( 0, nv_cdma_data ) == FALSE ||
          wms_ts_cdma_decode_smsp( nv_cdma_data, NV_SMS_CD_PARMS_SIZE, cdma_template_ptr ) == FALSE )
      {
        /* Write default values to NV params */
        cdma_template_ptr->teleservice  = WMS_TELESERVICE_CMT_95;
        cdma_template_ptr->mask = 0;
        (void)wms_ts_cdma_encode_smsp( cdma_template_ptr, NV_SMS_CD_PARMS_SIZE, nv_cdma_data );
        if( wms_nv_write_cdma_params( 0, nv_cdma_data ) == FALSE )
        {
          MSG_ERROR_0("wms_nv_write_cdma_params failed");
        }
      }

      wms_mem_free ( nv_cdma_data );
    }
  }

#if !defined ( FEATURE_CDSMS_RUIM ) && !defined ( FEATURE_CSIM )
  /* To read the message_id and wap_message_id from NV */
  wms_nv_read_sms_status();
#endif
#endif /* FEATURE_CDSMS */


#ifdef FEATURE_GWSMS

  wms_nv_determine_max_gw_slots();

  if ( wms_nv_gw_msg_store_authenticate() )
  {
      /* Read meta information about slots */
      (void)wms_nv_gw_msg_store_read_tags();
  }

  /* Init for NV params */
  {
     int j = 0;

     MSG_HIGH_1("Device mode = %d", cfg_s_ptr->device_mode);
     if ( WMS_DEVICE_MODE_SS == cfg_s_ptr->device_mode )
     {
        j = 1;
     }
     else 
     {
       // Init for DS (2 SIM slots)
       if ( ( WMS_DEVICE_MODE_DSDS == cfg_s_ptr->device_mode ) || ( WMS_DEVICE_MODE_DSDA == cfg_s_ptr->device_mode ))
       {
          j = 2;
       }
       // Init for TSTS (3 SIM slots)
       else
       {
          j = 3;
       }
     }
    

     for (i = 0; i < j; i++) 
     {
        if (NULL == gw_template_ptr)
        {
           gw_template_ptr = wms_mem_malloc( sizeof(wms_gw_template_s_type) );
        }

        if (NULL != gw_template_ptr)
        {
          /* Init for NV params */
          gw_template_ptr->alpha_id.data = (uint8 *)wms_mem_malloc( (size_t) WMS_ALPHA_ID_MAX);
          if( NULL == gw_template_ptr->alpha_id.data )
          {
            MSG_ERROR_0("Memory Allocation failed");
          }
          else
          {
            if ( FALSE == wms_nv_read_gw_params( gw_template_ptr, i ) )  //ASID can be 0,1,2
            {
              MSG_HIGH_1("wms_nv_read_gw_params() failed for as_id=%d, writing default value to NV", i);
              /* Invalid NV template present */
              /* Write default values to NV params */
              gw_template_ptr->alpha_id.len = 0;

              // Enabling all fields for the Template
              // as per GCF certification
              gw_template_ptr->mask         = WMS_GW_TEMPLATE_MASK_SC_ADDR
                                            | WMS_GW_TEMPLATE_MASK_DEST_ADDR
                                            | WMS_GW_TEMPLATE_MASK_PID
                                            | WMS_GW_TEMPLATE_MASK_DCS
                                            | WMS_GW_TEMPLATE_MASK_VALIDITY;

              // Destination Address
              gw_template_ptr->dest_addr.number_of_digits   = 1;
              gw_template_ptr->dest_addr.number_type        = WMS_NUMBER_INTERNATIONAL;
              gw_template_ptr->dest_addr.number_plan        = WMS_NUMBER_PLAN_TELEPHONY;
              gw_template_ptr->dest_addr.digits[0]          = 7;

              // SC Address
              gw_template_ptr->sc_addr.number_of_digits     = 1;
              gw_template_ptr->sc_addr.number_type          = WMS_NUMBER_INTERNATIONAL;
              gw_template_ptr->sc_addr.number_plan          = WMS_NUMBER_PLAN_TELEPHONY;
              gw_template_ptr->sc_addr.digits[0]            = 7;

              // PID
              gw_template_ptr->pid                          = WMS_PID_DEFAULT;

              // DCS
              gw_template_ptr->dcs.is_compressed            = FALSE;
              gw_template_ptr->dcs.alphabet                 = WMS_GW_ALPHABET_7_BIT_DEFAULT;
              gw_template_ptr->dcs.msg_waiting              = WMS_GW_MSG_WAITING_NONE;
              gw_template_ptr->dcs.msg_class                = WMS_MESSAGE_CLASS_NONE;

              // Relative Validity
              memset( &(gw_template_ptr->relative_validity), 0, sizeof(wms_timestamp_s_type));
              gw_template_ptr->relative_validity.day = 1;


              if ( FALSE == wms_nv_write_gw_params( gw_template_ptr, i ) ) //ASID can be 0,1,2
              {
               MSG_ERROR_1("wms_nv_write_gw_params failed for as id = %d", i);
              }
            }

            wms_mem_free( gw_template_ptr->alpha_id.data );
          }
        }
     }
  }
#endif /* FEATURE_GWSMS */

  nvi_ptr = (nv_item_type *) wms_mem_malloc ( sizeof(nv_item_type) );

  if (NULL != nvi_ptr)
  {
    /* Read Retry Interval from NV */
    if (wms_nv_read_wait( NV_SMS_MO_RETRY_INTERVAL_I, nvi_ptr ) == NV_DONE_S)
    {
      msg_s_ptr->mo_retry_interval = nvi_ptr->sms_mo_retry_interval;
    }
    else
    {
      msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
      nvi_ptr->sms_mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
      (void) wms_nv_write_wait( NV_SMS_MO_RETRY_INTERVAL_I, nvi_ptr );
    }

    /* Read NV retry period for both CDMA and GW modes.
        * Note: Only GWSMS is still using the CGF flag to determine whether
        * retry should be attempted or not.
        */
    if (wms_nv_read_wait( NV_SMS_MO_RETRY_PERIOD_I, nvi_ptr ) == NV_DONE_S)
    {
      msg_s_ptr->mo_retry_period = nvi_ptr->sms_mo_retry_period;
    }
    else
    {
      msg_s_ptr->mo_retry_period = WMS_MAX_RETRY_PERIOD;
      nvi_ptr->sms_mo_retry_period = WMS_MAX_RETRY_PERIOD;
      (void) wms_nv_write_wait( NV_SMS_MO_RETRY_PERIOD_I, nvi_ptr );
    }
    wms_mem_free( nvi_ptr );
  }
  else
  {
    msg_s_ptr->mo_retry_interval = WMS_TIMER_RETRY_INTERVAL;
    msg_s_ptr->mo_retry_period   = WMS_MAX_RETRY_PERIOD;
  }

  MSG_HIGH_2("Retry Interval: %d, Retry Period = %d s", msg_s_ptr->mo_retry_interval, msg_s_ptr->mo_retry_period);
  /* done with NV init */
  return;

} /* end of wms_nv_init() */


#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_nv_determine_max_gw_slots

DESCRIPTION
  Fills in the number of slots available for storing messages
  accounting for GCF flag.

RETURN VALUE
  None.

===========================================================================*/

static void wms_nv_determine_max_gw_slots( void )
{
  nv_item_type nvi;

  /* Read Max Number of Messages from NV */
  if ( ( wms_nv_read_wait( NV_MS_SMS_MAX_NUMBER_OF_SMS_I, & nvi ) == NV_DONE_S )
       && (nvi.ms_sms_max_number_of_sms <= WMS_MAX_NV_GW_MESSAGES) )
  {
    cfg_s_ptr->nv_gw_max_slots = nvi.ms_sms_max_number_of_sms;
  }
  else
  {
    MSG_HIGH_0("Reading Max SMS from NV Failed or Bad Value");
    /* Setting default value to GCF NV Message Max */
    cfg_s_ptr->nv_gw_max_slots = WMS_MAX_GCF_NV_GW_MESSAGES;
    nvi.ms_sms_max_number_of_sms = (uint16) cfg_s_ptr->nv_gw_max_slots;
    (void)wms_nv_write_wait( NV_MS_SMS_MAX_NUMBER_OF_SMS_I, & nvi );
  }

  MSG_HIGH_1("Max SMS NV Slots = %d", cfg_s_ptr->nv_gw_max_slots);
}

/*===========================================================================
FUNCTION wms_nv_gw_msg_store_authenticate

DESCRIPTION
  Checks if the SMS storage for GW messages is present in EFS.  If not, it
  creates it.  If presents, it validates the version of the file.

RETURN VALUE
  TRUE successful case
  FAIL failure case

===========================================================================*/
static uint8 wms_nv_gw_msg_store_authenticate( void )
{
  struct    fs_stat temp_fs_buf;
  uint8     result = TRUE;

  /* Create SMS Message Store if necessary */
  if ( ( MCFG_FS_STATUS_OK != mcfg_fs_stat( SMS_NV_GW_MSG_STORE_HEADER, &temp_fs_buf, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE ) )  ||
       ( MCFG_FS_STATUS_OK != mcfg_fs_stat( SMS_NV_GW_MSG_STORE_DATA, &temp_fs_buf, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE ) ) )
  {
    MSG_HIGH_0("SMS GW storage file(s) not found.  Creating them...");

    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );
    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    result = wms_nv_gw_msg_store_init ();

  }
  /* msg store exists, just validate version information */
  else if ( !wms_nv_gw_msg_store_is_version_valid() )
  {
    MSG_HIGH_0("Version mismatch.  Delete and re-create current version");
    /* TODO: "Convert" versions in the future */
    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );
    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    result = wms_nv_gw_msg_store_init ();
  }
  else
  {
    MSG_HIGH_0("SMS_NV_GW_MSG_STORE_DATA authenticated successfully");
  }

  return result;
}

/*===========================================================================
FUNCTION wms_nv_gw_msg_store_is_version_valid

DESCRIPTION
  Reads the information from the storage file and validates it.

RETURN VALUE
  TRUE/FALSE

===========================================================================*/
static uint8 wms_nv_gw_msg_store_is_version_valid( void )
{
  int    fd = -1;
  uint8  version_info[8];
  int    bytes_read = 0;
  uint32 major_ver = 0;
  uint32 minor_ver = 0;

  /* Open header */
  fd =  mcfg_fopen( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

  if ( fd < 0 )
  {
    MSG_ERROR_0( "Could not open header file");
    return FALSE;
  }
  else /* File opened successfully for reading ... */
  {
    bytes_read =  mcfg_fread( fd, version_info, (uint32) sizeof ( version_info ), MCFG_FS_TYPE_EFS );

    if ( bytes_read != (int) sizeof ( version_info ) )
    {
      MSG_ERROR_0("Unable to read version correctly");

      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    major_ver = ((uint32)version_info[0] << 24) |
                ((uint32)version_info[1] << 16) |
                ((uint32)version_info[2] << 8)  |
                 version_info[3];

    minor_ver = ((uint32)version_info[4] << 24) |
                ((uint32)version_info[5] << 16) |
                ((uint32)version_info[6] << 8)  |
                 version_info[7];

    /* In this implementation, if either version is wrong, return error */
    if ( ( SMS_NV_GW_MSG_STORE_MAJOR_VER != major_ver )  ||
         ( minor_ver != cfg_s_ptr->nv_gw_max_slots ) )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;

    }
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
  }

  return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_gw_msg_store_init

DESCRIPTION
  Creates the storage file and initializes all the slots.
  Also creats the header file

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_gw_msg_store_init ( void )
{
  if ( !wms_nv_gw_msg_store_create_header_file() )
  {
    MSG_ERROR_0("Cannot make SMS Message Storage Header");
    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );
    return FALSE;
  }

  if ( !wms_nv_gw_msg_store_create_data_file() )
  {
    MSG_ERROR_0("Cannot make SMS Message Storage Header");

    /* Ensure the corrupt files are deleted before returning */
    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );
    (void) mcfg_fs_delete( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    return FALSE;
  }

  return TRUE;
}
/*===========================================================================
FUNCTION wms_nv_gw_msg_store_create_header_file

DESCRIPTION
  Creates the storage header file.

RETURN VALUE
  1 successful case
  0 failure case

===========================================================================*/
static uint8 wms_nv_gw_msg_store_create_header_file ( void )
{
    int       fd = -1;
    uint8     *data = NULL;
    int       data_size = 0;
    int       bytes_written = 0;

    fd = mcfg_fopen( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    if ( fd < 0 )
    {
      /*
       * Unfortunately cant do much at this point...
       * We will try creating the header before every write operation
       * so hopefully it will eventually succeed..
       */
      MSG_ERROR_0("Cannot make SMS Message Storage Header");
      return FALSE;
    }
    else
    {
      /* Calculate file size */
      data_size += 4; /* Major Version */
      data_size += 4; /* Minor Version (number of slots) */
      data_size += 4; /* Individual Slot length */
      data_size += (cfg_s_ptr->nv_gw_max_slots); /* Meta info. about each slot */

      data = (uint8*) wms_mem_malloc( data_size );

      if ( NULL == data )
      {
        return FALSE;
      }

      /* Set major version */
      /*lint -save -e572 can be disabled since this is a const macro so we cant cast */
      data [0]  = (uint8) (((SMS_NV_GW_MSG_STORE_MAJOR_VER) & 0xff000000) >> 24);
      data [1]  = (uint8) (((SMS_NV_GW_MSG_STORE_MAJOR_VER) & 0x00ff0000) >> 16);
      data [2]  = (uint8) (((SMS_NV_GW_MSG_STORE_MAJOR_VER) & 0x0000ff00) >> 8);
      /*lint -restore */
      data [3]  = (uint8) ((SMS_NV_GW_MSG_STORE_MAJOR_VER) & 0x000000ff);

      /* Set minor version (number of slots) */
      data [4]  = (uint8) (((cfg_s_ptr->nv_gw_max_slots) & 0xff000000) >> 24);
      data [5]  = (uint8) (((cfg_s_ptr->nv_gw_max_slots) & 0x00ff0000) >> 16);
      data [6]  = (uint8) (((cfg_s_ptr->nv_gw_max_slots) & 0x0000ff00) >> 8);
      data [7]  = (uint8) ((cfg_s_ptr->nv_gw_max_slots) & 0x000000ff);

      /* Set slot length */
      /*lint -save -e572 can be disabled since this is a const macro so we cant cast */
      data [8]  = (uint8) (((SMS_NV_GW_MSG_STORE_SLOT_SIZ) & 0xff000000) >> 24);
      data [9]  = (uint8) (((SMS_NV_GW_MSG_STORE_SLOT_SIZ) & 0x00ff0000) >> 16);
      data [10] = (uint8) (((SMS_NV_GW_MSG_STORE_SLOT_SIZ) & 0x0000ff00) >> 8);
      /*lint -restore */
      data [11] = (uint8) ((SMS_NV_GW_MSG_STORE_SLOT_SIZ) & 0x000000ff);

      /* Masks init */
      memset( data + 12, (int) WMS_TAG_NONE, (cfg_s_ptr->nv_gw_max_slots) );

      bytes_written = mcfg_fwrite( fd, data, (uint32)data_size, MCFG_FS_TYPE_EFS );

      if ( bytes_written != (int) data_size )
      {
         MSG_ERROR_0("Unable to write correctly to header file");
         wms_mem_free( data );
         data = NULL;
         (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
         return FALSE;
      }

      wms_mem_free( data );
      data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    }

    return TRUE;
}
/*===========================================================================
FUNCTION wms_nv_gw_msg_store_create_data_file

DESCRIPTION
  Creates the storage data file and initializes all the slots.

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_gw_msg_store_create_data_file ( void )
{
    int       fd = -1;
    uint32    i = 0;
    int8      *data = NULL;
    int       data_size = 0;
    int       bytes_written = 0;

    fd = mcfg_fopen( SMS_NV_GW_MSG_STORE_DATA, MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );
    if ( fd < 0 )
    {
      /*
       * Unfortunately cant do much at this point...
       * We will try creating the storage before every write operation
       * so hopefully it will eventually succeed..
       */
      MSG_ERROR_0("Cannot make SMS Message Storage");
      return FALSE;

    }
    else
    {
      data_size = (int)SMS_NV_GW_MSG_STORE_SLOT_SIZ;

      data = (int8*) wms_mem_malloc( data_size );

      if ( NULL == data )
      {
        (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
        return FALSE;
      }

      for ( i = 0; i < cfg_s_ptr->nv_gw_max_slots; i++ )
      {
          bytes_written = mcfg_fwrite( fd, data, (uint32)data_size, MCFG_FS_TYPE_EFS );

          if ( bytes_written !=  data_size )
          {
            /* Bad, bad, bad... */
            /* Somehow we were not able to allocate memory correctly for the storage file */
            MSG_ERROR_0("Error in reserving space for sms storage");
            wms_mem_free( data );
            data = NULL;
            (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

            return FALSE;
          }
      }

      wms_mem_free( data );
      data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

      return TRUE;
    }
}

/*===========================================================================
FUNCTION wms_nv_gw_msg_store_update_header_file

DESCRIPTION
    Updates the EFS header file with the value of the passed in tag.
    To be called whenever the tag associated with the slot changes.

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_gw_msg_store_update_header_file ( wms_message_index_type index, uint8 tag )
{
    int       fd = -1;
    int       bytes_written = 0;
    int       offset = 0;

    fd = mcfg_fopen( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_O_WRONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    if ( fd < 0 )
    {
      /*
       * Unfortunately cant do much at this point...
       */
      MSG_ERROR_0("Updating header file failed");
      return FALSE;
    }
    else
    {
        offset = 4   /* Major Version Size */
                 + 4 /* Minor Version Size */
                 + 4 /* Slot size */
                 + (fs_off_t) index;

        /* Go to index position in message store before writing ... */
        offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );
        if ( offset == -1 )
        {
          (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
          return FALSE;
        }

        /* Update tag */
        bytes_written =  mcfg_fwrite( fd, &tag, (uint32) sizeof(tag), MCFG_FS_TYPE_EFS );

         if ( bytes_written != (int) sizeof(tag) )
         {
           MSG_ERROR_0("Unable to update mask");
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
         }

         (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    }

    return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_gw_msg_store_read_tags

DESCRIPTION
    Reads in the meta tag values associated with the slots into
    the *global* buffer used to store this for in RAM

RETURN VALUE
    TRUE if successful
    FALSE if failed

===========================================================================*/
static uint8 wms_nv_gw_msg_store_read_tags ( void )
{
    int       fd = -1;
    int       bytes_read = 0;
    int       offset = 0;
    uint8*    tags = NULL;
    uint32    i;

    /* Check for zero slots */
    if ( 0 == cfg_s_ptr->nv_gw_max_slots )
    {
      MSG_LOW_0("Zero NV slots, no further processing needed for GW");
      return TRUE;
    }

    fd = mcfg_fopen( SMS_NV_GW_MSG_STORE_HEADER, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    if ( fd < 0 )
    {
      MSG_ERROR_0("Read failed");
      return FALSE;
    }
    else
    {
        offset = 4   /* Major Version Size */
                 + 4 /* Minor Version Size */
                 + 4; /* Slot size */

        /* Go to start of meta tag information ... */
        offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );
        if ( offset == -1 )
        {
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
        }

        tags = (uint8*) wms_mem_malloc( cfg_s_ptr->nv_gw_max_slots );
        if ( NULL == tags )
        {
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
        }

        memset( tags, (int) WMS_TAG_NONE, (cfg_s_ptr->nv_gw_max_slots) );

        bytes_read =  mcfg_fread( fd, tags, (uint32) cfg_s_ptr->nv_gw_max_slots, MCFG_FS_TYPE_EFS );

        if ( bytes_read != (int) cfg_s_ptr->nv_gw_max_slots )
        {
           MSG_ERROR_0("Unable read slot meta data correctly");
           wms_mem_free( tags );
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
        }

        /* Set the tags correctly in the "global" buffer */
        for( i = 0; i < cfg_s_ptr->nv_gw_max_slots; i++ )
        {
            cfg_s_ptr->nv_gw_tags[i] = (wms_message_tag_e_type) tags[i];
        }

        wms_mem_free( tags );
        (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    }

    return TRUE;
}

#endif /* FEATURE_GW_SMS */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_authenticate

DESCRIPTION
  Checks if the SMS storage for CD messages is present in EFS.  If not, it
  creates it.  If presents, it validates the version of the file.

RETURN VALUE
  TRUE successful case
  FAIL failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_authenticate( void )
{
  struct    fs_stat temp_fs_buf;
  uint8     result = TRUE;

  /* Create SMS Message Store if necessary */
  if ( ( MCFG_FS_STATUS_OK != mcfg_fs_stat( SMS_NV_CD_MSG_STORE_HEADER, &temp_fs_buf, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 ) ) ||
       ( MCFG_FS_STATUS_OK != mcfg_fs_stat( SMS_NV_CD_MSG_STORE_DATA, &temp_fs_buf, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 ) ) )
  {
    MSG_HIGH_0(" SMS CD storage file(s) not found.  Creating them...");

    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

    result = wms_nv_cd_msg_store_init();
  }
  /* msg store exists, just validate version information */
  else if ( !wms_nv_cd_msg_store_is_version_valid() )
  {
    MSG_HIGH_0(" Version mismatch.  Delete and re-create current version");
    /* TODO: "Convert" versions in the future */
    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

    result = wms_nv_cd_msg_store_init();
  }
  else
  {
    MSG_HIGH_0("SMS_NV_CD_MSG_STORE_DATA authenticated successfully");
  }

  return result;
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_is_version_valid

DESCRIPTION
  Reads the information from the storage file and validates it.

RETURN VALUE
  TRUE/FALSE

===========================================================================*/
static uint8 wms_nv_cd_msg_store_is_version_valid( void )
{
  int   fd = -1;
  uint8 version_info[8];
  int   bytes_read = 0;
  uint32 major_ver = 0;
  uint32 minor_ver = 0;

  /* Open header */
  fd =  mcfg_fopen( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

  if ( fd < 0 )
  {
    MSG_ERROR_0( "Could not open header file");
    return FALSE;
  }
  else /* File opened successfully for reading ... */
  {
    bytes_read =  mcfg_fread( fd, version_info, (uint32) sizeof ( version_info ), MCFG_FS_TYPE_EFS );

    if ( bytes_read != (int) sizeof ( version_info ) )
    {
      MSG_ERROR_0("Unable to read version correctly");

      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;
    }

    major_ver = ((uint32)version_info[0] << 24) |
                ((uint32)version_info[1] << 16) |
                ((uint32)version_info[2] << 8)  |
                 version_info[3];

    minor_ver = ((uint32)version_info[4] << 24) |
                ((uint32)version_info[5] << 16) |
                ((uint32)version_info[6] << 8)  |
                 version_info[7];

    /* In this implementation, if either version is wrong, return error */
    if ( ( SMS_NV_CD_MSG_STORE_MAJOR_VER != major_ver ) ||
         ( minor_ver != cfg_s_ptr->nv_cdma_max_slots ) )
    {
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
      return FALSE;

    }
    (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
  }

  return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_init

DESCRIPTION
  Creates the storage file and initializes all the slots.
  Also creats the header file

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_init ( void )
{
  if ( !wms_nv_cd_msg_store_create_header_file() )
  {
    MSG_ERROR_0("Cannot make SMS Message Storage Header");
    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    return FALSE;
  }

  if ( !wms_nv_cd_msg_store_create_data_file() )
  {
    MSG_ERROR_0("Cannot make SMS Message Storage Header");

    /* Ensure the corrupt files are deleted before returning */
    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    (void) mcfg_fs_delete( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

    return FALSE;
  }

  return TRUE;
}
/*===========================================================================
FUNCTION wms_nv_cd_msg_store_create_header_file

DESCRIPTION
  Creates the storage header file.

RETURN VALUE
  1 successful case
  0 failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_create_header_file ( void )
{
    int       fd = -1;
    uint8     *data = NULL;
    int       data_size = 0;
    int       bytes_written = 0;

    fd = mcfg_fopen( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

    if ( fd < 0 )
    {
      /*
       * Unfortunately cant do much at this point...
       * We will try creating the header before every write operation
       * so hopefully it will eventually succeed..
       */
      MSG_ERROR_0("Cannot make SMS Message Storage Header");
      return FALSE;
    }
    else
    {
      /* Calculate file size */
      data_size += 4; /* Major Version */
      data_size += 4; /* Minor Version (number of slots) */
      data_size += 4; /* Individual Slot length */
      data_size += (cfg_s_ptr->nv_cdma_max_slots); /* Meta info. about each slot */

      data = (uint8*) wms_mem_malloc( data_size );

      if ( NULL == data )
      {
        return FALSE;
      }

      /* Set major version */
      /*lint -save -e572 can be disabled since this is a const macro so we cant cast */
      data [0]  = (uint8) (((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0xff000000) >> 24);
      data [1]  = (uint8) (((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0x00ff0000) >> 16);
      data [2]  = (uint8) (((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0x0000ff00) >> 8);
      /*lint -restore */
      data [3]  = (uint8) ((SMS_NV_CD_MSG_STORE_MAJOR_VER) & 0x000000ff);

      /* Set minor version (number of slots) */
      data [4]  = (uint8) (((cfg_s_ptr->nv_cdma_max_slots) & 0xff000000) >> 24);
      data [5]  = (uint8) (((cfg_s_ptr->nv_cdma_max_slots) & 0x00ff0000) >> 16);
      data [6]  = (uint8) (((cfg_s_ptr->nv_cdma_max_slots) & 0x0000ff00) >> 8);
      data [7]  = (uint8) ((cfg_s_ptr->nv_cdma_max_slots) & 0x000000ff);

      /* Set slot length */
      /*lint -save -e572 can be disabled since this is a const macro so we cant cast */
      data [8]  = (uint8) (((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0xff000000) >> 24);
      data [9]  = (uint8) (((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0x00ff0000) >> 16);
      data [10] = (uint8) (((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0x0000ff00) >> 8);
      /*lint -restore */
      data [11] = (uint8) ((SMS_NV_CD_MSG_STORE_SLOT_SIZ) & 0x000000ff);

      /* Masks init */
      memset( data + 12, (int) WMS_TAG_NONE, (cfg_s_ptr->nv_cdma_max_slots) );

      bytes_written = mcfg_fwrite( fd, data, (uint32)data_size, MCFG_FS_TYPE_EFS );

      if ( bytes_written != (int) data_size )
      {
         MSG_ERROR_0("Unable to write correctly to header file");
         wms_mem_free( data );
         data = NULL;
         (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
         return FALSE;
      }

      wms_mem_free( data );
      data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    }

    return TRUE;
}
/*===========================================================================
FUNCTION wms_nv_cd_msg_store_create_data_file

DESCRIPTION
  Creates the storage data file and initializes all the slots.

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_create_data_file ( void )
{
    int       fd = -1;
    uint32    i = 0;
    int8      *data = NULL;
    int       data_size = 0;
    int       bytes_written = 0;

    fd = mcfg_fopen( SMS_NV_CD_MSG_STORE_DATA, MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );
    if ( fd < 0 )
    {
      /*
       * Unfortunately cant do much at this point...
       * We will try creating the storage before every write operation
       * so hopefully it will eventually succeed..
       */
      MSG_ERROR_0("Cannot make SMS Message Storage");
      return FALSE;

    }
    else
    {
      data_size = (int)SMS_NV_CD_MSG_STORE_SLOT_SIZ;

      data = (int8*) wms_mem_malloc( data_size );

      if ( NULL == data )
      {
        (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
        return FALSE;
      }

      memset( data, (int) WMS_TAG_NONE, data_size );

      for ( i = 0; i < cfg_s_ptr->nv_cdma_max_slots; i++ )
      {
          bytes_written = mcfg_fwrite( fd, data, (uint32)data_size, MCFG_FS_TYPE_EFS );

          if ( bytes_written !=  data_size )
          {
            /* Bad, bad, bad... */
            /* Somehow we were not able to allocate memory correctly for the storage file */
            MSG_ERROR_0("Error in reserving space for sms storage");
            wms_mem_free( data );
            data = NULL;
            (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

            return FALSE;
          }
      }

      wms_mem_free( data );
      data = NULL;
      (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );

      return TRUE;
    }
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_update_header_file

DESCRIPTION
    Updates the EFS header file with the value of the passed in tag.
    To be called whenever the tag associated with the slot changes.

RETURN VALUE
  TRUE successful case
  FALSE failure case

===========================================================================*/
static uint8 wms_nv_cd_msg_store_update_header_file ( wms_message_index_type index, uint8 tag )
{
    int       fd = -1;
    int       bytes_written = 0;
    int       offset = 0;

    fd = mcfg_fopen( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_O_WRONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

    if ( fd < 0 )
    {
      /*
       * Unfortunately cant do much at this point...
       */
      MSG_ERROR_0("Updating header file failed");
      return FALSE;
    }
    else
    {
        offset = 4   /* Major Version Size */
                 + 4 /* Minor Version Size */
                 + 4 /* Slot size */
                 + (fs_off_t) index;

        /* Go to index position in message store before writing ... */
        offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );
        if ( offset == -1 )
        {
          (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
          return FALSE;
        }

        /* Update tag */
        bytes_written =  mcfg_fwrite( fd, &tag, (uint32) sizeof(tag), MCFG_FS_TYPE_EFS );

         if ( bytes_written != (int) sizeof(tag) )
         {
           MSG_ERROR_0("Unable to update mask");
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
         }

         (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    }

    return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_cd_msg_store_read_tags

DESCRIPTION
    Reads in the meta tag values associated with the slots into
    the *global* buffer used to store this for in RAM

RETURN VALUE
    TRUE if successful
    FALSE if failed

===========================================================================*/
static uint8 wms_nv_cd_msg_store_read_tags ( void )
{
    int       fd = -1;
    int       bytes_read = 0;
    int       offset = 0;
    uint8*    tags = NULL;
    uint32    i;

    /* Check for zero slots */
    if ( 0 == cfg_s_ptr->nv_cdma_max_slots )
    {
      MSG_LOW_0("Zero NV slots, no further processing needed for CDMA");
      return TRUE;
    }

    fd = mcfg_fopen( SMS_NV_CD_MSG_STORE_HEADER, MCFG_FS_O_RDONLY, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_0 );

    if ( fd < 0 )
    {
      MSG_ERROR_0("Read failed");
      return FALSE;
    }
    else
    {
        offset = 4   /* Major Version Size */
                 + 4 /* Minor Version Size */
                 + 4; /* Slot size */

        /* Go to start of meta tag information ... */
        offset = mcfg_lseek( fd, offset, SEEK_SET, MCFG_FS_TYPE_EFS );
        if ( offset == -1 )
        {
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
        }

        tags = (uint8*) wms_mem_malloc( cfg_s_ptr->nv_cdma_max_slots );
        if ( NULL == tags )
        {
           return FALSE;
        }
        memset( tags, (int) WMS_TAG_NONE, (cfg_s_ptr->nv_cdma_max_slots) );

        bytes_read =  mcfg_fread( fd, tags, cfg_s_ptr->nv_cdma_max_slots, MCFG_FS_TYPE_EFS );

        if ( bytes_read != (int) cfg_s_ptr->nv_cdma_max_slots )
        {
           MSG_ERROR_0("Unable read slot meta data correctly");
           wms_mem_free( tags );
           (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
           return FALSE;
        }

        /* Set the tags correctly in the "global" buffer */
        for( i = 0; i < cfg_s_ptr->nv_cdma_max_slots; i++ )
        {
            cfg_s_ptr->nv_cdma_tags[i] = (wms_message_tag_e_type) tags[i];
        }

        wms_mem_free( tags );
        (void) mcfg_fclose( fd, MCFG_FS_TYPE_EFS );
    }

    return TRUE;
}

/*===========================================================================
FUNCTION wms_nv_read_cdma_service_option

DESCRIPTION
    Reads NV_SMS_SERVICE_OPTION_I from NV items or RUIM card and map those
    value to WMS format.

RETURN VALUE
    None

===========================================================================*/
void wms_nv_read_cdma_service_option(void)
{
   nv_item_type nvi;

  /* Read Service Option from NV */
  if (NV_DONE_S == wms_nv_read_wait(NV_SMS_SERVICE_OPTION_I, &nvi))
  {
     switch (nvi.sms_service_option)
     {
        case 0:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_AUTO;
           break;
        case 1:
        case WMS_DC_SO_6:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_6;
           break;
        case 2:
        case WMS_DC_SO_14:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_14;
           break;
        default:
           dc_s_ptr->default_so_from_nv = WMS_DC_SO_AUTO;
     }
  }
  else
  {
    /* Write By Default value WMS_DC_SO_6 to NV*/
    dc_s_ptr->default_so_from_nv = WMS_DC_SO_6;
    nvi.sms_service_option =  1; /* 1 correspond to WMS_DC_SO_6 */
    (void) wms_nv_write_wait(NV_SMS_SERVICE_OPTION_I, &nvi);
  }

  MSG_HIGH_1("Service Option = %d", dc_s_ptr->default_so_from_nv);

  {
    wms_client_s_type   *client_ptr = wms_client_ptr_fetch((uint8)WMS_CLIENT_TYPE_INTERNAL);
    if (NULL != client_ptr)
    {
      client_ptr->so = wms_get_client_default_service_option();
      MSG_HIGH_1("Default service option = %d", client_ptr->so);
      ref_cnt_obj_release(client_ptr);
    }
  }
}

/*===========================================================================
FUNCTION wms_nv_read_cdma_channel_setting

DESCRIPTION
    Reads NV_SMS_MO_ON_ACCESS_CHANNEL_I and NV_SMS_MO_ON_TRAFFIC_CHANNEL_I
    from NV items or RUIM card

RETURN VALUE
    None

===========================================================================*/
void wms_nv_read_cdma_channel_setting(void)
{
   nv_item_type nvi;

   if(NV_DONE_S == wms_nv_read_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi))
   {
      dc_s_ptr->is_access_channel_enabled = nvi.sms_mo_on_access_channel;
   }
   else
   {
      /* Set to Default Value & Write to NV items */
      dc_s_ptr->is_access_channel_enabled = 1;
      nvi.sms_mo_on_access_channel = 1;

      (void) wms_nv_write_wait(NV_SMS_MO_ON_ACCESS_CHANNEL_I, &nvi );

      MSG_HIGH_1( "Access Channel = %d ", dc_s_ptr->is_access_channel_enabled);
   }

   if(NV_DONE_S == wms_nv_read_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi))
   {
      dc_s_ptr->is_traffic_channel_enabled = nvi.sms_mo_on_traffic_channel;
   }
   else
   {
      /* Set to Default Value & Write to NV items */
      dc_s_ptr->is_traffic_channel_enabled = 1;
      nvi.sms_mo_on_traffic_channel = 1;

      (void) wms_nv_write_wait(NV_SMS_MO_ON_TRAFFIC_CHANNEL_I, &nvi );

      MSG_HIGH_1( "Traffic Channel = %d ", dc_s_ptr->is_traffic_channel_enabled);
   }

   {
      (void)wms_client_update_channel_pref();
   }
}

/*===========================================================================
FUNCTION wms_nv_read_sms_status

DESCRIPTION
  Read the sms_status item from the non-volatile memory. This in turn will set
  the ruim_last_message_number entry of the config manager.

RETURN VALUE
  void

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_read_sms_status (void)
{
  nv_item_type *nvi_ptr = (nv_item_type *) wms_mem_malloc ( sizeof(nv_item_type) );

  if (NULL != nvi_ptr)
  {
    /* Read Service Option from NV */
    if ( NV_DONE_S == wms_nv_read_wait( NV_RUIM_SMS_STATUS_I, nvi_ptr ) )
    {
      cfg_s_ptr->cdma_last_message_number     = nvi_ptr->ruim_sms_status.msg_id;
    }
    else
    {
      MSG_ERROR_0("Unable to read sms_status_i item from NV...setting default values");
      cfg_s_ptr->cdma_last_message_number       = 0;
    }
  }
}

/*===========================================================================
FUNCTION wms_nv_write_sms_status

DESCRIPTION
  Write the sms_status item to the non-volatile memory.

RETURN VALUE
  void
DEPENDENCIES
  None
===========================================================================*/
void wms_nv_write_sms_status (void)
{
  nv_item_type nvi;

  nvi.ruim_sms_status.msg_id     = ( word )cfg_s_ptr->cdma_last_message_number;
  if ( NV_DONE_S != wms_nv_write_wait ( NV_RUIM_SMS_STATUS_I , &nvi ) )
  {
    MSG_ERROR_0("Unable to write sms_status_i item to NV");
  }
}

#endif /* FEATURE_CDSMS */

/*===========================================================================

FUNCTION wms_nv_efs_map_efs_id_to_filename

DESCRIPTION
  Map EFS id to EFS filename


DEPENDENCIES
  None

RETURN VALUE
  Return NULL or pointer to String

SIDE EFFECTS
  None.

===========================================================================*/

static const char* wms_nv_efs_map_efs_id_to_filename( wms_nv_efs_id_e_type efs_id)
{
  int i;
  int len = sizeof( wms_nv_efs_tbl) / sizeof( wms_nv_efs_tbl[0] );

  for( i = 0; i < len; i++ )
  {
    if ( wms_nv_efs_tbl[i].efs_id == efs_id )
    {
      return wms_nv_efs_tbl[i].file_name;
    }
  }

  MSG_HIGH_0("No match found");

  return NULL;
}

/*===========================================================================
FUNCTION wms_nv_efs_read_mt_msg_limit_in_ecbm

DESCRIPTION
  Read the MMGSDI NV EFS item controlling maximum MT messages limit in ECBM.

  The NV item will only be read once at power up and is not expected to change dynamically.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.
===========================================================================*/
void wms_nv_efs_read_mt_msg_limit_in_ecbm(
  void
)
{
  const char        *file_path     = wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_MAX_MT_ALLOWED_IN_ECBM);
  wms_cfg_s_type    *lcfg_s_ptr    = wms_cfg_s_ptr();
  uint8	             read_nv_value;
  mcfg_fs_status_e_type   res;

  if ( NULL == file_path )
  {
    return;
  }

   res = mcfg_fs_read ( file_path,
                      (void *)&read_nv_value,
                      (uint32) sizeof(read_nv_value),
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_0 );

  if ( MCFG_FS_STATUS_OK != res )
  {
    // Failed to read, item is 'inactive'
    MSG_HIGH_1( "mcfg_fs_read(mt_msg_limit_in_ecbm) status %d", res );
    lcfg_s_ptr->mt_msg_limit_in_ecbm_mode = 10;

    /* Open/Write file */
    res =  mcfg_fs_write( file_path,
                          (void*)&lcfg_s_ptr->mt_msg_limit_in_ecbm_mode,
                          (uint32) sizeof(uint8),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_0);

  }
  else
  {
    // Item was read, update the 'cache'
    lcfg_s_ptr->mt_msg_limit_in_ecbm_mode = (uint8)read_nv_value;
  }
  
  MSG_HIGH_2("wms_nv_efs_read_mt_msg_limit_in_ecbm(): res = %d, Maximum no. of messages allowed in ECBM: %d", res, lcfg_s_ptr->mt_msg_limit_in_ecbm_mode );

  return;
} /* wms_nv_efs_read_mt_msg_limit_in_ecbm() */	


/*===========================================================================
FUNCTION wms_nv_efs_read_sms_allowed_in_ecbm

DESCRIPTION
  Read the MMGSDI NV EFS item indicating whether device is on sprint network.
  The NV item will only be read once at power up and is not expected to change dynamically.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  None.
===========================================================================*/
void wms_nv_efs_read_sms_allowed_in_ecbm(
  void
)
{
  const char       *file_path       = wms_nv_efs_map_efs_id_to_filename( WMS_NV_EFS_ALLOW_SMS_IN_ECBM );
  wms_cfg_s_type   *lcfg_s_ptr      = wms_cfg_s_ptr();
  boolean           read_nv_value;
  mcfg_fs_status_e_type  res;

  if ( NULL == file_path )
  {
    return;
  }

  res = mcfg_fs_read ( file_path,
                      (void *)&read_nv_value,
                      (uint32) sizeof(read_nv_value),
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_0 );

  if ( MCFG_FS_STATUS_OK != res )
  {
    // Failed to read, item is 'inactive'
    lcfg_s_ptr->sms_allowed_in_ecbm = 0;
  }
  else
  {
    // Item was read, update the 'cache'
    lcfg_s_ptr->sms_allowed_in_ecbm = (boolean)read_nv_value;
  }

  MSG_HIGH_2("wms_nv_efs_read_sms_allowed_in_ecbm(): res = %d, SMS is allowed in ECBM mode: %d", res, lcfg_s_ptr->sms_allowed_in_ecbm );

  return;
} /* wms_nv_efs_read_sms_allowed_in_ecbm() */	


/*===========================================================================
FUNCTION wms_nv_efs_read_refresh_vote_ok

DESCRIPTION
  Read the NV EFS item controlling refresh behavior.

  The NV item will only be read once.
  Subsequent requests will return the value initially read (cached).

PARAMETERS
  boolean  *val_out_p   [OUT] This parameter is a reference to where the
                              NV/EFS item value is to be stored.
                              May be NULL if no return value is required.
                              NOTE: No return value will be stored if the
                              item is inactive.

RETURN VALUE
  boolean   The return value indicates the active state of the item.
              TRUE = Item active
              FALSE = Item inactive

DEPENDENCIES
  None
===========================================================================*/
boolean wms_nv_efs_read_refresh_vote_ok(
  boolean                 *val_out_p,
  sys_modem_as_id_e_type  as_id
)
{
  boolean nv_is_active = FALSE;

  //Mark NV as inactive if it hasn't been read.
  if ( !wms_nv_refresh_item.has_been_read[as_id] ) 
  {
    wms_nv_refresh_item.nv_value[as_id] = -1;
  }

  // Only read the item if it hasn't already been cached
  if ( !wms_nv_refresh_item.has_been_read[as_id] )
  {
    const char *file_path = wms_nv_efs_map_efs_id_to_filename( WMS_NV_EFS_MMGSDI_REFRESH_VOTE_OK );
    uint32      res;
    boolean     read_nv_value;

    if ( NULL != file_path )
    {
      /* Open/Read file */
      res = mcfg_fs_read ( file_path,
                          (void *)&read_nv_value,
                          (uint32) sizeof(read_nv_value),
                           MCFG_FS_TYPE_EFS,
                          (mcfg_fs_sub_id_e_type)as_id );

      if ( MCFG_FS_STATUS_OK != res )
      {
        // Failed to read, item is 'inactive'
        MSG_HIGH_2("mcfg_fs_read(refresh_vote_ok) status %d, as id %d", res, as_id);
      }
      else
      {
        // Item was read, update the 'cache'
        wms_nv_refresh_item.nv_value[as_id] = (int)read_nv_value;
      }

      // Activate the 'cache'
      wms_nv_refresh_item.has_been_read[as_id] = TRUE;
    }
  }
  else
  {
    MSG_HIGH_0( "EFS WMS_NV_EFS_MMGSDI_REFRESH_VOTE_OK file map failed" );
  }

  // If item is active, set up returns
  if ( -1 != wms_nv_refresh_item.nv_value[as_id] )
  {
    nv_is_active = TRUE;

    if ( NULL != val_out_p )
    {
      // Convert int to boolean
      *val_out_p = (wms_nv_refresh_item.nv_value[as_id] > 0);
    }
  }

  return nv_is_active;
} /* wms_nv_efs_read_refresh_vote_ok() */

#ifdef FEATURE_GWSMS
/*===========================================================================

FUNCTION wms_nv_efs_read_enable_fdn_control

DESCRIPTION
Read FDN Control EFS item

  MODE
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/

static void wms_nv_efs_read_enable_fdn_control(void)
{
  const char *file_path =
    wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_ENABLE_FDN_CONTROL);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                      (void*)&cfg_s_ptr->enable_fdn_control,
                      (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    cfg_s_ptr->enable_fdn_control = TRUE;

    /* Open/Write file */
    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->enable_fdn_control,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }

  MSG_HIGH_2("wms_nv_efs_read_enable_fdn_control(): res = %d, FDN setting = %d", res, cfg_s_ptr->enable_fdn_control);
}

/*===========================================================================
FUNCTION wms_nv_efs_read_store_to_sim_if_nv_full

DESCRIPTION
  Read the store_to_sim_if_nv_full Flag from NV

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_efs_read_store_to_sim_if_nv_full ( void )
{
  const char *file_path =
    wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_STORE_TO_SIM_IF_NV_FULL);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                      (void*)&cfg_s_ptr->store_to_sim_if_nv_full,
                      (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    cfg_s_ptr->store_to_sim_if_nv_full = FALSE;

    /* Open/Write file */
    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->store_to_sim_if_nv_full,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }

  MSG_HIGH_2("wms_nv_efs_read_store_to_sim_if_nv_full(): res = %d, WMS_NV_EFS_STORE_TO_SIM_IF_NV_FULL setting = %d", res, cfg_s_ptr->store_to_sim_if_nv_full);
} /* wms_nv_efs_read_store_to_sim_if_nv_full() */

/*===========================================================================

FUNCTION wms_nv_efs_read_CS_domain_fallback

DESCRIPTION
This NV efs item is used (when enabled) for retry the SMS on CS domain when
there is a temporary PS failure.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
static void wms_nv_efs_read_CS_domain_fallback(void)
{
  const char *file_path =
     wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_CS_DOMAIN_FALLBACK);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                      (void*)&cfg_s_ptr->cs_domain_fallback_on_lte,
                      (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    /* Open/Write file */
    cfg_s_ptr->cs_domain_fallback_on_lte = TRUE;

    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->cs_domain_fallback_on_lte,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }

  MSG_HIGH_2("wms_nv_efs_read_CS_domain_fallback(): res = %d, CS domain fallback on LTE = %d", res, cfg_s_ptr->cs_domain_fallback_on_lte);
}
#endif /* FEATURE_GWSMS */

#ifdef FEATURE_CDSMS
/*===========================================================================

FUNCTION wms_nv_efs_read_mo_on_data_channel

DESCRIPTION
Read MO ON DATA Channel EFS item

  MODE
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/

static void wms_nv_efs_read_mo_on_data_channel(void)
{
  const char *file_path =
    wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_MO_ON_DATA_CHANNEL);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  MSG_HIGH_1( "Open/Read file=%s", file_path);

  res = mcfg_fs_read( file_path,
                      (void*)&cfg_s_ptr->mo_on_data_channel,
                      (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    MSG_HIGH_1("mcfg_fs_read status %d", res);

    cfg_s_ptr->mo_on_data_channel = TRUE;

    /* Open/Write file */
    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->mo_on_data_channel,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);

    if ( MCFG_FS_STATUS_OK != res )
    {
      MSG_HIGH_1( "mcfg_fs_write status = %d", res);
    }
  }

  MSG_HIGH_1("MO ON DATA Channel = %d", cfg_s_ptr->mo_on_data_channel);
}
#endif /* FEATURE_CDSMS */


/*===========================================================================

FUNCTION wms_nv_efs_read_1x_sms_and_voice

DESCRIPTION
Read 1x SMS and voice EFS item

  MODE
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
static void wms_nv_efs_read_1x_sms_and_voice(void)
{
  unsigned char efs_data;
  const char *file_path =
    wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_1x_SMS_AND_VOICE);

  mcfg_fs_status_e_type res;

  cfg_s_ptr->simul_sms_voice_over_1x_allowed = TRUE;
  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                      &efs_data,
                      (uint32) sizeof(unsigned char),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_0 );

  if ( MCFG_FS_STATUS_OK != res )
  {
    /* Open/Write file */
    efs_data = 1;

    res =  mcfg_fs_write( file_path,
                          (void*)&efs_data,
                          (uint32) sizeof(unsigned char),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_0 );

    if ( MCFG_FS_STATUS_OK != res )
    {
      MSG_HIGH_1( "mcfg_fs_write status = %d", res);
    }

  }
  else
  {
    cfg_s_ptr->simul_sms_voice_over_1x_allowed = (boolean)efs_data;
  }

  MSG_HIGH_2("wms_nv_efs_read_1x_sms_and_voice(): res=%d, 1x SMS and Voice Allowed = %d", res, cfg_s_ptr->simul_sms_voice_over_1x_allowed);
}

#ifdef FEATURE_GWSMS
/*===========================================================================
FUNCTION wms_nv_efs_read_telecom_smsp()

DESCRIPTION
  This NV efs item is used to indicate whether the EFsmsp file from the
  DFtelecom directory should be read to obtain smsp information if the
  USIM EFsmsp file does not exist.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
static void wms_nv_efs_read_telecom_smsp_fallback(
  boolean  *ret_val_ptr
)
{
  boolean      fallback_value = FALSE;
  mcfg_fs_status_e_type       res = MCFG_FS_STATUS_EFS_ERR;
  const char  *file_path      = wms_nv_efs_map_efs_id_to_filename( WMS_NV_EFS_TELECOM_SMSP_FALLBACK );

  if (NULL != file_path)
  {
    /* Open/Read file */
    res = mcfg_fs_read( file_path,
                       (void *)&fallback_value,
                       (uint32) sizeof(fallback_value),
                        MCFG_FS_TYPE_EFS,
                        MCFG_FS_SUBID_NONE );

    if ( MCFG_FS_STATUS_OK != res )
    {
      // Unreadable item or incorrect size
      fallback_value = FALSE;  // Set again, mcfg_fs_read() may have touched it

      /* Open/Write file */
      res =  mcfg_fs_write( file_path,
                            (void *)&fallback_value,
                            (uint32) sizeof(fallback_value),
                            MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                            MCFG_FS_ALLPERMS,
                            MCFG_FS_TYPE_EFS,
                            MCFG_FS_SUBID_NONE);
    }
  }

  if ( NULL != ret_val_ptr )
  {
    // Update/return value at caller's request
    *ret_val_ptr = fallback_value;
  }

  MSG_HIGH_2("wms_nv_efs_read_telecom_smsp(): res = %d, Telecom EFsmsp fallback allowed = %d", res, fallback_value);

  return;
} /* wms_nv_efs_read_telecom_smsp() */

/*===========================================================================
FUNCTION wms_nv_fallback_to_telecom_smsp_is_allowed()

See wmsnv.h for details
==========================================================================*/
boolean wms_nv_fallback_to_telecom_smsp_is_allowed(
  void
)
{
  boolean            fallback_is_allowed = FALSE;

  wms_nv_efs_read_telecom_smsp_fallback(&fallback_is_allowed);

  return fallback_is_allowed;
} /* wms_nv_fallback_to_telecom_smsp_is_allowed() */

#endif

/*===========================================================================

FUNCTION wms_nv_efs_read_sms_retry_limit

DESCRIPTION
This NV efs item is used to limit the number of retries send over the IMS transport.

  MODE
DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None.

==========================================================================*/
static void wms_nv_efs_read_sms_retry_limit(void)
{
  const char *file_path =
     wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_SMS_RETRY_LIMIT);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                     (void *)&cfg_s_ptr->sms_retry_limit,
                     (uint32) sizeof(uint8),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    /* Open/Write file */
    cfg_s_ptr->sms_retry_limit = 1;

    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->sms_retry_limit,
                          (uint32) sizeof(uint8),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }

  MSG_HIGH_2("wms_nv_efs_read_sms_retry_limit(): res = %d, SMS retry limit = %d", res, cfg_s_ptr->sms_retry_limit);
}

/*===========================================================================
FUNCTION wms_nv_efs_read_disable_lte_cb_dup_detection

DESCRIPTION
  Read the disable_lte_cb_dup_detection Flag from NV

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_efs_read_disable_lte_cb_dup_detection ( void )
{
  const char *file_path = wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_DISABLE_LTE_CB_DUP_DETECTION);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                     (void *)&cfg_s_ptr->lte_cb_dup_detection_is_disabled,
                     (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    cfg_s_ptr->lte_cb_dup_detection_is_disabled = FALSE;

    /* Open/Write file */
    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->lte_cb_dup_detection_is_disabled,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }

  MSG_HIGH_2("wms_nv_efs_read_disable_lte_cb_dup_detection(): res %d, WMS_NV_EFS_DISABLE_LTE_CB_DUP_DETECTION setting = %d", res, cfg_s_ptr->lte_cb_dup_detection_is_disabled);
} /* wms_nv_efs_read_disable_lte_cb_dup_detection() */


/*===========================================================================
FUNCTION wms_nv_efs_read_disable_pres_bc_alert

DESCRIPTION
  Read the disable_pres_bc_alert flag from NV EFS file.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_efs_read_disable_pres_bc_alert ( void )
{
  const char *file_path = wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_DISABLE_PRES_BC_ALERT);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */
  res = mcfg_fs_read( file_path,
                     (void *)&cfg_s_ptr->presidential_bc_alert_is_disabled,
                     (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    cfg_s_ptr->presidential_bc_alert_is_disabled = FALSE;

    /* Open/Write file */
    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->presidential_bc_alert_is_disabled,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }

  MSG_HIGH_2("wms_nv_efs_read_disable_pres_bc_alert(): res = %d, WMS_NV_EFS_DISABLE_PRES_BC_ALERT setting = %d", res, cfg_s_ptr->presidential_bc_alert_is_disabled);
} /* wms_nv_efs_read_disable_pres_bc_alert() */

/*===========================================================================
FUNCTION wms_nv_efs_read_stk_sms_enable_retry

DESCRIPTION
  Read the stk_sms_retry_enabled flag from NV EFS file.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void wms_nv_efs_read_stk_sms_enable_retry ( void )
{
  const char *file_path = wms_nv_efs_map_efs_id_to_filename(WMS_NV_EFS_STK_SMS_RETRY_ENABLE);

  mcfg_fs_status_e_type res;

  if ( NULL == file_path )
  {
    return;
  }

  /* Open/Read file */

  MSG_HIGH_1 ( "Open/Read file=%s",file_path);

  res = mcfg_fs_read( file_path,
                     (void *)&cfg_s_ptr->stk_sms_retry_enable,
                     (uint32) sizeof(boolean),
                      MCFG_FS_TYPE_EFS,
                      MCFG_FS_SUBID_NONE );

  if ( MCFG_FS_STATUS_OK != res )
  {
    cfg_s_ptr->stk_sms_retry_enable = FALSE;

    /* Open/Write file */ 
    res =  mcfg_fs_write( file_path,
                          (void*)&cfg_s_ptr->stk_sms_retry_enable,
                          (uint32) sizeof(boolean),
                          MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR,
                          MCFG_FS_ALLPERMS,
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  }
  
  MSG_HIGH_2("wms_nv_efs_read_stk_sms_enable_retry(): res = %d, WMS_NV_EFS_STK_SMS_RETRY_ENABLE setting = %d", res, cfg_s_ptr->stk_sms_retry_enable);
} /* wms_nv_efs_read_stk_sms_enable_retry() */


/*===========================================================================
FUNCTION wms_nv_efs_init

DESCRIPTION
  Initialize WMS EFS items.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void wms_nv_efs_init(void)
{
  struct fs_stat file_stat;
  int conf_fd = -1;

  /* Create wms.conf file */
  if ( MCFG_FS_STATUS_OK != mcfg_fs_stat( WMS_EFS_CONF_F_NAME, &file_stat, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE ) )
  {
    MSG_HIGH_0("wms.conf not present yet.creating it now");

    /* Open /nv/item_files/conf/wms.conf file */
    conf_fd =  mcfg_fopen( WMS_EFS_CONF_F_NAME, MCFG_FS_O_WRONLY | MCFG_FS_O_CREAT | MCFG_FS_O_TRUNC, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, MCFG_FS_SUBID_NONE );

    if ( conf_fd >= 0 )
    {
      (void)mcfg_fwrite( conf_fd, wms_config_items, (uint32)strlen(wms_config_items), MCFG_FS_TYPE_EFS );

    }
  
    (void) mcfg_fclose( conf_fd, MCFG_FS_TYPE_EFS );
    MSG_HIGH_1("wms.conf file fd %d", conf_fd);
  }

#ifdef FEATURE_CDSMS
  /* Read MO ON DATA channel setting */
  wms_nv_efs_read_mo_on_data_channel();
#endif

#ifdef FEATURE_GWSMS
  /* Read FDN setting */
  wms_nv_efs_read_enable_fdn_control();

  //Read STORE_TO_SIM_IF_NV_FULL setting
  wms_nv_efs_read_store_to_sim_if_nv_full();

  /* Read CS domain fallback setting for the SMS retry */
  wms_nv_efs_read_CS_domain_fallback();

#endif

  //Read WMS_NV_EFS_SMS_RETRY_LIMIT setting
  wms_nv_efs_read_sms_retry_limit();

  // Read QIPCALL_1x_SMS_AND_VOICE setting
  wms_nv_efs_read_1x_sms_and_voice();

  //Read WMS_NV_EFS_DISABLE_LTE_CB_DUP_DETECTION setting
  wms_nv_efs_read_disable_lte_cb_dup_detection();

  //WMS_NV_EFS_DISABLE_PRES_BC_ALERT
  wms_nv_efs_read_disable_pres_bc_alert();

  //WMS_NV_EFS_STK_SMS_RETRY_ENABLE
  wms_nv_efs_read_stk_sms_enable_retry();

  // Read NV_EFS_MAX_MT_ALLOWED_IN_ECBM
  wms_nv_efs_read_mt_msg_limit_in_ecbm();

  // Read WMS_NV_EFS_ALLOW_SMS_IN_ECBM
  wms_nv_efs_read_sms_allowed_in_ecbm();
}


/*===========================================================================
FUNCTION wms_nv_get_device_mode

DESCRIPTION:
 Get device mode. Combination number of SIMs and active subscriptions
 determine the device configuration mode.

DEPENDENCIES:
  None

RETURN VALUE:
        WMS_DEVICE_MODE_SS    (1 SIM,  1 active sub)
        WMS_DEVICE_MODE_DSDS  (2 SIMs, 1 active sub)
        WMS_DEVICE_MODE_DSDA  (2 SIMs, 2 active subs)


SIDE EFFECTS
  None.

===========================================================================*/
wms_device_mode_e_type wms_nv_get_device_mode(
   void
)
{
  policyman_status_t         status;
  policyman_item_t const     *pItem      = NULL;
  policyman_item_id_t const  id          = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  wms_device_mode_e_type     device_mode = WMS_DEVICE_MODE_SS;  // Default device mode is single_sim
  size_t                     num_sim;
  size_t                     num_active_sub;

  status = policyman_get_items(&id, 1, &pItem);
  if ( POLICYMAN_SUCCEEDED(status) )
  {
    status = policyman_device_config_num_sims(pItem, &num_sim);
    if ( POLICYMAN_SUCCEEDED(status) )
    {
      status = policyman_device_config_max_active(pItem, &num_active_sub);
      if ( POLICYMAN_SUCCEEDED(status) )
      {
        if ( (2 == num_sim) && (2 == num_active_sub))
        {
          device_mode = WMS_DEVICE_MODE_DSDA;
        }
        else if ( (2 == num_sim) && (1 == num_active_sub))
        {
          device_mode = WMS_DEVICE_MODE_DSDS;
        }
        else if ( (3 == num_sim) && (1 == num_active_sub))
        {
          device_mode = WMS_DEVICE_MODE_TSTS;
        }
        else
        {
          device_mode = WMS_DEVICE_MODE_SS;
        }
      }
    }
  }

  if ( NULL != pItem )
  {
    policyman_item_release(pItem);
  }

  return device_mode;
} /* wms_nv_get_device_mode() */

/*===========================================================================
FUNCTION  wms_nv_policy_chg_ind_proc

DESCRIPTION:
 Process Policy Item Change notification from Policy manager

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  wms_nv_policy_chg_ind_proc(
   wms_policyman_cfg_s_type          *rpt_ptr
)
{
  uint32                   i            = 0;
  msgr_attach_s            *pattach     = NULL;
  sys_modem_as_id_e_type   sub_id       = SYS_MODEM_AS_ID_NONE;

  /* PolicyManager returns a single attachment listing the changed policy items.
     Fetching the attachment to search for device mode item. */
  const policyman_item_collection_t *policy_item = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH_0("wms_policy_chg_ind_proc()");

  if (NULL == rpt_ptr)
  {
    MSG_ERROR_0("Incoming policy message is NULL");
    return;
  }

  pattach = msgr_get_attach(&(rpt_ptr->msg_hdr), 0);
  if ( (NULL == pattach) || (MSGR_ATTACH_REFCNT != pattach->type) )
  {
    MSG_ERROR_0("Invalid attachment. Abort new policy.");
    return;
  }

  policy_item = pattach->data.refcnt.refcnt_obj_ptr;

  if (NULL == policy_item)
  {
    MSG_ERROR_0("Null policy item");
    return;
  }

  /* Check each policy item that has changed. */
  for ( i = 0; i < policy_item->numItems; i++ )
  {
    const policyman_item_t *pitem = NULL;
    policyman_item_id_t     pitem_id;
     
    pitem    = policy_item->pItems[i];
    pitem_id = policyman_item_get_id_msim(pitem, &sub_id);

    switch (pitem_id)
    {
      case POLICYMAN_ITEM_DEVICE_CONFIGURATION:
        {
          wms_cfg_s_type           *cfg_s_ptr   = wms_cfg_s_ptr();
          wms_device_mode_e_type   device_mode;

          device_mode = wms_nv_get_device_mode();

          if (cfg_s_ptr->device_mode != device_mode)
          {
            MSG_HIGH_1("Current device mode = %d", cfg_s_ptr->device_mode);
            cfg_s_ptr->device_mode = device_mode;
            MSG_HIGH_1("Updated device mode = %d", device_mode);
          }

          wms_nv_init_feature_mode();

          break;
        }
      case POLICYMAN_ITEM_UE_MODE:
        {
          wms_cfg_s_type      *cfg_s_ptr = wms_cfg_s_ptr();
          sys_ue_mode_e_type  ue_mode;

          ue_mode = wms_nv_get_ue_mode();

          if (cfg_s_ptr->ue_mode != ue_mode)
          {
            MSG_HIGH_1("Current UE mode = %d", cfg_s_ptr->ue_mode);
            cfg_s_ptr->ue_mode = ue_mode;
            MSG_HIGH_1("Updated UE mode = %d", ue_mode);
          }

          break;
        }

       default:
         break;
    }
  } /* for */

  policyman_msgr_attachments_release(&(rpt_ptr->msg_hdr));
  return;
}  /* wms_policy_chg_ind_proc() */

/*===========================================================================
FUNCTION wms_nv_get_ue_mode

DESCRIPTION:
 Get UE mode. 

DEPENDENCIES:
  None

RETURN VALUE:
  Returns the mode the device is operating in. Example: 

SIDE EFFECTS
  None.

===========================================================================*/
sys_ue_mode_e_type wms_nv_get_ue_mode(
  void
)
{
  policyman_status_t         status;
  const policyman_item_t     *pitem      = NULL;
  const policyman_item_id_t  id          = POLICYMAN_ITEM_UE_MODE;
  sys_ue_mode_e_type         ue_mode     = SYS_UE_MODE_NORMAL;

  status = policyman_get_items(&id, 1, &pitem);
  if ( POLICYMAN_SUCCEEDED(status) )
  {
    status = policyman_get_ue_mode(pitem, &ue_mode);
    if ( POLICYMAN_SUCCEEDED(status) )
    {
      MSG_HIGH_1("UE mode = %d", ue_mode);
    }    
  }

  if ( NULL != pitem )
  {
    policyman_item_release(pitem);
  }

  return ue_mode;
} /* wms_nv_get_ue_mode() */

void wms_nv_init_feature_mode(
  void
)
{
  policyman_status_t         status;
  policyman_item_t const    *pItem;
  policyman_item_id_t const  id           = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  wms_cfg_s_type            *lcfg_s_ptr   = wms_cfg_s_ptr();
  sys_modem_as_id_e_type     as_id;
  size_t                     num_of_sims = 1;
  
  status = policyman_get_items_block(&id, 1, &pItem);
  if (POLICYMAN_SUCCEEDED(status))
  {
    status = policyman_device_config_num_sims(pItem, &num_of_sims);

    if (POLICYMAN_SUCCEEDED( status ))
    {
      num_of_sims = MIN(num_of_sims, (size_t)MAX_AS_IDS);
    }
    else
    {
      num_of_sims = 1;
    }
      
    for( as_id = SYS_MODEM_AS_ID_1; ((size_t)as_id < num_of_sims); as_id++ )
    {
      sys_subs_feature_t temp_sub_feature_mode = SYS_SUBS_FEATURE_MODE_NORMAL;
      if(POLICYMAN_SUCCEEDED(policyman_device_config_get_subs_feature( pItem, 
                                                                       0,
                                                                       (size_t)as_id,
                                                                       &temp_sub_feature_mode )))
      {
        lcfg_s_ptr->sub_feature_mode[as_id] = temp_sub_feature_mode;
      }
      else
      {
        lcfg_s_ptr->sub_feature_mode[as_id] = SYS_SUBS_FEATURE_MODE_NORMAL;
      }

      MSG_HIGH_2("SUB Feature mode for AS_ID %d = %d", as_id, lcfg_s_ptr->sub_feature_mode[as_id]);
    }
  }

  if ( NULL != pItem )
  {
    policyman_item_release(pItem);
  }
    
} /* wms_nv_get_ue_mode() */


boolean wms_nv_rtre_control_is_nv(
  void
)
{
  nv_rtre_control_type   rtre_control_val;

  rtre_control_val = nv_rtre_control();

  MSG_HIGH_1("fetched rtre_control: %d", rtre_control_val);

  return (NV_RTRE_CONTROL_NO_RUIM == rtre_control_val);
} /* wms_nv_rtre_control_is_nv() */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION WMS_NV_REFRESH_CB

DESCRIPTION
  Prcoess the NV refresh event.

RETURN VALUE
  None.

DEPENDENCIES
  NONE.
===========================================================================*/
void wms_nv_refresh_cb_proc(
  wms_cmd_type *cmd_ptr
)
{
  mcfg_refresh_info_s_type refresh_info;
  boolean                  nv_refresh_vote_ok;
  boolean                  item_is_active;

  if (NULL == cmd_ptr)
  {
    MSG_HIGH_0("Null cmd_ptr in wms_nv_refresh_cb_proc()");
    return;
  }

  refresh_info = cmd_ptr->cmd.nv_refresh_info.refresh_info; // This is of no use as of now except for reference id field .
  
  MSG_HIGH_2("In wms_nv_refresh_cb_proc(): refresh type %d slot %d", refresh_info.type, refresh_info.slot_index);

  if (SYS_MODEM_AS_ID_1 == cmd_ptr->hdr.as_id) 
  {
     // Read NV_EFS_MAX_MT_ALLOWED_IN_ECBM
     wms_nv_efs_read_mt_msg_limit_in_ecbm();

     // Read WMS_NV_EFS_ALLOW_SMS_IN_ECBM
     wms_nv_efs_read_sms_allowed_in_ecbm();  
  }

  //Read WMS_NV_EFS_MMGSDI_REFRESH_VOTE_OK
  wms_nv_refresh_item.has_been_read[cmd_ptr->hdr.as_id] = FALSE;
  item_is_active = wms_nv_efs_read_refresh_vote_ok(&nv_refresh_vote_ok, cmd_ptr->hdr.as_id);
  MSG_HIGH_2("NV Refresh vote ok: active = %d,  value = %d", item_is_active, nv_refresh_vote_ok);

  if (FALSE == mcfg_refresh_done(refresh_info.reference_id))
  {
    MSG_ERROR_0("mcfg_refresh_done notification failed");
  }

  return;  
}  /* wms_nv_refresh_cb_proc() */

/*===========================================================================
FUNCTION WMS_NV_REFRESH_CB

DESCRIPTION
  Callback registered with MCFG module to handle NV refresh event.

RETURN VALUE
  Boolean.

DEPENDENCIES
  NONE.
===========================================================================*/
boolean wms_nv_refresh_cb(
  mcfg_refresh_info_s_type *p_info
)
{
  wms_cmd_type   *cmd_ptr;

  cmd_ptr = wms_get_cmd_buf();

  if ( NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id	                       =  WMS_CMD_NV_REFRESH_CB;
    cmd_ptr->hdr.client_id	               =  (wms_client_id_type)WMS_CLIENT_TYPE_INTERNAL;
    cmd_ptr->hdr.as_id                         =  (sys_modem_as_id_e_type)p_info->sub_index;
    cmd_ptr->hdr.cmd_cb	                       =  NULL;
    cmd_ptr->hdr.user_data	               =  NULL;
    cmd_ptr->hdr.appInfo_ptr	               =  NULL;
    cmd_ptr->cmd.nv_refresh_info.refresh_info  = *p_info;

    wms_put_cmd( cmd_ptr );
  }
  else
  {
    return FALSE;
  }

  return TRUE;
} /* wms_nv_refresh_cb() */
#endif /* FEATURE_MODEM_CONFIG_REFRESH */


#endif /* FEATURE_CDSMS || FEATURE_GWSMS */
