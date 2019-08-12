/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E

GENERAL DESCRIPTION
  This module makes up the Call Manager of the subscriber station software.

  The Call Manager provides out of bound call control and event notification
  services to its clients.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmcc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/13   xs      FDN issue fix
10/06/13   xs      Fix issue of FDN present in CSIM but not in USIM
11/02/12   vm      *228 in 3gpp is treated as voice call
10/24/12   vm      CM Call control use new centralized cc for old api
10/09/12   fj      ACL specific change CR 361917.
05/14/12   ab      LTE call control specific changes CR 360560
04/15/12   ab      FR1681: Location Information support in Call Control envelope command
04/10/12   sk      Fixing the CW and KW warnings
03/23/12   gm      DIME integration fix: TMC deprecation and others.
02/01/12   ar      Removing Deadlock by avoiding cm_wait() inside callback fn
1/11/12    xs      Lint error fix
01/06/12   gm      Data synchronization changes for freefloating platform
07/13/11   rn      Handle MMGSDI error in call control callback
06/13/11   rm      Move to session based APIs of GSTK
04/28/11   rm      Correct the logic of determining session type
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/11/11   rm      GSDI clean-up
02/01/11   am      No need to cc if cdma subs from NV and gw subs none
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
11/08/10   xs      Fix the ambiguous pointer parameter when calling pack_bearer_capability
11/02/10   xs      Resolved NULL pointer issue when subscription is not available in call
                   control complete callback function
10/01/10   ak      Merged the Fusion code from Dev branch.
09/02/10   rn      Resolve call type before subsc check in call control
08/30/10   fj      Add support for LTE call control.
07/12/10   am      Suppport for modified sub-address in call control
04/29/10   sv      Add NAS stubs for QTF
01/25/10   am      Adding code to pack CCP struture before sending to SIM.
01/13/10   sv      KW Fixes
09/23/09   sv      KW fixes
04/29/09   rn      Initial release

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Customer configuration file
*/
#include "mmcp_variation.h"
#include "customer.h"

#include "cm.h"
#include "cmi.h"
#include "cmdbg.h"
#include "cmcall.h"
#include "cmmmgsdi.h"
#include "cmtaski.h"
//#include "cmwsups.h"

#include "pbmlib.h"

#include "gstk_exp.h"
#include "gstk_exp_v.h"
#ifdef CM_GW_SUPPORTED
#include "bearer_capability_utils.h"
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM */

#include "cmcall.h"
#include "cmss.h"
#include "cmclnup.h"
#ifdef CM_DEBUG
#error code not present
#endif

#include "cmcc.h"
#include "cmph.h"
#include "cmmsc.h"
#include "cmsimcoord.h"
#include "cmpmprx.h"

#if defined(WIN32)
#define strcasecmp _stricmp
#endif
/*======================================================================
                        DEFINES FOR MODULE
======================================================================*/

#ifdef FEATURE_PACKET_CALL_FDN_CHECK
#define CMCC_PACKET_CALL_FDN_STR   "*99#"
#endif

/*======================================================================
                        DATA STRUCTURES FOR MODULE
======================================================================*/

#ifdef FEATURE_OTASP
#define STANDARD_OTASP_LENGTH 6
  /* Table specifying default hard-coded special numbers and their
  ** corresponding call type. This is taken from 3GPP2 C.S0016-C
  **
  ** Note that for OTASP numbers the corresponding activation code
  ** is also specified.
  */
  static const struct {
      byte                     *num_str;      /* number string */
      cm_activate_code_e_type  activate_code; /* OTASP activation code */
  } cm_otasp_num_tbl[] = {

      /* number */   /* call type */               /* OTASP activate code */
      { (byte *) "*228", CM_OTASP_ACT_CODE_NONE },
      { (byte *) "*22800", CM_ACTIVATE_CODE_BC0_ABAND },
      { (byte *) "*22801", CM_ACTIVATE_CODE_BC0_BBAND },
      { (byte *) "*22802", CM_ACTIVATE_CODE_BC1_ABLOCK },
      { (byte *) "*22803", CM_ACTIVATE_CODE_BC1_BBLOCK },
      { (byte *) "*22804", CM_ACTIVATE_CODE_BC1_CBLOCK },
      { (byte *) "*22805", CM_ACTIVATE_CODE_BC1_DBLOCK },
      { (byte *) "*22806", CM_ACTIVATE_CODE_BC1_EBLOCK },
      { (byte *) "*22807", CM_ACTIVATE_CODE_BC1_FBLOCK },
      { (byte *) "*22812", CM_ACTIVATE_CODE_BC4_ABAND },
      { (byte *) "*22813", CM_ACTIVATE_CODE_BC4_BBAND },
      { (byte *) "*22814", CM_ACTIVATE_CODE_BC4_CBAND },
      { (byte *) "*22815", CM_ACTIVATE_CODE_BC5_ABAND },
      { (byte *) "*22816", CM_ACTIVATE_CODE_BC5_BBAND },
      { (byte *) "*22817", CM_ACTIVATE_CODE_BC5_CBAND },
      { (byte *) "*22818", CM_ACTIVATE_CODE_BC5_DBAND },
      { (byte *) "*22819", CM_ACTIVATE_CODE_BC5_EBAND },
      { (byte *) "*22820", CM_ACTIVATE_CODE_BC5_FBAND },
      { (byte *) "*22821", CM_ACTIVATE_CODE_BC5_GBAND },
      { (byte *) "*22822", CM_ACTIVATE_CODE_BC5_HBAND },
      { (byte *) "*22823", CM_ACTIVATE_CODE_BC6 },
      { (byte *) "*22824", CM_ACTIVATE_CODE_BC7_CBAND },
      { (byte *) "*22825", CM_ACTIVATE_CODE_BC19_ABAND },
      { (byte *) "*22826", CM_ACTIVATE_CODE_BC19_BBAND },
      { (byte *) "*22827", CM_ACTIVATE_CODE_BC19_CBAND },
      { (byte *) "*22828", CM_ACTIVATE_CODE_BC8 },
      { (byte *) "*22829", CM_ACTIVATE_CODE_BC9 },
      { (byte *) "*22830", CM_ACTIVATE_CODE_BC10_ABAND },
      { (byte *) "*22831", CM_ACTIVATE_CODE_BC10_BBAND },
      { (byte *) "*22832", CM_ACTIVATE_CODE_BC10_CBAND },
      { (byte *) "*22833", CM_ACTIVATE_CODE_BC10_DBAND },
      { (byte *) "*22834", CM_ACTIVATE_CODE_BC10_EBAND },
      { (byte *) "*22835", CM_ACTIVATE_CODE_BC11_ABAND },
      { (byte *) "*22836", CM_ACTIVATE_CODE_BC11_BBAND },
      { (byte *) "*22837", CM_ACTIVATE_CODE_BC11_CBAND },
      { (byte *) "*22838", CM_ACTIVATE_CODE_BC11_DBAND },
      { (byte *) "*22839", CM_ACTIVATE_CODE_BC11_EBAND },
      { (byte *) "*22840", CM_ACTIVATE_CODE_BC11_FBAND },
      { (byte *) "*22841", CM_ACTIVATE_CODE_BC12_ABAND },
      { (byte *) "*22842", CM_ACTIVATE_CODE_BC12_BBAND },
      { (byte *) "*22857", CM_ACTIVATE_CODE_BC14_GBLOCK},
      { (byte *) "*22858", CM_ACTIVATE_CODE_BC15_ABAND },
      { (byte *) "*22859", CM_ACTIVATE_CODE_BC15_BBAND },
      { (byte *) "*22860", CM_ACTIVATE_CODE_BC15_CBAND },
      { (byte *) "*22861", CM_ACTIVATE_CODE_BC15_DBAND },
      { (byte *) "*22862", CM_ACTIVATE_CODE_BC15_EBAND },
      { (byte *) "*22863", CM_ACTIVATE_CODE_BC15_FBAND },
      { (byte *) "*22864", CM_ACTIVATE_CODE_BC16_ABAND },
      { (byte *) "*22865", CM_ACTIVATE_CODE_BC16_BBAND },
      { (byte *) "*22866", CM_ACTIVATE_CODE_BC16_CBAND },
      { (byte *) "*22867", CM_ACTIVATE_CODE_BC16_DBAND },
      { (byte *) "*22868", CM_ACTIVATE_CODE_BC18_DBAND},
      { (byte *) "*22869", CM_ACTIVATE_CODE_BC18_PSB_BAND}

};

#endif /* FEATURE_OTASP */


/*
** Static data
*/
#if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_LTE)

static cm_mmgsdi_acl_avail_status_e_type cm_mmgsdi_acl_availability =
                                                CM_MMGSDI_ACL_AVAIL_INVALID;
static boolean cm_acl_status = FALSE;

#endif

/*
** Structure definitions
*/
static void cmcc_send_cc_command(cmcc_object_s_type *cc_object_ptr);
static void cmcc_gstk_rsp_cb (gstk_cmd_from_card_type *req_rsp);
#ifdef FEATURE_OTASP
static boolean cmcc_is_mode_3gpp_only (cmcall_s_type* call_ptr);
#endif
static boolean cmcc_is_cc_required
(
   cmcc_object_s_type                *cc_object_ptr,
   cm_cc_subscription_source_e_type  cdma_subscription_source,
   cm_cc_subscription_source_e_type  gw_subscription_source
);
#ifdef FEATURE_MMODE_DUAL_SIM
boolean cmcc_is_number_emerg_on_other_sub
(
  cmcc_object_s_type                  *cc_object_ptr
);
#endif

/*======================================================================
                        FUNCTIONS FOR MODULE
======================================================================*/

/*===========================================================================

FUNCTION cmcc_object_allocate

DESCRIPTION
  Allocate a CMCC Object buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  Pointer to allocated IP app rpt buffer.

SIDE EFFECTS
  none

===========================================================================*/
static cmcc_object_s_type *cmcc_object_allocate (

  cm_call_id_type           call_id
)
{

  cmcc_object_s_type        *cmcc_object_ptr = NULL;
    /* CM CC object buffer */

  /*----------------------------------------------------------------------*/

  /* Try allocating a CM CC object buffer
  */
  cmcc_object_ptr =
             (cmcc_object_s_type*) cm_mem_malloc(sizeof(cmcc_object_s_type));

  CM_MSG_HIGH_1( "CM CC object allocated for call id %d", call_id);
  /* This is to keep lint happy. It does not consider print as access */
  SYS_ARG_NOT_USED(call_id);
  return cmcc_object_ptr;

}  /* cmcc_object_allocate */


/*===========================================================================

FUNCTION cmcc_object_deallocate

DESCRIPTION
  Deallocates a CM CC object

DEPENDENCIES
  none

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  none

===========================================================================*/
void cmcc_object_deallocate
(
    cmcc_object_s_type *cmcc_object_ptr
      /* CM CC object */
)
{

  cm_call_id_type   call_id = CM_CALL_ID_UNASSIGNED;

  cmcall_s_type              *call_ptr;

  if(cmcc_object_ptr == NULL)
  {
    CM_ERR_1("Null CM CC Ptr for call id %d, Deallocate Failed!", call_id);
    return;
  }

  call_ptr = cmcall_ptr (cmcc_object_ptr->call_id);
  /* For sups call_ptr will be Null*/
  if(call_ptr != NULL)
  {
    call_ptr->cc_object_ptr = NULL;
  }
  call_id = cmcc_object_ptr->call_id;

  cm_mem_free(cmcc_object_ptr);

  CM_MSG_HIGH_1( "CM CC object deallocated for call id %d", call_id);
  /* This is to keep lint happy. It does not consider print message as access */
  SYS_ARG_NOT_USED(call_id);

} /* cmcc_object_deallocate() */

/*===========================================================================
FUNCTION cmcc_resolve_call_type

DESCRIPTION
  This is internal CM function, called within cmcc_call_control_processing()
  This function determines whether an originating number matches one of the
  special number-classification number, such as EMERGENCY or OTASP.
  If a match is found, this function should indicate the originating
  call type via the call_type_ptr and if applicable the OTASP activation
  code via the activate_code_ptr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void cmcc_resolve_call_type (

  mmgsdi_session_type_enum_type session_type,

  cmcc_object_s_type *cc_object_ptr

)
/*lint -esym(715,session_type)*/
{
  cm_num_s_type* orig_num_ptr                   = &(cc_object_ptr->orig_request.num);
  cm_call_type_e_type* call_type_ptr            = &(cc_object_ptr->call_type);
  cm_orig_client_categ_e_type client_categ      = CM_ORIG_CLIENT_CATEG_NONE;
  uint8 gw_ecc_categ = 0;
  #ifdef FEATURE_OTASP
  cm_activate_code_e_type* activate_code_ptr    = &(cc_object_ptr->orig_request.activate_code);
  uint8 index = 0;
  cmcall_s_type* call_ptr = NULL;
  #endif

  /* Emergency/OTASP can be originated as Voice call. If this is not a
  ** voice call, return now
  */
  if(( *call_type_ptr != CM_CALL_TYPE_VOICE )
  && ( *call_type_ptr != CM_CALL_TYPE_VT ))
  {
    return;
  }
  // Only CALL_TYPE_VOICE is allowed so typecasting is fine
  client_categ = CALL_CMD_PTR(cc_object_ptr->user_data)->client_categ;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
 {
   boolean found_in_gw = FALSE;
   boolean found_in_1x = FALSE;
   uint8 cdma_ecc_categ = 0;
   pbm_session_enum_type pbm_session_type;
   pbm_session_type = pbmif_mmgsdi_session_to_pbm_session(session_type);
    if(client_categ != CM_ORIG_CLIENT_CATEG_CARD &&
       (pbm_session_emergency_number_cat_ext( pbm_session_type,
                                              orig_num_ptr->buf,
                                              orig_num_ptr->len,
                                              &found_in_gw,
                                              &found_in_1x,
                                              &gw_ecc_categ,
                                              &cdma_ecc_categ)== PBM_SUCCESS))
    {

       /*lint -e{774} Boolean within 'if' always evaluates to False */
     if(
         #if defined(CM_GW_SUPPORTED)&&(defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
         (found_in_gw)||(found_in_1x)
         #elif defined(CM_GW_SUPPORTED)
             (found_in_gw)
         #elif (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
             (found_in_1x)
         #endif
       )
     {
        *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
         if(!found_in_gw)
         {
           cc_object_ptr->gw_result_ptr = &(cc_object_ptr->cc_result_buf);
           cc_object_ptr->gw_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
         }
         else
         {
           cc_object_ptr->gw_result_ptr->num.emerg_srv_categ = (cm_emerg_srv_categ_type)gw_ecc_categ;
           CM_MSG_HIGH_1 ("e call, gw categ %d",gw_ecc_categ);
         }
         if(!found_in_1x)
         {
           cc_object_ptr->cdma_result_ptr  = &(cc_object_ptr->cc_result_buf);
           cc_object_ptr->cdma_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;

         }
         else
         {
           /* Ignore CDMA emerg srv category. This is not used in origination towards BS */
           CM_MSG_HIGH_1 ("e call, cdma categ %d",cdma_ecc_categ);
         }

        return;
     }
    }
 }

#else
  if(client_categ != CM_ORIG_CLIENT_CATEG_CARD &&
     pbm_emergency_number_cat(orig_num_ptr->buf, orig_num_ptr->len,
                               &gw_ecc_categ))
  {
    *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
    cc_object_ptr->gw_result_ptr->num.emerg_srv_categ = (cm_emerg_srv_categ_type)gw_ecc_categ;
    CM_MSG_HIGH_1 ("e call, categ %d",gw_ecc_categ);
    return;
  }
#endif
  /* 112 is the only emergency number for SIM initiated calls as per spec
  ** 3GPP TS 22.101 */
  if(client_categ == CM_ORIG_CLIENT_CATEG_CARD)
  {
    if ((orig_num_ptr->len == 3)       &&
        (orig_num_ptr->buf[0] == '1')  &&
        (orig_num_ptr->buf[1] == '1')  &&
        (orig_num_ptr->buf[2] == '2') )
    {
      *call_type_ptr = CM_CALL_TYPE_EMERGENCY;
      cc_object_ptr->gw_result_ptr->num.emerg_srv_categ = (cm_emerg_srv_categ_type)0;
      CM_MSG_HIGH_1 ("SIM e call, categ %d",gw_ecc_categ);
      return;
    }
  }

  #ifdef FEATURE_OTASP

  call_ptr = cmcall_ptr (cc_object_ptr->call_id);
  if( call_ptr != NULL &&
      cmcc_is_mode_3gpp_only(call_ptr) == TRUE )
  {

    /* For 3gpp only mode otasp number is considered as normal voice call. */

    CM_MSG_HIGH_1( "For 3gpp only mode %d is considered as normal voice call",
                   orig_num_ptr->buf );

    return;
  }

  /* Consult the default hard-coded otasp numbers table to
  ** determine the originating number call type.
  */
  if(! memcmp(orig_num_ptr->buf,
               cm_otasp_num_tbl[index].num_str, 4))
  {


    *call_type_ptr     = CM_CALL_TYPE_STD_OTASP;
    for( index=1; index< (int)ARR_SIZE(cm_otasp_num_tbl); index++ )
  {
    /* Check whether one of the otasp numbers match the origination #.
    */
    if( orig_num_ptr->len >= STANDARD_OTASP_LENGTH &&
      ! memcmp(orig_num_ptr->buf,
               cm_otasp_num_tbl[index].num_str, STANDARD_OTASP_LENGTH) )
    {
      /* If we found a match, change the call type to the corresponding,
      ** type in the table and indicate the OTASP activation code.
      */
      *activate_code_ptr = cm_otasp_num_tbl[index].activate_code;
      CM_MSG_HIGH_1 ("std otasp call, code %d",
                    cm_otasp_num_tbl[index].activate_code);
      return;
    }
  }


    /*If we are here, that means
           1. Dialed string starts with *228
           2. there is no match for the dialed string with any
              of the spec defined system selection codes.
          Change the call type to NON_STD OTASP
          change activate code to CM_PREFERRED*/

    *call_type_ptr     = CM_CALL_TYPE_NON_STD_OTASP;

    *activate_code_ptr = CM_PREFERRED;

    CM_MSG_HIGH_0 ("non std otasp call");
  }

  #endif /* FEATURE_OTASP */

}/* cmcc_resolve_call_type */
/*lint +esym(818, activate_code_ptr)*/



#ifdef FEATURE_GSTK
/*===========================================================================
FUNCTION cmcc_compare_nw_apn_with_acl_apn

DESCRIPTION
  This function compares the Network provided APN with the
  ACL List APN, ignoring case.

DEPENDENCIES
  None.

RETURN VALUE
  int32 - 0 if strings are equal, Non Zero if strings are Not equal.

SIDE EFFECTS
  None.

===========================================================================*/

