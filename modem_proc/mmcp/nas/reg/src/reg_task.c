/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_task.c_v   1.4   06 Jun 2002 10:45:38   kabraham  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_task.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/01   sbk     reg_cmd_type is used to pass the cmd to various routines
08/07/01   ts      Standardized usage of MSG_HIGH and MSG_ERROR.
08/16/01   ab      Renamed mmi_reg.h references to reg_cm.  Renamed MMI to CM.
09/18/01   sbk     Added dog support by creating the reg_wait function. 
                   Fixed task startup problems by moving around initialization code
                   Initialize the GHDI IF for NV by calling ghdi_nvmem_register_task fn
10/31/01   jca     Replaced redundant constant PLMN_ID_LEN with PLMN_SIZE.
02/20/02   ATM     Merge GS_GSM and GS functions together
03/13/02   ATM     Added support for new GS_SEND_MESSAGE
04/13/02   kwa     Updated to support dual mode requirements.
05/16/02   kwa     Replaced MSG_ERROR with ERR_FATAL in reg_cm_get_cmd_buf if 
                   CM attempts to allocate a buffer for an unknown message.
05/31/02   kwa     Removed reg_wait from static function declarations.
09/19/02   kwa     Added cm_act_req to reg_cm_get_cmd_buf.
05/22/03   kwa     Modified reg_send_routine to pass the msg_p parameter as
                   a const pointer to eliminate a lint warning.
02/02/04   kwa     Added an additional error check for a NULL buffer pointer
                   in reg_cm_get_cmd_buf to eliminate a LINT warning.
03/05/04   kwa     Removed call to reg_mode_initialize_equivalent_plmn_list.
09/21/05   up      Updated reg_cm_get_cmd_buf for message CM_HPLMN_SEARCH_REQ.
08/14/06   pk      Updated to add CFA log for CM->REG message.
10/10/11   HC      Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                   Replaced the std_strlen with C equivalent function strlen
11/15/11  abhi   For EFRPLMNSI Selecting RLPMN if HPLMN is not found
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include <string.h>
#include <stringl/stringl.h>
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "err.h"
#include "dog_hb_rex.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "ms.h"
#include "gs_v.h"
#include "reg_mode.h"
#include "reg_sim_v.h"
#include "reg_state.h"
#include "reg_timers.h"
#include "reg_task.h"
#include "reg_task_int.h"
#include "ghdi_nvmem.h"
#include "ghdi_nvmem_v.h"
#include "ghdi_exp_int.h"
#include "sys.h"
#include "sys_v.h"
#include "cfa_v.h"
#include "reg_nv.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "fs_public.h"
#include "fs_lib.h"
#include "sys_eplmn_list_v.h"
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#include "policyman.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
#define NAS_REG_CONF_FILE "/nv/item_files/conf/reg.conf"

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
rex_crit_sect_type  reg_sim_crit_sect;
extern uint8           reg_sim_max_subscription;
#endif

nas_crit_sect_type reg_hplmn_crit_sect;
nas_crit_sect_type reg_ehplmn_crit_sect;

#ifdef FEATURE_TDSCDMA
nas_crit_sect_type reg_tdscdma_op_list_crit_sect;
#endif
rex_tcb_type* reg_tcb_ptr;

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*===========================================================================

FUNCTION NAME

  reg_initialize

DESCRIPTION

  Function that initializes REG static data during task startup.

RETURN VALUE

  None

===========================================================================*/

void reg_initialize
(
  void
);


/*===========================================================================

FUNCTION NAME

  reg_nvmem_default_values

DESCRIPTION

  Function that provides default values for items read from NVRAM.

RETURN VALUE

  ghdi_nvmem_status_T

===========================================================================*/

ghdi_nvmem_status_T reg_nvmem_default_values
(
  ghdi_nvmem_status_T  in_status, 
  nv_items_enum_type   item_type, 
  nv_item_type*        nv_item
);


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*
** Command queue for the REG task
*/
q_type reg_cmd_q;                                       

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_create_efs_config

