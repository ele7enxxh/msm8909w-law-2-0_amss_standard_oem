/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            A D D R E S S   M A N A G E M E N T   P R O T O C O L

GENERAL DESCRIPTION
  The Address Management Protocol (AMP) negotiates and maintains the
  address of the Access Terminal in HDR.

EXTERNALIZED FUNCTIONS (Global)

  hdramp_msg_cb()          - Callback registered with HMP to deliver
                             messages to AMP
  hdramp_ind_cb()          - Callback registered for delivery of indications
  hdramp_timer_cb()        - Callback registered for timer expiration
  hdramp_activate()        - Command function to activate AMP
  hdramp_deactivate()      - Command function to deactivate AMP
  hdramp_update_uati()     - Command function to update the UATI
  hdramp_update_esn()      - Command function to update the Hardware ID(ESN)
  hdramp_first_acq_init()  - Init done on first acquisition after powerup
  hdramp_powerup_init()    - Powerup Initialization command
  hdramp_write_session_info_to_efs() - Function to write session data to EFS
  
  hdramp_get_uati()        - Accessor function to get the UATI
  hdramp_get_session_seed  - Accessor function to get the session seed
  hdramp_get_transmit_ati  - Accessor function to get the transmit ATI
  hdramp_get_previous_uati() - Accessor function to get the UATI from the
                               previous session

EXTERNALIZED FUNCTIONS (Regional)
  hdramp_process_cmd()     - Function called by hdrmc task to process
                              a command for AMP
  hdramp_process_ind()     - Function called by hdrmc task to process
                              an indication for AMP
  hdramp_process_msg()     - Function called by hdrmc task to process
                              a message for AMP
  hdramp_process_timer()   - Function called by hdrmc task to process
                              a timer for AMP

  hdramp_send_uati_req_msg() - Function to send a UATI Request msg
  hdramp_enter_setup() - Function to perform actions necessary when entering
                         SETUP state
  hdramp_reset_data() - Function to reset data
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdramp.c_v   1.60   06 Mar 2003 09:13:32   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdramp.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/27/16   svu     HDR changes for FR-CT SMS AUTO REGISTRATION
12/14/15   vko     Fixed KW issues 
12/11/15   rmv     In AMP init, skip ATI data reset if AMP is not INACTIVE
11/23/15   vko     Ignore UATI assignment timer expiry if d2l reselect is in progress
09/05/14   cnx     Do not redefine timer if it is NV refresh.
05/09/14   vko     Support to handle bad NV read gracefully
12/10/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/01/13   smd     Used new secapi to get random number.
07/31/13   sat     Mainlining Modem Statistics Feature
06/20/13   ukl     Set PCCC on entering idle state.
05/09/13   wsh     Fixed sending multiple UATI, not sending RTC msg in tunnel  
04/04/13   cnx     Fixed UE doesn't perform PreRegistration in ping-pong of CELL
03/28/13   vko     Fixed Session renegotiation after Mode Offline-d / Mode LPM
03/26/13   vko     Do not start SAP backoff timer in all cases of AC failure
03/19/13   cnx     Send SessionClose when both NAI and eHRPD capability change
02/04/13   cnx     Replace PriorSession with UpdateUATI in some network.
02/03/13   cnx     Don't reset tunnel info if AT comes back to the same cell
                   after OOS. 
01/02/12   cnx     Fixed no IRMI response to DS when HDR is deactivated.
11/04/12   cnx     Close virtual connection if preReg is not allowed.
11/03/12   cnx     Resend UATIreq if AMP SETUP is pending.
08/22/12   cnx     Abort sending IRMI when HDR exits online.
07/11/12   cnx     Handle IRMI when AT is doing RestorePriorSession.
04/20/12   cnx     Unsubcribe LTE_RRC_SERVICE_IND.
03/30/12   cnx     Close virtual connection when preReg zone changes.
03/19/12   cnx     Resolved TCB compiler errors for Dime. 
03/12/12   cnx     Set uati_asgn_received before clearing UATI assignment timer.
01/27/11   cnx     Support pre-Registration status update.
01/06/12   cnx     Ignore preReg change in the same cell.
11/29/11   wsh     Update SCMDB immediate after HWID read
11/28/11   cnx     Support NV for set preReg status.
11/08/11   cnx     Support Optimized Handoff phase 3.
10/18/11   cnx     Resend UATIrequest when backoff timer expires if AT is 
                   updating UATI.
10/10/11   cnx     Session maintainence fix for Optimized Handoff.
10/03/11   cnx     Session maintainence fix for Optimized Handoff.
09/24/11   cnx     Session maintainence for Optimized Handoff.
08/19/11   wsh     Replaced depreciated MMGSDI API calls
07/27/11   cnx     Merged Optimized Handoff changes.
05/03/11   cnx     Fixed compiler warnings.
05/02/11   cnx     Fixed compiler warnings.
04/27/11   cnx     Supported hdrmc callbacks.
03/03/11   wsh     Fixed invalid HardwareIDResp if UIM API return 0 as ESN
06/21/10   cnx     Fixed compiler warnings.
06/10/10   pba     IRAT Measurement feature.
06/01/10   pba     Added support for FTD stats.
03/22/10   pba     Added support for eHRPD session negotiation based on 
                   confirming availability of USIM App
10/27/09   pba     Klockwork fixes.
04/01/09   wsh     JCDMA passport mode changes
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
03/09/09   pba     Avoid sending redundant UATIRequest.
02/24/09   wsh     CMI: updated included headers due to header split
02/18/09   wsh     Updated file to include private header hdrmc_v.h
01/30/09   wsh     Merged BCMCS 2.0 on to mainbranch
12/10/08   smd     Replaced assert.h with amssassert.h
11/21/08   kss     Removed/replaced intlocks.
11/21/08   pba     Added events for SLPQH feature.
10/01/08   wsh     Fixed critical/high lint warnings
09/22/08   wsh     Fixed SessionClose when no RUIM card is inserted
06/17/08   wsh     Compile error introduced by incorrect featurization
06/13/08   wsh     Report shell ESN/MEID to AN. Close session if RUIM changed
06/08/08   wsh     Fixed DCM can't turn on when in ConSetup
04/17/08   wsh     DCM optimization: not closing session unnecessarily when
                   moving across subnet
04/17/08   wsh     Added support to cancel waiting for UATIAssign after 
                   receiving CC msg
04/03/08   wsh     Merged fix for waiting UATIAssign onto 2H07
11/13/07   wsh     Fixed memory overwrite when reading NV_ITEM
09/24/07   wsh     Fixed AMP OVHD_INFO_UPDATED_IND handling
09/11/07   wsh     Removed lint high warnings
09/04/07   wsh     AT sends multiple UATI requests when openning a session
06/14/07   wsh     Fixed error where AT sends RATI instead of UATI during 
                   idle handoff to a different subnet.
06/08/07   mpa     Featurize RUIM card changes under FEATURE_MMGSDI
06/04/07   mpa     Close session if RUIM card has been in a different shell
05/29/07   mpa     Close session if session NAI changes.
05/18/07   mpa     Always send ESN_ME as hardware ID for JCDMA/RUIM.
04/03/07   wsh     Fixed error where dsm_item is not freed
03/27/07   wsh     Added support for new prior session handling
10/04/06   pba     Moved location information storage from NV to EFS.
08/08/06   pba     Fixed an issue that was causing, AT to send redundant 
                   UATIRequest after session negotiation.
07/31/06   pba     Added SecondaryColorCode Support
06/05/06   pba     Added support to store session data in EFS.
05/31/06   dna     Remember SectorID of outstanding UATIRequest and don't send
                   a second (redundant) request on personality switch
                   Clear UATIAssignment timer on reacq on original subnet
05/12/06   hal     Store the most recent OVHD location in NV for use between 
                   power cycles
05/09/06   yll     Added support for Minimal UATI Mode.
05/09/06   dna     Don't think HWID is changed if ESN is set to "zero"
04/07/06   hal     Changed hdrutil_new_buffer() to use hdrutil_new_msg_buffer()
02/23/06   hal     Added support for MaxNoMonitorDistance, ReducedSubnetMask
02/07/06   dna     Tell if hdramp_previous_session_opened closed the session.
01/14/06   dna     Change session seed for every new session
11/07/05   pba     Modified SCMDB calls to use updated API
10/04/05   pba     Added support to read/write config parameters from SCMDB/EFS
09/19/05   pba     Send UATI and Connection req together on crossing RNC border
07/07/05   dna     Added support for Enhanced Idle State protocol
                   Removed FEATURE_HDR_RATI_IN_NV because it is obsolete
05/10/05   dna     Add support for MEID
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
12/07/04   hal     Changed dsm_offset_new_buffer to hdrutil_new_buffer.     
11/29/04   grl     Added connected state stat logging support.
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
10/19/04   mpa     Replaced hdrscp_session_is_open with hdrsmp_session_is_open.
09/30/04   mpa     Always assign esn to hwid if they differ on a ph stat change.
08/26/04   mpa     Close the Session if the stored ESN does not match the AT's
                   new ESN.
09/10/03   mpa     Converted F3 messages to use new MSG2.0 SSIDs
06/06/03   mpa     Resend UATIReq msg if msg was aborted in AMP_SETUP
03/22/03   mpa     Added TX event for UATI req message.
03/05/03   mpa     Changed timers to use REX callbacks instead of signals 
10/29/02   mpa     Log full uati instead of uati024
08/05/02   mpa     Remember last ATI given to ACMAC as transmit ATI (match it).
07/26/02   dna     Read ESN from NV at power up.
07/08/02   mpa     Retrieve transmit ati from NV as the current UATI.
06/24/02   mpa     Cancel UATI assign timer if HO back to subnet that assigned
                   our current UATI.  Added new msgs.
06/19/02   dna     Back out change to avoid sending multiple UATIRequests
04/17/02   mpa     Allow for n concurrent UATIs instead of two.
                   Receive ati list handling moved to hdrampati.{h,c}
04/16/02   dna     Avoid sending multiple UATIRequest to same AN at same time
03/19/02   dna     No longer keep dual expiration timer in NV.
03/07/02   dna     When in setup, only wait up to 5 seconds for UATIAssignment,
                   otherwise retry UATIRequest.  Fail session after 5 failures.
01/30/02   mpa     Minor change to hdramp_first_acq_init.
01/28/02   dna     Restore session seed as part of prior session
01/21/02   sh      Added function for session seed generation and generated
                   session seed on first acquisition if session is not open
01/17/02   dna     Perform dual address timer init on first acquisition.
                   Remove hdramp_init() since it was never used.
12/13/01   mpa     Added session attempt logging upon powerup
12/04/01   dna     Write Dual Address timer to NV (was commented out)
09/12/01   nmn     Changed how FEATURE_HDR_RATI_IN_NV works
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/23/01   sh      New param for hdrplog_uati_rcvd and always log uati assig msg 
08/18/01   sh      Added protocol logging support
07/02/01   nmn     fixed timers to be miliseconds instead of 80ms
06/26/01   nmn     changed FEATURE_RATI_IN_NV to FEATURE_HDR_RATI_IN_NV
06/06/01   dna     new param for hmp_send_msg()
05/18/01   sh      called idle provided function to generate session seed
05/08/01   nmn     Beautification
05/02/01   nmn     modified NV interface for address data
05/01/01   dna     For consistency, match_receive_ati_address takes a pointer
                   Check for NULL in get_uati
03/29/01   nmn     updated for OVHD interface
03/21/01   dna     Chagnes to some event messages, removed one.
12/11/00   nmn     Initial revision

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "amssassert.h"
#include "nv.h"
#include "ran.h"
#include "memory.h"
#include "sys.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "distreg.h"
#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"

#ifdef FEATURE_CMI
#include "mmgsdilib_v.h"
#endif

#endif /* FEATURE_MMGSDI*/

#include "hdrhai.h"
#include "hdrmci.h"
#include "hdralmp.h"
#include "hdramp.h"
#include "hdrampi.h"
#include "hdramps.h"
#include "hdrampati.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdrind.h"
#include "hdrtrace.h"
#include "hdraddr.h"

#ifdef FEATURE_CMI
#include "hdraddr_v.h"
#endif

#include "hdrutil.h"
#include "hdridle.h"
#include "hdridlec.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif
#include "hdrplog.h"
#include "hdrdsm.h"
#include "hdrovhd.h"
#include "hdrhmpsa.h"
#include "hdrscmdb.h"
#ifdef FEATURE_HDR_REVC
#include "hdrscmfmac.h"
#include "hdrscmidle.h"
#endif /* FEATURE_HDR_REVC */
#include "hdrscmamp.h"
#include "hdrsmpka.h"
#include "hdrmc_v.h"

#if defined(FEATURE_UIM_RUIM)
#include "uim.h"

#ifdef FEATURE_CMI
#include "uim_v.h"
#endif

#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
#define HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_LENGTH_MS 4000
#include "hdrslp.h"
#include "hdrhmpsa.h"
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrsap.h"
#include "hdrsaps.h"
#endif /* FEATURE_LTE_TO_HDR_OH*/

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* Address of the TCB for the task in which this protocol runs */
#define HDRAMP_TASK_PTR hdrmc_tcb_ptr

#define HDRAMP_UATI_REQ_MSG        0x00   /* UATI Request Message ID */
#define HDRAMP_UATI_ASGN_MSG       0x01   /* UATI Assignment Message ID */
#define HDRAMP_UATI_COMP_MSG       0x02   /* UATI Complete Message ID */
#define HDRAMP_HARDWARE_ID_REQ_MSG 0x03   /* Hardware ID Request Message ID */
#define HDRAMP_HARDWARE_ID_RSP_MSG 0x04   /* Hardware ID Response Message ID */
#define HDRAMP_IRMI_MSG            0x05   /* Inter RAT Mobility Indication Message ID */
#define HDRAMP_IRMA_MSG            0x06   /* Inter RAT Mobility Ack Message ID */

#define HDRAMP_HWID_MESSAGE_PRIORITY 40 /* priority for hardware ID msg */
#define HDRAMP_UATI_MESSAGE_PRIORITY 10 /* priority for UATI messages */
#define HDRAMP_IRMI_MESSAGE_PRIORITY 20 /* priority for IRMI msg */

/* Time to receive UATI Assignment after sending UATI Request */
#define HDRAMP_UATI_ASSIGNMENT_LENGTH_SECONDS 120

/* Time to receive UATI Assignment after our initial UATI Request */
#define HDRAMP_INITIAL_UATI_ASSIGN_MS (5 * 1000)

/* Time to receive IRMA after IRMI is sent */
#define HDRAMP_IRMA_MS 1000

/* Maximum number of attempts for sending an InterRATMobilityIndication */
#define HDRAMP_IRMI_MAX_RETRY 2

/* Number of times to try a UATI Request */
#define HDRAMP_MAX_INITIAL_UATI_REQ_ATTEMPTS  5

/* N-Address Matching time */
#define HDRAMP_ADDRESS_MATCH_LENGTH_SECONDS 180

/* Second to ms unit conversion */
#define HDRAMP_SECOND_TO_MS_CONVERSION 1000

/* Right shift timestamp 22 bits to convert it into an 80ms value */
#define HDRAMP_TS_TO_80MS_CONVERSION  -22

/* Time to receive UATI Assignment after sending UATI Request in ms units */
#define HDRAMP_UATI_ASSIGNMENT_LENGTH_MS \
        (uint32)(HDRAMP_UATI_ASSIGNMENT_LENGTH_SECONDS * \
        HDRAMP_SECOND_TO_MS_CONVERSION)

/* Dual Address Matching time in ms units */
#define HDRAMP_ADDRESS_MATCH_LENGTH_MS \
        (uint32)(HDRAMP_ADDRESS_MATCH_LENGTH_SECONDS * \
        HDRAMP_SECOND_TO_MS_CONVERSION)

#define HDRAMP_INITIALIZE_MSG_SEQ 255

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define HDRAMP_UATI_ASSIGN_INITIAL_LENGTH 24
#define HDRAMP_UATI_ASSIGN_SUBNET_INFO_LENGTH 112
#define HDRAMP_UATI_ASSIGN_COLOR_CODE_LENGTH 8
#define HDRAMP_UATI_24_LENGTH 24

#define HDRAMP_OCTETS_IN_UATI104 13
#define HDRAMP_MAX_UPPER_OLD_UATI_OCTETS 13 /* Maximum number of octets in the
                                            UpperOldUATI */
#define HDRAMP_MAX_HARDWARE_ID_OCTETS 4     /* Maximum number of octets in the
                                            Hardware ID */

/* Enumeration of return values from the 
   hdramp_subnet_changed() function */
typedef enum
{
  HDRAMP_SUBNET_ERROR,
  HDRAMP_SUBNET_UNCHANGED,
  HDRAMP_SUBNET_CHANGED_SAME_SUPERNET,
  HDRAMP_SUBNET_CHANGED_NEW_SUPERNET

} hdramp_subnet_changed_enum_type;

/* UATI Request Message structure */
typedef struct
{
  uint8 message_id;
  uint8 transaction_id;
} hdrampi_uati_request_msg_type;

/* UATI Response Message structure */
typedef struct
{
  uint8 message_id;
  uint8 message_seq;
  boolean subnet_included;
  uint8 uati_subnet_mask;
/***************************************************************************
* The entire address will be filled in here (if included), with the bottom
* three octets set to 0;
***************************************************************************/
  hdraddr_type uati_104;
  uint8 uati_color_code;
  uint32 uati_024;
  uint8 upper_old_uati_length;
} hdrampi_uati_assignment_msg_type;

/* UATI Complete Message structure */
typedef struct
{
  uint8 message_id;
  uint8 message_seq;
  uint8 upper_old_uati_length;
/***************************************************************************
* A note concerning the formation of the data in the upper_old_uati field:
*
* This field contains the least significant bits of the UATI after the
* least significant 24 bits.  Offset 0 contains the most significant bits
* of the requested subset, and offset (upper_old_uati_length - 1) contains
* the least significant bits of the requested subset.
***************************************************************************/
  uint8 upper_old_uati[HDRAMP_MAX_UPPER_OLD_UATI_OCTETS];
} hdrampi_uati_complete_msg_type;

typedef struct
{
  /* Timer for receiving the UATI assignment Msg after sending the UATI
     Request Msg */
  rex_timer_type uati_assignment_timer; 

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  /* Sanity timer for having SLP hold off sending other messages after
     UATIRequest. */
  rex_timer_type uati_assignment_slp_holdoff_timer; 
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Timer for receiving the InterRATMobilityAck msg after sending
   * InterRATMobilityIndication msg */
  rex_timer_type irma_timer;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* transaction ID for the UATI Request Messages */
  uint8 uati_req_trans_id;

  /* UATI assignment corresponding to last sent UATI Request received */
  boolean uati_asgn_received;

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Waiting for UATI assignment */
  boolean waiting_uati_asgn;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Number of attempts to send out initial UATI Request */
  uint8 initial_uati_req_attempts;

  /* When a UATI request is sent when the AT already has a valid UATI, that
     UATI is saved in old_uati because the AN may request part of this
     in its subsequent UATI assignment. */
  hdraddr_type old_uati;

  /* Current and previous session's AMP configuration data. */
  hdramp_session_info_type session_info;
  hdramp_session_info_type prior_session_info;  

  /* Remember what sector we are on when we trigger a UATIRequest.  If
     we get OverheadInfoUpdated again for the same sector (i.e. an IHO that 
     ended up back on the same sector, SectorSignature change, or personality 
     switch), don't send a second UATIRequest. (see section 7.3.7.1.6.1) */
  hdraddr_type sector_id_of_last_uati_request;

  /* if prev UATI matched newly acquired network, but CC not in SP PARM,
     a new UATI is needed after restoring prior session */
  boolean req_uati_for_prev;
  hdraddr_type prev_uati_sect_id;
  boolean amp_setup_pending;

  boolean awaiting_uatireq_tx_outcome;

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  boolean dcm_reacq_diff_subnet;
  uint8 dcm_iho_count;
#endif

#ifdef FEATURE_LTE_TO_HDR_OH
  /* received ovhd after idle ho from LTE */
  boolean rxed_ovhd_after_irat_iho;
  
  /* Pre-Registration is allowed in the last LTE cell */
  boolean last_prereg_is_allowed;

  /* Reselect from HDR to LTE, this flag is not needed now but it may be needed
   * to optimize HDR to LTE reselect behavior (i.e. update UATI not restore session
   */
  boolean hdr_to_lte_resel;

  /* first time entering tunnel */
  boolean entering_tunnel; 

  /* PreReg value in the previous LTE cell */
  boolean prev_cell_prereg;

  /* Last preReg value */
  boolean current_cell_prereg;

  /* Whether UE moves to a new LTE cell */
  boolean in_same_cell;

  /* LTE current cell id */
  lte_rrc_global_cell_id_t lte_cell_id;

  /* Last LTE cell id */
  lte_rrc_global_cell_id_t lte_cell_id_last;

  /* Whether UE is ping-ponging between two cells */
  boolean back_to_same_cell;

  /* The cell pre-Registration status when AT enters the cell last time */
  boolean same_cell_last_prereg; 

  /* If we need to send uati after backoff timer expires */
  boolean backoff_sending_uati;

  /* Counter for IRMI that have been sent */
  uint8 irmi_cnt;

  /* Sending IRMI is pending because OVHD is not updated */
  boolean irmi_is_pending;

  /* Whether we are still waiting for IRMI L2 ACK */ 
  boolean awaiting_irmi_tx_cb;

  /* Whether prior session is supported by network.
   * If this flag is TRUE, replace all the restore prior session scenario
   * with update UATI in the tunnel mode. */
  boolean ps_is_not_supported;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* Session needs to be closed because of NAI change */
  boolean session_close_nai_pending;
  hdramp_nai_type session_nai_pending;

} hdramp_data_type;

