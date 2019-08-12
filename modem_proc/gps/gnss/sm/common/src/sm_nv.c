/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Session Manager NV Items Module

GENERAL DESCRIPTION
  This module contains the SM interface routines for the Non-Volatile Memory
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006, 2011-2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Copyright (c) 2013 - 2014 QUALCOMM Atheros, Inc.
All Rights Reserved.
QCA Proprietary and Confidential.
 
Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_nv.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/04/15   js      Default Enablement of XTRA download 
03/14/15   rh      Added support for MultiSIM NV 
04/17/14   skm     NV Async Read changes 
06/11/10   gk      Enable XTRA and time injection by default
02/10/10   vp      OEM DR Data Enablement
06/10/09   gk      Added Get UTC offset from NV
01/15/09    atien  XTRA-T support
01/25/09   ns      Added support for QWiP NV item
08/22/08   jlp     Added support of emergency call simulation for simulating
                   E911 A-GPS
09/14/07    gk     NMEA CONFIG set parameters support
08/01/07    rw     Add support for XTRA feature
07/31/07    ank    Added Dynamic QoS and Override logic.
05/01/07    lt     Restore the default for cgps_mo_method back to MO_CP.
04/17/07    lt     Changed default for cgps_mo_method to MO_CP_LOC_EST.
04/12/07    ah     Added default read values for all SM-related NV items.
12/22/06    ank    Added default values for various NV items.
11/14/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include "gps_variation.h"
#include "comdef.h"
//#include "customer.h"

#include "sm_nv.h"

#include "msg.h"
#ifdef FEATURE_CGPS_CDMA_IF
#include "mccdma.h"
#ifdef FEATURE_CMI
#include "mccdma_v.h"
#endif
#endif
#include "pdapibuf.h"
#include "tm_api.h"
#include "gnss_common.h"
#include "aries_os_api.h"

/*
 * Constant definitions
*/
#define C_SM_NV_READ_SIGNAL   0x01

/*
 * Structure definitions
*/
typedef struct
{
  /*1X PDE Server Ipv4 Address */
  uint32   cgps_1x_pde_server_addr_ipv4;  
  /*1X PDE Server Port */
  uint32   cgps_1x_pde_server_port;
  /*1X PDE Server URL */
  uint8  cgps_1x_pde_server_addr_url[128];  
  /*NMEA Config Info*/
  nv_nmea_config_info_type cgps_nmea_config_info;
  /*UMTS PDE Server Ipv4 Address */
  uint32   cgps_umts_pde_server_addr_ipv4;  
  /*UMTS PDE Server Port */
  uint32   cgps_umts_pde_server_port;
  /*XTRA download interval*/
  uint16   gps1_xtra_download_interval;
  /*XTRA Auto Download Enabled*/
  boolean  gps1_xtra_auto_download_enabled;
  /* Item to disable or enable Vx LCS Agent functionality. */
  uint8  gps1_vx_lcs_agent;
  /* 1x MPC Server Address IPV4 */
  uint32 cgps_1x_mpc_server_addr_ipv4;
  /* 1x MPC Server Port */
  uint32 cgps_1x_mpc_server_port;  
  /* 1x MPC Server Address URL */
  uint8  cgps_1x_mpc_server_addr_url[128];
   /* MT LR Support */
  byte   aagps_mt_lrsupport;
  /* MO Method */
  uint8  cgps_mo_method;
 /* Simple gpsOne PD API password */
  nv_gpsone_password_type  gpsone_password;  
 /* Rate of security updates sent to client */
  uint8 gps1_sec_update_rate; 
 /* Configures the On-Demand Positioning support */
  uint8 cgps_on_demand_enabled; 
 /* 2G MO-LR Support */
  byte  aagps_2g_mo_lrsupport; 
  /* 3F MO LR Support */
  byte  aagps_3g_mo_lrsupport; 
  /*SBAS User Preference*/
  uint8  cgps_sbas_user_preference;    

/* Below are carrier-specific NVs (one copy per sub) */
  /* GPS Lock */
  dword   gps1_lock[SM_CONFIG_MAX_SUBS];
  /*NMEA Sentence Type*/
  uint8  gnss_nmea_sentence_type[SM_CONFIG_MAX_SUBS];
  /*NMEA Extended Sentence Type*/
  uint16   gnss_nmea_extended_sentence_type[SM_CONFIG_MAX_SUBS];
  /*UMTS PDE Server URL */
  uint8  cgps_umts_pde_server_addr_url[SM_CONFIG_MAX_SUBS][128];
  /*SUPL Version*/
  uint32   gnss_supl_version[SM_CONFIG_MAX_SUBS];  
  /* Enable User Plane Secure Transport */
  uint8  aagps_use_transport_security[SM_CONFIG_MAX_SUBS];  
  /* Positioning Modes Supported */
  uint32 aagps_positioning_modes_supported[SM_CONFIG_MAX_SUBS];  

} sm_nv_cached_struct_type;

#ifdef FEATURE_GNSS_NO_NV_WRITES
#error code not present
#endif
static boolean           sm_nv_initialized = FALSE;
static nv_cmd_type       sm_nv_cmd;
static q_type            sm_nv_cmd_q;
static sm_nv_clnt_s_type sm_nv_clnt_tbl[SM_NV_CLIENT_HANDLE_MAX];

/* current Subs ID for carrier-specific legacy/EFS NV, default is 0 (subscription independent)
   This value must NOT be -1, should always be valid 0~1 or 0~2 (even if phone is OOS) */
sys_modem_as_id_e_type sm_nv_current_sub = SM_CONFIG_MIN_SUB_ID;
/* Max valid Subscription ID at run-time */
sys_modem_as_id_e_type sm_nv_max_sub = SM_CONFIG_MAX_SUB_ID;


/*
 * Function definitions
*/
/*===========================================================================

FUNCTION sm_nv_efs_map_sub

DESCRIPTION
  This function is used to map Subscription ID to proper value (if it's
  subscription-indepdendent NV, it'll always map to Subs 0)
 
  
RETURN VALUE
  cgps_nv_sub_id_type 
  
DEPENDENCIES
 
===========================================================================*/
static sys_modem_as_id_e_type sm_nv_map_sub(nv_items_enum_type nv_item, sys_modem_as_id_e_type sub)
{
  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d (Max=%d)", sub, sm_nv_max_sub);
    return SM_CONFIG_MIN_SUB_ID;
  }

  switch (nv_item)
  {
    case NV_GPS1_LOCK_I:
    //case NV_CGPS_NMEA_CONFIG_INFO_I:
    case NV_GNSS_NMEA_SENTENCE_TYPE_I:
    case NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I:
    //case NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I:
    //case NV_CGPS_UMTS_PDE_SERVER_PORT_I:
    case NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I:
    case NV_GNSS_SUPL_VERSION_I:
    case NV_AAGPS_USE_TRANSPORT_SECURITY_I:
    case NV_AAGPS_POSITIONING_MODES_SUPPORTED_I:
      return sub; /* subscription-dependent NV, return sub */

    default:
      return SM_CONFIG_MIN_SUB_ID; /* subscription-independent EFS NV, return 0 */
  }
}


  /*===========================================================================
  
  FUNCTION sm_nv_load_default_values_to_cache
  
  DESCRIPTION
    This function is used to load default values to cache
    
  RETURN VALUE
    None
    
  DEPENDENCIES
   
  ===========================================================================*/
void sm_nv_load_default_values_to_cache(void)
{

#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif /*FEATURE_GNSS_NO_NV_WRITES*/   
}

