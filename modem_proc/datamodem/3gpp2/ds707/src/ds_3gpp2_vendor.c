
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D S _ 3 G P P 2 _ V E N D . C 

DESCRIPTION
  This file contains the functions used to implement the data services
  3GPP2 vendor-specific feature - Version/Capability Indication as 
  defined in IS-835-D.

  The Organizationally-Unique Identifier (OUI) for this is 0xCF002.

Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds_3gpp2_vendor.c#1 $
  $Author: mplcsds1 $  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
06/26/13    sc     Replace all the memcpy and memmove with memscpy and 
                   memsmove as its safer.
08/13/12    ss     Moving efs write operation to dsutil performed during 
                   bootup.
04/12/12    jee    To propagate correct call end reason from MH
02/22/12    ash    Enforce upper bound check for Always on NV config
02/17/12    ssh    Added ds_3gpp2_mpit_timer_stop()
02/02/12    vpk    Avoid double free of received packet.
11/02/11    ssh    Drop mismatched VCRs
11/01/11    ssh    EFS item needs to be PACKED
10/17/11    ssh    eHRPD Always On support
08/08/11    ash    Reset MPIT timeout upon LCP resync
07/20/11    ash    Default behaviour of MPIT is enabled by spec.
07/11/11    ash    Include ID for MPIT packet in ds_3gpp2_send_vendor_packet.
06/06/11    ash    Added support for PPP Partial Context
10/13/10    op     Migrated to MSG 2.0 macros
07/04/09    pp     hton/ntoh* macros replaced with ps_hton/ntoh*.
04/29/09    vs     Updated, Tested code after discussions with Sid and Shshank
07/13/07    ssr    Incorporated Code review comments from Shrawan
12/28/06    ssh    Created module
===========================================================================*/


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#if ( defined(FEATURE_DATA_3GPP2_ALWAYS_ON) || defined(FEATURE_DATA_3GPP2_VS) )
#include "ps_ppp.h"
#include "ps_ppp_vend.h"
#include "ds_3gpp2_vendor.h"
#include "nv.h"
#include "nv_items.h"
#include "dstaski.h"
#include "ds707_gonulltmr.h"
#include "data_msg.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr.h"
#include "dsutil.h"
#include "ds3gtimer.h"
#include "data_err_suppr.h"
#include "ds3gcfgmgr.h"
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
#include "ps_ppp_defs.h"
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Define for the 3GPP2 OUI field
---------------------------------------------------------------------------*/
#define VENDOR_SPECIFIC_3GPP2_OUI      0xCF0002   /* IS-835-D Ch.2. Sec. 8  */

/*---------------------------------------------------------------------------
  Vendor specific reject packet
---------------------------------------------------------------------------*/
#define VENDOR_SPECIFIC_REJECT_KIND  0

/*---------------------------------------------------------------------------
  Max PPP Inactivity Timer Packet, IS-835-D, Ch.2 Sec. 3
---------------------------------------------------------------------------*/
#define MAX_PPP_INACTIVITY_TIMER_KIND  1

/*---------------------------------------------------------------------------
 The value of MPIT reply packet is TBD in the standard
---------------------------------------------------------------------------*/
#define MAX_PPP_INACTIVITY_TIMER_REPLY_KIND 8

/*---------------------------------------------------------------------------
  VENDOR PACKET header size which include following fields.
   0              7               15                              31
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |     Code      |  Identifier   |            Length             |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                         Magic-Number                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      OUI                      |     Kind      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
---------------------------------------------------------------------------*/
#define VENDOR_HEADER_SIZE            12

/*---------------------------------------------------------------------------
  Vendor extension field for MPIT comes after generic packet header.
   0              7               15                              31
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      OUI                      |    *Kind*     |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |              *Max PPP Inactivity Timer Value*                 |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 
---------------------------------------------------------------------------*/
#define OFFSET_KIND                    3 /* Offset for KIND field */
#define OFFSET_MPIT                    4 /* Offset for MPIT field */

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
/*---------------------------------------------------------------------------
 Version Capability Indication/Reply (VCI/VCR) Packet, IS-835-D Ch.2 Sec. 8              
---------------------------------------------------------------------------*/
#define VERSION_CAPABILITY_IND_KIND    2      /* kind field for VCI pkt    */
#define VERSION_CAPABILITY_REPLY_KIND  3      /* kind field for VCR pkt    */

#define VS_VERSION_D                   0      /* Version D                 */
#define VS_VERSION_E                   1      /* Version D                 */

/*---------------------------------------------------------------------------
  PDSN fields comes after generic packet header.
   0              7               15                              31
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                      OUI                      |     Kind      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   | *PDSN Version* |             *PDSN Capabilities*              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 
---------------------------------------------------------------------------*/
#define OFFSET_PDSN_VERSION            4 /* Offset for PDSN VERSION */
#define OFFSET_PDSN_CAPABILITIES       5 /* Offset for PDSN CAPABILITIES */

/*---------------------------------------------------------------------------
  Field size in packet.
---------------------------------------------------------------------------*/
#define PDSN_CAPABILITIES_SIZE         3 /* Size of PDSN CAPABILITIED in 
                                            the PACKET*/

/*------------------------------------------------------------------------- 
  #defines for Always On default configuration
--------------------------------------------------------------------------*/
#define ALWAYS_ON_DEFAULT_VCI_TRIES             3              /* count   */
#define ALWAYS_ON_DEFAULT_VCI_TIMEOUT           3              /* seconds */
#define ALWAYS_ON_DEFAULT_MPIT_PERCENT_TRIGGER  50             /* percent */
#define ALWAYS_ON_DEFAULT_ECHO_TRIES            3              /* count   */
#define ALWAYS_ON_DEFAULT_ECHO_TIMEOUT          3              /* seconds */

/*------------------------------------------------------------------------- 
  #defines for Always On configuration upper bound
--------------------------------------------------------------------------*/
#define ALWAYS_ON_MAX_MPIT_PERCENT_TRIGGER      100            /* percent */

/*---------------------------------------------------------------------------
  MS Capabilities, 24-bits coded as a bitmask, C0 is MSB
  (See IS-835-D Ch.2 Sec. 8)
  MSB                                                               LSB
  +----+----+----+----+----+----+----+---- ... ------------ ... ------+
  | C0 | C1 | C2 | C3 | C4 | <----- C5 through C23 (reserved) ----->  |
  +----+----+----+----+----+----+----+---- ... ------------ ... ------+
---------------------------------------------------------------------------*/
typedef enum ms_capability_enum_type
{
  MS_SUPPORTS_SIPV4 = 1 << 23,    /* C0, indicates support for S.IPv4      */
  MS_SUPPORTS_MIPV4 = 1 << 22,    /* C1, indicates support for M.IPv4      */
  MS_SUPPORTS_SIPV6 = 1 << 21,    /* C2, indicates support for S.IPv6      */
  MS_SUPPORTS_MIPV6 = 1 << 20,    /* C3, indicates support for M.IPv6      */
  MS_SUPPORTS_MPIT  = 1 << 19     /* C4, indicates support for 3GPP2 Max   */ 
                                  /*     PPP Inactivity Timer Packet       */
                                  /*     Bits C5 through C23 are reserved  */
} ms_capability_enum_type;

