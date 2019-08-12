/*===========================================================================
                                      
                       D S M I P _ O U T M S G . C

DESCRIPTION

 The Data Services Mobile IP output message generator implementation.  
 Contains API and internal functions to generate the agent solicitation
 and registration request messages that are sent from the mobile to the 
 foreign agent.

EXTERNALIZED FUNCTIONS

  mip_outmsg_send_sol()
    Composes and expedites an agent solicitation via the mip_io module

  mip_outmsg_send_rrq()
    Composes and expedites a registration request via the mip_io module

  mip_outmsg_send_dereg_rrq()
    Composes and expedites a deregistration request via the mip_io module

Copyright (c) 2000-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/dsmip_outmsg.c_v   1.30   29 Dec 2002 15:44:10   jeffd  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_outmsg.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
06/03/13    sd     ATCoP decoupling changes
05/01/12    sb     Always On UIM Feature Cleanup
02/06/12    sn     Fix to increment MIP stats RRQ count for MIP Re-registration 
                   scenarios.
05/20/11    op     Migrated to MSG 2.0 macros
03/15/11    mg     Global variable cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/11/11    ack    Global Variable Cleanup
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
07/16/10    ssh    Fixed compiler warning due to -Wstrict-prototypes
01/11/10    ms     Replaced UIM_3GPD Constants with NVRUIM_3GPD
09/04/09    ss     CMI SU level modifications.
09/02/08    cs     Klocwork fixes.
08/07/08    as     Lint error fixes.
05/23/07    vp     MIP CCoA related changes.
12/07/06    as     Added support for sending a mip dereg RRQ.
01/19/05    jd     Wrap dmu cleartext mode under test feature
10/31/04   msr/ks  Removed memdump() and added it to ps_utils.c.
05/18/04    jk     Added support for 3GPD authentication on R-UIM card.
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
01/17/04    jd     Cleaned up MIP event generation code.
10/09/03    jd     Moved definition of MIP_UDP_PORTNO to dsmip_io.h
06/05/03    jd     Fixed bug in memdump() debug routine
03/07/03    js     Added MIP events reporting. 
10/09/02    jay    Changed #define from MIP_SPI_CHAP to MIP_CHAP_SPI
10/02/02    jay    Fixed cleartext mode by using memset instead of memcpy
09/20/02    jay    Added a few debug messages.
08/19/02    jay    Added support for cleartext mode of DMU.
07/18/02    jd     Added support for MIP DMU
05/10/02    jd     Moved shared secret selection logic to dsmip_auth.c, 
                   now done via function mip_auth_retrieve_ss_data()
04/17/02    jd     Use global ps_nv_item when retrieving items from NV
02/06/02    jay    Use shadow ss lengths instead of strlen.
01/22/02    jay    Fix some warning messages
01/17/02    jay    Use shadow NV in the session block.
10/30/01    jd     change IP dest. address for ASM's from all-routers 
                   multicast (224.0.0.2) to subnet-limited broadcast 
                   (255.255.255.255)
10/16/01    jd     relocate RRQ holdoff flag assignment to avoid race cond.
09/06/01    jd     retrieve NAI and password from user callbacks if they 
                   have been registered.
08/16/01    jd     time offset NV read changed to reflect restructuring in 
                   nv.h
08/02/01    jd     Read the time offset from NV 
                   Dependency - MSM_NV.01.00.34 VU Release
07/05/01    jd     we send RRQ even if we know the mobile's reverse tunneling 
                   preference will not be granted.
06/26/01    jd     added switch b/w rfc2002/bis MN-HA authenticator 
                   calculation by checking AT$QCMIPT value
06/20/01    jd     changed MN-HA authenticator calculation to exclude SPI
                   field as per rfc2002
06/19/01    jd     was adding wrong length of MN-AAA auth ext to RRQ. fixed.
06/19/01    jd     MN-AAA SPI is taken from NV rather than lookup table
06/13/01    mvl    updated references to match names in session info block.
06/07/01    jd     Fixed registration logic to fail immediately if mobile 
                   wants reverse tunneling but MA does not support it.
12/19/00    jd     Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"

#include "target.h"
#include "customer.h"
#include "data_msg.h"
#include "ds707_pkt_mgr.h"

#ifdef FEATURE_DS_MOBILE_IP
#include <string.h>

#include "amssassert.h"
#include "nv.h"
#include "pstimer.h"
#include "dssocket.h"

#include "dsmip_io.h"
#include "dsmip_msg.h"
#include "dsmip_v.h"
#include "dsmip_parse.h"
#include "dsmip_auth.h"
#include "dsntp.h"
#include "dsmip_regsm.h"
#include "dsmip_metasm.h"
#include "dsmip_event_defs.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#include "dsat707mipctab.h"
#include "ps_utils.h"
#include "nvruimi.h"
#include "ds_1x_profile.h"
#include "ps_system_heap.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  BP_COPY8(d,s), BP_COPY16(d,s), BP_COPY32(d,s)

  copy 8, 16, 32 bits respectively from source address s to destination
  address d and advance d by the same number of bits.

  note: d must be a byte pointer, s must be a byte pointer
---------------------------------------------------------------------------*/

