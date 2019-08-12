/*===========================================================================
                                      
                          D S M I P _ P A R S E . C

DESCRIPTION

 The Data Services Mobile IP input parser implementation.  Contains API 
 and internal functions to parse incoming messages from mobility agents.

 NOTE:  If the parser is passed a mangled or erroneous message as an input,
        we attempt to identify error conditions.  However, these errors will
        not always be logged, since the code will be optimized for speed
        (short circuit return if message is irrelevant)
 
EXTERNALIZED FUNCTIONS

  mip_parse_msg()
    Parses a message, signals the MIP state machine appropriately.

Copyright (c) 2005-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_parse.c_v   1.44   14 Feb 2003 17:34:44   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_parse.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/08/13    vl     Optimization of F3 messages
09/13/13    cx     Fix the error usage of sizeof in memscpy 
06/27/13    cx     Replace memcpy and memmove with memscpy and memsmove
06/03/13    sd     ATCoP decoupling changes
05/01/12    sb     Always On UIM Feature Cleanup
05/20/11    op     Migrated to MSG 2.0 macros
05/04/11    ttv    Added changes to compute mip statistics.
03/15/11    mg     Global variable cleanup
04/11/11    jee    Fix to handle RRP error code 129
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
02/11/11    ack    Global Variable Cleanup
01/10/11    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/07/10    ttv    Added messages which explain the reason for asserting.
09/03/10    jee    Modifications to handle RRP error code 129
07/16/10    ssh    Fixed compiler warning due to -Wstrict-prototypes
04/23/10    gc     Lint fixes
03/29/10    gc     Do not resent RRQ if RRP with error code 75 is received.
01/12/10    ms     Replaced UIM_3GPD Constants with NVRUIM_3GPD
08/17/09    ps     Added support to re-send RRQ if RRP with error code 74/75 
                   is recieved.
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
04/23/09    ls     Add the support to report MIP rrp code
08/21/07    sc     Added fix for mip de-reg so that ASM is not sent for RRP 
                   error code 104,105 & 106
06/25/07    jd     Added support for IS-835-D DNS vendor-specific extension
05/23/07    vp     MIP CCoA related changes.
05/10/07    ac     Lint fix
12/07/06    as     Added support for parsing a mip deregistration rrp.
07/22/05    kvd    Set mip_fail_reasons while parsing RRP to reportto DCTM.
06/16/05    kvd    Fixed AAM seq_no rollover check to re-register on seq_no
                   reset also.
02/25/05    jd/gr  Featurised the check for mip_get_session_info_cb to handle
                   DMU key requests for MULTIPLE_NAI scenarios
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
05/18/04    jk     Added profile index parameter to MD5 algorithm function
                   call.
04/21/04    kvd    Moved RRP rcvd event logging to report events inorder wrt 
                   RRQ logging
04/20/04    jd     Silent discard on authenticable RRP(131)
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
01/17/04    jd     Changes for synchronous DMU key data encryption
01/17/04    jd     Add 'dropped' bit to MIP RRP event.
                   Cleaned up MIP event generation code.
08/29/03    jd     Silently discard unauthenticable RRPs from HA (code > 127)
07/15/03    jd     Set DMU flag during re-encryption on account of different
                   PKOID in RRQ(Key_Request).
06/23/03    jd     Add CVSE support for non-DMU builds
06/23/03    jd     Retry on authenticable RRP(131), comment cleanup.
06/23/03    jd     Pick up HA unicast IP from RRP(136) for RFC2002-style DHA
06/16/03    jd     Don't immediately fallback to next HA when authentication 
                   fails 
06/11/03    jd     Set RTT estimate when RRP(106) estimate is received
06/11/03    jd     Remove byte alignment dependencies from mip_parsei_rrp
06/03/03    jd     Don't pick up HA and home IPs on bad MN-HA auth or bad SPI
03/07/03    js     Added Mobile IP event reporting. 
02/14/03    jay    Fail dmu calls if initialization or encryption fails
12/29/02    jd     Fixed includes for multimode DMU builds
12/23/02    jd     Move logic to save dmu passwords into dmu_commit_key_data 
12/17/02    jd     Post SSM_RXED_AGT_ADV_EV before RSM_MOVE_EV to avoid
                   skipping 1st RRQ
12/17/02    jd     Save CHAP ss in new PPP password NV item
                   Update lcp_auth_info for immediate use of DMU updated pw
10/23/02    jay    Fixed MN re-transmiting RRQs when talking to Non-DHA PDSNs
10/09/02    jd     Pick up HA address from other failure codes for DHA.
10/01/02    jay    Added parsing for DMU Invalid pub key extension.
09/21/02    jd     Pick up HA address from authenticated failure RRP's (133)
09/21/02    jd     Fixed bug - save assigned HA address in current ha entry
                   rather than always primary.
09/20/02    jay    Formatted debug messages.
08/16/02    jay    Modified QCMIPT to check for the RFC2002bis bit mask.
07/23/02    jd     Call mip_auth_retrieve_ss_data to get mn-ha SS
07/18/02    jay    Regenerate keys and encrypt them after we get a successful
                   AAA_Auth during DMU process.
06/14/02    jd     Handle FA RRP codes before checking MN-HA authenticator
05/07/02    jd     Moved FA info block to dsmip.c
04/24/02    jd     Optimization - don't bother resoliciting on stale 
                   challenge if a new FAC value was included in RRP.
04/17/02    jd     Use global ps_nv_item when retrieving nv items
03/16/02    jd     Cleaned up MSG_* 
03/14/02    jd     Added support for MIP DMU 
02/06/02    jay    Use shadow_nv shared secret if it is set (CR 20432)
11/21/01    js     Post immediate solicitation event to the sol sm when 
                   registration failed because the foreign agent rejects 
                   the challenge value (or it wasn't present) sent by the 
                   mobile in the RRQ.
10/16/01    jd     Reset RRQ holdoff flag when RRP is received so we don't 
                   delay registration any longer than necessary.
10/16/01    jd     Check if RRP ID matches before parsing to avoid extra work 
09/11/01    mvl    Now saving the RRP HA as the primary HA (only in RAM)
09/10/01    jd     Added handling for failed authentication RRP failure. 
08/27/01    jd     If RRP registration lifetime is 0, fail and try next HA.
08/24/01    mvl    Now copying the FA challenge whenever we get one (we used
                   only to do it for successful messages).
08/16/01    jd     Time offset NV read changed to reflect restructuring in 
                   nv.h
08/14/01    jd     If RRP returns "administratively prohibited", fallback
                   to SIP
08/14/01    jd     If bad SPI or authentication fails, try next home agent.
08/14/01    jd     Check if RRP comes from FA, otherwise throw it away.
08/14/01    jd     added from_ip argument to mip_parse_msg()
08/02/01    jd     Write time offset to NV
                   Dependency on MSM_NV.01.00.34 VU Release
07/17/01    jd     RRP common failure case generates HA_UNAVAIL event now.
                   Fixed bug in MN-HA authenticator validation (length 
                   parameter in compare was not specified correctly).
                   Changed mip_parse_msg argument types for memory efficiency
                   Replaced questionable C syntax in FA challenge memcpy() 
                   call with standard ansi C.  
                   Removed code that blanked last byte of fa challenge.
07/10/01    na     Fixed some comments
07/06/01    mvl    Storing the SID/NID/PZID -> PDSN mapping everytime an
                   Agent Advert is received.  Also removed code related
                   reverse tunnelling failures.                   
06/28/01    mvl    Fixed move detection algorithm
06/27/01    mvl    Fixed bug where lifetime was not set to min of RRQ and RRP
06/26/01    jd     switch between rfc2002/bis MN-HA authenticator calculation 
                   method based on AT$QCMIPT value
06/19/01    jd     check RRP SPI against provisioned value and use fixed
                   authentication algorithm (MD5) instead of lookup
06/14/01    jd     handle RRP failure due to stale challenge
06/14/01    jd     fix subnet comparison bug
06/13/01    mvl    updated references to match names in session info block
06/11/01    jd     dsmip_parse_msg accepts arbitrary pointer to data to parse 
10/25/00    jd     Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "ds707_pkt_mgr.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "memory.h"
#include "amssassert.h"
#include "err.h"
#include "dsmip_msg.h"
#include "dsmip_vse_msg.h"
#include "dsmip_auth.h"
#include "dsmip_parse.h"
#include "dsmip_regsm.h"
#include "dsmip_solsm.h"
#include "dsmip_metasm.h"
#include "dsmip_pdsn_db.h"
#include "dsmip_cfg.h"
#include "dssocket.h"
#include "md5.h"
#include "nv.h"
#ifdef FEATURE_DS_MOBILE_IP_DMU
#include "dsmip_dmu.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU */

#include "ps_byte.h"
#include "dsat707mipctab.h"
#include "dsat707mip.h"
#include "ps_utils.h"

#include "dsmip_event_defs.h"
#include "nvruimi.h"
#include "ds_1x_profile.h"
#include "ps_system_heap.h"
#include "dsmip_v.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define LONG_ALIGNED(addr)  (((uint32) addr % sizeof(uint32)) == 0)
#define FLAG_SET(x,fl)     (x & fl)

/*---------------------------------------------------------------------------
  Message and extension type definitions for handled messages

  protocol type
   ICMP     9   Router Advertisement
                 extension types:
                  0   One-byte Padding (no Length nor Data field)
                  5   Foreign Agent challenge 
                  16  Mobility Agent Advertisement
                  19  Prefix-Lengths

   UDP      3   Registration Reply
                 extension types:
                  0   One-byte Padding (no Length nor Data field)
                  32  Mobile Node-Home Agent authentication
                  33  Mobile Node-Foreign Agent authentication
                  34  Foreign Agent-Home Agent authentication
                  89  Vendor Specific Reason (+ DMU CVSE = DMU messages)
                  131 Mobile Network Access Identifier extension
                  132 Mobile Node received Foreign Agent challenge 
                   
---------------------------------------------------------------------------*/
#define ICMP_EXTTYPE_PADDING                (0)
#define ICMP_EXTTYPE_AG_ADV                 (16)
#define ICMP_EXTTYPE_PREFIX_LEN             (19)
#define ICMP_EXTTYPE_MN_FA_CHAL             (24)

#define  MIP_MSGTYPE_RRP                    (3)
#define  MIP_EXTTYPE_AUTH_MN_FA             (33)
#define  MIP_EXTTYPE_AUTH_FA_HA             (34)


