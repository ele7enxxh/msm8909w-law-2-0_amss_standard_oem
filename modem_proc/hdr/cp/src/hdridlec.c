/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     I D L E  S T A T E  P R O T O C O L   C O N F I G U R A T I O N

GENERAL DESCRIPTION
  This module contains code related to the configuration negotiation of the
  idle state protocol (IDLE).


EXTERNALIZED FUNCTIONS (Global)
  None

EXTERNALIZED FUNCTIONS (Regional)
  hdridlec_process_msg      - process message for the configuration module
  hdridlec_process_ind      - process indication for the configuration module
  hdridlec_process_sig      - process signal for the configuration module
  hdridlec_powerup_init     - perform powerup initialization
  hdridlec_start_config     - start idle state configuration
  hdridlec_update_config    - update the configuration if needed
  hdridlec_config_is_needed - query whether the configuration negotiation
                              is needed
  hdridlec_set_current_pccc_to_default - 
                            - initialize the PCCC to be default hash val.
                              (because ran_next() doesn't work on powerup)
                              
  Functions that are really part of SCM, but are here for now:
                                                           
  hdridlec_get_current_pccc - Returns current PCCC                            
  hdridlec_get_slotted_mode_attrib - Returns current SlottedMode attribute. 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating
  the protocol.


Copyright (c) 2000 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdridlec.c_v   1.30   24 Apr 2003 11:42:08   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdridlec.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/09/14   cnx     Support NV refresh without reset.
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
06/20/13   ukl     Set PCCC on entering idle state.
08/10/12   wsh     Fixed MultiATPage packing error
09/24/11   cnx     Changed subtype checking for Optimized Handoff.
12/17/10   cnx     Fixed compiler warnings.
06/21/10   cnx     Fixed compiler warnings.
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/17/09   pba     Do not accpet or send GAUP of slot cycle < 6 when 
                   SmallSlotCycleAllowed attribute is set to 0.
12/23/08   pba     Support for Quick Idle Protocol
05/22/08   pba     Klockwork fixes.
10/17/07   pba     Support for PageResponseApersistenceSupported attribute.
09/11/07   wsh     Removed lint high warnings
05/22/07   sju     Added support for phase2 of AT-init GAUP of SlottedMode attribute. 
12/04/06   pba     Check and propose PCCC if needed after personality switch.
11/06/06   pba     Strictly check that in a ConfigResponse message an attribute
                   has no more than just the length, attribute_id and value_id
                   fields, ignore the message if otherwise.
11/03/06   yll     Fixed a MUM sleep issue by notifying Search with our 
                   default CCC value.
10/03/06   pba     Fixed potential bug in ConfigReq message unpacking routine.
09/29/06   yll     Treat SCI < 5 as non-slotted operation
09/21/06   yll     Reject SlottedMode attribute when SCI is lower than 5.
08/08/06   hal     Added support for NV_HDRIDLE_PREF_SLOTTED_MODE_CONFIG_I to
                   store/propose desired SlottedMode attrib values
07/26/06   pba     Do not propose SmallSlotCycleAllowed 0. Since the AT does 
                   support SCI 5 now, the default of 1 is appropriate.
06/14/06   etv     Added support for AT-init GAUP of SlottedMode attribute.
05/10/06   yll/ksu compute pccc before writing to persistant storage
05/10/06   dna     Fixed gaup_failed_attrib_id reported for logging
04/27/06   pba     If inuse PCCC value is recomputed after first HDR acquisition
                   update SCMDB's copy also.
04/21/06   hal     Clear ConfigResponse and GAUPResponse timers on SessionClose
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
04/02/06   dna     Reuse NV item#3465 to allow a test API to enable the AT to 
                   propose the SlottedMode attribute during AT_INIT state
03/22/06   pba     Send Session close if AT could not accept AN's AUR message.
03/17/06   dna     Don't propose PCCC in HDR Only mode.
01/24/06   hal     Fix to ignore invalid attributes based on the InConfig subtype
01/04/06   hal     Added GAUP support for AccessHashingChannelMask
12/13/05   pba     Changes to support multiple personality negotiation
12/06/05   hal     Fixed attrib parsing error for AccessHashingChannelMask
11/29/05   hal     Fixed fallthrough to Unknown Attrib ID default case
11/21/05   hal     Added support for the AccessHashingChannelMask attribute
11/14/05   dna     Phase 0 support for multiple personalities
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
07/19/05   dna     Fixed GAUPing of SlottedMode attribute
07/07/05   dna     Added support for Enhanced Idle State protocol
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
12/13/04   hal     Changed another occurrence of dsm_offset_new_buffer to use 
                   hdrutil_new_buffer.
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
10/06/03    km     Replaced cm_get_cur_hybr_status() with sd_misc_is_hybr_opr()
09/10/03   mpa     Converted F3 messages to use new MSG2.0 PROT SSID
03/19/03   mpa     Allocated session_fail_data out of the heap because data
                   may be used in different task contexts
03/12/03   mpa     Added session close reason parameter to fail_cur_config_neg
04/02/02   dna     Add E_CANCELED as possible outcome for config req. msg.
03/29/02   dna     Changed timers to use REX callback instead of indications
01/21/02   sh      Featurized pref session seed and use fall-back PCCC value
                   instead of failing the session if proposed configuration
                   is not accepted by the AN.
09/10/01   sh      Make config request handling identical to others.
06/06/01   dna     new param for hmp_send_msg()
06/05/01   SH      Set negotiated to current when current is updated
05/31/01   SH      Added hdridlec_init_config_if_empty.
                   Removed session open processing.
05/17/01   SH      Added session seed generation and modified config request
                   message processing
04/19/01   SH      Initialied PCCCnegotiated and PCCCdefault_val when seesion
                   open
04/17/01   SH      Removed negotiation state
03/27/01   SH      Ignored SLP callback if trans is mismatched
03/23/01   SH      Added more debug messages and fixed bugs found from unit
                   test
03/19/01   vas     Changed to use hash in ran.c instead of hdrutil_hash
03/14/01   SH      Added Sending Config request and Process Config response
02/23/00   SH      Created module.

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrtrace.h"
#include "rex.h"
#include "hdrdebug.h"
#include "err.h"
#include "task.h"
#include "hdridle.h"
#include "hdridlei.h"
#include "hdrcmac.h"
#include "hdrerrno.h"
#include "hdridlec.h"
#include "hdridles.h"
#include "hdrhmp.h"
#include "hdrmci.h"
#include "cm.h"

#ifdef FEATURE_CMI
#include "cm_v.h"
#endif

#include "ran.h"
#include "hdrscp.h"
#include "hdrsmp.h"
#include "hdramp.h"
#include "hdrutil.h"
#include "hdrsrch.h"
#include "hdrscmdb.h"
#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
#include "mccdma.h"

#ifdef FEATURE_CMI
#include "mccdma_v.h"
#endif

#ifdef FEATURE_HDR_REVC
#include "hdrmc_v.h"
#endif /* FEATURE_HDR_REVC */

#endif

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains defidleions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define HDRIDLEC_SMALL_SIMPLE_ATTRIB_VAL_SIZE      8
#define HDRIDLEC_SMALL_SIMPLE_ATTRIB_MIN_VAL_SIZE 16
#define HDRIDLEC_LARGE_SIMPLE_ATTRIB_VAL_SIZE     16
#define HDRIDLEC_LARGE_SIMPLE_ATTRIB_MIN_VAL_SIZE 24

#define HDRIDLEC_SMALL_SLOT_CYCLE_ALLOWED_ATTRIB_ID    0xff
#define HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_ID 0xfe
#define HDRIDLEC_CONNECTION_DENY_BACKOFF_ATTRIB_ID     0xfd
#define HDRIDLEC_PAGERESPONSE_APERSISTENCE_SUPPORTED_ATTRIB_ID 0xfc
#define HDRIDLEC_QUICK_PAGING_ATTRIB_ID   0x03
#define HDRIDLEC_PCCC_CONFIG_ATTRIB_ID    0x00 /* PCCC Attribute ID */
#define HDRIDLEC_SLOTTED_MODE_ATTRIB_ID   0x01 /* SlottedMode Attribute ID */

#define HDRIDLEC_CONFIG_REQ_DEFAULT_LEN       3 /* 3 bytes for the Length
                                                   field                   */
#define HDRIDLEC_CONFIG_REQ_NON_DEFAULT_LEN   4 /* 4 bytes for the Length
                                                   field                   */
#define HDRIDLEC_CONFIG_RES_REC_LEN           2 /* length in bytes for
                                                   attribute and
                                                   value id field          */
#define HDRIDLEC_CONFIG_RES_ONE_ATTRIB_LEN    5 /* need 5 bytes for config
                                                   res to contain only one
                                                   attribute record        */

#define HDRIDLEC_CONFIG_RES_TIMER_MSECS   2000  /* config response time */

#define HDRIDLEC_BITS_PER_BYTE            8
#define HDRIDLEC_MSG_ID_SIZE              8
#define HDRIDLEC_TRANSACTION_ID_SIZE      8
#define HDRIDLEC_ATTRIB_LEN_SIZE          8
#define HDRIDLEC_ATTRIB_ID_SIZE           8
#define HDRIDLEC_MSG_PRIORITY             40

#define HDRIDLEC_VALUE_ID_SIZE 8

#define HDRIDLEC_LARGE_SIMPLE_ATTRIB_SIZE_BYTES \
          ((HDRIDLEC_ATTRIB_ID_SIZE + HDRIDLEC_LARGE_SIMPLE_ATTRIB_VAL_SIZE) \
           / HDRIDLEC_BITS_PER_BYTE)
           
#define HDRIDLEC_SMALL_SIMPLE_ATTRIB_SIZE_BYTES \
          ((HDRIDLEC_ATTRIB_ID_SIZE + HDRIDLEC_SMALL_SIMPLE_ATTRIB_VAL_SIZE) \
           / HDRIDLEC_BITS_PER_BYTE)

#define  HDRIDLEC_PCCC_ENABLED_SIZE  1
#define  HDRIDLEC_PCCC_VAL_SIZE      15
#define  HDRIDLEC_PCCC_RESERVED_SIZE 7

#define  HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE 5
#define  HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE 4
#define  HDRIDLEC_SLOTTED_MODE_RESERVED_SIZE   1

#define  HDRIDLEC_QUICKPAGING_QP_ENABLED_SIZE 1
#define  HDRIDLEC_QUICKPAGING_SUBSYNC_QP_ENABLED_SIZE 1

/* For a complex attribute, ATTRIB_MIN_VAL_SIZE is the minimum size of the
 * attribute value structure, including the ValueID
 */
#define HDRIDLEC_PCCC_CONFIG_ATTRIB_MIN_VAL_SIZE 16
#define HDRIDLEC_SLOTTED_MODE_MIN_ATTRIB_SIZE 48
#define HDRIDLEC_SLOTTED_MODE_ATTRIB_VAL_SIZE 32
#define HDRIDLEC_SLOTTED_MODE_ATTRIB_SIZE_BYTES \
          ((HDRIDLEC_ATTRIB_ID_SIZE + HDRIDLEC_SLOTTED_MODE_ATTRIB_VAL_SIZE) \
           / HDRIDLEC_BITS_PER_BYTE)

#define HDRIDLEC_QUICK_PAGING_MIN_ATTRIB_SIZE 32
#define HDRIDLEC_QUICK_PAGING_ATTRIB_VAL_SIZE 16

/* For a complex attribute, ATTRIB_MAX_VAL_SIZE is the maximum size of the
 * attribute value structure, including the ValueID
 */
#define HDRIDLEC_PCCC_CONFIG_ATTRIB_MAX_VAL_SIZE 24


/* The minimum attribute has Length, attribute ID and one value */
#define HDRIDLEC_PCCC_CONFIG_MIN_ATTRIB_LEN \
        ( HDRIDLEC_ATTRIB_LEN_SIZE + \
          HDRIDLEC_ATTRIB_ID_SIZE + \
          HDRIDLEC_PCCC_CONFIG_ATTRIB_MIN_VAL_SIZE )


/* The smallest attribute is a length with an AttributeID */
#define HDRIDLEC_MIN_ATTRIB_LEN \
        ( HDRIDLEC_ATTRIB_LEN_SIZE + \
          HDRIDLEC_ATTRIB_ID_SIZE )

/* Config response attributes are always just Attribute ID, AttributeValue */
#define HDRIDLEC_CRSP_ATTRIB_VAL_SIZE 8
#define HDRIDLEC_CRSP_ATTRIB_LEN     ( HDRIDLEC_ATTRIB_LEN_SIZE + \
                                       HDRIDLEC_ATTRIB_ID_SIZE + \
                                       HDRIDLEC_CRSP_ATTRIB_VAL_SIZE )

/* The field in the config response is length in bytes,
 * excluding the length of length field.
 */
#define HDRIDLEC_CRSP_ATTRIB_LEN_BYTES  ((HDRIDLEC_ATTRIB_ID_SIZE + \
          HDRIDLEC_CRSP_ATTRIB_VAL_SIZE) / HDRIDLEC_BITS_PER_BYTE)


#define HDRIDLEC_SLOTTED_MODE_VALUE_ID  0x01
#define HDRIDLEC_DESIRED_SLOT_CYCLE1  0x09
#define HDRIDLEC_DESIRED_SLOT_CYCLE2  0x09
#define HDRIDLEC_DESIRED_SLOT_CYCLE3  0x09
#define HDRIDLEC_DESIRED_WAKE_COUNT1  0x00
#define HDRIDLEC_DESIRED_WAKE_COUNT2  0x00

#define HDRIDLEC_MAX_SLOT_CYCLE       0x1C
#define HDRIDLEC_MIN_SLOT_CYCLE       0x05
  /* Although standard didn't specify it, but SCI lower than 5
   * doesn't make much sense. */

#ifdef FEATURE_HDR_REVC
#define HDRIDLEC_MULTI_PAGE_LEN_SIZE              0x03 /* Length of MultiATIPage */

#define HDRIDLEC_MULTI_PAGE_SUPPORTED_ATTRIB_ID   0xFB /* MultiATIPage AttribID */

#define HDRIDLEC_MULTI_PAGE_UNSUPPORTED           0x00 /* Not Support MultiATIPage */
#define HDRIDLEC_MULTI_PAGE_SUPPORTED             0x01 /* Support MultiATIPage */
#endif /* FEATURE_HDR_REVC */

/* Configuration Request message */
typedef struct
{
   uint8                            trans_id;
   boolean                          pccc_is_requested;
   uint8                            pccc_value_id;
   boolean                          slotted_mode_is_requested;
   boolean                          small_slot_cycle_is_requested;
   hdridlec_pccc_config_attrib_type pccc;
#ifdef FEATURE_HDR_REVC
   boolean                          multi_page_is_requested;
#endif /* FEATURE_HDR_REVC */
} hdridlec_config_req_msg_type;

