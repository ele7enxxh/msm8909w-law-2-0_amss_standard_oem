/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D
                        
                        U T I L I T Y   S E R V I C E S

GENERAL DESCRIPTION
  This module contains utility functions that are used in executing/processing
  AT commands.

EXTERNALIZED FUNCTIONS
  dsatutil_strncmp_ig_sp_case
    This function compares two strings for <n> characters, ignoring case.
    Lower case letters are converted to upper case before the 
    difference is taken.

  dsatutil_itoa
    This function converts an integer to ASCII, using a specified
    radix. The resulting string is null terminated.

  dsatutil_atoi
    This function converts an ASCII string to an integer, using a specified
    radix.  Spaces are ignored. Letters used for radices above 10 may be of
    either case.

  dsatutil_strcmp_ig_sp_case
    This function compares two null terminated strings, ignoring case.

  dsatutil_reset_password
    Clear the password variable or specified length.
    The password string is set to 0xFF, null terminated.  
    
  dsatutil_convert_tuple
    This function checks for the validity of known tuple formats and converts
    between string and number representations.  For IPv6 addresses, the number
    pointer is assumed to be an array of uint32 values.

  dsatutil_init_timers_ex
    This function registers timers with the opeating system.

  dsatutil_dispatch_timer_handler
    This function dispatches the common timer expiry event to the
    correct timer handler.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatutil_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     -----------------------------------------------------------------
12/10/15   skc     Removing usage of enum TUPLE_TYPE_IPv4_254
11/24/14   pg      Fixed ws46 featurization issue.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#include "dstaski.h"
#include "nv.h"
#include "dsati.h"
#include "err.h"
#include "msg.h"
#include "amssassert.h"
#include "ps_utils.h"
#include <string.h>
#include <stdio.h>
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatvoice.h"
#include "ds3gmgr.h"

#include "dsatact.h"

#ifdef FEATURE_DATA_IS707 
#include  "dsat707extctab.h"
#include  "dsat707faxctab.h"
#include  "dsat707hdrctab.h"
#include  "dsat707mipctab.h"
#include  "dsat707pstatsctab.h"
#include  "dsat707smsctab.h"
#include  "dsat707vendctab.h"
#endif /* FEATURE_DATA_IS707   */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#include "dsat707mip.h"

#include "fs_public.h"
#include "fs_sys_types.h"
#include "fs_errno.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
  These macros used to be in dsumtspdpreg.h but had to removed from there 
  due to compilation issues seen with pdp reg rpc stub code.
-------------------------------------------------------------------------*/
#define pdp_addr_ipv4 pdp_addr.pdp_addr_ipv4
#define pdp_addr_ipv6 pdp_addr.pdp_addr_ipv6

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/
/* To get whether the originated call is in dialing or alerting state. */
boolean dsat_mo_dailing_state[DSAT_SYS_MODEM_AS_ID_MAX];