#define BP_COPY8(d,s)   (*(d) = *(s)); (d)++;
#define BP_COPY16(d,s)  BP_COPY8(d,s); BP_COPY8(d,(s)+1);
#define BP_COPY32(d,s)  BP_COPY16(d,s); BP_COPY16(d,(s)+2);

/*---------------------------------------------------------------------------
  BP_PUT8(d,s), BP_PUT16(d,s), BP_PUT32(d,s)

  puts immediate 8, 16, 32 bit value s respectively to destination
  address d and advance d by the same number of bits.

  note: d must be a byte pointer, s must be an immediate value
  assumption:  s is in network byte order (big-endian)
---------------------------------------------------------------------------*/

#define BP_PUT8(d,s)   (*(d) = ((uint8)(s))); (d)++; 
#define BP_PUT16(d,s)  BP_PUT8(d, (s) >> 8); BP_PUT8(d,s);
#define BP_PUT32(d,s)  BP_PUT16(d,(s) >> 16); BP_PUT16(d,s);



/*---------------------------------------------------------------------------
  Message and extension type definitions for generated messages

  protocol type
   ICMP     10   Router Solicitation (code = 0)

   UDP       1   Registration Request
                 extension types:
                  32  Mobile Node-Home Agent authentication
                  36  Generalized Mobile IP Authentication extension
                   subtypes:
                      1      Mobile AAA authentication
                  131 Mobile Network Access Identifier extension
                  132 Mobile Node received Foreign Agent challenge

---------------------------------------------------------------------------*/
#define MIP_MSGTYPE_RRQ               (1)
#define MIP_EXTTYPE_AUTH_GEN          (36)
#define MIP_GEN_EXT_SUBTYPE_AAA_AUTH  (1)

/*---------------------------------------------------------------------------
  Reverse tunneling flag definition for RRQ
---------------------------------------------------------------------------*/
#define MIP_RRQ_FLAG_REV_TUN (0x2)

#ifdef FEATURE_DATA_PS_MIP_CCOA
/*---------------------------------------------------------------------------
  D flag definition for RRQ - CCoA Mode - MN decapsulation
---------------------------------------------------------------------------*/
#define MIP_RRQ_FLAG_MN_DECAPSULATE (0x20)
#endif /* FEATURE_DATA_PS_MIP_CCOA */

/*---------------------------------------------------------------------------
   Destination IP address for Agent Solicitation (224.0.0.1 in network order)
---------------------------------------------------------------------------*/
#define IP_MULTICAST (0xffffffffLU)



/*lint -save -e641 Reason for suppressing error 641*/

/*===========================================================================

                             INTERNAL DATA 

===========================================================================*/

#if 0
/*---------------------------------------------------------------------------
   MN-AAA authentication algorithm lookup table (by SPI)
---------------------------------------------------------------------------*/
static mip_auth_lookup_table mn_aaa_auth_lookup[2] =
{
  { MIP_CHAP_SPI, (mip_auth_fptr_type)mip_auth_calc_chap },
  { 0, NULL } /* final entry handler must be NULL, key indicates NOT_FOUND */
};
#endif /* IF 0 */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIPOUT_SEND_SOL()