/*===========================================================================

          MACROS FOR EXTRACING TRANSACTION ID

===========================================================================*/

#define HDRIDLEC_UNPACK_TRANS_ID( msg_ptr, trans_id) \
          (HDRBIT_UNPACK8(msg_ptr, (trans_id), 8, 8))

/*===========================================================================

          MACROS FOR EXTRACING CONFIG REQUEST MESSAGE FIELDS

===========================================================================*/

/*-- Configuration Request Message --*/
#define HDRIDLEC_UNPACK_CON_REQ_LEN( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), (uint16) ((aoffset) + 0), 8))

#define HDRIDLEC_UNPACK_CON_REQ_ATTRIBUTE_ID( msg_ptr, val_ptr, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr), (uint16) ((aoffset) + 8), 8))

  /*- PCCC Configuration Attribute  -*/
#define HDRIDLEC_UNPACK_CON_REQ_PCCC_VALUE_ID( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr),   \
          (uint16) ((aoffset) + (voffset) + 0), 8))

#define HDRIDLEC_UNPACK_CON_REQ_PCCC_ENABLED( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK8((msg_ptr), (val_ptr),    \
          (uint16) ((aoffset) + (voffset) + 8), 1))

#define HDRIDLEC_UNPACK_CON_REQ_PCCC_VALUE( msg_ptr, val_ptr, aoffset, voffset ) \
          (HDRBIT_UNPACK16((msg_ptr), (val_ptr), \
          (uint16) ((aoffset) + (voffset) + 9), 15))



/*===========================================================================

          MACROS FOR EXTRACING CONFIG RESPONSE MESSAGE FIELDS

===========================================================================*/

#define HDRIDLEC_UNPACK_TRANS_ID( msg_ptr, trans_id) \
          (HDRBIT_UNPACK8(msg_ptr, (trans_id), 8, 8))

/*-- Configuration Response Message --*/
#define HDRIDLEC_UNPACK_CON_RSP_LEN( msg_ptr, val, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), ((aoffset) + 0), 8))
#define HDRIDLEC_UNPACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), ((aoffset) + 8), 8))

#define HDRIDLEC_UNPACK_CON_RSP_VALUE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_UNPACK8((msg_ptr), (val), ((aoffset) + 16), 8)) 


/*===========================================================================

          MACROS FOR PACKING CONFIG REQUEST MESSAGE FIELDS

===========================================================================*/


#define HDRIDLEC_PACK_CONFIG_REQ_MSG_ID(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, MessageID), \
               (uint16) FSIZ(hdridlec_config_req_type, MessageID))

#define HDRIDLEC_PACK_CONFIG_REQ_TRANS_ID(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, TransactionID), \
               (uint16) FSIZ(hdridlec_config_req_type, TransactionID))

#define HDRIDLEC_PACK_CONFIG_REQ_LENGTH(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, Length), \
               (uint16) FSIZ(hdridlec_config_req_type, Length))


#define HDRIDLEC_PACK_CONFIG_REQ_ATTRIB_ID(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, AttributeID), \
               (uint16) FSIZ(hdridlec_config_req_type, AttributeID))


#define HDRIDLEC_PACK_CONFIG_REQ_VALUE_ID(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, ValueID), \
               (uint16) FSIZ(hdridlec_config_req_type, ValueID))

#define HDRIDLEC_PACK_CONFIG_REQ_PCCC_ENABLED(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, PcccEnabled), \
               (uint16) FSIZ(hdridlec_config_req_type, PcccEnabled))


#define HDRIDLEC_PACK_CONFIG_REQ_PCCC(src_ptr, data) \
               HDRBIT_PACK16(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_type, Pccc), \
               (uint16) FSIZ(hdridlec_config_req_type, Pccc))

#define HDRIDLEC_PACK_CONFIG_REQ_RESERVED(src_ptr, data) \
               HDRBIT_PACK8(src_ptr, data,    \
               (uint16) FPOS(hdridlec_config_req_reserved_type, Reserved), \
               (uint16) FSIZ(hdridlec_config_req_reserved_type, Reserved))

/*===========================================================================

          MACROS FOR PACKING CONFIG RESPONSE MESSAGE FIELDS

===========================================================================*/
#define HDRIDLEC_PACK_CON_RSP_MSG_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 0, 8))
#define HDRIDLEC_PACK_CON_RSP_TRANS_ID( msg_ptr, val) \
          (HDRBIT_PACK8((msg_ptr), (val), 8, 8))

#define HDRIDLEC_PACK_CON_RSP_LEN( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), (uint16) ((aoffset) + 0), 8))
#define HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK16((msg_ptr), (val), (uint16) ((aoffset) + 8), 8))
#define HDRIDLEC_PACK_CON_RSP_VALUE_ID( msg_ptr, val, aoffset) \
          (HDRBIT_PACK8((msg_ptr), (val), (uint16) ((aoffset) + 16), 8))



static struct
{
   rex_timer_type                config_res_timer;/* waiting for
                                                     configuration
                                                     response timer    */
   hdridlec_config_params_type   current;         /* current in use
                                                     configuration     */
   hdridlec_config_params_type   negotiated;      /* negotiated
                                                     configuration     */
   boolean                       await_slp_config_req_cb; /* flag
                                                   inidcating whether Idle
                                                   is waiting for config req
                                                   msg TX callback from
                                                   SLP                  */
   hdridlec_config_req_msg_type  config_req_msg;  /* config request msg */
   boolean                       gen_pgslot_pccc; /* generate pccc based on
                                                    idle paging slot */
   hdridlec_slotted_mode_type    desired_slotted_mode_config; 

}hdridlec;



/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_COMPUTE_PCCC
DESCRIPTION
  This function using the paging slot to compute the pccc
  for the hybrid operation.

DEPENDENCIES
  CDMA software must be present

PARAMETERS
  None

RETURN VALUE
  The desired preferred control channel cycle.

SIDE EFFECTS
  None

===========================================================================*/
static uint16 hdridlec_compute_pccc
(
  boolean is_custom,
  uint8 max_sci
)
{
  uint16 idle_pgslot=0;    /* paging slot from CDMA */
  uint16 pccc;             /* computed value for pccc */
  uint32 session_seed = 0; /* session seed value */
  int    num_cc_cycles;    /* how many cc cycles are in the slot */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* max_sci   num_cc_cycles   period
   *    6            1          426ms
   *    7            3          1.28s
   *    8            6          2.54s
   *    9           12          5.12s
   *   10           24         10.24s
   *   etc..
   */

  if (max_sci >= 7 )
  {
    num_cc_cycles = 3 * (1 << (max_sci - 7));
  }
  else
  {
    num_cc_cycles = 1;
  }

  session_seed = hdramp_get_session_seed(HDRHAI_IDLE_STATE_PROTOCOL);
  pccc = (uint16) hash( session_seed,  
    num_cc_cycles, (uint16) (6*(session_seed &  0x0FFF)));

  if (is_custom)
  {
    idle_pgslot = mcc_get_idle_pgslot();

    /* SRCH wants to know the "default" CCC to help determine where to
     * place the "calculated" PCCC.  SRCH will pick a value close to the 
     * hash result that doesn't colide with 1x.  This ensures the PCCC
     * chosen is spread across the full range of possible values.
     */
    pccc = hdrlmac_get_pccc( idle_pgslot, max_sci, pccc,
                             HDRHAI_IDLE_STATE_PROTOCOL );
    HDR_MSG_PROT_5 ( MSG_LEGACY_HIGH, 
                    "Custom PCCC=%d, SS=0x%x, max_sci=%d, CCC=%d, 1x_slot=%d",
                     pccc, session_seed, max_sci, num_cc_cycles, idle_pgslot );
  }
  else
  {
    HDR_MSG_PROT_4 ( MSG_LEGACY_HIGH, 
                    "Default PCCC=%d, SS=0x%x, max_sci=%d, CCC=%d", 
                     pccc, session_seed, max_sci, num_cc_cycles );
  }

  return pccc;

} /* hdridlec_compute_pccc() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_FAIL_CUR_CONFIG_NEG

DESCRIPTION
  This function fails the current config negotiation

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_fail_cur_config_neg
(
  hdrhai_session_close_reason_enum_type close_reason,
  uint8 attrib_id
)
{
  hdrind_ind_data_union_type ind_data;

  /* Allocate out of the heap because it is possible for this indication to
  be queued from a different task context. Otherwise, the payload may be
  corrupted which could cause a null pointer de-reference. */
  static hdrscp_scp_fail_ind_type session_fail_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* fill the indication data */
  session_fail_data.type = HDRHAI_IDLE_STATE_PROTOCOL;
  session_fail_data.subtype = hdrscp_get_config_subtype(HDRHAI_IDLE_STATE_PROTOCOL) ;
  session_fail_data.attribute_id_length = HDRIDLE_ATTRIBUTE_ID_LENGTH;
  session_fail_data.attribute_id = attrib_id;
  session_fail_data.reason = close_reason;

  ind_data.scp_fail_ptr = (void *)&session_fail_data;
  hdrind_give_ind(HDRIND_IDLE_PROT_NEG_FAILED,(void *)&ind_data);

} /* hdridlec_fail_cur_config_neg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_CONFIG_REQ_MSG_TX_OUTCOME_CB

DESCRIPTION
  This function is as a callback funtion for transmission outcome of the
  config request message.

DEPENDENCIES
  None

PARAMETERS
  data        - user data
  status      - message transmission status


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_config_req_msg_tx_outcome_cb
(
  void                 *data,
  hdrerrno_enum_type    status
)
{
  hdridles_state_enum_type cur_state = hdridles_get_state();/* current state*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (!hdridlec.await_slp_config_req_cb)
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Ignore config req tx outcome callback");
    return;
  }

  if (hdridlec.config_req_msg.trans_id != (byte) ((dword) data))
  {
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Ignore config req tx outcome callback");
    return;
  }

  HDR_MSG_PROT_1 (MSG_LEGACY_LOW, "SLP config req tx outcome cb - status=%d",status);

  hdridlec.await_slp_config_req_cb = FALSE;

  switch(status)
  {
    case E_SUCCESS:

      /* message transmitted successfully */
      HDRTRACE_TXMSG_ACKED(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLEC_CONFIG_REQ_ID, cur_state);  /*lint !e506 */
      ( void ) rex_set_timer( &hdridlec.config_res_timer,
                              HDRIDLEC_CONFIG_RES_TIMER_MSECS );
      break;

    case E_FAILURE:

      HDRTRACE_TXMSG_FAILED(HDRHAI_IDLE_STATE_PROTOCOL,
      HDRIDLEC_CONFIG_REQ_ID, cur_state); /*lint !e506 */
      hdridlec_fail_cur_config_neg( HDRHAI_CLOSE_CFG_MSG_FAIL, 
                                    HDRIDLEC_PCCC_CONFIG_ATTRIB_ID );
      break;

    case E_AGAIN:   /* given after max retries on the RTC */
    case E_CANCELED:/* given if connection closes before max retries */
      /* ignore, configure command will be given again by SCP */
    case E_NOT_ALLOWED:
      break;

    default:
        ERR ("Unrecognized SLP status %d",
             status, 0, 0);   /*lint !e516 */

      break;
  }

} /* hdridlec_config_req_msg_tx_outcome_cb() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_SET_CURRENT_PCCC_TO_DEFAULT

DESCRIPTION
  This function sets the current PCCC value based on the hash algorithm.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_set_current_pccc_to_default(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdridlec.current.pccc.is_custom)
  {
    /* When AT powers up without a valid session PCCC value is not computed
     * correctly because the session_seed is not generated, as the random 
     * number generator is not yet initialized. For this special case we 
     * initialize it again after the random number generator is initialized. 
     */
    hdridlec.current.pccc.value = hdridlec_compute_pccc( FALSE, 0x09 );

    /* Also update the SCMDB copy of IDLE PCCC */
    hdrscmdb_set_current_pccc_to_default( hdridlec.current.pccc.value );

    /* Notify Search about our default CCC value, which will overwrite
     * Search's default Null CCC value (0xff). It is OK to give Search
     * a value that AN doesn't know about, because we never expect the
     * AN to send us something "in our slot" until we have negotiated 
     * a session and agreed upon a value for PCCC.  So before that, 
     * any value is fine. For the MUM builds specifically, we don't 
     * negotiate a session until there is a packet data call, and don't
     * expect AN to send AT anything before that.
     */
    hdridlec_set_ccc();
  }
}/* hdridlec_set_current_pccc_to_default() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_SET_CONFIG_PARAMS_DEFAULT

DESCRIPTION
  This function initializes the current config values to their defaults.

DEPENDENCIES
  None

PARAMETERS
  current_config - Pointer to IDLE Protocol's current configuration structure
                   in SCMDB.

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_set_config_params_default
( 
  hdridlec_config_params_type *current_config 
)
{
  current_config->pccc.is_custom = FALSE;
  /* The PCCC value computed here during powerup is not correct since rand
   * number generator is not yet initialized. So the case when AT powers up
   * without a valid session inuse PCCC value is set correctly after first
   * acquisition in hdridlec_set_current_pccc_to_default() function */
  current_config->pccc.value = hdridlec_compute_pccc( FALSE, 0x09 );
  current_config->slotted_mode.slot_cycle1 = 0x09;
  current_config->slotted_mode.slot_cycle2 = 0x09;
  current_config->slotted_mode.slot_cycle3 = 0x09;
  current_config->slotted_mode.wake_count1 = 0x00;
  current_config->slotted_mode.wake_count2 = 0x00;
  current_config->small_slot_cycle_is_allowed = 0x01;
  current_config->access_hashing_channel_mask = 
    HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_DEFAULT;
  current_config->saved_scmidle_info.saved_slotted_mode_is_valid = FALSE;
  current_config->saved_scmidle_info.saved_slotted_mode = current_config->slotted_mode;
  current_config->connectiondeny_backoff = 0xFF;
  current_config->pageresponse_apersistence_supported = 0x00;
  current_config->quick_paging.quick_paging_enabled = FALSE;
  current_config->quick_paging.subsync_quick_paging = FALSE;
#ifdef FEATURE_HDR_REVC
  current_config->multi_page_supported = FALSE;