==============================================================================*/
void reg_create_efs_config( void)
{
  int32 nas_conf_fd;

  /* **********************
  **Do not any indentation when adding new EFS path
  ***************************/

  char *buff = 
"/nv/item_files/modem/nas/ignore_uplmn\n"\
"/nv/item_files/modem/nas/ignore_uplmn_Subscription01\n"\
"/nv/item_files/modem/nas/ignore_uplmn_Subscription02\n"\
"/nv/item_files/modem/nas/imsi_switch\n"\
"/nv/item_files/modem/nas/ehplmn\n"\
"/nv/item_files/modem/nas/ehplmn_Subscription01\n"\
"/nv/item_files/modem/nas/ehplmn1\n"\
"/nv/item_files/modem/nas/ehplmn2\n"\
"/nv/item_files/modem/nas/efrplmnsi_select_rplmn_after_hplmn\n"\
"/nv/item_files/modem/nas/forced_irat\n"\
"/nv/item_files/modem/nas/forced_irat_Subscription01\n"\
"/nv/item_files/modem/nas/tdscdma_op_plmn_list\n"\
"/nv/item_files/modem/nas/tdscdma_op_plmn_list_Subscription01\n"\
"/nv/item_files/modem/nas/max_validate_sim_counter\n"\
"/nv/item_files/modem/nas/t3245_timer\n"\
"/nv/item_files/modem/nas/t3245_timer_test\n"\
"/nv/item_files/modem/nas/efnas_config\n"\
"/nv/item_files/modem/nas/lpm_power_off\n"\
"/nv/item_files/modem/nas/reg_nv_items\n"\
"/nv/item_files/modem/nas/reg_nv_items_Subscription0\n"\
"/nv/item_files/modem/nas/vplmn\n"\
"/nv/item_files/modem/nas/vpmln_Subscription01\n"\
"/nv/item_files/modem/nas/max_validate_sim_counter_Subscription01\n"\
"/nv/item_files/modem/nas/max_validate_sim_counter_Subscription02\n"\
"/nv/item_files/modem/nas/t3245_timer_Subscription01\n"\
"/nv/item_files/modem/nas/t3245_timer_Subscription02\n"\
"/nv/item_files/modem/nas/t3245_timer_test_Subscription01\n"\
"/nv/item_files/modem/nas/t3245_timer_test_Subscription02\n"\
"/nv/item_files/modem/nas/efnas_config_Subscription02\n"\
"/nv/item_files/modem/nas/efnas_config_Subscription01\n"\
"/nv/item_files/modem/nas/ehplmn3\n"\
"/nv/item_files/modem/nas/ehplmn4\n"\
"/nv/item_files/modem/nas/ehplmn5\n"\
"/nv/item_files/modem/nas/lte_avaialable_cell_list\n"\
"/nv/item_files/modem/nas/lte_non_avaialable_cell_list\n"\
"/nv/item_files/modem/nas/data_roam_status\n"\
"/nv/item_files/modem/nas/data_roam_status_Subscription01\n"\
"/nv/item_files/modem/nas/data_roam_status_Subscription02\n"\
"/nv/item_files/modem/nas/lte_available_tg_cell_list\n"\
"/nv/item_files/modem/nas/lte_non_available_tg_cell_list\n"\
"/nv/item_files/modem/nas/lte_available_timer_values\n"\
"/nv/item_files/modem/nas/lte_available_timer_values_Subscription01\n"\
"/nv/item_files/modem/nas/preferred_plmn_list\n"\
"/nv/item_files/modem/nas/preferred_plmn_list1\n"\
"/nv/item_files/modem/nas/preferred_plmn_list2\n"\
"/nv/item_files/modem/nas/preferred_plmn_list3\n"\
"/nv/item_files/modem/nas/preferred_plmn_list4\n"\
"/nv/item_files/modem/nas/preferred_plmn_list5\n"\
"/nv/item_files/modem/nas/preferred_plmn\n"\
"/nv/item_files/modem/nas/preferred_plmn1\n"\
"/nv/item_files/modem/nas/preferred_plmn2\n"\
"/nv/item_files/modem/nas/preferred_plmn3\n"\
"/nv/item_files/modem/nas/preferred_plmn4\n"\
"/nv/item_files/modem/nas/preferred_plmn5\n\
";

  struct fs_stat  nas_conf_stat;
  
  if (efs_stat("/nv", &nas_conf_stat) == -1 )
  {
    (void)efs_mkdir("/nv", 0777);
    (void)efs_mkdir("/nv/item_files", 0777);    
    (void)efs_mkdir("/nv/item_files/conf", 0777); 
  }
  else if (efs_stat("/nv/item_files", &nas_conf_stat) == -1 )
  {
    (void)efs_mkdir("/nv/item_files", 0777);    
    (void)efs_mkdir("/nv/item_files/conf", 0777); 
  }
  else if (efs_stat("/nv/item_files/conf", &nas_conf_stat) == -1 )
  {
    (void)efs_mkdir("/nv/item_files/conf", 0777); 
  }
    
  /* Create reg conf file if it does not exists */
  if(efs_stat(NAS_REG_CONF_FILE, &nas_conf_stat) < 0 )
  {
    nas_conf_fd = efs_creat(NAS_REG_CONF_FILE, 0777);
    
    if( nas_conf_fd >= 0 )
    {
      (void)efs_write(nas_conf_fd, buff,(uint32)strlen(buff));
    }
    else
    {
      MSG_ERROR_DS_0(REG_SUB, "=REG= EFS CONF file NOT created");
    }
    (void)efs_close(nas_conf_fd);  
  }
}

