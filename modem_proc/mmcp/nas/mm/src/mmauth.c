  /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Authentication server (AUTH.C)

GENERAL DESCRIPTION
  This file contains an implementation of the authentication module that
  provides ...

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001, 2002, 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/auth.c_v   1.17   18 Jul 2002 11:39:00   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmauth.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/10   zren    Updated fun param to set only_for_reg flag in REG CNF and 
                   REG IND
12/18/09   RI      Updates for unified read for EFS/NV/USIM files for multimode.
10/20/09   RI      Added AMF handling during authentication.
10/06/09   RI      Fix compiler warnings.
08/31/09   RI      Fixed the global returns for LTE security.
02/06/08   ab    Ensuring CKSN value used in messages is always less than 0x07
11/21/07    ab     removed lint errors
01/18/06    vdr    Now starting cache mem guard timer if 3G authentication is 
                   performed by USIM 
01/04/05    vdr    Corrected cache clean up
12/17/04    vdr    o Added cache memory mechanism in order to prevent SYNC failure 
                     in case of "the same RAND authentication" scenario
                   o Removed auth_domain_identity global variable
10/13/04    vdr    Fixed Authentication Request search procedure
10/05/04    vdr    Fixed compilation error
10/01/04    vdr    Added recovery mechanism in order to fix possible memory corruption
09/01/04    vdr    Added invatidation of the GSM/GPRS ciphering algorithm upon
                   invalidation of the PS security context
08/25/04    npr    Update auth_ps_ciphering_algorithm only for PS domain
09/02/03    vdr    Fixed UMTS CS & PS CKSN initialization when CK & IK are not available
05/15/03    vdr    Added UMTS-into-GSM ciphering key convertion (C3 conversion)
04/16/03    vdr    Now rejecting GSM AKA when in UMTS mode and MS is running USIM 
02/21/03    vdr    Fixed GSM-To-UMTS Security Key generation criteria
02/18/03    cd     Fixed memory error introduced when fixing Lint errors
02/10/03    cd     Removed Lint errors
02/04/03    ks     Added FEATURE_MMGSDI_UMTS if mm_sim_card_mode is GSDI_CARD_USIM
                   in function auth_build_sim_authentication_request().
12/18/02    vdr    Added NV support for the GSM GPRS security context
                   Changed multiple cases of Authentication recovering procedure
12/12/02    ks     Replaced the mm_sim_valid with mm_serving_plmn.info.sim_state.
11/26/02    jca    Removed code to set mm_sim_valid = TRUE; this is done in mmcoord.c
11/14/02    mks    Added support for sending security updates to both active
                   and inactive RATs. Now setting the mm_sim_valid flag to
                   TRUE if gsdi_get_card_mode() returns success.
11/11/02    vdr    Fixed auth_delete_cksn() for GSM GPRS security context
11/05/02    vdr    Modified the code which prevanted GSM GPRS authentication
                   in case of SIM that does not have GSM GPRS security context
10/24/02    vdr    Fixed USIM Authentication Response parsing error
                   Fixed processing of the Authentication transaction reset
10/16/02    vdr    Fixed Authentication SIM MAK & SQN error handling
10/15/02    vdr    Changed Authentication server's data structure according
                   to the definitions of new GSDI interface
10/10/02    vdr    Changed invalidation procedure of the CKSN when UMTS MS is
                   running GSM SIM
10/04/02    vdr    Fixed HFN handling when the MS is activated by the 2G SIM card
10/01/02    vdr    Fixed Authentication recovering procedure
                   Changed GSDI interface according to new GSDI task definitions
09/28/02    vdr    Added invalidation of the specific security context according
                   to the domain identity that deleted HFN is belonging to
09/23/02    vdr    Fixed initialization of the GSM SIM security data
09/10/02    vdr    Fixed Authentication attempt termination condition for
                   UMTS/2G authentication scenario
09/04/02    vdr    Added GSM-to-UMTS Security Key conversion
08/26/02    mks    Now calling mm_send_rrc_security_updates to notify security
                   updates to RRC
08/10/02    mks    Now sending SIM updates to RRC
07/18/02    vdr    Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/16/02    vdr    Added HFN caching mechanism
                   Removed obsolete Kc & CKSN access functions
07/15/02    vdr    Fixed saving of the GSM (CS/PS) security context
07/12/02    vdr    Added GSM GPRS security context handling
07/05/02    vdr    Fixed Authentication and Security Keys management in mixed
                   networks
05/15/02    vdr    Changed SIM/USIM access error recovery
05/02/02    vdr    Modified HFN handling
                   Added security info caching
                   Added multi SIM/multi SIM configuration support
04/24/02    vdr    Added HFN handling
04/02/02    vdr    Fixed format of the saving USIM security context
02/28/02    vdr    Fixed handling of the different security contextes
02/28/02    vdr    Added simultaneous handling of the different security contextes
02/22/02    vdr    Added validity check of the GPRS security keys before
                   sending them to the RRC
02/04/02    ks     Added FEATURE_MMGSDI_UMTS if mm_sim_card_mode is GSDI_CARD_USIM
                   in function auth_build_sim_authentication_request().
03/15/05    sm     Now, aborting RR/RRC connections and sending CNM release 
                   indication in case auth_reset_attempt
03/19/05    sm     Writing HFNs into USIM in RRC format and loading them as RRC format
03/21/05    sm     Authentication failures are counted and if there are 3 
                   conseqcutive failures then the Network fails 
                   in Authentication.
03/28/05    sm     Now, sending RRC_ABORT_REQ, RR_ABORT_REQ for cell barring 
                   incase Network fails the Authentication
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include <stdio.h>
#include <string.h>
#include <stringl/stringl.h>
#include "customer.h"
#include "comdef.h"
#include "err.h"
#include "msg.h"
#include "bit.h"

#if defined (FEATURE_LTE)
  #include "lte_nas_log.h"
  #include "naslog_v.h"
#endif

#include "rrcmmif.h"
#include "rrccmd.h"

#include "mm_v.h"
#include "mm_sim.h"
#include "sys_cnst.h"
#include "mm_coord.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#else
#include "gs_v.h"
#endif

#include "timers_v.h"


#include "gmm_int_v.h"

#include "mmauth.h"

#ifdef FEATURE_GSM_GPRS
#include "gmmllcm.h"
#endif /* FEATURE_GSM_GPRS */

#include "ghdi_exp.h"
#include "ghdi_exp_int.h"

#include "mmsecurity.h"
#include "IxErrno.h"
#include "secapi.h"
#include "fs_public.h"  /* Interface to EFS services */
#include "mcfg_fs.h"
#ifdef FEATURE_LTE
#ifdef TEST_FRAMEWORK
 #error code not present
#endif


#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK


#include "emm_database.h"
#include "emm_rrc_if.h"
#include "emm_security.h"
#include "emm_utility.h"
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

/* Authentication request buffer. This variable is able to keep up two
** Authentication and Authentication & Ciphering requests simultaneously if
** a first authentication request has been failed.
*/
static
auth_request_storage_type auth_request_storage_sim[MAX_NAS_STACKS][MAX_REQUEST_STORAGE_SIZE] ;

/* Authentication command buffer. This variable is able to keep up two
** Authentication and Authentication & Ciphering requests simultaneously
** when the SIM/USIM card perform authentication procedure
*/
static
auth_request_storage_type auth_suspended_cmds_buff_sim[MAX_NAS_STACKS][MAX_REQUEST_STORAGE_SIZE] ;

/*
** GSM Ciphering Key (CS domain)
*/
auth_gsm_cipherkey_type auth_gsm_cipherkey_sim[MAX_NAS_STACKS];

auth_gsm_cipherkey_type     gsm_kc_128bit_sim[MAX_NAS_STACKS] ;
auth_gsm_cipherkey_type     gsm_gprs_kc_128bit_sim[MAX_NAS_STACKS] ;

byte auth_gsm_cksn_sim[MAX_NAS_STACKS];

/*
** GSM GPRS Ciphering Key (PS domain)
*/
auth_gsm_cipherkey_type auth_gsm_gprs_cipherkey_sim[MAX_NAS_STACKS];

byte auth_gsm_gprs_cksn_sim[MAX_NAS_STACKS];

/* GPRS service availability indicator */
boolean auth_gsm_gprs_keys_present_sim[MAX_NAS_STACKS];

/*
** CS domain GPRS Ciphering Key
*/
auth_gprs_cipherkey_type auth_cs_gprs_cipherkey_sim[MAX_NAS_STACKS] ;

/*
** CS domain GPRS Integrity Key
*/
auth_gprs_integritykey_type auth_cs_gprs_integritykey_sim[MAX_NAS_STACKS] ;

byte auth_cs_gprs_cksn_sim[MAX_NAS_STACKS];

/*
** PS domain GPRS Ciphering Key
*/
auth_gprs_cipherkey_type auth_ps_gprs_cipherkey_sim[MAX_NAS_STACKS] ;

/*
** PS domain GPRS Ciphering Key
*/
auth_gprs_cipherkey_type lte_auth_ps_gprs_cipherkey ;

/*
** PS domain GPRS Integrity Key
*/
auth_gprs_integritykey_type lte_auth_ps_gprs_integritykey ;

/*
** PS domain GPRS Integrity Key
*/
auth_gprs_integritykey_type auth_ps_gprs_integritykey_sim[MAX_NAS_STACKS] ;

byte auth_ps_gprs_cksn_sim[MAX_NAS_STACKS];

byte auth_ps_ciphering_algorithm_sim[MAX_NAS_STACKS];

/* USIM SO Table */
//static byte auth_usim_so_buffer_sim[MAX_NAS_STACKS][255] ;


static hfn_type auth_current_hfns_sim[MAX_NAS_STACKS] ;

/***********************************************************************/
/*                  Authentication cache memory                        */
/***********************************************************************/

/* This variable contains the RAND for which authentication is performing right now */
static byte mm_ongoing_rand_sim[MAX_NAS_STACKS][MAX_AUTH_DATA_LENGTH] ;

/* This variable contains last RAND which has been received from the NW */
static byte mm_last_rand_cs_sim[MAX_NAS_STACKS][MAX_AUTH_DATA_LENGTH] ;
static byte mm_last_rand_ps_sim[MAX_NAS_STACKS][MAX_AUTH_DATA_LENGTH] ;
/* This variable contains last RES which has been sent to the NW */
static auth_res_type mm_last_res_cs_sim[MAX_NAS_STACKS] ;
static auth_res_type mm_last_res_ps_sim[MAX_NAS_STACKS] ;

/* Last Domain ID which Authentication has been initiated for */
static rrc_cn_domain_identity_e_type mm_last_authenticated_domain_sim[MAX_NAS_STACKS] ;


#define auth_request_storage (auth_request_storage_sim[mm_as_id])
#define auth_suspended_cmds_buff (auth_suspended_cmds_buff_sim[mm_as_id])
//#define auth_usim_so_buffer (auth_usim_so_buffer_sim[mm_as_id])

#define auth_current_hfns (auth_current_hfns_sim[mm_as_id])
#define mm_ongoing_rand (mm_ongoing_rand_sim[mm_as_id])
#define mm_last_rand_cs (mm_last_rand_cs_sim[mm_as_id])
#define mm_last_res_cs (mm_last_res_cs_sim[mm_as_id])
#define mm_last_rand_ps (mm_last_rand_ps_sim[mm_as_id])
#define mm_last_res_ps (mm_last_res_ps_sim[mm_as_id])
#define mm_last_authenticated_domain (mm_last_authenticated_domain_sim[mm_as_id])

sys_modem_as_id_e_type mm_sim_get_as_id
(
  mmgsdi_session_id_type session_id
);
static ARFCN_T    mm_auth_failed_arfcn_sim[MAX_NAS_STACKS];
#define mm_auth_failed_arfcn mm_auth_failed_arfcn_sim[mm_as_id]

static uint8    mm_auth_failed_bsic_sim[MAX_NAS_STACKS];
#define mm_auth_failed_bsic mm_auth_failed_bsic_sim[mm_as_id]

#else


/* Authentication request buffer. This variable is able to keep up two
** Authentication and Authentication & Ciphering requests simultaneously if
** a first authentication request has been failed.
*/
static
auth_request_storage_type auth_request_storage[MAX_REQUEST_STORAGE_SIZE] ;

/* Authentication command buffer. This variable is able to keep up two
** Authentication and Authentication & Ciphering requests simultaneously
** when the SIM/USIM card perform authentication procedure
*/
static
auth_request_storage_type auth_suspended_cmds_buff[MAX_REQUEST_STORAGE_SIZE] ;

/*
** GSM Ciphering Key (CS domain)
*/
auth_gsm_cipherkey_type auth_gsm_cipherkey;

byte auth_gsm_cksn ;

/*
** GSM GPRS Ciphering Key (PS domain)
*/
auth_gsm_cipherkey_type auth_gsm_gprs_cipherkey;

auth_gsm_cipherkey_type     gsm_kc_bit128;

/*New changes for the gprs key*/
auth_gsm_cipherkey_type     gsm_gprs_kc_bit128;

byte auth_gsm_gprs_cksn;

/* GPRS service availability indicator */
boolean auth_gsm_gprs_keys_present;

/*
** CS domain GPRS Ciphering Key
*/
auth_gprs_cipherkey_type auth_cs_gprs_cipherkey ;

/*
** CS domain GPRS Integrity Key
*/
auth_gprs_integritykey_type auth_cs_gprs_integritykey ;

byte auth_cs_gprs_cksn;

/*
** PS domain GPRS Ciphering Key
*/
auth_gprs_cipherkey_type auth_ps_gprs_cipherkey, lte_auth_ps_gprs_cipherkey ;

/*
** PS domain GPRS Integrity Key
*/
auth_gprs_integritykey_type auth_ps_gprs_integritykey, lte_auth_ps_gprs_integritykey ;

byte auth_ps_gprs_cksn;

byte auth_ps_ciphering_algorithm;

static hfn_type auth_current_hfns ;

/***********************************************************************/
/*                  Authentication cache memory                        */
/***********************************************************************/

/* This variable contains the RAND for which authentication is performing right now */
static byte mm_ongoing_rand[MAX_AUTH_DATA_LENGTH] ;

/* This variable contains last RAND which has been received from the NW */
static byte mm_last_rand_cs[MAX_AUTH_DATA_LENGTH] ;
static byte mm_last_rand_ps[MAX_AUTH_DATA_LENGTH] ;

/* This variable contains last RES which has been sent to the NW */
static auth_res_type mm_last_res_cs ;
static auth_res_type mm_last_res_ps;

/* Last Domain ID which Authentication has been initiated for */
static rrc_cn_domain_identity_e_type mm_last_authenticated_domain ;

#endif /* FEATURE_DUAL_SIM  ||  FEATURE_SGLTE*/

#ifdef FEATURE_DUAL_SIM

#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#define mm_ghdi_nvmem_write(a,b) ghdi_nvmem_write_per_subs(a,b,mm_nv_context_id)

#else

#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#define mm_ghdi_nvmem_write(a,b) ghdi_nvmem_write(a,b)

#endif

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
/*
** SIM/USIM card Authentication Request Sequence Number
**
** NOTE:  The valid range of the Authentication Request Sequence Number
**        is 0x1 - 0xFFFF, 0 is a reserved value.Any of SIM/USIM Authentication
**        Request shall not be sent to the GSDI task with ZERO client reference
*/
static dword auth_request_reference_number_sim[MAX_NAS_STACKS] = { 1, 1, 1 } ;
#define auth_request_reference_number (auth_request_reference_number_sim[mm_as_id])

/* Cache memory validity indicator */
static boolean mm_cache_mem_cs_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE } ;
#define mm_cache_mem_cs_valid (mm_cache_mem_cs_valid_sim[mm_as_id])

static boolean mm_cache_mem_ps_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE } ;
#define mm_cache_mem_ps_valid (mm_cache_mem_ps_valid_sim[mm_as_id])

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE /* FEATURE_DUAL_SIM ||  FEATURE_SGLTE*/
/*
** SIM/USIM card Authentication Request Sequence Number
**
*/
static dword auth_request_reference_number_sim[MAX_NAS_STACKS] = { 1, 1 } ;
#define auth_request_reference_number (auth_request_reference_number_sim[mm_as_id])

/* Cache memory validity indicator */
static boolean mm_cache_mem_cs_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#define mm_cache_mem_cs_valid (mm_cache_mem_cs_valid_sim[mm_as_id])

static boolean mm_cache_mem_ps_valid_sim[MAX_NAS_STACKS] = { FALSE, FALSE } ;
#define mm_cache_mem_ps_valid (mm_cache_mem_ps_valid_sim[mm_as_id])

#else
/*
** SIM/USIM card Authentication Request Sequence Number
**
*/
static dword auth_request_reference_number = 1 ;

/* Cache memory validity indicator */
static boolean mm_cache_mem_cs_valid = FALSE ;
static boolean mm_cache_mem_ps_valid = FALSE ;
static ARFCN_T    mm_auth_failed_arfcn;

static uint8    mm_auth_failed_bsic;
#endif

extern void gmm_handle_common_auth_response
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
#ifdef FEATURE_LTE
extern void emm_handle_auth_response
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
#endif
/***********************************************************************/

void auth_mmgsdi_generic_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mm_cmd_type *mm_cmd_p;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id;
#endif

  if (cnf_ptr == NULL)
  {
     MSG_ERROR_DS(MM_SUB, "=MM= MMGSDI returned cnf_ptr as NULL",0,0,0);
     return;
  }

  switch (cnf)
  {
    case MMGSDI_USIM_AUTH_CNF:
      if ((mm_cmd_p = mm_get_cmd_buf()) != NULL)
      {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
        as_id = mm_sim_get_as_id(cnf_ptr->response_header.session_id);
        if(IS_NOT_VALID_SUB_ID(as_id))
        {
          MSG_ERROR_DS(MM_LOCAL_SUB, "=MM= Invalid AS ID: %d", as_id+1,0,0);
        }
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.as_id = as_id;
#endif

        MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= MMGSDI CNF %d Status %d ", cnf, status);

        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.message_header.message_set = MS_GSDI;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.message_header.message_id  =
                      SIM_MM_USIM_AUTH_CNF;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.message_header.message_len_lsb =
                   (sizeof(sim_mm_gsdi_cnf_T) - sizeof(IMH_T)) % 256;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.message_header.message_len_msb =
                   (sizeof(sim_mm_gsdi_cnf_T) - sizeof(IMH_T)) / 256;

        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.message_header.client_ref =
                                      cnf_ptr->usim_auth_cnf.response_header.client_data;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.message_header.gsdi_status = status;

        memscpy(mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.data,MMGSDI_MAX_DATA_BLOCK_LEN,
               cnf_ptr->usim_auth_cnf.auth_response.data_ptr,
               cnf_ptr->usim_auth_cnf.auth_response.data_len);
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.usim_auth_cnf.returned_data_len = 
                        (byte) cnf_ptr->usim_auth_cnf.auth_response.data_len;

        /*
        ** Put on MM command queue
        */
        mm_put_cmd(mm_cmd_p);
      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
      }
      break;

    case MMGSDI_SESSION_RUN_GSM_ALGO_CNF:
      if ((mm_cmd_p = mm_get_cmd_buf()) != NULL)
      {
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
        as_id = mm_sim_get_as_id(cnf_ptr->response_header.session_id);
        if(IS_NOT_VALID_SUB_ID(as_id))
        {
          MSG_ERROR_DS(MM_LOCAL_SUB, "=MM= Invalid AS ID: %d", as_id+1,0,0);
        }
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.as_id = as_id;
#endif

        MSG_HIGH_DS_2(MM_LOCAL_SUB, "=MM= MMGSDI CNF %d Status %d ", cnf, status);

        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.message_header.message_set = MS_GSDI;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.message_header.message_id  =
                     SIM_MM_GSM_ALGO_CNF;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.message_header.message_len_lsb =
                   (sizeof(sim_mm_gsdi_cnf_T) - sizeof(IMH_T)) % 256;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.message_header.message_len_msb =
                   (sizeof(sim_mm_gsdi_cnf_T) - sizeof(IMH_T)) / 256;

        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.message_header.client_ref =
                                      cnf_ptr->session_run_gsm_algo_cnf.response_header.client_data;
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.message_header.gsdi_status = status;

        /* SRES */
        memscpy(mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.data,MMGSDI_MAX_DATA_BLOCK_LEN,
               cnf_ptr->session_run_gsm_algo_cnf.sres_resp,
               MMGSDI_GSM_ALGO_SRES_LEN);
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.returned_data_len = 
                            MMGSDI_GSM_ALGO_SRES_LEN;
        /* GSM Kc */
        memscpy(&(mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.data[MMGSDI_GSM_ALGO_SRES_LEN]),MMGSDI_MAX_DATA_BLOCK_LEN-MMGSDI_GSM_ALGO_SRES_LEN,
               cnf_ptr->session_run_gsm_algo_cnf.kc_resp,
               MMGSDI_GSM_ALGO_KC_LEN);
        mm_cmd_p->cmd.sim_mm_gsdi_cnf.run_gsm_alg_cnf.returned_data_len += 
                            MMGSDI_GSM_ALGO_KC_LEN;

        /*
        ** Put on MM command queue
        */
        mm_put_cmd(mm_cmd_p);
      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
      }
      break;
    default:
      MSG_HIGH_DS_1(MM_SUB, "=MM= MMGSDI CNF %d not handled at MM", cnf);
      break;
  }
  
}

