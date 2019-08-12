/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     C A L L   M A N A G E R   AUTO   REGISTER   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Auto Register Object.

  The Auto Register Object is responsible for:
  1. Reading NV for IMSI, OPERATOR and DEVICE Details
  2. Notifying the Network with Auto Register Message


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmss_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1998 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmautoreg.c#1 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------- 
09/04/13   mh/jvo   Mainline the modem statistics code
07/10/13   ar       checking index of array SM and Lint Errors
06/03/13   nvuchula FR3264: Auto Register Short Message
05/17/13   dk       CR470025 stack reduction effort

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/
#include "queue.h"

#include "mmcp_variation.h"
#include "customer.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"
#include "cmautoreg.h"      /* Interface to CM Serving System Object */
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* System wide definitions */
#include "sys_v.h"
#include "cmtaski.h"    /* Interface to timer definitions */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmlog.h"     /* Interface for Diagnostic Logging */
#include "cmlog_v.h"
#include "cmph.h"      /* Interface to CM phone */
#include "cmutil.h"    /* Interface for cm_util_undefine_sys_id */
#include "cmss.h"      /* Interface to CM serving system object */
#include "cmmmgsdi.h"  /* Interface to MMGSDI */

#if (defined(FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH) && defined(FEATURE_WCDMA))
#include "cmwll.h"
#endif

#include "mmoc.h"      /* interface to dormant protocol state */
#include "time_svc.h"       /* Interface to clock services */

#if (defined CM_GW_SUPPORTED|| defined FEATURE_EOOS )
#include "cmregprx.h"  /* Interface for CM Reg Proxy */
#include "cmcall.h"
#include "cmregprxi.h"  /* Regproxy implementation related declarations */
#include <string.h>
#endif /* defined CM_GW_SUPPORTED */

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "mccreg.h"    /* CDMA registration service */
#include "mccreg_v.h"
#include "srch.h"
#include "srch_v.h"
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

#include "nv.h"        /* interface to NV */


#include "cm_msgr_msg.h"


#ifdef FEATURE_HDR_HANDOFF
#include "cmssidm.h"     /* Interface to CM IDM module */
#endif // FEATURE_HDR_HANDOFF

#ifdef FEATURE_IP_CALL
#include "cmipappi.h" /* Interface to inform cm ip app about service */
#endif

#ifdef FEATURE_BCMCS
#include "cmbcmcs.h"
#endif


#ifdef FEATURE_DEDICATED_C2K_MODE
#include "cmxdbm.h"
#include "cmxsms.h"
#endif

#if defined(FEATURE_WCDMA)
#include "rrcmmif.h"
#endif

#if defined(FEATURE_TDSCDMA)
#include "tdsrrccsp.h"
#endif

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
#ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
#include "geran_eng_mode_read_api.h"
#else
#include "rr_cb_if.h"
#endif  /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */
#endif /* defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK) */

#ifdef FEATURE_CM_LTE
#include "esm_cm_msgr_enum_msg.h"  /* Interface to UMID */
#include "cmltecall.h"
#endif

#include "cmsds.h"
#include "cmsoa.h"

#ifdef FEATURE_MODEM_STATISTICS
#include "cmstats.h"
#endif

#if (defined(FEATURE_HDR) || defined(FEATURE_HDR_HYBRID))
#include "hdrcp_msg.h"
#include "hdrl1_api.h"
#endif

#include "cmefs.h"

#ifdef FEATURE_MMODE_DUAL_SIM
#include "subs_prio.h"
#endif

#include "cmmmgsdi.h"

#ifdef FEATURE_MMODE_SC_SVLTE
#include "cmregprxi.h"
#endif
#ifdef CM_DEBUG
#error code not present
#endif


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

static const byte DigitToAscTable[10] =
		{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

static const uint32  crc32_reg_table[256] = {
  0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
  0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
  0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
  0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
  0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
  0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
  0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
  0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
  0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
  0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
  0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
  0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
  0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
  0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
  0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
  0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
  0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
  0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
  0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
  0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
  0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
  0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
  0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
  0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
  0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
  0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
  0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
  0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
  0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
  0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
  0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
  0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
  0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
  0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
  0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
  0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
  0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
  0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
  0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
  0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
  0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
  0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
  0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/* Variables used by the China Telecom code to determine 
if registration is necessary. 
*/
static struct imsi_data
{
  nv_imsi_mcc_type                     imsi_mcc;
  nv_imsi_11_12_type                   imsi_11_12;
  nv_min2_type                         min2;
  nv_min1_type                         min1;
} cm_auto_reg_efs_imsi_data = { { 0, 0 }, { 0, 0 }, { 0, {0, 0} }, { 0, {0, 0} } },
cm_auto_reg_ruim_imsi_data = { { 0, 0 }, { 0, 0 }, { 0, {0, 0} }, { 0, {0, 0} } };

static qword cm_auto_reg_nv_meid = {0,0};

static boolean cm_auto_reg_sms_sent = FALSE;
static byte  cm_auto_reg_nv_imsi[CM_AUTO_REG_EFS_CU_IMSI_LEN];

static sys_sys_mode_e_type current_ss_mode = SYS_SYS_MODE_NONE;
static timer_type                    *cm_auto_reg_timer = NULL;


/* Variables used by the China Telecom code 
to send a registration message. 
*/
static wms_client_id_type              cm_auto_reg_wms_cid = (wms_client_id_type) WMS_CLIENT_TYPE_MAX;
static wms_client_message_s_type       *cm_auto_reg_wms_msg = NULL;
static cm_auto_reg_cmds_e_type         cm_auto_reg_cmds = CM_AUTO_REG_INFO_CLEAR_CMD;
static uint8                           cm_auto_reg_send_sms_retry = 0;
static cm_device_model_info_efs        *cm_auto_reg_efs_data = NULL;
static uint32                          cm_auto_reg_imsi_mcc = 0;

/*===========================================================================
                       Functions
=========================================================================== */

/*===========================================================================

FUNCTION cm_auto_reg_crc_32_calc_reg

===========================================================================*/
static uint32 cm_auto_reg_crc_32_calc_reg
(
      /* Pointer to data over which to compute CRC */
  uint8  *buf_ptr,

  /* Number of bits over which to compute CRC */
  uint16  len
);


/*===========================================================================
FUNCTION cm_auto_reg_info_send_cu_8bit

DESCRIPTION
  Called when send the AUTO_REG_SMS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_info_send_cu_8bit()
{
  wms_client_err_e_type client_err;
  uint8 SM[WMS_MAX_LEN];
  uint8 pos_sm = 0;
  uint8 s_IMEI[24] = "IMEI:";
  uint8 size_imei = 0;
  uint8 s_IMSI[8*2];
  int bcd_index = 0;
  int ascii_index = 0;
  uint8 asc_1 = 0;
  uint8 asc_2 = 0;
  uint8 temp_pos_sm = 0;
  
  uint8 pdu[] = {0x41,0x00};
  /*0x41 -> Msg Type Ind =  SMS-SUBMIT, UserDataInd = TRUE*/
  uint8 num_type = 0x81;
  /*0x81 -> TypeOfNumber=Unknown,NumPlanId=ISDN*/
  #ifdef FEATURE_QRD_CU_AUTO_REGISTER_MESSAGE_TEST
  uint8 address[] = "10655464";
  #else
  uint8 address[] = "10655459";
  #endif

  uint8 ud_head[] = {0x06,0x05,0x04,0x68,0x38,0x68,0x38};
  unsigned int i = 0;
  uint16 pos_data=0, pos_udl;
  
  /* get the UE information */
  if ( !cm_auto_reg_efs_model_data())
  {
    CM_ERR_0("cm_auto_reg_efs_model_data() - failed");
	
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }

  
  /* IMEI */
  size_imei = (uint8) cm_auto_reg_get_imei( &s_IMEI[5], ((ARR_SIZE(s_IMEI) - 5)*sizeof(s_IMEI[0])) );
  s_IMEI[size_imei + 5] = 0;
  
  temp_pos_sm = pos_sm + (uint8)strlen( (char *)s_IMEI) ;
  if(temp_pos_sm < WMS_MAX_LEN)
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), s_IMEI, strlen( (char *)s_IMEI) );
    pos_sm += (uint8) strlen( (char *)s_IMEI);
    SM[pos_sm++] = '/';
  }
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
  
  /* IMSI */
  /*
  ** THIS IS A VALID IMSI, SO CONVERT IT TO ASCII
  */
  asc_1 = (cm_auto_reg_nv_imsi[bcd_index] & 0xF0);
  asc_1 = asc_1 >> 4;
  s_IMSI[ascii_index++] = (asc_1 <= 9)? (asc_1+'0')
                                    : ( (asc_1-10)+'A');
  for ( bcd_index = 1; bcd_index < (int) REG_NV_IMSI_MAX_LENGTH; bcd_index++ )
  {
    asc_1 = (cm_auto_reg_nv_imsi[bcd_index] & 0x0F);
    asc_2 = (cm_auto_reg_nv_imsi[bcd_index] & 0xF0) >> 4;
    s_IMSI[ascii_index++] = (asc_1 <= 9)? (asc_1+'0')
                                            :( (asc_1-10)+'A');
    s_IMSI[ascii_index++] = (asc_2 <= 9)? (asc_2+'0')
                                            : ( (asc_2-10)+'A');
  }
  s_IMSI[ascii_index] = 0;

  temp_pos_sm = pos_sm + (uint8) strlen( (char *)s_IMSI);
  if(temp_pos_sm < WMS_MAX_LEN )
  {
    memscpy(&SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), s_IMSI, MIN(sizeof(s_IMSI),strlen( (char *)s_IMSI)));
    pos_sm += (uint8) strlen( (char *)s_IMSI);
    SM[pos_sm++] = '/';
  }
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;	
  }	
  
  /* MANUFACTURER NAME */
  temp_pos_sm = strlen(cm_auto_reg_efs_data->manufacturer_name);
  if(temp_pos_sm + pos_sm < WMS_MAX_LEN )
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), cm_auto_reg_efs_data->manufacturer_name, MIN(sizeof(cm_auto_reg_efs_data->manufacturer_name),strlen(cm_auto_reg_efs_data->manufacturer_name)));
    pos_sm += (uint8) strlen(cm_auto_reg_efs_data->manufacturer_name);
    SM[pos_sm++] = '/';
  } 
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
    
  /* DEVICE MODEL */
  temp_pos_sm = strlen(cm_auto_reg_efs_data->device_model);	
  if( temp_pos_sm + pos_sm < WMS_MAX_LEN )
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), cm_auto_reg_efs_data->device_model, MIN(sizeof(cm_auto_reg_efs_data->device_model),strlen(cm_auto_reg_efs_data->device_model)) );
    pos_sm += (uint8) strlen(cm_auto_reg_efs_data->device_model);
    SM[pos_sm++] = '/';
  }  
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
    
  /* SOFTWARE VERSION */
   temp_pos_sm = strlen(cm_auto_reg_efs_data->sw_version);
  if(temp_pos_sm + pos_sm < WMS_MAX_LEN)
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), cm_auto_reg_efs_data->sw_version, MIN(sizeof(cm_auto_reg_efs_data->sw_version),strlen(cm_auto_reg_efs_data->sw_version)) );
    pos_sm += (uint8) strlen(cm_auto_reg_efs_data->sw_version);
  
    SM[pos_sm] = 0;
  }else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
  /* Allocate MEM for SMS */
  if (NULL != cm_auto_reg_wms_msg)
  {
    cm_mem_free(cm_auto_reg_wms_msg);
    cm_auto_reg_wms_msg = NULL;
  }
  
  cm_auto_reg_wms_msg = (wms_client_message_s_type *)cm_mem_malloc(sizeof(wms_client_message_s_type));
  
  if (NULL == cm_auto_reg_wms_msg)
  {
    cm_auto_reg_dereg();
    sys_err_fatal_null_ptr_exception();
    return;
  }
  
  memset(cm_auto_reg_wms_msg, 0, sizeof(wms_client_message_s_type));

  /* Fill in the MSG header */
  cm_auto_reg_wms_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
  cm_auto_reg_wms_msg->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
  cm_auto_reg_wms_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
  cm_auto_reg_wms_msg->msg_hdr.index = WMS_DUMMY_MESSAGE_INDEX;
  
  cm_auto_reg_wms_msg->u.gw_message.is_broadcast = FALSE;
  cm_auto_reg_wms_msg->u.gw_message.sc_address.number_of_digits = 0;

  /* add TP-XX */
  for ( i = 0; i < sizeof(pdu); i++ )
  {
    cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data++] = pdu[i];
    /*Byte 0 -> Msg Type + User Def Header Ind*/
    /*Byte 1 -> Msg Reference*/
  }
  
  /* fill in the destinate addr */
  pos_data += 2;
  /*Byte 2-12 -> Dest Address */
  /*Byte 4-12 -> Dest Address Value*/
  cm_auto_reg_assic_to_byte((char *)address, &cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data], strlen((char *)address));
  pos_data += (uint16) cm_auto_reg_ceiling((int)strlen((char *)address),2);
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[2] = (uint8) strlen((char *)address);
  /*Byte3 -> NumberType+NumPlanId*/
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[3] = (uint8) num_type;
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data+1] = 0x4; /* for 8bit */ /*Byte12 with dummy value*/
  pos_data += 3; /*keep this space*/ /*Byte12 with dummy value above;Byte13->ProtocolId;Byte14->DataCodingScheme*/
  pos_udl = pos_data - 1;
  
  /* add UDH */
  for ( i = 0; i < sizeof(ud_head); i++ )
  {
    cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data++] = ud_head[i];
  } 

  memscpy( &cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data],
           ((ARR_SIZE(cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data) - pos_data) * sizeof(cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[0])),
           &SM[0],
           strlen((char *)SM));
  
  pos_data += pos_sm--;

  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_udl] = (uint8)(sizeof(ud_head) + strlen((char *)SM));
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.format = WMS_FORMAT_GW_PP;
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.len = pos_data;
  
  /* Initialize the UI client type with the WMS task */
  client_err = wms_client_init(WMS_CLIENT_FLOATING1,  &cm_auto_reg_wms_cid );
  
  if (client_err == WMS_CLIENT_ERR_NONE) 
  {
    /* Register the configuration call back function */
    client_err = wms_client_reg_cfg_cb (cm_auto_reg_wms_cid, cm_auto_reg_receive_reg_cfg_cu_cb );
    
    if (WMS_CLIENT_ERR_NONE !=  client_err)
    {
      CM_MSG_HIGH_1("wms_status %d", client_err);
    }
    
    /* Reg callback for wms */
    client_err = wms_client_reg_msg_cb(cm_auto_reg_wms_cid, cm_auto_reg_receive_reg_msg_cu_cb);

    if (client_err == WMS_CLIENT_ERR_NONE) 
    {
      /* Activate the UI client type with the WMS task. */
      client_err = wms_client_activate(cm_auto_reg_wms_cid);
      if (client_err != WMS_CLIENT_ERR_NONE)
      {
        cm_auto_reg_dereg();
        CM_ERR_1("client_err %d", client_err);
      }
    }
    else 
    {
      cm_auto_reg_dereg();
      CM_ERR_1("wms_client_activate: client_err %d", client_err);
    }
  }
  else 
  {
    cm_auto_reg_dereg();
    CM_ERR_1("wms_client_init: client_err %d", client_err);
  }
}


