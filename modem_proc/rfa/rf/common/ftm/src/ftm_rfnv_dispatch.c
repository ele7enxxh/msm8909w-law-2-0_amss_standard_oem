/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM RFNV Dispatch

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2010 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_rfnv_dispatch.c#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/15/14    cd     Fix compiler warnings
09/04/14   cd      Free memory before early return from rfnv_read 
05/07/14   sar     Fixed KW errors. 
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
04/16/13   ems     Ensure file handles are closed for error conditions
04/09/13   ems     Added support for NV items larger than 4K 
03/05/13   aki     Memset rfnv buffer to 0's before EFS read   
08/30/11   aki     Pass in rfnv buffer size to rfcommon_rfnv_get_item 
07/28/10   aki     Dynamic allocation for rfnv_item, rfnv_id added to write resp
07/22/10   aki     Added fields cmd_data_len and cmd_rsp_pkt_size to rfnv header 
07/12/10   aki     Included 'task.h' 
07/01/10   aki     Merging RFNV FTM changes from dev/rfnv_dev 
05/05/10   aki     Initial version.
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "customer.h"

#include "stringl.h" /* for memscpy */
#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_rf_cmd.h"
#include "ftm_rfnv_dispatch.h"
#include "rfcommon_nv.h"
#include "rf_test_task_v.h"
#include "task.h"
#include "rfcommon_core_error.h"
#include "fs_lib.h"
#include "fs_public.h"

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/



