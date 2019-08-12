/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmtask.c_v   1.16   13 Jun 2002 15:25:44   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmtask.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/28/12   am      Fixing compilation warnings 
10/22/12   cogolt  CR407246. Disable LSTI feature by default and allow enabling
                   LSTI feature through EFS only
10/01/12   am      Back off timer code
07/05/12   us      Removing static buffers allocated by EMM for msgr & adding dynamic mechanism of 
                    allocating buffers when required.  
05/29/12   pahire  Added changes for UE Specific DRX Cycle for QChat/LTE
10/28/11   rajesh  Added support for reading "nas_lai_change_force_lau_for_emergency" EFS item.
10/10/11   HC      Removed AEEstd.h inclusion as aeestdlib is obsoleted 
                   Replaced the std_strlen with C equivalent function strlen
                   Replaced the STD_OFFSETOF with local definition NAS_STD_OFFSETOF

09/30/11   rajesh  Added support for "exclude_ptmsi_type_field" EFS item.
08/01/11   rajesh Initialized "t3442_value" of emm_ctrl_data_ptr.
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services 
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
02/01/10   PA      Added changes for Security Key generation during L2W PS HO
10/07/10   MNK     1XCSFB introduction 
08/09/10   MNK     Make code compatible for 1xCSFB & 3GPPCSFB
08/30/10   MNK     CSFB changes
07/11/10   zren    Updated to correctly free ESM and RRC cmd
05/21/10   zren    Updated support of handling REG CMD coming during L2GW IRAT
05/17/10   zren    Updated EMM to only process EMM timers
05/07/10   zren    Added temp FPLMN feature
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/09/10   zren    Added combined procedure feature
02/11/10   MNK     Moved LTE NAS EFS read to happen after SIM read is successful
03/03/10   MNK     TIN support 
02/21/10   MNK     Add support for GW to LTE i-RAT
02/16/10   RI      Updates to EFS read ( temporary)
02/16/10   MNK     Initialize tin and ignore_GW_activation_ind
02/12/10   zren    Updated to always abort the existing connection before 
                   sending RRC DEACT REQ
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/22/10   RI      Added detach reason initialization in emm ctrl data.
01/12/10   RI      Updated the EFS read for UE sec capabilities
                   for target.
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/30/09   zren    Modified access barring timeout handling
11/04/09   MNK     Initialize deactivate reason
03/13/02   ATM     Added support for new GS_SEND_MESSAGE
02/20/02   ATM     Merge GS_GSM and GS functions together
04/12/01   jca     Began integration with GMM and new MM_COORD.
05/03/01   jca     Added watchdog support.
05/29/01   sbk     Added RRC_ACT_CNF processing to the mm_rrc_get_cmd_buf
06/13/01   jca     Deleted RRC_UNIT_DATA_IND primitive.
09/18/01   sbk     Added dog support by calling the mm_wait function
                   Fixed task startup problems by fixing init code
                   Initialize the GHDI interface for NV by ghdi_nvmem_register_task fn
                   RRC_DEACT_CNF is added to the list of MM commands RRC can request for
06/05/02   ks      Added support for Dual mode operation.
09/10/02   vdr     Added func for allocation of the LLC primitive mm_gllc_get_cmd_buf()
11/12/02   jca     Added RRC_CLOSE_SESSION_CNF.
12/08/02   mks     Added allocation for RRC_ACTIVATION_IND primitive in mm_rrc_get_cmd_buf()
02/12/03   vdr     Added mm_cm_get_cmd_buf() CM->MM interface function to support PS Data
                   Continuity feature
05/13/03   npr     Added command buf support for GLLCMM_LLGMM_NEW_TLLI_IND
06/04/03   ks      Added support for RRC_MODE_CHANGE_CNF for MODE_CHANGE
                   support for UMTS.
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
11/16/04   sv      Addition of new primitive RRC_SERVICE_IND_FROM_OTA_MSG
07/20/05   abe     Added support for new primitive RRC_WTOG_CCO_COMPLETE_IND
04/06/06   sn      Added support for RRC_HSDPA_HSUPA_CELL_IND.
08/14/06   pk      Updated to log CFA packet for RRC->MM messages.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>
#include "mmcp_variation.h"
#include <stdio.h>
#include <stdlib.h> /* for srand() */
#include <string.h>
#include "customer.h"
#include "rex.h"
#include "dog_hb_rex.h"
#include "queue.h"
#include "err.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#include "mm_rr.h"
#include "mm_mm_v.h"
#include "mm_v.h"
#include "mm_coord.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */
#include "mmtask.h"
#include "mmtask_int.h"
#include "mm_umts_v.h"
#include "gmm_int_v.h"
#include "gmm_v.h"
#include "cfa_v.h"
#include "fs_public.h"
#include "mcfg_fs.h"
#include "fs_lib.h"
#include "policyman.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#ifdef FEATURE_LTE
#include "amssassert.h"
#include "emm_rrc_emm_if.h"
#include "emm_timer_sigs.h"
#include "msgr_lte.h"
#include "emm_esm_if_msg.h"
#include "emm_reg_handler.h"
#include "esm.h"
#include "emm_rrc_handler.h"
#include "emm_update_lib.h"
#include "emm_database.h"
#include "emm_mem.h"
#include "emm_connection_handler.h"
#include "emm_esm_handler.h"
#include "lte_nas_msg_parse.h"
#include "emm_service_request_lib.h"
#include "emm_utility.h"
#include "pthread.h"
#include "emm.h"
#include "rex.h"
#include "mmauth.h"
#include "err.h"
#include "lte_nas_tlb.h"
#include "queue.h"
#include "msgr_nas.h"
#include "msgr_rex.h"
#include "mm_multimode_common.h"
#include "emm_irat_if_msg.h"
#include "emm_csfb_handler.h"
#include "lte_rrc_irat_msg.h"
#if defined (FEATURE_LTE_REL9)
#include "emm_gps_handler.h"
#include "emm_gps_if_msg.h"
#endif /*FEATURE_LTE_REL9*/
#ifdef FEATURE_MODEM_RCINIT
#include "rcinit.h"
#endif
#include "gmm_int_v.h"
#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif
#include "event.h"
#include "naslog_v.h"
#include "event_defs.h"

#if defined (FEATURE_SGLTE)|| defined (FEATURE_DUAL_SIM)
extern boolean mm_get_as_id(mm_cmd_type *message);
#endif

extern void mm_set_mm_controls_resel_dec_mt_cs_call(mm_cmd_type *message);

#ifdef FEATURE_LTE
extern void emm_get_esm_as_id
(
  void *emm_message_router_cmd
);
extern void emm_get_rrc_as_id
(
  void *rrc_message_router_cmd
);
extern void emm_get_tlb_as_id
(
  void *cmd_ptr
);
extern void emm_get_irat_csfb_as_id
(
  void *cmd_ptr
);
extern void emm_get_gps_as_id
(
  void *cmd_ptr
);
#endif

encode_nas_msg_func_ptr* encode_nas_msg_fp = NULL;
decode_nas_msg_func_ptr* decode_nas_msg_fp = NULL;
nas_retrieve_msg_func_ptr* nas_retrieve_msg_fp = NULL;

pthread_mutex_t  emm_mutex = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_t  emm_manual_ftai_list_mutex = PTHREAD_MUTEX_INITIALIZER;  

extern boolean emm_disable_geran_cap;
#ifdef FEATURE_DUAL_SIM
extern boolean gmm_set_drx_req_pending_sim[MAX_AS_IDS];
#define gmm_set_drx_req_pending gmm_set_drx_req_pending_sim[mm_sub_id]
#else
extern boolean gmm_set_drx_req_pending;
#endif
#define EMM_DISABLE_LTE_DURATION_MIN_VALUE 720


void emm_init_msglib_encode_func_ptr(encode_nas_msg_func_ptr encode_func_ptr)
{
  encode_nas_msg_fp = encode_func_ptr;
  ASSERT(encode_nas_msg_fp != NULL) ; 
}

void emm_init_msglib_decode_func_ptr(decode_nas_msg_func_ptr decode_func_ptr)
{
  decode_nas_msg_fp = decode_func_ptr;
  ASSERT(decode_nas_msg_fp != NULL) ; 
}

void emm_init_msglib_retrieve_func_ptr(nas_retrieve_msg_func_ptr retrieve_func_ptr)
{
  nas_retrieve_msg_fp = retrieve_func_ptr;
  ASSERT(nas_retrieve_msg_fp != NULL) ; 
}
#endif /*FEATURE_LTE*/

#if defined (FEATURE_SGLTE)|| defined (FEATURE_DUAL_SIM)
static mm_emm_dispatcher_fp *mm_emm_dispatcher_func_ptr_sim[MAX_NAS_STACKS];
#define mm_emm_dispatcher_func_ptr mm_emm_dispatcher_func_ptr_sim[mm_as_id]
#else
static mm_emm_dispatcher_fp *mm_emm_dispatcher_func_ptr;
#endif

#ifdef FEATURE_NAS_DIAG_INIT
extern void nas_diag_init (void);
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

nas_crit_sect_type mm_rat_prioritylist_crit_sec;

/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
  #undef  DOG_MM_RPT
  #define DOG_MM_RPT       mm_dog_rpt_var
  dog_report_type mm_dog_rpt_var  = 0;      /* Initial Safety */

/* -----------------------------
** Command queue for the MM task
** ----------------------------- */
q_type mm_cmd_q;

#ifdef FEATURE_LTE


q_type esm_esm_cmd_q_msg;





q_type rrc_emm_cmd_q_msg;





q_type tlb_tlb_cmd_q_msg;





q_type emm_csfb_cmd_q_msg;


#if defined (FEATURE_LTE_REL9)


q_type emm_gps_cmd_q_msg;

#endif /*FEATURE_LTE_REL9 */

#endif /*FEATURE_LTE*/

rex_tcb_type* mm_tcb_ptr = NULL;