/* Local Variable */
LOCAL dsat_string_item_type  csmp_fo_val[DSAT_SYS_MODEM_AS_ID_MAX][CSMP_FO_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  csmp_vp_val[DSAT_SYS_MODEM_AS_ID_MAX][CSMP_VP_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  qcpdpp_passw_val[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+4];
LOCAL dsat_string_item_type  qcpdpp_uname_val[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+4];
LOCAL dsat_string_item_type  cusd_str_val[CUSD_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cusd_dcs_val[CUSD_DCS_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cgans_l2p_type_val[8];
LOCAL dsat_string_item_type  cpin_pin_val[DSAT_SYS_MODEM_AS_ID_MAX][MAX_PIN_STRING_LEN+1];
LOCAL dsat_string_item_type  cpin_newpin_val[DSAT_SYS_MODEM_AS_ID_MAX][MAX_PIN_STRING_LEN+1];
LOCAL dsat_string_item_type  cops_oper_val[DSAT_SYS_MODEM_AS_ID_MAX][COPS_OPER_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cops_csg_id_val[DSAT_SYS_MODEM_AS_ID_MAX][COPS_CSG_ID_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cpol_oper_val[DSAT_SYS_MODEM_AS_ID_MAX][CPOL_OPER_STR_MAX_LEN+1];
/*  CGDCONT  */
LOCAL dsat_string_item_type  cgdcont_pdptype_val[8];
LOCAL dsat_string_item_type  cgdcont_apn_val[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+3];
LOCAL dsat_string_item_type  cgdcont_pdpaddr_val[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+3];
/*  Value items: "+3" to allow for quotes and NULL  */
LOCAL dsat_string_item_type  cgtft_addr_subnet_val[MAX_ADDR_SUBNET_STRING_LEN+3];
LOCAL dsat_string_item_type  cgtft_src_port_val[MAX_PORT_STRING_LEN+3];
LOCAL dsat_string_item_type  cgtft_dst_port_val[MAX_PORT_STRING_LEN+3];
LOCAL dsat_string_item_type  cgtft_tos_tclass_val[MAX_TOS_TCLASS_STRING_LEN+3];
LOCAL dsat_string_item_type  cgeqreq_sdu_err_val[MAX_QOSERR_STRING_LEN+1];
LOCAL dsat_string_item_type  cgeqreq_res_biterr_val[MAX_QOSERR_STRING_LEN+1];
LOCAL dsat_string_item_type  cgeqmin_sdu_err_val[MAX_QOSERR_STRING_LEN+1];
LOCAL dsat_string_item_type  cgeqmin_res_biterr_val[MAX_QOSERR_STRING_LEN+1];
LOCAL dsat_string_item_type  cscb_mids_val[CSCB_MIDS_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  cscb_dcss_val[CSCB_DCSS_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_es_orig_rqst_val[ES_ORGI_RQST_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_es_orig_fbk_val[ES_ORIG_FBK_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_es_ans_fbk_val[ES_ANS_FBK_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_trans_idle_val[ESA_FRAMED_IDLE_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_framed_un_ov_val[ESA_FRAMED_UN_OV_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_hd_auto_val[ESA_HD_AUTO_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_esa_syn2_val[ESA_SYN2_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_csca_sca_val[DSAT_SYS_MODEM_AS_ID_MAX][CSCA_SCA_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  dsat_csca_tosca_val[DSAT_SYS_MODEM_AS_ID_MAX][CSCA_TOSCA_STR_MAX_LEN+1];
#ifdef FEATURE_DSAT_CDMA_PIN 
LOCAL dsat_string_item_type  dsat707_cpin_pin_val[MAX_PIN_STRING_LEN+1];
LOCAL dsat_string_item_type  dsat707_cpin_newpin_val[MAX_PIN_STRING_LEN+1];
#endif /* FEATURE_DSAT_CDMA_PIN */
#ifdef FEATURE_CDMA_SMS
LOCAL dsat_string_item_type  qcsmp_vp_val[QCSMP_VP_STR_MAX_LEN+1];
LOCAL dsat_string_item_type  qcsmp_tdd_val[QCSMP_DDT_STR_MAX_LEN+1];
#endif/* FEATURE_CDMA_SMS*/
#if defined(FEATURE_DSAT_SP_CMDS)
LOCAL dsat_string_item_type  spc_val[NV_SEC_CODE_SIZE + 1];
LOCAL dsat_string_item_type  unlock_val[UNLOCK_STRING_LEN + 1];
LOCAL dsat_string_item_type  lock_val[NV_LOCK_CODE_SIZE + 1];
LOCAL dsat_string_item_type  old_lock_val[LOCK_OLD_STRING_LEN + 1];
LOCAL dsat_string_item_type  new_lock_val[NV_LOCK_CODE_SIZE + 1];
#endif /*defined(FEATURE_DSAT_SP_CMDS)*/

/*Command table Val structure pointers*/
dsat_basic_table_ss_val_struct_type            dsat_basic_vals;
dsat_basic_action_table_ss_val_struct_type     dsat_basic_act_vals;
dsat_basic_sreg_table_ss_val_struct_type       dsat_basic_sreg_vals;
dsat_basic_ext_table_ss_val_struct_type        dsat_basic_ext_vals;
dsat_basic_vend_table_ss_val_struct_type       dsat_basic_vend_ss_vals;
dsat_basic_vend_table_ms_val_struct_type       dsat_basic_vend_ms_vals[DSAT_MS_MAX_SUBS];
dsat_etsi_ext_act_table_ss_val_struct_type     dsat_etsi_ext_act_ss_vals;
dsat_etsi_ext_act_table_ms_val_struct_type     dsat_etsi_ext_act_ms_vals[DSAT_MS_MAX_SUBS];
dsat_etsi_ext_table_ss_val_struct_type         dsat_etsi_ext_ss_vals;
dsat_etsi_ext_table_ms_val_struct_type         dsat_etsi_ext_ms_vals[DSAT_MS_MAX_SUBS];
dsat_basic_etsi_table_common_val               dsat_basic_etsi_cmn_ms_vals[DSAT_MS_MAX_SUBS];
#ifdef FEATURE_DATA_IS707 
dsat707_ext_para_table_ss_val_struct_type      dsat707_ext_ss_vals;
#ifdef FEATURE_DSAT_CDMA_PIN 
dsat707_ext_vend_act_table_common_val          dsat707_ext_act_cmn_ss_vals;
#endif /* FEATURE_DSAT_CDMA_PIN */
#ifdef FEATURE_DATA_IS707_ASYNC
dsat707_fax_table_ss_val_struct_type           dsat707_fax_ss_vals;
#endif /* FEATURE_DATA_IS707_ASYNC */
#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_AT_TEST_ONLY 
  dsat707_hdr_table_ss_val_struct_type         dsat707_hdr_ss_vals;
#endif /* FEATURE_HDR_AT_TEST_ONLY */
#endif /* FEATURE_HDR */
#ifdef FEATURE_CDMA_SMS
dsat707_sms_table_ss_val_struct_type            dsat707_sms_ss_vals;
#endif /* FEATURE_CDMA_SMS*/
dsat707_vend_para_table_ss_val_struct_type      dsat707_vend_para_ss_vals;
dsat707_vend_act_table_ss_val_struct_type       dsat707_vend_act_ss_vals;
#endif/* FEATURE_DATA_IS707 */
/*--------------------------------------------------------------*/
/*ATCoP CM interface state vairables*/
dsatcmif_servs_state_info         dsatcmif_ss_ph_val;

dsatetsicall_call_state_info      dsatcmif_call_val;

dsatetsicmif_sups_state_info      dsatcmif_sups_val;

dsatme_mmgsdi_state_info          dsatme_mmgsdi_val;

dsatme_pbm_state_info             dsatme_pbm_val;

dsat_common_state_info            dsat_common_val;

dsatetsipkt_gprs_msg_info         dsatetsipkt_gprs_val;

  dsat_num_item_type  dsat_cgauto_val;
/*Removed till here*/

/*---------------------------------------------------------------------------
     Operating system timers
---------------------------------------------------------------------------*/
LOCAL void dsatutil_timer_cb(unsigned long timer_id);

dsat_timer_s_type dsatutil_timer_table[] =
{
  {DSAT_TIMER_VOICE_RING,
   &dsatvoice_ring_timer, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE, &dsatvoice_sys_mode[SYS_MODEM_AS_ID_1]}  /* Duration varies, set in cmd handler */

#ifdef FEATURE_DUAL_ACTIVE
  ,{DSAT_TIMER_VOICE_RING_2,
   &dsatvoice_ring_timer_2, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE, &dsatvoice_sys_mode[SYS_MODEM_AS_ID_2]}  /* Duration varies, set in cmd handler */
#endif /* FEATURE_DUAL_ACTIVE */

#ifdef FEATURE_DSAT_ETSI_MODE
  ,{DSAT_TIMER_CAOC_REPORT,
   &dsatetsicall_aoc_timer, dsatutil_timer_cb,
   dsatetsicall_caoc_timer_expired_handler,
   DSATETSICALL_CAOC_REPORT_TIMER, FALSE, FALSE,NULL}

  ,{DSAT_INBAND_DTMF_RING,
   &dsatetsicall_inband_dtmf_timer, dsatutil_timer_cb,
   NULL, /* Initialized in dsatutil_init_timers_ex() */
   DSAT_INBAND_DTMF_RING_DELAY, FALSE, FALSE,NULL}
#endif /* FEATURE_DSAT_ETSI_MODE */

  ,{DSAT_TIMER_VOICE_CALL_TIMEOUT,
   &dsatvoice_voice_connection_timer, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE,&dsatvoice_voice_call_id[SYS_MODEM_AS_ID_1]}/* Duration is the S7 value */

#ifdef FEATURE_DUAL_ACTIVE
  ,{DSAT_TIMER_VOICE_CALL_TIMEOUT_2,
   &dsatvoice_voice_connection_timer_2, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE,&dsatvoice_voice_call_id[SYS_MODEM_AS_ID_2]}/* Duration is the S7 value */
#endif /* FEATURE_DUAL_ACTIVE */

   ,{DSAT_TIMER_DATA_CALL_TIMEOUT,
   &dsatvoice_data_connection_timer, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE,&dsatvoice_data_call_id[SYS_MODEM_AS_ID_1]}/* Duration is the S7 value */

   ,{DSAT_TIMER_DATA_CALL_TIMEOUT_2,
   &dsatvoice_data_connection_timer_2, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE,&dsatvoice_data_call_id[SYS_MODEM_AS_ID_2]}/* Duration is the S7 value */

#ifdef FEATURE_DATA_MUX
  ,{DSAT_TIMER_RDM_OPEN_TIMEOUT,
  &dsat_rdm_open_timer, dsatutil_timer_cb,
   dsatact_rdm_timer_expired_handler,
   DSAT_RDM_OPEN_TIMEOUT, FALSE, FALSE,&dsat_port_open_count}
#endif /* FEATURE_DATA_MUX */

#ifdef FEATURE_DATA_PS_EAP
   ,{DSAT_TIMER_CEAP_REPORT_TIMEOUT,
   &dsatetsime_ceap_report_timer, dsatutil_timer_cb,
   NULL, /* Initialized in dsatutil_init_timers_ex() */
   DSAT_CEAP_CMD_WAIT_DELAY, FALSE, FALSE,NULL}

   ,{DSAT_TIMER_CEAP_EAP_TIMEOUT,
   &dsatetsime_ceap_eap_process_timer, dsatutil_timer_cb,
   NULL, /* Initialized in dsatutil_init_timers_ex() */
   DSAT_CEAP_PROCESS_WAIT_DELAY, FALSE, FALSE,NULL}
#endif /* FEATURE_DATA_PS_EAP */
  ,{DSAT_TIMER_FREE_ITEM,
   &dsati_free_item_timer, dsatutil_timer_cb,
   NULL, /* Initialized in dsatutil_init_timers_ex() */
   DSAT_FREE_ITEM_DLY, FALSE, FALSE,NULL}  
};

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL nv_stat_enum_type util_get_nv_item
(
  nv_items_enum_type        item,           /* Which item */
  nv_item_type             *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type    subs_id
);


LOCAL nv_stat_enum_type util_put_nv_item
(
  nv_items_enum_type       item,           /* Which item */
  nv_item_type            *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type   subs_id
);

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================
  Utility Functions
===========================================================================*/

/*===========================================================================

FUNCTION DSAT_IS_ETSI_MODE

DESCRIPTION
   Check if a particular mode passed is 3GPP based opertating mode or not
 
DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_etsi_mode(dsati_mode_e_type mode)
{
  if((mode == DSAT_MODE_GSM) || 
     (mode == DSAT_MODE_WCDMA) || 
     (mode == DSAT_MODE_LTE) || 
     (mode == DSAT_MODE_TDS)|| 
     (mode == DSAT_MODE_1XLTE) )
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION DSAT_IS_VALID_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid

DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_valid_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index
)
{
  if( (tok_ptr!= NULL &&
       token_index < tok_ptr->args_found) && 
      (NULL != tok_ptr->arg[token_index]) && 
      ('\0' != *tok_ptr->arg[token_index]) )
  {
    return TRUE;
  }
  DS_AT_MSG1_HIGH("Empty Argument:  %d", token_index);
  return FALSE;
}

/*===========================================================================

FUNCTION DSAT_IS_VALID_NUM_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid and also converts the string to integer. It checks
   if the token passed is in range.

DEPENDENCIES

RETURN VALUE
  dsat_err_e_type

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_err_e_type dsat_is_valid_num_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index,
  dsat_num_item_type        range_min,
  dsat_num_item_type        range_max,
  void                     *output_buf
)
{

  dsat_num_item_type    *temp = NULL;

  if (tok_ptr == NULL || output_buf == NULL)
  {
    DS_AT_MSG1_HIGH("Invalid Argument:  %d", token_index);
    return DSAT_ERR_INVALID_TOKENS;
  }

  temp = (dsat_num_item_type *)output_buf;

  if( (token_index < tok_ptr->args_found) && 
      (NULL != tok_ptr->arg[token_index]) && 
      ('\0' != *tok_ptr->arg[token_index]) )
  {
    if(ATOI_OK == dsatutil_atoi(temp,tok_ptr->arg[token_index],10) &&
       (*temp <= range_max) &&(*temp >= range_min))
    {
      return DSAT_ERR_NO_ERROR;
    }
    DS_AT_MSG1_ERROR("[INT] Conversion Fail or Out of Range token: %d", token_index);
    return DSAT_ERR_ATOI_CONVERSION_FAILURE;
  }

  DS_AT_MSG1_HIGH("Empty Argument:  %d", token_index);
  return DSAT_ERR_INVALID_TOKENS;
}

/*===========================================================================

FUNCTION DSAT_IS_VALID_STRING_TOKEN

DESCRIPTION
   Check if a particular token of the current token_ptr is 
   in valid and also strip the quotes from the token_ptr passed.

DEPENDENCIES

RETURN VALUE
  dsat_err_e_type

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_err_e_type dsat_is_valid_string_token
(
  const tokens_struct_type *tok_ptr,
  uint8                     token_index,
  void                     *input_buf,
  void                     *output_buf,
  word                      str_len
)
{
  const byte *input_temp  = NULL;
  byte       *output_temp = NULL;

  if (tok_ptr == NULL || output_buf == NULL || input_buf == NULL || str_len <= 0)
  {
    DS_AT_MSG1_HIGH("Invalid Argument:  %d", token_index);
    return DSAT_ERR_INVALID_TOKENS;
  }

  input_temp  = (const byte *) input_buf;
  output_temp = (byte *) output_buf;

  if( (token_index < tok_ptr->args_found) && 
      (NULL != tok_ptr->arg[token_index]) && 
      ('\0' != *tok_ptr->arg[token_index]) )
  {
    if ( dsatutil_strip_quotes_out(input_temp,
                                   output_temp,
                                   str_len))
    {
      return DSAT_ERR_NO_ERROR;
    }
    DS_AT_MSG1_ERROR("[String] Conversion Fail or Out of Range token: %d", token_index);
    return DSAT_ERR_QUOTE_STRIP_FAILURE;
  }

  DS_AT_MSG1_HIGH("Empty Argument:  %d", token_index);
  return DSAT_ERR_INVALID_TOKENS;
}

/*===========================================================================

FUNCTION DSAT_IS_PROFILE_ID_IN_RANGE

DESCRIPTION
   Check if a particular profile_id being passed is in valid range or not
 
DEPENDENCIES

RETURN VALUE
  boolean: TRUE if valid, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsat_is_profile_id_in_range(ds_profile_num_type profile_id)
{
  if( (profile_id >= DSAT_LEGACY_PROFILE_NUM_MIN && profile_id <= DSAT_LEGACY_PROFILE_NUM_MAX) || 
      (profile_id >= DSAT_EXT_PROFILE_NUM_MIN && profile_id<= DSAT_EXT_PROFILE_NUM_MAX ) )
  {
    return TRUE;
  }
  return FALSE;

}

/*===========================================================================

FUNCTION DSAT_PROFILE_SET_PARAM

DESCRIPTION
   Function to set the paramter in a profile created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_set_param
(
  ds_profile_hndl_type profile_hndl,
  ds_profile_identifier_type param_type,
  const ds_profile_info_type param_info
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status =  ds_profile_set_param(profile_hndl, 
                                         param_type, 
                                         &param_info);

  DS_AT_MSG2_HIGH("status for param %d is %d", 
                         param_type,profile_status);
  return profile_status;
}

/*===========================================================================

FUNCTION DSAT_PROFILE_GET_PARAM

DESCRIPTION
   Function to get the paramter in a profile created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_get_param
(
  ds_profile_hndl_type profile_hndl,
  ds_profile_identifier_type param_type,
  ds_profile_info_type param_info
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status =  ds_profile_get_param(profile_hndl,
                                         param_type,
                                         &param_info);

  DS_AT_MSG2_HIGH("status for param %d is %d", param_type,profile_status);
  return profile_status;
}

/*===========================================================================

FUNCTION DSAT_PROFILE_RESET_PARAM_TO_DEFAULT

DESCRIPTION
   Function to reset the paramter to default in a profile
   created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_reset_param_to_default
(
  ds_profile_tech_etype         tech,
  ds_profile_num_type           num,
  ds_profile_identifier_type    ident
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status = ds_profile_reset_param_to_default_per_sub(tech,
                                                             num,
                                                             ident,
                                                             dsat_get_current_ds_profile_subs_id()
                                                            );

  DS_AT_MSG2_HIGH("status for param %d is %d", ident, profile_status);
  return profile_status;
}

/*===========================================================================

FUNCTION DSAT_PROFILE_RESET_PARAM_TO_INVALID

DESCRIPTION
   Function to reset the paramter to invalid in a profile
   created using ds_profile.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_reset_param_to_invalid
(
  ds_profile_tech_etype         tech,
  ds_profile_num_type           num,
  ds_profile_identifier_type    ident
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status = ds_profile_reset_param_to_invalid_per_sub(tech,
                                                             num,
                                                             ident,
                                                             dsat_get_current_ds_profile_subs_id()
                                                            );

  DS_AT_MSG2_HIGH("status for param %d is %d", ident, profile_status);
  return profile_status;
}

/*===========================================================================

FUNCTION DSAT_PROFILE_BEGIN_TRANSACTION

DESCRIPTION
   Function to begin a ds_profile transaction to perform action, such as -
   SET_PARAM, GET_PARAM, etc...
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_begin_transaction
(
  ds_profile_tech_etype         tech,
  ds_profile_num_type           num,
  ds_profile_hndl_type          *profile_hndl
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status =  ds_profile_begin_transaction_per_sub( DS_PROFILE_TRN_RW, 
                                                          tech,
                                                          (ds_profile_num_type)num,
                                                          dsat_get_current_ds_profile_subs_id(),
                                                          profile_hndl);
                                                          

  if( profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    DS_AT_MSG2_ERROR("Error Begin tx for profile: %d error : %d",num,profile_status);
  }
  return profile_status;
}

/*===========================================================================

FUNCTION DSAT_PROFILE_BEGIN_TRANSACTION

DESCRIPTION
   Function to end a ds_profile transaction to perform action. This will
   release the profile_hndl created in ds_profile using atcop.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_end_transaction
(
  ds_profile_hndl_type          profile_hndl
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status =  ds_profile_end_transaction(profile_hndl, 
                                               DS_PROFILE_ACTION_COMMIT);

  if( profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  { 
    DS_AT_MSG1_ERROR("Error end tx profile_status : %d",profile_status);
  }
  return profile_status;
}
/*===========================================================================

FUNCTION DSAT_PROFILE_SESTROY_ITERATOR

DESCRIPTION
   Function to delete the iterator created. This will
   release the interator created in ds_profile using atcop for
   traversing the profiles.
 
DEPENDENCIES

RETURN VALUE
  ds_profile_status_etype: DS_PROFILE_REG_RESULT_SUCCESS is set was
                           successful, FALSE otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
ds_profile_status_etype dsat_profile_destroy_iterator
(
  ds_profile_itr_type           itr
)
{
  ds_profile_status_etype profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  profile_status = ds_profile_itr_destroy(itr); 

  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS) 
  {
    DS_AT_MSG1_ERROR ("Destroying iterator failed. Error %d", profile_status); 
  }
  return profile_status;
}

/*===========================================================================

FUNCTION DSATUTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFFFFFF and a radix of 2,
  which returns 33 bytes (32 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * dsatutil_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
)
{
  byte buf[MAX_ITOA_LEN], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      DS_AT_MSG0_HIGH("OVERFLOW");
      break;
    }

    buf[--n] = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');

  } while ( (v /= r) > 0);

  while ( (*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  dsatutil_itoa */


/*===========================================================================

FUNCTION DSATUTIL_STRCMP_IG_SP_CASE

DESCRIPTION
  This function compares two null terminated strings, ignoring case.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical, except for case,
            otherwise, it returns the difference between the first
            characters to disagree. The difference is first arg minus
            second arg.  Lower case letters are converted to upper
            case before the difference is taken.

SIDE EFFECTS
  None.

===========================================================================*/
int dsatutil_strcmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2              /*  Pointer to string 2 */
)
{
  byte c1, c2;
  int d;

  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0');

  return d;
} /*  dsatutil_strcmp_ig_sp_case  */

/*===========================================================================

FUNCTION DSATUTIL_STRNCMP_IG_SP_CASE

DESCRIPTION
  This function compares two strings for <n> characters, ignoring case.
  Lower case letters are converted to upper case before the 
  difference is taken.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical till <n> characters, except for 
            case otherwise, it returns the difference between 
            the first characters to disagree. The difference is first arg 
            minus second arg.  

SIDE EFFECTS
  None.

===========================================================================*/
int dsatutil_strncmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2,             /*  Pointer to string 2 */
  uint32      n               /* Number of characters to compare */
)
{
  byte c1, c2;
  int d;

  if ( n== 0 )
  {
    return -1;
  }
  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
    --n;
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0' && n != 0 );

  return d;
}/* dsatutil_strncmp_ig_sp_case */

/*===========================================================================

FUNCTION DSATUTIL_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
  radix.  Spaces are ignored. Letters used for radices above 10 may be of
  either case.

DEPENDENCIES


RETURN VALUE
  Returns 
    ATOI_OK           : for a correct conversion,
    ATOI_NO_ARG       : 1 if no argument was found,
    ATOI_OUT_OF_RANGE : if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/

atoi_enum_type dsatutil_atoi
(
  dsat_num_item_type *val_arg_ptr,      /*  value returned  */
  const byte *s,                        /*  points to string to eval  */
  unsigned int r                        /*  radix */
)
{
  atoi_enum_type err_ret = ATOI_NO_ARG;
  byte c;
  dsat_num_item_type val, val_lim, dig_lim;

  val = 0;
  val_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM / r);
  dig_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM % r);

  while ( (c = *s++) != '\0')
  {
    if (c != ' ')
    {
      c = (byte) UPCASE (c);
      if (c >= '0' && c <= '9')
      {
        c -= '0';
      }
      else if (c >= 'A')
      {
        c -= 'A' - 10;
      }
      else
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too small */
        break;
      }

      if (c >= r || val > val_lim
          || (val == val_lim && c > dig_lim))
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too large */
        break;
      }
      else
      {
        err_ret = ATOI_OK;            /*  arg found: OK so far*/
        val = (dsat_num_item_type) (val * r + c);
      }
    }
    *val_arg_ptr =  val;
  }
  
  return err_ret;

} /*  dsatutil_atoi */

/*===========================================================================

FUNCTION DSATUTIL_RESET_PASSWORD

DESCRIPTION
  This function clears the password variable of specified length.
  The password string is set to 0xFF, null terminated.  

  The provided 'length' should be the total length of the buffer pointed to by 'password' including allocation 
  for the trailing NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatutil_reset_password
(
  byte * password,            /* String containing password */
  word length                 /* Maximum length of storage pointed to by "password" */
)
{
  int index;

  /* Do nothing if null pointer passed */
  if (NULL == password) return; 

  /* init the password */
  for (index = 0; index < length-1; index++)
  {
    password[index] = (char) 0xFF;
  }
  password[index] = 0;
  return;
} /* dsatutil_reset_password */

/*===========================================================================

FUNCTION DSATUTIL_DSM_NEW_BUFFER

DESCRIPTION
  This function is a wrapper for dsm_new_buffer() API.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsatutil_dsm_new_buffer
(
  dsm_mempool_id_type  pool_id,
  const char          *filename,
  uint32               line,
  boolean              graceful
)
{
  dsm_item_type *dsm_item_ptr = NULL;

  dsm_item_ptr = dsmi_new_buffer(pool_id, filename, line);
  if (NULL == dsm_item_ptr)
  {
    if (TRUE == graceful)
    {
      DS_AT_MSG_SPRINTF_3_ERROR("DSM items exhausted in pool 0x%x (%s, %d)",
                                pool_id, filename, line);
    }
    else
    {
      ERR_FATAL("DSM items exhausted in pool 0x%x", pool_id, 0, 0);
    }
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated DSM item 0x%x in pool 0x%x (%s, %d)",
                            pool_id, dsm_item_ptr, filename, line);
  }

  return dsm_item_ptr;
} /* dsatutil_dsm_new_buffer */

/*===========================================================================

FUNCTION DSATUTIL_DSM_PUSHDOWN_TAIL

DESCRIPTION
  This function is a wrapper for dsm_pushdown_tail() API.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the pushdown requires an additional data item, the pkt_ptr field of the 
  last item in the passed packet chain may be changed.

===========================================================================*/
void dsatutil_dsm_pushdown_tail
(
  dsm_item_type       **pkt_head_ptr,
  const void           *buf,
  uint16                size,
  dsm_mempool_id_type   pool_id,
  const char           *filename,
  uint32                line,
  boolean               graceful
)
{
  uint16 bytes_copied;

  bytes_copied = dsmi_pushdown_tail(pkt_head_ptr, buf, size, pool_id, filename, line);
  if (bytes_copied != size)
  {
    if (NULL != *pkt_head_ptr)
    {
      dsmi_free_packet(pkt_head_ptr, filename, line);
      *pkt_head_ptr = NULL;
    }
  }

  if (NULL == *pkt_head_ptr)
  {
    if (TRUE == graceful)
    {
      DS_AT_MSG_SPRINTF_5_ERROR("Failed to pushdown data of size %d on DSM packet "
                                "in pool 0x%x and bytes copied %d (%s, %d)",
                                size, pool_id, bytes_copied, filename, line);
    }
    else
    {
      ERR_FATAL("Failed to pushdown data of size %d on DSM packet in pool 0x%x and"
                " bytes copied %d", size, pool_id, bytes_copied);
    }
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Pushed down data of size %d on DSM packet in pool "
                            "0x%x (%s, %d)", size, pool_id, filename, line);
  }

  return;
} /* dsatutil_dsm_pushdown_tail */

/*===========================================================================

FUNCTION DSATUTIL_DSM_CREATE_PACKET

DESCRIPTION
  This function creates a DSM packet.

DEPENDENCIES
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type *dsatutil_dsm_create_packet
(
  const void *buf,
  uint16      size,
  const char *filename,
  uint32      line,
  boolean     graceful
)
{
  dsm_item_type *dsm_item_ptr = NULL;

  dsm_item_ptr = dsatutil_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL,
                                         filename, line, graceful);
  if (NULL == dsm_item_ptr)
  {
    dsm_item_ptr = dsatutil_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL,
                                           filename, line, graceful);
  }

  if (NULL != dsm_item_ptr)
  {
    (void) dsatutil_dsm_pushdown_tail(&dsm_item_ptr, buf, size,
                                      DSM_ITEM_POOL(dsm_item_ptr),
                                      filename, line, graceful);
  }

  return dsm_item_ptr;
} /* dsatutil_dsm_create_packet */

/*===========================================================================

FUNCTION DSATUTIL_INIT_OS_TIMERS

DESCRIPTION
  This function registers timers with the opeating system.  Starting
  and clearing timers is done in individual command handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_timers_ex ( void )
{
  uint8 i;

  /* Loop over timer table */
  for(i=0; i<(uint8)DSAT_TIMER_MAX; i++)
  {
    /* Register each timer with operating system */
    if ( !dsatutil_timer_table[i].is_defined )
    {
      rex_def_timer_ex(dsatutil_timer_table[i].timer_ptr,
                       dsatutil_timer_table[i].cb_func, 
                       (unsigned long) dsatutil_timer_table[i].timer_id);
      dsatutil_timer_table[i].is_defined = TRUE;
    }
  }
} /* dsatutil_init_timers_ex */


/*===========================================================================

FUNCTION DSATUTIL_DISPATCH_TIMER_HANDLER

DESCRIPTION
  This function dispatches the common timer expiry event to the
  correct timer handler.  If the timer is not found, an error message
  is generated.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNCH_EVENT : asynch event has been dispatched

SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsatutil_dispatch_timer_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type  result       = DSAT_ASYNC_EVENT;
  boolean                timer_found  = FALSE;
  uint8                  i            = 0;
  dsat_timer_id_e_type  *timer_id_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  /* Loop over timer table */
  for(i=0; i<(uint8)DSAT_TIMER_MAX; i++)
  {
    /* Check for match of timer ID */
    if ( ((unsigned int)dsatutil_timer_table[i].timer_id == *timer_id_ptr) &&
         (NULL != dsatutil_timer_table[i].hdlr_func))
    {
      timer_found = TRUE;
      
      /* Invoke the timer handler function */
      dsatutil_timer_table[i].is_running = FALSE;
      result = dsatutil_timer_table[i].hdlr_func (cmd_ptr);
    }
  }

  /* Report error if unknown timer */
  if( FALSE == timer_found )
  {
    DS_AT_MSG1_ERROR("Timer ID or handler not found in table: %d",
         *timer_id_ptr);
  }

  return result;
} /* dsatutil_dispatch_timers */

/*===========================================================================
FUNCTION UTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL nv_stat_enum_type util_get_nv_item
(
  nv_items_enum_type         item,           /* Which item */
  nv_item_type              *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type     subs_id
)
{
  nv_cmd_type  nv_command;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&nv_command, 0x0, sizeof(nv_cmd_type));
  
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  if (rex_self() == rcinit_lookup_rextask("ds"))
  {
    nv_command.status = ds3gcfgmgr_read_legacy_nv_ex( item,
                                                       data_ptr,
                                                      (sys_modem_as_id_e_type)subs_id);
  }
  else if (rex_self() == rcinit_lookup_rextask("ps")) 
  {
        nv_command.status = ps_get_nv_item (item, data_ptr);
  }
#else
  if (rex_self() == &ds_tcb)
  {
    nv_command.status = ds3gcfgmgr_read_legacy_nv_ex( item,
                                                       data_ptr,
                                                      (sys_modem_as_id_e_type)subs_id);
  }
  else if (rex_self() == &ps_tcb) 
  {
        nv_command.status = ps_get_nv_item (item, data_ptr);
  }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  if (NV_DONE_S != nv_command.status)
  {
    DS_AT_MSG2_ERROR("Failed to get NV item %d, status = %d",
                     item, nv_command.status);
  }

  return( nv_command.status );
}/* util_get_nv_item */

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  return util_get_nv_item(item, data_ptr, SYS_MODEM_AS_ID_1);
}

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM_PER_SLOT

DESCRIPTION
  Get an item from the nonvolatile memory per slot.

RETURN VALUE
  The NV return code.

DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item_per_slot
(
  nv_items_enum_type        item,           /* Which item */
  nv_item_type             *data_ptr,       /* Pointer to space for item */
  mmgsdi_slot_id_enum_type  slot_id         /* Slot ID */
)
{
  return util_get_nv_item(item, data_ptr, (sys_modem_as_id_e_type)(slot_id - MMGSDI_SLOT_1));
}

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM_PER_SUBS 

DESCRIPTION
  Get an item from the nonvolatile memory per subscription.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item_per_subs 
(
  nv_items_enum_type        item,           /* Which item */
  nv_item_type              *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type    subs_id
  
)
{
  return util_get_nv_item(item, data_ptr, subs_id);
}
/*===========================================================================
FUNCTION UTIL_PUT_NV_ITEM_WRAPPER

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL nv_stat_enum_type util_put_nv_item
(
  nv_items_enum_type     item,           /* Which item */
  nv_item_type           *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type subs_id
)
{
  nv_cmd_type  nv_command;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&nv_command, 0x0, sizeof(nv_cmd_type));
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  if (rex_self() == rcinit_lookup_rextask("ds"))
  {
    nv_command.status = ds3gcfgmgr_write_legacy_nv_ex ( item, 
                                                        data_ptr,
                                                        (sys_modem_as_id_e_type)subs_id);
  }
  else if (rex_self() == rcinit_lookup_rextask("ps")) 
  {
        nv_command.status = ps_put_nv_item (item, data_ptr);
  }
#else
  if (rex_self() == &ds_tcb)
  {
    nv_command.status = ds3gcfgmgr_write_legacy_nv_ex ( item, 
                                                        data_ptr,
                                                        (sys_modem_as_id_e_type)subs_id);
  }
  else if (rex_self() == &ps_tcb) 
  {
        nv_command.status = ps_put_nv_item (item, data_ptr);
  }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

  if (NV_DONE_S != nv_command.status)
  {
    DS_AT_MSG2_ERROR("Failed to put NV item %d, status = %d",
                     item, nv_command.status);
  }

  return( nv_command.status );
}/* util_put_nv_item */


