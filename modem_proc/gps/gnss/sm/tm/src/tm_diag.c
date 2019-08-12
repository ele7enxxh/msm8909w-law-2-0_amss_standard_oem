
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Diag Interface

General Description
  This file contains implementations for TM Diag Interface
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_diag.c#1 $  
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
4/17/15    jv      Added QZSS Support
11/20/14   am      Added masking of configurations for TM_DIAG_NAV_CONFIG_CMD
09/01/09   gk      Added a check to validate the directory for EFS operation
07/25/09    jlp    Added diag command to force a dump of various data
                   structures.
06/16/08   jlp     Added handling of SV No Exist list diag command.
03/13/08   jw      Fixed encoding of the gps time for setting PM position using
                   diag.
02/14/08   jw      Support for Optimistic Punc
06/18/07   jd      Implemented diag support for GPS File System support.
05/09/07   jw      Added CGPS Intelliceiver session ctrl command
02/22/07   jw      Removed early hack for measurement combining
02/01/07   cl      Add feature to check if CDMA is supported
01/29/07   jw      Added proper featurization for PPM specific code.
12/20/06   ank     Changes to send PRM and post-fix PPM together.
12/13/06   jw      Added temporary code to set the time_ref of the Post Fix PPMs.
07/12/06   cl      Initail Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <stdlib.h>
#include "diag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "msg.h"

#include "aries_os_api.h"
#include "tm_diag.h"
#include "tm_data.h"
#include "tm_prtl_iface.h"
#include "tm_nmea.h"
#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
#include "tm_is801.h"
#include "tm_is801_util.h"
#endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */
#include "lm_api.h"
#include "pdapibuf.h"
#include "gps_common.h"
#include "tm_common.h"
#ifdef FEATURE_LTE
#include "tm_lpp.h"
#endif
#ifdef FEATURE_CGPS_FILE_SYSTEM
#include "gps_fs_api.h"
#endif /* FEATURE_CGPS_FILE_SYSTEM */

#include "mgp_api.h"
#include "navhw_api.h"
#include "loc_lte_otdoa_gnss_api.h"
#include "lowi_wrapper.h"
#include "gts_loc_api.h"

tm_prtl_cb_s_type          tm_diag_prtl_table;

typedef struct
{
  uint8                             operation;
  tm_nmea_diag_open_port_id_e_type  port_to_open;
} tm_diag_nmea_s_type;
  
#ifdef FEATURE_QDSP6
/* 
   This union helps convert uint32 data coming in diag to float
   Casting the data directly to float results in warnings on gcc
 */
typedef union 
{
  uint32 q_uint32temp;
  FLT    f_flttemp;
} tm_diag_uint_flt_type;
#endif

/* Packet buffer to hold incoming diag pket for asynchronous processing */
/* Make it 1960 bytes */
#define TM_DIAG_PACKET_BUFFER_SIZE  1960
byte tm_diag_pkt_buffer[TM_DIAG_PACKET_BUFFER_SIZE];
uint16 tm_diag_pkt_buffer_length = 0;

tm_sess_req_param_u_type   tm_diag_sess_req_param;
tm_post_data_e_type        tm_diag_post_data_type;
tm_post_data_payload_type  tm_diag_post_data_payload;
lm_request_info_s_type     tm_diag_aiding_data_delete_info;
tm_diag_nmea_s_type        tm_diag_nmea_cmd_info;

boolean tm_diag_in_ftest_mode = FALSE;

/* This flag is used to decide if we should cache PRM */
boolean tm_diag_ppm_in_progress = FALSE;

/* This flag is used to tell us if cached PRM is avaliable getting PPM results */
boolean tm_diag_cached_prm_available = FALSE;

/* This flag is used to tell us if cached PPM is avaliable sending AA to MGP */
boolean tm_diag_cached_ppm_available = FALSE;

/* For cached PRM */
sm_GpsMeasRptStructType tm_diag_cached_prm;
gps_PreFixPpmSructType  tm_diag_cached_ppm;

/* For cached WBIQ info */
sm_ReportWBIQInfoStructType tm_diag_wbiq_info;
/* This is just a counter of the time when wbiq info is updated. */
uint8 tm_diag_wbiq_info_cnt = 0;
#define TM_DIAG_WBIQ_INFO_VERSION (2);

/* Structure to save info for delayed response. Everything else
   for the delayed response is cached in the tm_diag_wbiq_info
   defined above. */
typedef struct{
  boolean v_FactoryIqTestRequested;
  diagpkt_subsys_delayed_rsp_id_type w_DelayedRspId;
  uint8 u_Version;
} tm_diag_factory_iq_test_type;
tm_diag_factory_iq_test_type z_tm_diag_factory_iq_test = {FALSE, 0, 0};

#ifdef FEATURE_CGPS_FILE_SYSTEM
/* Response identifier for the delay response packet for DM Diag */
static diagpkt_subsys_delayed_rsp_id_type gpsdiag_delay_rsp_id;

/* Function Declarations */
PACKED void* gpsdiag_EfsFileReqHandler (PACKED void* p_PktReq, uint16 w_PktLength);
uint32 process_gps_fs_task_EFS_read    (gps_fs_u_cmd_type* p_gpsfsPtr);
#endif /* FEATURE_CGPS_FILE_SYSTEM */

boolean me_DataDump( uint32 q_DumpMask );

/*===========================================================================

FUNCTION tm_diag_send_ipc 

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_send_ipc(uint32 diag_command)
{
  os_IpcMsgType              *ipc_msg_ptr;
  
  ipc_msg_ptr = os_IpcCreate(sizeof(uint32),IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    /* Set the msg id and init the command which failed. */
    ipc_msg_ptr->q_MsgId = TM_CORE_MSG_ID_DIAG;

    *((uint32 *)ipc_msg_ptr->p_Data)= diag_command; /*lint !e826 */

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      (void)os_IpcDelete(ipc_msg_ptr);   
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_ERROR("Failed to create IPC mesage",0,0,0); 
  } /* End ipc_msg_ptr */
}

/*===========================================================================

FUNCTION tm_diag_process_rf_dev_cmd

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_process_rf_dev_cmd(void)
{
  aries_diag_sess_ctrl_req_type *rf_dev_req_ptr = (aries_diag_sess_ctrl_req_type *)tm_diag_pkt_buffer;
  navhw_DiagCmdStruct z_NavHwDiagCmd;
  byte dont_care = 0;
 
  (void) dont_care;
  
  switch(rf_dev_req_ptr->gps_control)
  {

#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)

    case 1: /* Provodie AA */
      MSG_MED("TM-Diag received AA from script", 0, 0, 0);

      /* Log incoming message */
	  if ((rf_dev_req_ptr->data_length <= ARR_SIZE(rf_dev_req_ptr->supporting_data)) && (rf_dev_req_ptr->data_length < IS801_FWD_MSG_LEN))
	  {
	    is801_util_log_flink_msg((void *)&(rf_dev_req_ptr->supporting_data[0]), rf_dev_req_ptr->data_length);
	  }
	  else
	  {
	    MSG_ERROR("Attempting to over flow buffer",0,0,0);
		return;
	  }	
      (void)is801_decode_fwd_link_pddm(TM_PRTL_TYPE_DIAG, (byte *)rf_dev_req_ptr->supporting_data, rf_dev_req_ptr->data_length, &dont_care);
    break;

    case 2: /* Provodie SA */
      MSG_MED("TM-Diag received SA from script", 0, 0, 0);

      /* Log incoming message */
	  if ((rf_dev_req_ptr->data_length <= ARR_SIZE(rf_dev_req_ptr->supporting_data)) && (rf_dev_req_ptr->data_length < IS801_FWD_MSG_LEN))
	  {
        is801_util_log_flink_msg((void *)&(rf_dev_req_ptr->supporting_data[0]), rf_dev_req_ptr->data_length);
	  }
	  else
	  {
	    MSG_ERROR("Attempting to over flow buffer",0,0,0);
		return;
	  }

      (void)is801_decode_fwd_link_pddm(TM_PRTL_TYPE_DIAG, (byte *)rf_dev_req_ptr->supporting_data, rf_dev_req_ptr->data_length, &dont_care);
    break;

    case 5: /* Provodie LR */
      MSG_MED("TM-Diag received LR from script", 0, 0, 0);

      /* Log incoming message */
	  if ((rf_dev_req_ptr->data_length <= ARR_SIZE(rf_dev_req_ptr->supporting_data)) && (rf_dev_req_ptr->data_length < IS801_FWD_MSG_LEN))
	  {
        is801_util_log_flink_msg((void *)&(rf_dev_req_ptr->supporting_data[0]), rf_dev_req_ptr->data_length);
	  }
	  else
	  {
	    MSG_ERROR("Attempting to over flow buffer",0,0,0);
		return;
	  }
      (void)is801_decode_fwd_link_pddm(TM_PRTL_TYPE_DIAG, (byte *)rf_dev_req_ptr->supporting_data, rf_dev_req_ptr->data_length, &dont_care);
    break;

    case 8: /* Provide ALM */
      MSG_MED("TM-Diag received ALM from script", 0, 0, 0);

      /* Log incoming message */
	  if ((rf_dev_req_ptr->data_length <= ARR_SIZE(rf_dev_req_ptr->supporting_data)) && (rf_dev_req_ptr->data_length < IS801_FWD_MSG_LEN))
	  {
        is801_util_log_flink_msg((void *)&(rf_dev_req_ptr->supporting_data[0]), rf_dev_req_ptr->data_length);
	  }
	  else
	  {
	    MSG_ERROR("Attempting to over flow buffer",0,0,0);
		return;
	  }
      (void)is801_decode_fwd_link_pddm(TM_PRTL_TYPE_DIAG, (byte *)rf_dev_req_ptr->supporting_data, rf_dev_req_ptr->data_length, &dont_care);
    break;

    case 9: /* Provide EPH */
      MSG_MED("TM-Diag received EPH from script", 0, 0, 0);

      /* Log incoming message */
	  if ((rf_dev_req_ptr->data_length <= ARR_SIZE(rf_dev_req_ptr->supporting_data)) && (rf_dev_req_ptr->data_length < IS801_FWD_MSG_LEN))
	  {
        is801_util_log_flink_msg((void *)&(rf_dev_req_ptr->supporting_data[0]), rf_dev_req_ptr->data_length);
	  }	
	  else
	  {
	    MSG_ERROR("Attempting to over flow buffer",0,0,0);
	  }
      (void)is801_decode_fwd_link_pddm(TM_PRTL_TYPE_DIAG, (byte *)rf_dev_req_ptr->supporting_data, rf_dev_req_ptr->data_length, &dont_care);
    break;

    case 7: /* start/stop PPM/PRM */
      MSG_MED("TM-Diag received PPM/PRM control command = %x", rf_dev_req_ptr->supporting_data[0], 0, 0);
      
      switch(rf_dev_req_ptr->supporting_data[0])
      {
        case 0x00: /* start PPM */
          /* This flag is used to decide if we should postpone PRM logging */
          tm_diag_ppm_in_progress = TRUE;
          
          /* In FTEST mode, use hardcoded values */
          tm_diag_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_FTM;
          tm_diag_sess_req_param.continue_param.op_req  = TM_OP_REQ_PPM;
          tm_diag_sess_req_param.continue_param.ppm_qos = 0xFFFFFFFF;

          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_CONTINUE,
                            &tm_diag_sess_req_param);
        break;

        case 0x01: /* get PPM result */
          tm_diag_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;

          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_POLLING_DATA,
                            &tm_diag_sess_req_param);
        break;

        case 0x02: /* get PPM result and end PPM */
          /* This flag is used to decide if we should postpone PRM logging */
          tm_diag_ppm_in_progress = FALSE;
          
          tm_diag_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;

          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_POLLING_DATA,
                            &tm_diag_sess_req_param);

          tm_diag_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
          tm_diag_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
          tm_diag_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_PPM;
  
          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_STOP,
                            &tm_diag_sess_req_param);
        break;

        case 0x04: /* GSC init, first cmd of ftest */
         tm_diag_in_ftest_mode = TRUE; /*lint !e506 */

          /* In FTEST mode, use hardcoded values */
          tm_diag_sess_req_param.start_param.op_mode   = TM_SESS_OPERATION_MODE_FTM;
          tm_diag_sess_req_param.start_param.op_req    = TM_OP_REQ_NONE;
          tm_diag_sess_req_param.start_param.num_fixes = 1;
          tm_diag_sess_req_param.start_param.tbf_ms    = 0;
          tm_diag_sess_req_param.start_param.ppm_qos   = 0;
          tm_diag_sess_req_param.start_param.prm_qos   = 0;
          tm_diag_sess_req_param.start_param.lr_qos    = 0;
      tm_diag_sess_req_param.start_param.dynamic_qos_enabled    = TRUE;

          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_START,
                            &tm_diag_sess_req_param);

        break;

        case 0x05: /* End GSC, last cmd of ftest */
          tm_diag_in_ftest_mode = FALSE;
          tm_diag_cached_ppm_available = FALSE;
          tm_diag_cached_prm_available = FALSE;          
          tm_diag_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_SESSION;
          tm_diag_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
          tm_diag_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_NONE;
  
          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_STOP,
                            &tm_diag_sess_req_param);
        break;

        case 0x09: /* End PPM */
          /* This flag is used to decide if we should postpone PRM logging */
          tm_diag_ppm_in_progress = FALSE;
          
          tm_diag_sess_req_param.stop_param.stop_type    = TM_STOP_TYPE_OPERATION;
          tm_diag_sess_req_param.stop_param.stop_reason  = TM_STOP_REASON_COMPLETED;
          tm_diag_sess_req_param.stop_param.op_to_stop   = TM_OP_REQ_PPM;
  
          (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                            TM_SESS_HANDLE_FTEST,
                            TM_SESS_REQ_STOP,
                            &tm_diag_sess_req_param);
        break;

        case 0x25: /* Abort PRM */
        break;

        default:
        break;
      }
    break;

#endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */
  case TM_DIAG_SESS_CGPS_SUB_COMMAND:
    MSG_HIGH("CGPS commands: 0x%x",rf_dev_req_ptr->supporting_data[0],0,0);

    switch(rf_dev_req_ptr->supporting_data[0])
    {
    case TM_DIAG_SESS_CGPS_START_SBAS_TEST_SUB_COMMAND:
      {
      uint32 q_SvMask;
      mgp_ConfigStructType z_OnParams, *p_OnParams = &z_OnParams;

        q_SvMask = rf_dev_req_ptr->supporting_data[1] | (rf_dev_req_ptr->supporting_data[2] << 8) | 
                   (rf_dev_req_ptr->supporting_data[3] << 16) | (rf_dev_req_ptr->supporting_data[4] << 24);
  
        MSG_HIGH("Turning ON SBAS test: SvMask 0x%x",q_SvMask,0,0);

        mgp_SetSbasCannedSvList(q_SvMask);
    
        /* Now turn on MGP */
        p_OnParams->e_OperationMode = MGP_OPERATION_SBAS_TESTMODE;
        p_OnParams->u_SmMeasRptPeriod   = 0;
        p_OnParams->u_EnablePpmRpt      = FALSE; /*lint -e506 */
        p_OnParams->u_EnablePosRpt      = FALSE;
        p_OnParams->u_EnableUtcRpt      = FALSE;
        p_OnParams->u_DataDemodFlag     = TRUE;
        (void)lm_start_sbas_test(p_OnParams);
      }
      break;
    case TM_DIAG_SESS_CGPS_STOP_SBAS_TEST_SUB_COMMAND:
      MSG_HIGH("Turning OFF SBAS test",0,0,0);
      (void)lm_stop_sbas_test();
      break;
    case TM_DIAG_SESS_CGPS_SET_PM_POS_SUB_COMMAND:
      /*
        supporting_data[1] - position type
        supporting_data[2] - lsb lat
        supporting_data[3]
        supporting_data[4]
        supporting_data[5] - msb lat
        supporting_data[6] - lsb lon
        supporting_data[7]
        supporting_data[8]
        supporting_data[9] - msb lon
        supporting_data[10] - lsb punc
        supporting_data[11]
        supporting_data[12]
        supporting_data[13] - msb punc
        supporting_data[14] - lsb wk
        supporting_data[15] - msb wk
        supporting_data[16] - lsb ms
        supporting_data[17]
        supporting_data[18]
        supporting_data[19] - msb ms
     */
      {
        uint8 u_Temp;
        uint16 w_Temp;
        uint32 q_Temp;
  #ifdef FEATURE_QDSP6
  tm_diag_uint_flt_type z_uint_flt_temp;
  #endif

        FLT *p_FltTemp;
        char b_Buffer[200];

        mgp_CellDBPosInfoStruct z_PosInject;

        /* Position Type */
        u_Temp = rf_dev_req_ptr->supporting_data[1];
        z_PosInject.e_PosInfoSrc = (mgp_PosInfoSrcType) u_Temp;

        /* Lat */
  #ifndef FEATURE_QDSP6
        q_Temp = rf_dev_req_ptr->supporting_data[2] | (rf_dev_req_ptr->supporting_data[3] << 8) | 
        (rf_dev_req_ptr->supporting_data[4] << 16) | (rf_dev_req_ptr->supporting_data[5] << 24);
        p_FltTemp = (FLT *)(&q_Temp);
    #else
        z_uint_flt_temp.q_uint32temp = rf_dev_req_ptr->supporting_data[2] | (rf_dev_req_ptr->supporting_data[3] << 8) | 
                (rf_dev_req_ptr->supporting_data[4] << 16) | (rf_dev_req_ptr->supporting_data[5] << 24);
        p_FltTemp = (&z_uint_flt_temp.f_flttemp);
    #endif
        z_PosInject.d_PosLLa[0] = (DBL) (*p_FltTemp);

        /* Lon */
    #ifndef FEATURE_QDSP6
        q_Temp = rf_dev_req_ptr->supporting_data[6] | (rf_dev_req_ptr->supporting_data[7] << 8) | 
                (rf_dev_req_ptr->supporting_data[8] << 16) | (rf_dev_req_ptr->supporting_data[9] << 24);
        p_FltTemp = (FLT *)(&q_Temp);
    #else
        z_uint_flt_temp.q_uint32temp = rf_dev_req_ptr->supporting_data[6] | (rf_dev_req_ptr->supporting_data[7] << 8) | 
                (rf_dev_req_ptr->supporting_data[8] << 16) | (rf_dev_req_ptr->supporting_data[9] << 24);
        p_FltTemp = (&z_uint_flt_temp.f_flttemp);
    #endif

        z_PosInject.d_PosLLa[1] = (DBL) (*p_FltTemp);

        /* Pos Unc */
    #ifndef FEATURE_QDSP6
        q_Temp = rf_dev_req_ptr->supporting_data[10] | (rf_dev_req_ptr->supporting_data[11] << 8) | 
                   (rf_dev_req_ptr->supporting_data[12] << 16) | (rf_dev_req_ptr->supporting_data[13] << 24);
        z_PosInject.f_PosUnc = (FLT) q_Temp;
    #else
        z_uint_flt_temp.q_uint32temp = rf_dev_req_ptr->supporting_data[10] | (rf_dev_req_ptr->supporting_data[11] << 8) | 
                   (rf_dev_req_ptr->supporting_data[12] << 16) | (rf_dev_req_ptr->supporting_data[13] << 24);
        z_PosInject.f_PosUnc = z_uint_flt_temp.f_flttemp ;
    #endif

        /* GPS wk */
        w_Temp = rf_dev_req_ptr->supporting_data[14] | (rf_dev_req_ptr->supporting_data[15] << 8);
        z_PosInject.w_GpsWeek = w_Temp;

        /* GPS ms */
        q_Temp = rf_dev_req_ptr->supporting_data[16] | (rf_dev_req_ptr->supporting_data[17] << 8) | 
                   (rf_dev_req_ptr->supporting_data[18] << 16) | (rf_dev_req_ptr->supporting_data[19] << 24);
        z_PosInject.q_GpsMsec = q_Temp;

        z_PosInject.u_PosInfoValid = TRUE;
        z_PosInject.u_GpsTimeValid = TRUE;
        
        /* Alt unc */
        z_PosInject.f_AltUnc = (FLT)(500.0);
        
        if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_HIGH ) )
        {
          (void) snprintf( b_Buffer, sizeof(b_Buffer), 
                           "Type %d, Wk %d, Ms %lu, Lat %8.6f, Lon %8.6f, Punc %8.2f",
                           z_PosInject.e_PosInfoSrc,
                           z_PosInject.w_GpsWeek,
                           z_PosInject.q_GpsMsec, 
                           z_PosInject.d_PosLLa[0],
                           z_PosInject.d_PosLLa[1],
                           z_PosInject.f_PosUnc
                         );
          MSG_SPRINTF_1( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "%s", b_Buffer);
        }

        /* Convert to Rad */
        z_PosInject.d_PosLLa[0] *= DEG2RAD;
        z_PosInject.d_PosLLa[1] *= DEG2RAD;
        /* Alt */
        z_PosInject.d_PosLLa[2] = (DBL) 0.0;

        mgp_InjectPosEstimate(&z_PosInject, TRUE);
      }
      break;
        case TM_DIAG_SESS_GNSS_NAV_HW_SUB_COMMAND:
          memscpy((void *)&z_NavHwDiagCmd.u_DiagData[0],
		  	     sizeof(z_NavHwDiagCmd.u_DiagData),
                 (void *)&rf_dev_req_ptr->supporting_data[0], 
                 (sizeof(uint8) * TM_DIAG_SUPPORTING_DATA_LENGTH) );
          navhw_ProcessDiagCommands( &z_NavHwDiagCmd ); 
        break;
      } /* rf_dev_req_ptr->supporting_data[0] switch ends */
    break;
#if defined FEATURE_LOC_LTE_OTDOA && !defined (FEATURE_GNSS_LOW_MEMORY)
    case TM_DIAG_SESS_LTE_OTDOA_COMMAND:
    {
      MSG_HIGH("LTE OTDOA DIAG command", 0, 0, 0);

      loc_lte_otdoa_ProcDiagCmd( tm_diag_pkt_buffer, tm_diag_pkt_buffer_length );
      break;
    }
#endif

    case TM_DIAG_SESS_LTE_RF_DEV_COMMAND:
    {
      MSG_HIGH("LTE TTr: Enable RF dev test command %d %d", 
               TM_DIAG_SESS_LTE_RF_DEV_COMMAND, rf_dev_req_ptr->supporting_data[0], 0);

      mgp_ProcLteRfDevCmd( (uint8)rf_dev_req_ptr->supporting_data[0] );
      break;
    }
#if defined (FEATURE_LTE) && !defined (FEATURE_GNSS_LOW_MEMORY)
    case TM_DIAG_SESS_LTE_FORCE_HYBRID_COMMAND:
    {
      MSG_HIGH("LTE OTDOA force hybrid fix command %d %d",
               TM_DIAG_SESS_LTE_FORCE_HYBRID_COMMAND, rf_dev_req_ptr->supporting_data[0], 0);

      tm_lpp_proc_force_lte_hybrid_fix_cmd( (uint8)rf_dev_req_ptr->supporting_data[0] );
      break;
    }
    case TM_DIAG_LOWI_ENABLE_CMD:
    {
      MSG_HIGH("LOWI diag command %d %d",
          TM_DIAG_LOWI_ENABLE_CMD, rf_dev_req_ptr->supporting_data[0], 0);
      switch(rf_dev_req_ptr->supporting_data[0])
      {
      case TM_DIAG_LOWI_INIT_SUB_COMMAND:
        lowi_wrapper_init ();
        break;
      case TM_DIAG_LOWI_DESTROY_SUB_COMMAND:
        lowi_wrapper_destroy ();
        break;
      case TM_DIAG_LOWI_CAPABILITY_SUB_COMMAND:
        lowi_queue_capabilities_req ();
        break;
      case TM_DIAG_LOWI_DISCOVERY_SCAN_SUB_COMMAND:
        if (0 == rf_dev_req_ptr->supporting_data[1])
        {
          lowi_queue_passive_scan_req ();
        }
        else
        {
          lowi_queue_discovery_scan_req (rf_dev_req_ptr->supporting_data[2],
              rf_dev_req_ptr->supporting_data[3],
              rf_dev_req_ptr->supporting_data[4],
              rf_dev_req_ptr->supporting_data[5],
              rf_dev_req_ptr->supporting_data[6]);
        }
        break;
      case TM_DIAG_LOWI_SNOOP_SUBSCRIPTION_SUB_COMMAND:
        lowi_queue_async_discovery_scan_result_req (60, 1);
        break;
      case TM_DIAG_LOWI_SNOOP_SUBSCRIPTION_CANCEL_SUB_COMMAND:
        lowi_queue_async_discovery_scan_result_req (60, 0);
        break;
      case TM_DIAG_LOWI_EVENTS_QUERY_SUB_COMMAND:
        lowi_queue_events_req (1, 0);
        break;
      case TM_DIAG_LOWI_EVENTS_SUBSCRIPTION_SUB_COMMAND:
        lowi_queue_events_req (0, 1);
        break;
      case TM_DIAG_LOWI_EVENTS_SUBSCRIPTION_CANCEL_SUB_COMMAND:
        lowi_queue_events_req (0, 0);
        break;
      default:
        break;
      }
    }
    break;
#endif
    #ifndef FEATURE_GNSS_SA
    #ifdef FEATURE_LOC_GTS_SUPPORT
    case TM_DIAG_SESS_GTS_COMMAND:
    {
      MSG_HIGH("GTS Commands %d %d",
               TM_DIAG_SESS_GTS_COMMAND, rf_dev_req_ptr->supporting_data[0], 0);

      gts_ProcDiagCmd( (uint8 *)&rf_dev_req_ptr->supporting_data[0],TM_DIAG_SUPPORTING_DATA_LENGTH );
      break;
    } 
    #endif /* FEATURE_LOC_GTS_SUPPORT */
    #endif /* ! FEATURE_GNSS_SA */
    default:
    {
      ERR("Unknown RF Dev DIAG command %d", rf_dev_req_ptr->gps_control, 0, 0);
    }    
    break;    
  }
}

