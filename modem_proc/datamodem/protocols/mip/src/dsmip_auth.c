/*===========================================================================
                                      
                          D S M I P _ A U T H . C

DESCRIPTION

 The Data Services Mobile IP authentication lookup table implementation.
 Contains API to perform lookups on tables matching security parameter 
 indices and authentication algorithms as required by RFC 2002.

EXTERNALIZED FUNCTIONS

  mip_auth_lookup_alg()
    Returns a function pointer to the authentication algorithm associated
    with a specified security parameter index (SPI).

  mip_auth_lookup_spi()
    Returns the security parameter index (SPI) associated with the specified 
    authentication algorithm function.

  mip_auth_calc_md5_prefix_plus_suffix
    Generates the MD5 "postfix + suffix" result for the provided data 
    (ref RFC2002 S 3.5.1).

  mip_auth_calc_chap
    Generates the CHAP authenticator for the provided data (ref ?).

  mip_auth_retrieve_ss_data
    Determine which shared secrets to use for this MIP registration,
    and retrieve them wherever they are stored (if necesaary)

Copyright (c) 2000-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_auth.c_v   1.9   16 Nov 2002 00:30:40   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_auth.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
06/03/13    sd     ATCoP decoupling changes
05/01/12    sb     Always On UIM Feature Cleanup
05/20/11    op     Migrated to MSG 2.0 macros
03/15/11    op     Featurization cleanup fix
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/31/11    op     Cleaning up usage of UIM API
07/16/10    ssh    Fixed compiler warning due to -Wstrict-prototypes
02/05/10    ls     Removed assert when calling MMGSDI API
01/11/10    ms     Replaced UIM_3GPD Constants with NVRUIM_3GPD
10/23/09    ls     Fixed the issue causing MIP crash
06/04/09    ls     Support CSIM feature
01/17/08   mga/sq  Merged and made changes to use uim_freeq
12/03/04    jk     Set uim slot to UIM_SLOT_AUTOMATIC to support dual slots
05/18/04    jk     Added support for 3GPD authentication on the R-UIM card.
04/28/04    jd     Use new shadow NV structure, linted file
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
07/18/02    jay    If DMU is active, use the temporary key values from 
                   DMU session info for shared secrets.
05/07/02    jd     Created mip_auth_retrieve_ss_data to centralized logic 
                   to determining password to be used for MIP registration
10/30/01    jd     Include MSB of FAC in lower-order 237 byte of FAC portion
                   of MN-AAA hash calculation.
10/08/01    jd     Calculate offset correctly for challenge > 237 bytes. 
07/17/01    jd     Authentication functions return 16-bit (was 32) length.
07/04/01    jd     Code cleanup and naming made consistent with coding std
06/27/01    mvl    Changed the message level for these routines to LOW
03/05/01    jd     Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "dsmip_auth.h"
#ifdef __unix
#include "md5global.h"
#endif
#include "md5.h"
#include "amssassert.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#include "dsmip_regsm.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#include "dsat707mip.h"
#include "ps_utils.h"
#include "psi.h"
#include "uim.h"
#include "uim_v.h" 
#include "nvruimi.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "ds3gmmgsdiif.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================

                             INTERNAL DATA

===========================================================================*/

nv_item_type      mip_nv_item; /* scratchpad */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION MIP_AUTH_LOOKUP_ALG

DESCRIPTION
  This function searches an authorisation lookup table for the specified 
  key (SPI) and returns the associated handler if found.

PARAMETERS
  table - pointer to the appropriate authentication lookup table 
  key   - key (SPI) to look up

RETURN VALUE
  Pointer to the authentication function corresponding to 'key', if found.
  NULL if not found.

DEPENDENCIES
  The last entry in 'table' must have the handler field = NULL.  
  This delineates the bound of the table.

SIDE EFFECTS
  None.

===========================================================================*/
mip_auth_fptr_type mip_auth_lookup_alg
(
  mip_auth_lookup_table *  table,
  uint32                   key
)
{
  while (table->auth_fn != NULL)
  {
    if (table->key == key)
    {
      break;  
    }
    table++;
  }

  return (table->auth_fn);

} /* mip_auth_lookup_alg() */


