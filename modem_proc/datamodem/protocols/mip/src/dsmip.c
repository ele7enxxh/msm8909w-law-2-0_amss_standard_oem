/*===========================================================================

                         D S M I P . C

DESCRIPTION

 The Data Services Mobile IP configuration source file.

EXTERNALIZED FUNCTIONS

   mip_config_session()
     Load MIP session defaults from NV to all sessions.
   mip_reg_ses_info_cb()
     Register a callback to fill in MIP session information
   mip_reg_pwd_info_cb()
     Register a callback to fill in passwords for MIP session
   mip_get_rm_nai()
     Retrieves the NAIs for a given MIP profile for the
     tethered device and stores them in the mip session info.


Copyright (c) 2000-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.c_v   1.5   19 Feb 2003 15:54:24   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
07/31/13    sd     Fix to ensure DMU random key is generated before MIP call.
06/27/13    cx     replace memcpy and memmove with memscpy and memsmove
06/25/13    sd     Boundary check for 707-mip profile variables.
06/03/13    sd     ATCoP decoupling changes
05/01/12    sb     Always On UIM Feature Cleanup
05/04/12    jee    Mainlined FEATURE_UIM_SUPPORT_HOTSWAP
03/26/12    vpk    Use new core apis for tcb
02/06/12    sn     Removed setting of MIP RRP code to SUCCESS in MIP event
                   callback function as RRP code needs to be retained.
01/24/12    msh    Retain MIP timer handles on card refresh or error
05/20/11    op     Migrated to MSG 2.0 macros
05/04/11    ttv    Added changes to compute mip statistics.
04/11/11    ttv    Added support for Hot Swap feature.
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free
                   Floating environment.
10/07/10    ttv    Added messages which explain the reason for asserting.
07/28/10    ttv    Merged the fix for the issue of not reading MIP active
                   profile index from NV item NV_DS_MIP_ACTIVE_PROF_I during
                   call establishment.
01/11/10    ms     Replaced UIM_3GPD Constants with NVRUIM_3GPD
01/06/10    ls     Add change back into mip_reg_ses_info_cb() since it has
                   been removed during SU API featurization
11/17/09    ls     Ensure mip_event_client_cnt set to 1 session correctly
10/15/09    vs     SU Level Api Defeaturization
09/04/09    ss     CMI SU level modifications.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
04/23/09    ls     Add the support to report MIP rrp code
10/17/08    cs     Lint error fixes.
08/07/08    as     Lint error fixes.
08/01/08    mv/sa  Added support for DS NAA refresh.
06/05/08    ms     Added MIP_DEREGED_EV_MASK to supported events in mip_event_register()
05/23/07    vp     MIP CCoA related changes.
12/07/06    as     Added support for new NV item for MIP deregistration
11/13/06    ks     Added mip_get_rm_nai() to retrieve RM_NAI from nv.
07/22/05/   kvd    Add support for MIP event generation& report for DCTM;
                   mip_call_event_callback,mip_event_alloc,mip_event_register
04/25/05    kvd    Optimize MIP settings in mipinv_get_info() after MIP
                   callbacks are called so that we have latest mip session
                   info following changes in the callbacks.
02/07/05    kvd    Fixed mip_reg_ses_info_cb to allow deregistration via NULL
                   callback function.
09/17/04    vp     Removed inclusion of psglobal.h.
06/22/03    kvd    Added new function mip_reset_config.
06/22/03    kvd    Added new function mip_config_valid.
05/18/04    jk     Added initialization of IP parameters for 3GPD support on
                   R-UIM card.
04/28/04    jd     Use new shadow NV structure, linted file
04/15/04    jd/mt  Added mip_get_nv_item to use correct nv_get function
                   depending on which task handles session configuration
02/04/04    jd     Included psglobal.h
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Make sure FEATURE_DATA_MM is defined at compile time
01/20/04    jd     Mainlined Multimode code (multimode only now)
06/19/03    jd     Make sure primary HA holds valid IP address in all cases
06/04/03    jd     Check if both HAs are dynamic even when using shadow NV.
06/03/03    jd     Comment and formatting cleanup.
05/29/03    jd     If profile enable flag can't be read, assume disabled to
                   allow shadow NV settings.
10/09/02    jay    Made sure both Pri and Sec HA are not Dynamic
09/17/02    jd     Changed hard coded dynamic HA address to DYNAMIC_HA_ADDR
07/19/02    jay    Used a temp variable to copy nai and keep the
                   shadow_nv.nai free.
04/29/02    jd     Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "dsmip_api.h"
#include "ds707_pkt_mgr.h"
#include "ds_1x_profile.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "nv.h"
#include "amssassert.h"

#include "ds3gcfgmgr.h"
#include "ps_utils.h"
#include "dsat707mip.h"

#include "dsmip_v.h"
#include "dsmip_metasm.h"
#include "dsmip_regsm.h"
#include "dsmip_parse.h"
#include "dsmip_cfg.h"
#include "dsmip_dmu.h"
#include "rex.h"
#include "nvruimi.h"
#include "ps_system_heap.h"
#include "ps_mip_task.h"

#include "rcinit_rex.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_metasm.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

/*lint -save -e641 Reason for suppressing error 641*/


/*---------------------------------------------------------------------------
  Make sure that the definition of NAI length is reasonable - does this work
  with acc?
---------------------------------------------------------------------------*/
#if MAX_NAI_LEN < NV_MAX_NAI_LENGTH
#error "MAX_NAI_LEN is too small!  Must be at least the same as NV_MAX_NAI_LEN"
#endif


/*===========================================================================
MACRO MIP_INDEX_IS_VALID()

DESCRIPTION
  This macro checks if the mip index is valid for NV read

PARAMETERS

RETURN VALUE

===========================================================================*/
#define MIP_INDEX_IS_VALID(mip_index)  \
  ( mip_index != -1 )


/*===========================================================================

                             EXTERNAL DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  Shadow NV
---------------------------------------------------------------------------*/
dsmip_shadow_nv_type  shadow_nv;

/*---------------------------------------------------------------------------
  Mobile IP session info array
---------------------------------------------------------------------------*/
static mip_session_info_type  mip_ses_info_array[MIP_MAX_SESSIONS];

static mip_event_info_s_type mip_event_buf[MIP_EVENT_CLIENT_MAX];

/*---------------------------------------------------------------------------
   Foreign Agent currently serving the mobile
---------------------------------------------------------------------------*/
static mip_ma_info_type  ma_info;

/*---------------------------------------------------------------------------
  Holds password info, passed to mip_get_pwd_cb.
---------------------------------------------------------------------------*/
static mip_pwd_info_type  mip_pwd_info = { { 0 }, { 0 }, 0, 0 };

static mip_stats_type  mip_stats_info = {0};
/*---------------------------------------------------------------------------
  Callback to retrieve session information - if non-null, this callback
  will be called after mipi_get_nv_info to update the session info block
  for the given session (first parameter).
---------------------------------------------------------------------------*/
void (* mip_get_session_info_cb)(
                                  byte profile_num,
                                  mip_session_info_type * session
                                ) = NULL;

/*---------------------------------------------------------------------------
  Callback to retrieve shared secret information - if non-null,
  this callback will be called to retrieve the user password info for
  the given session (first parameter).  Callback should populate
  referenced mip_pwd_info_type structure.

  Otherwise, NAI and shared secrets will be retrieved from NV.
---------------------------------------------------------------------------*/
void (* mip_get_pwd_cb)(
                         byte profile_num,
                         mip_pwd_info_type * pwd_info
                       ) = NULL;

/*===========================================================================

                        INTERNAL FUNCTION DEFINTIONS

===========================================================================*/
/*===========================================================================
FUNCTION mip_get_ps_tcb_ptr()

DESCRIPTION
   Gets the PS task's tcb pointer

PARAMETERS
   None

RETURN VALUE
  Returns PS tcb ptr
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static rex_tcb_type* mip_get_ps_tcb_ptr(void)
{
   /*---------------------------------------------------------------------------
     TCB pointer for PS task
   ---------------------------------------------------------------------------*/
   rex_tcb_type* tcb_ptr = NULL;

#ifdef FEATURE_MODEM_RCINIT_PHASE2
   tcb_ptr = rcinit_lookup_rextask("ps");
   if ( RCINIT_NULL == tcb_ptr)
   {
      DATA_ERR_FATAL("RCINIT Lookup for ps tcb returned NULL");
   }
#else
   tcb_ptr = &ps_tcb;
#endif
   return tcb_ptr;
}
/*===========================================================================
FUNCTION mip_get_ds_tcb_ptr()

DESCRIPTION
   Gets the DS task's tcb pointer

PARAMETERS
   None

RETURN VALUE
  Returns DS tcb ptr
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static rex_tcb_type* mip_get_ds_tcb_ptr(void)
{
   /*---------------------------------------------------------------------------
     TCB pointer for DS task
   ---------------------------------------------------------------------------*/
   rex_tcb_type* tcb_ptr = NULL;

#ifdef FEATURE_MODEM_RCINIT_PHASE2
   tcb_ptr = rcinit_lookup_rextask("ds");
   if ( RCINIT_NULL == tcb_ptr)
   {
      DATA_ERR_FATAL("RCINIT Lookup for ds tcb returned NULL");
   }
#else
   tcb_ptr = &ds_tcb;
#endif
   return tcb_ptr;
}
/*===========================================================================
FUNCTION MIP_GET_NV_ITEM()

DESCRIPTION
  Wrapper function to call the appropriate NV get function based on current
  task.

PARAMETERS
  item_code:  NV item to retrieve
  data_ptr:   NV item where results of operation are stored

RETURN VALUE
  NV_FAIL_S and ERR fatal if calling task is not DS or PS
  otherwise, the return value from the nv_get

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL nv_stat_enum_type mip_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get                      */
  nv_item_type        *data_ptr        /* Pointer where to put the item    */
)
{
  nv_stat_enum_type nv_status = NV_FAIL_S;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Call appropriate NV get function by current task
  -------------------------------------------------------------------------*/
  // nv_get_fn = rex_self()->nv_get_fn;

  if (rex_self() == mip_get_ds_tcb_ptr())
  {
    nv_status = ds3gcfgmgr_read_legacy_nv( item_code, data_ptr );
  }
  else if (rex_self() == mip_get_ps_tcb_ptr())
  {
    nv_status = ps_get_nv_item( item_code, data_ptr );
  }
  else
  {
    ERR_FATAL ("MIP queried an NV item from some random task context!",0,0,0);
    /* ERR_FATAL will call _exit(1), so the code will never reach next line*/
    /* return NV_FAIL_S; */
  }

  return nv_status;

} /* mip_get_nv_item() */

/*===========================================================================
FUNCTION MIPI_GET_NV_INFO()

DESCRIPTION
  This function retrieves the active Mobile IP profile configuration and
  stores it in the Mobile IP session info block passed by the caller.

  Note: The shared secret information is read, but not stored
        in the control block.

PARAMETERS
  session: pointer to the mip session control block where info is stored

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, the active profile number

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL sint15 mipi_get_nv_info
(
  mip_session_info_type *  session,
  int8 mip_index
)
{
  nv_stat_enum_type nv_status;
  int               loop;
  char *            nai_temp;
  uint16            multiplier;
  nv_item_type     *ps_nv_item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Default values for missing NV items
  -------------------------------------------------------------------------*/
