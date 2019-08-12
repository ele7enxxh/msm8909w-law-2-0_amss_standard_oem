#ifndef PBMUTILS_H
#define PBMUTILS_H

/*===========================================================================

                  INTERNAL HEADER FILE FOR PBM UTILITIES

DESCRIPTION
  This is the internal header file for Phonebook Manager utility functions.

                      COPYRIGHT INFORMATION

Copyright (c) 2002 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmutils.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   nr      NAL China emergency requirement
06/22/16   sp      Fix KW warning
05/24/16   sp      Logging improvements
05/04/16   lm      Update ECC cache and send ready after NV Refresh
11/25/15   sp      Updated copyright
08/05/15   nr      Operator specifc NV ECC numbers configuration
09/18/14   stv     Fix for logging strings in F3s 
07/25/14   NR      Use v2.0 diag macros
07/07/14   NR      Use v2.0 diag macros
05/01/14   NR      FR 19033: Reduce Code Duplication pbm
03/06/14   stv     CM SS event handling changes for SGLTE 
10/04/13   stv     F3 message reduction 
05/10/10   krishna provided the clients Session based field info
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/08/08   clm     Queue up pbm notify requests to avoid calling them while in critical sections of pbm.
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
11/07/07   clm     Add PBM_FIELD_TECH_TYPE and PBM_FIELD_FIELDID_PRIMARY_PHONE
11/01/06   cvs     multi pb set fixes
10/10/06   cvs     Move messages to PBM's diag msg ID
08/02/06   cvs     Header file + macro cleanup for WINCE
07/27/06   slw     Differentiated field info for ADN and SIM groups
07/26/06   cvs     Fix compile errors
07/24/06   cvs     Fixes for unequal size multi pb set support
06/19/06   aps     Fix for fields being written as PBM_FT_STRING
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
03/06/06   cvs     Add new fields
02/24/06   cvs     Move string functions to pbm_conversion.
02/23/06   cvs     rework memory allocation macros
12/13/05   cvs     Implement partial PB refresh
11/16/05   cvs     Lint Fixes
10/18/05   cvs     Add new fields
09/27/05   cvs     Move size parameters to their own file.
                   Mainline USE_BEST_FIT_MEMORY_MANAGER.  Change field sizes.
08/30/05   cvs     Revert enum_rec_init() behavior of caching names.  Increase field size.
08/26/05   AT      Added new Emergency Group for searches.
08/24/05   cvs     Minor optimizations to pbm_find_name(), make functions public.
08/12/05   AT      Added support for UCS2 character set.
08/12/05   cvs     When extension records go unused, write out blank record
07/08/05   cvs     Add new fields
06/70/05   AT      One more field - increment the constant.
06/23/05   cvs     Speed up searching, using qsort() for results
05/31/05   cvs     Fix return type of pbm_convert_pbm8_to_gsm7()
05/26/05   cvs     support for API changes to make APIs more multiprocessor friendly
05/18/05   AT      Updated the string comparison functions to handle wild cards.
04/08/05   cvs     Add fields
03/28/05   RSB,cvs Added Phone Unique ID functionality
03/28/05   AT      Updated ECC requirements.
03/08/05   PA      Support for PBM notifications
02/08/05   PA      Support for re-entrant complex field enumeration.
01/20/05   PA      Moved some definitions to pbmlib.h
01/14/05   PA      Support for non-MMGSDI builds
01/12/05   PA      PBM USIM Support
01/10/05   PA      Updated maximum number of PBM fields
12/16/04   SUN     Added a new PBM field
12/15/04   SUN     Updated maximum number of PBM fields
12/09/04   PA      Correct emergency number start location
12/08/04   PA      Updated maximum number of PBM fields.
12/03/04   PA      Record validation utilities
11/19/04   PA      Code review comments
11/18/04   PA      Support for complex fields
09/29/04   PA      Initial revision.
===========================================================================*/
#include "uim_variation.h"
#include "uim_common_efs.h"
#include "pbmcache.h"
#include "pbm_tuning.h"
#include "mmgsdilib.h"
#include "uim.h"
#include "cm.h" /* for cm_sups_cmd_e_type */
#include "modem_mem.h"
#include "sys.h"

/* For accessing slot at index 0 */
#define PBM_SLOT_1_INDEX                    0

/* For accessing slot at index 1 */
#define PBM_SLOT_2_INDEX                    1

/* For accessing slot at index 2 */
#define PBM_SLOT_3_INDEX                    2

#define PBM_STAR_BCD            0xA
#define PBM_HASH_BCD            0xB
#define PBM_TIMEPAUSE_BCD       0xC
#define PBM_WILD_BCD            0xD
#define PBM_EXPANSION_BCD       0xE

#define PBM_ASYNC_BUFFERS 5

#define ALIGNMENT 4       /* Keep fields aligned on this boundry. */
#define ALIGNED_SIZE(x)   (ALIGNMENT * (((x) + ALIGNMENT - 1)/ALIGNMENT))

#ifdef VOID
#undef VOID
#endif

#define VOID
#define NO_ACTION       ;

#define EXT_REC_TYPE_FREE                    0x00
#define EXT_REC_TYPE_CALLED_PARTY_SUBADDRESS 0x01
#define EXT_REC_TYPE_ADDITIONAL_DATA         0x02
#define PBM_MAX_ECC  5
#define PBM_ECC_MAX_LEN  6
#define PBM_ECC_MIN_LEN  3
#define PBM_MAX_ECC_NUMBERS  50
#define ADN_KNOWN_REC_LEN   14
#define FDN_KNOWN_REC_LEN   14
#define LND_KNOWN_REC_LEN   14
#define MDN_1X_KNOWN_REC_LEN 11
#define OCI_KNOWN_REC_LEN   27
#define ICI_KNOWN_REC_LEN   28
#define EXT_KNOWN_REC_LEN   13
#define ANR_KNOWN_REC_LEN   15
#define CALL_HISTORY_SIZE   10  /* Number of history records supported by SIM.*/

#define PBM_MAX_SLOTS PBM_NUM_SLOTS -1
#define PBM_MAX_SESSION_PENDING 10

extern pbm_session_enum_type curr_q[PBM_MAX_SLOTS];

//lint -emacro(545,OFFSET)  Suspicious use of &.  True enough.
#define OFFSET(struct_type, field)  (((int) &(((struct_type *)4)->field)) - 4)

#define PBM_SUB_FIELD_HEADER_SIZE OFFSET(pbm_sub_field_s_type, buf)

/* Align at 4 bytes */
#define PBM_PAD_BYTES(p)  ((4 - ((uint32)(p) & 0x3)) & 0x03)

#define PBM_FIELD_ALIGN(ptr) ((void *)((ptr) + PBM_PAD_BYTES((ptr))))

#define PBM_NEXT_SUB_FIELD(sub_field)  \
               (pbm_sub_field_s_type*)( PBM_FIELD_ALIGN((uint8*)(sub_field) + PBM_SUB_FIELD_HEADER_SIZE + \
                                 (sub_field)->data_len) )
#define PBM_COMPLEX_FIELD_HEADER_SIZE OFFSET(pbm_complex_field_s_type, buf)

#define PBM_RECID_NULL      0