/*===========================================================================

FUNCTION MIP_AUTH_LOOKUP_SPI

DESCRIPTION
  This function searches an authorisation lookup table for the specified 
  handler (function) and returns the associated security parameter index,
  if found.

PARAMETERS
  table - pointer to the appropriate authentication lookup table 
  handler - pointer to the authentication function to look up

RETURN VALUE
  key   - key (SPI) corresponding to 'handler', if found.
          set to SPI of last entry in table if not found.

DEPENDENCIES
  The last entry in 'table' must have the handler field = NULL.  
  This delineates the bound of the table.  the SPI should be configured
  to not fool the software or message recipients.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 mip_auth_lookup_spi
(
  mip_auth_lookup_table *  table,
  mip_auth_fptr_type       auth_fn
)
{
  while (table->auth_fn != NULL)
  {
    if (table->auth_fn == auth_fn)
    {
      break;
    }
    table++;
  }

  return (table->key);

} /* mip_auth_lookup_spi() */


/*===========================================================================

FUNCTION MIP_AUTH_CALC_MD5_PREFIX_PLUS_SUFFIX

DESCRIPTION

  This function calculates the "MD5 prefix+suffix" authenticator value
  on the data as follows:

    -  the shared secret provided
    -  the data provided
    -  the shared secret again.

  NOTE:  The caller should anticipate the length of the authenticator it 
         expects returned and make sufficient memory available at the 
         address pointed to by the authenticator parameter (see below).

DEPENDENCIES
  None

PARAMETERS
  data          - ptr to the data for which the authenticator is calculated
  data_len      - the length of 'data'
  secret        - pointer to the shared secret value
  secret_len    - the length of 'secret'
  authenticator - the calculated authenticator is returned via this pointer
  ignore_param_1 - to keep compiler happy, needed with -Wstrict-prototypes
  ignore_param_2 - to keep compiler happy, needed with -Wstrict-prototypes

RETURN VALUE
  Returns the length of the generated authenticator (in bytes)

SIDE EFFECTS
  None

===========================================================================*/
uint16 mip_auth_calc_md5_prefix_plus_suffix
(
  void *   data,                   /* start of data to authenticate        */
  uint32   data_len,               /* length of data                       */
  void *   secret,                 /* shared secret used for calculation   */
  uint32   secret_len,             /* length of shared secret              */
  void *   authenticator,          /* storage for calculated authenticator */
  uint32   profile_index,          /* nai entry index on RUIM to use       */
  void *   ignore_param_1,         /* to keep compiler happy               */
  uint32   ignore_param_2          /* to keep compiler happy               */
)
{
  MD5_CTX   auth_ctx;                            /* authentication context */
  mmgsdi_compute_ip_data_type  cmpt_ip_data;
  mmgsdi_return_enum_type      mmgsdi_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Calculate the authenticator value for the provided data.
    Check whether authentication should be done in software, or on the card
  -------------------------------------------------------------------------*/
  if(NVRUIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     NVRUIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    DATA_MIP_MSG2(MSG_LEGACY_LOW,"MD5 << init MD5 << secret (%d bytes)" 
                  "MD5 << data (%d bytes) MD5 << secret",\
                  secret_len,data_len);
    MD5Init (&auth_ctx);

    MD5Update (&auth_ctx, (unsigned char *) secret, secret_len);

    MD5Update (&auth_ctx, (unsigned char *) data, data_len);

    MD5Update (&auth_ctx, (unsigned char *) secret, secret_len);

    MD5Final ((unsigned char *) authenticator, &auth_ctx);
  }
  else
  {
    cmpt_ip_data.operation_type = MMGSDI_CMPT_IP_MIP_MN_HA;
    cmpt_ip_data.data.mn_ha_data.nai_entry_index = (uint8)profile_index;
    cmpt_ip_data.data.mn_ha_data.registration_data_length = data_len;
    /* Pass the data pointer */
    cmpt_ip_data.data.mn_ha_data.registration_data_ptr = data;

    DATA_MIP_MSG0(MSG_LEGACY_MED, "Sending Compute Ip Auth - MN-HA");

    mmgsdi_status = ds3g_mmgsdi_session_compute_ip_auth (cmpt_ip_data);
    /* Check to see if REQ was OK.  */
    if ( mmgsdi_status != MMGSDI_SUCCESS)
    {
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed to Call MMGSDI Session "
                    "Compute Ip Auth %d - MN-HA",
                    mmgsdi_status);
      return 0;  
    }

    DATA_MIP_MSG0(MSG_LEGACY_MED, "Retrieve Authentication Data - MN-HA");

    ds3g_mmgsdi_mip_data_retrieve( authenticator ); 
  }

  /*-------------------------------------------------------------------------
    Return the authenticator length
  -------------------------------------------------------------------------*/
  return (MIP_AUTH_MD5_LEN);

} /* mip_auth_calc_md5_prefix_plus_suffix */