/*===========================================================================

FUNCTION MM_TIMER_EXPIRY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

void mm_timer_expiry(unsigned long param)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = mm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = (byte)(param & 0x0000FFFF);
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on REG command queue */
   mm_put_cmd(cmd_ptr);
}
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
void mm_timer_expiry_sub1(unsigned long param)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = mm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = SYS_MODEM_AS_ID_2;
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on REG command queue */
   mm_put_cmd(cmd_ptr);
}
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
void mm_timer_expiry_sub2(unsigned long param)
{
   mm_cmd_type   *cmd_ptr;

   /* Get command buffer from reg task */
   cmd_ptr = mm_get_cmd_buf();

   if (cmd_ptr == NULL)
   {
      MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
    /* Construct message */
   cmd_ptr->cmd.timer_expiry.IMH.message_set = MS_TIMER;
   cmd_ptr->cmd.timer_expiry.IMH.message_id  = MID_TIMER_EXPIRY;

   PUT_IMH_LEN (sizeof (cmd_ptr->cmd.timer_expiry) - sizeof (cmd_ptr->cmd.timer_expiry.IMH),
                     &(cmd_ptr->cmd.timer_expiry.IMH));

   cmd_ptr->cmd.timer_expiry.data        = SYS_MODEM_AS_ID_3;
   cmd_ptr->cmd.timer_expiry.timer_id    = (byte)(param >> 16);

   /* Put on REG command queue */
   mm_put_cmd(cmd_ptr);
}
#endif
#endif
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION MM_GET_CMD_BUF

DESCRIPTION
  This function gets a mm_cmd_type from MM task with memory already allocated.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
mm_cmd_type *mm_get_cmd_buf( void )
{
   /* Allocate command buffer */
#ifndef FEATURE_MODEM_HEAP
   return (mm_cmd_type*) gs_alloc( sizeof( mm_cmd_type ));
#else
   return (mm_cmd_type*) modem_mem_calloc(1, sizeof( mm_cmd_type ), MODEM_MEM_CLIENT_NAS);
#endif
}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION MM_CM_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of CM with the header information
  already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/
mm_cmd_type *mm_cm_get_cmd_buf(cm_mm_cmd_e_T cmd_type)
{
   mm_cmd_type *cmd;

   if(cmd_type >= MMCM_CMD_MAX)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid mm command ID is supplied", 0,0,0);
      return NULL;
   }
#ifndef FEATURE_MODEM_HEAP
   cmd = (mm_cmd_type*)gs_alloc(sizeof(mm_cmd_type));
#else
   cmd = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
   mm_check_for_null_ptr((void*)cmd);

   cmd->cmd.hdr.message_set = MS_CM_MM;
   cmd->cmd.hdr.message_id  = (byte) cmd_type;
   switch(cmd_type)
   {
     case MMCM_PS_DATA_AVAILABLE_REQ:
#ifdef FEATURE_LTE
     case MMCM_DUAL_RX_1XCSFB_TAU_REQ:
#endif
       PUT_IMH_LEN(0, &cmd->cmd.hdr) ;
       break;
#ifdef FEATURE_LTE
     case MMCM_RPM_UPDATE_FTAI_LIST:
       PUT_IMH_LEN(sizeof(cm_mm_update_ftai_list_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
#endif

#if defined(FEATURE_LTE) && defined(FEATURE_LTE_REL9)
     case MMCM_IMS_REGISTRATION_INFO:
       PUT_IMH_LEN(sizeof(cm_mm_ims_registration_info_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
#endif

#ifdef FEATURE_LTE
     case MMCM_UE_CAPABILITIES_INFO:
       PUT_IMH_LEN(sizeof(cm_mm_ue_capabilities_info_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
#endif
     case MMCM_MMTEL_CALL_INFO:
       PUT_IMH_LEN(sizeof(cm_mm_mmtel_call_info_T) - sizeof(IMH_T), &cmd->cmd.hdr) ;
       break;
     default:
       MSG_FATAL_DS(MM_SUB, "=MM= Unknown command type", 0,0,0);
   }

   return cmd ;
} /* end mm_cm_get_cmd_buf() */



/*===========================================================================

FUNCTION MM_RRC_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of RRC with the header information
  already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

mm_cmd_type *mm_rrc_get_cmd_buf(rrc_to_mm_sink_e_type cmd_type)
{
   mm_cmd_type *cmd;

   if(cmd_type >= RRC_MM_SINK_CMD_MAX)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid mm command ID is supplied", 0,0,0);
      return NULL;
   }
#ifndef FEATURE_MODEM_HEAP
   cmd = (mm_cmd_type*)gs_alloc(sizeof(mm_cmd_type));
#else
   cmd = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
   mm_check_for_null_ptr((void*)cmd);

   cmd->cmd.hdr.message_set = MS_MM_RRC;
   cmd->cmd.hdr.message_id  = (byte) cmd_type;
   switch(cmd_type)
   {
      case RRC_EST_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_REL_IND:
         PUT_IMH_LEN(sizeof(rrc_rel_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SYNC_IND:
         PUT_IMH_LEN(sizeof(rrc_sync_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SERVICE_CNF:
         PUT_IMH_LEN(sizeof(rrc_service_cnf_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SERVICE_IND:
         PUT_IMH_LEN(sizeof(rrc_service_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SERVICE_IND_FROM_OTA_MSG:
         PUT_IMH_LEN(sizeof(rrc_service_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_PLMN_LIST_CNF:
         PUT_IMH_LEN(sizeof(rrc_plmn_list_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_ABORT_IND:
         PUT_IMH_LEN(sizeof(rrc_abort_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_PAGE_IND:
         PUT_IMH_LEN(sizeof(rrc_page_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_DATA_IND:
         PUT_IMH_LEN(sizeof(rrc_data_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_OPEN_SESSION_CNF:
         PUT_IMH_LEN(sizeof(rrc_open_session_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_CLOSE_SESSION_IND:
         PUT_IMH_LEN(sizeof(rrc_close_session_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_CLOSE_SESSION_CNF:
         PUT_IMH_LEN(sizeof(rrc_close_session_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_ACTIVATION_IND:
         PUT_IMH_LEN(sizeof(rrc_activation_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_ABORT_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_DEACT_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_MODE_CHANGE_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_STOP_WCDMA_MODE_CNF:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
      case RRC_INTERRAT_CELL_CHANGE_IND:
         PUT_IMH_LEN(sizeof(rrc_interrat_cell_change_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#ifdef FEATURE_HSDPA
      case RRC_HSDPA_HSUPA_CELL_IND:
         PUT_IMH_LEN(sizeof(rrc_hsdpa_hsupa_cell_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#ifdef FEATURE_HSPA_CALL_STATUS_IND
      case RRC_HSPA_CALL_STATUS_IND:
         PUT_IMH_LEN(sizeof(rrc_hspa_call_status_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#endif
#endif
#ifdef FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
      case RRC_WTOG_CCO_COMPLETE_IND:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
#endif /* FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER */
#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
      case RRC_CONNECTED_MODE_SERVICE_STATUS_IND:
         PUT_IMH_LEN(sizeof(rrc_conn_mode_service_status_ind_type) - sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#endif
#ifdef FEATURE_MBMS //rrc_mbms_conn_req_ind_type
      #error code not present
#endif
      case RRC_FAST_DORMANCY_CNF:
         PUT_IMH_LEN(sizeof(rrc_fast_dormancy_cnf_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
#ifdef FEATURE_WRLF_SYSTEM_SEL
      case RRC_DISABLE_BPLMN_IND:
         PUT_IMH_LEN(sizeof(rrc_disable_bplmn_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
         break;
      case RRC_SUITABLE_SEARCH_END_IND:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
#endif
#ifdef FEATURE_LTE
      case RRC_SRVCC_HANDOVER_FAILURE_IND:
         PUT_IMH_LEN(0, &cmd->cmd.hdr);
         break;
#endif
      case RRC_PCH_STATE_IND:
        PUT_IMH_LEN(sizeof(rrc_pch_state_ind_type)- sizeof(IMH_T), &cmd->cmd.hdr);
        break;

      default:
         MSG_FATAL_DS(MM_SUB, "=MM= Unknown RRC command type", 0,0,0);
   }

   return cmd;
}


/*===========================================================================

FUNCTION MM_TDSRRC_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of TD-SCDMA RRC with the header information
  already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

mm_cmd_type *mm_tdsrrc_get_cmd_buf(rrc_to_mm_sink_e_type cmd_type)
{
   mm_cmd_type *cmd;

   cmd = mm_rrc_get_cmd_buf( cmd_type );

   if (cmd != NULL)
   {
     cmd->cmd.hdr.message_set = MS_MM_TDSRRC;
   }

   return cmd;
}


#ifdef FEATURE_GSM_GPRS
/*===========================================================================

FUNCTION MM_GLLC_GET_CMD_BUF

DESCRIPTION
  Get a mm_cmd_type buffer for specific use of GPRS LLC with the header
  information already filled in.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to mm_cmd_type buffer.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
mm_cmd_type *mm_gllc_get_cmd_buf( uint8 cmd )
{
#ifndef FEATURE_MODEM_HEAP
  mm_cmd_type *cmd_p = (mm_cmd_type*)gs_alloc(sizeof(mm_cmd_type));
#else
  mm_cmd_type *cmd_p = (mm_cmd_type*)modem_mem_calloc(1,sizeof(mm_cmd_type), MODEM_MEM_CLIENT_NAS);
#endif
  mm_check_for_null_ptr((void*)cmd_p);

  cmd_p -> cmd.hdr.message_set = MS_LLC_GMM;
  cmd_p -> cmd.hdr.message_id  = cmd;

  switch( cmd )
  {
  case GLLCMM_LLGMM_STATUS_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_status_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_TRIGGER_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_trigger_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_RDY_TMR_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_rdy_tmr_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_SUSPEND_CNF:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_suspend_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LLGMM_NEW_TLLI_IND:
    PUT_IMH_LEN(sizeof(gllcmm_llgmm_new_tlli_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_UNITDATA_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_unitdata_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_XID_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_xid_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_XID_CNF:
    PUT_IMH_LEN(sizeof(gllcmm_ll_xid_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_RESET_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_reset_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCMM_LL_STATUS_IND:
    PUT_IMH_LEN(sizeof(gllcmm_ll_status_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

#ifdef FEATURE_GPRS_PS_HANDOVER
  case GLLCMM_LLGMM_PSHO_CNF:
    PUT_IMH_LEN(sizeof(gllcmm_ll_psho_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;    
#endif    

  case GLLCSMS_LL_UNITDATA_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_unitdata_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_XID_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_xid_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_XID_CNF:
    PUT_IMH_LEN(sizeof(gllcsms_ll_xid_cnf_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_RESET_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_reset_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  case GLLCSMS_LL_STATUS_IND:
    PUT_IMH_LEN(sizeof(gllcsms_ll_status_ind_type) - sizeof(IMH_T), &cmd_p->cmd.hdr);
    break;

  default:
     MSG_FATAL_DS(MM_SUB, "=MM= Unknown LLC command type", 0,0,0);
  }

  return cmd_p;

} /* end of mm_gllc_get_cmd_buf() */

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#endif /* FEATURE_GSM_GPRS */


/*===========================================================================

FUNCTION MM_FREE_CMD_BUF

DESCRIPTION
  Free a previously allocated mm_cmd_type buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_free_cmd_buf( mm_cmd_type *cmd_buf )
{
   if(cmd_buf == NULL)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Trying to free an empty pointer", 0,0,0);
   }
#ifndef FEATURE_MODEM_HEAP
   gs_free(cmd_buf);
#else
   modem_mem_free(cmd_buf, MODEM_MEM_CLIENT_NAS);
#endif
}


/*===========================================================================

FUNCTION MM_SEND_ROUTINE

DESCRIPTION
  This is the MM-specific routine used when other layers call gs_send_message

DEPENDENCIES
  None

RETURN VALUE
  GS_STATUS - GS_SUCCESS if sent, GS_FAILURE otherwise

SIDE EFFECTS
  None

===========================================================================*/
gs_status_T mm_send_routine( void *msg_p )
{
    word          message_length;
    void         *cmd_ptr;

    /* Get message length */
    GET_IMH_LEN(message_length, msg_p);

    message_length = MIN(sizeof(mm_int_cmd_type_T) - sizeof(IMH_T),message_length);

    /* Add in header size */
    message_length += sizeof(IMH_T);

    /* Get command buffer from destination task */
    if((cmd_ptr = mm_get_cmd_buf()) != NULL)
    {
        /* Copy data to command buffer */
        memscpy(&((mm_cmd_type*)cmd_ptr)->cmd,sizeof(mm_int_cmd_type_T), (byte*)msg_p, message_length);

        PUT_IMH_LEN((message_length-sizeof(IMH_T)), &((mm_cmd_type*)cmd_ptr)->cmd);

        /* Put message in destination task queue */
        mm_put_cmd((mm_cmd_type*) cmd_ptr);
        return GS_SUCCESS;
    }
    return GS_FAILURE;
}


/*===========================================================================

FUNCTION MM_PUT_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void mm_put_cmd(mm_cmd_type *cmd_ptr)
{

   /* Check for Null pointer */
   if(cmd_ptr != NULL)
   {
#ifdef FEATURE_SEGMENT_LOADING
      if (((cmd_ptr->cmd.hdr.message_set == MS_MM_RRC)&&(get_wcdma_interface())) ||
          ((cmd_ptr->cmd.hdr.message_set == MS_MM_TDSRRC)&&(get_tdscdma_interface())))
#else
      if ((cmd_ptr->cmd.hdr.message_set == MS_MM_RRC) ||
          (cmd_ptr->cmd.hdr.message_set == MS_MM_TDSRRC))
#endif
      {
        word      message_length;
        /*    Get message length */
        GET_IMH_LEN(message_length, &(cmd_ptr->cmd));
        /*    Add in header size */
        message_length += sizeof(IMH_T);
        /* Log using CFA packet */
        cfa_log_packet( GS_QUEUE_MM, message_length, (byte*)&(cmd_ptr->cmd) );
      }

      /* First initialize the queue link. This is always done
      before putting an item in a queue. */
      (void) q_link(cmd_ptr, &(cmd_ptr->link));

      /* Put the message on the queue */
      q_put(&mm_cmd_q, &cmd_ptr->link);

      /* Set the command queue signal for */
     /* receive task                     */
     (void) rex_set_sigs(mm_tcb_ptr, MM_CMD_Q_SIG);

   }
   else
   {
      /* Raise error condition */
      MSG_FATAL_DS(MM_SUB, "=MM= Trying to put a null pointer on the mm queue", 0,0,0);
   }
}

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
#ifdef FEATURE_MODEM_HEAP
/*===========================================================================

FUNCTION nas_mem_alloc

DESCRIPTION
  This function is called by the external modules like RRC in order to allocate 
  dynamic memory. This function simply calls mem_malloc and selects suitable
  heap and returs pointer to the allocated memory.

DEPENDENCIES
  The memory manager must have been previously initialized.

RETURN VALUE
  A pointer to the allocated memory if the memory was successfully allocated,
  otherwise NULL.

SIDE EFFECTS
  None.

===========================================================================*/
void * nas_mem_alloc(size_t size)
{
  void * return_ptr =  NULL;
  
  return_ptr = 
       (unsigned char *) modem_mem_calloc(1,size,MODEM_MEM_CLIENT_NAS);

  mm_check_for_null_ptr((void*)return_ptr);

  return(return_ptr);
}
#endif

/*===========================================================================

FUNCTION NAS_SET_EFS_NAS_MM_CONFIG

DESCRIPTION
 This function would check if the directory for "conf" exists, if not
 will create the directory and nas_mm.conf file in the directoru which
 would contain the paths to NV items. nas_mm.conf is used by the qpst tools
 to backup and restore nv items specified in the .conf file.

DEPENDENCIES
  None.

RETURN VALUE
  None. 
 
SIDE EFFECTS
  None.

===========================================================================*/
#define NAS_MM_CONF_FILE "/nv/item_files/conf/nas_mm.conf"
#define NAS_MM_PERMISSIONS 0777

void nas_set_efs_nas_mm_config( void )
{
  struct fs_stat     nas_conf_stat;
  int32              nas_conf_fd;

  char *nas_conf_items = 
    "/nv/item_files/modem/nas/geran_cap\n"\
"/nv/item_files/modem/nas/lte_nas_lsti_config\n"\
"/nv/item_files/modem/nas/lte_nas_ue_sec_capability\n"\
"/nv/item_files/modem/nas/lte_nas_temp_fplmn_backoff_time\n"\
"/nv/item_files/modem/nas/lte_nas_temp_fplmn_backoff_time_Subscription01\n"\
"/nv/item_files/modem/nas/lte_nas_srlte_esr_time\n"\
"/nv/item_files/modem/nas/lte_nas_srlte_esr_time_Subscription01\n"\
"/nv/item_files/modem/nas/drx_cn_coeff_s1\n"\
"/nv/item_files/modem/nas/exclude_ptmsi_type_field\n"\
"/nv/item_files/modem/nas/exclude_old_lai_type_field\n"\
"/nv/item_files/modem/nas/nas_lai_change_force_lau_for_emergency\n"\
"/nv/item_files/modem/nas/nas_lai_change_force_lau_for_emergency_Subscription01\n"\
"/nv/item_files/modem/nas/nas_srvcc_support\n"\
"/nv/item_files/modem/nas/l21xsrvcc_support\n"\
"/nv/item_files/modem/nas/mobility_management_for_voims_feature\n"\
"/nv/item_files/modem/nas/mobility_management_for_voims_feature_Subscription01\n"\
"/nv/item_files/modem/nas/nas_config_feature\n"\
"/nv/item_files/modem/nas/aggression_management\n"\
"/nv/item_files/modem/nas/aggression_management_Subscription01\n"\
"/nv/item_files/modem/nas/csg_support_configuration\n"\
"/nv/item_files/modem/nas/nas_l2g_srvcc_support\n"\
"/nv/item_files/modem/nas/tighter_capability\n"\
"/nv/item_files/modem/nas/nas_nv_classmark_ie\n"\
"/nv/item_files/modem/nas/sglte_nas_nv_config\n"\
"/nv/item_files/modem/nas/gmm_drx_cn_coeff_s1\n"\
"/nv/item_files/modem/nas/isr\n"\
"/nv/item_files/modem/nas/emm_combined_proc\n"\
"/nv/item_files/modem/nas/conn_mode_manual_search\n"\
"/nv/item_files/modem/nas/avoid_guti_nas_security_check\n"\
"/nv/item_files/modem/nas/is_accepted_on_lte\n"\
"/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call\n"\
"/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call_Subscription01\n"\
"/nv/item_files/modem/nas/mm_manual_search_timer_value\n"\
"/nv/item_files/modem/nas/early_call_init\n"\
"/nv/item_files/modem/nas/VOLTE_persistent_active_flag\n"\
"/nv/item_files/modem/nas/volte_rej_mt_csfb\n"\
"/nv/item_files/modem/nas/emm_nas_nv_items\n"\
"/nv/item_files/modem/nas/emm_nas_nv_items_Subscription01\n"\
"/nv/item_files/modem/nas/emergency_call_after_lu_in_lpm\n"\
"/nv/item_files/modem/nas/suppress_add_update_param\n"\
"/nv/item_files/modem/nas/suppress_add_update_param_Subscription01\n"\
"/nv/item_files/modem/nas/disable_conn_mode_manual_search_DSDS\n"\
"/nv/item_files/modem/nas/lte_accepted_vplmn\n\
";

  /* create the directory if it doesn't exist */
  if (efs_stat("/nv/item_files/conf", &nas_conf_stat) == -1 )
  {
    if (efs_stat("/nv/item_files", &nas_conf_stat) == -1 )
    {
      if (efs_stat("/nv", &nas_conf_stat) == -1 )
      {
        (void)efs_mkdir("/nv", NAS_MM_PERMISSIONS);
      }
      (void)efs_mkdir("/nv/item_files", NAS_MM_PERMISSIONS);          
    }
    (void)efs_mkdir("/nv/item_files/conf", NAS_MM_PERMISSIONS); 
  }
 
  /* create the nas_mm.conf file if it doesn't exist */
  if (efs_stat(NAS_MM_CONF_FILE, &nas_conf_stat) < 0 )
  {
    nas_conf_fd = efs_creat(NAS_MM_CONF_FILE, NAS_MM_PERMISSIONS);
  
    if( nas_conf_fd >= 0 )
    {
      (void)efs_write(nas_conf_fd, nas_conf_items,strlen(nas_conf_items));
    }
    else
    {
      MSG_ERROR_DS(MM_SUB,"MM EFS CONF file NOT created %d",nas_conf_fd,0,0);
    }
    (void)efs_close(nas_conf_fd);  
    MSG_HIGH_DS_1(MM_SUB ,"=MM= nas_mm.conf file status %d",nas_conf_fd);
  }
} 

/*===========================================================================

FUNCTION MM_WAIT

DESCRIPTION
  Main Wait routine for MM task. This routine performs the following functions:
    - Checks MM queue to see if anything is available.  The MM queue
      will be checked to see if anything is available before performing
      a rex_wait().  (There is no need to wait if something is on the queue
      already).  If an item is found on the queue the corresponding bit in
      the returned mask is set.
    - Kicks the watchdog timer while waiting for the specified mask.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type mm_wait
(
   rex_sigs_type requested_mask    /* Mask of signals to wait for */
)
{
   rex_sigs_type queue_mask;        /* Mask of signals indicating queue status */
   rex_sigs_type rex_signals_mask;  /* Mask of signals returned by REX     */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Initialize temporary mask holder for queue information */
   queue_mask = 0;

   /* --------------------------------------------------------------
   ** Check message queue:  If the request mask contains a check for
   ** MM_CMD_Q_SIG_MASK then we should check the queue, and set in
   ** the queue_mask an indication that something is waiting to be
   ** serviced on that queue.  The rex signal is also cleared
   ** regardless of the queue count.  This is to cover the case
   ** where there is a signal set but nothing on the queue.
   ** ------------------------------------------------------------- */
   if (requested_mask & MM_CMD_Q_SIG)
   {
      (void) rex_clr_sigs( mm_tcb_ptr, MM_CMD_Q_SIG );

      if (q_cnt( &mm_cmd_q ))
      {
         queue_mask |= MM_CMD_Q_SIG;
      }
   }

   /* Get current state of rex signals */
   rex_signals_mask = rex_get_sigs(mm_tcb_ptr);

   /* --------------------------------------------------------------------
   ** Loop until the current signals from REX (including the queue signals
   ** faked due to an item on a queue, if any) match with one or more of
   ** the requested signals.
   ** -------------------------------------------------------------------- */
   do
   {
#ifndef TEST_FRAMEWORK  
        /* Kick the watchdog */
        /* Report to the Dog HB */
        dog_hb_report(DOG_MM_RPT);
#endif

      /* Perform the 'real' wait (if needed) */
      if (((rex_signals_mask | queue_mask) & requested_mask) == 0)
      {
         rex_signals_mask = rex_wait( requested_mask | MM_DOGRPT_TIMER_SIG );
       /* Clear the watchdog signal */
       (void) rex_clr_sigs( mm_tcb_ptr, MM_DOGRPT_TIMER_SIG);

      }

      /* ---------------------------------------------------------------
      ** Make sure that if we got a queue signal, that there is actually
      ** an item on the queue indicated
      ** --------------------------------------------------------------- */
      if (rex_signals_mask & MM_CMD_Q_SIG)
      {
         (void) rex_clr_sigs( mm_tcb_ptr, MM_CMD_Q_SIG );

         if (q_cnt( &mm_cmd_q ) == 0)   /* No elements in queue ? */
         {
            rex_signals_mask &= ~MM_CMD_Q_SIG;  /* Turn off bit */
         }
      }

   /* do while mask requested does not match current mask */
   } while (((rex_signals_mask | queue_mask) & requested_mask) == 0);

   return ((rex_signals_mask | queue_mask));

} /* end mm_wait() */

#ifdef FEATURE_LTE

typedef PACKED struct PACKED_POST{ 
  boolean    eps_encrypt_alg_128_eea0_supported; 
  boolean    eps_encrypt_alg_128_eea1_supported; 
  boolean    eps_encrypt_alg_128_eea2_supported;
  boolean    eps_encrypt_alg_eea3_supported;
  boolean    eps_encrypt_alg_eea4_supported;
  boolean    eps_encrypt_alg_eea5_supported;
  boolean    eps_encrypt_alg_eea6_supported;
  boolean    eps_encrypt_alg_eea7_supported;
  boolean    eps_integrity_alg_128_eia1_supported; 
  boolean    eps_integrity_alg_128_eia2_supported; 
  boolean    eps_integrity_alg_128_eia3_supported; 
  boolean    eps_integrity_alg_128_eia4_supported; 
  boolean    eps_integrity_alg_128_eia5_supported; 
  boolean    eps_integrity_alg_128_eia6_supported; 
  boolean    eps_integrity_alg_128_eia7_supported; 
  boolean    umts_encrypt_alg_uea0_supported; 
  boolean    umts_encrypt_alg_uea1_supported;
  boolean    umts_encrypt_alg_uea2_supported;
  boolean    umts_encrypt_alg_uea3_supported;
  boolean    umts_encrypt_alg_uea4_supported;
  boolean    umts_encrypt_alg_uea5_supported;
  boolean    umts_encrypt_alg_uea6_supported;
  boolean    umts_encrypt_alg_uea7_supported;
  boolean    ucs2_supported; 
  boolean    umts_integrity_alg_uia1_supported; 
  boolean    umts_integrity_alg_uia2_supported;
  boolean    umts_integrity_alg_uia3_supported;
  boolean    umts_integrity_alg_uia4_supported;
  boolean    umts_integrity_alg_uia5_supported;
  boolean    umts_integrity_alg_uia6_supported;
  boolean    umts_integrity_alg_uia7_supported;
  boolean    gprs_encrypt_alg_1_supported; 
  boolean    gprs_encrypt_alg_2_supported; 
  boolean    gprs_encrypt_alg_3_supported; 
  boolean    gprs_encrypt_alg_4_supported; 
  boolean    gprs_encrypt_alg_5_supported; 
  boolean    gprs_encrypt_alg_6_supported; 
  boolean    gprs_encrypt_alg_7_supported; 

} ue_security_capability;

/*=============================================================================

FUNCTION  LTE_NAS_EFS_READ

DESCRIPTION
  This function reads NV items from EFS upon power up

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void lte_nas_efs_read
(
  void
)
{
  boolean lsti_config = FALSE;
  ue_security_capability ue_security;
  emm_ctrl_data_type *emm_s1_ctrl_data_ptr = NULL;
  mcfg_fs_status_e_type efs_status[4] = {MCFG_FS_STATUS_EFS_ERR,MCFG_FS_STATUS_EFS_ERR,MCFG_FS_STATUS_EFS_ERR,MCFG_FS_STATUS_EFS_ERR};
  emm_nas_nv_items_T emm_nas_nv;  
  boolean use_lte_disable_duration_timer_nv = FALSE;
#ifdef FEATURE_TDSCDMA
  uint8 nv_special_tdscdma_test_setting_enable = 0;
#endif

#ifdef FEATURE_DUAL_SIM
  for(mm_sub_id = MM_AS_ID_1, mm_as_id = MM_AS_ID_1; mm_sub_id < mm_sim_max_subscription; mm_sub_id++, mm_as_id++)
#endif
  {
    emm_read_carrier_specific_efs();
  }

#ifdef FEATURE_DUAL_SIM
  mm_sub_id = MM_AS_ID_1;
  mm_as_id = MM_AS_ID_1;
#endif

  emm_s1_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(emm_s1_ctrl_data_ptr != NULL);

  /* default encryption and integrity algorithms supported */
  memset((void *)&emm_s1_ctrl_data_ptr->ue_security_capability, 0, sizeof(lte_nas_emm_ue_security_type));

#ifdef FEATURE_NAS_EAB
  /* Marking this as true for now, may need NV later on to control this feature */
  emm_s1_ctrl_data_ptr->eab_barring_flag = TRUE;  
#endif
     /* 
   * Initialize STRUCT emm_nas_nv to 0.
   * Later in the function, values read from EFS will be stored here.
   */
   memset( &emm_nas_nv, 0x00, sizeof(emm_nas_nv_items_T) ); 

   efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/emm_nas_nv_items",
						 &emm_nas_nv,
						 sizeof(emm_nas_nv_items_T),
						 MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);

   if (efs_status[0] != MCFG_FS_STATUS_OK)
   {
	 emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_8   =  FALSE;
     use_lte_disable_duration_timer_nv   =  FALSE;
     emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = FALSE;
   }
   else
   {
     if(emm_nas_nv.emm_plmn_block_on_rej_15_8 > 0)
     {
       emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 = TRUE;
     }
     else
     {
       emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 = FALSE;
     }			
     if(emm_nas_nv.use_lte_disable_duration_timer_nv > 0)
     {
       use_lte_disable_duration_timer_nv = TRUE;
     }
     else
     {
       use_lte_disable_duration_timer_nv = FALSE;
     }

     if((emm_nas_nv.version >= 3) &&
        (emm_nas_nv.psm_local_detach_enabled == TRUE))
     {
       emm_s1_ctrl_data_ptr->psm_local_detach_enabled = TRUE;
     }
     else
     {
       emm_s1_ctrl_data_ptr->psm_local_detach_enabled = FALSE;
     }
     if(emm_nas_nv.emm_plmn_block_on_rej_15_roaming > 0)
     {
       emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = TRUE;
     }
     else
     {
       emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = FALSE;
     }
   }
     MSG_HIGH_DS_3(MM_SUB ,
      "=EMM= version of emm_nas_nv = %d use_lte_disable_duration_timer_nv %d  emm_plmn_block_on_rej_15_roaming %d",
      emm_nas_nv.version,use_lte_disable_duration_timer_nv, emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming);
  if(use_lte_disable_duration_timer_nv)
  {
    dog_hb_report(DOG_MM_RPT);
    if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/mmode/lte_disable_duration", 
             &emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv, 
             sizeof(uint32), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
    {
      emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv = 0;
    }
    else if((emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv < EMM_DISABLE_LTE_DURATION_MIN_VALUE) &&
            (emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv != 0))
    {
      emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv = EMM_DISABLE_LTE_DURATION_MIN_VALUE;
    }
    emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv *= 1000;
    MSG_HIGH_DS_2(MM_SUB ,"=MM= lte_disable_duration_timer_nv %d, status = %d",
                  emm_s1_ctrl_data_ptr->lte_disable_duration_timer_nv, efs_status[1]);
  }

  /* Changing the directory to item_files that can be edited
     using UI
  */

  if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/isr", 
                              &emm_s1_ctrl_data_ptr->isr_efs_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->isr_efs_flag = TRUE;
  }
   
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  
  if((efs_status[2] = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_ignore_mt_csfb_during_volte_call", 
                              &emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->ignore_mt_csfb_during_volte_call = FALSE;
  }

  MSG_HIGH_DS_6(MM_SUB,"=EMM= emm_nas_nv_items (status %d version %d) isr_efs_flag (status =%d value = %d) volte_persistent_active_flag (status =%d value = %d)",
                    efs_status[0],
                    emm_nas_nv.version,
                    efs_status[1],
                    emm_s1_ctrl_data_ptr->isr_efs_flag,
                    efs_status[2],
                    emm_s1_ctrl_data_ptr->volte_persistent_active_flag);

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/emm_combined_proc", 
                              &emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag = TRUE;
  }

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);
  
  if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/VOLTE_persistent_active_flag", 
                              &emm_s1_ctrl_data_ptr->volte_persistent_active_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->volte_persistent_active_flag = TRUE;
  }
    /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  if((efs_status[2] = mcfg_fs_read("/nv/item_files/modem/nas/avoid_guti_nas_security_check", 
                              &emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check = FALSE;
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  /* GERAN cap */

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  if((efs_status[3] = mcfg_fs_read("/nv/item_files/modem/nas/geran_cap", 
                              &emm_disable_geran_cap, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_disable_geran_cap = FALSE;
  }

  MSG_HIGH_DS_8(MM_SUB,"=EMM= exclude_ptmsi_type_field (status %d value %d) emm_combined_proc_efs_flag (status =%d value = %d) avoid_guti_nas_security_check (status =%d value = %d) geran_cap (status =%d value = %d) ",
                    efs_status[0],
                    emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag,
                    efs_status[1],
                    emm_s1_ctrl_data_ptr->volte_persistent_active_flag ,
                    efs_status[2],
                    emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check,
                    efs_status[3],
                    emm_disable_geran_cap);

#ifdef FEATURE_TDSCDMA
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  /*------------ Check for the TDS NV0 policy flag ------------*/
  if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/special_test_setting_enabled", 
                      &nv_special_tdscdma_test_setting_enable, 
                                      sizeof(uint8), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    nv_special_tdscdma_test_setting_enable = 2;
  }
  MSG_HIGH_DS_2(MM_SUB,"=EMM= nv_special_tdscdma_test_setting_enable (status = %d value = %d", efs_status[0],nv_special_tdscdma_test_setting_enable);
  
  if (nv_special_tdscdma_test_setting_enable != 2)
  {
    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);
    /*------------ Check for the TDS ciphering Flag ------------*/
    if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/ciphering_enabled", 
                             &emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag, 
                             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
    {
      emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
    }
    
    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);
    
    /*------------ Check for the TDS INTEGRITY Flag ------------*/
    if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/integrity_enabled", 
                              &emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
    {
      emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = TRUE;
    }
    
    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);
    
    /*------------ Check for the TDS Fake Security Flag ------------*/
    if((efs_status[2] = mcfg_fs_read("/nv/item_files/modem/tdscdma/rrc/fake_sec_enabled", 
                              &emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag, 
                              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
    {
      emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
    }
    MSG_HIGH_DS_6(MM_SUB,"=EMM= emm_tds_nv_cipher_enable_flag (status %d value %d) emm_tds_nv_integ_enable_flag (status =%d value = %d) emm_tds_nv_fake_sec_flag (status =%d value = %d) geran_cap (status =%d value = %d) ",
                        efs_status[0],
                        emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag,
                        efs_status[1],
                        emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag,
                        efs_status[2],
                        emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag);

    /*If TDS integrity is TRUE irrespective of fake security value make it FALSE  */
    if (emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag == TRUE )
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= TDS RRC integrity Enable Making Fake sec FALSE");
      emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
    }
  }
  else 
  {
    /*Its the case when the NV0 policy says 2 value, treat it as fail 
         make cipher = false , fake Security  as FALSE, intergrity as TRUE*/
    emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
    emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = TRUE;
    emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = FALSE;
    
  }
#endif

  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  if ((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_ue_sec_capability", 
                        &ue_security, 
                        sizeof(ue_security), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Read EFS : UE Security");
    if( ue_security.eps_encrypt_alg_128_eea0_supported )
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea0_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }
    if (ue_security.eps_encrypt_alg_128_eea1_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea1_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }
    if (ue_security.eps_encrypt_alg_128_eea2_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea2_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }
    if (ue_security.eps_encrypt_alg_eea3_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_eea3_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
    }

    if (ue_security.eps_integrity_alg_128_eia1_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia1_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    }
    if (ue_security.eps_integrity_alg_128_eia2_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia2_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    }
    if (ue_security.eps_integrity_alg_128_eia3_supported)
    {
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia3_supported = TRUE;
      emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    }
    MSG_HIGH_DS_2(MM_SUB,"=EMM=  ZUC Enc Algo supported thru NAS EFS = %d ZUC Int Algo supported thru NAS EFS = %d ",
                           ue_security.eps_encrypt_alg_eea3_supported,
                           ue_security.eps_integrity_alg_128_eia3_supported);
  } else
  {
     /* temp */
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Read EFS : UE Security, NV read failed, status = %d", efs_status[0]);
    emm_s1_ctrl_data_ptr->ue_security_capability.spare_bit = FALSE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea0_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea1_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea2_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_eea3_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia1_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia2_supported = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_integrity_alg_128_eia3_supported = TRUE;

    emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
    emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;

  }

/* LSTI EFS should be read after emm_3gpp_spec_ver is read in the function
 emm_read_carrier_specific_efs() */
#ifdef FEATURE_LTE_REL9
  if(emm_s1_ctrl_data_ptr->emm_3gpp_spec_ver < LTE_3GPP_REL9)
  {
#endif

    /* Report to the Dog HB */
    dog_hb_report(DOG_MM_RPT);

    /* Read LSTI config from EFS */
    if ((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/lte_nas_lsti_config", 
                                &lsti_config, 
                                sizeof(byte), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
    {
      MSG_HIGH_DS_1(MM_SUB ,"=EMM= Read EFS : LSTI config %d", lsti_config);

      if(lsti_config == TRUE)
      {
        /* Default value - NULL encryption */
        emm_s1_ctrl_data_ptr->ue_security_capability.eps_encrypt_alg_128_eea0_supported = TRUE;
        /* Default value - SNOW3G encryption*/
        /* Default value - spare bit - to indicate NULL MACI*/
        emm_s1_ctrl_data_ptr->ue_security_capability.spare_bit = TRUE;
         
        emm_s1_ctrl_data_ptr->ue_security_capability.eps_enc_exists = TRUE;
        emm_s1_ctrl_data_ptr->ue_security_capability.eps_int_exists = TRUE;
      }
    }  
#ifdef FEATURE_LTE_REL9
  }
#endif
  

  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/nas_srvcc_support", 
             &emm_s1_ctrl_data_ptr->srvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->srvcc_capability_support = TRUE;
  }

  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/l21xsrvcc_support", 
             &emm_s1_ctrl_data_ptr->onexsrvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->onexsrvcc_capability_support = FALSE;
  }

  dog_hb_report(DOG_MM_RPT);
  /* set the l2g_srvcc_capability_indicator from EFS */
  if((efs_status[2] = mcfg_fs_read("/nv/item_files/modem/nas/nas_l2g_srvcc_support", 
             &emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support, 
             sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support = TRUE;

  }

  dog_hb_report(DOG_MM_RPT);
  /* set the srvcc_capability_indicator from EFS */
  if((efs_status[3] = mcfg_fs_read("/nv/item_files/modem/nas/early_call_init", 
              &emm_s1_ctrl_data_ptr->early_call_init, 
              sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->early_call_init = TRUE;
  } 

  MSG_HIGH_DS_8(MM_SUB,"=EMM= srvcc_capability_support (status %d value %d) l21xsrvcc_support (status =%d value = %d) nas_l2g_srvcc_support (status =%d value = %d) early_call_init (status =%d value = %d) ",
                    efs_status[0],
                    emm_s1_ctrl_data_ptr->srvcc_capability_support ,
                    efs_status[1],
                    emm_s1_ctrl_data_ptr->onexsrvcc_capability_support,
                    efs_status[2],
                    emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support,
                    efs_status[3],
                    emm_s1_ctrl_data_ptr->early_call_init);

  dog_hb_report(DOG_MM_RPT);
  if((efs_status[0] = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_TIN_item", 
                           &emm_s1_ctrl_data_ptr->TIN, 
                           sizeof(nas_mm_tin_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) != MCFG_FS_STATUS_OK)
  {
    emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
  }


  if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/mm_rplmn_type", 
                           &emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type, 
                           sizeof(mm_rplmn_type_enum_T), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
  {
    emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type = MM_RPLMN_TYPE_NONE;
  }
   MSG_HIGH_DS_4(MM_SUB,"=EMM= lte_nas_TIN_item (status %d value %d) mm_rplmn_type (status =%d value = %d) ",
                    efs_status[0],
                    emm_s1_ctrl_data_ptr->TIN ,
                    efs_status[1],
					emm_s1_ctrl_data_ptr->rplmn_info.rplmn_type);
  
#if defined  FEATURE_LTE
  /* IS ACCEPTED ON LTE */
  if(emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)
  {
     dog_hb_report(DOG_MM_RPT);
     if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/is_accepted_on_lte", 
                         &emm_s1_ctrl_data_ptr->is_accepted_on_lte, 
                         sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) == MCFG_FS_STATUS_OK)
     {
        emm_s1_ctrl_data_ptr->is_accepted_on_lte = FALSE;
     }
  }
  if(emm_s1_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming == TRUE)
  {
    dog_hb_report(DOG_MM_RPT);
    if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/lte_accepted_vplmn", 
                        &emm_s1_ctrl_data_ptr->emm_lte_accepted_vplmn, 
                        sizeof(sys_plmn_id_s_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
    {
      sys_plmn_undefine_plmn_id(&emm_s1_ctrl_data_ptr->emm_lte_accepted_vplmn);
    }
    MSG_HIGH_DS_6(MM_SUB," =MM= is_accepted_on_lte (status %d value %d) lte_accepted_vplmn (status =%d) lte accepted VPLMN [%x %x %x]",
                    efs_status[0],
                    emm_s1_ctrl_data_ptr->is_accepted_on_lte ,
                    efs_status[1],
                    emm_s1_ctrl_data_ptr->emm_lte_accepted_vplmn.identity[0],
                    emm_s1_ctrl_data_ptr->emm_lte_accepted_vplmn.identity[1],
                    emm_s1_ctrl_data_ptr->emm_lte_accepted_vplmn.identity[2]);
  }
  else
  {
    /* Default value */
    MSG_HIGH_DS_0(MM_SUB," =MM= use undefined value");
    sys_plmn_undefine_plmn_id(&emm_s1_ctrl_data_ptr->emm_lte_accepted_vplmn);
  }
#endif
#ifdef FEATURE_DUAL_SIM
  if(mm_sim_max_subscription > 1)
  {
    emm_ctrl_data_type *emm_s2_ctrl_data_ptr = NULL;
    mm_as_id = MM_AS_ID_2;
    mm_sub_id = mm_sub_id_stack[mm_as_id];
    emm_s2_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_s2_ctrl_data_ptr != NULL);
    dog_hb_report(DOG_MM_RPT);
    if((efs_status[0] = mcfg_fs_read("/nvm/alpha/modem/nas/lte_nas_TIN_item", 
                           &emm_s2_ctrl_data_ptr->TIN, 
                           sizeof(nas_mm_tin_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
    {
      emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
    }

    if((efs_status[1] = mcfg_fs_read("/nv/item_files/modem/nas/mm_rplmn_type", 
                           &emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type, 
                           sizeof(mm_rplmn_type_enum_T), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id) ) != MCFG_FS_STATUS_OK)
    {
      emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type = MM_RPLMN_TYPE_NONE;
    }
    MSG_HIGH_DS_4(MM_SUB,"=EMM= lte_nas_TIN_item (status %d value %d) mm_rplmn_type (status =%d value = %d) ",
                      efs_status[0],
                      emm_s2_ctrl_data_ptr->TIN ,
                      efs_status[1],
                      emm_s2_ctrl_data_ptr->rplmn_info.rplmn_type);

    //Copy the EFS data to other stack which share the same value
    emm_s2_ctrl_data_ptr->srvcc_capability_support = emm_s1_ctrl_data_ptr->srvcc_capability_support;
    emm_s2_ctrl_data_ptr->onexsrvcc_capability_support = emm_s1_ctrl_data_ptr->onexsrvcc_capability_support;
    emm_s2_ctrl_data_ptr->l2g_srvcc_capability_support = emm_s1_ctrl_data_ptr->l2g_srvcc_capability_support;
    emm_s2_ctrl_data_ptr->isr_efs_flag = emm_s1_ctrl_data_ptr->isr_efs_flag;
    emm_s2_ctrl_data_ptr->emm_combined_proc_efs_flag = emm_s1_ctrl_data_ptr->emm_combined_proc_efs_flag;
    emm_s2_ctrl_data_ptr->avoid_guti_nas_security_check = emm_s1_ctrl_data_ptr->avoid_guti_nas_security_check;
	emm_s2_ctrl_data_ptr->volte_persistent_active_flag = emm_s1_ctrl_data_ptr->volte_persistent_active_flag;
#ifdef FEATURE_TDSCDMA
    emm_s2_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag;
    emm_s2_ctrl_data_ptr->emm_tds_nv_integ_enable_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_integ_enable_flag;
    emm_s2_ctrl_data_ptr->emm_tds_nv_fake_sec_flag = emm_s1_ctrl_data_ptr->emm_tds_nv_fake_sec_flag;
#endif
    emm_s2_ctrl_data_ptr->ue_security_capability = emm_s1_ctrl_data_ptr->ue_security_capability;
    emm_s2_ctrl_data_ptr->exclude_identity_type_ie = emm_s1_ctrl_data_ptr->exclude_identity_type_ie;
    efs_status[2] = mcfg_fs_read("/nv/item_files/modem/nas/emm_nas_nv_items_Subscription01",
                              &emm_nas_nv,
                              sizeof(emm_nas_nv_items_T),
                              MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)MM_AS_ID_1);

    if (efs_status[2] != MCFG_FS_STATUS_OK)
    {
      emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_8   =  FALSE;
      use_lte_disable_duration_timer_nv   =  FALSE;
      emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = FALSE;
    }
    else
    {
      if(emm_nas_nv.emm_plmn_block_on_rej_15_8 > 0)
      {
        emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 = TRUE;

      }
      else
      {
        emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 = FALSE;
      }
      if(emm_nas_nv.use_lte_disable_duration_timer_nv > 0)
      {
        use_lte_disable_duration_timer_nv = TRUE;
      }
      else
      {
        use_lte_disable_duration_timer_nv = FALSE;
      }
      if(emm_nas_nv.emm_plmn_block_on_rej_15_roaming > 0)
      {
        emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = TRUE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= block eplmn on rej cause 15 is TRUE for SUB2");
      }
      else
      {
        emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = FALSE;
        MSG_HIGH_DS_0(MM_SUB,"=MM= block eplmn on rej cause 15 is FALSE for SUB2");
      }
    }
    if(use_lte_disable_duration_timer_nv)
    {
      dog_hb_report(DOG_MM_RPT);
      if((efs_status[3] = mcfg_fs_read("/nv/item_files/modem/mmode/lte_disable_duration_Subscription01", 
               &emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv, 
               sizeof(uint32), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
      {
        emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv = 0;
      }
      else if((emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv < EMM_DISABLE_LTE_DURATION_MIN_VALUE) &&
              (emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv != 0))
      {
        emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv = EMM_DISABLE_LTE_DURATION_MIN_VALUE;
      }
      emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv *= 1000;
      MSG_HIGH_DS_2(MM_SUB ,"=MM= lte_disable_duration_timer_nv %d, status = %d",
                    emm_s2_ctrl_data_ptr->lte_disable_duration_timer_nv, efs_status[3]);
    }
    if(emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming == TRUE)
    {
      dog_hb_report(DOG_MM_RPT);
      if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/lte_accepted_vplmn", 
	   &emm_s2_ctrl_data_ptr->emm_lte_accepted_vplmn, 
	   sizeof(sys_plmn_id_s_type), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
      {
        sys_plmn_undefine_plmn_id(&emm_s2_ctrl_data_ptr->emm_lte_accepted_vplmn);
        MSG_HIGH_DS_0(MM_SUB, "=MM= NV read failed VPLMN is undefined");
      }
      else
      {
         MSG_HIGH_DS_3( MM_SUB, "=MM= lte accepted VPLMN [%x %x %x]",
       emm_s2_ctrl_data_ptr->emm_lte_accepted_vplmn.identity[0],
       emm_s2_ctrl_data_ptr->emm_lte_accepted_vplmn.identity[1],
       emm_s2_ctrl_data_ptr->emm_lte_accepted_vplmn.identity[2]);
      }
    }	
    else
    {
      /* Default value */
      sys_plmn_undefine_plmn_id(&emm_s2_ctrl_data_ptr->emm_lte_accepted_vplmn);
      MSG_HIGH_DS_0(MM_SUB, "=MM= feature is not set, VPLMN is undefined");
    } 
    emm_s2_ctrl_data_ptr->early_call_init = emm_s1_ctrl_data_ptr->early_call_init;

    if(emm_s2_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)
    {
      /* IS ACCEPTED ON LTE */
       dog_hb_report(DOG_MM_RPT);
       if((efs_status[0] = mcfg_fs_read("/nv/item_files/modem/nas/is_accepted_on_lte", 
                           &emm_s2_ctrl_data_ptr->is_accepted_on_lte, 
                           sizeof(boolean), MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_as_id )) != MCFG_FS_STATUS_OK)
       {
        emm_s2_ctrl_data_ptr->is_accepted_on_lte = FALSE;
      }
    } 
    else
    {
      /* Default value */
      emm_s2_ctrl_data_ptr->is_accepted_on_lte = FALSE;
    }
   MSG_HIGH_DS_2(MM_SUB,"=EMM= is_accepted_on_lte (status %d value %d)",
                      efs_status[0],
                      emm_s2_ctrl_data_ptr->is_accepted_on_lte
                );
    mm_as_id = MM_AS_ID_1;
    mm_sub_id = mm_sub_id_stack[mm_as_id];
  }
#endif
} 

void mm_emm_oos_rlf_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmcoord_route_oos_rlf_mm_emm_messages;  
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_oos_rlf_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr =(void *) emm_process_oos_rlf_rrc_primitive; 
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#if defined (FEATURE_LTE_REL9)
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
  else
  {
    *local_fp_ptr = (void *)NULL;
    MSG_ERROR_DS(MM_SUB,"=EMM= received invalid message in EMM queue during OOS/RLF",0,0,0);
  }
}

/*===========================================================================

FUNCTION  emm_chk_cs_rat_available

DESCRIPTION: This function checks whether cs rat available

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
void emm_chk_cs_rat_availability(emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  uint32              ratMask  = 0;
  sys_band_mask_type cgw_band_capability = 0;
  sys_lte_band_mask_e_type  lte_band_capability;
  sys_band_mask_type tds_band_capability = 0;

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&lte_band_capability);
  ASSERT(emm_ctrl_data_ptr != NULL);


  policyman_get_hardware_rats_bands(&ratMask, &cgw_band_capability, &lte_band_capability, &tds_band_capability);

  if(((ratMask & SYS_SYS_MODE_MASK_CDMA) == 0)&&
    ((ratMask & SYS_SYS_MODE_MASK_GSM) == 0)&&
    ((ratMask & SYS_SYS_MODE_MASK_WCDMA) == 0)&&
    ((ratMask & SYS_SYS_MODE_MASK_TDS) == 0))
  {
    MSG_HIGH("CS RAT is not available.",0,0,0);
    emm_ctrl_data_ptr->cs_rat_available = FALSE;
  }
  else
  {
    MSG_HIGH("CS RAT is available. rat mask %d", ratMask,0,0);
    emm_ctrl_data_ptr->cs_rat_available = TRUE;
  }
}


/*===========================================================================

FUNCTION  EMM_INITIALIZE_CTRL_DATA

DESCRIPTION
  This function initializes emm_ctrl_data structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_initialize_ctrl_data
(
  void
)
{
  uint32            count = 0 ;
  emm_ctrl_data_type   *emm_ctrl_data_ptr;
  byte    null_lai[LAI_SIZE] = { 0xff, 0xff, 0xff, 0xff, 0xfe };
  
#ifdef FEATURE_DUAL_SIM  
  for(mm_sub_id = MM_AS_ID_1, mm_as_id = MM_AS_ID_1; mm_sub_id < mm_sim_max_subscription; mm_sub_id++, mm_as_id++)
#endif
  {   
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);
    memset((void *)emm_ctrl_data_ptr,0x00,sizeof(emm_ctrl_data_type)) ;
#ifdef FEATURE_GSTK
    emm_ctrl_data_ptr->attach_ctrl_timer = 0x00;
#endif
    emm_ctrl_data_ptr->plmn_service_state  = &mm_serving_plmn.info; 
    emm_ctrl_data_ptr->t3346_plmn_ptr	  = &gmm_3gpp_t3346_plmn_id;
    emm_ctrl_data_ptr->emm_mutex_ptr = &emm_mutex;
    emm_ctrl_data_ptr->manual_ftai_list_mutex_ptr = &emm_manual_ftai_list_mutex;
    emm_ctrl_data_ptr->plmn_service_state->sim_state = SYS_SIM_STATE_NOT_AVAILABLE ;
    emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402; /* 12 minutes default value*/
    emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;
    memset((void *)&(emm_ctrl_data_ptr->t3402_attach_rej_param.plmn), 
                                 0xFF, sizeof(sys_plmn_id_s_type));
    emm_ctrl_data_ptr->t3412_value = EMM_DEFAULT_T3412;/* 54 minutes default value*/
    emm_ctrl_data_ptr->t3423_value = EMM_DEFAULT_T3412; /* 54 minutes default value. Same as T3412. 
                                                                                      This can be overwriten by attach accept or tau accept*/
#ifdef FEATURE_LTE_REL9
    emm_ctrl_data_ptr->t3442_value = 0;
#endif
    emm_ctrl_data_ptr->eps_bearer_context_status_updated = FALSE;
    emm_ctrl_data_ptr->emm_location_area = &mm_location_information;
    emm_ctrl_data_ptr->is_srvcc_in_progress = FALSE;

    emm_ctrl_data_ptr->rrc_deact_req.deact_reason = SYS_STOP_MODE_REASON_NONE;
    emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending = FALSE;
    emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_INVALID; 
    emm_ctrl_data_ptr->detach_reason = INVALID_DETACH;
    emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag = FALSE;
    emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag = FALSE;
    emm_ctrl_data_ptr->emm_nv_fake_sec_flag = FALSE;
#ifdef FEATURE_TDSCDMA
    emm_ctrl_data_ptr->emm_tds_nv_cipher_enable_flag = FALSE;
    emm_ctrl_data_ptr->emm_tds_nv_integ_enable_flag	= FALSE;
    emm_ctrl_data_ptr->emm_tds_nv_fake_sec_flag		= FALSE;
#endif
    emm_ctrl_data_ptr->update_sim_params = TRUE;
    emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming = FALSE;
    emm_ctrl_data_ptr->avoid_guti_nas_security_check = FALSE;
    emm_ctrl_data_ptr->isr_efs_flag = TRUE;
    emm_ctrl_data_ptr->emm_combined_proc_efs_flag = TRUE;
    emm_ctrl_data_ptr->last_incoming_sqn = 0;
    emm_ctrl_data_ptr->w2l_psho = FALSE;
    emm_ctrl_data_ptr->l2gcco_state = EMM_LTOG_CCO_INACTIVE;
    emm_ctrl_data_ptr->set_drx_req_pending_ptr = &gmm_set_drx_req_pending;
    emm_ctrl_data_ptr->TIN = NAS_MM_TIN_NONE;
    emm_ctrl_data_ptr->reg_lte_pended_cs_domain_param_ptr = &gmm_reg_pended_lte_cs_param;
    emm_ctrl_data_ptr->restart_mt_service_req = FALSE;
#ifdef FEATURE_DUAL_DATA
    emm_ctrl_data_ptr->is_volte_only_ps = &mm_is_volte_only_ps;
#endif
    emm_ctrl_data_ptr->ds_data_priority = SYS_DATA_PRIORITY_LOW;

    /* pended nw_sel_mode is checked for sglte. initialize it */
    if(emm_ctrl_data_ptr->reg_req_pending_ptr != NULL)
    {
     emm_ctrl_data_ptr->reg_req_pending_ptr->network_selection_mode
                                    = SYS_NETWORK_SELECTION_MODE_NONE;
    }
    
#ifdef FEATURE_LTE_REL9 
    emm_ctrl_data_ptr->emc_srv_status = LTE_NAS_EMC_SRV_NONE;
    emm_ctrl_data_ptr->non_emc_bearer_deact_in_progress = FALSE;
#endif
  
#ifdef FEATURE_LTE_REL9 
#ifndef TEST_FRAMEWORK
    emm_ctrl_data_ptr->emm_3gpp_spec_ver = LTE_3GPP_DEFAULT_RELEASE_VERSION; /*Initialize to some default value*/
#else
    #error code not present
#endif
#endif
  
    emm_ctrl_data_ptr->combined_attach_cs_rej_cause = LTE_NAS_CAUSE_NONE;
    emm_ctrl_data_ptr->emm_sms_active = FALSE;
  
    /* Init FPLMN list for combined registration */
    emm_ctrl_data_ptr->combined_reg_fplmn_list.length_of_plmn_lst_contents = 0;
    memset((void *)emm_ctrl_data_ptr->combined_reg_fplmn_list.plmn, 
                    0xFF, sizeof(sys_plmn_id_s_type) * MAX_NO_OF_PLMNs);
  
    /* Flag to indicate whether USIM supports the sotrage of EPS into or not */
    emm_ctrl_data_ptr->eps_storage_supported = FALSE;
  
    emm_ctrl_data_ptr->temp_fplmn_backoff_time = EMM_TEMP_FPLMN_DISABLED;
  
    emm_ctrl_data_ptr->mt_detach_info.type = INVALID_MT_DETACH_TYPE;
    emm_ctrl_data_ptr->mt_detach_info.emm_cause = LTE_NAS_NO_FAILURE;
  
    emm_ctrl_data_ptr->emm_rat_drx_param_to_be_sent = NAS_SEND_DRX_NONE;
  
    emm_ctrl_data_ptr->last_reg_cause = MMR_CAUSE_NONE;
  
#ifdef FEATURE_FEMTO_CSG
    emm_ctrl_data_ptr->operator_csg_list = &mm_sim_active_operator_csg_list;
    emm_ctrl_data_ptr->allowed_csg_list = &mm_sim_active_allowed_csg_list;
    emm_ctrl_data_ptr->nv_csg_support = &mm_nv_csg_support;
#endif 
#if defined(TEST_FRAMEWORK)
    #error code not present
#endif // TEST_FRAMEWORK
  
    /* Init cached PDN_CONNECTIVITY_REQ message and corresponding ESM trans ID */
    emm_ctrl_data_ptr->pdn_conn_req_ptr = NULL;
  
    emm_ctrl_data_ptr->out_msg_ptr = NULL;
  
    emm_ctrl_data_ptr->emm_ota_message_ptr  = NULL;
  
    emm_ctrl_data_ptr->nas_incoming_msg_ptr = NULL;
  
    emm_ctrl_data_ptr->pdn_conn_req_trans_id = EMM_INVALID_PDN_CONN_REQ_TRANS_ID;
  
    emm_ctrl_data_ptr->attach_pdn_seq_num = PDN_CONN_REQ_SEQ_NUM_UNASSIGNED;
  
    emm_ctrl_data_ptr->esm_srv_req_trans_id = EMM_INVALID_ESM_SRV_REQ_TRANS_ID;
  
    memset(&emm_ctrl_data_ptr->last_rrc_deactivate_cnf,0,sizeof(lte_rrc_deactivate_cnf_s));
  
    emm_ctrl_data_ptr->mm_send_mmr_stop_mode_cnf_fp = mm_send_mmr_stop_mode_cnf;
  
#ifdef FEATURE_LTE_REL9 
    emm_ctrl_data_ptr->emm_ota_message_ptr_flipped = FALSE;
#endif
    memset((void *)&emm_ctrl_data_ptr->last_rrc_bearer_ind, 0x0, 
         sizeof(lte_rrc_active_eps_bearer_update_ind_s));
  
    emm_ctrl_data_ptr->sync_drb_status = FALSE;
  
    emm_ctrl_data_ptr->aggression_management = FALSE;
    emm_ctrl_data_ptr->plmn_service_state->extend_srv_info = 
                                                      SYS_EXTEND_SRV_INFO_NONE;
    emm_ctrl_data_ptr->plmn_service_state->emc_attached = FALSE;
    emm_ctrl_data_ptr->is_accepted_on_lte = FALSE;
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr = &mm_csfb_mt_call_info;
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr->mt_csfb_highPriority = FALSE;
    emm_ctrl_data_ptr->csfb_mt_call_info_ptr->is_mt_call_recv_in_conn_state = FALSE;
  
    /* Set t311_rem_time = 0 & no_svc_cause = SYS_NO_SVC_CAUSE_NORMAL
     to prevent Service request to be sent in connected state during
     w2l handover*/
    if(emm_ctrl_data_ptr->last_rrc_service_ind_ptr != NULL)
    {
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->t311_rem_time = 0;
      emm_ctrl_data_ptr->last_rrc_service_ind_ptr->no_svc_cause = 
                                                  SYS_NO_SVC_CAUSE_NORMAL;
      emm_ctrl_data_ptr->skip_acb_for_volte_call = FALSE;
    }
    emm_ctrl_data_ptr->reg_complete_ack_pending = EMM_PENDING_REG_ACK_NONE;
    emm_ctrl_data_ptr->is_plmn_block_req_pending = FALSE;
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr->lte_trm_priority = LTE_TRM_PRIORITY_LOW;
#endif
#ifdef FEATURE_LTE_REL9
    emm_ctrl_data_ptr->mmit_parameters.utran_ps_voice_parameters_ptr = &mm_utran_ps_voice_parameters;
    emm_ctrl_data_ptr->mmit_parameters.ims_registration_parameters.is_ims_reg_for_voice_tag = FALSE;
    emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_ps_voice_on_eutran_supported = FALSE;
    emm_ctrl_data_ptr->mmit_parameters.eutran_ps_voice_parameters.is_last_eutran_voims_supported = FALSE;
#endif

    sys_plmn_undefine_plmn_id(&(emm_ctrl_data_ptr->rplmn_info.rplmn));
    emm_ctrl_data_ptr->emm_lte_mode_change_pending = FALSE;
    emm_ctrl_data_ptr->ps_resumption_status = EMM_PS_RESUMPTION_NONE;
    emm_ctrl_data_ptr->rr_blind_redir_pending_call_abort = FALSE;

  
    for(count = MIN_EMM_TIMER; count < MAX_EMM_TIMER; count++)
    {
      mm_timer_status[count] = TIMER_STOPPED;
    }
  
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_emergency_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_EMERGENCY_BARRED) ;
  
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mt_access_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MT_ACCESS_BARRED) ;
  
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_signaling_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_SIGNALING_BARRED) ;
  
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_data_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_DATA_BARRED) ;
  
#ifdef FEATURE_LTE_REL10
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_mo_csfb_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_MO_CSFB_BARRED);
    rex_def_timer_ex(&emm_ctrl_data_ptr->emm_access_barring_delay_tolerant_timer, (rex_timer_cb_type)emm_barring_call_back, (dword)EMM_DELAY_TOLERANT_BARRED);
#endif
  
    emm_ctrl_data_ptr->ignore_GW_activation_ind = FALSE;
    emm_ctrl_data_ptr->esr_type_in_progress = NAS_EMM_CSFB_NONE;
    emm_ctrl_data_ptr->pended_esr_msg = NULL;
  
    emm_ctrl_data_ptr->plmn_service_state->lte_cs_capability = SYS_LTE_CS_CAPABILITY_NONE;
  
    emm_ctrl_data_ptr->is_sglte_device  = FALSE;
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr->imsi_not_native = FALSE;
    emm_ctrl_data_ptr->tau_trm_priority = FALSE;
#endif
    emm_ctrl_data_ptr->conn_mode_manual_search_ptr = &mm_conn_mode_manual_search_enabled;
    emm_ctrl_data_ptr->access_class = LTE_RRC_INVALID_ACCESS_CLASS;
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr->dds_switch_pended_ptr  = &gmm_force_local_detach_sim[mm_sub_id];
#endif
    emm_ctrl_data_ptr->rpm_ftai_list_ptr = NULL;
#ifdef FEATURE_LTE_REL10
    emm_ctrl_data_ptr->delay_tolerant_access = FALSE;
    emm_ctrl_data_ptr->responding_to_page = FALSE;
    emm_ctrl_data_ptr->extended_wait_time = 0;
#endif
    emm_ctrl_data_ptr->psm_local_detach_enabled = FALSE;
#ifdef FEATURE_NAS_EAB
    emm_ctrl_data_ptr->eab_barring = FALSE;
    emm_ctrl_data_ptr->eab_barring_flag = FALSE;
#endif
    emm_ctrl_data_ptr->emm_cs_loci_info.lai_exists = FALSE;
    memscpy( (void *)emm_ctrl_data_ptr->emm_cs_loci_info.lai, LAI_SIZE, null_lai, LAI_SIZE );
  }
#ifdef FEATURE_DUAL_SIM
  mm_sub_id = MM_AS_ID_1;
  mm_as_id = MM_AS_ID_1;
#endif
}


/*===========================================================================

FUNCTION  EMM_INITIALIZE

DESCRIPTION
  This function initializes emm related data

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_initialize
(
  msgr_client_t        *esm_client,
  msgr_client_t        *rrc_client,
  msgr_client_t        *tlb_client,
  msgr_client_t        *emm_csfb_client
#if defined (FEATURE_LTE_REL9)
  ,
  msgr_client_t        *emm_gps_client
#endif /* FEATURE_LTE_REL9 */
)
{
  errno_enum_type e_status ;
  q_type          *q_ptr;
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  ASSERT(esm_client != (msgr_client_t *)NULL);
  ASSERT(rrc_client != (msgr_client_t *)NULL);
  ASSERT(tlb_client != (msgr_client_t *)NULL);
  ASSERT(emm_csfb_client != NULL);

#if defined (FEATURE_LTE_REL9)
  ASSERT(emm_gps_client != (msgr_client_t *)NULL);
#endif


  emm_initialize_ctrl_data();
#ifdef FEATURE_DUAL_SIM
  mm_as_id = MM_AS_ID_1;
  mm_sub_id = MM_AS_ID_1;
#endif
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  emm_ctrl_data_mem_alloc(emm_ctrl_data_ptr);

  lte_nas_efs_read();

  /* ESM input Qs */

  q_ptr = q_init(&esm_esm_cmd_q_msg);

  ASSERT( q_ptr );



  q_ptr = q_init(&tlb_tlb_cmd_q_msg);

  ASSERT( q_ptr );



  q_ptr = q_init(&rrc_emm_cmd_q_msg);

  ASSERT( q_ptr );


  q_init(&emm_csfb_cmd_q_msg);

#if defined (FEATURE_LTE_REL9)
  /* IRAT input Qs */

  q_init(&emm_gps_cmd_q_msg);

#endif

  emm_init_msglib_decode_func_ptr(decode_nas_msg);
  emm_init_msglib_encode_func_ptr(encode_nas_msg);
  emm_init_msglib_retrieve_func_ptr(nas_retrieve_msg);

  //*mm_emm_dispatcher_fn_ptr = mm_emm_normal_msg_handler;

  /* Register with message router */
  e_status = msgr_client_create(esm_client);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_client_set_rex_q(esm_client,mm_tcb_ptr,
                                   ESM_EMM_CMD_Q_SIG,&esm_esm_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( esm_cmd_type, cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_cmd_type));
  ASSERT(e_status == E_SUCCESS);   

  e_status = msgr_client_create(tlb_client);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_client_set_rex_q(tlb_client,mm_tcb_ptr,
                                   TLB_EMM_CMD_Q_SIG,&tlb_tlb_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( tlb_cmd_type, cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(tlb_cmd_type));
  ASSERT(e_status == E_SUCCESS); 

  e_status = msgr_client_create(rrc_client);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_client_set_rex_q(rrc_client,mm_tcb_ptr,
                                   RRC_EMM_CMD_Q_SIG,&rrc_emm_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( emm_rrc_cmd_type, rrc_msgr_cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_rrc_cmd_type));
  ASSERT(e_status == E_SUCCESS);   

  e_status = msgr_client_create(emm_csfb_client);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_client_set_rex_q(emm_csfb_client,
                                   mm_tcb_ptr,
                                   CSFB_EMM_CMD_Q_SIG,
                                   &emm_csfb_cmd_q_msg,
                                   MSGR_NO_QUEUE,
                                   NAS_STD_OFFSETOF(emm_csfb_cmd_type, cmd),
                                                   MSGR_NO_OFFSET,
                                   (uint16)sizeof(emm_csfb_cmd_type));
  ASSERT(e_status == E_SUCCESS);   

#if defined (FEATURE_LTE_REL9)
  e_status = msgr_client_create(emm_gps_client);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_client_set_rex_q(emm_gps_client,mm_tcb_ptr,
                                   GPS_EMM_CMD_Q_SIG,&emm_gps_cmd_q_msg,
                                   MSGR_NO_QUEUE,NAS_STD_OFFSETOF( emm_gps_cmd_type, gps_msgr_cmd ),
                                   MSGR_NO_OFFSET,(uint16)sizeof(emm_gps_cmd_type));
  ASSERT(e_status == E_SUCCESS);
#endif /*FEATURE_LTE_REL9 */

  e_status = msgr_register(MSGR_NAS_EMM,tlb_client,MSGR_ID_NONE,LTE_TLB_UL_INFO_REQ);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_SERVICE_REQ);
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_DATA_REQ);
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_DETACH_CMD);
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_EPS_BEARER_STATUS_CMD) ;
  ASSERT(e_status == E_SUCCESS); 
#ifdef FEATURE_LTE_REL9 
  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_EMC_SRV_STATUS_CMD) ;
  ASSERT(e_status == E_SUCCESS);  
#endif
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CONN_EST_CNF) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_UL_DATA_CNF) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_DEACTIVATE_CNF) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_ACTIVATION_IND) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_SERVICE_IND) ;
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_BARRING_UPDATE_IND) ;
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_PESUDO_RESEL_LOW_PRIORITY_IND) ;
  ASSERT(e_status == E_SUCCESS);       
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CONN_REL_IND) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_DL_DATA_IND) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_PAGE_IND) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_ACTIVE_EPS_BEARER_UPDATE_IND) ;
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_PLMN_SEARCH_CNF);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_NAS_UMTS_KEY_IND);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_NAS_GSM_KEY_IND);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_NAS_LTE_KEY_IND);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_TUNNEL_UL_MSG_CNF);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_TUNNEL_DL_MSG_IND);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CCO_NACC_COMPLETED_IND);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_CSG_INFO_UPDATE_IND);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,emm_csfb_client,MSGR_ID_NONE,NAS_EMM_IRAT_UL_MSG_REQ);
  ASSERT(e_status == E_SUCCESS);  