/* Use the following macros to print the strings */
#define UIM_PBM_MSG_SPRINTF_1(str,a)         MSG_SPRINTF_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH , str , a );
#define UIM_PBM_MSG_SPRINTF_2(str,a,b)       MSG_SPRINTF_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH , str , a , b );
#define UIM_PBM_MSG_SPRINTF_3(str,a,b,c)     MSG_SPRINTF_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH , str , a , b , c);    

/* ===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define PBM_SS_INFO_CHANGED(x)            (((x) & CM_SS_EVT_SRV_STATUS_MASK) ||  \
                                           ((x) & CM_SS_EVT_ROAM_STATUS_MASK))

#define PBM_3GPP_SYS_MODE(x)                                                     \
          (((x) == SYS_SYS_MODE_GSM) || ((x) == SYS_SYS_MODE_WCDMA)  ||          \
           ((x) == SYS_SYS_MODE_GW)  || ((x) == SYS_SYS_MODE_LTE)    ||          \
           ((x) == SYS_SYS_MODE_GWL) || ((x) == SYS_SYS_MODE_TDS))

#define PBM_3GPP2_SYS_MODE(x)                                                    \
          (((x) == SYS_SYS_MODE_CDMA) || ((x) == SYS_SYS_MODE_HDR))

/* ===========================================================================
   MACRO:       PBMUTIL_SLOT_INDEX_VALID
   DESCRIPTION: This macro is used to validate the slot index.
   ===========================================================================*/
#define PBMUTIL_SLOT_INDEX_VALID(x)                                   \
                                       (((x) == PBM_SLOT_1_INDEX) ||  \
                                        ((x) == PBM_SLOT_2_INDEX) ||  \
                                        ((x) == PBM_SLOT_3_INDEX))


#define PBM_MEM_ALLOC(dest,size)                                              \
{                                                                             \
	if((size) > 0)                                                            \
{                                                                             \
     (dest)=modem_mem_alloc((size), MODEM_MEM_CLIENT_QMI_PBM);                \
	if((dest) != NULL)                                                        \
{                                                                             \
	memset((dest),0,(size));                                                  \
}                                                                             \
}                                                                             \
}                                                                                  


//lint -emacro(774,PBM_MEM_FREEIF) doesn't matter if the if is always true here
#define PBM_MEM_FREEIF(mem)                                     \
  {                                                             \
    if ((mem))                                                  \
    {                                                           \
      	modem_mem_free((mem),MODEM_MEM_CLIENT_QMI_PBM);         \
      	(mem) = NULL;                                           \
    }                                                           \
  }

//The checks tend to be true.
//lint -emacro(774,PBM_ALLOC_MEM_W_ERR_RET)
//lint -emacro(506,PBM_ALLOC_MEM_W_ERR_RET)
#define PBM_ALLOC_MEM_W_ERR_RET(dest, size, err_code)  \
  if ((size) > 0)                                      \
  {                                                    \
    PBM_MEM_ALLOC(dest, size)                          \
    if ((dest) == NULL)                                \
    {                                                  \
      UIM_MSG_ERR_2("Could not allocate %d bytes of memory for 0x%x",  (size), &(dest)); \
      err_code;                                        \
      return PBM_ERROR;                                \
    }                                                  \
  }

/* If ret_val is not PBM_SUCCESS, prints "str" */
#define PBM_CHECK_ERR(ret_val, str)                          \
  if ((ret_val) != PBM_SUCCESS)                              \
  {                                                          \
    UIM_MSG_ERR_1(str " Error %d", (ret_val));           \
  }

/* If ret_val is not PBM_SUCCESS, prints "str"
 * In addition, it returns with ret_val */
#define PBM_CHECK_ERR_RET(ret_val, str)                      \
  if ((ret_val) != PBM_SUCCESS)                              \
  {                                                          \
    UIM_MSG_ERR_1(str " Error %d", (ret_val));           \
    return ret_val;                                          \
  }

#define PBM_CHECK_ERR_BREAK(ret_val, str)                    \
  if ((ret_val) != PBM_SUCCESS)                              \
  {                                                          \
    UIM_MSG_ERR_1(str " Error %d", (ret_val));           \
    break;                                                   \
  }

/* If x is NULL, prints error and returns ret */
#define PBM_CHECK_PTR_RET(x, ret)                             \
  if ((x) == NULL) {                                          \
    UIM_MSG_ERR_0("Unexpected NULL pointer");                 \
    return ret;                                               \
  }

/* ===========================================================================
   MACRO:       PBM_CHECK_PTR2_RET
   DESCRIPTION: Will check the parameters provided and return
                if any of the pointer is NULL
   INPUT:       p_var1   : pointer to be checked
                p_var2   : pointer to be checked
   ===========================================================================*/
#define PBM_CHECK_PTR2_RET(p_var1, p_var2, ret)                            \
  {                                                                        \
    uint8 is_null = 0;                                                     \
    if ((p_var1) == NULL)                                                  \
    {                                                                      \
      is_null = 1;                                                         \
    }                                                                      \
    else if ((p_var2) == NULL)                                             \
    {                                                                      \
      is_null = 2;                                                         \
    }                                                                      \
    if (is_null > 0)                                                       \
    {                                                                      \
      UIM_MSG_ERR_1("Unexpected NULL pointer : 0x%x", is_null);            \
      return ret;                                                          \
    }                                                                      \
  }

/* ===========================================================================
   MACRO:       PBM_CHECK_PTR3_RET
   DESCRIPTION: Will check the parameters provided and return provided error
                if any of the pointer is NULL
   ===========================================================================*/
#define PBM_CHECK_PTR3_RET(p_var1, p_var2, p_var3, ret)                \
  {                                                                    \
    uint8 is_null = 0;                                                 \
    if ((p_var1) == NULL)                                              \
    {                                                                  \
      is_null = 1;                                                     \
    }                                                                  \
    else if ((p_var2) == NULL)                                         \
    {                                                                  \
      is_null = 2;                                                     \
    }                                                                  \
    else if ((p_var3) == NULL)                                         \
    {                                                                  \
      is_null = 3;                                                     \
    }                                                                  \
    if (is_null > 0)                                                   \
    {                                                                  \
      UIM_MSG_ERR_1("Unexpected NULL pointer : 0x%x", is_null);        \
      return ret;                                                      \
    }                                                                  \
  }

/* ===========================================================================
   MACRO:       PBM_CHECK_PTR4_RET
   DESCRIPTION: Will check the parameters provided and return provided error
                if any of the pointer is NULL
   ===========================================================================*/
#define PBM_CHECK_PTR4_RET(p_var1, p_var2, p_var3, p_var4, ret)      \
  {                                                                    \
    uint8 is_null = 0;                                                 \
    if ((p_var1) == NULL)                                              \
    {                                                                  \
      is_null = 1;                                                     \
    }                                                                  \
    else if ((p_var2) == NULL)                                         \
    {                                                                  \
      is_null = 2;                                                     \
    }                                                                  \
    else if ((p_var3) == NULL)                                         \
    {                                                                  \
      is_null = 3;                                                     \
    }                                                                  \
    else if ((p_var4) == NULL)                                         \
    {                                                                  \
      is_null = 4;                                                     \
    }                                                                  \
    if (is_null > 0)                                                   \
    {                                                                  \
      UIM_MSG_ERR_1("Unexpected NULL pointer : 0x%x", is_null); 	     \
      return ret;													     \
    }                                                                  \
  }