/*===========================================================================

FUNCTION      AUTH_START_AUTHENTICATION

DESCRIPTION
  This function prepares the SIM/USIM Authentication Request command buffer
  and send it to GSDI task in order to perform an Authentication

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_start_authentication
(
  /* Current Sim card mode (SIM/USIM) */
  mmgsdi_app_enum_type        sim_card_mode,

  /* Authentication Request reference number */
  dword reference_number,

  /* RAND value pointer */
  auth_rand_type                  *p_rand,

  /* AUTN value pointer */
  auth_autn_type                  *autn,

  /* Pointer to the properties of the appropriate Authentication request */
  auth_request_properties_type  *request_properties
)
{
  mmgsdi_autn_context_enum_type auth_context = MMGSDI_USIM_AUTN_MAX;
  mmgsdi_data_type              auth_req;
  mmgsdi_return_enum_type auth_req_status = MMGSDI_ERROR;

  switch (sim_card_mode)
  {
    case MMGSDI_APP_SIM:
      if (autn->value_length > 0)
      {
        MSG_ERROR_DS(MM_SUB, "=MM= The server is unable to perform UMTS authentication",0,0,0) ;
        return ;
      }

      /* Start GSM authentication challenge */
#ifndef FEATURE_MODEM_HEAP
      auth_req.data_ptr = (uint8 *) gs_alloc(p_rand->value_length);
#else
      auth_req.data_ptr = (uint8 *) modem_mem_calloc(1, p_rand->value_length, MODEM_MEM_CLIENT_NAS);
#endif
      if (auth_req.data_ptr == NULL)
      {
        MSG_ERROR_DS(MM_SUB, "=MM= NAS Auth: Mem alloc failed",0,0,0);
        return;
      }
      memscpy((void *)auth_req.data_ptr,p_rand->value_length,
              (void *)p_rand->value_data,p_rand->value_length) ;
      auth_req.data_len = p_rand->value_length;

      request_properties->current_auth_request_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;

      auth_req_status = mmgsdi_session_run_gsm_algo_ext 
                                  (
                                   mm_sim_mmgsdi_info.session_id,
                                   auth_req,
                                   TRUE,
                                   auth_mmgsdi_generic_cb,
                                   reference_number
                                  );

      break ;
    case MMGSDI_APP_USIM:
      if (autn->value_length == 0)
      {
        autn = NULL ;
      }

      if (autn != NULL)
      {
        /* If the AUTN is present and its length grater than 0 use
        ** UMTS security context
        */
        /*   If Context is MMGSDI_AUTN_USIM_3G_CONTEXT
        **   Data should be in the following format -
        **   Rand Length
        **   Rand Data
        **   Auth Length
        **   Auth Data
        */
        auth_req.data_len = 1+p_rand->value_length+1+autn->value_length;

#ifndef FEATURE_MODEM_HEAP
        auth_req.data_ptr = (uint8 *) gs_alloc(auth_req.data_len);
#else
        auth_req.data_ptr = (uint8 *) modem_mem_calloc(1, auth_req.data_len, MODEM_MEM_CLIENT_NAS);
#endif
        if (auth_req.data_ptr == NULL)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= NAS Auth: Mem alloc failed",0,0,0);
          return;
        }
        auth_req.data_ptr[0] = p_rand->value_length;
        memscpy((void *)(&auth_req.data_ptr[1]),auth_req.data_len-1,
               (void *)p_rand->value_data,
                       p_rand->value_length);

        auth_req.data_ptr[1+p_rand->value_length] = autn->value_length;
        memscpy((void *)(&auth_req.data_ptr[1+p_rand->value_length+1]),auth_req.data_len -(1+p_rand->value_length+1),
               (void *)autn->value_data,
                       autn->value_length);

        auth_context = MMGSDI_AUTN_USIM_3G_CONTEXT;

        request_properties->current_auth_request_context = MMGSDI_AUTN_USIM_3G_CONTEXT;
      }
      else
      {
#ifdef FEATURE_WCDMA      
        if (is_umts_mode())
        {
            MSG_ERROR_DS(MM_SUB, "=MM= GSM AKA is unacceptable",0,0,0) ;

          /* Reject erroneous auth request reference number */
          auth_handle_sim_authentication_response(NULL, reference_number) ;

          /* Stop authentication attempt */
          return ;
        }
        else 
#endif /*FEATURE_WCDMA*/          
#ifdef FEATURE_GSM        
        if (is_gsm_mode())
        {
          if ((!auth_gsm_access) || (!auth_usim_gsm_security_context))
          {
            MSG_ERROR_DS(MM_SUB, "=MM= Unable to run GSM auth due to USIM config",0,0,0) ;
            return ;
          }

          /* If the AUTN is present but its length is equal to 0 use
          ** GSM security context
          */
          /*   If Context is MMGSDI_AUTN_USIM_GSM_CONTEXT
          **   Data should be in the following format -
          **   Rand Length
          **   Rand Data
          */
          auth_req.data_len = 1+p_rand->value_length;
#ifndef FEATURE_MODEM_HEAP
          auth_req.data_ptr = (uint8 *) gs_alloc(auth_req.data_len);
#else
          auth_req.data_ptr = (uint8 *) modem_mem_calloc(1, auth_req.data_len, MODEM_MEM_CLIENT_NAS);
#endif
          if (auth_req.data_ptr == NULL)
          {
            MSG_ERROR_DS(MM_SUB, "=MM= NAS Auth: Mem alloc failed",0,0,0);
            return;
          }
          auth_req.data_ptr[0] = p_rand->value_length;
          memscpy((void *)(&auth_req.data_ptr[1]),auth_req.data_len-1,
                 (void *)p_rand->value_data,
                         p_rand->value_length);

          auth_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;

          request_properties->current_auth_request_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;
        }
        else
#endif /*FEATURE_GSM*/          
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unknown SIM card mode",0,0,0) ;

          /* Stop authentication attempt */
          return ;
        }
      }

      auth_req_status = mmgsdi_session_usim_authenticate_ext 
                                       (
                                        mm_sim_mmgsdi_info.session_id,
                                        auth_context,
                                        auth_req,
                                        TRUE,
                                        auth_mmgsdi_generic_cb,
                                        reference_number
                                       );

      break ;
    default:
      MSG_ERROR_DS(MM_SUB, "=MM= Illegal SIM card mode (%d)",sim_card_mode,0,0);
      return ;
  }

  /* Data passed into our MMGSDI which has been allocated by NAS 
    *   should be deallocated by NAS after calling the API. */
  if (auth_req.data_ptr != NULL)
  {
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free(auth_req.data_ptr, MODEM_MEM_CLIENT_NAS);
#else
    gs_free(auth_req.data_ptr);
#endif
  }

  if (auth_req_status == MMGSDI_SUCCESS)
  {
    mm_last_authenticated_domain = request_properties->cn_domain_identity ;

    /* Store incoming RAND */
    memscpy((void *)mm_ongoing_rand, MAX_AUTH_DATA_LENGTH,(void *)p_rand->value_data,p_rand->value_length) ;

    /* Invalidate cache memory */
    if(mm_last_authenticated_domain==RRC_CS_DOMAIN_CN_ID)
    {
      mm_cache_mem_cs_valid = FALSE ;
      mm_cs_auth_sent_to_card = TRUE;
    }
    else
    { 
      mm_cache_mem_ps_valid = FALSE;
      mm_ps_auth_sent_to_card = TRUE;      
    }
  }
}/* end auth_start_authentication() */


/*===========================================================================

FUNCTION    AUTH_PUT_REQUEST_INTO_STORAGE

DESCRIPTION
  This function saves a authentication request that is currently placed in
  incoming Authentication Request buffer <auth_suspended_cmds_buff> into
  <auth_request_storage>

DEPENDENCIES
  None

RETURN VALUE
  After the Authentication Request has been saved the function returns a
  pointer to new loccation.

SIDE EFFECTS
  None

===========================================================================*/
auth_request_properties_type *auth_put_request_into_storage
(
  /* Authentication request's client callback pointer */
  auth_response_callback_type cur_callback
)
{
  byte search_index;

  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (auth_suspended_cmds_buff[search_index].
            auth_request_properties.client_response_callback == cur_callback)
    {
      byte storage_index ;
      auth_request_storage_type *saved_entry_ptr = NULL ;

      for (storage_index=0;
                     storage_index<MAX_REQUEST_STORAGE_SIZE; storage_index++)
      {
        if (auth_request_storage[storage_index].
               auth_request_properties.client_response_callback ==
                    auth_suspended_cmds_buff[search_index].
                              auth_request_properties.client_response_callback)
        {
          saved_entry_ptr = &auth_request_storage[storage_index] ;

          break ;
        }
        else if (auth_request_storage[storage_index].
                      auth_request_properties.client_response_callback == NULL)
        {
          if (saved_entry_ptr == NULL)
          {
            saved_entry_ptr = &auth_request_storage[storage_index] ;
          }
        }
      }

      if (saved_entry_ptr != NULL)
      {
        memscpy((void *)saved_entry_ptr,sizeof(auth_request_storage_type),
                       (void *)&auth_suspended_cmds_buff[search_index],
                                          sizeof(auth_request_storage_type)) ;

        memset((void *)&auth_suspended_cmds_buff[search_index],
                                     0x00,sizeof(auth_request_storage_type)) ;

        return &saved_entry_ptr->auth_request_properties ;
      }
      
    }
  }

  MSG_ERROR_DS(MM_SUB, "=MM= There is no enough room to save SIM auth. response.",0,0,0) ;

  return NULL ;

} /* end auth_put_request_into_storage() */


/*===========================================================================

FUNCTION      AUTH_FIND_FREE_TMP_ENTRY

DESCRIPTION
  This function is looking for a free entry in the <auth_suspended_cmds_buff>.
  The entry is empty if it does not contain another Authentication Request
  or if it contains another Authentication Request from the same client

DEPENDENCIES
  None

RETURN VALUE
  The function returns a pointer to the free <auth_suspended_cmds_buff>'s
  entry

SIDE EFFECTS
  None

===========================================================================*/
static auth_request_storage_type *auth_find_free_tmp_entry
(
  /* Incoming MM/GMM Authentication Request */
  auth_request_properties_type *current_request_property
)
{
  int search_index ;
  auth_request_storage_type *saved_entry_ptr = NULL ;

 for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
       MSG_HIGH_DS_3(MM_SUB, "=MM= \n Auth_Debug: index %d, auth_suspended_cmds_buff ref_num : %d, cn_domain_identity = %d  (0 - CS, 1 - PS) ",search_index,
                  auth_suspended_cmds_buff[search_index].auth_request_reference_num, auth_suspended_cmds_buff[search_index].auth_request_properties.cn_domain_identity );     
  }

  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if ((current_request_property->cn_domain_identity ==
                    auth_suspended_cmds_buff[search_index].
                              auth_request_properties.cn_domain_identity) &&
        (auth_suspended_cmds_buff[search_index].
                      auth_request_properties.client_response_callback != NULL))
    {
      saved_entry_ptr = &auth_suspended_cmds_buff[search_index] ;

      break ;
    }
    else if (auth_suspended_cmds_buff[search_index].
                      auth_request_properties.client_response_callback == NULL)
    {
      if (saved_entry_ptr == NULL)
      {
        saved_entry_ptr = &auth_suspended_cmds_buff[search_index] ;
      }
    }
  }

  return saved_entry_ptr ;

} /* end auth_find_free_tmp_entry() */


/*===========================================================================

FUNCTION      AUTH_SAVE_REQUEST_PROPERTIES

DESCRIPTION
  This function saves the incoming MM/GMM Authentication Request into
  <auth_suspended_cmds_buff>

DEPENDENCIES
  None

RETURN VALUE
  The function returns auth_request_reference_number that has been allocated
  for the current MM/GMM Authentication Request

SIDE EFFECTS
  Upon reception new MM/GMM Authentication Request the function increments
  the <auth_request_reference_number>

===========================================================================*/
auth_request_storage_type *auth_save_request_properties
(
  byte                          ciphering_key_sequence_number,
  auth_request_properties_type *current_request_property
)
{
  auth_request_storage_type *auth_request_tmp_storage ;

  if ((auth_request_tmp_storage=
                  auth_find_free_tmp_entry(current_request_property)) == NULL)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= There is no enough room to save incoming auth request",0,0,0) ;

    switch (current_request_property->cn_domain_identity)
    {
      case RRC_CS_DOMAIN_CN_ID :
        MSG_ERROR_DS(MM_SUB, "=MM= Recovering, ongoing PS authentication may by aborted",0,0,0) ;
        break ;
      case RRC_PS_DOMAIN_CN_ID :
        MSG_ERROR_DS(MM_SUB, "=MM= Recovering, ongoing CS authentication may by aborted",0,0,0) ;
        break ;
      default :
          MSG_FATAL_DS(MM_SUB, "=MM= Invalid domain Id=%d",
                        (int)current_request_property->cn_domain_identity,0,0) ;
    }    

    memset((byte *)auth_suspended_cmds_buff,0x00,
                (sizeof(auth_request_storage_type)*MAX_REQUEST_STORAGE_SIZE)) ;

    auth_request_tmp_storage = auth_suspended_cmds_buff ;
  }

  auth_request_tmp_storage->
      auth_request_properties.ciphering_key_sequence_number =
                                             ciphering_key_sequence_number ;

  auth_request_tmp_storage->auth_request_reference_num =
                                             auth_request_reference_number ;
#ifdef FEATURE_SGLTE
if (MM_SUB_IS_SGLTE &&
    (RRC_PS_DOMAIN_CN_ID == current_request_property->cn_domain_identity ))
   {
   /* Adding domain id in the first bit of the reference id 0 for cs 1 for ps , as
         CS is by default zero so no need to add */
      auth_request_tmp_storage->auth_request_reference_num =  
          auth_request_tmp_storage->auth_request_reference_num |0x80000000;
   }
#endif  
  auth_request_reference_number =
            (auth_request_reference_number==(dword)(0xFFFFFFFF))?
                                          1:(auth_request_reference_number+1) ;

  memscpy((void *)&auth_request_tmp_storage->auth_request_properties,sizeof(auth_request_properties_type),
      (void *)current_request_property,sizeof(auth_request_properties_type)) ;

  return auth_request_tmp_storage ;
}


/*===========================================================================

FUNCTION      AUTH_AUTHENTICATION_REQUEST

DESCRIPTION
  This function saves incoming MM/GMM Authentication Request and sends
  Authentication Request to the GSDI

DEPENDENCIES
  None

RETURN VALUE
  The function returns Client ID

SIDE EFFECTS
  None

===========================================================================*/
client_id auth_authentication_request
(
  /* Ciphering Key Sequence Number */
  byte                          ciphering_key_sequence_number,

  /* Pointer to the RAND value */
  auth_rand_type                *p_rand,

  /* Pointer to the AUTN value */
  auth_autn_type                *autn,

  /* Pointer to the MM/GMM Authentication Request properties */
  auth_request_properties_type  *request_properties
)
{
  dword reference_number ;
  client_id id = (client_id)NULL ;

  auth_request_storage_type *auth_request_storage_ptr =
          auth_save_request_properties
                          (ciphering_key_sequence_number,request_properties) ;

  if (auth_request_storage_ptr != NULL)
  {
    /* Save incoming MM/GMM Authentication Request */
    reference_number = 
                      auth_request_storage_ptr->auth_request_reference_num ;

    MSG_HIGH_DS_1(MM_SUB, "=MM= Auth_Debug: Starting authentiction for auth_request_reference_num = %d",reference_number);

    /* Start Authentication */
    switch (mm_sim_card_mode)
    {
      case MMGSDI_APP_SIM:
      case MMGSDI_APP_USIM:
        auth_start_authentication(mm_sim_card_mode,reference_number,
                p_rand,autn,&auth_request_storage_ptr->auth_request_properties) ;
        break ;
      default:
        MSG_ERROR_DS(MM_SUB, "=MM= Illegal SIM card mode (%d)",mm_sim_card_mode,0,0);
        return (client_id)(NULL) ;
    }

    id = (client_id)(request_properties->client_response_callback) ;
  }

  /* return Client ID */
  return id ;
} /* end auth_authentication_request() */

/*===========================================================================

FUNCTION      AUTH_FIND_CLIENT_RECORD

DESCRIPTION
  This function looking for a saved Authentication Request by the
  Authentication Request Reference number (client_reference)

DEPENDENCIES
  None

RETURN VALUE
  The function returns a pointer to the location of the found request or NULL
  pointer otherwise

SIDE EFFECTS
  None

===========================================================================*/
auth_request_properties_type *auth_find_client_record
(
  /* pointer to the current request storage */
  auth_request_storage_type *storage_ptr,

  /* Authentication Request Reference number */
  dword client_reference
)
{
  int search_index ;

  for ( search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (storage_ptr[search_index].
                            auth_request_reference_num == client_reference)
    {
      /* A wanted request is found */
      return &storage_ptr[search_index].auth_request_properties ;
    }
  }

  /* A wanted request is not found */
  return NULL ;
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  The function returns a pointer to the location of the found request or NULL
  pointer otherwise

SIDE EFFECTS
  None

===========================================================================*/
auth_request_storage_type *auth_find_client_record_by_callback
(
  /* pointer to the current request storage */
  auth_request_storage_type *storage_ptr,

  /* Authentication Request's client callback pointer */
  auth_response_callback_type cur_callback
)
{
  int search_index ;

  for ( search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (storage_ptr[search_index].
             auth_request_properties.client_response_callback == cur_callback)
    {
      /* A wanted request is found */
      return &storage_ptr[search_index] ;
    }
  }

  /* A wanted request is not found */
  return NULL ;
} /* end auth_find_client_record_by_callback() */

/*===========================================================================

FUNCTION      AUTH_FIND_CALLBACK_PTR

DESCRIPTION
  This function searches a call back function pointer by the Authentication
  Request Reference number (<client_reference>) in order to send to the MM or
  GMM client GSDI Authentication Response info (RES and/or AUTS)

DEPENDENCIES
  None

RETURN VALUE
  The function returns a pointer to the client's call back function pointer

SIDE EFFECTS
  None

===========================================================================*/
auth_response_callback_type auth_find_callback_ptr
(
  /* Authentication Request Reference number */
  dword client_reference
)
{
  byte search_index ;

  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (auth_suspended_cmds_buff[search_index].
                             auth_request_reference_num==client_reference)
    {
      auth_response_callback_type auth_response_callback =
           auth_suspended_cmds_buff[search_index].
                           auth_request_properties.client_response_callback ;

      return auth_response_callback ;
    }
  }

  MSG_ERROR_DS(MM_SUB, "=MM= Existing client references don't match auth response",0,0,0) ;
  return NULL ;
}

/*===========================================================================

FUNCTION      AUTH_STOP_RETX_TIMERS

DESCRIPTION
  This functions stops all running retx timers according to MM/GMM
  Authenticaction client definition. The rext timers are located in client's
  timer list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_stop_retx_timers
(
  /* Timer list length */
  byte timer_list_length,

  /* Timer list */
  auth_timer_list_type *retx_timer_record
)
{
  byte                  timer_index ;
  auth_timer_list_type *timer_ptr_local = retx_timer_record ;

  for (timer_index=0; timer_index<timer_list_length; timer_index++)
  {
    if (mm_timer_status[timer_ptr_local[timer_index].timer] == TIMER_ACTIVE)
    {
      mm_stop_timer(timer_ptr_local[timer_index].timer);

      timer_ptr_local[timer_index].timer_was_active = TRUE ;
    }
  }
}


/*===========================================================================

FUNCTION      AUTH_RESTART_RETX_TIMERS

DESCRIPTION
  This function re-starts the timers that has been stoped before according to
  MM/GMM Authenticaction client definition. The rext timers are located in
  client's timer list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_restart_retx_timers
(
  /* Timer list length */
  byte timer_list_length,

  /* Timer list */
  auth_timer_list_type *retx_timer_record
)
{
  byte                  timer_index ;
  auth_timer_list_type *timer_ptr_local = retx_timer_record ;

  for (timer_index=0; timer_index<timer_list_length; timer_index++)
  {
    if (timer_ptr_local[timer_index].timer_was_active == TRUE)
    {
      mm_start_timer(timer_ptr_local[timer_index].timer,DEFAULT_TIMEOUT);

      timer_ptr_local[timer_index].timer_was_active = FALSE ;
    }
  }
}

/*===========================================================================

FUNCTION      AUTH_STOP_ERROR_TIMERS

DESCRIPTION
  This function stops runnung MM or GMM client timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_stop_error_timers
(
  /* Clients timer record */
  auth_timer_set_type *timer_record
)
{
  if (mm_timer_status[timer_record->mak_failure_timer] == TIMER_ACTIVE)
  {
    mm_stop_timer(timer_record->mak_failure_timer);
  }

  if (mm_timer_status[timer_record->sqn_failure_timer] == TIMER_ACTIVE)
  {
    mm_stop_timer(timer_record->sqn_failure_timer);
  }

} /* end auth_stop_error_timers() */

/*===========================================================================

FUNCTION      AUTH_RESET_AUTH_ATTEMPT

DESCRIPTION
  This function restarts all currently running MM or GMM rext timers and
  treats the cell where the AUTHENTICATION or AUTHENTICATION & CIPHERING
  message was received as barred

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_reset_auth_attempt
(
  auth_request_properties_type *request_properties_ptr
)
{

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
#endif // FEATURE_LTE

  /* Get pointer to MM or GMM timer record */
  auth_timer_set_type *timer_record = &(request_properties_ptr->client_timer_set);
       
  /* Stop running error handling timer */
  auth_stop_error_timers(timer_record) ;
#ifdef FEATURE_WCDMA  
  if (is_umts_mode())
  {     
     rrc_cmd_type *rrc_cmd_p = NULL;

     if (mm_cnm_is_active)
     {
        mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
     }

     if(mm_state == MM_LOCATION_UPDATE_INITIATED)
     {
       mm_state_control( MM_STD_LU_REJECT );
     }
     else 
     {
       mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
     }

     rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

     if (rrc_cmd_p == NULL)
     {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
     }
     rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
     rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = FALSE;
     rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = TRUE;
     rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;     
   
     rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
     MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RRC_ABORT_REQ");
     send_cmd_to_rrc(rrc_cmd_p, mm_serving_plmn.info.active_rat );

     /* Send REG_CNF if REG REQ typ is MMR_REG_REQ_CSG_SELECTION_APP */
#ifdef FEATURE_FEMTO_CSG
     if(mm_reg_waiting_for_reg_cnf && 
        (mmcoord_pending_reg_message.cmd.mmr_reg_req.type == MMR_REG_REQ_CSG_SELECTION_APP))         
     {
        mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
        mmcoord_mmr_reg_cnf.service_state.service_status = SYS_SRV_STATUS_LIMITED;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;
     }
#endif 
  }
#if (defined(FEATURE_GSM) || defined(FEATURE_LTE))   
  else 
#endif /*(defined(FEATURE_GSM) || defined(FEATURE_LTE))*/    
#endif /*FEATURE_WCDMA*/         
#ifdef FEATURE_GSM  
  if (is_gsm_mode())
  {
     if (request_properties_ptr->cn_domain_identity == RRC_CS_DOMAIN_CN_ID)
     {
        if (mm_cnm_is_active)
        {
           mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
        }
        if(mm_state == MM_LOCATION_UPDATE_INITIATED)
        {
          mm_state_control( MM_STD_LU_REJECT );
        }
        else 
        {
          mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
        }
                
        MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_ABORT_REQ");
        mm_send_rr_abort_req(TRUE);

     }
     else if(request_properties_ptr->cn_domain_identity == RRC_PS_DOMAIN_CN_ID)   
     {
        if (((gmm_substate != GMM_SUSPENDED) && (!gmm_is_suspended)) ||
            (timer_record->num_failures > 2)        
#ifdef FEATURE_GSM_DTM
             || (mm_serving_plmn.dtm_supported == TRUE)            
#endif
           )
        {           
           rr_gmm_gprs_auth_fail_ind_T    rr_gmm_gprs_auth_fail_ind;
        
           rr_gmm_gprs_auth_fail_ind.message_header.message_set = MS_MM_RR;
#ifdef FEATURE_GSM_GPRS
           rr_gmm_gprs_auth_fail_ind.message_header.message_id  = (int) RR_GMM_GPRS_AUTH_FAIL_IND;
#endif /*FEATURE_GSM_GPRS*/
     
           rr_gmm_gprs_auth_fail_ind.t3302_timer_value = gmm_t3302_timer_value;
           rr_gmm_gprs_auth_fail_ind.arfcn = mm_auth_failed_arfcn;
           rr_gmm_gprs_auth_fail_ind.bsic = mm_auth_failed_bsic;

           PUT_IMH_LEN( sizeof( rr_gmm_gprs_auth_fail_ind ) - sizeof(IMH_T),
                        &rr_gmm_gprs_auth_fail_ind.message_header);
     
           MSG_HIGH_DS_1( MM_SUB, "=MM= MM sent RR_GMM_GPRS_AUTH_FAIL_IND. T3302: %d", gmm_t3302_timer_value);
           MSG_HIGH_DS_3(MM_SUB,"=MM= camped ARFCN NUM is %d, Band is %d and BSIC is %d", 
                        mm_auth_failed_arfcn.num,mm_auth_failed_arfcn.band,mm_auth_failed_bsic);
           mm_send_message( (IMH_T *)&rr_gmm_gprs_auth_fail_ind, GS_QUEUE_RR ); 
        }
        
        if (((timer_record->num_failures > 2)
#ifdef FEATURE_GSM_DTM
             || (mm_serving_plmn.dtm_supported == TRUE)            
#endif
            ) &&
            (mm_state != MM_IDLE))
        {
           if (mm_cnm_is_active)
           {
              mm_send_mmcnm_rel_ind( OTHER_CAUSE, FALSE );
           }
           mm_state_control( MM_STD_RR_CONNECTION_ABORTED );
                   
           MSG_HIGH_DS_0( MM_SUB, "=MM= MM sent RR_ABORT_REQ");
           mm_send_rr_abort_req(TRUE);
        
        }            
     }     
  }
#ifdef FEATURE_LTE
  else