/*---------------------------------------------------------------------------
  Masks to extract bit field entries in mobility agent advertisement.

  What we define in our code as the 'flags' field is composed of bits:

  76543210  (from MSB->LSB)
  ||||||||   
  |||||||+-  Reverse tunneling supported by Foreign Agent
  ||||||+--  (reserved)
  |||||+---  This agent will accept datagrams w/ GRE encapsulation
  ||||+----  This agent will accept datagrams w/ minimal IP hdr encapsulation
  |||+-----  This is a foreign agent(can be HA too)
  ||+------  This is a home agent(can be FA too)
  |+-------  This foreign agent will not accept registrations right now
  +--------  Registration is required for co-located care-of-address

---------------------------------------------------------------------------*/
#define AG_ADV_FL_REG_REQ  (0x80)   /* must register, no CL-COA            */
#define AG_ADV_FL_BUSY     (0x40)   /* reg. not accepted right now         */
#define AG_ADV_FL_HA       (0x20)   /* will serve as HA on this link       */
#define AG_ADV_FL_FA       (0x10)   /* will serve as FA on this link       */
#define AG_ADV_FL_MIN_ENC  (0x08)   /* supports min. encapsulation         */
#define AG_ADV_FL_GRE_ENC  (0x04)   /* supports gre. encapsulation         */
#define AG_ADV_FL_RSV_1    (0x02)   /* reserved (was V.J. IP hdr compr.)   */
#define AG_ADV_FL_REV_TUN  (0x01)   /* supports reverse tunnelling         */




/*---------------------------------------------------------------------------
   Macros for subnet comparison
---------------------------------------------------------------------------*/
#define SUBNET(ipaddr,subnet_len) \
  ((ipaddr)&((unsigned int) 0xffffffff << (32-(subnet_len))))
#define SAME_SUBNETS(a,a_len,b,b_len) \
  ((SUBNET(a,a_len) == SUBNET(b,b_len)) && (a_len == b_len))


/*===========================================================================

                             INTERNAL DATA 

===========================================================================*/

#if 0
/*---------------------------------------------------------------------------
   MN-HA authentication algorithm lookup table (by SPI)
---------------------------------------------------------------------------*/
static mip_auth_lookup_table mn_ha_auth_lookup[2] = 
{
  { MIP_MD5_SPI, (mip_auth_fptr_type) mip_auth_calc_md5_prefix_plus_suffix },
  { 0, NULL } /* final entry handler must be NULL, key indicates NOT_FOUND */
};
#endif /* IF 0 */


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_PARSEI_ROUTER_ADV()

DESCRIPTION
  This function function parses the ICMP router advertisement and
  any mobility extensions present.  

  The IP header information is not passed in.

  The format and expected values of this message are described in the
  Mobile Interface Specification.

NOTES:
* Router Advertisement Checksum is not verified.  It is not specified in 
  RFC 2002 whether this will include extensions or not, so we ignore.
* The addresses and preferences are ignored - NOT: subnet prefix lengths 
  extension refers to router addresses, i.e. subnet prefix length of care 
  of addresses is determined by matching the CofA to the router address.
* Sequence Number indicates re-registration when value is both less than
  the previous value and 256.
* Agent adv. Registration lifetime is measured in seconds
* Prefix Length(s) indicate "the number of leading bits that define the 
  network number of the corresponding Router Address listed in the ICMP
  Router Advertisement portion of the message".

PARAMETER 
  r_adv - pointer to the ICMP router advertisement message
  len   - length of 'r_adv' (in bytes)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