/*===========================================================================

FUNCTION tm_diag_msg_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_msg_handler(const void *data)
{
  uint32 diag_cmd;

  
  if(data != NULL)
  {
    diag_cmd = *((uint32 *)data);

    switch(diag_cmd)
    {
      case TM_DIAG_SESS_REQ_RF_DEV_COMMAND:
        tm_diag_process_rf_dev_cmd();
      break;
      
      case TM_DIAG_SESS_REQ_START_COMMAND:
        
        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_START,
                       &tm_diag_sess_req_param))
        {
          tm_diag_in_ftest_mode = TRUE; /*lint !e506 */
          MSG_ERROR("Start Sess Req Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_ERROR("Start Sess Req Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_REQ_STOP_COMMAND:
        
        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_STOP,
                       &tm_diag_sess_req_param))
        {
          tm_diag_in_ftest_mode = FALSE;                
          MSG_ERROR("Stop Sess Req Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_ERROR("Stop Sess Req Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_REQ_CONTINUE_COMMAND:

        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_CONTINUE,
                       &tm_diag_sess_req_param))
        {
          MSG_ERROR("Continue Sess Req Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_ERROR("Continue Sess Req Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_POST_DATA_COMMAND:
                   
        if(tm_post_data(TM_PRTL_TYPE_DIAG,
                        TM_SESS_HANDLE_FTEST,
                        tm_diag_post_data_type,
                        &tm_diag_post_data_payload))
        {
          MSG_ERROR("Post Data Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_ERROR("Posd Data Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_POLLING_PPM_COMMAND:
                   
        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_POLLING_DATA,
                       &tm_diag_sess_req_param))
        {
          MSG_ERROR("Polling PPM Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_ERROR("Polling PPM Failed", 0, 0, 0);
        }

      break;

      case TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND:
        tm_diag_aiding_data_delete_info.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
        tm_diag_aiding_data_delete_info.z_request.z_delete_request.delete_all = TRUE;

        /* Deletes GPS and Glonass data */
        tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

        (void)lm_request(&tm_diag_aiding_data_delete_info);
      break;

      case TM_DIAG_PORT_CONTROL_COMMAND:
        if(tm_diag_nmea_cmd_info.operation == 1)
        {
          MSG_HIGH("Trying to open NMEA port %d", tm_diag_nmea_cmd_info.port_to_open, 0, 0);
          tm_nmea_diag_open_port(tm_diag_nmea_cmd_info.port_to_open);
        }
        else if(tm_diag_nmea_cmd_info.operation == 0)
        {
          MSG_HIGH("Closing NMEA port", 0, 0, 0);
          tm_nmea_diag_close_port(NULL);
        }
      break;
      
      default:
      break;      
    }
  }
}

/*===========================================================================

FUNCTION tm_diag_sess_start_req_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_start_req_cmd_handler (
                                                   PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                   word pkt_len               /* length of request packet   */
                                                 )
{
  tm_diag_sess_start_req_pkt_type *cmd_ptr = (tm_diag_sess_start_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_start_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_sess_req_param.start_param.op_mode   = (tm_sess_operation_mode_e_type)cmd_ptr->operation_mode;
  tm_diag_sess_req_param.start_param.op_req    = cmd_ptr->req_op;
  tm_diag_sess_req_param.start_param.num_fixes = cmd_ptr->num_fixes;
  tm_diag_sess_req_param.start_param.tbf_ms    = cmd_ptr->time_between_fixes * 1000;
  tm_diag_sess_req_param.start_param.ppm_qos   = cmd_ptr->ppm_qos;
  tm_diag_sess_req_param.start_param.prm_qos   = cmd_ptr->prm_qos;  
  tm_diag_sess_req_param.start_param.lr_qos    = cmd_ptr->lr_qos;    
  tm_diag_sess_req_param.start_param.accuracy_threshold = cmd_ptr->accuracy_threshold;   
  tm_diag_sess_req_param.start_param.dynamic_qos_enabled = TRUE;   

  rsp_ptr = (tm_diag_sess_start_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_START_COMMAND,
                                  sizeof (tm_diag_sess_start_rsp_pkt_type) );
  
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_START_COMMAND);
 
  return rsp_ptr;
}/*end tmDiag_SessStartReqCmdHandler*/

/*===========================================================================

FUNCTION tm_diag_sess_stop_req_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_stop_req_cmd_handler (
                                                   PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                   word pkt_len               /* length of request packet   */
                                                )
{
  tm_diag_sess_stop_req_pkt_type *cmd_ptr = (tm_diag_sess_stop_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_stop_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_sess_req_param.stop_param.stop_type    = (tm_sess_stop_e_type)cmd_ptr->stop_type;
  tm_diag_sess_req_param.stop_param.stop_reason  = (tm_sess_stop_reason_e_type)cmd_ptr->stop_reason;
  tm_diag_sess_req_param.stop_param.op_to_stop   = cmd_ptr->op_to_stop;
   
  rsp_ptr = (tm_diag_sess_stop_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_STOP_COMMAND,
                                  sizeof (tm_diag_sess_stop_rsp_pkt_type) );
  
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_STOP_COMMAND);   
  
  return rsp_ptr;
}/*end tmDiag_SessStopReqCmdHandler*/


/*===========================================================================

FUNCTION tm_diag_sess_continue_req_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_continue_req_cmd_handler (
                                                      PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                      word pkt_len               /* length of request packet   */
                                                    )
{
  tm_diag_sess_continue_req_pkt_type *cmd_ptr = (tm_diag_sess_continue_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_continue_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_sess_req_param.continue_param.op_req    = cmd_ptr->req_op;
  tm_diag_sess_req_param.continue_param.ppm_qos   = cmd_ptr->ppm_qos;
  tm_diag_sess_req_param.continue_param.prm_qos   = cmd_ptr->prm_qos;  
  tm_diag_sess_req_param.continue_param.lr_qos    = cmd_ptr->lr_qos;      
  tm_diag_sess_req_param.continue_param.dynamic_qos_enabled    = TRUE;      
  tm_diag_sess_req_param.start_param.accuracy_threshold = cmd_ptr->accuracy_threshold;   
  tm_diag_sess_req_param.start_param.dynamic_qos_enabled = TRUE;   
  
  rsp_ptr = (tm_diag_sess_continue_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_CONTINUE_COMMAND,
                                  sizeof (tm_diag_sess_continue_rsp_pkt_type) );
  
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_CONTINUE_COMMAND);
  
  return rsp_ptr;
}/*end tmDiag_SessContinueReqCmdHandler*/

/*===========================================================================

FUNCTION tm_diag_sess_post_data_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_post_data_cmd_handler (
                                                   PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                   word pkt_len               /* length of request packet   */
                                                 )
{
  tm_diag_sess_post_data_req_pkt_type *cmd_ptr = (tm_diag_sess_post_data_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_post_data_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_post_data_type = (tm_post_data_e_type)cmd_ptr->data_type;
   
  rsp_ptr = (tm_diag_sess_post_data_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_POST_DATA_COMMAND,
                                  sizeof (tm_diag_sess_post_data_rsp_pkt_type));

  tm_diag_send_ipc(TM_DIAG_SESS_POST_DATA_COMMAND);

  return rsp_ptr;
}/*end tmDiag_SessPostDataCmdHandler*/


/*===========================================================================

FUNCTION tm_diag_sess_polling_ppm_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_polling_ppm_cmd_handler (
                                                     PACKED void *req_pkt_ptr,   /* pointer to request packet  */
                                                     word  pkt_len               /* length of request packet   */
                                                   )
{  
  tm_diag_sess_polling_ppm_rsp_pkt_type *rsp_ptr = NULL;

  (void)req_pkt_ptr;
  (void)pkt_len;  
  
  tm_diag_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;
 
  rsp_ptr = (tm_diag_sess_polling_ppm_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_POLLING_PPM_COMMAND,
                                  sizeof (tm_diag_sess_polling_ppm_rsp_pkt_type));

  tm_diag_send_ipc(TM_DIAG_SESS_POLLING_PPM_COMMAND);

  return rsp_ptr;
}/*end tmDiag_SessPollingPPMCmdHandler*/ 


/*===========================================================================

FUNCTION tm_diag_sess_del_aiding_data_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_del_aiding_data_cmd_handler (
                                                         PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                         word  pkt_len              /* length of request packet   */
                                                       )
{
  tm_diag_delete_aiding_data_req_pkt_type *cmd_ptr = (tm_diag_delete_aiding_data_req_pkt_type *) req_pkt_ptr;
  tm_diag_delete_aiding_data_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask    = cmd_ptr->del_clock_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeletePosMask      = cmd_ptr->del_pos_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteEphMask      = cmd_ptr->del_eph_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteAlmMask      = cmd_ptr->del_alm_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteIonoMask     = cmd_ptr->del_iono_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask    = cmd_ptr->del_sv_dir_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask  = cmd_ptr->del_sv_steer_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvHealthMask = cmd_ptr->del_sv_health_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteSaDataMask   = cmd_ptr->del_aa_data_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteRtiMask      = cmd_ptr->del_rti_mask;

  rsp_ptr = (tm_diag_delete_aiding_data_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND,
                                  sizeof (tm_diag_delete_aiding_data_rsp_pkt_type));

  tm_diag_send_ipc(TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND);

  return rsp_ptr;
}/*end tmDiag_SessDelAidingDataCmdHandler*/

/*===========================================================================

FUNCTION tm_diag_sess_req_sw_ver_cmd_handler            

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_req_sw_ver_cmd_handler (
                                                         PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                         word  pkt_len              /* length of request packet   */
                                                       )
{
  tm_diag_sess_req_sw_ver_rsp_pkt_type *rsp_ptr = NULL;

  (void)req_pkt_ptr;
  (void)pkt_len;
  
  rsp_ptr = (tm_diag_sess_req_sw_ver_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_SW_VER_COMMAND,
                                  sizeof (tm_diag_sess_req_sw_ver_rsp_pkt_type));

  if(rsp_ptr != NULL)
  {
    rsp_ptr->pkt_ver = 0;
    rsp_ptr->cgps_major_sw_version = 1;
    rsp_ptr->cgps_minor_sw_version = 0;
    rsp_ptr->cgps_build_sw_version = 1;
    rsp_ptr->cgps_comm_state = 0;
    rsp_ptr->cgps_sess_state = 0;
    rsp_ptr->cgps_wireless_mode = 0;
    rsp_ptr->cgps_pdapi_rev_id = 0;
  }

  return rsp_ptr;
}/*end tm_diag_sess_req_sw_ver_cmd_handler*/



