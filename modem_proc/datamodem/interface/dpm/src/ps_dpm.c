/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ D P M . C

GENERAL DESCRIPTION
  This is the implementation of the Data Path Manager module

Copyright (c) 2014-15 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/18/15   skc     Fixed KW issues
06/04/15    gk     Removed un-necessary F3s from data path
01/13/15    gk     Added featurisation to make sure associated flow is retrived
                   only when EPC featurisation is enabled
04/09/14    ash    Added Prefix Delegation support.
11/01/13    rk     Removed feature mobileap.
09/10/13    bvd    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "ps_dpmi.h"
#include "ps_dpm_utils.h"
#include "sio.h"
#include "ps_logging.h"
#include "ps_in.h"
#include "ps_lan_llc.h"
#include "ps_arp.h"
#include "ps_dpm_opt_defs.h"
#include "ps_rm_defs.h"
#include "ps_dpm_ul_opt.h"
#include "ps_system_heap.h"
#include "ps_dpm_ul_legacy.h"
#include "ps_iface_defs.h"
#include "ps_iface_ipfltr.h"
#include "ps_rm_svc.h"
#include "ps_handle_mgr.h"
#include "dcc_task_svc.h"
#include "ps_dpm_mbim.h"
#include "rex.h"
#include "dstaski.h"
#include "dstask_v.h"
#include "ds_qmi_svc.h"
#include "ds_qmi_qos.h"
#include "ps_dpm_hw.h"
#include "ps_iface_ipfltr_hw.h"
#include "ps_pkt_info_utils.h"
#include "ps_sys_fast_dormancy.h"
#include "ps_stat_iface.h"
#include "ps_logging_defs.h"
#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_dpm_clati.h"
#endif /* FEATURE_DATA_PS_464XLAT */

/*===========================================================================

                           DEFINES

===========================================================================*/
#define   PS_DPM_HW_PS_WM_DNE   305000
#define   PS_DPM_HW_PS_WM_HI    225000
#define   PS_DPM_HW_PS_WM_LO    120000
/*Discard  received DSM item when DNE packet count hit*/
#define   PS_DPM_HW_PS_WM_DNE_PKT_COUNT 1000

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================

                           GLOBAL DATA DECLARATIONS

===========================================================================*/
ps_dpm_global_info_type   ps_dpm_global_info;
ps_crit_sect_type         ps_dpm_crit_section;

extern rex_tcb_type     * rex_dcc_tcb;

/*----------------------------------------------------------------------------
  This Holds call back function and threshold count passed by mode handlers.
  Mode handlers call bak would be invoked when continuos invalid packet 
  count reaches threshold count  or when DPM recives first valid packet 
  after threshold number of invalid packets.
----------------------------------------------------------------------------*/
ps_dpm_cont_err_pkt_cb_info ps_dpm_cont_err_pkt_info;

/*===========================================================================

                           STATIC DATA DECLARATIONS

===========================================================================*/
static uint8 ps_dpm_last_uid_generated;

/*---------------------------------------------------------------------------
  Holds the status of DPM RM dereg operation. It would be accessed in
  calling task of ps_dpm_rm_reg_iface to send the status of dpm rm dereg
  operation
---------------------------------------------------------------------------*/
static int ps_dpm_status_rm_reg_iface = -1;

/*-------------------------------------------------------------------------
  This variable is set through NV 67343, Sys IOCTL.  If it is set, DL data path 
  would be taken through software path and if its set to 0, DL data path would go 
  through hardware. Default value is set to 0.
-------------------------------------------------------------------------*/
static ps_dpm_disable_hw_path_enum_type  disable_hw_bridge_mode =  PS_DPM_DISABLE_HW_PATH_NONE;

/*-------------------------------------------------------------------------
   Stores cb function ptr given by modehandler for sending DPM
   reg/dereg/rereg complete notification
-------------------------------------------------------------------------*/
static ps_dpm_um_ev_cback_fptr_type  ps_dpm_global_um_ev_cback[PS_SYS_TECH_MAX];

/*---------------------------------------------------------------------------
  Table holds in use flags for all DPM uplink signals
---------------------------------------------------------------------------*/
static boolean   ps_dpm_ul_sig_in_use_tbl[PS_DPM_UL_MAX_SIGNALS];

/*---------------------------------------------------------------------------
  Table holds in use flags for all DPM uplink signals
---------------------------------------------------------------------------*/
static boolean   ps_dpm_dl_sig_in_use_tbl[PS_DPM_DL_MAX_SIGNALS];

static boolean ps_dpm_uids_inuse[PS_DPM_MAX_UID];

/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
static ps_dpm_um_info_type * ps_dpmi_alloc_dpm_um_info
(
  void
)
{
  ps_dpm_um_info_type  * dpm_um_info_ptr;
  int16                  dpm_um_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Find an open slot. Dynamically allocate memory, initialize it, update
      the open slot and return it
    -----------------------------------------------------------------------*/

    for (dpm_um_index = 0;
         dpm_um_index < PS_DPM_MAX_PDN_SESSIONS;
         dpm_um_index++)
    {
      if (NULL == ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index])
      {
        break;
      }
    }

    if (PS_DPM_MAX_PDN_SESSIONS <= dpm_um_index)
    {
      LOG_MSG_ERROR_0("ps_dpmi_alloc_dpm_um_info(): "
                      "No free space in DPM Um tbl");
      break;
    }

    PS_SYSTEM_HEAP_MEM_ALLOC(dpm_um_info_ptr, sizeof(ps_dpm_um_info_type),
                             ps_dpm_um_info_type*);
    if (NULL ==  dpm_um_info_ptr)
    {
      break;
    }

    memset(dpm_um_info_ptr, 0, sizeof(ps_dpm_um_info_type));

    dpm_um_info_ptr->handle =
      ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_DPM_UM, dpm_um_index);

    dpm_um_info_ptr->is_bridged_with_hw = FALSE;

    ps_dpm_global_info.num_dpm_um_info_ptr++;
    ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index] = dpm_um_info_ptr;

    dpm_um_info_ptr->all_bearers_flushed = 0xFF;

    LOG_MSG_INFO1_2("ps_dpmi_alloc_dpm_um_info(): "
                    "Allocated DPM Um entry 0x%p with handle 0x%x",
                    dpm_um_info_ptr, dpm_um_info_ptr->handle);

    return dpm_um_info_ptr;
  } while (0);

  return NULL;
} /* ps_dpmi_alloc_dpm_um_info() */


ps_dpm_um_info_type * ps_dpmi_get_dpm_um_info
(
  ps_iface_type  * um_iface_ptr
)
{
  ps_dpm_um_info_type  * dpm_um_info_ptr;
  int32                  num_dpm_um_info_ptr = 0;
  int16                  dpm_um_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if any entry exists for the input Um iface
  -------------------------------------------------------------------------*/
  for (dpm_um_index = 0;
       dpm_um_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_um_info_ptr < ps_dpm_global_info.num_dpm_um_info_ptr;
       dpm_um_index++)
  {
    if (NULL != ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index])
    {
      num_dpm_um_info_ptr++;

      dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];

      if (dpm_um_info_ptr->v4_iface_ptr == um_iface_ptr ||
          dpm_um_info_ptr->v6_iface_ptr == um_iface_ptr)
      {
        LOG_MSG_INFO1_2("ps_dpmi_get_dpm_um_info(): "
                        "Found DPM Um entry 0x%p at index %d",
                        dpm_um_info_ptr, dpm_um_index);

        return dpm_um_info_ptr;
      }
    }
  }

  LOG_MSG_ERROR_0("ps_dpmi_get_dpm_um_info(): Couldn't find DPM Um info");

  return NULL;
} /* ps_dpmi_get_dpm_um_info() */


ps_dpm_um_info_type * ps_dpmi_get_dpm_um_info_by_handle
(
  int32  handle
)
{
  ps_dpm_um_info_type   * dpm_um_info_ptr = NULL;
  int16                   dpm_um_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_um_index = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_UM, handle);

  if (dpm_um_index >= PS_DPM_MAX_PDN_SESSIONS || dpm_um_index < 0)
  {
    LOG_MSG_ERROR_1("ps_dpmi_get_dpm_um_info_by_handle(): "
                    "Invalid DPM um index 0x%x", dpm_um_index);
    return NULL;  
  }
  
  dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];
  if (NULL == dpm_um_info_ptr || handle != dpm_um_info_ptr->handle)
  {
    LOG_MSG_ERROR_1("ps_dpmi_get_dpm_um_info_by_handle(): "
                    "Invalid DPM Um handle 0x%x", handle);
    return NULL;
  }

  return dpm_um_info_ptr;
} /* ps_dpmi_get_dpm_um_info_by_handle() */


static void ps_dpmi_free_dpm_um_info
(
  ps_dpm_um_info_type  * dpm_um_info_ptr
)
{
  int16  dpm_um_index;
#ifdef FEATURE_DATA_PS_464XLAT
  ps_dpm_clat_pdn_cntxt_info_type       * clat_pdn_cntxt = NULL;  
#endif /* FEATURE_DATA_PS_464XLAT */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(0 == dpm_um_info_ptr->num_bearer);

  LOG_MSG_INFO1_2("ps_dpmi_free_dpm_um_info(): "
                  "Freeing DPM Um entry 0x%p with handle 0x%x",
                  dpm_um_info_ptr, dpm_um_info_ptr->handle);
#ifndef TEST_FRAMEWORK
#ifdef FEATURE_DATA_PS_464XLAT 
  /*-----------------------------------------------------------------------
    For CLAT case, only v6 iface would be up and used. 
    1. Deregister CLAT PDN context with hardware 
    2. Delete v6 destination filter for CLAT
    3. Free CLAT pdn context
  -----------------------------------------------------------------------*/  
  if (NULL != dpm_um_info_ptr->v6_iface_ptr && 
      NULL != dpm_um_info_ptr->v6_iface_ptr->clat_pdn_cntxt)
  {
    clat_pdn_cntxt = 
      (ps_dpm_clat_pdn_cntxt_info_type *)dpm_um_info_ptr->v6_iface_ptr->clat_pdn_cntxt;
      
    ps_dpm_hw_clat_dereg_pdn_context(clat_pdn_cntxt->hw_pdn_handle);

    if (NULL != clat_pdn_cntxt->hw_v6_addr_handle)
    {
      ps_iface_ipfltr_delete_v6_dst_fltr
      (
        clat_pdn_cntxt->hw_v6_addr_handle
      );  
      
      clat_pdn_cntxt->hw_v6_addr_handle = NULL;
    }
    
    PS_SYSTEM_HEAP_MEM_FREE(dpm_um_info_ptr->v6_iface_ptr->clat_pdn_cntxt);
    dpm_um_info_ptr->v6_iface_ptr->clat_pdn_cntxt = NULL;
  }
#endif /* FEATURE_DATA_PS_464XLAT */
#endif /* TEST_FRAMEWORK */

  dpm_um_index = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_UM,
                                         dpm_um_info_ptr->handle);

  if (dpm_um_index < 0 || dpm_um_index >= PS_DPM_MAX_PDN_SESSIONS)
  {
    LOG_MSG_ERROR_1("ps_dpmi_free_dpm_um_info(): "
                    "Invalid dpm um index %d", dpm_um_index); 
    ASSERT(0);
    return; 
  }

  ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index] = NULL;  
  ps_dpm_global_info.num_dpm_um_info_ptr--;

  PS_SYSTEM_HEAP_MEM_FREE(dpm_um_info_ptr);

  return;
} /* ps_dpmi_free_dpm_um_info() */


static ps_dpm_bearer_info_type * ps_dpmi_alloc_bearer_info
(
  ps_dpm_um_info_type  * dpm_um_info_ptr
)
{
  ps_dpm_bearer_info_type  * bearer_ptr;
  int16                      bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Find an open slot. Dynamically allocate memory, initialize it, update
      the open slot and return it
    -----------------------------------------------------------------------*/

    for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
    {
      if (NULL == dpm_um_info_ptr->bearer_ptr_arr[bearer_index])
      {
        break;
      }
    }

    if (PS_DPM_MAX_BEARERS <= bearer_index)
    {
      LOG_MSG_ERROR_0("ps_dpmi_alloc_bearer_info(): "
                      "No free space in bearer tbl");
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate memory for bearer info
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(bearer_ptr, sizeof(ps_dpm_bearer_info_type),
                             ps_dpm_bearer_info_type*);
    if (NULL == bearer_ptr)
    {
      break;
    }

    memset(bearer_ptr, 0, sizeof(ps_dpm_bearer_info_type));

    /*-----------------------------------------------------------------------
      Set to invalid values so that this information can be used during
      clean up to know if a field is initialized or not
    -----------------------------------------------------------------------*/
    bearer_ptr->uid        = PS_DPM_MAX_UID;
    bearer_ptr->dpm_dl_sig = PS_MAX_SIGNALS;

    bearer_ptr->handle =
      ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_DPM_BEARER, bearer_index);

    dpm_um_info_ptr->num_bearer++;
    dpm_um_info_ptr->bearer_ptr_arr[bearer_index] = bearer_ptr;

    dpm_um_info_ptr->all_bearers_flushed &= ~(1 << bearer_index);

    LOG_MSG_INFO1_2("ps_dpmi_alloc_bearer_info(): "
                    "Allocated bearer entry 0x%p with handle 0x%x",
                    bearer_ptr, bearer_ptr->handle);
    return bearer_ptr;
  } while (0);

  return NULL;
} /* ps_dpmi_alloc_bearer_info() */


static ps_dpm_bearer_info_type * ps_dpmi_get_bearer_info
(
  ps_dpm_um_info_type  * dpm_um_info_ptr,
  ps_phys_link_type    * phys_link_ptr
)
{
  ps_dpm_bearer_info_type  * bearer_ptr;
  int16                      bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
  {
    bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
    if (NULL != bearer_ptr && phys_link_ptr == bearer_ptr->ps_phys_link_ptr)
    {
      LOG_MSG_INFO1_2("ps_dpmi_get_bearer_info(): "
                      "Found bearer entry 0x%p at index %d",
                      bearer_ptr, bearer_index);
      return bearer_ptr;
    }
  }

  LOG_MSG_ERROR_0("ps_dpmi_get_bearer_info(): Couldn't find bearer info");

  return NULL;
} /* ps_dpmi_get_bearer_info() */


static ps_dpm_bearer_info_type * ps_dpmi_get_dpm_bearer_info_by_handle
(
  ps_dpm_um_info_type   * dpm_um_info_ptr,
  int32                   bearer_handle
)
{
  ps_dpm_bearer_info_type   * dpm_bearer_info_ptr = NULL;
  int16                       bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  bearer_index =
    ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_BEARER, bearer_handle);

  if (bearer_index >= PS_DPM_MAX_BEARERS || bearer_index < 0)
  {
    LOG_MSG_ERROR_1("ps_dpmi_get_dpm_bearer_info_by_handle(): "
                    "Invalid DPM bearer index %d", bearer_index);
    return NULL;  
  }
  
  dpm_bearer_info_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
  if (NULL == dpm_bearer_info_ptr ||
      bearer_handle != dpm_bearer_info_ptr->handle)
  {
    LOG_MSG_ERROR_1("ps_dpmi_get_dpm_bearer_info_by_handle(): "
                    "Invalid DPM bearer handle 0x%x", bearer_handle);
    return NULL;
  }

  return dpm_bearer_info_ptr;
} /* ps_dpmi_get_dpm_bearer_info_by_handle() */

static ps_phys_link_type* ps_dpmi_get_phys_link_from_uid
(
  uint8                   uid
)
{
  ps_dpm_bearer_info_type   * bearer_ptr      = NULL;
  ps_dpm_um_info_type       * dpm_um_info_ptr = NULL;
  int16                       bearer_index = 0;
  uint8                       dpm_um_index = 0;
 /*--------------------------------------------------------*/
  for (dpm_um_index = 0; 
       dpm_um_index < ps_dpm_global_info.num_dpm_um_info_ptr &&
       dpm_um_index < PS_DPM_MAX_PDN_SESSIONS; dpm_um_index++)
  {
    /* ------------------------------------------------
          Get the um info from ps_dpm_global_info one by one
      ---------------------------------------------------*/
    dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];
    if(dpm_um_info_ptr != NULL)
    {
      for (bearer_index = 0;
           bearer_index < PS_DPM_MAX_BEARERS ;
           bearer_index++)
      {
       /*----------------------------------------------
        Traverse through all bearers in each um_info and 
        compare against UID.
       ------------------------------------------------*/
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr && uid == bearer_ptr->uid)
        {
          LOG_MSG_INFO1_3("ps_dpmi_get_phys_link_from_uid(): "
                          "Found bearer entry 0x%p for UID %d in um_info 0x%p",
                          bearer_ptr, uid ,dpm_um_info_ptr);
          return bearer_ptr->ps_phys_link_ptr;
        }
      }
    }
  }
  return NULL;
} /* ps_dpmi_get_phys_link_from_uid() */

ps_dpm_bearer_info_type* ps_dpmi_get_bearer_from_phys_link
(
  const ps_phys_link_type	* ps_phys_link_ptr
)
{
  ps_dpm_bearer_info_type   * bearer_ptr      = NULL;
  ps_dpm_um_info_type       * dpm_um_info_ptr = NULL;
  int16                       bearer_index = 0;
  uint8                       dpm_um_index = 0;
  
 /*--------------------------------------------------------*/
  for (dpm_um_index = 0; 
       dpm_um_index < ps_dpm_global_info.num_dpm_um_info_ptr &&
       dpm_um_index < PS_DPM_MAX_PDN_SESSIONS; dpm_um_index++)
  {
    /* ------------------------------------------------
          Get the um info from ps_dpm_global_info one by one
      ---------------------------------------------------*/
    dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];
    if(dpm_um_info_ptr != NULL)
    {
      for (bearer_index = 0;
           bearer_index < PS_DPM_MAX_BEARERS ;
           bearer_index++)
      {
       /*----------------------------------------------
        Traverse through all bearers in each um_info and 
        compare against phys_link.
       ------------------------------------------------*/
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr && ps_phys_link_ptr == bearer_ptr->ps_phys_link_ptr)
        {
          LOG_MSG_INFO1_3("ps_dpmi_get_phys_link_from_uid(): "
                          "Found phys link entry 0x%p for UID 0x%p in um_info 0x%p",
                          bearer_ptr, ps_phys_link_ptr ,dpm_um_info_ptr);
          return bearer_ptr;
        }
      }
    }
  }
  return NULL;
} /* ps_dpmi_get_bearer_from_phys_link */

static void ps_dpmi_free_bearer_info
(
  ps_dpm_um_info_type      * dpm_um_info_ptr,
  ps_dpm_bearer_info_type  * bearer_ptr
)
{
  int16  bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_dpmi_free_bearer_info(): "
                  "Freeing bearer entry 0x%p with handle 0x%x",
                  bearer_ptr, bearer_ptr->handle);

  bearer_index = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_BEARER,
                                         bearer_ptr->handle);
  //MSR TODO validate index. Other places too

  if (bearer_index < 0 || bearer_index >= PS_DPM_MAX_BEARERS)
  {
    LOG_MSG_INFO1_1("ps_dpmi_free_bearer_info(): "
                    "Invalid bearer index %d", bearer_index); 
    ASSERT(0);      
    return;            
  }  
  
  dpm_um_info_ptr->bearer_ptr_arr[bearer_index] = NULL;
  dpm_um_info_ptr->num_bearer--;
  dpm_um_info_ptr->all_bearers_flushed |= (1 << bearer_index);  

  PS_SYSTEM_HEAP_MEM_FREE(bearer_ptr);

  return;
} /* ps_dpmi_free_bearer_info() */


static ps_dpm_rm_info_type * ps_dpmi_alloc_dpm_rm_info
(
  void
)
{
  ps_dpm_rm_info_type  * dpm_rm_info_ptr;
  int16                  dpm_rm_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Find an open slot. Dynamically allocate memory, initialize it, update
      the open slot and return it
    -----------------------------------------------------------------------*/

    for (dpm_rm_index = 0;
         dpm_rm_index < PS_DPM_MAX_PDN_SESSIONS;
         dpm_rm_index++)
    {
      if (NULL == ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index])
      {
        break;
      }
    }

    if (PS_DPM_MAX_PDN_SESSIONS <= dpm_rm_index)
    {
      LOG_MSG_ERROR_0("ps_dpmi_alloc_dpm_rm_info(): "
                      "No free space in DPM Rm tbl");
      break;
    }

    PS_SYSTEM_HEAP_MEM_ALLOC(dpm_rm_info_ptr, sizeof(ps_dpm_rm_info_type),
                             ps_dpm_rm_info_type*);
    if (NULL ==  dpm_rm_info_ptr)
    {
      break;
    }

    memset(dpm_rm_info_ptr, 0, sizeof(ps_dpm_rm_info_type));

    dpm_rm_info_ptr->handle =
      ps_handle_mgr_get_handle(PS_HANDLE_MGR_CLIENT_DPM_RM, dpm_rm_index);

    dpm_rm_info_ptr->dpm_ul_sig             = PS_RM_MAX_SIGNALS;
    dpm_rm_info_ptr->is_eth_addr_configured = FALSE;

    ps_dpm_global_info.num_dpm_rm_info_ptr++;
    ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index] = dpm_rm_info_ptr;

    LOG_MSG_INFO1_2("ps_dpmi_alloc_dpm_rm_info(): "
                    "Allocated DPM Rm entry 0x%p with handle 0x%x",
                    dpm_rm_info_ptr, dpm_rm_info_ptr->handle);
    return dpm_rm_info_ptr;
  } while (0);

  return NULL;
} /* ps_dpmi_alloc_dpm_rm_info() */


static ps_dpm_rm_info_type * ps_dpmi_get_dpm_rm_info
(
  sio_stream_id_type  sio_stream_id
)
{
  ps_dpm_rm_info_type  * dpm_rm_info_ptr;
  int32                  num_dpm_rm_info_ptr = 0;
  int16                  dpm_rm_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (dpm_rm_index = 0;
       dpm_rm_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_rm_info_ptr < ps_dpm_global_info.num_dpm_rm_info_ptr;
       dpm_rm_index++)
  {
    if (NULL != ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index])
    {
      num_dpm_rm_info_ptr++;

      dpm_rm_info_ptr = ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index];

      if (sio_stream_id == dpm_rm_info_ptr->sio_stream_id)
      {
        LOG_MSG_INFO1_2("ps_dpmi_get_dpm_rm_info(): "
                        "Found DPM Rm entry 0x%p at index %d",
                        dpm_rm_info_ptr, dpm_rm_index);
        return dpm_rm_info_ptr;
      }
    }
  }

  LOG_MSG_ERROR_0("ps_dpmi_get_dpm_rm_info(): Couldn't find DPM Rm info");

  return NULL;
} /* ps_dpmi_get_dpm_rm_info() */


static void ps_dpmi_free_dpm_rm_info
(
  ps_dpm_rm_info_type  * dpm_rm_info_ptr
)
{
  int16  dpm_rm_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_dpmi_free_dpm_rm_info(): "
                  "Freeing DPM Rm entry 0x%p with handle 0x%x",
                  dpm_rm_info_ptr, dpm_rm_info_ptr->handle);

  dpm_rm_index =  ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_RM,
                                          dpm_rm_info_ptr->handle);

  if (dpm_rm_index < 0 || dpm_rm_index >= PS_DPM_MAX_PDN_SESSIONS)
  {
    LOG_MSG_ERROR_1("ps_dpmi_free_dpm_rm_info(): "
                    "DPM rm index out of bound %d", dpm_rm_index); 
    return;  
  }  

  ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index] = NULL;
  ps_dpm_global_info.num_dpm_rm_info_ptr--;

  PS_SYSTEM_HEAP_MEM_FREE(dpm_rm_info_ptr);

  return;
} /* ps_dpmi_free_dpm_rm_info() */


static void ps_dpmi_reset_wm_counters
(
  dsm_watermark_type  * dsm_wm_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_wm_ptr->highest_cnt           = 0;
  dsm_wm_ptr->total_rcvd_cnt        = 0;
  dsm_wm_ptr->dne_discard_pkt_cnt   = 0;
  dsm_wm_ptr->dne_discard_bytes_cnt = 0;

  return;
} /* ps_dpmi_reset_wm_counters() */


static void ps_dpmi_sio_rx_non_empty_cback
(
  dsm_watermark_type  * dsm_wm_ptr,
  void                * user_data_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_RM_SET_SIGNAL((ps_rm_sig_enum_type) user_data_ptr);

} /* ps_dpmi_sio_rx_non_empty_cback() */

static void ps_dpmi_set_ul_signal_handler
(
  ps_dpm_rm_info_type  * dpm_rm_info_ptr,
  ps_iface_type        * rm_iface_ptr
)
{
#ifdef FEATURE_DATA_PS_464XLAT
  ps_iface_type        * um_iface_ptr     = NULL;
  ps_iface_type        * bridge_iface_ptr = NULL;
  boolean                is_clat_capable  = FALSE;
#endif /* FEATURE_DATA_PS_464XLAT */  
  ps_iface_type        * um_base_iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_PS_464XLAT 
  um_base_iface_ptr = 
    PS_DPM_GET_BASE_IFACE(PS_IFACEI_GET_BRIDGE_IFACE(rm_iface_ptr));
#else
  um_base_iface_ptr = 
    PS_IFACE_GET_BASE_IFACE(PS_IFACEI_GET_BRIDGE_IFACE(rm_iface_ptr));
#endif /* FEATURE_DATA_PS_464XLAT */  
                  
  if (PS_IFACE_IS_VALID(um_base_iface_ptr) && 
      EMBMS_IFACE == PS_IFACE_GET_NAME(um_base_iface_ptr))   
  {      
    /*---------------------------------------------------------------------
      Drop the packet for embms iface
    ---------------------------------------------------------------------*/  
    (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                 ps_dpm_ul_opt_embms_rx_sig_hdlr,
                                 (void *) dpm_rm_info_ptr);       
  }
  else
  {  
  
#ifdef FEATURE_DATA_PS_464XLAT  
    /*-----------------------------------------------------------------------
      Check the CLAT capbility on the um_iface_ptr. Fetch base iface of the 
      bridge iface in order to find the corresponding um_iface.
    -----------------------------------------------------------------------*/
    bridge_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(rm_iface_ptr);
    if (PS_IFACE_IS_VALID(bridge_iface_ptr))
    {
      um_iface_ptr = PS_DPM_GET_BASE_IFACE(bridge_iface_ptr);
      if (PS_IFACE_IS_VALID(um_iface_ptr))
      {
        if(PS_IFACE_GET_CAPABILITY(um_iface_ptr,
                                   PS_IFACE_CAPABILITY_CLAT_CAPABLE))
        {
          is_clat_capable = TRUE;
          LOG_MSG_INFO2_0("ps_dpmi_set_ul_signal_handler() : CLAT Capable");
        }
      }
    }
    
    /*-----------------------------------------------------------------------
      Use clat sig handler path only for translate mode. Take normal path
      for pass through mode.
    -----------------------------------------------------------------------*/    
    if (is_clat_capable == TRUE && 
        IPV6_ADDR == ps_iface_get_addr_family(um_base_iface_ptr))
    {
      if (RMNET_ENABLE_DATA_AGG_MBIM ==  dpm_rm_info_ptr->data_agg_protocol ||
          RMNET_ENABLE_DATA_AGG_QC_NCM == dpm_rm_info_ptr->data_agg_protocol)
      {
        (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                     ps_dpm_mbim_ul_opt_clat_ip_rx_sig_hdlr,
                                     (void *) dpm_rm_info_ptr);
      }    
      else if (RMNET_IP_MODE == dpm_rm_info_ptr->link_prot)
      {
        if (PS_DPM_QOS_DISABLED == dpm_rm_info_ptr->data_format)
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                     ps_dpm_ul_opt_clat_ip_no_qos_rx_sig_hdlr,
                    (void *) dpm_rm_info_ptr);
        }
        else
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                ps_dpm_ul_opt_clat_ip_qos_rx_sig_hdlr,
                (void *) dpm_rm_info_ptr);

        }
      }
      else
      {
        if (PS_DPM_QOS_DISABLED == dpm_rm_info_ptr->data_format)
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                ps_dpm_ul_opt_clat_eth_no_qos_rx_sig_hdlr,
                (void *) dpm_rm_info_ptr);
        }
        else
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
               ps_dpm_ul_opt_clat_eth_qos_rx_sig_hdlr,
               (void *) dpm_rm_info_ptr);
        }
      }
    }
    else 
#endif  /* FEATURE_DATA_PS_464XLAT */  
    {
      if (RMNET_ENABLE_DATA_AGG_MBIM ==  dpm_rm_info_ptr->data_agg_protocol ||
          RMNET_ENABLE_DATA_AGG_QC_NCM == dpm_rm_info_ptr->data_agg_protocol)
      {
        (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                     ps_dpm_mbim_ul_opt_ip_rx_sig_hdlr,
                                     (void *) dpm_rm_info_ptr);
      }    
       /*-----------------------------------------------------------------------
        Register UL signal hdlr for SIO
      -----------------------------------------------------------------------*/
      else if (RMNET_IP_MODE == dpm_rm_info_ptr->link_prot)
      {
        if (PS_DPM_QOS_DISABLED == dpm_rm_info_ptr->data_format)
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                       ps_dpm_ul_opt_ip_no_qos_rx_sig_hdlr,
                                       (void *) dpm_rm_info_ptr);
        }
        else
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                       ps_dpm_ul_opt_ip_qos_rx_sig_hdlr,
                                       (void *) dpm_rm_info_ptr);
        }
      }
      else
      {
        if (PS_DPM_QOS_DISABLED == dpm_rm_info_ptr->data_format)
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                       ps_dpm_ul_opt_eth_no_qos_rx_sig_hdlr,
                                       (void *) dpm_rm_info_ptr);
        }
        else
        {
          (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig,
                                       ps_dpm_ul_opt_eth_qos_rx_sig_hdlr,
                                       (void *) dpm_rm_info_ptr);
        }
      }    
    }    
  }
} /* ps_dpmi_set_ul_signal_handler */