int32 cmcc_compare_nw_apn_with_acl_apn(const char *nw_apn, const char *acl_list_apn, int32 apn_len)
{
  int32 cmp_status = 0;
  char* nw_apn_str = NULL;
  char* acl_list_apn_str = NULL;

  if(nw_apn == NULL || acl_list_apn == NULL || apn_len == 0)
  {
    CM_ERR_FATAL_3("Invalid input params nw_apn:%x acl_list_apn:%x apn_len;%d",
                  nw_apn, acl_list_apn, apn_len);
  }

  /* APN Address strings are not guranteed to be NULL terminated,
     so making sure to NULL terminate the strings explicitely */
  /* customized string comparison using pointers could have been
     done here but it may not work for all ASCII values, so instead
     using costlier but safe comparison approach*/

  /* Allocate the memory for local variables. */
  if((nw_apn_str = (char *)cm_mem_malloc( apn_len+1 )) != NULL)
  {
    CM_MSG_MED_2("Allocated memory for nw_apn_str object = %x of size = %d",
               nw_apn_str, (apn_len+1));

    memscpy(nw_apn_str, apn_len, nw_apn, apn_len);

    nw_apn_str[apn_len] = '\0';
  }

  /* Allocate the memory for local variables. */
  if((acl_list_apn_str = (char *)cm_mem_malloc( apn_len+1)) != NULL)
  {
    CM_MSG_MED_2("Allocated memory for acl_list_apn_str object = %x of size = %d",
               acl_list_apn_str, (apn_len+1));

    memscpy(acl_list_apn_str, apn_len, acl_list_apn, apn_len);

    acl_list_apn_str[apn_len] = '\0';
  }


  cmp_status = strcasecmp(nw_apn_str, acl_list_apn_str);

  if(cmp_status == 0)
  {
    CM_MSG_MED_0("APN Strings matched");
  }
  else
  {
    CM_MSG_MED_0("APN Strings are not matching");
  }

  if (nw_apn_str != NULL)
  {
     cm_mem_free((void *)(nw_apn_str));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  if (acl_list_apn_str != NULL)
  {
     cm_mem_free((void *)(acl_list_apn_str));
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  return cmp_status;
}

/*===========================================================================
FUNCTION cmcc_acl_read_complete

DESCRIPTION
  This is the read callback for reading ACL.  This is used in call
  control for packet data calls.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cmcc_acl_read_complete(

  uint32       file_length,
   /* Specifies the file_lenght of ACL */

  uint8*       acl_file_content,
   /* Pointer to acl file content */

  boolean      status,
   /* Status of 'read' sent by MMGSDI */

  void        *client_data
   /* data block pointer */
)
{
  boolean pass = FALSE;
  uint32 sim_data_index = 0;
  uint32 num_apns = 0;
  uint32 apn_len, i = 0;
  cmcc_object_s_type *cc_object_ptr;
  cmcall_s_type                *call_ptr;
  boolean allow_nw_apn = FALSE;
  apn_T   ueAPN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( client_data != NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ueAPN, 0, sizeof(ueAPN));

  cc_object_ptr = client_data;
  call_ptr = cmcall_ptr (cc_object_ptr->call_id);

  if (status == TRUE &&
      acl_file_content != NULL &&
      file_length != 0)
  {
    CM_MSG_HIGH_3( "ACL read status=%d, acl_file_content %d, file_length=%d",
                   status, acl_file_content, file_length );

    num_apns = acl_file_content[sim_data_index];
    sim_data_index++;

    if (num_apns == 0)
    {
      #ifndef FEATURE_DISABLE_DATA_ON_EMPTY_ACL
      pass = TRUE;
      CM_MSG_HIGH_0("APN file has 0 entries, ACL pass");
      #else
      CM_MSG_HIGH_0("APN file has 0 entries, ACL fail");
      #endif
    }

    #if defined(FEATURE_CM_LTE)
    if (cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
    {
      ueAPN = cc_object_ptr->orig_request.lte_ps_cc_params.apn_name;
    }
    else
    #endif
    #ifdef CM_GW_SUPPORTED
    {
      ueAPN = cc_object_ptr->orig_request.gw_ps_cc_params.apn_name;
    }
    #else
    {
      /*CDMA data call skip ACL check*/
      pass = TRUE;
    }
    #endif

    while ((num_apns > 0) && (sim_data_index < file_length) && !pass)
    {
      /* read tag */
      if (acl_file_content[sim_data_index] != 0xdd)
      {
        CM_MSG_HIGH_2("APN file has invalid tag entry 0x%x at index %d",
                     acl_file_content[sim_data_index],sim_data_index);
        break;
      }
      sim_data_index++;

      /* read length */
      apn_len = acl_file_content[sim_data_index];
      if (apn_len + sim_data_index >= file_length)
      {
        CM_MSG_HIGH_3("APN file has invalid length 0x%x at index %d, File len %d",
                      apn_len, sim_data_index, file_length);
        break;
      }

      if(apn_len == 0)
      {
        allow_nw_apn = TRUE;
      }

      sim_data_index++;


      /* value is now apn_len bytes, beginning at sim_data_index
      ** apn matches if the lengths are equal, and the data is the same
      */
        /* - - - - - - - - - - - - - - - - - - - -- - - - -- - - - - - - -*/
        /* Debug messages */
        CM_MSG_LOW_0("ACL APN name");
        for (i = 0; i < apn_len; i++)
        {
          CM_MSG_LOW_1("%x",acl_file_content[sim_data_index+i]);
        }

     CM_MSG_LOW_2("cc APN name valid=%d, apn_addr_len=%d",ueAPN.valid,ueAPN.apn_addr_len);

        if (cc_object_ptr->orig_request.lte_ps_cc_params.apn_name.valid)
        {
          for (i = 0;
               i < cc_object_ptr->orig_request.lte_ps_cc_params.apn_name.apn_addr_len;
               i++)
          {
            CM_MSG_LOW_1("%x",
            cc_object_ptr->orig_request.lte_ps_cc_params.apn_name.address[i]);
          }
        }
        /* handle normal APNs */
      if (ueAPN.valid &&
           (ueAPN.apn_addr_len == apn_len) &&
            (0 == cmcc_compare_nw_apn_with_acl_apn(
                (const char*)ueAPN.address,
             (const char*)&acl_file_content[sim_data_index],
             apn_len)))
        {
          pass = TRUE;
          break;
        }
        num_apns--;
        sim_data_index += apn_len;
        }

    /* Deallocate the memory */
    cm_mem_free( acl_file_content );

        /* handle network provided APNs */
    if (!pass && !ueAPN.valid &&((cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)|| allow_nw_apn))
    {
      pass = TRUE;
    }

    if (pass)
    {
      /*If ACL checking passes
      ** and call control is applicable
      ** we will send Envelope command.
      */
      #ifdef FEATURE_CM_LTE
      if (cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
      {
        if(!cc_object_ptr->srv_available)
        {
          cc_object_ptr->orig_request.call_control_result =
                                        CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

          cc_object_ptr->gw_result_ptr   = &cc_object_ptr->orig_request;
          CM_MSG_HIGH_1("Call allowed, srv_available = %d",
                       cc_object_ptr->srv_available);

          /* CC complete, invoke CM callback */
          (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                                                 cc_object_ptr->status,
                                                 cc_object_ptr->call_type,
                                                 cc_object_ptr->cdma_result_ptr,
                                                 cc_object_ptr->gw_result_ptr,
                                                 (void*) cc_object_ptr->user_data);

            /* This sequence is complete, so free the memory. */
            cmcc_object_deallocate( cc_object_ptr );

        }
        else
        {
          cmcc_send_cc_command(cc_object_ptr);
        }
      }
      else
      #endif
      {
        #ifdef FEATURE_GPRS_CALLCONTROL
        if((call_ptr != NULL)&&
          (cc_object_ptr->sim_cc_required == CMCC_CARD_STATUS_CC_REQUIRED))
        {
           call_ptr->cc_object_ptr = cc_object_ptr;
           cc_object_ptr->is_cc_delayed = TRUE;
           /* This is the case of non LTE post a reorig command and send the envelope command*/
           (*(cc_object_ptr->cc_complete_cb_func))(cc_object_ptr->call_id,
                                            CM_CC_STATUS_SUCCESS,
                                            cc_object_ptr->call_type,
                                            cc_object_ptr->cdma_result_ptr,
                                            cc_object_ptr->gw_result_ptr,
                                           (void*)cc_object_ptr->user_data);
        }
        else
        #endif
        {
          /* This is the case of non LTE post a reorig command and don't send the envelope command*/
          /* Or for a race condition scenario, when Call is ended before call control is completed
             and MMGSDI sending read confirmation later aftr call object is already dealloacted.
             No need to crash ASSERT here for NULL call_ptr check and instead allow CM to deallocate, 
             Call CMD ptr, call_ctrl_object_ptr and SIM read request in respective order and ignore 
             the ACL Read CNF. Confirmed with DS, no need to send call_cmd_cb if Callend CMD is sent
             by DS and callend evt is sent by CM*/
          (*(cc_object_ptr->cc_complete_cb_func))(cc_object_ptr->call_id,
                                            CM_CC_STATUS_SUCCESS,
                                            cc_object_ptr->call_type,
                                            cc_object_ptr->cdma_result_ptr,
                                            cc_object_ptr->gw_result_ptr,
                                           (void*)cc_object_ptr->user_data);
          CM_MSG_HIGH_0("Sim call-control not needed, so skipping it");
          cmcc_object_deallocate( cc_object_ptr );
        }
         /* Otherwise the cc_object_ptr will be deallocated when CM gets OK_to_orig from SD*/
      }

      return;
    }
    #ifdef FEATURE_CM_LTE
    else if (cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
    {
      /* no ACL matching for this APN */
      cc_object_ptr->orig_request.call_control_result =
        CM_CC_RESULT_NOT_ALLOWED;
      cc_object_ptr->orig_request.error_cause = CM_CALL_CMD_ERR_CAUSE_ACL_NO_MATCH;
      cc_object_ptr->gw_result_ptr   = &cc_object_ptr->orig_request;
      CM_MSG_HIGH_1("Call not allowed, err cause: %d",
                   cc_object_ptr->orig_request.error_cause);

      (*cc_object_ptr->cc_complete_cb_func)(cc_object_ptr->call_id,
                       CM_CC_STATUS_SUCCESS,
                       cc_object_ptr->call_type,
                       cc_object_ptr->cdma_result_ptr,
                       cc_object_ptr->gw_result_ptr,
                       (void*) cc_object_ptr->user_data);

        /* This sequence is complete, so free the memory. */
        cmcc_object_deallocate( cc_object_ptr );
      return;
    }
    #endif
  }

  SYS_ARG_NOT_USED(i);
  SYS_ARG_NOT_USED(call_ptr);

  /* ACL read returned error */
  /* fail call control */
  cc_object_ptr->orig_request.call_control_result =
    CM_CC_RESULT_NOT_ALLOWED;
  cc_object_ptr->orig_request.error_cause = CM_CALL_CMD_ERR_CAUSE_ACL_FAIL;
  cc_object_ptr->gw_result_ptr   = &cc_object_ptr->orig_request;
  CM_MSG_HIGH_1("Call not allowed, err cause: %d",
                           cc_object_ptr->orig_request.error_cause);

  (*cc_object_ptr->cc_complete_cb_func)(cc_object_ptr->call_id,
                         CM_CC_STATUS_SUCCESS,
                         cc_object_ptr->call_type,
                         cc_object_ptr->cdma_result_ptr,
                         cc_object_ptr->gw_result_ptr,
                         (void*) cc_object_ptr->user_data);
  /* This sequence is complete, so free the memory. */
    cmcc_object_deallocate( cc_object_ptr );
    return;

} /* cmcc_acl_read_complete */


#endif /* ifdef  GSTK */

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION cmcc_map_mmgsdi_to_pbm_session_type

DESCRIPTION
 Maps MMGSDI session type to PBM prov type

DEPENDENCIES
  None.

RETURN VALUE
  pbm_provision_enum_type.

SIDE EFFECTS
  None.

===========================================================================*/
static pbm_provision_enum_type cmcc_map_mmgsdi_to_pbm_session_type
(
  mmgsdi_session_type_enum_type  session_type
)
{
  pbm_provision_enum_type prov_type = PBM_PROVISION_UNKNOWN;
  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
         prov_type = PBM_PROVISION_GW_PRIMARY;
         break;
    case MMGSDI_GW_PROV_SEC_SESSION:
         prov_type = PBM_PROVISION_GW_SECONDARY;
         break;
    case MMGSDI_GW_PROV_TER_SESSION:
         prov_type = PBM_PROVISION_GW_TERTIARY;
         break;
    case MMGSDI_1X_PROV_PRI_SESSION:
         prov_type = PBM_PROVISION_1X_PRIMARY;
         break;
    case MMGSDI_1X_PROV_SEC_SESSION:
         prov_type = PBM_PROVISION_1X_SECONDARY;
         break;
    case MMGSDI_1X_PROV_TER_SESSION:
         prov_type = PBM_PROVISION_1X_TERTIARY;
         break;
    default:
         prov_type = PBM_PROVISION_UNKNOWN;
         break;
  }

  return prov_type;
}
#endif

/*===========================================================================
FUNCTION cmcc_send_envelope_cmd

DESCRIPTION
 Sends the GSTK envelope command

DEPENDENCIES
  This function is called after CM gets ok_to_orig

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cmcc_send_envelope_cmd(cmcall_s_type *call_ptr)
{
/*lint -esym(818, call_ptr)*/
  #ifdef FEATURE_MMGSDI_SESSION_LIB

  cmcc_object_s_type                       *cc_object_ptr;

  cc_object_ptr = call_ptr->cc_object_ptr;

  cc_object_ptr->cc_complete_cb_func = cmcall_call_control_complete_phase2_cb;
  if(cc_object_ptr->is_cc_delayed == FALSE)
  {
    CM_MSG_HIGH_1("cmcc_send_envelope_cmd wrong value of cc_object_ptr->is_cc_delayed %d Should always be true",
                  cc_object_ptr->is_cc_delayed);

    cc_object_ptr->is_cc_delayed = TRUE;
  }

  /*origination ss might have changed after call control phase I*/
  CM_MSG_HIGH_2("Updating cc_object_ptr->ss from %d to %d", cc_object_ptr->ss,call_ptr->ss);
  cc_object_ptr->ss = call_ptr->ss;

  cmcc_send_cc_command(cc_object_ptr);
  call_ptr->call_state = CM_CALL_STATE_CC_IN_PROGRESS;

  #endif /* defined(FEATURE_GSTK)  */
  SYS_ARG_NOT_USED(call_ptr);
  /*lint +esym(818, call_ptr)*/
}
/*===========================================================================
FUNCTION cmcc_call_ctrl_after_sim_capabilities

DESCRIPTION
 Performs call control operations that depend on having SIM capabilities

DEPENDENCIES
  This function must run in CM context only.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmcc_call_ctrl_after_sim_capabilities
(
  cmcc_object_s_type          *cc_object_ptr,
  /* Pointer to CC object type */

  cm_orig_client_categ_e_type client_categ
  /* client category */
)
{
  boolean is_call_allowed_cdma = TRUE;   /* Is call still allowed on CDMA? */
  boolean is_call_allowed_gw   = TRUE;   /* Is call still allowed on GW? */
  cmcall_s_type                *call_ptr;
  boolean is_fdn_to_check      = TRUE;
  #if defined(FEATURE_MMGSDI_SESSION_LIB)
  mmgsdi_return_enum_type (*cc_required_fn) (
    mmgsdi_session_id_type              session_id,
    mmgsdi_service_enum_type            srvc_type,
    mmgsdi_callback_type                response_cb_ptr,
    mmgsdi_client_data_type             client_ref) = NULL;
  #endif
  boolean is_fdn_enabled = FALSE;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  pbm_phonebook_type                       pb_info;
  mmgsdi_service_enum_type                 mmgsdi_srv_type=MMGSDI_NONE;
  cm_mmgsdi_error_e_type                   mmgsdi_result = CM_MMGSDI_SUCCESS;
  cm_mmgsdi_session_info_s_type            sess_info =
                                          {MMGSDI_MAX_SESSION_TYPE_ENUM, 0 };
  mmgsdi_app_enum_type                     app_type;
  #endif

  CM_ASSERT(cc_object_ptr != NULL);

  call_ptr = cmcall_ptr (cc_object_ptr->call_id);
  // Check only for voice call (as skipping FDN is intended here for eCall, where in case of ERA-GLONASS
  // some numbers are there in eCall EFS but not in FDN
  /* Do not check lte ps call since user data is not valid */
  if(cc_object_ptr->call_type != CM_CALL_TYPE_SUPS &&
    !(cc_object_ptr->call_id == CM_CALL_ID_INVALID && cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA))
  {
    cm_call_cmd_s_type   *call_cmd_ptr = (cm_call_cmd_s_type*) cc_object_ptr->user_data;
    if(call_cmd_ptr != NULL)
    {
      is_fdn_to_check                       = call_cmd_ptr->info.cc_generic_params.is_fdn_to_check;
      CM_MSG_HIGH_1 ("is_fdn_to_check = %d",is_fdn_to_check);
    }
  }


  CM_MSG_HIGH_2 ("CC_after_sim_cap: call_id=%d, cc call_type=%d",
                   cc_object_ptr->call_id, cc_object_ptr->call_type);

  if (call_ptr)
  {
    CM_MSG_HIGH_1 ("CC_after_sim_cap: cm call_type=%d", call_ptr->call_type);
  }

  CM_MSG_HIGH_3 ("CC_after_sim_cap: cdma subs src %d, gw subs src %d, ps_cc_sys_mode=%d",
               cc_object_ptr->cdma_subscription_source,
               cc_object_ptr->gw_subscription_source,
               cc_object_ptr->orig_request.ps_cc_sys_mode);

  if (CM_CC_SUBSCRIPTION_SOURCE_NONE ==
         cc_object_ptr->cdma_subscription_source)
  {
    is_call_allowed_cdma = FALSE;
    cc_object_ptr->cc_result_buf.error_cause = CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
    cc_object_ptr->cc_result_buf.call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    cc_object_ptr->cdma_result_ptr = &cc_object_ptr->cc_result_buf;
  }

  if(CM_CC_SUBSCRIPTION_SOURCE_NONE == cc_object_ptr->gw_subscription_source)
  {
    is_call_allowed_gw = FALSE;
    cc_object_ptr->cc_result_buf.error_cause = CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
    cc_object_ptr->cc_result_buf.call_control_result = CM_CC_RESULT_NOT_ALLOWED;
    cc_object_ptr->gw_result_ptr = &cc_object_ptr->cc_result_buf;
  }

  CM_MSG_HIGH_5("cc_result_buf: err cause %d cc result %d call_allow_cdma %d call_allow_gw %d sim_cap_valid %d",
                 cc_object_ptr->cc_result_buf.error_cause,
                 cc_object_ptr->cc_result_buf.call_control_result,
                 is_call_allowed_cdma,
                 is_call_allowed_gw,
                 cc_object_ptr->sim_capabilities_valid);

  if (is_call_allowed_cdma || is_call_allowed_gw)
  {
    /* Next, the SIM has required checks of the FDN.  If the FDN is
     * enabled, these must be checked.
     * Limit calls to SIM1 FDN
     */
      if (cc_object_ptr->sim_capabilities_valid == FALSE)
      {

        /* Call is not allowed because SIM cap. was not success
        */
        cc_object_ptr->orig_request.call_control_result =
                                        CM_CC_RESULT_NOT_ALLOWED;
        cc_object_ptr->orig_request.error_cause =
                                        CM_CALL_CMD_ERR_CAUSE_OTHER;

        CM_MSG_HIGH_1("Call not allowed, err cause: %d",
                     cc_object_ptr->orig_request.error_cause);

        /* Post re-orig cmd with CC reject cause */
        (*(cc_object_ptr->cc_complete_cb_func))(cc_object_ptr->call_id,
                                                CM_CC_STATUS_ERROR,
                                                cc_object_ptr->call_type,
                                                cc_object_ptr->cdma_result_ptr,
                                                cc_object_ptr->gw_result_ptr,
                                               (void*)cc_object_ptr->user_data);
        cmcc_object_deallocate( cc_object_ptr );
        return;
      }

      is_fdn_enabled = cc_object_ptr->sim_capabilities.fdn_enabled;

      CM_MSG_HIGH_2("Is FDN enabled %d on session id %d", is_fdn_enabled,cc_object_ptr->sim_capabilities.response_header.session_id);

    /* Check FDN list,
    ** if FDN is enabled and if this is not a SETUP_CALL */
    CM_MSG_HIGH_1 ("client categ %d", client_categ);
    #ifdef FEATURE_MMGSDI_SESSION_LIB
    pb_info.device_type = PBM_FDN;
    pb_info.pb_category = PBM_LPB;
    pb_info.slot_id = (uint16)cc_object_ptr->sim_capabilities.response_header.slot_id;

    if(cm_mmgsdi_ssn_tbl_get_ssn_info_for_id(cc_object_ptr->sim_capabilities.response_header.session_id, &sess_info) != CM_MMGSDI_SUCCESS)
    {
      /* default to SLOT-1 and GW Primary*/
      pb_info.slot_id   = (uint16)MMGSDI_SLOT_1;
      sess_info.session_type = MMGSDI_GW_PROV_PRI_SESSION;
      CM_MSG_HIGH_0 ("set to default slot id n prov type");
    }

    pb_info.prov_type  = cmcc_map_mmgsdi_to_pbm_session_type(
                                                      sess_info.session_type);
    CM_MSG_HIGH_3 ("ph info slot id: %d prov type: %d, ssn type %d",
                                 pb_info.slot_id,
                                 pb_info.prov_type, sess_info.session_type);
    #endif
    if ( client_categ != CM_ORIG_CLIENT_CATEG_CARD &&
         is_fdn_enabled == TRUE && is_fdn_to_check
       )
    {
      CM_MSG_HIGH_1( "FDN checking..for call type %d", cc_object_ptr->call_type );

      if (cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      {
        #ifdef FEATURE_PACKET_CALL_FDN_CHECK
        #ifdef FEATURE_MMGSDI_SESSION_LIB

        if (PBM_SUCCESS != pbm_session_find_number(pb_info,
                               (const byte *)CMCC_PACKET_CALL_FDN_STR,
                               (int)strlen(CMCC_PACKET_CALL_FDN_STR),
                               NULL, NULL))
        #else /* FEATURE_MMGSDI_SESSION_LIB */
        if (PBM_SUCCESS != pbm_find_number(PBM_FDN,
                              (const byte *)CMCC_PACKET_CALL_FDN_STR,
                          (int)strlen(CMCC_PACKET_CALL_FDN_STR), NULL, NULL))
        #endif /* FEATURE_MMGSDI_SESSION_LIB */
        {
          CM_MSG_HIGH_0("FDN failed");
          is_call_allowed_cdma = FALSE;
          is_call_allowed_gw = FALSE;
          cc_object_ptr->orig_request.error_cause =
                                            CM_CALL_CMD_ERR_CAUSE_FDN_FAIL;
          cc_object_ptr->orig_request.call_control_result =
                                                 CM_CC_RESULT_NOT_ALLOWED;
        } /* pbm_find_number */
        #endif /* FEATURE_PACKET_CALL_FDN_CHECK */
      } /* ps data */
      else
      {
        /* voice or CS data or sups */
        #ifdef FEATURE_MMGSDI_SESSION_LIB
        if (PBM_SUCCESS != pbm_session_find_number(pb_info,
                               cc_object_ptr->orig_request.num.buf,
                               cc_object_ptr->orig_request.num.len,
                               NULL, NULL))
        #else
        if (PBM_SUCCESS != pbm_find_number(PBM_FDN,
                           cc_object_ptr->orig_request.num.buf,
                           cc_object_ptr->orig_request.num.len,
                           NULL, NULL))
        #endif
        {
          CM_MSG_HIGH_0("FDN failed");
          is_call_allowed_cdma = FALSE;
          is_call_allowed_gw = FALSE;
          cc_object_ptr->orig_request.error_cause =
                                 CM_CALL_CMD_ERR_CAUSE_FDN_FAIL;
          cc_object_ptr->orig_request.call_control_result =
                                      CM_CC_RESULT_NOT_ALLOWED;
        } /* pbm_find_number() */
      } /* voice or CS data */

    }
    /* Now the call has passed the FDN checks for being allowed  */

  }

  if (!is_call_allowed_cdma && !is_call_allowed_gw)
  {
    CM_MSG_HIGH_1("Call not allowed, err cause: %d",
                 cc_object_ptr->orig_request.error_cause);
    /* The call is not allowed on either system, so say no to both. */
    (*(cc_object_ptr->cc_complete_cb_func))(cc_object_ptr->call_id,
                                            CM_CC_STATUS_SUCCESS,
                                            cc_object_ptr->call_type,
                                            cc_object_ptr->cdma_result_ptr,
                                            cc_object_ptr->gw_result_ptr,
                                           (void*)cc_object_ptr->user_data);
    cmcc_object_deallocate( cc_object_ptr );
    return;
  }

  /* build the envelope command info */
  /* For USIM App only, we need to check if CC uis required or not.
   ** For other Apps, its CC Required, by default.
   ** This is as per spec:11.11 for GSM
   */

  #if defined(FEATURE_MMGSDI_SESSION_LIB)
  app_type = cm_mmgsdi_ssn_tbl_get_app_type_for_type(sess_info.session_type);
  CM_MSG_HIGH_1("CC: app_type=%d", app_type);

  if(app_type != MMGSDI_APP_NONE)
  {
    if (cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA)
    {
      if ( app_type != MMGSDI_APP_USIM )
      {
        CM_MSG_HIGH_0( "USIM App. SIM CC required");
        cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_REQUIRED;
        cc_required_fn = NULL;
      }
      else
      {
        #ifdef FEATURE_GPRS_CALLCONTROL
        cc_required_fn =  mmgsdi_session_is_service_available;
        mmgsdi_srv_type = MMGSDI_USIM_SRV_GPRS_CC;
        #else
        cc_required_fn = NULL;
        #endif
      }
    }
    else
    {
      cc_required_fn = mmgsdi_session_is_service_available;

      switch (app_type)
      {
        case MMGSDI_APP_SIM:
          mmgsdi_srv_type = MMGSDI_GSM_SRV_CALL_CONTROL;
          break;

        case MMGSDI_APP_RUIM:
          mmgsdi_srv_type = MMGSDI_CDMA_SRV_CALL_CONTROL;
          break;

        case MMGSDI_APP_USIM:
          mmgsdi_srv_type = MMGSDI_USIM_SRV_CALL_CONTROL;
          break;

        case MMGSDI_APP_CSIM:
          mmgsdi_srv_type = MMGSDI_CSIM_SRV_CALL_CONTROL;
          break;

        default:
          mmgsdi_srv_type = MMGSDI_NONE;
          CM_ERR_0 ("Invalid app type from MMGSDI");
      }
    }
  }

  /* If either subscription comes from SIM */
  if ( is_call_allowed_cdma || is_call_allowed_gw )
  {
    /* Check if CC is required from SIM1.
     * If so, set a variable to remember that CC is required
     * for those subscriptions.
     */
    /*lint -save -e774 Boolean within 'if' always evaluates to False */
    if (mmgsdi_result == CM_MMGSDI_SUCCESS)
    {
      if (cc_required_fn != NULL)
      {
        CM_MSG_HIGH_1("calling cc_required_fn, call_id=%d", cc_object_ptr->call_id);
        if(cc_required_fn(sess_info.session_id,
                          mmgsdi_srv_type,
                          cmcc_service_available_cb,
                          (uint32)cc_object_ptr ) != MMGSDI_SUCCESS)
      {
        CM_MSG_HIGH_0 ("MMGSDI is_service_available False. CC not req");
        cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
      }
      else
      {
        if (cm_wait_for_mmgsdi() == FALSE)
        {
          cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
        }

        if (cm_mmgsdi_srv_available_cnf_ptr != NULL &&
                  cm_mmgsdi_srv_available_cnf_ptr->srv_available == TRUE)
        {
          cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_REQUIRED;
          CM_MSG_HIGH_0("SIM CC required");
        }
        else
        {
          cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
          CM_MSG_HIGH_0("SIM CC not required");
        }

        /* Free the cb memory */
        if (cm_mmgsdi_srv_available_cnf_ptr != NULL)
        {
          cm_mem_free(cm_mmgsdi_srv_available_cnf_ptr);
          cm_mmgsdi_srv_available_cnf_ptr = NULL;
        }
      }
    }
     else
     {
       cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
       CM_MSG_HIGH_0("cb null.SIM CC not required");
     }
   }
  }


  /* ACL checking for PS calls */
  if (cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    if(is_call_allowed_gw == TRUE)
    {
      boolean is_acl_enabled = FALSE;

      #ifdef FEATURE_CM_LTE
      if (cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
      {
        #ifdef FEATURE_MMGSDI_SESSION_LIB
        if ((cm_mmgsdi_acl_availability == CM_MMGSDI_ACL_AVAIL_ENABLED )
              &&(cm_acl_status == TRUE))
        {
          is_acl_enabled = TRUE;
        }
        else
        {
          is_acl_enabled = FALSE;
        }
        #endif

        CM_MSG_HIGH_1("is_acl_enabled=%d",is_acl_enabled);

      }
      else
      #endif
      {
        is_acl_enabled = cc_object_ptr->sim_capabilities.acl_enabled;
      }

      CM_MSG_HIGH_1("is_acl_enabled=%d",is_acl_enabled);

      /* read ACL from GW subscription source if applicable */
      if ( is_acl_enabled )
      {
        boolean status = FALSE;

        /*
        ** Read ACL file.
        */
        status = cmmmgsdi_read_acl_file(cc_object_ptr);
        CM_MSG_HIGH_1("read acl status=%d",status);

        if ( status == FALSE )
        {
          /* can't read ACL.  May not exist.  Continue with envelope command
          */
          CM_MSG_HIGH_0("MMGSDI_USIM_ACL file read not success");
        }
        else
        {
          /* Next processing happens in ACL read callback */
          return;
        }
      }
    }
  }
  /* We reach here if
  **    it is PS data call and ACL is not applicable OR
  **    it is not PS_DATA call OR
  **    GSDI ACL read failed
  ** In all above cases, we will send Envelope command.
  */
  #ifdef FEATURE_CM_LTE
  if (cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
  {
    if(!cc_object_ptr->srv_available)
    {
      cc_object_ptr->orig_request.call_control_result =
                                    CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

      cc_object_ptr->gw_result_ptr = &cc_object_ptr->orig_request;
      CM_MSG_HIGH_1("Call allowed, srv_available = %d",
                     cc_object_ptr->srv_available);

      /* CC complete, invoke CM callback */
      (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                                             cc_object_ptr->status,
                                             cc_object_ptr->call_type,
                                             cc_object_ptr->cdma_result_ptr,
                                             cc_object_ptr->gw_result_ptr,
                                             (void*) cc_object_ptr->user_data);

      /* This sequence is complete, so free the memory. */
      cmcc_object_deallocate( cc_object_ptr );

    }
    else
    {
      cmcc_send_cc_command(cc_object_ptr);
    }
  }
  else
  #endif
  {
    #ifndef FEATURE_GPRS_CALLCONTROL
      if (cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA)
      {
         /* CC complete, invoke CM callback */
         if( cc_object_ptr->cc_complete_cb_func != NULL )
         {
         (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                                             cc_object_ptr->status,
                                             cc_object_ptr->call_type,
                                             cc_object_ptr->cdma_result_ptr,
                                             cc_object_ptr->gw_result_ptr,
                                             (void*) cc_object_ptr->user_data);
        /* This sequence is complete, so free the memory. */
        cmcc_object_deallocate( cc_object_ptr );
        return ;
      }


      }
    #endif

    CM_MSG_HIGH_1("sim_cc_required=%d", cc_object_ptr->sim_cc_required);

    if (cc_object_ptr->sim_cc_required != CMCC_CARD_STATUS_CC_REQUIRED)
    {

       /* CC complete, invoke CM callback */
      if( cc_object_ptr->cc_complete_cb_func != NULL )
      {
       (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                                             cc_object_ptr->status,
                                             cc_object_ptr->call_type,
                                             cc_object_ptr->cdma_result_ptr,
                                             cc_object_ptr->gw_result_ptr,
                                             (void*) cc_object_ptr->user_data);
       /* This sequence is complete, so free the memory. */
       cmcc_object_deallocate( cc_object_ptr );
       return;
    }

    }

    /* If the call is to be placed on WLAN, we would not have any indication from SD.
    ** So, call control should proceed like SUPS.
    */
    if(cc_object_ptr->call_type != CM_CALL_TYPE_SUPS &&
       cc_object_ptr->orig_request.voip_cc_sys_mode != SYS_SYS_MODE_WLAN)
    {
      /* CC complete, invoke CM callback and don't deallocate the cc_object_ptr
         it will be freed while CM gets ok_to_orig from SD*/
      if (cc_object_ptr->cc_complete_cb_func != NULL )
      {
        (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                                             cc_object_ptr->status,
                                             cc_object_ptr->call_type,
                                             cc_object_ptr->cdma_result_ptr,
                                             cc_object_ptr->gw_result_ptr,
                                             (void*) cc_object_ptr->user_data);
        if(call_ptr != NULL)
        {
          /* Removing cmd_ptr address from cc_obj_ptr as it is pushed into CM queue*/
          cc_object_ptr->user_data = NULL;
          call_ptr->cc_object_ptr = cc_object_ptr;
          cc_object_ptr->is_cc_delayed = TRUE;
        }
        else
        {
          CM_MSG_HIGH_0 ("cmcc_call_ctrl_after_sim_capabilities call_ptr is NULL");
          cmcc_object_deallocate( cc_object_ptr );
        }
      }
    }
    else
    {
      cmcc_send_cc_command(cc_object_ptr);
    }
  }


#endif /* defined(FEATURE_GSTK)  */
} /* cmcc_call_ctrl_after_sim_capabilities */


/*===========================================================================
FUNCTION cmcc_sim_cap_cb

DESCRIPTION
 Callback for gsdi_get_sim_capabilities

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(765, cmcc_sim_cap_cb)
** Can't be static, unit test uses it
*/
/*lint -esym(714, cmcc_sim_cap_cb) */
void cmcc_sim_cap_cb
(
  mmgsdi_return_enum_type    status,
     /* Status sent by MMGSDI */
  mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
  mmgsdi_cnf_type*           gsdi_cnf
   /* confirmation info received from MMGSDI */
)
/*lint -esym(818, gsdi_cnf)*/
{
  cm_generic_rpt_s_type *cm_generic_rpt_ptr = NULL;

  cmcc_object_s_type *cc_object_ptr = NULL;

  cm_call_id_type    call_id;
  


  CM_MSG_HIGH_2("Sim cap callback, status=%d, cnf=%d",status,cnf);

  if (gsdi_cnf == NULL)
  {
    CM_ERR_0("gsdi_cnf is NULL");
    return;
  }

  call_id = (cm_call_id_type)gsdi_cnf->response_header.client_data;

  if(call_id > CMCALL_MAX_CALL_OBJ)
  {
    call_id = CMCALL_MAX_CALL_OBJ;
  }

  if (cmmmgsdi_validate_sim_read_req(call_id))
  {
    
    cc_object_ptr = (cmcc_object_s_type*)
                  cmmmgsdi_ptr()->mmgsdi_read_req_uptime[call_id]->cc_object_ptr;
    cmmmgsdi_deallocate_sim_read_req(call_id);

  if (cc_object_ptr == NULL)
  {
    CM_ERR_0("cc_object_ptr is NULL");
    return;
  }

  if (status == MMGSDI_SUCCESS && cnf == MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF)
  {
    cc_object_ptr->sim_capabilities = gsdi_cnf->session_app_capabilities_cnf;
    cc_object_ptr->sim_capabilities_valid = TRUE;
  }
  else
  {
    cc_object_ptr->sim_capabilities_valid = FALSE;
  }

    CM_MSG_HIGH_3("CC Sim cap callback, sim_capabilities_valid=%d, call_id=%d, call_type=%d",
                  cc_object_ptr->sim_capabilities_valid,
                  cc_object_ptr->call_id,
                  cc_object_ptr->call_type);

  cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
    /* Create the report */
    cm_generic_rpt_ptr->hdr.cmd = CM_CALL_CTRL_RPT;
    cm_generic_rpt_ptr->generic_rpt.call_ctrl_rpt.cc_object_ptr =
                      (void*)cc_object_ptr;

    /* Queue the report internally */
    cm_generic_rpt(cm_generic_rpt_ptr);
  }


} /* cmcc_sim_cap_cb */
/*lint -esym(818, gsdi_cnf)*/


#if defined(FEATURE_GSTK)

/*===========================================================================
FUNCTION cmcc_match_plmn_and_lac_id

DESCRIPTION
  This function will compare the plmn id and lac id present in sys_id and cell_info
  if both are equal return true else return false.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean cmcc_match_plmn_and_lac_id
(
  const cm_mm_ss_info_s_type *ss_info_ptr,
  cm_ss_e_type         ss
)
{
  boolean result = TRUE;
  if(ss == CM_SS_HYBR_2)
  {
    if(ss_info_ptr->gw_cell_info.lac_id != ss_info_ptr->gw_sys_id.id.plmn_lac.lac)
    {
      result = FALSE;
    }
    if((ss_info_ptr->gw_cell_info.plmn_id.identity[0] != ss_info_ptr->gw_sys_id.id.plmn_lac.plmn.identity[0]) ||
      (ss_info_ptr->gw_cell_info.plmn_id.identity[1] != ss_info_ptr->gw_sys_id.id.plmn_lac.plmn.identity[1]) ||
      (ss_info_ptr->gw_cell_info.plmn_id.identity[2] != ss_info_ptr->gw_sys_id.id.plmn_lac.plmn.identity[2]))
    {
      result = FALSE;
    }
  }
  else if(ss == CM_SS_HYBR_3)
  {
    if(ss_info_ptr->gw3_cell_info.lac_id != ss_info_ptr->gw3_sys_id.id.plmn_lac.lac)
    {
      result = FALSE;
    }
    if((ss_info_ptr->gw3_cell_info.plmn_id.identity[0] != ss_info_ptr->gw3_sys_id.id.plmn_lac.plmn.identity[0]) ||
      (ss_info_ptr->gw3_cell_info.plmn_id.identity[1] != ss_info_ptr->gw3_sys_id.id.plmn_lac.plmn.identity[1]) ||
      (ss_info_ptr->gw3_cell_info.plmn_id.identity[2] != ss_info_ptr->gw3_sys_id.id.plmn_lac.plmn.identity[2]))
    {
      result = FALSE;
    }
  }
  else
  {
    if(ss_info_ptr->cell_info.lac_id != ss_info_ptr->sys_id.id.plmn_lac.lac)
    {
      result = FALSE;
    }
    if((ss_info_ptr->cell_info.plmn_id.identity[0] != ss_info_ptr->sys_id.id.plmn_lac.plmn.identity[0]) ||
      (ss_info_ptr->cell_info.plmn_id.identity[1] != ss_info_ptr->sys_id.id.plmn_lac.plmn.identity[1]) ||
      (ss_info_ptr->cell_info.plmn_id.identity[2] != ss_info_ptr->sys_id.id.plmn_lac.plmn.identity[2]))
    {
      result = FALSE;
    }
  }

  return result;
}

/*===========================================================================
FUNCTION cmcc_map_sys_mode_to_rat

DESCRIPTION
  This function will convert the sys mode to gstk_access_technology

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static gstk_access_technology_type cmcc_map_sys_mode_to_rat
(
  sys_sys_mode_e_type sys_mode
)
{
  switch(sys_mode)
  {
    case SYS_SYS_MODE_CDMA:
      return GSTK_ACCESS_TECH_CDMA;

    case SYS_SYS_MODE_GSM:
      return GSTK_ACCESS_TECH_GSM;

    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      return GSTK_ACCESS_TECH_UTRAN;

    case SYS_SYS_MODE_LTE:
      return GSTK_ACCESS_TECH_LTE;

    case SYS_SYS_MODE_WLAN:
      return GSTK_ACCESS_TECH_WLAN;

    default:
      CM_MSG_HIGH_1("Wrong value of sys_mode received in cmcc_map_sys_mode_to_rat sys_mode %d", sys_mode);
      return GSTK_ACCESS_NONE;
  }
}


/*===========================================================================
FUNCTION cmcc_send_cc_command

DESCRIPTION
  This function will send envelope command to card

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cc_object_ptr may be modified if the request fails.

===========================================================================*/
static void cmcc_send_cc_command
(
  cmcc_object_s_type *cc_object_ptr
  /* Pointer to CC object type */
)
{
  gstk_call_enum_type gstk_call_type = GSTK_VOICE;

  cm_cmd_type *cmd_ptr = NULL;

  gstk_slot_id_enum_type gstk_slot = GSTK_SLOT_ID_MAX;

  cm_mm_ss_info_s_type *ss_info_ptr      = &(cmss_ptr()->info);

  cmcall_s_type                *call_ptr;


  if (cc_object_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  call_ptr = cmcall_ptr (cc_object_ptr->call_id);

  cmd_ptr = (cm_cmd_type*) cc_object_ptr->user_data;

  if (cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA)
  {
    /* If GSTK will accept this, set up the type, otherwise,
    ** don't send to GSTK
    */
    #if defined (FEATURE_GPRS_CALLCONTROL)
    gstk_call_type = GSTK_PDP_CONTEXT_ACT;
    #else
    cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
    #endif /* FEATURE_GPRS_CALLCONTROL */

    #ifdef FEATURE_CM_LTE
    if(cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
    {
      gstk_call_type = GSTK_EPS_PDN_CONN_ACT;
      cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_REQUIRED;
    }
    #endif
  }
  else if (cc_object_ptr->call_type == CM_CALL_TYPE_SUPS)
  {
    gstk_call_type = GSTK_SS;
  }

  CM_MSG_HIGH_4 ("Send_cc_cmd: call_id=%d  call_type=%d sim cc req %d  gstk_call_type %d",
                cc_object_ptr->call_id, cc_object_ptr->call_type,
                cc_object_ptr->sim_cc_required, gstk_call_type);


  /* If we are required to send the command to SIM1 */
  if (cc_object_ptr->sim_cc_required == CMCC_CARD_STATUS_CC_REQUIRED)
  {
    byte *buf = cc_object_ptr->orig_request.num.buf;
    uint8 length = cc_object_ptr->orig_request.num.len;
    #if defined (FEATURE_GPRS_CALLCONTROL)
    cm_gw_ps_cc_params_s_type gw_ps_cc_params =
                      cc_object_ptr->orig_request.gw_ps_cc_params;
    #endif
    #if defined (FEATURE_CM_LTE)
    cm_lte_ps_cc_params_s_type lte_ps_cc_params =
                     cc_object_ptr->orig_request.lte_ps_cc_params;
    #endif
    gstk_cc_ind_type_ext command_data;
    gstk_status_enum_type status;

    memset(&command_data, 0, sizeof(command_data));

    command_data.call_enum_type = gstk_call_type;

    switch ( gstk_call_type )
    {
      case GSTK_SS:
      {
        command_data.gstk_address_string.ss_string.TON = GSTK_RFU_TON;
        command_data.gstk_address_string.ss_string.NPI = GSTK_RFU_NPI;

        command_data.gstk_address_string.ss_string.length =
                     memscpy( command_data.gstk_address_string.ss_string.address,
                              sizeof(command_data.gstk_address_string.ss_string.address),
               buf,
               length);
        break;
      }
      case GSTK_USSD:
      {
        command_data.gstk_address_string.ussd_string.dcs = 0;
                                                /* DCS from cell broadcast */
        command_data.gstk_address_string.ussd_string.length = length;
        command_data.gstk_address_string.ussd_string.text = buf;
        break;
      }
      case GSTK_VOICE:
      {
        command_data.gstk_address_string.voice.address.TON =
                                                          GSTK_UNKNOWN_TON;
        command_data.gstk_address_string.voice.address.NPI =
                                                     GSTK_ISDN_TELEPHONY_NPI;
        if( buf[0] == '+')
        {
           command_data.gstk_address_string.voice.address.TON =
                                                      GSTK_INTERNATIONAL_NUM;
           buf++;
           length--;
        }
        /*lint -save -e534*/
        gstk_ascii_to_gstk_address_type(buf,
                                        length,
                            &command_data.gstk_address_string.voice.address);
        /*lint -restore */

        #ifdef CM_GW_SUPPORTED
        if(CALL_CMD_PTR(cmd_ptr)->info.cm_gw_mode_info_u.gsm_wcdma_info.bearer_capability_1.present ==
                                                                        TRUE)
        {
          int tmp_len = command_data.gstk_address_string.voice.ccp1.length;
          if(!(pack_bearer_capability(
             command_data.gstk_address_string.voice.ccp1.ccp,
             &CALL_CMD_PTR(cmd_ptr)->info.cm_gw_mode_info_u.gsm_wcdma_info.bearer_capability_1,
             &tmp_len )))
          {
            CM_MSG_HIGH_0("BC1 pack failed");
          }
          command_data.gstk_address_string.voice.ccp1.length = (uint8)tmp_len;
        }
        else
        #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
        {
          command_data.gstk_address_string.voice.ccp1.length = 0;
        }
        #ifdef CM_GW_SUPPORTED
        if(CALL_CMD_PTR(cmd_ptr)->info.cm_gw_mode_info_u.gsm_wcdma_info.bearer_capability_2.present ==
                                                                        TRUE)
        {
          int tmp_len = command_data.gstk_address_string.voice.ccp2.length;
          if(!(pack_bearer_capability(
             command_data.gstk_address_string.voice.ccp2.ccp,
             &CALL_CMD_PTR(cmd_ptr)->info.cm_gw_mode_info_u.gsm_wcdma_info.bearer_capability_2,
             &tmp_len )))
          {
            CM_MSG_HIGH_0("BC2 pack failed");
          }
          command_data.gstk_address_string.voice.ccp2.length = (uint8)tmp_len;
        }
        else
        #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */
        {
          command_data.gstk_address_string.voice.ccp2.length = 0;
        }
        if(CALL_CMD_PTR(cmd_ptr)->info.num.subaddr.num_char > 0)
        {
          cmcall_pack_subaddress(
             &(command_data.gstk_address_string.voice.subaddress),
             &(CALL_CMD_PTR(cmd_ptr)->info.num.subaddr));
        }
        else
        {
          command_data.gstk_address_string.voice.subaddress.length = 0;
        }

        break;
      }
      #if defined (FEATURE_GPRS_CALLCONTROL)
      case GSTK_PDP_CONTEXT_ACT:
      {
        command_data.gstk_address_string.pdp_context.length = memscpy( command_data.gstk_address_string.pdp_context.pdp_act_param,
                                                                       sizeof(command_data.gstk_address_string.pdp_context.pdp_act_param),
                                                                       gw_ps_cc_params.pdp_act_mesg,
                                                                       MIN(sizeof(gw_ps_cc_params.pdp_act_mesg),gw_ps_cc_params.length ));
        break;
      }
      #endif
      #if defined (FEATURE_CM_LTE)
      case GSTK_EPS_PDN_CONN_ACT:
      {
        command_data.gstk_address_string.eps_conn_act.length =
                                memscpy( command_data.gstk_address_string.eps_conn_act.eps_act_param,
                                         sizeof(command_data.gstk_address_string.eps_conn_act.eps_act_param),
                                         lte_ps_cc_params.pkd_msg,
                                         lte_ps_cc_params.length );
        break;
      }
      #endif

      default:
        CM_ERR_1("Unexpected call type %d", gstk_call_type);
        return;
    }

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if(cc_object_ptr->ss == CM_SS_HYBR_3)
    {
      if(ss_info_ptr->gw3_sys_mode  == SYS_SYS_MODE_LTE || cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
      {
        /* call is PS DATA or Voice and RAT is LTE, then pick the PLMN/cell id info from RRC cell info */
        command_data.rat = cmcc_map_sys_mode_to_rat(SYS_SYS_MODE_LTE);
        command_data.loc_info.gwl_info.mcc_and_mnc[0] = ss_info_ptr->gw3_cell_info.plmn_id.identity[0];
        command_data.loc_info.gwl_info.mcc_and_mnc[1] = ss_info_ptr->gw3_cell_info.plmn_id.identity[1];
        command_data.loc_info.gwl_info.mcc_and_mnc[2] = ss_info_ptr->gw3_cell_info.plmn_id.identity[2];
        command_data.loc_info.gwl_info.loc_area_code[1] = (byte)(ss_info_ptr->gw3_cell_info.tac & 0x00FF);
        command_data.loc_info.gwl_info.loc_area_code[0] = (byte)((ss_info_ptr->gw3_cell_info.tac & 0xFF00) >> 8);
        if(cmcc_match_plmn_and_lac_id(ss_info_ptr, cc_object_ptr->ss))
        {
          command_data.loc_info.gwl_info.cell_id[1] = (uint8)(ss_info_ptr->gw3_cell_info.cell_id & 0x000000FF);
          command_data.loc_info.gwl_info.cell_id[0] = (uint8)((ss_info_ptr->gw3_cell_info.cell_id & 0x0000FF00) >> 8);
          command_data.loc_info.gwl_info.cell_id[3] = (uint8)((ss_info_ptr->gw3_cell_info.cell_id & 0x00FF0000) >> 16);
          command_data.loc_info.gwl_info.cell_id[2] = (uint8)((ss_info_ptr->gw3_cell_info.cell_id & 0xFF000000) >> 24);
          if(ss_info_ptr->gw3_sys_mode != SYS_SYS_MODE_GSM)
          {
            command_data.loc_info.gwl_info.cell_id_len = 4;
          }
          else
          {
            command_data.loc_info.gwl_info.cell_id_len = 2;
          }
        }
        else
        {
          command_data.loc_info.gwl_info.cell_id[0] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[1] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[2] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[3] = 0xFF;
          command_data.loc_info.gwl_info.cell_id_len = 0;
        }
        CM_MSG_HIGH_1("cell_id_len =%d,sys_mode=SYS_SYS_MODE_LTE", command_data.loc_info.gwl_info.cell_id_len);
      }
      else
      {
      command_data.rat = cmcc_map_sys_mode_to_rat(ss_info_ptr->gw3_sys_mode);
      command_data.loc_info.gwl_info.mcc_and_mnc[0] = ss_info_ptr->gw3_sys_id.id.plmn.identity[0];
      command_data.loc_info.gwl_info.mcc_and_mnc[1] = ss_info_ptr->gw3_sys_id.id.plmn.identity[1];
      command_data.loc_info.gwl_info.mcc_and_mnc[2] = ss_info_ptr->gw3_sys_id.id.plmn.identity[2];
      command_data.loc_info.gwl_info.loc_area_code[1] = (byte)(ss_info_ptr->gw3_sys_id.id.plmn_lac.lac & 0x00FF);
      command_data.loc_info.gwl_info.loc_area_code[0] = (byte)((ss_info_ptr->gw3_sys_id.id.plmn_lac.lac & 0xFF00) >> 8);
      if(cmcc_match_plmn_and_lac_id(ss_info_ptr, cc_object_ptr->ss))
      {
        command_data.loc_info.gwl_info.cell_id[1] = (uint8)(ss_info_ptr->gw3_cell_info.cell_id & 0x000000FF);
        command_data.loc_info.gwl_info.cell_id[0] = (uint8)((ss_info_ptr->gw3_cell_info.cell_id & 0x0000FF00) >> 8);
        command_data.loc_info.gwl_info.cell_id[3] = (uint8)((ss_info_ptr->gw3_cell_info.cell_id & 0x00FF0000) >> 16);
        command_data.loc_info.gwl_info.cell_id[2] = (uint8)((ss_info_ptr->gw3_cell_info.cell_id & 0xFF000000) >> 24);
        if(ss_info_ptr->gw3_sys_mode != SYS_SYS_MODE_GSM)
        {
       command_data.loc_info.gwl_info.cell_id_len = 4;
        }
        else
        {
           command_data.loc_info.gwl_info.cell_id_len = 2;
        }
      }
      else
      {
        command_data.loc_info.gwl_info.cell_id[0] = 0xFF;
        command_data.loc_info.gwl_info.cell_id[1] = 0xFF;
        command_data.loc_info.gwl_info.cell_id[2] = 0xFF;
        command_data.loc_info.gwl_info.cell_id[3] = 0xFF;
        command_data.loc_info.gwl_info.cell_id_len = 0;
      }
      CM_MSG_HIGH_2("cell_id_len =%d,sys_mode=%d", command_data.loc_info.gwl_info.cell_id_len, ss_info_ptr->gw3_sys_mode);
    }
    }
        else
        #endif
    if(cc_object_ptr->ss == CM_SS_HYBR_2)
    {
      if(ss_info_ptr->gw_sys_mode == SYS_SYS_MODE_LTE || cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
      {
        /* call is PS DATA or VOICE and RAT is LTE, then pick the PLMN/cell id info from RRC cell info */
        command_data.rat = cmcc_map_sys_mode_to_rat(SYS_SYS_MODE_LTE);
        command_data.loc_info.gwl_info.mcc_and_mnc[0] = ss_info_ptr->gw_cell_info.plmn_id.identity[0];
        command_data.loc_info.gwl_info.mcc_and_mnc[1] = ss_info_ptr->gw_cell_info.plmn_id.identity[1];
        command_data.loc_info.gwl_info.mcc_and_mnc[2] = ss_info_ptr->gw_cell_info.plmn_id.identity[2];
        command_data.loc_info.gwl_info.loc_area_code[1] = (byte)(ss_info_ptr->gw_cell_info.tac & 0x00FF);
        command_data.loc_info.gwl_info.loc_area_code[0] = (byte)((ss_info_ptr->gw_cell_info.tac & 0xFF00) >> 8);
        if(cmcc_match_plmn_and_lac_id(ss_info_ptr, cc_object_ptr->ss))
        {
          command_data.loc_info.gwl_info.cell_id[1] = (uint8)(ss_info_ptr->gw_cell_info.cell_id & 0x000000FF);
          command_data.loc_info.gwl_info.cell_id[0] = (uint8)((ss_info_ptr->gw_cell_info.cell_id & 0x0000FF00) >> 8);
          command_data.loc_info.gwl_info.cell_id[3] = (uint8)((ss_info_ptr->gw_cell_info.cell_id & 0x00FF0000) >> 16);
          command_data.loc_info.gwl_info.cell_id[2] = (uint8)((ss_info_ptr->gw_cell_info.cell_id & 0xFF000000) >> 24);
          if(ss_info_ptr->gw_sys_mode != SYS_SYS_MODE_GSM)
          {
            command_data.loc_info.gwl_info.cell_id_len = 4;
          }
          else
          {
            command_data.loc_info.gwl_info.cell_id_len = 2;
          }
        }
        else
        {
          command_data.loc_info.gwl_info.cell_id[0] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[1] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[2] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[3] = 0xFF;
          command_data.loc_info.gwl_info.cell_id_len = 0;
        }
        CM_MSG_HIGH_1("cell_id_len =%d,sys_mode=SYS_SYS_MODE_LTE", command_data.loc_info.gwl_info.cell_id_len);
      }
      else
      {
        command_data.rat = cmcc_map_sys_mode_to_rat(ss_info_ptr->gw_sys_mode);
        command_data.loc_info.gwl_info.mcc_and_mnc[0] = ss_info_ptr->gw_sys_id.id.plmn.identity[0];
        command_data.loc_info.gwl_info.mcc_and_mnc[1] = ss_info_ptr->gw_sys_id.id.plmn.identity[1];
        command_data.loc_info.gwl_info.mcc_and_mnc[2] = ss_info_ptr->gw_sys_id.id.plmn.identity[2];
        command_data.loc_info.gwl_info.loc_area_code[1] = (byte)(ss_info_ptr->gw_sys_id.id.plmn_lac.lac & 0x00FF);
        command_data.loc_info.gwl_info.loc_area_code[0] = (byte)((ss_info_ptr->gw_sys_id.id.plmn_lac.lac & 0xFF00) >> 8);
        if(cmcc_match_plmn_and_lac_id(ss_info_ptr, cc_object_ptr->ss))
        {
          command_data.loc_info.gwl_info.cell_id[1] = (uint8)(ss_info_ptr->gw_cell_info.cell_id & 0x000000FF);
          command_data.loc_info.gwl_info.cell_id[0] = (uint8)((ss_info_ptr->gw_cell_info.cell_id & 0x0000FF00) >> 8);
          command_data.loc_info.gwl_info.cell_id[3] = (uint8)((ss_info_ptr->gw_cell_info.cell_id & 0x00FF0000) >> 16);
          command_data.loc_info.gwl_info.cell_id[2] = (uint8)((ss_info_ptr->gw_cell_info.cell_id & 0xFF000000) >> 24);
          if(ss_info_ptr->gw_sys_mode != SYS_SYS_MODE_GSM)
          {
            command_data.loc_info.gwl_info.cell_id_len = 4;
          }
          else
          {
            command_data.loc_info.gwl_info.cell_id_len = 2;
          }
        }
        else
        {
          command_data.loc_info.gwl_info.cell_id[0] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[1] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[2] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[3] = 0xFF;
          command_data.loc_info.gwl_info.cell_id_len = 0;
        }
        CM_MSG_HIGH_2("cell_id_len =%d,sys_mode=%d", command_data.loc_info.gwl_info.cell_id_len, ss_info_ptr->gw_sys_mode);
      }
    }
    else
    {

      if(ss_info_ptr->sys_mode == SYS_SYS_MODE_LTE || cc_object_ptr->orig_request.ps_cc_sys_mode == SYS_SYS_MODE_LTE)
      {
        /* call is PS DATA or Voice and RAT is LTE, then pick the PLMN/cell id info from RRC cell info */
        command_data.rat = cmcc_map_sys_mode_to_rat(SYS_SYS_MODE_LTE);
        command_data.loc_info.gwl_info.mcc_and_mnc[0] = ss_info_ptr->cell_info.plmn_id.identity[0];
        command_data.loc_info.gwl_info.mcc_and_mnc[1] = ss_info_ptr->cell_info.plmn_id.identity[1];
        command_data.loc_info.gwl_info.mcc_and_mnc[2] = ss_info_ptr->cell_info.plmn_id.identity[2];
        command_data.loc_info.gwl_info.loc_area_code[1] = (byte)(ss_info_ptr->cell_info.tac & 0x00FF);
        command_data.loc_info.gwl_info.loc_area_code[0] = (byte)((ss_info_ptr->cell_info.tac & 0xFF00) >> 8);
        if(cmcc_match_plmn_and_lac_id(ss_info_ptr, cc_object_ptr->ss))
        {
          command_data.loc_info.gwl_info.cell_id[1] = (uint8)(ss_info_ptr->cell_info.cell_id & 0x000000FF);
          command_data.loc_info.gwl_info.cell_id[0] = (uint8)((ss_info_ptr->cell_info.cell_id & 0x0000FF00) >> 8);
          command_data.loc_info.gwl_info.cell_id[3] = (uint8)((ss_info_ptr->cell_info.cell_id & 0x00FF0000) >> 16);
          command_data.loc_info.gwl_info.cell_id[2] = (uint8)((ss_info_ptr->cell_info.cell_id & 0xFF000000) >> 24);
          if(ss_info_ptr->gw_sys_mode != SYS_SYS_MODE_GSM)
          {
            command_data.loc_info.gwl_info.cell_id_len = 4;
          }
          else
          {
            command_data.loc_info.gwl_info.cell_id_len = 2;
          }
        }
        else
        {
          command_data.loc_info.gwl_info.cell_id[0] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[1] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[2] = 0xFF;
          command_data.loc_info.gwl_info.cell_id[3] = 0xFF;
          command_data.loc_info.gwl_info.cell_id_len = 0;
        }
        CM_MSG_HIGH_1("cell_id_len =%d,sys_mode=SYS_SYS_MODE_LTE", command_data.loc_info.gwl_info.cell_id_len);
      }
      else if (cc_object_ptr->orig_request.voip_cc_sys_mode == SYS_SYS_MODE_WLAN)
      {
        command_data.rat = cmcc_map_sys_mode_to_rat(SYS_SYS_MODE_WLAN);
        // No location information is required in case of WLAN, as per UIM.
      }
      else
      {
        command_data.rat = cmcc_map_sys_mode_to_rat(ss_info_ptr->sys_mode);

        if(ss_info_ptr->sys_mode == SYS_SYS_MODE_CDMA)
        {
          command_data.loc_info.cdma_info.base_id = ss_info_ptr->mode_info.cdma_info.base_id;
          command_data.loc_info.cdma_info.base_lat = ss_info_ptr->mode_info.cdma_info.base_lat;
          command_data.loc_info.cdma_info.base_long = ss_info_ptr->mode_info.cdma_info.base_long;
          command_data.loc_info.cdma_info.imsi_11_12 = ss_info_ptr->sys_id.id.is95.imsi_11_12;
          command_data.loc_info.cdma_info.mcc = ss_info_ptr->sys_id.id.is95.mcc;
          command_data.loc_info.cdma_info.nid = ss_info_ptr->sys_id.id.is95.nid;
          command_data.loc_info.cdma_info.sid = ss_info_ptr->sys_id.id.is95.sid;
        }
        else
        {
          command_data.loc_info.gwl_info.mcc_and_mnc[0] = ss_info_ptr->sys_id.id.plmn.identity[0];
          command_data.loc_info.gwl_info.mcc_and_mnc[1] = ss_info_ptr->sys_id.id.plmn.identity[1];
          command_data.loc_info.gwl_info.mcc_and_mnc[2] = ss_info_ptr->sys_id.id.plmn.identity[2];
          command_data.loc_info.gwl_info.loc_area_code[1] = (byte)(ss_info_ptr->sys_id.id.plmn_lac.lac & 0x00FF);
          command_data.loc_info.gwl_info.loc_area_code[0] = (byte)((ss_info_ptr->sys_id.id.plmn_lac.lac & 0xFF00) >> 8);
          if(cmcc_match_plmn_and_lac_id(ss_info_ptr, cc_object_ptr->ss))
          {
            command_data.loc_info.gwl_info.cell_id[1] = (uint8)(ss_info_ptr->cell_info.cell_id & 0x000000FF);
            command_data.loc_info.gwl_info.cell_id[0] = (uint8)((ss_info_ptr->cell_info.cell_id & 0x0000FF00) >> 8);
            command_data.loc_info.gwl_info.cell_id[3] = (uint8)((ss_info_ptr->cell_info.cell_id & 0x00FF0000) >> 16);
            command_data.loc_info.gwl_info.cell_id[2] = (uint8)((ss_info_ptr->cell_info.cell_id & 0xFF000000) >> 24);
            if(ss_info_ptr->sys_mode != SYS_SYS_MODE_GSM)
            {
              command_data.loc_info.gwl_info.cell_id_len = 4;
            }
            else
            {
              command_data.loc_info.gwl_info.cell_id_len = 2;
            }
          }
          else
          {
            command_data.loc_info.gwl_info.cell_id[0] = 0xFF;
            command_data.loc_info.gwl_info.cell_id[1] = 0xFF;
            command_data.loc_info.gwl_info.cell_id[2] = 0xFF;
            command_data.loc_info.gwl_info.cell_id[3] = 0xFF;
            command_data.loc_info.gwl_info.cell_id_len = 0;
          }
          CM_MSG_HIGH_2("cell_id_len =%d,sys_mode=%d", command_data.loc_info.gwl_info.cell_id_len, ss_info_ptr->sys_mode);
        }
      }
    }
    if(cc_object_ptr->gw_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      if (cc_object_ptr->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)
      {
        gstk_slot = GSTK_SLOT_1;
      }
      else if (cc_object_ptr->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)
      {
        gstk_slot = GSTK_SLOT_2;
      }
      else if (cc_object_ptr->gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)
      {
        gstk_slot = GSTK_SLOT_3;
      }
      else
      {
        CM_ERR_1 ("Invalid GW Subscription Source for Call Control %d!", cc_object_ptr->gw_subscription_source);
      }
    }
    else if(cc_object_ptr->cdma_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      if (cc_object_ptr->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_1)
      {
        gstk_slot = GSTK_SLOT_1;
      }
      else if(cc_object_ptr->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)
      {
        gstk_slot = GSTK_SLOT_2;
      }
      else if(cc_object_ptr->cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)
      {
        gstk_slot = GSTK_SLOT_3;
      }
      else
      {
        CM_ERR_1 ("Invalid GW Subscription Source for Call Control %d!", cc_object_ptr->gw_subscription_source);
      }
    }

    /* build the envelope command and send it to GSTK */
    status = gstk_slot_send_envelope_cc_command_ext( gstk_slot,
                                             (uint32) cc_object_ptr,
                                             &command_data,
                                             cmcc_gstk_rsp_cb );

    /* This has been done so that cc_object_ptr will be freed only in cmcc_gstk_rsp_cb
       ** and not in cmcall_end*/
    if(call_ptr!= NULL)
    {
      call_ptr->cc_object_ptr = NULL;
    }

    if( status != GSTK_SUCCESS )
    {
      /* Envelope command send failed */
      cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
      cc_object_ptr->status = CM_CC_STATUS_ERROR;
      cc_object_ptr->orig_request.call_control_result =
                                                    CM_CC_RESULT_NOT_ALLOWED;
      cc_object_ptr->orig_request.error_cause =
                                          CM_CALL_CMD_ERR_CAUSE_OTHER;

      CM_MSG_HIGH_2("GSTK send env cmd failed with status %d, Call not allowed err cause %d",
                    status, cc_object_ptr->orig_request.error_cause);

      /* CC complete, invoke CM callback */
      (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                                 cc_object_ptr->status,
                                 cc_object_ptr->call_type,
                                 cc_object_ptr->cdma_result_ptr,
                                 cc_object_ptr->gw_result_ptr,
                                 (void*) cc_object_ptr->user_data);
      if(cc_object_ptr->is_cc_delayed == FALSE)
      {
        /* This sequence is complete, so free the memory. */
        cmcc_object_deallocate( cc_object_ptr );
      }
      /* If cc is delayed then cc_object_ptr will be freed in reorig_phase2
          otherwise it will lead to crash in ok_to_orig report*/
    }
  }
  else
  {
    CM_MSG_HIGH_0("SIM CC not reqd, call allowed");
    /* If we reach here, SIM CC is not required. Invoke CM callback */
    (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                               cc_object_ptr->status,
                               cc_object_ptr->call_type,
                               cc_object_ptr->cdma_result_ptr,
                               cc_object_ptr->gw_result_ptr,
                               (void*) cc_object_ptr->user_data);
    if(cc_object_ptr->is_cc_delayed == FALSE)
    {
      /* This sequence is complete, so free the memory. */
      cmcc_object_deallocate( cc_object_ptr );
    }
    /* If cc is delayed then cc_object_ptr will be freed in reorig_phase2
          otherwise it will lead to crash in ok_to_orig report*/
  }
} /* cmcc_send_cc_command */
#endif /* defined(FEATURE_GSTK)   */

/*===========================================================================

FUNCTION NAME cmcc_service_available_cb

DESCRIPTION
  Function is called by MMGSDI when finishing processing is_service_available request.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(765, cmcc_service_available_cb)
** Can't be static, unit test uses it
*/
void cmcc_service_available_cb(
    mmgsdi_return_enum_type    status,
     /* Status sent by MMGSDI */
    mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
    const mmgsdi_cnf_type      *cnf_ptr
     /* conf pointer returned by MMGSDI */
)
{
  if (cnf_ptr == NULL)
  {
    cm_mmgsdi_srv_available_cnf_ptr = NULL;

    CM_MSG_HIGH_0("cnf_ptr is NULL");
  }
  else
  {
    if (cnf == MMGSDI_SRV_AVAILABLE_CNF)
    {
      /* Dynamically allocate memory */
      cm_mmgsdi_srv_available_cnf_ptr =
         cm_mem_malloc( sizeof(mmgsdi_srv_available_cnf_type));

      *cm_mmgsdi_srv_available_cnf_ptr = *(&(cnf_ptr->srv_available_cnf));

      /* If the status is not MMGSDI_SUCCESS, srv is not available. */
      if (status != MMGSDI_SUCCESS)
      {
        cm_mmgsdi_srv_available_cnf_ptr->srv_available = FALSE;

        CM_MSG_HIGH_1("cmcc_service_available_cb(), status = %d", status);
      }
      else
      {
        CM_MSG_HIGH_1("cmcc_service_available_cb(), srv_available = %d",
                         cm_mmgsdi_srv_available_cnf_ptr->srv_available );
      }

    }
    else
    {
      CM_MSG_HIGH_1("Invalid cnf %d in cmcc_service_available_cb",
                  cnf);
    }

  }


  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}
/*===========================================================================
FUNCTION cmcc_call_control_use_gw_session

DESCRIPTION
  check with subscription should be used for FDN checking

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

static boolean cmcc_call_control_use_gw_session
(
    cm_call_id_type call_id
    /* call id */
)
{
  cmcall_s_type *call_ptr = cmcall_ptr(call_id);
  cmss_s_type  * ss_ptr = cmss_ptr();

  if(call_ptr != NULL)
  {
    if(call_ptr->ss == CM_SS_MAIN)
    {
      /*3gpp only*/
      if(is_contain_3GPP_mode_pref(call_ptr->favored_mode_pref) &&
        !cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_CDMA_ONLY))
      {
        return TRUE;
      }
      /*1x only*/
      if(!is_contain_3GPP_mode_pref(call_ptr->favored_mode_pref) &&
        cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_CDMA_ONLY))
      {
        return FALSE;
      }
     /* automatic */
     if(is_contain_3GPP_mode_pref(call_ptr->favored_mode_pref) &&
       cmcall_misc_is_mode_pref(call_ptr->favored_mode_pref,CM_MODE_PREF_CDMA_ONLY))
     {
       /*check current srving sys mode*/
       if(ss_ptr->info.srv_status == SYS_SRV_STATUS_SRV)
       {
         switch(ss_ptr->info.sys_mode)
         {
         case SYS_SYS_MODE_CDMA:
           return FALSE;
         case SYS_SYS_MODE_GSM:
         case SYS_SYS_MODE_WCDMA:
         case SYS_SYS_MODE_TDS:
         default:
           return TRUE;
         }
       }
     }

    }
    else if(call_ptr->ss == CM_SS_HYBR_1)
    {
      return FALSE;
    }
  }

  return TRUE;
}

#ifdef FEATURE_CM_LTE_CC
/*===========================================================================
FUNCTION cmcc_call_control_processing_common

DESCRIPTION
  Common call control procedures.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void cmcc_call_control_processing_common(

  cmcc_object_s_type                 *cc_object_ptr,
    /* Pointer to Call Control Object */

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_subscription_source_e_type        cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type        gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_num_s_type                     *orig_num_ptr,
    /* orginating number */

  void                                    *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type   *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  const cm_gw_ps_cc_params_s_type         *gw_ps_cc_params_ptr,
    /* PS call params for call control */

  const cm_lte_ps_cc_params_s_type        *lte_ps_cc_params_ptr
    /* LTE ps call params for call control */

)
{
  cm_cmd_type      *cmd_ptr = (cm_cmd_type*) user_data;

  cmcall_s_type              *call_ptr;


  memset(cc_object_ptr, 0, sizeof(cmcc_object_s_type));
  cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
  cc_object_ptr->call_id = call_id;
  cc_object_ptr->user_data = user_data;
  cc_object_ptr->cc_complete_cb_func = cm_call_control_complete_cb_func;
  cc_object_ptr->status = CM_CC_STATUS_SUCCESS;
  cc_object_ptr->call_type = call_type;
  cc_object_ptr->cdma_subscription_source = cdma_subscription_source;
  cc_object_ptr->gw_subscription_source = gw_subscription_source;

  call_ptr = cmcall_ptr (cc_object_ptr->call_id);

  if(cc_object_ptr->call_type != CM_CALL_TYPE_SUPS)
  {
    if(call_ptr != NULL)
    {
      cc_object_ptr->ss = call_ptr->ss;
    }
  }
  else
  {
    CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SUPS );
    cc_object_ptr->ss = cmph_map_as_id_to_cm_ss(SUPS_CMD_PTR(cmd_ptr)->cmd_info.cmd_subs_id);

    if ( cmph_is_subs_feature_mode_sglte( SUPS_CMD_PTR(cmd_ptr)->cmd_info.cmd_subs_id ) )
    {
      cm_ss_e_type cc_obj_ss = CM_SS_NONE;
      cmcall_sglte_determine_ss_per_call_type(
                                     SUPS_CMD_PTR(cmd_ptr)->cmd_info.cmd_subs_id,
                                     cc_object_ptr->call_type, &cc_obj_ss );

      cc_object_ptr->ss = cc_obj_ss;
    }
  }

  cc_object_ptr->orig_request.ps_cc_sys_mode = SYS_SYS_MODE_NONE;
  cc_object_ptr->orig_request.voip_cc_sys_mode = SYS_SYS_MODE_NONE;

  cc_object_ptr->is_envelope_command_sent = FALSE;
  cc_object_ptr->is_cc_delayed = FALSE;
  memset(&(cc_object_ptr->orig_request.gw_ps_cc_params),
         0,
         sizeof(cc_object_ptr->orig_request.gw_ps_cc_params));

  if (cmcall_is_active_ip_app(SYS_SYS_MODE_WLAN, call_type))
  {
    cc_object_ptr->orig_request.voip_cc_sys_mode = SYS_SYS_MODE_WLAN;
    CM_MSG_HIGH_1("Voip CC sys_mode = %d", cc_object_ptr->orig_request.voip_cc_sys_mode);
  }

  if ((call_type == CM_CALL_TYPE_PS_DATA) &&
      (gw_ps_cc_params_ptr != NULL))
  {
    cc_object_ptr->orig_request.ps_cc_sys_mode = SYS_SYS_MODE_GW;

    *(&(cc_object_ptr->orig_request.gw_ps_cc_params)) = *gw_ps_cc_params_ptr;
  }
  else if (call_type == CM_CALL_TYPE_PS_DATA && lte_ps_cc_params_ptr != NULL)
  {
    cc_object_ptr->orig_request.ps_cc_sys_mode = SYS_SYS_MODE_LTE;
    *(&(cc_object_ptr->orig_request.lte_ps_cc_params)) = *lte_ps_cc_params_ptr;
  }

  #ifdef FEATURE_OTASP
  if(call_type == CM_CALL_TYPE_STD_OTASP ||
     call_type == CM_CALL_TYPE_NON_STD_OTASP)
  {
    CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_CALL );
    cc_object_ptr->orig_request.activate_code =
      CALL_CMD_PTR(cmd_ptr)->info.cdma_info.otasp_act_code;
  }
  #else
  SYS_ARG_NOT_USED(cmd_ptr);
  #endif
  cc_object_ptr->cdma_result_ptr = &cc_object_ptr->orig_request;
  cc_object_ptr->gw_result_ptr   = &cc_object_ptr->orig_request;
  cc_object_ptr->orig_request.call_control_result =
                                      CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
  cc_object_ptr->orig_request.error_cause = CM_CALL_CMD_ERR_CAUSE_NO_ERROR;

  cc_object_ptr->orig_request.alpha.len = 0;
  memset(cc_object_ptr->orig_request.alpha.buf,
         0, sizeof(cc_object_ptr->orig_request.alpha.buf));
  if (orig_num_ptr != NULL)
  {
    cc_object_ptr->orig_request.num = *orig_num_ptr;
  }

  /* To be used for another CC result */
  *(&(cc_object_ptr->cc_result_buf)) = *(&(cc_object_ptr->orig_request));

  cc_object_ptr->cc_result_buf.call_control_result =
                                      CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

}



/*===========================================================================
FUNCTION cmcc_call_control_processing

DESCRIPTION
  Classify number as Emergency/OTASP/regular voice/data/sups calls, and
  perform call control for Voice, data and Sups calls.
  For backward compatibilty current functionality will be retained and
  new design will be effective under this feature -
  FEATURE_MODEM_CALL_CONTROL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcc_call_control_processing (

  cm_call_id_type                       call_id,
    /* call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /* call type */

  const cm_num_s_type                   *orig_num_ptr,
    /* orginating number */

  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  const cm_gw_ps_cc_params_s_type       *gw_ps_cc_params_ptr
    /* PS call params for call control */

)
{

  cmcc_object_s_type *cc_object_ptr = NULL;
  cm_cmd_type      *cmd_ptr = (cm_cmd_type*) user_data;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  sys_modem_as_id_e_type   asubs_id = SYS_MODEM_AS_ID_1;
  #endif
  cm_mmgsdi_session_info_s_type sess_info =
                                          {MMGSDI_MAX_SESSION_TYPE_ENUM, 0 };
  cm_mmgsdi_error_e_type        mmgsdi_result = CM_MMGSDI_NOTFOUND;

  cmcall_s_type                *call_ptr;
  cm_mmgsdi_read_uptime_s_type* mmgsdi_acl_read_req;

  CM_ASSERT(cmd_ptr != NULL);
  CM_MSG_LOW_0("CMCC processing");

  /* Validate incoming parameters */
  if ((orig_num_ptr == NULL) ||
      (cm_call_control_complete_cb_func == NULL)
      || (call_type == CM_CALL_TYPE_PS_DATA && gw_ps_cc_params_ptr == NULL)
     )
  {
    CM_ERR_3 ("Bad parameters for CC %x %x %x",
             orig_num_ptr,
             cm_call_control_complete_cb_func,
             gw_ps_cc_params_ptr);

    return FALSE;
  }

cc_object_ptr = cmcc_object_allocate( call_id );

cmcc_call_control_processing_common(cc_object_ptr,
                         call_id,
                     cdma_subscription_source,
                     gw_subscription_source,
                     call_type,
                     orig_num_ptr,
                     user_data,
                     cm_call_control_complete_cb_func,
                                         gw_ps_cc_params_ptr,
                                         NULL);


#ifdef FEATURE_MMGSDI_SESSION_LIB
  /* Get the SS, based on the subscription-id, on which this call is
  ** originated.
  */
  if (cmph_is_ssim())
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }
  else
  {
    asubs_id = cmph_map_cm_ss_to_as_id(cc_object_ptr->ss);
  }

  if (asubs_id == SYS_MODEM_AS_ID_3)
  {
    sess_info.session_type = MMGSDI_GW_PROV_TER_SESSION;
    cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
  }
  else if (asubs_id == SYS_MODEM_AS_ID_2)
  {
    sess_info.session_type = MMGSDI_GW_PROV_SEC_SESSION;
    cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
  }
  else
  {
    /* Decide whether it is a 1x or GW session */
    if(gw_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE &&
      cmcc_call_control_use_gw_session(call_id))
    {
      mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_gw_slot(
             ((gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:\
             ((gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)?MMGSDI_SLOT_2\
           :MMGSDI_SLOT_1)), &sess_info);
    }
    else if(cdma_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      if((cdma_subscription_source ==CM_CC_SUBSCRIPTION_SOURCE_SIM_1)||
         (cdma_subscription_source==CM_CC_SUBSCRIPTION_SOURCE_SIM_2) ||
         (cdma_subscription_source==CM_CC_SUBSCRIPTION_SOURCE_SIM_3))
      {
         mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_cdma_slot(
           ((cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:\
           ((cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)?MMGSDI_SLOT_2\
           :MMGSDI_SLOT_1)), &sess_info);
      }
      else
      {
        /* Note : this is temporary fix to allow call control on GW SIM card
        ** when cdma subscription is from NV */
        mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot(MMGSDI_SLOT_1,
                                                                &sess_info);
      }
    }

    if(mmgsdi_result != CM_MMGSDI_SUCCESS)
    {
      sess_info.session_type = MMGSDI_GW_PROV_PRI_SESSION;
      (void)cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
      CM_MSG_HIGH_0("mmgsdi_sess_info_ptr NULL. session type set to GW PRI");
    }

    CM_MSG_HIGH_1 ("sess type: %d", sess_info.session_type);
  }

  CM_MSG_HIGH_3("asubs_id=%d, session_id %d, session_type=%d",
                 asubs_id, sess_info.session_id, sess_info.session_type);

#endif /* FEATURE_MMGSDI_SESSION_LIB*/
  /* This memory is freed after invoking CM callback or within this fn
  ** if Envelope command was not sent to card
  */

  cmcc_resolve_call_type(sess_info.session_type,
                         cc_object_ptr);

  CM_MSG_HIGH_1("Resolved call type %d",cc_object_ptr->call_type);

  call_ptr = cmcall_ptr (cc_object_ptr->call_id);

  if(call_ptr != NULL)
  {

    if((call_ptr->call_type == CM_CALL_TYPE_VT) && (cc_object_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
    {
      call_ptr->is_em_vt = TRUE;
      CM_MSG_HIGH_1("Set is_em_vt %d",call_ptr->is_em_vt);
    }
    else
    {
      call_ptr->is_em_vt = FALSE;
    }

    call_ptr->call_type = cc_object_ptr->call_type;

    #ifdef FEATURE_MMODE_DUAL_SIM

    call_ptr->is_emerg_on_other_sub = FALSE;

    if(cc_object_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      call_ptr->is_emerg_on_other_sub =
               cmcc_is_number_emerg_on_other_sub(cc_object_ptr);
    }

    CM_MSG_HIGH_1("Is number emerg on other sub: is_emerg_on_other_sub: %d",call_ptr->is_emerg_on_other_sub);

    #endif
  }

  /* check if call control needs to be performed further
   ** skip call control if
   ** 1. call type is not voice, data, sups
   ** 2. or gw sub source none and cdma sub source nv
   */
  if (!cmcc_is_cc_required(cc_object_ptr,
                           cdma_subscription_source,
                           gw_subscription_source))
  {
    /* Send CC SUCCESS now. No need to do more call control on this */
    cm_call_control_complete_cb_func( call_id,
                                      CM_CC_STATUS_SUCCESS,
                                      cc_object_ptr->call_type,
                                      cc_object_ptr->cdma_result_ptr,
                                      cc_object_ptr->gw_result_ptr,
                                      (void *) user_data);

    cmcc_object_deallocate( cc_object_ptr );
    return TRUE;
  }

  /* populate sim capabilities, required by rest of call control */
  cc_object_ptr->sim_capabilities_valid = FALSE;

  mmgsdi_acl_read_req = cmmmgsdi_allocate_sim_read_req(cc_object_ptr->call_id);
  if(mmgsdi_acl_read_req != NULL)
  {
    mmgsdi_acl_read_req->uptime = time_get_uptime_secs() + CM_MMGSDI_ABORT_TIMER_VALUE_SECS;
    mmgsdi_acl_read_req->cc_object_ptr = (void*)cc_object_ptr;
    mmgsdi_acl_read_req->response_cb_ptr = (mmgsdi_callback_type)cmcc_sim_cap_cb;
  }

  CM_MSG_HIGH_4("call_control L: call_id %d, call_type %d cc_object_ptr %x, cc_object_ptr->user_data %x",
                 cc_object_ptr->call_id, cc_object_ptr->call_type,
  								cc_object_ptr, cc_object_ptr->user_data);
  
  /*lint -save -e534*/
  if(MMGSDI_SUCCESS != mmgsdi_session_get_app_capabilities(
                                  sess_info.session_id,
                            (mmgsdi_callback_type)cmcc_sim_cap_cb,
                                   (uint32)cc_object_ptr->call_id))
  {
    CM_MSG_HIGH_0("mmgsdi_session_get_app_capabilities failed");
    cmmmgsdi_deallocate_sim_read_req(cc_object_ptr->call_id);
    cmcc_object_deallocate( cc_object_ptr );
    return FALSE;
  }
  /*lint -restore */


  SYS_ARG_NOT_USED(cmd_ptr);

  return TRUE;

}/* cmcc_call_control_processing */



#if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_LTE)

/*===========================================================================

FUNCTION NAME cmcc_acl_operation_cb

DESCRIPTION
  Function is called by MMGSDI when finishing processing MMGSDI_USIM_SRV_ACL request.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(765, cmcc_acl_operation_cb)
** Can't be static, unit test uses it
*/
void cmcc_acl_operation_cb(
    mmgsdi_return_enum_type    status,
     /* Status sent by MMGSDI */
    mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
    const mmgsdi_cnf_type      *cnf_ptr
     /* conf pointer returned by MMGSDI */
)
{
  if (cnf_ptr == NULL)
  {
    cm_mmgsdi_acl_availability = CM_MMGSDI_ACL_AVAIL_INVALID;

    CM_MSG_HIGH_0("cnf_ptr is NULL");
  }
  else
  {
    if (cnf == MMGSDI_SRV_AVAILABLE_CNF)
    {
      /* If the status is not MMGSDI_SUCCESS, ACL is not enabled. */
      if (status != MMGSDI_SUCCESS)
      {
        cm_mmgsdi_acl_availability = CM_MMGSDI_ACL_AVAIL_DISABLED;

        CM_MSG_HIGH_1("cmcc_acl_operation_cb(), status = %d", status);
      }
      else
      {
        if(cnf_ptr->srv_available_cnf.srv_available)
        {
          cm_mmgsdi_acl_availability = CM_MMGSDI_ACL_AVAIL_ENABLED;
        }
        else
        {
          cm_mmgsdi_acl_availability = CM_MMGSDI_ACL_AVAIL_DISABLED;
        }
        CM_MSG_HIGH_1("cmcc_acl_operation_cb(), cm_mmgsdi_acl_availability = %d",
                    cm_mmgsdi_acl_availability );
      }

    }
    else
    {
      CM_MSG_HIGH_1("Invalid cnf %d in cmcc_acl_operation_cb()",
                  cnf);
    }

  }

  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}



/*===========================================================================

FUNCTION NAME cmcc_est_acl_capability_cb

DESCRIPTION
  Function is called by MMGSDI when finishing processing MMGSDI_USIM_SRV_ACL request.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(765, cmcc_est_acl_capability_cb)
** Can't be static, unit test uses it
*/
void cmcc_est_acl_capability_cb(
    mmgsdi_return_enum_type    status,
     /* Status sent by MMGSDI */
    mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
    const mmgsdi_cnf_type      *cnf_ptr
     /* conf pointer returned by MMGSDI */
)
{
  if ((cnf_ptr == NULL)||(status != MMGSDI_SUCCESS))
  {
    cm_acl_status = FALSE;
    CM_MSG_HIGH_1("cmcc_est_acl_capability_cb(), status = %d", status);

  }
  else
  {
    if (cnf == MMGSDI_SESSION_GET_APP_CAPABILITIES_CNF)
    {

      cmcc_object_s_type* cc_object_ptr = 
          (cmcc_object_s_type*)cnf_ptr->session_app_capabilities_cnf.response_header.client_data;
       
      CM_MSG_HIGH_1("cmcc_est_acl_capability_cb(), acl_enabled = %d",
                  cnf_ptr->session_app_capabilities_cnf.acl_enabled);

      if (cnf_ptr->session_app_capabilities_cnf.acl_enabled)
      {
        cm_acl_status = TRUE;
      }
      else
      {
        cm_acl_status = FALSE;
      }

      cc_object_ptr->sim_capabilities = cnf_ptr->session_app_capabilities_cnf;
      cc_object_ptr->sim_capabilities_valid = TRUE;

    }
    else
    {
      CM_MSG_HIGH_1("Invalid cnf %d in cmcc_est_acl_capability_cb()",
                  cnf);
    }

  }

  (void) rex_set_sigs( get_cm_tcb(), CM_MMGSDI_REPORT_SIG );


}

#endif /* #if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_LTE) */


#ifdef FEATURE_CM_LTE
/*===========================================================================
FUNCTION cmcc_call_control_processing_lte

DESCRIPTION
  Perform call control for PS data call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcc_call_control_processing_lte (

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  const cm_lte_ps_cc_params_s_type       *lte_ps_cc_params_ptr,
    /* PS call params for call control */

  cm_ss_e_type                            cm_ss,
    /* On which ss the call is made */

  sys_modem_as_id_e_type                  asubs_id
    /* On which asubs_id the call is made */
)
{
  cmcc_object_s_type *cc_object_ptr = NULL;
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type          cm_mmgsdi_session_id;
  cm_mmgsdi_error_e_type           cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_return_enum_type          mmgsdi_status = MMGSDI_NOT_FOUND;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Validate incoming parameters */
  if ((cm_call_control_complete_cb_func == NULL) ||
      (lte_ps_cc_params_ptr == NULL))
  {
    CM_ERR_2 ("Bad parameters for CC %x %x",
            cm_call_control_complete_cb_func,
            lte_ps_cc_params_ptr);

    return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This memory is freed after invoking CM callback or within this fn
  ** if Envelope command was not sent to card
  */
  cc_object_ptr = cmcc_object_allocate( CM_CALL_ID_INVALID );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Do Call Control common procedure */

  cmcc_call_control_processing_common(cc_object_ptr,
                                      CM_CALL_ID_INVALID,
                                      cdma_subscription_source,
                                      gw_subscription_source,
                                      CM_CALL_TYPE_PS_DATA,
                                      NULL,
                                      (void *)cc_object_ptr,
                                      cm_call_control_complete_cb_func,
                                      NULL,
                                      lte_ps_cc_params_ptr);
  cc_object_ptr->ss = cm_ss;

  CM_MSG_HIGH_3("lte_cc: ps_cc_sys_mode=%d, ss=%d, lte_ps_cc_param as_id=%d",
                cc_object_ptr->orig_request.ps_cc_sys_mode,
                cc_object_ptr->ss,
                cc_object_ptr->orig_request.lte_ps_cc_params.as_id);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(
     ((asubs_id == SYS_MODEM_AS_ID_2)? MMGSDI_GW_PROV_SEC_SESSION: MMGSDI_GW_PROV_PRI_SESSION),
                     &cm_mmgsdi_session_id);

  CM_MSG_HIGH_1("cm_mmgsdi_status=%d", cm_mmgsdi_status);

  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status){
     CM_ERR_1("Unable to get session id; err: %d",
        cm_mmgsdi_status);
  }


  mmgsdi_status =  mmgsdi_session_is_service_available (
                   cm_mmgsdi_session_id,
                   MMGSDI_USIM_SRV_EPS_PDN_CC,
                   cmcc_service_available_cb,
                   (uint32)cc_object_ptr);

  CM_MSG_HIGH_1("MMGSDI EPS mmgsdi_status=%d",
                   mmgsdi_status);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    CM_MSG_HIGH_0("mmgsdi_session_is_service_available failed");
    cmcc_object_deallocate( cc_object_ptr );
    return (FALSE);
  }
  else
  {
    if (cm_wait_for_mmgsdi() == FALSE)
    {
      return FALSE;
    }


    if (cm_mmgsdi_srv_available_cnf_ptr == NULL)
    {
      CM_MSG_HIGH_0 ("srv_available_cnf_ptr is null, returning FALSE");
      cmcc_object_deallocate( cc_object_ptr );
      return (FALSE);
    }

    cc_object_ptr = (cmcc_object_s_type*)\
      (cm_mmgsdi_srv_available_cnf_ptr->response_header.client_data);

    cc_object_ptr->srv_available_valid = TRUE;
    cc_object_ptr->srv_available = cm_mmgsdi_srv_available_cnf_ptr->srv_available;

    cm_mem_free(cm_mmgsdi_srv_available_cnf_ptr);
    cm_mmgsdi_srv_available_cnf_ptr = NULL;

    CM_MSG_HIGH_2 ("MMGSDI EPS srv_available=%d, cm_mmgsdi_acl_availability=%d",
                      cc_object_ptr->srv_available,
                      cm_mmgsdi_acl_availability);

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check ACL availability.
    */
    if(cm_mmgsdi_acl_availability != CM_MMGSDI_ACL_AVAIL_ENABLED)
    {
      /* Check whether ACL is enabled */
      mmgsdi_status =  mmgsdi_session_is_service_available (
                       cm_mmgsdi_session_id,
                       MMGSDI_USIM_SRV_ACL,
                       cmcc_acl_operation_cb,
                       (uint32)cc_object_ptr);

      CM_MSG_HIGH_1("is_service_available, acl oper, mmgsdi_status=%d", mmgsdi_status);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        CM_MSG_HIGH_1("cmcc_call_control_processing_lte(), status = %d",
                     mmgsdi_status);
        cmcc_object_deallocate( cc_object_ptr );
        return (FALSE);
      }
      else
      {
        if (cm_wait_for_mmgsdi() == FALSE)
        {
          return FALSE;
        }


      }
    }

    /* If UST ACL availability is TRUE, check EST ACL availability */
      /* Check whether EST ACL is enabled */
      mmgsdi_status =  mmgsdi_session_get_app_capabilities (
                       cm_mmgsdi_session_id,
                       cmcc_est_acl_capability_cb,
                       (uint32)cc_object_ptr);

      CM_MSG_HIGH_1("is_service_available, acl cap, mmgsdi_status=%d", mmgsdi_status);

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        CM_MSG_HIGH_1("cmcc_call_control_processing_lte(), status = %d",
                    mmgsdi_status);
        cmcc_object_deallocate( cc_object_ptr );
        return (FALSE);
      }
      else
      {
        if (cm_wait_for_mmgsdi() == FALSE)
        {
          return FALSE;
        }

      }

    CM_MSG_HIGH_1("lte_cc: ps_cc_sys_mode=%d",
                   cc_object_ptr->orig_request.ps_cc_sys_mode);

    cmcc_call_ctrl_after_sim_capabilities(cc_object_ptr,
                                          CM_ORIG_CLIENT_CATEG_NONE);

    return (TRUE);

  }

  #else

  CM_MSG_HIGH_0("FEATURE_MMGSDI_SESSION_LIB is disabled");
  cmcc_object_deallocate( cc_object_ptr );

  return (FALSE);

  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}