static void mip_parsei_router_adv 
(
  mip_router_adv_msg *     r_adv,         /* data received                 */
  int                      len            /* data length                   */ 
)
{
  byte *                          end;             /* end of router adv    */
  byte *                          ext;             /* pointer to extension */

  mip_session_info_type *         session;         /* ptr to session info  */

  /* temporary pointers for parsing */

  r_info *                          r;               /* router info        */
  mip_router_adv_ext_pad *          ext_padding;     /* padding ext.       */
  mip_router_adv_ext_ma_adv *       ext_ma_adv;      /* agent adv ext.     */
  mip_router_adv_ext_prefix_len *   ext_prefix_len;  /* prefix length ext. */
  mip_ext_mn_fa_chal *              ext_chal;        /* FA challenge ext.  */

  /* working variables for move detection */

  uint32  maa_ipaddr;                   /* advertising MA's IP address     */
  uint32  maa_subnet;                   /* advertising MA's subnet         */
  byte    maa_subnet_len;               /* advertising MA's subnet length  */
  uint32  home_subnet;                  /* mobile's home (HA) subnet       */
  byte    home_subnet_len = 0;          /* mobile's home (HA) subnet len.  */
  uint32  mn_subnet;                    /* mobile's current subnet         */
  byte    mn_subnet_len;                /* mobile's current subnet length  */

  boolean all_homeaddrs_same_subnet;  /* common home subnet for all sessions */
  boolean moved;                        /* flag: mobile has changed subnet */
  int     i;                            /* temp variable */

  mip_event_payload_type  mip_event_payload;  /* MIP event data structure  */
  mip_ma_info_type *ma_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Generate MIP event - AAM received
  -----------------------------------------------------------------------*/
  mip_event_payload.mip_event  = EVENT_MIP_AAM_RCVD;
  mip_event_payload.info.i_aam = MIP_EVENT_INFO_NA;
  event_report_payload( EVENT_MIP, 
                        sizeof (mip_event_payload), 
                        &mip_event_payload );
  
  /*-------------------------------------------------------------------------
    Calculate the end of message so we know when to stop parsing
  -------------------------------------------------------------------------*/
  end = (byte *) r_adv + len;

  /*-------------------------------------------------------------------------
    Extensions start after the router advertisement message =
     icmp_router_adv_header + n_addrs * (router address + preference level)
  -------------------------------------------------------------------------*/
  r = &r_adv->router_info;
  ext = (byte *) &r[r_adv->num_addrs];

  /*=========================================================================
    PARSE step 1 - scan message to determine which extensions are present.
  =========================================================================*/

  /*-------------------------------------------------------------------------
    Translate byte ordering of this message to the local platform.
    We can only translate RRQ portion now, translate extensions based on
    the extension types while scanning the message.  The calculations above
    rely on byte values which don't need to be translated, so they're ok.
  -------------------------------------------------------------------------*/
  r_adv->lifetime = dss_ntohs(r_adv->lifetime);

  /*-------------------------------------------------------------------------
    Clear extension pointers.  These will be set to point at the appropriate
    extension if present in the agent advertisement.  If multiple extensions
    of a given type are present, the last valid one will be handled.  NOTE
    that this should not occur according to RFC 2002 (x.x.x)
  -------------------------------------------------------------------------*/
  ext_ma_adv     = NULL;
  ext_padding    = NULL;
  ext_prefix_len = NULL;
  ext_chal       = NULL;

  while (ext < end)
  {
    /*-----------------------------------------------------------------------
      Go through each extension in sequence, save pointers to the relevant
      extensions.  The extension type is determined by the 'type' field 
      which is the first byte.  
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_LOW,"Extension type : %d",ext[0]);
    switch (ext[0])  
    {
      case ICMP_EXTTYPE_PADDING:
        /*-------------------------------------------------------------------
          The padding extension (RFC 2002 S 2.1.3)

          Optionally included to comply with IP protocol implementations that 
          require ICMP messages to an even length. It has no meaning, so 
          ignore (skip) it.  Note this extension SHOULD be last if present. 
        -------------------------------------------------------------------*/
        if (ext_padding != NULL)
        {
          /* the msg should contain only one extensions of each type */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Multiple padding extensions present!");
        }
        ext_padding = (mip_router_adv_ext_pad *) ext;
        ext++;
        continue;

      case ICMP_EXTTYPE_AG_ADV:
        if (!LONG_ALIGNED(ext))
        {
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Ext. not long aligned - skipping AAM");
          return;
        }
        /*-------------------------------------------------------------------
          Agent advertisment extension (RFC 2002 S 2.1.1)
        -------------------------------------------------------------------*/
        if (ext_ma_adv != NULL)
        {
          /* the msg should contain only one extensions of each type */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra agent advertisement ext. present!");
        }
        ext_ma_adv = (mip_router_adv_ext_ma_adv *) ext;

        /*-------------------------------------------------------------------
          Translate byte ordering of this extension to the local platform.
          We calculate the number of COA's present based on the length 
          field, i.e. n_addrs = (length - all other fields) / sizeof(coa).
          The magic number 6 is the size of the fixed fields (except type
          and length which aren't included in the length field) in the
          agent adv.
        -------------------------------------------------------------------*/
        ext_ma_adv->seq_num      = ps_ntohs(ext_ma_adv->seq_num);
        ext_ma_adv->reg_lifetime = ps_ntohs(ext_ma_adv->reg_lifetime);
        break;

      case ICMP_EXTTYPE_PREFIX_LEN:
        /*-------------------------------------------------------------------
          Prefix length extension (RFC 2002 S 2.1.2)
        -------------------------------------------------------------------*/
        if (ext_prefix_len != NULL)
        {
          /* the msg should contain only one extensions of each type */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra prefix length extension present!");
        }
        ext_prefix_len = (mip_router_adv_ext_prefix_len *) ext;
        break;

      case ICMP_EXTTYPE_MN_FA_CHAL:
        /*-------------------------------------------------------------------
          FA challenge extension (RFC 3012 S ?)
        -------------------------------------------------------------------*/
        if (ext_chal != NULL)
        {
          /* the msg should contain only one extensions of each type */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra FAC extension present!");
        }
        ext_chal = (mip_ext_mn_fa_chal *) ext;
        break;

      default:
        DATA_MIP_MSG1(MSG_LEGACY_MED, "Unrecognized extension (type %d)", ext[0]);
        if (ext[0] < 128) 
        {
          /*-----------------------------------------------------------------
            according to RFC 2002, if the extension is not recognized
            and < 128, the message should be silently discarded.  So 
            return without performing actions indicated by other extensions
          -----------------------------------------------------------------*/
          DATA_MIP_MSG0(MSG_LEGACY_MED, "Ignoring extension");
          return;
        }
        /*-------------------------------------------------------------------
          otherwise, the extension should just be skipped
        -------------------------------------------------------------------*/
        break;
    }

    /*-----------------------------------------------------------------------
      Advance 'ext' to point to the next extension.  Every extension has the
      length field in the second byte position EXCEPT the padding extension
      (which doesn't contain a length field).  (ref: RFC 2002, 1.8)
    -----------------------------------------------------------------------*/
    ext += ext[1] + 2;
  }

  /*=========================================================================
    PARSE step 2 - apply validation logic to filter out advertisements that 
                   should not affect the mobile IP subsystem. 

    Conditions causing the message to be ignored (= continue to solicit):
    1.  No agent advertisment extension present
    2.  After step 1, the pointer scanning the message does not end up
        with the length parameter (that returned by dss_recvfrom()).  
    3.  MA cannot serve mobile (busy bit set)
    4.  MA supported lifetime is 0.
    5.  Mobile on foreign subnet, advertising MA is not a FA (F bit clear)
    6.  Mobile on home subnet, advertising MA is not a HA (H bit clear)
  =========================================================================*/

  /*-------------------------------------------------------------------------
    2.1: If mobility agent advertisement extension present, handle the msg. 
  -------------------------------------------------------------------------*/
  if (!ext_ma_adv)
  {
    /*-----------------------------------------------------------------------
      Otherwise, this is just a plain ICMP router advertisement, ignore it
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "No mobility info in RA, ignoring");
    return;
  }

  /*-------------------------------------------------------------------------
    2.2: Check: the end of extensions line up with the end of the datagram.  
                 (if not, this is a malformed message, ignore).
  -------------------------------------------------------------------------*/
  if (ext != end)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Malformed AAM - size wrong, ignoring");
    return;
  }

  /*-------------------------------------------------------------------------
    2.3:  Check if the busy bit is set (the foreign agent can't serve any  
          more clients, so ignore the agent advertisement) 
 
          We hope that there is another FA on the subnet that is not busy.
          If we don't get any other agent advertisements, we simply cannot 
          do mobile IP and drop down to simple IP (this logic is located in 
          the meta state machine)
  -------------------------------------------------------------------------*/
  if (FLAG_SET(ext_ma_adv->flags, AG_ADV_FL_BUSY))
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "AAM busy bit set, ignoring");
    return;
  }

  /*-------------------------------------------------------------------------
    2.4: Check: if the lifetime is 0, registering is useless, so ignore.  
  -------------------------------------------------------------------------*/
  if (ext_ma_adv->reg_lifetime == 0)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "AAM with lifetime of 0, ingoring");
    return;
  }

  /*-------------------------------------------------------------------------
    Check for registration required (R) bit
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_MIP_CCOA
  if (dshdr_get_mobile_ip_uses_ccoa() && 
      !(FLAG_SET(ext_ma_adv->flags, AG_ADV_FL_REG_REQ)))
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "AAM has R-bit NOT set, ignoring");
  }
#endif /* FEATURE_DATA_PS_MIP_CCOA */

  /*-------------------------------------------------------------------------
    We need to know whether we are on the home or a foreign subnet for the
    next two checks.  So do the following:
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Extract the mobility agent's IP address from the advertisment.  
    We make the assumption that the first address listed is that of the 
    MA.  (ref: MIP MIS, x.x)
  -------------------------------------------------------------------------*/
  maa_ipaddr = ext_ma_adv->COA_array;

  /*-------------------------------------------------------------------------
    Determine the subnet prefix length for this care-of-address.  

    If there is a router prefix length extension present and has an entry 
    for a router with the same IP address as the COA, we can infer the 
    COA's subnet.

    If we can't infer the COA subnet length, we consider the entire
    IP address, i.e. we use subnet prefix length = 32 for IPv4.  
  -------------------------------------------------------------------------*/
  maa_subnet_len = 32;
  if (ext_prefix_len)
  {
    for (i=0; i < r_adv->num_addrs; i++)
    {
      if (r[i].router_addr == maa_ipaddr)
      {
        /*-------------------------------------------------------------------
          Found the router with the same IP address as COA!
          The corresponding subnet prefix length is valid for the COA.
          NOTE:  we could replace the break with error logging here to 
                 flag multiple advertisements of the same router.
        -------------------------------------------------------------------*/
        maa_subnet_len = MIP_AGENT_ADV_EXT_PREFIX(ext_prefix_len, i);
        break;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Get ma_info pointer.
  -------------------------------------------------------------------------*/
  ma_info_ptr = mip_get_ma_info_ptr();

  /*-------------------------------------------------------------------------
    Use the prefix length to form the subnet mask.
  -------------------------------------------------------------------------*/
  maa_subnet    = SUBNET(maa_ipaddr, maa_subnet_len);
  mn_subnet_len = ma_info_ptr->ma_prefix_len;
  mn_subnet     = SUBNET(ma_info_ptr->ma_addr, mn_subnet_len);

  /*-------------------------------------------------------------------------
    Determine the home subnet for this mobile.  Set the home subnet value to
    zero if there is no unique home subnet (in the case of multiple 
    simultaneous sessions).
  -------------------------------------------------------------------------*/
  session = mip_get_ses_info_array();
  all_homeaddrs_same_subnet = TRUE;
  home_subnet = 0;
  for (i=0; i < MIP_MAX_SESSIONS; i++, session++)
  {
    /*-----------------------------------------------------------------------
      An inactive session is indicated by a 0 current home address.  This 
      value should be set to the provisioned value before solicitation
      begins. 
    -----------------------------------------------------------------------*/
    if (session->home_addr != 0)
    {
      if (home_subnet == 0)
      {
        home_subnet_len = (uint8) session->home_addr_subnet_len; 
        home_subnet     = SUBNET( session->home_addr, home_subnet_len );
      }
      else
      {
        if (!SAME_SUBNETS(home_subnet, 
                          home_subnet_len, 
                          session->home_addr, 
                          session->home_addr_subnet_len))
        {
          all_homeaddrs_same_subnet = FALSE;
        }
      }
    }
  }

  /*-------------------------------------------------------------------------
    Check to see if this agent can service our mobile, i.e. either
    i)  the FA bit is set, or
    ii) the advertised subnet is the home subnet for all sessions AND 
        the HA bit is set.

    If neither apply, ignore the advertisement in hopes that we'll find a
    different MA that can serve the mobile.
  -------------------------------------------------------------------------*/
  if(!(FLAG_SET(ext_ma_adv->flags, AG_ADV_FL_FA)) && 
     !(all_homeaddrs_same_subnet &&
       FLAG_SET(ext_ma_adv->flags, AG_ADV_FL_HA)))
  {
    DATA_MIP_MSG0(MSG_LEGACY_MED, "MA not HA and FA bit not set, ignoring");
    return; 
  }

  /*-------------------------------------------------------------------------
    Now check to see if we need to register.

    Conditions that warrant a registration with a new MA:
    1.  Changed subnets.  mip_send_rrq determines whether a registration or
         deregistration is required in this case.

    Conditions that warrant a re-registration with the current MA:
    2.  Sequence number rollover (i.e. < previous value) to < 256.
    3.  Subnet prefix length is known for the current MA, but the latest
        agent advertisement does not include the prefix length (i.e. 
        something changed on the network side, so reregister to be safe)
  -------------------------------------------------------------------------*/

  moved = FALSE;

  /*-------------------------------------------------------------------------
    If we are currently registered, check to see if the advertising 
    mobility agent is on a different subnet.  

    If so, this indicates we have moved and therefore must register with 
    the MA on the new subnet. 

    If not, check to see if the advertised sequence number indicates that
    the MA requires all mobiles to re-register.
  -------------------------------------------------------------------------*/
  if(!SAME_SUBNETS(mn_subnet, mn_subnet_len, maa_subnet, maa_subnet_len))
  {
    /*-----------------------------------------------------------------------
      Subnet has changed, so we have moved!
    -----------------------------------------------------------------------*/
    moved = TRUE;
    DATA_MIP_MSG0(MSG_LEGACY_MED, "Rx'd AAM from new subnet, re-reregister");
  }
  else
  {
    /*-----------------------------------------------------------------------
      If the previous sequence number was > 255, check if the sequence 
      number has rolled over to < 256.  This necessitates a manditory 
      re-registration according to RFC 2002.
    -----------------------------------------------------------------------*/
    if (ext_ma_adv->seq_num < 256
        && ma_info_ptr->last_seq_num > ext_ma_adv->seq_num)
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "Sequence number rollover, re-register");
      moved = TRUE;
    }
  }
  DATA_MIP_MSG1(MSG_LEGACY_MED, "AAM sequence #%d", ext_ma_adv->seq_num);

  if (ext_chal)
  {
    /*---------------------------------------------------------------------
      If a foreign agent challenge extension is present, save the 
      challenge value and length in the mobility agent info block.
      NOTE:  The challenge is required to be at least 32 bits (?)
    ---------------------------------------------------------------------*/
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "FA challenge length = %d", ext_chal->length);
    ma_info_ptr->challenge_length = ext_chal->length;
    if (ma_info_ptr->challenge_length < 4)
    {
      /*-------------------------------------------------------------------
        RFC 2002 - SHOULD be minimum 4 bytes
      -------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "FAC value < 4 bytes!  Using anyway");
    }

    memscpy ( ma_info_ptr->fa_challenge, 
              sizeof(ma_info_ptr->fa_challenge),
              ext_chal->challenge, 
              ext_chal->length );
  }
  else
  {
    /*---------------------------------------------------------------------
      If not, zero the challenge length to indicate the foreign agent
      did not challenge the mobile.
    ---------------------------------------------------------------------*/
    ma_info_ptr->challenge_length = 0;
  }

  /*-------------------------------------------------------------------------
    Post 'received agent advertisement' event to solicitation state 
    machine.    
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG0(MSG_LEGACY_MED, "AAM processed successfully, notify SSM");
  mip_increment_mip_stat_info(ADV_CNT_ID);
  mip_sol_sm_post_event (SSM_RXED_AGT_ADV_EV);

  /*-------------------------------------------------------------------------
     Finally, signal the state machine if we have moved. 
  -------------------------------------------------------------------------*/
  if (moved)
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Mobile has moved subnets.");
    /*-----------------------------------------------------------------------
      (ref: MIP SDD, 4.4.5.4.x: agent advertisment handling)
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Insert FA into the PDSN database.
    -----------------------------------------------------------------------*/
    pdsn_db_put(maa_ipaddr);

    /*-----------------------------------------------------------------------
      Save mobility agent information in the info block.  We don't need
      the old information any longer because we have left its subnet.
    -----------------------------------------------------------------------*/
    ma_info_ptr->ma_addr            = maa_ipaddr;
    ma_info_ptr->ma_prefix_len      = maa_subnet_len;
    ma_info_ptr->ma_lifetime        = ext_ma_adv->reg_lifetime; 
    ma_info_ptr->last_seq_num       = ext_ma_adv->seq_num; 
    ma_info_ptr->ma_supp_rev_tun    = (ext_ma_adv->flags & AG_ADV_FL_REV_TUN) ?
                                   TRUE :
                                   FALSE;

    /*-----------------------------------------------------------------------
      post a move event to the registration state machine for each session
      to indicate that we have encountered a new mobility agent, 
      i.e. have detected a move to a new subnet.
    -----------------------------------------------------------------------*/
    for (i=0; i < MIP_MAX_SESSIONS; i++)
    {
      mip_reg_sm_post_event( INDEX_2_MIP_SES_PTR(i), RSM_MOVE_EV );
    }
  }
}


/*===========================================================================

FUNCTION MIP_PARSEI_RRP

DESCRIPTION
  This function parses the registration reply received from the 
  foreign agent.  The registration reply contains the following fields:

  The format and expected values of this message are described in the
  Mobile Interface Specification.

  Notes:
      Lifetime: # of seconds until the registration expires
                0xffff = infinity
                0 = mobile node has been deregistered
               (only valid if code field indicates reg. accepted)
      Home Address: The IP address of the mobile node.
      Home Agent: The IP address of the mobile node's home agent.
      Identification: the identification number of the registration 
               request generating this reply, ciphered as determined
               by the SPI value in the Mobile-Home Authentication 
               Extension.

PARAMETERS
  session - the session that received the RRP 
  rrp     - pointer to the received registration reply message
  len     - length of 'rrp'

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void mip_parsei_rrp
(
  mip_session_info_type *  session,
  mip_rrp_msg *            rrp,
  uint16                   len
)
{
  byte               mn_ha_auth[MIP_AUTH_MAX_LEN];
  nv_stat_enum_type  nv_status;            /* return val for ps_nv_read    */
  nv_item_type      *ps_nv_item_ptr;
 
  /* parser data */
  byte *  ext;                                   /* pointer to extension   */
  byte *  end;                                   /* pointer to end of msg  */
  mip_rrq_ext_mn_nai *      ext_mn_nai;          /* mobile NAI             */
  mip_ext_mn_fa_chal *      ext_mn_fa_chal;      /* MN-FA challenge        */
  mip_rrq_ext_mn_ha_auth *  ext_mn_ha_auth;      /* MN-HA authorisation    */