#ifdef FEATURE_LTE_TO_HDR_OH  
  /* UL and DL messages for LTE - DO interface */
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_HDR_TUNNEL_UL_MSG_CNF);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_IRAT_HDR_TUNNEL_DL_MSG_IND);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,emm_csfb_client,MSGR_ID_NONE,NAS_EMM_IRAT_HDR_UL_MSG_REQ);
  ASSERT(e_status == E_SUCCESS);  
#endif

  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_1XCSFB_ESR_CALL_REQ);
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,esm_client,MSGR_ID_NONE,NAS_EMM_1XCSFB_ESR_CALL_ABORT_REQ);
  ASSERT(e_status == E_SUCCESS);   
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XCSFB_HO_STARTED_IND);
  ASSERT(e_status == E_SUCCESS);  
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XCSFB_HO_FAILED_IND);
  ASSERT(e_status == E_SUCCESS);  
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XSRVCC_HO_STARTED_IND);
  ASSERT(e_status == E_SUCCESS); 
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_1XSRVCC_HO_FAILED_IND);
  ASSERT(e_status == E_SUCCESS);  
#if defined (FEATURE_LTE_REL9)
  e_status = msgr_register(MSGR_NAS_EMM,emm_gps_client,MSGR_ID_NONE,NAS_EMM_UL_GENERIC_NAS_TRANSPORT_REQ);
  ASSERT(e_status == E_SUCCESS);
