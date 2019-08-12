/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_common_mm_primitives.c_v   1.2   14 Aug 2001 15:15:44   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_common_mm_primitives.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

09/18/02   AB      Added CNM_send_mmcnm_rel_req() to sent MMCNM_REL_REQ to 
                   MM.

05/14/05   HS      Added support for FEATURE_CCBS
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "ms.h"
#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"
#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"


/**************** Begin of   CNM_send_primitive_to_mm *****************************
 *
 *  Function name: CNM_send_primitive_to_mm()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  This function fills in the details of the message to be sent to MM
 * and then sends it.
 *
 *
 *  Parameters:
 *  -----------
 *  connection_id : INPUT
 *  message : INPUT
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_send_primitive_to_mm( connection_id_T connection_id,
                                    byte protocol_desc,
                                   IMH_T * message ) 
{
   ((msg_head_T *)((void *)message))->IMH.message_set = MS_CC_MM;
   ((msg_head_T *)((void *)message))->IMH.message_id  = MMCNM_DATA_REQ;
   ((msg_head_T *)((void *)message))->ti_pd = (unsigned char)(FORM_TI_PD(connection_id, protocol_desc));

   CNM_manage_cnm_data_queue( (mmcnm_data_req_T *)((void *)message) );
}
/**************** End of   CNM_send_primitive_to_mm ******************************/

/**************** Begin of   CNM_send_mmcnm_rel_req() ************************************
 *
 *  Function name: CNM_send_mm_cnm_rel_req()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *    Send the MMCNM_REL_REQ to request MM to release the
 *    TCH radio channel.
 * 
 *  This function 
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
VOID_FUNC CNM_send_mmcnm_rel_req(IMH_T *message)
{
    mmcnm_rel_req_T    mmcnm_rel_req, *mmcnm_rel_req_ptr;

    mmcnm_rel_req_ptr = (mmcnm_rel_req_T *)((void *)message);

    mmcnm_rel_req.header.message_set = MS_CC_MM;
    mmcnm_rel_req.header.message_id  = MMCNM_REL_REQ;
    mmcnm_rel_req.rel_type = mmcnm_rel_req_ptr->rel_type;
#ifdef FEATURE_DUAL_SIM 
    if(MNCNM_IS_SINGLE_STACK)
    {
      mmcnm_rel_req.as_id = mmcnm_rel_req_ptr->as_id;
      PUT_IMH_LEN(sizeof(mmcnm_rel_req_T) - sizeof(IMH_T),
                         &mmcnm_rel_req.header);       
    }
#ifdef FEATURE_DUAL_ACTIVE 
    else
    {
      mmcnm_rel_req.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      PUT_IMH_LEN(sizeof(mmcnm_rel_req_T) - sizeof(IMH_T),
                  &mmcnm_rel_req.header);
    }
#endif
#else
    {
       PUT_IMH_LEN(sizeof(mmcnm_rel_req.rel_type), &mmcnm_rel_req.header);
    }
#endif     
    CNM_send_message((IMH_T *)&mmcnm_rel_req, GS_QUEUE_MM);
}

/**************** End of   CNM_send_mmcnm_rel_req()  ***************************/

#ifdef FEATURE_CCBS
/**************** Begin of   CNM_send_mmcnm_prompt_res() ************************************
 *
 *  Function name: CNM_send_mmcnm_prompt_res()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *    Send the MMCNM_PROMPT_RES to request MM
 * 
 *  This function 
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/
VOID_FUNC CNM_send_mmcnm_prompt_res( boolean support, 
                                     boolean accept 
#if defined(FEATURE_DUAL_SIM )
                     ,sys_modem_as_id_e_type  as_id
#endif
                                     )
{
   mmcnm_prompt_res_T  prim;  /* primitive */

   prim.header.message_set = MS_CC_MM;
   prim.header.message_id  = MMCNM_PROMPT_RES;
   prim.supported          = support;
   prim.accept             = accept;
#if defined(FEATURE_DUAL_SIM)
   if(MNCNM_IS_SINGLE_STACK)
   {
     prim.as_id = as_id;
     PUT_IMH_LEN(sizeof(mmcnm_prompt_res_T) - sizeof(IMH_T),
                  &prim.header);      
   }
#ifdef FEATURE_DUAL_ACTIVE
   else
   {
     prim.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
     PUT_IMH_LEN(sizeof(mmcnm_prompt_res_T) - sizeof(IMH_T),
                 &prim.header);
   }
#endif
#else
   {
     PUT_IMH_LEN(sizeof(prim.supported)+sizeof(prim.accept), &prim.header);
   }
