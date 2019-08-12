/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmparse.c_v   1.8   13 Jun 2002 15:08:50   jault  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmparse.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/29/01   sbk     The mandatory part of LU accept is changed from 18 to 7
                   The LAI checking for non PCS mode is removed
06/20/01   sbk     If an IE can not be checked, then it is passed by default
08/07/01   ts      Standardized MSG_ERROR and MSG_HIGH usage.
08/15/01   ab      Renamed CM to CNM.
08/22/01   jca     Removed errant CNM renames.
11/01/02   mks     Added support for MM/GMM Information procedure
04/03/03   cd      Added support for equivalent PLMN list in the LU_ACCEPT, 
                   GMM_RAU_ACCEPT and GMM_ATTACH_ACCEPT msg
06/06/05   abe     Added code for CCBS support
09/20/05   ajt     Added IEI_EMERGENCY_LIST to LU Accept message
09/08/11   gps     skipping other checks inorder to remove read overflows, if 
                   there are too many digits in iei_mobile_id
09/19/11   gps     checking whether whole ie is inside the message boundary,
                     using exact message boundary rather than max message length,
                     and correcting logical errors while handling ies of format TLV
09/12/11   HC      Replacing the #include "gs.h" with #include #include "gs_v.h"
                   as a part of CMI activity 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "mm_iei.h"
#include "com_iei_v.h"
#include "timers_v.h"
#include "gs_v.h"
#include "mm_mm_v.h"
#include "mm_v.h"
#include "gmm_v.h"
#include "err.h"
#include "gmm_msg.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================
  The following macro functions are used to simplify the data entry.
  The linked list data is entered in the following form:
 
  ROOT_MESSAGE(<first_message_name>)
  MESSAGE_DETAIL(<first_message_name>, 
                 <message_type>, 
                 <Length of imperative part of message>,
                 <next message name>
                 <name of first IE in this message> )
 
  Then for each IE in the message, there is one entry as follows:
 
  IE_DETAIL( <first IE name>,
             <corresponding name of the type of IE from the IE_TYPE table>,
             <format of the IE in this message>,
             <either MANDATORY, OPTIONAL, or CONDITIONAL depending on presence field 
                                                             from 04.08>,
             <name of next IE in message>
 
  The last IE_DETAILS has the name END_POINTER in the next IE field
 
 
  The last MESSAGE_DETAIL has END_POINTER for the next message name field
 
 
  THE ONLY OTHER PROBLEM IS THAT THE DATA IS SPECIFIED IN REVERSE ORDER SO
  THAT THE ADDRESSES FOR THE LINKED LIST ARE KNOWN WHEN THEY ARE REFERENCED
 
 
  The IE_TYPE table is a list of IE_TYPE macro function calls in any order
  There is no list linking within this table.
 ===========================================================================*/

#define MESSAGE_DETAIL(Name,\
                       message_type, \
                       ImperativeLength, \
                       NextName, \
                      FirstMessageIE) \