#ifdef FEATURE_DS_MOBILE_IP_DMU
  /* DMU data */
  dmu_info_type *  dmu_info;                 /* pointer to DMU info        */

  /* DMU parser data */
  dmu_key_request_ext_type *    key_request_ext;      /* Key request       */
  dmu_inval_pub_key_ext_type *  inval_pub_key_ext;    /* Key data invalid  */
  dmu_aaa_auth_ext_type *       aaa_auth_ext;         /* AAA authenticator */
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  mip_3gpp2_dns_nvse_type *     dns_ext = NULL;   /* DNS server addresses  */
  mip_3gpp2_dns_nvse_type *     temp_dns_ext = NULL;   

  uint32  vse_vendor;                           /* temp for VSE vendor ID  */
  uint16  vse_type;                             /* temp for VSE type       */

  uint32  spi;

  byte *  mn_ha_ss = NULL;
  byte    mn_ha_ss_len = 0;
  byte *  mn_aaa_ss = NULL;
  byte    mn_aaa_ss_len = 0;

  boolean   retry;        /* flag - retry registration w/ current HA       */
  boolean   fa_fail;      /* flag - failure at FA, abort w/o trying 2nd HA */
  boolean   retrieve_ip_addrs;      /* flag - pull HA, Home addr from RRP  */

  ps_ip_addr_type   pri_dns_addr;  /* temp storage for primary DNS address */
  ps_ip_addr_type   sec_dns_addr;  /* temp storage for secondary DNS addr  */

  mip_event_payload_type  mip_event_payload;  /* MIP event data structure  */
  mip_ma_info_type       *ma_info_ptr;
  ds707_mip_profile_type *ds_1x_runtime_profile_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* RRP code needs to be reported */
  session->mip_rrp_code = rrp->code;
  DATA_MIP_MSG1(MSG_LEGACY_MED, "RRP received with rrp code = %d",
                session->mip_rrp_code);
   mip_increment_mip_stat_info(RRP_CNT_ID);
  /*-----------------------------------------------------------------------
    Prepare the RRP Receive event payload with the default event details:
    - Event is always RRP rx'd
    - Zero field is always 0
    - RRP code is always the incoming RRP message code
    - Generally, the RRP is not silently discarded.  
      If it is, 'dropped' field will be modified at that time.
  -----------------------------------------------------------------------*/ 
   mip_event_payload.mip_event                 = EVENT_MIP_RRP_RCVD;
   mip_event_payload.info.zero                 = 0;
   mip_event_payload.info.i_rrp.rrp_code       = rrp->code;
   mip_event_payload.info.i_rrp.flags.dropped  = FALSE;

  /*-------------------------------------------------------------------------
    Verify that the ID is the same as that sent in the RRQ, otherwise
    toss the RRP.

    Note we need to convert id2 to host order to do comparison.  We don't
    alter the message contents at this point since we still need to 
    validate authentication before processing.
  -------------------------------------------------------------------------*/
  if (ps_ntohl(rrp->id2) != session->reg_id_lsw)
  {
    DATA_MIP_MSG2(MSG_LEGACY_ERROR, "RRP received with bad ID = %x.%x", 
               ps_ntohl(rrp->id1), ps_ntohl(rrp->id2));
    goto _SILENTLY_DISCARD_RRP;
  }
  else
  {
    DATA_MIP_MSG2(MSG_LEGACY_MED, "RRP ID matches, RRQ ID = %x.%x", 
             ps_ntohl(rrp->id1), ps_ntohl(rrp->id2));
  }

  /*-------------------------------------------------------------------------
    Reset RRQ one second holdoff timer - no need to wait before sending new 
    RRQ since we have received the expected RRP already.
  -------------------------------------------------------------------------*/
  session->rsm_info.one_second_since_last_rrq = TRUE; 

  /*-------------------------------------------------------------------------
    Calculate the end of message so we know when to stop parsing
  -------------------------------------------------------------------------*/
  end = (byte *) rrp + len;

  /*-------------------------------------------------------------------------
    Extensions start after the router advertisement message =
     icmp_router_adv_header + n_addrs * (router address + preference level 
  -------------------------------------------------------------------------*/
  ext = (byte *) rrp + sizeof (mip_rrp_msg);

  DATA_MIP_MSG3(MSG_LEGACY_LOW, "[msg, ext, end] @ [%p, %p, %p]", rrp, ext, end);

  /*=========================================================================
    PARSE step 1 - scan message to determine which extensions are present.
  =========================================================================*/

  ext_mn_nai     = NULL;
  ext_mn_fa_chal = NULL;
  ext_mn_ha_auth = NULL;
 
#ifdef FEATURE_DS_MOBILE_IP_DMU
  key_request_ext   = NULL;
  aaa_auth_ext      = NULL;
  inval_pub_key_ext = NULL;

  /*--------------------------------------------------------------------------
    Determine the DMU info block corresponding to the session info block
    provided by the caller.
    
    FUTURE - reexamine later to see if better method (more efficient/clear)
  --------------------------------------------------------------------------*/
  dmu_info = mip_get_dmu_info_ptr_by_index( MIP_SES_PTR_2_INDEX (session) );
