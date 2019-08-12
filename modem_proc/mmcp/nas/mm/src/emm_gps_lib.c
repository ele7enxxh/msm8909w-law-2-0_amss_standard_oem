
/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_gps_lib.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $ 
 
when       who     what
--------   ---     ----------------------------------------------------------
04/22/10   zren    Updated to use global structure for NAS outgoing message
09/25/09   hnam    Added TLB support
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#if defined (FEATURE_LTE) && defined (FEATURE_LTE_REL9)
#include "emm.h"
#include "rex.h"
#include "comdef.h"
#include "emm_database.h"
#include "msgr.h"
#include "msgr_nas.h"
#include "lte_nas.h"
#include "err.h"
#include "emm_gps_if_msg.h"
#include "emm_gps_lib.h"
#include "emm_mem.h"
#include <dsm_item.h>

/*************************************************************************** 
                   Internal functions 
****************************************************************************/
/*==========================================================================
 FUNCTION  EMM_ALLOCATE_UL_GENERIC_IEI_MEMORY

DESCRIPTION
  Allocates nas out msg memory required to pull up the dsm items

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_allocate_ul_generic_iei_memory
(
   uint8                                num_attach,
   dsm_item_type                        **dsm_arr_ptr, 
   lte_nas_emm_ul_generic_nas_transport *ul_generic_transport,
   emm_ctrl_data_type                   *emm_ctrl_data_ptr
)
{
  uint32 dsm_packet_length = 0;

  /*-------------------------------------------------------------------------*/
  /*             get the dsm packet length                                   */
  /*-------------------------------------------------------------------------*/
  dsm_packet_length  = dsm_length_packet(dsm_arr_ptr[0]);

  ul_generic_transport->generic_msg_container.container_length = (word)dsm_packet_length;
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Length of Message Container = %ld", dsm_packet_length);

  /*---------------------------------------------------------------------------
        Get the values from DSMPULLUP & send this message for encoding
  ----------------------------------------------------------------------------*/
  ul_generic_transport->generic_msg_container.container_value_ptr =                   
             (byte *)emm_modem_mem_alloc(sizeof(byte)*ul_generic_transport-> \
                                         generic_msg_container.container_length,
                                         emm_ctrl_data_ptr);
  mm_check_for_null_ptr((void*)ul_generic_transport->generic_msg_container.container_value_ptr);

  if(ul_generic_transport->generic_msg_container.container_length != dsm_pullup(&dsm_arr_ptr[0],
                                 ul_generic_transport->generic_msg_container.container_value_ptr,
                                 ul_generic_transport->generic_msg_container.container_length))
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= gps generic length not equal to dsm length, dsm_arr_ptr[0]", 0,0,0 );
  }
  mm_check_for_null_ptr((void *)&dsm_arr_ptr[0]); 
  /* if num of attaches are two, there is additional info */
  if(num_attach == 2)      
  {
    ul_generic_transport->additional_info.additional_info_length = (byte)dsm_length_packet(dsm_arr_ptr[1]); 
    MSG_HIGH_DS_1(MM_SUB, "=EMM= Length of Additional Information = %ld",
                  dsm_length_packet(dsm_arr_ptr[1]));
    ul_generic_transport->additional_info_exists = TRUE;

    ul_generic_transport->additional_info.additional_info_value_ptr =                   
             (byte *)emm_modem_mem_alloc(sizeof(byte)* ul_generic_transport-> \
                                         additional_info.additional_info_length ,
                                         emm_ctrl_data_ptr);
    mm_check_for_null_ptr((void*)ul_generic_transport->additional_info.additional_info_value_ptr );


    if(ul_generic_transport->additional_info.additional_info_length != dsm_pullup(&dsm_arr_ptr[1],
                                 ul_generic_transport->additional_info.additional_info_value_ptr,
                                 ul_generic_transport->additional_info.additional_info_length))
    {
     MSG_FATAL_DS(MM_SUB, "=EMM= gps generic length not equal to dsm length, dsm_arr_ptr[1]", 0,0,0 );     
    }
	
    mm_check_for_null_ptr((void *)&dsm_arr_ptr[1]);
  }

}


/*==========================================================================
 FUNCTION  EMM_GENERIC_NAS_DSM_FREE

DESCRIPTION
  This function would free the dsm items of the UL generic nas MSG Req

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_generic_nas_dsm_free
(
  uint8              num_dsm_ptr,
  dsm_item_type      **dsm_arr_ptr  
)
{
  uint8 i = 0;

  for(i = 0; i < num_dsm_ptr && 
             i < EMM_GENERIC_NAS_TRANSPORT_MAX_DSM_ARRAY_SIZE; i++)
  {
    /* free the dsm item */  
    dsm_free_packet(&dsm_arr_ptr[i]);   

    /* Received dsm should have been freed */
    ASSERT(dsm_arr_ptr[i] == NULL);

  }
}