#endif /* FEATURE_HDR_REVC */
}/* hdridlec_set_config_params_default() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_ABORT_CUR_CONFIG_NEG

DESCRIPTION
  This function aborts the current config negotiation if it is
  in progress

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_abort_cur_config_neg(void)
{
  if(hdridlec.await_slp_config_req_cb)
  {
    /* The last parameter should be a pointer, which is 4 bytes long on most machines.
       However, trans_id is only 1 byte (uint8), so compiler will give a warning like:
       cast to pointer from integer of different size. Cast the uint8 to uint32 first
       to avoid compiler warning. */
    (void) hdrhmp_abort_msg( hdridlec_config_req_msg_tx_outcome_cb,
             (void *) ((uint32)hdridlec.config_req_msg.trans_id));
    hdridlec.await_slp_config_req_cb = FALSE;
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Abort SLP Config Req tx");
  }

  /* stop the config response timer */
  ( void ) rex_clr_timer( &hdridlec.config_res_timer);
} /* hdridlec_abort_cur_config_neg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_SLOTTED_MODE_ATTRIB_CONFIG_IS_NEEDED
DESCRIPTION
  This function check whether or not the Slotted Mode attribute needs to
  be configured.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdridlec_slotted_mode_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */
  /* Shorthand pointer to current slotted mode attribute */
  hdridlec_slotted_mode_type *current_sma_ptr = 
    &hdridlec.current.slotted_mode;
  uint16 protocol_subtype_being_configured =
    hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
  hdridlec_slotted_mode_type  *dsmcp = &hdridlec.desired_slotted_mode_config; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( protocol_subtype_being_configured != HDRSCP_DEFAULT_IDLE )
  {
    /* Compare hdridlec.current.slotted_mode to the values desired */
    if ((current_sma_ptr->slot_cycle1 == dsmcp->slot_cycle1) &&
        (current_sma_ptr->slot_cycle2 == dsmcp->slot_cycle2) &&
        (current_sma_ptr->slot_cycle3 == dsmcp->slot_cycle3) &&
        (current_sma_ptr->wake_count1 == dsmcp->wake_count1) &&
        (current_sma_ptr->wake_count2 == dsmcp->wake_count2))
    {
      /* No config is needed since we already have our desired values */
    }
    else
    {
      config_is_needed = TRUE;
    }
  }

  return config_is_needed;
} /* hdridlec_slotted_mode_attrib_config_is_needed() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_PCCC_ATTRIB_CONFIG_IS_NEEDED

DESCRIPTION
  This function checks whether the PreferredControlChannelCycle attribute
  needs to be re-negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_pccc_attrib_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* whether config is needed */
  uint16 pccc; /* Our desired PCCC */
  uint16 protocol_subtype_being_configured =
    hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sd_misc_is_hybr_opr())
  {
    if (!hdridlec.current.pccc.is_custom)
    {
      /* We always want a custom pccc while in hybrid mode */
      config_is_needed = TRUE;
    }
    else 
    {
      if ( protocol_subtype_being_configured != HDRSCP_DEFAULT_IDLE )
      {
        /* We want the pccc to be custom and based on our largest desired
         * sleep period */
        pccc = hdridlec_compute_pccc( TRUE, HDRIDLEC_DESIRED_SLOT_CYCLE3);
      }
      else
      {
        /* We want the pccc to be custom and based on 5.12s sleep cycle */
        pccc = hdridlec_compute_pccc( TRUE, 0x09 );
      }

      if (hdridlec.current.pccc.value != pccc)
      {
        config_is_needed = TRUE;
      }
    }
  }

  return config_is_needed;
} /* hdridlec_pccc_attrib_config_is_needed() */

/* <EJECT> */
#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRIDLEC_SEND_CONFIG_REQ_MSG

DESCRIPTION
  This function sends the configuration request message to the access
  network

DEPENDENCIES
  None

PARAMETERS
  full_negotiation - If this is an initial negotiation of the idle state
  protocol or just a re-negotiation of the PreferredControlChannelCycle

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_send_config_req_msg 
( 
  boolean full_negotiation /* as opposed to PCCC re-negotiation only */
)
{
  dsm_item_type  *dsm_ptr;
  uint16 offset;
  uint16 pccc; /* Our desired PCCC */
  uint16 protocol_subtype_being_configured =
    hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
  hdridlec_slotted_mode_type  *dsmcp = &hdridlec.desired_slotted_mode_config; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsm_ptr = hdrutil_new_msg_buffer();

  /* Compose the config request message */

  offset = 0;

  /* MessageID */
  HDRBIT_PACK8( dsm_ptr, HDRIDLEC_CONFIG_REQ_ID,
                offset, HDRIDLEC_MSG_ID_SIZE);
  offset += HDRIDLEC_MSG_ID_SIZE;

  /* TransactionID*/
  hdridlec.config_req_msg.trans_id++;
  HDRBIT_PACK8( dsm_ptr, hdridlec.config_req_msg.trans_id,
                offset, HDRIDLEC_TRANSACTION_ID_SIZE);
  offset += HDRIDLEC_TRANSACTION_ID_SIZE;

  hdridlec.config_req_msg.pccc_is_requested = FALSE;
  hdridlec.config_req_msg.slotted_mode_is_requested = FALSE;
  hdridlec.config_req_msg.small_slot_cycle_is_requested = FALSE;
  hdridlec.config_req_msg.pccc.is_custom = FALSE;

#ifdef FEATURE_HDR_REVC
  hdridlec.config_req_msg.multi_page_is_requested = FALSE;
#endif /* FEATURE_HDR_REVC */
  /* PCCC Attribute */

  if (hdridlec_pccc_attrib_config_is_needed())
  {
    /* Use a custom value that doesn't colide with the 1x slot */

    hdridlec.config_req_msg.pccc_is_requested = TRUE;

    /* Length */
    HDRBIT_PACK8( dsm_ptr, HDRIDLEC_CONFIG_REQ_NON_DEFAULT_LEN,
                  offset, HDRIDLEC_ATTRIB_LEN_SIZE);
    offset += HDRIDLEC_ATTRIB_LEN_SIZE;

    /* AttributeID */
    HDRBIT_PACK8( dsm_ptr, HDRIDLEC_PCCC_CONFIG_ATTRIB_ID,
                  offset, HDRIDLEC_ATTRIB_ID_SIZE);
    offset += HDRIDLEC_ATTRIB_ID_SIZE;

    /* ValueID */
    hdridlec.config_req_msg.pccc_value_id++;
    HDRBIT_PACK8( dsm_ptr, hdridlec.config_req_msg.pccc_value_id,
                  offset, HDRIDLEC_VALUE_ID_SIZE);
    offset += HDRIDLEC_VALUE_ID_SIZE;

    /* PCCCEnabled */
    HDRBIT_PACK8( dsm_ptr, TRUE, offset, HDRIDLEC_PCCC_ENABLED_SIZE);
    offset += HDRIDLEC_PCCC_ENABLED_SIZE;

    if ( protocol_subtype_being_configured != HDRSCP_DEFAULT_IDLE )
    {
      /* We want the pccc to be custom and based on our largest desired
       * sleep period */
      pccc = hdridlec_compute_pccc( TRUE, HDRIDLEC_DESIRED_SLOT_CYCLE3);
    }
    else
    {
      /* We want the pccc to be custom and based on 5.12s sleep cycle */
      pccc = hdridlec_compute_pccc( TRUE, 0x09 );
    }

    /* PCCCValue */
    hdridlec.config_req_msg.pccc.is_custom = TRUE;
    hdridlec.config_req_msg.pccc.value = pccc;
    HDRBIT_PACK16( dsm_ptr, pccc, offset, HDRIDLEC_PCCC_VAL_SIZE);
    offset += HDRIDLEC_PCCC_VAL_SIZE;
  }


  if (full_negotiation)
  {
    /* Only include other attributes if we haven't already negotiated them */

    if (hdridlec_slotted_mode_attrib_config_is_needed())
    {
      hdridlec.config_req_msg.slotted_mode_is_requested = TRUE;

      /* Length, assuming we include just one SlottedMode attribute value */
      HDRBIT_PACK8( dsm_ptr,  HDRIDLEC_SLOTTED_MODE_ATTRIB_SIZE_BYTES,
                    offset, HDRIDLEC_ATTRIB_LEN_SIZE);
      offset += HDRIDLEC_ATTRIB_LEN_SIZE;

      /* AttributeID */
      HDRBIT_PACK8( dsm_ptr, HDRIDLEC_SLOTTED_MODE_ATTRIB_ID,
                    offset, HDRIDLEC_ATTRIB_ID_SIZE);
      offset += HDRIDLEC_ATTRIB_ID_SIZE;

      /* ValueID */
      HDRBIT_PACK8( dsm_ptr, HDRIDLEC_SLOTTED_MODE_VALUE_ID,
                    offset, HDRIDLEC_VALUE_ID_SIZE);
      offset += HDRIDLEC_VALUE_ID_SIZE;

      /* SlotCycle1 */
      HDRBIT_PACK8( dsm_ptr, dsmcp->slot_cycle1,
                    offset, HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE);
      offset += HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE;

      /* SlotCycle2 */
      HDRBIT_PACK8( dsm_ptr, dsmcp->slot_cycle2,
                    offset, HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE);
      offset += HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE;

      /* SlotCycle3 */
      HDRBIT_PACK8( dsm_ptr, dsmcp->slot_cycle3,
                    offset, HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE);
      offset += HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE;

      /* WakeCount1 */
      HDRBIT_PACK8( dsm_ptr, dsmcp->wake_count1,
                    offset, HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE);
      offset += HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE;

      /* WakeCount2 */
      HDRBIT_PACK8( dsm_ptr, dsmcp->wake_count2,
                    offset, HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE);
      offset += HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE;

      /* Reserved (1 bit) */
      HDRBIT_PACK8( dsm_ptr, 0,
                    offset, HDRIDLEC_SLOTTED_MODE_RESERVED_SIZE);
      offset += HDRIDLEC_SLOTTED_MODE_RESERVED_SIZE;
    }

#ifdef FEATURE_HDR_REVC
    if  ((hdrscp_get_config_subtype(HDRHAI_IDLE_STATE_PROTOCOL) != 
          HDRSCP_DEFAULT_IDLE) &&
         hdrmc_feature_is_enabled(HDRMC_FEATURE_MULTI_AT_PAGE))
    {
      hdridlec.config_req_msg.multi_page_is_requested = TRUE;

      /* Length */
      HDRBIT_PACK8(dsm_ptr, HDRIDLEC_MULTI_PAGE_LEN_SIZE,
                   offset, HDRIDLEC_ATTRIB_LEN_SIZE);
      offset += HDRIDLEC_ATTRIB_LEN_SIZE;

      /* AttributeID */
      HDRBIT_PACK8( dsm_ptr, HDRIDLEC_MULTI_PAGE_SUPPORTED_ATTRIB_ID,
                    offset, HDRIDLEC_ATTRIB_ID_SIZE);
      offset += HDRIDLEC_ATTRIB_ID_SIZE;

      /* ValueIDs */
      HDRBIT_PACK8( dsm_ptr, HDRIDLEC_MULTI_PAGE_SUPPORTED,
                    offset, HDRIDLEC_VALUE_ID_SIZE);
      offset += HDRIDLEC_VALUE_ID_SIZE;

      HDRBIT_PACK8( dsm_ptr, HDRIDLEC_MULTI_PAGE_UNSUPPORTED,
                    offset, HDRIDLEC_VALUE_ID_SIZE);
    }
#endif /* FEATURE_HDR_REVC */
  } /* end if (we haven't previously negotiated these attributes) */

  /* sending the message to HMP for transmission */

  /* The last parameter should be a pointer, which is 4 bytes long on most machines.
     However, trans_id is only 1 byte (uint8), so compiler will give a warning like:
     cast to pointer from integer of different size. Cast the uint8 to uint32 first
     to avoid compiler warning. */
  hdrhmp_send_msg
  (
    HDRHAI_IDLE_STATE_PROTOCOL,
    HDRHAI_RTC,
    TRUE,
    HDRIDLEC_MSG_PRIORITY,
    HDRHAI_IN_CONFIG, /* Instance of protocol sending message */
    dsm_ptr,
    hdridlec_config_req_msg_tx_outcome_cb,
    (void *) ((uint32)hdridlec.config_req_msg.trans_id)
  );

  /* set the flag to indicate idle is waiting
     for the SLP TX outcome callback */
  hdridlec.await_slp_config_req_cb = TRUE;

#ifdef FEATURE_HDR_REVC
  HDR_MSG_PROT_4 (MSG_LEGACY_HIGH, 
                  "Send ConfigReq pccc?%d slotted_mode?%d small_sc?%d multi_page?%d",
                  hdridlec.config_req_msg.pccc_is_requested,
                  hdridlec.config_req_msg.slotted_mode_is_requested,
                  hdridlec.config_req_msg.small_slot_cycle_is_requested,
                  hdridlec.config_req_msg.multi_page_is_requested );
#else /* !FEATURE_HDR_REVC */
  HDR_MSG_PROT_3 (MSG_LEGACY_HIGH, 
                  "Send ConfigReq pccc?%d slotted_mode?%d small_sc?%d",
                  hdridlec.config_req_msg.pccc_is_requested,
                  hdridlec.config_req_msg.slotted_mode_is_requested,
                  hdridlec.config_req_msg.small_slot_cycle_is_requested );
#endif /* !FEATURE_HDR_REVC */

} /* hdridlec_send_config_req_msg() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GET_MAX_SCI

DESCRIPTION
  This function returns the maximum SCI that could be used for sleeping.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The maximum SCI that could be used for sleeping.

SIDE EFFECTS
  None

===========================================================================*/
static uint8 hdridlec_get_max_sci (void)
{
  uint8 max_sleep_cycle; /* Longest sleep cycle the AT will use */
  hdrhai_protocol_name_enum_type  curr_subtype = 
      (hdrhai_protocol_name_enum_type)
      hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( curr_subtype != HDRSCP_DEFAULT_IDLE )
  {
    max_sleep_cycle = hdridlec.current.slotted_mode.slot_cycle3;
  }
  else
  {
    max_sleep_cycle = 0x09;
  }

  return max_sleep_cycle;
} /* hdridlec_get_max_sci() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GET_CURRENT_PCCC

DESCRIPTION
  This function returns the current Control Channel Cycle negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  The current Control Channel Cycle

SIDE EFFECTS
  None

===========================================================================*/
uint16 hdridlec_get_current_pccc( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (hdridlec.current.pccc.value);
} /* hdridlec_get_current_pccc() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_SET_CCC

DESCRIPTION
  This function determines the Control Channel Cycle to be used and gives 
  it to HDRSRCH.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_set_ccc (void)
{
  uint16 ccc; /* The Control Channel Cycle value to use */
  uint8 max_sleep_cycle; /* Longest sleep cycle the AT will use */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ccc = hdridlec_get_current_pccc();
  max_sleep_cycle = hdridlec_get_max_sci();
  hdrlmac_set_ccc( ccc, max_sleep_cycle, HDRHAI_IDLE_STATE_PROTOCOL );

} /* hdridlec_set_ccc() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GAUP_ACCEPT

DESCRIPTION
  This function performs processing needed when a GAUP message is accepted.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_gaup_accept (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!hdridlec.current.pccc.is_custom)
  {
    /* We can re-compute the "default" pccc at this point since we know
     * the max_sci has been established.
     */
    hdridlec.current.pccc.value = 
      hdridlec_compute_pccc( FALSE, hdridlec_get_max_sci());
  }

  /* Set the cc hash in LMAC based on what was negotiated */
  hdridlec_set_ccc();

} /* hdridlec_gaup_accept() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_SESSION_CHANGED_IND

DESCRIPTION
  This function processes the session changed indication.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_process_session_changed_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  boolean prev_personality_pccc_is_custom = FALSE;
    /* Flag indicating whether previous personality PCCC is custom or not */