/*===========================================================================
FUNCTION cm_auto_reg_info_send_cmcc_8bit

DESCRIPTION
  Called when send the AUTO_REG_SMS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_info_send_cmcc_8bit()
{
  wms_client_err_e_type client_err;
  uint8 SM[WMS_MAX_LEN];
  uint8 pos_sm = 0;
  uint8 s_IMEI[24] = "IMEI:";
  uint8 size_imei = 0;
  uint8 temp_pos_sm = 0;



  uint8 pdu[] = {0x41,0x00};
  /*0x41 -> Msg Type Ind =  SMS-SUBMIT, UserDataInd = TRUE*/
  uint8 num_type = 0x81;
  /*0x81 -> TypeOfNumber=Unknown,NumPlanId=ISDN*/
  uint8 address[] = "10654040";

  uint8 ud_head[] = {0x06,0x05,0x04,0x42,0x66,0x42,0x66};
  unsigned int i = 0;
  uint16 pos_data=0, pos_udl;

  
  /* get the UE information */
  if ( !cm_auto_reg_efs_model_data())
  {
    CM_ERR_0("cm_auto_reg_efs_model_data() - failed");

    
    CM_ERR_0("CMCC : Auto Register Message not Sent");
	return;
  }

    
  /* IMEI */
  size_imei = (uint8) cm_auto_reg_get_imei( &s_IMEI[5], ((ARR_SIZE(s_IMEI) - 5)*sizeof(s_IMEI[0])) );
  s_IMEI[size_imei + 5] = 0;
  
  temp_pos_sm = strlen( (char *)s_IMEI);
  if(temp_pos_sm + pos_sm < WMS_MAX_LEN)  
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), s_IMEI, strlen( (char *)s_IMEI) );
    pos_sm += (uint8) strlen( (char *)s_IMEI);
    SM[pos_sm++] = '/';
  }
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
  
  /* IMSI */
  /* Not including IMSI in the Auto-registration-SMS for CMCC.
    ( IMEI/MANUFACTURE_NAME/DEVICE_MODEL/SOFTWARE_VERSION is for CMCC. )
   */
   
  /* MANUFACTURER NAME */
  temp_pos_sm = strlen(cm_auto_reg_efs_data->manufacturer_name) ;
  if(temp_pos_sm + pos_sm < WMS_MAX_LEN)  
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), cm_auto_reg_efs_data->manufacturer_name, MIN(sizeof(cm_auto_reg_efs_data->manufacturer_name),strlen(cm_auto_reg_efs_data->manufacturer_name)) );
    pos_sm += (uint8) strlen(cm_auto_reg_efs_data->manufacturer_name);
    SM[pos_sm++] = '/';
  } 
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
    
  /* DEVICE MODEL */
  temp_pos_sm = strlen(cm_auto_reg_efs_data->device_model);
  if( temp_pos_sm + pos_sm < WMS_MAX_LEN )
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), cm_auto_reg_efs_data->device_model, MIN(sizeof(cm_auto_reg_efs_data->device_model),strlen(cm_auto_reg_efs_data->device_model)) );
    pos_sm += (uint8) strlen(cm_auto_reg_efs_data->device_model);
    SM[pos_sm++] = '/';
  }
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	
    
  /* SOFTWARE VERSION */
  temp_pos_sm = strlen(cm_auto_reg_efs_data->sw_version);
  if(temp_pos_sm + pos_sm < WMS_MAX_LEN)  
  {
    memscpy( &SM[pos_sm], ((ARR_SIZE(SM) - pos_sm) * sizeof(SM[0])), cm_auto_reg_efs_data->sw_version, MIN(sizeof(cm_auto_reg_efs_data->sw_version),strlen(cm_auto_reg_efs_data->sw_version)) );
    pos_sm += (uint8) strlen(cm_auto_reg_efs_data->sw_version);
  
    SM[pos_sm] = 0;
  }
  else
  {
    CM_ERR_0("CU : Auto Register Message not Sent");
	return;
  }	

  /* Allocate MEM for SMS */
  if (NULL != cm_auto_reg_wms_msg)
  {
    cm_mem_free(cm_auto_reg_wms_msg);
    cm_auto_reg_wms_msg = NULL;
  }
  
  cm_auto_reg_wms_msg = (wms_client_message_s_type *)cm_mem_malloc(sizeof(wms_client_message_s_type));
  
  if (NULL == cm_auto_reg_wms_msg)
  {
    cm_auto_reg_dereg();
    sys_err_fatal_null_ptr_exception();
    return;
  }
  
  memset(cm_auto_reg_wms_msg, 0, sizeof(wms_client_message_s_type));

  /* Fill in the MSG header */
  cm_auto_reg_wms_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
  cm_auto_reg_wms_msg->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
  cm_auto_reg_wms_msg->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
  cm_auto_reg_wms_msg->msg_hdr.index = WMS_DUMMY_MESSAGE_INDEX;
  
  cm_auto_reg_wms_msg->u.gw_message.is_broadcast = FALSE;
  cm_auto_reg_wms_msg->u.gw_message.sc_address.number_of_digits = 0;

  /* add TP-XX */
  for ( i = 0; i < sizeof(pdu); i++ )
  {
    cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data++] = pdu[i];
    /*Byte 0 -> Msg Type + User Def Header Ind*/
    /*Byte 1 -> Msg Reference*/
  }
  
  /* fill in the destinate addr */
  pos_data += 2;
  /*Byte 2-12 -> Dest Address */
  /*Byte 4-12 -> Dest Address Value*/
  cm_auto_reg_assic_to_byte((char *)address, &cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data], strlen((char *)address));
  pos_data += (uint16) cm_auto_reg_ceiling((int)strlen((char *)address),2);
  /*Byte 2 -> Address Length*/
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[2] = (uint8) strlen((char *)address);
  /*Byte3 -> NumberType+NumPlanId*/
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[3] = (uint8) num_type;
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data+1] = 0x4; /* for 8bit */ /*Byte12 with dummy value*/
  pos_data += 3; /*keep this space*/ /*Byte12 with dummy value above;Byte13->ProtocolId;Byte14->DataCodingScheme*/
  pos_udl = pos_data - 1;
  
  /* add UDH */
  for ( i = 0; i < sizeof(ud_head); i++ )
  {
    cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data++] = ud_head[i];
  } 

  memscpy( &cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_data],
           ((ARR_SIZE(cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data) - pos_data) * sizeof(cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[0])),
           &SM[0],
           strlen((char *)SM) );
  
  pos_data += pos_sm--;

  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.data[pos_udl] = (uint8)(sizeof(ud_head) + strlen((char *)SM));
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.format = WMS_FORMAT_GW_PP;
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
  cm_auto_reg_wms_msg->u.gw_message.raw_ts_data.len = pos_data;
  
  /* Initialize the UI client type with the WMS task */
  client_err = wms_client_init(WMS_CLIENT_FLOATING1,  &cm_auto_reg_wms_cid );
  
  if (client_err == WMS_CLIENT_ERR_NONE) 
  {
    /* Register the configuration call back function */
    client_err = wms_client_reg_cfg_cb (cm_auto_reg_wms_cid, cm_auto_reg_receive_reg_cfg_cmcc_cb );
    
    if (WMS_CLIENT_ERR_NONE !=  client_err)
    {
      CM_MSG_HIGH_1("wms_status %d", client_err);
    }
    
    /* Reg callback for wms */
    client_err = wms_client_reg_msg_cb(cm_auto_reg_wms_cid, cm_auto_reg_receive_reg_msg_cmcc_cb);

    if (client_err == WMS_CLIENT_ERR_NONE) 
    {
      /* Activate the UI client type with the WMS task. */
      client_err = wms_client_activate(cm_auto_reg_wms_cid);
      if (client_err != WMS_CLIENT_ERR_NONE)
      {
        cm_auto_reg_dereg();
        CM_ERR_1("client_err %d", client_err);
      }
    }
    else 
    {
      cm_auto_reg_dereg();
      CM_ERR_1("wms_client_activate: client_err %d", client_err);
    }
  }
  else 
  {
    cm_auto_reg_dereg();
    CM_ERR_1("wms_client_init: client_err %d", client_err);
  }
}



/*===========================================================================
FUNCTION cm_auto_reg_assic_to_byte

DESCRIPTION
  Convert an SMS PDU from ASCII hex format to a byte array.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_assic_to_byte
(
  char * hex_pdu,
  byte * byte_pdu,
  uint32 num_hex_chars
)
{
  uint32 i;
  uint8 temp = 0;

  for (i=0; i < num_hex_chars; i++)
  {
    temp = cm_auto_reg_hex_char_to_byte(hex_pdu[i]);
    if(i%2 == 0)
    {
      byte_pdu[i/2] = temp;
    }
    else
    {
      byte_pdu[i/2] |= (temp << 4);
    }
  }
  if(i%2 != 0)
  {
    byte_pdu[i/2] |= 0xf0;
  }
}

/*===========================================================================
FUNCTION cm_auto_reg_hex_char_to_byte

DESCRIPTION
 Convert a hex character to a byte

DEPENDENCIES

SIDE EFFECTS
 None
===========================================================================*/

byte cm_auto_reg_hex_char_to_byte
(
  char hex_char
)
{
  if (hex_char >= 'A' && hex_char <= 'Z')
  {
    hex_char = hex_char + 'a' - 'A';
  }

  if (hex_char >= 'a' && hex_char <= 'f')
  {
    return (char)( (hex_char - 'a') + 10);
  }
  else if (hex_char >= 'A' && hex_char <= 'F')
  {
    return (char)( (hex_char - 'A') + 10);
  }
  else if (hex_char >= '0' && hex_char <= '9')
  {
    return (char)(hex_char-'0');
  }
  else
  {
    return 0;
  }
}