#endif /*FEATURE_LTE_REL9 */
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_SIB6_INFO_IND);
  ASSERT(e_status == E_SUCCESS);
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_SIB7_INFO_IND);
  ASSERT(e_status == E_SUCCESS);
#ifdef FEATURE_NAS_EAB
  e_status = msgr_register(MSGR_NAS_EMM,rrc_client,MSGR_ID_NONE,LTE_RRC_EAB_UPDATE_IND);
  ASSERT(e_status == E_SUCCESS);
#endif
  /*Notify T3402 clients of the T3402 value*/
  emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
  emm_chk_cs_rat_availability(emm_ctrl_data_ptr);
} /* end of emm_initialize() */

/*===========================================================================

FUNCTION  EMM_PROCESS_TIMER_EXPIRY

DESCRIPTION
  This function processes timer expiry and calls corresponding handler

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_timer_expiry
(
  mm_cmd_type         *cmd_ptr, 
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  log_lte_nas_emm_timer_event_type event;

  ASSERT(cmd_ptr != NULL);
  ASSERT(emm_ctrl_data_ptr != NULL);

  gs_clear_timer_ext( 
                      get_mm_timer_queue(),
                      cmd_ptr->cmd.timer_expiry.timer_id,
                      mm_timer_non_deferrable( cmd_ptr->cmd.timer_expiry.timer_id )
                    );

  if (((mm_timer_status[cmd_ptr->cmd.timer_expiry.timer_id] == TIMER_ACTIVE) &&
       ((dword) gs_enquire_timer_ext (
         get_mm_timer_queue(),
         cmd_ptr->cmd.timer_expiry.timer_id,
         mm_timer_non_deferrable(cmd_ptr->cmd.timer_expiry.timer_id)) == 0))
      ||
      (cmd_ptr->cmd.timer_expiry.timer_id == EMM_ACCESS_BARRING_TIMER))
  {
    if(((cmd_ptr->cmd.timer_expiry.timer_id >= MIN_EMM_TIMER) &&
       (cmd_ptr->cmd.timer_expiry.timer_id <= MAX_EMM_TIMER)) ||
       (cmd_ptr->cmd.timer_expiry.timer_id == TIMER_T3346))
    {
      mm_timer_status[cmd_ptr->cmd.timer_expiry.timer_id] = TIMER_STOPPED;
    }
    MSG_HIGH_DS_1(MM_SUB,"=EMM= TIMER = %d has expired", cmd_ptr->cmd.timer_expiry.timer_id);
    /* ---------------------------------------------
     ** Send timer message off to appropriate handler
     ** --------------------------------------------- */
    switch (cmd_ptr->cmd.timer_expiry.timer_id)
    {
      case TIMER_T3410:
        emm_timer_t3410_expire(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3402:
        emm_attach_timer_t3402_t3411_expire(emm_ctrl_data_ptr,
                                            cmd_ptr->cmd.timer_expiry.timer_id);
        break;
  
      case TIMER_T3411:
        emm_attach_timer_t3402_t3411_expire(emm_ctrl_data_ptr,
                                            cmd_ptr->cmd.timer_expiry.timer_id);
        break;
  
      case EMM_POWEROFF_DETACH_TIMER:
        emm_powerdown_timer_expire(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3421:
        emm_t3421_expire(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3417:
        emm_timer_t3417_expire(emm_ctrl_data_ptr) ;
        break;
  
      case TIMER_T3418:
      case TIMER_T3420:
        auth_handle_timer_expiry(cmd_ptr);
      break;
  
      case TIMER_T3416: 
        /* cache guard timer */
        auth_reset_cache_memory(TRUE,
                                cmd_ptr->cmd.timer_expiry.timer_id,
                                RRC_PS_DOMAIN_CN_ID);
        break;
        
      case TIMER_T3440:
        emm_timer_t3440_expire(emm_ctrl_data_ptr) ;
        break;
  
      case TIMER_T3412:
        emm_timer_t3412_expire(emm_ctrl_data_ptr) ;
        break;
  
      case TIMER_T3430:
        emm_timer_t3430_expire(emm_ctrl_data_ptr) ;
        break;
  
      case EMM_ACCESS_BARRING_TIMER:
        emm_process_access_barring_timer_expiry(emm_ctrl_data_ptr);
        break;
  
      case EMM_FORBIDDEN_TAI_CLEAR_TIMER:
        emm_forbidden_tai_clear_timer_expiry(emm_ctrl_data_ptr);
        break;
  
      case TIMER_T3423:
        emm_timer_t3423_expire(emm_ctrl_data_ptr) ;
        break;
      case TIMER_T3417_EXT:
        emm_timer_t3417_ext_expire(emm_ctrl_data_ptr) ;
        break;
      case TIMER_EMM_PS_DETACH:
        emm_ps_detach_clean_up(emm_ctrl_data_ptr);
        reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);
        break;
  #ifdef FEATURE_LTE_REL9
      case TIMER_T3442:
        MSG_HIGH_DS_0(MM_SUB,"=EMM= T3442 timer expired");
        break;
  #endif
#ifdef FEATURE_LTE_REL10
    case TIMER_T3346:
      emm_timer_t3346_expire(FALSE, emm_ctrl_data_ptr) ;
/*Reset LU initiated flag to flase since 3346 expired.*/
      mm_is_lu_to_be_initiated_t3346_ltogw = FALSE;
      break;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_1XSRLTE
    case TIMER_EMM_RADIO_RETRY:
      emm_timer_radio_retry_expire(emm_ctrl_data_ptr) ;
      break;
#endif 

#ifdef FEATURE_1XSRLTE 
      case TIMER_SRLTE_ESR:
        emm_srlte_esr_timer_expire(emm_ctrl_data_ptr);
        break;
#endif
#ifdef FEATURE_SGLTE
      case TIMER_DELAY_TAU:
        emm_timer_delay_tau_expire(emm_ctrl_data_ptr) ;
        break;
#endif
#ifdef FEATURE_TDSCDMA
      case TIMER_POLICY_CHANGE_PENDING:
        emm_timer_policy_change_pending_expire(emm_ctrl_data_ptr) ;
        break;
#endif 
      default:
        MSG_FATAL_DS(MM_SUB, "Unexpected timer ID %d",cmd_ptr->cmd.timer_expiry.timer_id, 0,0 );
        break;
    }
  }
  else
  {
     MSG_ERROR_DS(MM_SUB,"=EMM= Timer %d expired after being stopped or restarted", cmd_ptr->cmd.timer_expiry.timer_id,0,0);  
  }
  
  event.emm_timer_event = cmd_ptr->cmd.timer_expiry.timer_id;
  event_report_payload(EVENT_LTE_EMM_TIMER_EXPIRY,sizeof(event),(void*)&event);
}