/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  return util_put_nv_item(item, data_ptr,SYS_MODEM_AS_ID_1);
}

/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM_PER_SUBS 

DESCRIPTION
  Put an item from the nonvolatile memory per subs.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item_per_subs 
(
  nv_items_enum_type     item,           /* Which item */
  nv_item_type           *data_ptr,        /* Pointer to space for item */
  sys_modem_as_id_e_type subs_id
)
{
  return util_put_nv_item(item, data_ptr, subs_id);
}

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_TUPLE

DESCRIPTION
  This function checks for the validity of known tuple formats and converts
  between string and number representations.  
  For IPV6 addresses only the preferred text representation of RFC2373 is 
  supported: quantity 8, 16 bit HEX numbers, separated by colons and without
  zero compressing double colon pairs "::".
    
      For IPv6 addresses, the number
  pointer is assumed to be an array of uint32 values.
  
  Tuples supported:
     IPv4       range: 0.0.0.0 - 255.255.255.255
     IPv4_254   range: 0.0.0.0 - 254.254.254.254
     PORT       range: 0.0 - 65535.65535
     TMASK      range: 0.0 - 255.255
     IPv6       range: 0:0:0:0:0:0:0:0 - 
                         FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF

DEPENDENCIES
  For IPv6 addresses, the number pointer is assumed to be an array[4] of
  uint32 values.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if validation successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_tuple 
(
   tuple_convert_enum_type mode,             /* Mode of conversion      */
   tuple_type_enum_type ttype,               /* Type of tuple           */
   uint32* num_ptr,                          /* Integer representation */
   byte* buf_ptr,                            /* String representation   */
   uint8 buf_len,                            /* String length           */
   dsat_ip_mask_enum_type ip_mask            /* IP or Mask Convertion   */
)
{
  #define MAX_TOKENS 16             /* Max num of delimiter separated tokens */
  #define MAX_TOKEN_LEN 5           /* Max num chars in a token */

  /* Only RFC2732 preferred text representation for IPV6 addresses uses ':'
     separators */
  char delimiter = 
    (ttype == TUPLE_TYPE_IPv6 ? IPV6_DELIMITER : IPV4_DELIMITER);
  /* When parsing IPV6 addresses, how many tokens per uint32 number */
  uint8 max_tokens_per_number = (ttype == TUPLE_TYPE_IPv6_OCTETS ? 4 : 2);
  atoi_enum_type result;
  /* Only RFC2732 preferred text representation for IPV6 addresses uses hex
     characters */
  byte radix = (ttype == TUPLE_TYPE_IPv6 ? 16 : 10);
  uint8  num_tokens;
  uint8  token_bits;
  uint32 token_mask;
  uint32 token_limit;
  dsat_num_item_type val[MAX_TOKENS]={0};
  uint32 *num_ptr_orig = num_ptr; 
  uint8 num_ptr_max_len=1;

  /* Setup conversion paramaters */
  switch (ttype)
  {
  case TUPLE_TYPE_IPv4:
    token_bits = 8;
    token_mask = token_limit = (1 << token_bits) - 1;
    max_tokens_per_number = num_tokens = 4;
    break;

  case TUPLE_TYPE_PORT:
    token_bits = 16;
    token_mask = token_limit = (1 << token_bits) - 1;
    max_tokens_per_number = num_tokens = 2;
    break;
    
  case TUPLE_TYPE_TMASK:
    token_bits = 8;
    token_mask = token_limit = (1 << token_bits) - 1;
    max_tokens_per_number = num_tokens = 2;
    break;
    
  case TUPLE_TYPE_IPv6:
    token_bits = 16;
    token_mask = token_limit = (1 << token_bits) - 1;
    num_tokens = 8;
    max_tokens_per_number = 2;
    num_ptr_max_len = 4;
    break;
    
  case TUPLE_TYPE_IPv6_OCTETS:
    token_bits = 8;
    token_mask = token_limit = (1 << token_bits) - 1;
    num_tokens = 16;
    max_tokens_per_number = 4;
    num_ptr_max_len = 4;
    break;
    
  default:
    DS_AT_MSG1_ERROR("Unsupported tuple type: %d", ttype);
    return DSAT_ERROR;
  }

  /* Perform conversion */
  if (STRING_TO_INT == mode)
  { 
#define NUM_BITS_VAL 32

    int cpi=0, vali=0;
    byte cp[MAX_TOKEN_LEN+1];
    byte * c = buf_ptr;
    int i,j;
    boolean loop = TRUE;
    memset((void*)cp,0,sizeof(cp));

    /*---------------------------------------------------------------
      The following loop processes the string representing an tuple.
      Each token is converted in to decimal integer and stored in
      a temporary array.
      If the value is greater than the limit, we return an error.
    ---------------------------------------------------------------*/
    while (loop && (num_tokens > vali))
    {
      /* Skip quotes */
      if ('"' != *c)
      {
        /* Build token until delimiter or end of buffer */
        loop = (('\0' != *c)? TRUE : FALSE );
        if ( (delimiter != *c) && loop )
        {
          cp[cpi++] = *c;

          if (MAX_TOKEN_LEN < cpi)
          {
            DS_AT_MSG2_ERROR("Token exceeds max length: %d < %d",
                       MAX_TOKEN_LEN,cpi);
            return DSAT_ERROR;
          }
        }
        else 
        {
          /* Convert to integer */
          result = dsatutil_atoi( (val+vali), cp, radix);
          if (ATOI_OK != result)
          {
            return DSAT_ERROR;
          }

          /* Test token against limit */
          if((*(val+vali)) > token_limit)
          {
            DS_AT_MSG2_MED("Token %d larger than limit %d",*(val+vali),token_limit);
            return DSAT_ERROR;
          }

          /* Reinitialize buffer */
          memset((void*)cp,0,sizeof(cp));
          cpi = 0;         
          vali++;
        }
      }
      c++;
    }
    
    /* Verify expected tokens found */
    if (num_tokens != vali)
    {
      DS_AT_MSG2_ERROR("Expected number of tokens violated: %d != %d",
                num_tokens,vali);
      return DSAT_ERROR;
    }

    /* Convert to number, processing tokens in order encountered */
    *num_ptr = 0;
    j = 0;
    for (i = num_tokens - 1; i >= 0; i--)
    {
      /* Check for number array element change (IPv6 only) */
      if (max_tokens_per_number <= j)
      {
        num_ptr++;
        if ((num_ptr - num_ptr_orig) >= num_ptr_max_len)
        {
          DS_AT_MSG1_ERROR("exceeded num_ptr_max_len %d",num_ptr_max_len);
          return DSAT_ERROR;
        }

        *num_ptr = 0;
        j = 0;
      }

      *num_ptr = (*num_ptr >> token_bits) | 
                 (val[i] << (NUM_BITS_VAL - token_bits));
      j++;

      /* If all the tokens per this 32 bit number have been processed and
         it didn't use up all 32 bits of number:
         right shift number down by quantity of unused bits.
         (this currently only applies to case  of TOS/mask) */
      if ( j == max_tokens_per_number &&
           NUM_BITS_VAL > (token_bits * max_tokens_per_number) ) 
      {
        *num_ptr = (*num_ptr >> (NUM_BITS_VAL - 
                                 (token_bits * max_tokens_per_number)));
      }
    }
  }
  else /* INT_TO_STRING */
  {
    uint32 num;
    int i,j;
    dsat_num_item_type   cgpiaf_add_format        = 0;
    dsat_num_item_type   cgpiaf_LeadingZero_format = 0;
    dsat_num_item_type   cgpiaf_CompressZero_format = 0;
    byte num_tokens_per_word = 
      ((ttype == TUPLE_TYPE_IPv6 || ttype == TUPLE_TYPE_IPv6_OCTETS) ? 
        (num_tokens >> 2) : num_tokens);
    byte delim_str[2];

    delim_str [0] = delimiter;
    delim_str [1] = '\0';

    memset ((void*)buf_ptr, 0, buf_len);
    j = num_tokens_per_word-1;

    /* uint32 from DSUMTS looks like this: 
       byte0="4", byte1="3", byte2="2", byte 3="1" for IP address like
       "1.2.3.4" so parse off token bits from bottom of uint32s
       inserting into val array starting from top */
    for (i = num_tokens - 1; i >= 0; i--)
    {
      if ((num_ptr - num_ptr_orig) >= num_ptr_max_len)
      {
        DS_AT_MSG1_ERROR("exceeded num_ptr_max_len %d",num_ptr_max_len);
        return DSAT_ERROR;
      }
      num = *num_ptr;
      num >>= (token_bits * ((num_tokens_per_word-1) - j));

      val[i] = (dsat_num_item_type)(num & token_mask);
      j--;
      
      /* Check for number array element change (IPv6 only) */
      if (0 > j)
      {
        num_ptr++;
        j = num_tokens_per_word-1;
      }
    }
    
    /* then go through val array of token integers creating delimited 
       string */
    /* Decides IPv6 address representation based on +CGPIAF setting*/
     cgpiaf_add_format          = (dsat_num_item_type)dsatutil_get_val(
                                      DSATETSI_EXT_CGPIAF_IDX,0,0,NUM_TYPE);

    if( (DSAT_CONVERT_IP == ip_mask)&&
        (ttype == TUPLE_TYPE_IPv6)&&
        (1 == cgpiaf_add_format))
    {
      int last_count = 0;
      int count = 0;
      int index = 0;
      int last_token = 0;
      byte work_buffer[6];

      cgpiaf_CompressZero_format  = (dsat_num_item_type)dsatutil_get_val(
                                        DSATETSI_EXT_CGPIAF_IDX,0,3,NUM_TYPE);
      if(1 == cgpiaf_CompressZero_format)
      {
        for(i = 0 ; i < num_tokens ; i++)
        {
          if(val[i] == 0)
          {   
            count++;
            if(last_count <= count)
            {
               last_count = count;
               index = i;
            }
          }
          else
          {
              count = 0;
          }
        }
      }
      DS_AT_MSG3_HIGH("MAX count %d last index %d first index %d \n",
                                last_count,index,index-last_count+1);
      
      cgpiaf_LeadingZero_format  = (dsat_num_item_type)dsatutil_get_val(
                                        DSATETSI_EXT_CGPIAF_IDX,0,2,NUM_TYPE);

      for( i = 0 ; i < num_tokens ; i++)
      {
        if (i == (num_tokens - 1))
          last_token = 1;

        if(i < index-last_count+1 || i > index)
        {
          if(1 == cgpiaf_LeadingZero_format)
          {
            snprintf((char *)work_buffer, sizeof(work_buffer),
                     "%04X%c",
                     val[i], (last_token ? '\0' : delimiter));
          }
          else
          {
            snprintf((char *)work_buffer, sizeof(work_buffer),
                     "%X%c",
                     val[i], (last_token ? '\0' : delimiter));
          }
          strlcat((char *)buf_ptr, (char *)work_buffer, buf_len);
        }
        else
        {
          if(i == index)
          {
            strlcat((char *)buf_ptr, (char *)delim_str, buf_len);
          }
          if(0 == index-last_count+1 && i == 0)
          {
            strlcat((char *)buf_ptr, (char *)delim_str, buf_len);
          }
        }
      }
    }
    else
    {
      /* This size is guaranteed not to overflow with any radix */
      byte work_buf[MAX_ITOA_LEN];
      for (i=0; i < num_tokens; i++)
      {
        dsatutil_itoa (val[i], work_buf, radix);
        strlcat((char *)buf_ptr, (char *)work_buf, buf_len);
        if (i < (num_tokens - 1))
          strlcat((char *)buf_ptr, (char *)delim_str, buf_len);
      }
    }    
  }

  return DSAT_OK;
} /* dsatutil_convert_tuple */