#endif /* FEATURE_DS_MOBILE_IP_DMU */

  while (ext < end)
  {
    DATA_MIP_MSG2(MSG_LEGACY_LOW, "extension (type %d) @ %p", ext[0], ext);

    switch (ext[0])  /* first byte is the extension type field */
    {
#ifdef FEATURE_DATA_INTERNAL_TEST_ONLY
      case ICMP_EXTTYPE_PADDING:
        ASSERT(0);
        /*-------------------------------------------------------------------
          The padding extension (RFC 2002 S 2.1.3)

          Optionally included to comply with IP protocol implementations that 
          require ICMP messages to an even length. Note this shouldn't be
          found in the UDP messages, but the Solaris MIP implementation does
          insert padding extensions occasionally, so we deviate here out of
          necessity for development purposes.  This should be fixed later. 
        -------------------------------------------------------------------*/
         ext++;
        continue;
#endif /* FEATURE_DATA_INTERNAL_TEST_ONLY */

      case MIP_EXTTYPE_MN_NAI:
        /*-------------------------------------------------------------------
          Mobile Network Access Identifier is specified in the MIS, but
          serves no purpose so we don't process it (but we don't throw out
          the RRP if it is present).
        -------------------------------------------------------------------*/
        if (ext_mn_nai != NULL)
        {
          /* there should only be one of each extension present in the msg */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra MN NAI extension present!");
        }
        ext_mn_nai = (mip_rrq_ext_mn_nai *) ext;
        break;

      case MIP_EXTTYPE_MN_FA_CHAL:
        /*-------------------------------------------------------------------
          MN-FA challenge is provided by the FA for the next RRQ 
          (to be used for re-registration, as per IS-835 S 6.2.2.3)
        -------------------------------------------------------------------*/
        if (ext_mn_fa_chal != NULL)
        {
          /* there should only be one of each extension present in the msg */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra FAC extension present!");
        }
        ext_mn_fa_chal = (mip_ext_mn_fa_chal *) ext;
        break;

      case MIP_EXTTYPE_AUTH_MN_HA:
        /*-------------------------------------------------------------------
          Can't have multiple mn-ha auth extensions
        -------------------------------------------------------------------*/
        if (ext_mn_ha_auth != NULL)
        {
          /* there should only be one of each extension present in the msg */
          DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra MN-HA auth extension present!");
        }
        ext_mn_ha_auth = (mip_rrq_ext_mn_ha_auth *) ext;
        break;

      case MIP_EXTTYPE_CVSE:
      {
        vse_vendor = get32(&ext[4]);
        vse_type   = get16(&ext[8]);

        DATA_MIP_MSG2(MSG_LEGACY_LOW,  "CVSE (vendor %d type %d) present",vse_vendor,vse_type);

        switch (vse_vendor)
        {
#ifdef FEATURE_DS_MOBILE_IP_DMU
          case MIP_VSE_VERIZON_ORG_ID:
          {
            switch( vse_type )
            {
              case MIP_DMU_CVSETYPE_KEY_REQ:
                /*-----------------------------------------------------------
                  DMU Key Request, verify not the case that multiple DMU 
                  CVSEs present in the same RRP (i.e. Key_Update and AAA_Auth)
                -----------------------------------------------------------*/
                if ( key_request_ext != NULL  ||
                     aaa_auth_ext != NULL     ||
                     inval_pub_key_ext != NULL )
                {
                  /* should only be one of key_request and aaa_auth present */
                  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra DMU CVSE present!");
                }
                key_request_ext = (dmu_key_request_ext_type *) ext;
                break;
    
              case MIP_DMU_CVSETYPE_AAA_AUTH:
                /*-----------------------------------------------------------
                  DMU AAA Authentor, verify not the case that multiple DMU 
                  CVSEs present in the same RRP (i.e. Key_Update and AAA_Auth)
                -----------------------------------------------------------*/
                if ( key_request_ext != NULL ||
                     aaa_auth_ext != NULL    ||
                     inval_pub_key_ext != NULL )
                {
                  /* should only be one of key_request and aaa_auth present */
                  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra DMU CVSE present!");
                }
                aaa_auth_ext = (dmu_aaa_auth_ext_type *) ext;
                break;
    
              case MIP_DMU_CVSETYPE_PUB_KEY_INVALID:
                /*-----------------------------------------------------------
                  DMU invalid pub key, verify not the case that multiple DMU 
                  CVSEs present in the same RRP (i.e. Key_Update and AAA_Auth)
                -----------------------------------------------------------*/
                if ( key_request_ext != NULL ||
                     aaa_auth_ext != NULL    ||
                     inval_pub_key_ext != NULL )
                {
                  /* should only be one of key_request and aaa_auth present */
                   DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Extra DMU CVSE present!");
                }
                inval_pub_key_ext = (dmu_inval_pub_key_ext_type *) ext;
                break;
    
              default:
                /*-----------------------------------------------------------
                  according to RFC 3015, an unrecognized CVSE cannot be  
                  skipped so toss the RRP. 
                -----------------------------------------------------------*/
                DATA_MIP_MSG1(MSG_LEGACY_MED, "Unrecognized CVSE subtype %d, dropping RRP", 
                         vse_type);
                goto _SILENTLY_DISCARD_RRP;
            
            } /* switch on verizon-specific extension type */
            break;
          }
#endif /* FEATURE_DS_MOBILE_IP_DMU */

          default:
          {
            DATA_MIP_MSG2(MSG_LEGACY_MED,  "Unsupported CVSE (vendor %d, type %d), dropping RRP", 
                     vse_vendor,
                     vse_type);
          
            /*---------------------------------------------------------------
              According to RFC 3015, an unrecognized CVSE cannot be  
              skipped so toss the RRP. 
            ---------------------------------------------------------------*/
            goto _SILENTLY_DISCARD_RRP;
          } /* unrecognized vendor ID */
        }

        /*-------------------------------------------------------------------
          Skip to the end of the VSE.  
          Note that VSE length field is 16 bits, hence the arithmetic.
        -------------------------------------------------------------------*/
        /*lint -save -e527 This code is reached*/
        ext += get16(&ext[2]) + 4;
        /*lint -restore */
        continue;
      }

      case MIP_EXTTYPE_NVSE:
      {
        vse_vendor = get32(&ext[4]);
        vse_type   = get16(&ext[8]);

        DATA_MIP_MSG2(MSG_LEGACY_LOW,  "NVSE (vendor %d type %d) present",vse_vendor,vse_type);

        switch (vse_vendor)
        {
          case MIP_VSE_3GPP2_ORG_ID:
          {
            switch( vse_type )
            {
              case MIP_3GPP2_NVSETYPE_DNS:
                /*-----------------------------------------------------------
                  DNS server addresses.  Note that the extension is present
                  and apply the DNS servers to Um interface upon successful
                  registration below.  This will be done below after the
                  RRP has been authenticated.  Use the last DNS NVSE in the
                  RRP, if multiple DNS NVSEs are present.
                -----------------------------------------------------------*/
                temp_dns_ext = (mip_3gpp2_dns_nvse_type *) ext;   
                if ( ( dns_ext == NULL ) ||
                     ( temp_dns_ext->sender == MIP_DNS_SENDER_HAAA ) ||
                     ( (dns_ext->sender != MIP_DNS_SENDER_HAAA ) &&
                       (temp_dns_ext->sender > dns_ext->sender)
                     )
                   )
                {
                  dns_ext = temp_dns_ext;
                  DATA_MIP_MSG1(MSG_LEGACY_MED, 
                        "Setting DNS extension from sender %d", 
                        dns_ext->sender );
                }
                else 
                {
                  DATA_MIP_MSG0(MSG_LEGACY_MED, "Ignoring DNS extension as already set");
                }
                break;
    
              default:
                /*-----------------------------------------------------------
                  Unrecognized NVSE org-id, ignore it
                -----------------------------------------------------------*/
                DATA_MIP_MSG2(MSG_LEGACY_MED,  "Unsupported NVSE (vendor %d, type %d), ignoring", 
                         vse_vendor, vse_type);
                break;
            } /* switch on 3gpp2-specific extension type */
            break;
          }

          default:
          {
            /*---------------------------------------------------------------
              Unrecognized NVSE org-id, ignore it
            ---------------------------------------------------------------*/
            DATA_MIP_MSG2(MSG_LEGACY_MED,  "Unsupported NVSE (vendor %d, type %d), ignoring", 
                     vse_vendor, vse_type);
            break;
          } /* unrecognized vendor ID */
        }
        break;
      }

      default:
        DATA_MIP_MSG1(MSG_LEGACY_LOW, "Unrecognized extension (type %d)", ext[0]);
        if (ext[0] < 128) 
        {
          /*-----------------------------------------------------------------
            according to RFC 2002, if the extension is not recognized
            and < 128, the message should be silently discarded.  So 
            return without performing actions indicated by other extensions
          -----------------------------------------------------------------*/
          DATA_MIP_MSG0(MSG_LEGACY_HIGH, "unknown critical ext, dropping RRP");
          goto _SILENTLY_DISCARD_RRP;
        }
        /*-------------------------------------------------------------------
          otherwise, the extension should just be skipped
        -------------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_LOW, "ignoring extension");
        break;
    }
    /*-----------------------------------------------------------------------
      advance to the next extension
    -----------------------------------------------------------------------*/
    ext += ext[1] + 2;
  }

  DATA_MIP_MSG1(MSG_LEGACY_LOW, "pointer after extensions @ %p", ext);

  /*-------------------------------------------------------------------------
    2.2: Check: the end of extensions line up with the end of the datagram.  
                 (if not, this is a malformed message, ignore).
  -------------------------------------------------------------------------*/
  if (ext != end)
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "RRP malformed - size wrong!");
    goto _SILENTLY_DISCARD_RRP;
  }

  DATA_MIP_MSG1(MSG_LEGACY_HIGH, "RRP code = %d", rrp->code);

  /*-------------------------------------------------------------------------
    This table summarizes the behaviour for FA RRP codes:
 
    (FUTURE - rename RSM_RRQ_FAIL_W_RETRY_EV to RSM_RRQ_RETRY_EV)

     +----- solicit for new FAC value  (RSM_NEED_NEW_FA_INFO_EV)
     | +--- retry registration         (RSM_RRQ_FAIL_W_RETRY_EV)
     | | +- fail registration          (RSM_RRQ_FAIL_NO_RETRY_EV)
     | | |
     | | |       RRP 
     v v v RFC   CODE  MEANING
     - - - - ---  ----  -------------------------------------------------

    Registration denied by the foreign agent:
 
     . . x 2002   64   reason unspecified
     . . x 2002   65   administratively prohibited
     . . x 2002   66   insufficient resources
     . . x 2002   67   mobile node failed authentication
     . . x 2002   68   home agent failed authentication
     . . x 2002   69   requested Lifetime too long
     . . x 2002   70   poorly formed Request
     . . x 2002   71   poorly formed Reply
     . . x 2002   72   requested encapsulation unavailable
     . . x 2002   73   reserved and unavailable
     . . x 2344   74   requested reverse tunnel unavailable
     . . x 2344   75   reverse tunnel is mandatory and 'T' bit not set
     . . x 2344   76   mobile node too distant
     . . x 2002b  77   invalid care-of address
     . . x 2002b  78   registration timeout
     . . x 2002   80   home network unreachable (ICMP error received)
     . . x 2002   81   home agent host unreachable (ICMP error received)
     . . x 2002   82   home agent port unreachable (ICMP error received)
     . . x 2002   88   home agent unreachable (other ICMP error received)
     . $ $ ????   89   vendor specific reason at FA
     . x . ????   98   missing home agent address
     * * . 3012  104   Unknown challenge
     * * . 3012  105   Missing challenge
     * * . 3012  106   Stale challenge
 
     . . x  (-)  (-)   DEFAULT (any other result code)
 
     * Retry if FAC included in RRP, otherwise trigger solicitation
     $ Retry if DMU failure reason, otherwise fail
  -------------------------------------------------------------------------*/
  retry   = FALSE;
  fa_fail = FALSE;
  retrieve_ip_addrs = TRUE;

  /*-------------------------------------------------------------------------
    Get ma_info pointer.
  -------------------------------------------------------------------------*/
  ma_info_ptr = mip_get_ma_info_ptr();

  /*-------------------------------------------------------------------------
    If Foreign Agent challenge is present, save the challenge for the next
    registration attempt - originally this was only done if the registration
    was successful - for greater interoperability the new challenge will
    always be used.
  -------------------------------------------------------------------------*/
  if (ext_mn_fa_chal)
  {
    ma_info_ptr->challenge_length = ext_mn_fa_chal->length;
    memscpy (ma_info_ptr->fa_challenge,
             sizeof(ma_info_ptr->fa_challenge),
             ext_mn_fa_chal->challenge,
             ma_info_ptr->challenge_length );
  }

  /*-------------------------------------------------------------------------
    Handle foreign agent RRP codes
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG1(MSG_LEGACY_MED,"Foreign agent RRP code : %d",rrp->code);
  switch (rrp->code)
  {
    /*-----------------------------------------------------------------------
      Registration failed because the foreign agent didn't like the 
      challenge value (or it wasn't present) sent by the mobile in the RRQ.

      Generate a solicitation trigger to get new FA challenge value before 
      trying to register again with the current HA.

      Ignore any challenge present in the failed RRP since the standard 
      says we can only include a challenge value in the RRQ from the last 
      AAM or returned in the last successful RRP.  
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_STALE_CHALLENGE:
      /*---------------------------------------------------------------------
        Set the RTT estimation to be added to the RRQ timeout in the case
        where traffic load on the link introduces RRP latency, and RRQ 
        retransmission before receipt of RRP(0) elicits RRP(106) from 
        the PDSN.
      ---------------------------------------------------------------------*/
      mip_reg_sm_set_rtt_estimation(session);
      /* fall through */

    case MIP_RRP_CODE_FAILURE_UNKNOWN_CHALLENGE:
    case MIP_RRP_CODE_FAILURE_MISSING_CHALLENGE:

      if (!ext_mn_fa_chal)
      {
        /*-------------------------------------------------------------------
          Generate the Mobile IP RRP received event.
        -------------------------------------------------------------------*/
        mip_event_payload.info.i_rrp.flags.dropped = TRUE;
        event_report_payload( EVENT_MIP,
                              sizeof (mip_event_payload),
                              &mip_event_payload );

        /*-------------------------------------------------------------------
          No new FAC in the RRP, zero fields that potentially do not get 
          reset in dsmip_parsei_aam 
        -------------------------------------------------------------------*/
        ma_info_ptr->ma_addr       = 0;
        ma_info_ptr->ma_prefix_len = 0;

        /*-------------------------------------------------------------------
          Post event to SSM to obtain new FA info, including FAC
        -------------------------------------------------------------------*/
        DATA_MIP_MSG1(MSG_LEGACY_MED, "FA challenge was absent/old ,"
                      "De-reg lifetime of MIP :%d",ma_info_ptr->ma_lifetime);
        if (ma_info_ptr->ma_lifetime != 0)
	{
            /* ----------------------------------------------------------
	  Post Event to SSM only if MIP is not in De-reg state. 
	  MIP would be in de-reg state if lifetimer is timed out =0
	  ---------------------------------------------------------------*/
          mip_sol_sm_post_event (SSMI_IMMED_SOL_EV);
	}

        /*-------------------------------------------------------------------
          Post an event to reg sm to await new FA info before resending 
          registration
        -------------------------------------------------------------------*/
        mip_reg_sm_post_event (session, RSM_NEED_NEW_FA_INFO_EV);

        /*-------------------------------------------------------------------
          Pick up HA address from RRP
        -------------------------------------------------------------------*/
        if (rrp->ha_addr)
        {
          session->ha_addrs[session->curr_ha_index] = rrp->ha_addr;
          DATA_MIP_MSG1(MSG_LEGACY_MED, "Picking HA Address from RRP (%x)", rrp->ha_addr); 
        }

        /*-------------------------------------------------------------------
          Pick up Home IP address too if HA is known.
        -------------------------------------------------------------------*/
        if (session->ha_addrs[session->curr_ha_index] != DYNAMIC_HA_ADDR)
        {
          session->home_addr = rrp->home_addr;
          DATA_MIP_MSG1(MSG_LEGACY_MED, "Picking Home IP Addr from RRP (%x)", rrp->home_addr); 
        }
       
        /* FUTURE - need to test this case against commercial PDSNs */
        return;
      }
      else
      {
        /*-------------------------------------------------------------------
          Else, we got a new FAC in the RRP, so just try again
        -------------------------------------------------------------------*/
        retry = TRUE;
      }
      break;

    /*-----------------------------------------------------------------------
      Registration failed because reverse tunneling was requested but is 
      not supported by the FA.  
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_FA_CANT_REV_TUN:
      fa_fail = TRUE;
      break;

    /*-----------------------------------------------------------------------
      Registration failed because reverse tunneling was not requested but
      is required by the FA.  
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_FA_MUST_REV_TUN:
      fa_fail = TRUE;
      break;

    /*-----------------------------------------------------------------------
      Registration failed because the mobile was too far from the MA to
      do reverse tunneling.  This should never happen since the IP TTL 
      should be set to 255 for the mip UDP sockets .
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_FA_BAD_TTL:
      fa_fail = TRUE;
      break;

    /*-----------------------------------------------------------------------
      If FA fails authentication (i.e. MN-AAA authenticator was wrong)
      drop the call immediately.
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_FA_MOBILE_NODE_FAILED_AUTH:
      fa_fail = TRUE;
      break;

    /*-----------------------------------------------------------------------
      Administratively prohibited - not sure what this code means.  But 
      fallback to SIP immediately.
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_FA_ADMIN_PROHIBITED:
      fa_fail = TRUE;
      break;

#ifdef FEATURE_DS_MOBILE_IP_DMU

    /*-----------------------------------------------------------------------
      Reason Unspecified - overloaded for DMU specification.

      If CVSE's are present, handle appropriately:

        Mip_Key_request -> send key data
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_FA_VS_REASON:

      /*--------------------------------------------------------------------- 
        Force RRP Fail no retry event if DMU challenge received while doing
        MIP Deregistration
      ---------------------------------------------------------------------*/ 
      if (ma_info_ptr->ma_lifetime == 0) 
      {
        fa_fail = FA_FAIL_DMU_IN_MIP_DEREG;
        break;
      }

      /*---------------------------------------------------------------------
        Make sure DMU initialized correctly

        Note if additional CVSE support is added this check should removed
        here and instead ||'d with each DMU extensions below
      ---------------------------------------------------------------------*/
      if (!mip_is_dmu_rand_ready())
      {
        fa_fail = FA_FAIL_DMU_INIT_FAILED;
        break;
      }

      /*---------------------------------------------------------------------
        If using the UIM for 3GPD Authentication, DMU will fail
      ---------------------------------------------------------------------*/
      if (NVRUIM_3GPD_MIP_NV_SIP_NV != uim_3gpd_support())
      {
        fa_fail = FA_FAIL_3GPD_AUTH_R_UIM;
        break;
      }

      /*---------------------------------------------------------------------
        Got a DMU Key Request.  This means the AAA wants to be updated
        with a new set of passwords for the mobile.
      ---------------------------------------------------------------------*/
      if (key_request_ext)
      {
        /*-------------------------------------------------------------------
          Do not retrieve HA IP address during DMU
        -------------------------------------------------------------------*/
        retrieve_ip_addrs = FALSE;

#ifndef FEATURE_DS_MULTIPLE_NAI
        /*-------------------------------------------------------------------
          Check to make sure we're using a non-volatile set of passwords,
          otherwise fail the call since we won't be able to save new p/ws.
        -------------------------------------------------------------------*/
        if (mip_get_pwd_cb || mip_get_session_info_cb)
        {
          fa_fail = FA_FAIL_USING_UI_USER_INFO;
          break;
        }
#else
        /*-------------------------------------------------------------------
          If using multiple NAI feature, rmsm would have registered these
          callbacks hence allow DMU.
        -------------------------------------------------------------------*/
#endif /* FEATURE_DS_MULTIPLE_NAI */

        if (dsmip_is_using_shadow_nv())
        {
          fa_fail = FA_FAIL_USING_SHADOW_PARAMS;
          break;
        }

        /*-------------------------------------------------------------------
          Check if the encryption is completed. The check applies only to
          intial DMU request. The following are the assumptions
          1) If the phone has just powered up, the encryption should be
             completed by now.  If it has failed, it is due some
             irrecoverable problem and we can't do DMU.
          2) If the pkoid in RRP(Key_Request) differs from that which
             the key data is encrypted, we re-encrypt with the requested
             pkoid and skip sending RRQs until encryption is complete.

          Only edge condition (highly unlikely):

          After successful DMU completion, we start encryption again.
          If we start another DMU exchange (due to resync/rrq lifetime
          expiry, subsequent RRQ, RRP(DMU)) before encryption completes,
          DMU will erroneously terminate.
        -------------------------------------------------------------------*/
        if (!dmu_info->encrypted)
        {
          fa_fail = FA_FAIL_ENCR_FAILED;
          break;
        }

        /*-------------------------------------------------------------------
          Check to see that preencrypted key_data was encrypted with the 
          requested public key
        -------------------------------------------------------------------*/
        if (key_request_ext->pkoid != dmu_info->pk_info.pkoid)
        {
          /*-----------------------------------------------------------------
            If a different key was requested, see if it is provisioned
          -----------------------------------------------------------------*/
          if (!dmu_pkoid_supported(key_request_ext->pkoid))
          {
            /*---------------------------------------------------------------
              If not provisioned, we can't do DMU and so can't make a call
            ---------------------------------------------------------------*/
            DATA_MIP_MSG1(MSG_LEGACY_ERROR,
                          "No public key provisioned for pkoid %d!",
                          key_request_ext->pkoid);
            fa_fail = FA_FAIL_NO_PUBLIC_KEY;
            break;
          }

          /*-----------------------------------------------------------------
            Use the pkoid requested in the RRP and re-encrypt the key data.
            (outmsg won't send RRQs until encryption is complete)
          -----------------------------------------------------------------*/
          DATA_MIP_MSG1(MSG_LEGACY_MED, "Re-encrypting new keys using pkoid %d",
                  key_request_ext->pkoid);
          dmu_info->pk_info.pkoid = key_request_ext->pkoid;
          if (!dmu_encrypt_key_data(dmu_info))
          {
            fa_fail = FA_FAIL_RE_ENCR_FAILED;
            break;
          }
        }

        /*-------------------------------------------------------------------
          Start DMU - set the DMU flag and retry registration
        -------------------------------------------------------------------*/
        if (!session->dmu)
        {
          DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Network initiates DMU key update");
          session->dmu = TRUE;
        }

        retry = TRUE;
        break;
      }

      /*---------------------------------------------------------------------
        Got a DMU AAA Authenticator.  
        This means the AAA has successfully decrypted MIP Key data. 
      ---------------------------------------------------------------------*/
      if (aaa_auth_ext)
      {
        /*-------------------------------------------------------------------
          Do not retrieve HA IP address during DMU
        -------------------------------------------------------------------*/
        retrieve_ip_addrs = FALSE;

        /*-------------------------------------------------------------------
          Verify that the AAA authenticator is the same as that sent with 
          the new keys.
        -------------------------------------------------------------------*/
        if (session->dmu && memcmp(aaa_auth_ext->aaa_auth_val, 
                                   dmu_info->key_data.aaa_auth, 
                                   MIP_DMU_AAA_AUTH_LEN)    == 0 )
        {
          DATA_MIP_MSG0(MSG_LEGACY_HIGH, "DMU complete");

          /*-----------------------------------------------------------------
            Commit the new passwords to NV
          -----------------------------------------------------------------*/
          (void) dmu_commit_key_data (&(dmu_info->key_data));

          /*-----------------------------------------------------------------
            Clear the DMU process state flag
          -----------------------------------------------------------------*/
          session->dmu = FALSE;

          /*-----------------------------------------------------------------
            Generate new random keys and encrypt them.
          -----------------------------------------------------------------*/
          DATA_MIP_MSG0(MSG_LEGACY_LOW, "Generating new keys");
          dmu_gen_key_data (dmu_info);
          (void) dmu_encrypt_key_data (dmu_info);

          /*-----------------------------------------------------------------
            Send another registration using the new keys
            Retry the RRQ to seal the deal on the AAA
          -----------------------------------------------------------------*/
          retry = TRUE;
          break;
        }

        /*-------------------------------------------------------------------
          If not the same AAA Authenticator, silently discard the RRP 
          since it might be a spoof.
        -------------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_MED, "AAA Auth did not match!!");
        goto _SILENTLY_DISCARD_RRP;
      }

      /*---------------------------------------------------------------------
        Got a DMU Invalid public key extension.  
        This means the PKOI used to encrypt the MIP_Key_Data is invalid.
      ---------------------------------------------------------------------*/
      if (inval_pub_key_ext)
      {
        /*-------------------------------------------------------------------
          Assume RRP is sent by correct AAA (not DOS attack) since spoofing
          RRP's should be "difficult" IS-835 networks.

          Fail registration due to bad provisioning of DMU public key
        -------------------------------------------------------------------*/
        fa_fail = FA_FAIL_KEY_NO_MATCH;
        break;
      }

      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Reg denied for unknown reason (vendor specific)");
      fa_fail = FA_FAIL_UNKNOWN_REASON;
      break;

      DATA_MIP_MSG1(MSG_LEGACY_MED,"RRP Code Failure status : %d",fa_fail);

      
#endif /* FEATURE_DS_MOBILE_IP_DMU */
    
    case MIP_RRP_CODE_FAILURE_MISSING_HA_ADDR:
      /*---------------------------------------------------------------------
        Here, handle appropriately IS-835 A/B.
      ---------------------------------------------------------------------*/
      if (rrp->ha_addr != session->ha_addrs[session->curr_ha_index])
      {  
        retry = TRUE;
        break;
      }
      /*---------------------------------------------------------------------
        We encountered a IS-835-B PDSN, but DHA not supported, 
        i.e. fall back to the secondary HA
      ---------------------------------------------------------------------*/  
      retrieve_ip_addrs = FALSE;
      break;

    /*-----------------------------------------------------------------------
      Non-FA or unknown FA RRP code 
    -----------------------------------------------------------------------*/
    default:

      if (rrp->code > 63 && rrp->code < 128)
      {
        /*-------------------------------------------------------------------
          FA error code (range 64-127), i.e.
          registration failed at the FA for some other reason, 
          so we can't hope to register.  Stop.
        -------------------------------------------------------------------*/
        fa_fail = TRUE;
      }
      else 
      {
        /*-------------------------------------------------------------------
          HA return code
        -------------------------------------------------------------------*/
        goto _CONTINUE_PROCESSING_RRP;
      }
      break;
  } /* switch on FA RRP codes */

  if (fa_fail)
  {
    DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Irrecoverable reg failure @ FA, end call");

    /*-----------------------------------------------------------------------
      Generate the Mobile IP RRP received event.
    -----------------------------------------------------------------------*/
    mip_event_payload.info.i_rrp.flags.dropped = TRUE;
    event_report_payload( EVENT_MIP,
                          sizeof (mip_event_payload),
                          &mip_event_payload );
    session->mip_fail_reason= MIP_FAIL_REASON_FA_FAILURE;

    /*-----------------------------------------------------------------------
      Notify REGSM that irrecoverable failure has occurred
    -----------------------------------------------------------------------*/
    mip_reg_sm_post_event(session, RSM_RRQ_FAIL_NO_RETRY_EV);
    return;
  }
  else
  {
    goto _DONE_RRP_PROCESSING;
  }