/*===========================================================================
FUNCTION tm_diag_sarf_cmd_handler

DESCRIPTION
  Handles the group of commands that execute in Standalone-RF mode (MGP).
  SingleSv_Standalone is a lone exception. This can run even when MGP is
  NOT in this mode. Mode_switch cmd triggers MGP to switch to this mode.

  All four cmds that this function receives has a version number followed by
  payload. We take adv of this fact by extracting just the payload and sending
  it to LM. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* tm_diag_sarf_cmd_handler (PACKED void* p_PktReq, uint16 w_PktLength)
{
  tm_diag_sarf_cmds_common_req_pkt_type* p_Req = (tm_diag_sarf_cmds_common_req_pkt_type*) p_PktReq;
  tm_diag_sarf_common_rsp_pkt_type* p_CommonRsp = NULL;
  tm_diag_sarf_rfdevfeature_rsp_pkt_type *p_RfDevRsp = NULL;
  tm_diag_sarf_wbiq_info_rsp_pkt_type *p_RFDevWbiqRsp = NULL;
  tm_diag_lm_msg_id_type lm_dstn_ipc_msg_id;


  /* Function parameters Sanity Check */
  if (p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  if( p_Req->z_DiagCmdCommon.u_GpsCmdCode >= TM_DIAG_GPS_CMD_CODE_LAST )
  {
    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, p_PktReq, w_PktLength));
  }

  /* Get diag cmd id */
  lm_dstn_ipc_msg_id = (tm_diag_lm_msg_id_type) 
  (((uint32) p_Req->z_DiagCmdCommon.u_GpsCmdCode - (uint32)TM_DIAG_GPS_CMD_CODE_START) 
                                                + (uint32)ARIES_MSG_ID_TM_LM_FIRST + 1);

  if (lm_dstn_ipc_msg_id == ARIES_MSG_ID_TM_LM_RF_DEV_FEATURE_CAPABILITIES)
  {
    /* Allocate space through diag for response packet */
    p_RfDevRsp = (tm_diag_sarf_rfdevfeature_rsp_pkt_type*)
             diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_SESS_SARF_COMMAND,
                                   sizeof(tm_diag_sarf_rfdevfeature_rsp_pkt_type) );

    if(p_RfDevRsp != NULL)
    {
      p_RfDevRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
      p_RfDevRsp->q_RfDevFeatureEnum = mgp_GetRfDevFeatureNum();
    }

    return p_RfDevRsp;

  }
  else if (lm_dstn_ipc_msg_id == ARIES_MSG_ID_TM_LM_GET_LATEST_WB_FFT_RESULTS)
  {
    /* Allocate space through diag for response packet */
    p_RFDevWbiqRsp = (tm_diag_sarf_wbiq_info_rsp_pkt_type*)
             diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_SESS_SARF_COMMAND,
                                   sizeof(tm_diag_sarf_wbiq_info_rsp_pkt_type) );

    if(p_RFDevWbiqRsp != NULL)
    {
      p_RFDevWbiqRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
      p_RFDevWbiqRsp->u_Ver = TM_DIAG_WBIQ_INFO_VERSION;
      p_RFDevWbiqRsp->u_Cnt = tm_diag_wbiq_info_cnt;
      p_RFDevWbiqRsp->q_GpsCnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_GpsCNoDBHz);
      p_RFDevWbiqRsp->l_GpsFreqHz = (int32)tm_diag_wbiq_info.f_GpsFreqHz; 
      p_RFDevWbiqRsp->q_GloCnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_GloCNoDBHz);
      p_RFDevWbiqRsp->l_GloFreqHz = (int32)tm_diag_wbiq_info.f_GloFreqHz; 
      p_RFDevWbiqRsp->q_BdsCnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_BdsCNoDBHz);
      p_RFDevWbiqRsp->l_BdsFreqHz = (int32)tm_diag_wbiq_info.f_BdsFreqHz; 
      p_RFDevWbiqRsp->q_GalCnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_GalCNoDBHz);
      p_RFDevWbiqRsp->l_GalFreqHz = (int32)tm_diag_wbiq_info.f_GalFreqHz;
      p_RFDevWbiqRsp->u_GnssConfig = tm_diag_wbiq_info.u_GnssConfig;
      p_RFDevWbiqRsp->l_AdcMeanIp1mV = tm_diag_wbiq_info.l_AdcMeanIp1mV;
      p_RFDevWbiqRsp->l_AdcMeanQp1mV = tm_diag_wbiq_info.l_AdcMeanQp1mV;
      p_RFDevWbiqRsp->q_AdcAmpIp1mV = tm_diag_wbiq_info.q_AdcAmpIp1mV;
      p_RFDevWbiqRsp->q_AdcAmpQp1mV = tm_diag_wbiq_info.q_AdcAmpQp1mV; 
    }

    return p_RFDevWbiqRsp;

  }
  else
  {
    /* Allocate space through diag for response packet */
    p_CommonRsp = (tm_diag_sarf_common_rsp_pkt_type*)
             diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_SESS_SARF_COMMAND,
                                   sizeof(tm_diag_sarf_common_rsp_pkt_type) );
  
    if(p_CommonRsp != NULL)
    {
      p_CommonRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
      p_CommonRsp->u_Status = (uint8) lm_send_sarf_cmd(&p_Req->u_data[0], lm_dstn_ipc_msg_id);
    }

    return p_CommonRsp;
  }



} /* gpsdiag_RfVerifCmdHandler */

/*===========================================================================

FUNCTION tm_diag_port_control_cmd_handler

DESCRIPTION
  This function handles port control command

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_port_control_cmd_handler (
                                                 PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                 word pkt_len               /* length of request packet   */
                                               )
{
  tm_diag_port_control_req_pkt_type *cmd_ptr = (tm_diag_port_control_req_pkt_type *) req_pkt_ptr;
  tm_diag_port_control_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;

  rsp_ptr = (tm_diag_port_control_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_PORT_CONTROL_COMMAND,
                                  sizeof (tm_diag_port_control_rsp_pkt_type) );

  if(cmd_ptr != NULL)
  {
    /* Service type 0 => NMEA */
    if(cmd_ptr->service_type == 0)
    {
      MSG_HIGH("Received NMEA Port Control Command", 0, 0, 0);
      tm_diag_nmea_cmd_info.operation     = cmd_ptr->operation_type;
      tm_diag_nmea_cmd_info.port_to_open  = (tm_nmea_diag_open_port_id_e_type)cmd_ptr->port;
      tm_diag_send_ipc(TM_DIAG_PORT_CONTROL_COMMAND);    
    }
    else if(cmd_ptr->service_type == 1)  /* Service type 1 => Serial PD API */
    {
      MSG_HIGH("Received Serial PD API Port Control Command", 0, 0, 0);
    }

    if(rsp_ptr != NULL)
    {
      rsp_ptr->result = 1;
    }    
  }
  else
  {
    if(rsp_ptr != NULL)
    {
      rsp_ptr->result = 0;
    }  
  }

  return rsp_ptr;
}/*end tm_diag_port_control_cmd_handler*/

#ifdef FEATURE_CGPS_FILE_SYSTEM
/*===========================================================================
FUNCTION gpsdiag_EfsFileRespHandler

DESCRIPTION
  This function is called by the GPS_FS task to send back the result of 
  file operation (write/append/delete) performed. This function updates
  V2 delayed response.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void gpsdiag_EfsFileRespHandler (uint32 q_FileOperationResult)
{
  gpsdiag_EfsFileSysRspType *delayed_rsp_ptr;
  const int rsp_len = sizeof(gpsdiag_EfsFileSysRspType);
  
  delayed_rsp_ptr = (gpsdiag_EfsFileSysRspType*)diagpkt_subsys_alloc_v2_delay(
                (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS, 
                CGPS_EFS_FILE_HANDLING_F,
                gpsdiag_delay_rsp_id, rsp_len );
  if (delayed_rsp_ptr != NULL)
  {
    /* sets the response count value */
    diagpkt_subsys_set_rsp_cnt(delayed_rsp_ptr, 1);

    /* Set the status in the response */
    diagpkt_subsys_set_status(delayed_rsp_ptr, q_FileOperationResult);
    diagpkt_delay_commit( delayed_rsp_ptr );
  }
  else
  {
    MSG_ERROR("EfsFileRespHandler - failed allocating Resp buffer", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION gpsdiag_EfsFileReqHandler

DESCRIPTION
  This function is the handler for packets received by the MSM destined for
  DIAG_SUBSYS_GPS with command code 24, which is used to perform EFS file 
  operations. This functions performs preliminary sanity check on request 
  packet received and then builds an IPC message to be sent to the GPS_FS 
  task if file operation is write/append or delete. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void *gpsdiag_EfsFileReqHandler
(
    PACKED void* p_PktReq,/* pointer to valid request packet */
    uint16 w_PktLength       /* length of the request packet */
)
{
  gpsdiag_EfsFileSysReqType *req_ptr = (gpsdiag_EfsFileSysReqType *)p_PktReq;
  gpsdiag_EfsFileSysRspType *imm_rsp_ptr;
  uint8 u_FileNameLen;
  uint8 u_FileData[C_GPSDIAG_EFS_MAX_PAYLOAD];
  uint16 w_ExpectedLen;
  uint32 q_Result, q_I, q_J;
  /* kk is a variable to make KlockWork happy, it is stupid and needed to make Klockwork happy */
  int32 kk = 0;
  gps_fs_u_cmd_type  z_GpsFs;
  char DirectoryAndFileName[ C_GPSDIAG_MAX_FULL_FILENAME_LENGTH ];
  os_IpcMsgType* p_Msg;
  
  const int rsp_len = sizeof(gpsdiag_EfsFileSysRspType);
  w_ExpectedLen = sizeof(gpsdiag_EfsFileSysReqType);
  
  /* check for valid request packet length */
  if( w_PktLength <= w_ExpectedLen )
  {
    /* Check if we exceed Filename length bounds */
    if( req_ptr->u_FullPathAndFileNameLength > 
                (C_GPSDIAG_MAX_FULL_FILENAME_LENGTH - 1) )
    {
      q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_FILENAME_LENGTH;
    }
    else
    {
      if( (w_PktLength - C_GPSDIAG_EFS_FILE_HANDLING_FIXED_FIELD_SIZE) != 
          (req_ptr->u_FullPathAndFileNameLength + req_ptr->q_FileSize) )
      {
        q_Result = GPSDIAG_EFS_FILE_REQ_FAIL_PKT_DATA_SIZE_MISMATCH;
      }
      else
      {
        /* Check if u_FileOperation is valid */
        if( req_ptr->u_FileOperation > GPS_FS_READ_FILE )
        {
          q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_OPERATION;
        }
        else
        {
          /* Make sure file size doesn't exceed C_GPSDIAG_EFS_MAX_PAYLOAD */
          if( ((req_ptr->u_FileOperation == GPS_FS_CREATE_WRITE_FILE) ||
              (req_ptr->u_FileOperation == GPS_FS_APPEND_FILE)) &&
              (req_ptr->q_FileSize > C_GPSDIAG_EFS_MAX_PAYLOAD)
          )
          {
            q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_FILE_SIZE;
          }
          else
          {
            /* Separate filename and file data from incoming file payload */
            for( q_I = 0; q_I < (req_ptr->u_FullPathAndFileNameLength); q_I++ )
            {
              DirectoryAndFileName[q_I] = req_ptr->FilePayload[q_I];
            }
            DirectoryAndFileName[q_I] = '\0';
            q_J = q_I;
            if( (req_ptr->u_FileOperation == GPS_FS_CREATE_WRITE_FILE) ||
                (req_ptr->u_FileOperation == GPS_FS_APPEND_FILE) )
            {
              for (q_I = q_J; q_I < (w_PktLength - C_GPSDIAG_EFS_FILE_HANDLING_FIXED_FIELD_SIZE); q_I++)
              {
                if((q_I - q_J) < C_GPSDIAG_EFS_MAX_PAYLOAD)
                {
                  kk = (int32) (q_I - q_J);
                  if(kk >= 0)
                  {
                    u_FileData[kk] = (uint8) req_ptr->FilePayload[q_I];
                  }
                }
              }
            }
            /* Make sure only Valid GPS files are accessed */
            if( tm_util_efs_dir_and_file_name_is_valid(&DirectoryAndFileName[0], 
                                                        req_ptr->u_FileOperation,
                                                        req_ptr->q_FileSize))
            {
              /* Preliminary sanity checks are done. If there are any more
               * sanity checks, they are done in GPS_FS task. 
               */
              switch ( req_ptr->u_FileOperation )
              {
              case GPS_FS_CREATE_WRITE_FILE:
              case GPS_FS_DELETE_FILE:
              {
                memset (&z_GpsFs, 0, sizeof(z_GpsFs));
                z_GpsFs.u_FileOP = req_ptr->u_FileOperation;
                z_GpsFs.u_FileNameLen = req_ptr->u_FullPathAndFileNameLength + 1;
                for( q_I = 0; q_I < C_GPSDIAG_EFS_FILE_HANDLING_RESERVED_BYTES; q_I++ )
                {
                  z_GpsFs.u_Reserved[q_I] = req_ptr->u_Reserved[q_I];
                }
                z_GpsFs.q_DataSize = req_ptr->q_FileSize;

                if ( z_GpsFs.u_FileNameLen > GPS_FS_MAX_PATH_LEN )
                {
                  u_FileNameLen = GPS_FS_MAX_PATH_LEN;
                }
                else
                {
                  u_FileNameLen = z_GpsFs.u_FileNameLen;
                }
                for( q_I = 0; q_I < u_FileNameLen; q_I++)
                {
                  z_GpsFs.p_FileName[q_I] = DirectoryAndFileName[q_I];
                }
                if( (req_ptr->u_FileOperation == GPS_FS_CREATE_WRITE_FILE) ||
                    (req_ptr->u_FileOperation == GPS_FS_APPEND_FILE) )
                {
                  z_GpsFs.p_DataPtr = (void *)&u_FileData[0];
                }
                else
                {
                  z_GpsFs.p_DataPtr = NULL;
                }
                z_GpsFs.gps_fs_efs_status_get = gpsdiag_EfsFileRespHandler; 

                p_Msg = os_IpcCreate(sizeof(gps_fs_u_cmd_type), IPC_ALLOCATION_DYNAMIC, THREAD_ID_GPS_FS);
                if (p_Msg)
                {
                  p_Msg->q_MsgId = C_GPS_FS_MSG_DIAG_CMD;
                  memscpy((gps_fs_u_cmd_type *)p_Msg->p_Data, sizeof(z_GpsFs), &z_GpsFs, sizeof(z_GpsFs));
                  if (!os_IpcSend(p_Msg, THREAD_ID_GPS_FS))
                  {
                    (void) os_IpcDelete(p_Msg);
                    q_Result = GPSDIAG_EFS_FILE_REQ_CMD_BUF_ALLOC_FAIL;
                  }
                  else
                  {
                    q_Result = GPSDIAG_EFS_FILE_REQ_SUCCESS;
                  }
                }
                else
                {
                  q_Result = GPSDIAG_EFS_FILE_REQ_CMD_BUF_ALLOC_FAIL;
                }
                break;
              }

              case GPS_FS_READ_FILE:
              {
                q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_OPERATION;
                break;
              }

              case GPS_FS_APPEND_FILE:
              {        
                q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_OPERATION;
                break;
              }

              default:
                q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_OPERATION;
                break;
              }
            }
            else
            {
              q_Result = GPSDIAG_EFS_FILE_REQ_INVALID_DIR;
            }
          }
        }
      }
    }
  }
  else
  {
    q_Result = GPSDIAG_EFS_FILE_REQ_FAIL_PKT_LENGTH;  
  }

  /* Allocate the memory for sending immediate response */
  imm_rsp_ptr = (gpsdiag_EfsFileSysRspType*)diagpkt_subsys_alloc_v2(
                (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS, 
                CGPS_EFS_FILE_HANDLING_F,
                rsp_len );
  if (imm_rsp_ptr == NULL)
  {
    return NULL;
  }
  
  /* Set the status in the response */
  diagpkt_subsys_set_status(imm_rsp_ptr, q_Result);
  
  if( q_Result == GPSDIAG_EFS_FILE_REQ_SUCCESS )
  {
    /* Save the packet id for later use in the delay respose */
    gpsdiag_delay_rsp_id = diagpkt_subsys_get_delayed_rsp_id(imm_rsp_ptr);

    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(imm_rsp_ptr, 0);
  } 
  else
  {
    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(imm_rsp_ptr);
  } 
  return imm_rsp_ptr;
}

/*
 ******************************************************************************
 * process_gps_fs_task_EFS_read
 *
 * Function description:
 *   This function  processes EFS reads of GPS FS task.
 * 
 * Parameters: 
 *    GPS FS task message.
 *
 * Return value: 
 *   None
 *
 ******************************************************************************
*/
uint32 process_gps_fs_task_EFS_read(gps_fs_u_cmd_type *p_gpsfsPtr )
{
  return 0;
}
#endif /* FEATURE_CGPS_FILE_SYSTEM */

/*===========================================================================

FUNCTION tm_diag_sv_no_exist_cmd_handler

DESCRIPTION
  This procedure receives Diag packets from AriesDiag
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sv_no_exist_cmd_handler
(
  PACKED void * p_PktReq,    /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  const uint32 cq_MaxSvNoExist = 8;
  const uint16 cw_ExpectedLen = sizeof(gpsdiag_SvNoExistReqPktType);
  const uint16 cw_RspLen = sizeof(gpsdiag_SvNoExistRspPktType);

  gpsdiag_SvNoExistRspPktType * pz_Rsp;
  gpsdiag_SvNoExistReqPktType * pz_Req
   = (gpsdiag_SvNoExistReqPktType *)p_PktReq;

  uint32 status = GPSDIAG_NOEXIST_STATUS_FAIL;

  if(!p_PktReq)
  {
    return NULL;
  }

  /* check for valid request packet length */
  if(pz_Req && pkt_len == cw_ExpectedLen)
  {
    /* Validate the parameters in the message */
    if(pz_Req->u_Action != GPSDIAG_NOEXIST_ACTION_SET_TO_MASK)
    {
      status = GPSDIAG_NOEXIST_STATUS_INVALID_ACTION;
    }
    else
    {
      /* Make sure there are not too many Svs in the no exist list */
      uint32 q_NoExistList = pz_Req->q_SvNoExistMask;
      uint32 q_Count;

      for(q_Count = 0; q_NoExistList && q_Count < cq_MaxSvNoExist;
       q_NoExistList >>= 1)
      {
        if(q_NoExistList & 0x00000001)
        {
          q_Count++;
        }
      }

      if(q_Count < cq_MaxSvNoExist)
      {
        status = GPSDIAG_NOEXIST_STATUS_SUCCESS;
      }
      else
      {
        status = GPSDIAG_NOEXIST_STATUS_TOO_MANY_SVS;
      }
    }
  }
  else
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, pkt_len));
  }

  if(status == GPSDIAG_NOEXIST_STATUS_SUCCESS)
  {
    (void) lm_send_sv_no_exist_cmd(pz_Req->q_SvNoExistMask,
     (sv_no_exist_action_type)(pz_Req->u_Action));
  }

  /*
     Send the response.  In the first implementation, the response is
     immediate, but a v2 delay diag pkt is used to make the functionality
     more extendable later on.
  */
  pz_Rsp = (gpsdiag_SvNoExistRspPktType *)diagpkt_subsys_alloc_v2(
   (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS, TM_DIAG_SV_NO_EXIST_CMD, cw_RspLen);

  if(pz_Rsp != NULL)
  {
    /* Set the status in the response */
    diagpkt_subsys_set_status(pz_Rsp, status);
  
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(pz_Rsp, 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(pz_Rsp);
  }
  
  return pz_Rsp;
} /* tm_diag_sv_no_exist_cmd_handler */

/*===========================================================================

FUNCTION tm_diag_force_dpo_for_power_meas_cmd_handler

DESCRIPTION
  This procedure receives Diag packets from AriesDiag.
  The following is handler forces DPO for Power measurements
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_force_dpo_for_power_meas_cmd_handler
(
  PACKED void * p_PktReq,    /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  const uint16 cw_ExpectedLen = sizeof(gpsdiag_ForceDpoForPowerMeasReqPktType);
  const uint16 cw_RspLen = sizeof(gpsdiag_ForceDpoForPowerMeasRspPktType);

  gpsdiag_ForceDpoForPowerMeasRspPktType * pz_Rsp;
  gpsdiag_ForceDpoForPowerMeasReqPktType * pz_Req
   = (gpsdiag_ForceDpoForPowerMeasReqPktType *)p_PktReq;

  uint32 u_Status = 0;

  if(!p_PktReq)
  {
    return NULL;
  }

  /* check for valid request packet length */
  if( !pz_Req || (pkt_len != cw_ExpectedLen) )
  {
    /* Command not valid */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, pkt_len));
  }

  if(u_Status == 0)
  {
    (void) lm_send_force_dpo_for_power_meas_cmd(pz_Req->u_ForceDpoForPowerMeas);
  }

  /*
     Send the response.  In the first implementation, the response is
     immediate, but a v2 delay diag pkt is used to make the functionality
     more extendable later on.
  */
  pz_Rsp = (gpsdiag_ForceDpoForPowerMeasRspPktType *)diagpkt_subsys_alloc_v2(
   (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS, TM_DIAG_FORCE_DPO_FOR_POWER_MEAS_CMD, cw_RspLen);

  if(pz_Rsp != NULL)
  {
    /* Set the status in the response */
    diagpkt_subsys_set_status(pz_Rsp, u_Status);
  
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(pz_Rsp, 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(pz_Rsp);
  }
  
  return pz_Rsp;
} /* tm_diag_force_dpo_for_power_meas_cmd_handler */

/*===========================================================================
FUNCTION tm_diag_data_dump_cmd_handler

DESCRIPTION
  This procedure handles the diag command to dump various data for debugging
  purposes.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* tm_diag_data_dump_cmd_handler(PACKED void * p_PktReq,
 word w_PktLength)
{
  gpsdiag_DataDumpReqType * pz_Req = (gpsdiag_DataDumpReqType *)p_PktReq;
  gpsdiag_DataDumpRspType * pz_Rsp = NULL;
  boolean u_Status = 1;

  if(pz_Req == NULL)
  {
    return NULL;
  }

  if(me_DataDump(pz_Req->q_ReqDataBitMask))
  {
    /* 0 indicates success */
    u_Status = 0;
  }

  /* Allocate space through diag for response packet */
  pz_Rsp = (gpsdiag_DataDumpRspType *) 
   diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
   TM_DIAG_DATA_DUMP_CMD, sizeof(gpsdiag_DataDumpRspType));

  if(pz_Rsp != NULL)
  {
    /* Set the status in the response */
    diagpkt_subsys_set_status(&(pz_Rsp->z_DiagCmdHdrV2), u_Status);
  
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(pz_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(pz_Rsp->z_DiagCmdHdrV2));
  }
  else
  {
    MSG_ERROR("gpsdiag_DataDumpCmdHandler: Rsp packet is NULL!!", 0, 0, 0);
  }

  return pz_Rsp;
} /* gpsdiag_DataDumpCmdHandler */