/* ===========================================================================
   MACRO:       PBM_CHECK_PTR5_RET
   DESCRIPTION: Will check the parameters provided and return provided error
                if any of the pointer is NULL
   ===========================================================================*/
#define PBM_CHECK_PTR5_RET(p_var1, p_var2, p_var3, p_var4, p_var5, ret)      \
  {                                                                            \
    uint8 is_null = 0;                                                         \
    if ((p_var1) == NULL)                                                      \
    {                                                                          \
      is_null = 1;                                                             \
    }                                                                          \
    else if ((p_var2) == NULL)                                                 \
    {                                                                          \
      is_null = 2;                                                             \
    }                                                                          \
    else if ((p_var3) == NULL)                                                 \
    {                                                                          \
      is_null = 3;                                                             \
    }                                                                          \
    else if ((p_var4) == NULL)                                                 \
    {                                                                          \
      is_null = 4;                                                             \
    }                                                                          \
    else if ((p_var5) == NULL)                                                 \
    {                                                                          \
      is_null = 5;                                                             \
    }                                                                          \
    if (is_null > 0)                                                           \
    {                                                                          \
      UIM_MSG_ERR_1("Unexpected NULL pointer : 0x%x", is_null); 	             \
      return ret;													             \
    }                                                                          \
  }

#define PBM_CHECK_PTR_BREAK(x, ret, val)                     \
  if ((x) == NULL) {                                         \
    UIM_MSG_ERR_0("Unexpected NULL pointer ");        \
    (ret) = (val);                                           \
    break;                                                   \
  }

#define PBM_CHECK_PTR_BREAK_2(x)                     \
  if ((x) == NULL) {                                         \
    UIM_MSG_ERR_0("Unexpected NULL pointer ");        \
    break;                                                   \
  }

#define PBM_ADD_UNIQUE_ITEM_TO_LIST(item, list, list_size, loop_var)   \
          for ((loop_var)=0; (loop_var) < (list_size); (loop_var)++)   \
          {                                                            \
            if ((list)[(loop_var)] == (item))                          \
            {                                                          \
              break; /* found it in the list, exit search loop */      \
            }                                                          \
          }                                                            \
          if ((loop_var) == (list_size))                               \
          {                                                            \
            /* it's not already in the list, so add it */              \
            if ((list_size) < (int)ARR_SIZE(list))                     \
            {                                                          \
              (list)[(list_size)++] = (item);                          \
            }                                                          \
            else                                                       \
            {                                                          \
              UIM_MSG_ERR_2("outgrew list.  size %d max size %d",        \
                          (list_size), ARR_SIZE(list));             \
            }                                                          \
          }


/* ===========    SUPS Parsing related definitions / data structures       ============== */

typedef enum {
       pbm_all_ss                  =   0x00,
       pbm_allLineIdentificationSS =   0x10, /* all_line_identification */
       pbm_clip                    =   0x11, /* calling_line_identification_
                                                         presentation_service */
       pbm_clir                    =   0x12, /* calling_line_identification_
                                                         restriction_service
                                                presentation_service */
       pbm_colp                    =   0x13, /* connected_line_identification_
                                            presentation_service */
       pbm_colr                    =   0x14, /* connected_line identification_
                                             restriction_service */
       pbm_cnap                    =   0x19, /* call name identification presentation */
       pbm_mci                     =   0x1a, /* malicious call identify */
       pbm_allForwardingSS         =   0x20, /* all_call_forwarding */
       pbm_cfu                     =   0x21, /*  call_forwarding_unconditional */
       pbm_cd                      =   0x24, /* call deflection */
       pbm_allCondForwardingSS     =   0x28, /* all conditional call forwarding  */
       pbm_cfb                     =   0x29, /* call_forwarding_on_mobile_sub-
                                                scriber_busy */
       pbm_cfnry                   =   0x2a, /* call_forwarding_on_no_reply */
       pbm_cfnrc                   =   0x2b, /* call_forwarding_on_mobile_subsc-
                                                riber_unreachable */
       pbm_allCallOfferingSS       =   0x30, /* all call offering sub-services*/
       pbm_ect                     =   0x31, /* call transfer */
       pbm_mah                     =   0x32, /* mobile access hunting */
       pbm_allCallCompletionSS     =   0x40, /* all call completion */
       pbm_cw                      =   0x41, /* call waiting */
       pbm_hold                    =   0x42,
       pbm_ccbs                    =   0x43, /* completion of call to busy
                                                                 subscribers */
       pbm_allMultiPartySS         =   0x50, /* all multi-party services */
       pbm_multiPTY                =   0x51, /* multi_party_service */
       pbm_allCommunityOfInterest_SS  = 0x60, /* *** NB name deviates from
                                               09.02 ***/
       pbm_cug                     =   0x61, /* closed_user_group */
       pbm_allChargingSS           =   0x70,

       pbm_aoci                    =   0x71, /* advice_of_charge_information */
       pbm_aocc                    =   0x72, /* advice_of_charge_charge */

       pbm_allAdditionalInfoTransferSS = 0x80,
       pbm_uus                     =   0x81, /* user to user signalling */
       pbm_allCallRestrictionSS    =   0x90,
       pbm_barringOfOutgoingCalls  =   0x91,
       pbm_baoc                    =   0x92, /* barring_of_all_outgoing_calls */
       pbm_boic                    =   0x93, /* barring_of_outgoing_inter-
                                                national_calls */
       pbm_boicExHC                =   0x94, /* barring_of_outgoing_inter-
                                                national_calls_except_those_
                                                to_home_plmn */
       pbm_barringOfIncomingCalls  =   0x99,
       pbm_baic                    =   0x9a, /* barring of all incoming calls */
       pbm_bicRoam                 =   0x9b, /* barring of incomming calls when
                                                roaming outside home PLMN
                                                Country */
       pbm_allPLMN_specificSS      =   0xf0, /* all PLMN specific Supplementary
                                                services *** NB name deviates
                                                from 09.02 ***/
       pbm_chargingInfoId          =   0xa1
} pbm_ss_operation_code;

typedef enum
{
  PBM_BSG_NO_CODE = -1,
  PBM_BSG_BS_CODE,
  PBM_BSG_TS_CODE,
  PBM_BSG_MAX_CODE
} pbm_bsg_code_type;

#define PBMUTIL_SUPS_MAX_BSG_LEN            2
#define PBMUTIL_SUPS_MAX_SC_LEN             3
#define PBMUTIL_SUPS_MAX_SIA_LEN            80  /* Changed from 20 to accomodate GCF 27.22.4.11.1 Seq 1.4 */
#define PBMUTIL_SUPS_MAX_SIB_LEN            4   /* PWD LEN */
#define PBMUTIL_SUPS_MAX_SIC_LEN            4   /* PWD LEN */
#define MIN_SUPS_DIGITS                     3
#define PBM_MAX_PWD_CHAR                    4
#define PBM_MAX_USS_CHAR                    160
#define PBMUTIL_SUPS_BSG_BM( val ) (val << 16)
#define PBMUTIL_SUPS_MAX_TOKENS             6