/*------------------------------------------------------------------------*/

  /* Everytime AT switches personality check if the previous personality
     had custom PCCC, if it did then the new personality should also try
     and get a custom PCCC. If the previous personality did not have a 
     custom PCCC then it is most likely that it tried and got rejected
     by the AN, so we do not want to unnecessarily try again */
  if (ind_data_ptr->change_reason == HDRHAI_SCHG_PERSONALITY_SWITCH)
  {
    prev_personality_pccc_is_custom = hdridlec.current.pccc.is_custom;
  }

  /* Update the local copy of current configuration from SCMDB */
  hdrscmdb_get_inuse_config(HDRHAI_IDLE_STATE_PROTOCOL, &hdridlec.current);

  /* Set the initial value of hdrscmidle.saved_scmidle_info*/
  hdrscmidle_init_scmidle_info(&hdridlec.current.saved_scmidle_info);

  if (!hdridlec.current.pccc.is_custom)
  {
    /* We can re-compute the "default" pccc at this point since we know
     * the max_sci has been established.
     */
    hdridlec.current.pccc.value = 
      hdridlec_compute_pccc( FALSE, hdridlec_get_max_sci());
  }

  /* Set the cc hash in LMAC based on what was negotiated */
  hdridlec_set_ccc();

  /* Everytime we switch personality make sure that the new personality's 
     PCCC value is good. It is possible that we could have updated the 
     PCCC in the last personality that AT was using, so we need to do
     this check on every personality switch and re-configure PCCC for 
     the new inuse personality if needed.
   */
  if ((ind_data_ptr->change_reason == HDRHAI_SCHG_PERSONALITY_SWITCH) &&
       hdridlec_pccc_attrib_config_is_needed() &&
       prev_personality_pccc_is_custom)
  {
    hdridle_update_pref_cntl_chan_cycle(HDRHAI_IDLE_STATE_PROTOCOL);
  }

} /* hdridlec_process_session_changed_ind() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_SESSION_CLOSED_IND

DESCRIPTION
  This function processes the session opened indication

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_process_session_closed_ind (void)
{

  /* abort the current configuration negotiation if it is in progress */
  hdridlec_abort_cur_config_neg();

} /* hdridlec_process_session_closed_ind() */

/* <EJECT> */
#ifndef PERLUTF_TDTGEN_EXCLUDE
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_CONFIG_RES_MSG

DESCRIPTION
  This function processes the configuration response message

DEPENDENCIES
  hdridlec_init must have been called before calling this function.

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void hdridlec_process_config_res_msg
(
  dsm_item_type * item_ptr
)
{
  uint8              trans_id=0;       /* transaction id field */
  uint8              attribute_id=0;   /* attribute id field */
  uint8              value_id=0;       /* value id field */
  uint16 total_bits_remaining; /* number of bits remaining in the message */
  uint8  attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */

  /* Flags if this response accepts our proposed config attributes */
  boolean pccc_attrib_is_accepted = FALSE;
  boolean slotted_mode_attrib_is_accepted = FALSE; 
  /* Flag if we should fail negotiation based on this config response */
  boolean negotiation_failed = FALSE; 

  /* Flag if there was an error with the message structure */
  boolean message_parsing_error = FALSE; 

#ifdef FEATURE_HDR_REVC
  boolean multi_page_attrib_is_accepted = FALSE;
  boolean multi_page_attrib_supported = FALSE;
#endif /* FEATURE_HDR_REVC */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = dsm_length_packet( item_ptr ) *
                         HDRIDLEC_BITS_PER_BYTE;

  /* Extract the Transaction ID from the message */
  if (HDRIDLEC_UNPACK_TRANS_ID(item_ptr, &trans_id) != E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigRsp Msg", 0, 0, 0 );
    return;
  }

  /* check transaction id matches the transaction id sent in the
     configuration request message or not
  */

  if(trans_id != hdridlec.config_req_msg.trans_id)
  {
    HDRTRACE_RXMSG_IGNORED_SEQ(HDRHAI_STREAM_PROTOCOL,
     0x51, 0); //lint !e506

    HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "IgnoreConfigRes-TransID exp =%d rcv=%d",
                    hdridlec.config_req_msg.trans_id,trans_id);
    return;
  }

  attrib_len = HDRIDLEC_MSG_ID_SIZE + HDRIDLEC_TRANSACTION_ID_SIZE;

  while ((total_bits_remaining - attrib_len) >= HDRIDLEC_MIN_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    if (HDRIDLEC_UNPACK_CON_RSP_LEN(item_ptr, &attrib_len, attrib_offset) !=
        E_SUCCESS)
    {
      ERR("Error unpacking Configuration Response message, ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in 
     * bytes, excluding the length of the length field.  For our purposes, 
     * we want attrib_len to be the total length of the attribute in bits, 
     * including the length field.
     */
    attrib_len *= HDRIDLEC_BITS_PER_BYTE;
    attrib_len += HDRIDLEC_ATTRIB_LEN_SIZE;

    /* In a config response, all attributes are length, attrib_id, value_id */
    if ((attrib_len != HDRIDLEC_CRSP_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining))
    {
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    if (HDRIDLEC_UNPACK_CON_RSP_ATTRIBUTE_ID(item_ptr, &attribute_id,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking creq_attrib_id",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    if (HDRIDLEC_UNPACK_CON_RSP_VALUE_ID(item_ptr, &value_id,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking creq_value_id",0,0,0);
      message_parsing_error = TRUE;
      break;
    }

    switch (attribute_id)
    {
      case HDRIDLEC_PCCC_CONFIG_ATTRIB_ID:
        if ((hdridlec.config_req_msg.pccc_is_requested) && 
            (value_id == hdridlec.config_req_msg.pccc_value_id))
        {
          pccc_attrib_is_accepted = TRUE;
        }
        else
        {
          /* Accepting a different ValueID than we proposed is a failure */
          negotiation_failed = TRUE;
        }
        break;

      case HDRIDLEC_SLOTTED_MODE_ATTRIB_ID:
        if ((hdridlec.config_req_msg.slotted_mode_is_requested) && 
            (value_id == HDRIDLEC_SLOTTED_MODE_VALUE_ID))
        {
          /* We may have more than one attribute in this accept message */
          slotted_mode_attrib_is_accepted = TRUE;
        }
        else
        {
          /* Accepting a different ValueID than we proposed is a failure */
          negotiation_failed = TRUE;
        }
        break;

#ifdef FEATURE_HDR_REVC
      case HDRIDLEC_MULTI_PAGE_SUPPORTED_ATTRIB_ID:
        if ((hdridlec.config_req_msg.multi_page_is_requested) &&
            ((value_id == HDRIDLEC_MULTI_PAGE_UNSUPPORTED) ||
             (value_id == HDRIDLEC_MULTI_PAGE_SUPPORTED)))
        {
          /* set MultiATIPage to value in AN's ConResp */
          multi_page_attrib_supported = value_id;
          multi_page_attrib_is_accepted = TRUE;
        }
        else
        {
          /* Didn't propose MultiPage, or received a invalid value in resp */
          HDR_MSG_PROT_2( MSG_LEGACY_ERROR, 
                          "Wrong MultiPageSup Resp, Req %d Val %d",
                          hdridlec.config_req_msg.multi_page_is_requested, 
                          value_id );
          negotiation_failed = TRUE;
        }
        break;
#endif /* FEATURE_HDR_REVC */

      default:
        ERR("Unknown Attribute ID %d",attribute_id,0,0);
        /* Accepting an attribute we never negotiated is grounds for failure */
        negotiation_failed = TRUE;
        break;
    } /* switch */

    if ((negotiation_failed) || (message_parsing_error))
    {
      break;
    }
  }

  if (message_parsing_error)
  {
    /* If we had an unexpected problem parsing the message then it is better
     * to just ignore it.  If the AN wants to send a valid response it still 
     * can, or else our config response timer will expire and we will abort.
     */
  }
  else if (negotiation_failed)
  {
    hdridlec_fail_cur_config_neg(HDRHAI_CLOSE_PROT_NEG_FAIL, attribute_id);
  }
  else
  {
    if (hdridlec.await_slp_config_req_cb)
    {
      /* The flag must be set first because hdrhmp_abort_msg() causes
       * the callback to be called, and if this flag is not FALSE
       * the callback will set the config response timer, for example.
       */
      hdridlec.await_slp_config_req_cb = FALSE;
      (void) hdrhmp_abort_msg(hdridlec_config_req_msg_tx_outcome_cb, NULL);
    }

    /* Message is a response to our config request. Stop and clear timer */
    hdridlec_abort_cur_config_neg();

    if (pccc_attrib_is_accepted)
    {
      /* AN accepts the proposed configuration */
      hdridlec.negotiated.pccc = hdridlec.config_req_msg.pccc;
    }
    else if (hdridlec.config_req_msg.pccc_is_requested)
    {
      /* AN refused our request. Keep negotiated values unchanged */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "AN did not accept PCCC request!");
    }
  
    if (slotted_mode_attrib_is_accepted)
    {
      /* Set hdridlec.negotiated.slotted_mode to the values we proposed */
      /* Shorthand pointer to where to set negotiated slotted mode attrib info */
      hdridlec.negotiated.slotted_mode = hdridlec.desired_slotted_mode_config; 
    }
    else if (hdridlec.config_req_msg.slotted_mode_is_requested)
    {
      /* AN refused our request. Keep negotiated values unchanged */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "AN did not accept SlottedMode request!");
    }

#ifdef FEATURE_HDR_REVC
    if (multi_page_attrib_is_accepted)
    {
      hdridlec.negotiated.multi_page_supported = multi_page_attrib_supported; 
    }
    else if (hdridlec.config_req_msg.multi_page_is_requested)
    {
      /* AN refused our request. Keep negotiated values unchanged */
      HDR_MSG_PROT (MSG_LEGACY_HIGH, "AN did not accept MultiPage request!");
    }
#endif /* FEATURE_HDR_REVC */

    hdrind_give_ind(HDRIND_IDLE_CONFIG_COMPLETE,NULL);
  }

} /* hdridlec_process_config_res_msg() */
#endif /* PERLUTF_TDTGEN_EXCLUDE */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_PCCC_CONFIG_ATTRIB
DESCRIPTION
  This function processes the preferred control channel cycle configuration
  attribute of the Configuration Request Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  value_id_ptr - Pointer to location to write value ID of accepted attribute
  pccc_config_ptr - Pointer to struct to put the negotiated value into

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, value_id is returned through value_id_ptr, and the
  acceptable value is returned through pccc_config_ptr.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdridlec_process_pccc_config_attrib
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_id_ptr,
  hdridlec_pccc_config_attrib_type *pccc_config_ptr
)
{
  /* Bit offset of current attribute value from the beginning of the
   * current attribute */
  uint16 attrib_val_offset;
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 attrib_value_id; /* Value ID associated with current value */
  hdridlec_pccc_config_attrib_type attrib_value; /* current attribute value */
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRIDLEC_ATTRIB_LEN_SIZE + HDRIDLEC_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + HDRIDLEC_PCCC_CONFIG_ATTRIB_MIN_VAL_SIZE) <=
         attrib_len)
  {

    /* There is at least one more attribute value in this attribute */
    err  = HDRIDLEC_UNPACK_CON_REQ_PCCC_VALUE_ID(
             item_ptr, &attrib_value_id,
             attrib_offset, attrib_val_offset );

    err |= HDRIDLEC_UNPACK_CON_REQ_PCCC_ENABLED(
             item_ptr, &attrib_value.is_custom,
             attrib_offset, attrib_val_offset );

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",
            0,0,0);
      break;
    }

    if(attrib_value.is_custom)
    {
       if((attrib_val_offset + HDRIDLEC_PCCC_CONFIG_ATTRIB_MAX_VAL_SIZE) <=
          attrib_len)
       {
          err = HDRIDLEC_UNPACK_CON_REQ_PCCC_VALUE(
                  item_ptr, &attrib_value.value,
                  attrib_offset, attrib_val_offset );
       }
       else
       {
         ERR("Error unpacking Configuration Request message, ignoring",
              0,0,0);
         break;
       }

    }
    else
    {
      attrib_value.value  = 0;
    }

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",
           0,0,0);
      break;
    }

    /* Really the AN should not propose this attribute, or at most 
     * propose with PreferredControlChannelCycleEnabled = 0.
     */

    if (attrib_value.is_custom)
    {
      /* If the AN proposes a custom PCCC, that is not acceptable */
    }
    else
    {
      val_is_acceptable = TRUE;
    }

    if (val_is_acceptable)
    {
       *value_id_ptr = attrib_value_id;
#ifdef FEATURE_MEMCPY_REMOVAL
       *pccc_config_ptr = attrib_value;
#else
       memcpy(pccc_config_ptr, &attrib_value,
              sizeof(hdridlec_pccc_config_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

       /* Leave the "default" pccc value un-computed at this time
        * in case the max_sci changes before we finish configuring.
        * We will fill in the value when we finish negotiation.
        */
       break;
    }

    /* update the attrib_val_offset for the next attrib value record */
    if (attrib_value.is_custom)
    {
      attrib_val_offset += HDRIDLEC_PCCC_CONFIG_ATTRIB_MAX_VAL_SIZE;
    }
    else
    {
      attrib_val_offset += HDRIDLEC_PCCC_CONFIG_ATTRIB_MIN_VAL_SIZE;
    }
  } /* while */

  return (val_is_acceptable);
} /* hdridlec_process_pccc_config_attrib */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_SLOTTED_MODE_ATTRIB
DESCRIPTION
  This function processes the preferred control channel cycle configuration
  attribute of the Configuration Request Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  value_id_ptr - Pointer to location to write value ID of accepted attribute
  slotted_mode_ptr - Pointer to struct to put the negotiated value into
  is_gaup - If this Slotted Mode attribute is from a GAUP message

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, value_id is returned through value_id_ptr, and the
  acceptable value is returned through slotted_mode_ptr.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdridlec_process_slotted_mode_attrib
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_id_ptr,
  hdridlec_slotted_mode_type *slotted_mode_ptr,
  boolean is_gaup
)
{
  /* Bit offset of current attribute value from the beginning of the
   * current attribute */
  uint16 attrib_val_offset;
  uint16 offset; /* Total bit offset in item to unpack from */
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 attrib_value_id; /* Value ID associated with current value */
  hdridlec_slotted_mode_type attrib_value; /* current attribute value */
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRIDLEC_ATTRIB_LEN_SIZE + HDRIDLEC_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + HDRIDLEC_SLOTTED_MODE_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {

    offset = attrib_offset + attrib_val_offset;

    /* There is at least one more attribute value in this attribute */

    err = HDRBIT_UNPACK8(item_ptr, &attrib_value_id, offset, 
                          HDRIDLEC_VALUE_ID_SIZE);
    offset += HDRIDLEC_VALUE_ID_SIZE;

    /* Slot Cycle 1 */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.slot_cycle1, offset, 
                          HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE);
    offset += HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE;

    /* Slot Cycle 2 */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.slot_cycle2, offset, 
                          HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE);
    offset += HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE;

    /* Slot Cycle 3 */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.slot_cycle3, offset, 
                          HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE);
    offset += HDRIDLEC_SLOTTED_MODE_SLOT_CYCLE_SIZE;

    /* Wake Count 1 */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.wake_count1, offset, 
                          HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE);
    offset += HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE;

    /* Wake Count 2 */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.wake_count2, offset, 
                          HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE);
    offset += HDRIDLEC_SLOTTED_MODE_WAKE_COUNT_SIZE;


    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",
            0,0,0);
      break;
    }

    /* Verify that the received attribute is acceptable */
    if ((attrib_value.slot_cycle1 <= attrib_value.slot_cycle2) &&
        (attrib_value.slot_cycle2 <= attrib_value.slot_cycle3))
    {
       /* If the SmallSlotCycleAllowed attribute is set to 0x00, then the 
          AN and the AT shall not send an AttributeUpdateRequest message 
          proposing a value of the SlotCycle1 field of the SlottedMode 
          attribute that is less than 0x06. */
       if (is_gaup && (attrib_value.slot_cycle1 < 0x06) && 
           !hdridlec_is_small_slotcycle_gaup_allowed())
       {
         break;
       }

      /* AT wants to explicitly reject the Slotted Mode attribute with SCI < 5
       * if it's sent during AN initiated phase. However if it's sent in a 
       * GAUP message, AT can't reject it, and we don't want to close the 
       * session just because of a small SCI value either. So AT will accept
       * the GAUP with Slotted Mode Attribute and pass it to HDR Search. */
      if ((attrib_value.slot_cycle1 >= HDRIDLEC_MIN_SLOT_CYCLE) ||
           is_gaup )
      {
        val_is_acceptable = TRUE;
        *value_id_ptr = attrib_value_id;
#ifdef FEATURE_MEMCPY_REMOVAL
        *slotted_mode_ptr = attrib_value;
#else
        memcpy(slotted_mode_ptr, &attrib_value,
               sizeof(hdridlec_slotted_mode_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
        break;
      }
    }

    attrib_val_offset += HDRIDLEC_SLOTTED_MODE_ATTRIB_VAL_SIZE;

  } /* while */

  return (val_is_acceptable);
} /* hdridlec_process_slotted_mode_attrib */