LOCAL hdramp_data_type hdramp;

/* <EJECT> */ 
/*===========================================================================

                   MESSAGE PACK / UNPACK MACROS

===========================================================================*/

/**********************
** All Pack routines take a pointer to a dsm_item and
** a variable of the type belonging to the appropriate message
**********************/

/*---------------------
* UATI Request Message
---------------------*/
#define HDRAMP_PACK_UATI_REQ_MSG_ID( msg_ptr, uati_req ) \
          (HDRBIT_PACK8(msg_ptr, uati_req.message_id, 0, 8))
#define HDRAMP_PACK_UATI_REQ_TRANS_ID( msg_ptr, uati_req) \
          (HDRBIT_PACK8(msg_ptr, uati_req.transaction_id, 8, 8)) 
/*---------------------
* UATI Complete Message
---------------------*/
#define HDRAMP_PACK_UATI_COMP_MSG_ID( msg_ptr, uati_comp) \
          (HDRBIT_PACK8(msg_ptr, uati_comp.message_id, 0, 8))
#define HDRAMP_PACK_UATI_COMP_MSG_SEQ( msg_ptr, uati_comp) \
          (HDRBIT_PACK8(msg_ptr, uati_comp.message_seq, 8, 8))
#define HDRAMP_PACK_UATI_COMP_RESERVED( msg_ptr ) \
          (HDRBIT_PACK8(msg_ptr, 0x00, 16, 4))
#define HDRAMP_PACK_UATI_COMP_OLD_LEN( msg_ptr, uati_comp) \
          (HDRBIT_PACK8(msg_ptr, uati_comp.upper_old_uati_length, 20, 4))
#define HDRAMP_PACK_UATI_COMP_OLD( msg_ptr, uati_comp, octet) \
          (HDRBIT_PACK8(msg_ptr, (uint8)(uati_comp.upper_old_uati[octet]), \
           (uint8)(24 + (octet * 8)), 8))
/*---------------------
* Hardware ID Response Message
---------------------*/
#define HDRAMP_PACK_HARDWARE_RSP_MSG_ID( msg_ptr, message_id) \
          HDRBIT_PACK8( (msg_ptr), (uint8)(message_id), 0, 8)
#define HDRAMP_PACK_HARDWARE_RSP_TRANS_ID( msg_ptr, transaction_id) \
          HDRBIT_PACK8( (msg_ptr), (transaction_id), 8, 8)
#define HDRAMP_PACK_HARDWARE_RSP_TYPE( msg_ptr, hardware_id_type) \
          HDRBIT_PACK32( (msg_ptr), (hardware_id_type), 16, 24)
#define HDRAMP_PACK_HARDWARE_RSP_LENGTH( msg_ptr, hardware_id_length) \
          HDRBIT_PACK8( (msg_ptr), (uint8)(hardware_id_length), 40, 8)
#define HDRAMP_PACK_HARDWARE_RSP_VALUE_ESN( msg_ptr, esn ) \
          HDRBIT_PACK32( (msg_ptr), (esn), 48, 32)
#define HDRAMP_PACK_HARDWARE_RSP_VALUE_MEID( msg_ptr, meid_hi, meid_lo ) \
          HDRBIT_PACK32( (msg_ptr), (meid_hi), 48, 24); \
          HDRBIT_PACK32( (msg_ptr), (meid_lo), 72, 32)
/*---------------------
* Inter RAT Mobility Indication Message
---------------------*/
#define HDRAMP_PACK_IRMI_MSG_ID( msg_ptr, message_id ) \
          (HDRBIT_PACK8(msg_ptr, message_id, 0, 8))

/**********************
** Unpack routines.  All take a pointer to a DSM item and
** a variable of the appropriate type
**********************/
#define HDRAMP_UNPACK_MSG_ID( msg_ptr, msg_id) \
          (HDRBIT_UNPACK8(msg_ptr, (msg_id), 0, 8))
#define HDRAMP_UNPACK_TRANS_ID( msg_ptr, trans_id) \
          (HDRBIT_UNPACK8(msg_ptr, (trans_id), 8, 8))
/*---------------------
* UATI Assignment Message
---------------------*/
#define HDRAMP_UNPACK_UATI_SUBNET_INC( msg_ptr, subnet_inc) \
          (HDRBIT_UNPACK8(msg_ptr, (uint8*)(subnet_inc), 23, 1))
#define HDRAMP_UNPACK_UATI_SUBNET_MASK( msg_ptr, mask) \
          (HDRBIT_UNPACK8(msg_ptr, (mask), 24, 8))
#define HDRAMP_UNPACK_UATI_104( msg_ptr, address, octet) \
          (HDRBIT_UNPACK8(msg_ptr, (address[octet]), \
           (uint16)(32 + (octet * 8)), 8))
#define HDRAMP_UNPACK_UATI_COLOR_CODE( msg_ptr, color_code, offset) \
          (HDRBIT_UNPACK8(msg_ptr, (color_code), (uint16)offset, 8))
#define HDRAMP_UNPACK_UATI_024( msg_ptr, uati024, offset) \
          (HDRBIT_UNPACK32(msg_ptr, (uati024), (uint16)offset, 24))
#define HDRAMP_UNPACK_UATI_OLD_LENGTH(msg_ptr, old_length, offset) \
          (HDRBIT_UNPACK8(msg_ptr, (old_length), (uint16)offset, 4))

/*---------------------
* UATI Conversion
---------------------*/
#define HDRAMP_FULL_UATI_TO_UATI32(full_uati, color_code) \
          (uint32)((((color_code)&0xFF) << 24) | \
                   ((full_uati)[13] << 16) | \
                   ((full_uati)[14] << 8) | \
                   ((full_uati)[15]))
/*--------------------------------------------------------------------------

                          UATI Assignment Timer
              
--------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* header for the tx_outcome_cb function used as a pointer for hmp_send_msg */
LOCAL void hdramp_uati_req_tx_outcome_cb
( 
  void *data_ptr,
  hdrerrno_enum_type err_val
);

/* <EJECT> */ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

                              MAIN FUNCTIONS

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
/*===========================================================================

FUNCTION HDRAMP_DONE_WAITING_FOR_UATI_ASSIGNMENT

DESCRIPTION
  Whenever AMP is done waiting for a UATIAssignment it needs to make sure any
  messages queued to SLP that were held off are now sent.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void hdramp_done_waiting_for_uati_assignment
( 
  hdramp_clear_slp_holdoff_reason_type reason
)
{
  hdrslp_stop_waiting_for_uati_assignment();
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                  "SLPQH: Canceling timer with %d ms remaining",
                  rex_get_timer(&hdramp.uati_assignment_slp_holdoff_timer));
  (void) rex_clr_timer(&hdramp.uati_assignment_slp_holdoff_timer);

  HDRTRACE_SLP_SLPQH_TIMER_STOPPED( reason );

  if (hdrslp_tx_queue_is_empty( HDRHAI_HDR_MESSAGING_PROTOCOL ))
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "SLPQH: No pending msgs so no need to start access.");
  }
  else
  {
    /* If there are message queued, try to start another access. */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                  "SLPQH: Start access to send pending msgs.");

    hdrhmp_send_msg( HDRHAI_ROUTE_UPDATE_PROTOCOL,
                     HDRHAI_AC, FALSE, 255, HDRHAI_IN_USE, NULL, NULL, NULL );
  }
} /* hdramp_done_waiting_for_uati_assignment() */
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

/*===========================================================================

FUNCTION HDRAMP_REQUEST_NEW_ATI

DESCRIPTION
  This function saves the old UATI and requests a new UATI.

DEPENDENCIES
  None.

PARAMETERS
  sp_sector_id - Sector ID from current SectorParameters message

RETURN VALUE
  None.

SIDE EFFECTS
  hdramp.old_uati is updated with the old UATI.

===========================================================================*/
static void hdramp_request_new_ati
(
  hdraddr_type sp_sector_id
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (((rex_get_timer( &hdramp.uati_assignment_timer ) != 0 ) &&
       hdraddr_is_equal( hdramp.sector_id_of_last_uati_request, sp_sector_id )) ||
      hdramp.awaiting_uatireq_tx_outcome)
  {
    /* We have already sent the UATIRequest on this subnet and are still waiting
       for a UATIAssignment. Don't send another request. */
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Avoid sending redundant UATIRequest" );
  }
  else
  {
    /* Remember what sector this UATI was requested on */
    hdraddr_set( hdramp.sector_id_of_last_uati_request, sp_sector_id );

    /* Save current UATI to OldUATI and request a new UATI */
    hdraddr_set( hdramp.old_uati, hdramp.session_info.full_uati );

    hdramp.awaiting_uatireq_tx_outcome = TRUE;

    hdramp_send_uati_req_msg();

#ifdef FEATURE_LTE_TO_HDR_OH
    if ( !hdrsap_tunnel_mode_is_enabled( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) )
#endif /*FEATURE_LTE_TO_HDR_OH */
    {
      /* The call to hdrhmpsa_restart() is added here to ensure that the 
         HDRHMP_SEND_MSG_CMD that was just queued on the hdrmc.event_q to
         send a UATIRequest message is processed before HMPSA starts the
         access attempt. Otherwise, if there was already a ConnectionRequest
         message queued to SLP, the ConnectionRequest would be sent before
         the UATIRequest. Calling hdrhmpsa_restart() forces another 
         (superfluous) handshake with HDRSRCH to ensure it is in access mode.  
         A side effect of this is that since the HDRHMP_SEND_MSG_CMD is 
         already queued, HDRMC task will process it before it gets 
         HDRIND_LMAC_ACCESS_MODE_ENTERED and starts the access attempt. */
      hdrhmpsa_restart();
    }
  }

} /* hdramp_request_new_ati() */


/*===========================================================================

FUNCTION HDRAMP_SET_TRANSMIT_ATI

DESCRIPTION
  This function sets the transmit ati in a critical section
  
DEPENDENCIES
  None

PARAMETERS
  ati  - ati type and value to set our transmit ati to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdramp_set_transmit_ati
(
  hdrhai_ati_record_type ati
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  REX_DISABLE_PREMPTION(); /* Lock interrupts for this update */

  hdramp.session_info.transmit_ati.ati_type  = ati.ati_type;
  hdramp.session_info.transmit_ati.ati_value = ati.ati_value;

  REX_ENABLE_PREMPTION(); /* Unlock interrupts */

} /* hdramp_set_transmit_ati */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_WRITE_SESSION_INFO_TO_DB

DESCRIPTION
  This helper function is used to write AMP's session information to EFS.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdramp_write_session_info_to_db ( void )
{

  hdramp.session_info.receive_current_uati = 
                 hdrampati_receive_ati_get(HDRHAI_UATI);

  hdrscmdb_set_amp_session_info( &hdramp.session_info );

} /* hdramp_write_session_info_to_db */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SET_UATI024

DESCRIPTION
  This helper function sets the bottom three octets of the UATI out of the
    UATI024

DEPENDENCIES
  None

PARAMETERS
  uati_ptr - pointer to the 128-bit UATI whose LSB will be changed
  uati_24 - the 24 bits to be set into the 3 LSB of the uati

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdramp_set_uati024
(
  hdraddr_type uati_ptr,
  uint32 uati_24
)
{
  if (uati_ptr != NULL)
  {
    /* write the three bytes of the UATI024 individually into their respective
       bytes of the UATI */
    hdraddr_set_octet(uati_ptr, ((uint8)(uati_24>>16)),13);
    hdraddr_set_octet(uati_ptr, 
            ((uint8)((uati_24 & 0x0000FF00) >> 8)), 14);
    hdraddr_set_octet(uati_ptr,
            ((uint8)(uati_24 & 0x000000FF)), 15);
  }

} /* hdramp_set_uati024 */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SUBNET_HAS_CHANGED

DESCRIPTION
  This helper function determines if the subnet mask or subnet has changed.

  |--------- UATISubnetMask bits ---------|--(128 - UATISubnetMask)--|

   --------------------------------------- --------------------------
  |                  subnet               |                          | 
  |  - - - - -   - - - - - - - - - - - -  | addresses within subnet  |
  |  supernet  | subnets within supernet  |                          |
   --------------------------------------- --------------------------

  |-- N bits --|--------- M bits ---------|

  N = (UATISubnetMask - ReducedSubnetMaskOffset)
  M = ReducedSubnetMaskOffset

DEPENDENCIES
  None

PARAMETERS
  sp_sector_id - Sector ID from current SectorParameters message
  sp_subnet_mask - SubnetMask from current SectorParameters message
  full_uati - Current full UATI
  uati_subnet_mask - SubnetMask of current UATI

RETURN VALUE
  HDRAMP_SUBNET_ERROR - If there was an error during the routine
  HDRAMP_SUBNET_UNCHANGED - The subnet did not change
  HDRAMP_SUBNET_CHANGED_SAME_SUPERNET - The upper 'supernet' bits are the same,
    but one or more of the lower subnet bits are different
  HDRAMP_SUBNET_CHANGED_NEW_SUPERNET - One or more of the 'supernet' bits are
    different

SIDE EFFECTS
  None

===========================================================================*/
static hdramp_subnet_changed_enum_type hdramp_subnet_has_changed 
( 
  hdraddr_type sp_sector_id,
  uint8        sp_subnet_mask,
  hdraddr_type full_uati,
  uint8        uati_subnet_mask,
  uint8        reduced_subnet_mask_offset
)
{
  hdraddr_type sector_subnet; /* Address of the subnet as determined from the
                                 sector ID and mask  from OVHD */
  hdraddr_type uati_subnet;   /* Address of the subnet as defined using AMP's
                                 UATI values */

  /* The common subnet mask shared by the UATI and the current sector */
  uint8 subnet_mask;

  /* Mask of the reduced subnet (supernet) */
  uint8 reduced_subnet_mask = 0;

  /* Holds return value */
  hdramp_subnet_changed_enum_type ret_val;

  void* tmp_ptr1;
  void* tmp_ptr2;
    /* Temporary pointer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Use void pointer as a bridge to avoid the following compiler warning: 
     "cast increases required alignment of target type". */
  tmp_ptr1 = (void *)&sp_sector_id[12];
  tmp_ptr2 = (void *)&full_uati[12];
  HDR_MSG_PROT_5(MSG_LEGACY_MED, 
                 "subnet_has_changed: OTA net:%x/%d AT net:%x/%d, reduced_mask %d",
                 *(uint32*)tmp_ptr1, sp_subnet_mask,
                 *(uint32*)tmp_ptr2, uati_subnet_mask,
                 reduced_subnet_mask_offset);

  /* If the subnet mask has changed, the subnet has changed */
  if( sp_subnet_mask != uati_subnet_mask )
  {
    ret_val = HDRAMP_SUBNET_CHANGED_NEW_SUPERNET;
  }
  else
  {
    /* For clarity, use a common variable name since both subnet masks
       are the same */
    subnet_mask = sp_subnet_mask;

    /* Create the ReducedSubnetMask from the current sector */
    reduced_subnet_mask = subnet_mask - reduced_subnet_mask_offset;

    /* Compare subnets */
    hdraddr_mask( sector_subnet, sp_sector_id, subnet_mask );
    hdraddr_mask( uati_subnet, full_uati, subnet_mask );

    /* If the subnets are equal, then there was no change */
    if( hdraddr_is_equal( sector_subnet, uati_subnet ) )
    {
      ret_val = HDRAMP_SUBNET_UNCHANGED;
    }
    else
    {
      /* Use ReducedSubnetMask to check for supernet membership */
      hdraddr_mask( sector_subnet, sp_sector_id, reduced_subnet_mask );
      hdraddr_mask( uati_subnet, full_uati, reduced_subnet_mask );
  
      /* If the ReducedSubnetMasks differ then the AT has changed supernets */
      if( hdraddr_is_equal( sector_subnet, uati_subnet ) )
      {
        ret_val = HDRAMP_SUBNET_CHANGED_SAME_SUPERNET;
      }
      else
      {
        ret_val = HDRAMP_SUBNET_CHANGED_NEW_SUPERNET;
      }
    }
  }

  return ret_val;

} /* hdramp_subnet_has_changed */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_RESET_DATA

DESCRIPTION
  This helper function resets all address data and local data/flags for
    entering INACTIVE state.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_reset_data(void)
{

  /* receive ati record to be added to the list */
  hdrhai_ati_record_type ati_record; 

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set OldUATI to NULL */
  memset(hdramp.old_uati, 0, HDRADDR_LENGTH_IN_BYTES);

  /* Clear the Receive ATI list */
  hdrampati_receive_ati_clear();

  /* Set TransmitATI to <ATIType = INACTIVE, ATI = NULL> */
  ati_record.ati_type = HDRHAI_ATI_INACTIVE;
  ati_record.ati_value = 0x00000000;
  hdramp_set_transmit_ati(ati_record);

  /* Set UATI to NULL */
  memset(hdramp.session_info.full_uati, 0, HDRADDR_LENGTH_IN_BYTES);

  /* Set UATIColorCode to NULL */
  hdramp.session_info.uati_cc = 0x00000000;

  /* Set UATISubnetMask to NULL */
  hdramp.session_info.uati_subnet_mask = 0x00;

  /* Clear the location information */
  hdramp.session_info.location.initialized = FALSE;

  hdramp.session_info.max_no_monitor_distance = 0;
  hdramp.session_info.support_secondary_colorcodes = 0;
  hdramp.session_info.reduced_subnet_mask_offset = 0;

} /* hdramp_reset_data */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SEND_UATI_REQ_MSG

DESCRIPTION
  This helper function prepares and sends a UATI Request Message.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_send_uati_req_msg(void)
{
  hdrampi_uati_request_msg_type uati_req_msg; /* structure holding the data
                                                 for the UATI Request Msg */
  dsm_item_type * send_msg_ptr;               /* DSM item pointer to store
                                                 packed message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* assign values for UATI Request Msg fields */
  uati_req_msg.message_id     = HDRAMP_UATI_REQ_MSG;
  uati_req_msg.transaction_id = ++hdramp.uati_req_trans_id;
  hdramp.uati_asgn_received   = FALSE;
#ifdef FEATURE_LTE_TO_HDR_OH
  hdramp.waiting_uati_asgn = TRUE;
  hdramp.backoff_sending_uati = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* get new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* PACK UATI Request Msg fields */
  HDRAMP_PACK_UATI_REQ_MSG_ID(send_msg_ptr,uati_req_msg);
  HDRAMP_PACK_UATI_REQ_TRANS_ID(send_msg_ptr,uati_req_msg);

  hdrhmp_send_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, /* prot sending msg */
                   HDRHAI_AC, /* AC or RTC */
                   FALSE, /* Reliable = TRUE, Best effort = FALSE */
                   HDRAMP_UATI_MESSAGE_PRIORITY, /* priority */
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr, /* pointer to the message to be delivered */
                   hdramp_uati_req_tx_outcome_cb, /* outcome function */
                   NULL); /* data to deliver to outcome callback */

  /* The access terminal shall initialize the assignment validation value to 
     255 when it sends a UATIRequest Message and the RATI has a value.  When 
     the RATI has a value, no UATI has been assigned yet, so the assignment
     validation sequence number can be re-initialized. */

  /* make sure that we had a rati in the receive list */
  if (hdrampati_receive_rati_is_valid())
  {
    hdramp.session_info.uati_assign_validation_seq_num = 
                                                     HDRAMP_INITIALIZE_MSG_SEQ;
  }

  if(hdramp.session_info.transmit_ati.ati_type == HDRHAI_RATI)
  {
    /* log the start of a session attempt */
    hdrplog_start_session_attempt(uati_req_msg.transaction_id,
                                  hdramp.session_info.transmit_ati.ati_value);
  }

} /* hdramp_send_uati_req_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_PREVIOUS_UATI

DESCRIPTION
  This function returns the previous session's UATI

DEPENDENCIES
  None

PARAMETERS
  previous_uati_ptr - pointer to the previous UATI

RETURN VALUE
  E_SUCCESS - Value returned through the pointer is a valid UATI
  E_FAILURE - Value returned through the pointer is not a valid UATI

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdramp_get_previous_uati
(
  hdraddr_type previous_uati_ptr
)
{
  if (previous_uati_ptr != NULL)
  {
    hdraddr_set(previous_uati_ptr, hdramp.prior_session_info.full_uati);
    return E_SUCCESS;
  }
  return E_FAILURE;

} /* hdramp_get_previous_uati */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_TRANSMIT_ATI

DESCRIPTION
  This function returns the transmit ATI

DEPENDENCIES
  None

