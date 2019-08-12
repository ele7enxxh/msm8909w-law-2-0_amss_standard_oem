/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             P S _ D P M . C

GENERAL DESCRIPTION
  This is the implementation of the DPM CLAT module

Copyright (c) 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_clat.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include "ps_dpmi.h"
#include "ps_dpm_clati.h"
#include "ps_rm_svc.h"
#include "ps_in.h"
#include "ps_dpm_hw.h"
#include "ps_iface_ipfltr_hw.h"
#include "ps_iface_ipfltr.h"

#ifdef FEATURE_DATA_PS_464XLAT

/*===========================================================================

                           DEFINES

===========================================================================*/

/*===========================================================================

                           GLOBAL DATA DECLARATIONS

===========================================================================*/
extern ps_crit_sect_type         ps_dpm_crit_section;

/*===========================================================================

                           STATIC DATA DECLARATIONS

===========================================================================*/
ps_dpm_clat_ev_cback_fptr_type ps_dpm_clat_ev_cback_ptr;


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void ps_dpm_clat_ev_cback_reg
(
  ps_dpm_clat_ev_cback_fptr_type  ev_cback_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ev_cback_ptr)
  {
    LOG_MSG_ERROR_0("ps_dpm_clat_ev_cback_reg(): NULL cbak registered");
    return;
  }

  ps_dpm_clat_ev_cback_ptr = ev_cback_ptr;

  return;
} /* ps_dpm_clat_ev_cback_reg() */