static int32 ps_dpmi_setup_sio_wm
(
  ps_dpm_rm_info_type  * dpm_rm_info_ptr,
  dsm_watermark_type   * sio_tx_wm_ptr,
  dsm_watermark_type   * sio_rx_wm_ptr,
  ps_iface_type        * rm_iface_ptr
)
{
  int32  ul_sig_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup UL sig for DPM Rm info
  -------------------------------------------------------------------------*/
  for (ul_sig_index = 0; ul_sig_index < PS_DPM_UL_MAX_SIGNALS; ul_sig_index++)
  {
    if (FALSE == ps_dpm_ul_sig_in_use_tbl[ul_sig_index])
    {
      dpm_rm_info_ptr->dpm_ul_sig =
        (ps_rm_sig_enum_type) (PS_RM_RMNET_RX_DATA_Q_SIGNAL_1 + ul_sig_index);
      ps_dpm_ul_sig_in_use_tbl[ul_sig_index] = TRUE;

      ps_rm_enable_sig(dpm_rm_info_ptr->dpm_ul_sig);
      break;
    }
  }

  if (PS_DPM_UL_MAX_SIGNALS <= ul_sig_index)
  {
    LOG_MSG_ERROR_0("ps_dpmi_setup_sio_wm(): Couldn't find UL sig");
    return -1;
  }

  LOG_MSG_INFO1_3("ps_dpmi_setup_sio_wm(): Assigned UL sig %d for link mode %d"
                  " data agg protocol %d", dpm_rm_info_ptr->dpm_ul_sig,
                  dpm_rm_info_ptr->link_prot,
                  dpm_rm_info_ptr->data_agg_protocol);

  ps_dpmi_set_ul_signal_handler(dpm_rm_info_ptr, rm_iface_ptr);

  /*-------------------------------------------------------------------------
    Reset WM counters
  -------------------------------------------------------------------------*/
  ps_dpmi_reset_wm_counters(sio_tx_wm_ptr);
  ps_dpmi_reset_wm_counters(sio_rx_wm_ptr);

  /*-------------------------------------------------------------------------
    Update DPM Rm info with SIO WM and setup non empty cb
  -------------------------------------------------------------------------*/
  dpm_rm_info_ptr->sio_tx_wm_ptr = sio_tx_wm_ptr;
  dpm_rm_info_ptr->sio_rx_wm_ptr = sio_rx_wm_ptr;

  dpm_rm_info_ptr->sio_rx_wm_ptr->non_empty_func_ptr  =
    ps_dpmi_sio_rx_non_empty_cback;
  dpm_rm_info_ptr->sio_rx_wm_ptr->non_empty_func_data =
    (void *) (dpm_rm_info_ptr->dpm_ul_sig);

  return 0;
} /* ps_dpmi_setup_sio_wm() */


static void ps_dpmi_reset_sio_wm
(
  ps_dpm_rm_info_type  * dpm_rm_info_ptr
)
{
  int32  ul_sig_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reset UL sig hdlr and UL sig.

    It may not be needed to reset UL sig hdlr as UL sig is cleared but doing
    it as a safe side. If sig hdlr gets called, and if DPM Rm info is already
    freed, then sig hdlr would access already freed memory
  ------------------------------------------------------------------------*/
  if (dpm_rm_info_ptr->dpm_ul_sig < PS_RM_MAX_SIGNALS &&
      dpm_rm_info_ptr->dpm_ul_sig > 0)
  {
    (void) ps_rm_set_sig_handler(dpm_rm_info_ptr->dpm_ul_sig, NULL, NULL);
    PS_RM_CLR_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);

    ul_sig_index = dpm_rm_info_ptr->dpm_ul_sig - PS_RM_RMNET_RX_DATA_Q_SIGNAL_1;
    
    if (ul_sig_index < 0 || ul_sig_index >= PS_DPM_UL_MAX_SIGNALS)
    {
      LOG_MSG_ERROR_1("ps_dpmi_reset_sio_wm(): Invalid sig index  %d",
                      ul_sig_index); 
      ASSERT(0); 
      return;                      
    }

    ps_dpm_ul_sig_in_use_tbl[ul_sig_index] = FALSE;

    LOG_MSG_INFO1_1("ps_dpmi_reset_sio_wm(): Reclaimed UL sig %d",
                    dpm_rm_info_ptr->dpm_ul_sig);
  }

  /*-------------------------------------------------------------------------
    Reset SIO Tx and Rx WM
  -------------------------------------------------------------------------*/
  if (NULL != dpm_rm_info_ptr->sio_rx_wm_ptr)
  {
    dsm_empty_queue(dpm_rm_info_ptr->sio_rx_wm_ptr);
    dpm_rm_info_ptr->sio_rx_wm_ptr->non_empty_func_ptr = NULL;
    dpm_rm_info_ptr->sio_rx_wm_ptr->non_empty_func_data = NULL;
  }

  if (NULL != dpm_rm_info_ptr->sio_tx_wm_ptr)
  {
    dsm_empty_queue(dpm_rm_info_ptr->sio_tx_wm_ptr);
    dpm_rm_info_ptr->sio_tx_wm_ptr->non_empty_func_ptr = NULL;
    dpm_rm_info_ptr->sio_tx_wm_ptr->non_empty_func_data = NULL;
  }

  return;
} /* ps_dpmi_reset_sio_wm() */


/*===========================================================================
FUNCTION   PS_DPM_IFACE_EV_HDLR_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dpmi_um_iface_ev_cback
(
  ps_iface_type             * this_iface_ptr,
  ps_iface_event_enum_type    event_name,
  ps_iface_event_info_u_type  event_info,
  void                      * user_data_ptr
)
{
  ps_dpm_rm_info_type    * dpm_rm_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data_ptr;

  LOG_MSG_INFO2_2("ps_dpmi_um_iface_ev_cback(): Um iface 0x%p DPM info 0x%p",
                  this_iface_ptr, dpm_rm_info_ptr);

  if (IFACE_FLOW_ENABLED_EV == event_name)
  {
    /*-----------------------------------------------------------------------
      Set the signal so that data can be dequeued again. If Um is still
      flow controlled, then sig hdlr will reset the signal

      //MSR TODO Ideally a cmd should be posted here so that DPM info is not
      //accessed in non PS RM task
    -----------------------------------------------------------------------*/
    PS_RM_SET_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);
  }
  else
  {
    LOG_MSG_ERROR_1("ps_dpmi_um_iface_ev_cback(): Unknown event %d",
                    event_name);
  }

  return;
} /* ps_dpmi_um_iface_ev_cback() */


/*===========================================================================
FUNCTION   PS_DPM_UM_FLOW_EV_HDLR CB()

DESCRIPTION
   This callback function is called when one of the registered ps_phys_link events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dpmi_um_flow_ev_cback
(
  ps_flow_type                * this_flow_ptr,
  ps_iface_event_enum_type      event_name,
  ps_iface_event_info_u_type    event_info,
  void                        * user_data_ptr
)
{
  ps_dpm_rm_info_type    * dpm_rm_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data_ptr;

  LOG_MSG_INFO2_2("ps_dpmi_um_flow_ev_cback(): ps flow 0x%p DPM info 0x%p",
                  this_flow_ptr, dpm_rm_info_ptr);

  if (FLOW_TX_ENABLED_EV == event_name)
  {
    /*-----------------------------------------------------------------------
      Set the signal so that data can be dequeued again. If Um is still
      flow controlled, then sig hdlr will reset the signal

      //MSR TODO Ideally a cmd should be posted here so that DPM info is not
      //accessed in non PS RM task
    -----------------------------------------------------------------------*/
    PS_RM_SET_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);
  }
  else
  {
    LOG_MSG_ERROR_1("ps_dpmi_um_flow_ev_cback(): Unknown event %d",
                    event_name);
  }

  return;
} /* ps_dpmi_um_flow_ev_cback() */


/*===========================================================================
FUNCTION   PS_DPM_UM_PHYS_LINK_EV_HDLR CB()

DESCRIPTION
  This callback function is called when one of the registered ps_phys_link events
  occur on Um interface.  Various Um iface events are registered based on
  call type during the call setup type.  Each iface event generates
  appropriate rmnet_meta_sm event .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dpmi_um_phys_link_ev_cback
(
  ps_phys_link_type             * this_phys_link_ptr,
  ps_iface_event_enum_type        event_name,
  ps_iface_event_info_u_type      event_info,
  void                          * user_data_ptr
)
{
  ps_dpm_rm_info_type    * dpm_rm_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data_ptr;

  LOG_MSG_INFO2_3("ps_dpmi_um_phys_link_ev_cback(): "
                  "phys link 0x%p DPM info 0x%p event %d",
                  this_phys_link_ptr, dpm_rm_info_ptr, event_name);

  if (PHYS_LINK_FLOW_ENABLED_EV == event_name)
  {
    /*-----------------------------------------------------------------------
      Set the signal so that data can be dequeued again. If Um is still
      flow controlled, then sig hdlr will reset the signal

      //MSR TODO Ideally a cmd should be posted here so that DPM info is not
      //accessed in non PS RM task
    -----------------------------------------------------------------------*/
    PS_RM_SET_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);
  }
  else
  {
    LOG_MSG_ERROR_1("ps_dpmi_um_phys_link_ev_cback(): Unknown event %d",
                    event_name);
  }

  return;
} /* ps_dpmi_um_phys_link_ev_cback() */

/*===========================================================================
FUNCTION   PS_DPM_HANDOFF_SUCCESS_EV_CBACK()

DESCRIPTION
  This callback function is called when bearer tech changes corresponding to
  call. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dpmi_handoff_success_ev_cback
(
  ps_iface_type                 * this_iface_ptr,
  ps_iface_event_enum_type        event_name,
  ps_iface_event_info_u_type      event_info,
  void                          * user_data_ptr
)
{
  ps_dpm_rm_info_type                  * dpm_rm_info_ptr;
  ps_dpm_cmd_info_type                 * dpm_cmd_info_ptr;
  ps_dpm_handoff_success_ev_info_type    handoff_success_ev_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data_ptr;

  LOG_MSG_INFO2_3("ps_dpmi_handoff_success_ev_cback(): "
                  "iface 0x%p DPM rm info 0x%p event %d",
                  this_iface_ptr, dpm_rm_info_ptr, event_name);

  handoff_success_ev_info.sio_stream_id = dpm_rm_info_ptr->sio_stream_id;
  handoff_success_ev_info.rm_iface_ptr = 
    PS_IFACEI_GET_BRIDGE_IFACE(this_iface_ptr);
  
  if (!PS_IFACE_IS_VALID(handoff_success_ev_info.rm_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_dpmi_handoff_success_ev_cback() : " 
                    "Invalid rm iface 0x%p", 
                    handoff_success_ev_info.rm_iface_ptr);
    return;
  }
  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                                  ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_HANDOFF_SUCCESS_EV_CMD;
  
  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.handoff_success_ev_info),
          sizeof(ps_dpm_handoff_success_ev_info_type),
          &handoff_success_ev_info,
          sizeof(ps_dpm_handoff_success_ev_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpmi_handoff_success_ev_cback() */

/*===========================================================================
FUNCTION   PS_DPMI_PREFIX_UPDATED_EV_CBACK()

DESCRIPTION
  This callback function is called when prefix is added

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dpmi_prefix_updated_ev_cback
(
  ps_iface_type                 * this_iface_ptr,
  ps_iface_event_enum_type        event_name,
  ps_iface_event_info_u_type      event_info,
  void                          * user_data_ptr
)
{
  ps_dpm_rm_info_type                  * dpm_rm_info_ptr;
  ps_dpm_cmd_info_type                 * dpm_cmd_info_ptr;
  ps_dpm_prefix_updated_ev_info_type   * prefix_updated_ev_info_ptr;
  ps_iface_type                        * um_base_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_dpmi_prefix_updated_ev_cback(): "
                  "iface 0x%p event %d",
                  this_iface_ptr, event_name);

  dpm_rm_info_ptr = (ps_dpm_rm_info_type *) user_data_ptr;

  #ifdef FEATURE_DATA_PS_464XLAT
    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(this_iface_ptr);
  #else
    um_base_iface_ptr = PS_IFACE_GET_BASE_IFACE(this_iface_ptr);
  #endif /* FEATURE_DATA_PS_464XLAT */

  if (!PS_IFACE_IS_VALID(um_base_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_dpmi_prefix_updated_ev_cback() : " 
                    "Invalid um iface", um_base_iface_ptr);
    return;
  }

  if ( !(PREFIX_ADDED == event_info.prefix_info.kind ||
  	     PREFIX_REMOVED == event_info.prefix_info.kind ))
  {
    LOG_MSG_INFO1_0("ps_dpmi_prefix_updated_ev_cback() : "
		    "prefix info kind is neither added nor removed");
    return;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  dpm_cmd_info_ptr = ps_system_heap_mem_alloc(sizeof(ps_dpm_cmd_info_type));
  if (NULL == dpm_cmd_info_ptr)
  {
    DATA_ERR_FATAL("ps_dpmi_prefix_updated_ev_cback(): Failed to alloc DPM cmd buf");
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_PREFIX_UPDATED_EV_CMD;

  prefix_updated_ev_info_ptr = 
     &(dpm_cmd_info_ptr->dpm_cmd_info.prefix_updated_ev_info);

  prefix_updated_ev_info_ptr->sio_stream_id = dpm_rm_info_ptr->sio_stream_id;
  prefix_updated_ev_info_ptr->um_iface_ptr = um_base_iface_ptr;
  prefix_updated_ev_info_ptr->prefix_update_kind = event_info.prefix_info.kind;

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpmi_prefix_updated_ev_cback() */

/*===========================================================================
FUNCTION   PS_DPM_GET_IFACE_STAT_ADJUSTMENT()

DESCRIPTION
  This function retrieves DL stats from hardware and updates iface with it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_dpm_get_iface_stat_adjustment
(
  ps_iface_type                  * iface_ptr,
  ps_iface_stat_adjustment_type  * adjustment
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr = NULL;
  ps_dpm_bearer_info_type  * bearer_ptr;
  int16                      bearer_index;  
  int                        ret_val;
  ps_dpm_hw_stats_info_type  stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_2("ps_dpm_get_iface_stat_adjustment() : iface_ptr 0x%x:%d ", 
                  PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->name:0,
                  PS_IFACE_IS_VALID(iface_ptr)?iface_ptr->instance:0);
  
  memset(adjustment, 0, sizeof(ps_iface_stat_adjustment_type));

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  do
  {
    if (! PS_IFACE_IS_VALID(iface_ptr))
    {
      break;
    }
    
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      break;
    }

    for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
    {
      bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
      if (NULL != bearer_ptr)
      {
        ret_val = ps_dpm_hw_get_iface_stat_adjustment(bearer_ptr->uid,
                                                      iface_ptr, 
                                                      &stats);

        if (0 != ret_val)
        {
          LOG_MSG_ERROR_2("ps_dpm_get_iface_stat_adjustment(): "
                          "Couldn't get hardware stats uid %d  err %d",
                          bearer_ptr->uid, ret_val);
          break;
        }
                            
        if(PS_IFACE_IS_ADDR_FAMILY_V4(iface_ptr) == TRUE)
        {
          adjustment->bytes_rx += stats.dl_stats.num_ipv4_bytes;
          adjustment->pkts_rx += stats.dl_stats.num_ipv4_pkts;
          LOG_MSG_INFO3_5("ps_dpm_get_iface_stat_adjustment() v4 pkt: %d,"
                          "v4 byte: %llu adj bytes rx %llu adj pkt rx %d uid %d", 
                          stats.dl_stats.num_ipv4_pkts,
                           stats.dl_stats.num_ipv4_bytes, 
                           adjustment->bytes_rx,
                           adjustment->pkts_rx,
                           bearer_ptr->uid);
        }
        else
        {
          adjustment->bytes_rx += stats.dl_stats.num_ipv6_bytes;
          adjustment->pkts_rx += stats.dl_stats.num_ipv6_pkts;
          LOG_MSG_INFO3_5("ps_dpm_get_iface_stat_adjustment() v6 pkt: %d,"
                          "v6 byte: %llu adj bytes rx %llu adj pkt rx %d uid %d", 
                          stats.dl_stats.num_ipv6_pkts,
                           stats.dl_stats.num_ipv6_bytes, 
                           adjustment->bytes_rx,
                           adjustment->pkts_rx,
                           bearer_ptr->uid);
        }
      }
    }
    
    PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    
    return;
  } while(0);  

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  LOG_MSG_ERROR_1("ps_dpm_get_iface_stat_adjustment() : Stat adj failed 0x%p",
                  dpm_um_info_ptr);
  return;                  
} /* ps_dpm_get_iface_stat_adjustment */


/*===========================================================================
  FUNCTION PS_DPM_REG_IFACE_EVENTS()

  DESCRIPTION
    This function registers dpm for iface events.

  PARAMETERS
    dpm_info_ptr - pointer to the dpm control block

  RETURN VALUE
    TRUE if success
    FALSE if failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int32 ps_dpmi_reg_iface_events
(
  ps_dpm_rm_info_type        * dpm_rm_info_ptr,
  ps_iface_addr_family_type    addr_family
)
{
  ps_iface_type   * um_iface_ptr;
  void           ** iface_flow_enabled_buf_ptr_ptr = NULL;
  void           ** prefix_updated_buf_ptr_ptr = NULL;
  void           ** handoff_success_buf_ptr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (IPV4_ADDR == addr_family)
    {
      um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v4_iface_ptr);

      iface_flow_enabled_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v4_iface_flow_enabled_buf_ptr);
        
      handoff_success_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v4_handoff_success_buf_ptr);      
    }
    else
    {
      um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v6_iface_ptr);

      iface_flow_enabled_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v6_iface_flow_enabled_buf_ptr);
      
      handoff_success_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v6_handoff_success_buf_ptr);      
        
      prefix_updated_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v6_prefix_updated_buf_ptr);        
    }

    if (NULL != iface_flow_enabled_buf_ptr_ptr)
    {
      *iface_flow_enabled_buf_ptr_ptr =
        ps_iface_alloc_event_cback_buf(ps_dpmi_um_iface_ev_cback,
                                       dpm_rm_info_ptr);

      if (-1 == ps_iface_event_cback_reg(um_iface_ptr,
                                         IFACE_FLOW_ENABLED_EV,
                                         *iface_flow_enabled_buf_ptr_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpmi_reg_iface_events(): "
                        "Couldn't reg for IFACE_FLOW_ENABLED_EV");
        break;
      }
    }
    
    if (NULL != handoff_success_buf_ptr_ptr)
    {
      *handoff_success_buf_ptr_ptr =
        ps_iface_alloc_event_cback_buf(ps_dpmi_handoff_success_ev_cback,
                                       dpm_rm_info_ptr);

      if (-1 == ps_iface_event_cback_reg(um_iface_ptr,
                                         IFACE_HANDOFF_STATUS_SUCCESS_EV,
                                         *handoff_success_buf_ptr_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpmi_reg_iface_events(): "
                        "Couldn't reg for IFACE_HANDOFF_STATUS_SUCCESS_EV");
        break;
      }
    }    
         
    if (NULL != prefix_updated_buf_ptr_ptr)
    {
      *prefix_updated_buf_ptr_ptr =
        ps_iface_alloc_event_cback_buf(ps_dpmi_prefix_updated_ev_cback,
                                       dpm_rm_info_ptr);

      if (-1 == ps_iface_event_cback_reg(um_iface_ptr,
                                         IFACE_PREFIX_UPDATE_EV,
                                         *prefix_updated_buf_ptr_ptr))
      {
        LOG_MSG_ERROR_0("ps_dpmi_reg_iface_events(): "
                        "Couldn't reg for IFACE_PREFIX_UPDATE_EV");
        break;
      }    
    }
    
    return 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Free the allocated buffer
  -------------------------------------------------------------------------*/
  if (NULL != iface_flow_enabled_buf_ptr_ptr &&
      NULL != *iface_flow_enabled_buf_ptr_ptr)
  {
    ps_iface_free_event_cback_buf(*iface_flow_enabled_buf_ptr_ptr);
    *iface_flow_enabled_buf_ptr_ptr = NULL;
  }

  if (NULL != handoff_success_buf_ptr_ptr &&
      NULL != *handoff_success_buf_ptr_ptr)
  {
    ps_iface_free_event_cback_buf(*handoff_success_buf_ptr_ptr);
    *handoff_success_buf_ptr_ptr = NULL;
  }  
  
  if (NULL != prefix_updated_buf_ptr_ptr &&
      NULL != *prefix_updated_buf_ptr_ptr)
  {
    ps_iface_free_event_cback_buf(*prefix_updated_buf_ptr_ptr);
    *prefix_updated_buf_ptr_ptr = NULL;
  }  

  return -1;
} /* ps_dpmi_reg_iface_events() */


/*===========================================================================
  FUNCTION PS_DPM_DEREG_IFACE_EVENTS()

  DESCRIPTION
    This function de-Registers DPM for iface events.

  PARAMETERS
    dpm_info_ptr - pointer to the dpm control block

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ps_dpmi_dereg_iface_events
(
  ps_dpm_rm_info_type        * dpm_rm_info_ptr,
  ps_iface_addr_family_type    addr_family
)
{
  ps_iface_type  * um_iface_ptr;
  void           * iface_flow_enabled_buf_ptr;
  void           * prefix_updated_buf_ptr = NULL;
  void           * handoff_success_buf_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (IPV4_ADDR == addr_family)
  {
    um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v4_iface_ptr);

    iface_flow_enabled_buf_ptr = dpm_rm_info_ptr->v4_iface_flow_enabled_buf_ptr;
    dpm_rm_info_ptr->v4_iface_flow_enabled_buf_ptr = NULL;
    
    handoff_success_buf_ptr = dpm_rm_info_ptr->v4_handoff_success_buf_ptr;
    dpm_rm_info_ptr->v4_handoff_success_buf_ptr = NULL;    
  }
  else
  {
    um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v6_iface_ptr);

    iface_flow_enabled_buf_ptr = dpm_rm_info_ptr->v6_iface_flow_enabled_buf_ptr;
    dpm_rm_info_ptr->v6_iface_flow_enabled_buf_ptr = NULL;
    
    handoff_success_buf_ptr = dpm_rm_info_ptr->v6_handoff_success_buf_ptr;
    dpm_rm_info_ptr->v6_handoff_success_buf_ptr = NULL;
    
    prefix_updated_buf_ptr = dpm_rm_info_ptr->v6_prefix_updated_buf_ptr;
    dpm_rm_info_ptr->v6_prefix_updated_buf_ptr = NULL;    
  }

  if (NULL != iface_flow_enabled_buf_ptr)
  {
    ps_iface_event_cback_dereg(um_iface_ptr,
                               IFACE_FLOW_ENABLED_EV,
                               iface_flow_enabled_buf_ptr);

    ps_iface_free_event_cback_buf(iface_flow_enabled_buf_ptr);
  }

  if (NULL != handoff_success_buf_ptr)
  {
    ps_iface_event_cback_dereg(um_iface_ptr,
                               IFACE_HANDOFF_STATUS_SUCCESS_EV,
                               handoff_success_buf_ptr);

    ps_iface_free_event_cback_buf(handoff_success_buf_ptr);
  }  
  
  if (NULL != prefix_updated_buf_ptr)
  {
    ps_iface_event_cback_dereg(um_iface_ptr,
                               IFACE_PREFIX_UPDATE_EV,
                               prefix_updated_buf_ptr);

    ps_iface_free_event_cback_buf(prefix_updated_buf_ptr);
  }  

  return;
} /* ps_dpmi_dereg_iface_events() */


/*===========================================================================
  FUNCTION PS_DPM_REG_FLOW_EVENTS()

  DESCRIPTION
    This function registers dpm for flow events.

  PARAMETERS
    dpm_info_ptr - pointer to the dpm control block

  RETURN VALUE
    TRUE if success
    FALSE if failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int32 ps_dpmi_reg_flow_events
(
  ps_dpm_rm_info_type        * dpm_rm_info_ptr,
  ps_iface_addr_family_type    addr_family
)
{
  ps_iface_type   * um_iface_ptr;
  ps_flow_type   * flow_ptr;
  void          ** flow_tx_enabled_buf_ptr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (IPV4_ADDR == addr_family)
    {
      um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v4_iface_ptr);

      flow_tx_enabled_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v4_flow_tx_enabled_buf_ptr);
    }
    else
    {
      um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v6_iface_ptr);

      flow_tx_enabled_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v6_flow_tx_enabled_buf_ptr);
    }

    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
    if (!PS_FLOW_IS_VALID(flow_ptr))
    {
      break;
    }

    *flow_tx_enabled_buf_ptr_ptr =
      ps_flow_alloc_event_cback_buf(ps_dpmi_um_flow_ev_cback, dpm_rm_info_ptr);

    if (-1 == ps_flow_event_cback_reg(flow_ptr,
                                      FLOW_TX_ENABLED_EV,
                                      *flow_tx_enabled_buf_ptr_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_reg_flow_events(): "
                      "Couldn't reg for FLOW_TX_ENABLED_EV");
      break;
    }

    return 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Free the allocated buffer
  -------------------------------------------------------------------------*/
  if (NULL != flow_tx_enabled_buf_ptr_ptr &&
      NULL != *flow_tx_enabled_buf_ptr_ptr)
  {
    ps_flow_free_event_cback_buf(*flow_tx_enabled_buf_ptr_ptr);
    *flow_tx_enabled_buf_ptr_ptr = NULL;
  }

  return -1;
} /* ps_dpmi_reg_flow_events() */


/*===========================================================================
  FUNCTION PS_DPM_DEREG_FLOW_EVENTS()

  DESCRIPTION
    This function de-Registers DPM for flow events.

  PARAMETERS
    dpm_info_ptr - pointer to the dpm control block

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ps_dpmi_dereg_flow_events
(
  ps_dpm_rm_info_type        * dpm_rm_info_ptr,
  ps_iface_addr_family_type    addr_family
)
{
  ps_iface_type  * um_iface_ptr;
  ps_flow_type   * flow_ptr;
  void           * flow_tx_enabled_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (IPV4_ADDR == addr_family)
  {
    um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v4_iface_ptr);

    flow_tx_enabled_buf_ptr = dpm_rm_info_ptr->v4_flow_tx_enabled_buf_ptr;
    dpm_rm_info_ptr->v4_flow_tx_enabled_buf_ptr =  NULL;
  }
  else
  {
    um_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE(dpm_rm_info_ptr->v6_iface_ptr);

    flow_tx_enabled_buf_ptr = dpm_rm_info_ptr->v6_flow_tx_enabled_buf_ptr;
    dpm_rm_info_ptr->v6_flow_tx_enabled_buf_ptr =  NULL;
  }

  flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    LOG_MSG_ERROR_1("ps_dpmi_dereg_flow_events(): Invalid ps flow 0x%p",
                    flow_ptr);
    ASSERT(0);
    return;
  }

  if (NULL != flow_tx_enabled_buf_ptr)
  {
    ps_flow_event_cback_dereg(flow_ptr,
                              FLOW_TX_ENABLED_EV,
                              flow_tx_enabled_buf_ptr);

    ps_flow_free_event_cback_buf(flow_tx_enabled_buf_ptr);
  }

  return;
} /* ps_dpmi_dereg_flow_events() */

/*===========================================================================
  FUNCTION PS_DPM_REG_PHYSLINK_EVENTS()

  DESCRIPTION
    This function registers dpm for phys link events.

  PARAMETERS
    dpm_info_ptr - pointer to the dpm control block

  RETURN VALUE
    TRUE if success
    FALSE if failure

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static int32 ps_dpmi_reg_phys_link_events
(
  ps_dpm_rm_info_type        * dpm_rm_info_ptr,
  ps_iface_addr_family_type    addr_family
)
{
  ps_phys_link_type   * phys_link_ptr;
  void               ** phys_link_flow_enabled_buf_ptr_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (IPV4_ADDR == addr_family)
    {
      phys_link_ptr = dpm_rm_info_ptr->v4_phys_link_ptr;

      phys_link_flow_enabled_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v4_phys_link_flow_enabled_buf_ptr);
    }
    else
    {
      phys_link_ptr = dpm_rm_info_ptr->v6_phys_link_ptr;
      
      phys_link_flow_enabled_buf_ptr_ptr =
        &(dpm_rm_info_ptr->v6_phys_link_flow_enabled_buf_ptr);
    }

    if (!PS_PHYS_LINK_IS_VALID(phys_link_ptr))
    {
      break;
    }

    *phys_link_flow_enabled_buf_ptr_ptr =
      ps_phys_link_alloc_event_cback_buf(ps_dpmi_um_phys_link_ev_cback,
                                         dpm_rm_info_ptr);

    if (-1 == ps_phys_link_event_cback_reg(phys_link_ptr,
                                           PHYS_LINK_FLOW_ENABLED_EV,
                                           *phys_link_flow_enabled_buf_ptr_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_reg_phys_link_events(): "
                      "Couldn't reg for PHYS_LINK_FLOW_ENABLED_EV");
      break;
    }

    return 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Free the allocated buffer
  -------------------------------------------------------------------------*/
  if (NULL != phys_link_flow_enabled_buf_ptr_ptr &&
      NULL != *phys_link_flow_enabled_buf_ptr_ptr)
  {
    ps_phys_link_free_event_cback_buf(*phys_link_flow_enabled_buf_ptr_ptr);
    *phys_link_flow_enabled_buf_ptr_ptr = NULL;
  }
  return -1;
} /* ps_dpmi_reg_phys_link_events() */

/*===========================================================================
  FUNCTION PS_DPM_DEREG_PHYSLINK_EVENTS()

  DESCRIPTION
    This function de-egisters DPM for phys link events.

  PARAMETERS
    dpm_info_ptr - pointer to the dpm control block

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void ps_dpmi_dereg_phys_link_events
(
  ps_dpm_rm_info_type        * dpm_rm_info_ptr,
  ps_iface_addr_family_type    addr_family
)
{
  ps_phys_link_type  * phys_link_ptr;
  void               * phys_link_flow_enabled_buf_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (IPV4_ADDR == addr_family)
  {
    phys_link_flow_enabled_buf_ptr =
      dpm_rm_info_ptr->v4_phys_link_flow_enabled_buf_ptr;
    if (NULL == phys_link_flow_enabled_buf_ptr)
    {
      return;
    }

    phys_link_ptr = dpm_rm_info_ptr->v4_phys_link_ptr;
    dpm_rm_info_ptr->v4_phys_link_flow_enabled_buf_ptr = NULL;
  }
  else
  {
    phys_link_flow_enabled_buf_ptr =
      dpm_rm_info_ptr->v6_phys_link_flow_enabled_buf_ptr;
    if (NULL == phys_link_flow_enabled_buf_ptr)
    {
      return;
    }
    
    phys_link_ptr = dpm_rm_info_ptr->v6_phys_link_ptr;
    dpm_rm_info_ptr->v6_phys_link_flow_enabled_buf_ptr = NULL;
  }

  if (!PS_PHYS_LINK_IS_VALID(phys_link_ptr))
  {
    LOG_MSG_ERROR_1("ps_dpmi_dereg_phys_link_events(): Invalid phys link 0x%p",
                    phys_link_ptr);
    ASSERT(0);
    return;
  }

  if (NULL != phys_link_flow_enabled_buf_ptr)
  {
    ps_phys_link_event_cback_dereg(phys_link_ptr,
                                   PHYS_LINK_FLOW_ENABLED_EV,
                                   phys_link_flow_enabled_buf_ptr);

    ps_phys_link_free_event_cback_buf(phys_link_flow_enabled_buf_ptr);
  }

  return;
} /* ps_dpmi_dereg_phys_link_events() */