/*===========================================================================

FUNCTION tm_diag_forward_pkt

DESCRIPTION
  This procedure receives Diag packets from AriesDiag
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_forward_pkt(
  const PACKED void *req_pkt_ptr,
  word pkt_len
)
{
  aries_diag_sess_ctrl_req_type *req_ptr = (aries_diag_sess_ctrl_req_type *) req_pkt_ptr;

  if( pkt_len > TM_DIAG_PACKET_BUFFER_SIZE )
{
    MSG_ERROR("Diag packet too big: %d > %d",
              pkt_len, TM_DIAG_PACKET_BUFFER_SIZE, 0);
    MSG_ERROR("Bad packet Cmd code %d GPS cmd %d GPS Subcmd %d",
              req_ptr->cmd_code, req_ptr->gps_control, req_ptr->supporting_data[0]);

    pkt_len = TM_DIAG_PACKET_BUFFER_SIZE;
  }

  memset(tm_diag_pkt_buffer, 0, TM_DIAG_PACKET_BUFFER_SIZE);
  memscpy((void *)tm_diag_pkt_buffer, sizeof(tm_diag_pkt_buffer), (void *)req_pkt_ptr, pkt_len);
  tm_diag_pkt_buffer_length = pkt_len;
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_RF_DEV_COMMAND);
  return;
}

/*===========================================================================

FUNCTION ariesDiag_SessCtrl

DESCRIPTION
  This procedure starts retrieval of the GPS search grid and decide where 
  (TM or LM) to forward the request to.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * ariesDiag_SessCtrl (
  PACKED void *req_pkt_ptr,
  word pkt_len
)
{
  aries_diag_sess_ctrl_req_type *req_ptr = (aries_diag_sess_ctrl_req_type *) req_pkt_ptr;
  aries_diag_sess_ctrl_rsp_type *rsp_ptr = NULL;
  byte gps_cmd = req_ptr->gps_control;
  boolean lm_command = FALSE;

  /* Dispatch legacy requests to LM or TM */
  /* Find out what needs to be sent to LM, others should be sent to TM */

  /* AFTL and GPS control commands share the same gps_cmd code */
  /* We need to figure out what goes to LM and what goes to TM */
  if(gps_cmd == 7)
  {
    switch(req_ptr->supporting_data[0])
    {
      case 0x10:   /* Set gps search mode: high */
      case 0x11:   /* Set gps search mode: Fast TCal */
      case 0x13:   /* Set gps search mode: high to DeepMedium */
      case 0x1b:   /* Set gps search mode: high to SuperLow */
      case 0x23:   /* Turn off Jammer Detection Alg */
      case 0x24:   /* Turn on Jammer Detection Alg */
      case 0xA0:   /* Set GPS Intelliceiver mode */
        lm_command = TRUE; /*lint !e506 */
      break;

      default:
      break;      
    }
  }

  if(lm_command == TRUE) /*lint !e506  !e731 */
  {
    (void)lmDiag_ForwardGSCPkt(req_pkt_ptr, pkt_len);
  }
  else
  {
    tm_diag_forward_pkt(req_pkt_ptr, pkt_len);  
  }  
  
  rsp_ptr = (aries_diag_sess_ctrl_rsp_type *)diagpkt_alloc (DIAG_GPS_SESS_CTRL_F, sizeof (aries_diag_sess_ctrl_rsp_type));
  
  if(rsp_ptr != NULL)
  {
    rsp_ptr->gps_control = req_ptr->gps_control;
  }

  return rsp_ptr;
}


/*===========================================================================
FUNCTION tm_diag_query_blanking_cmd_handler

DESCRIPTION
  This function is used to query the blanking state (as configured by RF)
  and the current blanking count.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_query_blanking_cmd_handler(PACKED void* p_PktReq, uint16 w_PktLength)
{
  gpsdiag_QueryBlankingReqType* p_Req = (gpsdiag_QueryBlankingReqType*) p_PktReq;
  gpsdiag_QueryBlankingRspType* p_Rsp = NULL;
  mgp_QueryBlankingType z_QueryBlanking;

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_QueryBlankingRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                TM_DIAG_QUERY_BLANKING_CMD,
                                sizeof(gpsdiag_QueryBlankingRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_ERROR("tm_diag_query_blanking_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    /* Set the status in the response, 0 => Success */
    diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
 
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));

    /* Query the blanking config and counter from MGP */
    mgp_QueryBlanking(&z_QueryBlanking);

    /* Fill out the response */
    p_Rsp->b_Enabled = z_QueryBlanking.b_Enabled;
    p_Rsp->q_Rx1Counter = z_QueryBlanking.q_Rx1Counter;
    p_Rsp->q_Rx2Counter = z_QueryBlanking.q_Rx2Counter;
  }

  return p_Rsp;
} 
/*===========================================================================
FUNCTION tm_diag_nav_config_cmd_handler

DESCRIPTION
  This function is used to configure the Nav Core, which will then
  configure Nav RF as needed and notify upper layers like MC.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_nav_config_cmd_handler(PACKED void* p_PktReq, 
                                            uint16 w_PktLength)
{
  gpsdiag_NavConfigReqType* p_Req = (gpsdiag_NavConfigReqType*) p_PktReq;
  gpsdiag_NavConfigRspType* p_Rsp = NULL;
  mgp_NavConfigType z_NavConfig;

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_NavConfigRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_NAV_CONFIG_CMD,
                                   sizeof(gpsdiag_NavConfigRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_ERROR("tm_diag_nav_config_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    /* Set the status in the response, 0 => Success */
    diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
 
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));

    /* Send the config to MGP */
    z_NavConfig.u_GnssDesiredConfig = p_Req->u_GnssDesiredConfig;
    mgp_NavConfig(&z_NavConfig);

    /* Fill out the response */
    p_Rsp->u_GnssSupportedConfig = z_NavConfig.u_GnssSupportedConfig &
                                   ( C_RCVR_GNSS_CONFIG_GPS_ENABLED |
                                     C_RCVR_GNSS_CONFIG_GLO_ENABLED |
                                     C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |
                                     C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE );

    p_Rsp->u_GnssActualConfig = z_NavConfig.u_GnssActualConfig &
                                   ( C_RCVR_GNSS_CONFIG_GPS_ENABLED |
                                     C_RCVR_GNSS_CONFIG_GLO_ENABLED |
                                     C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |
                                     C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE );
  }

  return(p_Rsp);
} 