#endif /*#ifdef FEATURE_LTE*/    
#endif /*FEATURE_GSM*/   
#ifdef FEATURE_LTE
  if(is_lte_mode())
  {
     /* Abort any of the EMM signalling procedure and then move to deregistered state*/
     /* cell must be barred when the auth attempt is reset */
  
     /* temporary method to get the global ctrl data of EMM */
     emm_ctrl_data_ptr = emm_db_get_ctrl_data();
  
#ifdef FEATURE_LTE_REL9  
     if(emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE)
#endif
     {
     emm_rrc_abort_connection(emm_ctrl_data_ptr, TRUE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL);
     MSG_HIGH_DS_0(MM_SUB,"=EMM= RRC Abort Connection, not moving to Deregistered state ") ;
     }
#ifdef FEATURE_LTE_REL9  
     else
     {
       MSG_HIGH_DS_1(MM_SUB,"=EMM= Do not abort conn and bar the cell - EMC SRV Status %d",
                emm_ctrl_data_ptr->emc_srv_status);
       emm_send_deact_non_emc_bearer_ind(emm_ctrl_data_ptr);
     }
#endif
   }

#endif

  timer_record->num_failures = 0;  

  /* According to TS 24.008 4.3.2.6.1 & 4.7.7.6, the GMM or MM shall re-start
  ** all previously running timers
  */
  auth_restart_retx_timers(
                timer_record->timer_list_length,timer_record->timer_list) ;

  /* According to TS 24.008 4.3.2.6.1 & 4.7.7.6, if the MS deems that the
  ** network has failed the authentication check, then the it shall treat
  ** the cell where the AUTHENTICATION REQUEST or AUTHENTICATION &
  ** CIPHERING REQUEST message was received as barred
  */

} /* end auth_reset_auth_attempt() */

/*===========================================================================

FUNCTION        AUTH_ERROR_HANDLER

DESCRIPTION
  This function performs all actions are required for the Authentication
  procedure when the MS receives invalid AUTHENTICATION or AUTHENTICATION
  & CIPHERING REQUEST message

DEPENDENCIES
  None

RETURN VALUE
  The function returns the pointer to the current authentication attempt
  properties

SIDE EFFECTS
  None

===========================================================================*/
auth_response_callback_type auth_error_handler
(
  /* Authentication error code */
  auth_error_type auth_error,

  /* Authentication request's client callback pointer */
  auth_response_callback_type cur_callback,

  /* Authentication status code, output parameter */
  auth_status_type *status
)
{
  auth_timer_set_type *timer_record ;

  auth_request_properties_type *request_properties_ptr ;

  /* Find the clients record into a storage */
  auth_request_storage_type *auth_request_storage_ptr =
    auth_find_client_record_by_callback(auth_request_storage,cur_callback) ;

  boolean fail_indication = FALSE;

  if (auth_request_storage_ptr == NULL)       
  {
    /* If request does not exist in the storage put it there */
    request_properties_ptr = auth_put_request_into_storage(cur_callback) ;

  }else 
  {
    request_properties_ptr =
                        &auth_request_storage_ptr->auth_request_properties ;
  }
  
  if (request_properties_ptr != NULL)
  {
      timer_record = &request_properties_ptr->client_timer_set ;
      if (timer_record->num_failures == 0)
      {
        int i=0;
          for (i=0; i<timer_record->timer_list_length;i++)
          {
              timer_record->timer_list[i].timer_was_active = FALSE;
          }
      }
      /* According to authentication error code, the procedure shall start
      ** appropriate timer
      */
      switch (auth_error)
      {
        case AUTH_MAK_CODE_FAILURE:
        case AUTH_AUTHENTICATION_UNACCEPTABLE:
#ifdef FEATURE_LTE
        case AUTH_NON_EPS_ACCESS:
#endif  // FEATURE_LTE
          /* increment the num_failures */
          timer_record->num_failures++;
          if (timer_record->num_failures > 2)
          {
             fail_indication = TRUE;
          } else {
             mm_start_timer(timer_record->mak_failure_timer, DEFAULT_TIMEOUT);
             mm_auth_failed_arfcn = mm_serving_plmn.arfcn;
             mm_auth_failed_bsic = mm_serving_plmn.bsic;
          }
          break ;
        case AUTH_SQN_FAILURE:
          /* increment the num_failures */
          timer_record->num_failures++;
          if (timer_record->num_failures > 2)
          {
             fail_indication = TRUE;
          } else {
             mm_start_timer(timer_record->sqn_failure_timer, DEFAULT_TIMEOUT);
             mm_auth_failed_arfcn = mm_serving_plmn.arfcn;
             mm_auth_failed_bsic = mm_serving_plmn.bsic;
          }
          break ;
        default:
          MSG_FATAL_DS(MM_SUB, "=MM= Illegal SIM/USIM error code returned",0,0,0);
      }
      if (fail_indication == FALSE)
      {
         /* Stop retx timers if they are running */
         auth_stop_retx_timers(
                    timer_record->timer_list_length,timer_record->timer_list) ;

         /* Get our response callback pointer */
         cur_callback = request_properties_ptr->client_response_callback ;

         /* return authentication status attempt code */
         *status = AUTHENTICATION_IN_PROGRESS ;
      }
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to save intermediate auth results") ;
    return cur_callback ;
  }
  
  if (fail_indication == TRUE)
  {   
    /* Get our response callback pointer */
    cur_callback = request_properties_ptr->client_response_callback ;

    /* According to TS 24.008 4.3.2.6.1 & 4.7.7.6, the MM/GMM shall stop any
    ** of authentication error handling timer if they are running when the MS
    ** receives current invalid AUTHENTICATION or AUTHENTICATION & CIPHERING
    ** REQUEST messages
    */

#ifdef FEATURE_FEMTO_CSG
    /* If auth error is AUTH_AUTHENTICATION_UNACCEPTABLE then add to rejected CSG list */
    if(auth_error == AUTH_AUTHENTICATION_UNACCEPTABLE)
    {
      mm_add_csg_to_rejected_list(mm_serving_plmn.info.plmn,
                                  mm_serving_plmn.info.csg_info.csg_id,
                                  AUTH_AUTHENTICATION_UNACCEPTABLE);
    }
#endif

    /* Reset current authentication attempt */
    auth_reset_auth_attempt(request_properties_ptr) ;

    if(auth_request_storage_ptr!=NULL)
    {
      memset(auth_request_storage_ptr,0x00,sizeof(auth_request_storage_type)) ;
    }
    else
    {
      MSG_FATAL_DS(MM_SUB,"Inavlid memory location ",0,0,0);
    }

    /* return authentication status attempt code */
    *status = AUTHENTICATION_NETWORK_FAILURE ;

  }

  return cur_callback ;
}


/*===========================================================================

FUNCTION      AUTH_FILTER_AUTH_RESPONSE

DESCRIPTION
  This function filters the GSDI authentication responses by authentication
  response reference number

DEPENDENCIES
  None

RETURN VALUE
  The function returns TRUE if the GSDI authentication response shall be
  processed by the Authentication server and FALSE when this response can
  be discarded

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_filter_auth_response
(
  /* Incoming GSDI Authentication response */
  mm_cmd_type *message,

  /* Wanted authentication response reference number */
  dword        *client_reference
)
{
  byte search_index ;

  *client_reference = message->cmd.gsdi_cnf.message_header.client_ref ;
   MSG_HIGH_DS_1(MM_SUB, "=MM= Auth_Debug: GSDI response for client_reference = %d",*client_reference);

  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (auth_suspended_cmds_buff[search_index].auth_request_reference_num !=0)
    {
      MSG_HIGH_DS_2(MM_SUB, "=MM= \nclient_reference: %d, auth_suspended_cmds_buff: %d",*client_reference,
                     auth_suspended_cmds_buff[search_index].auth_request_reference_num);
      if (auth_suspended_cmds_buff[search_index].
                            auth_request_reference_num==*client_reference)
      {
        return TRUE ;
      }
    }
  }

  return FALSE ;
} /* end auth_filter_auth_response() */


/*===========================================================================

FUNCTION      AUTH_PARSE_SIM_RESPONSE

DESCRIPTION
  This function performs the processing of the GSDI authentication response
  when the SIM card is running in GSM mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#define MIN_GSM_GSM_SIM_RESPONSE_LENGTH ( 4 + /* SRES */    \
                                          8   /* GSM Kc */  )

#define MIN_GSM_UMTS_SIM_RESPONSE_LENGTH ( 1 + /* SRES length   */    \
                                           4 + /* SRES */             \
                                           1 + /* GSM Kc length */    \
                                           8   /* GSM Kc */           )

auth_error_type auth_parse_sim_response
(
  /* Incoming GSDI Authentication response */
  mm_cmd_type                 *message,


  /*
  ** OUTPUT PARAMETERS
  */

  /* Pointer to the RES value */
  auth_res_type               *res,

  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey
)
{
  byte total_msg_length;

  switch (mm_sim_card_mode)
  {
    case MMGSDI_APP_SIM:
      /* Read total length of the receivde SIM response */
      total_msg_length = message->cmd.gsdi_cnf.run_gsm_alg_cnf.returned_data_len ;

      if (total_msg_length >= MIN_GSM_GSM_SIM_RESPONSE_LENGTH)
      {
        /* Read Authentication RES value */
        res->value_length = 4 ;
        (void)memscpy((void *)res->value_data, MAX_AUTH_DATA_LENGTH,
                    (void *)&message->cmd.gsdi_cnf.run_gsm_alg_cnf.data[0],res->value_length);

        /* Read Authentication Ciphering Key value */
        gsm_cipherkey->value_length = 8 ;
        (void)memscpy((void *)gsm_cipherkey->value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&message->cmd.gsdi_cnf.run_gsm_alg_cnf.data[4],
                                                 gsm_cipherkey->value_length);

      }
      else
      {
        return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
      }
      break ;
    case MMGSDI_APP_USIM:
      /* Read total length of the receivde SIM response */
      total_msg_length = message->cmd.gsdi_cnf.usim_auth_cnf.returned_data_len ;

      if (total_msg_length >= MIN_GSM_UMTS_SIM_RESPONSE_LENGTH)
      {
        /* Read Authentication RES value */
        res->value_length = message->cmd.gsdi_cnf.usim_auth_cnf.data[0] ;

        /*
        ** The GSM SRES' length has to be equal to 4 according to
        ** TS 31.102 7.1.2
        */
        if (res->value_length != 4)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Invalid GSM SRES length (%d)",res->value_length,0,0) ;
          return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
        }
        (void)memscpy((void *)res->value_data,MAX_AUTH_DATA_LENGTH,
                    (void *)&message->cmd.gsdi_cnf.usim_auth_cnf.data[1],res->value_length);

        /* Read Authentication Ciphering Key value */
        gsm_cipherkey->value_length =
                             message->cmd.gsdi_cnf.usim_auth_cnf.data[res->value_length+1] ;

        /*
        ** The GSM Kc length has to be equal to 8 according to
        ** TS 31.102 7.1.2
        */
        if (gsm_cipherkey->value_length != GSM_SECURITY_KEY_LENGTH)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Invalid GSM Kc length (%d)",gsm_cipherkey->value_length,0,0) ;
          return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
        }

        (void)memscpy((void *)gsm_cipherkey->value_data,MAX_AUTH_DATA_LENGTH,
               (void *)&message->cmd.gsdi_cnf.usim_auth_cnf.data[res->value_length+2],
                                                 gsm_cipherkey->value_length);
      }
      else
      {
        return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
      }
      break ;
    default:
      MSG_ERROR_DS(MM_SUB, "=MM= Illegal SIM card mode (%d)",mm_sim_card_mode,0,0);
      return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
  }

  return AUTH_NO_ERROR ;

} /* end auth_parse_sim_response() */


/*===========================================================================

FUNCTION AUTH_READ_AUTS

DESCRIPTION
  Read the failure identification value (AUTS) from the USIM response

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_read_auts
(
  /* Pointer to GSDI authentication response buffer */
  byte *sim_response,

  /* Pointer to AUTS value */
  auth_auts_type  *gmm_auth_auts
)
{
  if(sim_response[0] <= 16)
  {
    gmm_auth_auts->value_length = sim_response[0] ;
    memscpy((void *)gmm_auth_auts->value_data,MAX_AUTH_DATA_LENGTH,
                                &sim_response[1],gmm_auth_auts->value_length) ;
  }
  else
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Auts must be <= 16 bytes long", 0,0,0);
  }

} /* end gmm_read_auts() */


/*===========================================================================

FUNCTION      AUTH_PARSE_USIM_RESPONSE

DESCRIPTION
  This function parses the GSDI authentication response when the SIM card runs
  in USIM mode

DEPENDENCIES
  None

RETURN VALUE
  The function returns a authentication error code

SIDE EFFECTS
  None

===========================================================================*/
auth_error_type auth_parse_usim_response
(
  /* Incaming USIM authentication response */
  mm_cmd_type                 *message,

  /*
  ** OUTPUT PARAMETERS
  */

  /* Pointer to RES value */
  auth_res_type               *res,

  /* Pointer to GPRS Ciphering key value */
  auth_gprs_cipherkey_type    *gprs_cipherkey,

  /* Pointer to GPRS Integrity key value */
  auth_gprs_integritykey_type *gprs_integritykey,

  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey,

  /* Pointer to failure identification value (AUTS) */
  auth_auts_type              *auts,

  /* Current Authentication Response type */
  mmgsdi_autn_context_enum_type current_auth_request_context
)
{
  if(message->cmd.gsdi_cnf.message_header.gsdi_status == MMGSDI_AUTH_ERROR_GSM_CNTXT_NOT_SUP)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= USIM does not support GSM authentication", 0,0,0);

    return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
  }

  switch (current_auth_request_context)
  {
    /* Parse 3D USIM Authentication Response */

    case MMGSDI_AUTN_USIM_3G_CONTEXT:
      if(!((message->cmd.gsdi_cnf.message_header.gsdi_status == MMGSDI_SUCCESS) ||
         (message->cmd.gsdi_cnf.message_header.gsdi_status == MMGSDI_AUTH_ERROR_INCORRECT_MAC)))
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Failed to get Auth Response from the USIM", 0,0,0);
        return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
      }

      if(message->cmd.gsdi_cnf.message_header.gsdi_status == MMGSDI_AUTH_ERROR_INCORRECT_MAC)
      {
        return AUTH_MAK_CODE_FAILURE ;
      }

      if(message->cmd.gsdi_cnf.usim_auth_cnf.data[0] == AUTH_SYNCH_FAILURE)
      {
        auth_read_auts(&message->cmd.gsdi_cnf.usim_auth_cnf.data[1],auts) ;
        return AUTH_SQN_FAILURE ;
      }

      if(message->cmd.gsdi_cnf.usim_auth_cnf.data[0] == AUTH_3G_UMTS_CONTEXT_SUCCESS)
      {
        byte parsing_index = 1 ;

        /* Copy the RES field into internal GMM database */
        res->value_length =  message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index++] ;

        /*
        ** The 3G RES length has to be less than or equal to 16
        ** according to TS 31.102 7.1.2
        */
        if (res->value_length > 16)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Invalid 3G RES length (%d)",res->value_length,0,0) ;
          return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
        }
        memscpy(res->value_data,MAX_AUTH_DATA_LENGTH,
               &message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index], res->value_length) ;

        parsing_index += res->value_length ;

        /* Copy the GPRS Ciphering Key field into internal GMM database */
        gprs_cipherkey->value_length =
                                  message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index++] ;
        /*
        ** The 3G CK length has to be less than or equal to 16 according to
        ** TS 31.102 7.1.2
        */
        if (gprs_cipherkey->value_length > 16)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Invalid 3G CK length (%d)",gprs_cipherkey->value_length,0,0) ;
          return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
        }
        memscpy(gprs_cipherkey->value_data,MAX_AUTH_DATA_LENGTH,
                    &message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index],
                                                gprs_cipherkey->value_length) ;

        parsing_index += gprs_cipherkey->value_length ;

        /* Copy the GPRS Integrity Key field into internal GMM database */
        gprs_integritykey->value_length =
                                  message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index++] ;
        /*
        ** The 3G IK length has to be less than or equal to 16 according to
        ** TS 31.102 7.1.2
        */
        if (gprs_integritykey->value_length > 16)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Invalid 3G IK length (%d)",gprs_integritykey->value_length,0,0) ;
          return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
        }
        memscpy(gprs_integritykey->value_data,MAX_AUTH_DATA_LENGTH,
                    &message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index],
                                             gprs_integritykey->value_length) ;

        /*
        ** If service no. 27 (GSM access) is disabled the USIM authentication
        ** response does not contain GSM Kc
        */
        if (auth_gsm_access)
        {
          parsing_index += gprs_integritykey->value_length ;

          /* Copy the GSM Ciphering Key field into internal GMM database */
          gsm_cipherkey->value_length =
                                  message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index++] ;
          /*
          ** The GSM Kc length has to be equal to 8 according to
          ** TS 31.102 7.1.2
          */
          if (gsm_cipherkey->value_length != GSM_SECURITY_KEY_LENGTH)
          {
            MSG_ERROR_DS(MM_SUB, "=MM= Invalid GSM Kc length (%d)",gsm_cipherkey->value_length,0,0) ;
            return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
          }
          memscpy(gsm_cipherkey->value_data,MAX_AUTH_DATA_LENGTH,
                      &message->cmd.gsdi_cnf.usim_auth_cnf.data[parsing_index],
                                                 gsm_cipherkey->value_length) ;
        }
      }
      else  /* case of gsm security context */
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Failed to get 3D Auth Response from the USIM: %d", 
            message->cmd.gsdi_cnf.usim_auth_cnf.data[0],0,0);
        return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
      }
      break ;
    /* Parse GSM USIM Authentication Response */
    case MMGSDI_AUTN_USIM_GSM_CONTEXT:
      if (message->cmd.gsdi_cnf.message_header.gsdi_status == MMGSDI_SUCCESS)
      {
        return auth_parse_sim_response(message,res,gsm_cipherkey) ;
      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Failed to get GSM Auth Response from the USIM", 0,0,0);

        return AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
      }
    default :
      MSG_FATAL_DS(MM_SUB, "=MM= Illegal Security Context ID card mode (%d)",
                                            (int)current_auth_request_context,0,0) ;
  }


  return AUTH_NO_ERROR ;
} /* end auth_parse_usim_response() */

/*===========================================================================

FUNCTION      AUTH_GENERATE_CK

DESCRIPTION
  This function derives the CK from the current Kc according to the c4
  conversion function which is described in TS 33.102

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_generate_ck
(
  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey,

  /* Pointer to GPRS Ciphering key value */
  auth_gprs_cipherkey_type    *gprs_cipherkey
)
{
  gprs_cipherkey->value_length = UMTS_SECURITY_KEY_LENGTH ;

  memscpy((void *)gprs_cipherkey->value_data,MAX_AUTH_DATA_LENGTH,
                   (void *)gsm_cipherkey->value_data,GSM_SECURITY_KEY_LENGTH) ;

  memscpy((void *)(&gprs_cipherkey->value_data[GSM_SECURITY_KEY_LENGTH]), MAX_AUTH_DATA_LENGTH-GSM_SECURITY_KEY_LENGTH,
                   (void *)gsm_cipherkey->value_data,GSM_SECURITY_KEY_LENGTH) ;
} /* end auth_generate_ck() */

/*===========================================================================

FUNCTION      AUTH_GENERATE_IK

DESCRIPTION
  This function derives the IK from the current Kc according to the c5
  conversion function which is described in TS 33.102

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#define C5_KCi_LENGTH 4 /* 4 bytes according to TS 33.102 */
void auth_generate_ik
(
  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey,

  /* Pointer to GPRS Integrity key value */
  auth_gprs_integritykey_type *gprs_integritykey
)
{
  dword   xored_Kc1_Kc2      ;
  byte    Kc1[C5_KCi_LENGTH] ;
  byte    Kc2[C5_KCi_LENGTH] ;

  memscpy((void *)Kc1,C5_KCi_LENGTH,(void *)gsm_cipherkey->value_data,C5_KCi_LENGTH) ;
  memscpy((void *)Kc2,C5_KCi_LENGTH,
           (void *)(&gsm_cipherkey->value_data[C5_KCi_LENGTH]),C5_KCi_LENGTH) ;

  xored_Kc1_Kc2 = (*(dword *)Kc1) ^ (*(dword *)Kc2);

  gprs_integritykey->value_length = UMTS_SECURITY_KEY_LENGTH ;

  memscpy((void *)gprs_integritykey->value_data,MAX_AUTH_DATA_LENGTH,
                                      (void *)(&xored_Kc1_Kc2),C5_KCi_LENGTH) ;

  memscpy((void *)(&gprs_integritykey->value_data[C5_KCi_LENGTH]),MAX_AUTH_DATA_LENGTH-C5_KCi_LENGTH,
                   (void *)gsm_cipherkey->value_data,GSM_SECURITY_KEY_LENGTH) ;

  memscpy((void *)
    (&gprs_integritykey->value_data[GSM_SECURITY_KEY_LENGTH+C5_KCi_LENGTH]),MAX_AUTH_DATA_LENGTH-(GSM_SECURITY_KEY_LENGTH+C5_KCi_LENGTH),
                                      (void *)(&xored_Kc1_Kc2),C5_KCi_LENGTH) ;
} /* end auth_generate_ik() */

/*===========================================================================

FUNCTION      AUTH_XOR_QWORD

DESCRIPTION
  This function XORs two incoming qwords (64 bits) and puts a result into 
  first qword

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_xor_qword
(
  byte *first_qword_ptr,
  byte *second_qword_ptr
)
{
  byte index ;

  for (index=0; index<GSM_SECURITY_KEY_LENGTH; index++)
  {
    first_qword_ptr[index] ^= second_qword_ptr[index] ;
  }
} /* end auth_xor_qword() */

/*===========================================================================

FUNCTION      AUTH_GENERATE_KC

DESCRIPTION
  This function converts UMTS CK & IK into Kc according to the c3 conversion 
  function which is described in TS 33.102

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_generate_kc
(
  /* Pointer to GPRS Ciphering key value */
  auth_gprs_cipherkey_type    *gprs_cipherkey,

  /* Pointer to GPRS Integrity key value */
  auth_gprs_integritykey_type *gprs_integritykey,

  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey,
  /* Pointer to the GSM Ciphering Key value kc_128*/
  auth_gsm_cipherkey_type     *gsm_kc_128bit_in 
)
{
  secapi_handle_type hsh_handler = NULL;
  errno_enum_type err = E_FAILURE;
  byte ck2[GSM_SECURITY_KEY_LENGTH] ;
  byte ik1[GSM_SECURITY_KEY_LENGTH] ;
  byte ik2[GSM_SECURITY_KEY_LENGTH] ;
  byte ckik[UMTS_GENERATOR_KEY_LEN];
  byte key_out[UMTS_GENERATOR_KEY_LEN];
  byte input_str[1];
  byte input_len;

  memscpy((void *)gsm_cipherkey->value_data,MAX_AUTH_DATA_LENGTH,
                  (void *)gprs_cipherkey->value_data,GSM_SECURITY_KEY_LENGTH) ;
  memscpy((void *)ck2, GSM_SECURITY_KEY_LENGTH, (void *)(&gprs_cipherkey->
                value_data[GSM_SECURITY_KEY_LENGTH]),GSM_SECURITY_KEY_LENGTH) ;

  memscpy((void *)ik1,GSM_SECURITY_KEY_LENGTH,
               (void *)gprs_integritykey->value_data,GSM_SECURITY_KEY_LENGTH) ;
  memscpy((void *)ik2,GSM_SECURITY_KEY_LENGTH,(void *)(&gprs_integritykey->
                value_data[GSM_SECURITY_KEY_LENGTH]),GSM_SECURITY_KEY_LENGTH) ;

  auth_xor_qword(gsm_cipherkey->value_data,ck2) ;
  auth_xor_qword(gsm_cipherkey->value_data,ik1) ;
  auth_xor_qword(gsm_cipherkey->value_data,ik2) ;

  gsm_cipherkey->value_length = GSM_SECURITY_KEY_LENGTH ;


  /* calculation of 128 bit KC */
  
  /* Step 1 do CK || IK */
  memscpy((void *)ckik,UMTS_SECURITY_KEY_LENGTH,
                  (void *)gprs_cipherkey->value_data,MAX_AUTH_DATA_LENGTH) ;
  memscpy((void *)&ckik[UMTS_SECURITY_KEY_LENGTH],MAX_AUTH_DATA_LENGTH,
               (void *)gprs_integritykey->value_data,MAX_AUTH_DATA_LENGTH) ;

  /* Step 2 FC = 0x32 and input len  */
  input_str[0] = 0x32;  
  input_len    = 1;
  
  /*Step 3  Create sec handle and call sec api to generate KC*/
   /* creating handle for sec api SHA 256.*/
  err = secapi_new(&hsh_handler,SECAPI_SHA256);
  ASSERT(E_SUCCESS==err);
  /* Generating key through API hsh_create_hmac */
  err = secapi_hsh_create_hmac(hsh_handler,
                               SECAPI_SHA256,
                               input_str,
                               input_len,
                               (uint8 *)ckik,
                               UMTS_GENERATOR_KEY_LEN,
                               (uint8 *)key_out);
  ASSERT(E_SUCCESS==err);
  /* delete the hsh handler */
  err = secapi_delete(&hsh_handler);
  ASSERT(E_SUCCESS==err);
  
  /* Step 4 copy lower 128 bit of key , this is 128 bit KC */
  memscpy((void *)gsm_kc_128bit_in->value_data,UMTS_SECURITY_KEY_LENGTH,
              (void *)&key_out[0],UMTS_SECURITY_KEY_LENGTH) ;
  
  /* Step 5 set the key value length of kc_128 */
  gsm_kc_128bit_in->value_length = UMTS_SECURITY_KEY_LENGTH;  

} /* end auth_generate_kc() */