void ps_dpm_process_qos_hdr
(
  dsm_item_type          * pkt_ptr,
  ps_iface_type          * um_iface_ptr,
  ip_version_enum_type     ip_ver,
  boolean                  is_opt,
  ps_tx_meta_info_type   * tx_meta_info_ptr,
  void                   * qos_hdr,
  ps_dpm_qos_format        qos_hdr_format,
  ps_flow_type           **flow_ptr_ret
)
{
  uint16          ip_id;

#ifdef  FEATURE_EPC_HANDOFF
  ps_flow_type  * assoc_flow_ptr;
#endif

  int             return_val;
  int16           ps_errno;
  ps_flow_type *  flow_ptr = NULL;
  ps_dpm_qos_hdr_type*  qos_6_byte_hdr = NULL;
  ps_dpm_qos2_hdr_type* qos_8_byte_hdr = NULL;
  uint32                qos_handle = 0;
  uint8                 version = 0;
  uint8                 flags = 0 ;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (qos_hdr == NULL || NULL == um_iface_ptr )
  {
    LOG_MSG_ERROR_1("QOS header is NULL or um iface 0x%p is NULL", um_iface_ptr);
    return;
  }
  if (qos_hdr_format == PS_DPM_QOS_FORMAT_8_BYTE)
  {
    qos_8_byte_hdr = (ps_dpm_qos2_hdr_type*)qos_hdr;
    qos_handle     = qos_8_byte_hdr->hdr.qos_handle;
    version        = qos_8_byte_hdr->hdr.version;
    flags          = qos_8_byte_hdr->hdr.flags; 
  }
  else if (qos_hdr_format == PS_DPM_QOS_FORMAT_6_BYTE)
  {
    qos_6_byte_hdr = (ps_dpm_qos_hdr_type*)qos_hdr;
    qos_handle     = qos_6_byte_hdr->qos_handle;
    version        = qos_6_byte_hdr->version;
    flags          = qos_6_byte_hdr->flags; 
  }
  else 
  {
    LOG_MSG_ERROR_1("Invalid QOS Format %d", qos_hdr_format);
  }

  /* Replace qos handle with flow_ptr if valid */
  if (0 == qmi_qos_get_flow_ptr_from_handle(qos_handle, &flow_ptr ))
  {
    LOG_MSG_ERROR_1("Flow handle 0x%x passed in is invalid, forwarding onto default flow", qos_handle);
    flow_ptr = NULL;
  }
  else
  {
    LOG_MSG_DATA_PATH_INFO_2("Replacing qos handle 0x%x with flow ptr 0x%x", 
                              qos_hdr->qos_handle, 
                              flow_ptr);
  }

  /*-------------------------------------------------------------------------
    Update flow in QoS header based on follwing steps
      1. If flow in QoS header is valid, go to step 2. Else set it to
         default flow of Um iface
      2. If it is optimized path, fetch assoc flow and go to step 3. Else, exit
      3. If assoc flow is valid, update flow in QoS header to assoc flow. Else,
         set flow in QoS header to default flow of Um iface
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (!PS_FLOW_IS_VALID(flow_ptr))
  {
    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
  }
#ifdef  FEATURE_EPC_HANDOFF
  else
  {
    if (is_opt)
    {
      assoc_flow_ptr = PS_FLOWI_GET_ASSOC_PS_FLOW(flow_ptr);
      if (PS_FLOW_IS_VALID(assoc_flow_ptr))
      {
        flow_ptr = assoc_flow_ptr;
      }
      else
      {
        flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
      }
    }
  }
#endif

  /*-------------------------------------------------------------------------
    Always forward on default if flow is not ACTIVATED
  -------------------------------------------------------------------------*/
  if (FLOW_ACTIVATED != PS_FLOW_GET_STATE(flow_ptr))
  {
    /* Resume flow if it is SUSPENDED, this is needed for NW init QOS on eHRPD*/
    if (PS_FLOW_IS_NW_INITIATED(flow_ptr) &&
        PS_FLOW_GET_STATE(flow_ptr) == FLOW_SUSPENDED)
    {
      LOG_MSG_INFO2_1("Resuming SUSPENDED NW init QOS flow 0x%x", flow_ptr);
      return_val =  ps_flow_ioctl(flow_ptr,
                                  PS_FLOW_IOCTL_QOS_RESUME,
                                  NULL,
                                  &ps_errno);
      if (return_val != 0)
      {
        LOG_MSG_ERROR_1("Error while trying to resume flow %d", ps_errno);
      }
    }
    flow_ptr = PS_IFACEI_GET_DEFAULT_FLOW(um_iface_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Update Tx meta info with routing cache and QoS filter result
  -------------------------------------------------------------------------*/
  PS_TX_META_SET_ROUTING_CACHE(tx_meta_info_ptr, um_iface_ptr);
  PS_TX_META_SET_FILTER_RESULT(tx_meta_info_ptr,
                               IP_FLTR_CLIENT_QOS_OUTPUT,
                               flow_ptr);
  PS_TX_META_SET_TX_FLAGS(tx_meta_info_ptr, 0);

  /*-------------------------------------------------------------------------
    Handle DoS. Use the IP_ID from the packet as the DOS ack handle
  -------------------------------------------------------------------------*/
  if (flags & ((uint8) 0x1))
  {
    if (IP_V4 == ip_ver)
    {
      (void) dsm_extract(pkt_ptr,
                         PS_DPM_OPT_V4_ID_OFFSET,
                         &ip_id,
                         PS_DPM_OPT_V4_ID_LEN);
      PS_TX_META_SET_TX_FLAGS(tx_meta_info_ptr, MSG_FAST_EXPEDITE);
      PS_TX_META_SET_DOS_ACK_HANDLE(tx_meta_info_ptr, ip_id);

      LOG_MSG_DATA_PATH_INFO_1("ps_dpm_process_qos_hdr():"
                               "Set DoS Ack handle to %d", ip_id);
    }
    else
    {
      LOG_MSG_DATA_PATH_INFO_0("ps_dpm_process_qos_hdr(): "
                                "DOS not enabled for IPv6");
    }
  }
  if (NULL != flow_ptr_ret)
  {
    *flow_ptr_ret = flow_ptr;
  }

  return;
} /* ps_dpm_process_qos_hdr() */


/*===========================================================================
FUNCTION RMNET_META_SMI_RM_TX_DATA

DESCRIPTION
  This function is called by the bound lan llc instance to transmit a framed
  packet over the RmNet interface.

PARAMETERS
  item_head_ptr -  message (in dsm chain) to be transmitted
  user_data     -  user data passed to lan_llc_start(),
                   contains rmnet_meta_sm info ptr

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void ps_dpmi_rm_tx_data
(
  dsm_item_type   ** dsm_item_ptr_ptr,
  void             * user_data_ptr
)
{
  ps_dpm_rm_info_type  * dpm_rm_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  dpm_rm_info_ptr = ps_dpm_get_dpm_rm_info_by_handle((int32) user_data_ptr);
  if (NULL == dpm_rm_info_ptr)
  {
    dsm_free_packet(dsm_item_ptr_ptr);    
    PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
    return;
  }

  #ifndef TEST_FRAMEWORK
    sio_transmit(dpm_rm_info_ptr->sio_stream_id, *dsm_item_ptr_ptr);
  #else
    #error code not present
#endif /* TEST_FRAMEWORK */

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);

  return;
} /* ps_dpmi_rm_tx_data() */


/*===========================================================================
FUNCTION PS_DPM_MBIM_IP_MODE_TX_CMD

DESCRIPTION
  This function would execute mbim filters on every incoming pkts on rm 
  iface and would then send the data on downlink to sio path

PARAMETERS
  iface_ptr     -  The interface on which to transmit the packet
  pkt_chain_ptr -  message (in dsm chain) to be transmitted
  client_info   -  user data passed that contains rmnet_meta_sm info ptr

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
static int ps_dpm_mbim_ip_mode_tx_cmd
(
  ps_iface_type      * iface_ptr,
  dsm_item_type     ** dsm_item_ptr_ptr,
  ps_meta_info_type  * meta_info_ptr,
  void               * client_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Execute MBIM filters for each packet if installed   
  -----------------------------------------------------------------------*/    
  if (PS_IFACEI_NUM_FILTERS(iface_ptr, IP_FLTR_CLIENT_MBIM) > 0)
  { 
    if (PS_IFACE_IPFLTR_NOMATCH == ps_iface_ipfltr_mbim_execute(iface_ptr, 
                                                               IP_FLTR_CLIENT_MBIM,
                                                               *dsm_item_ptr_ptr))
    {
      LOG_MSG_INFO3_0("MBIM Filtering applied, Filter didnt match");
      /*-----------------------------------------------------------------------
        powersave_restrictive == TRUE : 
         Whitelist filter, Send the packet further up if filter match else 
         drop the packet    
      -----------------------------------------------------------------------*/    
      if (iface_ptr->powersave_restrictive == TRUE)
      {
        PS_META_INFO_FREE(&meta_info_ptr);    
        dsm_free_packet(dsm_item_ptr_ptr);
        return -1;
      }
    }
    else  //filter matched.
    {
      LOG_MSG_INFO3_0("MBIM Filtering applied, Filter matched");
      /*-----------------------------------------------------------------------
        powersave_restrictive == FALSE : 
         Blacklist filter, Drop the packet if filter match else send up the 
         packet further up in data path       
      -----------------------------------------------------------------------*/    
      if (iface_ptr->powersave_restrictive == FALSE)
      { 
        PS_META_INFO_FREE(&meta_info_ptr);   
        dsm_free_packet(dsm_item_ptr_ptr);
        return -1;
      }  
    }
  }
   
  /*-------------------------------------------------------------------------
    Free meta info and transmit packet.
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE(&meta_info_ptr);

  ps_dpmi_rm_tx_data(dsm_item_ptr_ptr, client_info);
  
  return 0;
} /* ps_dpm_mbim_ip_mode_tx_cmd() */

/*===========================================================================
FUNCTION ps_dpm_ip_mode_tx_cmd

DESCRIPTION
  This function is used to transmit IP packets on downlink towards sio path

PARAMETERS
  iface_ptr     -  The interface on which to transmit the packet
  pkt_chain_ptr -  message (in dsm chain) to be transmitted
  meta_info_ptr -  meta info associated with the packet
  client_info   -  user data passed that contains rmnet_meta_sm info ptr

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
int ps_dpm_ip_mode_tx_cmd
(
  ps_iface_type      * iface_ptr,
  dsm_item_type     ** dsm_item_ptr_ptr,
  ps_meta_info_type  * meta_info_ptr,
  void               * client_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Free meta info and transmit packet.
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE(&meta_info_ptr);

  ps_dpmi_rm_tx_data(dsm_item_ptr_ptr, client_info);

  return 0;
} /* ps_dpm_ip_mode_tx_cmd() */

int ps_dpm_powersave_mode_tx_cmd
(
  ps_iface_type      * iface_ptr,
  dsm_item_type     ** pkt_ref_ptr,
  ps_meta_info_type  * meta_info_ptr,
  void               * client_info
)
{
  ps_iface_type               * base_um_iface_ptr = NULL;
  ps_iface_type               * bridge_iface_ptr = NULL;
  ps_rx_meta_info_type        * rx_mi_ptr = NULL;
  ip_pkt_info_type            * ip_pkt_info = NULL;
  errno_enum_type               ps_errno;
  int                           ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
 /* ------------------------------------------------------------------------
   Execute powersave filters on downlink packet in legacy path. 
   If there is a match send it for upperlayers. Exit powersave mode if auto 
   exit mode is enabled.

   If there is no match  drop the paket. If first packet drop indication is not
   called already send an indication for fast dormancy algorithm to start.
  -------------------------------------------------------------------------*/
  do
  {
    if( PS_IFACE_GET_POWERSAVE_FILTERING_MODE(iface_ptr) )
    {
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      bridge_iface_ptr = ps_iface_bridge_iface( iface_ptr );
      if( ps_rx_pkt_info_generate(pkt_ref_ptr, &rx_mi_ptr, &ps_errno) == -1 )
      {
        dsm_free_packet(pkt_ref_ptr);
          
        IFACE_INC_INSTANCE_STATS(iface_ptr, pkts_dropped_tx, 1);
        if( PS_IFACE_IS_VALID(bridge_iface_ptr))
        {
          #ifdef FEATURE_DATA_PS_464XLAT
            base_um_iface_ptr =  
                PS_DPM_GET_BASE_IFACE( bridge_iface_ptr );
          #else
            base_um_iface_ptr =  
                PS_IFACE_GET_BASE_IFACE( bridge_iface_ptr );
          #endif /* FEATURE_DATA_PS_464XLAT */
          
          if( NULL != base_um_iface_ptr )
          {
            IFACE_INC_INSTANCE_STATS( base_um_iface_ptr, pkts_dropped_rx, 1 );
          }
        }
        
        if( NULL != rx_mi_ptr )
        {
          ps_rx_meta_info_free(&rx_mi_ptr);
        }
  
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        
        LOG_MSG_ERROR_0("ps_dpm_powersave_mode_tx_cmd(): Not able to generate"
                        " rx_mi_ptr info"); 
        ret_val = -1;
        break;
      }
  
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      
      ip_pkt_info = &rx_mi_ptr->pkt_info;

      if( PS_IFACE_IPFLTR_NOMATCH == 
          ps_iface_ipfltr_execute( iface_ptr, IP_FLTR_CLIENT_POWERSAVE, 
                                   PS_IFACE_IPFLTR_SUBSET_ID_DEFAULT, 
                                   ip_pkt_info) )
      {
        PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
        /*-----------------------------------------------------------------------
         drop packet and send indication to run the fast dormancy delay timer
         algorithm
        -----------------------------------------------------------------------*/
        IFACE_INC_INSTANCE_STATS(iface_ptr, pkts_dropped_tx, 1);
  
        if( PS_IFACE_IS_VALID(bridge_iface_ptr))
        {
          #ifdef FEATURE_DATA_PS_464XLAT
            base_um_iface_ptr =  
                PS_DPM_GET_BASE_IFACE( bridge_iface_ptr );
          #else
            base_um_iface_ptr =  
                PS_IFACE_GET_BASE_IFACE( bridge_iface_ptr );
          #endif /* FEATURE_DATA_PS_464XLAT */
          if( NULL != base_um_iface_ptr )
          { 
            IFACE_INC_INSTANCE_STATS( base_um_iface_ptr, pkts_dropped_rx, 1 );
          }
        }
        if(!ps_iface_is_first_powersave_packet_drop_reported())
        {
        ps_iface_set_first_powersave_packet_drop_reported(TRUE);
          ps_sys_fast_dormancy_pkt_dropped_ind(iface_ptr);
        } 
          
        dsm_free_packet(pkt_ref_ptr);
        ps_rx_meta_info_free(&rx_mi_ptr);
        
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        break;
      }
      else
      {
        if( PS_IFACE_GET_POWERSAVE_FILTERING_AUTO_EXIT_MODE(iface_ptr) )
        {
          if( -1 == ps_iface_set_powersave_filtering_mode( iface_ptr,
                                                           FALSE,
                                                          (sint15*)&ps_errno) )
          {
            LOG_MSG_ERROR_0( "ps_dpm_powersave_mode_tx_cmd:"
                             " powersave filtering mode change failed ");
           }
        }
        ps_rx_meta_info_free(&rx_mi_ptr);
        ps_dpmi_rm_tx_data(pkt_ref_ptr, client_info);
      }
    }
  }while(0);

  /*-------------------------------------------------------------------------
    Free meta info
  -------------------------------------------------------------------------*/
  PS_META_INFO_FREE(&meta_info_ptr);

  return ret_val;
}/* ps_dpm_powersave_mode_tx_cmd() */
 
static boolean ps_dpmi_dl_sig_hdlr
(
  ps_sig_enum_type      sig,
  void                * user_data_ptr
)
{
  dsm_item_type                * dsm_item_ptr;
  ps_phys_link_type            * ps_phys_link_ptr;
  ps_dpm_um_info_type          * dpm_um_info_ptr;
  ps_dpm_bearer_info_type      * bearer_ptr;
  int32                          dpm_um_bearer_handle;
  int32                          dpm_um_handle;
  int32                          dpm_bearer_handle;
  boolean                        retval = FALSE;
  ps_rx_meta_info_type         * meta_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Fetch bearer info using user data
  -------------------------------------------------------------------------*/
  dpm_um_bearer_handle = (int32) user_data_ptr;
  dpm_um_handle        = (0x0000FFFF & dpm_um_bearer_handle);
  dpm_bearer_handle    = (0x0000FFFF & (dpm_um_bearer_handle >> 16));

  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info_by_handle(dpm_um_handle);
    if (NULL == dpm_um_info_ptr)
    {
      retval = FALSE;
      break;
    }

    bearer_ptr =
      ps_dpmi_get_dpm_bearer_info_by_handle(dpm_um_info_ptr, dpm_bearer_handle);
    if (NULL == bearer_ptr)
    {
      retval = FALSE;
      break;
    }

    /*-----------------------------------------------------------------------
      Dequeue pkt from HW to PS WM and send it up to framework
    -----------------------------------------------------------------------*/
    dsm_item_ptr = (dsm_item_type *) dsm_dequeue(&(bearer_ptr->hw_to_ps_wm));
    if (NULL == dsm_item_ptr)
    {
      retval = TRUE;
      break;
    }

    ps_phys_link_ptr = bearer_ptr->ps_phys_link_ptr;

    /*-----------------------------------------------------------------------
      Generate pkt meta info and set packet logged by HW flag to true therefore legacy path 
      will not log the packet on UM iface again.
    -----------------------------------------------------------------------*/
    PS_RX_META_INFO_GET(meta_info_ptr);

    if ( NULL != meta_info_ptr &&
         TRUE == dpm_um_info_ptr->is_bridged_with_hw )
    {
      meta_info_ptr->is_pkt_logged = TRUE;
    }
    
    PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);

    (void) ps_phys_link_input(ps_phys_link_ptr, &dsm_item_ptr, meta_info_ptr);

    return FALSE;
  } while(0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);

  return retval;
} /* ps_dpmi_dl_sig_hdlr() */


static void ps_dpmi_hw_to_ps_non_empty_cback
(
  dsm_watermark_type  * dsm_wm_ptr,
  void                * user_data_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_SET_EXT1_SIGNAL((ps_sig_enum_type) user_data_ptr);

  return;
} /* ps_dpmi_hw_to_ps_non_empty_cback() */


static void ps_dpmi_setup_hw_to_ps_wm
(
  ps_dpm_bearer_info_type  * bearer_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    1. Initiatlize the queue
    2. Set WM levels
    3. Set non empty func ptr
    4. Reset WM counters
  -------------------------------------------------------------------------*/

  dsm_queue_init(&(bearer_ptr->hw_to_ps_wm),
                 PS_DPM_HW_PS_WM_DNE,
                 &(bearer_ptr->hw_to_ps_wm_q));

  dsm_set_low_wm(&(bearer_ptr->hw_to_ps_wm), PS_DPM_HW_PS_WM_LO);
  dsm_set_hi_wm(&(bearer_ptr->hw_to_ps_wm), PS_DPM_HW_PS_WM_HI);
  dsm_set_dne(&(bearer_ptr->hw_to_ps_wm), PS_DPM_HW_PS_WM_DNE);
  dsm_set_dne_q_cnt( &(bearer_ptr->hw_to_ps_wm), PS_DPM_HW_PS_WM_DNE_PKT_COUNT );

  bearer_ptr->hw_to_ps_wm.non_empty_func_ptr =
    ps_dpmi_hw_to_ps_non_empty_cback;
  bearer_ptr->hw_to_ps_wm.non_empty_func_data =
    (void *) (bearer_ptr->dpm_dl_sig);

  ps_dpmi_reset_wm_counters(&(bearer_ptr->hw_to_ps_wm));

} /* ps_dpmi_setup_hw_to_ps_wm() */

static uint8 ps_dpmi_generate_uid
(
  void
)
{
  uint8   uid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1("ps_dpm_last_uid_generated %d", ps_dpm_last_uid_generated);
  
  for (uid = ps_dpm_last_uid_generated + 1; uid < PS_DPM_MAX_UID; uid++)
  {
    if (FALSE == ps_dpm_uids_inuse[uid])
    {
      break;
    }    
  }
  
  if (PS_DPM_MAX_UID == uid)
  {
    for (uid = 0; uid <= ps_dpm_last_uid_generated; uid++)
    {
      if (FALSE == ps_dpm_uids_inuse[uid])
      {
        break;
      }    
    }  
  }
  
  if (ps_dpm_last_uid_generated == uid)
  {
    LOG_MSG_ERROR_1("ps_dpmi_generate_uid() : ALL UIDS are in use"
                    "last in use was %d", ps_dpm_last_uid_generated);
    ASSERT(0);
  }
  
  ps_dpm_uids_inuse[uid] = TRUE;
  /*-----------------------------------------------------------------------
    Set the ps_dpm_last_uid_generated so that next uid is fetched after
    ps_dpm_last_uid_generated position
  -----------------------------------------------------------------------*/   
  ps_dpm_last_uid_generated = uid;
  
  LOG_MSG_INFO1_1("ps_dpmi_generate_uid(): UID %d", uid);

  return uid;
} /* ps_dpmi_generate_uid() */

static void ps_dpmi_reset_uid
(
  uint8   uid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_DPM_MAX_UID <= uid)
  {
    LOG_MSG_INFO1_1("ps_dpmi_reset_uid(): Invalid uid %d", uid);
    return;
  }

  ps_dpm_uids_inuse[uid] = FALSE;
  return;
} /* ps_dpmi_reset_uid() */


static void ps_dpmi_um_get_bridge_status
(
  ps_dpm_um_info_type  * dpm_um_info_ptr,
  boolean              * is_v4_bridged_ptr,
  boolean              * is_v6_bridged_ptr
)
{
  ps_iface_type  * bridge_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *is_v4_bridged_ptr = FALSE;
  *is_v6_bridged_ptr = FALSE;

  /*-----------------------------------------------------------------------
    Use following algorithm to check whether v4 Rm or v6 Rm or both are bridged
      1. Get base iface of the bridge iface of Rm iface
      2. Check if it is not NULL and if it is same as Um iface

    Using above algorithm ensures that Rm is in fact in a call with Um. If
    any other algorithm is used, following scenario may not be handled
    correctly
      1. Both APN X and Y are configured as dual IP
      2. Tethered device makes a IPv4 call to APN X on SIO stream ID 5
      3. Tethered device makes a IPv6 call to APN Y on SIO stream ID 5

    When APN X is torn down, assuming that Rm is bridged to APN X for both
    v4 and v6 will have unwanted side effects
  -----------------------------------------------------------------------*/
  if (NULL != dpm_um_info_ptr->rm_info_ptr &&
      NULL != dpm_um_info_ptr->rm_info_ptr->v4_iface_ptr)
  {
    bridge_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE
                       (
                         dpm_um_info_ptr->rm_info_ptr->v4_iface_ptr
                       );
    if (NULL != bridge_iface_ptr)
    {    
      #ifdef FEATURE_DATA_PS_464XLAT
        bridge_iface_ptr = PS_DPM_GET_BASE_IFACE(bridge_iface_ptr);
      #else
        bridge_iface_ptr = PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr);
      #endif /* FEATURE_DATA_PS_464XLAT */

      if (bridge_iface_ptr == dpm_um_info_ptr->v4_iface_ptr)
      {
        *is_v4_bridged_ptr = TRUE;
      }
    }
  }

  if (NULL != dpm_um_info_ptr->rm_info_ptr &&
      NULL != dpm_um_info_ptr->rm_info_ptr->v6_iface_ptr)
  {
    bridge_iface_ptr = PS_IFACEI_GET_BRIDGE_IFACE
                       (
                         dpm_um_info_ptr->rm_info_ptr->v6_iface_ptr
                       );

    if (NULL != bridge_iface_ptr)
    {                        
      #ifdef FEATURE_DATA_PS_464XLAT
        bridge_iface_ptr = PS_DPM_GET_BASE_IFACE(bridge_iface_ptr);
      #else
        bridge_iface_ptr = PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr);
      #endif /* FEATURE_DATA_PS_464XLAT */

      if (bridge_iface_ptr == dpm_um_info_ptr->v6_iface_ptr)
      {
        *is_v6_bridged_ptr = TRUE;
      }
    }
  }

  LOG_MSG_INFO1_3("ps_dpmi_um_get_bridge_status(): DPM Um entry 0x%p "
                  "is_v4_bridged %d is_v6_bridged %d",
                  dpm_um_info_ptr, *is_v4_bridged_ptr, *is_v6_bridged_ptr);
  return;
} /* ps_dpmi_um_get_bridge_status() */


INLINE void ps_dpmi_rm_wait_for_cback
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) dcc_wait((rex_sigs_type) (1 << DCC_DPM_RM_WAIT_SIGNAL));

  return;
} /* ps_dpmi_rm_wait_for_cback() */

INLINE void ps_dpmi_um_wait_for_cback
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  (void) dsi_wait(DS_DPM_WAIT_SIGNAL);
  rex_clr_sigs(dsi_get_ds_tcb(), DS_DPM_WAIT_SIGNAL);
  return;
} /* ps_dpmi_um_wait_for_cback() */

static boolean ps_dpmi_config_dpl_with_hw
(
  uint8                      uid,
  ps_dpm_um_info_type      * dpm_um_info_ptr
)
{
  uint16                dpl_pkt_len_v4 = 0;
  uint16                dpl_pkt_len_v6 = 0;
  int                   ret_val;
  ps_iface_type       * iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (PS_IFACE_IS_VALID(dpm_um_info_ptr->v4_iface_ptr))
    {
    dpl_pkt_len_v4 =
        (uint16) DPL_GET_NETWORK_RX_SNAPLEN(dpm_um_info_ptr->v4_iface_ptr,
                                            DPL_IID_NETPROT_IP);
    }

    if (PS_IFACE_IS_VALID(dpm_um_info_ptr->v6_iface_ptr))
    {
      dpl_pkt_len_v6 =
        (uint16) DPL_GET_NETWORK_RX_SNAPLEN(dpm_um_info_ptr->v6_iface_ptr,
                                            DPL_IID_NETPROT_IP);
    }

  if (dpm_um_info_ptr->v4_iface_ptr != NULL)
  {
    iface_ptr = dpm_um_info_ptr->v4_iface_ptr;
  }
  else if (dpm_um_info_ptr->v6_iface_ptr != NULL)
  {
    iface_ptr = dpm_um_info_ptr->v6_iface_ptr;
  }   
  else
  {
    LOG_MSG_ERROR_1("ps_dpmi_config_dpl_with_ipa(): "
                    "No iface associated with dpm um entry 0x%p",
                    dpm_um_info_ptr);
    return -1;                        
  }

  ret_val = ps_dpm_hw_config_dpl(uid, 
                                 dpl_pkt_len_v4, 
                                 dpl_pkt_len_v6, 
                                 dpm_um_info_ptr->handle,
                                 iface_ptr);
  if (0 != ret_val)
    {
    LOG_MSG_ERROR_2("ps_dpmi_config_dpl_with_hw(): "
                    "Couldn't config DPL on uid %d with hw, err %d",
                    uid, ret_val);
    }

  return ret_val;
} /* ps_dpmi_config_dpl_with_hw() */

 
static int32 ps_dpmi_bridge_bearer_with_hw
(
  ps_dpm_um_info_type      * dpm_um_info_ptr,
  ps_dpm_bearer_info_type  * bearer_ptr
)
{
  int32                  ret_val = -1;
  sio_port_id_type       sio_port_id = SIO_PORT_NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (NULL != dpm_um_info_ptr->rm_info_ptr)
    {          
      sio_port_id = dpm_um_info_ptr->rm_info_ptr->sio_port_id;
    }
    
    /*-----------------------------------------------------------------------
      Bridge bearer with Hardware
    -----------------------------------------------------------------------*/
    ret_val = ps_dpm_hw_bridge_bearer(bearer_ptr->uid, sio_port_id);    

    if (0 != ret_val)
    {
      LOG_MSG_ERROR_1("ps_dpmi_bridge_bearer_with_hw(): "
                      "Couldn't bridge bearer with hardware, err%d", ret_val);
      break;
    }
    bearer_ptr->is_bridged = TRUE;
    LOG_MSG_INFO1_1("ps_dpmi_bridge_bearer_with_hw(): Bridged bearer 0x%p",
                    bearer_ptr);

    /*-----------------------------------------------------------------------
      Enable DPL on Um
    -----------------------------------------------------------------------*/
    (void) ps_dpmi_config_dpl_with_hw(bearer_ptr->uid,
                                       dpm_um_info_ptr);

    ret_val = 0;
  } while (0);

  return ret_val;
} /* ps_dpmi_bridge_bearer_with_hw() */

static void ps_dpmi_suspend_bearer_with_hw
(
  uint8   uid
)
{
  int    ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ret_val = ps_dpm_hw_suspend_bearer(uid);
  if (0 != ret_val)
  {
    LOG_MSG_ERROR_2("ps_dpmi_suspend_bearer_with_hw(): Couldn't suspend "
                    "bearer with uid %d with hardware, err %d",
                     uid, ret_val);
    ASSERT(0);
  }

  return;
} /* ps_dpmi_suspend_bearer_with_hw() */

static void ps_dpmi_activate_bearer_with_hw
(
  uint8   uid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_dpm_hw_activate_bearer(uid);

} /* ps_dpmi_activate_bearer_with_hw() */
  