/*---------------------------------------------------------------------------
  PDSN Capabilities, 24-bits coded as a bitmask, C0 is MSB
  (See IS-835-D Ch.2 Sec. 8)
  MSB                                                                    LSB
  +----+----+----+----+----+----+----+---- ... ------------ ... -----------+
  | C0 | C1 | C2 | C3 | C4 | C5 | <----- C6 through C23 (reserved) ----->  |
  +----+----+----+----+----+----+----+---- ... ------------ ... -----------+
---------------------------------------------------------------------------*/
typedef enum pdsn_capability_enum_type
{
  PDSN_SUPPORTS_S060        = 1 << 23,  /* C0, indicates support for S0 60 */
  PDSN_SUPPORTS_S061        = 1 << 22,  /* C1, indicates support for S0 61 */
  PDSN_SUPPORTS_S066        = 1 << 21,  /* C2, indicates support for S0 66 */
  PDSN_SUPPORTS_HA_RES_MGMT = 1 << 20,  /* C3, indicates support for HA    */
                                        /*     resoource mgmt via MIP4     */
                                        /*     revocation                  */
  PDSN_SUPPORTS_S064        = 1 << 19,  /* C4, support for S0 64           */
  PDSN_SUPPORTS_S067        = 1 << 18   /* C5, support for S0 67           */
                                        /* Bits C6 thru C23 are reserved   */
} pdsn_capability_enum_type;

typedef uint32 ms_capabilities;
typedef uint32 pdsn_capabilities;

/*---------------------------------------------------------------------------
  PPP Link Status Determination EFS config (part of Always On EFS config)
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  boolean  is_enabled;                     /* UE-inited PPP LSD enabled ?  */
  uint8    mpit_percent_trigger;           /* when to trigger PPP LSD      */
  uint8    echo_req_tries;                 /* Max Echo-Req attempts        */
  uint32   echo_req_timeout;               /* Echo-Req timeout             */
} ds_3gpp2_ppp_lsd_config_type;

/*--------------------------------------------------------------------------- 
  eHRPD Always On EFS Configuration
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  boolean  is_enabled;                     /* Is always on enabled         */
  uint8    vci_tries;                      /* Max VCI attempts             */
  uint32   vci_timeout;                    /* VCI timeout                  */
  ds_3gpp2_ppp_lsd_config_type lsd;        /* Link Status Determ Config    */
} ds_3gpp2_always_on_config_type;

/*---------------------------------------------------------------------------
  DS 3GPP2 Always On control block
---------------------------------------------------------------------------*/
typedef struct ds_3gpp2_always_on_cb_s
{ 
  ds_3gpp2_always_on_config_type config;    /* EFS config                  */
  ms_capabilities   ms_capability_mask;     /* MS capability bitmask       */
  pdsn_capabilities pdsn_capability_mask;   /* PDSN capability bitmask     */
  boolean          was_reject_rxed;         /* Should stop sending VCI ?   */
  boolean          is_vci_done;
  uint8            next_vci_id;             /* id for the next VCI pkt     */
  uint8            vci_tries;               /* VCI retries                 */
  rex_timer_type   vci_timer;               /* VCI timer handle            */
  rex_timer_type   lsd_timer;               /* LSD timer handle            */
  dword            lsd_timeout;             /* LSD timeout                 */
} ds_3gpp2_always_on_cb_type;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

/*---------------------------------------------------------------------------
  DS 3GPP2 Max PPP Inactivity Timer control block
---------------------------------------------------------------------------*/
typedef struct ds_3gpp2_mpit_cb_s
{
  boolean         is_enabled;           /* Flag for MPIT timer feature */
  rex_timer_type  mpit_timer;           /* pointer to timer */
  dword           timeout;              /* Max PPP inactivity timeout value */
} ds_3gpp2_mpit_cb_type;

/*---------------------------------------------------------------------------
  DS 3GPP2 vendor-specific information control block
---------------------------------------------------------------------------*/
typedef struct ds_3gpp2_vendor_cb_s
{  
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
  ds_3gpp2_always_on_cb_type always_on_cb;     /* Always On control block  */
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
  ds_3gpp2_mpit_cb_type      mpit_cb;          /* MPIT control block       */
  void                     * ppp_event_handle; /* Handle to PPP events     */
} ds_3gpp2_vendor_cb_type;

/*------------------------------------------------------------------------- 
  Global variable to store state information for ds_3gpp2_vendor module 
  - Always on information
  - MPIT information
--------------------------------------------------------------------------*/
static ds_3gpp2_vendor_cb_type ds_3gpp2_vendor_cb;

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
/*------------------------------------------------------------------------- 
  Always on default configuration
--------------------------------------------------------------------------*/
static ds_3gpp2_always_on_config_type always_on_default_config = 
{
  FALSE, /* is always on enabled ? */
  ALWAYS_ON_DEFAULT_VCI_TRIES, /* vci tries */
  ALWAYS_ON_DEFAULT_VCI_TIMEOUT, /* vci timeout */
  {
    FALSE, /* is UE initiated link status determination enabled ? */
    ALWAYS_ON_DEFAULT_MPIT_PERCENT_TRIGGER, /* mpit percent trigger */
    ALWAYS_ON_DEFAULT_ECHO_TRIES, /* LSD echo tries */
    ALWAYS_ON_DEFAULT_ECHO_TIMEOUT /* LSD echo timeout */
  }
};

/*------------------------------------------------------------------------- 
  Handy pointer to Always On control block
--------------------------------------------------------------------------*/
static ds_3gpp2_always_on_cb_type * always_on_cb_ptr = 
  &ds_3gpp2_vendor_cb.always_on_cb;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             INTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DS_3GPP2_SEND_VENDOR_PACKET()

DESCRIPTION
  This function frames and sends a VCI packet to PPP vendor extension module
  