#endif

   CNM_send_message((IMH_T *)&prim, GS_QUEUE_MM);
}
#endif /* FEATURE_CCBS */


/**************** Begin of   CNM_fill_mmcnm_data_req() ************************************
 *
 *  Function name: CNM_fill_mmcnm_data_req()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *    Fill as_id in MMCNM_DATA_REQ
 * 
 *  This function 
 *  Parameters:
 *  -----------
 *
 *  Returns:
 *  --------
 *
 ******************************************************************************/

VOID_FUNC CNM_fill_mmcnm_data_req(mmcnm_data_req_T* mmcnm_data_req)
{
  unsigned int     length;
#ifdef FEATURE_DUAL_SIM  
  connection_id_T   connection_id;
  connection_id = (unsigned char)(GET_TI(mmcnm_data_req->data[0]));
#endif  
  GET_IMH_LEN(length, &mmcnm_data_req->header);


  /* Append CNM resel decision status to MM in DATA REQ */  

  if(mmcnm_data_req->data[1] == CALL_CONFIRMED)
  {
    mmcnm_data_req->data[length] = TRUE;
  }
  ++length;
  PUT_IMH_LEN(length,&mmcnm_data_req->header);

#ifdef FEATURE_DUAL_SIM

  if(MNCNM_IS_SINGLE_STACK)
  {
     if(GET_PD(mmcnm_data_req->data[0]) == PD_SS)
     {
       mmcnm_data_req->data[length] = (byte)ss_transaction_information[connection_id].as_id;
       PUT_IMH_LEN((length + sizeof(byte)),&mmcnm_data_req->header);
     }
     else if(GET_PD(mmcnm_data_req->data[0]) == PD_SMS)
     {
       mmcnm_data_req->data[length] = (byte)sm_rl_transaction_information[connection_id].as_id;
       PUT_IMH_LEN((length + sizeof(byte)),&mmcnm_data_req->header);
     }
     else if(GET_PD(mmcnm_data_req->data[0]) == PD_CC)
     {
       if(mn_call_information_p[connection_id]!= NULL 
            && mn_call_information_p[connection_id]->as_id != SYS_MODEM_AS_ID_NONE)
       {
         mmcnm_data_req->data[length] = (byte)mn_call_information_p[connection_id]->as_id;
         PUT_IMH_LEN((length + sizeof(byte)),&mmcnm_data_req->header);
       }  
       else if (mn_as_id != SYS_MODEM_AS_ID_NONE)
       {
         mmcnm_data_req->data[length] = (byte)mn_as_id;
         PUT_IMH_LEN((length + sizeof(byte)),&mmcnm_data_req->header);    
       }
       else
       {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= mn_as_id is NULL\n");
       }
     }
     else
     {
       MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid PD = %d\n",(dword) (GET_PD(mmcnm_data_req->data[0])));
     }
  }
#ifdef FEATURE_DUAL_ACTIVE
  else
  {
     mmcnm_data_req->data[length] = mn_dsda_as_id;
     PUT_IMH_LEN((length + sizeof(byte)),&mmcnm_data_req->header);
  }
#endif
#endif  
}


#ifdef FEATURE_DUAL_SIM 

VOID_FUNC CNM_fill_mmcnm_est_req(mmcnm_est_req_T* mmcnm_est_req)
{
  if(MNCNM_IS_SINGLE_STACK)
  {
     if(mmcnm_est_req->CNM_service_type == SUPPLEMENTARY_SERVICE_ACTIVATION)
     {
       mmcnm_est_req->as_id = ss_transaction_information[mm_connection_pending.connection_id].as_id;
     }
     else if(mmcnm_est_req->CNM_service_type == SHORT_MESSAGE_SERVICE_ESTABLISHMENT)
     {
       mmcnm_est_req->as_id = sm_rl_transaction_information[mm_connection_pending.connection_id].as_id;
     }
     else // CC
     {
   
       if(mn_call_information_p[mm_connection_pending.connection_id]!= NULL 
            && mn_call_information_p[mm_connection_pending.connection_id]->as_id != SYS_MODEM_AS_ID_NONE)
       {
         mmcnm_est_req->as_id = mn_call_information_p[mm_connection_pending.connection_id]->as_id;
       }
       else if (mn_as_id != SYS_MODEM_AS_ID_NONE)
       {
         mmcnm_est_req->as_id = (byte)mn_as_id;
       }
       else
       {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= mn_as_id is NULL");  
       }
     }
  }
#ifdef FEATURE_DUAL_ACTIVE
  else
  {
     mmcnm_est_req->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
  }
#endif
}
#endif