static int32 ps_dpmi_reg_bearer_with_hw
(
  ps_dpm_um_info_type      * dpm_um_info_ptr,
  ps_dpm_bearer_info_type  * bearer_ptr,
  boolean                    is_rereg
)
{
  int32               dl_sig_index;
  int32               dpm_um_bearer_handle;
  sio_stream_id_type  sio_stream_id = SIO_NO_STREAM_ID;
#ifndef TEST_FRAMEWORK
  int                 ret_val;
#endif /* TEST_FRAMEWORK */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (FALSE == is_rereg)
    {
      /*---------------------------------------------------------------------
        Setup DL sig for bearer
      ---------------------------------------------------------------------*/
      for (dl_sig_index = 0;
           dl_sig_index < PS_DPM_DL_MAX_SIGNALS;
           dl_sig_index++)
      {
        if (FALSE == ps_dpm_dl_sig_in_use_tbl[dl_sig_index])
        {
          ps_dpm_dl_sig_in_use_tbl[dl_sig_index] = TRUE;
          bearer_ptr->dpm_dl_sig =
            (ps_sig_enum_type) (PS_DPM_DL_SIGNAL_0 + dl_sig_index);

          /*-----------------------------------------------------------------
            Generate user data by combining DPM Um handle and bearer handle
          -----------------------------------------------------------------*/
          dpm_um_bearer_handle = (0xFFFF0000 & (bearer_ptr->handle << 16)) |
                                 (0x0000FFFF & dpm_um_info_ptr->handle);
          ps_set_sig_handler(bearer_ptr->dpm_dl_sig,
                             ps_dpmi_dl_sig_hdlr,
                             (void *) dpm_um_bearer_handle);
          //MSR TODO is this needed every time? When is this disabled?
          ps_enable_sig(bearer_ptr->dpm_dl_sig);

          break;
        }
      }

      if (PS_DPM_DL_MAX_SIGNALS <= dl_sig_index)
      {
        LOG_MSG_ERROR_0("ps_dpmi_reg_bearer_with_hw(): Couldn't find DL sig");
        break;
      }

      LOG_MSG_INFO1_1("ps_dpmi_reg_bearer_with_hw(): Assigned DL sig %d",
                      bearer_ptr->dpm_dl_sig);

      /*-----------------------------------------------------------------------
        Setup HW to PS WM
      -----------------------------------------------------------------------*/
      ps_dpmi_setup_hw_to_ps_wm(bearer_ptr);
    }

    /*-----------------------------------------------------------------------
      If Um is in tethered call, fetch stream ID
    -----------------------------------------------------------------------*/
    if (NULL != dpm_um_info_ptr->rm_info_ptr)
    {
      sio_stream_id = dpm_um_info_ptr->rm_info_ptr->sio_stream_id;
    }

#ifndef TEST_FRAMEWORK
    ret_val = ps_dpm_hw_reg_bearer(bearer_ptr->uid,
		                   bearer_ptr->l2_to_ps_wm_ptr,
		                   &(bearer_ptr->hw_to_ps_wm),
                                   sio_stream_id,
                                   dpm_um_info_ptr->rat);
    if (0 != ret_val)
    {
      LOG_MSG_ERROR_1("ps_dpmi_reg_bearer_with_hw(): "
                      "Couldn't reg bearer with hw, err %d", ret_val);
      break;
    }
#endif /* TEST_FRAMEWORK */

    LOG_MSG_INFO1_2("ps_dpmi_reg_bearer_with_hw(): Registered bearer 0x%p "
                    "with SIO stream ID %d", bearer_ptr, sio_stream_id);

    if (TRUE == dpm_um_info_ptr->is_bridged_with_hw)
    {
      if (TRUE == dpm_um_info_ptr->is_hw_bridging_allowed)
      {
        if (0 != ps_dpmi_bridge_bearer_with_hw(dpm_um_info_ptr,
                                                          bearer_ptr))
        {
          LOG_MSG_ERROR_0("ps_dpmi_reg_bearer_with_hw(): "
                          "Couldn't bridge bearer with hw");
          break;
        }
      }
      else
      {
        dpm_um_info_ptr->is_bridged_with_hw = FALSE;
        LOG_MSG_ERROR_0("ps_dpmi_reg_bearer_with_hw() : Hardware bridging not"
                        " allowed. Disable the NV 67343 to enable briding");
      }
    }

    return 0;
  } while (0);
  /*-------------------------------------------------------------------------
    Error handling must be performed by the caller
  -------------------------------------------------------------------------*/
  return -1;
} /* ps_dpmi_reg_bearer_with_hw() */


static void ps_dpmi_dereg_bearer_with_hw
(
  ps_dpm_bearer_info_type  *bearer_ptr,
  sio_port_id_type          sio_port_id,
  boolean                   is_data_path_bridged
)
{
  int   ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (NULL == bearer_ptr)
  {
    ASSERT(0);
    return;
  }

  /*-----------------------------------------------------------------------
    1. De-register bearer with Hardware
    2. Reset DL signal used for Hw-PS WM
    3. Reset Hw-PS WM. This must happen after de-registering bearer with Hw,
       otherwise, HW might crash accessing invalid WM
  -----------------------------------------------------------------------*/

  ret_val = ps_dpm_hw_dereg_bearer
                       (bearer_ptr->uid, sio_port_id, is_data_path_bridged );
  if (0 != ret_val)
  {
    LOG_MSG_ERROR_1("ps_dpmi_dereg_bearer_with_hw(): Couldn't dereg bearer "
                    "with hw, err %d", ret_val);
    ASSERT(0);
    return;
  }
  bearer_ptr->is_bridged = FALSE;
  if (bearer_ptr->dpm_dl_sig < PS_MAX_SIGNALS && 
      bearer_ptr->dpm_dl_sig >= PS_DPM_DL_SIGNAL_0)
  {
    if (PS_DPM_DL_MAX_SIGNALS >  (bearer_ptr->dpm_dl_sig - PS_DPM_DL_SIGNAL_0))
    {
      ps_dpm_dl_sig_in_use_tbl[bearer_ptr->dpm_dl_sig - PS_DPM_DL_SIGNAL_0] =
                                                                         FALSE;
    }
    ps_clr_ext_signal(bearer_ptr->dpm_dl_sig);
    /*-------------------------------------------------------------------------
        Reset the signal to default handler.This way we avoid a race condiotion
        where we will end up in a loop when downlink packets are being 
        processed in dl_opt handler and bearer is deleted(we will always return
        TRUE and that signal is set again resulting in a loop). Setting it to 
        NULL might result in NULL pointer access By using default sig handler 
        we will process the signal at max once. 
      -----------------------------------------------------------------------*/
    ps_reset_sig_handler( bearer_ptr->dpm_dl_sig );

    LOG_MSG_INFO1_1("ps_dpmi_dereg_bearer_with_hw(): Reclaimed DL sig %d",
                    bearer_ptr->dpm_dl_sig);
  }

  if (NULL != bearer_ptr->hw_to_ps_wm.non_empty_func_ptr)
  {
    bearer_ptr->hw_to_ps_wm.non_empty_func_ptr = NULL;
    bearer_ptr->hw_to_ps_wm.non_empty_func_data = NULL;

    dsm_queue_destroy(&(bearer_ptr->hw_to_ps_wm));
  }

  return;
} /* ps_dpmi_dereg_bearer_with_hw() */


static int32 ps_dpmi_bridge_all_bearers_with_hw
(
  ps_dpm_um_info_type  * dpm_um_info_ptr
)
{
  ps_dpm_bearer_info_type   * bearer_ptr;
  int32                       bearer_index;
  sio_port_id_type            sio_port_id = SIO_PORT_NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /*-----------------------------------------------------------------------
      Bridge the bearers if following conditions are met
        1. Um is in a tethered call AND
        2. Um is not already bridged AND
        3. Rm is not in ETHERNET mode OR Rm is in ETHERNET mode but
           ethernet address is configured

      If Rm is in ETHETNET mode, bearer must be bridged only after
      Rm receives ethernet address of the tethered device
    ----------------------------------------------------------------------*/
    if (NULL != dpm_um_info_ptr->rm_info_ptr &&
        FALSE == dpm_um_info_ptr->is_bridged_with_hw &&
        (PS_SYS_TECH_3GPP2 != ps_sys_get_tech_from_rat(dpm_um_info_ptr->rat)) &&
        (RMNET_ETHERNET_MODE != dpm_um_info_ptr->rm_info_ptr->link_prot ||
         TRUE == dpm_um_info_ptr->rm_info_ptr->is_eth_addr_configured))
    {
      dpm_um_info_ptr->is_bridged_with_hw = TRUE;

      for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
      {
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr)
        {
          if (-1 == ps_dpmi_reg_bearer_with_hw(dpm_um_info_ptr,
                                                bearer_ptr,
                                                TRUE))
          {
            LOG_MSG_ERROR_0("ps_dpmi_bridge_all_bearers_with_hw(): "
                            "Couldn't bridge bearer with hardware");
            break;
          }
        }
      }

      if (PS_DPM_MAX_BEARERS > bearer_index)
      {
        break;
      }
    }

    return 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Unbridge all the bridged bearers
  -------------------------------------------------------------------------*/
  for (; bearer_index >= 0; bearer_index--)
  {
    bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
    if (NULL != bearer_ptr &&
        TRUE == dpm_um_info_ptr->is_bridged_with_hw)
    {
      ps_dpm_hw_dereg_dpl(bearer_ptr->uid);
      
      if (NULL != dpm_um_info_ptr->rm_info_ptr)
    {
        sio_port_id = dpm_um_info_ptr->rm_info_ptr->sio_port_id;                           
    }

      ps_dpm_hw_unbridge_bearer(bearer_ptr->uid, sio_port_id);        
      bearer_ptr->is_bridged = FALSE;
    }
  }

  dpm_um_info_ptr->is_bridged_with_hw = FALSE;
  
  return -1;
} /* ps_dpmi_bridge_all_bearers_with_hw() */


static int32 ps_dpmi_unbridge_all_bearers_with_hw
(
  ps_dpm_um_info_type  * dpm_um_info_ptr
)
{
  ps_dpm_bearer_info_type   * bearer_ptr;
  int32                       bearer_index;
  boolean                     is_v4_bridged;
  boolean                     is_v6_bridged;
  sio_port_id_type            sio_port_id = SIO_PORT_NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_dpmi_um_get_bridge_status(dpm_um_info_ptr, &is_v4_bridged, &is_v6_bridged);

  /*-------------------------------------------------------------------------
    Unbridge all the bearers with Hardware if all of the following conditions are
    met
      1. Number of bearers is not zero
      2. Bearers are currently bridged to Hardware
      3. v4 Um is currently not bridged to any Rm
      4. v6 Um is currently not bridged to any Rm

    #3 and #4 are very important as bearer must not be unbridged if one of
    v4 or v6 Um is still in a tethered call
  -------------------------------------------------------------------------*/
  if (0 != dpm_um_info_ptr->num_bearer &&
      TRUE == dpm_um_info_ptr->is_bridged_with_hw)
  {
    if (FALSE == is_v4_bridged && FALSE == is_v6_bridged)
    {
      for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
      {
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr)
        {
          ps_dpm_hw_dereg_dpl(bearer_ptr->uid);
          
          if (NULL != dpm_um_info_ptr->rm_info_ptr)
          { 
            sio_port_id = dpm_um_info_ptr->rm_info_ptr->sio_port_id;                           
          }    
          
          ps_dpm_hw_unbridge_bearer(bearer_ptr->uid, sio_port_id);   
          bearer_ptr->is_bridged = FALSE;
        }
      }

      dpm_um_info_ptr->is_bridged_with_hw = FALSE;
    }
  }

  return 0;
} /* ps_dpmi_unbridge_all_bearers_with_hw() */

static int32 ps_dpmi_delete_bearer
(
  ps_dpm_um_info_type      * dpm_um_info_ptr,
  ps_dpm_bearer_info_type  * bearer_ptr
)
{
  sio_port_id_type            sio_port_id = SIO_PORT_NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    1. De-register bearer with Hardware
    2. Reset UID
    3. Free bearer
  -------------------------------------------------------------------------*/

  if (NULL != dpm_um_info_ptr->rm_info_ptr)
  {
    sio_port_id = dpm_um_info_ptr->rm_info_ptr->sio_port_id;
  }
  ps_dpmi_dereg_bearer_with_hw( bearer_ptr,
                                sio_port_id,
                                dpm_um_info_ptr->is_bridged_with_hw);
  ps_dpmi_reset_uid(bearer_ptr->uid);
  ps_dpmi_free_bearer_info(dpm_um_info_ptr, bearer_ptr);

  return 0;
} /* ps_dpmi_delete_bearer() */


static int32 ps_dpmi_add_bearer
(
  ps_dpm_um_info_type              * dpm_um_info_ptr,
  ps_dpm_um_reg_bearer_info_type   * um_reg_bearer_info_ptr
)
{
  ps_dpm_bearer_info_type  * bearer_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    bearer_ptr = ps_dpmi_alloc_bearer_info(dpm_um_info_ptr);
    if (NULL == bearer_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpmi_add_bearer(): Couldn't alloc bearer");
      break;
    }

    /*-----------------------------------------------------------------------
      Generate UID. UID would be between 0 and 31
    -----------------------------------------------------------------------*/
    bearer_ptr->uid = ps_dpmi_generate_uid();
    if (PS_DPM_MAX_UID <= bearer_ptr->uid)
    {
      LOG_MSG_ERROR_0("ps_dpmi_add_bearer(): Couldn't find UID");
      break;
    }

    /*-----------------------------------------------------------------------
      Fill up the bearer info and register bearer with Hardware
    -----------------------------------------------------------------------*/
    bearer_ptr->ps_phys_link_ptr = um_reg_bearer_info_ptr->ps_phys_link_ptr;
    bearer_ptr->l2_to_ps_wm_ptr  = um_reg_bearer_info_ptr->l2_to_ps_wm_ptr;

    if (-1 == ps_dpmi_reg_bearer_with_hw(dpm_um_info_ptr, bearer_ptr, FALSE))
    {
      LOG_MSG_ERROR_0("ps_dpmi_add_bearer(): Couldn't reg bearer with hw");
      break;
    }

    return 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Delete the bearer
  -------------------------------------------------------------------------*/
  if (NULL != bearer_ptr)
  {
    ps_dpmi_delete_bearer(dpm_um_info_ptr, bearer_ptr);
  }

  return -1;
} /* ps_dpmi_add_bearer() */

static int32 ps_dpmi_add_tlb_bearer
(
  ps_dpm_um_info_type                  * dpm_um_info_ptr,
  ps_dpm_um_reg_tlb_bearer_info_type   * um_reg_tlb_bearer_info_ptr
)
{
  ps_dpm_bearer_info_type  * bearer_ptr = NULL;
  int                        ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    bearer_ptr = ps_dpmi_alloc_bearer_info(dpm_um_info_ptr);
    if (NULL == bearer_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpmi_add_tlb_bearer(): Couldn't alloc bearer");
      break;
    }

    /*-----------------------------------------------------------------------
      Generate UID. UID would be between 0 and 31
    -----------------------------------------------------------------------*/
    bearer_ptr->uid = ps_dpmi_generate_uid();
    if (PS_DPM_MAX_UID <= bearer_ptr->uid)
    {
      LOG_MSG_ERROR_0("ps_dpmi_add_tlb_bearer(): Couldn't find UID");
      break;
    }

    /*-----------------------------------------------------------------------
      Fill up the bearer info and register bearer with hardware
    -----------------------------------------------------------------------*/
    bearer_ptr->ps_phys_link_ptr = um_reg_tlb_bearer_info_ptr->ps_phys_link_ptr;
    bearer_ptr->l2_to_ps_wm_ptr  = um_reg_tlb_bearer_info_ptr->l2_to_ps_wm_ptr;

    ret_val = ps_dpm_hw_reg_bearer(bearer_ptr->uid,
                                   um_reg_tlb_bearer_info_ptr->l2_to_ps_wm_ptr,
                                   um_reg_tlb_bearer_info_ptr->hw_to_ps_wm_ptr,
                                   SIO_NO_STREAM_ID,
                                   dpm_um_info_ptr->rat);                                 
    if (0 != ret_val)
    {
      LOG_MSG_ERROR_0("ps_dpmi_add_tlb_bearer(): Couldn't reg bearer with hw");
      break;
    }   

    LOG_MSG_INFO1_1("ps_dpmi_add_tlb_bearer(): Registered bearer 0x%p "
                    "with SIO NO stream ID", bearer_ptr);

    return 0;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Delete the bearer
  -------------------------------------------------------------------------*/
  if (NULL != bearer_ptr)
  {
    ps_dpmi_delete_bearer(dpm_um_info_ptr, bearer_ptr);
  }

  return -1;
} /* ps_dpmi_add_tlb_bearer() */


static void ps_dpmi_rm_dereg_internal
(
  ps_dpm_rm_info_type  * dpm_rm_info_ptr,
  boolean                dereg_v4,
  boolean                dereg_v6
)
{
  ps_iface_type    * um_iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_3("ps_dpmi_rm_dereg_internal(): DPM Rm entry 0x%p "
                  "dereg_v4 %d dereg_v6 %d",
                  dpm_rm_info_ptr, dereg_v4, dereg_v6);

  /*-------------------------------------------------------------------------
    Do v4 specific clean up
  -------------------------------------------------------------------------*/
  if (TRUE == dereg_v4 && NULL != dpm_rm_info_ptr->v4_iface_ptr)
  {
    /*-----------------------------------------------------------------------
      For attached pdn, since um doesnt dereg with dpm, the link
      dpm_rm_info_ptr->v4_um_info_ptr would be valid, however for
      non-attached scenario, it would be null. For non-attached scenario
      dereg of all bearers on pdn would result in dereg of qos hardware filters
      so rm neednot dereg the qos filters with hw in that case
    -----------------------------------------------------------------------*/    
    if (NULL != dpm_rm_info_ptr->v4_um_info_ptr)
    {
      um_iface_ptr = dpm_rm_info_ptr->v4_um_info_ptr->v4_iface_ptr;
    }

    /*---------------------------------------------------------------------
      Delete QOS HW Filters since RM is going away and HW would loose
      stream id info
    ---------------------------------------------------------------------*/    
    if (PS_IFACE_IS_VALID(um_iface_ptr) && 
        TRUE == um_iface_ptr->qos_fltr_offloaded_to_hw)
    {
      ps_iface_ipfltr_hw_delete_all_iface_filters (um_iface_ptr,
                                                         IP_FLTR_CLIENT_QOS_OUTPUT); 
    }

    /*-----------------------------------------------------------------------
      Deregister for iface/flow/phys link events
    -----------------------------------------------------------------------*/
    (void) ps_dpmi_dereg_iface_events(dpm_rm_info_ptr, IPV4_ADDR);
    (void) ps_dpmi_dereg_flow_events(dpm_rm_info_ptr, IPV4_ADDR);
    (void) ps_dpmi_dereg_phys_link_events(dpm_rm_info_ptr, IPV4_ADDR);

    /*-----------------------------------------------------------------------
      Clean up ethernet mode specific info
    -----------------------------------------------------------------------*/
    if (RMNET_ETHERNET_MODE == dpm_rm_info_ptr->link_prot)
    {
      PS_SYSTEM_HEAP_MEM_FREE(dpm_rm_info_ptr->v4_eth_hdr_ptr);
    }

    /*-----------------------------------------------------------------------
      Reset Rm. This must happen before ps_dpmi_unbridge_all_bearers_with_hw()
      as that function relies on bridge status to decide whether to unbridge
    -----------------------------------------------------------------------*/
    dpm_rm_info_ptr->v4_iface_ptr          = NULL;
    dpm_rm_info_ptr->v4_logical_iface_ptr  = NULL;
    dpm_rm_info_ptr->v4_phys_link_ptr      = NULL;

    /*-----------------------------------------------------------------------
      Unbridge bearers and reset Um info
    -----------------------------------------------------------------------*/
    if (NULL != dpm_rm_info_ptr->v4_um_info_ptr)
    {
      (void) ps_dpmi_unbridge_all_bearers_with_hw
             (
               dpm_rm_info_ptr->v4_um_info_ptr
             );

      /*-------------------------------------------------------------------
        Free Link(DPM UM -> DPM RM) if v4 um info is not same as v6 um info
        ptr. Wrt dual ip over single rmnet, following would happen
        1. v4 went down. 
        2. DPM RM -> v4 dpm um == DPM RM -> v6 dpm um   
        3. We don free link((DPM UM -> DPM RM)
        4. DPM RM -> v4 dpm um = NULL
 
        5. v6 iface down
        6. DPM RM ->  v4 dpm um != DPM RM -> v6 dpm um (see step 4)
        7. DPM RM -> v6 dpm um  -> DPM RM = NULL
        8.  DPM RM -> v6 dpm um = NULL        
        
        Wrt single ip call. it would be step (5-8)
        Wrt dual ip rmnet talking to two differnet pdn. It would be again
        ( 5 - 7) as their dpm um's are different
      -------------------------------------------------------------------*/              
      if (dpm_rm_info_ptr->v4_um_info_ptr != dpm_rm_info_ptr->v6_um_info_ptr)
      {
        dpm_rm_info_ptr->v4_um_info_ptr->rm_info_ptr = NULL;
      } 
      
      /*-------------------------------------------------------------------
        Since 3GPP2 doesn't register with DPM, clean up DPM Um. This is needed
        as DPM Um is only allocated when Rm iface calls ps_dpm_rm_reg().
      -------------------------------------------------------------------*/      
      if (PS_SYS_TECH_3GPP2 == 
          ps_sys_get_tech_from_rat(dpm_rm_info_ptr->v4_um_info_ptr->rat))
      {
        ps_dpmi_free_dpm_um_info(dpm_rm_info_ptr->v4_um_info_ptr);  
        
        /*
          CR 676312
          1.	Embms call is brought up
          2.	Embms registers bearer with dpm with both v4 iface and v6 iface same
          3.	Before RM registers, embms deregister the bearer with dpm
          4.	Rm get iface up ind, rm creates new enter but doesn’t find dpm um info, 
              it assumes its 3gpp2
          5.	Rm gets iface down ind
          6.	Rm deregisters with dpm but since we don’t have below check, 
              it results in double free            
        */
        if (dpm_rm_info_ptr->v4_um_info_ptr == dpm_rm_info_ptr->v6_um_info_ptr)
        {
          dpm_rm_info_ptr->v6_um_info_ptr = NULL;
        }         
      }

      dpm_rm_info_ptr->v4_um_info_ptr = NULL;
    }
  }

  /*-------------------------------------------------------------------------
    Do v6 specific clean up
  -------------------------------------------------------------------------*/
  if (TRUE == dereg_v6 && NULL != dpm_rm_info_ptr->v6_iface_ptr)
  {
    /*-----------------------------------------------------------------------
      For attached pdn, since um doesnt dereg with dpm, the link
      dpm_rm_info_ptr->v4_um_info_ptr would be valid, however for
      non-attached scenario, it would be null. For non-attached scenario
      dereg of all bearers on pdn would result in dereg of qos hw filters
      so rm neednot dereg the qos filters with hw in that case
    -----------------------------------------------------------------------*/  
    if (NULL != dpm_rm_info_ptr->v6_um_info_ptr)
    {
      um_iface_ptr = dpm_rm_info_ptr->v6_um_info_ptr->v6_iface_ptr;
    }

    /*---------------------------------------------------------------------
      Delete QOS HW Filters since RM is going away and HW would loose
      stream id info
    ---------------------------------------------------------------------*/    
    if (PS_IFACE_IS_VALID(um_iface_ptr) && 
        TRUE == um_iface_ptr->qos_fltr_offloaded_to_hw)
    {
      ps_iface_ipfltr_hw_delete_all_iface_filters(um_iface_ptr,
                                                  IP_FLTR_CLIENT_QOS_OUTPUT);                                                       
    }

    /*-----------------------------------------------------------------------
      Deregister for iface/flow/phys link events
    -----------------------------------------------------------------------*/
    (void) ps_dpmi_dereg_iface_events(dpm_rm_info_ptr, IPV6_ADDR);
    (void) ps_dpmi_dereg_flow_events(dpm_rm_info_ptr, IPV6_ADDR);
    (void) ps_dpmi_dereg_phys_link_events(dpm_rm_info_ptr, IPV6_ADDR);

    /*-----------------------------------------------------------------------
      Clean up ethernet mode specific info
    -----------------------------------------------------------------------*/
    if (RMNET_ETHERNET_MODE == dpm_rm_info_ptr->link_prot)
    {
      PS_SYSTEM_HEAP_MEM_FREE(dpm_rm_info_ptr->v6_eth_hdr_ptr);
    }

    /*-----------------------------------------------------------------------
      Reset Rm. This must happen before ps_dpmi_unbridge_all_bearers_with_hw()
      as that function relies on bridge status to decide whether to unbridge
    -----------------------------------------------------------------------*/
    dpm_rm_info_ptr->v6_iface_ptr          = NULL;
    dpm_rm_info_ptr->v6_logical_iface_ptr  = NULL;
    dpm_rm_info_ptr->v6_phys_link_ptr      = NULL;    

    /*-----------------------------------------------------------------------
      Unbridge bearers and reset Um info
    -----------------------------------------------------------------------*/
    if (NULL != dpm_rm_info_ptr->v6_um_info_ptr)
    {
      (void) ps_dpmi_unbridge_all_bearers_with_hw
             (
               dpm_rm_info_ptr->v6_um_info_ptr
             );


      /*-------------------------------------------------------------------
        Free Link(DPM UM -> DPM RM) if v4 um info is not same as v6 um info
        ptr. Wrt dual ip over single rmnet, following would happen
        1. v4 went down. 
        2. DPM RM -> v4 dpm um == DPM RM -> v6 dpm um   
        3. We don free link((DPM UM -> DPM RM)
        4. DPM RM -> v4 dpm um = NULL
 
        5. v6 iface down
        6. DPM RM ->  v4 dpm um != DPM RM -> v6 dpm um (see step 5)
        7. DPM RM -> v6 dpm um  -> DPM RM = NULL
        8.  DPM RM -> v6 dpm um = NULL        
        
        Wrt single ip call. it would be step (5-8)
        Wrt dual ip rmnet talking to two differnet pdn. It would be again
        ( 5 - 8) as their dpm um's are different
      -------------------------------------------------------------------*/                
      if (dpm_rm_info_ptr->v6_um_info_ptr != dpm_rm_info_ptr->v4_um_info_ptr)
      {
        dpm_rm_info_ptr->v6_um_info_ptr->rm_info_ptr = NULL;
      }
      
      /*-------------------------------------------------------------------
        Since 3GPP2 doesn't register with DPM, clean up DPM Um. This is needed
        as DPM Um is only allocated when Rm iface calls ps_dpm_rm_reg().
      -------------------------------------------------------------------*/               
      if (PS_SYS_TECH_3GPP2 == 
          ps_sys_get_tech_from_rat(dpm_rm_info_ptr->v6_um_info_ptr->rat))
      {
        ps_dpmi_free_dpm_um_info(dpm_rm_info_ptr->v6_um_info_ptr);  

        /*
          CR 676312
          1.	Embms call is brought up
          2.	Embms registers bearer with dpm with both v4 iface and v6 iface same
          3.	Before RM registers, embms deregister the bearer with dpm
          4.	Rm get iface up ind, rm creates new enter but doesn’t find dpm um info, 
              it assumes its 3gpp2
          5.	Rm gets iface down ind
          6.	Rm deregisters with dpm but since we don’t have below check, 
              it results in double free            
        */
        if (dpm_rm_info_ptr->v6_um_info_ptr == dpm_rm_info_ptr->v4_um_info_ptr)
        {
          dpm_rm_info_ptr->v4_um_info_ptr = NULL;
        }        
      }             

      dpm_rm_info_ptr->v6_um_info_ptr = NULL;
    }
    
    dpm_rm_info_ptr->v6_logical_iface_ptr  = NULL;
    
    /*-----------------------------------------------------------------------
      Remove ipv6 prefix rule
    -----------------------------------------------------------------------*/
    if (PS_IFACE_IPFLTR_INVALID_HANDLE !=
          dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl)
    {
      ps_iface_ipfltr_hw_delete_v6_prefix_hndl
      (
        dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl
      );

      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl =
        PS_IFACE_IPFLTR_INVALID_HANDLE;
    }
  }

  /*-------------------------------------------------------------------------
    Free up DPM Rm info if both v4 and v6 Rm ifaces are down
  -------------------------------------------------------------------------*/
  if (NULL == dpm_rm_info_ptr->v4_iface_ptr &&
      NULL == dpm_rm_info_ptr->v6_iface_ptr)
  {
    ps_dpmi_reset_sio_wm(dpm_rm_info_ptr);
    dsm_free_packet(&(dpm_rm_info_ptr->rx_pkt));
    ps_dpmi_free_dpm_rm_info(dpm_rm_info_ptr);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Set the signal so that data can be dequeued again. If Um is still
      flow controlled, then sig hdlr will reset the signal
    -----------------------------------------------------------------------*/
    PS_RM_SET_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);
  }

  return;
} /* ps_dpmi_rm_dereg_internal() */


static void ps_dpmi_call_um_ev_cback
(
  ps_sys_rat_ex_enum_type       rat,
  ps_iface_type               * v4_iface_ptr,
  ps_iface_type               * v6_iface_ptr,
  ps_phys_link_type           * phys_link_ptr,
  ps_dpm_um_ev_enum_type        um_ev,
  boolean                       result
)
{
  ps_dpm_um_ev_cback_info_type   cback_info;
  ps_sys_tech_enum_type          tech_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&cback_info, 0, sizeof(ps_dpm_um_ev_cback_info_type));

  cback_info.rat               = rat;
  cback_info.v4_iface_ptr      = v4_iface_ptr;
  cback_info.v6_iface_ptr      = v6_iface_ptr;
  cback_info.result            = result;
  cback_info.phys_link_ptr     = phys_link_ptr;
  cback_info.is_hw_accelerated = TRUE;

  tech_type = ps_sys_get_tech_from_rat(rat);
  if (NULL != ps_dpm_global_um_ev_cback[tech_type])
  {
    LOG_MSG_INFO1_3("ps_dpmi_call_um_ev_cback(): Calling call back for "
                    "tech %d ev %d result %d", tech_type, um_ev, result);

    ps_dpm_global_um_ev_cback[tech_type](um_ev, &cback_info);
  }

  return;
} /* ps_dpmi_call_um_ev_cback() */