#define PBMUTIL_SUPS_LOCAL_SC_ALL               ""
#define PBMUTIL_SUPS_LOCAL_SC_CF                "002"
#define PBMUTIL_SUPS_LOCAL_SC_CFC               "004"
#define PBMUTIL_SUPS_LOCAL_SC_CFU               "21"
#define PBMUTIL_SUPS_LOCAL_SC_CFB               "67"
#define PBMUTIL_SUPS_LOCAL_SC_CFNRY             "61"
#define PBMUTIL_SUPS_LOCAL_SC_CFNRC             "62"
#define PBMUTIL_SUPS_LOCAL_SC_CLIP              "30"
#define PBMUTIL_SUPS_LOCAL_SC_CLIR              "31"
#define PBMUTIL_SUPS_LOCAL_SC_COLP              "76"
#define PBMUTIL_SUPS_LOCAL_SC_COLR              "77"
#define PBMUTIL_SUPS_LOCAL_SC_CW                "43"
#define PBMUTIL_SUPS_LOCAL_SC_CNAP              "300"
#define PBMUTIL_SUPS_LOCAL_SC_B                 "330"
#define PBMUTIL_SUPS_LOCAL_SC_OB                "333"
#define PBMUTIL_SUPS_LOCAL_SC_IB                "353"
#define PBMUTIL_SUPS_LOCAL_SC_BAOC              "33"
#define PBMUTIL_SUPS_LOCAL_SC_BOIC              "331"
#define PBMUTIL_SUPS_LOCAL_SC_BOICEXHC          "332"
#define PBMUTIL_SUPS_LOCAL_SC_BAIC              "35"
#define PBMUTIL_SUPS_LOCAL_SC_BICROAM           "351"
#define PBMUTIL_SUPS_LOCAL_BSG_REG_PASSWD       "03"
#define PBMUTIL_SUPS_LOCAL_SC_CCBS              "37"

#define PBMUTIL_SUPS_LOCAL_BSG_ALL              ""
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_TS           "10"
#define PBMUTIL_SUPS_LOCAL_BSG_TELEPHONY        "11"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_DATA         "12"
#define PBMUTIL_SUPS_LOCAL_BSG_FAX              "13"
#define PBMUTIL_SUPS_LOCAL_BSG_SMS              "16"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_TELE_EX_SMS  "19"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_ALL    "50"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_1      "51"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_2      "51"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_3      "53"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_4      "54"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_5      "55"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_6      "56"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_7      "57"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_8      "58"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_9      "59"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_A      "60"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_B      "61"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_C      "62"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_D      "63"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_E      "64"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_F      "65"

#define PBMUTIL_SUPS_LOCAL_BSG_ALL_BS           "20"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_ASYNC        "21"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_SYNC         "22"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_DC_SYNC      "24"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_DC_ASYNC     "25"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_DED_PACKET   "26"
#define PBMUTIL_SUPS_LOCAL_BSG_ALL_DED_PAD      "27"
#define PBMUTIL_SUPS_LOCAL_BSG_12_KBIT          "29"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_ALL    "70"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_1      "71"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_2      "71"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_3      "73"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_4      "74"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_5      "75"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_6      "76"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_7      "77"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_8      "78"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_9      "79"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_A      "80"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_B      "81"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_C      "82"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_D      "83"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_E      "84"
#define PBMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_F      "85"

typedef enum
{
  PBMUTIL_SUPS_BSG_ALL_SERVICES =0x00 | (PBMUTIL_SUPS_BSG_BM(PBM_BSG_NO_CODE)),
/*
** BEARER-SERVICES
*/
  PBMUTIL_SUPS_BSG_ALL_BEARER_SERVICES           =0x00 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_DATA_CDA_SERVICES         =0x10 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDA_300BPS               =0x11 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDA_1200BPS              =0x12 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDA_1200_75BPS           =0x13 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDA_2400BPS              =0x14 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDA_4800BPS              =0x15 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDA_9600BPS              =0x16 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_DATA_CDS_SERVICES         =0x18 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDS_1200BPS              =0x1A |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDS_2400BPS              =0x1C |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDS_4800BPS              =0x1D |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_CDS_9600BPS              =0x1E |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_PAD_ACCESS_CA_SERVICES    =0x20 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PAD_ACCESS_CA_300BPS          =0x21 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PAD_ACCESS_CA_1200BPS         =0x22 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PAD_ACCESS_CA_1200_75BPS      =0x23 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PAD_ACCESS_CA_2400BPS         =0x24 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PAD_ACCESS_CA_4800BPS         =0x25|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PAD_ACCESS_CA_9600BPS         =0x26|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_DATA_PDS_SERVICES         =0x28|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_PDS_2400BPS              =0x2C|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_PDS_4800BPS              =0x2D|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_DATA_PDS_9600BPS              =0x2E|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_ALTERNATE_SPEECH_DATA_CDA =0x30|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_ALTERNATE_SPEECH_DATA_CDS =0x38|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_SPEECH_FOLLWOED_BYDATA_CDA=0x40|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_SPEECH_FOLLWOED_BYDATA_CDS=0x48|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_ASYNCHRONOUS =0x50|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_SYNCHRONOUS  =0x58|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_ASYNCHRONOUS_SERVICES     =0x60|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_SYNCHRONOUS_SERVICES      =0x68|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_BS          =0xD0|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_1            =0xD1|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_2            =0xD2|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_3            =0xD3|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_4            =0xD4|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_5            =0xD5|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_6            =0xD6|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_7            =0xD7|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_8            =0xD8|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_9            =0xD9|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_A            =0xDA|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_B            =0xDB|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_C            =0xDC|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_D            =0xDD|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_E            =0xDE|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_F            =0xDF|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_BS_CODE)),

  PBMUTIL_SUPS_BSG_ALL_TELESERVICES               =0x0 |(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_SPEECH_TRANSMISSION_SERVCES=0x10|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_TELEPHONY                      =0x11|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_EMERGENCY_CALLS                =0x12|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_SMS_SERVICES               =0x20|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_SHORT_MSG_MT_PP                =0x21|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_SHORT_MSG_MO_PP                =0x22|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_FAX_TRANSMISSION_SERVICES  =0x60|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_FAX_GROUP3_AND_ALTER_SPEECH    =0x61|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_AUTO_FAX_GROUP3                =0x62|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_FAX_GROUP4                     =0x63|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_DATA_TELESERVICES          =0x70|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_TELESERVICES_EXCEPT_SMS    =0x80|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_TS           =0xD0|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_1             =0xD1|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_2             =0xD2|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_3             =0xD3|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_4             =0xD4|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_5             =0xD5|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_6             =0xD6|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_7             =0xD7|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_8             =0xD8|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_9             =0xD9|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_A             =0xDA|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_B             =0xDB|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_C             =0xDC|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_D             =0xDD|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_E             =0xDE|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE)),
  PBMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_F             =0xDF|(PBMUTIL_SUPS_BSG_BM(PBM_BSG_TS_CODE))

}pbmutil_sups_basic_service_e_type;