/*==============================================================================

FUNCTION NAME

  reg_get_cmd_buf

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
 #error code not present
#endif
reg_cmd_type *reg_get_cmd_buf
(
  void
)
{
#ifndef FEATURE_MODEM_HEAP
  return (reg_cmd_type*)gs_alloc(sizeof(reg_cmd_type));
#else
  return (reg_cmd_type*)modem_mem_calloc(1,sizeof(reg_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif 

/*==============================================================================

FUNCTION NAME

  reg_cm_get_cmd_buf

==============================================================================*/

reg_cmd_type* reg_cm_get_cmd_buf
(
  cm_to_reg_cmd_type cmd_type
)
{
#ifndef FEATURE_MODEM_HEAP
  reg_cmd_type* cmd = (reg_cmd_type *)gs_alloc(sizeof(reg_cmd_type));
#else
  reg_cmd_type* cmd = (reg_cmd_type *)modem_mem_calloc(1,sizeof(reg_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
  reg_check_for_null_ptr((void*)cmd);

    cmd->cmd.hdr.message_set = MS_CM_REG;
    cmd->cmd.hdr.message_id  = (byte) cmd_type;
   
    switch(cmd_type)
    {
      case CM_NETWORK_LIST_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_network_list_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_network_list_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_SERVICE_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_service_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_service_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_SIM_AVAILABLE_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_sim_available_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_sim_available_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_SIM_NOT_AVAILABLE_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_sim_not_available_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_sim_not_available_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_STOP_MODE_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_stop_mode_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_stop_mode_req_s_type) - sizeof(IMH_T)) / 256;
        break;

#ifdef FEATURE_GW_ON_DEMAND_HPLMN_SRCH
      case CM_HPLMN_SEARCH_REQ:
        /*lint -e778 */
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_hplmn_search_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_hplmn_search_req_s_type) - sizeof(IMH_T)) / 256;
        /*lint +e778 */
        break;
#endif

      case CM_PH_STATUS_CHGD_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_ph_status_chgd_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_ph_status_chgd_req_s_type) - sizeof(IMH_T)) / 256;
        break;