static void ps_dpmi_um_reg_bearer_hdlr
(
  ps_dpm_um_reg_bearer_info_type  * um_reg_bearer_info_ptr
)
{
  ps_dpm_um_info_type   * dpm_um_info_ptr;
  ps_iface_type         * um_iface_ptr;
  boolean                 is_dpm_um_info_allocated = FALSE;
  boolean                 result                   = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_5("ps_dpmi_um_reg_bearer_hdlr(): rat %d v4 Um iface 0x%p "
                  "v6 Um iface 0x%p phys link 0x%p l2_to_ps WM 0x%p",
                  um_reg_bearer_info_ptr->rat,
                  um_reg_bearer_info_ptr->v4_iface_ptr,
                  um_reg_bearer_info_ptr->v6_iface_ptr,
                  um_reg_bearer_info_ptr->ps_phys_link_ptr,
                  um_reg_bearer_info_ptr->l2_to_ps_wm_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Pick either v4 Um iface or v6 Um iface to use as a key for searching
      DPM entry, as both are maintained in same DPM entry for a dual-IP PDN
    -----------------------------------------------------------------------*/
    if (NULL != um_reg_bearer_info_ptr->v4_iface_ptr)
    {
      um_iface_ptr = um_reg_bearer_info_ptr->v4_iface_ptr;
    }
    else
    {
      um_iface_ptr = um_reg_bearer_info_ptr->v6_iface_ptr;
    }

    /*-----------------------------------------------------------------------
      Check if DPM Um info is already present. If not, allocate a new
      DPM Um info
    -----------------------------------------------------------------------*/
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      dpm_um_info_ptr = ps_dpmi_alloc_dpm_um_info();
      if (NULL == dpm_um_info_ptr)
      {
        break;
      }

      is_dpm_um_info_allocated = TRUE;

      /*---------------------------------------------------------------------
        Update DPM Um info with non-bearer specific info. This is needed only
        when DPM Um info is created as it is same for all subsequent bearers
      ----------------------------------------------------------------------*/
      dpm_um_info_ptr->rat          = um_reg_bearer_info_ptr->rat;
      dpm_um_info_ptr->v4_iface_ptr = um_reg_bearer_info_ptr->v4_iface_ptr;
      dpm_um_info_ptr->v6_iface_ptr = um_reg_bearer_info_ptr->v6_iface_ptr;

      /*---------------------------------------------------------------------
        Update iface stat adjustment callback to ps_dpm_get_iface_stat_adjustment
        which would update stats from hardware
      ----------------------------------------------------------------------*/      
      if (PS_IFACE_IS_VALID(dpm_um_info_ptr->v4_iface_ptr))
      {
        dpm_um_info_ptr->v4_iface_ptr->ps_iface_stat_get_adjustment_f_ptr =
          ps_dpm_get_iface_stat_adjustment;        
      }
      if (PS_IFACE_IS_VALID(dpm_um_info_ptr->v6_iface_ptr))
      {
        dpm_um_info_ptr->v6_iface_ptr->ps_iface_stat_get_adjustment_f_ptr =
         ps_dpm_get_iface_stat_adjustment;
      }
        
      if (disable_hw_bridge_mode != PS_DPM_DISABLE_HW_PATH_NONE)
      {
        dpm_um_info_ptr->is_hw_bridging_allowed = FALSE;
      }
      else
      {
        dpm_um_info_ptr->is_hw_bridging_allowed = TRUE;
      }
    }
    else
    {
      if (0 == dpm_um_info_ptr->num_bearer)
      {
        DATA_ERR_FATAL("ps_dpmi_um_reg_bearer_hdlr() : DPM um info was already allocated and number of  bearers is zero. Asserting...");
      }
      
      /*---------------------------------------------------------------------
        Since DPM entry already exists, make sure that mode handler is passing
        same v4 and v6 Um ifaces as that are there in the DPM entry
      ---------------------------------------------------------------------*/
      if (dpm_um_info_ptr->v4_iface_ptr !=
            um_reg_bearer_info_ptr->v4_iface_ptr ||
          dpm_um_info_ptr->v6_iface_ptr !=
            um_reg_bearer_info_ptr->v6_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpmi_um_reg_bearer_hdlr(): "
                        "DPM entry doesn't match passed arg, "
                        "DPM v4 Um iface 0x%p DPM v6 Um iface 0x%p",
                        dpm_um_info_ptr->v4_iface_ptr,
                        dpm_um_info_ptr->v6_iface_ptr);
        break;
      }
      
      dpm_um_info_ptr->rat = um_reg_bearer_info_ptr->rat;
    }

    /*-----------------------------------------------------------------------
      Add bearer to DPM
    -----------------------------------------------------------------------*/
    if (-1 == ps_dpmi_add_bearer(dpm_um_info_ptr, um_reg_bearer_info_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_um_reg_bearer_hdlr(): Couldn't add bearer");
      break;
    }

    result = TRUE;
  } while (0);

  /*-------------------------------------------------------------------------
    Perform clean up in case of failure
  -------------------------------------------------------------------------*/
  if (FALSE == result && TRUE == is_dpm_um_info_allocated)
  {
    ps_dpmi_free_dpm_um_info(dpm_um_info_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Send callback to mode handler
  -------------------------------------------------------------------------*/
  ps_dpmi_call_um_ev_cback(um_reg_bearer_info_ptr->rat,
                           um_reg_bearer_info_ptr->v4_iface_ptr,
                           um_reg_bearer_info_ptr->v6_iface_ptr,
                           um_reg_bearer_info_ptr->ps_phys_link_ptr,
                           PS_DPM_UM_EV_BEARER_REG_COMPLETE,
                           result);
  
  return;
} /* ps_dpmi_um_reg_bearer_hdlr() */


static void ps_dpmi_um_reg_tlb_bearer_hdlr
(
  ps_dpm_um_reg_tlb_bearer_info_type  * um_reg_tlb_bearer_info_ptr
)
{
  ps_dpm_um_info_type   * dpm_um_info_ptr;
  ps_iface_type         * um_iface_ptr;
  boolean                 is_dpm_um_info_allocated = FALSE;
  boolean                 result                   = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_6("ps_dpmi_um_reg_tlb_bearer_hdlr(): rat %d v4 Um iface 0x%p "
                  "v6 Um iface 0x%p phys link 0x%p l2_to_ps WM 0x%p, "
                  "hw_to_ps WM 0x%x",
                  um_reg_tlb_bearer_info_ptr->rat,
                  um_reg_tlb_bearer_info_ptr->v4_iface_ptr,
                  um_reg_tlb_bearer_info_ptr->v6_iface_ptr,
                  um_reg_tlb_bearer_info_ptr->ps_phys_link_ptr,
                  um_reg_tlb_bearer_info_ptr->l2_to_ps_wm_ptr,
                  um_reg_tlb_bearer_info_ptr->hw_to_ps_wm_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Pick either v4 Um iface or v6 Um iface to use as a key for searching
      DPM entry, as both are maintained in same DPM entry for a dual-IP PDN
    -----------------------------------------------------------------------*/
    if (NULL != um_reg_tlb_bearer_info_ptr->v4_iface_ptr)
    {
      um_iface_ptr = um_reg_tlb_bearer_info_ptr->v4_iface_ptr;
    }
    else
    {
      um_iface_ptr = um_reg_tlb_bearer_info_ptr->v6_iface_ptr;
    }

    /*-----------------------------------------------------------------------
      Check if DPM Um info is already present. If not, allocate a new
      DPM Um info
    -----------------------------------------------------------------------*/
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      dpm_um_info_ptr = ps_dpmi_alloc_dpm_um_info();
      if (NULL == dpm_um_info_ptr)
      {
        break;
      }

      is_dpm_um_info_allocated = TRUE;

      /*---------------------------------------------------------------------
        Update DPM Um info with non-bearer specific info. This is needed only
        when DPM Um info is created as it is same for all subsequent bearers
      ----------------------------------------------------------------------*/
      dpm_um_info_ptr->rat          = um_reg_tlb_bearer_info_ptr->rat;
      dpm_um_info_ptr->v4_iface_ptr = um_reg_tlb_bearer_info_ptr->v4_iface_ptr;
      dpm_um_info_ptr->v6_iface_ptr = um_reg_tlb_bearer_info_ptr->v6_iface_ptr;
    }
    else
    {
      /*---------------------------------------------------------------------
        Since DPM entry already exists, make sure that mode handler is passing
        same v4 and v6 Um ifaces as that are there in the DPM entry
      ---------------------------------------------------------------------*/
      if (dpm_um_info_ptr->v4_iface_ptr !=
            um_reg_tlb_bearer_info_ptr->v4_iface_ptr ||
          dpm_um_info_ptr->v6_iface_ptr !=
            um_reg_tlb_bearer_info_ptr->v6_iface_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpmi_um_reg_tlb_bearer_hdlr(): "
                        "DPM entry doesn't match passed arg, "
                        "DPM v4 Um iface 0x%p DPM v6 Um iface 0x%p",
                        dpm_um_info_ptr->v4_iface_ptr,
                        dpm_um_info_ptr->v6_iface_ptr);
        break;
      }
    }

    /*-----------------------------------------------------------------------
      Add bearer to DPM
    -----------------------------------------------------------------------*/
    if (-1 == ps_dpmi_add_tlb_bearer(dpm_um_info_ptr, um_reg_tlb_bearer_info_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_um_reg_tlb_bearer_hdlr(): Couldn't add bearer");
      break;
    }

    result = TRUE;
  } while (0);

  /*-------------------------------------------------------------------------
    Perform clean up in case of failure
  -------------------------------------------------------------------------*/
  if (FALSE == result && TRUE == is_dpm_um_info_allocated)
  {
    ps_dpmi_free_dpm_um_info(dpm_um_info_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Send callback to mode handler
  -------------------------------------------------------------------------*/
  ps_dpmi_call_um_ev_cback(um_reg_tlb_bearer_info_ptr->rat,
                           um_reg_tlb_bearer_info_ptr->v4_iface_ptr,
                           um_reg_tlb_bearer_info_ptr->v6_iface_ptr,
                           um_reg_tlb_bearer_info_ptr->ps_phys_link_ptr,
                           PS_DPM_UM_EV_BEARER_REG_COMPLETE,
                           result);
  
  return;
} /* ps_dpmi_um_reg_tlb_bearer_hdlr() */


static void ps_dpmi_um_reg_iface_hdlr
(
  ps_dpm_um_reg_iface_info_type  * um_reg_iface_info_ptr
)
{
  ps_dpm_um_info_type  * dpm_um_info_ptr;
  boolean                result = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_dpmi_um_reg_iface_hdlr():"
                  "registered Um iface 0x%p new Um iface 0x%p",
                  um_reg_iface_info_ptr->registered_iface_ptr,
                  um_reg_iface_info_ptr->iface_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Fetch DPM Um info using registered Um iface
    -----------------------------------------------------------------------*/
    dpm_um_info_ptr =
      ps_dpmi_get_dpm_um_info(um_reg_iface_info_ptr->registered_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      break;
    }

    if (NULL != dpm_um_info_ptr->v4_iface_ptr &&
        NULL != dpm_um_info_ptr->v6_iface_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpmi_um_reg_iface_hdlr(): Both v4 and v6 Um ifaces "
                      "are already regisered");
      break;
    }

    /*-----------------------------------------------------------------------
      Add the new Um iface to DPM Um info
    -----------------------------------------------------------------------*/
    if (dpm_um_info_ptr->v4_iface_ptr ==
          um_reg_iface_info_ptr->registered_iface_ptr)
    {
      dpm_um_info_ptr->v6_iface_ptr = um_reg_iface_info_ptr->iface_ptr;
      /*---------------------------------------------------------------------
        Update iface stat adjustment callback to ps_dpm_get_iface_stat_adjustment
        which would update stats from hardware
      ----------------------------------------------------------------------*/ 
      if (PS_IFACE_IS_VALID(dpm_um_info_ptr->v6_iface_ptr))
      {
        dpm_um_info_ptr->v6_iface_ptr->ps_iface_stat_get_adjustment_f_ptr =
         ps_dpm_get_iface_stat_adjustment;
      }
    }
    else
    {
      dpm_um_info_ptr->v4_iface_ptr = um_reg_iface_info_ptr->iface_ptr;
      /*---------------------------------------------------------------------
        Update iface stat adjustment callback to ps_dpm_get_iface_stat_adjustment
        which would update stats from hardware
      ----------------------------------------------------------------------*/      
      if (PS_IFACE_IS_VALID(dpm_um_info_ptr->v4_iface_ptr))
      {
        dpm_um_info_ptr->v4_iface_ptr->ps_iface_stat_get_adjustment_f_ptr =
          ps_dpm_get_iface_stat_adjustment;        
      }
    }

    result = TRUE;
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  if (NULL != dpm_um_info_ptr)
  {
    /*-------------------------------------------------------------------------
      Send callback to mode handler
    -------------------------------------------------------------------------*/
    //MSR TODO what if DPM entry is not found?
    ps_dpmi_call_um_ev_cback(dpm_um_info_ptr->rat,
                             dpm_um_info_ptr->v4_iface_ptr,
                             dpm_um_info_ptr->v6_iface_ptr,
                             NULL,
                             PS_DPM_UM_EV_IFACE_REG_COMPLETE,
                             result);
  }
  
  return;
} /* ps_dpmi_um_reg_iface_hdlr() */


static void ps_dpmi_um_rereg_bearer_hdlr
(
  ps_dpm_um_rereg_bearer_info_type   * um_rereg_bearer_info_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;
  ps_dpm_bearer_info_type  * bearer_ptr;
  ps_iface_type            * um_iface_ptr;
  boolean                    result = FALSE;
  ps_dpm_hw_stats_info_type  stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_5("ps_dpmi_um_rereg_bearer_hdlr(): rat %d v4 Um iface 0x%p "
                  "v6 Um iface 0x%p phys link 0x%p l2_to_ps WM 0x%p",
                  um_rereg_bearer_info_ptr->rat,
                  um_rereg_bearer_info_ptr->v4_iface_ptr,
                  um_rereg_bearer_info_ptr->v6_iface_ptr,
                  um_rereg_bearer_info_ptr->ps_phys_link_ptr,
                  um_rereg_bearer_info_ptr->l2_to_ps_wm_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Fetch DPM Um info using either v4 Um iface or v6 Um iface as a key for
      searching DPM entry, as both are maintained in same DPM entry for a
      dual-IP PDN
    -----------------------------------------------------------------------*/
    if (NULL != um_rereg_bearer_info_ptr->v4_iface_ptr)
    {
      um_iface_ptr = um_rereg_bearer_info_ptr->v4_iface_ptr;
    }
    else
    {
      um_iface_ptr = um_rereg_bearer_info_ptr->v6_iface_ptr;
    }

    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Fetch bearer info from DPM Um info
    -----------------------------------------------------------------------*/
    bearer_ptr =
      ps_dpmi_get_bearer_info(dpm_um_info_ptr,
                              um_rereg_bearer_info_ptr->ps_phys_link_ptr);
    if (NULL == bearer_ptr)
    {
      break;
    }

    /*-------------------------------------------------------------------------
    Update bearer stats onto the new iface so that they aren't lost during irat
    scenarios
    -----------------------------------------------------------------------*/
    memset(&stats, 0x0, sizeof(stats));  
    
    if( 0 == ps_dpm_hw_get_iface_stat_adjustment( bearer_ptr->uid,
                                                  um_iface_ptr, 
                                                  &stats ))
    {
      if( IFACE_IPV4_ADDR_FAMILY == ps_iface_get_addr_family(um_iface_ptr))
      {
        um_iface_ptr->iface_i_stats.bytes_rx += 
          stats.dl_stats.num_ipv4_bytes;

        um_iface_ptr->iface_i_stats.pkts_rx += 
          stats.dl_stats.num_ipv4_pkts;
      }
      else
      {
        um_iface_ptr->iface_i_stats.bytes_rx += 
         stats.dl_stats.num_ipv6_bytes;

        um_iface_ptr->iface_i_stats.pkts_rx += 
          stats.dl_stats.num_ipv6_pkts;
      }
    }
    /*-----------------------------------------------------------------------
      Update bearer info and register with hardware again
    -----------------------------------------------------------------------*/
    dpm_um_info_ptr->rat        = um_rereg_bearer_info_ptr->rat;
    bearer_ptr->l2_to_ps_wm_ptr = um_rereg_bearer_info_ptr->l2_to_ps_wm_ptr;

    if (-1 == ps_dpmi_reg_bearer_with_hw(dpm_um_info_ptr, bearer_ptr, TRUE))
    {
      /*---------------------------------------------------------------------
        ASSERT in case of failure here as, the only way to recover is to
        maintain old RAT and l2_to_ps WM and re-register with hardware and 
        since registration with hardware failed here, not sure if registration 
        with old l2_to_ps WM would go through
      ---------------------------------------------------------------------*/
      ASSERT(0);
      break;
    }

    result = TRUE;
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Send callback to mode handler
  -------------------------------------------------------------------------*/
  ps_dpmi_call_um_ev_cback(um_rereg_bearer_info_ptr->rat,
                           um_rereg_bearer_info_ptr->v4_iface_ptr,
                           um_rereg_bearer_info_ptr->v6_iface_ptr,
                           um_rereg_bearer_info_ptr->ps_phys_link_ptr,
                           PS_DPM_UM_EV_BEARER_REREG_COMPLETE,
                           result);
  
  return;
} /* ps_dpmi_um_rereg_bearer_hdlr() */


static void ps_dpmi_rm_reg_iface_hdlr
(
  ps_dpm_rm_reg_iface_info_type   * rm_reg_iface_info_ptr
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr = NULL;
  ps_dpm_um_info_type             * dpm_um_info_ptr;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                   * bridge_iface_ptr;
  llc_frm_packed_mode_zero_type  ** eth_hdr_ptr_ptr = NULL;
  ps_iface_addr_family_type         addr_family = IP_ADDR_INVALID;
  boolean                           is_dpm_um_info_allocated = FALSE;
  boolean                           is_dpm_rm_info_allocated = FALSE;
  int                               ret_val;   
  int16                             ps_errno;    
  uint16                            dpl_pkt_len = 0;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_4("ps_dpmi_rm_reg_iface_hdlr(): SIO stream id %d "
                  "Rm iface 0x%p SIO Rx WM 0x%p SIO Tx WM 0x%p",
                  rm_reg_iface_info_ptr->sio_stream_id,
                  rm_reg_iface_info_ptr->rm_iface_ptr,
                  rm_reg_iface_info_ptr->sio_rx_wm_ptr,
                  rm_reg_iface_info_ptr->sio_tx_wm_ptr);
                  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Check if DPM Rm info is already present. If not, allocate a new
      DPM Rm info
    -----------------------------------------------------------------------*/
    dpm_rm_info_ptr =
      ps_dpmi_get_dpm_rm_info(rm_reg_iface_info_ptr->sio_stream_id);
    if (NULL == dpm_rm_info_ptr)
    {
      dpm_rm_info_ptr = ps_dpmi_alloc_dpm_rm_info();
      if (NULL == dpm_rm_info_ptr)
      {
        break;
      }

      is_dpm_rm_info_allocated = TRUE;
    }

    addr_family = ps_iface_get_addr_family(rm_reg_iface_info_ptr->rm_iface_ptr);

    /*-----------------------------------------------------------------------
      Fetch base iface of the bridge iface in order to find corresponding
      DPM Um info
    -----------------------------------------------------------------------*/
    bridge_iface_ptr =
      PS_IFACEI_GET_BRIDGE_IFACE(rm_reg_iface_info_ptr->rm_iface_ptr);
    if (!PS_IFACE_IS_VALID(bridge_iface_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_rm_reg_iface_hdlr(): Invalid bridge iface");
      break;
    }

    #ifdef FEATURE_DATA_PS_464XLAT
      um_iface_ptr = PS_DPM_GET_BASE_IFACE(bridge_iface_ptr);
    #else
      um_iface_ptr = PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr);
    #endif /* FEATURE_DATA_PS_464XLAT */

    if (!PS_IFACE_IS_VALID(um_iface_ptr))
    {
      LOG_MSG_ERROR_1("ps_dpmi_rm_reg_iface_hdlr(): Invalid um_iface_ptr 0x%p",
                      um_iface_ptr);
      break;
    }

    /*-----------------------------------------------------------------------
      Find corresponding DPM Um info using Um iface
    -----------------------------------------------------------------------*/
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      /*---------------------------------------------------------------------
        Since 3GPP2 mode handler doesn't register with DPM, this case is
        not invalid. So allocate a new DPM Um info
      ---------------------------------------------------------------------*/
      dpm_um_info_ptr = ps_dpmi_alloc_dpm_um_info();
      if (NULL == dpm_um_info_ptr)
      {
        break;
      }

      is_dpm_um_info_allocated = TRUE;

      /*---------------------------------------------------------------------
        Initialize DPM Um info
      ---------------------------------------------------------------------*/
      dpm_um_info_ptr->rat = PS_SYS_RAT_EX_3GPP2_1X;

      if (IPV4_ADDR == addr_family)
      {
        dpm_um_info_ptr->v4_iface_ptr         = um_iface_ptr;
      }
      else
      {
        dpm_um_info_ptr->v6_iface_ptr         = um_iface_ptr;        
      }
    }

    /*-----------------------------------------------------------------------
      If DPM Rm info is newly allocated, update common Rm info
    -----------------------------------------------------------------------*/
    if (TRUE == is_dpm_rm_info_allocated)
    {
      if (NULL != dpm_um_info_ptr->rm_info_ptr)
      {
        LOG_MSG_ERROR_2("ps_dpmi_rm_reg_iface_hdlr() Two rmnet connecting to"
                        " same PDN. Aborting call", 
                        dpm_um_info_ptr, 
                        dpm_um_info_ptr->rm_info_ptr);
        break;
      }
      
      dpm_rm_info_ptr->sio_stream_id     = rm_reg_iface_info_ptr->sio_stream_id;
      dpm_rm_info_ptr->link_prot         = rm_reg_iface_info_ptr->link_prot;
      dpm_rm_info_ptr->data_format       = rm_reg_iface_info_ptr->data_format;
      dpm_rm_info_ptr->data_agg_protocol =
        rm_reg_iface_info_ptr->data_agg_protocol;
      dpm_rm_info_ptr->te_flow_control   = rm_reg_iface_info_ptr->te_flow_control;
      dpm_rm_info_ptr->qos_header_format = rm_reg_iface_info_ptr->qos_header_format;

      /* rm_iface_ptr is passed to find if the correspoding um_iface_ptr
         is clat capable to register the ul signal handlers */
      if (-1 == ps_dpmi_setup_sio_wm(dpm_rm_info_ptr,
                                     rm_reg_iface_info_ptr->sio_tx_wm_ptr,
                                     rm_reg_iface_info_ptr->sio_rx_wm_ptr,
                                     rm_reg_iface_info_ptr->rm_iface_ptr))
      {
        break;
      }
      
      ret_val = ps_iface_ioctl
               (
                 rm_reg_iface_info_ptr->rm_iface_ptr,
                 PS_IFACE_IOCTL_RM_GET_SIO_PORT,
                 &(dpm_rm_info_ptr->sio_port_id),
                 &ps_errno
               );
      if (0 != ret_val)
      {
        LOG_MSG_ERROR_0("ps_dpmi_rm_reg_iface_hdlr() :"
                        "Unable to retrieve sio port id");    
        break;                        
      }            
    }
    #ifdef FEATURE_DATA_PS_464XLAT
    else
    {
      ps_dpmi_set_ul_signal_handler(dpm_rm_info_ptr,
                                    rm_reg_iface_info_ptr->rm_iface_ptr);
    }

    /* Associcate the stream id with clat pdn context */
    if (um_iface_ptr->clat_pdn_cntxt != NULL)
    {
      ps_dpm_hw_clat_assoc_sio_stream
      (
        ((ps_dpm_clat_pdn_cntxt_info_type *)um_iface_ptr->clat_pdn_cntxt)->hw_pdn_handle,
        dpm_rm_info_ptr->sio_stream_id
      );
    }
    #endif /* FEATURE_DATA_PS_464XLAT */

    dpm_um_info_ptr->rm_info_ptr = dpm_rm_info_ptr;

    /*-----------------------------------------------------------------------
      Update DPM Rm info based on addr family
        1. Rm iface
        2. DPM Um info
        3. LAN LLC instance
        4. Install prefix routing rule for IPv6 Rm iface
        5. Register bearers with SIO stream ID and also bridge them
    -----------------------------------------------------------------------*/
    if (IPV4_ADDR == addr_family)
    {
      dpm_rm_info_ptr->v4_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v4_iface_ptr   = rm_reg_iface_info_ptr->rm_iface_ptr;
      dpm_rm_info_ptr->v4_logical_iface_ptr  = bridge_iface_ptr;
      
      dpm_rm_info_ptr->v4_phys_link_ptr  = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);
      
      eth_hdr_ptr_ptr                 = &(dpm_rm_info_ptr->v4_eth_hdr_ptr);

      dpm_rm_info_ptr->v4_lan_llc_instance =
        rm_reg_iface_info_ptr->lan_llc_instance;

      if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
      {
        break;
      }
    }
    else if (IPV6_ADDR == addr_family)
    {
      dpm_rm_info_ptr->v6_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v6_iface_ptr   = rm_reg_iface_info_ptr->rm_iface_ptr;
      dpm_rm_info_ptr->v6_logical_iface_ptr  = bridge_iface_ptr;
      
      dpm_rm_info_ptr->v6_phys_link_ptr  = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);
      
      eth_hdr_ptr_ptr                 = &(dpm_rm_info_ptr->v6_eth_hdr_ptr);

      dpm_rm_info_ptr->v6_lan_llc_instance =
        rm_reg_iface_info_ptr->lan_llc_instance;

      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl =
        ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr
        (
          um_iface_ptr,
          rm_reg_iface_info_ptr->sio_stream_id,
          PS_IFACE_IPFLTR_RESULT_UL_IPV6
        );
        
      LOG_MSG_INFO2_1("ps_dpmi_rm_reg_iface_hdlr() : "
                      "v6 prefix comp filter handle 0x%p",      
                      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl);    

      if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
      {
        break;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Special case for eMBMS
      ---------------------------------------------------------------------*/

      dpm_rm_info_ptr->v4_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v6_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v4_iface_ptr   = rm_reg_iface_info_ptr->rm_iface_ptr;
      dpm_rm_info_ptr->v6_iface_ptr   = rm_reg_iface_info_ptr->rm_iface_ptr;
      dpm_rm_info_ptr->v4_logical_iface_ptr  = bridge_iface_ptr;
      dpm_rm_info_ptr->v6_logical_iface_ptr  = bridge_iface_ptr;
      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl = PS_IFACE_IPFLTR_INVALID_HANDLE;

      if (RMNET_IP_MODE != rm_reg_iface_info_ptr->link_prot)
      {
        DATA_ERR_FATAL("ps_dpmi_rm_reg_iface_hdlr(): Ethernet mode is not supported for eMBMS");
        break;
      }

      if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
      {
        break;
      }
    }

    /*-----------------------------------------------------------------------
      Register for iface, and flow events on Um iface
    ---------------------------- ------------------------------------------*/
    (void) ps_dpmi_reg_iface_events(dpm_rm_info_ptr, addr_family);
    (void) ps_dpmi_reg_flow_events(dpm_rm_info_ptr, addr_family);
    (void) ps_dpmi_reg_phys_link_events(dpm_rm_info_ptr, addr_family);

    if (RMNET_ENABLE_DATA_AGG_MBIM == dpm_rm_info_ptr->data_agg_protocol ||
        RMNET_ENABLE_DATA_AGG_QC_NCM == dpm_rm_info_ptr->data_agg_protocol)
    {
      ps_iface_set_tx_function(rm_reg_iface_info_ptr->rm_iface_ptr,
                               ps_dpm_mbim_ip_mode_tx_cmd,
                               (void *) dpm_rm_info_ptr->handle);    
    }
    else if (RMNET_IP_MODE == dpm_rm_info_ptr->link_prot)
    {
      ps_iface_set_tx_function(rm_reg_iface_info_ptr->rm_iface_ptr,
                               ps_dpm_ip_mode_tx_cmd,
                               (void *) dpm_rm_info_ptr->handle);
    }
    else
    {
      if (NULL != eth_hdr_ptr_ptr)
      {
        PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(*eth_hdr_ptr_ptr, sizeof(llc_frm_packed_mode_zero_type),
                                        llc_frm_packed_mode_zero_type*);
        if (NULL == *eth_hdr_ptr_ptr)
        {
          break;
        }
      }
    }

    dpl_pkt_len =
      (uint16) DPL_GET_NETWORK_RX_SNAPLEN(rm_reg_iface_info_ptr->rm_iface_ptr,
                                          DPL_IID_NETPROT_IP); 
    /*---------------------------------------------------------------------
      Configure sio stream id with hardware if DPL is being handled by
      hardware
    ---------------------------------------------------------------------*/  
#ifndef TEST_FRAMEWORK
    ps_dpm_hw_config_dpl_sio
    (
       rm_reg_iface_info_ptr->sio_stream_id,
       rm_reg_iface_info_ptr->rm_iface_ptr->dpl_net_cb.recv_dpl_id.ifname,
       dpl_pkt_len
    );
#endif /* TEST_FRAMEWORK */

    /*---------------------------------------------------------------------
      Add QOS HW Filters because if Filters were installed before RM 
      came up, HW wouldnt have any information on stream id.
    ---------------------------------------------------------------------*/    
    ps_iface_ipfltr_hw_add_all_iface_filters (um_iface_ptr,
                                                  IP_FLTR_CLIENT_QOS_OUTPUT);

    /*-------------------------------------------------------------------
      Set the DPM status to success
    -------------------------------------------------------------------*/      
    ps_dpm_status_rm_reg_iface = 0;                                                  

    PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 

    /*-------------------------------------------------------------------
      Unblock DCC task since DPM is completed with registration
    -------------------------------------------------------------------*/  
    DCC_SET_SIGNAL(DCC_DPM_RM_WAIT_SIGNAL);  

    return;
  } while (0);

  /*-------------------------------------------------------------------
    Set the DPM status to failure
  -------------------------------------------------------------------*/ 
  ps_dpm_status_rm_reg_iface = -1;
  
  /*-------------------------------------------------------------------------
    Error handling

    Clean up DPM Rm info based on addr family

    If DPM Um info is allocated, free it blindly as it happens only for 3GPP2
    case
  -------------------------------------------------------------------------*/
#if 0
    //MSR TODO reset correctly based on addr family
    dpm_um_info_ptr->rm_info_ptr = dpm_rm_info_ptr;