/*===========================================================================

FUNCTION      AUTH_PARSE_SIM_AUTHENTICATION_RESPONSE

DESCRIPTION
  This function performs a parsing of the GSDI task authentication response
  according to currently using SIM card mode (SIM/USIM)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The function can treat the GSDI authentication response as obsolete and
  discard it.

===========================================================================*/
auth_error_type auth_parse_sim_authentication_response
(
  /* Incaming USIM authentication response */
  mm_cmd_type                 *message,

  /* Wanted Authentication response reference number */
  dword                       *client_reference,

  /*
  ** OUTPUT PARAMETERS
  */

  /* Pointer to RES value */
  auth_res_type               *res,

  /* Pointer to GPRS Ciphering key value */
  auth_gprs_cipherkey_type    *gprs_cipherkey,

  /* Pointer to GPRS Integrity key value */
  auth_gprs_integritykey_type *gprs_integritykey,

  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey,
  /*pointer to GSM 128 bit kc */
  auth_gsm_cipherkey_type     *gsm_kc_128bit_in,
  /* Pointer to failure identification value (AUTS) */
  auth_auts_type              *auts,

  /* Pointer to current CKSN */
  byte                        *cur_cksn,

  /* Pointer to current domain identity */
  rrc_cn_domain_identity_e_type *domain_identity,

  /* Pointer to requested ciphering algorithm */
  byte                        *ciphering_algorithm,

  byte                        *cache_guard_timer_id,

  mmgsdi_autn_context_enum_type *current_authentication_context
)
{
  auth_request_properties_type *auth_request_properties ;
  auth_error_type parsing_result = AUTH_NO_ERROR ;

  res->value_length = 0 ;
  gprs_cipherkey->value_length = 0 ;
  gprs_integritykey->value_length = 0 ;
  gsm_cipherkey->value_length = 0 ;
  auts->value_length = 0 ;

  if (auth_filter_auth_response(message,client_reference) == FALSE)
  {
    /* Discard the response */
    return AUTH_SKIP_AUTHENTICATION_RESPONSE ;
  }

  auth_request_properties =
          auth_find_client_record(auth_suspended_cmds_buff,*client_reference) ;

  if(auth_request_properties != NULL)
  {

    /* Get stored CKSN */
    *cur_cksn = auth_request_properties->ciphering_key_sequence_number ;

    *domain_identity = auth_request_properties->cn_domain_identity ;

    *ciphering_algorithm = auth_request_properties->ciphering_algorithm ;

    *cache_guard_timer_id = auth_request_properties->cache_guard_timer_id ;

    *current_authentication_context =
                          auth_request_properties->current_auth_request_context ;

  }
  else
  {
    /* Discard the response */
    return AUTH_SKIP_AUTHENTICATION_RESPONSE ;
  }


  /* Parse SIM response according to the current SIM card mode */
  switch (mm_sim_card_mode)
  {
    case MMGSDI_APP_SIM:
      parsing_result = auth_parse_sim_response(message,res,gsm_cipherkey) ;
      break ;
    case MMGSDI_APP_USIM:
      parsing_result = auth_parse_usim_response(message,res,
                  gprs_cipherkey,gprs_integritykey,
                           gsm_cipherkey,auts,*current_authentication_context) ;
      break ;
    default:
      MSG_ERROR_DS(MM_SUB, "=MM= Illegal SIM card mode (%d)",mm_sim_card_mode,0,0);
      parsing_result = AUTH_CAN_NOT_PERFORM_AUTHENTICATION ;
      break ;
  }

  if (parsing_result == AUTH_NO_ERROR)
  {
    if (gprs_cipherkey->value_length == 0)
    {
      auth_generate_ck(gsm_cipherkey,gprs_cipherkey) ;
    }

    if (gprs_integritykey->value_length == 0)
    {
      auth_generate_ik(gsm_cipherkey,gprs_integritykey) ;
    }

    if (gsm_cipherkey->value_length == 0)
    {
      auth_generate_kc(gprs_cipherkey,gprs_integritykey,gsm_cipherkey,
                                                   gsm_kc_128bit_in) ;
    }
    else
    {
      if((gprs_integritykey->value_length!=0)&&(gprs_cipherkey->value_length!=0))
      {
        auth_gsm_cipherkey_type     temp_gsm_cipherkey;
        /*calculation of kc 128 bit , temp variable used, 
                                   so that passing value won't change*/
        auth_generate_kc(gprs_cipherkey,gprs_integritykey,&temp_gsm_cipherkey,
                                                           gsm_kc_128bit_in) ;
      }
    }
  }

  return parsing_result ;
}


/*===========================================================================

FUNCTION      AUTH_IS_ERROR_HANDLING_ACTIVATED

DESCRIPTION
  This function verifies if there is an previously activated authentication
  error handler for specific MM or GMM authentication cient

DEPENDENCIES
  None

RETURN VALUE
  The function returns the pointer to the specific Authentication request
  properties or NULL pointer if the request properties has not been found

SIDE EFFECTS
  None

===========================================================================*/
auth_request_storage_type *auth_is_error_handling_activated
(
 /* Authentication response reference number */
  dword client_reference
)
{
  byte search_index ;

  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (auth_suspended_cmds_buff[search_index].
                             auth_request_reference_num == client_reference)
    {
      byte comp_index ;

      auth_response_callback_type request_properties =
               auth_suspended_cmds_buff[search_index].
                             auth_request_properties.client_response_callback ;

      for (comp_index=0; comp_index<MAX_REQUEST_STORAGE_SIZE; comp_index++)
      {
        if (auth_request_storage[comp_index].
                    auth_request_properties.
                            client_response_callback == request_properties)
        {
          return &auth_request_storage[comp_index] ;
        }
      }
    }
  }

  return NULL ;

} /* end auth_is_error_handling_activated() */


/*===========================================================================

FUNCTION      AUTH_CANCEL_ERROR_HANDLING

DESCRIPTION
  This function stops Authentication Client's error handling timers if they are
  running and restarts previously runniing client's retx timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_cancel_error_handling
(
  /* Pointer to current authentication request */
  auth_request_storage_type *auth_request_info
)
{
  byte timer_list_length ;
  auth_timer_list_type *retx_timer_list ;

  /* Stop MAK failure timer if it is running */
  if (mm_timer_status[auth_request_info->
    auth_request_properties.client_timer_set.mak_failure_timer]==TIMER_ACTIVE)
  {
    mm_stop_timer(auth_request_info->
                auth_request_properties.client_timer_set.mak_failure_timer) ;
  }

  /* Stop SQN failure timer if it is running */
  if (mm_timer_status[auth_request_info->
    auth_request_properties.client_timer_set.sqn_failure_timer]==TIMER_ACTIVE)
  {
    mm_stop_timer(auth_request_info->
                auth_request_properties.client_timer_set.sqn_failure_timer) ;
  }

  timer_list_length = auth_request_info->
                  auth_request_properties.client_timer_set.timer_list_length ;

  retx_timer_list = auth_request_info->
                         auth_request_properties.client_timer_set.timer_list ;

  /* Restart MM or GMM retx timers */
  auth_restart_retx_timers(timer_list_length,retx_timer_list) ;

  /* Clear request properties into a storage */
  memset((void *)auth_request_info,0x00,sizeof(auth_request_storage_type));
} /* end auth_cancel_error_handling() */

/*===========================================================================

FUNCTION      AUTH_GET_GSM_FILENAME

DESCRIPTION
  This function calculates current GSM security context file name according
  to the card mode (SIM or USIM) and current security context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static mmgsdi_file_enum_type auth_get_gsm_filename
(
  rrc_cn_domain_identity_e_type domain_identity
)
{
  mmgsdi_file_enum_type info_name = MMGSDI_USIM_KC ;

  switch (mm_sim_card_mode)
  {
    case MMGSDI_APP_USIM :
      switch (domain_identity)
      {
        case RRC_CS_DOMAIN_CN_ID :
          info_name = MMGSDI_USIM_KC ;
          break ;
        case RRC_PS_DOMAIN_CN_ID :
          info_name = MMGSDI_USIM_KCGPRS ;
          break ;
        default :
          MSG_FATAL_DS(MM_SUB, "=MM= Invalid domain Id=%d",(int) domain_identity,0,0) ;
      }
      break ;
    case MMGSDI_APP_SIM  :
      switch (domain_identity)
      {
        case RRC_CS_DOMAIN_CN_ID :
          info_name = MMGSDI_GSM_KC ;
          break ;
        case RRC_PS_DOMAIN_CN_ID :
          info_name = MMGSDI_GSM_KCGPRS ;
          break ;
        default :
          MSG_FATAL_DS(MM_SUB, "=MM= Invalid domain Id=%d",(int) domain_identity,0,0) ;
      }
      break ;
    default:
      MSG_ERROR_DS(MM_SUB,"Invalid SIM card mode (%d)", (int) mm_sim_card_mode,0,0) ;
      break ;
  }

  return info_name ;
} /* end auth_get_gsm_filename() */


/*===========================================================================

FUNCTION      AUTH_GET_GSM_CONTEXT_AVAILABILITY

DESCRIPTION
  This function returns TRUE if the GSM security context is accessible
  according to the current SIM card (SIM/USIM) and current transaction
  domain identity and FALSE otherwise

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_get_gsm_context_availability
(
  rrc_cn_domain_identity_e_type domain_identity
)
{
  switch (mm_sim_card_mode)
  {
    case MMGSDI_APP_USIM :
      if (!auth_gsm_access)
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Unable to save GSM GPRS Kc due to SIM/USIM config (see SST)",0,0,0) ;
        return FALSE ;
      }
      break ;
    case MMGSDI_APP_SIM  :
      if ((domain_identity == RRC_PS_DOMAIN_CN_ID)&&(!auth_gsm_gprs_keys_present))
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to save GSM GPRS Kc due to SIM/USIM config (see SST)");
        return FALSE ;
      }
      break ;
    default:
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid SIM card mode (%d)",mm_sim_card_mode,0,0) ;
      return FALSE ;
  }

  return TRUE ;
} /* end auth_get_gsm_context_availability() */


/*===========================================================================

FUNCTION      AUTH_SAVE_GSM_SECURITY_CONTEX

DESCRIPTION
  This function saves the GSM security context according to the SIM card mode
  (GSM or UMTS)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_save_gsm_security_context
(
  /* Domain Identity that the current security info are belonging to */
  rrc_cn_domain_identity_e_type domain_identity,

  /* Current Ciphering Key Sequence Number (CKSN) */
  byte                    curr_ciphering_key_sequence_number,

  /* Pointer to the GSM Ciphering Key value */
  auth_gsm_cipherkey_type *gsm_cipherkey
)
{
  byte sim_buffer[9];

  if (gsm_cipherkey->value_length != GSM_SECURITY_KEY_LENGTH)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Invalid GSM ciphering key length %d",gsm_cipherkey->value_length,0,0);
    return FALSE ;
  }

  /* Put the GSM Security context into a buffer */
  memscpy((void *)sim_buffer,9,
                  (void *)gsm_cipherkey->value_data,GSM_SECURITY_KEY_LENGTH) ;

  sim_buffer[8] = curr_ciphering_key_sequence_number ;

  if (auth_get_gsm_context_availability(domain_identity))
  {
    if (mm_sim_write(auth_get_gsm_filename(domain_identity), sim_buffer, 9) != TRUE)
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to save GSM security context",0,0,0) ;
      return FALSE ;
    }
  }
  else
  {
    if (mm_sim_card_mode == MMGSDI_APP_SIM)
    {
      ghdi_status_T nv_status ;

      if ((nv_status = mm_ghdi_nvmem_write(
             NV_GSM_KCGPRS_I,(ghdi_nvmem_data_T *)sim_buffer)) != GHDI_SUCCESS)
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to save GSM security context: %d",nv_status,0,0) ;
        return FALSE ;
      }
    }
  }

  return TRUE ;
} /* end auth_save_gsm_security_context() */


/*===========================================================================

FUNCTION      AUTH_SAVE_UMTS_SECURITY_CONTEX

DESCRIPTION
  This function saves the UMTS security context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_save_umts_security_context
(
  /* Security Key's Domain Identity */
  rrc_cn_domain_identity_e_type domain_identity,

  /* Current Ciphering Key Sequence Number (CKSN) */
  byte                        curr_ciphering_key_sequence_number,

  /* Pointer to the GPRS Ciphering Key value */
  auth_gprs_cipherkey_type    *gprs_cipherkey,

  /* Pointer to the GPRS Integrity Key value */
  auth_gprs_integritykey_type *gprs_integritykey,

  /* Pointer to the Ciphering Key value */
  auth_gsm_cipherkey_type     *gsm_cipherkey

)
{
  byte sim_buffer[40] ;
  byte sim_buffer_index = 0 ;
  mmgsdi_file_enum_type usim_file_name = MMGSDI_USIM_KEYS ;

  /* Clean up memory buffer */
  memset((void *)sim_buffer,0x00,40) ;

  sim_buffer[sim_buffer_index++] =
                            (byte)(curr_ciphering_key_sequence_number & 0x0F) ;

  memscpy((void *)&sim_buffer[sim_buffer_index],40-sim_buffer_index,
            (void *)gprs_cipherkey->value_data, MIN(gprs_cipherkey->value_length,MAX_AUTH_DATA_LENGTH));
  /* The GPRS CK length must be 16 bytes */
  sim_buffer_index += 16 ;

  memscpy((void *)&sim_buffer[sim_buffer_index],40-sim_buffer_index,
      (void *)gprs_integritykey->value_data,MIN(gprs_integritykey->value_length,MAX_AUTH_DATA_LENGTH));
  /* The GPRS IK length must be 16 bytes */
  sim_buffer_index += 16 ;

  switch (domain_identity)
  {
    case RRC_CS_DOMAIN_CN_ID :
        usim_file_name = MMGSDI_USIM_KEYS ;
        break ;
    case RRC_PS_DOMAIN_CN_ID  :
        usim_file_name = MMGSDI_USIM_KEYSPS ;
        break ;
    default:
      MSG_FATAL_DS(MM_SUB, "=MM= Invalid domain identity (%d)", (int)domain_identity,0,0) ;
  }

  if (mm_sim_write( usim_file_name, sim_buffer, sim_buffer_index) != TRUE )
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Unable to save UMTS security context",0,0,0) ;
    return FALSE ;
  }

  if (gsm_cipherkey->value_length == GSM_SECURITY_KEY_LENGTH)
  {
    if (!auth_get_gsm_context_availability(domain_identity))
    {
      /*
      ** Required GSM context is not accesible and the GSM/GSM GPRS
      ** key could not be saved, but because of successful saving of the
      ** UMTS security context the function returns TRUE
      */
      return TRUE ;
    }

    memscpy((void *)sim_buffer,40,(void *)gsm_cipherkey->value_data,8) ;

    sim_buffer[8] = curr_ciphering_key_sequence_number ;

    if (mm_sim_write(auth_get_gsm_filename(domain_identity),sim_buffer, 9) != TRUE )
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to save USIM GSM security context",0,0,0) ;
      return FALSE ;
    }
  }
  else
  {
    gsm_cipherkey->value_length = 0 ;
    MSG_HIGH_DS_0(MM_SUB, "=MM= UMTS security context contains illegal GSM ciphering key");
  }

  return TRUE ;
} /* end auth_save_umts_security_context() */


/*===========================================================================

FUNCTION      AUTH_SAVE_SECURITY_CONTEX

DESCRIPTION
  This function saves a security context that is currently active according
  to the type of active security context and SIM card mode (SIM / USIM)

DEPENDENCIES
  None

RETURN VALUE
  The function returns TRUE if the current security context has been saved
  successfully and FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_save_security_context
(
  byte                          ciphering_key_sequence_number,
  rrc_cn_domain_identity_e_type domain_identity,
  auth_gprs_cipherkey_type      *gprs_cipherkey,
  auth_gprs_integritykey_type   *gprs_integritykey,
  auth_gsm_cipherkey_type       *gsm_cipherkey
)
{
  switch (mm_sim_card_mode)
  {
    case MMGSDI_APP_SIM :
      return auth_save_gsm_security_context(domain_identity,
                           ciphering_key_sequence_number,gsm_cipherkey) ;
    case MMGSDI_APP_USIM:
      if( is_lte_mode() )
      {
        /* Note : Do not save the Ck and Ik for EPS authentication */
        return TRUE;
      } else 
      {
        return auth_save_umts_security_context(
            domain_identity,ciphering_key_sequence_number,
                        gprs_cipherkey,gprs_integritykey,gsm_cipherkey) ;
      }
    default:
      MSG_ERROR_DS(MM_SUB, "=MM= Illegal SIM card mode",0,0,0) ;
      return FALSE ;
  }
}

