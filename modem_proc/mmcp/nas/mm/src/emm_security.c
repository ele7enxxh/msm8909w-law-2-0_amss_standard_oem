
/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/emm_security.c#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/14   gps     Removed refrence for header file "gsdi_exp.h"
06/16/11   PA      Added changes for Rel 9 Emergency Bearer services  
05/27/11   PA      Added changes for Security Key generation during W2L PS HO
02/01/10   PA      Added changes for Security Key generation during L2W PS HO
06/24/10   Ri      Fixed the input string to NAS token calculation.
06/10/10   zren    Fixed mapping errors of mobile identity from LTE to UMTS
06/07/10   RI      Lint fixes.
06/05/10   RI      KW fix.
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
05/11/10   RI      Mainlined spec changes upto Dec 09.
05/04/10   RI      Modified the decoding of NAS messages to use the DL cnt from 
                   the SQN in the message.
04/22/10   zren    Updated to use global structure for NAS outgoing message
04/09/10   zren    Added combined procedure feature
04/03/10   RI      Modified the idle mobility key computation function.
04/02/10   zren    Added failure handling for RRC connection release causes of
                   CRE_FAILURE and OOS_DURING_CRE 
03/16/10   RI      GCF validation fixes.
02/26/10   RI      Updated the EFS storage of context - NAS keys are not stored.
02/22/10   RI      Modified deleting of mapped context.
02/20/10   RI      Fixed the error in saving the context info after an SMC
02/13/10   RI      Modified the EFS delete of security context file.
02/02/10   RI      Fixed the EFS read/write of security context.
02/01/10   zren    Added TAU trigger of RRC connection failure and no user data
                   pending. 
                   Fixed service requests collision.
01/22/10   RI      Added support for handling multiple EPS contexts.
12/18/09   RI      Modified the EPS NSC storage to use the global flag.
                   MMGSDI changes.
12/10/09   zren    Updated to make routing information inclusion in 
                   rrc conn req spec compliant
11/10/09   RI      Fixes for GCF test cases.
10/20/09   RI      Added security context state update when in connected mode.
                   Added AMF handling during authentication.
10/06/09   RI      Fixed compiler warnings.
                   Fixes for NAS security.
09/21/09   RI      Added support for GUTI reallocation command.
                   LSTI updates.
09/16/09   zren    Added support for PDN_CONNECTIVITY_REQ msg req/rsp feature
08/31/09   RI      Updated with security integration
08/14/09   zren    Modified procedure connection release handling
07/15/09   zren    Modified message list handling functions to use message 
                   pointer as parameter
07/07/09   hnam    Did the Little Endian to big Endian conversion (for Length fields)
07/07/09   hnam    Fixed the code to correctly load the value into DIAG log packet
06/24/09   hnam    Added support for logging packet
06/01/09   RI      Updated the NAS security procedures.
05/29/09   zren    Added support for TAU feature
05/19/09   hnam    Added support for transmission failure scenario - when 
                   connection is released by RRC during any UL procedure failures
04/14/09   RI      Initial version
===========================================================================*/
#include "mmcp_variation.h"
#include<customer.h>
#ifdef  FEATURE_LTE
#include "comdef.h"
#include "emm_security.h"
#include "mmsecurity.h"
#include "timers_v.h"
#include "err.h"
#include "msg.h"
#include "emm_database.h"
#include "emm_sim_handler.h"
#include "emm_rrc_if.h"
#include "emm_utility.h"
#include "lte_nas_emm_message_description.h"
#include "lte_nas.h"
#include "emm_update_lib.h"
#include "libprot.h"
#include "bit.h"
#include "emm_service_request_lib.h"
#include "fs_public.h"
#include "ghdi_exp_int.h"
#include "lte_nas_msg_parse.h"
#include "lte_security.h"
#include "lte_nas_log.h"
#include "naslog_v.h"
#include "msgr_nas.h"
#include "gmm_int_v.h"
#include "gmmllcm.h"
#include "smtask_v.h"
#include "emm_csfb_handler.h"
#include "emm_rrc_handler.h"
#include "stringl.h"
#include "ttl_map.h"
#include "mcfg_fs.h"
#include "emm_esm_handler.h"
#define USIM_FILE_FORMAT_NOT_DEFINED 1

#define EPS_NSC_FILE_SIZE 0x36 /*Value starts from 0xA0 ......*/

#ifdef TEST_FRAMEWORK
 #error code not present
#endif 

/* Global to keep track of the DL SQN for DL msgs */
extern byte  dl_sqn_for_current_msg;

static client_id  auth_transaction_id = (client_id)NULL;

static boolean reset_nas_ul_count = FALSE;

static boolean reset_nas_dl_count = FALSE;

static lte_nas_message_id ongoing_sec_procedure = ESM_LAST_UNDEFINED;

static lte_nas_emm_native_sec_context_type log_native_sec_ctxt;

static boolean eps_aka_run = FALSE;

static boolean dummy_ctxt = FALSE;

static boolean eps_nsc_len_chk_failed = FALSE;

static boolean invalidate_ul_count = FALSE;

static byte emm_ck_key_value[MAX_AUTH_DATA_LENGTH] ;
static byte emm_ik_key_value[MAX_AUTH_DATA_LENGTH] ;

static auth_gprs_cipherkey_type emm_srvcc_ck_key_value;
static auth_gprs_integritykey_type emm_srvcc_ik_key_value;

static byte   emm_srvcc_eKSI;  // Key Set Identifier

static byte  prev_dl_cnt[4];

extern void mm_reset_ul_count_at_generation(void);

static auth_rand_type rand_value_cache;
static auth_autn_type autn_cache;
static byte  nasksi_cache;

static void emm_store_auth_data(byte  nasksi,
                auth_rand_type rand_value,
                auth_autn_type autn_value);

static boolean emm_compare_auth_data(byte nasksi, 
                      auth_rand_type rand_value, 
                           auth_autn_type autn);

void emm_handle_auth_response
(
  /* Functional Authentication Status code */
  auth_status_type status_code,

  /* Authentication Response Error code */
  auth_error_type auth_error,

  /* Pointer to Authentication Response Value (RES) */
  auth_res_type *auth_res,

  /* Pointer to AUTS value */
  auth_auts_type *auth_auts
);

static boolean emm_validate_smc_nas_security_algos
(
  lte_nas_emm_security_mode_command  *sec_cmd_ptr,
  emm_ctrl_data_type                 *emm_ctrl_data_ptr
);

#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
void ghdi_ds_read_imeisv(byte *imeisv,sys_modem_as_id_e_type as_id);
#endif


/*===========================================================================
FUNCTION    EMM_ESTIMATE_DL_COUNT

DESCRIPTION
  This function derives a 32 bit DL count from the given 4 bits of DL NAS count from MME.
  This derivation is explained in Section 4.8 of Multimode Inter-RAT Security FDD 
  (Doc# 80-N1321-1 A)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECT
  None
===========================================================================*/
void emm_estimate_dl_count
(
  lte_rrc_nas_umts_key_ind_s *rrc_nas_umts_key_ind,
  emm_ctrl_data_type         *emm_ctrl_data_ptr,
  byte                       *nas_dl_count,
  size_t                     nas_dl_count_buf_len
);

void emm_send_security_mode_reject(lte_nas_emm_cause_type emm_cause,
                                   emm_ctrl_data_type     *emm_ctrl_data_ptr);

boolean emm_compute_khashasme_idle_mobility( byte *nonce_ue,
                                             byte *nonce_mme,
                                             emm_ctrl_data_type  *emm_ctrl_data_ptr,
                                             emm_security_context_info *context_ptr);

void emm_send_security_mode_complete(boolean imeisv_req, 
                                     emm_ctrl_data_type *emm_ctrl_data_ptr);

boolean emm_compute_kasme(emm_ctrl_data_type *emm_ctrl_data_ptr);

boolean emm_get_efs_security_context(emm_efs_security_context_info *security_context_ptr);

boolean emm_perform_smc_integriy_check( emm_ctrl_data_type     *emm_ctrl_data_ptr, emm_security_context_info *context_info
);

void emm_send_identity_response
(
  lte_nas_emm_ms_id_type              *mobile_id,
  emm_ctrl_data_type*                 emm_ctrl_data_ptr
);

void emm_abort_ongoing_sec_procedure(emm_ctrl_data_type *emm_ctrl_data_ptr );

int decodelength(byte *len, byte *offset);

byte encodelength(byte *buffer, word number);

void emm_update_contexts(emm_ctrl_data_type *emm_ctrl_data_ptr);

/*===========================================================================

FUNCTION EMM_SWAP_ARRAY_OF_BYTES

DESCRIPTION: This functions takes input as 2 byte pointers and their buffer sizes
             Value coming in from one byte pointer will be swapped & put into 
                 another byte pointer(ed) location. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_swap_array_of_bytes
(
  byte *src_ptr,
  byte src_size,
  byte* swap_dest_ptr,
  size_t swap_dest_size
)
{
  byte index = 0;
  for(index = 0; index < src_size; index++)
  {
    *(swap_dest_ptr + index) = *(src_ptr + ((src_size - 1) - index));
  }
}

void send_sec_mode_cmd_log_pkt(lte_nas_emm_current_sec_context_type *emm_current_sec_context_log, emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  emm_security_context_info *context_ptr;

  context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

  ASSERT ( context_ptr != NULL );

  memscpy((void*)&emm_current_sec_context_log->nas_ul_cnt[0],
          sizeof(emm_current_sec_context_log->nas_ul_cnt),
          (void*)&context_ptr->nas_ul_cnt[0], 
          sizeof(context_ptr->nas_ul_cnt));

  memscpy((void*)&emm_current_sec_context_log->nas_dl_cnt[0],
          sizeof(emm_current_sec_context_log->nas_dl_cnt),
          (void*)&context_ptr->nas_dl_cnt[0], 
          sizeof(context_ptr->nas_dl_cnt)); 

  memscpy((void*)&emm_current_sec_context_log->nas_integrity_algo_key[0],
          sizeof(emm_current_sec_context_log->nas_integrity_algo_key),
          (void*)&context_ptr->nas_int_key[0], 
          sizeof(context_ptr->nas_int_key));

  memscpy((void*)&emm_current_sec_context_log->nas_cipher_algo_key[0],
          sizeof(emm_current_sec_context_log->nas_cipher_algo_key),
          (void*)&context_ptr->nas_enc_key[0],
          sizeof(context_ptr->nas_enc_key));

  emm_current_sec_context_log->nas_int_algo = context_ptr->security_alg.integrity_protect_alg;
  emm_current_sec_context_log->ciphering_alg = context_ptr->security_alg.ciphering_alg;
  emm_current_sec_context_log->nas_eksi = (context_ptr->nasKSI.security_context << 3 | context_ptr->nasKSI.keyset_id) ;   
  
  memscpy((void*)&emm_current_sec_context_log->nas_kasme[0],
          sizeof(emm_current_sec_context_log->nas_kasme),
          (void*)&context_ptr->nas_kasme[0],
          sizeof(context_ptr->nas_kasme));

  send_emm_current_ctxt_sec_log_info(emm_current_sec_context_log);
}

/*lint -save -e715 unreferenced variable trans_id*/

void emm_auth_msg_succeeded
(
  dword trans_id
)
{
 
  /* If the authentication procedure has been completed successfully, i.e.,
     the Mobile sends AUTHENTICATION RESPONSE message,
     the current auth_transaction_id must be reset
  */
  
  auth_transaction_id = (client_id)NULL; 
} 

/*lint -restore */

/*===========================================================================

FUNCTION PARSE_USIM_EPSNSC

DESCRIPTION 
        Function to parse the USIM EFS-nsc while loading security context on power up. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean parse_usim_epsnsc( byte *sim_buffer, emm_security_context_info *usim_security_context_ptr)
{
  byte swapped_nas_counts[8];
  byte invalid_kasme[32];
  byte invalid_dl_nas_count[4];
  byte invalid_ul_nas_count[4];

  byte offset = 0;  
#ifdef USIM_FILE_FORMAT_NOT_DEFINED
  byte num_len_fields_in_usim_file = 7;
#endif
 
  int len = 0;

  word overflow_cnt = 0;


  memset(&swapped_nas_counts[0],0,sizeof(swapped_nas_counts));
  memset(invalid_kasme,0xFF,sizeof(invalid_kasme));
  memset(invalid_dl_nas_count,0xFF,sizeof(invalid_dl_nas_count));
  memset(invalid_ul_nas_count,0xFF,sizeof(invalid_ul_nas_count));

  ASSERT( sim_buffer != NULL );

  ASSERT ( usim_security_context_ptr != NULL );

  memset(&log_native_sec_ctxt,0,sizeof(lte_nas_emm_native_sec_context_type));

  /* get the data from the USIM file */

  if( *sim_buffer != 0xA0 ) /* EPS NSC tag */
    return FALSE;

  log_native_sec_ctxt.eps_nas_sec_context_tag = (byte)(*sim_buffer);

  sim_buffer++;

  /* Length of all the subsequent data */
  len = decodelength ( sim_buffer, &offset );  

  if( len != 0x34 )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Length of EPSnsc contents is %d, valid length must be 52 (or 0x34) - invalid security context is stored in the USIM",
                   len);
    return FALSE;
  }

/*hnam shall revisit this later, after clarifying the size of all length fields in USIM file*/
#ifdef USIM_FILE_FORMAT_NOT_DEFINED
log_native_sec_ctxt.eps_nas_sec_context_len = len + num_len_fields_in_usim_file * (sizeof(word) -sizeof(byte));
log_native_sec_ctxt.eps_nas_sec_context_len = ((log_native_sec_ctxt.eps_nas_sec_context_len>>8)|(log_native_sec_ctxt.eps_nas_sec_context_len<<8));
#else
  log_native_sec_ctxt.eps_nas_sec_context_len = (word)len;
#endif

  /* skip the length fields */
  sim_buffer += offset;

  /* KSI  -tag ,length, value*/
  if( *sim_buffer != 0x80 )
    return FALSE;     

  log_native_sec_ctxt.ksi_tag = (byte)(*sim_buffer);

  sim_buffer++;

  len = decodelength( sim_buffer, &offset );

  if( len != 1 )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= KSI length = %d, must be 1 - invalid contents in the SIM",
                   len);
    return FALSE;
  }

  log_native_sec_ctxt.ksi_len = len;

#ifdef USIM_FILE_FORMAT_NOT_DEFINED
  log_native_sec_ctxt.ksi_len = ((log_native_sec_ctxt.ksi_len >> 8)|(log_native_sec_ctxt.ksi_len << 8));
#endif
  /* skip the length fields */
  sim_buffer += offset;

  usim_security_context_ptr->nasKSI.keyset_id  = ( *sim_buffer++ & 0x07);
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Parsing USIM, KSI = %d ",
                usim_security_context_ptr->nasKSI.keyset_id);
  if (usim_security_context_ptr->nasKSI.keyset_id != NO_KEY_AVAIALABLE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting context_type to FULL_NATIVE ");
    /* Set the contxt type to FULL_NATIVE */
    usim_security_context_ptr->context_type = CONTEXT_TYPE_FULL_NATIVE_EPS;
  }

  usim_security_context_ptr->nasKSI.security_context  = 0;
  log_native_sec_ctxt.ksi_value = usim_security_context_ptr->nasKSI.keyset_id;

  /* KASME */
  if( *sim_buffer != 0x81 )
    return FALSE;
    
  log_native_sec_ctxt.kasme_tag = *sim_buffer;

  sim_buffer++;

  len = decodelength( sim_buffer, &offset );


  if( len != 32 )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= KASME length = %d, must be 32 - invalid contents in the SIM",
                   len);
    return FALSE;
  }

  log_native_sec_ctxt.kasme_len = (word)len;

#ifdef USIM_FILE_FORMAT_NOT_DEFINED
  log_native_sec_ctxt.kasme_len = ((log_native_sec_ctxt.kasme_len >> 8)|(log_native_sec_ctxt.kasme_len << 8));
#endif
   
  /* skip the length fields */
  sim_buffer += offset;
  /* KASME value */
  if(len <= MAX_KASME_VALUE_LEN)
  {
    if(memcmp(invalid_kasme,sim_buffer,sizeof(invalid_kasme)) == 0)
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= invalid KASME is present in the USIM, discarding the stored security context");
      return FALSE;
    }
    memscpy((void*)&usim_security_context_ptr->nas_kasme[0],
            sizeof(usim_security_context_ptr->nas_kasme),
            (void*)sim_buffer, len);
  }

  memscpy((void*)&log_native_sec_ctxt.kasme_value[0],
          sizeof(log_native_sec_ctxt.kasme_value),
          (void*)sim_buffer, len);

  sim_buffer += len;

  /* NAS UL COUNT  */
  if( *sim_buffer != 0x82 )
    return FALSE;
    
  log_native_sec_ctxt.ul_nas_count_tag = *sim_buffer;
  
  sim_buffer++;

  len = decodelength( sim_buffer, &offset );

  if( len != NAS_COUNT_LEN )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= length of UL NAS count = %d - invalid contents in the SIM",
                   len);
    return FALSE;
  }

  log_native_sec_ctxt.ul_nas_count_len = (word)len;

#ifdef USIM_FILE_FORMAT_NOT_DEFINED
  log_native_sec_ctxt.ul_nas_count_len = ((log_native_sec_ctxt.ul_nas_count_len >> 8)|(log_native_sec_ctxt.ul_nas_count_len << 8));
#endif

  /* skip the length fields */
  sim_buffer += offset;

  /* NAS UL CNT value */ 
  if(len <= NAS_COUNT_LEN)
  {
    if(memcmp(invalid_ul_nas_count,sim_buffer,len) == 0)
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= invalid UL_NAS count is present in the USIM, discarding the stored security context");
      return FALSE;
    }
    memscpy((void*)&usim_security_context_ptr->nas_ul_cnt[0],
            sizeof(usim_security_context_ptr->nas_ul_cnt),
            (void*)sim_buffer, len);

   overflow_cnt  = ( (word)usim_security_context_ptr->nas_ul_cnt[2]  << 8 )| (word)usim_security_context_ptr->nas_ul_cnt[1];
   if( usim_security_context_ptr->nas_ul_cnt[0] == 0 )
   {
     /* check the over_flow_cnt, if it is set, the sqn has wrapped around */
     if( overflow_cnt )
     {
       overflow_cnt--;
       usim_security_context_ptr->nas_ul_cnt[0] = 0xFF;

     }
     memscpy((void*)&usim_security_context_ptr->nas_ul_cnt[1],
             sizeof(usim_security_context_ptr->nas_ul_cnt) - 1,
             (void*)&overflow_cnt, sizeof(word));
     
     /*Swap the NAS counts before it can be used in EMM, because it will be stored in the reverse order in USIM*/
     emm_swap_array_of_bytes(&usim_security_context_ptr->nas_ul_cnt[0],
                             sizeof(usim_security_context_ptr->nas_ul_cnt), 
                             &swapped_nas_counts[0],
                             sizeof(usim_security_context_ptr->nas_ul_cnt));

     memscpy((void*)&usim_security_context_ptr->nas_ul_cnt[0],
             sizeof(usim_security_context_ptr->nas_ul_cnt),
             (void*)&swapped_nas_counts[0], sizeof(usim_security_context_ptr->nas_ul_cnt));

     MSG_HIGH_DS_4(MM_SUB, "=EMM= UL NAS COUNT used by EMM  UL_NAS_COUNT[0] = %d  UL_NAS_COUNT[1] = %d \
                   UL_NAS_COUNT[2] = %d UL_NAS_COUNT[3] = %d",usim_security_context_ptr->nas_ul_cnt[0],
                   usim_security_context_ptr->nas_ul_cnt[1], usim_security_context_ptr->nas_ul_cnt[2],
                   usim_security_context_ptr->nas_ul_cnt[3]);                   

   }

  }

  memscpy((void*)&log_native_sec_ctxt.ul_nas_count_value[0],
          sizeof(log_native_sec_ctxt.ul_nas_count_value),
          (void*)sim_buffer, len);

  sim_buffer += len;
      
  /* NAS DL COUNT */

  if( *sim_buffer != 0x83 )
    return FALSE;
    
  log_native_sec_ctxt.dl_nas_count_tag = *sim_buffer;

  sim_buffer++;

  len = decodelength( sim_buffer, &offset );

  if( len != NAS_COUNT_LEN )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= invalid DL_NAS count %d is present in the USIM, discarding the stored security context",
                   len);
    return FALSE;
  }

  log_native_sec_ctxt.dl_nas_count_len = len;

#ifdef USIM_FILE_FORMAT_NOT_DEFINED
  log_native_sec_ctxt.dl_nas_count_len = ((log_native_sec_ctxt.dl_nas_count_len >> 8)|(log_native_sec_ctxt.dl_nas_count_len << 8));
#endif

  /* skip the length fields */
  sim_buffer += offset;

  /* NAS DL CNT value */
  if(len <= NAS_COUNT_LEN)
  {
    if(memcmp(invalid_dl_nas_count,sim_buffer,len) == 0)
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= invalid DL_NAS count is present in the USIM, discarding the stored security context");
      return FALSE;
    }
    memscpy((void*)&usim_security_context_ptr->nas_dl_cnt[0],
            sizeof(usim_security_context_ptr->nas_dl_cnt),
            (void*)sim_buffer, len);
  }

 /*Reverse the byte order of NAS DL count, it will be stored in reverse order in USIM*/
  emm_swap_array_of_bytes(&usim_security_context_ptr->nas_dl_cnt[0],
                          sizeof(usim_security_context_ptr->nas_dl_cnt),
                          &swapped_nas_counts[4],
                          sizeof(usim_security_context_ptr->nas_dl_cnt));

  memscpy((void*)&usim_security_context_ptr->nas_dl_cnt[0],
          sizeof(usim_security_context_ptr->nas_dl_cnt),
          (void*)&swapped_nas_counts[4], sizeof(usim_security_context_ptr->nas_dl_cnt));

  MSG_HIGH_DS_8(MM_SUB, "=EMM= UL NAS COUNT used by EMM  UL_NAS_COUNT[0] = %d  UL_NAS_COUNT[1] = %d \
	                           UL_NAS_COUNT[2] = %d UL_NAS_COUNT[3] = %d and \
	                           DL NAS COUNT to be written to USIM  DL_NAS_COUNT[0] = %d  DL_NAS_COUNT[1] = %d \
	                           DL_NAS_COUNT[2] = %d DL_NAS_COUNT[3] = %d",usim_security_context_ptr->nas_ul_cnt[0],
							    usim_security_context_ptr->nas_ul_cnt[1], usim_security_context_ptr->nas_ul_cnt[2],
							    usim_security_context_ptr->nas_ul_cnt[3], usim_security_context_ptr->nas_dl_cnt[0],
							    usim_security_context_ptr->nas_dl_cnt[1], usim_security_context_ptr->nas_dl_cnt[2],
							    usim_security_context_ptr->nas_dl_cnt[3]);

  memscpy((void*)&log_native_sec_ctxt.dl_nas_count_value[0],
          sizeof(log_native_sec_ctxt.dl_nas_count_value),
          (void*)&usim_security_context_ptr->nas_dl_cnt[0],
          sizeof(usim_security_context_ptr->nas_dl_cnt));

  sim_buffer += len;

 /* NAS integrity and ciphering algorithms */
  if( *sim_buffer != 0x84 )
    return FALSE;

  log_native_sec_ctxt.nas_algo_tag = *sim_buffer;

  sim_buffer++;

  len = decodelength( sim_buffer, &offset );

  if( len <= 0 )
    return FALSE;

  log_native_sec_ctxt.nas_algo_len = len;

#ifdef USIM_FILE_FORMAT_NOT_DEFINED
  log_native_sec_ctxt.nas_algo_len = ((log_native_sec_ctxt.nas_algo_len >> 8)|(log_native_sec_ctxt.nas_algo_len << 8));
#endif

  sim_buffer += len;  

  usim_security_context_ptr->security_alg.ciphering_alg = (lte_nas_emm_ciphering_alg_type)(b_unpackb(sim_buffer, 1, 3));
    
  usim_security_context_ptr->security_alg.integrity_protect_alg = (lte_nas_emm_integrity_protect_alg_type)(b_unpackb(sim_buffer,5,3));

  log_native_sec_ctxt.nas_algo_value = *sim_buffer;

  return TRUE;
}

/*===========================================================================

FUNCTION EMM_INVALIDATE_GMM_PARAMETERS

DESCRIPTION 
        Function to invalidate the GMM parameters when Authentication Reject is recevied. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_invalidate_gmm_parameters(void)
{

  sm_cmd_type                *sm_cmd;

  /* Invalidate the UMTS/GPRS parameters */
 /* ------------------------------------------------------------------
  ** The MS shall set the GPRS update status to GU3 ROAMING NOT ALLOWED
  ** and shall delete any P-TMSI, P-TMSI signature, RAI and GPRS
  ** ciphering key sequence number.  The new GMM state is
  ** GMM-DEREGISTERED.  The SIM shall be considered as invalid for
  ** GPRS services until switching off or the SIM is removed.
  ** ------------------------------------------------------------------ */

  gmm_update_status = GMM_GU3_PLMN_NOT_ALLOWED;
  gmm_delete_ps_location_information();
  gmm_delete_gprs_cksn();
  mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                      gmm_stored_ptmsi_signature,
                                      gmm_stored_rai,
                                      gmm_update_status );

  gmm_set_state( GMM_DEREGISTERED );
  gmm_set_substate( GMM_LIMITED_SERVICE );
  pmm_mode = PMM_DETACHED;


  /* TMSI, LAI and ciphering key sequence number shall be deleted and the
  ** update status shall be set to U3 ROAMING NOT ALLOWED.
  ** The SIM shall be considered as invalid until switching off or the
  ** SIM is removed.
  */
 
  mm_delete_lu_status( ROAMING_NOT_ALLOWED );

  mm_state_control( MM_STD_AUTHENTICATION_REJECT );

  mm_substate_control( MM_STD_NO_IMSI );

  gmm_update_mm_serving_plmn_info( SYS_SIM_STATE_CS_PS_INVALID,
                                   SYS_SRV_DOMAIN_NO_SRV,
                                   TRUE,
                                   SYS_SRV_STATUS_LIMITED,
                                   TRUE );
  /* send notification to SM */
  sm_cmd = sm_get_cmd_buf();

  if (sm_cmd == NULL)
  {
    MSG_FATAL_DS(MM_SUB, "ERROR: Unable to allocate memory", 0,0,0 );
  }

  sm_cmd->header.message_set = MS_GMM_SM;
  sm_cmd->header.cmd_id      = GMMSM_GPRS_RELEASE_IND;
  sm_cmd->cmd.gmm_gprs_rel_ind.cause = NO_GPRS_CONTEXT;
#ifdef FEATURE_DUAL_SIM
  sm_cmd->cmd.gmm_gprs_rel_ind.as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif

  MSG_HIGH_DS_0(MM_SUB, "=EMM=Sending GMMSM_GPRS_RELEASE_IND");

  sm_put_cmd( sm_cmd );
}

/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_GW_KEY_INDICATION

DESCRIPTION
  This function processes RRC_NAS_UMTS_KEY_IND/RRC_NAS_GSM_KEY_IND suring L2W PS HO.
  EMM computes the DL count, new PS CK/IK and CS SRVCC CK/IK and sends to RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_GW_key_indication