typedef enum
{
 PBMUTIL_SUPS_OP_CODE_CLIP                       =   0x11, /* calling_line_identification_
                                       presentation_service */
 PBMUTIL_SUPS_OP_CODE_CLIR                       =   0x12, /* calling_line_identification_
                                       restriction_service presentation_service */
 PBMUTIL_SUPS_OP_CODE_COLP                       =   0x13, /* connected_line_identification_
                                       presentation_service */
 PBMUTIL_SUPS_OP_CODE_COLR                       =   0x14, /* connected_line identification_
                                       restriction_service */
 PBMUTIL_SUPS_OP_CODE_ALL_FORWARDING_SS          =   0x20, /* all_call_forwarding */
 PBMUTIL_SUPS_OP_CODE_CFU                        =   0x21, /*  call_forwarding_unconditional */
 PBMUTIL_SUPS_OP_CODE_ALL_CONDFWD_SS             =   0x28, /* all conditional call forwarding  */
 PBMUTIL_SUPS_OP_CODE_CFB                        =   0x29, /* call_forwarding_on_mobile_sub-
                                       scriber_busy */
 PBMUTIL_SUPS_OP_CODE_CFNRY                      =   0x2a, /* call_forwarding_on_no_reply */
 PBMUTIL_SUPS_OP_CODE_CFNRC                      =   0x2b, /* call_forwarding_on_mobile_subscriber_unreachable */
 PBMUTIL_SUPS_OP_CODE_CW                         =   0x41, /* call waiting */
 PBMUTIL_SUPS_OP_CODE_CCBS                       =   0x43, /* completion of call to busy subscribers */
 PBMUTIL_SUPS_OP_CODE_ALL_CALL_RSTRCT_SS         =   0x90,
 PBMUTIL_SUPS_OP_CODE_BARRING_OUTGOING_CALLS     =   0x91,
 PBMUTIL_SUPS_OP_CODE_BAOC                       =   0x92, /* barring_of_all_outgoing_calls */
 PBMUTIL_SUPS_OP_CODE_BOIC                       =   0x93, /* barring_of_outgoing_inter-
                                                national_calls */
 PBMUTIL_SUPS_OP_CODE_BOIC_EXHC                  =   0x94, /* barring_of_outgoing_inter-
                                                national_calls_except_those_
                                                to_home_plmn */
 PBMUTIL_SUPS_OP_CODE_BARRING_OF_INCOMING_CALLS  =   0x99,
 PBMUTIL_SUPS_OP_CODE_BAIC                       =   0x9a, /* barring of all incoming calls */
 PBMUTIL_SUPS_OP_CODE_BICROAM                    =   0x9b, /* barring of incomming calls when
                                                roaming outside home PLMN
                                                Country */
 PBMUTIL_SUPS_OP_CODE_NULL_SS_CODE               =   0x10000000
} pbmutil_sups_op_code_e_type;

typedef enum pbm_sups_cmd_e {

  PBM_SUPS_CMD_NONE = -1,
    /**< @internal */

  /* 0 */
  PBM_SUPS_CMD_REGISTER,
    /**< Register Supplementary Service Request */

  PBM_SUPS_CMD_ERASE,
    /**< Erase Supplementary Service Request */

  PBM_SUPS_CMD_ACTIVATE,
    /**< Activate Supplementary Service Request */

  PBM_SUPS_CMD_DEACTIVATE,
    /**< Deactivate Supplementary Service Request */

  /* 4 */
  PBM_SUPS_CMD_INTERROGATE,
    /**< Interrogate Supplementary Service Request */

  PBM_SUPS_CMD_PROCESS_USS,
    /**< Process Unstructured Supplementary Service */

  PBM_SUPS_CMD_REG_PASSWORD,
    /**< Register Password Supplementary Service Request */

  PBM_SUPS_CMD_MAX
    /**< @internal */
} pbm_sups_cmd_e_type;

typedef struct
{
  char *sups_operation;
  pbm_sups_cmd_e_type sups_type;
} pbmutil_sups_operation_s_type;


/*Password Information*/
typedef struct
{
  char  *ss_password;
  char  *ss_new_password;
  char  *ss_new_password_again;
}pbmutil_sups_passwd_s_type;

/*Registration Information*/
typedef struct
{
  char  *data;
  uint32    len;
  uint32    nr_timer;
}pbmutil_sups_reg_s_type;

/*Supplementary Service Data*/
typedef struct
{
   uint32                           length;
   char                         *data;
} pbmutil_sups_ussd_s_type;

typedef struct
{
   char * local_sc;         /* local value of SC */
   pbm_ss_operation_code   net_sc;           /* Network SC */
} pbmutil_sups_sc_table_s_type;

typedef struct
{
   char   *local_bsg;       /* local BSG value */
   pbmutil_sups_basic_service_e_type   net_bsg_code;     /* network BSG value */
} pbmutil_sups_bsg_table_s_type;

typedef struct
{
  pbm_sups_cmd_e_type     ss_operation;
  pbmutil_sups_op_code_e_type          ss_code;
  pbmutil_sups_basic_service_e_type    basic_service;

  union
  {
    pbmutil_sups_reg_s_type  reg;
    pbmutil_sups_passwd_s_type      passwd;
    pbmutil_sups_ussd_s_type          ussd;
    uint32                  ccbsIndex;
  }req;

}pbmutil_sups_start_s_type;

typedef boolean (*PBMUTIL_PF_CHECK_TOKEN)(int ntokens, void *tokens[],pbmutil_sups_start_s_type*);
typedef struct token_input
{
  char *reg_exp;                /* Quasi-regular expression. */
  PBMUTIL_PF_CHECK_TOKEN check_fn;      /* Function to be called. */
} PBMUTIL_TOKEN_INPUT;

/* ================================= SUPS parsing related def end ============================= */

//denotes the maximum different EFS fields supported by PBM
#define PBM_MAX_NUM_FIELDS 80
//denotes the maximum different fields supported by PBM for phonebooks other than EFS
#define PBM_MAX_OTHER_NUM_FIELDS 20

//below struct will have the info about the specifications of the different
//fields for EFS phonebooks
typedef struct
{
   //the order of the below two fields should never change
   int num_fields;//this should be always the first field
   pbm_field_info_s_type info[PBM_MAX_NUM_FIELDS];
} pbm_g_efs_field_info_s_type;
//PBM will be using the below structure to store the fieldinfo of all phonebooks
//Since it should be generic for any phonebook
//ex - we can store fieldinfo of efs,adn,ecc  in this single fieldinfo
//Hence it should be able to handle all phonebooks fields information
//like  pbm_g_efs_field_info_s_type and pbm_g_other_field_info_s_type
typedef  pbm_g_efs_field_info_s_type pbm_internal_field_info;
//below struct will have the info about the specifications of the different
//fields for other than EFS,ADN,FDN (ex-callhistory,ecc)
typedef struct
{
   //the order of the below two fields should never change
   int num_fields;
   pbm_field_info_s_type info[PBM_MAX_OTHER_NUM_FIELDS];
} pbm_g_other_field_info_s_type;

#define INVALID_GROUP        -1
#define SIM_GROUP             0
#define SDL_GROUP             1
#define CALLHISTORY_GROUP     2
#define ECC_GROUP             3 //Group for SIM ECC & NW ECC
#define ECC_OTHER_GROUP       4 //Group for hardcoded ECC and NV ECC
//any groups those belong to g_rest_field_info should be added above
#define ADN_GROUP             5
#define FDN_GROUP             6
#define EFS_GROUP             7
#define GRPNAME_GROUP         8
#define AAS_GROUP             9
#define NUM_GROUPS            10