/*===========================================================================
FUNCTION cm_auto_reg_ceiling

DESCRIPTION
  Return the celing of divident/divisor.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

int cm_auto_reg_ceiling(int divident,int divisor)
{
    int result;
    result = divident/divisor;
    if (result*divisor != divident)
        result = result + 1;
    return result;
}

/*===========================================================================
FUNCTION cm_auto_reg_receive_msg_cfg_cu_cb

DESCRIPTION
  Called when received wms cfg event.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_cfg_cu_cb
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
)
{
  wms_status_e_type wms_status = WMS_OK_S;

  CM_MSG_HIGH_1("cm_auto_reg_receive_msg_cfg_cu_cb, event:%d", event);

  switch ( event )
  {
    case WMS_CFG_EVENT_GW_READY:
      if ((cm_auto_reg_wms_cid == (wms_client_id_type) WMS_CLIENT_FLOATING1) && (NULL != cm_auto_reg_wms_msg))
      {
        wms_status  = wms_msg_send(cm_auto_reg_wms_cid, 
                                   cm_auto_reg_send_reg_msg_cu_cb, 
                                   0, 
                                   WMS_SEND_MODE_CLIENT_MESSAGE, 
                                   cm_auto_reg_wms_msg);
        CM_MSG_HIGH_1("cm_auto_reg_info_send: wms_status %d", wms_status);
      }
      break;

    default:
	  break;
  }
  SYS_ARG_NOT_USED (event_ptr);
  SYS_ARG_NOT_USED (wms_status);
  return;
}


/*===========================================================================
FUNCTION cm_auto_reg_receive_msg_cfg_cmcc_cb

DESCRIPTION
  Called when received wms cfg event.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_cfg_cmcc_cb
(
  wms_cfg_event_e_type            event,     /* Event ID              */
  wms_cfg_event_info_s_type *event_ptr /* Pointer to Event info */
)
{
  wms_status_e_type wms_status = WMS_OK_S;

  CM_MSG_HIGH_1("cm_auto_reg_receive_msg_cfg_cu_cb, event:%d", event);

  switch ( event )
  {
    case WMS_CFG_EVENT_GW_READY:
      if ((event_ptr!=NULL) && (event_ptr->ms_ready.as_id == SYS_MODEM_AS_ID_1) && 
          (cm_auto_reg_wms_cid == (wms_client_id_type) WMS_CLIENT_FLOATING1) && 
          (NULL != cm_auto_reg_wms_msg))
      {
          wms_status  = wms_msg_ms_send_ext(cm_auto_reg_wms_cid,
                                            SYS_MODEM_AS_ID_1,
                                   cm_auto_reg_send_reg_msg_cmcc_cb, 
                                   0, 
                                   WMS_SEND_MODE_CLIENT_MESSAGE, 
                                            cm_auto_reg_wms_msg,
                                            WMS_MSG_TRANSPORT_TYPE_CS);

        CM_MSG_HIGH_1("cm_auto_reg_info_send: wms_status %d", wms_status);
      }
      break;

    default:
      CM_MSG_HIGH_1("unable to map, event:%d", event);
  }
  SYS_ARG_NOT_USED (event_ptr);
  SYS_ARG_NOT_USED (wms_status);
  return;
}


/*===========================================================================
FUNCTION cm_auto_reg_receive_reg_msg_cu_cb

DESCRIPTION
  Called when a UI received MT SMS or Report.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_msg_cu_cb
(
  wms_msg_event_e_type         event,
  /* event type */
  wms_msg_event_info_s_type    *info_ptr,
  /* event information */
  boolean                      *shared
)
{
  boolean efs_status;

  if(info_ptr == NULL)
  {
     CM_ERR_0("info_ptr NULL");
     return;
  }

  switch ( event )
  {
    case  WMS_MSG_EVENT_SUBMIT_REPORT:
      CM_MSG_HIGH_1("write_efs_imsi() event = %d", event);
      cm_auto_reg_dereg();

      /* Write the IMSI information to the on-board EFS. */
      efs_status = cmefs_write( CMEFS_CU_IMSI,
                          (byte*)cm_auto_reg_nv_imsi,
                          CM_AUTO_REG_EFS_CU_IMSI_LEN);
      if (!efs_status) 
      {
        CM_ERR_1("write nv s1 error 0x%X", 
                  efs_status);  
      }

      break;
    
    default:  	
      break;
  }
  SYS_ARG_NOT_USED (shared);
}


/*===========================================================================
FUNCTION cm_auto_reg_receive_reg_msg_cmcc_cb

DESCRIPTION
  Called when a UI received MT SMS or Report.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_msg_cmcc_cb
(
  wms_msg_event_e_type         event,
  /* event type */
  wms_msg_event_info_s_type    *info_ptr,
  /* event information */
  boolean                      *shared
)
{
  boolean efs_status;

  if(info_ptr == NULL)
  {
     CM_ERR_0("info_ptr NULL");
     return;
  }

  switch ( event )
  {
    case  WMS_MSG_EVENT_SUBMIT_REPORT:
      CM_MSG_HIGH_1("WMS_MSG_EVENT_SUBMIT_REPORT recv with status %d", info_ptr->submit_report_info.report_status);

      if(info_ptr->submit_report_info.report_status == WMS_RPT_OK)
      {
      CM_MSG_HIGH_0("write_efs_imsi()");  
      cm_auto_reg_dereg();
      /* Write the IMSI information to the on-board EFS. */
      efs_status = cmefs_write( CMEFS_CMCC_IMSI,
                          (byte*)cm_auto_reg_nv_imsi,
                          CM_AUTO_REG_EFS_CMCC_IMSI_LEN);
      if (!efs_status) 
      {
        CM_ERR_1("write nv s1 error 0x%X", 
                  efs_status);  
      }
      }
      break;
    
    default:  	
      break;
  }
  SYS_ARG_NOT_USED (shared);
}


/*===========================================================================
FUNCTION cm_auto_reg_dereg

DESCRIPTION
  Called when a UI to de-register.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_dereg()
{
  cmph_s_type              *ph_ptr = cmph_ptr();


  if ( ( current_ss_mode == SYS_SYS_MODE_CDMA) &&
	    ( cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CT)
          
     )
  {
      /* This time is used only for CT */

	  /*Clear the timer */
	  	  
      if(cm_auto_reg_timer != NULL)
      {
        CM_MSG_HIGH_0("ui dereg cm_auto_reg_timer ");
        (void) timer_clr(cm_auto_reg_timer,T_SEC);
      }
  }
  /* Clear the wait for ACK command. */
  cm_auto_reg_cmds = CM_AUTO_REG_INFO_COMPLETE_CMD;

  // Deactivate the client from WMS, if it is active .  
  if(cm_auto_reg_wms_cid == (wms_client_id_type) WMS_CLIENT_FLOATING1)
  {
    CM_MSG_HIGH_0("dereg from WMS ");
    if( wms_client_reg_msg_cb(cm_auto_reg_wms_cid,NULL) != WMS_CLIENT_ERR_NONE )
	{
      CM_ERR_0("wms_client_reg_msg_cb() failed");  
	}
    if( wms_client_reg_cfg_cb(cm_auto_reg_wms_cid,NULL) != WMS_CLIENT_ERR_NONE )
	{
      CM_ERR_0("wms_client_reg_cfg_cb() failed");  
	}

    if( wms_client_deactivate(cm_auto_reg_wms_cid) != WMS_CLIENT_ERR_NONE )
	{
      CM_ERR_0("wms_client_deactivate() failed");  
	}

    if( wms_client_release(cm_auto_reg_wms_cid) != WMS_CLIENT_ERR_NONE )
	{
      CM_ERR_0("wms_client_release() failed");  
	}

  }

  /*Deallocate the mem*/
  if(cm_auto_reg_efs_data != NULL)
  {
    cm_mem_free(cm_auto_reg_efs_data);
    cm_auto_reg_efs_data=NULL;
  }
  if(cm_auto_reg_timer != NULL)
  {
    timer_undef(cm_auto_reg_timer);
    cm_mem_free(cm_auto_reg_timer);
    cm_auto_reg_timer=NULL;
  }
}

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_msg_cu_cb

DESCRIPTION
  Called when a UI message has been sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_send_reg_msg_cu_cb
(
  wms_cmd_id_e_type          cmd,       /* command status being reported */
  void                      *user_data, /* data passed to send request */
  wms_cmd_err_e_type         cmd_err    /* the status of the command */
)
{
  wms_status_e_type wms_status = WMS_OK_S;

  CM_MSG_HIGH_1("cm_auto_reg_send_reg_msg_cu_cb cmd_err = %d", cmd_err);
  /* If the registration message is successfully sent... */
  if (cmd_err != WMS_CMD_ERR_NONE) 
  {
    /*No Retries for each Attempt*/    
      cm_auto_reg_dereg();
  }
  SYS_ARG_NOT_USED (cmd);
  SYS_ARG_NOT_USED (user_data);
  SYS_ARG_NOT_USED (wms_status);
}


/*===========================================================================
FUNCTION cm_auto_reg_send_reg_msg_cmcc_cb

DESCRIPTION
  Called when a UI message has been sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_send_reg_msg_cmcc_cb
(
  wms_cmd_id_e_type          cmd,       /* command status being reported */
  void                      *user_data, /* data passed to send request */
  wms_cmd_err_e_type         cmd_err    /* the status of the command */
)
{
  wms_status_e_type wms_status = WMS_OK_S;

  CM_MSG_HIGH_1("cm_auto_reg_send_reg_msg_cmcc_cb cmd_err = %d", cmd_err);
  /* If the registration message is successfully sent... */
  if (cmd_err != WMS_CMD_ERR_NONE) 
  {
    /* No retries */
    cm_auto_reg_dereg();
    
  }
  SYS_ARG_NOT_USED (cmd);
  SYS_ARG_NOT_USED (user_data);
  SYS_ARG_NOT_USED (wms_status);
}



/*===========================================================================
FUNCTION cm_auto_reg_efs_model_data

DESCRIPTION
  Called to read device model info from EFS.
  
DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

boolean cm_auto_reg_efs_model_data()
{
  cm_device_model_info_efs *model_ptr;
  cmph_s_type           *ph_ptr = cmph_ptr();

  if( cm_auto_reg_efs_data == NULL)
  {
    cm_auto_reg_efs_data = (cm_device_model_info_efs *)cm_mem_malloc(sizeof(cm_device_model_info_efs));

	if(cm_auto_reg_efs_data == NULL)
    {
      sys_err_fatal_null_ptr_exception();
      return FALSE;
    }
  }
  
  memset(cm_auto_reg_efs_data,0x0,sizeof(cm_device_model_info_efs));

  if(   ( cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CT) ||
        ( cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CU) ||
        #ifdef FEATURE_MMODE_DUAL_SIM
        ( cm_get_operator_info(SYS_MODEM_AS_ID_2) == OPERATOR_CU) ||
        #endif
        ( cm_get_operator_info(SYS_MODEM_AS_ID_1) == OPERATOR_CMCC) 
        #ifdef FEATURE_MMODE_DUAL_SIM
        || ( cm_get_operator_info(SYS_MODEM_AS_ID_2) == OPERATOR_CMCC)
        #endif
    )
  {
    if (cm_model_initialized != DEV_MODEL_INIT_FULL)/* make sure cm model had been initialized before*/
    {
      CM_ERR_0("CM Model Initialisation had failed...");
      return FALSE;
    }

    model_ptr = cm_get_device_info(); 

    /*Copy the device info into efs */
    memscpy(cm_auto_reg_efs_data,sizeof(cm_device_model_info_efs),model_ptr,sizeof(*model_ptr));
  }
  
  else
  {
    CM_MSG_HIGH_0("Auto Register not triggered as Feature not enabled");    
	return FALSE;
  }
  CM_MSG_HIGH_0("cm_auto_reg_efs_model_data read success");

  return TRUE;
}

