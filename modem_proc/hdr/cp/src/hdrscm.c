/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      H D R   S E S S I O N   C O N F I G U R A T I O N   M A N A G E R
       
GENERAL DESCRIPTION
  This module provides a common API for new protocols to use to process
  ConfigurationRequest messages and AttributeUpdateRequest messages.
    
EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  
  
  Copyright (c) 2005 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscm.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/16   vko     Reduce excessive F3 logging
05/06/16   vko     Reduce excessive F3 logging
07/09/14   cnx     Support NV refresh without reset.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
09/19/13   ukl     Reduce Code Duplication HDR-CP
02/17/13   arm     Supported NV enable/disable for network dtx attributes. 
04/29/13   smd     Disable RevB code if FEATURE_HDR_REVB_SUPPORTED is not defined.
04/08/13   cnx     Don't process L2 ACK and L3 resp in twisted.
05/25/12   wsh     Fixed MultiATPage GAUP not working
03/08/12   wsh     Merged NLB support
07/21/10   cnx     Added support for GAUP LoadInformationSupported attribute
11/22/10   cnx     Added hdrscmgmmscd_is_simple_attrib_valid().  
06/21/10   cnx     Fixed compiler warnings.
10/27/09   pba     Klockwork fixes.
09/10/09   etv     Allow only default value for MaxRLPilotTxPwrDiff during 
                   session negotiation.
09/07/08   etv     Added support for AN GAUP for PHY protocol.
05/22/08   pba     Klockwork fixes.
02/06/08   pba     For ConfigurationRequest message TX outcome SCM handles 
                   only E_SUCCESS and E_FAILURE and ignores the rest.
09/25/07   wsh     Fixed ASSERT crash in SCM module when sending GAUP
08/22/07   etv     Fixed high lint warnings.
08/15/07   etv     Added support for PHY module.
08/22/07   wsh     Fixed bug where GAUP response is ignored under some
                   situations when GAUP is sent over RTC reliably.
08/16/07   wsh     Removed compiler and lint critical warnings
07/19/07   sju     Fixed value_id_size in hdrscm_process_rsp_msg()
07/13/07   sju     Added support for MC RUP negotiation
06/19/07   cc      Added support for AT-initiated FTC MAC config.
06/14/07   kss     Added support for AT-initiated AC MAC config.
06/14/07   wsh     Moved value_id comparision to SCM instead of individual 
                   modules when handling get_prop_attrib()
03/27/07   wsh     Added support for new prior session handling
03/06/07   wsh     Removal of RVCT compiler warnings
03/06/07   etv     Decremented the num_in_use timers from tx_outcome_cb.
02/16/07   wsh     Added value_id/attrib_id checking processing ConfigResp
01/03/07   etv     Added support to handle unknown attributes.
01/08/07   yll     Added support for CSNA attributes.
12/18/06   pba     Used the newer routine to allocate DSM buffer.
11/10/06   etv     Handle E_NOT_AVAILABLE ( could be returned by HMP for 
                   ConfigLock) in tx_outcome_cb as E_FAILURE.
10/04/06   pba     Since now we write to EFS only during graceful powerdown, 
                   removed obsolete code that tells SCMDB when to flush GAUP'ed
                   data to EFS.
09/14/06   hal     Added GAUP support for MaxNoMonitorDistance
08/16/06   etv     Change interface get_simple_attr_size to take attrib_id.
07/25/06   mpa     Added missing entry to hdrscmovhd
06/21/06   etv     Added hdrscmovhd module to support OMP config attribs.
06/14/06   etv     Added support for AT init GAUP of SlottedMode
07/11/06   etv     Fixed bug by passing dsm_item_typ** instead of dsm_item_typ*
06/08/06   etv     Send session close if AN GAUPs a non-GAUPable attribute.
06/12/06   mpa     Updates for 7500 RPCing of HDR rev0
05/15/06   etv     Reset number of inuse rsp timers to zero on session close.
                   Dumping hash table for debugging.
05/10/06   yll/ksu Added support for EMPA
05/05/06   pba     Use updated SCMDB API that commits all attributes in a given
                   GAUP message to EFS at once instead of one at a time.
04/21/06   hal     Added hdrscm_clear_all_rsp_timers()
04/18/06   etv     Enhanced SCM to support indefinite num of attribs in CReq.
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
03/22/06   pba     Send Session close if AT could not accept AN's AUR message.
02/23/06   hal     Added support for configuring the AMP module
02/07/06   dna     Don't put gaup_trans_id in CONFIG_COMPLETE indications.
01/31/06   etv     Removed ASSERT for graceful degradation.
01/31/06   etv     Added support for AT-init Neg. for RMAC3.
12/15/05   etv     Added a few F3 messages.
12/13/05   pba     Modified SCMDB calls to use updated API's
12/07/05   etv     Register a function pointer to get simple attrib size.
12/06/05   etv     Added SCM support for AMAC, CMAC and GMMCDP.
11/17/05   etv     Increased the number of allowed attributes in a CReq to 20
11/10/05   etv     Removed temporary feature FEATURE_HDR_SCM_RMAC3_HACK
11/10/05   etv     Initialized attribute information structs before use.
11/07/05   pba     Moved hdrscm_reg_protocol_name_type to SCMDB, so explicitly 
                   defined HDRSCM_MAX_REG_PROTOCOLS
11/04/05   etv     Enabled RMAC3 negotiation using SCM
11/02/05   etv     Changed SCM design to propogate subtype info.
11/02/05   etv     Fixed overflow bug
10/26/05   hal     Added support for configuring the ALMP module
10/26/05   etv     Integration updates to GAUP support for RMAC1 rate params
10/21/05   pba     Added Broadcast protocol to SCM
10/10/05   etv     Added support for AN-init GAUP for RMAC.
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/07/05   etv     Added RTCMAC to SCM and minor code review fixes.
                   Added support for AT-initiated GAUP.
08/22/05   etv     Initial SCM support.
05/14/05   dna     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE
                     
===========================================================================*/
#include "hdr_variation.h"
#include "dsm.h"
#include "comdef.h"
#include "err.h"

#include "hdrbit.h" 
#include "hdrhai.h"
#include "hdrtrace.h"
#include "hdrdebug.h"
#include "hdrutil.h"
#include "hdrmci.h"
#include "hdrdsm.h"
#include "hdrhmp.h"
#include "hdrscp.h"
#include "hdrstream.h"
#include "hdrscm.h"
#include "hdrscmdb.h"
#include "hdrscmfmac.h"
#include "hdrscmrmac0.h"
#include "hdrscmrmac.h"
#include "hdrscmbccp.h"
#include "hdrscmalmp.h"
#include "hdrscmcmac.h"
#include "hdrscmamac.h"
#include "hdrscmgmmcdp.h"
#include "hdrscmrup.h"
#include "hdrscmamp.h"
#include "hdrscmidle.h"
#include "hdrscmovhd.h"
#include "hdrscmcsna.h"
#include "hdrscmscp.h"
#include "hdrscmphy.h"

/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

#define HDRSCM_PROTOCOL_NOT_FOUND       0xFF

#ifdef FEATURE_HDR_PS_GAUP
#error code not present
#else
#define HDRSCM_MAX_REG_PROTOCOLS        13
#endif /* FEATURE_HDR_PS_GAUP */

 /* EJECT */
/*===========================================================================
            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*------------------------------------------------------------------------
                                  MACROS 
 -----------------------------------------------------------------------*/
#define HDRSCM_CRSP_ATTRIB_LEN_BYTES( aip ) \
  (((aip)->attrib_id_size + \
    (aip)->sattr_val_size) / HDRSCM_BITS_PER_BYTE)

#define HDRSCM_EMPTY_ATTRIB_LEN( pip ) \
  ((pip)->attrib_len_size + (pip)->attrib_id_size)

#define HDRSCM_SIMPLE_ATTRIB_LEN( pip ) \
  ((pip)->attrib_len_size + (pip)->attrib_id_size + \
   (pip)->value_id_size)

/* Used when a particular kind of indication is not applicable for a protocol 
   in PIP */
#define HDRSCM_IND_NOT_APPLICABLE           HDRIND_IND_NAME_ENUM_MAX

/*------------------------------------------------------------------------
 * The following are the function prototypes of the callbacks that each 
 * target protocol registers with SCM for various purposes 
------------------------------------------------------------------------*/

/* Attribute handlers parse a given attribute and return to SCM via out_ptr
 * the outcome of the parsing, and of the value that was accepted (if any).
 */
typedef hdrerrno_enum_type (*hdrscm_complex_attrib_handler_type)
(
  dsm_item_type                             *item_ptr, 
    /* Received message */
  hdrscm_proposed_attrib_type  const *const in_ptr,   
    /* input params to the parser */
  hdrscm_accepted_attrib_type        *const out_ptr,
    /* return values from parser */
  uint16                              const subtype
);

/* This function checks if the attribute is simple, given the attribute ID */
typedef boolean (*hdrscm_is_attrib_simple_type)
(
  uint16 const attrib_id,
  uint16 const subtype
);
 
/* This function checks if the parsed value of a simple attribute is valid */
typedef hdrerrno_enum_type (*hdrscm_simple_attrib_validity_checker_type)
(
  uint16 const attrib_id,
  uint32 const attrib_value,
  uint16 const subtype
);

/* This function is used by SCM to get the value of the complex attribute,
   that the protocol xyz proposed as a GAUP Attribute Update request or
   AT-initiated Session Negotiation */
typedef hdrerrno_enum_type (*hdrscm_get_proposed_attrib_value_type)
(
  uint16 const trans_id,
  hdrscm_accepted_attrib_type *const a_aip,
  uint16 const subtype
);

typedef hdrerrno_enum_type (*hdrscm_get_value_of_gauped_attributes_type)
(
  uint8 const trans_id,
  dsm_item_type **aai_dsm_ptr,
  uint8 *const attrib_count,
  uint16 const subtype
);

/* This function is used by SCM to get the size of the Attrib ID in bits. For
   Most protocols, Attrib ID size is protocol specific. However, for certain
   protocols like RMAC, the size of attrib ID is subtype-specific. */
typedef uint8 (*hdrscm_get_attrib_id_size_type)
(
  uint16 const subtype
);

/* This function is used by SCM to fail the AT init Session negotiation */
typedef void (*hdrscm_fail_curr_config_neg_type)
(
  hdrhai_session_close_reason_enum_type close_reason
);

/* This function is used by SCM to get the size of the simple attrib in bits. 
   For Most protocols, this size is protocol specific. However, for certain
   protocols like FMAC, the size of simple attrrib is subtype-specific and
   for certain other apps like CSNA, the simple attribute size is 
   attribute-specific. */
typedef uint8 (*hdrscm_get_simple_attrib_size_type)
(
  uint16 const attrib_id,
  uint16 const subtype
);

/* This function is used by SCM to find if an attribute is GAUPable. */
typedef hdrerrno_enum_type (*hdrscm_is_attrib_gaupable_type)
(
  uint16 const attrib_id,
  uint16 const subtype
);

typedef boolean (*hdrscm_attrib_is_supported_type)
(
  uint16 const attrib_id,
  uint16 const subtype
);

/*--------------------------------------------------------------------------
 * "hdrscm_reg_type" encapsulates the Protocol specific information that SCM 
 * needs to know. Each of the Regisetered protocol would have one instance 
 * of this struct in static memory and SCM would use this information to 
 * parse the attribs.
--------------------------------------------------------------------------*/
typedef struct
{
  uint8 protocol_index;
    /* Index of the PIP in the registration table */

  hdrhai_protocol_name_enum_type protocol_name;
    /* Protocol Name */

  hdrscm_complex_attrib_handler_type parse_complex_attrib;
    /* Function to parse complex attributes */

  hdrscm_is_attrib_simple_type is_simple;
    /* Function to check if an attribute is simple */

  hdrscm_simple_attrib_validity_checker_type is_valid;
    /* Function to check if the parsed simple attrib is valid */

  hdrscm_get_proposed_attrib_value_type 
    get_proposed_attrib_value;
    /* Function to get the value of the proposed complex attribute */

  hdrscm_get_value_of_gauped_attributes_type get_value_of_gauped_attributes;
    /* Function to get the value of the GAUPed attributes */

  hdrscm_get_attrib_id_size_type get_attrib_id_size;
    /* Function to get the size of the attrib Id. field 'attrib_id_size'
       is used if this points to NULL */

  hdrscm_fail_curr_config_neg_type fail_neg;
    /* Function to fail the current Config negotiation */

  hdrscm_get_simple_attrib_size_type get_simple_attrib_size;
    /* Function to get the size of the value ID. */

  hdrscm_is_attrib_gaupable_type is_attrib_gaupable;
    /* Is attribute GAUPable? */

  hdrscm_attrib_is_supported_type attrib_is_supported;
    /* Is this attribute supported? */

  uint8 message_id_size;
    /* Size in bits of the 'Message ID' field of Config. req. message */

  uint8 transaction_id_size;
    /* Size in bits of the 'Transaction ID' field of Config. req. message */
    
  uint8 attrib_len_size;
    /* Size in bits of the "Length" field of Attribute Record */

  uint8 attrib_id_size;
    /* Size in bits of the "Attrib ID" field of Attribute Record */

  uint8 value_id_size;
    /* Size in bits of the "Value ID" field of Attribute Record */

  hdrind_ind_name_enum_type an_gaup;
    /* Indication given to the protocol when the AttribUpdateRequest(GAUP) sent
       by AN is accepted by AT and the current attribute has changed */

  hdrind_ind_name_enum_type gaup_accepted;
    /* Indication given to the protocol in AT when AttribUpdateRequest(GAUP) 
       sent is accepted by AN */

  hdrind_ind_name_enum_type gaup_rejected;
    /* Indication given to the protocol in AT when AttribUpdateRequest(GAUP) 
       sent is rejected by AN */

  hdrind_ind_name_enum_type gaup_failed;
    /* Indication given to the protocol in AT when AttribUpdateRequest(GAUP) 
       transmission has failed */

  hdrind_ind_name_enum_type config_complete;
    /* Indication given when the Configuration Request made by a protocol is
       complete */

} hdrscm_reg_type;


