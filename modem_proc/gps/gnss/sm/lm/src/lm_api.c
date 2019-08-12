/*----------------------------------------------------------------------------
 *  Copyright (c) 2014 Qualcomm Atheros, Inc.
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------
 */
/*======================================================================

                         Location Manager API definitions file

 GENERAL DESCRIPTION
  This file contains API definitions by which Location Manager(LM) services can 
  be accessed by other tasks (e.g Transport Manager). 

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2005 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header $$DateTime $$Author $ 

 when      who   what, where, why
 ----      ----  --- ---------------------------------------------------
 -
 07/15/15  al    Added support for prescribed dwell command versions 51 and 53 
 06/18/08  jlp   Added messaging support so tm_diag can set the Sv No Exist
                  list in cd.
 6/20/06   mr   Initial creation of header file.
======================================================================*/

#include "gps_variation.h"
#include "lm_api.h"
#include "lm_iface.h"
#include "aries_os_api.h"
#include "aries_os_globals.h"
#include "lm_data.h"
#define TM_SARF_CMD_ARRAY_SIZE ARIES_MSG_ID_TM_LM_END-ARIES_MSG_ID_TM_LM_FIRST-1

const uint16 tm_sarf_cmds_payload_sizes[TM_SARF_CMD_ARRAY_SIZE] =
                                       {sizeof(lm_sarf_mode_type),
                                        MAX(sizeof(lm_GnssPrescDwellType_V51),sizeof(lm_GnssPrescDwellType_V53)),
                                        sizeof(lm_GnssIqTestType),
                                        sizeof(lm_1sv_standalone_type),
                                        sizeof(lm_mem_bw_test_type),
                                        sizeof(lm_TsgCmdType),
                                        sizeof(lm_RfCmdType),
                                        sizeof(lm_multichan_track_type),
                                        sizeof(lm_PrxRfCmdType),
                                        sizeof(lm_RfLinearityCmdType),
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        sizeof(lm_SpectrumAnalyzerCmdType)};

/****************** Function Definitions**************************************/

/*****************************LM-TM interface functions***********************/

/*
 ******************************************************************************
 * lm_session_request 
 *
 * Function description:
 *
 * This function is called by TM for:
 * 1.  starting/stopping/updating a session with LM
 * 2.  To request a PPM report, PRM report or position report
 *     for an already established session.
 *
 * Parameters: 
 * p_req - Pointer to structure containing request parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/
boolean lm_session_request(const lm_session_request_info_s_type *p_req)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  lm_session_request_info_s_type *p_hdr_ptr;

  if(p_req == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(lm_session_request_info_s_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (lm_session_request_info_s_type*) p_ipc_ptr->p_Data;/*lint !e826 */

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_MSG_SESSION_REQUEST;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = sizeof(lm_session_request_info_s_type);

      *p_hdr_ptr = *p_req;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;

}


/*
 ******************************************************************************
 * lm_request 
 *
 * Function description:
 *
 * This function is called by TM to send a LM-TM session independent request to LM.
 * The type of request could be:
 * 1.  Turn keep warm processing ON/OFF
 * 2.  Turn NMEA sentence generation ON/OFF
 * 3.  Get MGP's Aiding data wishlist
 * 4.  Delete certain parameters in MGP database
 *
 * Parameters: 
 * p_req - Pointer to structure containing request parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/
boolean lm_request(const lm_request_info_s_type *p_req)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  lm_request_info_s_type *p_hdr_ptr;

  if(p_req == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(lm_request_info_s_type),
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (lm_request_info_s_type*) p_ipc_ptr->p_Data;/*lint !e826 */

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_MSG_REQUEST;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = sizeof(lm_request_info_s_type);

      *p_hdr_ptr = *p_req;

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;
}

/*
 ******************************************************************************
 * lm_translate_lm_info_to_ipc_type 
 *
 * Function description:
 *
 * This function translates from lm_info_s_type to lm_info_s_type_ipc_type
 *
 * Parameters: 
 * p_ipc_type - Pointer to structure lm_info_s_type_ipc_type
 * p_info - Pointer to structurelm_info_s_type
 *
 * Return value: 
 *
 * boolean - If conversion is succesfull or not
 *
 ******************************************************************************
*/