(
  lte_rrc_nas_umts_key_ind_s *rrc_nas_gw_key_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  byte nas_dl_count[4];
  emm_security_context_info *context_ptr = NULL;
  auth_gprs_cipherkey_type    Ck;  
  auth_gprs_integritykey_type Ik; 
  auth_gprs_cipherkey_type    Ck_srvcc;  
  auth_gprs_integritykey_type Ik_srvcc;
  auth_gsm_cipherkey_type     Kc;
  auth_gsm_cipherkey_type     gsm_kc_128bit;

  memset( (void *)&Ck, 0, sizeof(auth_gprs_cipherkey_type));
  memset( (void *)&Ik, 0, sizeof(auth_gprs_integritykey_type));
  memset( (void *)&Ck_srvcc, 0, sizeof(auth_gprs_cipherkey_type));
  memset( (void *)&Ik_srvcc, 0, sizeof(auth_gprs_integritykey_type));
  memset( (void *)&Kc, 0, sizeof(auth_gsm_cipherkey_type));
  memset( (void *)&gsm_kc_128bit, 0, sizeof(auth_gsm_cipherkey_type));

  emm_estimate_dl_count(rrc_nas_gw_key_ind,emm_ctrl_data_ptr,
                                    &nas_dl_count[0], sizeof(nas_dl_count));

  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  ASSERT(context_ptr != NULL);

  if((context_ptr != NULL ) && 
      ((context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS ) || 
       (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS )))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= context_type = %d, Compute CK/IK & SRVCC CK/IK.",
                  context_ptr->context_type);

    if( emm_compute_ck_ik_from_kasme(emm_ctrl_data_ptr,
                                     context_ptr,
                                     &Ik, &Ck, TRUE ) == FALSE )
    {
      MSG_FATAL_DS(MM_SUB,"=EMM= Failed to compute CK/IK",0,0,0);
    }

    if( emm_compute_srvcc_ck_ik_from_kasme(emm_ctrl_data_ptr,
                                           context_ptr,
                                           &Ik_srvcc, &Ck_srvcc ) == FALSE )
    {
      MSG_FATAL_DS(MM_SUB,"=EMM= Failed to compute SRVCC CK/IK",0,0,0);
    }
  }

  if(rrc_nas_gw_key_ind->msg_hdr.id == LTE_RRC_NAS_UMTS_KEY_IND)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_NAS_UMTS_KEY_IND");
  emm_send_nas_umts_key_rsp(Ck, Ik, Ck_srvcc, Ik_srvcc );
  }
  else if(rrc_nas_gw_key_ind->msg_hdr.id == LTE_RRC_NAS_GSM_KEY_IND)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Received LTE_RRC_NAS_GSM_KEY_IND");
    auth_generate_kc(&Ck_srvcc, &Ik_srvcc, &Kc,&gsm_kc_128bit);
    emm_send_nas_gsm_key_rsp(Ck, Ik, Ck_srvcc, Ik_srvcc, Kc,gsm_kc_128bit,emm_srvcc_eKSI );
  }

} /* end of emm_rrc_process_GW_key_indication() */

/*===========================================================================
FUNCTION    EMM_RRC_PROCESS_LTE_KEY_INDICATION

DESCRIPTION
  This function processes LTE_RRC_NAS_LTE_KEY_IND during W2L PS HO.
  EMM computes the K'ASME and sends to LRRC in LTE_RRC_NAS_LTE_KEY_RSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_rrc_process_lte_key_indication
(
  lte_rrc_nas_lte_key_ind_s *rrc_nas_lte_key_ind,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  byte nonce_mme[4];
  emm_security_context_info *context_info = NULL;
  unsigned char nas_sec_param_to_eutra[6];

  /* the NAS container cannot be empty */
  ASSERT( rrc_nas_lte_key_ind != NULL );
  ASSERT( emm_ctrl_data_ptr != NULL );

  memset((void *)nas_sec_param_to_eutra, 0, sizeof(nas_sec_param_to_eutra));

  memscpy((void*)&nas_sec_param_to_eutra[0],
          sizeof(nas_sec_param_to_eutra),
          (void*)&rrc_nas_lte_key_ind->nas_sec_param_to_eutra[0],
          sizeof(rrc_nas_lte_key_ind->nas_sec_param_to_eutra));

   /* allocate a new mapped EPS context */
   context_info = emm_alloc_context(emm_ctrl_data_ptr, CONTEXT_TYPE_MAPPED_EPS);
   ASSERT(context_info != NULL);

  /* Copy the next 4 octets - NONCE_MME */
   memscpy((void*)&nonce_mme[0],
           sizeof(nonce_mme),
           (void*)&nas_sec_param_to_eutra[0], sizeof(nonce_mme));

  MSG_HIGH_DS_4(MM_SUB, "=EMM= NONCE MME received in LTE_RRC_NAS_LTE_KEY_IND:\
                nonce_mme[0] = 0x%02x, nonce_mme[1] = 0x%02x, nonce_mme[2] = 0x%02x, nonce_mme[3] = 0x%02x",
                nonce_mme[0], nonce_mme[1], nonce_mme[2], nonce_mme[3]);
   
   context_info->security_alg.ciphering_alg = ((nas_sec_param_to_eutra[4] & 0x70) >> 4);
   context_info->security_alg.integrity_protect_alg = (nas_sec_param_to_eutra[4] & 0x07);

   context_info->nasKSI.keyset_id  = ( nas_sec_param_to_eutra[5] & 0x07);
   context_info->nasKSI.security_context = 1; /* TSC bit should be set to 1, indicating Mapped context */

   MSG_HIGH_DS_3(MM_SUB, "=EMM= Received KSI=%d, INT ALG=%d, ENC ALG=%d in LTE_RRC_NAS_LTE_KEY_IND",
                 context_info->nasKSI.keyset_id,
                 context_info->security_alg.integrity_protect_alg,
                 context_info->security_alg.ciphering_alg);
   
   if ( !emm_compute_khashasme_psho_mobility(
           &nonce_mme[0], sizeof(nonce_mme), emm_ctrl_data_ptr, context_info))
   {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= K'ASME key derivation error during PS HO ");
      return;
   }

   /* RRC derives Kenb from KhashASME and ((2 raised to 32) - 1) as the value of UL NAS count section 9.2.2.2 of 33.401 */
   memset((void *)context_info->nas_ul_cnt,0xFF, MAX_NAS_COUNT_IN_BYTES);
   emm_send_nas_lte_key_rsp(context_info->nas_kasme, context_info->nas_ul_cnt);

   /* Set the DL and UL NAS counts to 0 */
   memset( (void *)context_info->nas_ul_cnt, 0, sizeof(context_info->nas_ul_cnt));
   memset( (void *)context_info->nas_dl_cnt, 0, sizeof(context_info->nas_dl_cnt));
} /* end of emm_rrc_process_lte_key_indication() */


/*===========================================================================
FUNCTION    EMM_MAKE_MAPPED_CTXT_CURRENT_AND_COMPUTE_CK_IK

DESCRIPTION
  This function makes the mapped EPS security context that was created when
  LTE_RRC_NAS_LTE_KEY_IND was received as the current EPS security context.
  If a previous current full native EPS security context exists, it is made non
  current. CK/IK are computed now using the K'ASME from the mapped EPS security
  context and stored into current mapped EPS security context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_make_mapped_ctxt_current_and_compute_ck_ik
(
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{
  emm_security_context_info *current_context_ptr = NULL;
  emm_security_context_info *mapped_context_ptr = NULL;
  lte_nas_emm_current_sec_context_type  emm_current_sec_context_log;

  ASSERT( emm_ctrl_data_ptr != NULL );

  /* Retrieve the mapped context that was created when
     LTE_RRC_NAS_LTE_KEY_IND was received */
  mapped_context_ptr = emm_get_context_by_type(emm_ctrl_data_ptr, CONTEXT_TYPE_MAPPED_EPS);
  ASSERT(mapped_context_ptr != NULL);

 /* Check if full native current context exits and make it non current */
 current_context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);
        
 if((current_context_ptr != NULL ) && 
    (current_context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS))
 {
   MSG_HIGH_DS_1(MM_SUB, "=EMM= Making full native context with KSI=%d as non current.",
                 current_context_ptr->nasKSI.keyset_id);
   current_context_ptr->context_state = CONTEXT_STATE_NON_CURRENT;
 }

  /* Make the mapped context current */
  MSG_HIGH_DS_1(MM_SUB, "=EMM= Making mapped context with KSI=%d as current EPS security context.",
                mapped_context_ptr->nasKSI.keyset_id);
  mapped_context_ptr->context_state = CONTEXT_STATE_CURRENT;

  /* Compute INTEGRITY and CIPHERING keys */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Compute NAS_INTEGRITY KEY");
  if ( emm_compute_nas_int_key(emm_ctrl_data_ptr, mapped_context_ptr) == FALSE )
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= INT KEY Computation Failed ", 0,0,0 );
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Compute NAS_CIPHERING KEY");
  if( emm_compute_nas_enc_key(emm_ctrl_data_ptr, mapped_context_ptr) == FALSE )
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= CIPHER KEY Computation Failed ", 0,0,0 );   
  }

   /*Reset the logging structure*/
   memset(&emm_current_sec_context_log,0,sizeof(lte_nas_emm_current_sec_context_type));
   send_sec_mode_cmd_log_pkt(&emm_current_sec_context_log,emm_ctrl_data_ptr);

  emm_ctrl_data_ptr->w2l_psho = TRUE;
  emm_ctrl_data_ptr->reset_nas_context = FALSE;
  emm_ctrl_data_ptr->last_incoming_sqn = 0;

   MSG_HIGH_DS_0(MM_SUB, " EMM: Reset UL counts ");
   reset_nas_ul_count = TRUE;
} /* end of emm_make_mapped_ctxt_current_and_compute_ck_ik() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
void emm_abort_ongoing_procedure(boolean cancel_auth)
{
  emm_failure_type    emm_failure_cause;

  memset((void *)&emm_failure_cause,0,sizeof(emm_failure_type));
   if((auth_transaction_id != NULL && auth_is_procedure_to_be_aborted(auth_transaction_id))
	   || cancel_auth == TRUE)
   {
      MSG_HIGH_DS_0("EMM: Processing SIM busy");
      emm_cancel_authentication();
    
      /*Authentication transaction is complete, clear the transaction id */
      auth_transaction_id = (client_id)NULL ;
    
      /* Clear the authentication cache memory */
      auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID);

      if((emm_ctrl_data_ptr->esr_type_in_progress != NAS_EMM_CSFB_NONE ))
      {
        emm_failure_cause.access_barring_rem_time = 0;
	    emm_failure_cause.cause_type = LTE_NAS_IRAT_SIM_BUSY;
        emm_abort_service_request(emm_ctrl_data_ptr, FALSE, TRUE, LTE_NAS_EMM_FAILURE_SIM_BUSY, emm_failure_cause);
      }
    
      /* Abort any of the EMM signalling procedure */
      emm_rrc_abort_connection(emm_ctrl_data_ptr, TRUE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);

    }

}
#endif

/*===========================================================================

FUNCTION    EMM_PROCESS_AUTH_REQUEST

DESCRIPTION
  This function processes the Authentication request sent by the NW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_auth_request
(
  lte_nas_emm_auth_req  *emm_auth_request,
  emm_ctrl_data_type    *emm_ctrl_data_ptr
)
{

  auth_rand_type Rand;

  auth_autn_type Autn;

  byte           nasKSI;

  emm_security_context_info *context_ptr;

  ASSERT(emm_auth_request != (lte_nas_emm_auth_req *)NULL) ; 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL) ; 

  if( ( emm_ctrl_data_ptr->detach_reason == SWITCH_OFF ) || 
      ( emm_ctrl_data_ptr->plmn_service_state->sim_state == SYS_SIM_STATE_NOT_AVAILABLE ) )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= AUTHENTICATION REQUEST received while switching off UE/ USIM removed");
    MSG_HIGH_DS_0(MM_SUB, "=EMM= AUTHENTICATION REQUEST message ignored");
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    return;
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Received AUTHENTICATION REQUEST message");
  TTLBOOTUP(LTE_NAS_AUTH_REQ_DLM);

  /* save the RAND and KSI(ASME) for subsequent authentications */
  /* this is done in MM AUTH server */

  /* get the parameters from the incoming message for authentication */

  /* NAS Key Set identifier , only the last 3 bits used
     check if TSC is also needed by GSDI
  */
  nasKSI = emm_auth_request->nas_key_set_identifier_asme.keyset_id;

  /* overwrite the KSI in the current context info since an AKA means a new
     KSI
  */
  /* Allocate a context */
  context_ptr = emm_alloc_context(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);

  ASSERT ( context_ptr != NULL );

  /* save the eKSI of this context from the authentication request*/
  context_ptr->nasKSI = emm_auth_request->nas_key_set_identifier_asme;

  /* RAND is always 16 octets long */
  Rand.value_length = SIZE_OF_RAND_AUTH_PARAM ;
  /* RAND value */
  memscpy((void*)&Rand.value_data[0], sizeof(Rand.value_data),
          (void*)&emm_auth_request->nas_auth_rand_param[0],
          sizeof(emm_auth_request->nas_auth_rand_param));

  /* AUTN length */
  Autn.value_length = emm_auth_request->nas_umts_auth_challenge.length ;
  /* AUTN value */
  memscpy((void*)&Autn.value_data[0], sizeof(Autn.value_data),
          (void*)&emm_auth_request->nas_umts_auth_challenge.val[0],
          Autn.value_length);

  /* save the SQN ^ Ak for KASME computation */
  memscpy((void*)&emm_ctrl_data_ptr->SQNxorAk[0], 
          sizeof(emm_ctrl_data_ptr->SQNxorAk),
          (void*)&emm_auth_request->nas_umts_auth_challenge.val[0],
          sizeof(emm_ctrl_data_ptr->SQNxorAk));

  /* save the AMF inforamtion for the separation bit check */
  memscpy((void *)&emm_ctrl_data_ptr->autn_amf[0],
          sizeof(emm_ctrl_data_ptr->autn_amf),
          (void*)&emm_auth_request->nas_umts_auth_challenge.val[6],
          sizeof(emm_ctrl_data_ptr->autn_amf));

  emm_perform_authentication(&nasKSI, &Rand, &Autn);
 
} /* emm_process_auth_request() */

/*===========================================================================

FUNCTION  EMM_SEND_AUTH_RESPONSE

DESCRIPTION
  This function sends auth response to the NW 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void emm_send_auth_response
(
  auth_res_type*         pAuthRes,
  emm_ctrl_data_type*    emm_ctrl_data_ptr
)
{
  lte_nas_emm_auth_resp*              auth_response;
  dword                               sequence_number = 0xFFFFFFFF;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL); 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  /* Init the global before use */
  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  auth_response = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.auth_resp;

  auth_response->lte_nas_hdr.pd     = EPS_MOBILITY_MANAGEMENT_MESSAGES;

  auth_response->lte_nas_hdr.msg_id = EMM_AUTHENTICATION_RESPONSE;

  auth_response->security_hdr       = PLAIN_NAS_MSG;

  if(( pAuthRes->value_length < 4 )|| ( pAuthRes->value_length > 16 ))
  {
    /* RES must be a minimum of 4 octets and can be only upto 16 octets */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= RES length not ok");
    // TBD: drop the message?
    if( pAuthRes->value_length > 16 )
      pAuthRes->value_length = 16;
  }
  auth_response->nas_auth_resp.length = pAuthRes->value_length;

  memscpy((void*)&auth_response->nas_auth_resp.val[0],
          sizeof(auth_response->nas_auth_resp.val),
          (void*)&pAuthRes->value_data[0], pAuthRes->value_length);
  
  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  /* Send the message and process any transmission failures using 
     emm_security_msg_tx_fail() callback
  */
  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM, 
                                emm_auth_msg_succeeded, 
                                emm_security_msg_tx_fail, 
                                emm_ctrl_data_ptr))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Cannot send AUTHENTICATION RESPONSE, RRC Connection lost");
  }
  /* keep track of the ongoing procedure for tx failure handling */
  ongoing_sec_procedure = EMM_AUTHENTICATION_RESPONSE;
  TTLBOOTUP(LTE_NAS_AUTH_RES_ULM); 
} /* end of emm_auth_response*/

/*===========================================================================

FUNCTION  EMM_SEND_AUTH_FAILURE

DESCRIPTION
  This function sends auth failure with that failure cause that was sent by the USIM.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_auth_failure( auth_error_type    auth_error,
                            auth_auts_type     *auth_auts,
                            emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  lte_nas_emm_auth_fail*              auth_failure;
  dword                               sequence_number = 0xFFFFFFFF;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL); 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  auth_failure = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.auth_fail;

  auth_failure->lte_nas_hdr.pd     = EPS_MOBILITY_MANAGEMENT_MESSAGES;

  auth_failure->lte_nas_hdr.msg_id = EMM_AUTHENTICATION_FAILURE;

  auth_failure->security_hdr       = PLAIN_NAS_MSG;

  // translate error to an emm cause 
  if( auth_error == AUTH_SQN_FAILURE ) 
  {
    auth_failure->emm_cause = LTE_NAS_SYNCH_FAILURE;

    /* Note: using the UMTS defines instead of LTE defines */
    if( (auth_auts->value_length > MAX_AUTH_DATA_LENGTH) || (auth_auts->value_length == 0 ))
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Error in AUTS data length %d", auth_auts->value_length);
      // TBD drop the message?
      if (auth_auts->value_length > MAX_AUTH_DATA_LENGTH)
      auth_auts->value_length = MAX_AUTH_DATA_LENGTH;
    }
    auth_failure->auth_fail_param_exists = TRUE;

    memscpy((void*)&auth_failure->auth_fail_param[0],
            sizeof(auth_failure->auth_fail_param),
            (void*)&auth_auts->value_data[0], auth_auts->value_length);
  }

  else if ( auth_error == AUTH_MAK_CODE_FAILURE)
    auth_failure->emm_cause = LTE_NAS_MAC_FAILURE;

  else if ( auth_error  == AUTH_NON_EPS_ACCESS )
    auth_failure->emm_cause = LTE_NON_EPS_AUTH_UNACCEPTABLE;

  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  /* Send the message and process any transmission failures using 
     emm_security_msg_tx_fail() callback
  */
  /* callback function for success should be NULL while sending auth failure */
  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM, 
                                NULL,
                                emm_security_msg_tx_fail,
                                emm_ctrl_data_ptr))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Cannot send AUTHENTICATION FAILURE, RRC Connection Lost");
  }
  /* keep track of the ongoing procedure for tx failure handling */
  ongoing_sec_procedure = EMM_AUTHENTICATION_FAILURE;
}


/*===========================================================================

FUNCTION EMM_GET_TIMER_LIST

DESCRIPTION
  This function access the NAS EMM timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_get_timer_list
(
  byte *timer_list_length,
  auth_timer_list_type *timer_list
)
{
  *timer_list_length = 4 ;

  timer_list[0].timer = TIMER_T3410 ;

  timer_list[1].timer = TIMER_T3417 ;

  timer_list[2].timer = TIMER_T3421 ;

  timer_list[3].timer = TIMER_T3430 ;
} /*  emm_get_timer_list() */


/*===========================================================================

FUNCTION EMM_PERFORM_AUTHENTICATION

DESCRIPTION
  This function is called upon receipt of Authentication Request,
  it sends an Auth Challange to the USIM.
  Depending on what the USIM sends, pass of failure, UE responds with Auth Response or Auth Reject.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_perform_authentication(
                byte  *pnasksi,
                auth_rand_type* prand,
                auth_autn_type* pautn
)
{
 
  auth_request_properties_type  request_properties;

  /* First check whether the received RAND is same as the one stored, if so
     send an authentication response immediately
  */

  if(mm_sim_card_mode == MMGSDI_APP_UNKNOWN)  
  {
     MSG_ERROR_DS_0(MM_SUB, "SIM either not initialized or invalid");
     return;
  }

  /* If the MAK/SQN Timers are already running, stop them */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Perform Authentication, stop MAK/SQN timers if running ");
  if (auth_transaction_id != ((client_id)NULL))
  {
    auth_stop_auth_mak_sqn_timers(auth_transaction_id) ;
  }

  if ((mm_sim_card_mode == MMGSDI_APP_USIM) && 
                     (mm_timer_status[TIMER_T3416] == TIMER_ACTIVE))
  {
    if (auth_gmm_perform_cache_authentication( prand->value_length,
                                              (byte *)prand->value_data, 
                                              emm_handle_auth_response))
    {
      return ; /* Authentication completed */
    }
  }
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  if(mm_is_sim_busy)
  {
     emm_abort_ongoing_procedure(TRUE);
     return;
  }  
#endif  
  /* Check if SIM is alredy processing a request*/
  if (mm_ps_auth_sent_to_card) 
  {
        /* Compare the new request to the old one */
        if (emm_compare_auth_data(*pnasksi, *prand, *pautn)) 
        {
            MSG_HIGH_DS_0(MM_SUB, "=MM= Back to back same Auth Req, ignore") ;
            return;
        }          
    }

  emm_store_auth_data(*pnasksi, *prand, *pautn);
  request_properties.ciphering_key_sequence_number = *pnasksi;

  request_properties.ciphering_algorithm = FIELD_NOT_IN_USE;

  request_properties.cn_domain_identity = RRC_PS_DOMAIN_CN_ID;

  request_properties.client_response_callback =
                                    emm_handle_auth_response ;

  request_properties.client_timer_set.mak_failure_timer = TIMER_T3418;

  request_properties.client_timer_set.sqn_failure_timer = TIMER_T3420;

  request_properties.client_timer_set.num_failures = 0;

  request_properties.cache_guard_timer_id = TIMER_T3416;

  /* Initialize  re-tx timer list */
  emm_get_timer_list(
                &request_properties.client_timer_set.timer_list_length,
                request_properties.client_timer_set.timer_list);
     
  request_properties.current_auth_request_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;

  auth_transaction_id = auth_authentication_request( *pnasksi,
                                                      prand,
                                                      pautn,
                                                      &request_properties);
} /*  emm_perform_authentication() */

/*===========================================================================

FUNCTION    EMM_CANCEL_AUTHENTICATION

DESCRIPTION
  This function is called to cancel any ongoing authentication. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_cancel_authentication( void )
{
  if (auth_transaction_id != ((client_id)NULL))
  {
    auth_reset_transaction(auth_transaction_id);
    auth_transaction_id = ((client_id)NULL);
  }
  mm_ps_auth_sent_to_card = FALSE;
} /* emm_cancel_authentication() */

/*===========================================================================

FUNCTION    EMM_HANDLE_AUTH_RESPONSE

DESCRIPTION
  This function processes the authentication response from USIM and sends an
  AUTHETICATION FAILURE or an AUTHENTICATION RESPONSE to MME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_handle_auth_response
(
  /* Functional Authentication Status code */
  auth_status_type status_code,

  /* Authentication Response Error code */
  auth_error_type auth_error,

  /* Pointer to Authentication Response Value (RES) */
  auth_res_type *auth_res,

  /* Pointer to AUTS value */
  auth_auts_type *auth_auts
)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
  emm_security_context_info *context_ptr = NULL;
  boolean result = FALSE;
  
  if (status_code == AUTHENTICATION_NETWORK_FAILURE)
  {
    auth_transaction_id = (client_id)NULL;
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Authentication failed for Network in MS, not sending AUTH FAILURE");
    return;
  }

  switch (auth_error)
  {
    case AUTH_NO_ERROR:
      /* The Authentication challenge completed successfully, build
       AUTHENTICATION RESPONSE message.
      */

      MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending AUTHENTICATION RESPONSE message");

      /* temporary method to get the global ctrl data of EMM */
      emm_ctrl_data_ptr = emm_db_get_ctrl_data();

      if( NULL != emm_ctrl_data_ptr ) 
      {
         /* get the context that is partial native EPS context type */
         context_ptr = emm_get_context_by_type(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);
         if(context_ptr != NULL)
         {
           emm_send_auth_response(auth_res, emm_ctrl_data_ptr);

           /* compute KASME , pending context state*/
           result = emm_compute_kasme(emm_ctrl_data_ptr);
        
           if( result == TRUE )
           {
             eps_aka_run = TRUE;
           }
           else
           {
             MSG_FATAL_DS(MM_SUB,"=EMM= Failed to compute KASME",0,0,0);
           }
         }
         else
         {
           /* usually this should not happen..field test was seeing this */
           MSG_ERROR_DS_0(MM_SUB, "=EMM= Could not find partial context to compute kasme, drop response");
         }
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= emm ctrl data NULL");
      }
    break;

    case AUTH_MAK_CODE_FAILURE:
    case AUTH_SQN_FAILURE:
    case AUTH_NON_EPS_ACCESS:
      /* Error condition has been met when the mobile tried to authenticate the
         network, send an AUTHENTICATION FAILURE message.
      */

      MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending AUTHENTICATION FAILURE message");

      /* temporary method to get the global ctrl data of EMM */
      emm_ctrl_data_ptr = emm_db_get_ctrl_data();

      if( NULL != emm_ctrl_data_ptr )
      {
        emm_send_auth_failure(auth_error, auth_auts, emm_ctrl_data_ptr);
      }
      else
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= emm ctrl data NULL");
      }        

      auth_reset_cache_memory(FALSE, NO_TIMER_EXPIRED, RRC_PS_DOMAIN_CN_ID);                     
    break;

    default:
      MSG_FATAL_DS(MM_SUB,"=EMM= Illegal authentication error code returned",0,0,0) ;
    break;
  }
} /* emm_handle_auth_response() */



/*===========================================================================

FUNCTION EMM_PROCESS_AUTH_REJECT

DESCRIPTION
  This function processes Authentication Reject sent by the NW.
  Spec 24.301, section 5.4.2.5

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_authentication_reject
(
  emm_ctrl_data_type  *emm_ctrl_data_ptr
)
{
  byte emm_timer_list_length ;
  auth_timer_list_type emm_retx_timer_list[MAX_TIMER_ARRAY_LENGTH];
  boolean cell_barring_required = FALSE;

  if((mm_timer_status[TIMER_T3418] == TIMER_ACTIVE) ||
     (mm_timer_status[TIMER_T3420] == TIMER_ACTIVE))
  {
    cell_barring_required = TRUE;
  }
  
  /* Retrieve EMM retx timers */
  emm_get_timer_list(&emm_timer_list_length,emm_retx_timer_list);

  /* Call the Authentication server to handle the Authentication Reject message 
     1. Stop any of the retransmission timers: T3410, 3417 or T3430 if running
     2. Stop the MAC and SQN failure timers
     3. Reset the transaction in the cache
  */
     
  auth_handle_network_reject
          (auth_transaction_id,emm_timer_list_length,emm_retx_timer_list);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Processing Authentication Reject, Cancel any on-going Authentication");
  emm_cancel_authentication();

  /*Authentication transaction is complete, clear the transaction id */
  auth_transaction_id = (client_id)NULL ;

  if(mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_UMTS_RADIO_ACCESS)|| 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_GSM_RADIO_ACCESS) || 
     mm_checkif_rat_pri_list_has_rat(&mm_rat_pri_list_info, 
                                     SYS_RAT_TDS_RADIO_ACCESS) )
  {
    emm_invalidate_gmm_parameters();
  }
  /* The following must be done when an authentication reject is received
     1. The update status must be set to EU3 ROAMING NOT ALLOWED
     2. Delete the stored GUTI
     3. delete the TAI list
     4. delete the last visited TAI
     5. Delete the KSI (ASME)
     6. Invalidate USIM (until power down/ USIM removed) - USIM is marked as illegal
     7. Notify RRC of the change of status
         emm_invalidate_mobility_info() takes care of all of the above
  */

  emm_invalidate_mobility_info(EMM_STATUS_ROAMING_NOT_ALLOWED, 
                               SYS_SIM_STATE_CS_PS_INVALID, 
                               emm_ctrl_data_ptr,
                               FALSE);

  /* Clear the authentication cache memory */
  auth_reset_cache_memory(FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID);

  /* Abort any of the EMM signalling procedure and then move to deregistered state*/
  emm_rrc_abort_connection(emm_ctrl_data_ptr, cell_barring_required,
                           LTE_RRC_CONN_ABORT_CAUSE_NORMAL);

  /* enter state EMM-DEREGISTERED */
  EMM_MOVES_TO_DEREGISTERED_STATE(EMM_DEREGISTERED_PLMN_SEARCH);
            /* Notify ESM */
  emm_build_and_send_detach_ind(EMM_DETACHED);  

  /* update the PLMN info? */
  emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED ;
  emm_ctrl_data_ptr->plmn_service_state->reg_service_domain = SYS_SRV_DOMAIN_NO_SRV ; 
  emm_ctrl_data_ptr->srv_domain_registered = SYS_SRV_DOMAIN_NO_SRV; 
  
  /* send a notification to REG */
  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

} /* end emm_process_auth_reject() */