#define MIP_NV_DEFAULT_RRQ_RETRIES  2
#ifdef FEATURE_DS_MOBILE_IP_DEREG
  #define MIP_NV_DEFAULT_DERRQ_RETRIES  0
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
#define MIP_NV_DEFAULT_RETRY_INT    0
#define MIP_NV_DEFAULT_PRE_RRQ_TIME 0

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Check to be sure the a valid session control block was specified
  -------------------------------------------------------------------------*/
  if (session == NULL)
  {
    PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
    DATA_ERR_FATAL( "Invalid session control block");
    return (-1);
  }
  session->valid = FALSE;

  /*-------------------------------------------------------------------------
    If MIP profile id is passed through net policy (151-156) it is converted
    to a corresponding index (mip_index) which is used for NV read,
    else mip_index is the NV_DS_MIP_ACTIVE_PROF_I
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG1(MSG_LEGACY_MED, "MIP profile id %d", mip_index);
  
  if ( MIP_INDEX_IS_VALID(mip_index) )
  {
    session->profile_index = mip_index;
  }
  else
  {
    PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Set default values for session info
  -------------------------------------------------------------------------*/
  session->curr_ha_index = PRIMARY_HA_INDEX;

  /*-------------------------------------------------------------------------
    Retrieve Mobile IP max registration retries from NV - set max RRQs
    variable as this plus one (total attempts is retries + 1.)
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item( NV_DS_MIP_RETRIES_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG1(MSG_LEGACY_HIGH, "MIP max retry unavail:default %d",
             MIP_NV_DEFAULT_RRQ_RETRIES);
    session->rsm_info.max_rrqs = MIP_NV_DEFAULT_RRQ_RETRIES + 1;
  }
  else
  {
    session->rsm_info.max_rrqs = ps_nv_item_ptr->ds_mip_retries + 1;
  }

#ifdef FEATURE_DS_MOBILE_IP_DEREG
  /*-------------------------------------------------------------------------
    Retrieve Mobile IP max deregistration retries from NV - set max deRRQs
    variable as this plus one (total attempts is retries + 1.)
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item( NV_DS_MIP_DEREG_RETRIES_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG1(MSG_LEGACY_HIGH, "MIP max retry unavail:default %d",
             MIP_NV_DEFAULT_DERRQ_RETRIES);
    session->rsm_info.max_derrqs = MIP_NV_DEFAULT_DERRQ_RETRIES + 1;
  }
  else
  {
    session->rsm_info.max_derrqs = ps_nv_item_ptr->ds_mip_retries + 1;
  }
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
  /*-------------------------------------------------------------------------
    Retrieve Mobile IP retry interval from NV.
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item( NV_DS_MIP_RETRY_INT_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG1(MSG_LEGACY_HIGH, "MIP retry int unavail:default %d",
             MIP_NV_DEFAULT_RETRY_INT);
    ps_nv_item_ptr->ds_mip_retry_int = MIP_NV_DEFAULT_RETRY_INT;
  }

  /*-------------------------------------------------------------------------
    The retry interval is a number between 0 and 4 which specified the
    retry interval for RRQs, the corresponding time: N * 250 + 1000ms
  -------------------------------------------------------------------------*/
  session->base_retry_interval = 250*ps_nv_item_ptr->ds_mip_retry_int + 1000;
  session->rsm_info.re_rrq_timeout = session->base_retry_interval;

  /*-------------------------------------------------------------------------
    Calculate the max RRQ time (the time we spend trying to register).

    NOTE: this calculation includes enough time for an extra RRQ
    (i.e. we start with the first attempt, and we add time for max RRQ
    more attempts).  This allows a cushion for re-registration.
  -------------------------------------------------------------------------*/
  session->max_reg_time = session->base_retry_interval;
  multiplier = MIP_RRQ_RETRY_BACKOFF_MULT;
  for(loop = 0; loop < session->rsm_info.max_rrqs; loop += 1)
  {
    session->max_reg_time += (uint16) (session->base_retry_interval *
                                       multiplier);
    multiplier *=  MIP_RRQ_RETRY_BACKOFF_MULT;
  }

  /*-------------------------------------------------------------------------
    Retrieve Mobile IP pre-registration retry interval from NV.
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item( NV_DS_MIP_PRE_RE_RRQ_TIME_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "MIP prereg ret int unavail:default %d",
              MIP_NV_DEFAULT_PRE_RRQ_TIME);
    ps_nv_item_ptr->ds_mip_pre_re_rrq_time = MIP_NV_DEFAULT_PRE_RRQ_TIME;
  }

  /*-------------------------------------------------------------------------
    This value is in minutes: convert to ms - there are 60000 ms/minute
  -------------------------------------------------------------------------*/
  session->rsm_info.pre_re_rrq_time =
    ps_nv_item_ptr->ds_mip_pre_re_rrq_time * 60000;

  /*-------------------------------------------------------------------------
    Retrieve the user preference for sending RRQ only if there is some
    traffic since previous registration. We don't consider it a serious
    error if the NV does not have this value. In such a case, we simply
    default it to false.
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item( NV_DS_MIP_RRQ_IF_TFRK_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "MIP RRQ if traffic unavailable:default");
    session->rrq_if_tfrk = FALSE;
  }
  else
  {
    session->rrq_if_tfrk = ps_nv_item_ptr->ds_mip_rrq_if_tfrk;
  }

  /*-------------------------------------------------------------------------
    Check NV to see if active profile is enabled or disabled
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item(NV_DS_MIP_ENABLE_PROF_I, ps_nv_item_ptr);
  if( nv_status != NV_DONE_S )
  {
    /*-----------------------------------------------------------------------
      If the NV item was never written, assume disabled in case shadow NV
      settings have been entered.
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "MIP enable profile flag array couldn't be retrieved!");
    DATA_MIP_MSG1(MSG_LEGACY_MED, "Assuming profile %d is disabled",session->profile_index);
    ps_nv_item_ptr->ds_mip_enable_prof[session->profile_index] = 0;
  }

  if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
     NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
    {
    /*-----------------------------------------------------------------------
      If not all of the items are available in NV, then all must
      be retrived from the card
    -----------------------------------------------------------------------*/
      if( !shadow_nv.nai.len         ||
          !shadow_nv.mn_aaa_ss.len   ||
          !shadow_nv.mn_ha_ss.len    ||
          !shadow_nv.mn_aaa_spi.set  ||
          !shadow_nv.mn_ha_spi.set   ||
          !shadow_nv.home_addr.set   ||
          !shadow_nv.pri_ha_addr.set ||
          !shadow_nv.sec_ha_addr.set )
      {
        dsmip_erase_shadow_nv();
      }
    }

  /*-------------------------------------------------------------------------
    If the profile is enabled, load profile settings to session info block
    Use, in order of preference:
    - shadow NV settings (set via "AT$QCMIPxxx=...,0" commands)
    - NV settings
  -------------------------------------------------------------------------*/
  if (ps_nv_item_ptr->ds_mip_enable_prof[session->profile_index])
  {
    /*-----------------------------------------------------------------------
      Retrieve Mobile IP general user profile from NV.
    -----------------------------------------------------------------------*/
    ps_nv_item_ptr->ds_mip_gen_user_prof.index = session->profile_index;
    nv_status = mip_get_nv_item( NV_DS_MIP_GEN_USER_PROF_I, ps_nv_item_ptr);
    if( nv_status != NV_DONE_S )
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIP general user profile cannot be retrieved!");
      PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
      return (-1);
    }

    /*-----------------------------------------------------------------------
      Store Mobile IP general user profile items into M.IP session info
      struct. Only copy the secondary HA address if we have memory for it.
    -----------------------------------------------------------------------*/
    if (shadow_nv.home_addr.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Home Add");
      session->home_addr = ps_ntohl(shadow_nv.home_addr.value);
    }
    else
    {
      session->home_addr = ps_ntohl(ps_nv_item_ptr->ds_mip_gen_user_prof.home_addr);
    }

    /*-----------------------------------------------------------------------
      Use shadow NV settings if set
    -----------------------------------------------------------------------*/
    if (shadow_nv.pri_ha_addr.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Pri HA");
      session->ha_addrs[PRIMARY_HA_INDEX] =
        ps_ntohl(shadow_nv.pri_ha_addr.value);
    }
    else
    {
      session->ha_addrs[PRIMARY_HA_INDEX] =
        ps_ntohl(ps_nv_item_ptr->ds_mip_gen_user_prof.primary_ha_addr);
    }

#if MAX_HA_ADDRS > 1
    if(shadow_nv.sec_ha_addr.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Sec HA");
      session->ha_addrs[SECONDARY_HA_INDEX] =
        ps_ntohl(shadow_nv.sec_ha_addr.value);
    }
    else
    {
      session->ha_addrs[SECONDARY_HA_INDEX] =
        ps_ntohl(ps_nv_item_ptr->ds_mip_gen_user_prof.secondary_ha_addr);
    }
