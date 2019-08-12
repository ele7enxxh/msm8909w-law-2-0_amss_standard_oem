#ifndef mn_ss_v_h
#define mn_ss_v_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_ss.h_v   1.20   13 May 2002 15:15:36   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn_ss_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

7/12/01    CD      Transferred SS data from mn.h
7/18/01    CD      Moved MAX_COMPONENTS to sys_const.h
7/24/01    CD      Transferred SS operation codes from mn_cm.h
8/06/01    CD      Added ss_invoke_data_T, ss_ForBS_T and registerSS_ARG_T 
              
                   Added MN specific values for ss error

8/8/01     CD      Deleted call_ss_operation_req_T and ss_problem_T since they are
                   no longer used

                   Added network ss codes for possible future use

08/16/01   AB      Renamed MMI references to CM.

08/21/01   CD      Moved external data declarations to mnglobal.h

09/07/01   AB      Restored the network SS code for used in SS unit drivers.

10/08/01   AB      Added include file that contains referenced to its data type per Lint.

10/12/01   AB      Changed hold to Hold to remove conflicting definition w/ the sound
                    task defines.

11/1/01    CD      Fixed lint error

12/11/01   AB      Added component type structure for testing.

01/25/02   CD      Moved some #defs to mn_cm_exp.h per CM/UI request
                   Updated Copyright

04/02/02   AB      Added Context Specific Class for the type tags, and CNAP
                     unit test structure ss_cnap_T.

04/30/02   AB      Added #ifdef FEATURE_MN_UNIT_TEST for structure that
                     used for unit test.  Added Multicall_Status session literal.
                   Adjusted the SS_MAX_USS_DATA_LENGTH to match that of 
                     CM defined value in mn_cm_exp.h 

04/15/03   sbs     Added support for Location Services related SS messages

04/20/05   HS      Added support for FEATURE_CCBS

06/14/05   AB      Increase the SS_MAX_ADD_LOC_EST_LENGTH by 1, per TS23.032,
                     encoding of polygon shape.
===========================================================================*/


#include "mmcp_variation.h"
#include "cc_iei_v.h"
#include "mn_cm_exp_v.h"

#include "cc_state.h"

#include "mn_ss.h"

/* Multicall_Status_Index Session */
#define MN_ACTIVE_HOLD_SESSION          (0x00)
#define MN_HOLD_ACTIVE_SESSION          (0x01)
#define MN_CALL_WAITING_SESSION         (0x02)
#define MN_MAXNO_CALL_SESSION           (0x03)
#define MN_MAXNO_CONNECTION_IN_SESSION  (0x14)
#define MN_INVALID_CALL_SESSION         (0xff)


typedef enum ss_mn_state_e {
    SS_MN_IDLE = 0x00, 
    SS_MN_NOT_IDLE
} ss_mn_state_T;


typedef struct 
{
    byte iei;
    byte length;
    byte coding;
    byte cause;
} ss_cause_ie_T;

typedef struct 
{
    byte ti_pd;
    byte type;
    byte data[MAX_OCTETS_L3_MESSAGE - 2];
} ss_ota_message_T;

/* ss invoke data */

typedef struct
{

/* MO components */

   byte             ss_stored_operation;
   connection_id_T  connection_id;
   byte              cm_ss_ref;
   boolean      ss_password_present;
   ie_ss_password_T ss_password;
   boolean      ss_new_password_present;
   ie_ss_password_T ss_new_password;
   boolean      ss_new_password_again_present;
   ie_ss_password_T ss_new_password_again;

/* MT components */

   byte             mt_invoke_id;
   boolean          mt_invoke_id_valid;
   byte             mt_ss_operation;
   connection_id_T  mt_connection_id;
   byte             mt_cm_ss_ref;
}ss_invoke_data_T;

typedef struct ss_transaction_information_tag
{
   ss_state_T                ss_state;
   /* call_ss_operation_req_T   call_ss_operation_req;  */
   boolean                   offline;

   /* ss_mn_state indicates whether MN has sent a BEGIN_REQ to CNM for SS
    * SET -> When sending MNSS_FACILITY_REQ or MNSS_NEGIN_REQ
    * RESET -> When CNM resets ss_state to IDLE */
   ss_mn_state_T             ss_mn_state;

#if defined(FEATURE_DUAL_SIM )
   sys_modem_as_id_e_type   as_id;
#endif
} ss_transaction_information_T;


/* SS DATA TYPE & CONSTANTS */

/* ASN.1 tag type */