/*===========================================================================

FUNCTION EMM_SECURITY_MSG_TX_FAIL

DESCRIPTION
    This function is a callback function, used to process any transmission failures while sending the message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e715 unreferenced variable trans_id*/

boolean emm_security_msg_tx_fail
(
  lte_emm_connection_cause_type cause,
  dword                         trans_id,
  emm_failure_type               emm_failure_cause,
  lte_nas_tai_lst1_type               tai,
  struct emm_pended_msg_list_tag *message_ptr,
  emm_ctrl_data_type           *emm_ctrl_data_ptr
)
{
  emm_pended_msg_list_type *pended_message_ptr = (struct emm_pended_msg_list_tag *)message_ptr ;

  emm_state_type    emm_state    = EMM_INVALID_STATE;

  ASSERT(pended_message_ptr != (emm_pended_msg_list_type *)NULL); 

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL); 

  emm_state = EMM_GET_STATE();
  switch (cause)
  {
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_HO:   

       /* section 5.4.3.6 of TS 24.301 Abnormal cases in UE (a) */
      if ( emm_state == EMM_TRACKING_AREA_UPDATING_INITIATED)
      {
        /* Start TAU procedure
           Note: Delayed TAU or TAU retry. No need to set TAU cause again in 
           emm data base. */
        emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                   emm_get_eps_update_type(emm_ctrl_data_ptr),
                                   TRUE, 
                                   emm_ctrl_data_ptr);
      } 
      else
      {
        if( emm_state == EMM_SERVICE_REQUEST_INITIATED )
        {
          if (emm_search_tai_list(&tai,emm_ctrl_data_ptr->emm_tai_list_ptr))
          {    
            emm_start_nas_service_request_procedure(emm_ctrl_data_ptr, 
                                         emm_ctrl_data_ptr->service_req_cause);
          }
          else
          {
            /* Current TAI is NOT part of the TAI list */
            /* abort the ongoing procedure */
            emm_abort_ongoing_sec_procedure( emm_ctrl_data_ptr);
            /* Start TAU procedure */
            emm_ctrl_data_ptr->tau_cause = TAU_CAUSE_TA_CHANGE;
            emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                    emm_get_eps_update_type(emm_ctrl_data_ptr),
                                    FALSE, 
                                    emm_ctrl_data_ptr);
          }
        }
      }
      break ;
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_TXN:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_RLF:
  
      switch ( emm_state)
      {
        case EMM_REGISTERED_INITIATED:
          /* re run the attach procedure */
          emm_init_attach_procedure(EMM_INITIAL_UPDATE,emm_ctrl_data_ptr);
          break;
        case EMM_DEREGISTERED_INITIATED:
          /* Initiate the ongoing detach */
          if(emm_ctrl_data_ptr->ongoing_detach_procedure == TRUE)
          {
            emm_start_mo_detach(emm_ctrl_data_ptr->ongoing_detach_type, FALSE, NULL, 
                            emm_detach_failed_routine, emm_ctrl_data_ptr);
          }
          break;
        case EMM_TRACKING_AREA_UPDATING_INITIATED:
          /* Start TAU procedure 
             Note: Delayed TAU or TAU retry. No need to set TAU cause again in 
             emm data base. */
          emm_initiate_tau_procedure(EMM_INITIAL_UPDATE,
                                     emm_get_eps_update_type(emm_ctrl_data_ptr),
                                     TRUE, 
                                     emm_ctrl_data_ptr);
          break;
        default:
          MSG_HIGH_DS_0(MM_SUB, "=EMM= No EMM specific procedure ongoing");
          break;
      }
      break;

    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL:
    case TRANSMISSION_LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN:
    case CONN_REL_LTE_RRC_CONN_REL_NORMAL:
    case CONN_REL_LTE_RRC_CONN_REL_RLF:
    case CONN_REL_LTE_RRC_CONN_REL_CRE_FAILURE:
    case CONN_REL_LTE_RRC_CONN_REL_OOS_DURING_CRE:
    case CONN_REL_LTE_RRC_CONN_REL_ABORTED:
    case CONN_REL_LTE_RRC_CONN_REL_SIB_READ_ERROR:  
    case CONN_REL_LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS:    
      /* Ignore these failures */
      MSG_ERROR_DS_0(MM_SUB, "Connection Lost when the UE was trying to send a sec message");
      break ;
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE:
      /* Ignore this failure */
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Internal Error CONN_CNF_LTE_RRC_CONN_EST_FAILURE, Ignore");
      break;
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ABORTED:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED:
#ifdef FEATURE_NAS_EAB
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB:
#endif
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CELL_RESEL:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_SI_FAILURE:
    case CONN_CNF_LTE_RRC_CONN_EST_FAILURE_CONN_REJECT:
      /* should not get in here */
      ASSERT(0);
      break ;
    case EMM_SRV_REQ_FAILURE_NO_THROTTLE:
    case EMM_SRV_REQ_FAILURE_THROTTLE:
      MSG_HIGH_DS_3(MM_SUB, "=EMM= Failed to send sec msg %d - Trans ID %x Cause %d",
                    message_ptr->nas_out_message.nas_hdr.msg_id,
                    trans_id,
                    cause);
      break;
    default :
      MSG_FATAL_DS(MM_SUB,"=EMM= Illegal EMM connection cause",0,0,0) ;
      break ;
  }

  pended_message_ptr->message_sent_indicator = TRUE ;

  return FALSE ;
} /* end of emm_security_msg_tx_fail() */

/*lint -restore */

#define MAX_SIM_BUFFER  255  // this is a limitation of GSDI

/*===========================================================================

FUNCTION EMM_LOAD_SECURITY_CONTEXT

DESCRIPTION
  This function is called to laod the security context info from the USIM/NV
  when a subsription_ready event is received by mmcoord.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_load_security_context(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  mmgsdi_return_enum_type                 mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_len_type         file_length = 0;
  boolean   status = FALSE;


  emm_security_context_info *context_ptr = NULL;
  byte sim_buffer[MAX_SIM_BUFFER];
  ghdi_status_T               nv_status             = GHDI_SUCCESS ;


  emm_security_context_info usim_security_context_ptr;

  emm_efs_security_context_info saved_security_context;

  ASSERT(emm_ctrl_data_ptr != NULL);
  if(emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr == NULL)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Wrong sub this should not happen, return from function emm_load_security_context");
    return;
  }

  memset( (void *)&saved_security_context, 0xFF, sizeof(saved_security_context));

  memset( (void *)&usim_security_context_ptr, 0xFF, sizeof(usim_security_context_ptr));

  if((nv_status = mm_ghdi_nvmem_read(
        NV_PREF_MODE_I,(ghdi_nvmem_data_T *)(emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr))) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= Unable to access NV to read NV_PREF_MODE_I, SETTING PREFERRED MODE TO LTE");

    emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr->pref_mode.mode = NV_MODE_LTE_ONLY;      
  }

  /*Check for the UMTS ciphering Flag*/
  if((nv_status = mm_ghdi_nvmem_read(
          NV_RRC_CIPHERING_ENABLED_I,(ghdi_nvmem_data_T *)&(emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag))) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "Unable to access NV to read NV_RRC_CIPHERING_ENABLED_I Enable by Default ");
    emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag = TRUE;
  }


  /*Check for the UMTS INTEGRITY Flag*/
  if((nv_status = mm_ghdi_nvmem_read(
            NV_RRC_INTEGRITY_ENABLED_I,(ghdi_nvmem_data_T *)&(emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag))) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "Unable to access NV to read NV_RRC_INTEGRITY_ENABLED_I Enable by Default ");
    emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag = TRUE;
  }

  if((nv_status = mm_ghdi_nvmem_read(
            NV_RRC_FAKE_SECURITY_ENABLED_I,(ghdi_nvmem_data_T *)&(emm_ctrl_data_ptr->emm_nv_fake_sec_flag))) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "Unable to access NV to read NV_RRC_FAKE_SECURITY_ENABLED_I");
    emm_ctrl_data_ptr->emm_nv_fake_sec_flag = FALSE;
  }

  MSG_HIGH_DS_4(MM_SUB, "=EMM= =EMM= NV_PREF_MODE_I = %d NV_RRC_CIPHERING_ENABLED_I = %d  NV_RRC_INTEGRITY_ENABLED_I = %d NV_RRC_FAKE_SECURITY_ENABLED_I = %d",
                emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr->pref_mode.mode,emm_ctrl_data_ptr->emm_umts_nv_cipher_enable_flag,emm_ctrl_data_ptr->emm_umts_nv_integ_enable_flag, emm_ctrl_data_ptr->emm_nv_fake_sec_flag);


  /* check if service is enabled */
  if( emm_ctrl_data_ptr->eps_storage_supported ) 
  {
    mmgsdi_status = mmgsdi_session_read_cache_file_size (mm_sim_mmgsdi_info.session_id,
                                          (mmgsdi_file_enum_type)MMGSDI_USIM_EPSNSC,
                                          &file_length) ;
    if(( mmgsdi_status != MMGSDI_SUCCESS ) || (file_length == 0 ) )
    {
      /* If EPS Storage is supported and there is no file present in USIM,
         set the status as no key available for the NAS initial message
      */
      MSG_ERROR_DS_0(MM_SUB, "=EMM= EPSnsc file is missing; marking this USIM as illegal for CS_PS");
      emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED ;
      emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED,
                                   SYS_SIM_STATE_CS_PS_INVALID,
                                   emm_ctrl_data_ptr,
                                   FALSE);
      return;     
    }   
   
   /* Read the USIM EPS NSC file */
    memset(sim_buffer,0, sizeof(sim_buffer));

    status = mm_sim_read( MMGSDI_USIM_EPSNSC,
                         sim_buffer,
                         (dword )file_length
                        );

    if (status != TRUE )
    {
      /* File present, but read error */
      MSG_ERROR_DS_0(MM_SUB, "=EMM= EPS NSC file read error");
      /* there is no need to read the EFS if the EPS NSC storage is allowed */      
      emm_ctrl_data_ptr->plmn_service_state->service_status = SYS_SRV_STATUS_LIMITED ;
      emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                   SYS_SIM_STATE_CS_PS_INVALID, 
                                   emm_ctrl_data_ptr,
                                   FALSE);
      return;
    }

    memset(( void *)&log_native_sec_ctxt, 0, sizeof( log_native_sec_ctxt));

    /* Parse the file information */
    if(! parse_usim_epsnsc(sim_buffer, &usim_security_context_ptr) )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= EPSnsc parsing failed");
      emm_ctrl_data_ptr->update_sim_params = FALSE;
      emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED,
                                   SYS_SIM_STATE_CS_PS_INVALID,
                                   emm_ctrl_data_ptr,
                                   FALSE);
      emm_ctrl_data_ptr->update_sim_params = TRUE;
      if(eps_nsc_len_chk_failed == FALSE)/*length check passed, EFSnsc file is updated*/      
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= invalidated the security context info");
        /* there is no need to read the EFS if the EPS NSC storage is allowed */
        return;
      }
      else /*length check failed, so EFSnsc file is not updated*/  
      {
        MSG_ERROR_DS_0(MM_SUB, "=EMM= READ/WRITE operations to EPSnsc fails on this USIM, UE shall continue to use default values upon power up");
        eps_nsc_len_chk_failed = FALSE; /*Reset the variable*/
      }
    }
    
    /* log the native security context info */
 
    send_emm_native_sec_ctxt_log_info(&log_native_sec_ctxt);

    /* keep the new copy from USIM in the EFS*/
    /* this will also create a new file if file not present */

     if( usim_security_context_ptr.nasKSI.keyset_id  != NO_KEY_AVAIALABLE )
    {
      MSG_HIGH_DS_3(MM_SUB, "=EMM= USIM stored KSI = %d, Context type = %d, stored context state = %d",
                    usim_security_context_ptr.nasKSI.keyset_id,
                    usim_security_context_ptr.context_type,
                    usim_security_context_ptr.context_state);

      context_ptr = emm_get_context_by_type (emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS );

      if(context_ptr != NULL)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Full Native context does exist in the USIM");
        *context_ptr = usim_security_context_ptr;
      }
     else  
     {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Full Native context does not exist in the USIM, create one");
       context_ptr = emm_alloc_context(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);
       ASSERT ( context_ptr != NULL );
       *context_ptr = usim_security_context_ptr;
       context_ptr->context_type = CONTEXT_TYPE_FULL_NATIVE_EPS;
     }

     if (context_ptr->context_state != CONTEXT_STATE_CURRENT)
     {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting the Full Native context to Current");
        context_ptr->context_state = CONTEXT_STATE_CURRENT;
     }
       
     context_ptr->in_use = TRUE;
     emm_ctrl_data_ptr->reset_nas_context = FALSE;
     /* calculate the  NAS Int and Enc keys since the USIM contains only the
           algorithm info 
     */
     if(emm_compute_nas_int_key(emm_ctrl_data_ptr, context_ptr) == FALSE )
     {
       MSG_FATAL_DS(MM_SUB, "=EMM= INT KEY Computation Failed ", 0,0,0 );
     }
     if( emm_compute_nas_enc_key(emm_ctrl_data_ptr,context_ptr) == FALSE )
     {
       MSG_FATAL_DS(MM_SUB, "=EMM= CIPHER KEY Computation Failed ", 0,0,0 );
     }
     emm_ctrl_data_ptr->last_incoming_sqn = context_ptr->nas_dl_cnt[0];
  }
   else
   {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= usim_security_context_ptr.nasKSI.keyset_id = %d",
                    usim_security_context_ptr.nasKSI.keyset_id);
   }
    
  }
  else /* eps_storage NOT supported */
  {
    /* EF EPS NSC not present in USIM, read the EFS */
    if( emm_get_efs_security_context(&saved_security_context) )
    {
       if ( saved_security_context.nasKSI.keyset_id  != NO_KEY_AVAIALABLE )
       {
         MSG_HIGH_DS_1(MM_SUB, "=EMM= EFS stored KSI = %d ",
                       saved_security_context.nasKSI.keyset_id);
   
         context_ptr = emm_get_context_by_type (emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS );

         if(context_ptr != NULL)
         {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Full Native context does exist in EFS ");
         }
         else  
         {
           MSG_HIGH_DS_0(MM_SUB, "=EMM= Full Native context does not exist in EFS ");
           context_ptr = emm_alloc_context(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);
           ASSERT ( context_ptr != NULL );
           context_ptr->context_type = CONTEXT_TYPE_FULL_NATIVE_EPS;
          }

          /* copy the fields from the EFS security context*/
          memscpy((void*)&context_ptr->nas_kasme[0],
                  sizeof(context_ptr->nas_kasme),
                  (void*)&saved_security_context.nas_kasme[0],
                  sizeof(saved_security_context.nas_kasme));

          memscpy((void*)&context_ptr->nas_ul_cnt[0],
                  sizeof(context_ptr->nas_ul_cnt),
                  (void*)&saved_security_context.nas_ul_cnt[0],
                  sizeof(saved_security_context.nas_ul_cnt));

          memscpy((void*)&context_ptr->nas_dl_cnt[0],
                  sizeof(context_ptr->nas_dl_cnt),
                  (void*)&saved_security_context.nas_dl_cnt[0],
                  sizeof(saved_security_context.nas_dl_cnt));

          context_ptr->nasKSI = saved_security_context.nasKSI;
          context_ptr->security_alg = saved_security_context.security_alg;
              
          if (context_ptr->context_state != CONTEXT_STATE_CURRENT)
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting the Full Native context to Current");
            context_ptr->context_state = CONTEXT_STATE_CURRENT;
          }

          context_ptr->in_use = TRUE;
          emm_ctrl_data_ptr->reset_nas_context = FALSE;

          if( emm_compute_nas_int_key(emm_ctrl_data_ptr, context_ptr) == FALSE)
          {
            MSG_FATAL_DS(MM_SUB, "=EMM= INT KEY Computation Failed ", 0,0,0 );
          }
          if( emm_compute_nas_enc_key(emm_ctrl_data_ptr,context_ptr) == FALSE )
          {
            MSG_FATAL_DS(MM_SUB, "=EMM= CIPHER KEY Computation Failed ", 0,0,0 );
          }
          emm_ctrl_data_ptr->last_incoming_sqn = context_ptr->nas_dl_cnt[0];
       } /* saved_security_context.nasKSI.keyset_id  != NO_KEY_AVAIALABLE */
       else
       {
         MSG_HIGH_DS_1(MM_SUB, "=EMM= saved_security_context.nasKSI.keyset_id = %d",
                       saved_security_context.nasKSI.keyset_id);
       }
    }
  }

} /* emm_load_security_context */

/*===========================================================================

FUNCTION EMM_GET_EFS_SECURITY_CONTEXT

DESCRIPTION
  This function is called while loading the security context from the EFS
  when EF EPS NSC not present in USIM/

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_get_efs_security_context(emm_efs_security_context_info *security_context_ptr)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
  ASSERT( security_context_ptr != NULL );

  status = mcfg_fs_read(EPS_NATIVE_SECURITY_CONTEXT_FILE,
                        security_context_ptr, 
                        sizeof(emm_efs_security_context_info),
                        MCFG_FS_TYPE_EFS,
                        (mcfg_fs_sub_id_e_type)mm_sub_id);

  if (status != MCFG_FS_STATUS_OK )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= EPS Native context file read from EFS failed, status - %d", status);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION EMM_WRITE_EFS_SECURITY_CONTEXT

DESCRIPTION
  This function writes the security context to the EFS. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_write_efs_security_context(emm_security_context_info* security_context_ptr)
{
 mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;
#ifdef FEATURE_LTE_REL9
  emm_ctrl_data_type  *emm_db_ptr;
#endif

  /* do not save the information if a dummy context
       got created without an actual AKA
  */

  emm_efs_security_context_info efs_security_context;

  if( dummy_ctxt )
    return TRUE;

  ASSERT( security_context_ptr != NULL );

#ifdef FEATURE_LTE_REL9
  /* 24.301, Rel 9, Annex C 
     If the UE is attached for emergency bearer services, the UE shall not 
     store the EMM parameters on the USIM or in non-volatile memory */
  emm_db_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_db_ptr != NULL);

  if(emm_db_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= EMC attached. Do not save sec ctxt to EFS.");
    return TRUE;
  }
#endif

  memset( (void *)&efs_security_context, 0xFF, sizeof(emm_efs_security_context_info));
 
  memscpy((void*)&efs_security_context.nas_kasme[0],
          sizeof(efs_security_context.nas_kasme),
          (void*)&security_context_ptr->nas_kasme[0],
          sizeof(security_context_ptr->nas_kasme));

  memscpy((void*)&efs_security_context.nas_ul_cnt[0],
          sizeof(efs_security_context.nas_ul_cnt),
          (void*)&security_context_ptr->nas_ul_cnt[0],
          sizeof(security_context_ptr->nas_ul_cnt));

  memscpy((void*)&efs_security_context.nas_dl_cnt[0],
          sizeof(efs_security_context.nas_dl_cnt),
          (void*)&security_context_ptr->nas_dl_cnt[0],
          sizeof(security_context_ptr->nas_dl_cnt));

  efs_security_context.nasKSI = security_context_ptr->nasKSI;
  efs_security_context.security_alg = security_context_ptr->security_alg;

  status = mcfg_fs_write(EPS_NATIVE_SECURITY_CONTEXT_FILE, 
                         &efs_security_context, 
                         sizeof(emm_efs_security_context_info), 
                         MCFG_FS_O_RDWR|MCFG_FS_O_CREAT|MCFG_FS_O_TRUNC|MCFG_FS_O_AUTODIR,
                         MCFG_FS_ALLPERMS,
                         MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)mm_sub_id);

  if( status != MCFG_FS_STATUS_OK )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Write to EFS Native security context file failed, status - %d", status);
    return FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION EMM_INVALIDATE_NAS_SECURITY_PARAMS

DESCRIPTION
  This function is used to invalidate all the security parameters
  in the Full native security context of the UE. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_invalidate_nas_security_params( emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  emm_security_context_info *context_ptr = NULL;

  context_ptr = emm_get_context_by_type (emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS );

  if ( context_ptr == NULL ) 
  {
    /* It may be that there was only Partial Native context */
    context_ptr = emm_get_context_by_type(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);

    if ( context_ptr == NULL )
    {
       context_ptr = emm_alloc_context( emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);
    }

    MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting Partial context, Cancel any on-going Authentication");
    emm_cancel_authentication();
  }

  ASSERT ( context_ptr != NULL ); 

  context_ptr->nasKSI.keyset_id = NO_KEY_AVAIALABLE;

  memset(context_ptr->nas_kasme, 0xFF, sizeof(context_ptr->nas_kasme));

  memset(context_ptr->nas_ul_cnt,0xFF, sizeof(context_ptr->nas_ul_cnt));

  memset(context_ptr->nas_dl_cnt, 0xFF, sizeof(context_ptr->nas_dl_cnt));

  memset(context_ptr->nas_int_key, 0xFF, sizeof(context_ptr->nas_int_key));

  memset(context_ptr->nas_enc_key, 0xFF, sizeof(context_ptr->nas_enc_key));

  /* update the EF EPS NSC in USIM */

  /*If this flag is not set, then the function called below, will assume that the 
    value 0xFF for UL_COUNT is because of the overflow, just to avoid this scenario 
    set this bit here. It will be reset in the function below.*/
  invalidate_ul_count = TRUE;
  /* Invalidating the mm ul count at generation */
  mm_reset_ul_count_at_generation();

 MSG_HIGH_DS_1(MM_SUB, "=EMM= based on eps_storage_supported supported , update context in USIM(1) / EFS(0) =  %d", 
                               emm_ctrl_data_ptr->eps_storage_supported);

  if (emm_ctrl_data_ptr->eps_storage_supported)
  {
    emm_update_usim_eps_security_context(context_ptr);
  }
  else
  {
    if( emm_write_efs_security_context(context_ptr) == FALSE )
    {
      MSG_FATAL_DS(MM_SUB, "=EMM= Security context efs write Failed ", 0,0,0 );
    }
  }

  if( context_ptr != NULL )
  {
    emm_reset_security_info(context_ptr );
  }

} /* emm_invalidate_nas_security_params */


/*===========================================================================

FUNCTION EMM_UPDATE_USIM_EPS_SECURITY_CONTEXT

DESCRIPTION
   This function is used to write the current security context to the USIM or the EFS. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_update_usim_eps_security_context( emm_security_context_info  *context_info_ptr)
{

  byte sim_buffer[MAX_SIM_BUFFER];

  byte write_buffer[MAX_SIM_BUFFER];

  byte security_alg  = 0;

  byte offset = 0;
  byte swapped_nas_count[8];

  word overflow_cnt = 0;

  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_ERROR;
  mmgsdi_len_type         file_length = 0;
  boolean   status = FALSE;

  word pos = 0;

  emm_ctrl_data_type  *emm_db_ptr;

  memset(swapped_nas_count,0,sizeof(swapped_nas_count));

  emm_db_ptr = emm_db_get_ctrl_data();

  ASSERT( emm_db_ptr != NULL );

  if( dummy_ctxt )
  {
    /* do not save the information if a dummy context
       got created without an actual AKA
    */
    return TRUE; 
  }
  if( !emm_db_ptr->eps_storage_supported )
  {
    return FALSE;

  }

#ifdef FEATURE_LTE_REL9
  /* 24.301, Rel 9, Annex C 
     If the UE is attached for emergency bearer services, the UE shall not 
     store the EMM parameters on the USIM or in non-volatile memory */
  if(emm_db_ptr->emc_srv_status == LTE_NAS_EMC_SRV_ONLY)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= EMC attached. Do not save sec ctxt to USIM/EFS.");
    return TRUE;
  }