/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_QUICK_PAGING_ATTRIB

DESCRIPTION
  This function processes the Quick Paging configuration attribute of the 
  Configuration Request Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  attrib_len - Length, in bits, of the attribute, including the length field
  attrib_offset - Bit offset of the attribute from the beginning of the message
  value_id_ptr - Pointer to location to write value ID of accepted attribute
  quick_paging_ptr - Pointer to struct to put the negotiated value into
  is_gaup - If this Slotted Mode attribute is from a GAUP message

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, value_id is returned through value_id_ptr, and the
  acceptable value is returned through quick_paging_ptr.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdridlec_process_quick_paging_attrib
(
  dsm_item_type * item_ptr,
  uint16 attrib_len,
  uint16 attrib_offset,
  uint8 *value_id_ptr,
  hdridlec_quick_paging_attrib_type *quick_paging_ptr,
  boolean is_gaup
)
{
  /* Bit offset of current attribute value from the beginning of the
   * current attribute */
  uint16 attrib_val_offset;
  uint16 offset; /* Total bit offset in item to unpack from */
  boolean val_is_acceptable = FALSE;  /* If we accept the attribute */
  uint8 attrib_value_id; /* Value ID associated with current value */
  hdridlec_quick_paging_attrib_type attrib_value; /* current attribute value */
  int err; /* sum of errno return vals, depends that E_SUCCESS is zero */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The first attribute value is after the attribute length and ID fields */
  attrib_val_offset = HDRIDLEC_ATTRIB_LEN_SIZE + HDRIDLEC_ATTRIB_ID_SIZE;

  while ((attrib_val_offset + HDRIDLEC_QUICK_PAGING_ATTRIB_VAL_SIZE) <=
         attrib_len)
  {

    offset = attrib_offset + attrib_val_offset;

    /* There is at least one more attribute value in this attribute */

    err = HDRBIT_UNPACK8(item_ptr, &attrib_value_id, offset, 
                          HDRIDLEC_VALUE_ID_SIZE);
    offset += HDRIDLEC_VALUE_ID_SIZE;

    /* QuickPagingEnabled */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.quick_paging_enabled, offset, 
                          HDRIDLEC_QUICKPAGING_QP_ENABLED_SIZE);
    offset += HDRIDLEC_QUICKPAGING_QP_ENABLED_SIZE;

    /* SunSyncQuickPaging */
    err |= HDRBIT_UNPACK8(item_ptr, &attrib_value.subsync_quick_paging, offset, 
                          HDRIDLEC_QUICKPAGING_SUBSYNC_QP_ENABLED_SIZE);
    offset += HDRIDLEC_QUICKPAGING_SUBSYNC_QP_ENABLED_SIZE;

    if (err)
    {
      ERR("Error unpacking Configuration Request message, ignoring",
            0,0,0);
      break;
    }

    if (attrib_value.subsync_quick_paging && !attrib_value.quick_paging_enabled)
    {
      val_is_acceptable = FALSE;
    }
    else
    {
      val_is_acceptable = TRUE;
    }
    *value_id_ptr = attrib_value_id;
#ifdef FEATURE_MEMCPY_REMOVAL
    *quick_paging_ptr = attrib_value;
#else
    memcpy(quick_paging_ptr, &attrib_value, sizeof(hdridlec_quick_paging_attrib_type));
#endif /* FEATURE_MEMCPY_REMOVAL */

    attrib_val_offset += HDRIDLEC_QUICK_PAGING_ATTRIB_VAL_SIZE;

  } /* while */

  return (val_is_acceptable);
} /* hdridlec_process_quick_paging_attrib */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_SIMPLE_ATTRIB

DESCRIPTION
  This function process simple attributes from a Configuration Request
  message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr      - Pointer to the DSM item chain holding the message.
  attrib_len    - Length in bits of the attribute, including the length field.
  attrib_offset - Bit offset of the attribute from the beginning of the message.
  value_ptr     - Pointer to location to write value of accepted attribute.

RETURN VALUE
  TRUE if a new, acceptable value for the attribute was found, FALSE otherwise.
  If return value is TRUE, the value is returned through value_ptr.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdridlec_process_simple_attrib
(
  dsm_item_type *const item_ptr, /* IN */
  uint16         const attrib_len,
  uint16         const attrib_offset,
  uint8          const attrib_id,
  boolean        const attrib_is_large,
  uint16        *const value_ptr /* OUT */
)
{
  uint16 attrib_val_offset; /* Offset to begin unpacking attrib */
  boolean val_is_acceptable = FALSE; /* Return value; default to FALSE */
  uint16 attrib_val; /* 16 bit simple attrib value */
  
  uint16 attrib_val_size; /* Size of the simple attrib value */

  int err; /* Traps any error codes */
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The first value is after the length and ID fields */
  attrib_val_offset = HDRIDLEC_ATTRIB_LEN_SIZE + 
                      HDRIDLEC_ATTRIB_ID_SIZE;

  if( attrib_is_large )
  {
    attrib_val_size = HDRIDLEC_LARGE_SIMPLE_ATTRIB_VAL_SIZE;
  }
  else
  {
    attrib_val_size = HDRIDLEC_SMALL_SIMPLE_ATTRIB_VAL_SIZE;
  }

  while( attrib_val_offset + attrib_val_size <= attrib_len )
  {
    switch( attrib_id )
    {
      case HDRIDLEC_SMALL_SLOT_CYCLE_ALLOWED_ATTRIB_ID:
        err = HDRBIT_UNPACK16( item_ptr, &attrib_val, 
                attrib_val_offset + attrib_offset,
                HDRIDLEC_SMALL_SIMPLE_ATTRIB_VAL_SIZE );

        /* Verify that the received attribute is acceptable */
        if( !err && ((attrib_val == 0x00) || (attrib_val == 0x01)) )
        {
          val_is_acceptable = TRUE;
          *value_ptr = attrib_val;
        }

        break;

      case HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_ID:
        err = HDRBIT_UNPACK16( item_ptr, &attrib_val,
                attrib_val_offset + attrib_offset,
                HDRIDLEC_LARGE_SIMPLE_ATTRIB_VAL_SIZE );

        /* Accept the first value */
        if( !err )
        {
          val_is_acceptable = TRUE;
          *value_ptr = attrib_val;
        }
        
        break;

      case HDRIDLEC_CONNECTION_DENY_BACKOFF_ATTRIB_ID:
        err = HDRBIT_UNPACK16( item_ptr, &attrib_val, 
                attrib_val_offset + attrib_offset,
                HDRIDLEC_SMALL_SIMPLE_ATTRIB_VAL_SIZE );

        /* Verify that the received attribute is acceptable */
        if( !err && (attrib_val == 0xFF) )
        {
          /* Currently we accept only default value for ConnectionDenyBackoff */
          val_is_acceptable = TRUE;
          *value_ptr = attrib_val;
        }

        break;

      case HDRIDLEC_PAGERESPONSE_APERSISTENCE_SUPPORTED_ATTRIB_ID:
        err = HDRBIT_UNPACK16( item_ptr, &attrib_val, 
                attrib_val_offset + attrib_offset,
                HDRIDLEC_SMALL_SIMPLE_ATTRIB_VAL_SIZE );

        /* Verify that the received attribute is acceptable */
        if( !err && (attrib_val == 0x00) )
        {
          /* Currently we accept only default value */
          val_is_acceptable = TRUE;
          *value_ptr = attrib_val;
        }

        break;

    }

    /* If we've found an acceptable value then exit */
    if( val_is_acceptable )
    {
      break;
    }

    /* Continues onto the next value ... */
    attrib_val_offset += attrib_val_size;

  } /* end while( attrib_val_offset + attrib_val_size <= attrib_len ) */

  return val_is_acceptable;

} /* hdridlec_process_simple_attrib() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_CONFIG_REQ_MSG

DESCRIPTION
  This function processes the Configuration Request Message.

DEPENDENCIES
  The MSG_ID is not checked.  This function assumes that the given message
    is the Configuration Request Message.

PARAMETERS
  item_ptr - pointer to the received message
  is_gaup - If this is an AttributeUpdateRequest message

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdridlec_process_config_req_msg
(
  dsm_item_type * item_ptr,
  boolean is_gaup
)
{
  int    total_bits_remaining; /* number of bits remaining in the message */
  uint8  creq_transaction_id; /* Transaction ID of the message */

  uint8  attrib_len_in_bytes = 0;
  uint16 attrib_len; /* Bit length of attribute, including the length field */
  uint16 attrib_offset = 0; /* Bit offset of current attribute */
  uint8  attrib_id; /* ID field of attribute */
  /* If during processing we end up in a situation where we don't know what
   * attribute ID the AN included, or aren't sure if they meant to include
   * one, we can't send a response because we may be accepting to use the
   * default value for an attribute that we are not actually using the
   * default value for (e.g. if we negotiated a non-default value during
   * AT initiated negotiation).
   */
  boolean send_response = TRUE;

  /* As we parse the message we update the negotiated parameters in a
   * local copy as we agree to accept new values.  That way, if we
   * later encounter an error and decide not to send a response, or if
   * the same attribute is negotiated twice and we accept the first but
   * decline the second, we don't corrupt the real negotiated parameters.
   */

  /* For each simple attribute we need a flag if we are accpeting it
   * and the value we are accepting.
   */

  /* For each complex attribute we need a flag if we are accpeting it,
   * the ValueID we are accepting, and a structure holding the accepted value.
   */
  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdridlec_pccc_config_attrib_type value;
  } pccc_config_attrib = { FALSE, 0, { FALSE, 0 } };

  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdridlec_slotted_mode_type value;
  } slotted_mode_attrib = { FALSE, 0, { 0, 0, 0, 0, 0 } };

  struct
  {
    boolean is_accepted;
    uint16 value;
  } small_slot_cycle_allowed_attrib = { FALSE, 0 };

  struct
  {
    boolean is_accepted;
    uint16 value;
  } access_hashing_channel_mask_attrib = { FALSE, 0 };

  struct
  {
    boolean is_accepted;
    uint16 value;
  } connection_deny_backoff_attrib = { FALSE, 0 };

  struct
  {
    boolean is_accepted;
    uint16 value;
  } pageresponse_apersistence_supported_attrib = { FALSE, 0 };

  struct
  {
    boolean is_accepted;
    uint8 value_id;
    hdridlec_quick_paging_attrib_type value;
  } quick_paging_attrib = { FALSE, 0, {FALSE, FALSE} };

#ifdef FEATURE_HDR_REVC
  struct
  {
    boolean is_accepted;
    uint8 value_id;
  } multi_page_attrib = { FALSE, 0 };
#endif /* FEATURE_HDR_REVC */

  boolean pccc_config_attrib_is_included = FALSE;
  boolean slotted_mode_attrib_is_included = FALSE;
  boolean small_slot_cycle_allowed_attrib_is_included = FALSE;
  boolean access_hashing_channel_mask_attrib_is_included = FALSE;
  boolean connection_deny_backoff_attrib_is_included = FALSE;
  boolean pageresponse_apersistence_supported_attrib_is_included = FALSE;
  boolean unknown_attrib_is_included = FALSE;
  boolean quick_paging_attrib_is_included = FALSE;
  hdrhai_protocol_instance_enum_type protocol_instance;
#ifdef FEATURE_HDR_REVC
  boolean multi_page_attrib_is_included = FALSE;