PARAMETERS
  device         : PPP device
  protocol       : PPP protocol to use to send this packet.
  kind           : Specified Kind field in Vendor specific packet.
  *pkt_info_ptr  : Incoming packet information.
  **received_pkt_ptr_ptr  : Original packet content.
   

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  *received_pkt_ptr_ptr is updated to NULL if the packet is appended.
===========================================================================*/
void ds_3gpp2_send_vendor_packet
(  
  ppp_dev_enum_type     device,
  uint16                protocol,
  uint32                kind,
  ppp_vendor_input_pkt_info_type *pkt_info_ptr,
  dsm_item_type                  **received_pkt_ptr_ptr
)
{
  dsm_item_type *pkt_ptr = NULL; /* pointer to Vendor specific (VS) packet to be sent */
  uint8          id = 0;         /* Identifier for the VS packet */
  uint32         temp;           /* temporary variable to hold 4 bytes values to
                                  * write to pkt */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
  /*---------------------------------------------------------------------------
    Filling in version and list of capabilities if it is a VCI packet.   
  ---------------------------------------------------------------------------*/
  if (kind == VERSION_CAPABILITY_IND_KIND)            
  {
    /*-------------------------------------------------------------------------
      Set MS capability bitmask - C4 bit if Always On is enabled
    -------------------------------------------------------------------------*/
    if( always_on_cb_ptr->config.is_enabled == TRUE )
    {
      always_on_cb_ptr->ms_capability_mask = MS_SUPPORTS_MPIT; /* C4 bit */
    }

    /*------------------------------------------------------------------------
      Allocate a new Id for the next VCI packet.
    ------------------------------------------------------------------------*/
    if(always_on_cb_ptr->next_vci_id == 0xFF)
    {
      always_on_cb_ptr->next_vci_id = 0; 
    }
    id = ++always_on_cb_ptr->next_vci_id;
    temp = always_on_cb_ptr->ms_capability_mask & 0xffffff;
    temp |= ((uint32)VS_VERSION_D << 24);
    temp = ps_htonl(temp);
    if ( dsm_pushdown_packed(&pkt_ptr, 
                             &temp, 
                             sizeof(temp),
                             DSM_DS_SMALL_ITEM_POOL) != sizeof(temp) )
    {    
      DATA_IS707_MSG2(MSG_LEGACY_ERROR, "dsm_pushdown_packed error while filling in "
                      "version and capabilities. oui:%d and kind:%d",
                      VENDOR_SPECIFIC_3GPP2_OUI,
                      kind);
      dsm_free_packet(&pkt_ptr);
      return;
    }
  } /* sending VCI packet */
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

  /*------------------------------------------------------------------------
    Filling in OUI and kind fields.
  ------------------------------------------------------------------------*/
  temp = (uint32)kind & 0xff;
  temp |= ((uint32)VENDOR_SPECIFIC_3GPP2_OUI << 8);
  temp = ps_htonl(temp);
  if ( dsm_pushdown_packed(&pkt_ptr, 
                           &temp, 
                           sizeof(temp),
                           DSM_DS_SMALL_ITEM_POOL) != sizeof(temp))
  {    
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, "dsm_pushdown_packed error while filling in "
                    "oui and kind. oui:%d and kind:%d",
                    VENDOR_SPECIFIC_3GPP2_OUI,
                    kind);
    dsm_free_packet(&pkt_ptr);
    return;
  }

  switch (kind) 
  {
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
    case VERSION_CAPABILITY_IND_KIND:
      /* nothing to do */
      break;

    case VERSION_CAPABILITY_REPLY_KIND:
      if (NULL == pkt_info_ptr)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL pkt_info_ptr passed for VC REPLY KIND packet");
        return;
      }
  
      /*------------------------------------------------------------------------
        ID must match the id of VCI received from PDSN.
      ------------------------------------------------------------------------*/
      id = pkt_info_ptr->id;  
      break;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

    case MAX_PPP_INACTIVITY_TIMER_REPLY_KIND:
      if (NULL == pkt_info_ptr)
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL pkt_info_ptr passed for MPIT packet");
        return;
      }

      /*------------------------------------------------------------------------
        ID must match the id of MPIT received from PDSN.
      ------------------------------------------------------------------------*/
      id = pkt_info_ptr->id;  
      break;

    case VENDOR_SPECIFIC_REJECT_KIND:
      if (NULL == pkt_info_ptr || NULL == received_pkt_ptr_ptr || 
        (NULL != received_pkt_ptr_ptr && *received_pkt_ptr_ptr == NULL) )
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL pkt_info_ptr or received_pkt_ptr_ptr"
                                          " passed for REJECT KIND packet");
        return;
      }
      /*------------------------------------------------------------------------
        ID must match the id of VCI received from PDSN.
      ------------------------------------------------------------------------*/
      id = pkt_info_ptr->id;

      /*------------------------------------------------------------------------
        Copy the rest of the packet from the OUI field.
        dsm_append updates the *received_pkt_ptr_ptr to NULL 
        avoids double free.
      ------------------------------------------------------------------------*/
      dsm_append (&pkt_ptr, received_pkt_ptr_ptr);
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "TX: Invalid Kind! = %d", kind);
      break;
  } /* kind */

  DATA_IS707_MSG2(MSG_LEGACY_MED, "TX: kind = %d, Id = %d", kind, id);

  /*--------------------------------------------------------------------------
    Send the VCI packet.
  --------------------------------------------------------------------------*/
  if(-1 == ppp_vendor_send (device, protocol, id, pkt_ptr))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "ppp_vendor_send failed");
    return;
  }

  return;
} /* ds_3gpp2_send_vendor_packet() */

/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_INPUT_HDLR()

DESCRIPTION
  This function handles incoming packets for the 3GPP2 vendor-specific OUI.
  
PARAMETERS 
  device        : (in) PPP device.
  *pkt_ptr      : (in) Pointer to incoming packet.
  *pkt_info_ptr : (in) Information about the incoming packet.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp2_vendor_input_hdlr
