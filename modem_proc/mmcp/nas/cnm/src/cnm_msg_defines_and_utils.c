/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_msg_defines_and_utils.c_v   1.8   12 Jul 2002 17:16:20   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_msg_defines_and_utils.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

17-Sep-2001  VT    Modified CNM_check_IE_syntax for case IEI_BEARER_CAPABILITY
                     to add checks for octets 6d & 6e. Also, added a lot of 
                     comments throughout that case to make it more readable.

10/22/01   AB      Replaced all SS state references with SS state changes access
                   function, CNM_get_ss_state().

12-Mar-2002 VT     Removed Lint errors

04/19/02   CD      Deleted function CNM_check_for_last_connection_released()
                   Updated Copyright
                   Replaced ERR macro by MSG_ERR

04/30/02   CD      Added function CNM_check_for_last_connection()

06/25/02   CD      In CNM_write_cause_ie() change return value to num of bytes
                   written rather than a hardcoded value of 4

07/01/02   CD      Fixed 'unusual pointer cast' lint error
                   Fixed 'loss of sign in promotion' lint error

07/02/02   CD      Added check for IEI_CAUSE_OF_NO_CLI in 
                   CNM_check_IE_syntax()

08/26/02   AB      In CNM_check_for_last_connection(), added a checking
                   for call in re-estblishment mode, which means MM already
                   response to MMCNM_EST_REQ, before sending out the potential
                   NORMAL RELEASE or ABORT request to MM.

09/04/02   AB      In CNM_check_IE_syntax(), added checking for 
                   IEI_REDIR_PARTY_BCD_NO, and IEI_REDIR_PARTY_SUBAD, redirecting
                   party BCD number and Subaddress.

09/18/02   AB      IN CNM_check_for_last_connection(), route the MMCNM_REL_REQ
                   message to MN_CNM queue so that pending CNM/MN events
                   can be processed before the radio channel is released.

11/27/02   AB      In CNM_check_IE_syntax(), add checking for octet 5 
                   for other rate adaption that supports multimedia services.    

02/15/03   AB      Added indentation in CNM_check_for_last_connection() per Lint.

12/15/04    AB     Added supports for AT+CMMS=1, ATCOP multiple MO SMS.

05/10/05    HS     Added support for FEATURE_CCBS

09/01/05    AB     Restored the last sync indication on possible TCH reused.

02/06/07    RD     Added fixes for kloc errors

08/13/07    PP     Removed kloc feature flag

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "ms.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"

#include "nasutils.h"
#include "mn_sms_v.h"
#include <stringl/stringl.h>

#define MM_AS_ID_1 1
extern boolean CNM_has_active_cc_connection(void);
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );

/*
 * Local definitions
 */
 
/*
 * The following macro functions are used to simplify the data entry.
 * The linked list data is entered in the following form:
 *
 * ROOT_MESSAGE(<first_message_name>)
 * MESSAGE_DETAIL(<first_message_name>, 
 *                <message_type>, 
 *                <direction, MO, MT, or BI>,
 *                <Length of imperative part of message>,
 *                <next message name>
 *                <name of first IE in this message> )
 *
 * Then for each IE in the message, there is one entry as follows:
 *
 * IE_DETAIL( <first IE name>,
 *            <corresponding name of the type of IE from the IE_TYPE table>,
 *            <format of the IE in this message>,
 *            <either MANDATORY, OPTIONAL, or CONDITIONAL depending on presence field 
 *                                                            from 04.08>,
 *            <name of next IE in message>
 *
 * The last IE_DETAILS has the name END_POINTER in the next IE field
 *
 *
 * The last MESSAGE_DETAIL has END_POINTER for the next message name field
 *
 *
 * THE ONLY OTHER PROBLEM IS THAT THE DATA IS SPECIFIED IN REVERSE ORDER SO
 * THAT THE ADDRESSES FOR THE LINKED LIST ARE KNOWN WHEN THEY ARE REFERENCED
 *
 *
 * The IE_TYPE table is a list of IE_TYPE macro function calls in any order
 * There is no list linking within this table.
 */

#define MESSAGE_DETAIL(Name,\
                       message_type, \
                       Direction, \
                       ImperativeLength, \
                       NextName, \
                      FirstMessageIE) \
\
   struct MsgDetailTag Name = \
   { \
      message_type, \
      Direction, \
      ImperativeLength, \
      (struct MsgDetailTag *) & NextName,  \
      (struct MessageIETag *) & FirstMessageIE \
   };

#define IE_DETAIL( Name, \
                   TypeRef, \
                   Format, \
                   Mandatory, \
                   NextImpIE ) \
\
   struct MessageIETag Name = \
      { \
         &TypeRef, \
         FORMAT_ ## Format, \
         Mandatory, \
         (struct MessageIETag *) & NextImpIE \
};

#define IE_TYPE( Name, \
                 IEI, \
                 Length, \
                 Type ) \
\
   struct IETypeTag Name = \
   { \
      IEI, \
      Length, \
      Type \
   };

/*
 * Define a dummy address to use as a list terminator
 */

struct MessageIETag EndLoc;
struct MsgDetailTag EndMessagesLoc;

#define END_POINTER EndLoc
#define END_MESSAGES EndMessagesLoc

#define ROOT_MESSAGE(Name) VOID_DATA * CNM_RootName = & Name;

/* Commented by Vtawker while removing Lint errors, as this macro is
 * not used anywhere
#define VALID_MSG_TAG(MsgPtr) ((MsgPtr) != (struct MsgDetailTag   *)0)
 */

/*
 * definition of the information elements we are likely to come across. The
 * entries are as follows:
 *
 * Type of information element -----------------------------------------.
 *                                                                      |
 * Maximum length of the information element ----------------------.    |
 *                                                                 |    |
 * Value of the information                                        |    |
 * element identifier -----------------------.                     |    |
 *                                           |                     |    |
 * Arbitrary name                            |                     |    |
 * given to entry                            |                     |    |
 * - This is used                            |                     |    |
 * to reference                              |                     |    |
 * the entry                                 |                     |    |
 * elsewhere --.                             |                     |    |
 *             |                             |                     |    |
 *             v                             v                     v    v
 */

IE_TYPE( cp_cause_ie,                 IEI_CP_CAUSE,                 2,  3 )
IE_TYPE( cp_user_data_ie,             IEI_CP_USER_DATA,           255,  4 )

IE_TYPE( shift_ie,                    IEI_SHIFT,                     1, 1 )
IE_TYPE( aux_states_ie,               IEI_AUXILIARY_STATES,          3, 4 )
IE_TYPE( bearer_capability_ie,        IEI_BEARER_CAPABILITY,        16, 4 )
IE_TYPE( call_control_cap_ie,         IEI_CALL_CONTROL_CAPABIL,      4, 4 )
IE_TYPE( call_state_ie,               IEI_CALL_STATE,                1, 3 )
IE_TYPE( called_party_bcd_no_ie,      IEI_CALLED_PARTY_BCD_NO,      43, 4 )
IE_TYPE( called_party_subadd_ie,      IEI_CALLED_PARTY_SUBAD,       23, 4 )
IE_TYPE( calling_party_bcd_no_ie,     IEI_CALLING_PARTY_BCD_NO,     14, 4 )
IE_TYPE( calling_party_subadd_ie,     IEI_CALLING_PARTY_SUBAD,      23, 4 )
IE_TYPE( cause_ie,                    IEI_CAUSE,                    32, 4 )
IE_TYPE( clir_suppression_ie,         IEI_CLIR_SUPPRESSION,          1, 2 )
IE_TYPE( clir_invocation_ie,          IEI_CLIR_INVOCATION,           1, 2 )
IE_TYPE( congestion_level_ie,         IEI_CONGESTION_CONTROL,        1, 1 )
IE_TYPE( connected_no_ie,             IEI_CONNECTED_NUMBER,         14, 4 )
IE_TYPE( connected_subadd_ie,         IEI_CONNECTED_SUBADDRESS,     23, 4 )
IE_TYPE( facility_ie,                 IEI_FACILITY,                255, 4 )
IE_TYPE( high_level_compat_ie,        IEI_HIGH_LAYER_COMPATIB,       5, 4 )
IE_TYPE( keypad_facility_ie,          IEI_KEYPAD_FACILITY,           2, 3 )
IE_TYPE( low_layer_compat_ie,         IEI_LOW_LAYER_COMPATIB,       18, 4 )
IE_TYPE( more_data_ie,                IEI_MORE_DATA,                 1, 2 )
IE_TYPE( notification_ind_ie,         IEI_NOTIFICATION_IND,          1, 3 )
IE_TYPE( progress_indicator_ie,       IEI_PROGRESS_INDICATOR,        4, 4 )
IE_TYPE( redir_party_bcd_no_ie,       IEI_REDIR_PARTY_BCD_NO,       19, 4 )
IE_TYPE( redir_party_subadd_ie,       IEI_REDIR_PARTY_SUBADDR,      23, 4 )
IE_TYPE( repeat_ind_ie,               IEI_REPEAT_INDICATOR,          1, 1 )
IE_TYPE( reverse_call_setup_ie,       IEI_REVERSE_CALL_SETUP_DIR,    1, 2 )
IE_TYPE( signal_ie,                   IEI_SIGNAL,                    2, 3 )
IE_TYPE( ss_version_ind_ie,           IEI_SS_VERSION_INDICATOR,    255, 4 )
IE_TYPE( user_user_ie,                IEI_USER_USER,               131, 4 )
IE_TYPE( cause_of_no_CLI_ie,          IEI_CAUSE_OF_NO_CLI,           3, 4 )
IE_TYPE( alerting_pattern_ie,         IEI_ALERTING_PATTERN,          3, 4 )
#ifdef FEATURE_CCBS
IE_TYPE( allowed_actions_ie,          IEI_ALLOWED_ACTIONS,           3, 4 )
IE_TYPE( recall_type_ie,              IEI_RECALL_TYPE,               1, 3 )
IE_TYPE( setup_container_ie,          IEI_SETUP_CONTAINER,         255, 4 ) 
#endif /* FEATURE_CCBS */
IE_TYPE( stream_identifier_ie,        IEI_STREAM_IDENTIFIER,         3, 4 )
IE_TYPE( codecs_list_ie,              IEI_SUPPORTED_CODECS_LIST,    66, 4 )
IE_TYPE( network_cc_capability_ie,    IEI_NETWORK_CC_CAPABILITY,     3, 4 )
IE_TYPE( backup_bearer_capability_ie, IEI_BACKUP_BEARER_CAPABILITY, 16, 4 )
IE_TYPE( modify_bearer_capability_ie, IEI_BEARER_CAPABILITY,        15, 3 )
IE_TYPE( immediate_mod_ind_ie,        IEI_IMMEDIDATE_MOD_IND,        1, 2 )
IE_TYPE( nw_initiated_svc_upgrade_ind_ie,        IEI_NW_INITIATED_SVC_UPGRADE_IND,        1, 2 )

/*
 * REGISTER message
 */

IE_DETAIL(RegFacility           , facility_ie,             TLV, MANDATORY_IE, END_POINTER ) 
MESSAGE_DETAIL(RegisterMT       , REGISTER,            MT_CALL, 255, END_MESSAGES, RegFacility )

/*
 * CP_error message
 */

IE_DETAIL(CPCause               , cp_cause_ie,             V, MANDATORY_IE, END_POINTER ) 
MESSAGE_DETAIL(CPError          , CP_ERROR,            BI_CALL, 3, RegisterMT, CPCause )

/*
 * CP_ack message
 */

MESSAGE_DETAIL(CPAck            , CP_ACK,              BI_CALL, 2, CPError, END_POINTER )

/*
 * CP_data message
 */

IE_DETAIL(CPUserData            , cp_user_data_ie,        LV, MANDATORY_IE, END_POINTER ) 
MESSAGE_DETAIL(CPData           , CP_DATA,             BI_CALL, 3, CPAck, CPUserData )

/*
 * User information message
 */