\
   struct MsgDetailTag Name = \
   { \
      message_type, \
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

struct MessageIETag mm_EndLoc;
struct MsgDetailTag mm_EndMessagesLoc;

#define END_POINTER mm_EndLoc
#define END_MESSAGES mm_EndMessagesLoc

#define ROOT_MESSAGE(Name) VOID_DATA * RootName = & Name;

/*
 * definition of the information elements we are likely to come across. The
 * entries are as follows:
 *
 * Type of information element -------------------------------.
 *                                                            |
 * Maximum length of the information element -------------.   |
 *                                                        |   |
 * Value of the information                               |   |
 * element identifier --------------.                     |   |
 *                                  |                     |   |
 * Arbitrary name                   |                     |   |
 * given to entry                   |                     |   |
 * - This is used                   |                     |   |
 * to reference                     |                     |   |
 * the entry                        |                     |   |
 * elsewhere --.                    |                     |   |
 *             |                    |                     |   |
 *             v                    v                     v   v
 */

IE_TYPE(rand_ie,                 IEI_RAND,               17,  3 )
IE_TYPE(sres_ie,                 IEI_SRES,                5,  3 )
IE_TYPE(cm_service_type_ie,      IEI_CM_SERVICE_TYPE,     1,  1 )
IE_TYPE(identity_ie,             IEI_IDENTITY,            1,  1 )
IE_TYPE(lu_type_ie,              IEI_LU_TYPE,             1,  1 )
IE_TYPE(reject_cause_ie,         IEI_REJECT_CAUSE,        2,  3 )
IE_TYPE(follow_on_proceed_ie,    IEI_FOLLOW_ON_PROCEED,   1,  2 )
IE_TYPE(lai_ie,                  IEI_LAI,                 6,  3 )
IE_TYPE(mobile_id_ie,            IEI_MOBILE_ID,          10,  4 )
IE_TYPE(ciph_key_seq_number_ie,  IEI_CIPHER_KEY_SEQ_NUMBER, 1,1 )
IE_TYPE(autn_ie,                 IEI_AUTN,               18,  4 )
IE_TYPE(equiv_plmn_ie,           IEI_EQUIV_PLMN,         47,  4 )
IE_TYPE(emergency_list_ie,       IEI_EMERGENCY_NUM_LIST, 50,  4 )
#ifdef FEATURE_NAS_REL10
IE_TYPE(per_ms_3212_ie,          IEI_PER_MS_3212,         1,  4 )
#endif
#ifdef FEATURE_NAS_REL10
IE_TYPE(reject_t3246_ie,         IEI_PER_T3246_VALUE,     1,  4 )
#endif



/* -----------------------------------------------------------------
 * Per TS24.008, Full name for network is a type 4 IE with a minimum 
 * length of 3 octets and its maximum length is only limited by the
 * maximum number of octets allowed in an L3 message.
 * ----------------------------------------------------------------- */
IE_TYPE(network_full_name_ie,    IEI_NETWORK_FULL_NAME,  249,  4 )

/* -----------------------------------------------------------------
 * Per TS24.008, Short name for network is a type 4 IE with a minimum 
 * length of 3 octets and its maximum length is only limited by the
 * maximum number of octets allowed in an L3 message.
 * ----------------------------------------------------------------- */
IE_TYPE(network_short_name_ie,   IEI_NETWORK_SHORT_NAME, 249,  4 )

/* -----------------------------------------------------------------
 * Time Zone is a type 3 IE with a length of 2 octets 
 * ----------------------------------------------------------------- */
IE_TYPE(time_zone_ie,            IEI_TIME_ZONE,           2,  3 )

/* -----------------------------------------------------------------
 * Time Zone & time is a type 3 IE with a length of 8 octets 
 * ----------------------------------------------------------------- */
IE_TYPE(time_zone_and_time_ie,   IEI_TIME_ZONE_AND_TIME,  8,  3 )

/* -----------------------------------------------------------------
 * LSA Identifier is a type 4 IE with a max length of 5 octets 
 * ----------------------------------------------------------------- */
IE_TYPE(lsa_id_ie,               IEI_LSA_ID,              5,  4 )

/* -----------------------------------------------------------------
 * Daylight saving time is a type 4 IE with a length of 3 octets 
 * ----------------------------------------------------------------- */
IE_TYPE(daylight_saving_time_ie, IEI_DAYLIGHT_SAVING_TIME, 3,  4 )


IE_TYPE(gmm_ptmsi_sig_ie,        GMM_IEI_PTMSI_SIGNATURE,      4,  3 )
IE_TYPE(gmm_ready_timer_ie,      GMM_IEI_READY_TIMER_VALUE,    2,  3 )
IE_TYPE(gmm_cause_ie,            GMM_IEI_CAUSE,                2,  3 )
IE_TYPE(gmm_t3302_ie,            GMM_IEI_T3302_VALUE,          2,  3 )
IE_TYPE(gmm_cell_notify_ie,      GMM_IEI_CELL_NOTIFICATION,    1,  2 )
IE_TYPE(gmm_auth_rand_ie,        GMM_IEI_AUTH_PARAMETER_RAND, 18,  4 )
IE_TYPE(gmm_auth_cksn_ie,        GMM_IEI_AUTH_GPRS_CKSN,       1,  1 )
IE_TYPE(gmm_auth_autn_ie,        GMM_IEI_AUTH_PARAMETER_AUTN, 18,  4 )
IE_TYPE(gmm_npdu_list_ie,        GMM_IEI_AUTH_PARAMETER_AUTN, 19,  4 )

#ifdef FEATURE_CCBS 
IE_TYPE(pd_and_sapi_ie,          IEI_PD_AND_SAPI,          2,  3 )
#endif  

/*
 * GMM RAU Reject */
IE_DETAIL( gmm_RAU_reject_t3302         , gmm_t3302_ie,              TLV, OPTIONAL_IE,  END_POINTER )
MESSAGE_DETAIL(GMM_RAU_reject, GMM_ROUTING_AREA_UPDATE_REJECT, 4, END_MESSAGES, gmm_RAU_reject_t3302 )



/*
 * GMM RAU Accept */
IE_DETAIL( gmm_RAU_accept_equiv_plmn    , equiv_plmn_ie,             TLV, OPTIONAL_IE,  END_POINTER )
IE_DETAIL( gmm_RAU_accept_cell_notify   , gmm_cell_notify_ie,          T, OPTIONAL_IE,  gmm_RAU_accept_equiv_plmn )
IE_DETAIL( gmm_RAU_accept_t3302         , gmm_t3302_ie,              TLV, OPTIONAL_IE,  gmm_RAU_accept_cell_notify )
IE_DETAIL( gmm_RAU_accept_gmm_cause     , gmm_cause_ie,               TV, OPTIONAL_IE,  gmm_RAU_accept_t3302 )
IE_DETAIL( gmm_RAU_accept_ready_timer   , gmm_ready_timer_ie,         TV, OPTIONAL_IE,  gmm_RAU_accept_gmm_cause )
IE_DETAIL( gmm_RAU_accept_npdu_list     , gmm_npdu_list_ie,          TLV, OPTIONAL_IE,  gmm_RAU_accept_ready_timer )
IE_DETAIL( gmm_RAU_accept_ms_id         , mobile_id_ie,              TLV, OPTIONAL_IE,  gmm_RAU_accept_npdu_list )
IE_DETAIL( gmm_RAU_accept_ptmsi         , mobile_id_ie,              TLV, OPTIONAL_IE,  gmm_RAU_accept_ms_id )
IE_DETAIL( gmm_RAU_accept_ptmsi_sig     , gmm_ptmsi_sig_ie,           TV, OPTIONAL_IE,  gmm_RAU_accept_ptmsi )
MESSAGE_DETAIL(GMM_RAU_accept, GMM_ROUTING_AREA_UPDATE_ACCEPT, 10, GMM_RAU_reject, END_POINTER )


/*
 * GMM Identity Request
 */

MESSAGE_DETAIL(GMM_identity_request,  GMM_IDENTITY_REQUEST,          3, GMM_RAU_accept, END_POINTER )


/*
 * GMM Authentication and Ciphering Reject
 */

MESSAGE_DETAIL( GMM_auth_reject, GMM_AUTHENTICATION_AND_CIPHERING_REJECT, 2,
                GMM_identity_request, END_POINTER )


/*
 * GMM Authentication and Ciphering Request
 */
IE_DETAIL( gmm_auth_req_autn            , gmm_auth_autn_ie,          TLV,  OPTIONAL_IE,  END_POINTER )
IE_DETAIL( gmm_auth_req_cksn            , gmm_auth_cksn_ie,           TV,  CONDITIONAL_IE,  gmm_auth_req_autn )
IE_DETAIL( gmm_auth_req_rand            , gmm_auth_rand_ie,           TV,  OPTIONAL_IE,  gmm_auth_req_cksn )
MESSAGE_DETAIL( GMM_auth_req, GMM_AUTHENTICATION_AND_CIPHERING_REQUEST, 4,
                GMM_auth_reject, gmm_auth_req_rand )


/*
 * GMM P-TMSI REALLOCATION 
 */

MESSAGE_DETAIL( GMM_ptmsi_realloc, GMM_PTMSI_REALLOCATION_COMMAND, 4, GMM_auth_req,
                END_POINTER )


/*
 * GMM MO Detach Accept
 */

MESSAGE_DETAIL(GMM_MO_detach_accept , GMM_DETACH_ACCEPT,          2, GMM_ptmsi_realloc, END_POINTER )


/*
 * GMM MT Detach Request
 */

IE_DETAIL( gmm_mt_detach_gmm_cause      , gmm_cause_ie,           TV, OPTIONAL_IE,  END_POINTER )
MESSAGE_DETAIL(GMM_MT_detach_req    , GMM_DETACH_REQUEST,  3, GMM_MO_detach_accept,  gmm_mt_detach_gmm_cause )


/*
 * GMM Attach Reject
 */

IE_DETAIL( gmm_at_reject_t3302          , gmm_t3302_ie,              TLV, OPTIONAL_IE,  END_POINTER )
MESSAGE_DETAIL(GMM_attach_reject    , GMM_ATTACH_REJECT, 3, GMM_MT_detach_req,      gmm_at_reject_t3302 )

/*
 * GMM Attach Accept
 */

IE_DETAIL( gmm_accept_equiv_plmn        , equiv_plmn_ie,             TLV, OPTIONAL_IE,  END_POINTER )
IE_DETAIL( gmm_at_accept_cell_notify    , gmm_cell_notify_ie,          T, OPTIONAL_IE,  gmm_accept_equiv_plmn )
IE_DETAIL( gmm_at_accept_t3302          , gmm_t3302_ie,              TLV, OPTIONAL_IE,  gmm_at_accept_cell_notify )
IE_DETAIL( gmm_at_accept_gmm_cause      , gmm_cause_ie,               TV, OPTIONAL_IE,  gmm_at_accept_t3302 )
IE_DETAIL( gmm_at_accept_ms_id          , mobile_id_ie,              TLV, OPTIONAL_IE,  gmm_at_accept_gmm_cause )
IE_DETAIL( gmm_at_accept_ptmsi          , mobile_id_ie,              TLV, OPTIONAL_IE,  gmm_at_accept_ms_id )
IE_DETAIL( gmm_at_accept_ready_timer    , gmm_ready_timer_ie,         TV, OPTIONAL_IE,  gmm_at_accept_ptmsi )
IE_DETAIL( gmm_at_accept_ptmsi_sig      , gmm_ptmsi_sig_ie,           TV, OPTIONAL_IE,  gmm_at_accept_ready_timer )
MESSAGE_DETAIL(GMM_attach_accept        , GMM_ATTACH_ACCEPT,  11, GMM_attach_reject, gmm_at_accept_ptmsi_sig )


/* --------------------------
 * GMM Information Message
 * -------------------------- */

IE_DETAIL( gmm_network_daylight_saving_time , daylight_saving_time_ie,   TLV,   OPTIONAL_IE,       END_POINTER )
IE_DETAIL( gmm_lsa_id                       , lsa_id_ie,                 TLV,   OPTIONAL_IE,       gmm_network_daylight_saving_time )
IE_DETAIL( gmm_univ_and_local_tz            , time_zone_and_time_ie,      TV,   OPTIONAL_IE,       gmm_lsa_id )
IE_DETAIL( gmm_local_tz                     , time_zone_ie,               TV,   OPTIONAL_IE,       gmm_univ_and_local_tz )
IE_DETAIL( gmm_network_short_name           , network_short_name_ie,     TLV,   OPTIONAL_IE,       gmm_local_tz)
IE_DETAIL( gmm_network_full_name            , network_full_name_ie,      TLV,   OPTIONAL_IE,       gmm_network_short_name )
MESSAGE_DETAIL( GMM_information             , GMM_INFORMATION,             2,   GMM_attach_accept, gmm_network_full_name )

/*GMM Message END here */


/*
 * MM Messages Start From Here
 */

/* --------------------------
 * MM Information Message
 * -------------------------- */

IE_DETAIL( mm_network_daylight_saving_time , daylight_saving_time_ie,   TLV,   OPTIONAL_IE,        END_POINTER )
IE_DETAIL( mm_lsa_id                       , lsa_id_ie,                 TLV,   OPTIONAL_IE,        mm_network_daylight_saving_time )
IE_DETAIL( mm_univ_and_local_tz            , time_zone_and_time_ie,      TV,   OPTIONAL_IE,        mm_lsa_id )
IE_DETAIL( mm_local_tz                     , time_zone_ie,               TV,   OPTIONAL_IE,        mm_univ_and_local_tz )
IE_DETAIL( mm_network_short_name           , network_short_name_ie,     TLV,   OPTIONAL_IE,        mm_local_tz)
IE_DETAIL( mm_network_full_name            , network_full_name_ie,      TLV,   OPTIONAL_IE,        mm_network_short_name )
MESSAGE_DETAIL( MM_information             , MM_INFORMATION,              2,   GMM_information,    mm_network_full_name )

/*
 * AUTHENTICATION_REJECT  message
 */

MESSAGE_DETAIL(auth_rej          , AUTHENTICATION_REJECT,         2, MM_information, END_POINTER )


/*
 * Authentication request message
 */
IE_DETAIL( auth_req_autn            , autn_ie,                     TLV, OPTIONAL_IE , END_POINTER )
IE_DETAIL( auth_req_rand            , rand_ie,                       V, MANDATORY_IE, auth_req_autn)
IE_DETAIL( auth_req_ciph_key_seq_no , ciph_key_seq_number_ie,        V, MANDATORY_IE, auth_req_rand )
MESSAGE_DETAIL(auth_req             , AUTHENTICATION_REQUEST,     19, auth_rej, auth_req_ciph_key_seq_no )

/*
 * CM_SERVICE_ACCEPT message
 */

MESSAGE_DETAIL(cm_service_acc       , CM_SERVICE_ACCEPT,          2, auth_req, END_POINTER )

/* CM_SERVICE_PROMPT */

#ifdef FEATURE_CCBS
IE_DETAIL( cmsp_pd_sapi             , pd_and_sapi_ie,               V, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(cm_service_prompt    , CM_SERVICE_PROMPT,          3, cm_service_acc, cmsp_pd_sapi )

/*
 * CM_SERVICE_REJECT message
 */

IE_DETAIL( cms_rej_cause            , reject_cause_ie,               V, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(cm_service_rej       , CM_SERVICE_REJECT,          3, cm_service_prompt, cms_rej_cause )

#else 
/*
 * CM_SERVICE_REJECT message
 */
#ifdef FEATURE_NAS_REL10
IE_DETAIL( serv_rej_t3246           ,reject_t3246_ie,                TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL( cms_rej_cause            ,reject_cause_ie,               V,    MANDATORY_IE,serv_rej_t3246 )
#else
IE_DETAIL( cms_rej_cause            ,reject_cause_ie,               V, MANDATORY_IE, END_POINTER )
#endif
MESSAGE_DETAIL(cm_service_rej       ,CM_SERVICE_REJECT,          3, cm_service_acc, cms_rej_cause )
#endif /* FEATURE_CCBS */


/*
 * ABORT message
 */

IE_DETAIL( abort_rej_cause          , reject_cause_ie,               V, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(abort_msg            , ABORT ,                     3, cm_service_rej, abort_rej_cause )

/*
 * IDENTITY_REQUEST message
 */

IE_DETAIL( identity_req_type        , identity_ie,                   V, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(identity_req         , IDENTITY_REQUEST,           3, abort_msg, identity_req_type )

/*
 * LOCATION_UPDATING_ACCEPT message
 */
#ifdef FEATURE_NAS_REL10
IE_DETAIL( lu_accept_per_ms_3212    , per_ms_3212_ie,              TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL( lu_accept_emergency_list , emergency_list_ie,           TLV, OPTIONAL_IE, lu_accept_per_ms_3212 )
#else
IE_DETAIL( lu_accept_emergency_list , emergency_list_ie,           TLV, OPTIONAL_IE, END_POINTER )
#endif
IE_DETAIL( lu_accept_equiv_plmn     , equiv_plmn_ie,               TLV, OPTIONAL_IE, lu_accept_emergency_list )
IE_DETAIL( lu_accept_follow_on      , follow_on_proceed_ie,          T, OPTIONAL_IE, lu_accept_equiv_plmn )
IE_DETAIL( lu_accept_identity       , mobile_id_ie,                TLV, OPTIONAL_IE, lu_accept_follow_on )
IE_DETAIL( lu_accept_lai            , lai_ie,                        V, MANDATORY_IE, lu_accept_identity )
MESSAGE_DETAIL(lu_accept            , LOCATION_UPDATING_ACCEPT,   7, identity_req, lu_accept_lai )

/*
 * LOCATION_UPDATING_REJECT message
 */
#ifdef FEATURE_NAS_REL10
IE_DETAIL( lu_rej_t3246             ,reject_t3246_ie,                TLV, OPTIONAL_IE, END_POINTER )
IE_DETAIL( lu_rej_cause             ,reject_cause_ie,                V,  MANDATORY_IE, lu_rej_t3246 )
#else
IE_DETAIL( lu_rej_cause             ,reject_cause_ie,                V,  MANDATORY_IE, END_POINTER )
#endif
MESSAGE_DETAIL(lu_reject            , LOCATION_UPDATING_REJECT,   3, lu_accept, lu_rej_cause )

/*
 * MM_STATUS message
 */

IE_DETAIL( mm_status_cause          , reject_cause_ie,               V, MANDATORY_IE, END_POINTER )
MESSAGE_DETAIL(mm_status            , MM_STATUS,                  3, lu_reject, mm_status_cause )

/*
 * TMSI_REALLOCATION_COMMAND message
 */

IE_DETAIL( tmsi_realloc_id          , mobile_id_ie,                 LV, MANDATORY_IE, END_POINTER )
IE_DETAIL( tmsi_realloc_lai         , lai_ie,                        V, MANDATORY_IE, tmsi_realloc_id )
MESSAGE_DETAIL(tmsi_realloc         , TMSI_REALLOCATION_COMMAND,  16, mm_status, tmsi_realloc_lai )


/*
 * Define head of linked list
 */

ROOT_MESSAGE(tmsi_realloc)


/*===========================================================================

FUNCTION MM_SETUP_IE_DATA_READ

DESCRIPTION
  This function returns a pointer to the first ie details entry 
  for the message defined by message_type 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_setup_ie_data_read(message_type_T message_type, 
                              struct MessageIETag ** p_iedesc )
{
   struct MsgDetailTag   * Ptr;
   boolean                 return_value = FALSE;

   Ptr = RootName;      /* point at start of linked list */

   /*
    * Scan down the linked list
    */

   while( Ptr != &mm_EndMessagesLoc)
   {
 
      /*
       * if match on message type
       */

      if ( Ptr->message_type == message_type )
      {

         return_value = TRUE;
         break;
      }
      Ptr = Ptr->NextMsgDetail;
   }
   if (return_value == FALSE)
   {
      *p_iedesc = 0;
   }
   else
   {
      *p_iedesc = Ptr->FirstMessageIE;
   }
   return return_value;
}


/*===========================================================================

FUNCTION MM_GET_NEXT_IE_ALLOWED

DESCRIPTION
  This function returns details of successive information 
  elements allowed to be in a message. mm_setup_ie_data_read() 
  must be called first to identify the message of interest, 
  and to return a pointer to the (possible) first IE in the message.
  Then this function can be called, with the ref value obtained 
  above. The function returns the IEI, type, max length, and 
  presence flag for the next IE, and updates the reference so that 
  further calls to this function reveal the following IEs allowed. 
  When there are no more, the function returns FALSE.

DEPENDENCIES
  None

RETURN VALUE
  IEI
  Type
  Length
  Presence
  Valid - indicates if above data is meaningful, or if there 
                       are no more IEs

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_get_next_ie_allowed(byte *IEI, 
                               byte *Type, 
                               byte *Length,
                               byte *Presence, 
                               byte *Format,
                               struct MessageIETag** NextRef)

{
   boolean  return_value;

   if (*NextRef == &mm_EndLoc)
   {
      return_value = FALSE;
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
      return_value = TRUE;
   }

   return return_value;
}


/*===========================================================================

FUNCTION MM_CHECK_IE_SYNTAX

DESCRIPTION
  This function checks the semantics of an IE

DEPENDENCIES
  None

RETURN VALUE
  TRUE if IE is ok

SIDE EFFECTS
  None

===========================================================================*/

boolean  mm_check_ie_syntax( byte iei, byte * p_ie)
{
   boolean     return_value;

   return_value = TRUE;

   switch (iei)
   {
      case IEI_RAND:
         break;         /* no checking possible */

      case IEI_SRES:
         break;         /* no checking possible */ 

      case IEI_CM_SERVICE_TYPE:
         
         /*
          * check for values 1,2,4,or 8
          */

         if (((((*p_ie) & 0x0f)  & (-((*p_ie) & 0x0f ))) != ((*p_ie) & 0x0f )) 
             ||
             (((*p_ie) & 0x0f) == 0 )) 
         {
            return_value = FALSE;
         }
         break;

      case IEI_IDENTITY:
         if ((((*p_ie) & 0x07 ) < 1 ) ||
             (((*p_ie) & 0x07 ) > 5 ) )
         {
            return_value = FALSE;
         }
         break;

      case IEI_LU_TYPE:
         if (((*p_ie) & 0x07) > 2 )
         {
            return_value = FALSE;
         }
         break;

      case IEI_REJECT_CAUSE:
         break;         /* no checking possible */ 

      case IEI_FOLLOW_ON_PROCEED:
         break;         /* no checking possible */ 

      case IEI_LAI:

      /*
       * no reserved values for PCS, so checking only possible
       * with GSM and DCS
       */

/*#ifndef PCS_1900
      if ((*(p_ie+2) & 0xf0 ) != 0xf0 )
      {
         return_value = FALSE;
      }
#endif*/

      break;

      case IEI_MOBILE_ID:
         if ( (p_ie[2] & 0x07 ) == IMSI )      /* if its an IMSI */
         {
            if (p_ie[1] > 8 )
            {
               return_value = FALSE;      /* too many digits */
            }
            else
            {
               if ((p_ie[2] & 0x08) == 0 )   /* even number */
               {
                  if ( (p_ie[ p_ie[1] + 1 ] & 0xf0 ) != 0xf0 )
                  {
                     return_value = FALSE;   /* wrong filler nibble */
                  }
               }
            }
         }
         else
         if ( (p_ie[2]&0x07) == IMEI )         /* if its an IMEI */
         {
            if (p_ie[1] != 8 ) 
            {
               return_value = FALSE;      /* too many digits */
            }
            if ((p_ie[2] & 0x08) == 0 )   /* even number */
            {
               return_value = FALSE;      /* too many digits */
            }
         }
         else
         if ( (p_ie[2]&0x07) == TMSI )         /* if its an TMSI */
         {
            if (p_ie[1] != 5 ) 
            {
               return_value = FALSE;      /* too many digits */
            }
            if ((p_ie[2] & 0x08) != 0 )   /* even number */
            {
               return_value = FALSE;      /* wrong odd/even ind */
            }
            if ((p_ie[2] & 0xf0) != 0xf0 )
            {
               return_value = FALSE;      /* wrong filler */
            }
         }
         else
         {
            return_value = FALSE;      /* wrong id type */
         }
         break;

      case IEI_CIPHER_KEY_SEQ_NUMBER:
         if ((p_ie[0] & 0x07) == 0x07 )
         {
            return_value = FALSE;      /* invalid value */
         }
         break;

      default:
         /*-------------------------------------------------
          The default way should be that the IE can not be
          checked so it should PASS 
          -------------------------------------------------*/
         /* return_value = FALSE; */
         break;
   }
   return return_value;
}


/*===========================================================================

FUNCTION MM_STORE_IE_INFO

DESCRIPTION
  This function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_store_ie_info( byte * start, 
                              byte iei, 
                              byte length, 
                              byte presence )

{
   if (mm_no_of_ies < MAXNO_IES_IN_MESSAGE)
   {
 
      mm_IE_info[mm_no_of_ies].start = start;
      mm_IE_info[mm_no_of_ies].IEI = iei;
      mm_IE_info[mm_no_of_ies].length = length;
      mm_IE_info[mm_no_of_ies].presence = presence;
 
      mm_no_of_ies++;
   }
   else  
   {  
      MSG_ERROR_DS(MM_SUB, "=MM= MM: Too many IEs in message - ignored", 0,0,0);
   }
}



/*
 * local definitions
 */

typedef struct stored_allowed_ie_tag
{
   byte     iei;
   byte     type;
   byte     length;
   byte     presence;
   byte     format;
   boolean  found;


} stored_allowed_ie_T;


/*===========================================================================

FUNCTION MM_CHECK_FOR_PROTOCOL_ERRORS

DESCRIPTION
  This function checks the information elements in the received network message.
  If there are any problems, then the function returns FALSE so that no further
  processing is performed.
 
  If there is an IE which is not valid for the message, then a mm-status
  message is generated and sent to the network. Special processing applies
  for certain errors and certain messages.
 

DEPENDENCIES
  None

RETURN VALUE
  TRUE if success else FALSE

SIDE EFFECTS
  None

===========================================================================*/

boolean  mm_check_for_protocol_errors( mm_int_cmd_type_T * message )
{

   byte                          allowed_ie_pointer;
   byte                          *p_message_read;
   boolean                       return_value;
   boolean                       valid;
   struct MessageIETag           *nextref;
   byte                          valid_ie;
   byte                          this_ie;
   uint32                        length;
   byte                          ie_counter;
   stored_allowed_ie_T           stored_allowed_ie[MAXNO_IES_IN_MESSAGE];
   byte                          offset;
   boolean                       status;
   
   byte                          *ie_pointer = NULL;  /* Default to NULL */
   boolean                       allowed_ie = 0;      /* Default to 0 */
   byte                          error_reason = 0;    /* Default to 0 */
   byte                          actual_ie_length = 0;/* Default to 0 */
   
   mm_no_of_ies = 0;

   return_value = TRUE;
   GET_IMH_LEN( length, message );
   length = MIN( length,  MAX_OCTETS_L3_MESSAGE);

   if ( length < 2)   /* check if message is long enough to contain a type */
   {
      error_reason = 0;        /* no status return for this error */

      return_value = FALSE;    /* ignore short messages */

   }
   else if(length >= 3)
   {

      /*
       * set pointer to point at potential first IE after message type
       */

      p_message_read = &((rr_data_ind_T *)message)->layer3_message[2];


      status =
         mm_setup_ie_data_read(((mm_msg_head_T *)message)->type, &nextref ) ;

      if ( status != FALSE )
      {
         ie_counter = 0;

         do
         {
            valid = mm_get_next_ie_allowed (
                                      &stored_allowed_ie[ie_counter].iei,
                                      &stored_allowed_ie[ie_counter].type,
                                      &stored_allowed_ie[ie_counter].length,
                                      &stored_allowed_ie[ie_counter].presence,
                                      &stored_allowed_ie[ie_counter].format,
                                      &nextref);


            stored_allowed_ie[ie_counter].found = FALSE;
            if ( valid != FALSE )
            {
               ie_counter++;
            }
            if(ie_counter >= MAXNO_IES_IN_MESSAGE)
            {
               valid = FALSE;
               MSG_ERROR_DS(MM_SUB,"=MM= Index out of bounds and buffer overflow",0,0,0);
            }

         } while (valid != FALSE );


         allowed_ie_pointer = 0;

         /*
          * while more IEs in message ie.1 pMessageRead is not off end of
          * message as defined by message length in header
          */

         while ( ( p_message_read <
                           (byte *) message + sizeof(IMH_T) + length ) &&
                 (return_value != FALSE ))
         {

            /*
             * stop if no more Ies to match or out of mandatory stuff
             */

            if (( allowed_ie_pointer >= ie_counter ) ||
             ( stored_allowed_ie[allowed_ie_pointer].presence != MANDATORY_IE ))
            {
               break;     /* end of processing mandatory section of message */
            }

            /*
             * set up an offset value which is used to adjust the stored
             * length and pointer to start of IE for the cases when no IEI
             * is included and this affects the actual length of the IE
             */

            if ( ( stored_allowed_ie[allowed_ie_pointer].type == 1 ) ||
                 ( stored_allowed_ie[allowed_ie_pointer].type == 2 ) )
            {
               offset = 0;
            }
            else
            {
               offset = 1;
            }

            switch (stored_allowed_ie[allowed_ie_pointer].format)
            {

               case FORMAT_V:

                  ie_pointer = p_message_read - offset;
                  /* This will be stored as Format TV in mm_ie_info. 
                  *  stored_allowed_ie[allowed_ie_pointer].length will
                  *  take care of the extra  byte for type value.
                  */
                  mm_store_ie_info( ie_pointer,
                                   stored_allowed_ie[allowed_ie_pointer].iei,
                                   (byte)(stored_allowed_ie[allowed_ie_pointer].length),
                                   MANDATORY_IE );

                  stored_allowed_ie[allowed_ie_pointer].found = TRUE;

                  /*
                   * add the default length of this IE, less 1 to allow for
                   * the missing type, as its a V not a TV
                   */

                  p_message_read +=
                           stored_allowed_ie[allowed_ie_pointer].length -
                                                                     offset;

                  if ( p_message_read >
                           ((byte *) message + sizeof(IMH_T) + length ) )
                  {
                     return_value = FALSE;
                     error_reason = MM_INVALID_MANDATORY_INFORMATION;
                  }
                  break;

               case FORMAT_LV:
                  ie_pointer = p_message_read - offset;

                  mm_store_ie_info( ie_pointer,
                                      stored_allowed_ie[allowed_ie_pointer].iei,
                                      (byte)( *p_message_read + 1 + offset),
                                      MANDATORY_IE );

                  stored_allowed_ie[allowed_ie_pointer].found = TRUE;

                  p_message_read += *p_message_read + 1;

                  if ( p_message_read >
                           ((byte *) message + sizeof(IMH_T) + length ) )
                  {
                     return_value = FALSE;
                     error_reason = MM_INVALID_MANDATORY_INFORMATION;
                  }

                  break;

               /* all other formats contain the type, so we can check */

               default:
                  if ( *p_message_read != stored_allowed_ie[
                                                   allowed_ie_pointer].iei )
                  {
                     error_reason = MM_SEMANTICALLY_INCORRECT_MESSAGE;
                     return_value = FALSE;
                  }
                  else
                  {

                     /*
                      * work out the length
                      */

                     if ( stored_allowed_ie[allowed_ie_pointer].format ==
                                                               FORMAT_TLV )
                     {
                        if((p_message_read +1) >= ((byte *)message + sizeof(IMH_T) + length)) 
                        {
                           MSG_ERROR_DS(MM_SUB, "=MM= MM: Illegal L3 IE, out of bounds(%d) ", (byte)( (p_message_read+1) - ((byte *) message + sizeof(IMH_T) + length ) ),0,0);
                           actual_ie_length = (byte)length;
                        }
                        else
                        {
                           actual_ie_length = (byte)(p_message_read[1] + 2);
                        }
                     }
                     else
                     {

                        /*
                         * must be TV or T
                         */

                        actual_ie_length = stored_allowed_ie[
                                             allowed_ie_pointer].length;
                     }


                     ie_pointer = p_message_read;

                     mm_store_ie_info( ie_pointer,
                                      stored_allowed_ie[allowed_ie_pointer].iei,
                                      actual_ie_length,
                                      MANDATORY_IE );

                     stored_allowed_ie[allowed_ie_pointer].found = TRUE;

                     p_message_read += actual_ie_length;

                     if ( p_message_read >
                              ((byte *) message + sizeof(IMH_T) + length ) )
                     {
                        return_value = FALSE;
                        error_reason = MM_INVALID_MANDATORY_INFORMATION;
                     }

                  }

                  break;
            }

            /*
             * Now check the IE syntax
             */
            if(ie_pointer != NULL)
            {
               status = mm_check_ie_syntax(
                              stored_allowed_ie[allowed_ie_pointer].iei,
                              ie_pointer );
            }
            else
            {
               status = FALSE;
            }

            if ( status == FALSE )
            {
               error_reason = MM_INVALID_MANDATORY_INFORMATION;
               return_value = FALSE;
               break;
            }

            allowed_ie_pointer++; /* point to information for next IE */

         }       /* end of imperative part */


         if ( return_value != FALSE ) /* if ok so far */
         {
            while ( ( p_message_read <
                      (byte *) message + sizeof(IMH_T) + length ) &&
                   (return_value != FALSE ))
            {
               allowed_ie = FALSE;
               valid_ie = FALSE;

               for ( this_ie=allowed_ie_pointer;
                     this_ie< ie_counter;
                     this_ie++)

               {
                  if ( stored_allowed_ie[this_ie].iei ==
                     *p_message_read )
                  {
                     allowed_ie = TRUE;

                     if ( stored_allowed_ie[this_ie].found == FALSE)
                     {
                        valid_ie = TRUE;
                        stored_allowed_ie[this_ie].found = TRUE;
                        break;
                     }
                  }
               }

               if ( valid_ie != FALSE )
               {
                  if ( ((*p_message_read) & (byte)(0x80)) != 0 )
                  {

                     /*
                      * Its a type 1 or 2
                      */

                     actual_ie_length = 1;
                  }
                  else
                  {
                     if ( stored_allowed_ie[this_ie].format == FORMAT_TV )
                     {
                        actual_ie_length = stored_allowed_ie[this_ie].length;
                        MSG_ERROR_DS(MM_SUB, "=MM= MM: This is not expected yet...", 0,0,0);
                     }
                     else
                     {

                        /*
                         * check message contains a length field
                         */

                        if ( p_message_read+1 >=
                                 ((byte *) message + sizeof(IMH_T) + length ) )
                        {
                           MSG_ERROR_DS(MM_SUB, "=MM= Bad Conditional IE in message-ignored(%d)", (byte) ((p_message_read+1)  - ((byte *) message + sizeof(IMH_T) + length )),0,0);
                           status = FALSE;
                           actual_ie_length = (byte)length;
                           PUT_IMH_LEN(( p_message_read - (byte*)message - sizeof(IMH_T)),message);
                        }
                        else
                        {
                           actual_ie_length = (byte)( p_message_read[1] + 2);
                        }
                     }
                  }

                  /*
                   * check that all of IE is in message
                   */

                  if ( ( status != FALSE ) &&
                       ( p_message_read + actual_ie_length >
                           ((byte *) message + sizeof(IMH_T) + length ) ) )
                  {
                     MSG_ERROR_DS(MM_SUB, "=MM= Bad Conditional IE in message-(ignored)", 0,0,0);
                     status = FALSE;
                     PUT_IMH_LEN(( p_message_read - (byte*)message - sizeof(IMH_T)),message);
                  }

                  if ( status != FALSE )
                  {
                     status = mm_check_ie_syntax(
                                    stored_allowed_ie[this_ie].iei,
                                    p_message_read );
                  }

                  if ( status != FALSE )
                  {

                     /*
                      * store IE if syntax is ok , otherwise just skip it
                      */

                     mm_store_ie_info( p_message_read,
                                      *p_message_read,
                                      actual_ie_length,
                                      stored_allowed_ie[this_ie].presence );
                  }

                  p_message_read += actual_ie_length;
               }
               else
               if ( allowed_ie != FALSE )   /* check for unexpected repeats */
               {
                  if (((*p_message_read) & (byte)(0x80)) != 0)
                                                         /* type 1 or 2 */
                  {
                     actual_ie_length = 1;
                  }
                  else if ( (p_message_read+1  >= ((byte *) message + sizeof(IMH_T) + length )) ||  
                                                    (((byte*)p_message_read + p_message_read[1] + 2) > ((byte*)message + sizeof(IMH_T) + length))) 
                  {
                     MSG_ERROR_DS(MM_SUB, "=MM= Bad Conditional IE in message-ignored(%d)", (byte) (((byte *) message + sizeof(IMH_T) + length ) - (p_message_read+1)),0,0);
                     actual_ie_length = (byte)length;
                     PUT_IMH_LEN(( p_message_read - (byte*)message - sizeof(IMH_T)),message);
                  }
                  else
                  {
                     actual_ie_length = (byte)( p_message_read[1] + 2);
                  }

                  p_message_read += actual_ie_length;
               }
               else
               {

                  /* not a valie IE */

                  /* check if comprehension required */

                  if (((*p_message_read) & (byte)(0xf0)) == 0 )
                  {
                     error_reason = MM_INVALID_MANDATORY_INFORMATION;
                     return_value = FALSE;
                     break;
                  }
                  else
                  {
                     if (((*p_message_read) & (byte)(0x80)) != 0)
                                                         /* type 1 or 2 */
                     {
                        actual_ie_length = 1;
                     }
                     else if ( (p_message_read+1 >= ((byte *) message + sizeof(IMH_T) + length )) || 
                                                   (((byte*)p_message_read + p_message_read[1] + 2) > ((byte*)message + sizeof(IMH_T) + length)))
                     {
                        MSG_ERROR_DS(MM_SUB, "=MM= Bad Conditional IE in message-ignored(%d)", (byte) (((byte *) message + sizeof(IMH_T) + length ) - (p_message_read+1)),0,0);
                        actual_ie_length = (byte)length;
                        PUT_IMH_LEN(( p_message_read - (byte*)message - sizeof(IMH_T)),message);
                     }
                     else
                     {
                        actual_ie_length = (byte)(p_message_read[1] + 2);
                     }
                  }

                  /*
                   * Skip the unknown IE
                   */

                  p_message_read += actual_ie_length;

               }
            }
         }

         /*
          * if still processing the message, check that all mandatory IEs
          * are present
          */

         if ( return_value != FALSE )
         {
            valid_ie = TRUE;

            for ( this_ie=0;
                  this_ie< ie_counter;
                  this_ie++)
            {
               if ( ( stored_allowed_ie[this_ie].found == FALSE)
                 && ( stored_allowed_ie[this_ie].presence == MANDATORY_IE ) )
               {
                  valid_ie = FALSE;
                  break;
               }
            }

            if ( valid_ie == FALSE )
            {
               error_reason = MM_INVALID_MANDATORY_INFORMATION;
               return_value = FALSE;
            }
         }

         if ( return_value == FALSE )
         {
            if (error_reason != 0 )
            {
               mm_send_status( error_reason );
            }
         }
      }
      else
      {
         mm_send_status( MM_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED );
      }
   }
   return return_value;
}