PARAMETERS
  transmit_ati_ptr - pointer to the address where the transmit ATI should
                     be placed
  sender - protocol which requested the transmit ati

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid Transmit address
  E_FAILURE - Value returned to the pointer is not a valid Transmit address

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdramp_get_transmit_ati
(
  hdrhai_ati_record_type * transmit_ati_ptr,
  hdrhai_protocol_name_enum_type sender
)
{
  /* if the transmit ATI has a valid type */
  if (hdramp.session_info.transmit_ati.ati_type != HDRHAI_ATI_INACTIVE)
  {
    /* copy type and value into return parameter */
    transmit_ati_ptr->ati_type  = hdramp.session_info.transmit_ati.ati_type;
    transmit_ati_ptr->ati_value = hdramp.session_info.transmit_ati.ati_value;

    /* also save this value as the last ATI given to ACMAC */
    if ( sender == HDRHAI_AC_MAC_PROTOCOL )
    {
      hdrampati_set_last_acmac_ati( transmit_ati_ptr->ati_value );
    }

    /* E_SUCCESS = TRUE, E_FAILURE = FALSE for this trace */
    HDRTRACE_HDRAMP_GET_TRANSMIT_ATI(sender, TRUE); //lint !e506
    return (E_SUCCESS);
  }
  else /* the transmit ATI does not have a valid type */
  {
    /* set type and value in return parameter to inactive/NULL */
    transmit_ati_ptr->ati_type = HDRHAI_ATI_INACTIVE;
    transmit_ati_ptr->ati_value = 0x00000000;

    /* E_SUCCESS = TRUE, E_FAILURE = FALSE for this trace */
    HDRTRACE_HDRAMP_GET_TRANSMIT_ATI(sender, FALSE); //lint !e506
    return (E_FAILURE);
  }

} /* hdramp_get_transmit_ati */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_UATI_COLOR_CODE

DESCRIPTION
  This function returns the UATI color code

DEPENDENCIES
  None

PARAMETERS
  uati_color_code - Pointer to location where color code need to copied 

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid
  E_FAILURE - Value returned to the pointer is not a valid

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdramp_get_uati_color_code
(
  uint8 *uati_color_code
)
{
  /* if the transmit ATI has a valid type */
  if (hdramp.session_info.transmit_ati.ati_type != HDRHAI_ATI_INACTIVE)
  {
    /* copy value into return parameter */
    *uati_color_code = hdramp.session_info.uati_cc;
    return (E_SUCCESS);
  }
  else /* the transmit ATI does not have a valid type */
  {
    /* set value in return parameter to NULL */
    *uati_color_code = 0;
    return (E_FAILURE);
  }

} /* hdramp_get_uati_color_code() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_GET_UATI_SUBNET_MASK

DESCRIPTION
  This function returns the UATI subnet mask

DEPENDENCIES
  None

PARAMETERS
  uati_subnet_mask - Pointer to location where subnet mask need to copied 

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid
  E_FAILURE - Value returned to the pointer is not a valid

SIDE EFFECTS
  None

===========================================================================*/

hdrerrno_enum_type hdramp_get_uati_subnet_mask
(
  uint8 *uati_subnet_mask
)
{
  /* if the transmit ATI has a valid type */
  if (hdramp.session_info.transmit_ati.ati_type != HDRHAI_ATI_INACTIVE)
  {
    /* copy value into return parameter */
    *uati_subnet_mask = hdramp.session_info.uati_subnet_mask;
    return (E_SUCCESS);
  }
  else /* the transmit ATI does not have a valid type */
  {
    /* set value in return parameter to NULL */
    *uati_subnet_mask = 0;
    return (E_FAILURE);
  }

} /* hdramp_get_uati_subnet_mask() */
/* <EJECT> */ 

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRAMP_GET_UATI_SUBNET

DESCRIPTION
  This function returns the subnet in the UATI assignment.

DEPENDENCIES
  None

PARAMETERS
  uati_subnet - Pointer to location where the subnet will get copied 

RETURN VALUE
  E_SUCCESS - Value returned to the pointer is a valid
  E_FAILURE - Value returned to the pointer is not a valid

SIDE EFFECTS
  None

===========================================================================*/
hdrerrno_enum_type hdramp_get_uati_subnet
(
  hdraddr_type uati_subnet
)
{
  /* if the transmit ATI has a valid type and UATI is fresh (we are not
   * waiting for UATIAssignment msg) */
  if ( ( hdramp.session_info.transmit_ati.ati_type == HDRHAI_UATI ) && 
       ( rex_get_timer( &hdramp.uati_assignment_timer ) == 0 ) )
  {
    hdraddr_mask( uati_subnet, hdramp.session_info.full_uati, 
                  hdramp.session_info.uati_subnet_mask );
    return E_SUCCESS;
  }
  else
  {
    if ( ( hdramp.session_info.transmit_ati.ati_type == HDRHAI_UATI ) && 
         !rex_get_timer( &hdramp.uati_assignment_timer ) )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "Waiting for UATIAssignment. Can't get subnet from UATI" );
    }
    memset( uati_subnet, 0, HDRADDR_LENGTH_IN_BYTES );
    return E_FAILURE;
  }

} /* hdramp_get_uati_subnet() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/*===========================================================================

FUNCTION HDRAMP_GET_SESSION_SEED

DESCRIPTION
  This function returns the session seed

DEPENDENCIES
  None

PARAMETERS
  sender - protocol which requested the session seed

RETURN VALUE
  uint32 - the session seed

SIDE EFFECTS
  None

===========================================================================*/

uint32 hdramp_get_session_seed
(
  hdrhai_protocol_name_enum_type sender
)
{
  HDRTRACE_HDRAMP_GET_SESSION_SEED(sender);
  return (hdramp.session_info.session_seed);
} /* hdramp_get_session_seed */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_UATI_CC_DIFF_FROM_SECONDARY_CC

DESCRIPTION
  This function checks if the AT's UATIColorCode is different from all of
  the SecondaryColorCode values retrieved from the current sector parameter
  message.

DEPENDENCIES
  None

PARAMETERS
  Current - compare against current CC code or CC from previous session

RETURN VALUE
  TRUE if SupportSecondaryColorCodes is set to 0x00 or UATIColorCode is
  different from all of the SecondaryColorCode values provided as public
  data by the Overhead Messages Protocol. FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static boolean hdramp_uati_cc_diff_from_secondary_cc
(
  boolean current
)
{
  uint8 sec_cc_count;
    /* Number of ColorCodes in SecondaryColorCodes list */
  uint8 sec_cc_list[HDROVHD_MAX_SEC_COLORCODES];
    /* List of SecondaryColorCodes */
  uint8 support_secondary_colorcodes;
    /* Flag indicating if SecondaryColorCode is supported */
  int i;
    /* Loop index */
  boolean cc_are_different = TRUE;
  uint8 uati_cc;
/*-------------------------------------------------------------------------*/

  if (current)
  {
    /* Retrieve the InUse value of the SupportSecondaryColorCodes attrib */
    (void) hdrscmdb_get_inuse_attrib( 
                        HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
                        HDRSCMAMP_SUPPORT_SECONDARY_COLORCODES_ATTRIB_ID,
                        &support_secondary_colorcodes,
                        HDRSCP_DEFAULT_AMP );
    uati_cc = hdramp.session_info.uati_cc;
  }
  else
  {
    /* support_secondary_colorcodes and uati_cc were saved when
       previous session was deactivated. */ 
    support_secondary_colorcodes = 
        hdramp.prior_session_info.support_secondary_colorcodes;
    uati_cc = hdramp.prior_session_info.uati_cc;
  }

  /* If SecondaryColorCode is not supported or is not retrievable from Overhead
     Message Protocol treat this as UATI_CC being different from all Secondary
     ColorCodes */
  if ( (!support_secondary_colorcodes) || 
       (hdrovhd_get_secondary_color_codes( &sec_cc_count, sec_cc_list ) !=
          E_SUCCESS) )
  {
    return TRUE;
  }

  /*  "If the access terminal is to consider all possible values of Secondary
      ColorCode to be included in this message, then the access network shall
      set this field to ‘000’." Since the SecondaryColorCode list covers the
      entire set, UATI_CC will match some SecondaryColorCode. */
  if ( sec_cc_count == 0 )
  {
    cc_are_different = FALSE;
  }

  /* Check if the UATI_CC is different from all the SecondaryColorCode */
  for (i = 0; i < sec_cc_count; i++)
  {
    if (uati_cc == sec_cc_list[i])
    {
      cc_are_different = FALSE;
      break;
    }
  }

  return cc_are_different;
} /* hdramp_uati_cc_diff_from_secondary_cc() */


#ifdef FEATURE_LTE_TO_HDR_OH
/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAMP_SET_HDR_TO_LTE_RESEL

DESCRIPTION
  This function set hdr_to_lte_resel flag 

DEPENDENCIES
  None

PARAMETERS
  resel - Whether we just successful complete hdr to lte reselection 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdramp_set_hdr_to_lte_resel
(
  boolean resel
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "AMP resel: %d", resel );
  hdramp.hdr_to_lte_resel = resel;

  if ( resel )
  {
    /* resel set to TRUE, means we are entering tunnel */
    hdramp.entering_tunnel = TRUE;
  }

} /* hdramp_set_hdr_to_lte_resel() */


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAMP_SET_FIRST_ENTERING_TUNNEL

DESCRIPTION
  This function set entering_tunnel flag 

DEPENDENCIES
  None

PARAMETERS
  entering_tunnel - Whether this is the first time we enters tunnel mode 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdramp_set_entering_tunnel
(
  boolean entering_tunnel
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "AMP entering_tunnel: %d", entering_tunnel );
  hdramp.entering_tunnel = entering_tunnel;

  if ( !entering_tunnel )
  {
    /* Not first time entering tunnel, also reset resel flag */
    hdramp.hdr_to_lte_resel = FALSE;
  }

} /* hdramp_set_entering_tunnel() */
#endif /* FEATURE_LTE_TO_HDR_OH */


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAMP_PROCESS_OVHD_INFO_UPDATED

DESCRIPTION
  This function processes the OVHD.InfoUpdated Indication

  This indication simulates the INIT.NetworkAcquired Indication.

  If the is_first_after_acq associated data is set to TRUE, this is the
  first InfoUpdated after the last network acquisition, so behave as if
  the INIT.NetworkAcquired Indication was received.

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - points to indication data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdramp_process_ovhd_info_updated
(
  hdrind_ind_data_union_type *ind_data_ptr 
)
{
  /* Color code from OVHD */
  uint8 ovhd_color_code = 0;

  /* Flag that the AT has traveled further than MaxNoMonitorDistance since
     it last received a SectorParameters message */
  boolean trigger_no_monitor_distance = FALSE;

  /* Subnet changed return value */
  hdramp_subnet_changed_enum_type subnet_changed = HDRAMP_SUBNET_CHANGED_NEW_SUPERNET;

  /* Location given by the most recent SP message */
  hdrovhd_sp_msg_location_type sp_msg_location;

  /* SectorID and SubnetMask from current SectorParameters message */
  hdraddr_type sp_sector_id;
  uint8 sp_subnet_mask;
  boolean previous_session_valid = FALSE;
  hdramps_state_enum_type amp_state;
  /* is CC from current/prev UATI in SP PARAM's allowed list */
  boolean cc_diff = FALSE;
  /* if AMP needs to request a UATI from AN, this may or may not cause a
     HDRIND_AMP_FAILED_NEW_NETWORK indication */
  boolean new_network = TRUE;
  boolean loc_info_in_sp;
  boolean matched_current_session = FALSE;
  boolean amp_setup_pending;

  void* tmp_ptr;
    /* Temporary pointer */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT( MSG_LEGACY_LOW, 
                "Entering hdramp_process_ovhd_info_updated()" );

  /* prev. UATI is used if
       1) current UATI invalid, or
       2) prev UATI is a closer match */
  previous_session_valid = hdrscmdb_is_silent_restore_enabled();
  amp_state = hdramps_get_state();

  amp_setup_pending = hdramp.amp_setup_pending;
  hdramp.amp_setup_pending = FALSE;

  /* Only process OvhdInfoUpdatedInd in OPEN state per standard.
   * We also process it if the previous session is valid */
  if ((amp_state != HDRAMP_OPEN) && (!previous_session_valid))
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
        HDRIND_OVHD_INFO_UPDATED, hdramps_get_state() );
    return;
  }

  /* Get the color code */
  if ( hdrovhd_get_color_code( &ovhd_color_code,
                               HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) 
       != E_SUCCESS )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not get Color Code from OVHD" );
    return;
  }

  /* Retrieve the InUse value of the ReducedSubnetMaskOffset attrib */
  (void) hdrscmdb_get_inuse_attrib( 
      HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
      HDRSCMAMP_REDUCED_SUBNET_MASK_OFFSET_ATTRIB_ID,
      &hdramp.session_info.reduced_subnet_mask_offset,
      HDRSCP_DEFAULT_AMP );

  /* Get the current SectorID */
  if( hdrovhd_get_sector_id( sp_sector_id, &sp_subnet_mask,
      HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) == E_SUCCESS )
  {
  /* Check if the subnet has been changed */
    subnet_changed = 
      hdramp_subnet_has_changed( sp_sector_id, sp_subnet_mask,
                                 hdramp.session_info.full_uati,
                                 hdramp.session_info.uati_subnet_mask,
                                 hdramp.session_info.reduced_subnet_mask_offset );
  }
  else
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not get Sector ID from OVHD" );
    subnet_changed = HDRAMP_SUBNET_ERROR;
  }

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  /* delay processing of OVHD_INFO_UPDATED after DCM is off */
  if (!hdrsmp_is_to_maintain_evdo_session()) 
  {
    /* if reacqed on a different subnet during DCM, need to
       act as re-acq instead of idle hand-off after DCM is off.
       Since trigger_no_monitor_distance is not taken into account
       the current check is slightly sub-optimal, but close enough */   
    if (subnet_changed == HDRAMP_SUBNET_UNCHANGED)
    {
      hdramp.dcm_iho_count = 0;
    }
    else
    {
      hdramp.dcm_iho_count++;
    }

    if (((ind_data_ptr->ovhd_updated_is_first_after_acq) &&
         (subnet_changed != HDRAMP_SUBNET_UNCHANGED)) ||
        (hdramp.dcm_iho_count > 1))
    {
      hdramp.dcm_reacq_diff_subnet = TRUE;
    }

    HDR_MSG_PROT_1(MSG_LEGACY_HIGH, "DCM: Recv. InfoUpdated during DCM. Subnet Changed:%d", 
                   subnet_changed);
    return;
  }
  /* if AT reacq-ed during DCM, clear the flag when DCM is off */
  else if (hdramp.dcm_reacq_diff_subnet)
  {
    hdramp.dcm_reacq_diff_subnet = FALSE;
    hdramp.dcm_iho_count = 0;
  }
#endif

  /* Retrieve the current latitude and longitude from the most
     recent SP message (if possible) */
  if (hdrovhd_get_sp_msg_location(&sp_msg_location,
          HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) == E_SUCCESS)
  {
    loc_info_in_sp = TRUE;
  }
  else
  {
    loc_info_in_sp = FALSE;
    HDR_MSG_PROT (MSG_LEGACY_MED, "No location info from SP MSG");
  }

  if (hdralmp_connection_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL))
  {
    if (loc_info_in_sp)
    {
      hdramp.session_info.location.initialized = TRUE;
      hdramp.session_info.location.latitude = sp_msg_location.latitude;
      hdramp.session_info.location.longitude = sp_msg_location.longitude;
      hdramp_write_session_info_to_db();
    }
    HDR_MSG_PROT(MSG_LEGACY_HIGH, 
        "Ignored OVHD_INF_UPD while connected");
    return;
  }

  /* Try to match agains current session first */
  if (amp_state == HDRAMP_OPEN)
  {
    /* Retrieve the InUse value of MaxNoMonitorDistance */
    (void) hdrscmdb_get_inuse_attrib(
                       HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
                       HDRSCMAMP_MAX_NO_MONITOR_DISTANCE_ATTRIB_ID,
                       &hdramp.session_info.max_no_monitor_distance,
                       HDRSCP_DEFAULT_AMP );

    trigger_no_monitor_distance = FALSE;
    if (loc_info_in_sp)
    {
      if (hdramp.session_info.location.initialized && 
          (hdramp.session_info.max_no_monitor_distance != 0))
      {
        HDR_MSG_PROT_5(MSG_LEGACY_MED, 
                       "Loc. Info: Current %u/%u, OTA %u/%u, MaxNoMon Dist:%d",
                       hdramp.session_info.location.latitude, 
                       hdramp.session_info.location.longitude,
                       sp_msg_location.latitude, sp_msg_location.longitude,
                       hdramp.session_info.max_no_monitor_distance);
  
        trigger_no_monitor_distance = 
          reg_dist( hdramp.session_info.location.latitude, 
                    sp_msg_location.latitude, /* represents the value Xn */
                    hdramp.session_info.location.longitude, 
                    sp_msg_location.longitude, /* represents the value Yn */
                    hdramp.session_info.max_no_monitor_distance);
      }
    }

    cc_diff = hdramp_uati_cc_diff_from_secondary_cc(TRUE);

    if (((subnet_changed != HDRAMP_SUBNET_UNCHANGED) && 
         trigger_no_monitor_distance) ||
        ((subnet_changed == HDRAMP_SUBNET_CHANGED_NEW_SUPERNET) &&
         ind_data_ptr->ovhd_updated_is_first_after_acq))
    {
      new_network = TRUE;
      matched_current_session = FALSE;
    }
    else
    {
      new_network = FALSE;
      matched_current_session = TRUE;

      HDR_MSG_PROT_4(MSG_LEGACY_LOW, 
                     "Updating Loc Info,Lat:%u->%u,Long:%u->%u",
                     hdramp.session_info.location.latitude, 
                     sp_msg_location.latitude, 
                     hdramp.session_info.location.longitude,
                     sp_msg_location.longitude);

      hdramp.session_info.location.initialized = TRUE;
      hdramp.session_info.location.latitude = sp_msg_location.latitude;
      hdramp.session_info.location.longitude = sp_msg_location.longitude;
      hdramp_write_session_info_to_db();
    }
  }

  /* if OTA subnet doesn't match current subnet, try to
   * match against prev. subnet. */
  if (new_network && previous_session_valid &&
      ind_data_ptr->ovhd_updated_is_first_after_acq)
  {
    subnet_changed = hdramp_subnet_has_changed(
        sp_sector_id, sp_subnet_mask,
        hdramp.prior_session_info.full_uati,
        hdramp.prior_session_info.uati_subnet_mask,
        hdramp.prior_session_info.reduced_subnet_mask_offset);

    trigger_no_monitor_distance = FALSE;
    if (loc_info_in_sp)
    {   
      if (hdramp.prior_session_info.location.initialized &&
          (hdramp.prior_session_info.max_no_monitor_distance != 0))
      {
        HDR_MSG_PROT_5(MSG_LEGACY_MED, 
                       "Loc. Info: Prev. %u/%u, OTA %u/%u, MaxNoMon Dist:%d",
                       hdramp.prior_session_info.location.latitude,
                       hdramp.prior_session_info.location.longitude,
                       sp_msg_location.latitude, sp_msg_location.longitude,
                       hdramp.prior_session_info.max_no_monitor_distance);
        trigger_no_monitor_distance = 
          reg_dist( hdramp.prior_session_info.location.latitude, 
                    sp_msg_location.latitude, /* represents the value Xn */
                    hdramp.prior_session_info.location.longitude, 
                    sp_msg_location.longitude, /* represents the value Yn */
                    hdramp.prior_session_info.max_no_monitor_distance );
      }
    }

    cc_diff = hdramp_uati_cc_diff_from_secondary_cc(FALSE);

    if (((subnet_changed != HDRAMP_SUBNET_UNCHANGED) &&
          trigger_no_monitor_distance) ||
         (subnet_changed == HDRAMP_SUBNET_CHANGED_NEW_SUPERNET))
    {
      /* OTA does not match prior session. */
    }
    else
    {
      new_network = FALSE;
    }
  }

  HDR_MSG_PROT_6(MSG_LEGACY_MED, 
      "AmpOvhInfUpd State:%d Prev:%d CCDiff:%d Dist>:%d Subnet:%d 1stAcq:%d",
      (uint32)amp_state, (uint32) previous_session_valid, 
      (uint32)cc_diff, (uint32)trigger_no_monitor_distance,
      subnet_changed, (uint32)ind_data_ptr->ovhd_updated_is_first_after_acq);

  /* Use void pointer as a bridge to avoid the following compiler warning: 
     "cast increases required alignment of target type". */
  tmp_ptr = (void *)&hdramp.session_info.full_uati[12];
  if (matched_current_session)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, 
        "AmpOvhInfUpd matched current subnet:%X/%d",
        *(uint32*)tmp_ptr, 
        (uint32)hdramp.session_info.uati_subnet_mask);
  }
  else if (!new_network)
  {
    HDR_MSG_PROT_2(MSG_LEGACY_MED, 
        "AmpOvhInfUpd matched prev subnet:%X/%d",
        *(uint32*)tmp_ptr, 
        (uint32)hdramp.prior_session_info.uati_subnet_mask);
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( !hdramp.rxed_ovhd_after_irat_iho )
  {
    HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                    "IRAT_HO: restore %d; subnet_changed %d, prepreg %d",
                    new_network, 
                    subnet_changed,
                    hdramp.last_prereg_is_allowed );

    /* We may be able to use ind_data_ptr->ovhd_updated_is_first_after_acq flag
     * but it's better to use new flag to indicate that this is first ovhd after
     * LTE to HDR idle handoff */
    hdramp.rxed_ovhd_after_irat_iho = TRUE; 

    /* LTE->DO handoff should follow native HRPD behavior except for one case: 
     * PreReg is allowed in the last LTE cell, move to DO not due to OOS,
     * and subnet is changed. In this case we can simply update UATI instead of 
     * restoring prior session becasue it's similar to idle ho to a new subnet.
     */
    if ( hdramp.last_prereg_is_allowed && 
         new_network &&
         ( ( subnet_changed == HDRAMP_SUBNET_CHANGED_NEW_SUPERNET ) &&
           ind_data_ptr->ovhd_updated_is_first_after_acq ) )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "IRAT_HO: prereg allowed, diff Subnet" );
      new_network = FALSE;
      matched_current_session = TRUE;
    }
  }

  /* Received HDR OVHD, it means that we acquire HDR, reset the entering_tunnel flag*/
  if ( hdramp.entering_tunnel )
  {
    hdramp_set_entering_tunnel( FALSE );
  }
  hdramp.current_cell_prereg = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

  if ((amp_state != HDRAMP_OPEN) && new_network)
  {
    if (amp_setup_pending)
    {
      HDR_MSG_PROT(MSG_LEGACY_MED, "Continue with AMP SETUP");
      hdramp_send_uati_req_msg();
    }
  }
  else if (new_network)
  {
    /* If the subnets have changed, generate the AMP.FailedNewNetwork 
       or AMP.FailedNotMaintainingUATI Indication.
       AMP does not change state at this point, SMP will deactivate it. */
    if (hdrsmp_is_to_maintain_evdo_session())
    {
      hdrind_give_ind(HDRIND_AMP_FAILED_NEW_NETWORK, NULL);
      HDR_MSG_PROT(MSG_LEGACY_MED, "OVHD: FAILED_NEW_NETWORK");
    }
    else
    {
      hdrind_give_ind(HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI, NULL);
      HDR_MSG_PROT (MSG_LEGACY_MED, "MUM: give NOT_MAINTAINING_UATI ind");
    }

#ifdef FEATURE_LTE_TO_HDR_OH
    /* Act as if we have received IRMA msg. This is because at this
     * point, we know that RestorePriorSession will be triggered.
     * In RPS, a ConnectionRequest will be sent to AN. Sending CR is 
     * the same as sending IRMI. 
     * If open session fails, AT will continue to send ConnecitonRequest 
     * to establish a session. */
    if ( hdramp.irmi_is_pending )
    {
      hdramp.irmi_is_pending = FALSE;
      hdramp.irmi_cnt = 0;
      hdrind_give_ind( HDRIND_AMP_IRMI_ACCEPTED, NULL );
    }
#endif /* FEATURE_LTE_TO_HDR_OH */

  }
  else 
  {
    if (matched_current_session)
    {
      if (cc_diff && (subnet_changed != HDRAMP_SUBNET_UNCHANGED))
      {
        HDR_MSG_PROT(MSG_LEGACY_MED, "CC Diff, keep session, req. UATI");
        hdramp_request_new_ati( sp_sector_id );
      }
    }
    else
    {
      HDR_MSG_PROT(MSG_LEGACY_MED, "matched prev UATI, restoring prev session");
      if (cc_diff && (subnet_changed != HDRAMP_SUBNET_UNCHANGED))
      {
        HDR_MSG_PROT(MSG_LEGACY_MED, "CC Diff, restore prev session, req. UATI");
        hdramp.req_uati_for_prev = TRUE;
        hdraddr_set(hdramp.prev_uati_sect_id, sp_sector_id);
      }
      hdrind_give_ind(HDRIND_AMP_OPENED_PREV_SESSION, NULL);
    }

    /* Clear UATIAssignment Timer if moved back to original
     * subnet or decided to restore prior session, in which case
     * AT will send UATI request after session is restored */
    if ((subnet_changed == HDRAMP_SUBNET_UNCHANGED))
    {
      /* We are back on the subnet that assigned our current UATI but we were
       * previously on a different subnet and sent a UATI request there.
       * There is no reason to worry about that old UATI request any more.
       * Also note that we do not perform AHO while probing a UATIRequest.
       */
      (void)rex_clr_timer(&hdramp.uati_assignment_timer);
      hdramp.uati_asgn_received = TRUE;

#ifdef FEATURE_LTE_TO_HDR_OH
      hdramp.waiting_uati_asgn = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

      #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
      if (hdrslp_is_waiting_for_uati_assignment())
      {
        if( rex_get_timer( &hdramp.uati_assignment_timer ) != 0 )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "SLPQH: IdleHO back to original subnet %d",
                          ovhd_color_code);
        }
        else
        {
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "SLPQH: AHO back to original subnet %d",
                          ovhd_color_code);
        }
        hdramp_done_waiting_for_uati_assignment( 
            HDRAMP_CLEAR_SLP_HOLDOFF_BACK_IN_ORIG_SUBNET );
      }
      #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
    }
  }

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdramp.irmi_is_pending )
  {
    hdramp.irmi_is_pending = FALSE;
    if ( !hdramp_send_irmi_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) )
    {
      hdramp.irmi_cnt = 0;
      hdrind_give_ind( HDRIND_AMP_IRMI_FAILED, NULL );
    }
  }
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdramp_process_ovhd_info_updated */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRAMP_PROCESS_LTE_CELL_CHANGED