boolean lm_translate_lm_info_to_ipc_type(lm_info_s_type_ipc_type *p_ipc_type,
                                         const lm_info_s_type *p_info)
{
  boolean ret_value = TRUE;
  
  p_ipc_type->e_info_type = p_info->e_info_type;
  p_ipc_type->z_info.q_delay = p_info->z_info.q_delay;
  p_ipc_type->z_info.f_new_pos_unc = p_info->z_info.f_new_pos_unc; 
  p_ipc_type->z_info.q_pos_offset = p_info->z_info.q_pos_offset;
  p_ipc_type->z_info.f_new_time_unc = p_info->z_info.f_new_time_unc;  
  p_ipc_type->z_info.f_time_offset = p_info->z_info.f_time_offset;  
  p_ipc_type->z_info.z_sess_info = p_info->z_info.z_sess_info;  

  
  p_ipc_type->z_info.z_aiding_data.ref_loc_is_pos_fix = p_info->z_info.z_aiding_data.ref_loc_is_pos_fix;
  p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType = p_info->z_info.z_aiding_data.z_assist_data.e_GnssType;
  p_ipc_type->z_info.z_aiding_data.z_assist_data.e_AssistanceType = (uint32)p_info->z_info.z_aiding_data.z_assist_data.e_AssistanceType;
  p_ipc_type->z_info.z_aiding_data.z_assist_data.u_Force = (uint8)p_info->z_info.z_aiding_data.z_assist_data.u_Force;
  
  if(p_ipc_type->e_info_type == LM_SESSION_INFO_AIDING_DATA)
  {
    switch(p_info->z_info.z_aiding_data.z_assist_data.e_AssistanceType)
    {
      case MGP_GNSS_ASSIST_DATA_REF_TIME: 
          {
            if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
            {
              memscpy((char*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
               sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo),
                     (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo,
                      sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo)
                     );
            }
            else
            {

              memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
               sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo),
                     (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo,
                     sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo)
                     );        
            }
            break;
         }

      case MGP_GNSS_ASSIST_DATA_REF_LOC: 
          {
              memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
               sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo),
                     (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo,
                     sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo)
                     );        
            break;
          }
      

      case MGP_GNSS_ASSIST_DATA_ACQ_ASSIST: 
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(gnss_SvAcqAssistCommonStructType),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvAcqAssistInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvAcqAssistInfo)
                   );        
            break;
          }

      case MGP_GNSS_ASSIST_DATA_SV_DIR: 
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(gnss_SvDirStructType),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GnssSvDirInfo,
                    sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GnssSvDirInfo)
                   );
            break;
          }
     
/*AHSIAU_OBSOLETE?
      case MGP_GNSS_ASSIST_DATA_SA_DATA: 
          {
              memcpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
                     (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SADataInfo,
                      sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SADataInfo)
                     );
            break;
          }
*/

    case MGP_GNSS_ASSIST_DATA_EPH: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo)
                   );
          }
          else
          {
    
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo)
                   );        
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_ALM: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo)
                   );
          }
          else
          {
    
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo)
                   );        
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_IONO: 
        {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo)
                   );
          break;
        }
      

    case MGP_GNSS_ASSIST_DATA_RTI: 
        {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_SV_NO_EXIST: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo)
                   );
          }
          else
          {
    
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo)
                   );        
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_PRE_FIX_PPM: 
        {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_UTC: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo)
                   );
          }
          else
          {
    
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo)
                   );        
          }
          break;
        }

      case MGP_GNSS_ASSIST_DATA_GNSS_TIME_MODEL: 
        {
          /* GPS & GLONASS system uses same TimeModelInfo structure */
          if ( ( p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO ) ||
               ( p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GPS ) )
          {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo)
                  );
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_ALM_XTRA: 
      {
        if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
        {
          memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo),
                 (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo,
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo)
                 );
        }
        else
        {
      
          memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo),
                 (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo,
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo)
                 );        
        }
        break;
      }
    
    case MGP_GNSS_ASSIST_DATA_ADV_HEALTH: 
      {
        if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
        {
          memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo),
                 (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo,
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo)
                 );
        }
        else
        {
      
          memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo),
                 (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo,
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo)
                 );        
        }
        break;
      }

    case MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO: 
        {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_GGTO: 
        {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_SV_UNHEALTHY: 
        {
             memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_FTA_DATA: 
        {
            memscpy((void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo),
                   (void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo,
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo)
                   );
          break;
        }

    default:
        LM_MSG_ERROR("Unknown Assistance Type %d",p_info->z_info.z_aiding_data.z_assist_data.e_AssistanceType,0,0);
        ret_value = FALSE;
        break;     
    }
  }

  return ret_value;
}