IE_DETAIL(UserInfoMoreData      , more_data_ie,            T, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(UserInfoUserUser      , user_user_ie,            LV, MANDATORY_IE, UserInfoMoreData ) 
MESSAGE_DETAIL(UserInfo         , USER_INFORMATION,   MT_CALL, 132, CPData, UserInfoUserUser )

/*
 * Stop DTMF Ack message
 */

MESSAGE_DETAIL(StopDTMFAck      , STOP_DTMF_ACK,      MT_CALL, 2, UserInfo, END_POINTER )

/*
 * Stop DTMF message
 */

MESSAGE_DETAIL(StopDTMF         , STOP_DTMF,          MO_CALL, 2, StopDTMFAck, END_POINTER )

/*
 * Statu enquiry message
 */

MESSAGE_DETAIL(StatusEnquiry    , STATUS_ENQUIRY,     BI_CALL, 2, StopDTMF, END_POINTER )

/*
 * Status message
 */

IE_DETAIL(StatusAuxState        , aux_states_ie,           TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(StatusCallState       , call_state_ie,           V, MANDATORY_IE, StatusAuxState ) 
IE_DETAIL(StatusCause           , cause_ie,                LV, MANDATORY_IE, StatusCallState ) 
MESSAGE_DETAIL(Status           , STATUS_MSG,             BI_CALL, 34, StatusEnquiry, StatusCause )

/*
 * Start DTMF reject message
 */

IE_DETAIL(StartDTMFRejCause     , cause_ie,                LV, MANDATORY_IE, END_POINTER ) 
MESSAGE_DETAIL(StartDTMFRej     , START_DTMF_REJ,     MT_CALL, 35, Status, StartDTMFRejCause )

/*
 * Start DTMF Ack message
 */

IE_DETAIL(StartDTMFAckKeypadFac , keypad_facility_ie,      TV, MANDATORY_IE, END_POINTER ) 
MESSAGE_DETAIL(StartDTMFAck     , START_DTMF_ACK,     MT_CALL, 4, StartDTMFRej, StartDTMFAckKeypadFac )

/*
 * Start DTMF message
 */

IE_DETAIL(StartDTMFKeypadFac    , keypad_facility_ie,     TV, MANDATORY_IE, END_POINTER ) 
MESSAGE_DETAIL(StartDTMF        , START_DTMF,         MO_CALL, 4, StartDTMFAck, StartDTMFKeypadFac )

/*
 * Setup message, mobile originating direction
 */
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
IE_DETAIL(SetupMOCodecsList     , codecs_list_ie,          TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(SetupMOStreamId       , stream_identifier_ie,    TLV, OPTIONAL_IE, SetupMOCodecsList ) 
#else
IE_DETAIL(SetupMOStreamId       , stream_identifier_ie,    TLV, OPTIONAL_IE, END_POINTER ) 
#endif
IE_DETAIL(SetupMOCCCapab        , call_control_cap_ie,     TLV, OPTIONAL_IE, SetupMOStreamId ) 
IE_DETAIL(SetupMOCLIRInvocate   , clir_invocation_ie,      T, CONDITIONAL_IE, SetupMOCCCapab ) 
IE_DETAIL(SetupMOCLIRSuppress   , clir_suppression_ie,     T, CONDITIONAL_IE, SetupMOCLIRInvocate ) 
IE_DETAIL(SetupMOSSVersion      , ss_version_ind_ie,       TLV, OPTIONAL_IE, SetupMOCLIRSuppress ) 
IE_DETAIL(SetupMOUserUser       , user_user_ie,            TLV, OPTIONAL_IE, SetupMOSSVersion ) 
IE_DETAIL(SetupMOHLC2           , high_level_compat_ie,    TLV, OPTIONAL_IE, SetupMOUserUser )
IE_DETAIL(SetupMOHLC1           , high_level_compat_ie,    TLV, OPTIONAL_IE, SetupMOHLC2 )
IE_DETAIL(SetupMOHLCRepeat      , repeat_ind_ie,           TV, OPTIONAL_IE, SetupMOHLC1 )
IE_DETAIL(SetupMOLLC2           , low_layer_compat_ie,     TLV, OPTIONAL_IE, SetupMOHLCRepeat )
IE_DETAIL(SetupMOLLC1           , low_layer_compat_ie,     TLV, OPTIONAL_IE, SetupMOLLC2 )
IE_DETAIL(SetupMOLLCRepeat      , repeat_ind_ie,           TV, OPTIONAL_IE, SetupMOLLC1 )
IE_DETAIL(SetupMOCalledSubadd   , called_party_subadd_ie,  TLV, OPTIONAL_IE, SetupMOLLCRepeat )
IE_DETAIL(SetupMOCalledBCDNo    , called_party_bcd_no_ie,  TLV, MANDATORY_IE, SetupMOCalledSubadd )
IE_DETAIL(SetupMOCallingSubadd  , calling_party_subadd_ie, TLV, OPTIONAL_IE, SetupMOCalledBCDNo )
IE_DETAIL(SetupMOFacility       , facility_ie,             TLV, OPTIONAL_IE, SetupMOCallingSubadd )
IE_DETAIL(SetupMOBC2            , bearer_capability_ie,    TLV, OPTIONAL_IE, SetupMOFacility )
IE_DETAIL(SetupMOBC1            , bearer_capability_ie,    TLV, MANDATORY_IE, SetupMOBC2 )
IE_DETAIL(SetupMOBCRepeat       , repeat_ind_ie,           TV, CONDITIONAL_IE, SetupMOBC1 )
MESSAGE_DETAIL(SetupMO          , SETUP,              MO_CALL, 2, StartDTMF, SetupMOBCRepeat )

/*
 * Setup message, mobile terminating direction
 */
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
IE_DETAIL(SetupMTBackupBC       , backup_bearer_capability_ie,TLV, OPTIONAL_IE, END_POINTER)
IE_DETAIL(SetupMTNWCCCapability , network_cc_capability_ie,TLV, OPTIONAL_IE, SetupMTBackupBC)
IE_DETAIL(SetupMTCauseOfNoCLI   , cause_of_no_CLI_ie,      TLV, OPTIONAL_IE, SetupMTNWCCCapability)
#else
IE_DETAIL(SetupMTCauseOfNoCLI   , cause_of_no_CLI_ie,      TLV, OPTIONAL_IE, END_POINTER)
#endif
IE_DETAIL(SetupMTAlertingPattern , alerting_pattern_ie,  TLV, OPTIONAL_IE, SetupMTCauseOfNoCLI) 
IE_DETAIL(SetupMTUserUser       , user_user_ie,            TLV, OPTIONAL_IE, SetupMTAlertingPattern) 
IE_DETAIL(SetupMTHLC2           , high_level_compat_ie,    TLV, CONDITIONAL_IE, SetupMTUserUser )
IE_DETAIL(SetupMTHLC1           , high_level_compat_ie,    TLV, OPTIONAL_IE, SetupMTHLC2 )
IE_DETAIL(SetupMTHLCRepeat      , repeat_ind_ie,           TV, OPTIONAL_IE, SetupMTHLC1 )
IE_DETAIL(SetupMTLLC2           , low_layer_compat_ie,     TLV, CONDITIONAL_IE, SetupMTHLCRepeat )
IE_DETAIL(SetupMTLLC1           , low_layer_compat_ie,     TLV, OPTIONAL_IE, SetupMTLLC2 )
IE_DETAIL(SetupMTLLCRepeat      , repeat_ind_ie,           TV, OPTIONAL_IE,  SetupMTLLC1 )
IE_DETAIL(SetupMTRedirectingSubadd, redir_party_subadd_ie, TLV, OPTIONAL_IE, SetupMTLLCRepeat )
IE_DETAIL(SetupMTRedirectingBCDNo, redir_party_bcd_no_ie,  TLV, OPTIONAL_IE, SetupMTRedirectingSubadd )
IE_DETAIL(SetupMTCalledSubadd   , called_party_subadd_ie,  TLV, OPTIONAL_IE, SetupMTRedirectingBCDNo )
IE_DETAIL(SetupMTCalledBCDNo    , called_party_bcd_no_ie,  TLV, OPTIONAL_IE, SetupMTCalledSubadd )
IE_DETAIL(SetupMTCallingSubadd  , calling_party_subadd_ie, TLV, OPTIONAL_IE, SetupMTCalledBCDNo )
IE_DETAIL(SetupMTCallingBCDNo   , calling_party_bcd_no_ie, TLV, OPTIONAL_IE, SetupMTCallingSubadd )
IE_DETAIL(SetupMTSignal         , signal_ie,               TV, OPTIONAL_IE,  SetupMTCallingBCDNo ) 
IE_DETAIL(SetupMTProgInd        , progress_indicator_ie,   TLV, OPTIONAL_IE, SetupMTSignal ) 
IE_DETAIL(SetupMTFacility       , facility_ie,             TLV, OPTIONAL_IE, SetupMTProgInd )
IE_DETAIL(SetupMTBC2            , bearer_capability_ie,    TLV, OPTIONAL_IE, SetupMTFacility )
IE_DETAIL(SetupMTBC1            , bearer_capability_ie,    TLV, OPTIONAL_IE, SetupMTBC2 )
IE_DETAIL(SetupMTBCRepeat       , repeat_ind_ie,           TV, CONDITIONAL_IE, SetupMTBC1 )
MESSAGE_DETAIL(SetupMT          , SETUP,              MT_CALL, 2, SetupMO, SetupMTBCRepeat )

/*
 * Retrieve reject message
 */

IE_DETAIL(RetrieveRejCause      , cause_ie,                LV, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(RetrieveRej      , RETRIEVE_REJ,       MT_CALL, 3, SetupMT, RetrieveRejCause )

/*
 * Retrieve ack message
 */

MESSAGE_DETAIL(RetrieveAck      , RETRIEVE_ACK,       MT_CALL, 2, RetrieveRej, END_POINTER )

/*
 * Retrieve message
 */

MESSAGE_DETAIL(Retrieve         , RETRIEVE,           MO_CALL, 2, RetrieveAck, END_POINTER )

/*
 * Release complete message - Mobile originating direction
 */

IE_DETAIL(ReleaseCompMOSSVersion, ss_version_ind_ie,       TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(ReleaseCompMOUserUser , user_user_ie,            TLV, OPTIONAL_IE, ReleaseCompMOSSVersion ) 
IE_DETAIL(ReleaseCompMOFacility , facility_ie,             TLV, OPTIONAL_IE, ReleaseCompMOUserUser )
IE_DETAIL(ReleaseCompMOCause1   , cause_ie,                TLV, OPTIONAL_IE, ReleaseCompMOFacility )
MESSAGE_DETAIL(ReleaseCompMO    , RELEASE_COMPLETE,   MO_CALL, 2, Retrieve, ReleaseCompMOCause1 )

/*
 * Release complete message - Mobile terminating direction
 */

IE_DETAIL(ReleaseCompMTUserUser , user_user_ie,            TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(ReleaseCompMTFacility , facility_ie,             TLV, OPTIONAL_IE, ReleaseCompMTUserUser )
IE_DETAIL(ReleaseCompMTCause1   , cause_ie,                TLV, OPTIONAL_IE, ReleaseCompMTFacility )
MESSAGE_DETAIL(ReleaseCompMT    , RELEASE_COMPLETE,   MT_CALL, 2, ReleaseCompMO, ReleaseCompMTCause1 )

/*
 * Release message - Mobile originating direction
 */

IE_DETAIL(ReleaseMOSSVersion    , ss_version_ind_ie,       TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(ReleaseMOUserUser     , user_user_ie,            TLV, OPTIONAL_IE, ReleaseMOSSVersion ) 
IE_DETAIL(ReleaseMOTFacility    , facility_ie,             TLV, OPTIONAL_IE, ReleaseMOUserUser )
IE_DETAIL(ReleaseMOCause2       , cause_ie,                TLV, OPTIONAL_IE, ReleaseMOTFacility )
IE_DETAIL(ReleaseMOCause1       , cause_ie,                TLV, OPTIONAL_IE, ReleaseMOCause2 )
MESSAGE_DETAIL(ReleaseMO        , RELEASE,            MT_CALL, 2, ReleaseCompMT, ReleaseMOCause1 )

/*
 * Release message - Mobile terminating direction
 */

IE_DETAIL(ReleaseMTUserUser     , user_user_ie,            TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(ReleaseMTFacility     , facility_ie,             TLV, OPTIONAL_IE, ReleaseMTUserUser )
IE_DETAIL(ReleaseMTCause2       , cause_ie,                TLV, OPTIONAL_IE, ReleaseMTFacility )
IE_DETAIL(ReleaseMTCause1       , cause_ie,                TLV, OPTIONAL_IE, ReleaseMTCause2 )
MESSAGE_DETAIL(ReleaseMT        , RELEASE,            MT_CALL, 2, ReleaseMO, ReleaseMTCause1 )

/*
 * Progress message
 */

IE_DETAIL(ProgressUserUser      , user_user_ie,            TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(ProgressIndicator     , progress_indicator_ie,   LV, MANDATORY_IE, ProgressUserUser ) 
MESSAGE_DETAIL(Progress         , PROGRESS,           MT_CALL, 5, ReleaseMT, ProgressIndicator )

/*
 * Notify message
 */
IE_DETAIL(NotifyIndicator       , notification_ind_ie,     V, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(Notify           , NOTIFY,             BI_CALL, 3, Progress, NotifyIndicator )

/*
 * Modify Reject message
 */

IE_DETAIL(ModifyRejHLC          , high_level_compat_ie,           TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL(ModifyRejLLC          , low_layer_compat_ie,            TLV, OPTIONAL_IE, ModifyRejHLC )
IE_DETAIL(ModifyRejCause        , cause_ie,                       LV, MANDATORY_IE, ModifyRejLLC )
IE_DETAIL(ModifyRejBC           , modify_bearer_capability_ie,    LV, MANDATORY_IE, ModifyRejCause )
MESSAGE_DETAIL(ModifyReject     , MODIFY_REJECT,      BI_CALL, 48, Notify, ModifyRejBC )

/*
 * Modify complete message
 */

IE_DETAIL(ModifyCompReverseCall , reverse_call_setup_ie,       T, OPTIONAL_IE, END_POINTER )
IE_DETAIL(ModifyCompHLC         , high_level_compat_ie,        TLV, OPTIONAL_IE, ModifyCompReverseCall )
IE_DETAIL(ModifyCompLLC         , low_layer_compat_ie,         TLV, OPTIONAL_IE, ModifyCompHLC )
IE_DETAIL(ModifyCompBC          , modify_bearer_capability_ie, LV, MANDATORY_IE, ModifyCompLLC )
MESSAGE_DETAIL(ModifyComplete   , MODIFY_COMPLETE,    BI_CALL, 17, ModifyReject, ModifyCompBC )

/*
 * Modify Message
 */

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
IE_DETAIL(ModifyNwInitiated     , nw_initiated_svc_upgrade_ind_ie,    T, OPTIONAL_IE, END_POINTER )
IE_DETAIL(ModifyImmediate       , immediate_mod_ind_ie,    T, OPTIONAL_IE, ModifyNwInitiated )
IE_DETAIL(ModifyReverseCall     , reverse_call_setup_ie,   T, OPTIONAL_IE, ModifyImmediate )
#else
IE_DETAIL(ModifyReverseCall     , reverse_call_setup_ie,   T, OPTIONAL_IE, END_POINTER )
#endif
IE_DETAIL(ModifyHLC             , high_level_compat_ie,    TLV, OPTIONAL_IE, ModifyReverseCall )
IE_DETAIL(ModifyLLC             , low_layer_compat_ie,     TLV, OPTIONAL_IE, ModifyHLC )
IE_DETAIL(ModifyBC              , modify_bearer_capability_ie,    LV, MANDATORY_IE, ModifyLLC )
MESSAGE_DETAIL(Modify           , MODIFY,             BI_CALL, 11, ModifyComplete, ModifyBC )

/*
 * Hold reject message
 */

IE_DETAIL(HoldRejCause          , cause_ie,                LV, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(HoldRej          , HOLD_REJECT,        MT_CALL, 33, Modify, HoldRejCause )

/*
 * Hold ack message
 */

MESSAGE_DETAIL(HoldAck          , HOLD_ACK,           MT_CALL, 2, HoldRej, END_POINTER )

/*
 * Hold  message
 */

MESSAGE_DETAIL(Hold             , HOLD,               MO_CALL, 2, HoldAck, END_POINTER )

/*
 * Facility message - Mobile originating direction
 */

IE_DETAIL(FacilityMOSSVersion   , ss_version_ind_ie,       TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(FacilityMOFacility    , facility_ie,             LV, MANDATORY_IE, FacilityMOSSVersion )
MESSAGE_DETAIL(FacilityMO       , FACILITY,           MO_CALL, 255, Hold, FacilityMOFacility )

/*
 * Facility message - Mobile terminating direction
 */

IE_DETAIL(FacilityMTFacility    , facility_ie,             LV, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(FacilityMT       , FACILITY,           MT_CALL, 255, FacilityMO, FacilityMTFacility )

/*
 * Emergency setup message
 */

IE_DETAIL(EmergencySUStreamId   , stream_identifier_ie,    TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL(EmergencySUBC         , bearer_capability_ie,    TLV, OPTIONAL_IE, EmergencySUStreamId )
MESSAGE_DETAIL(EmergencySetup , EMERGENCY_SETUP,    MO_CALL, 2, FacilityMT, EmergencySUBC )

/*
 * Disconnect message - Mobile originating direction
 */

IE_DETAIL(DisconnMOSSVersion    , ss_version_ind_ie,       TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(DisconnMOUserUser     , user_user_ie,            TLV, OPTIONAL_IE, DisconnMOSSVersion ) 
IE_DETAIL(DisconnMOFacility     , facility_ie,             TLV, OPTIONAL_IE, DisconnMOUserUser )
IE_DETAIL(DisconnMOCause        , cause_ie,                LV, MANDATORY_IE, DisconnMOFacility )
MESSAGE_DETAIL(DisconnMO   , DISCONNECT,        MO_CALL, 33, EmergencySetup, DisconnMOCause )

/*
 * Disconnect message - Mobile terminating direction
 */

#ifdef FEATURE_CCBS
IE_DETAIL(DisconnMTAllowedActions,allowed_actions_ie,       TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(DisconnMTUserUser     , user_user_ie,            TLV, OPTIONAL_IE, DisconnMTAllowedActions )
#else
IE_DETAIL(DisconnMTUserUser     , user_user_ie,            TLV, OPTIONAL_IE, END_POINTER ) 
#endif /* FEATURE_CCBS */
IE_DETAIL(DisconnMTProgressInd  , progress_indicator_ie,   TLV, OPTIONAL_IE, DisconnMTUserUser ) 
IE_DETAIL(DisconnMTFacility     , facility_ie,             TLV, OPTIONAL_IE, DisconnMTProgressInd )
IE_DETAIL(DisconnMTCause        , cause_ie,                LV, MANDATORY_IE, DisconnMTFacility )
MESSAGE_DETAIL(DisconnMT   , DISCONNECT,        MT_CALL, 33, DisconnMO, DisconnMTCause )

/*
 * Connect Ack Message
 */

MESSAGE_DETAIL(ConnectAck  , CONNECT_ACK,       BI_CALL, 2, DisconnMT, END_POINTER )

/*
 * Connect message - Mobile originating
 */

IE_DETAIL(ConnectMOStreamId     , stream_identifier_ie,    TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(ConnectMOSSVersion    , ss_version_ind_ie,       TLV, OPTIONAL_IE, ConnectMOStreamId ) 
IE_DETAIL(ConnectMOConnSubAdd   , connected_subadd_ie,     TLV, OPTIONAL_IE, ConnectMOSSVersion )
IE_DETAIL(ConnectMOUserUser     , user_user_ie,            TLV, OPTIONAL_IE, ConnectMOConnSubAdd )
IE_DETAIL(ConnectMOFacility     , facility_ie,             TLV, OPTIONAL_IE, ConnectMOUserUser )
MESSAGE_DETAIL(ConnectMO        , CONNECT,            MO_CALL, 2, ConnectAck, ConnectMOFacility )

/*
 * Connect message - Mobile terminating
 */

IE_DETAIL(ConnectMTUserUser     , user_user_ie,            TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL(ConnectMTConnSubAdd   , connected_subadd_ie,     TLV, OPTIONAL_IE, ConnectMTUserUser )
IE_DETAIL(ConnectMTConnNo       , connected_no_ie,         TLV, OPTIONAL_IE, ConnectMTConnSubAdd )
IE_DETAIL(ConnectMTProgress     , progress_indicator_ie,   TLV, OPTIONAL_IE, ConnectMTConnNo )
IE_DETAIL(ConnectMTFacility     , facility_ie,             TLV, OPTIONAL_IE, ConnectMTProgress )
MESSAGE_DETAIL(ConnectMT        , CONNECT,            MT_CALL, 2, ConnectMO, ConnectMTFacility )

/*
 * Congestion control message
 */ 

IE_DETAIL(CongestContCause      , cause_ie,                TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL(CongestContLevel      , congestion_level_ie,     V, MANDATORY_IE, CongestContCause )
MESSAGE_DETAIL(CongestCont    , CONGESTION_CONTROL, BI_CALL, 3, ConnectMT, CongestContLevel )

/*
 * Call Proceeding message
 */ 
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
IE_DETAIL(CallProcCCCapability  , network_cc_capability_ie,TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL(CallProcProgInd       , progress_indicator_ie,   TLV, OPTIONAL_IE, CallProcCCCapability )
#else
IE_DETAIL(CallProcProgInd       , progress_indicator_ie,   TLV, OPTIONAL_IE, END_POINTER)
#endif
IE_DETAIL(CallProcFacility      , facility_ie,             TLV, OPTIONAL_IE, CallProcProgInd )
IE_DETAIL(CallProcBC2           , bearer_capability_ie,    TLV, OPTIONAL_IE, CallProcFacility )
IE_DETAIL(CallProcBC1           , bearer_capability_ie,    TLV, OPTIONAL_IE, CallProcBC2 )
IE_DETAIL(CallProcRepeat        , repeat_ind_ie,           TV, CONDITIONAL_IE, CallProcBC1 )
MESSAGE_DETAIL(CallProc         , CALL_PROCEEDING,    MT_CALL, 2, CongestCont, CallProcRepeat )

/*
 * Call Confirmed message
 */
IE_DETAIL(CallConfStreamId      , stream_identifier_ie,    TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL(CallConfCCCapab       , call_control_cap_ie,     TLV, OPTIONAL_IE, CallConfStreamId )
IE_DETAIL(CallConfCause         , cause_ie,                TLV, OPTIONAL_IE, CallConfCCCapab )
IE_DETAIL(CallConfBC2           , bearer_capability_ie,    TLV, OPTIONAL_IE, CallConfCause )
IE_DETAIL(CallConfBC1           , bearer_capability_ie,    TLV, OPTIONAL_IE, CallConfBC2 )
IE_DETAIL(CallConfRepeat        , repeat_ind_ie,           TV,  CONDITIONAL_IE, CallConfBC1 )
MESSAGE_DETAIL(CallConf         , CALL_CONFIRMED,     MO_CALL, 2, CallProc,    CallConfRepeat )

/*
 * Alert message - Mobile originating direction
 */

IE_DETAIL(AlertMOSSVersion      , ss_version_ind_ie,       TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(AlertMOUserUser       , user_user_ie,            TLV, OPTIONAL_IE, AlertMOSSVersion ) 
IE_DETAIL(AlertMOFacility       , facility_ie,             TLV, OPTIONAL_IE, AlertMOUserUser )
MESSAGE_DETAIL(AlertingMO       , ALERTING,           MO_CALL, 2, CallConf,    AlertMOFacility )

#ifdef FEATURE_CCBS
/*
 * CC_ESTABLISHMENT message
 */

IE_DETAIL(CcEstSetupCont        , setup_container_ie,      LV, MANDATORY_IE,    END_POINTER ) 
MESSAGE_DETAIL(CcEst            , CC_ESTABLISHMENT,   MT_CALL, 255, AlertingMO, CcEstSetupCont )

/*
 * CC_ESTABLISHMENT_CONFIRMED message
 */

IE_DETAIL(CcEstConfCause        , cause_ie,                   TLV, OPTIONAL_IE,    END_POINTER )
IE_DETAIL(CcEstConfBC2          , bearer_capability_ie,       TLV, OPTIONAL_IE,    CcEstConfCause )
IE_DETAIL(CcEstConfBC1          , bearer_capability_ie,       TLV, MANDATORY_IE,   CcEstConfBC2 )
IE_DETAIL(CcEstConfBCRepeat     , repeat_ind_ie,               TV, CONDITIONAL_IE, CcEstConfBC1 )
MESSAGE_DETAIL(CcEstConf        , CC_ESTABLISHMENT_CONFIRMED, MO_CALL,5, CcEst,    CcEstConfBCRepeat )

/*
 * RECALL message
 */

IE_DETAIL(RecallFacility        , facility_ie,              LV, MANDATORY_IE, END_POINTER ) 
IE_DETAIL(RecallRecallType      , recall_type_ie,            V, MANDATORY_IE, RecallFacility ) 
MESSAGE_DETAIL(Recall           , RECALL,              MT_CALL, 255, CcEstConf,  RecallRecallType )

/*
 * START_CC message
 */

IE_DETAIL(StartCcCcCapa         , call_control_cap_ie,     TLV, OPTIONAL_IE,  END_POINTER ) 
MESSAGE_DETAIL(StartCc          , START_CC,            MO_CALL, 2, Recall,    StartCcCcCapa )
#endif /* FEATURE_CCBS */

/*
 * Alert message - Mobile terminating direction
 */

IE_DETAIL(AlertMTUserUser       , user_user_ie,            TLV, OPTIONAL_IE, END_POINTER ) 
IE_DETAIL(AlertMTProgressInd    , progress_indicator_ie,   TLV, OPTIONAL_IE, AlertMTUserUser ) 
IE_DETAIL(AlertMTFacility       , facility_ie,             TLV, OPTIONAL_IE, AlertMTProgressInd )
#ifdef FEATURE_CCBS
MESSAGE_DETAIL(AlertingMT       , ALERTING,           MT_CALL, 2, StartCc,       AlertMTFacility )
#else
MESSAGE_DETAIL(AlertingMT       , ALERTING,           MT_CALL, 2, AlertingMO,    AlertMTFacility )
#endif /* FEATURE_CCBS */

ROOT_MESSAGE(AlertingMT)



/**************** Begin of   CNM_setup_IE_data_read *********************************
 *
 * Function: CNM_setup_IE_data_read()
 *
 * Description:  Function to return a pointer to the message details entry 
 *               for the message defined by message_type and for the 
 *               direction given by Direction.
 *
 * Input:        Message type
 *               Direction
 *
 * Output:       Pointer to Message Detail entry
 */

struct MsgDetailTag   * CNM_setup_IE_data_read(message_type_T message_type,
                                              byte    Direction)
{
   struct MsgDetailTag   * Ptr;
   boolean Found=FALSE;

   Ptr = CNM_RootName;      /* point at start of linked list */

   /*
    * Scan down the linked list
    */

   while( Ptr != &EndMessagesLoc)
   {
 
      /*
       * if match on message type
       */

      if ( Ptr->message_type == message_type )
      {

         /*
          * if direction doesnt matter or if match on direction
          */

         if ((Ptr->Direction == BI_CALL) || (Ptr->Direction == Direction) )
         {

            /*
             * Check to see if there are any IEs associated with 
             * this message
             */

            Found = TRUE;
            break;
         }
      }
      Ptr = Ptr->NextMsgDetail;
   }
   if (Found == FALSE)
   {
      Ptr = 0;
   }
   return (Ptr);
}
/**************** End of   CNM_setup_IE_data_read **********************************/


/**************** Begin of   CNM_get_next_IE_allowed ******************************
 * 
 * Function: CNM_get_next_IE_allowed()
 *
 * Description: Function to return details of successive information 
 *              elements allowed to be in a message. CNM_setup_IE_data_read() 
 *              must be called first to identify the message of interest, 
 *              and to return a pointer to the database. GET_FIRST_REF() 
 *              must then be called to return a pointer to details of the 
 *              first IE of the message or NULL if there are none. Then this 
 *              function can be called, with the ref value obtained above. 
 *              The function returns the IEI, type, max length, and presence 
 *              flag for the next IE, and updates the reference so that 
 *              further calls to this function reveal the following IEs 
 *              allowed. When there are no more, the function returns
 *              FALSE.
 *
 * Input:       Reference to IE data entry
 *
 * Output:      IEI
 *              Type
 *              Length
 *              Presence
 *              Valid - indicates if above data is meaningful, or if there 
 *                      are no more IEs
 *              
 */ 

boolean CNM_get_next_IE_allowed(byte *IEI, 
                               byte *Type, 
                               byte *Length,
                               byte *Presence, 
                               byte *Format,
                               struct MessageIETag** NextRef)

{
   if (*NextRef == &EndLoc)
   {
      return(FALSE);
   }
   else
   {
      *IEI =  (*NextRef)->IEType->IEI;
      *Type =(*NextRef)->IEType->Type; 
      *Length = (*NextRef)->IEType->Length;
      *Format =  (*NextRef)->Format;
      *Presence = (*NextRef)->Mandatory;
      *NextRef = (*NextRef)->NextMessageIE;    /* set up to point to 
                                                  next entry */
      return(TRUE);
   }
}
/**************** End of   CNM_get_next_IE_allowed **********************************/


/**************** Begin of   CNM_identify_IE **************************************
 *
 *  Function name: CNM_identify_IE()
 *  -------------------------------
 *  Description:
 *  ------------
 *  Function to identify the information element pointed to by pIE in 
 *  message pointed to by pMessage. The IEI is returned, together with 
 *  the length of this IE. Note that not all IEs have an IEI in them in 
 *  the message. These oddities will occur within the mandatory portion 
 *  of the message, so we need to use the Message details information 
 *  to identify these IEs.
 * 
 *
 *  Parameters:
 *  -----------
 *  p_IE : IN - Pointer to information element
 *  p_message : IN - Pointer to message containing IE
 *  p_length : IN - Pointer to location where length is to be stored
 *
 *  Returns:
 *  --------
 *  IE : Information element identification
 *
 ******************************************************************************/

byte CNM_identify_IE(VOID_DATA * p_IE, IMH_T * p_message, byte * p_length )
{
   byte                 message_type;
   struct MsgDetailTag  *p_message_detail;
   struct MessageIETag  *next_ref;
   int                  offset = 0;
   VOID_DATA            *ptr;       /* general pointer */
   byte                 allowed_IEI;
   byte                 type;
   byte                 IE_length;
   byte                 presence;
   byte                 format;
   boolean              valid;
   byte                 return_value;

   return_value = 0xff; /* Initialize to unknown/invalid IEI  */


   message_type = ((msg_head_T *)((void *)p_message))->type;

   /* 
    * get pointer to message details entry 
    */

   p_message_detail = CNM_setup_IE_data_read(message_type, MT_CALL);

   /* 
    * use defined macro to get magic pointer to entry in IE database, which
    * is required for CNM_get_next_IE_allowed()
    */
   if (p_message_detail != (struct MsgDetailTag *) 0 )
   {
         
      /* 
       * calculate offset into L3 mesage to IE 
       */
      next_ref = GET_FIRST_REF(p_message_detail);
      offset = (int)((byte *)(p_IE) - (byte *)(p_message)) - (int)(sizeof(IMH_T));

      if ( offset < p_message_detail->ImperativeLength )
      {         
         /* 
          * IE is within imperative part 
          * Set up ptr to point to first IE
          */

         ptr = p_message + sizeof(msg_head_T);
    
         valid = TRUE;

         while ( valid != FALSE )
         {

            /*
             * get next possible IE (which should be mandatory) 
             */

            valid = CNM_get_next_IE_allowed( & allowed_IEI, 
                                            & type, 
                                            & IE_length, 
                                            & presence,
                                            & format,
                                            & next_ref);

            if ( valid != FALSE )
            { 
               if ((type == 1) || (type == 2))     /* Must be 1 byte log */
               {
                  *p_length = 1;
                  return_value = allowed_IEI;
               }
               else
               if (type == 3)    /* get length from message details database */
               {
                  *p_length = IE_length;

                  return_value = allowed_IEI;
               }
               else
               if ( type == 4)     /* IE is either TLV or LV */
               {
                  if ( format == FORMAT_TLV )
                  {
                     /*
                      * length is taken from 2nd byte ie IE, but this doesnt
                      * include the IEI and length fields, so add 2
                      */

                     *p_length = (unsigned char)(*(((byte *)(p_IE)) + 1) + 2);
                  }
                  else
                  {                 /* must be LV */
                     /*
                      * length is taken from 1st byte ie IE, as no IEI. Again,
                      * this does not include length field, so add 1
                      */

                     *p_length = (unsigned char)(*((byte *)(p_IE)) + 1);
                  }
                  return_value = allowed_IEI;
               }
               else
               {
                  GS_SET_UNDEFINED(return_value);

                  *p_length = 0;

                  MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Type 5 IE not expected, received type = %d\n",
                           (dword) type);
                  break;
               }

               /* 
                * Check to see if we have reached the IE we are looking for 
                */

               if ( ptr == p_IE)       
               {
                  break;
               }
               else
               {

                  /* 
                   * Skip this IE 
                   */

                  ptr = (VOID_DATA *)(((byte *)(ptr)) + *p_length); 
               }
            }
            else
            {
               MSG_ERROR_DS_0(MN_SUB,"=MNCNM= IE not found \n");

               GS_SET_UNDEFINED(return_value);
               *p_length = 0;
               break;
            }
         }
      }
      else  /* not in the mandatory part */
      {
         if ( IS_IEI_REPEAT_INDICATOR(*(byte *)(p_IE)))
         {
            *p_length = 1;
            return_value = IEI_REPEAT_INDICATOR;
         }
         else
         if ( IS_IEI_CONGESTION_CONTROL(*(byte *)(p_IE)))
         {
            *p_length = 1;
            return_value = IEI_CONGESTION_CONTROL;
         }
         else
         {
            if ( (*((byte *)(p_IE)) & 0x80 ) != 0 )  /* its a type 1 or 2 IE */
            {
               *p_length = 1;
            }
            else        /* deal with special case type 3 and 4 IEs */
            {
               if ( *(byte *)(p_IE) == IEI_SIGNAL )
               {
                  *p_length = 2;
               }
               else if (*(byte *)(p_IE) == IEI_NOTIFICATION_IND )
               {
                  *p_length = 1;
               }
               else if (*(byte *)(p_IE) == IEI_KEYPAD_FACILITY )
               {
                  *p_length = 1;
               }
               else if (*(byte *)(p_IE) == IEI_CALL_STATE )
               {
                  *p_length = 1;
               }
               else
               {
                  
                  /*
                   * next byte is length not including IEI and itself 
                   */

                  *p_length = (unsigned char)(*(((byte *)(p_IE))+1) + 2);
               }
            }
               
            return_value = *(byte *)(p_IE);
         }
      }
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Internal Error in CNM_identify_IE \n");

      GS_SET_UNDEFINED(return_value);
   }

   return return_value;
}
/**************** End of   CNM_identify_IE ***************************************/


/**************** Begin of CNM_check_IE_syntax ************************************
 *
 *  Function name: CNM_check_IE_syntax()
 *  -----------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks the syntax of an information element. The input 
 *  indicates the type of IE, and points to the IE to check. The check is 
 *  for invalid values including the use of reserved values
 *
 *  There are 5 anomolous information elements, which do not have an IEI.,
 *  These are:
 *  Notification Indicator
 *  Call State
 *  Mobile station classmark 1
 *  Mobile station classmark 2
 *  Cipher Key sequence number
 *  congestion level
 
 *  Of these, only Notification indicator, Call state, and congestion
 *  level affect CM

 *  It is up to the calling functions to detect these and use identifying
 *  IEIs.
 *
 *
 *  Parameters:
 *  -----------
 *  IE : INPUT - pointer to information element to check
 *  IEI : INPUT - type of IE
 *
 *  Returns:
 *  --------
 *  syntax_ok - indication if check passes of fails
 *
 ******************************************************************************/

boolean CNM_check_IE_syntax( byte IEI,  byte *IE,byte IE_count ) 
{

   boolean        return_value;
   byte           length;
   byte           ByteRef;
   byte           octet_number;     /* as in 04.08 section 10  */
   byte           sub_octet_number; /* within an octet group   */
   byte           state;
   byte           bit_8_count;
   word           IEI_copy;
   boolean        infinite_loop = TRUE; /* A dummy variable to by-pass a 
                                             compiler warning */

   return_value = TRUE;          /* default return indication */

   /*
    * Workaround for c166 compiler bug, which would otherwise produce
    * "S 524: (line 426) assertion failed - please report"
    * when compiled with makefile
    *
    * Only works if IEI_copy is a different size to IEI 
    */

   IEI_copy = IEI;

   /* First check for type 1 and 2 IEIs, which share the IEI with data */

   if ( IS_IEI_REPEAT_INDICATOR(IEI) )
   {
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      if ( (*IE != IEI_REPEAT_INDICATOR) &&
           (*IE != (IEI_REPEAT_INDICATOR|REPEAT_CIRCULAR)) &&
           (*IE != (IEI_REPEAT_INDICATOR|REPEAT_SUPPORTS_FALLBACK)) &&
           (*IE != (IEI_REPEAT_INDICATOR|REPEAT_SERVICE_CHANGE_FALLBACK)) )
      {
         return_value = FALSE;
      }
#else
      if ( ( (*IE | 0xf2) ^ 0x0c) != 0xff )   /* check bits 4,3 =0, bit 1=1 */
      { 
         return_value = FALSE;
      }
#endif  /* FEATURE_MULTIMEDIA_FALLBACK */
   }

#ifdef FEATURE_UUS
   else if ( IS_IEI_CONGESTION_CONTROL(IEI) )
   {
      if ( !(( (*IE & 0x0f ) !=  0) || ( (*IE & 0x0f ) != 0x0f)) )
      {
         return_value = FALSE;
      }
   }
#endif
   else
   {
      switch (IEI)
      {
         case IEI_NOTIFICATION_IND:
            if ( (IE[1] & 0x80) == 0 ) /* This is NOT 0/1 Ext bit */
            {
               return_value = FALSE;
            }

            if ((IE[1] & 0x7f)  >= 3)
            {
               return_value = FALSE;
            }
            break;

         case IEI_LAI:

#ifndef PCS_1900

            if ( ( IE[2] | 0x0f ) != 0xff )
            {
               return_value = FALSE;
            }
#endif
            break;


         case IEI_MORE_DATA:
            /* no checking */
            break;

         case IEI_CLIR_SUPPRESSION:
            /* no checking */
            break;

         case IEI_CLIR_INVOCATION:
            /* no checking */
            break;

         case IEI_REVERSE_CALL_SETUP_DIR:
            /* no checking */
            break;

         case IEI_IMMEDIDATE_MOD_IND:
            /* no checking */
            break;

         case IEI_NW_INITIATED_SVC_UPGRADE_IND:
            /* no checking */
            break;

         case IEI_BEARER_CAPABILITY:
         case IEI_BACKUP_BEARER_CAPABILITY:
 /****************************************************************************************** 
 *From sections 10.5.4.4a and 10.5.4.5 of 24.008,MAX lengths of IEI_BACKUP_BEARER_CAPABILITY IE and 
 * IEI_BEARER_CAPABILITY IE are 15 bytes.and 16 bytes, respectively.So its value parts are of max 
 * 13 bytes. and 14 bytes respectively. From section 11.4.2 of 24.007 network can send more than 
 *13 and 14 octets bytes of value part of these IEs .If Network sends the value part more than 13 and 14 
 *bytes then,take only 13 and 14 bytes from the network given value, since value part of these IEs can have 
 *MAX of 13 and 14 bytes only , respectively for these IEs  as per 24.008
 *******************************************************************************************/
            if(IEI == IEI_BEARER_CAPABILITY)
            {
              if(IE[1] > 14 )
              {
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= IEI_BEARER_CAPABILITY  Length %d is greater than 14",IE[1]);
                IE[1] = 14;
              }
            }
            else
            {
              if(IE[1] > 13 )
              {
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= IEI_BACKUP_BEARER_CAPABILITY Length %d is greater than 13",IE[1]);
                IE[1] = 13;
              }
            }

            length = (unsigned char)(IE[1] + 2);

            ByteRef = 2;   /* This is a reference to the actual byte in 
                              the message */

            /* 
             * for all possible bytes in the information element 
             * - Note some of these may not be present - bit 8 is 0 in 
             *   octets 6, 6a, and 6b to indicate that 
             * octets 6a, 6b, and 6c repestivey are present 
             * NOTE: Octet numbers start at 1, to match 04.08 10.5.4.5
             */

            sub_octet_number = 0;
            octet_number = 3;    /* start from byte after length field */

            /*CONSTCOND*/
            while (infinite_loop)
            {

               /*
                * switch value is 2 digits, the first is the octet number,
                * as per 04.08 setion 10, and the second digit equates to
                * 0 for octet N, 1 for octet Na, 1 for Nb etc 
                * Any new octets introduced later by the GSM chaps should
                * be ignored
                *
                */

               switch ( octet_number * 10 + sub_octet_number )  
               {
                  case 30:                             /* octet 3 */
                     /* Checking for invalid/reserved values of ITC,
                        Transfer mode, Coding std, & Radio chn requirement. 
                      */
                     if (  
                          ( ( IE[ByteRef] & 0x10 ) == 0x10 ) ||
                          ( 
                            ( ( ( IE[ByteRef] & 0x04 ) != 0 ) && 
                              ( ( IE[ByteRef] & 0x03 ) != 0x03 ) && ( ( IE[ByteRef] & 0x03 ) != 0x02 )
  /*
                              ( ( IE[ByteRef] & 0x03 ) != 0x03 )
                              
 ITC  for the AUX_SPEECH can also be 6 (new value). This will be made NV Controllable to be configured to 6 or 4.
                             
   */
                            )
                          )
                        )
                     {
                        /* Notes: ITC Value of Other ITC is not yet supported.
                                  Coding Std must be GSM standardized coding.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 40:                             /* octet 4 */
                     /* Checking for invalid/reserved values of Establi,
                        NIRR, Config., Dupl. mode, Struct., & Compress. 
                      */
                     if ( 
                          ( ( IE[ByteRef] & 0x04 ) != 0 ) ||
                          ( ( IE[ByteRef] & 0x01 ) != 0 ) 
                        )
                     {
                        /* Notes: Config. value must be 0.
                                  Establi. value must be 0.
                         */
                        return_value = FALSE;
                     }                   

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;
    
                  case 50:                             /* octet 5 */
                     /* Checking for invalid/reserved values of Signaling
                        access protocol, Rate adaption, & Access id. 
                      */
                     if ( 
                          ( ( IE[ByteRef] & 0x40 ) != 0 ) ||
                          ( ( IE[ByteRef] & 0x20 ) != 0 ) ||
                         /*  ( ( IE[ByteRef] & 0x18 ) == 0x18 ) || */  /* support Multimedia Other Rate Adaption */
                          ( ( IE[ByteRef] & 0x07 ) == 0 ) ||
                          ( ( IE[ByteRef] & 0x07 ) == 0x07 )
                        )
                     {
                        /* Notes: Access id value must be 0.
                                  Rate adaption value of Other rate adaption 
                                    is not yet supported.
                                  Signalling access protocol values of 0 & 7
                                    are reserved.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 51:                             /* octet 5a */
                     /* Checking for invalid/reserved values of Signaling
                        access protocol, Rate adaption, & Access id. 
                      */
                     if ( 
                           (( IE[ByteRef] & 0x60 ) != 0  ) ||  /* other_ITC */
                           (( IE[ByteRef] & 0x18 ) == 0x18 ) 
                        )
                     {
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 60:                             /* octet 6 */
                     /* Checking for invalid/reserved values of Sync,
                        User info layer 1 protocol, & Layer 1 id. 
                      */
                     if ( ( ( IE[ByteRef] & 0x60 ) != 0x20 ) ||
                          ( ( IE[ByteRef] & 0x1e ) != 0 )
                        )
                     {
                        /* Notes: Layer 1 id value must be 1.
                                  User info layer 1 protocol value must be 0.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 61:                             /* known as octet 6a */
                     /* Checking for invalid/reserved values of User rate,
                        Numb. data bits, Negotiation, & Numb. stop bits. 
                      */
                     if ( ( ( IE[ByteRef] & 0x20 ) != 0 ) ||
                          ( ( IE[ByteRef] & 0x0f ) == 0 ) ||
                          ( ( IE[ByteRef] & 0x08 ) != 0 )
                        )
                     {
                        /* Notes: Negotiation value must be 0.
                                  User rate value must neither be 0 nor be
                                    greater than 7.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                  break;

                  case 62:                             /* known as octet 6b */
                     /* Checking for invalid/reserved values of Parity,
                        NIC on RX, NIC on TX, & Intermed. rate. 
                      */
                     if ( ( ( IE[ByteRef] & 0x40 ) == 0 ) ||
                          ( ( IE[ByteRef] & 0x06 ) == 0x06 ) ||
                          ( 
                            ( ( IE[ByteRef] & 0x06 ) == 0 ) &&
                            ( ( IE[ByteRef] & 0x01 ) != 0 )
                          )
                        )
                     {
                        /* Notes: Itermed. rate value must be greater than 1.
                                  Parity value must not be greater than 5.
                                  Parity value must not be 1.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;
                  case 63:                             /* known as octet 6c */
                     /* Checking for invalid/reserved values of Modem type,
                        & Connection element. 
                      */
                     if ( ( ( IE[ByteRef] & 0x10 ) != 0 ) ||
                          ( 
                            ( ( IE[ByteRef] & 0x08 ) != 0 ) &&
                            ( ( IE[ByteRef] & 0x07 ) != 0 )
                          )
                        )
                     {
                        /* Notes: Modem type value must not be greater than 8.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 64:                             /* known as octet 6d */
                     /* Checking for invalid/reserved values of FNUR,
                        & Other modem type. 
                      */
                     if ( ( 
                            ( ( IE[ByteRef] & 0x40 ) != 0 ) &&
                            ( ( IE[ByteRef] & 0x20 ) != 0 )
                          ) ||
                          ( ( IE[ByteRef] & 0x1f ) > 0x0C )
                          
                        )
                     {
                        /* Notes: Other modem type value must not be 3.
                                  FNUR value must not be greater than 12
                                    where, 12 is defined as FNUR_57600, 
                                    in addition to the values specified in
                                    TS 24.008 V3.6.0
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 65:                             /* known as octet 6e */
                     /* Checking for invalid/reserved values of Max. 
                        num. of traffic chns., & Acceptable channel codings. 
                      */
                     /* Notes: Both the above params in octet 6e, in the Network
                                 to MS direction are spare. So, nothing to check.
                      */

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  case 70:                            /* known as octet 7 */
                     /* Checking for invalid/reserved values of User info
                        layer 2 protocol, & Layer 2 id. 
                      */
                     if ( 
                          ( ( IE[ByteRef] & 0x40 ) == 0 ) ||
                          ( ( IE[ByteRef] & 0x20 ) != 0 ) ||
                          ( ( IE[ByteRef] & 0x10 ) != 0 ) ||
                          ( (byte)( IE[ByteRef] & 0x0f ) <= 5 ) ||
                          ( (byte)( IE[ByteRef] & 0x0f ) > 13 ) ||
                          ( (byte)( IE[ByteRef] & 0x0f ) == 7 ) ||
                          ( (byte)( IE[ByteRef] & 0x0f ) == 11 )
                        )
                     { 
                        /* Notes: Layer 2 id value must be 2.
                                  User info layer 2 protocol value must 
                                    be greater than 5 & not be greater than 13.
                                  User info layer 2 protocol value must neither
                                    be 7 nor be 11.
                         */
                        return_value = FALSE;
                     }

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;

                  default:  /* unknown extension in octet group - ignore */

                     if ( ( IE[ByteRef] & 0x80 ) == 0 )
                     {
                        sub_octet_number++;     /* not yet end of group */
                     }
                     else
                     {
                        octet_number++;
                        sub_octet_number = 0;
                     }
                     break;
               }
               ByteRef++;     /* point at next real octet of data */

               /* 
                *if no more entries in IE to process 
                */

               if (( ByteRef >= length ) ||
                   ( octet_number >= 8 ))
               {
                  break;
               }

            }   /* End loop */

            break;

         case IEI_CAUSE:
            if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
            {
                 /********************************************************************************** 
                 * if the received size of the Cause IE is > GSM_CAUSE_SIZE, truncate it to GSM_CAUSE_SIZE. Cause IE is a 
                 * Type 4 IE whose max size is GSM_CAUSE_SIZE from Spec TS 24.080 Section 10.5.4.11.
                 **********************************************************************************/
                 
              if(cc_IE_info[IE_count].length > GSM_CAUSE_SIZE )
              {
                MSG_ERROR_DS_2(MN_SUB,"=MNCNM= IEI_Cause:Oversized Cause IE  size = %d,truncating it to %d",
                                                      cc_IE_info[IE_count].length, GSM_CAUSE_SIZE);
                cc_IE_info[IE_count].length = GSM_CAUSE_SIZE;
                       
                if(*(cc_IE_info[IE_count].start + 1) > GSM_CAUSE_SIZE - 2)
                {
                  *(cc_IE_info[IE_count].start + 1) = GSM_CAUSE_SIZE - 2;
                }
              }
            }

            if ( IE[1] < 2 )
            {
               return_value = FALSE;
            }
            else
            {
               if ( ( (byte)( IE[2] & 0x0f ) == 6 ) ||
                    ( (byte)( IE[2] & 0x0f ) == 8 ) ||
                    ( (byte)( IE[2] & 0x0f ) == 9 ) ||
                    ( (byte)( IE[2] & 0x0f ) >= 11 ) )
               {
                  return_value = FALSE;
               }
            }

            /*
             * Check that there are at least 2 bytes with the 0/1 Ext bit
             * set to 1
             */

            bit_8_count = 0;

            for ( ByteRef = 2; 
                  (ByteRef < IE[1]+2) && (bit_8_count < 2); 
                  ByteRef++ )
            {
               if ( IE[ByteRef] & 0x80 )
               {
                  bit_8_count++;
               }
            }

            if (bit_8_count != 2)
            {
               return_value = FALSE;
            }

            break;

         case IEI_FACILITY:
            /* no checking */;
            break;

         case IEI_PROGRESS_INDICATOR:


            if ( IE[1] < (IEI_PROGRESS_INDICATOR_LENGTH - 2) ) // length is always 2
            {
               return_value = FALSE;
            }
            else 
            {

              if( IE[1] > (IEI_PROGRESS_INDICATOR_LENGTH - 2) )
              {
                MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Truncating IEI progress length to %d",(IEI_PROGRESS_INDICATOR_LENGTH - 2));
                IE[1] = (IEI_PROGRESS_INDICATOR_LENGTH - 2);
              }

              if ( ( (byte)( IE[2] & 0x0f ) == 3 ) ||
                   ( ( (byte)( IE[2] & 0x0f ) >= 6 ) &&
                     ( (byte)( IE[2] & 0x0f ) <= 9 )
                   ) ||
                   ( (byte)( IE[2] & 0x0f ) >= 11 )
                 )
              {
                 return_value = FALSE;
              }

            }

            /*   
             * Check that there are at least 2 bytes with the 0/1 Ext bit
             * set to 1
             */    
 
            bit_8_count = 0;
 
            for ( ByteRef = 2;
                  (ByteRef < IE[1]+2) && (bit_8_count < 2);
                  ByteRef++ )
            {
               if ( IE[ByteRef] & 0x80 )
               { 
                  bit_8_count++;
               } 
            }   
 
            if (bit_8_count != 2)
            {
               return_value = FALSE;
            }

            break;

         case IEI_AUXILIARY_STATES:
            
             
            if ( IE[1] != (IEI_AUXILIARY_STATES_LENGTH - 2) ) // length is always 1
            {
               return_value = FALSE;
            }
            if(IE[1] > (IEI_AUXILIARY_STATES_LENGTH - 2))
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Truncating IEI_AUXILIARY_STATES length to %d",(IEI_AUXILIARY_STATES_LENGTH - 2));            
              IE[1] = (IEI_AUXILIARY_STATES_LENGTH -2);
            }

            /*
             * Check that there are at least 1 byte with the 0/1 Ext bit
             * set to 1
             */

            bit_8_count = 0;
 
            for ( ByteRef = 2;
                  (ByteRef < IE[1]+2) && (bit_8_count < 1);
                  ByteRef++ )
            {
               if ( IE[ByteRef] & 0x80 )
               { 
                  bit_8_count++;
               } 
            }   

            if (bit_8_count != 1)
            {
               return_value = FALSE;
            }


            break;

         case IEI_KEYPAD_FACILITY:

            /* 
             * NOTE: this checks a spare bit, not 0/1 Ext 
             */

            if ( ( IE[1] & 0x80 ) != 0 )  
            {
               return_value = FALSE;
            }
            break;

         case IEI_SIGNAL:
            if ( ( IE[1] > 8 ) &&
                 ( IE[1] != 0x3f ) &&
                 ( IE[1] != 0x4f )
               )
            {
               return_value = FALSE;
            }
            break;

         case IEI_CONNECTED_NUMBER:
 /********************************************************************************** 
 * if the received size of the IEI_CONNECTED_NUMBER is < 1 octets, treat it as syntax error. IEI_CONNECTED_NUMBER
 * is a Type 4 IE whose MIN length is 3 octets from Spec TS 24.080 Section 10.5.4.13.
 **********************************************************************************/

             if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
            {
               /********************************************************************************** 
               * if the received size of the IEI_CONNECTED_NUMBER is > 14 octets, truncate it to 14. IEI_CONNECTED_NUMBER
               * is a Type 4 IE whose max size is 14 from Spec TS 24.080 Section 10.5.4.13.
               **********************************************************************************/
               if(cc_IE_info[IE_count].length > IEI_CONNECTED_NUMBER_MAX_LENGTH)
               {
                 cc_IE_info[IE_count].length = IEI_CONNECTED_NUMBER_MAX_LENGTH;
                 if(*(cc_IE_info[IE_count].start + 1) > cc_IE_info[IE_count].length - 2)
                 {
                   *(cc_IE_info[IE_count].start + 1) = cc_IE_info[IE_count].length - 2;
                 }
               }

            }

            if ( IE[1] < 1 )
            {
              return_value = FALSE;
            }
            else
            {
              if ( (((byte)( IE[2] & 0x70 )>>4) >= 5 ) ||
                   ( (byte)( IE[2] & 0x0f ) == 2 ) ||
                   ( ( (byte)( IE[2] & 0x0f ) >= 5 ) &&
                     ( (byte)( IE[2] & 0x0f ) <= 7 )
                   ) ||
                   ( (byte)( IE[2] & 0x0f ) >= 10 )
                 )
              {
               return_value = FALSE;
              }
              if ( ( IE[2] & 0x80 ) != 0 )                 /* octet 3a follows */
              {

                 /* 
                  * looking at octet 3a 
                  */

                 if ( ( IE[2] & 0x60 ) == 0x60 )
                 {
                    return_value = FALSE;
                 }
              }
            }


            break;

         case IEI_CONNECTED_SUBADDRESS:

            if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
            {
               /********************************************************************************** 
                 * if the received size of the IEI_CONNECTED_SUBADDRESS is > 23 octets, truncate it to 23. 
                 * IEI_CONNECTED_SUBADDRESS is a Type 4 IE whose max size is 23 from Spec TS 24.080 Section 10.5.4.14.
                 **********************************************************************************/
                 {
                   if(cc_IE_info[IE_count].length > IEI_CONNECTED_SUBADDRESS_MAX_LENGTH)
                   {
                     cc_IE_info[IE_count].length = IEI_CONNECTED_SUBADDRESS_MAX_LENGTH;
                     if(*(cc_IE_info[IE_count].start + 1) > cc_IE_info[IE_count].length - 2)
                     {
                       *(cc_IE_info[IE_count].start + 1) = cc_IE_info[IE_count].length - 2;
                     }
                   }
                 }
            }

            if ( IE[1] < 1 )
            {
              return_value = FALSE;
            }
            else
            {
              if ( ( IE[2] & 0x50 ) != 0 )
              {
                 return_value = FALSE;
              }
            }

            break;

         case IEI_REDIR_PARTY_BCD_NO:
         case IEI_CALLING_PARTY_BCD_NO:
         case IEI_CALLED_PARTY_BCD_NO:             /* same checks */

            if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
            {
              if(IEI_copy == IEI_CALLING_PARTY_BCD_NO)
              {
                 if( *(cc_IE_info[IE_count].start + 1) > CC_MAX_CALLING_PTY_BCD_NUM_LENGTH)
                   {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized IEI_CALLING_PARTY_BCD_NO IEsize = %d,truncating to %d", 
                                               *(cc_IE_info[IE_count].start + 1),CC_MAX_CALLING_PTY_BCD_NUM_LENGTH + 2);
                     *(cc_IE_info[IE_count].start + 1) = CC_MAX_CALLING_PTY_BCD_NUM_LENGTH;
                     cc_IE_info[IE_count].length  = CC_MAX_CALLING_PTY_BCD_NUM_LENGTH + 2; /* T + L + V (12 octets) */
                     /* IE_length value in cc_IE_info[] is always p_IE[1] + 2 ; after parsing */
                   }
              }
              else if(IEI_copy == IEI_CALLED_PARTY_BCD_NO)
              {
                   if( *(cc_IE_info[IE_count].start + 1) > CC_MAX_CALLED_PTY_BCD_NUM_LENGTH)
                   {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized IEI_CALLED_PARTY_BCD_NO IEsize = %d,truncating to %d", 
                                            *(cc_IE_info[IE_count].start + 1), (CC_MAX_CALLED_PTY_BCD_NUM_LENGTH + 2));
                     *(cc_IE_info[IE_count].start + 1) = CC_MAX_CALLED_PTY_BCD_NUM_LENGTH;
                     cc_IE_info[IE_count].length = CC_MAX_CALLED_PTY_BCD_NUM_LENGTH + 2; /* T + L + V (41 octets) */
                     /* IE_length value in cc_IE_info[] is always p_IE[1] + 2 ; after parsing */
                   }
              }
              else if(IEI_copy == IEI_REDIR_PARTY_BCD_NO)
              {
                   if( *(cc_IE_info[IE_count].start + 1) >  CC_MAX_REDIRECTING_PTY_BCD_NUM_LENGTH)
                   {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized IEI_REDIR_PARTY_BCD_NO IEsize = %d,truncating to %d", 
                                       *(cc_IE_info[IE_count].start + 1), (CC_MAX_REDIRECTING_PTY_BCD_NUM_LENGTH + 2));
                     *(cc_IE_info[IE_count].start + 1) = CC_MAX_REDIRECTING_PTY_BCD_NUM_LENGTH;
                     cc_IE_info[IE_count].length = CC_MAX_REDIRECTING_PTY_BCD_NUM_LENGTH + 2; /* T + L + V (17 octets) */
                     /* IE_length value in cc_IE_info[] is always p_IE[1] + 2 ; after parsing */
                   }
              }
            }

            if ( IE[1] < 1 )
            {
              return_value = FALSE;
            }
            else
            {
              if ( ( ((byte)( IE[2] & 0x70 ))>>4 >= 5 ) ||
                   ( (byte)( IE[2] & 0x0f ) == 2 ) ||
                   ( ( (byte)( IE[2] & 0x0f ) >= 5 ) &&
                     ( (byte)( IE[2] & 0x0f ) <= 7 )
                   ) ||
                   ( (byte)( IE[2] & 0x0f ) >= 10 )
                 )
              {
                 return_value = FALSE;
              }

              /*
               * additional special check if its CALLING party
               */


              if ( (IEI_copy == IEI_CALLING_PARTY_BCD_NO) ||       /* %%% */
                   (IEI_copy == IEI_REDIR_PARTY_BCD_NO) )       /* %%% */
              {
                 if ( ( IE[2] & 0x80 ) == 0 )           /* octet 3a follows */
                 {

                    if ( ( IE[3] & 0x60 ) == 0x60 ) 
                    {
                       return_value = FALSE;
                    }
                 }
              }
            
            }

            break;

         case IEI_REDIR_PARTY_SUBADDR:
         case IEI_CALLING_PARTY_SUBAD:
         case IEI_CALLED_PARTY_SUBAD:              /* same test */ 

            if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
            {
              if(IEI_copy == IEI_CALLING_PARTY_SUBAD)
              {
                   if( *(cc_IE_info[IE_count].start + 1) > CC_MAX_CALLING_PTY_SUBADDR_LENGTH)
                   {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized IEI_CALLING_PARTY_SUBAD IEsize = %d,truncating to %d",
                                           *(cc_IE_info[IE_count].start + 1), (CC_MAX_CALLING_PTY_SUBADDR_LENGTH + 2));
                     *(cc_IE_info[IE_count].start + 1) = CC_MAX_CALLING_PTY_SUBADDR_LENGTH;
                     cc_IE_info[IE_count].length = CC_MAX_CALLING_PTY_SUBADDR_LENGTH + 2; /* T + L + V (23 octets) */
                     /* IE_length value in cc_IE_info[] is always p_IE[1] + 2 ; after parsing */
                   }
              }
              else if(IEI_copy == IEI_CALLED_PARTY_SUBAD)
              {
                   if( *(cc_IE_info[IE_count].start + 1) > CC_MAX_CALLED_PTY_SUBADDR_LENGTH)
                   {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized IEI_CALLED_PARTY_SUBAD IEsize = %d,truncating to %d", 
                                            *(cc_IE_info[IE_count].start + 1), (CC_MAX_CALLED_PTY_SUBADDR_LENGTH + 2));
                     *(cc_IE_info[IE_count].start + 1) = CC_MAX_CALLED_PTY_SUBADDR_LENGTH;
                     cc_IE_info[IE_count].length = CC_MAX_CALLED_PTY_SUBADDR_LENGTH + 2; /* T + L + V (23 octets) */
                     /* IE_length value in cc_IE_info[] is always p_IE[1] + 2 ; after parsing */
                   }
              }
              else if(IEI_copy == IEI_REDIR_PARTY_SUBADDR)
              {
                   if( *(cc_IE_info[IE_count].start + 1) > CC_MAX_REDIRECTING_PTY_SUBADDR_LENGTH)
                   {
                     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Oversized IEI_REDIR_PARTY_SUBADDR IEsize = %d,truncating to %d", 
                                       *(cc_IE_info[IE_count].start + 1), (CC_MAX_REDIRECTING_PTY_SUBADDR_LENGTH + 2));
                     *(cc_IE_info[IE_count].start + 1) = CC_MAX_REDIRECTING_PTY_SUBADDR_LENGTH;
                     cc_IE_info[IE_count].length = CC_MAX_REDIRECTING_PTY_SUBADDR_LENGTH + 2; /* T + L + V (23 octets) */
                     /* IE_length value in cc_IE_info[] is always p_IE[1] + 2 ; after parsing */
                   }
              }
            }

            if ( IE[1] < 1 )
            {
              return_value = FALSE;
            }
            else
            {
              if ((((byte)( IE[2] & 0x70 ))>>4) > 1 )
              { 
                 return_value = FALSE;
              }
            }
            break;

         case IEI_LOW_LAYER_COMPATIB:
            /****************************************************************************************** 
            *From section 10.5.4.18 of 24.008,MAX length of Low Layer Compatibility(LLC) IE is 18 bytes.
            *So its value part is max of 16 bytes.From section 11.4.2 of 24.007 network can send more 
            *than 16 bytes of LLC value.If Network sends the LLC value more than 16 bytes,take only 
            *16 bytes from the network given value since LLC value part can have 
            *MAX of 16 bytes only as per 24.008            
            *******************************************************************************************/
            if(IE[1] > 16 )
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Low Layer Compatibility IE Length %d is greater than 16",IE[1]);
               IE[1] = 16;
            }
            break;

         case IEI_HIGH_LAYER_COMPATIB:
            if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
            {
                 /********************************************************************************** 
                 * Truncate the size of IE if it's oversized
                 **********************************************************************************/
                 
              if(cc_IE_info[IE_count].length > IEI_HIGH_LAYER_COMPATIB_MAX_LENGTH)
              {
                MSG_ERROR_DS_2(MN_SUB,"=MNCNM= IEI_Cause:Oversized IEI_HIGH_LAYER_COMPATIB  size = %d,truncating it to %d",
                                                      cc_IE_info[IE_count].length, IEI_HIGH_LAYER_COMPATIB_MAX_LENGTH);
                cc_IE_info[IE_count].length = IEI_HIGH_LAYER_COMPATIB_MAX_LENGTH;
                       
                if(*(cc_IE_info[IE_count].start + 1) > (IEI_HIGH_LAYER_COMPATIB_MAX_LENGTH - 2))
                {
                  *(cc_IE_info[IE_count].start + 1) = IEI_HIGH_LAYER_COMPATIB_MAX_LENGTH - 2;
                }
              }
            }
            break;

#ifdef FEATURE_UUS
         case IEI_USER_USER:
            /************************************************************************************ 
            *From section 10.5.4.25 of 24.008,MAX length of USER-USER IE is 131 bytes.So its
            *value part is max of 129 bytes.From section 11.4.2 of 24.007 network can send
            *more than 129 bytes of USER-USER value.If Network sends the USER-USER value more than
            *129 bytes,take only 129 bytes form the network given value since USER-USER value part
            *can have MAX of 129 bytes only as per 24.008            
            **************************************************************************************/
            /* value of MAX_USER_USER_DATA_LENGTH is 128. IE[1] contains the length of the user-user IE*/
            if(IE[1] > (MAX_USER_USER_DATA_LENGTH + 1) )
            {
               MSG_ERROR_DS_2(MN_SUB,"=MNCNM= USER=USER IE Length %d is greater than %d",
                  IE[1], MAX_USER_USER_DATA_LENGTH + 1);

               IE[1] = MAX_USER_USER_DATA_LENGTH + 1;
            }
            else if(IE[1] < 1) /* Min data length of this IE is 1*/
            {
               return_value = FALSE;
            }

            if ( (return_value == TRUE) &&
                 ( ( IE[2] == 5 ) ||
                 ( IE[2] == 6 ) ||
                 ( ( IE[2] >= 9 ) &&
                   ( IE[2] <= 15 )
                 )||
                 ( IE[2] == 255 )
               ))
            {
               return_value = FALSE;
            }

            break;

         case IEI_CONGESTION_CONTROL:
         case IEI_CONGESTION_CONTROL|CONGESTION_RECEIVER_NOT_READY:
            /* No checks */ 
            break;
#endif

         case IEI_SS_VERSION_INDICATOR:
            break;          /* no checking */

         case IEI_CALL_STATE:
         
            if ((IE[1] & 0xc0) == 0xc0)
            {                             /* check its GSM coding */
               state = (unsigned char)(IE[1] & 0x3f);

               if ( state <= 12 )
               {
                  if ( state == 5 ) 
                  {
                     return_value = FALSE;
                  }
               }
               else
               {
                  if ( ( state != 19 ) &&
                       ( state < 26 ) )
                  {
                     return_value = FALSE;
                  }
                  else
                  {
                     if ( state > 28 )
                     {
                        return_value = FALSE;
                     }
                  }
               }
            }
            break;

         case IEI_ALERTING_PATTERN:
         case IEI_CAUSE_OF_NO_CLI:
             /* If this optional IE is included in the SETUP msg */
             /* it must have a length of 1                       */

             if (IE[1] != 1)
             {
                 IE[1] = 1; 
                 /* 
                  * if length is more it would be trunctated, so that even if this IE gets processed somehow, 
                  * buffer overflow should not happen while copying its data
                  */
                  
                 return_value = FALSE;
             }
             else if(IEI == IEI_ALERTING_PATTERN)
             {
               if(IE[2] > 8)
               {
                 return_value = FALSE; /* Table 10.5.132/3GPP TS 24.008: Alerting Pattern information element */
               }
             }

             break;

#ifdef FEATURE_CCBS
         case IEI_ALLOWED_ACTIONS:
            if (IE[1] !=  1)  // length is always 1
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]allowed action IE len:%d",IE[1]);
               IE[1] = 1; 
               /* 
                * if length is more it would be trunctated, so that even if this IE gets processed somehow, 
                * buffer overflow should not happen while copying its data
                */

               return_value = FALSE;
            }
            else if ((IE[2]&0x7f) != 0x00) // spare bits are zero
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]allowed action IE spare bits:%d",IE[2]);
               return_value = FALSE;
            }

            break;

         case IEI_RECALL_TYPE:
            if ((IE[1]&0xf8) != 0x00) // spare bits are zero
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]recall type IE spare bits:%d",IE[1]);
               return_value = FALSE;
            }

            if ((IE[1]&0x07) == 0x07) // 111 is reserved
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= [ccbs]recall type IE reserved:%d",IE[1]);
               return_value = FALSE;
            }
            break;

         case IEI_SETUP_CONTAINER:
                 if((return_value == TRUE) && (IE_count < MAXNO_IES_IN_MESSAGE))
                 {
                   if(cc_IE_info[IE_count].length -1 > sizeof(ie_setup_container_T))
                   {
                     MSG_HIGH_DS_2(MN_SUB,"=MNCNM=  Truncating IEI SETUP CONTAINER length %d to size of data buffer %d",cc_IE_info[IE_count].length -1,sizeof(ie_setup_container_T));
                     cc_IE_info[IE_count].length = sizeof(ie_setup_container_T)+1;
                     *(cc_IE_info[IE_count].start + 1) = cc_IE_info[IE_count].length -2;
                   }
                 }
            break;
#endif /* FEATURE_CCBS */

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
         case IEI_NETWORK_CC_CAPABILITY:
            if (IE[1] !=  1)  // length is always 1
            {
               MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Network CC Capability IE len:%d",IE[1]);
               IE[1] = 1; 
               /* 
                * if length is more it would be trunctated, so that even if this IE gets processed somehow, 
                * buffer overflow should not happen while copying its data
                */

               return_value = FALSE;
            }
            break;
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

         default:
            MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected IEI value: given IEI = %d\n",
                        (dword) IEI);

            return_value = FALSE;
            break;

      }
   }

   if ( return_value == FALSE )
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Syntax error in IE: given IEI = %d\n", (dword) IEI);
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= * * * * * Ignoring this IE... * * * * * \n");
   }
   return return_value;
}
/**************** End of   CNM_check_IE_syntax ***************************************/

/**************** Begin of   CNM_write_cause_ie ***********************************
 *
 *  Function name: CNM_write_cause_ie()
 *  ----------------------------------
 *
 *  Description:
 *  ------------
 *  Function to append a cause IE to a message.
 *
 *  Parameters:
 *  -----------
 *  p_write_point : IN   - pointer to location where cause IE is to be writted
 *  cause_value   : IN   - Single byte cause value to use
 *
 *  Returns:
 *  --------
 *  Length of cause IE written
 *
 ******************************************************************************/
byte CNM_write_cause_ie( byte * p_write_point,
                             byte cause_value )
{
    byte num_of_bytes = 0;

   *(p_write_point++) = IEI_CAUSE;
   *(p_write_point++) = 2;             /* 2 bytes follow */
   *(p_write_point++) = 0xe0;  /* standard GSM   */
   *(p_write_point) = (unsigned char)(cause_value | 0x80);
   num_of_bytes = 4;

   return num_of_bytes;            /* number of bytes written */
}
/**************** End of   CNM_write_cause_ie ***************************************/


/**************** Begin of   CNM_append_IE_to_message *****************************
 *
 *  Function name: CNM_append_IE_to_message()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to append an information element to a message. Pointer to
 *  write point is updated to location after end of IE
 *
 *  Parameters:
 *  -----------
 *  p_IE : IN - pointer to information element to append
 *  pp_write_point : INOUT - Pointer to pointer to location where IE is to
 *                           be written
 *  size : IN - Size of IE
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
boolean CNM_append_IE_to_message( byte * p_IE,
                                   byte ** pp_write_point,
                                   byte size,
                                   byte *p_end_write_point)
{

   if( (*pp_write_point + size - 1) <= p_end_write_point )
   {

     (void)memscpy(*pp_write_point, (p_end_write_point - *pp_write_point + 1) ,p_IE, size);

     *pp_write_point += size;

       return TRUE;
   }
   else
   {
     MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!!! Cannot write %d bytes because of insufficient space in write buffer, space in write buffer = %d,",size,(p_end_write_point - *pp_write_point + 1));
     return FALSE;
   }

}
/**************** End of   CNM_append_IE_to_message ***************************************/

/**************** Begin of   CNM_check_for_last_connection ************************************
 *
 *  Function name: CNM_check_for_last_connection()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks to see if there are any active connections.  If it finds no
 *  active connections,  it sends a release request to MM and returns a true/false flag
 *  that indicates if it found any transactions in progress.
 *  
 *
 *  This function 
 *  Parameters:
 *  -----------
 *  cause : IN  - rel type to put in the message
 *
 *  Returns:
 *  --------
 *  transaction_in_progress : TRUE/FALSE
 *
 ******************************************************************************/

boolean CNM_check_for_last_connection (byte rel_type
#if defined(FEATURE_DUAL_SIM )
                                     , sys_modem_as_id_e_type subs_id
#endif
                                       )
{
  boolean           transactions_in_progress = FALSE;
  transaction_id_T  transaction_id;
  mmcnm_rel_req_T    mmcnm_rel_req;

  if(mm_link_reestablishing == FALSE)
  {
    for( transaction_id = 0; transaction_id < MAXNO_CC_TRANSACTIONS; transaction_id++ )
    {
      if(cc_transaction_information_p[transaction_id] != NULL
          && cc_transaction_information_p[transaction_id]->cc_state != CC_STATE_U0)
      {
        transactions_in_progress = TRUE;
        break;
      }
    }
    if(transactions_in_progress == FALSE)
    {
      transaction_id = 0x07;

      MN_clear_call_data(transaction_id);
    }

    if(transactions_in_progress == FALSE)
    {
      for( transaction_id = 0; transaction_id < MAXNO_SS_TRANSACTIONS; transaction_id++ )
      {
        if((CNM_get_ss_state(transaction_id) != SS_IDLE) || (ss_transaction_information[transaction_id].ss_mn_state == SS_MN_NOT_IDLE))
        {
          transactions_in_progress = TRUE;
          break;
        }
      }

      if(transactions_in_progress == FALSE)
      {
        for( transaction_id = 0; transaction_id < MAXNO_SMS_TRANSACTIONS; transaction_id++ )
        {
          if(sms_transaction_information[transaction_id].sms_state != SMS_IDLE)
          {
            transactions_in_progress = TRUE;
            break;
          }
        }
      }
    }

    /* AT +CMMS=1: reset on last mo SMS transaction */

    if(sms_link_control.cp_ack_deferred)
    {
      CNM_send_cp_ack(sms_link_control.connection_id);

      sms_link_control.cp_ack_deferred = FALSE;  /* reset cp_ack_deferred */
    }

    if((transactions_in_progress == FALSE) && (mm_rel_req_sent == FALSE))
    {
      /* Send a release request to MM */

      mmcnm_rel_req.header.message_set = MS_CC_MM;
      mmcnm_rel_req.header.message_id  = MMCNM_REL_REQ;
      mmcnm_rel_req.rel_type = rel_type;

#ifdef FEATURE_DUAL_SIM
      if(MNCNM_IS_SINGLE_STACK)
      {
        if((subs_id > SYS_MODEM_AS_ID_NONE) && (subs_id < MAX_AS_IDS))
        {
          mmcnm_rel_req.as_id = subs_id;
        }
        else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
        {
          mmcnm_rel_req.as_id = mn_as_id;
        }
        else
        {
          MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Both mn_as_id and subs_id are invalid");
          return FALSE; //either mn_as_id or subs_id need to be valid
        }
        PUT_IMH_LEN(sizeof(mmcnm_rel_req_T) - sizeof(IMH_T), 
                           &mmcnm_rel_req.header);
      }
#ifdef FEATURE_DUAL_ACTIVE
      else
      {
        mmcnm_rel_req.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
        PUT_IMH_LEN(sizeof(mmcnm_rel_req_T) - sizeof(IMH_T), 
                     &mmcnm_rel_req.header);  
      }
#endif
#else
      {
        PUT_IMH_LEN(sizeof(mmcnm_rel_req.rel_type), &mmcnm_rel_req.header);
      }
#endif   
      CNM_send_message((IMH_T *)&mmcnm_rel_req, GS_QUEUE_MM);

      /* Start the CNM_MM_REL_TIMER 
       * Use default values for TI and PD 
      */

      CNM_start_timer(CNM_MM_REL_TIMER, 0, 0
#if defined(FEATURE_DUAL_SIM )
                    , mmcnm_rel_req.as_id //we want to give this as data to timer module in this timer
#endif
                      );


      /* Set the mm_rel_req_sent flag to true */

      mm_rel_req_sent = TRUE;

#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
        if( (CNM_check_if_any_service_object_exists() == FALSE) &&
            (mn_as_id != SYS_MODEM_AS_ID_NONE) )
        {
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
          mn_as_id = SYS_MODEM_AS_ID_NONE;
        }
      }
#endif

    }

    if((transactions_in_progress == TRUE) && (mm_rel_req_sent == FALSE))
    {
      if((CNM_mmcc_sync_ind.reason != ((byte)WCDMA_RAB_RELEASED)) &&
          (CNM_has_active_cc_connection() == FALSE))   /* possible TCH reused */
      {
         boolean is_G_RAT = FALSE;
#ifdef FEATURE_DUAL_SIM
         if(MNCNM_IS_SINGLE_STACK)
         {
           is_G_RAT = is_cs_gsm_mode(mn_as_id);
         }
#ifdef FEATURE_DUAL_ACTIVE
         else
         {
           is_G_RAT = is_cs_gsm_mode((sys_modem_as_id_e_type)mn_dsda_as_id);
         }
#endif
#else 
         {
           is_G_RAT = is_cs_gsm_mode(MM_AS_ID_1);
         }
#endif
        if(is_G_RAT == TRUE)
        {
          CNM_mmcc_sync_ind.reason = (byte)GSM_RES_ASSIGNED;
        }
        else
        {
          CNM_mmcc_sync_ind.reason = (byte)WCDMA_RAB_ESTABLISHED;
        }
#if defined(FEATURE_DUAL_SIM)
        if(MNCNM_IS_SINGLE_STACK)
        {
          if((subs_id > SYS_MODEM_AS_ID_NONE) && (subs_id < MAX_AS_IDS))
          {
            CNM_mmcc_sync_ind.as_id = subs_id; /* Check if setting global var is correct or shd we have a local struct */
          }
          else if(mn_as_id != SYS_MODEM_AS_ID_NONE)
          {
            CNM_mmcc_sync_ind.as_id = mn_as_id;
          }
          else
          {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Both mn_as_id and subs_id are invalid");
            return FALSE; //either mn_as_id or subs_id need to be valid
          }
        }
#if defined(FEATURE_DUAL_ACTIVE)
        else
        {
          CNM_mmcc_sync_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id; /* Check if setting global var is correct or shd we have a local struct */
        }
#endif
#endif	
        CNM_send_message( (IMH_T *)&CNM_mmcc_sync_ind, GS_QUEUE_MN_CM );   /* Restore the last saved sync ind */
      }
    }
  }  /* (mm_link_reestablishing == FALSE) */
  return transactions_in_progress;
}

/**************** End of   CNM_check_for_last_connection **************************************/


/**************** Begin of   CNM_check_if_any_service_object_exists ************************************
 *
 *  Function name: CNM_check_for_last_connection()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks to see if there are any active connections.
 *  returns TRUE if it found any transactions in progress, else FALSE..
 *  
 *  This function 
 *  Parameters:
 *  -----------
 *  Void
 *
 *  Returns:
 *  --------
 *  transaction_in_progress : TRUE/FALSE
 *
 ******************************************************************************/

boolean CNM_check_if_any_service_object_exists(void ) 
{
  boolean           transactions_in_progress = FALSE;
  transaction_id_T  transaction_id;


  for( transaction_id = 0; transaction_id < MAXNO_CC_TRANSACTIONS; transaction_id++ )
  {
    if(cc_transaction_information_p[transaction_id] != NULL
        && cc_transaction_information_p[transaction_id]->cc_state != CC_STATE_U0)
    {
      transactions_in_progress = TRUE;
      break;
    }
  }
  
  if(transactions_in_progress == FALSE)
  {
    for( transaction_id = 0; transaction_id < MAXNO_SS_TRANSACTIONS; transaction_id++ )
    {
      /* Check both MN and CNM SS states to ensure that SS transaction is not in progress 
       * MN SS state also needed to handle a race scenario where MN has sent BEGIN_REQ to CNM but CNM hasn't received it.
       * */
      if((CNM_get_ss_state(transaction_id) != SS_IDLE) || (ss_transaction_information[transaction_id].ss_mn_state == SS_MN_NOT_IDLE))
      {
        transactions_in_progress = TRUE;
        break;
      }
    }
  
    if(transactions_in_progress == FALSE)
    {
      for( transaction_id = 0; transaction_id < MAXNO_SMS_TRANSACTIONS; transaction_id++ )
      {
        if(sms_transaction_information[transaction_id].sms_state != SMS_IDLE
            || sm_rl_transaction_information[transaction_id].rl_state != IDLE)
          /* the MO SMSs stored in the SMS Queue also should be considered */
        {
          transactions_in_progress = TRUE;
          break;
        }
      }
    }
  }

  return transactions_in_progress;

}
/**************** End of   CNM_check_if_any_service_object_exists **************************************/


/*===========================================================================

FUNCTION: CNM_is_GW_MT_SMS_in_progress

DESCRIPTION: This function checks whether MT SMS over MM connection exists 

DEPENDENCIES: none

PARAMETERS: none

RETURN VALUE: TRUE, if MT SMS over CNM/MM exists.
                          else FALSE

SIDE EFFECTS:
  None
===========================================================================*/


boolean CNM_is_GW_MT_SMS_in_progress(void)
{
   transaction_id_T  transaction_id;
   boolean           transactions_in_progress = FALSE;

   for( transaction_id = MT; transaction_id < MAXNO_SMS_TRANSACTIONS; transaction_id++ )
   {
     if(sms_transaction_information[transaction_id].sms_state != SMS_IDLE)
     {
       MSG_HIGH_DS_0(MN_SUB,"=MNCNM= GW MT SMS Transaction in progress ");
       transactions_in_progress = TRUE;
       break;
     }
   }
   return transactions_in_progress;
}