/*===========================================================================

FUNCTION sm_nv_init

DESCRIPTION
  This function is used to initialize internal SM-NV data structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/

void sm_nv_init (void)
{
  int i = 0;

  if (sm_nv_initialized == FALSE)
  {
    (void)q_init(&sm_nv_cmd_q);

    for (i = 0; i < SM_NV_CLIENT_HANDLE_MAX; i++)
    {
      sm_nv_clnt_tbl[i].active    = FALSE;
      sm_nv_clnt_tbl[i].thread_id = (uint32)THREAD_ID_ERROR;
      sm_nv_clnt_tbl[i].nv_write_cb_fn  = NULL;
      sm_nv_clnt_tbl[i].nv_read_cb_fn = NULL;
    }

    sm_nv_initialized = TRUE;
    sm_nv_load_default_values_to_cache();
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: already initialized");
  }
}

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
                       sm_nv_cmd_cb_f_type nv_clnt_read_cb_fn)
{
  uint32 i = 0;

  /* SM NV Module Init sanity check */
  if (sm_nv_initialized == FALSE)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "SM_NV_INIT: Initializing SM NV module");
    sm_nv_init();
  }

  /* Passed argument sanity check */
  if ((os_ThreadIdEnumType)thread_id >= THREAD_ID_MAX)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: FAILED - Unknown parameter");
    return (uint32)SM_NV_CLIENT_HANDLE_ERROR;
  }

  /* Passed argument sanity check */
  if ((nv_clnt_write_cb_fn == NULL) || (nv_clnt_read_cb_fn == NULL))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: FAILED - NULL parameter");
    return (uint32)SM_NV_CLIENT_HANDLE_ERROR;
  }

  /* Look for room in the client table to register NV write cmd callback */
  for (i = 0; i < SM_NV_CLIENT_HANDLE_MAX; i++)
  {
    if (sm_nv_clnt_tbl[i].active == TRUE)
    {
      if (sm_nv_clnt_tbl[i].thread_id == thread_id)
      {
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: already initialized");
        return i;
      }
    }
    else /* sm_nv_clnt_tbl[i].active == FALSE */
    {
      sm_nv_clnt_tbl[i].active    = TRUE;
      sm_nv_clnt_tbl[i].thread_id = thread_id;
      sm_nv_clnt_tbl[i].nv_write_cb_fn  = nv_clnt_write_cb_fn;
      sm_nv_clnt_tbl[i].nv_read_cb_fn = nv_clnt_read_cb_fn;
      return i;
    }
  }

  /* Can't initialize since maximum number of clients initialized */
  MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_INIT: FAILED - max # of clients reached");
  return (uint32)SM_NV_CLIENT_HANDLE_ERROR;
}

/*===========================================================================

FUNCTION sm_nv_write_to_cache_sub

DESCRIPTION
  This function is used to write NV to cache for a subscription

RETURN VALUE
  TRUE  - NV Cache write command successful
  FALSE - NV Cache write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_cache_sub(nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif
  
  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Cache: FAILED - NULL parameter");
    return FALSE;
  }
  
  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Cache: FAILED Invalid sub %d (Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

#ifdef FEATURE_GNSS_NO_NV_WRITES
   #error code not present
#else /*FEATURE_GNSS_NO_NV_WRITES*/
  return FALSE;
#endif /*FEATURE_GNSS_NO_NV_WRITES*/
  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_write_to_cache

DESCRIPTION
  This function is used to write NV to cache for current DD sub

RETURN VALUE
  TRUE  - NV Cache write command successful
  FALSE - NV Cache write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_cache(nv_items_enum_type nv_item, void* item_ptr)
{
  return sm_nv_write_to_cache_sub(nv_item, item_ptr, 
                                  sm_nv_map_sub(nv_item, sm_nv_current_sub) );
}

/*===========================================================================

FUNCTION sm_nv_write_to_hw

DESCRIPTION
  This function is used to write NV to Hw (for sub-independent NV)

RETURN VALUE
  TRUE  - NV Hw write command successfully sent
  FALSE - NV Hw write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_hw(nv_items_enum_type nv_item, void* item_ptr)
{
  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Hw: FAILED - NULL parameter");
    return FALSE;
  }

  /* Populate NV write command */
  sm_nv_cmd.item       = nv_item;
  sm_nv_cmd.cmd        = NV_WRITE_F;
  sm_nv_cmd.data_ptr   = (nv_item_type*)item_ptr;
  sm_nv_cmd.tcb_ptr    = rex_self();
  sm_nv_cmd.sigs       = C_OS_FLAG_NV_WRITE;
  sm_nv_cmd.done_q_ptr = NULL;

  
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Write: %d", nv_item);
  nv_cmd(&sm_nv_cmd);

  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_write_to_hw_sub

DESCRIPTION
  This function is used to write NV to Hw for a specific sub

RETURN VALUE
  TRUE  - NV Hw write command successfully sent
  FALSE - NV Hw write command unsuccessful

DEPENDENCIES

===========================================================================*/
static boolean sm_nv_write_to_hw_sub(nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
  /* Extended NV command struct for dual sim */
  nv_cmd_ext_type nvcmd_ext;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV Write TO Hw sub: FAILED - NULL parameter");
    return FALSE;
  }

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d(Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

  /* Populate NV write command */
  sm_nv_cmd.item       = nv_item;
  sm_nv_cmd.cmd        = NV_WRITE_F;
  sm_nv_cmd.data_ptr   = (nv_item_type*)item_ptr;
  sm_nv_cmd.tcb_ptr    = rex_self();
  sm_nv_cmd.sigs       = C_OS_FLAG_NV_WRITE;
  sm_nv_cmd.done_q_ptr = NULL;

  nvcmd_ext.nvcmd   = &sm_nv_cmd;
  nvcmd_ext.context = (uint16)sub;

  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Write: %d (sub=%d)", nv_item, sub);
  nv_cmd_ext(&nvcmd_ext);

  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_async_read_hw

DESCRIPTION
  This function is used for Async NV Read's from Hw 

RETURN VALUE
  TRUE  - NV Hw write command successfully sent
  FALSE - NV Hw write command unsuccessful

DEPENDENCIES

===========================================================================*/
boolean sm_nv_async_read_hw(nv_items_enum_type nv_item, void* item_ptr)
{
  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM NV async read Hw: FAILED - NULL parameter");
    return FALSE;
  }

  /* Populate NV READ command */
  sm_nv_cmd.item       = nv_item;
  sm_nv_cmd.cmd        = NV_READ_F;
  sm_nv_cmd.data_ptr   = (nv_item_type*)item_ptr;
  sm_nv_cmd.tcb_ptr    = rex_self();
  sm_nv_cmd.sigs       = C_OS_FLAG_NV_READ;
  sm_nv_cmd.done_q_ptr = NULL;

  
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Async hw read: %d", nv_item);
  nv_cmd(&sm_nv_cmd);

  return TRUE;
}

/*===========================================================================

FUNCTION sm_nv_handle_rdwr_cmd

DESCRIPTION
  This function is used to handle a NV HW Async-Read/Write command for NV.
  Function services the request at the head of the NV Read/Write command queue
  and waits until NV returns with a response.Function checks the CMD type
  "nv-ptr->cmd" to call appropriate Asyns-read/Write function.
  
RETURN VALUE
  None

DEPENDENCIES
  SM NV async-read/write callback needs to be registered via call to
  sm_nv_write_reg/sm_nv_async_read_reg

===========================================================================*/

static void sm_nv_handle_rdwr_cmd (void)
{
  sm_nv_item_s_type* nv_ptr;
#ifndef FEATURE_GNSS_NO_NV_WRITES  
  boolean nv_hw_write_needed = FALSE;
#endif /* ! FEATURE_GNSS_NO_NV_WRITES */

  if (sm_nv_initialized)
  {
    nv_ptr = (sm_nv_item_s_type*)q_check(&sm_nv_cmd_q);

    /* Send NV write command to NV module only if a request exists in queue */
    if ((nv_ptr != NULL) && (nv_ptr->cmd == NV_WRITE_F))
    {      
      sm_nv_cmd.item = nv_ptr->item;
#ifdef FEATURE_GNSS_NO_NV_WRITES
      #error code not present
#else
      nv_hw_write_needed = TRUE;
      if(sm_nv_write_to_cache_sub(nv_ptr->item, 
                                  (nv_item_type*)&(nv_ptr->data),
                                  nv_ptr->sub))
      {
        MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Cache Write: %d (sub=%d)", nv_ptr->item, nv_ptr->sub); 
        nv_hw_write_needed = FALSE;
      }

      if(nv_hw_write_needed)
      {
        /* Send NV write command to NV module */
        if (nv_ptr->sub == 0)
        {
          sm_nv_write_to_hw(nv_ptr->item, (nv_item_type*)&(nv_ptr->data));        
        }
        else
        {
          sm_nv_write_to_hw_sub(nv_ptr->item, (nv_item_type*)&(nv_ptr->data), nv_ptr->sub);        
        }
        MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Write: %d (sub=%d)", nv_ptr->item, nv_ptr->sub);
      }
      else
      {
        /* If the write was sent to the NV module, once the data was written to 
          * hw, rex signal 'C_OS_FLAG_NV_WRITE' would be sent to tm task, which 
          * is handled in sm_nv_write_done(). As the data is not flushed to hw, 
          * at this point, the NV write is done. Simply call sm_nv_write_done()
          * to invoke the client cb */
      
        sm_nv_cmd.status = NV_DONE_S;
        sm_nv_rdrw_done();   
      }
#endif
    }

    if ((nv_ptr != NULL) && (nv_ptr->cmd == NV_READ_F)) 
    {
      sm_nv_cmd.item = nv_ptr->item;
      {
        /* Send NV Aync Read command to NV module
           Function used for reading RUIM NV's*/
        sm_nv_async_read_hw(nv_ptr->item, (nv_item_type*)&(nv_ptr->data));        
      }
    }
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_HANDLE_WR_CMD: SM NV not initialized");
  }
}

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