_CONTINUE_PROCESSING_RRP:

  /*-------------------------------------------------------------------------
    Verify the MN-HA authenticator if present in the RRP,
    otherwise fail registration since this is required!
  -------------------------------------------------------------------------*/
  if (ext_mn_ha_auth)
  {
    /*-----------------------------------------------------------------------
      Copy SPI from message to local var for two reasons - 
      i)  SPI field is not long-aligned in the message.  
      ii) Authenticator value must be calculated with the message in its 
          transmitted (network order) form.
    -----------------------------------------------------------------------*/
    spi = get32(ext_mn_ha_auth->spi);
    DATA_MIP_MSG1(MSG_LEGACY_LOW, "SPI = %x", spi);

    /*-----------------------------------------------------------------------
      Retrieve shared secret data.
    -----------------------------------------------------------------------*/
    mip_auth_retrieve_ss_data( session,
                               &mn_ha_ss,
                               &mn_ha_ss_len,
                               &mn_aaa_ss,
                               &mn_aaa_ss_len );

    /*-----------------------------------------------------------------------
      Calculate the expected authenticator value.
    -----------------------------------------------------------------------*/
    DATA_MIP_MSG0(MSG_LEGACY_LOW, "Calculating authenticator");

    /*-----------------------------------------------------------------------
      Code should be fixed to include user provisioned spi in authentication
      algorithm lookup table.  Once this is done, the following code should
      replace the block below.

    auth_fn   auth_alg;

    auth_alg = (auth_fn) mip_auth_lookup (mn_ha_auth_lookup, spi);
    if (auth_alg)
    {
      len = auth_alg( rrp, 
                      ext_mn_ha_auth->authenticator - (byte *) rrp,
                      ps_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret,
                      ps_nv_item.ds_mip_ss_user_prof.mn_ha_shared_secret_length,
                      mn_ha_auth );
    }
    else
    {
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Unknown SPI (%x) in RRP!  ignoring", 
                 get32(ext_mn_ha_auth->spi));
      goto _SILENTLY_DISCARD_RRP;
    }
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Note: RFC2002 authenticator calculation *DOES NOT* include SPI
    -----------------------------------------------------------------------*/
    len = (uint16) (ext_mn_ha_auth->spi - (byte *) rrp);

    /*-----------------------------------------------------------------------
      RFC2002bisV4 authenticator calculation *DOES* include SPI
    -----------------------------------------------------------------------*/
    ds_1x_runtime_profile_ptr = ds_get_1x_runtime_profile_ptr();
    if (ds_1x_runtime_profile_ptr->mip.ds_qcmipt_val &
                              MIP_QCMIPT_MN_HA_AUTH_2002BIS_BIT)
    {
      len += sizeof(uint32);
    }

    if (spi == session->mn_ha_spi)
    {
      len = mip_auth_calc_md5_prefix_plus_suffix( 
              rrp, 
              len,
              (void *) mn_ha_ss,
              mn_ha_ss_len,
              mn_ha_auth,
              session->profile_index,
              NULL,
              0 );
    }
    else
    {
      DATA_MIP_MSG2(MSG_LEGACY_ERROR,  "prov/RRP SPIs(%d/%d) diff! ignoring",
                 session->mn_ha_spi, 
                 get32(ext_mn_ha_auth->spi)
                 );
      session->mip_fail_reason= MIP_FAIL_REASON_HA_FAILURE;
      goto _SILENTLY_DISCARD_RRP;
    }

    if (memcmp(mn_ha_auth, ext_mn_ha_auth->authenticator, len) != 0)
    {
      DATA_MIP_MSG0(MSG_LEGACY_ERROR, "Bad MN-HA Authenticator! drop RRP");
      session->mip_fail_reason= MIP_FAIL_REASON_HA_FAILURE;
      goto _SILENTLY_DISCARD_RRP;
    }
    else
    {
      DATA_MIP_MSG0(MSG_LEGACY_MED, "MN-HA Authenticator OK!");
    }
  }
  else
  {
    DATA_MIP_MSG0(MSG_LEGACY_ERROR, "RRP from HA, No MN-HA Auth!  Discard");
    goto _SILENTLY_DISCARD_RRP;
  }

  /*-------------------------------------------------------------------------
    Translate byte ordering of this message to the local platform.
    We can only translate RRP portion now, translate extensions based on
    the extension types while scanning the message.  The calculations above
    rely on byte values which don't need to be translated, so they're ok.
  -------------------------------------------------------------------------*/
  rrp->lifetime  = ps_ntohs(rrp->lifetime);
  rrp->id1       = ps_ntohl(rrp->id1);
  rrp->id2       = ps_ntohl(rrp->id2);

  /*-------------------------------------------------------------------------
    This table summarizes the behaviour:
 
     +----- registration succeeded     (RSM_RRQ_SUCC_EV)
     | +--- retry registration         (RSM_RRQ_FAIL_W_RETRY_EV)
     | | +- failure, try next HA       (RSM_RRQ_FAIL_HA_UNAVAIL_EV)
     | | |
     | | |      RRP 
     v v v RFC  CODE  MEANING
     - - - ---  ----  -------------------------------------------------
     * . * 2002   0   registration accepted
     * . * 2002   1   registration accepted, but simultaneous mobility
                         bindings unsupported

    Registration denied by the home agent:
 
     . . x 2002  128  reason unspecified
     . . x 2002  129  administratively prohibited
     . . x 2002  130  insufficient resources
     . x . 2002  131  mobile node failed authentication
     . . x 2002  132  foreign agent failed authentication
     . x . 2002  133  registration Identification mismatch
     . . x 2002  134  poorly formed Request
     . . x 2002  135  too many simultaneous mobility bindings
     . x . 2002  136  unknown home agent address
     . . x 2344  137  requested reverse tunnel unavailable
     . . x 2344  138  reverse tunnel is mandatory and 'T' bit not set
     . . x 2344  139  requested encapsulation unavailable

     . . x  (-)  (-)  DEFAULT (any other result code)

     * Indicate success unless granted lifetime is 0, otherwise try next HA
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Handle home agent RRP codes
  -------------------------------------------------------------------------*/
  DATA_MIP_MSG1(MSG_LEGACY_MED, "Home agent RRP code : %d",rrp->code);
  switch (rrp->code)
  {
    /*-----------------------------------------------------------------------
      Registration success
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_SUCCESS:
    case MIP_RRP_CODE_SUCCESS_NO_SIM_BINDINGS:

      /*---------------------------------------------------------------------
        Save the home agent address as it may have been dynamically 
        assigned or different than that requested in the RRQ.
      ---------------------------------------------------------------------*/
      session->ha_addrs[session->curr_ha_index] = rrp->ha_addr;

      /*---------------------------------------------------------------------
        Registration lifetime is set to lesser of:
        i)  Lifetime advertised by mobility agent in ag_adv msg, and
        ii) RRP lifetime
      ---------------------------------------------------------------------*/
      session->reg_lifetime = MIN(rrp->lifetime, ma_info_ptr->ma_lifetime);
      DATA_MIP_MSG3(MSG_LEGACY_MED, "lifetime: RRP %d, RRQ %d -> using %d",
              rrp->lifetime,
              ma_info_ptr->ma_lifetime, 
              session->reg_lifetime);              

      if (session->reg_lifetime == 0 && ma_info_ptr->ma_lifetime != 0)
      {
        /*-------------------------------------------------------------------
          If RRP registration lifetime is 0, then fail and try next HA.
        -------------------------------------------------------------------*/
        DATA_MIP_MSG0(MSG_LEGACY_ERROR, "lifetime in RRP was 0!  Try next HA");
        break;
      }

      session->home_addr = rrp->home_addr;

      /*---------------------------------------------------------------------
        If DNS server addresses were defined, set the DNS server addresses
        to the Um PS iface now.
      ---------------------------------------------------------------------*/
      if (dns_ext)
      {
        pri_dns_addr.type = IPV4_ADDR;
        memscpy( &pri_dns_addr.addr.v4.ps_s_addr,
                 sizeof(pri_dns_addr.addr.v4.ps_s_addr),
                 (byte *) &(dns_ext->pri_dns_v4addr),
                 sizeof(dns_ext->pri_dns_v4addr) );

        sec_dns_addr.type = IPV4_ADDR;
        memscpy( &sec_dns_addr.addr.v4.ps_s_addr,
                 sizeof(sec_dns_addr.addr.v4.ps_s_addr),
                 (byte *) &(dns_ext->sec_dns_v4addr),
                 sizeof(dns_ext->sec_dns_v4addr) );

        if( -1 == ps_iface_set_v4_dns_addrs( session->iface_ptr,
                                             &pri_dns_addr,
                                             &sec_dns_addr ) )
        {
          DATA_MIP_MSG2(MSG_LEGACY_ERROR, "Couldn't set pri (%x) and sec (%x) DNS server addrs!",
                     pri_dns_addr.addr.v4.ps_s_addr, 
                     sec_dns_addr.addr.v4.ps_s_addr
                     );
        }
      }

      /*---------------------------------------------------------------------
        Generate the Mobile IP RRP received event.
      ---------------------------------------------------------------------*/
      event_report_payload( EVENT_MIP,
                            sizeof (mip_event_payload),
                            &mip_event_payload );

      /*---------------------------------------------------------------------
        Notify the registration state machine of the successful registration  
      ---------------------------------------------------------------------*/
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Registration successful");
      session->mip_fail_reason = MIP_FAIL_REASON_NONE;
      mip_reg_sm_post_event(session, RSM_RRQ_SUCC_EV);
      return;

    /*-----------------------------------------------------------------------
      Registration failed because the mobile's timebase differed from that
      of the HA by too much.  So calculate the offset as the difference 
      between when the RRQ was sent (session->id1) and RRP was rx'd by the
      home agent (rrp->id1) to be used for a subsequent registration retry.
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_HA_REG_ID_MISMATCH:

      /*---------------------------------------------------------------------
        Save the home agent address as it may have been dynamically assigned
        or different than that requested in the RRQ.
      ---------------------------------------------------------------------*/
      session->ha_addrs[session->curr_ha_index] = rrp->ha_addr;

      session->time_offset += rrp->id1 - session->reg_id_msw;
      DATA_MIP_MSG3(MSG_LEGACY_MED, "MN and HA timebases variance too great"
              "timestamp: RRQ(%x) HA(%x) ~%d", 
               session->reg_id_msw,
               rrp->id1, 
               session->time_offset);

      /*---------------------------------------------------------------------
        Allocate temporary memory for the NV item
      ---------------------------------------------------------------------*/
      ps_nv_item_ptr = (nv_item_type *)
        ps_system_heap_mem_alloc(sizeof(nv_item_type));
      if( ps_nv_item_ptr == NULL )
      {
        DATA_ERR_FATAL("Mem alloc from system heap failed.");
        return;
      }

      /*---------------------------------------------------------------------
        Write the time offset to NV 
      ---------------------------------------------------------------------*/
      ps_nv_item_ptr->ds_mip_mn_ha_time_delta.index      = session->profile_index;
      ps_nv_item_ptr->ds_mip_mn_ha_time_delta.time_delta = session->time_offset;
      nv_status = ps_put_nv_item(NV_DS_MIP_MN_HA_TIME_DELTA_I, ps_nv_item_ptr);
  
      /*---------------------------------------------------------------------
        If profile hasn't been written to NV yet, set shared secret lengths
        to zero.  Note: for now, it's ok to use ps_nv_item for temporary
        storage since we are the only caller of ps_get_nv_item - this will
        need to be fixed if anyone else calls this function.
      ---------------------------------------------------------------------*/
      if( nv_status != NV_DONE_S)
      {
        DATA_MIP_MSG1(MSG_LEGACY_HIGH, "error writing time_offset for profile %d",
                  session->profile_index);
      }

      retry = TRUE;

      /*---------------------------------------------------------------------
        Free the temporary NV item memory
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_FREE(ps_nv_item_ptr);

      break;

    /*-----------------------------------------------------------------------
      Home Agent unicast IP was returned by HA in reponse to DHA request
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_UNKNOWN_HA:

      DATA_MIP_MSG1(MSG_LEGACY_MED, "Got unicast HA IP (%x)", rrp->ha_addr);
      retrieve_ip_addrs = TRUE;
      session->mip_fail_reason= MIP_FAIL_REASON_HA_FAILURE;
      retry = TRUE;
      break;

    /*-----------------------------------------------------------------------
      Registration failed because reverse tunneling was requested but is 
      not supported by the HA.  
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_HA_CANT_REV_TUN:
      session->mip_fail_reason= MIP_FAIL_REASON_HA_FAILURE;
      break;

    /*-----------------------------------------------------------------------
      Registration failed because reverse tunneling was not requested but
      is required by the HA.  
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_HA_MUST_REV_TUN:
      session->mip_fail_reason= MIP_FAIL_REASON_HA_FAILURE;
      break;

    /*-----------------------------------------------------------------------
      Should never get here unless MIM attack - HA RRP codes aren't 
      processed unless MN-HA authentication succeeds.  Since HA is using
      same password as provisioned in the mobile, yet the RRQ received by 
      the HA had a bad MN-HA authenticator, either the HA validation is 
      broken or the RRQ changed sometime between being sent by MN and 
      receipt at HA.  Be safe and try again.
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_HA_MOBILE_NODE_FAILED_AUTH:
      session->mip_fail_reason= MIP_FAIL_REASON_HA_FAILURE;
      goto _SILENTLY_DISCARD_RRP;

    /*-----------------------------------------------------------------------
      Registration administratively prohibited. If reg SM is in re-rrq state 
      retry with home address set to 0.0.0.0. This is to try & stay on MIP 
      instead of falling to SIP. Also Re-RRQ timer is set to default value 
      since we got the response for RRQ. 
      If it is not in re-rrq state then follow the default behaviour
    -----------------------------------------------------------------------*/
    case MIP_RRP_CODE_FAILURE_HA_ADMIN_PROHIBITED:
      
      DATA_MIP_MSG1(MSG_LEGACY_MED, 
                  "Registration administratively prohibited, Reg state = %d",
                  session->rsm_info.state);
      if (RSMI_RE_RRQ_STATE == session->rsm_info.state)
      {
        session->mip_fail_reason = MIP_FAIL_REASON_HA_ADMIN_PROHIBIT;
      }

      else
      {
        session->mip_fail_reason = MIP_FAIL_REASON_HA_FAILURE;
      }

      retrieve_ip_addrs = FALSE;
      break;

    /*-----------------------------------------------------------------------
      FUTURE - need to identify failure code generated for RRQ with 
               HA address = home subnet local broadcast address.
    
    case MIP_RRP_CODE_DHA_WITH_HA_SUBNET_BROADCAST:
      retrieve_ip_addrs = TRUE;
      retry = TRUE;
      break;
    -----------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
      Registration failed for another reason.

      Don't take home or HA address from this RRP - if need to do this for
      some RRP code not listed above, add an entry above rather than here.
    -----------------------------------------------------------------------*/
    default:
      retrieve_ip_addrs = FALSE;
      break;
  } /* switch on HA RRP codes */

  /*-------------------------------------------------------------------------
    Notify the registration state machine of failure 
  -------------------------------------------------------------------------*/

