
/*===========================================================================
 
                    Callflow Analysis Logging Source File
 
DESCRIPTION
   Required for NAS logging.
 
Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef FEATURE_LTE
#include "comdef.h"
#include "amssassert.h"
#include "naslog_v.h"
#include "lte_log_codes.h"
#include "lte_nas_log.h"
#include "err.h"
#include "event.h"
#include "event_defs.h"
#include <stringl/stringl.h>
/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/
#define LTE_NAS_DIAG_LOG_VERSION 1
#define LTE_NAS_DIAG_UPDATED_LOG_VERSION 2
#define LOG_PTR_SIZE sizeof(log_hdr_type)

/*===========================================================================



                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/lte_nas_log.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/09   hnam    Added support for forbidden tracking are list & GUTI in EMM STATE log packet
24/06/09   hnam    Changed the function definition param name for 
                    send_emm_usimcard_mode_log_info
06/19/09   hnam     Initial Revision
===========================================================================*/

/*---------------------------------------------------------------------
                    ESM LOGGING FUNCTIONS
---------------------------------------------------------------------*/

void send_esm_bearer_ctxt_state_log_info
(
  lte_nas_esm_bearer_context_state_T  *esm_bearer_ctxt_state_log_info
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  byte pkt_size;

  ASSERT(esm_bearer_ctxt_state_log_info != NULL);

  /*Add the DIAG log version*/
  esm_bearer_ctxt_state_log_info->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_esm_bearer_context_state_T);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0E4 not logged\n");
    return;
  }
  
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_esm_bearer_context_state_T),esm_bearer_ctxt_state_log_info, sizeof(lte_nas_esm_bearer_context_state_T));
  log_commit((void*)diag_log_ptr);
  
}

void send_esm_bearer_ctxt_log_info
(
  lte_nas_esm_bearer_context_info_T  *esm_bearer_ctxt_log_info
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(esm_bearer_ctxt_log_info != NULL);

  /*Add the DIAG log version*/
  esm_bearer_ctxt_log_info->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_esm_bearer_context_info_T);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0E5 not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_esm_bearer_context_info_T),esm_bearer_ctxt_log_info, sizeof(lte_nas_esm_bearer_context_info_T));
  log_commit((void*)diag_log_ptr);
}

void send_esm_proc_state_log_info
(
  lte_nas_esm_procedure_state_T  *esm_proc_state_log_info
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(esm_proc_state_log_info != NULL);

  /*Add the DIAG log version*/
  esm_proc_state_log_info->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_esm_procedure_state_T);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_ESM_PROCEDURE_STATE_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0E6 not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_esm_procedure_state_T),esm_proc_state_log_info, sizeof(lte_nas_esm_procedure_state_T));
  log_commit((void*)diag_log_ptr);
}


/*---------------------------------------------------------------------
                    EMM LOGGING FUNCTIONS
---------------------------------------------------------------------*/