/*
 ******************************************************************************
 * lm_translate_lm_info_ipc_type_to_lm_info 
 *
 * Function description:
 *
 * This function translates from lm_info_s_type_ipc_type to lm_info_s_type
 *
 * Parameters: 
 * p_ipc_type - Pointer to structure lm_info_s_type_ipc_type
 * p_info - Pointer to structurelm_info_s_type
 *
 * Return value: 
 *
 * boolean - If conversion is succesfull or not
 *
 ******************************************************************************
*/
boolean lm_translate_lm_info_ipc_type_to_lm_info(lm_info_s_type_ipc_type *p_ipc_type,
                                                 lm_info_s_type *p_info)
{
  boolean ret_value = TRUE;
  
  p_info->e_info_type = p_ipc_type->e_info_type;
  p_info->z_info.q_delay = p_ipc_type->z_info.q_delay;
  p_info->z_info.f_new_pos_unc = p_ipc_type->z_info.f_new_pos_unc; 
  p_info->z_info.q_pos_offset = p_ipc_type->z_info.q_pos_offset;
  p_info->z_info.f_new_time_unc = p_ipc_type->z_info.f_new_time_unc;  
  p_info->z_info.f_time_offset = p_ipc_type->z_info.f_time_offset;  
  p_info->z_info.z_sess_info = p_ipc_type->z_info.z_sess_info;  

  
  p_info->z_info.z_aiding_data.ref_loc_is_pos_fix = p_ipc_type->z_info.z_aiding_data.ref_loc_is_pos_fix;
  p_info->z_info.z_aiding_data.z_assist_data.e_GnssType = p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType;
  p_info->z_info.z_aiding_data.z_assist_data.e_AssistanceType = p_ipc_type->z_info.z_aiding_data.z_assist_data.e_AssistanceType;
  p_info->z_info.z_aiding_data.z_assist_data.u_Force = (boolean)p_ipc_type->z_info.z_aiding_data.z_assist_data.u_Force;
  
  if(p_info->e_info_type == LM_SESSION_INFO_AIDING_DATA)
  {
    switch(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_AssistanceType)
    {
      case MGP_GNSS_ASSIST_DATA_REF_TIME: 
          {
            if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
            {
              memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo,
               sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo),
                     (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                   
                      sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo)
                     );
            }
            else
            {

              memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo,
               sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo),
                     (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,
                     sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo)
                     );        
            }
            break;
         }

      case MGP_GNSS_ASSIST_DATA_REF_LOC: 
          {
              memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo,
               sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo),
                     (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                   
                     sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo)
                     );        
            break;
          }
      

      case MGP_GNSS_ASSIST_DATA_ACQ_ASSIST: 
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvAcqAssistInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvAcqAssistInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                   
                   sizeof(gnss_SvAcqAssistCommonStructType)
                   );        
            break;
          }

      case MGP_GNSS_ASSIST_DATA_SV_DIR: 
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GnssSvDirInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GnssSvDirInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                   
                   sizeof(gnss_SvDirStructType));
            break;
          }
     
      
/*AHSIAU_OBSOLETE?
      case MGP_GNSS_ASSIST_DATA_SA_DATA: 
          {
              memcpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SADataInfo,
                     (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                   
                     sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SADataInfo)
                     );
            break;
          }
*/

    case MGP_GNSS_ASSIST_DATA_EPH: 
        {
          if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo)
                   );
          }
          else
          {
    
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo)
                   );        
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_ALM: 
        {
          if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo)
                   );
          }
          else
          {
    
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo)
                   );        
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_IONO: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo)
                   );
          break;
        }
      

    case MGP_GNSS_ASSIST_DATA_RTI: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_SV_NO_EXIST: 
        {
          if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo)
                   );
          }
          else
          {
    
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo)
                   );        
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_PRE_FIX_PPM: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_UTC: 
        {
          if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo)
                   );
          }
          else
          {
    
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo)
                   );        
          }
          break;
        }

      case MGP_GNSS_ASSIST_DATA_GNSS_TIME_MODEL:
        {
          /* GPS & GLONASS system uses same TimeModelInfo structure */
          if ((p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO) ||
              (p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GPS))
          {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo)
                   );
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_ALM_XTRA: 
      {
        if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
        {
          memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo),
                 (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,               
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo)
                 );
        }
        else
        {
      
          memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo),
                 (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,               
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo)
                 );        
        }
        break;
      }
    
    case MGP_GNSS_ASSIST_DATA_ADV_HEALTH: 
      {
        if(p_ipc_type->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
        {
          memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo),
                 (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,               
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo)
                 );
        }
        else
        {
      
          memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo,
             sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo),
                 (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,               
                 sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo)
                 );        
        }
        break;
      }

    case MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_GGTO: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_SV_UNHEALTHY: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo)
                   );
          break;
        }

    case MGP_GNSS_ASSIST_DATA_FTA_DATA: 
        {
            memscpy((void*)&p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo,
           sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo),
                   (void*)p_ipc_type->z_info.z_aiding_data.z_assist_data.z_AssistData,                 
                   sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo)
                   );
          break;
        }

    default:
        LM_MSG_ERROR("Unknown Assistance Type %d",p_ipc_type->z_info.z_aiding_data.z_assist_data.e_AssistanceType,0,0);
        ret_value = FALSE;
        break;     
    }
  }

  return ret_value;
}


