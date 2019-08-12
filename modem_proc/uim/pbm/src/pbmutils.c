/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER UIM ENTRIES

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM.

  Copyright (c) 2002 - 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmutils.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/16   sp      Fix KW warning
05/26/16   sp      Fix KW warning
05/24/16   sp      Logging improvements
05/11/16   sp      Logging improvements
05/11/16   nr      Logging improvements
05/06/16   nr      Adding support for special chinese character encoding in 0x80
05/05/16   dd      Adding support for special chinese character encoding
05/04/16   lm      Update ECC cache and send ready after NV Refresh
11/25/15   stv     Fix to convert compressed fileds to uncompressed while performing excat match
11/17/15   stv     Logging improvements
08/05/15   nr      Operator specifc NV ECC numbers configuration
09/30/14   NR      Fix to validate the record Id with total number of records in PBC cache
09/18/14   stv     Fix for logging strings in F3s 
09/10/14   NR      Fix to query for WMS ready in case of REFRESH while provisioning 
                   session is ongoing
09/09/14   vr      Reduce stack usage
08/04/14   stv     Check for NULL in return of pbm_get_field_data_ptr
07/31/14   stv     Correcting handling of Supplementary string to have + included in correct
                   position when record is read byclients and correct TON while writing to card
07/07/14    NR     Use v2.0 diag macros
05/01/14    NR     FR 19033: Reduce Code Duplication pbm
03/13/14   stv     KW fix : Possible Array length overflow 
03/06/14   stv     CM SS event handling changes for SGLTE 
03/03/14    NR     Complex field is getting freed before pbm_ascii_to_bcd () 
                   conversion for ANR number and hence user is not able to 
                   save the ANR number 
02/28/14   stv     Fix to retrive number and name correctly through pbm_session_find_location
02/16/14    NR     PBM/QMI_PBM KW Critical warnings
1/23/14    stv     FDN number match successful for any dialed number if FDN list has a record with BCD length 
                   as non-0xFF and actual BCD number is all set to 0xFF
01/21/14    NR     All calls including FDN list numbers are blocked when FDN is
                   Enabled on SIM and TON AND NPI stored as FF in FDN record
01/12/14    NR     TMC header deprecation changes
01/07/13    NR     Possible out of bound access check added for validating the 
                   session type being passed to the function pbm_device_id_to_cache ()
11/27/13  nalavala change the implementation as 8930 in 8X26 as well to display 
                   the record with the valid fields 
11/26/13   nr      QTF CRM MOB fixes
11/14/13   stv     Fix for memory leak while clients read record from PBM 
10/28/13   stv     Write FF to card for name when record doesnt have name field
10/10/13   stv     Fix for KW array bounds
10/04/13   stv     F3 message reduction 
10/03/13  nalavala memcpy banned api replaced with mescpy
10/14/09  krishnac Support for feature DSDS
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
04/01/09   kcp     Fixed CR 169091 - Terminal does not read PB files during power up.
16/03/09   kcp     resolved Lint errors
03/26/08   cvs     Force NULL termination in strncpy()
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
10/24/07   clm     Fix CR125200.  Record id's from external apps should be
                   unaware of special circular cache indexing.
09/05/07   cvs     Change PBM_FIELD_NUMBER_TYPE and PBM_FIELD_NUMBER_PLAN to look for BYTEs
03/12/07   cvs     Featurize 4 PB set support
02/09/07   cvs     Support for 4 phonebook sets
01/16/07   cvs     Add error message
12/07/06   cvs     Lint fixes
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/02/06   cvs     Header file cleanup for WINCE
07/27/06   slw     Differentiated field info for ADN and SIM groups
07/26/06   cvs     Fix compile errors
07/24/06   cvs     Fixes for unequal size multi pb set support
07/12/06   cvs     Fix compile warnings
07/11/06   slw     Added pointer validity check for pbm_fields_convert_str_to_ucs2()
06/19/06   aps     Fix for fields being written as PBM_FT_STRING
06/14/06   aps     support for ANRC and ANRC1 type records
05/31/06   cvs     Fix support of multiple ANR files
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
04/28/06   cvs     fix issue where going from international number to not isn't saved.
04/12/06   cvs     better logging
03/24/06   cvs     %x -> 0x%x in log messages
03/14/06   cvs     fix + at beginning of intl numbers
03/10/06   cvs     Lint fixes
02/28/06   cvs     Add support for MBDN
02/24/06   cvs     Move string functions to pbm_conversion.
02/23/06   cvs     rework memory allocation macros
01/19/06   cvs     Emergency number cleanup
01/09/06   cvs     Lint fixes
12/20/05   cvs     Fix reboot (infinite loop) when adding to a full USIM file
11/16/05   cvs     Lint fixes
11/16/05   cvs     Moved pbm_record_id_to_file_id() to pbmutils.c
11/10/05   cvs     Add network ECC numbers
09/29/05   cvs     Fix usage of PBM_TEXT_SIZE and PBM_MAX_TEXT_LENGTH
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
09/09/05   cvs     Fix pbm_get_name() sorting if first/last names are separate fields
09/01/05   cvs     match_found(): dummy records don't match
08/30/05   cvs     Revert enum_rec_init() behavior of caching names.  Increase field size.
08/26/05   AT      Added new Emergency Group for Searches.
08/24/05   cvs     Minor optimizations to pbm_find_name(), make functions public.
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/20/05   AT      Fixed order of types when searching mixed datatypes.
08/12/05   AT      Added UCS2 support for strings.
08/04/05   cvs     pbm_allocate_index(): Only check PB_SET if we need to.
08/04/05   cvs     pbm_build_record_from_fields(): Move number/TON handling back
                   to end, after we've seen all fields.
08/02/05   cvs     Fix field corruption in pbm_group_fields(), compile warnings
07/29/05   AT      Added some range checking and the feature PBM_MEM_INIT.
                   PBM_MEM_INIT causes pbm_mem_alloc to initialize memory to
                   0's, and pbm_mem_free to 0xCACACACA.  Helps catch memory
                   being used after being freed.
07/11/05   cvs     Better error messages
06/23/05   cvs     Speed up searching, using qsort() for results
06/10/05   cvs     Fix issue where pbm_build_record_from_fields() put junk
                   in the number field.
05/31/05   cvs     Fix return type of pbm_convert_pbm8_to_gsm7()
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/18/05   AT      Updated the string comparison functions to correctly
                   work with pause characters and wild cards.
04/12/05   AT      Fix LINT errors.
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements, including making the ECC
                   read only.
03/22/05   PA      Changed PBM_MAX_PBR_RECS to PBM_MAX_NUM_PB_SETS.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for MSISDN, re-entrant complex field enumeration,
                   greater than enumeration and extended lock support.
01/18/05   PA      Fixed typo in error message
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
12/08/03   PA      Provide EFS field info for PBM_DEFAULT
12/02/03   PA      Record validation utilities
11/19/04   PA      Support non-GSDI builds, code review comments.
11/18/04   PA      Support for complex fields.
11/02/04   PA      Added "999" to list of emergency numbers.
09/29/04   PA      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "cmutil.h"
#include "uim_msg.h"
#include "pbm.h"
#include "pbmutils.h"
#include "pbmefs.h"
#include "pbmgsdi.h"
#include "pbm_conversion.h"
#include "mmgsdilib.h"
#include "stringl/stringl.h"
#include "uim_common_efs.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
uint8 pbm_number_type_to_gsmton[4] = {
0, CM_TON_INTERNATIONAL, CM_TON_NATIONAL, CM_TON_NETWORK_SPECIFIC,
};

static uint8 pbm_number_plan_to_gsmnpi[] = {
0, CM_NPI_ISDN, CM_NPI_DATA, CM_NPI_TELEX,
CM_NPI_PRIVATE
};

char pbm_bcd_to_char_mapping[] = {
'0', '1', '2', '3',
'4', '5', '6', '7',
'8', '9', '*', '#',
/* Time Pause         Wild           Expansion,        Endmark.*/
PBM_TIMEPAUSE_CHAR, PBM_WILD_CHAR,  PBM_EXPANSION_CHAR, '\0'
};

pbm_session_enum_type pbm_session_q[PBM_MAX_SLOTS][PBM_MAX_SESSION_PENDING];

/*pbm_head_q[i] points to the head index in pbm_session_q[i][index]*/
int pbm_head_q[PBM_MAX_SLOTS];
/*pbm_tail_q[i] points to the tail index in pbm_session_q[i][index]*/
int pbm_tail_q[PBM_MAX_SLOTS];
/* curr_q[i] points to the current executing session for slot i+1 */
pbm_session_enum_type curr_q[PBM_MAX_SLOTS];

/*==========================================================================
FUNCTION      pbm_remove_deactivate_sess_from_queue

DESCRIPTION
  Function called to remove the deactivate session for the given queue id.

DEPENDENCIES
  None

SIDE EFFECTS
============================================================================*/
boolean pbm_remove_deactivate_sess_from_queue(pbm_session_enum_type sess,int q_id)
{
  int            loop_index = 0;
  boolean        ret_val    = FALSE;

  do
  {
    if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
    {
      break;
    }

    for ( loop_index = 0 ; loop_index < PBM_MAX_SESSION_PENDING ; loop_index++ )
    {
      if ( pbm_session_q[q_id][loop_index] == sess )
      {
        pbm_session_q[q_id][loop_index] = PBM_SESSION_MAX;
        pbm_head_q[q_id] = ( pbm_head_q[q_id] + 1 ) % PBM_MAX_SESSION_PENDING;
        ret_val = TRUE;
        break;
      }
    }
  }while(0);

  UIM_MSG_HIGH_4("PBM remove deactivate session 0x%x from q_id 0x%x loop_index 0x%x ret_val 0x%x",
                  sess, q_id, loop_index, ret_val);
  return ret_val;
}


/*==============================================================
FUNCTION      pbm_session_q_init

DESCRIPTION
  Function called to Initialises the queue to default values for the given slot id 
  to serilize the PhoneBook  Initlization.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
void pbm_session_q_init(pbm_slot_type slot)
{
  int q_id,loop_index;

  q_id = ((int)slot ) - 1 ;
  if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
  {
    return;
  }

  for ( loop_index = 0 ; loop_index < PBM_MAX_SESSION_PENDING ; loop_index++ )
  {
    pbm_session_q[q_id][loop_index] = PBM_SESSION_MAX ;
  }

  curr_q[q_id]     = PBM_SESSION_MAX ;
  pbm_head_q[q_id] = 0;
  pbm_tail_q[q_id] = 0 ;
}


/*==============================================================
FUNCTION      pbm_is_queue_empty

DESCRIPTION
  Function called to check queue is empty or not.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
boolean pbm_is_queue_empty(int q_id)
{
  boolean ret_val = FALSE;

  do
  {
    if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
    {
      break;
    }

    if ( pbm_head_q[q_id] == pbm_tail_q[q_id] )
    {
      ret_val = TRUE;
    }
  }while(0);

  UIM_MSG_HIGH_2("pbm_is_queue_empty q_id 0x%x ret_val 0x%x", q_id, ret_val);
  return ret_val;
}


/*==============================================================
FUNCTION      pbm_is_queue_full

DESCRIPTION
  Function called to check the queue is full or not.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
boolean pbm_is_queue_full(int q_id)
{
  boolean ret_val = FALSE;

  do
  {
    if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
    {
      break;
    }

    if ( (pbm_tail_q[q_id] - pbm_head_q[q_id]) == (PBM_MAX_SESSION_PENDING - 1) )
    {
      ret_val = TRUE;
    }
  }while(0);

  UIM_MSG_HIGH_2("pbm_is_queue_full q_id 0x%x ret_val 0x%x", q_id, ret_val);
  return ret_val;
}

/*==============================================================
FUNCTION      pbm_if_queue_has_sess

DESCRIPTION
  Function called to search the given session for the given queue id.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
boolean pbm_if_queue_has_sess(pbm_session_enum_type sess,int q_id)
{
  int           loop_index  = 0;
  boolean       ret_val     = FALSE;

  do
  {
    if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
    {
      break;
    }

    for ( loop_index = 0 ; loop_index < PBM_MAX_SESSION_PENDING ; loop_index++ )
    {
      if ( pbm_session_q[q_id][loop_index] == sess )
      {
        ret_val = TRUE;
        break;
      }
    }
 }while(0);

 UIM_MSG_HIGH_3("pbm_if_queue_has_sess q_id 0x%x sess 0x%x ret_val 0x%x",
                 q_id, sess, ret_val);
 return ret_val;
}

/*==============================================================
FUNCTION      pbm_q_dequeue

DESCRIPTION
  Function called to deque the session for the given queue id to 
  serilize the PhoneBook  Initlization.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
pbm_session_enum_type pbm_q_dequeue(int q_id)
{
  pbm_session_enum_type sess = PBM_SESSION_MAX;

  do
  {
    if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
    {
      break;
    }

    if( pbm_is_queue_empty(q_id) )
    {
      break;
    }

    if(pbm_head_q[q_id] < 0 || pbm_head_q[q_id] >= PBM_MAX_SESSION_PENDING)
    {
      break;
    }

    sess  = pbm_session_q[q_id][pbm_head_q[q_id]];
    pbm_session_q[q_id][pbm_head_q[q_id]] = PBM_SESSION_MAX ;
    pbm_head_q[q_id] = ( pbm_head_q[q_id] + 1 ) % PBM_MAX_SESSION_PENDING;
  }while(0);

  UIM_MSG_HIGH_2("PBM pbm_q_dequeue q_id 0x%x sess 0x%x",
                  q_id, sess);
  return sess;
}

/*==============================================================
FUNCTION      pbm_q_enqueue

DESCRIPTION
  Function called to enque the given session for the given queue 
  id to serilize the PhoneBook initlization.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
boolean  pbm_q_enqueue(int q_id,pbm_session_enum_type sess )
{
  boolean ret_val = FALSE;

  do
  {
    if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
    {
      break;
    }

    if(sess < PBM_SESSION_DEFAULT || sess >= PBM_SESSION_MAX)
    {
      break;
    }

    if( pbm_is_queue_full(q_id) )
    {
      break;
    }

    if(pbm_tail_q[q_id] < 0 || pbm_tail_q[q_id] >= PBM_MAX_SESSION_PENDING)
    {
      break;
    }

    pbm_session_q[q_id][pbm_tail_q[q_id]] = sess;
    pbm_tail_q[q_id] = ( pbm_tail_q[q_id] + 1  ) %  PBM_MAX_SESSION_PENDING;
    ret_val = TRUE;
  }while(0);

  UIM_MSG_HIGH_3("PBM pbm_q_enqueue q_id 0x%x sess 0x%x ret_val 0x%x",
                  q_id, sess, ret_val);
  return ret_val;
}

/*==============================================================
FUNCTION      pbm_queue_algorithm

DESCRIPTION
  Function called after getting SUBSCRIPTION READY event from 
  MMGSDI, EVT is received for provisioning 1X, GW apps for which 
  we'll queue the session to sequentilize the PhoneBook initlization.

DEPENDENCIES
  None

SIDE EFFECTS
===============================================================*/
void pbm_queue_algorithm(pbm_session_enum_type sess)
{
  int                   q_id = 0;
  pbm_session_enum_type card_sess;

  q_id      = pbm_session_info[sess].slot_id - 1;
  card_sess = pbm_pb_type_and_slot_to_session_type(
                                  (pbm_slot_type)pbm_session_info[sess].slot_id,
                                    PBM_GPB);
  if( q_id < 0 || q_id >= PBM_MAX_SLOTS )
  {
    return;
  }

  UIM_MSG_HIGH_3("PBM pbm_q_alog q_id 0x%x curr_q[q_id] 0x%x card_sess 0x%x", 
                  q_id, curr_q[q_id], card_sess);
  if (pbm_if_queue_has_sess(sess,q_id) )
  {
    return;  
  }

  if( curr_q[q_id] == PBM_SESSION_MAX )
  {
    (void)pbm_q_enqueue( q_id, sess);
    (void)pbm_q_enqueue( q_id, card_sess);
  }
  else if ( curr_q[q_id] == sess || curr_q[q_id] == card_sess )
  {
    if ( curr_q[q_id] == sess )
    {
      PBM_SESSION_SET(pbm_session_restart,curr_q[q_id]);
      if ( !pbm_if_queue_has_sess(card_sess, q_id) )
      {
        (void)pbm_q_enqueue( q_id, card_sess);
      }
    }

    if ( curr_q[q_id] == card_sess )
    {
      pbm_stop_fileinfo_internal(curr_q[q_id]);
      if ( !pbm_if_queue_has_sess(sess, q_id) )
      {
        (void)pbm_q_enqueue( q_id, sess);
      }

      if (!pbm_if_queue_has_sess(card_sess, q_id) )
      {
        (void)pbm_q_enqueue( q_id, card_sess);
      }

      curr_q[q_id] = PBM_SESSION_MAX;
    }
  }
  else
  {
    if (pbm_if_queue_has_sess(card_sess, q_id) )
    {
      pbm_q_dequeue(q_id);
      pbm_q_enqueue(q_id, sess);
      pbm_q_enqueue(q_id, card_sess);
    }
    else
    {
      pbm_q_enqueue(q_id, sess);
      (void)pbm_q_enqueue( q_id, card_sess);
    }
  }
}


/* =======================    SUPS Parsing related static Functions  ========================== */
static boolean pbm_util_int_find_pos_for_plus_in_ss_string(int num_tokens, void *tokens[],
                                                           pbmutil_sups_start_s_type *ss_start, int *plus_pos);
static boolean pbmutil_tokenize_sups_string(char *in_str, PBMUTIL_TOKEN_INPUT *inputs,
                                            void *tokens[],pbmutil_sups_start_s_type *sups_start,
                                            boolean find_pos, int *plus_pos);
static boolean pbmutil_regex_to_match(char **src_ptr, char **regex_ptr);
static boolean pbmutil_reg_exp_chars_match(boolean special_char, char match_char, char src);
static boolean pbmutil_validate_ss(int num_tokens, void *tokens[], pbmutil_sups_start_s_type *ss_start);
static pbmutil_sups_operation_s_type pbmutil_sups_operation_input[] =
{
  {"**", PBM_SUPS_CMD_REGISTER},
  {"*",  PBM_SUPS_CMD_ACTIVATE},
  {"#",  PBM_SUPS_CMD_DEACTIVATE},
  {"*#", PBM_SUPS_CMD_INTERROGATE},
  {"##", PBM_SUPS_CMD_ERASE},
  {NULL, PBM_SUPS_CMD_NONE}
};

static const pbmutil_sups_sc_table_s_type pbmutil_sups_sc_conversion_table [] =
{
   { PBMUTIL_SUPS_LOCAL_SC_ALL,        pbm_all_ss },
   { PBMUTIL_SUPS_LOCAL_SC_CF,         pbm_allForwardingSS },
   { PBMUTIL_SUPS_LOCAL_SC_CFC,        pbm_allCondForwardingSS },
   { PBMUTIL_SUPS_LOCAL_SC_CFU,        pbm_cfu },
   { PBMUTIL_SUPS_LOCAL_SC_CFB,        pbm_cfb },
   { PBMUTIL_SUPS_LOCAL_SC_CFNRY,      pbm_cfnry },
   { PBMUTIL_SUPS_LOCAL_SC_CFNRC,      pbm_cfnrc },
   { PBMUTIL_SUPS_LOCAL_SC_CLIP,       pbm_clip },
   { PBMUTIL_SUPS_LOCAL_SC_CLIR,       pbm_clir },
   { PBMUTIL_SUPS_LOCAL_SC_COLP,       pbm_colp },
   { PBMUTIL_SUPS_LOCAL_SC_COLR,       pbm_colr },
   { PBMUTIL_SUPS_LOCAL_SC_CW,         pbm_cw },
   { PBMUTIL_SUPS_LOCAL_SC_B,          pbm_allCallRestrictionSS },
   { PBMUTIL_SUPS_LOCAL_SC_OB,         pbm_barringOfOutgoingCalls },
   { PBMUTIL_SUPS_LOCAL_SC_IB,         pbm_barringOfIncomingCalls },
   { PBMUTIL_SUPS_LOCAL_SC_BAOC,       pbm_baoc },
   { PBMUTIL_SUPS_LOCAL_SC_BOIC,       pbm_boic },
   { PBMUTIL_SUPS_LOCAL_SC_BOICEXHC,   pbm_boicExHC },
   { PBMUTIL_SUPS_LOCAL_SC_BAIC,       pbm_baic },
   { PBMUTIL_SUPS_LOCAL_SC_BICROAM,    pbm_bicRoam },
   { PBMUTIL_SUPS_LOCAL_SC_CNAP,       pbm_cnap },
   { PBMUTIL_SUPS_LOCAL_SC_CCBS,       pbm_ccbs }
};


#define PBMUTIL_SUPS_SC_MAX_ENTRY (sizeof(pbmutil_sups_sc_conversion_table)/sizeof(pbmutil_sups_sc_table_s_type))

const pbmutil_sups_bsg_table_s_type pbmutil_sups_bsg_conversion_table[] = {
   { PBMUTIL_SUPS_LOCAL_BSG_ALL,             PBMUTIL_SUPS_BSG_ALL_SERVICES },
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_TS,          PBMUTIL_SUPS_BSG_ALL_TELESERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_TELEPHONY,       PBMUTIL_SUPS_BSG_ALL_SPEECH_TRANSMISSION_SERVCES},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_DATA,        PBMUTIL_SUPS_BSG_ALL_DATA_TELESERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_FAX,             PBMUTIL_SUPS_BSG_ALL_FAX_TRANSMISSION_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_SMS,             PBMUTIL_SUPS_BSG_ALL_SMS_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_TELE_EX_SMS, PBMUTIL_SUPS_BSG_ALL_TELESERVICES_EXCEPT_SMS},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_ALL,   PBMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_TS},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_1,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_1},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_2,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_2 },
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_3,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_3},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_4,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_4},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_5,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_5},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_6,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_6},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_7,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_7},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_8,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_8},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_9,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_9},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_A,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_A},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_B,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_B},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_C,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_C},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_D,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_D},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_E,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_E},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_F,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_F},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_BS,          PBMUTIL_SUPS_BSG_ALL_BEARER_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_ASYNC,       PBMUTIL_SUPS_BSG_ALL_ASYNCHRONOUS_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_SYNC,        PBMUTIL_SUPS_BSG_ALL_SYNCHRONOUS_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_DC_SYNC,     PBMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_SYNCHRONOUS},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_DC_ASYNC,    PBMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_ASYNCHRONOUS},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_DED_PACKET,  PBMUTIL_SUPS_BSG_ALL_DATA_PDS_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_ALL_DED_PAD,     PBMUTIL_SUPS_BSG_ALL_PAD_ACCESS_CA_SERVICES},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_ALL,   PBMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_BS},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_1,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_1 },
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_2,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_2 },
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_3,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_3},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_4,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_4},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_5,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_5},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_6,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_6},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_7,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_7},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_8,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_8},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_9,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_9},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_A,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_A},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_B,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_B},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_C,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_C},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_D,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_D},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_E,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_E},
   { PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_F,     PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_F}
};

#define PBMUTIL_SUPS_BSG_MAX_ENTRY (sizeof(pbmutil_sups_bsg_conversion_table)/sizeof(pbmutil_sups_bsg_table_s_type))

static PBMUTIL_TOKEN_INPUT pbmutil_regex_input[] = {
  {"([*#]+)(\\d+){\\*([\\+\\d]*)\\*(\\d*)\\*(\\d+)\\*(\\d+)}#{(\\+)(\\d+)#}",pbmutil_validate_ss},
  { NULL, NULL }
};


/*===========================================================================*
 *              PHONE BOOK MANAGER INTERNAL UTILITY FUNCTIONS                *
 *              These function are only used internally by PBM               *
 *              and provide general utility services throughout PBM          *
 *===========================================================================*/