void send_emm_state_log_info
(
  byte                emm_state,
  byte                emm_substate, /*Contradicts -> emm_substate is of type 'word" in emm_database.h*/
  sys_plmn_id_s_type     *plmn,
  boolean                guti_valid,
  lte_nas_emm_guti_type  *guti  
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;
  lte_nas_emm_state_type emm_state_log;
  //Initializing guti to FF (Invalid guti)
  //memcpy(&emm_state_log.guti,0xFF,sizeof(lte_nas_emm_guti_log_type));
  //byte guti_id = 6;/*6 is the enum value of GUTI ID*/

  /*Initialize plmn*/
  memset(&emm_state_log.plmn,0xFF, sizeof(sys_plmn_id_s_type));

  /*Add the DIAG log version*/
  emm_state_log.log_version = LTE_NAS_DIAG_UPDATED_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_emm_state_type);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_STATE_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0EE not logged\n");
    return;
  }
  //emm_state_log.log_version = emm_val;
  emm_state_log.emm_state = emm_state;
  emm_state_log.emm_substate = emm_substate;
  memscpy(&emm_state_log.plmn.identity[0],sizeof(sys_plmn_id_s_type),plmn,3);
  if(guti_valid)
  {
    //memcpy(&emm_state_log.guti,guti,sizeof(lte_nas_emm_guti_log_type));
    /*************************************************************************
     UE_ID is an enum type, so it takes 4 bytes (i.e. three extra bytes are 
     padded into the *guti structure, so memcpy()'ing this structure to the 
     logging function will contain 3 extra bytes in the middle and GUTI 
     parameters following UE_ID will be unrecognized
    **************************************************************************/
    emm_state_log.guti.ue_id            = (byte)guti->ue_id;
    emm_state_log.guti.plmn.identity[0] = guti->plmn.identity[0];
    emm_state_log.guti.plmn.identity[1] = guti->plmn.identity[1];
    emm_state_log.guti.plmn.identity[2] = guti->plmn.identity[2];
    emm_state_log.guti.mme_group_id[0]  = guti->mme_group_id[0];
    emm_state_log.guti.mme_group_id[1]  = guti->mme_group_id[1];
    emm_state_log.guti.mme_code         = guti->mme_code;
    emm_state_log.guti.m_tmsi[0]        = guti->m_tmsi[0];
    emm_state_log.guti.m_tmsi[1]        = guti->m_tmsi[1];
    emm_state_log.guti.m_tmsi[2]        = guti->m_tmsi[2];
    emm_state_log.guti.m_tmsi[3]        = guti->m_tmsi[3];
    emm_state_log.guti_valid            = (byte)guti_valid;
  }
  //if guti_valid == FALSE, fill emm_state_log with invalid info
  else
  {
    emm_state_log.guti.ue_id            = (byte)0xFF;
    emm_state_log.guti.plmn.identity[0] = 0xFF;
    emm_state_log.guti.plmn.identity[1] = 0xFF;
    emm_state_log.guti.plmn.identity[2] = 0xFF;
    emm_state_log.guti.mme_group_id[0]  = 0xFF;
    emm_state_log.guti.mme_group_id[1]  = 0xFF;
    emm_state_log.guti.mme_code         = 0xFF;
    emm_state_log.guti.m_tmsi[0]        = 0xFF;
    emm_state_log.guti.m_tmsi[1]        = 0xFF;
    emm_state_log.guti.m_tmsi[2]        = 0xFF;
    emm_state_log.guti.m_tmsi[3]        = 0xFF;
    emm_state_log.guti_valid            = (byte)guti_valid;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  if(temp_ptr != NULL)
  {
    memscpy(temp_ptr,sizeof(lte_nas_emm_state_type),&emm_state_log, sizeof(lte_nas_emm_state_type));
  }
  else
  {
    MSG_ERROR_0("NAS: LOG_LTE_NAS_EMM_STATE_LOG_C log packet could not be logged");
  }
  log_commit((void*)diag_log_ptr);
}

void send_emm_usimcard_mode_log_info
(
  lte_nas_emm_usim_card_mode_type  *emm_usim_card_mode_info
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(emm_usim_card_mode_info != NULL);

  /*Add the DIAG log version*/
  emm_usim_card_mode_info->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_emm_usim_card_mode_type);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_USIM_CARD_MODE_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0EF not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_emm_usim_card_mode_type),emm_usim_card_mode_info, sizeof(lte_nas_emm_usim_card_mode_type));
  log_commit((void*)diag_log_ptr);
}


void send_emm_current_ctxt_sec_log_info
(
  lte_nas_emm_current_sec_context_type  *emm_current_sec_ctxt_log
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(emm_current_sec_ctxt_log != NULL);

  /*Add the DIAG log version*/
  emm_current_sec_ctxt_log->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_emm_current_sec_context_type);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_CURRENT_SECUIRY_CONTEXT_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0F2 not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_emm_current_sec_context_type),emm_current_sec_ctxt_log, sizeof(lte_nas_emm_current_sec_context_type));
  log_commit((void*)diag_log_ptr);
}

