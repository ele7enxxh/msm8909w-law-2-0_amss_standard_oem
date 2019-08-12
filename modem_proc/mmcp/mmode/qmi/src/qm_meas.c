/*===========================================================================

                        QM_MEAS.C 

DESCRIPTION

 QMI_MMODE source file for MEASUREMENT module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/23/13    aa     Initial version
===========================================================================*/

#include "comdef.h"
#include "msg.h"
#include "qm_si.h"
#include "qm_nas_clnt.h"
#include "qm_util.h"
#include "qmi_mmode_svc.h"
#include "qm_meas.h"

/*--------------------------------------------------------------------------- 
  defines
---------------------------------------------------------------------------*/
#define QM_MEAS_INVALID_ERR_RATE              0xFFFFFFFF
#define QM_MEAS_INVALID_RSSI                  (-125)  // value of -125 dBm or lower is used to indicate No Signal
#define QM_MEAS_INVALID_ECIO                  0
#define QM_MEAS_INVALID_RSCP                  (-125)  // Measurement range: -120 dBm to -25 dBm. 
#define QM_MEAS_INVALID_SINR                  (-1)
#define QM_MEAS_INVALID_IO                    0
#define QM_MEAS_INVALID_SNR                   0
#define QM_MEAS_INVALID_RSRP                  0      // range -44 to 140 dBm
#define QM_MEAS_INVALID_RSRQ                  0      // range -3 to -20 dB


/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/
 struct qm_sig_s
 {
   struct
   {
     boolean is_valid;
     float   rssi;
     float   ecio;
     uint32  frame_error_rate;
   } cdma_sig_str;
  
   struct
   {
     boolean is_valid;
     float   rssi;
     float   ecio;
     float   sinr;
     float   io;
     uint32  packet_err_rate;
   } hdr_sig_str;
   
   struct
   {
     boolean is_valid;
     float   rssi;
     uint32   bit_error_rate;
   } gsm_sig_str;
   
   struct
   {
     boolean is_valid;
     float   rssi;
     float   ecio;
     uint32   block_error_rate;
   } wcdma_sig_str;
   
   struct
   {
     boolean is_valid;
     float   rssi;
     float   rsrq;
     float   rsrp;
     float   snr;
   } lte_sig_str;
     
   struct
   {
     boolean is_valid;
     float   rssi;
     float   rscp;
     float   ecio;
     float   sinr;
     uint32   bler;
     boolean in_dch;
   } tdscdma_sig_str;
 };


struct qm_meas_s 
{
  struct qm_sig_s meas[QM_SUBS_MAX];
  int8 send_sig_info_ind[QM_NAS_MAX_NUM_CLIDS];
  int8 send_err_rate_ind[QM_NAS_MAX_NUM_CLIDS];  
};

//===========================================================================
// GLOBALS
//===========================================================================
static struct qm_meas_s global;

//===========================================================================
// STATIC FUNCTIONS
//===========================================================================
static void send_sig_info_ind( void );
static void send_err_rate_ind( void );
static boolean populate_sig_info_ind( nas_sig_info_ind_msg_v01 *ind_msg, enum qm_subs_e sub_id);
static boolean populate_err_rate_ind( nas_err_rate_ind_msg_v01 *ind_msg, enum qm_subs_e sub_id);
static void invalidate_inactive_rats( enum qm_subs_e sub_id, rat_bitmask_type inactive_rat_mask );
static void check_send_ind_for_cm_measurement_event( cm_mm_msim_ss_info_s_type *p_ss_info );
static void update_cm_measurements_to_global_struct(cm_mm_msim_ss_info_s_type *p_ss_info);
#ifdef FEATURE_TDSCDMA
static void check_send_ind_for_tds_measurements(tds_rrc_scell_signal_status_ind_type *tds_meas, enum qm_subs_e sub_id );
static void update_tds_measurements_to_global_struct(tds_rrc_scell_signal_status_ind_type *tds_meas, enum qm_subs_e sub_id );
#endif

//===========================================================================
// UTILITY FUNCTIONS
//===========================================================================
static float convert_cm_ss_evt_rssi(uint16 rssi){ return (float)((int16)rssi * -1.0); }         // saved as dBm
static float convert_cm_ss_evt_ecio(int16 ecio){ return (float)ecio* -0.5; }                    // saved as dBm
static float convert_cm_ss_evt_hdr_sinr(byte sinr){ return (float)sinr; }                  // saved as value in range from 0.0 to 8.0 
static float convert_cm_ss_evt_hdr_io(int io){ return (float)io; }                             // saved as dBm
static float convert_cm_ss_evt_lte_rsrq(int8 rsrq){ return (float)rsrq; }                      // saved as dB
static float convert_cm_ss_evt_lte_rsrp(int16 rsrp){ return (float)rsrp; }                     // saved as dBm
static float convert_cm_ss_evt_lte_snr(byte sinr){ return (float)(( 2 * sinr ) - 200)/10.0; }  // saved as dB
static float convert_cm_ss_evt_err_rate(uint16 err_rate){ return (uint32)err_rate; }
static float convert_cm_ss_evt_gw_err_rate(uint8 err_rate){ return (uint32)err_rate; }
#ifdef FEATURE_TDSCDMA
static float convert_msgr_evt_tds_rscp(int16 rscp){ return (float)(rscp)/10.0; }               // saved as dBm
static float convert_msgr_evt_tds_rssi(int16 rssi){ return (float)(rssi)/10.0; }                // saved as dBm
static float convert_msgr_evt_tds_ecio(int16 ecio){ return (float)(ecio)/10.0; }                // saved as dB
static float convert_msgr_evt_tds_sinr(int16 sinr){ return (float)(sinr)/10.0; }                // saved as dB
static float convert_msgr_evt_tds_bler(uint8 bler){ return (uint32)bler; }
#endif
static int8 convert_global_rssi(float rssi){ return (int8)rssi; }                                  // saved as dBm
static int16 convert_global_ecio(float ecio){ return (int16)(ecio/-0.5); }                        // saved as dB
static nas_sinr_enum_v01 convert_global_hdr_sinr(float sinr){ return (nas_sinr_enum_v01)sinr; }
static int32 convert_global_hdr_io(float io){ return (int32)io; }                                // saved as dBm
static int8 convert_global_lte_rsrq(float rsrq){ return (int8)rsrq; }                            // saved as dB
static int16 convert_global_lte_rsrp(float rsrp){ return (int16)rsrp; }                          // saved as dBm

static int16 convert_global_lte_snr(float snr) // saved as dB, to be returned in 0.1 dB
{ 
  float lte_snr = snr*10.0; 
  return (int16)lte_snr; 
}

static int8 convert_global_tds_rscp(float rscp){ return (int8)rscp; }                           // saved as dBm