/*===========================================================================
FUNCTION MATCH_FIELDS

DESCRIPTION
Local utility function used by match_found() to determine whether the
field id to be searched matches the cache field id.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
static boolean match_fields(pbm_field_id_e_type data_field,
                            pbm_field_id_e_type cache_field,
                            uint32 flags,
                            pbm_field_type_e_type field_type)
{
   if ((flags & PBM_FIELDID_MASK) != PBM_FIELDID_PHONE)
   {
      if ((data_field == PBM_FIELD_NONE) || (data_field == cache_field))
         return TRUE;
      else
         return FALSE;
   } else
   {
      if (field_type == PBM_FT_PHONE || field_type == PBM_FT_BCD)
         return TRUE;
      else
         return FALSE;
   }
}

/*===========================================================================
FUNCTION PBM_UTILS_IS_PHONE_FIELD

DESCRIPTION
  This API tells if the passed field is a phone number type field.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
static boolean pbm_utils_is_phone_field(pbm_field_id_e_type field_id)
{
   if((field_id == PBM_FIELD_PHONE_BUSINESS_FAX)
      ||(field_id ==PBM_FIELD_PHONE_WORK)
      ||(field_id ==PBM_FIELD_PHONE_HOME)
      ||(field_id ==PBM_FIELD_PHONE_FAX)
      ||(field_id ==PBM_FIELD_PHONE_VOICE)
      ||(field_id ==PBM_FIELD_PHONE_PREF)
      ||(field_id ==PBM_FIELD_PHONE_CELL)
      ||(field_id ==PBM_FIELD_PHONE_PAGER)
      ||(field_id ==PBM_FIELD_PHONE_GENERIC)
      ||(field_id ==PBM_FIELD_PHONE_OTHER)
      ||(field_id ==PBM_FIELD_NUMBER_TYPE)
      ||(field_id ==PBM_FIELD_PHONE_BUSINESS_FAX)
      ||(field_id ==PBM_FIELD_DEFAULT_PHONE)
      )
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

/*===========================================================================
FUNCTION MATCH_FOUND

DESCRIPTION
Local utility function used by pbm_enum_rec_init() to determine whether the
given cache entry is a match for the given search criteria. Assumes that
data is null terminated.

DEPENDENCIES

SIDE EFFECTS
=========================================================================*/
boolean match_found(
  pbm_cat_e_type               category,
  pbm_field_id_e_type          field_id,
  const void                  *data_ptr,
  uint16                       data_size,
  uint32                       flags,
  const pbm_addr_cache_s_type *cache_entry
)
{
  int                   j                          = 0;
  int                   data_size_temp             = 0;
  boolean               match                      = FALSE;
  /* queried_number_has_plus signifies if the queried number contains + sign */
  boolean               queried_number_has_plus    = FALSE;
  /* cache_number_contains_ton signifies if the number in PBM cache has GSM TON as international*/
  boolean               cache_number_contains_ton  = FALSE;
  /* valid_ss_string signifies if the queried number is a valid SS string */
  boolean               valid_ss_string            = FALSE;
  /* cache_number_contains_plus signifies if the number in pbm cache contains + sign */
  boolean               cache_number_contains_plus = FALSE;
  boolean               match_numbers              = FALSE;
  pbm_field_s_type     *sfield                     = NULL;
  pbm_return_type       ret                        = PBM_ERROR;
  uint8                 offset_for_cache_number    = 0;
  pbm_field_type_e_type cached_entry_data_type;
  /* PBM cache number is to be send for matching with this offset, it can be 0 or 1
  depending upon whether the number contains + sign or not*/
  byte                  local_number[PBM_MAX_NUM_LENGTH + 2] = {0};
  uint8                 cached_entry_data[PBM_TEXT_SIZE_BYTES];

  PBM_CHECK_PTR_RET(cache_entry, FALSE);
  if (cache_entry->fields == NULL)
  {
    /* We're looking at a dummy entry (deleted from cache, only there in
     * case write fails).  This can't match */
    return FALSE;
  }

  if((category == PBM_CAT_NONE) || (cache_entry->category == (uint16) category))
  {
    if((data_ptr == NULL) && (field_id == PBM_FIELD_NONE))
    {
      /* if data is NULL (and any field) , we only look at category */
      return TRUE;
    }

    if(pbm_utils_is_phone_field(field_id))
    {
      memset(local_number, 0, PBM_MAX_NUM_LENGTH + 2);

      /* max length supported for number field is PBM_MAX_LENGTH +1 is for "+" */
      if ( data_size > (PBM_MAX_NUM_LENGTH + 1))
      {
        return FALSE;
      }

      PBM_CHECK_PTR_RET(data_ptr, FALSE);
      memscpy(local_number, PBM_MAX_NUM_LENGTH + 1, data_ptr, data_size);

      /* passed number contains the + sign */
      if(strstr((char *)local_number,"+"))
      {
        data_size_temp = data_size;
        if(!pbm_util_get_ss_ton_status_of_queried_number(&valid_ss_string,
                                                         &queried_number_has_plus,
                                                         (int *)&data_size_temp,
                                                         local_number))
        {
          return FALSE;
        }
        data_size = data_size_temp;
      }
      else
      {
        /* + sign not present in the number queried,
          hence, match with numbers whose TON is not set */
        queried_number_has_plus = FALSE;
      }

      for (j = 0; j < cache_entry->num_fields; j++)
      {
        pbm_complex_result_s_type *result = NULL;

        ret = pbm_enum_complex_init(&cache_entry->fields[j], NULL, &result);
        if (ret != PBM_SUCCESS)
        {
          UIM_MSG_ERR_2("Failed complex field enumeration ret_val 0x%x for id 0x%x",
                         ret, cache_entry->record_id);
          return FALSE;
        }

        while ((sfield = pbm_enum_complex_next(result)) != NULL)
        {
          cache_number_contains_ton = pbm_util_get_cache_number_status_of_ton(sfield);
        }

        pbm_enum_complex_free(result);
        result = NULL;
      }/* for each field */
    }

    for (j = 0; j < cache_entry->num_fields && !match; j++)
    {
      pbm_complex_result_s_type *result           = NULL;
      uint16                     cached_entry_len = 0;

      offset_for_cache_number = 0;
      ret = pbm_enum_complex_init(&cache_entry->fields[j], NULL, &result);
      if (ret != PBM_SUCCESS)
      {
        UIM_MSG_ERR_2("Failed complex field enumeration 0x%x for id 0x%x",
                       ret, cache_entry->record_id);
        return FALSE;
      }

      while ((sfield = pbm_enum_complex_next(result)) != NULL)
      {
        /* reset the flags */
        cache_number_contains_plus = FALSE;
        match_numbers              = FALSE;

        if (match_fields(field_id,
                         sfield->field_id,
                         flags,
                         sfield->field_type))
        {
          if (data_ptr == NULL)
          {
            match = TRUE;
            break;
          }

          memset(cached_entry_data,0,PBM_TEXT_SIZE_BYTES);
          if (pbm_utils_is_phone_field(sfield->field_id) &&
              sfield->field_type == PBM_FT_BCD)
          {
            byte *bcd_num = (byte*)pbm_get_field_data_ptr(sfield,0);

            if (!pbm_bcd_to_ascii(&bcd_num[2],
                                  bcd_num[0]-1,
                                  bcd_num[1],
                                 (byte *) cached_entry_data))
            {
              UIM_MSG_ERR_1("SIM ERROR record 0x%x could not be converted to ascii", bcd_num[0]-1);
              return FALSE;
            }
            cached_entry_data_type = PBM_FT_PHONE;
            cached_entry_len       = strlen((const char *)cached_entry_data) + 1;
          }
          else if (sfield->field_type == PBM_FT_UCS2_COMPRESSED)
          {
            (void) pbmconvert_sim_to_string((uint8*)pbm_get_field_data_ptr(sfield,0),
                                             sfield->data_len,
                                            (uint16*)cached_entry_data,
                                            PBM_TEXT_SIZE_CHARS);
             cached_entry_data_type = PBM_FT_UCS2;
             cached_entry_len       = 2 * (pbmutils_wstrlen((uint16*)cached_entry_data) + 1);
          }
          else
          {
            memscpy(cached_entry_data,
                    PBM_TEXT_SIZE_BYTES,
                    (uint8*)pbm_get_field_data_ptr(sfield,0),
                    sfield->data_len);
            cached_entry_data_type = PBM_FT_STRING;
            cached_entry_len       = sfield->data_len;
          }

          if (sfield->data_len != 0)
          {
            /* place to check if this number from cache should be checked with the queried
               number or not. Move the big if else condition here */
            if(pbm_utils_is_phone_field(field_id))
            {
              /* since match fields has been done already and only number fields can come in this code leg */
              /* need check for + sign if it is just number field */
              if (cached_entry_data[0] == '+')
              {
                cache_number_contains_plus = TRUE;
                /* since cache number contains the + sign, while matching pbm cache number + 1 is to be send */
                offset_for_cache_number = 1;
              }

              match_numbers = pbm_util_query_to_match_numbers(sfield->field_id,
                                                              queried_number_has_plus,
                                                              cache_number_contains_ton,
                                                              cache_number_contains_plus,
                                                              valid_ss_string);
            }

            if(!pbm_utils_is_phone_field(field_id) ||
                match_numbers)
            {
              if ( (flags & PBM_DATATYPE_MASK) == PBM_DATATYPE_STR)
              {
                /* Number of chars in input */
                uint16 num_chars_given     = 0;
                /* Number of chars in record */
                uint16 num_chars_in_record = 0;
                /* PBM_DATATYPE_STR */
                boolean case_sensitive = ((flags & PBM_CASE_MASK) == PBM_CASE_SENSITIVE);

                /* If the type provided is not UCS2, we need to divide to
                 * get the number of characters in the input data. */
                if ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)
                {
                  num_chars_given = data_size/2 - 1;
                }
                else
                {
                  num_chars_given = data_size - 1;
                }

                /* If the record we found is FT_UCS2, divide to get the
                 * number of characters */
                if (sfield->field_type == PBM_FT_UCS2 || cached_entry_data_type == PBM_FT_UCS2)
                {
                  num_chars_in_record = ((cached_entry_len - offset_for_cache_number)/2) - 1;
                }
                else
                {
                  num_chars_in_record = cached_entry_len - offset_for_cache_number - 1;
                }

                if ((flags & PBM_SEARCHTYPE_MASK) == PBM_SEARCHTYPE_CONTAINS)
                {
                  /* PBM_SEARCHTYPE_CONTAINS */
                  if(pbm_utils_is_phone_field(field_id))
                  {
                    match = (pbm_strstr((void *)(cached_entry_data + offset_for_cache_number),
                                        local_number,
                                        case_sensitive,
                                        (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE,
                                        ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)? TRUE:FALSE) != NULL ?
                                        TRUE : FALSE);
                  }
                  else
                  {
                    match = (pbm_strstr((void*)cached_entry_data,
                                        data_ptr,
                                        case_sensitive,
                                        (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE,
                                        ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)? TRUE:FALSE) != NULL ?
                                        TRUE : FALSE);
                  }
                }
                else
                {
                  /* PBM_SEARCHTYPE_STARTSWITH or PBM_SEARCHTYPE_EXACT */
                  if ((flags & PBM_MATCHING_MASK) == PBM_MATCHING_NORMAL)
                  {
                    /* PBM_MATCHING_NORMAL */
                    if(pbm_utils_is_phone_field(field_id))
                    {
                      match = (pbm_strncmp(local_number,
                                          (void *) (cached_entry_data + offset_for_cache_number),
                                           num_chars_given,
                                           case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                    else
                    {
                      match = (pbm_strncmp(data_ptr,
                                           (void *)cached_entry_data,
                                           num_chars_given, case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                  }
                  else
                  {
                    /* PBM_MATCHING_GREATER */
                    if(pbm_utils_is_phone_field(field_id))
                    {
                      match = (pbm_strncmp(local_number,
                                           (void *)(cached_entry_data + offset_for_cache_number),
                                           num_chars_given,
                                           case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                    else
                    {
                      match = (pbm_strncmp(data_ptr,
                                           (void *)cached_entry_data,
                                           num_chars_given,
                                           case_sensitive,
                                           ((flags & PBM_STRTYPE_MASK) == PBM_STRTYPE_UCS2)?TRUE:FALSE,
                                           (cached_entry_data_type == PBM_FT_UCS2)?TRUE:FALSE) == 0 ?
                                           TRUE : FALSE);
                    }
                  }
                }

                if (match && ((flags & PBM_SEARCHTYPE_MASK) == PBM_SEARCHTYPE_EXACT))
                {
                  if (num_chars_in_record != num_chars_given)
                  {
                    UIM_MSG_MED_2("Excat match failed, num_chars_in_record 0x%x num_chars_given 0x%x",
                                   num_chars_in_record, num_chars_given);
                    match = FALSE;
                  }
                }
              }
              else
              {
                /* PBM_DATATYPE_BYTE, Always EXACT */
                PBM_CHECK_PTR_RET(sfield, FALSE);
                if(pbm_utils_is_phone_field(field_id))
                {
                  match = (memcmp(local_number,
                                  cached_entry_data + offset_for_cache_number,
                                  data_size) == 0 ? TRUE : FALSE);
                }
                else
                {
                  match = (memcmp(data_ptr,
                                  cached_entry_data,
                                  data_size) == 0 ? TRUE : FALSE);
                }
              }
            }/* match_numbers == TRUE */
          } /* if data_len != 0 */
        }  /* if field_id*/
      } /* for each sub field */
      pbm_enum_complex_free(result);
    }  /* for each field */
  }
  return match;
} /* match_found */


/*===========================================================================
FUNCTION PBM_NUMBER_MATCH_INT

DESCRIPTION
  Does an exact match of a phone number.  Wild card characters match
  any digits.  All searches end at the end of either string, or a pause
  character in the SIM string.  These matches require the dialed string
  does not contain a wild card or pause character - these routines are
  meant to validate a string which was given to call manager to see if
  the call is allowed.  If fdn_comparison is TRUE, we do a starts with
  match (extra digits are allowed).

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_number_match_int(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len,
                             boolean fdn_comparison)
{
   int index;                    /* Control variable for loop. */
   boolean ret_val = TRUE;       /* Current return value. */

   /* Validate the parameters. */
   if (sim_num_len == 0 || !sim_num || !dialed_num)
      return FALSE;

   /* For each character, compare the sim_num to the dialed_num. */
   for (index = 0; index < sim_num_len; index++)
   {
      if (sim_num && dialed_num)
      {
         /* TIME and HARDPAUSE characters will end the sim string */
         if ((sim_num[index] == PBM_TIMEPAUSE_CHAR) ||
             (sim_num[index] == PBM_HARDPAUSE_CHAR))
         {
           /* Setting ret_val to FALSE to make the fdn_comparison FALSE,
            * If the PBM_TIME_PAUSE_CHAR is present in first location of the sim_num.
            */
           if(index == 0)
           {
             ret_val = FALSE;
           }

           /* Pause characters make our searches stop there. */
           break;
         }

         /* We have more sim digits, but if we are out of dialed digits,
          * then we return that it is not a match. */
         if (dialed_num_len <= index)
         {
            ret_val = FALSE;
            break;
         }

         /* We have sim digits and dialed digits, if the SIM is a wildcard,
          * don't worry about the dialed character.. It matches wildcards. */
         if (sim_num[index] == PBM_WILD_CHAR)
	{
		if((dialed_num[index]>='0')&&(dialed_num[index]<='9'))
            continue;
		else
		{
			ret_val = FALSE;
			break;
		}
	}

         /* If we reach here, the digits must be compared. */
         if (sim_num[index] != dialed_num[index])
         {
            ret_val = FALSE;
            break;
         }
      } else
      {
         UIM_MSG_ERR_2("sim_num or dialed num NULL, sim_num 0x%x, dialed_num 0x%x", sim_num, dialed_num);
      }
   }

   if (!fdn_comparison)
   {
      /* At this point, index is the actual sim number length.  Make
       * sure this matches the dialed number length, if not, return
       * FALSE.
       */
      if (dialed_num_len != index)
         ret_val = FALSE;
   }

   return ret_val;
}


/*===========================================================================

FUNCTION: pbm_bcd_to_ascii

DESCRIPTION
  Convert the phone number from BCD to ASCII, but checks to make sure
  we don't overwrite memory.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean, if we can do this.

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_bcd_to_ascii(PACKED const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number)
{
   int   bcd_index   = 0;
   int   ascii_index = 0;
   uint8 asc_1 = 0;
   uint8 asc_2 = 0;

   PBM_CHECK_PTR2_RET(bcd_number, ascii_number, FALSE);

   if (bcd_len > (PBM_MAX_NUM_LENGTH / 2))
   {
      UIM_MSG_ERR_1("BCD_LENGTH 0x%x maybe corrupt.", bcd_len);
      return FALSE;
   }

   /*****************************/
   /* International call or not */
   /*****************************/
   if ((ton_npi & CM_TON_MASK) == CM_TON_INTERNATIONAL)
   {
      ascii_number[ascii_index++] = '+';
   }

   /*************************/
   /* Decode BCD into ascii */
   /*************************/
   for ( bcd_index = 0; bcd_index < bcd_len; bcd_index++)
   {
      asc_1 = (bcd_number[bcd_index] & 0x0F);
      asc_2 = (bcd_number[bcd_index] & 0xF0) >> 4;

      ascii_number[ascii_index++] = pbm_bcd_to_char_mapping[asc_1];
      ascii_number[ascii_index++] = pbm_bcd_to_char_mapping[asc_2];
   }

   /* Null terminate the ascii string */
   if (asc_2 != 0x0f)
   {
      ascii_number[ascii_index] = '\0';
   }
   return TRUE;
}

#if !defined(__GNUC__)
boolean pbm_mdn_to_ascii(__packed const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number)
#else
boolean pbm_mdn_to_ascii(PACKED const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number)
#endif /* __GNUC__ */
{
  char pbm_mdn_to_char_mapping[] = {
  '\0', '1', '2', '3', '4',
  '5', '6', '7', '8',  '9',
  '0', '*', '#',
  '\0', '\0',  '\0'
  };

   int bcd_index = 0;
   int ascii_index = 0;
   uint8 asc_1 = 0;
   uint8 asc_2 = 0;

   PBM_CHECK_PTR2_RET(bcd_number, ascii_number, FALSE);

   if (bcd_len > (PBM_MAX_NUM_LENGTH / 2))
   {
      UIM_MSG_ERR_1("BCD_LENGTH 0x%x maybe corrupt.", bcd_len);
      return FALSE;
   }

   /*****************************/
   /* International call or not */
   /*****************************/
   if ((ton_npi & CM_TON_MASK) == CM_TON_INTERNATIONAL)
   {
      ascii_number[ascii_index++] = '+';
   }

   /*************************/
   /* Decode BCD into ascii */
   /*************************/
   for ( bcd_index = 0; bcd_index < bcd_len; bcd_index++)
   {
      asc_1 = (bcd_number[bcd_index] & 0x0F);
      asc_2 = (bcd_number[bcd_index] & 0xF0) >> 4;

      ascii_number[ascii_index++] = pbm_mdn_to_char_mapping[asc_1];
      ascii_number[ascii_index++] = pbm_mdn_to_char_mapping[asc_2];
   }

   /* Null terminate the ascii string */
   if (asc_2 != 0x0f)
   {
      ascii_number[ascii_index] = '\0';
   }
   return TRUE;
}

/*===========================================================================

FUNCTION: pbm_ascii_to_bcd

DESCRIPTION
  Convert the phone number from ASCII to BCD, but checks to make sure
  we don't overwrite memory. Updates number type of bcd number if the ascii
  number starts with a '+'.

DEPENDENCIES
  Boolean, if we can do this.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean pbm_ascii_to_bcd(byte *ascii_number, uint8 type, byte *bcd_number)
{
   int i, j;
   char temp[PBM_MAX_NUM_LENGTH+2] = "";  /* extra + and NULL */
   uint8 bcd_index = BCD_NUM;          /* Index into output bcd_number */
   int num_len = 0;

   PBM_CHECK_PTR_RET(bcd_number, FALSE);
   PBM_CHECK_PTR_RET(ascii_number, FALSE);

   num_len = (int) strlen((char *)ascii_number);
   if (num_len > PBM_MAX_NUM_LENGTH + 1)
   {
      UIM_MSG_ERR_1("Number is too long (%d)", num_len);
      return FALSE;
   }

   /* Null record for deletion */
   if (!num_len)
   {
      bcd_number[BCD_LEN] = 0;
      return TRUE;
   }

   /* Translate or skip over invalid characters */
   for (i = 0, j = 0; i < num_len; i++)
   {
      if (ISDIGIT(ascii_number[i]))
      {
         temp[j++] = ascii_number[i] & 0x0F;
      } else
      {
         switch (ascii_number[i])
         {
            case PBM_TIMEPAUSE_CHAR:
            case PBM_HARDPAUSE_CHAR:
               temp[j++] = PBM_TIMEPAUSE_BCD;
               break;

            case PBM_WILD_CHAR:
               temp[j++] = PBM_WILD_BCD;
               break;

            case PBM_EXPANSION_CHAR:
               temp[j++] = PBM_EXPANSION_BCD;
               break;

            case '#':
               temp[j++] = PBM_HASH_BCD;
               break;

            case '*':
               temp[j++] = PBM_STAR_BCD;
               break;

            default:
               /* Skipping '+' and everything else */
               break;
         }
      }
   }

   if (!j)
   {
      UIM_PBM_MSG_SPRINTF_1("Invalid number (%s)", ascii_number);
      return FALSE;
   }

   /* Odd number of digits must have the 0x0f at the end. */
   if (j & 0x01) temp[j++] = 0x0f;

   /* Now that temp has the bcd codes in natural order... Squish them together
    * and reverse the order per bcd coding.
    */
   for (i = 0; i < j; i += 2)
      bcd_number[bcd_index++] = ((char)(temp[i+1] << 4)) | temp[i];


   bcd_number[BCD_NT_NPI] = 0;
   if (ascii_number[0] == '+')
   {
      bcd_number[BCD_NT_NPI] |= CM_TON_INTERNATIONAL;
   } else
   {
      bcd_number[BCD_NT_NPI] &= ~CM_TON_INTERNATIONAL;
   }

   /* Update Number type/NPI byte */
   bcd_number[BCD_NT_NPI] |= 0x80 | type;

   /* Update length byte */
   bcd_number[BCD_LEN] = (bcd_index - 1);

   return TRUE;
}


/*===========================================================================
FUNCTION PBM_BCD_TO_BIN

DESCRIPTION
  Converts time quantities from bcd to binary.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bcd_to_bin( byte bcd )
{
   return( (bcd >> 4) + (bcd & 0x0F) * 10 );
}


/*===========================================================================
FUNCTION PBM_BIN_TO_BCD

DESCRIPTION
  Converts time quantities from binary to bcd.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bin_to_bcd( byte bin )
{
   return( bin/10 + ((bin % 10) << 4) );
}
/*===========================================================================
FUNCTION PBM_GET_PRIMARY_FIELD_FROM_COMPLEX

DESCRIPTION
  Returns the primary field from a complex field.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_field_id_e_type pbm_get_primary_field_from_complex(const pbm_field_s_type *cfield)
{
  pbm_field_s_type *sfield;
  pbm_complex_result_s_type *result = NULL;
  pbm_return_type ret_val;
  ret_val = pbm_enum_complex_init(cfield, NULL, &result);
  if (ret_val != PBM_SUCCESS)
  {
     UIM_MSG_ERR_1("Failed complex field enumeration %d",
                 ret_val);
     return PBM_FIELD_NONE;
  }
  while ((sfield = pbm_enum_complex_next(result)) != NULL)
  {
    if(sfield->field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
    {
       pbm_enum_complex_free(result);
       return(PBM_FIELD_ADDITIONAL_NUMBER_EXT);
    }

  } /* for each subfield */
  pbm_enum_complex_free(result);
  return PBM_FIELD_NONE;

}

/*===========================================================================
FUNCTION pbm_compress_fields

DESCRIPTION
  Compresses the data from clients to store the text fields to the format as that stored on to SIM 
  Number to BCD format from ASCII

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_compress_fields(const pbm_field_s_type *fields,
                                    int                    num_fields,
                                    pbm_field_s_type       **new_fields)
{
  uint32 *new_rec_sizes = NULL;  uint8 cnt,j;
  pbm_field_type_e_type   compressed_field_type;
  uint8                   generic_number_type = 0;
  uint16                  chars_converted;
  uint8                   *compressed_data = NULL;

  PBM_MEM_ALLOC(new_rec_sizes, num_fields * sizeof(uint32));
  PBM_CHECK_PTR_RET(new_rec_sizes, PBM_ERROR);

  PBM_MEM_ALLOC(compressed_data, PBM_TEXT_SIZE_BYTES * sizeof(uint8));
  if(compressed_data == NULL)
  {
    PBM_MEM_FREEIF(new_rec_sizes); 
    return PBM_ERROR;
  }

  for(cnt=0;cnt<num_fields;cnt++)
  {
    const  pbm_field_s_type *sfield = &fields[cnt];
    uint8  *sfield_data_ptr         = pbm_get_field_data_ptr(fields,cnt);

    if ( sfield_data_ptr == NULL )
    {
       PBM_MEM_FREEIF(new_rec_sizes); 
       UIM_MSG_ERR_0("sfield_data_ptr is NULL");
       PBM_MEM_FREEIF(compressed_data);
       return PBM_ERROR;
    }

    memset(compressed_data,0xFF,PBM_TEXT_SIZE_BYTES);
    if (sfield->field_type == PBM_FT_PHONE)
    {
      if(!pbm_ascii_to_bcd((byte *)sfield_data_ptr,
                             0x80, //unknown type
                             (byte *)compressed_data))
      {
        UIM_MSG_ERR_2("BCD to ASCII erro %d %d", sfield_data_ptr[0], sfield_data_ptr[1]);
        PBM_MEM_FREEIF(new_rec_sizes);
        PBM_MEM_FREEIF(compressed_data);
        return PBM_ERROR;
      }
      new_rec_sizes[cnt] = compressed_data[0] + 1;
    }
    else if (sfield->field_type == PBM_FT_UCS2)
    {
      uint16 ucs_str_len = pbmutils_wstrlen((uint16*)sfield_data_ptr);
      (void) pbmconvert_string_to_sim(sfield_data_ptr,
                                      ucs_str_len,
                                      PBM_FT_UCS2,
                                      compressed_data,
                                      PBM_TEXT_SIZE_BYTES,
                                      &chars_converted,
                                      FALSE);
      if (chars_converted != ucs_str_len)
      {
         UIM_MSG_ERR_2("Did not convert all characters expected %d converted %d",
                     ucs_str_len, chars_converted);
         PBM_MEM_FREEIF(new_rec_sizes);
         PBM_MEM_FREEIF(compressed_data);
         return PBM_ERROR;
      }

      /* Some special characters encoded in 0x81, 0x82 can be FFFF and
         can't compute length using FFFF as delimiter, so handling separately */
      if((sim_coding_mode_enum_type)compressed_data[0] == SIM_CODING_UCS2_HALF_PAGE_1)
      {
        new_rec_sizes[cnt] = compressed_data[1] + UCS81_LEN_OH;
      }
      else if((sim_coding_mode_enum_type)compressed_data[0] == SIM_CODING_UCS2_HALF_PAGE_2)
      {
        new_rec_sizes[cnt] = compressed_data[1] + UCS82_LEN_OH;
      }
      else
      {
        /* we cannot compute the length of the name using wstrlen since "@@" 
           will have 0x0000 in GSM7. which gives an incorrect result with wstrlen */
        for(j = 0; ((uint16*)compressed_data)[j] != 0XFFFF && new_rec_sizes[cnt] <= PBM_MAX_TEXT_LENGTH; j++);
        new_rec_sizes[cnt] = j * 2;

        /* If it is 080 encoding length should be always
           Odd number of bytes if not add the header length explicitly */
        if((sim_coding_mode_enum_type)compressed_data[0] == SIM_CODING_PURE_UCS2 &&
           new_rec_sizes[cnt] % 2 == 0)
        {
          new_rec_sizes[cnt] += UCS80_LEN_OH;
        }
      }
    }
    else
    {
      new_rec_sizes[cnt] = fields[cnt].data_len;
    }

  }

  if (!pbm_allocate_fields_internal(new_fields,
                                    num_fields,
                                    new_rec_sizes))
  {
    PBM_MEM_FREEIF(new_rec_sizes);
    PBM_MEM_FREEIF(compressed_data);
    return PBM_ERROR;
  }

  for(cnt=0;cnt<num_fields;cnt++)
  {
    const pbm_field_s_type *sfield = &fields[cnt];
    uint8 *sfield_data_ptr = pbm_get_field_data_ptr(fields,cnt);

    memset(compressed_data,0xFF,PBM_TEXT_SIZE_BYTES);
    if (sfield->field_type == PBM_FT_PHONE)
    {
      uint8 num_type = 0x80 ;
      /* Mark number to be international to preserve + for ss string */
      if( strstr((const char *)sfield_data_ptr,"+") ) 
      {
        num_type = 0x80 | CM_TON_INTERNATIONAL ; 
      }

      if(!pbm_ascii_to_bcd((byte *)sfield_data_ptr,
                            num_type, 
                             (byte *)compressed_data))
      {
        UIM_MSG_ERR_2("BCD to ASCII erro %d %d", sfield_data_ptr[0], sfield_data_ptr[1]);
        PBM_MEM_FREEIF(new_fields);
        PBM_MEM_FREEIF(new_rec_sizes);
        PBM_MEM_FREEIF(compressed_data);
        return PBM_ERROR;
      }
      compressed_field_type = PBM_FT_BCD;
    }
    else if (sfield->field_type == PBM_FT_UCS2)
    {
      (void) pbmconvert_string_to_sim(sfield_data_ptr,
                                      pbmutils_wstrlen((uint16*)sfield_data_ptr),
                                      PBM_FT_UCS2,
                                      compressed_data,
                                      PBM_TEXT_SIZE_BYTES,
                                      &chars_converted,
                                    FALSE);
      compressed_field_type = PBM_FT_UCS2_COMPRESSED;
    }
    else
    {
      memscpy(compressed_data,PBM_TEXT_SIZE_BYTES,sfield_data_ptr,fields[cnt].data_len);
      compressed_field_type = fields[cnt].field_type;
    }
    pbm_fill_field(&(*new_fields)[cnt], fields[cnt].field_id,
                   compressed_field_type,
                   compressed_data,new_rec_sizes[cnt]);
    if (sfield->field_id == PBM_FIELD_GSM_TONNPI)
    {
      generic_number_type =  compressed_data[0];
    }
    if (sfield->field_id == PBM_FIELD_NUMBER_TYPE)
    {
      generic_number_type =  pbm_number_type_to_gsmton[compressed_data[0]];
    }

  }

  PBM_MEM_FREEIF(new_rec_sizes);

  if (generic_number_type != 0)
  {
    for(cnt=0;cnt<num_fields;cnt++)
    {
      if ((*new_fields)[cnt].field_id == PBM_FIELD_PHONE_GENERIC)
      {
        uint8* temp_data_ptr;
        temp_data_ptr = (uint8*)pbm_get_field_data_ptr((*new_fields),cnt);
        if( temp_data_ptr != NULL)
        {
          temp_data_ptr[1] |= generic_number_type;
        }
      }
    }
  }

  PBM_MEM_FREEIF(compressed_data);
  return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION pbm_convert_to_complex_anr

DESCRIPTION
  Complex ANR conversion.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
static pbm_return_type pbm_convert_to_complex_anr(
  const pbm_field_s_type *fields,
  int                     num_fields,
  pbm_field_s_type      **new_fields,
  int                    *total_fields
)
{
  uint8                        cnt                                          = 0;
  uint8                        index                                        = 0;
  uint8                        no_of_anrs                                   = 0;
  uint8                        fields_cnt                                   = 0;
  uint32                      *new_rec_sizes                                = NULL;
  uint16                       anr_num_fields[PBM_MAX_NUM_SAME_USIM_FILES];
  uint32                       anr_data_sizes[PBM_MAX_NUM_SAME_USIM_FILES][3];
  uint32                       anr_complex_size[PBM_MAX_NUM_SAME_USIM_FILES];
  uint32                       fill_cnt                                     = 0;
  pbm_field_s_type            *anr_fields[PBM_MAX_NUM_SAME_USIM_FILES];
  pbm_sim_record_s_type       *record_ptr                                   = NULL;
  byte                        *anr_data_ptr                                 = NULL;
  uint8                       *anr_number_ptr[PBM_MAX_NUM_SAME_USIM_FILES];
  pbm_return_type              pbm_status                                   = PBM_SUCCESS;

  PBM_CHECK_PTR3_RET(fields, total_fields, new_fields, PBM_ERROR);

  *total_fields = 0;
  memset(anr_num_fields,0,sizeof(anr_num_fields));
  memset(anr_data_sizes,0,sizeof(anr_data_sizes));
  memset(anr_complex_size,0,sizeof(anr_complex_size));
  memset(anr_fields,0,sizeof(anr_fields));
  memset(anr_number_ptr,0,sizeof(anr_number_ptr));

  do
  {
    PBM_MEM_ALLOC(new_rec_sizes, (num_fields+PBM_MAX_NUM_SAME_USIM_FILES) * sizeof(int));
    if (NULL == new_rec_sizes)
    {
      pbm_status = PBM_ERROR;
      break;
    }

    PBM_MEM_ALLOC( record_ptr,sizeof(pbm_sim_record_s_type));
    if (NULL == record_ptr)
    {
      pbm_status = PBM_ERROR;
      break;
    }

    for(index = 0;index < PBM_MAX_NUM_SAME_USIM_FILES; index++)
    {
      PBM_MEM_ALLOC(anr_number_ptr[index], sizeof(uint8)*(PBM_MAX_NUM_LENGTH + 2));
      if (NULL == anr_number_ptr[index])
      {
        pbm_status = PBM_ERROR;
        break;
      }
    }
  }while(0);

  if(pbm_status != PBM_SUCCESS)
  {
    PBM_MEM_FREEIF(new_rec_sizes);
    PBM_MEM_FREEIF(record_ptr);
    for(index = 0;index < PBM_MAX_NUM_SAME_USIM_FILES; index++)
    {
      PBM_MEM_FREEIF(anr_number_ptr[index]);
    }
    return pbm_status;
  }

  for(cnt=0;cnt<num_fields;cnt++)
  {
    if (fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
    {
      /* If number of ANR records is more than 4 then skip */
      if(no_of_anrs >= PBM_MAX_NUM_SAME_USIM_FILES)
      {
        continue;
      }
      pbm_build_record_from_usim_fields(0,&fields[cnt],1,record_ptr);
      anr_data_ptr = record_ptr->data.anr.anr_data;
      if (anr_data_ptr[1] != 0)
      {
        /* Convert the number to ASCII. */
        if (!pbm_bcd_to_ascii(&anr_data_ptr[3],
                              anr_data_ptr[1]-1,
                              anr_data_ptr[2],
                              (byte *) anr_number_ptr[no_of_anrs]))
        {
          UIM_MSG_ERR_0("SIM ERROR record could not be converted to ascii");
        }
      }
      anr_num_fields[no_of_anrs]++;
      anr_data_sizes[no_of_anrs][anr_num_fields[no_of_anrs]-1] = (MIN(strlen((const char *)anr_number_ptr[no_of_anrs]), PBM_MAX_NUM_LENGTH) + 1);
      if(anr_data_ptr[0] != 0)
      {
        anr_num_fields[no_of_anrs]++;
        anr_data_sizes[no_of_anrs][anr_num_fields[no_of_anrs]-1] = 1;
      }
      if(anr_data_ptr[2] != 0)
      {
        anr_num_fields[no_of_anrs]++;
        anr_data_sizes[no_of_anrs][anr_num_fields[no_of_anrs]-1] = 1;
      }
      if (FALSE == pbm_allocate_fields_internal(&anr_fields[no_of_anrs],
            anr_num_fields[no_of_anrs],
            anr_data_sizes[no_of_anrs]))
      {
        pbm_status = PBM_ERROR;
        break;
      }

      fill_cnt = 0;
      pbm_fill_field(&anr_fields[no_of_anrs][fill_cnt],PBM_FIELD_ADDITIONAL_NUMBER,PBM_FT_PHONE,
                     (byte *)anr_number_ptr[no_of_anrs], (uint16) anr_data_sizes[no_of_anrs][fill_cnt]);
      fill_cnt++;

      if(anr_data_ptr[0] != 0)
      {
        pbm_fill_field(&anr_fields[no_of_anrs][fill_cnt],PBM_FIELD_ADDITIONAL_NUMBER_TYPE,
                       PBM_FT_BYTE,(byte *)&anr_data_ptr[0],
                       (uint16) anr_data_sizes[no_of_anrs][fill_cnt]);
        fill_cnt++;
      }
      if(anr_data_ptr[2] != 0)
      {
        pbm_fill_field(&anr_fields[no_of_anrs][fill_cnt],PBM_FIELD_GSM_TONNPI,PBM_FT_BYTE,
                       (byte *)&anr_data_ptr[2], (uint16) anr_data_sizes[no_of_anrs][fill_cnt]);
        fill_cnt++;
      }

      if(anr_num_fields[no_of_anrs] > sizeof(anr_data_sizes[no_of_anrs])/sizeof(anr_data_sizes[no_of_anrs][0]))
      {
        pbm_status = PBM_ERROR;
        break;
      }

      anr_complex_size[no_of_anrs] = pbm_cal_complex_field_size(anr_num_fields[no_of_anrs],anr_data_sizes[no_of_anrs]);
      no_of_anrs++;
    }
    else
    {
      new_rec_sizes[fields_cnt++] = fields[cnt].data_len;
    }
  }

  PBM_MEM_FREEIF(record_ptr);

  if(pbm_status != PBM_SUCCESS)
  {
    PBM_MEM_FREEIF(new_rec_sizes);
    for(index = 0;index < PBM_MAX_NUM_SAME_USIM_FILES; index++)
    {
      PBM_MEM_FREEIF(anr_number_ptr[index]);
    }
    return pbm_status;
  }

  for (cnt = 0; cnt < no_of_anrs; cnt++)
  {
    new_rec_sizes[fields_cnt++] = anr_complex_size[cnt];
    new_rec_sizes[fields_cnt++] = MIN(strlen((const char *)anr_number_ptr[cnt]), PBM_MAX_NUM_LENGTH+2);
  }
  if (!pbm_allocate_fields_internal(new_fields, fields_cnt,
                                    new_rec_sizes))
  {
    PBM_MEM_FREEIF(new_rec_sizes);
    for(index = 0;index < PBM_MAX_NUM_SAME_USIM_FILES; index++)
    {
      PBM_MEM_FREEIF(anr_number_ptr[index]);
    }
    return PBM_ERROR;
  }
  fields_cnt = 0;
  for (cnt = 0; cnt < num_fields ; cnt++)
  {
    if (fields[cnt].field_id != PBM_FIELD_ADDITIONAL_NUMBER_EXT)
    {
      pbm_fill_field(&(*new_fields)[fields_cnt], fields[cnt].field_id, fields[cnt].field_type,
                     (byte *)pbm_get_field_data_ptr(fields,cnt), (uint16)fields[cnt].data_len);
      fields_cnt++;
    }
  }
  for (cnt = 0; cnt < no_of_anrs; cnt++)
  {
    int anr_fields_size = pbm_calculate_size_from_fields(anr_fields[cnt],anr_num_fields[cnt]);
    pbm_fill_complex_field(anr_num_fields[cnt],(byte *)anr_fields[cnt],anr_fields_size,&(*new_fields)[fields_cnt]);

    fields_cnt++;
    PBM_MEM_FREEIF(anr_fields[cnt]);
    pbm_fill_field(&(*new_fields)[fields_cnt], PBM_FIELD_ADDITIONAL_NUMBER, PBM_FT_PHONE,
                    anr_number_ptr[cnt],(uint16)new_rec_sizes[num_fields+cnt]);
    fields_cnt++;
  }
  *total_fields = fields_cnt;
  PBM_MEM_FREEIF(new_rec_sizes);
  for(index = 0;index < PBM_MAX_NUM_SAME_USIM_FILES; index++)
  {
    PBM_MEM_FREEIF(anr_number_ptr[index]);
  }
  return PBM_SUCCESS;
}/* pbm_convert_to_complex_anr */

/*===========================================================================
FUNCTION pbm_uncompress_fields

DESCRIPTION
  Converts the number from BCD format to ascii and text to UCS2 to send to 
  the clients.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_uncompress_fields(const pbm_field_s_type *fields,
                                      int num_fields,
                                      pbm_field_s_type **new_fields,
                                      int *total_fields, boolean skip_text_fields)
{
  pbm_field_s_type              *my_data_buf_with_complex_fields=NULL;
  pbm_field_s_type              *uncompressed_fields=NULL;
  int                           num_fields_with_complex_fields;
  uint32                        *new_rec_sizes = NULL;
  boolean                       converted_to_complex_anr = FALSE;
  uint8                         cnt = 0,valid_field_cnt = 0;
  pbm_return_type               ret = PBM_SUCCESS;
  pbm_field_type_e_type         uncompressed_field_type;
  boolean                       is_sups_string = FALSE ;
  uint8                         *uncompressed_data = NULL;

   for(cnt=0;cnt<num_fields;cnt++)
   {
     /* need to compute the size for extra simple anr,
      * since legacy clients would still be expecting
      * simple additional number field
      */
     if (fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER_EXT)
     {
       /* if atleast EXT anr field , then convert it
        * into complex anr and simple ANR before sending the record to clinet
        */
       if(PBM_SUCCESS != pbm_convert_to_complex_anr((const pbm_field_s_type *)fields,
                                                    num_fields,&my_data_buf_with_complex_fields,
                                                    &num_fields_with_complex_fields))
       {
         UIM_MSG_ERR_0("Can't copy fields");
         ret = PBM_ERROR;
         goto END_PBM_FUNC;
       }
       else
       {
         converted_to_complex_anr = TRUE;
         break;
       }
     }
   }

   if(converted_to_complex_anr)
   {
     fields = my_data_buf_with_complex_fields;
     num_fields = num_fields_with_complex_fields;
   }

   PBM_MEM_ALLOC(new_rec_sizes, num_fields * sizeof(uint32));
   if(new_rec_sizes == NULL)
   {
     UIM_MSG_ERR_1("no memory %d ", num_fields * sizeof(uint32));
     goto END_PBM_FUNC;
   }

   PBM_MEM_ALLOC(uncompressed_data, PBM_TEXT_SIZE_BYTES * sizeof(uint8));
   if(uncompressed_data == NULL)
   {
     UIM_MSG_ERR_1("no memory %d ", PBM_TEXT_SIZE_BYTES * sizeof(uint8));
     goto END_PBM_FUNC;
   }

   for(cnt=0;cnt<num_fields;cnt++)
   {
     const pbm_field_s_type     *sfield = &fields[cnt];
     uint8                      *sfield_data_ptr = pbm_get_field_data_ptr(fields,cnt);

     if( sfield_data_ptr == NULL ) 
     {
        ret = PBM_ERROR;
        goto END_PBM_FUNC;
     }

     memset(uncompressed_data,0,PBM_TEXT_SIZE_BYTES);
     if (sfield->field_type == PBM_FT_BCD)
     {
       if(!pbm_bcd_to_ascii((byte *)&sfield_data_ptr[2],
                              sfield_data_ptr[0]-1,
                              sfield_data_ptr[1],
                              (byte *)uncompressed_data))
       {
         UIM_MSG_ERR_2("BCD to ASCII erro %d %d",sfield_data_ptr[0],sfield_data_ptr[1]);
         continue;
       }

       //append NULL char
       new_rec_sizes[valid_field_cnt] = 1 + strlen((const char *)uncompressed_data);
       valid_field_cnt++;
     }
     else if (sfield->field_type == PBM_FT_UCS2_COMPRESSED && !skip_text_fields )
     {
       (void) pbmconvert_sim_to_string(sfield_data_ptr,
                                    sfield->data_len,
                                    (uint16 *)uncompressed_data,
                                     PBM_TEXT_SIZE_BYTES);

       //append NULL char
       new_rec_sizes[valid_field_cnt] = 2*(1 + pbmutils_wstrlen((uint16*)uncompressed_data));
       valid_field_cnt++;
     }
     else
     {
       new_rec_sizes[valid_field_cnt] = sfield->data_len;
       valid_field_cnt++;
     }
   }

   if (!pbm_allocate_fields_internal(&uncompressed_fields,valid_field_cnt,new_rec_sizes))
   {
      ret = PBM_ERROR;
      goto END_PBM_FUNC;
   }

   valid_field_cnt = 0;
   for(cnt=0;cnt<num_fields;cnt++)
   {
     const pbm_field_s_type       *sfield = &fields[cnt];
     uint8                        *sfield_data_ptr = pbm_get_field_data_ptr(fields,cnt);

     memset(uncompressed_data,0,PBM_TEXT_SIZE_BYTES);
     if (sfield->field_type == PBM_FT_BCD)
     {
       int pos_plus,i;
       if(!pbm_bcd_to_ascii((byte *)&sfield_data_ptr[2],
                              sfield_data_ptr[0]-1,
                              sfield_data_ptr[1],
                              (byte *)uncompressed_data))
       {
         UIM_MSG_ERR_2("BCD to ASCII erro %d %d",sfield_data_ptr[0],sfield_data_ptr[1]);
         continue;
       }

       if( uncompressed_data[0] == '+' )
       {
         if ( pbm_util_is_valid_sups_string( (char *)(uncompressed_data+1),
                                             strlen((const char *)(uncompressed_data+1)),
                                             &pos_plus,
                                             FALSE) )
         {
            /* if the number is valid SS string */
            if( pbm_util_is_valid_sups_string((char *)(uncompressed_data+1),
                                              strlen((const char *)(uncompressed_data+1)),
                                              &pos_plus, 
                                              TRUE)  )
           {
              is_sups_string = TRUE ;
              /* Place + in the correct position in the string */ 
              for (i=0;i<pos_plus;i++)
              {
                 uncompressed_data[i] = uncompressed_data[i+1];
              }
              uncompressed_data[i] = '+';
           }
           else
           {
              UIM_MSG_ERR_0("Could not find position of plus in sups string");
           }
         }
       }
       uncompressed_field_type = PBM_FT_PHONE;
     }
     else if (sfield->field_type == PBM_FT_UCS2_COMPRESSED && !skip_text_fields)
     {
       (void) pbmconvert_sim_to_string(sfield_data_ptr,
                                    sfield->data_len,
                                    (uint16*)uncompressed_data,
                                     PBM_TEXT_SIZE_BYTES);
       uncompressed_field_type = PBM_FT_UCS2;
     }
     else
     {
       memscpy(uncompressed_data,PBM_TEXT_SIZE_BYTES,sfield_data_ptr,sfield->data_len);
       uncompressed_field_type = sfield->field_type;
     }

     pbm_fill_field(&uncompressed_fields[valid_field_cnt], 
                    sfield->field_id,
                    uncompressed_field_type,
                    uncompressed_data,new_rec_sizes[valid_field_cnt]);
     valid_field_cnt++;
   }

   if ( is_sups_string ) 
   {
      UIM_MSG_HIGH_0(" Found sup string in record ");
      for ( cnt = 0 ; cnt < num_fields ; cnt++ )
      {
          uint8 *sfield_data_ptr = pbm_get_field_data_ptr(uncompressed_fields,cnt) ; 
          if( sfield_data_ptr != NULL )
          {
            if ( uncompressed_fields[cnt].field_id ==  PBM_FIELD_NUMBER_TYPE ) 
            {
               *sfield_data_ptr =  0; 
            }
            if ( uncompressed_fields[cnt].field_id ==  PBM_FIELD_GSM_TONNPI ) 
            {
               *sfield_data_ptr &= 0x8F ; 
            }
         }
     }
   }

  *new_fields   = uncompressed_fields;
  *total_fields = valid_field_cnt;

  END_PBM_FUNC:
  if(converted_to_complex_anr)
    PBM_MEM_FREEIF(my_data_buf_with_complex_fields);
  PBM_MEM_FREEIF(new_rec_sizes);
  PBM_MEM_FREEIF(uncompressed_data);
  return ret;
}

/*===========================================================================
FUNCTION pbm_convert_to_anr_ext

DESCRIPTION
Converts the fields COMPLEX or simple ANR to ANR_EXT fields 

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_convert_to_anr_ext(const pbm_field_s_type *fields,
									   int num_fields,
									   pbm_field_s_type **new_fields)
{
  pbm_field_s_type *sfield;
  pbm_complex_result_s_type *result = NULL;
  pbm_return_type ret_val;
  uint8 cnt;
  uint8 no_of_anrs = 0;
  uint32 *new_rec_sizes = NULL;
  byte anr_data[PBM_MAX_NUM_SAME_USIM_FILES][4+(2*PBM_SIM_DIALING_NUMBER_LEN)];

  PBM_MEM_ALLOC(new_rec_sizes, num_fields * sizeof(int));
  PBM_CHECK_PTR_RET(new_rec_sizes, FALSE);
  memset(anr_data,0,sizeof(anr_data));

  for(cnt=0;cnt<num_fields;cnt++)
  {
      if ( ( fields[cnt].field_id == PBM_FIELD_COMPLEX ||
          fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER)
             && no_of_anrs < PBM_MAX_NUM_SAME_USIM_FILES )
      {
        byte *anr_number_ptr = NULL;
        byte anr_type = 0;
        byte anr_id = 0;
        if (fields[cnt].field_id == PBM_FIELD_COMPLEX)
        {
          ret_val = pbm_enum_complex_init(&fields[cnt], NULL, &result);
          if (ret_val != PBM_SUCCESS)
          {
             UIM_MSG_ERR_1("Failed complex field enumeration %d ",
                         ret_val);
             PBM_MEM_FREEIF(new_rec_sizes);
             return ret_val;
          }
          while ((sfield = pbm_enum_complex_next(result)) != NULL && (no_of_anrs < PBM_MAX_NUM_SAME_USIM_FILES ))
          {

            if(sfield->field_id == PBM_FIELD_ADDITIONAL_NUMBER)
            {
              anr_number_ptr = (byte *)pbm_get_field_data_ptr(sfield,0);
            }
            if(sfield->field_id == PBM_FIELD_ADDITIONAL_NUMBER_TYPE)
            {
              anr_id = *(byte *)pbm_get_field_data_ptr(sfield,0);
            }
            if(sfield->field_id == PBM_FIELD_GSM_TONNPI)
            {
              anr_type = *(byte *)pbm_get_field_data_ptr(sfield,0);
            }

          } /* for each subfield */
        }
        else if(fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER)
        {
          anr_number_ptr = (byte *)pbm_get_field_data_ptr(fields,cnt);
        }
        if(NULL != anr_number_ptr)
        {
          if (!pbm_ascii_to_bcd(anr_number_ptr,
                                 anr_type,&anr_data[no_of_anrs][1]))
          {
            UIM_MSG_ERR_0("SIM ERROR record could not be converted to ascii");
          }
          anr_data[no_of_anrs][0] = anr_id;
          new_rec_sizes[cnt] = 2+anr_data[no_of_anrs][1];
          no_of_anrs++;
        }

        if(result != NULL)
        {
          pbm_enum_complex_free(result);
          result = NULL;
        }
      }
      else
      {
        new_rec_sizes[cnt] = fields[cnt].data_len;
      }
    }

    if (!pbm_allocate_fields_internal(new_fields, num_fields,
                                      new_rec_sizes))
    {
       PBM_MEM_FREEIF(new_rec_sizes);
       return PBM_ERROR;
    }

    no_of_anrs = 0;
    for (cnt = 0; cnt < num_fields; cnt++)
    {
      if ( (fields[cnt].field_id == PBM_FIELD_COMPLEX ||
          fields[cnt].field_id == PBM_FIELD_ADDITIONAL_NUMBER)
            && no_of_anrs < PBM_MAX_NUM_SAME_USIM_FILES )
      {
            pbm_fill_field(&(*new_fields)[cnt], PBM_FIELD_ADDITIONAL_NUMBER_EXT, PBM_FT_BCD,
                           (byte *)anr_data[no_of_anrs], (uint16)new_rec_sizes[cnt]);
            no_of_anrs++;
      }
      else
      {
        pbm_fill_field(&(*new_fields)[cnt], fields[cnt].field_id, fields[cnt].field_type,
                       (byte *)pbm_get_field_data_ptr(fields,cnt), (uint16)fields[cnt].data_len);
      }
    }
    PBM_MEM_FREEIF(new_rec_sizes);
    return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_FREE_FIELDS_INTERNAL

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields_internal.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields_internal( pbm_field_s_type **fields, int num_fields)
{
   PBM_CHECK_PTR_RET(fields, VOID);

   if ((num_fields > 0) && (*fields != NULL))
      PBM_MEM_FREEIF(*fields);
   *fields = NULL;
}