/*===========================================================================

FUNCTION DSATUTIL_TIMER_CB

DESCRIPTION
  This function is called when timer expires.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void dsatutil_timer_cb(unsigned long timer_id)
{
  /* submit dstask a cmd */
  ds_cmd_type                   * cmd_ptr     = NULL;
  dsat_timer_id_e_type          *timer_id_ptr = NULL;

    /*-------------------------------------------------------------------------
    Send a DS_TIMER_EXPIRED_CMD to the DS task, and indicate which timer
    expired.
  -------------------------------------------------------------------------*/
  cmd_ptr = dsat_get_cmd_buf(sizeof(dsat_timer_id_e_type), TRUE);

  if (NULL != cmd_ptr)
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_TIMER_EXPIRED_CMD;
    timer_id_ptr         = cmd_ptr->cmd_payload_ptr;
    *timer_id_ptr        = timer_id;
    ds_put_cmd( cmd_ptr );
  }
} /* dsatutil_timer_cb */

#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_IPADDR

DESCRIPTION
  This function converts between IP V4/V6 addresses and ds_umts_pdp_addr_types.
  In identifies the type of conversion to be done and passes the rest of 
  the work off to the convert tuple function. In string to number mode the 
  presence of a colon in string identifies an IPV6 conversion else IPV4 is 
  assumed. In number to string mode the setting of 
  ds_umts_pdp_addr_type.ip_vsn identifies conversion type.

DEPENDENCIES
  Buffer pointed to by buf_ptr should be at least MAX_IPADDR_STR_LEN bytes 
  in length to hold NULL terminated, maximum length IPV6 address for 
  INT_TO_STRING conversion.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_OK : if validation successful
    DSAT_ERROR : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_ipaddr
(
   tuple_convert_enum_type mode,             /* Mode of conversion      */
   dsat_ip_addr_type* addr_ptr,          /* Number representation   */
   byte* buf_ptr                             /* NULL terminated string  */
)
{
  /* word aligned address */
  uint32 addr32[4];
  dsat_num_item_type   cgpiaf_add_format = 0;
  tuple_type_enum_type tuple_type = TUPLE_TYPE_MAX;

  memset( (void *)addr32, 0, (sizeof(uint32) * 4) );

  if ( INT_TO_STRING == mode )  
  {
    /* copy to local word aligned address */
    if ( DS_PROFILE_3GPP_IP_V6 == addr_ptr->ip_vsn )
    {
          addr32[0] = addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[0];
          addr32[1] = addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[1];
          addr32[2] = addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[2];
          addr32[3] = addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[3];
    }
    else
    {
          addr32[0] = addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv4;
    }
  }

  /* For IPV6 conversion either mode... */
  if ( (STRING_TO_INT == mode && strchr((char *)buf_ptr, IPV6_DELIMITER)) ||
           (INT_TO_STRING == mode && DS_PROFILE_3GPP_IP_V6 == addr_ptr->ip_vsn) )
  {
    /* PDP address parm may not be given but IPV6 delimiter has already 
       been found in string, so no need to test for NULL string as
       in else block below */
       
    /* Decides IPv6 address representation based on +CGPIAF setting*/
    tuple_type = TUPLE_TYPE_IPv6;
    if(INT_TO_STRING == mode)
    {
      cgpiaf_add_format = (dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_CGPIAF_IDX,0,0,NUM_TYPE);
      /* "1" Use IPv6-like colon-notation. IP address */
      if(1 == cgpiaf_add_format)
      {
         tuple_type = TUPLE_TYPE_IPv6;
      }
      else
      {
         tuple_type = TUPLE_TYPE_IPv6_OCTETS;
      }
    }
    if ( DSAT_OK != dsatutil_convert_tuple(
                                     mode,
                                     tuple_type,
                                     addr32,
                                     buf_ptr,
                                     MAX_IPADDR_STR_LEN+1,
                                     DSAT_CONVERT_IP) )
    {
      DS_AT_MSG0_ERROR("Failed IPV6 address conversion");
      return DSAT_ERROR;
    }
    if ( STRING_TO_INT == mode )
    {
          addr_ptr->ip_vsn = DS_PROFILE_3GPP_IP_V6;
    }
  }
  else /* Assume IPV4 conversion */
  {
    if ( DSAT_OK != dsatutil_convert_tuple(mode,
                                           TUPLE_TYPE_IPv4,
                                           addr32,
                                           buf_ptr,
                                           MAX_IPADDR_STR_LEN+1,
                                           DSAT_CONVERT_IP) )
    {
      DS_AT_MSG0_ERROR("Failed IPV4 address conversion");
      return DSAT_ERROR;
    }
    if ( STRING_TO_INT == mode )
    {
          addr_ptr->ip_vsn = DS_PROFILE_3GPP_IP_V4;
    }
  }

  if ( STRING_TO_INT == mode )
  {
    /* copy from local word aligned address */
    if ( DS_PROFILE_3GPP_IP_V6 == addr_ptr->ip_vsn )
    {
      addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[0] = addr32[0];
      addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[1] = addr32[1];
      addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[2] = addr32[2];
      addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr32[3] = addr32[3];
    }
    else
    {
      addr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv4 = addr32[0];
    }
  }

  return DSAT_OK;
} /* dsatutil_convert_ipaddr */

#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================
FUNCTION  DSATUTIL_VALIDATE_MEMORY_MSG_ERROR

DESCRIPTION
  Validates an allocated block of memory from Modem Heap memory
  and calls an error F3 if invalid.

DEPENDENCIES
  None

RETURNS
  The Data Pointer to the allocated Memory.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_validate_memory_msg_error
(
  void       *data_ptr,
  uint32      data_size,
  const char *filename,
  uint32      line
)
{
  if (NULL == data_ptr)
  {
    DS_AT_MSG_SPRINTF_3_ERROR("Failed to allocate heap memory block of size %d (%s, %d)",
                              data_size, filename, line);
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated heap memory block 0x%x of size %d (%s, %d)",
                            data_size, data_ptr, filename, line);

    memset(data_ptr, 0x00, data_size);
  }

  return data_ptr;
} /* dsatutil_validate_memory_msg_error */

/*===========================================================================
FUNCTION  DSATUTIL_VALIDATE_MEMORY_ERR_FATAL

DESCRIPTION
  Validates an allocated block of memory from Modem Heap memory
  and calls ERR_FATAL if invalid.

DEPENDENCIES
  None

RETURNS
  The Data Pointer to the allocated Memory.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_validate_memory_err_fatal
(
  void       *data_ptr,
  uint32      data_size,
  const char *filename,
  uint32      line
)
{
  if (NULL == data_ptr)
  {
    ERR_FATAL("Failed to allocate heap memory block of size %d", data_size, 0, 0);
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated heap memory block 0x%x of size %d (%s, %d)",
                            data_size, data_ptr, filename, line);

    memset(data_ptr, 0x00, data_size);
  }

  return data_ptr;
} /* dsatutil_validate_memory_err_fatal */

/*===========================================================================
FUNCTION  DSATUTIL_FREE_MEMORY

DESCRIPTION
  Frees a block of memory from Modem Heap memory

DEPENDENCIES
  FEATURE_DATA_MODEM_HEAP

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_free_memory
(
  void *data_ptr
)
{
  DS_AT_MSG1_MED("modem_mem_free 0x%x:",(unsigned int)data_ptr);
  if (NULL != data_ptr)
  {
    dsat_modem_mem_free(data_ptr,MODEM_MEM_CLIENT_DATA);
    data_ptr = NULL;
  }
} /* dsatutil_free_memory() */

/*===========================================================================
FUNCTION  DSATUTIL_FREE_MEMORY_EXT

DESCRIPTION
  Frees a block of memory from Modem Heap memory and makes the ptr to NULL

DEPENDENCIES
  FEATURE_DATA_MODEM_HEAP

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_free_memory_ext
(
  void **data_ptr
)
{
  if (data_ptr != NULL && *data_ptr != NULL)
  {
    DS_AT_MSG1_MED("modem_mem_free 0x%x:",(unsigned int)*data_ptr);
    dsat_modem_mem_free(*data_ptr,MODEM_MEM_CLIENT_DATA);
    *data_ptr = NULL;
  }
} /* dsatutil_free_memory_ext() */

/*===========================================================================

FUNCTION DSATUTIL_GET_CMD_BUF

DESCRIPTION
  This function is a wrapper for ds_get_cmd_buf() API. It calls an error F3 in case of
  graceful handing and ERR_FATAL otherwise. The caller of this function must check
  for a NULL return value in case of graceful handing.

DEPENDENCIES
  None

RETURN VALUE
  Returns a pointer to a command buffer if avaiable or NULL in case of graceful handing
  if no command buffers are available.

SIDE EFFECTS
  None

===========================================================================*/

ds_cmd_type *dsatutil_get_cmd_buf
(
  size_t      payload_size, /* Number of bytes to allocate for cmd payload */
  const char *filename,
  uint32      line,
  boolean     graceful
)
{
  ds_cmd_type *cmd_buf = NULL;

  cmd_buf = ds_allocate_cmd_buf(payload_size);

  if ((payload_size > 0) && (NULL != cmd_buf) && (NULL == cmd_buf->cmd_payload_ptr))
  {
    ds_release_cmd_buf(&cmd_buf);
    cmd_buf = NULL;
  }

  if (NULL == cmd_buf)
  {
    if (TRUE == graceful)
    {
      DS_AT_MSG_SPRINTF_3_ERROR("Failed to get DS command buffer of payload size %d (%s, %d)",
                                payload_size, filename, line);
    }
    else
    {
      ERR_FATAL("Failed to get DS command buffer of payload size %d", payload_size, 0, 0);
    }
  }
  else
  {
    DS_AT_MSG_SPRINTF_4_MED("Allocated DS command buffer 0x%x of payload size %d (%s, %d)",
                            payload_size, cmd_buf, filename, line);

    if (NULL != cmd_buf->cmd_payload_ptr)
    {
      memset(cmd_buf->cmd_payload_ptr, 0x0, payload_size);
    }
  }
  
  return cmd_buf;
} /* dsatutil_get_cmd_buf() */

/*===========================================================================

FUNCTION DSATUTIL_GET_CMD_PAYLOAD_PTR

DESCRIPTION
  This function is used to get command payload pointer using command pointer.
  It calls ERR_FATAL when either command pointer or command payload pointer is NULL.

DEPENDENCIES
  None

RETURN VALUE
  Returns command payload pointer.

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_cmd_payload_ptr
(
  ds_cmd_type *cmd_ptr
)
{
  void *cmd_payload_ptr = NULL;

  if (NULL != cmd_ptr)
  {
    cmd_payload_ptr = cmd_ptr->cmd_payload_ptr;
  }

  if (NULL == cmd_payload_ptr)
  {
    ERR_FATAL("Invalid pointers: command = %p, payload = %p",
              cmd_ptr, cmd_payload_ptr, 0);
  }

  return cmd_payload_ptr;
} /* dsatutil_get_cmd_payload_ptr() */

/*===========================================================================
FUNCTION  DSATUTIL_MEMSCPY

DESCRIPTION
  
 This function will validate and copy data from src pointer to dest pointer.
  
DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

size_t dsatutil_memscpy
(
  void        *dst,
  size_t      dst_size,
  const void  *src,
  size_t      src_size

)
{
  if(dst_size == 0 || src_size == 0 || dst == NULL || src == NULL)
  {
    return 0;
  }
  else
  {
    return memscpy( dst,dst_size, src,src_size);
  }
} /* dsatutil_free_memory() */

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_VALS

DESCRIPTION
  This function will take basic table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT_BASIC_N_C_IDX:
      resp =  &dsat_basic_vals.dsat_amp_c_val;
      break;
    case DSAT_BASIC_N_D_IDX:
      resp = &dsat_basic_vals.dsat_amp_d_val;
      break;
    case DSAT_BASIC_N_E_IDX:
      resp = &dsat_basic_vals.dsat_amp_e_val;
      break;
   case DSAT_BASIC_N_F_IDX:
     resp = &dsat_basic_vals.dsat_amp_f_val;
     break;
   case DSAT_BASIC_N_S_IDX:
     resp = &dsat_basic_vals.dsat_amp_s_val;
     break;
   case DSAT_BASIC_N_W_IDX:
     resp = &dsat_basic_vals.dsat_amp_w_val;
     break;
   case DSAT_BASIC_E_IDX:
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
     resp = &dsat_basic_vals.dsat_e_val_multi_port[0];
#else
     resp = &dsat_basic_vals.dsat_e_val;
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
     break;
   case DSAT_BASIC_L_IDX:
     resp = &dsat_basic_vals.dsat_l_val;
     break;
   case DSAT_BASIC_M_IDX:
     resp = &dsat_basic_vals.dsat_m_val;
     break;
   case DSAT_BASIC_Q_IDX :
     resp = &dsat_basic_vals.dsat_q_val;
     break;
   case DSAT_BASIC_V_IDX:
     resp = &dsat_basic_vals.dsat_v_val;
     break;
   case DSAT_BASIC_X_IDX:
     resp =  &dsat_basic_vals.dsat_x_val;
     break;
   case DSAT_BASIC_Z_IDX:
     resp = &dsat_basic_vals.dsat_z_val;
     break;
   case DSAT_BASIC_DS_Q_IDX:
     resp = &dsat_basic_vals.dsat_slash_q_val;
     break;
   case DSAT_BASIC_DS_S_IDX:
     resp = &dsat_basic_vals.dsat_slash_s_val;
     break;
   case DSAT_BASIC_DS_V_IDX :
     resp = &dsat_basic_vals.dsat_slash_v_val;
     break;
   default:
    resp = NULL;
    break;
 }
 return resp;
}/*dsatutil_get_basic_vals*/
/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_ACTION_VALS