/* Number of concurrent GAUP requests. Has to be a Modulo-2 number. 
   A Modulo-2 number is chosen to make the wrapping around faster. 
   Otherwise it would be necessary to use the expensive Mod (%) operator */
#define HDRSCM_MAX_NUM_CONCURRENT_GAUPS             0x08

/*--------------------------------------------------------------------------
  The struct 'hdrscm_rsp_timer_type' defines the timer used for AT initiated
  GAUP or Session Configuration. In addition to ther timer instance, it holds 
  all relevant information required for the timer management.
--------------------------------------------------------------------------*/
typedef struct
{
  rex_timer_type rsp_timer;
    /* Response timer. Could either be used for GAUP or session configuration */

  uint8 trans_id;
    /* Transaction ID associated with this timer */

  hdrhai_protocol_name_enum_type prot_name;
    /* Name of the protocol using this timer instance. Could be used for
       debugging purposes */

  boolean in_use;
    /* Is the Response timer in use? */

  boolean is_gaup;
    /* Is the Response timer used for GAUP? */

} hdrscm_rsp_timer_type;

/*--------------------------------------------------------------------------
  Array of Response timers used as an hash table. Everytime a protocol sends
  a GAUP update or configuration request message, it reserves a timer from
  this hash table to monitor the reception of response message. The SCM
  module allocates a timer based on a running index which serves as a
  primitive hash function and the collision is resolved using linear probing.
  Everytime a reservation is requested by a protocol the timer instance at
  the current index would be inspected for availability. 
   1. If it is avaliable, it would be assigned to requested protocol and 
      the index would be incremented to next index which would wrap 
      around. However, until a request for next reservation, SCM doesn't
      care if the timer instance at this index is available.
   2. If after a reservation request, if SCM finds that the timer instance
      at the current index is in-use, it probes for the next available 
      index by linear probing.

  Note: It is interesting to note that this hash table is just a Circular
  Queue in disguise under ideal conditions. When there are no out-of-order 
  timer expiry or response message reception, this data structure behaves as
  a circular Queue. However, out-of-order timer expiry or response message
  reception could create random holes in the queue and it would force linear
  probing in the hash table to find available timer instances.

  Why is an out-of-order timer expiry possible? It is possible because, 
  the timer reservation and timer start doesn't happen synchronously. 
  The requesting protocol reserves a timer which would be allocated 
  sequentially from the running index, and packs the update message
  and sends it. Only after it receives a notification from HMP through 
  a callback that the message was sent successfully, the timer would be
  started. So there are chances for delay between reservation and actual
  timer start which could lead to out-of-order timer expiry.
--------------------------------------------------------------------------*/
typedef hdrscm_rsp_timer_type 
  hdrscm_rsp_timer_hash_table_type[HDRSCM_MAX_NUM_CONCURRENT_GAUPS];


/*------------------------------------------------------------------------
                            GLOBAL VARIABLES
 -----------------------------------------------------------------------*/
/* The following structure contains all the global variables used by SCM */
LOCAL struct
{
  hdrscm_rsp_timer_hash_table_type rt_hash_table;
    /* Hash table */

  uint8 rt_hash;
    /* Running array index used as a primitive hash function */
       
  uint8 num_in_use_timers;
    /* Total number of in-use timers. */

  rex_crit_sect_type timer_crit_sect; 
    /* Critical Section for timer Management */

  boolean is_gaup_pending[HDRSCM_MAX_REG_PROTOCOLS];
    /* Whether GAUP is pending for a protocol given by the index? */

} hdrscm;


/*------------------------------------------------------------------------
                              LOOKUP TABLES
 -----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * The table "hdrscm_reg" contains one instance of hdrscm_reg_type struct
 * for each of the registered protocols.
------------------------------------------------------------------------*/
const hdrscm_reg_type hdrscm_reg[HDRSCM_MAX_REG_PROTOCOLS] = 
{
  /*------------------------------------------------------------------------
                         CONTROL CHANNEL MAC PROTOCOL
  ------------------------------------------------------------------------*/
  {
    0,                                    /* Protocol Index */
    HDRHAI_CC_MAC_PROTOCOL,               /* Protocol Name */

    /* Function Pointers */
    NULL,                                 /* Parses Complex attribute */
    hdrscmcmac_is_attrib_simple,          /* Checks if attrib is simple */
    hdrscmcmac_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    NULL,                                 /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value */
    NULL,                                 /* Get Attrib ID size; Constant */
    NULL,                                 /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    NULL,                                 /* Is attrib GAUPable */
    hdrscmcmac_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* Message ID size in bits */
    8,                                    /* Transaction ID size in bits */
    8,                                    /* Attrib Length size in bits */ 
    8,                                    /* Attrib_id_size (max 16)   */
    8,                                    /* Value ID size in bits */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind given when AN GAUPs */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateAccept */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateReject */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for failed GAUP xmission */
    HDRSCM_IND_NOT_APPLICABLE             /* No AT-init Config */
  },
  /*------------------------------------------------------------------------
                         ACCESS CHANNEL MAC PROTOCOL
  ------------------------------------------------------------------------*/
  {
    1,                                    /* Protocol Index */
    HDRHAI_AC_MAC_PROTOCOL,               /* Protocol Name */

    /* Function Pointers */
    hdrscmamac_process_complex_attrib,    /* Parses Complex attribute */
    hdrscmamac_is_attrib_simple,          /* Checks if attrib is simple */
    hdrscmamac_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    hdrscmamac_get_prop_attrib_value,     /* Get proposed attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value */
    NULL,                                 /* Get Attrib ID size; Constant */
    hdrscmamac_fail_curr_config_neg,      /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    hdrscmamac_is_attrib_gaupable,        /* Is attribute GAUPable */
    hdrscmamac_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* Message ID size in bits */
    8,                                    /* Transaction ID size in bits */
    8,                                    /* Attrib Length size in bits */ 
    8,                                    /* Attrib_id_size (max 16)   */
    8,                                    /* Value ID size in bits */

    /* Indications */
    HDRIND_SCM_AMAC_GAUP,                 /* Ind given when AN GAUPs */
    HDRIND_SCM_AMAC_AT_GAUP_ACCEPTED,     /* Ind for AttribUpdateAccept */
    HDRIND_SCM_AMAC_AT_GAUP_REJECTED,     /* Ind for AttribUpdateReject */
    HDRIND_SCM_AMAC_AT_GAUP_FAILED,       /* Ind for failed GAUP xmission */
    HDRIND_AMAC_CONFIG_COMPLETE           /* AT-init Config */
  },
  /*------------------------------------------------------------------------
                      FORWARD TRAFFIC CHANNEL MAC PROTOCOL
  ------------------------------------------------------------------------*/
  {
    2,                                    /* Protocol Index */
    HDRHAI_FTC_MAC_PROTOCOL,              /* protocol_name                   */

    /* Function Pointers */
    hdrscmfmac_process_complex_attrib,    /* parses complex attribute        */
    hdrscmfmac_is_attrib_simple,          /* Checks if attrib is simple      */
    hdrscmfmac_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    hdrscmfmac_get_prop_attrib_value,
                                          /* Get proposed comp attrib value  */
#ifdef FEATURE_HDR_REVC
    hdrscmfmac_get_gauped_attrib_value,   /* Get GAUPed comp attrib value    */
#else
    NULL,                                 /* Get GAUPed comp attrib value    */
#endif /* FEATURE_HDR_REVC */
    NULL,                                 /* Attrib ID size constant */
    hdrscmfmac_fail_curr_config_neg,      /* Fail Current Config Negotiation */
    hdrscmfmac_get_simple_attrib_size,    /* Get Simple Attribute value size */
    hdrscmfmac_is_attrib_gaupable,        /* Is attribute GAUPable */
    hdrscmfmac_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRIND_SCM_FMAC_GAUP,                 /* Ind given when AN GAUPs */
    HDRIND_SCM_FMAC_AT_GAUP_ACCEPTED,     /* Ind for AttribUpdateAccept */
    HDRIND_SCM_FMAC_AT_GAUP_REJECTED,     /* Ind for AttribUpdateReject */
    HDRIND_SCM_FMAC_AT_GAUP_FAILED,       /* Ind for failed GAUP xmission */
    HDRIND_FMAC_CONFIG_COMPLETE           /* Ind for AT-init Config */
  },
  /*------------------------------------------------------------------------
                      REVERSE TRAFFIC CHANNEL MAC PROTOCOL
  ------------------------------------------------------------------------*/
  {
    3,                                    /* Protocol Index */
    HDRHAI_RTC_MAC_PROTOCOL,              /* Protocol Name */

    /* Function Pointers */
    hdrscmrmac_process_complex_attrib,    /* Parses Complex attribute */
    hdrscmrmac_is_attrib_simple,          /* Checks if attrib is simple */
    hdrscmrmac_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    hdrscmrmac_get_prop_attrib_value,                                 
                                          /* Get proposed comp attrib value  */
    hdrscmrmac_get_gauped_complex_attrib_value,
                                          /* Get GAUPed comp attrib value */
    hdrscmrmac_get_attrib_id_size,        /* Gets attrib ID size in bits */
    hdrscmrmac_fail_curr_config_neg,      /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    hdrscmrmac_is_attrib_gaupable,        /* Is attribute GAUPable */
    hdrscmrmac_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* Message ID size in bits */
    8,                                    /* Transaction ID size in bits */
    8,                                    /* Attrib Length size in bits */ 
    8,                                    /* Use the function pointer instead */
    8,                                    /* Value ID size in bits */

    /* Indications */
    HDRIND_SCM_RMAC_GAUP,                 /* Ind given when AN GAUPs */
    HDRIND_SCM_RMAC_AT_GAUP_ACCEPTED,     /* Ind for AttribUpdateAccept */
    HDRIND_SCM_RMAC_AT_GAUP_REJECTED,     /* Ind for AttribUpdateReject */
    HDRIND_SCM_RMAC_AT_GAUP_FAILED,       /* Ind for failed GAUP xmission */
    HDRIND_RMAC_CONFIG_COMPLETE           /* AT-init Config */
  },
  /*------------------------------------------------------------------------
                              BROADCAST PROTOCOL
  ------------------------------------------------------------------------*/
  {
    4,                                    /* Protocol Index */
    HDRHAI_BROADCAST_PROTOCOL,            /* protocol_name                   */

    /* Function Pointers */
    NULL,                                 /* parses complex attribute        */
    hdrscmbccp_is_attrib_simple,          /* Checks if attrib is simple      */
    hdrscmbccp_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    NULL,                                 /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value    */
    NULL,                                 /* Attrib ID size constant         */
    NULL,                                 /* Fail Current Config Negotiation */ 
    NULL,                                 /* Get simple attribute size */
    NULL,                                 /* Is attribute GAUPable */
    hdrscmbccp_attrib_is_supported,       /* Is this attribute supported ? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AN-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE             /* No AT-init Config */
  },
  /*------------------------------------------------------------------------
                            AIR LINK MANAGEMENT PROTOCOL
  ------------------------------------------------------------------------*/
  {
    5,                                    /* Protocol Index */
    HDRHAI_AIR_LINK_MANAGEMENT_PROTOCOL,  /* Protocol Name */

    /* Function Pointers */
    hdrscmalmp_process_complex_attrib,    /* Parses Complex attribute        */
    hdrscmalmp_is_attrib_simple,          /* Checks if attrib is simple      */
    hdrscmalmp_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    NULL,                                 /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value    */
    NULL,                                 /* Attrib ID size constant         */
    NULL,                                 /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    NULL,                                 /* Is attribute GAUPable */
    hdrscmalmp_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AN-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE             /* No AT-init Config */
  },
  /*------------------------------------------------------------------------
                      MULTI-MODE CAPABILITY DISCOVERY PROTOCOL
  ------------------------------------------------------------------------*/
  {
    6,                                    /* Protocol Index */
    HDRHAI_GENERIC_MM_CAP_DISC_PROTOCOL,  /* Protocol Name */

    /* Function Pointers */
    NULL,                                 /* Parses Complex attribute        */
    hdrscmgmmcdp_is_attrib_simple,        /* Checks if attrib is simple      */
    hdrscmgmmscd_is_simple_attrib_valid,  /* Check if simple attrib is valid */
    hdrscmgmmcdp_get_prop_attrib_value,
                                          /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value    */
    NULL,                                 /* Attrib ID size constant         */
    hdrscmgmmcdp_fail_curr_config_neg,    /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    NULL,                                 /* Is attribute GAUPable */
    hdrscmgmmcdp_attrib_is_supported,     /* Is this attribute supported? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AN-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRIND_GMMCDP_CONFIG_COMPLETE         /* AT-init Config Complete */
  },
  /*------------------------------------------------------------------------
                            ROUTE UPDATE PROTOCOL
  ------------------------------------------------------------------------*/
  {
    7,                                    /* Protocol Index */
    HDRHAI_ROUTE_UPDATE_PROTOCOL,         /* Protocol Name */

    /* Function Pointers */
    NULL,                                 /* Parses Complex attribute        */
    hdrscmrup_is_attrib_simple,           /* Checks if attrib is simple      */
    NULL,                                 /* Check if simple attrib is valid */
    hdrscmrup_get_proposed_attrib_value,
                                          /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value    */
    NULL,                                 /* Attrib ID size constant         */
    hdrscmrup_fail_curr_config_neg,       /* Fail Current Config Negotiation */
    hdrscmrup_get_simple_attrib_size,     /* Get simple attribute size */
    NULL,                                 /* Is attribute GAUPable */
    NULL,                                 /* Is this attribute supported? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AN-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRIND_RUP_CONFIG_COMPLETE            /* AT-init Config Complete */
  },
  /*------------------------------------------------------------------------
                           ADDRESS MANAGEMENT PROTOCOL
  ------------------------------------------------------------------------*/
  {
    8,                                    /* Protocol Index */
    HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,   /* Protocol Name */

    /* Function Pointers */
    hdrscmamp_process_complex_attrib,     /* Parses Complex attribute        */
    hdrscmamp_is_attrib_simple,           /* Checks if attrib is simple      */
    hdrscmamp_is_simple_attrib_valid,     /* Check if simple attrib is valid */
    NULL,                                 /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value    */
    NULL,                                 /* Attrib ID size constant         */
    NULL,                                 /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    hdrscmamp_is_attrib_gaupable,         /* Is attribute GAUPable */
    hdrscmamp_attrib_is_supported,        /* Is this attribute supported? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRIND_SCM_AMP_GAUP,                  /* Ind given when AN GAUPs */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AT-init GAUP */
    HDRSCM_IND_NOT_APPLICABLE             /* AT-init Config Complete */
  },
  /*------------------------------------------------------------------------
                            IDLE STATE PROTOCOL
  ------------------------------------------------------------------------*/
  {
    9,                                    /* Protocol Index */
    HDRHAI_IDLE_STATE_PROTOCOL,           /* Protocol Name */

    /* Function Pointers */
    NULL,                                 /* Parses Complex attribute        */
    hdrscmidle_is_attrib_simple,          /* Checks if attrib is simple      */
    NULL,                                 /* Check if simple attrib is valid */
    NULL,                                 /* Get proposed comp attrib value  */
    hdrscmidle_get_gauped_complex_attrib_value,                                 
                                          /* Get GAUPed comp attrib value    */
    NULL,                                 /* Attrib ID size constant         */
    NULL,                                 /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    NULL,                                 /* Is attrib GAUPable */
    NULL,                                 /* Is this attribute supported? */

    /* Constants */
    8,                                    /* message_id_size      (max 8)    */
    8,                                    /* transaction_id_size  (max 8)    */
    8,                                    /* attrib_len_size      (max 8)    */
    8,                                    /* attrib_id_size       (max 16)   */
    8,                                    /* value_id_size        (max 16)   */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* No AN-init GAUP                 */
    HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED,     /* Ind for AttribUpdateAccept      */
    HDRIND_SCM_IDLE_AT_GAUP_REJECTED,     /* Ind for AttribUpdateReject      */
    HDRIND_SCM_IDLE_AT_GAUP_FAILED,       /* Ind for failed GAUP xmission    */
    HDRSCM_IND_NOT_APPLICABLE             /* No AT-init Negotiation          */
  },
  /*------------------------------------------------------------------------
                      OVERHEAD MESSAGES PROTOCOL
  ------------------------------------------------------------------------*/
  {
    10,                                   /* Protocol Index */
    HDRHAI_OVERHEAD_MESSAGES_PROTOCOL,    /* Protocol Name */

    /* Function Pointers */
    NULL,                                 /* Parses Complex attribute */
    hdrscmovhd_is_attrib_simple,          /* Checks if attrib is simple */
    hdrscmovhd_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    NULL,                                 /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value */

    NULL,                                 /* Get Attrib ID size; Constant */
    NULL,                                 /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    NULL,                                 /* Is attrib GAUPable */
    hdrscmovhd_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* Message ID size in bits */
    8,                                    /* Transaction ID size in bits */
    8,                                    /* Attrib Length size in bits */ 
    8,                                    /* Attrib_id_size (max 16)   */
    8,                                    /* Value ID size in bits */

    /* Indications */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind given when AN GAUPs */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateAccept */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateReject */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for failed GAUP xmission */
    HDRSCM_IND_NOT_APPLICABLE             /* No AT-init Config */
  },
  /*------------------------------------------------------------------------
                Circuit Services Notification Application
  ------------------------------------------------------------------------*/
  {
    11,                                   /* Protocol Index */
    HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL, /* Protocol Name */

    /* Function Pointers */
    hdrscmcsna_process_complex_attrib,    /* Parses Complex attribute */
    hdrscmcsna_is_attrib_simple,          /* Checks if attrib is simple */
    hdrscmcsna_is_simple_attrib_valid,    /* Check if simple attrib is valid */
    hdrscmcsna_get_prop_attrib_value,                                 
                                          /* Get proposed comp attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value */
    NULL,                                 /* Gets attrib ID size in bits */
    hdrscmcsna_fail_curr_config_neg,      /* Fail Current Config Negotiation */
    hdrscmcsna_get_simple_attrib_size,    /* Get simple attribute size */
    hdrscmcsna_is_attrib_gaupable,        /* Is attribute GAUPable */
    hdrscmcsna_attrib_is_supported,       /* Is this attribute supported? */

    /* Constants */
    8,                                    /* Message ID size in bits */
    8,                                    /* Transaction ID size in bits */
    8,                                    /* Attrib Length size in bits */ 
    8,                                    /* Attrib_id_size (max 16)   */
    8,                                    /* Value ID size in bits */

    /* Indications */
    HDRIND_SCM_CSNA_AN_GAUPED,            /* Ind given when AN GAUPs */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateAccept */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateReject */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for failed GAUP xmission */
    HDRIND_SCM_CSNA_AT_CONFIG_COMPLETE    /* Ind AT-init Config */
  },
#ifdef FEATURE_HDR_PS_GAUP
  #error code not present
#endif /* FEATURE_HDR_PS_GAUP */
  {
    13,                                   /* Protocol Index */
    HDRHAI_PHYSICAL_LAYER_PROTOCOL,       /* Protocol Name */

    /* Function Pointers */
    hdrscmphy_process_complex_attrib,     /* Parses Complex attribute */
    hdrscmphy_attrib_is_simple,           /* Checks if attrib is simple */
    hdrscmphy_simple_attrib_is_valid,     /* Check if simple attrib is valid */
    hdrscmphy_get_proposed_attrib_value,  /* Get proposed attrib value  */
    NULL,                                 /* Get GAUPed comp attrib value */
    NULL,                                 /* Get Attrib ID size; Constant */
    hdrscmphy_fail_curr_config_neg,       /* Fail Current Config Negotiation */
    NULL,                                 /* Get simple attribute size */
    hdrscmphy_attrib_is_gaupable,         /* Is attrib GAUPable */
    hdrscmphy_attrib_is_supported,        /* Is this attribute supported? */

    /* Constants */
    8,                                    /* Message ID size in bits */
    8,                                    /* Transaction ID size in bits */
    8,                                    /* Attrib Length size in bits */ 
    8,                                    /* Attrib_id_size (max 16)   */
    8,                                    /* Value ID size in bits */

    /* Indications */
    HDRIND_SCM_PHY_AN_GAUP,               /* Ind given when AN GAUPs */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateAccept */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for AttribUpdateReject */
    HDRSCM_IND_NOT_APPLICABLE,            /* Ind for failed GAUP xmission */
    HDRIND_PHY_CONFIG_COMPLETE            /* AT-init Config */
  }
};