DESCRIPTION
  This function processes the HDRIND_OVHD_LTE_CELL_CHANGED indication.

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to indication data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdramp_process_lte_cell_changed
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  lte_rrc_global_cell_id_t curr_lte_cell_id = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( hdrovhd_get_lte_cell_id(&curr_lte_cell_id) == E_SUCCESS )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "LTE CELL ID:%d", curr_lte_cell_id );
  }

  /* UE could have moved across several cells before entering a cell in which
   * preReg is allowed. We would like UE to perform restore prior session
   * rather than update UATI if the above case happens, so we reset 
   * hdr_to_lte_resel flag if UE enters a different cell. 
   */ 
  if ( !ind_data_ptr->first_cell_changed_ind )
  {
    hdramp.hdr_to_lte_resel = FALSE;
  }
  else
  {
    /* If it is the first cell changed indication, it means we are entering
     * LTE for the first time. Reset lte cell ID since it is used for
     * LTE to LTE scenario. */
    hdramp.lte_cell_id = 0;
    hdramp.lte_cell_id_last = 0;
  }

  /* Check if it is
   * LTE1 (preReg allowed) -> LTE2 (preReg not allowed)-> LTE1 case. */
  if ( curr_lte_cell_id == hdramp.lte_cell_id_last )
  {
    HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                    "Move to CELL (%d), ping-pong between CELL(%d) and CELL(%d)", 
                    curr_lte_cell_id, curr_lte_cell_id, hdramp.lte_cell_id );
    hdramp.back_to_same_cell = TRUE;
    hdramp.same_cell_last_prereg = hdramp.prev_cell_prereg;
  }
  else
  {
    hdramp.back_to_same_cell = FALSE;
    hdramp.same_cell_last_prereg = FALSE;
  }

  /* Remember the current and previous cell id */
  hdramp.lte_cell_id_last = hdramp.lte_cell_id;
  hdramp.lte_cell_id = curr_lte_cell_id; 

  /* Remember the previous cell preReg value when we enters a new cell */
  hdramp.prev_cell_prereg = hdramp.current_cell_prereg;

  /* UE has entered to a different LTE cell or comes from HDR */
  hdramp.in_same_cell = FALSE;

} /* hdramp_process_lte_cell_changed */


/*===========================================================================
FUNCTION HDRAMP_PROCESS_PREREG_INFO_UPDATED

DESCRIPTION
  This function processes the PreregInfoUpdated Indication

DEPENDENCIES
  None

PARAMETERS
  ind_data_ptr - pointer to indication data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdramp_process_prereg_info_updated
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  /* ColorCode */
  uint8 ovhd_color_code = 0;

  /* Whether CC from current UATI in allowed list */
  boolean sec_cc_is_diff = FALSE;

  /* Whether prerregistration zone has been changed */
  boolean prereg_zone_changed = FALSE;

  boolean prev_prereg_allowed = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  HDR_MSG_PROT( MSG_LEGACY_LOW, 
                "Entering hdramp_process_prereg_info_updated()" );

  /* For the reason that whether preReg is allowed may be different in SIB8
   * and preReg info from the same LTE cell, we want to know whether a change 
   * from preReg 0 (not allowed) to 1 (allowed) should trigger a UATI update/ 
   * restore prior session. 
   * If UE has rx'ed preReg in the same LTE cell before, do nothing;
   * If UE just moves from a different cell (or from HDR)
   * 1. PreReg is not allowed: do nothing;
   * 2. PreReg is allowed, do whatever should do and update the flag which
   * indicates that from now on UE are moving in the same LTE cell until
   * a cell changed indication is received. 
   */
  hdramp.current_cell_prereg = ind_data_ptr->prereg_is_allowed;
  HDR_MSG_PROT_3( MSG_LEGACY_HIGH,
                  "LTE cell change %d, prev cell preReg %d, current preReg %d",
                  hdramp.in_same_cell,
                  hdramp.prev_cell_prereg,
                  hdramp.current_cell_prereg );

  if ( !ind_data_ptr->prereg_is_allowed )
  {
    if ( (hdrsaps_get_state() != HDRSAPS_CLOSE) )
    {
      /* Silently close virtual connection if preReg zone changed.
       * PreReg zone info is available only when preReg is allowed. 
       * So when preReg is not allowed, we can treat as entering a 
       * different preReg zone. 
       */ 
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "PreReg is not allowed, silently close connection" );
      hdrind_give_ind( HDRIND_AMP_PREREG_ZONE_CHANGED, NULL );
    }
    return;
  }
  else
  {
    /* We want optimization only in the following case:
     * LTE1 (preReg allowed) -> LTE2 (preReg not allowed)-> LTE1 case.
     * So current LTE cell is definitely not LTE2. Reset last lte cell ID 
     * so that optimization won't be triggered in 
     * LTE1 (preReg allowed) -> LTE2 (preReg allowed)-> LTE1 case 
     */ 
    hdramp.lte_cell_id_last = 0;
    if ( hdramp.back_to_same_cell && !hdramp.same_cell_last_prereg )
    {
      HDR_MSG_PROT( MSG_LEGACY_MED, 
                    "Back to the same cell, but preReg status changes" );
      hdramp.back_to_same_cell = FALSE;
    }
  }

  if ( hdramp.in_same_cell )
  {
    /* Do nothing if rx'ed preReg update in the same cell.
     * Note that this flag is only set when preReg is allowed and processed 
     */
    HDR_MSG_PROT( MSG_LEGACY_MED, 
                  "PreReg info update is rx'ed in the same LTE cell. Ignore." );
    return;
  }

  /* Pre-registration is allowed */

  /* Get the color code */
  if ( hdrovhd_get_color_code( &ovhd_color_code,
                               HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) 
       != E_SUCCESS )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not get Color Code" );
    return;
  }

  if ( hdramps_get_state() != HDRAMP_OPEN ) 
  {
    if ( hdramp.amp_setup_pending )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "Continue with AMP SETUP" );
      hdramp.amp_setup_pending = FALSE;
      hdramp_send_uati_req_msg();
    }
    else
    {
      HDRTRACE_IND_IGNORED_STATE( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                                  HDRIND_OVHD_INFO_UPDATED, 
                                  hdramps_get_state() );
    }
  }
  else if ( hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
              != HDRSCP_IRAT_SAP )
  {
    /* If PreReg is not allowed and we do not have IRAT subtypes: Do nothing.
     * If PreReg is allowed and we do not have IRAT subtypes, SMP will 
     * close sessoin 
     */
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                    "Rxed prereg_info_updated (allowed:%d) but no IRAT subtypes",
                    ind_data_ptr->prereg_is_allowed );
  }
  else
  {
    if(!( hdrsap_tunnel_mode_is_enabled( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) ))
    {
       ERR_FATAL( "Tunnel mode not enabled", 0, 0, 0 );
    }

    if ( ovhd_color_code != hdramp.session_info.uati_cc )
    {
      prereg_zone_changed = TRUE;
    }

    /* Check whether UATIColorCode matches any of the SceondaryColorCode. */
    sec_cc_is_diff = hdramp_uati_cc_diff_from_secondary_cc( TRUE );

    /* How UE performs session maintaince (whether restore prior session or 
     * update UATI) is based on the preReg value change between neighbor cells.
     * When UE receives preReg allowed for the first time in a LTE cell, it 
     * should compare with the preReg value in the last LTE cell (or 0 if it 
     * moves from HDR) but not the previous preReg received.
     * e.g. UE moves from preReg allowed cell to a neighbor cell which SIB8 
     * does not have preReg info presented (UE receives preReg not allowed 
     * first after entering the cell) and preReg is allowed in RRCreconfig.
     * In this case, UE is expected to update UATI instead of restore prior
     * session since both cell allows preReg.
     */
    prev_prereg_allowed = hdramp.prev_cell_prereg;
  
    HDR_MSG_PROT_6( MSG_LEGACY_HIGH,
                    "AMP: prereg %d -> %d, cc diff %d, scc diff %d, acq(lte) %d, resel(lte) %d",
                    prev_prereg_allowed,
                    ind_data_ptr->prereg_is_allowed,
                    prereg_zone_changed,
                    sec_cc_is_diff,
                    hdramp.entering_tunnel,
                    hdramp.hdr_to_lte_resel );
  
    if ( ( !prev_prereg_allowed || hdramp.entering_tunnel ) &&
         !hdramp.hdr_to_lte_resel )
    {
      /* Restore prior session when
       * 1. LTE (PreRegAllowed = 0) havdover to LTE (PreRegAllowed = 1).
       * There is one exception. When AT moves from LTE1 -> LTE2 -> LTE1,
       * where LTE1 has PreRegAllowed = 1 and LTE2 has PreRegAllowed = 0,
       * it does nothing after going back to LTE1.
       * 2. LTE OOS to LTE
       * 3. eHRPD OOS to LTE 
       */
      if ( !hdramp.back_to_same_cell )
      {
        if ( hdrsmp_is_to_maintain_evdo_session() )
        {
          /* If Prior Session is not supported, replace with update UATI */
          if ( hdramp.ps_is_not_supported )
          {
            HDR_MSG_PROT( MSG_LEGACY_MED, 
                          "Should do RestorePriorSession, Update UATI instead" );
            hdramp_request_new_ati( hdramp.session_info.full_uati );
          }
          else
          {
            hdrind_give_ind( HDRIND_AMP_FAILED_NEW_NETWORK, NULL );
            HDR_MSG_PROT( MSG_LEGACY_MED, "AMP: FAILED_NEW_NETWORK" );
          }
        }
        else
        {
          hdrind_give_ind( HDRIND_AMP_FAILED_NOT_MAINTAINING_UATI, NULL );
          HDR_MSG_PROT( MSG_LEGACY_MED, "AMP: NOT_MAINTAINING_UATI" );
        }
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "Back to same cell, do nothing" );
      }
    }
    else if ( hdramp.hdr_to_lte_resel )
    {
      /* Update UATI when
       * 1. HDR to LTE reselection 
       */ 
      HDR_MSG_PROT( MSG_LEGACY_MED, "Reselected to LTE, keep session, req. UATI" );
      hdramp_request_new_ati( hdramp.session_info.full_uati );
    }
    else if ( prereg_zone_changed )
    {
      /* Update UATI when
       * 2. LTE (PreRegAllowed = 1) havdover to LTE (PreRegAllowed = 1) and 
       *    ColorCode is different
       */ 

      HDR_MSG_PROT( MSG_LEGACY_MED, "LTE ho to LTE, CC Diff" );
      hdrind_give_ind( HDRIND_AMP_PREREG_ZONE_CHANGED, NULL );

      /* Need to update UATI */
      if ( sec_cc_is_diff )
      {
        HDR_MSG_PROT( MSG_LEGACY_MED, "SCC Diff, keep session, req. UATI" );
        hdramp_request_new_ati( hdramp.session_info.full_uati );
      }
    }
    else
    {
      /* Clear UATIAssignment Timer if moved back to original
       * subnet or decided to restore prior session, in which case
       * AT will send UATI request after session is restored.
       * We are back to the subnet that assigned our current UATI but we were
       * previously on a different subnet and sent a UATI request there.
       * There is no reason to worry about that old UATI request any more.
       * Also note that we do not perform AHO while probing a UATIRequest.
       */
      (void) rex_clr_timer( &hdramp.uati_assignment_timer );
      hdramp.uati_asgn_received = TRUE;

#ifdef FEATURE_LTE_TO_HDR_OH
      hdramp.waiting_uati_asgn = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
      if ( hdrslp_is_waiting_for_uati_assignment() )
      {
        if( rex_get_timer( &hdramp.uati_assignment_timer ) != 0 )
        {
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "SLPQH: IdleHO back to original subnet %d",
                          ovhd_color_code);
        }
        else
        {
          HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                          "SLPQH: AHO back to original subnet %d",
                          ovhd_color_code);
        }
        hdramp_done_waiting_for_uati_assignment( 
          HDRAMP_CLEAR_SLP_HOLDOFF_BACK_IN_ORIG_SUBNET );
      }
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
    }
  }

  /* UE are in the same LTE cell until a cell changed ind rx'ed */
  hdramp.in_same_cell = TRUE;

  hdramp.entering_tunnel = FALSE;
  hdramp.hdr_to_lte_resel = FALSE;

} /* hdramp_process_prereg_info_updated() */
#endif /* FEATURE_LTE_TO_HDR_OH */

  
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_UATI_ASSIGN_MSG

DESCRIPTION
  This function processes the UATI Assignment Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item with the message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdramp_process_uati_assign_msg
(
  dsm_item_type * item_ptr
)
{
  /* hold data from received UATI Assignment Message after unpack */
  hdrampi_uati_assignment_msg_type uati_assign_msg;
  /* structure to hold data for sent UATI Complete Message before packing */
  hdrampi_uati_complete_msg_type uati_complete_msg;
  dsm_item_type * send_msg_ptr; /* pointer to dsm buffer of sent message */
  uint8 ret_val = 0; /* counter to store the return value of unpack macros */
  uint8 count;       /* byte of OldUATI are counted individually */
  uint8 index   = 0; /* bit offset into the message, used for unpacking after 
                        the SUBNET_INCLUDED field */
  boolean msg_is_fresh = FALSE; /* assume the message is not fresh until
                                   confirmed otherwise */
  uint8 ovhd_color_code  = 0;   /* color code from OVHD */
  uint8 ovhd_subnet_mask = 0;   /* subnet mask from OVHD */
  hdraddr_type ovhd_sector_id;  /* sector ID from OVHD */
  hdrhai_ati_record_type ati_record; /* to set transmit ati & uati */
  hdrsrch_link_id_type link_id; /* Link ID UATIAssignment received on */
  sys_link_type uati_assign_link_info; /* Link info for UATIAssignment */
  hdrmc_cb_params_rsp_union_type cb_param;  /* Parameters for UATI Update callback */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdramps_get_state() == HDRAMP_INACTIVE)
  {
    HDRTRACE_RXMSG_IGNORED_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                     HDRAMP_UATI_ASGN_MSG, hdramps_get_state() ); //lint !e506
    return;
  }

  memset(uati_assign_msg.uati_104, 0, HDRADDR_LENGTH_IN_BYTES);