boolean sm_nv_write (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle)
{
  return sm_nv_write_to_sub(nv_item, item_ptr, handle, 
                            sm_nv_map_sub(nv_item, sm_nv_current_sub)); /* map to current DD sub */
}

/*===========================================================================

FUNCTION sm_nv_read_default

DESCRIPTION
  This function is used to return the default value for the NV item to be
  read. This should be used only when failing to read from NV.

RETURN VALUE
  None

DEPENDENCIES
 
===========================================================================*/

void sm_nv_read_default (nv_items_enum_type nv_item, void *item_ptr)
{
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_Default - NULL parameter");
    return;
  }

  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM_NV_READ: Default NV item(%d) returned", nv_item);

  switch (nv_item)
  {
    /*******************************************/
    /* Transport Manager (TM) Related NV Items */
    /*******************************************/

    case NV_CGPS_1X_PDE_SERVER_ADDR_IPV4_I:
      /* Default value: 1x PDE Server Address IPV4 = 0 */
      ((nv_item_type*)item_ptr)->cgps_1x_pde_server_addr_ipv4 = 0; //uint32
      break;

    case NV_CGPS_1X_PDE_SERVER_PORT_I:
      /* Default value: 1x PDE Server Port = 0 */
      ((nv_item_type*)item_ptr)->cgps_1x_pde_server_port = 0; //uint32
      break;

    case NV_CGPS_1X_MPC_SERVER_ADDR_IPV4_I:
      /* Default value: 1x MPC Server Address IPV4 = 0 */
      ((nv_item_type*)item_ptr)->cgps_1x_mpc_server_addr_ipv4 = 0; //uint32
      break;

    case NV_CGPS_1X_MPC_SERVER_PORT_I:
      /* Default value: 1x MPC Server Port = 0 */
      ((nv_item_type*)item_ptr)->cgps_1x_mpc_server_port = 0; //uint32
      break;

    case NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV4_I:
      /* Default value: UMTS PDE Server Address IPV4 = 0 */
      ((nv_item_type*)item_ptr)->cgps_umts_pde_server_addr_ipv4 = 0; //uint32
      break;
      
    case NV_CGPS_UMTS_PDE_SERVER_ADDR_IPV6_I:
      /* Default value: UMTS PDE Server Address IPV6 = 0 */
      memset((void *)&(((nv_item_type*)item_ptr)->cgps_umts_pde_server_addr_ipv6[0]),0, sizeof(uint16) * 8);
      break;

    case NV_CGPS_UMTS_PDE_SERVER_PORT_I:
      /* Default value: UMTS PDE Server Port = 0 */
      ((nv_item_type*)item_ptr)->cgps_umts_pde_server_port = 0; //uint32
      break;

    case NV_GPS1_LOCK_I:
      #ifdef FEATURE_GNSS_LOCK_AT_BOOTUP
      /* Default value: GPS Lock = LOCK_ALL if the feature is defined */
      ((nv_item_type*)item_ptr)->gps1_lock = (dword)PDSM_GPS_LOCK_ALL;
      #else
      /* Default value: GPS Lock = LOCK_NONE if the feature is not defined */
      ((nv_item_type*)item_ptr)->gps1_lock = (dword)PDSM_GPS_LOCK_NONE;
      #endif
      MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH ,"Inside default NV_GPS1_LOCK_I, Value: %d", ((nv_item_type*)item_ptr)->gps1_lock);
      break;

    case NV_GNSS_NMEA_SENTENCE_TYPE_I:
      ((nv_item_type*)item_ptr)->gnss_nmea_sentence_type = NV_GNSS_NMEA_DEFAULT_GNSS_SENTENCE_TYPE;
      break;

    case NV_GNSS_NMEA_EXTENDED_SENTENCE_TYPE_I:
      ((nv_item_type*)item_ptr)->gnss_nmea_extended_sentence_type = NV_GNSS_NMEA_DEFAULT_GNSS_EXTENDED_SENTENCE_TYPE;
      break;

    case NV_AAGPS_DEFAULT_REF_TIME_UNC_I:
      /* Default value: Default Reference Time Uncertainty */
      /* Unit for this item: 50ms, so Default value = 160 or 8000 ms */
      ((nv_item_type*)item_ptr)->aagps_default_ref_time_unc = NV_AAGPS_DEFAULT_REF_TIME_UNC_V; //uint32
      break;

    case NV_AAGPS_DEFAULT_REF_POSITION_UNC_I:
      /* Default value: Default Reference Position Uncertainty */
      /* Unit for this item: 1 meter, so Default value = 10000 */
      ((nv_item_type*)item_ptr)->aagps_default_ref_position_unc = NV_AAGPS_DEFAULT_REF_POSITION_UNC_V; //uint32
      break;

    case NV_AAGPS_GPS_LOCK_CONTROL_I:
      /* Default value: not locked  -- value of 0 */
      ((nv_item_type*)item_ptr)->aagps_gps_lock_control = NV_AAGPS_DEFAULT_GPS_LOCK_CONTROL_V;
      break;

    case NV_AAGPS_RTI_VALIDITY_DUR_I:
      /* Default value:  4 hours */
      ((nv_item_type*)item_ptr)->aagps_rti_validity_dur = NV_AAGPS_RTI_VALIDITY_DUR_V;
      break;
      
    case NV_GNSS_GLO_CONTROL_I:
      /* default: GLONASS ENABLED as MC module enabled at boot time */
      ((nv_item_type*)item_ptr)->gnss_glo_control = 1;
      break;
        

    /**************************************/
    /* Link Manager (LM) Related NV Items */
    /**************************************/

    case NV_AAGPS_APP_TRACKING_GPSON_THSLD_I:
      /* Default value: Application-Based Tracking GPS On Threshold = 5 */
      ((nv_item_type*)item_ptr)->aagps_app_tracking_gpson_thsld = NV_AAGPS_APP_TRACKING_GPSON_THSLD_V; //uint32
      break;

    case NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_I:
      /* Default value: Application-Based Tracking GPS Idle Threshold = 15 */
      ((nv_item_type*)item_ptr)->aagps_app_tracking_gpsidle_thsld = NV_AAGPS_APP_TRACKING_GPSIDLE_THSLD_V; //uint32
      break;

    case NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_I:
      /* Default value: Application-Based Tracking Periodic Request Delay Margin = 60 */
      ((nv_item_type*)item_ptr)->aagps_app_trkg_periodic_req_dly_margin = NV_AAGPS_APP_TRKG_PERIODIC_REQ_DLY_MARGIN_V; //uint32
      break;

    case NV_CGPS_QOS_OVERRIDE_ACC_THRESHOLD_I:
      /* Default value: QoS Override Accuracy Threshold [meters] = 0 */
      ((nv_item_type*)item_ptr)->cgps_qos_override_acc_threshold = 0; //uint16
      break;

    case NV_CGPS_QOS_OVERRIDE_TIME_I:
      /* Default value: QoS Override Time [seconds] = 0 */
      ((nv_item_type*)item_ptr)->cgps_qos_override_time = 0; //uint8
      break;

    /**************************************************************/
    /* OEM Related NV Items */
    /**************************************************************/
    case NV_GNSS_OEM_FEATURE_MASK_I:
#ifdef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT      
      /* Default value: OEM DRE Feature data ENABLE Accept MT over MO */
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask = NV_GNSS_OEM_FEATURE_MASK_3_1XCPMT; //uint32
      /* Default value: OEM DRE Feature data ENABLE Always Accept SUPL INIT */
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask |= NV_GNSS_OEM_FEATURE_MASK_6_SUPLINIT_NOTIFY; //uint32
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask |= NV_GNSS_OEM_FEATURE_MASK_7_AUTO_ACCEPT_MTLR_IN_EMERGENCY; //uint32
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask |= NV_GNSS_OEM_FEATURE_MASK_9_KILL_PERIODIC_AT_EMERG_END; //uint32
#else
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask |= NV_GNSS_OEM_FEATURE_MASK_10_REQ_XTRA_DURING_SUPL; //uint32
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask |= NV_GNSS_OEM_FEATURE_MASK_12_REQ_XTRA_DURING_1XUP; //uint32
      ((nv_item_type*)item_ptr)->gnss_oem_feature_mask |= NV_GNSS_OEM_FEATURE_MASK_13_END_SUPL_AFTER_FINAL_FIX; //uint32
#endif
      break;

    /**************************************************************/
    /* Protocol Submodule: 1x Control Plane (CP) Related NV Items */
    /**************************************************************/

    case NV_GPS1_NET_DBM_SIZE_I:
      /* Default value: Net DBM Size = 200 */
      ((nv_item_type*)item_ptr)->gps1_net_dbm_size = PDSM_MAX_PDDM_DEFAULT_VAL; //byte
      break;

    /***********************************************************/
    /* Protocol Submodule: 1x User Plane (UP) Related NV Items */
    /***********************************************************/

    case NV_CURR_NAM_I:
      /* Default value: Current NAM = 0 */
      ((nv_item_type*)item_ptr)->curr_nam = 0; //byte
      break;

#ifdef FEATURE_CGPS_CDMA_IF
    case NV_MIN1_I:
      /* Default value: MIN 1 (phone number) for the given NAM, in quotes, or encoded MIN1 */
      ((nv_item_type*)item_ptr)->min1.min1[NV_CDMA_MIN_INDEX] = IMSI_S1_ZERO; //nv_min1_type
      break;

    case NV_MIN2_I:
      /* Default value: MIN 2 (area code) for the given NAM, in quotes, or encoded MIN2 */
      ((nv_item_type*)item_ptr)->min2.min2[NV_CDMA_MIN_INDEX] = IMSI_S2_ZERO; //nv_min2_type
      break;

    case NV_IMSI_MCC_I:
      /* Default value: Mobile Country Code: string of 3 digits in quotes, or encoded MCC */
      ((nv_item_type*)item_ptr)->imsi_mcc.imsi_mcc = IMSI_MCC_ZERO; //nv_imsi_mcc_type
      break;

    case NV_IMSI_11_12_I:
      /* Default value: True IMSI Mobile Network Code for given NAM */
      ((nv_item_type*)item_ptr)->imsi_11_12.imsi_11_12 = IMSI_11_12_ZERO; //nv_imsi_11_12_type
      break;
#endif

    case NV_GPS1_MSB_THROTTLE_ENABLE_I:
      /* Default value: MS-Based Aiding Data Throttle Enable = TRUE */
      ((nv_item_type*)item_ptr)->gps1_msb_throttle_enable = GPSONE_MSB_THROTTLE_ENABLE; //boolean
      break;

    case NV_GPS1_MSB_BACK_OFF_FACTOR_I:
      /* Default value: MS-Based Aiding Data Throttle Back Off Factor = 4 */
      ((nv_item_type*)item_ptr)->gps1_msb_back_off_factor = BACK_OFF_FACTOR; //uint8
      break;

    case NV_GPS1_MSB_BACK_OFF_MIN_I:
      /* Default value: MS-Based Aiding Data Throttle Back Off Minimum Duration = 60 */
      ((nv_item_type*)item_ptr)->gps1_msb_back_off_min = BACK_OFF_MIN_DURATION; //uint32
      break;

    case NV_GPS1_MSB_BACK_OFF_MAX_I:
      /* Default value: MS-Based Aiding Data Throttle Back Off Maximum Duration = 900 */
      ((nv_item_type*)item_ptr)->gps1_msb_back_off_max = BACK_OFF_MAX_DURATION; //uint32
      break;

    case NV_GPS1_MSB_BACK_OFF_RESET_I:
      /* Default value: MS-Based Aiding Data Throttle Back Off Duration Reset = 900 */
      ((nv_item_type*)item_ptr)->gps1_msb_back_off_reset = BACK_OFF_RESET_DURATION; //uint32
      break;

    case NV_GPS1_CAPABILITIES_I:
      /* Default value: Enable Sending System Param Info Msg (Capabilities) = TRUE */
      ((nv_item_type *)item_ptr)->gps1_capabilities = TRUE; //byte
      break;

    /***************************************************/
    /* Protocol Submodule: V1/V2 (Vx) Related NV Items */
    /***************************************************/

    case NV_GPS1_VX_LCS_AGENT_I:
      /* Default value: Item to disable or enable Vx LCS Agent functionality = 0 */
      ((nv_item_type*)item_ptr)->gps1_vx_lcs_agent = (uint8)PDSM_LCS_AGENT_DISABLED; //uint8
      break;

    case NV_GPS1_VX_APP_TRUSTED_SETTINGS_I:
      /* Default value: Bitmap indicating whether applications are Trusted or Non-Trusted = 0 */
      ((nv_item_type*)item_ptr)->gps1_vx_app_trusted_settings = LCS_AGENT_DEFAULT_ALL_DISABLED; //uint8
      break;

    case NV_GPS1_VX_NI_TELESERVICE_ID_I:
      /* Default value: GPSOne Vx dedicated SMS Teleservice Identifier = 65001 */
      ((nv_item_type*)item_ptr)->gps1_vx_ni_teleservice_id = LCS_AGENT_DEFAULT_TELESERVICE_ID; //uint16
      break;

    case NV_GPS1_VX_LCS_AGENT_PREV6_ONLY_I:
      /* Default value: Flag to decide whether to force Vx over PRev 6 and above = FALSE */
      ((nv_item_type*)item_ptr)->gps1_vx_lcs_agent_prev6_only = FALSE; //boolean
      break;

    case NV_GPS1_VX_MO_MAX_DURATION_I:
      /* Default value: Max SPPReq duration for MO Non-Trusted = 0xFFFFFFFF */
      ((nv_item_type*)item_ptr)->gps1_vx_mo_max_duration = LCS_AGENT_DEFAULT_MAX_DURATION; //uint32
      break;

    case NV_GPS1_VX_GPS_DURING_VOICE_CALL_I:
      /* Default value: Item to disable position location when voice call is active = 0 */
      ((nv_item_type*)item_ptr)->gps1_vx_gps_during_voice_call = PDSM_LCS_AGENT_DISABLE_GPS_DURING_VOICE_CALL; //uint8
      break;

    /**********************************************/
    /* Protocol Submodule: IS801 Related NV Items */
    /**********************************************/

    case NV_GPS1_GPS_RF_DELAY_I:
      /* Default value: GPS RF Signal Delay = 0 */
      ((nv_item_type*)item_ptr)->gps1_gps_rf_delay = 0; //int16
      break;

    case NV_BC0_GPS1_RF_DELAY_I:
      /* Default value: GPS Chain Delay cal. parameter = 0 */
      ((nv_item_type*)item_ptr)->bc0_gps1_rf_delay = 0; //word
      break;

    case NV_BC1_GPS1_RF_DELAY_I:
      /* Default value: GPS Chain Delay cal. parameter = 0 */
      ((nv_item_type*)item_ptr)->bc1_gps1_rf_delay = 0; //word
      break;

    case NV_BC3_GPS1_RF_DELAY_I:
      /* Default value: GPS Chain Delay cal. parameter = 0 */
      ((nv_item_type*)item_ptr)->bc3_gps1_rf_delay = 0; //word
      break;

    case NV_BC4_GPS1_RF_DELAY_I:
      /* Default value: GPS Chain Delay cal. parameter = 0 */
      ((nv_item_type*)item_ptr)->bc4_gps1_rf_delay = 0; //word
      break;

    case NV_BC5_GPS1_RF_DELAY_I:
      /* Default value: GPS Chain Delay cal. parameter = 0 */
      ((nv_item_type*)item_ptr)->bc5_gps1_rf_delay = 0; //word
      break;

    case NV_BC6_GPS1_RF_DELAY_I:
      /* Default value: GPS Chain Delay cal. parameter = 0 */
      ((nv_item_type*)item_ptr)->bc6_gps1_rf_delay = 0; //word
      break;

    case NV_GPS1_CDMA_RF_DELAY_I:
      /* Default value: CDMA RF Signal Delay = 0 */
      ((nv_item_type*)item_ptr)->gps1_cdma_rf_delay = 0; //int16
      break;

    case NV_GPS1_PCS_RF_DELAY_I:
      /* Default value: The time taken for a PCS signal to pass through the RF chain = 0 */
      ((nv_item_type*)item_ptr)->gps1_pcs_rf_delay = 0; //int16
      break;

    /*********************************************************/
    /* Protocol Submodule: Call Manager (CM) Related NV Item */
    /*********************************************************/

    case NV_GPS1_CALL_RELATED_I:
      /* Default value: FEATURE_GPSONE_CALL_RELATED functionality = TRUE */
      ((nv_item_type*)item_ptr)->gps1_call_related = TRUE; //byte
      break;


    /*************************************************************************/
    /* Protocol Submodule: UMTS Related NV Item */
    /*************************************************************************/

    case NV_CGPS_MO_METHOD_I:
      /* Default value: MO Method = 1: MO_UP */
      ((nv_item_type*)item_ptr)->cgps_mo_method = MO_UP; //uint8
      break;

    case NV_AAGPS_EMERGENCY_SERVICES_SPPRT_I:
      /* Default value: Emergency Services Support = 1 */
      ((nv_item_type*)item_ptr)->aagps_emergency_services_spprt = NV_AAGPS_EMERGENCY_SERVICES_SPPRT_V; //byte
      break;

    case NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_I:
      /* Default value: */
      ((nv_item_type*)item_ptr)->aagps_development_test_control5 = NV_AAGPS_DEVELOPMENT_TEST_CONTROL5_V;
      break;
      
    case NV_GNSS_SUPL_VERSION_I:
#ifdef FEATURE_CGPS_FW_NON_STANDARD_SUPPORT    
      /* Default value: 0, SUPL disabled */
      ((nv_item_type*)item_ptr)->gnss_supl_version = 0; 
#else      
      /* Default value: SUPL2 */
      ((nv_item_type*)item_ptr)->gnss_supl_version = NV_GNSS_SUPL_VERSION_V; 
#endif
      break;
      
    case NV_GNSS_RRLP8_RRC8_SUPPORTED_I:
      /* Default value: RRLP-8 (GANSS portion) not supported - OBSOLETE. Instead use GNSS_NV_EFS_SM_TM_ASSISTED_GLO_PROTOCOL_SELECT. */ 
      ((nv_item_type*)item_ptr)->gnss_rrlp8_rrc8_supported = NV_GNSS_RRLP8_RRC8_SUPPORTED_V;
      break;
      
    /*************************************************************************/
    /* Protocol Submodule: UMTS-CP Related NV Items */
    /*************************************************************************/

    case NV_AAGPS_POSITIONING_MODES_SUPPORTED_I:
      /* Default value: Positioning Modes Supported = 0xFFFF */
      ((nv_item_type*)item_ptr)->aagps_positioning_modes_supported = NV_AAGPS_POSITIONING_MODES_SUPPORTED_V; //uint32
      break;

    case NV_AAGPS_MT_LRSUPPORT_I:
      /* Default value: MT LR Support = 1 */
      ((nv_item_type*)item_ptr)->aagps_mt_lrsupport = 1; //byte
      break;

    case NV_AAGPS_DEFAULT_SMLC_COMM_TIMEOUT_I:
      /* Default value: Default SMLC Communication Timeout = 20 seconds */
      ((nv_item_type*)item_ptr)->aagps_default_smlc_comm_timeout = NV_AAGPS_DEFAULT_SMLC_COMM_TIMEOUT_V; //uint32
      break;

    case NV_AAGPS_DEFAULT_ALLOW_RRC_I:
      /* Default value: Default Allow RRC = TRUE */
      ((nv_item_type*)item_ptr)->aagps_default_allow_rrc = NV_AAGPS_DEFAULT_ALLOW_RRC_V; //uint32
      break;

    case NV_AAGPS_DEFAULT_MTLR_GUARD_TIMER_I:
      /* Default value: Default MT LR Guard Timer = 10 seconds */
      ((nv_item_type*)item_ptr)->aagps_default_mtlr_guard_timer = NV_AAGPS_DEFAULT_MTLR_GUARD_TIMER_V; //uint32
      break;

    case NV_AAGPS_2G_MO_LRSUPPORT_I:
      /* Default value: 2G MO-LR Support = 7 */
      /* Support LOW and HIGH Accuracy and demand AssistData as requestType */
      ((nv_item_type*)item_ptr)->aagps_2g_mo_lrsupport = NV_AAGPS_2G_MO_LRSUPPORT_V; //byte
      break;

    case NV_AAGPS_3G_MO_LRSUPPORT_I: 
      /* Default value: 3G MO-LR Support = 3 */
      /* Support LOW and HIGH Accuracy and demand LocEstimate as requestType */
      ((nv_item_type*)item_ptr)->aagps_3g_mo_lrsupport = NV_AAGPS_3G_MO_LRSUPPORT_V; //byte
      break;

    case NV_AAGPS_DEFAULT_QOS_TIME_I: 
      ((nv_item_type*)item_ptr)->aagps_default_qos_time = NV_AAGPS_DEFAULT_QOS_TIME_V; //byte
      break;

    case NV_AAGPS_DEFAULT_QOS_UNC_I: 
      ((nv_item_type*)item_ptr)->aagps_default_qos_unc = NV_AAGPS_DEFAULT_QOS_UNC_V; //byte
      break;

    /*************************************************************************************/
    /* Protocol Submodule: UMTS-UP Secure User Plane Location (SUPL) Related NV Item */
    /*************************************************************************************/

    case NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER1_VALUE_I:
      /* Default value: PRESUPL UE Timer 1 = 20 */
      ((nv_item_type*)item_ptr)->aagps_default_presupl_ue_timer1_value = NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER1_VALUE_V; //uint32
      break;

    case NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER2_VALUE_I:
      /* Default value: PRESUPL UE Timer 2 = 20 */
      ((nv_item_type*)item_ptr)->aagps_default_presupl_ue_timer2_value = NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER2_VALUE_V; //uint32
      break;

    case NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER3_VALUE_I:
      /* Default value: PRESUPL UE Timer 3 = 20 */
      ((nv_item_type*)item_ptr)->aagps_default_presupl_ue_timer3_value = NV_AAGPS_DEFAULT_PRESUPL_UE_TIMER3_VALUE_V; //uint32
      break;

    case NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_I:
      /* Default value: UMTS PDE Server Address URL = '\0' */
      ((nv_item_type*)item_ptr)->cgps_umts_pde_server_addr_url[0] = NV_CGPS_UMTS_PDE_SERVER_ADDR_URL_V; //byte
      break;

    case NV_AAGPS_USE_TRANSPORT_SECURITY_I:
      /* Default value: Enable User Plane Secure Transport = 1 */
      ((nv_item_type*)item_ptr)->aagps_use_transport_security = NV_AAGPS_DEFAULT_USE_TRANSPORT_SECURITY_V; //uint8
      break;


    /******************************************************/
    /* Transceiver Resource Manager (TRM) Related NV Item */
    /******************************************************/

    case NV_TRM_CONFIG_I:
      /* Default value:  Limit RF assignments for simulating reduced h/w capability (nv_trm_config_type) */
      /* Limit RF assignments for simulating reduced h/w capability */
      ((nv_item_type*)item_ptr)->trm_config.rf_config = NV_TRM_RF_DUAL_RX; //nv_trm_rf_enum
      /* Bit field for disallowing certain mode combinations */
      ((nv_item_type*)item_ptr)->trm_config.mask = 0; //uint32
      break;