boolean emm_process_unexpected_rrc_primitive
(
  void                 *rrc_message_router_cmd,
  emm_ctrl_data_type   *emm_ctrl_data_ptr,
  mm_emm_dispatcher_fp **mm_emm_dispatcher_fn_ptr
)
{
  emm_rrc_cmd_type   *rrc_msgr_cmd = NULL;

  ASSERT(rrc_message_router_cmd != (void *)NULL);
  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL);

  rrc_msgr_cmd = (emm_rrc_cmd_type*)rrc_message_router_cmd;

  MSG_ERROR_DS(MM_SUB,"Unexpected RRC message %d - discarded",
           rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id,0,0);

  switch(rrc_msgr_cmd->rrc_msgr_cmd.msg_hdr.id)
  {
    case LTE_RRC_DL_DATA_IND:
      emm_free_dsm_memory_in_rrc_data_ind((lte_rrc_dl_data_ind_s *)\
                &(rrc_msgr_cmd->rrc_msgr_cmd.rrc_dl_data_ind));
      break;

    default:
      break;
  }

  return TRUE;
}

void emm_process_LTE_to_GW_rat_change(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmmultimode_route_L2GW_irat_mm_emm_messages;  
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_irat_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_irat_rrc_primitive; 
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#if defined (FEATURE_LTE_REL9)
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
  else
  {
    *local_fp_ptr = NULL;
    MSG_ERROR_DS(MM_SUB,"=MM= received invalid message in MM/EMM queue during Inter-RAT",0,0,0);
  }
}