/*===========================================================================
FUNCTION tm_diag_nav_set_test_mode_cmd_handler

DESCRIPTION
  This function is used to do mandatory PGA backoff/restore to NavRF driver.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_nav_set_test_mode_cmd_handler(PACKED void* p_PktReq, 
                                            uint16 w_PktLength)
{
  gpsdiag_NavSetTestModeReqType* p_Req = (gpsdiag_NavSetTestModeReqType*) p_PktReq;
  gpsdiag_NavSetTestModeRspType* p_Rsp = NULL;

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_NavSetTestModeRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_NAV_SET_TEST_MODE_CMD,
                                   sizeof(gpsdiag_NavSetTestModeRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_ERROR("tm_diag_nav_set_pga_backoff_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));

    /*Send the config to MGP */
    if (mgp_NavSetTestMode(p_Req->u_Cmd)== TRUE )
    {
       /* Set the status in the response, 0 => Success */
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }
  }

  return(p_Rsp);
} 


/*===========================================================================
FUNCTION tm_diag_query_adc_iq_sat_per_cmd_handler

DESCRIPTION
  This function is used to do query adc iq saturation percentage

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_query_adc_iq_sat_per_cmd_handler(PACKED void* p_PktReq, 
                                                      uint16 w_PktLength)
{
  gpsdiag_QueryAdcIqSatPerReqType *p_Req = (gpsdiag_QueryAdcIqSatPerReqType *)p_PktReq; 
  gpsdiag_QueryAdcIqSatPerRspType *p_Rsp = NULL; 
  mgp_QueryAdcIqSatPerType z_AdcIqSatParams;


  /* Function parameters Sanity Check */
  if ( p_Req == NULL )
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_QueryAdcIqSatPerRspType *)
  diagpkt_subsys_alloc_v2((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                          TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD, 
                            sizeof(gpsdiag_QueryAdcIqSatPerRspType)); 

  /* Fill in response packet */
  if ( p_Rsp == NULL )
  {
    MSG_ERROR("tm_diag_query_adc_iq_sat_per_cmd_handler: Rsp packet is NULL!!", 0, 0, 0); 
  }
  else
  {
    MSG_HIGH("tm_diag_query_adc_iq_sat_per_cmd_handler: Rsp packet is not NULL!!", 0, 0, 0);

    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));

    /* Query ADC IQ saturation status from MGP */
    mgp_QueryAdcIqSatPer(&z_AdcIqSatParams);

    MSG_HIGH("tm_diag_query_adc_iq_sat_per_cmd_handler Status %u ISat %d QSat %d",
             z_AdcIqSatParams.u_AdcIqSatPerStatus,
             z_AdcIqSatParams.q_ISatPer,
             z_AdcIqSatParams.q_QSatPer);
    /* Fill out the response */
    p_Rsp->u_AdcIqSatPerStatus = z_AdcIqSatParams.u_AdcIqSatPerStatus;
    p_Rsp->q_ISatPer = z_AdcIqSatParams.q_ISatPer;
    p_Rsp->q_QSatPer = z_AdcIqSatParams.q_QSatPer; 

    if ( z_AdcIqSatParams.u_AdcIqSatPerStatus  == C_MGP_ADC_IQ_SAT_PER_VALID ) 
    {
      /* Set the status in the response, 0 => Success */
      diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
      diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }

  }

  return p_Rsp; 
}

/*===========================================================================
FUNCTION tm_diag_factory_iq_test_cmd_handler

DESCRIPTION
  This function handles Factory I/Q Test Command requests

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_factory_iq_test_cmd_handler(PACKED void* p_PktReq, 
                                                      uint16 w_PktLength)
{
  gpsdiag_QueryFactoryIqTestReqType *p_Req = (gpsdiag_QueryFactoryIqTestReqType *)p_PktReq; 
  gpsdiag_QueryFactoryIqTestImmedRspType *p_ImmedRsp = NULL; 
  lm_GnssIqTestType z_IqTest;

  /* Function parameters sanity check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  /* Make sure we are not currently processing a previous request */
  if( (z_tm_diag_factory_iq_test.v_FactoryIqTestRequested == FALSE) &&
      (z_tm_diag_factory_iq_test.w_DelayedRspId == 0) )
  {
    /* Allocate space through diag for response packet */
    p_ImmedRsp = (gpsdiag_QueryFactoryIqTestImmedRspType *)
    diagpkt_subsys_alloc_v2((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                            TM_DIAG_FACTORY_IQ_TEST_CMD, 
                              sizeof(gpsdiag_QueryFactoryIqTestImmedRspType)); 

    /* Fill in response packet */
    if(p_ImmedRsp == NULL)
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_factory_iq_test_cmd_handler: Unable to allocate immediate response packet"); 
    }
    else
    {
      /* Set the first (0) response count */
      diagpkt_subsys_set_rsp_cnt(&(p_ImmedRsp->z_DiagCmdHdrV2), 0);

      /* Get the delayed response ID and save it for later */
      z_tm_diag_factory_iq_test.w_DelayedRspId = 
        diagpkt_subsys_get_delayed_rsp_id(&(p_ImmedRsp->z_DiagCmdHdrV2));
      z_tm_diag_factory_iq_test.u_Version = p_Req->u_Version;

      MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
          "tm_diag_factory_iq_test_cmd_handler: Saved delayed response ID 0x%04X", z_tm_diag_factory_iq_test.w_DelayedRspId); 

       /* Copy the Factory I/Q test request into the "standard" I/Q
          test command structure and send it to LM */
       z_IqTest.u_Version = p_Req->u_Version;
       z_IqTest.u_TestControl = 0;
       /* Ensure that the dropped log recovery feature is disabled */
       z_IqTest.w_Flags = p_Req->w_Flags | 0x0008; 
       z_IqTest.w_RetransmitPktNum = 0;
       z_IqTest.u_RfStatusLoggingRateHz = 4; /* 1-10 allowed */
       z_IqTest.u_CollectMode = p_Req->u_CollectMode;
       z_IqTest.b_GloR1Chan = p_Req->b_GloR1Chan;
       z_IqTest.u_GloHWChan = p_Req->u_GloHWChan;
       z_IqTest.w_SampleCnt1k = p_Req->w_SampleCnt1k;
       z_IqTest.u_Integrations = p_Req->u_Integrations;
       z_IqTest.l_CenterFreqNBHz = 0; /* n/a for Factory I/Q test */
       z_IqTest.q_ADCIqSatThresh = p_Req->q_ADCIqSatThresh;
       z_IqTest.q_AdcIQWidebandJammerStartFreqHz =
         p_Req->q_AdcIQWidebandJammerStartFreqHz;  
       z_IqTest.q_AdcIQWidebandJammerStopFreqHz = 
         p_Req->q_AdcIQWidebandJammerStopFreqHz;
       z_IqTest.l_AdcIQRefTonePwrDBm = 
         p_Req->l_AdcIQRefTonePwrDBm;

      /* Send the I/Q test request to LM */
      if(lm_send_sarf_cmd((uint8 *)&z_IqTest, ARIES_MSG_ID_TM_LM_IQ_TEST) == FALSE) 
      { 
        /* Set the status in the response, 0 => Success */
        diagpkt_subsys_set_status(&(p_ImmedRsp->z_DiagCmdHdrV2), 0);
        z_tm_diag_factory_iq_test.v_FactoryIqTestRequested = TRUE;
      }
      else
      { 
        /* Failure, clear the response ID, nothing more to follow */
        diagpkt_subsys_set_status(&(p_ImmedRsp->z_DiagCmdHdrV2), 1);
        diagpkt_subsys_reset_delayed_rsp_id(&(p_ImmedRsp->z_DiagCmdHdrV2));
        z_tm_diag_factory_iq_test.w_DelayedRspId = 0;
        z_tm_diag_factory_iq_test.v_FactoryIqTestRequested = FALSE;

        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_factory_iq_test_cmd_handler: I/Q test send to LM failed"); 
      }
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
        "tm_diag_factory_iq_test_cmd_handler: Request denied, still processing previous request");
  }

  return(p_ImmedRsp); 
}

/*===========================================================================
FUNCTION tm_diag_ioctl_cmd_handler

DESCRIPTION
  This function is used to run multiple ftm tests.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_ioctl_cmd_handler(PACKED void* p_PktReq, 
                                            uint16 w_PktLength)
{
  gpsdiag_NavIoctlReqType* p_Req = (gpsdiag_NavIoctlReqType*) p_PktReq;
  gpsdiag_NavIoctlRspType* p_Rsp = NULL;
  mgp_NavIoctlType z_NavIoctl;

  memset((void *)&z_NavIoctl, 0, sizeof(mgp_NavIoctlType));

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_NavIoctlRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_NAV_IOCTL_CMD,
                                   sizeof(gpsdiag_NavIoctlRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_ERROR("tm_diag_nav_set_pga_backoff_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));

    /*Set the request params*/
    z_NavIoctl.u_RequestCode = p_Req->u_RequestCode;
    z_NavIoctl.l_RequestParam1 = p_Req->l_RequestParam1;
    z_NavIoctl.l_RequestParam2 = p_Req->l_RequestParam2;

    /*Send the config to MGP */
    if (mgp_NavIoctl(&z_NavIoctl)== TRUE )
    {
       /* Set the status in the response, 0 => Success */
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }
     /*Set the response params*/
     p_Rsp->u_RequestCode = z_NavIoctl.u_RequestCode;
     p_Rsp->l_ResponseParam1 = z_NavIoctl.l_ResponseParam1;
     p_Rsp->l_ResponseParam2 = z_NavIoctl.l_ResponseParam2;
  }

  return(p_Rsp);
} 

/*  Diag table for the session control function */
static const diagpkt_user_table_entry_type ariesOneDiag_LegacyTbl[] =
{
  {DIAG_GPS_SESS_CTRL_F, DIAG_GPS_SESS_CTRL_F, ariesDiag_SessCtrl}
};

/*  Diag table for the new subsystem commands */
static const diagpkt_user_table_entry_type tm_DiagTable[] =
{
  {TM_DIAG_SESS_REQ_START_COMMAND,           TM_DIAG_SESS_REQ_START_COMMAND,           tm_diag_sess_start_req_cmd_handler},
  {TM_DIAG_SESS_REQ_STOP_COMMAND,            TM_DIAG_SESS_REQ_STOP_COMMAND,            tm_diag_sess_stop_req_cmd_handler},
  {TM_DIAG_SESS_REQ_CONTINUE_COMMAND,        TM_DIAG_SESS_REQ_CONTINUE_COMMAND,        tm_diag_sess_continue_req_cmd_handler},
  {TM_DIAG_SESS_POST_DATA_COMMAND,           TM_DIAG_SESS_POST_DATA_COMMAND,           tm_diag_sess_post_data_cmd_handler}, 
  {TM_DIAG_SESS_POLLING_PPM_COMMAND,         TM_DIAG_SESS_POLLING_PPM_COMMAND,         tm_diag_sess_polling_ppm_cmd_handler},
  {TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND,  TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND,  tm_diag_sess_del_aiding_data_cmd_handler},
  {TM_DIAG_SESS_REQ_SW_VER_COMMAND,          TM_DIAG_SESS_REQ_SW_VER_COMMAND,          tm_diag_sess_req_sw_ver_cmd_handler},
  {TM_DIAG_SESS_SARF_COMMAND,                TM_DIAG_SESS_SARF_COMMAND,                tm_diag_sarf_cmd_handler},
  {TM_DIAG_PORT_CONTROL_COMMAND,             TM_DIAG_PORT_CONTROL_COMMAND,             tm_diag_port_control_cmd_handler}
};