/*- - - - - - - - - - - Unpack UATI Assignment Message - - - - - - - - - - */

  ret_val |= (uint8)HDRAMP_UNPACK_TRANS_ID(item_ptr, 
                    &(uati_assign_msg.message_seq));

  ret_val |= (uint8)HDRAMP_UNPACK_UATI_SUBNET_INC(item_ptr,
                    &(uati_assign_msg.subnet_included));

  if (ret_val != (uint8)E_SUCCESS)
  {
    ERR( "Error unpacking UATI Assignment message, ignoring", 0, 0, 0);
    return;
  }

  /* since subnet_included is defined as a boolean but technically
     unsigned character, assert that the unpacked data was 0 or 1 */
  ASSERT(uati_assign_msg.subnet_included == TRUE ||
         uati_assign_msg.subnet_included == FALSE);

  index = HDRAMP_UATI_ASSIGN_INITIAL_LENGTH;

  if (uati_assign_msg.subnet_included) /* if subnet is included, unpack
                                               subnet mask and UATI104 from msg
                                               and set bit index */
  {
    ret_val |= (uint8)HDRAMP_UNPACK_UATI_SUBNET_MASK(item_ptr,
                      &(uati_assign_msg.uati_subnet_mask));
    for (count = 0; count < HDRAMP_OCTETS_IN_UATI104; count++)
    {
      ret_val |= (uint8)HDRAMP_UNPACK_UATI_104(item_ptr,
                        &(uati_assign_msg.uati_104), count);
    }
    index += HDRAMP_UATI_ASSIGN_SUBNET_INFO_LENGTH;
  }

  ret_val |= (uint8)HDRAMP_UNPACK_UATI_COLOR_CODE(item_ptr,
                    &(uati_assign_msg.uati_color_code), index);
  index += HDRAMP_UATI_ASSIGN_COLOR_CODE_LENGTH;

  ret_val |= (uint8)HDRAMP_UNPACK_UATI_024(item_ptr,
                    &(uati_assign_msg.uati_024), index);
  index += HDRAMP_UATI_24_LENGTH;

  ret_val |= (uint8)HDRAMP_UNPACK_UATI_OLD_LENGTH(item_ptr,
                    &(uati_assign_msg.upper_old_uati_length), index);

  if (ret_val != (uint8)E_SUCCESS)
  {
    ERR( "Error unpacking UATI Assignment message, ignoring", 0, 0, 0);
    return;
  }

  /* UpperOldUATILength max is 13.  If AN sent a value larger, use 13. */
  if (uati_assign_msg.upper_old_uati_length > HDRAMP_MAX_UPPER_OLD_UATI_OCTETS)
  {
    uati_assign_msg.upper_old_uati_length = HDRAMP_MAX_UPPER_OLD_UATI_OCTETS;
  }
/*- - - - - - - - - End Unpack UATI Assignment Message - - - - - - - - - - */

/*- - - - - - - - - - - - Verify Freshness - - - - - - - - - - - - - - - - */

#ifdef FEATURE_LTE_TO_HDR_OH
  if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) &&
       hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
         == HDRSCP_IRAT_SAP )
  {
    msg_is_fresh = TRUE;

    if ( !uati_assign_msg.subnet_included )
    {
      msg_is_fresh = FALSE;
    }
    else
    {
      if ( hdrovhd_get_color_code( &ovhd_color_code,
             HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) != E_SUCCESS )
      {
        msg_is_fresh = FALSE;
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not get color code from OVHD" );
      }
      else
      {
        if ( ovhd_color_code != uati_assign_msg.uati_color_code )
        {
          msg_is_fresh = FALSE;
          HDR_MSG_PROT( MSG_LEGACY_ERROR, "ColorCode does not match" );
        }
      }
    }
  }
  else
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    if ( uati_assign_msg.subnet_included )
    {
      msg_is_fresh = TRUE;
    }
    else
    {
      /* If the subnet is not included, the UATIsubnetMask and UATI104
       * must be determined from the current overhead information.
       * If an AHO is pending, the overhead info may be marked as
       * "not current" even though the current subnet and subnet mask
       * are still valid.  To determine this, HDROVHD needs the link
       * information (pilot PN and channel) the UATIAssignment was
       * received on.
       */
      link_id = (hdrsrch_link_id_type) HDRDSM_GET_LINK_ID( item_ptr );
      hdrlmac_get_link_info( link_id , &uati_assign_link_info);

      if ( hdrovhd_get_sector_id2(
             ovhd_sector_id, 
             &ovhd_subnet_mask,
             &uati_assign_link_info,
             HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) == E_SUCCESS )
      {
         if ( hdrovhd_get_color_code( &ovhd_color_code,
                HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) != E_SUCCESS )
         {
           HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not get color code from OVHD" );
         }

         if ( ovhd_color_code == uati_assign_msg.uati_color_code )
         {
           msg_is_fresh = TRUE;
         }
         else
         {
           HDR_MSG_PROT( MSG_LEGACY_ERROR, "ColorCode does not match" );
         }
      }
      else
      {
        HDR_MSG_PROT( MSG_LEGACY_ERROR, "Could not determine subnet" );
      }
    }
  }

  /* if we have gotten here and the message is still not fresh, ignore it */
  if (!msg_is_fresh)
  {
    HDRTRACE_HDRAMP_ASSIGN_MSG_IGNORED_FRESH(hdramps_get_state());
    return;
  }
/*- - - - - - - - - - End Verify Freshness - - - - - - - - - - - - - - - - */

  /* if the message sequence number does not follow rules, ignore message */
  if(hdrutil_msg_seq_num_is_greater(uati_assign_msg.message_seq, 
     hdramp.session_info.uati_assign_validation_seq_num, 8))
  {
    hdramp.session_info.uati_assign_validation_seq_num = 
                                                   uati_assign_msg.message_seq;
  }
  else
  {
    HDRTRACE_RXMSG_IGNORED_SEQ( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
               HDRAMP_UATI_ASGN_MSG, hdramps_get_state()); //lint !e506
    return;
  }

/* If we have reached this point in the processing, all possible error
   conditions have passed.  All subsequent code may assume that the
   UATI Assignment Message has been successfully received and decoded.
   If we are in the Setup State, we can transition to Open State. */

  /* uati_asgn_received flag needs to be set before clearing the timer so that
   * we won't start timer if UATIAssignment arrives before than L2 ACK. */
  hdramp.uati_asgn_received = TRUE;
  (void)rex_clr_timer(&hdramp.uati_assignment_timer);

#ifdef FEATURE_LTE_TO_HDR_OH
  hdramp.waiting_uati_asgn = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
  HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
            "SLPQH: UATIAssignment rcvd. Canceling timer with %d ms remaining", 
            rex_get_timer(&hdramp.uati_assignment_slp_holdoff_timer));

  /* We clear timer here but don't clear the 
   * hdrslp_waiting_for_uati_assignment flag, instead, we clear it
   * only after queuing UATIComplete to SLP */

  (void) rex_clr_timer(&hdramp.uati_assignment_slp_holdoff_timer);

  HDRTRACE_SLP_SLPQH_TIMER_STOPPED( 
       HDRAMP_CLEAR_SLP_HOLDOFF_RCVD_UATI_ASSIGNMENT );

#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

#ifdef FEATURE_HDR_REVC
  /* A revC capable phone should send LoadInformationSupported attribute update
     if all of the following conditions are true:
     1. it has received Load Information message from current serving sector
        in either IDLE or CONNECTED state
     2. the AN hasn't rejected the LoadInformationSupported attribute proposed
        (either by Config or GAUP) by the AT after entering current subnet 
     3.	it is in connected state or is going to bring up traffic channel due
        to data activities.
     So every time when a phone is entering a new subset, we should reset the
     variables which record the above condition information */
  hdrscmfmac_reset_load_info_gaup_conditions();

  /* Conditions to trigger MultiATPageMessageSupported attribute update request 
     is similar to those of LoadInformationSupported. So every time when AT is 
     entering a new subset, we reset all the conditions which control
     MultiATPageMessageSupported attribute update request*/
  hdrscmidle_reset_multi_at_page_gaup_conditions();
#endif /* FEATURE_HDR_REVC */

  /* Make RATI invalid*/
  hdrampati_receive_ati_delete(HDRHAI_RATI);

  /* copy UATI Color Code */
  hdramp.session_info.uati_cc = uati_assign_msg.uati_color_code;

  /*-- calculate UATI 104 --*/
  /* if subnet is included, get UATI104 from message */
  if (uati_assign_msg.subnet_included) 
  {
    hdraddr_set(hdramp.session_info.full_uati, uati_assign_msg.uati_104);
    hdramp.session_info.uati_subnet_mask = uati_assign_msg.uati_subnet_mask;
  }
  /* subnet is not included, get UATI104 from OVHD */
  else 
  {
    hdraddr_set(hdramp.session_info.full_uati, ovhd_sector_id);
    hdramp.session_info.uati_subnet_mask = ovhd_subnet_mask;
  }

  hdramp_set_uati024(hdramp.session_info.full_uati, uati_assign_msg.uati_024);

  /* Callback functions to inform registered clients. */
  cb_param.uati_update.cb_type = HDRMC_CB_EVENT_UATI_UPDATE;
  hdraddr_set(cb_param.uati_update.full_uati, hdramp.session_info.full_uati);
  hdrmc_execute_cb(&cb_param);

  ati_record.ati_type  = HDRHAI_UATI;
  ati_record.ati_value = ((hdramp.session_info.uati_cc << 24) |
                              (uati_assign_msg.uati_024));
  
  /* set the transmit ATI */
  hdramp_set_transmit_ati(ati_record); 

  /* add a new UATI to the receive ati list */
  hdrampati_receive_ati_insert(ati_record);

  HDR_MSG_PROT_2 (MSG_LEGACY_HIGH, "New UATI asgn: 0x%x (CCode: %d)",
    hdramp.session_info.transmit_ati.ati_value,
    hdramp.session_info.uati_cc);

  /* Track how many UATI assignment messages were received for logging
   * purposes.
   */
  hdrplog_inc_conn_state_stats_counter(HDRPLOG_UATI_ASSIGNMENT_COUNT, 1);

#ifdef FEATURE_LTE_TO_HDR_OH
  hdramp.backoff_sending_uati = FALSE;
  hdrsap_set_prereg_status( HDRHAI_PREREG_STATUS_EVENT_UATI_ASSIGNED );
#endif /* FEATURE_LTE_TO_HDR_OH */

  /* fill in the fields of the UATI Complete Message */
  uati_complete_msg.message_id = HDRAMP_UATI_COMP_MSG;
  uati_complete_msg.message_seq = uati_assign_msg.message_seq;
  uati_complete_msg.upper_old_uati_length =
                    uati_assign_msg.upper_old_uati_length;

  /* set UpperOldUATI of the UATI Complete Message.  This is the
     <UpperOldUATILength> Least Significant Bits of the 13 MSB of the UATI */
  memset(uati_complete_msg.upper_old_uati, 0, 
         HDRAMP_MAX_UPPER_OLD_UATI_OCTETS);

  hdraddr_copy_specific_octets(uati_complete_msg.upper_old_uati,
                               hdramp.old_uati,
                               (uint16)(HDRAMP_MAX_UPPER_OLD_UATI_OCTETS -
                                 uati_assign_msg.upper_old_uati_length),
                               uati_assign_msg.upper_old_uati_length);
 
  /* get new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* pack UATI Complete Message into DSM item */
  HDRAMP_PACK_UATI_COMP_MSG_ID(send_msg_ptr, uati_complete_msg);
  HDRAMP_PACK_UATI_COMP_MSG_SEQ(send_msg_ptr, uati_complete_msg);
  HDRAMP_PACK_UATI_COMP_RESERVED(send_msg_ptr);
  HDRAMP_PACK_UATI_COMP_OLD_LEN(send_msg_ptr, uati_complete_msg);
  for (count = 0; count < uati_complete_msg.upper_old_uati_length; count++)
  {
    HDRAMP_PACK_UATI_COMP_OLD(send_msg_ptr, uati_complete_msg, count);
  }

  /* send UATI Complete Message */
  hdrhmp_send_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                   HDRHAI_AC_OR_RTC,
                   TRUE,
                   HDRAMP_UATI_MESSAGE_PRIORITY - 1,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL);

  hdramp_write_session_info_to_db();

  /* log the UATI assignment message */
  hdrplog_uati_rcvd(item_ptr,
                    hdramp.session_info.full_uati,
                    uati_assign_msg.uati_color_code);


  /* change state and return indication if the message was received in
     Setup state */
  if (hdramps_get_state() == HDRAMP_SETUP)
  {
    hdramps_change_state(HDRAMP_OPEN);
  }
  hdrind_give_ind(HDRIND_AMP_UATI_ASSIGNED, NULL);
} /* hdramp_process_uati_assign_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_HARDWARE_REQ_MSG

DESCRIPTION
  This function processes the Hardware ID Request Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item containing the received message

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdramp_process_hardware_req_msg
(
  dsm_item_type * item_ptr
)
{
  uint8 transaction_id; /* transaction ID of the Hardware ID Request Msg */
  /* structure to hold sent Hardware ID Response Message before pack */
  dsm_item_type * send_msg_ptr; /* pointer to dsm buffer of sent message */

#if defined(FEATURE_UIM_RUIM)
  uint32 esn_me = 0; /* Used to send the shell's ESN */
#endif /* FEATURE_UIM_RUIM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (HDRAMP_UNPACK_TRANS_ID(item_ptr,&(transaction_id)) != E_SUCCESS)
  {
    ERR( "Error unpacking Hardware Request message, ignoring", 0, 0, 0);

    return;
  }

  /* get new DSM buffer */
  send_msg_ptr = hdrutil_new_msg_buffer();

  /* Pack Hardware ID Response Message info into DSM buffer */
  HDRAMP_PACK_HARDWARE_RSP_MSG_ID(send_msg_ptr, HDRAMP_HARDWARE_ID_RSP_MSG);
  HDRAMP_PACK_HARDWARE_RSP_TRANS_ID(send_msg_ptr, transaction_id);
  HDRAMP_PACK_HARDWARE_RSP_TYPE(send_msg_ptr, 
                                hdramp.session_info.session_hwid.type);

  if (hdramp.session_info.session_hwid.type == HDRAMP_HARDWARE_ID_TYPE_ESN)
  {
    HDRAMP_PACK_HARDWARE_RSP_LENGTH(send_msg_ptr, 4);
#if defined(FEATURE_UIM_RUIM)
    if (hdrmc_feature_is_enabled(HDRMC_FEATURE_JCDMA))
    {
      /* Get the shell ESN from UIM API, otherwise the ESN value read from NV may 
         represent the RUIM_ID instead. */

      esn_me = uim_get_esn_me();
  
      /* Send the shell's ESN as hardware ID */
      if (esn_me != 0)
      {
        HDRAMP_PACK_HARDWARE_RSP_VALUE_ESN(send_msg_ptr, esn_me);
      }
      else
      {
        /* If the shell ESN is not available through UIM API, we default back to sending
           whatever value we read from the ESN NV item. */

        HDRAMP_PACK_HARDWARE_RSP_VALUE_ESN(send_msg_ptr, hdramp.session_info.session_hwid.esn);
      }
    }
    else
#endif /* FEATURE_UIM_RUIM */
    {
      HDRAMP_PACK_HARDWARE_RSP_VALUE_ESN(send_msg_ptr, hdramp.session_info.session_hwid.esn);
    }
  }
  else if (hdramp.session_info.session_hwid.type == HDRAMP_HARDWARE_ID_TYPE_MEID)
  {
    HDRAMP_PACK_HARDWARE_RSP_LENGTH(send_msg_ptr, 7);
    HDRAMP_PACK_HARDWARE_RSP_VALUE_MEID(send_msg_ptr, 
      qw_hi(hdramp.session_info.session_hwid.meid), qw_lo(hdramp.session_info.session_hwid.meid));
  }
  else
  {
    HDRAMP_PACK_HARDWARE_RSP_LENGTH(send_msg_ptr, 0);
  }



  /* send Hardware ID Response Message */
  hdrhmp_send_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                   HDRHAI_AC_OR_RTC,
                   TRUE,
                   HDRAMP_HWID_MESSAGE_PRIORITY,
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr,
                   NULL,
                   NULL);

} /* hdramp_process_hardware_req_msg */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRAMP_PROCESS_IRMA_MSG

DESCRIPTION
  This function processes the InterRATMobilityAck Message

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item containing the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void hdramp_process_irma_msg
(
  dsm_item_type * item_ptr
)
{
  /* Clear timer */
  (void) rex_clr_timer( &hdramp.irma_timer );
  hdramp.irmi_cnt = 0;
  hdramp.awaiting_irmi_tx_cb = FALSE;

  /* AMP receives IRMA in time. Return success indication to protocols. */
  hdrind_give_ind( HDRIND_AMP_IRMI_ACCEPTED, NULL );

} /* hdramp_process_irma_msg() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_UATI_ASSIGN_TIMER_EXP

DESCRIPTION
  This function processes the UATI Assignment Timer expiration

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdramp_process_uati_assign_timer_exp(void)
{
  /* clear timer and sig before returning indication */
  (void)rex_clr_timer(&hdramp.uati_assignment_timer);

#ifdef FEATURE_HDR_TO_LTE
  if(hdralmp_hdr_to_lte_resel_is_in_progress() == TRUE)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "D2L reselection is in progress. Ignore UATI timer expiry !!");
    return;
  }
#endif

#ifdef FEATURE_HDR_TO_LTE_REDIRECTION
  if(hdralmp_hdr_to_lte_redir_is_in_progress() == TRUE)
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, "D2L redirection is in progress. Ignore UATI timer expiry !!");
    return;
  }
#endif

  if ((hdramps_get_state() == HDRAMP_SETUP) &&
      (++hdramp.initial_uati_req_attempts <
         HDRAMP_MAX_INITIAL_UATI_REQ_ATTEMPTS ))
  {
    /* We failed to get our initial UATI assignment.  Rather than fail
       the session at this point, just send another UATI request. */
    hdrplog_end_session_attempt(FALSE);
    hdramp_send_uati_req_msg();
  }
  else
  {
    hdrind_give_ind(HDRIND_AMP_FAILED_REQUEST_TIMEOUT,NULL);
  }  
} /* hdramp_process_uati_assign_timer_exp */

/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRAMP_PROCESS_UPDATE_UATI

DESCRIPTION
  This function processes the Update UATI Command.  

  This command was included in the standard in AMP even though it is not 
  used by any other protocol at the time the Default Address Management 
  Protocol was written.  The reason is that at some point in the future, 
  another protocol may have logic in which it decides that AMP should update 
  the UATI.  Having the Command already defined prevents an unnecessary 
  additional subtype of the AMP.  

  If this function is needed it needs to be implemented.  It should probably
  call hdramp_request_new_ati(), but this function requires the overhead
  information to be current, so if overhead info isn't current it may need
  to set a flag that is checked from hdramp_process_ovhd_info_updated().

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL void hdramp_process_update_uati
( 
  hdramp_cmd_type * amp_cmd_ptr 
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (hdramps_get_state() != HDRAMP_OPEN)
  {
    HDRTRACE_CMD_IGNORED_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
      amp_cmd_ptr->name, amp_cmd_ptr->sender, hdramps_get_state());

    return;
  }

  ASSERT( FALSE ); /* This command is not supported at this time */
} /* hdramp_process_update_uati() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_HWIDS_ARE_EQUAL

DESCRIPTION
  This function compares two HWIDs. 

DEPENDENCIES
  None.

PARAMETERS
  hwid1 and hwid2, the two HWIDs to compare

RETURN VALUE
  TRUE if the HWIDs are equal, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean hdramp_hwids_are_equal
(
  hdramp_hwid_type hwid1,
  hdramp_hwid_type hwid2
)
{
  if (hwid1.type != hwid2.type)
  {
    return FALSE;
  }
  else if ((hwid1.type == HDRAMP_HARDWARE_ID_TYPE_ESN) &&
           (hwid1.esn != hwid2.esn))
  {
    return FALSE;
  }
  else if ((hwid1.type == HDRAMP_HARDWARE_ID_TYPE_MEID) &&
           (qw_cmp(hwid1.meid, hwid2.meid) != 0))
  {
    return FALSE;
  }

  return TRUE;

} /* hdramp_hwids_are_equal() */

#if defined(FEATURE_UIM_RUIM) && defined(FEATURE_MMGSDI)
/*===========================================================================

FUNCTION HDRAMP_MMGSDI_READ_TRANS_CB

DESCRIPTION
  This function is a callback passed to mmgsdi_read_transparent()
  It will be called to receive the ICC_ID

DEPENDENCIES
  None

PARAMETERS
  status - status code of mmgsdi operation
  cnf - type of the mmgsdi operation
  cnf_ptr - mmgsdi data_ptr
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdramp_mmgsdi_read_trans_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  uint8 current_icc_id[HDRAMP_ICC_ID_LEN];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((cnf == MMGSDI_READ_CNF) &&
      (status == MMGSDI_SUCCESS))
  {
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy(current_icc_id, sizeof(current_icc_id), 
            cnf_ptr->read_cnf.read_data.data_ptr,
            sizeof(current_icc_id));
#else
    memcpy(current_icc_id, cnf_ptr->read_cnf.read_data.data_ptr,
           sizeof(current_icc_id));
#endif /* FEATURE_MEMCPY_REMOVAL */

    HDR_MSG_PROT_4(MSG_LEGACY_MED, "data len: %d, ICC_ID[0x%04x%08x%08x]",
                   cnf_ptr->read_cnf.read_data.data_len, \
                   (current_icc_id[9] << 8) | current_icc_id[8], \
                   (current_icc_id[7] << 24) | (current_icc_id[6] << 16) | \
                   (current_icc_id[5] << 8) | current_icc_id[4], \
                   (current_icc_id[3] << 24) | (current_icc_id[2] << 16) | \
                   (current_icc_id[1] << 8) | current_icc_id[0]);

    if (hdrsmp_session_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL))
    {
      if (memcmp(current_icc_id,
                 hdramp.session_info.icc_id,
                 sizeof(current_icc_id)) != 0)
      {
        HDR_MSG_PROT_3(MSG_LEGACY_HIGH, 
          "ICC_ID mismatch, session ICC_ID[0x%04x%08x%08x]",
          (hdramp.session_info.icc_id[9] << 8) | \
          hdramp.session_info.icc_id[8], \
          (hdramp.session_info.icc_id[7] << 24) | \
          (hdramp.session_info.icc_id[6] << 16) | \
          (hdramp.session_info.icc_id[5] << 8) | \
          hdramp.session_info.icc_id[4], \
          (hdramp.session_info.icc_id[3] << 24) | \
          (hdramp.session_info.icc_id[2] << 16) | \
          (hdramp.session_info.icc_id[1] << 8) | \
          hdramp.session_info.icc_id[0]);


        hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
      }
    }