int ps_dpm_clat_reg_global_context
(
  ps_dpm_clat_global_cntxt_info_type   * clat_global_cntxt_info_ptr
)
{
  ps_dpm_cmd_info_type	* dpm_cmd_info_ptr;
  ps_dpm_error_enum_type  dpm_error_info = PS_DPM_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if (NULL == clat_global_cntxt_info_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_ARG_INVALID;
      break;
    }

    /*-------------------------------------------------------------------------
      Post cmd to PS_RM task
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                             ps_dpm_cmd_info_type*);
    if (NULL == dpm_cmd_info_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_MEM_ALLOC_FAILED;
      break;
    }

    dpm_cmd_info_ptr->dpm_cmd = PS_DPM_CLAT_GLOBAL_CONTEXT_EV_CMD;

    memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.clat_global_context_ev_info),
            sizeof(ps_dpm_clat_global_cntxt_info_type),
            clat_global_cntxt_info_ptr,
            sizeof(ps_dpm_clat_global_cntxt_info_type));

    ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);
    
    return 0;
  } while(0);

  LOG_MSG_ERROR_1("ps_dpm_clat_reg_global_context() : DPM Err %d", dpm_error_info);
  return -1;
} /* ps_dpm_clat_reg_global_context */

int ps_dpm_clat_reg_pdn_context
(
  ps_dpm_clat_reg_pdn_cntxt_info_type     * cntxt_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
  ps_dpm_error_enum_type  dpm_error_info = PS_DPM_SUCCESS; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if (NULL == cntxt_info_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_ARG_INVALID;
      break;
    }

    /*-------------------------------------------------------------------------
      Post cmd to PS_RM task
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                             ps_dpm_cmd_info_type*);
    if (NULL == dpm_cmd_info_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_MEM_ALLOC_FAILED;
      break;
    }

    dpm_cmd_info_ptr->dpm_cmd = PS_DPM_CLAT_PDN_CONTEXT_EV_CMD;
      
    memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.clat_pdn_cntxt_ev_info),
            sizeof(ps_dpm_clat_reg_pdn_cntxt_info_type),
            cntxt_info_ptr,
            sizeof(ps_dpm_clat_reg_pdn_cntxt_info_type));    

    ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);
    return 0;
  } while(0);
  
  LOG_MSG_ERROR_1("ps_dpm_clat_reg_pdn_context() : DPM Err %d", dpm_error_info);
  return -1;
} /* ps_dpm_clat_reg_pdn_context */

int ps_dpm_clat_rereg_pdn_context
(
  ps_dpm_clat_reg_pdn_cntxt_info_type       * cntxt_info_ptr
)
{
  ps_dpm_cmd_info_type  * dpm_cmd_info_ptr;
  ps_dpm_error_enum_type  dpm_error_info = PS_DPM_SUCCESS;   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    if (NULL == cntxt_info_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_ARG_INVALID;
      break;
    }

    /*-------------------------------------------------------------------------
      Post cmd to PS_RM task
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(dpm_cmd_info_ptr, sizeof(ps_dpm_cmd_info_type),
                             ps_dpm_cmd_info_type*);
    if (NULL == dpm_cmd_info_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_MEM_ALLOC_FAILED;
      break;
    }

    dpm_cmd_info_ptr->dpm_cmd = PS_DPM_CLAT_PDN_REREG_EV_CMD;

    memscpy(&(dpm_cmd_info_ptr->dpm_cmd_info.clat_pdn_cntxt_rereg_ev_info),
            sizeof(ps_dpm_clat_reg_pdn_cntxt_info_type),
            cntxt_info_ptr,
            sizeof(ps_dpm_clat_reg_pdn_cntxt_info_type)); 

    ps_rm_send_cmd(PS_RM_DPM_CMD, dpm_cmd_info_ptr);
    return 0;
  } while(0);
  
  LOG_MSG_ERROR_1("ps_dpm_clat_rereg_pdn_context() : DPM Err %d", dpm_error_info);
  return -1;
} /* ps_dpm_clat_rereg_pdn_context */


int ps_dpm_clati_reg_global_cntxt_cmd_hdlr
(
  ps_dpm_clat_global_cntxt_info_type  * clat_global_cntxt_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_3("ps_dpm_clati_reg_global_cntxt_cmd_hdlr():clat global context: " 
                  "subnet mask 0x%x, tos action %d, tos value %d",
                  clat_global_cntxt_info->v4_client_subnet_mask,
                  clat_global_cntxt_info->tos_ignore_bit_flag,
                  clat_global_cntxt_info->tos_override_val);
                               
  /* Configure the CLAT global info with HW */
  return ps_dpm_hw_clat_reg_global_context(clat_global_cntxt_info);  
  
} /* ps_dpm_clati_reg_global_cntxt_cmd_hdlr */

int ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr
(
  ps_dpm_clat_reg_pdn_cntxt_info_type   * clat_reg_info_ptr
)
{
  ps_iface_type                         * um_base_iface_ptr = NULL;
  int32                                   result = -1;
  int32                                   hw_result = -1;
  void                                  * hw_pdn_handle = NULL;
  ps_dpm_error_enum_type                  dpm_error_info = PS_DPM_SUCCESS;
  ps_dpm_clat_pdn_cntxt_info_type       * clat_pdn_cntxt = NULL;  
  ps_dpm_clat_ev_enum_type                clat_event = 
                                            PS_DPM_CLAT_EV_PDN_CONTEXT_REGISTERED;   
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_5("ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr(): iface_ptr 0x%x:%d, "
                  "v4 addr %d,v6_dev_prefix_len %d,v6_plat_prefix_len %d", 
                  PS_IFACE_IS_VALID(clat_reg_info_ptr->iface_ptr)?
                    clat_reg_info_ptr->iface_ptr->name:0,
                  PS_IFACE_IS_VALID(clat_reg_info_ptr->iface_ptr)?
                    clat_reg_info_ptr->iface_ptr->instance:0,                  
                  clat_reg_info_ptr->v4_client_addr,
                  clat_reg_info_ptr->v6_dev_prefix_len,
                  clat_reg_info_ptr->v6_plat_prefix_len);
                    
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);
  
  do
  {
    if (NULL == clat_reg_info_ptr->iface_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_ARG_INVALID;
      break;
    }
  
    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(clat_reg_info_ptr->iface_ptr);
    if (!PS_IFACE_IS_VALID(um_base_iface_ptr))
    {
      dpm_error_info = PS_DPM_ERROR_IFACE_INVALID;
      break;
    }  
  
    /*-------------------------------------------------------------------------
      Allocate memory for clat context
      Move it to dpm structure for 9x45. It is currently being stored in
      iface because 3gpp2 doesnt register with DPM
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(clat_pdn_cntxt, 
                             sizeof(ps_dpm_clat_pdn_cntxt_info_type),
                             ps_dpm_clat_pdn_cntxt_info_type*);
    if (NULL == clat_pdn_cntxt)
    {
      dpm_error_info = PS_DPM_ERROR_MEM_ALLOC_FAILED;
      break;
    }
  
    clat_pdn_cntxt->v4_client_addr = 
      clat_reg_info_ptr->v4_client_addr;
    clat_pdn_cntxt->v6_dev_prefix_len = 
      clat_reg_info_ptr->v6_dev_prefix_len;
    clat_pdn_cntxt->v6_plat_prefix_len = 
      clat_reg_info_ptr->v6_plat_prefix_len;
    clat_pdn_cntxt->client_pdn_handle = 
      clat_reg_info_ptr->client_pdn_handle;      
    clat_pdn_cntxt->v6_dev_addr_prefix_len = 
      clat_reg_info_ptr->v6_dev_addr_prefix_len;   
    clat_pdn_cntxt->clat_iface_ptr = 
      clat_reg_info_ptr->iface_ptr;      
    
    memscpy(&(clat_pdn_cntxt->v6_dev_prefix),
            sizeof(struct ps_in6_addr),
            &(clat_reg_info_ptr->v6_dev_prefix),
            sizeof(struct ps_in6_addr));
            
    memscpy(&(clat_pdn_cntxt->v6_plat_prefix),
            sizeof(struct ps_in6_addr),
            &(clat_reg_info_ptr->v6_plat_prefix),
            sizeof(struct ps_in6_addr));   
            
    memscpy(&(clat_pdn_cntxt->v6_dev_addr),
            sizeof(struct ps_in6_addr),
            &(clat_reg_info_ptr->v6_dev_addr),
            sizeof(struct ps_in6_addr));            
            
    IPV6_ADDR_MSG(clat_reg_info_ptr->v6_dev_prefix.ps_s6_addr64);
    IPV6_ADDR_MSG(clat_reg_info_ptr->v6_plat_prefix.ps_s6_addr64);   
    IPV6_ADDR_MSG(clat_reg_info_ptr->v6_dev_addr.ps_s6_addr64);     

    /* Send the pdn context to hardware */
    hw_result = ps_dpm_hw_clat_reg_pdn_context(clat_reg_info_ptr, &hw_pdn_handle);
    if (0 != hw_result)
    {
      LOG_MSG_INFO1_0("ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr() : "
                      "Adding filters as hardware clat reg failed");
      clat_pdn_cntxt->hw_v6_addr_handle = 
        ps_iface_ipfltr_add_clat_v6_dst_fltr(clat_reg_info_ptr->iface_ptr, 
                                            &(clat_reg_info_ptr->v6_dev_addr),
                                            clat_reg_info_ptr->v6_dev_addr_prefix_len); 
      if (NULL == clat_pdn_cntxt->hw_v6_addr_handle)
      {
        dpm_error_info = PS_DPM_ERROR_HARDWARE_OP_FAILED;
        break;
      }                                            
    }
    
    clat_pdn_cntxt->hw_pdn_handle = hw_pdn_handle;

    um_base_iface_ptr->clat_pdn_cntxt = (void *)clat_pdn_cntxt;
   
    result = 0;

  } while(0);

  if (PS_DPM_SUCCESS != dpm_error_info)
  {
    PS_SYSTEM_HEAP_MEM_FREE(clat_pdn_cntxt);
    if (PS_IFACE_IS_VALID(um_base_iface_ptr))
    {
      um_base_iface_ptr->clat_pdn_cntxt = NULL;  
    }
    
    clat_event = PS_DPM_CLAT_EV_PDN_CONTEXT_REGISTRATION_FAILED;
  }
  
  /* invoke the callback */
  if (NULL != ps_dpm_clat_ev_cback_ptr)
  {
    ps_dpm_clat_ev_cback_ptr(clat_event,
                             clat_reg_info_ptr->client_pdn_handle);
  }
  else
  {
    LOG_MSG_ERROR_0("ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr() : "
                    "No callback registered by clat client");
  }
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section); 
  
  LOG_MSG_INFO1_1("ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr() : DPM Err %d", 
                   dpm_error_info);  
  
  return result;
} /* ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr */


int ps_dpm_clati_rereg_pdn_cntxt_cmd_hdlr
(
  ps_dpm_clat_reg_pdn_cntxt_info_type   * clat_rereg_info_ptr
)
{
  ps_iface_type                         * um_base_iface_ptr = NULL;
  int32                                   result = -1;
  int                                     hw_result;
  ps_dpm_error_enum_type                  dpm_error_info = PS_DPM_SUCCESS; 
  ps_dpm_clat_pdn_cntxt_info_type       * clat_pdn_cntxt = NULL;   
  ps_dpm_clat_ev_enum_type                clat_event = 
                                            PS_DPM_CLAT_EV_PDN_CONTEXT_REREGISTERED;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_5("ps_dpm_clati_rereg_pdn_cntxt_cmd_hdlr(): iface_ptr 0x%x:%d, "
                  "v4 addr %d,v6_dev_prefix_len %d,v6_plat_prefix_len %d", 
                  PS_IFACE_IS_VALID(clat_rereg_info_ptr->iface_ptr)?
                    clat_rereg_info_ptr->iface_ptr->name:0,
                  PS_IFACE_IS_VALID(clat_rereg_info_ptr->iface_ptr)?
                    clat_rereg_info_ptr->iface_ptr->instance:0,
                  clat_rereg_info_ptr->v4_client_addr,
                  clat_rereg_info_ptr->v6_dev_prefix_len,
                  clat_rereg_info_ptr->v6_plat_prefix_len);
                    
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  PS_ENTER_CRIT_SECTION(&ps_dpm_crit_section);        
  
  do
  {
    if (NULL == clat_rereg_info_ptr->iface_ptr)
    {
      dpm_error_info = PS_DPM_ERROR_ARG_INVALID;
      break;
    }
  
    um_base_iface_ptr = PS_DPM_GET_BASE_IFACE(clat_rereg_info_ptr->iface_ptr);
    if (!PS_IFACE_IS_VALID(um_base_iface_ptr))
    {
      dpm_error_info = PS_DPM_ERROR_IFACE_INVALID;
      break;
    }
    
    if (NULL == um_base_iface_ptr->clat_pdn_cntxt)
    {
      dpm_error_info = PS_DPM_ERROR_CLAT_CNTXT_NULL;
      break;
    }    
  
    clat_pdn_cntxt = 
      (ps_dpm_clat_pdn_cntxt_info_type *)um_base_iface_ptr->clat_pdn_cntxt;
      
    clat_pdn_cntxt->v4_client_addr = 
      clat_rereg_info_ptr->v4_client_addr;
    clat_pdn_cntxt->v6_dev_prefix_len = 
      clat_rereg_info_ptr->v6_dev_prefix_len;
    clat_pdn_cntxt->v6_plat_prefix_len = 
      clat_rereg_info_ptr->v6_plat_prefix_len;
    clat_pdn_cntxt->client_pdn_handle = 
      clat_rereg_info_ptr->client_pdn_handle;
    
    memscpy(&(clat_pdn_cntxt->v6_dev_prefix),
            sizeof(struct ps_in6_addr),
            &(clat_rereg_info_ptr->v6_dev_prefix),
            sizeof(struct ps_in6_addr));
            
    memscpy(&(clat_pdn_cntxt->v6_plat_prefix),
            sizeof(struct ps_in6_addr),
            &(clat_rereg_info_ptr->v6_plat_prefix),
            sizeof(struct ps_in6_addr));   

    IPV6_ADDR_MSG(clat_rereg_info_ptr->v6_dev_prefix.ps_s6_addr64);
    IPV6_ADDR_MSG(clat_rereg_info_ptr->v6_plat_prefix.ps_s6_addr64);    

    /* Send the pdn context to hardware */                                        
    hw_result = ps_dpm_hw_clat_rereg_pdn_context
               (
                 clat_rereg_info_ptr, 
                 clat_pdn_cntxt->hw_pdn_handle
               );  
    if (0 != hw_result)
    {
      LOG_MSG_ERROR_0("ps_dpm_clati_rereg_pdn_cntxt_cmd_hdlr() : "
                      "Hardware registration failed");

      if (NULL != clat_pdn_cntxt->hw_v6_addr_handle)
      {
        ps_iface_ipfltr_delete_v6_dst_fltr
        (
          clat_pdn_cntxt->hw_v6_addr_handle
        );  
      }  
      
      clat_pdn_cntxt->hw_v6_addr_handle = 
        ps_iface_ipfltr_add_clat_v6_dst_fltr(clat_rereg_info_ptr->iface_ptr, 
                                             &(clat_rereg_info_ptr->v6_dev_addr),
                                             clat_rereg_info_ptr->v6_dev_addr_prefix_len);    
      if (NULL == clat_pdn_cntxt->hw_v6_addr_handle)
      {
        dpm_error_info = PS_DPM_ERROR_HARDWARE_OP_FAILED;
        break;
      }      
    }                                        
    
    result = 0;

  } while(0);

  if (PS_DPM_SUCCESS != dpm_error_info)
  {    
    clat_event = PS_DPM_CLAT_EV_PDN_CONTEXT_REGISTRATION_FAILED;
  }
  
  /* invoke the callback */
  if (NULL != ps_dpm_clat_ev_cback_ptr)
  {
    ps_dpm_clat_ev_cback_ptr(clat_event,
                             clat_rereg_info_ptr->client_pdn_handle);
  }
  else
  {
    LOG_MSG_ERROR_0("ps_dpm_clati_rereg_pdn_cntxt_cmd_hdlr() : "
                    "No callback registered by clat client");
  }  
  
  PS_LEAVE_CRIT_SECTION(&ps_dpm_crit_section);
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  
  LOG_MSG_INFO1_1("ps_dpm_clati_reg_pdn_cntxt_cmd_hdlr() : DPM Err %d", 
                   dpm_error_info);    
  
  return result;
} /* ps_dpm_clati_rereg_pdn_cntxt_cmd_hdlr */
#endif /* FEATURE_DATA_PS_464XLAT */