/*===========================================================================
FUNCTION   cm_auto_reg_ss_event_mdm_cb

DESCRIPTION
  This function is called when we get an SS event.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Updates LED announciator behavior
===========================================================================*/
void cm_auto_reg_ss_event_mdm_cb(cm_ss_event_e_type ss_event,
                      const cm_mm_ss_info_s_type *ii_info_ptr)
{

  boolean sim_changed = TRUE;
  boolean is_cu_card = FALSE;
  sys_modem_as_id_e_type asid = SYS_MODEM_AS_ID_1;
  cmph_s_type *ph_ptr = cmph_ptr();


  boolean is_cmcc_card = FALSE;   /* CMCC AUTO REGISTER MSG */
  byte cu_mcc_mnc1[3] = {0x49,0x06,0x10};
  byte cu_mcc_mnc2[3] = {0x49,0x06,0x60};
  byte cmcc_mcc_mnc1[3] = {0x49,0x06,0x00};
  byte cmcc_mcc_mnc2[3] = {0x49,0x06,0x20};
  byte cmcc_mcc_mnc3[3] = {0x49,0x06,0x70};
  
  
  if (ii_info_ptr == NULL)
  {
    CM_MSG_HIGH_0("cm_auto_reg_ss_event_cb_m SC F");
    return;
  }

  CM_MSG_HIGH_3("SS EM CB sys_mode : %x srv_status: %x srv_domain: %x", ii_info_ptr->sys_mode, \
            ii_info_ptr->srv_status, ii_info_ptr->srv_domain);

  CM_MSG_HIGH_5("SS EM CB HYBR2 sys_mode : %x srv_status: %x srv_domain: %x. fmode(as_id_1):%d Operator is %d ", ii_info_ptr->gw_sys_mode, \
            ii_info_ptr->gw_srv_status, ii_info_ptr->gw_srv_domain,
            ph_ptr->subs_feature_mode[SYS_MODEM_AS_ID_1], cm_get_operator_info(asid));

  /* If the card has registered with a new network, check to see if the
   * SMS auto registration message should be sent. */
  

   /* If the card has registered with a new network, check to see if the 
    * SMS auto registration message should be sent. */
  if (( ii_info_ptr->sys_mode == SYS_SYS_MODE_CDMA ) &&
      ( ii_info_ptr->sys_id.id_type == SYS_SYS_ID_TYPE_IS95 ) && 
      ( ii_info_ptr->srv_status == SYS_SRV_STATUS_SRV ) )
  {

    if ( cm_get_operator_info(asid) == OPERATOR_CT )
    {
      CM_MSG_HIGH_2("cm_auto_reg_cmds:%d, cm_auto_reg_imsi_mcc:%d",cm_auto_reg_cmds, cm_auto_reg_imsi_mcc);
      if ( cm_auto_reg_cmds & CM_AUTO_REG_INFO_CLEAR_CMD)
      {

        if (!cm_is_operator_ct(SYS_MODEM_AS_ID_1))
	  {
            return;
          }
      }
    }

  else
  {
    CM_ERR_1("Operator EFS : %d. Auto Register not initiated ",cm_get_operator_info(asid));
    return;
  }

	CM_MSG_HIGH_1("cm_auto_reg_cmds:%d",cm_auto_reg_cmds);
	
    /* The WAIT command is not cleared so the SMS message will be queued 
    * only once per power up. */
    
    if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_WAIT_CMD)
    {
      /* The CTS command is set after the 60 second timer has expired, and
       * the modem is determined to be in the correct network. */
      if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_CTS_CMD)
      {
        /* Clear the CTS command and set the command to send the message. */
        cm_auto_reg_cmds &= ~CM_AUTO_REG_INFO_CTS_CMD;
        cm_auto_reg_cmds |= CM_AUTO_REG_INFO_SEND_CMD;
      	CM_MSG_HIGH_1("cm_auto_reg_cmds:%d,CM_AUTO_REG_INFO_SEND_CMD",cm_auto_reg_cmds);				
        handleAutoRegister(0);    // Sending invalid value 0 as this is not a Call Back
      }
    }
    else 
    {
      /* Set the CHECK command only if the ESN tracking process is not already completed once so that the UI code checks to see if the
      * SMS message needs to be sent. */
      if(!(cm_auto_reg_cmds & CM_AUTO_REG_INFO_COMPLETE_CMD))
      {
         cm_auto_reg_cmds |= CM_AUTO_REG_INFO_CHECK_CMD;
         CM_MSG_HIGH_1("cm_auto_reg_cmds:%d,CM_AUTO_REG_INFO_CHECK_CMD",cm_auto_reg_cmds);				
	 handleAutoRegister(0);    // Sending invalid value 0 as this is not a Call Back
      }
    }
  }
  

  /* Get current mode, if it's CDMA mode. Not for CU */
  current_ss_mode = ii_info_ptr->sys_mode;
  if(current_ss_mode == SYS_SYS_MODE_CDMA)
  {
    return;
  }

  if ( ss_event == CM_SS_EVENT_SRV_CHANGED &&
  	   ( ii_info_ptr->sys_mode == SYS_SYS_MODE_GW ||
  	    ii_info_ptr->sys_mode == SYS_SYS_MODE_WCDMA ||
  	    ii_info_ptr->sys_mode == SYS_SYS_MODE_GSM ) &&
  	   ( ii_info_ptr->srv_status == SYS_SRV_STATUS_SRV ) &&
  	   ( ii_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_ONLY ||
  	     ii_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS ) &&
  	     !cm_auto_reg_sms_sent )
  {

    if ( ( cm_model_initialized == DEV_MODEL_INIT_FULL) && ( cm_get_operator_info(asid) != OPERATOR_CU ) && (cm_get_operator_info(asid) != OPERATOR_CMCC ))
    {
      cm_auto_reg_dereg();
      return;
    }

	if( cm_get_operator_info(asid) == OPERATOR_CU )
    {
      cmph_getNVImsi(cm_auto_reg_nv_imsi);

      is_cu_card = ( ( ((cu_mcc_mnc1[0]&0xf0) == (cm_auto_reg_nv_imsi[0]&0xf0)) &&
                       (cu_mcc_mnc1[1] == cm_auto_reg_nv_imsi[1]) &&
                       (cu_mcc_mnc1[2] == cm_auto_reg_nv_imsi[2])) ||
                     ( ((cu_mcc_mnc2[0]&0xf0) == (cm_auto_reg_nv_imsi[0]&0xf0)) &&
                       (cu_mcc_mnc2[1] == cm_auto_reg_nv_imsi[1]) &&
                       (cu_mcc_mnc2[2] == cm_auto_reg_nv_imsi[2]) ) );
    
      sim_changed = (boolean)(memcmp(cm_auto_reg_efs_cu_imsi, cm_auto_reg_nv_imsi, CM_AUTO_REG_EFS_CU_IMSI_LEN) != 0);
     
      CM_MSG_HIGH_2("is_cu_card=%d, sim_changed=%d", is_cu_card, sim_changed);

      if ( is_cu_card && sim_changed )
      {
        cm_auto_reg_cmds |= CM_AUTO_REG_INFO_CU_SEND_CMD;

        handleAutoRegister(0);    /* Sending invalid value 0 as this is not a Call Back */
        return;

      }
      else
      {
        cm_auto_reg_dereg();
      }
	}
  }
  /*FEATURE_QRD_CU_AUTO_REGISTER_MESSAGE*/

  /* CMCC AUTO REGISTER MESSAGE */

  if ( ss_event == CM_SS_EVENT_SRV_CHANGED &&
       ( ( ( ii_info_ptr->sys_mode == SYS_SYS_MODE_GW   ||
  	    ii_info_ptr->sys_mode == SYS_SYS_MODE_WCDMA ||
  	    ii_info_ptr->sys_mode == SYS_SYS_MODE_GSM   ||
            ii_info_ptr->sys_mode == SYS_SYS_MODE_TDS   ||
            ii_info_ptr->sys_mode == SYS_SYS_MODE_LTE) &&
  	   ( ii_info_ptr->srv_status == SYS_SRV_STATUS_SRV ) &&
  	   ( ii_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_ONLY ||
               ii_info_ptr->srv_domain == SYS_SRV_DOMAIN_CS_PS ) 
         )||
         ( (ph_ptr->subs_feature_mode[SYS_MODEM_AS_ID_1] == SYS_SUBS_FEATURE_MODE_SGLTE)&&
           (ii_info_ptr->gw_sys_mode == SYS_SYS_MODE_GSM)&&
           (ii_info_ptr->gw_srv_status == SYS_SRV_STATUS_SRV)
         )
       )&&
  	     !cm_auto_reg_sms_sent )
  {

    if ( (cm_model_initialized == DEV_MODEL_INIT_FULL) && ( cm_get_operator_info(asid) != OPERATOR_CU ) && (cm_get_operator_info(asid) != OPERATOR_CMCC ) )
    {
      cm_auto_reg_dereg();
      return;
    }

    if ( cm_get_operator_info(asid) == OPERATOR_CMCC )
    {
      cmph_getNVImsi(cm_auto_reg_nv_imsi);

      is_cmcc_card = ( ( ((cmcc_mcc_mnc1[0]&0xf0) == (cm_auto_reg_nv_imsi[0]&0xf0)) &&
                          (cmcc_mcc_mnc1[1] == cm_auto_reg_nv_imsi[1]) &&
                          (cmcc_mcc_mnc1[2] == cm_auto_reg_nv_imsi[2]) ) ||
                       ( ((cmcc_mcc_mnc2[0]&0xf0) == (cm_auto_reg_nv_imsi[0]&0xf0)) &&
                          (cmcc_mcc_mnc2[1] == cm_auto_reg_nv_imsi[1]) &&
                          (cmcc_mcc_mnc2[2] == cm_auto_reg_nv_imsi[2]) ) ||
                       ( ((cmcc_mcc_mnc3[0]&0xf0) == (cm_auto_reg_nv_imsi[0]&0xf0)) &&
                          (cmcc_mcc_mnc3[1] == cm_auto_reg_nv_imsi[1]) &&
                          (cmcc_mcc_mnc3[2] == cm_auto_reg_nv_imsi[2]) )
                     );
    
      sim_changed = (boolean)(memcmp(cm_auto_reg_efs_cmcc_imsi, cm_auto_reg_nv_imsi, CM_AUTO_REG_EFS_CMCC_IMSI_LEN) != 0);
    
      CM_MSG_HIGH_2("is_cmcc_card=%d, sim_changed=%d", is_cmcc_card, sim_changed);

      if ( is_cmcc_card && sim_changed )
      {
        cm_auto_reg_cmds |= CM_AUTO_REG_INFO_CMCC_SEND_CMD;

        handleAutoRegister(0);    /* Sending invalid value 0 as this is not a Call Back */

      }
      else
      {
        cm_auto_reg_dereg();
      }
    }

  }
  
}

/*===========================================================================
FUNCTION cm_auto_reg_info_check

DESCRIPTION
  Called when the SIM initialization has been competed to send a registration
  SMS message, if necessary.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_info_check(void)
{

  nv_item_type nvi;
	
  /* If the SMS message hasn't been sent before... */
  if (!(cm_auto_reg_cmds & CM_AUTO_REG_INFO_WAIT_CMD))
  {
    /* ...see if an SMS auto registration message is needed. */
    CM_MSG_HIGH_1("cm_auto_reg_cmds:%d- not CM_AUTO_REG_INFO_WAIT_CMD",cm_auto_reg_cmds);				

    /* Set the WAIT command so that the CHECK command is not called again. */
    cm_auto_reg_cmds |= CM_AUTO_REG_INFO_WAIT_CMD;
	
    /* Read the IMSI/MEID/ESN items stored on the RUIM. */
    if (cm_auto_reg_read_ruim_items(NV_RUIM_NAM) != NV_DONE_S)
    {
      /* If the RUIM items could not be read, the SMS message should not be
       * sent. */
      CM_ERR_1("cm_auto_reg_cmds:%d- Read RUIM NAM failed",cm_auto_reg_cmds);				
      return;
    }
	
    /* Read the IMSI/MEID/ESN items stored in the local NVRAM. */
    if (cm_auto_reg_read_efs_imsi() != TRUE)
    {
      /* If the EFS imsi could not be read, the SMS message should not be
       * sent. */
      CM_ERR_1("cm_auto_reg_cmds:%d- Read EFS IMSI failed. IMSI not configured in EFS. Proceed with RUIM value",cm_auto_reg_cmds);				
      
    }
    
	/* Read Manufacturer Equipment ID */
	
    cmnv_read_wait (NV_MEID_ME_I, &nvi);
	cm_auto_reg_nv_meid[0] = nvi.meid[0];
    cm_auto_reg_nv_meid[1] = nvi.meid[1];
    
    /* Read the user data from local efs */
    if(!cm_auto_reg_efs_model_data())
    {
      CM_ERR_0("cm_auto_reg_compare_ef_model_cb Read EFS MODEL failed, deregister it");
      cm_auto_reg_dereg();
      return;
    }
  
    /* If a previous registration was successful...*/
    if (cm_auto_reg_get_local_reg_ind())
    {
      /* If the items in the RUIM and EFS are the same, exit. */
      if (cm_auto_reg_compare_efs_ruim_items())
      {
        CM_MSG_HIGH_0("RUIM card matches. No Registration Required.");
        cm_auto_reg_dereg();
        return;
      }
      CM_MSG_HIGH_0("RUIM card differs. Registration Required.");
      cm_auto_reg_set_local_reg_ind(FALSE);
    }

    /*Allocate mem of timer*/
    cm_auto_reg_timer = (timer_type *)cm_mem_malloc(sizeof(timer_type));
    if(cm_auto_reg_timer == NULL)
    {
      sys_err_fatal_null_ptr_exception();
      return;
    }
    cm_auto_reg_set_timer(60, cm_auto_reg_cts_msg_timer_cb);
    
  }
}