#ifdef FEATURE_MEMCPY_REMOVAL 
    memscpy(hdramp.session_info.icc_id, 
            sizeof(hdramp.session_info.icc_id),
            current_icc_id, sizeof(current_icc_id));
#else
    memcpy(hdramp.session_info.icc_id, current_icc_id,
           sizeof(hdramp.session_info.icc_id));
#endif /* FEATURE_MEMCPY_REMOVAL */
    /* immediately save to scmdb in case of power off */
    hdramp_write_session_info_to_db();
  }
  else if (hdrsmp_session_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL))
  {    
    /* RUIM reading failure should be rare. In this case, close 
       session and retry */
    HDR_MSG_PROT(MSG_LEGACY_FATAL, "Cannot read ICC_ID, close session");
    hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
  }
} /* hdramp_mmgsdi_read_trans_cb */

#endif /* FEATURE_MMGSDI && FEATURE_UIM_RUIM */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_UPDATE_ESN

DESCRIPTION
  This function processes the Update ESN Command. 

DEPENDENCIES
  None.

PARAMETERS
  amp_cmd_ptr - Command Pointer

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdramp_process_update_esn
(
  hdramp_cmd_type * amp_cmd_ptr 
)
{
  hdramp_hwid_type phone_hwid;
  boolean session_open;
#if defined(FEATURE_MMGSDI) && defined(FEATURE_UIM_RUIM)
  static boolean first_update_after_powerup = TRUE;
  mmgsdi_return_enum_type mmgsdi_status;
  boolean mmgsdi_ready;
#ifdef FEATURE_HDR_NEW_GSDI_API 
  mmgsdi_session_id_type mmgsdi_sid;  
  mmgsdi_access_type file_access;
#else
  mmgsdi_client_id_type mmgsdi_cid;
#endif /* FEATURE_HDR_NEW_GSDI_API */
#endif /* FEATURE_MMGSDI && FEATURE_UIM_RUIM */
int get_esn_me_change_flag;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SYS_ARG_NOT_USED(amp_cmd_ptr); /* remove this line when amp_cmd_ptr 
                                  * is used */

  session_open = hdrsmp_session_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL);

  memset ( &phone_hwid, 0, sizeof (hdramp_hwid_type) );

#if defined(FEATURE_MMGSDI) && defined(FEATURE_UIM_RUIM)
  /* check if AT reads user-subscription data from RUIM card.
     If so, read & compare ICC-ID to see if card has changed. */
  if (nv_rtre_control() == NV_RTRE_CONTROL_USE_RUIM)
  {
#ifdef FEATURE_HDR_NEW_GSDI_API
     /* Get session ID registered during power up with MMGSDI */
    mmgsdi_ready = hdrmc_get_reg_mmgsdi_sid(&mmgsdi_sid);
#else
     /* Get client ID registered during power up with MMGSDI */   
     mmgsdi_cid = hdrmc_get_reg_mmgsdi_cid();
     mmgsdi_ready = (mmgsdi_cid != 0);
#endif /* FEATURE_HDR_NEW_GSDI_API */

    /* If session ID is not available and session is open, then close 
       the session */
    if (!mmgsdi_ready && session_open)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR, "MMGSDI not ready, reopen session");
      hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
    }
    else
    {    
      HDR_MSG_PROT(MSG_LEGACY_MED, "UPDATE_ESN, mmgsdi ready. Read ICC_ID");
      /* Should always be slot1.  UI will notify if API changes. */

#ifdef FEATURE_HDR_NEW_GSDI_API
      file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
      file_access.file.file_enum = MMGSDI_ICCID;

      mmgsdi_status = mmgsdi_session_read_transparent(mmgsdi_sid, 
                                                      file_access,
                                                      0,
                                                      HDRAMP_ICC_ID_LEN,
                                                      hdramp_mmgsdi_read_trans_cb,
                                                      0);
#else
      mmgsdi_status = mmgsdi_read_transparent(mmgsdi_cid, MMGSDI_SLOT_1, 
                              MMGSDI_ICCID, 0, HDRAMP_ICC_ID_LEN, 
                              hdramp_mmgsdi_read_trans_cb, 0);
#endif /* FEATURE_HDR_NEW_GSDI_API */

      if ((mmgsdi_status != MMGSDI_SUCCESS) && session_open)
      {
        HDR_MSG_PROT_1(MSG_LEGACY_ERROR, "MMGSDI ERROR:%d", mmgsdi_status);
        hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
      }
    }
  }
#endif /* FEATURE_MMGSDI && FEATURE_UIM_RUIM */

  phone_hwid.type = HDRAMP_HARDWARE_ID_TYPE_ESN;

  /* We are only reading the Current MEID or ESN from NV if we received 
   * a Phone Status change command from MMOC. The new NV ITEM
   * NV_ESN_ME_I/NV_MEID_ME_I always turns the ESN/MEID of the shell */

  if ( hdrutil_read_nv_item( NV_MEID_ME_I, (nv_item_type*)phone_hwid.meid ) 
        == NV_DONE_S )
  {
    phone_hwid.type = HDRAMP_HARDWARE_ID_TYPE_MEID;

    /* The MEID isn't stored in the RUIM, so the session MEID is always
     * the same as the phone MEID.
     */
    qw_equ( hdramp.session_info.session_hwid.meid, phone_hwid.meid );

    /* We set the ESN to zero and later write it to NV, and this serves
     * as the flag that the HWID is MEID next time we power up.
     */
    hdramp.session_info.session_hwid.esn = 0;
    HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "Phone HWID is MEID 0x%x:%x",
                   qw_hi(phone_hwid.meid), qw_lo(phone_hwid.meid));
  }
  else
  {
    HDR_MSG_PROT ( MSG_LEGACY_ERROR, "UIM should return cached value for NV_MEID_ME_I. Continue with old values ");	
  }

  /* if reading of MEID was unsuccessful, the AT is provisioned
   * with an ESN */
  if (phone_hwid.type == HDRAMP_HARDWARE_ID_TYPE_ESN)
  {
    if ( hdrutil_read_nv_item( NV_ESN_ME_I, (nv_item_type*)&phone_hwid.esn )
          != NV_DONE_S )
    {
      ERR("AMP: ESN data not active in NV", 0, 0, 0);
      phone_hwid.esn = 0x00FFFFFF;
    }
    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Phone HWID is ESN 0x%x", 
                    phone_hwid.esn );
  }

  /* At this point the phone_hwid has been read from NV and is initialized.
   * Now we need to compare it with the session HWID and close the session
   * if they don't match. */
  if ( session_open )
  {
    if ( !hdramp_hwids_are_equal(hdramp.session_info.session_hwid, phone_hwid) )
    {
      /* Print a meaningful F3 message */
      if (hdramp.session_info.session_hwid.type != phone_hwid.type)
      {
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, "HWID type changed from 0x%x to 0x%x",
                         hdramp.session_info.session_hwid.type, phone_hwid.type);

      }
      else if (hdramp.session_info.session_hwid.type == HDRAMP_HARDWARE_ID_TYPE_ESN)
      {
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
                        "Close Session: ESN changed from 0x%x to 0x%x",
                        hdramp.session_info.session_hwid.esn, phone_hwid.esn);

      }

      /* The new session HardwareID will be written to NV after the Session 
       * is re-opened */
      hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
    }
#if defined(FEATURE_MMGSDI) && defined(FEATURE_UIM_RUIM)
    /* detect card swap sceanrio: 
       AT 1 w/ Card A -> power off -> AT 2 w/ Card A -> power off ->
       AT 1 w/ Card A */
    else if ( first_update_after_powerup &&
              (nv_rtre_control() == NV_RTRE_CONTROL_USE_RUIM) )
    {

      first_update_after_powerup = FALSE;
      /* Should always be slot1.  UI will notify if API changes. */
      get_esn_me_change_flag = mmgsdi_get_esn_me_change_flag(MMGSDI_SLOT_1);
      
      if(((get_esn_me_change_flag == MMGSDI_ESN_ME_CHANGE) ||
          (get_esn_me_change_flag == MMGSDI_ESN_ME_UPDATE_PENDING))&& session_open)

      {
        HDR_MSG_PROT(MSG_LEGACY_HIGH,
                     "Close Session - RUIM card was used in different phone");
        hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
      }
    }
#endif /* FEATURE_MMGSDI && FEATURE_UIM_RUIM */
  }

  /* No matter what, the new session HWID is the phone_hwid. */
  hdramp.session_info.session_hwid = phone_hwid;

  /* Save hardware_id to scmdb. */
  hdramp_write_session_info_to_db( );
} /* hdramp_process_update_esn() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_UPDATE_NAI

DESCRIPTION
  This function processes the Update NAI Command. 

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void hdramp_process_update_nai
(
  hdramp_cmd_type * amp_cmd_ptr 
)
{
  nv_item_type user_info; /* NAI(userID) for AN-AAA auth */
  boolean fail_session = FALSE; /* Whether to close the session */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* This is only read when we get a phone status change from CM */
  if ( hdrutil_read_nv_item( NV_HDR_AN_AUTH_USER_ID_LONG_I, (nv_item_type*)&user_info ) 
        != NV_DONE_S )
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, " NAI - user ID not initialized!" );
    user_info.hdr_an_auth_user_id_long.user_id_len = 0;
    memset( (uint8*)user_info.hdr_an_auth_user_id_long.user_id, 0, 
            sizeof(user_info.hdr_an_auth_user_id_long.user_id) );
  }

  /* if user_id_len is longer than the MAX possible size,
   * this is possibly due to hdr_an_auth_user_id_long 
   * is not used? Marie-Pierre, can you see if there's
   * a better way of handling this case, which does happen
   * in RUIM build */
  if (user_info.hdr_an_auth_user_id_long.user_id_len >
      sizeof(hdramp.session_info.session_nai.user_id_info))
  {
    HDR_MSG_PROT( MSG_LEGACY_ERROR, " NAI - user ID longer than max. size" );
    user_info.hdr_an_auth_user_id_long.user_id_len = 0;
    memset( (uint8*)user_info.hdr_an_auth_user_id_long.user_id, 0, 
            sizeof(user_info.hdr_an_auth_user_id_long.user_id) );
  }

  HDR_MSG_PROT(MSG_LEGACY_MED,"Read NAI userID info");

  /* At this point the phone's NAI has been read from NV and is initialized.
   * Now we need to compare it with the session NAI and close the session
   * if they don't match. */

  if ( hdrsmp_session_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) )
  {
    if (user_info.hdr_an_auth_user_id_long.user_id_len != hdramp.session_info.session_nai.user_id_len)
    {
      HDR_MSG_PROT_2(MSG_LEGACY_ERROR,"NAI nv uidlen %d != session uidlen %d",
                     user_info.hdr_an_auth_user_id_long.user_id_len,
                     hdramp.session_info.session_nai.user_id_len);
      fail_session = TRUE;
    }
    else if (memcmp((uint8*)user_info.hdr_an_auth_user_id_long.user_id,
                    hdramp.session_info.session_nai.user_id_info,
                    user_info.hdr_an_auth_user_id_long.user_id_len) != 0)
    {
      HDR_MSG_PROT(MSG_LEGACY_ERROR,"NAI uid does not match session uid");
      fail_session = TRUE;
    }

    if (fail_session)
    {
      hdramp.session_close_nai_pending = TRUE;
    }
  }

  if(fail_session)
  {
      /* Wait session is closed before updating hdramp.session_info.session_nai */
      hdramp.session_nai_pending.user_id_len = user_info.hdr_an_auth_user_id_long.user_id_len;
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy(hdramp.session_nai_pending.user_id_info, 
              sizeof(hdramp.session_nai_pending.user_id_info),
             (uint8*)user_info.hdr_an_auth_user_id_long.user_id, 
              user_info.hdr_an_auth_user_id_long.user_id_len);
#else
      memcpy(hdramp.session_nai_pending.user_id_info,
              (uint8*)user_info.hdr_an_auth_user_id_long.user_id, 
               user_info.hdr_an_auth_user_id_long.user_id_len);
#endif /* FEATURE_MEMCPY_REMOVAL */
  }
  else
  {
      hdramp.session_info.session_nai.user_id_len = user_info.hdr_an_auth_user_id_long.user_id_len;
#ifdef FEATURE_MEMCPY_REMOVAL
      memscpy( hdramp.session_info.session_nai.user_id_info, 
               sizeof(hdramp.session_info.session_nai.user_id_info),
              (uint8*)user_info.hdr_an_auth_user_id_long.user_id, 
               user_info.hdr_an_auth_user_id_long.user_id_len);
#else
      memcpy( hdramp.session_info.session_nai.user_id_info,
              (uint8*)user_info.hdr_an_auth_user_id_long.user_id, 
               user_info.hdr_an_auth_user_id_long.user_id_len);
#endif /* FEATURE_MEMCPY_REMOVAL */
  }
  
} /* hdramp_process_update_nai() */

/* EJECT */
/*===========================================================================

FUNCTION HDRAMP_ENTER_SETUP

DESCRIPTION
  This function sets data upon entering setup state

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_enter_setup ( void )
{

  hdrhai_ati_record_type ati_buf; /* ati buffer used to update random 
                                     and transmit ati */  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* get ATI information using RATI with session seed as value */
  ati_buf.ati_type  = HDRHAI_RATI;
  ati_buf.ati_value = hdramp.session_info.session_seed;

  /* update the transmit ATI */
  hdramp_set_transmit_ati(ati_buf);

  /* insert RATI into the receive ati list */
  hdrampati_receive_ati_insert(ati_buf);

  /* Reset count of how many UATI requests we have sent */
  hdramp.initial_uati_req_attempts = 0;

} /* hdramp_enter_setup */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_INIT

DESCRIPTION
  This function processes the Init command

DEPENDENCIES
  None

PARAMETERS
  session_is_open - Determines behavior whether the session is open or closed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdramp_process_init
(
  boolean session_is_open
)
{
  hdrhai_ati_record_type ati_buf;
    /* holds the most recent ati (pulled from nv) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  hdramp.uati_req_trans_id  = 0;
  hdramp.uati_asgn_received = FALSE;
  hdramp.req_uati_for_prev = FALSE;

#ifdef FEATURE_LTE_TO_HDR_OH
  hdramp.waiting_uati_asgn = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
  hdramp.dcm_reacq_diff_subnet = FALSE;
  hdramp.dcm_iho_count = 0;
#endif

  /* only read from NV if the session is open */
  if (session_is_open)
  {
    /* Get current configuration parameters from SCMDB */    
    hdrscmdb_get_amp_session_info(&hdramp.session_info); 

    memset(hdramp.old_uati, 0, HDRADDR_LENGTH_IN_BYTES);

    /* retrieve the transmit ati and add it to the receive ati list */
    /* (saved transmit ati and current ati should always match)     */
    if (hdramp.session_info.receive_current_uati != HDRHAI_ATI_NO_VALUE)
    {
      ati_buf.ati_type  = HDRHAI_UATI;
      ati_buf.ati_value = hdramp.session_info.transmit_ati.ati_value;

      hdrampati_receive_ati_insert(ati_buf);

      /* also set the transmit ati */
      hdramp_set_transmit_ati(ati_buf);
    }

    hdrplog_powerup_update_session_data( 
      hdramp.session_info.full_uati,
      hdrampati_receive_ati_get( HDRHAI_RATI ),
      hdramp.session_info.uati_cc );
      /* log session attempt packet */

    hdramps_change_state(HDRAMP_OPEN);
  }
  else
  {
    /* This is to take care of case where MCFG refresh cmd calls init for
       AMP and AMP is not in INACTIVE state. We want to skip reset_data in
       that case */
    if (hdramps_get_state() == HDRAMP_INACTIVE)
    {
    hdramp_reset_data();
  }
    else
    {
      HDR_MSG_PROT_3( MSG_LEGACY_HIGH, 
                      "AMP Proccess Init: AMP not INACTIVE! Not resetting Data"
                      " AMP state=%d ati_type=%d ati_value=%d", 
                      hdramps_get_state(),
                      hdramp.session_info.transmit_ati.ati_type,
                      hdramp.session_info.transmit_ati.ati_value );
    }
  }
  hdramp.amp_setup_pending = FALSE;
} /* hdramp_process_init */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to HDRAMP.

DEPENDENCIES
  None
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdramp_msg_cb
(
  dsm_item_type * msg_ptr 
)
{
  hdrmc_queue_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
                   HDRHAI_IN_USE, msg_ptr );
} /* hdramp_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to HDRAMP.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data_ptr - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdramp_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void * ind_data_ptr 
)
{
  if (rex_self() == HDRAMP_TASK_PTR)
  {
    hdramp_process_ind(ind_name, (hdrind_ind_data_union_type *)ind_data_ptr);
  }
  else
  {
    hdrmc_queue_ind( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, ind_name, 
                     ind_data_ptr );
  }
} /* hdramp_ind_cb */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRAMP_TIMER_CB

DESCRIPTION
  This function is the callback for all amp state protocol timers.
  This function is called by REX when the timer expires, and runs
  in the context of the task that defined the timer (HDRMC), however
  it runs as an independent thread and thus must queue an event for HDRMC.

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void hdramp_timer_cb
(
  uint32 timer_id
)
{
  hdrmc_queue_timer(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, timer_id);
} /* hdramp_timer_cb() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRAMP_STOP_UATI_ASSIGNMENT_TIMER

DESCRIPTION
  This function stops the uati assignment timer if it was running
  
DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdramp_stop_uati_assignment_timer( void )
{

  if ( rex_get_timer( &hdramp.uati_assignment_timer) != 0)
  {
    /* Stop the timer */
    HDR_MSG_PROT (MSG_LEGACY_LOW, "Stop the UATI assignment timer");
    ( void ) rex_clr_timer( &hdramp.uati_assignment_timer );
  }

} /* hdramp_stop_uati_assignment_timer() */
/* <EJECT> */ 

/*===========================================================================

FUNCTION HDRAMP_FIRST_ACQ_INIT

DESCRIPTION
  This function commands AMP to initialize upon our first HDR acquisition.
  This is only needed because ran_next() doesn't work at power up.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdramp_first_acq_init ( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!hdrsmp_session_is_open(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL)) 
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    if (hdrsap_tunnel_mode_is_enabled(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL))
    {
      /* Init seed, for test purpose, remove it later */
      ran_seed( 50 );
    }
#endif /*FEATURE_LTE_TO_HDR_OH */

    hdramp.session_info.session_seed = 
      (((uint32)ran_dist(hdrutil_get_random(),0,65535)) << 16 | 
                ran_dist(hdrutil_get_random(),0,65535));

    /* The current control channel cycle could not be set earlier because 
     * the session seed is used in its computation.  We could not compute
     * the session seed earlier because ran_next() doesn't work at power up.
     */
    hdridlec_set_current_pccc_to_default();
  }
  else
  {
    /* The PCCC is set once again here in order to avoid the race where searcher 
    task power up initilaization set it to 0xff overwriting the value set by 
    idlec powerup initilization which may execute slightly before searcher */ 
    hdridlec_set_ccc();
  }

} /* hdramp_first_acq_init() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_POWERUP_INIT

DESCRIPTION
  This function commands AMP to initialize.

DEPENDENCIES
  None

PARAMETERS
  session_is_open - whether AMP should initialize as if the session is open
                    or closed

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdramp_powerup_init 
(
  boolean session_is_open
)
{

  if ( hdrmc_is_first_init() )
  {
    /* define timers only occurs at powerup */
    rex_def_timer_ex( &hdramp.uati_assignment_timer,
                      hdramp_timer_cb,
                      HDRAMP_UATI_ASSIGNMENT_TIMER );

#ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
    rex_def_timer_ex( &hdramp.uati_assignment_slp_holdoff_timer,
                      hdramp_timer_cb,
                      HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_TIMER );
#endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

#ifdef FEATURE_LTE_TO_HDR_OH
    rex_def_timer_ex( &hdramp.irma_timer,
                      hdramp_timer_cb,
                      HDRAMP_IRMA_TIMER );
#endif /* FEATURE_LTE_TO_HDR_OH */
  }

  /* initialize amp ati */
  hdrampati_powerup_init();

  hdramp.awaiting_uatireq_tx_outcome = FALSE;

  hdramp_process_init(session_is_open);

#ifdef FEATURE_LTE_TO_HDR_OH
  /* Set rxed_ovhd_after_irat_iho to TRUE so it won't affect the decision of 
   * restoring sessoin or updating uati.
   * Only if LTE to HDR idle ho happens, this flag will be set to FALSE 
   */
  hdramp.rxed_ovhd_after_irat_iho = TRUE; 
  hdramp.last_prereg_is_allowed = TRUE; 
  hdramp.hdr_to_lte_resel = FALSE;
  hdramp.entering_tunnel = FALSE;
  hdramp.irmi_cnt = 0;
  hdramp.irmi_is_pending = FALSE;
  hdramp.awaiting_irmi_tx_cb = FALSE;
  hdramp.current_cell_prereg = FALSE;
  hdramp.ps_is_not_supported = FALSE;