DESCRIPTION
  This function will take basic action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_action_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  if(cmd_id == DSAT_BASIC_ACT_O_IDX)
  {
    resp = &dsat_basic_act_vals.dsat_o_val;
  }
  return resp;
}/*dsatutil_get_basic_action_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_SREG_VALS

DESCRIPTION
  This function will take basic sreg table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_sreg_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT_SREG_S0_IDX:
      resp =  &dsat_basic_sreg_vals.dsat_s0_val;
      break;
    case DSAT_SREG_S2_IDX:
      resp = &dsat_basic_sreg_vals.dsat_s2_val;
      break;
    case DSAT_SREG_S3_IDX:
      resp = &dsat_basic_sreg_vals.dsat_s3_val;
      break;
   case DSAT_SREG_S4_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s4_val;
     break;
   case DSAT_SREG_S5_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s5_val;
     break;
   case DSAT_SREG_S6_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s6_val;
     break;
   case DSAT_SREG_S7_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s7_val;
     break;
   case DSAT_SREG_S8_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s8_val;
     break;
   case DSAT_SREG_S9_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s9_val;
     break;
   case DSAT_SREG_S10_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s10_val;
     break;
   case DSAT_SREG_S11_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s11_val;
     break;
   case DSAT_SREG_S30_IDX:
     resp =  &dsat_basic_sreg_vals.dsat_s30_val;
     break;
   case DSAT_SREG_S103_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s103_val;
     break;
   case DSAT_SREG_S104_IDX:
     resp = &dsat_basic_sreg_vals.dsat_s104_val;
     break;
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_basic_sreg_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_EXT_VALS

DESCRIPTION
  This function will take basic extended table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_ext_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT_EXT_FCLASS_IDX:
      resp =  &dsat_basic_ext_vals.dsat_fclass_val;
      break;
    case DSAT_EXT_ICF_IDX:
      resp = &dsat_basic_ext_vals.dsat_icf_val[0];
      break;
    case DSAT_EXT_IFC_IDX:
      resp = &dsat_basic_ext_vals.dsat_ifc_val[0];
      break;
   case DSAT_EXT_IPR_IDX:
     resp = &dsat_basic_ext_vals.dsat_ipr_val;
     break;
   case DSAT_EXT_DR_IDX:
     resp = &dsat_basic_ext_vals.dsat_dr_val;
     break;
   case DSAT_EXT_DS_IDX:
     resp = &dsat_basic_ext_vals.dsat_ds_val[0];
     break;
   case DSAT_EXT_CMEE_IDX:
     resp = &dsat_basic_ext_vals.dsat_cmee_val;
     break;
#if defined(FEATURE_DSAT_ETSI_MODE)
   case DSAT_EXT_WS46_IDX:
     resp = &dsat_basic_ext_vals.dsat_ws46_val;
     break;
#endif /* defined(FEATURE_DSAT_ETSI_MODE)*/
#ifdef FEATURE_DSAT_TEST_32ARG
   case DSAT_EXT_TST32_IDX:
     resp = &dsat_basic_ext_vals.dsat_tst32_val[0];
     break;
#endif /*FEATURE_DSAT_TEST_32ARG*/
   case DSAT_EXT_CFUN_IDX:
     resp = &dsat_basic_ext_vals.dsat_cfun_val[0];
     break;

   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_basic_ext_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASIC_VEND_VALS

DESCRIPTION
  This function will take basic vender table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_basic_vend_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    case DSAT_VENDOR_QCSIMSTAT_IDX:
      resp =  &dsat_basic_vend_ss_vals.dsat_qcsimstat_val;
      break;
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
    case DSAT_VENDOR_QCPBMPREF_IDX:
      resp = &dsat_basic_vend_ss_vals.dsat_qcpbmpref_val;
      break;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)*/
#ifdef FEATURE_DSAT_DEV_CMDS
    case DSAT_VENDOR_CREG_IDX:
      if (subs < DSAT_MS_MAX_SUBS)
      {
        resp = &dsat_basic_vend_ms_vals[subs].dsat_qccreg_val;
      }
      break;
#endif/*FEATURE_DSAT_DEV_CMDS*/
#ifdef FEATURE_ETSI_SMS
   case DSAT_VENDOR_QCCNMI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_basic_etsi_cmn_ms_vals[subs].dsat_cnmi_val[0];
     }
     break;
#endif /*FEATURE_ETSI_SMS*/
#ifdef FEATURE_DSAT_DEV_CMDS
   case DSAT_VENDOR_CNTI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_basic_vend_ms_vals[subs].dsat_qccnti_val;
     }
     break;
#endif /*FEATURE_DSAT_DEV_CMDS*/
#endif /*FEATURE_DSAT_ETSI_MODE*/
   case DSAT_VENDOR_QCDMR_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcdmr_val;
     break;
   case DSAT_VENDOR_QCDNSP_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcdnspri_val[0];
     break;
   case DSAT_VENDOR_QCDNSS_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcdnspri_val[0];
     break;
   case DSAT_VENDOR_QCTER_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcter_val;
     break;
   case DSAT_VENDOR_QCSIMAPP_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcsimapp_val;
     break;
   case DSAT_VENDOR_QCPDPP_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcpdpp_val[0];
     break;
   case DSAT_VENDOR_QCPDPLT_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcpdplt_val;
     break;
#ifdef FEATURE_DATA_TE_MT_PDP
   case DSAT_VENDOR_QCGANSM_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcgansm_val;
     break;
#endif /*FEATURE_DATA_TE_MT_PDP*/
#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
   case DSAT_VENDOR_QCSCFTEST_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcscftest_val[0];
     break;
#endif /*  FEATURE_DATA_UCSD_SCUDIF_TEST */
#ifdef FEATURE_TTY
   case DSAT_VENDOR_QCCTM_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcctm_val;
     break;
#endif/* FEATURE_TTY */
#ifdef FEATURE_HDR
  case DSAT_VENDOR_PREFMODE_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_prefmode_val;
     break;
#endif /* FEATURE_HDR*/
#ifdef FEATURE_DSAT_ETSI_MODE 
  case DSAT_VENDOR_DSCI_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_dsci_val;
     break;
#endif /*FEATURE_DSAT_ETSI_MODE*/
#ifdef FEATURE_ECALL_APP 
   case DSAT_VENDOR_ECALL_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_ecall_val[0];
     break;
#endif /*FEATURE_ECALL_APP*/
#ifdef FEATURE_DSAT_ETSI_DATA
   case DSAT_VENDOR_QCDEFPROF_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcdefprof_val[0];
     break;
#endif /*FEATURE_DSAT_ETSI_DATA */
#ifdef FEATURE_DSAT_LTE
   case DSAT_VENDOR_QCDRX_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcdrx_val;
     break;
#endif/* FEATURE_DSAT_LTE*/	 
   case DSAT_VENDOR_MODE_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_mode_val;
     break;
     case DSAT_VENDOR_QCRCIND_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcrcind_val;
     break;
   case DSAT_VENDOR_QCNSP_IDX:
     resp = &dsat_basic_vend_ss_vals.dsat_qcnsp_val[0];
     break;
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_basic_vend_vals*/


/*===========================================================================
FUNCTION  DSATUTIL_GET_ETSI_EXT_ACT_VALS

DESCRIPTION
  This function will take etsi ext action table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_etsi_ext_act_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
#ifdef FEATURE_DATA_ETSI_PIN 
    case DSATETSI_EXT_ACT_CPIN_ETSI_IDX:
      if (subs < DSAT_MS_MAX_SUBS)
      {
        resp =  &dsat_etsi_ext_act_ms_vals[subs].dsat_cpin_val[0];
      }
      break;
#endif /*FEATURE_DATA_ETSI_PIN*/
    case DSATETSI_EXT_ACT_CMEC_ETSI_IDX:
      resp = &dsat_etsi_ext_act_ss_vals.dsat_cmec_val[0];
      break;
    case DSATETSI_EXT_ACT_CIND_ETSI_IDX:
      if (subs < DSAT_MS_MAX_SUBS)
      {
        resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cind_val[0];
      }
      break;

   case DSATETSI_EXT_ACT_CMER_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_cmer_val[0];
     break;
#ifdef FEATURE_DSAT_ETSI_DATA
   case DSATETSI_EXT_ACT_CGATT_ETSI_IDX:
      if (subs < DSAT_MS_MAX_SUBS)
      {
        resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cgatt_val;
      }
     break;
   case DSATETSI_EXT_ACT_CGACT_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_cgact_val;
     break;
#endif /*#FEATURE_DSAT_ETSI_DATA*/
#ifdef FEATURE_ETSI_PBM
   case DSATETSI_EXT_ACT_CPBS_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_cpbs_val;
     break;
#endif/* FEATURE_ETSI_PBM*/

#ifdef FEATURE_ETSI_SMS
   case DSATETSI_EXT_ACT_CPMS_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cpms_val[0];
     }
     break;
     /* CNMI belongs into CTAB and ETSICTAB,Both have same variable and it is already allocated 
        in vender Structure */
   case DSATETSI_EXT_ACT_CNMI_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp =  &dsat_basic_etsi_cmn_ms_vals[subs].dsat_cnmi_val[0];
     }
     break;
   case DSATETSI_EXT_ACT_CMMS_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cmms_val;
     }
     break;
#endif /* FEATURE_ETSI_SMS*/

#ifdef FEATURE_DATA_GCSD_FAX
   case DSATETSI_EXT_ACT_FTS_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_fts_val;
     break;

   case DSATETSI_EXT_ACT_FRS_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_frs_val;
     break;


   case DSATETSI_EXT_ACT_FTH_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_fth_val;
     break;
     
   case DSATETSI_EXT_ACT_FRH_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_frh_val;
     break;

   case DSATETSI_EXT_ACT_FTM_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_ftm_val;
     break;

   case DSATETSI_EXT_ACT_FRM_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_frm_val;
     break;
#endif/*FEATURE_DATA_GCSD_FAX*/
     
   case DSATETSI_EXT_ACT_CCUG_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_ccug_val[0];
     break;

   case DSATETSI_EXT_ACT_COPS_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cops_val[0];
     }
     break;

   case DSATETSI_EXT_ACT_CUSD_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_cusd_val[0];
     break;
   case DSATETSI_EXT_ACT_CAOC_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_caoc_val;
     break;

   case DSATETSI_EXT_ACT_CCWA_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_ccwa_val;
     }
     break;
#ifdef FEATURE_MMGSDI
   case DSATETSI_EXT_ACT_CPOL_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cpol_val[0];
     }
     break;
#endif /* FEATURE_MMGSDI*/
  case DSATETSI_EXT_ACT_CPLS_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cpls_val;
     }
     break;

  case DSATETSI_EXT_ACT_CTZR_ETSI_IDX:
     resp = &dsat_etsi_ext_act_ss_vals.dsat_ctzr_val;
     break;

   case DSATETSI_EXT_ACT_CTZU_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_ctzu_val;
     break;

   case DSATETSI_EXT_ACT_CLIP_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_clip_val;
     break;

   case DSATETSI_EXT_ACT_COLP_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_colp_val;
     break;

   case DSATETSI_EXT_ACT_CDIP_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_cdip_val;
     break;

   case DSATETSI_EXT_ACT_CLIR_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_clir_val;
     break;

#ifdef FEATURE_DATA_TE_MT_PDP
   case DSATETSI_EXT_ACT_CGANS_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_cgans_val[0];
     break;
#endif /* FEATURE_DATA_TE_MT_PDP*/
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
   case DSATETSI_EXT_ACT_CQI_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_cqi_val;
     break;
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT*/
   case DSATETSI_EXT_ACT_CSDF_ETSI_IDX:
     resp =  &dsat_etsi_ext_act_ss_vals.dsat_csdf_val;
     break;

   case DSATETSI_EXT_ACT_CEN_ETSI_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_act_ms_vals[subs].dsat_cen_val;
     }
     break;

   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_etsi_ext_act_vals*/


/*===========================================================================
FUNCTION  DSATUTIL_GET_ETSI_EXT_VALS

DESCRIPTION
  This function will take etsi ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_etsi_ext_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
    case DSATETSI_EXT_CBST_IDX:
      resp =  &dsat_etsi_ext_ss_vals.dsat_cbst_val[0];
      break;
    case DSATETSI_EXT_CRLP_IDX:
      resp =  &dsat_etsi_ext_ss_vals.dsat_crlp_val[0][0];
      break;

    case DSATETSI_EXT_CV120_IDX:
      resp =  &dsat_etsi_ext_ss_vals.dsat_cv120_val[0];
      break;
    case DSATETSI_EXT_CHSN_IDX:
      resp =  &dsat_etsi_ext_ss_vals.dsat_chsn_val[0];
      break;
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)*/
    case DSATETSI_EXT_CSSN_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cssn_val[0];

    case DSATETSI_EXT_CREG_IDX:
      if (subs < DSAT_MS_MAX_SUBS)
      {
        resp = &dsat_etsi_ext_ms_vals[subs].dsat_creg_val;
      }
      break;

   case DSATETSI_EXT_CGREG_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_ms_vals[subs].dsat_cgreg_val;
     }
     break;
#ifdef FEATURE_DSAT_LTE
   case DSATETSI_EXT_CEREG_IDX:
     if (subs < DSAT_MS_MAX_SUBS)
     {
       resp = &dsat_etsi_ext_ms_vals[subs].dsat_cereg_val;
     }
     break;
#endif /*FEATURE_DSAT_LTE*/
   case DSATETSI_EXT_CSCS_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_cscs_val;
     break;

   case DSATETSI_EXT_CSTA_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_csta_val;
     break;

   case DSATETSI_EXT_CR_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cr_val;
       break;

   case DSATETSI_EXT_CRC_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_crc_val;
       break;
#ifdef FEATURE_DSAT_ETSI_DATA
   case DSATETSI_EXT_CGDCONT_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_cgdcont_val[0];
     break;
#ifdef FEATURE_SECONDARY_PDP
   case  DSATETSI_EXT_CGDSCONT_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_cgdscont_val[0];
      break;
  case DSATETSI_EXT_CGTFT_IDX:
    resp = &dsat_etsi_ext_ss_vals.dsat_cgtft_val[0];
    break;
#endif/*FEATURE_SECONDARY_PDP*/
#ifdef FEATURE_DATA_WCDMA_PS
   case  DSATETSI_EXT_CGEQREQ_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_cgeqreq_val[0];
     break;
  case DSATETSI_EXT_CGEQMIN_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_cgeqmin_val[0];
     break;
#endif /*# FEATURE_DATA_WCDMA_PS*/

#ifdef FEATURE_DSAT_LTE
    case  DSATETSI_EXT_CGEQOS_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgeqos_val[0];
      break;
#endif /* FEATURE_DSAT_LTE*/
    case  DSATETSI_EXT_CGQREQ_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgqreq_val[0];
      break;
    case  DSATETSI_EXT_CGQMIN_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgqmin_val[0];
      break;
    case  DSATETSI_EXT_CGEREP_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgerep_val[0];
      break;
    case  DSATETSI_EXT_CGDATA_IDX:
     resp = &dsat_etsi_ext_ss_vals.dsat_cgdata_val;
     break;
#if defined(FEATURE_GSM)
    case  DSATETSI_EXT_CGCLASS_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgclass_val;
      break;
#endif /* FEATURE_GSM*/
#ifdef FEATURE_DATA_TE_MT_PDP
    case  DSATETSI_EXT_CGAUTO_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgauto_val;
      break;
#endif /* FEATURE_DATA_TE_MT_PDP*/
    case  DSATETSI_EXT_CGPIAF_IDX:
      resp = &dsat_etsi_ext_ss_vals.dsat_cgpiaf_val[0];
      break;
#endif /* FEATURE_DSAT_ETSI_DATA*/

#ifdef FEATURE_ETSI_SMS
#ifdef FEATURE_ETSI_SMS_PS
    case  DSATETSI_EXT_CGSMS_IDX:
      if (subs < DSAT_MS_MAX_SUBS)
      {
        resp = &dsat_etsi_ext_ms_vals[subs].dsat_cgsms_val;
      }
      break;
#endif  /*FEATURE_ETSI_SMS_PS*/
    case  DSATETSI_EXT_CSMS_IDX:
       if (subs < DSAT_MS_MAX_SUBS)
       {
         resp = &dsat_etsi_ext_ms_vals[subs].dsat_csms_val;
       }
       break;
    case  DSATETSI_EXT_CMGF_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cmgf_val;
       break;
    case  DSATETSI_EXT_CSAS_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_csas_val;
       break;
    case  DSATETSI_EXT_CRES_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cres_val;
       break;
    case  DSATETSI_EXT_CSCA_IDX:
       if (subs < DSAT_MS_MAX_SUBS)
       {
         resp = &dsat_etsi_ext_ms_vals[subs].dsat_csca_val[0];
       }
       break;
    case  DSATETSI_EXT_CSMP_IDX:
       if (subs < DSAT_MS_MAX_SUBS)
       {
         resp = &dsat_etsi_ext_ms_vals[subs].dsat_csmp_val[0];
       }
       break;
    case  DSATETSI_EXT_CSDH_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_csdh_val;
       break;