(
  ppp_dev_enum_type               device, 
  dsm_item_type                  *pkt_ptr,
  ppp_vendor_input_pkt_info_type *pkt_info_ptr      
)
{
  uint16  protocol;
  uint32  oui;
  uint32  max_ppp_inactvity_timeout;
  uint8   kind, orig_kind;
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
  uint8   pdsn_version;
  pdsn_capabilities pdsn_capabilities_val;
  double timeout = 0;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == pkt_ptr || NULL == pkt_info_ptr )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Null arguments to "
                                       "ds_3gpp2_vendor_input_hdlr");
    dsm_free_packet(&pkt_ptr);
    return;
  }

  protocol = pkt_info_ptr->protocol;
  oui      = pkt_info_ptr->oui;
  if( oui != VENDOR_SPECIFIC_3GPP2_OUI )
  {
    DATA_IS707_MSG3( MSG_LEGACY_ERROR, "Unrecognized OUI (0x%x) received, "
                                       "dev %d, proto 0x%x", 
                                       device, protocol, oui);
    dsm_free_packet(&pkt_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Process incoming vendor-specific packet.
  -------------------------------------------------------------------------*/
  switch ( protocol )
  {
    case PPP_LCP_PROTOCOL:   
      /*----------------------------------------------------------------------
        Extract the kind field.
      ----------------------------------------------------------------------*/
      if (dsm_extract(pkt_ptr, OFFSET_KIND, &kind, sizeof(kind)) != sizeof(kind))
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                        "Error while extracting the kind field for OUI:%d",
                        oui);
        break;
      }

      DATA_IS707_MSG2(MSG_LEGACY_MED, "RX: kind = %d, Id = %d",
                      kind, pkt_info_ptr->id); 
      switch( kind )
      {
        case MAX_PPP_INACTIVITY_TIMER_KIND:   
          if ( ppp_get_phase(device) & ( pppAUTHENTICATE | pppNETWORK ) )
          {
            /*-----------------------------------------------------------------
              Extract Max PPP inactivity timout value.
            ------------------------------------------------------------------*/
            if ( dsm_extract( pkt_ptr, 
                              OFFSET_MPIT, 
                              &max_ppp_inactvity_timeout, \
                              sizeof(max_ppp_inactvity_timeout)) != \
                                                sizeof(max_ppp_inactvity_timeout) )
             {
               DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                               "Error while extracting max_ppp_inactvity_timeout "
                               "value for OUI:%d", oui);            
               break;
             }
  
             /*-----------------------------------------------------------------
               Change the value to host representation.
             ------------------------------------------------------------------*/
             max_ppp_inactvity_timeout = ps_ntohl(max_ppp_inactvity_timeout);
  
             /*-----------------------------------------------------------------
               Store Max PPP inactivity timeout.
             ------------------------------------------------------------------*/
             ds_3gpp2_vendor_cb.mpit_cb.timeout = max_ppp_inactvity_timeout;
             DATA_IS707_MSG1(MSG_LEGACY_MED, "RX: MPIT: Timeout = %d",
                             ds_3gpp2_vendor_cb.mpit_cb.timeout);
             /*-----------------------------------------------------------------
               Send MPIT reply packet.
             ------------------------------------------------------------------*/
             ds_3gpp2_send_vendor_packet( device, 
                                          (uint16) protocol, 
                                          MAX_PPP_INACTIVITY_TIMER_REPLY_KIND, 
                                          pkt_info_ptr, 
                                          &pkt_ptr);
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
             /*-----------------------------------------------------------------
               Store LSD timeout as the configured fraction of this value
               We will use this to trigger link status determination only if
               VCI/VCR exchange is successful (see ds_3gpp2_lsd_timeout_cb)
             ------------------------------------------------------------------*/
             if( ( always_on_cb_ptr->config.is_enabled ) &&
                 ( always_on_cb_ptr->config.lsd.is_enabled ) )
             {
               timeout = 
                 ((double)always_on_cb_ptr->config.lsd.mpit_percent_trigger/100) *
                 ds_3gpp2_vendor_cb.mpit_cb.timeout;
               always_on_cb_ptr->lsd_timeout = (dword)timeout;
               DATA_IS707_MSG2(MSG_LEGACY_MED,
                               "Rx: Using LSD timeout, double = %f, uint32 = %d",
                               timeout, 
                               always_on_cb_ptr->lsd_timeout);   
             }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
          }
          break;

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
        case VERSION_CAPABILITY_IND_KIND:     
           /*-----------------------------------------------------------------
             Extract version and make sure it is a Rev. D PDSN. 
             Otherwise do not process the packet
           ------------------------------------------------------------------*/
          if ( dsm_extract(pkt_ptr, 
                           OFFSET_PDSN_VERSION, 
                           &pdsn_version, \
                           sizeof(pdsn_version)) != \
                                           sizeof(pdsn_version) )
          {
            DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                            "Error while extracting pdsn_version for OUI:%d",
                            oui);
            break;
          }

          /*-----------------------------------------------------------------
            Allow all PDSN versions
          ------------------------------------------------------------------*/
          if( ( pdsn_version != VS_VERSION_D ) && 
              ( pdsn_version != VS_VERSION_E) )
          {
            DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unexpected PDSN version %d",
                            pdsn_version);
            break;
          }

          if ( dsm_extract(pkt_ptr, 
                           OFFSET_PDSN_CAPABILITIES, 
                           &pdsn_capabilities_val,
                           PDSN_CAPABILITIES_SIZE) != PDSN_CAPABILITIES_SIZE )
          {
            DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                            "Error while extracting "
                            "pdsn_capabilities for OUI:%d",
                            oui);            
            break;
          }

          pdsn_capabilities_val = ps_ntohl(pdsn_capabilities_val);
          pdsn_capabilities_val &= 0xffffff;          
          always_on_cb_ptr->pdsn_capability_mask = pdsn_capabilities_val;

          /*-----------------------------------------------------------------
            Send VCI reply.
          ------------------------------------------------------------------*/
          ds_3gpp2_send_vendor_packet( device, 
                                       (uint16) protocol, \
                                       VERSION_CAPABILITY_REPLY_KIND, 
                                       pkt_info_ptr, 
                                       &pkt_ptr );          
          break;

        case VERSION_CAPABILITY_REPLY_KIND:   
          /*-----------------------------------------------------------------
            Verify if Id matches that of the last VCI packet sent out.
          ------------------------------------------------------------------*/
          if (pkt_info_ptr->id != always_on_cb_ptr->next_vci_id) 
          {
            DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                            "Rx: VCR Expected id = %d, Rxed Id = %d. Dropping!",
                            always_on_cb_ptr->next_vci_id,
                            pkt_info_ptr->id);
            break;
          }

          /*-----------------------------------------------------------------
            Clear VCI timer and update control block variables 
          ------------------------------------------------------------------*/
          rex_clr_timer(&always_on_cb_ptr->vci_timer);
          always_on_cb_ptr->vci_tries = always_on_cb_ptr->config.vci_tries;
          always_on_cb_ptr->was_reject_rxed = FALSE;
          always_on_cb_ptr->is_vci_done = TRUE;
          break;
        
        case VENDOR_SPECIFIC_REJECT_KIND:
          /*-----------------------------------------------------------------
            Identify the kind field of the original packet for which this
            reject has been sent
          ------------------------------------------------------------------*/
          if (dsm_extract( pkt_ptr, 
                           VENDOR_HEADER_SIZE + OFFSET_KIND, 
                           &orig_kind, 
                           sizeof(orig_kind) 
                         ) != sizeof(orig_kind))
          {
            DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                            "Error while extracting orig_kind for OUI:%d",
                            oui);
            break;
          }
          DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                          "RX: VS Reject!: Id = %d, Kind = %d",
                          pkt_info_ptr->id,
                          orig_kind);
          /*-----------------------------------------------------------------
            A reject packet can only be received in response to sending a
            VCI packet. Just ignore all other cases.
          ------------------------------------------------------------------*/
          if (orig_kind != VERSION_CAPABILITY_IND_KIND) 
          {
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                            "RX: VS Reject to non-VCI! Dropping!");
            break;
          }

          /*-----------------------------------------------------------------
            Clear VCI timer and update control block variables 
          ------------------------------------------------------------------*/
          rex_clr_timer(&always_on_cb_ptr->vci_timer);
          always_on_cb_ptr->vci_tries = always_on_cb_ptr->config.vci_tries;
          always_on_cb_ptr->was_reject_rxed = TRUE;
          always_on_cb_ptr->is_vci_done = FALSE;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
          break;          

        default:
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "RX: Unexpected VS packet!");
          /*-----------------------------------------------------------------
            Send VCI reject.
          ------------------------------------------------------------------*/
          ds_3gpp2_send_vendor_packet( device, 
                                       (uint16) protocol, \
                                       VENDOR_SPECIFIC_REJECT_KIND, 
                                       pkt_info_ptr, 
                                       &pkt_ptr);
          break;
      }
      break;

    default:
      DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                      "RX: Unexpected VS packet! Id = %d, protocol = 0x%x",
                      pkt_info_ptr->id,
                      protocol);
      break;
  }
  dsm_free_packet(&pkt_ptr);
  return;
} /* ds_3gpp2_vendor_input_hdlr() */

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMEOUT_CB