#endif


#else
/*===========================================================================
FUNCTION cmcc_call_control_processing

DESCRIPTION
  Classify number as Emergency/OTASP/regular voice/data/sups calls, and
  perform call control for Voice, data and Sups calls.
  For backward compatibilty current functionality will be retained and
  new design will be effective under this feature -
  FEATURE_MODEM_CALL_CONTROL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcc_call_control_processing (

  cm_call_id_type                       call_id,
    /* call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /* call type */

  const cm_num_s_type                   *orig_num_ptr,
    /* orginating number */

  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  const cm_gw_ps_cc_params_s_type       *gw_ps_cc_params_ptr
    /* PS call params for call control */

)
{
/*lint -esym(529, cmd_ptr)*/
  cmcc_object_s_type *cc_object_ptr = NULL;
  cm_cmd_type      *cmd_ptr = (cm_cmd_type*) user_data;
  cm_ss_e_type             ss = CM_SS_MAIN;

  cm_mmgsdi_session_info_s_type sess_info =
                                        {MMGSDI_MAX_SESSION_TYPE_ENUM, 0 };
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  cm_mmgsdi_error_e_type       mmgsdi_result = CM_MMGSDI_NOTFOUND;
  #endif

  cmcall_s_type                *call_ptr;
  cm_mmgsdi_read_uptime_s_type* mmgsdi_acl_read_req;

  CM_ASSERT(cmd_ptr != NULL);

  /* Validate incoming parameters */
  if ((orig_num_ptr == NULL) ||
      (cm_call_control_complete_cb_func == NULL)
      || (call_type == CM_CALL_TYPE_PS_DATA && gw_ps_cc_params_ptr == NULL)
     )
  {
    CM_ERR_3 ("Bad parameters for CC %x %x %x",
             orig_num_ptr,
             cm_call_control_complete_cb_func,
             gw_ps_cc_params_ptr);

    return FALSE;
  }

  /* This memory is freed after invoking CM callback or within this fn
  ** if Envelope command was not sent to card
  */
  cc_object_ptr = cmcc_object_allocate( call_id );

  cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_NOT_REQUIRED;
  cc_object_ptr->call_id = call_id;
  call_ptr = cmcall_ptr (cc_object_ptr->call_id);
  cc_object_ptr->user_data = user_data;
  cc_object_ptr->cc_complete_cb_func = cm_call_control_complete_cb_func;
  cc_object_ptr->status = CM_CC_STATUS_SUCCESS;
  cc_object_ptr->call_type = call_type;
  cc_object_ptr->cdma_subscription_source = cdma_subscription_source;
  cc_object_ptr->gw_subscription_source = gw_subscription_source;

  cc_object_ptr->is_envelope_command_sent = FALSE;
  cc_object_ptr->is_cc_delayed = FALSE;

  cc_object_ptr->orig_request.ps_cc_sys_mode = SYS_SYS_MODE_NONE;
  memset(&(cc_object_ptr->orig_request.gw_ps_cc_params),
         0,
         sizeof(cc_object_ptr->orig_request.gw_ps_cc_params));

  if (call_type == CM_CALL_TYPE_PS_DATA)
  {
    cc_object_ptr->orig_request.ps_cc_sys_mode = SYS_SYS_MODE_GW;

    *(&(cc_object_ptr->orig_request.gw_ps_cc_params)) = *gw_ps_cc_params_ptr;
  }

  #ifdef FEATURE_OTASP
  if(call_type == CM_CALL_TYPE_STD_OTASP ||
     call_type == CM_CALL_TYPE_NON_STD_OTASP)
  {
    cc_object_ptr->orig_request.activate_code =
                           CALL_CMD_PTR(cmd_ptr)->info.cdma_info.otasp_act_code;
  }
  #endif
  cc_object_ptr->cdma_result_ptr = &cc_object_ptr->orig_request;
  cc_object_ptr->gw_result_ptr   = &cc_object_ptr->orig_request;
  cc_object_ptr->orig_request.call_control_result =
                                     CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
  cc_object_ptr->orig_request.error_cause = CM_CALL_CMD_ERR_CAUSE_NO_ERROR;

  cc_object_ptr->orig_request.alpha.len = 0;
  memset(cc_object_ptr->orig_request.alpha.buf,
      0, sizeof(cc_object_ptr->orig_request.alpha.buf));
  cc_object_ptr->orig_request.num = *orig_num_ptr;

  /* To be used for another CC result */
  *(&(cc_object_ptr->cc_result_buf)) = *(&(cc_object_ptr->orig_request));

  cc_object_ptr->cc_result_buf.call_control_result =
                                       CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

  if(cc_object_ptr->call_type != CM_CALL_TYPE_SUPS)
  {
    if(call_ptr != NULL)
    {
      cc_object_ptr->ss = call_ptr->ss;
    }
  }
  #ifdef CM_GW_SUPPORTED
  else
  {
    CM_ASSERT( CMD_TYPE(cmd_ptr) == CM_CMD_TYPE_SUPS );
    cc_object_ptr->ss = cmph_map_as_id_to_cm_ss( SUPS_CMD_PTR(cmd_ptr)->cmd_info.cmd_subs_id );
  }
  #endif
#ifdef FEATURE_MMGSDI_SESSION_LIB

  if (cmph_is_msim())
  {
    /* Get the SS, based on the subscription-id, on which this call is
    ** originated.
    */
     ss = cc_object_ptr->ss;
  }

  if (ss == CM_SS_HYBR_2)
  {
    sess_info.session_type = MMGSDI_GW_PROV_SEC_SESSION;
    (void)cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
  }
  else
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if (ss == CM_SS_HYBR_3)
  {
    cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(SUPS_CMD_PTR(cmd_ptr)->cmd_info.cmd_subs_id, &sess_info.session_type);
    (void)cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
  }
  else if (ss == CM_SS_HYBR_2 && cmph_is_sxlte())
  {
    sess_info.session_type = MMGSDI_GW_PROV_SEC_SESSION;
    (void)cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
  }
  else
  #endif
  {

    if(gw_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE &&
      cmcc_call_control_use_gw_session(call_id))
    {
      mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_gw_slot(
            ((gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:\
            ((gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)?MMGSDI_SLOT_2\
            :MMGSDI_SLOT_1)),
            &sess_info);
    }
    else if(cdma_subscription_source != CM_CC_SUBSCRIPTION_SOURCE_NONE)
    {
      if((cdma_subscription_source ==CM_CC_SUBSCRIPTION_SOURCE_SIM_1)||
         (cdma_subscription_source==CM_CC_SUBSCRIPTION_SOURCE_SIM_2) ||
         (cdma_subscription_source==CM_CC_SUBSCRIPTION_SOURCE_SIM_3))
      {
        mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_cdma_slot(
          ((cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:\
           ((cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)?MMGSDI_SLOT_2\
           :MMGSDI_SLOT_1)), &sess_info);
      }
      else
      {
        /* Note : this is temporary fix to allow call control on GW SIM card
             ** when cdma subscription is from NV */
        mmgsdi_result = cm_mmgsdi_ssn_tbl_get_ssn_info_for_slot(MMGSDI_SLOT_1,
                                                                  &sess_info);
      }
    }
    if(mmgsdi_result != CM_MMGSDI_SUCCESS)
    {
      sess_info.session_type = MMGSDI_GW_PROV_PRI_SESSION;
      (void)cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type, &sess_info.session_id);
      CM_MSG_HIGH_0("mmgsdi_sess_info_ptr NULL. session type set to GW PRI");
    }

    CM_MSG_HIGH_1 ("sess type: %d", sess_info.session_type);
  }
#endif /* FEATURE_MMGSDI_SESSION_LIB */

  cmcc_resolve_call_type(sess_info.session_type,
                         cc_object_ptr);

  CM_MSG_HIGH_1("Resolved call type %d",cc_object_ptr->call_type);

  if(call_ptr != NULL)
  {
    if((call_ptr->call_type == CM_CALL_TYPE_VT) && (cc_object_ptr->call_type == CM_CALL_TYPE_EMERGENCY))
    {
      call_ptr->is_em_vt = TRUE;
      CM_MSG_HIGH_1("Set is_em_vt %d",call_ptr->is_em_vt);
    }
    else
    {
      call_ptr->is_em_vt = FALSE;
    }
    call_ptr->call_type = cc_object_ptr->call_type;
  }

  #ifdef FEATURE_MMODE_DUAL_SIM
  if(call_ptr != NULL)
  {
    call_ptr->is_emerg_on_other_sub = FALSE;

    if(cc_object_ptr->call_type == CM_CALL_TYPE_EMERGENCY)
    {
      call_ptr->is_emerg_on_other_sub =
               cmcc_is_number_emerg_on_other_sub(cc_object_ptr);
    }

    CM_MSG_HIGH_1("Is number emerg on other sub: is_emerg_on_other_sub: %d",call_ptr->is_emerg_on_other_sub);
  }
  #endif
  /* check if call control needs to be performed further
   ** skip call control if
   ** 1. call type is not voice, data, sups
   ** 2. or gw sub source none and cdma sub source nv
   */
  if (!cmcc_is_cc_required(cc_object_ptr,
                           cdma_subscription_source,
                           gw_subscription_source))
  {
    /* Send CC SUCCESS now. No need to do more call control on this */
    cm_call_control_complete_cb_func( call_id,
                                      CM_CC_STATUS_SUCCESS,
                                      cc_object_ptr->call_type,
                                      cc_object_ptr->cdma_result_ptr,
                                      cc_object_ptr->gw_result_ptr,
                                      (void *) user_data);

    cmcc_object_deallocate( cc_object_ptr );
    return TRUE;
  }

  /* populate sim capabilities, required by rest of call control */
  #ifdef FEATURE_MMGSDI_SESSION_LIB

  CM_MSG_LOW_0("Get Sim capabilities");
  cc_object_ptr->sim_capabilities_valid = FALSE;
  if((ss != CM_SS_HYBR_2)
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
        && (ss != CM_SS_HYBR_3)
        #endif
        )
  {
    if(mmgsdi_result != CM_MMGSDI_SUCCESS)
    {
      sess_info.session_type = MMGSDI_GW_PROV_PRI_SESSION;
      (void)cm_mmgsdi_ssn_tbl_get_id_for_type(sess_info.session_type,
                                              &sess_info.session_id);
      CM_MSG_HIGH_0("mmgsdi_sess_info_ptr NULL. session type set to GW PRI");
    }

    CM_MSG_HIGH_1 ("sess type: %d", sess_info.session_type);
  }

  mmgsdi_acl_read_req = cmmmgsdi_allocate_sim_read_req(cc_object_ptr->call_id);
  if(mmgsdi_acl_read_req != NULL)
  {
    mmgsdi_acl_read_req->uptime = time_get_uptime_secs() + CM_MMGSDI_ABORT_TIMER_VALUE_SECS;
    mmgsdi_acl_read_req->cc_object_ptr = (void*)cc_object_ptr;
    mmgsdi_acl_read_req->response_cb_ptr = (mmgsdi_callback_type)cmcc_sim_cap_cb;
  }

  /*lint -save -e534*/
  if(MMGSDI_SUCCESS != mmgsdi_session_get_app_capabilities( sess_info.session_id,
                            (mmgsdi_callback_type)cmcc_sim_cap_cb,
                            (uint32)cc_object_ptr->call_id))
  {
    CM_MSG_HIGH_0("mmgsdi_session_get_app_capabilities failed");
    cmmmgsdi_deallocate_sim_read_req(cc_object_ptr->call_id);
    cmcc_object_deallocate( cc_object_ptr );
    return FALSE;
  }

  /*lint -restore */

  #else

  /* no MMGSDI, so just carry on */
  cmcc_call_ctrl_after_sim_capabilities(cc_object_ptr,
                                        CALL_CMD_PTR(cmd_ptr)->client_categ);

  #endif

  return TRUE;

}/* cmcc_call_control_processing */
#endif /* FEATURE_CM_LTE_CC */



/*===========================================================================
FUNCTION cmcc_gstk_rsp_cb

DESCRIPTION
  When the envelope command for call control is completed, this function
  is called by GSDI/GSTK to communicate the results.   The user_data should
  be the cmcc_object_s_type structure we were working with.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void cmcc_gstk_rsp_cb
(
  gstk_cmd_from_card_type *req_rsp
   /* Pointer to req response */
)
/*lint -esym(818, req_rsp)*/
{
  cmcc_object_s_type *cc_object_ptr =
                       (cmcc_object_s_type *) req_rsp->hdr_cmd.user_data;
  cm_orig_call_control_s_type *sim_cc_result_ptr = NULL;


  CM_MSG_HIGH_0("GSTK SIM CC callback");
  if (cc_object_ptr->sim_cc_required == CMCC_CARD_STATUS_CC_REQUIRED)
  {
    cc_object_ptr->sim_cc_required = CMCC_CARD_STATUS_CC_COMPLETED;
    sim_cc_result_ptr = &cc_object_ptr->orig_request;
  }
  else
  {
    CM_ERR_0("Unexpected callback.  No request pending.");
  }

  /* If the data looks good, process the response. */
  if (sim_cc_result_ptr)
  {
    /* Save the results of the CC command in the structure. */
    if (req_rsp->cmd.cc_rsp.general_resp == GSTK_ENVELOPE_CMD_SUCCESS)
    {

      /* SIM call control done. Set the flag to TRUE here */
      sim_cc_result_ptr->is_sim_cc_performed = TRUE;

      /* Call control can say the call is allowed, but with a modification,
      ** the call is allowed as is, or, the call is not allowed.
      */
      switch(req_rsp->cmd.cc_rsp.call_ctrl_result)
      {
        case GSTK_ALLOWED_BUT_MOD:
        {
          /* They have given us a modification to the original command. */
          gstk_cc_req_action_type req_action;
          cm_orig_client_categ_e_type    client_categ = CM_ORIG_CLIENT_CATEG_NONE;
          gstk_call_enum_type old_call_type = GSTK_NONE;
          req_action = req_rsp->cmd.cc_rsp.cc_req_action;
          CM_MSG_HIGH_1("Call modified to call type: %d",
                                                 req_action.call_type);

          switch ( req_action.call_type )
          {
            case GSTK_VOICE:
              /* Apply the origination if its still an origination. */
              sim_cc_result_ptr->call_control_result =
                                CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE;
              sim_cc_result_ptr->num.len =
                    req_action.gstk_address_string.voice_info.address.length;
              (void) gstk_address_to_ascii
                  (&req_action.gstk_address_string.voice_info.address,
                    sim_cc_result_ptr->num.buf, CM_MAX_NUMBER_CHARS);
              sim_cc_result_ptr->num.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;

              switch (req_action.gstk_address_string.voice_info.address.TON)
              {
                case GSTK_INTERNATIONAL_NUM:
                  sim_cc_result_ptr->num.number_type =
                         (uint8) CM_NUM_TYPE_INTERNATIONAL;
                  break;
                case GSTK_NATIONAL_NUM:
                  sim_cc_result_ptr->num.number_type =
                         (uint8) CM_NUM_TYPE_NATIONAL;
                  break;
                case GSTK_UNKNOWN_TON:
                  sim_cc_result_ptr->num.number_type =
                         (uint8) CM_NUM_TYPE_UNKNOWN;
                  break;
                case GSTK_NETWORK_SPECIFIC_NUM:
                  sim_cc_result_ptr->num.number_type =
                        (uint8) CM_NUM_TYPE_NETWORK_SPECIFIC;
                  break;
                default:
                  sim_cc_result_ptr->num.number_type =
                        (uint8) CM_NUM_TYPE_UNKNOWN;
                  break;
              }
              switch (req_action.gstk_address_string.voice_info.address.NPI)
              {
                case GSTK_UNKNOWN_NPI:
                  /* Unknown */
                  sim_cc_result_ptr->num.number_plan =
                                      (uint8) CM_NUM_PLAN_UNKNOWN;
                  break;
                case GSTK_ISDN_TELEPHONY_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                      (uint8) CM_NUM_PLAN_ISDN;
                  /* ISDN/telephony numbering plan */
                  break;
                case GSTK_DATA_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                      (uint8) CM_NUM_PLAN_DATA;
                  /* Data numbering plan */
                  break;
                case GSTK_TELEX_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                      (uint8) CM_NUM_PLAN_TELEX;
                  /* Telex numbering plan */
                  break;
                case GSTK_PRIVATE_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                      (uint8) CM_NUM_PLAN_PRIVATE;
                  /* Private numbering plan */
                  break;
                case GSTK_EXT_RESERVED_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                     (uint8) CM_NUM_PLAN_RESERVED_EXTENSION;
                  break;
                default:
                  sim_cc_result_ptr->num.number_plan =
                                     (uint8) CM_NUM_PLAN_UNKNOWN;
                  break;
              }
              sim_cc_result_ptr->num.pi = 0;
              sim_cc_result_ptr->num.si = 0;

              if (req_action.gstk_address_string.voice_info.ccp1.length
                    > CM_MAX_CCP_LEN)
              {
                sim_cc_result_ptr->ccp1.length = memscpy( sim_cc_result_ptr->ccp1.ccp,
                                                          sizeof(sim_cc_result_ptr->ccp1.ccp),
                                                          req_action.gstk_address_string.voice_info.ccp1.ccp,
                                                          sizeof(req_action.gstk_address_string.voice_info.ccp1.ccp) );
              }
              else
              {
                sim_cc_result_ptr->ccp1.length = memscpy( sim_cc_result_ptr->ccp1.ccp,
                                                          sizeof(sim_cc_result_ptr->ccp1.ccp),
                                                          req_action.gstk_address_string.voice_info.ccp1.ccp,
                                                          req_action.gstk_address_string.voice_info.ccp1.length );
              }

              if (req_action.gstk_address_string.voice_info.ccp2.length
                    > CM_MAX_CCP_LEN)
              {
                sim_cc_result_ptr->ccp2.length = memscpy( sim_cc_result_ptr->ccp2.ccp,
                                                          sizeof(sim_cc_result_ptr->ccp2.ccp),
                                                          req_action.gstk_address_string.voice_info.ccp2.ccp,
                                                          sizeof(req_action.gstk_address_string.voice_info.ccp2.ccp) );
              }
              else
              {
                sim_cc_result_ptr->ccp2.length = memscpy( sim_cc_result_ptr->ccp2.ccp,
                                                          sizeof(sim_cc_result_ptr->ccp2.ccp),
                                                          req_action.gstk_address_string.voice_info.ccp2.ccp,
                                                          req_action.gstk_address_string.voice_info.ccp2.length );
              }


              if(req_action.gstk_address_string.voice_info.subaddress.length > 0)
              {
                cmcall_unpack_subaddress(
                    &(sim_cc_result_ptr->num.subaddr),
                    &(req_action.gstk_address_string.voice_info.subaddress));
              }
              else
              {
                sim_cc_result_ptr->num.subaddr.num_char = 0;
              }

              sim_cc_result_ptr->alpha.dcs = req_action.alpha.dcs;

              sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                      sizeof(sim_cc_result_ptr->alpha.buf),
                                                      req_action.alpha.text,
                                                      req_action.alpha.length );

              /* per spec 3GPP TS 22.101
              ** 112 is the only emergency number for SETUP CALLs
              */
              if ((sim_cc_result_ptr->num.len == 3)       &&
                  (sim_cc_result_ptr->num.buf[0] == '1')  &&
                  (sim_cc_result_ptr->num.buf[1] == '1')  &&
                  (sim_cc_result_ptr->num.buf[2] == '2') )
              {
                cc_object_ptr->call_type = CM_CALL_TYPE_EMERGENCY;
                sim_cc_result_ptr->num.emerg_srv_categ = 0;
              }
              break;

            case GSTK_SS:
              sim_cc_result_ptr->call_control_result =
                                     CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS;
              sim_cc_result_ptr->num.len =
                             req_action.gstk_address_string.ss_string.length;

              (void) gstk_address_to_ascii(
                                  &req_action.gstk_address_string.ss_string,
                                   sim_cc_result_ptr->num.buf,
                                   CM_MAX_NUMBER_CHARS);
              sim_cc_result_ptr->num.digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
              switch ( req_action.gstk_address_string.ss_string.TON )
              {
                case GSTK_INTERNATIONAL_NUM:
                  sim_cc_result_ptr->num.number_type =
                                       (uint8) CM_NUM_TYPE_INTERNATIONAL;
                  break;
                case GSTK_NATIONAL_NUM:
                  sim_cc_result_ptr->num.number_type =
                                       (uint8) CM_NUM_TYPE_NATIONAL;
                  break;
                case GSTK_UNKNOWN_TON:
                  sim_cc_result_ptr->num.number_type =
                                       (uint8) CM_NUM_TYPE_UNKNOWN;
                  break;
                case GSTK_NETWORK_SPECIFIC_NUM:
                  sim_cc_result_ptr->num.number_type =
                                       (uint8) CM_NUM_TYPE_NETWORK_SPECIFIC;
                  break;
                default:
                  sim_cc_result_ptr->num.number_type =
                                                 (uint8) CM_NUM_TYPE_UNKNOWN;
                  break;
              }
              switch (req_action.gstk_address_string.ss_string.NPI )
              {
                case GSTK_UNKNOWN_NPI:
                  /* Unknown */
                  sim_cc_result_ptr->num.number_plan =
                                  (uint8) CM_NUM_PLAN_UNKNOWN;
                  break;
                case GSTK_ISDN_TELEPHONY_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                 (uint8) CM_NUM_PLAN_ISDN;
                  /* ISDN/telephony numbering plan */
                  break;
                case GSTK_DATA_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                (uint8) CM_NUM_PLAN_DATA;
                  /* Data numbering plan */
                  break;
                case GSTK_TELEX_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                (uint8) CM_NUM_PLAN_TELEX;
                  /* Telex numbering plan */
                  break;
                case GSTK_PRIVATE_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                (uint8) CM_NUM_PLAN_PRIVATE;
                  /* Private numbering plan */
                  break;
                case GSTK_EXT_RESERVED_NPI:
                  sim_cc_result_ptr->num.number_plan =
                                (uint8) CM_NUM_PLAN_RESERVED_EXTENSION;
                  break;
                default:
                  sim_cc_result_ptr->num.number_plan =
                                (uint8) CM_NUM_PLAN_UNKNOWN;
                  break;
              }
              sim_cc_result_ptr->num.pi = 0;
              sim_cc_result_ptr->num.si = 0;

              sim_cc_result_ptr->alpha.dcs = req_action.alpha.dcs;

              sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                      sizeof(sim_cc_result_ptr->alpha.buf),
                                                      req_action.alpha.text,
                                                      req_action.alpha.length );
              break;

            case GSTK_USSD:
              {
                uint8 length =
                           req_action.gstk_address_string.ussd_string.length;

                if (length > sizeof(sim_cc_result_ptr->num.buf))
                {
                  length = sizeof(sim_cc_result_ptr->num.buf);
                }

                sim_cc_result_ptr->call_control_result =
                             CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD;
                sim_cc_result_ptr->sups_cmd = CM_SUPS_CMD_PROCESS_USS;
                #ifdef CM_GW_SUPPORTED
                sim_cc_result_ptr->sups_params.uss_params.uss_data.present =
                                                                       TRUE;

                sim_cc_result_ptr->sups_params.uss_params.uss_data.size =
                               memscpy( sim_cc_result_ptr->sups_params.uss_params.uss_data.ussData,
                                        sizeof(sim_cc_result_ptr->sups_params.uss_params.uss_data.ussData),
                                        req_action.gstk_address_string.ussd_string.text,
                                        length);

                /* sim_cc_result_ptr->sups_params.uss_params.uss_data.uss_data_coding_scheme =
                              req_action.gstk_address_string.ussd_string.dcs;
                Copying the orig_dcs_from_sim to uss_data_coding_scheme since GSTK wants CM to use this value only
                                 CM is howver using the uss_data_coding_scheme value while forwarding it to NAS */
                 sim_cc_result_ptr->sups_params.uss_params.uss_data.uss_data_coding_scheme =
                  req_action.gstk_address_string.ussd_string.orig_dcs_from_sim;
                sim_cc_result_ptr->sups_params.uss_params.uss_data.orig_dcs_from_sim=
                              req_action.gstk_address_string.ussd_string.orig_dcs_from_sim;
                /* ss_ref is reference for client to know a CNF is received
                ** for this request. This will be passed from network back to
                ** client in Confirmation message. This hard-coded to 1 now
                */
                #endif
                sim_cc_result_ptr->sups_params.uss_params.ss_ref = 1;
                sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                        sizeof(sim_cc_result_ptr->alpha.buf),
                                                        req_action.alpha.text,
                                                        req_action.alpha.length );
                sim_cc_result_ptr->alpha.dcs = req_action.alpha.dcs;
              }
              break;

            #if defined (FEATURE_GPRS_CALLCONTROL)
            case GSTK_PDP_CONTEXT_ACT:
            {
              sim_cc_result_ptr->call_control_result =
                                           CM_CC_RESULT_ALLOWED_BUT_MODIFIED;

              sim_cc_result_ptr->gw_ps_cc_params.length =
                               memscpy( sim_cc_result_ptr->gw_ps_cc_params.pdp_act_mesg,
                                        sizeof(sim_cc_result_ptr->gw_ps_cc_params.pdp_act_mesg),
                                        req_action.gstk_address_string.pdp_context_param.pdp_act_param,
                                        (word)req_action.gstk_address_string.pdp_context_param.length );

              sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                      sizeof(sim_cc_result_ptr->alpha.buf),
                                                      req_action.alpha.text,
                                                      req_action.alpha.length );

              }
              break;
              #endif

              #if defined (FEATURE_CM_LTE)
              case GSTK_EPS_PDN_CONN_ACT:
              {
                sim_cc_result_ptr->call_control_result =
                                           CM_CC_RESULT_ALLOWED_BUT_MODIFIED;

                sim_cc_result_ptr->lte_ps_cc_params.length =
                                               memscpy( sim_cc_result_ptr->lte_ps_cc_params.pkd_msg,
                                                        sizeof(sim_cc_result_ptr->lte_ps_cc_params.pkd_msg),
                                                        req_action.gstk_address_string.eps_conn_act.eps_act_param,
                                                        (word)req_action.gstk_address_string.eps_conn_act.length );

                sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                        sizeof(sim_cc_result_ptr->alpha.buf),
                                                        req_action.alpha.text,
                                                        req_action.alpha.length );

                break;
              }
              #endif

            default:
              CM_ERR_1("Unexpected call type returned by GSTK %d",
                      req_action.call_type);
              break;
          }

            /*
                      Find the call category using the old call type
                    */
            /*
                        Need to check if the call is modified from 1 call type to another call type
                        If yes, only then GSTK API needs to be called
                   */
            #if defined CM_GW_SUPPORTED
            if(cc_object_ptr->call_type == CM_CALL_TYPE_SUPS)
            {
              client_categ = ((cm_sups_cmd_s_type*)(cc_object_ptr->user_data))->client_categ;
              if((((cm_sups_cmd_s_type*)(cc_object_ptr->user_data))->cmd) == CM_SUPS_CMD_PROCESS_USS)
              {
                old_call_type = GSTK_USSD;
              }
              else
              {
                old_call_type = GSTK_SS;
              }
            }
            else
            #endif
            if(cc_object_ptr->call_type == CM_CALL_TYPE_VOICE)
            {
              old_call_type = GSTK_VOICE;
              client_categ = ((cm_call_cmd_s_type*)(cc_object_ptr->user_data))->client_categ;
            }

            CM_MSG_HIGH_3("SIM initiated, client_categ=%d,old_call_type=%d, new_call_type=%d",
                          client_categ, old_call_type, req_action.call_type);
            /* if the call is initiated from SIM and
                        if old call type and the new call is different , means call is modified and GSTK API needs to be invoked
                   */
            if(client_categ == CM_ORIG_CLIENT_CATEG_CARD && \
                   old_call_type != req_action.call_type)
            {
                gstk_status_enum_type        gstk_result;
                int dummy = 1; /* non null dummy value: GSTK doesnt want NULL to be passed */
                gstk_result =  gstk_io_ctrl(GSTK_IO_SIM_INITIATED_MODIFIED_CC_RESP, (void*)&req_rsp->cmd.cc_rsp ,(void *)&dummy);
                CM_MSG_HIGH_1("Called gstk_io_ctrl Api: gstk_result =%d",gstk_result);
            }

          break;
        }

        case GSTK_NOT_ALLOWED:
          CM_MSG_HIGH_0("Call not allowed");
          sim_cc_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
          sim_cc_result_ptr->error_cause =
                                          CM_CALL_CMD_ERR_CAUSE_CARD_CC_FAIL;

          sim_cc_result_ptr->alpha.dcs = req_rsp->cmd.cc_rsp.cc_req_action.alpha.dcs;

          sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                  sizeof(sim_cc_result_ptr->alpha.buf),
                                                  req_rsp->cmd.cc_rsp.cc_req_action.alpha.text,
                                                  req_rsp->cmd.cc_rsp.cc_req_action.alpha.length );

          break;

        case GSTK_ALLOWED_NO_MOD:
          CM_MSG_LOW_0("Call allowed No Mod");
          sim_cc_result_ptr->call_control_result =
                                       CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

          sim_cc_result_ptr->alpha.dcs = req_rsp->cmd.cc_rsp.cc_req_action.alpha.dcs;

          sim_cc_result_ptr->alpha.len = memscpy( sim_cc_result_ptr->alpha.buf,
                                                  sizeof(sim_cc_result_ptr->alpha.buf),
                                                  req_rsp->cmd.cc_rsp.cc_req_action.alpha.text,
                                                  req_rsp->cmd.cc_rsp.cc_req_action.alpha.length );
          break;
        default:
          /* We got an error from GSTK.  Allowing the call */
          sim_cc_result_ptr->call_control_result =
                                       CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;
          CM_MSG_HIGH_1("Unexected response from GSTK: %d, allowing the call",
                 req_rsp->cmd.cc_rsp.general_resp);
          break;
      }
    }
    else
    {
      /* We got Envelope cmd fail from GSTK.  Not Allowing the call */
      CM_MSG_HIGH_1("GSTK returned err: %d", req_rsp->cmd.cc_rsp.general_resp);
      sim_cc_result_ptr->call_control_result = CM_CC_RESULT_NOT_ALLOWED;
      sim_cc_result_ptr->error_cause = CM_CALL_CMD_ERR_CAUSE_OTHER;
    }
  }

  /* CC complete, invoke CM callback */
  (*cc_object_ptr->cc_complete_cb_func)( cc_object_ptr->call_id,
                             cc_object_ptr->status,
                             cc_object_ptr->call_type,
                             cc_object_ptr->cdma_result_ptr,
                             cc_object_ptr->gw_result_ptr,
                             (void*) cc_object_ptr->user_data);
      /* This sequence is complete, so free the memory. */
  cmcc_object_deallocate( cc_object_ptr );
} /* cmcc_gstk_rsp_cb */
/*lint +esym(818, req_rsp)*/
#ifdef FEATURE_OTASP
/*===========================================================================
FUNCTION cmcc_is_mode_3gpp_only

DESCRIPTION
  Checks whether the current system is configured in 3gpp only mode.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if in 3gpp only
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmcc_is_mode_3gpp_only
(
  cmcall_s_type* call_ptr
  /* call ptr */
)
{
  sd_ss_mode_pref_e_type          supp_sd_mode_pref;

  // 1st check that cap from SDSS common (RF)
  supp_sd_mode_pref = (cmph_map_cm_mode_pref_to_sd_mode_pref(
                         cmph_get_supp_mode_pref(CMPH_SS_MODE_PREF(call_ptr->asubs_id, cmph_ptr()),
                                              CMPH_SS_BAND_PREF(call_ptr->asubs_id, cmph_ptr()),
                                              CMPH_SS_LTE_BAND_PREF(call_ptr->asubs_id, cmph_ptr()),
                                              CMPH_SS_TDS_BAND_PREF(call_ptr->asubs_id, cmph_ptr())))) 
                                              &
                       (cmph_map_cm_mode_pref_to_sd_mode_pref(call_ptr->favored_mode_pref));
  CM_MSG_HIGH_1("supp_sd_mode_pref %d",supp_sd_mode_pref);

  if (cmpmprx_read_cm_policy_consumption_eligibility(call_ptr->asubs_id))
  {
    // if SUB is enabled check cap from Policyman pref
    supp_sd_mode_pref &= (cmph_map_cm_mode_pref_to_sd_mode_pref(
                         cmph_get_supp_mode_pref(
             /*arg 1(mode_pref) */     cmph_map_sd_mode_pref_to_cm_mode_pref(cmph_map_sys_mode_mask_to_sd_mode_pref(
                                        cmmsimc_get_msc_ptr_per_cm_ss(call_ptr->ss)->stack_common_info.ue_mode_capability)),
                                      cmph_map_sd_band_pref_to_cm_band_pref((sd_ss_band_pref_e_type)cmmsimc_get_msc_ptr_per_cm_ss(call_ptr->ss)->stack_common_info.ue_band_capability),
                                      cmmsimc_get_msc_ptr_per_cm_ss(call_ptr->ss)->stack_common_info.ue_lte_band_capability,
                                      cmph_map_sd_tds_band_pref_to_cm_tds_band_pref((sd_ss_band_pref_e_type)cmmsimc_get_msc_ptr_per_cm_ss(call_ptr->ss)->stack_common_info.ue_tds_band_capability))));
     CM_MSG_HIGH_1("after ANDing with MSC : supp_sd_mode_pref %d",supp_sd_mode_pref);
  }
  /* Check if mode pref contains any mode other than GWLT
  */
  if (!(SD_MODE_CONTAIN(supp_sd_mode_pref,~(int)SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE)))
  {
    return TRUE;
  }

  return FALSE;
}/*cmcc_is_mode_3gpp_only*/
#endif /*FEATURE_OTASP*/

