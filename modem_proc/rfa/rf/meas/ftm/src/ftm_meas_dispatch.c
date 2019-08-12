/*----------------------------------------------------------------------------*/
/*!
                      
  @brief
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2009 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/meas/ftm/src/ftm_meas_dispatch.c#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/12/13   rmb     Add FTM commands for W2W IRAT.
08/13/13   pl      Fix KW warnings 
08/13/13   pl      Update FTM IRAT response packet to include script info
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
01/22/13   pl      Implement code review comments 
01/20/13   pl      FTM LTE-CA IRAT support
10/16/12   ka      Added W2W FTM test 
10/09/12   pl      FTM IRAT cleanup
05/14/12   sar     Removed rtr8600 device.
05/09/11   sar     Added ftm_rf_cmd.h for CMI4.
03/24/11   pl      Fix warnings
03/07/11   jyu     Added Level 1&2 RF FTM IRAT test  
02/24/11   tnt     Merge to MDM9k PLF110
10/06/10   jyu     added test for xtoy_test_y and xtoy_test_x IRAT
09/24/10   kguo    Featurized LTOW IRAT test 
07/27/10   pl      Added test for x2DO IRAT
07/27/10   pl      Added test for x2L IRAT
06/02/10   kguo    Added support for LTOW test 
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/21/09   jps     fixed lint error (out of bounds) for ftm_meas_extended_pkt_type 
12/01/09   ckl     Added command dispatch for W2W.
11/09/09   jfc     Lint Fix: suppress warnings 
09/15/09   jps     Initial version of the file

*/

/*------------------------------------------------------------------------------
                               INCLUDES
------------------------------------------------------------------------------*/

#include "rfa_variation.h"
#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "stringl.h" /* for memscpy */

#include "ftmicap.h"

#ifdef FTM_HAS_IRAT

#include "ftm.h"
#include "ftm_meas_dispatch.h"
#include "ftm_meas.h"
#include "ftm_common.h"
#include "rf_test_task_v.h"
#include "rfmeas_types.h"
#include "ftm_rf_cmd.h"


/*------------------------------------------------------------------------------
                       VARIABLES
------------------------------------------------------------------------------*/
typedef PACKED union
{
  ftm_meas_extended_result_type     gen_result;
  
} ftm_meas_extended_result_union;