DESCRIPTION   MPIT timer callback function.

PARAMETERS    None 

DEPENDENCIES  None

RETURN VALUE 
None
SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_mpit_timeout_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    ASSERT(0);
    return;
  }

  /*--------------------------------------------------------------------------- 
    Post MPIT timer command to DS task.
  ---------------------------------------------------------------------------*/
  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_MPIT:
      cmd_ptr->hdr.cmd_id= DS_CMD_707_MPIT_TIMEOUT;
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad timer id on callback %d",
                      timer_id);
      ASSERT(0);
      return;
  }
  ds_put_cmd( cmd_ptr );
} /* ds_3gpp2_mpit_timeout_cb() */

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
/*===========================================================================
FUNCTION DS_3GPP2_VCI_TIMER_CB()

DESCRIPTION
  This function handles the VCI timer callback and sends out a VCI packet if 
  there are additional retries remaining.
  
PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp2_vci_timer_cb
(
  uint32 param /* NULL for this timer call back */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------------
    If configure reject has been received do not send VCI packet.
  --------------------------------------------------------------------------*/
  if( always_on_cb_ptr->was_reject_rxed ) 
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, "Code-reject rxed for VCI, aborting!");  
    return;
  }

  /*--------------------------------------------------------------------------
    If number of attempts and timeout are non-zero, send a VCI packet.
  --------------------------------------------------------------------------*/
  if ( ( always_on_cb_ptr->config.is_enabled ) && 
       ( always_on_cb_ptr->vci_tries > 0 ) && 
       ( always_on_cb_ptr->config.vci_timeout > 0 ) )
  {
    ds_3gpp2_send_vendor_packet( PPP_UM_SN_DEV, 
                                 (uint16)PPP_LCP_PROTOCOL, 
                                 VERSION_CAPABILITY_IND_KIND, 
                                 NULL, 
                                 NULL );
    rex_set_timer( &always_on_cb_ptr->vci_timer, 
                   SEC_TO_MSEC(always_on_cb_ptr->config.vci_timeout) );
    always_on_cb_ptr->vci_tries--;
  }
  else
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, 
                    "Not sending VCI pkt, vci_tries = %d, timeout = %d",
                    always_on_cb_ptr->vci_tries,
                    always_on_cb_ptr->config.vci_timeout);
  }
} /* ds_3gpp2_vci_timer_cb() */

/*===========================================================================
FUNCTION      DS_3GPP2_LSD_RESULT_CBACK

DESCRIPTION   Link Status Determination (LSD) timer callback function.
              Resets MPIT if result is success, no op if failure.

PARAMETERS    None 

DEPENDENCIES  None

RETURN VALUE 
None
SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_lsd_result_cback
(
  ppp_dev_enum_type device,
  boolean           lsd_succeeded,
  void            * user_data
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( PPP_UM_SN_DEV != device ) || 
      ( user_data !=  &always_on_cb_ptr->config.lsd ) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unexpected callback, ignoring!");
    return;
  }

  if( lsd_succeeded )
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Link Status Determination passed. Resetting MPIT" );
    ds_3gpp2_mpit_timer_restart();
  }
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                     "Link Status Determination failed!" );
  }
} /* ds_3gpp2_lsd_result_cback() */

/*===========================================================================
FUNCTION      DS_3GPP2_LSD_TIMEOUT_CB

DESCRIPTION   Link Status Determination (LSD) timer callback function.

PARAMETERS    None 

DEPENDENCIES  None

RETURN VALUE 
None
SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_lsd_timeout_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
  {
    ASSERT(0);
    return;
  }

  /*--------------------------------------------------------------------------- 
    Post LSD timer command to DS task.
  ---------------------------------------------------------------------------*/
  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_LSD:
      cmd_ptr->hdr.cmd_id= DS_CMD_707_LSD_TIMEOUT;
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad timer id on callback %d",
                      timer_id);
      ASSERT(0);
      return;
  }
  ds_put_cmd( cmd_ptr );
} /* ds_3gpp2_lsd_timeout_cb() */

#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_PPP_EVENT_HANDLER_CB()

DESCRIPTION
  This function handles the LCP UP and Resync events
  Always ON: On UP event, send VCI packet
  Always ON: On Resync event, reset VCI timer and retries, stop LSD timer
  MPIT: On Resync event, stop MPIT timer
  
PARAMETERS
  device          : PPP device
  protocol        : PPP protocol
  ppp_event       : Event type
  *user_data      : User data 
  session_handle  : PPP session handle
  fail_reason     : Fail reason for down events

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp2_vendor_ppp_event_handler_cb
(
  ppp_dev_enum_type      device,
  ppp_protocol_e_type    protocol,
  ppp_event_e_type       ppp_event,
  void                  *user_data,
  uint32                 session_handle,
  ppp_fail_reason_e_type fail_reason
)
{  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT (device == PPP_UM_SN_DEV);
  ASSERT (protocol == PPP_PROTOCOL_LCP);  

  DATA_IS707_MSG1(MSG_LEGACY_MED, "3GPP2 OUI rxed event 0x%x", ppp_event );
  switch(ppp_event)
  {
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
    case PPP_UP_EV:            
      /*----------------------------------------------------------------------
        Send the first VCI packet
      ----------------------------------------------------------------------*/
      if( always_on_cb_ptr->config.is_enabled )
      {
        ds_3gpp2_vci_timer_cb(0);
      }
      break;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
    
    case PPP_RESYNC_EV:  
      /*---------------------------------------------------------------------
        Stop Max PPP Inactivity timer.
      ---------------------------------------------------------------------*/
      ds3gi_stop_timer( DS3G_TIMER_MPIT );
      ds_3gpp2_vendor_cb.mpit_cb.timeout = 0;
      DATA_IS707_MSG0(MSG_LEGACY_MED, "PPP Resync! MPIT stopped");

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
      /*---------------------------------------------------------------------
        Stop LSD and VCI timers. Reset state.
      ---------------------------------------------------------------------*/                               
      if( always_on_cb_ptr->config.is_enabled )
      {
        rex_clr_timer ( &always_on_cb_ptr->vci_timer );
        always_on_cb_ptr->vci_tries = always_on_cb_ptr->config.vci_tries;
        always_on_cb_ptr->next_vci_id = 0;
        
        ds3gi_stop_timer( DS3G_TIMER_LSD );
        always_on_cb_ptr->lsd_timeout = 0;
        DATA_IS707_MSG0(MSG_LEGACY_MED, "PPP Resync! LSD stopped");
        
        always_on_cb_ptr->was_reject_rxed = FALSE;
        always_on_cb_ptr->is_vci_done = FALSE;
      }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
      break;

    default:
      ASSERT(FALSE);
      break;
  }  
}/* ds_3gpp2_vendor_ppp_event_handler_cb() */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_POWERUP_INIT()