void mm_process_GW_to_LTE_rat_change(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)mmmultimode_route_GW2L_irat_mm_emm_messages;  
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_irat_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = (void *)emm_process_unexpected_rrc_primitive;
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }
#if defined (FEATURE_LTE_REL9)
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
  else
  {
    *local_fp_ptr = NULL;
    MSG_ERROR_DS(MM_SUB,"=MM= received invalid message in MM/EMM queue during Inter-RAT",0,0,0);
  }
}
#endif/*FEATURE_LTE*/

void mm_emm_normal_msg_handler(rex_sigs_type sigs,void **mm_process_msg_fn_ptr)
{
  mm_process_messages_fp **local_fp_ptr = ((mm_process_messages_fp **)mm_process_msg_fn_ptr);

  if (sigs & MM_CMD_Q_SIG)
  {
    *local_fp_ptr = mmcoord_route_mm_emm_messages;  
  }
#ifdef FEATURE_LTE
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = emm_process_outgoing_esm_message; 
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
    *local_fp_ptr = emm_process_rrc_primitive; 
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_outgoing_tlb_message; 
  }
  else if(sigs & CSFB_EMM_CMD_Q_SIG  )
  {
    *local_fp_ptr = emm_process_csfb_primitive; 
  }

#if defined (FEATURE_LTE_REL9)
  else if(sigs & GPS_EMM_CMD_Q_SIG )
  {
    *local_fp_ptr = emm_process_ul_generic_nas_transport_msg; 
  } 