/*===========================================================================
FUNCTION cm_auto_reg_read_ruim_items

DESCRIPTION
  Reads needed UI parameters from RUIM to use later to see if registration
  is necessary.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

nv_stat_enum_type cm_auto_reg_read_ruim_items(
  byte nam                            /* NAM to read RUIM items. */
)
{
  nv_item_type nvi;
  
  /* Read the IMSI information from the attached RUIM. */

  /* First, read the MCC value from the RUIM. */
  nvi.imsi_mcc.nam = nam;
  
  cmnv_read_wait (NV_IMSI_MCC_I, &nvi);
  
  cm_auto_reg_ruim_imsi_data.imsi_mcc = nvi.imsi_mcc;

  /* Next, read the MCN value from the RUIM. */
  nvi.imsi_11_12.nam = nam;
  
  CM_MSG_HIGH_3("cm_auto_reg_cmds:%d- Read RUIM imsi_mcc=%d- Read RUIM nam=%d",
  	cm_auto_reg_cmds, cm_auto_reg_ruim_imsi_data.imsi_mcc.imsi_mcc, nvi.imsi_11_12.nam);				
  
  cmnv_read_wait (NV_IMSI_11_12_I, &nvi);

  cm_auto_reg_ruim_imsi_data.imsi_11_12 = nvi.imsi_11_12;

  /* Next, read the MIN2 value from the RUIM. */
  nvi.min2.nam = nam;
  
  cmnv_read_wait (NV_MIN2_I, &nvi);
  
  cm_auto_reg_ruim_imsi_data.min2 = nvi.min2;
  nvi.min1.nam = nam;

  /* Next, read the MIN2 value from the RUIM. */
  
  cmnv_read_wait (NV_MIN1_I, &nvi);
  
  cm_auto_reg_ruim_imsi_data.min1 = nvi.min1;

  CM_MSG_HIGH_6("auto_reg_cmds %d: Read RUIM imsi_mcc_11_12 %d, RUIM min2[0] %d min[1] %d - min1[0] %d min[1] %d",
  	cm_auto_reg_cmds,cm_auto_reg_ruim_imsi_data.imsi_11_12.imsi_11_12,
  	cm_auto_reg_ruim_imsi_data.min2.min2[0],cm_auto_reg_ruim_imsi_data.min2.min2[1],
  	cm_auto_reg_ruim_imsi_data.min1.min1[0],cm_auto_reg_ruim_imsi_data.min1.min1[1]);	

  return NV_DONE_S;  
}

/*===========================================================================
FUNCTION cm_auto_reg_read_efs_imsi

DESCRIPTION
  Reads needed UI parameters from NVRAM to use later to see if registration
  is necessary.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_read_efs_imsi()
{
   
   int32  size = 0;

  /* Read the IMSI MCC information from the on-board EFS. */
  
  size = sizeof(cm_auto_reg_efs_imsi_data.imsi_mcc.imsi_mcc);
  if(!cmefs_read(CMEFS_IMSI_MCC, (byte*)&cm_auto_reg_efs_imsi_data.imsi_mcc.imsi_mcc, &size))
  {
    CM_ERR_1("cm_auto_reg_cmds:%d- Read CMEFS_IMSI_MCC failed",cm_auto_reg_cmds);				
    return FALSE;
  }

  size = sizeof(cm_auto_reg_efs_imsi_data.min1.min1[CDMAMIN]);
  if(!cmefs_read(CMEFS_IMSI_MIN1, (byte*)&cm_auto_reg_efs_imsi_data.min1.min1[CDMAMIN], &size))
  {
    CM_ERR_1("cm_auto_reg_cmds:%d- Read CMEFS_IMSI_MIN1 failed",cm_auto_reg_cmds);				
    return FALSE;   
  }

  size = sizeof(cm_auto_reg_efs_imsi_data.min2.min2[CDMAMIN]);
  if(!cmefs_read(CMEFS_IMSI_MIN2, (byte*)&cm_auto_reg_efs_imsi_data.min2.min2[CDMAMIN], &size))
  {
    CM_ERR_1("cm_auto_reg_cmds:%d- Read CMEFS_IMSI_MIN2 failed",cm_auto_reg_cmds);				
    return FALSE;   
  }

  size = sizeof(cm_auto_reg_efs_imsi_data.imsi_11_12.imsi_11_12);
  if(!cmefs_read(CMEFS_IMSI_11_12, (byte*)&cm_auto_reg_efs_imsi_data.imsi_11_12.imsi_11_12, &size))
  {
    CM_ERR_1("cm_auto_reg_cmds:%d- Read CMEFS_IMSI_11_12 failed",cm_auto_reg_cmds);				
    return FALSE;   
  }
   
  CM_MSG_HIGH_0("cm_auto_reg_read_efs_imsi(), read IMSI from efs complete");  

 return TRUE;
}



/*===========================================================================
FUNCTION cm_auto_reg_get_local_reg_ind

DESCRIPTION
  Reads local NV item to determine registration status.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_get_local_reg_ind(void)
{
  boolean efs_i = FALSE;
  int32 size = sizeof(boolean);
  /* Read the local NV item for the local registration indicator. */
  if(cmefs_read(CMEFS_LOCAL_REG_IND, (byte*)&efs_i, &size))
  {
    CM_MSG_HIGH_1 ("cm_auto_reg_get_local_reg_ind()efs:%d", efs_i);
    /* ... return the local registration indicator. */
    return (TRUE == (boolean)efs_i);
  }
  /* else, if the local registration indicator wasn't previously written...*/
  
  else
  {
    /* ... set the local registration indicator to FALSE. */
    CM_MSG_HIGH_0 ("cm_auto_reg_get_local_reg_ind()default:FALSE");
    cm_auto_reg_set_local_reg_ind(FALSE);
    efs_i = FALSE;
    (void) cmefs_write( CMEFS_LOCAL_REG_IND,
                          (byte*)&efs_i,
                          size);
  }
  /* Return FALSE since the value could not be read. */
  return FALSE;
}

/*===========================================================================
FUNCTION cm_auto_reg_set_local_reg_ind

DESCRIPTION
  Sets local NV item for local registration indicator.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_set_local_reg_ind(
  boolean local_reg_ind               /* Value of the LRI to set. */
)
{
  boolean efs_i = FALSE;
  int32 size = sizeof(boolean);
  /* Read the local EFS item for the local registration indicator. */
  /* If the local registration indicator has not been previously written... */
  
  CM_MSG_HIGH_1 ("cm_auto_reg_set_local_reg_ind():%d", local_reg_ind);
  if(cmefs_read(CMEFS_LOCAL_REG_IND, (byte*)&efs_i, &size))
  {
    /* ..set the value to input value. */
    efs_i = local_reg_ind;
    (void) cmefs_write( CMEFS_LOCAL_REG_IND,
                          (byte*)&efs_i,
                          size);
  }
  else
  {
    CM_ERR_0("Unable to update Local Registration indication, EFS doesn't exist");
  }
}

/*===========================================================================
FUNCTION cm_auto_reg_msg_parsing_cb

DESCRIPTION
  Called when WMS polls CM to check if it is receipient of the Msg.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_msg_parsing_cb(
    const wms_client_message_s_type *msg_ptr,
    boolean *shared )
{
  /*Parse the message*/
  if( (msg_ptr->msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) &&
      (msg_ptr->u.cdma_message.teleservice == CM_AUTO_REG_CT_TELESERVICE_ID))
  {
    *shared=FALSE;
  }

  else
  {
    *shared=TRUE;
  }

  CM_MSG_HIGH_1("cm_auto_reg_msg_parsing_cb: shared %d", *shared);
  return;  
}

/*===========================================================================
FUNCTION cm_auto_reg_msg_cmd_cb

DESCRIPTION
  Called by WMS to inform CM status of ACK sent to Lower Layers

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_msg_cmd_cb(
  wms_cmd_id_e_type        cmd,
  void                     *user_data,
  wms_cmd_err_e_type       cmd_err )
{
  CM_MSG_HIGH_2("cm_auto_reg_msg_cmd_cb:cmd=%d,cmd_err=%d", cmd, cmd_err);
}

/*===========================================================================
FUNCTION cm_auto_reg_msg_send_ack

DESCRIPTION
  Called by CM to inform ACK to WMS

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_msg_send_ack(
  wms_transaction_id_type   trans_id,
  wms_error_class_e_type    error_class,
  wms_cdma_tl_status_e_type tl_status)
{
  wms_status_e_type status = WMS_OK_S;
  wms_ack_info_s_type ack_info;

  /*Construct ACK Message*/
  ack_info.message_mode=WMS_MESSAGE_MODE_CDMA;
  ack_info.transaction_id = trans_id;
  ack_info.u.cdma.error_class = error_class;
  ack_info.u.cdma.tl_status = tl_status;
  
  status = wms_msg_ms_ack_ext( cm_auto_reg_wms_cid,
                              SYS_MODEM_AS_ID_1,
                              cm_auto_reg_msg_cmd_cb,
                              NULL,
                              &ack_info,
                              WMS_MSG_TRANSPORT_TYPE_CS );
  CM_MSG_HIGH_3("wms_msg_ms_ack_ext:Status=%d,TL_Status=%d,error=%d",status,ack_info.u.cdma.tl_status,ack_info.u.cdma.error_class);
}

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_info

DESCRIPTION
  Called when the client reg with WMS layer.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_info_send(void)
{
  wms_client_ts_data_s_type *wms_ts = (wms_client_ts_data_s_type*) cm_mem_malloc(sizeof(wms_client_ts_data_s_type));
  wms_client_err_e_type client_err;

  CM_ASSERT(wms_ts != NULL);

  //Allocate MEM for SMS.
  cm_auto_reg_wms_msg = (wms_client_message_s_type *)cm_mem_malloc(sizeof(wms_client_message_s_type));
  
  if(cm_auto_reg_wms_msg == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return;
  }
 
  /* Format a message with the registration information. */
  cm_auto_reg_format_reg_msg(wms_ts, (uint8 *)CHINATEL_ADDRESS_STRING, CHINATEL_ADDRESS_LENGTH);
  
  cm_auto_reg_format_reg_msg_data(wms_ts);
  /* Initialize the UI client type with the WMS task. */
  client_err = wms_client_init(WMS_CLIENT_FLOATING1,  &cm_auto_reg_wms_cid );
  if (client_err == WMS_CLIENT_ERR_NONE) 
  {
    /* Reg callback for wms */
    client_err = wms_client_reg_msg_cb(cm_auto_reg_wms_cid,cm_auto_reg_receive_reg_msg_cb);

    /*Reg Callback to allow WMS to poll if CM is recepient of msg*/
    client_err = wms_client_reg_parse_msg_cb(cm_auto_reg_wms_cid,cm_auto_reg_msg_parsing_cb);
    CM_MSG_HIGH_1("cm_auto_reg_info_send:wms_client_reg_parse_msg_cb:client_err %d", client_err);


    if (client_err == WMS_CLIENT_ERR_NONE) 
    {
      /* Activate the UI client type with the WMS task. */
      client_err = wms_client_activate(cm_auto_reg_wms_cid);

      if (client_err == WMS_CLIENT_ERR_NONE) 
      {
        cm_auto_reg_send_sms(0);
      }
    }
    else 
    {
      CM_ERR_1("cm_auto_reg_info_send: wms_client_activate: client_err %d", client_err);
    }
  }
  else 
  {
    CM_ERR_1("cm_auto_reg_info_send: wms_client_init: client_err %d", client_err);
  }

  /* Free allocated memory to heap */
  cm_mem_free(wms_ts);
}