/*
 ******************************************************************************
 * lm_calculate_lm_info_ipc_size 
 *
 * Function description:
 *
 * This function calculates the IPC size required for lm_info_s_type
 *
 * Parameters: 
 * p_info - Pointer to structure lm_info_s_type
 *
 * Return value: 
 *
 * boolean - If calculation is succesfull or not
 *
 ******************************************************************************
*/
uint32 lm_calculate_lm_info_ipc_size(const lm_info_s_type *p_info)
{
  uint32 ipc_size = 0;

  
  ipc_size += sizeof(lm_info_s_type_ipc_type);
  
  if(p_info->e_info_type == LM_SESSION_INFO_AIDING_DATA)
  {
    switch(p_info->z_info.z_aiding_data.z_assist_data.e_AssistanceType)
    {
      case MGP_GNSS_ASSIST_DATA_REF_TIME: 
          {
            if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
            {
              ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloRefTimeInfo);
            }
            else
            {
              ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefTimeInfo);            
            }
            break;
         }

      case MGP_GNSS_ASSIST_DATA_REF_LOC: 
          {
              
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RefLocInfo);
            break;
          }
      

      case MGP_GNSS_ASSIST_DATA_ACQ_ASSIST: 
          {
            ipc_size += sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvAcqAssistInfo);
            break;
          }

      case MGP_GNSS_ASSIST_DATA_SV_DIR: 
          {
            ipc_size += sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GnssSvDirInfo);
            break;
          }
     
/*AHSIAU_OBSOLETE?
      case MGP_GNSS_ASSIST_DATA_SA_DATA: 
          {
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SADataInfo);                                    
            break;
          }
*/

    case MGP_GNSS_ASSIST_DATA_EPH: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloEphInfo);                                    
            
          }
          else
          {
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_EphInfo);                                    
            
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_ALM: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo);                                    
            
          }
          else
          {  
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo);                                              
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_IONO: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_IonoInfo);                                            
          break;
        }
      

    case MGP_GNSS_ASSIST_DATA_RTI: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RtiInfo);                                                     
          break;
        }

    case MGP_GNSS_ASSIST_DATA_SV_NO_EXIST: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            ipc_size += sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloNonExistenceInfo);                                                     
            
          }
          else
          {  
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvNoExistInfo);                                                               
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_PRE_FIX_PPM: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_PreFixPpmInfo);                                                                         
          break;
        }

    case MGP_GNSS_ASSIST_DATA_UTC: 
        {
          if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
          {
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloUtcInfo);                                                                                   
          }
          else
          {
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_UtcInfo);                                                                                   
          }
          break;
        }

      case MGP_GNSS_ASSIST_DATA_GNSS_TIME_MODEL: 
        {
          if ( (p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO) ||
               ( p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GPS) )
          {
            // Common time model struct for both GPS + GLONASS
            ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_TimeModelInfo);
          }
          break;
        }

    case MGP_GNSS_ASSIST_DATA_ALM_XTRA: 
      {
        if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloAlmInfo);                                                                                   
          
        }
        else
        {    
          ipc_size += sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_AlmInfo);                                                                                           
        }
        break;
      }
    
    case MGP_GNSS_ASSIST_DATA_ADV_HEALTH: 
      {
        if(p_info->z_info.z_aiding_data.z_assist_data.e_GnssType == MGP_GNSS_TYPE_GLO)
        {
          ipc_size += sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GloHealthInfo);                                                                                                   
        }
        else
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GpsHealthInfo);                                                                                                   
        }
        break;
      }

    case MGP_GNSS_ASSIST_DATA_RF_DEV_CMD_INFO: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_RfDevCmdInfo);                                                                                                             
          break;
        }

    case MGP_GNSS_ASSIST_DATA_GGTO: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_GGTO);                                                                                                                       
          break;
        }

    case MGP_GNSS_ASSIST_DATA_SV_UNHEALTHY: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_SvUnhealthyInfo);                                                                                                                                
          break;
        }

    case MGP_GNSS_ASSIST_DATA_FTA_DATA: 
        {
          ipc_size +=  sizeof(p_info->z_info.z_aiding_data.z_assist_data.z_AssistData.z_FTAInfo);                                                                                                                                          
          break;
        }

    default:
        LM_MSG_ERROR("Unknown Assistance Type %d",p_info->z_info.z_aiding_data.z_assist_data.e_AssistanceType,0,0);
        ipc_size = 0;
        break;     
    }
  }

  return ipc_size;
}