static const diagpkt_user_table_entry_type gpsdiag_SubsysCmdTable_v2[] = 
{
#ifdef FEATURE_CGPS_FILE_SYSTEM
  {CGPS_EFS_FILE_HANDLING_F, CGPS_EFS_FILE_HANDLING_F, gpsdiag_EfsFileReqHandler},
#endif /* FEATURE_CGPS_FILE_SYSTEM */
  {TM_DIAG_SV_NO_EXIST_CMD, TM_DIAG_SV_NO_EXIST_CMD, tm_diag_sv_no_exist_cmd_handler},
  {TM_DIAG_FORCE_DPO_FOR_POWER_MEAS_CMD, TM_DIAG_FORCE_DPO_FOR_POWER_MEAS_CMD, tm_diag_force_dpo_for_power_meas_cmd_handler},
  {TM_DIAG_DATA_DUMP_CMD, TM_DIAG_DATA_DUMP_CMD, tm_diag_data_dump_cmd_handler},
  {TM_DIAG_QUERY_BLANKING_CMD, TM_DIAG_QUERY_BLANKING_CMD, tm_diag_query_blanking_cmd_handler},
  {TM_DIAG_NAV_CONFIG_CMD, TM_DIAG_NAV_CONFIG_CMD, tm_diag_nav_config_cmd_handler},
  {TM_DIAG_NAV_SET_TEST_MODE_CMD, TM_DIAG_NAV_SET_TEST_MODE_CMD, tm_diag_nav_set_test_mode_cmd_handler},
  {TM_DIAG_NAV_IOCTL_CMD,TM_DIAG_NAV_IOCTL_CMD, tm_diag_ioctl_cmd_handler},
  {TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD, TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD, tm_diag_query_adc_iq_sat_per_cmd_handler },
  {TM_DIAG_FACTORY_IQ_TEST_CMD, TM_DIAG_FACTORY_IQ_TEST_CMD, tm_diag_factory_iq_test_cmd_handler }
};

/*===========================================================================

FUNCTION tm_diag_start_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_diag_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
)
{
  (void)sess_handle;  
  (void)start_sess_req_param;
  (void)actions;  

  return PRTL_START_SESS_REQ_ALLOWED;
}

/*===========================================================================

FUNCTION tm_diag_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
   tm_sess_req_param_u_type tm_sess_req_param;
   
  (void)stop_type;

   tm_sess_req_param.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
   tm_sess_req_param.stop_param.stop_reason = stop_reason;
   
   (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                     TM_SESS_HANDLE_FTEST,
                     TM_SESS_REQ_STOP,
                     &tm_sess_req_param);
                  
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_diag_sess_req_data_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  (void)session_handle;  
  (void)req_data;

  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_diag_sess_info_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_sess_info_handler
(
  tm_sess_handle_type                session_handle,
  prtl_sess_info_e_type              sess_info_type,
  prtl_sess_info_param_u_type       *sess_info_param
)
{
#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
  int i;
  is801_rev_link_resp_s_type      resp_msg;
  is801_pddm_sess_info_s_type     rev_pddm_hdr; 
  is801_rev_link_pddm_info_s_type rev_pddm; 
  int                             num_rev_pddm;
  boolean                         ret_val = FALSE;
#endif /* FEATURE_CGPS_CDMA_IF && ! FEATURE_GNSS_LOW_MEMORY */

  if(sess_info_param == NULL)
  {
    MSG_ERROR("Null sess_info_param pointer", 0, 0, 0);   
    return FALSE;
  }
  
  if(session_handle == TM_SESS_HANDLE_FTEST)
  {
    switch(sess_info_type)
    {
#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
      case PRTL_SESS_INFO_PPM:
        if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
        {
          if(tm_diag_cached_prm_available == FALSE)
          {
            tm_diag_cached_ppm.l_RfCalibGCx1024 = is801_get_rf_delay(sess_info_param->ppm_data.band_class);
            memscpy((void *)&(tm_diag_cached_ppm.z_PpmRpt), sizeof(tm_diag_cached_ppm.z_PpmRpt),
                    (const void *)&(sess_info_param->ppm_data), sizeof(sess_info_param->ppm_data));
            tm_diag_cached_ppm_available = TRUE;
              
            /* If cached PRM is not available, just log PPM */
            resp_msg.response_type = IS801_REV_LINK_RESP_MASK_PPM;
            resp_msg.unsolicited   = 1;
            (void)is801_util_rlink_convert_ppm(&(sess_info_param->ppm_data),
                                             &(resp_msg.resp_msg_payload.ppm_resp));
            (void)is801_new_rev_link_resp(TM_PRTL_TYPE_DIAG, &resp_msg);

            rev_pddm_hdr.pd_msg_type = PD_MSG_TYPE;
            rev_pddm_hdr.sess_start  = 0;
            rev_pddm_hdr.sess_end    = 0;
            rev_pddm_hdr.sess_tag    = 0;
            rev_pddm_hdr.sess_source = 0;
  
            num_rev_pddm = is801_create_rev_link_pddm(TM_PRTL_TYPE_DIAG, rev_pddm_hdr, MAX_IS801_PDDM_SIZE);

            for(i = 0; i < num_rev_pddm; i++)
            {
              if(is801_get_rev_link_pddm(TM_PRTL_TYPE_DIAG, &rev_pddm))
              {
                is801_util_log_rlink_msg((void *)&(rev_pddm.msg_payload[0]), rev_pddm.msg_size);
              }
            }
          }
          else
          {
            tm_diag_cached_ppm.l_RfCalibGCx1024 = is801_get_rf_delay(sess_info_param->ppm_data.band_class);
            memscpy((void *)&(tm_diag_cached_ppm.z_PpmRpt), sizeof(tm_diag_cached_ppm.z_PpmRpt),
                    (const void *)&(sess_info_param->ppm_data), sizeof(sess_info_param->ppm_data));
            tm_diag_cached_ppm_available = TRUE;
            
            /* If cached PRM is available, log PPM and PRM together */
            tm_diag_cached_prm_available = FALSE;
            
            ret_val = is801_util_rlink_convert_prm(&(sess_info_param->ppm_data),
                                                   &(tm_diag_cached_prm),
                                                   &(resp_msg.resp_msg_payload.prm_resp));

            /* If PRM has bogus peak, ignore it */
            if(ret_val == TRUE) /*lint !e506  !e731 */
            {
              resp_msg.response_type = IS801_REV_LINK_RESP_MASK_PRM;
              resp_msg.unsolicited   = 1;
              (void)is801_new_rev_link_resp(TM_PRTL_TYPE_DIAG, &resp_msg);
            }
            
            resp_msg.response_type = IS801_REV_LINK_RESP_MASK_PPM;
            resp_msg.unsolicited   = 1;
            (void)is801_util_rlink_convert_ppm(&(sess_info_param->ppm_data),
                                                 &(resp_msg.resp_msg_payload.ppm_resp));
            (void)is801_new_rev_link_resp(TM_PRTL_TYPE_DIAG, &resp_msg);

            rev_pddm_hdr.pd_msg_type = PD_MSG_TYPE;
            rev_pddm_hdr.sess_start  = 0;
            rev_pddm_hdr.sess_end    = 0;
            rev_pddm_hdr.sess_tag    = 0;
            rev_pddm_hdr.sess_source = 0;
  
            num_rev_pddm = is801_create_rev_link_pddm(TM_PRTL_TYPE_DIAG, rev_pddm_hdr, MAX_IS801_PDDM_SIZE);

            for(i = 0; i < num_rev_pddm; i++)
            {
              if(is801_get_rev_link_pddm(TM_PRTL_TYPE_DIAG, &rev_pddm))
              {
                is801_util_log_rlink_msg((void *)&(rev_pddm.msg_payload[0]), rev_pddm.msg_size);
              }
            }
          }
        }
        break;

      case PRTL_SESS_INFO_PRM:
        if ( sess_info_param->prm_data.e_MeasRptReason == GPS_MEAS_RPT_MEAS_DONE )
        {
          if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
          {  
            if(tm_diag_ppm_in_progress == FALSE)
            {            
              /* If ppm is not in progress, log PRM right now */
              resp_msg.response_type = IS801_REV_LINK_RESP_MASK_PRM;
              resp_msg.unsolicited   = 1;
            
              ret_val = is801_util_rlink_convert_prm(NULL,
                                                    &(sess_info_param->prm_data),
                                                    &(resp_msg.resp_msg_payload.prm_resp));

              /* If PRM has bogus peak, ignore it */
              if(ret_val == TRUE) /*lint !e506  !e731 */
              {
                (void)is801_new_rev_link_resp(TM_PRTL_TYPE_DIAG, &resp_msg);

                rev_pddm_hdr.pd_msg_type = PD_MSG_TYPE;
                rev_pddm_hdr.sess_start  = 0;
                rev_pddm_hdr.sess_end    = 0;
                rev_pddm_hdr.sess_tag    = 0;
                rev_pddm_hdr.sess_source = 0;
  
                num_rev_pddm = is801_create_rev_link_pddm(TM_PRTL_TYPE_DIAG, rev_pddm_hdr, MAX_IS801_PDDM_SIZE);

                for(i = 0; i < num_rev_pddm; i++)
                {
                  if(is801_get_rev_link_pddm(TM_PRTL_TYPE_DIAG, &rev_pddm))
                  {
                    is801_util_log_rlink_msg((void *)&(rev_pddm.msg_payload[0]), rev_pddm.msg_size);
                  }
                }      
              }

              /* Get PPM result */
              tm_diag_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;

              (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                                TM_SESS_HANDLE_FTEST,
                                TM_SESS_REQ_POLLING_DATA,
                                &tm_diag_sess_req_param);
            }
            else
            {
              /* This flag is used to tell us if cached PRM is avaliable getting PPM results */          
              tm_diag_cached_prm_available = TRUE;
            
              /* If ppm is in progress, cache PRM and log it when we get PPM */
              MSG_HIGH("Caching PRM", 0, 0, 0);
              memscpy((void *)&tm_diag_cached_prm, sizeof(tm_diag_cached_prm),
                     (void *)&(sess_info_param->prm_data), sizeof(sess_info_param->prm_data));
            
              /* Get PPM result */
              tm_diag_sess_req_param.polling_data_param.op_req = TM_OP_REQ_PPM;

              (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                                TM_SESS_HANDLE_FTEST,
                                TM_SESS_REQ_POLLING_DATA,
                                &tm_diag_sess_req_param);            
            }
          }
        }
      break;

      case PRTL_SESS_INFO_STATUS:
        MSG_HIGH("Got Aiding Data Wishlist", 0 ,0 ,0);
        break; 

      case PRTL_SESS_INFO_LR:
        MSG_HIGH("Got LR",0, 0, 0);
        if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
        {  
          /* Log PPM */
          resp_msg.response_type = IS801_REV_LINK_RESP_MASK_LR;
          resp_msg.unsolicited   = 1;
          (void)is801_util_convert_tm_lr_to_is801_lr(&(sess_info_param->lr_data),
                                                     &(resp_msg.resp_msg_payload.lr_resp));

          (void)is801_new_rev_link_resp(TM_PRTL_TYPE_DIAG, &resp_msg);

          rev_pddm_hdr.pd_msg_type = PD_MSG_TYPE;
          rev_pddm_hdr.sess_start  = 0;
          rev_pddm_hdr.sess_end    = 0;
          rev_pddm_hdr.sess_tag    = 0;
          rev_pddm_hdr.sess_source = 0;
  
          num_rev_pddm = is801_create_rev_link_pddm(TM_PRTL_TYPE_DIAG, rev_pddm_hdr, MAX_IS801_PDDM_SIZE);

          for(i = 0; i < num_rev_pddm; i++)
          {
            if(is801_get_rev_link_pddm(TM_PRTL_TYPE_DIAG, &rev_pddm))
            {
              is801_util_log_rlink_msg((void *)&(rev_pddm.msg_payload[0]), rev_pddm.msg_size);
            }
          }
        }
        break; 
        
#endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */

      default:
        break;
    }
    return TRUE; /*lint !e506 */
  }
  else
  {
    MSG_ERROR("Invalid handle received", 0, 0, 0);
    return FALSE;
  }  
}
#ifdef FEATURE_CGPS_PDCOMM
/*===========================================================================

FUNCTION tm_diag_data_pipe_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_data_pipe_handler
(
  pd_comms_client_ipc_message_type  msg_type,
  void                              *msg_data_ptr
)
{
  (void)msg_type;
  (void)msg_data_ptr;  

  return TRUE; /*lint !e506 */
}
#endif