#ifdef FEATURE_TM_SECURITY
    case NV_GPSONE_PASSWORD_I:
      /* Default value: Simple gpsOne PD API password = reset to 0 */
      memset((void *)&((nv_item_type*)item_ptr)->gpsone_password, 
             0, 
             sizeof(nv_gpsone_password_type));
      ((nv_item_type*)item_ptr)->gpsone_password.password_len = 0; //uint8
      break;

    case NV_GPS1_SEC_UPDATE_RATE_I:
      /* Default value: Rate of security updates sent to client = 0 */
      /* Do random challenge on a per fix basis */
      ((nv_item_type*)item_ptr)->gps1_sec_update_rate = 0; //uint8
      break;
#endif /* FEATURE_TM_SECURITY */

    case NV_CGPS_NMEA_CONFIG_INFO_I:
      /* Default value: Do not start NMEA at startup */
      ((nv_item_type*)item_ptr)->cgps_nmea_config_info.nmea_port_type = NV_CGPS_NMEA_DEFAULT_PORT_TYPE ;
      ((nv_item_type*)item_ptr)->cgps_nmea_config_info.nmea_reporting_type = NV_CGPS_NMEA_DEFAULT_REPORTING_TYPE;
      break;
    /*******************************************************/
    /* Extended Receiver Assistance (XTRA) Related NV Item */
    /*******************************************************/     
       
    case NV_GPS1_XTRA_ENABLED_I:                 
      ((nv_item_type*)item_ptr)->gps1_xtra_enabled = GNSS_NV_DEFAULT_XTRA_ENABLED;
      break;    
      
    case NV_GPS1_XTRA_DOWNLOAD_INTERVAL_I:
      ((nv_item_type*)item_ptr)->gps1_xtra_download_interval = NV_AAGPS_DEFAULT_XTRA_DOWNLOAD_INTERVAL;
      break;    
      
    case NV_GPS1_XTRA_NUM_DOWNLOAD_ATTEMPTS_I:   
      ((nv_item_type*)item_ptr)->gps1_xtra_num_download_attempts = NV_AAGPS_DEFAULT_XTRA_NUM_DOWNLOAD_ATTEMPTS;
      break;    
      
    case NV_GPS1_XTRA_TIME_BETWEEN_ATTEMPTS_I:    
      ((nv_item_type*)item_ptr)->gps1_xtra_time_between_attempts = NV_AAGPS_DEFAULT_XTRA_TIME_BETWEEN_ATTEMPTS;
      break;    
      
    case NV_GPS1_XTRA_AUTO_DOWNLOAD_ENABLED_I:    
      ((nv_item_type*)item_ptr)->gps1_xtra_auto_download_enabled = GNSS_NV_DEFAULT_XTRA_AUTO_DL_DISABLED;
      break;    
      
    case NV_GPS1_XTRA_PRIMARY_SERVER_URL_I:       
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_primary_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_PRIMARY_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_primary_server_url));
      break;    
      
    case NV_GPS1_XTRA_SECONDARY_SERVER_URL_I:     
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_secondary_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_SECONDARY_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_secondary_server_url));
      break;    
      
    case NV_GPS1_XTRA_TERTIARY_SERVER_URL_I:      
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_tertiary_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_TERTIARY_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_tertiary_server_url));
      break;    

    /* XTRA time injection NV items */
    case NV_GPS1_XTRA_TIME_INFO_ENABLED_I:
      ((nv_item_type*)item_ptr)->gps1_xtra_time_info_enabled  = NV_AAGPS_DEFAULT_XTRA_TIME_INFO_ENABLED;
      break;
  
    case NV_GPS1_XTRA_TIME_INFO_UNC_THRESH_I:
      ((nv_item_type*)item_ptr)->gps1_xtra_time_info_unc_thresh  = NV_AAGPS_DEFAULT_XTRA_TIME_INFO_UNC_THRESH;
      break;
  
    case NV_GPS1_XTRA_TIME_INFO_DELAY_THRESH_I:      
      ((nv_item_type*)item_ptr)->gps1_xtra_time_info_delay_thresh = NV_AAGPS_DEFAULT_XTRA_TIME_INFO_DELAY_THRESH;
      break;
  
    case NV_GPS1_XTRA_PRIMARY_SNTP_SERVER_URL_I:
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_primary_sntp_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_PRIMARY_SNTP_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_primary_sntp_server_url));
      break;    
  
    case NV_GPS1_XTRA_SECONDARY_SNTP_SERVER_URL_I:
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_secondary_sntp_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_SECONDARY_SNTP_SERVER_URL, 
                  sizeof(((nv_item_type *)item_ptr)->gps1_xtra_secondary_sntp_server_url));
      break;    
  
    case NV_GPS1_XTRA_TERTIARY_SNTP_SERVER_URL_I:
      (void) GNSS_STRLCPY((char *) &(((nv_item_type *)item_ptr)->gps1_xtra_tertiary_sntp_server_url[0]), 
                  (const char *) NV_AAGPS_DEFAULT_XTRA_TERTIARY_SNTP_SERVER_URL, 
                  sizeof((nv_item_type *)item_ptr)->gps1_xtra_tertiary_sntp_server_url);
      break;

    case NV_GPS1_DYNAMIC_MODE_I:
      /* default is to enable data demod */
      ((nv_item_type*)item_ptr)->gps1_dynamic_mode = 1;
      break;
      
    case NV_CGPS_1X_PDE_SERVER_ADDR_URL_I:
      /* Default value: Default URL = '\0' */
      ((nv_item_type*)item_ptr)->cgps_1x_pde_server_addr_url[0] = '\0';
      break;

    case NV_CGPS_ON_DEMAND_ENABLED_I:
      /* default value should be enabled. for now disable it */
      ((nv_item_type*)item_ptr)->cgps_on_demand_enabled = FALSE;
      break;
    case NV_CGPS_UTC_GPS_TIME_OFFSET_I:
      ((nv_item_type*)item_ptr)->cgps_utc_gps_time_offset = GPS_UTC_OFFSET;
      break;
     /* QWiP related NV items*/
    case NV_CGPS_QWIP_LOC_ENGINE_CONFIG_I:
       /* Default value should be Sysd Automatic. */
       ((nv_item_type*)item_ptr)->cgps_qwip_loc_engine_config = 0;
       break;
    case NV_CGPS_XTRA_T_CONTROL_I:
      /* default should be disabled. */
      ((nv_item_type*)item_ptr)->cgps_xtra_t_control = FALSE;
      break;

    case NV_GNSS_XSPI_INJECTION_TIMEOUT_I:
       ((nv_item_type*)item_ptr)->gnss_xspi_injection_timeout = SM_NV_XSPI_DEFAULT_INJECTION_TIMEOUT;
       break;

    default:
       break;
  }
}


/*===========================================================================

FUNCTION sm_nv_read_from_cache_sub

DESCRIPTION
  This function is used to perform a read from cached value for a SUB

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_cache_sub(nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
#ifdef FEATURE_GNSS_NO_NV_WRITES
  #error code not present
#endif

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"SM_NV_READ_from_cache_sub: FAILED - NULL parameter");
    return FALSE;
  }

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "M_NV_READ: FAILED - Invalid sub %d (Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

#ifdef FEATURE_GNSS_NO_NV_WRITES
    #error code not present
#else
    return FALSE;
#endif /*FEATURE_GNSS_NO_NV_WRITES*/

  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_read_from_cache

DESCRIPTION
  This function is used to perform a read from cached value from current DD sub

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_cache(nv_items_enum_type nv_item, void* item_ptr)
{
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_Cache - NULL parameter");
    return FALSE;
  }

  return sm_nv_read_from_cache_sub(nv_item, item_ptr,
                                   sm_nv_map_sub(nv_item, sm_nv_current_sub) );
}