DESCRIPTION
  Composes and expedites an agent solicitation via the mip_io module.

  The format and expected values of this message are described in the
  Mobile Interface Specification.

  NOTE: ASM IP TTL must be 1

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_outmsg_send_sol 
(
  void
)
{
  mip_router_sol_msg      router_sol;      /* ICMP router solicitation msg */
  mip_event_payload_type  mip_event_payload;  /* MIP event data structure  */
  boolean                 result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Build the agent solicitation message which is identical to an ICMP
    router solicitation message as per RFC 1256.  No payload, and header
    is built in dssicmp.c so we don't have to do anything here!
  -------------------------------------------------------------------------*/
  router_sol.reserved = 0x0;

  DATA_MIP_MSG1(MSG_LEGACY_HIGH, "** Sending ASM (@ %x)", &router_sol);
 
  /*-------------------------------------------------------------------------
    Send the message to the foreign agent
    - IP ttl = 1  (set in mipioi_init)
    - IP src addr = my IP or 0
    - IP dst addr = all systems multicast (224.0.0.1)
    - IP dst port = don't care?
  -------------------------------------------------------------------------*/
  result = mipio_write ( MIPIO_ICMP_TX_HANDLE, 
                         &router_sol, 
                         sizeof (router_sol),
                         IP_MULTICAST,
                         0 ); 

  /*-----------------------------------------------------------------------
    Generate MIP event - ASM sent
  -----------------------------------------------------------------------*/
  mip_event_payload.mip_event  = EVENT_MIP_ASM_SENT;
  mip_event_payload.info.i_asm = result ? MIP_EVENT_INFO_SENT_OTA
                                        : MIP_EVENT_INFO_SENT_FAIL; 
  event_report_payload(EVENT_MIP, 
                       sizeof (mip_event_payload), 
                       &mip_event_payload);

} /* mip_outmsg_send_sol() */



/*===========================================================================
FUNCTION MIPOUT_SEND_RRQ()

DESCRIPTION
  Composes and sends a registration request to the specified foreign agent.

  The format and expected values of this message are described in the
  Mobile Interface Specification.

NOTES:
  * we ask to use the first listed COA in the agent advertisement as per
    rfc2002bis
  * we generate the following extensions:
    - MN_NAI
    - MN-HA authentication
    - MN-FA challenge, if rx'd in AAM or last RRP from this FA
    - MN-AAA authentication, if MN-FA challenge is present

PARAMETERS
  session - pointer to session info block for which we are registering.
             (RRQ is based on this and the mobility agent info block)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void mip_outmsg_send_rrq
(
  mip_session_info_type *  session
)
{
  nv_stat_enum_type  nv_status;            /* return val for ps_nv_read    */
  mip_rrq_msg        rrq;/* registration request msg */
  byte *             ext;                  /* extension pointer            */
  uint32             spi;                  /* temporary var for SPI        */
  byte *             mn_ha_ss  = NULL;     /* MN-HA shared secret ptr      */
  byte *             mn_aaa_ss = NULL ;    /* MN-AAA shared secret ptr     */
  uint32             len;                  /* length to hash for MN-HA     */
  uint16             t_uint16;             /* temporary uint16             */
  boolean            result;               /* result of mipio_write()      */
  byte               mn_ha_ss_len = 0;         /* MN-HA shared secret length   */
  byte               mn_aaa_ss_len = 0;        /* MN-AAA shared secret length  */
#ifdef FEATURE_DS_MOBILE_IP_DMU
  byte               dmu_ix;               /* index for dmu_info           */
  dmu_info_type      *mip_dmu_info_ptr;
#ifdef FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT
  byte               dmu_padding_len;      /* for DMU clear text mode      */