/*
 ******************************************************************************
 * lm_info 
 *
 * Function description:
 *
 * This function is called by TM to send LM-TM session independent info to LM.
 * The information type can be:
 * 1.  Aiding data to be injected in MGP.
 * 2.  Expected network delays for getting aiding data.
 *
 * Parameters: 
 * p_info - Pointer to structure containing information parameters
 *
 * Return value: 
 *
 * boolean - IPC send successful or not 
 *
 ******************************************************************************
*/
boolean lm_info(const lm_info_s_type      *p_info)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  lm_info_s_type_ipc_type *p_hdr_ptr;

  if(p_info == NULL)
  {
    result = FALSE;
  }
  else
  {
    uint32 ipc_size = lm_calculate_lm_info_ipc_size(p_info);

    if(ipc_size == 0)        
    {
      LM_MSG_ERROR("Lm info IPC Size calculated as 0",0,0,0);
      result = FALSE;
    }
    else
    {
      p_ipc_ptr = os_IpcCreate(ipc_size,
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
        LM_MSG_ERROR("Unable to allocate Ipc memory for %d bytes",ipc_size,0,0);
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
        p_hdr_ptr = (lm_info_s_type_ipc_type*) p_ipc_ptr->p_Data;/*lint !e826 */

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_MSG_INFO;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
        p_ipc_ptr->q_Size           = ipc_size;

        if(lm_translate_lm_info_to_ipc_type(p_hdr_ptr,p_info))
        {
      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
             LM_MSG_ERROR("Failure in sending IPC",0,0,0);
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }
        }
        else
        {                  
          LM_MSG_ERROR("Unable to translate lm_info to lm_info ipc type",0,0,0);
          result = FALSE;
          (void)os_IpcDelete(p_ipc_ptr);
        }

     }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;
}


/*****************************LM-GPSDIAG interface functions***********************/

/*===========================================================================

FUNCTION lmDiag_ForwardGSCPkt

DESCRIPTION
  This procedure is called by ARIES GPS Diag to send GPS Session Control 
  DIAG packets to LM.
  
DEPENDENCIES
  None.

RETURN VALUE
boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/
boolean lmDiag_ForwardGSCPkt(
  PACKED const void *p_req_pkt_ptr,
  word q_pkt_len
)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  
  void *p_hdr_ptr;

  if(p_req_pkt_ptr == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(q_pkt_len,
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      /*Pack the message in IPC HDR*/
      p_hdr_ptr = (void*) p_ipc_ptr->p_Data;

      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_DIAG_GSC_MSG;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = q_pkt_len;

      memscpy((void*)p_hdr_ptr, p_ipc_ptr->q_Size, (void*)p_req_pkt_ptr, q_pkt_len);

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

     }

  }/*if(p_req ==  NULL) else {}*/


  return result;
}


/*===========================================================================

FUNCTION lm_start_sbas_test

DESCRIPTION
  This funtion starts the SBAS SV test mode which is very similar to the GPS
  Single SV track.
  
DEPENDENCIES
  None.

PARAMETERS
  p_MgpConfig - MGP Configuration 

RETURN VALUE
  boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/

boolean lm_start_sbas_test( const mgp_ConfigStructType * const p_MgpConfig )
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  

  if(p_MgpConfig == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr = os_IpcCreate(sizeof(mgp_ConfigStructType),
                             IPC_ALLOCATION_DYNAMIC,
                             (uint32)THREAD_ID_SM_LM); 


    if(p_ipc_ptr == NULL)
    {
      result = FALSE;
    }
    else
    {
      p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_SBAS_START;
      p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
      p_ipc_ptr->q_Size           = sizeof(mgp_ConfigStructType);

      memscpy( (mgp_ConfigStructType *)p_ipc_ptr->p_Data, sizeof(*p_MgpConfig), p_MgpConfig, sizeof(*p_MgpConfig) );

      if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
      {
         result = FALSE;
         (void)os_IpcDelete(p_ipc_ptr);
      }

    }

  }/*if(p_req ==  NULL) else {}*/


  return result;

}



/*===========================================================================

FUNCTION lm_stop_sbas_test

DESCRIPTION
  This funtion stops the SBAS SV test mode which was initiated by 
  lm_start_sbas_test.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  boolean - IPC send successful or not 

SIDE EFFECTS
  None.

===========================================================================*/

boolean lm_stop_sbas_test( void )