/*============================================================================

                        LOCAL FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================
FUNCTION HDRSCM_FIND_REG_ENTRY

DESCRIPTION
  Finds the specified protocol in the registration table

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Protocol to look for in registration table
  
RETURN VALUE
  Pointer to protocol's registered information or NULL if it is not registered.

SIDE EFFECTS
  None.
=============================================================================*/
static hdrscm_reg_type const *hdrscm_find_reg_entry
( 
  hdrhai_protocol_name_enum_type protocol_name
)
{
  int pip_index; /* index */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (pip_index = 0; pip_index < HDRSCM_MAX_REG_PROTOCOLS; pip_index++)
  {
    if (hdrscm_reg[pip_index].protocol_name == protocol_name)
    {
      return &hdrscm_reg[pip_index];
    }
  }

  return NULL;

} /* hdrscm_find_reg_entry() */



/*=============================================================================
FUNCTION HDRSCM_FIND_RSVD_TIMER

DESCRIPTION
  Find the timer reserved for the transaction specified by trans_id. 
  If an InUse timer record is found, a pointer to it is returned in the 
  parameter. If not, the timer for this transaction has probably expired and 
  was removed from the table. Return an error message.

DEPENDENCIES
  None.

PARAMETERS
  
RETURN VALUE
  E_SUCCESS - If a timer was successfully found for this transaction.
  E_TIMER_EXP - If a timer record was not found for this transaction.

SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscm_find_rsvd_timer
(
  hdrhai_protocol_name_enum_type protocol,
  uint8 trans_id,
  hdrscm_rsp_timer_type **rsp_timer_ptr
)
{
  hdrerrno_enum_type err_no = E_TIMER_EXP;
    /* Error Code. Assume that timer has expired for this transaction */

  uint8 timer_ind = 0;
    /* Timer Index in the hash table */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Just loop around the hash table until we find the timer record or until the
     end of it */
  for ( timer_ind = 0; (timer_ind < HDRSCM_MAX_NUM_CONCURRENT_GAUPS); 
        ++timer_ind )
  {
    /* If the current timer record is the one, */
    if ( (hdrscm.rt_hash_table[timer_ind].prot_name == protocol) &&
         (hdrscm.rt_hash_table[timer_ind].trans_id == trans_id) &&
         (hdrscm.rt_hash_table[timer_ind].in_use == TRUE) )
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "Find ConReq/GAUP Resp Timer=%d",
                      timer_ind );
      *rsp_timer_ptr = &hdrscm.rt_hash_table[timer_ind];
      err_no = E_SUCCESS;
      break;
    }
  }

  return err_no;

} /* hdrscm_find_rsvd_timer */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_PARSE_SIMPLE_ATTRIB                                  LOCAL

DESCRIPTION
  This is a generic routine to parse simple attributes of all protocols.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr - Pointer to DSM item containing Configuration Req. Message.
  pip      - Constant pointer to constant Protocol Information
  p_aip    - Constant pointer to constant Proposed Attribute Information.
  a_aip    - Constant pointer to Accepted Attribute Information.
  
RETURN VALUE
  E_SUCCESS if we could parse a valid attrib value from the config req. mesg.
  E_NO_DATA - Parsing failed because of insufficient bits in Attribute Record.
  E_OUT_OF_RANGE - Un-supported value proposed for the simple attribute.

SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscm_parse_simple_attrib
(
  dsm_item_type                           *item_ptr,
  hdrscm_reg_type             const *const pip,
  hdrscm_proposed_attrib_type const *const p_aip,
  hdrscm_accepted_attrib_type       *const a_aip,
  uint16                      const        subtype
)
{
  uint16 attrib_offset = p_aip->attrib_offset;
    /* Offset to the current byte in process from start of DSM chain which
     * is the start of the current attribute record */
  
  uint32 attrib_value;
    /* Value of the simple attribute */

  hdrerrno_enum_type err_no = E_NO_DATA;
    /* Error code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /* Initialize the attrib_offset to start of the first value_id */ 
  attrib_offset = p_aip->attrib_len_size + a_aip->attrib_id_size;
  
  /* Are there enough bits for a(nother) attrib value in the message? */
  while ( ( attrib_offset + a_aip->sattr_val_size) <= p_aip->attrib_len )
  {
    /*----------------------------------------------------------------------
     *  There is at least one more attribute value in this attribute Record
     *--------------------------------------------------------------------*/

    /* Initialize the error code to success since we know that we have at least
     * one attrib value in the Attrib Record that is hopefully valid! */
    err_no = E_SUCCESS;

    /* Unpack the Simple attrib Value into a uint32 */
    err_no = HDRBIT_UNPACK32( item_ptr, 
                              &(attrib_value), 
                              (p_aip->attrib_offset + attrib_offset), 
                              a_aip->sattr_val_size );

    if (err_no != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",
            0,0,0);
      break;
    }
    
    /* Though we could have multiple values in the Attribute record, the
     * default behaviour is to choose the first valid one. Is the parsed 
     * simple Attribute value Valid? */
    if ( (err_no = 
          pip->is_valid(p_aip->attrib_id, attrib_value, subtype)) == E_SUCCESS )
    {
#ifdef FEATURE_HDR_REVB_SUPPORTED //FEATURE_HDR_REVB
      if ( ( pip->protocol_name == HDRHAI_RTC_MAC_PROTOCOL ) &&
           ( subtype == HDRSCP_MC_RMAC ) &&
           ( p_aip->is_gaup == FALSE ) &&
           ( p_aip->attrib_id == HDRSCMRMAC4_MAX_RL_PILOT_TX_PWR_DIFF_ATTRIB ) &&
           ( attrib_value != HDRSCMRMAC4_DEFAULT_MAX_RL_PILOT_TX_PWR_DIFF ) )
      {
        /* Rejecting non-default MaxRLPilotTxPwrDiff attribute proposed during 
           session negotiation */
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "Rejecting non default (0x%x) MaxRLPilotTxPwrDiff",
                        attrib_value );
      }
      else
#endif /* FEATURE_HDR_REVB_SUPPORTED */
      {
        HDR_MSG_PROT_2(MSG_LEGACY_LOW, "Accepting simple attrib 0x%x value 0x%x",
                       a_aip->attrib_id,
                       attrib_value);

        a_aip->is_accepted = TRUE;
        a_aip->value_id = attrib_value;
        a_aip->complex_val_ptr = NULL;
        break;
      }
    }
    else
    {
      HDR_MSG_PROT_2(MSG_LEGACY_HIGH, "Rejecting simple attrib 0x%x value 0x%x",
                     p_aip->attrib_id,
                     attrib_value);
    }
    
    /* Since the Last attrib value is not valid, try the next one. Adjust
     * the offset to next attrib value */
    attrib_offset += a_aip->sattr_val_size;

  } /* While there are enough bits for another attrib value */
  
  /* Return True if we have successfully parsed a valid attrib value */
  return err_no;

} /* hdrscm_parse_simple_attrib() */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_UNPACK_LENGTH_AND_ATTRIB_ID