/*===========================================================================

FUNCTION sm_nv_read_from_hw

DESCRIPTION
  This function is used to perform a synchronous read from NV (sub-independent)

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_hw(nv_items_enum_type nv_item, void* item_ptr)
{
  nv_cmd_type* cmd_ptr = NULL;
  rex_sigs_type sigs = 0;

  /* Passed argument sanity check */
  if (NULL == item_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW - NULL parameter");
    return FALSE;
  }

  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  cmd_ptr = (nv_cmd_type*)pdsm_getbuf(sizeof(nv_cmd_type));

  /* Dynamic buffer allocation sanity check */
  if (NULL == cmd_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ failed: no buffer allocated");
    sm_nv_read_default(nv_item, item_ptr);
    return FALSE;
  }

  /* Populate NV read command */
  cmd_ptr->item       = nv_item;
  cmd_ptr->cmd        = NV_READ_F;
  cmd_ptr->data_ptr   = (nv_item_type*)item_ptr;
  cmd_ptr->tcb_ptr    = os_Self();
  cmd_ptr->sigs       = (uint32) C_SM_NV_READ_SIGNAL;
  cmd_ptr->done_q_ptr = NULL;
  
  /* Clear NV Read signal before waiting on it */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );
  
  /* Send NV read command to NV module */
  nv_cmd(cmd_ptr);
    
  /* Block until NV Read signal is set
     When in block state, respont to dog heartbeat*/  
  while ((sigs & C_SM_NV_READ_SIGNAL)==0)
  {
     sigs =  rex_wait( C_SM_NV_READ_SIGNAL | C_OS_FLAG_DOG_HB );
     /*If the SIG Set when Wait returns is not SM_NV_READ. 
       It must be DOG Heart beat, Send Heartbeat report.
       to ensure DOG timeout does not occur*/
     if ((sigs & C_SM_NV_READ_SIGNAL)==0)
     {
        os_DogHeartbeatReport();
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW: DOG PAT");
     }
  }
  /* Clear NV Read signal after it is read out */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );

    /* Check status of NV Read */
  if (cmd_ptr->status != NV_DONE_S)
  {
    /* NV read failed. Return default value for NV item. */
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - Unknown Status");
    sm_nv_read_default(nv_item, item_ptr);

    (void)pdsm_freebuf((char*)cmd_ptr);
    return FALSE;
  }

  (void)pdsm_freebuf((char*)cmd_ptr);
  return TRUE;

}

/*===========================================================================

FUNCTION sm_nv_read_from_hw_sub

DESCRIPTION
  This function is used to perform a synchronous read from NV (for a
  specific subscription)

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
static boolean sm_nv_read_from_hw_sub(nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
  nv_cmd_type* cmd_ptr = NULL;
  /* Extended NV command struct for dual sim */
  nv_cmd_ext_type nvcmd_ext;

  rex_sigs_type sigs = 0;

  /* Passed argument sanity check */
  if (NULL == item_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW_Sub failed: NULL parameter");
    return FALSE;
  }

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d (Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  cmd_ptr = (nv_cmd_type*)pdsm_getbuf(sizeof(nv_cmd_type));

  /* Dynamic buffer allocation sanity check */
  if (NULL == cmd_ptr)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ failed: no buffer allocated");
    sm_nv_read_default(nv_item, item_ptr);
    return FALSE;
  }

  /* Populate NV read command */
  cmd_ptr->item       = nv_item;
  cmd_ptr->cmd        = NV_READ_F;
  cmd_ptr->data_ptr   = (nv_item_type*)item_ptr;
  cmd_ptr->tcb_ptr    = os_Self();
  cmd_ptr->sigs       = (uint32) C_SM_NV_READ_SIGNAL;
  cmd_ptr->done_q_ptr = NULL;
  
  nvcmd_ext.nvcmd   = cmd_ptr;
  nvcmd_ext.context = (uint16)sub;

  /* Clear NV Read signal before waiting on it */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );
  
  /* Send NV read command to NV module */
  nv_cmd_ext(&nvcmd_ext);
    
  /* Block until NV Read signal is set
     When in block state, respont to dog heartbeat*/  
  while ((sigs & C_SM_NV_READ_SIGNAL)==0)
  {
     sigs =  rex_wait( C_SM_NV_READ_SIGNAL | C_OS_FLAG_DOG_HB );
     /*If the SIG Set when Wait returns is not SM_NV_READ. 
       It must be DOG Heart beat, Send Heartbeat report.
       to ensure DOG timeout does not occur*/
     if ((sigs & C_SM_NV_READ_SIGNAL)==0)
     {
        os_DogHeartbeatReport();
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_HW: DOG PAT");
     }
  }
  /* Clear NV Read signal after it is read out */
  (void) os_ClrLocalSignal( C_SM_NV_READ_SIGNAL );

    /* Check status of NV Read */
  if (cmd_ptr->status != NV_DONE_S)
  {
    /* NV read failed. Return default value for NV item. */
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - Unknown Status");
    sm_nv_read_default(nv_item, item_ptr);

    (void)pdsm_freebuf((char*)cmd_ptr);
    return FALSE;
  }

  (void)pdsm_freebuf((char*)cmd_ptr);
  return TRUE;

}


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
boolean sm_nv_read (nv_items_enum_type nv_item, void* item_ptr)
{
  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  boolean ret_val = FALSE;
  sys_modem_as_id_e_type sub;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - NULL parameter");
    return FALSE;
  }

  sub = sm_nv_map_sub(nv_item, sm_nv_current_sub);
  if (sub == SM_CONFIG_MIN_SUB_ID)
  {
    /* sub-indepdent NV or Sub1 NV */
    ret_val = sm_nv_read_from_hw(nv_item, item_ptr);
  }
  else
  {
    /* sub-dedepdent NV */
    ret_val = sm_nv_read_from_hw_sub(nv_item, item_ptr, sub);
  }

  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Read: %d (sub=%d) ret=%d", nv_item, sub, ret_val);

  (void)sm_nv_write_to_cache_sub(nv_item, item_ptr, sub);

  return ret_val;
}

/*===========================================================================

FUNCTION sm_nv_read_ex

DESCRIPTION
  This function is used to perform a read from cache. And if cached value not present, read from Hw.
  GPS SHALL CALL THIS API INSTEAD OF sm_nv_read AFTER BOOT UP INITIALIZATION

RETURN VALUE
  TRUE  - NV read success
  FALSE - NV read failure (read default value instead)

DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_read_ex (nv_items_enum_type nv_item, void* item_ptr)
{
  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space is memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  boolean ret_val = FALSE;
  sys_modem_as_id_e_type sub;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - NULL parameter");
    return FALSE;
  }

  sub = sm_nv_map_sub(nv_item, sm_nv_current_sub);
  if(sm_nv_read_from_cache_sub(nv_item,item_ptr, sub))
  {
    /* NV HW read not needed */
    ret_val = TRUE;
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Cache Read: %d (sub=%d)", nv_item, sub);
  }
  else
  {
    if (sub == SM_CONFIG_MIN_SUB_ID)
    {
      /* sub-indepdent NV */
      ret_val = sm_nv_read_from_hw(nv_item, item_ptr);
    }
    else
    {
      /* sub-dedepdent NV */
      ret_val = sm_nv_read_from_hw_sub(nv_item, item_ptr, sub);
    }
      
    MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Read: %d (sub=%d) ret=%d", nv_item, sub, ret_val);
  }

  return ret_val;
}


/*===========================================================================

FUNCTION   sm_nv_rdrw_done

DESCRIPTION
  This function is used to handle NV HW read/write callback status. If there are any
  clients who issued read/write requests, they will be notified and any pending
  read/write requests will be subsequently handled.

RETURN VALUE
  TRUE  - NV write success
  FALSE - NV write failure

DEPENDENCIES
 
===========================================================================*/