/*===========================================================================
FUNCTION cmcc_is_cc_required

DESCRIPTION
  Checks whether cm needs to perform the call control based on the call type and
  subscription.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if call control needs to performed.
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmcc_is_cc_required
(
   cmcc_object_s_type                *cc_object_ptr,
   cm_cc_subscription_source_e_type  cdma_subscription_source,
   cm_cc_subscription_source_e_type  gw_subscription_source
)
{
  cmcall_s_type  *call_ptr = NULL;

  if (cc_object_ptr == NULL)
  {
    CM_ERR_0("cc_object_ptr null ptr");
    return FALSE;
  }

  call_ptr = cmcall_ptr(cc_object_ptr->call_id);

  /* skip call control if
   ** 1. call type is not voice, data, sups, VT, VS
   ** 2. or gw sub source none and cdma sub source nv
   ** 3. 3GPP2 PS Data call during clean up.
   */
  if( cc_object_ptr->call_type != CM_CALL_TYPE_VOICE &&
      cc_object_ptr->call_type != CM_CALL_TYPE_CS_DATA &&
      cc_object_ptr->call_type != CM_CALL_TYPE_PS_DATA &&
      cc_object_ptr->call_type != CM_CALL_TYPE_SUPS &&
      cc_object_ptr->call_type != CM_CALL_TYPE_VT &&
      cc_object_ptr->call_type != CM_CALL_TYPE_VS
    )
  {
    CM_MSG_HIGH_3("skip further CC; call_type: %d, ecc categ %d, act code %d",
                cc_object_ptr->call_type,
                cc_object_ptr->gw_result_ptr->num.emerg_srv_categ,
                cc_object_ptr->orig_request.activate_code);

    return FALSE;
  }

  /* For the case in which there is no gw sub and cdma sub is from NV,
   ** Set cdma call control result = allowed and gw call control result = not allowed
   */
  else if(cdma_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NV &&
          gw_subscription_source == CM_CC_SUBSCRIPTION_SOURCE_NONE
    )
  {
    CM_MSG_HIGH_3("skip further CC; call_type: %d, cdma_subs_src %d, gw_subs_src %d",
                cc_object_ptr->call_type,
                cdma_subscription_source,
                gw_subscription_source);
    cc_object_ptr->cdma_result_ptr->call_control_result
                             = CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS;

    cc_object_ptr->cc_result_buf.error_cause
                              = CM_CALL_CMD_ERR_CAUSE_NO_SUBSC;
    cc_object_ptr->cc_result_buf.call_control_result
                             = CM_CC_RESULT_NOT_ALLOWED;
    cc_object_ptr->gw_result_ptr = &cc_object_ptr->cc_result_buf;

    return FALSE;
  }
  else if ( cc_object_ptr->call_type == CM_CALL_TYPE_PS_DATA   &&
            call_ptr != NULL                                   &&
            !is_contain_3GPP_mode_pref( call_ptr->favored_mode_pref )  &&
            cmclnup_is_ongoing_subsc(BM(call_ptr->asubs_id))
          )
  {
    CM_MSG_HIGH_5("CLNUP: skip CC, call_type %d cdma_subs_src %d gw_subs_src %d call_id %d call_ptr->fav_mode_pref %d",
                cc_object_ptr->call_type, cdma_subscription_source, gw_subscription_source,
                cc_object_ptr->call_id, call_ptr->favored_mode_pref);
  
    return FALSE;
  }

  /* For all other cases, call control needs to be performed */
  return TRUE;
}

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================
FUNCTION cmcc_is_number_emerg_on_other_sub