DESCRIPTION
  This function extract the length and attribute ID.
  
DEPENDENCIES
  None

PARAMETERS
  pip       - Protocol's registered information pointer
  item_ptr  - Pointer to the DSM item chain holding the message.
  attrib_offset - Byte offset of the AttributeRecord in CReq.
  total_bits_remaining - Total bits remaining in the CReq.
  p_attrib_len - Address where the parsed length is stored.
  p_attrib_id - Address where the parsed "attrib_id" is stored.
  p_attrib_id_size - Address where the attrib_id_size is stored.
  subtype - Protocol subtype.
  
RETURN VALUE
  E_NO_DATA - If we do not have enough bits to parse the fields.
  E_DATA_INVALID - If any of the field has invalid values.
  E_SUCCESS - Otherwise.
  
SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscm_unpack_length_and_attrib_id
(
  hdrscm_reg_type const *const pip,
  dsm_item_type *item_ptr,
  uint16 attrib_offset,
  uint16 total_bits_remaining,
  uint32 *p_attrib_len,
  uint16 *p_attrib_id,
  uint8 *p_attrib_id_size,
  uint16 subtype
)
{
  hdrerrno_enum_type err = E_SUCCESS;
    /* Error Code */

  uint8 attrib_len;
    /* Local variable for attribute length in bytes */

  uint32 attrib_len_in_bits;
    /* Attribute Length in bits */

  uint16 attrib_id;
    /* Local variable for Attribute ID */

  uint8 attrib_id_size;
    /* Size of the attribute ID in bits */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    /* Get the length of the attribute. If sufficient bits cannot be
       extracted from the DSM item for length... */
    if ( ( err = HDRBIT_UNPACK8( item_ptr, &attrib_len, attrib_offset,
                                 pip->attrib_len_size ) ) != E_SUCCESS )
    {
      ERR("Error unpacking Config Msg", 0, 0, 0 );

      /* Not being able to unpack the length field is an abnormal condition
         upon which we want to stop processing further and send response for
         the attribute records that have been unpacked already. */
      err = E_DATA_INVALID;

      /* Without the length we can't continue to parse the message.
         Break to error handler */
      break;
    }

    if (attrib_len == 0)
    {
      ERR("Error: Attrib length zero",0,0,0);

      /* Length field with a value of zero is an abnormal condition upon 
         which we want to stop processing further and send response for
         the attribute records that have been unpacked already. */
      err = E_DATA_INVALID;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in 
     * bytes, excluding the length of the length field.  For our purposes, 
     * we want attrib_len to be the total length of the attribute in bits, 
     * including the length field. */
    attrib_len_in_bits = attrib_len * HDRSCM_BITS_PER_BYTE;
    attrib_len_in_bits += pip->attrib_len_size;

    /* Any attribute record must have at least an attribute ID and a value */
    if ((attrib_len_in_bits < (uint32)HDRSCM_SIMPLE_ATTRIB_LEN( pip )) ||
        (attrib_len_in_bits > (uint32)total_bits_remaining ))
    {
      /* If the length is something unexpected, we can't be sure of what 
       * the AN intended and it is safest to not send a response */
        ERR("Error, invalid total len vs bits in message. Ignoring",0,0,0);
        err = E_NO_DATA;
        break;
    }

    /*---------------------------------------------------------------------
          There is at least one more attribute record in the message
    ---------------------------------------------------------------------*/

    /* Attrib-ID size is subtype-specific. For protocols that has different 
       attribute ID sizes for different subtypes, query using the registered 
       function pointer and find the attribute ID size */
    attrib_id_size = (pip->get_attrib_id_size == NULL)? 
                        pip->attrib_id_size: pip->get_attrib_id_size(subtype);

    /* Unpack the Attribute ID */
    if ( ( err = HDRBIT_UNPACK16( item_ptr, &attrib_id, 
                                  (attrib_offset + pip->attrib_len_size),
                                  attrib_id_size ) ) != E_SUCCESS )
    {
      ERR("Error extracting AttributeID from Config Msg. Ignore", 0, 0, 0 );
      /* If we can't get the attrib ID, return */
      break;
    }

    HDR_MSG_PROT_2(MSG_LEGACY_LOW, "Rcvd CReq/AUR for attrib 0x%x length 0x%x",
                   attrib_id,
                   (attrib_len_in_bits/8));

    /* Return the parsed values of attribute length and ID */
    *p_attrib_len = attrib_len_in_bits;
    *p_attrib_id = attrib_id;
    *p_attrib_id_size = attrib_id_size;
   
  } while ( 0 );

  return err;

} /* hdrscm_unpack_length_and_attrib_id */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_EXTRACT_ATTRIB_INFO                                LOCAL

DESCRIPTION
  This function is used to extract an instance of "hdrscm_accepted_attrib_type"
  from the dsm item.

DEPENDENCIES
  None.

PARAMETERS
  a_aip        - Address to which the accepted_attrib_type instance should be 
                 extracted to.
  item_ptr     - Pointer to DSM item containing the accepted attrib instances.
  attrib_index - Index of the attribute in the dsm item.
  
RETURN VALUE
  E_SUCCESS if we could extract out an instance from DSM item
  E_FAILURE Otherwise.

SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscm_extract_attrib_info
(
  hdrscm_accepted_attrib_type *a_aip,
  dsm_item_type *parsed_attribs_item_ptr,
  uint8 attrib_index
)
{
  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the hdrscm_accepted_attrib_type instance */
  memset(a_aip, 0,  sizeof(hdrscm_accepted_attrib_type));

  /* If the size of the instance extracted is not equal to size of accepted
     attribute type... */
  if (dsm_extract(parsed_attribs_item_ptr, 
                  (attrib_index * sizeof(hdrscm_accepted_attrib_type)),
                  a_aip, sizeof(hdrscm_accepted_attrib_type)) !=
      sizeof(hdrscm_accepted_attrib_type) )
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                   "Reading accepted attrib at index %d from DSM item failed",
                   attrib_index);

    err_no = E_FAILURE;
  }

  return err_no;

} /* hdrscm_extract_attrib_info */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_PROCESS_REQ_MSG

DESCRIPTION
  This function processes the ConfigurationRequest and AttributeUpdateRequest
  messages.
  
DEPENDENCIES
  Assumes that the config request message was received on a stream negotiated
  for a default packet apps bound to the SN.

ERROR CODES
The error codes have the following meaning in this function.
E_SUCCESS:      If there is no error. 
E_NO_DATA:      If there is insufficient bits to parse fields from an attribute 
                record. However, this means that the attribute length has been 
                unpacked successfully. Hence, the action involves jumping to 
                next attribute record by skipping the current one. This AR will 
                be rejected.
E_OUT_OF_RANGE: If the parsed value for an attribute or the parsed attribute ID
                is not supported. The action is to skip the current AR and 
                proceed to next AR. 
E_DATA_INVALID: If we are not able to unpack the length field from the CReq or
                if the length field of current AR is zero. In this case, we 
                can't proceed unpacking further without knowing the length 
                field. Just stop parsing further and accept the attributes 
                unpacked so far.
E_NOT_ALLOWED:  If the GAUPed attribute is not GAUPable. Drop AUR message and
                send a session close.

Please note that for all scenarios except E_NOT_ALLOWED, we send a CRsp back.