#endif /* MAX_HA_ADDRS > 1 */

    /*-----------------------------------------------------------------------
      Check whether to retrieve the NAI  - real or soft NV - If there is no
      shadow NV, then use the value retrieved (use shadow NV pointer value
      so only memscpy() is needed)
    -----------------------------------------------------------------------*/
    if(shadow_nv.nai.len != 0)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for NAI");
      session->nai_len = (byte) strlen((char *) shadow_nv.nai.value);
      nai_temp = (char *) shadow_nv.nai.value;
    }
    else
    {
      nai_temp = (char*) ps_nv_item_ptr->ds_mip_gen_user_prof.nai;
      session->nai_len = ps_nv_item_ptr->ds_mip_gen_user_prof.nai_length;
    }

    /*-----------------------------------------------------------------------
      Only copy NAI if its length is valid.  If NAI length is > max allowed
      assume a memory/flash scribble and set length to 0.
    -----------------------------------------------------------------------*/
    if(session->nai_len > NV_MAX_NAI_LENGTH)
    {
      session->nai_len = 0;
    }
    else if(session->nai_len > 0)
    {
      memscpy(session->nai,MAX_NAI_LEN, nai_temp, session->nai_len);
      DATA_MIP_MSG1(MSG_LEGACY_LOW, " session->nai_len is %d",session->nai_len);
    }

    /*-----------------------------------------------------------------------
      Set the MN-AAA and MN-HA SPI values - real or soft NV
    -----------------------------------------------------------------------*/
    if (shadow_nv.mn_aaa_spi.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for AAA-SPI");
      session->mn_aaa_spi = shadow_nv.mn_aaa_spi.value;
    }
    else
    {
      if(!ps_nv_item_ptr->ds_mip_gen_user_prof.mn_aaa_spi_set &&
         (NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
          NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        DATA_MIP_MSG0(MSG_LEGACY_HIGH, "MN-AAA SPI is not provisioned on the card.  USING 2");
        session->mn_aaa_spi = MIP_CHAP_SPI;
      }
      else
      {
        session->mn_aaa_spi = ps_nv_item_ptr->ds_mip_gen_user_prof.mn_aaa_spi;
      }
    }

    if (shadow_nv.mn_ha_spi.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for HA-SPI");
      session->mn_ha_spi = shadow_nv.mn_ha_spi.value;
    }
    else
    {
      if(!ps_nv_item_ptr->ds_mip_gen_user_prof.mn_ha_spi_set &&
         (NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
          NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support()))
      {
        DATA_MIP_MSG0(MSG_LEGACY_HIGH, "MN-HA SPI is not provisioned on the card.  USING 3");
        session->mn_ha_spi = MIP_MD5_SPI;
      }
      else
      {
        session->mn_ha_spi = ps_nv_item_ptr->ds_mip_gen_user_prof.mn_ha_spi;
      }
    }

    /*-----------------------------------------------------------------------
      Set the Rev Tunneling preference - real or soft NV
    -----------------------------------------------------------------------*/
    if (shadow_nv.rev_tun.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Rev Tun");
      session->mn_request_rev_tun = shadow_nv.rev_tun.value;
    }
    else
    {
      session->mn_request_rev_tun =
        ps_nv_item_ptr->ds_mip_gen_user_prof.rev_tun_pref;
    }

    /*-----------------------------------------------------------------------
      If Mobile IP shared secret user profile has not been written to NV,
      log an error. - except when using the RUIM for MIP
    -----------------------------------------------------------------------*/

   /*------------------------------------------------------------------------
      If FALLBACK for 3GPD R-UIM is disabled and the card doesn't support MIP
      uim_3gpd_control will return UIM_3GPD_MIP_RUIM_SIP_RUIM.  However, the
      NV read for GEN_USER_PROF above will fail, and will cause this function
      to fail above
      ---------------------------------------------------------------------*/
    if(NVRUIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
       NVRUIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
    {
      ps_nv_item_ptr->ds_mip_ss_user_prof.index = session->profile_index;
      nv_status = mip_get_nv_item( NV_DS_MIP_SS_USER_PROF_I, ps_nv_item_ptr);
      if( nv_status != NV_DONE_S )
      {
        DATA_MIP_MSG0(MSG_LEGACY_ERROR, "MIP shared secret profile cannot be retrieved!");
        PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
        return (-1);
      }
    }
  }
  else /* the profile is disabled */
  {
    /*-----------------------------------------------------------------------
      If the profile is disabled, use (in order of preference):
      - shadow NV settings (set via "AT$QCMIPxxx=...,0" commands)
      - session info defaults
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_MED, "Profile %d is disabled",session->profile_index);

    /*-----------------------------------------------------------------------
      Load NAI
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.nai.len != 0)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for NAI");
      session->nai_len = (byte) strlen( (char *) shadow_nv.nai.value);
      if(session->nai_len > NV_MAX_NAI_LENGTH)
      {
       session->nai_len = 0;
      }
      else if(session->nai_len > 0)
      {
        memscpy( session->nai,
                 MAX_NAI_LEN, 
                 shadow_nv.nai.value, 
                 session->nai_len);
      }
    }
    else
    {
      session->nai_len = 0;
    }

    /*-----------------------------------------------------------------------
      Load Mobile's Home IP address
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.home_addr.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Home Add");
      session->home_addr = ps_ntohl(shadow_nv.home_addr.value);
    }
    else
    {
      session->home_addr = 0;
    }

    /*-----------------------------------------------------------------------
      Load Primary and Secondary Home Agent IP addresses
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.pri_ha_addr.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Pri HA");
      session->ha_addrs[PRIMARY_HA_INDEX] = ps_ntohl(shadow_nv.pri_ha_addr.value);
    }
    else
    {
      session->ha_addrs[PRIMARY_HA_INDEX] = DYNAMIC_HA_ADDR;
    }

#if MAX_HA_ADDRS > 1
    if(shadow_nv.sec_ha_addr.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Sec HA");
      session->ha_addrs[SECONDARY_HA_INDEX] = ps_ntohl(shadow_nv.sec_ha_addr.value);
    }
    else
    {
      session->ha_addrs[SECONDARY_HA_INDEX] = UNSET_HA_ADDR;
    }
#endif /* MAX_HA_ADDRS > 1 */

    /*-----------------------------------------------------------------------
      Load MN_AAA and MN_HA SPI values
      Use shadow NV value if defined, else set to default.
     ----------------------------------------------------------------------*/
    if (shadow_nv.mn_aaa_spi.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for AAA-SPI");
      session->mn_aaa_spi = shadow_nv.mn_aaa_spi.value;
    }
    else
    {
      session->mn_aaa_spi = MIP_CHAP_SPI;
    }

    if (shadow_nv.mn_ha_spi.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for HA-SPI");
      session->mn_ha_spi = shadow_nv.mn_ha_spi.value;
    }
    else
    {
      session->mn_ha_spi = MIP_MD5_SPI;
    }

    /*-----------------------------------------------------------------------
      Load reverse tunneling preference
      Use shadow NV value if defined, else set to default.
    -----------------------------------------------------------------------*/
    if (shadow_nv.rev_tun.set)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Using shadow NV item for Rev Tun");
      session->mn_request_rev_tun = shadow_nv.rev_tun.value;
    }
    else
    {
      session->mn_request_rev_tun = 0;
    }
  } /* else if profile is disabled */

  session->valid = TRUE;

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);
  return (session->profile_index);

} /* mipi_get_nv_info() */


/*===========================================================================
FUNCTION MIPI_OPTIMIZE_MIP_SETTINGS()

DESCRIPTION
  This function otpimizes MIP settimgs for default and unset cases.

PARAMETERS
  session: pointer to the mip session control block where info is stored

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, 0

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL sint15 mipi_optimize_mip_settings
(
  mip_session_info_type *session
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Optimization Heuristics
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    If all HAs are unset, set the primary HA to dynamic.  Since the
    mobile is configured to try MIP, this is a best effort heuristic.
  -------------------------------------------------------------------------*/
  if (session->ha_addrs[PRIMARY_HA_INDEX] == UNSET_HA_ADDR)
  {
#if MAX_HA_ADDRS > 1
    if (session->ha_addrs[SECONDARY_HA_INDEX] != UNSET_HA_ADDR)
    {
      session->ha_addrs[PRIMARY_HA_INDEX] =
        session->ha_addrs[SECONDARY_HA_INDEX];
      session->ha_addrs[SECONDARY_HA_INDEX] = UNSET_HA_ADDR;
    }
    else /* secondary HA is unset as well */
#endif /* MAX_HA_ADDRS > 1 */
    {
      session->ha_addrs[PRIMARY_HA_INDEX] = DYNAMIC_HA_ADDR;
    }
  }

  /*-------------------------------------------------------------------------
    Check for error conditions.
  -------------------------------------------------------------------------*/
#if MAX_HA_ADDRS > 1
  /*-------------------------------------------------------------------------
    Primary and Secondary HAs cannot both be dynamic.
    If so, reset secondary to UNSET.
  -------------------------------------------------------------------------*/
  if ( session->ha_addrs[PRIMARY_HA_INDEX] == DYNAMIC_HA_ADDR &&
       session->ha_addrs[SECONDARY_HA_INDEX] == DYNAMIC_HA_ADDR )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Changing sec HA to UNSET, Pri and Sec HA are both Dynamic");
    session->ha_addrs[SECONDARY_HA_INDEX] = UNSET_HA_ADDR;
  }
#endif /* MAX_HA_ADDRS > 1 */

  /*-------------------------------------------------------------------------
    One of NAI and Home address must be known
    If not, flag an error and fail configuration
  -------------------------------------------------------------------------*/
  if (session->home_addr == DYNAMIC_HOME_ADDR &&
      session->nai_len == 0)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Home address and NAI length are both 0");
    session->valid = FALSE;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set the session info valid flag and return
  -------------------------------------------------------------------------*/
  session->valid = TRUE;
  return 0;
}  /* mipi_optimize_mip_settings() */

/*===========================================================================

                        EXTERNAL FUNCTION DEFINTIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_CONFIG_SESSION()

DESCRIPTION
  Load MIP session defaults from NV to all sessions. Note: this function
  be invoked before the traffic channel is established.

PARAMETERS
  None

RETURN VALUE
  TRUE: No errors encoutered
  FALSE: otherwise

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_config_session ( int8 mip_index )
{
  int    loop;
  sint15 profile_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Load MIP session defaults from NV

    If multiple MIP sessions are allowed simultaneously,
    we will loop through to configure each session.
  -------------------------------------------------------------------------*/
  for (loop=0; loop < MIP_MAX_SESSIONS; loop++)
  {
    /*---------------------------------------------------------------------
      Configure session #'loop'.
      Config function will return the profile being used for this session.
    ---------------------------------------------------------------------*/
    profile_num = mipi_get_nv_info(INDEX_2_MIP_SES_PTR(loop), mip_index);
    if (profile_num == -1)
    {
      /*---------------------------------------------------------------------
         NV get failed
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                    "Errors encountered while retrieving NV info!");
      return FALSE;
    }

    /*-----------------------------------------------------------------------
      If upper level software has registered a callback to
      populate/modify the session and/or password info, call it now.

      We retrieve the session info from NV first to allow UI to use
      modifications to NV settings if desired.
    -----------------------------------------------------------------------*/
    if (mip_get_session_info_cb != NULL)
    {
      mip_get_session_info_cb((byte) profile_num, INDEX_2_MIP_SES_PTR(loop));
    }

    if (mip_get_pwd_cb != NULL)
    {
      mip_get_pwd_cb((byte) profile_num, &mip_pwd_info);
    }

    if(mipi_optimize_mip_settings((INDEX_2_MIP_SES_PTR(loop)))<0)
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                    "Errors encountered while retrieving NV info!");
      return FALSE;
    }
  }

#ifdef FEATURE_DS_MOBILE_IP_DMU
  DATA_MIP_MSG0( MSG_LEGACY_HIGH, "Setting signal to generate random key." );
  mip_task_rand_data_ready();
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  return TRUE;

} /* mip_config_session() */

/*===========================================================================
FUNCTION MIP_REG_PWD_INFO_CB()

DESCRIPTION
  Registers a user callback to provide MIP password info.

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_pwd_info_type * pwd_info )

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_pwd_info_cb
(
  void (* callback)( byte profile_num, mip_pwd_info_type * pwd_info )
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Save the pointer to callback
  -------------------------------------------------------------------------*/
  if (callback != NULL)
  {
    mip_get_pwd_cb = callback;
  }
} /* mip_reg_pwd_info_cb() */

/*===========================================================================
FUNCTION MIP_CONFIG_VALID()

DESCRIPTION
  Check if MIP session is configured currently

PARAMETERS
  None

RETURN VALUE
  Flag  indicating config status of MIP

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_config_valid
(
  void
)
{
  return(mip_ses_info_array[0].valid);
} /* mip_config_valid() */

/*===========================================================================
FUNCTION MIP_RESET_CONFIG()

DESCRIPTION
  Reset mip configuration at end of MIP call, so that subsequent calls can
  do mip_config

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_reset_config
(
  void
)
{
  mip_ses_info_array[0].valid = FALSE;
  mip_reg_ses_info_cb( NULL );
} /* mip_reset_config() */