#endif /* FEATURE_LTE_TO_HDR_OH */

} /* hdramp_powerup_init */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRAMP_UPDATE_ESN

DESCRIPTION
  This function sends an ESN update command.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdramp_update_esn
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdramp_cmd_type hdramp_cmd; /* Build command into local struct */

  HDR_MSG_PROT( MSG_LEGACY_LOW, "AMP update ESN");
  
  hdramp_cmd.name   = HDRAMP_UPDATE_ESN_CMD;
  hdramp_cmd.sender = sender;

  if ( rex_self() == HDRAMP_TASK_PTR )
  {
    hdramp_process_cmd(&hdramp_cmd);
  }
  else
  {
    ERR("Update ESN cmd called from outside HDRMC!",0 ,0 ,0);
    hdrmc_queue_cmd( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                     &hdramp_cmd,
                     sizeof(hdramp_cmd_type));
  }

} /* hdramp_update_esn() */

/* <EJECT> */ 
/*=============================================================================
FUNCTION HDRAMP_UPDATE_NAI

DESCRIPTION
  This function sends an NAI update command.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
=============================================================================*/
void hdramp_update_nai
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdramp_cmd_type hdramp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  hdramp_cmd.name   = HDRAMP_UPDATE_NAI_CMD;
  hdramp_cmd.sender = sender;

  if ( rex_self() == HDRAMP_TASK_PTR )
  {
    hdramp_process_cmd(&hdramp_cmd);
  }
  else
  {
    ERR("Update NAI cmd called from outside HDRMC!",0 ,0 ,0);
    hdrmc_queue_cmd( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                     &hdramp_cmd,
                     sizeof(hdramp_cmd_type));
  }

} /* hdramp_update_nai() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_UATI_REQ_TX_OUTCOME_CB

DESCRIPTION
  This function tells AMP that the UATI Request message was sent.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - Pointer to data associated with this callback
  err_val - status of the message transmission.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void hdramp_uati_req_tx_outcome_cb 
( 
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{
  hdramps_state_enum_type cur_state = hdramps_get_state(); /* Current state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (cur_state != HDRAMP_INACTIVE)
  {
    hdramp.awaiting_uatireq_tx_outcome = FALSE;

    switch (err_val)
    {
      /* dual address timer is started upon confirmation of receipt of
       the request */

      case E_SUCCESS:
        HDRTRACE_TXMSG_ACKED( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
           HDRAMP_UATI_REQ_MSG, cur_state); //lint !e506

        /* only start the timer if a valid assignment hasn't already been
           received and processed */
        if (!hdramp.uati_asgn_received)
        {
          if (cur_state == HDRAMP_SETUP)
          {
            /* We are trying to set up a session for the first time.
               Sometimes the network will ACK our UATI Request but never
               send a UATI Assignment, and we don't want to get stuck
               for 120s without even having a session so we use a smaller
               timeout. */
            /* set timer waiting for UATI Assignment Message */
            (void) rex_set_timer(&hdramp.uati_assignment_timer,
                               HDRAMP_INITIAL_UATI_ASSIGN_MS);
             
          }
          else
          {
            /* set timer waiting for UATI Assignment Message */
            (void) rex_set_timer(&hdramp.uati_assignment_timer,
                                 HDRAMP_UATI_ASSIGNMENT_LENGTH_MS);
          }
        }
      break;

      case E_ABORTED:
        HDRTRACE_TXMSG_ABORTED(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
        HDRAMP_UATI_REQ_MSG, cur_state); /*lint !e506 */
  
        /* AMP is responsible for resending UATIReq messages if need be.
           HMP will not auto-resend to avoid sending multiple requests.  We need
           to send another UATI request message to resume opening the session 
           if we abort the message in AMP_SETUP.  (there is no other trigger) */
        if (cur_state == HDRAMP_SETUP)
        {
          HDR_MSG_PROT (MSG_LEGACY_MED, 
                        "Resend UATIReq (aborted during AMPsetup)");
          hdramp_send_uati_req_msg();
        }
        else
        {
          HDR_MSG_PROT (MSG_LEGACY_MED, "UATI req. msg was dropped");
        }
      break;

      case E_FAILURE:
        HDRTRACE_TXMSG_FAILED(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
        HDRAMP_UATI_REQ_MSG, cur_state); /*lint !e506 */

#ifdef FEATURE_LTE_TO_HDR_OH
        if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) )
        {
          /* In tunnel mode, there is no Max Access Probe so AT will not acquire 
           * a different system to send UATIRequest again.
           * set backoff_sending_uati flag to true so AT will re-send when backoff 
           * timer expires.
           */
          if (rex_self() != HDRAMP_TASK_PTR) //L2 ACK failure, not HMP failure
          {
             hdramp.backoff_sending_uati = TRUE;
             hdrsap_set_ac_msg_type(HDRSAP_UATI_REQ);                        
          }
        }
#endif /* FEATURE_LTE_TO_HDR_OH */
      break;

      default:
      break;
    }

    #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
    if ((err_val == E_SUCCESS) && (cur_state == HDRAMP_SETUP))
    {
      /* During initial session negotiation we don't care to hold off other
         messages */
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "SLPQH: Don't hold messages during initial session negotiation." );
      hdrslp_stop_waiting_for_uati_assignment();
    }
    else if (err_val == E_SUCCESS)
    {
      uint32 cur_timer_val;
      /* Start a sanity timer that we will only force SLP to hold off sending
         other messages after UATIRequest for a short period of time
         */
      cur_timer_val = rex_get_timer(&hdramp.uati_assignment_slp_holdoff_timer);

      if (cur_timer_val > 0)
      {
        HDR_MSG_PROT_2( MSG_LEGACY_HIGH, 
          "SLPQH: ACACK for UATIRequest rxed. "
          "Restarting timer for %d ms after IdleHO. "
          "Timer had %d ms remaining.",
           HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_LENGTH_MS, cur_timer_val );
      }
      else
      {
        HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                        "SLPQH: ACACK for UATIRequest rxed.  Starting timer for %d ms.",
                        HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_LENGTH_MS );
      }

      (void) rex_set_timer(&hdramp.uati_assignment_slp_holdoff_timer,
                           HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_LENGTH_MS);

      HDRTRACE_SLP_SLPQH_TIMER_STARTED();
    }
    else if (err_val == E_ABORTED)
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "SLPQH: AHO. Keep holding msgs. "
                    "If AHO=new subnet, resend UATIReq. "
                    "If AHO=old subnet, send msgs after OH Info updated.");
    }
    else
    {
      HDR_MSG_PROT_1( MSG_LEGACY_HIGH, 
                      "SLPQH: UATIRequest TX failed; errno %d", err_val );
      hdramp_done_waiting_for_uati_assignment( 
          HDRAMP_CLEAR_SLP_HOLDOFF_UATIREQ_TX_FAILED );
    }
    #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */
  } /* end if */

} /* hdramp_uati_req_tx_outcome_cb */ //lint !e715 data_ptr not used

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_ACTIVATE

DESCRIPTION
  This function commands AMP to activate.

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdramp_activate
(
  hdrhai_protocol_name_enum_type sender 
)
{
  hdramp_cmd_type hdramp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRAMP_ACTIVATE_CMD received %d", 
                  hdramps_get_state());

  /* Fill in all members of the hdramp_cmd_type for the Activate command */
  hdramp_cmd.name = HDRAMP_ACTIVATE_CMD;
  hdramp_cmd.sender = sender;

  if (rex_self() == HDRAMP_TASK_PTR)
  {
    hdramp_process_cmd( &hdramp_cmd );
  }
  else
  {
    ERR("Activate Command not expected from other task",0,0,0);
    /* This function copies the hdramp_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, &hdramp_cmd, 
                     sizeof (hdramp_cmd) );
  }
} /* hdramp_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_UPDATE_UATI

DESCRIPTION
  This function commands AMP to update the UATI.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdramp_update_uati
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdramp_cmd_type hdramp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRAMP_UPDATE_UATI CMD received %d", 
                  hdramps_get_state());

  /* Fill in all members of the hdramp_cmd_type for the Update UATI command */
  hdramp_cmd.name = HDRAMP_UPDATE_UATI_CMD;
  hdramp_cmd.sender = sender;

  if (rex_self() == HDRAMP_TASK_PTR)
  {
    hdramp_process_cmd( &hdramp_cmd );
  }
  else
  {
    /* This function copies the hdramp_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, &hdramp_cmd,
                     sizeof (hdramp_cmd) );
  }
} /* hdramp_update_uati */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_DEACTIVATE

DESCRIPTION
  This function commands HDRAMP to deactivate

DEPENDENCIES
  None

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdramp_deactivate
(
  hdrhai_protocol_name_enum_type sender
)
{
  hdramp_cmd_type hdramp_cmd; /* Build command into local struct */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_PROT_1 (MSG_LEGACY_MED, "HDRAMP_DEACTIVATE_CMD received %d", 
                  hdramps_get_state());

  /* Fill in all members of the hdramp_cmd_type for the Activate command */
  hdramp_cmd.name = HDRAMP_DEACTIVATE_CMD;
  hdramp_cmd.sender = sender;

  if (rex_self() == HDRAMP_TASK_PTR)
  {
    hdramp_process_cmd( &hdramp_cmd );
  }
  else
  {
    ERR("Deactivate Command not expected from other task",0,0,0);
    /* This function copies the hdramp_cmd into a buffer before it returns */
    hdrmc_queue_cmd( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, &hdramp_cmd,
                     sizeof (hdramp_cmd) );
  }
} /* hdramp_deactivate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SET_PREVIOUS_TO_CURRENT

DESCRIPTION
  This function saves the current session values to previous session 
  information.

DEPENDENCIES
  This function must be called before hdrscmdb_set_previous_to negotiated()
  to correctly update amp session data for previous session.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdramp_set_previous_to_current( void )
{
  /* when we set a session to previous, it's expiration timers are stopped,
     and the expiration time will not change. Therefore, no timer is needed.
     The previous session may be restored as long as the expiration time
     has not passed. Do not directly copy from session_info since 
     expiration time might not be updated. */

  hdrscmdb_get_amp_session_info( &hdramp.session_info );

  memset(hdramp.session_info.expiration_time, 0, sizeof(qword));
  hdrsmpka_get_ka_expire_time(hdramp.session_info.expiration_time);

  hdramp_write_session_info_to_db();

#ifdef FEATURE_MEMCPY_REMOVAL
  hdramp.prior_session_info = hdramp.session_info;
#else
  memcpy(&hdramp.prior_session_info, 
      &hdramp.session_info, sizeof(hdramp_session_info_type));
#endif /* FEATURE_MEMCPY_REMOVAL */
} /* hdramp_set_previous_to_current() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_SET_CURRENT_TO_PREVIOUS

DESCRIPTION
  This function restores the previous session information as current session
  information.

DEPENDENCIES
  None

PARAMETERS
  Restore_UATI - whether the UATI needs to be restored.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void hdramp_set_current_to_previous
(
  boolean restore_UATI
)
{
  hdrhai_ati_record_type ati_record;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AT restores only the session_seed and UATI assignment message sequence
     from the saved prior session but not the UATI and HWID. However the 
     previous UATI is used by the AT while requesting the AN to restore the
     prior session and previous HWID is used to ensure that it is not different
     from the current HWID */
  hdramp.session_info.session_seed = hdramp.prior_session_info.session_seed;
  hdramp.session_info.uati_assign_validation_seq_num = 
      hdramp.prior_session_info.uati_assign_validation_seq_num;

  hdramp.session_info.reduced_subnet_mask_offset = 
      hdramp.prior_session_info.reduced_subnet_mask_offset;

  hdramp.session_info.location = 
      hdramp.prior_session_info.location;
  hdramp.session_info.support_secondary_colorcodes =
      hdramp.prior_session_info.support_secondary_colorcodes;
  hdramp.session_info.max_no_monitor_distance =
      hdramp.prior_session_info.max_no_monitor_distance;

  hdramp.session_info.session_hwid =
    hdramp.prior_session_info.session_hwid;
  hdramp.session_info.session_nai =
    hdramp.prior_session_info.session_nai;

  #if defined(FEATURE_UIM_RUIM) && defined(FEATURE_MMGSDI)
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(hdramp.session_info.icc_id, 
         sizeof(hdramp.session_info.icc_id), 
         hdramp.prior_session_info.icc_id,
         sizeof(hdramp.prior_session_info.icc_id));
#else
  memcpy(hdramp.session_info.icc_id, hdramp.prior_session_info.icc_id,
         sizeof(hdramp.prior_session_info.icc_id));
#endif /* FEATURE_MEMCPY_REMOVAL */
  #endif

  /* if this function is called because of PS GAUP or ConfigReq processing,
     a new UATI's been assigned on this network and should be used. However, 
     if AMP determined AT has reacquired a subnet from prev. session, AT 
     silently restore all prev session data including UATI */
  if (restore_UATI)
  {
    hdraddr_set(hdramp.session_info.full_uati, 
        hdramp.prior_session_info.full_uati);
    hdramp.session_info.uati_subnet_mask = 
        hdramp.prior_session_info.uati_subnet_mask;
    hdramp.session_info.uati_cc = hdramp.prior_session_info.uati_cc;

    ati_record.ati_type = HDRHAI_UATI;
    ati_record.ati_value = HDRAMP_FULL_UATI_TO_UATI32(
        hdramp.session_info.full_uati, hdramp.session_info.uati_cc);
    hdramp_set_transmit_ati(ati_record);
    /* add a new UATI to the receive ati list */
    hdrampati_receive_ati_insert(ati_record);
  }
  /* update the current copy to scmdb */
  hdramp_write_session_info_to_db();
} /* hdramp_set_current_to_previous() */


#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRAMP_IRMI_TX_OUTCOME_CB

DESCRIPTION
  This function tells AMP that the InterRATMobilityIndication message
  was sent.

DEPENDENCIES
  None

PARAMETERS
  data_ptr - Pointer to data associated with this callback
  err_val - status of the message transmission.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdramp_irmi_tx_outcome_cb 
( 
  void *data_ptr,
  hdrerrno_enum_type err_val
)
{
  HDR_MSG_PROT_1( MSG_LEGACY_MED, "TX IRMI cb result %d", err_val );

  switch ( err_val )
  {
    case E_SUCCESS:
      HDRTRACE_TXMSG_ACKED( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                            HDRAMP_IRMI_MSG, hdramps_get_state() );
      break;

    case E_ABORTED:
      HDRTRACE_TXMSG_ABORTED( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                              HDRAMP_IRMI_MSG, hdramps_get_state() );
      break;

    case E_FAILURE:
    default:
      HDRTRACE_TXMSG_FAILED( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                             HDRAMP_IRMI_MSG, hdramps_get_state() );
      break;
  }

  if ( hdramp.awaiting_irmi_tx_cb )
  {
    hdramp.awaiting_irmi_tx_cb = FALSE;

    /* Start the timer waiting for IRMA msg no matter the tx result is */
    (void) rex_set_timer( &hdramp.irma_timer, HDRAMP_IRMA_MS );
  }

} /* hdramp_irmi_tx_outcome_cb() */


/*===========================================================================
 
FUNCTION HDRAMP_SEND_IRMI_MSG

DESCRIPTION
  This function checks whether the InterRATMobilityIndication message is
  sendable and queue to process.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If IRMI is allowed to be sent
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_send_irmi_msg
( 
  hdrhai_protocol_name_enum_type caller
)
{
  hdramp_cmd_type hdramp_cmd;
  uint8 irm_enabled = 0; /* Whether InterRATMobilityEnabled is supported */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check only we have a valid session */
  if ( hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) )
  {
    /* Get the value of InterRATMobilityEnabled */
    (void) hdrscmdb_get_inuse_attrib( 
             HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, 
             HDRSCMAMP_INTER_RAT_MOBILITY_ENABLED_ATTRIB_ID,
             &irm_enabled,
             HDRSCP_DEFAULT_AMP );
  
    /* InterRATMobilityEnabled not supported. Abort sending IRMI msg. */
    if ( !irm_enabled )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "InterRATMobilityEnabled is not supported" );
      return FALSE;
    }
    else if ( hdrscp_get_current_subtype( HDRHAI_SIGNALING_ADAPTATION_PROTOCOL ) 
                != HDRSCP_IRAT_SAP )
    {
      HDR_MSG_PROT( MSG_LEGACY_HIGH, 
                    "Can't send InterRATMobility in non-IRAT personality" );
      return FALSE;
    }
  }

  hdramp_cmd.name = HDRAMP_SEND_IRMI_CMD;
  hdramp_cmd.sender = caller;

  if ( rex_self() == HDRAMP_TASK_PTR )
  {
    hdramp_process_cmd( &hdramp_cmd );
  }
  else
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "%d attempts to send IRMI", caller );
    hdrmc_queue_cmd( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, &hdramp_cmd,
                     sizeof( hdramp_cmd_type ) );
  }
      
  return TRUE;

} /* hdramp_send_irmi_msg() */

/*===========================================================================
 
FUNCTION HDRAMP_PROCESS_SEND_IRMI_MSG

DESCRIPTION
  This function process sending the InterRATMobilityIndication message command

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_process_send_irmi_msg( void )
{

  dsm_item_type *send_msg_ptr; /* DSM pointer to the msg */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( !hdrovhd_info_is_updated(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL) )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Sending IRMI is pending" );
    hdramp.irmi_is_pending = TRUE;
    return;
  }
  else if ( !hdrstream_stream_is_assigned(HDRHAI_APP_PACKET_APP_ANY) )
  {
    /* This is the case that HDR receive sending IRMI request in the middle
     * of RestorePriorSession. Give IRMI_ACCEPTED indication as we know
     * there is ConnectionRequest to be sent to AN */
    HDR_MSG_PROT( MSG_LEGACY_HIGH,
                  "In RestorePriorSession, give IRMI_ACCEPTED" );
    hdramp.irmi_cnt = 0;
    hdrind_give_ind( HDRIND_AMP_IRMI_ACCEPTED, NULL );
    return;
  }

  send_msg_ptr = hdrutil_new_msg_buffer();

  /* PACK IRMI Msg fields */
  HDRAMP_PACK_IRMI_MSG_ID( send_msg_ptr, HDRAMP_IRMI_MSG );

  hdramp.irmi_cnt++;
  hdramp.awaiting_irmi_tx_cb = TRUE;

  hdrhmp_send_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, /* prot sending msg */
                   HDRHAI_AC, /* AC or RTC */
                   FALSE, /* Reliable = TRUE, Best effort = FALSE */
                   HDRAMP_IRMI_MESSAGE_PRIORITY, /* priority */
                   HDRHAI_IN_USE, /* Instance of protocol sending message */
                   send_msg_ptr, /* pointer to the message to be delivered */
                   hdramp_irmi_tx_outcome_cb, /* outcome function */
                   NULL ); /* data to deliver to outcome callback */

} /* hdramp_process_send_irmi_msg */