PARAMETERS
  pip       - Protocol's registered information pointer
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  is_gaup   - True if this is an AttributeUpdateRequest message
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscm_process_req_msg
(
  hdrscm_reg_type const *const pip,
  dsm_item_type *item_ptr,
  hdrhai_stream_enum_type stream_id,
  boolean is_gaup
)
{

  uint16 total_bits_remaining = 0; 
    /* Number of bits remaining in the message */

  uint8  trans_id;
    /* Transaction ID of the config req */

  uint32  attrib_len;
    /* Bit length of attribute, including the length field */

  uint16 attrib_offset = 0;
    /* Bit offset of current attribute */

  uint16 attrib_id;
    /* ID field of attribute */

  hdrscm_proposed_attrib_type input_params;
    /* Structure holding the input params for parsing the proposed attribute */

  uint8 attrib_count = 0;
    /* Number of attributes included in this message */

  hdrscm_accepted_attrib_type *a_aip; 
    /* Attribute information pointer of Current attribute being parsed */

  hdrhai_protocol_instance_enum_type protocol_instance; 
    /* in use/in config */

  dsm_item_type *send_msg_ptr = NULL; 
    /* dsm buffer for message sent from AT */

  uint8 attrib_index = 0;
    /* Index into the array of accepted attributes */

  hdrerrno_enum_type err = E_SUCCESS;
    /* error status */

  boolean gaup_is_accepted = TRUE;
    /* Is the entire GAUP (Attrib update Req) message accepted? */

  uint8 attrib_id_size;
    /* Size of the attribute ID in bits */

  uint16 st;
    /* Negotiated Subtype of the protocol which has received this config.
       request mesg */

  boolean is_simple;
    /* Is attribute simple? */

  boolean send_response = TRUE;
     /* Flag to indicate if AT needs response to ConfigReq message */

  static hdrscp_scp_fail_ind_type gaup_fail_data;
    /* Data for AN GAUP failed indication */

  hdrind_ind_data_union_type ind_data;
    /* Indication data */

  hdrscm_accepted_attrib_type gaup_failed_attrib;
    /* Info of attribute that caused the GAUP failure */

  dsm_item_type *parsed_attribs_item_ptr = NULL;
    /* Dynamic memory for accepted attributes */

  hdrscm_accepted_attrib_type current_attrib_being_parsed;
    /* Temp to store parsed attribute info before pushing into DSM item */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    The outer do-while is used to avoid returning from multiple places and to
    consolidate error handling to one place.
   ------------------------------------------------------------------------*/
  do
  {
    total_bits_remaining = 
      ( dsm_length_packet(item_ptr) * HDRSCM_BITS_PER_BYTE );

    /* TransactionID/SequenceNumber */
    if( ( err = HDRBIT_UNPACK8( item_ptr, &trans_id, pip->message_id_size,
                                pip->transaction_id_size ) ) != E_SUCCESS )
    {

      ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );

      /* Without the transaction ID, it makes no sense to send a response */
      /* Break out of do-while to error handler */
      break;
    }

    /* Get the subtype of the negotiated protocol */
    st = (is_gaup)? hdrscp_get_current_subtype(pip->protocol_name):
                    hdrscp_get_config_subtype(pip->protocol_name);

    /* Initialize attrib_len so the initial attrib_offset will be
       immediately after the transaction ID of the config request. */
    attrib_len = pip->message_id_size + pip->transaction_id_size;

    /* get new buffer */
    parsed_attribs_item_ptr = hdrutil_new_msg_buffer(); 

    /*---------------------------------------------------------------------
      Loop once for each attribute record (AR) included in the message
      While there are at least enough bits for the smallest attribute.
    ---------------------------------------------------------------------*/
    while ( (total_bits_remaining - attrib_len) >= HDRSCM_EMPTY_ATTRIB_LEN(pip) )
    {

      /* Initialize the Attribute Information structs */
      memset(&current_attrib_being_parsed, 
             0,  
             sizeof(hdrscm_accepted_attrib_type));

      /* Accepted attrib pointer points to next accepted attrib struct in DSM 
         item  */
      a_aip = &current_attrib_being_parsed;

      /* Move to the next attribute record */
      attrib_offset += attrib_len;
      total_bits_remaining -= attrib_len;

      /* Unpack the attrib length and attrib ID from the Config Req message */
      if ( (err = hdrscm_unpack_length_and_attrib_id( 
              pip, item_ptr, attrib_offset, total_bits_remaining, 
              &attrib_len, &attrib_id, &attrib_id_size, st)) == E_DATA_INVALID )
      {
        /*--------------------------------------------------------------------
          E_DATA_INVALID: 
          This means that we were not able to unpack even a valid length field 
          or the unpacked length field is zero. We stop unpacking furrther and 
          send CRsp for attributes unpacked so far.
        ---------------------------------------------------------------------*/
        ERR("Error extracting length from CReq Msg. Ignore Msg.", 0,0,0);
        break;
      }

      /*----------------------------------------------------------------------
        At this stage the error code could only be either one of the following

         1. E_NO_DATA: This means a valid length field was unpacked. However, 
            we are not able to unpack the attribute ID or we have in-sufficient
            bits for the value ID after that. In such a case, it is better to
            skip the attribute record rather than rejecting the whole message. 

         2. E_SUCCESS: Proceed unpacking further... 
      ---------------------------------------------------------------------*/
      if (err == E_NO_DATA)
      {
        ERR("Insufficient bits in AttribRecord(AR) of CReq Msg. Ignore AR",
            0, 0, 0 );
        a_aip->is_accepted = FALSE;
      }
      else if (pip->attrib_is_supported == NULL)
      {
        /* We are not supposed to get here!!! If attrib_is_supported is NULL,
           Just reject the attribute and print a message to detect */
        ERR("is_supported call back NULL prot: 0x%x, attrib: 0x%x. Reject AR",
            pip->protocol_name, a_aip->attrib_id, 0 );
        a_aip->is_accepted = FALSE;
      }
      else
      {
        /* We have unpacked an attribute ID. Remember it. */
        a_aip->attrib_id = attrib_id;

        /* If the attribute is currently supported... */
        if (pip->attrib_is_supported(attrib_id, st))
        {
          /* If the message was AttributeUpdateRequest message */
          if (is_gaup)
          {
            if (pip->is_attrib_gaupable != NULL)
            {
              if ((err = 
                     pip->is_attrib_gaupable(attrib_id, st)) == E_NOT_ALLOWED)
              {
                HDR_MSG_PROT_3(MSG_LEGACY_ERROR,
                               "Attrib %d, for P 0x%x, St %d not GAUPable",
                               attrib_id, pip->protocol_name, st);
                /* Break out of while loop */
                break;
              }
            }
            else
            {
              HDR_MSG_PROT_2(MSG_LEGACY_ERROR, 
                             "No GAUPable attribs for Protocol 0x%x, St %d",
                             pip->protocol_name, st);
              /* Break out of while loop */
              err = E_NOT_ALLOWED;
              break;
            }
          }

          /*-------------------------------------------------------------------
            Unpack the attribute and determine which value is acceptable
          -------------------------------------------------------------------*/

          /* Create the proposed attrib struct based on what we know of the 
             attrib */
          input_params.is_gaup            = is_gaup;
          input_params.attrib_id          = attrib_id;
          input_params.attrib_len         = attrib_len;
          input_params.attrib_offset      = attrib_offset;
          input_params.attrib_len_size    = pip->attrib_len_size;
          input_params.value_id_size      = pip->value_id_size;

          /* Find out if the attribute is simple */
          is_simple = pip->is_simple(attrib_id, st);

        /* If the current attribute is simple and if we have a registered 
           function pointer to get the simple attribute value size, get the 
           value ID size using the function or use the registered const */
          if ((pip->get_simple_attrib_size != NULL) && (is_simple))
          {
            a_aip->sattr_val_size = pip->get_simple_attrib_size(attrib_id, st);
          }
          else
          {
            a_aip->sattr_val_size = pip->value_id_size;
          }

          /* AttribID size is subtype-specific. Store it in accepted attribute 
             struct So it could be used by parsing routines and while packing 
             the rsp msg */
          a_aip->attrib_id_size = attrib_id_size;

          /* If the attribute is simple... */ 
          if ( is_simple )
          {
            /* SCM's generic simple attribute parsing routine is called! */
            err = hdrscm_parse_simple_attrib( 
                    item_ptr, pip, &input_params, a_aip, st );

            if (err != E_SUCCESS)
            {
              HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                               "Simple AR Rejected :( Parsing failed");
            }
          }
          else /* If the attribute is complex... */
          {
            /* Make sure we have a valid Complex attribute parsing routine 
               registered for this protocol */
            if (pip->parse_complex_attrib == NULL)
            {
              HDR_MSG_PROT_3(MSG_LEGACY_ERROR, 
                         "Err: Invalid attrib 0x%x for prot: 0x%x, sub: %d",
                         a_aip->attrib_id, pip->protocol_name, st);

              err = E_NOT_SUPPORTED;
            }
            else
            {
              /* The complex attribute parsing callback registered by target 
                 protocol in the Protocol Information Pointer(PIP) is called! */
              err = pip->parse_complex_attrib( 
                      item_ptr, &input_params, a_aip, st );

              if (err != E_SUCCESS)
              {
                HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                             "Complex AR Rejected :( Parsing failed");
              }

            } /* parse_complex_attrib != NULL */

          } /* attribute is not simple */

        } /* If attrib_is_supported */
        else
        {
          HDR_MSG_PROT_3( MSG_LEGACY_ERROR,
                          "Attr: 0x%x for P: 0x%x, S: 0x%x is not supported",
                          a_aip->attrib_id,
                          pip->protocol_name,
                          st );
          err = E_NOT_SUPPORTED;
        }

        /* If the parsed attribute had nothing but invalid values or if the
           proposed attribute is not supported or if the parsing failed, just 
           flag the attribute as not accepted and proceed to process the next 
           attribute */
        if (( err == E_OUT_OF_RANGE ) ||  /* Invalid value in AR */
            ( err == E_NOT_SUPPORTED ) || /* Attribute ID not supported */
            ( err == E_NO_DATA ))         /* Parse error due to insuff. bits */
        {
          /* If the parsed attribute is not valid, we just don't accept it and
             exclude it from the config response message */
          a_aip->is_accepted = FALSE;
        }

      } /* err != E_NO_DATA */

      (void) dsm_pushdown_tail(&parsed_attribs_item_ptr,
                               &current_attrib_being_parsed,
                               sizeof(hdrscm_accepted_attrib_type),
                               DSM_DS_SMALL_ITEM_POOL);

      /* Since we have successfully parsed one attribute, increment the counter
         This means we have one more attrib pushed into parsed_attribs DSM */
      attrib_count++;

    } /* while( there are more attributes in the config request ) */ 

    /*-----------------------------------------------------------------------
     At this stage error code could be one of the following. 
       1. E_SUCCESS      - No Error. Good to commit and send_response = TRUE
       2. E_OUT_OF_RANGE -  One of the Attribute was proposed with an invalid 
                           value. Reject that attribute alone. Send response
                           for rest of the attributes. send_response = TRUE. 
       3. E_NOT_SUPPORTED - One of the attribute was not supported by software.
                            Just discard that AR and send response for rest.
                            send_response = TRUE.
       4. E_NO_DATA      - The CReq had insufficient data for processing some 
                            of the attributes and parsing failed. At this stage
                            we have a valid attribute length. So, just skip
                            the current Attribute record and proceed to next.
                            Send CRsp for attributes that were unpacked 
                            successfully. send_response = TRUE.
       5. E_DATA_INVALID -  Unpacking attribute length failed for one of the
                            ARs in the CReq message. Abort unpacking further
                            and send response for attributes unpacked so far,
                            if the message is CReq. However, if the message 
                            is AUR, abort unpacking and send session close. 
                            send_response = TRUE, for CReq msg.
                            send_response = FALSE, for AUR msg.
       6. E_NOT_ALLOWED  -  The attribute is not GAUPable. Drop the AUR message
                            and send session close. send_response = FALSE.

       Please note that the error codes 1 thru 4 could be over-written by 
       subsequent processing of attribute records. The error scenarios 
       are preserved by a_aip->is_accepted flag. Setting this to FALSE make
       sure that they are skipped when packing the response message. However,
       error codes 5 & 6 abort the CReq/AUR processing in the above while loop 
       and hence are preserved when we get here.
     ----------------------------------------------------------------------*/


    /*---------------------------------------------------------------------
                         COMMIT THE PARSED ATTRIBS TO DB
    ---------------------------------------------------------------------*/

    if (is_gaup)
    {
      /*----------------------------------------------------------------------
        Commit to DB the attributes received thru AttributeUpdateRequest Msg. 
       ---------------------------------------------------------------------*/

      /* Initialize the struct gaup_failed_attrib */
      memset(&gaup_failed_attrib, 0, sizeof(hdrscm_accepted_attrib_type));

      if ((err == E_NOT_ALLOWED) || (err == E_DATA_INVALID))
      {
        /* AN GAUPed an attribute that was not GAUPable... 
           Do not accept GAUP. Send session close instead. */
        gaup_is_accepted = FALSE;
      }
      else
      {
        /*---------------------------------------------------------------------
          GAUP Update:
          The Attrib update Accept message contains only the message ID 
          and the Transaction ID. So the entire GAUP update message is either
          accepted or rejected. It is not possible to selectively accept part
          of attributes in a Attrib Update Request message. So we loop around
          attrib_count times and extract one parsed_attrib at a time from the 
          dsm item to make sure that all the attributes in the current message 
          were accepted.
        -----------------------------------------------------------------------*/
        for (attrib_index = 0; (attrib_index < attrib_count && gaup_is_accepted);
             ++attrib_index)
        {
          err = hdrscm_extract_attrib_info(&current_attrib_being_parsed, 
                                           parsed_attribs_item_ptr, 
                                           attrib_index);
          /* Break out of for-loop if we are not able to extract from dsm 
             item */
          if ( err != E_SUCCESS)
          {
            break;
          }

          gaup_is_accepted &= current_attrib_being_parsed.is_accepted;

          /* If the GAUP msg was not accepted, remember the attribute that was 
             rejected */
          if (!gaup_is_accepted)
          {
#ifdef FEATURE_MEMCPY_REMOVAL
            gaup_failed_attrib = current_attrib_being_parsed;
#else
            memcpy(&gaup_failed_attrib, 
                   &current_attrib_being_parsed, 
                   sizeof(hdrscm_accepted_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
          }

        } /* for */

        /* If we quit for-loop out of error, break out of while loop as well 
           and jump to error handler... */
        if (err != E_SUCCESS)
        {
          /* Break out of do-while */
          break;
        }
      }

      /* Since the entire GAUP Request message was accepted */
      if (gaup_is_accepted)
      {
        /* Loop through the array of parsed attributes and commit to the Db */
        for (attrib_index = 0; attrib_index < attrib_count; attrib_index++)
        {
          err = hdrscm_extract_attrib_info(&current_attrib_being_parsed, 
                                           parsed_attribs_item_ptr, 
                                           attrib_index);

          /* Break out of for-loop if we are not able to extract from dsm 
             item */
          if ( err != E_SUCCESS)
          {
            break;
          }

          /* Current accepted attrib pointer stored in a convenience ptr */
          a_aip = &current_attrib_being_parsed;

          HDR_MSG_PROT_4(MSG_LEGACY_LOW,
                         "Attrib 0x%x of prot 0x%x; ST: %d GAUPed, Val: 0x%x", 
                         a_aip->attrib_id, pip->protocol_name, st, 
                         a_aip->value_id);

          /* GAUP attributes accepted are now our current values */
          hdrscmdb_set_gauped_attrib( pip->protocol_name,
                                      a_aip->attrib_id, 
                                      a_aip->value_id,
                                      a_aip->complex_val_ptr,
                                      st);
        } /* for */

        /* If we quit for-loop out of error, break out of while loop as well 
           and jump to error handler... */
        if (err != E_SUCCESS)
        {
          /* Break out of do-while */
          break;
        }

        /* Give indication to target protocol about GAUP update */
        hdrind_give_ind(pip->an_gaup, NULL);

      }
      else
      {
        /* AT did not accept the AN's GAUP message, so ignore the message and
         * send session close */
        send_response = FALSE;

        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "GAUP Msg Trans Id: %d Dropped!", 
                       trans_id);

        /* AT could not accept the AttributeUpdateRequest message so send 
           session close as AT is not allowed to send AttributeUpdateReject */
        gaup_fail_data.reason = HDRHAI_CLOSE_AN_GAUP_FAIL;
        gaup_fail_data.type = pip->protocol_name;
        gaup_fail_data.subtype = hdrscp_get_current_subtype(pip->protocol_name);
        gaup_fail_data.attribute_id_length = 
                        gaup_failed_attrib.attrib_id_size/HDRSCM_BITS_PER_BYTE;
        gaup_fail_data.attribute_id = gaup_failed_attrib.attrib_id;
        ind_data.scp_fail_ptr = (void *)&gaup_fail_data;
        hdrind_give_ind(HDRIND_SCM_AN_GAUP_FAILED, (void*)&ind_data);

      }  /* !gaup_is_accepted */

    } /* is_gaup */
    else
    {
      /*----------------------------------------------------------------------
        Commit to DB the attributes received during session negotiation 
       ---------------------------------------------------------------------*/

      /* Loop through the array of parsed attributes and commit the accepted
       * ones to the Db. */
      for (attrib_index = 0; attrib_index < attrib_count; attrib_index++)
      {
        err = hdrscm_extract_attrib_info(&current_attrib_being_parsed, 
                                         parsed_attribs_item_ptr, 
                                         attrib_index);
        /* Break out of for-loop if we are not able to extract from dsm 
           item */
        if (err != E_SUCCESS)
        {
          break;
        }

        /* Current accepted attrib pointer stored in a convenience ptr */
        a_aip = &current_attrib_being_parsed;

        /* If the attribute was accepted */
        if (a_aip->is_accepted)
        {
          /* Config attributes accepted are now our negotiated values */
          hdrscmdb_set_inconfig_attrib( pip->protocol_name,
                                        a_aip->attrib_id, 
                                        a_aip->value_id,
                                        a_aip->complex_val_ptr,
                                        st ); 
        }
        else /* Attribute not accepted */
        {
          /* Attributes not accepted are set to the fall-back value */
          hdrscmdb_set_inconfig_attrib_to_inuse( pip->protocol_name,
                                                 a_aip->attrib_id,
                                                 st );
        }

      } /* for */

      /* If we quit for-loop out of error, break out of while loop as well 
         and jump to error handler... */
      if (err != E_SUCCESS)
      {
          /* Break out of do-while */
          break; 
      }

    } /* Not GAUP */

    if (send_response)
    {
      /*---------------------------------------------------------------------
                              BUILD THE RESPONSE MESSAGE
      ---------------------------------------------------------------------*/

      /* get new buffer */
      send_msg_ptr = hdrutil_new_msg_buffer();

      attrib_offset = 0;

      if (is_gaup)
      {
        /* If all the attribs in the GAUP message are accepted */
        if (gaup_is_accepted)
        {

          HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                         "GAUP Msg Trans Id: %d Accepted!", trans_id);

          /* Pack Accept message's Message ID */
          HDRBIT_PACK8( send_msg_ptr, HDRSCM_ATTRIB_UPDATE_ACPT_MSG, 
                        attrib_offset, pip->message_id_size );
        }
        else
        {
          HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                         "GAUP Msg Trans Id: %d Dropped!", trans_id);
          err = E_DATA_INVALID;

          /* Break out of do-while */
          break; 
        }

      } /* is_gaup */
      else
      {
        /* MessageID */
        HDRBIT_PACK8( send_msg_ptr, HDRSCM_CONFIG_RSP_MSG, 
                      attrib_offset, pip->message_id_size );
      }

      attrib_offset += pip->message_id_size;

      /* TransactionID */
      HDRBIT_PACK8( send_msg_ptr, trans_id,
                    attrib_offset, pip->transaction_id_size );

      attrib_offset += pip->transaction_id_size;

      if (!is_gaup)
      {
        for (attrib_index = 0; attrib_index < attrib_count; attrib_index++)
        {
          err = hdrscm_extract_attrib_info(&current_attrib_being_parsed, 
                                           parsed_attribs_item_ptr, 
                                           attrib_index);
          /* Break out of for-loop if we are not able to extract from dsm 
             item */
          if (err != E_SUCCESS)
          {
            break;
          }

          a_aip = &current_attrib_being_parsed;

          if (a_aip->is_accepted)
          {
            /* Length */
            HDRBIT_PACK8( send_msg_ptr, HDRSCM_CRSP_ATTRIB_LEN_BYTES(a_aip), 
                          attrib_offset, pip->attrib_len_size );
            attrib_offset += pip->attrib_len_size;

            /* AttributeID */
            HDRBIT_PACK16( send_msg_ptr, a_aip->attrib_id, 
                           attrib_offset, a_aip->attrib_id_size );
            attrib_offset += a_aip->attrib_id_size;

            /* Attribute Value/ValueID */
            HDRBIT_PACK32( send_msg_ptr, a_aip->value_id, 
                           attrib_offset, a_aip->sattr_val_size );
            attrib_offset += a_aip->sattr_val_size;
          }
        } /* end for attrib_index < attrib_count */

        /* If we quit for-loop out of error, break out of while loop as well 
           and jump to error handler... */
        if (err != E_SUCCESS)
        {
          break;
        }
      }

      protocol_instance = (is_gaup) ? HDRHAI_IN_USE : HDRHAI_IN_CONFIG;

      /* Send config response on RTC.  All config messages are reliable. */
      hdrhmp_app_send_msg( pip->protocol_name,
                           stream_id,
                           HDRHAI_RTC,
                           TRUE,
                           HDRSCM_CONFIG_RSP_MSG_PRI,
                           protocol_instance,
                           send_msg_ptr,
                           NULL,
                           NULL);

    } /* if (send_response) */

  } while ( 0 );
  
  /*----------------------------------------------------------------------
                              ERROR HANDLER
   ---------------------------------------------------------------------*/

  /* Free the parsed_attribs_item_ptr */
  if (parsed_attribs_item_ptr != NULL)
  {
    dsm_free_packet(&parsed_attribs_item_ptr);
  }

  return err;

} /* hdrscm_process_req_msg() */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_PROCESS_RSP_MSG