/*===========================================================================
FUNCTION MIP_CALL_EVENT_CALLBACK()

DESCRIPTION
  Traverses the MIP event queue and calls the passed event.

  NOTE: The callback function should not dealloc the event notification
        structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Generates MIP events
===========================================================================*/
void mip_call_event_callback
(
  mip_event_e_type    event
)
{
  mip_event_info_s_type    *event_ptr;
  mip_session_info_type    *mip_session;
  uint8 event_mask = (1<<event);
  mip_fail_reason_e_type   fail_reason = MIP_FAIL_REASON_NONE;
  byte                     i=0;
  byte                     mip_rrp_code = MIP_RRP_CODE_SUCCESS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Verify arguments
  -------------------------------------------------------------------------*/
  if( MIP_MAX_EV <= event )
  {
    DATA_MIP_MSG1(MSG_LEGACY_ERROR,  "Invalid args passed. event: %d",
               event);
    ASSERT( 0 );
    return;
  }

  mip_session = &mip_ses_info_array[0];

  /*-------------------------------------------------------------------------
    Traverse queue of MIP event info structures
  -------------------------------------------------------------------------*/

  for (i=0;i<mip_session->mip_event_client_cnt;i++)
  {
    event_ptr = &mip_event_buf[i];

    if( 0 != (event_mask & event_ptr->event_mask))
    {
      if (MIP_FAILURE_EV == event)
      {
        fail_reason = mip_session->mip_fail_reason;
        mip_session->mip_fail_reason = MIP_FAIL_REASON_NONE;
        mip_rrp_code = mip_session->mip_rrp_code;
      }
      else
      {
        fail_reason = MIP_FAIL_REASON_NONE;
        mip_rrp_code = MIP_RRP_CODE_SUCCESS;
      }
      event_ptr->cback( event,
                        fail_reason,
                        mip_rrp_code,
                        event_ptr->user_data
                       );
    }
  }

  return;
} /* mip_call_event_callback() */

/*===========================================================================
FUNCTION MIP_EVENT_ALLOC()

DESCRIPTION
  This function will allocate a MIP event information structure and
  register callback functions.

PARAMETERS
  mip_cb_f:         callback function to call
  user_data:        user data to be returned with the callback fn.

RETURN VALUE
  NULL on error
  handle to the MIP event information structure on success

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a MIP event information structure and places it on the event
  queue.  Initializes event masks and registers callbacks for MIP protocol.
===========================================================================*/
void *mip_event_alloc
(
  mip_event_cback_f_ptr_type  mip_cb_f,
  void                        *user_data
)
{
  mip_event_info_s_type *event_info_ptr = NULL;
  mip_session_info_type           *mip_session;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check  that the callback is not NULL.
  -------------------------------------------------------------------------*/
  if( NULL == mip_cb_f )
  {
    DATA_ERR_FATAL("NULL functions passed");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Get default MIP session
  -------------------------------------------------------------------------*/
  mip_session = &mip_ses_info_array[0];


  if(  mip_session->mip_event_client_cnt >= MIP_EVENT_CLIENT_MAX)
  {
    DATA_ERR_FATAL("Couldn't allocate event info structure");
  }
  else
  {
    /*-------------------------------------------------------------------------
      Get a new event information structure
    -------------------------------------------------------------------------*/
    /* Right now, only one MIP session is supported */
    mip_session->mip_event_client_cnt = 0;
    event_info_ptr =  &mip_event_buf[mip_session->mip_event_client_cnt++];

    /*-----------------------------------------------------------------------
      Initialize the event information structure
    -----------------------------------------------------------------------*/
    memset( event_info_ptr, 0, sizeof(mip_event_info_s_type) );

    /*-----------------------------------------------------------------------
      Fill the structure with the passed callbacks and user_data cookies
    -----------------------------------------------------------------------*/
    event_info_ptr->cback             = mip_cb_f;
    event_info_ptr->user_data         = user_data;

    DATA_MIP_MSG2( MSG_LEGACY_MED,  "Event Allocation for cback 0x%x, user_data 0x%x",
                   mip_cb_f, user_data );

  }

  return event_info_ptr;
} /* mip_event_alloc() */

/*===========================================================================
FUNCTION MIP_EVENT_REGISTER()

DESCRIPTION
  This function will add to an event mask for a  MIP session.
  It will  generate an UP event if the event mask is registering for an UP
  event and the UP event was previously not registerd for this protocol and
  the associated protocol is already up.

PARAMETERS
  handle      PPP event structure to add this event mask to
  event_mask  event mask to add

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  Adds the passed event mask to the already registered event mask
===========================================================================*/
int32 mip_event_register
(
  void                  *handle,
  mip_event_mask_e_type  event_mask
)
{
  mip_event_info_s_type *event_info_ptr = handle;
  /*lint -save -e655*/
  uint8 supported_events = MIP_SUCCESS_EV_MASK |
                                            MIP_FAILURE_EV_MASK;
  /*lint -restore error 655*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DS_MOBILE_IP_DEREG
  supported_events  = supported_events | MIP_DEREGED_EV_MASK;
#endif /* FEATURE_DS_MOBILE_IP_DEREG */
  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle ||
      0 == (supported_events & event_mask) )
  {
    /*lint -save -e507 */
    DATA_MIP_MSG2(MSG_LEGACY_ERROR,  "invalid args: handle 0x%x, event_mask 0x%x",
               handle, event_mask);
    /*lint -restore */
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    If registering a mask for a protocol for which no callback is registerd
    return error.
  -------------------------------------------------------------------------*/
  if( NULL == event_info_ptr->cback )
  {
    DATA_ERR_FATAL("Registering event for a protocol with NULL callback");
    return -1;
  }

  DATA_MIP_MSG2(MSG_LEGACY_MED,  "Registering event for handle 0x%x, event_mask 0x%x",
                handle, event_mask);
  /*-------------------------------------------------------------------------
    Set the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  /*lint -save -e655  */
  event_info_ptr->event_mask |= event_mask;
  /*lint -restore for error 655*/
  return 0;
} /* mip_event_register() */
/*===========================================================================
FUNCTION MIP_EVENT_DEREGISTER_ALL()

DESCRIPTION
  This function will deregister all events for a  MIP session.

PARAMETERS
  handle      deregister all events to this handle

RETURN VALUE
  -1 on error
   0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 mip_event_deregister_all
(
  void                  *handle
)
{
  mip_event_info_s_type *event_info_ptr = handle;
  mip_session_info_type           *mip_session;

  /*-------------------------------------------------------------------------
    Verify that the passed arguments are valid
  -------------------------------------------------------------------------*/
  if( NULL == handle )
  {
    /*lint -save -e507 */
    DATA_MIP_MSG1( MSG_LEGACY_ERROR, "invalid args: handle 0x%x", handle );
    /*lint -restore */
    ASSERT( 0 );
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get default MIP session
  -------------------------------------------------------------------------*/
  mip_session = &mip_ses_info_array[0];

  mip_session->mip_event_client_cnt = 0;

  /*-------------------------------------------------------------------------
   Clear the event_mask for the event information structure
  -------------------------------------------------------------------------*/
  /*lint -save -e655  */
  event_info_ptr->event_mask = MIP_INIT_EV_MASK;
  /*lint -restore for error 655*/

  DATA_MIP_MSG1( MSG_LEGACY_MED,  "Deregister for mip client_cnt %d",
                 mip_session->mip_event_client_cnt );

  return 0;
} /* mip_event_deregister_all() */

#ifdef FEATURE_DATA_PS_MIP_CCOA
/*===========================================================================
FUNCTION MIP_GET_CURR_HA_ADDR()

DESCRIPTION
  This function retrieves the current Home Address of the MIP session and
  returns it.

PARAMETERS
  void

RETURN VALUE
  uint32 - Current HA address

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 mip_get_curr_ha_addr
(
  void
)
{
  mip_session_info_type  *mip_session;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Get default MIP session
  -------------------------------------------------------------------------*/
  mip_session = &mip_ses_info_array[0];

  return (mip_session->ha_addrs[mip_session->curr_ha_index]);
}/* mip_get_curr_ha_addr() */
#endif /* FEATURE_DATA_PS_MIP_CCOA */

#ifdef FEATURE_MMGSDI

/*==============================================================================
FUNCTION: DSMIP_READ_MIP_NV_ITEMS

DESCRIPTION:
  This function reads the MIP related NV Items NV_DS_MIP_RETRIES_I,
  NV_DS_PRE_RE_RRQ_TIME_I and NV_DS_MIP_RETRY_INT_I that needs to be
  read during MMGSDI_CARD_INIT_COMPLETED.
  All three are under the EF MMGSDI_CDMA_3GPD_MIPUPP which is call
  stack related.

PARAMETERS
  void

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void dsmip_read_mip_nv_items(void)
{
  nv_item_type            ds_nv_item;
  nv_stat_enum_type       nv_status;
  /*NV_DS_MIP_RETRIES_I*/
  nv_status = ds3gcfgmgr_read_legacy_nv( NV_DS_MIP_RETRIES_I , &ds_nv_item);
  if(nv_status == NV_DONE_S)
  {
    mip_ses_info_array[0].rsm_info.max_rrqs = ds_nv_item.ds_mip_retries + 1;
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "NV_DS_MIP_RETRIES_I Read Failed Using Default");
    mip_ses_info_array[0].rsm_info.max_rrqs = MIP_NV_DEFAULT_RRQ_RETRIES + 1;
  }

  /*NV_DS_PRE_RE_RRQ_TIME_I */
  nv_status = ds3gcfgmgr_read_legacy_nv(NV_DS_MIP_PRE_RE_RRQ_TIME_I, &ds_nv_item);
  if(nv_status == NV_DONE_S)
  {
    mip_ses_info_array[0].rsm_info.pre_re_rrq_time =
      ds_nv_item.ds_mip_pre_re_rrq_time * 60000;
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "NV_DS_MIP_PRE_RE_RRQ_TIME_I read Failed, using Default");
    mip_ses_info_array[0].rsm_info.pre_re_rrq_time = MIP_NV_DEFAULT_PRE_RRQ_TIME;
  }

  /*NV_DS_MIP_RETRY_INT_I*/
  nv_status = ds3gcfgmgr_read_legacy_nv(NV_DS_MIP_RETRY_INT_I, &ds_nv_item);
  if(nv_status == NV_DONE_S)
  {
    mip_ses_info_array[0].base_retry_interval = 250 * ds_nv_item.ds_mip_retry_int+ 1000;
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR,
                  "NV_DS_MIP_RETRY_INT_I Read Failed using Default");
    mip_ses_info_array[0].base_retry_interval = MIP_NV_DEFAULT_RETRY_INT;
  }
}/*dsmip_read_mip_nv_items*/

/*==============================================================================
FUNCTION: DSMIP_RESET_MIP_NV_ITEMS

DESCRIPTION:
  This function reset the mip_ses_info_array information.
  Note that we reset everything except the timer handles. These timers must be
  retained. They are set during power-up and will be re-used in every MIP call.

PARAMETERS
  void

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void dsmip_reset_mip_nv_items(void)
{

  ps_timer_handle_type life_timer_temp = mip_ses_info_array[0].rsm_info.life_timer;
                                              /* Registration lifetime timer handle */
  ps_timer_handle_type reg_timer_temp = mip_ses_info_array[0].rsm_info.reg_timer;
                                               /* Registration timeout timer handle  */
  ps_timer_handle_type dereg_timer_temp = mip_ses_info_array[0].rsm_info.dereg_timer;
                                              /* Deregistration timeout timer handle */
  ps_timer_handle_type rrq_holdoff_timer_temp = mip_ses_info_array[0].rsm_info.rrq_holdoff_timer;
                                              /* RRQ holdoff (1s) timer handle      */
  /*-------------------------------------------------------------------------
  The timer handles need to retained and will be used during subsequent calls.
  -------------------------------------------------------------------------*/

  DATA_MIP_MSG0(MSG_LEGACY_HIGH,
                "dsmip_reset_mip_nv_items: reset mip ses info");

  memset(mip_ses_info_array, 0, sizeof(mip_session_info_type)*MIP_MAX_SESSIONS);