/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS_INTERNAL

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the PBM heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields_internal(pbm_field_s_type **fields_buf_ptr, uint32 num_fields, const uint32 *sizes)
{
   uint32 i;
   uint32 total_size = 0;
   uint32 mem_offset = 0;
   const uint32 size_one_field = sizeof(pbm_field_s_type);

   PBM_CHECK_PTR2_RET(fields_buf_ptr, sizes, FALSE);
   if( num_fields == 0 )
   {
     UIM_MSG_ERR_0("Invalid num_fields for allocate_internal ");
     return FALSE;
   }

   *fields_buf_ptr = NULL;
   total_size = ALIGNED_SIZE (num_fields * size_one_field);
   for (i = 0; i < num_fields; i++)
   {
      total_size += ALIGNED_SIZE (sizes[i]);
   }

   if (total_size > 0)
   {
      PBM_MEM_ALLOC(*fields_buf_ptr, total_size);
      if (*fields_buf_ptr == NULL)
      {
         return FALSE;
      }

      mem_offset = ALIGNED_SIZE(num_fields * size_one_field);
      for (i = 0; i < num_fields; i++)
      {
         // Each offset is from the start of the particular field
         // to the data associated with this field.
         // Thus, the first offset points to the start of the
         // data part of the buffer, and then each subsequent offset
         // is incremented by the size of the previous items data minus
         // the size of the previous items field header.  By doing
         // this, each field can be handled in a self contained way, and
         // accessing the data does not require knowledge of the entire
         // field memory buffer.
         (*fields_buf_ptr)[i].buffer_offset = mem_offset;
         mem_offset += ALIGNED_SIZE(sizes[i]);
         mem_offset -= size_one_field; /* take #bytes from next field to its data */
      }

   }
   else
   {
     return FALSE;
   }

   return TRUE;
}


/*===========================================================================
FUNCTION PBM_COPY_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the destination field array
  and copies the data from the source field array.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_fields(pbm_field_s_type **dst,
                        pbm_field_s_type const * const * src,
                        uint32 num_fields)
{
   uint32 total_size = 0;

   PBM_CHECK_PTR3_RET(dst, src, *src, FALSE);

   total_size = (uint32) pbm_calculate_size_from_fields(*src, (int) num_fields);

   PBM_MEM_ALLOC(*dst, total_size);
   if (*dst == NULL)
   {
      return FALSE;
   }

   memscpy (*dst, total_size, *src, total_size);

   return TRUE;
}


/*===========================================================================
FUNCTION PBM_BUILD_NUM_TEXT_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_record_s_type record expected by
  the old software.

  It only supports one name field, PBM_FIELD_NAME, however
  it does support all the number fields.  For consistency in this interface,
  the order of the phone numbers in the field does not matter, the number
  returned will be the one with the highest enumerated type.  This was
  chosen so that the GSM number will be favored if multiple numbers were
  stored.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_num_text_record_from_fields(pbm_field_s_type *fields, int item_count,
                                           int index, pbm_num_text_record_s_type *record)
{
   int i;
   int filled_in_number = -1; /* Remember if we put a number in there. */
   pbm_return_type ret;
   pbm_field_s_type *sfield;
   uint8 number_type=0;

   PBM_CHECK_PTR2_RET(fields, record, VOID);

   /* Fill in the record with default values. */
   memset(record, 0, sizeof(pbm_num_text_record_s_type));
   memset(record->sim_text, 0xFF, sizeof(record->sim_text));
   record->bcd_number[1] = 0x80 | CM_TON_UNKNOWN | CM_NPI_ISDN ;

   /* Get the records_index. */
   record->index = index;
   for (i = 0; i < item_count; i++)
   {
      pbm_complex_result_s_type *result = NULL;
      if( fields[i].field_id == PBM_FIELD_COMPLEX)
	      continue;

      ret = pbm_enum_complex_init(&fields[i], NULL, &result);
      if (ret != PBM_SUCCESS)
      {
         UIM_MSG_ERR_1("Failed complex field enumeration %d", ret);
         return;
      }

      while ((sfield = pbm_enum_complex_next(result)) != NULL)
      {
         PBM_CHECK_PTR_RET(sfield, VOID);
         switch (sfield->field_id)
         {
            case PBM_FIELD_NAME:
               memscpy(record->sim_text,(sfield->data_len >= sizeof(record->sim_text))?
		       sizeof(record->sim_text) - 1:sfield->data_len,pbm_get_field_data_ptr(sfield,0),
                      (sfield->data_len >= sizeof(record->sim_text))?
                      sizeof(record->sim_text) - 1:sfield->data_len);
               break;
            case PBM_FIELD_PHONE_WORK:
            case PBM_FIELD_PHONE_HOME:
            case PBM_FIELD_PHONE_FAX:
            case PBM_FIELD_PHONE_VOICE:
            case PBM_FIELD_PHONE_PREF:
            case PBM_FIELD_PHONE_CELL:
            case PBM_FIELD_PHONE_PAGER:
            case PBM_FIELD_PHONE_GENERIC:
            case PBM_FIELD_PHONE_OTHER:
            case PBM_FIELD_HARDCODED_ECC:
            case PBM_FIELD_NV_ECC:
            case PBM_FIELD_SIM_ECC:
            case PBM_FIELD_NETWORK_ECC:
               if (((int) sfield->field_id > filled_in_number) &&
                   (filled_in_number != PBM_FIELD_PHONE_GENERIC))
               {
                  if (sfield->field_type == PBM_FT_BCD)
                  {
                    filled_in_number = (int) sfield->field_id;
                    /* copy the number, so it doesn't get overwritten later */
                    memset(record->bcd_number, 0, sizeof(record->bcd_number));
                    memscpy(record->bcd_number, sizeof(record->bcd_number), pbm_get_field_data_ptr(sfield,0),
                           sfield->data_len);

                  }
                  else
                  {
                    UIM_MSG_ERR_2("Wrong field type found 0x%x for field 0x%x",
                                sfield->field_type, sfield->field_id);
                  }

               }
               break;
            case PBM_FIELD_GSM_TONNPI:
               if (sfield->field_type != PBM_FT_BYTE)
               {
                  UIM_MSG_ERR_3("Wrong field type found 0x%x at index %d expected 0x%x",
                              sfield->field_type, i, PBM_FT_BYTE);
               }
               number_type = *((byte *) pbm_get_field_data_ptr(sfield,0));
               break;
            case PBM_FIELD_NUMBER_TYPE:
               if (sfield->field_type != PBM_FT_BYTE)
               {
                  UIM_MSG_ERR_3("Wrong field type found 0x%x at index %d expected 0x%x",
                              sfield->field_type, i, PBM_FT_BYTE);
               }
               if (*(uint8 *)pbm_get_field_data_ptr(sfield,0) < ARR_SIZE(pbm_number_type_to_gsmton))
                  record->bcd_number[1] |= pbm_number_type_to_gsmton[*(uint8 *)pbm_get_field_data_ptr(sfield,0)];
               break;
            case PBM_FIELD_NUMBER_PLAN:
               if (sfield->field_type != PBM_FT_BYTE)
               {
                  UIM_MSG_ERR_3("Wrong field type found 0x%x at index %d expected 0x%x",
                              sfield->field_type, i, PBM_FT_BYTE);
               }
               if (*(uint8 *)pbm_get_field_data_ptr(sfield,0) < ARR_SIZE(pbm_number_plan_to_gsmnpi))
                  record->bcd_number[1] |= pbm_number_plan_to_gsmnpi[*(uint8 *)pbm_get_field_data_ptr(sfield,0)];
               break;
            case PBM_FIELD_DATE:
               if (sfield->field_type != PBM_FT_DWORD)
               {
                  UIM_MSG_ERR_3("Wrong field type found 0x%x at index %d expected 0x%x",
                              sfield->field_type, i, PBM_FT_DWORD);
               }
               record->time = *((uint32 *) pbm_get_field_data_ptr(sfield,0));
               break;
            case PBM_FIELD_DURATION:
               if (sfield->field_type != PBM_FT_DWORD)
               {
                  UIM_MSG_ERR_3("Wrong field type found 0x%x at index %d expected 0x%x",
                              sfield->field_type, i, PBM_FT_DWORD);
               }
               record->duration = *((uint32 *) pbm_get_field_data_ptr(sfield,0));
               break;
            case PBM_FIELD_INDEX:
               if (*((uint16 *)pbm_get_field_data_ptr(sfield,0)) != index)
               {
                  int field_index = *(uint16 *)pbm_get_field_data_ptr(sfield,0);
                  UIM_MSG_ERR_2("Given index %d does not match with index in fields"
                              "%d", index, field_index);

               }
               break;

            default:    /* Ignore other fields. */
               break;
         }
      } /* for each sub field */
      pbm_enum_complex_free(result);
   } /* for each field */
   record->bcd_number[1] |= number_type;
   return;
}