DESCRIPTION
  This function processes the ConfigurationResponse, AttributeUpdateAccept and
  AttribUpdateReject messages.
  
DEPENDENCIES
  Assumes that the config response message was received on a stream negotiated
  for a default packet apps bound to the SN.

PARAMETERS
  pip       - Protocol's registered information pointer
  item_ptr  - Pointer to the DSM item chain holding the message.
  stream_id - Stream the message was received on
  is_gaup   - True if this is an AttributeUpdateRequest message
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
static hdrerrno_enum_type hdrscm_process_rsp_msg
(
  hdrscm_reg_type const *const pip,
  dsm_item_type *item_ptr,
  uint8 msg_id,
  boolean is_gaup
)
{
  int total_bits_remaining = 0; 
    /* Number of bits remaining in the message */

  uint8  trans_id;
    /* Transaction ID of the config response message */

  uint32  attrib_len = 0;
    /* Bit length of attribute, including the length field */

  uint16 attrib_offset = 0;
    /* Bit offset of current attribute */

  uint16 attrib_id;
    /* ID field of attribute */

  uint16 value_id;
    /* Value ID field of the attribute */

  uint16 value_id_size = 0;
    /* Value ID Size */

  boolean is_simple;
    /* Is attribute simple? */

  hdrscm_accepted_attrib_type accepted_attribs;
    /* Store the accepted attributes locally until the entire message is 
       processed.  This ensures that the "negotiated" session information 
       stays consistent and accurate if a parsing error occurs. */

  hdrerrno_enum_type err = E_SUCCESS;
    /* error status */

  uint8 attrib_count = 0;
    /* Number of attributes included in this message */

  hdrscm_accepted_attrib_type *a_aip = NULL;
    /* Attribute information pointer of Current attribute being parsed */

  uint8 attrib_index = 0;
    /* Attribute Index in accepted_attribs array */

  hdrind_ind_data_union_type ind_data;
    /* Indication Data */

  uint8 attrib_id_size = 0;
    /* Attribute ID size */

  hdrscm_rsp_timer_type *timer_rec_ptr = NULL;
    /* Pointer to the timer record reserved for this transaction */

  uint16 st;
    /* Negotiated Subtype of the protocol which has received this config.
       response mesg */

  dsm_item_type *accepted_attribs_item_ptr = NULL;

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    The outer do-while is used to avoid returning from multiple places and to
    consolidate error handling to one place.
   ------------------------------------------------------------------------*/
  do
  {
    /* Compute the total number of bits in the DSM item */
    total_bits_remaining = (dsm_length_packet(item_ptr) * HDRSCM_BITS_PER_BYTE);

    /* Extract TransactionID/SequenceNumber */
    if( ( err = HDRBIT_UNPACK8( item_ptr, &trans_id, pip->message_id_size,
                                pip->transaction_id_size ) ) != E_SUCCESS )
    {

      ERR("Error extracting TransactionID from ConfigReq Msg", 0, 0, 0 );

      /* Without the transaction ID, the response doesn't make any sense */
      /* Break out of do-while to Error Handler */
      break;
    }

    /* Entering critical section... */
    rex_enter_crit_sect(&hdrscm.timer_crit_sect);

    err = hdrscm_find_rsvd_timer(pip->protocol_name, trans_id, &timer_rec_ptr);

    /* Did we find a timer record? */
    if ( err == E_SUCCESS )
    {
      /* We did find a timer record! Disable the timer */
      (void) rex_clr_timer(&timer_rec_ptr->rsp_timer);

      /* Return the timer to the pool */
      timer_rec_ptr->in_use = FALSE;

      /* Decrement the number of In-use timers */
      --hdrscm.num_in_use_timers;
    }

    /* Leave critical section... */
    rex_leave_crit_sect(&hdrscm.timer_crit_sect);

    if ( err == E_TIMER_EXP )
    {
      /* No Timer record found! The timer has probably expired before we got
         this response. Just drop the message. */
      HDR_MSG_PROT_1(MSG_LEGACY_ERROR, 
                     "No timer rec found. TransId: %d! Timer expired?.", 
                     trans_id);

      /* Break out of do-while to Error Handler */
      break; 
    }

    /* Store the transaction ID to Indication data */
    ind_data.gaup_trans_id = trans_id;

    /* Get the subtype of the negotiated protocol */
    st = (is_gaup)? hdrscp_get_current_subtype(pip->protocol_name):
                    hdrscp_get_config_subtype(pip->protocol_name);

    /* get new buffer */
    accepted_attribs_item_ptr = hdrutil_new_msg_buffer(); 

    /* If the Configuration message was a GAUP Accept/Reject */
    if (is_gaup)
    {
      switch(msg_id)
      {
        case HDRSCM_ATTRIB_UPDATE_ACPT_MSG:

          HDR_MSG_PROT_2( MSG_LEGACY_MED, "GAUP ACCEPT T:%d P:%d",
                          trans_id, pip->protocol_name );

          /* If we have a registered call back, Get the GAUPed attributes' values
             from the target protocol */
          if ( (pip->get_value_of_gauped_attributes != NULL) &&
               (err = pip->get_value_of_gauped_attributes(
                        trans_id, 
                        &accepted_attribs_item_ptr, 
                        &attrib_count, st)) == E_SUCCESS)
          {
            /* The GAUP AttribUpdateRequest message had attrib_count Attributes 
               that was just accepted. Commit one after another into the DB */
            for (attrib_index=0; attrib_index < attrib_count; ++attrib_index)
            {

              err = hdrscm_extract_attrib_info(&accepted_attribs, 
                                               accepted_attribs_item_ptr, 
                                               attrib_index);
              if (err != E_SUCCESS)
              {
                /* Breaks out of switch-case */
                break;
              }

              /* Current accepted attrib pointer stored in a convenience ptr */
              a_aip = &accepted_attribs;

              /* GAUPed attributes accepted are now our current values */
              hdrscmdb_set_gauped_attrib(pip->protocol_name,
                                         a_aip->attrib_id,
                                         a_aip->value_id,
                                         a_aip->complex_val_ptr,
                                         st);
            } /* For */

            /* Send accepted indication */
            hdrind_give_ind(pip->gaup_accepted, &ind_data);

          } /* get_value */

          break;

        case HDRSCM_ATTRIB_UPDATE_REJ_MSG:

          /* AttribUpdateRequest has been rejected by AN. Send Reject ind */
          hdrind_give_ind(pip->gaup_rejected, &ind_data);
          break;

      } /* switch */

      if (err != E_SUCCESS)
      {
        /* Break out of do-while to Error Handler */
        break; 
      }

      /* At this point the GAUP message sent out was either accepted or rejected.
         Turn off the is_gaup_pending flag */
      hdrscm.is_gaup_pending[pip->protocol_index] = FALSE;

    } /* GAUP */
    else
    /* else the Config message was a Configuration Response for AT-init 
       ConfigurationRequest message during initial session negotiation */
    {
      /* Initialize attrib_len so the initial attrib_offset will be
       * immediately after the transaction ID of the config request. */
      attrib_len = pip->message_id_size + pip->transaction_id_size;

      /*---------------------------------------------------------------------
        Loop once for each attribute included in the message, While the parsing
        of the previous attribute hadn't failed and there are at least enough 
        bits for the smallest attribute.
      ---------------------------------------------------------------------*/
      while ((err == E_SUCCESS) && 
             (total_bits_remaining - attrib_len) >= HDRSCM_EMPTY_ATTRIB_LEN(pip))
      {
        /* Move to the next attribute record */
        attrib_offset += attrib_len;
        total_bits_remaining -= attrib_len;

        /* Unpack the attrib length and attrib ID from the ConfigRsp message */
        if ( (err = hdrscm_unpack_length_and_attrib_id(
                      pip, item_ptr, attrib_offset, total_bits_remaining, 
                      &attrib_len, &attrib_id, &attrib_id_size, st)) != E_SUCCESS)
        {
          ERR("Error extracting length/AttribID from ConfigRsp Msg. Ignore", 
              0, 0, 0);
          break;
        }

        /* Find out if the attribute is simple */
        is_simple = pip->is_simple(attrib_id, st);

        /* If the current attribute is simple and if we have a registered 
           function pointer to get the simple attribute value size, get the 
           value ID size using the function or use the registered const */
        if ((pip->get_simple_attrib_size != NULL) && (is_simple))
        {
          value_id_size = pip->get_simple_attrib_size(attrib_id, st);
        }
        else
        {
          value_id_size = pip->value_id_size;
        }

        /* Unpack the Value ID */
        if ( ( err = HDRBIT_UNPACK16( 
                      item_ptr, 
                      &value_id, 
                      (attrib_offset + pip->attrib_len_size + attrib_id_size),
                      value_id_size ) ) != E_SUCCESS )
        {
          ERR("Error extracting Value ID from ConfigReq Msg. Ignore", 0, 0, 0 );

          /* If we can't get the value ID, drop the message */
          break;
        }

        a_aip = &accepted_attribs;

        a_aip->attrib_id = attrib_id;
        a_aip->is_accepted = FALSE;
        a_aip->value_id = 0xFF;
        a_aip->complex_val_ptr = NULL;

        if (pip->get_proposed_attrib_value == NULL)
        {
            if (pip->is_simple(attrib_id, st)) 
            {
              /* in leagacy code, if AT propose simple attrib, it might not
                 include a get_proposed_attrib_value() call back, in this case,
                 treat the value as accepted. */
              err = E_SUCCESS;
            }
            else
            {
              err = E_FAILURE;
            }
        }
        else
        {
          /* retrieve proposed attrib value */
          err = pip->get_proposed_attrib_value(
              trans_id, a_aip, st);

          /* if proposed does not match AN's response, close the session */
          if ((err == E_SUCCESS) &&
              (a_aip->value_id != value_id))
          {
            HDR_MSG_PROT_6(MSG_LEGACY_HIGH, 
                "ConfigRsp ValueID mismatch P:%x/%x T:%x A:%x AT:%x AN:%x",
              pip->protocol_name, st, trans_id, attrib_id, a_aip->value_id,
              value_id);
            
            err = E_NOT_ALLOWED;
          }
        }

        switch(err)
        {
          case E_SUCCESS:
          /* response is valid, will update scmdb later */
          a_aip->is_accepted = TRUE;
          (void) dsm_pushdown_tail(&accepted_attribs_item_ptr, &accepted_attribs, 
              sizeof(hdrscm_accepted_attrib_type), DSM_DS_SMALL_ITEM_POOL);

          /* Since we have successfully parsed one attribute, increment the counter. 
             This means we have one more valid attrib in the accepted_attribs array */
          attrib_count++;
          break;

          case E_NOT_ALLOWED:
          /* trans_id, attrib_id matched, valid_id doesn't match for complex
             attrib, or value invalid for simple attrib. Whenever a 
             get_prosed_attrib_value() returns E_NOT_ALLOWED, AT fails with a 
             SessionClose */
          ASSERT(pip->fail_neg != NULL);
          pip->fail_neg(HDRHAI_CLOSE_PROT_NEG_FAIL);
          break;

          case E_FAILURE:
          default:
          /* some error occured, but not important enough to send a 
             SessionClose. Set err back to E_SUCCESS to continue
             processing next attrib record */
          err = E_SUCCESS;
          break;
        }
      } /* while */

      /* If the entire ConfigurationResponse message was valid */
      if (err == E_SUCCESS)
      {
        /* The config response message had attrib_count valid Attribute records.
           Commit one after another into the database */
        for (attrib_index=0; attrib_index < attrib_count; ++attrib_index)
        {
          err = hdrscm_extract_attrib_info(&accepted_attribs, 
                                           accepted_attribs_item_ptr, 
                                           attrib_index);

          if (err != E_SUCCESS)
          {
            /* Break out of for-loop */
            break;
          }

          /* Current accepted attrib pointer stored in a convenience ptr */
          a_aip = &accepted_attribs;

          /* Config attributes accepted are now our negotiated values */
          hdrscmdb_set_inconfig_attrib( pip->protocol_name,
                                        a_aip->attrib_id,
                                        a_aip->value_id,
                                        a_aip->complex_val_ptr,
                                        st ); 
        } /* For */

        if (err != E_SUCCESS)
        {
          /* Break out of do-while to Error Handler */
          break;
        }

        /* Give Config Complete indication to target protocol */
        hdrind_give_ind(pip->config_complete, NULL);

      } /* err == E_SUCCESS */

    } /* ConfigurationResponse Message */
    
  } while ( 0 );

  /*----------------------------------------------------------------------
                              ERROR HANDLER
   ---------------------------------------------------------------------*/
  if (accepted_attribs_item_ptr != NULL)
  {
    dsm_free_packet(&accepted_attribs_item_ptr);
  }

  return err;

} /* hdrscm_process_rsp_msg */ 

 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_DUMP_RSP_TIMER_HASH_TABLE                           LOCAL