DESCRIPTION
  This function is called during powerup. It reads te VCI parameters from 
  an NV item into a VCI Config structure and defines a VCI Timer.
  
PARAMETERS
  None

RETURN VALUE
  0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_3gpp2_vendor_powerup_init
(
  void
)
{
  ds_3gpp2_mpit_cb_type *ds_3gpp2_mpit_cb_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(&ds_3gpp2_vendor_cb, 0, sizeof(ds_3gpp2_vendor_cb));

  /*------------------------------------------------------------------------- 
    Register MPIT timer callback.
  -------------------------------------------------------------------------*/
  ds_3gpp2_mpit_cb_ptr = &ds_3gpp2_vendor_cb.mpit_cb;
  ds3gi_timer_register(&(ds_3gpp2_mpit_cb_ptr->mpit_timer),
                       ds_3gpp2_mpit_timeout_cb,
                       DS3G_TIMER_MPIT,
                       DS3G_TIMER_TYPE_REX_TIMER);

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
   /*------------------------------------------------------------------------
     Initialize the VCI timer.
   ------------------------------------------------------------------------*/
   rex_def_timer_ex(&always_on_cb_ptr->vci_timer,
                    ds_3gpp2_vci_timer_cb, 
                    (uint32)NULL );
      
   /*------------------------------------------------------------------------
     Register LSD timer callback.
   ------------------------------------------------------------------------*/
   ds3gi_timer_register(&always_on_cb_ptr->lsd_timer,
                        ds_3gpp2_lsd_timeout_cb,
                        DS3G_TIMER_LSD,
                        DS3G_TIMER_TYPE_REX_TIMER);
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
  return 0;
} /* ds_3gpp2_vendor_powerup_init() */

/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_INIT()

DESCRIPTION
  This function initializes state variables and parameters for 3GPP2 OUI.
  It registers with PPP for the LCP Up event and also registers with 
  the PPP vendor-extensions module to receive all vendor-specific packets
  for the 3GPP2 OUI (0xCF002). 
  
PARAMETERS
  None

RETURN VALUE
  0: Success
  -1: Failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ds_3gpp2_vendor_init