/*===========================================================================
FUNCTION PBM_BUILD_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_record_s_type record expected by
  the old software.

  It only supports one name field, PBM_FIELD_NAME, however
  it does support all the number fields.  For consistency in this interface,
  the order of the phone numbers in the field does not matter, the number
  returned will be the one with the highest enumerated type.  This was
  chosen so that the GSM number will be favored if multiple numbers were
  stored.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_fields(pbm_field_s_type *fields, int item_count,
                                  int index, pbm_record_s_type *record)
{
   pbm_num_text_record_s_type *new_record_ptr = NULL;

   PBM_CHECK_PTR2_RET(fields, record, VOID);

   PBM_MEM_ALLOC( new_record_ptr,sizeof(pbm_num_text_record_s_type));
   PBM_CHECK_PTR_RET(new_record_ptr,VOID);

   pbm_build_num_text_record_from_fields(fields, item_count, index, new_record_ptr);

   /* Fill in the record with default values. */
   memset(record, 0, sizeof(pbm_record_s_type));

   /* Copy in the fields from new_record to record */
   record->index = new_record_ptr->index;

   if (!pbm_bcd_to_ascii((byte *)&new_record_ptr->bcd_number[2] , new_record_ptr->bcd_number[0] - 1 ,
                          new_record_ptr->bcd_number[1] , (byte*)record->number ))
   {
     UIM_MSG_ERR_1("SIM ERROR record %d could not be converted to ascii", index);
   }
   record->type |= new_record_ptr->bcd_number[1];
   record->time        = new_record_ptr->time;
   record->duration    = new_record_ptr->duration;
   record->is_answered = new_record_ptr->is_answered;
   {
      uint16 *ucs2_text_field_ptr = NULL ;
      uint16 len=0;

      PBM_MEM_ALLOC( ucs2_text_field_ptr , PBM_TEXT_SIZE_BYTES );

      if( ucs2_text_field_ptr != NULL ) 
      {
      /* calculate length correctly strlen cannot be used as 00 is @ in GSM7 encoding */
        while ( new_record_ptr->sim_text[len++] != 0xFF  && len < PBM_TEXT_SIZE_CHARS ) ;
   
        pbmconvert_sim_to_string(new_record_ptr->sim_text ,len , 
	                         ucs2_text_field_ptr , PBM_TEXT_SIZE_BYTES   ) ; 
						   
        (void) pbmconvert_ucs2_to_pbm8(ucs2_text_field_ptr,
                                       pbmutils_wstrlen(ucs2_text_field_ptr),
                                      (uint8 *) record->text, sizeof(record->text));
        PBM_MEM_FREEIF(ucs2_text_field_ptr);
      }
   }
   /* free this buffer always (though could not allocate for ucs2_text_field_ptr) */
   PBM_MEM_FREEIF (new_record_ptr);
   return;
}


/*===========================================================================
FUNCTION PBM_BUILD_FIELDS_FROM_RECORD

DESCRIPTION
  This function is used for the legacy API.  It takes a pbm_record_s_type
  record and builds a record that contains an array of fields expected by
  the new interface.

===========================================================================*/
pbm_addr_cache_s_type * pbm_build_fields_from_record (pbm_phonebook_type pb_id,
                                                      pbm_record_s_type *record)
{
   pbm_addr_cache_s_type *node;
   uint32 field_lengths[6];            /* Sim records have at most 5 fields */
   uint32 build_field_lengths[6];      /* build_field_lengths are the lengths actually to build */
   PB_CACHE *cache = pbm_pb_id_to_cache(pb_id);
   uint16 index = 0;
   uint16 location = 0;
   uint16 *temp_ucs2_str=NULL;
   int j = 0;

   memset(field_lengths, 0, sizeof(field_lengths));
   memset(build_field_lengths, 0, sizeof(build_field_lengths));

   PBM_CHECK_PTR2_RET(record, cache, NULL);

   PBM_MEM_ALLOC(node, sizeof(pbm_addr_cache_s_type));
   if (node)
   {
      memset((byte *)node, 0, sizeof(pbm_addr_cache_s_type));
      switch (pb_id.device_type)
      {
         case PBM_ADN:
            node->category = (uint16) PBM_CAT_SIM_ADN;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;

         case PBM_SDN:
            node->category = (uint16) PBM_CAT_SIM_SDN;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;

         case PBM_MSISDN:
            node->category = (uint16) PBM_CAT_SIM_MSISDN;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;

         case PBM_GRPNAME:
            node->category = (uint16) PBM_CAT_GRPNAME;
            node->num_fields = 1;
            break;
         case PBM_AAS:
            node->category = (uint16) PBM_CAT_AAS;
            node->num_fields = 1;
            break;
         case PBM_MBDN:
            node->category = (uint16) PBM_CAT_SIM_MBDN;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;

         case PBM_MBN:
            node->category = (uint16) PBM_CAT_SIM_MBN;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;

         case PBM_FDN:
            node->category = (uint16) PBM_CAT_SIM_FDN;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;

         case PBM_SDL:
            node->category = (uint16) PBM_CAT_SDL;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;
         case PBM_EFS:
            node->category = (uint16) PBM_CAT_NONE;
            node->num_fields = PBM_SIM_NUM_FIELDS;
            break;
         case PBM_DIALED:
         case PBM_OCI:
         case PBM_LND:
            node->category = (uint16) PBM_CAT_OUTGOING;
            node->num_fields = PBM_CALLHISTORY_NUM_FIELDS;
            break;
         case PBM_RCVD:
         case PBM_ICI:
            node->category = (uint16) PBM_CAT_INCOMING;
            node->num_fields = PBM_CALLHISTORY_NUM_FIELDS;
            break;
         case PBM_MISS:
            node->category = (uint16) PBM_CAT_MISSED;
            node->num_fields = PBM_CALLHISTORY_NUM_FIELDS;
            break;
         default:
            UIM_MSG_ERR_1("Unknown category.  Searches will be interesting. %d", pb_id.device_type);
            node->num_fields = PBM_SIM_NUM_FIELDS;
            node->category = (uint16) PBM_CAT_NONE;
            break;
      }

      /* Allocate 3 fields for an ADN record.
       * 1 for the Name.          ( Type string. )
       * 1 for the GSM_NUMBER     ( Type string. )
       * 1 for the GSM_TON_NPI    ( Type Byte. )
       */
      field_lengths[0] = strlen(record->text);
      if (field_lengths[0] > PBM_MAX_TEXT_LENGTH) field_lengths[0] = PBM_MAX_TEXT_LENGTH;
      if (field_lengths[0])
      {
         field_lengths[0]++; /* Room for the NULL. */
         field_lengths[0] *= sizeof(uint16); /* Make room for UCS2 characters. */
      }

	  if ( node->num_fields > 1 )
	  {
      field_lengths[1] = strlen(record->number);
      if (field_lengths[1] > PBM_MAX_NUM_LENGTH) field_lengths[1] = PBM_MAX_NUM_LENGTH;
      if (field_lengths[1])
         field_lengths[1]++; /* Room for the NULL. */

      field_lengths[2] = 1;       /* GSM_TON_NPI is 1 byte always. */

      if (record->index != -1)
         field_lengths[3] = 2;     /* Index */

      if (node->num_fields == 6)
      {
         field_lengths[4] = 4;     /* The DATE is stored as a DWORD */
         field_lengths[5] = 4;     /* The DURATION is also stored as a DWORD */
      }
	  }

      for (j = 0; j < node->num_fields; j++)
      {
         if (field_lengths[j] > 0)
            build_field_lengths[index++] = field_lengths[j];
      }

      if (pbm_allocate_fields_internal(&node->fields, (uint32) index, build_field_lengths) == FALSE)
      {
         PBM_MEM_FREEIF(node);
         return NULL;
      }

      if (record->index == -1)
         node->record_id = PBM_SESSION_GET_FIRST_AVAILABLE;
      else
      {
         location = (uint16) record->index;
         node->record_id = pbm_session_location_to_record_id(pb_id, location);
      }

      index = 0;

      if (field_lengths[0] > 0)
      {
         PBM_MEM_ALLOC(temp_ucs2_str, field_lengths[0]);
         if (temp_ucs2_str)
         {
            if (!pbmconvert_pbm8_to_ucs2((uint8 *) record->text,
                                         (uint16) strlen(record->text),
                                         temp_ucs2_str,
                                         (uint16) field_lengths[0]/sizeof(uint16)))
            {
               UIM_MSG_ERR_0("Could not convert entire string.");
            }
            pbm_fill_field(&node->fields[index++], PBM_FIELD_NAME, PBM_FT_UCS2,
                           (byte *) temp_ucs2_str, (uint16) field_lengths[0]);

            PBM_MEM_FREEIF(temp_ucs2_str);
         }
      }

	  if ( node->num_fields > 1 )
	  {
      if (field_lengths[1] > 0)
         pbm_fill_field(&node->fields[index++], PBM_FIELD_PHONE_GENERIC, PBM_FT_PHONE,
                        (byte *) record->number, (uint16) field_lengths[1]);

      pbm_fill_field(&node->fields[index++], PBM_FIELD_GSM_TONNPI,
                     PBM_FT_BYTE, (byte *) &record->type, (uint16) field_lengths[2]);

      if (record->index != -1)
      {
         pbm_fill_field(&node->fields[index++], PBM_FIELD_INDEX,
                        PBM_FT_WORD, (byte *) &location, (uint16) field_lengths[3]);
      }

      if (node->num_fields == 6)
      {
         pbm_fill_field(&node->fields[index++], PBM_FIELD_DATE,
                        PBM_FT_DWORD, (byte *) &record->time, (uint16) field_lengths[4]);
         pbm_fill_field(&node->fields[index++], PBM_FIELD_DURATION,
                        PBM_FT_DWORD, (byte *) &record->duration, (uint16) field_lengths[5]);
      }
	   }
      node->num_fields = index;
   }

   return node;
}


/*===========================================================================
FUNCTION pbm_populate_enum_file_access

DESCRIPTION
This function will map the FDN file to correspoding MMGSDI enums based on 
APP and fill the access type as BY ENUM 

This is used for accessing Read/Write/SEEK/Get_file attributes 

For Refresh it will be still by path only even for FDN

SIDE EFFECTS
===========================================================================*/


pbm_return_type pbm_populate_enum_file_access(pbm_session_enum_type pbm_sess_type,
                                          mmgsdi_access_type *file_struct,
                                          pbm_file_type pbm_file)
{
          mmgsdi_file_enum_type fdn_file = MMGSDI_NO_FILE_ENUM;
   	  int index = pbm_util_return_fileset_for_session(pbm_sess_type);

          switch ( index )
   	  {
   	      case 0 : 
		  fdn_file = MMGSDI_TELECOM_FDN ;
	          break;
   	      case 1 :
   	          fdn_file = MMGSDI_USIM_FDN;
   	          break;
   	      case 2 :
   	          fdn_file = MMGSDI_CSIM_FDN; 
   	          break;
   	      case 3 :
	          fdn_file = MMGSDI_NO_FILE_ENUM;
	          break;
   	      case 4 :
		  fdn_file = MMGSDI_NO_FILE_ENUM;
   	          break;
   	      default :
                  UIM_MSG_HIGH_3("Invalid session and file match : session %d file %d index %d", 
                                  pbm_sess_type,pbm_file,index);
   	          break;
   	 }

         if (PBM_FILE_FDN == pbm_file )
         {
             file_struct->file.file_enum = fdn_file ;
         }

         file_struct->access_method	= MMGSDI_EF_ENUM_ACCESS;

         UIM_MSG_HIGH_3("pbm_populate_enum_file_access file %d enum %d sess %d",
                         pbm_file,file_struct->file.file_enum,pbm_sess_type);

         if( MMGSDI_NO_FILE_ENUM == file_struct->file.file_enum ) 
         {
             return PBM_ERROR;
         }

         return PBM_SUCCESS;
}
/*===========================================================================
FUNCTION PBM_POPULATE_FILE_ACCESS

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_populate_file_access(pbm_session_enum_type pbm_sess_type,
                                          mmgsdi_access_type *file_struct,
                                          pbm_file_type pbm_file)
{
    int j;
    int index = 0;
    boolean is_ef_init_in_table_ptr = 0;
    pbm_file_type file_id = PBM_FILE_NONE;
    pbm_cache_type_e_type cache_type;
    pbm_return_type ret = PBM_ERROR;

    file_struct->access_method	= MMGSDI_EF_ENUM_ACCESS;
    index = pbm_util_return_fileset_for_session(pbm_sess_type);
    for (j = 0; j < (int)PBM_FILE_MAX_REQUESTS; j++)
    {
      if (pbm_file_to_uim_file[j].pbm_session_filename[index] == pbm_file)
      {
        file_id = pbm_file_to_uim_file[j].pbm_filename;
        break;
      }
    }
    if(file_id == PBM_FILE_NONE)
    {
        UIM_MSG_ERR_2("invalid PBM file %d for session %d",pbm_file,pbm_sess_type);
        return PBM_ERROR;
    }

    cache_type = pbm_file_id_to_cache_type(file_id);
    if(cache_type == PBM_CACHE_TYPE_USIM || cache_type == PBM_CACHE_TYPE_INDEX || cache_type == PBM_CACHE_TYPE_SYNC1
        || cache_type == PBM_CACHE_TYPE_SYNC2 || cache_type == PBM_CACHE_TYPE_EXT ||
        file_id == PBM_FILE_ADN || file_id == PBM_FILE_ADN1
           || file_id == PBM_FILE_GAS
           || file_id == PBM_FILE_AAS
        || file_id == PBM_FILE_ADN2 || file_id == PBM_FILE_ADN3
      )
    {
        ret = pbm_util_access_path_lookup_table (pbm_file, file_struct, &is_ef_init_in_table_ptr, pbm_sess_type);
        if(is_ef_init_in_table_ptr == TRUE && ret == PBM_SUCCESS)
        {
            file_struct->access_method = MMGSDI_BY_PATH_ACCESS;
            file_struct->file.path_type.path_buf[0] = 0x3F00;
            //convert path  3F00 5F3A 4F3A FFFF to 3F00 FFFF 5F3A 4F3A
            file_struct->file.path_type.path_buf[3] = file_struct->file.path_type.path_buf[2];
            file_struct->file.path_type.path_buf[2] = file_struct->file.path_type.path_buf[1];
            if(USE_USIM_LOCAL_SUPPORT(pbm_sess_type) || USE_CSIM_LOCAL_SUPPORT(pbm_sess_type))
            {
              file_struct->file.path_type.path_buf[1] = 0x7FFF;
            }
            else
            {
              file_struct->file.path_type.path_buf[1] = 0x7F10;
            }
        }
    }
    if(file_struct->access_method == MMGSDI_BY_PATH_ACCESS) /* path found in usim path lookup table */
    {
    	return PBM_SUCCESS;
    }
    else /* look in static path lookup table */
    {
       for (j=0; j<(int)ARR_SIZE(pbm_static_path_lookup_table); j++)
       {
           if(file_id == pbm_static_path_lookup_table[j].pbm_filename)
           {
             if(pbm_static_path_lookup_table[j].paths[index][0] != 0x0000)
        {
               file_struct->access_method = MMGSDI_BY_PATH_ACCESS;
               /* put static path in path_type.path_buf and put the right length as well*/
               file_struct->file.path_type.path_len = pbm_static_path_lookup_table[j].path_length_to_be_used[index];
               if(file_struct->file.path_type.path_len > 0)
               {
                 memscpy(file_struct->file.path_type.path_buf, sizeof(file_struct->file.path_type.path_buf),
                         pbm_static_path_lookup_table[j].paths[index],
                         MIN( file_struct->file.path_type.path_len * 2 , sizeof(pbm_static_path_lookup_table[j].paths[index])));
               }
             }
             break;
           }
        }
    }
    if(file_struct->access_method == MMGSDI_BY_PATH_ACCESS) /* path found in static path lookup table */
    {
      return PBM_SUCCESS;
    }
    else
    {
      return PBM_ERROR;
    }
}

/*===========================================================================
FUNCTION pbm_build_record_from_usim_fields

DESCRIPTION
  This function adds to a record from USIM fields.  It expects to either be
  sent 1 field, or multiple consecutive group ID fields.


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_usim_fields(pbm_record_id_type rec_id,
                                       const pbm_field_s_type *fields,
                                       int num_fields,
                                       pbm_sim_record_s_type *record)
{
   int i=0;

  PBM_CHECK_PTR2_RET(fields, record, VOID);
  if (num_fields > 1)
  {
    UIM_MSG_ERR_2("Too many fields sent to pbm_build_record_from_usim_fields() %d %d",
                   num_fields,fields[0].field_id);
  }

   switch (fields[0].field_id)
   {
      case PBM_FIELD_OTHERNAME:
      case PBM_FIELD_EMAIL:
         memscpy(record->data.text.sim_text,sizeof(record->data.text.sim_text),
		 	     pbm_get_field_data_ptr(fields,0),fields[0].data_len);
         record->data.text.adn_record_id = rec_id;
         if (num_fields > 1)
         {
            UIM_MSG_ERR_2("Too many fields sent to pbm_build_record_from_usim_fields() %d %d",
                        num_fields, fields[0].field_id);
         }
         break;

      case PBM_FIELD_ADDITIONAL_NUMBER_EXT:
      {
          byte *anr_data_ptr;
          anr_data_ptr = (byte*)pbm_get_field_data_ptr(fields,0);
          memscpy(&record->data.anr.anr_data,sizeof(record->data.anr.anr_data),
                 anr_data_ptr,2+anr_data_ptr[1]);
          record->data.anr.adn_record_id = rec_id;
      }
         break;

      case PBM_FIELD_GRPNAME_ID:
         for ( i = 0; i < num_fields; i++)
         {
            PBM_CHECK_PTR_RET(fields,VOID);
            record->data.grp.group_id[i] = *((uint8 *)pbm_get_field_data_ptr(fields,i));
            if (fields[i].field_id != PBM_FIELD_GRPNAME_ID)
            {
               UIM_MSG_ERR_3("Not a groupname field %d %d %d",
                              i, fields[i].field_id, num_fields);
            }
         }
         break;

      default:
         break;
   }
}


/*===========================================================================
FUNCTION CHECK_PB_READY

DESCRIPTION
Checks if the phonebook is ready or not and returns the appropriate PBM
return value. Any value other than PBM_SUCCESS means the phonebook is not
available for use.

Checking for PBM_DEFAULT returns an ERROR

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type check_pb_ready(pbm_phonebook_type pb_id)
{
   pbm_return_type ret;
   PB_CACHE *cache;

   if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
      return PBM_ERROR;

   switch (cache->status)
   {
      case PBM_STATUS_READY:
         ret = PBM_SUCCESS;
         break;

      case PBM_STATUS_NOT_INIT:
         if (pbm_action_requires_sim(pb_id))
            ret = PBM_ERROR_NO_SIM;
         else
            ret = PBM_ERROR_NOT_READY;
         break;

      case PBM_STATUS_INIT_SYNC:
      case PBM_STATUS_WRITE_SYNC:
         if (rex_self() == PBM_TCB)
            ret = PBM_SUCCESS;
         else
            ret = PBM_ERROR_SYNC;
         break;

      case PBM_STATUS_PIN_REQ: ret = PBM_ERROR_SIM_PIN_REQ; break;
      case PBM_STATUS_PUC_REQ: ret = PBM_ERROR_SIM_PUC_REQ; break;
      case PBM_STATUS_PIN2_REQ: ret = PBM_ERROR_PIN2_REQ; break;
      case PBM_STATUS_PUC2_REQ: ret = PBM_ERROR_PUC2_REQ; break;
      case PBM_STATUS_SIM_FAIL: ret = PBM_ERROR_SIM_FAIL; break;
      case PBM_STATUS_NOT_AVAILABLE: ret = PBM_ERROR_NOT_AVAILABLE; break;
      case PBM_STATUS_RESTRICTED: ret = PBM_ERROR_RESTRICTED; break;

      case PBM_STATUS_NOT_READY:
      default:
         ret = PBM_ERROR_NOT_READY;
         break;
   }

   return ret;
}

/*===========================================================================
FUNCTION PBM_FILE_CIRCULAR

DESCRIPTION
  This function returns TRUE if the init file is circular, FALSE otherwise.

SIDE EFFECTS
===========================================================================*/
boolean pbm_file_circular(pbm_file_type file_id)
{
   switch (file_id)
   {
      case PBM_FILE_LND:
      case PBM_FILE_OCI:
      case PBM_FILE_ICI:
         return TRUE;

      default:
         return FALSE;
   }
}


/*==============================================================================

FUNCTION NAME pbm_action_requires_sim

DESCRIPTION
   This function is called to determine if an action on a given pb_id
   requires the SIM to be fully initialized.  The purpose of this call
   is to prohibit reads and writes to memory while the SIM is still
   initializing.

RETURN VALUE
  TRUE: If the action would require the SIM.
  FALSE: The pb_id does not touch the UIM.

DEPENDENCIES
  None
==============================================================================*/
boolean pbm_action_requires_sim(pbm_phonebook_type pb_id)
{
   switch (pb_id.device_type)
   {
      case PBM_DEFAULT:
      case PBM_ADN:                 /* Abbreviated Dialing Number list */
      case PBM_FDN:                 /* Fixed Dialing Number list */
      case PBM_LND:                 /* SIM/RUIM Last Number Dialed list */
      case PBM_SDN:                 /* Service Dialling Number */
      case PBM_MSISDN:              /* MSISDN */
      case PBM_MBN:                 /* MBN */
      case PBM_ECC:                 /* Emergency Call Code (Emergency number list) */
      case PBM_GRPNAME:
      case PBM_AAS:
      case PBM_OCI:                 /* USIM Outgoing Call Information file.  */
      case PBM_ICI:                 /* USIM Incoming Call Information file.  */
      case PBM_MBDN:                /* MBDN */
         return TRUE;
      default:
         return FALSE;
   }
}


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache type associated with a given file id

SIDE EFFECTS
===========================================================================*/
pbm_cache_type_e_type pbm_file_id_to_cache_type(pbm_file_type file_id)
{
   switch (file_id)
   {
      case PBM_FILE_ADN:
      case PBM_FILE_FDN:
      case PBM_FILE_SDN:
      case PBM_FILE_LND:
      case PBM_FILE_MSISDN:
      case PBM_FILE_MBN:
      case PBM_FILE_GAS:
      case PBM_FILE_AAS:
      case PBM_FILE_OCI:
      case PBM_FILE_ICI:
      case PBM_FILE_MBDN:
      case PBM_FILE_ADN1:
      case PBM_FILE_ADN2:
      case PBM_FILE_ADN3:
         return PBM_CACHE_TYPE_PB;

      case PBM_FILE_EXT1:
      case PBM_FILE_EXT2:
      case PBM_FILE_EXT3:
      case PBM_FILE_EXT5:
      case PBM_FILE_EXT6:
         return PBM_CACHE_TYPE_EXT;
      case PBM_FILE_EMAIL:
      case PBM_FILE_SNE:
      case PBM_FILE_GRP:
      case PBM_FILE_ANR:
      case PBM_FILE_ANRA:
      case PBM_FILE_ANRB:
      case PBM_FILE_ANRC:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_SNE1:
      case PBM_FILE_GRP1:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRC1:
      case PBM_FILE_EMAIL2:
      case PBM_FILE_SNE2:
      case PBM_FILE_GRP2:
      case PBM_FILE_ANR2:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRC2:
      case PBM_FILE_EMAIL3:
      case PBM_FILE_SNE3:
      case PBM_FILE_GRP3:
      case PBM_FILE_ANR3:
      case PBM_FILE_ANRA3:
      case PBM_FILE_ANRB3:
      case PBM_FILE_ANRC3:
         return PBM_CACHE_TYPE_USIM;

      case PBM_FILE_IAP:
      case PBM_FILE_IAP1:
      case PBM_FILE_IAP2:
      case PBM_FILE_IAP3:
         return PBM_CACHE_TYPE_INDEX;

      case PBM_FILE_PSC:
      case PBM_FILE_PUID:
      case PBM_FILE_CC:
         return PBM_CACHE_TYPE_SYNC1;

      case PBM_FILE_UID:
      case PBM_FILE_UID1:
      case PBM_FILE_PBC:
      case PBM_FILE_PBC1:
      case PBM_FILE_UID2:
      case PBM_FILE_UID3:
      case PBM_FILE_PBC2:
      case PBM_FILE_PBC3:
         return PBM_CACHE_TYPE_SYNC2;
      default:
         UIM_MSG_ERR_1("Unknown file id %d", file_id);
         return PBM_CACHE_TYPE_UNKNOWN;
   }
}