void send_emm_ps_domain_int_cipher_log_info
(
  lte_nas_emm_ps_domain_int_cipher_keys  *emm_ps_domain_int_cipher_log
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(emm_ps_domain_int_cipher_log != NULL);

  /*Add the DIAG log version*/
  emm_ps_domain_int_cipher_log->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_emm_ps_domain_int_cipher_keys);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_CKIK_FOR_PS_DOMAIN_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0F3 not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_emm_ps_domain_int_cipher_keys),emm_ps_domain_int_cipher_log, sizeof(lte_nas_emm_ps_domain_int_cipher_keys));
  log_commit((void*)diag_log_ptr);
}

void send_emm_native_sec_ctxt_log_info
(
  lte_nas_emm_native_sec_context_type  *emm_native_sec_ctxt_log
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(emm_native_sec_ctxt_log != NULL);

  /*Add the DIAG log version*/
  emm_native_sec_ctxt_log->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_emm_native_sec_context_type);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_NATIVE_SECURITY_CONTEXT_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0F4 not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_emm_native_sec_context_type),emm_native_sec_ctxt_log, sizeof(lte_nas_emm_native_sec_context_type));
  log_commit((void*)diag_log_ptr);
}

void send_emm_usim_serv_table_log_info
(
  lte_nas_emm_usim_serv_table_type  *emm_usim_serv_table_log
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size;

  ASSERT(emm_usim_serv_table_log != NULL);

  /*Add the DIAG log version*/
  emm_usim_serv_table_log->log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + sizeof(lte_nas_emm_usim_serv_table_type);
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_USIM_SERVICE_TABLE_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0F5 not logged\n");
    return;
  }
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,sizeof(lte_nas_emm_usim_serv_table_type),emm_usim_serv_table_log, sizeof(lte_nas_emm_usim_serv_table_type));
  log_commit((void*)diag_log_ptr);
}