#endif

  memset(sim_buffer,0, sizeof(sim_buffer));

  memset(write_buffer,0xFF, sizeof(write_buffer));

 /* KSI  -tag ,length, value =  1 byte*/  
  sim_buffer[pos] = 0x80;  //tag

  pos++;

  offset = encodelength(&sim_buffer[pos], 1);  // length

  pos += offset;

  sim_buffer[pos] = context_info_ptr->nasKSI.keyset_id;  // value

  /* KASME */
         
  pos++;

  sim_buffer[pos] = 0x81;  // tag

  pos++;

  offset = encodelength(&sim_buffer[pos], sizeof(context_info_ptr->nas_kasme));  // length

  pos+= offset;

  ASSERT( ( MAX_SIM_BUFFER - pos ) > (word)sizeof( context_info_ptr->nas_kasme ) );

  memscpy((void*)&sim_buffer[pos], sizeof(sim_buffer) - pos,
          (void*)&context_info_ptr->nas_kasme[0],
          sizeof(context_info_ptr->nas_kasme));     
    
  pos += sizeof(context_info_ptr->nas_kasme);

  ASSERT( pos < (MAX_SIM_BUFFER - 1) );

  /* NAS UL COUNT  */
       
  sim_buffer[pos] = 0x82;  //tag

  pos++;

  ASSERT( pos < ( MAX_SIM_BUFFER - 1 ) );

  offset = encodelength(&sim_buffer[pos], (word)sizeof(context_info_ptr->nas_ul_cnt));  //length

  pos+= offset;

  ASSERT( ( MAX_SIM_BUFFER - pos ) > (word)sizeof( context_info_ptr->nas_ul_cnt) );
    
  /* increment the NAS UL count by one before writing. ie it must be the count corresponding to 
     to the next UL NAS message
  */

   overflow_cnt  = ( (word)context_info_ptr->nas_ul_cnt[2]  << 8 )| (word)context_info_ptr->nas_ul_cnt[1];
   if( context_info_ptr->nas_ul_cnt[0] == 0xFF )
   {
     if(invalidate_ul_count == TRUE)
     {
       invalidate_ul_count = FALSE;
     }
     else
     {
       overflow_cnt++;
       context_info_ptr->nas_ul_cnt[0] = 0;
       memscpy((void*)&context_info_ptr->nas_ul_cnt[1],
               sizeof(context_info_ptr->nas_ul_cnt) - 1,
               (void*)&overflow_cnt, sizeof(word));
     }   
   }

   /*UL NAS count is stored in EMM, with Index[0] = LSB, this must come at the very end (while writing to USIM),  
    so we must swap before writing to USIM, as USIM expects this in the following order*/
    emm_swap_array_of_bytes(&context_info_ptr->nas_ul_cnt[0],
                            sizeof(context_info_ptr->nas_ul_cnt),
                            &swapped_nas_count[0], sizeof(context_info_ptr->nas_ul_cnt));
 
   memscpy((void*)&sim_buffer[pos], sizeof(sim_buffer) - pos,
           (void*)&swapped_nas_count[0], sizeof(context_info_ptr->nas_ul_cnt));

  pos+= sizeof(context_info_ptr->nas_ul_cnt);

  ASSERT( pos < ( MAX_SIM_BUFFER - 1 ) );

  /* NAS DL COUNT */

  sim_buffer[pos] = 0x83;  // tag

  pos++;

  ASSERT( pos < ( MAX_SIM_BUFFER - 1) );
  
  offset = encodelength(&sim_buffer[pos], (word)sizeof(context_info_ptr->nas_dl_cnt));  //length

  pos+= offset;

  emm_swap_array_of_bytes(&context_info_ptr->nas_dl_cnt[0],
                          sizeof(context_info_ptr->nas_dl_cnt),
                          &swapped_nas_count[4],
                          sizeof(context_info_ptr->nas_dl_cnt));

  MSG_HIGH_DS_8(MM_SUB, "=EMM= UL COUNT to be written to USIM UL_NAS_COUNT [0]= %d UL_NAS_COUNT [1]= %d \
	                       UL_NAS_COUNT [2]= %d UL_NAS_COUNT [3]= %d, DL COUNT to be written to USIM DL_NAS_COUNT [0]= %d DL_NAS_COUNT [1]= %d \
	                       DL_NAS_COUNT [2]= %d DL_NAS_COUNT [3]= %d",swapped_nas_count[0],swapped_nas_count[1],
						                                              swapped_nas_count[2],swapped_nas_count[3],
						                                              swapped_nas_count[4],swapped_nas_count[5],
						                                              swapped_nas_count[6],swapped_nas_count[7]);

  memscpy((void*)&sim_buffer[pos], sizeof(sim_buffer) - pos,
          (void*)&swapped_nas_count[4], sizeof(context_info_ptr->nas_dl_cnt));

  pos+= sizeof(context_info_ptr->nas_dl_cnt);
    
  ASSERT( pos < (MAX_SIM_BUFFER - 1) );

  /* NAS integrity and ciphering algorithms */
  sim_buffer[pos] = 0x84;  // tag

  pos++;

  ASSERT( pos < ( MAX_SIM_BUFFER - 1 ) );
 
  offset = encodelength(&sim_buffer[pos], 1);  //length

  pos+= offset;

  ASSERT( pos < ( MAX_SIM_BUFFER - 1) );

  b_packb((byte)context_info_ptr->security_alg.ciphering_alg, &security_alg,1,3);

  b_packb((byte)context_info_ptr->security_alg.integrity_protect_alg, &security_alg,5,3);

  sim_buffer[pos] = security_alg;

  pos++;

  write_buffer[0] = 0xA0;  // EPS NSC tag

  offset = encodelength(&write_buffer[1], pos );

  ASSERT ( pos  < ( MAX_SIM_BUFFER - ( 1 + offset )));

  memscpy((void*)&write_buffer[1+offset],
          sizeof(write_buffer) - 1 - offset,
          (void*)&sim_buffer[0], pos);

  pos = pos + offset + 1;

   mmgsdi_status = mmgsdi_session_read_cache_file_size (mm_sim_mmgsdi_info.session_id,
                                          (mmgsdi_file_enum_type)MMGSDI_USIM_EPSNSC,
                                          &file_length) ;
   if( (mmgsdi_status != MMGSDI_SUCCESS)  || (file_length == 0 ) )
   {
     if(file_length < EPS_NSC_FILE_SIZE)
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= available EPSnsc buffer is less than 0x36 bytes, cannot update the Security ctxt");
      eps_nsc_len_chk_failed = TRUE;   
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "=EMM= Cannot update EF EPS NSC %d", mmgsdi_status);
    }
    return TRUE;
   }

  status = mm_sim_write(MMGSDI_USIM_EPSNSC, write_buffer, (dword)file_length);   // hack
  if( status != TRUE )
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Cannot update EF EPS NSC %d", status);
  }
    //return FALSE;
    /* Note : Do not return FALSE here even if GSDI write fails if
              USIM EPS storage is supported. 
    */

  return TRUE;

} /* emm_update_eps_security_context */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION EMM_PROCESS_SECURITY_MODE_CMD

DESCRIPTION
  This function is called to process the SMC message sent by the NW.
  Spec 24.301, section 5.4.3

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_security_mode_cmd(lte_nas_emm_security_mode_command *emm_security_mode_cmd,
                                   emm_ctrl_data_type                *emm_ctrl_data_ptr)
{

  boolean imeisv_req = FALSE;
  lte_nas_emm_current_sec_context_type  emm_current_sec_context_log;
  boolean native_context = FALSE;
  boolean overwritten_current_context = FALSE; /* Set it to TRUE when the current context is overwritten with the modified algos */

  emm_security_context_info *current_ctxt = NULL, *context_info = NULL;
  emm_security_context_info *tmp_ctxt = NULL;

  byte  temp_prev_dl_cnt[4];
  byte  sqn = 0;
  word  over_flow_cnt = 0;

  lte_nas_emm_security_alg_type       temp_nas_security_alg = {0};

  ASSERT ( emm_ctrl_data_ptr != NULL );
  ASSERT ( emm_security_mode_cmd != NULL );
  ASSERT ( emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr != NULL );

  if( emm_ctrl_data_ptr->detach_reason == SWITCH_OFF )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= SECURITY MODE CMD received while switching off UE");
    MSG_HIGH_DS_0(MM_SUB, "=EMM= SECURITY MODE CMD message ignored");
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    return;
  }

  emm_invalidate_srvcc_ck_ik();

  MSG_MED_DS_0(MM_SUB, "=EMM= resetting the 'emm_current_sec_context' structure");
  /*Reset the logging structure*/
  memset(&emm_current_sec_context_log,0,sizeof(lte_nas_emm_current_sec_context_type));

  // The SMC message is integrtiy protected but not encrypted.
  // after checking the params and computing new keys, integrity check the message

  /* Send SMC reject right away if SMC NAS security algos validation fails */
  if(FALSE == emm_validate_smc_nas_security_algos(emm_security_mode_cmd,
                                                  emm_ctrl_data_ptr))
  {
    MSG_ERROR_DS_0(MM_SUB, "SMC NAS security algos validation failed. Sending SMC reject");
    emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
    return;
  }

  // Check the UE security capabilities
  if((memcmp( (void *)&emm_ctrl_data_ptr->ue_security_capability,
              (void *)&emm_security_mode_cmd->ue_security_capability, 
              sizeof (lte_nas_emm_ue_security_type) ) != 0 ) || 
               ((emm_security_mode_cmd->nonce_ue_exists) && 
               (memcmp((void *)&emm_ctrl_data_ptr->nonce_ue[0],
              (void *)&emm_security_mode_cmd->nonce_ue[0], sizeof(emm_ctrl_data_ptr->nonce_ue) )!= 0)  ) )

  {
#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->emm_3gpp_spec_ver >= LTE_3GPP_REL9)
    {
      byte num_of_lte_bits_in_ue_sec_cap = 18; /*8 LTE INT_ALG +  8 CIPH_ALG + 1 INT_EXISTS + 1 CIPH_EXISTS*/
      if(emm_ctrl_data_ptr->emm_nv_pref_mode_read_ptr->pref_mode.mode == NV_MODE_LTE_ONLY) 
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= REL-9 enabled, LTE_ONLY mode, comparing only LTE algorithms");
        if(memcmp( (void *)&emm_ctrl_data_ptr->ue_security_capability,
                   (void *)&emm_security_mode_cmd->ue_security_capability,
                   num_of_lte_bits_in_ue_sec_cap) != 0 )
        {
          MSG_HIGH_DS_0(MM_SUB, " EMM: UE Security capabilities not ok in SMC");
                     emm_send_security_mode_reject(LTE_NAS_UE_SECURITY_CAPABILITIES_MISMATCH, emm_ctrl_data_ptr);
          return;
        }
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, " EMM: UE Security capabilities not ok in SMC");
                   emm_send_security_mode_reject(LTE_NAS_UE_SECURITY_CAPABILITIES_MISMATCH, emm_ctrl_data_ptr);
        return;
      }
    }
    else
    {
#endif
      MSG_HIGH_DS_0(MM_SUB, " EMM: UE Security capabilities not ok in SMC");
      emm_send_security_mode_reject(LTE_NAS_UE_SECURITY_CAPABILITIES_MISMATCH, emm_ctrl_data_ptr);
      return;
#ifdef FEATURE_LTE_REL9
    }
 #endif
  }
  // check if IMEI SV is requested by the MME
  if(( emm_security_mode_cmd->imiesv_req_exists) && (emm_security_mode_cmd->imiesv_req == IMIESV_REQUESTED))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= IMEISV is requested in SMC");
    imeisv_req = TRUE;
  }
 
  // check whether the native EPS security context or
  // or the mapped EPS security context is used.
  // Possibilities are:
  /* 1. SMC for a partial native EPS context received.
     2. SMC for a new current EPS context received, in which case make any existing current
        EPS context non-current
     3. Mapped EPS context ( IRAT )
  */

#ifdef FEATURE_LTE_REL9
   /* NOTE: The emergency_bearer_services flag should be set to TRUE
     when Attach Request is sent out with Attach_REQ_TYPE = EMERGENCY
  */
  if( ( ( emm_security_mode_cmd->nas_key_set_identifier_asme.keyset_id != NO_KEY_AVAIALABLE) && 
        ( emm_security_mode_cmd->nas_key_set_identifier_sgsn.security_context != 1 ) ) ||
      ( (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && /* Rel 9: SMC for Emergency services with NULL Algos */
        ( emm_security_mode_cmd->nas_security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
        ( emm_security_mode_cmd->nas_security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) ) )
#else
  if( ( ( emm_security_mode_cmd->nas_key_set_identifier_asme.keyset_id != NO_KEY_AVAIALABLE) && 
        ( emm_security_mode_cmd->nas_key_set_identifier_sgsn.security_context != 1 ) ) ||
      ( ( emm_security_mode_cmd->nas_key_set_identifier_asme.keyset_id == NO_KEY_AVAIALABLE ) && 
        ( emm_security_mode_cmd->nas_security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
        ( emm_security_mode_cmd->nas_security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) ) )
#endif
  { 
    MSG_HIGH_DS_1(MM_SUB, "=EMM= received SMC with KSI %d (ASMI)",
                  emm_security_mode_cmd->nas_key_set_identifier_asme.keyset_id);
    /* case 1, 2 of above */
    MSG_HIGH_DS_0(MM_SUB, "=EMM= querying CONTEXT_TYPE_PARTIAL_NATIVE_EPS from the stored context info list");
    /* get the context that is partial native EPS context type */
    context_info = emm_get_context_by_type(emm_ctrl_data_ptr,CONTEXT_TYPE_PARTIAL_NATIVE_EPS);
    if(context_info == NULL)
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= CONTEXT_TYPE_PARTIAL_NATIVE_EPS does not exist, Context info = NULL");
    } 
#ifdef FEATURE_LTE_REL9
    else if((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) &&
            (emm_security_mode_cmd->nas_key_set_identifier_asme.keyset_id == 0) &&
            (emm_security_mode_cmd->nas_security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0) &&
            (emm_security_mode_cmd->nas_security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0))
    {
      /* reset the context_info to NULL so that dummy EPS Security context can be created further.
         deletion of existing partial native context(s) will be taken care of inside emm_alloc_context()
         upon creation of dummy EPS security context */
      MSG_HIGH_DS_1(MM_SUB, "=EMM= emc_srv_status = %d", emm_ctrl_data_ptr->emc_srv_status);
      context_info = NULL;
    }
#endif
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= CONTEXT_TYPE_PARTIAL_NATIVE_EPS Exists, Context info exists");
    }

#ifdef FEATURE_LTE_REL9
    if( ( context_info == NULL ) && /* Rel 9: SMC for Emergency services with NULL Algos */
        ( emm_security_mode_cmd->nas_security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
        ( emm_security_mode_cmd->nas_security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) )
#else
    if( ( context_info == NULL ) &&  /* LSTI */
        ( emm_security_mode_cmd->nas_key_set_identifier_asme.keyset_id == NO_KEY_AVAIALABLE ) && 
        ( emm_security_mode_cmd->nas_security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
        ( emm_security_mode_cmd->nas_security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) )
#endif
    {

#ifdef FEATURE_LTE_REL9
      if (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)
      {
        /* 24.301, 5.4.2.7 Abnormal cases */
        /* if the UE receives the SECURITY MODE COMMAND message before the timeout of timer T3418 or T3420,
             the UE shall deem that the network has passed the authentication check successfully,
             stop timer T3418 or T3420, respectively, and execute the security mode control procedure.
         
             NOTE: SMC can be received only while the timers are active, and hence we don't need to check
                   if the timers are active. The fact that we received SMC, means timers are active,
                   hence cancel the authentication which will stop the timers.
        */
        MSG_HIGH_DS_0(MM_SUB, "=EMM= SMC recevied for Emergency services, cancel any on-going Authentication");
        emm_cancel_authentication();
#endif

      dummy_ctxt = TRUE; /* support for SMC with no AKA */
      eps_aka_run = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=EMM= AKA was not performed. This is a HACK to support SMC w/o AKA, EMM allocating a CONTEXT_TYPE_PARTIAL_NATIVE_EPS");
      context_info = emm_alloc_context(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);

      ASSERT ( context_info != NULL );

        context_info->nasKSI = emm_security_mode_cmd->nas_key_set_identifier_asme;

      MSG_HIGH_DS_0(MM_SUB, "=EMM= copied the context info in SMC into allocated CONTEXT_TYPE_PARTIAL_NATIVE_EPS");
#ifdef FEATURE_LTE_REL9

        /* Set current context if any to non-current
           non-current context will be deleted when emm_update_contexts() is called  */
        current_ctxt = emm_get_context_by_state( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

        if(current_ctxt != NULL ) 
        {
          current_ctxt->context_state = CONTEXT_STATE_NON_CURRENT;
        }

        memset(context_info->nas_kasme, 0xFF, sizeof(context_info->nas_kasme));
        /* set the context state as current */
        context_info->context_state = CONTEXT_STATE_CURRENT;
        context_info->context_type = CONTEXT_TYPE_FULL_NATIVE_EPS;
        MSG_HIGH_DS_0(MM_SUB, "=EMM= copied the context info in SMC into allocated CONTEXT_TYPE_PARTIAL_NATIVE_EPS");
      }
      else
      {
        /* NULL algos came in for non-emergency bearer services, Reject the SMC */
        MSG_HIGH_DS_0(MM_SUB, " EMM: NULL algos came in for non-emergency bearer services, Reject the SMC ");
        emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
        return;
    }    
#endif

    }    

    if ( context_info != NULL )  /* Partial Context exists */
    { 
#ifdef FEATURE_LTE_REL9
      if( ( emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) &&
          ( emm_security_mode_cmd->nas_security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
          ( emm_security_mode_cmd->nas_security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) )
      {
        MSG_HIGH_DS_0(MM_SUB, " EMM: NULL algos came in for emergency bearer services, AKA was successful ");
      }
#endif

      if( memcmp((void *)&context_info->nasKSI,
             (void *)&emm_security_mode_cmd->nas_key_set_identifier_asme,
             sizeof(lte_nas_emm_keyset_identifier_type)  ) == 0 ) 
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= KSI for the received CONTEXT_TYPE_PARTIAL_NATIVE_EPS matched with the one in the stored list");
        if( eps_aka_run )
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= AKA was performed earlier, resetting the NAS counts");
          /* Reset the NAS counts -  AKA followed by an SMC */
          memset( (void *)context_info->nas_ul_cnt , 0, sizeof(context_info->nas_ul_cnt));
          memset((void *)context_info->nas_dl_cnt, 0, sizeof(context_info->nas_dl_cnt));
          reset_nas_dl_count = TRUE;
          reset_nas_ul_count = TRUE;
          if ( dummy_ctxt == FALSE )
            native_context = TRUE;

          current_ctxt = emm_get_context_by_state( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

          if(current_ctxt != NULL ) {
            current_ctxt->context_state = CONTEXT_STATE_NON_CURRENT;
          }

          if(current_ctxt != NULL)
          {
            MSG_HIGH_DS_1(MM_SUB, "=EMM= current_ctxt exists, type %d ",
                          current_ctxt->context_type);
          } 
          else
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= NO current_ctxt exists ");
          }
          /* set the context state as current */
          context_info->context_state = CONTEXT_STATE_CURRENT;
          context_info->context_type = CONTEXT_TYPE_FULL_NATIVE_EPS;
        } else
        {
          /* Do not Crash */
          /* TODO: Remove saving a PARTIAL CONTEXT with Invalid Data, that is what is causing this junk Partial Context to exist */
          //MSG_FATAL_DS(MM_SUB,"=EMM= There is no Partial Native context for which SMC is received",0,0,0);

          /* Send SMC Reject for now */
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending security mode reject, Junk Partial context exists, need a new AKA");
          emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
          return;
        }

      } 
      else  /* KSI is different now */
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Partial Context does not exist for this KSI, Need AKA, sending Security Mode REJECT");
        emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);                
        return; 
      }
    }
    else /* Partial Context does not exsit */
    {
       current_ctxt = emm_get_context_by_state ( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);
        if(current_ctxt != NULL)
        {
          MSG_HIGH_DS_2(MM_SUB, "=EMM= current_ctxt exists, type %d , KSI = %d",
                        current_ctxt->context_type, current_ctxt->nasKSI.keyset_id);
          if(current_ctxt->context_type == CONTEXT_TYPE_MAPPED_EPS)
          {
            overwritten_current_context = FALSE;
          }
          else if(current_ctxt->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS)
          {
            overwritten_current_context = TRUE;
          }

        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= NO current_ctxt exists ");
        }

      /* save this in case smc reject needs to be sent */
      context_info = emm_get_context_by_type ( emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS);

      if(context_info != NULL)
      {
        /* Check if FULL exists, if not send SMC Reject */
        if (memcmp((void *)&context_info->nasKSI,
                     (void *)&emm_security_mode_cmd->nas_key_set_identifier_asme,
                      sizeof(lte_nas_emm_keyset_identifier_type)  ) == 0 )
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= CONTEXT_TYPE_PARTIAL_NATIVE_EPS does not exist, but CONTEXT_TYPE_FULL_NATIVE_EPS with same KSI does, continue...");

          /*  Save the NAS algorithms to revert, incase of SMC fail*/
		  temp_nas_security_alg = context_info->security_alg;

          if (memcmp((void *)&context_info->security_alg,
                           (void *)&emm_security_mode_cmd->nas_security_alg,
                            sizeof(lte_nas_emm_security_alg_type)  ) == 0 )
          {
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Sec Algos same as stored in context_info ");
          }
          else
          {  
            MSG_HIGH_DS_0(MM_SUB, "=EMM= Sec Algos NOT same as stored in context_info , copy now ");
            /*  copy the NAS algorithms */
            context_info->security_alg = emm_security_mode_cmd->nas_security_alg;
          }

           MSG_HIGH_DS_0(MM_SUB, "=EMM= CONTEXT_TYPE_FULL_NATIVE_EPS with same KSI exists, over-write the algos...");
           overwritten_current_context = TRUE;
           if(current_ctxt != NULL)
           {
             MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting non-current Full native to Current and the current Mapped to non-current ");
             current_ctxt->context_state = CONTEXT_STATE_NON_CURRENT;
             context_info->context_state = CONTEXT_STATE_CURRENT;
           }
        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= No Partial/Full context exists for this KSI, send SMC Reject, need AKA ");
          emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
          return;
        }
      } 
      else /* No Current context */
      {
         MSG_HIGH_DS_0(MM_SUB, "=EMM= No Partial/Full context exists for this KSI, send SMC Reject, need AKA ");
         emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
         return;
      }
    }   

    if (memcmp((void *)&context_info->security_alg,
                     (void *)&emm_security_mode_cmd->nas_security_alg,
                      sizeof(lte_nas_emm_security_alg_type)  ) == 0 )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Sec Algos same as stored in context_info ");
    }

    MSG_HIGH_DS_0(MM_SUB, "=EMM= Copying NAS algos ");
    /*  copy the NAS algorithms */
    context_info->security_alg = emm_security_mode_cmd->nas_security_alg;
  } 
  else if( ( emm_security_mode_cmd->nas_key_set_identifier_sgsn.security_context ) &&
             ( emm_security_mode_cmd->nas_key_set_identifier_sgsn.keyset_id != NO_KEY_AVAIALABLE) )
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= received KSI %d (SGSN)",
                  emm_security_mode_cmd->nas_key_set_identifier_sgsn.keyset_id);
    /* mapped EPS security context */
    /* Idle mode mobility from A/Gb to S1 modes or Iu to S1 modes. */
    /* use the K'ASME from KSI SGSN. */
    /* save this in case smc reject needs to be sent */
    current_ctxt = emm_get_context_by_state ( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);
    if(current_ctxt != NULL)
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= current_ctxt exists, type %d, revert to earlier DL count ",
                    current_ctxt->context_type);

      MSG_HIGH_DS_4(MM_SUB, "=EMM= Before Reverting DL Count : emm_get_dl_security_info, current_ctxt->nas_dl_cnt[0] = %d current_ctxt->nas_dl_cnt[1] = %d current_ctxt->nas_dl_cnt[2] = %d current_ctxt->nas_dl_cnt[3] = %d ",
                    current_ctxt->nas_dl_cnt[0],
                    current_ctxt->nas_dl_cnt[1],
                    current_ctxt->nas_dl_cnt[2],
                    current_ctxt->nas_dl_cnt[3]);

      if(current_ctxt->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS)
      {
         sqn = ++prev_dl_cnt[0];
         over_flow_cnt = ( (word )prev_dl_cnt[2] << 8) | prev_dl_cnt[1];

         if ( sqn == 0) 
         {
           MSG_HIGH_DS_0(MM_SUB, "=EMM= over_flow_cnt++");
           over_flow_cnt++ ;
         }

         prev_dl_cnt[0] = sqn;
         prev_dl_cnt[1] = (byte )(over_flow_cnt & 0xFF);
         prev_dl_cnt[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
         memscpy((void*)&current_ctxt->nas_dl_cnt[0],
                       sizeof(current_ctxt->nas_dl_cnt),
                       (void*)&prev_dl_cnt[0], sizeof(prev_dl_cnt));
      }

      MSG_HIGH_DS_4(MM_SUB, "=EMM= After reverting DL count :emm_get_dl_security_info, current_ctxt->nas_dl_cnt[0] = %d current_ctxt->nas_dl_cnt[1] = %d current_ctxt->nas_dl_cnt[2] = %d current_ctxt->nas_dl_cnt[3] = %d ",
                    current_ctxt->nas_dl_cnt[0],
                    current_ctxt->nas_dl_cnt[1],
                    current_ctxt->nas_dl_cnt[2],
                    current_ctxt->nas_dl_cnt[3]);

    } 
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= NO current_ctxt exists ");
    }

    MSG_HIGH_DS_0(MM_SUB, "=EMM= allocating CONTEXT_TYPE_MAPPED_EPS");
    /* allocate a new mapped EPS context */
    context_info = emm_alloc_context(emm_ctrl_data_ptr, CONTEXT_TYPE_MAPPED_EPS);

    ASSERT( context_info != NULL );

    MSG_HIGH_DS_0(MM_SUB, "=EMM= computing KHASHASME for CONTEXT_TYPE_MAPPED_EPS");
    // need to regenerate the K'ASME in the UE from the nonceMME, nonceUE and cached Ck || Ik
    if( !emm_compute_khashasme_idle_mobility( (byte *)&emm_security_mode_cmd->nonce_ue[0],
                                                   (byte *)&emm_security_mode_cmd->nonce_mme[0],
                                                   emm_ctrl_data_ptr,
                                                   context_info) )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= K'ASME key derivation error on SMC");
      emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
      /* delete the allocated context */
      emm_reset_security_info(context_info);
      if(( current_ctxt != NULL ) && ( current_ctxt->in_use) )
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Restoring previous current context (type %d) to current again",
                      current_ctxt->context_type);
        /* restore the previous current context */
        current_ctxt->context_state = CONTEXT_STATE_CURRENT;
      }
      return;
    }
    MSG_HIGH_DS_0(MM_SUB, "=EMM= KHASHASME computation for CONTEXT_TYPE_MAPPED_EPS successful");

    /* K'ASME computation is successful. Now make mapped EPS security context as current */
    context_info->context_state = CONTEXT_STATE_CURRENT;

    /* If previous current EPS security context is of type full native, make it non current */
    if((current_ctxt != NULL ) && 
       (current_ctxt->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS))
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= Making full native context with KSI=%d as non current.",
                    current_ctxt->nasKSI.keyset_id);
      current_ctxt->context_state = CONTEXT_STATE_NON_CURRENT;
    }

    MSG_HIGH_DS_0(MM_SUB, "=EMM= resetting the NAS counts");
    /* Reset the NAS counts -  AKA followed by an SMC */
    memset( (void *)context_info->nas_ul_cnt , 0, sizeof(context_info->nas_ul_cnt));
    memset((void *)context_info->nas_dl_cnt, 0, sizeof(context_info->nas_dl_cnt));
    reset_nas_dl_count = TRUE;
    reset_nas_ul_count = TRUE;

    /* update the eKSI info */
    if( memcmp((void *)&context_info->nasKSI,
             (void *)&emm_security_mode_cmd->nas_key_set_identifier_sgsn,
             sizeof(lte_nas_emm_keyset_identifier_type)  ) == 0 )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Rx KSI same as stored KSI, not overwriting");
    }
    else
    {
      MSG_HIGH_DS_1(MM_SUB, "=EMM= KSI info is updated in the database, Rx KSI %d",
                    context_info->nasKSI.keyset_id);
      context_info->nasKSI = emm_security_mode_cmd->nas_key_set_identifier_sgsn;
    }

    if( memcmp((void *)&context_info->security_alg,
             (void *)&emm_security_mode_cmd->nas_security_alg,
             sizeof(lte_nas_emm_security_alg_type)  ) == 0 )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= Rx Sec Algos same as stored algos, not overwriting");
    }
    else
    {
      context_info->security_alg = emm_security_mode_cmd->nas_security_alg;
      // NAS algorithms have changed
      MSG_HIGH_DS_1(MM_SUB, "=EMM= algorithms for CONTEXT_TYPE %d have changed, over wrote the stored NAS security Algorithms with the ones received in SMC",
                    context_info->context_type);


    }
 
  } else 
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= SMC does not contain any KSI information, sending SMC Reject ");
    emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED, emm_ctrl_data_ptr);
    return;
  }

  /* generate the NAS keys */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= now computing NAS_INTEGRITY KEY");
  if( emm_compute_nas_int_key(emm_ctrl_data_ptr, context_info) == FALSE )
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= INT KEY Computation Failed ", 0,0,0 );
  }
  MSG_HIGH_DS_0(MM_SUB, "=EMM= computing NAS_CIPHERING KEY");
  if( emm_compute_nas_enc_key(emm_ctrl_data_ptr,context_info) == FALSE )
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= CIPHER KEY Computation Failed ", 0,0,0 );
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= performs SMC_INTEGRITY_CHECK");

  /*save a copy of pervious dl count*/
  memscpy((void*)&temp_prev_dl_cnt[0], sizeof(temp_prev_dl_cnt),
          (void*)&prev_dl_cnt[0], sizeof(prev_dl_cnt));

   /* perform the intergrity check on the message now that the integrity key is computed */
  if( !emm_perform_smc_integriy_check( emm_ctrl_data_ptr, context_info ) )
  {
    MSG_ERROR_DS_0(MM_SUB, " EMM: Cannot perform integrity check on SMC");
    /* restore the context info */
    tmp_ctxt = emm_get_context_by_state( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

    ASSERT( tmp_ctxt != NULL );

    if( native_context )
    {
      MSG_HIGH_DS_0(MM_SUB, " EMM: restore the UL/DL NAS COUNT flags");
      reset_nas_dl_count = FALSE;
      reset_nas_ul_count = FALSE;
      native_context = FALSE;
      tmp_ctxt->context_state = CONTEXT_STATE_NONE;
      tmp_ctxt->context_type = CONTEXT_TYPE_PARTIAL_NATIVE_EPS;
    } 
    else
    {
      if (overwritten_current_context == FALSE)
      {
        MSG_HIGH_DS_1(MM_SUB, " EMM: Reset the new security context (type %d ) which was set to current ",
                      tmp_ctxt->context_type);
        emm_reset_security_info(tmp_ctxt);
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB, " EMM: Do not Reset the overwritten security context (type %d ) ",
                      tmp_ctxt->context_type);
		if(current_ctxt != NULL)
		{
           if(current_ctxt->context_type == CONTEXT_TYPE_MAPPED_EPS )
           {
             /*Mark stored security context as non current, in fail case*/
             if(context_info!=NULL)
             {
               context_info->context_state = CONTEXT_STATE_NON_CURRENT;
               context_info->security_alg = temp_nas_security_alg ;
             }
           }
		   else if(current_ctxt->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS )
		   {
		     /* Roll back the algo in case of failure */
			 if(context_info!=NULL)
             {
               context_info->security_alg = temp_nas_security_alg ;
             }
			 
		   }
		}
      }
    }

    if(( current_ctxt != NULL ) && ( current_ctxt->in_use) )
    {
      MSG_HIGH_DS_2(MM_SUB, " EMM: Restoring last known current context, type = %d, eKSI = %d",
                    current_ctxt->context_type, current_ctxt->nasKSI.keyset_id);
      /* set back to the previous one if valid */
      current_ctxt->context_state = CONTEXT_STATE_CURRENT;
    }

    MSG_HIGH_DS_0(MM_SUB, " EMM: sending security mode reject");
    emm_send_security_mode_reject(LTE_NAS_SECURITY_MODE_REJ_UNSPECIFIED,emm_ctrl_data_ptr);

    return;
  }

    /* We should copy the Algos only if everything went fine till here, now copy the algos. */
  /*  copy the NAS algorithms */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Copying NAS algos , contexts info are not stored in USIM/EFS, reset cache memory, Sending SECURITY MODE COMPLETE");
  context_info->security_alg = emm_security_mode_cmd->nas_security_alg;

  /* UE stores the context to the USIM/EFS only when it moves to IDLE or Deregistered */

  emm_ctrl_data_ptr->reset_nas_context = FALSE;

  /* Note: This is temporary, an AKA needs to be run to derive KASME and the
           NAS keys, this is needed to bypass the wrong keys from being sent
           to RRC
  */
  // Reset the RAND and RES with SMC
  auth_reset_cache_memory (FALSE,NO_TIMER_EXPIRED,RRC_PS_DOMAIN_CN_ID) ;
  send_sec_mode_cmd_log_pkt(&emm_current_sec_context_log,emm_ctrl_data_ptr); 
  emm_send_security_mode_complete(imeisv_req, emm_ctrl_data_ptr);

  /* update RRC with the new KASME (K'ASME) and NAS UL count 
     Moved the emm_send_sim_update to emm_send_msg and emm_connection_setup
  */
  /*Reset the AKA flag, since Security Mode Command is accepted and Security Mode Complete us sent out*/
  eps_aka_run = FALSE;
  MSG_HIGH_DS_1(MM_SUB, "=EMM=  Updating Security Context, eps_aka_run = %d", eps_aka_run);

  emm_update_contexts(emm_ctrl_data_ptr);      
}/* end emm_process_security_mode_cmd() */