/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given file id
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
void *pbm_file_id_to_cache(pbm_session_enum_type session_type,
                           pbm_file_type file_id)
{
   pbm_pb_set_type pb_set;

   PBM_VALIDATE_SESSION_RET(session_type, NULL);

   switch (file_id)
   {
      case PBM_FILE_ADN:
      case PBM_FILE_ADN1:
      case PBM_FILE_ADN2:
      case PBM_FILE_ADN3:
         return &adn_cache[session_type];

      case PBM_FILE_FDN:
         return &fdn_cache[session_type];

      case PBM_FILE_SDN:
         return &sdn_cache[session_type];

      case PBM_FILE_LND:
         return &lnd_cache[session_type];

      case PBM_FILE_MSISDN:
         return &msisdn_cache[session_type];

      case PBM_FILE_MBN:
         return &mbn_cache[session_type];

      case PBM_FILE_EXT1:
         return &ext1_cache[session_type];

      case PBM_FILE_EXT2:
         return &ext2_cache[session_type];

      case PBM_FILE_EXT3:
         return &ext3_cache[session_type];
      case PBM_FILE_ECC:
         return &ecc_sim_cache[session_type];

      case PBM_FILE_GAS:
         return &grpname_cache[session_type];
      case PBM_FILE_AAS:
         return &aas_cache[session_type];
      case PBM_FILE_OCI:
         return &oci_cache[session_type];

      case PBM_FILE_ICI:
         return &ici_cache[session_type];

      case PBM_FILE_MBDN:
         return &mbdn_cache[session_type];

      case PBM_FILE_EXT5:
         return &ext5_cache[session_type];

      case PBM_FILE_EXT6:
         return &ext6_cache[session_type];

      case PBM_FILE_PSC:
         return &psc_cache[session_type];

      case PBM_FILE_PUID:
         return &puid_cache[session_type];

      case PBM_FILE_CC:
         return &cc_cache[session_type];

      case PBM_FILE_EMAIL:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_EMAIL2:
      case PBM_FILE_EMAIL3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
            return &email_cache[session_type][pb_set];
         }
         else
         {
            return NULL;
         }

      case PBM_FILE_SNE:
      case PBM_FILE_SNE1:
      case PBM_FILE_SNE2:
      case PBM_FILE_SNE3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
            return &sne_cache[session_type][pb_set];
         }
         else
         { 
             return NULL; 
         }

      case PBM_FILE_GRP:
      case PBM_FILE_GRP1:
      case PBM_FILE_GRP2:
      case PBM_FILE_GRP3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
             return &grp_cache[session_type][pb_set];
         }
         else
         { 
             return NULL; 
         }

      case PBM_FILE_ANR:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANR2:
      case PBM_FILE_ANR3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
             return &anr_cache[session_type][PBM_USIM_SAMEFILE_INDEX(pb_set, 0)];
         }
         else
         { 
             return NULL; 
         }

      case PBM_FILE_ANRA:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRA3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
            return &anr_cache[session_type][PBM_USIM_SAMEFILE_INDEX(pb_set, 1)];
         }
         else
         { 
            return NULL; 
         }

      case PBM_FILE_ANRB:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRB3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
             return &anr_cache[session_type][PBM_USIM_SAMEFILE_INDEX(pb_set, 2)];
         }
         else
         { 
             return NULL; 
         }

      case PBM_FILE_ANRC:
      case PBM_FILE_ANRC1:
      case PBM_FILE_ANRC2:
      case PBM_FILE_ANRC3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
             return &anr_cache[session_type][PBM_USIM_SAMEFILE_INDEX(pb_set, 3)];
         }
         else
         { 
             return NULL; 
         }

      case PBM_FILE_IAP:
      case PBM_FILE_IAP1:
      case PBM_FILE_IAP2:
      case PBM_FILE_IAP3:
         pb_set = pbm_file_id_to_pb_set(file_id);
         if ( pbm_validate_pb_set(pb_set) )
         {
             return &adn_m_usim_cache[session_type][pb_set];
         }
         else
         { 
             return NULL; 
         }   

      case PBM_FILE_UID:
      case PBM_FILE_UID1:
      case PBM_FILE_UID2:
      case PBM_FILE_UID3:
         return &uid_cache[session_type];

      case PBM_FILE_PBC:
      case PBM_FILE_PBC1:
      case PBM_FILE_PBC2:
      case PBM_FILE_PBC3:
         return &pbc_cache[session_type];
      default:
         UIM_MSG_ERR_1("Unknown file id %d", file_id);
         return NULL;
   }
}


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_SET

DESCRIPTION
  Returns the proper pb set based on the given file id.

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_file_id_to_pb_set(pbm_file_type file_id)
{
   switch (file_id)
   {
      case PBM_FILE_ADN:
      case PBM_FILE_EMAIL:
      case PBM_FILE_SNE:
      case PBM_FILE_GRP:
      case PBM_FILE_ANR:
      case PBM_FILE_ANRA:
      case PBM_FILE_ANRB:
      case PBM_FILE_ANRC:
      case PBM_FILE_IAP:
      case PBM_FILE_UID:
      case PBM_FILE_PBC:
      case PBM_FILE_ECC:
      case PBM_FILE_GAS:
      case PBM_FILE_AAS:
         return PBM_USIM_PB_ONE;

      case PBM_FILE_ADN1:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_SNE1:
      case PBM_FILE_GRP1:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRC1:
      case PBM_FILE_IAP1:
      case PBM_FILE_UID1:
      case PBM_FILE_PBC1:
         return PBM_USIM_PB_TWO;

      case PBM_FILE_ADN2:
      case PBM_FILE_EMAIL2:
      case PBM_FILE_SNE2:
      case PBM_FILE_GRP2:
      case PBM_FILE_ANR2:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRC2:
      case PBM_FILE_IAP2:
      case PBM_FILE_UID2:
      case PBM_FILE_PBC2:
         return PBM_USIM_PB_THREE;

      case PBM_FILE_ADN3:
      case PBM_FILE_EMAIL3:
      case PBM_FILE_SNE3:
      case PBM_FILE_GRP3:
      case PBM_FILE_ANR3:
      case PBM_FILE_ANRA3:
      case PBM_FILE_ANRB3:
      case PBM_FILE_ANRC3:
      case PBM_FILE_IAP3:
      case PBM_FILE_UID3:
      case PBM_FILE_PBC3:
         return PBM_USIM_PB_FOUR;

      default:
         return PBM_USIM_PB_INVALID;
   }
}/* pbm_file_id_to_pb_set */


/*===========================================================================
FUNCTION PBM_LOCATION_TO_ADN_REC_ID

DESCRIPTION
  Returns the ADN record id given the session id, phonebook set and location
  of ADN record.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_adn_record_id_type pbm_location_to_adn_rec_id(pbm_session_enum_type session_type,
                                                  pbm_pb_set_type pb_set,
                                                  uint8 index)
{
   PB_CACHE *pb_cache;
   int location;

   PBM_VALIDATE_SESSION_RET(session_type, PBM_INVALID_REC_ID);

   if ( ! pbm_validate_pb_set(pb_set) )
   {
      return PBM_INVALID_REC_ID; 
   }

   if (index == 0xFF)
      return PBM_INVALID_REC_ID;

   pb_cache = pbm_file_id_to_cache(session_type, PBM_FILE_ADN);

   PBM_CHECK_PTR_RET(pb_cache, PBM_INVALID_REC_ID);

   if (!pb_cache->num_of_records || !pb_cache->num_files ||
       index > pb_cache->num_of_records)
   {
      UIM_MSG_ERR_3("Invalid values cache 0x%x, index %d, files %d",
                  pb_cache, index, pb_cache->num_files);
      return PBM_INVALID_REC_ID;
   }

   location = index;
   if (pb_set >= PBM_USIM_PB_FOUR)
      location += (pb_cache->records_in_pb_set[2]);
   if (pb_set >= PBM_USIM_PB_THREE)
      location += (pb_cache->records_in_pb_set[1]);
   if (pb_set >= PBM_USIM_PB_TWO)
      location += (pb_cache->records_in_pb_set[0]);
   return pbm_file_location_to_adn_record_id(PBM_FILE_ADN, location);
}


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_FIELD_ID

DESCRIPTION
  Returns the field id stored within the given file device.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_id_e_type pbm_file_id_to_field_id(pbm_file_type file_id)
{
   switch (file_id)
   {
      case PBM_FILE_EMAIL:
      case PBM_FILE_EMAIL1:
      case PBM_FILE_EMAIL2:
      case PBM_FILE_EMAIL3:
         return PBM_FIELD_EMAIL;

      case PBM_FILE_SNE:
      case PBM_FILE_SNE1:
      case PBM_FILE_SNE2:
      case PBM_FILE_SNE3:
         return PBM_FIELD_OTHERNAME;

      case PBM_FILE_GRP:
      case PBM_FILE_GRP1:
      case PBM_FILE_GRP2:
      case PBM_FILE_GRP3:
         return PBM_FIELD_GRPNAME_ID;

      case PBM_FILE_ANR:
      case PBM_FILE_ANRA:
      case PBM_FILE_ANRB:
      case PBM_FILE_ANRC:
      case PBM_FILE_ANR1:
      case PBM_FILE_ANRA1:
      case PBM_FILE_ANRB1:
      case PBM_FILE_ANRC1:
      case PBM_FILE_ANR2:
      case PBM_FILE_ANRA2:
      case PBM_FILE_ANRB2:
      case PBM_FILE_ANRC2:
      case PBM_FILE_ANR3:
      case PBM_FILE_ANRA3:
      case PBM_FILE_ANRB3:
      case PBM_FILE_ANRC3:
         return PBM_FIELD_ADDITIONAL_NUMBER_EXT;
      case PBM_FILE_ECC:
         return PBM_FIELD_SIM_ECC;

      default:
         return PBM_FIELD_NONE;
   }
}


/*===========================================================================
FUNCTION PBM_RECORD_WRITE_CHECK

DESCRIPTION
  This function does all the initial error checking for a record write
  operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
//lint -e{715} suppress unused parameters (cat, proc_func)
pbm_return_type pbm_record_write_check(pbm_phonebook_type pb_id,
                                       pbm_record_id_type rec_id,
                                       uint16 cat,
                                       const pbm_field_s_type *fields,
                                       int num_fields)
{
   PB_CACHE *cache;
   boolean deletion;
   pbm_return_type ret;

   if (num_fields < 0)
      return PBM_ERROR;

   /* The ECC is currently read only. */
   if (pb_id.device_type == PBM_ECC)
   {
      return PBM_ERROR_NOT_SUPPORTED;
   }

   /* We have the cache, make sure this one is ready to go. */
   if ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS)
   {
      return ret;
   }

   /* Get cache */
   if ((cache = pbm_pb_id_to_cache(pb_id)) == NULL)
   {
      return PBM_ERROR;
   }

   /* We can only write to the first available record for circular files. */
   /* SIM records, I can only write to the top of the circular file, for
    * EFS records, we currently assume the last record written is the
    * latest when initializing the stack, thus, only the first available
    * there can be written.  This is also true with deleting records,
    * but deleting the "FIRST_AVAILABLE", the oldest records will be
    * pushed out of the circular file.  This is true both for the SIM
    * and for EFS.
    */
   deletion = ((num_fields == 0 || fields == NULL) ? TRUE : FALSE);

   if ( rec_id == PBM_SESSION_GET_FIRST_AVAILABLE)
   {
      if (deletion && !cache->circular)
      {
         return PBM_ERROR_INDEX_INVALID;
      }
   } else
   {
      if (cache->circular)
      {
         return PBM_ERROR_INDEX_INVALID;
      }
   }

   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_INIT

DESCRIPTION
  This function initializes enumeration of a complex field. If there are
  complex fields within complex fields, all of them are converted into
  individual PBM fields.

  If the input field is a simple field, the simple field is returned as
  enumeration.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_complex_init(const pbm_field_s_type *field,
                                      int *pnum_fields,
                                      pbm_complex_result_s_type **presult)
{
   int num_sub_fields;
   int returned_val;
   pbm_return_type ret;
   pbm_complex_result_s_type *result = NULL;
   uint32 *sizes=NULL;

  PBM_CHECK_PTR2_RET(field, presult, PBM_ERROR);

   *presult = NULL;

   PBM_MEM_ALLOC(result, sizeof(pbm_complex_result_s_type));
   PBM_CHECK_PTR_RET(result, PBM_ERROR);
   memset(result, 0, sizeof(pbm_complex_result_s_type));

   /* Get number of fields */
   if (field->field_type == PBM_FT_COMPLEX)
   {
      num_sub_fields = pbm_complex_num_sub_fields(
                                                 (pbm_complex_field_s_type *) pbm_get_field_data_ptr(field,0));
      PBM_MEM_ALLOC(sizes, (uint32) num_sub_fields * sizeof(uint32));
      if (sizes == NULL)
      {
         pbm_enum_complex_free(result);
         return PBM_ERROR;
      }

      returned_val = pbm_complex_fill_sizes(sizes,
                                            num_sub_fields,
                                            (pbm_complex_field_s_type *)
                                            pbm_get_field_data_ptr(field,0));
      if (returned_val != num_sub_fields)
      {
         UIM_MSG_ERR_2("pbm_complex_fill_sizes() didn't fill all subfields %d %d",
                     returned_val, num_sub_fields);
         PBM_MEM_FREEIF(sizes);
         pbm_enum_complex_free(result);
         return PBM_ERROR;
      }
   } else
   {
      num_sub_fields = 1;

      PBM_MEM_ALLOC(sizes, (uint32) num_sub_fields * sizeof(uint32));
      if (sizes == NULL)
      {
         pbm_enum_complex_free(result);
         return PBM_ERROR;
      }

      *sizes = field->data_len;
   }

   result->num_fields = num_sub_fields;

   /* Allocate memory */
   if (!pbm_allocate_fields_internal(&result->fields, (uint32) num_sub_fields,
                                     sizes))
   {
      PBM_MEM_FREEIF(sizes);
      pbm_enum_complex_free(result);
      return PBM_ERROR;
   }

   PBM_MEM_FREEIF(sizes);

   /* Read fields */
   if (field->field_id == PBM_FIELD_COMPLEX)
   {
      ret = pbm_complex_unwrap((pbm_complex_field_s_type *)
                               pbm_get_field_data_ptr(field,0), result);
      if (ret != PBM_SUCCESS)
      {
         pbm_enum_complex_free(result);
         return PBM_ERROR;
      }
   } else
   {
      pbm_fill_field(&result->fields[result->enumerator++],
                     field->field_id,
                     field->field_type,
                     pbm_get_field_data_ptr(field,0),
                     field->data_len);
   }

   if (pnum_fields)
      *pnum_fields = num_sub_fields;

   result->enumerator = 0;

   *presult = result;

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_NEXT

DESCRIPTION
  This function obtains the next sub field within a complex field, converts it
  to a normal PBM field and returns it.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_s_type *pbm_enum_complex_next(pbm_complex_result_s_type *result)
{
   if (!result)
      return NULL;

   if (result->enumerator >= result->num_fields)
      return NULL;
   else
      return &result->fields[result->enumerator++];
}


/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_FREE

DESCRIPTION
  This function frees the memory allocated to store the search result.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_enum_complex_free(pbm_complex_result_s_type *result)
{
   PBM_CHECK_PTR_RET(result, VOID);

   PBM_MEM_FREEIF(result->fields);
   result->num_fields = 0;
   result->enumerator = 0;

   PBM_MEM_FREEIF(result);
}


/*===========================================================================
FUNCTION PBM_CAT_TO_PHONEBOOK

DESCRIPTION
  This function returns the phonebook associated with a given category.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_cat_to_phonebook(pbm_cat_e_type cat)
{
   switch (cat)
   {
      case PBM_CAT_BUSINESS:
      case PBM_CAT_PERSONAL:
         return PBM_EFS;

      case PBM_CAT_SIM_ADN:
         return PBM_ADN;

      case PBM_CAT_SIM_FDN:
         return PBM_FDN;

      case PBM_CAT_SIM_BDN:
         return PBM_BDN;

      case PBM_CAT_SDL:
         return PBM_SDL;

      case PBM_CAT_INCOMING:
         return PBM_RCVD;

      case PBM_CAT_OUTGOING:
         return PBM_DIALED;

      case PBM_CAT_MISSED:
         return PBM_MISS;

      case PBM_CAT_SIM_SDN:
         return PBM_SDN;

      case PBM_CAT_SIM_MSISDN:
         return PBM_MSISDN;

      case PBM_CAT_SIM_MBN:
         return PBM_MBN;

      case PBM_CAT_ECC:
         return PBM_ECC;

      case PBM_CAT_GRPNAME:
         return PBM_GRPNAME;
      case PBM_CAT_AAS:
         return PBM_AAS;
      case PBM_CAT_SIM_MBDN:
         return PBM_MBDN;


      default:
         UIM_MSG_ERR_1("Unexpected category %d", cat);
         return PBM_DEFAULT;
   }
}

/*===========================================================================
FUNCTION PBM_LEGACY_CAT_TO_SESSION_CAT

DESCRIPTION
  This function convert the slot2 category to session based cat.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_cat_e_type pbm_legacy_cat_to_session_cat(pbm_cat_e_type p_leg_cat)
{
   switch (p_leg_cat)
   {
         case PBM_CAT_SIM_ADN2:
           return PBM_CAT_SIM_ADN;

         case PBM_CAT_SIM_FDN2:
           return PBM_CAT_SIM_FDN;

         case PBM_CAT_SIM_BDN2:
           return PBM_CAT_SIM_BDN;

         case PBM_CAT_SIM_SDN2:
           return PBM_CAT_SIM_SDN;

         case PBM_CAT_SIM_MSISDN2:
           return PBM_CAT_SIM_MSISDN;

         case PBM_CAT_SIM_MBN2:
           return PBM_CAT_SIM_MBN;

         case PBM_CAT_SIM_MBDN2:
           return PBM_CAT_SIM_MBDN;

      default:
         return p_leg_cat;
   }
}
/*===========================================================================
FUNCTION PBM_SESSION_CAT_TO_LEGACY_CAT

DESCRIPTION
  This function convert the session category to legacy cat.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_cat_e_type pbm_session_cat_to_legacy_cat(pbm_phonebook_type pb_id,pbm_cat_e_type p_leg_cat)
{
   if(pb_id.slot_id== (uint16)PBM_SLOT_2)
   {
        switch (p_leg_cat)
        {
              case PBM_CAT_SIM_ADN:
                return PBM_CAT_SIM_ADN2;

              case PBM_CAT_SIM_FDN:
                return PBM_CAT_SIM_FDN2;

              case PBM_CAT_SIM_BDN:
                return PBM_CAT_SIM_BDN2;

              case PBM_CAT_SIM_SDN:
                return PBM_CAT_SIM_SDN2;

              case PBM_CAT_SIM_MSISDN:
                return PBM_CAT_SIM_MSISDN2;

              case PBM_CAT_SIM_MBN:
                return PBM_CAT_SIM_MBN2;

              case PBM_CAT_SIM_MBDN:
                return PBM_CAT_SIM_MBDN2;

           default:
              return p_leg_cat;
        }

   }
   else
   {
     return p_leg_cat;
   }
}

/*===========================================================================
FUNCTION PBM_PB_ID_TO_FIELDGROUP

DESCRIPTION
  This function returns the field info group corresponding to this phonebook.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_pb_id_to_fieldgroup(pbm_phonebook_type pb_id)
{
   switch (pb_id.device_type)
   {
      case PBM_FDN:
         return FDN_GROUP;
      case PBM_BDN:
      case PBM_SDN:
      case PBM_MSISDN:
      case PBM_MBN:
      case PBM_MBDN:
         return SIM_GROUP;
      case PBM_GRPNAME:
         return GRPNAME_GROUP;
      case PBM_AAS:
         return AAS_GROUP;

      case PBM_ADN:
         return ADN_GROUP;

      case PBM_DIALED:
      case PBM_RCVD:
      case PBM_MISS:
      case PBM_LND:
      case PBM_OCI:
      case PBM_ICI:
         return CALLHISTORY_GROUP;

      case PBM_EFS:
      case PBM_DEFAULT:
         return EFS_GROUP;

      case PBM_SDL:
         return SDL_GROUP;

      case PBM_ECC:
         return ECC_GROUP;
      case PBM_ECC_OTHER:
        return ECC_OTHER_GROUP;

      default:
         UIM_MSG_ERR_1("Unexpected phonebook device %d", pb_id.device_type);
         return INVALID_GROUP;
   }
}

/*===========================================================================
FUNCTION pbm_get_active_session_type

DESCRIPTION
  This function returns the active session_type corresponding to this slot.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_active_prov_session_type(pbm_slot_type slot,pbm_session_enum_type *session_type)
{
  pbm_session_enum_type i = PBM_SESSION_DEFAULT;
  rex_enter_crit_sect(&pbm_crit_sect);
  for ( i = PBM_SESSION_DEFAULT; i < PBM_SESSION_MAX; i++ )
  {
    if ( pbm_session_info[i].slot_id == (uint16) slot
          && pbm_session_info[i].app_type != MMGSDI_APP_NONE
          && pbm_session_info[i].app_type != MMGSDI_APP_UNKNOWN)
    {
      *session_type =  i;
      rex_leave_crit_sect(&pbm_crit_sect);
      return PBM_SUCCESS;
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);
  return PBM_ERROR;
}

/*===========================================================================
FUNCTION pbm_get_active_app_type

DESCRIPTION
  This function returns the active session_type corresponding to this slot.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
mmgsdi_app_enum_type pbm_get_active_app_type(pbm_session_enum_type session_type)
{
  pbm_session_enum_type i = PBM_SESSION_DEFAULT;
  pbm_slot_type slot;

  rex_enter_crit_sect(&pbm_crit_sect);
  slot = (pbm_slot_type)pbm_session_info[session_type].slot_id;

  for ( i = PBM_SESSION_DEFAULT; i < PBM_SESSION_MAX; i++ )
  {
    if ( pbm_session_info[i].slot_id == (uint16) slot
          && pbm_session_info[i].app_type != MMGSDI_APP_NONE
          && pbm_session_info[i].app_type != MMGSDI_APP_UNKNOWN
          && (i == session_type || PBM_LPB != pbm_session_type_to_pb_category(session_type)))
    {
      rex_leave_crit_sect(&pbm_crit_sect);
      return pbm_session_info[i].app_type;
    }
  }
  rex_leave_crit_sect(&pbm_crit_sect);
  return MMGSDI_APP_UNKNOWN;
}


/*===========================================================================
FUNCTION pbm_compare_rec_ids

DESCRIPTION
  This function compares the two rec_ids (discards slot comparision in case of Provisioning Session

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_compare_rec_ids(pbm_record_id_type rec_id,pbm_record_id_type rec_id_other)
{
    pbm_phonebook_type pb_id,pb_id_other;
    pb_id=pbm_session_record_id_to_phonebook_type(rec_id);
    pb_id_other=pbm_session_record_id_to_phonebook_type(rec_id_other);
    if( ( pbm_session_record_id_to_location(rec_id) == pbm_session_record_id_to_location(rec_id_other) ) &&
        (pb_id.device_type == pb_id_other.device_type) && (pb_id.pb_category == pb_id_other.pb_category) && (pb_id.prov_type == pb_id_other.prov_type) )
    {
        if ( pbm_util_is_session_provision(pb_id.prov_type) || (pb_id.slot_id == pb_id_other.slot_id ))
                return TRUE;
    }
    return FALSE;
}


/*===========================================================================
FUNCTION pbm_util_is_session_provision

DESCRIPTION
  This function whether the session is provisioning or not

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_util_is_session_provision(pbm_provision_enum_type prov_type)
{
switch ( prov_type )
{
  case PBM_PROVISION_GW:
  case PBM_PROVISION_1X:
  case PBM_PROVISION_GW_SECONDARY:
  case PBM_PROVISION_1X_SECONDARY:
#ifdef FEATURE_TRIPLE_SIM
  case PBM_PROVISION_GW_TERTIARY :
  case PBM_PROVISION_1X_TERTIARY :
#endif
     return TRUE ;
  default :
    return FALSE;
}
}

/*===========================================================================
FUNCTION PBM_IS_USIM_FIELD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_field (pbm_field_id_e_type fid)
{
   switch (fid)
   {
      case PBM_FIELD_ADDITIONAL_NUMBER:
      case PBM_FIELD_ADDITIONAL_NUMBER_EXT:
      case PBM_FIELD_GRPNAME_ID:
      case PBM_FIELD_OTHERNAME:
      case PBM_FIELD_EMAIL:
         return TRUE;

      default:
         return FALSE;
   }
}


/*===========================================================================
FUNCTION pbm_is_usim_required

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_required(pbm_record_id_type rec_id, const pbm_field_s_type *fields,
                             int num_fields)
{
   int i;
   pbm_phonebook_type pb_id = {PBM_GPB};

   if (!fields || !num_fields)
      return FALSE;

   pb_id = pbm_session_record_id_to_phonebook_type(rec_id);

   if (pbm_action_requires_sim(pb_id))
   {
      for (i = 0; i < num_fields; i++)
      {
        pbm_field_id_e_type l_field;
        l_field = fields[i].field_id;
        if(l_field == PBM_FIELD_COMPLEX)
        {
          l_field = pbm_get_primary_field_from_complex(&fields[i]);
        }
        if (pbm_is_usim_field(l_field))
            return TRUE;

   }

   }

   return FALSE;
}

/*===========================================================================
FUNCTION pbm_get_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_get_usim_index(pbm_record_id_type rec_id,
                           const pbm_field_s_type *fields, int num_fields,
                           uint8 *index_array)
{
   PB_USIM_CACHE *usim_cache;
   PB_CACHE *pb_cache;
   int i, j;
   boolean *cache_in_use=NULL;
   PB_MASTER_USIM_CACHE *m_usim_cache;
   int location;
   int adn_index = 0;
   int index;
   int num_grp_ids = 0;
   PB_USIM_CACHE *group_cache = NULL;
   boolean ret = TRUE;
   pbm_pb_set_type pb_set;

  PBM_CHECK_PTR2_RET(index_array, fields,FALSE);
  pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
  PBM_CHECK_PTR_RET(pb_cache, FALSE);

   pb_set = pbm_record_id_to_pb_set(rec_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
      return FALSE; 
   }
   m_usim_cache = pb_cache->m_usim_cache[pb_set];
   PBM_CHECK_PTR_RET(m_usim_cache, FALSE);

   PBM_MEM_ALLOC(cache_in_use,
                 (uint32) (m_usim_cache->num_caches+1) * sizeof(boolean));
   PBM_CHECK_PTR_RET(cache_in_use, FALSE);
   memset(cache_in_use, 0,
          (uint32)(m_usim_cache->num_caches+1) * sizeof(boolean));

   location = pbm_record_id_to_location(rec_id);
   adn_index = pbm_pb_record_index_to_fileloc(pb_cache, location);
   if (adn_index > pb_cache->num_of_records)
   {
      UIM_MSG_ERR_3("Invalid adn_index %d based on location %d number_of_records %d",
                     adn_index,location, pb_cache->num_of_records);
      PBM_MEM_FREEIF(cache_in_use);
      return FALSE;
   }

   for (i = 0; i < num_fields; i++)
   {
      pbm_field_id_e_type l_field;

      l_field = fields[i].field_id;
      if(l_field == PBM_FIELD_COMPLEX)
      {
        l_field = pbm_get_primary_field_from_complex(&fields[i]);
      }
      if (pbm_is_usim_field(l_field))
      {
         if (l_field == PBM_FIELD_GRPNAME_ID)
         {
            if (group_cache)
            {
               if (++num_grp_ids <= group_cache->record_length)
                  continue;
               else
               {
                  group_cache = NULL;
                  num_grp_ids = 0;
               }
            }
         }

         j = 0;
         index = 0;
		 //go through the caches, and find the first one that is not
         //in use, and is valid for this type of data
         while ((usim_cache = pbm_get_next_usim_cache(rec_id,
                                                        l_field, &j)) != NULL)
         {
			if( !cache_in_use[usim_cache->order] )
            {
              if (usim_cache->mapping_type == PBM_MAPPING_TYPE_1)
              {
                 index = adn_index;
              }
              else
              {
                 if (m_usim_cache->index_array[usim_cache->order][adn_index])
                    index = m_usim_cache->index_array[usim_cache->order][adn_index];
                 else
                    index = pbm_usim_get_first_available(usim_cache);
              }

              if (index)
              {
                 index_array[usim_cache->order] = (uint8) index;
                 cache_in_use[usim_cache->order] = TRUE;
				 break;
              }
            }
         }

         if (!index)
         {
            /* Unable to find space for this field */
            ret = FALSE;
            break;
         }

         if(usim_cache == NULL)
         {
           UIM_MSG_ERR_0("usim_cache pointer NULL ");
           ret = FALSE;
           break;
         }

         if (pbm_file_id_to_field_id(usim_cache->file_id) ==
             PBM_FIELD_GRPNAME_ID)
         {
            group_cache = usim_cache;
            num_grp_ids++;
         }

      } /* if usim field */
   } /* for each field in current record */

   if (!ret)
   {
      UIM_MSG_ERR_0("Returning False from pbm_get_usim_index");
   }

   PBM_MEM_FREEIF(cache_in_use);

   return ret;
}