#endif

  if (NULL != dpm_rm_info_ptr)
  {
    if (IPV4_ADDR == addr_family)
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, TRUE, FALSE);
    }
    else if (IPV6_ADDR == addr_family)
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, FALSE, TRUE);
    }
    else
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, TRUE, TRUE);
    }
  }

  if (TRUE == is_dpm_um_info_allocated)
  {
    ps_dpmi_free_dpm_um_info(dpm_um_info_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 
  
  /*-------------------------------------------------------------------
    Unblock DCC task since DPM is completed with registration
  -------------------------------------------------------------------*/  
  DCC_SET_SIGNAL(DCC_DPM_RM_WAIT_SIGNAL);  

  return;
} /* ps_dpmi_rm_reg_iface_hdlr() */

static void ps_dpmi_handoff_success_ev_hdlr
(
  ps_dpm_handoff_success_ev_info_type   * handoff_success_ev_info_ptr
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr = NULL;
  ps_dpm_um_info_type             * dpm_um_info_ptr;
  ps_iface_type                   * um_iface_ptr;
  ps_iface_type                      * bridge_iface_ptr = NULL; 
  ps_iface_addr_family_type         addr_family = IP_ADDR_INVALID;
  boolean                           is_dpm_um_info_allocated = FALSE;
  ps_iface_ioctl_data_path_info_type   data_path_info_ptr;
  sint15                               ps_err;
  uint8                                unbridge_mode_ref_cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_dpmi_handoff_success_ev_hdlr(): stream id %d iface 0x%p",
                  handoff_success_ev_info_ptr->sio_stream_id, 
                  handoff_success_ev_info_ptr->rm_iface_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                   
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section); 

  do
  {
    /*-----------------------------------------------------------------------
      Check if DPM Rm info is already present. If not, allocate a new
      DPM Rm info
    -----------------------------------------------------------------------*/
    dpm_rm_info_ptr = 
      ps_dpmi_get_dpm_rm_info(handoff_success_ev_info_ptr->sio_stream_id);
    if (NULL == dpm_rm_info_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpmi_handoff_success_ev_hdlr() : "
                      "Invalid dpm rm info ptr 0x%x", dpm_rm_info_ptr);
      break;
    }
    
    addr_family = 
      ps_iface_get_addr_family(handoff_success_ev_info_ptr->rm_iface_ptr);  
    
    /*-----------------------------------------------------------------------
      If v4_um_info_ptr or v6_um_info_ptr is not NULL then UM has not
      deregistered with us. So perform following
      1. Deregister the physlink events
      2. Unmap the DPM RM to DPM UM and vice versa
    -----------------------------------------------------------------------*/
    if (IPV4_ADDR == addr_family && NULL != dpm_rm_info_ptr->v4_um_info_ptr)
    {
      if (NULL != dpm_rm_info_ptr->v4_iface_ptr)
      {
        /*-----------------------------------------------------------------------
          Deregister for phys link events
        -----------------------------------------------------------------------*/
        (void) ps_dpmi_dereg_phys_link_events(dpm_rm_info_ptr, IPV4_ADDR);
         dpm_rm_info_ptr->v4_phys_link_ptr = NULL;
      }
      
      /*-----------------------------------------------------------------------
        If call is of type dual ip over single rmnet call then RM would have
        same ptr form dpm um v4 and dpm um v6. So if handoff happens for 
        v4 and v6 is still there then dpm shouldnt break the link else it
        would result in crash and vice versa.
      -----------------------------------------------------------------------*/      
      if (dpm_rm_info_ptr->v4_um_info_ptr != dpm_rm_info_ptr->v6_um_info_ptr)
      {
        LOG_MSG_INFO1_2("ps_dpmi_handoff_success_ev_hdlr() : "
                        "V4 DPM UM 0x%x no longer linked to DPM RM 0x%x",
                         dpm_rm_info_ptr->v4_um_info_ptr,
                         dpm_rm_info_ptr->v4_um_info_ptr->rm_info_ptr);      
        ps_dpmi_unbridge_all_bearers_with_hw(dpm_rm_info_ptr->v4_um_info_ptr);
        dpm_rm_info_ptr->v4_um_info_ptr->rm_info_ptr = NULL; 
      }
      
      /*-----------------------------------------------------------------------
        Wrt 3gpp2, bearer or iface is not deregistered with dpm. So in case
        of handoff, if its dual ip call then set one iface to NULL else 
        free dpm um entry
      -----------------------------------------------------------------------*/      
      if (PS_SYS_TECH_3GPP2 == 
            ps_sys_get_tech_from_rat(dpm_rm_info_ptr->v4_um_info_ptr->rat))
      {
        if (NULL != dpm_rm_info_ptr->v4_um_info_ptr->v6_iface_ptr)
        {
          dpm_rm_info_ptr->v4_um_info_ptr->v4_iface_ptr = NULL; 
        }
        else
        {
          ps_dpmi_free_dpm_um_info(dpm_rm_info_ptr->v4_um_info_ptr);
          /*
            CR 676312
            1.	Embms call is brought up
            2.	Embms registers bearer with dpm with both v4 iface and v6 iface same
            3.	Before RM registers, embms deregister the bearer with dpm
            4.	Rm get iface up ind, rm creates new enter but doesn’t find dpm um info, 
                it assumes its 3gpp2
            5.	Rm gets iface down ind
            6.	Rm deregisters with dpm but since we don’t have below check, 
                it results in double free            
          */
          if (dpm_rm_info_ptr->v4_um_info_ptr == dpm_rm_info_ptr->v6_um_info_ptr)
          {
            dpm_rm_info_ptr->v6_um_info_ptr = NULL;
          }          
        }
      }
      
      LOG_MSG_INFO1_2("ps_dpmi_handoff_success_ev_hdlr() : "
                      "DPM RM 0x%x no longer linked to v4 DPM UM 0x%x",
                      dpm_rm_info_ptr,
                      dpm_rm_info_ptr->v4_um_info_ptr);       
      dpm_rm_info_ptr->v4_um_info_ptr = NULL;            
    }
    
    if (IPV6_ADDR == addr_family && NULL != dpm_rm_info_ptr->v6_um_info_ptr)
    {    
      if (NULL != dpm_rm_info_ptr->v6_iface_ptr)
      {
        /*-----------------------------------------------------------------------
          Deregister for iface/flow/phys link events
        -----------------------------------------------------------------------*/
        (void) ps_dpmi_dereg_phys_link_events(dpm_rm_info_ptr, IPV6_ADDR);
         dpm_rm_info_ptr->v6_phys_link_ptr  = NULL;
      } 
      
      /*-----------------------------------------------------------------------
        If call is of type dual ip over single rmnet call then RM would have
        same ptr form dpm um v4 and dpm um v6. So if handoff happens for 
        v4 and v6 is still there then dpm shouldnt break the link else it
        would result in crash and vice versa.
      -----------------------------------------------------------------------*/      
      if (dpm_rm_info_ptr->v6_um_info_ptr != dpm_rm_info_ptr->v4_um_info_ptr)
      {
        LOG_MSG_INFO1_2("ps_dpmi_handoff_success_ev_hdlr() : "
                        "V6 DPM UM 0x%x no longer linked to DPM RM 0x%x",
                        dpm_rm_info_ptr->v6_um_info_ptr,
                        dpm_rm_info_ptr->v6_um_info_ptr->rm_info_ptr);      
        ps_dpmi_unbridge_all_bearers_with_hw(dpm_rm_info_ptr->v6_um_info_ptr);
        dpm_rm_info_ptr->v6_um_info_ptr->rm_info_ptr = NULL;
      }
      
      /*-----------------------------------------------------------------------
        Wrt 3gpp2, bearer or iface is not deregistered with dpm. So in case
        of handoff, if its dual ip call then set one iface to NULL else 
        free dpm um entry
      -----------------------------------------------------------------------*/      
      if (PS_SYS_TECH_3GPP2 == 
            ps_sys_get_tech_from_rat(dpm_rm_info_ptr->v6_um_info_ptr->rat))
      {
        if (NULL != dpm_rm_info_ptr->v6_um_info_ptr->v4_iface_ptr)
        {
          dpm_rm_info_ptr->v6_um_info_ptr->v6_iface_ptr = NULL; 
        }
        else
        {
          ps_dpmi_free_dpm_um_info(dpm_rm_info_ptr->v6_um_info_ptr);
          /*
            CR 676312
            1.	Embms call is brought up
            2.	Embms registers bearer with dpm with both v4 iface and v6 iface same
            3.	Before RM registers, embms deregister the bearer with dpm
            4.	Rm get iface up ind, rm creates new enter but doesn’t find dpm um info, 
                it assumes its 3gpp2
            5.	Rm gets iface down ind
            6.	Rm deregisters with dpm but since we don’t have below check, 
                it results in double free            
          */
          if (dpm_rm_info_ptr->v6_um_info_ptr == dpm_rm_info_ptr->v4_um_info_ptr)
          {
            dpm_rm_info_ptr->v4_um_info_ptr = NULL;
          }           
        }
      }
      
      LOG_MSG_INFO1_2("ps_dpmi_handoff_success_ev_hdlr() : "
                      "DPM RM 0x%x no longer linked to v6 DPM UM 0x%x",
                      dpm_rm_info_ptr,
                      dpm_rm_info_ptr->v6_um_info_ptr);       
      
      dpm_rm_info_ptr->v6_um_info_ptr = NULL;
    }

    /*-----------------------------------------------------------------------
      Fetch base iface of the bridge iface in order to find corresponding
      DPM Um info
    -----------------------------------------------------------------------*/
    bridge_iface_ptr =
      PS_IFACEI_GET_BRIDGE_IFACE(handoff_success_ev_info_ptr->rm_iface_ptr);
    
    if (!PS_IFACE_IS_VALID(bridge_iface_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_handoff_success_ev_hdlr(): "
                      "Invalid bridge iface");
      break;
    }

    #ifdef FEATURE_DATA_PS_464XLAT
      um_iface_ptr = PS_DPM_GET_BASE_IFACE(bridge_iface_ptr);
    #else
      um_iface_ptr = PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr);
    #endif /* FEATURE_DATA_PS_464XLAT */
    
    if (um_iface_ptr == bridge_iface_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpmi_handoff_success_ev_hdlr(): "
                      "Call was brought down after handoff swap rat and"
                      " before DPM recieved the event");
      break;    
    }

    LOG_MSG_INFO1_2("ps_dpmi_handoff_success_ev_hdlr() : "
                    "DPM RM linking to um_iface_ptr 0x%x:%d", 
                    um_iface_ptr->name, um_iface_ptr->instance);    

    /*-----------------------------------------------------------------------
      Find corresponding DPM Um info using Um iface. If no DPM um info is 
      not found, it means either um hasnt registered yet (ehrpd to lte) or
      it is lte to ehrp handoff where 3gpp2 doesnt register with DPM
    -----------------------------------------------------------------------*/
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      LOG_MSG_INFO1_0("ps_dpmi_handoff_success_ev_hdlr() : "
                      "Allocing new dpm um info, possibly 3gpp2");      
      /*---------------------------------------------------------------------
        Since 3GPP2 mode handler doesn't register with DPM, this case is
        not invalid. So allocate a new DPM Um info
      ---------------------------------------------------------------------*/
      dpm_um_info_ptr = ps_dpmi_alloc_dpm_um_info();
      if (NULL == dpm_um_info_ptr)
      {
        break;
      }

      is_dpm_um_info_allocated = TRUE;

      /*---------------------------------------------------------------------
        Initialize DPM RAT info to be either lte or 1x to get started with.
        RAt would be updated to correct value in case of 3gpp when bearer
        or iface is registered
      ---------------------------------------------------------------------*/      
      if (um_iface_ptr->name == CDMA_SN_IFACE ||
          um_iface_ptr->name == CDMA_AN_IFACE)
      {
        dpm_um_info_ptr->rat = PS_SYS_RAT_EX_3GPP2_1X;
      }
      else
      {
        dpm_um_info_ptr->rat = PS_SYS_RAT_EX_3GPP_LTE;
      }

      if (IPV4_ADDR == addr_family)
      {
        dpm_um_info_ptr->v4_iface_ptr = um_iface_ptr;
      }
      else
      {
        dpm_um_info_ptr->v6_iface_ptr = um_iface_ptr;
      }
      
      if (um_iface_ptr->name == UMTS_IFACE)
      {
        LOG_MSG_ERROR_2("ps_dpmi_handoff_success_ev_hdlr() : "
                        "DBG msg Crashing the UE to catch one issue"
                        " dpm_um_info_ptr 0x%p iface 0x%p" ,                       
                        dpm_um_info_ptr, um_iface_ptr);  
        ASSERT(0);
      }      
    }

    dpm_um_info_ptr->rm_info_ptr = dpm_rm_info_ptr;

    /*-----------------------------------------------------------------------
      Update DPM Rm info based on addr family
        1. Rm iface
        2. DPM Um info
        3. Install prefix routing rule for IPv6 Rm iface
        4. Register bearers with SIO stream ID and also bridge them
    -----------------------------------------------------------------------*/
    if (IPV4_ADDR == addr_family)
    {
      dpm_rm_info_ptr->v4_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v4_iface_ptr   = 
        handoff_success_ev_info_ptr->rm_iface_ptr;
      
      dpm_rm_info_ptr->v4_phys_link_ptr  = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);

      if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
      {
        break;
      }
    }
    else if (IPV6_ADDR == addr_family)
    {
      dpm_rm_info_ptr->v6_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v6_iface_ptr   = 
        handoff_success_ev_info_ptr->rm_iface_ptr;
      
      dpm_rm_info_ptr->v6_phys_link_ptr  = PS_IFACEI_GET_PHYS_LINK(um_iface_ptr);       
       
      if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
      {
        break;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Special case for eMBMS
      ---------------------------------------------------------------------*/

      dpm_rm_info_ptr->v4_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v6_um_info_ptr = dpm_um_info_ptr;
      dpm_rm_info_ptr->v4_iface_ptr   = 
        handoff_success_ev_info_ptr->rm_iface_ptr;
      dpm_rm_info_ptr->v6_iface_ptr   = 
        handoff_success_ev_info_ptr->rm_iface_ptr;
      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl = PS_IFACE_IPFLTR_INVALID_HANDLE;        

      if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
      {
        break;
      }
    }

    /*-------------------------------------------------------------------------
    Unbridge the bearer after handover
    -------------------------------------------------------------------------*/
    if (PS_IFACEI_GET_UNBRIDGE_MODE(bridge_iface_ptr) == TRUE)
    {
      data_path_info_ptr.data_path = IFACE_DATA_PATH_SOFTWARE;
      unbridge_mode_ref_cnt = PS_IFACEI_GET_UNBRIDGE_MODE_REF_CNT(PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr));
      PS_IFACEI_SET_UNBRIDGE_MODE_REF_CNT(PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr),0);
      /*-------------------------------------------------------------------------
        Ignore result code, HO should not stop because of IOCTL failure
      -------------------------------------------------------------------------*/
      ps_iface_ioctl (bridge_iface_ptr,
                      PS_IFACE_IOCTL_SET_DATA_PATH_BRIDGE_MODE,
                      &data_path_info_ptr,
                      &ps_err);
      PS_IFACEI_SET_UNBRIDGE_MODE_REF_CNT(PS_IFACE_GET_BASE_IFACE(bridge_iface_ptr),unbridge_mode_ref_cnt);
    }

    /*-----------------------------------------------------------------------
      Register for iface, and flow events on Um iface
    ---------------------------- ------------------------------------------*/
    (void) ps_dpmi_reg_phys_link_events(dpm_rm_info_ptr, addr_family);

    /*-----------------------------------------------------------------------
      Continue UL data processing by enabling the UL signal
    ---------------------------- ------------------------------------------*/    
    PS_RM_SET_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);
    
    PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 

    return;
  } while (0);

  /*-------------------------------------------------------------------------
    Error handling

    Clean up DPM Rm info based on addr family

    If DPM Um info is allocated, free it blindly as it happens only for 3GPP2
    case
  -------------------------------------------------------------------------*/

  if (NULL != dpm_rm_info_ptr)
  {
    if (IPV4_ADDR == addr_family)
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, TRUE, FALSE);
    }
    else if (IPV6_ADDR == addr_family)
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, FALSE, TRUE);
    }
    else
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, TRUE, TRUE);
    }
  }

  if (TRUE == is_dpm_um_info_allocated)
  {
    ps_dpmi_free_dpm_um_info(dpm_um_info_ptr);
  }

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 

  return;
} /* ps_dpmi_handoff_success_ev_hdlr() */



static void ps_dpmi_prefix_updated_ev_hdlr
(
  ps_dpm_prefix_updated_ev_info_type   * prefix_updated_ev_info_ptr
)
{
  ps_dpm_rm_info_type             * dpm_rm_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("ps_dpmi_prefix_updated_ev_hdlr(): stream id %d um iface 0x%p",
                  prefix_updated_ev_info_ptr->sio_stream_id, 
                  prefix_updated_ev_info_ptr->um_iface_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                   
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section); 

  do
  {
    /*-----------------------------------------------------------------------
      Check if DPM Rm info is already present. If not, allocate a new
      DPM Rm info
    -----------------------------------------------------------------------*/
    dpm_rm_info_ptr = 
      ps_dpmi_get_dpm_rm_info(prefix_updated_ev_info_ptr->sio_stream_id);
    if (NULL == dpm_rm_info_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpmi_prefix_updated_ev_hdlr() : "
                      "Invalid dpm rm info ptr 0x%x", dpm_rm_info_ptr);
      break;
    }
    /*-----------------------------------------------------------------------
      Remove ipv6 prefix rule
    -----------------------------------------------------------------------*/    
    if (PS_IFACE_IPFLTR_INVALID_HANDLE !=
          dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl)
    {
      ps_iface_ipfltr_hw_delete_v6_prefix_hndl
      (
        dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl
      );

      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl =
        PS_IFACE_IPFLTR_INVALID_HANDLE;
    }    
          
    /*----------------------------------------------------------------------
      Check if Iface is valid and NOT in DOWN or GOING DOWN state, then only add
      filter
    -----------------------------------------------------------------------*/
    if (PREFIX_ADDED == prefix_updated_ev_info_ptr->prefix_update_kind )
    {
      if ( PS_IFACE_IS_VALID( prefix_updated_ev_info_ptr->um_iface_ptr ))
      {
        if ( IFACE_DOWN != 
	       PS_IFACEI_GET_STATE( prefix_updated_ev_info_ptr->um_iface_ptr) &&
             IFACE_GOING_DOWN !=
               PS_IFACEI_GET_STATE( prefix_updated_ev_info_ptr->um_iface_ptr)
           )
        {
          dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl =
            ps_iface_ipfltr_hw_add_ul_prefix_comp_fltr
            (
              prefix_updated_ev_info_ptr->um_iface_ptr,
              prefix_updated_ev_info_ptr->sio_stream_id,
              PS_IFACE_IPFLTR_RESULT_UL_IPV6
            );
        }
        else
        {
          LOG_MSG_ERROR_1("ps_dpmi_prefix_updated_ev_hdlr() : "
		      	          " Iface state 0x%x", 
                PS_IFACEI_GET_STATE(prefix_updated_ev_info_ptr->um_iface_ptr));
        }
      }
      
      LOG_MSG_INFO2_1("ps_dpmi_prefix_updated_ev_hdlr() : "
                      "v6 prefix comp filter handle 0x%p",      
                      dpm_rm_info_ptr->v6_prefix_comp_ipfltr_hdl);
    }
  } while (0);


  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 

  return;
} /* ps_dpmi_prefix_updated_ev_hdlr() */


static void ps_dpmi_um_dereg_bearer_hdlr
(
  ps_dpm_um_dereg_bearer_info_type  * um_dereg_bearer_info_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;
  ps_dpm_bearer_info_type  * bearer_ptr;
  ps_iface_type            * um_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("ps_dpmi_um_dereg_bearer_hdlr(): "
                  "v4 Um iface 0x%p v6 Um iface 0x%p phys link 0x%p",
                  um_dereg_bearer_info_ptr->v4_iface_ptr,
                  um_dereg_bearer_info_ptr->v6_iface_ptr,
                  um_dereg_bearer_info_ptr->ps_phys_link_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Find DPM Um info
    -----------------------------------------------------------------------*/
    if (NULL != um_dereg_bearer_info_ptr->v4_iface_ptr)
    {
      um_iface_ptr = um_dereg_bearer_info_ptr->v4_iface_ptr;
    }
    else
    {
      um_iface_ptr = um_dereg_bearer_info_ptr->v6_iface_ptr;
    }

    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Make sure that mode handler is passing same v4 and v6 Um ifaces as that
      are there in the DPM entry
    -----------------------------------------------------------------------*/
    if (dpm_um_info_ptr->v4_iface_ptr !=
          um_dereg_bearer_info_ptr->v4_iface_ptr ||
        dpm_um_info_ptr->v6_iface_ptr !=
          um_dereg_bearer_info_ptr->v6_iface_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpmi_um_dereg_bearer_hdlr(): DPM entry doesn't match "
                      "passed arg, DPM v4 Um iface 0x%p DPM v6 Um iface 0x%p",
                      dpm_um_info_ptr->v4_iface_ptr,
                      dpm_um_info_ptr->v6_iface_ptr);
      break;
    }

    /*-----------------------------------------------------------------------
      Find bearer info
    -----------------------------------------------------------------------*/
    bearer_ptr =
      ps_dpmi_get_bearer_info(dpm_um_info_ptr,
                              um_dereg_bearer_info_ptr->ps_phys_link_ptr);
    if (NULL == bearer_ptr)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Delete bearer
    -----------------------------------------------------------------------*/
    if (-1 == ps_dpmi_delete_bearer(dpm_um_info_ptr, bearer_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_um_dereg_bearer_hdlr(): Couldn't delete bearer");
      break;
    }

    /*-----------------------------------------------------------------------
      If this is not the last bearer, return without cleaning up DPM Um info
    -----------------------------------------------------------------------*/
    if (0 != dpm_um_info_ptr->num_bearer)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Dereg Rm if Um is in tethered call
    -----------------------------------------------------------------------*/
    if (NULL != dpm_um_info_ptr->rm_info_ptr)
    {
      if (dpm_um_info_ptr == dpm_um_info_ptr->rm_info_ptr->v4_um_info_ptr)
      {
        /*-----------------------------------------------------------------------
          Deregister for phys link events
        -----------------------------------------------------------------------*/
        (void) ps_dpmi_dereg_phys_link_events(dpm_um_info_ptr->rm_info_ptr, IPV4_ADDR);
        
        dpm_um_info_ptr->rm_info_ptr->v4_um_info_ptr = NULL;
      }
      
      if (dpm_um_info_ptr == dpm_um_info_ptr->rm_info_ptr->v6_um_info_ptr)
      {
        /*-----------------------------------------------------------------------
          Deregister for phys link events
        -----------------------------------------------------------------------*/
        (void) ps_dpmi_dereg_phys_link_events(dpm_um_info_ptr->rm_info_ptr, IPV6_ADDR);
        
        dpm_um_info_ptr->rm_info_ptr->v6_um_info_ptr = NULL;
      } 
    }
    
    /*-----------------------------------------------------------------------
      Free DPM Um info
    -----------------------------------------------------------------------*/
    ps_dpmi_free_dpm_um_info(dpm_um_info_ptr);

  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-----------------------------------------------------------------------
    Unblock DS task since DPM is completed with deregistration
  -----------------------------------------------------------------------*/                             
#ifdef TEST_FRAMEWORK
  #error code not present
#endif 
  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_DPM_WAIT_SIGNAL );    
  return;
} /* ps_dpmi_um_dereg_bearer_hdlr() */


static void ps_dpmi_um_dereg_iface_hdlr
(
  ps_dpm_um_dereg_iface_info_type  * um_dereg_iface_info_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;
  ps_dpm_bearer_info_type  * bearer_ptr;
  ps_iface_type            * um_iface_ptr;
  int32                      bearer_index;
  boolean                    is_v4_free    = FALSE;
  boolean                    is_v6_free    = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_dpmi_um_dereg_iface_hdlr(): "
                  "v4 Um iface 0x%p v6 Um iface 0x%p",
                  um_dereg_iface_info_ptr->v4_iface_ptr,
                  um_dereg_iface_info_ptr->v6_iface_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    /*-----------------------------------------------------------------------
      Find DPM Um info
    -----------------------------------------------------------------------*/
    if (NULL != um_dereg_iface_info_ptr->v4_iface_ptr)
    {
      um_iface_ptr = um_dereg_iface_info_ptr->v4_iface_ptr;
    }
    else
    {
      um_iface_ptr = um_dereg_iface_info_ptr->v6_iface_ptr;
    }

    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
    if (NULL == dpm_um_info_ptr)
    {
      break;
    }

    //MSR TODO add comments
    if (NULL != dpm_um_info_ptr->v4_iface_ptr)
    {
      if (NULL != um_dereg_iface_info_ptr->v4_iface_ptr)
      {
        if (dpm_um_info_ptr->v4_iface_ptr !=
              um_dereg_iface_info_ptr->v4_iface_ptr)
        {
          LOG_MSG_ERROR_1("ps_dpmi_um_dereg_iface_hdlr(): DPM entry doesn't "
                          "match passed arg, DPM v4 Um iface 0x%p",
                          dpm_um_info_ptr->v4_iface_ptr);
          break;
        }

        is_v4_free = TRUE;
      }
    }
    else
    {
      if (NULL != um_dereg_iface_info_ptr->v4_iface_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpmi_um_dereg_iface_hdlr(): DPM entry doesn't "
                        "match passed arg, DPM v4 Um iface 0x%p", NULL);
        break;
      }

      is_v4_free = TRUE;
    }

    if (NULL != dpm_um_info_ptr->v6_iface_ptr)
    {
      if (NULL != um_dereg_iface_info_ptr->v6_iface_ptr)
      {
        if (dpm_um_info_ptr->v6_iface_ptr !=
              um_dereg_iface_info_ptr->v6_iface_ptr)
        {
          LOG_MSG_ERROR_1("ps_dpmi_um_dereg_iface_hdlr(): DPM entry doesn't "
                          "match passed arg, DPM v6 Um iface 0x%p",
                          dpm_um_info_ptr->v6_iface_ptr);
          break;
        }

        is_v6_free = TRUE;
      }
    }
    else
    {
      if (NULL != um_dereg_iface_info_ptr->v6_iface_ptr)
      {
        LOG_MSG_ERROR_1("ps_dpmi_um_dereg_iface_hdlr(): DPM entry doesn't "
                        "match passed arg, DPM v6 Um iface 0x%p", NULL);
        break;
      }

      is_v6_free = TRUE;
    }

    /*-----------------------------------------------------------------------
      If both v4 and v6 ifaces are deregistered, then free up the bearer and
      dpm um info else just reset the iface in dpm um entry
    -----------------------------------------------------------------------*/
    if (NULL != dpm_um_info_ptr->rm_info_ptr)
    {

      if ((TRUE ==  is_v4_free) && 
          (dpm_um_info_ptr == dpm_um_info_ptr->rm_info_ptr->v4_um_info_ptr))
      {
        /*-----------------------------------------------------------------------
          Deregister for phys link events
        -----------------------------------------------------------------------*/
        (void) ps_dpmi_dereg_phys_link_events
               (
                 dpm_um_info_ptr->rm_info_ptr, IPV4_ADDR
               );              
        /*-----------------------------------------------------------------------
          Donot set dpm_um_info_ptr->rm_info_ptr->v4_um_info_ptr to NULL 
          See CR 683379
        -----------------------------------------------------------------------*/
      }
      
      if ((TRUE == is_v6_free) &&
          (dpm_um_info_ptr == dpm_um_info_ptr->rm_info_ptr->v6_um_info_ptr))
      {
        /*-----------------------------------------------------------------------
          Deregister for phys link events
        -----------------------------------------------------------------------*/
        (void) ps_dpmi_dereg_phys_link_events
               (
                  dpm_um_info_ptr->rm_info_ptr, IPV6_ADDR
               );
        /*-----------------------------------------------------------------------
          Donot set dpm_um_info_ptr->rm_info_ptr->v6_um_info_ptr to NULL 
          See CR 683379. Also check analysis of CR 692508
        -----------------------------------------------------------------------*/
      } 
    }

    if (TRUE == is_v4_free && TRUE == is_v6_free)
    {
      for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
      {
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr)
        {
          if (-1 == ps_dpmi_delete_bearer(dpm_um_info_ptr, bearer_ptr))
          {
            LOG_MSG_ERROR_1("ps_dpmi_um_dereg_bearer_hdlr(): "
                            "Couldn't delete bearer 0x%p", bearer_ptr);
            ASSERT(0);
            break;
          }
        }
      }

      /*-----------------------------------------------------------------------
        DPM has to check bridging functionality else following scenario breaks
        1. DPM RM v4 with stream id A connects to APN X
        2. DPM RM v6 with stream id A connects to APN Y
        
        If briding is not checked, DPM would errorneously remove link to 
        other APN.
      -----------------------------------------------------------------------*/
      if (NULL != dpm_um_info_ptr->rm_info_ptr)
      {
        if (dpm_um_info_ptr == dpm_um_info_ptr->rm_info_ptr->v4_um_info_ptr)
        {
          dpm_um_info_ptr->rm_info_ptr->v4_um_info_ptr = NULL;
        }
          
        if (dpm_um_info_ptr == dpm_um_info_ptr->rm_info_ptr->v6_um_info_ptr)
        {
          dpm_um_info_ptr->rm_info_ptr->v6_um_info_ptr = NULL;
        }
      }
      
      ps_dpmi_free_dpm_um_info(dpm_um_info_ptr);
    }
    else if (TRUE == is_v4_free)
    {
      dpm_um_info_ptr->v4_iface_ptr = NULL;
    }
    else
    {
      dpm_um_info_ptr->v6_iface_ptr = NULL;
    }

  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-----------------------------------------------------------------------
    Unblock DS task since DPM is completed with deregistration
  -----------------------------------------------------------------------*/                             
  (void)rex_set_sigs( dsi_get_ds_tcb(), DS_DPM_WAIT_SIGNAL );

  return;
} /* ps_dpmi_um_dereg_iface_hdlr() */


static void ps_dpmi_rm_dereg_iface_hdlr
(
  ps_dpm_rm_dereg_iface_info_type  * rm_dereg_iface_info_ptr
)
{
  ps_dpm_rm_info_type        * dpm_rm_info_ptr;
  ps_iface_addr_family_type    addr_family;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_dpmi_rm_dereg_iface_hdlr(): "
                  "SIO stream ID %d Rm iface 0x%p",
                  rm_dereg_iface_info_ptr->sio_stream_id,
                  rm_dereg_iface_info_ptr->rm_iface_ptr);

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    addr_family =
      ps_iface_get_addr_family(rm_dereg_iface_info_ptr->rm_iface_ptr);

    dpm_rm_info_ptr =
      ps_dpmi_get_dpm_rm_info(rm_dereg_iface_info_ptr->sio_stream_id);
    if (NULL == dpm_rm_info_ptr)
    {
      break;
    }

    if (dpm_rm_info_ptr->v4_iface_ptr != rm_dereg_iface_info_ptr->rm_iface_ptr &&
        dpm_rm_info_ptr->v6_iface_ptr != rm_dereg_iface_info_ptr->rm_iface_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpmi_rm_dereg_iface_hdlr(): "
                      "Call with rm iface 0x%p wasnt registered with DPM ",
                      rm_dereg_iface_info_ptr->rm_iface_ptr);    
      ASSERT(0);
      break;
    }

    if (IPV4_ADDR == addr_family)
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, TRUE, FALSE);
    }
    else if (IPV6_ADDR == addr_family)
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, FALSE, TRUE);
    }
    else
    {
      ps_dpmi_rm_dereg_internal(dpm_rm_info_ptr, TRUE, TRUE);
    }
    
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  /*-------------------------------------------------------------------
    Unblock DCC task since DPM is completed with Deregistration
  -------------------------------------------------------------------*/  
  DCC_SET_SIGNAL(DCC_DPM_RM_WAIT_SIGNAL);

  return;
} /* ps_dpmi_rm_dereg_iface_hdlr() */


static void ps_dpmi_rm_config_eth_hdr_cmd_hdlr
(
  ps_dpm_rm_eth_hdr_info_type  * eth_hdr_info_ptr
)
{
  ps_dpm_rm_info_type        * dpm_rm_info_ptr;
  ps_dpm_um_info_type         * dpm_um_info_ptr;
  llc_frm_packed_mode_zero_type   * eth_hdr_ptr;
  ps_iface_addr_family_type     addr_family;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_2("ps_dpmi_rm_config_eth_hdr_cmd_hdlr(): SIO stream id %d "
                  "Rm iface 0x%p",
                  eth_hdr_info_ptr->sio_stream_id,
                  eth_hdr_info_ptr->rm_iface_ptr);
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  do
  {
    addr_family =
      ps_iface_get_addr_family(eth_hdr_info_ptr->rm_iface_ptr);

    dpm_rm_info_ptr = ps_dpmi_get_dpm_rm_info(eth_hdr_info_ptr->sio_stream_id);
    if (NULL == dpm_rm_info_ptr)
    {
      break;
    }

    if (IPV4_ADDR == addr_family)
    {
      dpm_um_info_ptr = dpm_rm_info_ptr->v4_um_info_ptr;
      eth_hdr_ptr = dpm_rm_info_ptr->v4_eth_hdr_ptr;
    }
    else
    {
      dpm_um_info_ptr = dpm_rm_info_ptr->v6_um_info_ptr;
      eth_hdr_ptr = dpm_rm_info_ptr->v6_eth_hdr_ptr;
    }

    memscpy(eth_hdr_ptr,
            sizeof(llc_frm_packed_mode_zero_type),
            &(eth_hdr_info_ptr->eth_hdr),
            sizeof(llc_frm_packed_mode_zero_type));

    //MSR TODO have to bridge once v4 gets eth addr but what if v6 hasnt
    //gotten yet? How will hardware handle v6 data? And is there a point in trying
    //todo this for even v4?
    dpm_rm_info_ptr->is_eth_addr_configured = TRUE;

    if (-1 == ps_dpmi_bridge_all_bearers_with_hw(dpm_um_info_ptr))
    {
      LOG_MSG_ERROR_0("ps_dpmi_rm_config_eth_hdr_cmd_hdlr(): "
                      "Couldn't bridge all bearers with Hardware");
      break;
    }
  } while (0);

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return;
} /* ps_dpmi_rm_config_eth_hdr_cmd_hdlr */