/*===========================================================================

FUNCTION  EMM_SEND_SECURITY_MODE_COMPLETE

DESCRIPTION
  This function sends security mode complete message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_security_mode_complete(boolean imeisv_req, 
                                     emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  lte_nas_emm_security_mode_complete* smc_complete;
  dword                               sequence_number = 0xFFFFFFFF;

  byte                                imeisv[MAX_IMEI_LEN+2];

  ASSERT( emm_ctrl_data_ptr != NULL );
  ASSERT( emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  MSG_MED_DS_0(MM_SUB, "=EMM= reset the outgoing message structure");

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  smc_complete = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.security_mode_complete;

  smc_complete->lte_nas_hdr.pd        = EPS_MOBILITY_MANAGEMENT_MESSAGES;

  smc_complete->lte_nas_hdr.msg_id    = SECURITY_MODE_COMPLETE;

  // Integrity protected and encripted.
  smc_complete->security_hdr          = PLAIN_NAS_MSG;

  // Set the IMEI SV
  if(imeisv_req)
  {
    MSG_MED_DS_0(MM_SUB, "=EMM= including IMEISV in smc_complete");
   
    memset(imeisv, 0, sizeof(imeisv));

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      ghdi_ds_read_imeisv(imeisv,(sys_modem_as_id_e_type)mm_sub_id);
#else
      read_imeisv(imeisv) ;
#endif

    smc_complete->imeisv_exists = TRUE;

    /* set the length */
    smc_complete->imeisv_id.length = imeisv[0];

    memscpy((void*)&smc_complete->imeisv_id.value[0],
            sizeof(smc_complete->imeisv_id.value), 
            (void*)&imeisv[1], imeisv[0]);
    MSG_MED_DS_0(MM_SUB, "=EMM= copied IMEISV into SMC_COMPLETE");
  }

  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  emm_ctrl_data_ptr->nas_secure_exchange = TRUE;

  MSG_MED_DS_0(MM_SUB, "=EMM= calling 'emm_send_message'to send SMC_COMPLETE");

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                NULL, 
                                emm_security_msg_tx_fail, 
                                emm_ctrl_data_ptr))
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Cannot send SMC COMPLETE message, RRC Connection Lost",0,0,0);
  }
  /* keep track of the ongoing procedure for tx failure handling */
  ongoing_sec_procedure = SECURITY_MODE_COMPLETE;

  MSG_MED_DS_0(MM_SUB, "=EMM= 'ongoing_sec_procedure' = SECURITY_MODE_COMPLETE");

}

/*===========================================================================

FUNCTION EMM_SEND_SECURITY_MODE_REJECT

DESCRIPTION
  This function sends security mode reject message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_security_mode_reject(lte_nas_emm_cause_type emm_cause,
                                   emm_ctrl_data_type  *emm_ctrl_data_ptr)
{
  lte_nas_emm_security_mode_rej*      smc_reject;
  dword                               sequence_number = 0xFFFFFFFF;

  byte max_cnt[4] = { 0xFF, 0xFF, 0xFF, 0};
  emm_security_context_info *context_ptr = NULL;

  ASSERT ( emm_ctrl_data_ptr != NULL );
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  smc_reject = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.security_mode_rej;

  smc_reject->lte_nas_hdr.pd        = EPS_MOBILITY_MANAGEMENT_MESSAGES;

  smc_reject->lte_nas_hdr.msg_id    = SECURITY_MODE_REJECT;

  // the SMC Reject is not integrity protected
  smc_reject->security_hdr          = PLAIN_NAS_MSG;

  smc_reject->emm_cause             = emm_cause;

  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;


  /* check if the UL NAS count wraps around when sending this message.
     If so, abort the RRC connection
      
  */

  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

  if( context_ptr != NULL )
  {    
    if( memcmp(context_ptr->nas_ul_cnt, max_cnt, sizeof(context_ptr->nas_ul_cnt) ) == 0 )
    {
      MSG_HIGH_DS_0(MM_SUB, "=EMM= UL_NAS_COUNT has reached max_count, aborting RRC connection");
      emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
      return;
    }
  }
  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM,
                                NULL, 
                                emm_security_msg_tx_fail,
                                emm_ctrl_data_ptr))
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Cannot send SMC REJECT message, RRC Connection Lost",0,0,0);
  }
  /* keep track of the ongoing procedure for tx failure handling */
  ongoing_sec_procedure = SECURITY_MODE_REJECT;
  MSG_HIGH_DS_0(MM_SUB, "=EMM= 'ongoing_sec_procedure' = SECURITY_MODE_REJECT");
  
}

/*===========================================================================

FUNCTION EMM_PERFORM_SMC_INTEGRIY_CHECK

DESCRIPTION
  This function performs Integrtiy check on the incoming SMC. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define MAX_MAC_HDR_IN_BYTES 4

 boolean emm_perform_smc_integriy_check( emm_ctrl_data_type     *emm_ctrl_data_ptr
,  emm_security_context_info *context_info
)
 {
   lte_nas_sec_info_params_type  *smc_params;

   lte_nas_dl_security_info_type  curr_security_info;
   boolean status = TRUE;

   word pos = 8;
   byte security_hdr;

#ifdef FEATURE_LTE_REL9
   byte nas_invalid_kasme[32];
   memset(nas_invalid_kasme, 0xFF, sizeof(nas_invalid_kasme));

   /* Rel 9: No integrity check for SMC when Emergency Services requested, becasue there is no KASME, and hence no CK/IK */
   if ((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && 
         (memcmp(context_info->nas_kasme,nas_invalid_kasme, sizeof(nas_invalid_kasme)) == 0))
   {
     MSG_HIGH_DS_0(MM_SUB, "=EMM= Rel 9: emergency services: KASME is 0xFF, do not perform Integrity check ");
     return TRUE;
   }
#endif
   
   smc_params = get_nas_sec_info(&security_hdr);

   memset(&curr_security_info, 0, sizeof(lte_nas_dl_security_info_type));

   MSG_MED_DS_0(MM_SUB, "=EMM= copying MACI received in SMC");
   memscpy((void*)&curr_security_info.maci[0],
           sizeof(curr_security_info.maci),
           (void*)&smc_params->maci[0], sizeof(smc_params->maci)); // MAC value in the SMC
  
   curr_security_info.bearer = 0;   // BEARER

   ///* 
   ///* Possible cases eg: 
   //1. Authentication request followed by SMC - 
   //           SQN = 0, CNT = 0
   //2. Algorithm changed in the MME, triggering just an SMC - 
   //           SQN = 4, CNT = 0; 
   //           SQN = 0, CNT = 1  ( SQN wrapped around so the CNT is incremented 
   //3. Mobility/ Handover, triggering  an SMC - 
   //           SQN = 0, CNT = 0; 
   //           SQN = 0, CNT = 1  ( SQN wrapped around so the CNT is incremented
   //*/
   //
   //over_flow_cnt = ( (word )emm_ctrl_data_ptr->context_info.nas_dl_cnt[2] << 8) | emm_ctrl_data_ptr->context_info.nas_dl_cnt[1];

   //if( (emm_ctrl_data_ptr->context_info.nas_dl_cnt[0] == 0 ) && ( over_flow_cnt == 0 ) )
   //{
   //  /* NAS counters got reset with the SMC */
   //  memset( curr_security_info.nas_count, 0, sizeof(curr_security_info.nas_count));
   // 
   //} else {
   //   sqn  = smc_params->sqn % 256;
   //   
   //   if( sqn == 0)
   //   {
   //     /* SQN wrapped around at the sending side, update the local copy also */
   //     over_flow_cnt++ ;
   //   }
   //   curr_security_info.nas_count[0] = sqn;
   //   curr_security_info.nas_count[1] = (byte )(over_flow_cnt) & 0xFF;
   //   curr_security_info.nas_count[2] = (byte )((over_flow_cnt & 0xFF00) >> 8 );
   //
   //   /* save the DL count in emm ctrl data */
   //   memcpy((void *)&emm_ctrl_data_ptr->context_info.nas_dl_cnt[0], (void *)&curr_security_info.nas_count[0], sizeof(curr_security_info.nas_count));
   //}

   //*/
 
   MSG_MED_DS_0(MM_SUB, "=EMM= calling 'emm_get_dl_security_info'");
   (void)emm_get_dl_security_info(emm_ctrl_data_ptr, &curr_security_info, TRUE,NULL);

   emm_ctrl_data_ptr->ota_msg_size *= 8;

   if (!nas_msg_integrity_check ((byte *)emm_ctrl_data_ptr->emm_ota_message_ptr, 
                                 (word *)&pos, (word *)&emm_ctrl_data_ptr->ota_msg_size, 
                                 &curr_security_info, FALSE,TRUE ) )
   {
     status = FALSE;

   }
   MSG_HIGH_DS_1(MM_SUB, "=EMM= SMC INTEGRITY CHECK (passed(1) /Failed(0)) = %d", status);
   return status;

 }

/*===========================================================================

FUNCTION EMM_CANCEL_SECURITY_MODE

DESCRIPTION
  This function cancels the ongoing security mode command procedure
  if security is valid, it Invalidates the current context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_cancel_security_mode(emm_ctrl_data_type *emm_ctrl_data_ptr)
{
  emm_security_context_info *context_ptr;

  context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);
  ASSERT (context_ptr != NULL );

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Cancelling SMC procedure, valid security exists, resetting the CONTEXT_STATE_CURRENT info");
  /* Revisit this to determine whether all the fields can be reset or not */  
    
  if( context_ptr != NULL )
  {

    context_ptr->nasKSI.keyset_id = NO_KEY_AVAIALABLE;
    /* cannot reset the kasme since it comes in authentication request */
    memset((void *)context_ptr->nas_ul_cnt,0xFF, MAX_NAS_COUNT_IN_BYTES);

    memset((void *)context_ptr->nas_dl_cnt, 0xFF, MAX_NAS_COUNT_IN_BYTES);

    memset((void *)context_ptr->nas_int_key, 0xFF, MAX_NAS_KEY_LENGTH_IN_BYTES);

    memset((void *)context_ptr->nas_enc_key, 0xFF, MAX_NAS_KEY_LENGTH_IN_BYTES);
  }
}

/*===========================================================================

FUNCTION EMM_COMPUTE_KASME

DESCRIPTION
  This function computes the KASME during authentication procedure.
  Spec 33.401, section A.2

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 boolean emm_compute_kasme(emm_ctrl_data_type *emm_ctrl_data_ptr)
 {
  /* 
       FC = 0x10,
       P0 = PLMN ID,
       L0 = length of PLMN ID (i.e. 0x00 0x03),
       P1 = SQN ^ AK
       L1 = length of SQN ^ AK (i.e. 0x00 0x06) 
       S = FC || P0 || L0 || P1 || L1
       Key = Ck || Ik
       derived key = HMAC-SHA-256 (Key, S),

  */
  lte_sec_errno_e  nas_errno = E_FAILURE;

  emm_security_context_info *context_ptr;

  auth_gprs_integritykey_type Ik;  // Integrity Key
  auth_gprs_cipherkey_type    Ck;  // Ciphering Key
  sys_plmn_id_s_type          plmn_id;  // Service PLMN ID

  byte  key[32];   // holds the KASME Key

  byte  input_str[14];

  byte input_str_len = 0;

  byte output_str[32];

  memset(input_str, 0, sizeof(input_str));

  memset( output_str,0, sizeof(output_str));

  /* get the cached values of Ck and Ik from the MM AUTH Server */
  Ik = mm_auth_get_ik();
  Ck = mm_auth_get_ck();

  // Ck || Ik
  memscpy((void*)&key[0], sizeof(key),
          (void*)&Ck.value_data[0], Ck.value_length);

  memscpy((void*)&key[UMTS_SECURITY_KEY_LENGTH],
          sizeof(key) - UMTS_SECURITY_KEY_LENGTH,
          (void*)&Ik.value_data[0], Ik.value_length);

  /* set the input string from the parameters collected */

  input_str[12] = 0;  // 2 bytes of L1
  input_str[13]= 0x06;
 
  memscpy((void*)&input_str[6], sizeof(input_str) - 6, 
         (void *)&emm_ctrl_data_ptr->SQNxorAk[0], 
         sizeof(emm_ctrl_data_ptr->SQNxorAk));  // P1; 6 bytes of SQN ^ Ak

  input_str[4] = 0x00;
  input_str[5] = 0x03;  // L0  2 bytes

  plmn_id = emm_ctrl_data_ptr->plmn_service_state->plmn;

  memscpy((void*)&input_str[1], sizeof(input_str) - 1,
          (void*)&plmn_id, sizeof(plmn_id)); // P0; PLMN ID = 3 bytes

  input_str[0] = 0x10;  // FC

  input_str_len = sizeof(input_str);
  
  nas_errno = lte_security_generate_key( key,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,      /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  /* save the KASME */

  /* get the context that is partial native EPS context type */
  context_ptr = emm_get_context_by_type(emm_ctrl_data_ptr, CONTEXT_TYPE_PARTIAL_NATIVE_EPS);
  ASSERT (context_ptr != NULL );

  if( nas_errno != E_SUCCESS)
  {
    /* delete any pending partial native EPS context
       since the KASME could not be computed
    */
    emm_reset_security_info(context_ptr);
    return FALSE;
  }
  memscpy((void*)&context_ptr->nas_kasme[0],
          sizeof(context_ptr->nas_kasme),
          (void*)&output_str[0], sizeof(output_str));

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  return TRUE;
}
/*===========================================================================

FUNCTION EMM_COMPUTE_KHASHASME_IDLE_MOBILITY

DESCRIPTION
  This function computes the K_hash_ASME during Idle mode mobility.
  Spec 33.401, section A.11

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_compute_khashasme_idle_mobility(byte *nonce_ue, 
                                            byte *nonce_mme,
                                            emm_ctrl_data_type *emm_ctrl_data_ptr
                                            , emm_security_context_info *context_ptr
)
 {

   /* 
       FC = 0x19,
       P0 = NONCEUE
       L0 = length of the NONCEUE (i.e. 0x00 0x04)
       P1 = NONCEMME
       L1 = length of the NONCEMME (i.e. 0x00 0x04)
       Key =  CK || IK
 */
  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[13];

  byte input_str_len = 0;

  byte output_str[32];

  byte  key[32];                  // holds the Ck || Ik
 
  byte i = 0;
  
  ASSERT( nonce_ue != NULL );

  ASSERT (nonce_mme != NULL );

  ASSERT(emm_ctrl_data_ptr != NULL); 

  memset((void *)&input_str[0], 0, sizeof(input_str));

  memset((void *)&output_str[0], 0, sizeof(output_str));

  input_str[12] = 0x04; // length of NONCE MME (2 bytes )

  input_str[11] = 0x00;

  memscpy((void*)&input_str[7], sizeof(input_str) - 7,
          (void*)nonce_mme,
          sizeof(emm_ctrl_data_ptr->nas_incoming_msg_ptr->\
            emm_incoming_msg.security_mode_cmd.nonce_mme));

  input_str[6] =  0x04;   // length of NONCE UE (2 bytes)

  input_str[5] = 0x00;

  memscpy((void*)&input_str[1], sizeof(input_str) - 1,
          (void*)nonce_ue,
          sizeof(emm_ctrl_data_ptr->nas_incoming_msg_ptr->\
            emm_incoming_msg.security_mode_cmd.nonce_ue)); 

  input_str[0] = 0x19;  // FC (1 byte )  

  /* get the cached values of Ck and Ik from the MM AUTH Server */

  // Ck || Ik
  memscpy((void*)&key[0], sizeof(key),
          (void*)&auth_ps_gprs_cipherkey.value_data[0],
          auth_ps_gprs_cipherkey.value_length);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Cached CK(auth_ps_gprs_cipherkey)from MM AUTH server - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  auth_ps_gprs_cipherkey.value_data[i], auth_ps_gprs_cipherkey.value_data[i+1],
                  auth_ps_gprs_cipherkey.value_data[i+2], auth_ps_gprs_cipherkey.value_data[i+3],
                  auth_ps_gprs_cipherkey.value_data[i+4], auth_ps_gprs_cipherkey.value_data[i+5],
                  auth_ps_gprs_cipherkey.value_data[i+6], auth_ps_gprs_cipherkey.value_data[i+7]);
  }
  
  memscpy((void*)&key[UMTS_SECURITY_KEY_LENGTH],
          sizeof(key) - UMTS_SECURITY_KEY_LENGTH,
          (void*)&auth_ps_gprs_integritykey.value_data[0],
          auth_ps_gprs_integritykey.value_length);

  MSG_HIGH_DS_0(MM_SUB,"=EMM= Cached IK(auth_ps_gprs_integritykey)from MM AUTH server - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  auth_ps_gprs_integritykey.value_data[i], auth_ps_gprs_integritykey.value_data[i+1],
                  auth_ps_gprs_integritykey.value_data[i+2], auth_ps_gprs_integritykey.value_data[i+3],
                  auth_ps_gprs_integritykey.value_data[i+4], auth_ps_gprs_integritykey.value_data[i+5],
                  auth_ps_gprs_integritykey.value_data[i+6], auth_ps_gprs_integritykey.value_data[i+7]);
  }
  
  input_str_len = sizeof(input_str);

  nas_errno = lte_security_generate_key( key,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,  /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
    return FALSE;
  
  /* save the K'ASME */
  memscpy((void*)&context_ptr->nas_kasme[0],
          sizeof(context_ptr->nas_kasme),
          (void*)&output_str[0], sizeof(output_str));

  MSG_HIGH_DS_0(MM_SUB, "=EMM= K'ASME key generated during PS idle mode mobility to E-UTRAN - including index [0-31]");
  for(i = 0; i+7 < 32; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  context_ptr->nas_kasme[i], context_ptr->nas_kasme[i+1],
                  context_ptr->nas_kasme[i+2], context_ptr->nas_kasme[i+3],
                  context_ptr->nas_kasme[i+4], context_ptr->nas_kasme[i+5],
                  context_ptr->nas_kasme[i+6], context_ptr->nas_kasme[i+7]);
  }
  
  return TRUE;
 }

/*===========================================================================

FUNCTION  EMM_COMPUTE_KHASHASME_PSHO_MOBILITY

DESCRIPTION 
 Compute the K_HASH_ASME during the W2L PS HO
 33.401 v8.6.0, Annex A, Section A.10

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_compute_khashasme_psho_mobility(byte *nonce_mme,
                                            size_t nonce_mme_buf_len, 
                                            emm_ctrl_data_type *emm_ctrl_data_ptr,
                                            emm_security_context_info *context_ptr
)
{
   /* 
       FC = 0x18,
       P0 = NONCEMME
       L0 = length of the NONCEMME (i.e. 0x00 0x04)
       Key =  CK || IK
 */
  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[7];

  byte input_str_len = 0;

  byte output_str[32];

  byte  key[32];                  // holds the Ck || Ik
  byte i = 0;
 
  ASSERT (nonce_mme != NULL );

  ASSERT(emm_ctrl_data_ptr != NULL); 

  memset((void *)&input_str[0], 0, sizeof(input_str));

  memset((void *)&output_str[0], 0, sizeof(output_str));

  input_str[0] = 0x18;  // FC (1 byte ) 
                        // 
  memscpy((void*)&input_str[1], sizeof(input_str) - 1,
          (void*)nonce_mme, nonce_mme_buf_len);

  // length of NONCE MME (2 bytes )
  input_str[5] = 0x00;
  input_str[6] = 0x04; 

  /* get the cached values of Ck and Ik from the MM AUTH Server */
  // Ck || Ik - check if it shud be lte_auth_ps_gprs_cipherkey ? 
  memscpy((void*)&key[0], sizeof(key),
          (void*)&auth_ps_gprs_cipherkey.value_data[0],
          auth_ps_gprs_cipherkey.value_length);

  MSG_HIGH_DS_0(MM_SUB,"=EMM= Print cached CK(auth_ps_gprs_cipherkey) from MM AUTH server - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  auth_ps_gprs_cipherkey.value_data[i], auth_ps_gprs_cipherkey.value_data[i+1],
                  auth_ps_gprs_cipherkey.value_data[i+2], auth_ps_gprs_cipherkey.value_data[i+3],
                  auth_ps_gprs_cipherkey.value_data[i+4], auth_ps_gprs_cipherkey.value_data[i+5],
                  auth_ps_gprs_cipherkey.value_data[i+6], auth_ps_gprs_cipherkey.value_data[i+7]);
  }
  
  memscpy((void*)&key[UMTS_SECURITY_KEY_LENGTH],
          sizeof(key) - UMTS_SECURITY_KEY_LENGTH,
          (void*)&auth_ps_gprs_integritykey.value_data[0],
          auth_ps_gprs_integritykey.value_length);

  MSG_HIGH_DS_0(MM_SUB,"=EMM= Print cached IK(auth_ps_gprs_integritykey) from MM AUTH server - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  auth_ps_gprs_integritykey.value_data[i], auth_ps_gprs_integritykey.value_data[i+1],
                  auth_ps_gprs_integritykey.value_data[i+2], auth_ps_gprs_integritykey.value_data[i+3],
                  auth_ps_gprs_integritykey.value_data[i+4], auth_ps_gprs_integritykey.value_data[i+5],
                  auth_ps_gprs_integritykey.value_data[i+6], auth_ps_gprs_integritykey.value_data[i+7]);
  }
  
  input_str_len = sizeof(input_str);

  nas_errno = lte_security_generate_key( key,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,  /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
  {  
    MSG_HIGH_DS_1(MM_SUB, "=EMM= KhashASME Key Generation Failed, nas_errno = %d ", nas_errno);
    return FALSE;
  }

  /* save the K'ASME */
  memscpy((void*)&context_ptr->nas_kasme[0], sizeof(context_ptr->nas_kasme),
          (void*)&output_str[0], sizeof(output_str));

  MSG_HIGH_DS_0(MM_SUB, "=EMM= K'ASME key generated during handover to E-UTRAN - including index [0-31]");
  for(i = 0; i+7 < 32; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  context_ptr->nas_kasme[i], context_ptr->nas_kasme[i+1],
                  context_ptr->nas_kasme[i+2], context_ptr->nas_kasme[i+3],
                  context_ptr->nas_kasme[i+4], context_ptr->nas_kasme[i+5],
                  context_ptr->nas_kasme[i+6], context_ptr->nas_kasme[i+7]);
  }

  return TRUE;
}

/*===========================================================================

FUNCTION EMM_COMPUTE_NAS_INT_KEY

DESCRIPTION
   This function computes the Integrity key.
  Spec 33.401, section A.7

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 boolean emm_compute_nas_int_key(emm_ctrl_data_type *emm_ctrl_data_ptr
                                 , emm_security_context_info *context_ptr
)
 {
   /* 
       FC = 0x15
       P0 = algorithm type distinguisher
       L0 = length of algorithm type distinguisher (i.e. 0x00 0x01)
       P1 = algorithm identity
       L1 = length of algorithm identity (i.e. 0x00 0x01)

   Table A.8-1 of TS 33.401 for Algorithm type distinguishers
            Algorithm distinguisher           Value
            NAS-enc-alg                         0x01
            NAS-int-alg                         0x02

   Algorithm identity is indicated in the NAS SMC message TS 24.301
   Type of integrity protection algorithm (octet 2, bit 1 to 3)
     Bits
    3 2
    0 0      0            Reserved
    0 0      1            EPS integrity algorithm 128-EIA1
    0 1      0            EPS integrity algorithm 128-EIA2
    0 1      1            EPS integrity algorithm EIA3
    1 0      0            EPS integrity algorithm EIA4
    1 0      1            EPS integrity algorithm EIA5
    1 1      0            EPS integrity algorithm EIA6
    1 1      1            EPS integrity algorithm EIA7

   Key = KASME

   derived key = HMAC-SHA-256 (Key, S),
  */

  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[7];

  byte input_str_len = 0;

  byte output_str[32];

  byte i;