/*===========================================================================

FUNCTION      AUTH_LOAD_HFNS

DESCRIPTION
  This function loads the CS and PS Hyper Frame Numbers and Hyper Frame
  threshhold from the SIM card.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_load_hfns
(
  /* Output buffer */
  hfn_type *hfn
)
{
  /* Topmost Half nibble is unused in HFN */
  byte hfn_init_buf[3] = {0x00, 0x00, 0x00};
  byte threshold_init_buf[3] = {0xFF, 0xFF, 0xFF};
  
#ifdef FEATURE_NAS_REL11
  mcfg_fs_status_e_type result = MCFG_FS_STATUS_EFS_ERR; 
  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
  hfn_nv_stored_type hfn_nv;
  memset(&hfn_nv, 0x00, sizeof(hfn_nv_stored_type));
#endif
  
  /*
  ** Init Hfns
  */
  memscpy(hfn->cs_hfn,MM_HFN_SIZE,    hfn_init_buf, 3);
  memscpy(hfn->ps_hfn,MM_HFN_SIZE,    hfn_init_buf, 3);
  memscpy(hfn->threshold, MM_HFN_SIZE, threshold_init_buf, 3);

  if((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
    || (mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
    )
  {
    boolean status = FALSE;
    byte start_hfn[6] = {0x0,0x00,0x0,0x00,0x0,0x00};
    byte threshold[3] = {0x0,0x00,0x0};

#ifdef FEATURE_NAS_REL11
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
    if ((!MM_SUB_IS_SGLTE_SUB(MM_AS_ID_1)) &&
        (mm_sub_id == MM_AS_ID_2))
    {
      result = mcfg_fs_read("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap",
                              &hfn_nv,sizeof(hfn_nv_stored_type),MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);
      if (result == MCFG_FS_STATUS_OK)
      {
        (void)mcfg_fs_delete("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id);
      }
    }
    else
#endif
    if (mm_sub_id == MM_AS_ID_1)
    {
      result = mcfg_fs_read("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap",
                              &hfn_nv,sizeof(hfn_nv_stored_type),MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

      if (result == MCFG_FS_STATUS_OK)
      {
        (void)mcfg_fs_delete("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id);
      }
    }
#ifdef FEATURE_TRIPLE_SIM
    else if (mm_sub_id == MM_AS_ID_3)
    {
      result = mcfg_fs_read("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap",
                              &hfn_nv,sizeof(hfn_nv_stored_type),MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

      if (result == MCFG_FS_STATUS_OK)
      {
        (void)mcfg_fs_delete("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id);
      }
    }
#endif

    if ( (result == MCFG_FS_STATUS_OK)&& 
          mm_imsi_is_native &&
         (mm_sim_card_mode == MMGSDI_APP_USIM) &&
         (hfn_nv.hotswap == 0x01))
    {
      memscpy(hfn,sizeof(hfn_type),&hfn_nv.current_hfn,sizeof(hfn_type));
      auth_update_hfns_to_sim_card(FALSE);
      return;
    }
    else
#endif
    if(mm_sim_card_mode == MMGSDI_APP_USIM)     /* Read CS & PS HFNs from the USIM card */
    {
      status = mm_sim_read ( MMGSDI_USIM_START_HFN, start_hfn, 6);

      if(status)
      {
        /* Read Threshold */
        status = mm_sim_read ( MMGSDI_USIM_THRESHOLD, threshold, 3);
      }
    }
#ifdef FEATURE_NAS_REL11
    else if (mm_imsi_is_native)
    {
      efs_status = mcfg_fs_read("/nv/item_files/modem/nas/mm_nas_nv_sim_parameters",
               &mm_nas_nv_sim_hfns,sizeof(mm_nas_nv_sim_parameters_T),MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);

      memscpy(start_hfn,6,mm_nas_nv_sim_hfns.start_hfn,6);
      memscpy(threshold,3,mm_nas_nv_sim_hfns.threshold,3);
    }
#endif

    if(status
#ifdef FEATURE_NAS_REL11
        || (efs_status == MCFG_FS_STATUS_OK)
#endif
    )
    {
      dword tmp ;

      /* Save CS & PS HFNs into internal database */
      memscpy(hfn->cs_hfn, MM_HFN_SIZE, start_hfn,    3);
      hfn->cs_hfn[0] &= 0x0F ;
      memscpy(hfn->ps_hfn, MM_HFN_SIZE, start_hfn+3,  3);
      hfn->ps_hfn[0] &= 0x0F ;

      /* Convert CS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn->cs_hfn[0],4,20) ;
      memset(&hfn->cs_hfn[0],0x00,3) ;
      b_packd(tmp,&hfn->cs_hfn[0],0,20) ;

      /* Convert PS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn->ps_hfn[0],4,20) ;
      memset(&hfn->ps_hfn[0],0x00,3) ;
      b_packd(tmp,&hfn->ps_hfn[0],0,20) ;

      /* Convert HFN Threshold from USIM to internal format */
      tmp = b_unpackd(threshold,4,20) ;
      memset(hfn->threshold,0x00,3) ;
      b_packd(tmp,hfn->threshold,0,20) ;
    }
    else
    {
#ifdef FEATURE_NAS_REL11	
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to read HFNs (GSDI err = %d), NV failure status = %d",status,efs_status,0) ;
#else
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to read HFNs (GSDI err = %d)",status,0,0) ;
#endif
    }
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM to read HFNs") ;
  }
} /* end auth_load_hfns() */

/*===========================================================================

FUNCTION GMM_NOTIFY_LOWER_LAYER

DESCRIPTION
  This function is called when the Authentication procedure has been finished
  successfully, in order to configure ciphering subsystem of the RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_notify_lower_layer
(
  auth_gprs_cipherkey_type    *gmm_cipherkey, /* GMM Ciphering key */
  auth_gprs_integritykey_type *gmm_integritykey,   /* GPRS Integrity key */
  auth_gsm_cipherkey_type     *gsm_cipherkey ,
  rrc_cn_domain_identity_e_type domain_identity,    /* Current domain ID */
  rrc_sim_update_e_type sim_update_type
)
{
#ifdef FEATURE_WCDMA 
 if (is_umts_mode())
  {
    if ((gmm_cipherkey->value_length == UMTS_SECURITY_KEY_LENGTH) &&
                  (gmm_integritykey->value_length == UMTS_SECURITY_KEY_LENGTH))
    {
      /* Notify SIM updates to RRC */
      if (mm_wcdma_supported)
      {
        mm_send_rrc_security_update_req(
          sim_update_type,domain_identity, SYS_RAT_UMTS_RADIO_ACCESS );
      }

      if (mm_tdscdma_supported)
      {
        mm_send_rrc_security_update_req(
          sim_update_type,domain_identity, SYS_RAT_TDS_RADIO_ACCESS );
      }

      /* Notify SIM updates to RR/LLC */
      if (( mm_gsm_supported ) && ( domain_identity == RRC_CS_DOMAIN_CN_ID ))
      {
        mm_send_rr_sim_update_req() ;
      }
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid UMTS security key lengths (CK-%d, IK-%d)",
                gmm_cipherkey->value_length,gmm_integritykey->value_length,0) ;
    }
  }
  else
#endif /*FEATURE_WCDMA*/        
#ifdef FEATURE_GSM
    if (is_gsm_mode())
    {
      if (gsm_cipherkey->value_length == GSM_SECURITY_KEY_LENGTH)
      {
        switch (domain_identity)
        {
          case RRC_CS_DOMAIN_CN_ID :
            /* Update RR layer that Keys have been changed */
            mm_send_rr_sim_update_req() ;
            break ;
          case RRC_PS_DOMAIN_CN_ID :
#ifdef FEATURE_GSM_GPRS
            /* Notify LLC */
            gmmllc_gmm_trigger_llc_event(GMM_TRIGGER_NEW_KC_VALUE,FALSE,0) ;
#else /* FEATURE_GSM_GPRS is not supported */
            MSG_ERROR_DS(MM_SUB, "=MM= Feature GPRS is not supported",0,0,0) ;
#endif /* FEATURE_GSM_GPRS */
            break ;
          default:
            MSG_ERROR_DS(MM_SUB, "=MM= Invalid domain identity (%d)",domain_identity,0,0) ;
            break ;
        } /* switch (domain_identity) */

        /* Notify SIM updates to RRC */
        if ( mm_wcdma_supported )
        {
          mm_send_rrc_security_update_req(
            sim_update_type,domain_identity, SYS_RAT_UMTS_RADIO_ACCESS );
        }

        if (mm_tdscdma_supported)
        {
          mm_send_rrc_security_update_req(
            sim_update_type,domain_identity, SYS_RAT_TDS_RADIO_ACCESS );
        }
      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Invalid GSM ciphering key length (Kc-%d)",
                                             gsm_cipherkey->value_length,0,0) ;
      }
    }    
    else
#endif /*FEATURE_GSM*/       
    {
      MSG_ERROR_DS(MM_SUB,"Unknown current mode (UMTS nor GSM nor LTE)",0,0,0) ;
    }
} /* end auth_notify_lower_layer() */


/*===========================================================================

FUNCTION    GET_CIPHERING_ALGORITHM

DESCRIPTION
  This is an access function that provides an access to the Ciphering Algorithm
  value which currently in use

DEPENDENCIES
  None

RETURN VALUE
  The function returns Ciphering Algorithm value if it is available and 0xFF
  otherwise

SIDE EFFECTS
  None

===========================================================================*/
byte get_ciphering_algorithm
(
  dword client_reference
)
{
  byte search_index ;

  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    if (auth_suspended_cmds_buff[search_index].
                             auth_request_reference_num == client_reference)
    {
      return auth_suspended_cmds_buff[search_index].
                                  auth_request_properties.ciphering_algorithm ;
    }
  }

  MSG_HIGH_DS_0(MM_SUB,"=MM= Unable to identify ciphering algorithm") ;

  return 0xFF ;
} /* end get_ciphering_algorithm() */

/*===========================================================================
FUNCTION      AUTH_UPDATE_SECURITY_KEYS

DESCRIPTION
  This function saves the GSM/UMTS security keys into internal database of
  the Authentication Server

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_update_security_keys
(
  rrc_cn_domain_identity_e_type domain_identity,   /* Current domain ID */
  byte                          cur_cksn,
  byte                          ciphering_algorithm,
  auth_gprs_cipherkey_type      *gprs_cipherkey,
  auth_gsm_cipherkey_type       *gsm_cipherkey,
  auth_gprs_integritykey_type   *gprs_integritykey,
  auth_gsm_cipherkey_type       *gsm_kc_128bit_in
)
{
#if defined (FEATURE_LTE)
  lte_nas_emm_ps_domain_int_cipher_keys emm_ps_domain_int_cipher_log;
#endif
  switch (domain_identity)
  {
    case RRC_CS_DOMAIN_CN_ID :
      if (gprs_cipherkey->value_length != 0)
      {
        /* Save CS GPRS Ciphering Key */
        memscpy((void *)&auth_cs_gprs_cipherkey, sizeof(auth_gprs_cipherkey_type),
                             gprs_cipherkey,sizeof(auth_gprs_cipherkey_type)) ;
      }

      if (gprs_integritykey->value_length != 0)
      {
        /* Save CS GPRS Integrity Key */
        memscpy((void *)&auth_cs_gprs_integritykey, sizeof(auth_gprs_integritykey_type),
                       gprs_integritykey,sizeof(auth_gprs_integritykey_type)) ;
      }

      if ((gprs_cipherkey->value_length != 0) ||
                                       (gprs_integritykey->value_length != 0))
      {
        auth_cs_gprs_cksn = cur_cksn ;
      }

      if (gsm_cipherkey->value_length == GSM_SECURITY_KEY_LENGTH)
      {
        /* Save GSM Ciphering key */
        memscpy((void *)&auth_gsm_cipherkey,sizeof(auth_gsm_cipherkey_type),
                             gsm_cipherkey,sizeof(auth_gsm_cipherkey_type)) ;

        memscpy((void*)&gsm_kc_bit128.value_data[0],UMTS_SECURITY_KEY_LENGTH,
                (void*)&gsm_kc_128bit_in->value_data[0],UMTS_SECURITY_KEY_LENGTH);
        gsm_kc_bit128.value_length = UMTS_SECURITY_KEY_LENGTH;

        auth_gsm_cksn = cur_cksn ;
      }

      break ;
    case RRC_PS_DOMAIN_CN_ID :

      if (gprs_cipherkey->value_length != 0)
      {
         if( is_lte_mode() )
         {
          /* Save CS GPRS Ciphering Key */
          memscpy((void *)&lte_auth_ps_gprs_cipherkey,sizeof(auth_gprs_cipherkey_type),
                             gprs_cipherkey,sizeof(auth_gprs_cipherkey_type)) ;
         } else 
         {
           /* Save CS GPRS Ciphering Key */
          memscpy((void *)&auth_ps_gprs_cipherkey,sizeof(auth_gprs_cipherkey_type),
                             gprs_cipherkey,sizeof(auth_gprs_cipherkey_type)) ;
         }
      }

      if (gprs_integritykey->value_length != 0)
      {
        if( is_lte_mode() )
        {
          /* Save CS GPRS Integrity Key */
          memscpy((void *)&lte_auth_ps_gprs_integritykey,sizeof(auth_gprs_integritykey_type),
                       gprs_integritykey,sizeof(auth_gprs_integritykey_type)) ;
        }
        else 
        {
          /* Save CS GPRS Integrity Key */
          memscpy((void *)&auth_ps_gprs_integritykey,sizeof(auth_gprs_integritykey_type),
                       gprs_integritykey,sizeof(auth_gprs_integritykey_type)) ;
        }
      }
      if (!is_lte_mode() ) 
      {
        if ((gprs_cipherkey->value_length != 0) ||
                                          (gprs_integritykey->value_length != 0))
        {
          auth_ps_gprs_cksn = cur_cksn ;
        }

        if (gsm_cipherkey->value_length == GSM_SECURITY_KEY_LENGTH)
        {
          /* Save GSM Ciphering key */
          memscpy((void *)&auth_gsm_gprs_cipherkey,sizeof(auth_gsm_cipherkey_type),
                               gsm_cipherkey,sizeof(auth_gsm_cipherkey_type)) ;

      memscpy((void*)&gsm_gprs_kc_bit128.value_data[0],UMTS_SECURITY_KEY_LENGTH,
               (void*)&gsm_kc_128bit_in->value_data[0],UMTS_SECURITY_KEY_LENGTH);

      gsm_gprs_kc_bit128.value_length = UMTS_SECURITY_KEY_LENGTH;

          auth_gsm_gprs_cksn = cur_cksn ;
        }

        auth_ps_ciphering_algorithm = ciphering_algorithm ;
      }
#if defined (FEATURE_LTE)
      memset(&emm_ps_domain_int_cipher_log,0,sizeof(lte_nas_emm_ps_domain_int_cipher_keys));
      memscpy(&emm_ps_domain_int_cipher_log.keyset_id_ksips,sizeof(byte),&cur_cksn,sizeof(byte));
      memscpy(emm_ps_domain_int_cipher_log.cipher_keys_ckps,NAS_INT_CIPHER_KEY_LEN,lte_auth_ps_gprs_cipherkey.value_data,NAS_INT_CIPHER_KEY_LEN);
      memscpy(emm_ps_domain_int_cipher_log.int_keys,NAS_INT_CIPHER_KEY_LEN,lte_auth_ps_gprs_integritykey.value_data,NAS_INT_CIPHER_KEY_LEN);
      send_emm_ps_domain_int_cipher_log_info(&emm_ps_domain_int_cipher_log);
#endif
      break ;
    default :
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid domain ID (%d)",domain_identity, 0,0) ;
      break ;
  }

} /* end auth_update_security_keys() */


/*===========================================================================

FUNCTION      AUTH_HANDLE_SIM_AUTHENTICATION_RESPONSE

DESCRIPTION
  This function performs the actions that are required by the MM/GMM
  authentication procedure after the SIM card returns its authentication
  response

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_handle_sim_authentication_response
(
  mm_cmd_type *message,          /* SIM card authentication response */
  dword        client_reference  /* Erroneous auth request reference number */
)
{
  auth_res_type                 res = { 0, {0} } ;
  auth_gprs_cipherkey_type      gprs_cipherkey ;
  auth_gsm_cipherkey_type       gsm_cipherkey ;
  auth_gprs_integritykey_type   gprs_integritykey ;
  auth_gsm_cipherkey_type       gsm_kc_128bit_key;
  auth_auts_type                auts ;
  auth_error_type               auth_error = AUTH_NO_ERROR ;
  byte                          cur_cksn = DELETE_CKSN;
  rrc_cn_domain_identity_e_type domain_identity = RRC_CS_DOMAIN_CN_ID;
  byte                          ciphering_algorithm = FIELD_NOT_IN_USE;
  byte                          cache_guard_timer_id = 0x0;
  mmgsdi_autn_context_enum_type current_authentication_context = MMGSDI_AUTN_USIM_GSM_CONTEXT;
#ifdef FEATURE_LTE
  emm_ctrl_data_type            *emm_ctrl_data_ptr;
#endif  // FEATURE_LTE
  if (message != NULL)
  {
    /* Parse the SIM authentication response */
    auth_error =
      auth_parse_sim_authentication_response(message,&client_reference,
            &res,&gprs_cipherkey,&gprs_integritykey,&gsm_cipherkey,&gsm_kc_128bit_key,
                 &auts,&cur_cksn,&domain_identity,&ciphering_algorithm,
                       &cache_guard_timer_id,&current_authentication_context) ;

#ifdef FEATURE_LTE
      if( AUTH_NO_ERROR == auth_error && is_lte_mode())
      {
        emm_ctrl_data_ptr = emm_db_get_ctrl_data();

        if( NULL != emm_ctrl_data_ptr )
        {
          /* check the separation bit in the AMF */
          if( ( emm_ctrl_data_ptr->autn_amf[0]  & 0x80) != 0x80 )
          {
            auth_error = AUTH_NON_EPS_ACCESS;

            auts.value_length = 0;
          }

        } else 
        {
          MSG_FATAL_DS(MM_SUB, "EMM : EMM Ctrl Data NULL!", 0,0,0);
        }
      }
#endif  // FEATURE_LTE
  }
  else 
  {

#ifdef FEATURE_LTE
    if(is_lte_mode())
    {
      MSG_FATAL_DS(MM_SUB, "EMM : NULL message ptr from GSDI!", 0,0,0);
    }
    else
#endif
    {
      auth_error = AUTH_AUTHENTICATION_UNACCEPTABLE ;

      auts.value_length = 0 ;
    }
  } 
  
  mm_cs_auth_sent_to_card = FALSE;
  mm_ps_auth_sent_to_card = FALSE;
  

  switch (auth_error)
  {
     case AUTH_NO_ERROR:
       {
        auth_request_storage_type *storage_entry ;
        auth_response_callback_type cur_callback ;

        /* Check the authentication error handler is running which belongs to
        ** current (client_reference) authentication client (MM/GMM)
        */
        if ((storage_entry =
                     auth_is_error_handling_activated(client_reference))!=NULL)
        {
          /* Cancel error handling if it is running */
          auth_cancel_error_handling(storage_entry) ;
        }

        if (auth_save_security_context(cur_cksn,domain_identity,
                           &gprs_cipherkey,&gprs_integritykey,&gsm_cipherkey))
        {
          /* Update internal data base of the Authentication server */
          auth_update_security_keys(domain_identity,
                cur_cksn,ciphering_algorithm,
                           &gprs_cipherkey,&gsm_cipherkey,&gprs_integritykey,&gsm_kc_128bit_key) ;

          if( !is_lte_mode() )
          {
            auth_notify_lower_layer(&gprs_cipherkey,&gprs_integritykey,
                                              &gsm_cipherkey,domain_identity, RRC_SIM_UPDATE_VALUE_CHANGED) ;
          }
          /* Call specific authentication client according to
          ** the <client_reference>
          */
          cur_callback = auth_find_callback_ptr(client_reference) ;

          if (cur_callback != NULL)
          {
            /*
            ** Call client's call back in order to send Authentication Response
            ** to the network
            */
            cur_callback(AUTHENTICATION_DONE,AUTH_NO_ERROR,&res,NULL) ;
          }
          else
          {
            MSG_ERROR_DS(MM_SUB, "=MM= There is no client's waiting for the current auth. response",0,0,0) ;
          }

          if ((mm_sim_card_mode == MMGSDI_APP_USIM) && 
              (current_authentication_context == MMGSDI_AUTN_USIM_3G_CONTEXT))
          {
            /* Store last successfull RES */

            if(domain_identity==RRC_CS_DOMAIN_CN_ID)
            {
              memscpy((void *)&mm_last_res_cs,sizeof(auth_res_type),(void *)&res,sizeof(auth_res_type)) ;

              /* Store incoming RAND */
              memscpy((void *)mm_last_rand_cs,MAX_AUTH_DATA_LENGTH,(void *)mm_ongoing_rand,MAX_AUTH_DATA_LENGTH) ;

              mm_cache_mem_cs_valid = TRUE ;
            }
            else
            {
              memscpy((void *)&mm_last_res_ps,sizeof(auth_res_type),(void *)&res,sizeof(auth_res_type)) ;

            /* Store incoming RAND */
              memscpy((void *)mm_last_rand_ps,MAX_AUTH_DATA_LENGTH,(void *)mm_ongoing_rand,MAX_AUTH_DATA_LENGTH) ;

              mm_cache_mem_ps_valid = TRUE ;

            }

            /* Check cache memory guard timer status */
            if (mm_timer_status [ cache_guard_timer_id ] == TIMER_ACTIVE)
            {
              mm_stop_timer(cache_guard_timer_id) ;
            }

            /* Start MM Authentication cache memory guard timer */
            mm_start_timer(cache_guard_timer_id, DEFAULT_TIMEOUT ) ;
          }

        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= SIM/USIM is unable to save security context");
        }
       }
       break ;
     case AUTH_MAK_CODE_FAILURE:
     case AUTH_SQN_FAILURE:
     case AUTH_AUTHENTICATION_UNACCEPTABLE:
#ifdef FEATURE_LTE
     case AUTH_NON_EPS_ACCESS:
#endif  // FEATURE_LTE
       {
        auth_status_type status ;

        /* Find current authentication request properties */
        auth_request_properties_type *request_properties_ptr =
           auth_find_client_record(auth_suspended_cmds_buff,client_reference) ;


        if(request_properties_ptr != NULL)
        { 

          /* In case of MAK or SQN failure perform an error handilng */
          auth_response_callback_type cur_callback =
                auth_error_handler(auth_error,
                      request_properties_ptr->client_response_callback,&status) ;

          /*
          ** Call specific authentication client according to the
          ** <client_reference>
          */
          cur_callback(status,auth_error,NULL,&auts) ;

        }
        else
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= SIM/USIM  Could not find client record. Skipped");
        }

       }
       break ;
     case AUTH_SKIP_AUTHENTICATION_RESPONSE:
       MSG_HIGH_DS_0(MM_SUB, "=MM= SIM/USIM Auth response recognized as obsolete(skiped)");
       break ;
     case AUTH_CAN_NOT_PERFORM_AUTHENTICATION:
       MSG_ERROR_DS(MM_SUB, "=MM= Authentication Request parsing error",0,0,0);
       break ;
     default:
       MSG_FATAL_DS(MM_SUB, "=MM= Illegal parsing error code",0,0,0);
  }
} /* end auth_sim_authentication_response() */


/*===========================================================================

FUNCTION      AUTH_SIM_WRITE_HFN

DESCRIPTION
  This function writes down the PS nad CS Hyper Frame Numbers on the SIM card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_sim_write_hfn
(
  /* CS HFN output buffer */
  byte *cs_hfn,

  /* PS HFN output buffer */
  byte *ps_hfn,

  /* Domain identity */
  rrc_cn_domain_identity_e_type domain_identity
)
{
   
   dword tmp ;
   mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
  if( mm_sim_card_mode == MMGSDI_APP_USIM
#ifdef FEATURE_NAS_REL11
      ||(mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
    )
   {
      byte hfn_buf[6] = {0};
      memscpy((void *)hfn_buf, 6, (void *)cs_hfn, 3);
      memscpy((void *)(hfn_buf+3),3, (void *)ps_hfn, 3);

      /* Convert CS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[0],0,20) ;
      memset(&hfn_buf[0],0x00,3) ;
      b_packd(tmp,&hfn_buf[0],4,20) ;

      /* Convert PS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[3],0,20) ;
      memset(&hfn_buf[3],0x00,3) ;
      b_packd(tmp,&hfn_buf[3],4,20) ;

#ifdef FEATURE_NAS_REL11
      if(mm_sim_card_mode == MMGSDI_APP_USIM)
#endif
      {
        if(! mm_sim_write( MMGSDI_USIM_START_HFN, hfn_buf, 6))
        {
           MSG_ERROR_DS(MM_SUB, "=MM= Could not write HFNs to the START_HFN EF", 0,0,0);
        }
      }
#ifdef FEATURE_NAS_REL11
      else if (mm_sim_card_mode == MMGSDI_APP_SIM)
      {
        memscpy(mm_nas_nv_sim_hfns.start_hfn,6,hfn_buf,6);
        if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/mm_nas_nv_sim_parameters", &mm_nas_nv_sim_hfns, 
                                          sizeof(mm_nas_nv_sim_parameters_T), MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS, 
                                          MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id)) != MCFG_FS_STATUS_OK)
        {
           MSG_ERROR_DS_1(MM_SUB, "=MM= Could not write HFNs to the NV, status = %d", efs_status);
        }
      }
#endif
      memscpy((void *)auth_current_hfns.cs_hfn, MM_HFN_SIZE, (void *)cs_hfn, 3) ;
      memscpy((void *)auth_current_hfns.ps_hfn, MM_HFN_SIZE, (void *)ps_hfn, 3) ;
#ifdef FEATURE_WCDMA
      if (is_umts_mode())
      {
        /* Notify SIM updates to RRC */
         if( mm_wcdma_supported )
         {
            mm_send_rrc_sim_update_req( domain_identity, SYS_RAT_UMTS_RADIO_ACCESS );
         }

#ifdef FEATURE_TDSCDMA
         if (mm_tdscdma_supported)
         {
            mm_send_rrc_sim_update_req( domain_identity, SYS_RAT_TDS_RADIO_ACCESS );
         }
#endif /* FEATURE_TDSCDMA */
      }
#endif /*FEATURE_WCDMA*/      

   }
   else
   {
     MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM to save HFNs") ;
   }
} /* end auth_sim_write_hfn() */

#ifdef FEATURE_NAS_REL11
/*===========================================================================

FUNCTION        AUTH_RESET_CS_HFNS

DESCRIPTION
  This function resets the S Hyper Frame numbers from the USIM card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_reset_cs_hfn(boolean update_to_lower_layer_req)
{
  byte cs_hfn[3] ;
  byte ps_hfn[3] ;
  dword tmp ;

  memset(auth_current_hfns.cs_hfn,0x00,MM_HFN_SIZE);
  memscpy((void *)cs_hfn, 3,(void *)auth_current_hfns.cs_hfn, 3) ;

/*Reset UMTS keys*/
  memscpy((void *)ps_hfn, 3, (void *)auth_current_hfns.ps_hfn, 3) ;
  memset((void *)auth_cs_gprs_cipherkey.value_data,0x00,UMTS_SECURITY_KEY_LENGTH);
  auth_cs_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;
  memset((void *)auth_cs_gprs_integritykey.value_data,0x00,UMTS_SECURITY_KEY_LENGTH) ;
  auth_cs_gprs_integritykey.value_length =  UMTS_SECURITY_KEY_LENGTH;

  /*Reset GSM keys*/
  memset((void *)auth_gsm_cipherkey.value_data,0x00,GSM_SECURITY_KEY_LENGTH) ;
  auth_gsm_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;
  auth_gsm_cksn = 0x07 ;
  memset((void *)gsm_kc_bit128.value_data,0x00,UMTS_SECURITY_KEY_LENGTH) ;
  gsm_kc_bit128.value_length = UMTS_SECURITY_KEY_LENGTH ;  

  if((mm_sim_card_mode == MMGSDI_APP_USIM||mm_sim_card_mode == MMGSDI_APP_SIM) && update_to_lower_layer_req)
  {
      byte hfn_buf[6] = {0};

      memscpy((void *)hfn_buf, 6, (void *)cs_hfn, 3);
      memscpy((void *)(hfn_buf+3),3, (void *)ps_hfn, 3);

      /* Convert CS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[0],0,20) ;
      memset(&hfn_buf[0],0x00,3) ;
      b_packd(tmp,&hfn_buf[0],4,20) ;

      /* Convert PS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[3],0,20) ;
      memset(&hfn_buf[3],0x00,3) ;
      b_packd(tmp,&hfn_buf[3],4,20) ;

#ifdef FEATURE_WCDMA
      if (is_umts_mode())
      {
        /* Notify SIM updates to RRC */
         if( mm_wcdma_supported )
         {
            mm_send_rrc_security_update_req(RRC_SIM_UPDATE_VALUE_CHANGED,RRC_CS_DOMAIN_CN_ID,SYS_RAT_UMTS_RADIO_ACCESS);
         }

#ifdef FEATURE_TDSCDMA
         if (mm_tdscdma_supported)
         {
            mm_send_rrc_security_update_req(RRC_SIM_UPDATE_VALUE_CHANGED,RRC_CS_DOMAIN_CN_ID,SYS_RAT_TDS_RADIO_ACCESS);
         }
#endif /* FEATURE_TDSCDMA */
      }