#ifdef FEATURE_ETSI_SMS_CB
    case  DSATETSI_EXT_CSCB_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cscb_val[0];
       break;
#endif/* FEATURE_ETSI_SMS_CB*/
#endif /*FEATURE_ETSI_SMS*/


#ifdef FEATURE_DATA_GCSD_FAX
    case  DSATETSI_EXT_FDD_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_fdd_val;
       break;
    case  DSATETSI_EXT_FAR_IDX :
       resp = &dsat_etsi_ext_ss_vals.dsat_far_val;
       break;
    case  DSATETSI_EXT_FCL_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_fcl_val;
       break;
    case  DSATETSI_EXT_FIT_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_fit_val[0];
       break;       
#endif/*FEATURE_DATA_GCSD_FAX*/
#if defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)
    case  DSATETSI_EXT_ES_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_es_val[0];
       break; 
    case  DSATETSI_EXT_ESA_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_esa_val[0];
       break; 
#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA*/ 
    case  DSATETSI_EXT_CMOD_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cmod_val;
       break;
#ifdef FEATURE_DSAT_LTE
    case  DSATETSI_EXT_CEMODE_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cemode_val;
       break;
#endif /*FEATURE_DSAT_LTE*/
    case  DSATETSI_EXT_CVHU_IDX:
       resp = &dsat_etsi_ext_ss_vals.dsat_cvhu_val;
       break;
   default:
    resp = NULL;
    break;
 }
 return resp;

}/*dsatutil_get_etsi_ext_vals*/
#ifdef FEATURE_DATA_IS707 
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_EXT_VALS

DESCRIPTION
  This function will take CDMA(707) ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_ext_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT707_EXT_PARA_EB_IDX:
      resp =  &dsat707_ext_ss_vals.dsat707_eb_val[0];
      break;
    case DSAT707_EXT_PARA_EFCS_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_efcs_val;
      break;
    case DSAT707_EXT_PARA_ER_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_er_val;
      break;
    case DSAT707_EXT_PARA_ESR_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_esr_val;
      break;
    case DSAT707_EXT_PARA_ETBM_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_etbm_val[0];
      break;
    case DSAT707_EXT_PARA_MA_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_ma_val[0];
      break;
    case DSAT707_EXT_PARA_MR_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_mr_val;
      break;
    case DSAT707_EXT_PARA_MS_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_ms_val[0];
      break;
    case DSAT707_EXT_PARA_MV18R_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_mv18r_val;
      break;
    case DSAT707_EXT_PARA_MV18S_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_mv18s_val[0];
      break;
    case DSAT707_EXT_PARA_CXT_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cxt_val;
      break;
    case DSAT707_EXT_PARA_CDR_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cdr_val;
      break;
    case DSAT707_EXT_PARA_CDS_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cds_val[0];
      break;
    case DSAT707_EXT_PARA_CFC_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cfc_val;
      break;
    case DSAT707_EXT_PARA_CFG_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cfg_val[0];
      break;
    case DSAT707_EXT_PARA_CQD_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cqd_val;
      break;
    case DSAT707_EXT_PARA_CRC_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_crc_val;
      break;
    case DSAT707_EXT_PARA_CMUX_IDX:
 #ifdef FEATURE_DS_IS707A_CMUX
      resp = &dsat707_ext_ss_vals.dsat707_cmux_val[0];
 #else
      resp = &dsat707_ext_ss_vals.dsat707_cmux_val;
 #endif   /*FEATURE_DS_IS707A_CMUX*/
      break;
    case DSAT707_EXT_PARA_CTA_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_cta_val;
      break;
    case DSAT707_EXT_PARA_ILRR_IDX:
      resp = &dsat707_ext_ss_vals.dsat707_ilrr_val;
      break;
      
    default:
     resp = NULL;
     break;
  }
  return resp;

}/*dsatutil_get_707_ext_vals*/
#ifdef FEATURE_DSAT_CDMA_PIN 
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_ACT_VALS

DESCRIPTION
  This function will take CDMA(707) ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_act_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  if(DSAT707_EXT_ACT_QCPIN_IDX == cmd_id)
  {
    resp = &dsat707_ext_act_cmn_ss_vals.dsat707_cpin_val[0];
  }
  return resp;
} /*dsatutil_get_707_act_vals*/
#endif /* FEATURE_DSAT_CDMA_PIN */
#ifdef FEATURE_DATA_IS707_ASYNC 
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_FAX_VALS

DESCRIPTION
  This function will take CDMA(707) ext table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_fax_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT707_FAX_FAA_IDX:
      resp =  &dsat707_fax_ss_vals.dsat707_faa_val;
      break;
    case DSAT707_FAX_FAP_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fap_val[0];
      break;
    case DSAT707_FAX_FBO_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fbo_val;
      break;
    case DSAT707_FAX_FBU_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fbu_val;
      break;
    case DSAT707_FAX_FCQ_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fcq_val[0];
      break;
    case DSAT707_FAX_FCC_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fcc_val[0];
      break;
    case DSAT707_FAX_FCR_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fcr_val;
      break;
    case DSAT707_FAX_FCT_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fct_val;
      break;
    case  DSAT707_FAX_FEA_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fea_val;
      break;
    case DSAT707_FAX_FFC_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_ffc_val[0];
      break;
    case DSAT707_FAX_FHS_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fhs_val;
      break;
    case DSAT707_FAX_FIE_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fie_val;
      break;
    case DSAT707_FAX_FIP_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fip_val;
      break;
    case DSAT707_FAX_FIS_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fis_val[0];
      break;
    case DSAT707_FAX_FLI_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fli_val[0];
      break;
    case DSAT707_FAX_FLO_IDX:
      resp = &dsat707_fax_ss_vals.dsat_flo_val;
      break;
    case DSAT707_FAX_FLP_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_flp_val;
      break;
    case DSAT707_FAX_FMS_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fms_val;
      break;
    case DSAT707_FAX_FNR_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fnr_val[0];
      break;
    case DSAT707_FAX_FNS_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fns_val[0];
      break;
    case DSAT707_FAX_FPA_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fpa_val[0];
      break;
    case DSAT707_FAX_FPI_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fpi_val[0];
      break;
    case DSAT707_FAX_FPP_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fpp_val;
      break;
    case DSAT707_FAX_FPR_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fpr_val;
      break;
    case DSAT707_FAX_FPS_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fps_val;
      break;
    case DSAT707_FAX_FPW_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fpw_val[0];
      break;
    case DSAT707_FAX_FRQ_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_frq_val[0];
      break;
    case DSAT707_FAX_FRY_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fry_val;
      break;
    case DSAT707_FAX_FSA_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fsa_val[0];
      break;
    case DSAT707_FAX_FSP_IDX:
      resp = &dsat707_fax_ss_vals.dsat707_fsp_val;
      break;
    default:
     resp = NULL;
     break;
  }
  return resp;
}/*dsatutil_get_707_fax_vals*/
#endif /* FEATURE_DATA_IS707_ASYNC */

#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_AT_TEST_ONLY 
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_HDR_VALS

DESCRIPTION
  This function will take  HDR(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_hdr_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT707_HDR_QCHDRT_IDX:
      resp =  &dsat707_hdr_ss_vals.dsat707_qchdrt_val;
      break;
    case DSAT707_HDR_QCHDRC_IDX:
      resp =  &dsat707_hdr_ss_vals.dsat707_qchdrc_val;
      break;
    case DSAT707_HDR_QCHDRR_IDX:
      resp =  &dsat707_hdr_ss_vals.dsat707_qchdrr_val;
      break;
    case DSAT707_HDR_QCHDRET_IDX:
      resp =  &dsat707_hdr_ss_vals.dsat707_qchdret_val;
      break;
    default:
     resp = NULL;
     break;
  }
  return resp;
}/*dsatutil_get_707_hdr_vals*/
#endif /* FEATURE_HDR_AT_TEST_ONLY */
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_MIP_VALS

DESCRIPTION
  This function will take  MIP table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_mip_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;
  /*DS 707 Will own this variable. ATCoP will use get/set api*/
  return resp;
}/*dsatutil_get_707_mip_vals*/
#endif /* FEATURE_DS_MOBILE_IP*/
#ifdef FEATURE_CDMA_SMS
/*===========================================================================
FUNCTION  DSATUTIL_GET_707_SMS_VALS

DESCRIPTION
  This function will take  SMS(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_sms_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT707_SMS_QCNMI_IDX:
      resp =  &dsat707_sms_ss_vals.dsat707sms_qcnmi_val[0];
      break;
    case DSAT707_SMS_QCSMP_IDX:
      resp =  &dsat707_sms_ss_vals.dsat707sms_qcsmp_val[0];
      break;
    case DSAT707_SMS_QCPMS_IDX:
      resp =  &dsat707_sms_ss_vals.dsat707sms_qcpms_val[0];
      break;
    case DSAT707_SMS_QCMGF_IDX:
      resp =  &dsat707_sms_ss_vals.dsat707sms_qcmgf_val;
      break;
    case DSAT707_SMS_HSMSSS_IDX:
      resp =  &dsat707_sms_ss_vals.dsat707sms_smsss_val[0];
      break;
    default:
     resp = NULL;
     break;
  }
  return resp;
}/*dsatutil_get_707_sms_vals*/
#endif /* FEATURE_CDMA_SMS*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_707_VEND_PARA_VALS

DESCRIPTION
  This function will take  Vender Parameter(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_vend_para_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT707_VEND_PARA_QCMDR_IDX:
      resp =  &dsat707_vend_para_ss_vals.dsat707_qcmdr_val;
      break;
#ifdef  FEATURE_IS2000_R_SCH 
    case DSAT707_VEND_PARA_QCSCRM_IDX:
      resp =  &dsat707_vend_para_ss_vals.dsat707_qcscrm_val;
      break;
    case DSAT707_VEND_PARA_QCTRTL_IDX:
      resp =  &dsat707_vend_para_ss_vals.dsat707_qctrtl_val;
      break;
#endif /* FEATURE_IS2000_R_SCH */
    case DSAT707_VEND_PARA_QCVAD_IDX:
      resp =  &dsat707_vend_para_ss_vals.dsat707_qcvad_val;
      break;
#ifdef FEATURE_IS2000_REL_A
    case DSAT707_VEND_PARA_QCQOSPRI_IDX:
      resp =  &dsat707_vend_para_ss_vals.dsat707_qcqospri_val;
      break;
#endif /* FEATURE_IS2000_REL_A*/
#ifdef FEATURE_IS2000_CHS
    case DSAT707_VEND_PARA_QCCHS_IDX:
      resp =  &dsat707_vend_para_ss_vals.dsat707_qcchs_val;
      break;
#endif /*FEATURE_IS2000_CHS*/
    default:
     resp = NULL;
     break;
  }
  return resp;
}/*dsatutil_get_707_vend_para_vals*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_707_VEND_ACT_VALS

DESCRIPTION
  This function will take  Vender Action(707) table command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_707_vend_act_vals
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

  switch(cmd_id)
  {
    case DSAT707_VEND_ACT_QCOTC_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_qcotc_val[0];
      break;
#ifdef FEATURE_DSAT_CDMA_PIN 
    case DSAT707_VEND_ACT_CPIN_IDX:
      resp = &dsat707_ext_act_cmn_ss_vals.dsat707_cpin_val[0];
      break;
#endif/* FEATURE_DSAT_CDMA_PIN */
#if defined(FEATURE_DSAT_SP_CMDS)
    case DSAT707_VEND_ACT_SPSPC_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat_spc_val;
      break;
    case DSAT707_VEND_ACT_SPROAM_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_roam_pref_val;
      break;
    case DSAT707_VEND_ACT_SPUNLOCK_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat_unlock_val;
      break;
    case DSAT707_VEND_ACT_SPLOCK_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat_lock_val;
      break;
    case DSAT707_VEND_ACT_SPLOCKCHG_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat_lock_chg_val[0];
      break;
#endif /* FEATURE_DSAT_SP_CMDS */

#if defined(FEATURE_DSAT_GPS_CMDS)
    case DSAT707_VEND_ACT_SPNMEA_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_gps_nmea_config_val[0];
      break;
    case DSAT707_VEND_ACT_SPLOCATION_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_sp_lcs_state;
      break;
    case DSAT707_VEND_ACT_ACT_GPS_MODE_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_gps_mode_val;
      break;
    case DSAT707_VEND_ACT_ACT_GPS_PORT_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_gps_port_val;
      break;
    case DSAT707_VEND_ACT_ACT_PDE_TRS_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_pde_trs_val;
      break;
    case DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_gps_pdeaddr_val[0];
      break;
    case DSAT707_VEND_ACT_ACT_INIT_MASK_IDX:
      resp =  &dsat707_vend_act_ss_vals.dsat707_gps_init_mask_val;
      break; 
#endif /*defined(FEATURE_DSAT_GPS_CMDS)*/
    default:
     resp = NULL;
     break;
  }
  return resp;
}/*dsatutil_get_707_vend_act_vals*/
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================
FUNCTION  DSATUTIL_GET_VAL_FROM_CMD_ID

DESCRIPTION
  This function will take valid command ID and will return val pointer corresponding .
  to the command ID.
DEPENDENCIES
  None

RETURNS
  val pointer

SIDE EFFECTS
  None

===========================================================================*/