/*===========================================================================
FUNCTION cm_auto_reg_format_reg_msg

DESCRIPTION
  Formats the headers of a WMS message to send to the registration server.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_format_reg_msg(
  wms_client_ts_data_s_type *wms_ts,  /* teleservice data to format */
  uint8 *address,                     /* phone number to send message */
  uint8 digits                        /* number of digits in phone number */
)
{
  /* Reset message and payload data structures. */
  memset((void*)wms_ts, 0, sizeof(wms_client_ts_data_s_type));
  memset((void*)cm_auto_reg_wms_msg, 0, sizeof(wms_client_message_s_type));

  CM_MSG_HIGH_0("cm_auto_reg_format_reg_msg()");
  /* Set up payload data structure. */
  wms_ts->format                                     = WMS_FORMAT_CDMA;
  wms_ts->u.cdma.display_mode                        = WMS_DISPLAY_MODE_DEFAULT;
  wms_ts->u.cdma.message_id.type                     = WMS_BD_TYPE_SUBMIT;
  wms_ts->u.cdma.message_id.udh_present              = FALSE;
  wms_ts->u.cdma.mask                                = WMS_MASK_BD_MSG_ID;
  wms_ts->u.cdma.message_id.id_number                = 0xFFFF;
  wms_ts->u.cdma.num_messages                        = 1;
  /* Set up message header. */
  cm_auto_reg_wms_msg->msg_hdr.index                       = WMS_DUMMY_MESSAGE_INDEX;
  cm_auto_reg_wms_msg->msg_hdr.mem_store                   = WMS_MEMORY_STORE_NONE;
  cm_auto_reg_wms_msg->msg_hdr.message_mode                = WMS_MESSAGE_MODE_CDMA;
  cm_auto_reg_wms_msg->msg_hdr.tag                         = WMS_TAG_MO_NOT_SENT;
  /* Set up message. */
  cm_auto_reg_wms_msg->u.cdma_message.is_mo                = TRUE;
  cm_auto_reg_wms_msg->u.cdma_message.teleservice          = WMS_TELESERVICE_CMT_95;
  cm_auto_reg_wms_msg->u.cdma_message.is_tl_ack_requested  = FALSE;
  cm_auto_reg_wms_msg->u.cdma_message.is_service_present   = FALSE;
  cm_auto_reg_wms_msg->u.cdma_message.service              = WMS_SRV_UNKOWN;
  cm_auto_reg_wms_msg->u.cdma_message.address.digit_mode   = WMS_DIGIT_MODE_4_BIT;
  cm_auto_reg_wms_msg->u.cdma_message.address.number_plan  = WMS_NUMBER_PLAN_TELEPHONY;
  cm_auto_reg_wms_msg->u.cdma_message.address.number_type  = WMS_NUMBER_UNKNOWN;
  cm_auto_reg_wms_msg->u.cdma_message.subaddress.number_of_digits = 0;
  /* Set up address digits */
  cm_auto_reg_wms_msg->u.cdma_message.address.number_of_digits = digits;
  (void) strlcpy((char*)cm_auto_reg_wms_msg->u.cdma_message.address.digits, (const char *)address, digits + 1);
}

/*===========================================================================
FUNCTION cm_auto_reg_format_reg_msg_data

DESCRIPTION
  Formats a registration message with the appropriate MEID/ESN and IMSI 
  fields.
  00: 0x01 ESN 0x02 MEID
  01: 0x03 REG 0x04 ACK
  02: 0x5C length
  03: 0x00 fill                                      MC 11  S      S
                              MEID/ESN               C  12  2      1
  <a1><b1>QC-GOBI2000</b1><b2>XXXXXXXXXXXXXX</b2><b3>NNNnnNNNnnnnnnn</b3>
  <b4>KBH309_C_CH_1.10.B08</b4></a1>XXXXXXXXX
      Software ver info             CRC 32
DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_format_reg_msg_data(
  wms_client_ts_data_s_type *wms_ts   /* teleservice data to format */
)
{
  char *p, *start, *end, model_info[50];
  char short_model[CM_AUTO_REG_DEV_MODEL_CT_LEN + 1]= {0};
  uint32 crc32 = 0;
  uint8 chars_to_write = 0;
  uint32 encoded_len = 0, tmp_len = 0;
  
  /* Setup the start, index and end pointers. */
  
  start = (char*)wms_ts->u.cdma.user_data.data;  /*data is of len - WMS_CDMA_USER_DATA_MAX (229)*/

/* First 4 bytes ignored */
  encoded_len += 4; /* Increment Length even if ignored */
  p = start +  encoded_len;
  end = start + (WMS_CDMA_USER_DATA_MAX - 1);


  /* Clear the registration message_mode. */
  memset(start, 0, WMS_CDMA_USER_DATA_MAX);
  
  /* Set the message type being sent. */
  start[1] = CHINATEL_SEND_REG_MSG_TYPE;
  
  /* Add the first field to the message, the mfg and hardware name. */
  chars_to_write = (uint8) strlen(cm_auto_reg_efs_data->device_model);
  CM_MSG_HIGH_2("reg_msg_data-device model:%s,len=%d", cm_auto_reg_efs_data->device_model, chars_to_write);
  
  if(chars_to_write > CM_AUTO_REG_DEV_MODEL_CT_LEN)
  {
    chars_to_write = CM_AUTO_REG_DEV_MODEL_CT_LEN;
  }

  /* Clear Buffer*/
  memset(short_model, 0, sizeof(short_model));
  memset(model_info, 0, sizeof(model_info));

  /* Encode Manufaturer Code, Device Model */

  (void) strlcpy(short_model, cm_auto_reg_efs_data->device_model, (chars_to_write+1));
  short_model[chars_to_write] = '\0';
  
  CM_MSG_HIGH_1("reg_msg_data- Short Model=%s", short_model);
  
  if(chars_to_write)
  {
    tmp_len = strlen(cm_auto_reg_efs_data->manufacturer_code)+ chars_to_write + 18 ; /*Length of <a1><b1>-</b1><b2>*/

	
	CM_MSG_HIGH_3("reg_msg_data- Manu Code=%s,len:%d,tmp_len:%d", cm_auto_reg_efs_data->manufacturer_code, 
                strlen(cm_auto_reg_efs_data->manufacturer_code), tmp_len); 
	
    (void) snprintf(model_info,sizeof(model_info),"<a1><b1>%s-%s</b1><b2>",(char *)cm_auto_reg_efs_data->manufacturer_code,short_model);
    (void) strlcpy(p,model_info, (tmp_len+1));
  }
  else
  {
  
    CM_MSG_HIGH_0("reg_msg_data-Encode default:<a1><b1>QSC-CHIPSET</b1><b2>"); 
    tmp_len = 28;
    (void) strlcpy(p, "<a1><b1>QSC-CHIPSET</b1><b2>", (size_t) (tmp_len+1));
  }
  encoded_len += tmp_len;    
  p += tmp_len;
  
  /* Set the type of hardware ID stored on the modem. */
  start[0] = CHINATEL_SEND_REG_MEID_ID;
  tmp_len = 6+8; /* %06X gives 6 digits, %08X gives 8 digits*/
  (void) snprintf(p, (size_t) (tmp_len+1), "%06X%08X", (unsigned int) qw_hi(cm_auto_reg_nv_meid), (unsigned int) qw_lo(cm_auto_reg_nv_meid));
  CM_MSG_HIGH_1("cm_auto_reg_format_reg_msg_data()-ESN ME:%ld", cm_auto_reg_nv_meid);

  encoded_len += tmp_len;
  p += tmp_len;
  
  /* Add the third field to the message, the IMSI data. */ 

  tmp_len = 9; /*size of </b2><b3>*/
  (void) strlcpy(p, "</b2><b3>", (size_t) (tmp_len+1));
  p += tmp_len;
  encoded_len += tmp_len;

  /* Encode MCC */
  cm_auto_reg_min2_to_str(cm_auto_reg_ruim_imsi_data.imsi_mcc.imsi_mcc, (byte*)p);
  tmp_len = 3; /*MCC is 3 digits*/
  p += tmp_len;
  encoded_len += tmp_len;

  /* Encode IMSI 11, 12 digits */
  cm_auto_reg_mcn_to_str(cm_auto_reg_ruim_imsi_data.imsi_11_12.imsi_11_12, (byte*)p);
  tmp_len = 2; /*IMSI 11, 12 is 2 digits*/
  p += tmp_len;
  encoded_len += tmp_len;

  /* Encode MIN2 */
  cm_auto_reg_min2_to_str(cm_auto_reg_ruim_imsi_data.min2.min2[CDMAMIN], (byte*)p);
  tmp_len = 3; /*MIN2 is 3 digits*/
  p += tmp_len;
  encoded_len += tmp_len;
  
  /* Encode MIN1 */
  cm_auto_reg_min1_to_str(cm_auto_reg_ruim_imsi_data.min1.min1[CDMAMIN], (byte*)p);
  tmp_len = 7; /*MIN1 is 7 digits*/
  p += tmp_len;
  encoded_len += tmp_len;
  
  /* Add the fourth field to the message, the IMSI data. */   
  tmp_len = 9; /*Lenght of </b3><b4> is 9 digits*/
  (void) strlcpy(p, "</b3><b4>", (size_t) (tmp_len+1));
  p += tmp_len;
  encoded_len += tmp_len;

  /* Encode SW Version */

    CM_MSG_HIGH_2("reg_msg sw version:%s,len:%d", cm_auto_reg_efs_data->sw_version, 
		                                        strlen(cm_auto_reg_efs_data->sw_version));
  if(strlen(cm_auto_reg_efs_data->sw_version))
  {
    
    tmp_len = strlen(cm_auto_reg_efs_data->sw_version);
    (void) snprintf(p, (size_t) (tmp_len+1), "%s", cm_auto_reg_efs_data->sw_version);
  }
  else
  {
    tmp_len = 0;
  }
  p += tmp_len;
  encoded_len += tmp_len;

  tmp_len = 10; /*Size of the format pattern */
  (void) strlcpy(p, "</b4></a1>", (size_t) (tmp_len+1) );
  p += tmp_len;
  encoded_len += tmp_len;

  /* Set the field length. */
  start[2] = (uint8)(encoded_len) - 4;
  
  /* Calculate CRC and append to tail of message. */
  crc32 = cm_auto_reg_crc_32_calc_reg(wms_ts->u.cdma.user_data.data, (uint16)(encoded_len));
  tmp_len = 8; /*%08x gives 8 digits*/
  (void) snprintf(p, (size_t) (tmp_len+1), "%08x", (unsigned int) crc32);
  p += tmp_len;
  encoded_len += tmp_len;
  
  /* Null terminate the message. */
  if (p < end)
  {
    p[0] = 0; 
    CM_MSG_HIGH_1("reg_msg.Encoded correctly.len:%d",encoded_len);
  }
  else
  {
    end[0] = 0;
    CM_ERR_1("reg_msg.Encoded incorrectly.len:%d",encoded_len);
  }

  /*Lenght of message should not exceed WMS_CDMA_USER_DATA_MAX*/
  tmp_len = (encoded_len < WMS_CDMA_USER_DATA_MAX)? encoded_len:WMS_CDMA_USER_DATA_MAX;
  
  /* Set up the user data fields for this message. */
  wms_ts->u.cdma.user_data.encoding = WMS_ENCODING_OCTET;
  wms_ts->u.cdma.user_data.data_len = (uint8)tmp_len;
  wms_ts->u.cdma.user_data.number_of_digits = (uint8)tmp_len;
  /* Add the user data mask to this message. */
  wms_ts->u.cdma.mask |= WMS_MASK_BD_USER_DATA;
  /* setup Priority as normal*/
  wms_ts->u.cdma.priority = WMS_PRIORITY_NORMAL;
  /* Add the priority mask to this message. */
  wms_ts->u.cdma.mask |= WMS_MASK_BD_PRIORITY;

  CM_MSG_HIGH_2("reg_msg.datalen:%d,numdigits:%d",wms_ts->u.cdma.user_data.data_len,wms_ts->u.cdma.user_data.number_of_digits);
  
  /* Encode the user data for transmission, and add it to the message. */
  (void) wms_ts_encode(wms_ts, &(cm_auto_reg_wms_msg->u.cdma_message.raw_ts));
  return;
}