#endif /* FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT */
#endif /* FEATURE_DS_MOBILE_IP_DMU */
  mip_event_payload_type  mip_event_payload;  /* MIP event data structure  */
  uint16             ext_len = 0;
  nv_item_type      *ps_nv_item_ptr;
  mip_ma_info_type  *ma_info_ptr;
  ds707_mip_profile_type  *ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rrq.id1 = rrq.id2 = 0;

  DATA_MIP_MSG2(MSG_LEGACY_HIGH, "** Compose RRQ for session %d (info @ %x)", 
            MIP_SES_PTR_2_INDEX(session), 
            &rrq);

  /*-------------------------------------------------------------------------
    Check to see if one second has passed since last RRQ.  If not, skip RRQ.
  -------------------------------------------------------------------------*/
  if (session->rsm_info.one_second_since_last_rrq == FALSE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_MED, "RRQ was sent less than a second ago, dump this one");
    return;
  };

  ma_info_ptr = mip_get_ma_info_ptr();
  /*-------------------------------------------------------------------------
    Check to see if foreign agent address is known.  If not, skip RRQ.
  -------------------------------------------------------------------------*/
  if (ma_info_ptr->ma_addr == 0)
  {
    DATA_MIP_MSG0(MSG_LEGACY_MED, "We don't know FA addr, skipping RRQ");
    return;
  }

  /*-------------------------------------------------------------------------
    Build the registration request body
  -------------------------------------------------------------------------*/
  rrq.type      = MIP_MSGTYPE_RRQ;
  rrq.flags     = 0;

  /*-------------------------------------------------------------------------
    Set reverse tunneling bit in RRQ as provisioned on the mobile.

    Note that this is an intolerant setting (me MUST reverse tunnel or we
    MUST NOT reverse tunnel, for mn_request_rev_tun = 1, 0 respectively).
  -------------------------------------------------------------------------*/
  if (session->mn_request_rev_tun)
  {
    rrq.flags |= MIP_RRQ_FLAG_REV_TUN;

    /*-----------------------------------------------------------------------
      To think about:
      If the mobile requires reverse tunneling but reverse tunneling is 
      not supported by one of the mobility agents, we could fail the call 
      here rather than waste time trying to register.  But this will require
      retooling of the state machines.
    -----------------------------------------------------------------------*/
  }

#ifdef FEATURE_DATA_PS_MIP_CCOA
  /*-------------------------------------------------------------------------
    Set the D bit ON in flags when operating in CCoA mode 
  -------------------------------------------------------------------------*/
  if(dshdr_get_mobile_ip_uses_ccoa())
  {
    rrq.flags |= MIP_RRQ_FLAG_MN_DECAPSULATE;
  }
#endif /* FEATURE_DATA_PS_MIP_CCOA */

  rrq.lifetime  = dss_htons(ma_info_ptr->ma_lifetime);
 
  /*-------------------------------------------------------------------------
    Check to be sure that the home address has been provisioned, or if not
    that an NAI is present to identify the mobile.  If not, we can't
    register, so cause an error!
  -------------------------------------------------------------------------*/
  if ((session->home_addr == PS_INADDR_ANY) && (session->nai_len == 0))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "NAI not specified!  Home addr must be provisioned.");
    return;
  }

  /*-------------------------------------------------------------------------
    IP addresses are stored in network order, so no translation is required
    here. 
  -------------------------------------------------------------------------*/
  rrq.home_addr = session->home_addr;
  rrq.ha_addr   = session->ha_addrs[session->curr_ha_index];
#ifdef FEATURE_DATA_PS_MIP_CCOA
  if(dshdr_get_mobile_ip_uses_ccoa())
  {
    rrq.coa = session->curr_ccoa;
  }
  else
  {
    /* FA nmode operation */
    rrq.coa = ma_info_ptr->ma_addr;
  }
#else
  rrq.coa = ma_info_ptr->ma_addr;