{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  boolean result = TRUE;  

  p_ipc_ptr = os_IpcCreate(0,  /* No Payload */
                           IPC_ALLOCATION_DYNAMIC,
                           (uint32)THREAD_ID_SM_LM); 

  if(p_ipc_ptr == NULL)
  {
    result = FALSE;
  }
  else
  {
    p_ipc_ptr->q_MsgId          = (uint32)C_SM_LM_SBAS_STOP;

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
    {
       result = FALSE;
       (void)os_IpcDelete(p_ipc_ptr);
    }

  }


  return result;

}

/*===========================================================================
FUNCTION lm_send_sarf_cmd

DESCRIPTION
  Call this function to send SARF cmds to LM:-
  1. Mode switch
  2. Single SV
  3. Presc dwell
  4. IQ test
  5. Mem BW test

PARAMETERS
  p_payload - Pointer to payload
  lm_ipc_msg_id - Enum identifying the SARF cmd. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send failed, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean lm_send_sarf_cmd (PACKED uint8* p_payload, tm_diag_lm_msg_id_type lm_ipc_msg_id)
{
  os_IpcMsgType* p_Msg;
  boolean u_return_status = TRUE;/* Initialize to failure state */
  uint16 w_payload_size;
  lm_multichan_track_type* p_rfdev_multichan_cmd;
  int32 q_cmd_index =  ((int32)lm_ipc_msg_id - (int32)ARIES_MSG_ID_TM_LM_FIRST) - 1;
    
  /*lint -e{656} : Arithmetic operation uses (compatible) enum's */
  /*lint -e{641} : Converting enum 'tm_diag_lm_msg_id_type' to int */
  if(q_cmd_index >= 0 && q_cmd_index < TM_SARF_CMD_ARRAY_SIZE )
  {
    w_payload_size = tm_sarf_cmds_payload_sizes[q_cmd_index];
  }
  else
  {
    return FALSE;
  }

  /* Before sending IPC message, do sanity check on the cmd if the 
     cmd is for Callisto RF Dev multi-chan track cmd */
  if(q_cmd_index == (int32)ARIES_MSG_ID_TM_LM_RFDEV_TEST)
  {
    p_rfdev_multichan_cmd = (lm_multichan_track_type*) p_payload;
    if(((p_rfdev_multichan_cmd->l_GLO_Freq_ID == -8) && (p_rfdev_multichan_cmd->u_GLO_HW_Chan_Num != 0))||
       ((p_rfdev_multichan_cmd->u_Test_Mode != (uint8)MultiChan_Track_SBAS) && (p_rfdev_multichan_cmd->u_GPS_SBAS_SV_ID > 32))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_SBAS) && (p_rfdev_multichan_cmd->u_GPS_SBAS_SV_ID <= 32))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_PRI_GLO_BDS) && (p_rfdev_multichan_cmd->u_BDS_SV_ID > 37))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_PRI_BDS) && (p_rfdev_multichan_cmd->u_BDS_SV_ID > 37))||
       ((p_rfdev_multichan_cmd->u_Test_Mode == (uint8)MultiChan_Track_BDS) && (p_rfdev_multichan_cmd->u_BDS_SV_ID > 37)) ||
       ( p_rfdev_multichan_cmd->u_GAL_SV_ID > 36)
       )
    {
      /* Invalid Cmd parameter entered by user. Skip sending IPC message and return DM Error Response*/

      return TRUE;
    }
  }

  /* Dispatch LSM Subsystem requests */
  p_Msg = os_IpcCreate((uint32) w_payload_size, 
                       IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_LM);

  if (p_Msg)
  {
    /* Fill and send the IQ Collect Msg to LM */
    p_Msg->q_MsgId = (uint32)lm_ipc_msg_id;

    memscpy((void*)p_Msg->p_Data, w_payload_size, (void*) p_payload, w_payload_size);

    if (!os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_LM))
    {
     (void)os_IpcDelete(p_Msg);
      ERR_FATAL("Not able to send SARF cmd to LM", 0, 0, 0);
    }
    else
    {
      u_return_status = FALSE;
    }
  }
  else
  {
    ERR_FATAL("Not able to create ipc msg", 0, 0, 0);
  }

  return u_return_status;
} /* gpsdiag_IqCollectCmd */

