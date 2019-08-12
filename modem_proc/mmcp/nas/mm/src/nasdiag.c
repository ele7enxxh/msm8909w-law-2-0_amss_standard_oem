
/*======================================================================  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/nasdiag.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $  
 
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
========================================================================*/

#include "mmcp_variation.h"
#include "nasdiag.h"
#include "diagcmd.h"
#include "gmm_v.h"
#include "gmm_int_v.h"
#include "gmmllcm.h"
#include "sys_eplmn_list_v.h"
#include <stringl/stringl.h>


#ifdef FEATURE_NAS_DIAG_INIT

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


#ifdef FEATURE_DUAL_SIM
/*===========================================================================
Function: nasdiag_validate_req_parameters
Description:

       This function validates the input parameters in request packet

============================================================================*/
static boolean nasdiag_validate_req_parameters
(  uint16 pkt_len,
   PACKED const void* req_ptr,
   uint8 packet_type
)
{
  boolean ret_status = TRUE;
  sys_modem_as_id_e_type req_as_id = SYS_MODEM_AS_ID_NONE;
  if (req_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "Requested Packet is NULL");
    return FALSE;
  }
  
  /* Check for the length of packet contents*/
  switch(packet_type)
  {
    case NAS_DIAG_GMM_STATE_INFO_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_PTMSI_RAC_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_NMO_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_ATT_COUNTERS_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_RAU_COUNTERS_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_SMS_PRIORITY_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_EPLMN_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_T3212_F :
	  if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_req_type*)req_ptr)->as_id;
      }
      break;
    case NAS_DIAG_T3312_F :
      if (pkt_len != (uint16) sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_req_type))
      {
        ret_status = FALSE;
      }
      else
      {
	  req_as_id = ((DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_req_type*)req_ptr)->as_id;
      }
      break;

    default:
      return FALSE;

  }
  
  if(ret_status == FALSE)
  {
    MSG_ERROR_DS_2(MM_SUB, "Requested Packed Length :%d : Check Failed for %d Type ", pkt_len, packet_type);
    return FALSE;
  }
  
  if (IS_NOT_VALID_SUB_ID(req_as_id))
  {
    MSG_ERROR_DS_2( MM_SUB,"Invalid AS ID recieved, Packet as_id: %d for Diagpkt Type %d", req_as_id, packet_type );
    return FALSE;
  }
  return TRUE;
}
#endif