/*===========================================================================

FUNCTION ftm_rfnv_fill_common_header

DESCRIPTION
   Copies common items from request to reponse packed.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_rfnv_fill_common_header(ftm_rfnv_common_header_type* req, ftm_rfnv_common_header_type* resp, uint2 rsp_len)
{
  resp->cmd = req->cmd;
  resp->sub_sys = req->sub_sys;
  resp->mode = req->mode;
  resp->ftm_cmd = req->ftm_cmd;
  resp->cmd_data_len = 0;
  resp->cmd_rsp_pkt_size = rsp_len;
}

/*===========================================================================

FUNCTION ftm_rfnv_read

DESCRIPTION
   - Reads RFNV item from NV storage. Reads items from EFS if item ID is
     greater than RFNV_USE_EFS_NV_CUTOFF.
   - Prepares response package.
   - If RFNV read was successfull copies NV item to reponse package.

DEPENDENCIES
   Ftm task running for IDs less than RFNV_USE_EFS_NV_CUTOFF.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rfnv_read(ftm_rfnv_read_request_type *req)
{

  ftm_rsp_pkt_type rsp = {FTM_RSP_BAD_CMD,0,NULL,FALSE};
  ftm_rfnv_read_response_type *rfnv_rsp = NULL;
  rfnv_item_type *rfnv_item = NULL;
  nv_stat_enum_type nv_read_status = NV_FAIL_S;
  rfnv_item_size_type rfnv_item_size = sizeof(rfnv_item_type);  
  
  rfnv_item = ftm_malloc(sizeof(rfnv_item_type));

  RF_ASSERT(NULL != rfnv_item);

  if ((NULL != rfnv_item))
  {
     memset(rfnv_item, 0, sizeof(rfnv_item_type));

     nv_read_status = rfcommon_rfnv_get_item((rfnv_item_id_enum_type)(req->rfnv_item_code), rfnv_item, &rfnv_item_size, rex_self(), FS_OP_COMPLETE_SIG, ftm_sig_rex_wait_cb);
    
     rsp = ftmdiag_create_new_pkt(sizeof(ftm_rfnv_read_response_header_type)+rfnv_item_size);
  }
  
  if(NULL == rsp.pkt_payload)
  {
    // Exit if memory allocation for reponse data failed
    ftm_free(rfnv_item);
    return rsp;
  }

  rfnv_rsp = rsp.pkt_payload;
  
  ftm_rfnv_fill_common_header(&req->header.cmn_header, &rfnv_rsp->header.cmn_header, rsp.pkt_len);

  rfnv_rsp->header.rfnv_item_code = req->rfnv_item_code;
        
  if (NV_DONE_S != nv_read_status)
  {
    rfnv_rsp->header.err_code = ( (NV_NOTACTIVE_S == nv_read_status) ?  RFCOMMON_NV_INACTIVE_ITEM : RFCOMMON_NV_READ_FAILED);
    rfnv_rsp->header.rfnv_item_size = 0; 
  }
  else
  {
    rfnv_rsp->header.err_code = RFCOMMON_NV_READ_SUCCESS;
    rfnv_rsp->header.rfnv_item_size = rfnv_item_size;
    memscpy(rfnv_rsp->rfnv_item_data, rfnv_item_size, rfnv_item, rfnv_item_size);
  }
  ftm_free(rfnv_item);
  return rsp;
};

/*===========================================================================

FUNCTION ftm_rfnv_read_v2

DESCRIPTION 
  
   - Reads RFNV item from NV storage. Reads items from EFS if item ID is
     greater than RFNV_USE_EFS_NV_CUTOFF.
     If the amount of data to be transfered is greater than RFNV_MAX_TRANSFER_SIZE
     this API must be called multiple times until bytes remainming
     in the transfer sequence is zero.
     Example: Transer 4000 bytes.  Max diag transfer size is 3800
      REQ -> rfnv_offset = 0
      RSP <- rfnv_bytes_remaining = 200 rfnv_item_size = 3800
      REQ -> rfnv_offset = 3800
      RSP <-rfnv_bytes_remaining=0 rfnv_item_size = 200
 
   - Prepares response package.
   - If RFNV read was successfull copies NV item to reponse package.

DEPENDENCIES
   Ftm task running for IDs less than RFNV_USE_EFS_NV_CUTOFF.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rfnv_read_v2(ftm_rfnv_read_request_v2_type *req)
{

  ftm_rsp_pkt_type rsp;
  ftm_rfnv_read_response_v2_type *rfnv_rsp = NULL;
  rfnv_item_type  *file_read_buf           = NULL;
  int   file_read_buf_size   =  0;
  char   path[RFNV_MAX_PATH] = "";
  int    fs_status;
  struct fs_stat f_stat;
  int    file_handle       = -1;
  int    offset            = -1;
  int    bytes_read        =  0;

  // For NV items below RFNV_USE_EFS_NV_CUTOFF
  nv_stat_enum_type nv_read_status   = NV_FAIL_S;
  rfnv_item_size_type rfnv_item_size = sizeof(rfnv_item_type); 

  // Include bytes remaining
  rsp = ftmdiag_create_new_pkt(sizeof(ftm_rfnv_read_response_header_type)+RFNV_MAX_TRANSFER_SIZE+sizeof(unsigned long));
  if(NULL == rsp.pkt_payload)
  {
    // Exit if memory allocation for reponse data failed
    FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d memory allocation for reponse data failed" ,req->rfnv_item_code, 0, 0);
    return rsp;
  }
  rfnv_rsp = rsp.pkt_payload;

  ftm_rfnv_fill_common_header(&req->header.cmn_header, &rfnv_rsp->header.cmn_header, rsp.pkt_len);

  if( req->rfnv_item_code < RFNV_USE_EFS_NV_CUTOFF )
  {
    file_read_buf      = ftm_malloc(rfnv_item_size);

    RF_ASSERT(NULL != file_read_buf);

    if (NULL == file_read_buf)
    {
       rfnv_rsp->header.err_code = RFCOMMON_NV_READ_FAILED;
       FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d Memory alloc failed",req->rfnv_item_code, 0, 0);
       return rsp;
    }

    memset(file_read_buf, 0, rfnv_item_size);

    nv_read_status = rfcommon_rfnv_get_item((rfnv_item_id_enum_type)(req->rfnv_item_code), file_read_buf, &rfnv_item_size, rex_self(), FS_OP_COMPLETE_SIG, ftm_sig_rex_wait_cb);
           
    if (NV_DONE_S != nv_read_status)
    {
      rfnv_rsp->header.err_code = ( (NV_NOTACTIVE_S == nv_read_status) ?  RFCOMMON_NV_INACTIVE_ITEM : RFCOMMON_NV_READ_FAILED);
      file_read_buf_size = 0; 
    }
    else
    {
      file_read_buf_size        = rfnv_item_size;
      rfnv_rsp->header.err_code = RFCOMMON_NV_READ_SUCCESS;
    }

    rfnv_rsp->rfnv_bytes_remaining  = 0;
  }
  else
  {
    (void) snprintf(path, RFNV_MAX_PATH, "%s%08d", RFNV_FILE_PATH, req->rfnv_item_code); 
    path[RFNV_MAX_PATH-1] = 0; 

    fs_status = efs_stat (path, &f_stat);
    if (fs_status == -1) 
    {
      rfnv_rsp->header.err_code = RFCOMMON_NV_FILE_NOT_FOUND;
      FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d File Not Found",req->rfnv_item_code, 0, 0);
      return rsp;
    }

    if ( req->rfnv_offset > f_stat.st_size)
    {
      rfnv_rsp->header.rfnv_item_size = 0;
      rfnv_rsp->header.err_code       = RFCOMMON_NV_BAD_OFFSET;
      return rsp;
    }

    file_handle = efs_open(path,O_RDONLY,0644);
    if (file_handle < 0)
    {
       rfnv_rsp->header.rfnv_item_size = 0;
       rfnv_rsp->header.err_code       = RFCOMMON_NV_FILE_OPEN_FAIL;
       FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d Open Fail",req->rfnv_item_code, 0, 0);
       return rsp;
    }

    offset = efs_lseek(file_handle,req->rfnv_offset,SEEK_SET);
    if (offset < 0)
    {
       rfnv_rsp->header.rfnv_item_size = 0;
       rfnv_rsp->header.err_code       = RFCOMMON_NV_FILE_SEEK_FAIL;
       FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d Seek Fail",req->rfnv_item_code, 0, 0);
       efs_close(file_handle);
       return rsp;
    }

    if ((f_stat.st_size - offset) > RFNV_MAX_TRANSFER_SIZE) 
    {
       file_read_buf_size = RFNV_MAX_TRANSFER_SIZE;
    }
    else
    {
       file_read_buf_size = f_stat.st_size - req->rfnv_offset;
    }

    file_read_buf = ftm_malloc(file_read_buf_size);

    RF_ASSERT(NULL != file_read_buf);

    if (NULL == file_read_buf)
    {
       rfnv_rsp->header.err_code = RFCOMMON_NV_READ_FAILED;
       FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d Memory alloc failed",req->rfnv_item_code, 0, 0);
       return rsp;
    }

    memset(file_read_buf, 0, file_read_buf_size);

    bytes_read = efs_read(file_handle,file_read_buf,file_read_buf_size);
    if (bytes_read != file_read_buf_size)
    {
       rfnv_rsp->header.rfnv_item_size = 0;
       rfnv_rsp->header.err_code       = RFCOMMON_NV_READ_FAILED;
       FTM_MSG_HIGH("ftm_rfnv_read_v2 Item ID=%d Read Fail",req->rfnv_item_code, 0, 0);
       /* Free memory before returning with error */
       ftm_free(file_read_buf);
       efs_close(file_handle);
       return rsp;
    }

    rfnv_rsp->rfnv_bytes_remaining  = f_stat.st_size - (req->rfnv_offset + file_read_buf_size);
    rfnv_rsp->header.err_code       = RFCOMMON_NV_READ_SUCCESS;

  }//if( req->header.rfnv_item_code < RFNV_USE_EFS_NV_CUTOFF )

  rfnv_rsp->header.rfnv_item_code = req->rfnv_item_code;
  rfnv_rsp->header.rfnv_item_size = file_read_buf_size;
  memscpy(rfnv_rsp->rfnv_item_data, file_read_buf_size, file_read_buf, file_read_buf_size);

  ftm_free(file_read_buf);

  efs_close(file_handle);

  FTM_MSG_MED("ftm_rfnv_read_v2 Item ID=%d Error Code =%d",req->rfnv_item_code, rfnv_rsp->header.err_code, 0);

  return rsp;
};