/*===========================================================================
FUNCTION lm_send_sv_no_exist_cmd

DESCRIPTION
  Sends the Sv No Exist List on to lm.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send was successful, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean lm_send_sv_no_exist_cmd(const uint32 cq_NoExistMask,
 const sv_no_exist_action_type ce_Action)
{
  os_IpcMsgType * p_Msg;
  lm_sv_no_exist_cmd_type * pz_NoExistCmd;
  boolean u_ReturnStatus = FALSE;

  /* Dispatch LSM Subsystem requests */
  p_Msg = os_IpcCreate(sizeof(lm_sv_no_exist_cmd_type), IPC_ALLOCATION_DYNAMIC,
   (uint32)THREAD_ID_SM_LM);

  if (p_Msg)
  {
    /* Fill and send the IQ Collect Msg to LM */
    p_Msg->q_MsgId = (uint32)C_SM_LM_SV_NO_EXIST;

    pz_NoExistCmd = (lm_sv_no_exist_cmd_type *)p_Msg->p_Data;
    pz_NoExistCmd->q_SvNoExistMask = cq_NoExistMask;
    pz_NoExistCmd->e_Action = ce_Action;

    if (!os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_LM))
    {
      (void)os_IpcDelete(p_Msg);
      MSG_MED("Not able to send Sv No Exist cmd to LM", 0, 0, 0);
    }
    else
    {
      u_ReturnStatus = TRUE;
    }
  }
  else
  {
    MSG_MED("Not able to create ipc msg", 0, 0, 0);
  }

  return u_ReturnStatus;
} /* lm_send_sv_no_exist_cmd */

/*===========================================================================
FUNCTION lm_send_force_dpo_for_power_meas_cmd

DESCRIPTION
  Forces DPO Operation for Power Measurements
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if msg-send was successful, FALSE if otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean lm_send_force_dpo_for_power_meas_cmd(const uint8 u_ForceDpoForPowerMeas )
{
  os_IpcMsgType * p_Msg;
  lm_force_dpo_for_power_meas_cmd_type * pz_ForceDpoForPowerMeasCmd;
  boolean u_ReturnStatus = FALSE;

  /* Dispatch LSM Subsystem requests */
  p_Msg = os_IpcCreate(sizeof(lm_force_dpo_for_power_meas_cmd_type), IPC_ALLOCATION_DYNAMIC,
   (uint32)THREAD_ID_SM_LM);

  if (p_Msg)
  {
    /* Fill and send the IQ Collect Msg to LM */
    p_Msg->q_MsgId = (uint32)C_SM_LM_FORCE_DPO_FOR_POWER_MEAS;

    pz_ForceDpoForPowerMeasCmd = (lm_force_dpo_for_power_meas_cmd_type *)p_Msg->p_Data;
    pz_ForceDpoForPowerMeasCmd->u_ForceDpoForPowerMeas = u_ForceDpoForPowerMeas;

    if (!os_IpcSend(p_Msg, (uint32)THREAD_ID_SM_LM))
    {
      (void)os_IpcDelete(p_Msg);
      MSG_MED("Not able to send Force DPO Power cmd to LM", 0, 0, 0);
    }
    else
    {
      u_ReturnStatus = TRUE;
    }
  }
  else
  {
    MSG_MED("Not able to create ipc msg", 0, 0, 0);
  }

  return u_ReturnStatus;
}

/* Helper functions */
/*
 ******************************************************************************
 * lm_update_session_state 
 *
 * Function description:
 *
 *
 * Parameters: 
 * e_newSessionState - new session state to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_update_session_state(lm_session_state_e_type  e_newSessionState)
{
  z_lmControl.e_sessionState = e_newSessionState;

  LM_MSG_MED("New session state: %d.", z_lmControl.e_sessionState, 0, 0);
}

/*
 ******************************************************************************
 * lm_set_waitingAAData 
 *
 * Function description:
 *
 *
 * Parameters: 
 * u_waitingAAData - new value for waitingAAData variable
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_waitingAAData(boolean  u_waitingAAData)
{
 z_lmControl.u_waitingAAData = u_waitingAAData;
 LM_MSG_MED("Waiting for AA Data = %d",u_waitingAAData,0,0);
}

/*
 ******************************************************************************
 * lm_set_rc_on_threshold_period 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_on_threshold_period(uint32 q_gpsOnThreshold)
{

  if(q_gpsOnThreshold > LM_MAX_RC_ON_THRESHOLD)
  {
    z_lmControl.q_gpsOnThreshold = LM_MAX_RC_ON_THRESHOLD;
    LM_MSG_MED("New RC_ON_THRESHOLD value greater than maximum[%d].",
               LM_MAX_RC_ON_THRESHOLD,0,0);
  }
  else
  {
    z_lmControl.q_gpsOnThreshold = q_gpsOnThreshold;
  }

  LM_MSG_MED("Setting RC_ON_THRESHOLD to %d",z_lmControl.q_gpsOnThreshold,0,0);
}

/*
 ******************************************************************************
 * lm_set_rc_idle_threshold 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_idle_threshold(uint32 q_gpsIdleThreshold)
{

  if(q_gpsIdleThreshold > LM_MAX_RC_IDLE_THRESHOLD)
  {
    z_lmControl.q_gpsIdleThreshold = LM_MAX_RC_IDLE_THRESHOLD;
    LM_MSG_MED("New RC_IDLE_THRESHOLD value greater than maximum[%d].",
               LM_MAX_RC_IDLE_THRESHOLD,0,0);
  }
  else
  {
    z_lmControl.q_gpsIdleThreshold = q_gpsIdleThreshold;
  }
  LM_MSG_MED("Setting RC_IDLE_THRESHOLD to %d",z_lmControl.q_gpsIdleThreshold,0,0);
}

/*
 ******************************************************************************
 * lm_set_rc_shut_off_delay_margin 
 *
 * Function description:
 *
 *
 * Parameters: 
 * the value of parameter LM_SET_PARAM_APP_TRK_DELAY_MARGIN to be set
 *
 * Return value: 
 *
 *
 ******************************************************************************
*/