/*===========================================================================
Function: nasdiag_gmm_state_info
Description:

       This function processes the NAS specific GMM state request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_gmm_state_info (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_GMM_STATE_INFO_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_GMM_STATE_INFO_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_GMM_STATE_INFO_F) == TRUE)
   {
     DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_req_type *req_ptr;
     req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_GMM_STATE_INFO_F_req_type*)req_pkt;
     rsp_ptr->gmm_state_diag = (uint8)gmm_state_sim[req_ptr->as_id];
     rsp_ptr->gmm_substate_diag = (uint8)gmm_substate_sim[req_ptr->as_id];
   }
   else
#endif
   {
    rsp_ptr->gmm_state_diag = (uint8)gmm_state;
    rsp_ptr->gmm_substate_diag = (uint8)gmm_substate;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_ptmsi_rac
Description:

       This function processes the NAS specific PTMSI and RAC request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_ptmsi_rac (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_PTMSI_RAC_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_PTMSI_RAC_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_PTMSI_RAC_F) == TRUE)
   {
     DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_req_type *req_ptr;
     req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_PTMSI_RAC_F_req_type*)req_pkt;
     memscpy( (byte *)rsp_ptr->ptmsi, NAS_DIAG_PTMSI_SIZE, &gmm_stored_ptmsi_sim[req_ptr->as_id], NAS_DIAG_PTMSI_SIZE);
     rsp_ptr->routing_area_code_diag = gmm_stored_rai_sim[req_ptr->as_id].routing_area_code;
   }
   else
#endif
   {
    memscpy( (byte *)rsp_ptr->ptmsi, NAS_DIAG_PTMSI_SIZE, &gmm_stored_ptmsi, NAS_DIAG_PTMSI_SIZE);
    rsp_ptr->routing_area_code_diag = gmm_stored_rai.routing_area_code;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_nmo
Description:

       This function processes the NAS specific NMO request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_nmo (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_NMO_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_NMO_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_NMO_F) == TRUE)
   {
      DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_req_type *req_ptr;
      req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_NMO_F_req_type*)req_pkt;
      rsp_ptr->network_op_mode_diag = (uint8) mm_serving_plmn_sim[req_ptr->as_id].network_op_mode;
   }
   else
#endif
   {
    rsp_ptr->network_op_mode_diag = (uint8) mm_serving_plmn.network_op_mode;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_att_counters
Description:

       This function processes the NAS specific ATTACH counter request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_att_counters (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_ATT_COUNTERS_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_ATT_COUNTERS_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_ATT_COUNTERS_F) == TRUE)
   {
      DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_req_type *req_ptr;
      req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_ATT_COUNTERS_F_req_type*)req_pkt;
      rsp_ptr->gprs_attach_attempt_counter_diag = gprs_attach_attempt_counter_sim[req_ptr->as_id];
   }
   else
#endif
   {
    rsp_ptr->gprs_attach_attempt_counter_diag = gprs_attach_attempt_counter;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_rau_counters
Description:

       This function processes the NAS specific RAU counter request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_rau_counters (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_RAU_COUNTERS_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_RAU_COUNTERS_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_RAU_COUNTERS_F) == TRUE)
   {
     DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_req_type *req_ptr;
     req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_RAU_COUNTERS_F_req_type*)req_pkt;
     rsp_ptr->gmm_rau_attempt_counter_diag = (uint8) gmm_rau_attempt_counter_sim[req_ptr->as_id];
   }
   else
#endif
   {
    rsp_ptr->gmm_rau_attempt_counter_diag = (uint8) gmm_rau_attempt_counter;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_sms_priority
Description:

       This function processes the NAS specific SMS Priority request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_sms_priority (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_SMS_PRIORITY_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_SMS_PRIORITY_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_SMS_PRIORITY_F) == TRUE)
   {
      DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_req_type *req_ptr;
      req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_SMS_PRIORITY_F_req_type*)req_pkt;
      rsp_ptr->sms_radio_priority_diag = sms_radio_priority_sim[req_ptr->as_id];
   }
   else
#endif
   {
    rsp_ptr->sms_radio_priority_diag = sms_radio_priority;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_radio_network_msc_sgsn_info
Description:

       This function processes the NAS specific RNC MSC and SGSN request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_radio_network_msc_sgsn_info (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM

   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F) == TRUE)
   {
      DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_req_type *req_ptr;
      req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F_req_type*)req_pkt;
      rsp_ptr->mscr_diag = mm_system_information_sim[req_ptr->as_id].mscr;
      rsp_ptr->sgsnr_diag = mm_system_information_sim[req_ptr->as_id].sgsnr;
   }
   else
#endif
   {
    rsp_ptr->mscr_diag = mm_system_information.mscr;
    rsp_ptr->sgsnr_diag = mm_system_information.sgsnr;
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_eplmn
Description:

       This function processes the NAS specific EPLMN List request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_eplmn (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_rsp_type *rsp_ptr;
  
  sys_plmn_list_s_type eplmn_list;
  sys_plmn_id_s_type rplmn;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_EPLMN_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_EPLMN_F, rsp_len);
  }
  else
  {
  #ifdef FEATURE_DUAL_SIM
    if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_EPLMN_F) == TRUE)
    {
       DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_req_type *req_ptr;
       req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_EPLMN_F_req_type*)req_pkt;
       sys_eplmn_per_nas_stacks_list_get (&eplmn_list, &rplmn, (sys_modem_as_id_e_type)req_ptr->as_id);
    }
    else
  #endif
    {
      sys_eplmn_list_get (&eplmn_list, &rplmn);
    }

  
    rsp_ptr->eplmn_list_length_diag = (byte)MIN(eplmn_list.length, SYS_PLMN_LIST_MAX_LENGTH);
    memset(&rsp_ptr->eplmn_list_diag, 0xFF, NAS_DIAG_SYS_PLMN_LIST);
    memscpy ( (byte *)rsp_ptr->eplmn_list_diag, NAS_DIAG_SYS_PLMN_LIST, &rplmn, 3);
    memscpy ( (byte *)(rsp_ptr->eplmn_list_diag+3), NAS_DIAG_SYS_PLMN_LIST-3 , &eplmn_list.plmn, rsp_ptr->eplmn_list_length_diag*3);
    rsp_ptr->eplmn_list_length_diag++ ; // due to RPLMN
  }
  return rsp_ptr ;

}


/*===========================================================================
Function: nasdiag_t3212
Description:

       This function processes the NAS specific T3212 request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_t3212 (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_T3212_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_T3212_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_T3212_F) == TRUE)
   {
        DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_req_type *req_ptr;
        req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_T3212_F_req_type*)req_pkt;
#ifdef FEATURE_NAS_REL10
        if(mm_is_per_ms_t3212_present_sim[req_ptr->as_id])
        {
          rsp_ptr->T3212_timer_value_diag = mm_stored_per_ms_t3212_value_sim[req_ptr->as_id];
        }
        else
#endif /*FEATURE_NAS_REL10*/
        { 
          rsp_ptr->T3212_timer_value_diag = mm_system_information_sim[req_ptr->as_id].T3212_timer_value;
        }
   }
   else		