#ifdef FEATURE_OOSC_USER_ACTION
      case CM_EXIT_FROM_PWR_SAVE:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_exit_from_pwr_save_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_exit_from_pwr_save_s_type) - sizeof(IMH_T)) / 256;
        break;
#endif

#ifdef FEATURE_DUAL_SIM
      case CM_DS_STAT_CHGD_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_ds_stat_chgd_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_ds_stat_chgd_req_s_type) - sizeof(IMH_T)) / 256;
        break;
#endif

#if defined(FEATURE_DUAL_SIM)
      case CM_DS_TUNEAWAY_STATUS_CHANGE_IND:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_ds_tuneaway_stat_chgd_ind_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_ds_tuneaway_stat_chgd_ind_s_type) - sizeof(IMH_T)) / 256;
        break;
#endif

#ifdef FEATURE_LTE
      case CM_PS_DETACH_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_ps_detach_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_ps_detach_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_BLOCK_PLMN_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_block_plmn_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_block_plmn_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_SET_DRX_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_set_drx_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_set_drx_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_GET_DRX_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_get_drx_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_get_drx_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_DATA_ROAMING_IND:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_data_roaming_ind_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_data_roaming_ind_s_type) - sizeof(IMH_T)) / 256;
        break;
#endif

      case CM_SET_HPLMN_IRAT_SEARCH_TIMER_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_set_hplmn_irat_search_timer_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_set_hplmn_irat_search_timer_req_s_type) - sizeof(IMH_T)) / 256;
        break;


      case CM_SET_HPLMN_SEARCH_TIMER_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_set_hplmn_search_timer_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_set_hplmn_search_timer_req_s_type) - sizeof(IMH_T)) / 256;
        break;

      case CM_GET_HPLMN_SEARCH_TIMER_REQ:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_get_hplmn_search_timer_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_get_hplmn_search_timer_req_s_type) - sizeof(IMH_T)) / 256;
        break;
		
#ifdef FEATURE_DUAL_SIM
      case CM_MULTIMODE_SUBS_CHGD_REQ:
        cmd->cmd.hdr.message_len_lsb =
            (sizeof(cm_multimode_subs_chgd_req_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
            (sizeof(cm_multimode_subs_chgd_req_s_type) - sizeof(IMH_T)) / 256;
        break;
#endif
#if defined (FEATURE_SGLTE) || defined (FEATURE_1XSRLTE)||defined (FEATURE_SVLTE_DUAL_SIM)
    case  CM_UE_MODE_IND :
       cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_ue_mode_ind_s_type) - sizeof(IMH_T)) % 256;
      cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_ue_mode_ind_s_type) - sizeof(IMH_T)) / 256;  
     break;
#endif

#ifdef FEATURE_DUAL_DATA
    case  CM_DATA_PRIORITY_IND :
       cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_data_priority_ind_s_type) - sizeof(IMH_T)) % 256;
      cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_data_priority_ind_s_type) - sizeof(IMH_T)) / 256;  
     break;
#endif

#ifdef FEATURE_FEMTO_CSG
    case  CM_CSG_SELECT_CONFIG_UPDATE_IND :
       cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_csg_select_config_update_ind_s_type) - sizeof(IMH_T)) % 256;
      cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_csg_select_config_update_ind_s_type) - sizeof(IMH_T)) / 256;  
     break;
#endif
#ifdef FEATURE_DUAL_SIM
      case CM_DDS_SWITCH_IND:
        cmd->cmd.hdr.message_len_lsb =
            (sizeof(cm_dds_switch_ind_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
            (sizeof(cm_dds_switch_ind_s_type) - sizeof(IMH_T)) / 256;
        break;
#endif

    case  CM_PSM_READY_REQ:
       cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_psm_ready_req_s_type) - sizeof(IMH_T)) % 256;
      cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_psm_ready_req_s_type) - sizeof(IMH_T)) / 256;  
     break;

    case CM_ROAMING_IND:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(cm_roaming_ind_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(cm_roaming_ind_s_type) - sizeof(IMH_T)) / 256;
        break;

      default:
#ifndef FEATURE_MODEM_HEAP
        gs_free(cmd);
#else
        modem_mem_free(cmd, MODEM_MEM_CLIENT_NAS);
#endif
        cmd = NULL;
        MSG_FATAL_DS(REG_SUB, "=REG= Received unknown command %d from CM", (int) cmd_type, 0, 0);
        break;
    }

  return cmd;
}

#ifdef FEATURE_NAS_ECALL
/*==============================================================================

FUNCTION NAME

  reg_ecall_get_cmd_buf

==============================================================================*/
reg_cmd_type* reg_ecall_get_cmd_buf
(
  ecall_to_reg_cmd_type cmd_type
)
{
#ifndef FEATURE_MODEM_HEAP
  reg_cmd_type* cmd = (reg_cmd_type *)gs_alloc(sizeof(reg_cmd_type));
#else
  reg_cmd_type* cmd = (reg_cmd_type *)modem_mem_calloc(1,sizeof(reg_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
  reg_check_for_null_ptr((void*)cmd);
    cmd->cmd.hdr.message_set = MS_ECALL_REG;
    cmd->cmd.hdr.message_id  = (byte) cmd_type;
   
    switch(cmd_type)
    {
      case ECALL_REG_MODE_IND:
        cmd->cmd.hdr.message_len_lsb =
          (sizeof(ecall_reg_mode_ind_s_type) - sizeof(IMH_T)) % 256;
        cmd->cmd.hdr.message_len_msb =
          (sizeof(ecall_reg_mode_ind_s_type) - sizeof(IMH_T)) / 256;
        break;
      default:
#ifndef FEATURE_MODEM_HEAP
        gs_free(cmd);
#else
        modem_mem_free(cmd, MODEM_MEM_CLIENT_NAS);
#endif
        cmd = NULL;
        MSG_FATAL_DS(REG_SUB, "=REG= Received unknown command %d from ECALL", (int) cmd_type, 0, 0);
        break;
  }
  return cmd;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_put_cmd

==============================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
 #error code not present
#endif
void reg_put_cmd
(
  reg_cmd_type* reg_cmd_p
)
{
  /*
  ** Check for Null pointer
  */
  if (reg_cmd_p != NULL)
  {
    if(reg_cmd_p->cmd.hdr.message_set == MS_CM_REG)
    {
      word          message_length;
      /*    Get message length */
      GET_IMH_LEN(message_length, &(reg_cmd_p->cmd)); 
      /*    Add in header size */
      message_length += sizeof(IMH_T);
      /* Log using CFA packet */
      cfa_log_packet( GS_QUEUE_REG, message_length, (byte*)&(reg_cmd_p->cmd) );
    }  
    /*
    ** First initialize the queue link. This is always done
    ** before putting an item in a queue.
    */
    (void)q_link(reg_cmd_p, &(reg_cmd_p->link));

    /*
    ** Put the message on the queue
    */
    q_put(&reg_cmd_q, &reg_cmd_p->link);

    /*
    ** Set the command queue signal for receive task.
    */
    (void)rex_set_sigs(reg_tcb_ptr, REG_CMD_Q_SIG);
  }
  else
  {
    MSG_FATAL_DS(REG_SUB, "=REG= Attempt to put NULL reg_cmd_type on queue", 0, 0, 0);
  }
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION 
#error code not present
#endif

/*==============================================================================

FUNCTION NAME

  reg_nvmem_default_values

==============================================================================*/

ghdi_nvmem_status_T reg_nvmem_default_values
(
  ghdi_nvmem_status_T in_status, 
  nv_items_enum_type  item_type, 
  nv_item_type*       nv_item
)
{
  switch(item_type)
  {
    case NV_UMTS_UE_OP_CLASS_I:
      nv_item->umts_ue_op_class = ue_op_class_a;
      break;
    default:
      return in_status;
  }

  return NV_DONE_S;
}


/*==============================================================================

FUNCTION NAME

  reg_send_routine

==============================================================================*/

gs_status_T reg_send_routine
(
  const void* msg_p
)
{
  uint32 message_length;
  void*  cmd_ptr;

  /*
  ** Get message length
  */
  GET_IMH_LEN(message_length, msg_p);

  /*
  ** Add in header size
  */
  message_length += sizeof(IMH_T);

  /*
  ** Get command buffer from destination task
  */
  if ((cmd_ptr = reg_get_cmd_buf()) != NULL)
  {
    /*
    ** Copy data to command buffer
    */
    memscpy(&((reg_cmd_type*)cmd_ptr)->cmd, sizeof(((reg_cmd_type*)cmd_ptr)->cmd), (byte*)msg_p, message_length);

    /*
    ** Put message in destination task queue
    */
    reg_put_cmd((reg_cmd_type*) cmd_ptr);

    return GS_SUCCESS;
  }

  return GS_FAILURE;
}


/*==============================================================================

FUNCTION NAME

  reg_init_ before_task_start

==============================================================================*/

void reg_init_before_task_start (void)
{
  /*
  ** Initialize command queue
  */
  (void)q_init(&reg_cmd_q);

  /*
  ** Initialize gs message passing for this queue
  */
  if (gs_init_for_send_only(GS_QUEUE_REG,
        (gs_put_handler_ptr)reg_send_routine) != GS_SUCCESS)
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= GS message passing not initialized");
  }

#ifndef FEATURE_MODEM_RCINIT
  /*
  ** Clear signals
  */
  (void) rex_clr_sigs(reg_tcb_ptr, (rex_sigs_type)~0);
#endif

#ifdef FEATURE_LTE
  /* Initialize the Critical Section for REG Task */
  rex_init_crit_sect(&reg_backoff_fplmn_crit_sect);

  /* Set the backoff plmn list lenght to zero*/
  reg_sim_backoff_fplmn_list.length = 0;
#endif

  sys_eplmn_list_init_crit_sect();

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  rex_init_crit_sect(&reg_sim_crit_sect); 
#endif
  NAS_INIT_CRIT_SECT(reg_hplmn_crit_sect);
  NAS_INIT_CRIT_SECT(reg_ehplmn_crit_sect);
#ifdef FEATURE_TDSCDMA
  NAS_INIT_CRIT_SECT(reg_tdscdma_op_list_crit_sect);
#endif 
}


/*==============================================================================

FUNCTION NAME

  reg_wait

==============================================================================*/

rex_sigs_type reg_wait
(
  rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
  rex_sigs_type queue_mask;        /* Mask of signals indicating queue status */
  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */

  /*
  ** Initialize temporary mask holder for queue information
  */
  queue_mask = 0;

  /*
  ** Check message queue:  If the request mask contains a check for
  ** REG_CMD_Q_SIG then we should check the queue, and set in
  ** the queue_mask an indication that something is waiting to be
  ** serviced on that queue.  The rex signal is also cleared
  ** regardless of the queue count.  This is to cover the case
  ** where there is a signal set but nothing on the queue.
  */
  if (requested_mask & REG_CMD_Q_SIG)
  {
    (void) rex_clr_sigs( reg_tcb_ptr, REG_CMD_Q_SIG );

    if (q_cnt( &reg_cmd_q ))
    {
      queue_mask |= REG_CMD_Q_SIG;
    }
  }

  /*
  ** Get current state of rex signals
  */
  rex_signals_mask = rex_get_sigs( reg_tcb_ptr );

  /*
  ** Loop until the current signals from REX (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  */
  do
  {
    /*
    ** Pet the watchdog
    */
    reg_timers_pet_watchdog();

    /*
    ** Perform the 'real' wait (if needed)
    */
    if (((rex_signals_mask | queue_mask) & requested_mask) == 0)
    {
      rex_signals_mask = rex_wait( requested_mask | REG_DOGRPT_TIMER_SIG );
       /* Clear the watchdog signal */
      (void) rex_clr_sigs( reg_tcb_ptr, REG_DOGRPT_TIMER_SIG);

    }

    /*
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue indicated
    */
    if (rex_signals_mask & REG_CMD_Q_SIG)
    {
      (void) rex_clr_sigs( reg_tcb_ptr, REG_CMD_Q_SIG );

      /*
      ** No elements in queue
      */
      if (q_cnt( &reg_cmd_q ) == 0)
      {
        /*
        ** Turn off bit
        */
        rex_signals_mask &= ~REG_CMD_Q_SIG;
      }
    }

  /*
  ** Do while mask requested does not match current mask
  */
  } while (((rex_signals_mask | queue_mask) & requested_mask) == 0);

  return ((rex_signals_mask | queue_mask));
}


/*==============================================================================

FUNCTION NAME

  reg_initilize

==============================================================================*/
void reg_initialize
(
  void
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  reg_as_id_e_type as_id;
  policyman_status_t        status;
  policyman_item_t const    *pItem;
  policyman_item_id_t const id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  size_t                  num_of_sims = 1;
  sys_subs_feature_t  sub_feature_mode = SYS_SUBS_FEATURE_MODE_NORMAL;
  reg_sim_max_subscription = 1;


  status = policyman_get_items_block(&id, 1, &pItem);
  if (POLICYMAN_SUCCEEDED(status))
  {
    status = policyman_device_config_num_sims(pItem, &num_of_sims);
    if (POLICYMAN_SUCCEEDED(status))
    {
      reg_sim_max_subscription = num_of_sims;
    }
    for(as_id = REG_AS_ID_1; (as_id < (reg_as_id_e_type)num_of_sims && as_id < (reg_as_id_e_type)MAX_AS_IDS); as_id++)
    {
      reg_sub_config_sim[as_id] = SYS_OVERALL_FEATURE_MODE_MULTISIM;
      status = policyman_device_config_get_subs_feature(pItem, 0, (size_t)as_id, &sub_feature_mode);
      if(POLICYMAN_SUCCEEDED(status))
      {
        if(sub_feature_mode == SYS_SUBS_FEATURE_MODE_SGLTE)
        {
          reg_sub_config_sim[as_id] = SYS_OVERALL_FEATURE_MODE_SGLTE;
        }
      }
    }
    policyman_item_release(pItem);
  }
  else
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Reading of policy manager failed");
    for(as_id = REG_AS_ID_1; as_id < (reg_as_id_e_type)MAX_AS_IDS; as_id++)
    {
      reg_sub_config_sim[as_id] = SYS_OVERALL_FEATURE_MODE_MULTISIM;
    }
  }

  for(as_id = REG_AS_ID_1; as_id < (reg_as_id_e_type)MAX_NAS_STACKS; as_id++)
  {
    reg_sub_id_stack[as_id] = as_id;
  }

#endif
}

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION REG_MCFG_REFRESH_CB

DESCRIPTION
This is a callback function used by MCFG for NV/EFS refresh purpose
This function puts the received refresh info into REG command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean reg_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
   reg_cmd_type   *cmd_ptr;

   ASSERT(p_info != NULL);

   if(p_info->type != MCFG_REFRESH_TYPE_SUBS)
   {
     MSG_ERROR_DS_1(REG_SUB,"=REG= Unexpected refresh type %d rcvd in reg_mcfg_refresh_cb", p_info->type);
     return FALSE;
   }

   /* Get command buffer from REG task */
   if( (cmd_ptr = reg_get_cmd_buf()) != NULL)
   {
      /* Construct message */
     cmd_ptr->cmd.mcfg_refresh_ind.IMH.message_set = MS_REG_REG;
     cmd_ptr->cmd.mcfg_refresh_ind.IMH.message_id  = REG_MCFG_REFRESH_IND;

     PUT_IMH_LEN (sizeof(cmd_ptr->cmd.mcfg_refresh_ind) - sizeof(IMH_T),
                       &(cmd_ptr->cmd.mcfg_refresh_ind.IMH));

     cmd_ptr->cmd.mcfg_refresh_ind.as_id = (sys_modem_as_id_e_type)p_info->sub_index;
     cmd_ptr->cmd.mcfg_refresh_ind.reference_id = p_info->reference_id;

     /* Put on REG command queue */
     reg_put_cmd(cmd_ptr);
   }
   else
   {
     MSG_FATAL_DS( REG_SUB, "=REG= ERROR: Unable to allocate memory", 0,0,0 );
   }
   return TRUE;
}

/*===========================================================================
FUNCTION REG_MCFG_REFRESH_REGISTER

DESCRIPTION
This function call registers REG Task with MCFG for NV/EFS refresh purpose
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void reg_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type mcfg_reg;

  memset((void *)&mcfg_reg,0x00,sizeof(mcfg_refresh_registration_s_type));

  mcfg_reg.order = MCFG_REFRESH_ORDER_50;  
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.group = 0;
  mcfg_reg.cb = reg_mcfg_refresh_cb;

  if(mcfg_refresh_register(&mcfg_reg) != TRUE)
  {
     MSG_ERROR_DS_0( REG_SUB, "=REG= ERROR: MCFG Registration failed");    
  }
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_main

==============================================================================*/

void reg_main
(
  dword dummy
)
{
  reg_cmd_type* reg_cmd_p;
  static rex_sigs_type sigs;
   
  (void) dummy;

  reg_tcb_ptr = rex_self();

#ifndef FEATURE_MODEM_RCINIT
  reg_init_before_task_start();
#endif

/*
  ** Initialize REG timers
  */
  reg_timers_initialize();

#ifdef FEATURE_MODEM_RCINIT
  rcinit_handshake_startup();
#else
#ifndef FEATURE_MODEM_RCINIT_PHASE2
  tmc_task_start();
#endif
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  reg_mcfg_refresh_register();
#endif

  reg_create_efs_config();

  reg_initialize();
  /*
  ** Register with the NV task
  */
  if (ghdi_nvmem_register_task(REG_NVIF_SIG, reg_wait, reg_nvmem_default_values) != GHDI_SUCCESS)
  {
     MSG_FATAL_DS(REG_SUB, "=REG= Could not register with nvmem task", 0, 0, 0);
  }

  reg_nv_initialize_cache();
  

  /* Initialize cleint IDs for MMGSDI API calls */
  reg_sim_mmgsdi_init();


  /*
  ** Loop forever
  */
  for(;;)
  {
    /*
    ** Wait on signal
    */
    sigs = reg_wait(REG_CMD_Q_SIG 
           #ifdef TEST_FRAMEWORK
                    #error code not present
#endif  
           );

    /*
    ** If this signal is for the command queue
    */
    if (sigs & REG_CMD_Q_SIG)
    {
      /*
      ** Clear signals
      */
      (void) rex_clr_sigs( reg_tcb_ptr,REG_CMD_Q_SIG);

      /*
      ** Read all commands from command queue
      */
      while ((reg_cmd_p = (reg_cmd_type*)q_get(&reg_cmd_q) ) != NULL)
      {
        /*
        ** Send command to main state machine
        */
        reg_state_main(reg_cmd_p); 

        /*
        ** Deallocate command
        */
#ifndef FEATURE_MODEM_HEAP
        gs_free(reg_cmd_p);
#else
        modem_mem_free(reg_cmd_p, MODEM_MEM_CLIENT_NAS);
#endif
      }   
    }
#ifdef TEST_FRAMEWORK
    #error code not present
#endif
  }
}
