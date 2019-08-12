/*!
  @file
  mem_share_qmi_client.c

  @brief
  Implementation of the QMI client used to request and free memory from the server on HLOS.

*/

/*===========================================================================

  Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/mem_share_qmi_client.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/29/14   abh     Replaced qmi_client_init() with qmi_client_init_instance().
11/30/12   ps      Initial Version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "stdio.h"
#include "stdlib.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "dynamic_heap_memory_sharing_v01.h"
#include "mem_share_client.h"



/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

extern qmi_client_type               mem_client_handle;
extern mem_share_client_info_type mem_share_client_info[DYNAMIC_MEM_CLIENT_ID_MAX];





/*===========================================================================

  FUNCTION:  mem_share_client_init

===========================================================================*/
/*!
    @brief
    Initialize the QMI client for dynamic memory allocation requests

    @detail
    This function will initialize the mem_share client to talk to the service on HLOS that will 
   allocate the memory on HLOS and pass the handle back to the client.

    @return
    MEM_SHARE_SUCCESS : Initialization was succesful
    MEM_SHARE_FAILURE: Initialization failed

*/
/*=========================================================================*/
mem_share_status_t mem_share_client_init(void)
{
 
   static boolean mem_share_client_initialized = FALSE;
   mem_share_status_t ret_status;
   if (mem_share_client_initialized == TRUE)
    {
	  return MEM_SHARE_SUCCESS;
	}   
   
   ret_status = mem_share_get_qmi_poll_status();

   if (ret_status == MEM_SHARE_SUCCESS )
   {
	   mem_share_client_initialized = TRUE;
   }
   return ret_status;

   
}


/*===========================================================================

  FUNCTION:  mem_share_request_pool

===========================================================================*/
/*!
    @brief
    Send a request to the server to allocate a memory block to be used on modem

    @detail
    This function is called when modem requires additional memory to be allocated in the HLOS.
    This function requires the size and the block alignment required for the requested memory.
    The handle and number of bytes actually allocated are returned in the pointer *handle and *num_bytes_ret

    @return
    MEM_SHARE_SUCCESS : request for memory was succesful
    MEM_SHARE_FAILURE: request for memory failed

*/
/*=========================================================================*/
mem_share_status_t mem_share_request_pool(
															dynamic_mem_client_id client_id, 
															dynamic_mem_attribute attr,
                                             uint32_t num_bytes, 
                                             uint32_t block_alignment,
                                             mem_share_alloc_rsp_type ** mem_share_alloc_rsp)
{
   int ret = 0;
   dhms_mem_alloc_generic_req_msg_v01 mem_alloc_req;
   dhms_mem_alloc_generic_resp_msg_v01 mem_alloc_resp;
   mem_share_status_t  return_stat = MEM_SHARE_SUCCESS;
   int i=0;

	
	mem_alloc_req.alloc_contiguous_valid = 1;
	if(attr == DYNAMIC_MEM_ATTRIBUTE_CMA)
	{
		mem_alloc_req.alloc_contiguous=1;
	}
	else
	{
		mem_alloc_req.alloc_contiguous=0;
	}
	mem_alloc_req.client_id=(dhms_mem_client_id_v01)client_id;
	mem_alloc_req.proc_id= DHMS_MEM_PROC_MPSS_V01;
	mem_alloc_req.sequence_id=mem_share_client_info[client_id].sequence_id++;
   mem_alloc_req.num_bytes = num_bytes;
   mem_alloc_req.block_alignment = block_alignment;
   mem_alloc_req.block_alignment_valid = 0;

   MSG_HIGH("requesting for memory from server\n",0,0,0);

   /* QMI call to APPS to get memmory */
   ret = qmi_client_send_msg_sync(mem_client_handle,QMI_DHMS_MEM_ALLOC_GENERIC_REQ_V01,
                                            (void*)&mem_alloc_req,sizeof(mem_alloc_req),
                                            (void*)&mem_alloc_resp,sizeof(mem_alloc_resp),0 );
   if(QMI_NO_ERR != ret)
   {
      MSG_ERROR("requesting for memory from server failed, ret = %d\n",ret,0,0);
      return_stat = MEM_SHARE_IPC_FAILURE;
      return return_stat;
   }

   if(mem_alloc_resp.resp.result != QMI_NO_ERR)
   {
      MSG_ERROR("requesting for memory from server failed, result:errorcode = %d:%d\n",mem_alloc_resp.resp.result,mem_alloc_resp.resp.error,0);
      return_stat = MEM_SHARE_IPC_FAILURE;      
      return return_stat;
   }

   MSG_HIGH("requesting for memory from server success",0,0,0);

	// Should we reverify all the information client id, procid
	if(mem_alloc_resp.dhms_mem_alloc_addr_info_valid == 1)
	{
		*mem_share_alloc_rsp=(mem_share_alloc_rsp_type *)malloc(sizeof(mem_share_alloc_rsp_type)* mem_alloc_resp.dhms_mem_alloc_addr_info_len);
        if(*mem_share_alloc_rsp == NULL)
        {
        
           MSG_ERROR("Memory allocation failure for response\n",0,0,0);
           return_stat = MEM_SHARE_FAILURE;      
           return return_stat;
        }
		for(i =0; i <mem_alloc_resp.dhms_mem_alloc_addr_info_len;i++)
		{
			(*mem_share_alloc_rsp)[i].phy_addr=mem_alloc_resp.dhms_mem_alloc_addr_info[i].phy_addr;
			(*mem_share_alloc_rsp)[i].phy_addr_length=mem_alloc_resp.dhms_mem_alloc_addr_info[i].num_bytes;
		}
	}
  return return_stat;
}