/*-------------------------------------------------------------------------
Restore the timer handles
-------------------------------------------------------------------------*/
  mip_ses_info_array[0].rsm_info.life_timer = life_timer_temp;
  mip_ses_info_array[0].rsm_info.reg_timer = reg_timer_temp;
  mip_ses_info_array[0].rsm_info.dereg_timer = dereg_timer_temp;
  mip_ses_info_array[0].rsm_info.rrq_holdoff_timer = rrq_holdoff_timer_temp;
  mip_ses_info_array[0].rsm_info.one_second_since_last_rrq = TRUE;

} /* dsmip_reset_mip_nv_items */
#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION MIP_GET_ACTIVE_PROFILE_INDEX_FROM_NVRUIM()

DESCRIPTION
  This function retrieves Mobile IP active profile index from NV/RUIM.

PARAMETERS
  None

RETURN VALUE
  -1 if MIP active profile index couldn't be read.
  Otherwise, the active profile index 0-5.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int8 mip_get_active_profile_index_from_nvruim
(
  void
)
{
  nv_stat_enum_type nv_status;
  nv_item_type      nv_item;
  int8              mip_profile_index = -1;

  /*-------------------------------------------------------------------------
    Retrieve MIP active profile index from NV/RUIM.
  -------------------------------------------------------------------------*/
  nv_status = mip_get_nv_item(NV_DS_MIP_ACTIVE_PROF_I, &nv_item);
  if( nv_status == NV_DONE_S )
  {
    mip_profile_index = nv_item.ds_mip_active_prof;
    DATA_MIP_MSG1( MSG_LEGACY_HIGH, "Active MIP profile index read is %d.",
                   mip_profile_index );
  }
  else
  {
    DATA_MIP_MSG0( MSG_LEGACY_ERROR,
                   "Active MIP profile index couldn't be read" );
  }

  return mip_profile_index;
}

/*lint -restore Restore lint error 641*/

/*============================================================================
FUNCTION MIP_GET_SES_INFO_ARRAY

DESCRIPTION
  Access function for mip_ses_info_array.

PARAMETERS
  None

RETURN VALUE
  Pointer to mip_ses_info_array.

DEPENDENCIES
  None

============================================================================*/
mip_session_info_type* mip_get_ses_info_array(void)
{
  return mip_ses_info_array;
}

/*============================================================================
FUNCTION MIP_GET_MA_INFO_PTR

DESCRIPTION
  Access function for ma_info.

PARAMETERS
  None

RETURN VALUE
  Pointer to ma_info.

DEPENDENCIES
  None

============================================================================*/
mip_ma_info_type* mip_get_ma_info_ptr(void)
{
  return &ma_info;
}

/*============================================================================
FUNCTION MIP_GET_PWD_INFO_PTR

DESCRIPTION
  Access function for mip_pwd_info.

PARAMETERS
  None

RETURN VALUE
  Pointer to mip_pwd_info.

DEPENDENCIES
  None

============================================================================*/
mip_pwd_info_type* mip_get_pwd_info_ptr(void)
{
  return &mip_pwd_info;
}

/*============================================================================
FUNCTION MIP_INCREMENT_MIP_STAT_INFO

DESCRIPTION
  Function to increment the particular stat value.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None
============================================================================*/
void mip_increment_mip_stat_info
(
  mip_stats_identifier ident
)
{
  switch(ident)
  {
    case RRP_CNT_ID:
      mip_stats_info.rrp_cnt++;
      break;
    case RRQ_CNT_ID:
      mip_stats_info.rrq_cnt++;
      break;
    case SOL_CNT_ID:
      mip_stats_info.sol_cnt++;
      break;
    case ADV_CNT_ID:
      mip_stats_info.adv_cnt++;
      break;
    default:
      DATA_MIP_MSG1( MSG_LEGACY_HIGH, "Invalid Identifier passed %d.", ident);
      break;
  }
} /* mip_increment_mip_stat_info */

/*============================================================================
FUNCTION MIP_GET_MIP_STAT_INFO

DESCRIPTION
  Function to retrive the value form stat_val based on identifier value.

PARAMETERS
  None

RETURN VALUE
  uint32 - stat value.

DEPENDENCIES
  None
============================================================================*/
uint32 mip_get_mip_stat_info
(
  mip_stats_identifier ident
)
{
  uint32  stat_val = 0;

  switch(ident)
  {
    case RRP_CNT_ID:
      stat_val = mip_stats_info.rrp_cnt;
      break;
    case RRQ_CNT_ID:
      stat_val = mip_stats_info.rrq_cnt;
      break;
    case SOL_CNT_ID:
      stat_val = mip_stats_info.sol_cnt;
      break;
    case ADV_CNT_ID:
      stat_val = mip_stats_info.adv_cnt;
      break;
    default:
      DATA_MIP_MSG1( MSG_LEGACY_HIGH, "Invalid Identifier passed %d.", ident);
      break;
  }

  return stat_val;
} /* mip_get_mip_stat_info */

/*===========================================================================
FUNCTION MIP_GET_MA_ADDR()

DESCRIPTION: Get current Mobility Agent address of the MIP session

PARAMETERS: None

RETURN VALUE: uint32 - MA address
===========================================================================*/
uint32 mip_get_ma_addr
(
  void
)
{
  mip_ma_info_type* mip_ma_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mip_ma_info_ptr = mip_get_ma_info_ptr();
  if (NULL == mip_ma_info_ptr)
  {
    DATA_ERR_FATAL("mip_ma_info_ptr is NULL");
    return 0;
  }

  return (mip_ma_info_ptr->ma_addr);
} /* mip_get_ma_addr() */

/*===========================================================================
FUNCTION DS_QCMIP_UPDATE

DESCRIPTION
 Called by $QCMIP command and refresh event handler to update
 values associated with NV_DS_QCMIP_I.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if success
  FALSE: if failure

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_qcmip_update
(
  boolean read_flag
)
{
  nv_stat_enum_type status;          /* status from NV call */
  nv_item_type      ds707_nv_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DATA_IS707_MSG0(MSG_LEGACY_MED,"In ds_qcmip_update");
  
  if(TRUE == read_flag)
  {
    status = ds3gcfgmgr_read_legacy_nv( NV_DS_QCMIP_I, &ds707_nv_item);
    if( status == NV_DONE_S)
    {
      (void) dsmip_set_1x_profile_val_num(DS_QCMIP_VAL,
                                          (void *)&(ds707_nv_item.ds_qcmip),
      	                                  MIP_NON_ARRAY);
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
      	              "Bad NV read status %d for DS QCMIP,pass_fail= FALSE",
                      status);
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------
    Modify QCMDR and CRM values to reflect QCMIP setting
  -----------------------------------------------------------------------*/
  ds707_pkt_mgr_qcmip_set_mdr_val();
  ds707_pkt_mgr_qcmip_set_crm_val();

  /*-----------------------------------------------------------------------
    Recalculate 1x service option table
  -----------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  return TRUE;
}

/*===========================================================================
FUNCTION DS_QCMIPP_UPDATE

DESCRIPTION
  Called by refresh event handler and $QCMIPP command execution
  to update values associated with NV_DS_MIP_ACTIVE_PROF_I.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if success
  FALSE: if failure

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_qcmipp_update
(
  boolean read_write_flag /* flag denoting write/read to NV is required or not*/
)
{
  boolean            result= TRUE;
  nv_stat_enum_type  status;          /* status from NV call */
  dsat_num_item_type tmp_val = 0;
  nv_item_type       ds707_nv_item;  
  uint8              temp_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DATA_IS707_MSG1(MSG_LEGACY_MED,
                  "In ds_qcmipp_update with read_write_flag=%d",
                  read_write_flag);

  /* read the value from NV and update the global variable */
  if(FALSE == read_write_flag)
  {
    status = ds3gcfgmgr_read_legacy_nv( NV_DS_MIP_ACTIVE_PROF_I, &ds707_nv_item);
    if( status == NV_DONE_S)
    {
      (void) dsmip_set_1x_profile_val_num(DS_QCMIPP_VAL,
                 (void *)&(ds707_nv_item.ds_mip_active_prof),MIP_NON_ARRAY);
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                      "Bad NV read status %d for MIP active"
                      "profile return FALSE",
                      status);
      return FALSE;
    }
  }

  /*-----------------------------------------------------------------------
    Retrieve the defaults if it was not active, else retrieve the current
    values.
  -----------------------------------------------------------------------*/
  if (ds_init_mip_prof() == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Initialization of MIP user profile failed");
    result = FALSE;
  }
  else
  {
    /*---------------------------------------------------------------------
      Copy the retrieved values into 707-atcop values for display
      // replace both with mip_get_nv_item()
    ---------------------------------------------------------------------*/
#ifdef FEATURE_MODEM_RCINIT_PHASE2
    if (rex_self() == rcinit_lookup_rextask("ds")) 
    {
      dsmip_fill_mip_vals(ds3gcfgmgr_read_legacy_nv, &ds707_nv_item);
    }
    else if (rex_self() == rcinit_lookup_rextask("ps")) 
    {
      dsmip_fill_mip_vals(ps_get_nv_item, &ds707_nv_item);
    }
#else
    if (rex_self() == &ds_tcb) 
    {
      dsmip_fill_mip_vals(ds3gcfgmgr_read_legacy_nv, &ds707_nv_item);
    }
    else if (rex_self() == &ps_tcb) 
    {
      dsmip_fill_mip_vals(ps_get_nv_item, &ds707_nv_item);
    }
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

    /*---------------------------------------------------------------------
      Is the profile enabled or disabled?  later - check return value
    ---------------------------------------------------------------------*/
    (void)ds3gcfgmgr_read_legacy_nv(NV_DS_MIP_ENABLE_PROF_I,&ds707_nv_item);
    (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
    	                                   (void *)&temp_index,MIP_NON_ARRAY);
    tmp_val = ds707_nv_item.ds_mip_enable_prof[temp_index];
    (void) dsmip_set_1x_profile_val_num(DS_QCMIPEP_VAL,
    	                                   (void *)&tmp_val,MIP_NON_ARRAY);

    /*---------------------------------------------------------------------
      Write QCMIPP value into NV.  later - check return value
    ---------------------------------------------------------------------*/
    (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
                                        (void *)&temp_index,MIP_NON_ARRAY);
    ds707_nv_item.ds_mip_active_prof = temp_index;
    status = ds3gcfgmgr_write_legacy_nv(NV_DS_MIP_ACTIVE_PROF_I, &ds707_nv_item);

    /*---------------------------------------------------------------------
      Once the active profile is changed, erase all the MIP
      shadow settings for the previous active profile
    ---------------------------------------------------------------------*/
    dsmip_erase_shadow_nv();

#ifdef FEATURE_DS_MOBILE_IP_DMU
    /*---------------------------------------------------------------------
      Re-encrypt the keys.  Set encrypted to false so RRQ's are
      skipped until encryption is complete.
      Re-using this signal to re-encrypt with new keys; Make sure DMU is
      not in progress,  else currentkeys in use will be messed up.
    ---------------------------------------------------------------------*/
    if(!mip_dmu_in_progress())
    {
      if (mip_is_dmu_rand_ready())
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED,"Changed profile, re-encrypt new keys");
        ps_send_cmd(DMU_PREENCRYPT_CMD, NULL);
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_MED,
                        "Skipping pre-encryption - no random data yet");
      }
    }