/*===========================================================================
FUNCTION PBM_GET_NEXT_USIM_CACHE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
PB_USIM_CACHE *pbm_get_next_usim_cache(pbm_record_id_type rec_id,
                                       pbm_field_id_e_type field_id,
                                       int *index)
{
   int i;
   pbm_pb_set_type pb_set;
   PB_CACHE *pb_cache;
   uint16 location;
   PB_MASTER_USIM_CACHE *m_usim_cache;
   PB_USIM_CACHE *usim_cache = NULL;

   PBM_CHECK_PTR_RET(index, NULL);
   if (*index < 0)
   {
      UIM_MSG_ERR_1("Invalid index %d", *index);
      return NULL;
   }

   pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   PBM_CHECK_PTR_RET(pb_cache, NULL);

   location =  pbm_record_id_to_location(rec_id);
   pb_set = pbm_pb_record_index_to_pb_set(pb_cache,location);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return NULL; 
   }

   m_usim_cache = pb_cache->m_usim_cache[pb_set];
   if (!m_usim_cache)
      return NULL;

   for (i = *index + 1; i <= m_usim_cache->num_caches; i++)
   {
      usim_cache = m_usim_cache->usim_cache_array[i];
      if (!usim_cache)
         continue;

      if (pbm_file_id_to_field_id(usim_cache->file_id) == field_id)
      {
         *index = i;
         return usim_cache;
      }
   }

   return NULL;
}


/*===========================================================================
FUNCTION PBM_GROUP_FIELDS

DESCRIPTION
  Group fields together. Currently we care only about grouping group ids
  together.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
/*lint -e{818} record can be const, but stuff pointed to by record isn't */
void pbm_group_fields(pbm_addr_cache_s_type *record)
{
   int i;
   pbm_field_s_type field;
   uint32 offset;
   const uint32 size_one_field = ALIGNED_SIZE(sizeof(pbm_field_s_type));
   int num_grp_id = 0;

   if (!record)
      return;

   for ( i = 0; i < record->num_fields; i++)
   {
      if (record->fields[i].field_id == PBM_FIELD_GRPNAME_ID)
      {
         /* if the field needs to move... */
         if (i != num_grp_id)
         {
            /* swap fields */
            field = record->fields[i];
            record->fields[i] = record->fields[num_grp_id];
            record->fields[num_grp_id] = field;

            /* adjust buffer offsets */
            offset = (uint32) (i - num_grp_id) * size_one_field;
            record->fields[i].buffer_offset -= offset;
            record->fields[num_grp_id].buffer_offset += offset;
         }

         num_grp_id++;
      }
   }
}


/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_pb_record_index_to_pb_set(const PB_CACHE *cache, int index)
{
   int i;
   int records_accumulator = 0;
   pbm_pb_set_type pb_set = PBM_USIM_PB_INVALID;

   PBM_CHECK_PTR_RET(cache, PBM_USIM_PB_INVALID);

   if ((index > cache->num_of_records) || (index < 1))
   {
      UIM_MSG_ERR_2("pbm_pb_record_index_to_pb_set out of bounds index %d, max %d",
                  (index), (cache)->num_of_records);
   }

   for (i=0; i<cache->num_files; i++)
   {
      records_accumulator += cache->records_in_pb_set[i];
      if (index <= records_accumulator) /* 1 based numbering */
      {
         pb_set = (pbm_pb_set_type) i;
         break;
      }
   }

   if (pb_set < PBM_USIM_PB_ONE || pb_set >= PBM_MAX_NUM_PB_SETS)
   {
      UIM_MSG_ERR_2("pb_set out of bounds index %d, max %d",
                  pb_set, PBM_MAX_NUM_PB_SETS);
      pb_set = PBM_USIM_PB_INVALID;
   }

   return pb_set;
}


/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given SYNC2 index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_sync2_record_index_to_pb_set(const PB_SYNC2_CACHE *cache, int index)
{
   int i;
   int records_accumulator = 0;
   pbm_pb_set_type pb_set;

   pb_set = PBM_USIM_PB_INVALID;

   PBM_CHECK_PTR_RET(cache, PBM_USIM_PB_INVALID);

   if ((index > cache->num_of_records) || (index < 1))
   {
      UIM_MSG_ERR_2("pbm_sync2_record_index_to_pb_set out of bounds index %d, max %d",
                  (index), (cache)->num_of_records);
   }

   for (i=0; i<cache->num_files; i++)
   {
      records_accumulator += cache->records_in_pb_set[i];
      if (index <= records_accumulator) /* 1 based numbering */
      {
         pb_set = (pbm_pb_set_type) i;
         break;
      }
   }

   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return PBM_USIM_PB_INVALID; 
   }

   return pb_set;
}


/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_pb_record_index_to_filename(const PB_CACHE *cache, int index)
{
   pbm_pb_set_type pb_set;
   pbm_file_type file_id = PBM_FILE_NONE;

   PBM_CHECK_PTR_RET(cache, PBM_FILE_NONE);

   if (cache->num_of_records && cache->num_files && index)
   {
      pb_set = pbm_pb_record_index_to_pb_set(cache, index);
      if ((pb_set >= PBM_USIM_PB_ONE) && (pb_set < PBM_MAX_NUM_PB_SETS))
      {
         file_id = cache->file_ids[pb_set];
      }
   }

   return file_id;
}


/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the file_id for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_sync2_record_index_to_filename(const PB_SYNC2_CACHE *cache, int index)
{
   pbm_pb_set_type pb_set;
   pbm_file_type file_id = PBM_FILE_NONE;

   PBM_CHECK_PTR_RET(cache, PBM_FILE_NONE);

   if (cache->num_of_records && cache->num_files && index)
   {
      pb_set = pbm_sync2_record_index_to_pb_set(cache, index);
      if ((pb_set >= PBM_USIM_PB_ONE) && (pb_set < PBM_MAX_NUM_PB_SETS))
      {
         file_id = cache->file_ids[pb_set];
      }
   }

   return file_id;
}


/*===========================================================================
FUNCTION pbm_record_id_to_fileloc

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_fileloc(pbm_record_id_type rec_id)
{
   PB_CACHE *pb_cache;
   uint16 location;
   int index = 0;

   pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   location = pbm_record_id_to_location(rec_id);

   PBM_CHECK_PTR_RET(pb_cache, 0);

   index = pbm_pb_record_index_to_fileloc(pb_cache, location);
   if (index < 0)
   {
      UIM_MSG_ERR_1("Invalid record id %d", rec_id);
      return 0;
   }

   return(uint16)index;
}


/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_sync2_record_index_to_fileloc(const PB_SYNC2_CACHE *cache, int index)
{
   pbm_pb_set_type pb_set;
   int location;

   PBM_CHECK_PTR_RET(cache, index);

   location = index;
   pb_set = pbm_sync2_record_index_to_pb_set(cache, index);

   if (pb_set >= PBM_USIM_PB_FOUR)
   {
      location -= cache->records_in_pb_set[2];
   }
   if (pb_set >= PBM_USIM_PB_THREE)
   {
      location -= cache->records_in_pb_set[1];
   }
   if (pb_set >= PBM_USIM_PB_TWO)
   {
      location -= cache->records_in_pb_set[0];
   }

   return location;
}

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PB_SET

DESCRIPTION
  Map a record ID to a phonebook set

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_record_id_to_pb_set (pbm_record_id_type rec_id)
{
   PB_CACHE *pb_cache;
   uint16 location;
   pbm_pb_set_type pb_set;

   pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   location = pbm_record_id_to_location(rec_id);

   PBM_CHECK_PTR_RET(pb_cache, PBM_USIM_PB_INVALID);

   pb_set = pbm_pb_record_index_to_pb_set(pb_cache, location);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return PBM_USIM_PB_INVALID; 
   }

   return pb_set;
}

/*===========================================================================
FUNCTION pbm_fill_adn_sfi

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
  pbm_return_type pbm_fill_adn_sfi(pbm_adn_record_id_type rec_id, byte *buffer,
                                               pbm_session_enum_type session_type)
{
   PB_CACHE             *pb_cache;
   PB_MASTER_USIM_CACHE *m_usim_cache;
   pbm_pb_set_type       pb_set = pbm_adn_record_id_to_pb_set(session_type,rec_id);
   int                   location = pbm_record_id_to_location(rec_id);
   int                   index = 0;

   PBM_CHECK_PTR_RET(buffer, PBM_ERROR);

   if ( ! pbm_validate_pb_set(pb_set) )
   {
      return PBM_ERROR; 
   }

   pb_cache = pbm_pb_id_to_cache(
               pbm_device_type_to_phonebook_type(session_type,PBM_ADN));
   PBM_CHECK_PTR_RET(pb_cache, PBM_ERROR);

   m_usim_cache = pb_cache->m_usim_cache[pb_set];
   PBM_CHECK_PTR_RET(m_usim_cache, PBM_ERROR);

   index = pbm_pb_record_index_to_fileloc(pb_cache, location);

   buffer[0] = (uint8) m_usim_cache->adn_sfi;
   buffer[1] = (uint8)index;

   return PBM_SUCCESS;
}


/*===========================================================================
FUNCTION pbm_allocate_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_index(pbm_record_id_type rec_id, const pbm_field_s_type *fields,
                           int num_fields, uint8 **prev_index,
                           uint8 **curr_index)
{
   PB_CACHE *cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   PB_MASTER_USIM_CACHE *m_usim_cache;
   pbm_addr_cache_s_type *prev_rec;
   uint8 *curr = NULL;
   uint8 *prev = NULL;
   pbm_pb_set_type pb_set;

   PBM_CHECK_PTR3_RET(prev_index, curr_index, cache, FALSE);

   if (pbm_is_usim_required(rec_id, fields, num_fields))
   {
      pb_set = pbm_record_id_to_pb_set(rec_id);
      if ( ! pbm_validate_pb_set(pb_set) )
      { 
         return FALSE; 
      }

      m_usim_cache = cache->m_usim_cache[pb_set];

      PBM_CHECK_PTR_RET(m_usim_cache, FALSE); // Perhaps there is no USIM.

      PBM_MEM_ALLOC(curr, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
      if (!curr)
      {
         return FALSE;
      }
      memset(curr, 0, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
   }

   prev_rec = cache->pb_cache_array[pbm_record_id_to_location(rec_id)];
   if (prev_rec &&
       pbm_is_usim_required(rec_id, prev_rec->fields, prev_rec->num_fields))
   {
      pb_set = pbm_record_id_to_pb_set(rec_id);
      if ( ! pbm_validate_pb_set(pb_set) )
      {
          PBM_MEM_FREEIF(curr);
          return FALSE; 
      }

      m_usim_cache = cache->m_usim_cache[pb_set];

      PBM_CHECK_PTR_RET(m_usim_cache, FALSE);

      PBM_MEM_ALLOC(prev, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
      if (!prev)
      {
        PBM_MEM_FREEIF(curr);
        return FALSE;
      }
      memset(prev, 0, (uint32) (m_usim_cache->num_caches+1) * sizeof(uint8));
   }

   *curr_index = curr;
   *prev_index = prev;

   return TRUE;
}


/*===========================================================================
FUNCTION pbm_free_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_index(uint8 *prev_index, uint8 *curr_index)
{
   PBM_MEM_FREEIF(prev_index);
   PBM_MEM_FREEIF(curr_index);
}


/*===========================================================================
FUNCTION pbm_copy_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_usim_index(pbm_record_id_type rec_id,
                            uint8 *index_array)
{
   int i;
   int index = pbm_record_id_to_fileloc(rec_id);
   PB_MASTER_USIM_CACHE *m_usim_cache;
   PB_CACHE *pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   pbm_pb_set_type pb_set;

   PBM_CHECK_PTR2_RET(index_array, pb_cache, FALSE);
   pb_set = pbm_record_id_to_pb_set(rec_id);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return FALSE; 
   }
   m_usim_cache = pb_cache->m_usim_cache[pb_set];
   PBM_CHECK_PTR_RET(m_usim_cache, FALSE);

   memset(index_array, 0, (uint32)(m_usim_cache->num_caches+1) * sizeof(uint8));

   for (i = 1; i <= m_usim_cache->num_caches; i++)
      if (m_usim_cache->usim_cache_array[i]) /* if cache exists */
         index_array[i] = m_usim_cache->index_array[i][index];

   return TRUE;
}


/*===========================================================================
FUNCTION pbm_gsm_modified

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsm_modified(pbm_record_id_type rec_id)
{
   PB_CACHE *pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   uint16 location = pbm_record_id_to_location(rec_id);
   PB_SYNC2_CACHE *sync2_cache;
   boolean ret = FALSE;

   if (pb_cache && pb_cache->m_sync_cache &&
       (sync2_cache = pb_cache->m_sync_cache->pbc_cache) != NULL)
   {
     if(location <= sync2_cache->num_of_records)
     {
       if (sync2_cache->data_array &&
          (sync2_cache->data_array[location] & PBM_PBC_MODIFIED_MASK))
         ret = TRUE;
     }
   }

   return ret;
}


/*===========================================================================
FUNCTION pbm_is_rec_hidden

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_rec_hidden(pbm_record_id_type rec_id)
{
   PB_CACHE *pb_cache = pbm_pb_id_to_cache(pbm_session_record_id_to_phonebook_type(rec_id));
   uint16 location = pbm_record_id_to_location(rec_id);
   pbm_session_enum_type session_type = pbm_record_id_to_session_type (rec_id);
   PB_SYNC2_CACHE *sync2_cache;
   boolean ret = FALSE;
   PBM_CHECK_PTR_RET(pb_cache, FALSE);
   if(pb_cache->pb_id.device_type == PBM_ADN)
   {
     if (pb_cache->m_sync_cache &&
       (sync2_cache = pb_cache->m_sync_cache->pbc_cache) != NULL)
     {
       if(location <= sync2_cache->num_of_records)
       {
         if(pbm_session_info[session_type].dir_index != 0)
         {
           if (sync2_cache->data_array &&
               ((sync2_cache->data_array[location] & PBM_PBC_HIDDEN_MASK)
                          == pbm_session_info[session_type].dir_index))
              ret = TRUE;
         }
        }
     }
   }

   return ret;
}

/*===========================================================================
FUNCTION PBM_NOTIFY

DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_notify(pbm_notify_data_s_type *notify_data)
{
   pbm_notify_node_s_type *curr, *temp;
   pbm_notify_node_s_type *node;
   pbm_notify_node_s_type *temp_pbm_notify_list = NULL;

   rex_enter_crit_sect(&pbm_crit_sect);
   curr = pbm_notify_list;
   while (curr)
   {
      if (curr->notify_func)
      {
         /* add the node into temp list */
		 /*node = pbm_alloc_notify_list_node();*/
		 PBM_MEM_ALLOC(node, sizeof(pbm_notify_node_s_type));
		 PBM_CHECK_PTR_BREAK_2(node); /* free the nodes before return and call as many temp nodes have been allocated */
         /* each node is being added at the start of the list */
		 node->next = temp_pbm_notify_list;
		 node->notify_func = curr->notify_func;
		 node->user_data = curr->user_data;
		 temp_pbm_notify_list = node;
      } else
      {
         UIM_MSG_ERR_1("Invalid entry %p found in notify list", curr);
         break;
      }
      curr = curr->next;
   }
   rex_leave_crit_sect(&pbm_crit_sect);
   /* loop to call notify functions now */
   curr = temp_pbm_notify_list;
   while (curr)
   {
      if (curr->notify_func)
      {
         (*curr->notify_func)(curr->user_data, notify_data);
      } else
      {
         UIM_MSG_ERR_1("Invalid entry %p found in notify list", curr);
         break;
      }
      curr = curr->next;
   }
   /* free the temp list */
   temp = curr  = temp_pbm_notify_list;
   while (temp)
   {
      temp = curr->next;
	  PBM_MEM_FREEIF(curr);
	  curr = temp;
}

}

/*===========================================================================
FUNCTION pbm_add_notify

DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_add_notify(pbm_notify_event_e_type evt,pbm_session_event_data_u_type evt_data)
{
    do
    {
        pbm_notify_evt_s_type *pNotify_evt_cmd = NULL;
        /* Allocate PBM notify event command */
        pNotify_evt_cmd = (pbm_notify_evt_s_type *)
                pbm_cmd_alloc(sizeof(pbm_notify_evt_s_type));
        if (!pNotify_evt_cmd)
        {
            UIM_MSG_ERR_0("Unable to allocate PBM command");
            break;
        }

        switch(evt)
        {

            case PBM_EVENT_REC_ADD:
            case PBM_EVENT_REC_UPDATE:
            case PBM_EVENT_REC_DELETE:
            case PBM_EVENT_REC_FAILED:
                pNotify_evt_cmd->notify_data.session_data.rec_id = evt_data.rec_id;
                //since this is the old rec_id we would need just 16 LSBs
                pNotify_evt_cmd->notify_data.data.rec_id = (uint16)evt_data.rec_id;
                UIM_MSG_HIGH_2("PBM sending event %d to clients, for record id %d",
                              evt,
                              evt_data.rec_id);
                break;
            case PBM_EVENT_PB_REFRESH_START:
            case PBM_EVENT_PB_REFRESH_DONE:
            case PBM_EVENT_PB_READY:
            case PBM_EVENT_PH_UNIQUE_IDS_VALIDATED:
            case PBM_EVENT_SESSION_INIT_DONE:
            case PBM_EVENT_CACHE_STATUS_UPDATE:
                pNotify_evt_cmd->notify_data.session_data.pb_id= evt_data.pb_id;
                  //copy the device type into old event
                pNotify_evt_cmd->notify_data.data.pb_id = evt_data.pb_id.device_type;
                UIM_MSG_HIGH_3("PBM sending event %d to clients, for provision type %d, and slot id %d",
                              evt,
                              evt_data.pb_id.prov_type,evt_data.pb_id.slot_id);
                break;
            case PBM_EVENT_LOCKED:
            case PBM_EVENT_UNLOCKED:
                pNotify_evt_cmd->notify_data.session_data.lock_type= evt_data.lock_type;
                pNotify_evt_cmd->notify_data.data.lock_type= evt_data.lock_type;
                UIM_MSG_HIGH_2("PBM sending event %d to clients, for lock type %d",
                              evt,
                              evt_data.lock_type);
                break;
            case PBM_EVENT_HIDDEN_CONTACTS_STATUS:
                pNotify_evt_cmd->notify_data.session_data.hidden_contact_status.pb_id= evt_data.hidden_contact_status.pb_id;
                pNotify_evt_cmd->notify_data.session_data.hidden_contact_status.status= evt_data.hidden_contact_status.status;
                //copy the device type into old event
                pNotify_evt_cmd->notify_data.data.hidden_contact_status.pb_id = evt_data.hidden_contact_status.pb_id.device_type;
                pNotify_evt_cmd->notify_data.data.hidden_contact_status.status = evt_data.hidden_contact_status.status;
                UIM_MSG_HIGH_3("PBM sending event %d to clients, status %d,prov_type %d",
                              evt,
                              evt_data.hidden_contact_status.status,
                              evt_data.hidden_contact_status.pb_id.prov_type);
                break;
            case PBM_EVENT_SIM_INIT_DONE:
                pNotify_evt_cmd->notify_data.session_data.sim_id = evt_data.sim_id;
                pNotify_evt_cmd->notify_data.data.sim_id = evt_data.sim_id;
                UIM_MSG_HIGH_2("PBM sending event %d to clients, for slot id %d",
                              evt,
                              evt_data.sim_id);
                break;
        }

        pNotify_evt_cmd->cmd_type = PBM_CMD_NOTIFY_EVT;
        pNotify_evt_cmd->notify_data.event = evt;
        pbm_notify_evt_q_put(PBM_CMD_PTR(pNotify_evt_cmd));
    } while (FALSE);

}


/*===========================================================================
FUNCTION pbm_hidden_key_notify

DESCRIPTION

  This function is used to notify an hidden key verified/not verified event to all registered clients.
	Clients need to update their ADN caches as per this event. Verified status is for future use, in case
	separate event is to be send for Hidden key verified and not verified.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_hidden_key_notify(pbm_session_enum_type session_type, boolean verified_status)
{
  /* use the PB id as event data */
  /* use add notify in place of this function */
  pbm_session_event_data_u_type evt_data = {0};
  evt_data.hidden_contact_status.pb_id = pbm_device_type_to_phonebook_type(session_type, PBM_ADN);

  if(verified_status)
  {
    evt_data.hidden_contact_status.status = PBM_HIDDEN_CONTACTS_VALID;
  }
  else
  {
    evt_data.hidden_contact_status.status = PBM_HIDDEN_CONTACTS_INVALID;
  }
  pbm_add_notify(PBM_EVENT_HIDDEN_CONTACTS_STATUS, evt_data);

}



/*===========================================================================
FUNCTION PBM_APPEND_PHONE_UID_FIELD

DESCRIPTION
  This function is used to append the field PBM_FIELD_PH_UID to a field list with a new
  phone unique ID. ph_unique_id is an input parameter,

DEPENDENCIES
  Must be called from within pbm_crit_sect critical section.

SIDE EFFECTS
  Frees the existing field list. New memory is allocated for the appended field list, and the pointer
  to it is returned in the parameter.

COMMENTS

SEE ALSO
===========================================================================*/

pbm_return_type pbm_append_phone_uid_field(uint64 ph_unique_id, pbm_field_s_type **ppfields,
                                           uint16 *pnum_fields)
{
   pbm_return_type nRet = PBM_SUCCESS;
   int i;
   uint16 new_num_fields;
   pbm_field_s_type *new_fields = NULL;
   uint32 *new_sizes = NULL;

   PBM_CHECK_PTR3_RET(ppfields, pnum_fields, *ppfields, PBM_ERROR);

   /* The only way to do this is to copy the entire field array to a new field array
    * with the appended field. */

   new_num_fields = *pnum_fields + 1;

   do
   {
      /* Create array of field sizes to allocate the fields from */
      PBM_MEM_ALLOC(new_sizes, new_num_fields*sizeof(uint32));
      PBM_CHECK_PTR_BREAK(new_sizes, nRet, PBM_ERROR);

      for (i = 0; i < *pnum_fields; i++)
      {
         new_sizes[i] = (*ppfields)[i].data_len;
      }

      new_sizes[i] = PBM_DDWORD_SIZE;

      /* now allocate the field structure, based on the sizes */
      if (!pbm_allocate_fields_internal(&new_fields, new_num_fields, new_sizes))
      {
         nRet = PBM_ERROR;
         break;
      }

      /* copy over the fields */
      for (i = 0; i < *pnum_fields; i++)
      {
         pbm_fill_field(&new_fields[i], (*ppfields)[i].field_id, (*ppfields)[i].field_type,
                        pbm_get_field_data_ptr(*ppfields,i), (*ppfields)[i].data_len);
      }

      /* Fill in the UID field */
      pbm_fill_field(&new_fields[i], PBM_FIELD_PH_UID, PBM_FT_BINARY,
                     (byte *) &ph_unique_id, sizeof(ph_unique_id));

      /* free the old fields */
      pbm_free_fields_internal(ppfields, *pnum_fields);

      /* set pointers to the new fields */
      *ppfields = new_fields;
      *pnum_fields = new_num_fields;
   }
   while (FALSE); /*lint !e717 */

   PBM_MEM_FREEIF(new_sizes);

   if (nRet != PBM_SUCCESS)
   {
      if (new_fields)
      {
         pbm_free_fields_internal(&new_fields, new_num_fields);
      }
   }

   return nRet;
}


/*===========================================================================
FUNCTION PBM_FIND_PHONE_UID_FIELD

DESCRIPTION
  This function locates the PH UID field from a field list and returns the
  index to the field. Returns TRUE on found, FALSE if not found.

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/

boolean pbm_find_phone_uid_field(const pbm_field_s_type *fields, int num_fields, int *index)
{
   int i;

   if (!fields)
   {
      return FALSE;
   }

   /* Locate PH_UID field in update field list. */
   for (i = 0; i < num_fields; i++)
   {
      if (fields[i].field_id == PBM_FIELD_PH_UID)
      {
         if (index)
         {
            *index = i;
         }
         return TRUE;
      }
   }

   return FALSE;
}





/*===========================================================================
FUNCTION PBM_DEVICE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given file id
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
PB_CACHE *pbm_device_id_to_cache(pbm_session_enum_type session_type,
                                 pbm_device_type device_id)
{
   PBM_VALIDATE_SESSION_RET(session_type, NULL);

   switch (device_id)
   {
      case PBM_EFS:
         return &efs_cache;

      case PBM_SDL:
         return &sdl_cache;

      case PBM_MISS:
         return &missed_calls_cache;

      case PBM_RCVD:
         return &incoming_calls_cache;

      case PBM_DIALED:
         return &outgoing_calls_cache;

      case PBM_ADN:
         return &adn_cache[session_type];

      case PBM_FDN:
         return &fdn_cache[session_type];

      case PBM_SDN:
         return &sdn_cache[session_type];

      case PBM_LND:
         return &lnd_cache[session_type];

      case PBM_MSISDN:
         return &msisdn_cache[session_type];

      case PBM_MBN:
         return &mbn_cache[session_type];

      case PBM_GRPNAME:
         return &grpname_cache[session_type];
      case PBM_AAS:
         return &aas_cache[session_type];
      case PBM_OCI:
         return &oci_cache[session_type];

      case PBM_ICI:
         return &ici_cache[session_type];

      case PBM_MBDN:
         return &mbdn_cache[session_type];
      case PBM_ECC:
         return &ecc_sim_cache[session_type];

      case PBM_ECC_OTHER:
         return &ecc_other_cache;

      default:
         return NULL;
   }
}


/*===========================================================================
FUNCTION PBM_OLD_RECID_TO_NEW_RECID

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_old_recid_to_new_recid(uint16 old_rec_id,pbm_record_id_type *new_rec_id)
{
  pbm_return_type ret = PBM_SUCCESS;
  pbm_device_type device_type;
  pbm_phonebook_type pb_id = {PBM_GPB};

  device_type = pbm_record_id_to_phonebook(old_rec_id);
  if((ret = pbm_sim_device_type_to_phonebook_type(device_type,&pb_id)) == PBM_SUCCESS)
  {
    *new_rec_id = pbm_session_location_to_record_id(pb_id,pbm_record_id_to_location(old_rec_id));
  }
  return ret;
}


  /*===========================================================================
  FUNCTION PBM_PB_ID_TO_LEGACY_DEVICETYPE

  DESCRIPTION

  DEPENDENCIES

  SIDE EFFECTS
  ===========================================================================*/
  pbm_return_type pbm_pb_id_to_legacy_devicetype(pbm_phonebook_type pb_id,pbm_device_type *device_type)
  {
    if(pb_id.device_type == PBM_EFS
        || pb_id.device_type == PBM_SDL
        || pb_id.device_type == PBM_MISS
        || pb_id.device_type == PBM_RCVD
        || pb_id.device_type == PBM_ECC_OTHER
        || pb_id.device_type == PBM_DIALED )
    {
      //all the above phonebooks will be stored in phone memory
      //in these cases only device_type is enough..
      *device_type =  pb_id.device_type;
    }
    if((pb_id.slot_id == (uint16)PBM_SLOT_2 && pb_id.pb_category == PBM_GPB)
        || (pb_id.prov_type == PBM_PROVISION_GW_SECONDARY && pb_id.pb_category == PBM_LPB)
        || (pb_id.prov_type == PBM_PROVISION_1X_SECONDARY && pb_id.pb_category == PBM_LPB) )
    {
      switch(pb_id.device_type)
      {
        case PBM_ADN:
          *device_type = PBM_ADN2;
          break;

        case PBM_FDN:
          *device_type = PBM_FDN2;
          break;

        case PBM_BDN:
          *device_type = PBM_BDN2;
          break;

        case PBM_SDN:
          *device_type = PBM_SDN2;
          break;

        case PBM_MSISDN:
          *device_type = PBM_MSISDN2;
          break;

        case PBM_MBN:
          *device_type = PBM_MBN2;
          break;

        case PBM_ECC2:
          *device_type = PBM_ECC;
          break;

        case PBM_MBDN:
          *device_type = PBM_MBDN2;
          break;

        default:
          UIM_MSG_ERR_1("Unexpected device_type %d", pb_id.device_type);
          *device_type = PBM_DEFAULT;
          return PBM_ERROR;
      }
    }
    else
    {
       *device_type = pb_id.device_type;
    }
    return PBM_SUCCESS;
  }