(
  void
)
{
  nv_stat_enum_type      nv_status;
  ds_3gpp2_mpit_cb_type *ds_3gpp2_mpit_cb_ptr;
  char mpit_itemfp[] = "/nv/item_files/data/3gpp2/mpit_enabled";
  ppp_event_alloc_info_type  ppp_event_alloc_info;
  ppp_event_mask_e_type event_mask = PPP_NO_EV_MASK;
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
  char always_on_itemfp[] = 
         "/nv/item_files/data/3gpp2/always_on_config_info";
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Read MPIT EFS configuration
  -------------------------------------------------------------------------*/
  ds_3gpp2_mpit_cb_ptr = &ds_3gpp2_vendor_cb.mpit_cb;
  nv_status = ds3gcfgmgr_read_efs_nv_ex( mpit_itemfp,
                              (void *)&(ds_3gpp2_mpit_cb_ptr->is_enabled),
                              sizeof(ds_3gpp2_mpit_cb_ptr->is_enabled), ds707_curr_subs_id() );
  if(nv_status != NV_DONE_S)
  {
    /*-----------------------------------------------------------------------
      Enabled by default
    -----------------------------------------------------------------------*/
    ds_3gpp2_mpit_cb_ptr->is_enabled = TRUE;
  }

  DATA_IS707_MSG1(MSG_LEGACY_MED, "MPIT Enabled = %d", 
                  ds_3gpp2_mpit_cb_ptr->is_enabled);

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
  /*-------------------------------------------------------------------------
    Read Always On EFS configuration
  -------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_efs_nv_ex( always_on_itemfp,
                              (void *)&(always_on_cb_ptr->config),
                              sizeof(ds_3gpp2_always_on_config_type), ds707_curr_subs_id() );
  if(nv_status != NV_DONE_S)
  {
    /*-----------------------------------------------------------------------
      Set up defaults
    -----------------------------------------------------------------------*/
    memscpy( &always_on_cb_ptr->config,
             sizeof(ds_3gpp2_always_on_config_type),
             &always_on_default_config,
             sizeof(ds_3gpp2_always_on_config_type) );
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Using defaults for Always on. Disabled!");
  }
  else
  {
    if ( always_on_cb_ptr->config.lsd.mpit_percent_trigger >= 
                                          ALWAYS_ON_MAX_MPIT_PERCENT_TRIGGER )
    {
      DATA_IS707_MSG2(MSG_LEGACY_MED, 
                      "Invalid Configuration for MPIT percent Trigger (%d), "
                      "Using Default (%d)", 
                      always_on_cb_ptr->config.lsd.mpit_percent_trigger,
                      ALWAYS_ON_DEFAULT_MPIT_PERCENT_TRIGGER);
      always_on_cb_ptr->config.lsd.mpit_percent_trigger =
                                        ALWAYS_ON_DEFAULT_MPIT_PERCENT_TRIGGER;
    }

    DATA_IS707_MSG7(MSG_LEGACY_MED, "Always On NV Config: Enabled = %d "
              "VCI Tries = %d, VCI Timeout = %d, LSD: Enabled = %d "
              "LSD: MPIT Percent Trigger = %d, LSD: Echo Request Tries = %d "
              "LSD: Echo Request Timeout = %d",
              always_on_cb_ptr->config.is_enabled,
              always_on_cb_ptr->config.vci_tries,
              always_on_cb_ptr->config.vci_timeout,
              always_on_cb_ptr->config.lsd.is_enabled,
              always_on_cb_ptr->config.lsd.mpit_percent_trigger,
              always_on_cb_ptr->config.lsd.echo_req_tries,
              always_on_cb_ptr->config.lsd.echo_req_timeout);
  }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

  /*-------------------------------------------------------------------------
    Error if Always On is enabled but MPIT is not
  -------------------------------------------------------------------------*/  
  if( always_on_cb_ptr->config.is_enabled && 
      !ds_3gpp2_mpit_cb_ptr->is_enabled )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Always On enabled without MPIT!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Atleast MPIT must be enabled to proceed
  -------------------------------------------------------------------------*/  
  if( ds_3gpp2_mpit_cb_ptr->is_enabled )
  {
    /*-----------------------------------------------------------------------
      Register with the PPP Vendor Extensions module to receive packets 
      for the 3GPP2 vendor-specific OUI. 
     ----------------------------------------------------------------------*/
     if (ppp_vendor_register( PPP_UM_SN_DEV, 
                              VENDOR_SPECIFIC_3GPP2_OUI,
                              ds_3gpp2_vendor_input_hdlr ) != 0)
     {
       DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to register for 3GPP2 OUI");
       return -1;
     }
     
     /*----------------------------------------------------------------------
       Allocate PPP event handle.
     ----------------------------------------------------------------------*/
     memset( &ppp_event_alloc_info, 0, sizeof(ppp_event_alloc_info) );
     ppp_event_alloc_info.lcp_cb_f = ds_3gpp2_vendor_ppp_event_handler_cb;
     ppp_event_alloc_info.lcp_user_data = &ds_3gpp2_vendor_cb;
     ds_3gpp2_vendor_cb.ppp_event_handle =
       ppp_event_alloc_ex( PPP_UM_SN_DEV,
                           &ppp_event_alloc_info );
     if (ds_3gpp2_vendor_cb.ppp_event_handle == NULL)
     {
       /*--------------------------------------------------------------------
         Cannot reg for events. Deregister from vendor module and give up.
       --------------------------------------------------------------------*/
       DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Failed to register for PPP events!");
       if(-1 == ppp_vendor_deregister( PPP_UM_SN_DEV, 
                                       VENDOR_SPECIFIC_3GPP2_OUI ) )
       {
         DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Cleanup failed: Unable to "
             "deregister PPP vendor callback");
       }
       return -1;
     }

     event_mask = ds_3gpp2_mpit_cb_ptr->is_enabled ? 
                  PPP_RESYNC_EV_MASK : event_mask;
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
     event_mask |= always_on_cb_ptr->config.is_enabled ? 
                  PPP_UP_EV_MASK : event_mask;
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
     
     /*----------------------------------------------------------------------
       Register with the PPP for LCP events.
       Always ON: On UP event, send VCI packet
       Always ON: On Resync event, reset VCI timer and retries
       MPIT: On Resync event, reset MPIT timer
     ----------------------------------------------------------------------*/ 
     DATA_IS707_MSG1(MSG_LEGACY_MED, 
                     "DS 3GPP2 OUI: Registering for event_mask 0x%x", 
                     event_mask );
     if (-1 == ppp_event_register( ds_3gpp2_vendor_cb.ppp_event_handle,
                                   PPP_PROTOCOL_LCP,
                                   event_mask ) )
     {
       /*--------------------------------------------------------------------
         Cannot reg for events. Dealloc event handle, deregister from 
         vendor module and give up.
       --------------------------------------------------------------------*/
       if(-1 == ppp_event_dealloc( PPP_UM_SN_DEV,
                                   &(ds_3gpp2_vendor_cb.ppp_event_handle) ) )
       {
         DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to dealloc PPP event!");
       }
       if(-1 == ppp_vendor_deregister( PPP_UM_SN_DEV, 
                                       VENDOR_SPECIFIC_3GPP2_OUI ) )
       {
         DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Unable to deregister 3gpp2 OUI!");
       }
       return -1;
     } /* Register for PPP events failed */

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
     /*----------------------------------------------------------------------
       If always On is enabled, register timers and initialize VCI tries in 
       anticipation of LCP Up
     ----------------------------------------------------------------------*/
    if (always_on_cb_ptr->config.is_enabled)
    {  
      always_on_cb_ptr->vci_tries = always_on_cb_ptr->config.vci_tries;
    } /* always on enabled */
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
  } /* MPIT is enabled */
  return 0;
} /* ds_3gpp2_vendor_init() */

/*===========================================================================
FUNCTION DS_3GPP2_VENDOR_CLEANUP()

DESCRIPTION
  This function de-registers with the LCP starting event and resets the number 
  of retries and other values.
  
PARAMETERS
  None

RETURN VALUE
  None
 
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp2_vendor_cleanup
(
  void
)
{
  ds_3gpp2_mpit_cb_type  *ds_3gpp2_mpit_cb_ptr = &ds_3gpp2_vendor_cb.mpit_cb;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /*------------------------------------------------------------------------
     Atleast MPIT must be enabled 
   ------------------------------------------------------------------------*/
  if (!ds_3gpp2_mpit_cb_ptr->is_enabled )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "MPIT disabled. Nothing to cleanup" );
    return;
  }

  /*-----------------------------------------------------------------------
    Stop MPIT timer
  -----------------------------------------------------------------------*/
  if (ds_3gpp2_mpit_cb_ptr->is_enabled)
  {
    /*-----------------------------------------------------------------------
      Stop Max PPP Inactivity timer.
    -----------------------------------------------------------------------*/
    ds3gi_stop_timer(DS3G_TIMER_MPIT);
    ds_3gpp2_vendor_cb.mpit_cb.timeout = 0;
  }

  /*-----------------------------------------------------------------------
    Stop VCI and LSD timers and reset state
  -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
  if (always_on_cb_ptr->config.is_enabled)
  {
    /*-----------------------------------------------------------------------
      Stop the timers and reset Always On control block
    -----------------------------------------------------------------------*/
    rex_clr_timer(&always_on_cb_ptr->vci_timer);
    ds3gi_stop_timer(DS3G_TIMER_LSD);
    always_on_cb_ptr->ms_capability_mask = 0;
    always_on_cb_ptr->pdsn_capability_mask = 0;
    always_on_cb_ptr->was_reject_rxed = FALSE;
    always_on_cb_ptr->is_vci_done = FALSE;
    always_on_cb_ptr->next_vci_id = 0;
    always_on_cb_ptr->vci_tries = 0;
    always_on_cb_ptr->lsd_timeout = 0;
  }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */

  /*-------------------------------------------------------------------------
    De-register 3GPP2 OUI from PPP vendor specific module.
  -------------------------------------------------------------------------*/
  if (-1 == ppp_vendor_deregister(PPP_UM_SN_DEV, 
                                  VENDOR_SPECIFIC_3GPP2_OUI))
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Failed to deregister 3GPP2 OUI.");
  }

  /*------------------------------------------------------------------------- 
    De-alloc PPP event handle.
  -------------------------------------------------------------------------*/
  if (ppp_event_dealloc( PPP_UM_SN_DEV,
                         &(ds_3gpp2_vendor_cb.ppp_event_handle) ) < 0)
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Failed to de-allocate the handle for "
                    "lcp starting event");
  }  
} /* ds_3gpp2_vendor_cleanup() */

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_START