#endif /* FEATURE_HDR_REVC */

  dsm_item_type * send_msg_ptr; /* dsm buffer for sent message */

  /* The InConfig IDLE subtype */
  uint16 subtype = hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL );

  boolean gaup_is_accepted = TRUE;
    /* Flag to indicate if GAUP message from AN is accepted or not */
  uint8 gaup_failed_attrib_id = 0;
    /* Attribute that AN GAUP'ed and AT rejected */
  static hdrscp_scp_fail_ind_type gaup_fail_data;
    /* Data for AN GAUP failed indication */
  hdrind_ind_data_union_type ind_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = (dsm_length_packet(item_ptr) *
                         HDRIDLEC_BITS_PER_BYTE);

  /* Extract the Transaction ID */
  if (HDRIDLEC_UNPACK_TRANS_ID(item_ptr, &creq_transaction_id ) !=
      E_SUCCESS)
  {
    ERR("Error extracting TransactionID from ConfigReq Msg",0,0,0);
    /* Without the transaction ID, it makes no sense to send a response */
    return;
  }

  /* initalize the configuration is acceptable to FALSE */
  pccc_config_attrib.is_accepted = FALSE;
  slotted_mode_attrib.is_accepted = FALSE;
  access_hashing_channel_mask_attrib.is_accepted = FALSE;

  /* Initialize attrib_len so the initial attrib_offset will be
   * immediately after the transaction ID of the config request.
   */
  attrib_len = HDRIDLEC_MSG_ID_SIZE + HDRIDLEC_TRANSACTION_ID_SIZE;

  /* Unpack the configuration attributes */

  /* While there are at least enough bits for the smallest attribute... */
  while ((total_bits_remaining - attrib_len) >= HDRIDLEC_MIN_ATTRIB_LEN)
  {
    /* Move to the next attribute record */
    attrib_offset += attrib_len;
    total_bits_remaining -= attrib_len;

    /* Get the length of the new attribute */
    if (HDRIDLEC_UNPACK_CON_REQ_LEN(item_ptr, &attrib_len_in_bytes,
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking ConfigReq Msg", 0, 0, 0 );
      /* Without the length we can't continue to parse the message. */
      send_response = FALSE;
      break;
    }

    /* The AttributeLength field in the message is the attribute length in
     * bytes, excluding the length of the length field.  For our purposes,
     * we want attrib_len to be the total length of the attribute in bits,
     * including the length field.
     */
    attrib_len = attrib_len_in_bytes * HDRIDLEC_BITS_PER_BYTE;
    attrib_len += HDRIDLEC_ATTRIB_LEN_SIZE;

    /* Any attribute record must have at least an attribute ID */
    if ((attrib_len < HDRIDLEC_MIN_ATTRIB_LEN) ||
        (attrib_len > total_bits_remaining))
    {
      /* If the length is something unexpected, we can't be sure of what
       * the AN intended and it is safest to not send a response
       */
      ERR("Error, invalid total len vs bits in message. ignoring",0,0,0);
      send_response = FALSE;
      break;
    }

    /* There is at least one more attribute record in the message */

    if (HDRIDLEC_UNPACK_CON_REQ_ATTRIBUTE_ID(item_ptr, &(attrib_id),
        attrib_offset) != E_SUCCESS)
    {
      ERR("Error unpacking Configuration Request message, ignoring",0,0,0);
      /* If we can't get the attribute_id, don't send a response */
      send_response = FALSE;
      break;
    }

    /* If the default idle state subtype has been negotiated */
    if( subtype == HDRSCP_DEFAULT_IDLE )
    {
      /* If the attrib being negotiated is not PreferredControlChannelCycle */
      if( attrib_id != HDRIDLEC_PCCC_CONFIG_ATTRIB_ID )
      {
        /* Flag the attrib as unknown and continue to the next */
        ERR("Unknown Attribute ID %d",attrib_id,0,0);
        unknown_attrib_is_included = TRUE;
        continue;
      }
    }

    switch (attrib_id)
    {
      case HDRIDLEC_PCCC_CONFIG_ATTRIB_ID:
        pccc_config_attrib_is_included = TRUE;
        if (attrib_len >= HDRIDLEC_PCCC_CONFIG_MIN_ATTRIB_LEN)
        {
          pccc_config_attrib.is_accepted =
            hdridlec_process_pccc_config_attrib(
               item_ptr, attrib_len, attrib_offset,
               &pccc_config_attrib.value_id,
               &pccc_config_attrib.value );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        break;

      case HDRIDLEC_SLOTTED_MODE_ATTRIB_ID:
        slotted_mode_attrib_is_included = TRUE;
        if (attrib_len >= HDRIDLEC_SLOTTED_MODE_MIN_ATTRIB_SIZE)
        {
          slotted_mode_attrib.is_accepted =
            hdridlec_process_slotted_mode_attrib(
               item_ptr, attrib_len, attrib_offset,
               &slotted_mode_attrib.value_id,
               &slotted_mode_attrib.value,
               is_gaup );
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        break;

      case HDRIDLEC_SMALL_SLOT_CYCLE_ALLOWED_ATTRIB_ID:
        small_slot_cycle_allowed_attrib_is_included = TRUE;
        if( attrib_len >= HDRIDLEC_SMALL_SIMPLE_ATTRIB_MIN_VAL_SIZE )
        {
          small_slot_cycle_allowed_attrib.is_accepted =
            hdridlec_process_simple_attrib( item_ptr, attrib_len, attrib_offset, 
              attrib_id, FALSE, &small_slot_cycle_allowed_attrib.value );
          /* FALSE indicates parsing for an 8 bit (small) simple attribute */
        }
        else
        {
          ERR("Not enough bits for attribute %d",attrib_id,0,0);
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        break;

      case HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_ID:
        access_hashing_channel_mask_attrib_is_included = TRUE;
        if( attrib_len >= HDRIDLEC_LARGE_SIMPLE_ATTRIB_MIN_VAL_SIZE )
        {
          access_hashing_channel_mask_attrib.is_accepted =
            hdridlec_process_simple_attrib( item_ptr, attrib_len, attrib_offset,
              attrib_id, TRUE, &access_hashing_channel_mask_attrib.value );
          /* TRUE indicates parsing for a 16 bit (large) simple attribute */
        }
        else
        {
          ERR( "Not enough bits for attribute %d", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        break;

      case HDRIDLEC_CONNECTION_DENY_BACKOFF_ATTRIB_ID:
        connection_deny_backoff_attrib_is_included = TRUE;
        if( attrib_len >= HDRIDLEC_SMALL_SIMPLE_ATTRIB_MIN_VAL_SIZE )
        {
          connection_deny_backoff_attrib.is_accepted =
            hdridlec_process_simple_attrib( item_ptr, attrib_len, attrib_offset,
              attrib_id, FALSE, &connection_deny_backoff_attrib.value );
          /* FALSE indicates parsing for an 8 bit (small) simple attribute */
        }
        else
        {
          ERR( "Not enough bits for attribute %d", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        break;

    case HDRIDLEC_PAGERESPONSE_APERSISTENCE_SUPPORTED_ATTRIB_ID:
      pageresponse_apersistence_supported_attrib_is_included = TRUE;
      if( attrib_len >= HDRIDLEC_SMALL_SIMPLE_ATTRIB_MIN_VAL_SIZE )
      {
        pageresponse_apersistence_supported_attrib.is_accepted =
          hdridlec_process_simple_attrib( item_ptr, attrib_len, attrib_offset,
            attrib_id, FALSE, &pageresponse_apersistence_supported_attrib.value );
        /* FALSE indicates parsing for an 8 bit (small) simple attribute */
      }
      else
      {
        ERR( "Not enough bits for attribute %d", attrib_id, 0, 0 );
        /* If the AN didn't include enough bits for at least one complete
         * attribute value we don't know what the AN intended, so it is
         * safest not to send a response.
         */
        send_response = FALSE;
      }
      break;

      case HDRIDLEC_QUICK_PAGING_ATTRIB_ID:
        quick_paging_attrib_is_included = TRUE;
        if( attrib_len >= HDRIDLEC_QUICK_PAGING_MIN_ATTRIB_SIZE )
        {
          quick_paging_attrib.is_accepted =
            hdridlec_process_quick_paging_attrib(                
               item_ptr, attrib_len, attrib_offset,
               &quick_paging_attrib.value_id,
               &quick_paging_attrib.value,
               is_gaup );
        }
        else
        {
          ERR( "Not enough bits for attribute %d", attrib_id, 0, 0 );
          /* If the AN didn't include enough bits for at least one complete
           * attribute value we don't know what the AN intended, so it is
           * safest not to send a response.
           */
          send_response = FALSE;
        }
        break;

#ifdef FEATURE_HDR_REVC
      case HDRIDLEC_MULTI_PAGE_SUPPORTED_ATTRIB_ID:        
        multi_page_attrib_is_included = TRUE;

        if ((hdrscp_get_config_subtype(HDRHAI_IDLE_STATE_PROTOCOL) != 
             HDRSCP_DEFAULT_IDLE) ||
            !hdrmc_feature_is_enabled(HDRMC_FEATURE_MULTI_AT_PAGE))
        {
          HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                       "Recv MultiPage w/ feature disabled or not QuickIdle");

          multi_page_attrib.is_accepted = FALSE;
          break;
        }        
        
        while (attrib_len >= HDRIDLEC_VALUE_ID_SIZE)
        {
          if (HDRBIT_UNPACK8(item_ptr, &multi_page_attrib.value_id, 
                             attrib_offset, HDRIDLEC_VALUE_ID_SIZE) != 
              E_SUCCESS)
          {
            HDR_MSG_PROT(MSG_LEGACY_ERROR, 
                           "Error unpacking MultiPage attrib");
            multi_page_attrib.is_accepted = FALSE;
            break;
          }

          if (multi_page_attrib.value_id == 
              HDRIDLEC_MULTI_PAGE_SUPPORTED)
          {            
            multi_page_attrib.is_accepted = TRUE;
            /* if received most preferred value_id, skip checking the rest */
            break;
          }
          else if (multi_page_attrib.value_id == 
                   HDRIDLEC_MULTI_PAGE_UNSUPPORTED)
          {
            multi_page_attrib.is_accepted = TRUE;
          }
          attrib_len -= HDRIDLEC_VALUE_ID_SIZE;
        }
        break;
#endif /* FEATURE_HDR_REVC */

      default:
        ERR("Unknown Attribute ID %d",attrib_id,0,0);
        unknown_attrib_is_included = TRUE;
        break;
    } /* switch */

    if (!send_response)
    {
      /* No need to continue parsing once we decide not to send a response */
      break;
    }
  } /* while */

  if (is_gaup)
  {
    /* AN should not send PCCC or SmallSlotCycleAllowed via GAUP.
     * Reject by not responding. */
    if (pccc_config_attrib_is_included ||
        small_slot_cycle_allowed_attrib_is_included ||
        connection_deny_backoff_attrib_is_included ||
        pageresponse_apersistence_supported_attrib_is_included ||
        unknown_attrib_is_included     ||
        (slotted_mode_attrib_is_included && 
         !slotted_mode_attrib.is_accepted) ||
        (access_hashing_channel_mask_attrib_is_included && 
         !access_hashing_channel_mask_attrib.is_accepted) ||
        (quick_paging_attrib_is_included && 
         !quick_paging_attrib.is_accepted))
    {
      send_response = FALSE;
      gaup_is_accepted = FALSE;

      if (slotted_mode_attrib_is_included && 
          !slotted_mode_attrib.is_accepted)
      {
        gaup_failed_attrib_id = HDRIDLEC_SLOTTED_MODE_ATTRIB_ID;
      }
      else if (access_hashing_channel_mask_attrib_is_included && 
               !access_hashing_channel_mask_attrib.is_accepted)
      {
        gaup_failed_attrib_id = HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_ID;
      }
      else if (pccc_config_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRIDLEC_PCCC_CONFIG_ATTRIB_ID;
      }
      else if (small_slot_cycle_allowed_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRIDLEC_SMALL_SLOT_CYCLE_ALLOWED_ATTRIB_ID;
      }
      else if (connection_deny_backoff_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRIDLEC_CONNECTION_DENY_BACKOFF_ATTRIB_ID;
      }
      else if (pageresponse_apersistence_supported_attrib_is_included)
      {
        gaup_failed_attrib_id = HDRIDLEC_PAGERESPONSE_APERSISTENCE_SUPPORTED_ATTRIB_ID;
      }
      else if (quick_paging_attrib_is_included && 
               !quick_paging_attrib.is_accepted)
      {
        gaup_failed_attrib_id = HDRIDLEC_QUICK_PAGING_ATTRIB_ID;
      }

    }
  }

  if (!is_gaup)
  {
    if ((pccc_config_attrib_is_included) && (!pccc_config_attrib.is_accepted))
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.pccc = hdridlec.current.pccc;
    }

    if ((slotted_mode_attrib_is_included) && (!slotted_mode_attrib.is_accepted))
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.slotted_mode = hdridlec.current.slotted_mode;
    }

    if ((small_slot_cycle_allowed_attrib_is_included) && 
        (!small_slot_cycle_allowed_attrib.is_accepted))
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.small_slot_cycle_is_allowed = 
        hdridlec.current.small_slot_cycle_is_allowed;
    }

    if ((access_hashing_channel_mask_attrib_is_included) && 
         (!access_hashing_channel_mask_attrib.is_accepted))
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.access_hashing_channel_mask = 
        hdridlec.current.access_hashing_channel_mask;
    }

    if ((connection_deny_backoff_attrib_is_included) && 
        (!connection_deny_backoff_attrib.is_accepted))
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.connectiondeny_backoff = 
          hdridlec.current.connectiondeny_backoff;
    }

    if (pageresponse_apersistence_supported_attrib_is_included &&
        !pageresponse_apersistence_supported_attrib.is_accepted)
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.pageresponse_apersistence_supported = 
          hdridlec.current.pageresponse_apersistence_supported;
    }

    if (quick_paging_attrib_is_included && !quick_paging_attrib.is_accepted)
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.quick_paging = hdridlec.current.quick_paging;
    }

#ifdef FEATURE_HDR_REVC
    if (multi_page_attrib_is_included && !multi_page_attrib.is_accepted)
    {
      /* Fall back to the default, which is the last agreed upon value */
      hdridlec.negotiated.multi_page_supported = 
        hdridlec.current.multi_page_supported;
    }
#endif /* FEATURE_HDR_REVC */
  }

  /* Form and send the Configuration Response message. */
  if (send_response)
  {
    /* get new buffer */
    send_msg_ptr = hdrutil_new_msg_buffer();

    if (is_gaup)
    {
      HDRIDLEC_PACK_CON_RSP_MSG_ID( send_msg_ptr, 
                                    HDRIDLEC_ATTRIB_UPDATE_ACCEPT_ID);
    }
    else
    {
      HDRIDLEC_PACK_CON_RSP_MSG_ID( send_msg_ptr, HDRIDLEC_CONFIG_RES_ID);
    }
    HDRIDLEC_PACK_CON_RSP_TRANS_ID( send_msg_ptr, creq_transaction_id);

    /* Now pack in the attribute ID and value of all attributes we have
     * flagged that we want to accept.
     */

    /* attrib_offset is now offset for response message */
    attrib_offset = HDRIDLEC_MSG_ID_SIZE + HDRIDLEC_TRANSACTION_ID_SIZE;

    if ((pccc_config_attrib_is_included) && (pccc_config_attrib.is_accepted))
    {
      /* Include that we are accepting the pccc config attribute */
      HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                               attrib_offset);
      HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRIDLEC_PCCC_CONFIG_ATTRIB_ID, attrib_offset );
      HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
        pccc_config_attrib.value_id, attrib_offset);

      attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

      /* Update our regional structure to reflect the newly accepted values */