/*===========================================================================

FUNCTION HDRAMP_STOP_IRMA_TIMER

DESCRIPTION
  This function stops IRMA Timer if it is still running.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_stop_irma_timer( void )
{
  if ( rex_get_timer( &hdramp.irma_timer ) != 0 )
  {
    HDR_MSG_PROT( MSG_LEGACY_HIGH, "Stop IRMI timer" );
    (void) rex_clr_timer( &hdramp.irma_timer );
    hdramp.irmi_cnt = 0;
    hdrind_give_ind( HDRIND_AMP_IRMI_FAILED, NULL );
  }
} /* hdramp_stop_irma_timer() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_CMD

DESCRIPTION
  This function processes commands given to AMP.

DEPENDENCIES
  None

PARAMETERS
  amp_cmd_ptr - Pointer to the command for AMP to process

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_process_cmd
(
  hdramp_cmd_type * amp_cmd_ptr 
)
{
  HDRTRACE_CMD( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, amp_cmd_ptr->name, 
    amp_cmd_ptr->sender, hdramps_get_state() );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( amp_cmd_ptr->name )
  {

    /*-------------------------------------------------------------------------
    Activate Command
    -------------------------------------------------------------------------*/
    case HDRAMP_ACTIVATE_CMD:        /* Begin Opening a Session*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "Process Activate Command");
      switch (hdramps_get_state())
      {
        case HDRAMP_OPEN:
          HDRTRACE_CMD_IGNORED_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
            amp_cmd_ptr->name, amp_cmd_ptr->sender, hdramps_get_state());
        break;

        case HDRAMP_INACTIVE:
          hdramps_change_state(HDRAMP_SETUP);
        break;

        case HDRAMP_SETUP:
          /* fail the current in progress session attempt */
          hdrplog_end_session_attempt(FALSE);
          (void)rex_clr_timer(&hdramp.uati_assignment_timer);
          if (hdrscmdb_is_silent_restore_enabled())
          {
            HDR_MSG_PROT(MSG_LEGACY_MED, 
                         "ACTIVATED in SETUP w/ prev. session. Delay UATIReq");
            hdramp.amp_setup_pending = TRUE;
          }
          else
          {
            hdramp_send_uati_req_msg();
          }
        break;

        default:
          ERR("unknown state during AMP Activate cmd: %d",
              hdramps_get_state(),0,0);
        break;
      }
      break;

    /*-------------------------------------------------------------------------
    Deactivate Command
    -------------------------------------------------------------------------*/
    case HDRAMP_DEACTIVATE_CMD:      /* Deactivate the protocol */
      HDR_MSG_PROT (MSG_LEGACY_MED, "Process Deactivate Command");
      if (hdramps_get_state() == HDRAMP_INACTIVE)
      {
        HDRTRACE_CMD_IGNORED_STATE(HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
          HDRAMP_DEACTIVATE_CMD, amp_cmd_ptr->sender, hdramps_get_state());
      }
      else
      {
        hdramp_set_previous_to_current();

        (void)rex_clr_timer(&hdramp.uati_assignment_timer);

        hdramp.awaiting_uatireq_tx_outcome = FALSE;

        hdramp.session_info.session_seed = 
          (((uint32)ran_dist(hdrutil_get_random(),0,65535)) << 16 | 
                    ran_dist(hdrutil_get_random(),0,65535));

        hdramps_change_state(HDRAMP_INACTIVE);
      }
      break;

    /*-------------------------------------------------------------------------
    Update UATI Command
    -------------------------------------------------------------------------*/
    case HDRAMP_UPDATE_UATI_CMD:     /* Update the UATI */
      HDR_MSG_PROT (MSG_LEGACY_MED, "Process Update UATI Command");
      hdramp_process_update_uati( amp_cmd_ptr );
      break;

    /*-------------------------------------------------------------------------
    Update ESN Command
    -------------------------------------------------------------------------*/
    case HDRAMP_UPDATE_ESN_CMD: /* Update the ESN associated with our session*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "Process Update ESN Command");
      hdramp_process_update_esn( amp_cmd_ptr );
      break;

    /*-------------------------------------------------------------------------
    Update NAI Command
    -------------------------------------------------------------------------*/
    case HDRAMP_UPDATE_NAI_CMD: /* Update the NAI associated with our session*/
      HDR_MSG_PROT (MSG_LEGACY_MED, "Process Update NAI Command");
      hdramp_process_update_nai( amp_cmd_ptr );
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRAMP_SEND_IRMI_CMD:
      HDR_MSG_PROT( MSG_LEGACY_MED, "Process Send IRMI command" );
      hdramp_process_send_irmi_msg();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      break;
  }
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdramp_process_cmd */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PREVIOUS_SESSION_OPENED

DESCRIPTION
  This function is called by SCP when previous session is opened.

DEPENDENCIES
  None

PARAMETERS
  Restore_UATI - Whether the UATI needs to be restored.
  
RETURN VALUE
  TRUE if the previous session is opened successfully, FALSE if there was
  an error (i.e. wrong HWID) and the session was failed.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_previous_session_opened
(
  boolean restore_UATI
)
{
  /* Although not a 'configurable' parameter, the session seed & session's 
   * ESN are actually part of the previous session. */

  if (!hdramp_hwids_are_equal( hdramp.prior_session_info.session_hwid,
                               hdramp.session_info.session_hwid ))
  {
    /* The AT shouldn't restore a prior session with a different HWID
       than the current one.  Close the session immediately if we do */
     hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
     return FALSE;
  }

  #if defined(FEATURE_UIM_RUIM) && defined(FEATURE_MMGSDI)
  if (memcmp(hdramp.prior_session_info.icc_id,
             hdramp.session_info.icc_id,
             sizeof(hdramp.session_info.icc_id)) != 0)
  {
    /* Similiar, the session's ICC_ID should be the same, and the AT
       should not restore a session with a different ICC_ID */
     hdrind_give_ind(HDRIND_AMP_FAILED_NEW_ESN, NULL);
     return FALSE;
  }
  #endif

  /* Similarly, the session's NAI is part of the previous session */
  if ( ( hdramp.prior_session_info.session_nai.user_id_len != 
         hdramp.session_info.session_nai.user_id_len ) ||
       ( memcmp(hdramp.prior_session_info.session_nai.user_id_info,
                hdramp.session_info.session_nai.user_id_info,
                hdramp.prior_session_info.session_nai.user_id_len) != 0 )
     )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH,
                 "Prior session NAI does not match current session NAI");
    /* The AT shouldn't restore a prior session with a different NAI
       than the current one.  Close the session immediately if we do */
     hdrind_give_ind(HDRIND_AMP_FAILED_NEW_NAI, NULL);

     return FALSE;
  }

  hdramp_set_current_to_previous(restore_UATI);

  if (hdramp.req_uati_for_prev)
  {
    hdramp.req_uati_for_prev = FALSE;
    HDR_MSG_PROT(MSG_LEGACY_MED,
                 "Restored prior session w/ dif. subnet, send UATI.");
    hdramp_request_new_ati(hdramp.prev_uati_sect_id);
  }

  return TRUE;
} /* hdramp_previous_session_opened() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRAMP_PROCESS_BACKOFF_TIMER_EXPIRED_IND

DESCRIPTION
  This function processes the backoff timer expired Indication sent by SAP

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdramp_process_backoff_timer_expired_ind( void )
{
  hdramps_state_enum_type amp_state = hdramps_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( amp_state == HDRAMP_INACTIVE)
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                                HDRIND_SAP_BACKOFF_TIMER_EXPIRED, amp_state );
    return;
  }

  if ( hdramp.backoff_sending_uati )
  {
    /* keep retry until receiving backoff_timer_stop_ind */
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "retry UATIRequest, state %d", 
                    amp_state );

    /* We failed to get our initial UATI assignment.  Rather than fail
     * the session at this point, just send another UATI request. */
    hdrplog_end_session_attempt( FALSE );

    hdramp.backoff_sending_uati = FALSE;
    hdramp_send_uati_req_msg();
  }

} /* hdramp_process_backoff_timer_expired_ind() */

/*===========================================================================

FUNCTION HDRAMP_PROCESS_BACKOFF_TIMER_STOPPED_IND

DESCRIPTION
  This function processes the backoff timer stopped Indication sent by SAP

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdramp_process_backoff_timer_stopped_ind( void )
{
  hdramps_state_enum_type amp_state = hdramps_get_state();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( amp_state == HDRAMP_INACTIVE )
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                                HDRIND_SAP_BACKOFF_TIMER_STOPPED, amp_state );
    return;
  }

  if ( hdramp.backoff_sending_uati )
  {
    HDR_MSG_PROT_1( MSG_LEGACY_MED, "Couldn't send out UATIReq, state %d",
                    amp_state );

    hdramp.backoff_sending_uati = FALSE;
    /* PreReg is not allowed, stop sending UATI 
     * If AMP is in SETUP state, HMP will open a new session after receiving
     *                           PreReg update ind 
     * If AMP is in OPEN state, AMP may trigger update UATI procedure again
     *                          after receiving PreRegUpdate ind
     */
  }

} /* hdramp_process_backoff_timer_stopped_ind() */


/*===========================================================================
FUNCTION HDRAMP_PROCESS_IDLE_HANDOFF_IND

DESCRIPTION
  This function processes the idle handoff Indication sent by SAP

DEPENDENCIES
  None.

PARAMETERS
  ind_data_ptr - pre-registration is allowed or not for the last LTE cell 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hdramp_process_idle_handoff_ind
(
  hdrind_ind_data_union_type *ind_data_ptr
)
{
  if ( hdramps_get_state() != HDRAMP_OPEN ) 
  {
    HDRTRACE_IND_IGNORED_STATE( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL,
                                HDRIND_SAP_IDLE_HANDOFF, 
                                hdramps_get_state() );

    HDR_MSG_PROT_1( MSG_LEGACY_HIGH, "Rxed SAP IDLE HO IND, AMP state: %d",
                    hdramps_get_state() );
    return;
  }

  hdramp.last_prereg_is_allowed = ind_data_ptr->last_prereg_is_allowed;
  hdramp.rxed_ovhd_after_irat_iho = FALSE;

} /* hdramp_process_idle_handoff_ind() */
#endif  /* FEATURE_LTE_TO_HDR_OH*/


/*===========================================================================
FUNCTION HDRAMP_PROCESS_IND

DESCRIPTION
  This function processes indications given to AMP.

DEPENDENCIES
  None

PARAMETERS
  ind_name     - Name of the indication for AMP to process
  ind_data_ptr - Any data associated with the indication
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdramp_process_ind
(
  hdrind_ind_name_enum_type ind_name, 
  hdrind_ind_data_union_type *ind_data_ptr 
)
{
  HDRTRACE_IND( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, ind_name, 
                hdramps_get_state() );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

  switch ( ind_name )
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRIND_OVHD_PREREG_INFO_UPDATED:
      hdramp_process_prereg_info_updated( ind_data_ptr );
      break;

    case HDRIND_SAP_BACKOFF_TIMER_EXPIRED:
      hdramp_process_backoff_timer_expired_ind();
      break;

    case HDRIND_SAP_BACKOFF_TIMER_STOPPED:
      hdramp_process_backoff_timer_stopped_ind();
      break;

    case HDRIND_SAP_IDLE_HANDOFF:
      hdramp_process_idle_handoff_ind( ind_data_ptr );
      break;

    case HDRIND_OVHD_LTE_CELL_CHANGED:
      hdramp_process_lte_cell_changed( ind_data_ptr );
      break;

    case HDRIND_ALMP_EXIT_HDR_MODE:
      if ( hdramp.irmi_is_pending || hdramp.awaiting_irmi_tx_cb )
      {
        HDR_MSG_PROT( MSG_LEGACY_HIGH, "Abort IRMI when exit HDR" );
        hdramp.irmi_cnt = 0;
        hdramp.irmi_is_pending = FALSE;
        hdramp.awaiting_irmi_tx_cb = FALSE;
        hdrind_give_ind( HDRIND_AMP_IRMI_FAILED, NULL );
      }

      hdramp_stop_irma_timer();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    case HDRIND_OVHD_INFO_UPDATED:
      hdramp_process_ovhd_info_updated( ind_data_ptr );
      break;

    default:
      HDR_MSG_PROT_1( MSG_LEGACY_ERROR, 
                      "Unrecognized Indication %d", ind_name );
      break;
  }

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif

} /* hdramp_process_ind() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_MSG

DESCRIPTION
  This function processes messages for AMP.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to the DSM item chain holding the message
  protocol_instance - Instance of the protocol the message is for

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_process_msg
( 
  dsm_item_type * item_ptr, 
  hdrhai_protocol_instance_enum_type protocol_instance
)
{
  uint8 msg_id;           /* Message identifier */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((protocol_instance == HDRHAI_IN_USE) ||
         (protocol_instance == HDRHAI_IN_CONFIG)); 

  /* MessageID is the first uint8 */
  if (HDRAMP_UNPACK_MSG_ID(item_ptr, &(msg_id)) != E_SUCCESS)
  {
    ERR( "Could not extract message_id", 0, 0, 0 );
    dsm_free_packet( &item_ptr );
    return;
  }

  /* Currently only the in use instance receives any messages */
  if (protocol_instance != HDRHAI_IN_USE)
  {
    ERR("Msg for wrong instance %d", msg_id, 0, 0 );
    dsm_free_packet(&item_ptr);
    return;
  }

  HDRTRACE_MSG( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL, msg_id, 
    hdramps_get_state() );

#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
  switch ( msg_id )
  {
    case HDRAMP_UATI_ASGN_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "UATI Assignment Msg received");
      hdramp_process_uati_assign_msg(item_ptr);
      break;

    case HDRAMP_HARDWARE_ID_REQ_MSG:
      HDR_MSG_PROT (MSG_LEGACY_LOW, "Hardware ID Request Msg received");
      hdramp_process_hardware_req_msg(item_ptr);
      break;

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRAMP_IRMA_MSG:
      HDR_MSG_PROT( MSG_LEGACY_LOW, "Inter RAT Mobility Ack Msg received" );
      hdramp_process_irma_msg( item_ptr );
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      ERR ("HDRAMP unknown message %d", msg_id, 0, 0);
      break;
  }
  dsm_free_packet( &item_ptr );
#ifdef FEATURE_HDRMC_DEBUG
  #error code not present
#endif
} /* hdramp_process_msg */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRAMP_PROCESS_IRMA_TIMER_EXP

DESCRIPTION
  This function processes the IRMA Timer expiration

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void hdramp_process_irma_timer_exp( void )
{
  /* Clear timer */
  (void) rex_clr_timer( &hdramp.irma_timer );

  if ( hdramp.irmi_cnt < HDRAMP_IRMI_MAX_RETRY )
  {
    /* Retry sending IRMI until max attempt is reached. */

    /* This should not happen since the first IRMI is allowed to be sent.
     * Give FAILED indication in case the retry of IRMI is not allowed. */
    if ( !hdramp_send_irmi_msg( HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) )
    {
      hdramp.irmi_cnt = 0;
      hdrind_give_ind( HDRIND_AMP_IRMI_FAILED, NULL );
    }
  }
  else
  {
    /* AMP did not receive IRMA in time and AMP reaches the max retry of
     * sending IRMI. */
    hdramp.irmi_cnt = 0;
    hdrind_give_ind( HDRIND_AMP_IRMI_FAILED, NULL );
  }

} /* hdramp_process_irma_timer_exp() */
#endif /* FEATURE_LTE_TO_HDR_OH */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMP_PROCESS_TIMER

DESCRIPTION
  Callback function that gets called when a timer for hdramp expires

DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdramp_process_timer
( 
  uint32 timer_id
)
{

  /*---------------------------------------------------------------------------
  Determine which amp timer has expired.
  ---------------------------------------------------------------------------*/
  switch ( (hdramp_timer_name_enum_type) timer_id )
  {
    case HDRAMP_N_ADDR_TIMER:
      /*-----------------------------------------------------------------------
      N-Address Timer: Delete the oldest uati and reset the timer if we still 
                       have multiple valid uatis
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_DUAL_ADDR_SIG( hdramps_get_state() );
      hdrampati_receive_ati_delete(HDRHAI_UATI);

      break;

    case HDRAMP_UATI_ASSIGNMENT_TIMER:
      /*-----------------------------------------------------------------------
      UATI Assignment Timer: 
      -----------------------------------------------------------------------*/
      HDRTRACE_HDRMC_UATI_ASSIGN_SIG( hdramps_get_state() );
      hdramp_process_uati_assign_timer_exp();

      break;

    #ifdef FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT
    case HDRAMP_UATI_ASSIGNMENT_SLP_HOLDOFF_TIMER:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "SLPQH: Timer expired." );
      hdramp_done_waiting_for_uati_assignment(
          HDRAMP_CLEAR_SLP_HOLDOFF_UATIREQ_TIMER_EXPIRED);
      break;
    #endif /* FEATURE_HDR_SLP_WAIT_FOR_UATI_ASSIGNMENT */

#ifdef FEATURE_LTE_TO_HDR_OH
    case HDRAMP_IRMA_TIMER:
      HDR_MSG_PROT( MSG_LEGACY_HIGH, "IRMA Timer expired." );
      hdramp_process_irma_timer_exp();
      break;
#endif /* FEATURE_LTE_TO_HDR_OH */

    default:
      /*-----------------------------------------------------------------------
      Unrecognized Timer: 
      -----------------------------------------------------------------------*/      
      ERR("Invalid Timer ID (%d)",timer_id,0,0);
      break;
  }
  
} /* hdramp_process_timer() */

/*===========================================================================

FUNCTION HDRAMP_IS_PREVIOUS_UATI_VALID

DESCRIPTION
  This function checks if previous UATI has expired

DEPENDENCIES
  None

PARAMETERS
  valid - indicates if previous session expired
  
RETURN VALUE
  TRUE if Success
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_is_previous_uati_valid( void )
{
  qword current_time;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrsmpka_get_time_source(current_time);
  /* if expiration_time for prev session is unknown (all 0), we assume
     it is not expired */
  if ((qw_hi(hdramp.prior_session_info.expiration_time) == 0) &&
      (qw_lo(hdramp.prior_session_info.expiration_time) == 0))
  {
    return TRUE;
  }
  return qw_cmp(hdramp.prior_session_info.expiration_time, 
      current_time) > 0;
}

/*===========================================================================

FUNCTION HDRAMP_SILENT_RESTORE_PRIOR_SESSION

DESCRIPTION
  This function silently restore prior session

DEPENDENCIES
  hdrsmps_silent_restore_prior_session() must be called before this.

PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdramp_silent_restore_prior_session( void )
{
  hdramps_silent_restore_prior_session();
  (void) hdrsmpka_set_expire_time(hdramp.prior_session_info.expiration_time);
}

#if defined (FEATURE_DEDICATED_C2K_MODE) || defined (FEATURE_IRAT_PILOT_MEAS)
/*===========================================================================

FUNCTION HDRAMP_GET_DCM_REACQ_DIFF_SUBNET

DESCRIPTION
  Check if AT has reacq-ed during DCM mode

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - AT has reacq-ed during DCM
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_get_dcm_reacq_diff_subnet( void )
{
  return hdramp.dcm_reacq_diff_subnet;
}
#endif /* FEATURE_DEDICATED_C2K_MODE */

/*===========================================================================

FUNCTION HDRAMP_UATI_MATCH_SUBNET

DESCRIPTION
  Check if the current UATI address matches the subnet from SP MSG.
  This function should only be called when OVHD INFO is current.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - UATI is of the same subnet of the serving AN
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_uati_match_subnet( void )
{
  hdraddr_type sp_sector_id;
  uint8 sp_subnet_mask;  
  hdramp_subnet_changed_enum_type subnet_changed;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( hdrovhd_get_sector_id( sp_sector_id, &sp_subnet_mask,
      HDRHAI_ADDRESS_MANAGEMENT_PROTOCOL ) == E_SUCCESS )
  {
  /* Check if the subnet has been changed */
    subnet_changed = 
      hdramp_subnet_has_changed( sp_sector_id, sp_subnet_mask,
                                 hdramp.session_info.full_uati,
                                 hdramp.session_info.uati_subnet_mask, 0 );
  }
  else
  {
    ERR( "Could not get Sector ID from OVHD", 0, 0, 0 );
    subnet_changed = HDRAMP_SUBNET_ERROR;
  }
  return (subnet_changed == HDRAMP_SUBNET_UNCHANGED);
}

/*===========================================================================

FUNCTION HDRAMP_IS_SESSION_CLOSE_NAI_PENDNG

DESCRIPTION
  This function checks if sesson close is pending due to NAI update

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE if session close is pending
  
SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_is_session_close_nai_pending( void )
{
  return hdramp.session_close_nai_pending;
} /* hdramp_is_session_close_nai_pending() */

/*===========================================================================

FUNCTION HDRAMP_SET_SESSION_CLOSE_NAI_PENDNG

DESCRIPTION
  This function checks sets session_close_nai_pending flag.

DEPENDENCIES
  None

PARAMETERS
  FALSE to set session_close_nai_pending to FALSE.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/
void hdramp_set_session_close_nai_pending
( 
  boolean value
)
{
  if ( hdramp.session_close_nai_pending &&
       !value )
  {
    /* Set session_close_nai_pending to FALSE only if we know session is closed.
     * Update session_info.session_nai */
    HDR_MSG_PROT( MSG_LEGACY_MED, "Reset session_close_nai_pending flag" );
    hdramp.session_info.session_nai.user_id_len = hdramp.session_nai_pending.user_id_len;
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( hdramp.session_info.session_nai.user_id_info,
            sizeof(hdramp.session_info.session_nai.user_id_info),
            hdramp.session_nai_pending.user_id_info,
            hdramp.session_nai_pending.user_id_len );
#else
    memcpy( hdramp.session_info.session_nai.user_id_info,
            hdramp.session_nai_pending.user_id_info,
            hdramp.session_nai_pending.user_id_len );
#endif /* FEATURE_MEMCPY_REMOVAL */
  }

  hdramp.session_close_nai_pending = value;
} /* hdramp_set_session_close_nai_pending() */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================
FUNCTION HDRAMP_SET_PS_IS_NOT_SUPPORTED

DESCRIPTION
  This function sets the flag ps_is_not_supported, which indicates whether
  we should replace all the restore prior session with update UATI in the
  tunnel mode.
  
DEPENDENCIES
  None.

PARAMETERS
  value - whether prior session is supported.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdramp_set_ps_is_not_supported
(
  boolean value
)
{
  hdramp.ps_is_not_supported = value;

  HDR_MSG_PROT_1( MSG_LEGACY_MED, 
                  "set ps_is_not_supported to %d", hdramp.ps_is_not_supported );
} /* hdramp_set_ps_is_not_supported() */

/*===========================================================================

FUNCTION HDRAMP_IS_WAITING_FOR_UATI_ASSGN

DESCRIPTION
  Check if AMP is waiting for UATI Assignment

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE - If AMP is waiting for UATIAssignment
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdramp_is_waiting_for_uati_assgn( void )
{
  return hdramp.waiting_uati_asgn;
}
#endif /* FEATURE_LTE_TO_HDR_OH */