//represent efs
extern pbm_g_efs_field_info_s_type g_efs_field_info;
//represent adn & fdn & grpname for all sessions
extern pbm_g_other_field_info_s_type g_sim_field_info[MAX_UIM_APPS][4];
//represent other groups like ecc, call-hist ,sdl
extern pbm_g_other_field_info_s_type g_rest_field_info [NUM_GROUPS-5];

extern char pbm_bcd_to_char_mapping[];

extern uint8 pbm_number_type_to_gsmton[4];

typedef struct
{
   pbm_field_s_type *fields;	   /**< data buffer pointing to fields*/
   int               num_fields; /**< Number of fields in the data buffer. */
   int               enumerator; /**< enumerator of the current field. */
} pbm_complex_result_s_type;

void pbm_session_q_init(pbm_slot_type slot);

boolean pbm_is_queue_empty(int q_id);

boolean pbm_is_queue_full(int q_id);

boolean pbm_if_queue_has_sess(pbm_session_enum_type sess,int q_id);

pbm_session_enum_type pbm_q_dequeue(int q_id);

boolean  pbm_q_enqueue(int q_id,pbm_session_enum_type sess );

void pbm_queue_algorithm(pbm_session_enum_type sess);


/* ===========================================================================
FUNCTION pbm_get_internal_field_info
DESCRIPTION
  This function is used to return the exact field info structure among .
g_rest_field_info,g_sim_field_info,g_efs_field_info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
  pbm_return_type pbm_get_internal_field_info(pbm_phonebook_type pb_id,
                                            pbm_internal_field_info *field_info);

/*===========================================================================
FUNCTION pbm_sim_device_type_to_phonebook_type;

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_sim_device_type_to_phonebook_type(pbm_device_type device_type,
																		  pbm_phonebook_type *pb_id);

/*===========================================================================
FUNCTION pbm_add_notify
DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_add_notify(pbm_notify_event_e_type evt,pbm_session_event_data_u_type evt_data);


/*===========================================================================
FUNCTION pbm_hidden_key_notify
DESCRIPTION
  This function is used to notify an hidden key verified/not verified event to all registered clients.
  Clients need to update their ADN caches as per this event.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_hidden_key_notify(pbm_session_enum_type session_type, boolean verified_status);


/*===========================================================================
FUNCTION PBM_NOTIFY

DESCRIPTION
  This function is used to notify an event to all registered clients.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_notify(pbm_notify_data_s_type *notify_data);


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
boolean pbm_action_requires_sim(pbm_phonebook_type pb_id);


/*==============================================================================

FUNCTION NAME pbm_old_recid_to_new_recid

DESCRIPTION


RETURN VALUE

DEPENDENCIES
==============================================================================*/
pbm_return_type pbm_old_recid_to_new_recid(uint16 old_rec_id,pbm_record_id_type *new_rec_id);
/*===========================================================================
FUNCTION PBM_NEW_RECID_TO_OLD_RECID

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_new_recid_to_old_recid(uint16 *old_rec_id,pbm_record_id_type new_rec_id);


/*===========================================================================
FUNCTION PBM_LOCATION_TO_ADN_REC_ID

DESCRIPTION
  Returns the ADN record id given the sim id, phonebook set and location
  of ADN record.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_adn_rec_id(pbm_session_enum_type session_type,
                                  pbm_pb_set_type pb_set,
                                  uint8 index);

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_PB_SET

DESCRIPTION
  Returns the proper pb set based on the given file id.

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_file_id_to_pb_set(pbm_file_type file_id);

/*===========================================================================
FUNCTION pbm_file_id_to_cache_type

DESCRIPTION
  This function will find the cache type associated with a given file id

SIDE EFFECTS
===========================================================================*/
pbm_cache_type_e_type pbm_file_id_to_cache_type(pbm_file_type file_id);


/*===========================================================================
FUNCTION PBM_FILE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given file id
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
void *pbm_file_id_to_cache(pbm_session_enum_type session_type,
                           pbm_file_type file_id);

/*===========================================================================
FUNCTION PBM_FILE_ID_TO_FIELD_ID

DESCRIPTION
  Returns the field id stored within the given file device.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_field_id_e_type pbm_file_id_to_field_id(pbm_file_type file_id);


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
pbm_return_type check_pb_ready(pbm_phonebook_type pb_id);

/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_FIELDS

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  passed in.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_size_from_fields( const pbm_field_s_type *fields, int num_fields);

/*===========================================================================
FUNCTION PBM_FREE_FIELDS_INTERNAL

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields_internal.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields_internal( pbm_field_s_type **fields, int num_fields);

/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS_INTERNAL

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the PBM heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields_internal(pbm_field_s_type **fields_buf_ptr, uint32 num_fields, const uint32 *sizes);

/*===========================================================================
FUNCTION PBM_COPY_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the destination field array
  and copies the data from the source field array.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_fields(pbm_field_s_type **dst, pbm_field_s_type const * const * src,
                        uint32 num_fields);

/*===========================================================================
FUNCTION PBM_BUILD_NUM_TEXT_RECORD_FROM_FIELDS

DESCRIPTION
  This function is used for the legacy API.  It takes a record that contains
  an array of fields, and builds the pbm_num_text_record_s_type record
  required for SIM writing.

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
                                           int index, pbm_num_text_record_s_type *record);

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
                                  int index, pbm_record_s_type *record);

/*===========================================================================
FUNCTION PBM_BUILD_FIELDS_FROM_RECORD

DESCRIPTION
  This function is used for the legacy API.  It takes a pbm_record_s_type
  record and builds a record that contains an array of fields expected by
  the new interface.

===========================================================================*/
pbm_addr_cache_s_type * pbm_build_fields_from_record (pbm_phonebook_type pb_id,
                                                      pbm_record_s_type *record);

/*===========================================================================
FUNCTION MATCH_FOUND

DESCRIPTION
Local utility function used by pbm_enum_rec_init() to determine whether the
given cache entry is a match for the given search criteria. Assumes that
data is null terminated.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean match_found(pbm_cat_e_type category,
                    pbm_field_id_e_type field_id,
                    const void *data_ptr,
                    uint16 data_size,
                    uint32 flags,
                    const pbm_addr_cache_s_type *cache_entry);

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
#if !defined(__GNUC__)
boolean pbm_bcd_to_ascii(__packed const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number);
#else
boolean pbm_bcd_to_ascii(PACKED const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number);
#endif /* __GNUC__ */


#if !defined(__GNUC__)
  boolean pbm_mdn_to_ascii(__packed const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number);
#else
  boolean pbm_mdn_to_ascii(PACKED const byte *bcd_number, uint8 bcd_len, uint8 ton_npi, byte *ascii_number);
#endif /* __GNUC__ */