DESCRIPTION
  Dump the response timer hash table on error for debugging purposes.
   
DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
=============================================================================*/
static void hdrscm_dump_rsp_timer_hash_table ( void )
{

  uint8 ind = 0;
    /* Response timer index in the hash table */

  rex_timer_cnt_type time_left = 0;
    /* Time left for timeout */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* for each response timer in the hash table... */
  for (ind = 0; ind < HDRSCM_MAX_NUM_CONCURRENT_GAUPS; ++ind)
  {
    /* Find the time left with the current response timer instance */
    time_left = rex_get_timer(&hdrscm.rt_hash_table[ind].rsp_timer);

    /* Dump the info for current timer instance */
    HDR_MSG_PROT_6(MSG_LEGACY_ERROR, 
                   "I %d; U: %d; G: %d; P: 0x%x; Tr: %d; Tm: %d",
                   ind,
                   hdrscm.rt_hash_table[ind].in_use,
                   hdrscm.rt_hash_table[ind].is_gaup,
                   hdrscm.rt_hash_table[ind].prot_name,
                   hdrscm.rt_hash_table[ind].trans_id,
                   time_left);
  }

} /*  hdrscm_dump_rsp_timer_hash_table */


 /* EJECT */
/*============================================================================

                        EXTERNAL FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

FUNCTION HDRSCM_RESERVE_RSP_TIMER

DESCRIPTION
  This function is used to reserve a response timer instance for either AT-init
  GAUP or Initial AT-initiated Session configuration. The reserved rsp timer 
  resource is released and returned to queue when,
    1. When the timer expires and
    2. When a response is received back before timer expiry.

  This routine needs to be re-entrant. Since it could be invoked from several 
  different task contexts interested in sending GAUP update request or Config
  requests. It has been made re-entrant by protecting the updates to global 
  variable using a critical section variable.

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Name of the protocol requesting reservation.
  p_timer_id    - Constant Pointer to reserved timer ID.
  trans_id      - Transaction ID of the message about to be sent.

RETURN VALUE
  E_SUCCESS - If a timer was available for use.
  E_NOT_AVAILABLE - If No free timers are available.

SIDE EFFECTS
  There are only finite number of timers available in the pool. Appropriate 
  corrections to this finite count needs to be made before trying to add new
  protocols to use timers from this pool. Gives a Fatal error if all the timers
  are already in use.

===========================================================================*/
hdrerrno_enum_type hdrscm_reserve_rsp_timer
(
  hdrhai_protocol_name_enum_type const protocol_name,
  uint8 *const p_timer_id,
  uint8 const trans_id,
  boolean is_gaup
)
{

  uint8 timer;
    /* Response Timer hash table iterator */

  hdrerrno_enum_type err_no = E_SUCCESS;
    /* Error Code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Entering critical section... */
  rex_enter_crit_sect(&hdrscm.timer_crit_sect);

  /* Check if there are any timer instance available. */
  if (hdrscm.num_in_use_timers < HDRSCM_MAX_NUM_CONCURRENT_GAUPS)
  /* At least one Timer instance available for use */
  {
    /* If the current timer instance is in use, Linear probe to find the next 
       available instance. Note that the loop runs as many times as the number
       of Response timer instances in the hash table. */
    for (timer = 0; (hdrscm.rt_hash_table[hdrscm.rt_hash].in_use == TRUE) &&
           (timer < HDRSCM_MAX_NUM_CONCURRENT_GAUPS); ++timer)
    {
      /* Move the running index to next timer */
      ++hdrscm.rt_hash;

      /* Wrap around the end of table, if we need to... */
      hdrscm.rt_hash %= HDRSCM_MAX_NUM_CONCURRENT_GAUPS;
    }

    /* Did we find a free timer? */
    if (hdrscm.rt_hash_table[hdrscm.rt_hash].in_use == FALSE)
    {
      HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                     "Reserve Rsp timer %d, %d timers in use",
                      hdrscm.rt_hash, hdrscm.num_in_use_timers );

      /* Reserve the current instance for the requesting protocol */
      hdrscm.rt_hash_table[hdrscm.rt_hash].prot_name = protocol_name;

      /* Save the transaction ID for future use */
      hdrscm.rt_hash_table[hdrscm.rt_hash].trans_id = trans_id;

      /* Mark this timer as being used! */
      hdrscm.rt_hash_table[hdrscm.rt_hash].in_use = TRUE;

      /* Store the is_gaup flag in the timer struct */
      hdrscm.rt_hash_table[hdrscm.rt_hash].is_gaup = is_gaup;

      /* Return the timer ID to requesting protocol */
      *p_timer_id = hdrscm.rt_hash;
    }
    else
    /* If we get here, that means that the num_in_use_timers is less than
       max and we still couldn't find a free timer through linear probing.
       Something has gotten out-of-sync. Theoritically, this should never 
       happen. Just print a fatal error to detect this abnormal behavior */
    {
      HDR_MSG_PROT_2( MSG_LEGACY_ERROR,
                      "No free Rsp timer. (num_in_use_timers: %d < Max: %d)!",
                      hdrscm.num_in_use_timers, HDRSCM_MAX_NUM_CONCURRENT_GAUPS );

      hdrscm_dump_rsp_timer_hash_table();
      return E_NOT_AVAILABLE;
    }

    /* Move the running index to next timer. */
    ++hdrscm.rt_hash;

    /* Ensure that the running index wraps around if it goes beyond the table 
       size */
    hdrscm.rt_hash %= HDRSCM_MAX_NUM_CONCURRENT_GAUPS;
    
    /* Increase the number of in-use timers by one */
    ++hdrscm.num_in_use_timers;
  }
  else
  {
    /* All the response timers in the hash table are in use */
    ERR("No Free Rsp timer. Num-in-use: %d", hdrscm.num_in_use_timers, 0, 0); 
    hdrscm_dump_rsp_timer_hash_table();
    err_no = E_NOT_AVAILABLE;
  }

  /* Leave critical section... */
  rex_leave_crit_sect(&hdrscm.timer_crit_sect);

  return err_no;

} /* hdrscm_reserve_rsp_timer */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_RSP_TIMER_CB

DESCRIPTION
  This function is the callback for all hdrscm protocol timers. This function
  is called by REX when the timer expires, and runs in the context of the task
  that defined the timer (HDRMC), however it runs as an independent thread and
  thus must queue an event for HDRMC.

DEPENDENCIES
  None

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_rsp_timer_cb
(
  uint32 timer_id
)
{

  hdrmc_queue_timer(HDRHAI_SESSION_CONFIG_MANAGER, timer_id);

} /* hdrscm_rsp_timer_cb */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for the Session 
  Configuration Manager.
    
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_process_timer
(
  uint32 timer_id
)
{

  hdrscm_reg_type const *pip;
    /* Protocol Information pointer */

  hdrind_ind_data_union_type ind_data;
    /* Indication Data */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Entering critical section... */
  rex_enter_crit_sect(&hdrscm.timer_crit_sect);

  /*---------------------------------------------------------------------------
    We process the queued timer expiry here. If the reponse message arrived
    between timer expiry and now, the process_msg routine would have switched
    the in_use flag to FALSE and processed the message. We do not want to fail
    the GAUP/Negotiation after rsp mesg reception. So we just fail this only 
    when the timer's in_use flag was set. Otherwise, we just ignore this timer 
    expiry.
  ---------------------------------------------------------------------------*/

  /* Give GAUP failed, only if the timer was in use, */
  if (hdrscm.rt_hash_table[timer_id].in_use == TRUE)
  {
    /* Find the protocol information pointer */
    pip = hdrscm_find_reg_entry(hdrscm.rt_hash_table[timer_id].prot_name);

    if (pip == NULL)
    {
      ERR_FATAL("Unable to find pip for protocol- %d",
                hdrscm.rt_hash_table[timer_id].prot_name,0,0);
    }

    /* Give GAUP failed indication to XYZ protocol */
    ind_data.gaup_trans_id = hdrscm.rt_hash_table[timer_id].trans_id;

    if (hdrscm.rt_hash_table[timer_id].is_gaup)
    {
      /* AN hasn't responded to the AT's AttributeUpdateRequest msg within the
         turnaround time allowed.  We fail this GAUP update. Give an indication
         to target protocol reporting timeout */
      hdrind_give_ind(pip->gaup_failed, &ind_data);

      /* Turn off the GAUP Pending flag */
      hdrscm.is_gaup_pending[pip->protocol_index] = FALSE;
    }
    else
    {
      ASSERT(pip->fail_neg != NULL);

      /* Fail the Current Config negotiation since we timed out waiting for a 
         response */
      pip->fail_neg(HDRHAI_CLOSE_CFG_RSP_EXP);
    }

    /* Mark the timer as available */
    hdrscm.rt_hash_table[timer_id].in_use = FALSE;

    /* Decrement the number of InUse timers */
    --hdrscm.num_in_use_timers;
  }

  /* Leave critical section... */
  rex_leave_crit_sect(&hdrscm.timer_crit_sect);

} /* hdrscm_process_timer */


 /* EJECT */
/*=============================================================================
FUNCTION HDRSCM_POWERUP_INIT

DESCRIPTION
  This function should be called once upon system startup.  The protocol 
  configuration is initialized. 

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  Returns a pointer to the current config data (always valid)

SIDE EFFECTS
  None.
=============================================================================*/
void hdrscm_powerup_init( void )
{

  uint32 timer_id = 0;
    /* Timer ID used as an array index */

  uint32 protocol_index = 0;
    /* Registered protocol index */

/*- - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmc_is_first_init() )
  {
    /* Initialize the timer management critical section variable */
    rex_init_crit_sect(&hdrscm.timer_crit_sect);

    /* Define HDRSCM_MAX_NUM_CONCURRENT_GAUPS timers for later use */
    for (timer_id=0; timer_id < HDRSCM_MAX_NUM_CONCURRENT_GAUPS; ++timer_id)
    {
      /* Initialize Response timer identified by 'timer_id' */
      rex_def_timer_ex(&(hdrscm.rt_hash_table[timer_id].rsp_timer),
                       hdrscm_rsp_timer_cb,
                       timer_id);

      /* None of the timers are in use at powerup */
      hdrscm.rt_hash_table[timer_id].in_use = FALSE;
    }
  }

  /* Set the number of timer in use to zero to indicate that the queue is 
     empty */
  hdrscm.num_in_use_timers = 0;

  /* Set the running index to zero */
  hdrscm.rt_hash = 0;

  hdrscmrmac0_read_vt_rp_from_nv();

#ifdef FEATURE_HDR_REVB_DTX_MODE
  hdrscmphy_set_net_dtx_mode( 
     hdrmc_feature_is_enabled( HDRMC_FEATURE_HDR_NETWORK_DTX ));   
#endif /* FEATURE_HDR_REVB_DTX_MODE */

  /* Initialize all GAUP Pending flags to false */
  for (protocol_index = 0; protocol_index < HDRSCM_MAX_REG_PROTOCOLS; 
        ++protocol_index)
  {
    hdrscm.is_gaup_pending[protocol_index] = FALSE;
  }

} /* hdrscm_powerup_init() */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-config instance. All Config 
  messages are processed in the HDRMC task. 

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_config_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the PAC. Messages for the In Config instance
  should run in the HDRMC task
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_SESSION_CONFIG_MANAGER, 
                   HDRHAI_IN_CONFIG, msg_ptr );

} /* hdrscm_config_msg_cb() */


 /* EJECT */
/*===========================================================================
FUNCTION HDRSCM_IN_USE_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAPP1, HDRAPP2, HDRAPP3 in-config instance. All Inuse Config 
  messages are processed in the HDRMC task. 

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_in_use_config_msg_cb
(
  dsm_item_type *msg_ptr
)
{
  /*-----------------------------------------------------------------------
  Queue the message & tag it for the PAC. Messages for the In Config instance
  should run in the HDRMC task
  -----------------------------------------------------------------------*/
  hdrmc_queue_msg( HDRHAI_SESSION_CONFIG_MANAGER, 
                   HDRHAI_IN_USE, msg_ptr );

} /* hdrscm_config_msg_cb() */

 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_PROCESS_MSG

DESCRIPTION
  This function processes messages for SCM.

DEPENDENCIES
  None