void log_forbidden_tracking_area_info
(
  lte_nas_tai_lst2_type *forbidden_for_service_list,
  lte_nas_tai_lst2_type *rrc_forbidden_list
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;
  word pkt_size = 0;
  lte_nas_emm_forbidden_tailist_type forbidden_list_log;
  byte index = 0, struct_size = 0;

  ASSERT(forbidden_for_service_list != NULL);
  ASSERT(rrc_forbidden_list != NULL);

  /*
  1 -> Log header
  (rrc_forbidden_list->tai_lst_length * 5 + 1) =  RRC Forbidden TAI length * ((PLMN (3 bytes) + TAC (2 bytes)) 
                                                  + Length octet of RRC forbidden tai list (1 byte)
  (forbidden_for_service_list->tai_lst_length * 5 + 1) = Forbidden TAI length for service * ((PLMN (3 bytes) + TAC (2 bytes)) 
                                                  + Length octet of forbidden tai list for service (1 byte)                                                
  */
  struct_size = ((rrc_forbidden_list->tai_lst_length * 5) + 1) + ((forbidden_for_service_list->tai_lst_length * 5) + 1) + 1;

  memset(&forbidden_list_log,0,sizeof(lte_nas_emm_forbidden_tailist_type));
  /*Add the DIAG log version*/
  forbidden_list_log.log_version = LTE_NAS_DIAG_LOG_VERSION;

  pkt_size = LOG_PTR_SIZE + struct_size;
  diag_log_ptr = log_alloc(LOG_LTE_NAS_EMM_FORBIDDEN_TRACKING_AREA_LIST_LOG_C, pkt_size);
  if(diag_log_ptr == NULL)
  {
    MSG_ERROR_0("Diag returned a NULL log pointer, log packet B0F6 not logged\n");
    return;
  }
  /*===============================================================================
     lte_nas_tai_lst2_type structure is not a PACK structure, so extra bytes 
     are padded,hence memcpy() of this structure will lead to extra bytes...
     so the following method is used to copy all the data into the logging buffer
  ================================================================================*/
  /*******************************************************
         Log FORBIDDEN TRACKING AREA LIST FOR ROAMING
  *******************************************************/
  forbidden_list_log.forbidden_roaming_list.length = rrc_forbidden_list->tai_lst_length;
  for(index = 0; index < rrc_forbidden_list->tai_lst_length; index++)
  {
    memscpy(&forbidden_list_log.forbidden_roaming_list.diff_plmn[index].plmn,
           sizeof(sys_plmn_id_s_type),&rrc_forbidden_list->diff_plmn[index].plmn,
           sizeof(sys_plmn_id_s_type));
    forbidden_list_log.forbidden_roaming_list.diff_plmn[index].tac = rrc_forbidden_list->diff_plmn[index].tac;
                           //((rrc_forbidden_list->diff_plmn[index].tac >> 8) | 
                            //(rrc_forbidden_list->diff_plmn[index].tac << 8));
  }

  /*******************************************************
         Log FORBIDDEN TRACKING AREA LIST FOR RSERVICE
  *******************************************************/
  forbidden_list_log.forbidden_service_list.length = forbidden_for_service_list->tai_lst_length;
  for(index = 0; index < forbidden_for_service_list->tai_lst_length; index++)
  {
    memscpy(&forbidden_list_log.forbidden_service_list.diff_plmn[index].plmn,
           sizeof(sys_plmn_id_s_type),&forbidden_for_service_list->diff_plmn[index].plmn,
            sizeof(sys_plmn_id_s_type));
    forbidden_list_log.forbidden_service_list.diff_plmn[index].tac = forbidden_for_service_list->diff_plmn[index].tac;
                     //((forbidden_for_service_list->diff_plmn[index].tac << 8) |
                      //(forbidden_for_service_list->diff_plmn[index].tac >> 8));
  }
  /*Get a DIAG PTR*/
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  /*Copy Log version number*/
  memscpy(temp_ptr,struct_size,&forbidden_list_log.log_version, sizeof(byte));
  temp_ptr ++;
  /*Copy "rrc forbidden tracking area list" length octet*/
  memscpy(temp_ptr,struct_size-1,&forbidden_list_log.forbidden_roaming_list.length, sizeof(byte));
  temp_ptr ++;
  /*Copy "rrc forbidden tracking area list"*/
  memscpy(temp_ptr,struct_size-2,forbidden_list_log.forbidden_roaming_list.diff_plmn, forbidden_list_log.forbidden_roaming_list.length * 5);
  temp_ptr += forbidden_list_log.forbidden_roaming_list.length * 5;
  /*Copy "forbidden tracking area list for service" length octet*/
  memscpy(temp_ptr,struct_size- 2 - (forbidden_list_log.forbidden_roaming_list.length * 5),&forbidden_list_log.forbidden_service_list.length, sizeof(byte));
  temp_ptr ++;
  /*Copy "forbidden tracking area list for service"*/
  memscpy(temp_ptr,struct_size-3- (forbidden_list_log.forbidden_roaming_list.length * 5),forbidden_list_log.forbidden_service_list.diff_plmn, forbidden_list_log.forbidden_service_list.length * 5);
  temp_ptr += forbidden_list_log.forbidden_service_list.length * 5;

  log_commit((void*)diag_log_ptr);  
}