/*===========================================================================
FUNCTION PBM_NEW_RECID_TO_OLD_RECID

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_new_recid_to_old_recid(uint16 *old_rec_id,pbm_record_id_type new_rec_id)
{
  pbm_return_type ret = PBM_ERROR;
  pbm_device_type device_type;
  pbm_phonebook_type pb_id = {PBM_GPB};
  pb_id = pbm_session_record_id_to_phonebook_type(new_rec_id);
  if((ret =pbm_pb_id_to_legacy_devicetype(pb_id,&device_type)) == PBM_SUCCESS)
  {
    *old_rec_id = pbm_location_to_record_id(device_type,
                                          pbm_session_record_id_to_location(new_rec_id));
  }
  return ret;
}

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_pb_record_index_to_fileloc(const PB_CACHE *cache, int index)
{
   pbm_pb_set_type pb_set;
   int location;

   PBM_CHECK_PTR_RET(cache, index);

   location = index;
   pb_set = pbm_pb_record_index_to_pb_set(cache, index);
   if (pb_set >= PBM_USIM_PB_FOUR)
   {
      location -= cache->records_in_pb_set[2];
   }
   if (pb_set >= PBM_USIM_PB_THREE)
   {
      location -= cache->records_in_pb_set[1];
   }
   if (pb_set >= PBM_USIM_PB_TWO)
   {
      location -= cache->records_in_pb_set[0];
   }

   return location;
}


/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PB_SET

DESCRIPTION
  Map a record ID to a phonebook set

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_adn_record_id_to_pb_set(pbm_session_enum_type session_type,
                                            pbm_adn_record_id_type rec_id)
{
   PB_CACHE *pb_cache;
   uint16 location;
   pbm_pb_set_type pb_set;

   pb_cache = pbm_device_id_to_cache(session_type,
                                   (pbm_device_type)pbm_adn_record_id_to_file_device_type(rec_id));
   location = pbm_record_id_to_location(rec_id);

   PBM_CHECK_PTR_RET(pb_cache, PBM_USIM_PB_INVALID);

   pb_set = pbm_pb_record_index_to_pb_set(pb_cache, location);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return PBM_USIM_PB_INVALID; 
   }

   return pb_set;
}

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_ID

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_file_id_to_pb_id(pbm_session_enum_type session_type,
                                        pbm_file_type file_id)
{
   pbm_phonebook_type pb_id = {PBM_GPB};

   pb_id.slot_id = pbm_session_info[session_type].slot_id;
   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
   pb_id.prov_type = pbm_session_type_to_provision_type(session_type);

   switch (file_id)
   {
      case PBM_FILE_ADN:
      case PBM_FILE_ADN1:
      case PBM_FILE_ADN2:
      case PBM_FILE_ADN3:
         pb_id.device_type = PBM_ADN;
         break;

      case PBM_FILE_FDN:
         pb_id.device_type = PBM_FDN;
         break;

      case PBM_FILE_LND:
         pb_id.device_type = PBM_LND;
         break;

      case PBM_FILE_SDN:
         pb_id.device_type = PBM_SDN;
         break;

      case PBM_FILE_MSISDN:
         pb_id.device_type = PBM_MSISDN;
         break;

      case PBM_FILE_MBN:
         pb_id.device_type = PBM_MBN;
         break;

      case PBM_FILE_GAS:
         pb_id.device_type = PBM_GRPNAME;
         break;
      case PBM_FILE_AAS:
         pb_id.device_type = PBM_AAS;
         break;
      case PBM_FILE_ICI:
         pb_id.device_type = PBM_ICI;
         break;

      case PBM_FILE_OCI:
         pb_id.device_type = PBM_OCI;
         break;

      case PBM_FILE_MBDN:
         pb_id.device_type = PBM_MBDN;
         break;
      case PBM_FILE_ECC:
         pb_id.device_type = PBM_ECC;
         break;
      default:
         UIM_MSG_ERR_1("pbm_file_id_to_pb_id called for invalid file %d", file_id);
         pb_id.device_type = PBM_DEFAULT;
         break;
   }
   return pb_id;
}
/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_FILE_ID

DESCRIPTION
  This function will find the file id corresponding to the given record id.

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_record_id_to_file_id(pbm_record_id_type rec_id)
{
   pbm_phonebook_type pb_id = pbm_session_record_id_to_phonebook_type(rec_id);
   int location = pbm_record_id_to_location(rec_id);
   PB_CACHE *pb_cache;
   pbm_pb_set_type pb_set;

   pb_cache = pbm_pb_id_to_cache(pb_id);
   if (!pb_cache)
   {
      UIM_MSG_ERR_1("Invalid rec id %d", rec_id);
      return PBM_FILE_MAX_REQUESTS;
   }

   pb_set = pbm_pb_record_index_to_pb_set(pb_cache, location);
   if ( ! pbm_validate_pb_set(pb_set) )
   {
  	  return PBM_FILE_MAX_REQUESTS; 
   }

   return pb_cache->file_ids[pb_set];
}
/*===========================================================================
FUNCTION pbm_device_type_to_phonebook_type

DESCRIPTION
DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_device_type_to_phonebook_type(pbm_session_enum_type session_type,
                                                     pbm_device_type device_type)
{
   pbm_phonebook_type pb_id = {PBM_GPB};

   pb_id.prov_type = pbm_session_type_to_provision_type(session_type);
   pb_id.slot_id = pbm_session_info[session_type].slot_id;
   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
   pb_id.device_type = device_type;

   return pb_id;
}

/*===========================================================================
FUNCTION PBM_MAPPING_TO_SESSION

DESCRIPTION
retrieves session id from record info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_mapping_to_session(pbm_pb_category_enum_type pb_category,
                                             pbm_slot_type slot_id,
                                             pbm_provision_enum_type prov_type)
{
   pbm_session_enum_type i = PBM_SESSION_DEFAULT;
   boolean found = FALSE;
   if ( PBM_GPB == pb_category )
   {
     switch(slot_id)
     {
         case PBM_SLOT_1:
           {
             i = PBM_SESSION_GPB_1;
             found = TRUE;
           }
           break;
         case PBM_SLOT_2:
           {
             i = PBM_SESSION_GPB_2;
             found = TRUE;
           }
           break;
#ifdef FEATURE_TRIPLE_SIM
         case PBM_SLOT_3:
           {
             i = PBM_SESSION_GPB_3;
             found = TRUE;
           }
           break;
#endif 
         default:
           break;
     }

   }
   else
   {
       for ( i = (int) PBM_SESSION_DEFAULT; i < (int) PBM_SESSION_MAX; i++ )
       {
              /* local phonebook, check only mandatory prov info */
              if (((int) PBM_SESSION_LPB_GW == i &&
                   PBM_PROVISION_GW == prov_type) ||
                  ((int) PBM_SESSION_LPB_1X == i &&
                   PBM_PROVISION_1X == prov_type)
#ifdef FEATURE_DUAL_SIM
                ||( (int) PBM_SESSION_LPB_GW_SECONDARY == i &&
                   PBM_PROVISION_GW_SECONDARY == prov_type) ||
                  ((int)PBM_SESSION_LPB_1X_SECONDARY == i &&
                   PBM_PROVISION_1X_SECONDARY == prov_type)
#ifdef FEATURE_TRIPLE_SIM
               ||( (int) PBM_SESSION_LPB_GW_TERTIARY == i &&
              PBM_PROVISION_GW_TERTIARY == prov_type) ||
             ((int)PBM_SESSION_LPB_1X_TERTIARY == i &&
              PBM_PROVISION_1X_TERTIARY == prov_type)
#endif
#endif
                  )
              {
                 found = TRUE;
                 break;
              }
       }
   }

   if ( TRUE == found && i < (int) MAX_UIM_APPS )
   {
      /* success - return session id*/
      return i;
   }

   UIM_MSG_ERR_3("Invalid parameters %d %d %d",pb_category,slot_id,prov_type);
   return PBM_SESSION_MAX;
}

/*===========================================================================
FUNCTION pbm_record_id_to_session_type

DESCRIPTION
Converts a record ID to a session ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_record_id_to_session_type (pbm_record_id_type record_id)
{
   pbm_phonebook_type pb_id = {PBM_GPB};
   pb_id = pbm_session_record_id_to_phonebook_type(record_id);
   return pbm_mapping_to_session (pb_id.pb_category, (pbm_slot_type)pb_id.slot_id, pb_id.prov_type);
}

/*===========================================================================
FUNCTION pbm_mmgsdi_session_to_pbm_session

DESCRIPTION
retrieves session id from record info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_mmgsdi_session_to_pbm_session (
  mmgsdi_session_id_type session_id
)
{
  pbm_session_enum_type   pb_session = PBM_SESSION_DEFAULT;

  for (pb_session = PBM_SESSION_DEFAULT; pb_session < PBM_SESSION_MAX; pb_session++ )
  {
    if (session_id == pbm_session_info[pb_session].session_id)
    {
      return pb_session;
    }
  }
  UIM_MSG_ERR_0("Session out of range, picking default");
  return PBM_SESSION_DEFAULT;
} /* pbm_mmgsdi_session_to_pbm_session */

/*===========================================================================
FUNCTION pbm_get_internal_field_info
DESCRIPTION
  This function is used to return the exact field info structure among .
g_rest_field_info,g_sim_field_info,g_efs_field_info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_internal_field_info(pbm_phonebook_type pb_id,
                                          pbm_internal_field_info *field_info)
{
   pbm_session_enum_type session_type;
   pbm_return_type ret = PBM_ERROR;
   PB_CACHE *cache = NULL;
   int field_group = pbm_pb_id_to_fieldgroup(pb_id);
   PBM_CHECK_PTR_RET(field_info,PBM_ERROR);
   if (!(field_group < NUM_GROUPS && field_group > INVALID_GROUP))
   {
      UIM_MSG_ERR_2("Invalid group: phonebook %d and category %d", 
                     pb_id.device_type,pb_id.pb_category);
      return PBM_ERROR;
   }
   session_type = pbm_mapping_to_session(pb_id.pb_category, (pbm_slot_type) pb_id.slot_id,pb_id.prov_type);
   if (( field_group != ECC_GROUP) && ((ret = check_pb_ready(pb_id)) != PBM_SUCCESS) )
   {
      return ret;
   }
    if ( (session_type < PBM_SESSION_MAX)  || (!pbm_action_requires_sim(pb_id))
          || (pb_id.device_type == PBM_ECC))
    {
        switch(field_group)
        {
             case ADN_GROUP:
               if(session_type < PBM_SESSION_MAX)
               memscpy(field_info,sizeof(pbm_g_efs_field_info_s_type),
                     &(g_sim_field_info[session_type][0]),sizeof(pbm_g_other_field_info_s_type));
               break;
             case FDN_GROUP:
               if(session_type < PBM_SESSION_MAX)
               memscpy(field_info,sizeof(pbm_g_efs_field_info_s_type),
                      &(g_sim_field_info[session_type][1]),sizeof(pbm_g_other_field_info_s_type));
               break;
             case GRPNAME_GROUP:
               if(session_type < PBM_SESSION_MAX)
               memscpy(field_info,sizeof(pbm_g_efs_field_info_s_type),
                      &(g_sim_field_info[session_type][2]),sizeof(pbm_g_other_field_info_s_type));
               break;
             case AAS_GROUP:
               if(session_type < PBM_SESSION_MAX)
               memscpy(field_info,sizeof(pbm_g_efs_field_info_s_type),
                      &(g_sim_field_info[session_type][3]),sizeof(pbm_g_other_field_info_s_type));
               break;
             case SIM_GROUP:
             case SDL_GROUP:
             case CALLHISTORY_GROUP:
             case ECC_GROUP:
             case ECC_OTHER_GROUP:
               memscpy(field_info,sizeof(pbm_g_efs_field_info_s_type),
			   	      &(g_rest_field_info[field_group]),sizeof(pbm_g_other_field_info_s_type));
               cache = pbm_pb_id_to_cache(pb_id);
               if(cache != NULL)
               {
                  if(cache->text_length[0])
                  {
                    field_info->info[0].max_length = cache->text_length[0] + 2;
                  }
                  /* send the dynamic text length supported rather than the static max stored in PBM arrays */
               }
               break;
             case EFS_GROUP:
               memscpy(field_info,sizeof(pbm_g_efs_field_info_s_type),
			   	      &(g_efs_field_info),sizeof(pbm_internal_field_info));
               /* using pbm_g_other_field_info_s_type as size because of lint error */
               break;
             default:
               return PBM_ERROR;
        }
        return PBM_SUCCESS;
    }
    else
    {
        return PBM_ERROR;
    }
}
/*===========================================================================
FUNCTION pbm_sim_device_type_to_phonebook_type;

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
pbm_return_type pbm_sim_device_type_to_phonebook_type(pbm_device_type device_type,
                                                                      pbm_phonebook_type *pb_id)
{
  pbm_slot_type slot_id;
  pbm_device_type session_device_type;
  pbm_return_type ret_value = PBM_SUCCESS;
  pbm_session_enum_type session_type = PBM_SESSION_GPB_1;
  pbm_session_enum_type card_session = PBM_SESSION_GPB_1;  

  do
  {

  memset(pb_id,0,sizeof(pbm_phonebook_type));

  if(device_type == PBM_EFS
      || device_type == PBM_SDL
      || device_type == PBM_MISS
      || device_type == PBM_RCVD
      || device_type == PBM_ECC_OTHER
      || device_type == PBM_DIALED
      || device_type == PBM_DEFAULT )
  {
    //all the above phonebooks will be stored in phone memory
    //in these cases only device_type is enough..
    pb_id->device_type = device_type;
    break;
  }

  slot_id = pbm_device_type_to_sim_id(device_type);
  session_device_type = pbm_sim_device_type_to_session_device_type(device_type);
  if(PBM_SUCCESS != pbm_get_active_prov_session_type(slot_id,&session_type))
  {
    if(session_device_type != PBM_ECC)
    {
      //this means that we havent recieved subscription_ready event
      ret_value = PBM_ERROR_NOT_READY;
    }
    else
    {
       //in NV only build , NV and Hardcoded ecc doesnt require any prov app
       pb_id->device_type = PBM_ECC_OTHER;
    }
    break;
  }
  PBM_VALIDATE_SESSION_RET(session_type,PBM_ERROR); /* KW error */
  rex_enter_crit_sect(&pbm_crit_sect);
  switch(session_device_type)
  {

    case PBM_FDN:
    case PBM_SDN:
      //the above phonebooks will reside in ADF if exists else TELECOM
      if(pbm_session_info[session_type].app_type == MMGSDI_APP_SIM
              || pbm_session_info[session_type].app_type == MMGSDI_APP_RUIM)
      {
        if(slot_id == PBM_SLOT_1)
        {
          session_type = PBM_SESSION_GPB_1;
        }
        else if(slot_id == PBM_SLOT_2)
        {
          session_type = PBM_SESSION_GPB_2;
      }
#ifdef FEATURE_TRIPLE_SIM
        else if(slot_id == PBM_SLOT_3)
        {
          session_type = PBM_SESSION_GPB_3;
        }
#endif
      }
      break;
    case PBM_MSISDN:
      //the above phonebooks will reside in ADF if exists else TELECOM
      if(pbm_session_info[session_type].app_type == MMGSDI_APP_SIM)
      {
        if(slot_id == PBM_SLOT_1)
        {
          session_type = PBM_SESSION_GPB_1;
        }
        else if(slot_id == PBM_SLOT_2)
        {
          session_type = PBM_SESSION_GPB_2;
      }
#ifdef FEATURE_TRIPLE_SIM
        else if(slot_id == PBM_SLOT_3)
        {
          session_type = PBM_SESSION_GPB_3;
        }
#endif
      }
      break;

    case PBM_MBN:
    case PBM_OCI:
    case PBM_ICI:
    case PBM_ECC:
    case PBM_MBDN:
      //do nothing since this phonebook always exists in the prov session
      break;


    case PBM_LND:
    //the above phonebook will exists only for GSM & CDMA and in telecom
    if(pbm_session_info[session_type].app_type == MMGSDI_APP_SIM
      || pbm_session_info[session_type].app_type == MMGSDI_APP_RUIM)
    {
        if(slot_id == PBM_SLOT_1)
        {
        session_type = PBM_SESSION_GPB_1;
        }
        else if(slot_id == PBM_SLOT_2)
        {
        session_type = PBM_SESSION_GPB_2;
    }
#ifdef FEATURE_TRIPLE_SIM
        else if(slot_id == PBM_SLOT_3)
        {
          session_type = PBM_SESSION_GPB_3;
        }
#endif
    }
    break;

    case PBM_ADN:
    if(pbm_session_info[session_type].app_type == MMGSDI_APP_SIM
      || pbm_session_info[session_type].app_type == MMGSDI_APP_RUIM)
    {
        if(slot_id == PBM_SLOT_1)
        {
        session_type = PBM_SESSION_GPB_1;
        }
        else if(slot_id == PBM_SLOT_2)
        {
        session_type = PBM_SESSION_GPB_2;
    }
#ifdef FEATURE_TRIPLE_SIM
        else if(slot_id == PBM_SLOT_3)
        {
          session_type = PBM_SESSION_GPB_3;
        }
#endif
    }
    //ofcourse csim case should never come here ..since CSIM always should use session based API
    else if(pbm_session_info[session_type].app_type == MMGSDI_APP_USIM
       || pbm_session_info[session_type].app_type == MMGSDI_APP_CSIM)
    {
        //check whether the phonebook (local/global /default) is known
        if(pbm_session_info[session_type].in_use_adn_pb == PBM_PB_NOT_KNOWN)
        {
        //this can happen due to PBM havent decided yet which phonebook to be read
        ret_value = PBM_ERROR_NOT_READY;
        }
        else 
        {
            if(slot_id == PBM_SLOT_1)
            {
              card_session = PBM_SESSION_GPB_1;
            }
            else if(slot_id == PBM_SLOT_2)
            {
              card_session = PBM_SESSION_GPB_2;
            }
#ifdef FEATURE_TRIPLE_SIM
            else if(slot_id == PBM_SLOT_3)
            {
              card_session = PBM_SESSION_GPB_3;        
        }
#endif

            if(pbm_session_info[session_type].in_use_adn_pb != PBM_PB_LOCAL_PBR_READ
                || pbm_session_info[card_session].in_use_adn_pb == PBM_PB_GLOBAL_PBR_READ)
            {
                //if local PBR not present or global PBR present then always telecom phonebook
                session_type = card_session;
            }
            else if(pbm_session_info[card_session].in_use_adn_pb == PBM_PB_NOT_KNOWN)
            {
            //this can happen that PBM know that local PBR is present but not sure abt global pbr presence
                ret_value = PBM_ERROR_NOT_READY;
            }
        }
    }
    break;
    default:
    UIM_MSG_ERR_1("Invalid device_type: %d ", device_type);
    ret_value = PBM_ERROR;
    break;

  }
  rex_leave_crit_sect(&pbm_crit_sect); /* for in_use_adn_pb and app_type */

  if(ret_value == PBM_SUCCESS)
  {
    *pb_id = pbm_uim_device_type_to_phonebook_type((pbm_uim_app_enum_type)session_type,session_device_type);
  }

  }while(0);
  return ret_value;
}

/* =======================    SUPS Parsing related Functions  ========================== */

/*===========================================================================
FUNCTION pbmutil_validate_ss

DESCRIPTION
 This utility function is internal to PBM to find if
 the tokenize string is proper SS string or not.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
static boolean pbmutil_validate_ss(int num_tokens, void *tokens[], pbmutil_sups_start_s_type *ss_start)
{
   char in_val[PBMUTIL_SUPS_MAX_TOKENS][CM_MAX_NUMBER_CHARS + 1];
   int i,j;
   char *ptr;
   char *ss_str;
   int pos = 0;
   pbmutil_sups_operation_s_type         *current_ss_operation = pbmutil_sups_operation_input;
   uint8 str_len;
   boolean ss_match = TRUE;
   pbm_sups_cmd_e_type ss_operation = PBM_SUPS_CMD_NONE;
   char   bsg[PBMUTIL_SUPS_MAX_BSG_LEN + 1];
   char sia[PBMUTIL_SUPS_MAX_SIA_LEN + 1];
   char sib[PBMUTIL_SUPS_MAX_SIB_LEN + 1];
   char sic[PBMUTIL_SUPS_MAX_SIC_LEN + 1];
   char ss_code[PBMUTIL_SUPS_MAX_SC_LEN + 1];
   pbmutil_sups_start_s_type*    sups_start = (pbmutil_sups_start_s_type*) ss_start;
   if(num_tokens <= 0)
   {
      return FALSE;
   }
   for(i=0;i<PBMUTIL_SUPS_MAX_TOKENS;i++)
   {
      (void)memset(in_val[i],'\0',(CM_MAX_NUMBER_CHARS + 1));
   }
   ptr = (char *)tokens[0];
   str_len = (uint8)strlen(ptr);
   ss_str = (char *)tokens[0];
   for (i =0;!ISDIGIT(ptr[i]);i++)
   {
     in_val[0][i] = ptr[i];
   }

   for(j=1;j<num_tokens;j++)
   {
     ptr = (char *)tokens[j];
     for (i = 0;(ptr[i] != '*') && (ptr[i] != '#') &&  i<CM_MAX_NUMBER_CHARS;i++)
     {
        in_val[j][i] = ptr[i];
     }
   }
   (void)memset( sia, '\0', sizeof(sia) );
   (void)memset( sib, '\0', sizeof(sib) );
   (void)memset( sic, '\0', sizeof(sic) );
   (void)memset(ss_code, '\0', sizeof(ss_code) );
   (void)memset (bsg, '\0', sizeof(bsg));
   memset(sups_start, 0, sizeof(pbmutil_sups_start_s_type));
   if( ( (memcmp(tokens[0], "*31#", 4) == 0) ||
      (memcmp(tokens[0], "#31#", 4) == 0) ))
   {
      return FALSE;
   }
   /* STEP 1: Find the SS operation */
   while((current_ss_operation->sups_operation != NULL) &&
         (strlen(current_ss_operation->sups_operation) <= CM_MAX_NUMBER_CHARS))
   {
      if(!strncmp(in_val[pos], current_ss_operation->sups_operation, CM_MAX_NUMBER_CHARS))
      {
       ss_operation = current_ss_operation->sups_type;
       pos++;
       break;
      }
      current_ss_operation++;
   }
   if( (strlen(PBMUTIL_SUPS_LOCAL_BSG_REG_PASSWD) <= CM_MAX_NUMBER_CHARS)
       && !strncmp( in_val[pos], PBMUTIL_SUPS_LOCAL_BSG_REG_PASSWD, CM_MAX_NUMBER_CHARS ) )
   {
      ss_operation = PBM_SUPS_CMD_REG_PASSWORD;
      pos++;
   }
   sups_start->ss_operation = (pbm_sups_cmd_e_type)ss_operation;
   if ( sups_start->ss_operation != PBM_SUPS_CMD_NONE )
   {
      /* By now the operation must have been identified */
      /* STEP 2: Find the SI fields */
      (void)memscpy( ss_code, sizeof(ss_code), in_val[pos++],PBMUTIL_SUPS_MAX_SC_LEN);
      (void)memscpy (sia, sizeof(sia), in_val[pos++], PBMUTIL_SUPS_MAX_SIA_LEN);
      (void)memscpy (sib, sizeof(sib), in_val[pos++], PBMUTIL_SUPS_MAX_SIB_LEN);
      (void)memscpy (sic, sizeof(sic), in_val[pos++], PBMUTIL_SUPS_MAX_SIC_LEN);
      /* by now SIA, SIB, SIC fields must have been separated out */
      /* STEP 3: Convert SC to network SC and update the
      sups_user_data field
      */
      {
         byte n = 0;
         while( n < PBMUTIL_SUPS_SC_MAX_ENTRY )
         {
            if( !strncmp( pbmutil_sups_sc_conversion_table[n].local_sc,
               ss_code, PBMUTIL_SUPS_MAX_SC_LEN ) )
            {
               sups_start->ss_code = (pbmutil_sups_op_code_e_type)pbmutil_sups_sc_conversion_table[n].net_sc;
               //sups_start->currentState = AEESUPPSVC_CODE_STATE;
               break;
            }
            n++;
         }
         if (n >= PBMUTIL_SUPS_SC_MAX_ENTRY)
         {
            ss_match = FALSE;
         }
      }
      /*Special Case - Only Interrogation and Deactivation are allowed on CCBS*/
      if(sups_start->ss_code == PBMUTIL_SUPS_OP_CODE_CCBS &&
         sups_start->ss_operation != PBM_SUPS_CMD_INTERROGATE &&
         sups_start->ss_operation != PBM_SUPS_CMD_DEACTIVATE)
      {
         ss_match = FALSE;
      }
      /* by now the translation of SC must be complete */
      /* STEP 4: Check for BSG . This may be in SIA, SIB or SIC */
      if (ss_match)
      {
         if( (sups_start->ss_operation != PBM_SUPS_CMD_REG_PASSWORD)  &&
         ( ( ( (uint16)(sups_start->ss_code) & 0xF0 ) == (uint16)PBMUTIL_SUPS_OP_CODE_ALL_FORWARDING_SS  ) ||
         ( ( (uint16)(sups_start->ss_code) & 0xF0 ) == (uint16)PBMUTIL_SUPS_OP_CODE_ALL_CALL_RSTRCT_SS ) ) )
         {
            /* BSG is in SIB */
            (void)strlcpy( bsg, sib, sizeof(bsg) );
         }
         else if( sups_start->ss_code == PBMUTIL_SUPS_OP_CODE_CW )
         {
            /* BSG is in  SIA */
            (void)strlcpy( bsg, sia ,sizeof(bsg));
         }
         else
         {
            bsg[0] = '\0';
         }
         /* by now BSG must be identified */
         /* STEP 5: Convert BSG to net BSG */
         sups_start->basic_service = PBMUTIL_SUPS_BSG_ALL_SERVICES;
         if ( sups_start->ss_operation != PBM_SUPS_CMD_REG_PASSWORD)
         {
            /* special case */
            byte n = 0;
            //sups_start->currentState = AEESUPPSVC_BSG_STATE;
            /*  BSG - Basic Service Group :
            BS  = Bearer Service
            TS  = Tele Service
            */
            while( n < PBMUTIL_SUPS_BSG_MAX_ENTRY )
            {
               if( !strncmp( pbmutil_sups_bsg_conversion_table[n].local_bsg,
               bsg, PBMUTIL_SUPS_MAX_BSG_LEN ) )
               {
                  sups_start->basic_service  = (pbmutil_sups_basic_service_e_type) pbmutil_sups_bsg_conversion_table[n].net_bsg_code;
                  break;
               }
               n++;
            }
            if(n == PBMUTIL_SUPS_BSG_MAX_ENTRY  )
            {
               ss_match = FALSE;
            }
         }
      }
      /* STEP 6:  Convert DN, if any */
      if ( ss_match)
      {
         if ( ( (uint16)(sups_start->ss_code) & 0xF0 ) == (uint16)PBMUTIL_SUPS_OP_CODE_ALL_FORWARDING_SS )
         {
            if (sups_start->ss_operation == PBM_SUPS_CMD_REGISTER && sia[0] == '\0')
            {
               return FALSE;
            }
            if ( sia[0] != '\0' && (sups_start->ss_operation == PBM_SUPS_CMD_REGISTER ||
               sups_start->ss_operation == PBM_SUPS_CMD_ACTIVATE))
            {
               int sia_len = strlen(sia);
               sups_start->req.reg.len = MIN(sia_len,CM_MAX_NUMBER_CHARS);
            }
            /* else the length of the DN will be 0 */
         }
      }
      /* STEP 7: Check is activation is also registration? */
      if( sups_start->ss_operation == PBM_SUPS_CMD_ACTIVATE )
      {
         if( (sups_start->req.reg.len > 0)  || (sups_start->req.reg.nr_timer != 0))
         {
            sups_start->ss_operation = PBM_SUPS_CMD_REGISTER;
         }
      }
      /* STEP 8: Set the NR timer */
      if( (sups_start->ss_operation == PBM_SUPS_CMD_REGISTER ) &&
         ( ( sups_start->ss_code == PBMUTIL_SUPS_OP_CODE_ALL_FORWARDING_SS ) ||
         ( sups_start->ss_code == PBMUTIL_SUPS_OP_CODE_ALL_CONDFWD_SS ) ||
         ( sups_start->ss_code == PBMUTIL_SUPS_OP_CODE_CFNRY ) ) )
      {

         if( sic[0] != '\0' )
         {
            sups_start->req.reg.nr_timer = (uint8)atoi(sic);
         }
         else
         {
            sups_start->req.reg.nr_timer = 0;
         }
      }
      /* STEP 9: copy the password */
      if ( ss_match)
      {
         if( ( ((uint16)(sups_start->ss_code) & 0xF0 ) == (uint16)PBMUTIL_SUPS_OP_CODE_ALL_CALL_RSTRCT_SS ) &&
         (sups_start->ss_operation != PBM_SUPS_CMD_REG_PASSWORD) )
         {

         }
         else if(sups_start->ss_code  == PBMUTIL_SUPS_OP_CODE_CCBS)
         {
            #ifdef FEATURE_CCBS
            if( sia[0] == '\0' )
            {
               sups_start->req.ccbsIndex = CM_CCBS_INDX_INVALID;
            }
            else
            {
               sups_start->req.ccbsIndex = (uint8)atoi(sia);
            }
            #else
            ss_match = FALSE;
            #endif
         }
      }
      /* copy old_pwd, new_pwd, new_pwd_again */
      if( sups_start->ss_operation == PBM_SUPS_CMD_REG_PASSWORD)
      {
         if((sia[0] == '\0' || sib[0] == '\0' || sic[0] == '\0' ))
         {
            ss_match = FALSE;
         }
      }
   }
   if(ss_str[str_len -1] == '#' && ss_match)
   {
      ss_operation = sups_start->ss_operation;
      UIM_MSG_HIGH_0(" pbmutil_chk_ss Valid SS string");
   }
   else
   {
      UIM_MSG_HIGH_0(" pbmutil_chk_ss Invalid SS string");
      return FALSE;
   }
   return TRUE;
}