/*===========================================================================
FUNCTION PBM_BCD_TO_BIN

DESCRIPTION
  Converts time quantities from bcd to binary.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bcd_to_bin( byte bcd );

/*===========================================================================
FUNCTION PBM_PB_ID_TO_SIM_ID

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_slot_type pbm_device_type_to_sim_id(pbm_device_type device_type);

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
boolean pbm_ascii_to_bcd(byte *ascii_number, uint8 type, byte *bcd_number);

/*===========================================================================
FUNCTION PBM_BIN_TO_BCD

DESCRIPTION
  Converts time quantities from binary to bcd.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
byte pbm_bin_to_bcd( byte bin );

/*===========================================================================
FUNCTION PBM_INIT_ID_TO_PB_ID

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS

===========================================================================*/
pbm_phonebook_type pbm_file_id_to_pb_id(pbm_session_enum_type session_type,
                                        pbm_file_type file_id);

/*===========================================================================
FUNCTION PBM_INIT_FILE_CIRCULAR

DESCRIPTION
  This function returns TRUE if the init file is circular, FALSE otherwise.

SIDE EFFECTS
===========================================================================*/
boolean pbm_file_circular(pbm_file_type file_id);

/*===========================================================================
FUNCTION PBM_RECORD_WRITE_CHECK

DESCRIPTION
  This function is does all the initial error checking for a record write operation.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_record_write_check(pbm_phonebook_type pb_id,
                                       pbm_record_id_type rec_id,
                                       uint16 cat,
                                       const pbm_field_s_type *fields,
                                       int num_fields);

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_INIT

DESCRIPTION
  This function initializes enumeration of a complex field. If there are
  complex fields within complex fields, all of them are converted into
  individual PBM fields.

  If the input field is a simple field, the simple field is returned as enumeration.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_complex_init(const pbm_field_s_type *field,
                                      int *num_fields,
                                      pbm_complex_result_s_type **presult);

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_NEXT

DESCRIPTION
  This function obtains the next sub field within a complex field, converts it
  to a normal PBM field and returns it.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_field_s_type *pbm_enum_complex_next(pbm_complex_result_s_type *result);

/*===========================================================================
FUNCTION PBM_ENUM_COMPLEX_FREE

DESCRIPTION
  This function frees the memory allocated to store the search result.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_enum_complex_free(pbm_complex_result_s_type *result);

/*===========================================================================
FUNCTION PBM_CAT_TO_PHONEBOOK

DESCRIPTION
  This function returns the phonebook associated with a given category.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_cat_to_phonebook(pbm_cat_e_type cat);

/*===========================================================================
FUNCTION PBM_PB_ID_TO_FIELDGROUP

DESCRIPTION
  This function returns the field info group corresponding to this phonebook.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int pbm_pb_id_to_fieldgroup(pbm_phonebook_type pb_id);
/*===========================================================================
FUNCTION PBM_SESSION_CAT_TO_LEGACY_CAT

DESCRIPTION
  This function convert the session category to legacy cat.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_cat_e_type pbm_session_cat_to_legacy_cat(pbm_phonebook_type pb_id,pbm_cat_e_type p_leg_cat);
/*===========================================================================
FUNCTION PBM_LEGACY_CAT_TO_SESSION_CAT

DESCRIPTION
  This function convert the slot2 category to session based cat.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_cat_e_type pbm_legacy_cat_to_session_cat(pbm_cat_e_type p_leg_cat);


/*===========================================================================
FUNCTION pbm_get_active_app

DESCRIPTION
  This function returns the active session_type corresponding to this slot.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_active_prov_session_type(pbm_slot_type slot,pbm_session_enum_type *session_type);

/*===========================================================================
FUNCTION pbm_get_active_app_type

DESCRIPTION
  This function returns the active session_type corresponding to this slot.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
mmgsdi_app_enum_type pbm_get_active_app_type(pbm_session_enum_type session_type);

/*===========================================================================
FUNCTION pbm_compare_rec_ids

DESCRIPTION
  This function compares the two rec_ids (discards slot comparision in case of Provisioning Session)

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/

boolean pbm_compare_rec_ids(pbm_record_id_type rec_id,pbm_record_id_type rec_id_other);

/*===========================================================================
FUNCTION pbm_util_is_session_provision

DESCRIPTION
  This function whether the session is provisioning or not

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_util_is_session_provision(pbm_provision_enum_type prov_type);


/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_FILE_ID

DESCRIPTION
  This function will find the file id corresponding to the given record id.

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_record_id_to_file_id(pbm_record_id_type rec_id);

/*===========================================================================
FUNCTION PBM_IS_USIM_FIELD

DESCRIPTION

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_field(pbm_field_id_e_type fid);

pbm_field_id_e_type pbm_get_primary_field_from_complex(const pbm_field_s_type *cfield);

/*===========================================================================
FUNCTION pbm_is_usim_required

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_usim_required(pbm_record_id_type rec_id, const pbm_field_s_type *fields,
                             int num_fields);


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
                                          pbm_file_type pbm_file);

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
                                          pbm_file_type pbm_file);

/*===========================================================================
FUNCTION PBM_GROUP_FIELDS

DESCRIPTION
  Group fields together. Currently we care only about grouping group ids
  together.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_group_fields(pbm_addr_cache_s_type *record);

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_pb_record_index_to_pb_set(const PB_CACHE *cache, int index);

/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_PB_SET

DESCRIPTION
  Return the PB set for a given SYNC2 index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_sync2_record_index_to_pb_set(const PB_SYNC2_CACHE *cache, int index);

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_pb_record_index_to_filename(const PB_CACHE *cache, int index);

/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILENAME

DESCRIPTION
  Return the filename for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to PB set 1, and UIM_TELECOM_ADN1)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_file_type pbm_sync2_record_index_to_filename(const PB_SYNC2_CACHE *cache, int index);

/*===========================================================================
FUNCTION PBM_PB_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_pb_record_index_to_fileloc(const PB_CACHE *cache, int index);

/*===========================================================================
FUNCTION PBM_SYNC2_RECORD_INDEX_TO_FILELOC

DESCRIPTION
  Return the file location for a given ADN index (ie, with 2 sets, 10 records
  each, index 15 maps to location 5)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_sync2_record_index_to_fileloc(const PB_SYNC2_CACHE *cache, int index);

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PB_SET

DESCRIPTION
  Map a record ID to a phonebook set

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_set_type pbm_record_id_to_pb_set(pbm_record_id_type rec_id);

/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_INDEX

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_fileloc(pbm_record_id_type rec_id);

/*===========================================================================
FUNCTION pbm_fill_adn_sfi

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_fill_adn_sfi(pbm_adn_record_id_type rec_id, byte *buffer,
                                             pbm_session_enum_type session_type);

/*===========================================================================
FUNCTION pbm_build_record_from_usim_fields

DESCRIPTION
  This function adds ato a record from USIM fields.  It expects to either be
  sent 1 field, or multiple consecutive group ID fields.


DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_build_record_from_usim_fields(pbm_record_id_type adn_rec_id,
                                       const pbm_field_s_type *fields,
                                       int num_fields,
                                       pbm_sim_record_s_type *record);
/*===========================================================================
FUNCTION pbm_allocate_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_index(pbm_record_id_type rec_id, const pbm_field_s_type *fields,
                           int num_fields, uint8 **prev_index,
                           uint8 **curr_index);


/*===========================================================================
FUNCTION pbm_free_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_index(uint8 *prev_index, uint8 *curr_index);

/*===========================================================================
FUNCTION PBM_COMPLEX_UNWRAP

DESCRIPTION
  This function converts a complex field into its constituent simple fields and
  adds it to the enumeration list.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_complex_unwrap(
                                         pbm_complex_field_s_type *complex_field,
                                         pbm_complex_result_s_type *result);

/*===========================================================================
FUNCTION pbm_copy_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_copy_usim_index(pbm_record_id_type rec_id,
                            uint8 *index_array);

/*===========================================================================
FUNCTION pbm_get_usim_index

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_get_usim_index(pbm_record_id_type rec_id,
                           const pbm_field_s_type *fields, int num_fields,
                           uint8 *index_array);

/*===========================================================================
FUNCTION PBM_GET_NEXT_USIM_CACHE

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
PB_USIM_CACHE *pbm_get_next_usim_cache(pbm_record_id_type rec_id,
                                       pbm_field_id_e_type field_id,
                                       int *index);

/*===========================================================================
FUNCTION pbm_gsm_modified

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_gsm_modified(pbm_record_id_type rec_id);


/*===========================================================================
FUNCTION pbm_is_rec_hidden

DESCRIPTION

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/

boolean pbm_is_rec_hidden(pbm_record_id_type rec_id);

/*===========================================================================
FUNCTION PBM_GET_NAME

DESCRIPTION
Extracts the name field from the cache based on a priority.

DEPENDENCIES
None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_name(const pbm_addr_cache_s_type *cache,
                             boolean pbm_sort_last_first, uint16 *nameBuff,
                             int buffSize);

/*===========================================================================
FUNCTION PBM_APPEND_PHONE_UID_FIELD

DESCRIPTION
  This function is used to append the field PBM_FIELD_PH_UID to a field list
  with the given phone unique ID.

DEPENDENCIES
  None.

SIDE EFFECTS
  Frees the existing field list. New memory is allocated for the appended field list, and the pointer
  to it is returned in the parameter.

SEE ALSO
==========================================================================*/
pbm_return_type pbm_append_phone_uid_field(uint64 ph_unique_id, pbm_field_s_type **ppfields,
                                           uint16 *pnum_fields);

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
boolean pbm_find_phone_uid_field(const pbm_field_s_type *fields, int num_fields, int *index);