void send_nas_ota_msg_log_packet
(
  word log_code,
  word nas_emm_msg_size,
  byte *nas_ota_msg_ptr
)
{
  byte   *diag_log_ptr;
  word   pkt_size;
  byte   *temp_ptr;
  byte   log_version;
  byte   std_version;
  byte   std_major_version;
  byte   std_minor_version;

  ASSERT(nas_ota_msg_ptr != NULL);

  pkt_size = LOG_PTR_SIZE + nas_emm_msg_size + sizeof(log_version) +sizeof(std_version)
                          + sizeof(std_major_version) +sizeof(std_minor_version);

  switch(log_code)
  {
  case LOG_LTE_NAS_ESM_SEC_OTA_IN_MSG_LOG_C:    
  case LOG_LTE_NAS_ESM_SEC_OTA_OUT_MSG_LOG_C:
  case LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C:
  case LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C:
  case LOG_LTE_NAS_EMM_SEC_OTA_IN_MSG_LOG_C:
  case LOG_LTE_NAS_EMM_SEC_OTA_OUT_MSG_LOG_C:
  case LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C:
  case LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C:

    /* Allocate log packet */
    diag_log_ptr = log_alloc(log_code, pkt_size);    
    /* Check to see if log packet allocated */
    if (diag_log_ptr == NULL)
    {
      /* Could not allocate buffer */
      MSG_ERROR_1("Diag could not allocate the buffer for LOG : %d \n",log_code);
      return;
    }
    break;
  default:
    MSG_ERROR_0("Unknown DIAG packet received\n");
    return;
  } 
  /*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      Featurization or Log versioning is done in this section
      Based on the spec used, std_major_version values are changed.
      As of now...
      FOR DECEMBER spec, we follow these values...
          std_major_version = 0;
          std_minro_version = 0;
      For MARCH SPEC....we would be using...
          std_major_version = 1;
          std_minor_version = 0;
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
  log_version = LTE_NAS_DIAG_LOG_VERSION;
  std_version = 8;
  std_major_version = 2;
#ifdef FEATURE_LTE_REL9
  std_version = 9;
  std_major_version = 5;
#endif
  std_minor_version = 0;

/*+++++++++++++++++++++  END OF LONG VERSIONING +++++++++++++++++++++++++++*/
  /* Copy the log data into the logging memory space */
  /*lint -e644 */
  temp_ptr = diag_log_ptr + LOG_PTR_SIZE;
  memscpy(temp_ptr,pkt_size-LOG_PTR_SIZE, &log_version, sizeof(byte));
  temp_ptr++;
  memscpy(temp_ptr,pkt_size-LOG_PTR_SIZE-1, &std_version, sizeof(byte));
  temp_ptr++;
  memscpy(temp_ptr,pkt_size-LOG_PTR_SIZE-2, &std_major_version, sizeof(byte));
  temp_ptr++;
  memscpy(temp_ptr,pkt_size-LOG_PTR_SIZE-3, &std_minor_version, sizeof(byte));
  temp_ptr++;
  memscpy(temp_ptr,pkt_size-LOG_PTR_SIZE-4, nas_ota_msg_ptr, nas_emm_msg_size); 
  /*lint +e644 */
  /* Send the packet */
  log_commit((void*)diag_log_ptr); 
}


#if defined (OLD_NAS_LOGGING_DESIGN)

void send_nas_esm_log_packet
(
  word log_code    
)
{

  byte *diag_log_ptr;
  byte *temp_ptr;

  lte_nas_esm_bearer_context_state_T esm_bearer_ctxtstate_log;
  lte_nas_esm_bearer_context_info_T  esm_bearer_ctxt_info_log;
  lte_nas_esm_procedure_state_T      esm_proc_state_log;

  byte esm_val = 1;
  word pkt_size = 0;

  ASSERT(log_code != NULL);

  /*------------------------------------------------
         Logging based on log code received
  -------------------------------------------------*/
  
  switch(log_code)
  {
  case LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C:
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_esm_bearer_context_state_T);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    esm_bearer_ctxtstate_log.log_version = esm_val;
    esm_bearer_ctxtstate_log.bearer_id = esm_val;
    esm_bearer_ctxtstate_log.bearer_state = esm_val;
    esm_bearer_ctxtstate_log.connection_id = esm_val;

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_esm_bearer_context_state_T),&esm_bearer_ctxtstate_log, sizeof(lte_nas_esm_bearer_context_state_T));
    break;
  case LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C:
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_esm_bearer_context_info_T);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    esm_bearer_ctxt_info_log.log_version = esm_val;
    esm_bearer_ctxt_info_log.context_type = esm_val;
    esm_bearer_ctxt_info_log.bearer_id = esm_val;
    esm_bearer_ctxt_info_log.bearer_state = esm_val;
    esm_bearer_ctxt_info_log.connection_id = esm_val;
    esm_bearer_ctxt_info_log.sdf_id = esm_val;
    memset(&esm_bearer_ctxt_info_log.sdf_id,&esm_val,LTE_NAS_OTA_MSG_MAX_SIZE);

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_esm_bearer_context_info_T),&esm_bearer_ctxt_info_log, sizeof(lte_nas_esm_bearer_context_info_T));
    break;
  case LOG_LTE_NAS_ESM_PROCEDURE_STATE_LOG_C:
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_esm_procedure_state_T);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    esm_proc_state_log.log_version = esm_val;
    esm_proc_state_log.instance_id = esm_val;
    esm_proc_state_log.proc_state = esm_val;
    esm_proc_state_log.pti = esm_val;
    esm_proc_state_log.sdf_id = esm_val;
    esm_proc_state_log.pending_msg_id = esm_val;

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr, sizeof(lte_nas_esm_procedure_state_T),&esm_proc_state_log, sizeof(lte_nas_esm_procedure_state_T));
    break;
  }
  log_commit((void*)diag_log_ptr);
}