#endif /* FEATURE_DATA_MIP_CCOA */

  /*-------------------------------------------------------------------------
    Allocate temporary memory for the NV item
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr = (nv_item_type *)
    ps_system_heap_mem_alloc(sizeof(nv_item_type));
  if( ps_nv_item_ptr == NULL )
  {
    DATA_ERR_FATAL("Mem alloc from system heap failed.");
    return;
  }

  /*-------------------------------------------------------------------------
    Read the time offset from NV and save it in the session info block.
  -------------------------------------------------------------------------*/
  ps_nv_item_ptr->ds_mip_mn_ha_time_delta.index = session->profile_index;
  nv_status = ps_get_nv_item(NV_DS_MIP_MN_HA_TIME_DELTA_I, ps_nv_item_ptr);

  /*-------------------------------------------------------------------------
    If profile hasn't been written to NV yet, set time offset to zero. 
  -------------------------------------------------------------------------*/
  if (nv_status != NV_DONE_S)
  {
    DATA_MIP_MSG1(MSG_LEGACY_HIGH, "time_offset for profile %d never written",
              session->profile_index);
    ps_nv_item_ptr->ds_mip_mn_ha_time_delta.time_delta = 0;
  }

  session->time_offset = ps_nv_item_ptr->ds_mip_mn_ha_time_delta.time_delta;

  PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);

  /*-------------------------------------------------------------------------
    Get the NTP timestamp.  Note that the following assumes big-endian 
    architecture.  Add the HA/MN timebase offset correction to the id.
  -------------------------------------------------------------------------*/
  ds_get_ntp (&rrq.id1, &rrq.id2); 

  if (rrq.id1 == 0 || rrq.id2 == 0 )
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "RRQ id not is not set correctly");
  }
  rrq.id1 += session->time_offset;
  DATA_MIP_MSG3(MSG_LEGACY_HIGH, "registration ID is %x.%x (offset %x)",
            rrq.id1, rrq.id2, (session->time_offset));

  /*-------------------------------------------------------------------------
    Save the ID so we can match RRP to this RRQ
  -------------------------------------------------------------------------*/
  session->reg_id_msw = rrq.id1;
  session->reg_id_lsw = rrq.id2;

  /*-------------------------------------------------------------------------
    Convert the RRQ ID to network byte order
  -------------------------------------------------------------------------*/
  rrq.id1 = dss_htonl(rrq.id1);
  rrq.id2 = dss_htonl(rrq.id2);

  /*-------------------------------------------------------------------------
    Build the MN Network access identifier extension
  -------------------------------------------------------------------------*/
  ext = (byte *) &rrq.extensions;

  if (session->nai_len != 0)
  {
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "copying nai (%d bytes)",session->nai_len);

    BP_PUT8(ext, MIP_EXTTYPE_MN_NAI);
    BP_COPY8(ext, &session->nai_len);
    memscpy(ext,(sizeof(mip_extensions)-2), session->nai, session->nai_len);
    ext += session->nai_len; 
  }

  if(NVRUIM_3GPD_MIP_NV_SIP_NV == uim_3gpd_support() ||
     NVRUIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
  {
    /*-----------------------------------------------------------------------
      Retrieve shared secret data. 

      If using RUIM, SS data is maintained on the card, and cannot
      be retrieved.  Hashing using SS data is done by the card

      Note: for now, it's ok to use ps_nv_item for temporary storage since we 
            are the only caller of ps_get_nv_item - this will need to be 
            fixed if anyone else calls this function.

      JD - consider, for security purposes, splitting the following function 
      into two functions - "get SS pointers" and "retrieve SS" so a malicious 
      user cannot call this any time to retrieve SS info
    -----------------------------------------------------------------------*/
    mip_auth_retrieve_ss_data( session,
                               &mn_ha_ss, 
                               &mn_ha_ss_len, 
                               &mn_aaa_ss, 
                               &mn_aaa_ss_len );
  }

  /*-------------------------------------------------------------------------
    Build the MN-HA Authentication extension

    If no MN-HA shared secret is provided, the extension is still included,
    the authenticator calculated as if the shared secret was of length 0.
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG0(MSG_LEGACY_LOW, "Building MN-HA Authentiation");

  BP_PUT8(ext, MIP_EXTTYPE_AUTH_MN_HA);
  BP_PUT8(ext, 4 + MIP_AUTH_MD5_LEN);

  /*-------------------------------------------------------------------------
    Note: RFC2002 authenticator calculation *DOES NOT* include SPI
  -------------------------------------------------------------------------*/
  len = (ext - (byte *) &rrq); 

  /*-------------------------------------------------------------------------
    Set MN-HA SPI to provisioned value. 
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Later, implement an SPI lookup according to authentication algorithm, 
    i.e. something like:

    spi = mip_auth_lookup_spi (mn_ha_auth_lookup, 
                               calc_auth_md5_prefix_plus_suffix);


  NOTE FOR RUIM - WHAT IF NO SPI IS SPECIFIED?????
  -------------------------------------------------------------------------*/
  spi = session->mn_ha_spi;
  spi = dss_htonl(spi);
  BP_COPY32 (ext, (byte *) &spi);               /* TYPE CASTING ESSENTIAL! */

  DATA_MIP_MSG0(MSG_LEGACY_LOW, "calculating MD5 authenticator");

  /*-------------------------------------------------------------------------
    Check to see if RFC 2002bis style authentication is enabled -
    authenticator calculation *DOES* include SPI
  -------------------------------------------------------------------------*/
  ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
  if (ds_1x_runtime_profile_ptr->mip.ds_qcmipt_val & 
                  MIP_QCMIPT_MN_HA_AUTH_2002BIS_BIT)
  {
    len += sizeof(uint32);               /* rfc2002 doesn't hash SPI value */
  }

  ext_len = mip_auth_calc_md5_prefix_plus_suffix( &rrq, 
                                        len, 
                                        (void *) mn_ha_ss, 
                                        mn_ha_ss_len,
                                        ext,
                                        session->profile_index,
                                        NULL,
                                        0 );
  if (ext_len != 0 ) 
  {
    ext += ext_len;
  }
  else 
  {
    ext += MIP_AUTH_MD5_LEN;
  }

  /*-------------------------------------------------------------------------
    If the foreign agent issued a challenge in the agent advertisement,
    include the MN-FA challenge extension in the registration request.
  -------------------------------------------------------------------------*/
  if (ma_info_ptr->challenge_length)
  {
    /*-----------------------------------------------------------------------
      Build the mn-fa challenge extension
    -----------------------------------------------------------------------*/
    BP_PUT8(ext, MIP_EXTTYPE_MN_FA_CHAL);
    BP_COPY8(ext, &ma_info_ptr->challenge_length);
    memscpy( ext, 
             sizeof(mip_extensions),
             ma_info_ptr->fa_challenge,
             ma_info_ptr->challenge_length );
    ext += ma_info_ptr->challenge_length;
  }

  /*-------------------------------------------------------------------------
    If the foreign agent sent a challenge, or Dynamic Mobile Update is 
    in progress, the mobile must also provide the AAA authentication 
    extension so the AAA can authenticate the mobile.  
  -------------------------------------------------------------------------*/
 
  if (ma_info_ptr->challenge_length 
#ifdef FEATURE_DS_MOBILE_IP_DMU
      || session->dmu == TRUE
#endif /* FEATURE_DS_MOBILE_IP_DMU */
     )
  {

#ifdef FEATURE_DS_MOBILE_IP_DMU
    /*-----------------------------------------------------------------------
      If DMU in progress, build the DMU_Key_Data extension
    -----------------------------------------------------------------------*/
    if (session->dmu == TRUE)
    {
      dmu_ix = MIP_SES_PTR_2_INDEX (session);
      mip_dmu_info_ptr = mip_get_dmu_info_ptr_by_index( dmu_ix );

      if(!mip_dmu_info_ptr->encrypted)
      {
        /*-------------------------------------------------------------------
          AAA wants a DMU, but we haven't finished encrypting the keys yet.

          So just drop this RRQ for now, hopefully by the time the re-rrq 
          timer expires, we will have finished encryption.

          JD - better, post an event to reduce the rrq_cnt by one for this
               case.
        -------------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_MED, "Encryption not yet complete, skip RRQ");
        return;
      }

      DATA_MIP_MSG0(MSG_LEGACY_MED, "Adding MIP_KEY_DATA extn");
      
#ifdef FEATURE_DS_MOBILE_IP_DMU_DEBUG

      memdump ((void *)&(mip_dmu_info_ptr->key_data), 
               sizeof (mip_dmu_info_ptr->key_data));
      
      memdump ((void *)&(mip_dmu_info_ptr->key_data.aaa_auth[0]),
               MIP_DMU_AAA_AUTH_LEN );

#ifndef FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT
        DATA_MIP_MSG0(MSG_LEGACY_MED, "Encrypted Key data");
        memdump ((void *)&(mip_dmu_info_ptr->encrypted_key_data), 
                 sizeof (mip_dmu_info_ptr->encrypted_key_data));
#endif /* FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT */
#endif /* FEATURE_DS_MOBILE_IP_DMU_DEBUG */

      /*---------------------------------------------------------------------
        make critical vendor specific extension header first:
        - length is 138 = 6 (header) + 128 (encrypted keys) + 4 (PKI)
      ---------------------------------------------------------------------*/
      BP_PUT8(ext, MIP_EXTTYPE_CVSE);
      BP_PUT8(ext, 0);
/*lint -save -e572 Reason for suppressing error 572 - typecast wont work if s is a constant*/
      BP_PUT16(ext, 138);  
      BP_PUT32(ext, ((uint32) MIP_VSE_VERIZON_ORG_ID & 0x00FFFFFF));
      BP_PUT16(ext, MIP_DMU_CVSETYPE_DMU_KEY_DATA);
/*lint -restore Restore lint error 572*/

#ifdef FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT
      /*---------------------------------------------------------------------
        If clear text debug feature is defined, send key_data without 
        encrypting.
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_MED, "DMU DEBUG: Use DMUV=7 (Clear Text)");
      memscpy( ext,
               sizeof(mip_extensions),
               (byte *)&(mip_dmu_info_ptr->key_data),
               sizeof(dmu_key_data_type) );
      ext += sizeof(dmu_key_data_type);
        
      /*---------------------------------------------------------------------
          Now add the zero padding (69 = 128 - 59 )
      ---------------------------------------------------------------------*/
        dmu_padding_len =
          MIP_DMU_ENCRYPTED_KEY_DATA_LEN - sizeof(dmu_key_data_type);
        
        memset (ext, 0, dmu_padding_len);
        ext += dmu_padding_len;
      
#else /* encrypted DMU payload */

      /*---------------------------------------------------------------------
        Build encrypted key data VSE. 
      ---------------------------------------------------------------------*/
        memscpy (ext,
                 sizeof(mip_extensions) ,
                 mip_dmu_info_ptr->encrypted_key_data,
                 MIP_DMU_ENCRYPTED_KEY_DATA_LEN);
        ext += MIP_DMU_ENCRYPTED_KEY_DATA_LEN;

      /*---------------------------------------------------------------------
        Clear the mn auth from memory for security reasons
      ---------------------------------------------------------------------*/
        memset ((mip_dmu_info_ptr->key_data.mn_auth),
                0,
                MIP_DMU_MN_AUTH_LEN);
#endif /* !FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT */
      
      /*---------------------------------------------------------------------
        make Public Key Identifier
        - currently, only support PK_EXPANSION 0xFF = default (no extension)
        - currently, only support AV 1 = RSA-1024
        - currently, only support DMUV 0 = DMU v1.3
      ---------------------------------------------------------------------*/
      BP_COPY8(ext, &(mip_dmu_info_ptr->pk_info.pkoid));
      BP_COPY8(ext, &(mip_dmu_info_ptr->pk_info.pkoi));
      BP_PUT8(ext, MIP_DMU_PK_VERSION);

#ifdef FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT
      /*---------------------------------------------------------------------
        If clear text feature is enabled, use DMU version 7 (unencrypted).
      ---------------------------------------------------------------------*/
        BP_PUT8(ext, MIP_DMU_AV << 4 | MIP_DMU_DMUV_CLEARTEXT);
#else /* encrypted mode */
      /*---------------------------------------------------------------------
        Insert DMU version 1.3
      ---------------------------------------------------------------------*/
        BP_PUT8(ext, MIP_DMU_AV << 4 | MIP_DMU_DMUV);
#endif /* FEATURE_DS_MOBILE_IP_DMU_DEBUG_CLEARTEXT */
    }
#endif /* FEATURE_DS_MOBILE_IP_DMU */

    /*-----------------------------------------------------------------------
      Build the MN-AAA authentication extension. 

      Note AUTH_CHAP_LEN and calc_auth_chap() are hard coded and not
      configurable.
    -----------------------------------------------------------------------*/
    BP_PUT8(ext, MIP_EXTTYPE_AUTH_GEN);
    BP_PUT8(ext, MIP_GEN_EXT_SUBTYPE_AAA_AUTH);
    t_uint16 = 4 + MIP_AUTH_MD5_LEN;
    t_uint16 = dss_htons (t_uint16);
    BP_COPY16(ext, (byte *) &t_uint16);         /* TYPE CASTING ESSENTIAL! */

    /*-----------------------------------------------------------------------
      should look like:
      spi = mip_auth_lookup_spi( mn_aaa_auth_lookup, calc_auth_chap );

      NOTE FOR RUIM - WHAT IF NO SPI IS SPECIFIED????
    -----------------------------------------------------------------------*/
    spi = session->mn_aaa_spi;
    spi = dss_htonl(spi);
    BP_COPY32 (ext, (byte *) &spi);             /* TYPE CASTING ESSENTIAL! */

    /*-----------------------------------------------------------------------
      If the foreign agent sent a challenge, the mobile must also provide
      the AAA authentication extension so the FA can authenticate the 
      mobile.  So build the MN-AAA authentication extension. 

      Reuse 'spi' as return value from calc_auth_chap()
    -----------------------------------------------------------------------*/
    spi = mip_auth_calc_chap( 
            &rrq, 
            ext - (byte *) &rrq,
            (void *) mn_aaa_ss,
            mn_aaa_ss_len,
            ma_info_ptr->fa_challenge,
            ma_info_ptr->challenge_length,
            ext,
            session->profile_index );
    ext += spi;
  }

  /*-------------------------------------------------------------------------
    Send the RRQ message to the foreign agent
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG2(MSG_LEGACY_LOW, "Sending RRQ to %x:%d",
            dss_htonl(ma_info_ptr->ma_addr), MIP_UDP_PORTNO);
  result = mipio_write( MIP_SES_PTR_2_INDEX(session),
                        &rrq, 
                        ext - (byte *) &rrq,
                        ma_info_ptr->ma_addr,
                        MIP_UDP_PORTNO ); 
  if (result == TRUE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "RRQ Sent");

    mip_increment_mip_stat_info(RRQ_CNT_ID);

    /*-----------------------------------------------------------------------
      Set the holdoff timer to make sure RRQ's are sent no less than 1s apart
    -----------------------------------------------------------------------*/
    result = ps_timer_start (session->rsm_info.rrq_holdoff_timer, 1000);
    ASSERT (result != PS_TIMER_FAILURE);

    /*-----------------------------------------------------------------------
      Clear the holdoff flag to suppress RRQ's for a second.

      Do it after setting the timer.  There is no race condition 
      because:
      i)  the following line should take less than 1 second to execute.
      ii) for the case where the timer is active and the corresponding RRP 
          is received before this timeout occurs, resetting the timer above
          will ensure that the callback isn't called, putting the holdoff
          variable TRUE incorrectly.
    -----------------------------------------------------------------------*/
    session->rsm_info.one_second_since_last_rrq = FALSE;
     
    DATA_MIP_MSG1(MSG_LEGACY_MED, "Set session %d RRQ holdoff timer for 1s", 
             MIP_SES_PTR_2_INDEX(session));
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "RRQ send failed");
  }

  /*-------------------------------------------------------------------------
    Generate MIP event - RRQ sent
  -------------------------------------------------------------------------*/
  mip_event_payload.mip_event  = EVENT_MIP_RRQ_SENT;
  mip_event_payload.info.i_rrq = result ? MIP_EVENT_INFO_SENT_OTA
                                        : MIP_EVENT_INFO_SENT_FAIL; 
  event_report_payload(EVENT_MIP, 
                       sizeof (mip_event_payload), 
                       &mip_event_payload);

} /* mip_outmsg_send_rrq() */