void* dsatutil_get_val_from_cmd_id
(
  dsat_num_item_type       cmd_id,
  dsat_memory_subs_type    subs
)
{
  void *resp = NULL;

#ifdef FEATURE_DATA_IS707 
 /* 1X command tables*/
  if(cmd_id < DSAT707_EXT_PARA_MAX_IDX)
  {
    resp = dsatutil_get_707_ext_vals(cmd_id,dsat_get_current_1x_subs_id());
  }
  else if( DSAT707_EXT_ACT_CDV_IDX <= cmd_id &&
                                  cmd_id < DSAT707_EXT_ACT_MAX_IDX)
  {
#ifdef FEATURE_DSAT_CDMA_PIN 
   resp = dsatutil_get_707_act_vals(cmd_id,dsat_get_current_1x_subs_id());
#endif /* FEATURE_DSAT_CDMA_PIN */
  }
  else if(DSAT707_FAX_FAA_IDX <= cmd_id &&
                                cmd_id < DSAT707_FAX_MAX_IDX)
  {
#ifdef FEATURE_DATA_IS707_ASYNC
   resp = dsatutil_get_707_fax_vals(cmd_id,dsat_get_current_1x_subs_id());
#endif /* FEATURE_DATA_IS707_ASYNC */
  }
  else if(  DSAT707_HDR_QCHDRT_IDX <= cmd_id && 
                          cmd_id < DSAT707_HDR_MAX_IDX)
  {
#ifdef FEATURE_HDR
#ifdef FEATURE_HDR_AT_TEST_ONLY 
   resp = dsatutil_get_707_hdr_vals(cmd_id,dsat_get_current_1x_subs_id());
#endif /* FEATURE_HDR_AT_TEST_ONLY */
#endif /* FEATURE_HDR */
  }
#ifdef FEATURE_DS_MOBILE_IP
  else if( DSAT707_MIP_QCMIP_IDX <= cmd_id &&
                                 cmd_id < DSAT707_MIP_MAX_IDX)
  {
    resp = dsatutil_get_707_mip_vals(cmd_id,dsat_get_current_1x_subs_id());
  }
#endif /*FEATURE_DS_MOBILE_IP*/
  else if( DSAT707_PSTATS_QCRLPD_IDX <= cmd_id &&
                                   cmd_id < DSAT707_PSTATS_MAX_IDX)
  {
#ifdef FEATURE_DS_PSTATS
     /*PSTATE table doesnt have val pointes*/
    return resp;
#endif /* FEATURE_DS_PSTATS */
  }
#ifdef FEATURE_CDMA_SMS
  else if( DSAT707_SMS_QCNMI_IDX <= cmd_id && 
                                     cmd_id < DSAT707_SMS_MAX_IDX)
  {
    resp = dsatutil_get_707_sms_vals(cmd_id,dsat_get_current_1x_subs_id());
  }
#endif/* FEATURE_CDMA_SMS*/
  else if( DSAT707_VEND_PARA_QCPREV_IDX <= cmd_id && 
                                cmd_id < DSAT707_VEND_PARA_MAX_IDX)
  {
    resp = dsatutil_get_707_vend_para_vals(cmd_id,dsat_get_current_1x_subs_id());
  }
  else if( DSAT707_VEND_ACT_CPBR_IDX <= cmd_id && 
                          cmd_id < DSAT707_VEND_ACT_MAX_IDX)
  {
     resp = dsatutil_get_707_vend_act_vals(cmd_id,dsat_get_current_1x_subs_id());
  }
  else
#endif /* FEATURE_DATA_IS707   */
  if( DSAT_BASIC_N_C_IDX <= cmd_id && 
           cmd_id < DSAT_BASIC_MAX_IDX)
  {
    resp = dsatutil_get_basic_vals(cmd_id,DSAT_MS_FIRST_SUBS);
  }
  else if( DSAT_BASIC_ACT_D_IDX <= cmd_id && 
                cmd_id < DSAT_BASIC_ACT_MAX_IDX)
  {
    resp = dsatutil_get_basic_action_vals(cmd_id,DSAT_MS_FIRST_SUBS);
  }
  else if(  DSAT_SREG_S0_IDX <= cmd_id && 
                 cmd_id < DSAT_SREG_MAX_IDX )
  {
    resp = dsatutil_get_basic_sreg_vals(cmd_id,DSAT_MS_FIRST_SUBS);
  }
  else if( DSAT_EXT_FCLASS_IDX <= cmd_id && 
                cmd_id < DSAT_EXT_MAX_IDX)
  {
    resp = dsatutil_get_basic_ext_vals(cmd_id,DSAT_MS_FIRST_SUBS);
  }
  else if( DSAT_VENDOR_QCSIMSTAT_IDX <= cmd_id &&
                cmd_id < DSAT_VENDOR_MAX_IDX)
  {
    resp = dsatutil_get_basic_vend_vals(cmd_id,subs);
  }
#ifdef FEATURE_DSAT_ETSI_MODE
  /* ETSI command tables*/
  else if( DSATETSI_EXT_ACT_CSQ_ETSI_IDX <= cmd_id && 
                    cmd_id < DSATETSI_EXT_ACT_MAX_ETSI_IDX)
  {
    resp = dsatutil_get_etsi_ext_act_vals(cmd_id,subs);
  }
  else if( DSATETSI_EXT_CBST_IDX <= cmd_id && 
                    cmd_id < DSATETSI_EXT_MAX_IDX )
  {
    resp = dsatutil_get_etsi_ext_vals(cmd_id,subs);
  }
#endif /* FEATURE_DSAT_ETSI_MODE */
  else
  {
    DS_AT_MSG1_ERROR("Table not found for %d index",cmd_id);
    ASSERT(0);
  }
  return resp;
}/*dsatutil_get_val_from_cmd_id*/
/*===========================================================================
FUNCTION  DSATUTIL_DEFAULT_CONSTRUCTOR

DESCRIPTION
  This function will initialize the global variables.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatutil_default_constructor(void)
{
  dsat_memory_subs_type i;

  /* Initilize val pointers to default values */
  dsat_basic_vend_ss_vals.dsat_qcpdpp_val[2].string_item = qcpdpp_passw_val;
  dsat_basic_vend_ss_vals.dsat_qcpdpp_val[3].string_item = qcpdpp_uname_val;
 
  /* Initilize val pointers to static memory */
  dsat_etsi_ext_act_ss_vals.dsat_cusd_val[1].string_item = cusd_str_val;
  dsat_etsi_ext_act_ss_vals.dsat_cusd_val[2].string_item = cusd_dcs_val;
  dsat_etsi_ext_act_ss_vals.dsat_csdf_val = 1;
#ifdef FEATURE_DATA_TE_MT_PDP
  dsat_etsi_ext_act_ss_vals.dsat_cgans_val[1].string_item = cgans_l2p_type_val;
#endif /* FEATURE_DATA_TE_MT_PDP*/

  /* Initilize val pointers to static memory */
  dsat_etsi_ext_ss_vals.dsat_cgdcont_val[1].string_item = cgdcont_pdptype_val;
  dsat_etsi_ext_ss_vals.dsat_cgdcont_val[2].string_item = cgdcont_apn_val;
  dsat_etsi_ext_ss_vals.dsat_cgdcont_val[3].string_item = cgdcont_pdpaddr_val;
   
#ifdef FEATURE_SECONDARY_PDP
  dsat_etsi_ext_ss_vals.dsat_cgtft_val[3].string_item = cgtft_addr_subnet_val;
  dsat_etsi_ext_ss_vals.dsat_cgtft_val[5].string_item = cgtft_dst_port_val;
  dsat_etsi_ext_ss_vals.dsat_cgtft_val[6].string_item = cgtft_src_port_val;
  dsat_etsi_ext_ss_vals.dsat_cgtft_val[8].string_item = cgtft_tos_tclass_val;
#endif /* FEATURE_SECONDARY_PDP */

#ifdef FEATURE_DATA_WCDMA_PS
  dsat_etsi_ext_ss_vals.dsat_cgeqreq_val[8].string_item = cgeqreq_sdu_err_val;
  dsat_etsi_ext_ss_vals.dsat_cgeqreq_val[9].string_item = cgeqreq_res_biterr_val;
  
  dsat_etsi_ext_ss_vals.dsat_cgeqmin_val[8].string_item = cgeqmin_sdu_err_val;
  dsat_etsi_ext_ss_vals.dsat_cgeqmin_val[9].string_item =cgeqmin_res_biterr_val;
#endif /* FEATURE_DATA_WCDMA_PS */

  dsat_etsi_ext_ss_vals.dsat_cscb_val[1].string_item= cscb_mids_val;
  dsat_etsi_ext_ss_vals.dsat_cscb_val[2].string_item= cscb_dcss_val;

#if  defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)   
  dsat_etsi_ext_ss_vals.dsat_es_val[0].string_item =  dsat_es_orig_rqst_val;
  dsat_etsi_ext_ss_vals.dsat_es_val[1].string_item =  dsat_es_orig_fbk_val;
  dsat_etsi_ext_ss_vals.dsat_es_val[2].string_item =  dsat_es_ans_fbk_val;

  dsat_etsi_ext_ss_vals.dsat_esa_val[1].string_item = dsat_esa_trans_idle_val;
  dsat_etsi_ext_ss_vals.dsat_esa_val[2].string_item = dsat_esa_framed_un_ov_val;
  dsat_etsi_ext_ss_vals.dsat_esa_val[3].string_item = dsat_esa_hd_auto_val;
  dsat_etsi_ext_ss_vals.dsat_esa_val[7].string_item = dsat_esa_syn2_val;
#endif /* FEATURE_WCDMA ||  FEATURE_TDSCDMA*/

#ifdef FEATURE_DATA_IS707 

#ifdef FEATURE_DSAT_CDMA_PIN
  dsat707_ext_act_cmn_ss_vals.dsat707_cpin_val[0].string_item = dsat707_cpin_pin_val;
  dsat707_ext_act_cmn_ss_vals.dsat707_cpin_val[1].string_item = dsat707_cpin_newpin_val;
#endif /* FEATURE_DSAT_CDMA_PIN */

#ifdef FEATURE_CDMA_SMS
  dsat707_sms_ss_vals.dsat707sms_qcsmp_val[2].string_item = qcsmp_vp_val;
  dsat707_sms_ss_vals.dsat707sms_qcsmp_val[4].string_item = qcsmp_tdd_val;
#endif /* FEATURE_CDMA_SMS*/

#if defined(FEATURE_DSAT_SP_CMDS)
  dsat707_vend_act_ss_vals.dsat_spc_val.string_item = spc_val;
  dsat707_vend_act_ss_vals.dsat_unlock_val.string_item =  unlock_val;
  dsat707_vend_act_ss_vals.dsat_lock_val.string_item = lock_val;
  dsat707_vend_act_ss_vals.dsat_lock_chg_val[0].string_item = old_lock_val;
  dsat707_vend_act_ss_vals.dsat_lock_chg_val[1].string_item = new_lock_val;
#endif /*FEATURE_DSAT_SP_CMDS*/
#endif /* FEATURE_DATA_IS707 */

  /* CM interface Variable */
  /* SS event and PH event */
  dsatcmif_initialize_to_default(DSAT_SS_PH_SS_VALS, &dsatcmif_ss_ph_val.ss);

  /* Call Info */
  dsatcmif_initialize_to_default(DSAT_CALL_SS_VALS, &dsatcmif_call_val.ss);

  /* SUPs Info */
  dsatcmif_initialize_to_default(DSAT_SUPS_SS_VALS, &dsatcmif_sups_val.ss);

  /* MMGSDI Info */
  dsatme_initialize_to_default(DSAT_MMGSDI_SS_VALS, &dsatme_mmgsdi_val.ss);

  /* PBM info */
  dsatme_initialize_to_default(DSAT_PBM_SS_VALS, &dsatme_pbm_val.ss);

  /* Common info */
  dsatme_initialize_to_default(DSAT_COMMON_SS_VALS, &dsat_common_val.ss);

  for (i = DSAT_MS_FIRST_SUBS; i < DSAT_MS_MAX_SUBS; i++)
  {
    dsat_etsi_ext_act_ms_vals[i].dsat_cpin_val[0].string_item = cpin_pin_val[i];
    dsat_etsi_ext_act_ms_vals[i].dsat_cpin_val[1].string_item = cpin_newpin_val[i];
    dsat_etsi_ext_act_ms_vals[i].dsat_cops_val[2].string_item = cops_oper_val[i];
    dsat_etsi_ext_act_ms_vals[i].dsat_cpol_val[2].string_item = cpol_oper_val[i];
    dsat_etsi_ext_act_ms_vals[i].dsat_cops_val[4].string_item = cops_csg_id_val[i];

    dsat_etsi_ext_ms_vals[i].dsat_csca_val[0].string_item = dsat_csca_sca_val[i];
    dsat_etsi_ext_ms_vals[i].dsat_csca_val[1].string_item = dsat_csca_tosca_val[i];
    dsat_etsi_ext_ms_vals[i].dsat_csmp_val[0].string_item =  csmp_fo_val[i];
    dsat_etsi_ext_ms_vals[i].dsat_csmp_val[1].string_item =  csmp_vp_val[i];

    dsatcmif_initialize_to_default(DSAT_SS_PH_MS_VALS, &dsatcmif_ss_ph_val.ms[i]);

    dsatcmif_initialize_to_default(DSAT_CALL_MS_VALS, &dsatcmif_call_val.ms[i]);

    dsatcmif_initialize_to_default(DSAT_CALL_DA_VALS, &dsatcmif_call_val.da[i]);

    dsatcmif_initialize_to_default(DSAT_SUPS_MS_VALS, &dsatcmif_sups_val.ms[i]);

    dsatme_initialize_to_default(DSAT_MMGSDI_MS_VALS, &dsatme_mmgsdi_val.ms[i]);

    dsatme_initialize_to_default(DSAT_MMGSDI_MS_SLOT_VALS, &dsatme_mmgsdi_val.ms_slot[i]);

    dsatme_initialize_to_default(DSAT_PBM_MS_VALS, &dsatme_pbm_val.ms[i]);

    dsatme_initialize_to_default(DSAT_MS_MD_VALS, &dsatetsipkt_gprs_val.ms[i]);
  }

  return;
}

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory for mem_type
  which are subscription/slot/session independent.
  
DEPENDENCIES
  None

RETURNS


SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr
(
  dsat_mem_type           mem_type,
  boolean                 graceful
)
{
  void  *addr = NULL;
  
  switch(mem_type)
  {
    case DSAT_SS_PH_SS_VALS:
      {
        addr = &dsatcmif_ss_ph_val.ss;
      }
    break;

    case DSAT_CALL_SS_VALS:
     {
       addr = &dsatcmif_call_val.ss;
     }
    break;

    case DSAT_SUPS_SS_VALS:
      {
        addr = &dsatcmif_sups_val.ss;
      }
    break;
    
    case DSAT_MMGSDI_SS_VALS:
      {
        addr = &dsatme_mmgsdi_val.ss;
      }
    break;

    case DSAT_PBM_SS_VALS:
      {
        addr = &dsatme_pbm_val.ss;
      }
    break;
    
    case DSAT_COMMON_SS_VALS:
      {
        addr = &dsat_common_val.ss;
      }
    break;
    
    default:
    break;
  }

  if (addr == NULL)
  {
    DS_AT_MSG1_ERROR("Invalid addr for mem %d", mem_type);

    if (FALSE == graceful)
    {
      ASSERT(0);
    }
  }

  return addr;
} /*dsatutil_get_base_addr*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_PER_SLOT

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per slot.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_per_slot
(
  mmgsdi_slot_id_enum_type slot_id,
  boolean                  graceful
)
{
  void *addr = NULL;
  
  if (IS_VALID_SLOT_ID(slot_id))
  {
    addr = &dsatme_mmgsdi_val.ms_slot[DSAT_SLOT_INDEX(slot_id)];
  }

  if (addr == NULL)
  {
    DS_AT_MSG1_ERROR("Invalid addr for slot ID %d", slot_id);

    if (FALSE == graceful)
    {
      ASSERT(0);
    }
  }

  return addr;
} /*dsatutil_get_base_addr_per_slot*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_CURRENT_SLOT

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per slot.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_current_slot()
{
  return dsatutil_get_base_addr_per_slot(dsat_get_current_slot_id(), FALSE);
} /*dsatutil_get_base_addr_per_slot*/


/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_PER_SESSION

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per
  session.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_per_session
(
  dsat_apps_id_e_type     apps_id,
  boolean                 graceful
)
{
  void *addr = NULL;
  
  if (IS_VALID_GW_APPS_ID(apps_id))
  {
    addr = &dsatme_mmgsdi_val.ms[apps_id];
  }

  if (addr == NULL)
  {
    DS_AT_MSG1_ERROR("Invalid addr for apps ID %d", apps_id);

    if (FALSE == graceful)
    {
      ASSERT(0);
    }
  }

  return addr;
} /*dsatutil_get_base_addr_per_session*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_CURRENT_SESSION

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory 
  for currently selected session.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_current_session()
{
  return dsatutil_get_base_addr_per_session(dsat_get_current_gw_apps_id(FALSE), FALSE);
} /* dsatutil_get_base_addr_current_session */

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_PER_SUBS

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory per subs
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_per_subs
(
  dsat_mem_type           mem_type,
  sys_modem_as_id_e_type  subs_id,
  boolean                 graceful
)
{
  void *addr = NULL;

  if (IS_VALID_SUBS_ID(subs_id))
  {
    switch(mem_type)
    {
      case DSAT_SS_PH_MS_VALS:
      {
        addr = &dsatcmif_ss_ph_val.ms[subs_id];
      }
      break;

      case DSAT_CALL_DA_VALS:
      {
        addr = &dsatcmif_call_val.da[subs_id];
      }
      break;
    
      case DSAT_CALL_MS_VALS:
      {
        addr = &dsatcmif_call_val.ms[subs_id];
      }
      break;

      case DSAT_SUPS_MS_VALS:
      {
        addr = &dsatcmif_sups_val.ms[subs_id];
      }
      break;

      case DSAT_MS_MD_VALS:
      {
        addr = &dsatetsipkt_gprs_val.ms[subs_id];
      }
      break;

      case DSAT_PBM_MS_VALS:
      {
        addr = &dsatme_pbm_val.ms[subs_id];
      }
      break;
    
      default:
      break;
    }
  }

  if (addr == NULL)
  {
    DS_AT_MSG2_ERROR("Invalid addr for mem %d and subs ID %d", mem_type, subs_id);

    if (FALSE == graceful)
    {
      ASSERT(0);
    }
  }

  return addr;
} /*dsatutil_get_base_addr_per_subs*/