/*===========================================================================

FUNCTION MIP_AUTH_CALC_CHAP

DESCRIPTION

  This function calculates the "chap" authenticator value
  on the data as follows:

  NOTE:  The caller should anticipate the length of the authenticator it 
         expects returned and make sufficient memory available at the 
         address pointed to by the authenticator parameter (see below).

DEPENDENCIES
  None

PARAMETERS
  data          - ptr to the data for which the authenticator is calculated
  data_len      - the length of 'data'
  secret        - pointer to the shared secret value
  secret_len    - the length of 'secret'
  authenticator - the calculated authenticator is returned via this pointer

RETURN VALUE
  Returns the length of the generated authenticator (in bytes)

SIDE EFFECTS
  None

===========================================================================*/
uint16 mip_auth_calc_chap
(
  void *   data,                   /* start of data to authenticate        */
  uint32   data_len,               /* length of data                       */
  void *   secret,                 /* shared secret used for calculation   */
  uint32   secret_len,             /* length of shared secret              */
  void *   challenge,              /* FA challenge value                   */
  uint32   challenge_len,          /* length of FA challenge               */
  void *   authenticator,          /* storage for calculated authenticator */
  uint32   profile_index           /* nai entry index on RUIM to use       */
)
{
  MD5_CTX   auth_ctx;              /* authentication context               */
  char      imdt_val[16];          /* intermediate MD5 hash value          */
  uint32    offset;                /* start of 237 LSbytes of challenge    */
  mmgsdi_compute_ip_data_type  cmpt_ip_data;
  mmgsdi_return_enum_type      mmgsdi_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(NVRUIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     NVRUIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    /*-----------------------------------------------------------------------
      Calculate the byte offset into challenge where the 237 
      least-significant bytes begin
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "Challenge len = %d", challenge_len);
  
    if (challenge_len > 237)
    {
      offset = challenge_len - 237;
    }
    else 
    {
      offset = 0;
    }
  
    DATA_MIP_MSG2(MSG_LEGACY_LOW, "Offset = %d data_len = %d", offset, data_len);
    /*-----------------------------------------------------------------------
      Calculate the intermediate MD5 value
    -----------------------------------------------------------------------*/
    MD5Init( &auth_ctx );
  
    MD5Update( &auth_ctx, (unsigned char *) data,   data_len);
  
    MD5Final ((unsigned char *) imdt_val, &auth_ctx);
  
    /*-----------------------------------------------------------------------
      Calculate the authenticator value to be returned
    -----------------------------------------------------------------------*/
    MD5Init( &auth_ctx );
  
    if (challenge_len != 0)
    {
      DATA_MIP_MSG0(MSG_LEGACY_LOW, "MD5 << MSB(challenge) (1 byte) ");
      MD5Update (&auth_ctx, (unsigned char *) challenge, 1);
    }
  
    DATA_MIP_MSG2(MSG_LEGACY_LOW, "MD5 << secret (%d bytes) MD5 << intermediate"
                  "(%d bytes)", secret_len, imdt_val);
    MD5Update( &auth_ctx, (unsigned char *) secret, secret_len);
  
    MD5Update( &auth_ctx, (unsigned char *) imdt_val, 16);
  
    if (challenge_len != 0)
    {
      DATA_MIP_MSG1(MSG_LEGACY_LOW, "MD5 << challenge(LSB's) (%d bytes)", 
               challenge_len - offset);
      MD5Update( &auth_ctx, 
                 &(((unsigned char *) challenge)[offset]), 
                 challenge_len - offset);
    }
  
    DATA_MIP_MSG0(MSG_LEGACY_LOW, "MD5 << final ");
    MD5Final ((unsigned char *) authenticator, &auth_ctx);
  }
  else
  {
    cmpt_ip_data.operation_type = MMGSDI_CMPT_IP_MIP_RRQ;
    cmpt_ip_data.data.rrq_data.rrq_data_length = data_len;
    /* Pass the data pointer */
    cmpt_ip_data.data.rrq_data.rrq_data_ptr = data;

    DATA_MIP_MSG0(MSG_LEGACY_MED, "Sending Compute Ip Auth - RRQ");

    mmgsdi_status = ds3g_mmgsdi_session_compute_ip_auth( cmpt_ip_data );
    /* Check to see if REQ was OK.  */
    if ( mmgsdi_status != MMGSDI_SUCCESS)
    {
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed to Call MMGSDI Session "
                    "Compute Ip Auth %d - RRQ",
                    mmgsdi_status);
      return 0;  
    }

    cmpt_ip_data.operation_type = MMGSDI_CMPT_IP_MIP_MN_AAA;
    cmpt_ip_data.data.mn_aaa_data.nai_entry_index = (uint8)profile_index;
    cmpt_ip_data.data.mn_aaa_data.challenge_length = challenge_len;
    /* Pass the data pointer */
    cmpt_ip_data.data.mn_aaa_data.challenge_ptr = challenge;

    DATA_MIP_MSG0(MSG_LEGACY_MED, "Sending Compute Ip Auth - MN-AAA");

    mmgsdi_status = ds3g_mmgsdi_session_compute_ip_auth( cmpt_ip_data );
    /* Check to see if REQ was OK.  */
    if ( mmgsdi_status != MMGSDI_SUCCESS)
    {
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP Failed to Call MMGSDI Session "
                    "Compute Ip Auth %d - MN-AAA",
                    mmgsdi_status);
      return 0;  
    }
    DATA_MIP_MSG0(MSG_LEGACY_MED, "Retrieve Authentication Data - MN-AAA");

    /* Retrieve the data */
    ds3g_mmgsdi_mip_data_retrieve( authenticator ); 

  }

  /*-------------------------------------------------------------------------
    Return the authenticator length
  -------------------------------------------------------------------------*/
  return (MIP_AUTH_MD5_LEN);

} /* mip_auth_calc_chap() */