#endif /* FEATURE_DS_MOBILE_IP_DMU */
  }

  return result;
}

/*===========================================================================
FUNCTION    DS_QCMIP_GET_RUNTIME_VAL

DESCRIPTION
  This function will return the value of qcmip, from runtime profile structure.

DEPENDENCIES
  None

RETURN VALUE
  UINT8 : Value of ds_qcmip_val.

SIDE EFFECTS
  None
===========================================================================*/
uint8 ds_qcmip_get_runtime_val
(
  void
)
{
  ds707_mip_profile_type* ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();

  return ( ds_1x_runtime_profile_ptr->mip.ds_qcmip_val );
}

/*===========================================================================
FUNCTION DS_QCMIP_SET_VAL

DESCRIPTION
  This function sets the QCMIP value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_qcmip_set_val
(
  uint32 qcmip_val 
)
{
  ds707_mip_profile_type *ds707_mip_static_profile_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_mip_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds707_mip_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }

  ds707_mip_static_profile_ptr->mip.ds_qcmip_val = (uint8)qcmip_val;
}

/*===========================================================================
FUNCTION DSMIP_GET_SHADOW_NV_PTR

DESCRIPTION
  This functions returns the shadow pointer to interface with ATCOP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsmip_shadow_nv_type* dsmip_get_shadow_nv_ptr
(
  void
)
{
  return  &shadow_nv;
}

/*===========================================================================
FUNCTION DSMIP_IS_USING_SHADOW_NV

DESCRIPTION
  Checks if any of the shadow NV values are in use for MIP.

  Called from PS so make sure no inter-task contention issues

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - using shadow NV
  FALSE - not using shadow NV

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_is_using_shadow_nv
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* We check for all the fields except reverse tunneling */
  if(shadow_nv.nai.len         ||
     shadow_nv.mn_aaa_ss.len   ||
     shadow_nv.mn_ha_ss.len    ||
     shadow_nv.mn_aaa_spi.set  ||
     shadow_nv.mn_ha_spi.set   ||
     shadow_nv.home_addr.set   ||
     shadow_nv.pri_ha_addr.set ||
     shadow_nv.sec_ha_addr.set
    )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION DSMIP_ERASE_SHADOW_NV

DESCRIPTION
  This functions flushes the shadow NV structure. It is called at the
  initilization ,when the active profile has been changed and
  when the call is ended.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmip_erase_shadow_nv
(
  void
)
{
  DATA_IS707_MSG0(MSG_LEGACY_MED,"Erase shadow NV");
  memset(&shadow_nv, 0, sizeof(shadow_nv));
} 

/*===========================================================================
FUNCTION DS_INIT_MIP_PROF

DESCRIPTION
 This function initializes the MIP user profile and sets the default
 values.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - success
  FALSE - error

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_init_mip_prof
(
  void
)
{
  nv_stat_enum_type               nv_status;
  nv_ds_mip_gen_user_prof_type *  prof_ptr;
  nv_ds_mip_ss_user_prof_type *   ss_ptr;
  nv_item_type                    ds707_nv_item;
  uint8                           temp_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 /*-------------------------------------------------------------------------
   Retrieve the active user profile
 -------------------------------------------------------------------------*/
  (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
                                      (void *)&temp_val,MIP_NON_ARRAY);
  ds707_nv_item.ds_mip_gen_user_prof.index = temp_val;
  nv_status = ds3gcfgmgr_read_legacy_nv(NV_DS_MIP_GEN_USER_PROF_I,&ds707_nv_item);

 /*-------------------------------------------------------------------------
   If the active user profile was never written, write default values 
   to the profile and disable it until provisioning is complete.
 -------------------------------------------------------------------------*/
  switch(nv_status)
  {
    case NV_DONE_S:
      DATA_IS707_MSG1(MSG_LEGACY_MED,
     	              "Active MIP User profile %d has been provisioned",0);
      break;

    case NV_NOTACTIVE_S:
      DATA_IS707_MSG0(MSG_LEGACY_MED,"User profile never written");

   /*---------------------------------------------------------------------
       If R-UIM is being used, we can't write to it, so fail the operation.
   ---------------------------------------------------------------------*/ 
      if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "In ds_init_mip_prof. No MIP user profile on UIM!");
        return FALSE;
      }

      DATA_IS707_MSG0(MSG_LEGACY_MED,"Create default MIP user profile");
      prof_ptr = &ds707_nv_item.ds_mip_gen_user_prof;
      memset((void *)prof_ptr, 0, sizeof(ds707_nv_item.ds_mip_gen_user_prof));
#ifdef FEATURE_DSAT_GOBI_MAINLINE
      (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
                                          (void *)&temp_val,MIP_NON_ARRAY);
      ds707_nv_item.ds_mip_gen_user_prof.index = temp_val;
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

      /*---------------------------------------------------------------------
        Set user profile to defaults.
      ---------------------------------------------------------------------*/
      prof_ptr->nai_length        = 0;
      prof_ptr->mn_ha_spi_set     = TRUE;
      prof_ptr->mn_ha_spi         = MIP_MD5_SPI;
      prof_ptr->mn_aaa_spi_set    = TRUE;
      prof_ptr->mn_aaa_spi        = MIP_CHAP_SPI;
      prof_ptr->rev_tun_pref      = FALSE;
      prof_ptr->home_addr         = DYNAMIC_HOME_ADDR;
      prof_ptr->primary_ha_addr   = DYNAMIC_HA_ADDR;
      prof_ptr->secondary_ha_addr = UNSET_HA_ADDR;
 
     /*---------------------------------------------------------------------
       Write profile to nv
     ---------------------------------------------------------------------*/
      nv_status = ds3gcfgmgr_write_legacy_nv( NV_DS_MIP_GEN_USER_PROF_I, 
                                   &ds707_nv_item );
      if( nv_status != NV_DONE_S )
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "In ds_init_mip_prof. "
                        "Error (%d) writing default MIP user profile",
                        nv_status);
        return FALSE;
      }

     /*---------------------------------------------------------------------
       Disable the profile so calls aren't made until provisioning is 
       complete.  later: check return value
     ---------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_GOBI_MAINLINE
      nv_status = ds3gcfgmgr_read_legacy_nv(NV_DS_MIP_ENABLE_PROF_I,&ds707_nv_item);
      if( nv_status == NV_NOTACTIVE_S )
      {
        DATA_IS707_MSG1(MSG_LEGACY_MED,
                        "MIP enable profile is inactive",nv_status);
        memset((void *)ds707_nv_item.ds_mip_enable_prof, 0,
       	   sizeof(ds707_nv_item.ds_mip_enable_prof[0])*NV_DS_MIP_MAX_NUM_PROF);
      }
      else if( nv_status != NV_DONE_S )
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "In ds_init_mip_prof. "
                        "Error (%d) reading MIP enable profile",nv_status);
        return FALSE;
      }
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
      (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
                                          (void *)&temp_val,MIP_NON_ARRAY);
      ds707_nv_item.ds_mip_enable_prof[temp_val] = 0;
      (void) ds3gcfgmgr_write_legacy_nv (NV_DS_MIP_ENABLE_PROF_I, &ds707_nv_item);
      break;

    default:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "In ds_init_mip_prof. "
                      "MIP gen profile cannot be retrieved");
      return FALSE;
  }

  /*-------------------------------------------------------------------------
    Check if the shared secret profile has been written, if not set to 
    defaults and disable profile
  -------------------------------------------------------------------------*/
  (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
                                      (void *)&temp_val,MIP_NON_ARRAY);
  ds707_nv_item.ds_mip_ss_user_prof.index = temp_val;
  nv_status = ds3gcfgmgr_read_legacy_nv(NV_DS_MIP_SS_USER_PROF_I,&ds707_nv_item);
  switch(nv_status)
  {
    case NV_DONE_S:
      DATA_IS707_MSG1(MSG_LEGACY_MED,
                      "Active MIP shared secret profile %d "
                      "has been provisioned",
                      0);
      break;

    case NV_NOTACTIVE_S:
      DATA_IS707_MSG0(MSG_LEGACY_MED,"Shared secret profile never written");

      if(NVRUIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
         NVRUIM_3GPD_MIP_RUIM_SIP_NV == uim_3gpd_support())
      {
        /* Password not needed for R-UIM */
        break;
      }

      DATA_IS707_MSG0(MSG_LEGACY_MED,
                      "Create default MIP shared secret profile");
      ss_ptr = &ds707_nv_item.ds_mip_ss_user_prof;
      memset((void *)ss_ptr, 0, sizeof(ds707_nv_item.ds_mip_ss_user_prof));
      (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
      	                                   (void *)&temp_val,MIP_NON_ARRAY);
      ds707_nv_item.ds_mip_ss_user_prof.index = temp_val;
      nv_status = ds3gcfgmgr_write_legacy_nv( NV_DS_MIP_SS_USER_PROF_I,
                                   &ds707_nv_item );
      if( nv_status != NV_DONE_S )
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "In ds_init_mip_prof. "
                        "Error (%d) writing default MIP shared secret profile",
                        nv_status);
        return FALSE;
      }
      break;

    default:
      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                      "In ds_init_mip_prof. "
                      "MIP shared secret cannot be retrieved");
      return FALSE;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION DSMIP_FILL_MIP_VALS

DESCRIPTION
  This function copies the values retreived from NV into dsmip_val which is
  used for display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmip_fill_mip_vals