/*===========================================================================
  FUNCTION POPULATE_SIG_INFO_IND()

  DESCRIPTION
    This function populates QMI_NAS_SIG_INFO_IND message for a given subscription

  PARAMETERS   
    ind_msg : Pointer to SIG_INFO_IND IDL structure
    sub_id   : subscription id

  RETURN VALUE
    TRUE is population of TLVs is successful, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean populate_sig_info_ind( nas_sig_info_ind_msg_v01 *ind_msg, enum qm_subs_e sub_id)
{
  boolean retval = FALSE;

  if( QM_NAS_SUB_ID_VALID(sub_id) )
  {
    // CDMA
    if( global.meas[sub_id].cdma_sig_str.is_valid )
    {
      ind_msg->cdma_sig_info.rssi = convert_global_rssi(global.meas[sub_id].cdma_sig_str.rssi);
      ind_msg->cdma_sig_info.ecio = convert_global_ecio(global.meas[sub_id].cdma_sig_str.ecio);
      ind_msg->cdma_sig_info_valid = TRUE;
      retval = TRUE;
    }
    // HDR
    if( global.meas[sub_id].hdr_sig_str.is_valid && !global.meas[sub_id].wcdma_sig_str.is_valid && !global.meas[sub_id].gsm_sig_str.is_valid && !global.meas[sub_id].tdscdma_sig_str.is_valid )
    {
      ind_msg->hdr_sig_info.common_sig_str.rssi = convert_global_rssi(global.meas[sub_id].hdr_sig_str.rssi);
      ind_msg->hdr_sig_info.common_sig_str.ecio = convert_global_ecio(global.meas[sub_id].hdr_sig_str.ecio);
      ind_msg->hdr_sig_info.sinr = convert_global_hdr_sinr(global.meas[sub_id].hdr_sig_str.sinr);
      ind_msg->hdr_sig_info.io   = convert_global_hdr_io(global.meas[sub_id].hdr_sig_str.io);
      ind_msg->hdr_sig_info_valid = TRUE;
      retval = TRUE;
    }
    // GSM
    if( global.meas[sub_id].gsm_sig_str.is_valid )
    {
      ind_msg->gsm_sig_info = convert_global_rssi(global.meas[sub_id].gsm_sig_str.rssi);
      ind_msg->gsm_sig_info_valid = TRUE;
      retval = TRUE;
    }
    // WCDMA
    if( global.meas[sub_id].wcdma_sig_str.is_valid )
    {
      ind_msg->wcdma_sig_info.rssi = convert_global_rssi(global.meas[sub_id].wcdma_sig_str.rssi);
      ind_msg->wcdma_sig_info.ecio = convert_global_ecio(global.meas[sub_id].wcdma_sig_str.ecio);
      ind_msg->wcdma_sig_info_valid = TRUE;
      retval = TRUE;
    }
    // LTE
    if( global.meas[sub_id].lte_sig_str.is_valid )
    {
      ind_msg->lte_sig_info.rssi = convert_global_rssi(global.meas[sub_id].lte_sig_str.rssi);
      ind_msg->lte_sig_info.rsrq = convert_global_lte_rsrq(global.meas[sub_id].lte_sig_str.rsrq);
      ind_msg->lte_sig_info.rsrp = convert_global_lte_rsrp(global.meas[sub_id].lte_sig_str.rsrp);
      ind_msg->lte_sig_info.snr  = convert_global_lte_snr(global.meas[sub_id].lte_sig_str.snr);
      ind_msg->lte_sig_info_valid = TRUE;
      retval = TRUE;
    }
    // TDSCDMA
    if( global.meas[sub_id].tdscdma_sig_str.is_valid )
    {
      ind_msg->rscp                   = convert_global_tds_rscp(global.meas[sub_id].tdscdma_sig_str.rscp);
      ind_msg->rscp_valid             = TRUE;

      ind_msg->tdscdma_sig_info.rssi  = global.meas[sub_id].tdscdma_sig_str.rssi;
      ind_msg->tdscdma_sig_info.rscp  = global.meas[sub_id].tdscdma_sig_str.rscp;
      ind_msg->tdscdma_sig_info.ecio  = global.meas[sub_id].tdscdma_sig_str.ecio;
      ind_msg->tdscdma_sig_info.sinr  = global.meas[sub_id].tdscdma_sig_str.sinr;
      ind_msg->tdscdma_sig_info_valid = TRUE;
      retval = TRUE;
    }
  }
  else
  {
    QM_MSG_ERROR_1("sub_id not handled %d", sub_id);
  }

  return retval;
}

/*===========================================================================
  FUNCTION SEND_SIG_INFO_IND()

  DESCRIPTION
    This function populates and sends QMI_NAS_SIG_INFO_IND indicator to registered clients

  PARAMETERS   
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void send_sig_info_ind( void )
{
  enum qm_subs_e sub_id = QM_SUBS_MIN;
  uint8          index  = 0;
  int32          clid   = QM_NAS_INVALID_CLIENT;

  nas_sig_info_ind_msg_v01   *ind_msg     = NULL;
  dsm_item_type              * indication = NULL;

  clid = global.send_sig_info_ind[index];

  while( index < QM_NAS_MAX_NUM_CLIDS && clid != QM_NAS_INVALID_CLIENT )
  {
    // ask Client module which subscription this client is bound to
    sub_id = qm_nas_clnt_get_binding(clid);
    QM_MSG_HIGH_2("Populate & Sending sig_info_ind to clid:%d, sub_id:%d", clid, sub_id);

    ind_msg = (nas_sig_info_ind_msg_v01 *) modem_mem_alloc( sizeof( nas_sig_info_ind_msg_v01 ), MODEM_MEM_CLIENT_QMI_MMODE );    

    if( ind_msg && QM_NAS_SUB_ID_VALID(sub_id) )       
    {
      indication = NULL;
      memset(ind_msg, 0x00, sizeof(nas_sig_info_ind_msg_v01));
       
      if( populate_sig_info_ind( ind_msg, sub_id) )
      {
        if ( qmi_mmode_idl_message_encode( qmi_nasi_global.svc_obj,
                                           QMI_IDL_INDICATION,
                                           (uint16_t) QMI_NAS_SIG_INFO_IND_MSG_V01,
                                           (void *) ind_msg,
                                           (uint32_t) sizeof( nas_sig_info_ind_msg_v01 ),
                                           &indication 
                                          ) 
           )
        {
          qm_nas_clnt_send_ind( clid, QMI_NAS_SIG_INFO_IND_MSG_V01, indication );
        }
        else
        {
          QM_MSG_HIGH_2("idl_message_encode failed, no sig info indication sent clid %d, subsription %d", clid, sub_id);
        }
      }
      else
      {
        QM_MSG_HIGH_2("SIG_INFO_IND - no TLVs populated, nothing to send clid %d, subsription %d", clid, sub_id);
      }        
    }
    else
    {
      QM_MSG_ERROR_3("SIG_INFO_IND not sent ind_msg %x for clid %d, subscription %d", ind_msg, clid, sub_id);
    }

    if ( ind_msg != NULL ) { modem_mem_free( (void *) ind_msg, MODEM_MEM_CLIENT_QMI_MMODE ); }
    ind_msg = NULL;

    index++;
    if( index == QM_NAS_MAX_NUM_CLIDS ) 
    { 
      clid = QM_NAS_INVALID_CLIENT; 
    }
    else
    {
      clid = global.send_sig_info_ind[index];
    }
  }

  // clear all entries in global sig info ind array to QM_NAS_INVALID_CLIENT
  memset(&global.send_sig_info_ind, 0xFF, sizeof(global.send_sig_info_ind) );
}

/*===========================================================================
  FUNCTION POPULATE_ERROR_RATE_IND()

  DESCRIPTION
    This function populates QMI_NAS_ERR_RATE_IND message for a given subscription

  PARAMETERS   
    ind_msg : Pointer to ERR_RATE_IND IDL structure
    sub_id   : subscription id

  RETURN VALUE
    TRUE is population of TLVs is successful, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static boolean populate_err_rate_ind( nas_err_rate_ind_msg_v01 *ind_msg, enum qm_subs_e sub_id)
{
  boolean retval = FALSE;

  if( QM_NAS_SUB_ID_VALID(sub_id) )
  {
    // CDMA
    if( global.meas[sub_id].cdma_sig_str.is_valid )
    {
      ind_msg->cdma_frame_err_rate = (uint16)global.meas[sub_id].cdma_sig_str.frame_error_rate;
      ind_msg->cdma_frame_err_rate_valid = TRUE;
      retval = TRUE;
    }
    // HDR
    if( global.meas[sub_id].hdr_sig_str.is_valid )
    {
      ind_msg->hdr_packet_err_rate = (uint16)global.meas[sub_id].hdr_sig_str.packet_err_rate;
      ind_msg->hdr_packet_err_rate_valid = TRUE;
      retval = TRUE;
    }
    // GSM
    if( global.meas[sub_id].gsm_sig_str.is_valid )
    {
      ind_msg->gsm_bit_err_rate= (uint8)global.meas[sub_id].gsm_sig_str.bit_error_rate;
      ind_msg->gsm_bit_err_rate_valid = TRUE;
      retval = TRUE;
    }
    // WCDMA
    if( global.meas[sub_id].wcdma_sig_str.is_valid )
    {
      ind_msg->wcdma_block_err_rate = (uint8)global.meas[sub_id].wcdma_sig_str.block_error_rate;
      ind_msg->wcdma_block_err_rate_valid = TRUE;
      retval = TRUE;
    }
    // TDSCDMA
    if( global.meas[sub_id].tdscdma_sig_str.is_valid )
    {
      ind_msg->tdscdma_block_err_rate = (uint8)global.meas[sub_id].tdscdma_sig_str.bler;
      ind_msg->tdscdma_block_err_rate_valid = TRUE;
      retval = TRUE;
    }
  }
  else
  {
    QM_MSG_ERROR_1("sub_id not handled %d", sub_id);
  }

  return retval;
}

/*===========================================================================
  FUNCTION SEND_ERR_RATE_IND()

  DESCRIPTION
    This function populates and sends QMI_NAS_ERR_RATE_IND indicator to registered clients

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void send_err_rate_ind( void )
{
  enum qm_subs_e sub_id = QM_SUBS_MIN;
  uint8          index  = 0;
  int32          clid   = QM_NAS_INVALID_CLIENT;
  
  nas_err_rate_ind_msg_v01   *ind_msg     = NULL;
  dsm_item_type              * indication = NULL;
  
  clid = global.send_err_rate_ind[index];

  while( index < QM_NAS_MAX_NUM_CLIDS && clid != QM_NAS_INVALID_CLIENT )
  {
    // ask Client module which subscription this client is bound to
    sub_id = qm_nas_clnt_get_binding(clid);
    ind_msg = (nas_err_rate_ind_msg_v01 *) modem_mem_alloc( sizeof( nas_err_rate_ind_msg_v01 ), MODEM_MEM_CLIENT_QMI_MMODE );    
  
    if(ind_msg && QM_NAS_SUB_ID_VALID(sub_id) )       
    {
      indication = NULL;
      memset(ind_msg, 0x00, sizeof(nas_err_rate_ind_msg_v01));
     
      if( populate_err_rate_ind( ind_msg, sub_id) )
      { 
        if ( qmi_mmode_idl_message_encode( qmi_nasi_global.svc_obj,
                                           QMI_IDL_INDICATION,
                                           (uint16_t) QMI_NAS_ERR_RATE_IND_MSG_V01,
                                           (void *) ind_msg,
                                           (uint32_t) sizeof( nas_err_rate_ind_msg_v01 ),
                                           &indication 
                                          ) 
           )
        {
          qm_nas_clnt_send_ind( clid, QMI_NAS_ERR_RATE_IND_MSG_V01, indication );
        }
        else
        {
          QM_MSG_HIGH_2("idl_message_encode failed, no err rate indication sent clid %d, subscription %d", clid, sub_id);
        }
      }
      else
      {
        QM_MSG_HIGH_2("ERR_RATE_IND - no TLVs populated, nothing to send clid %d, subscription %d", clid, sub_id);
      }
    }
    else
    {
      QM_MSG_ERROR_3("ERR_RATE_IND not sent ind_msg %x for clid %d, subscription %d", ind_msg, clid, sub_id);
    }

    if ( ind_msg != NULL ) { modem_mem_free( (void *) ind_msg, MODEM_MEM_CLIENT_QMI_MMODE ); }
    ind_msg = NULL;

    index++;

    if( index == QM_NAS_MAX_NUM_CLIDS ) 
    { 
      clid = QM_NAS_INVALID_CLIENT; 
    }
    else
    {
      clid = global.send_err_rate_ind[index];
    }
  }

  // clear all entries in global err rate ind array to QM_NAS_INVALID_CLIENT
  memset(&global.send_err_rate_ind, 0xFF, sizeof(global.send_err_rate_ind) );
}

/*===========================================================================
  FUNCTION INVALIDATE_INACTIVE_RATS()

  DESCRIPTION
    This static function invalidates measurement for new inactive RATs in global struct.

  PARAMETERS
    sub_id : Subscription id
    inactive_rat_mask : bitmask representing newly inactive RATs

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void invalidate_inactive_rats( enum qm_subs_e sub_id, rat_bitmask_type inactive_rat_mask )
{
  QM_MSG_HIGH_1("invalidate_inactive_rats rat mask %d", inactive_rat_mask);

  //CDMA
  if( inactive_rat_mask & QM_RAT_MASK_C )
  { 
    global.meas[sub_id].cdma_sig_str.is_valid = FALSE;
    global.meas[sub_id].cdma_sig_str.rssi = QM_MEAS_INVALID_RSSI;
    global.meas[sub_id].cdma_sig_str.ecio = QM_MEAS_INVALID_ECIO;
    global.meas[sub_id].cdma_sig_str.frame_error_rate = QM_MEAS_INVALID_ERR_RATE;
  }
  //HDR
  if( inactive_rat_mask & QM_RAT_MASK_H )
  { 
    global.meas[sub_id].hdr_sig_str.is_valid = FALSE; 
    global.meas[sub_id].hdr_sig_str.rssi = QM_MEAS_INVALID_RSSI;
    global.meas[sub_id].hdr_sig_str.ecio = QM_MEAS_INVALID_ECIO;
    global.meas[sub_id].hdr_sig_str.sinr = QM_MEAS_INVALID_SINR;
    global.meas[sub_id].hdr_sig_str.io = QM_MEAS_INVALID_IO;
    global.meas[sub_id].hdr_sig_str.packet_err_rate = QM_MEAS_INVALID_ERR_RATE;
  }
  //GSM
  if( inactive_rat_mask & QM_RAT_MASK_G )
  { 
    global.meas[sub_id].gsm_sig_str.is_valid = FALSE; 
    global.meas[sub_id].gsm_sig_str.rssi = QM_MEAS_INVALID_RSSI;
    global.meas[sub_id].gsm_sig_str.bit_error_rate = QM_MEAS_INVALID_ERR_RATE;
  }
  //WCDMA
  if( inactive_rat_mask & QM_RAT_MASK_W )
  { 
    global.meas[sub_id].wcdma_sig_str.is_valid = FALSE; 
    global.meas[sub_id].wcdma_sig_str.rssi = QM_MEAS_INVALID_RSSI;
    global.meas[sub_id].wcdma_sig_str.ecio = QM_MEAS_INVALID_ECIO;
    global.meas[sub_id].wcdma_sig_str.block_error_rate = QM_MEAS_INVALID_ERR_RATE;
  }
  //LTE
  if( inactive_rat_mask & QM_RAT_MASK_L )
  { 
    global.meas[sub_id].lte_sig_str.is_valid = FALSE; 
    global.meas[sub_id].lte_sig_str.rssi = QM_MEAS_INVALID_RSSI;
    global.meas[sub_id].lte_sig_str.rsrq = QM_MEAS_INVALID_RSRQ;
    global.meas[sub_id].lte_sig_str.rsrp = QM_MEAS_INVALID_RSRP;
    global.meas[sub_id].lte_sig_str.snr = QM_MEAS_INVALID_SNR;
  }
  //TDSCDMA
  if( inactive_rat_mask & QM_RAT_MASK_T )
  { 
    global.meas[sub_id].tdscdma_sig_str.is_valid = FALSE; 
    global.meas[sub_id].tdscdma_sig_str.rssi = QM_MEAS_INVALID_RSSI;
    global.meas[sub_id].tdscdma_sig_str.rscp = QM_MEAS_INVALID_RSCP;
    global.meas[sub_id].tdscdma_sig_str.ecio = QM_MEAS_INVALID_ECIO;
    global.meas[sub_id].tdscdma_sig_str.sinr = QM_MEAS_INVALID_SINR;
    global.meas[sub_id].tdscdma_sig_str.bler = QM_MEAS_INVALID_ERR_RATE;
  }
}

/*===========================================================================
  FUNCTION CHECK_SEND_IND_CM_MEASUREMENT_EVENT()

  DESCRIPTION
    This static function checks for each client if we need to send SIG_INFO_IND and ERR_RATE_IND
    to it by quering QM CLIENT module. 

  PARAMETERS
    p_ss_info : Pointer to CM SS EVENT

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void check_send_ind_for_cm_measurement_event( cm_mm_msim_ss_info_s_type *p_ss_info )
{
  float rssi = 0, ecio = 0, sinr = 0, io = 0, rsrp = 0, rsrq = 0, snr = 0;
  uint32 bit_error_rate = 0;
  uint8  sig_info_ind_index      = 0;
  uint8  err_rate_ind_index = 0;
  int32  clid           = QM_NAS_INVALID_CLIENT;
  int i;

  enum qm_subs_e sub_id = QM_SUBS_MIN;

  // TODO: Add in checks for changed bitmasks
  if(p_ss_info)
  {
    sub_id = qm_util_map_cm_asubs_to_qm_subs(p_ss_info->asubs_id);;

    if(sub_id != QM_SUBS_MIN)
    {
      QM_MSG_MED("check_send_ind_for_cm_measurement_event: getting client list for sig_info_ind");
  
      // SIG INFO IND
      clid = qm_nas_clnt_get_next_client_for_sig_info_ind(QM_NAS_INVALID_CLIENT, sub_id);
  
      while ( QM_NAS_CLNT_ID_VALID(clid) )
      {
        for( i = 0; i < p_ss_info->number_of_stacks && i < STACK_INDEX_MAX; i++)
        {
          if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_CDMA )
          {
            rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
            ecio = convert_cm_ss_evt_ecio(p_ss_info->stack_info[i].ecio);
  
            if( qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_1X, QM_SIG_TYPE_RSSI, rssi, global.meas[sub_id].cdma_sig_str.rssi) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_1X, QM_SIG_TYPE_ECIO, ecio, global.meas[sub_id].cdma_sig_str.ecio) 
              )
            {
              global.send_sig_info_ind[sig_info_ind_index++] = clid;
              break;
            }        
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_HDR )
          {
            rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
            ecio = convert_cm_ss_evt_ecio(p_ss_info->stack_info[i].ecio);
            sinr = convert_cm_ss_evt_hdr_sinr(p_ss_info->stack_info[i].sinr);
            io   = convert_cm_ss_evt_hdr_io(p_ss_info->stack_info[i].io);  
  
            if( qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_HDR, QM_SIG_TYPE_RSSI, rssi, global.meas[sub_id].hdr_sig_str.rssi) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_HDR, QM_SIG_TYPE_ECIO, ecio, global.meas[sub_id].hdr_sig_str.ecio) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_HDR, QM_SIG_TYPE_SINR, sinr, global.meas[sub_id].hdr_sig_str.sinr) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_HDR, QM_SIG_TYPE_IO, io, global.meas[sub_id].hdr_sig_str.io)
              )
            {
              global.send_sig_info_ind[sig_info_ind_index++] = clid;
              break;
            }
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_GSM )
          {
            rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
  
            if( qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_GERAN, QM_SIG_TYPE_RSSI, rssi, global.meas[sub_id].gsm_sig_str.rssi) )
            {
              global.send_sig_info_ind[sig_info_ind_index++] = clid;
              break;
            }
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_WCDMA )
          {
            rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
            ecio = convert_cm_ss_evt_ecio(p_ss_info->stack_info[i].ecio);
  
            if( qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_UMTS, QM_SIG_TYPE_RSSI, rssi, global.meas[sub_id].wcdma_sig_str.rssi) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_UMTS, QM_SIG_TYPE_ECIO, ecio, global.meas[sub_id].wcdma_sig_str.ecio)
              )
            {
              global.send_sig_info_ind[sig_info_ind_index++] = clid;
              break;
            }
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_LTE )
          {
            rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi); 
            rsrq = convert_cm_ss_evt_lte_rsrq(p_ss_info->stack_info[i].rat_mode_info.lte_mode_info.rsrq);
            rsrp = convert_cm_ss_evt_lte_rsrp(p_ss_info->stack_info[i].rat_mode_info.lte_mode_info.rsrp);
            snr  = convert_cm_ss_evt_lte_snr(p_ss_info->stack_info[i].sinr);
  
            if( qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_LTE, QM_SIG_TYPE_RSSI, rssi, global.meas[sub_id].lte_sig_str.rssi) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_LTE, QM_SIG_TYPE_RSRQ, rsrq, global.meas[sub_id].lte_sig_str.rsrq) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_LTE, QM_SIG_TYPE_RSRP, rsrp, global.meas[sub_id].lte_sig_str.rsrp) ||
                qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_LTE, QM_SIG_TYPE_SNR, snr, global.meas[sub_id].lte_sig_str.snr)
              )
            {
              global.send_sig_info_ind[sig_info_ind_index++] = clid;
              break;
            }
          }
        }
  
        clid = qm_nas_clnt_get_next_client_for_sig_info_ind(clid, sub_id);
      }
  
      QM_MSG_MED("check_send_ind_for_cm_measurement_event: getting client list for err_rate_ind");
  
      // ERR RATE
      clid  = qm_nas_clnt_get_next_client_for_err_rate_ind(QM_NAS_INVALID_CLIENT, sub_id);
  
      while ( QM_NAS_CLNT_ID_VALID(clid) )
      {
        for( i = 0; i < p_ss_info->number_of_stacks && i < STACK_INDEX_MAX; i++ )
        {
          if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_CDMA )
          {
            bit_error_rate = convert_cm_ss_evt_err_rate(p_ss_info->stack_info[i].rat_mode_info.cdma_mode_info.frame_err_rate);
            if( global.meas[sub_id].cdma_sig_str.frame_error_rate != bit_error_rate )
            {
              global.send_err_rate_ind[err_rate_ind_index++] = clid;
              break;
            }
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_HDR )
          {
            bit_error_rate = convert_cm_ss_evt_err_rate(p_ss_info->stack_info[i].rat_mode_info.hdr_mode_info.hdr_packet_err_rate);
            if( global.meas[sub_id].hdr_sig_str.packet_err_rate != bit_error_rate )
            {
              global.send_err_rate_ind[err_rate_ind_index++] = clid;
              break;
            }
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_GSM )
          {
            bit_error_rate = convert_cm_ss_evt_gw_err_rate(p_ss_info->stack_info[i].mode_info.gw_info.bit_err_rate);
            if( global.meas[sub_id].gsm_sig_str.bit_error_rate != bit_error_rate )
            {
              global.send_err_rate_ind[err_rate_ind_index++] = clid;
              break;
            }
          }
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_WCDMA )
          {
            bit_error_rate = convert_cm_ss_evt_gw_err_rate(p_ss_info->stack_info[i].mode_info.gw_info.bit_err_rate);
            if( global.meas[sub_id].wcdma_sig_str.block_error_rate != bit_error_rate )
            {
              global.send_err_rate_ind[err_rate_ind_index++] = clid;
              break;
            }
          }
        }
        //There is no change in error rate so breaking.
        if(err_rate_ind_index == 0)
        {
          QM_MSG_HIGH("No change in error rat");
          break;
        }
  
        clid = qm_nas_clnt_get_next_client_for_err_rate_ind(clid, sub_id);
      }
    }
  }
  else
  {
    QM_MSG_ERROR("cm ss event pointer is NULL");
  }
}

#ifdef FEATURE_TDSCDMA
/*===========================================================================
  FUNCTION CHECK_SEND_IND_FOR_TDS_MEASUREMENTS()

  DESCRIPTION
    This function updates the global cache for TDSCDMA measurements

  PARAMETERS
    msg_p : Pointer to TDS measurement data
    sub_id : Subscription id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void check_send_ind_for_tds_measurements( tds_rrc_scell_signal_status_ind_type *tds_meas, enum qm_subs_e sub_id )
{
  float rscp = 0, rssi = 0, ecio = 0, sinr = 0; 
  uint32 bler = 0; boolean in_dch = FALSE;
  uint8  sig_info_ind_index = 0;
  uint8  err_rate_ind_index = 0;
  int32  clid  = QM_NAS_INVALID_CLIENT;

  /////////////////////////////////////////////////////////
  // MAIN Stack
  /////////////////////////////////////////////////////////  

  if( tds_meas )
  {
    clid = qm_nas_clnt_get_next_client_for_sig_info_ind(QM_NAS_INVALID_CLIENT, sub_id);

    while ( QM_NAS_CLNT_ID_VALID(clid) )
    {
      //values are scaled by 10 as they come from MSGR   
      rscp   = convert_msgr_evt_tds_rscp(tds_meas->rscp);
      rssi   = convert_msgr_evt_tds_rssi(tds_meas->rssi);
      ecio   = convert_msgr_evt_tds_ecio(tds_meas->ecio);
      sinr   = convert_msgr_evt_tds_sinr(tds_meas->sinr);
      in_dch = tds_meas->in_dch;

      if( qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_TDSCDMA, QM_SIG_TYPE_RSCP, rscp, global.meas[sub_id].tdscdma_sig_str.rscp) ||
          qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_TDSCDMA, QM_SIG_TYPE_RSSI, rssi, global.meas[sub_id].tdscdma_sig_str.rssi) || 
          qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_TDSCDMA, QM_SIG_TYPE_ECIO, ecio, global.meas[sub_id].tdscdma_sig_str.ecio) ||
          qm_nas_clnt_crossed_threshold(clid, sub_id, QM_RAT_TDSCDMA, QM_SIG_TYPE_SINR, sinr, global.meas[sub_id].tdscdma_sig_str.sinr) 
        )
      {
        QM_MSG_HIGH_3("Adding clid%d to send_sig_info_ind list, , sub_id %d, sig_info_ind_index:%d, TDS", clid, sub_id, sig_info_ind_index);
        global.send_sig_info_ind[sig_info_ind_index++] = clid;
      }        

      clid = qm_nas_clnt_get_next_client_for_sig_info_ind(clid, sub_id);
    } 

    // ERR RATE
    clid = qm_nas_clnt_get_next_client_for_err_rate_ind(QM_NAS_INVALID_CLIENT, sub_id);
  
    while( QM_NAS_CLNT_ID_VALID(clid) )
    {
      bler = convert_msgr_evt_tds_bler(tds_meas->bler);

      if( qm_si_is_rat_active(sub_id, QM_RAT_TDSCDMA) && 
          in_dch && (global.meas[sub_id].tdscdma_sig_str.bler != bler) 
        )
      {
        global.send_err_rate_ind[err_rate_ind_index++] = clid;
      }
      //There is no change in error rate so breaking.
      if(err_rate_ind_index == 0)
      {
        QM_MSG_HIGH("No change in error rat");
        break;
      }
      clid = qm_nas_clnt_get_next_client_for_err_rate_ind(clid, sub_id);
    }
  }
  else
  {
    QM_MSG_ERROR("TDS meas pointer is NULL");
  }
}
#endif

/*===========================================================================
  FUNCTION UPDATE_CM_MEASUREMENTS_TO_GLOBAL_STRUCT()

  DESCRIPTION
    This static function updates the global structure with received CM Measurement information on MAIN 
    Stack. 

  PARAMETERS
    p_ss_info : Pointer to CM SS EVENT

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void update_cm_measurements_to_global_struct(cm_mm_msim_ss_info_s_type *p_ss_info)
{
  enum qm_subs_e sub_id = QM_SUBS_MIN;
  int i;

  if(p_ss_info)
  {
    sub_id = qm_util_map_cm_asubs_to_qm_subs(p_ss_info->asubs_id);

    if( QM_NAS_SUB_ID_VALID(sub_id) )
    {
      for( i = 0; i < p_ss_info->number_of_stacks && i < STACK_INDEX_MAX; i++ )
      {
        // CDMA
        // TODO: add in changed fields mask checks
        if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_CDMA )
        {
          global.meas[sub_id].cdma_sig_str.rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
          global.meas[sub_id].cdma_sig_str.ecio = convert_cm_ss_evt_ecio(p_ss_info->stack_info[i].ecio);
          global.meas[sub_id].cdma_sig_str.frame_error_rate = convert_cm_ss_evt_err_rate(p_ss_info->stack_info[i].rat_mode_info.cdma_mode_info.frame_err_rate);
          global.meas[sub_id].cdma_sig_str.is_valid = TRUE;
          QM_MSG_HIGH_2("CDMA rssi %d, ecio %d", p_ss_info->stack_info[i].rssi, p_ss_info->stack_info[i].ecio);
        }    
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_HDR )
        {
          global.meas[sub_id].hdr_sig_str.rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
          global.meas[sub_id].hdr_sig_str.ecio = convert_cm_ss_evt_ecio(p_ss_info->stack_info[i].ecio);
          global.meas[sub_id].hdr_sig_str.sinr = convert_cm_ss_evt_hdr_sinr(p_ss_info->stack_info[i].sinr);
          global.meas[sub_id].hdr_sig_str.io   = convert_cm_ss_evt_hdr_io(p_ss_info->stack_info[i].io);
          global.meas[sub_id].hdr_sig_str.packet_err_rate = convert_cm_ss_evt_err_rate(p_ss_info->stack_info[i].rat_mode_info.hdr_mode_info.hdr_packet_err_rate);
          global.meas[sub_id].hdr_sig_str.is_valid = TRUE;
          QM_MSG_HIGH_5("HDR rssi %d, ecio %d, sinr %d, io %d, err_rate %d", p_ss_info->stack_info[i].rssi, p_ss_info->stack_info[i].ecio, 
             p_ss_info->stack_info[i].sinr, p_ss_info->stack_info[i].io, p_ss_info->stack_info[i].rat_mode_info.hdr_mode_info.hdr_packet_err_rate);

        }    
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_GSM )
        {
          global.meas[sub_id].gsm_sig_str.rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi);
          global.meas[sub_id].gsm_sig_str.bit_error_rate = convert_cm_ss_evt_gw_err_rate(p_ss_info->stack_info[i].mode_info.gw_info.bit_err_rate);
          global.meas[sub_id].gsm_sig_str.is_valid = TRUE;
          QM_MSG_HIGH_2("GSM rssi %d, err_rate %d", p_ss_info->stack_info[i].rssi, p_ss_info->stack_info[i].mode_info.gw_info.bit_err_rate);
        }    
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_WCDMA )
        {
          global.meas[sub_id].wcdma_sig_str.rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi); 
          global.meas[sub_id].wcdma_sig_str.ecio = convert_cm_ss_evt_ecio(p_ss_info->stack_info[i].ecio);
          global.meas[sub_id].wcdma_sig_str.block_error_rate = convert_cm_ss_evt_gw_err_rate(p_ss_info->stack_info[i].mode_info.gw_info.bit_err_rate);
          global.meas[sub_id].wcdma_sig_str.is_valid = TRUE;
          QM_MSG_HIGH_3("WCDMA rssi %d, ecio %d, err_rate %d", p_ss_info->stack_info[i].rssi, p_ss_info->stack_info[i].ecio, p_ss_info->stack_info[i].mode_info.gw_info.bit_err_rate);
        }    
          else if( p_ss_info->stack_info[i].sys_mode == SYS_SYS_MODE_LTE )
        {
          global.meas[sub_id].lte_sig_str.rssi = convert_cm_ss_evt_rssi(p_ss_info->stack_info[i].rssi); 
          global.meas[sub_id].lte_sig_str.rsrq = convert_cm_ss_evt_lte_rsrq(p_ss_info->stack_info[i].rat_mode_info.lte_mode_info.rsrq);
          global.meas[sub_id].lte_sig_str.rsrp = convert_cm_ss_evt_lte_rsrp(p_ss_info->stack_info[i].rat_mode_info.lte_mode_info.rsrp);
          global.meas[sub_id].lte_sig_str.snr  = convert_cm_ss_evt_lte_snr(p_ss_info->stack_info[i].sinr );
          global.meas[sub_id].lte_sig_str.is_valid = TRUE;
          QM_MSG_HIGH_4("LTE rssi %d, rsrq %d, rsrp %d, sinr %d", p_ss_info->stack_info[i].rssi, p_ss_info->stack_info[i].rat_mode_info.lte_mode_info.rsrq, 
                                                                                       p_ss_info->stack_info[i].rat_mode_info.lte_mode_info.rsrp, p_ss_info->stack_info[i].sinr);
        }
        else
        {
            QM_MSG_ERROR_2("dropping measurements, sub id %d sys_mode %d", sub_id, p_ss_info->stack_info[i].sys_mode);
        }
      }
    }
  }
  else
  {
    QM_MSG_ERROR("cm ss event pointer is NULL");
  }
}

#ifdef FEATURE_TDSCDMA
/*===========================================================================
  FUNCTION UPDATE_TDS_MEASUREMENTS_TO_GLOBAL_STRUCT()

  DESCRIPTION
    This static function updates the global structure with received CM Measurement information on GW
    Hybrid Stack. 

  PARAMETERS
    msg_p : Pointer to MSGR data
    sub_id : Subscription id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
static void update_tds_measurements_to_global_struct( tds_rrc_scell_signal_status_ind_type *tds_meas, enum qm_subs_e sub_id )
{
  if(tds_meas && QM_NAS_SUB_ID_VALID(sub_id) )
  {
    global.meas[sub_id].tdscdma_sig_str.is_valid = TRUE;
    global.meas[sub_id].tdscdma_sig_str.rscp = convert_msgr_evt_tds_rscp(tds_meas->rscp);
    global.meas[sub_id].tdscdma_sig_str.rssi = convert_msgr_evt_tds_rssi(tds_meas->rssi);
    global.meas[sub_id].tdscdma_sig_str.ecio = convert_msgr_evt_tds_ecio(tds_meas->ecio);
    global.meas[sub_id].tdscdma_sig_str.in_dch = tds_meas->in_dch;

    if( tds_meas->in_dch )
    {
      global.meas[sub_id].tdscdma_sig_str.sinr = convert_msgr_evt_tds_sinr(tds_meas->sinr);
      global.meas[sub_id].tdscdma_sig_str.bler = convert_msgr_evt_tds_bler(tds_meas->bler);
    }
    else
    {
      global.meas[sub_id].tdscdma_sig_str.sinr = QM_MEAS_INVALID_SINR;
      global.meas[sub_id].tdscdma_sig_str.bler = QM_MEAS_INVALID_ERR_RATE;
    }

    QM_MSG_HIGH_6("TDSCDMA SIGNAL IND rscp = %d, rssi = %d, ecio = %d, in_dch = %d, sinr = %d, bler = %d", 
                                 tds_meas->rscp, tds_meas->rssi, tds_meas->ecio, tds_meas->sinr, tds_meas->bler, tds_meas->in_dch);
  }
  else
  {
    QM_MSG_ERROR_1("TDS meas pointer is NULL or subscription invalid %d", sub_id);
  }
}
#endif

/*===========================================================================
  FUNCTION QM_MEAS_INIT()

  DESCRIPTION
    This function initializes the measurement module global data

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    This function should be called before qmi_nas_init() during initialization process
===========================================================================*/
void qm_meas_init ( void )
{
  memset(&global, 0x00, sizeof(struct qm_meas_s) );
  memset(&global.send_sig_info_ind, 0xFF, sizeof(global.send_sig_info_ind));
  memset(&global.send_err_rate_ind, 0xFF, sizeof(global.send_err_rate_ind));
}