#ifdef FEATURE_MEMCPY_REMOVAL
      hdridlec.negotiated.pccc = pccc_config_attrib.value; 
#else
      memcpy( &hdridlec.negotiated.pccc,
              &pccc_config_attrib.value,
              sizeof( hdridlec_pccc_config_attrib_type ));
#endif /* FEATURE_MEMCPY_REMOVAL */
    }

    if ((slotted_mode_attrib_is_included) && (slotted_mode_attrib.is_accepted))
    {
      if (is_gaup)
      {
        /* Update our regional structure to reflect the newly accepted values */
#ifdef FEATURE_MEMCPY_REMOVAL
        hdridlec.current.slotted_mode = slotted_mode_attrib.value;
#else
        memcpy( &hdridlec.current.slotted_mode,
                &slotted_mode_attrib.value,
                sizeof( hdridlec_slotted_mode_type ));
#endif /* FEATURE_MEMCPY_REMOVAL */

        /* Update the control channel cycle based on new SlotCycle3 value */
        hdridlec_gaup_accept();
#ifdef FEATURE_EIDLE_SCI
        hdrind_give_ind(HDRIND_IDLE_AN_GAUP_SLOTTEDMODE, (void*)&ind_data);
#endif /* FEATURE_EIDLE_SCI */
      }
      else
      {

        /* Include that we are accepting the slotted mode config attribute */
        HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                                 attrib_offset);
        HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
          HDRIDLEC_SLOTTED_MODE_ATTRIB_ID, attrib_offset );
        HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
          slotted_mode_attrib.value_id, attrib_offset);

        attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

        /* Update our regional structure to reflect the newly accepted values */
#ifdef FEATURE_MEMCPY_REMOVAL
        hdridlec.negotiated.slotted_mode = slotted_mode_attrib.value;
#else
        memcpy( &hdridlec.negotiated.slotted_mode,
                &slotted_mode_attrib.value,
                sizeof( hdridlec_slotted_mode_type ));
#endif /* FEATURE_MEMCPY_REMOVAL */
      }
    }

    if ((small_slot_cycle_allowed_attrib_is_included) && 
        (small_slot_cycle_allowed_attrib.is_accepted))
    {
      /* Include that we are accepting the config attribute */
      HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                               attrib_offset);
      HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRIDLEC_SMALL_SLOT_CYCLE_ALLOWED_ATTRIB_ID, attrib_offset );
      HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
        small_slot_cycle_allowed_attrib.value, attrib_offset);

      attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

      hdridlec.negotiated.small_slot_cycle_is_allowed = 
        small_slot_cycle_allowed_attrib.value;
    }

    if( ( access_hashing_channel_mask_attrib_is_included ) &&
        ( access_hashing_channel_mask_attrib.is_accepted ) )
    {
      if( is_gaup )
      {
        /* Set the accepted value for immediate use */
        hdridlec.current.access_hashing_channel_mask = 
          access_hashing_channel_mask_attrib.value;
      }
      else
      {
        HDRBIT_PACK8( send_msg_ptr, 
          HDRIDLEC_LARGE_SIMPLE_ATTRIB_SIZE_BYTES, 
          attrib_offset, HDRIDLEC_ATTRIB_LEN_SIZE );
  
        attrib_offset += HDRIDLEC_ATTRIB_LEN_SIZE;
  
        HDRBIT_PACK8( send_msg_ptr,
          HDRIDLEC_ACCESS_HASHING_CHANNEL_MASK_ATTRIB_ID, attrib_offset,
          HDRIDLEC_ATTRIB_ID_SIZE );
  
        attrib_offset += HDRIDLEC_ATTRIB_ID_SIZE;
  
        HDRBIT_PACK16( send_msg_ptr, access_hashing_channel_mask_attrib.value, 
          attrib_offset, HDRIDLEC_LARGE_SIMPLE_ATTRIB_VAL_SIZE );
        attrib_offset += HDRIDLEC_LARGE_SIMPLE_ATTRIB_VAL_SIZE;
  
        hdridlec.negotiated.access_hashing_channel_mask = 
          access_hashing_channel_mask_attrib.value;
      }
    }

    if ((connection_deny_backoff_attrib_is_included) && 
        (connection_deny_backoff_attrib.is_accepted))
    {
      /* Include that we are accepting the ConnxDenyBackoff config attribute */
      HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                                 attrib_offset);
      HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRIDLEC_CONNECTION_DENY_BACKOFF_ATTRIB_ID, attrib_offset );
      HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
        connection_deny_backoff_attrib.value, attrib_offset);

      attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

      hdridlec.negotiated.connectiondeny_backoff = 
          connection_deny_backoff_attrib.value;
    }

    if ((pageresponse_apersistence_supported_attrib_is_included) && 
        (pageresponse_apersistence_supported_attrib.is_accepted))
    {
      /* Include that we are accepting the pageresponse_apersistence_supported
         config attribute */
      HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                                 attrib_offset);
      HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRIDLEC_PAGERESPONSE_APERSISTENCE_SUPPORTED_ATTRIB_ID, attrib_offset );
      HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
        pageresponse_apersistence_supported_attrib.value, attrib_offset);

      attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

      hdridlec.negotiated.pageresponse_apersistence_supported = 
          pageresponse_apersistence_supported_attrib.value;
    }

    if( ( quick_paging_attrib_is_included ) &&
        ( quick_paging_attrib.is_accepted ) )
    {
      if( is_gaup )
      {
        /* Set the accepted value for immediate use */
        hdridlec.current.quick_paging = 
          quick_paging_attrib.value;
      }
      else
      {
        /* Include that we are accepting the config attribute */
        HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                                   attrib_offset);
        HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
          HDRIDLEC_QUICK_PAGING_ATTRIB_ID, attrib_offset );
        HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
          quick_paging_attrib.value_id, attrib_offset);

        attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

        hdridlec.negotiated.quick_paging = 
          quick_paging_attrib.value;
      }
    }

#ifdef FEATURE_HDR_REVC
    /* The access network shall not send GAUP with MultiATIPage */
    if( ( multi_page_attrib_is_included ) &&
        ( multi_page_attrib.is_accepted ) )
    {
      /* Include that we are accepting the config attribute */
      HDRIDLEC_PACK_CON_RSP_LEN( send_msg_ptr, HDRIDLEC_CRSP_ATTRIB_LEN_BYTES,
                                 attrib_offset);
      HDRIDLEC_PACK_CON_RSP_ATTRIBUTE_ID( send_msg_ptr,
        HDRIDLEC_MULTI_PAGE_SUPPORTED_ATTRIB_ID, attrib_offset );
      HDRIDLEC_PACK_CON_RSP_VALUE_ID( send_msg_ptr,
        multi_page_attrib.value_id, attrib_offset);

      attrib_offset += HDRIDLEC_CRSP_ATTRIB_LEN;

      hdridlec.negotiated.multi_page_supported = 
        multi_page_attrib.value_id;
    }
#endif /* FEATURE_HDR_REVC */

    if ( is_gaup )
    {
      hdrscmdb_set_gauped_config(HDRHAI_IDLE_STATE_PROTOCOL, &hdridlec.current);
    }

    /* Send config response on RTC.  All config messages are reliable.
     */
    protocol_instance = (is_gaup) ? HDRHAI_IN_USE : HDRHAI_IN_CONFIG;

    hdrhmp_send_msg( HDRHAI_IDLE_STATE_PROTOCOL,
                     HDRHAI_RTC,
                     TRUE, /* Reliable */
                     HDRIDLEC_MSG_PRIORITY,
                     protocol_instance,
                     send_msg_ptr,
                     NULL,
                     NULL );

  } 
  else
  {
    /* AT could not accept the AttributeUpdateRequest message so send session
     * close as AT is not allowed to send AttributeUpdateReject */
    if (!gaup_is_accepted)
    {
      gaup_fail_data.reason = HDRHAI_CLOSE_AN_GAUP_FAIL;
      gaup_fail_data.type = HDRHAI_IDLE_STATE_PROTOCOL;
      gaup_fail_data.subtype = 
                 hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
      gaup_fail_data.attribute_id_length = 
                       HDRIDLEC_ATTRIB_LEN_SIZE/HDRIDLEC_BITS_PER_BYTE;
      gaup_fail_data.attribute_id = gaup_failed_attrib_id;
      
      ind_data.scp_fail_ptr = (void *)&gaup_fail_data;

      hdrind_give_ind(HDRIND_IDLE_AN_GAUP_FAILED, (void*)&ind_data);
    }
  }

} /* hdridlec_process_config_req_msg */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLE_PROCESS_CONFIG_RES_TIMEOUT

DESCRIPTION
  This function processes the config response timer timeout.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_process_config_res_timeout ( void )
{
  hdridlec_fail_cur_config_neg( HDRHAI_CLOSE_CFG_RSP_EXP, 
                                HDRIDLEC_PCCC_CONFIG_ATTRIB_ID );
}  /* hdridlec_process_config_res_timeout() */


/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_MSG

DESCRIPTION
  This function processes messages for IDLEC.

DEPENDENCIES
  None

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message
  msg_id - The message ID already extracted from the DSM item

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_process_msg
(
  dsm_item_type   *msg_ptr,
  uint8           msg_id
)
{
  hdrhai_protocol_name_enum_type  curr_subtype = 
      (hdrhai_protocol_name_enum_type)
      hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (msg_id)
  {
    case HDRIDLEC_CONFIG_REQ_ID:

      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Rcv ConfigRequest msg");
      hdridlec_process_config_req_msg( msg_ptr, FALSE );
      break;

    case HDRIDLEC_ATTRIB_UPDATE_REQ_ID:

      HDR_MSG_PROT (MSG_LEGACY_LOW,  "Rcv ConfigRequest msg");
      if ( curr_subtype != HDRSCP_DEFAULT_IDLE )
      {
        hdridlec_process_config_req_msg( msg_ptr, TRUE );
      }
      break;

    case HDRIDLEC_CONFIG_RES_ID:

      HDR_MSG_PROT (MSG_LEGACY_LOW, "Rcv ConfigRes msg");
      hdridlec_process_config_res_msg( msg_ptr );
      break;

    default:
      ERR_FATAL( "Unknown message", 0, 0, 0 );
      break;
  }
} /* hdridlec_process_msg() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_PROCESS_IND

DESCRIPTION
  This function processes indications given to IDLEC.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for LUP to process
  ind_data_ptr - Any data associated with the indication

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -efunc(715,hdridlec_process_ind) */
void hdridlec_process_ind
(
  hdrind_ind_name_enum_type    ind_name,
  hdrind_ind_data_union_type   *ind_data_ptr
)
{

#ifdef FEATURE_HDR_REVC
  boolean multi_at_page_supported = FALSE;
    /* Is MultiATPage msg supported? */

  uint16 sub_type = hdrscp_get_current_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
    /* subtype of IDLE state protocol */
#endif /* FEATURE_HDR_REVC */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (ind_name)
  {

    case HDRIND_SMP_SESSION_CLOSED:

      hdridlec_process_session_closed_ind();
      break;

    case HDRIND_SMP_SESSION_OPENED:

      /* do nothing for now */
      break;

    case HDRIND_SCM_SESSION_CHANGED:
      /* If the session has been closed, clear any existing
         ConfigResponse timers */
      if( ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_NORMAL ||
          ind_data_ptr->change_reason == HDRHAI_SCHG_CLOSED_RESTORE_PRIOR )
      {
        (void) rex_clr_timer( &hdridlec.config_res_timer );
      }

#ifdef FEATURE_HDR_REVC
      if ( sub_type != HDRSCP_DEFAULT_IDLE )
      {
        /* In the session config negotiation, if an attribute is rejected by
           the AN, the attribute is not included in the response from AN. So
           the way to find out whether an attribute is rejected is to check
           SCM DB. */
        (void) hdrscmdb_get_inuse_attrib( HDRHAI_IDLE_STATE_PROTOCOL, 
                                          HDRIDLEC_MULTI_PAGE_SUPPORTED_ATTRIB_ID,
                                          &multi_at_page_supported,
                                          sub_type );

        if ( !multi_at_page_supported )
        {
          HDR_MSG_PROT( MSG_LEGACY_HIGH,
                        "MultiATPageMessageSupported config rejected by AN" );
          hdridle_set_multi_at_page_attrib_rejected( TRUE );
        }
      }
#endif /* FEATURE_HDR_REVC */

      /* Allowed to intentionally fall through to process the change in 
         session */

    case HDRIND_SCM_IDLE_AT_GAUP_ACCEPTED:
      hdridlec_process_session_changed_ind( ind_data_ptr );
      break;

    default:
      ERR_FATAL( "Unknown indication", 0, 0, 0 );
      break;
  }
} /* hdridlec_process_ind() */

#ifdef FEATURE_HDR_REVC
/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_MULTI_AT_PAGE_ATTRIB_CONFIG_IS_NEEDED
DESCRIPTION
  This function checks whether or not the MultiATPageSupported attribute
  needs to be configured.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdridlec_multi_at_page_attrib_config_is_needed(void)
{
  uint16 subtype =
    hdrscp_get_config_subtype( HDRHAI_IDLE_STATE_PROTOCOL );
  boolean config_needed = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if  (subtype != HDRSCP_DEFAULT_IDLE &&
       hdrmc_feature_is_enabled(HDRMC_FEATURE_MULTI_AT_PAGE))
  {
    /* desired value is TRUE */
    if (hdridlec.current.multi_page_supported != TRUE)
    {
      config_needed = TRUE;
    }
  }

  HDR_MSG_PROT_2(MSG_LEGACY_MED, "MultiAtPage Config Needed:%d, subtype:%d",
                 (uint32) config_needed, subtype);

  return config_needed;
}
#endif /* FEATURE_HDR_REVC */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_CONFIG_IS_NEEDED
DESCRIPTION
  This function check whether the configuration negotiation is needed
  or not

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  Ture - config is needed

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_config_is_needed(void)
{
  boolean config_is_needed = FALSE; /* Whether or not config is needed */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* for a new negotiation, any desired change from the default config
     * should start a negotiation.
     */
    config_is_needed = ( hdridlec_pccc_attrib_config_is_needed() ||
                         hdridlec_slotted_mode_attrib_config_is_needed()
#ifdef FEATURE_HDR_REVC
                         || hdridlec_multi_at_page_attrib_config_is_needed() 
#endif /* FEATURE_HDR_REVC */
                       );

  return config_is_needed;

} /* hdridlec_config_is_needed() */