/*===========================================================================

  FUNCTION:  mem_share_release_pool

===========================================================================*/
/*!
    @brief
    Send a request to the server to free a memory block that was allocated earlier by HLOS

    @detail
    This function is called when modem is done using the memory allocated earlier by HLOS.
    This function requires the handle to the memory block that was passed by HLOS.

    @return
    MEM_SHARE_SUCCESS : request for memory was succesful
    MEM_SHARE_FAILURE: request for memory failed

*/
/*=========================================================================*/
mem_share_status_t mem_share_release_pool(dynamic_mem_client_id client_id,uint32 handle,
                                              uint32_t num_bytes)
{
  dhms_mem_free_generic_req_msg_v01 mem_free_req;
  dhms_mem_free_generic_resp_msg_v01 mem_free_resp;

  if(0 == handle || 0 == num_bytes)
  {
     MSG_ERROR("mem_share_release_pool,invalid arguments handle=%d, num_bytes=%d",handle,num_bytes,0);
     return MEM_SHARE_FAILURE;
  }
  mem_free_req.client_id_valid=1;
  mem_free_req.client_id=(dhms_mem_client_id_v01)client_id;
  mem_free_req.proc_id_valid=1;
  mem_free_req.proc_id= DHMS_MEM_PROC_MPSS_V01;
  mem_free_req.dhms_mem_alloc_addr_info_len=1;
  mem_free_req.dhms_mem_alloc_addr_info[0].phy_addr = (uint64)handle;
  mem_free_req.dhms_mem_alloc_addr_info[0].num_bytes=num_bytes;

  MSG_HIGH("releasing memory to server ",0,0,0);
  if(QMI_NO_ERR != qmi_client_send_msg_sync(mem_client_handle,QMI_DHMS_MEM_FREE_GENERIC_REQ_V01,
                                            (void*)&mem_free_req,sizeof(mem_free_req),
                                            (void*)&mem_free_resp,sizeof(mem_free_resp),0 ))
  {
    MSG_ERROR("qmi_client_send_msg_sync returned failure",0,0,0);  
    return MEM_SHARE_FAILURE;
  }
  MSG_HIGH("releasing memory to server success",0,0,0);
  return MEM_SHARE_SUCCESS;
}


/*===========================================================================

  FUNCTION:  mem_share_query_size

===========================================================================*/
/*!
    @brief
    Send a request to the server to query the max size which can be allocated for
    a particular client.
  

    @return
    MEM_SHARE_SUCCESS : request for memory was succesful
    MEM_SHARE_FAILURE: request for memory failed

*/
/*=========================================================================*/

mem_share_status_t mem_share_query_size(dynamic_mem_client_id client_id,
                                                 uint32_t *size)
{
  dhms_mem_query_size_req_msg_v01 mem_query_size_req;
  dhms_mem_query_size_resp_msg_v01 mem_query_size_resp;


  mem_query_size_req.client_id=(dhms_mem_client_id_v01)client_id;
  
  mem_query_size_req.proc_id_valid=1;
  mem_query_size_req.proc_id= DHMS_MEM_PROC_MPSS_V01;
  
  MSG_HIGH("querying size to server ",0,0,0);
  if(QMI_NO_ERR != qmi_client_send_msg_sync(mem_client_handle,QMI_DHMS_MEM_QUERY_SIZE_REQ_V01,
                                            (void*)&mem_query_size_req,sizeof(mem_query_size_req),
                                            (void*)&mem_query_size_resp,sizeof(mem_query_size_resp),0 ))
  {
    MSG_ERROR("qmi_client_send_msg_sync returned failure result:errorcode = %d:%d\n",mem_query_size_resp.resp.result,mem_query_size_resp.resp.error,0);  
    return MEM_SHARE_FAILURE;
  }
  MSG_HIGH("querying size to server success",0,0,0);
  if(mem_query_size_resp.size_valid == 1)
  {
	  *size=mem_query_size_resp.size;
  }

  MSG_HIGH("querying size to server success size returned %x",*size,0,0);
  return MEM_SHARE_SUCCESS;
}