/*===========================================================================
FUNCTION  DSATUTIL_GET_BASE_ADDR_CURRENT_SUBS

DESCRIPTION
  This function will allocate,deallocate and get the dynamic memory for currently
  selected subs.
  
DEPENDENCIES
  None

RETURNS

SIDE EFFECTS
  None

===========================================================================*/

void *dsatutil_get_base_addr_current_subs
(
  dsat_mem_type           mem_type
)
{
  return dsatutil_get_base_addr_per_subs(mem_type, dsat_get_current_subs_id(FALSE), FALSE);
} /*dsatutil_get_base_addr_current_subs*/

/*===========================================================================

FUNCTION DSATUTIL_VALUE_GET

DESCRIPTION
  This FUNCTION copy value from source pointer into destination pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void * dsatutil_value_get
(
  const void                    *dest,                           /*  Destination pointer*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
)
{
  dsat_mixed_param_val_type * val_list;
  dsat_string_item_type     * val_string ;
  dsat_num_item_type *element;

  switch(value_info->val_type)
  {
    /* Single number Type*/
  case NUM_TYPE:
    element = (dsat_num_item_type *)dest;
    return (void *)(*(element + (value_info ->index)));
    
    /* Array Type or Struct Type */
  case STR_TYPE:
    return ((void *)dest);

    /* Array of string*/
  case ARRAY_OF_STR_TYPE :
    /* TO DO*/   
    break;
    
  case MIX_NUM_TYPE :
    val_list =  (dsat_mixed_param_val_type *) dest;
    return (void *)(val_list[value_info->index].num_item);

    /* Mixed string type*/
  case MIX_STR_TYPE :
    val_list =  (dsat_mixed_param_val_type *) dest;
    val_string = val_list[value_info->index].string_item;
    return ((void *)val_string);

  case MIX_INDEX_TYPE:
    val_list =  (dsat_mixed_param_val_type *) dest;
    return (void *)( val_list + value_info->index );

  default:
    break;
  }
  return NULL;
} /*dsatutil_value_get*/
/*===========================================================================

FUNCTION DSATUTIL_GET_PTR_TO_VALUE

DESCRIPTION
  This FUNCTION take entry index of parse table and extract base address of table .
  according base table it will  search entry in the table, if record found then return
  value_ptr otherwise return NULL.

DEPENDENCIES
  None.
RETURN VALUE
  Return value pointer . 

SIDE EFFECTS
 None.
===========================================================================*/
 void * dsatutil_get_ptr_to_value
(
  dsat_num_item_type             cmd_id,            /*  entry index in parse table  */
  dsat_num_item_type             apps_id,           /* subscription ID*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
)
{
  void * ret_val = NULL;
  ret_val = dsatutil_get_val_from_cmd_id(cmd_id,apps_id);
  if(ret_val !=  NULL)
  {
    ret_val = dsatutil_value_get(ret_val ,value_info);
  }
  return ret_val;
} /*  dsatutil_get_ptr_to_value  */
/*===========================================================================

FUNCTION DSATUTIL_VALUE_COPY

DESCRIPTION
  This FUNCTION copy value from source pointer into destination pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsatutil_value_copy
(
  const void                    *dest,                           /*  Destination pointer*/
  dsati_value_info_struct_type  *value_info         /*  value information*/
)
{
  dsat_mixed_param_val_type * val_list;
  dsat_string_item_type     * val_string ;
  switch(value_info->val_type)
  {
    /* Single number Type*/
    case NUM_TYPE:
      ((dsat_num_item_type *)dest)[value_info->index]=(dsat_num_item_type)value_info->val_ptr;
      break;
    /* Array Type or Struct Type */
    case STR_TYPE:
      (void) dsatutil_memscpy((void*)dest, value_info->size,(void*)value_info->val_ptr,value_info->size); 
      break;
    /* Array of string*/
    case ARRAY_OF_STR_TYPE :
       /*Currently leave it implement is later*/   
      break;
    /* Mixed number type*/
    case MIX_NUM_TYPE :
       val_list =  (dsat_mixed_param_val_type *) dest;
       val_list[value_info->index].num_item = (dsat_num_item_type)value_info->val_ptr;
       break;
    /* Mixed string type*/
    case MIX_STR_TYPE :
      val_list =  (dsat_mixed_param_val_type *) dest;
      val_string = val_list[value_info->index].string_item;
      (void) dsatutil_memscpy((void*)val_string, value_info->size,(void*)value_info->val_ptr,value_info->size);
      break;
    default:
      DS_AT_MSG0_HIGH("Default case");
      break;
   }
} /*dsatutil_value_copy*/
/*===========================================================================

FUNCTION DSATUTIL_SET_VALUE

DESCRIPTION
  This FUNCTION take entry index of parse table and extract base address of table .
  according base table it will  search entry in  table, if record found then typecast
  value_ptr into value_type and set it into table value pointer .  

DEPENDENCIES
  None.
RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dsatutil_set_value
(
  dsat_num_item_type             cmd_id,           /*  entry index in parse table  */
  dsat_num_item_type             apps_id,          /*  subscription ID*/ 
  dsati_value_info_struct_type  *value_info        /*  value information*/
)
{
  void * ret_val = NULL;

  ret_val = dsatutil_get_val_from_cmd_id(cmd_id,apps_id);
  if(ret_val !=  NULL)
  {
    dsatutil_value_copy(ret_val ,value_info);
    return ;
  }
} /*  dsatutil_set_value  */
 /*===========================================================================
 
 FUNCTION dsatutil_get_val
 
 DESCRIPTION
   This FUNCTION take command index and return corresponding value stored in table.  
 
 DEPENDENCIES
   None.
 RETURN VALUE
   None
 
 SIDE EFFECTS
   None.
 ===========================================================================*/

 void * dsatutil_get_val
(
  dsat_num_item_type       cmd_id,            /*  entry index in parse table  */
  dsat_num_item_type       apps_id,           /* subscription ID*/
  dsat_num_item_type       index,
  value_type_enum_type     val_type
)
{
  dsati_value_info_struct_type val_info;
  void                        *return_val = NULL;
  memset((void *)&val_info,0,sizeof(val_info));
  val_info.index = index;
  val_info.val_type = val_type;
  return_val = ((void *)dsatutil_get_ptr_to_value( cmd_id,apps_id,&val_info));
  if((val_type == STR_TYPE) || 
     (val_type == MIX_STR_TYPE) || 
     (val_type == MIX_INDEX_TYPE) )
  {
    if(return_val == NULL)
    {
       DATA_ERR_FATAL("String Type return value NULL");
    }
  }
  return return_val;
}/* dsatutil_get_val*/

 /*===========================================================================
  
  FUNCTION dsatutil_get_val_ext
  
  DESCRIPTION
    This FUNCTION take command index and return corresponding value stored in table.  
  
  DEPENDENCIES
    None.
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None.
  ===========================================================================*/
 
  void * dsatutil_get_val_ext
 (
   dsat_num_item_type       cmd_id,            /*  entry index in parse table  */
   dsat_num_item_type       apps_id,           /* subscription ID*/
   dsat_num_item_type       index,
   value_type_enum_type     val_type
 )
 {
   dsati_value_info_struct_type val_info;
   void                        *return_val = NULL;
   memset((void *)&val_info,0,sizeof(val_info));
   val_info.index = index;
   val_info.val_type = val_type;
   return_val = ((void *)dsatutil_get_ptr_to_value( cmd_id,apps_id,&val_info));
   if((val_type == STR_TYPE) || 
      (val_type == MIX_STR_TYPE) || 
      (val_type == MIX_INDEX_TYPE) )
   {
     if(return_val == NULL)
     {
        DS_AT_MSG0_HIGH("String Type return value NULL");
     }
   }
   return return_val;
 }/* dsatutil_get_val_ext*/

 /*===========================================================================
 FUNCTION	   dsat_get_cbst_export_val
 
 DESCRIPTION   This function returns pointer to a prticular index in array 
			   dsat_cbst_export_val. 
 
 PARAMETERS    int index - index of the array
 
 DEPENDENCIES  None
 
 RETURN VALUE  dsat_num_item_type*
 
 SIDE EFFECTS  None
 ===========================================================================*/
 dsat_num_item_type* dsat_get_cbst_export_val(int index)
 {
   return &dsat_cbst_export_val[index];
 }

 #if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
 /*===========================================================================
 FUNCTION	   dsat_get_cv120_export_val
 
 DESCRIPTION   This function gets global variable value of dsat_cv120_export_val
 
 PARAMETERS    None
 
 DEPENDENCIES  None
 
 RETURN VALUE  variable value
 
 SIDE EFFECTS  None
 ===========================================================================*/
 dsat_num_item_type dsat_get_cv120_export_val(void)
 {
   return dsat_cv120_export_val;
 }
 
 /*===========================================================================
 FUNCTION	   dsat_set_cv120_export_val
 
 DESCRIPTION   This function sets global variable value of dsat_cv120_export_val
 
 PARAMETERS    ptr to variable, value to be set
 
 DEPENDENCIES  None
 
 RETURN VALUE  None
 
 SIDE EFFECTS  None
 ===========================================================================*/
 
 
 void dsat_set_cv120_export_val(dsat_num_item_type variable_value)
 {
    dsat_cv120_export_val = variable_value;
 }
 #endif /* ( (FEATURE_DATA_GCSD) || (FEATURE_DATA_WCDMA_CS) ) */
  
 /*==============================================================================
 
 FUNCTION  dsat_get_qcscftest_exp_val
 
 DESCRIPTION This function returns the dsat_qcscftest_exp_val global variable .  
 
 PARAMETERS index of array
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS	None
 ===========================================================================*/
#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
 uint8 dsat_get_qcscftest_exp_val(int index)
 {
   return dsat_qcscftest_exp_val[index];
 }
#endif

  /*==============================================================================
  
  FUNCTION	dsat_set_qcscftest_exp_val
  
  DESCRIPTION This function sets the dsat_qcscftest_exp_val global variable .  
  
  PARAMETERS value, index
  
  DEPENDENCIES None
  
  RETURN VALUE None
  
  SIDE EFFECTS	 None
  ===========================================================================*/
#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
  uint8 dsat_set_qcscftest_exp_val(int index, uint8 value)
  {
	 dsat_qcscftest_exp_val[index] = value;
  }
#endif

#ifdef FEATURE_WCDMA
 /*==============================================================================
 
 FUNCTION dsat_get_es_export_val
 
 DESCRIPTION This function returns the dsat_es_export_val global variable .  
 
 PARAMETERS index of array
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS None
 ===========================================================================*/
 dsat_num_item_type dsat_get_es_export_val(int index)
 {
   return dsat_es_export_val[index];
 }
 
  /*==============================================================================
 
 FUNCTION dsat_set_es_export_val
 
 DESCRIPTION This function returns the dsat_es_export_val global variable .  
 
 PARAMETERS index of array
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS None
 ===========================================================================*/
 void dsat_set_es_export_val(int index, dsat_num_item_type value)
 {
    dsat_es_export_val[index] = value;
 }
  
 /*==============================================================================
 
 FUNCTION dsat_get_esa_export_val
 
 DESCRIPTION This function returns the dsat_esa_export_val global variable .  
 
 PARAMETERS index of array
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS None
 ===========================================================================*/
 dsat_num_item_type dsat_get_esa_export_val(int index)
 {
   return dsat_esa_export_val[index];
 }

 /*==============================================================================
 
 FUNCTION dsat_set_esa_export_val
 
 DESCRIPTION This function sets the dsat_esa_export_val global variable .  
 
 PARAMETERS index of array, value
 
 DEPENDENCIES None
 
 RETURN VALUE global variable
 
 SIDE EFFECTS None
 ===========================================================================*/
 void dsat_set_esa_export_val(int index, dsat_num_item_type value)
 {
   dsat_esa_export_val[index] = value;
 }
#endif  /* FEATURE_WCDMA */

 /*===========================================================================
 
 FUNCTION DSAT_SET_VAL_INFO
 
 DESCRIPTION
       Set values into structure .  
 
 DEPENDENCIES
   None.
 RETURN VALUE
   None
 
 SIDE EFFECTS
   None.
 ===========================================================================*/

 void dsat_set_val_info
 (           
   dsat_num_item_type             index,
   dsat_num_item_type             size,
   void *                         val_ptr,
   value_type_enum_type           val_type,
   dsati_value_info_struct_type   *val_info
 )
 {
   val_info->index = index;
   val_info->size = size;
   val_info->val_ptr = val_ptr;
   val_info->val_type = val_type;

 } /*dsat_set_val_info*/
/*Other get set APIs for globals*/
void dsat_mo_dailing_state_init(void)
{
  sys_modem_as_id_e_type subs_id;

  for (subs_id = SYS_MODEM_AS_ID_1; subs_id < DSAT_SYS_MODEM_AS_ID_MAX; subs_id++)
  {
    dsat_mo_dailing_state[subs_id] = TRUE;
  }
}

boolean dsat_mo_dailing_state_get
(
  sys_modem_as_id_e_type subs_id
)
{
    return dsat_mo_dailing_state[subs_id];
}

void dsat_mo_dailing_state_set
(
  sys_modem_as_id_e_type subs_id,
  boolean        value
)
{
     dsat_mo_dailing_state[subs_id] = value;
}

void dsat_value_set
(
  dsat_num_item_type cmd_id,
  dsat_num_item_type apps_id,
  dsat_num_item_type index,
  dsat_num_item_type size,
  dsat_num_item_type value_num,
  value_type_enum_type value_type
)
{
  dsati_value_info_struct_type  val_info; 
  memset((void *)&val_info,0,sizeof(val_info)); 
  dsat_set_val_info( index, size,(void *)value_num, value_type,&val_info);
  dsatutil_set_value( cmd_id,apps_id,&val_info);
}
/*===========================================================================
 FUNCTION DSAT_IS_PENDING_CMD_STATE
 
 DESCRIPTION
   This function checks the pending state of a command being passed
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_is_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
)
{
  if(dsat_pending_state[PORT_ZERO].cmd_idx == cmd_id && 
     dsat_pending_state[PORT_ZERO].cmd_pending[apps_id] == pending) 
  {
    return TRUE;
  }
  return FALSE;
}/*dsat_is_pending_cmd_state*/

/*===========================================================================
 FUNCTION DSAT_IS_NOT_PENDING_CMD_STATE
 
 DESCRIPTION
   This function checks the pending state of a command being passed
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_is_not_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
)
{
  if(dsat_pending_state[PORT_ZERO].cmd_idx == cmd_id && 
     dsat_pending_state[PORT_ZERO].cmd_pending[apps_id] != pending) 
  {
    return TRUE;
  }
  return FALSE;
}/*dsat_is_not_pending_cmd_state*/

/*===========================================================================
 FUNCTION DSAT_SET_PENDING_CMD_STATE
 
 DESCRIPTION
   This function sets the pending state of a command being passed.
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   TRUE: Returns TRUE if found
   FALSE: otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
void dsat_set_pending_cmd_state
(
  uint16             cmd_id,
  dsat_num_item_type apps_id, 
  dsat_num_item_type pending
)
{
  dsat_pending_state[PORT_ZERO].cmd_idx = cmd_id;
  dsat_pending_state[PORT_ZERO].cmd_pending[apps_id] = pending;
}/*dsat_set_pending_cmd_state*/