/*===========================================================================
FUNCTION cm_auto_reg_receive_reg_msg_cb

DESCRIPTION
  Called when a UI received MT SMS or Report.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_receive_reg_msg_cb
(
  wms_msg_event_e_type         event,
  /* event type */
  wms_msg_event_info_s_type    *info_ptr,
  /* event information */
  boolean                      *shared
)
{

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  
  mmgsdi_session_type_enum_type cm_mmgsdi_session_type =
												  MMGSDI_1X_PROV_PRI_SESSION;
  mmgsdi_session_id_type		cm_mmgsdi_session_id=0;

  cm_mmgsdi_error_e_type        cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;

  #endif
  
  wms_client_ts_data_s_type *client_ts_data 
                                         = (wms_client_ts_data_s_type *) 
                                            cm_mem_malloc(sizeof(wms_client_ts_data_s_type));

CM_ASSERT(client_ts_data != NULL);

  if(info_ptr == NULL)
  {
     CM_ERR_0("info_ptr NULL");
     /* Free allocated memory to heap */
     cm_mem_free(client_ts_data);
     return;
  }

  switch(event)
  {
    case  WMS_MSG_EVENT_SUBMIT_REPORT:
      
    CM_MSG_HIGH_3("WMS_MSG_EVENT_SUBMIT_REPORT tl_ack_requested=%d, report_status=%d, error_class=%d",
		cm_auto_reg_wms_msg->u.cdma_message.is_tl_ack_requested,
		info_ptr->submit_report_info.report_status,
		info_ptr->submit_report_info.cause_info.error_class);
    
    if(cm_auto_reg_wms_msg->u.cdma_message.is_tl_ack_requested	== TRUE)
    {
      if(info_ptr->submit_report_info.report_status != WMS_RPT_OK )
      {

        if((info_ptr->submit_report_info.report_status == WMS_RPT_NO_ACK || 
            info_ptr->submit_report_info.cause_info.error_class == WMS_ERROR_TEMP))
        {

        }
        else if(info_ptr->submit_report_info.cause_info.error_class == WMS_ERROR_PERM)
        {
          cm_auto_reg_dereg();
        }
      }
      else
      {
        cm_auto_reg_set_local_reg_ind(TRUE);
      }
    }

    break;

    case WMS_MSG_EVENT_RECEIVED_MESSAGE:

    CM_MSG_HIGH_4("MT SMS message recv mode:%d TsID:%d;acq_req:%d,trans_id:%d",info_ptr->mt_message_info.message.msg_hdr.message_mode,
                                                                 info_ptr->mt_message_info.message.u.cdma_message.teleservice,
                                                                 info_ptr->mt_message_info.message.u.cdma_message.is_tl_ack_requested,
                                                                 info_ptr->mt_message_info.transaction_id);
    
    /* Retrieve User Data only if CT spcfic Teleservice service ID and MSG mode CDMA */
    if( (info_ptr->mt_message_info.message.msg_hdr.message_mode == WMS_MESSAGE_MODE_CDMA) && 
         (info_ptr->mt_message_info.message.u.cdma_message.teleservice == CM_AUTO_REG_CT_TELESERVICE_ID))
    {
      if(wms_ts_decode( &info_ptr->mt_message_info.message.u.cdma_message.raw_ts, client_ts_data ) != WMS_OK_S)
      {
        CM_MSG_HIGH_0("SMS message callback:TS DECODE failed");

        /*Send NACK if ACK requested by Network*/
        if( info_ptr->mt_message_info.message.u.cdma_message.is_tl_ack_requested == TRUE ) 
        {
          cm_auto_reg_msg_send_ack(info_ptr->mt_message_info.transaction_id,WMS_ERROR_TEMP, WMS_TL_UNRECOGNIZED_PARM_VALUE_S);
        }

        /* Free allocated memory to heap */     
        cm_mem_free(client_ts_data);
        return;  
      }
      else
      {
        if (client_ts_data->u.cdma.user_data.encoding == WMS_ENCODING_OCTET && 
            client_ts_data->u.cdma.user_data.data_len >= 2 && 
            (client_ts_data->u.cdma.user_data.data[0] == 0x01 || 
            client_ts_data->u.cdma.user_data.data[0] == 0x02) && 
            client_ts_data->u.cdma.user_data.data[1] == 0x04)
        {
          /*Send ACK if ACK requested by Network*/
          if( info_ptr->mt_message_info.message.u.cdma_message.is_tl_ack_requested == TRUE ) 
          {
            cm_auto_reg_msg_send_ack(info_ptr->mt_message_info.transaction_id,WMS_ERROR_NONE, WMS_TL_MAX32);
          }

          cm_auto_reg_set_local_reg_ind(TRUE);
          if( cm_auto_reg_info_complete())
          {
            CM_MSG_HIGH_0("SMS Reg Success..."); 
            /*Store ESN*/
            #ifdef FEATURE_MMGSDI_SESSION_LIB
            /*
                     ** Read the attributes for the Preferred PLMN With Access Technology List.
                     */
            cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type,
                                                                 &cm_mmgsdi_session_id);
            if( (cm_mmgsdi_status == MMGSDI_SUCCESS) &&
                (MMGSDI_SUCCESS == mmgsdi_session_store_esn(cm_mmgsdi_session_id,cmmmgsdi_session_store_esn_cb,(mmgsdi_client_data_type) cm_mmgsdi_session_type)))
            {
              CM_MSG_HIGH_0("mmgsdi_session_store_esn(), successful...");
            }
            else
            {
              CM_ERR_1("mmgsdi_session_store_esn(),failed :status:%d",cm_mmgsdi_status);
             }
             #endif
          }
          else
          {
            CM_ERR_0("SMS Reg Success...");
          }
        }
        else
        {
          CM_MSG_HIGH_0("SMS Reg failed, retry..."); 
          /*Send NACK if ACK requested by Network*/
          if( info_ptr->mt_message_info.message.u.cdma_message.is_tl_ack_requested == TRUE ) 
          {
            cm_auto_reg_msg_send_ack(info_ptr->mt_message_info.transaction_id,WMS_ERROR_TEMP, WMS_TL_UNEXPECTED_PARM_VALUE_S);
          }
        }
      }
    }

    else
    {
      /*No need to send NACK if ACK requested by Network as this SMS is not required by CM*/
      if( info_ptr->mt_message_info.message.u.cdma_message.is_tl_ack_requested == TRUE ) 
      {
        CM_MSG_HIGH_0("MT SMS message ignored : Not AUTO REG Confirmation SMS");
        }
      }

    break;

    case  WMS_MSG_EVENT_ACK_REPORT:
      
	CM_MSG_HIGH_2("WMS_MSG_EVENT_ACK_REPORT: TL ACK status:%d error:%d",
                      info_ptr->ack_report_info.is_success,
                      info_ptr->ack_report_info.error_status);
    
    if( (info_ptr->ack_report_info.error_status != WMS_RPT_OK ) || 
        (info_ptr->ack_report_info.is_success != TRUE ) )
    {
      
    }
    else
    {
      cm_auto_reg_set_local_reg_ind(TRUE);
    }

    break;
    
    default:  	
      CM_MSG_HIGH_1("cm_auto_reg_receive_reg_msg_cb event Ignored event = %d",event);
      break;
  }
  SYS_ARG_NOT_USED (info_ptr);
  SYS_ARG_NOT_USED (shared);
  /* Free allocated memory to heap */
  cm_mem_free(client_ts_data);
  return;
}

/*===========================================================================
FUNCTION cm_auto_reg_send_sms

DESCRIPTION
  Called when the WMS message should be sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_send_sms(unsigned long data)
{

  wms_status_e_type wms_status = WMS_OK_S;

  if( cm_auto_reg_send_sms_retry < CM_AUTO_REG_SMS_MAX_RETRY)
  {
    cm_auto_reg_send_sms_retry++;
  }
  else
  {
   cm_auto_reg_dereg();
   return;
  }

  wms_status  = wms_msg_ms_send(cm_auto_reg_wms_cid, SYS_MODEM_AS_ID_1, cm_auto_reg_send_reg_msg_cb, 0, WMS_SEND_MODE_CLIENT_MESSAGE, cm_auto_reg_wms_msg);

  CM_MSG_HIGH_1("cm_auto_reg_info_send: wms_status %d", wms_status);
  
  if (WMS_OK_S != wms_status) 
  {
    CM_ERR_1("cm_auto_reg_info_send: wms_status %d", wms_status);
  }

  cm_auto_reg_set_timer(60,cm_auto_reg_send_sms);
  SYS_ARG_NOT_USED (data);
}

/*===========================================================================
FUNCTION cm_auto_reg_complete_reg_info

DESCRIPTION
  Called when the ACK message from the server is received.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_info_complete(void)
{
    CM_MSG_HIGH_0("cm_auto_reg_info_complete()");
    if (cm_auto_reg_get_local_reg_ind())
    {  
      (void) cm_auto_reg_write_efs_imsi();
      /* Deactivate WMS - need not wait for receiving ack/nack for "ack of confirm sms" */
      cm_auto_reg_dereg();
      return TRUE;
    }
  return FALSE;
}

/*===========================================================================
FUNCTION cm_auto_reg_write_efs_imsi

DESCRIPTION
  Stores the values read from RUIM into the EFS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
nv_stat_enum_type cm_auto_reg_write_efs_imsi(void)
{
  
  nv_stat_enum_type efs_status = NV_FAIL_S;
  int32 size = 0;
  
  

  /* Write the MIN1 IMSI information from RUIM to the on-board EFS. */
  size = sizeof(cm_auto_reg_ruim_imsi_data.min1.min1[CDMAMIN]);
  if ( !cmefs_write( CMEFS_IMSI_MIN1,(byte*)&cm_auto_reg_ruim_imsi_data.min1.min1[CDMAMIN],size))
  {
    CM_ERR_1("cm_auto_reg_write_efs_imsi: write nv s1 error 0x%X", 
              efs_status);  
    return efs_status;
  }

  /* Write the MIN2 IMSI information from RUIM to the on-board EFS. */
  size = sizeof(cm_auto_reg_ruim_imsi_data.min2.min2[CDMAMIN]);
  if ( !cmefs_write( CMEFS_IMSI_MIN2,(byte*)&cm_auto_reg_ruim_imsi_data.min2.min2[CDMAMIN],size))
  {
    CM_ERR_1("cm_auto_reg_write_efs_imsi: write nv s2 error 0x%X", 
              efs_status);  
    return efs_status;
  }

  /* Write the MCC IMSI information from RUIM to the on-board EFS. */
  size = sizeof(cm_auto_reg_ruim_imsi_data.imsi_mcc.imsi_mcc);
  if ( !cmefs_write( CMEFS_IMSI_MCC,(byte*)&cm_auto_reg_ruim_imsi_data.imsi_mcc.imsi_mcc,size))
  {
    CM_ERR_1("cm_auto_reg_write_efs_imsi: write nv mcc error 0x%X", 
              efs_status);  
    return efs_status;
  }

  /* Write the 11_12 IMSI information from RUIM to the on-board EFS. */
  size = sizeof(cm_auto_reg_ruim_imsi_data.imsi_11_12.imsi_11_12);
  if ( !cmefs_write( CMEFS_IMSI_11_12,(byte*)&cm_auto_reg_ruim_imsi_data.imsi_11_12.imsi_11_12,size))
  {
    CM_ERR_1("cm_auto_reg_write_efs_imsi: write nv mcc error 0x%X", 
              efs_status);  
    return efs_status;
  }
  
  CM_MSG_HIGH_0("cm_auto_reg_write_efs_imsi() Complete");  
  
  return NV_DONE_S;
}

/*===========================================================================
FUNCTION cm_auto_reg_send_reg_msg_cb

DESCRIPTION
  Called when a UI message has been sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cm_auto_reg_send_reg_msg_cb
(
  wms_cmd_id_e_type          cmd,       /* command status being reported */
  void                      *user_data, /* data passed to send request */
  wms_cmd_err_e_type         cmd_err    /* the status of the command */
)
{
  /* If the registration message is successfully sent... */
  if (cmd_err == WMS_CMD_ERR_NONE) 
  {
    /* ... set the WAIT ACK command, so that the UI tasks looks for a ACK 
        * message to complete processing. */
    CM_MSG_HIGH_0("cm_auto_reg_send_reg_msg_cb: WMS_CMD_ERR_NONE ");
    if(cm_auto_reg_wms_msg->u.cdma_message.is_tl_ack_requested  == TRUE)
    {
      cm_auto_reg_cmds |= CM_AUTO_REG_INFO_WAIT_ACK_CMD;
    }

  }
  else
  {
    /* ... log an error so this problem may be correct later. */
    CM_ERR_1("cm_auto_reg_send_reg_msg_cb: cmd_err %d", (int)cmd_err);
  }
  SYS_ARG_NOT_USED (cmd);
  SYS_ARG_NOT_USED (user_data);
}

/*===========================================================================
FUNCTION cm_auto_reg_compare_efs_ruim_items

DESCRIPTION
  Compares UI parameters to see if registration is necessary.

RETURN
  False - A registration message does not need to be sent.
  True - A registration message was sent.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
boolean cm_auto_reg_compare_efs_ruim_items(void)
{
  mmgsdi_esn_me_change_enum_type esn_change_flag = mmgsdi_get_esn_me_change_flag(cm_mmgsdi_ssn_tbl_get_slot_for_type(MMGSDI_1X_PROV_PRI_SESSION));
  /* If the UIM_CDMA_ESN_ME field did not change at startup... */
  if (esn_change_flag != MMGSDI_ESN_ME_CHANGE && esn_change_flag != MMGSDI_ESN_ME_UPDATE_PENDING)
  {
    /* ...then, compare the IMSI data. If the data is the same... */
    if(cm_auto_reg_efs_imsi_data.imsi_mcc.imsi_mcc == cm_auto_reg_ruim_imsi_data.imsi_mcc.imsi_mcc &&
      cm_auto_reg_efs_imsi_data.min1.min1[CDMAMIN] == cm_auto_reg_ruim_imsi_data.min1.min1[CDMAMIN] &&
      cm_auto_reg_efs_imsi_data.min2.min2[CDMAMIN] == cm_auto_reg_ruim_imsi_data.min2.min2[CDMAMIN] &&
      cm_auto_reg_efs_imsi_data.imsi_11_12.imsi_11_12 == cm_auto_reg_ruim_imsi_data.imsi_11_12.imsi_11_12) 
    {
      /*memcmp(&cm_auto_reg_efs_imsi_data, &cm_auto_reg_ruim_imsi_data, sizeof(struct imsi_data)) == 0*/
      CM_MSG_MED_0("cm_auto_reg_compare_efs_ruim_items: IMSI match");  
      /* ...this is the same RUIM as before. */
      return (TRUE);
    }
    else
      /* The RUIM has been reconfigured since last boot. */
      CM_MSG_MED_0("cm_auto_reg_compare_efs_ruim_items: IMSI does not match");  
  }

  /* The RUIM is different than the one last recorded, return FALSE. */  
  CM_MSG_MED_0("cm_auto_reg_compare_efs_ruim_items: ESN does not match");  
  return (FALSE);
}