/*===========================================================================
  FUNCTION QM_MEAS_PROCESS_CM_MEASUREMENT_EVENT()

  DESCRIPTION
    This function processes CM SS EVENT. If Event type is handled, this function checks and 
    updates the global measurement data and calls routines to send QMI_NAS_SIG_INFO_IND 
    and QMI_NAS_ERR_RATE_IND messages to registered clients.

  PARAMETERS
    p_ss_info : Pointer to received CM SS EVENT
    evt         : CM SS EVENT type

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_meas_process_cm_measurement_event( cm_mm_msim_ss_info_s_type *p_ss_info, cm_ss_event_e_type evt )
{
  QM_MSG_HIGH_1("In qm_meas_process_cm_measurement_event evt %d", evt);

  if( p_ss_info )
  {
    switch(evt)
    {
      case CM_SS_EVENT_RSSI:
      case CM_SS_EVENT_HDR_RSSI:
      case CM_SS_EVENT_GW_RSSI:
        check_send_ind_for_cm_measurement_event(p_ss_info);
        update_cm_measurements_to_global_struct(p_ss_info);
        break;

      default:
        QM_MSG_ERROR_1("cm ss event not handled evt %d", evt);
        break;
    }

    send_sig_info_ind();
    send_err_rate_ind();
  }
  else
  {
    QM_MSG_ERROR("cm ss event pointer is NULL");
  }
}

#ifdef FEATURE_TDSCDMA
/*===========================================================================
  FUNCTION QM_MEAS_PROCESS_TDS_MEASUREMENT_INFO()

  DESCRIPTION
    This function processes MSG Router TDSCDMA Events. If TDSCDMA is in service, this function checks 
    and updates the global measurement data and calls routines to send QMI_NAS_SIG_INFO_IND and 
    QMI_NAS_ERR_RATE_IND messages to registered clients.

  PARAMETERS
    tds_meas : Pointer to TDS measurement data from MSGR

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_meas_process_tds_measurement_info( tds_rrc_scell_signal_status_ind_type *tds_meas )
{
  enum qm_subs_e sub_id = QM_SUBS_PRI;

#ifdef FEATURE_DUAL_SIM
  if(tds_meas->as_id == SYS_MODEM_AS_ID_2)
  {
    sub_id = QM_SUBS_SEC;
  }
#endif
#ifdef FEATURE_TRIPLE_SIM  
  if(tds_meas->as_id == SYS_MODEM_AS_ID_3)
  {
    sub_id = QM_SUBS_TER;
  }
#endif

  check_send_ind_for_tds_measurements( tds_meas, sub_id );
  update_tds_measurements_to_global_struct(tds_meas, sub_id);
  send_sig_info_ind();
  send_err_rate_ind();

}
#endif

/*===========================================================================
  FUNCTION QM_MEAS_POPULATE_GET_SIG_INFO_RESP()

  DESCRIPTION
    This function populates SIG INFO response message based on Measurement module private data.

  PARAMETERS
    rsp_msg : Pointer to SIG INFO response IDL Structure
    sub_id   : Subscription id

  RETURN VALUE
    Relevant error is returned if population routine fails, QMI_ERR_NONE_V01 is returned otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qm_meas_populate_get_sig_info_resp( nas_get_sig_info_resp_msg_v01 *rsp_msg, enum qm_subs_e sub_id )
{
  qmi_error_type_v01 errval = QMI_ERR_INFO_UNAVAILABLE_V01;

  if( qm_si_is_rat_active(sub_id, QM_RAT_1X) )
  {
    // include CDMA measurements if they are valid in global struct 
    if( global.meas[sub_id].cdma_sig_str.is_valid )
    {
      rsp_msg->cdma_sig_info.rssi = convert_global_rssi(global.meas[sub_id].cdma_sig_str.rssi);
      rsp_msg->cdma_sig_info.ecio = convert_global_ecio(global.meas[sub_id].cdma_sig_str.ecio);
      rsp_msg->cdma_sig_info_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_HDR) && !qm_si_is_rat_active(sub_id, QM_RAT_UMTS) && !qm_si_is_rat_active(sub_id, QM_RAT_GERAN) && !qm_si_is_rat_active(sub_id, QM_RAT_TDSCDMA) )
  {
    // include HDR measurements if they are valid in global struct 
    if( global.meas[sub_id].hdr_sig_str.is_valid )
    {
      rsp_msg->hdr_sig_info.common_sig_str.rssi = convert_global_rssi(global.meas[sub_id].hdr_sig_str.rssi);
      rsp_msg->hdr_sig_info.common_sig_str.ecio = convert_global_ecio(global.meas[sub_id].hdr_sig_str.ecio);
      rsp_msg->hdr_sig_info.sinr = convert_global_hdr_sinr(global.meas[sub_id].hdr_sig_str.sinr);
      rsp_msg->hdr_sig_info.io = convert_global_hdr_io(global.meas[sub_id].hdr_sig_str.io);
      rsp_msg->hdr_sig_info_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_GERAN) )
  {
    // include GSM measurements if they are valid in global struct 
    if( global.meas[sub_id].gsm_sig_str.is_valid )
    {
      rsp_msg->gsm_sig_info = convert_global_rssi(global.meas[sub_id].gsm_sig_str.rssi);
      rsp_msg->gsm_sig_info_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_UMTS) )
  {
    // include UMTS measurements if they are valid in global struct 
    if( global.meas[sub_id].wcdma_sig_str.is_valid )
    {
      rsp_msg->wcdma_sig_info.rssi = convert_global_rssi(global.meas[sub_id].wcdma_sig_str.rssi);
      rsp_msg->wcdma_sig_info.ecio = convert_global_ecio(global.meas[sub_id].wcdma_sig_str.ecio);
      rsp_msg->wcdma_sig_info_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_LTE) )
  {
    // include LTE measurements if they are valid in global struct 
    if( global.meas[sub_id].lte_sig_str.is_valid )
    {
      rsp_msg->lte_sig_info.rssi = convert_global_rssi(global.meas[sub_id].lte_sig_str.rssi);
      rsp_msg->lte_sig_info.rsrq = convert_global_lte_rsrq(global.meas[sub_id].lte_sig_str.rsrq);
      rsp_msg->lte_sig_info.rsrp = convert_global_lte_rsrp(global.meas[sub_id].lte_sig_str.rsrp);
      rsp_msg->lte_sig_info.snr  = convert_global_lte_snr(global.meas[sub_id].lte_sig_str.snr);
      rsp_msg->lte_sig_info_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_TDSCDMA) )
  {
    // include TDSCDMA measurements if they are valid in global struct 
    if( global.meas[sub_id].tdscdma_sig_str.is_valid )
    {
      rsp_msg->rscp                   = convert_global_tds_rscp(global.meas[sub_id].tdscdma_sig_str.rscp);
      rsp_msg->rscp_valid             = TRUE;

      rsp_msg->tdscdma_sig_info.rssi  = global.meas[sub_id].tdscdma_sig_str.rssi;
      rsp_msg->tdscdma_sig_info.rscp  = global.meas[sub_id].tdscdma_sig_str.rscp;
      rsp_msg->tdscdma_sig_info.ecio  = global.meas[sub_id].tdscdma_sig_str.ecio;
      rsp_msg->tdscdma_sig_info.sinr  = global.meas[sub_id].tdscdma_sig_str.sinr;
      rsp_msg->tdscdma_sig_info_valid = TRUE;
    }
  }

  if( rsp_msg->cdma_sig_info_valid  || rsp_msg->hdr_sig_info_valid || rsp_msg->gsm_sig_info_valid     ||
      rsp_msg->wcdma_sig_info_valid || rsp_msg->lte_sig_info_valid || rsp_msg->tdscdma_sig_info_valid
    )
  {
    errval = QMI_ERR_NONE_V01;
  }

  return errval;
}

/*===========================================================================
  FUNCTION QM_MEAS_POPULATE_GET_ERR_RATE_RESP()

  DESCRIPTION
    This function populates ERR RATE response message based on Measurement module private data.

  PARAMETERS
    rsp_msg : Pointer to ERR RATE response IDL Structure
    sub_id   : Subscription id

  RETURN VALUE
    Relevant error is returned if population routine fails, QMI_ERR_NONE_V01 is returned otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_error_type_v01 qm_meas_populate_get_err_rate_resp( nas_get_err_rate_resp_msg_v01 *rsp_msg, enum qm_subs_e sub_id )
{  
  qmi_error_type_v01 errval = QMI_ERR_INFO_UNAVAILABLE_V01;

  if( qm_si_is_rat_active(sub_id, QM_RAT_1X) )
  {
    // include CDMA ERR RATE if it is valid in global struct 
    if( global.meas[sub_id].cdma_sig_str.is_valid )
    {
      rsp_msg->cdma_frame_err_rate = (uint16)global.meas[sub_id].cdma_sig_str.frame_error_rate;
      rsp_msg->cdma_frame_err_rate_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_HDR) )
  {
    // include HDR ERR RATE if it is valid in global struct 
    if( global.meas[sub_id].hdr_sig_str.is_valid )
    {
      rsp_msg->hdr_packet_err_rate = (uint16)global.meas[sub_id].hdr_sig_str.packet_err_rate;
      rsp_msg->hdr_packet_err_rate_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_GERAN) )
  {
    // include GSM ERR RATE if it is valid in global struct 
    if( global.meas[sub_id].gsm_sig_str.is_valid )
    {
      rsp_msg->gsm_bit_err_rate = (uint8)global.meas[sub_id].gsm_sig_str.bit_error_rate;
      rsp_msg->gsm_bit_err_rate_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_UMTS) )
  {
    // include UMTS ERR RATE if it is valid in global struct 
    if( global.meas[sub_id].wcdma_sig_str.is_valid )
    {
      rsp_msg->wcdma_block_err_rate = (uint8)global.meas[sub_id].wcdma_sig_str.block_error_rate;
      rsp_msg->wcdma_block_err_rate_valid = TRUE;
    }
  }
  if( qm_si_is_rat_active(sub_id, QM_RAT_TDSCDMA) )
  {
    // include ERR RATE if it is valid in global struct 
    if( global.meas[sub_id].tdscdma_sig_str.is_valid )
    {
      rsp_msg->tdscdma_block_err_rate = (uint8)global.meas[sub_id].tdscdma_sig_str.bler;
      rsp_msg->tdscdma_block_err_rate_valid = TRUE;
    }
  } 

  if( rsp_msg->cdma_frame_err_rate_valid  || rsp_msg->hdr_packet_err_rate_valid || rsp_msg->gsm_bit_err_rate_valid ||
      rsp_msg->wcdma_block_err_rate_valid || rsp_msg->tdscdma_block_err_rate_valid
    )
  {
    errval = QMI_ERR_NONE_V01;
  }

  return errval;
}

/*===========================================================================
  FUNCTION QM_MEAS_REPORT_NEW_ACTIVE_RATS()

  DESCRIPTION
    This function reports to Measurement module which new RATs came in service for a subscription id.
    SIG INFO IND and ERR RATE IND are triggered to clients if measurement data is available for active RAT.

  PARAMETERS
    sub_id : subscription id
    active_rat_mask : bitmask indicating which new RATs came in service

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    This function is called from SYS_INFO module when CM SS EVENT (SRV_CHANGED) is received.
===========================================================================*/
void qm_meas_report_new_active_rats( enum qm_subs_e sub_id, rat_bitmask_type active_rat_mask)
{
  int32  clid  = QM_NAS_INVALID_CLIENT;
 
  nas_sig_info_ind_msg_v01   *sig_ind     = NULL;
  nas_err_rate_ind_msg_v01   *err_ind     = NULL;

  dsm_item_type              * indication = NULL;

  QM_MSG_HIGH_1("new active rat reported %d", active_rat_mask);

  // do I need to send SIG_INFO_IND(check for client)
  clid = qm_nas_clnt_get_next_client_for_sig_info_ind(QM_NAS_INVALID_CLIENT, sub_id);
  while ( QM_NAS_CLNT_ID_VALID(clid) )
  {
    // If Yes, send SIG_INFO_IND if you need to 
    if( ((active_rat_mask & QM_RAT_MASK_C) && global.meas[sub_id].cdma_sig_str.is_valid)    ||
        ((active_rat_mask & QM_RAT_MASK_H) && global.meas[sub_id].hdr_sig_str.is_valid)     ||
        ((active_rat_mask & QM_RAT_MASK_G) && global.meas[sub_id].gsm_sig_str.is_valid)     ||
        ((active_rat_mask & QM_RAT_MASK_W) && global.meas[sub_id].wcdma_sig_str.is_valid)   ||
        ((active_rat_mask & QM_RAT_MASK_L) && global.meas[sub_id].lte_sig_str.is_valid)     ||
        ((active_rat_mask & QM_RAT_MASK_T) && global.meas[sub_id].tdscdma_sig_str.is_valid)
      )
    {
      sig_ind = (nas_sig_info_ind_msg_v01 *) modem_mem_alloc( sizeof( nas_sig_info_ind_msg_v01 ), MODEM_MEM_CLIENT_QMI_MMODE );    
   
      if(sig_ind)
      {
        indication = NULL;
        memset(sig_ind, 0x00, sizeof(nas_sig_info_ind_msg_v01));
       
        if( populate_sig_info_ind( sig_ind, sub_id) )
        {
          if ( qmi_mmode_idl_message_encode( qmi_nasi_global.svc_obj,
                                             QMI_IDL_INDICATION,
                                             (uint16_t) QMI_NAS_SIG_INFO_IND_MSG_V01,
                                             (void *) sig_ind,
                                             (uint32_t) sizeof( nas_sig_info_ind_msg_v01 ),
                                             &indication 
                                            ) 
             )
          {
            qm_nas_clnt_send_ind( clid, QMI_NAS_SIG_INFO_IND_MSG_V01, indication );
          }
          else
          {
            QM_MSG_HIGH_1("new active rat - idl_message_encode failed, no sig info indication sent %d.", clid);
          }
        }
        else
        {
          QM_MSG_HIGH_1("SIG_INFO_IND - no TLVs populated, nothing to send clid %d", clid);
        }
      }
      else
      {
        QM_MSG_ERROR_1("no memory to send SIG_INFO_IND for clid %d", clid);
      }

      if ( sig_ind != NULL ) { modem_mem_free( (void *) sig_ind, MODEM_MEM_CLIENT_QMI_MMODE ); }
      sig_ind = NULL;
    }

    clid = qm_nas_clnt_get_next_client_for_sig_info_ind(clid, sub_id);
  }


  // do I need to send ERR_RATE_IND(check for client)
  clid = qm_nas_clnt_get_next_client_for_err_rate_ind(QM_NAS_INVALID_CLIENT, sub_id);    
  while ( QM_NAS_CLNT_ID_VALID(clid) )
  {
    // If Yes, send ERR_RATE_IND if you need to 
    if( ((active_rat_mask & QM_RAT_MASK_C) && global.meas[sub_id].cdma_sig_str.is_valid)    ||
        ((active_rat_mask & QM_RAT_MASK_H) && global.meas[sub_id].hdr_sig_str.is_valid)     ||
        ((active_rat_mask & QM_RAT_MASK_G) && global.meas[sub_id].gsm_sig_str.is_valid)     ||
        ((active_rat_mask & QM_RAT_MASK_W) && global.meas[sub_id].wcdma_sig_str.is_valid)   ||
        ((active_rat_mask & QM_RAT_MASK_L) && global.meas[sub_id].lte_sig_str.is_valid)     ||
        ((active_rat_mask & QM_RAT_MASK_T) && global.meas[sub_id].tdscdma_sig_str.is_valid)
      )
    {
      err_ind = (nas_err_rate_ind_msg_v01 *) modem_mem_alloc( sizeof( nas_err_rate_ind_msg_v01 ), MODEM_MEM_CLIENT_QMI_MMODE );    
 
      if(err_ind)       
      {
        indication = NULL;
        memset(err_ind, 0x00, sizeof(nas_err_rate_ind_msg_v01));
       
        if( populate_err_rate_ind( err_ind, sub_id) )
        {
          if ( qmi_mmode_idl_message_encode( qmi_nasi_global.svc_obj,
                                             QMI_IDL_INDICATION,
                                             (uint16_t) QMI_NAS_ERR_RATE_IND_MSG_V01,
                                             (void *) err_ind,
                                             (uint32_t) sizeof( nas_err_rate_ind_msg_v01 ),
                                             &indication 
                                            ) 
             )
          {
            qm_nas_clnt_send_ind( clid, QMI_NAS_ERR_RATE_IND_MSG_V01, indication );
          }
          else
          {
            QM_MSG_HIGH_1("new active rat - idl_message_encode failed, no err rate ind sent %d.", clid);
          }
        }
        else
        {
          QM_MSG_HIGH_1("ERR_RATE_IND - no TLVs populated, nothing to send clid %d", clid);
        }
      }
      else
      {
        QM_MSG_ERROR_1("no memory to send ERR_RATE_IND for clid %d", clid);
      }

      if ( err_ind != NULL ) { modem_mem_free( (void *) err_ind, MODEM_MEM_CLIENT_QMI_MMODE ); }
      err_ind = NULL;
    }

    clid = qm_nas_clnt_get_next_client_for_err_rate_ind(clid, sub_id);
  }
}

/*===========================================================================
  FUNCTION QM_MEAS_REPORT_NEW_INACTIVE_RATS()

  DESCRIPTION
    This function reports to Measurement module which new RATs went out of service for a subscription id.
    Measurement data is invalidated for inactive RATs

  PARAMETERS
    sub_id : subscription id
    inactive_rat_mask : bitmask indicating which new RATs came in service

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    This function is called from SYS_INFO module when CM SS EVENT (SRV_CHANGED) is received.
===========================================================================*/
void qm_meas_report_new_inactive_rats( enum qm_subs_e sub_id, rat_bitmask_type inactive_rat_mask)
{
  invalidate_inactive_rats(sub_id, inactive_rat_mask);
}