#define   OCTET_STRING       0x04
#define   NUMERIC_STRING     0x12
#define   IA5_STRING         0x16
#define   UNIVERSAL_SEQUENCE 0x10
#define   SEQUENCE_TAG       0x30
#define   SET_TAG            0x31
#define   ENUMERATED         0x0a
#define   UNIVERSAL_INTEGER  0x02
#define   UNIVERSAL_BOOLEAN  0x01
#define   CONTEXT_SPECIFY_PRIMITIVE 0x80
#define   CONTEXT_SPECIFY_CONSTRUCT 0xA0

/* Maximum SS element sizes  */

#define  SS_MAX_SS_DATA_LENGTH           320
#define  SS_MAX_FACILITY_LENGTH          300
#define  SS_MAX_INVOKE_COMPONENT_LENGTH  260
#define  SS_MAX_RETURN_RESULT_LENGTH     260
#define  SS_MAX_PARAMETER_LENGTH         300
#define  SS_MAX_RETURN_ERROR_LENGTH      60
#define  SS_MAX_REJECT_LENGTH            32
#define  SS_MAX_NOTIFY_LENGTH            64
#define  SS_MAX_BSGL_LENGTH              256
#define  SS_MAX_FF_LIST_LENGTH           256
#define  SS_MAX_FF_LENGTH                36
#define  SS_MAX_USS_REQ_LENGTH           MAX_USS_CHAR
#define  SS_MAX_USS_DATA_LENGTH          MAX_SS_USER_DATA_CHAR
#define  SS_MAX_FORWARDING_INFO_LENGTH   256
#define  SS_MAX_CALL_BARRING_LENGTH      200
#define  SS_MAX_CB_FEATURE_LIST_LENGTH   128
#define  SS_MAX_CB_FEATURE_LENGTH        32
#define  SS_MAX_CUG_INFO_LENGTH          64
#define  SS_MAX_CUG_SUB_LIST_LENGTH      128
#define  SS_MAX_CUG_SUB_LENGTH           36
#define  SS_MAX_CUG_FEATURE_LIST_LENGTH  256
#define  SS_MAX_CUG_FEATURE_LENGTH       64
#ifdef FEATURE_CCBS
#define  SS_MAX_CCBS_FEATURE_LIST_LENGTH (SS_MAX_RETURN_RESULT_LENGTH-5)
  /* 5 for comp tag, comp len, invoke tag, invoke len, invoke id */
#define  SS_MAX_CCBS_FEATURE_LENGTH      \
  (  3 /*ccbs-Index*/             +      \
    22 /*b-subscriberNumber*/     +      \
    11 /*b-subscriberNumber*/     +      \
    23 /*b-subscriberSubaddress*/ +      \
     5 /*basicServiceGroup*/               )
#endif /* FEATURE_CCBS */

#if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
#define  SS_MAX_LOC_NOTIFICATION_REQ_LENGTH 300 
#define  SS_MAX_LOC_TYPE_LENGTH          3
#define  SS_MAX_LCS_CLIENT_EXT_ID_LENGTH 300
#define  SS_MAX_LCS_CLIENT_NAME_LENGTH   300
#define  SS_MAX_MOLR_RES_LENGTH          125 
#define  SS_MAX_LOC_ESTIMATE_LENGTH      20
#define  SS_MAX_DECIPHERING_KEYS_LENGTH  15
#define  SS_MAX_ADD_LOC_EST_LENGTH       91
#define  SS_MAX_LCS_NAMESTRING_LENGTH    63
#define  SS_MAX_LCS_EXT_ADDR_LENGTH      20
#define  SS_MAX_SUBS_VIOL_LEN            300 
#define  SS_MAX_SUBS_VIOL_EXT_CONT_LEN   300 
#endif

/* defines for ss_invoke_data */

#define  INVOKE_FAIL        0xff

/************************************************************************
 *                                                                      *
 *                       MNSS Message Identity values                   *
 *                                                                      *
 ************************************************************************
 * To be loaded into MessageId field of Message Header
 */

enum 
{

/* MN to SS primitives */

   MNSS_BEGIN_REQ,
   MNSS_END_REQ,
   MNSS_FACILITY_REQ,

/* SS to MN Primitives */

   MNSS_BEGIN_IND       = 0x80,
   MNSS_END_IND,
   MNSS_FACILITY_IND,
   MNSS_REJ_IND
};

/************************************************************************
 *                                                                      *
 *                       MN to SS PRIMITIVES                            *
 *                       -------------------                            *
 *                                                                      *
 ************************************************************************
 */