#ifdef FEATURE_LTE_REL9
  byte nas_invalid_kasme[32];
#endif

  ASSERT ( emm_ctrl_data_ptr != NULL );

  ASSERT( context_ptr != NULL );

#ifdef FEATURE_LTE_REL9
  memset(nas_invalid_kasme, 0xFF, sizeof(nas_invalid_kasme));

  /* Rel 9: Cannot compute INT key, since there is no KASME generated for SMC with emergency services */
  if ((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && 
      (memcmp(context_ptr->nas_kasme,nas_invalid_kasme, sizeof(nas_invalid_kasme)) == 0))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Rel 9: emergency services: KASME is 0xFF, do not compute INT KEY, set it to 0xFF ");
    /* Do not compute INT and ENC keys, KASME is set to 0xFF */
    memset(context_ptr->nas_int_key, 0xFF, sizeof(context_ptr->nas_int_key));
    return TRUE;
  }
#endif

  memset((void *)&input_str[0], 0, sizeof(input_str));

  memset((void *)&output_str[0], 0, sizeof(output_str));

  input_str[0] = 0x15; // FC (1 byte ) 

  input_str[1] =  0x02;   // algorithm identity (1 bytes )  

  input_str[2] =  0x00;   // length of algorithm type distinguisher (2 bytes)

  input_str[3] = 0x01; 

  input_str[4] = (byte)context_ptr->security_alg.integrity_protect_alg;   // algorithm type distinguisher ( 1 byte )

  input_str[5] = 0x00;   // length of algorithm identity (2 bytes )

  input_str[6] = 0x01;

  input_str_len = sizeof(input_str);

  nas_errno = lte_security_generate_key( context_ptr->nas_kasme,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,  /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
    return FALSE;

 
  /* truncate the value to 128 bits */
  memscpy((void*)&context_ptr->nas_int_key[0], sizeof(context_ptr->nas_int_key),
          (void*)&output_str[16], sizeof(output_str) - 16);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= INT Key Rx from LTE_SECURITY - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  context_ptr->nas_int_key[i], context_ptr->nas_int_key[i+1],
                  context_ptr->nas_int_key[i+2], context_ptr->nas_int_key[i+3],
                  context_ptr->nas_int_key[i+4], context_ptr->nas_int_key[i+5],
                  context_ptr->nas_int_key[i+6], context_ptr->nas_int_key[i+7]);
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  return TRUE;
}

/*===========================================================================

FUNCTION EMM_COMPUTE_NAS_ENC_KEY

DESCRIPTION
  This function computes the Encryption key.
  Spec 33.401, section A.7 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 boolean emm_compute_nas_enc_key(emm_ctrl_data_type *emm_ctrl_data_ptr 
                                 , emm_security_context_info *context_ptr
)
 {
   /* 
       FC = 0x15
       P0 = algorithm type distinguisher
       L0 = length of algorithm type distinguisher (i.e. 0x00 0x01)
       P1 = algorithm identity
       L1 = length of algorithm identity (i.e. 0x00 0x01)

   Table A.8-1 of TS 33.401 for Algorithm type distinguishers 
             Algorithm distinguisher             Value
             NAS-enc-alg                         0x01
             NAS-int-alg                         0x02

   Algorithm identity is indicated in the NAS SMC message TS 24.301
   
    Type of ciphering algorithm (octet 2, bit 5 to 7)
    
        Bits
    7 6      5
    0 0      0            EPS encryption algorithm 128-EEA0 (ciphering not used)
    0 0      1            EPS encryption algorithm 128-EEA1
    0 1      0            EPS encryption algorithm 128-EEA2
    0 1      1            EPS encryption algorithm EEA3
    1 0      0            EPS encryption algorithm EEA4
    1 0      1            EPS encryption algorithm EEA5
    1 1      0            EPS encryption algorithm EEA6
    1 1      1            EPS encryption algorithm EEA7


   Key = Kasme
  */
  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[7];

  byte input_str_len = 0;

  byte output_str[32];
  byte i = 0;

#ifdef FEATURE_LTE_REL9
  byte nas_invalid_kasme[32];
#endif

  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT( context_ptr != NULL );

#ifdef FEATURE_LTE_REL9
  memset(nas_invalid_kasme, 0xFF, sizeof(nas_invalid_kasme));

  /* Rel 9: Cannot compute INT key, since there is no KASME generated for SMC with emergency services */
  if ((emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && 
      (memcmp(context_ptr->nas_kasme,nas_invalid_kasme, sizeof(nas_invalid_kasme)) == 0))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Rel 9: emergency services: KASME is 0xFF, do not compute ENC KEY, set it to 0xFF ");
    /* Do not compute INT and ENC keys, KASME is set to 0xFF */
    memset(context_ptr->nas_enc_key, 0xFF, sizeof(context_ptr->nas_enc_key));
    return TRUE;
  }
#endif


  memset(input_str, 0, sizeof(input_str));

  memset(output_str, 0, sizeof(output_str));

  input_str[0]  = 0x15;  // FC (1 byte )

  input_str[1] = 0x01;   // algorithm type distinguisher ( 1 byte )

  input_str[2] = 0x00;   // length of algorithm type distinguisher (2 bytes)

  input_str[3] = 0x01;

  input_str[4] = (byte)context_ptr->security_alg.ciphering_alg;  // algorithm identity ( 1 byte )

  input_str[5] =  0x00;

  input_str[6] =  0x01; // length of algorithm identity (2 bytes )

  input_str_len = sizeof(input_str);

  nas_errno = lte_security_generate_key( context_ptr->nas_kasme,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,     /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
    return FALSE;
  
  memscpy((void*)&context_ptr->nas_enc_key[0], sizeof(context_ptr->nas_enc_key),
          (void*)&output_str[16], sizeof(output_str) - 16);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= ENC Key Rx from LTE_SECURITY - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  context_ptr->nas_enc_key[i], context_ptr->nas_enc_key[i+1],
                  context_ptr->nas_enc_key[i+2], context_ptr->nas_enc_key[i+3],
                  context_ptr->nas_enc_key[i+4], context_ptr->nas_enc_key[i+5],
                  context_ptr->nas_enc_key[i+6], context_ptr->nas_enc_key[i+7]);
  }

#ifdef TEST_FRAMEWORK
  #error code not present
#endif

  return TRUE;
}/* end emm_compute_nas_enc_key() */



/*===========================================================================

FUNCTION EMM_RESET_SECURITY_INFO

DESCRIPTION
  This function resets all the parameters in the security context. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_reset_security_info (emm_security_context_info *context_info)
{

  ASSERT ( context_info != NULL );

  context_info->nasKSI.keyset_id = NO_KEY_AVAIALABLE;

  memset((void *)&context_info->nas_kasme[0], 0xFF, sizeof(context_info->nas_kasme));

  memset((void *)&context_info->nas_ul_cnt[0],0xFF, sizeof(context_info->nas_ul_cnt));

  memset((void *)&context_info->nas_dl_cnt[0], 0xFF, sizeof(context_info->nas_dl_cnt));

  memset((void *)&context_info->nas_int_key[0], 0xFF, sizeof(context_info->nas_int_key));

  memset((void *)&context_info->nas_enc_key[0], 0xFF, sizeof(context_info->nas_enc_key));

  context_info->in_use = FALSE;

  context_info->context_state = CONTEXT_STATE_NONE;

  context_info->context_type = CONTEXT_TYPE_NONE;
}

 /*===========================================================================

FUNCTION EMM_GET_UL_SECURITY_INFO

DESCRIPTION
  This function is used to retrieve the Uplink security context info, along with the UL NAS count. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_ul_security_info_type * emm_get_ul_security_info(emm_ctrl_data_type *emm_ctrl_data_ptr, lte_nas_ul_security_info_type *nas_security)
{
 
  byte sqn = 0;
  word over_flow_cnt = 0;

  emm_security_context_info *context_ptr = NULL;

  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT( nas_security != NULL );

  context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  if( context_ptr != NULL )
  {
    /* reset the flags */
    nas_security->int_valid = TRUE;

    nas_security->cipher_valid = FALSE;

#ifdef FEATURE_LTE_REL9
    if( ( context_ptr->nasKSI.keyset_id != NO_KEY_AVAIALABLE) || 
         ( ( emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && /* Rel 9: SMC for Emergency services with NULL Algos */
           ( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
           ( context_ptr->security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) ) )  // LSTI req 
#else
    if( ( context_ptr->nasKSI.keyset_id != NO_KEY_AVAIALABLE) || 
        ( ( context_ptr->nasKSI.keyset_id == NO_KEY_AVAIALABLE ) && 
          ( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
          ( context_ptr->security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) ) )  // LSTI req 
#endif
    {
      /* set the encryption information */
      /* Note: The SEC API can support only SNOW 3G and AES.
               This assumes that the MSG LIB will put only one type of algorithm
               info in this field even if SMC received indicates more than one type
      */
      if( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA1 )
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_SNOW3G;
      }
      else if( context_ptr->security_alg.ciphering_alg ==  ENCRYPTION_ALGO_128_EEA2)
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_AES;
      }
      else if ( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALG_EEA3 )
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_ZUC;
      }
      else
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_NONE;
      }

      if( nas_security->security_alg.ciphering_alg != LTE_CIPHER_ALGO_NONE )
      {
        /* copy the key only if it is a valid algorithm */
        memscpy((void*)&nas_security->security_alg.ciphering_alg_key[0],
                sizeof(nas_security->security_alg.ciphering_alg_key),
                (void*)&context_ptr->nas_enc_key[0],
             sizeof(context_ptr->nas_enc_key));
      }

      /* set the integrity information */
      if ( context_ptr->security_alg.integrity_protect_alg == INTEGRITY_ALGO_128_EIA1 )
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_SNOW3G;
      }
      else if ( context_ptr->security_alg.integrity_protect_alg == INTEGRITY_ALGO_128_EIA2 )
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_AES;
      }
      else if ( context_ptr->security_alg.integrity_protect_alg == INTEGRITY_ALGO_EIA3 )
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_ZUC;
      }
      else 
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_NONE;
      }

      if( nas_security->security_alg.integrity_alg != LTE_INTEGRITY_ALGO_NONE )
      {
        memscpy((void*)&nas_security->security_alg.integrity_alg_key[0],
                sizeof(nas_security->security_alg.integrity_alg_key),
                (void*)&context_ptr->nas_int_key[0],
                sizeof(context_ptr->nas_int_key));
      }
      /* compute the nas ul count */
      
      sqn = context_ptr->nas_ul_cnt[0];

      /* increment the NAS SQN, if the NAS count was not reset due to an SMC.
         NAS count starts at 0, other first SMC complete will be sent with SQN 1
      */
      if(reset_nas_ul_count)
      {
        memset( (void *)nas_security->nas_count, 0, sizeof(nas_security->nas_count));
        reset_nas_ul_count = FALSE;
        /*resetting the uplink generated global varible */
        mm_reset_ul_count_at_generation();
      }
      else 
      {
        sqn++;

        over_flow_cnt = ( (word )context_ptr->nas_ul_cnt[2] << 8) | context_ptr->nas_ul_cnt[1];

        if ( sqn == 0) 
        {
          over_flow_cnt++ ;
        }
        context_ptr->nas_ul_cnt[0] = sqn;
        context_ptr->nas_ul_cnt[1] = (byte )(over_flow_cnt & 0xFF);
        context_ptr->nas_ul_cnt[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
        /* increment the over flow count if SQN wraps around */
        
        memscpy((void*)&nas_security->nas_count[0],
                sizeof(nas_security->nas_count),
                (void*)&context_ptr->nas_ul_cnt[0],
                sizeof(context_ptr->nas_ul_cnt));
      }

        MSG_HIGH_DS_4(MM_SUB, "=EMM= context_ptr->nas_ul_cnt[0] = %d context_ptr->nas_ul_cnt[1] = %d context_ptr->nas_ul_cnt[2] = %d context_ptr->nas_ul_cnt[3] = %d",
                      context_ptr->nas_ul_cnt[0],
                      context_ptr->nas_ul_cnt[1],
                      context_ptr->nas_ul_cnt[2],
                      context_ptr->nas_ul_cnt[3]);
        
      /* Note: the above is true only when the over flow count has not wrapped around. It is assumed that
               when the over flow count wraps around, the MME will trigger a new AKA to re-authenticate the UE
      */
      nas_security->bearer = 0;

      if( emm_ctrl_data_ptr->nas_secure_exchange )
      {
        nas_security->int_valid = TRUE;
        nas_security->cipher_valid = TRUE;
      } 
      else 
      {
        nas_security->int_valid = TRUE;
        nas_security->cipher_valid = FALSE;
        
      }
    }
    return nas_security;
  }
  else 
    return NULL;
}


/*===========================================================================

FUNCTION EMM_GET_DL_SECURITY_INFO

DESCRIPTION
  This function is used to retrieve the Downlink security context info, along with the DL NAS count.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
lte_nas_dl_security_info_type *emm_get_dl_security_info(emm_ctrl_data_type *emm_ctrl_data_ptr, 
                                                        lte_nas_dl_security_info_type *nas_security,
                                                        boolean smc,
                                                        byte *incoming_ota_message)
{
 
  byte sqn = 0;
  word over_flow_cnt = 0;

  emm_security_context_info *context_ptr = NULL;
  byte num_bits_to_read = 0, security_hdr_offset = 0,security_hdr_type = 0;

  ASSERT( nas_security != NULL );

  ASSERT( emm_ctrl_data_ptr != NULL );

  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );
  
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  
  if( context_ptr != NULL )  
  {
    MSG_HIGH_DS_2(MM_SUB,"=EMM= get_dl_sec_info: Current context of type %d, KSI %d ", 
                  context_ptr->context_type, context_ptr->nasKSI.keyset_id);

#ifdef FEATURE_LTE_REL9
    if( ( context_ptr->nasKSI.keyset_id != NO_KEY_AVAIALABLE) ||
        ( ( emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE) && /* Rel 9: SMC for Emergency services with NULL Algos */
          ( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
          ( context_ptr->security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) ) ) 
#else
    if( ( context_ptr->nasKSI.keyset_id != NO_KEY_AVAIALABLE) ||
        ( ( context_ptr->nasKSI.keyset_id == NO_KEY_AVAIALABLE ) && 
        ( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALGO_128_EEA0 ) &&
        ( context_ptr->security_alg.integrity_protect_alg == (lte_nas_emm_integrity_protect_alg_type)0 ) ) )  // LSTI req 
#endif
    {
      /* set the encryption information */
      /* Note: The SEC API can support only SNOW 3G and AES.
               This assumes that the MSG LIB will put only one type of algorithm
               info in this field even if SMC received indicates more than one type
      */
      if( context_ptr->security_alg.ciphering_alg  == ENCRYPTION_ALGO_128_EEA1 )
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_SNOW3G;
      }
      else if( context_ptr->security_alg.ciphering_alg  ==  ENCRYPTION_ALGO_128_EEA2)
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_AES;
      }
      else if ( context_ptr->security_alg.ciphering_alg == ENCRYPTION_ALG_EEA3 )
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_ZUC;
      }
      else
      {
        nas_security->security_alg.ciphering_alg = LTE_CIPHER_ALGO_NONE;
      }

      if( nas_security->security_alg.ciphering_alg != LTE_CIPHER_ALGO_NONE )
      {
        /* copy the key only if it is a valid algorithm */
        memscpy((void*)&nas_security->security_alg.ciphering_alg_key[0],
                sizeof(nas_security->security_alg.ciphering_alg_key),
                (void*)&context_ptr->nas_enc_key[0],
                sizeof(context_ptr->nas_enc_key));
      }

      /* set the integrity information */
      if( context_ptr->security_alg.integrity_protect_alg == INTEGRITY_ALGO_128_EIA1 )
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_SNOW3G;
      }
      else if ( context_ptr->security_alg.integrity_protect_alg == INTEGRITY_ALGO_128_EIA2 )
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_AES;
      }
      else if ( context_ptr->security_alg.integrity_protect_alg == INTEGRITY_ALGO_EIA3 )
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_ZUC;
      }
      else 
      {
        nas_security->security_alg.integrity_alg = LTE_INTEGRITY_ALGO_NONE;
      }

      if( nas_security->security_alg.integrity_alg != LTE_INTEGRITY_ALGO_NONE )
      {
        memscpy((void*)&nas_security->security_alg.integrity_alg_key[0],
                sizeof(nas_security->security_alg.integrity_alg_key),
                (void*)&context_ptr->nas_int_key[0],
                sizeof(context_ptr->nas_int_key));
      }
      /* compute the nas ul count */
      
      sqn = dl_sqn_for_current_msg;
      MSG_HIGH_DS_1(MM_SUB, "=EMM= emm_get_dl_sec_info - Incoming DL Count = %d ",
                    dl_sqn_for_current_msg);

      if(incoming_ota_message != NULL)
      {
        num_bits_to_read = 4;         
        security_hdr_type = (byte)b_unpackb(incoming_ota_message, security_hdr_offset, num_bits_to_read);
        if(security_hdr_type == PLAIN_NAS_MSG)
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= OTA message is PLAIN, not updating the NAS counts");
          nas_security->nas_connected_mode = emm_ctrl_data_ptr->nas_secure_exchange;
          return nas_security; /*Check if the security header is PLAIN, if so then return SECURITY_CONEXT (which is non_null)*/
        }
      }
      /* increment the NAS SQN, if the NAS count was not reset due to an SMC.
         NAS count starts at 0, other first SMC complete will be sent with SQN 1
      */
      if(reset_nas_dl_count)
      {
        memset( (void *)nas_security->nas_count, 0, sizeof(nas_security->nas_count));
        reset_nas_dl_count = FALSE;
        MSG_HIGH_DS_3(MM_SUB,"=EMM= Reset DL counts for SMC , last_incoming_sqn = %d, current sqn = %d, smc = %d ",
                      emm_ctrl_data_ptr->last_incoming_sqn, sqn, smc);
      }
      over_flow_cnt = ( (word )context_ptr->nas_dl_cnt[2] << 8) | context_ptr->nas_dl_cnt[1];

      MSG_HIGH_DS_3(MM_SUB,"=EMM= old last_incoming_sqn = %d, current sqn = %d, smc = %d",
                    emm_ctrl_data_ptr->last_incoming_sqn, sqn, smc);

      if( ( emm_ctrl_data_ptr->last_incoming_sqn >= sqn ) && (!smc) )
      {
        MSG_HIGH_DS_3(MM_SUB,"=EMM= Incrementing overflow count, last_incoming_sqn = %d, current sqn = %d, smc = %d",
                      emm_ctrl_data_ptr->last_incoming_sqn, sqn, smc);
        over_flow_cnt++ ;
      }
        
      emm_ctrl_data_ptr->last_incoming_sqn = sqn;

      nas_security->nas_count[0] = sqn;
      nas_security->nas_count[1] = (byte )(over_flow_cnt & 0xFF);
      nas_security->nas_count[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
      nas_security->nas_count[3] = 0;
        
      if (((incoming_ota_message != NULL) && 
          (security_hdr_type != INTEGRITY_PROTECT_SECURITY_MODE_CMD )) ||
          (smc == TRUE))
      {
        MSG_HIGH_DS_1(MM_SUB, "=EMM= copy SQN into DL count, smc = %d ", smc);
        memscpy((void*)&context_ptr->nas_dl_cnt[0], 
                sizeof(context_ptr->nas_dl_cnt),
                (void*)&nas_security->nas_count[0], 
                sizeof(nas_security->nas_count));
      }
      else
      {
        /* Called from process_smc */
        MSG_HIGH_DS_1(MM_SUB, "=EMM= Do not copy SQN into Current context DL count , smc = %d ",
                      smc);
      }

      MSG_HIGH_DS_4(MM_SUB, "=EMM= nas_security->nas_count[0] = %d nas_security->nas_count[1] = %d nas_security->nas_count[2] = %d nas_security->nas_count[3] = %d", 
                              nas_security->nas_count[0],
                              nas_security->nas_count[1],
                              nas_security->nas_count[2],
                              nas_security->nas_count[3]);

      MSG_HIGH_DS_4(MM_SUB, "=EMM= context_ptr->nas_dl_cnt[0] = %d context_ptr->nas_dl_cnt[1] = %d context_ptr->nas_dl_cnt[2] = %d context_ptr->nas_dl_cnt[3] = %d ", 
                              context_ptr->nas_dl_cnt[0],
                              context_ptr->nas_dl_cnt[1], 
                              context_ptr->nas_dl_cnt[2],
                              context_ptr->nas_dl_cnt[3]);

      /* Note: the above is true only when the over flow count has not wrapped around. It is assumed that
               when the over flow count wraps around, the MME will trigger a new AKA to re-authenticate the UE
      */
      nas_security->bearer = 0;

      nas_security->nas_connected_mode = emm_ctrl_data_ptr->nas_secure_exchange;
    }

    return nas_security;

  } else 
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= get_dl_sec_info: No Current context, return NULL ");
    return NULL;    
  }
  
}

/*===========================================================================

FUNCTION EMM_DELETE_EFS_SECURITY_CONTEXT

DESCRIPTION
  This function deltes the security context that is saved on the EFS. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_delete_efs_security_context( void )
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_EFS_ERR;

  status = mcfg_fs_delete(EPS_NATIVE_SECURITY_CONTEXT_FILE,
                          MCFG_FS_TYPE_EFS,
                         (mcfg_fs_sub_id_e_type)mm_sub_id);

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Deleting Native Security Context file in EFS, status - %d", status);
}

/*===========================================================================

FUNCTION EMM_DELETE_MAPPED_CONTEXT

DESCRIPTION
  This function deletes the mapped security context (resets the context)
  from the EMM global emm_ctrl_data_ptr.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_delete_mapped_context( void )
{
  emm_state_type               emm_state    = EMM_INVALID_STATE ;

  int index;
  emm_security_context_info *context_ptr = NULL;

  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT( emm_ctrl_data_ptr != NULL );

  emm_state    = EMM_GET_STATE() ;

  if ( emm_state == EMM_DEREGISTERED )
  {
    context_ptr = emm_get_context_by_state ( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT );

    if( context_ptr != NULL )
    {
      if(context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS) 
      {
        context_ptr = emm_get_context_by_type ( emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS );

        if( ( context_ptr != NULL ) && ( context_ptr->context_state == CONTEXT_STATE_NON_CURRENT ) )
        {
          MSG_HIGH_DS_0(MM_SUB, "=EMM= Set the non-current Full Native context to Current");
          /* make this the current context if there is a non current native eps context */
          context_ptr->context_state = CONTEXT_STATE_CURRENT;
        }
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= No Mapped context found to delete");
      }
    }
    for ( index = 0; index < EMM_MAX_CONTEXTS; index++ )
    {
      context_ptr = (emm_security_context_info *)(&emm_ctrl_data_ptr->context_info[index]);
      if( ( context_ptr->in_use) && ( context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS) )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Deleting Mapped Context");
        emm_reset_security_info(context_ptr);
      }

      if( ( context_ptr->in_use) &&  
          ( context_ptr->context_type == CONTEXT_TYPE_PARTIAL_NATIVE_EPS ) )
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Delete Partial Context and cancel any on-going Authentication");
        emm_reset_security_info(context_ptr);
        emm_cancel_authentication();
      }
    }
  }
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION EMM_PROCESS_IDENTITY_REQUEST

DESCRIPTION
  This function processes the Identity Request message.
  Spec 24.301, section 5.4.4

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 void emm_process_identity_request
(
  lte_nas_emm_identity_req  *identity_request,
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
  byte  i = 0;
  byte  imeisv[MAX_IMEI_LEN+2];


  lte_nas_emm_ms_id_type  response;

  mobile_id_type  identity_type;

  ASSERT( emm_ctrl_data_ptr != NULL );
  ASSERT ( identity_request != NULL );

  if( emm_ctrl_data_ptr->detach_reason == SWITCH_OFF )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= IDENTITY REQUEST received while switching off UE -- Message Ignored");

#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    return;
  }
  identity_type = identity_request->mobile_identity;

  memset((void *)&response, 0, sizeof( lte_nas_emm_ms_id_type)); 

  MSG_HIGH_DS_1(MM_SUB, "=EMM= Rcved IDENTITY REQ w/ type %d", identity_type);

  if((emm_ctrl_data_ptr->plmn_service_state->reg_service_domain == 
                                                          SYS_SRV_DOMAIN_CS_PS) &&
     (identity_type == LTE_NAS_EMM_TMSI))
  {
    MSG_HIGH_DS_1(MM_SUB,"=EMM= Location_update_status = %d",
                emm_ctrl_data_ptr->emm_location_area->location_update_status);

    if(emm_ctrl_data_ptr->emm_location_area->location_update_status == 
                                                                       UPDATED)
    {
      if(FALSE == mm_is_tmsi_valid(emm_ctrl_data_ptr->emm_location_area->tmsi,TMSI_SIZE))
      {     
        MSG_ERROR_DS_0(MM_SUB, "=EMM= TMSI is invalid, setting the Identity_Response to LTE_NAS_EMM_NO_ID");
        response.mobile_id =  LTE_NAS_EMM_NO_ID;
      }
      else
      {
        response.mobile_id = LTE_NAS_EMM_TMSI;
        response.mobile_id_lst.tmsi.length = TMSI_SIZE;
        for (i=0; i < TMSI_SIZE; i++)
        {
          response.mobile_id_lst.tmsi.value[i] = emm_ctrl_data_ptr->emm_location_area->tmsi[i];
        }
        MSG_HIGH_DS_0(MM_SUB, "=EMM= Identity_Response is set to TMSI");
      }
    }
    else
    {      
      MSG_ERROR_DS_0(MM_SUB, "=EMM= TMSI is invalid, setting the Identity_Response to LTE_NAS_EMM_IMSI");
      response.mobile_id =  LTE_NAS_EMM_NO_ID;
    }   
  }
  else
  {
    switch(identity_type)
  {
      case LTE_NAS_EMM_IMEI:
    response.mobile_id = LTE_NAS_EMM_IMEI;
      emm_populate_imei(&response.mobile_id_lst.imei, FALSE);
        break;
      case LTE_NAS_EMM_IMEISV:
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
        ghdi_ds_read_imeisv(imeisv,(sys_modem_as_id_e_type)mm_sub_id);
#else
        read_imeisv(imeisv) ;
#endif
        response.mobile_id = LTE_NAS_EMM_IMEISV;
        /* even odd indicators are already set  */
        response.mobile_id_lst.imeisv.length = imeisv[0];
        memscpy((void*)&response.mobile_id_lst.imeisv.value[0],
              sizeof(response.mobile_id_lst.imeisv.value),
              (void*)&imeisv[1], response.mobile_id_lst.imeisv.length);
        break;
      case LTE_NAS_EMM_IMSI:
      /* If mobile_id is not one of IMSI/IMEI/IMEISV, then the default value will be LTE_NAS_EMM_IMSI */ 
      default:
        if(emm_ctrl_data_ptr->plmn_service_state->sim_state == 
                                                   SYS_SIM_STATE_NOT_AVAILABLE)
    {
      response.mobile_id =  LTE_NAS_EMM_NO_ID;
    }
    else
    {   
      response.mobile_id =  LTE_NAS_EMM_IMSI;
      response.mobile_id_lst.imsi.num_id_digits_len = emm_ctrl_data_ptr->imsi.num_digits;
      for (i=0; i < emm_ctrl_data_ptr->imsi.num_digits; i++)
      {
        response.mobile_id_lst.imsi.num_id_digits[i] = emm_ctrl_data_ptr->imsi.imsi_digits[i];
      }
    }
        break;
  }
  }

  //Send the Identity response 
  emm_send_identity_response(&response, emm_ctrl_data_ptr);

  /* Notify context lost */
  emm_send_irat_ctxt_lost_ind(LTE_NAS_CTXT_LOST_IDENTITY_REQUEST);

} /* emm_handle_identity_request() */