DESCRIPTION
  Checks whether number is emergency on other sub as well

DEPENDENCIES
  None

RETURN VALUE
  TRUE if emergency and other sub is active.
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmcc_is_number_emerg_on_other_sub
(
  cmcc_object_s_type                  *cc_object_ptr
)
{
  cmcall_s_type               *call_ptr = cmcall_ptr(cc_object_ptr->call_id);
  cm_num_s_type* orig_num_ptr                   = &(cc_object_ptr->orig_request.num);
  boolean found_in_gw = FALSE;
  boolean found_in_1x = FALSE;
  uint8 cdma_ecc_categ,gw_ecc_categ = 0;
  pbm_session_enum_type pbm_session_type;
  mmgsdi_session_type_enum_type session_type;

  if(!cmph_is_msim()
  || cmph_ptr()->internal_standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY)
  {
    return FALSE;
  }
  if(call_ptr == NULL)
  {
       return FALSE;
  }
  /* Check if number is emergency on other subs*/
  if(call_ptr->asubs_id == SYS_MODEM_AS_ID_1)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }

  pbm_session_type = pbmif_mmgsdi_session_to_pbm_session(session_type);
  if(pbm_session_emergency_number_cat_ext( pbm_session_type,
                                              orig_num_ptr->buf,
                                              orig_num_ptr->len,
                                              &found_in_gw,
                                              &found_in_1x,
                                              &gw_ecc_categ,
                                              &cdma_ecc_categ)== PBM_SUCCESS)
  {
    if(found_in_gw)
    {
      return TRUE;
    }

    /* If number is emergency only in CDMA & call is originated on sub2,
       ** then number callnot be emergency on other sub*/
    if(found_in_1x && call_ptr->asubs_id == SYS_MODEM_AS_ID_2)
    {
      return TRUE;
    }
  }

  return FALSE;
}
#endif