/************************************************************************
 *                                                                      *
 *                       MNSS_BEGIN_REQ                                 *
 *                                                                      *
 ************************************************************************
 */


/************************************************************************
 *                                                                      *
 *                       MNSS_END_REQ                                   *
 *                                                                      *
 ************************************************************************
 */


/************************************************************************
 *                                                                      *
 *                       MNSS_FACILITY_REQ                              *
 *                                                                      *
 ************************************************************************
 */




/************************************************************************
 *                                                                      *
 *                       SS to MN PRIMITIVES                            *
 *                       -------------------                            *
 *                                                                      *
 ************************************************************************
*/

/************************************************************************
 *                                                                      *
 *                       MNSS_BEGIN_IND                                 *
 *                                                                      *
 ************************************************************************
 */


/************************************************************************
 *                                                                      *
 *                       MNSS_END_IND                                   *
 *                                                                      *
 ************************************************************************
 */



/************************************************************************
 *                                                                      *
 *                       MNSS_FACILITY_IND                              *
 *                                                                      *
 ************************************************************************
 */


/************************************************************************
 *                                                                      *
 *                       MNSS_REJ_IND                                   *
 *                                                                      *
 ************************************************************************
 */



#ifdef FEATURE_MN_UNIT_TEST

/************************************************************************
*   IE:  Information Element Used for Testing only                                                    *  
*                                                                       *
*************************************************************************/

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
    byte operation_code_tag;
    byte operation_length;
    byte operation_code;
    byte parameters[MAX_SS_PARAMETER_LENGTH/MAX_COMPONENTS];
} ss_invoke_component_T;

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
    byte link_id_tag;
    byte link_id_length;
    byte link_id;
    byte operation_code_tag;
    byte operation_length;
    byte operation_code;
    byte parameters[MAX_SS_PARAMETER_LENGTH /MAX_COMPONENTS];
} ss_invoke_component_1_T;

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
} ss_result_component_T;

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
    byte operation_code_tag;
    byte operation_length;
    byte operation_code;
} ss_result_component_1_T;

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
    byte sequence_tag;
    byte sequence_length;
    byte operation_code_tag;
    byte operation_length;
    byte operation_code;
    byte parameters[MAX_SS_PARAMETER_LENGTH/MAX_COMPONENTS];
} ss_result_component_2_T;

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
    byte error_code_tag;
    byte error_length;
    byte error_code;
    byte parameters[MAX_SS_PARAMETER_LENGTH/MAX_COMPONENTS];
} ss_error_component_T;

typedef struct 
{
    byte component_type;
    byte component_length;
    byte invoke_id_tag;
    byte invoke_length;
    byte invoke_id;
    byte problem_code_tag;
    byte problem_length;
    byte problem_code;
} ss_reject_component_T;

typedef struct 
{
    byte length;
    byte information[sizeof(ss_invoke_component_1_T)];
} ss_lv_facility_ie_T;

typedef struct 
{
    byte iei;
    byte length;
    byte information[sizeof(ss_invoke_component_1_T)];
} ss_tlv_facility_ie_T;


typedef struct 
{
    byte facility_iei;
    byte facility_len;
    byte component_type;
    byte component_len;
    byte invoke_tag;
    byte invoke_len;
    byte invoke_id;
    byte operation_tag;
    byte operation_len;
    byte operation;
    byte sequence_tag;
    byte sequence_len;
    byte ect_ind_tag;
    byte ect_ind_len;
    byte ect_callstate_tag;
    byte ect_callstete_len;
    byte ect_active_alert;
    byte rdn_presentation_allow_tag;
    byte rdn_presentation_allow_len;
    byte party_number_tag;
    byte party_number_len;
    byte party_number[20];
    byte party_number2_tag;
    byte party_number2_len;
    byte party_number2[20];

} ss_notifySS_arg_T ;

typedef struct 
{
    byte facility_iei;
    byte facility_len;
    byte component_type;
    byte component_len;
    byte invoke_tag;
    byte invoke_len;
    byte invoke_id;
    byte operation_tag;
    byte operation_len;
    byte operation;
    byte sequence_tag;
    byte sequence_len;
    byte name_indicator_tag;
    byte name_indicator_len;
    byte name_presentation_tag;
    byte name_presentation_len;
    byte data_coding_tag;
    byte data_coding_len;
    byte data_scheme;
    byte name_len_tag;
    byte name_len_len;
    byte name_len;
    byte name_string_tag;
    byte name_string_len;
    byte name_string[160];
} ss_cnap_T;

#endif /* FEATURE_MN_UNIT_TEST */


#endif