#endif
#endif
  else
  {
    *local_fp_ptr = NULL;
  }
}

/*===========================================================================

FUNCTION FREE_CMD_QUEUE

DESCRIPTION
  This function clears the command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void free_cmd_queue(void *cmd_ptr, mm_process_messages_fp *mm_process_fp)
{
  if((mm_process_fp == mmcoord_route_mm_emm_messages) 

#ifdef FEATURE_LTE 
     ||(mm_process_fp == mmcoord_route_oos_rlf_mm_emm_messages)
     ||(mm_process_fp == mmmultimode_route_L2GW_irat_mm_emm_messages)
     ||(mm_process_fp == mmmultimode_route_GW2L_irat_mm_emm_messages)
#endif
     )
  {
    mm_free_cmd_buf( cmd_ptr );
  }

#ifdef FEATURE_LTE
  else if((mm_process_fp == emm_process_outgoing_esm_message)||
          (mm_process_fp == emm_process_oos_rlf_outgoing_esm_message) ||
          (mm_process_fp == emm_process_irat_outgoing_esm_message))
  {
    msgr_rex_free_msg_buf(&(((emm_cmd_type*)cmd_ptr)->link));
  }
  else if((mm_process_fp == emm_process_rrc_primitive)||
          (mm_process_fp == emm_process_oos_rlf_rrc_primitive)
          ||(mm_process_fp == emm_process_irat_rrc_primitive)
          ||(mm_process_fp == emm_process_unexpected_rrc_primitive))
  {
    msgr_rex_free_msg_buf(&(((emm_rrc_cmd_type*)cmd_ptr)->link));
  }
  else if(mm_process_fp == emm_process_outgoing_tlb_message)
  {
    msgr_rex_free_msg_buf(&(((tlb_cmd_type*)cmd_ptr)->link));
  }
  else if(mm_process_fp == emm_process_csfb_primitive)
  {
    msgr_rex_free_msg_buf(&(((emm_csfb_cmd_type*)cmd_ptr)->link));
  }
#if defined (FEATURE_LTE_REL9)
  else if(mm_process_fp == emm_process_ul_generic_nas_transport_msg )
  {
    msgr_rex_free_msg_buf(&(((emm_gps_cmd_type*)cmd_ptr)->link));
  }
#endif /*FEATURE_LTE_REL9 */
#endif
  else
  {
    MSG_ERROR_DS(MM_SUB,"=MM= Unable to free queue w/ fp %x", mm_process_fp,0,0);
  }
}

/*===========================================================================

FUNCTION mm_emm_get_as_id

===========================================================================*/
void mm_emm_get_as_id
(
  void *cmd_ptr, 
  rex_sigs_type sigs
)
{
#ifndef FEATURE_LTE
  mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                   );