void send_nas_emm_log_packet
(
  word                log_code,
  emm_ctrl_data_type* emm_log_info
)
{
  byte *diag_log_ptr;
  byte *temp_ptr;

  lte_nas_emm_state_type                emm_state_log;
  lte_nas_emm_usim_card_mode_type       emm_usim_card_mode_log;
  lte_nas_emm_current_sec_context_type  emm_current_sec_context_log;
  lte_nas_emm_ps_domain_int_cipher_keys ps_domain_cipher_log;
  lte_nas_emm_native_sec_context_type   emm_native_sec_context_log;

  byte emm_val = 1;
  word pkt_size = 0;

  ASSERT(log_code != NULL);  
  /*------------------------------------------------
         Logging based on log code received
  -------------------------------------------------*/
  
  /*lint -e564 */

  switch(log_code)
  {
  case LOG_LTE_NAS_EMM_STATE_LOG_C:
    memset(&emm_state_log,0,sizeof(lte_nas_emm_state_type));
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_emm_state_type);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    emm_state_log.log_version = emm_val;
    emm_state_log.emm_state = emm_log_info->emm_state;
    emm_state_log.emm_substate = emm_log_info->emm_substate;
    memset(&emm_state_log.plmn,emm_log_info->plmn_service_state.plmn,PLMN_ID_LEN);

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_emm_state_type),&emm_state_log, sizeof(lte_nas_emm_state_type));
    break;
  case LOG_LTE_NAS_EMM_USIM_CARD_MODE_LOG_C:
    memset(&emm_usim_card_mode_log,0,sizeof(lte_nas_emm_usim_card_mode_type));
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_emm_usim_card_mode_type);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    emm_usim_card_mode_log.log_version = esm_val;
    emm_usim_card_mode_log.usim_card_mode = mm_sim_card_mode;
    emm_usim_card_mode_log.lte_service_supported = TRUE; /*Hariprasad shall revisit this, to update this value, 
                                                         by calling a funtion that gets lte_supported from usim service table*/
    memset(emm_usim_card_mode_log.imsi,emm_log_info->imsi,sizeof(IMSI_LEN));
    memset(emm_usim_card_mode_log.epsloci,emm_log_info->imsi,sizeof(IMSI_LEN));

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_emm_usim_card_mode_type),&esm_bearer_ctxtstate_log, sizeof(lte_nas_emm_usim_card_mode_type));
    break;
  case LOG_LTE_NAS_EMM_CURRENT_SECUIRY_CONTEXT_LOG_C:
    memset(&emm_current_sec_context_log,0,sizeof(lte_nas_emm_current_sec_context_type));
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_emm_current_sec_context_type);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    emm_current_sec_context_log.log_version = 
    memset(&emm_current_sec_context_log.nas_ul_cnt,emm_log_info>context_info.nas_ul_cnt[0],NAS_UL_DL_COUNT); 
    memset(&emm_current_sec_context_log.nas_dl_cnt,emm_log_info>context_info.nas_dl_cnt[0],NAS_UL_DL_COUNT); 
    memset(&emm_current_sec_context_log.nas_integrity_algo_key,emm_log_info>context_info.nas_int_key[0],NAS_INT_CIPHER_KEY_LEN); 
    memset(&emm_current_sec_context_log.nas_cipher_algo_key,emm_log_info>context_info.nas_enc_key[0],NAS_UL_DL_COUNT); 
    emm_current_sec_context_log.nas_int_algo = emm_log_info>context_info.security_alg.integrity_protect_alg;
    emm_current_sec_context_log.ciphering_alg = emm_log_info>context_info.security_alg.ciphering_alg;
    emm_current_sec_context_log.nas_eksi = emm_log_info>context_info.nasKSI;   
    memset(&emm_current_sec_context_log.nas_kasme,emm_log_info>context_info.nas_kasme,KASME_LEN);     

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_emm_current_sec_context_type),&emm_current_sec_context_log, sizeof(lte_nas_emm_current_sec_context_type));
    break;
  case LOG_LTE_NAS_EMM_CKIK_FOR_PS_DOMAIN_LOG_C:
    memset(&ps_domain_cipher_log,0,sizeof(lte_nas_emm_ps_domain_int_cipher_keys));
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_emm_ps_domain_int_cipher_keys);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    ps_domain_cipher_log.log_version = emm_val;
    ps_domain_cipher_log.keyset_id_ksips = emm_val;
    memset(&ps_domain_cipher_log.cipher_keys_ckps,&mm_auth_get_ck(),NAS_INT_CIPHER_KEY_LEN);     
    memset(&ps_domain_cipher_log.int_keys,&mm_auth_get_ik(),NAS_INT_CIPHER_KEY_LEN);     

    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_emm_ps_domain_int_cipher_keys),&ps_domain_cipher_log, sizeof(lte_nas_emm_ps_domain_int_cipher_keys));
    break;
  case LOG_LTE_NAS_EMM_NATIVE_SECURITY_CONTEXT_LOG_C:
    memset(&emm_native_sec_context_log,0,sizeof(lte_nas_emm_native_sec_context_type));
    pkt_size = sizeof(log_hdr_type) + sizeof(lte_nas_emm_native_sec_context_type);
    diag_log_ptr = log_alloc(log_code, pkt_size);
    /*---------------------------------------------------
            Fill in the values for the structure
    ---------------------------------------------------*/
    emm_native_sec_context_log.log_version             = emm_val;
    emm_native_sec_context_log.eps_nas_sec_context_len = emm_val;
    emm_native_sec_context_log.eps_nas_sec_context_tag = emm_val;
    emm_native_sec_context_log.ksi_tag                 = emm_val;
    emm_native_sec_context_log.ksi_len                 = emm_val;
    emm_native_sec_context_log.ksi_value               = emm_val;
    emm_native_sec_context_log.kasme_tag               = emm_val;
    memset(&emm_native_sec_context_log.kasme_value,&emm_val,MAX_KASME_VALUE_LEN);
    emm_native_sec_context_log.ul_nas_count_tag        = emm_val;
    emm_native_sec_context_log.ul_nas_count_len        = emm_val;
    memset(&emm_native_sec_context_log.ul_nas_count_value,&emm_val,NAS_COUNT_LEN); 
    emm_native_sec_context_log.dl_nas_count_tag        = emm_val;
    emm_native_sec_context_log.dl_nas_count_len        = emm_val;
    memset(&emm_native_sec_context_log.dl_nas_count_value,&emm_val,NAS_COUNT_LEN); 
    emm_native_sec_context_log.nas_algo_tag            = emm_val;
    emm_native_sec_context_log.nas_algo_len            = emm_val;
    emm_native_sec_context_log.nas_algo_value          = emm_val;   
    temp_ptr = diag_log_ptr + sizeof(log_hdr_type);
    memscpy(temp_ptr,sizeof(lte_nas_emm_native_sec_context_type),&emm_native_sec_context_log, sizeof(lte_nas_emm_native_sec_context_type));
    break;
  }
  /*-----------------------------------
            Send the packet 
  -----------------------------------*/
  log_commit((void*)diag_log_ptr);
}
#endif

#endif /*FEATURE_LTE*/