/*===========================================================================

FUNCTION EMM_READ_IMEI

DESCRIPTION 
  This function is used to read the IMEI while processing the Identity Request message.
  Spec 24.301, section 5.4.4

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_read_imei( byte * write_ptr, size_t write_ptr_buf_len )
{
  byte stored_imei[MAX_IMEI_LEN+1] = {0};
  ghdi_status_T  status = GHDI_SUCCESS;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
  status = ghdi_ds_read_imei(stored_imei,(sys_modem_as_id_e_type)mm_sub_id);
#else
  status = ghdi_read_imei(stored_imei);
#endif
  if(status != GHDI_SUCCESS)
  {
    MSG_ERROR_DS_0(MM_SUB, "Error reading IMEI");
  }
  else
  {
    if (stored_imei[0] == MAX_IMEI_LEN)
    {

      /* Ensure that the last digit of the IMEI is set to zero
       (see TS 23.003 section 6.2.1)
      */
      stored_imei[8] &= 0x0F;
    }
  }
  

  if( stored_imei[0] == MAX_IMEI_LEN)
  {
    if (write_ptr != NULL)
    {
       memscpy((void*)write_ptr, write_ptr_buf_len,
               (void*)&stored_imei[0], sizeof(stored_imei));
    }
    }
    else
    {
      MSG_ERROR_DS_1(MM_SUB, "IMEI wrong size %d from ghdi", stored_imei[0]);
    }
}

/*===========================================================================

FUNCTION      EMM_SEND_IDENTITY_RESPONSE

DESCRIPTION
  This function sends identity response  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_identity_response
(
  lte_nas_emm_ms_id_type              *mobile_id,
  emm_ctrl_data_type*                 emm_ctrl_data_ptr
)
{
  
  dword                               sequence_number = 0xFFFFFFFF;
  lte_nas_emm_identity_resp           *identity_response;

  ASSERT(emm_ctrl_data_ptr != (emm_ctrl_data_type *)NULL); 
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  identity_response = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.identity_resp;

  identity_response->lte_nas_hdr.pd     = EPS_MOBILITY_MANAGEMENT_MESSAGES;

  identity_response->lte_nas_hdr.msg_id = EMM_IDENTITY_RESPONSE;

  identity_response->security_hdr       = PLAIN_NAS_MSG;

  identity_response->ms_id = *mobile_id;
  
  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  /* Send the message and process any transmission failures using 
     emm_security_msg_tx_fail() callback
  */
  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number, 
                                MSGR_NAS_EMM, 
                                NULL, 
                                emm_security_msg_tx_fail, 
                                emm_ctrl_data_ptr))
  {
    MSG_FATAL_DS(MM_SUB,"=EMM= Cannot send IDENTITY RESPONSE, RRC Connection Lost",0,0,0);
  }
} /* end of emm_identity_response*/


/*===========================================================================

FUNCTION  EMM_ABORT_ONGOING_SEC_PROCEDURE

DESCRIPTION
  This function is a wrapper function that is used to cancel any ongoing security procedures like -
  Authentication, SMC and Reallocation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_abort_ongoing_sec_procedure(emm_ctrl_data_type *emm_ctrl_data_ptr )
{
  ASSERT( emm_ctrl_data_ptr != NULL );

  switch (ongoing_sec_procedure)
  {
    case EMM_AUTHENTICATION_RESPONSE:
    case EMM_AUTHENTICATION_REJECT:
      emm_cancel_authentication();
      break;
    case SECURITY_MODE_REJECT:
    case SECURITY_MODE_COMPLETE:
      emm_cancel_security_mode(emm_ctrl_data_ptr);
      break;
    case GUTI_REALLOCATION_COMPLETE:
      //emm_cancel_guti_realloc(emm_ctrl_data_ptr);
      break;
    default:
      /* should not reach here */
      MSG_ERROR_DS_0(MM_SUB, "Unknown security procedure to be aborted!");
      break;

  }

  ongoing_sec_procedure = ESM_LAST_UNDEFINED;
}

/*===========================================================================

FUNCTION  DECODELENGTH

DESCRIPTION
  This function is used to decode the lengh of an input buffer, used while parsing the security context from USIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int decodelength(byte *len, byte *offset) 
{

  //len should point to the first byte of length field
  int decoded_len = 0;
  int first_byte = 0;
  int i = 0;
  int number_of_octets = 0;

   
  ASSERT( len != NULL );

  ASSERT ( offset != NULL );

  first_byte = *len++;

  if( first_byte & 0x80 )
  { 
    //Multi-byte length
    number_of_octets = first_byte & 0x7F;
    for(i = 0; i < number_of_octets; i++ ){
      decoded_len *= 256;
      decoded_len += *len++;
    }
    *offset = (byte)number_of_octets;
  } else{
    //Single byte length
    decoded_len = first_byte;
    *offset = 1;
  }
  return decoded_len;
}

/*===========================================================================

FUNCTION  ENCODELENGTH

DESCRIPTION
  This function is used to encode the lengh of an input buffer, used while writing the security context from USIM/EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte encodelength(byte *buffer, word number)
{

  byte offset = 0;

  ASSERT( buffer != NULL );

  ASSERT( number != NULL );

  if( number > 127 )
  { 

    /* multi byte  */
    *buffer++ = 0x80 | 0x02;  /*length byte with Multibyte set */
   
    *buffer++ = (byte )(number/256);    /* first byte */

    *buffer = number % 256;  /* second byte */

    offset = 3;

  }else
  {
    //Single byte length
    *buffer = (byte )number;  /* first byte */
    offset = 1;

  }

  return offset;
}

/*===========================================================================

FUNCTION  EMM_SEND_GUTI_REALLOC_COMPLETE

DESCRIPTION
  This function sends the GUTI Reallocation complete message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_send_guti_realloc_complete(emm_ctrl_data_type *emm_ctrl_data_ptr)
{

  lte_nas_emm_guti_realloc_complete *emm_guti_realloc_complete;
  dword                               sequence_number = 0xFFFFFFFF;

  ASSERT ( emm_ctrl_data_ptr != NULL );
  ASSERT(emm_ctrl_data_ptr->out_msg_ptr != NULL); 

  memset((void *)emm_ctrl_data_ptr->out_msg_ptr ,
         0x0,
         sizeof(lte_nas_outgoing_msg_type));

  emm_guti_realloc_complete = &emm_ctrl_data_ptr->out_msg_ptr->emm_outgoing_msg.guti_realloc_complete;

  emm_guti_realloc_complete->lte_nas_hdr.pd = EPS_MOBILITY_MANAGEMENT_MESSAGES ;

  emm_guti_realloc_complete->lte_nas_hdr.msg_id = GUTI_REALLOCATION_COMPLETE ;

  emm_guti_realloc_complete->security_hdr = PLAIN_NAS_MSG ;

  /* Obtain sequence number */
  sequence_number = emm_get_nas_transaction_id() ;

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sending GUTI REALLOC COMPLETE message to NW");

  if (FALSE == emm_send_message(emm_ctrl_data_ptr->out_msg_ptr, 
                                sequence_number,
                                MSGR_NAS_EMM,
                                NULL, 
                                emm_security_msg_tx_fail, 
                                emm_ctrl_data_ptr))
  {
    MSG_ERROR_DS_0(MM_SUB, "=EMM= GUTI REALLOC COMPLETE tx fail");
  }
  
  /* keep track of the ongoing procedure for tx failure handling */
  ongoing_sec_procedure = GUTI_REALLOCATION_COMPLETE;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION  EMM_PROCESS_GUTI_REALLOC_CMD

DESCRIPTION 
  This function processes GUTI REALLOCATION CMD 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_guti_realloc_cmd
(
  lte_nas_emm_guti_realloc_cmd  *emm_guti_realloc_cmd,
  emm_ctrl_data_type            *emm_ctrl_data_ptr
)
{
  
  ASSERT ( emm_guti_realloc_cmd != NULL);
  ASSERT ( emm_ctrl_data_ptr != NULL );
  ASSERT ( emm_ctrl_data_ptr->emm_tai_list_ptr != NULL );

  MSG_LOW_DS_0(MM_SUB, "=EMM= Processing GUTI REALLOC CMD");

  switch(EMM_GET_STATE())
  { 
    case EMM_REGISTERED:
      if(EMM_GET_SUBSTATE() == EMM_REGISTERED_IMSI_DETACH_INITIATED)
      {
        MSG_HIGH_DS_0(MM_SUB, "=EMM= GUTI REALLOC CMD ignored - IMSI DETACH");
#ifdef TEST_FRAMEWORK
        #error code not present
#endif /* TEST_FRAMEWORK */
        return;
      }
      break;
    case EMM_DEREGISTERED_INITIATED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= GUTI REALLOC CMD ignored - EPS/COMBINED DETACH");
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      return;
    case EMM_TRACKING_AREA_UPDATING_INITIATED:
      MSG_HIGH_DS_0(MM_SUB, "=EMM= GUTI REALLOC CMD ignored - TAU");
#ifdef TEST_FRAMEWORK
      #error code not present
#endif /* TEST_FRAMEWORK */
      return;
    default:
      break;
  }

  emm_ctrl_data_ptr->emm_guti = emm_guti_realloc_cmd->guti;
  emm_ctrl_data_ptr->plmn_service_state->rac_or_mme_code = emm_ctrl_data_ptr->emm_guti.mme_code;

  /* Process new TAI list if provided */
  if (emm_guti_realloc_cmd->tai_list_exists == TRUE)
  {
    memscpy((void*)emm_ctrl_data_ptr->emm_tai_list_ptr, 
             sizeof(lte_nas_emm_tai_lst_info_type),
             (void*)&(emm_guti_realloc_cmd->tai_lst),
             sizeof(lte_nas_emm_tai_lst_info_type));

#ifdef FEATURE_LTE_REL9
    if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
#endif
    {
      /* Process new TAI list in order to update forbidden lists */
      emm_process_tai_list(emm_ctrl_data_ptr->emm_tai_list_ptr, emm_ctrl_data_ptr) ;
    }
  }


  /* Update EMM USIM */
  emm_save_mobility_info(emm_ctrl_data_ptr) ;

  /* Update REG */
  reg_service_report(MMR_CAUSE_NOT_SPECIFIED, FALSE, emm_ctrl_data_ptr);

  /* Inform RRC regarding change of registration info */
  emm_send_rrc_system_update(emm_ctrl_data_ptr) ;

  /* Inform RRC that GUTI has been changed */
  emm_send_sim_update(emm_ctrl_data_ptr) ;

  /* Send a GUTI Reallocation complete */
  emm_send_guti_realloc_complete(emm_ctrl_data_ptr);
}


/*===========================================================================

FUNCTION  EMM_UPDATE_STORED_SECURITY_CONTEXT

DESCRIPTION
  This function updates the stored security context on the USIM/EFS
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void emm_update_stored_security_context(emm_ctrl_data_type *emm_ctrl_data_ptr,
                                        boolean is_invalid)

{

  emm_security_context_info curr_security_context, *context_ptr = NULL, *temp_context_ptr = NULL;
  static emm_security_context_info last_known_security_context;

  ASSERT( emm_ctrl_data_ptr != NULL );
#ifdef FEATURE_LTE_REL9
  /* 24.301, Rel 9, Annex C ,
         If the UE is attached for emergency bearer services,
         the UE shall not store the EMM parameters on the USIM or in non-volatile memory */
  if (emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_ONLY)
  {
#endif

  memset( (void *)&curr_security_context, 0, sizeof( curr_security_context));

    temp_context_ptr = emm_get_context_by_state(emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

    if ( temp_context_ptr != NULL )
  {
    /*Check if there was a FULL_NATIVE_SECURITY_CONTEXT, 
     if not there could be MAPPED_EPS_CONTEXT*/
    context_ptr = emm_get_context_by_type ( emm_ctrl_data_ptr, CONTEXT_TYPE_FULL_NATIVE_EPS );
      if(context_ptr != NULL)
    { /* FULL native exists , proceed to writing to USIM */
        curr_security_context = *context_ptr;
     if( is_invalid )
     {
       MSG_HIGH_DS_0(MM_SUB, "=EMM= Setting KSI to NO_KEY_AVAIALABLE");
       curr_security_context.nasKSI.keyset_id = NO_KEY_AVAIALABLE;
     }

     if( memcmp((void *)&last_known_security_context, (void *)&curr_security_context, sizeof(emm_security_context_info)  ) == 0 ) 
     { 
       MSG_HIGH_DS_0(MM_SUB, "=EMM= security context same as last one, Do Not write to USIM");
     } /* security context same */
     else
     {   
       MSG_HIGH_DS_3(MM_SUB, "=EMM= security context NOT same as last one, Do Write to the USIM (KSI ID = %d), eps_storage_supported supported %d, update context in USIM(1) / EFS(0) =  %d ",
                     curr_security_context.nasKSI.keyset_id,emm_ctrl_data_ptr->eps_storage_supported,emm_ctrl_data_ptr->eps_storage_supported);
       if (emm_ctrl_data_ptr->eps_storage_supported)
       {
         emm_update_usim_eps_security_context(&curr_security_context);
       }
       else
       {
         if( emm_write_efs_security_context(&curr_security_context) == FALSE )
         {
           MSG_FATAL_DS(MM_SUB, "=EMM= Security context efs write Failed ", 0,0,0 );
		 }
       }
       last_known_security_context = curr_security_context;
     } /* else - security context not same */
   } /* FULL native exists */
    } /* current context */
#ifdef FEATURE_LTE_REL9
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Rel 9: Emergency services session, DO NOT save the EMM parameters to the USIM/EFS ");
  }
#endif
  
}


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
/*===========================================================================

FUNCTION    : EMM_PROCESS_EMM_INFO

DESCRIPTION
  
 Processes the EMM INFORMATION message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_process_emm_info(lte_nas_emm_info     *emm_info,
                          emm_ctrl_data_type   *emm_ctrl_data_ptr)
{
  sys_mm_information_s_type *info;

  mmr_mm_information_ind_s_type  reg_info_ind;

  ASSERT( emm_info != NULL );

  ASSERT( emm_ctrl_data_ptr != NULL );

  if(( emm_ctrl_data_ptr->detach_reason == SWITCH_OFF ) || ( emm_ctrl_data_ptr->detach_reason == NORMAL_DETACH ) )
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= EMM INFO received during DETACH Procedure -- message ignored");
#ifdef TEST_FRAMEWORK
    #error code not present
#endif /* TEST_FRAMEWORK */
    return;
  }
  memset(&reg_info_ind,0,sizeof(mmr_mm_information_ind_s_type));

  info = &reg_info_ind.info;

  MSG_HIGH_DS_5(MM_SUB, "=EMM= EMM_INFORMATION Network Full Name received = %d Network Short Name received = %d Local time zone received = %d Universal Time and Zone received = %d Day Light Savings Adjustement received = %d",
                          emm_info->nw_full_name_exists ,
                          emm_info->nw_partial_name_exists,
                          emm_info->local_time_zone_exists,
                          emm_info->tp_serv_center_time_stamp_exists,
                          emm_info->day_light_savings_exists);

  /* Copy the network full name */
  if( emm_info->nw_full_name_exists )
  {
    info->full_name_avail = TRUE;
    memset( (void *)info->full_name.name, 0, sizeof( info->full_name.name));
    if( emm_info->nw_full_name.length > MAX_NW_NAME_LEN )
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Network Full Name greater than MAX_NW_NAME_LEN ");
      emm_info->nw_full_name.length = (byte)MAX_NW_NAME_LEN;
    }
    memscpy((void*)&info->full_name.name[0], 
             sizeof(info->full_name.name),
             (void*)&emm_info->nw_full_name.val[0],
             emm_info->nw_full_name.length);

    info->full_name.length = emm_info->nw_full_name.length;
    switch (emm_info->nw_full_name.coding_scheme )
    {
    case GSM_DEFAULT_ALPHABET:
      info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET;
      break;
    case UCS_2:
      info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_UCS2;
      break;
    case RESERVED_CODING_SCHEME:
      info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_RESERVED;
      break;
    default:
      info->full_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_NONE;
      break;
    }
    info->full_name.add_country_initials = emm_info->nw_full_name.country_initial_req;
    
  }

  /* Copy the network short name */
  if( emm_info->nw_partial_name_exists )
  {
    info->short_name_avail = TRUE;
    memset( (void *)info->short_name.name, 0, sizeof( info->short_name.name));
    if( emm_info->nw_partial_name.length > MAX_NW_NAME_LEN )
    {
      MSG_ERROR_DS_0(MM_SUB, "=EMM= Network Full Name greater than MAX_NW_NAME_LEN ");
      emm_info->nw_partial_name.length = MAX_NW_NAME_LEN;
    }
    memscpy((void*)&info->short_name.name[0],
            sizeof(info->short_name.name),
            (void*)&emm_info->nw_partial_name.val[0],
            emm_info->nw_partial_name.length);

    info->short_name.length = emm_info->nw_partial_name.length;
    switch (emm_info->nw_partial_name.coding_scheme )
    {
    case GSM_DEFAULT_ALPHABET:
      info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET;
      break;
    case UCS_2:
      info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_UCS2;
      break;
    case RESERVED_CODING_SCHEME:
      info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_RESERVED;
      break;
    default:
      info->short_name.coding_scheme = SYS_NW_NAME_CODING_SCHEME_NONE;
      break;
    }
    info->short_name.add_country_initials = emm_info->nw_partial_name.country_initial_req;
  }

  /* Copy the local time zone */
  if( emm_info->local_time_zone_exists)
  {
    info->time_zone_avail = TRUE;
    info->time_zone = emm_info->local_time_zone;

  }

  /* Copy the Universal time and time zone */
  if( emm_info->tp_serv_center_time_stamp_exists )
  {
    info->univ_time_and_time_zone_avail = TRUE;
    info->univ_time_and_time_zone.year  = emm_info->tp_serv_center_time_stamp.year;
    info->univ_time_and_time_zone.month = emm_info->tp_serv_center_time_stamp.month;
    info->univ_time_and_time_zone.day   = emm_info->tp_serv_center_time_stamp.day;
    info->univ_time_and_time_zone.hour  = emm_info->tp_serv_center_time_stamp.hour;
    info->univ_time_and_time_zone.minute = emm_info->tp_serv_center_time_stamp.minute;
    info->univ_time_and_time_zone.second = emm_info->tp_serv_center_time_stamp.second;
    info->univ_time_and_time_zone.time_zone = emm_info->tp_serv_center_time_stamp.time_zone;
  }

  if( emm_info->day_light_savings_exists )
  {
    info->daylight_saving_adj_avail = TRUE;
    switch (emm_info->day_light_savings )
    {
    case NO_ADJUSTMENTS :
      info->daylight_saving_adj = SYS_DAY_SAV_ADJ_NO_ADJUSTMENT;
      break;
    case PLUS_ONE_HOUR:
      info->daylight_saving_adj = SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR;
      break;
    case PLUS_TWO_HOUR:
      info->daylight_saving_adj = SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR;
      break;
    default:
      info->daylight_saving_adj = SYS_DAY_SAV_ADJ_NONE;
      break;
    }
  }

  /* send the message to REG */

  reg_info_ind.message_header.message_set     = MS_MM_REG;
  reg_info_ind.message_header.message_id      = (int)MMR_MM_INFORMATION_IND;
  info->plmn_avail       = sys_plmn_id_is_valid ( emm_ctrl_data_ptr->plmn_service_state->plmn );
  info->plmn             = emm_ctrl_data_ptr->plmn_service_state->plmn;
  info->rat              = emm_ctrl_data_ptr->plmn_service_state->active_rat;

  PUT_IMH_LEN(sizeof (mmr_mm_information_ind_s_type) - sizeof (IMH_T),
              &reg_info_ind.message_header );

  mm_send_message(&reg_info_ind.message_header, GS_QUEUE_REG);

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Sent EMM_INFORMATION to REG");
  
}


/*===========================================================================

FUNCTION    : EMM_ALLOC_CONTEXT

DESCRIPTION
  Allocates an EPS security context of type CONTEXT_TYPE_PARTIAL_NATIVE_EPS or
  CONTEXT_TYPE_MAPPED_EPS.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
emm_security_context_info *emm_alloc_context(emm_ctrl_data_type *emm_ctrl_data_ptr, 
                                             lte_nas_context_type context_type)
{
  int index;
  emm_security_context_info *context_ptr = NULL;

  ASSERT(emm_ctrl_data_ptr != NULL);

  if((context_type != CONTEXT_TYPE_PARTIAL_NATIVE_EPS) &&
     (context_type != CONTEXT_TYPE_MAPPED_EPS))
  {
    MSG_ERROR_DS_1(MM_SUB,"=EMM= Unexpected context type=%d allocation requested. Context type should be of type CONTEXT_TYPE_PARTIAL_NATIVE_EPS or CONTEXT_TYPE_MAPPED_EPS",
                   context_type);
    return (emm_security_context_info *)context_ptr;
  }

  for( index = 0; index < EMM_MAX_CONTEXTS; index++ )
  {
    context_ptr = (emm_security_context_info *) (&emm_ctrl_data_ptr->context_info[index]);
    ASSERT( context_ptr != NULL );
  
    /* Delete existing mapped EPS security context or 
       partial native EPS security context */
    if((context_type == CONTEXT_TYPE_MAPPED_EPS) &&
       (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS))
    {
      MSG_HIGH_DS_2(MM_SUB,"=EMM= Deleting existing MAPPED context. KSI=%d, context state=%d",
                      context_ptr->nasKSI.keyset_id,context_ptr->context_state);
         emm_reset_security_info(context_ptr);
      }
    else if((context_type == CONTEXT_TYPE_PARTIAL_NATIVE_EPS) &&
            (context_ptr->context_type == CONTEXT_TYPE_PARTIAL_NATIVE_EPS))
    {
      MSG_HIGH_DS_1(MM_SUB,"=EMM= Deleting existing PARTIAL NATIVE context. KSI=%d",
                    context_ptr->nasKSI.keyset_id);
        emm_reset_security_info(context_ptr);
      }
  }

  context_ptr = NULL;
  
  /* Allocate requested EPS security context(partial native or mapped).
     Context state should be set explicitly to CONTEXT_STATE_CURRENT for
     Mapped context allocation: 
     a)After LTE_RRC_ACTIVATION_IND is received during inter-system handover
       from A/Gb mode to S1 mode or Iu mode to S1 mode
     b)After K'ASME is successfully generated during inter-system change in
       idle mode from A/Gb mode to S1 mode or Iu mode to S1 mode upon
       receiving Security Mode Command from the Network to create a Mapped EPS 
       security context */
  for( index = 0; index < EMM_MAX_CONTEXTS; index++ )
  {
    if( FALSE == emm_ctrl_data_ptr->context_info[index].in_use )
    {
      context_ptr =(emm_security_context_info *) (&emm_ctrl_data_ptr->context_info[index]);
      ASSERT(context_ptr != NULL);
      emm_reset_security_info(context_ptr);
      context_ptr->in_use = TRUE;
      context_ptr->context_type = context_type;
      MSG_HIGH_DS_2(MM_SUB,"=EMM= Allocated EPS security context of type = %d, index = %d.",
                    context_type,index);
      break;
        }
      }
        
  if(context_ptr == NULL)
  {
    MSG_ERROR_DS_1(MM_SUB,"=EMM= Failed to allocate EPS security context of type = %d.",
                   context_type);
  }
  
  return (emm_security_context_info *)context_ptr;
}/* end emm_alloc_context() */

/*===========================================================================

FUNCTION    : EMM_GET_CONTEXT_BY_TYPE

DESCRIPTION
  This function is used to retrieve the security context when an input of context type is given.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
emm_security_context_info *emm_get_context_by_type(emm_ctrl_data_type *emm_ctrl_data_ptr, 
                                                   lte_nas_context_type context_type)
{
 int index;

 ASSERT( emm_ctrl_data_ptr != NULL );

 for( index = 0; index < EMM_MAX_CONTEXTS; index++ )
 {
  if( ( context_type == emm_ctrl_data_ptr->context_info[index].context_type ) &&
      ( emm_ctrl_data_ptr->context_info[index].in_use ) )
  {
    return (emm_security_context_info *)(&emm_ctrl_data_ptr->context_info[index]);
  }
 }

 return NULL;

}

/*===========================================================================

FUNCTION    : EMM_UPDATE_CONTEXTS

DESCRIPTION
  This function is used to delete any existing non-current full native or mapped
  EPS security context if the current context is a full native EPS security context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_update_contexts
(
  emm_ctrl_data_type *emm_ctrl_data_ptr
)
{
 byte index = 0;

 emm_security_context_info *ctxt_ptr, *tmp_ptr;

 ASSERT( emm_ctrl_data_ptr != NULL );

 ctxt_ptr = emm_get_context_by_state( emm_ctrl_data_ptr, CONTEXT_STATE_CURRENT);

 ASSERT( ctxt_ptr != NULL );

 if(ctxt_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS)
 {
   for( index = 0; index < EMM_MAX_CONTEXTS; index++ )
   {
    tmp_ptr = emm_get_context_by_state( emm_ctrl_data_ptr, CONTEXT_STATE_NON_CURRENT);
     if((tmp_ptr != NULL) && 
        ((tmp_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS) ||
         (tmp_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS)))
      emm_reset_security_info( tmp_ptr );
   }
   }
} /* end emm_update_contexts() */
 
/*===========================================================================

FUNCTION : emm_get_ck_ik

DESCRIPTION
  This function is used for reading the Ck and IK keys

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_get_ck_ik
(
  byte *Ik,
  size_t Ik_buf_len,
  byte *Ck,
  size_t Ck_buf_len
)
{
  int i;

  memscpy((void*)Ck, Ck_buf_len,
          (void*)&emm_ck_key_value[0], sizeof(emm_ck_key_value));
  memscpy((void*)Ik, Ik_buf_len,
          (void*)&emm_ik_key_value[0], sizeof(emm_ik_key_value));

  /* print the keys */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= CK - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  emm_ck_key_value[i], emm_ck_key_value[i+1],
                  emm_ck_key_value[i+2], emm_ck_key_value[i+3],
                  emm_ck_key_value[i+4], emm_ck_key_value[i+5],
                  emm_ck_key_value[i+6], emm_ck_key_value[i+7]);
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= IK - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  emm_ik_key_value[i], emm_ik_key_value[i+1],
                  emm_ik_key_value[i+2], emm_ik_key_value[i+3],
                  emm_ik_key_value[i+4], emm_ik_key_value[i+5],
                  emm_ik_key_value[i+6], emm_ik_key_value[i+7]);
  }
}

/*===========================================================================

FUNCTION : emm_get_srvcc_handover_auth_params

DESCRIPTION
  This function is used for reading the PS SRVCC Ck and IK keys

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If stored SRVCC CK & IK are valid  
  FALSE: If stored SRVCC CK & IK are invalid (all 0xFF)

SIDE EFFECTS
  None

===========================================================================*/