(
  nv_stat_enum_type (* nv_get_fn)(nv_items_enum_type, nv_item_type *),
  nv_item_type *    nv_item
)
{
  nv_stat_enum_type          nv_get_status;
  ds707_mip_profile_type    *ds707_mip_static_profile_ptr; 
  uint8                      temp_val;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ds707_mip_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr(); 
  if( ds707_mip_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }

  /*-------------------------------------------------------------------------
    This function is used to reload the MIP 707-AT command values with the NV 
    settings.  Called after the call has ended or when QCMIPP is called.
  -------------------------------------------------------------------------*/
  (void) dsmip_get_1x_profile_val_num(DS_QCMIPP_VAL,
                                      (void *)&temp_val,MIP_NON_ARRAY);
  nv_item->ds_mip_gen_user_prof.index = temp_val;
  nv_get_status = (*nv_get_fn)(NV_DS_MIP_GEN_USER_PROF_I, nv_item);
  if(nv_get_status != NV_DONE_S)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Trouble getting MIP user profile from NV!");
    DATA_IS707_MSG0(MSG_LEGACY_MED,"Set AT values to defaults!");

    ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0]      = 0;

    ds707_mip_static_profile_ptr->mip.ds_qcmipmhspi_val[0]    = 0x3;
    ds707_mip_static_profile_ptr->mip.ds_qcmipmaspi_val[0]    = 0x2;
    ds707_mip_static_profile_ptr->mip.ds_qcmiprt_val[0]       = FALSE;
    ds707_mip_static_profile_ptr->mip.ds_qcmip_ha_num_val     = 
                                          (uint32)DYNAMIC_HOME_ADDR;
    ds707_mip_static_profile_ptr->mip.ds_qcmip_pri_ha_num_val = 
                                          (uint32)DYNAMIC_HA_ADDR;
    ds707_mip_static_profile_ptr->mip.ds_qcmip_sec_ha_num_val = 
                                          (uint32)UNSET_HA_ADDR;
    return;
  }

  /*-------------------------------------------------------------------------
    Copy NAI and null terminate
  -------------------------------------------------------------------------*/
  memset(&ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0],
         0,
         NV_MAX_NAI_LENGTH);
  if (nv_item->ds_mip_gen_user_prof.nai_length != 0 &&
      nv_item->ds_mip_gen_user_prof.nai_length < MAX_NAI_LEN + 1)
  {
    memscpy((char *)&ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0],
            NV_MAX_NAI_LENGTH,
            (char *)nv_item->ds_mip_gen_user_prof.nai,
            nv_item->ds_mip_gen_user_prof.nai_length );
    ((char *)&ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0])[
       MIN(nv_item->ds_mip_gen_user_prof.nai_length, NV_MAX_NAI_LENGTH-1)] = 0;
  }

  ds707_mip_static_profile_ptr->mip.ds_qcmipmhspi_val[0]    = 
  	                nv_item->ds_mip_gen_user_prof.mn_ha_spi;
  ds707_mip_static_profile_ptr->mip.ds_qcmipmaspi_val[0]    = 
  	                nv_item->ds_mip_gen_user_prof.mn_aaa_spi;
  ds707_mip_static_profile_ptr->mip.ds_qcmiprt_val[0]       = 
  	                nv_item->ds_mip_gen_user_prof.rev_tun_pref;

  ds707_mip_static_profile_ptr->mip.ds_qcmip_ha_num_val     = 
                   nv_item->ds_mip_gen_user_prof.home_addr;
  ds707_mip_static_profile_ptr->mip.ds_qcmip_pri_ha_num_val =
                   nv_item->ds_mip_gen_user_prof.primary_ha_addr;
  ds707_mip_static_profile_ptr->mip.ds_qcmip_sec_ha_num_val =
                   nv_item->ds_mip_gen_user_prof.secondary_ha_addr;
}
 
/*===========================================================================
FUNCTION DSMIP_GET_1X_PROFILE_VAL_NUM

DESCRIPTION
  This function will return value for the requested val of type num.
  The first parameter is enum type for the val, second parameter is
  the value pointer and the third parameter specifies the number of
  elements to be returned if the val is of array type and it should 
  be 0 if the val is of non-array type.
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE : Value of the corresponding val from static profile was 
         returned successfully.
  FALSE: Value of the corresponding val from static profile was 
         not returned successfully.
             
SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_get_1x_profile_val_num
(
  dsmip_val_enum_type        type,
  void                      *val,
  uint8                      num_elements
)
{
  uint8                      i = 0;  
  boolean                    ret_val = FALSE;
  ds707_mip_profile_type    *ds707_mip_static_profile_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(val == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Null pointer passed in "
                    "dsmip_get_1x_profile_val_num");
    return ret_val;
  }

  ds707_mip_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds707_mip_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
  
  switch(type)
  {
    case DS_QCMIP_VAL:
      *((uint8 *)val) = ds707_mip_static_profile_ptr->mip.ds_qcmip_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIPP_VAL:
      *((uint8 *)val) = ds707_mip_static_profile_ptr->mip.ds_qcmipp_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIPT_VAL:
      *((uint8 *)val) = ds707_mip_static_profile_ptr->mip.ds_qcmipt_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIPEP_VAL:
      *((uint8 *)val) = ds707_mip_static_profile_ptr->mip.ds_qcmipep_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIPGETP_VAL:
      *((uint8 *)val) = ds707_mip_static_profile_ptr->mip.ds_qcmipgetp_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIPRT_VAL:
      if( num_elements > MIP_RT_MAX )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Invalid no_of_elements in "
                        "dsmip_get_1x_profile_val_num");
        break;
      }	
      for( i = 0; i < num_elements; i++ )
      {
        *(((uint32 *)val) + i) = 
                      ds707_mip_static_profile_ptr->mip.ds_qcmiprt_val[i];
      }
      ret_val = TRUE;
      break;
      
    case DS_QCMIPMASPI_VAL:
      if( num_elements > MIP_MA_SPI_MAX)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Invalid no_of_elements in "
                        "dsmip_get_1x_profile_val_num");
        break;
      }	
      for( i = 0; i < num_elements; i++ )
      {
        *(((uint32 *)val) + i) = 
                    ds707_mip_static_profile_ptr->mip.ds_qcmipmaspi_val[i];
      }
      ret_val = TRUE;
      break;
      
    case DS_QCMIPMHSPI_VAL:
      if( num_elements > MIP_MH_SPI_MAX )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Invalid no_of_elements in "
                        "dsmip_get_1x_profile_val_num");
        break;
      }	
      for( i = 0; i < num_elements; i++ )
      {
        *(((uint32 *)val) + i) = 
                    ds707_mip_static_profile_ptr->mip.ds_qcmipmhspi_val[i];
      }
      ret_val = TRUE;
      break;
      
    case DS_QCMIP_HA_NUM_VAL:
      *((uint32 *)val) = ds707_mip_static_profile_ptr->mip.ds_qcmip_ha_num_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIP_PRI_HA_NUM_VAL:
      *((uint32 *)val) = 
                    ds707_mip_static_profile_ptr->mip.ds_qcmip_pri_ha_num_val;
      ret_val = TRUE;
      break;
      
    case DS_QCMIP_SEC_HA_NUM_VAL:
      *((uint32 *)val) = 
                   ds707_mip_static_profile_ptr->mip.ds_qcmip_sec_ha_num_val;
      ret_val = TRUE;
      break;
      
    default:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "In dsmip_get_1x_profile_val_num. No match for type %d",
                      type);
  }
  
  return ret_val;
}


/*===========================================================================
FUNCTION DSMIP_GET_1X_PROFILE_VAL_STR

DESCRIPTION
  This function will fill the passed string buffer with the requested value
  and return whether the buffer is properly filled or not with the string in
  corresponding variable of static profile. 
  The first parameter is enum type for the val, second parameter is
  the string buffer pointer and the third parameter specifies the 
  length of the buffer.
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE  :  Value set.
  FALSE :  Value did not set.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_get_1x_profile_val_str
(
  dsmip_val_enum_type         type,
  char*                       str,
  uint32                      length
)
{
  boolean                 ret_val = FALSE;
  ds707_mip_profile_type *ds707_mip_static_profile_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds707_mip_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds707_mip_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
  
  if( (str == NULL) || (length == 0) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "String pointer is NULL or string length is 0.");
    return ret_val;
  }

  switch(type)
  {
    case DS_QCMIPNAI_VAL:
      if(length >= MAX_NAI_LEN)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0],
                MAX_NAI_LEN);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPMASS_VAL:
      if(length >= MIP_MN_AAA_SS_MAX_LEN)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipmass_val[0],
                MIP_MN_AAA_SS_MAX_LEN);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPMHSS_VAL:
      if(length >= MIP_MN_HA_SS_MAX_LEN)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipmhss_val[0],
                MIP_MN_HA_SS_MAX_LEN);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPMASSX_VAL:
      if(length >= MIP_MN_AAA_SS_MAX_LEN)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipmassx_val[0],
                MIP_MN_AAA_SS_MAX_LEN);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPMHSSX_VAL:
      if(length >= MIP_MN_HA_SS_MAX_LEN)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipmhssx_val[0],
                MIP_MN_HA_SS_MAX_LEN);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPHA_VAL:
      if(length >= MIP_HA_MAX)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipha_val[0],MIP_HA_MAX);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPPHA_VAL:
      if(length >= MIP_PHA_MAX)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmippha_val[0],MIP_PHA_MAX);
        ret_val = TRUE;
      }
      break;
      
    case DS_QCMIPSHA_VAL:
      if(length >= MIP_SHA_MAX)
      {
        memscpy(str,length,
                &ds707_mip_static_profile_ptr->mip.ds_qcmipsha_val[0],MIP_SHA_MAX);
        ret_val = TRUE;
      }
      break;
      
    default:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                      "In dsmip_get_1x_profile_val_str. "
                      "No match for type %d",
                      type);
  }

  return ret_val;
}

/*===========================================================================
FUNCTION DSMIP_SET_1X_PROFILE_VAL_NUM

DESCRIPTION
  This function will set members of ds707_mip_static_profile .
  The first parameter is enum type for the val, second parameter is
  the value pointer and the third parameter specifies the number of
  elements to be filled in the corresponding variable of static
  profile if the val is of array type and it should  be 0 if the val 
  is of non-array type.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - True : Success
            False: Failure

SIDE EFFECTS
  None
===========================================================================*/
boolean dsmip_set_1x_profile_val_num
(
  dsmip_val_enum_type         type,
  void                       *val,
  uint8                       num_elements
)
{
  uint8                      i = 0;
  uint8                      orig_val;
  boolean                    ret_val = FALSE;
  nv_item_type               ds707_nv_item;
  nv_stat_enum_type          status;
  ds707_mip_profile_type    *ds707_mip_static_profile_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(val == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "Null pointer passed in "
                    "dsmip_set_1x_profile_val_num");
    return ret_val;
  }

  ds707_mip_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds707_mip_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
	  
  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
                  "In dsmip_set_1x_profile_val_num. "
                  "Setting type = %d , val = %d",type,*((uint32 *)val));
	  
  switch(type)
  {
    case DS_QCMIP_VAL:
      if ( (*((uint8 *)val)) > QCMIP_VAL_UPPER_BOUND )
      {
        break;
      }
    	 
      ds707_mip_static_profile_ptr->mip.ds_qcmip_val = *((uint8 *)val);
      (void)ds_qcmip_update(FALSE);
      ds707_nv_item.ds_qcmip = *((uint8 *)val);
      status = ds3gcfgmgr_write_legacy_nv( NV_DS_QCMIP_I, &ds707_nv_item );
      if( status != NV_DONE_S )
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                        "NV write failed for type %d",
                        type);
        break;
      }
      ret_val = TRUE;
      break;
		  
    case DS_QCMIPP_VAL:
      if ( (*((uint8 *)val)) > QCMIPP_VAL_UPPER_BOUND )
      {
        break;
      }

      orig_val = ds707_mip_static_profile_ptr->mip.ds_qcmipp_val;
      ds707_mip_static_profile_ptr->mip.ds_qcmipp_val = *((uint8 *)val);
      if( ds_qcmipp_update(TRUE) == FALSE )
      {
        ds707_mip_static_profile_ptr->mip.ds_qcmipp_val = orig_val;
        break;
      }
      ret_val = TRUE;
      break;
		  
    case DS_QCMIPT_VAL:
      if ( (*((uint8 *)val)) > QCMIPT_VAL_UPPER_BOUND )
      {
        break;
      }

      ds707_mip_static_profile_ptr->mip.ds_qcmipt_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
	  
    case DS_QCMIPEP_VAL:
      if ( (*((uint8 *)val)) > QCMIPEP_VAL_UPPER_BOUND )
      {
        break;
      }

      ds707_mip_static_profile_ptr->mip.ds_qcmipep_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
		  
    case DS_QCMIPGETP_VAL:
      if ( (*((uint8 *)val)) > QCMIPGETP_VAL_UPPER_BOUND )
      {
        break;
      }

      ds707_mip_static_profile_ptr->mip.ds_qcmipgetp_val = *((uint8 *)val);
      ret_val = TRUE;
      break;
		  
    case DS_QCMIPRT_VAL:
      if( num_elements > MIP_RT_MAX )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid no_of_elements");
        break;
      }	

      if ( (*((uint32 *)val)) > QCMIPRT_PRI_VAL_UPPER_BOUND ||
      	    (*(((uint32 *)val) + 1)) > QCMIPRT_SEC_VAL_UPPER_BOUND )
      {
        break;
      }
      
      for( i = 0; i < num_elements; i++ )
      {
        ds707_mip_static_profile_ptr->mip.ds_qcmiprt_val[i] = 
                                                   *(((uint32 *)val) + i);
      }
      ret_val = TRUE;
      break;
      
    case DS_QCMIPMASPI_VAL:
      if( num_elements > MIP_MA_SPI_MAX )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid no_of_elements");
        break;
      }	