/*===========================================================================

FUNCTION HDRIDLE_CONFIGURE_PCC_ONLY

DESCRIPTION
  Have idle state protocol propose the PreferredControlChannelCycle
  attribute as part of AT initiated session negotiation.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_configure_pccc_only
( 
  hdrhai_protocol_name_enum_type sender 
)
{
  if (!hdridlec_pccc_attrib_config_is_needed())
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "IDLE PCCC Config is not required" );
    return FALSE;
  }

  HDR_MSG_PROT( MSG_LEGACY_HIGH, "Re-configuring IDLE PCCC" );
  hdridlec_abort_cur_config_neg(); /* Legacy code; afraid to remove :) */
  hdridlec_send_config_req_msg( FALSE ); /* This is a PCCC re-negotiation */
  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_UPDATE_CONFIG

DESCRIPTION
  This function update the configuration if needed

DEPENDENCIES
  hdridlec_init should have been called before calling this function.

PARAMETERS
  reason - reason for the reconfig request

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_update_config(void)
{
  /* Note that this command can come during a negotiation, in which case
   * SCP should re-start the negotiation when it is done and call
   * hdridle_configure again. */
  hdrind_give_ind(HDRIND_IDLE_CONFIG_REQ,NULL);
} /* hdridlec_update_config() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_START_CONFIG

DESCRIPTION
  This function start idle state pccc configuration

DEPENDENCIES
  hdridlec_init should have been called before calling this function.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_start_config ( void )
{
  /* abort the current negotiation if it is in progress */
  hdridlec_abort_cur_config_neg();

  hdridlec_send_config_req_msg( TRUE ); /* This is a full negotiation */

  /* Note: At the beginning of the negotiation,
     the negotiated and the current are the same.
  */

} /* hdridlec_start_config() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_POWERUP_INIT

DESCRIPTION
  This function performs the powerup initialization of the idle state configuration
  negotiation module.

DEPENDENCIES
  This function must called only once after powerup

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_powerup_init ( void )
{
  /* NV_HDRIDLE_PREF_SLOTTED_MODE_CONFIG_I */
  nv_hdridle_pref_slotted_mode_config_type pref_slotted_mode;

  boolean desired_slotted_mode_config_is_valid_in_nv = TRUE; 
  hdridlec_slotted_mode_type  *dsmcp = &hdridlec.desired_slotted_mode_config;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrmc_is_first_init() )
  {
    /* Initialize timers. */
    rex_def_timer_ex( &hdridlec.config_res_timer,
                      hdridle_timer_cb,
                      HDRIDLE_CONFIG_RES_TIMER );
  }

  /* Initialize parameters */

  hdridlec.await_slp_config_req_cb = FALSE;
  hdridlec.config_req_msg.trans_id = 0;
  hdridlec.config_req_msg.pccc_value_id = 0;
  hdridlec.config_req_msg.pccc_is_requested = FALSE;
  hdridlec.config_req_msg.slotted_mode_is_requested= FALSE;
#ifdef FEATURE_HDR_REVC
  hdridlec.config_req_msg.multi_page_is_requested = FALSE;
#endif /* FEATURE_HDR_REVC */

  /* Get current configuration parameters from SCMDB */
  hdrscmdb_get_inuse_config( HDRHAI_IDLE_STATE_PROTOCOL, &hdridlec.current );

  /* Set the initial value of hdrscmidle.saved_scmidle_info*/
  hdrscmidle_init_scmidle_info(&hdridlec.current.saved_scmidle_info);

  /* Initializing the pccc is now done after ALMP moves to idle */

  /* Read the preferred SlottedMode attribute values from NV */
  if( hdrutil_read_nv_item( NV_HDRIDLE_PREF_SLOTTED_MODE_CONFIG_I,
        (nv_item_type *) &pref_slotted_mode ) != NV_DONE_S )
  {
    desired_slotted_mode_config_is_valid_in_nv = FALSE;
  }
  
  /* If the preferred SlottedMode attribute values exist perform
     sanity checks to ensure standards compliance */
  if( desired_slotted_mode_config_is_valid_in_nv )
  {
    if( pref_slotted_mode.slot_cycle1 > HDRIDLEC_MAX_SLOT_CYCLE ) 
    { 
      HDR_MSG_PROT_3( MSG_LEGACY_ERROR, 
        "SlotCycle1 %d > %d; capping SlotCycle1 at %d",
        pref_slotted_mode.slot_cycle1, 
        HDRIDLEC_MAX_SLOT_CYCLE, HDRIDLEC_MAX_SLOT_CYCLE );
      pref_slotted_mode.slot_cycle1 = HDRIDLEC_MAX_SLOT_CYCLE;
    }
    else if (pref_slotted_mode.slot_cycle1 < HDRIDLEC_MIN_SLOT_CYCLE)
    {
      HDR_MSG_PROT_3( MSG_LEGACY_ERROR, 
        "SlotCycle1 %d < %d; setting SlotCycle1 at %d",
        pref_slotted_mode.slot_cycle1, 
        HDRIDLEC_MIN_SLOT_CYCLE, HDRIDLEC_MIN_SLOT_CYCLE );
      pref_slotted_mode.slot_cycle1 = HDRIDLEC_MIN_SLOT_CYCLE;
    }

    if( pref_slotted_mode.slot_cycle2 < pref_slotted_mode.slot_cycle1 )
    {
      HDR_MSG_PROT_3( MSG_LEGACY_ERROR, 
        "SlotCycle2 %d < SlotCycle1 %d; changing SlotCycle2 to %d",
        pref_slotted_mode.slot_cycle2, 
        pref_slotted_mode.slot_cycle1, pref_slotted_mode.slot_cycle1 );
      pref_slotted_mode.slot_cycle2 = pref_slotted_mode.slot_cycle1;
    }
    
    if( pref_slotted_mode.slot_cycle3 < pref_slotted_mode.slot_cycle2 )
    {
      HDR_MSG_PROT_3( MSG_LEGACY_ERROR, 
        "SlotCycle3 %d < SlotCycle2 %d; changing SlotCycle3 to %d",
        pref_slotted_mode.slot_cycle3, 
        pref_slotted_mode.slot_cycle2, pref_slotted_mode.slot_cycle2 );
      pref_slotted_mode.slot_cycle3 = pref_slotted_mode.slot_cycle2;
    }

    if( pref_slotted_mode.wake_count2 < pref_slotted_mode.wake_count1 )
    {
      HDR_MSG_PROT_3( MSG_LEGACY_ERROR, 
        "WakeCount2 %d < WakeCount1 %d; changing WakeCount2 to %d",
        pref_slotted_mode.wake_count2, 
        pref_slotted_mode.wake_count1, pref_slotted_mode.wake_count1 );
      pref_slotted_mode.wake_count2 = pref_slotted_mode.wake_count1;
    }

    if( pref_slotted_mode.slot_cycle1 == 0 ||
        pref_slotted_mode.slot_cycle2 == 0 ||
        pref_slotted_mode.slot_cycle3 == 0 )
    {
      HDR_MSG_PROT_3( MSG_LEGACY_ERROR, 
        "SC1 = %d, SC2 = %d, SC3 = %d; value of 0 invalid, defaulting all values",
        pref_slotted_mode.slot_cycle1, 
        pref_slotted_mode.slot_cycle2, 
        pref_slotted_mode.slot_cycle3 );

      /* Store preferred SlottedMode attribute values as default */
      dsmcp->slot_cycle1 = HDRIDLEC_DESIRED_SLOT_CYCLE1;
      dsmcp->slot_cycle2 = HDRIDLEC_DESIRED_SLOT_CYCLE2;
      dsmcp->slot_cycle3 = HDRIDLEC_DESIRED_SLOT_CYCLE3;
      dsmcp->wake_count1 = HDRIDLEC_DESIRED_WAKE_COUNT1;
      dsmcp->wake_count2 = HDRIDLEC_DESIRED_WAKE_COUNT2;
    }
    else
    {
      /* Store preferred SlottedMode attribute values */
      dsmcp->slot_cycle1 = pref_slotted_mode.slot_cycle1;
      dsmcp->slot_cycle2 = pref_slotted_mode.slot_cycle2;
      dsmcp->slot_cycle3 = pref_slotted_mode.slot_cycle3;
      dsmcp->wake_count1 = pref_slotted_mode.wake_count1;
      dsmcp->wake_count2 = pref_slotted_mode.wake_count2;
    }
  }
  else
  {
    /* Store preferred SlottedMode attribute values as default */
    dsmcp->slot_cycle1 = HDRIDLEC_DESIRED_SLOT_CYCLE1;
    dsmcp->slot_cycle2 = HDRIDLEC_DESIRED_SLOT_CYCLE2;
    dsmcp->slot_cycle3 = HDRIDLEC_DESIRED_SLOT_CYCLE3;
    dsmcp->wake_count1 = HDRIDLEC_DESIRED_WAKE_COUNT1;
    dsmcp->wake_count2 = HDRIDLEC_DESIRED_WAKE_COUNT2;
  }

} /* hdridlec_powerup_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GET_SLOTTED_MODE_ATTRIB

DESCRIPTION
  This function returns the current SlottedMode attribute
  negotiated.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_get_slotted_mode_attrib
( 
  hdridlec_slotted_mode_type *slotted_mode_attrib_ptr
)
{
  hdrhai_protocol_name_enum_type  curr_subtype = 
      (hdrhai_protocol_name_enum_type)
      hdrscp_get_current_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( curr_subtype != HDRSCP_DEFAULT_IDLE )
  {
    (*slotted_mode_attrib_ptr) = hdridlec.current.slotted_mode;
    return TRUE;
  }
  return FALSE;
} /* hdridlec_get_slotted_mode_attrib() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_SET_IDLE_SLOT_CYCLE_TIMEOUT

DESCRIPTION
  This function sets the current and negotiated value of IDLE slot cycle
  timeout to the given value.

DEPENDENCIES
  None

PARAMETERS
  t1 - System time at which to change from SlotCycle1 to SlotCycle2
  t2 - System time at which to change from SlotCcyle2 to SlotCycle3

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_set_idle_slot_cycle_timeout
(
  uint64 t1,
  uint64 t2
)
{

  hdridlec.current.slot_cycle1_timeout = t1;
  hdridlec.current.slot_cycle2_timeout = t2;

  hdridlec.negotiated.slot_cycle1_timeout = t1;
  hdridlec.negotiated.slot_cycle2_timeout = t2;

  hdrscmdb_set_idle_slot_cycle_timeout(hdridlec.current.slot_cycle1_timeout,
                                       hdridlec.current.slot_cycle2_timeout);

} /* hdridlec_set_idle_slot_cycle_timeout() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GET_IDLE_SLOT_CYCLE_TIMEOUT

DESCRIPTION
  This function returns the current IDLE slot cycle timeout values.

DEPENDENCIES
  None

PARAMETERS
  t1 - System time at which to change from SlotCycle1 to SlotCycle2
  t2 - System time at which to change from SlotCcyle2 to SlotCycle3

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdridlec_get_idle_slot_cycle_timeout
(
  uint64 *t1,
  uint64 *t2
)
{

  *t1 = hdridlec.current.slot_cycle1_timeout;
  *t2 = hdridlec.current.slot_cycle2_timeout;

} /* hdridlec_get_idle_slot_cycle_timeout() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_GET_ACCESS_HASHING_CHANNEL_MASK

DESCRIPTION
  This accessor function returns the current value of the access hashing
  channel mask attribute.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The current AccessHashingChannelMask attribute.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdridlec_get_access_hashing_channel_mask( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return hdridlec.current.access_hashing_channel_mask;

} /* hdridlec_get_access_hashing_channel_mask() */

/*===========================================================================

FUNCTION HDRIDLEC_GET_INCONFIG_HANDLE

DESCRIPTION
  This function returns a pointer to inconfig instance of IDLE State Protocol

DEPENDENCIES
  None

PARAMETERS
  None                   
  
RETURN VALUE
  Pointer to inconfig instance IDLE State Protocol

SIDE EFFECTS
  None

===========================================================================*/
void* hdridlec_get_inconfig_handle(void)
{
#ifdef FEATURE_CRD_ENHANCEMENTS
  if (!hdridlec.negotiated.pccc.is_custom)
  {
    hdridlec.negotiated.pccc.value = hdridlec_compute_pccc( FALSE, 0x09 );
  }
#endif

  return (void*) &hdridlec.negotiated;
} /* hdridlec_get_inconfig_handle() */

/*===========================================================================

FUNCTION HDRIDLEC_SLOTTED_MODE_IS_OK_TO_SLEEP

DESCRIPTION
  AST doesn't want to support the configuration and testing of SCI < 5.
  We treat all SCI < 5 as non-slotted mode.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: if it's OK to go to sleep
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_slotted_mode_is_ok_to_sleep(void)
{
  return (hdridlec.current.slotted_mode.slot_cycle1 >=
          HDRIDLEC_MIN_SLOT_CYCLE);
} /* hdridlec_slotted_mode_is_ok_to_sleep() */


/*===========================================================================

FUNCTION hdridlec_qpch_is_enabled

DESCRIPTION
  This function returns whether QPCH is enabled or not.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: if AT is allowed to do QPCH
  FALSE: otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_qpch_is_enabled(void)
{
  uint16 curr_subtype = hdrscp_get_config_subtype(HDRHAI_IDLE_STATE_PROTOCOL);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ( curr_subtype == HDRSCP_QUICK_IDLE
#ifdef FEATURE_LTE_TO_HDR_OH
         ||
         curr_subtype == HDRSCP_IRAT_QUICK_IDLE
#endif /* FEATURE_LTE_TO_HDR_OH */ 
       ) && 
       ( hdridlec.current.quick_paging.quick_paging_enabled )
     )
  {
    return TRUE;
  }

  return FALSE;
} /* hdridlec_qpch_is_enabled() */

/* <EJECT> */
/*===========================================================================

FUNCTION HDRIDLEC_IS_SMALL_SLOTCYCLE_GAUP_ALLOWED

DESCRIPTION
  This function returns whether AUR of small slot cycle is allowed or not.

DEPENDENCIES
  None

PARAMETERS
  none

RETURN VALUE
  TRUE - If GAUP of small slot cycle is allowed, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdridlec_is_small_slotcycle_gaup_allowed ( void )
{
  return hdridlec.current.small_slot_cycle_is_allowed;
} /* hdridlec_is_small_slotcycle_gaup_allowed() */