#endif /*FEATURE_WCDMA*/
      if (mm_gsm_supported)
      {
        mm_send_rr_sim_update_req() ;
      }
   }
   else
   {
     MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM to save HFNs") ;
   }

}
/*===========================================================================

FUNCTION        AUTH_RESET_PS_HFNS

DESCRIPTION
  This function resets the PS Hyper Frame numbers from the USIM card

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_reset_ps_hfn(boolean update_to_lower_layer_req)
{
  byte cs_hfn[3] ;
  byte ps_hfn[3] ;
  dword tmp ;

  memset(auth_current_hfns.ps_hfn,0x00,MM_HFN_SIZE);
  memscpy((void *)cs_hfn, 3,(void *)auth_current_hfns.cs_hfn, 3) ;
  memscpy((void *)ps_hfn, 3, (void *)auth_current_hfns.ps_hfn, 3) ;

  memset((void *)auth_ps_gprs_cipherkey.value_data,0x00,UMTS_SECURITY_KEY_LENGTH);
  auth_ps_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;
  memset((void *)auth_ps_gprs_integritykey.value_data,0x00,UMTS_SECURITY_KEY_LENGTH);
  auth_ps_gprs_integritykey.value_length = UMTS_SECURITY_KEY_LENGTH;

   if((mm_sim_card_mode == MMGSDI_APP_USIM||mm_sim_card_mode == MMGSDI_APP_SIM) && update_to_lower_layer_req)
   {
      byte hfn_buf[6] = {0};
      memscpy((void *)hfn_buf, 6, (void *)cs_hfn, 3);
      memscpy((void *)(hfn_buf+3),3, (void *)ps_hfn, 3);

      /* Convert CS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[0],0,20) ;
      memset(&hfn_buf[0],0x00,3) ;
      b_packd(tmp,&hfn_buf[0],4,20) ;

      /* Convert PS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[3],0,20) ;
      memset(&hfn_buf[3],0x00,3) ;
      b_packd(tmp,&hfn_buf[3],4,20) ;

#ifdef FEATURE_WCDMA
      if (is_umts_mode())
      {
        /* Notify SIM updates to RRC */
         if( mm_wcdma_supported )
         {
           mm_send_rrc_security_update_req(RRC_SIM_UPDATE_VALUE_CHANGED,RRC_PS_DOMAIN_CN_ID,SYS_RAT_UMTS_RADIO_ACCESS);
         }

#ifdef FEATURE_TDSCDMA
         if (mm_tdscdma_supported)
         {
           mm_send_rrc_security_update_req(RRC_SIM_UPDATE_VALUE_CHANGED,RRC_PS_DOMAIN_CN_ID,SYS_RAT_TDS_RADIO_ACCESS);
         }
#endif /* FEATURE_TDSCDMA */
      }
#endif /*FEATURE_WCDMA*/      

   }
   else
   {
     MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM to save HFNs") ;
   }

}
#endif
/*===========================================================================

FUNCTION        AUTH_DELETE_HFNS

DESCRIPTION
  This function deletes the CS and PS Hyper Frame numbers from the SIM card by
  setting CS and PS HFNs to be equal the HFN Threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_delete_hfns(void)
{
  if((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
   ||(mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
    )
  {
    auth_delete_cs_hfn() ;

    auth_delete_ps_hfn() ;
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM when deleting HFNs") ;
  }

} /* end auth_delete_hfns() */

/*===========================================================================

FUNCTION        AUTH_DELETE_CS_HFNS

DESCRIPTION
  This function deletes the CS Hyper Frame numbers from the USIM card by
  setting CS HFN to be equal the HFN Threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_delete_cs_hfn(void)
{
  byte cs_hfn[3] ;
  byte ps_hfn[3] ;

  memscpy((void *)auth_current_hfns.cs_hfn,MM_HFN_SIZE,
                                      (void *)auth_current_hfns.threshold, 3) ;
  memscpy((void *)cs_hfn, 3,(void *)auth_current_hfns.threshold, 3) ;

  memscpy((void *)ps_hfn, 3, (void *)auth_current_hfns.ps_hfn, 3) ;

  if((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
   ||(mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
    )
  {
      auth_sim_write_hfn(cs_hfn, ps_hfn, RRC_CS_DOMAIN_CN_ID);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM when deleting CS HFN") ;
  }

} /* end auth_delete_cs_hfn() */

/*===========================================================================

FUNCTION        AUTH_DELETE_PS_HFNS

DESCRIPTION
  This function deletes the PS Hyper Frame numbers from the USIM card by
  setting PS HFN to be equal the HFN Threshold

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_delete_ps_hfn(void)
{
  byte cs_hfn[3] ;
  byte ps_hfn[3] ;

  memscpy((void *)auth_current_hfns.ps_hfn,MM_HFN_SIZE,
                                      (void *)auth_current_hfns.threshold, 3) ;
  memscpy((void *)ps_hfn, 3, (void *)auth_current_hfns.threshold, 3) ;

  memscpy((void *)cs_hfn, 3, (void *)auth_current_hfns.cs_hfn, 3) ;

  if((mm_sim_card_mode == MMGSDI_APP_USIM)
#ifdef FEATURE_NAS_REL11
   ||(mm_sim_card_mode == MMGSDI_APP_SIM)
#endif
    )
  {
      auth_sim_write_hfn(cs_hfn, ps_hfn, RRC_PS_DOMAIN_CN_ID);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to use GSM SIM when deleting PS HFN") ;
  }

} /* end auth_delete_ps_hfn() */


/*===========================================================================

FUNCTION AUTH_DELETE_CKSN

DESCRIPTION
  This function deletes the current CKSN from the SIM card and set the
  auth_security_contex_availability varible to NO_SECURITY_CONTEXT_AVAILABLE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Set auth_security_contex_availability varible to
  NO_SECURITY_CONTEXT_AVAILABLE

===========================================================================*/
void auth_delete_cksn
(
  auth_security_context_id cksn_context
)
{
  byte cksn = DELETE_CKSN ;
  byte security_buffer[GSM_SECURITY_KEY_LENGTH+1];
  memset(security_buffer,0x00,sizeof(security_buffer));

  switch (cksn_context)
  {
    case AUTH_GSM_SECURITY_CONTEXT      :
 #ifdef FEATURE_SGLTE
     if(MM_SUB_IS_SGLTE)
     {
       HYBRID_STACK_DATA_COPY(auth_gsm_cksn,cksn)
     }
     else
#endif
     {
       auth_gsm_cksn = cksn ;
     }

      if (!auth_gsm_access)
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= Unable to delete GSM CKSN due to USIM config") ;
        return ;
      }

      /* Store GSM Kc */
      memscpy((void *)security_buffer, GSM_SECURITY_KEY_LENGTH+1, (void *)auth_gsm_cipherkey.value_data,
              GSM_SECURITY_KEY_LENGTH);

      /* Store GSM Kc CKSN */
      security_buffer[GSM_SECURITY_KEY_LENGTH] = auth_gsm_cksn;

      if (mm_sim_card_mode == MMGSDI_APP_UNKNOWN)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Card Mode Unknown");
        return;
      }

      if(! mm_sim_write( auth_get_gsm_filename(RRC_CS_DOMAIN_CN_ID),
                         (byte *)security_buffer,
                         GSM_SECURITY_KEY_LENGTH+1) )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN", 0,0,0);
      }
      break ;
    case AUTH_GSM_GPRS_SECURITY_CONTEXT:
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(auth_gsm_gprs_cksn,cksn)
        HYBRID_STACK_DATA_COPY(auth_ps_ciphering_algorithm,0xFF)
      }
      else
#endif
      {
        auth_gsm_gprs_cksn = cksn ;
        auth_ps_ciphering_algorithm = 0xFF ;
      }

      /* Delete GSM(PS)/GPRS ciphering algorithm */
              
      if ((auth_gsm_gprs_keys_present) && 
          (mm_sim_card_mode != MMGSDI_APP_UNKNOWN))
      {
        /* Store GSM Kc */
        memscpy((void *)security_buffer, GSM_SECURITY_KEY_LENGTH+1, (void *)auth_gsm_gprs_cipherkey.value_data,
                GSM_SECURITY_KEY_LENGTH);

        /* Store GSM Kc CKSN */
        security_buffer[GSM_SECURITY_KEY_LENGTH] = auth_gsm_gprs_cksn;

        if(! mm_sim_write( auth_get_gsm_filename(RRC_PS_DOMAIN_CN_ID),
                           (byte *)security_buffer,
                           GSM_SECURITY_KEY_LENGTH+1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN", 0,0,0);
        }
      }
      else if (mm_sim_card_mode != MMGSDI_APP_UNKNOWN)
      {
        ghdi_status_T nv_status ;
        byte kc_buf[9] ;

        MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to delete GSM CKSN due to SIM/USIM config") ;

        kc_buf[8] = auth_gsm_gprs_cksn ;

        if ((nv_status = mm_ghdi_nvmem_write(
             NV_GSM_KCGPRS_I,(ghdi_nvmem_data_T *)kc_buf)) != GHDI_SUCCESS)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to delete GSM GPRS CKSN: %d",nv_status,0,0) ;
        }
      }
      break ;
    case AUTH_CS_GPRS_SECURITY_CONTEXT  :
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(auth_cs_gprs_cksn,cksn)
      }
      else
#endif
      {
        auth_cs_gprs_cksn = cksn ;
      }


      if (mm_sim_card_mode == MMGSDI_APP_USIM)
      {
        if(! mm_sim_write( MMGSDI_USIM_KEYS,(byte *)&cksn,1 ) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete USIM CKSN", 0,0,0);
        }
      }
      else if (mm_sim_card_mode == MMGSDI_APP_SIM)
      {
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(auth_gsm_cksn,cksn)
      }
      else
#endif
      {
        auth_gsm_cksn = cksn ;
      }



        if (!auth_gsm_access)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN due to USIM config",0,0,0) ;
          return ;
        }

        /* Store GSM Kc */
        memscpy((void *)security_buffer, GSM_SECURITY_KEY_LENGTH+1, (void *)auth_gsm_cipherkey.value_data,
                GSM_SECURITY_KEY_LENGTH);

        /* Store GSM Kc CKSN */
        security_buffer[GSM_SECURITY_KEY_LENGTH] = auth_gsm_cksn;

        if(! mm_sim_write( auth_get_gsm_filename(RRC_CS_DOMAIN_CN_ID),
                           (byte *)security_buffer,
                           GSM_SECURITY_KEY_LENGTH+1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN", 0,0,0);
        }
      }
      break ;
    case AUTH_PS_GPRS_SECURITY_CONTEXT  :
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(auth_ps_gprs_cksn,cksn)
        HYBRID_STACK_DATA_COPY(auth_ps_ciphering_algorithm,0xFF)
      }
      else
#endif
      {
        auth_ps_gprs_cksn = cksn ;
        /* Delete GSM(PS)/GPRS ciphering algorithm */
        auth_ps_ciphering_algorithm = 0xFF ;
      }


      if (mm_sim_card_mode == MMGSDI_APP_USIM)
      {
        if(! mm_sim_write( MMGSDI_USIM_KEYSPS,(byte *)&cksn,1 ) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete UMTS PS CKSN, error %d", 0,0,0);
        }
      }
      else if (mm_sim_card_mode != MMGSDI_APP_UNKNOWN)
      {
#ifdef FEATURE_SGLTE
        if(MM_SUB_IS_SGLTE)
        {
          HYBRID_STACK_DATA_COPY(auth_gsm_gprs_cksn,cksn)
        }
        else
#endif
        {
          auth_gsm_gprs_cksn = cksn ;
        }


        if (!auth_gsm_gprs_keys_present)
        {
          MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to delete GSM CKSN due to SIM/USIM config") ;
          return ;
        }

        /* Store GSM Kc */
        memscpy((void *)security_buffer, GSM_SECURITY_KEY_LENGTH+1, (void *)auth_gsm_gprs_cipherkey.value_data,
                GSM_SECURITY_KEY_LENGTH);

        /* Store GSM Kc CKSN */
        security_buffer[GSM_SECURITY_KEY_LENGTH] = auth_gsm_gprs_cksn;

        if(! mm_sim_write( auth_get_gsm_filename(RRC_PS_DOMAIN_CN_ID),
                           (byte *)security_buffer,
                           GSM_SECURITY_KEY_LENGTH+1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM GPRS CKSN", 0,0,0);
        }
      }
      else
      {
 #ifdef FEATURE_SGLTE
        if(MM_SUB_IS_SGLTE)
        {
          HYBRID_STACK_DATA_COPY(auth_gsm_gprs_cksn,cksn)
        }
        else
#endif
        {
          auth_gsm_gprs_cksn = cksn ;
        }
        MSG_HIGH_DS_0(MM_SUB, "=MM= SIM Card Mode unknown");
      }
      break ;
    case AUTH_COMBINED_SECURITY_CONTEXT :
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(auth_gsm_cksn,cksn)
        HYBRID_STACK_DATA_COPY(auth_ps_ciphering_algorithm,0xFF)
      }
      else
#endif
      {
        auth_gsm_cksn = cksn ;
       /* Delete GSM(PS)/GPRS ciphering algorithm */
        auth_ps_ciphering_algorithm = 0xFF ;
      }



      if ((auth_gsm_access) &&
          (mm_sim_card_mode != MMGSDI_APP_UNKNOWN))
      {
        /* Store GSM Kc */
        memscpy((void *)security_buffer, GSM_SECURITY_KEY_LENGTH+1, (void *)auth_gsm_cipherkey.value_data,
                GSM_SECURITY_KEY_LENGTH);

        /* Store GSM Kc CKSN */
        security_buffer[GSM_SECURITY_KEY_LENGTH] = auth_gsm_cksn;

        if(! mm_sim_write( auth_get_gsm_filename(RRC_CS_DOMAIN_CN_ID),
                           (byte *)security_buffer,
                           GSM_SECURITY_KEY_LENGTH+1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN", 0,0,0);
        }

      }
      else
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN due to USIM config",0,0,0) ;
      }

#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(auth_gsm_gprs_cksn,cksn)
      }
      else
#endif
      {
        auth_gsm_gprs_cksn = cksn ;
      }


      if ((auth_gsm_gprs_keys_present) &&
          (mm_sim_card_mode != MMGSDI_APP_UNKNOWN))
      {
        /* Store GSM Kc */
        memscpy((void *)security_buffer,GSM_SECURITY_KEY_LENGTH+1,
               (void *)auth_gsm_gprs_cipherkey.value_data,
                GSM_SECURITY_KEY_LENGTH);

        /* Store GSM Kc CKSN */
        security_buffer[GSM_SECURITY_KEY_LENGTH] = auth_gsm_gprs_cksn;

        if(! mm_sim_write( auth_get_gsm_filename(RRC_PS_DOMAIN_CN_ID),
                           (byte *)security_buffer,
                           GSM_SECURITY_KEY_LENGTH+1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete GSM CKSN", 0,0,0);
        }

      }
      else if (mm_sim_card_mode != MMGSDI_APP_UNKNOWN)
      {
        ghdi_status_T nv_status ;
        byte kc_buf[9] ;

        MSG_HIGH_DS_0(MM_SUB, "=MM= Unable to delete GSM CKSN due to SIM/USIM config") ;

        kc_buf[8] = auth_gsm_gprs_cksn ;

        if ((nv_status = mm_ghdi_nvmem_write(
             NV_GSM_KCGPRS_I,(ghdi_nvmem_data_T *)kc_buf)) != GHDI_SUCCESS)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to delete GSM GPRS CKSN: %d",nv_status,0,0) ;
        }
      }

      if (mm_sim_card_mode == MMGSDI_APP_USIM)
      {
#ifdef FEATURE_SGLTE
        if(MM_SUB_IS_SGLTE)
        {
          HYBRID_STACK_DATA_COPY(auth_cs_gprs_cksn,cksn)
        }
        else
#endif
        {
          auth_cs_gprs_cksn = cksn ;
        }



        if(! mm_sim_write( MMGSDI_USIM_KEYS,(byte *)&cksn,1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete UMTS CS CKSN", 0,0,0);
        }

#ifdef FEATURE_SGLTE
        if(MM_SUB_IS_SGLTE)
        {
          HYBRID_STACK_DATA_COPY(auth_ps_gprs_cksn,cksn)
        }
        else
#endif
        {
          auth_ps_gprs_cksn = cksn ;
        }

        if(! mm_sim_write( MMGSDI_USIM_KEYSPS,(byte *)&cksn,1) )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to delete UMTS PS CKSN", 0,0,0);
        }
      }
      break ;
    default :
      MSG_FATAL_DS(MM_SUB, "=MM= Invalid CKSN context (%d)", (int)cksn_context,0,0) ;
  }
} /* end auth_delete_cksn() */

/*===========================================================================

FUNCTION      AUTH_GET_PS_HFN

DESCRIPTION
  This functions returns current PS Hyper Frame Number
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_get_ps_hfn
(
  /* Output buffer */
  byte *ps_hfn
)
{
  memscpy((void *)ps_hfn, MM_HFN_SIZE,(void *)auth_current_hfns.ps_hfn, 3);
} /* end auth_get_ps_hfn() */


/*===========================================================================

FUNCTION      AUTH_GET_CS_HFN

DESCRIPTION
  This functions returns current CS Hyper Frame Number
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_get_cs_hfn
(
  /* Output buffer */
  byte *cs_hfn
)
{
  memscpy((void *)cs_hfn,MM_HFN_SIZE,(void *)auth_current_hfns.cs_hfn, 3);
} /* end auth_get_ps_hfn() */

/*===========================================================================

FUNCTION      AUTH_HANDLE_TIMER_EXPIRY

DESCRIPTION
  This function performs the actions which are required by TS 24.008 when one
  of the authentication error handling timers (MAK or SQN timer) has been
  expired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_handle_timer_expiry
(
  /* Incoming timer expiration message */
  mm_cmd_type *message
)
{
  byte search_index ;

  /* Search authentication client's is currently running*/
  for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
  {
    /* Validate request storage entry */
    if (auth_request_storage[search_index].auth_request_reference_num != 0)
    {
      /* Recognize the timer that has been expired */
      if ((message->cmd.timer_expiry.timer_id ==
              auth_request_storage[search_index].
                 auth_request_properties.client_timer_set.mak_failure_timer) ||
          (message->cmd.timer_expiry.timer_id ==
              auth_request_storage[search_index].
                   auth_request_properties.client_timer_set.sqn_failure_timer))
      {
        /* Client has been found. Perform required actions */
        auth_reset_auth_attempt(&(auth_request_storage[search_index].auth_request_properties));
      }
    }
  }
} /* end auth_handle_timer_expiry() */


/*===========================================================================

FUNCTION        AUTH_RESET_TRANSACTION

DESCRIPTION
  This function allows the MM or GMM authentication clients to reset its
  own authentication attempts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_reset_transaction
(
  /* Authentication Client's ID */
  client_id id
)
{
  // byte search_index ;
  auth_request_storage_type *transaction_ptr ;

  if ((transaction_ptr = auth_find_client_record_by_callback(
              auth_suspended_cmds_buff,(auth_response_callback_type)id))!=NULL)
  {
     memset((void *)transaction_ptr, 0x00, sizeof(auth_request_storage_type)) ;
  }

  if ((transaction_ptr = auth_find_client_record_by_callback(
                  auth_request_storage,(auth_response_callback_type)id))!=NULL)
  {
    auth_timer_set_type *timer_record =
                   &transaction_ptr->auth_request_properties.client_timer_set ;

    /* Stop currently running authentication error handling timers
    ** (MAK or SQN)
    */
    auth_stop_error_timers(timer_record) ;

    /* Re-start previously running MM or GMM retx timers */
    auth_restart_retx_timers
                   (timer_record->timer_list_length,timer_record->timer_list) ;

    /* Clean up an authentication transaction properties */
    memset((void *)transaction_ptr,0x00,sizeof(auth_request_storage_type)) ;
  }
} /* end auth_reset_transaction() */
/*===========================================================================

FUNCTION         AUTH_STOP_AUTH_FAIL_TIMERS

DESCRIPTION
  This function allows the MM or GMM authentication clients to stop the
  timers T3320/T3318

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_stop_auth_fail_timers
(
  /* Authentication Client's ID */
  client_id id
)
{
  // byte search_index ;
  auth_request_storage_type *transaction_ptr ;

  if ((transaction_ptr = auth_find_client_record_by_callback(
              auth_suspended_cmds_buff,(auth_response_callback_type)id))!=NULL)
  {
     memset((void *)transaction_ptr, 0x00, sizeof(auth_request_storage_type)) ;
  }

  if ((transaction_ptr = auth_find_client_record_by_callback(
                  auth_request_storage,(auth_response_callback_type)id))!=NULL)
  {
    auth_timer_set_type *timer_record =
                   &transaction_ptr->auth_request_properties.client_timer_set ;

    /* Stop currently running authentication error handling timers
    ** (MAK or SQN)
    */
    auth_stop_error_timers(timer_record) ;

    /* Clean up an authentication transaction properties */
    memset((void *)transaction_ptr,0x00,sizeof(auth_request_storage_type)) ;
  }
} /* end auth_reset_transaction() */

/*===========================================================================

FUNCTION         AUTH_STOP_AUTH_MAK_SQN_TIMERS

DESCRIPTION
  This function allows the MM, EMM or GMM authentication clients to stop the
  timers the MAK/SQN timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_stop_auth_mak_sqn_timers
(
  /* Authentication Client's ID */
  client_id id
)
{
  auth_request_storage_type *transaction_ptr ;

  if ((transaction_ptr = auth_find_client_record_by_callback(
                  auth_request_storage,(auth_response_callback_type)id))!=NULL)
  {
    auth_timer_set_type *timer_record =
                   &transaction_ptr->auth_request_properties.client_timer_set ;

    /* Stop currently running authentication error handling timers
    ** (MAK or SQN)
    */
    auth_stop_error_timers(timer_record) ;

  }
} /* end auth_stop_auth_mak_sqn_timers() */

/*===========================================================================

FUNCTION    AUTH_HANDLE_NETWORK_REJECT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_handle_network_reject
(
  client_id id,
  byte timer_list_length,
  auth_timer_list_type *timer_list
)
{
  if (id != (client_id)NULL)
  {
    /*
    ** Tansaction in progress
    */
    byte search_index ;

    /* Search for the appropriate active authentication attempt */
    for (search_index=0; search_index<MAX_REQUEST_STORAGE_SIZE; search_index++)
    {
      if ((client_id)(auth_request_storage[search_index].
                       auth_request_properties.client_response_callback) == id)
      {
        auth_request_storage_type *transaction_ptr = NULL;
        auth_timer_set_type       *timer_record    = NULL; 

        if ((transaction_ptr = auth_find_client_record_by_callback(
              auth_suspended_cmds_buff,(auth_response_callback_type)id))!=NULL)
        {
           memset((void *)transaction_ptr, 0x00, sizeof(auth_request_storage_type)) ;
        }

        timer_record = &auth_request_storage[search_index].
                                      auth_request_properties.client_timer_set ;

        /* Stop currently running authentication error handling timers
        ** (MAK or SQN)
        */
        auth_stop_error_timers(timer_record) ;
        timer_record->num_failures = 0;

        /* Clean up an authentication transaction properties */
        memset(&auth_request_storage[search_index],
                                      0x00,sizeof(auth_request_storage_type)) ;
      } /* end if */
    } /* end for */
  } /* if */
  
  /* According to TS 24.008 4.3.2.6 & 4.7.7.6, the GMM or MM shall
  ** stop any of the retransmission timers that are running
  */
  auth_stop_retx_timers(timer_list_length,timer_list) ;
  

} /* end auth_handle_network_reject() */