/*===========================================================================
FUNCTION    EMM_SEND_GENERIC_NAS_TRANSPORT_FAILURE_IND

DESCRIPTION
  This function sends FAILURE_IND with error cause to GPS module

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_generic_nas_failure_ind
(
  emm_irat_failure_cause_type  cause_type,
  byte                         cause,
  dword                        trans_id,
  uint64                       access_barring_rem_time

)
{
   emm_irat_failure_ind_type  failure_ind;

   MSG_HIGH_DS_3(MM_SUB,"=EMM= Sending GENERIC_NAS_TRANSPORT_FAILURE_IND: Cause: %d, %d, Trans ID: 0x%x",
                 cause_type, cause, trans_id);
 
   /* Init */
   memset(&failure_ind, 0x0, sizeof(emm_irat_failure_ind_type));

   msgr_init_hdr((msgr_hdr_s *)&failure_ind.msg_hdr, 
                 MSGR_NAS_EMM, 
                 NAS_EMM_GENERIC_NAS_TRANSPORT_FAILURE_IND);
  
   /* Populate the ind */
   failure_ind.trans_id = trans_id;
   failure_ind.cause_type = cause_type;

   switch(cause_type)
  {
     case LTE_NAS_IRAT_CONN_EST_FAILURE:
       failure_ind.cause.est_status = cause;
       failure_ind.access_barring_rem_time = access_barring_rem_time;
       break;
     case LTE_NAS_IRAT_CONN_REL:
       failure_ind.cause.rel_reason = cause;
       break;
     case LTE_NAS_IRAT_UL_MSG_CNF_FAILURE:
       failure_ind.cause.cnf_status = cause;
       break;
     case LTE_NAS_IRAT_SR_REJECT:
       failure_ind.cause.emm_cause = cause;
       break;
     case LTE_NAS_IRAT_ACCESS_BARRING:
       failure_ind.access_barring_rem_time = access_barring_rem_time;
       break;
     default:
       break;
  }

  if(emm_msgr_send((msgr_hdr_s *)&failure_ind, sizeof(failure_ind)) != E_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= emm_msgr_send failed!", 0,0,0 );
  }
}

/*===========================================================================
FUNCTION    EMM_UL_GENERIC_NAS_TRANSPORT_FAILED

DESCRIPTION
  This function sends UL_GENERIC_TRANSPORT_TRANSMISSION_FAILURE to GPS module

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
boolean emm_ul_generic_nas_transport_failed
(
  lte_emm_connection_cause_type  cause,
  dword                          trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type          tac,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type             *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;

  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  pended_message_ptr->message_sent_indicator = TRUE ;

  /* for UL_DATA_CNF failure callback is of different type
     map it to the gps module failure_ind */
  emm_send_generic_nas_failure_ind(emm_failure_cause.cause_type,
                                   (byte)emm_failure_cause.cause.cnf_status,
                                   trans_id,
                                   emm_failure_cause.access_barring_rem_time);

  return FALSE;
}

/*===========================================================================
FUNCTION    EMM_UL_GENERIC_NAS_TRANSPORT_SUCESS

DESCRIPTION
  This function sends UL_GENERIC_TRANSPORT_TRANSMISSION_SUCESS to GPS module

DEPENDENCIES
  None

RETURN VALUE
 NONE

SIDE EFFECTS
  None
===========================================================================*/
void emm_ul_generic_nas_transport_sucess
(
  dword                          trans_id
)
{
   lte_nas_emm_ul_generic_transport_cnf_type ul_generic_nas_cnf;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= UL_GENERIC_NAS_TRANSPORT succeded");



   /* Init */
   memset(&ul_generic_nas_cnf, 0x0, sizeof(lte_nas_emm_ul_generic_transport_cnf_type));

   msgr_init_hdr((msgr_hdr_s *)&ul_generic_nas_cnf.msg_hdr, 
                  MSGR_NAS_EMM, 
                  NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF);

   ul_generic_nas_cnf.trans_id = trans_id;

   if(emm_msgr_send((msgr_hdr_s *)&ul_generic_nas_cnf, 
                 sizeof(lte_nas_emm_ul_generic_transport_cnf_type)) != E_SUCCESS)
  {
     MSG_FATAL_DS(MM_SUB, "=EMM= Failed to send NAS_EMM_UL_GENERIC_NAS_TRANSPORT_CNF", 0,0,0);
  }
}

#endif /*FEATURE_LTE && FEATURE_LTE_REL9 */