/*===========================================================================
FUNCTION pbmutil_reg_exp_chars_match

DESCRIPTION
 to match the characters of regular exp.
 d denotes digit.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
static boolean pbmutil_reg_exp_chars_match(boolean special_char, char match_char, char src)
{
   if (special_char)     /* Do we treat this special */
   {
      switch (match_char)
      {
        case 'd': /* Currently, we only handle digits as special characters */
          return (src >= '0' && src <= '9');
        default:
          UIM_MSG_HIGH_1("pbmutil_reg_exp_chars_match Unknown special character %d", match_char);
          return FALSE;
      }
   }
   else
   {
      /* Normal characters are just compared. */
      return (src == match_char);
   }
}

/*===========================================================================
FUNCTION pbmutil_regex_to_match

DESCRIPTION
 to match the characters of regular exp..

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
static boolean pbmutil_regex_to_match(char **src_ptr, char **regex_ptr)
{
   boolean special_char = FALSE;  /* Are we processing a special character */
   boolean multi_char = FALSE;    /* Does this token have multiple characters */
   boolean choice_exists = FALSE; /* Is there a choice of characters */
   int min_num_chars_to_match;   /* Number of characters necessary (0 or 1) */
   char match_char;              /* The character we are trying to match. */
   char *start_of_regex = *regex_ptr;  /* Save the start of the current regex */
   char *end_reg_ptr;            /* Save the end of the current regex. */
   char *tmp_reg_ptr;            /* Work variable. */
   int num_chars_matched = 0;    /* How long is the matching string. */
   end_reg_ptr = start_of_regex+1; /* End initially is the start + 1 */
   switch (*start_of_regex)
   {
   case '[':                   /* We are going to have a choice. */
      choice_exists = TRUE;
      start_of_regex++;         /* The characters to match start next */

      /* Find the end of the regular expression */
      while ((*end_reg_ptr != ']') && (*end_reg_ptr != '\0'))
      {
         end_reg_ptr++;
      }

      if (*end_reg_ptr != ']')
      {
         UIM_MSG_HIGH_0("pbmutil_regex_to_match Error in regular expression; no closing ]");
         return FALSE;
      }

      end_reg_ptr++;    /* End points to either the start of the next or the
                        repeat character */
      break;
   case '\\':          /* Escaped character? */
      end_reg_ptr++;    /* The regular expression takes an extra character. */
      break;
   default:
      break;
   }

   /* Now we check the character afterwards to see if it specifies a count. */
   switch (*(end_reg_ptr))
   {
   case '*':   /* Specifies 0 or more */
      multi_char = TRUE;
      min_num_chars_to_match = 0;
      end_reg_ptr++;
      break;
   case '+':   /* Specifies 1 or more */
      multi_char = TRUE;
      min_num_chars_to_match = 1;
      end_reg_ptr++;
      break;
   default:    /* No repeat counts */
      multi_char = FALSE;
      min_num_chars_to_match = 1;
      break;
   }

   tmp_reg_ptr = start_of_regex;         /* Initialize work variable */
   do
   {
      match_char = *tmp_reg_ptr;          /* We are searching for this char */
      // Check for escaped characters.
      if (match_char == '\\')             /* Unless it is escaped. */
      {
         match_char = *++tmp_reg_ptr;      /* Get the char after the backslash */
         switch (match_char)
         {
         case 'd':                       /* D is the only special char we know */
            special_char = TRUE;
            break;
         default:
            special_char = FALSE;
            break;
         }
      }

      /* Now check to see if the match_char from the regular expression matches
      * the current character in the input string. */
      if (pbmutil_reg_exp_chars_match(special_char, match_char,
         *((*src_ptr) + num_chars_matched)))
      {
         /* We have a match, mark that we did. */
         num_chars_matched++;
         tmp_reg_ptr = start_of_regex;
      }
      else
      {
         /* It doesn't match.  Either we are done matching to this regular
         * expression or we have more choices. */
         if (choice_exists)
         {
            tmp_reg_ptr++;
            if (*tmp_reg_ptr == ']') /* No more choices, we are done. */
            {
               break;
            }
         }
         else
         {
            break;
         }
      }
   } while (multi_char || choice_exists);

  /* Now to see if we are actually matching what was specified. */
  if (num_chars_matched >= min_num_chars_to_match)
   {
      /* Yep, move things beyond this regular expression */
      *src_ptr += num_chars_matched;
      *regex_ptr = end_reg_ptr;
      return TRUE;
   }
   else
   {
      /* Nope.  */
      return FALSE;
   }
}


/*===========================================================================
FUNCTION pbmutil_tokenize_sups_string

DESCRIPTION
 to tokenize the SS string passed.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
static boolean pbmutil_tokenize_sups_string(char *in_str, PBMUTIL_TOKEN_INPUT *inputs,
                                                                void *tokens[], pbmutil_sups_start_s_type *sups_start,
                                                                boolean find_pos, int *plus_pos)
{
   int ntokens;          /* Number of tokens we have saved. */
   char *current_in;     /* Current place in the input string. */
   char *current_regex;  /* Current place of the regular expression */
   PBMUTIL_TOKEN_INPUT *curr_ptr = inputs; /* Current regular express */
   boolean found_match = FALSE; /* Used to loop across multiple reg_exps */
   boolean optional_match = FALSE; /* Marks this part of the regex as optional */
   boolean do_save = FALSE; /* Should we save this if it matches. */
   boolean pbm_ret = FALSE;
   char *tmp_ptr;        /* Work variable when we may need to save it. */

   /* While we need to search and haven't found it. */
   while ((found_match == FALSE) && (curr_ptr->reg_exp != NULL))
   {
      /* New expression, init our variables. */
      current_in = in_str;                /* Start at beginning of source */
      current_regex = curr_ptr->reg_exp;  /* Start at beginning of regexp */
      found_match = TRUE;                 /* Assume we will match. */
      ntokens = 0;                        /* We haven't saved anything yet. */
      optional_match = FALSE;
      do_save = FALSE;

      /* While we are looking good. */
      while ((found_match) && (*current_regex != '\0'))
      {
         switch (*current_regex) /* Check if we handle the regular expression */
         {
         case '{':               /* this portion of the string is optional */
            optional_match = TRUE;
            current_regex++;
            break;
         case '}':               /* end the string is optional */
            optional_match = FALSE;
            current_regex++;
            break;
         case '(':               /* Save the current token. */
            do_save = TRUE;
            current_regex++;
            break;
         case ')':               /* Really a no-op since its up to the
                                 parsing function to mark the end. */
            current_regex++;
            break;
         default:
            /* We don't handle the character, so see if the token matches */
            tmp_ptr = current_in;
            found_match = pbmutil_regex_to_match(&current_in, &current_regex);
            if (found_match)
            {
               if (do_save)
               {
                  tokens[ntokens++] = (void *) tmp_ptr;
               }
            }
            else if (optional_match)
            {
               // If we didn't find a match, it may mean we are in an
               // optional part of the expression.  Skip to the end of
               // the optional part now and resume comparing there.
               while ((*current_regex++ != '}') && (*current_regex != '\0'))
               {
                  if (*current_regex == '\\') current_regex++;
                  current_regex++;
               }
               found_match = TRUE;
            }
            do_save = FALSE;
            break;
         }
      }//while(found_match && current_regex)
      /* Done checking regular expression... if everything matches... */
      if (found_match && (*current_in == '\0'))
      {
         if(find_pos) /* call is to find the position */
         {
            pbm_ret = pbm_util_int_find_pos_for_plus_in_ss_string(ntokens, tokens,
               sups_start, plus_pos);

         }
         else
         {
            /* Everything matched, call the function associated with it. */
            pbm_ret = ((*(curr_ptr->check_fn))(ntokens, tokens, sups_start));
         }
         return pbm_ret;
      }
      /* We don't have a match, check next regular expression. */
      found_match = FALSE;
      curr_ptr++;
   }
   return FALSE;
}


/*===========================================================================
FUNCTION pbm_util_is_valid_sups_string_find_pos_for_plus

DESCRIPTION
 API to check if the string is a valid SS string or to find the
 position for plus sign in the string.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_util_is_valid_sups_string( char *sups_str, uint8 sups_str_len,
    int *pos, boolean find_pos)
{
   char ascii_buffer[CM_MAX_NUMBER_CHARS];
   boolean pbm_ret = FALSE;
   pbmutil_sups_start_s_type sups_start;
   void *pbmutil_tokens[PBMUTIL_SUPS_MAX_TOKENS]; /* making this array of 6 pointers as local */
   memset(&sups_start, 0, sizeof(pbmutil_sups_start_s_type));
   *pos = 0xff;
   if(sups_str == NULL)
   {
      UIM_MSG_HIGH_0("pbm_util_is_valid_sups_string_find_pos_for_plus Input Sups String is null");
      return pbm_ret;
   }
   if((sups_str_len > CM_MAX_NUMBER_CHARS) || (sups_str_len < MIN_SUPS_DIGITS))
   {
      UIM_MSG_HIGH_0("pbm_util_is_valid_sups_string_find_pos_for_plus Length passed is invalid!");
      return pbm_ret;
   }
   //check if the sups string is terminated by # and is valid.
   if((sups_str[sups_str_len-1] != '#') || (!ISDIGIT(sups_str[sups_str_len-2])))
   {
      UIM_MSG_HIGH_0("pbm_util_is_valid_sups_string_find_pos_for_plus Invalid Sups String!");
      return pbm_ret;
   }
   memset(ascii_buffer,'\0',CM_MAX_NUMBER_CHARS);
   memscpy(ascii_buffer, sizeof(ascii_buffer), sups_str, sups_str_len);

   pbm_ret =  pbmutil_tokenize_sups_string(ascii_buffer, pbmutil_regex_input,
      pbmutil_tokens,&sups_start, find_pos, pos);
   if(pbm_ret)
   {
      UIM_MSG_HIGH_0("pbm_util_is_valid_sups_string_find_pos_for_plus The result of tokenization is SUCCESS");
      return TRUE;
   }
   else
   {
      UIM_MSG_HIGH_0("pbm_util_is_valid_sups_string_find_pos_for_plus Tokenization FAILED with value");
      return FALSE;
   }
}


/*===========================================================================
FUNCTION pbm_util_int_find_pos_for_plus_in_ss_string

DESCRIPTION
 PBM internal API to find the position of PLUS sign in tokenized
 SS string.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
static boolean pbm_util_int_find_pos_for_plus_in_ss_string(int num_tokens, void *tokens[],
   pbmutil_sups_start_s_type *ss_start, int *plus_pos)
{
   char in_val[PBMUTIL_SUPS_MAX_TOKENS][CM_MAX_NUMBER_CHARS + 1];
   int i,j;
   char                            *ptr;
   int                             pos = 0;
   pbmutil_sups_operation_s_type   *current_ss_operation = pbmutil_sups_operation_input;
   pbm_sups_cmd_e_type             ss_operation = PBM_SUPS_CMD_NONE;
   char                            bsg[PBMUTIL_SUPS_MAX_BSG_LEN + 1];
   char                            sia[PBMUTIL_SUPS_MAX_SIA_LEN + 1];
   char                            sib[PBMUTIL_SUPS_MAX_SIB_LEN + 1];
   char                            sic[PBMUTIL_SUPS_MAX_SIC_LEN + 1];
   char                            ss_code[PBMUTIL_SUPS_MAX_SC_LEN + 1];
   pbmutil_sups_start_s_type*    sups_start = (pbmutil_sups_start_s_type*) ss_start;
   *plus_pos = 0xff;

   if(num_tokens <= 0)
   {
      return FALSE;
   }
   for (i = 0; i < PBMUTIL_SUPS_MAX_TOKENS; i++)
   {
      (void)memset(in_val[i],'\0',(CM_MAX_NUMBER_CHARS + 1));
   }
   ptr = (char *)tokens[0];
   for (i =0;!ISDIGIT(ptr[i]);i++)
   {
      in_val[0][i] = ptr[i];
   }
   for(j=1;j<num_tokens;j++)
   {
      ptr = (char *)tokens[j];
      for (i = 0;(ptr[i] != '*') && (ptr[i] != '#') &&  i<CM_MAX_NUMBER_CHARS;i++)
      {
         in_val[j][i] = ptr[i];
      }
   }
   (void)memset( sia, '\0', sizeof(sia) );
   (void)memset( sib, '\0', sizeof(sib) );
   (void)memset( sic, '\0', sizeof(sic) );
   (void)memset(ss_code, '\0', sizeof(ss_code) );
   (void)memset(bsg, '\0', sizeof(bsg));
   memset(sups_start, 0, sizeof(pbmutil_sups_start_s_type));
   if( ( (memcmp(tokens[0], "*31#", 4) == 0) ||
      (memcmp(tokens[0], "#31#", 4) == 0) ))
   {
      return FALSE;
   }
    /* STEP 1: Find the SS operation */
   while((current_ss_operation->sups_operation != NULL) &&
         (strlen(current_ss_operation->sups_operation) <= CM_MAX_NUMBER_CHARS))
   {
      if(!strncmp(in_val[pos], current_ss_operation->sups_operation, CM_MAX_NUMBER_CHARS))
      {
         ss_operation = current_ss_operation->sups_type;
         pos++;
         break;
      }
      current_ss_operation++;
   }
   if( (strlen(PBMUTIL_SUPS_LOCAL_BSG_REG_PASSWD) <= CM_MAX_NUMBER_CHARS)
       && !strncmp( in_val[pos], PBMUTIL_SUPS_LOCAL_BSG_REG_PASSWD, CM_MAX_NUMBER_CHARS ) )
   {
      ss_operation = PBM_SUPS_CMD_REG_PASSWORD;
      pos++;
   }
   sups_start->ss_operation = (pbm_sups_cmd_e_type)ss_operation;
   if ( sups_start->ss_operation != PBM_SUPS_CMD_NONE )
   {
      /* By now the operation must have been identified */
      /* STEP 2: Find the SI fields */
      (void)memscpy (ss_code, sizeof(ss_code), in_val[pos++],PBMUTIL_SUPS_MAX_SC_LEN);
      (void)memscpy (sia, sizeof(sia), in_val[pos++], PBMUTIL_SUPS_MAX_SIA_LEN);
      (void)memscpy (sib, sizeof(sib), in_val[pos++], PBMUTIL_SUPS_MAX_SIB_LEN);
      (void)memscpy (sic, sizeof(sic), in_val[pos++], PBMUTIL_SUPS_MAX_SIC_LEN);
      *plus_pos = 1; /* every valid SS string has * before SIA field that wont be tokenize */
      *plus_pos += strlen(in_val[0]); /* for first sequence of **, *#, ##, *, # */
      *plus_pos += strlen (ss_code);
      return TRUE;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION pbm_util_get_cache_number_status_of_ton

DESCRIPTION
 this function tells if the passed field is TON or number type
 and returns TRUE if the TON is international.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_util_get_cache_number_status_of_ton(pbm_field_s_type *sfield_ptr)
{
   boolean result = FALSE;
   void *temp=pbm_get_field_data_ptr(sfield_ptr,0);
   PBM_CHECK_PTR_RET(temp , FALSE);

   if (sfield_ptr->data_len != 0)
   {
      if ( sfield_ptr->field_id == PBM_FIELD_GSM_TONNPI )
      {
        /* check if the cache number has TON as international */
         if((*(byte *)temp & CM_TON_MASK) == CM_TON_INTERNATIONAL)
         {
            result = TRUE;
         }
      }
      if (sfield_ptr->field_id == PBM_FIELD_NUMBER_TYPE)
      {
         if (*(uint8 *)temp < ARR_SIZE(pbm_number_type_to_gsmton))
         {
            if(CM_TON_INTERNATIONAL & pbm_number_type_to_gsmton[*(uint8 *)temp])
            {
                result = TRUE;
            }
         }
      }
      if (sfield_ptr->field_id == PBM_FIELD_PHONE_GENERIC
          && sfield_ptr->field_type == PBM_FT_BCD)
      {
        /* check if the cache number has TON as international */
        if((((byte *)temp)[1] & CM_TON_MASK) == CM_TON_INTERNATIONAL)
        {
          result = TRUE;
        }
      }
   } /* if data_len != 0 */
   return result;
}

/*===========================================================================
FUNCTION pbm_util_get_status_of_queried_number

DESCRIPTION
 this function returns true if
 1. valid SS string, and if  "+" sign is present in it at right position,
 2. If it is a normal number with or without + sign. If normal number
 contains + sign at first position it removes the + sign from local_number
 and update data_size accordingly.

 returns FALSE
 1. If it is a valid SS string with + sign  but + is not present at
 valid position

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_util_get_ss_ton_status_of_queried_number(boolean *valid_ss_string, boolean *queried_number_has_plus, int *data_size, byte *local_number)
{
   int pos_plus = 0xff;
   boolean result = TRUE;
   int j = 0;

   if (pbm_util_is_valid_sups_string((char *)local_number,
   strlen((char *)local_number), &pos_plus, FALSE)   /* if the number is valid SS string */
   )
   {
      /* SS string contains the + sign need to find the exact position for + sign */
      if (pbm_util_is_valid_sups_string((char *)local_number,
         strlen((char *)local_number), &pos_plus, TRUE))
      {
         *valid_ss_string = TRUE;
         if(local_number[pos_plus] == '+')
         {
            /* means that plus is present at the right position and number
            should only be matched in this case */
            *queried_number_has_plus = TRUE;
         }
         else
         {
            /* no need to match this category of numbers */
            result = FALSE;
         }
      }
   }
   else
   {
      if(local_number[0] == '+') /* for normal numbers + should be present in first byte only */
      {
         /* remove the + sign from the number string */
         //memmove((char*)local_number[0], (char*)local_number[1], ((*data_size) -2));
         for(j=0; j<= (*data_size -1); j++)
         {
             local_number[j] = local_number[j+1];
         }
         local_number[j] = '\0';
         *data_size-=1; /* as the + sign is removed*/
         *queried_number_has_plus = TRUE;
      }
   }
   return result;
}

/*===========================================================================
FUNCTION pbm_util_query_to_match_numbers

DESCRIPTION
 this function tells if the passed field is TON or number type
 and returns TRUE if the TON is international.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_util_query_to_match_numbers(pbm_field_id_e_type field_id, boolean queried_number_has_plus
                                        ,boolean cache_number_contains_ton,boolean cache_number_contains_plus, boolean valid_ss_string)
{
   boolean result = FALSE;
   if (field_id == PBM_FIELD_PHONE_GENERIC)
   {
      /* consider the TON as well in conditions */
      if ( ((queried_number_has_plus) && (!cache_number_contains_ton) && (valid_ss_string))
      || ((queried_number_has_plus) && (cache_number_contains_ton) && (valid_ss_string))
      || ((queried_number_has_plus) && (!valid_ss_string) && (cache_number_contains_ton ||cache_number_contains_plus))
      ||  ((!queried_number_has_plus) && (!cache_number_contains_ton && !cache_number_contains_plus))
      )
      {
         result = TRUE;
      }
   }
   else
   {
      /* other phone field no need to consider the ton */
      if ( ((queried_number_has_plus) && (!cache_number_contains_plus) && (valid_ss_string))
      || ((queried_number_has_plus) && (!valid_ss_string) && (cache_number_contains_plus))
      ||  ((!queried_number_has_plus) && (!cache_number_contains_plus) )
      )
      {
         result = TRUE;
      }
   }
   return result;
}

/* ======================== SUPS parsing related def end ======================= */


/*===========================================================================
FUNCTION pbm_util_return_fileset_for_session

DESCRIPTION
 this function returns the fileset to be used in pbm_file_to_uim_file and pbm_static_path_lookup_table

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
uint32 pbm_util_return_fileset_for_session(pbm_session_enum_type session_type)
{
  uint32 fileset;
  fileset = USE_USIM_LOCAL_SUPPORT(session_type)?1:0;
  if(!fileset)
    fileset = USE_CSIM_LOCAL_SUPPORT(session_type)?2:0;
  if(!fileset)
    fileset = USE_GSM_SUPPORT(session_type)?3:0;
  if(!fileset)
    fileset = USE_CDMA_SUPPORT(session_type)?4:0;
  return fileset;
}


/*===========================================================================
FUNCTION pbm_validate_pb_set

DESCRIPTION
 this function validates the pb_set value 

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
boolean pbm_validate_pb_set(pbm_pb_set_type pb_set)
{
    if ((pb_set < PBM_USIM_PB_ONE) || (pb_set >= PBM_MAX_NUM_PB_SETS)) 
    {																	   
        UIM_MSG_ERR_2("PB_SET %d out of bounds , num sets %d", pb_set,		   
                  		           PBM_MAX_NUM_PB_SETS);
        return FALSE;													   
    }

    return TRUE;
}

/*===========================================================================
FUNCTION pbm_mapping_as_id_to_pbm_session

DESCRIPTION
 Conversion of AS_ID to PBM session 
 This maps AS_ID only to GW sessions for now 
 Usually  AS_ID_1 maps to GW_PRI and 1x_PRI sessions 
 but from CM we always get OTA for GW sessions and hence the mapping 


DEPENDENCIES

SIDE EFFECTS

===========================================================================*/

pbm_session_enum_type   pbm_mapping_as_id_to_pbm_session( sys_modem_as_id_e_type as_id)
{

  switch(as_id) 
  {
  	case SYS_MODEM_AS_ID_1 :
		return PBM_SESSION_LPB_GW ;

#ifdef FEATURE_DUAL_SIM
	case SYS_MODEM_AS_ID_2 : 
		return PBM_SESSION_LPB_GW_SECONDARY; 

#ifdef FEATURE_TRIPLE_SIM
        case SYS_MODEM_AS_ID_3 : 
		return PBM_SESSION_LPB_GW_TERTIARY ; 
#endif
#endif
        default : 
                return PBM_SESSION_DEFAULT; 
  }
}


/* ============================================================================
FUNCTION PBM_UTIL_GET_SLOT_INDEX

DESCRIPTION
  This function based on the PBM slot id passed in, populates the slot index

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  PBM_SUCCESS if a valid slot index is available else return  PBM_ERROR

SIDE EFFECTS
  None
============================================================================*/
pbm_return_type pbm_util_get_slot_index(
  pbm_slot_type             pbm_slot,
  uint8                    *slot_index
)
{
  pbm_return_type   pbm_status = PBM_SUCCESS;

  switch(pbm_slot)
  {
    case PBM_SLOT_1:
      *slot_index = PBM_SLOT_1_INDEX;
      break;
    case PBM_SLOT_2:
      *slot_index = PBM_SLOT_2_INDEX;
      break;
    case PBM_SLOT_3:
      *slot_index = PBM_SLOT_3_INDEX;
      break;
    default:
      pbm_status = PBM_ERROR;
      UIM_MSG_ERR_1("Could not retrieve slot index for slot id 0x%x",
                    pbm_slot);
      break;
  }
  return pbm_status;
} /* pbm_util_get_slot_index */


/*===========================================================================
  FUNCTION PBM_UTIL_GET_COMMON_EFS_ITEM_CONTEXT

  DESCRIPTION
    Returns the index of the common EFS item context based on the index.

  PARAMETERS
    index

  RETURN VALUE
    uim_common_efs_context_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uim_common_efs_context_type pbm_util_get_common_efs_item_context(
  uint8 index
)
{
  switch (index)
  {
    case PBM_SLOT_1_INDEX:
      return UIM_COMMON_EFS_CONTEXT_0;
    case PBM_SLOT_2_INDEX:
      return UIM_COMMON_EFS_CONTEXT_1;
    case PBM_SLOT_3_INDEX:
      return UIM_COMMON_EFS_CONTEXT_2;
    default:
      return UIM_COMMON_EFS_CONTEXT_MAX;
  }
}/* pbm_util_get_common_efs_item_context */


/* ============================================================================
FUNCTION PBM_UTIL_GET_SLOT_ID

DESCRIPTION
  This function bases on the slot index passed in, populates the PBM slot id

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  PBM_SUCCESS if a valid slot id is available else return PBM_ERROR

SIDE EFFECTS
  None
============================================================================*/
pbm_return_type pbm_util_get_slot_id(
  uint8                      slot_index,
  pbm_slot_type             *slot_id
)
{
  PBM_CHECK_PTR_RET(slot_id, PBM_ERROR);

  switch(slot_index)
  {
    case PBM_SLOT_1_INDEX:
      *slot_id = PBM_SLOT_1;
      break;
    case PBM_SLOT_2_INDEX:
      *slot_id = PBM_SLOT_2;
      break;
    case PBM_SLOT_3_INDEX:
      *slot_id = PBM_SLOT_3;
      break;
    default:
      UIM_MSG_ERR_1("Could not retrieve slot id for slot index 0x%x", slot_index);
      return PBM_ERROR;
  }

  return PBM_SUCCESS;
} /* pbm_util_get_slot_id */


/* ============================================================================
FUNCTION PBM_UTIL_GET_EMERGENCY_MODE

DESCRIPTION
  This function derives emergency mode based on session type.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
============================================================================*/
void pbm_util_get_emergency_mode(
  pbm_session_enum_type        session_type,
  pbm_ecc_mode_type           *emergency_mode
)
{
  PBM_CHECK_PTR_RET(emergency_mode, VOID);

  switch(session_type)
  {
    case PBM_SESSION_LPB_GW:
    case PBM_SESSION_LPB_GW_SECONDARY:
    case PBM_SESSION_LPB_GW_TERTIARY:
      *emergency_mode = EMERGENCY_GW;
      break;

    case PBM_SESSION_LPB_1X:
    case PBM_SESSION_LPB_1X_SECONDARY:
    case PBM_SESSION_LPB_1X_TERTIARY:
      *emergency_mode = EMERGENCY_1X;
      break;

    case PBM_SESSION_GPB_1:
    case PBM_SESSION_GPB_2:
    case PBM_SESSION_GPB_3:
      *emergency_mode = EMERGENCY_GW_1X;
      break;

    default:
      *emergency_mode = NOT_EMERGENCY;
  }
} /* pbm_util_get_emergency_mode */