/*===========================================================================

FUNCTION    AUTH_IS_CS_CKIK_DERIVED_FROM_GSMKC

DESCRIPTION
  This function checks whether the UMTS/TDS CS keys CK and IK are derived from GSM KC or given by card.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If keys are derived from GSM KC
  FALSE - If keys are not derived and given by card

SIDE EFFECTS
  None
===========================================================================*/
boolean auth_is_cs_ckik_derived_from_gsmkc()
{
  if((auth_cs_gprs_cipherkey.value_length == UMTS_SECURITY_KEY_LENGTH) &&
     (auth_cs_gprs_integritykey.value_length == UMTS_SECURITY_KEY_LENGTH) &&
     ((GSM_SECURITY_KEY_LENGTH + GSM_SECURITY_KEY_LENGTH) == MAX_AUTH_DATA_LENGTH) &&
     ((GSM_SECURITY_KEY_LENGTH + C5_KCi_LENGTH + C5_KCi_LENGTH) == MAX_AUTH_DATA_LENGTH) &&
     (memcmp((void *)auth_cs_gprs_cipherkey.value_data, (void *)(&auth_cs_gprs_cipherkey.value_data[GSM_SECURITY_KEY_LENGTH]), GSM_SECURITY_KEY_LENGTH) == 0) &&
     (memcmp((void *)auth_cs_gprs_cipherkey.value_data, (void *)auth_gsm_cipherkey.value_data, GSM_SECURITY_KEY_LENGTH) == 0) &&
     (memcmp((void *)auth_cs_gprs_integritykey.value_data, (void *)(&auth_cs_gprs_integritykey.value_data[MAX_AUTH_DATA_LENGTH-C5_KCi_LENGTH]), C5_KCi_LENGTH) == 0) &&
     (memcmp((void *)(&auth_cs_gprs_integritykey.value_data[C5_KCi_LENGTH]), (void *)auth_gsm_cipherkey.value_data, GSM_SECURITY_KEY_LENGTH) == 0))
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION    AUTH_GET_CIPHERING_KEY_SEQUENCE_NUMBER

DESCRIPTION
  This function returns current Ciphering Key Sequence Number is stored on the
  SIM/USIM card and saves this number into internal Authentication server
  variable

DEPENDENCIES
  None

RETURN VALUE
  Ciphering Key Sequence Number

SIDE EFFECTS
  None
===========================================================================*/
byte auth_get_ciphering_key_sequence_number
(
  auth_security_context_id cksn_context
)
{
  byte cksn = 0 ;

  switch (cksn_context)
  {
    case AUTH_GSM_SECURITY_CONTEXT     :
      cksn = auth_gsm_cksn ;
      break ;
    case AUTH_GSM_GPRS_SECURITY_CONTEXT     :
      cksn = auth_gsm_gprs_cksn ;
      break ;
    case AUTH_CS_GPRS_SECURITY_CONTEXT :
      if (auth_cs_gprs_cksn != DELETE_CKSN &&
          !gmm_anite_gcf_flag
#ifdef FEATURE_ENHANCED_NW_SELECTION
          && (!reg_sim_read_ens_flag()) 
#endif
          )
      {
        if (auth_is_cs_ckik_derived_from_gsmkc() == TRUE)
        {
          auth_delete_cksn(cksn_context);
          MSG_HIGH_DS_1(MM_SUB,"=MM= CS CK and IK derived from GSM Kc. auth_cs_gprs_cksn= %d", auth_cs_gprs_cksn);
        }
      }
      cksn = auth_cs_gprs_cksn ;
      break ;
    case AUTH_PS_GPRS_SECURITY_CONTEXT :
      cksn = auth_ps_gprs_cksn ;
      break ;
    default:
      MSG_FATAL_DS(MM_SUB, "=MM= Invalid CKSN's security context ID (%d)", (int)cksn_context,0,0) ;
  }

  /* Return the Ciphering Key Sequence Number */
  return (cksn & 0x07);
} /* end auth_get_ciphering_key_sequence_number() */

/*===========================================================================

FUNCTION    AUTH_TRANSLATE_HFN

DESCRIPTION
  This function translate HFN value from interface into comparison able format

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dword auth_translate_hfn
(
  byte *incoming_hfn
)
{
  return b_unpackd(incoming_hfn,0,20) ;
} /* end auth_get_kc() */

/*===========================================================================

FUNCTION    AUTH_GET_HFN_THRESHOLD

DESCRIPTION
  This function returns the HFN threshhold is restored on the USIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dword auth_get_hfn_threshold
(
  void
)
{
  return auth_translate_hfn(auth_current_hfns.threshold) ;
} /* end auth_get_kc() */


/*===========================================================================
FUNCTION      AUTH_LOAD_CS_DOMAIN_KEYS

DESCRIPTION   load cs domain keys for sim/usim

DEPENDENCIES
  None

RETURN VALUE 
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void auth_load_cs_domain_keys
(
  void
)
{
  boolean mmgsdi_status = FALSE;
  byte security_buffer[40] ;

  /* reset the buffer */
  memset((void *)security_buffer,0x00,sizeof(security_buffer));

  if (mm_sim_card_mode == MMGSDI_APP_SIM)
  {
    if(mm_imsi_is_native)
    {
      mmgsdi_status = mm_sim_read( MMGSDI_GSM_KC, security_buffer, 
                                   GSM_SECURITY_KEY_LENGTH+1);

      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read SIM Service table",0,0,0) ;
        return ;
      }

      /* Store GSM Kc */
      memscpy((void *)auth_gsm_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                     (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

      /* Store GSM Kc CKSN */
      auth_gsm_cksn = security_buffer[8] ;

      auth_cs_gprs_cksn = auth_gsm_cksn ;
    }
  }
  else if (mm_sim_card_mode == MMGSDI_APP_USIM)
  {
    if (auth_gsm_access)
    {
      /* Read Ciphering Key Sequence Number */
      mmgsdi_status = mm_sim_read( MMGSDI_USIM_KC,
                                   security_buffer,
                                   GSM_SECURITY_KEY_LENGTH+1);

      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM GSM Kc",0,0,0) ;
        auth_gsm_access = FALSE ;
        return ;
      }

      /* Store GSM Kc */
      memscpy((void *)auth_gsm_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                           (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;
      auth_gsm_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;

      /* Store GSM Kc CKSN */
      auth_gsm_cksn = security_buffer[8] ;
    
      /* Read Security context for CS domain */
      mmgsdi_status = mm_sim_read( MMGSDI_USIM_KEYS,
                                   security_buffer,
                                   33);
  
      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM CS Keys",0,0,0) ;
        return  ;
      }

      auth_cs_gprs_cksn = security_buffer[0] ;

      memscpy((void *)auth_cs_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[1],UMTS_SECURITY_KEY_LENGTH) ;
      auth_cs_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;

      memscpy((void *)auth_cs_gprs_integritykey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[17],UMTS_SECURITY_KEY_LENGTH) ;
      auth_cs_gprs_integritykey.value_length = UMTS_SECURITY_KEY_LENGTH ;
    }
  }
  else
  {
    MSG_ERROR_DS(MM_SUB, "=MM= sim card is not usim",0,0,0);
    return ;
  }

  return ;
}

/*===========================================================================
FUNCTION      AUTH_LOAD_PS_DOMAIN_KEYS

DESCRIPTION   load ps domain keys for sim/usim

DEPENDENCIES
  None

RETURN VALUE 
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void auth_load_ps_domain_keys
(
  void
)
{
  boolean mmgsdi_status = FALSE;
  byte security_buffer[40] ;

  /* reset the buffer */
  memset((void *)security_buffer,0x00,sizeof(security_buffer));

  if (mm_sim_card_mode == MMGSDI_APP_SIM)
  {
    if (auth_gsm_gprs_keys_present)
    {
      /* Read GPRS Ciphering Key & GPRS Ciphering Key Sequence Number */
      mmgsdi_status = mm_sim_read( MMGSDI_GSM_KCGPRS,
                                   security_buffer,
                                   GSM_SECURITY_KEY_LENGTH+1);

      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read GPRS SIM security context",0,0,0) ;
        return  ;
      }

      /* Store GSM GPRS Kc */
      memscpy((void *)auth_gsm_gprs_cipherkey.value_data, MAX_AUTH_DATA_LENGTH,
                       (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

      /* Store GSM GPRS Kc CKSN */
      auth_gsm_gprs_cksn = security_buffer[8] ;

      auth_ps_gprs_cksn = auth_gsm_gprs_cksn ;
    }
    else
    {
      ghdi_status_T nv_status ;
      if ((nv_status = mm_ghdi_nvmem_read(
           NV_GSM_KCGPRS_I,(ghdi_nvmem_data_T *)security_buffer)) != GHDI_SUCCESS)
      {
        MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to read GSM security context: %d",nv_status,0,0) ;
        return ;
      }

      /* Store GSM GPRS Kc */
      memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                     (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

      /* Store GSM GPRS Kc CKSN */
      auth_gsm_gprs_cksn = security_buffer[8] ;

      auth_ps_gprs_cksn = auth_gsm_gprs_cksn ;
    }
  }
  else if (mm_sim_card_mode == MMGSDI_APP_USIM)
  {
    if (auth_gsm_gprs_keys_present)
    {
      /* Read Ciphering Key Sequence Number */
      mmgsdi_status = mm_sim_read( MMGSDI_USIM_KCGPRS,
                                   security_buffer,
                                   GSM_SECURITY_KEY_LENGTH+1);

      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM GSM GPRS Kc",0,0,0) ;
        auth_gsm_gprs_keys_present = FALSE ;
        return ;
      }

      /* Store GSM Kc */
      memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                           (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;
      auth_gsm_gprs_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;

      /* Store GSM Kc CKSN */
      auth_gsm_gprs_cksn = security_buffer[8] ;

      /* Read Security context for PS domain */
      mmgsdi_status = mm_sim_read( MMGSDI_USIM_KEYSPS,
                                   security_buffer,
                                   33);
  
      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM PS Keys",0,0,0) ;
        return  ;
      }

      auth_ps_gprs_cksn = security_buffer[0] ;

      memscpy((void *)auth_ps_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[1],UMTS_SECURITY_KEY_LENGTH) ;
      auth_ps_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;

      memscpy((void *)auth_ps_gprs_integritykey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[17],UMTS_SECURITY_KEY_LENGTH) ;
      auth_ps_gprs_integritykey.value_length = UMTS_SECURITY_KEY_LENGTH ;
    }
  }
  else
  {
    MSG_ERROR_DS(MM_SUB, "=MM= sim card is not usim",0,0,0);
    return ;
  }

  return ;
}

/*===========================================================================

FUNCTION    AUTH_LOAD_SECURITY_CONTEXT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean auth_load_security_context
(
  void
)
{
  boolean mmgsdi_status = FALSE;
  //mmgsdi_len_type file_length = 0;

  byte security_buffer[40] ;
  byte etalon_zero_8[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} ;
  byte etalon_zero_16[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} ;
  boolean equalize_cs_cksn = FALSE ;
  boolean equalize_ps_cksn = FALSE ;
#ifdef FEATURE_NAS_REL11
  dword cs_hfn ;
  dword ps_hfn ;
  dword hfn_threshold ;
#endif

  memset((void *)&security_buffer[0],0x00,sizeof(security_buffer));

  //boolean sim_nvm_read_status = FALSE;

  memset((void *)auth_gsm_cipherkey.value_data,0x00,GSM_SECURITY_KEY_LENGTH) ;
  auth_gsm_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;
  auth_gsm_cksn = 0x07 ;

  memset(
    (void *)auth_gsm_gprs_cipherkey.value_data,0x00,GSM_SECURITY_KEY_LENGTH) ;
  auth_gsm_gprs_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;
  auth_gsm_gprs_cksn = 0x07 ;

  memset(
    (void *)auth_cs_gprs_cipherkey.value_data,0x00,UMTS_SECURITY_KEY_LENGTH) ;
  auth_cs_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;
  memset(
    (void *)auth_cs_gprs_integritykey.value_data,
                                              0x00,UMTS_SECURITY_KEY_LENGTH) ;
  auth_cs_gprs_integritykey.value_length =  UMTS_SECURITY_KEY_LENGTH;
  auth_cs_gprs_cksn = 0x07 ;

  memset(
    (void *)auth_ps_gprs_cipherkey.value_data,0x00,UMTS_SECURITY_KEY_LENGTH) ;
  auth_ps_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;
  memset(
    (void *)auth_ps_gprs_integritykey.value_data,
                                              0x00,UMTS_SECURITY_KEY_LENGTH) ;
  auth_ps_gprs_integritykey.value_length = UMTS_SECURITY_KEY_LENGTH ;
  auth_ps_gprs_cksn = 0x07 ;

   memset(
    (void *)gsm_kc_bit128.value_data,0x00,UMTS_SECURITY_KEY_LENGTH) ;
   gsm_kc_bit128.value_length = UMTS_SECURITY_KEY_LENGTH ;  

  memset(
    (void *)gsm_gprs_kc_bit128.value_data,0x00,UMTS_SECURITY_KEY_LENGTH) ;
   gsm_gprs_kc_bit128.value_length = UMTS_SECURITY_KEY_LENGTH ;  

  /* Get current SIM/USIM file name according to SIM card mode */

  /* Read GSM Ciphering Key & GSM Ciphering Key Sequence Number */
  if (mm_sim_card_mode == MMGSDI_APP_SIM)
  {
    if(mm_imsi_is_native)
    {
      mmgsdi_status = mm_sim_read( MMGSDI_GSM_KC, security_buffer, GSM_SECURITY_KEY_LENGTH+1);

      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read SIM Service table",0,0,0) ;
        return FALSE;
      }

      /* Store GSM Kc */
      memscpy((void *)auth_gsm_cipherkey.value_data, MAX_AUTH_DATA_LENGTH,
                     (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

      /* Store GSM Kc CKSN */
      auth_gsm_cksn = security_buffer[8] ;

      auth_cs_gprs_cksn = auth_gsm_cksn ;

      if (auth_gsm_gprs_keys_present)
      {
      /* Read GPRS Ciphering Key & GPRS Ciphering Key Sequence Number */
        mmgsdi_status = mm_sim_read( MMGSDI_GSM_KCGPRS,
                                   security_buffer,
                                   GSM_SECURITY_KEY_LENGTH+1);

        if( !mmgsdi_status )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read SIM security context",0,0,0) ;
          return FALSE;
        }

        /* Store GSM GPRS Kc */
        memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                       (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

        /* Store GSM GPRS Kc CKSN */
        auth_gsm_gprs_cksn = security_buffer[8] ;

        auth_ps_gprs_cksn = auth_gsm_gprs_cksn ;

        if (auth_gsm_gprs_keys_present)
        {
              /* Read GPRS Ciphering Key & GPRS Ciphering Key Sequence Number */
          mmgsdi_status = mm_sim_read( MMGSDI_GSM_KCGPRS,
                                           security_buffer,
                                           GSM_SECURITY_KEY_LENGTH+1);

          if( !mmgsdi_status )
          {
             MSG_ERROR_DS(MM_SUB,"MM is unable to read GPRS SIM security context",0,0,0) ;
             return FALSE ;
          }

           /* Store GSM GPRS Kc */
          memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                           (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

          /* Store GSM GPRS Kc CKSN */
          auth_gsm_gprs_cksn = security_buffer[8] ;

          auth_ps_gprs_cksn = auth_gsm_gprs_cksn ;

        }
        if (auth_gsm_gprs_keys_present)
        {
              /* Read GPRS Ciphering Key & GPRS Ciphering Key Sequence Number */
          mmgsdi_status = mm_sim_read( MMGSDI_GSM_KCGPRS,
                                           security_buffer,
                                           GSM_SECURITY_KEY_LENGTH+1);

          if( !mmgsdi_status )
          {
             MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read GPRS SIM security context",0,0,0) ;
             return FALSE ;
          }

          /* Store GSM GPRS Kc */
          memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                           (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

          /* Store GSM GPRS Kc CKSN */
          auth_gsm_gprs_cksn = security_buffer[8] ;

          auth_ps_gprs_cksn = auth_gsm_gprs_cksn ;

        }
      }
      else
      {
        ghdi_status_T nv_status ;
        if ((nv_status = mm_ghdi_nvmem_read(
             NV_GSM_KCGPRS_I,(ghdi_nvmem_data_T *)security_buffer)) != GHDI_SUCCESS)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to read GSM security context: %d",nv_status,0,0) ;
          return FALSE;
        }

        /* Store GSM GPRS Kc */
        memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                       (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;

        /* Store GSM GPRS Kc CKSN */
        auth_gsm_gprs_cksn = security_buffer[8] ;

        auth_ps_gprs_cksn = auth_gsm_gprs_cksn ;
      }
    }
    else
    {
      /* Sim has changed so leave the CKSN fields to its initial values 0x7 */
      MSG_HIGH_DS_0(MM_SUB,"=MM= SIM has changed. so leaving the CKSNs as 0x07") ;
    }
  }
  else
  {
    if (mm_sim_card_mode == MMGSDI_APP_USIM)
    {

      if (auth_gsm_access)
      {
        /* Read Ciphering Key Sequence Number */
        mmgsdi_status = mm_sim_read( MMGSDI_USIM_KC,
                                     security_buffer,
                                     GSM_SECURITY_KEY_LENGTH+1);

        if( !mmgsdi_status )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM GSM Kc",0,0,0) ;

          auth_gsm_access = FALSE ;

          return FALSE;
        }

        /* Store GSM Kc */
        memscpy((void *)auth_gsm_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                             (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;
        auth_gsm_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;

        /* Store GSM Kc CKSN */
        auth_gsm_cksn = security_buffer[8] ;
      }

      if (auth_gsm_gprs_keys_present)
      {
        /* Read Ciphering Key Sequence Number */
        mmgsdi_status = mm_sim_read( MMGSDI_USIM_KCGPRS,
                                     security_buffer,
                                     GSM_SECURITY_KEY_LENGTH+1);

        if( !mmgsdi_status )
        {
          MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM GSM GPRS Kc",0,0,0) ;

          auth_gsm_gprs_keys_present = FALSE ;

          return FALSE;
        }

        /* Store GSM Kc */
        memscpy((void *)auth_gsm_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
                             (void *)security_buffer,GSM_SECURITY_KEY_LENGTH) ;
        auth_gsm_gprs_cipherkey.value_length = GSM_SECURITY_KEY_LENGTH ;

        /* Store GSM Kc CKSN */
        auth_gsm_gprs_cksn = security_buffer[8] ;
      }

      /* Read Security context for CS domain */
      mmgsdi_status = mm_sim_read( MMGSDI_USIM_KEYS,
                                   security_buffer,
                                   33);
  
      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM CS Keys",0,0,0) ;
        return  FALSE;
      }

      auth_cs_gprs_cksn = security_buffer[0] ;

      memscpy((void *)auth_cs_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[1],UMTS_SECURITY_KEY_LENGTH) ;
      auth_cs_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;

      memscpy((void *)auth_cs_gprs_integritykey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[17],UMTS_SECURITY_KEY_LENGTH) ;
      auth_cs_gprs_integritykey.value_length = UMTS_SECURITY_KEY_LENGTH ;

      /* Read Security context for PS domain */
      mmgsdi_status = mm_sim_read( MMGSDI_USIM_KEYSPS,
                                   security_buffer,
                                   33);
  
      if( !mmgsdi_status )
      {
        MSG_ERROR_DS(MM_SUB, "=MM= MM is unable to read USIM PS Keys",0,0,0) ;
        return  FALSE;
      }

      auth_ps_gprs_cksn = security_buffer[0] ;

      memscpy((void *)auth_ps_gprs_cipherkey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[1],UMTS_SECURITY_KEY_LENGTH) ;
      auth_ps_gprs_cipherkey.value_length = UMTS_SECURITY_KEY_LENGTH ;

      memscpy((void *)auth_ps_gprs_integritykey.value_data,MAX_AUTH_DATA_LENGTH,
             (void *)&security_buffer[17],UMTS_SECURITY_KEY_LENGTH) ;
      auth_ps_gprs_integritykey.value_length = UMTS_SECURITY_KEY_LENGTH ;
    }
    else
    {
      /*
      ** If the current card mode is invalid, i.e. SIM or USIM cards are
      ** not available, mark the Ciphering Key Sequence Number as deleted
      */
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid SIM card mode %d",mm_sim_card_mode,0,0) ;
      return FALSE;

    }
  }

  /* Initialize HFNs (CS, PS and Thershold */
  auth_load_hfns(&auth_current_hfns) ;
  
/*24.008: 4.1.1.8	Handling of security related parameters at switch on and switch off*/
#ifdef FEATURE_NAS_REL11
  if((mm_sim_card_mode == MMGSDI_APP_USIM) ||
     (mm_sim_card_mode == MMGSDI_APP_SIM))
  {
    cs_hfn = auth_translate_hfn(auth_current_hfns.cs_hfn)  ;
    ps_hfn = auth_translate_hfn(auth_current_hfns.ps_hfn)  ;
    hfn_threshold = auth_get_hfn_threshold() ;

    if ((cs_hfn >= hfn_threshold)  || (auth_cs_gprs_cksn == 7))
    {
      auth_reset_cs_hfn(FALSE);
      auth_delete_cksn(AUTH_CS_GPRS_SECURITY_CONTEXT) ;
    }

    if((ps_hfn >= hfn_threshold) || (auth_ps_gprs_cksn == 7))
    {
      auth_reset_ps_hfn(FALSE);
      auth_delete_cksn(AUTH_PS_GPRS_SECURITY_CONTEXT);
    }
  }
#endif

  if ((auth_gsm_cipherkey.value_length == 0) ||
        (memcmp((void *)auth_gsm_cipherkey.value_data,
                          (void *)etalon_zero_8,GSM_SECURITY_KEY_LENGTH)==0))
  {
    if (((auth_cs_gprs_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_cs_gprs_cipherkey.value_data,
                       (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)) &&
        ((auth_cs_gprs_integritykey.value_length != 0) &&
          (memcmp((void *)auth_cs_gprs_integritykey.value_data,
                         (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)))

    {
      auth_generate_kc(&auth_cs_gprs_cipherkey,&auth_cs_gprs_integritykey,&auth_gsm_cipherkey,
                       &gsm_kc_bit128) ;
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create GSM ( CS ) Kc",0,0,0) ;
    }
  }
  else
  {
    if (((auth_cs_gprs_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_cs_gprs_cipherkey.value_data,
                       (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)) &&
        ((auth_cs_gprs_integritykey.value_length != 0) &&
          (memcmp((void *)auth_cs_gprs_integritykey.value_data,
                         (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)))

    {
      auth_gsm_cipherkey_type     temp_gsm_cipherkey;
      auth_generate_kc(&auth_cs_gprs_cipherkey,&auth_cs_gprs_integritykey,&temp_gsm_cipherkey,
                       &gsm_kc_bit128) ;
    }
    else
    {
#ifndef MM_UTF_TESTING
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create GSM ( CS ) Kc,Even KC valid",0,0,0) ;
#endif     
    }
  }

  /*
  ** If the MS is running in UMTS mode which is operated by the GSM SIM,
  ** convert GSM Kc into UMTS CS CK & IK
  */
  if ((auth_cs_gprs_cipherkey.value_length == 0) ||
        (memcmp((void *)auth_cs_gprs_cipherkey.value_data,
                          (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)==0))
  {
    if ((auth_gsm_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_gsm_cipherkey.value_data,
                            (void *)etalon_zero_8,GSM_SECURITY_KEY_LENGTH)!=0))
    {
     auth_generate_ck(&auth_gsm_cipherkey,&auth_cs_gprs_cipherkey) ;

     equalize_cs_cksn = TRUE ;
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create UMTS CS CK",0,0,0) ;
    }
  }

  if ((auth_cs_gprs_integritykey.value_length == 0) ||
        (memcmp((void *)auth_cs_gprs_integritykey.value_data,
                          (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)==0))
  {
    if ((auth_gsm_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_gsm_cipherkey.value_data,
                            (void *)etalon_zero_8,GSM_SECURITY_KEY_LENGTH)!=0))
    {
      auth_generate_ik(&auth_gsm_cipherkey,&auth_cs_gprs_integritykey) ;

      equalize_cs_cksn = TRUE ;
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create UMTS CS IK",0,0,0) ;
    }
  }

  if (equalize_cs_cksn)
  {
     auth_cs_gprs_cksn  = auth_gsm_cksn ;
  }

  if ((auth_gsm_gprs_cipherkey.value_length == 0) ||
        (memcmp((void *)auth_gsm_gprs_cipherkey.value_data,
                          (void *)etalon_zero_8,GSM_SECURITY_KEY_LENGTH)==0))
  {
    if (((auth_ps_gprs_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_ps_gprs_cipherkey.value_data,
                       (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)) &&
        ((auth_ps_gprs_integritykey.value_length != 0) &&
          (memcmp((void *)auth_ps_gprs_integritykey.value_data,
                         (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)))

    {
      auth_generate_kc(&auth_ps_gprs_cipherkey,&auth_ps_gprs_integritykey,&auth_gsm_gprs_cipherkey,
                       &gsm_gprs_kc_bit128) ;
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create GSM GPRS Kc",0,0,0) ;
    }
  }
 else
  {
    if (((auth_ps_gprs_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_ps_gprs_cipherkey.value_data,
                       (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)) &&
        ((auth_ps_gprs_integritykey.value_length != 0) &&
          (memcmp((void *)auth_ps_gprs_integritykey.value_data,
                         (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)!=0)))

    {
      auth_gsm_cipherkey_type     temp_gsm_gprs_cipherkey;
      auth_generate_kc(&auth_ps_gprs_cipherkey,&auth_ps_gprs_integritykey,&temp_gsm_gprs_cipherkey,
                       &gsm_gprs_kc_bit128) ;
    }
    else
    {
#ifndef MM_UTF_TESTING
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create GSM GPRS Kc,Even gprs kc valid",0,0,0) ;
#endif
    }
  }

  /*
  ** If the MS is running in UMTS mode which is operated by the GSM SIM,
  ** convert GSM GPRS Kc into UMTS PS CK & IK
  */
  if ((auth_ps_gprs_cipherkey.value_length == 0) ||
        (memcmp((void *)auth_ps_gprs_cipherkey.value_data,
                          (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)==0))
  {
    if ((auth_gsm_gprs_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_gsm_cipherkey.value_data,
                            (void *)etalon_zero_8,GSM_SECURITY_KEY_LENGTH)!=0))
    {
      auth_generate_ck(&auth_gsm_gprs_cipherkey,&auth_ps_gprs_cipherkey) ;

      equalize_ps_cksn = TRUE ;
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create UMTS PS CK",0,0,0) ;
    }
  }

  if ((auth_ps_gprs_integritykey.value_length == 0) ||
        (memcmp((void *)auth_ps_gprs_integritykey.value_data,
                          (void *)etalon_zero_16,UMTS_SECURITY_KEY_LENGTH)==0))
  {
    if ((auth_gsm_gprs_cipherkey.value_length != 0) &&
          (memcmp((void *)auth_gsm_cipherkey.value_data,
                            (void *)etalon_zero_8,GSM_SECURITY_KEY_LENGTH)!=0))
    {
      auth_generate_ik(&auth_gsm_gprs_cipherkey,&auth_ps_gprs_integritykey) ;

      equalize_ps_cksn = TRUE ;
    }
    else
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to create UMTS PS IK",0,0,0) ;
    }
  }

  if (equalize_ps_cksn)
  {
     auth_ps_gprs_cksn  = auth_gsm_gprs_cksn ;
  }
  return TRUE;

} /* end auth_load_security_context() */

/*===========================================================================

FUNCTION    AUTH_HANDLE_HFN_RELEASE_IND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_handle_hfn_release_ind
(
  byte *cs_rrc_hfn,
  byte *ps_rrc_hfn
)
{
  dword cs_hfn ;
  dword ps_hfn ;
  dword hfn_threshold ;

  cs_hfn = auth_translate_hfn(cs_rrc_hfn)  ;
  ps_hfn = auth_translate_hfn(ps_rrc_hfn)  ;
  hfn_threshold = auth_get_hfn_threshold() ;

  memscpy((void *)auth_current_hfns.cs_hfn,MM_HFN_SIZE,(void *)cs_rrc_hfn,MM_HFN_SIZE) ;
  memscpy((void *)auth_current_hfns.ps_hfn,MM_HFN_SIZE,(void *)ps_rrc_hfn,MM_HFN_SIZE) ;


  /** 33.102 R11 6.4.3  
      Each time an RRC connection is released the values STARTCS and STARTPS of the bearers that were protected in that RRC 
      connection are compared with THRESHOLD. If STARTCS and/or STARTPS are greater than or equal to THRESHOLD, 
      the ME sets the START value in the ME for the corresponding core network domain(s) to zero, 
      deletes the cipher key and the integrity key stored on the USIM and the ME and sets the KSI to invalid (refer to section 6.4.4). 
      Otherwise, the STARTCS and STARTPS are stored in the ME ..***/
  
  if (cs_hfn >= hfn_threshold)
  {
#ifdef FEATURE_NAS_REL11
    if ((mm_sim_card_mode == MMGSDI_APP_USIM) ||
        (mm_sim_card_mode == MMGSDI_APP_SIM))
    {
      auth_reset_cs_hfn(TRUE);
    }
#else
    auth_delete_cs_hfn() ;
#endif

    auth_delete_cksn(AUTH_CS_GPRS_SECURITY_CONTEXT) ;
  }
#ifndef FEATURE_NAS_REL11
  else
  {
    auth_sim_write_hfn(cs_rrc_hfn, ps_rrc_hfn, RRC_CS_DOMAIN_CN_ID);
  }
#endif

  /*
  ** If PS HFN is reached the threshhold, invalidate it
  */
/*24.008: 4.1.1.8	Handling of security related parameters at switch on and switch off*/
  if (ps_hfn >= hfn_threshold)
  {
#ifdef FEATURE_NAS_REL11
    if ((mm_sim_card_mode == MMGSDI_APP_USIM) ||
        (mm_sim_card_mode == MMGSDI_APP_SIM))
    {
      auth_reset_ps_hfn(TRUE);
    }
#else
    auth_delete_ps_hfn() ;
#endif

    auth_delete_cksn(AUTH_PS_GPRS_SECURITY_CONTEXT);
  }
#ifndef FEATURE_NAS_REL11
  else
  {
    auth_sim_write_hfn(cs_rrc_hfn, ps_rrc_hfn, RRC_PS_DOMAIN_CN_ID);
  }
#endif

} /* end auth_handle_hfn_release_ind() */

/*===========================================================================

FUNCTION    AUTH_EQUALIZE_SECURITY_CONTEXT

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_equalize_security_context
(
  mm_cn_domain_identity_type current_domain_id
)
{
  auth_gprs_cipherkey_type    *source_ck   = NULL ;
  auth_gsm_cipherkey_type     *source_ik   = NULL ;
  auth_gprs_integritykey_type *source_kc   = NULL ;
  byte                        *source_cksn = NULL ;
  auth_gsm_cipherkey_type     *gsm_kc_128bit_in=NULL;

  switch (current_domain_id)
  {
    case MM_CS_DOMAIN_CN_ID :

      source_ck   = &auth_ps_gprs_cipherkey   ;
      source_ik   = &auth_ps_gprs_integritykey ;
      source_kc   = &auth_gsm_gprs_cipherkey  ;
      source_cksn = &auth_ps_gprs_cksn ;
      gsm_kc_128bit_in=&gsm_gprs_kc_bit128;

      break ;
    case MM_PS_DOMAIN_CN_ID :

      source_ck   = &auth_cs_gprs_cipherkey   ;
      source_ik   = &auth_cs_gprs_integritykey ;
      source_kc   = &auth_gsm_cipherkey       ;
      source_cksn = &auth_cs_gprs_cksn ;
      gsm_kc_128bit_in=&gsm_kc_bit128;

      break ;
    default:
      MSG_FATAL_DS(MM_SUB, "=MM= Invalid domain ID (%d)", (int)current_domain_id,0,0);
  }

  if ((current_domain_id == MM_CS_DOMAIN_CN_ID) ||
                                     (current_domain_id == MM_PS_DOMAIN_CN_ID))
  {
    /* Update internal database */
    auth_update_security_keys((rrc_cn_domain_identity_e_type)current_domain_id,
                            *source_cksn,0xFF,source_ck,source_kc,source_ik,gsm_kc_128bit_in) ;

    /* Update lower layers */
    auth_notify_lower_layer(source_ck,source_ik,source_kc,(rrc_cn_domain_identity_e_type)current_domain_id, RRC_SIM_UPDATE_VALUE_CHANGED) ;
  }

} /* end auth_equalize_security_context() */

/*===========================================================================

FUNCTION      AUTH_MM_PERFORM_CACHE_AUTHENTICATION

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_mm_perform_cache_authentication
(
  byte rand_length,
  byte *rand_value,
  auth_response_callback_type authentication_response_builder
)
{
  if (mm_cache_mem_cs_valid)
  {
    if (memcmp((void *)mm_last_rand_cs,(void *)rand_value,rand_length) == 0)
    {
                
      MSG_HIGH_DS_0(MM_SUB, "=MM= MM's performing MM cache authentication") ;

      /* Send Authentication Response message */
      authentication_response_builder(
                         AUTHENTICATION_DONE,AUTH_NO_ERROR,&mm_last_res_cs,NULL) ;

      return TRUE ; /* Authentication completed */
    }
  }
  return FALSE ; /* Authentication is still in progress */
} /* end auth_mm_perform_cache_authentication() */

/*===========================================================================

FUNCTION      AUTH_GMM_PERFORM_CACHE_AUTHENTICATION

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_gmm_perform_cache_authentication
(
  byte rand_length,
  byte *rand_value,
  auth_response_callback_type authentication_response_builder
)
{
    if (mm_cache_mem_ps_valid)
    {
      if (memcmp((void *)mm_last_rand_ps,(void *)rand_value,rand_length) == 0)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= MM's performing GMM cache authentication") ;

        /* Send Authentication Response message */
        authentication_response_builder(
                         AUTHENTICATION_DONE,AUTH_NO_ERROR,&mm_last_res_ps,NULL) ;

        return TRUE ; /* Authentication completed */
      }
    }
    return FALSE ; /* Authentication is still in progress */
} /* end auth_mm_perform_cache_authentication() */

/*===========================================================================

FUNCTION      AUTH_RESET_CACHE_MEMORY

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_reset_cache_memory
(
  boolean timer_expired,
  byte timer_id,
  rrc_cn_domain_identity_e_type cn_domain_identity
)
{
  if (mm_cache_mem_cs_valid && cn_domain_identity==RRC_CS_DOMAIN_CN_ID)
  {
    if (timer_expired)
    {
      switch (timer_id)
      {
        case TIMER_T3218:
          mm_timer_status [ TIMER_T3218 ] = TIMER_STOPPED ;
          break;      
        default:
          ERR_FATAL("Invalid timer id (%d)",timer_id,0,0);
          break;    
      }    
    }
    else
    {
       /* Stop MM Authentication cache guard timer */
       mm_stop_timer(TIMER_T3218) ;         
    }
    mm_auth_delete_rand();
    mm_auth_delete_res();
    MSG_HIGH_DS_0(MM_SUB, "=MM= Reset CS authentication cache memory");     
    mm_cache_mem_cs_valid = FALSE ;
  }
      
  if (mm_cache_mem_ps_valid && cn_domain_identity==RRC_PS_DOMAIN_CN_ID)
  {
    if (timer_expired)
    {
      switch (timer_id)
      {
        case TIMER_T3316:
          mm_timer_status [ TIMER_T3316 ] = TIMER_STOPPED ;
          break;

#ifdef FEATURE_LTE
        case TIMER_T3416:
          mm_timer_status [ TIMER_T3316 ] = TIMER_STOPPED ;
          break;
#endif
      
      default:
        MSG_FATAL_DS(MM_SUB,"Invalid timer id (%d)",timer_id,0,0);
        break;
      
      }

    }
    else
    {
#ifdef FEATURE_LTE 
        if(is_lte_mode())  
        {
          /* for true multimode, this needs to be revisited,
             timer Id is needed to identify which guard timer to stop
          */
      
          /* Stop  Authentication cache guard timer */
          mm_stop_timer(TIMER_T3416) ;
        }
        else       
#endif
        {
          /* Stop GMM Authentication cache guard timer */
          mm_stop_timer(TIMER_T3316) ;
        }
    }
    gmm_auth_delete_rand();
    gmm_auth_delete_res();
    MSG_HIGH_DS_0(MM_SUB, "=MM= Reset PS authentication cache memory");

    mm_cache_mem_ps_valid = FALSE ;
  }
} 

void mm_auth_delete_rand(void)
{
  memset((void*)mm_last_rand_cs, 0, MAX_AUTH_DATA_LENGTH) ;
}

void mm_auth_delete_res( void )
{
  memset((void *)&mm_last_res_cs,0,sizeof(auth_res_type)) ;
}
void gmm_auth_delete_rand(void)
{
  memset((void*)mm_last_rand_ps, 0, MAX_AUTH_DATA_LENGTH) ;
}

void gmm_auth_delete_res( void )
{
  memset((void *)&mm_last_res_ps,0,sizeof(auth_res_type)) ;
}

auth_gprs_cipherkey_type mm_auth_get_ck( void )
{
  if( is_lte_mode() )
    return lte_auth_ps_gprs_cipherkey;
  else 
    return auth_ps_gprs_cipherkey;
}

auth_gprs_integritykey_type mm_auth_get_ik( void )
{
  if( is_lte_mode() )
    return lte_auth_ps_gprs_integritykey;
  else
    return auth_ps_gprs_integritykey;
}
#ifdef FEATURE_SGLTE
boolean  auth_filter_domain_identity ( mm_cmd_type *msg_cmd,rrc_cn_domain_identity_e_type* domain_identity)
{
  dword client_reference;
  auth_request_properties_type *auth_request_properties ;
  if (TRUE== auth_filter_auth_response(msg_cmd, &client_reference))
  {
    auth_request_properties =
          auth_find_client_record(auth_suspended_cmds_buff,client_reference) ;
    if(auth_request_properties != NULL)
     {
       *domain_identity = auth_request_properties->cn_domain_identity ;
     }
    else
    {
      return FALSE;
    }
  }
  else 
  {
    return FALSE;
  }
  return TRUE;
}
void mm_copy_auth_cs_database(mm_as_id_e_type dest,mm_as_id_e_type src)
{
  memscpy((void *)mm_ongoing_rand_sim[dest],MAX_AUTH_DATA_LENGTH, 
                              (void *)mm_ongoing_rand_sim[src],MAX_AUTH_DATA_LENGTH);
/* This variable contains last RAND which has been received from the NW */
  memscpy((void *)mm_last_rand_cs_sim[dest],MAX_AUTH_DATA_LENGTH, 
                              (void *)mm_last_rand_cs_sim[src],MAX_AUTH_DATA_LENGTH);
/* This variable contains last RES which has been sent to the NW */
  memscpy((void *)&mm_last_res_cs_sim[dest],sizeof(auth_res_type), 
                   (void *)&mm_last_res_cs_sim[src],sizeof(auth_res_type));
  memscpy((void *)&auth_current_hfns_sim[dest],sizeof(hfn_type), 
                   (void *)&auth_current_hfns_sim[src],sizeof(hfn_type));
  memscpy((void *)&auth_request_storage_sim[dest],sizeof(auth_request_storage_type), 
                           (void *)&auth_request_storage_sim[src],sizeof(auth_request_storage_type));
   memscpy((void *)&auth_suspended_cmds_buff_sim[dest],sizeof(auth_request_storage_type), 
                           (void *)&auth_suspended_cmds_buff_sim[src],sizeof(auth_request_storage_type));
  auth_request_reference_number_sim[dest]=auth_request_reference_number_sim[src];
  mm_cache_mem_cs_valid_sim[dest] = mm_cache_mem_cs_valid_sim[src];
  auth_gsm_cksn_sim[dest]  =    auth_gsm_cksn_sim[src];
}
#endif

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
boolean auth_is_procedure_to_be_aborted(client_id  id)
{
    boolean abort_procedue = FALSE;
    auth_request_storage_type *transaction_ptr ;
    
   if ((transaction_ptr = auth_find_client_record_by_callback(
                    auth_suspended_cmds_buff,(auth_response_callback_type)id))!=NULL)
   {
        auth_timer_set_type *timer_record =
                     &transaction_ptr->auth_request_properties.client_timer_set ;
        if((mm_timer_status[timer_record->mak_failure_timer] == TIMER_ACTIVE)  || 
	    (mm_timer_status[timer_record->sqn_failure_timer] == TIMER_ACTIVE))
		{
         MSG_HIGH_DS_0(MM_SUB, "MAC FAILURE TIMER/SQNFAILURE TIMERS are runing");

		} 
        else
		{
         abort_procedue = TRUE;
	    MSG_HIGH_DS_0(MM_SUB, "Auth Request sent to MMGSDI");
		}
	}
	

    return abort_procedue;

}
#endif

#ifdef FEATURE_NAS_REL11
/*===========================================================================

FUNCTION    MM_AUTH_UPDATE_HFNS_SIM_CARD

DESCRIPTION
  This function saves the CS,PS Hyper Frame numbers to EF. If set_theshold is true then it deletes HFN and writes to card.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void auth_update_hfns_to_sim_card( boolean set_threshold )
{
   dword tmp ;
   hfn_type auth_local_hfn ;
   hfn_nv_stored_type hfn_nv_hotswap;
   mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
   memset( &hfn_nv_hotswap, 0x00, sizeof(hfn_nv_stored_type) );
   
   if( mm_sim_card_mode == MMGSDI_APP_USIM || mm_sim_card_mode == MMGSDI_APP_SIM)
   {
      byte hfn_buf[6] = {0};

      memscpy(&auth_local_hfn,sizeof(hfn_type),&auth_current_hfns,sizeof(hfn_type));
 
      MSG_HIGH_DS_3( MM_SUB, "=MM= Auth Current CS HFN: %d %d %d", auth_current_hfns.cs_hfn[0],auth_current_hfns.cs_hfn[1],auth_current_hfns.cs_hfn[2]);
      MSG_HIGH_DS_3( MM_SUB, "=MM= Auth Current PS HFN: %d %d %d", auth_current_hfns.ps_hfn[0],auth_current_hfns.ps_hfn[1],auth_current_hfns.ps_hfn[2]);
      MSG_HIGH_DS_3( MM_SUB, "=MM= Auth Threshold: %d %d %d", auth_current_hfns.threshold[0],auth_current_hfns.threshold[1],auth_current_hfns.threshold[2]);

      if(set_threshold)
      {
        memscpy((void *)auth_current_hfns.cs_hfn,MM_HFN_SIZE,
                                         (void *)auth_current_hfns.threshold, 3) ;
        memscpy((void *)auth_current_hfns.ps_hfn,MM_HFN_SIZE,
                                         (void *)auth_current_hfns.threshold, 3) ;

      }

      memscpy((void *)hfn_buf, 6, (void *)auth_current_hfns.cs_hfn, 3);
      memscpy((void *)(hfn_buf+3),3, (void *)auth_current_hfns.ps_hfn, 3);

      /* Convert CS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[0],0,20) ;
      memset(&hfn_buf[0],0x00,3) ;
      b_packd(tmp,&hfn_buf[0],4,20) ;

      /* Convert PS Start values from USIM to internal (RRC) format */
      tmp = b_unpackd(&hfn_buf[3],0,20) ;
      memset(&hfn_buf[3],0x00,3) ;
      b_packd(tmp,&hfn_buf[3],4,20) ;

      if(mm_sim_card_mode == MMGSDI_APP_USIM)
      {
        if(! mm_sim_write( MMGSDI_USIM_START_HFN, hfn_buf, 6))
        {
           MSG_ERROR_DS_0(MM_SUB, "=MM= Could not write HFNs to the START_HFN EF");
        }
      }
      else if (mm_sim_card_mode == MMGSDI_APP_SIM)
      {
        memscpy(mm_nas_nv_sim_hfns.start_hfn,6,hfn_buf,6);
        if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/mm_nas_nv_sim_parameters" , 
                             &mm_nas_nv_sim_hfns,
                             sizeof(mm_nas_nv_sim_parameters_T), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,
                             MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id)) != MCFG_FS_STATUS_OK)
        {
           MSG_ERROR_DS_1(MM_SUB, "=MM= Could not write HFNs to the NV mm_sim_card_mode = MMGSDI_APP_SIM, status= %d", efs_status);
        }
      }

      memscpy(&auth_current_hfns,sizeof(hfn_type),&auth_local_hfn,sizeof(hfn_type));
   }
   else
   {

     if ((mm_sim_card_mode == MMGSDI_APP_UNKNOWN) &&
         (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_AVAILABLE))
     {
        MSG_HIGH_DS_1(MM_SUB, "=MM= Write HFN to NV in case of hotswap - sim state : %d",mm_serving_plmn.info.sim_state) ;
        memscpy(&hfn_nv_hotswap.current_hfn,sizeof(hfn_type),&auth_current_hfns,sizeof(hfn_type));
        hfn_nv_hotswap.hotswap = 0x01;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
        if(!MM_SUB_IS_SGLTE &&
           (mm_sub_id == MM_AS_ID_2))
        {
          if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap" , 
                               &hfn_nv_hotswap,
                               sizeof(hfn_nv_stored_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id)) != MCFG_FS_STATUS_OK)
          {
            MSG_ERROR_DS_1(MM_SUB, "=MM= Could not write HFNs to the NV for SUB2, status = %d", efs_status);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, " HFNs value written in NV for SUB2 hotswap successfully");
          }
       }
       else