static void ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr
(
  int32     dpm_um_handle
)
{
  ps_dpm_bearer_info_type  *  bearer_info_ptr;
  ps_dpm_um_info_type      *  dpm_um_info_ptr;
  ps_dpm_rm_info_type      *  dpm_rm_info_ptr;
  int16                       bearer_index; 
  int32                       num_bearer_info_ptr = 0;   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr() ");
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section); 
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  
  do
  {
    dpm_um_info_ptr =  ps_dpmi_get_dpm_um_info_by_handle(dpm_um_handle);
    if (NULL == dpm_um_info_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr() : "
                      " Invalid dpm um handle 0x%x", dpm_um_handle);
      break;
    }  
    
    dpm_rm_info_ptr = dpm_um_info_ptr->rm_info_ptr;
    if (NULL == dpm_rm_info_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr() : "
                      " DPM RM side have gone away");
      break;
    }   
    
    if (NULL != dpm_rm_info_ptr->sio_tx_wm_ptr)
    {
      dpm_rm_info_ptr->sio_tx_wm_ptr->gone_empty_func_ptr = NULL;
      dpm_rm_info_ptr->sio_tx_wm_ptr->gone_empty_func_data = NULL;
    }
    
    /*-------------------------------------------------------------------------
      At this point, all bearers for given um is flushed and sio tx wmk is
      also flushed, so activate and bridge all the bearers
    -------------------------------------------------------------------------*/    
    for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS && 
           num_bearer_info_ptr < dpm_um_info_ptr->num_bearer; 
         bearer_index++)
    {
      bearer_info_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
      if (NULL != bearer_info_ptr)
      {
        num_bearer_info_ptr++; 
        ps_dpmi_activate_bearer_with_hw(bearer_info_ptr->uid);                            
        ps_dpmi_bridge_bearer_with_hw(dpm_um_info_ptr, bearer_info_ptr);      
      }
    }
    
    dpm_um_info_ptr->is_bridged_with_hw = TRUE;
    dpm_um_info_ptr->is_hw_bridging_allowed = TRUE;
    
    /*---------------------------------------------------------------------
      Reset the bit mask for bearer flushed.
    ---------------------------------------------------------------------*/     
    dpm_um_info_ptr->all_bearers_flushed = 0xFF;
    
  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
  
  return;
} /* ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr */

static void ps_dpm_sio_tx_wmk_gone_empty_cb
(
  dsm_watermark_type  *  dsm_wm_ptr, 
  void                *  cb_data
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_dpm_sio_tx_wmk_gone_empty_cb() ");
  /*-------------------------------------------------------------------------
    Allocate DPM cmd buf, populate it and post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                            ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_reg(): Failed to alloc DPM cmd buf");
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_SIO_TX_WMK_GONE_EMPTY_CMD;
  dpm_cmd_info_ptr->dpm_cmd_info.sio_tx_wmk_empty_info = (int32)cb_data;

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpm_sio_tx_wmk_gone_empty_cb */

static void ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr
(
  int32     dpm_um_bearer_handle
)
{
  int32                       dpm_bearer_handle;
  int32                       dpm_um_handle;
  ps_dpm_bearer_info_type  *  bearer_info_ptr;
  ps_dpm_um_info_type      *  dpm_um_info_ptr;
  ps_dpm_rm_info_type      *  dpm_rm_info_ptr;
  int16                       bearer_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr() ");
  /*-------------------------------------------------------------------------
   Retrieve respective handles
  -------------------------------------------------------------------------*/
  dpm_um_handle        = (0x0000FFFF & dpm_um_bearer_handle);
  dpm_bearer_handle    = (0x0000FFFF & (dpm_um_bearer_handle >> 16));  
  
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  
  do
  {
    dpm_um_info_ptr =  ps_dpmi_get_dpm_um_info_by_handle(dpm_um_handle);
    if (NULL == dpm_um_info_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr() : "
                      " Invalid dpm um handle 0x%x", dpm_um_handle);
      break;
    }  
    
    dpm_rm_info_ptr = dpm_um_info_ptr->rm_info_ptr;
    if (NULL == dpm_rm_info_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr() : "
                      " DPM RM side have gone away");    
      break;
    }
    
    bearer_info_ptr =  ps_dpmi_get_dpm_bearer_info_by_handle(dpm_um_info_ptr,
                                                             dpm_bearer_handle);
    if (NULL == bearer_info_ptr)
    {
      LOG_MSG_ERROR_1("ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr() : "
                      " Invalid bearer handle 0x%x", dpm_bearer_handle);
      break;
    }
    
    bearer_index =
      ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_BEARER, 
                              dpm_bearer_handle);
    
    dpm_um_info_ptr->all_bearers_flushed &= ~(1 << bearer_index);
    
    bearer_info_ptr->hw_to_ps_wm.gone_empty_func_ptr = NULL;
    bearer_info_ptr->hw_to_ps_wm.gone_empty_func_data = NULL;    
    
    /*-------------------------------------------------------------------------
      Check if all bearers for given dpm um info are flushed. If so then 
      wait till sio tx wmk is flushed
    -------------------------------------------------------------------------*/    
    if (0 == dpm_um_info_ptr->all_bearers_flushed)
    {
      LOG_MSG_INFO1_0("ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr() : "
                      "All bearers flushed.");
    }
    else
    {
      /*---------------------------------------------------------------------
         Not all bearers are flushed, so wait till all of them are flushed
      ---------------------------------------------------------------------*/      
      LOG_MSG_INFO1_1("ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr() : "
                      "Not All bearers flushed.", 
                      dpm_um_info_ptr->all_bearers_flushed);      
      break;
    }

    /*-------------------------------------------------------------------------
      At this point, all bearer for given dpm um info are flushed. Now wait
      till sio tx wmk is flushed
    -------------------------------------------------------------------------*/       
    if ( NULL != dpm_rm_info_ptr->sio_tx_wm_ptr)
    {
      dpm_rm_info_ptr->sio_tx_wm_ptr->gone_empty_func_ptr = 
        ps_dpm_sio_tx_wmk_gone_empty_cb;
      dpm_rm_info_ptr->sio_tx_wm_ptr->gone_empty_func_data = 
        (void *)dpm_um_handle;
       
      /*-------------------------------------------------------------------------
        Sio tx wmk is already empty, directly call the handler
      -------------------------------------------------------------------------*/       
      if (0 == dpm_rm_info_ptr->sio_tx_wm_ptr->current_cnt)
      {
        ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr(dpm_um_handle);     
      }
    }
   
  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
  
  return;
} /* ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr */

static void ps_dpm_hw_to_ps_wmk_gone_empty_cb
(
  dsm_watermark_type  *  dsm_wm_ptr, 
  void                *  cb_data
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_dpm_hw_to_ps_wmk_gone_empty_cb() ");
  /*-------------------------------------------------------------------------
    Allocate DPM cmd buf, populate it and post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_reg(): Failed to alloc DPM cmd buf");
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_HW_TO_PS_WMK_GONE_EMPTY_CMD;
  dpm_cmd_info_ptr->dpm_cmd_info.hw_to_ps_wmk_empty_info = (int32)cb_data; 

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpm_hw_to_ps_wmk_gone_empty_cb */

static void ps_dpm_resume_hw_path_per_um
(
  ps_dpm_um_info_type      * dpm_um_info_ptr
)
{
  ps_dpm_bearer_info_type  * bearer_ptr;
  int16                      bearer_index; 
  int32                      dpm_um_bearer_handle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    - Check if tech type is 3gpp if not proceed with next dpm um entry
    - Check if RM side is up if not then proceed with next dpm um entry 

    Go through all bearers in all dpm um info    
    -  Suspend hardware from dequeing further data from pdcp to hw wmk
    -  Wait till hardware to ps wmk for all bearers on dpm um info is empty
    -  Wait till sio wmk is empty
    -  Activate pdcp to hw path
    -  Bridge all the bearers
    -  Enable DPL logging
  -------------------------------------------------------------------------*/

  if ((NULL != dpm_um_info_ptr &&
      (FALSE == dpm_um_info_ptr->is_bridged_with_hw)))
  {

    dpm_um_info_ptr->is_hw_bridging_allowed = TRUE;
          
    /*-----------------------------------------------------------------
      Check if tech type is 3gpp and RM side is alive, if not then
      go to next dpm um entry
    -----------------------------------------------------------------*/      
    if (PS_SYS_TECH_3GPP != ps_sys_get_tech_from_rat(dpm_um_info_ptr->rat) ||
        NULL == dpm_um_info_ptr->rm_info_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpm_resume_hw_path_per_um() : rat %d"
                      " dpm rm 0x%p", dpm_um_info_ptr->rat, 
                      dpm_um_info_ptr->rm_info_ptr);
      return;
    }       
    
    for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS;
         bearer_index++)
    {
      bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
      if (NULL != bearer_ptr)
      {
        LOG_MSG_INFO2_3("ps_dpm_resume_hw_path_per_um(): "
                        "Calling resume operation for dpm um info 0x%x "
                        "bearer 0x%x at index %d", dpm_um_info_ptr,
                        bearer_ptr, bearer_index);
        
        /*-----------------------------------------------------------------
          Suspend hardware from dequeing further data from pdcp to hw wmk.
        -----------------------------------------------------------------*/                             
        ps_dpmi_suspend_bearer_with_hw(bearer_ptr->uid);   
        bearer_ptr->is_bridged = FALSE;
        /*-----------------------------------------------------------------
          Register for gone empty callback for hw_to_ps wmk. Once we 
          hit gone empty level, we register for gone empty on sio tx wmk,
          once we hit gone empty on sio tx wmk, we activate the bearer
          and bridge the bearer
        -----------------------------------------------------------------*/           
        bearer_ptr->hw_to_ps_wm.gone_empty_func_ptr = 
          ps_dpm_hw_to_ps_wmk_gone_empty_cb;
          
        /*-----------------------------------------------------------------
          Generate user data by combining DPM Um handle and bearer handle
        -----------------------------------------------------------------*/
        dpm_um_bearer_handle = (0xFFFF0000 & (bearer_ptr->handle << 16)) |
                               (0x0000FFFF & dpm_um_info_ptr->handle);
                               
        bearer_ptr->hw_to_ps_wm.gone_empty_func_data = 
          (void *)dpm_um_bearer_handle;
        
        /*-----------------------------------------------------------------
          If hw_to_ps wmk is already empty, call gone empty function 
          handler directly
        -----------------------------------------------------------------*/            
        if (0 == bearer_ptr->hw_to_ps_wm.current_cnt)
        {
          ps_dpm_hw_to_ps_wmk_gone_empty_cb( &(bearer_ptr->hw_to_ps_wm),
                                             (void *)dpm_um_bearer_handle);
        }        
      }
      else
      {
        dpm_um_info_ptr->all_bearers_flushed &= ~(1 << bearer_index);
      }
    }
  }
} /* ps_dpm_resume_hw_path_per_um */

static void ps_dpm_resume_hw_path_cmd_hdlr(void)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;
  int32                      num_dpm_um_info_ptr = 0;
  int16                      dpm_um_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section); 

  /*-------------------------------------------------------------------------
    Go through all non NULL dpm um entries and resume hardware path
  -------------------------------------------------------------------------*/
  for (dpm_um_index = 0;
       dpm_um_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_um_info_ptr < ps_dpm_global_info.num_dpm_um_info_ptr;
       dpm_um_index++)
  {
    if ((NULL != ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index]) &&
        (FALSE == 
          ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index]->is_bridged_with_hw))
    {
      num_dpm_um_info_ptr++;
      dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];
      ps_dpm_resume_hw_path_per_um(dpm_um_info_ptr);
      
      dpm_um_info_ptr->is_hw_bridging_allowed = TRUE;
    }
  }
  
  disable_hw_bridge_mode = PS_DPM_DISABLE_HW_PATH_NONE;
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);     
} /* ps_dpm_resume_hw_path_cmd_hdlr */

static void ps_dpm_resume_hw_path_per_pdn_cmd_hdlr
(
  ps_dpm_resume_hw_path_per_pdn_info_type  * resume_hw_path_per_pdn_info_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr = NULL;
  ps_iface_type            * iface_ptr = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section); 
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  
  do
  {
    if (NULL == resume_hw_path_per_pdn_info_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpm_resume_hw_path_per_pdn_cmd_hdlr() : "
                      "Invalid NULL arg");   
      break;                    
    }
    
    iface_ptr = resume_hw_path_per_pdn_info_ptr->iface_ptr;
    
    if (!PS_IFACE_IS_VALID(iface_ptr))
    {
      LOG_MSG_ERROR_1("ps_dpm_resume_hw_path_per_pdn_cmd_hdlr() : "
                      "Invalid iface 0x%p", iface_ptr);  
      break;                    
    }
    
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(iface_ptr);
    
    if (NULL != dpm_um_info_ptr)
    {
      ps_dpm_resume_hw_path_per_um(dpm_um_info_ptr);
    }
    else
    {
      LOG_MSG_ERROR_1("ps_dpm_resume_hw_path_per_pdn_cmd_hdlr() : "
                      "No dpm entry found for iface 0x%p", iface_ptr);
    }
  
  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  
} /* ps_dpm_resume_hw_path_per_pdn_cmd_hdlr */

static void ps_dpm_suspend_hw_path_per_um
(
  ps_dpm_um_info_type      * dpm_um_info_ptr
)
{
  ps_dpm_bearer_info_type  * bearer_ptr;
  int16                      bearer_index; 
  int32                      num_bearer_info_ptr = 0;  
  sio_port_id_type           sio_port_id = SIO_PORT_NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Go through all bearers in all dpm um info
    For each bearer
    -  Disable DPL logging
    -  Unbridge the bearer with hardware
  -------------------------------------------------------------------------*/
  if (NULL != dpm_um_info_ptr)
  {
    dpm_um_info_ptr->is_hw_bridging_allowed = FALSE;
    
    if (TRUE == dpm_um_info_ptr->is_bridged_with_hw)
    {
      for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS && 
             num_bearer_info_ptr < dpm_um_info_ptr->num_bearer; 
           bearer_index++)
      {
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr)
        {
          num_bearer_info_ptr++;
          LOG_MSG_INFO2_3("ps_dpm_suspend_hw_path_per_um(): "
                          "Calling suspend operation for dpm um info 0x%x "
                          "bearer 0x%x at index %d", dpm_um_info_ptr,
                          bearer_ptr, bearer_index);
        
          /*-----------------------------------------------------------------
            Unbridge the bearer and deregister dpl logging
          -----------------------------------------------------------------*/                             
          ps_dpm_hw_dereg_dpl(bearer_ptr->uid);
        
          if (NULL != dpm_um_info_ptr->rm_info_ptr)
          { 
            sio_port_id = dpm_um_info_ptr->rm_info_ptr->sio_port_id;                           
          }    
        
          ps_dpm_hw_unbridge_bearer(bearer_ptr->uid, sio_port_id);         
          bearer_ptr->is_bridged = FALSE;
        }
      } /* for loop for bearer*/
    
      /*---------------------------------------------------------------------
        DPM UM is no longer bridged.
      ---------------------------------------------------------------------*/        
      dpm_um_info_ptr->is_bridged_with_hw = FALSE;
    }
  }
} /* ps_dpm_suspend_hw_path_per_um */

static void ps_dpm_suspend_hw_path_cmd_hdlr(void)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;
  int32                      num_dpm_um_info_ptr = 0;
  int16                      dpm_um_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);                  
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  
  /*-------------------------------------------------------------------------
    Go through all non null dpm um entries and suspend hardware path for 
    the same
  -------------------------------------------------------------------------*/
  for (dpm_um_index = 0;
       dpm_um_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_um_info_ptr < ps_dpm_global_info.num_dpm_um_info_ptr;
       dpm_um_index++)
  {
    if ((NULL != ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index]) &&
        (TRUE == 
          ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index]->is_bridged_with_hw))
    {
      num_dpm_um_info_ptr++;
      dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];     
      ps_dpm_suspend_hw_path_per_um(dpm_um_info_ptr);      
    }
  } /* for loop for dpm um entries*/

  disable_hw_bridge_mode = PS_DPM_DISABLE_HW_PATH_SYS_IOCTL;
   
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);    
} /* ps_dpm_suspend_hw_path_cmd_hdlr */

static void ps_dpm_suspend_hw_path_per_pdn_cmd_hdlr
(
  ps_dpm_suspend_hw_path_per_pdn_info_type  * suspend_hw_path_per_pdn_info_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr = NULL;
  ps_iface_type            * iface_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section); 
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  
  do
  {
    if (NULL == suspend_hw_path_per_pdn_info_ptr)
    {
      LOG_MSG_ERROR_0("ps_dpm_suspend_hw_path_per_pdn_cmd_hdlr() : "
                      "Invalid NULL argument");  
      break;                    
    }
    
    iface_ptr = suspend_hw_path_per_pdn_info_ptr->iface_ptr;
    
    if (!PS_IFACE_IS_VALID(iface_ptr))
    {
      LOG_MSG_ERROR_1("ps_dpm_suspend_hw_path_per_pdn_cmd_hdlr() : "
                      "Invalid iface 0x%p", iface_ptr);  
      break;                     
    }
    
    dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(iface_ptr);
    
    if (NULL != dpm_um_info_ptr)
    {
      ps_dpm_suspend_hw_path_per_um(dpm_um_info_ptr);
    }
    else
    {
      LOG_MSG_INFO1_1("ps_dpm_suspend_hw_path_per_pdn_cmd_hdlr() : "
                      "No dpm entry found for iface 0x%p", iface_ptr);
    }
  } while(0);
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);    
  
} /* ps_dpm_suspend_hw_path_per_pdn_cmd_hdlr */

static void ps_dpmi_cmd_hdlr
(
  ps_rm_cmd_enum_type     cmd,
  void                  * user_data_ptr
)
{
  ps_dpm_cmd_info_type  * cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if (PS_RM_DPM_CMD != cmd || NULL == user_data_ptr)
    {
      LOG_MSG_ERROR_2("ps_dpmi_cmd_hdlr(): Invalid arg, cmd %d user data 0x%p",
                      cmd, user_data_ptr);
      break;
    }

    cmd_info_ptr = (ps_dpm_cmd_info_type *) user_data_ptr;

    switch (cmd_info_ptr->dpm_cmd)
    {
      case PS_DPM_UM_REG_BEARER_CMD:
      {
        ps_dpmi_um_reg_bearer_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.um_reg_bearer_info)
        );

        break;
      }

      case PS_DPM_UM_REG_IFACE_CMD:
      {
        ps_dpmi_um_reg_iface_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.um_reg_iface_info)
        );

        break;
      }

      case PS_DPM_UM_REREG_BEARER_CMD:
      {
        ps_dpmi_um_rereg_bearer_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.um_rereg_bearer_info)
        );

        break;
      }

      case PS_DPM_RM_REG_IFACE_CMD:
      {
        ps_dpmi_rm_reg_iface_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.rm_reg_iface_info)
        );

        break;
      }
      
      case PS_DPM_HANDOFF_SUCCESS_EV_CMD:
      {
        ps_dpmi_handoff_success_ev_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.handoff_success_ev_info)
        );

        break;
      }      

      case PS_DPM_PREFIX_UPDATED_EV_CMD:
      {
        ps_dpmi_prefix_updated_ev_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.prefix_updated_ev_info)
        );

        break;
      }      

      case PS_DPM_UM_DEREG_BEARER_CMD:
      {
        ps_dpmi_um_dereg_bearer_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.um_dereg_bearer_info)
        );

        break;
      }

      case PS_DPM_UM_DEREG_IFACE_CMD:
      {
        ps_dpmi_um_dereg_iface_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.um_dereg_iface_info)
        );

        break;
      }

      case PS_DPM_RM_DEREG_IFACE_CMD:
      {
        ps_dpmi_rm_dereg_iface_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.rm_dereg_iface_info)
        );

        break;
      }

      case PS_DPM_RM_CONFIG_ETH_HDR_CMD:
      {
        ps_dpmi_rm_config_eth_hdr_cmd_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.rm_eth_hdr_info)
        );

        break;
      }

      case PS_DPM_UM_REG_TLB_BEARER_CMD:
      {        
        ps_dpmi_um_reg_tlb_bearer_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.um_reg_tlb_bearer_info)
        );

        break;
      }    

      case PS_DPM_SUSPEND_HW_PATH_CMD:
      {        
        ps_dpm_suspend_hw_path_cmd_hdlr();
        break;
      } 

      case PS_DPM_RESUME_HW_PATH_CMD:
      {        
        ps_dpm_resume_hw_path_cmd_hdlr();
        break;
      } 

      case PS_DPM_SUSPEND_HW_PATH_PER_PDN_CMD:
      {        
        ps_dpm_suspend_hw_path_per_pdn_cmd_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.suspend_hw_path_per_pdn_info)
        );        
        break;
      } 

      case PS_DPM_RESUME_HW_PATH_PER_PDN_CMD:
      {        
        ps_dpm_resume_hw_path_per_pdn_cmd_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.resume_hw_path_per_pdn_info)
        );        
        break;
      }       

      case PS_DPM_HW_TO_PS_WMK_GONE_EMPTY_CMD:
      {        
        ps_dpm_hw_to_ps_wmk_gone_empty_cmd_hdlr
        (
          cmd_info_ptr->dpm_cmd_info.hw_to_ps_wmk_empty_info
        );

        break;
      } 

      case PS_DPM_SIO_TX_WMK_GONE_EMPTY_CMD:
      {        
        ps_dpm_sio_tx_wmk_gone_empty_cmd_hdlr
        (
          cmd_info_ptr->dpm_cmd_info.sio_tx_wmk_empty_info
        );

        break;
      }      

      #ifdef FEATURE_DATA_PS_464XLAT
      case PS_DPM_CLAT_PDN_CONTEXT_EV_CMD:
      { 			 
        ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.clat_pdn_cntxt_ev_info)
        );
        
        break;
      } 

      case PS_DPM_CLAT_GLOBAL_CONTEXT_EV_CMD:
      {        
        ps_dpm_clati_reg_global_cntxt_cmd_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.clat_global_context_ev_info)
        );

        break;
      } 

      case PS_DPM_CLAT_PDN_REREG_EV_CMD:
      {        
        ps_dpm_clati_rereg_pdn_cntxt_cmd_hdlr
        (
          &(cmd_info_ptr->dpm_cmd_info.clat_pdn_cntxt_rereg_ev_info)
        );

        break;
      } 			
      #endif /* FEATURE_DATA_PS_464XLAT */			
			
      default:
      {
        LOG_MSG_ERROR_0("ps_dpmi_cmd_hdlr(): Unknown cmd");
        break;
      }
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE(user_data_ptr);

  return;
} /* ps_dpmi_cmd_hdlr() */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void ps_dpm_init
(
  void
)
{
  int32  ul_sig_index;
  int32  dl_sig_index;
  uint8  disable_hw_bridge_flag = FALSE;
  uint8  index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ps_dpm_global_info, 0, sizeof(ps_dpm_global_info_type));

  PS_INIT_CRIT_SECTION(&ps_dpm_crit_section);

  ps_rm_set_cmd_handler(PS_RM_DPM_CMD, ps_dpmi_cmd_hdlr);

  for (ul_sig_index = 0; ul_sig_index < PS_DPM_UL_MAX_SIGNALS; ul_sig_index++)
  {
    ps_dpm_ul_sig_in_use_tbl[ul_sig_index] = FALSE;
  }

  for (dl_sig_index = 0; dl_sig_index < PS_DPM_DL_MAX_SIGNALS; dl_sig_index++)
  {
    ps_dpm_dl_sig_in_use_tbl[dl_sig_index] = FALSE;
  }

  for(index = 0; index < PS_DPM_MAX_UID; index++)
  {
    ps_dpm_uids_inuse[index] = FALSE;
  }  

  ps_dpm_ul_legacy_init();

  ps_dpm_ul_opt_init();
  
  ps_dpm_mbim_ul_opt_init();

  /*------------------------------------------------------------------------
    Initialize dpl dpm logging
  -------------------------------------------------------------------------*/
  ps_dpm_dpl_init();

  ps_handle_mgr_init_client(PS_HANDLE_MGR_CLIENT_DPM_UM,
                            PS_DPM_MAX_PDN_SESSIONS,
                            0,
                            0x7FFF);

  ps_handle_mgr_init_client(PS_HANDLE_MGR_CLIENT_DPM_RM,
                            PS_DPM_MAX_PDN_SESSIONS,
                            0,
                            0x7FFF);

  ps_handle_mgr_init_client(PS_HANDLE_MGR_CLIENT_DPM_BEARER,
                            PS_DPM_MAX_BEARERS,
                            0,
                            0x7FFF);

  ps_dpm_hw_reg_event_cb();

  /*-------------------------------------------------------------------------
    Read NV item to see if bridging is supported
  -------------------------------------------------------------------------*/
  if (ps_dpm_cfg_get(PS_DPM_CFG_DISABLE_HW_BRIDGING,
                    (unsigned char*) &disable_hw_bridge_flag,
                    sizeof(uint8)) == -1)
  {
    LOG_MSG_ERROR_0("ps_dpm_init Cannot get Hardware bridging preference. "
                    "Bridge allowed");
  }

  if (1 == disable_hw_bridge_flag)
  {
    disable_hw_bridge_mode = PS_DPM_DISABLE_HW_PATH_NV;
  }
  
  ps_dpmi_global_stats_init();

#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */

} /* ps_dpm_init() */


ps_dpm_rm_info_type * ps_dpm_get_dpm_rm_info_by_handle
(
  int32  handle
)
{
  ps_dpm_rm_info_type   * dpm_rm_info_ptr;
  int16                   dpm_rm_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  dpm_rm_index = ps_handle_mgr_get_index(PS_HANDLE_MGR_CLIENT_DPM_RM, handle);
  
  if (dpm_rm_index >= PS_DPM_MAX_PDN_SESSIONS || dpm_rm_index < 0)
  {
    LOG_MSG_ERROR_1("ps_dpm_get_dpm_rm_info_by_handle(): "
                    "Invalid DPM rm index 0x%x", dpm_rm_index);
    return NULL;  
  }  

  dpm_rm_info_ptr = ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index];
  if (NULL == dpm_rm_info_ptr || handle != dpm_rm_info_ptr->handle)
  {
    LOG_MSG_ERROR_1("ps_dpm_get_dpm_rm_info_by_handle(): "
                    "Invalid DPM Rm handle 0x%x", handle);
    dpm_rm_info_ptr = NULL;
  }

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);

  return dpm_rm_info_ptr;
} /* ps_dpm_get_dpm_rm_info_by_handle() */


ps_dpm_rm_info_type * ps_dpm_get_dpm_rm_info_by_iface
(
  ps_iface_type  * rm_iface_ptr
)
{
  ps_dpm_rm_info_type  * dpm_rm_info_ptr;
  int16                  dpm_rm_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);

  /*-------------------------------------------------------------------------
    Check if any entry exists for the input Um iface
  -------------------------------------------------------------------------*/
  for (dpm_rm_index = 0;
       dpm_rm_index < PS_DPM_MAX_PDN_SESSIONS;
       dpm_rm_index++)
  {
    if (NULL != ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index])
    {
      dpm_rm_info_ptr = ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index];

      if (dpm_rm_info_ptr->v4_iface_ptr == rm_iface_ptr ||
          dpm_rm_info_ptr->v6_iface_ptr == rm_iface_ptr)
      {
        PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
        return dpm_rm_info_ptr;
      }
    }
  }

  LOG_MSG_ERROR_0("ps_dpm_get_dpm_rm_info_by_iface(): "
                  "Couldn't find DPM Rm info");

  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);

  return NULL;
} /* ps_dpm_get_dpm_rm_info_by_iface() */


void ps_dpm_um_ev_cback_reg
(
  ps_sys_tech_enum_type         tech_type,
  ps_dpm_um_ev_cback_fptr_type  ev_cback_fptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_SYS_TECH_ALL <= tech_type)
  {
    LOG_MSG_ERROR_1("ps_dpm_um_ev_cback_reg(): Invalid tech %d", tech_type);
    return;
  }

  ps_dpm_global_um_ev_cback[tech_type] = ev_cback_fptr;

  return;
} /* ps_dpm_um_ev_cback_reg() */


int32 ps_dpm_um_reg_bearer
(
  ps_dpm_um_reg_bearer_info_type  * um_reg_bearer_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == um_reg_bearer_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_reg_bearer(): NULL arg");
    return -1;
  }

  if ((NULL == um_reg_bearer_info_ptr->v4_iface_ptr &&
        NULL == um_reg_bearer_info_ptr->v6_iface_ptr) ||
      NULL == um_reg_bearer_info_ptr->ps_phys_link_ptr ||
      NULL == um_reg_bearer_info_ptr->l2_to_ps_wm_ptr ||
      PS_SYS_RAT_MAX <= um_reg_bearer_info_ptr->rat)
  {
    LOG_MSG_ERROR_5("ps_dpm_um_reg_bearer(): Invalid arg, "
                    "v4 Um iface 0x%p v6 Um iface 0x%p "
                    "phys link 0x%p l2_to_ps WM 0x%p rat %d",
                    um_reg_bearer_info_ptr->v4_iface_ptr,
                    um_reg_bearer_info_ptr->v6_iface_ptr,
                    um_reg_bearer_info_ptr->ps_phys_link_ptr,
                    um_reg_bearer_info_ptr->l2_to_ps_wm_ptr,
                    um_reg_bearer_info_ptr->rat);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_UM_REG_BEARER_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.um_reg_bearer_info),
          sizeof(ps_dpm_um_reg_bearer_info_type),
          um_reg_bearer_info_ptr,
          sizeof(ps_dpm_um_reg_bearer_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return 0;
} /* ps_dpm_um_reg_bearer() */