/*---------------------------------------------------------------------------
  Primary value of QCMIPMASPI ranges from 0 to FFFFFFFF and hence no bound
  check for it.
---------------------------------------------------------------------------*/
      if ( (*(((uint32 *)val) + 1)) > QCMIPMASPI_SEC_VAL_UPPER_BOUND )
      {
        break;
      }
      
      for( i = 0; i < num_elements; i++ )
      {
        ds707_mip_static_profile_ptr->mip.ds_qcmipmaspi_val[i] = 
                                                   *(((uint32 *)val) + i);
      }
      ret_val = TRUE;
      break;
		    
    case DS_QCMIPMHSPI_VAL:
      if( num_elements > MIP_MH_SPI_MAX )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,"Invalid no_of_elements");
        break;
      }	

/*---------------------------------------------------------------------------
  Primary value of QCMIPMHSPI ranges from 0 to FFFFFFFF and hence no bound
  check for it.
---------------------------------------------------------------------------*/
      if ( (*(((uint32 *)val) + 1)) > QCMIPMHSPI_SEC_VAL_UPPER_BOUND )
      {
        break;
      }
      
      for( i = 0; i < num_elements; i++ )
      {
        ds707_mip_static_profile_ptr->mip.ds_qcmipmhspi_val[i] = 
                                                  *(((uint32 *)val) + i);
      }
      ret_val = TRUE;
      break;
      
    case DS_QCMIP_HA_NUM_VAL:
      ds707_mip_static_profile_ptr->mip.ds_qcmip_ha_num_val = *((uint32 *)val);
      ret_val = TRUE;
      break;
	     
    case DS_QCMIP_PRI_HA_NUM_VAL:
      ds707_mip_static_profile_ptr->mip.ds_qcmip_pri_ha_num_val = *((uint32 *)val);
      ret_val = TRUE;
      break;
	     
    case DS_QCMIP_SEC_HA_NUM_VAL:
      ds707_mip_static_profile_ptr->mip.ds_qcmip_sec_ha_num_val = *((uint32 *)val);
      ret_val = TRUE;
      break;
	
    default:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,"No match for type %d",type);
  }

  return ret_val; 	
}


/*===========================================================================
FUNCTION DSMIP_SET_1X_PROFILE_VAL_STR
  
DESCRIPTION
  This function will copy the value in passed string buffer in the 
  corresponding string of static profile and return whether the string is
  properly copied or not with the string in corresponding variable of 
  static profile. 
  
  The first parameter is enum type for the val, second parameter is
  the string buffer pointer and the third parameter specifies the 
  length of the buffer.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - True : Success
            False: Failure

SIDE EFFECTS
  None  
===========================================================================*/
boolean dsmip_set_1x_profile_val_str
(
  dsmip_val_enum_type              type, 
  char*                            str,
  uint32                           length
)
{
  boolean                 ret_val = FALSE;
  ds707_mip_profile_type *ds707_mip_static_profile_ptr;
   
  ds707_mip_static_profile_ptr = ds707_pkt_mgr_get_1x_data_profile_ptr();
  if( ds707_mip_static_profile_ptr == NULL )
  {
    DATA_ERR_FATAL("NULL pointer received from ds707_pkt_mgr_get_1x_data_profile_ptr");
  }
   
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Setting %d",type);
  if( (str == NULL) || (length == 0) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                    "String pointer is NULL or string length is 0.");
    return ret_val;
  }

  switch(type)
  {
    case DS_QCMIPNAI_VAL:
      if( length <= MAX_NAI_LEN )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0],0,
               MAX_NAI_LEN + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipnai_val[0],
                MAX_NAI_LEN,str,length);
        ret_val = TRUE;
      }
      break;
       
    case DS_QCMIPMASS_VAL:     
      if( length <= MIP_MN_AAA_SS_MAX_LEN )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipmass_val[0],0,
               MIP_MN_AAA_SS_MAX_LEN + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipmass_val[0],
      	         MIP_MN_AAA_SS_MAX_LEN,str,length);
        ret_val = TRUE;
      }
      break;
 
    case DS_QCMIPMHSS_VAL:
      if( length <= MIP_MN_HA_SS_MAX_LEN )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipmhss_val[0],0,
               MIP_MN_HA_SS_MAX_LEN + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipmhss_val[0],
                MIP_MN_HA_SS_MAX_LEN,str,length);
        ret_val = TRUE;
      }
      break;
 
    case DS_QCMIPMASSX_VAL:
      if( length <= MIP_MN_AAA_SS_MAX_LEN )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipmassx_val[0],
               0,MIP_MN_AAA_SS_MAX_LEN + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipmassx_val[0],
                MIP_MN_AAA_SS_MAX_LEN,str,length);
        ret_val = TRUE;
      }
      break;
       
    case DS_QCMIPMHSSX_VAL:
      if( length <= MIP_MN_HA_SS_MAX_LEN )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipmhssx_val[0],
               0,MIP_MN_HA_SS_MAX_LEN + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipmhssx_val[0],
                MIP_MN_HA_SS_MAX_LEN,str,length);
        ret_val = TRUE;
      }
      break;

    case DS_QCMIPHA_VAL:
      if( length <= MIP_HA_MAX )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipha_val[0],0,
               MIP_HA_MAX + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipha_val[0],
      	        MIP_HA_MAX,str,length);
        ret_val = TRUE;
      }
      break;
       
    case DS_QCMIPPHA_VAL:
      if( length <= MIP_PHA_MAX )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmippha_val[0],0,
               MIP_PHA_MAX + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmippha_val[0],
      	        MIP_PHA_MAX,str,length);
        ret_val = TRUE;
      }
      break;
        
    case DS_QCMIPSHA_VAL:
      if( length <= MIP_SHA_MAX )
      {
        memset((void*)&ds707_mip_static_profile_ptr->mip.ds_qcmipsha_val[0],0,
               MIP_SHA_MAX + 1);
        memscpy(&ds707_mip_static_profile_ptr->mip.ds_qcmipsha_val[0],
      	        MIP_SHA_MAX,str,length);
        ret_val = TRUE;
      }
      break;

    default:
       DATA_IS707_MSG1(MSG_LEGACY_HIGH,
       	               "In dsmip_set_1x_profile_val_str. No match for type %d",
       	               type);
  }

  return ret_val;
} /* dsmip_set_1x_profile_val_str */

#endif /* FEATURE_DS_MOBILE_IP */

/*===========================================================================
FUNCTION MIP_REG_SES_INFO_CB()

DESCRIPTION
  Registers a user callback to populate MIP session info control block.

PARAMETERS
  fn_ptr:  Pointer to user routine of the following signature:
           void fn_ptr( byte profile_num, mip_session_info_type * ses_info)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  The session info block settings may affect MIP negotiation
===========================================================================*/
void mip_reg_ses_info_cb
(
  void (* callback)( byte profile_num, mip_session_info_type * ses_info )
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    Save the pointer to callback
  -------------------------------------------------------------------------*/
  mip_get_session_info_cb = callback;

#else
  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "mip_reg_ses_info_cb() not supported");
#endif

} /* mip_reg_ses_info_cb() */

/*===========================================================================
FUNCTION MIP_GET_RM_NAI()

DESCRIPTION
  This function retrieves the NAIs for a given MIP profile for the
  tethered device and stores them in the mip session info.

PARAMETERS
  session: pointer to the mip session control block where info is stored
  profile: profile number associated with the mip session.

RETURN VALUE
  -1 if session information retrieved from NV is insufficient to make a call
  otherwise, the current profile no. that was passed in as argument.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 mip_get_rm_nai
(
  mip_session_info_type *  session,
  int                      active_mip_profile
)
{
#if defined(FEATURE_DS_MULTIPLE_NAI) && defined(FEATURE_DS_MOBILE_IP)
  nv_stat_enum_type nv_status;
  char *            nai_temp;
  nv_item_type      nv_item;
  byte              nai_len=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check to be sure the a valid session control block was specified
  -------------------------------------------------------------------------*/
  if (session == NULL)
  {
    DATA_ERR_FATAL("Invalid session control block.");
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Retrieve currently selected, or "active", profile from NV.
    Return from function if there is no active profile.
  -------------------------------------------------------------------------*/
  nv_item.ds_mip_rm_nai.index = active_mip_profile;
  nv_status = ds3gcfgmgr_read_legacy_nv( NV_DS_MIP_RM_NAI_I, &nv_item);
  if( nv_status != NV_DONE_S )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Laptop NAI cannot be retrieved");
    return (-1);
  }

  nai_temp = (char*) nv_item.ds_mip_rm_nai.nai;
  nai_len = nv_item.ds_mip_rm_nai.nai_length;

  /*-----------------------------------------------------------------------
    Only copy NAI if its length is valid.  If NAI length is > max allowed
    assume a memory/flash scribble and set length to 0.
  -----------------------------------------------------------------------*/
  if(nai_len > NV_MAX_NAI_LENGTH)
  {
    nai_len = 0;
  }
  else if(nai_len > 0)
  {
    session->nai_len = nai_len;
    memscpy(session->nai,MAX_NAI_LEN, nai_temp, session->nai_len);
    DATA_MIP_MSG1(MSG_LEGACY_LOW, " session->nai_len is %d",session->nai_len);
  }

  /*-------------------------------------------------------------------------
    return the active profile number to the caller.
  -------------------------------------------------------------------------*/
  return (session->profile_index);
#else
  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "mip_get_rm_nai() is not supported.");
  return (-1);
#endif /* FEATURE_DS_MULTIPLE_NAI*/
} /* mip_get_rm_nai() */