/*===========================================================================

FUNCTION MIP_AUTH_RETRIEVE_SS_DATA()
 
DESCRIPTION
  Determines (and retrieves if necessary) the shared secret data to be
  used when composing the Mobile IP registration request.

PARAMETERS
  mn_ha_ss      - where to store MN-HA shared secret pointer
  mn_ha_ss_len  - where to store length for above
  mn_aaa_ss     - where to store MN-HA shared secret pointer
  mn_aaa_ss_len - where to store length for above

RETURN VALUE
  mn_ha_ss      - set to point to MN-HA shared secret
  mn_ha_ss_len  - set to length of above
  mn_aaa_ss     - set to point to MN-AAA shared secret
  mn_aaa_ss_len - set to length of above

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies global ps_nv_item 

===========================================================================*/
void mip_auth_retrieve_ss_data
(
  mip_session_info_type * session,
  byte ** mn_ha_ss, 
  byte *  mn_ha_ss_len, 
  byte ** mn_aaa_ss, 
  byte *  mn_aaa_ss_len
)
{
  nv_stat_enum_type  nv_status;            /* return val for ps_nv_read    */

#ifdef FEATURE_DS_MOBILE_IP_DMU
  byte               dmu_ix;
  dmu_info_type      *mip_dmu_info_ptr;
  #endif /* FEATURE_DS_MOBILE_IP_DMU */
  mip_pwd_info_type  *mip_pwd_info_ptr;
  dsmip_shadow_nv_type  *temp_shadow_nv = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  temp_shadow_nv = (dsmip_shadow_nv_type *)dsmip_get_shadow_nv_ptr();
  /*-------------------------------------------------------------------------
    Determine which shared secret data to use for this registration

    In order of priority,
    1. If using shadow NV, use the shadow variables.
    2. If the UI has registered MIP session info and/or password callbacks, 
       they were called in meta_sm_config_session.  Use the values that were
       written to the shared secret profile by this callback.
    3. If in DMU, use the temporary keys stored in mip_dmu_key_data
    4. Otherwise, use the keys stored in the MIP session in NV.
  -------------------------------------------------------------------------*/
  ASSERT (mn_ha_ss != NULL);
  ASSERT (mn_ha_ss_len != NULL);
  ASSERT (mn_aaa_ss != NULL);
  ASSERT (mn_aaa_ss_len != NULL);

  /*-------------------------------------------------------------------------
    Get ma_pwd_info_ptr
  -------------------------------------------------------------------------*/
  mip_pwd_info_ptr = mip_get_pwd_info_ptr();

  /*-------------------------------------------------------------------------
    If UI SS callback is set, use these values
  -------------------------------------------------------------------------*/ 
  if (mip_get_pwd_cb != NULL)
  {
    /*-----------------------------------------------------------------------
      Use the info entered via the UI.  Note that this information is not as 
      secure as the NV info - it remains in RAM while the call is up, since 
      the information is required for re-registration and RRQ retries.
    -----------------------------------------------------------------------*/
    *mn_ha_ss      = mip_pwd_info_ptr->mn_ha_ss;
    *mn_aaa_ss     = mip_pwd_info_ptr->mn_aaa_ss;
    *mn_ha_ss_len  = mip_pwd_info_ptr->mn_ha_ss_len;
    *mn_aaa_ss_len = mip_pwd_info_ptr->mn_aaa_ss_len;
    return;
  }

  /*-------------------------------------------------------------------------
    If shadow NV is set, use these values;
    Otherwise, use NULL passwords unless found elsewhere 
  -------------------------------------------------------------------------*/ 
  if (temp_shadow_nv->mn_aaa_ss.len != 0)
  {
    *mn_aaa_ss     = temp_shadow_nv->mn_aaa_ss.value;
    *mn_aaa_ss_len = temp_shadow_nv->mn_aaa_ss.len;
  }
  else
  {
    *mn_aaa_ss     = NULL;
    *mn_aaa_ss_len = 0;
  }

  if (temp_shadow_nv->mn_ha_ss.len != 0)
  {
    *mn_ha_ss     = temp_shadow_nv->mn_ha_ss.value;
    *mn_ha_ss_len = temp_shadow_nv->mn_ha_ss.len;
  }
  else
  {
    *mn_ha_ss     = NULL;
    *mn_ha_ss_len = 0;
  }
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Shadow NV item length for AAA-SS : %d,"
                "HA-SS : %d",temp_shadow_nv->mn_aaa_ss.len,\
                temp_shadow_nv->mn_ha_ss.len);