/*===========================================================================

FUNCTION tm_diag_timer_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_timer_cb_handler
(
  void *timer_data
)
{
  (void)timer_data;  

  return;
}

/*===========================================================================

FUNCTION tm_diag_event_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload
)
{
  (void)event_type;
  (void)event_payload;  

  return TRUE;
}

#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)

/*===========================================================================

FUNCTION: tm_diag_is801_msg_cb

DESCRIPTION:
  This function is called by IS801 module to inform
  PDDM data is available.
  
PARAMETERS:
  
===========================================================================*/
boolean tm_diag_is801_msg_cb
(
  is801_fwd_pddm_info_s_type     pddm_info
)
{
  int i;
  int j;
  int k;
  is801_fwd_link_pddm_info_s_type  fwd_link_pddm;        
  tm_post_data_payload_type        *aiding_data_ptr;
  tm_post_data_payload_type        *mgp_sv_dir_ptr;  
  
  MSG_HIGH("# of PDDM = %d ", pddm_info.num_pddm, 0, 0);

  aiding_data_ptr = (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));

  if(aiding_data_ptr == NULL)
  {
    MSG_ERROR("Fail to get memory for aiding_data_ptr", 0, 0, 0);
    return FALSE;
  }
    
  
  /* Get Requests */
  for(i = 0; i < pddm_info.num_pddm; i++)
  {
    (void)is801_get_fwd_link_pddm(TM_PRTL_TYPE_DIAG,  &fwd_link_pddm);

    /* Process Requests */
    for(j = 0; j < fwd_link_pddm.hdr.num_requests; j++)
    {
      MSG_HIGH("Ignore IS801 requests for TM Diag", 0, 0, 0);
    }

    /* Process Responses */
    for(j = 0; j < fwd_link_pddm.hdr.num_responses; j++)
    {
      switch(fwd_link_pddm.resp_rd[j].resp_type)
      {
        case IS801_FWD_LINK_RESP_MASK_AA:
          /* If we're doing RF development test, start PPM and PRM */
          if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
          {
            /* In FTEST mode, use hardcoded values for PPM */
            tm_diag_ppm_in_progress = TRUE;
            tm_diag_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_FTM;
            tm_diag_sess_req_param.continue_param.op_req  = TM_OP_REQ_PPM;
            tm_diag_sess_req_param.continue_param.ppm_qos = 60;

            (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                              TM_SESS_HANDLE_FTEST,
                              TM_SESS_REQ_CONTINUE,
                              &tm_diag_sess_req_param);


            /* In FTEST mode, use hardcoded values for PRM */
            tm_diag_sess_req_param.continue_param.op_mode = TM_SESS_OPERATION_MODE_FTM;
            tm_diag_sess_req_param.continue_param.op_req  = TM_OP_REQ_PRM;
            tm_diag_sess_req_param.continue_param.prm_qos = 60;
            tm_diag_sess_req_param.continue_param.prm_prq = 0;
            tm_diag_sess_req_param.continue_param.multi_report_msa = FALSE;

            (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                              TM_SESS_HANDLE_FTEST,
                              TM_SESS_REQ_CONTINUE,
                              &tm_diag_sess_req_param);

            mgp_sv_dir_ptr = (tm_post_data_payload_type *)pdsm_getbuf(sizeof(tm_post_data_payload_type));

            if(mgp_sv_dir_ptr == NULL)
            {
              MSG_ERROR("Fail to get memory for aiding_data_ptr", 0, 0, 0);

              (void) pdsm_freebuf((char *)aiding_data_ptr);
              
              return FALSE;
            }

            /* Send prefix-PPM along with AA data */
            if(tm_diag_cached_ppm_available == TRUE)
            {
              memscpy((void*)&(aiding_data_ptr->ppm_data), sizeof(aiding_data_ptr->ppm_data), (const void *)&tm_diag_cached_ppm, sizeof(tm_diag_cached_ppm));
              
              MSG_MED("Send pre-fix PPM data to TM-Core", 0, 0, 0);
              
              (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                                 TM_SESS_HANDLE_FTEST,
                                 TM_POST_DATA_TYPE_PREFIX_PPM,
                                 aiding_data_ptr);

              tm_diag_cached_ppm_available = FALSE;
            }
      

            /* Post AA to LM */
            (void)is801_util_flink_convert_aa(&(fwd_link_pddm.resp_rd[j].resp_msg_payload.aa_resp),
                                              &(aiding_data_ptr->gnss_aa_data),
                                              &(mgp_sv_dir_ptr->gnss_sv_dir));



            (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                               TM_SESS_HANDLE_FTEST,
                               TM_POST_DATA_TYPE_AA,
                               aiding_data_ptr);

            (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                               TM_SESS_HANDLE_FTEST,
                               TM_POST_DATA_TYPE_SV_DIR,
                               mgp_sv_dir_ptr);

            (void) pdsm_freebuf((char *)mgp_sv_dir_ptr);
          }
        break;

        case IS801_FWD_LINK_RESP_MASK_SA:
          if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
          {
            /* Post SA to LM */
            (void)is801_util_flink_convert_sa(&(fwd_link_pddm.resp_rd[j].resp_msg_payload.sa_resp),
                                              &(aiding_data_ptr->sa_data));

            (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                               TM_SESS_HANDLE_FTEST,
                               TM_POST_DATA_TYPE_SA,
                               aiding_data_ptr);          
          }
        break;

        case IS801_FWD_LINK_RESP_MASK_EPH:
          if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
          {
            if(fwd_link_pddm.resp_rd[j].resp_msg_payload.eph_resp.fix.u_AB_Par_Incl)
            {
              (void)is801_util_flink_convert_iono(&(fwd_link_pddm.resp_rd[j].resp_msg_payload.eph_resp),
                                                  &(aiding_data_ptr->iono_data));
              
              (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                                 TM_SESS_HANDLE_FTEST,
                                 TM_POST_DATA_TYPE_IONO,
                                 aiding_data_ptr);
            }
  
            /* Post EPH to LM */ 
            for(k = 0; k < fwd_link_pddm.resp_rd[j].resp_msg_payload.eph_resp.fix.u_TotalSats; k++)
            {            
              (void)is801_util_flink_convert_eph(&(fwd_link_pddm.resp_rd[j].resp_msg_payload.eph_resp),
                                                 &(aiding_data_ptr->eph_data),
                                                 k);
              
              (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                                 TM_SESS_HANDLE_FTEST,
                                 TM_POST_DATA_TYPE_EPH,
                                 aiding_data_ptr);          
            }
          }
        break;

        case IS801_FWD_LINK_RESP_MASK_ALM:
          if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
          {
            /* Post ALM to LM */
            for(k = 0; k < fwd_link_pddm.resp_rd[j].resp_msg_payload.alm_resp.fix.u_TotalSats; k++)
            {            
              (void)is801_util_flink_convert_alm(&(fwd_link_pddm.resp_rd[j].resp_msg_payload.alm_resp),
                                                 &(aiding_data_ptr->alm_data),
                                                 k);

              (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                                 TM_SESS_HANDLE_FTEST,
                                 TM_POST_DATA_TYPE_ALM,
                                 aiding_data_ptr);     
            }
          }
        break;

        case IS801_FWD_LINK_RESP_MASK_LR:
          if(tm_diag_in_ftest_mode == TRUE) /*lint !e506  !e731 */
          {
            (void)is801_util_flink_convert_seed(&(fwd_link_pddm.resp_rd[j].resp_msg_payload.lr_resp),
                                                &(aiding_data_ptr->seed_data));

            (void)tm_post_data(TM_PRTL_TYPE_DIAG,
                               TM_SESS_HANDLE_FTEST,
                               TM_POST_DATA_TYPE_SEED,
                               aiding_data_ptr);     
          }
        break;

        default:
          break;
      }
    }
  }  

  (void) pdsm_freebuf((char *)aiding_data_ptr);  
  
  return TRUE; /*lint !e506 */
}

#endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */

/*===========================================================================

FUNCTION: tm_diag_init

DESCRIPTION:
  
PARAMETERS:
  
===========================================================================*/
void tm_diag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, ariesOneDiag_LegacyTbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_GPS, tm_DiagTable); /*lint !e641 */
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F,(uint8)DIAG_SUBSYS_GPS, gpsdiag_SubsysCmdTable_v2);
}

/*===========================================================================

FUNCTION: tm_diag_init_prtl_table

DESCRIPTION:
  
PARAMETERS:
  
===========================================================================*/
void tm_diag_init_prtl_table(void)
{
  tm_diag_prtl_table.start_sess_req_fp   = tm_diag_start_sess_req_handler; 
  tm_diag_prtl_table.stop_sess_req_fp    = tm_diag_stop_sess_req_handler;
  tm_diag_prtl_table.sess_req_data_fp    = tm_diag_sess_req_data_handler;
  tm_diag_prtl_table.sess_info_fp        = tm_diag_sess_info_handler; 
  #ifdef FEATURE_CGPS_PDCOMM 
  tm_diag_prtl_table.data_pipe_handle_fp = tm_diag_data_pipe_handler; //gk:QST1500 
  #endif
  tm_diag_prtl_table.timer_cb_fp         = tm_diag_timer_cb_handler;
  tm_diag_prtl_table.event_cb_fp         = tm_diag_event_cb_handler;
  tm_diag_prtl_table.general_query_fp    = NULL;  
  tm_diag_prtl_table.gm_event_handle_fp  = NULL;
}

/*===========================================================================

FUNCTION tm_diag_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_prtl_init(void)
{
  MSG_MED("tm_diag_prtl_init", 0, 0, 0);
  
  tm_diag_init_prtl_table();
  
  (void)tm_prtl_reg(TM_PRTL_TYPE_DIAG, &tm_diag_prtl_table);

#if defined (FEATURE_CGPS_CDMA_IF) && !defined (FEATURE_GNSS_LOW_MEMORY)
  (void)is801_prtl_reg(TM_PRTL_TYPE_DIAG, tm_diag_is801_msg_cb);
#endif /* FEATURE_CGPS_CDMA_IF && !FEATURE_GNSS_LOW_MEMORY */
}




/*===========================================================================

FUNCTION tm_diag_wbiq_info_handler

DESCRIPTION 
  Handles new info received for the WBIQ report.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_wbiq_info_handler(const sm_ReportWBIQInfoStructType *p_ReportWBIQInfo)
{
  gpsdiag_QueryFactoryIqTestDelayedRspType *p_DelayedRsp = NULL;

  if(p_ReportWBIQInfo != NULL)
  {
    tm_diag_wbiq_info = *p_ReportWBIQInfo;
    tm_diag_wbiq_info_cnt++;

    /* If a delayed response for Factory I/Q Test was requested,
       send it now */
    if(z_tm_diag_factory_iq_test.v_FactoryIqTestRequested == TRUE)
    {
      p_DelayedRsp = (gpsdiag_QueryFactoryIqTestDelayedRspType *)
        diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type) DIAG_SUBSYS_GPS,
                                       TM_DIAG_FACTORY_IQ_TEST_CMD,
                                       z_tm_diag_factory_iq_test.w_DelayedRspId,
                                       sizeof (gpsdiag_QueryFactoryIqTestDelayedRspType));

      if(p_DelayedRsp != NULL)
      {
        /* Set the response count to 1 */
        diagpkt_subsys_set_rsp_cnt(&(p_DelayedRsp->z_DiagCmdHdrV2), 1);
        /* Set the status to 0 => Success */ 
        diagpkt_subsys_set_status(&(p_DelayedRsp->z_DiagCmdHdrV2), 0); 
        
        /* Fill in the rest of the delayed response */
        p_DelayedRsp->u_Version = z_tm_diag_factory_iq_test.u_Version;
        p_DelayedRsp->u_GnssConfig = p_ReportWBIQInfo->u_GnssConfig;
        p_DelayedRsp->q_GpsCnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_GpsCNoDBHz));
        p_DelayedRsp->l_GpsFreqHz = ((int32)p_ReportWBIQInfo->f_GpsFreqHz);
        p_DelayedRsp->q_GloCnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_GloCNoDBHz));
        p_DelayedRsp->l_GloFreqHz = ((int32)p_ReportWBIQInfo->f_GloFreqHz);
        p_DelayedRsp->q_BdsCnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_BdsCNoDBHz));
        p_DelayedRsp->l_BdsFreqHz = ((int32)p_ReportWBIQInfo->f_BdsFreqHz);
        p_DelayedRsp->q_GalCnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_GalCNoDBHz));
        p_DelayedRsp->l_GalFreqHz = ((int32)p_ReportWBIQInfo->f_GalFreqHz);
        p_DelayedRsp->l_AdcMeanIp1mV = p_ReportWBIQInfo->l_AdcMeanIp1mV;
        p_DelayedRsp->l_AdcMeanQp1mV = p_ReportWBIQInfo->l_AdcMeanQp1mV;
        p_DelayedRsp->q_AdcAmpIp1mV = p_ReportWBIQInfo->q_AdcAmpIp1mV;
        p_DelayedRsp->q_AdcAmpQp1mV = p_ReportWBIQInfo->q_AdcAmpQp1mV;
        p_DelayedRsp->u_AdcIqSatPerStatus = p_ReportWBIQInfo->u_AdcIqSatPerStatus;
        p_DelayedRsp->q_ISatPer = p_ReportWBIQInfo->q_ISatPer;
        p_DelayedRsp->q_QSatPer = p_ReportWBIQInfo->q_QSatPer;
        p_DelayedRsp->l_AdcIqWbJammerPwrEstDb = p_ReportWBIQInfo->l_AdcIqWbJammerPwrEstDb;

        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
          "tm_diag_wbiq_info_handler: Sending delayed response for ID 0x%04X", z_tm_diag_factory_iq_test.w_DelayedRspId); 

        /* Send it! */
        diagpkt_delay_commit((void *)p_DelayedRsp);
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_wbiq_info_handler: Unable to allocate delayed response packet"); 
      }

      /* Clear out the cached flag and ID */
      z_tm_diag_factory_iq_test.v_FactoryIqTestRequested = FALSE;
      z_tm_diag_factory_iq_test.w_DelayedRspId = 0;
    }
    else
    {
      MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_wbiq_info_handler: v_FactoryIqTestRequested is FALSE"); 
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_wbiq_info_handler: Input pointer is NULL"); 
  }
}