#endif
       if(mm_sub_id == MM_AS_ID_1)
       {
          if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap" , 
                               &hfn_nv_hotswap,
                               sizeof(hfn_nv_stored_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id)) != MCFG_FS_STATUS_OK)
          {
            MSG_ERROR_DS_1(MM_SUB, "=MM= Could not write HFNs to the NV for SUB1, status = %d", efs_status);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, " HFNs value written in NV for SUB1 hotswap successfully");
          }
       }
#if defined(FEATURE_TRIPLE_SIM)
       else if(mm_sub_id == MM_AS_ID_3)
       {
          if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/hfn_info_stored_during_hotswap" , 
                               &hfn_nv_hotswap,
                               sizeof(hfn_nv_stored_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,
                               MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id)) != MCFG_FS_STATUS_OK)
          {
            MSG_ERROR_DS_1(MM_SUB, "=MM= Could not write HFNs to the NV for SUB3, status = %d", efs_status);
          }
          else
          {
            MSG_HIGH_DS_0(MM_SUB, " HFNs value written in NV for SUB3 hotswap successfully");
          }
       }
#endif
     }
     else
     {
       MSG_HIGH_DS_1(MM_SUB, "=MM= Unable to use SIM to save HFNs - %d",mm_sim_card_mode) ;
     }
   }
}

#endif

/*==================================================================================
FUNCTION AUTH_DEACT_RETX_TIMER

DESCRIPTION
  This function deactivates the specified timer if it was running and stopped during
  Authentication failure.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===================================================================================*/
void auth_deact_retx_timer
(
  client_id    id,
  timer_id_T   timer_id
)
{
  auth_request_storage_type *transaction_ptr;
  byte                      timer_index;


  if((transaction_ptr = auth_find_client_record_by_callback(
                  auth_request_storage,(auth_response_callback_type)id))!=NULL)
  {
    auth_timer_set_type *timer_record =
                   &transaction_ptr->auth_request_properties.client_timer_set;
    

    for(timer_index = 0; timer_index < timer_record->timer_list_length; timer_index++)
    {
      if((timer_record->timer_list[timer_index].timer_was_active == TRUE) &&
         (timer_record->timer_list[timer_index].timer == timer_id))
      {
        MSG_HIGH_DS_1(MM_SUB, "Retx Timer %d deactivated in MM Auth Server",timer_id);
        timer_record->timer_list[timer_index].timer_was_active = FALSE;
        break;
      }
    }    
  }
}