#ifdef FEATURE_DS_MOBILE_IP_DEREG
/*===========================================================================
FUNCTION MIPOUT_SEND_DEREG_RRQ()

DESCRIPTION
  Composes and sends a deregistration request to the specified foreign agent.

  The format and expected values of this message are described in the
  Mobile Interface Specification.

NOTES:
  * we ask to use the first listed COA in the agent advertisement as per
    rfc2002bis
  * we generate the following extensions:
    - MN_NAI
    - MN-HA authentication
    - MN-FA challenge, if rx'd in AAM or last RRP from this FA
    - MN-AAA authentication, if MN-FA challenge is present

PARAMETERS
  session - pointer to session info block for which we are registering.
             (RRQ is based on this and the mobility agent info block)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void mip_outmsg_send_dereg_rrq
(
  mip_session_info_type *  session
)
{
  mip_ma_info_type *ma_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get ma_info pointer.
  -------------------------------------------------------------------------*/
  ma_info_ptr = mip_get_ma_info_ptr();

  ma_info_ptr->ma_lifetime = 0;
  DATA_MIP_MSG0(MSG_LEGACY_MED, "Sending Deregistration RRQ");
  mip_outmsg_send_rrq (session);
} /* mip_outmsg_send_dereg_rrq() */

/*lint -restore Restore lint error 641*/

#endif  /* FEATURE_DS_MOBILE_IP_DEREG */

#endif  /* FEATURE_DS_MOBILE_IP */