PARAMETERS
  protocol_name - Name of the protocol the message is destined to.
  msg_ptr   - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrscm_process_msg
(
  dsm_item_type   *msg_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;      
    /* Message ID */

  uint8 protocol_id; 
    /* Protocol ID (stream the message was received on)*/

  hdrhai_stream_enum_type stream_id; 
    /* stream the message was rxed on */

  const hdrscm_reg_type *pip = NULL;  
    /* Protocol Information pointer */

  hdrerrno_enum_type err = E_SUCCESS;
    /* Error code */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  HDR_MSG_PROT(MSG_LEGACY_HIGH, "hdrscm_process_msg");

  /* Ensure the msg_ptr is valid.  If not, log an error and just return,
   * since it is already NULL. */
  if (msg_ptr == NULL)
  {
    ERR("NULL msg_ptr passed to hdrscm_process_msg()",0,0,0);
    return;
  }
  
  /*---------------------------------------------------------------------------
  Get the protocol id (stream) associated with the message.   
  ---------------------------------------------------------------------------*/
  protocol_id = (uint8)HDRDSM_GET_PROT_ID(msg_ptr);

  switch( protocol_id )
  {
    case HDRHAI_N_APP1_TYPE:
      stream_id = HDRHAI_STREAM1;
      break;

    case HDRHAI_N_APP2_TYPE:
      stream_id = HDRHAI_STREAM2;
      break;

    case HDRHAI_N_APP3_TYPE:
      stream_id = HDRHAI_STREAM3;
      break;

    default:
      stream_id = HDRHAI_STREAM0;
      break;
  }

  if (stream_id != HDRHAI_STREAM0)
  {
    if ( hdrstream_get_apptype_for_neg_stream( stream_id ) == 
         HDRHAI_3G1X_CSN_APP )
    {
      /* Set the internal protocol ID for message processing */
      protocol_id = (uint8) HDRHAI_CIRCUIT_SERVICES_NOTIF_PROTOCOL;

      HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                     "CSNA: process msg, bound w/ stream %d", stream_id);
    }
  } 

  /* Get the protocol information pointer by indexing into the table and If 
   * the protocol is not registered with SCM discard message */
  if ( ( pip = hdrscm_find_reg_entry( 
               (hdrhai_protocol_name_enum_type) protocol_id ) ) == NULL )
  {
    ERR("Protocol not registered with SCM", 0, 0, 0 );
    dsm_free_packet( &msg_ptr );
    return;
    }
  
  /* Extract the Message ID */
  if (HDRBIT_UNPACK8( msg_ptr, &msg_id, 0, pip->message_id_size) != E_SUCCESS)
  {
    ERR("Error Unpacking Msg ID", 0, 0,0 );
    dsm_free_packet( &msg_ptr );
    return;
  }

  /*---------------------------------------------------------------------------
                            IN CONFIG INSTANCE   
  ---------------------------------------------------------------------------*/
  if (protocol_instance == HDRHAI_IN_CONFIG)
  {
    HDRTRACE_MSG( protocol_id, msg_id, 0 );

    switch (msg_id)
    {
      /* Config Request Message */
      case HDRSCM_CONFIG_REQ_MSG:
        err = hdrscm_process_req_msg( pip, msg_ptr, stream_id, FALSE );
        break;

      /* Config Response Message */
      case HDRSCM_CONFIG_RSP_MSG:
        err = hdrscm_process_rsp_msg(pip, msg_ptr,msg_id, FALSE);
        break;

      default:
        ERR("Unknown InConfig message (0x%x)",msg_id,0,0);
        break;
    }

    /* If the message processing had failed because of Parse error... */
    if (err == E_NO_DATA)
    {
      ERR("Invalid InConfig mesg",0,0,0);
    }
  }

  /*---------------------------------------------------------------------------
                      IN USE INSTANCE - GAUP MESSAGES
  ---------------------------------------------------------------------------*/
  else if (protocol_instance == HDRHAI_IN_USE)
  {
    HDRTRACE_MSG( protocol_id, msg_id, 0 );

    switch (msg_id)
    {
      /* GAUP request message */
      case HDRSCM_ATTRIB_UPDATE_REQ_MSG:
        err = hdrscm_process_req_msg( pip, msg_ptr, stream_id, TRUE );
        break;

      /* Response to GAUP messages */
      case HDRSCM_ATTRIB_UPDATE_ACPT_MSG:
      case HDRSCM_ATTRIB_UPDATE_REJ_MSG:
        err = hdrscm_process_rsp_msg(pip, msg_ptr, msg_id, TRUE);
        break;

      default:
        ERR("Unknown InUse message (0x%x)",msg_id,0,0);
        break;
    }

    /* If the message processing had failed because of Parse error... */
    if (err == E_NO_DATA)
    {
      ERR("Invalid InUse mesg",0,0,0);
    }

  } /* end - protocol_instance */
  
  /* Free the DSM item after it has been processed */ 
  dsm_free_packet(&msg_ptr);

} /* hdrscm_process_msg() */




 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  configuration message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrscm_tx_outcome_cb
(
  void                 *data_ptr,
  hdrerrno_enum_type    err_val
)
{

  uint8 timer_id = (uint8) ((dword)data_ptr);
    /* Timer ID */

  hdrscm_reg_type const *pip;
    /* Protocol Information pointer */

  hdrind_ind_data_union_type ind_data;
    /* Indication Data */

  boolean is_gaup = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the protocol information pointer */
  pip = hdrscm_find_reg_entry(hdrscm.rt_hash_table[timer_id].prot_name);

  if (pip == NULL)
  {
    HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "Callback for unknown protocol %d", 
                   hdrscm.rt_hash_table[timer_id].prot_name);
    return;
  }

  /* Entering critical section... */
  rex_enter_crit_sect(&hdrscm.timer_crit_sect);
  if (!hdrscm.rt_hash_table[timer_id].in_use)
  {
    /* already received a GAUPResp before SLPAcked */
    if (rex_get_timer(&hdrscm.rt_hash_table[timer_id].rsp_timer))
    {
      ERR_FATAL("Rsp timer supposedly free was running!", 0, 0, 0);
    }
    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, 
                 "Resp for ConReq/GAUP Timer=%d already recv. ignore SLPAck",
                 timer_id);

    /* Leave critical section... */
    rex_leave_crit_sect(&hdrscm.timer_crit_sect);
    return;
  }

  switch(err_val)
  {
    case E_SUCCESS:
      /* Message transmitted successfully */
      if (hdrscm.rt_hash_table[timer_id].is_gaup)
      {
        HDRTRACE_TXMSG_ACKED(HDRHAI_SESSION_CONFIG_MANAGER,
                             HDRSCM_ATTRIB_UPDATE_REQ_MSG, 0);  /*lint !e506 */
      }
      else
      {
        HDRTRACE_TXMSG_ACKED(HDRHAI_SESSION_CONFIG_MANAGER,
                             HDRSCM_CONFIG_REQ_MSG, 0);  /*lint !e506 */
      }

      /*------------------------------------------------------------------------
                    START THE RESPONSE TIMER FOR THIS REQUEST 
      ------------------------------------------------------------------------*/
      if (rex_get_timer(&hdrscm.rt_hash_table[timer_id].rsp_timer))
      {
        /* We should never get here. If we did, it must be because of bug in timer
           Management algo. Err fatal to detect! */
        ERR_FATAL("Rsp timer supposedly free was running!", 0, 0, 0);
      }
      else
      {
        /* Start the timer */
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "RX SLPAck, start 2s ConReq/GAUP Resp Timer=%d",
                        timer_id );
        (void) rex_set_timer( &hdrscm.rt_hash_table[timer_id].rsp_timer, 
                              HDRSCM_RESPONSE_TIME_MS );
  
        /* If the message successfully sent out was a GAUP message, Mark the
           is GAUP Pending flag for this protocol */
        if (hdrscm.rt_hash_table[timer_id].is_gaup)
        {
          hdrscm.is_gaup_pending[pip->protocol_index] = TRUE;
        }
      }

      /* Leave critical section... */
      rex_leave_crit_sect(&hdrscm.timer_crit_sect);

      break;

    case E_FAILURE:
    case E_NOT_AVAILABLE: /* Returned when Config is locked */
    case E_AGAIN: 
    case E_CANCELED:
    case E_NOT_ALLOWED:
    case E_PROT_Q_FULL:   /* No buffer available in HMP/SLP Queue */

      ind_data.gaup_trans_id = hdrscm.rt_hash_table[timer_id].trans_id;

      /* Return the timer to the pool */
      hdrscm.rt_hash_table[timer_id].in_use = FALSE;

      /* Decrement the number of In-use timers */
      --hdrscm.num_in_use_timers;

      is_gaup = hdrscm.rt_hash_table[timer_id].is_gaup;

      /* Leave critical section... */
      rex_leave_crit_sect(&hdrscm.timer_crit_sect);

      if ( is_gaup )
      {
        HDRTRACE_TXMSG_FAILED(HDRHAI_SESSION_CONFIG_MANAGER,
                              HDRSCM_ATTRIB_UPDATE_REQ_MSG, 0); /*lint !e506 */

        /* Make sure we have a valid indication registered */
        ASSERT(pip->gaup_failed != HDRSCM_IND_NOT_APPLICABLE);

        /* Give an indication to target protocol reporting transmit failure */
        hdrind_give_ind(pip->gaup_failed, &ind_data); 
      }
      else
      {
        HDRTRACE_TXMSG_FAILED(HDRHAI_SESSION_CONFIG_MANAGER,
                              HDRSCM_CONFIG_REQ_MSG, 0); /*lint !e506 */

        if (err_val == E_FAILURE)
        {
          ASSERT(pip->fail_neg != NULL);

          /* fail the current config Negotiation */
          pip->fail_neg(HDRHAI_CLOSE_CFG_MSG_FAIL);
        }
        else
        {
          /* SCP handles other failure cases */
        }
      }

      break;

    default:
      /* Leave critical section... */
      rex_leave_crit_sect(&hdrscm.timer_crit_sect);
      ERR( "Unknown err val for Config outcome %d",
            err_val, 0, 0);
      break;
  }
} /* hdrscm_gaup_req_tx_outcome_cb */


 /* EJECT */
/*===========================================================================

FUNCTION HDRSCM_IS_GAUP_PENDING

DESCRIPTION
  AT is not allowed to send an AttributeUpdateRequest message for an attribute 
  while a response for the GAUP of same attribute is already pending. This 
  function is used to find out if a GAUP update sent by this protocol is 
  already in the pipeline.

  Note: This API doesn't check for the attribute ID. It only checks if the 
  protocol has a pending GAUP request. Most protocols have AT-init GAUP support
  for atmost one attribute. Because of this fact, it makes sense to use this 
  API to find if a GAUP update is pending.

DEPENDENCIES
  None

PARAMETERS
  protocol - protocol name

RETURN VALUE
  TRUE  - If the protocol has a GAUP request pending
  FALSE - Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrscm_is_gaup_pending
(
  hdrhai_protocol_name_enum_type protocol
)
{
  hdrscm_reg_type const *pip; 
    /* Protocol Information pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Get Pip from registration table */
  pip = hdrscm_find_reg_entry(protocol);

  if (pip == NULL)
  {
    ERR_FATAL("Unknown protocol name- %d in hdrscm_is_gaup_pending()", 
               protocol,0,0);
  }

  return hdrscm.is_gaup_pending[pip->protocol_index];

} /* hdrscm_is_gaup_pending */

/*===========================================================================

FUNCTION HDRSCM_CLEAR_ALL_RSP_TIMERS

DESCRIPTION
  If a SessionClose is received then all of the previously set ConfigResponse
  timers should be cleared to prevent them from expiring in the next opened
  session.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  All ConfigResponse timers will be cleared.

===========================================================================*/
void hdrscm_clear_all_rsp_timers( void )
{
  /* ConfigResponse timer loop counter */
  int rt_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                "Clear all ConReq/GAUP Resp Timers" );

  /* Reset the number of in-use response timers to zero. */
  hdrscm.num_in_use_timers = 0;

  /* Loop through the timer table and clear all timers */
  for( rt_index = 0; 
       rt_index < HDRSCM_MAX_NUM_CONCURRENT_GAUPS; 
       rt_index++ )
  {
    /* Clear the timer at rt_index */
    (void) rex_clr_timer( &(hdrscm.rt_hash_table[rt_index].rsp_timer) );

    /* Set the timer in-use flag to FALSE */
    hdrscm.rt_hash_table[rt_index].in_use = FALSE;
  }

} /* hdrscm_clear_rsp_timers() */


/*===========================================================================
FUNCTION HDRSCM_CONFIG_IS_NEEDED                                 EXTERNAL

DESCRIPTION
  This function is used to find out if an AT-initiated Configuration is 
  needed by a Prtocol Subtype

DEPENDENCIES
  None.

PARAMETERS
  hdrcom_hai_protocol_name_enum_type - protocol

RETURN VALUE
  TRUE - If AT-init config is required.
  FALSE - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrscm_config_is_needed( hdrcom_hai_protocol_name_enum_type protocol )
{
  boolean config_is_needed = TRUE;
    /* AT-init config needed for Protocol? */

  uint16 inuse_st, inconfig_st;
    /* prot Subtype */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First check if the InConfig subtype is the same as the InUse one */
  inuse_st = hdrscp_get_current_subtype(protocol);
  inconfig_st = hdrscp_get_config_subtype(protocol); 

  if (inuse_st == inconfig_st)
  {
    /* 
     * If prot InUse subtype is the same as InConfig subtype, don't need
     * to initiate AT-init configuration; otherwise, we need to propose
     * MaxTransmitPower attribute.
     */
    config_is_needed = FALSE;
  }

  return config_is_needed;

} /* hdrscm_config_is_needed */