/*===========================================================================
FUNCTION
  cm_auto_reg_min1_to_str
DESCRIPTION
  Convert a MIN1 value to an ASCII string.
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void cm_auto_reg_min1_to_str(dword dwPhoneNo, byte *pszText)
{
  word wTemp;
  int nIndex = 0;

  if( dwPhoneNo > 0 ) {
    wTemp = (word)(dwPhoneNo >> 14);
    pszText[nIndex++] = DigitToAscTable[wTemp / 100];
    wTemp %= 100;
    pszText[nIndex++] = DigitToAscTable[wTemp / 10 ];
    pszText[nIndex++] = DigitToAscTable[wTemp % 10 ];

    dwPhoneNo &= 0x3FFFL;
    wTemp = (word)(((dwPhoneNo & 0x3FFFL) >> 10 ) & 0xF );
    pszText[nIndex++] = (char)(((wTemp == 10) ? 0 : wTemp) + '0');

    wTemp = (word) ( dwPhoneNo & 0x3FF );
    pszText[nIndex++] = DigitToAscTable[ wTemp/100 ];
    wTemp %= 100;
    pszText[nIndex++] = DigitToAscTable[ wTemp/10 ];
    pszText[nIndex++] = DigitToAscTable[ wTemp%10 ];

    return;
  }
  for( wTemp=0; wTemp<7; wTemp++ ) {
    pszText[nIndex++] = '0';
  }
  
  return;
} /* cm_auto_reg_min1_to_str */

/*===========================================================================
FUNCTION
  cm_auto_reg_min2_to_str
DESCRIPTION
  Convert a MIN2 value to an ASCII string.
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void cm_auto_reg_min2_to_str(word wAreaCode, byte *pszText)
{
  int nIndex = 0;
  
    /* Defaulting the MIN2/MCC to Least Significant Digits */
  wAreaCode = (wAreaCode % 1000);
  pszText[nIndex++] = DigitToAscTable[wAreaCode / 100];
  wAreaCode %= 100;
  pszText[nIndex++] = DigitToAscTable[wAreaCode / 10];
  pszText[nIndex++] = DigitToAscTable[wAreaCode % 10];
  return;
} /* cm_auto_reg_min2_to_str */

/*===========================================================================
FUNCTION
  cm_auto_reg_mcn_to_str
DESCRIPTION
  Convert a MCN/11_12 value to an ASCII string.
DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void cm_auto_reg_mcn_to_str(byte bMCN, byte *pszText)
{
  int nIndex = 0;

    /* Defaulting the MNC/11_12 to Least Significant Digits */
  bMCN = (bMCN % 100);
  pszText[nIndex++] = DigitToAscTable[bMCN / 10];
  pszText[nIndex++] = DigitToAscTable[bMCN % 10];

  return;
} /* cm_auto_reg_min2_to_str */


/*===========================================================================

FUNCTION cm_auto_reg_crc_32_calc_reg

===========================================================================*/
static uint32 cm_auto_reg_crc_32_calc_reg
(
  /* Pointer to data over which to compute CRC */
  uint8  *buf_ptr,

  /* Number of bits over which to compute CRC */
  uint16  len
)
{
    uint32 crc = 0xffffffff;
    uint32 i = 0;
    for (i = 0 ; i < len ; i ++) 
    {
      crc = (crc >> 8 ^ crc32_reg_table[(crc ^ buf_ptr[i]) & 0xff]);
    }
    crc = crc ^ 0xffffffff;
    return crc;
}

/*===========================================================================
FUNCTION UIUTILS_GET_IMEI

DESCRIPTION
  This function fills in the imei into the given byte array.  The
  input string must be at least 2*NV_UE_IMEI_SIZE.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

int cm_auto_reg_get_imei(byte imei_ascii[], size_t imei_ascii_size)
{
  byte imei_bcd_len = 0;
  int bcd_index = 0;
  int ascii_index = 0;
  byte asc_1 = 0;
  byte asc_2 = 0;
  nv_item_type nvi;

  byte imei[(NV_UE_IMEI_SIZE-1)*2];

  /* This is a valid IMEI */
  memset(imei, 0, (NV_UE_IMEI_SIZE-1)*2);

  
  cmnv_read_wait (NV_UE_IMEI_I, &nvi);
  
   /*
   ** CONVERT IT INTO ASCII
   */

  /* The first byte of NV ue_imei is the length of IMEI BCDs */
  imei_bcd_len = nvi.ue_imei.ue_imei[0];

  if (imei_bcd_len == 0)
  {
    CM_ERR_0("NV Read of NV_UE_IMEI_I - failed");
    memscpy( (byte *) imei_ascii, imei_ascii_size, (byte *) " NOT SET", 8 );
    return (int) strlen(" NOT SET");
  }

  if (imei_bcd_len < NV_UE_IMEI_SIZE)
  {
    /*
    ** THIS IS A VALID IMEI, SO CONVERT IT TO ASCII
    */
    for ( bcd_index = 1; bcd_index <= (int) imei_bcd_len; bcd_index++ )
    {
      asc_1 = (nvi.ue_imei.ue_imei[bcd_index] & 0x0F);
      asc_2 = (nvi.ue_imei.ue_imei[bcd_index] & 0xF0) >> 4;
      imei[ascii_index++] = (asc_1 <= 9)? (asc_1+'0')
                                              : ((asc_1-10)+'A');
      imei[ascii_index++] = (asc_2 <= 9)? (asc_2+'0')
                                              : ((asc_2-10)+'A');
    }

    if (ascii_index > 2)
    {
      memscpy( imei_ascii, imei_ascii_size, imei + 1, (uint32) (ascii_index - 1) );
    }

    return (ascii_index - 1);
  }
  else
  {
    return 0;
  }
} //uiutils_get_imei(byte imei_asc...

/*===========================================================================
FUNCTION handleAutoRegister

DESCRIPTION
  This function checks the prerequisites and triggers auto register sms.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

void handleAutoRegister(unsigned long data)
{
  sys_modem_as_id_e_type asid = SYS_MODEM_AS_ID_1;
  cm_mm_ss_info_s_type *ss_info_ptr = NULL;


  if( cm_get_operator_info(asid) != OPERATOR_NULL )
  {
    if ( cm_get_operator_info(asid) == OPERATOR_CT )
    {
      /* Run the state machine for the China Telecom SMS registration message task. */
      /* If there is a registration info command pending... */
      CM_MSG_HIGH_1("cm_auto_reg_cmds:%d",cm_auto_reg_cmds);				
      if (cm_auto_reg_cmds)
      {
        /* If there is a format message command pending... */
        if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_CHECK_CMD)
        {
          /* Format the message and clear the command. */
          cm_auto_reg_info_check();
          cm_auto_reg_cmds &= ~CM_AUTO_REG_INFO_CHECK_CMD;
        }
        /* If there is a queue message command pending... */
        if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_QUEUE_CMD)
        {
          /* Get serving system info and clear the command. */
          cm_auto_reg_cmds &= ~CM_AUTO_REG_INFO_QUEUE_CMD;
          cm_auto_reg_cmds |= CM_AUTO_REG_INFO_CTS_CMD;
          CM_MSG_HIGH_1("cm_auto_reg_cmds:%d-CM_AUTO_REG_INFO_CTS_CMD",cm_auto_reg_cmds);				
          ss_info_ptr      = &(cmss_ptr()->info);
          cm_auto_reg_ss_event_mdm_cb(CM_SS_EVENT_SRV_CHANGED, ss_info_ptr);
        }
        /* If there is a send message command pending... */
        if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_SEND_CMD)
        {
          /* Send the message and clear the command. */
          cm_auto_reg_info_send();
          cm_auto_reg_cmds &= ~CM_AUTO_REG_INFO_SEND_CMD;
        }
        /* If there is a wait for message command pending... */
        if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_WAIT_ACK_CMD)
        {
          /* If the ACK message was received... */
          if (cm_auto_reg_info_complete())
          {
            /* Clear the wait for ACK command. */
            cm_auto_reg_cmds = CM_AUTO_REG_INFO_COMPLETE_CMD;            
            CM_MSG_HIGH_1("cm_auto_reg_cmds:%d-CM_AUTO_REG_INFO_COMPLETE_CMD",cm_auto_reg_cmds);				
          }
        }
      }
    }
    else if ( cm_get_operator_info(asid) == OPERATOR_CU )
    {
      if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_CU_SEND_CMD)
      {
        cm_auto_reg_info_send_cu_8bit();
        cm_auto_reg_sms_sent = TRUE;
        cm_auto_reg_cmds &= ~CM_AUTO_REG_INFO_CU_SEND_CMD;
      }
    }
	
    else if ( cm_get_operator_info(asid) == OPERATOR_CMCC )
    {
      if (cm_auto_reg_cmds & CM_AUTO_REG_INFO_CMCC_SEND_CMD)
      {
        cm_auto_reg_info_send_cmcc_8bit();
        cm_auto_reg_sms_sent = TRUE;
        cm_auto_reg_cmds &= ~CM_AUTO_REG_INFO_CMCC_SEND_CMD;
      }
    }
    else
    {
      CM_MSG_HIGH_0("cmss_info_new: Unsupported Operator for Auto Register");  
    }
  }
  SYS_ARG_NOT_USED (data);
}

/*===========================================================================
FUNCTION cm_auto_reg_set_timer

DESCRIPTION
  Sets timer.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/

void cm_auto_reg_set_timer(uint32 sec,timer_t1_cb_type cb_func)
{
  CM_MSG_HIGH_1 ("cm_auto_reg_set_timer()No.Of Sec :%d",sec);
  /* Setup a task to send the message in given seconds. */
  timer_def(cm_auto_reg_timer, 0, 0, 0, cb_func, 0);
  /*Clear it before set the timer */
  (void) timer_clr(cm_auto_reg_timer,T_SEC);
  /*Set the timer now*/
  timer_set(cm_auto_reg_timer, sec, 0, T_SEC );
}

/*===========================================================================
FUNCTION cm_auto_reg_send_msg_timer_cb

DESCRIPTION
  Timer callback function used to transmit registration message.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
 void cm_auto_reg_cts_msg_timer_cb(
  timer_cb_data_type timer_id         /* timer id for this event */
)
{
  cm_generic_rpt_s_type *cm_generic_rpt_ptr;
  
  CM_MSG_MED_0("cm_auto_reg_cts_msg_timer_cb");
  /* Set the QUEUE command so that the current state of the network can 
   * be queried. */
  cm_auto_reg_cmds |= CM_AUTO_REG_INFO_QUEUE_CMD;

  if ((cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal())!=NULL)
  {
    cm_generic_rpt_ptr->hdr.cmd = CM_AUTO_REG_TIMER_RPT;
    cm_generic_rpt(cm_generic_rpt_ptr);
  }
  SYS_ARG_NOT_USED (timer_id);
}

/*===========================================================================
FUNCTION cm_auto_reg_set_autoreg_sms_sent

DESCRIPTION
  API to set the AUTO REG MSG STATUS.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
 void cm_auto_reg_set_autoreg_sms_sent(
  boolean status
)
{
  CM_MSG_MED_2("cm_auto_reg_sms_sent:%d->%d",cm_auto_reg_sms_sent,status);
  cm_auto_reg_sms_sent = status;
}

/*===========================================================================
FUNCTION cm_auto_reg_get_autoreg_sms_sent

DESCRIPTION
  API to get the AUTO REG MSG STATUS..

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
 boolean cm_auto_reg_get_autoreg_sms_sent(
)
{
  CM_MSG_MED_1("cm_auto_reg_sms_sent:%d",cm_auto_reg_sms_sent);
  return cm_auto_reg_sms_sent;
}

/* Changes for QRD Features End */