#else

  mm_as_id = MM_AS_ID_1;
  mm_sub_id = MM_AS_ID_1;
  if (sigs & MM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    (void)mm_get_as_id((mm_cmd_type*) cmd_ptr);
#endif

    if((((mm_cmd_type*) cmd_ptr)->cmd.hdr.message_set == MS_CC_MM ||
        ((mm_cmd_type*) cmd_ptr)->cmd.hdr.message_set == MS_CC_MM_PS) &&
        (((mm_cmd_type*) cmd_ptr)->cmd.hdr.message_id == MMCNM_DATA_REQ))
    {
       mm_set_mm_controls_resel_dec_mt_cs_call((mm_cmd_type*) cmd_ptr);
    }

#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif
    mm_add_message_to_debug_buffer(((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_set,
                                   ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id, TO_MM
                                   ,(sys_modem_as_id_e_type)mm_as_id
                                   );
    if((mm_sim_card_mode == MMGSDI_APP_USIM) &&
       mm_is_multimode_sub() 
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
       && (mm_sub_capability == SUBS_CAPABILITY_MULTIMODE 
       || ((mm_cmd_type*)cmd_ptr)->cmd.hdr.message_id != MMR_SIM_NOT_AVAILABLE_REQ)
#endif
      )
    {
      emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
    }
  }
  else if(sigs & ESM_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_esm_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif
    emm_add_message_to_debug_buffer(((emm_cmd_type*)cmd_ptr)->emm_msgr_cmd.msg_hdr.id, 
                                    TO_EMM,
                                    (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
  else if(sigs & RRC_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_rrc_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif
      /* Don't store for DL data ind here will add to debug buffer later*/
     if ((((emm_rrc_cmd_type*)cmd_ptr)->rrc_msgr_cmd.msg_hdr.id)!= LTE_RRC_DL_DATA_IND)
     {
       emm_add_message_to_debug_buffer(((emm_rrc_cmd_type*)cmd_ptr)->rrc_msgr_cmd.msg_hdr.id,
                                         TO_EMM,(sys_modem_as_id_e_type)mm_as_id);
	 }
                                     
     emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
  else if(sigs & TLB_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_tlb_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif
     emm_add_message_to_debug_buffer(((tlb_cmd_type*)cmd_ptr)->cmd.hdr.id,
                                      TO_EMM,
                                      (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
  else if(sigs & CSFB_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_irat_csfb_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif
    emm_add_message_to_debug_buffer(((emm_csfb_cmd_type*)cmd_ptr)->cmd.msg_hdr.id,
                                     TO_EMM,
                                     (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }
#if defined (FEATURE_LTE_REL9)
  else if(sigs & GPS_EMM_CMD_Q_SIG)
  {
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    emm_get_gps_as_id(cmd_ptr);
#endif
#ifdef FEATURE_DUAL_SIM
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT(emm_ctrl_data_ptr != NULL);
#endif
    emm_add_message_to_debug_buffer(((emm_gps_cmd_type*)cmd_ptr)->gps_msgr_cmd.msg_hdr.id,
                                     TO_EMM,
                                     (sys_modem_as_id_e_type)mm_as_id);
    emm_ctrl_data_mem_alloc_check(emm_ctrl_data_ptr);
  }

#endif /*FEATURE_LTE_REL9 */
#endif

}


/*===========================================================================

FUNCTION MM_READ_QUEUE

DESCRIPTION
  This function gets the messages from the queue one at a time untile the queue is exhausted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_read_queue(q_type *queue, rex_sigs_type sigs)
{  
  void *cmd_ptr = NULL;
  boolean return_val = FALSE;
  void *process_msg_func_ptr = NULL;

  ASSERT(queue != (q_type *)NULL);

  while((cmd_ptr = (void*) q_get(queue)) != NULL)
  {
    /* Parse the as_id and add received messages to the debug buffer */
    mm_emm_get_as_id(cmd_ptr,sigs);
    mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);

    /*Note: Following order should be maintained 
    process the message --> free the queue  -->reset/set the mm_emm_dispatcher_fn_ptr*/
/*lint -save -e611 */
    return_val = ((mm_process_messages_fp*)process_msg_func_ptr)(
                                                                    cmd_ptr,
#ifdef FEATURE_LTE
                                                                    emm_ctrl_data_ptr,
#endif
                                                                    &mm_emm_dispatcher_func_ptr
                                                                   );    
 
    free_cmd_queue(cmd_ptr,(mm_process_messages_fp *)process_msg_func_ptr); 
   
   /* For OOS/RLF/INTER_RAT features, mm_emm_dispatcher_fn_ptr will be changed when service is lost or found, 
   once this pointer changes - we need to set/reset the mm_process_fp correctly before processing any 
   new incoming message, so that all incoming messages are handled appropriately. 
   In case there are two or more messages in the queue (after mm_emm_dispatcher_fn_ptr is changed), 
   then we need to immedicately change mm_process_fp, so call the  mm_emm_dispatcher_fn_ptr() here*/
   mm_emm_dispatcher_func_ptr(sigs,&process_msg_func_ptr);
  }
  /*lint -restore */
  return return_val;
}

#ifdef FEATURE_LTE
  msgr_client_t    esm_client;
  msgr_client_t    rrc_client;
  msgr_client_t    tlb_client;
  msgr_client_t    emm_csfb_client;

#if defined (FEATURE_LTE_REL9)
 msgr_client_t    emm_gps_client;
#endif

#endif


static void mm_init_main_routine_handler(void)
{
    /* Set the main routine handler to normal_msg_handler to begin with*/
#if defined (FEATURE_SGLTE)|| defined (FEATURE_DUAL_SIM)
  byte index;
  for(index = 0; index < MAX_NAS_STACKS; index++)
  {
    mm_emm_dispatcher_func_ptr_sim[index] = mm_emm_normal_msg_handler;
  }
#else
  mm_emm_dispatcher_func_ptr = mm_emm_normal_msg_handler;
#endif
}


/*===========================================================================

FUNCTION MM_EXTERN_INIT

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_extern_init (void)
{
   /* Initialize the Critical Section */
   rex_init_crit_sect(&timer_crit_sect); 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&nas_nv_crit_sect); 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&mm_ftd_crit_sect); 
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&mm_sim_crit_sect); 
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST

   rex_init_crit_sect(&mm_rat_prioritylist_crit_sec);

#endif
}

/*===========================================================================

FUNCTION MM_INIT_BEFORE_TASK_START

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_init_before_task_start (void)
{
   /* Initialize command queue */
   (void)q_init(&mm_cmd_q);

   /* Initialize gs message passing for this queue */
   if (gs_init_for_send_only(GS_QUEUE_MM,
         (gs_put_handler_ptr)mm_send_routine) != GS_SUCCESS)
   {
       MSG_ERROR_DS(MM_SUB, "=MM= MM not initialized!",0,0,0);
   }

#ifndef FEATURE_MODEM_RCINIT
   /* Clear signals */
   (void) rex_clr_sigs(mm_tcb_ptr, (rex_sigs_type)~0);
#endif
#ifndef FEATURE_MODEM_RCINIT
   /* Initialize the Critical Section */
   rex_init_crit_sect(&timer_crit_sect); 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&nas_nv_crit_sect); 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&mm_ftd_crit_sect); 
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
   /* Initialize the Critical Section */
   rex_init_crit_sect(&mm_sim_crit_sect); 
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST

   rex_init_crit_sect(&mm_rat_prioritylist_crit_sec);

#endif
#endif /*FEATURE_MODEM_RCINIT*/
}


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
FUNCTION MM_MCFG_REFRESH_CB

DESCRIPTION
This is a callback function used by MCFG for NV/EFS refresh purpose
This function puts the received refresh info into MM command queue

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
   mm_cmd_type   *cmd_ptr;

   ASSERT(p_info != NULL);

   if(p_info->type != MCFG_REFRESH_TYPE_SUBS)
   {
     MSG_ERROR_DS_1(MM_SUB,"=MM= Unexpected refresh type %d rcvd in mm_mcfg_refresh_cb", p_info->type);
     return FALSE;
   }

   /* Get command buffer from MM task */
   if( (cmd_ptr = mm_get_cmd_buf()) != NULL)
   {
      /* Construct message */
     cmd_ptr->cmd.mcfg_refresh_ind.IMH.message_set = MS_MM_MM;
     cmd_ptr->cmd.mcfg_refresh_ind.IMH.message_id  = MM_MCFG_REFRESH_IND;

     PUT_IMH_LEN (sizeof(cmd_ptr->cmd.mcfg_refresh_ind) - sizeof(IMH_T),
                       &(cmd_ptr->cmd.mcfg_refresh_ind.IMH));

     cmd_ptr->cmd.mcfg_refresh_ind.as_id = (sys_modem_as_id_e_type)p_info->sub_index;
     cmd_ptr->cmd.mcfg_refresh_ind.reference_id = p_info->reference_id;

     /* Put on MM command queue */
     mm_put_cmd(cmd_ptr);
   }
   else
   {
     MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
   }
   return TRUE;
}

/*===========================================================================
FUNCTION MM_MCFG_REFRESH_REGISTER

DESCRIPTION
This function call registers MM Task with MCFG for NV/EFS refresh purpose
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mm_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type mcfg_reg;

  memset((void *)&mcfg_reg,0x00,sizeof(mcfg_refresh_registration_s_type));

  mcfg_reg.order = MCFG_REFRESH_ORDER_50;  
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.group = 0;
  mcfg_reg.cb = mm_mcfg_refresh_cb;

  if(mcfg_refresh_register(&mcfg_reg) != TRUE)
  {
     MSG_ERROR_DS_0( MM_SUB, "=MM= ERROR: MCFG Registration failed");    
  }
}
#endif

/*===========================================================================

FUNCTION MM_MAIN

DESCRIPTION
  Entry point for the MM task.  This function performs task initialisation,
  then sits in an infinite loop, waiting on an input queue, and responding to
  messages received.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef GMM_DEBUG
int main( int argc, char *argv[] )
/*lint -esym( 715, argv, argc ) */
#else
void mm_main( dword dummy )
/*lint -esym(715,dummy) */
#endif
{
  static rex_sigs_type wait_sigs,sigs;

#if defined(FEATURE_LTE) && defined(TEST_FRAMEWORK)
   #error code not present
#endif /*FEATURE_LTE*/   

   mm_tcb_ptr = rex_self();

#ifndef FEATURE_MODEM_RCINIT
   mm_init_before_task_start ();
#endif

   /* -------------------------
   ** Initialize watchdog timer
   ** ------------------------- */
#ifndef TEST_FRAMEWORK    
   /* Register with DOG HB new API */
   mm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) MM_DOGRPT_TIMER_SIG);
#endif

#ifdef FEATURE_MODEM_RCINIT
   rcinit_handshake_startup();
#else
#ifndef FEATURE_MODEM_RCINIT_PHASE2
   tmc_task_start();
#endif
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
   mm_mcfg_refresh_register();
#endif

   nas_set_efs_nas_mm_config();


#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
   mm_read_device_configuration();
#endif 

#ifdef FEATURE_LTE
   emm_db_alloc_memory();
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_as_id = MM_AS_ID_1;
   mm_sub_id = MM_AS_ID_1;
#endif
   emm_ctrl_data_ptr  = emm_db_get_ctrl_data();
#endif
   mm_initialise();
   gmm_initialize();
   /* Set the main routine handler to normal_msg_handler to begin with*/
   mm_init_main_routine_handler();
#ifdef FEATURE_LTE   
   emm_initialize(&esm_client, 
                  &rrc_client, 
                  &tlb_client, 
                  &emm_csfb_client
#if defined (FEATURE_LTE_REL9)
                  ,
                  &emm_gps_client
#endif /* FEATURE_LTE_REL9*/
                  );
#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_as_id = MM_AS_ID_1;
   mm_sub_id = MM_AS_ID_1;
#endif

#ifdef FEATURE_NAS_DIAG_INIT
  nas_diag_init();
#endif

   wait_sigs = MM_CMD_Q_SIG  
#ifdef FEATURE_LTE
               | ESM_EMM_CMD_Q_SIG | 
               RRC_EMM_CMD_Q_SIG   |
               TLB_EMM_CMD_Q_SIG   |
               CSFB_EMM_CMD_Q_SIG
#if defined (FEATURE_LTE_REL9)
               |
               GPS_EMM_CMD_Q_SIG
#endif
#endif /*FEATURE_LTE*/
#ifdef TEST_FRAMEWORK
               #error code not present
#endif /*TEST_FRAMEWORK*/              
               ;             
   for(;;)
   {            
      /* Wait on signal mask*/
      sigs = mm_wait( wait_sigs );
      /* If this signal is for the command queue */
      if (sigs & MM_CMD_Q_SIG)
      {
         /* Clear signals */
        (void) rex_clr_sigs( mm_tcb_ptr, MM_CMD_Q_SIG );
/*lint -save -e611 */
        (void)mm_read_queue(&mm_cmd_q,MM_CMD_Q_SIG);
 /*lint -restore */
      }
#ifdef FEATURE_LTE      
      else if (sigs & ESM_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, ESM_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&esm_esm_cmd_q_msg,ESM_EMM_CMD_Q_SIG);
/*lint -restore */
      }
      else if (sigs & RRC_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, RRC_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&rrc_emm_cmd_q_msg,RRC_EMM_CMD_Q_SIG);
/*lint -restore */
      }
      else if(sigs & TLB_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, TLB_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&tlb_tlb_cmd_q_msg,TLB_EMM_CMD_Q_SIG);
/*lint -restore */
      }
      else if(sigs & CSFB_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, CSFB_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&emm_csfb_cmd_q_msg,CSFB_EMM_CMD_Q_SIG);
/*lint -restore */
      }
#if defined (FEATURE_LTE_REL9)
      else if(sigs & GPS_EMM_CMD_Q_SIG)
      {
        (void) rex_clr_sigs( mm_tcb_ptr, GPS_EMM_CMD_Q_SIG);

/*lint -save -e611 */
        (void)mm_read_queue(&emm_gps_cmd_q_msg,GPS_EMM_CMD_Q_SIG);
/*lint -restore */
      }
#endif /* FEATURE_LTE_REL9 */
#endif /*FEATURE_LTE*/
#ifdef TEST_FRAMEWORK
      #error code not present
#endif/*TEST_FRAMEWORK*/     
   }/* FOREVER */
}