_DONE_RRP_PROCESSING:

  if (retrieve_ip_addrs)
  {
    /*-----------------------------------------------------------------------
      Pick up HA address from RRP

      If the PDSN returns 0.0.0.0 for some reason, don't pick it up so our
      assumption that HA=0.0.0.0 only for IS-835-A mobiles remains valid.
    -----------------------------------------------------------------------*/
    if (rrp->ha_addr)
    {
      session->ha_addrs[session->curr_ha_index] = rrp->ha_addr;
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Picking HA Address from RRP (%x)", rrp->ha_addr); 
    }
  
    /*-----------------------------------------------------------------------
      Pick up Home IP address too if HA is known.
    -----------------------------------------------------------------------*/
    if (session->ha_addrs[session->curr_ha_index] != DYNAMIC_HA_ADDR)
    {
      session->home_addr = rrp->home_addr;
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Picking Home IP Addr from RRP (%x)", rrp->home_addr); 
    }

  } /* if HA and/or Home IP addresses have to be retrieved */

  DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Mobile Registration failed (code=%d)", rrp->code);

  /*-------------------------------------------------------------------------
    Generate the Mobile IP RRP event.
  -------------------------------------------------------------------------*/
  event_report_payload( EVENT_MIP,
                        sizeof (mip_event_payload),
                        &mip_event_payload );

  if (retry)
  {
    mip_reg_sm_post_event(session, RSM_RRQ_FAIL_W_RETRY_EV);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Common code for HA unavailable
    -----------------------------------------------------------------------*/
    mip_reg_sm_post_event(session, RSM_RRQ_FAIL_HA_UNAVAIL_EV);
    if (MIP_FAIL_REASON_HA_ADMIN_PROHIBIT != session->mip_fail_reason)
    {
      session->mip_fail_reason = MIP_FAIL_REASON_HA_FAILURE;
    }
  }

  DATA_MIP_MSG0(MSG_LEGACY_MED, "RRP processed successfully");
  return;