/*===========================================================================

FUNCTION ftm_rfnv_write

DESCRIPTION 
   - Prepares response package. 
   - Writes given RFNV item to NV storage. EFS is used for items with IDs
     greater than RFNV_USE_EFS_NV_CUTOFF.
   - Set the write status to reposnse package.
 

DEPENDENCIES
   Ftm task running for IDs less than RFNV_USE_EFS_NV_CUTOFF.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rfnv_write(ftm_rfnv_write_request_type *req)
{
  ftm_rsp_pkt_type rsp;
  ftm_rfnv_write_response_type *rfnv_rsp;
  
  rsp = ftmdiag_create_new_pkt(sizeof(ftm_rfnv_write_response_type));
  
  if(NULL == rsp.pkt_payload)
  {
    // Exit if memory allocation for reponse data failed
    return rsp;
  }

  rfnv_rsp = rsp.pkt_payload;
  
  ftm_rfnv_fill_common_header(&req->header.cmn_header, &rfnv_rsp->header.cmn_header, rsp.pkt_len);
  
  rfnv_rsp->rfnv_item_code = req->header.rfnv_item_code;  

  if (NV_DONE_S != rfcommon_rfnv_set_item((rfnv_item_id_enum_type)(req->header.rfnv_item_code), (rfnv_item_type *)req->rfnv_item_data, req->header.rfnv_item_size, rex_self(), FS_OP_COMPLETE_SIG, ftm_sig_rex_wait_cb))
  {
    rfnv_rsp->err_code = RFCOMMON_NV_WRITE_FAILED;
  }
  else
  {
    rfnv_rsp->err_code = RFCOMMON_NV_WRITE_SUCCESS;
  }
  return rsp;
}


/*===========================================================================

FUNCTION ftm_rfnv_write_v2

DESCRIPTION 
   - Prepares response package. 
   - Writes given RFNV item to NV storage. EFS is used for items with IDs
     greater than RFNV_USE_EFS_NV_CUTOFF.
   - Set the write status to reposnse package
 
    If the amount of data to be transfered is greater than RFNV_MAX_TRANSFER_SIZE
    this API must be called multiple times until bytes remainming
    in the transfer sequence is zero.
     Example: Transer 4000 bytes.  Max diag transfer size is 3800
      REQ -> rfnv_append_data = 0 rfnv_item_size = 3800
      RSP <- err_code = RFCOMMON_NV_WRITE_SUCCESS
      REQ -> rfnv_append_data = 1 rfnv_item_size = 200
      RSP <- err_code = RFCOMMON_NV_WRITE_SUCCESS

DEPENDENCIES
   

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rfnv_write_v2(ftm_rfnv_write_request_v2_type *req)
{
  ftm_rsp_pkt_type rsp;
  ftm_rfnv_write_response_v2_type *rfnv_rsp;
  char path[RFNV_MAX_PATH] = "";
  int fs_handle            = 0;
  int32       BytesWritten = 0;
  
  rsp = ftmdiag_create_new_pkt(sizeof(ftm_rfnv_write_response_v2_type));

  if(NULL == rsp.pkt_payload)
  {
    // Exit if memory allocation for reponse data failed
    FTM_MSG_HIGH("ftm_rfnv_write_v2 Item ID=%d memory allocation for reponse data failed" ,req->header.rfnv_item_code, 0, 0);
    return rsp;
  }

  rfnv_rsp           = rsp.pkt_payload;
  ftm_rfnv_fill_common_header(&req->header.cmn_header, &rfnv_rsp->header.cmn_header, rsp.pkt_len);

  rfnv_rsp->err_code = RFCOMMON_NV_WRITE_SUCCESS;
  rfnv_rsp->rfnv_item_code = req->header.rfnv_item_code;

  if( req->header.rfnv_item_code < RFNV_USE_EFS_NV_CUTOFF )
  {
    if (NV_DONE_S != rfcommon_rfnv_set_item((rfnv_item_id_enum_type)(req->header.rfnv_item_code), (rfnv_item_type *)req->rfnv_item_data, req->header.rfnv_item_size, rex_self(), FS_OP_COMPLETE_SIG, ftm_sig_rex_wait_cb))
    {
      rfnv_rsp->err_code = RFCOMMON_NV_WRITE_FAILED;
    }

    return rsp;
  }
  
  if (req->header.rfnv_item_size > RFNV_MAX_TRANSFER_SIZE)
  {
    rfnv_rsp->err_code = RFCOMMON_NV_INVALID_SIZE;
    FTM_MSG_HIGH("ftm_rfnv_write_v2 Item ID=%d Invalid Item Size: %d",req->header.rfnv_item_code, req->header.rfnv_item_size, 0);
    return rsp;
  }

  (void) snprintf(path, RFNV_MAX_PATH, "%s%08d", RFNV_FILE_PATH, req->header.rfnv_item_code); 
  path[RFNV_MAX_PATH-1] = 0; 
  
  if (req->rfnv_append_data == 1) 
  {
    fs_handle = efs_open(path, O_WRONLY | O_APPEND | O_CREAT, 0755);
    if(fs_handle == -1)
    {
       rfnv_rsp->err_code = RFCOMMON_NV_FILE_OPEN_FAIL;
       FTM_MSG_HIGH("ftm_rfnv_write_v2 Item ID=%d Failed to Open File O_WRONLY | O_APPEND | O_CREAT, 0755",req->header.rfnv_item_code, 0, 0);
       return rsp;
    }
  }
  else
  {
    fs_handle = efs_open(path, O_RDWR | O_CREAT | O_TRUNC, 0755);
    if(fs_handle == -1)
    {
       rfnv_rsp->err_code = RFCOMMON_NV_FILE_OPEN_FAIL;
       FTM_MSG_HIGH("ftm_rfnv_write_v2 Item ID=%d Failed to Open File O_RDWR | O_CREAT | O_TRUNC, 0755" ,req->header.rfnv_item_code, 0, 0);
       return rsp;
    }
  }

  BytesWritten = efs_write(fs_handle, 
                           (rfnv_item_type *)req->rfnv_item_data,
                           req->header.rfnv_item_size);

  if (BytesWritten != req->header.rfnv_item_size)
  {
     rfnv_rsp->err_code = RFCOMMON_NV_WRITE_FAILED;
     FTM_MSG_HIGH("ftm_rfnv_write_v2 Item ID=%d Write Failed" ,req->header.rfnv_item_code, 0, 0);
  }
    
  efs_close(fs_handle);

  FTM_MSG_MED("ftm_rfnv_write_v2 Item ID=%d Sucess" ,req->header.rfnv_item_code, 0, 0);

  return rsp;
}

/*===========================================================================

FUNCTION ftm_rfnv_get_max_transfer_size

DESCRIPTION 
   - Prepares response package. 
   - Sets the max number of bytes that can be tranported via the Diag payload
   - Set status to reposnse package.

DEPENDENCIES 
 
   
RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rfnv_get_max_transfer_size(ftm_rfnv_get_max_transfer_size_request_type *req)
{
  ftm_rsp_pkt_type rsp;
  ftm_rfnv_get_max_transfer_size_response_type *rfnv_rsp;
  
  rsp = ftmdiag_create_new_pkt(sizeof(ftm_rfnv_get_max_transfer_size_response_type));

  if(NULL == rsp.pkt_payload)
  {
    // Exit if memory allocation for reponse data failed
    return rsp;
  }

  rfnv_rsp           = rsp.pkt_payload;
  ftm_rfnv_fill_common_header(&req->header.cmn_header, &rfnv_rsp->header.cmn_header, rsp.pkt_len);
  rfnv_rsp->err_code               = RFCOMMON_NV_READ_SUCCESS; 
  rfnv_rsp->rfnv_max_transfer_size = RFNV_MAX_TRANSFER_SIZE;
  
  return rsp;
}

/*===========================================================================

FUNCTION FTM_RFNV_DISPATCH

DESCRIPTION
   This function handles requests by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_rfnv_dispatch(ftm_rfnv_common_header_type *ftm_rfnv_cmd)
{

  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  FTM_MSG_MED("FTM RFNV dispatching packet id: %d",ftm_rfnv_cmd->ftm_cmd, 0, 0);

  switch (ftm_rfnv_cmd->ftm_cmd)
  {
  
  case FTM_RFNV_READ:
    rsp_pkt =  ftm_rfnv_read((ftm_rfnv_read_request_type*) ftm_rfnv_cmd);
    break;

  case FTM_RFNV_WRITE:
    rsp_pkt =  ftm_rfnv_write((ftm_rfnv_write_request_type*) ftm_rfnv_cmd);
    break;

  case FTM_RFNV_READ_V2:
    rsp_pkt =  ftm_rfnv_read_v2((ftm_rfnv_read_request_v2_type*) ftm_rfnv_cmd);
    break;

  case FTM_RFNV_WRITE_V2:
    rsp_pkt =  ftm_rfnv_write_v2((ftm_rfnv_write_request_v2_type*) ftm_rfnv_cmd);
    break;

  case FTM_RFNV_GET_MAX_TRANSFER_SIZE:
    rsp_pkt =  ftm_rfnv_get_max_transfer_size((ftm_rfnv_get_max_transfer_size_request_type*) ftm_rfnv_cmd);
    break;

  default:
    FTM_MSG_ERROR("Unknown FTM RFNV Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
  }

  FTM_MSG_MED("Done processing FTM RFNV packet id: %d",ftm_rfnv_cmd->ftm_cmd, 0, 0);
  return rsp_pkt;
}   
#endif /*FEATURE_FACTORY_TESTMODE */