boolean emm_get_srvcc_handover_auth_params
(
  byte *eKSI,
                                         auth_gprs_cipherkey_type    *Ck_srvcc,
  auth_gprs_integritykey_type *Ik_srvcc 
)
{
  byte invalid_ck_ik[UMTS_SECURITY_KEY_LENGTH];

  memset((void*)&invalid_ck_ik[0], 0xFF, sizeof(invalid_ck_ik));

  *Ck_srvcc = emm_srvcc_ck_key_value;
  *Ik_srvcc = emm_srvcc_ik_key_value;
  *eKSI = emm_srvcc_eKSI;

  if((memcmp(&invalid_ck_ik[0], &Ck_srvcc->value_data[0],
                                  sizeof(invalid_ck_ik)) == 0) &&
     (memcmp(&invalid_ck_ik[0], &Ik_srvcc->value_data[0],
                                  sizeof(invalid_ck_ik)) == 0))
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Stored SRVCC CK & IK are invalid.");
    return FALSE;
  }

  return TRUE;
} /* emm_get_srvcc_ck_ik */


/*===========================================================================

FUNCTION : emm_compute_ck_ik_from_kasme

DESCRIPTION
  This function is used for mapping of EPS security context to UMTS security context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_compute_ck_ik_from_kasme( emm_ctrl_data_type *emm_ctrl_data_ptr, 
                                      emm_security_context_info *context_ptr,
                                      auth_gprs_integritykey_type *Ik,
                                      auth_gprs_cipherkey_type    *Ck, 
                                      boolean use_dl_count )
 {
   /* 
       FC = 0x16
       P0 = DownLink NAS count
       L0 = Length of downlink NAS count (i.e. 0x00 0x04)

   Key = Kasme
  */
  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[7];

  byte input_str_len = 0;

  byte output_str[32];
  byte i = 0;

  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT( context_ptr != NULL );

  ASSERT( Ik != NULL );
  
  ASSERT ( Ck != NULL );

  memset(input_str, 0, sizeof(input_str));

  memset(output_str, 0, sizeof(output_str));

  if(use_dl_count == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Use DL NAS count for CK, IK generation from KASME ");
    /* Use DL NAS count */
    input_str[0]  = 0x16;  // FC (1 byte ) A.8 in 33.401, page 62 
    input_str[1]  = 0;     // context_ptr->nas_ul_cnt[3] = 0
    input_str[2]  = context_ptr->nas_dl_cnt[2];  // overflow cnt
    input_str[3]  = context_ptr->nas_dl_cnt[1];  // overflow cnt
    input_str[4]  = context_ptr->nas_dl_cnt[0];  // SQN
    // DL NAS COUNT ( 4 byte )
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=EMM= Use UL NAS count for CK, IK generation from KASME ");
    input_str[0]  = 0x1B;  // FC (1 byte )
    input_str[1]  = 0;     // context_ptr->nas_ul_cnt[3] = 0
    input_str[2]  = context_ptr->nas_ul_cnt[2];  // overflow cnt
    input_str[3]  = context_ptr->nas_ul_cnt[1];  // overflow cnt
    input_str[4]  = context_ptr->nas_ul_cnt[0];  // SQN
    // UL NAS COUNT ( 4 byte )
  }


  input_str[5] =  0x00;

  input_str[6] =  0x04; // length of NAS DL COUNT (2 bytes )

  input_str_len = sizeof(input_str);

  nas_errno = lte_security_generate_key( context_ptr->nas_kasme,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,     /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
    return FALSE;
  
  /* output string is the CK' || IK' */
  Ck->value_length = memscpy((void*)&Ck->value_data[0], sizeof(Ck->value_data),
                             (void*)&output_str[0], sizeof(Ck->value_data));

  Ik->value_length = memscpy((void*)&Ik->value_data[0], sizeof(Ik->value_data),
                             (void*)&output_str[16], sizeof(Ik->value_data));

  memscpy((void*)&emm_ck_key_value[0], sizeof(emm_ck_key_value),
          (void*)&output_str[0], sizeof(emm_ck_key_value));

  memscpy((void*)&emm_ik_key_value[0], sizeof(emm_ik_key_value),
          (void*)&output_str[16], sizeof(emm_ik_key_value));
  
  /* print the CK' & IK' keys */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Computed CK' from KASME - including index[0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  emm_ck_key_value[i], emm_ck_key_value[i+1],
                  emm_ck_key_value[i+2], emm_ck_key_value[i+3],
                  emm_ck_key_value[i+4], emm_ck_key_value[i+5],
                  emm_ck_key_value[i+6], emm_ck_key_value[i+7]);
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Computed IK' from KASME - including index[0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  emm_ik_key_value[i], emm_ik_key_value[i+1],
                  emm_ik_key_value[i+2], emm_ik_key_value[i+3],
                  emm_ik_key_value[i+4], emm_ik_key_value[i+5],
                  emm_ik_key_value[i+6], emm_ik_key_value[i+7]);
  }

  return TRUE;
}

/*===========================================================================

FUNCTION : EMM_COMPUTE_SRVCC_CK_IK_FROM_KASME

DESCRIPTION
     Spec 33.401, A.12 KASME to CKSRVCC, IKSRVCC derivation
           This input string is used when there is a need to derive CKSRVCC|| IKSRVCC used in CS domain from KASME during
           mapping of security contexts between E-UTRAN and GERAN/UTRAN. KASME is a 256-bit element, and so is the
           concatenation of CKSRVCC and IKSRVCC (which are 128 bits each).
           - FC = 0x1A
           - P0 = NAS downlink COUNT value
           - L0 = length of NAS downlink COUNT value (i.e. 0x00 0x04)
           The input key shall be KASME.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_compute_srvcc_ck_ik_from_kasme( emm_ctrl_data_type *emm_ctrl_data_ptr, 
                                      emm_security_context_info *context_ptr,
                                      auth_gprs_integritykey_type *Ik_srvcc,
                                      auth_gprs_cipherkey_type    *Ck_srvcc)
{
   /* 
       FC = 0x1A
       P0 = DownLink NAS count
       L0 = Length of downlink NAS count (i.e. 0x00 0x04)

   Key = Kasme
  */
  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[7];

  byte input_str_len = 0;

  byte output_str[32];
  byte i = 0;

  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT( context_ptr != NULL );

  ASSERT( Ik_srvcc != NULL );
  
  ASSERT ( Ck_srvcc != NULL );

  memset(input_str, 0, sizeof(input_str));

  memset(output_str, 0, sizeof(output_str));

  MSG_HIGH_DS_0(MM_SUB, "=EMM= Use DL NAS count for CK, IK generation from KASME ");
  /* Use DL NAS count */
  input_str[0]  = 0x1A;  // FC (1 byte ) A.12 in 33.401, page 64 
  input_str[1]  = 0;     // context_ptr->nas_ul_cnt[3] = 0
  input_str[2]  = context_ptr->nas_dl_cnt[2];  // overflow cnt
  input_str[3]  = context_ptr->nas_dl_cnt[1];  // overflow cnt
  input_str[4]  = context_ptr->nas_dl_cnt[0];  // SQN
  // DL NAS COUNT ( 4 byte )
  

  input_str[5] =  0x00;
  input_str[6] =  0x04; // length of NAS DL COUNT (2 bytes )
  input_str_len = sizeof(input_str);

  nas_errno = lte_security_generate_key( context_ptr->nas_kasme,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,     /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
    return FALSE;
  
  /* output string is the CK' || IK' */
  Ck_srvcc->value_length = memscpy((void*)&Ck_srvcc->value_data[0], 
                                    sizeof(Ck_srvcc->value_data),
                                    (void*)&output_str[0], 
                                    sizeof(Ck_srvcc->value_data));

  Ik_srvcc->value_length = memscpy((void*)&Ik_srvcc->value_data[0],
                                   sizeof(Ik_srvcc->value_data),
                                   (void*)&output_str[16], 
                                   sizeof(Ik_srvcc->value_data));

  emm_srvcc_ck_key_value.value_length = memscpy((void*)&emm_srvcc_ck_key_value.value_data[0],
                                                sizeof(emm_srvcc_ck_key_value.value_data),
                                                (void*)&output_str[0],
                                                sizeof(emm_srvcc_ck_key_value.value_data));

  emm_srvcc_ik_key_value.value_length = memscpy((void*)&emm_srvcc_ik_key_value.value_data[0],
                                                sizeof(emm_srvcc_ik_key_value.value_data),
                                                (void*)&output_str[16],
                                                sizeof(emm_srvcc_ik_key_value.value_data));

  /* save the eKSI of this context from the current context */
  emm_srvcc_eKSI = context_ptr->nasKSI.keyset_id;

  MSG_HIGH_DS_1(MM_SUB, "=EMM= SRVCC eKSI = %d , SRVCC CK - including index [0-15]", emm_srvcc_eKSI);

  /* print the CK SRVCC & IK SRVCC keys */
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  emm_srvcc_ck_key_value.value_data[i], emm_srvcc_ck_key_value.value_data[i+1],
                  emm_srvcc_ck_key_value.value_data[i+2], emm_srvcc_ck_key_value.value_data[i+3],
                  emm_srvcc_ck_key_value.value_data[i+4], emm_srvcc_ck_key_value.value_data[i+5],
                  emm_srvcc_ck_key_value.value_data[i+6], emm_srvcc_ck_key_value.value_data[i+7]);
  }

  MSG_HIGH_DS_0(MM_SUB, "=EMM= SRVCC IK - including index [0-15]");
  for(i = 0; i+7 < MAX_AUTH_DATA_LENGTH; i = i+8)
  {
    MSG_HIGH_DS_8(MM_SUB,"=EMM= 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                  emm_srvcc_ik_key_value.value_data[i], emm_srvcc_ik_key_value.value_data[i+1],
                  emm_srvcc_ik_key_value.value_data[i+2], emm_srvcc_ik_key_value.value_data[i+3],
                  emm_srvcc_ik_key_value.value_data[i+4], emm_srvcc_ik_key_value.value_data[i+5],
                  emm_srvcc_ik_key_value.value_data[i+6], emm_srvcc_ik_key_value.value_data[i+7]);
  }
  
  return TRUE;
} /* emm_compute_srvcc_ck_ik_from_kasme */

/*===========================================================================

FUNCTION EMM_COMPUTE_NAS_TOKEN

DESCRIPTION
  This function derives the NAS token for inter-RAT mobility.
  Spec 33.401, section A.9

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean emm_compute_nas_token
(
  emm_ctrl_data_type *emm_ctrl_data_ptr,
  byte *nas_token,
  size_t nas_token_buf_len, 
  emm_security_context_info *context_ptr
)
 {
   /* 
       FC = 0x17
       P0 = DownLink NAS count
       L0 = Length of downlink NAS count (i.e. 0x00 0x04)

   Key = Kasme
  */
  lte_sec_errno_e nas_errno = E_FAILURE;

  byte input_str[7];

  byte input_str_len = 0;

  byte output_str[32];

  byte sqn;
  word over_flow_cnt;

  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT( context_ptr != NULL );

  ASSERT( nas_token != NULL );

  memset(input_str, 0, sizeof(input_str));

  memset(output_str, 0, sizeof(output_str));

  input_str[0]  = 0x17;  // FC (1 byte )

  /* section 4.4.3.1 of TS 24.301
     Increment the UL count for token computation */
  sqn = context_ptr->nas_ul_cnt[0];
  sqn++;
  over_flow_cnt = ( (word )context_ptr->nas_ul_cnt[2] << 8) | context_ptr->nas_ul_cnt[1];
  if ( sqn == 0) 
  {
    over_flow_cnt++ ;
  }
  context_ptr->nas_ul_cnt[0] = sqn;
  context_ptr->nas_ul_cnt[1] = (byte )(over_flow_cnt & 0xFF);
  context_ptr->nas_ul_cnt[2] = (byte )((over_flow_cnt & 0xFF00) >> 8);
  /* increment the over flow count if SQN wraps around */
  input_str[1] = 0;                           //context_ptr->nas_ul_cnt[3] = must be set to 0
  input_str[2] = context_ptr->nas_ul_cnt[2];  // overflow cnt
  input_str[3] = context_ptr->nas_ul_cnt[1];  // overflow cnt
  input_str[4] = context_ptr->nas_ul_cnt[0];  // sqn

  input_str[5] =  0x00;

  input_str[6] =  0x04; // length of NAS DL COUNT (2 bytes )

  input_str_len = sizeof(input_str);

  MSG_HIGH_DS_4(MM_SUB, "=EMM= NAS UL count used for NAS Token generation -> nas_ul_cnt[0] = %d nas_ul_cnt[1] = %d nas_ul_cnt[2] = %d nas_ul_cnt[3] = %d  ",
                           context_ptr->nas_ul_cnt[0],
                           context_ptr->nas_ul_cnt[1],
                           context_ptr->nas_ul_cnt[2],
                           context_ptr->nas_ul_cnt[3]);

  nas_errno = lte_security_generate_key( context_ptr->nas_kasme,                /* Input key*/
                                     (uint8 *)input_str, /* Input string ptr */
                                     &input_str_len,     /* Input string length*/
                                     (uint8 *)output_str); /* Output string */

  if( nas_errno != E_SUCCESS)
    return FALSE;
  
  memscpy((void*)nas_token, nas_token_buf_len,
          (void*)&output_str[30], sizeof(output_str) - 30);

  MSG_HIGH_DS_2(MM_SUB, "=EMM= Generated NAS Token - nas_token[0] = %d nas_token[1] = %d ",output_str[30],output_str[31]);

  return TRUE;
}

/*===========================================================================

FUNCTION EMM_GET_EPS_PARAMS

DESCRIPTION
  This function retrieves the EPS parameters - computes NAS token, CK/IK. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_get_eps_params( mm_gw_identity_param_type *mm_identity)
{

  byte  nas_token[2];

  emm_ctrl_data_type *emm_ctrl_data_ptr;
 
  emm_security_context_info *context_ptr;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT ( mm_identity != NULL );

  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );

  ASSERT( context_ptr != NULL );

  if((context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS ) || (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS ))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= context_type = %d, Compute NAS token now...",
                  context_ptr->context_type);
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected current context_type = %d",
                   context_ptr->context_type);
    return;
  }

  if( emm_compute_nas_token( emm_ctrl_data_ptr,
                        &nas_token[0], sizeof(nas_token),
                        context_ptr) == FALSE )
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= NAS Token Computation Failed ", 0,0,0 );
  }

  MSG_HIGH_DS_2(MM_SUB,"=EMM= nas_token[0] = %d, nas_token[1] = %d Compute ck_ik_from_kasme now...",
              nas_token[0],nas_token[1]);
  
  if( emm_compute_ck_ik_from_kasme( emm_ctrl_data_ptr, context_ptr, &mm_identity->Ik, &mm_identity->Ck, FALSE ) == FALSE)
  {
    MSG_FATAL_DS(MM_SUB, "=EMM= KASME from CK IK computation Failed ", 0,0,0 );
  }

  /* update the ksi with the nasKSI from EPS context */
  mm_identity->eKSI = context_ptr->nasKSI.keyset_id;

  /* P-TMSI signature */
  memscpy((void*)&mm_identity->ptmsi_signature.value[1],
          sizeof(mm_identity->ptmsi_signature.value) - 1,
          (void*)&nas_token[0], sizeof(nas_token));
}

/*===========================================================================
FUNCTION EMM_GET_EPS_HANDOVER_AUTH_PARAMS

DESCRIPTION
  This function retrieves the EPS parameters during handover - CK/IK. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void emm_get_eps_handover_auth_params( mm_gw_identity_param_type *mm_identity)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr;
  emm_security_context_info *context_ptr;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT( emm_ctrl_data_ptr != NULL );

  ASSERT ( mm_identity != NULL );

  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );
  ASSERT( context_ptr != NULL );

  if((context_ptr->context_type == CONTEXT_TYPE_FULL_NATIVE_EPS ) || (context_ptr->context_type == CONTEXT_TYPE_MAPPED_EPS ))
  {
    MSG_HIGH_DS_1(MM_SUB, "=EMM= context_type = %d, Get the stored CK/IK now",
                  context_ptr->context_type);
  }
  else
  {
    MSG_ERROR_DS_1(MM_SUB, "=EMM= Unexpected current context_type = %d",
                   context_ptr->context_type);
    return;
  }
  
  /* Get the CK, IK that was sent to RRC in UMTS_KEY_RESPONSE */
  emm_get_ck_ik(&mm_identity->Ik.value_data[0], sizeof(mm_identity->Ik.value_data),
                &mm_identity->Ck.value_data[0], sizeof(mm_identity->Ck.value_data));
  mm_identity->Ik.value_length = UMTS_SECURITY_KEY_LENGTH;
  mm_identity->Ck.value_length = UMTS_SECURITY_KEY_LENGTH;

  /* update the ksi with the nasKSI from EPS context */
  mm_identity->eKSI = context_ptr->nasKSI.keyset_id;

  /* P-TMSI signature */
  memset((void *)&mm_identity->ptmsi_signature.value[1], 0xFF, sizeof(word));

} /* emm_get_eps_handover_auth_params */


/*===========================================================================
FUNCTION EMM_VALIDATE_SMC_NAS_SECURITY_ALGOS

DESCRIPTION
  This function validates the NAS Security Algorithms received in SMC command. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  When indicated Integrity protection & Ciphering algorithm is 
         supported by UE
  FALSE: When indicated Integrity protection or Ciphering algorithm is not
         supported by UE

SIDE EFFECTS
  None

===========================================================================*/
static boolean emm_validate_smc_nas_security_algos
(
  lte_nas_emm_security_mode_command  *sec_cmd_ptr,
  emm_ctrl_data_type                 *emm_ctrl_data_ptr
)
{
  boolean result = TRUE;
  lte_nas_emm_integrity_protect_alg_type   integrity_algo;
  lte_nas_emm_ciphering_alg_type           ciphering_algo;
  lte_nas_emm_ue_security_type             ue_security_cap;

  ASSERT( sec_cmd_ptr != NULL );
  ASSERT( emm_ctrl_data_ptr != NULL );
  
  ue_security_cap = emm_ctrl_data_ptr->ue_security_capability;
  integrity_algo = sec_cmd_ptr->nas_security_alg.integrity_protect_alg;
  ciphering_algo = sec_cmd_ptr->nas_security_alg.ciphering_alg;
  
  switch(integrity_algo)
  {
    case INTEGRITY_ALGO_128_EIA0:
#ifdef FEATURE_LTE_REL9
      if(emm_ctrl_data_ptr->emc_srv_status != LTE_NAS_EMC_SRV_NONE)
      {
        if((sec_cmd_ptr->nas_key_set_identifier_asme.keyset_id == 0) &&
           (ciphering_algo == ENCRYPTION_ALGO_128_EEA0))
        {
          MSG_HIGH_DS_1(MM_SUB,"=EMM= Allow null algos for EMC %d",
                        emm_ctrl_data_ptr->emc_srv_status);
          return result;
        }
        else
        {
          MSG_HIGH_DS_1(MM_SUB,"=EMM= Null algos not allowed for EMC %d",
                    emm_ctrl_data_ptr->emc_srv_status);
          return FALSE;
        }  
      }
      else 
#endif
      if(ue_security_cap.spare_bit == FALSE)
      {
        result = FALSE;
      }
      break;
    case INTEGRITY_ALGO_128_EIA1:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia1_supported)
         result = FALSE;
      break;
    case INTEGRITY_ALGO_128_EIA2:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia2_supported)
         result = FALSE;
      break;
    case INTEGRITY_ALGO_EIA3:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia3_supported)
         result = FALSE;
      break;
    case INTEGRITY_ALGO_EIA4:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia4_supported)
         result = FALSE;
      break;
    case INTEGRITY_ALGO_EIA5:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia5_supported)
         result = FALSE;
      break;
    case INTEGRITY_ALGO_EIA6:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia6_supported)
         result = FALSE;
      break;
    case INTEGRITY_ALGO_EIA7:
       if(FALSE == ue_security_cap.eps_integrity_alg_128_eia7_supported)
         result = FALSE;
      break;
    default:
       MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid Integrity protection algo recieved in SMC.");
       result = FALSE;
      break;
  }

  if(FALSE == result)
  {
     MSG_HIGH_DS_1(MM_SUB,"=EMM= UE received Integrity algo = %d but doesn't support it.",
                   integrity_algo);
    return result;
  }

  switch(ciphering_algo)
  {
    case ENCRYPTION_ALGO_128_EEA0:
       if(FALSE == ue_security_cap.eps_encrypt_alg_128_eea0_supported)
         result = FALSE;
      break;
    case ENCRYPTION_ALGO_128_EEA1:
       if(FALSE == ue_security_cap.eps_encrypt_alg_128_eea1_supported)
         result = FALSE;
      break;
    case ENCRYPTION_ALGO_128_EEA2:
       if(FALSE == ue_security_cap.eps_encrypt_alg_128_eea2_supported)
         result = FALSE;
      break;
    case ENCRYPTION_ALG_EEA3:
       if(FALSE == ue_security_cap.eps_encrypt_alg_eea3_supported)
         result = FALSE;
      break;
    case ENCRYPTION_ALG_EEA4:
      if(FALSE == ue_security_cap.eps_encrypt_alg_eea4_supported)
        result = FALSE;
      break;
    case ENCRYPTION_ALG_EEA5:
       if(FALSE == ue_security_cap.eps_encrypt_alg_eea5_supported)
         result = FALSE;
      break;
    case ENCRYPTION_ALG_EEA6:
       if(FALSE == ue_security_cap.eps_encrypt_alg_eea6_supported)
         result = FALSE;
      break;
    case ENCRYPTION_ALG_EEA7:
       if(FALSE == ue_security_cap.eps_encrypt_alg_eea7_supported)
         result = FALSE;
      break;
    default:
       MSG_ERROR_DS_0(MM_SUB, "=EMM= Invalid Ciphering algo recieved in SMC.");
         result = FALSE;
      break;
  } /* end  switch(ciphering_algo) */
  
  if(FALSE == result)
  {
    MSG_HIGH_DS_1(MM_SUB,"=EMM= UE received Ciphering algo = %d but doesn't support it.",
              ciphering_algo);
  }
                    
  return result;
}

/*===========================================================================
FUNCTION EMM_INVALIDATE_SRVCC_CK_IK

DESCRIPTION
  This function invalidates the stored SRVCC CK and IK. 

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void emm_invalidate_srvcc_ck_ik(void)
{
  /* Set SRVCC CK IK to invalid (0xFF) values.
     Valid values will be populated when NAS receives
     RRC_NAS_UMTS_KEY_IND/RRC_NAS_GSM_KEY_IND during L2W PS HO */
  MSG_HIGH_DS_0(MM_SUB, "=EMM= Invalidating SRVCC CK & IK");
  memset((void*)&emm_srvcc_ck_key_value.value_data[0], 0xFF, MAX_AUTH_DATA_LENGTH);
  emm_srvcc_ck_key_value.value_length = UMTS_SECURITY_KEY_LENGTH;
  memset((void*)&emm_srvcc_ik_key_value.value_data[0], 0xFF, MAX_AUTH_DATA_LENGTH);
  emm_srvcc_ik_key_value.value_length = UMTS_SECURITY_KEY_LENGTH;
}

/*===========================================================================
FUNCTION emm_get_ul_count

DESCRIPTION This function will be used by the mm module to get the current uplink
            count resides inside the emm global datastructure emm_ctrl_data_type

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

boolean emm_get_ul_count(byte *ul_count)
{
  emm_ctrl_data_type *emm_ctrl_data_ptr;
  emm_security_context_info *context_ptr;
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  ASSERT(ul_count!=NULL);
  ASSERT( emm_ctrl_data_ptr != NULL ); 
  context_ptr = emm_get_context_by_state( emm_ctrl_data_ptr,CONTEXT_STATE_CURRENT );
  if(context_ptr==NULL)
  {
    return FALSE;
  }
  else
  {
    memscpy(ul_count,sizeof(context_ptr->nas_ul_cnt),context_ptr->nas_ul_cnt,sizeof(context_ptr->nas_ul_cnt)); 
  }  
  return TRUE;
}

/*==================================================================================
FUNCTION EMM_STOP_AUTH_TIMERS

DESCRIPTION
  This function stops the MAK(TIMER_T3418) & SQN(TIMER_T3420) timers if running.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If MAK and/or SQN timer(s) were running and stopped
  FALSE: If MAK and/or SQN timer(s) are not running
  
SIDE EFFECTS
  None

===================================================================================*/
boolean emm_stop_auth_timers
(  
  emm_ctrl_data_type   *emm_ctrl_data_ptr
)
{
  boolean timer_stopped = FALSE;

  ASSERT(emm_ctrl_data_ptr != NULL);

  if(mm_timer_status[TIMER_T3418] == TIMER_ACTIVE)
  {
    mm_stop_timer(TIMER_T3418);
    timer_stopped = TRUE;
  }

  if(mm_timer_status[TIMER_T3420] == TIMER_ACTIVE)
  {
    mm_stop_timer(TIMER_T3420);
    timer_stopped = TRUE;
  }

  return timer_stopped;
}

/*==================================================================================
FUNCTION EMM_DEACT_RETX_TIMER

DESCRIPTION
  This function instructs MM AUTH server to deactivate a particular EMM procedure timer
  (basically Attach(T3410), TAU(T3430), Service Request(T3417) or Detach(T3421)) if it was running
  and stopped during Authentication failure. This function is invoked when Emergency sesion
  is active and an EMM procedure completes. This will prevent the EMM procedure timer to be
  restarted upon Authentication timer (MAC/SQN)expiry.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===================================================================================*/
void emm_deact_retx_timer
(
  timer_id_T   timer_id
)
{
  if(auth_transaction_id != ((client_id)NULL))
  {
    auth_deact_retx_timer(auth_transaction_id, timer_id);
  }
}
/*===========================================================================

FUNCTION    EMM_COMPARE_AUTH_DATA

DESCRIPTION
   This function compares the authentication data sent by the NW to the
   cached data.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the two data packages are different, TRUE otherwise

SIDE EFFECTS

===========================================================================*/
boolean emm_compare_auth_data(byte nasksi, auth_rand_type rand_value, auth_autn_type autn)  
{

    if ((rand_value_cache.value_length == rand_value.value_length) && 
        (memcmp( &rand_value_cache.value_data , &rand_value.value_data, rand_value_cache.value_length) == 0) &&
        (autn_cache.value_length == autn.value_length) &&
        (memcmp( &autn_cache.value_data , &autn.value_data, autn_cache.value_length) == 0) &&
        (nasksi_cache == nasksi))
    {
        return TRUE;
    }
    MSG_HIGH_DS_0(MM_SUB, "=MM= Different Auth Req Sent by NW") ;
    return FALSE;
}
/*===========================================================================

FUNCTION    EMM_STORE_AUTH_DATA

DESCRIPTION
   This function stores the auth data passed in from the NW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void emm_store_auth_data(byte  nasksi,
                auth_rand_type rand_value,
                auth_autn_type autn_value)
{
    nasksi_cache = nasksi;

    rand_value_cache.value_length= rand_value.value_length;
    memset((void *)&rand_value_cache.value_data[0], 0, MAX_AUTH_DATA_LENGTH);
    memscpy((void *)&rand_value_cache.value_data[0], rand_value.value_length, 
        (void *)&rand_value.value_data[0], rand_value.value_length);

    autn_cache.value_length = autn_value.value_length;
    memset((void *)&autn_cache.value_data[0], 0, MAX_AUTH_DATA_LENGTH);
    memscpy((void *)&autn_cache.value_data[0], autn_value.value_length, 
        (void *)&autn_value.value_data[0], autn_value.value_length);
}
#endif // FEATURE_LTE