_SILENTLY_DISCARD_RRP:
  /*-------------------------------------------------------------------------
    An error condition caused the RRP to be silently discarded,
    set the dropped bit in the RRP event info and send the event.
  -------------------------------------------------------------------------*/
  mip_event_payload.info.i_rrp.flags.dropped = TRUE;
  event_report_payload( EVENT_MIP,
                        sizeof (mip_event_payload),
                        &mip_event_payload );

} /* mip_parsei_rrp */



/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION MIP_PARSE_MSG

DESCRIPTION
  Processes a received message passed from the mipio layer

PARAMETERS
  prot - rx'd message protocol (PS_IPPROTO_ICMP or PS_IPPROTO_UDP)
  data - pointer to rx'd message
  len  - length of rx'd message

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mip_parse_msg
(
  uint8   index,                          /* connection on which msg rx'd  */
  void *  p,                              /* data received                 */
  uint32  from_ip,                        /* sender's IP address           */
  uint16  len                             /* data length                   */ 
)
{
  byte *  data = p;
  mip_ma_info_type *ma_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate the arguments

    magic # = icmp x 2 + 1 udp / session 
            = 2 + MIP_MAX_SESSIONS
  -------------------------------------------------------------------------*/
  ASSERT (index < (MIP_MAX_SESSIONS + 2));
  if (data == NULL)
  {
    DATA_ERR_FATAL("Null data received for processing from MIPIO layer.");
    return;
  }

  /*-------------------------------------------------------------------------
    Get ma_info pointer.
  -------------------------------------------------------------------------*/
  ma_info_ptr = mip_get_ma_info_ptr();

  /*-------------------------------------------------------------------------
    Check to make sure MIP is active, otherwise ignore the message since we 
    don't care right now.  We may receive router advertisements while in
    SIP mode.
  -------------------------------------------------------------------------*/
  if (mip_meta_sm_open() == FALSE)
  {
    DATA_MIP_MSG0(MSG_LEGACY_MED, "Got a MIP message but MIP client is closed - ignore");
    return;
  }

  DATA_MIP_MSG2(MSG_LEGACY_LOW, "** Session %d received message type %d", index, data[0]); 

  /*-------------------------------------------------------------------------
    Handle messages coming from ICMP
    -> Agent Advertisement (extension on ICMP Router Advertisement)
  -------------------------------------------------------------------------*/
  if (index >= MIP_MAX_SESSIONS)
  {
    /*-----------------------------------------------------------------------
      The first byte is the message type - ignore all except 
      router advertisment
    -----------------------------------------------------------------------*/
    if (data[0] == ICMP_MSGTYPE_ROUTER_ADVERTISEMENT)
    {
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "** Parsing AAM (ICMP)"); 
      mip_parsei_router_adv( (mip_router_adv_msg *) data, len ); 
    }
    else
    {
      DATA_MIP_MSG1(MSG_LEGACY_MED, "** Unrecognized ICMP message (type %d)", data[0]);
    }
  }
  else
  /*-------------------------------------------------------------------------
    Handle messages coming from UDP
    -> Registration Reply on port 434
  -------------------------------------------------------------------------*/
  {
    /*-----------------------------------------------------------------------
      The first byte is the message type - ignore all except 
      registration reply
    -----------------------------------------------------------------------*/
    if (data[0] == MIP_MSGTYPE_RRP)
    {
      /*---------------------------------------------------------------------
        Make sure this message came from the current FA.  If not, discard. 
      ---------------------------------------------------------------------*/
      if (from_ip == ma_info_ptr->ma_addr)
      {
        DATA_MIP_MSG0(MSG_LEGACY_HIGH, "** Parsing RRP (UDP)"); 
        mip_parsei_rrp (INDEX_2_MIP_SES_PTR(index),(mip_rrp_msg *)data,len);
      }
      else
      {
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "RRP spoofed! src IP (%x) not FA", from_ip); 
      }
    }
    else
    {
      DATA_MIP_MSG1(MSG_LEGACY_MED, "** Unrecognized UDP message (type %d)", data[0]);
    }
  }

} /* mip_parse_msg */

#endif  /* FEATURE_DS_MOBILE_IP */