DESCRIPTION   This function Starts mpit timer, if and only if the timer is 
              not running. Calling this function while the timer is running 
              will not have any effect. If LSD is enabled,  this will start
              the LSD timer also.

PARAMETERS    None 

DEPENDENCIES  The timer should not be running for this function to take effect.

RETURN VALUE 
0 if started 
-1 if not started

SIDE EFFECTS  None
===========================================================================*/
int ds_3gpp2_mpit_timer_start(void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (TRUE  == ds_3gpp2_vendor_cb.mpit_cb.is_enabled) &&
       (0     != ds_3gpp2_vendor_cb.mpit_cb.timeout) &&
       (FALSE == ds3gi_is_timer_running( DS3G_TIMER_MPIT )) )
  {
    ds3gi_start_timer( DS3G_TIMER_MPIT,
                       SEC_TO_MSEC(ds_3gpp2_vendor_cb.mpit_cb.timeout) );
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
    if( ( always_on_cb_ptr->config.is_enabled ) &&
        ( always_on_cb_ptr->config.lsd.is_enabled ) &&
        ( 0 != always_on_cb_ptr->lsd_timeout ) )
    {
      ds3gi_start_timer( DS3G_TIMER_LSD,
                         SEC_TO_MSEC(always_on_cb_ptr->lsd_timeout) );
    }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
    return 0;
  }
  return -1;
} /* ds_3gpp2_mpit_timer_start */

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_RESTART

DESCRIPTION   This function restarts mpit timer, if and only if the timer is 
              running. Calling this function while the timer is not running 
              will not have any effect.  If LSD is enabled, this function 
              will restart the LSD timer also.

PARAMETERS    None 

DEPENDENCIES  The timer should be running for this function to take effect.

RETURN VALUE 
0 if restarted 
-1 if not restarted

SIDE EFFECTS  None
===========================================================================*/
int ds_3gpp2_mpit_timer_restart(void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( TRUE == ds3gi_is_timer_running( DS3G_TIMER_MPIT ) )
  {
    ds3gi_start_timer( DS3G_TIMER_MPIT,
                       SEC_TO_MSEC(ds_3gpp2_vendor_cb.mpit_cb.timeout) );
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
    if( ( always_on_cb_ptr->config.is_enabled ) &&
        ( always_on_cb_ptr->config.lsd.is_enabled ) &&
        ( 0 != always_on_cb_ptr->lsd_timeout ) )
    {
      ds3gi_start_timer( DS3G_TIMER_LSD,
                         SEC_TO_MSEC(always_on_cb_ptr->lsd_timeout) );
    }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
    return 0;
  }
  return -1;
} /* ds_3gpp2_mpit_timer_restart */

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_STOP

DESCRIPTION   This function stops the MPIT timer (and the associated LSD
              timer.)

PARAMETERS    None 

DEPENDENCIES  None

RETURN VALUE 
0 success
-1 failure

SIDE EFFECTS  None
===========================================================================*/
int ds_3gpp2_mpit_timer_stop(void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Stop MPIT timer if enabled and running
  -------------------------------------------------------------------------*/
  if ( (TRUE  == ds_3gpp2_vendor_cb.mpit_cb.is_enabled) &&
       (0     != ds_3gpp2_vendor_cb.mpit_cb.timeout) &&
       (TRUE == ds3gi_is_timer_running( DS3G_TIMER_MPIT )) )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Stopping MPIT timer...");
    ds3gi_stop_timer( DS3G_TIMER_MPIT );
#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON 
    /*----------------------------------------------------------------------- 
      Stop Link Status Determination (LSD) timer if enabled and running
    -----------------------------------------------------------------------*/
    if( ( always_on_cb_ptr->config.is_enabled ) &&
        ( always_on_cb_ptr->config.lsd.is_enabled ) &&
        ( 0 != always_on_cb_ptr->lsd_timeout ) &&
        (TRUE == ds3gi_is_timer_running( DS3G_TIMER_LSD )) )
    {
      DATA_IS707_MSG0(MSG_LEGACY_MED, "Stopping LSD timer...");
      ds3gi_stop_timer( DS3G_TIMER_LSD );
    }
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
  }
  return 0;
} /* ds_3gpp2_mpit_timer_stop */

/*===========================================================================
FUNCTION      DS_3GPP2_MPIT_TIMER_PROCESS_CMD

DESCRIPTION   Handles expiry of max ppp inactivity timer. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_mpit_timer_process_cmd
(
  ds_cmd_type * cmd_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);
  if( cmd_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid ds cmd ptr!");
    return;
  }

  if( ( cmd_ptr->hdr.cmd_id != DS_CMD_707_MPIT_TIMEOUT ) )
  {
    return;
  }

  DATA_IS707_MSG0(MSG_LEGACY_MED, "Max PPP Inactivity Timer expired.");
  ds707_pdncntx_teardown_all_pdns(PS_NET_DOWN_REASON_INTERNAL_MPIT_EXPIRED);
} /* ds_3gpp2_mpit_timer_process_cmd() */

#ifdef FEATURE_DATA_3GPP2_ALWAYS_ON
/*===========================================================================
FUNCTION      DS_3GPP2_LSD_TIMER_PROCESS_CMD

DESCRIPTION   Handles expiry of Link Status Determination timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp2_lsd_timer_process_cmd
(
  ds_cmd_type * cmd_ptr
)
{
  ppp_lsd_config_type lsd_config;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);
  if( cmd_ptr == NULL )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Invalid ds cmd ptr!");
    return;
  }

  if( ( cmd_ptr->hdr.cmd_id != DS_CMD_707_LSD_TIMEOUT ) )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    If VCI exchange was successful, trigger link status determination
  -------------------------------------------------------------------------*/
  if( always_on_cb_ptr->is_vci_done )
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "LSD Timer expired.");
    memset( &lsd_config, 0, sizeof(ppp_lsd_config_type) );
    lsd_config.echo_req_attempts = always_on_cb_ptr->config.lsd.echo_req_tries;
    lsd_config.echo_req_timeout = always_on_cb_ptr->config.lsd.echo_req_timeout;
    lsd_config.result_cback = 
      (ppp_lsd_result_cback_f_type)ds_3gpp2_lsd_result_cback; 
    lsd_config.user_data = &always_on_cb_ptr->config.lsd;
    if( 0 != ppp_trigger_lsd( PPP_UM_SN_DEV, &lsd_config ) )
    {
      /*---------------------------------------------------------------------
        LSD failed, log an error and return;
      ---------------------------------------------------------------------*/
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Link Status Determination failed!");
    }
  } /* VCI exchange was successful */
} /* ds_3gpp2_lsd_timer_process_cmd() */
#endif /* FEATURE_DATA_3GPP2_ALWAYS_ON */
#endif /*  #if ( defined(FEATURE_DATA_3GPP2_ALWAYS_ON) || defined(FEATURE_DATA_3GPP2_VS) ) */