/*===========================================================================
FUNCTION PBM_CONVERT_PBM8_TO_GSM7

DESCRIPTION
  This function converts a string from PBM8 alphabet to GSM7 alphabet

DEPENDENCIES

SIDE EFFECTS

COMMENTS

SEE ALSO
===========================================================================*/
void pbm_convert_pbm8_to_gsm7(char *dest, char *src, int len);

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
                             boolean fdn_comparison);


/*===========================================================================
FUNCTION pbm_adn_record_id_to_pb_set

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
pbm_pb_set_type pbm_adn_record_id_to_pb_set(pbm_session_enum_type session_type,
                                            pbm_adn_record_id_type rec_id);

/*===========================================================================
FUNCTION PBM_DEVICE_ID_TO_CACHE

DESCRIPTION
  This function will find the cache associated with a given device id
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
PB_CACHE *pbm_device_id_to_cache(pbm_session_enum_type session_type,
                                 pbm_device_type device_id);

/*===========================================================================
FUNCTION pbm_mmgsdi_session_to_pbm_session

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_mmgsdi_session_to_pbm_session(mmgsdi_session_id_type session_id);

/*===========================================================================
FUNCTION pbm_pb_type_and_slot_to_session_type

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_pb_type_and_slot_to_session_type(pbm_slot_type slot_id, pbm_pb_category_enum_type pb_type);

/*===========================================================================
FUNCTION pbm_device_type_to_phonebook_type

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_device_type_to_phonebook_type(pbm_session_enum_type session_type,
                                                     pbm_device_type device_type);

/*===========================================================================
FUNCTION  pbm_record_id_to_session_type

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_record_id_to_session_type (pbm_record_id_type record_id);

/*===========================================================================
FUNCTION  pbm_record_id_to_device_type

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_record_id_to_device_type(pbm_record_id_type record_id);

/*===========================================================================
FUNCTION  pbm_file_location_to_adn_record_id

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_adn_record_id_type pbm_file_location_to_adn_record_id(pbm_file_type file_id, int location);


/*===========================================================================
FUNCTION  pbm_adn_record_id_to_file_device_type

DESCRIPTION

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_adn_record_id_to_file_device_type(pbm_adn_record_id_type record_id);


/*===========================================================================
FUNCTION pbm_mapping_to_session;

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_mapping_to_session(pbm_pb_category_enum_type pb_category,
                                             pbm_slot_type slot_id,
                                             pbm_provision_enum_type prov_type);


/* =======================    SUPS Parsing related Functions  ========================== */
/*===========================================================================
FUNCTION pbm_util_is_valid_sups_string_find_pos_for_plus

DESCRIPTION
 API to check if the string is a valid SS string or to find the
 position for plus sign in the string.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
extern boolean pbm_util_is_valid_sups_string( char *sups_str, uint8 sups_str_len,
    int *pos, boolean find_pos);

/*===========================================================================
FUNCTION pbm_util_get_cache_number_status_flags

DESCRIPTION
 this function tells if the passed field is TON or number type
 and returns TRUE if the TON is international.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
extern boolean pbm_util_get_cache_number_status_of_ton(pbm_field_s_type *sfield_ptr);

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
extern boolean pbm_util_get_ss_ton_status_of_queried_number(boolean *valid_ss_string, boolean *contains_international_ton, int *data_size, byte *local_number);

/*===========================================================================
FUNCTION pbm_util_query_to_match_numbers

DESCRIPTION
 this function tells if the passed field is TON or number type
 and returns TRUE if the TON is international.

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
extern boolean pbm_util_query_to_match_numbers(pbm_field_id_e_type field_id, boolean contains_international_ton
                                        ,boolean cache_number_contains_ton,boolean cache_number_contains_plus, boolean valid_ss_string);

/* ======================== SUPS parsing related declarations end ======================= */

/*===========================================================================
FUNCTION pbm_util_return_fileset_for_session

DESCRIPTION
 this function returns the fileset to be used in pbm_file_to_uim_file and pbm_static_path_lookup_table

DEPENDENCIES

SIDE EFFECTS


===========================================================================*/
uint32 pbm_util_return_fileset_for_session(pbm_session_enum_type session_type);

/*===========================================================================
FUNCTION pbm_validate_pb_set

DESCRIPTION
 this function validates the pb_set value 

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
boolean pbm_validate_pb_set(pbm_pb_set_type pb_set);

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
pbm_session_enum_type   pbm_mapping_as_id_to_pbm_session( sys_modem_as_id_e_type as_id);

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
                            int *total_fields,
                            boolean skip_text_fields);

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
                            pbm_field_s_type **new_fields);

/*===========================================================================
FUNCTION pbm_compress_fields

DESCRIPTION
  Compresses the data from clients to store the text fields to the format as 
  that stored on to SIM Number to BCD format from ASCII
  
DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_compress_fields(const pbm_field_s_type *fields,
                            int num_fields,
                            pbm_field_s_type **new_fields);

/* ============================================================================
FUNCTION PBM_UTIL_GET_SLOT_INDEX

DESCRIPTION
  This function bases on the PBM slot id passed in, populates the slot index

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
);

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
);

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
  pbm_slot_type             *pbm_slot
);

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
);

#endif /* PBMUTILS_H */