/*------------------------------------------------------------------------------
                       EXTERNAL FUNCTIONS
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
                      PUBLIC FUNCTION IMPLEMENTATIONS
------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*!
   @brief
   This function handles requests to run MEAS tests and other primitives
   by dispatching the appropriate functions.

   @param
   cmd_ptr - 

   @retval
   Response packet
*/
ftm_rsp_pkt_type ftm_meas_extended_dispatch(ftm_meas_extended_pkt_type *cmd_ptr)
{
  uint16 serv_cell = 0, serv_chan= 0, meas_band = 0, meas_chan = 0;
  boolean tune_neighbour;
  uint16 num_trx = 0, num_trx_tasks = 0, num_non_trx = 0, num_non_trx_tasks = 0, exec_time_us = 0;
  rfm_meas_generic_params_type test_param;
  ftm_rsp_pkt_type               rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_composite_cmd_header_type  rsp_headers;
  /* local_buffer and extra_data_len allow return data size to depend on a 
     request parameter. Unused for now */
  uint16                         *local_buffer  = NULL;
  uint16                         extra_data_len = 0; 
  ftm_meas_extended_result_union   result;
  /* Pointer to parameter buffer */
  uint16                           *buff_ptr = NULL;

  if (cmd_ptr == NULL) 
  {
    rsp_pkt.cmd = FTM_RSP_NO_RSP;
    rsp_pkt.pkt_len = 0;
    rsp_pkt.pkt_payload = NULL;
    rsp_pkt.delete_payload = FALSE;
    return rsp_pkt;
  }

  /* Initialize response packet size to length of the common headers */
  /* the size of the return parameters will be added in the dispatch cases below */
  rsp_headers.ftm_hdr.cmd_rsp_pkt_size = sizeof(ftm_composite_cmd_header_type);

  buff_ptr = (uint16*)&(cmd_ptr->ftm_meas_extended_params[0]);
  if (buff_ptr == NULL) 
  {
    rsp_pkt.cmd = FTM_RSP_NO_RSP;
    rsp_pkt.pkt_len = 0;
    rsp_pkt.pkt_payload = NULL;
    rsp_pkt.delete_payload = FALSE;
    return rsp_pkt;
  }

  switch (cmd_ptr->ftm_hdr.cmd_id)
  {
    case FTM_MEAS_WTOW_TEST:
    {
      serv_cell = *buff_ptr++;
      serv_chan = *buff_ptr++;
      meas_band = *buff_ptr++;
      meas_chan = *buff_ptr;
      ftm_meas_wtow_test_wcdma(serv_cell,serv_chan,meas_band,meas_chan);
      result.gen_result.error_code = (int32)FTM_SUCCESS;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
    }
    break;

    case FTM_MEAS_WRITE_WTOW:
    serv_cell = *buff_ptr++;
    
    ftm_meas_wtow_execute_buffer(serv_cell);
    result.gen_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
    break;

    case FTM_MEAS_WTOW_TEST_BUILD_SCRIPTS_V2:
    {
      serv_chan = *buff_ptr++;
      meas_chan = *buff_ptr;
      ftm_meas_wtow_build_scripts(serv_chan,meas_chan);
      result.gen_result.error_code = (int32)FTM_SUCCESS;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
    }
    break;

    case FTM_MEAS_WTOW_TEST_WRITE_SCRIPTS_V2:
    tune_neighbour = *buff_ptr++;
    
    ftm_meas_wtow_write_scripts(tune_neighbour);
    result.gen_result.error_code = (int32)FTM_SUCCESS;
    rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
    break;

    case FTM_MEAS_XTOY_TEST_Y:
    {
      /* test_param preparation */
      if ( *buff_ptr >= RFM_NUM_MODES )
      {
        FTM_MSG_ERROR("Source mode=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.src_tech = (rfm_mode_enum_type)(*buff_ptr++);
  
      test_param.src_band = *buff_ptr++;
  
      test_param.src_chan = *buff_ptr++;
  
      if ( *buff_ptr >= RFCOM_NUM_LTE_BWS )
      {
        FTM_MSG_ERROR("Source bw=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.src_bw = (rfcom_lte_bw_type)(*buff_ptr++);
  
      test_param.src_div_en = *buff_ptr++;
      test_param.src_buffer_index = *buff_ptr++;

      if ( *buff_ptr >= RFM_NUM_MODES )
      {
        FTM_MSG_ERROR("Target mode=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.tgt_tech = (rfm_mode_enum_type)(*buff_ptr++);
  
      test_param.tgt_band = *buff_ptr++;
  
      test_param.tgt_chan = *buff_ptr++;
  
      if ( *buff_ptr >= RFCOM_NUM_LTE_BWS )
      {
        FTM_MSG_ERROR("Target bw=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.tgt_bw = (rfcom_lte_bw_type)(*buff_ptr++);
  
      test_param.tgt_div_en = *buff_ptr++;
      test_param.tgt_buffer_index = *buff_ptr++;
      test_param.test_level = *buff_ptr++;

      test_param.src_extra_carrier_flag = *buff_ptr++;
      test_param.src_band_carrier_1 = *buff_ptr++;
      test_param.src_chan_carrier_1 = *buff_ptr++;
      if ( *buff_ptr >= RFCOM_NUM_LTE_BWS )
      {
        FTM_MSG_ERROR("Source CAR_1 bw=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.src_bw_carrier_1 = *buff_ptr++;
      test_param.src_div_en_carrier_1 = *buff_ptr;

      ftm_meas_xtoy_test_y(&test_param);

      result.gen_result.error_code = (int32)FTM_SUCCESS;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
    }
    break;

    case FTM_MEAS_XTOY_TEST_X:
    {
      /* test_param preparation */
      if ( *buff_ptr >= RFM_NUM_MODES )
      {
        FTM_MSG_ERROR("Source mode=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.src_tech = (rfm_mode_enum_type)(*buff_ptr++);
  
      test_param.src_band = *buff_ptr++;
  
      test_param.src_chan = *buff_ptr++;
  
      if ( *buff_ptr >= RFCOM_NUM_LTE_BWS )
      {
        FTM_MSG_ERROR("Source bw=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.src_bw = (rfcom_lte_bw_type)(*buff_ptr++);
  
      test_param.src_div_en = *buff_ptr++;
      test_param.src_buffer_index = *buff_ptr++;

      if ( *buff_ptr >= RFM_NUM_MODES )
      {
        FTM_MSG_ERROR("Target mode=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.tgt_tech = (rfm_mode_enum_type)(*buff_ptr++);
  
      test_param.tgt_band = *buff_ptr++;
  
      test_param.tgt_chan = *buff_ptr++;
  
      if ( *buff_ptr >= RFCOM_NUM_LTE_BWS )
      {
        FTM_MSG_ERROR("Target bw=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.tgt_bw = (rfcom_lte_bw_type)(*buff_ptr++);
  
      test_param.tgt_div_en = *buff_ptr++;
      test_param.tgt_buffer_index = *buff_ptr++;
      test_param.test_level = *buff_ptr++;

      test_param.src_extra_carrier_flag = *buff_ptr++;
      test_param.src_band_carrier_1 = *buff_ptr++;
      test_param.src_chan_carrier_1 = *buff_ptr++;
      if ( *buff_ptr >= RFCOM_NUM_LTE_BWS )
      {
        FTM_MSG_ERROR("Source CAR_1 bw=%d is not valid", *buff_ptr ,0,0);
        result.gen_result.error_code = (int32)FTM_FAILURE;
        rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
        break;
      }
      test_param.src_bw_carrier_1 = *buff_ptr++;
      test_param.src_div_en_carrier_1 = *buff_ptr;

      ftm_meas_xtoy_test_x(&test_param);
      result.gen_result.error_code = (int32)FTM_SUCCESS;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
    }
    break;

    /*  Illegal, or unknown MEAS test id  */
    default:
      result.gen_result.error_code = (int32)FTM_FAILURE;
      rsp_headers.ftm_hdr.cmd_rsp_pkt_size += sizeof(ftm_meas_extended_result_type);
      break;
  }

  /* Create response packet */
  /* Copy common header information from the request to the response */
  rsp_headers.diag_hdr             = cmd_ptr->diag_hdr;
  rsp_headers.ftm_hdr.cmd_id       = cmd_ptr->ftm_hdr.cmd_id;
  rsp_headers.ftm_hdr.cmd_data_len = cmd_ptr->ftm_hdr.cmd_data_len;

  local_buffer = ftm_malloc(sizeof(uint16)*10);
  if (local_buffer != NULL)
  {
    /* Get the additional information to complete the response packet */
    ftm_meas_get_irat_script_info(RFM_MEAS_PROGRAM_STARTUP,
                                  &num_trx,
                                  &num_trx_tasks,
                                  &num_non_trx,
                                  &num_non_trx_tasks,
                                  &exec_time_us);
    local_buffer[0] = num_trx;
    local_buffer[1] = num_trx_tasks;
    local_buffer[2] = num_non_trx;
    local_buffer[3] = num_non_trx_tasks;
    local_buffer[4] = exec_time_us;
    ftm_meas_get_irat_script_info(RFM_MEAS_PROGRAM_CLEANUP,
                                  &num_trx,
                                  &num_trx_tasks,
                                  &num_non_trx,
                                  &num_non_trx_tasks,
                                  &exec_time_us);
    local_buffer[5] = num_trx;
    local_buffer[6] = num_trx_tasks;
    local_buffer[7] = num_non_trx;
    local_buffer[8] = num_non_trx_tasks;
    local_buffer[9] = exec_time_us;
    extra_data_len = sizeof(uint16)*10;
  }
  /* At this point the response packet size equals the size of the headers plus
     the size of the fixed return parameters. If the response contains extra data
     whose size is determined by a parameter in the request, then we add it here. */
  rsp_pkt = ftmdiag_create_new_pkt(rsp_headers.ftm_hdr.cmd_rsp_pkt_size + extra_data_len);
  if ( rsp_pkt.pkt_payload != NULL )
  {
    /* copy header information */
    memscpy((void*)rsp_pkt.pkt_payload,
            sizeof(ftm_composite_cmd_header_type), 
           (void*)&rsp_headers, 
           sizeof(ftm_composite_cmd_header_type));

    /* copy fixed return parameters */
    memscpy((void*)((byte*)rsp_pkt.pkt_payload + sizeof(ftm_composite_cmd_header_type)), 
            (rsp_headers.ftm_hdr.cmd_rsp_pkt_size - sizeof(ftm_composite_cmd_header_type)),
           (void*)&result,
           (rsp_headers.ftm_hdr.cmd_rsp_pkt_size - sizeof(ftm_composite_cmd_header_type)));

    if ( extra_data_len > 0 )
    {
      /* copy extra variable length return data if present */
      memscpy((void*)((byte*)rsp_pkt.pkt_payload + rsp_headers.ftm_hdr.cmd_rsp_pkt_size),
              extra_data_len, 
             local_buffer, 
             extra_data_len);
    }
  }
  else
  {
    FTM_MSG_ERROR("Cannot allocate memory for response packet payload, cmd id = %d", 
                  cmd_ptr->ftm_hdr.cmd_id, 0, 0);
  }

  if ( local_buffer != NULL )
  {
    ftm_free(local_buffer);
  }

  return rsp_pkt;
} /* end ftm_meas_extended_dispatch */

#endif /* FTM_HAS_IRAT */