#endif /*FEATURE_DUAL_SIM*/
   {

#ifdef FEATURE_NAS_REL10
        if(mm_is_per_ms_t3212_present)
        {
          rsp_ptr->T3212_timer_value_diag = mm_stored_per_ms_t3212_value;
        }
        else
#endif
        { 
          rsp_ptr->T3212_timer_value_diag = mm_system_information.T3212_timer_value;
        }
   }

  }
  return rsp_ptr ;

}



/*===========================================================================
Function: nasdiag_t3312
Description:

       This function processes the NAS specific T3312 request packet
       from QXDM and sends the response back.

============================================================================*/

PACKED void *nasdiag_t3312 (PACKED void* req_pkt, uint16 pkt_len)
{

  DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_rsp_type);

  /* Allocate the memory for this */
  rsp_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_NAS, NAS_DIAG_T3312_F,
                                 rsp_len );
  if (!rsp_ptr)
  {
    MSG_FATAL_DS(MM_SUB,"diagpkt_subsys_alloc(%d, %d, %d) returned NULL!", 
              DIAG_SUBSYS_NAS, NAS_DIAG_T3312_F, rsp_len);
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
   if(nasdiag_validate_req_parameters(pkt_len, req_pkt, NAS_DIAG_T3312_F) == TRUE)
   {
     DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_req_type *req_ptr;
     req_ptr = (DIAG_SUBSYS_NAS_NAS_DIAG_T3312_F_req_type*)req_pkt;
     rsp_ptr->gmm_t3312_timer_value_diag = gmm_t3312_timer_value_sim[req_ptr->as_id];
   }
   else
#endif
   {
    rsp_ptr->gmm_t3312_timer_value_diag = gmm_t3312_timer_value;
   }

  }
  return rsp_ptr ;

}





/**************************************************************************
** DISPATCH TABLE.
****************************************************************************/
static const diagpkt_user_table_entry_type nas_diag_tbl[] =
{
  {NAS_DIAG_GMM_STATE_INFO_F, NAS_DIAG_GMM_STATE_INFO_F, nasdiag_gmm_state_info},
  {NAS_DIAG_PTMSI_RAC_F, NAS_DIAG_PTMSI_RAC_F, nasdiag_ptmsi_rac},
  {NAS_DIAG_NMO_F, NAS_DIAG_NMO_F, nasdiag_nmo},
  {NAS_DIAG_ATT_COUNTERS_F, NAS_DIAG_ATT_COUNTERS_F, nasdiag_att_counters},
  {NAS_DIAG_RAU_COUNTERS_F, NAS_DIAG_RAU_COUNTERS_F, nasdiag_rau_counters},
  {NAS_DIAG_SMS_PRIORITY_F, NAS_DIAG_SMS_PRIORITY_F, nasdiag_sms_priority},
  {NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F, NAS_DIAG_RADIO_NETWORK_INFO_MSC_SGSN_F, nasdiag_radio_network_msc_sgsn_info},
  {NAS_DIAG_EPLMN_F, NAS_DIAG_EPLMN_F, nasdiag_eplmn},
  {NAS_DIAG_T3212_F, NAS_DIAG_T3212_F, nasdiag_t3212},
  {NAS_DIAG_T3312_F, NAS_DIAG_T3312_F, nasdiag_t3312},
};



/*===========================================================================

FUNCTION NAS_DIAG_INIT

DESCRIPTION
  Register the subsystem function with diag.

============================================================================*/
void nas_diag_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER ((uint16)DIAG_SUBSYS_NAS, nas_diag_tbl);
}
#endif