int32 ps_dpm_um_reg_tlb_bearer
(
  ps_dpm_um_reg_tlb_bearer_info_type  * um_reg_tlb_bearer_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == um_reg_tlb_bearer_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_reg_tlb_bearer(): NULL arg");
    return -1;
  }

  if ((NULL == um_reg_tlb_bearer_info_ptr->v4_iface_ptr &&
        NULL == um_reg_tlb_bearer_info_ptr->v6_iface_ptr) ||
      NULL == um_reg_tlb_bearer_info_ptr->ps_phys_link_ptr ||
      NULL == um_reg_tlb_bearer_info_ptr->l2_to_ps_wm_ptr ||
      NULL == um_reg_tlb_bearer_info_ptr->hw_to_ps_wm_ptr ||
      PS_SYS_RAT_MAX <= um_reg_tlb_bearer_info_ptr->rat)
  {
    LOG_MSG_ERROR_6("ps_dpm_um_reg_tlb_bearer(): Invalid arg, "
                    "v4 Um iface 0x%p v6 Um iface 0x%p "
                    "phys link 0x%p l2_to_ps WM 0x%p rat %d"
                    "hw_to_ps_wm_ptr 0x%p",
                    um_reg_tlb_bearer_info_ptr->v4_iface_ptr,
                    um_reg_tlb_bearer_info_ptr->v6_iface_ptr,
                    um_reg_tlb_bearer_info_ptr->ps_phys_link_ptr,
                    um_reg_tlb_bearer_info_ptr->l2_to_ps_wm_ptr,
                    um_reg_tlb_bearer_info_ptr->rat,
                    um_reg_tlb_bearer_info_ptr->hw_to_ps_wm_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_UM_REG_TLB_BEARER_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.um_reg_tlb_bearer_info),
          sizeof(ps_dpm_um_reg_tlb_bearer_info_type),
          um_reg_tlb_bearer_info_ptr,
          sizeof(ps_dpm_um_reg_tlb_bearer_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return 0;
} /* ps_dpm_um_reg_tlb_bearer() */


int32 ps_dpm_um_reg_iface
(
  ps_dpm_um_reg_iface_info_type  * um_reg_iface_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == um_reg_iface_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_reg_iface(): NULL arg");
    return -1;
  }

  if (NULL == um_reg_iface_info_ptr->registered_iface_ptr ||
      NULL == um_reg_iface_info_ptr->iface_ptr)
  {
    LOG_MSG_ERROR_2("ps_dpm_um_reg_iface(): Invalid arg, "
                    "registered Um iface 0x%p new Um iface 0x%p",
                    um_reg_iface_info_ptr->registered_iface_ptr,
                    um_reg_iface_info_ptr->iface_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_UM_REG_IFACE_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.um_reg_iface_info),
          sizeof(ps_dpm_um_reg_iface_info_type),
          um_reg_iface_info_ptr,
          sizeof(ps_dpm_um_reg_iface_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return 0;
} /* ps_dpm_um_reg_iface() */


int32 ps_dpm_um_rereg_bearer
(
  ps_dpm_um_rereg_bearer_info_type  * um_rereg_bearer_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == um_rereg_bearer_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_rereg_bearer(): NULL arg");
    return -1;
  }

  if ((NULL == um_rereg_bearer_info_ptr->v4_iface_ptr &&
        NULL == um_rereg_bearer_info_ptr->v6_iface_ptr) ||
      NULL == um_rereg_bearer_info_ptr->ps_phys_link_ptr ||
      NULL == um_rereg_bearer_info_ptr->l2_to_ps_wm_ptr ||
      PS_SYS_RAT_MAX <= um_rereg_bearer_info_ptr->rat)
  {
    LOG_MSG_ERROR_5("ps_dpm_um_rereg_bearer(): Invalid arg, "
                    "v4 Um iface 0x%p v6 Um iface 0x%p "
                    "phys link 0x%p l2_to_ps WM 0x%p rat %d",
                    um_rereg_bearer_info_ptr->v4_iface_ptr,
                    um_rereg_bearer_info_ptr->v6_iface_ptr,
                    um_rereg_bearer_info_ptr->ps_phys_link_ptr,
                    um_rereg_bearer_info_ptr->l2_to_ps_wm_ptr,
                    um_rereg_bearer_info_ptr->rat);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_UM_REREG_BEARER_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.um_rereg_bearer_info),
          sizeof(ps_dpm_um_rereg_bearer_info_type),
          um_rereg_bearer_info_ptr,
          sizeof(ps_dpm_um_rereg_bearer_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return 0;
} /* ps_dpm_um_rereg_bearer() */


int32 ps_dpm_rm_reg_iface
(
  ps_dpm_rm_reg_iface_info_type  * rm_reg_iface_info_ptr,
  int16                          * ps_errno
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    DPM RM Reg has to be called in DCC task since DCC task is blocked
    till DPM is done with registration
  -------------------------------------------------------------------------*/
  #ifndef TEST_FRAMEWORK
    ASSERT(rex_dcc_tcb == rex_self());
  #endif /* TEST_FRAMEWORK */
  
  if (NULL == rm_reg_iface_info_ptr || NULL == ps_errno)
  {
    LOG_MSG_ERROR_0("ps_dpm_rm_reg_iface(): NULL arg");
    return -1;
  }

  if (NULL == rm_reg_iface_info_ptr->rm_iface_ptr ||
      NULL == rm_reg_iface_info_ptr->sio_rx_wm_ptr ||
      NULL == rm_reg_iface_info_ptr->sio_tx_wm_ptr ||
      (RMNET_ETHERNET_MODE != rm_reg_iface_info_ptr->link_prot &&
       RMNET_IP_MODE != rm_reg_iface_info_ptr->link_prot))
  {
    LOG_MSG_ERROR_4("ps_dpm_rm_reg_iface(): Invalid arg, Rm iface 0x%p "
                    "SIO Rx WM 0x%p SIO Tx WM 0x%p link prot %d",
                    rm_reg_iface_info_ptr->rm_iface_ptr,
                    rm_reg_iface_info_ptr->sio_rx_wm_ptr,
                    rm_reg_iface_info_ptr->sio_tx_wm_ptr,
                    rm_reg_iface_info_ptr->link_prot);
    *ps_errno = DS_EFAULT;                    
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    *ps_errno = DS_NOMEMORY;
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_RM_REG_IFACE_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.rm_reg_iface_info),
          sizeof(ps_dpm_rm_reg_iface_info_type),
          rm_reg_iface_info_ptr,
          sizeof(ps_dpm_rm_reg_iface_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);
  
  /*-------------------------------------------------------------------
    DPM blocks calling task in ps_dpm_rm_reg_iface so that DPM
    is completed with registration before calling DCC task can go ahead
    with further processing. 
  -------------------------------------------------------------------*/   
  
  LOG_MSG_INFO1_0("ps_dpm_rm_reg_iface(): Waiting for DPM to complete " 
                  "registration ..");  
                  
  ps_dpmi_rm_wait_for_cback();
  
  LOG_MSG_INFO1_1("ps_dpm_rm_reg_iface(): DPM completed with process of " 
                  "registration .. status %d", ps_dpm_status_rm_reg_iface);     
                  
  if (0 != ps_dpm_status_rm_reg_iface)
  {
    *ps_errno = DS_EINVAL;
    return -1;
  }

  return 0;
} /* ps_dpm_rm_reg_iface() */


int32 ps_dpm_um_dereg_bearer
(
  ps_dpm_um_dereg_bearer_info_type  * um_dereg_bearer_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    DPM UM DeReg has to be called in DS task since DS task is blocked
    till DPM is done with deregistration
  -------------------------------------------------------------------------*/
  #ifndef TEST_FRAMEWORK
  if(dsi_get_ds_tcb() != rex_self())
  {
    LOG_MSG_ERROR_0("ps_dpm_rm_reg_iface(): DS TCB pointer does not match");
    ASSERT(0);
    return -1;
  }
  #endif /* TEST_FRAMEWORK */

  if (NULL == um_dereg_bearer_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_dereg_bearer(): NULL arg ");
    return -1;
  }

  if ((NULL == um_dereg_bearer_info_ptr->v4_iface_ptr &&
        NULL == um_dereg_bearer_info_ptr->v6_iface_ptr) ||
      NULL == um_dereg_bearer_info_ptr->ps_phys_link_ptr)
  {
    LOG_MSG_ERROR_3("ps_dpm_um_dereg_bearer(): Invalid arg, "
                    "v4 Um iface 0x%p v6 Um iface 0x%p phys link 0x%p",
                    um_dereg_bearer_info_ptr->v4_iface_ptr,
                    um_dereg_bearer_info_ptr->v6_iface_ptr,
                    um_dereg_bearer_info_ptr->ps_phys_link_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_UM_DEREG_BEARER_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.um_dereg_bearer_info),
          sizeof(ps_dpm_um_dereg_bearer_info_type),
          um_dereg_bearer_info_ptr,
          sizeof(ps_dpm_um_dereg_bearer_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  /*-------------------------------------------------------------------
    DPM blocks calling task in ps_dpm_um_reg_bearer so that DPM
    is completed with deregistration before calling DS task can go ahead
    with further processing. 
  -------------------------------------------------------------------*/   
  
  LOG_MSG_INFO1_0("ps_dpm_um_dereg_bearer(): Waiting for DPM to complete " 
                  "deregistration ..");  
                  
  ps_dpmi_um_wait_for_cback();
  
  LOG_MSG_INFO1_0("ps_dpm_um_dereg_bearer(): DPM completed with process of " 
                  "deregistration ..");   

  return 0;
} /* ps_dpm_um_dereg_bearer() */


int32 ps_dpm_um_dereg_iface
(
  ps_dpm_um_dereg_iface_info_type  * um_dereg_iface_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    DPM UM DeReg has to be called in DS task since DS task is blocked
    till DPM is done with deregistration
  -------------------------------------------------------------------------*/
  #ifndef TEST_FRAMEWORK
  if(dsi_get_ds_tcb() != rex_self())
  {
    LOG_MSG_ERROR_0("ps_dpm_um_dereg_iface(): DS TCB pointer does not match");
    ASSERT(0);
    return -1;
  }
  #endif /* TEST_FRAMEWORK */
  
  if (NULL == um_dereg_iface_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_um_dereg_iface(): NULL arg");
    return -1;
  }

  if (NULL == um_dereg_iface_info_ptr->v4_iface_ptr &&
      NULL == um_dereg_iface_info_ptr->v6_iface_ptr)
  {
    LOG_MSG_ERROR_2("ps_dpm_um_dereg_iface(): Invalid arg, "
                    "v4 Um iface 0x%p v6 Um iface 0x%p",
                    um_dereg_iface_info_ptr->v4_iface_ptr,
                    um_dereg_iface_info_ptr->v6_iface_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_UM_DEREG_IFACE_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.um_dereg_iface_info),
          sizeof(ps_dpm_um_dereg_iface_info_type),
          um_dereg_iface_info_ptr,
          sizeof(ps_dpm_um_dereg_iface_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  LOG_MSG_INFO1_0("ps_dpm_um_dereg_iface(): Waiting for DPM to complete " 
                  "deregistration ..");  
                  
  ps_dpmi_um_wait_for_cback();
  
  LOG_MSG_INFO1_0("ps_dpm_um_dereg_iface(): DPM completed with process of " 
                  "deregistration ..");    

  return 0;
} /* ps_dpm_um_dereg_iface() */


int32 ps_dpm_rm_dereg_iface
(
  ps_dpm_rm_dereg_iface_info_type  * rm_dereg_iface_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    DPM RM Dereg has to be called in DCC task since DCC task is blocked
    till DPM is done deregistratino
  -------------------------------------------------------------------------*/
  #ifndef TEST_FRAMEWORK
    ASSERT(rex_dcc_tcb == rex_self());
  #endif /* TEST_FRAMEWORK */
  
  if (NULL == rm_dereg_iface_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_rm_dereg_iface(): NULL arg");
    return -1;
  }

  if (NULL == rm_dereg_iface_info_ptr->rm_iface_ptr)
  {
    LOG_MSG_ERROR_1("ps_dpm_rm_dereg_iface(): Invalid arg, Rm iface 0x%p",
                    rm_dereg_iface_info_ptr->rm_iface_ptr);
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                            ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_RM_DEREG_IFACE_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.rm_dereg_iface_info),
          sizeof(ps_dpm_rm_dereg_iface_info_type),
          rm_dereg_iface_info_ptr,
          sizeof(ps_dpm_rm_dereg_iface_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);
  
  /*-------------------------------------------------------------------
    DPM blocks calling task in ps_dpm_rm_dereg_iface so that DPM
    is completed with deregisteration before calling task can go ahead
    with further processing. 
  -------------------------------------------------------------------*/   
  
  LOG_MSG_INFO1_0("ps_dpm_rm_dereg_iface(): Waiting for DPM to complete " 
                  "deregistration ..");  
                  
  ps_dpmi_rm_wait_for_cback();
  
  LOG_MSG_INFO1_0("ps_dpm_rm_dereg_iface(): DPM completed with process of " 
                  "deregistration ..");   

  return 0;
} /* ps_dpm_rm_dereg_iface() */


int32 ps_dpm_rm_config_eth_hdr
(
  ps_dpm_rm_eth_hdr_info_type  * eth_hdr_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == eth_hdr_info_ptr || NULL == eth_hdr_info_ptr->rm_iface_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_rm_config_eth_hdr(): NULL arg");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                            ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return -1;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_RM_CONFIG_ETH_HDR_CMD;

  memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.rm_eth_hdr_info),
          sizeof(ps_dpm_rm_eth_hdr_info_type),
          eth_hdr_info_ptr,
          sizeof(ps_dpm_rm_eth_hdr_info_type));

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return 0;
} /* ps_dpm_rm_config_eth_hdr() */

void ps_dpm_rm_enable_all_flows
(
  void
)
{
  ps_dpm_rm_info_type  * dpm_rm_info_ptr;
  int32                  num_dpm_rm_info_ptr = 0;
  int16                  dpm_rm_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Loop through all DPM RMs and enable the data flow by setting the signal.
    This function is called from ps_dpm_legacy_opt.c where once legacy wm
    goes from flow disabled to flow enabled state, it has to flow enable
    all the wm on DPM RM
  -------------------------------------------------------------------------*/
  for (dpm_rm_index = 0;
       dpm_rm_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_rm_info_ptr < ps_dpm_global_info.num_dpm_rm_info_ptr;
       dpm_rm_index++)
  {
    if (NULL != ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index])
    {
      num_dpm_rm_info_ptr++;

      dpm_rm_info_ptr = ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index];

      PS_RM_SET_SIGNAL(dpm_rm_info_ptr->dpm_ul_sig);
    }
  }
} /* ps_dpm_rm_enable_all_flows */

void ps_dpm_suspend_hw_path
(
  void
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_dpm_suspend_hw_path() : Suspending hardware data path");
  
  /*-------------------------------------------------------------------------
    Allocate DPM cmd buf, populate it and post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                            ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_SUSPEND_HW_PATH_CMD;

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpm_suspend_hw_path() */

void ps_dpm_suspend_hw_path_per_pdn
(
  ps_iface_type  * iface_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ps_dpm_suspend_hw_path_per_pdn() : Suspending hardware "
                  "data path iface 0x%p", iface_ptr);
  
  /*-------------------------------------------------------------------------
    Allocate DPM cmd buf, populate it and post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                           ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_SUSPEND_HW_PATH_PER_PDN_CMD;
  dpm_cmd_info_ptr-> \
    dpm_cmd_info.suspend_hw_path_per_pdn_info.iface_ptr = iface_ptr;

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpm_suspend_hw_path_per_pdn() */


void ps_dpm_resume_hw_path
(
  void
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_dpm_resume_hw_path() : Resuming hardware data path");
  /*-------------------------------------------------------------------------
    Allocate DPM cmd buf, populate it and post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                            ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_RESUME_HW_PATH_CMD;

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpm_resume_hw_path() */

void ps_dpm_resume_hw_path_per_pdn
(
  ps_iface_type * iface_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_1("ps_dpm_resume_hw_path_per_pdn() : Resuming hardware data "
                  "path iface 0x%p", iface_ptr);
  /*-------------------------------------------------------------------------
    Allocate DPM cmd buf, populate it and post cmd to PS_RM task
  -------------------------------------------------------------------------*/
  PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                            ps_dpm_cmd_info_type*);
  if (NULL == dpm_cmd_info_ptr)
  {
    return;
  }

  dpm_cmd_info_ptr->dpm_cmd = PS_DPM_RESUME_HW_PATH_PER_PDN_CMD;

  dpm_cmd_info_ptr-> \
    dpm_cmd_info.resume_hw_path_per_pdn_info.iface_ptr = iface_ptr;

  ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);

  return;
} /* ps_dpm_resume_hw_path_per_pdn() */

int ps_dpm_is_data_path_software
(
  ps_iface_type * iface_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_1("ps_dpm_is_data_path_software() : iface_ptr 0x%p ", iface_ptr);

  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_0("ps_dpm_is_data_path_software() : Invalid iface");
    return -1;  
  }
  
  dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(iface_ptr);
  
  if (NULL == dpm_um_info_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_is_data_path_software() : Call not up");
    return -1;
  }
  else
  {
    return !(dpm_um_info_ptr->is_bridged_with_hw);
  }  
  
} /* ps_dpm_is_data_path_software() */

 ps_iface_type * ps_dpm_get_companion_iface
(
  ps_iface_type  * um_iface_ptr
)
{
  ps_dpm_um_info_type   * dpm_um_info_ptr    = NULL;
  ps_iface_type         * comp_iface_ptr     = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    * 3GPP and 3GPP2 register with DPM so companion iface can be retrieved from DPM UM info.
 -------------------------------------------------------------------------*/ 
  dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(um_iface_ptr);
  
  if ( NULL != dpm_um_info_ptr)
  {
    if (um_iface_ptr == dpm_um_info_ptr->v4_iface_ptr)
    {
      comp_iface_ptr = dpm_um_info_ptr->v6_iface_ptr;
    }
    else  if ( um_iface_ptr == dpm_um_info_ptr->v6_iface_ptr )
    {
      comp_iface_ptr = dpm_um_info_ptr->v4_iface_ptr;
    }
  }
  
  LOG_MSG_INFO2_3("ps_dpm_get_companion_iface(): "
                  "Iface ptr 0x%x comp iface ptr 0x%x  um info ptr 0x%x",
                  um_iface_ptr, comp_iface_ptr, dpm_um_info_ptr );

  return comp_iface_ptr;
}/*ps_dpm_get_companion_iface*/
	

boolean ps_dpm_check_dpm_um_info_exist
(
  ps_iface_type * iface_ptr
)
{
  ps_dpm_um_info_type      * dpm_um_info_ptr;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dpm_um_info_ptr = ps_dpmi_get_dpm_um_info(iface_ptr);
  
  if (NULL == dpm_um_info_ptr)
  {
    return FALSE;
  }
  
  return TRUE;
} /* ps_dpm_check_dpm_um_info_exist */

void ps_dpm_control_dpl_hw_logging
(
  uint32         pkt_len
)
{
  ps_dpm_rm_info_type      * dpm_rm_info_ptr;
  int32                      num_dpm_rm_info_ptr = 0;
  int16                      dpm_rm_index;
  ps_dpm_um_info_type      * dpm_um_info_ptr;
  int32                      num_dpm_um_info_ptr = 0;
  int16                      dpm_um_index;  
  ps_dpm_bearer_info_type  * bearer_ptr;
  int16                      bearer_index;  
  ps_iface_type            * rm_iface_ptr = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO1_1("ps_dpm_control_dpl_hw_logging() pkt_len %d", pkt_len);

  /*-------------------------------------------------------------------------
    1. Go through all dpm rm entries and find one for call is up
    2. Register new pkt length with ipa
  -------------------------------------------------------------------------*/  
  for (dpm_rm_index = 0;
       dpm_rm_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_rm_info_ptr < ps_dpm_global_info.num_dpm_rm_info_ptr;
       dpm_rm_index++)
  {
    if (NULL != ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index])
    {
      num_dpm_rm_info_ptr++;

      dpm_rm_info_ptr = ps_dpm_global_info.dpm_rm_info_ptr_arr[dpm_rm_index];
      
      if (NULL != dpm_rm_info_ptr->v4_iface_ptr)
      {
        rm_iface_ptr = dpm_rm_info_ptr->v4_iface_ptr;                           
      }
      
      if (NULL != dpm_rm_info_ptr->v6_iface_ptr)
      {
        rm_iface_ptr = dpm_rm_info_ptr->v6_iface_ptr;
      }  

      if (NULL != rm_iface_ptr) 
      {
        ps_dpm_hw_config_dpl_sio
        (
           dpm_rm_info_ptr->sio_stream_id,
           rm_iface_ptr->dpl_net_cb.recv_dpl_id.ifname,
           (uint16)pkt_len
        );
      }
    }
  }  
  
  /*-------------------------------------------------------------------------
    1. Go through all dpm um entries and find one for call is up
    2. For each bearer, update config with ipa
  -------------------------------------------------------------------------*/
  for (dpm_um_index = 0;
       dpm_um_index < PS_DPM_MAX_PDN_SESSIONS &&
         num_dpm_um_info_ptr < ps_dpm_global_info.num_dpm_um_info_ptr;
       dpm_um_index++)
  {
    if (NULL != ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index])
    {
      num_dpm_um_info_ptr++;

      dpm_um_info_ptr = ps_dpm_global_info.dpm_um_info_ptr_arr[dpm_um_index];
      
      for (bearer_index = 0; bearer_index < PS_DPM_MAX_BEARERS; bearer_index++)
      {
        bearer_ptr = dpm_um_info_ptr->bearer_ptr_arr[bearer_index];
        if (NULL != bearer_ptr)
        {  
          if (0 != pkt_len)
          {          
            (void)ps_dpmi_config_dpl_with_hw(bearer_ptr->uid,dpm_um_info_ptr);  
          }
          else
          {
            ps_dpm_hw_dereg_dpl(bearer_ptr->uid);
          }          
        }
      }
      
    }
  } 
  
} /* ps_dpm_control_dpl_hw_logging */

boolean ps_dpm_data_path_switch_allowed
(
  ps_dpm_disable_hw_path_enum_type requested_mode
)
{
  boolean result_code = TRUE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    NV has the highest precedence, Once HW path is disabled by NV then 
    it can't be controlled by IOCTL.
    precedence order is NV > SYS IOCTL > IFACE IOCTL
  -------------------------------------------------------------------------*/
  
  switch (disable_hw_bridge_mode)
  {
    case PS_DPM_DISABLE_HW_PATH_NV:
    {
      result_code = FALSE;
      break;
    }

    case PS_DPM_DISABLE_HW_PATH_SYS_IOCTL:
    {
      if (requested_mode > PS_DPM_DISABLE_HW_PATH_SYS_IOCTL)
      {
        result_code = FALSE;
      }
      break;
    }

    case PS_DPM_DISABLE_HW_PATH_NONE:
    case PS_DPM_DISABLE_HW_PATH_IFACE_IOCTL:
    {
      break;
    }
    
    default:
    {
      result_code = FALSE;
      break;
    }
  }

  LOG_MSG_INFO2_3("ps_dpm_data_path_switch_allowed() :"
                  " requested mode %d current mode %d result %d", 
                  requested_mode, disable_hw_bridge_mode, result_code);
  
  return result_code;

}/*ps_dpm_data_path_switch_allowed()*/

int16 ps_dpm_ps_sys_ioctl_ex_hdlr
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                           *arg_val_ptr,
  ps_sys_subscription_enum_type   subscription_id,
  int16                          *ps_errno
)
{
  ps_sys_ioctl_global_stats_type    global_stats;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *ps_errno = DS_ENOERR;
  memset(&global_stats,0x00,sizeof(ps_sys_ioctl_global_stats_type));
  
  if (PS_SYS_IOCTL_GET_GLOBAL_STATS != ioctl_name)
  {
    *ps_errno = DS_EOPNOTSUPP;
  }
  if( arg_val_ptr == NULL )
  {
    *ps_errno = DS_EINVAL;	
  }
  if( subscription_id >= PS_SYS_SUBS_MAX || 
      subscription_id <= PS_SYS_DEFAULT_SUBS )
  {
    *ps_errno = DS_EFAULT;
  } 
  if( *ps_errno != DS_ENOERR )
  {
    LOG_MSG_ERROR_1("ps_dpm_ps_sys_ioctl_ex_hdlr:ps_errno = %d",*ps_errno);
    return -1;
  }
  /* retrieve packet count from global info */
  ((ps_sys_ioctl_global_stats_type*)(arg_val_ptr))->global_pkt_cnt = 
      ps_dpm_global_info.dpm_global_stats[subscription_id-1].global_stats.global_pkt_cnt;
  return 0;
}/* ps_dpm_ps_sys_ioctl_ex_hdlr */

void ps_dpmi_global_stats_init
(
  void
)
{
  ps_sys_subscription_enum_type subscription_id = PS_SYS_PRIMARY_SUBS;
  int16                         ps_errno;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Registering ps_sys_ioctl_ex API for all the subscriptions */
  for(;subscription_id < PS_SYS_SUBS_MAX; subscription_id++)
  {
    if(ps_sys_ioctl_register_ioctl_cback_ex(PS_SYS_IOCTL_GET_GLOBAL_STATS,
                                         ps_dpm_ps_sys_ioctl_ex_hdlr,
                                         subscription_id,
                                         &ps_errno) == -1)
    {
      LOG_MSG_ERROR_2("ps_dpmi_global_stats_init: PS sys IOCTL handler reg" 
                      "failed with cause %d for Subs id(PS):%d",
                       ps_errno,
                       subscription_id);
    }
  }
}/* ps_dpmi_init_global_stats */

void ps_dpm_data_activity_client_status_ind
(
  ps_sys_subscription_enum_type subscription_id,
  boolean                       client_status
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( subscription_id >= PS_SYS_PRIMARY_SUBS &&
      subscription_id < PS_SYS_SUBS_MAX )
  {
    /*----------------------------------------------------------------------
        1.reg_clnt_cnt is maintained to send PS_SYS_EVENT_DATA_ACTIVITY_RESUMED event
           when the count is >= 1.
        2.Each time when a client is registered this count is incremented
        3.When a client is deregistered this count will be decremented if current count is not 0.
       -----------------------------------------------------------------------*/
    if( client_status == TRUE )
    {
      ps_dpm_global_info.dpm_global_stats[subscription_id-1].reg_clnt_cnt++;
    }
    else
    {
      /*safe check: dont decrement if the count is already zero*/
      if( ps_dpm_global_info.dpm_global_stats[subscription_id-1].reg_clnt_cnt != 0 )
      {
        ps_dpm_global_info.dpm_global_stats[subscription_id-1].reg_clnt_cnt--;
      }
    }
    LOG_MSG_INFO1_3("ps_dpm_data_activity_client_status_ind"
                    "client status %d client count %d subscription_id %d",
                    client_status,
                    ps_dpm_global_info.dpm_global_stats[subscription_id-1].reg_clnt_cnt,
                    subscription_id);
  }
}/* ps_dpm_data_activity_client_status_ind */

void ps_dpm_global_stats_inc
(
  ps_iface_type       * iface_ptr
)
{
  
  int16                         ps_errno;
  boolean                       data_activity_flag   = TRUE;/*to send as event info*/
  ps_sys_subscription_enum_type subs_id =
       (ps_sys_subscription_enum_type)iface_ptr->iface_private.subs_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(subs_id > PS_SYS_DEFAULT_SUBS && subs_id < PS_SYS_SUBS_MAX )
  {
    /*increment the packet count on particular subscription*/
    ps_dpm_global_info.dpm_global_stats[subs_id-1].global_stats.global_pkt_cnt++;
    /*-------------------------------------------------------------------------
     1.If registered client count is >= 1 send PS_SYS_EVENT_DATA_ACTIVITY_RESUMED to 
       all regisred clients.
     2.Make sure no events are sent for further packets by initializing reg_clnt_cnt to zero.
     3.reg_clnt_cnt is incremented next time when a client is registered again.
       --------------------------------------------------------------------------*/
    if( ps_dpm_global_info.dpm_global_stats[subs_id-1].reg_clnt_cnt )
    {
      ps_dpm_global_info.dpm_global_stats[subs_id-1].reg_clnt_cnt = 0;
      if( ps_sys_event_ind_ex(PS_SYS_TECH_ALL,
                          PS_SYS_EVENT_DATA_ACTIVITY_RESUMED,
                          subs_id,
                          &data_activity_flag, /*to comply with ps_sys layer */
                          &ps_errno) == -1 )
      {
        LOG_MSG_ERROR_1("ps_dpm_global_stats_inc:"
              "Error in sending PS_SYS_EVENT_DATA_ACTIVITY_RESUMED ps_errno: %d",
               ps_errno);
      }
    }
  }
}

/**
  @brief  This would be invoked by HW when continuous error packet count
             reaches threshold (or) when recieved valid packet after recovery..
             DPM gets the phys_link_ptr with the uid and invokes MH callback to
             report HFN failure/success.

  @param[in] uid:                      UID of the bearer
  @param[in] resync_status:
             resync_status would be false when UE recieved threshold number
             of invalid packets continously.
             resync_status would be true when UE recieved first valid packet after
             threshold number of invlid packets.
  
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_cont_err_pkt_ind_handler
(
  uint8      uid,
  boolean    resync_status
)
{
  ps_phys_link_type* phys_link_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(uid >= PS_DPM_MAX_UID)
  {
    LOG_MSG_ERROR_1("ps_dpm_cont_err_pkt_ind_handler():"
                     "resync_status %d uid %d",uid);
    return;
  }
  /* get phys_link_ptr from UID */
  phys_link_ptr = ps_dpmi_get_phys_link_from_uid(uid);
  if( phys_link_ptr != NULL && ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr != NULL )
  {
    /* invoke MH callback  */
    ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr(phys_link_ptr,resync_status);
  }
}

/**
  @brief  Modehandler will Register cb with dpm to know 
-->Continuous error packet count reached threshold
-->first valid packet after previous failure
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_err_ind_cback_reg
(
  uint32                         err_pkt_threshold,
  ps_dpm_err_ind_cback_fptr_type err_ind_cb_fn_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------
       Validate arguments passed
   -----------------------------------------------------*/
  do{
    if( err_pkt_threshold == 0 || err_ind_cb_fn_ptr == NULL)
    {
      break;
    }
    /*------------------------------------------------------
         dont allow multiple registrations
        -----------------------------------------------------*/
    if( ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr == NULL )
    {
      /*------------------------------------------------------
           Copy to global info
        ---------------------------------------------------------*/
      
      ps_dpm_cont_err_pkt_info.err_pkt_threshold = err_pkt_threshold;
      ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr = err_ind_cb_fn_ptr;
      
      /*-------------------------------------------------
      Register with HW for HFN error indication
      -----------------------------------------------------*/
      ps_dpm_hw_register_bearer_err_ind(err_pkt_threshold,
                                        (void*)ps_dpm_cont_err_pkt_ind_handler);
      return;
    }
  }while(0);
  LOG_MSG_ERROR_3("ps_dpm_err_ind_cback_reg():"
                   "Invalid args %d 0x%p 0x%p", 
                   err_pkt_threshold,
                   err_ind_cb_fn_ptr,
                   ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr );
}

/**
  @brief  Modehandler will deregister cb with dpm when LTE detached.
  @return  N/A

  @code
  @endcode
*/
void ps_dpm_err_ind_cback_dereg
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------
   Deregister only if registered already
  -----------------------------------------------------*/
  if( ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr != NULL )
  {
    /*----------------------------------------------------
    Reset the global info 
    -----------------------------------------------------*/
    ps_dpm_cont_err_pkt_info.err_pkt_threshold = 0;
    ps_dpm_cont_err_pkt_info.err_ind_cb_fn_ptr       = NULL;
   /*----------------------------------------------------
    Deregister with HW
    -----------------------------------------------------*/
    ps_dpm_hw_deregister_bearer_err_ind();
  }
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

//MSR TODO ps_dpm_v4_arp_proxy should belong to ARP in linklayer itself. Not
//sure why it was part of RmNet before. Talk to Vinay and move it - future PL
//MSR TODO Seem to make more sense to define DPM own RAT instead of
//reusing SYS RAT