void lm_set_rc_shut_off_delay_margin(uint32 q_delayMargin)
{

  if(q_delayMargin > LM_MAX_RC_SHUT_OFF_DELAY_MARGIN)
  {
    z_lmControl.q_delayMargin = LM_RC_SHUT_OFF_DELAY_MARGIN_DEFAULT;
    LM_MSG_MED("New RC_SHUT_OFF_DELAY_MARGIN value greater than maximum[%d].",
               LM_MAX_RC_SHUT_OFF_DELAY_MARGIN,0,0);
  }
  else
  {
    z_lmControl.q_delayMargin = q_delayMargin;
  }
  LM_MSG_MED("Setting RC_SHUT_OFF_DELAY_MARGIN to %d",z_lmControl.q_delayMargin,0,0);
}

/*
 ******************************************************************************
 * lm_get_rc_on_threshold_period 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACK_RC_ON_THRESHOLD_PERIOD
 *
 ******************************************************************************
*/

uint32 lm_get_rc_on_threshold_period(void)
{
  
  return z_lmControl.q_gpsOnThreshold;
}

/*
 ******************************************************************************
 * lm_get_rc_idle_threshold 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACKING_RC_IDLE_THRESHOLD
 *
 ******************************************************************************
*/
uint32 lm_get_rc_idle_threshold(void)
{
  
  return z_lmControl.q_gpsIdleThreshold;
}

/*
 ******************************************************************************
 * lm_get_rc_shut_off_delay_margin 
 *
 * Function description:
 *
 *
 * Parameters: 
 * None
 *
 * Return value: 
 *
 * uint32 - returns the value of parameter AAGPS_APP_TRACKING_RC_SHUT_OFF_GRACE_PERIOD
 *
 ******************************************************************************
*/
uint32 lm_get_rc_shut_off_delay_margin(void)
{
  
  return z_lmControl.q_delayMargin;
}

/*
 ******************************************************************************
 * lm_get_utc_leap_secs
 *
 * Function description:
 * Returns the UTC leap seconds
 *
 * Parameters: 
 * None
 *
 * Return value: 
 * utc leap seconds
 *
 ******************************************************************************
*/

uint8 lm_get_utc_leap_secs(void)
{
  return z_lmControl.z_utcData.z_utcInfo.u_DeltaTls;
}
/*
 ******************************************************************************
 * lm_send_xtra_done_msg
 *
 * Function description:
 *   Send XTRA Injection Done IPC to LM
 *
 * Parameters: 
 * None
 *
 * Return value: 
 * None
 *
 ******************************************************************************
*/
void lm_send_xtra_done_msg(void)
{
  os_IpcMsgType* p_ipc_ptr = NULL;  
  MSG_MED("Xtra Injection Completed. Sending IND to LM.",0,0,0);
  p_ipc_ptr = os_IpcCreate(0,
                             IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_LM); 

  if(p_ipc_ptr == NULL)
  {
    MSG_ERROR("Unalbe to create IPC to send to LM",0,0,0);
  }
  else
  {

    p_ipc_ptr->q_MsgId          = (uint32)SM_MSG_ID_LM_MGP_REPORT_XTRA_INJ_DONE;
    p_ipc_ptr->q_DestThreadId   = (uint32)THREAD_ID_SM_LM;
    p_ipc_ptr->q_Size           = 0;

    if(!os_IpcSend(p_ipc_ptr,(uint32)THREAD_ID_SM_LM))
    {
       (void)os_IpcDelete(p_ipc_ptr);
    }
  }

}