boolean sm_nv_rdrw_done (void)
{
  /* First dequeue the async-read/write request at the head */
  sm_nv_item_s_type* nv_ptr = ((sm_nv_item_s_type*)q_get(&sm_nv_cmd_q));

  if (nv_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: No write requested");
    (void)pdsm_freebuf((char*)nv_ptr);
    return FALSE;
  }
  else
  {
    /* Make sure first dequeued element is same as current NV async-read/write element */
    if (sm_nv_cmd.item != nv_ptr->item)
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - Non-matching item");
      (void)pdsm_freebuf((char*)nv_ptr);
      return FALSE;
    }
    else
    {
      /* Check NV command result status */
      if (sm_nv_cmd.status != NV_DONE_S)
      {
        /* NV read/write failed. */
        MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - Error Status %d : %d ", sm_nv_cmd.status, sm_nv_cmd.item);
        (void)pdsm_freebuf((char*)nv_ptr);
        /*Handle next read/write command*/
        if (q_cnt(&sm_nv_cmd_q) != 0)
        {
          /* If NV Read/Write command queue has items in queue item, handle it*/
           sm_nv_handle_rdwr_cmd();
        }
        return FALSE;
      }

      /* Out-of-bound array index sanity check */
      if ((nv_ptr->handle >= SM_NV_CLIENT_HANDLE_MAX) ||
          (&nv_ptr->data == NULL))
      {
        MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - bad parameters");
        (void)pdsm_freebuf((char*)nv_ptr);
        return FALSE;
      }
      else
      {
         /* Call appropriate client's registered callback function.
            async-read/write function callback's are differentiated
            based on "nv_ptr->handle", currently in tm_core
            async-read = tm_nv_async_read_handle
            write      = tm_nv_write_handle*/
         if (nv_ptr->cmd == NV_WRITE_F) 
         {
            sm_nv_clnt_tbl[nv_ptr->handle].nv_write_cb_fn(nv_ptr->item, (void*)&nv_ptr->data, sm_nv_cmd.status);
         }
         else if (nv_ptr->cmd == NV_READ_F) 
         {
            /*Update cache Async NV read successful*/
            (void)sm_nv_write_to_cache(nv_ptr->item, &(nv_ptr->data));
            sm_nv_clnt_tbl[nv_ptr->handle].nv_read_cb_fn(nv_ptr->item, (void*)&nv_ptr->data, sm_nv_cmd.status);
         }
         else
         {
            MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_RDRW: FAILED - CMD not supported: %d",(int)nv_ptr->cmd);
         }
         /* Check if clients were waiting for NV write success */
         //pa_info_available(sm_nv_cmd.item);

         /* Handle next NV async-read/write request */
         sm_nv_handle_rdwr_cmd();
      }
    }
    (void)pdsm_freebuf((char*)nv_ptr);
  }
  return TRUE;
}

/*===========================================================================

FUNCTION sm_nv_read_async

DESCRIPTION
  This function is used to queue NV Async Read commands to NV. The READ's to NV
  are performed in asynchronous fashion.

RETURN VALUE
  TRUE  - NV read command successfully sent to NV
  FALSE - NV read command failed to be sent to NV

DEPENDENCIES
  SM NV read callback needs to be registered via call to sm_nv_read_async_reg

===========================================================================*/

boolean sm_nv_async_read (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle)
{
  sm_nv_item_s_type* nv_ptr = NULL;

  /* Passed argument sanity check */
  if ((NULL == item_ptr) || (handle >= SM_NV_CLIENT_HANDLE_MAX))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ_ASYNC failed: bad parameters");
    return FALSE;
  }

  if (sm_nv_initialized)
  {
    /* Since NV read and writes should normally be done in GPS idle state
     * (no sessions), no separate allocation of space is memory is done.
     * The buffers will be used from a dynamic buffer pool.
     */
    nv_ptr = (sm_nv_item_s_type*)pdsm_getbuf(sizeof(sm_nv_item_s_type));

    /* Dynamic buffer allocation sanity check */
    if (NULL == nv_ptr)
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE failed: no buffer allocated");
      return FALSE;
    }

    /* Populate NV write command queue element */
    nv_ptr->item   = nv_item;
    nv_ptr->data   = *item_ptr;
    nv_ptr->handle = handle;
    nv_ptr->cmd    = NV_READ_F;

    /* Queue a request for read to NV. Put the command into the
     * NV command queue and services them one at a time.
     */
    q_put(&sm_nv_cmd_q, &nv_ptr->link);

    if (q_cnt(&sm_nv_cmd_q) == 1)
    {
      /* If NV command queue has only one item, handle it immediately */
      sm_nv_handle_rdwr_cmd();
    }
    return TRUE;
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE: SM NV not initialized");
    return FALSE;
  }
}


/*===========================================================================

FUNCTION sm_nv_set_current_sub_id

DESCRIPTION
  This function is used to set the current default subscription ID (usually
  Dedicated-Data sub). When NV read/write is performed without giving
  specific Subs ID, this current Subs ID will be used.
 
  Note: When DD sub is out of service, NV read/write will continue on last Subs
  ID used (if no Subs ID is specified)
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_set_current_sub_id(sys_modem_as_id_e_type sub)
{
  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d (Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

  sm_nv_current_sub = sub;
  MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_MED, "NV SubsID=%d (Max=%d)", sub, sm_nv_max_sub);

  return TRUE;
}


/*===========================================================================

FUNCTION sm_nv_set_max_sub_id

DESCRIPTION
  This function is used to set the max subscription ID (0 or 1, or 2)
  
RETURN VALUE
  Boolean: TRUE for success, FALSE for failure
  
DEPENDENCIES
 
===========================================================================*/
boolean sm_nv_set_max_sub_id(sys_modem_as_id_e_type sub)
{
  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > SM_CONFIG_MAX_SUB_ID))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Max Subs ID %d (Max=%d)", sub, SM_CONFIG_MAX_SUB_ID);
    return FALSE;
  }

  sm_nv_max_sub = sub;
  MSG_1( MSG_SSID_GPSSM, MSG_LEGACY_MED, "NV Max SubsID=%d", sub);

  return TRUE;
}

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

boolean sm_nv_read_from_sub (nv_items_enum_type nv_item, void* item_ptr, sys_modem_as_id_e_type sub)
{
  /* Since NV read and writes should normally be done in GPS idle state
   * (no sessions), no separate allocation of space in memory is done.
   * The buffers will be used from a dynamic buffer pool.
   */
  boolean ret_val = FALSE;

  /* Passed argument sanity check */
  if (item_ptr == NULL)
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_READ: FAILED - NULL parameter");
    return FALSE;
  }

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d (Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

  ret_val = sm_nv_read_from_hw_sub(nv_item, item_ptr, sub);

  MSG_3( MSG_SSID_GPSSM, MSG_LEGACY_MED, "SM NV Hw Read: %d (sub=%d) ret=%d", nv_item, sub, ret_val);

  (void)sm_nv_write_to_cache_sub(nv_item, item_ptr, sub);

  return ret_val;
}



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

boolean sm_nv_write_to_sub (nv_items_enum_type nv_item, const nv_item_type* item_ptr, uint32 handle, sys_modem_as_id_e_type sub)
{
  sm_nv_item_s_type* nv_ptr = NULL;

  /* Passed argument sanity check */
  if ((NULL == item_ptr) || (handle >= SM_NV_CLIENT_HANDLE_MAX))
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE failed: bad parameters");
    return FALSE;
  }

  if ((sub < SM_CONFIG_MIN_SUB_ID) || (sub > sm_nv_max_sub))
  {
    MSG_2( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "Invalid Subs ID %d(Max=%d)", sub, sm_nv_max_sub);
    return FALSE;
  }

  if (sm_nv_initialized)
  {
    /* Since NV read and writes should normally be done in GPS idle state
     * (no sessions), no separate allocation of space is memory is done.
     * The buffers will be used from a dynamic buffer pool.
     */
    nv_ptr = (sm_nv_item_s_type*)pdsm_getbuf(sizeof(sm_nv_item_s_type));

    /* Dynamic buffer allocation sanity check */
    if (NULL == nv_ptr)
    {
      MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE failed: no buffer allocated");
      return FALSE;
    }

    /* Populate NV write command queue element */
    nv_ptr->item   = nv_item;
    nv_ptr->data   = *item_ptr;
    nv_ptr->handle = handle;
    nv_ptr->sub    = sub;
    nv_ptr->cmd    = NV_WRITE_F;

    /* Queue a request for writing to NV. Put the command into the
     * NV Write command queue and services them one at a time.
     */
    q_put(&sm_nv_cmd_q, &nv_ptr->link);

    if (q_cnt(&sm_nv_cmd_q) == 1)
    {
      /* If NV Write command queue has only one item, handle it immediately */
      sm_nv_handle_rdwr_cmd();
    }
    return TRUE;
  }
  else
  {
    MSG( MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "SM_NV_WRITE: SM NV not initialized");
    return FALSE;
  }
}