#ifdef FEATURE_DS_MOBILE_IP_DMU
  /*-------------------------------------------------------------------------
    If DMU is active, use the temporary key values
  -------------------------------------------------------------------------*/  
  if (session->dmu)
  {
    /*-----------------------------------------------------------------------
      Note, if shadow NV values are active, DMU should have failed and the
      call dropped before arriving here.
    -----------------------------------------------------------------------*/  
    ASSERT (*mn_ha_ss == NULL);
    ASSERT (*mn_aaa_ss == NULL);

    dmu_ix = (byte) MIP_SES_PTR_2_INDEX (session);
    mip_dmu_info_ptr = mip_get_dmu_info_ptr_by_index( dmu_ix );

    *mn_ha_ss      = mip_dmu_info_ptr->key_data.mn_ha_key;
    *mn_ha_ss_len  = MIP_DMU_MN_HA_KEY_LEN;
    *mn_aaa_ss     = mip_dmu_info_ptr->key_data.mn_aaa_key;
    *mn_aaa_ss_len = MIP_DMU_MN_AAA_KEY_LEN;
    return;
  }
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  /*-------------------------------------------------------------------------
    If passwords have not been obtained by other means, 
    get them from NV now. 
  -------------------------------------------------------------------------*/  
  if (!(*mn_aaa_ss) || !(*mn_ha_ss))
  {
    /*-----------------------------------------------------------------------
      Check if the profile is enabled
    -----------------------------------------------------------------------*/
    nv_status = ps_get_nv_item(NV_DS_MIP_ENABLE_PROF_I, &mip_nv_item);
    if( nv_status != NV_DONE_S )
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIP profile enable flag unreadable");
      return;
    }

    /*-----------------------------------------------------------------------
      If profile is disabled, return and use NULL password
    -----------------------------------------------------------------------*/  
    if (!mip_nv_item.ds_mip_enable_prof[session->profile_index])
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Profile %d disabled, using NULL p/w");
      return;
    }
 
    /*-----------------------------------------------------------------------
      Read the shared secret data from NV.  

      JD - looks like a bug in nvmio.c:5176,5177, size is too big 
         (incl. index) (ref nv.h, nv_ds_mip_gen_user_prof_type)
    -----------------------------------------------------------------------*/
    mip_nv_item.ds_mip_ss_user_prof.index = session->profile_index;
    nv_status = ps_get_nv_item(NV_DS_MIP_SS_USER_PROF_I, &mip_nv_item);
    if( nv_status != NV_DONE_S)
    {
      /*---------------------------------------------------------------------
        If NV is not written, we send 0 len shared secrets.
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "SS profile %d unreadable, using NULL p/w",
                 session->profile_index);
      return;
    }

    /*-----------------------------------------------------------------------
      If we're not using shadow NV for the MN-AAA SS, use the NV value
    -----------------------------------------------------------------------*/
    if (!(*mn_aaa_ss))
    {
      *mn_aaa_ss = 
        (byte *) mip_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret;
      *mn_aaa_ss_len = 
        mip_nv_item.ds_mip_ss_user_prof.mn_aaa_shared_secret_length;
    }

    /*-----------------------------------------------------------------------
      If we're not using shadow NV for the MN-HA SS, use the NV value
    -----------------------------------------------------------------------*/
    if (!(*mn_ha_ss))
    {
      *mn_ha_ss = 
        (byte *) mip_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret;
      *mn_ha_ss_len = 
        mip_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length;
    }
  } /* endif (unresolved shared secret) */
} /* mip_auth_retrieve_ss_data */


#endif  /* FEATURE_DS_MOBILE_IP */

