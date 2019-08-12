/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   U T I L I T Y   M O D U L E

GENERAL DESCRIPTION
  Call Manager utility functions.


Copyright (c) 2001 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmutil.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/18/14   ss      Changed IDD for Japan to 010
02/20/14   ss      Changed NDD for Macau to EMPTY value
07/11/13   jvo     Added helper cm_util_convert_to_gsm7 to reduce duplication
06/26/13   ss      Fix cmutil_number_cmp, Params Changed
05/02/13   ss      New API for converting ASCII to BCD
04/24/13   ss      New API for BCD to ASCII Conversion
10/17/12   vm      Keep CM static tables for HPCD conversion up to date
10/10/12   vm      Change converted num to conversion digits
09/26/12   vm      Fixing Lint errors
07/18/12   vm      Fix for mixed digit in replace plus with digits
04/02/12   gm      We actually print country code, not MCC.
11/01/11   fj      Fixed KW critical error.
11/07/11   fj      Fixed KW error.
02/15/12   vk      Replace usage of AEE library function with corresponding 
                   CoreBSP library
05/11/11   skp     Lint Issue Fixed.
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/15/11   rm      Correcting ascii_to_def_alpha_table for '_'
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/11/11   rm      Updated cm_util_set_default_voice_bearer_cap
12/02/10   gm      Fix to compiler warning- placed some code under a feature
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
09/27/10   gm      KW: Added comment in the code for "Not a Problem" errors
09/16/10   gm      Added new SID conflicts from IFAST.org
08/26/10   gm      Removed calls to cmnv_read_wait from frequently called APIs
07/14/10   mh      Fixed compile warnings
07/06/10   gm      Fix for Klocwork deferred critical errors.
05/27/10   np      Updated based on HPCD REV B
05/03/10   rm      Null terminating string in unpack API
02/09/10   sg      Initialize the orig param of API cm_util_default_gw_orig_params()
                   with default value.
01/19/10   rm      Correcting the GSM 7-bit default alphabet table
12/14/09   pm      Remove featurization for cm_ext_disp functions
10/29/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
09/29/09   sv      KW Fixes
08/13/09   rm      Modifying cm_util_set_default_voice_bearer_cap after
                   CMI phase-2 chnages
08/11/09   rm      Correcting BCMCS util function to retuen MAX_BCMCS_FLOWS
02/12/09   rm      Removing FEATURE_GSM_AMR_WB from cm_bearer_capability_T
                   - Initilising cm_bearer_capability_T correctly
05/22/09   sk      Centralised call control changes
                   Supplementary service control string parsing functionality
                   added to support centralized call control in CM
06/10/09   np      Fixing range check of LTM offset when daylight saving is on
05/27/09   aj      Adding support for updating clients with cdma local time
01/26/09   fj      Moved featurization into functions
12/01/08   jd      Fixed calling party number to have local IDD in ASCII
                   instead of DMTF format
10/16/08   sv      Featurizing cm_util_country_info_for_dialing to resolve
                   compilation errors in Dual Processor Targets.
10/06/08   np      Added HPCD Rev B.
10/01/08   sv      Featurizing include aeestd.h to remove lint errors
06/13/08   sv      Memory Reduction for ULC
05/12/08   sg      Updated AMR WB vocoder support for GERAN
03/17/08   ka      Fixing plus to digit conversion when converted num
                   results in dtmf for SMS.
03/14/08   sg      AMR WB vocoder support for GERAN
01/04/08   ka      Adding handset based plus code dial feature
08/07/07   ic      Lint clean up
07/09/07   cl      Move cm_ext_disp_itr_* from cm.c to cmutil.c to resolve
                   link errors. Merge 482706.
03/09/07   sk      RAM usage reduction.
06/05/06   ka/ic   Correcting buffer size in cm_util_number_to_bcd
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
09/01/05   sk      Fixed cm_util_number_to_bcd() and cm_util_bcd_to_ascii()
                   to include characters a,b and c in the conversions.
07/25/05   sk      Added functions (for fixing CR 67893):
                   - cm_util_ussd_pack()
                   - cm_util_ussd_unpack()
                   - cm_util_ascii_to_gsm_alphabet()
                   - cm_util_gsm_alphabet_to_ascii()
                   Added comments to old API indicating that the new one
                   should be used instead.
05/28/05   ka      CTM bit added to bearer capability
11/15/04   ic      Lint cleanup with CM_DEBUG on
08/11/04   ka      Utility function for reading bearer capability structure
08/04/04   ic      CM Lint clean up - cmutil.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/27/04   ws      Add routines to fill in default orig parameters.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/07/04   ka      Fixed Lint errors.
04/30/04   ws      num type and num plan updates
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ws      Added handling of '+' character in cm_utils_number_to_bcd
01/13/04   ka      Merged changes from MSM6200
11/04/03   RI      Modified cm_util_unpack() to handle line feed char.
04/28/03   AT      Lint cleanup.
01/21/03   ws      Updated copyright information for 2003
03/25/02   HD      Changed to use ISDIGIT macro.
03/12/02   RI      Fixed compiler warnings.
02/08/02   PRK     Changed the cm_util_sys_id_match function prototype.
01/23/02   PRK     Replaced cmutil prefix with cm_util prefix.
                   Added sys_id utility functions.
11/21/01   RI      Modified cmutil_phone_ascii_to_phone_bcd.
                    - modified paramter 1 from byte * to char *.
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include <string.h>
#include "comdef.h"
#include "cmutil.h"
#include <string.h>

#include <stringl/stringl.h>



#include "cmdbg.h"
#include "nv.h"
#include "cmnv.h"
#include "cmtaski.h"
#include "cmpmprx.h"
#include "cmsds.h"

#include "time_genoff.h"

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmregprxi.h"
#include "cmdrvcc.h"

/*
** A type to parameterize conversion to GSM7,
** used by the cm_util_convert_to_gsm7 function.
*/
typedef enum
{
  /* Source is ISO8859 (8-bit)*/
  CMUTIL_SRC_TYPE_ISO8859,
  
  /* Source is UTF16 (16-bit)*/
  CMUTIL_SRC_TYPE_UTF16
  
} cm_util_conversion_src_type;

#ifdef CM_GW_SUPPORTED

/* This table maps the ISO 8859-1 character set to the GSM 7-bit character set
** of 3GPP TS 23.038.  Characters mapped to ESC (0x1b) cannot be represented in
** this alphabet.  Some may be representable using the basic character set
** extension of 3GPP TS 23.038, but we are currently not doing that mapping.
*/

#define ESC 0x1b

static const byte iso8859_to_gsm7[] =
{
/* 0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f */
 ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,0x0a, ESC, ESC,0x0d, ESC, ESC, /* 0 */
 ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, /* 1 */ 
0x20,0x21,0x22,0x23,0x02,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f, /* 2 */
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f, /* 3 */
0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f, /* 4 */
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a, ESC, ESC, ESC, ESC,0x11, /* 5 */
0x2f,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f, /* 6 */
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a, ESC, ESC, ESC, ESC, ESC, /* 7 */
 ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, /* 8 */
 ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, /* 9 */
0x20,0x40, ESC,0x01,0x24,0x03, ESC,0x5f, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, /* a */
 ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC, ESC,0x60, /* b */
 ESC, ESC, ESC, ESC,0x5b,0x0e,0x1c,0x09, ESC,0x1f, ESC, ESC, ESC, ESC, ESC, ESC, /* c */
 ESC,0x5d, ESC, ESC, ESC, ESC,0x5c, ESC,0x0b, ESC, ESC, ESC,0x5e, ESC, ESC,0x1e, /* d */
0x7f, ESC, ESC, ESC,0x7b,0x0f,0x1d, ESC,0x04,0x05, ESC, ESC,0x07, ESC, ESC, ESC, /* e */
 ESC,0x7d,0x08, ESC, ESC, ESC,0x7c, ESC,0x0c,0x06, ESC, ESC,0x7e, ESC, ESC, ESC  /* f */
};

typedef struct
{
  uint16  utf16_char;
  byte    gsm7_char;
} utf16_to_gsm7_ext;

/* This table maps UTF-16 characters that are not ISO 8859-1 to their
** corresponding 3GPP TS 23.038 characters.  We search this, rather than
** index into it.
*/
static const utf16_to_gsm7_ext utf16_to_gsm7_map[] =
{
  {0x0393,  0x13},
  {0x0394,  0x10},
  {0x0398,  0x19},
  {0x039B,  0x14},
  {0x039E,  0x1a},
  {0x03A0,  0x16},
  {0x03A3,  0x18},
  {0x03A6,  0x12},
  {0x03A8,  0x17},
  {0x03A9,  0x15}
};


#define NBSP  0x00a0

/* NOTE: This is essentially the Unicode Consortium's mapping from
** ftp://ftp.unicode.org/Public/MAPPINGS/ETSI/GSM0338.TXT with the following
** exceptions:
**
** - UC feels that GSM 7-bit 0x09 should map to small letter c with cedilla
**   rather than capital letter c with cedilla.  We're sticking with our
**   previous mapping for backward compatibility.
*/
static const uint16 gsm7_to_utf16[] =
{
/* 0x00 */  '@',    0x00a3, '$',    0x00a5, 0x00e8, 0x00e9, 0x00f9, 0x00ec,
/* 0x08 */  0x00f2, 0x00c7, 0x000a, 0x00d8, 0x00f8, 0x000d, 0x00c5, 0x00e5,
/* 0x10 */  0x0394, '_',    0x03a6, 0x0393, 0x039b, 0x03a9, 0x03a0, 0x03a8,
/* 0x18 */  0x03a3, 0x0398, 0x039e, NBSP,   0x00c6, 0x00e6, 0x00df, 0x00c9,
/* 0x20 */  ' ',    '!',    0x0022, '#',    0x00a4, '%',    '&',    0x0027,
/* 0x28 */  '(',    ')',    '*',    '+',    ',',    '-',    '.',    '/',
/* 0x30 */  '0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
/* 0x38 */  '8',    '9',    ':',    ';',    '<',    '=',    '>',    '?',
/* 0x40 */  0x00a1, 'A',    'B',    'C',    'D',    'E',    'F',    'G',
/* 0x48 */  'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
/* 0x50 */  'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
/* 0x58 */  'X',    'Y',    'Z',    0x00c4, 0x00d6, 0x00d1, 0x00dc, 0x00a7,
/* 0x60 */  0x00bf, 'a',    'b',    'c',    'd',    'e',    'f',    'g',
/* 0x68 */  'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
/* 0x70 */  'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
/* 0x78 */  'x',    'y',    'z',    0x00e4, 0x00f6, 0x00f1, 0x00fc, 0x00e0
};

typedef struct
{
  byte    gsm7_char;
  uint16  utf16_char;
} gsm7_extension;

static const gsm7_extension gsm7_extension_to_utf16[] =
{
  {0x0a,  0x000c},    /* FF */
  {0x14,  '^'},
  {0x1b,  NBSP},      /* this escapes to yet another code page if necessary */
  {0x28,  '{'},
  {0x29,  '}'},
  {0x2f,  '\\'},
  {0x3c,  '['},
  {0x3d,  '~'},
  {0x3e,  ']'},
  {0x40,  '|'},
  {0x65,  0x20ac}     /* euro symbol */
};

#define NUM_GSM7_EXTENSION_CHARS (sizeof(gsm7_extension_to_utf16)/sizeof(gsm7_extension))


/* Most significant nibble of a number */
#define MOST_SIG_NIBBLE 0xF0
#define CLIR_INVOCATION "*31#"
#define CLIR_SUPRESSION "#31#"


/*===========================================================================

                     SUPS PARSING DEFINES START

===========================================================================*/

/*lint --e{402}*/
static
cmutil_parse_sups_result_e_type  cmutil_chk_ss
(
  int num_tokens,
  const void *tokens_ptr[],
  cmutil_sups_start_s_type*
);

typedef cmutil_parse_sups_result_e_type (*CMUTIL_PF_CHECK_TOKEN)(int ntokens,
                        const void *tokens_ptr[],cmutil_sups_start_s_type*);

typedef struct token_input
{
  char  *reg_exp;                       /* Quasi-regular expression. */
  CMUTIL_PF_CHECK_TOKEN  check_fn;      /* Function to be called. */
} CMUTIL_TOKEN_INPUT;

/*Extensible so that in future other MMI strings /IMEI/block, unblock PIN
  can be parsed */
/*lint --e{528}*/
static CMUTIL_TOKEN_INPUT cmutil_regex_input[] = {
{"([*#]+)(\\d+){\\*([\\+\\d]*)\\*(\\d*)\\*(\\d+)\\*(\\d+)}#{(\\+)(\\d+)#}",
  cmutil_chk_ss}, { NULL, NULL }
};

typedef struct
{
  char *sups_operation;
  cm_sups_cmd_e_type sups_type;
} cmutil_sups_operation_s_type;

/*To detect the sups operation*/
/*lint --e{528}*/
static cmutil_sups_operation_s_type cmutil_sups_operation_input[] =
{
  {"**", CM_SUPS_CMD_REGISTER},
  {"*",  CM_SUPS_CMD_ACTIVATE},
  {"#",  CM_SUPS_CMD_DEACTIVATE},
  {"*#", CM_SUPS_CMD_INTERROGATE},
  {"##", CM_SUPS_CMD_ERASE},
  {NULL, CM_SUPS_CMD_NONE}
};

typedef struct
{
  char * local_sc;         /* local value of SC */
  ss_operation_code_T   net_sc;           /* Network SC */
} cmutil_sups_sc_table_s_type;

/*Local to Network Mapping*/
/*lint --e{528}*/
static const cmutil_sups_sc_table_s_type cmutil_sups_sc_conversion_table[] =
{
  { CMUTIL_SUPS_LOCAL_SC_ALL,        all_ss },
  { CMUTIL_SUPS_LOCAL_SC_CF,         allForwardingSS },
  { CMUTIL_SUPS_LOCAL_SC_CFC,        allCondForwardingSS },
  { CMUTIL_SUPS_LOCAL_SC_CFU,        cfu },
  { CMUTIL_SUPS_LOCAL_SC_CFB,        cfb },
  { CMUTIL_SUPS_LOCAL_SC_CFNRY,      cfnry },
  { CMUTIL_SUPS_LOCAL_SC_CFNRC,      cfnrc },
  { CMUTIL_SUPS_LOCAL_SC_CLIP,       clip },
  { CMUTIL_SUPS_LOCAL_SC_CLIR,       clir },
  { CMUTIL_SUPS_LOCAL_SC_COLP,       colp },
  { CMUTIL_SUPS_LOCAL_SC_COLR,       colr },
  { CMUTIL_SUPS_LOCAL_SC_CW,         cw },
  { CMUTIL_SUPS_LOCAL_SC_B,          allCallRestrictionSS },
  { CMUTIL_SUPS_LOCAL_SC_OB,         barringOfOutgoingCalls },
  { CMUTIL_SUPS_LOCAL_SC_IB,         barringOfIncomingCalls },
  { CMUTIL_SUPS_LOCAL_SC_BAOC,       baoc },
  { CMUTIL_SUPS_LOCAL_SC_BOIC,       boic },
  { CMUTIL_SUPS_LOCAL_SC_BOICEXHC,   boicExHC },
  { CMUTIL_SUPS_LOCAL_SC_BAIC,       baic },
  { CMUTIL_SUPS_LOCAL_SC_BICROAM,    bicRoam },
  { CMUTIL_SUPS_LOCAL_SC_CNAP,       cnap },
  { CMUTIL_SUPS_LOCAL_SC_CCBS,       ccbs }
};

#define CMUTIL_SUPS_SC_MAX_ENTRY  \
(sizeof(cmutil_sups_sc_conversion_table)/sizeof(cmutil_sups_sc_table_s_type))

typedef struct
{
  char   *local_bsg;       /* local BSG value */
  cmutil_sups_basic_service_e_type   net_bsg_code;  /* network BSG value */
} cmutil_sups_bsg_table_s_type;

/*Local to Network Mapping*/
static const cmutil_sups_bsg_table_s_type \
cmutil_sups_bsg_conversion_table[] =
{
  { CMUTIL_SUPS_LOCAL_BSG_ALL,            CMUTIL_SUPS_BSG_ALL_SERVICES },
  { CMUTIL_SUPS_LOCAL_BSG_ALL_TS,         CMUTIL_SUPS_BSG_ALL_TELESERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_TELEPHONY,
                         CMUTIL_SUPS_BSG_ALL_SPEECH_TRANSMISSION_SERVCES},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_DATA,
                         CMUTIL_SUPS_BSG_ALL_DATA_TELESERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_FAX,
                         CMUTIL_SUPS_BSG_ALL_FAX_TRANSMISSION_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_SMS,
                         CMUTIL_SUPS_BSG_ALL_SMS_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_TELE_EX_SMS,
                         CMUTIL_SUPS_BSG_ALL_TELESERVICES_EXCEPT_SMS},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_ALL,
                         CMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_TS},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_1,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_1},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_2,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_2},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_3,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_3},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_4,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_4},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_5,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_5},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_6,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_6},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_7,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_7},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_8,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_8},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_9,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_9},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_A,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_A},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_B,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_B},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_C,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_C},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_D,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_D},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_E,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_E},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_TELE_F,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_TS_F},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_BS,       CMUTIL_SUPS_BSG_ALL_BEARER_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_ASYNC,
                                 CMUTIL_SUPS_BSG_ALL_ASYNCHRONOUS_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_SYNC,
                                 CMUTIL_SUPS_BSG_ALL_SYNCHRONOUS_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_DC_SYNC,
                               CMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_SYNCHRONOUS},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_DC_ASYNC,
                              CMUTIL_SUPS_BSG_ALL_DATA_CIRCUIT_ASYNCHRONOUS},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_DED_PACKET,
                                 CMUTIL_SUPS_BSG_ALL_DATA_PDS_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_ALL_DED_PAD,
                                 CMUTIL_SUPS_BSG_ALL_PAD_ACCESS_CA_SERVICES},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_ALL,
                                 CMUTIL_SUPS_BSG_ALL_PLMN_SPECIFIC_BS},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_1,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_1 },
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_2,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_2 },
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_3,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_3},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_4,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_4},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_5,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_5},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_6,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_6},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_7,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_7},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_8,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_8},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_9,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_9},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_A,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_A},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_B,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_B},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_C,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_C},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_D,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_D},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_E,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_E},
  { CMUTIL_SUPS_LOCAL_BSG_PLMN_BEAR_F,  CMUTIL_SUPS_BSG_PLMN_SPECIFIC_BS_F}
};

#define CMUTIL_SUPS_BSG_MAX_ENTRY \
(sizeof(cmutil_sups_bsg_conversion_table)/\
sizeof(cmutil_sups_bsg_table_s_type))

/*Declare the functions to fill the sups params*/
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_register_params
(
  cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_erase_params
(
  cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_activate_params
(
 cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_deactivate_params
(
  cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_interrogate_params
(
  cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_uss_params
(
  cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);
static
cmutil_parse_sups_result_e_type cmutil_sups_fill_reg_pwd_params
(
  cmutil_sups_params_s_type *,
  const cmutil_sups_start_s_type *,
  cm_num_s_type  sups_num
);

/*Index of the table is of type cm_sups_cmd_e_type*/
static cmutil_parse_sups_result_e_type (*cmutil_sups_fill_fn[])
(   cmutil_sups_params_s_type *,
    const cmutil_sups_start_s_type *,
    cm_num_s_type  sups_num
) = \
{
  cmutil_sups_fill_register_params,
  cmutil_sups_fill_erase_params,
  cmutil_sups_fill_activate_params,
  cmutil_sups_fill_deactivate_params,
  cmutil_sups_fill_interrogate_params,
  cmutil_sups_fill_uss_params,
  cmutil_sups_fill_reg_pwd_params
};

/*Function declarations which support Sups parsing*/
static
cmutil_parse_sups_result_e_type cmutil_tokenize_sups_string
(
  char *in_str,
  CMUTIL_TOKEN_INPUT *inputs,
  void *sups_start
);
static
boolean cmutil_regex_to_match
(
  char **src_ptr,
  char **regex_ptr);

static
boolean cmutil_reg_exp_chars_match
(
  boolean special_char,
  char match_char,
  char src
);

#endif /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================

                     SUPS PARSING DEFINES END

===========================================================================*/



/*===========================================================================

FUNCTION: cm_util_pack

DESCRIPTION
  USE cm_util_ussd_pack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Pack the bits in a byte.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_pack(
  byte         *coded_data,

  byte         num_chars,

  const byte         *string_to_convert
)
{

   byte up;
   byte down;
   byte coded_length;
   byte index;
   byte coded_index = 0;
   boolean added = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   CM_ASSERT(coded_data        != NULL);
   CM_ASSERT(string_to_convert != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   if ((num_chars % 8) == 0)
   {
      coded_length = (byte) (( num_chars * 7) / 8);
   }
   else
   {
      coded_length = (byte) (((num_chars * 7) / 8) + 1);
   }

   for (coded_index = 0; coded_index < coded_length; coded_index++)
   {
      coded_data[coded_index] = 0;
   }

   coded_index = 0;

   for (index = 0; index < num_chars; index++)
   {

      if ((coded_index % 7) == 0)
      {
         if (added == FALSE)
         {
            coded_data[coded_index] = string_to_convert[index];
            added = TRUE;
         }
         else
         {
            up = (string_to_convert[index] << (8 - ((coded_index + 1) % 7))); /*lint !e734 */
            down = (string_to_convert[index] >> ((coded_index + 1) % 7));

            coded_data[coded_index] |= up;
            coded_data[coded_index + 1] |= down;

            coded_index++;
            added = FALSE;
         }
      }

      else if ((coded_index % 7) == 6)
      {
         coded_data[coded_index] |= (byte) (string_to_convert[index] << 1);
         coded_index++;
      }

      else
      {

         up = (string_to_convert[index] << (8 - ((coded_index + 1) % 7))); /*lint !e734 */
         down = (string_to_convert[index] >> ((coded_index + 1) % 7));

         coded_data[coded_index] |= up;
         coded_data[coded_index + 1] |= down;

         coded_index++;
      }
   }

}


/*===========================================================================

FUNCTION: cm_util_unpack

DESCRIPTION
  USE cm_util_ussd_unpack INSTEAD. This function has bugs and exists for
  backward compatibility only.

  Unpack the bits into bytes.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_unpack(const byte *user_data, byte num_chars, byte *message_string)
{

  byte index = 0;
  int  loop_index;
  byte no_bit_mask;
  byte actual;
  byte next = 0;
  char character;
  boolean overwriting = FALSE;
  byte byte_number;
  byte char_counter = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(user_data      != NULL);
  CM_ASSERT(message_string != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  no_bit_mask = 0x7F;

  if (((num_chars * 7) % 8) != 0)
  {
     byte_number = (byte) (((num_chars * 7) / 8) + 1);
  }
  else
  {
     byte_number = (byte) ((num_chars * 7) / 8);
  }

  /*
  ** in this loop, index =        byte counter,
  **               char_counter = char counter.
  */

  char_counter = 0;
  for (loop_index = 0; loop_index < byte_number; loop_index++)
  {
     if ((loop_index % 7) == 0)
     {
        no_bit_mask = 0x7F;
     }

     actual = user_data[loop_index] & no_bit_mask;

     character = actual;
     character <<= (loop_index % 7); /*lint !e734 */
     next >>= (8 - (loop_index % 7));
     character |= next;

     /*
     ** add char to buffer
     */

     message_string[char_counter] = character;
     if( char_counter < MAX_DISPLAY_TEXT_LEN - 2 )
     {
        char_counter++;
     }

     next = user_data[loop_index] & ~(no_bit_mask);

     if ((loop_index % 7) == 6)
     {
        next >>= 1;
        character = next;

        if (next == 0x0A)
        {

           /*********************************/
           /* This is a Line Feed character */
           /*********************************/

           index = LCD_COLUMNS - char_counter % LCD_COLUMNS;

           while (index != 0)
           {
              message_string[char_counter] = ' ';
              if( char_counter < MAX_DISPLAY_TEXT_LEN - 2 )
              {
                 char_counter++;
              }
              index --;
           }

           overwriting = FALSE;
        }
        else if (next == 0x0D)
        {

           /***************************************/
           /* This is a carriage return character */
           /***************************************/

           loop_index =  LCD_COLUMNS - char_counter % LCD_COLUMNS;

           overwriting = TRUE;

           loop_index = (char_counter - 1) - loop_index;

        }
        else
        {
           if (overwriting == TRUE)
           {
              message_string[index] = character;
              loop_index++;

              if ((loop_index % LCD_COLUMNS) == 0)
              {
                 overwriting = FALSE;
              }
           }
           else
           {
              message_string[char_counter] = character;
              if( char_counter < MAX_DISPLAY_TEXT_LEN - 2 )
              {
                 char_counter++;
              }
           }
        }
     }

     no_bit_mask >>= 1;
  }
}

/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_MORE

DESCRIPTION
  Tests the iterator to see if it points to the end of the records.

DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

RETURN VALUE
  FALSE if the iterator points past the end of the record, TRUE otherwise.

SIDE EFFECTS
  Nothing.

===========================================================================*/
boolean cm_ext_disp_itr_more
(
  const cm_ext_disp_itr_type *itr_ptr             /* pointer to iterator */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( itr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return(itr_ptr->curr_offset < itr_ptr->info_ptr->len);
}


/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_NEXT

DESCRIPTION
  Increments the iterator that walks through the extended display subrecords
  which exist in a packed format in the alpha field of the call event info.

  If the end was already reached, the iterator is unchanged.

DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

RETURN VALUE
  none.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_ext_disp_itr_next
(
  cm_ext_disp_itr_type *itr_ptr             /* pointer to iterator */
)
{

  cm_ext_disp_subrecord* subrec_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(itr_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  if (itr_ptr->curr_offset < itr_ptr->info_ptr->len)
  { /* not at the end */
    /* now figure new index */
    /*lint -save -e826 -e413 */
    subrec_ptr = (cm_ext_disp_subrecord*)
                  &(itr_ptr->info_ptr->buf[itr_ptr->curr_offset]);

    itr_ptr->curr_offset += (byte)FPOS(cm_ext_disp_subrecord,chari) +
                            subrec_ptr->display_len;
    /*lint -restore */

  }
}


/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_GET

DESCRIPTION
  The contents of the current record pointed to by the iterator are
  copied into the supplied cm_ext_disp_subrecord structure.

DEPENDENCIES
  Iterator is initialized using cm_ext_disp_itr_reset

RETURN VALUE
  none.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_ext_disp_itr_get
(
  const cm_ext_disp_itr_type *itr_ptr,             /* pointer to iterator */
  cm_ext_disp_subrecord *subrec_ptr          /* area to copy next record to */
)
{
  int i;
  int len;
  cm_ext_disp_subrecord* currrec_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(itr_ptr);
  CM_ASSERT(subrec_ptr);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e826 */
  currrec_ptr = (cm_ext_disp_subrecord*)
                 &(itr_ptr->info_ptr->buf[itr_ptr->curr_offset]);
  /*lint -restore */

  subrec_ptr->display_tag = currrec_ptr->display_tag;
  subrec_ptr->display_len = currrec_ptr->display_len;

  len = MIN(currrec_ptr->display_len,
            sizeof(currrec_ptr->chari));
  for (i=0;i<len;i++)
  {
    subrec_ptr->chari[i] = currrec_ptr->chari[i];
  }
}

/*===========================================================================

FUNCTION CM_EXT_DISP_ITR_RESET

DESCRIPTION
  Resets the iterator that walks through the extended display subrecords
  which exist in a packed format in the alpha field of the call event info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
void cm_ext_disp_itr_reset
(
  cm_ext_disp_itr_type  *itr_ptr,            /* ptr to iterator */
  cm_alpha_s_type       *info_ptr            /* ptr to packed info struct */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(itr_ptr  != NULL);
  CM_ASSERT(info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* point to the packed record header */
  itr_ptr->info_ptr = info_ptr;

  /* point to the first subrecord */
  itr_ptr->curr_offset = 0;

}

/*===========================================================================

FUNCTION: cm_util_ascii_to_def_alphabet

DESCRIPTION
  USE cm_util_ascii_to_gsm_alphabet INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the ASCII to default alphabet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_ascii_to_def_alphabet(

    byte          *default_alphabet_string,

    byte          num_chars,

    const byte    *ascii_string
)
{
  #ifdef CM_GW_SUPPORTED
  byte temp_buffer[MAX_DISPLAY_TEXT_LEN];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(default_alphabet_string != NULL);
  CM_ASSERT(ascii_string            != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** convert from ascii coding into default-alphabet coding with
  ** 1 char per byte
  */

  (void) cm_util_iso8859_to_gsm7(ascii_string, num_chars,
             temp_buffer, sizeof(temp_buffer), NULL);

  /*
  ** now pack the string down to 7-bit format
  */
  cm_util_pack(default_alphabet_string, num_chars, temp_buffer); /*lint !e772 */
  #else
  SYS_ARG_NOT_USED(default_alphabet_string);
  SYS_ARG_NOT_USED(num_chars);
  SYS_ARG_NOT_USED(ascii_string);
  SYS_ARG_NOT_CONST(default_alphabet_string);
  #endif

}

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/*===========================================================================

FUNCTION: cmutil_get_max_number_chars

DESCRIPTION
  This function returns the max value of
  max_called_info_chars depending on features GSM or WCDMA
  are defined or not

DEPENDENCIES
  FEATURE_GSM ||FEATURE_WCDMA

RETURN VALUE
 maximum no. of emergency count based on definition of feature

SIDE EFFECTS
  None
===========================================================================*/
static int cmutil_get_max_number_chars( void )
{
  #ifdef CM_GW_SUPPORTED
  return CM_MAX_CALLED_INFO_CHARS;
  #else
  return CM_MAX_CALLED_INFO_CHARS_NON_UMTS;
  #endif
}
#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


/*===========================================================================

FUNCTION: cmutil_get_sys_bcmcs_max_flows

DESCRIPTION
  This function returns the max value of
  sys_bcmcs_max_flows depending on FEATURE_BCMCS
  is defined or not

DEPENDENCIES
  FEATURE_GSM ||FEATURE_WCDMA

RETURN VALUE
 maximum no. of emergency count based on definition of feature

SIDE EFFECTS
  None
===========================================================================*/
uint8 cmutil_get_sys_bcmcs_max_flows()
{
  #ifdef FEATURE_BCMCS_20
  #error code not present
#elif defined( FEATURE_BCMCS )
  return (uint8)SYS_BCMCS_10_MAX_FLOWS;
  #else
  return (uint8)SYS_BCMCS_MIN_FLOWS;
  #endif
}

/*===========================================================================

FUNCTION: cm_util_ussd_pack

DESCRIPTION
  Pack 7-bit GSM characters into bytes (8-bits)

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cm_util_ussd_pack(

    byte *packed_data,

    const byte *str,

    byte num_chars
)
{
  byte stridx=0;
  byte pckidx=0;
  byte shift;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(packed_data != NULL);
  CM_ASSERT(str         != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Loop through the 7-bit string till the last but one character.
  */
  while(stridx < (num_chars-1))
  {
   shift = stridx  & 0x07;

   /* A byte of packed data is always made up of only 2 7-bit characters. The
   ** shift of the two characters always depends on their index in the string.
   */
   /*lint -e{701} Shift left of signed quantity*/
   packed_data[pckidx++] = (str[stridx] >> shift) |
                           (str[stridx+1] << (7-shift)); /*lint !e734 */

   /* If the second characters fits inside the current packed byte, then skip
   ** it for the next iteration.
   */
   if(shift==6) stridx++;
   stridx++;
  }

  /* Special case for the last 7-bit character.
  */
  if(stridx < num_chars)
  {
    shift = stridx & 0x07;
    /* The tertiary operator (?:) takes care of the special case of (8n-1)
    ** 7-bit characters which requires padding with CR (0x0D).
    */
    packed_data[pckidx++] = ((shift == 6) ? (CHAR_CR << 1) : 0) |
                          (str[stridx] >> shift);
  }

  /* Takes care of special case when there are 8n 7-bit characters and the last
  ** character is a CR (0x0D).
  */
  if((num_chars & 0x07) == 0 && str[num_chars - 1] == CHAR_CR)
  {
    packed_data[pckidx++] = CHAR_CR;
  }

  return pckidx;
} /* cm_util_ussd_pack() */


#if defined(CM_GW_SUPPORTED)
/*===========================================================================

FUNCTION: cm_util_gsm7_unpack

DESCRIPTION:
  Unpack a packed GSM 7-bit buffer into an array of 8 bit bytes.

PARAMETERS:
  src_data          pointer to the packed GSM 7-bit string
  src_size          size of src_data in bytes
  src_spare_bits    number of spare bits that were set to 0 in the last byte in
                    the final octet (per 3GPP TS24.008 Section 10.5.3.5a). If
                    unknown, pass 0.
  dst_data          pointer to the destination buffer into which to unpack src_data
  dst_size          size of dst_data in bytes
  num_unpacked_ptr  if not NULL, the number of characters (*not* bytes)
                    unpacked will be written to this address
  
RETURN VALUE:
  CHR_CVRT_SUCCESS    - unpacking was successful
  CHR_CVRT_TRUNCATED  - unpacked string was truncated because destination
                        buffer was too small

SIDE EFFECTS
  None

===========================================================================*/
uint16 cm_util_gsm7_unpack(
    const byte  *src_data,
    size_t      src_size,
    size_t      src_spare_bits,
    byte        *dst_data,
    size_t      dst_size,
    size_t      *num_unpacked_ptr
)
{
  size_t  srcidx = 0;
  size_t  dstidx = 0;
  byte    prev = 0;
  byte    curr = 0;
  byte    shift;
  uint16  result = CHR_CVRT_SUCCESS;
    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(dst_data != NULL);
  CM_ASSERT(src_data != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while (srcidx < src_size)
  {
    if (dstidx >= dst_size - 1)
    {
      result = CHR_CVRT_TRUNCATED;
      break;
    }
    
    shift = dstidx & 0x07;
    curr = src_data[srcidx++];

    /* A 7-bit character can be split at the most between two bytes of packed
    ** data.
    */
    /*lint -e{701} Shift left of signed quantity*/
    dst_data[dstidx++] = ( (curr << shift) | (prev >> (8-shift)) ) & 0x7F;

    /* Special case where the whole of the next 7-bit character fits inside
    ** the current byte of packed data.
    */
    if (shift == 6)
    {
      /* If the next 7-bit character is a CR (0x0D) and it is the last
      ** character, then it indicates a padding character. Drop it.
      */

      if ((curr >> 1) == CHAR_CR && srcidx == src_size)
      {
        break;
      }
      if (dstidx >= dst_size - 1)
      {
        result = CHR_CVRT_TRUNCATED;
        break;
      }
      dst_data[dstidx++] = curr >> 1;
    }

    prev = curr;
  }

  /* If there are 7 spare bits in the source data, we know we decoded
  ** one extra character (as an '@').  Back off the number of characters.
  */
  if (7 == src_spare_bits)
  {
    --dstidx;
  }

  /* NULL-terminate the string */
  dst_data[dstidx]='\0';
  if (NULL != num_unpacked_ptr)
  {
    *num_unpacked_ptr = dstidx;
  }

  return result;
} /* cm_util_ussd_unpack() */


/*===========================================================================

FUNCTION: cm_util_gsm7_unpack_remove_suspected_padding

DESCRIPTION:
  Unpacks the bytes (8-bit) into 7-bit GSM characters.  If the last character
  that is unpacked is 0x00 and its position in the packed string indicates that
  it might have been the result of padding the original string with zeros,
  remove that character from the result.

PARAMETERS:
  src_data      Packed data to unpack.
  src_size      Number of bytes in src_data.
  dst_data      Pointer to buffer in which to write unpacked string.
  dst_size      Number of bytes in the dst_data buffer.
  num_unpacked  Pointer to storage for the number of bytes written
                to dst_data.

@return
  None.

@dependencies
  None.
===========================================================================*/
uint16 cm_util_gsm7_unpack_remove_suspected_padding(
    const byte  *src_data,
    size_t      src_size,
    byte        *dst_data,
    size_t      dst_size,
    size_t      *num_unpacked
)
{
  size_t  num_chars;
  uint16  result = cm_util_gsm7_unpack(src_data, src_size, 0,
                                       dst_data, dst_size, &num_chars);

  /* If we decoded a multiple of 8 characters, it could be that the final
  ** character was padding - without more knowledge we can't tell.  There
  ** are two likely cases of such padding, however:
  **
  ** 1) If we decoded a multiple of 8 characters and the final character is 0,
  **    that final character was likely 0-padded data.
  ** 2) If we decoded 8n + 1 characters and the final two characters are CR
  **    the final CR was likely padding.
  **
  ** In either of these cases, remove the last character.
  **
  ** If the unpacked string was truncated, however, the point is moot because
  ** the final character was already removed.
  */
  if (
          (result & CHR_CVRT_TRUNCATED) == 0
       && (
              (
                  (num_chars & 7) == 0
               && ('\0' == dst_data[num_chars - 1])
              )
           || (
                  (num_chars & 7) == 1
               && '\r' == dst_data[num_chars - 1]
               && '\r' == dst_data[num_chars - 2]
              )
          )
     )
  {
    --num_chars;
  }

  if (NULL != num_unpacked)
  {
    *num_unpacked = num_chars;
  }

  return result;
}


/*===========================================================================

FUNCTION: cm_util_ussd_unpack

DESCRIPTION
  THIS FUNCTION IS DEPRECATED - USE cm_util_gsm7_unpack() INSTEAD
  
  Unpack the bytes (8-bit) into 7-bit GSM characters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
byte cm_util_ussd_unpack(

    byte *str,

    const byte *packed_data,

    byte num_bytes
)
{
  size_t  num_unpacked;

  (void) cm_util_gsm7_unpack(packed_data, num_bytes, 0,
                             str, MAX_DISPLAY_TEXT_LEN,
                             &num_unpacked);

  return (byte) num_unpacked;
} /* cm_util_ussd_unpack() */

#endif  /* CM_GW_SUPPORTED */


/*===========================================================================

@FUNCTION cm_util_ascii_to_gsm_alphabet

@DESCRIPTION
  Convert the ASCII string to GSM default alphabets string and packs it
  into bytes.

@DEPENDENCIES
  None.

@RETURNS
  Number of characters written into the output buffer.

@SIDE_EFFECT
  None

===========================================================================*/
byte cm_util_ascii_to_gsm_alphabet(

    byte          *gsm_alphabet_string,

    const byte    *ascii_string,

    byte          num_chars
)
{
   #ifdef CM_GW_SUPPORTED

   byte temp_buffer[MAX_DISPLAY_TEXT_LEN];

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   CM_ASSERT(gsm_alphabet_string    != NULL);
   CM_ASSERT(ascii_string           != NULL);

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /*
   ** convert from ascii coding into GSM default-alphabet coding with
   ** 1 char per byte
   */


  (void) cm_util_iso8859_to_gsm7(ascii_string, num_chars,
             temp_buffer, sizeof(temp_buffer), NULL);

   /*
   ** now pack the string down to 7-bit format
   */
   return cm_util_ussd_pack( gsm_alphabet_string,
                             temp_buffer,
                             num_chars); /*lint !e772 */
  #else
   SYS_ARG_NOT_USED(gsm_alphabet_string);
   SYS_ARG_NOT_USED(num_chars);
   SYS_ARG_NOT_USED(ascii_string);
   SYS_ARG_NOT_CONST(gsm_alphabet_string);
   return 0;
  #endif
} /* cm_util_ascii_to_gsm_alphabet */


/*=============================================================================
FUNCTION: cm_util_utf16_to_iso8859

DESCRIPTION:
  Convert a UTF-16 string to ISO 8859-1.

PARAMETERS:
  utf16_string      String in UTF-16 encoding.
  utf16_len         Number of characters (*not* bytes) in utf16_string.
  iso8859_string    Pointer to buffer into which to convert to ISO 8859-1.
  iso8859_len       Number of characters (*not* bytes) in buffer pointed to
                    by iso8859_string.

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_INACCURATE   - at least one character in utf16_string was unable
                          to be encoded as ISO 8859-1
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
  CHR_CVRT_INACCURATE_TRUNCATED
                        - the result is inaccurate and was truncated
=============================================================================*/
uint16 cm_util_utf16_to_iso8859(
  const uint16  *utf16_string,
  size_t        utf16_len,
  uint8         *iso8859_string,
  size_t        iso8859_len
  )
{
  size_t  i;
  uint16  utf16_char;
  uint16  result = CHR_CVRT_SUCCESS;

  for (i = 0; i < utf16_len;)
  {
    if (i >= iso8859_len - 1)
    {
      result |= CHR_CVRT_TRUNCATED;
      break;
    }

    utf16_char = utf16_string[i];

    if (0xff < utf16_char)
    {
      result |= CHR_CVRT_INACCURATE;
    }

    iso8859_string[i++] = (uint8) (utf16_char & 0xff);
  }
	
  iso8859_string[i] = '\0';

  return result;
}


/*=============================================================================
FUNCTION: cm_util_iso8859_to_utf16

DESCRIPTION:
  Convert an ISO 8859-1 string to UTF-16

PARAMETERS:
  iso8859_string    String in ISO 8859-1 encoding..
  iso8859_len       Number of characters (*not* bytes) in iso8859_string.
  utf16_string      Pointer to buffer into which to convert to UTF-16.
  utf16_len         Number of characters (*not* bytes) in buffer pointed to by
                    utf16_string.

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
=============================================================================*/
uint16 cm_util_iso8859_to_utf16(
  const uint8   *iso8859_string,
  size_t        iso8859_len,
  uint16        *utf16_string,
  size_t        utf16_len
  )
{
  size_t  i;
  uint16  result = CHR_CVRT_SUCCESS;

  for (i = 0 ; i < iso8859_len ; ++i)
  {
    if (i >= utf16_len - 1)
    {
      result |= CHR_CVRT_TRUNCATED;
      break;
    }

    utf16_string[i] = (uint16) iso8859_string[i];
  }

  utf16_string[i] = 0;

  return result;
}


#if defined(CM_GW_SUPPORTED)

/*=============================================================================
FUNCTION: get_utf16_for_gsm7_extension

DESCRIPTION:
  Return the UTF-16 codepoint for a character in the GSM 7-bit basic character
  set extension encoding.

PARAMETERS:
  gsm7_char   The GSM 7-bit character that follows the ESC that indicates the
              the use of the basic character set extension.

RETURN VALUE:
  UTF-16 codepoint for the character.
=============================================================================*/
static uint16 get_utf16_for_gsm7_extension(
  byte  gsm7_char
  )
{
  int   i;

  for (i = 0 ; i < NUM_GSM7_EXTENSION_CHARS ; ++i)
  {
    if (gsm7_extension_to_utf16[i].gsm7_char == gsm7_char)
    {
      return gsm7_extension_to_utf16[i].utf16_char;
    }
  }

  return NBSP;
}

/*=============================================================================
FUNCTION: get_utf16_for_gsm7

DESCRIPTION:
  Return the UTF-16 codepoint for a character in the GSM 7-bit basic character
  set encoding.

PARAMETERS:
  gsm7_char   The GSM 7-bit character

RETURN VALUE:
  UTF-16 codepoint for the character.
=============================================================================*/
static __inline uint16 get_utf16_for_gsm7(
  byte    gsm7_char
  )
{
  return gsm7_to_utf16[gsm7_char];
}


/*=============================================================================
FUNCTION: get_gsm7_for_utf16

DESCRIPTION:
  Return the GSM 7-bit character for a UTF-16 codepoint.

PARAMETERS:
  gsm7_char   The GSM 7-bit character

RETURN VALUE:
  UTF-16 codepoint for the character.
=============================================================================*/
static uint8 get_gsm7_for_utf16(
  uint16  utf16_char
  )
{
  int i;

  /* If the character to be converted is in the ISO 8859-1 range,
  ** we do an indexed lookup in that mapping table.
  */
  if (0xff >= utf16_char)
  {
    return iso8859_to_gsm7[utf16_char];
  }

  /* If the character is out of the ISO 8859-1 range, we walk through
  ** the (short) table of UTF-16 -> GSM 7-bit mappings to see if there
  ** is a mapping.
  */
  for (i = 0 ; i < ARR_SIZE(utf16_to_gsm7_map) ; ++i)
  {
    if (utf16_to_gsm7_map[i].utf16_char == utf16_char)
    {
      return utf16_to_gsm7_map[i].gsm7_char;
    }
  }

  /* Otherwise, return ESC to indicate that the character can't be mapped.
  */
  return ESC;
}


#endif /* CM_GW_SUPPORTED */



/*=============================================================================
FUNCTION: cm_util_gsm7_to_utf16

DESCRIPTION:
   Unpacks bytes of data into a 7-bit GSM default alphabet
   string and then converts it to an ISO 8859-1 string.

PARAMETERS:
  gsm7_string       Packed GSM 7-bit string.
  gsm7_len          Number of bytes in gsm7_string.
  use_extension     If TRUE, use Basic Character Set Extension of 3GPP TS 23.038.
                    If FALSE, only return characters from the Basic Character Set.
  utf16_string      Pointer to buffer into which to convert to UTF-16
  utf16_len         Number of characters (*not* bytes) in buffer pointed to by
                    utf16_string.
  num_converted     Pointer to storage for number of characters that were
                    written to utf16_string (not counting the terminating NUL).

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
=============================================================================*/
uint16 cm_util_gsm7_to_utf16(
  const byte  *gsm7_string,     /* packed gsm 7-bit string */
  size_t      gsm7_len,         /* number of bytes in gsm7_string */
  boolean     use_extension,    /* should the GSM 7-bit extension table be used? */
  uint16      *utf16_string,    /* pointer to where to put converted string */
  size_t      utf16_len,        /* size of buffer pointed to by utf16_string */
  size_t      *num_converted_ptr/* pointer in which to store number of
                                   characters written to utf16_string */
  )
{
  size_t  num_utf16 = 0;
  uint16  result = CHR_CVRT_SUCCESS;

#ifdef CM_GW_SUPPORTED
  byte    temp_buffer[MAX_DISPLAY_TEXT_LEN];
  size_t  num_gsm7;
  size_t  i;
  uint16  utf16_char;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - */

  CM_ASSERT(gsm7_string != NULL);
  CM_ASSERT(utf16_string != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - */

  /*
  ** unpack the string from 7-bit format into 1 char per byte format
  */
  result = cm_util_gsm7_unpack(gsm7_string, gsm7_len, 0,
                    temp_buffer, ARR_SIZE(temp_buffer),
                    &num_gsm7);

  /*
  ** Convert the string
  */
  for (i = 0 ; i < num_gsm7 ; ++i)
  {
    utf16_char = get_utf16_for_gsm7(temp_buffer[i]);

    /* A non-breaking space (NBSP) indicates that the next character is
    ** escaped if we're using the GSM 7-bit extension table
    */
    if (NBSP == utf16_char && use_extension && i < num_gsm7 - 1)
    {
      ++i;
      utf16_char = get_utf16_for_gsm7_extension(temp_buffer[i]);
    }

    if (num_utf16 >= utf16_len - 1)
    {
      result |= CHR_CVRT_TRUNCATED;
      break;
    }

    utf16_string[num_utf16++] = utf16_char;
  }

  utf16_string[num_utf16] = 0;
  
#else

/*lint -save -esym(818, utf16_string) */
  SYS_ARG_NOT_USED(gsm7_string);
  SYS_ARG_NOT_USED(gsm7_len);
  SYS_ARG_NOT_USED(use_extension);
  SYS_ARG_NOT_USED(utf16_string);
  SYS_ARG_NOT_USED(utf16_len);
  result = CHR_CVRT_NOT_SUPPORTED;
/*lint -restore */
  
#endif

  if (NULL != num_converted_ptr)
  {
    *num_converted_ptr = num_utf16;
  }
  
  return result;
}

/*=============================================================================
FUNCTION: cm_util_convert_to_gsm7

DESCRIPTION:
  Convert a string to GSM 7-bit.

PARAMETERS:
  src_string    String in the specified encoding; character size varies by
                encoding type.
  string_len    Number of characters (*not* simply bytes) in src_string.
  gsm7_string   Pointer to buffer into which to convert to GSM 7-bit.
  gsm7_len      Number of characters (bytes) in buffer pointed to
                by gsm7_string.
  num_converted Pointer to storage for the number of bytes converted into
                gsm7_string.
  src_type      The type of source string (cm_util_conversion_src_type).
  
RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_INACCURATE   - at least one character in src_string was unable
                          to be encoded as ISO 8859-1
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
  CHR_CVRT_INACCURATE_TRUNCATED
                        - the result is inaccurate and was truncated
=============================================================================*/
uint16 cm_util_convert_to_gsm7(
  const void                    *src_string,
  size_t                        string_len,
  uint8                         *gsm7_string,
  size_t                        gsm7_len,
  size_t                        *num_converted,
  cm_util_conversion_src_type   src_type
  )
{
  uint16  result = CHR_CVRT_SUCCESS;
  size_t  num_gsm7 = 0;

#ifdef CM_GW_SUPPORTED
  size_t  i;
  uint8   gsm7_char;

  for (i = 0 ; i < string_len ; ++i)
  {
    if (i >= gsm7_len)
    {
      result |= CHR_CVRT_TRUNCATED;
      break;
    }

    switch (src_type){
      case CMUTIL_SRC_TYPE_ISO8859:
        // Convert from ISO8859 to GSM7
        gsm7_char = iso8859_to_gsm7[((const byte *)src_string)[i]];
        break;
      case CMUTIL_SRC_TYPE_UTF16:
        // Convert from UTF16 to GSM7
        gsm7_char = get_gsm7_for_utf16(((const uint16 *)src_string)[i]);
        break;
      default:
        // We don't support any other conversion to GSM7 here
        CM_ERR_1 ("Invalid cm_util_conversion_src_type: %d", src_type);
        /*  This return adds to the meaning of CHR_CVRT_NOT_SUPPORTED
        **
        **  Original meaning: 
        **    The build does not support GSM/WCDMA, as a result the requested
        **    conversion (involving GSM 7-bit) cannot be done.
        **
        **  Added meaning:
        **    This helper function does not support the source type
        **    indicated.
        **
        **  Original meaning description is in public api.
        */
        return CHR_CVRT_NOT_SUPPORTED;
    }
    
    if (ESC == gsm7_char)
    {
      result |= CHR_CVRT_INACCURATE;
      gsm7_char = '?';
    }

    gsm7_string[num_gsm7++] = gsm7_char;
  }
  
#else

/*lint -save -esym(818, gsm7_string) */
  SYS_ARG_NOT_USED(gsm7_string);
  SYS_ARG_NOT_USED(gsm7_len);
  SYS_ARG_NOT_USED(src_string);
  SYS_ARG_NOT_USED(string_len);  
  SYS_ARG_NOT_USED(src_type);
  result = CHR_CVRT_NOT_SUPPORTED;
/*lint -restore */
  
#endif

  /// The code here doesn't raise an error if the output buffer is null
  /// ... do we really want to silently ignore this?
  if (NULL != num_converted)
  {
    *num_converted = num_gsm7;
  }

  return result;
}


/*=============================================================================
FUNCTION: cm_util_utf16_to_gsm7

DESCRIPTION:
  Convert a UTF-16 string to GSM 7-bit.

PARAMETERS:
  utf16_string  String in UTF-16 encoding.
  utf16_len     Number of characters (*not* bytes) in utf16_string.
  gsm7_string   Pointer to buffer into which to convert to GSM 7-bit.
  gsm7_len      Number of characters (bytes) in buffer pointed to
                by gsm7_string.
  num_converted Pointer to storage for the number of bytes converted into
                gsm7_string.

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_INACCURATE   - at least one character in utf16_string was unable
                          to be encoded as ISO 8859-1
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
  CHR_CVRT_INACCURATE_TRUNCATED
                        - the result is inaccurate and was truncated
=============================================================================*/
uint16 cm_util_utf16_to_gsm7(
  const uint16  *utf16_string,
  size_t        utf16_len,
  uint8         *gsm7_string,
  size_t        gsm7_len,
  size_t        *num_converted
  )
{
  return cm_util_convert_to_gsm7( (const void *) utf16_string,
                                  utf16_len,
                                  gsm7_string,
                                  gsm7_len,
                                  num_converted,
                                  CMUTIL_SRC_TYPE_UTF16);
}


/*=============================================================================
FUNCTION: cm_util_iso8859_to_gsm7

DESCRIPTION:
  Convert an ISO 8859-1 string to GSM 7-bit.

PARAMETERS:
  iso8859_string  String in ISO 8859-1 encoding.
  iso8859_len     Number of characters (*not* bytes) in iso8859_string.
  gsm7_string     Pointer to buffer into which to convert to GSM 7-bit.
  gsm7_len        Number of characters (bytes) in buffer pointed to
                  by gsm7_string.
  num_converted   Pointer to storage for the number of bytes converted into
                  gsm7_string.

RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_INACCURATE   - at least one character in utf16_string was unable
                          to be encoded as ISO 8859-1
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
  CHR_CVRT_INACCURATE_TRUNCATED
                        - the result is inaccurate and was truncated
=============================================================================*/
uint16 cm_util_iso8859_to_gsm7(
  const byte  *iso8859_string,
  size_t      iso8859_len,
  uint8       *gsm7_string,
  size_t      gsm7_len,
  size_t      *num_converted
  )
{
  return cm_util_convert_to_gsm7( (const void *) iso8859_string,
                                  iso8859_len,
                                  gsm7_string,
                                  gsm7_len,
                                  num_converted,
                                  CMUTIL_SRC_TYPE_ISO8859);
}

#if defined(CM_GW_SUPPORTED)

/*=============================================================================
FUNCTION: cm_util_unpacked_gsm7_to_iso8859

DESCRIPTION:
   Converts a string in unpacked GSM 7-bit encoding into an ISO 8859-1 string.

PARAMETERS:
  gsm7_string       GSM alphabet string.
  gsm7_len          Number of bytes in gsm7_string.
  use_extension     If TRUE, use Basic Character Set Extension of 3GPP TS 23.038.
                    If FALSE, only return characters from the Basic Character Set.
  iso8859_string    Pointer to buffer into which to convert to ISO 8859-1.
  iso8859_len       Number of characters (*not* bytes) in buffer pointed to by
                    iso8859_string.
  num_converted     Pointer to storage for number of characters that were
                    written to iso8859_string (not counting the terminating NUL).
RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_INACCURATE   - at least one character in gsm7_string was unable
                          to be encoded as ISO 8859-1
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
  CHR_CVRT_INACCURATE_TRUNCATED
                        - the result is inaccurate and was truncated

=============================================================================*/
static uint16 cm_util_unpacked_gsm7_to_iso8859(
  const byte  *gsm7_string,
  size_t      gsm7_len,
  boolean     use_extension,
  byte        *iso8859_string,
  size_t      iso8859_len,
  size_t      *num_converted_ptr
  )
{
  size_t  num_iso8859 = 0;
  uint16  result = CHR_CVRT_SUCCESS;
  size_t  i;
  uint16  utf16_char;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - */

  CM_ASSERT(gsm7_string != NULL);
  CM_ASSERT(iso8859_string != NULL);

  /*
  ** Convert the string
  */
  for (i = 0 ; i < gsm7_len ; ++i)
  {
    utf16_char = get_utf16_for_gsm7(gsm7_string[i]);

    /* A non-breaking space (NBSP) indicates that the next character is
    ** escaped if we're using the GSM 7-bit extension table
    */
    if (NBSP == utf16_char && use_extension && i < gsm7_len - 1)
    {
      ++i;
      utf16_char = get_utf16_for_gsm7_extension(gsm7_string[i]);
    }

    /* If the character can't be expressed in a single byte, we'll need to
    ** use UTF-16.  In this case, to maintain backward compatibility, we'll
    ** replace the character with '?'.
    */
    if (utf16_char > 0xff)
    {
      result |= CHR_CVRT_INACCURATE;
      utf16_char = '?';
    }

    if (num_iso8859 >= iso8859_len - 1)
    {
      result |= CHR_CVRT_TRUNCATED;
      break;
    }

    iso8859_string[num_iso8859++] = (byte) (utf16_char & 0xff);
  }

  iso8859_string[num_iso8859] = '\0';

  if (NULL != num_converted_ptr)
  {
    *num_converted_ptr = num_iso8859;
  }
  
  return result;
}

#endif    /* CM_GW_SUPPORTED */


/*=============================================================================
FUNCTION: cm_util_gsm7_to_iso8859

DESCRIPTION:
   Unpacks bytes of data into a 7-bit GSM default alphabet
   string and then converts it to a UTF-16 string.

PARAMETERS:
  gsm7_string       GSM alphabet string.
  gsm7_len          Number of bytes in gsm7_string.
  use_extension     If TRUE, use Basic Character Set Extension of 3GPP TS 23.038.
                    If FALSE, only return characters from the Basic Character Set.
  iso8859_string    Pointer to buffer into which to convert to ISO 8859-1.
  iso8859_len       Number of characters (*not* bytes) in buffer pointed to by
                    iso8859_string.
  num_converted     Pointer to storage for number of characters that were
                    written to iso8859_string (not counting the terminating NUL).
RETURN VALUE:
  CHR_CVRT_SUCCESS      - conversion was successful
  CHR_CVRT_INACCURATE   - at least one character in gsm7_string was unable
                          to be encoded as ISO 8859-1
  CHR_CVRT_TRUNCATED    - destination buffer was not large enough to hold
                          the converted string, so it was truncated
  CHR_CVRT_INACCURATE_TRUNCATED
                        - the result is inaccurate and was truncated
=============================================================================*/
uint16 cm_util_gsm7_to_iso8859(
  const byte  *gsm7_string,     /* packed gsm 7-bit string */
  size_t      gsm7_len,         /* number of bytes in gsm7_string */
  boolean     use_extension,    /* should the GSM 7-bit extension table be used? */
  byte        *iso8859_string,  /* pointer to where to put converted string */
  size_t      iso8859_len,      /* size of buffer pointed to by iso8859_string */
  size_t      *num_converted_ptr/* pointer in which to store number of
                                   characters written to iso8859_string */
  )
{
  uint16  result = CHR_CVRT_NOT_SUPPORTED;

#ifdef CM_GW_SUPPORTED
  byte    temp_buffer[MAX_DISPLAY_TEXT_LEN];
  size_t  num_gsm7;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - */

  CM_ASSERT(gsm7_string != NULL);
  CM_ASSERT(iso8859_string != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - */

  /*
  ** unpack the string from 7-bit format into 1 char per byte format
  */
  result = cm_util_gsm7_unpack(gsm7_string, gsm7_len, 0,
                    temp_buffer, ARR_SIZE(temp_buffer),
                    &num_gsm7);

  /*
  ** Do the conversion.
  */
  result |= cm_util_unpacked_gsm7_to_iso8859(temp_buffer, num_gsm7, use_extension,
                  iso8859_string, iso8859_len, num_converted_ptr);

#else

  /*lint -save -esym(818, iso8859_string */
  SYS_ARG_NOT_USED(gsm7_string);
  SYS_ARG_NOT_USED(gsm7_len);
  SYS_ARG_NOT_USED(use_extension);
  SYS_ARG_NOT_USED(iso8859_string);
  SYS_ARG_NOT_USED(iso8859_len);

  if (NULL != num_converted_ptr)
  {
    *num_converted_ptr = 0;
  }
  /*lint -restore */
  
#endif

  return result;
}


/*===========================================================================

@FUNCTION: cm_util_gsm_alphabet_to_ascii

@DESCRIPTION
  THIS FUNCTION IS DEPRECATED - USE cm_util_gsm7_to_iso8859() INSTEAD

  Unpacks bytes of data into 7-bit GSM default alphabet string and then
  converts it to an ASCII string.

@DEPENDENCIES
  None.

@RETURNS
  Number of characters written into the output buffer.

@SIDE_EFFECT
  None

===========================================================================*/
byte cm_util_gsm_alphabet_to_ascii(
    byte        *ascii_string,
    const byte  *gsm_alphabet_string,
    byte        num_bytes
)
{
  size_t  num_converted;
  
  (void) cm_util_gsm7_to_iso8859(
            gsm_alphabet_string,
            num_bytes,
            FALSE,                /* don't use extensions - for backward compatibility */
            ascii_string,
            MAX_USS_CHAR_ASCII, /* need *some* length */
            &num_converted
            );

  return (byte) num_converted;
} /* cm_util_gsm_alphabet_to_ascii */



/*===========================================================================

@FUNCTION: cm_util_gsm_alphabet_to_ascii_internal

@DESCRIPTION
  THIS FUNCTION IS DEPRECATED - USE cm_util_gsm7_to_iso8859() INSTEAD

  Unpacks bytes of data into 7-bit GSM default alphabet string and then
  converts it to an ASCII string.

@DEPENDENCIES
  None.

@RETURNS
  Number of characters written into the output buffer.

@SIDE_EFFECT
  None

===========================================================================*/
byte cm_util_gsm_alphabet_to_ascii_internal(
    byte        *ascii_string,
    size_t       ascii_string_len,
    const byte  *gsm_alphabet_string,
    byte        num_bytes
)
{
  size_t  num_converted;
  
  (void) cm_util_gsm7_to_iso8859(
            gsm_alphabet_string,
            num_bytes,
            FALSE,                /* don't use extensions - for backward compatibility */
            ascii_string,
            ascii_string_len, /* need *some* length */
            &num_converted
            );

  return (byte) num_converted;
} /* cm_util_gsm_alphabet_to_ascii_internal */


/*===========================================================================

FUNCTION: cm_util_def_alphabet_to_ascii

DESCRIPTION
  THIS FUNCTION IS DEPRECATED - USE cm_util_gsm7_to_iso8859() INSTEAD

  USE cm_util_gsm_alphabet_to_ascii INSTEAD. This function has bugs and
  exists for backward compatibility only.

  Convert the default alphabet to ASCII.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_def_alphabet_to_ascii(

    const byte    *default_alphabet_string,

    byte    num_chars,

    byte    *ascii_string
)
{

  #ifdef CM_GW_SUPPORTED
  byte temp_buffer[MAX_DISPLAY_TEXT_LEN];
  size_t  num_converted;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(default_alphabet_string != NULL);
  CM_ASSERT(ascii_string            != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
  ** unpack the string from 7-bit format into 1 char per byte format
  */

  cm_util_unpack(default_alphabet_string, num_chars, temp_buffer);

  /*
  ** now convert from default alphabet coding into ascii coding
  */
  (void) cm_util_unpacked_gsm7_to_iso8859(temp_buffer, num_chars, FALSE,
                  ascii_string, MAX_DISPLAY_TEXT_LEN, &num_converted);

  #else
  SYS_ARG_NOT_USED(default_alphabet_string);
  SYS_ARG_NOT_USED(num_chars);
  SYS_ARG_NOT_USED(ascii_string);
  SYS_ARG_NOT_CONST(ascii_string);
  #endif
}



/*===========================================================================

FUNCTION: cm_util_number_to_bcd
DESCRIPTION
  Convert the phone number from ASCII to BCD

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cm_util_number_to_bcd(const cm_num_s_type *number, byte *bcd_number)
{
  int i, j;                        /* Control variables for loops. */
  int bcd_index      = BCD_NUM;    /* Index into output bcd_number */

  /* (digits + number_type) in bcd can be size CM_CALLED_PARTY_BCD_NO_LENGTH
  ** temp needs to be twice that of bcd. CM_CALLED_PARTY_BCD_NO_LENGTH is
  ** GW specific so using CM_MAX_NUMBER_CHARS instead.
  */
  /* KW: there should not be any problem in initialization
  */
  #ifdef CM_GW_SUPPORTED
  uint8 temp[2 * CM_MAX_NUMBER_CHARS] = { 0 };
  #else
  uint8 temp[2 * CM_MAX_NUMBER_CHARS_NON_UMTS] = { 0 };
  #endif

  uint8 number_type = 0;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(number     != NULL);
  CM_ASSERT(bcd_number != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store the number type of string coming in, it may be changed later */
  number_type = number->number_type;

  if ( number->len >= CM_MAX_NUMBER_CHARS )
  {
    CM_ERR_2( "bcd_number length %d is greater than max val %d", number->len, \
                                                          CM_MAX_NUMBER_CHARS);
    return;
  }

  /* Copy the phone number into its temporary home */
  for (i = 0, j = 0; i < number->len; i++)
  {
    /*
    ** If international number, '+' in beginning of dialed string, ignore '+'
    * and set the number_type to INTERNATIONAL.
    */
    if (i == 0)
    {
      if (number->buf[0] == '+')
      {
        number_type = (uint8)CM_NUM_TYPE_INTERNATIONAL;
        continue;
      }
    }

    /* If its a digit we care about.... */
    if (ISDIGIT(number->buf[i]) ||
        number->buf[i] == '*' ||
        number->buf[i] == '#' ||
        INRANGE(number->buf[i], 'a', 'c') ||
        INRANGE(number->buf[i], 'A', 'C') )
    {
      /* Store the bcd digits into temp. */
      /* KW: j is restricted by number->len, which is < CM_MAX_NUMBER_CHARS <
      **     CM_MAX_NUMBER_CHARS_NON_UMTS, so j is within temp bounds
      */
      temp[j++] = (number->buf[i] == '#') ? BCD_HASH :
                  (number->buf[i] == '*') ? BCD_STAR :
                  INRANGE(number->buf[i], 'a', 'c') ? (0x0C + number->buf[i] - 'a') :
                  INRANGE(number->buf[i], 'A', 'C') ? (0x0C + number->buf[i] - 'A') :
                  (number->buf[i] & 0x0f);
    }
  }
  /* Odd number of digits must have the 0x0f at the end. */
  if (j & 0x01)
  {
    temp[j++] = 0x0f;
  }

  /* Now that temp has the bcd codes in natural order... Squish them together
   * and reverse the order per bcd coding.
   */
  for (i = 0; i < j; i+=2)
  {
    /*lint -esym(644,temp)*/
    bcd_number[bcd_index++] = (byte) (temp[i+1] << 4) | temp[i];
    /*lint +esym(644,temp)*/
  }

  /* Put length value in to first element of bcd number array and the number
   * type and number plan into the second entry.
   */
  bcd_number[BCD_NT_NPI] = (byte) (0x80 | (number_type << 4) | number->number_plan);
  bcd_number[BCD_LEN] = (byte) (bcd_index - 1);

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION: ascii_char_to_bcd_byte

DESCRIPTION
  Convert an ASCII character byte to BCD byte.
 
  This function is used as a utility function for
  cm_util_ascii_num_to_bcd_num function. Any other
  invocations must appropriately handle the missing
  conditions.

DEPENDENCIES
  None.

RETURN VALUE
  BCD byte

SIDE EFFECTS
  None

===========================================================================*/
static byte ascii_char_to_bcd_byte( uint8 achar )
{
   if ( achar == '*' )
   {
      return BCD_STAR;
   }
   else if ( achar == '#' )
   {
      return BCD_HASH;
   }
   else if ( INRANGE( achar, 'a', 'c' ) )
   {
      return ( 0x0C + achar - 'a' );
   }
   else if ( INRANGE( achar, 'A', 'C' ) )
   {
      return ( 0x0C + achar - 'A' );
   }
   else if ( ISDIGIT( achar ) )
   {
      return ( achar & 0x0f );
   }
   else
   {
      return BCD_INVALID_BYTE;
   }
}


/*===========================================================================

FUNCTION: cm_util_ascii_num_to_bcd_num 
 
DESCRIPTION
  Convert the phone number from ASCII to BCD

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
boolean cm_util_ascii_num_to_bcd_num( const cm_num_s_type *number, byte *bcd_number, size_t bcd_size )
{
   int i;                                           /* Control variable for loops */
   size_t bcd_index         = BCD_NUM;              /* Index into output bcd_number */
   byte bcd_byte            = 0;
   boolean is_pending_shift = FALSE;                /* Flag to indicate whether to write at new index or shift and write at existing one*/

   uint8 number_type = 0;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(number     != NULL);
  CM_ASSERT(bcd_number != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Store the number type of string coming in, it may be changed later */
  number_type = number->number_type;

  /* Convert the ASCII number to the BCD and store it in buffer */
  for ( i = 0; i < number->len; i++ )
  {
     if ( bcd_index > bcd_size-1 )
     {
        break;
     }

     /*
    ** If international number, '+' in beginning of dialed string, ignore '+'
    * and set the number_type to INTERNATIONAL.
    */
    if (i == 0)
    {
      if (number->buf[0] == '+')
      {
        number_type = (uint8)CM_NUM_TYPE_INTERNATIONAL;
        continue;
      }
    }

     bcd_byte = ascii_char_to_bcd_byte( number->buf[i] );
     if ( bcd_byte == BCD_INVALID_BYTE )
     {
        continue;
     }

     /* Store the BCD bytes in bcd_number buffer */
     if ( !is_pending_shift )
     {
        bcd_number[bcd_index] = (bcd_byte);
        is_pending_shift = TRUE;
     }
     else
     {
        bcd_number[bcd_index] = ( ( bcd_byte << 4 ) | bcd_number[bcd_index] );
        bcd_index++;
        is_pending_shift = FALSE;
     }
  }

  /* Inserts a 0x0f at the end of the buffer if number is ODD length */
  if ( is_pending_shift )
  {
     bcd_number[bcd_index] = ( ( BCD_END_FLAG << 4 ) | bcd_number[bcd_index] );
     bcd_index++;
  }

  /* Add information about the Number plan and Number Type at index 1 */
  bcd_number[BCD_NT_NPI] = (byte) (0x80 | (number_type << 4) | number->number_plan);

  /* Add BCD number length at index 0 */
  bcd_number[BCD_LEN] = (byte) ( bcd_index - 1 );

  if ( bcd_index > bcd_size-1 )
  {
     return FALSE;
  }

  return TRUE;
}


/*===========================================================================

FUNCTION: cm_util_bcd_to_ascii

DESCRIPTION
  Convert the phone number from BCD to ASCII

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_bcd_to_ascii(const byte *bcd_number, byte *ascii_number)
{
  int bcd_index = 0;
  int ascii_index = 0;
  byte bcd_length;
  uint8 asc_1 = 0;
  uint8 asc_2 = 0;
  boolean presentation_indicator_absent = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(bcd_number   != NULL);
  CM_ASSERT(ascii_number != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  bcd_length = bcd_number[bcd_index++];

  /*****************************/
  /* International call or not */
  /*****************************/
  /*lint -save -e641*/
  if ((bcd_number[bcd_index] & CM_TON_MASK) >> 4 == CM_NUM_TYPE_INTERNATIONAL)
  {
     ascii_number[ascii_index++] = '+';
  }
  /*lint -restore */

  /* check if there is an extra byte or not (screening indicator,
  ** presentation_indicator)
  */

  presentation_indicator_absent =
     ((boolean)(bcd_number[bcd_index] & 0x80) >> 7);

  bcd_index++;

  /**************************/
  /* presentation_indicator */
  /**************************/

  if (presentation_indicator_absent == FALSE)
  {
     bcd_index++;
  }


  /*************************/
  /* Decode BCD into ascii */
  /*************************/

  for( ; bcd_index <= bcd_length; bcd_index++)
  {

     asc_1 = (bcd_number[bcd_index] & 0x0F);
     asc_2 = (bcd_number[bcd_index] & 0xF0) >> 4;

     ascii_number[ascii_index++] = (asc_1==BCD_STAR)? '*' :
                                    (asc_1==BCD_HASH)? '#' :
                                    INRANGE(asc_1, 0x0C, 0x0E)? (asc_1 - 0x0C) + 'a':
                                    asc_1 + '0';

     ascii_number[ascii_index++] = (asc_2==BCD_STAR)? '*' :
                                    (asc_2==BCD_HASH)? '#' :
                                    INRANGE(asc_2, 0x0C, 0x0E)? (asc_2 - 0x0C) + 'a':
                                    (asc_2==0x0F)? '\0' :
                                    asc_2 + '0';
  }

  /* Null terminate the ascii string */
  if (asc_2 != 0x0f)
  {
    ascii_number[ascii_index] = '\0';
  }
}

/*===========================================================================

FUNCTION: unpacked_bcd_byte_to_ascii_char

DESCRIPTION
  Convert an unpacked BCD byte to ASCII character.
 
  This function is used as a utility function for
  cm_util_bcd_num_to_ascii_num function. Any other
  invocations must appropriately handle the missing
  conditions.

DEPENDENCIES
  None.

RETURN VALUE
  ASCII Character

SIDE EFFECTS
  None

===========================================================================*/
static uint8 bcd_byte_to_ascii_char( uint8 ubcd )
{
   if ( ubcd == BCD_STAR )
   {
      return '*';
   }
   else if ( ubcd == BCD_HASH )
   {
      return '#';
   }
   else if ( INRANGE( ubcd, 0x0C, 0x0E ) )
   {
      return ( ( ubcd - 0x0C ) + 'a' );
   }
   else
   {
      return ( ubcd + '0' );
   }
}


/*===========================================================================

FUNCTION: cm_util_bcd_num_to_ascii_num

DESCRIPTION
  Convert the phone number from BCD to ASCII

DEPENDENCIES
  None.

RETURN VALUE
  Length of unpacked BCD string

SIDE EFFECTS
  None

===========================================================================*/
size_t cm_util_bcd_num_to_ascii_num( const byte *bcd_number, byte *ascii_number, size_t ascii_size )
{
  int bcd_index   = 0;
  size_t ascii_index = 0;
  size_t unpacked_bcd_num_length = 0;
  byte bcd_length;
  uint8 asc_1     = 0;
  uint8 asc_2     = 0;
  boolean presentation_indicator_absent = TRUE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( bcd_number   != NULL );
  CM_ASSERT( ascii_number != NULL );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  bcd_length = bcd_number[bcd_index++];

  /*****************************/
  /* International call or not */
  /*****************************/
  /*lint -save -e641*/
  if( ( bcd_number[bcd_index] & CM_TON_MASK ) >> 4 == CM_NUM_TYPE_INTERNATIONAL )
  {
     /* Write only if the ascii_num size is greater than 1.
     ** If ascii_size == 1, then function creates a NULL string*/
     unpacked_bcd_num_length++;
     if ( ascii_size > 1 )
     {
        ascii_number[ascii_index++] = '+';
     }
  }
  /*lint -restore */

  /* check if there is an extra byte or not (screening indicator,
  ** presentation_indicator)
  */

  presentation_indicator_absent =
     ((boolean)(bcd_number[bcd_index] & 0x80) >> 7);

  bcd_index++;

  /**************************/
  /* presentation_indicator */
  /**************************/

  if (presentation_indicator_absent == FALSE)
  {
     bcd_index++;
  }

  /*************************/
  /* Decode BCD into ascii */
  /*************************/

  for( ; bcd_index <= bcd_length; bcd_index++ )
  {
     asc_1 = ( bcd_number[bcd_index] & 0x0F );
     asc_2 = ( bcd_number[bcd_index] & 0xF0 ) >> 4;

     if ( ascii_index < ascii_size - 1 )
     {
        ascii_number[ascii_index++] = bcd_byte_to_ascii_char( asc_1 );
     }
     unpacked_bcd_num_length++;
     
     if ( asc_2 == 0x0F )
     {
        break;
     }

     if ( ascii_index < ascii_size - 1 )
     {
        ascii_number[ascii_index++] = bcd_byte_to_ascii_char( asc_2 );
     }
     unpacked_bcd_num_length++;
  }

  /***********************************/
  /* Null terminate the ascii string */
  /***********************************/
  ascii_number[ascii_index] = '\0';
 
  return unpacked_bcd_num_length;
}

/*===========================================================================

FUNCTION: cm_util_subaddr_to_ascii

DESCRIPTION
  Convert the subaddress to ASCII.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_subaddr_to_ascii(
    byte   *subaddress,
    byte   *ascii_subaddress,
    byte   length)
{
  byte *data_pointer;
  byte index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(subaddress != NULL);
  CM_ASSERT(ascii_subaddress != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  data_pointer = subaddress;

  data_pointer++;

  /*
  ** Look at the AFI (Authority and Format Identifier)
  */
  if (*data_pointer == 0x50)
  {
     /*
     ** The subaddress consists of ia5 characters
     */
     for (index = 0; index < (length - 2); index++)
     {
        ascii_subaddress[index] = subaddress[index + 2] + '0';
     }

     ascii_subaddress[index] = '\0';
  }
  else
  {
     ascii_subaddress[0] = '\0';
  }
}

/*===========================================================================

FUNCTION: cm_util_subaddr_to_ascii_num

DESCRIPTION
  Convert the subaddress to ASCII.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_subaddr_to_ascii_num(
    byte   *subaddress,
    byte    subadress_length,
    byte   *ascii_subaddress,
    byte    ascii_length
    )
{
  byte *data_pointer;
  byte index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(subaddress != NULL);
  CM_ASSERT(ascii_subaddress != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  data_pointer = subaddress;

  data_pointer++;

  /*
  ** Look at the AFI (Authority and Format Identifier)
  */
  if (*data_pointer == 0x50)
  {
     /*
     ** The subaddress consists of ia5 characters
     */
     for (index = 0; index < (subadress_length - 2)&& index < (ascii_length-1); index++)
     {
        ascii_subaddress[index] = subaddress[index + 2] + '0';
     }

     ascii_subaddress[index] = '\0';
  }
  else
  {
     ascii_subaddress[0] = '\0';
  }
}

/*===========================================================================

FUNCTION: cm_util_default_cdma_orig_params

DESCRIPTION
  Return a cm_cdma_orig_params_s_type  of default orig parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_default_cdma_orig_params(

  cm_cdma_orig_params_s_type           *cdma_orig_param_ptr
)
{
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  CM_ASSERT( cdma_orig_param_ptr != NULL );

  cdma_orig_param_ptr->srv_opt = 0;
  cdma_orig_param_ptr->activate_code = CM_OTASP_ACT_CODE_NONE;
  cdma_orig_param_ptr->drs_bit = TRUE;
  cdma_orig_param_ptr->sr_id_included = FALSE;
  cdma_orig_param_ptr->sr_id = 0;
  cdma_orig_param_ptr->qos_parms_incl = FALSE;
  cdma_orig_param_ptr->qos_parms_len = 0;
  cdma_orig_param_ptr->last_act_data_net = SYS_SYS_MODE_NO_SRV;

#else
  SYS_ARG_NOT_USED(cdma_orig_param_ptr);
  SYS_ARG_NOT_CONST(cdma_orig_param_ptr);

  CM_ERR_0("cm_util_default_cdma_orig_params shouldn't be called. ");

#endif  /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

}   /* cm_util_default_cdma_orig_params */

/*===========================================================================

FUNCTION: cm_util_default_gw_orig_params

DESCRIPTION
  Return a cm_cdma_orig_params_s_type  of default orig parameters

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_default_gw_orig_params(

  cm_gw_cs_orig_params_s_type                *gw_orig_param_ptr

)
{
  cm_util_default_gw_orig_params_per_sub(gw_orig_param_ptr, SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION: cm_util_default_gw_orig_params_per_sub

DESCRIPTION
  Return a cm_cdma_orig_params_s_type of default orig parameters based on asubs_id

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_default_gw_orig_params_per_sub(

  cm_gw_cs_orig_params_s_type                *gw_orig_param_ptr,

  sys_modem_as_id_e_type         asubs_id
)
{
#ifdef CM_GW_SUPPORTED

  cm_bearer_capability_s_type default_bearer_capability;

  CM_ASSERT( gw_orig_param_ptr != NULL );

  memset(gw_orig_param_ptr, 0, sizeof(cm_gw_cs_orig_params_s_type));

  cm_util_set_default_voice_bearer_cap_per_sub(&default_bearer_capability, asubs_id);

  gw_orig_param_ptr->bc_repeat_indicator.present = FALSE;
  gw_orig_param_ptr->bc_repeat_indicator.repeat_indication = 0;
  gw_orig_param_ptr->bearer_capability_1 = default_bearer_capability;
  gw_orig_param_ptr->bearer_capability_2.present = FALSE;
  gw_orig_param_ptr->llc_repeat_indicator.present = FALSE;
  gw_orig_param_ptr->llc_repeat_indicator.repeat_indication = 0;
  gw_orig_param_ptr->low_layer_compatibility_1.present = FALSE;
  gw_orig_param_ptr->low_layer_compatibility_2.present = FALSE;
  gw_orig_param_ptr->hlc_repeat_indicator.present = FALSE;
  gw_orig_param_ptr->hlc_repeat_indicator.repeat_indication = 0;
  gw_orig_param_ptr->high_layer_compatibility_1.present = FALSE;
  gw_orig_param_ptr->high_layer_compatibility_1.not_applicable = TRUE;
  gw_orig_param_ptr->high_layer_compatibility_2.present = FALSE;
  gw_orig_param_ptr->high_layer_compatibility_2.not_applicable = TRUE;
  gw_orig_param_ptr->called_party_bcd_number.present = FALSE;
  gw_orig_param_ptr->called_party_subaddress.present = FALSE;
  gw_orig_param_ptr->calling_party_subaddress.present = FALSE;
  gw_orig_param_ptr->forward_cug_info.present = FALSE;
  gw_orig_param_ptr->cm_clir = 0;

  /* Print default_bearer_capability */
  CM_MSG_HIGH_6 ( "present - %d, r_c_r - %d, c_s - %d tr_mode - %d, i_t_c - %d, ctm - %d",
                  default_bearer_capability.present,
                  default_bearer_capability.radio_channel_requirement,
                  default_bearer_capability.coding_standard ,
                  default_bearer_capability.transfer_mode,
                  default_bearer_capability.information_transfer_capability,
                  default_bearer_capability.ctm );
  
  CM_MSG_HIGH_6 ( "gsm_sp_ver_pref_0 %d, gsm_sp_ver_pref_1 %d, gsm_sp_ver_pref_2 %d gsm_sp_ver_pref_3 %d, gsm_sp_ver_pref_4 %d, gsm_sp_ver_pref_5 %d",
                  default_bearer_capability.gsm_speech_ver_pref_0,
                  default_bearer_capability.gsm_speech_ver_pref_1,
                  default_bearer_capability.gsm_speech_ver_pref_2 ,
                  default_bearer_capability.gsm_speech_ver_pref_3,
                  default_bearer_capability.gsm_speech_ver_pref_4,
                  default_bearer_capability.gsm_speech_ver_pref_5 );


#else
  SYS_ARG_NOT_USED(gw_orig_param_ptr);
  SYS_ARG_NOT_CONST(gw_orig_param_ptr);

  CM_ERR_0("cm_util_default_gw_orig_params shouldn't be called. ");

#endif  /* FEATURE_GSM || FEATURE_WCDMA */

}   /* cm_util_default_gw_orig_params */

/*===========================================================================

FUNCTION: cm_util_set_default_voice_bearer_cap

DESCRIPTION
  This function sets the bearer capability to default value indicating Voice call.

DEPENDENCIES
  None.

RETURN VALUE
  Default bearer capability.

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_set_default_voice_bearer_cap(

  cm_bearer_capability_s_type    *bc

)
{
  cm_util_set_default_voice_bearer_cap_per_sub(bc, SYS_MODEM_AS_ID_1);
}

/*===========================================================================

FUNCTION: cm_util_set_default_voice_bearer_cap_per_sub

DESCRIPTION
  This function sets the bearer capability to default value indicating Voice call.

DEPENDENCIES
  None.

RETURN VALUE
  Default bearer capability.

SIDE EFFECTS
  None

===========================================================================*/
void cm_util_set_default_voice_bearer_cap_per_sub(

  cm_bearer_capability_s_type    *bc,

  sys_modem_as_id_e_type          asubs_id

)
{
  #ifdef CM_GW_SUPPORTED
  cmph_sub_config_s_type *ph_sub_config_ptr = cmph_get_sub_config_ptr(asubs_id);
  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer[%d] is NULL",asubs_id);
    return;
  }
   CM_ASSERT(bc != NULL);

   /*--------------------------------------------------------------------*/

   memset (bc, 0, sizeof(cm_bearer_capability_s_type));

   /*
   **Initialize all of the bearer capability values for Voice Call.
   */

   /*
   ** indicate bc is present.
   */
   bc->present                                  = TRUE;

   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 3
   */

   /* CTM text telephony indication, Rel 5, 24.008 10.5.4.5
   */
   bc->ctm                                      = CTM_NOT_SUPPORTED;


 #ifdef FEATURE_GSM_HALF_RATE
   bc->radio_channel_requirement                = DUAL_RATE_FULL_RATE_PREFERRED;
 #else
   bc->radio_channel_requirement                = FULL_RATE_ONLY;
 #endif
   bc->coding_standard                          = GSM;
   bc->transfer_mode                            = CIRCUIT;
   bc->information_transfer_capability          = ITC_SPEECH;

   /*-------------------------------------------------------------------------*/

   /*
   ** Populate Octet 3a, 3b, 3c
   */

   /*  support for 3 speech versions: EFR, FR and AMR  */
   if (ph_sub_config_ptr->is_gsm_amr_call_config_on)
   {

     #ifdef FEATURE_GSM_AMR

     #ifdef FEATURE_GSM_AMR_WB
       /* octet 3a */
     bc->gsm_speech_ver_pref_0                    = GSM_FR_SPEECH_VER_5;  /* GSM AMR -WB Full Rate */

     /* octet 3a */
     bc->gsm_speech_ver_pref_1                    = GSM_FR_SPEECH_VER_3;  /* GSM AMR Full Rate */

     /* octet 3b */
     bc->gsm_speech_ver_pref_2                    = GSM_FR_SPEECH_VER_2;  /* GSM Enhanced Full Rate  */

     /* octet 3c */
     bc->gsm_speech_ver_pref_3                    = GSM_FR_SPEECH_VER_1;  /* GSM FR */

     #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
     /* octet 3d */
     bc->gsm_speech_ver_pref_4                    = GSM_HR_SPEECH_VER_3;

     /* octet 3e */
     bc->gsm_speech_ver_pref_5                    = GSM_HR_SPEECH_VER_1;

       #else
    /* octet 3d */
       bc->gsm_speech_ver_pref_4                    = GSM_INVALID_SPEECH_VER;

       /* octet 3e */
       bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;

     #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */


     #else /* FEATURE_GSM_AMR_WB */

       bc->gsm_speech_ver_pref_0                    = GSM_INVALID_SPEECH_VER;  /* GSM AMR -WB Full Rate */

       /* octet 3a */
       bc->gsm_speech_ver_pref_1                    = GSM_FR_SPEECH_VER_3;  /* GSM AMR Full Rate */

       /* octet 3b */
       bc->gsm_speech_ver_pref_2                    = GSM_FR_SPEECH_VER_2;  /* GSM Enhanced Full Rate  */

       /* octet 3c */
       bc->gsm_speech_ver_pref_3                    = GSM_FR_SPEECH_VER_1;  /* GSM FR */

      #ifdef FEATURE_GSM_EXT_SPEECH_PREF_LIST
       bc->gsm_speech_ver_pref_4                    = GSM_HR_SPEECH_VER_3;
       #ifdef FEATURE_GSM_HALF_RATE
       /* octet 3d */
       bc->gsm_speech_ver_pref_5                    = GSM_HR_SPEECH_VER_1;
       #else
       bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;
       #endif

      #else
    /* octet 3d */
      bc->gsm_speech_ver_pref_4                    = GSM_INVALID_SPEECH_VER;

      /* octet 3e */
      bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;

      #endif /* FEATURE_GSM_EXT_SPEECH_PREF_LIST */
    #endif /* FEATURE_GSM_AMR_WB */

    #else /* FEATURE_GSM_AMR */

     #ifdef FEATURE_GSM_AMR_WB
     bc->gsm_speech_ver_pref_0                    = GSM_FR_SPEECH_VER_2;  
     bc->gsm_speech_ver_pref_1                    = GSM_FR_SPEECH_VER_1;
     /* octet 3c */
     #ifdef FEATURE_GSM_HALF_RATE
     bc->gsm_speech_ver_pref_2                    = GSM_HR_SPEECH_VER_1; /* GSM Half Rate Version 0 or 1 */
     #else
     bc->gsm_speech_ver_pref_2                    = GSM_INVALID_SPEECH_VER; 
     #endif
     bc->gsm_speech_ver_pref_3                    = GSM_HR_SPEECH_VER_1; /* GSM Half Rate Version 0 or 1 */
     bc->gsm_speech_ver_pref_4                    = GSM_INVALID_SPEECH_VER;
     bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;

     #else /* FEATURE_GSM_AMR_WB */
	 bc->gsm_speech_ver_pref_0                    = GSM_INVALID_SPEECH_VER ;  
        
     /* octet 3a */
     bc->gsm_speech_ver_pref_1                    = GSM_FR_SPEECH_VER_2;
     /* octet 3c */
     bc->gsm_speech_ver_pref_2                    = GSM_FR_SPEECH_VER_1; 

     #ifdef FEATURE_GSM_HALF_RATE
     bc->gsm_speech_ver_pref_3                    = GSM_HR_SPEECH_VER_1; /* GSM Half Rate Version 0 or 1 */
     #else
     bc->gsm_speech_ver_pref_3                    = GSM_INVALID_SPEECH_VER;
     #endif


     bc->gsm_speech_ver_pref_4                    = GSM_INVALID_SPEECH_VER;
     bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;

     #endif /* FEATURE_GSM_AMR_WB */
    #endif /* FEATURE_GSM_AMR */

   }
   else
   {
     #ifdef FEATURE_GSM_AMR_WB
     /* Non - AMR Speech preference settings */
     /* octet 3a */
     bc->gsm_speech_ver_pref_0                    = GSM_FR_SPEECH_VER_2;  

     /* octet 3a */
     bc->gsm_speech_ver_pref_1                    = GSM_FR_SPEECH_VER_1;

     #ifdef FEATURE_GSM_HALF_RATE
     bc->gsm_speech_ver_pref_2                    = GSM_HR_SPEECH_VER_1; /* GSM Half Rate Version 0 or 1 */
     #else
     bc->gsm_speech_ver_pref_2                    = GSM_INVALID_SPEECH_VER;
     #endif

     /* octet 3c */
     bc->gsm_speech_ver_pref_3                    = GSM_INVALID_SPEECH_VER;
     /* octet 3d */
     bc->gsm_speech_ver_pref_4                    = GSM_INVALID_SPEECH_VER;
     /* octet 3e */
     bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;

     #else
     /* Non - AMR Speech preference settings */
     /* octet 3a */
     bc->gsm_speech_ver_pref_0                    = GSM_INVALID_SPEECH_VER ;  

     /* octet 3b */
     bc->gsm_speech_ver_pref_1                    = GSM_FR_SPEECH_VER_2;

     bc->gsm_speech_ver_pref_2                    = GSM_FR_SPEECH_VER_1; /* GSM Half Rate Version 0 or 1 */
     /* octet 3c */
     #ifdef FEATURE_GSM_HALF_RATE
     bc->gsm_speech_ver_pref_3                    = GSM_HR_SPEECH_VER_1; /* GSM Half Rate Version 0 or 1 */
     #else
     bc->gsm_speech_ver_pref_3                    = GSM_INVALID_SPEECH_VER;
     #endif /* FEATURE_GSM_HALF_RATE */
     bc->gsm_speech_ver_pref_4                    = GSM_INVALID_SPEECH_VER;

     /* octet 3d */
     bc->gsm_speech_ver_pref_5                    = GSM_INVALID_SPEECH_VER;
     #endif

   }

   CM_MSG_HIGH_1 ("NV defined : %d",ph_sub_config_ptr->is_gsm_amr_call_config_on);

   CM_MSG_HIGH_6 ("BC speech codec values: ver_0 %d ver_1 %d ver_2 %d ver_3 %d ver_4 %d ver_5 %d",
                 bc->gsm_speech_ver_pref_0, 
                 bc->gsm_speech_ver_pref_1,
                 bc->gsm_speech_ver_pref_2,
                 bc->gsm_speech_ver_pref_3, 
                 bc->gsm_speech_ver_pref_4,
                  bc->gsm_speech_ver_pref_5 );


   /*-------------------------------------------------------------------------*/

   /*
   ** Populate Octet 4
   */

   bc->compression                              = 0x00;
   bc->structure                                = 0x00;
   bc->duplex_mode                              = 0x01;
   bc->configuration                            = 0x00;
   bc->NIRR                                     = 0x00;
   bc->establishment                            = 0x00;

   /*-------------------------------------------------------------------------*/

   /*
   ** Populate Octet 5
   */
   bc->access_id                                = 0x00;
   bc->rate_adaption                            = 0x00;
   bc->signalling_access_protocol               = 0x00;

   /*-------------------------------------------------------------------------*/

   /*
   ** NOTE : Not required to populate Octet 5a.
   */
   bc->other_ITC                                = 0x00;
   bc->other_rate_adaption                      = 0x00;

   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 5b
   */
   bc->rate_adaption_header                     = 0x00;
   bc->multiple_frame_establishment             = 0x00;
   bc->mode_of_operation                        = 0x00;
   bc->logical_link_identifier_negotiation      = 0x00;
   bc->assignor_assignee                        = 0x00;
   bc->inband_outband_negotiation               = 0x00;

   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6
   */

   bc->layer_1_id                               = 0x01;  /* All other values reserved */
   bc->user_information_layer_1_protocol        = 0x00;
   bc->sync_async                               = 0x00;

   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6a
   */
   bc->number_of_stop_bits                      = 0x00;
   bc->negotiation                              = 0x00;
   bc->number_of_data_bits                      = 0x00;
   bc->user_rate                                = 0x00;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6b
   */
   bc->intermediate_rate                        = 0x00;
   bc->NIC_on_Tx                                = 0x00;
   bc->NIC_on_Rx                                = 0x00;
   bc->parity                                   = 0x00;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6c
   */
   bc->connection_element                       = 0x00;
   bc->modem_type                               = 0x00;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6d
   */
   bc->other_modem_type                         = OMT_UNDEFINED;
   bc->fixed_network_user_rate                  = FNUR_UNDEFINED;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6e
   ** NOTE : Meaningful only for DATA Calls.
   */
   bc->acceptable_channel_codings               = ACC_NONE;
   bc->maximum_number_of_trafic_channels        = MAX_NUMBER_OF_TCH_UNDEFINED;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6f
   */
   bc->user_initiated_modification_indication   = UIMI_UNDEFINED;
   bc->wanted_air_interface_user_rate           = WAIUR_UNDEFINED;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 6g
   */
   bc->acceptable_channel_coding_extended       = 0x00;
   bc->asymmetry_indication                     = 0x00;
   /*-------------------------------------------------------------------------*/
   /*
   ** Populate Octet 7
   */

   /* These are used for Phase2+ (i.e.14400bd) */
   bc->layer_2_id                               = 0x02;
   bc->user_information_layer_2_protocol        = 0x00;

   /*
   ** For logging purpose.
   */
   bc->mt_fall_back_rate                        = 0x00;

   #else
   SYS_ARG_NOT_USED(bc);
   SYS_ARG_NOT_CONST(bc);
   CM_ERR_0("cm_util_set_default_voice_bearer_cap bc shouldn't be called. ");

   #endif /* defined(FEATURE_WCDMA) || defined (FEATURE_GSM) */
}/* cm_set_default_bearer_cap */



#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

/*===========================================================================
Handset based plus code dialing
===========================================================================*/
/* Structure to hold sid conflict and resolution with LTM offset table
*/
struct sid_conflict_resolution_record_s
{
   uint16 sid;
   uint16 mcc1; /* Country code for country1 */
   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is OFF. For ex- USA falls within
   ** range of -10 to -5 hrs from UTC when daylt savings is OFF,
   ** so min value is -20 and max value is -10
   */
   int8  mcc1_min_ltm_no_dts;
   int8  mcc1_max_ltm_no_dts;

   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is ON. For ex- USA falls within
   ** range of -10 to -4 hrs from UTC when daylt savings is ON,
   ** so min value is -20 and max value is -8
   */
   int8  mcc1_min_ltm_with_dts;
   int8  mcc1_max_ltm_with_dts;
   uint16 mcc2; /* Country code for country 2 */
   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is OFF. For ex- USA falls within
   ** range of -10 to -5 hrs from UTC when daylt savings is OFF,
   ** so min value is -20 and max value is -10
   */
   int8  mcc2_min_ltm_no_dts;
   int8  mcc2_max_ltm_no_dts;

   /* offset of local time from UTC in units of 30 minutes
   ** when daylight savings is ON. For ex- USA falls within
   ** range of -10 to -4 hrs from UTC when daylt savings is ON,
   ** so min value is -20 and max value is -8
   */
   int8  mcc2_min_ltm_with_dts;
   int8  mcc2_max_ltm_with_dts;
};

/* Declarations for Static table and sid conflict table
**
** -- NEED TO AVOID ADDING duplicate country entry for sid conflicts
**
** -- NEED TO AVOID ADDING duplicate country entry for multiple MCCs
*/
static const struct country_specific_record_s   country_list [] =
{

/*  Sid range    cc     idd     spc    ndd      Network solution      MCC*/
{ 14976, 15103,   93,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  412 }, /* Afghanistan 14976*/
{  8288,  8319,  213,    "00",   "",   "7", NETWORK_SOLUTION_ABSENT,  603 }, /* Algeria 8288*/
{  4100,  4100,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  544 }, /* American Samoa 4100*/
{  9504,  9535,  244,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  631 }, /* Angola 9504*/
{ 32128, 32255,   54,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  722 }, /* Argentina 32128*/
{ 15938, 15945,  374,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  283 }, /* Armenia 15938*/
{  9648,  9663,  297,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  363 }, /* Aruba 9648*/
{  7680,  7807,   61,  "0011",   "",   "0", NETWORK_SOLUTION_ABSENT,  505 }, /* Australia 7680*/
{  8320,  8447,   61,  "0011",   "",   "0", NETWORK_SOLUTION_ABSENT,  505 }, /* Australia 8320*/
{ 22912, 23039,   43,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  232 }, /* Austria 22912*/
{ 15946, 15953,  994,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  400 }, /* Azerbaijan 15946*/
{  8080,  8095,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  364 }, /* Bahamas 8080 */
{ 13472, 13503,  880,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  470 }, /* Bangladesh 13472*/
{  8160,  8175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  342 }, /* Barbados 8160 */
{ 15954, 15961,  375,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  257 }, /* Belarus 15954 */
{ 32640, 32649,  501,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  702 }, /* Belize 32640*/
{  9088,  9119,  229,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  616 }, /* Benin 9088 */
{  8096,  8111,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  350 }, /* Bermuda 8096 */
{ 10176, 10207,  267,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  652 }, /* Botswana 10176 */
{ 31872, 32127,   55,  "0015",   "",   "0", NETWORK_SOLUTION_ABSENT,  724 }, /* Brazil 31872*/
{  8112,  8127,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  348 }, /* British Virgin Islands 8112 */
{ 15584, 15615,  359,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  284 }, /* Bulgaria 15584 */
{  9952,  9983,  257,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  642 }, /* Burundi 9952*/
{ 11104, 11135,  855,   "001",   "",   "0", NETWORK_SOLUTION_ABSENT,  456 }, /* Cambodia 11104*/
{  9344,  9375,  237,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  624 }, /* Cameroon 9344*/
{ 16384, 18431,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  302 }, /* Canada 16384*/
{  8128,  8143,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  346 }, /* Cayman Islands 8128*/
{ 31744, 31754,   56,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  730 }, /* Chile 31744*/
{ 31809, 31820,   56,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  730 }, /* Chile 31809*/
{ 31841, 31854,   56,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  730 }, /* Chile 31841*/
{ 13568, 14335,   86,    "00", "13",   "0", NETWORK_SOLUTION_ABSENT,  460 }, /* China 13568*/
{ 25600, 26111,   86,    "00", "13",   "0", NETWORK_SOLUTION_ABSENT,  460 }, /* China 25600*/
{ 13568, 14335,   86,    "00", "15",   "0", NETWORK_SOLUTION_ABSENT,  460 }, /* China 13568*/
{ 25600, 26111,   86,    "00", "15",   "0", NETWORK_SOLUTION_ABSENT,  460 }, /* China 25600*/
{ 13568, 14335,   86,    "00", "18",   "0", NETWORK_SOLUTION_ABSENT,  460 }, /* China 13568*/
{ 25600, 26111,   86,    "00", "18",   "0", NETWORK_SOLUTION_ABSENT,  460 }, /* China 25600*/
{ 31616, 31743,   57,   "009",   "",  "03", NETWORK_SOLUTION_ABSENT,  732 }, /* Colombia 31616*/
{  8960,  8991,  225,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  612 }, /* Cote D'Ivore/Ivory coast 8960*/
{ 22300, 22300,  875,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  901 }, /* Cruise Ship-MCP*/
{ 24416, 24447,  875,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  274 }, /* Cruise Ship-On Waves : 24416*/
{ 16128, 16255,  420,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  230 }, /* Czech republic : 16128*/
{ 22528, 22543,   45,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  238 }, /* Demark : 22528*/
{  8832,  8847,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  370 }, /* Dominica Republic :8832*/
{ 31296, 31327,  593,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  740 }, /* Ecuador 31296*/
{  8224,  8255,   20,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  602 }, /* Egypt : 8224*/
{ 32704, 32735,  503,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  706 }, /* El Salvador*/
{ 10872, 10879,  372,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  248 }, /* Estonia : 10872*/
{  9760,  9791,  251,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  636 }, /* Ethiopia : 9760*/
{ 10960, 10975,  679,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  542 }, /* Fiji : 10960*/
{ 24448, 24451,  358,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  244 }, /* Finland : 24448*/
{  8544,  8575,  220,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  607 }, /* Gambia : 8544*/
{ 15962, 15969,  995,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  282 }, /* Georgia : 15962*/
{  9216,  9247,  233,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  620 }, /* Ghana : 9216*/
{  9696,  9711,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  535 }, /* Guam : 9696*/
{ 32672, 32703,  502,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  704 }, /* Guatemala 32672*/
{ 32608, 32639,  509,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  372 }, /* Haiti 32608*/
{ 32736, 32767,  504,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  708 }, /* Honduras 32736*/
{ 10640, 10655,  852,   "001",   "",    "", NETWORK_SOLUTION_ABSENT,  454 }, /* Hong Kong*/
{ 24416, 24447,  354,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  274 }, /* Iceland : 24416*/
{ 14464, 14847,   91,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  404 }, /* India 14464*/
{ 10496, 10623,   62,   "001",   "",   "0", NETWORK_SOLUTION_ABSENT,  510 }, /* Indonesia 10496*/
{ 15456, 15487,  964,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  418 }, /* Iraq : 15456*/
{ 24384, 24387,  353,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  272 }, /* Ireland : 24384*/
{  8448,  8479,  972,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  425 }, /* Israel 8448*/
{  8176,  8191,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  338 }, /* Jamaica : 8176*/
{ 12288, 13311,   81,   "010",   "",   "0", NETWORK_SOLUTION_ABSENT,  440 }, /* Japan 12288*/
{  8928,  8943,    7,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  401 }, /* Kazakhstan 8928*/
{  9856,  9887,  254,   "000",   "",   "0", NETWORK_SOLUTION_ABSENT,  639 }, /* Kenya : 9856*/
{  2176,  2303,   82,   "001",   "",   "0", NETWORK_SOLUTION_ABSENT,  450 }, /* Korea (South)*/
{ 11312, 11327,  965,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  419 }, /* Kuwait : 11312*/
{ 21550, 21566,  996,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  437 }, /* Kyrgyz Republic*/
{ 13440, 13471,  856,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  457 }, /* Laos : 13440*/
{ 10784, 10785,  371,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  247 }, /* Latvia 10784*/
{ 11296, 11311,  853,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  455 }, /* Macao/Macau : 11296*/
{ 10048, 10079,  261,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  646 }, /* Madagascar : 10048*/
{ 10368, 10495,   60,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  502 }, /* Malaysia 10368*/
{  8768,  8799,  223,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  610 }, /* Mail Republic : 8768*/
{  8736,  8767,  222,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  609 }, /* Mauritania : 8736*/
{  9120,  9151,  230,   "020",   "",   "0", NETWORK_SOLUTION_ABSENT,  617 }, /* Mauritius : 9120*/
{ 24576, 25075,   52,    "00",   "",  "01", NETWORK_SOLUTION_ABSENT,  334 }, /* Mexico 24576*/
{ 25100, 25124,   52,    "00",   "",  "01", NETWORK_SOLUTION_ABSENT,  334 }, /* Mexico 25100*/
{ 15922, 15937,  373,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  259 }, /* Moldova : 15922*/
{ 15520, 15551,  976,   "001",   "",   "0", NETWORK_SOLUTION_ABSENT,  428 }, /* Mongolia 15520*/
{  8256,  8287,  212,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  604 }, /* Morocco 8256*/
{  9984, 10015,  258,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  634 }, /* Mozambique 9984*/
{ 15232, 15359,   95,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  414 }, /* Myanmar (Burma)*/
{ 10112, 10143,  264,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  649 }, /* Namibia */
{ 15552, 15583,  977,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  429 }, /* Nepal 15552*/
{ 31392, 31407,  599,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  362 }, /* Netherlands Antilles*/
{  8576,  8703,   64,    "00",   "",   "0", NETWORK_SOLUTION_PRESENT, 530 }, /* New Zealand*/
{ 32512, 32543,  505,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  710 }, /* Nicaragua 32512*/
{  9024,  9055,  227,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  614 }, /* Niger 9024*/
{  9248,  9279,  234,   "009",   "",   "0", NETWORK_SOLUTION_ABSENT,  621 }, /* Nigeria 9248*/
{  9680,  9695,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  534 }, /* Northern Marianas*/
{ 22272, 22399,   47,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  242 }, /* Norway 22272*/
{ 11344, 11359,  968,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  422 }, /* Oman 11344*/
{ 14848, 14975,   92,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  410 }, /* Pakistan 14848*/
{ 32576, 32607,  507,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  714 }, /* Panama 32576*/
{ 32384, 32511,   51,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  716 }, /* Peru 32384*/
{ 10624, 10639,   63,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  515 }, /* Philippines 10624*/
{ 10656, 10751,   63,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  515 }, /* Philippines 10656*/
{ 16256, 16383,   48,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  260 }, /* Poland 16256 */
{ 24320, 24351,  351,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  268 }, /* Portugal 24320*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  330 }, /* Puerto Rico: 1 */
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  330 }, /* Puerto Rico: 1 */
{ 16000, 16127,   40,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  226 }, /* Romania 16000*/
{ 11392, 11554,    7,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  250 }, /* Russia 11392*/
{ 11556, 11599,    7,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  250 }, /* Russia 11556*/
{ 11601, 12287,    7,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  250 }, /* Russia 11601*/
{  9728,  9759,  250,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  635 }, /* Rwanda : 9728*/
{ 15488, 15519,  966,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  420 }, /* Saudi Arabia*/
{  8704,  8735,  221,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  608 }, /* Senegal 8704*/
{  9184,  9215,  232,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  619 }, /* Sierra Leone*/
{ 10240, 10367,   27,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  655 }, /* South Africa*/
{ 15104, 15231,   94,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  413 }, /* Sri Lanka*/
{  8032,  8047,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  356 }, /* St. Kitts/Nevis*/
{  8010,  8011,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  358 }, /* St. Lucia 8010*/
{  8016,  8031,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  358 }, /* St. Lucia 8016*/
{  9568,  9599,  249,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  634 }, /* Sudan*/
{ 31136, 31167,  597,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  746 }, /* Suriname*/
{ 22400, 22403,   46,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  240 }, /* Sweden : 22400*/
{ 13504, 13535,  886,   "002",   "",    "", NETWORK_SOLUTION_ABSENT,  466 }, /* Taiwan 13504*/
{ 10856, 10871,  992,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  436 }, /* Tajikistan 10856*/
{  9888,  9919,  255,   "000",   "",   "0", NETWORK_SOLUTION_ABSENT,  640 }, /* Tanzania 9888*/
{  8192,  8223,   66,   "001",   "",   "0", NETWORK_SOLUTION_ABSENT,  520 }, /* Thailand 8192*/
{  9056,  9087,  228,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  615 }, /* Togolese Republic /Togo*/
{  9824,  9855,   1,    "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  374 }, /* Trinidad*/
{ 10800, 10815,   1,    "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  376 }, /* Turk and Caicos*/
{ 15970, 15977,  993,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  438 }, /* Turkmenistan: 15970*/
{  9920,  9951,  256,   "000",   "",   "0", NETWORK_SOLUTION_ABSENT,  641 }, /* Uganda 9920*/
{ 15906, 15921,  380,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  255 }, /* Ukraine : 15906*/
{ 31168, 31199,  598,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  748 }, /* Uruguay 31168*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  332 }, /* USA Virgin Islands :1*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  332 }, /* USA Virgin Islands : 2304*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  310 }, /* USA*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  311 }, /* USA*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  312 }, /* USA*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  313 }, /* USA*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  314 }, /* USA*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  315 }, /* USA*/
{     1,  2175,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  316 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  310 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  311 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  312 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  313 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  314 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  315 }, /* USA*/
{  2304,  7679,    1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  316 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  310 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  311 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  312 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  313 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  314 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  315 }, /* USA*/
{  21567, 21630,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  316 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  310 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  311 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  312 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  313 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  314 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  315 }, /* USA*/
{  22404, 22527,   1,   "011",   "",   "1", NETWORK_SOLUTION_ABSENT,  316 }, /* USA*/
{ 10832, 10855,  998,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  434 }, /* Uzbekistan 10832*/
{ 11555, 11555,  998,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  434 }, /* Uzbekistan 11555*/
{ 11600, 11600,  998,   "810",   "",   "8", NETWORK_SOLUTION_ABSENT,  434 }, /* Uzbekistan 11600*/
{ 31488, 31615,   58,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  734 }, /* Venezuela 31488*/
{ 13312, 13439,   84,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  452 }, /* Vietnam 13312*/
{ 11328, 11343,  967,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  421 }, /* Yemen 11328*/
{ 11360, 11375,  967,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  421 }, /* Yemen 11360*/
{  9472,  9503,  243,    "00",   "",    "", NETWORK_SOLUTION_ABSENT,  630 }, /* Zaire/Democratic republic of Congo*/
{ 10016, 10047,  260,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  645 }, /* Zambia 10016*/
{ 10080, 10111,  263,    "00",   "",   "0", NETWORK_SOLUTION_ABSENT,  648 }  /* Zimbabwe 10016*/

};


static const struct sid_conflict_resolution_record_s       sid_cnflt_res_list [] =
{

{     1, 310, -20, -10,         -16,          -8, 404,  11,  11, LTM_INVALID, LTM_INVALID }, /* 1 USA India */
{     2, 310, -20, -10,         -16,          -8, 404,  11,  11, LTM_INVALID, LTM_INVALID }, /* 2 USA India */
{     3, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 3 USA Brazil */
{     7, 310, -20, -10,         -16,          -8, 404,  11,  11, LTM_INVALID, LTM_INVALID }, /* 7 USA India */
{    13, 310, -20, -10,         -16,          -8, 454,  16,  16, LTM_INVALID, LTM_INVALID }, /* 13 USA Hong Kong */
{    23, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 23 USA Brazil */
{    27, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 27 USA Brazil */
{    29, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 29 USA Brazil */
{    33, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 33 USA Brazil */
{    43, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 43 USA Brazil */
{    47, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 47 USA Brazil */
{    61, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 61 USA Brazil */
{    67, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 67 USA Brazil */
{    71, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 71 USA Brazil */
{   111, 310, -20, -10,         -16,          -8, 621,   2,   2, LTM_INVALID, LTM_INVALID }, /* 111 USA Nigeria */
{   131, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 131 USA Brazil */
{   136, 310, -20, -10,         -16,          -8, 362,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 136 USA Netherlands Antilles */
{   211, 310, -20, -10,         -16,          -8, 621,   2,   2, LTM_INVALID, LTM_INVALID }, /* 211 USA Nigeria */
{   257, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 257 USA Brazil */
{   259, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 259 USA Brazil */
{   261, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 261 USA Brazil */
{   263, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 263 USA Brazil */
{   311, 310, -20, -10,         -16,          -8, 621,   2,   2, LTM_INVALID, LTM_INVALID }, /* 311 USA Nigeria */
{   320, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 320 USA Brazil */
{   322, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 322 USA Brazil */
{   326, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 326 USA Brazil */
{   387, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 387 USA Brazil */
{   389, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 389 USA Brazil */
{   391, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 391 USA Brazil */
{   395, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 395 USA Brazil */
{   576, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 576 USA Brazil */
{   582, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 582 USA Brazil */
{   700, 310, -20, -10,         -16,          -8, 346, -10, -10, LTM_INVALID, LTM_INVALID }, /* 700 USA Cayman */
{   703, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 703 USA Brazil */
{   739, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 739 USA Brazil */
{   741, 310, -20, -10,         -16,          -8, 740, -10, -10, LTM_INVALID, LTM_INVALID }, /* 741 USA Ecuador */
{  1111, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 1111 USA Korea(South) */
{  1112, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 1112 USA Korea(South) */
{  1113, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 1113 USA Korea(South) */
{  1218, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1218 USA Brazil */
{  1222, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1222 USA Brazil */
{  1235, 310, -20, -10,         -16,          -8, 515,  16,  16, LTM_INVALID, LTM_INVALID }, /* 1235 USA Philppines */
{  1282, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1282 USA Brazil */
{  1313, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1313 USA Brazil */
{  1315, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1315 USA Brazil */
{  1329, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1329 USA Brazil */
{  1409, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1409 USA Brazil */
{  1443, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1443 USA Brazil */
{  1521, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1521 USA Brazil */
{  1525, 310, -20, -10,         -16,          -8, 334, -16, -12,         -14,         -10 }, /* 1525 USA Mexico */
{  1569, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1569 USA Brazil */
{  1581, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1581 USA Brazil */
{  1634, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1634 USA Brazil */
{  1666, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1666 USA Brazil */
{  1668, 310, -20, -10,         -16,          -8, 724,  -8,  -4,          -8,          -4 }, /* 1668 USA Brazil */
{  1700, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 1700 USA Korea(South) */
{  1901, 310, -20, -10,         -16,          -8, 250,   4,   6,           6,           8 }, /* 1901 USA Russia */
{  2177, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2177 USA Korea(South) */
{  2179, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2179 USA Korea(South) */
{  2181, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2181 USA Korea(South) */
{  2183, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2183 USA Korea(South) */
{  2185, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2185 USA Korea(South) */
{  2187, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2187 USA Korea(South) */
{  2189, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2189 USA Korea(South) */
{  2191, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2191 USA Korea(South) */
{  2193, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2193 USA Korea(South) */
{  2195, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2195 USA Korea(South) */
{  2197, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2197 USA Korea(South) */
{  2199, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2199 USA Korea(South) */
{  2201, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2201 USA Korea(South) */
{  2203, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2203 USA Korea(South) */
{  2205, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2205 USA Korea(South) */
{  2207, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2207 USA Korea(South) */
{  2209, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2209 USA Korea(South) */
{  2211, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2211 USA Korea(South) */
{  2213, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2213 USA Korea(South) */
{  2215, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2215 USA Korea(South) */
{  2217, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2217 USA Korea(South) */
{  2219, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2219 USA Korea(South) */
{  2221, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2221 USA Korea(South) */
{  2222, 450,  18,  18, LTM_INVALID, LTM_INVALID, 404,  11,  11, LTM_INVALID, LTM_INVALID }, /* 2222 Korea(South) India */
{  2223, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2223 USA Korea(South) */
{  2225, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2225 USA Korea(South) */
{  2227, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2227 USA Korea(South) */
{  2229, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2229 USA Korea(South) */
{  2231, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2231 USA Korea(South) */
{  2233, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2233 USA Korea(South) */
{  2235, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2235 USA Korea(South) */
{  2237, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2237 USA Korea(South) */
{  2239, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2239 USA Korea(South) */
{  2241, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2241 USA Korea(South) */
{  2243, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2243 USA Korea(South) */
{  2301, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2301 USA Korea(South) */
{  2303, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2301 USA Korea(South) */
{  4100, 544, -22, -22,         -22,         -22, 734,  -9,  -9,          -9,          -9 }, /* 4100 America Samoa Venezuela */
{  4101, 310, -12, -12,         -10,         -10, 734,  -9,  -9, LTM_INVALID, LTM_INVALID }, /* 4101 USA Venezuela */
{  4110, 310, -12, -12,         -10,         -10, 734,  -9,  -9, LTM_INVALID, LTM_INVALID }, /* 4110 USA Venezuela */
{  4120, 310, -12, -12,         -10,         -10, 734,  -9,  -9, LTM_INVALID, LTM_INVALID }, /* 4120 USA Venezuela */
{  4130, 310,  20,  20, LTM_INVALID, LTM_INVALID, 734,  -9,  -9, LTM_INVALID, LTM_INVALID }, /* 4130 USA Venezuela */
{  4140, 310, -12, -12,         -10,         -10, 734,  -9,  -9, LTM_INVALID, LTM_INVALID }, /* 4140 USA Venezuela */
{  4150, 310, -12, -12,         -10,         -10, 734,  -9,  -9, LTM_INVALID, LTM_INVALID }, /* 4150 USA Venezuela */
{  4369, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2303 USA Korea(South) */
{  4370, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2303 USA Korea(South) */
{  4371, 310, -20, -10,         -16,          -8, 450,  18,  18, LTM_INVALID, LTM_INVALID }, /* 2303 USA Korea(South) */
{  5732, 310, -12, -12,         -10,         -10, 732, -10, -10, LTM_INVALID, LTM_INVALID }, /* 5732 USA Colombia */
{  5734, 310, -12, -12,         -10,         -10, 732, -10, -10, LTM_INVALID, LTM_INVALID }, /* 5734 USA Colombia */
{  5772, 310, -12, -12,         -10,         -10, 732, -10, -10, LTM_INVALID, LTM_INVALID }, /* 5772 USA Colombia */
{  8002, 360, -8,   -8, LTM_INVALID, LTM_INVALID, 352,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 8002 St. Vincent/Grenadines Grenada */
{  8003, 360, -8,   -8, LTM_INVALID, LTM_INVALID, 352,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 8003 St. Vincent/Grenadines Grenada */
{  8036, 310, -20, -10,         -16,          -8, 356,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 8066 USA St. Kitts/Nevis */
{  8066, 310, -20, -10,         -16,          -8, 352,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 8066 USA Grenada */
{  8166, 342,  -8,  -8, LTM_INVALID, LTM_INVALID, 356,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 8166 Barbados St. Kitts/Nevis */

{  8198, 520,  14,  14, LTM_INVALID, LTM_INVALID, 350,  -8,  -6, LTM_INVALID, LTM_INVALID }, /* 8198 Thailand Bermuda */
{  8294, 603,   2,   2,          -2,          -2, 310, -20, -10,         -16,          -8 }, /* 8294 Algeria USA */
{  8358, 505,  16,  20,          20,          22, 310, -20, -10,         -16,          -8 }, /* 8358 Australia USA */
{  8360, 505,  16,  20,          20,          22, 310, -20, -10,         -16,          -8 }, /* 8360 Australia USA */
{  8616, 530,  24,  24,          26,          26, 310, -20, -10,         -16,          -8 }, /* 8616 New Zealand USA */
{  8860, 625,  -2,  -2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8860 Cape Verde USA */
{  8861, 625,  -2,  -2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8861 Cape Verde USA */
{  8863, 625,  -2,  -2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8863 Cape Verde USA */
{  8950, 647,   8,   8, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8950 Reunion Island USA */
{  8952, 647,   8,   8, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8952 Reunion Island USA */
{  8960, 612,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8960 Cote d'Ivoire USA */
{  8962, 612,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 8962 Cote d'Ivoire USA */
{  9080, 615,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9080 Togolese Republic USA */
{  9212, 619,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9212 Sierra Leone USA */
{  9244, 620,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9244 Ghana USA */
{  9246, 620,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9246 Ghana USA */
{  9322, 623,   2,   2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9322 Central Africa Republic USA */
{  9332, 623,   2,   2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9332 Central Africa Republic USA */
{  9394, 627,   2,   2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9394 Equatorial Guinea USA */
{  9426, 620,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9426 Ghana USA */
{  9488, 629,   2,   2, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9488 Democratic Republic of the Congo USA */
{  9562, 632,   0,   0, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9562 Guinea-Bissau USA */
{  9640, 653,   4,   4, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9640 Swaziland USA */
{  9642, 653,   4,   4, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9642 Swaziland USA */
{  9644, 653,   4,   4, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9644 Swaziland USA */
{  9788, 636,   6,   6, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9788 Ethiopia USA */
{  9790, 636,   6,   6, LTM_INVALID, LTM_INVALID, 310, -20, -10,         -16,          -8 }, /* 9790 Ethiopia USA */
{ 12461, 440,  18,  18, LTM_INVALID, LTM_INVALID, 470,  12,  12, LTM_INVALID, LTM_INVALID }, /* 12461 Japan Bangladesh */
{ 12463, 440,  18,  18, LTM_INVALID, LTM_INVALID, 470,  12,  12, LTM_INVALID, LTM_INVALID }, /* 12463 Japan Bangladesh */
{ 12464, 440,  18,  18, LTM_INVALID, LTM_INVALID, 470,  12,  12, LTM_INVALID, LTM_INVALID }, /* 12464 Japan Bangladesh */
{ 12978, 440,  18,  18, LTM_INVALID, LTM_INVALID, 363,  -8,  -8, LTM_INVALID, LTM_INVALID }, /* 12978 Japan Aruba */
{ 14850, 410,  10,  10, LTM_INVALID, LTM_INVALID, 404,  11,  11, LTM_INVALID, LTM_INVALID }, /* 14850 Pakistan India */
{ 16128, 230,   2,   2,           4,           4, 259,   4,   4,           6,           6 }, /* 16128 Czech republic Moldova */
{ 17783, 457,  14,  14, LTM_INVALID, LTM_INVALID, 302, -16,  -7,         -14,          -5 }, /* 17783 Laos Canada */
{ 32752, 708, -12, -12, LTM_INVALID, LTM_INVALID, 364, -10, -10,          -8,          -8 }  /* 14850 Honduras Bahamas */

};

#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


/*===========================================================================

FUNCTION CM_UTIL_ENCODE_DTMF

DESCRIPTION
  Populates the dtmf equivalent of given ascii value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -  dtmf_val contains the converted value.
  FALSE -  dtmf_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_util_encode_dtmf(

  uint8 ascii_val,
    /* Ascii value */

  uint8 *dtmf_val_ptr
    /* dtmf value */
)
{
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  boolean status = FALSE;
    /* Status of conversion */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2 ("in call %d %c ", ascii_val, ascii_val);

  /* Sanity check on data_val_ptr */
  if (!dtmf_val_ptr)
  {
    CM_ERR_0 ("data_val_ptr is NULL");

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* set status to TRUE */
  status = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_0;

  switch (ascii_val)
  {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      *dtmf_val_ptr = ascii_val - '0';
      break;

    case '0':
      *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_0;
      break;

    case '#':
      *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_PND;
      break;

    case '*':
      *dtmf_val_ptr = CMUTIL_DTMF_FOR_DIAL_STAR;
      break;

    default:
      status = FALSE;
      CM_MSG_HIGH_1( "Illegal digit code %d", ascii_val);
  } /* end switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Status of encode operation */
  return (status);

#else
  SYS_ARG_NOT_USED(ascii_val);
  SYS_ARG_NOT_USED(dtmf_val_ptr);
  SYS_ARG_NOT_CONST(dtmf_val_ptr);

  CM_MSG_MED_0("cm_util_encode_dtmf shouldn't be called. ");

  return (FALSE);
#endif  /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

} /* cm_util_encode_dtmf() */

/*===========================================================================

FUNCTION CM_UTIL_DTMF_TO_ASCII

DESCRIPTION
  Populates the ascii equivalent of given DTMF value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -  ascii_val contains the converted value.
  FALSE -  sscii_val contains 0 (represented by CMUTIL_DTMF_FOR_DIAL_0).

SIDE EFFECTS
  None

===========================================================================*/
boolean cm_util_dtmf_to_ascii(

  uint8 dtmf_val,
    /* DTMF value */

  char *ascii_val_ptr
    /* Ascii value */
)
{
  boolean status = FALSE;
    /* Status of conversion */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2 ("in call %d %c ", dtmf_val, dtmf_val);

  /* Sanity check on data_val_ptr */
  if (!ascii_val_ptr)
  {
    CM_ERR_0 ("ascii_val_ptr is NULL");

    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* set status to TRUE */
  status = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  *ascii_val_ptr = '0';

  switch (dtmf_val)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      *ascii_val_ptr = dtmf_val + '0';
      break;

    case CMUTIL_DTMF_FOR_DIAL_0:
      *ascii_val_ptr = '0';
      break;

    case CMUTIL_DTMF_FOR_DIAL_PND:
      *ascii_val_ptr = '#';
      break;

    case CMUTIL_DTMF_FOR_DIAL_STAR:
      *ascii_val_ptr = '*';
      break;

    default:
      status = FALSE;
      CM_MSG_HIGH_1( "Illegal dtmf code %d", dtmf_val);
  } /* end switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Status of encode operation */
  return (status);

} /* cm_util_dtmf_to_ascii() */


/*===========================================================================

FUNCTION: cmutil_resolve_curr_mcc

DESCRIPTION
 Given sid, ltm offset and daylt savings, it gives the corresponding mcc.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ablel to resolve current mcc.
  FALSE if not able to find or resolve mcc.

SIDE EFFECTS
  None

===========================================================================*/
static boolean cmutil_resolve_curr_mcc (

  uint16 curr_sid,
    /* sid for which country list is generated */

  int8 curr_ltm,
    /* ltm offset of current system */

  sys_daylt_savings_e_type curr_daylt,
    /* daylt savings flag of current system */

  uint16 *ret_curr_mcc
    /* resolved current mcc */
)
{
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  int list_index = 0;

  boolean is_sid_cnflct =  FALSE;
    /* flag to tell if there is a sid cnfclt */

  int sid_cnflt_res_entry = 0;
    /* Index in sid conflict record list */

  boolean is_timezone_match = FALSE;
    /* Does current timezone match for given country */

  uint16 found_mcc=CM_INVALID_MOBILE_COUNTRY_CODE;
     /* Found mcc through sid confliction resolution table based LTM offset*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check */
  if(  !BETWEEN(curr_ltm, -26, 26)
     ||
     !BETWEEN(curr_daylt, SYS_DAYLT_SAVINGS_NONE, SYS_DAYLT_SAVINGS_MAX)
     ||
     !ret_curr_mcc
   )
  {
    CM_ERR_3 (" Input Error sid=%d,ltm= %d, daytl=%d", curr_sid,
        curr_ltm, curr_daylt);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Is the given sid used by more than one country
  */
  for (list_index = 0; list_index < (int)ARR_SIZE(sid_cnflt_res_list);
                                               ++list_index)
  {
    if (sid_cnflt_res_list[list_index].sid == curr_sid)
    {
      is_sid_cnflct = TRUE;

      sid_cnflt_res_entry = list_index;

      break;
    }

  } /* For */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Sid conflict is found, try to resolve the conflict */
  if (is_sid_cnflct)
  {
       CM_MSG_HIGH_1 ("sid %d does conflict", curr_sid);
       /* Check if current entry matches daylt and ltm_off
       */
       /* Daylt is ON */
       if (curr_daylt == SYS_DAYLT_SAVINGS_ON)
       {

         if ((curr_ltm >=
              sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_min_ltm_with_dts )
              &&
             (curr_ltm <=
              sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_max_ltm_with_dts))
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc1;
           is_timezone_match = TRUE;
         }
         else if ((curr_ltm >=
                   sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_min_ltm_with_dts )
                   &&
                  (curr_ltm <=
                   sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_max_ltm_with_dts))
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc2;
           is_timezone_match = TRUE;
         }
         else
         {
           CM_MSG_HIGH_3( "Conflict resolution table is wrong curr_sid %d, curr_ltm %d, curr_daylt %d",
           curr_sid, curr_ltm, curr_daylt);
         }
       }
       /* Daylt is OFF */
       else if (curr_daylt == SYS_DAYLT_SAVINGS_OFF)
       {

         if((curr_ltm >=
             sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_min_ltm_no_dts)
             &&
             (curr_ltm <=
              sid_cnflt_res_list[sid_cnflt_res_entry].mcc1_max_ltm_no_dts)
            )
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc1;
           is_timezone_match = TRUE;
         }
         else if ((curr_ltm >=
                   sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_min_ltm_no_dts )
                   &&
                   (curr_ltm <=
                    sid_cnflt_res_list[sid_cnflt_res_entry].mcc2_max_ltm_no_dts))
         {
           found_mcc = sid_cnflt_res_list[sid_cnflt_res_entry].mcc2;
           is_timezone_match = TRUE;
         }
         else
         {
           CM_MSG_HIGH_3 ( "Conflict resoultin table is wrong curr_sid=%d, curr_ltm=%d, curr_daylt=%d",
                           curr_sid, curr_ltm, curr_daylt );
         }
       }


       if (is_timezone_match == TRUE)
       {
          *ret_curr_mcc = found_mcc;
          CM_MSG_HIGH_1 ("cft and res table used, mcc = %d", found_mcc);
          return TRUE;
       }
       else
       {
          CM_MSG_HIGH_1 ("not found through cft and res table, mcc = %d", 
                       found_mcc );
          return FALSE;
       }
   }
   /* Populate the country and return if sid does not conflict */
  else
  {
     CM_MSG_HIGH_1 ("sid %d does not conflict", curr_sid);

    for (list_index = 0; list_index < (int)ARR_SIZE(country_list);
         ++list_index)
    {
        if  ((curr_sid >= country_list[list_index].sid1) &&
           (curr_sid <= country_list[list_index].sid2))
        {
          *ret_curr_mcc = country_list[list_index].mobile_country_code;

          CM_MSG_HIGH_1 ("Country FOUND, cc = %d", *ret_curr_mcc);
          return TRUE;
        }
    }

    CM_MSG_HIGH_0("NO SID MATCH FOUND");
    return FALSE;
  }
#else

  SYS_ARG_NOT_USED(curr_sid);
  SYS_ARG_NOT_USED(curr_ltm);
  SYS_ARG_NOT_USED(curr_daylt);
  SYS_ARG_NOT_USED(ret_curr_mcc);
  SYS_ARG_NOT_CONST(ret_curr_mcc);
  return FALSE;
#endif /* #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */
} /* cmutil_populate_country_list () */



/*===========================================================================
FUNCTION: cmutil_identify_current_country

DESCRIPTION

   Identifies current country given details like sid, ltm and daylt savings.


  Returns
  CMUTIL_CURRENT_COUNTRY_RESOLVED - curr_mcc is populated
  CMUTIL_CURRENT_COUNTRY_CONFLICT - curr_mcc is INVALID
  CMUTIL_CURRENT_COUNTRY_FAILED   - curr_mcc is INVALID

DEPENDENCIES
  None.

RETURN VALUE
  curr_mcc

SIDE EFFECTS
  None

===========================================================================*/
cmutil_current_country_e_type cmutil_identify_current_country (

  uint16 curr_sid,
    /* Sid of current system */

  int8   curr_ltm,
    /* Ltm offset of current system */

  sys_daylt_savings_e_type curr_daylt,
    /* daylt savings for current system */

  uint16 *curr_mcc
    /* current mcc */
)
{

  uint16 ret_curr_mcc;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Sanity check */
  if(  !BETWEEN(curr_ltm, -26, 26)
     ||
     !BETWEEN(curr_daylt, SYS_DAYLT_SAVINGS_NONE, SYS_DAYLT_SAVINGS_MAX)
     ||
     !curr_mcc
   )
  {
    CM_ERR_3 (" Error with input sid %d, ltm %d, daylt %d",
                              curr_sid, curr_ltm, curr_daylt);

    return CMUTIL_CURRENT_COUNTRY_FAILED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3("curr_sid = %d, curr_ltm = %d, daylt = %d ",
              curr_sid, curr_ltm, curr_daylt);

  *curr_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the country that match curr_sid, curr_daylt and curr_ltm. */
  if (cmutil_resolve_curr_mcc (curr_sid, curr_ltm, curr_daylt, &ret_curr_mcc))
  {
    *curr_mcc = ret_curr_mcc;
    CM_MSG_HIGH_1 ("Country FOUND, mcc = %d", *curr_mcc);
    return CMUTIL_CURRENT_COUNTRY_RESOLVED;
  }
  else
  {
    CM_MSG_HIGH_0 ("Country NOT FOUND ");
    return CMUTIL_CURRENT_COUNTRY_FAILED;
  }

} /* cmutil_identify_current_country () */

/*===========================================================================

FUNCTION: cmutil_number_cmp

DESCRIPTION
    This function compares the two given numbers similar to memcmp. In case if 
first number is stored in DTMF digit mode then it will be converted 
to ascii before comparison.

num_ptr             : ptr to num1 structure 
cmp_index           : index position in first num from where to start comparing
num2                : second number to compare. In ASCII mode.
len                 : the number of characters to be compared
                      (Length of second number 3 existing invocations)

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if first number starts with second number.
  FALSE - if comparison is failed.

SIDE EFFECTS
 None

===========================================================================*/
static boolean cmutil_number_cmp( const cm_num_s_type *num1_ptr,
                                  size_t cmp_index,
                                  const char* num2,
                                  size_t len )
{
  char     *num1_ascii_buf_ptr = NULL;
  size_t  loop = 0;
  char    ascii_val;
  boolean  num_match;
    
  /* If number of characters to compare is greater than
  ** the length of either arrays, return FALSE */
  if ( len > ( num1_ptr->len - cmp_index ) || len <= 0 )
  {
     return FALSE;
  }
    
  CM_MSG_HIGH_1( "Number digit_mode %d", num1_ptr->digit_mode);

  /* convert number to ASCII if digit_mode is DTMF */
  switch( num1_ptr->digit_mode )
  {
    case CM_DIGIT_MODE_4BIT_DTMF:
    {
      boolean converted_to_ascii = TRUE;
      num1_ascii_buf_ptr = cm_mem_malloc( len );

      /* Copy the existing number to the new buffer before attempting
      ** to convert. This is done to cover the case where the 
      ** digit_mode is DTMF but the buffer actually contians ASCII 
      ** values. Albeit incorrect, this was the legecy behavior and 
      ** is therefore incorporated in the new design
      */
      memscpy( num1_ascii_buf_ptr, len, &(num1_ptr->buf[ loop + cmp_index ]), len );

      /* Now Loop through the digits to convert each of them to ascii */
      for (loop=0; loop < len; ++loop)
      {
        if ( !(cm_util_dtmf_to_ascii( num1_ptr->buf[loop + cmp_index], (char*) &ascii_val ) ) )
        {
          CM_MSG_HIGH_0 ("decode_dtmf error");
          converted_to_ascii = FALSE;
          break;
        }
        *(num1_ascii_buf_ptr + loop) = ascii_val;
      }

      if(TRUE == converted_to_ascii)
      {
        CM_MSG_HIGH_0("cmutil_number_cmp | Converting to ASCII");
      }
      break;
    }
    case CM_DIGIT_MODE_8BIT_ASCII:
    {
      num1_ascii_buf_ptr = (char*)&(num1_ptr->buf[cmp_index]);
      break;
    }
    default:
    {
      CM_ERR_1( "Invalid Digit Mode %d", num1_ptr->digit_mode);
      return FALSE;
    }
  } 

  /* Is first number starting with  second number */
  num_match = ( 0 == memcmp( (void *)num1_ascii_buf_ptr, (void *)num2, (size_t)len ) );

  /* Free the memory if it was allocated */
  if( num1_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF )
  {
    cm_mem_free( num1_ascii_buf_ptr );
  }
  
  if( num_match )
  {
    CM_MSG_HIGH_0("cmutil_number_cmp | Matched");
  }

  return num_match;
}

#ifdef CM_DEBUG
#error code not present
#endif

/*===========================================================================

FUNCTION: cmutil_replace_plus_with_digits

DESCRIPTION
  Takes a number with + in first position and replaces it with digits.
  If its a voice call the digits mode are not changed. For SMS the output 
  is always in DTMF mode.

DEPENDENCIES
  None.

RETURN VALUE
  CMUTIL_CONVER_RESULT_NUM_MODIFIED - converted_num_ptr has converted number
  CMUTIL_CONVER_RESULT_NUM_UNCHANGED - converted_num_ptr same as input
  CMUTIL_CONVER_RESULT_NUM_INPUT_ERRa - converted_num_ptr same as input

SIDE EFFECTS
  converted_num carries the converted digits

===========================================================================*/
cmutil_conver_result_e_type cmutil_replace_plus_with_digits (

 const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
  /* Number that needs to be converted */

 cm_num_s_type                               *converted_num_ptr
    /* Output carrying the converted number */
)
/*lint -esym(818,converted_num_ptr)*/
{
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  uint8 conversion_digits[CMUTIL_PLUS_REPL_DIGIT_LEN];
    /* Carries digits that replace plus */

  int conversion_digits_len = 0;
    /* Length of conversion_digits */

  char serving_cc_str [CMUTIL_COUNTRY_CODE_LEN];
    /* Carries country code */

  int  loop = 0;

  /* offset within the plus number from
  ** where numbers can be taken
  */
  int  pos_to_copy_num_from = 1;

  int  spc_position = 0;

  uint16 serving_mcc = 0;

  const cm_num_s_type *num_with_plus_ptr = NULL;
    /* Points to input with plus */

  cm_country_code_type home_mcc    = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Home MCC */

  cm_country_code_type current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Current MCC */

  cm_call_type_e_type  curr_call_type = CM_CALL_TYPE_NONE;
    /* Call type for which conversion is being made */

  int pos_of_first_digit = 0;
    /* Gives the starting position of digits with/without plus */

  boolean   is_destn_international = FALSE;
  /* Is the destn for the call national/intern */

  boolean   is_serving_mcc_found = FALSE;
    /* serving mcc in list of countries */

  int       digit_pos_for_converted_num = 0;
    /* Carries position where num has to be copied in converted num */

  int       serving_mcc_loop_indexes [CMUTIL_MCC_LOOP_LEN];
    /* Carries loop indexes that match a serving_mcc */

  int       serving_mcc_loop_index_len = 0;
    /* Length of serving_mcc_loop_indexes[] */

  boolean   is_spc_match_found = FALSE;
    /* Is there a spc match for the country and the dialed number */

  int       i = 0;
    /* Loop counter */

  int       list_entry = 0;
    /* Entry into country_list */

  uint8     dtmf_val  = 0;
    /* carries dtmf value */

  boolean   data_4bit_dtmf = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if (
       !num_details_for_conv_ptr
       ||
       !converted_num_ptr
     )
  {
    CM_ERR_0 ("Invalid input to cmutil_replace_plus_with_digits");

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy details from input */

  num_with_plus_ptr = &num_details_for_conv_ptr->num;

  /* Current country */
  current_mcc       = num_details_for_conv_ptr->current_mobile_country_code;

  /* Mobile's Home country */
  home_mcc          = num_details_for_conv_ptr->home_mobile_country_code;

  /* Voice, SMS  */
  curr_call_type    = num_details_for_conv_ptr->call_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if ( current_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       home_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       num_with_plus_ptr->len > (uint8)ARR_SIZE(num_with_plus_ptr->buf) /* !!! cm_num_validate() instead !!!! */
       ||
       num_with_plus_ptr->len > (uint8)ARR_SIZE(converted_num_ptr->buf)
     )
  {
    CM_MSG_HIGH_3( "Invalid input to cmutil_replace_plus_with_digits, curr_mcc %d, home_mcc %d, num_with_plus_len %d",
                   current_mcc, home_mcc, num_with_plus_ptr->len );

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


 

  /* Clear conver digits */
  memset (conversion_digits, 0, sizeof(conversion_digits));

  /* Clear CC string */
  memset (serving_cc_str, 0, sizeof(serving_cc_str));

  /* Clear array carrying mcc loop indexes */
  memset (serving_mcc_loop_indexes, 0, ARR_SIZE(serving_mcc_loop_indexes));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *converted_num_ptr = *num_with_plus_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Either number should start with "+" or num_type
  ** should be CM_NUM_TYPE_INTERNATIONAL for plus
  ** code functionality to kick in
  */
  /*lint -save -e641 */
  if (! ((num_with_plus_ptr->buf[0] == '+') ||
         (num_with_plus_ptr->number_type == CM_NUM_TYPE_INTERNATIONAL)
        )
     )
  {
    CM_MSG_HIGH_0("Num does not have + or NUM_TYPE_INTER");

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
  /*lint -restore */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If there is a plus in buffer need to skip it */
  if (num_with_plus_ptr->buf[0] == '+')
  {
    pos_of_first_digit = 1;
  }
  else
  {
    pos_of_first_digit = 0;
  }
  if(num_with_plus_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF)
  {
    data_4bit_dtmf = TRUE;
    for (loop=pos_of_first_digit; loop<num_with_plus_ptr->len; ++loop)
    {
      if ( !(cm_util_dtmf_to_ascii ( num_with_plus_ptr->buf[loop], (char*)&dtmf_val )))
      {
        data_4bit_dtmf = FALSE;
        CM_MSG_HIGH_0 ("digit_mode is 4 bit and data in ASCII");
        break;
      }
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (curr_call_type == CM_CALL_TYPE_VOICE)
  {
    serving_mcc = current_mcc;
  }
  else if (curr_call_type == CM_CALL_TYPE_SMS)
  {
    serving_mcc = home_mcc;
  }
  else
  {
    CM_MSG_HIGH_1("Unrecognized call type %d, return",
        curr_call_type);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* MCC */
  CM_MSG_HIGH_1("serving_mcc = %d", serving_mcc);

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  is_serving_mcc_found = FALSE;

  /* Find entry in country list for serving MCC */
  for (i = 0; i < (int)ARR_SIZE(country_list) &&
              serving_mcc_loop_index_len < (CMUTIL_MCC_LOOP_LEN - 1);
       ++i)
  {
    if  (country_list[i].mobile_country_code == serving_mcc)
    {
      is_serving_mcc_found = TRUE;

      /* Record the loop indices that match serving_mcc */
      serving_mcc_loop_indexes [++ serving_mcc_loop_index_len] = i;
    }
  }

  if ( !is_serving_mcc_found || !serving_mcc_loop_index_len )
  {
    CM_MSG_HIGH_0("Country matching with MCC not found, return");

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* index of serving_mcc in country_list, array will
  ** have atleast one entry at this point.
  */
  CM_ASSERT (serving_mcc_loop_index_len > 0);

  loop = serving_mcc_loop_indexes[1];


  /* Convert country code into string */
  /*lint -e{746} -e{718}*/ 
  /*lint -esym(628, snprintf) */  
  if ( !snprintf (serving_cc_str, CMUTIL_COUNTRY_CODE_LEN,
                           "%d", country_list[loop].country_code) )
  {
    CM_ERR_2 (" Country code %d for loop %d Invalid",
                               country_list[loop].country_code, loop);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2("country_code %d, serving_mcc %d", country_list[loop].country_code, serving_mcc);
  
  /* If outgoing number starts with serving country code, use NDD or SPC */
  if( cmutil_number_cmp( num_with_plus_ptr,
                         (size_t)pos_of_first_digit,
                         serving_cc_str, 
                         strlen(serving_cc_str)))
  {
    /* 
             In QC HPCD solution, SPC code of China include 13/15/18 since mobile number start with 13x/15x/18x, and 17 isn't included.
       But as recently MVNO will use 17XXXXX in China, so When user dial +8617xxxxxxxxx in China, device will send the called number
       as 017xxxxxxxxx (17 is processed as PSTN number with NDD appended). which cause the voice call will fail and the same phenomena
       happens to SMS too.
             
              We proposed solutions as below:
              a.For in commercial devices, need network solution: MSC and MC add support for 017xxxxxxxxxx, delete the leading 0 after 
                   MSC/MC  recognize 17x is mobile number.
              b.For future devices, When device dial in China or send SMS by China subscriber, device will convert +86 to 0086 for called number,
                  no more check for SPC.
       */
    
    /* when serving_mcc is China(mainland) , will will replace plus with IDD and no more check for SPC.*/
    if ( serving_mcc == CMUTIL_MCC_OF_CHINA)
    {
        if( ((int) strlen (country_list[loop].inter_dialed_digits)) > (int)CMUTIL_PLUS_REPL_DIGIT_LEN )
        {
            CM_ERR_0 ("conversion_digits_len exceeds buffer size");
            conversion_digits_len = memscpy ( conversion_digits,
                                sizeof(conversion_digits),
                                country_list[loop].inter_dialed_digits,
                                (size_t)CMUTIL_PLUS_REPL_DIGIT_LEN );
        }
        else
        {
            /* Replace plus with IDD */
            conversion_digits_len = memscpy ( conversion_digits,
                                sizeof(conversion_digits),
                                country_list[loop].inter_dialed_digits,
                                (int) strlen (country_list[loop].inter_dialed_digits) );
         }

        /* Need to strip + only */
        pos_to_copy_num_from = pos_of_first_digit;
    }
    else
    {
    /* SPC when used is after Countrycode +.CC.SPC.Number */
    spc_position = pos_of_first_digit + (int)strlen(serving_cc_str);

    if ( !(spc_position < (int)ARR_SIZE(num_with_plus_ptr->buf)))
    {
      CM_ERR_0 ("spc_position exceeds buffer size");

      return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
    }

    /* Need to strip + and country code.
    */
    pos_to_copy_num_from = pos_of_first_digit + (int)strlen(serving_cc_str);

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check for all SPCs that can correspond to matching mcc */
    for (i=1; i<= serving_mcc_loop_index_len; ++i)
    {

      /* Copy the entry index in country_list */
      list_entry = serving_mcc_loop_indexes[i];

      /* Checking for spc match with dialed number */
      if (  ((int)strlen(country_list[list_entry].special_prefix_code) != 0)
          &&( cmutil_number_cmp( num_with_plus_ptr,
                                 (size_t)spc_position,
                                 country_list[list_entry].special_prefix_code, 
                                 strlen(country_list[list_entry].special_prefix_code)))
       )
      {
        is_spc_match_found = TRUE;

        CM_MSG_HIGH_2 ("Number contains SPC for cc=%d and spc=%d",
           country_list[list_entry].country_code,
           country_list[list_entry].special_prefix_code);
        break;
      }

    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If there is no SPC match, prepend NDD*/
    if (!is_spc_match_found)
    {
      CM_MSG_HIGH_1 ("Number is being dialed with NDD for cc=%d",
                                   country_list[loop].country_code);

      /* KW complaints : Fix ?*/
      if ( ((int) strlen (country_list[loop].national_direct_dial)) > ((int)CMUTIL_PLUS_REPL_DIGIT_LEN) )
      {
        CM_ERR_0 ("conversion_digits_len exceeds buffer size");
        conversion_digits_len = memscpy( conversion_digits,
                                         sizeof(conversion_digits),
                                         country_list[loop].national_direct_dial,
                                         (size_t) CMUTIL_PLUS_REPL_DIGIT_LEN );
      }
      else
      {
        conversion_digits_len = memscpy( conversion_digits,
                                         sizeof(conversion_digits),
                                         country_list[loop].national_direct_dial,
                                         (size_t) strlen (country_list[loop].national_direct_dial) );
      }

    }
    }

    /* Check for NATIONAL/INTERNATIONAL Destn */
    is_destn_international = FALSE;

  }
  /* If outgoing number does not start with serving CC, start with IDD */
  else
  {
    CM_MSG_HIGH_1 ("Number is being dialed with IDD for cc=%d",
                    country_list[loop].country_code);

    if( ((int) strlen (country_list[loop].inter_dialed_digits)) > (int)CMUTIL_PLUS_REPL_DIGIT_LEN )
    {
       CM_ERR_0 ("conversion_digits_len exceeds buffer size");
      conversion_digits_len = memscpy ( conversion_digits,
                                        sizeof(conversion_digits),
                                        country_list[loop].inter_dialed_digits,
                                        (size_t)CMUTIL_PLUS_REPL_DIGIT_LEN );
    }
    else
    {
      conversion_digits_len = memscpy ( conversion_digits,
                                        sizeof(conversion_digits),
                                        country_list[loop].inter_dialed_digits,
                                        (int) strlen (country_list[loop].inter_dialed_digits) );
    }

    /* Need to strip + only
    */
    pos_to_copy_num_from = pos_of_first_digit;

    /* Check for NATIONAL/INTERNATIONAL Destn */
    is_destn_international = TRUE;
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If Network support is available in the country
  ** do not convert plus into digits.
  */
  if ( (country_list[loop].is_network_soln) &&
       (is_destn_international)
     )
  {
    CM_MSG_HIGH_1 ( "Netw support avail for this country [indx %d]", loop );

    /* Network solution is present, so conversion should be placed*/
     return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
  else
  {
    /* Digits will be used in place of plus
    ** so all calls become national
    */
    /*lint -save -e641 */
    converted_num_ptr->number_type = CM_NUM_TYPE_NATIONAL;
    /*lint -restore */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_MSG_HIGH_0 ("Digits being added to front ");

  for (loop=0; loop<conversion_digits_len; ++loop)
  {
    CM_MSG_HIGH_1 ("%c", conversion_digits[loop]);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if additional digits can be accomodated in result */
  if ( (conversion_digits_len + num_with_plus_ptr->len) >
        cmutil_get_max_number_chars())
  {
    CM_MSG_HIGH_2 ("Cannot accomodate result in output %d, %d",
                   conversion_digits_len, num_with_plus_ptr->len);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the conversion digits, NDD or IDD */
  for (loop=0; loop<conversion_digits_len; ++loop)
  {
    if((num_with_plus_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF) &&
       (data_4bit_dtmf == TRUE) )
    {
      if ( !(cm_util_encode_dtmf ( conversion_digits[loop],
              &dtmf_val )))
      {
        CM_ERR_0 ("encode_dtmf error, shld have been caught before");
      }
      converted_num_ptr->buf[loop] = dtmf_val;
    }
    else
    {
      converted_num_ptr->buf[loop] = conversion_digits[loop];
    }
  }

  /* Position after IDD or NDD has been added */
  digit_pos_for_converted_num = conversion_digits_len;

  /* Start copying Number part +.CC.Number or +.CC.SPCNumber */
  for (loop=pos_to_copy_num_from; loop<num_with_plus_ptr->len;
                                     ++loop, ++digit_pos_for_converted_num)
  {
    /* KW: check
    ** (num_with_plus_ptr->len > (uint8)ARR_SIZE(num_with_plus_ptr->buf)
    ** at the beginning of the function, guards 'loop' going out of bounds
    */
    converted_num_ptr->buf[digit_pos_for_converted_num] =
                  num_with_plus_ptr->buf[loop];

  }

  /* Actual length of converted number */
  converted_num_ptr->len = (uint8)digit_pos_for_converted_num;
    
    /* Dialed digits for Voice calls are converted into DTMF
    ** by CP. Since SMS is sent as an encoded message directly by WMS,
    ** CM should do it here
    **
    ** digit_mode == 4BIT_DTMF means that all the digits contained in number
    ** buffer can be converted into DTMF digits.
    */
 
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (num_with_plus_ptr->digit_mode == CM_DIGIT_MODE_8BIT_ASCII) && 
     (curr_call_type == CM_CALL_TYPE_SMS) )
  {

    /* Check if digit_mode can be set to 4BIT_DTMF.
    ** 4BIT_DTMF is preferred over ASCII type for
    ** call origination, since it reduces the length
    ** of origination message.
    */
    converted_num_ptr->digit_mode = CM_DIGIT_MODE_4BIT_DTMF;

    /* Cannot be converted to DTMF if characters other
    ** than 0-9, *, # are used.
    ** One example is when input number is "+1858123+++".
    ** The trailing + should not be changed.
    */
    for (loop=0; loop < converted_num_ptr->len; ++loop)
    {
      if ( !(  cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                     &dtmf_val
                                   )
            )
          )
      {
        converted_num_ptr->digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    if ( converted_num_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF )         
	{
      /* Loop through the digits to convert each of them
          ** to dtmf
      */
      for (loop=0; loop < converted_num_ptr->len; ++loop)
      {

        if ( !(cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                     &dtmf_val ))
           )
        {
          CM_ERR_0 ("encode_dtmf error, shld have been caught before");
        }

        converted_num_ptr->buf[loop] = dtmf_val;
      }

    }
  }
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_1 ("Final dialed string has digit_mode %d",
                             converted_num_ptr->digit_mode);

  for (loop=0; loop<converted_num_ptr->len; ++loop)
  {
    CM_MSG_HIGH_1 ("0x%x", converted_num_ptr->buf[loop]);
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return CMUTIL_CONVER_RESULT_NUM_MODIFIED;

#else

  SYS_ARG_NOT_USED(num_details_for_conv_ptr);
  SYS_ARG_NOT_USED(converted_num_ptr);
  SYS_ARG_NOT_CONST(converted_num_ptr);

  return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;

#endif /* #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */
} /* cmutil_replace_plus_with_digits () */
/*lint +esym(818,converted_num_ptr)*/

/*===========================================================================

FUNCTION: cmutil_replace_digits_with_plus

DESCRIPTION
  Takes a number with + in first position and replaces it with digits.

DEPENDENCIES
  None.

RETURN VALUE
  CMUTIL_CONVER_RESULT_NUM_MODIFIED  - converted_num_ptr has converted number
  CMUTIL_CONVER_RESULT_NUM_UNCHANGED - converted_num_ptr same as input
  CMUTIL_CONVER_RESULT_NUM_INPUT_ERR - converted_num_ptr same as input

SIDE EFFECTS
  converted_num carries the converted digits

===========================================================================*/
cmutil_conver_result_e_type cmutil_replace_digits_with_plus (

 const cm_num_details_for_conversion_s_type  *num_details_for_conv_ptr,
  /* Number that needs to be converted */

 cm_num_s_type                               *converted_num_ptr
    /* Output carrying the converted number */
)
/*lint -esym(818,converted_num_ptr)*/
{
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
  int idd_digits_len = 0;
    /* Length of idd_digits */

  char inter_dialed_digits[CMUTIL_IDD_DIGIT_LEN];
   /* International Dialed Digits*/

  int  loop = 0;

  /* offset within the plus number from
  ** where numbers can be taken
  */
  int  pos_to_copy_num_from = 1;

  uint16 serving_mcc = 0;

  const cm_num_s_type *num_ptr = NULL;
    /* Points to input number */

  cm_country_code_type home_mcc    = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Home MCC */

  cm_country_code_type current_mcc = CM_INVALID_MOBILE_COUNTRY_CODE;
    /* Current MCC */

  cm_call_type_e_type  curr_call_type = CM_CALL_TYPE_NONE;
    /* Call type for which conversion is being made */

  boolean   is_serving_mcc_found = FALSE;
    /* serving mcc in list of countries */

  int       digit_pos_for_converted_num = 0;
    /* Carries position where num has to be copied in converted num */

  int       serving_mcc_loop_indexes [CMUTIL_MCC_LOOP_LEN];
    /* Carries loop indexes that match a serving_mcc */

  int       serving_mcc_loop_index_len = 0;
    /* Length of serving_mcc_loop_indexes[] */

  int       i = 0;
    /* Loop counter */

  uint8     dtmf_val  = 0;
    /* carries dtmf value */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if (
       !num_details_for_conv_ptr
       ||
       !converted_num_ptr
     )
  {
    CM_ERR_0 ("Invalid input to cmutil_replace_digits_with_plus");

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /* Copy details from input */
  num_ptr = &num_details_for_conv_ptr->num;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If number type is international call (CM_NUM_TYPE_INTERNATIONAL), then
  ** HPCD should bypass. so returns CMUTIL_CONVER_RESULT_NUM_UNCHANGED
  */

  /*
  ** HPCD Revision B :
  ** According to section 3.6 of  HPCD Revision B(80-VK924-1 B),
  ** which describes "when the received string is not already in plus-code format,
  ** the application includes logic to convert the string to plus-code format in
  ** some situations. Note that it has been observed that some operators are
  ** sending IDD, NUM_TYPE=International. Therefore, IDD should be removed,
  ** irrespective of NUM_TYPE.
  */

    CM_MSG_HIGH_1("HPCD number type = %d",
        num_ptr->number_type );
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Current country */
  current_mcc       = num_details_for_conv_ptr->current_mobile_country_code;

  /* Mobile's Home country */
  home_mcc          = num_details_for_conv_ptr->home_mobile_country_code;

  /* Voice, SMS  */
  curr_call_type    = num_details_for_conv_ptr->call_type;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify input */
  if ( current_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       home_mcc == CM_INVALID_MOBILE_COUNTRY_CODE
       ||
       num_ptr->len > (uint8)ARR_SIZE(num_ptr->buf) /* !!! cm_num_validate() instead !!!! */
       ||
       num_ptr->len > (uint8)ARR_SIZE(converted_num_ptr->buf)
     )
  {
    CM_ERR_0 ("Invalid input to cmutil_replace_digits_with_plus");

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_0("cmutil_replace_digits_with_plus ");


  /* Clear array carrying mcc loop indexes */
  memset (serving_mcc_loop_indexes, 0, ARR_SIZE(serving_mcc_loop_indexes));

  /* Clear IDD string */
  memset (inter_dialed_digits, 0, sizeof(inter_dialed_digits));


 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy given number directly to converted_num */
  // strncpy ((char *)&converted_num_ptr->buf, (char *)&num_with_plus_ptr->buf,
  //          num_with_plus_ptr->len);
  *converted_num_ptr = *num_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (curr_call_type == CM_CALL_TYPE_VOICE)
  {
    serving_mcc = current_mcc;
  }
  else if (curr_call_type == CM_CALL_TYPE_SMS)
  {
    serving_mcc = home_mcc;
  }
  else
  {
    CM_MSG_HIGH_1("Unrecognized call type %d, return ",
        curr_call_type);

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* MCC */
  CM_MSG_HIGH_1("serving_mcc = %d", serving_mcc);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  is_serving_mcc_found = FALSE;

  /* Find entry in country list for serving MCC */
  for (i = 0; i < (int)ARR_SIZE(country_list) &&
              serving_mcc_loop_index_len < (CMUTIL_MCC_LOOP_LEN-1); 
       ++i)
  {
    if  (country_list[i].mobile_country_code == serving_mcc)
    {
      is_serving_mcc_found = TRUE;

      /* Record the loop indices that match serving_mcc 
      ** Note: while fixing Klockwork, it seems intentional that the author uses
      ** pre-increment and starts the index from 1.
      ** Below, there's a code stating:
      ** loop = serving_mcc_loop_indexes[1];
      */
      serving_mcc_loop_indexes [++ serving_mcc_loop_index_len] = i;
    }
  }

  if ( !is_serving_mcc_found || !serving_mcc_loop_index_len )
  {
    CM_MSG_HIGH_0("Country matching with MCC not found , return");

    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }


  CM_ASSERT (serving_mcc_loop_index_len > 0);

  loop = serving_mcc_loop_indexes[1];


  /* Copy IDD from coutry list record to local variable*/
  idd_digits_len = (int) strlen (country_list[loop].inter_dialed_digits);

  /* KW complaints : Fix ?*/
   if ((idd_digits_len > (int)CMUTIL_IDD_DIGIT_LEN))
   {
       CM_ERR_0 ("idd_digits_len exceeds buffer size");
       /* Limit size the buffer size*/
       idd_digits_len = (int)CMUTIL_IDD_DIGIT_LEN;
   }
   memscpy( inter_dialed_digits,
            sizeof(inter_dialed_digits),
            country_list[loop].inter_dialed_digits,
            (size_t)idd_digits_len);

  /* IDD is included in calling party number (caller ID) */
  pos_to_copy_num_from = idd_digits_len;
  
  /* If Incomming number starts with  IDD */
  if( cmutil_number_cmp( num_ptr,
                         0,
                         inter_dialed_digits,
                         (size_t)idd_digits_len ) )
  {

    digit_pos_for_converted_num=0;

    /* Remove IDD in buf */
    for (loop=pos_to_copy_num_from; loop<num_ptr->len;
                                     ++loop, ++digit_pos_for_converted_num)
    {
      converted_num_ptr->buf[digit_pos_for_converted_num] =
                  num_ptr->buf[loop];
    }

    /* IDD is removed so  Num type should be INTERNATIONAL*/
    /*lint -save -e641 */
    converted_num_ptr->number_type = CM_NUM_TYPE_INTERNATIONAL;
    /*lint -restore */

    /* Actual length of converted number */
    converted_num_ptr->len = (uint8)digit_pos_for_converted_num;
    /* Digit mode is changed only for call types SMS */
    if ((num_ptr->digit_mode == CM_DIGIT_MODE_8BIT_ASCII) &&
         (curr_call_type == CM_CALL_TYPE_SMS))
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Check if digit_mode can be set to 4BIT_DTMF.
      ** 4BIT_DTMF is preferred over ASCII type for
      ** call origination, since it reduces the length
      ** of origination message.
      */
      converted_num_ptr->digit_mode = CM_DIGIT_MODE_4BIT_DTMF;

      /* Cannot be converted to DTMF if characters other
      ** than 0-9, *, # are used.
      ** One example is when input number is "+1858123+++".
      ** The trailing + should not be changed.
      */
      for (loop=0; loop < converted_num_ptr->len; ++loop)
      {
        if (!( cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                       &dtmf_val
                                     )
              )
            )
        {
          converted_num_ptr->digit_mode = CM_DIGIT_MODE_8BIT_ASCII;
          break;
        }
      }

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Got converted from ASCII -> 4BIT */
      if (converted_num_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF)
      {

        /* Loop through the digits to convert each of them
            ** to dtmf
        */
        for (loop=0; loop < converted_num_ptr->len; ++loop)
        {

          if ( !(cm_util_encode_dtmf ( converted_num_ptr->buf[loop],
                                       &dtmf_val ))
             )
          {
            CM_ERR_0 ("encode_dtmf error, shld have been caught before");
          }

          converted_num_ptr->buf[loop] = dtmf_val;
        }
      } /* converted_num_ptr->digit_mode == CM_DIGIT_MODE_4BIT_DTMF */
    } /* num_ptr->digit_mode == CM_DIGIT_MODE_8BIT_ASCII */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_MSG_HIGH_1 ("Final dialed string has digit_mode %d",
                             converted_num_ptr->digit_mode);

    for (loop=0; loop<converted_num_ptr->len; ++loop)
    {
       CM_MSG_HIGH_1 ("0x%x", converted_num_ptr->buf[loop]);
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    return CMUTIL_CONVER_RESULT_NUM_MODIFIED;
  }
  else
  {
    return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#else

  SYS_ARG_NOT_USED(num_details_for_conv_ptr);
  SYS_ARG_NOT_USED(converted_num_ptr);
  SYS_ARG_NOT_CONST(converted_num_ptr);

  return CMUTIL_CONVER_RESULT_NUM_UNCHANGED;

#endif /* #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */
} /* cmutil_replace_digits_with_plus () */
/*lint +esym(818,converted_num_ptr)*/

/*===========================================================================

FUNCTION: cm_util_perform_num_conversion

DESCRIPTION
  This function performs the following conversions

   - For CONV_REQ_PLUS_TO_DIGITS replaces starting "+" of number with
     digits.

   - For CONV_REQ_DIGITS_TO_PLUS replaces specific length of starting
     digits with "+".


DEPENDENCIES
  None.

RETURN VALUE
 If it is CMUTIL_NUM_CONVERSION_STATUS_SUCCESS
 converted_num_ptr carries changed number.

SIDE EFFECTS
  converted_num carries the converted digits

===========================================================================*/
cmutil_conver_result_e_type cm_util_perform_num_conversion(

  cmutil_num_conv_req_e_type               num_conv_req,
    /* Convert from plus to digits or digits to plus */

  const cm_num_details_for_conversion_s_type      *num_details_for_conv_ptr,
    /* Holds actual number on which conversion has
    ** to be performed
    */

  cm_num_s_type                             *converted_num_ptr /* !!! Check CM return convention !!!! */
    /* Converted number as a result of operation
    */

)
/*lint -esym(818,converted_num_ptr)*/
{

  cmutil_conver_result_e_type conver_result = CMUTIL_CONVER_RESULT_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!num_details_for_conv_ptr || !converted_num_ptr)
  {
    CM_ERR_0 (" One of the input param is NULL ");

    return CMUTIL_CONVER_RESULT_NUM_INPUT_ERR;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_2 (" num conv req %d, number length %d",
                  num_conv_req, num_details_for_conv_ptr->num.len);

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Copy given number for conversion to converted num */
  *converted_num_ptr = *(&(num_details_for_conv_ptr->num));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* !!!! Copy it into return_converted_num_ptr !!!!!*/
  if (num_details_for_conv_ptr->hs_based_plus_dial_setting ==
             CM_HS_BASED_PLUS_DIAL_DISABLED)
  {
    CM_MSG_HIGH_0 ("Plus dial is disabled ");

    return  CMUTIL_CONVER_RESULT_NUM_UNCHANGED;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process request to convert plus to digits*/
  if (num_conv_req == CMUTIL_NUM_CONV_REQ_PLUS_TO_DIGITS)
  {

    conver_result = cmutil_replace_plus_with_digits (
                            num_details_for_conv_ptr,
                            converted_num_ptr );

  }
  /* Process request to convert digits to plus*/
  else if (num_conv_req == CMUTIL_NUM_CONV_REQ_DIGITS_TO_PLUS)
  {
    conver_result = cmutil_replace_digits_with_plus (
                            num_details_for_conv_ptr,
                            converted_num_ptr );
  }
  else
  {
    CM_ERR_1 ("Invalid conversion request %d", num_conv_req);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If conversion failed clear converted_num */

  return conver_result;
}

/*===========================================================================

FUNCTION: cm_util_init_num_conversion_params

DESCRIPTION
  Initializes parameters that will be used in num conversion function

DEPENDENCIES
  None.

RETURN VALUE
  Initialized parameters

SIDE EFFECTS
  None
===========================================================================*/
void cm_util_init_num_conversion_params(

  cmutil_num_conv_req_e_type               *num_conv_req_ptr,
    /* Convert from plus to digits or digits to plus */

  cm_num_details_for_conversion_s_type      *num_details_for_conv
    /* Holds actual number on which conversion has
    ** to be performed
    */

)
/*lint -esym(818,num_conv_req_ptr)*/
/*lint -esym(818,num_details_for_conv)*/
{

  *num_conv_req_ptr = CMUTIL_NUM_CONV_REQ_NO_REQ;

  num_details_for_conv->call_type                   = CM_CALL_TYPE_VOICE;

  num_details_for_conv->current_mobile_country_code = CM_INVALID_MOBILE_COUNTRY_CODE;

  num_details_for_conv->home_mobile_country_code    = CM_INVALID_MOBILE_COUNTRY_CODE;

  num_details_for_conv->hs_based_plus_dial_setting  = CM_HS_BASED_PLUS_DIAL_DISABLED;

  cm_num_init(&num_details_for_conv->num);


} /* cm_util_init_num_conversion_params () */

/*===========================================================================

FUNCTION: cm_util_country_info_for_dialing

DESCRIPTION
  provide country network information for plus coding dialing but actully VZW assisted dialing.

DEPENDENCIES
  None.

RETURN VALUE

  country_info_ptr carries country information corresponding mcc


SIDE EFFECTS
  None
===========================================================================*/
cmutil_country_info_result_e_type cm_util_country_info_for_dialing(

  cm_country_code_type req_mcc,
     /* Requested MCC, input is invalid mcc, we try to provide serving mcc  */

  cmutil_country_specific_record_s *country_info_ptr
    /* Country network information corresponding to MCC*/

)
/*lint -esym(818,country_info_ptr)*/
{
    #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

    int          list_entry = 0;
    cmss_s_type  *ss_ptr       = cmss_ptr();
      /* point at serving_system object */

    /* We consider that client does not know current serving mcc, so we provide current mcc */
    if( req_mcc ==   CM_INVALID_MOBILE_COUNTRY_CODE)
    {
       if(( req_mcc =  ss_ptr->info.current_mcc) ==  CM_INVALID_MOBILE_COUNTRY_CODE)
       {
          CM_ERR_0 (" Country info dialing : Invalid MCC ");
          return CMUTIL_COUNTRY_INFO_RESULT_MCC_ERR;
       }
    }
    /* Find entry in country list for serving MCC */
    for(list_entry = 0; list_entry < (int)ARR_SIZE(country_list); ++list_entry)
    {
        if(country_list[list_entry].mobile_country_code == req_mcc)
        {
           CM_MSG_HIGH_2("Country info Reqeust mcc found : mcc  = %d index=%d",
            req_mcc, list_entry);

           memscpy( country_info_ptr,
                    sizeof(cmutil_country_specific_record_s),
                    &country_list[list_entry],
            sizeof(cmutil_country_specific_record_s));

           return  CMUTIL_COUNTRY_INFO_RESULT_FOUND;
        }
    }
    #else
    SYS_ARG_NOT_USED(req_mcc);
    SYS_ARG_NOT_USED(country_info_ptr);
    #endif /* #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

    return CMUTIL_COUNTRY_INFO_RESULT_NOT_FOUND;
}
/*lint +esym(818,country_info_ptr)*/

#ifdef CM_GW_SUPPORTED

/*===========================================================================

FUNCTION: cmutil_sups_fill_register_params

DESCRIPTION
  Fills the sups command and cm_reg_ss_params_s from the
  generic sups structure.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_reg_ss_params_s structure in case of success.
  In case of failure an error value matching the error case will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_register_params(

  cmutil_sups_params_s_type *ss_params_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/

  cm_num_s_type  sups_num

)
/*lint -esym(818, sups_start) */
{
  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;
  cm_num_s_type number;
  byte bcd_number[CM_CALLED_PARTY_BCD_NO_LENGTH];

  if(ss_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer ss_params_ptr is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer sups_start_ptr is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_params_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_params_ptr->sups_params.reg_ss_params.ss_ref = 0;
  ss_params_ptr->sups_params.reg_ss_params.ss_code = \
                               (uint8)sups_start_ptr->ss_code;
  /*BSG information*/
  ss_params_ptr->sups_params.reg_ss_params.basic_service.bs_code =
               (byte)(sups_start_ptr->basic_service & 0xFF); /*lint !e641 */
  /*lint -save -e641 -e702 */
  ss_params_ptr->sups_params.reg_ss_params.basic_service.code_type =
    (bsg_code_type)(sups_start_ptr->basic_service >> 16);
  ss_params_ptr->sups_params.reg_ss_params.basic_service.present =
   (ss_params_ptr->sups_params.reg_ss_params.basic_service.code_type == \
     NO_CODE) ? FALSE : TRUE;

  /*Forwarded to Number*/
  if(sups_start_ptr->req.reg.data[0] != '\0' )
  {
    cm_num_fill(&number, (byte *)sups_start_ptr->req.reg.data,
              (byte)sups_start_ptr->req.reg.len,
              CM_DIGIT_MODE_8BIT_ASCII);

    if(number.len > 0)
    {
      /*Default Values*/
      number.number_type = sups_num.number_type;
      number.number_plan = sups_num.number_plan;

      memset(bcd_number, '\0', CM_CALLED_PARTY_BCD_NO_LENGTH);
      /*BCD conversion*/
      cm_util_ascii_num_to_bcd_num( &number, bcd_number, sizeof( bcd_number ) ); 

      memset(ss_params_ptr->sups_params.reg_ss_params.fwd_to_num.data,
             0xFF, CM_CALLED_PARTY_BCD_NO_LENGTH);

      memscpy( ss_params_ptr->sups_params.reg_ss_params.fwd_to_num.data,
               sizeof(ss_params_ptr->sups_params.reg_ss_params.fwd_to_num.data),
             &bcd_number[1], MIN((sizeof(bcd_number)-sizeof(bcd_number[0])),bcd_number[0]));

      ss_params_ptr->sups_params.reg_ss_params.fwd_to_num.length = \
                                                          bcd_number[0];
      ss_params_ptr->sups_params.reg_ss_params.fwd_to_num.present = TRUE;
    }
  }
  else
  {
    ss_params_ptr->sups_params.reg_ss_params.fwd_to_num.present = FALSE;
  }

  /* forwarded to subaddress */
  ss_params_ptr->sups_params.reg_ss_params.fwd_to_subaddr.present = FALSE;

  /* no reply condition timer */
  if(sups_start_ptr->req.reg.nr_timer != 0 )
  {
    ss_params_ptr->sups_params.reg_ss_params.nr_timer.present = TRUE;
    ss_params_ptr->sups_params.reg_ss_params.nr_timer.timer_value =
                                (uint8)sups_start_ptr->req.reg.nr_timer;
  }
  else
  {
    ss_params_ptr->sups_params.reg_ss_params.nr_timer.present = FALSE;
  }

  return ret;
}
/*lint +esym(818, sups_start) */



/*===========================================================================

FUNCTION: cmutil_sups_fill_erase_params

DESCRIPTION
  Fills the sups command and cm_erase_ss_params_s from the
  generic sups structure.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_erase_ss_params_s structure, sups command in case
  of success. In case of failure an error value matching the error case
  will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_erase_params(

  cmutil_sups_params_s_type *ss_params_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/

  cm_num_s_type  sups_num
)
/*lint -esym(818, sups_start) */
{
  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;

  if(ss_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_params_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_params_ptr->sups_params.erase_ss_params.ss_ref = 0;
  ss_params_ptr->sups_params.erase_ss_params.ss_code =
                                      (uint8)sups_start_ptr->ss_code;
  /*BSG information*/
  ss_params_ptr->sups_params.erase_ss_params.basic_service.bs_code =
                (byte)(sups_start_ptr->basic_service & 0xFF);/*lint !e641 */
  ss_params_ptr->sups_params.erase_ss_params.basic_service.code_type =
            (bsg_code_type)(sups_start_ptr->basic_service >> 16);
            /*lint -save -e641 -e702 */
  ss_params_ptr->sups_params.erase_ss_params.basic_service.present =
  (ss_params_ptr->sups_params.erase_ss_params.basic_service.code_type == \
  NO_CODE) ? FALSE : TRUE;

  return ret;
}
/*lint +esym(818, sups_start) */


/*===========================================================================

FUNCTION: cmutil_sups_fill_activate_params

DESCRIPTION
  Fills the sups command and cm_act_ss_params_s from the
  generic sups structure.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_act_ss_params_s structure, sups command in case
  of success. In case of failure an error value matching the error case
  will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_activate_params(

  cmutil_sups_params_s_type *ss_params_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/

  cm_num_s_type  sups_num
)
/*lint -esym(818, sups_start) */
{
  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;
  int i;

  if(ss_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_params_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_params_ptr->sups_params.activate_ss_params.ss_ref = 0;
  ss_params_ptr->sups_params.activate_ss_params.ss_code =
                                       (uint8)sups_start_ptr->ss_code;
  /*BSG information*/
  ss_params_ptr->sups_params.activate_ss_params.basic_service.bs_code =
              (byte)(sups_start_ptr->basic_service & 0xFF); /*lint !e641 */
  ss_params_ptr->sups_params.activate_ss_params.basic_service.code_type =
           (bsg_code_type)(sups_start_ptr->basic_service >> 16);
           /*lint -save -e641 -e702 */
  ss_params_ptr->sups_params.activate_ss_params.basic_service.present =
   (ss_params_ptr->sups_params.activate_ss_params.basic_service.code_type \
    == NO_CODE)? FALSE : TRUE;

  ss_params_ptr->sups_params.activate_ss_params.ss_pwd.present = FALSE;

  /*lint -e655 */
  /* 655: bit-wise operation uses (compatible) enum's */
  if(((ss_operation_code_T)
      (sups_start_ptr->ss_code & MOST_SIG_NIBBLE) == allCallRestrictionSS) )
  {
    /*Fill Password in the structure*/
    for (i = 0;
        (i < MAX_PWD_CHAR) && \
        ( sups_start_ptr->req.passwd.ss_password[i] != '\0' ); i++)
    {
      ss_params_ptr->sups_params.activate_ss_params.ss_pwd.ss_password[i] =
                                  sups_start_ptr->req.passwd.ss_password[i];
    }

    /* Password is of MAX_PWD_CHAR chars by standard */
    if(MAX_PWD_CHAR == i)
    {
      ss_params_ptr->sups_params.activate_ss_params.ss_pwd.present = TRUE;
    }
  }

  return ret;
}
/*lint +esym(818, sups_start_ptr) */

/*===========================================================================

FUNCTION: cmutil_sups_fill_deactivate_params

DESCRIPTION
  Fills the sups command and cm_deact_ss_params_s from the
  generic sups structure.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_deact_ss_params_s structure, sups command in case
  of success. In case of failure an error value matching the error case
  will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_deactivate_params(

  cmutil_sups_params_s_type *ss_params_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/

  cm_num_s_type  sups_num
)
/*lint -esym(818, sups_start) */
{

  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;
  int i;

  if(ss_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_params_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_params_ptr->sups_params.deactivate_ss_params.ss_ref = 0;
  ss_params_ptr->sups_params.deactivate_ss_params.ss_code = \
                           sups_start_ptr->ss_code;
  /*BSG information*/
  ss_params_ptr->sups_params.deactivate_ss_params.basic_service.bs_code =
                (byte) (sups_start_ptr->basic_service & 0xFF);/*lint !e641 */
  ss_params_ptr->sups_params.deactivate_ss_params.basic_service.code_type =
           (bsg_code_type) (sups_start_ptr->basic_service >> 16);
           /*lint -save -e641 -e702 */
  ss_params_ptr->sups_params.deactivate_ss_params.basic_service.present =
  (ss_params_ptr->sups_params.deactivate_ss_params.basic_service.code_type
    == NO_CODE) ? FALSE : TRUE;

  ss_params_ptr->sups_params.deactivate_ss_params.ss_pwd.present = FALSE;

  /*lint -e655 */
  /* 655: bit-wise operation uses (compatible) enum's */
  if((((ss_operation_code_T)
      (sups_start_ptr->ss_code & MOST_SIG_NIBBLE)) == allCallRestrictionSS) )
  {
    /*Fill Password*/
    for (i = 0;
         (i < MAX_PWD_CHAR) && \
          ( sups_start_ptr->req.passwd.ss_password[i] != '\0' ); i++)
    {
      ss_params_ptr->sups_params.deactivate_ss_params.ss_pwd.ss_password[i] =
                            sups_start_ptr->req.passwd.ss_password[i];
    }

    /* Password is of MAX_PWD_CHAR chars by standard */
    if(MAX_PWD_CHAR == i)
    {
      ss_params_ptr->sups_params.deactivate_ss_params.ss_pwd.present = TRUE;
    }
  }
  else if(sups_start_ptr->ss_code == ccbs)
  {
    /*CCBS applicable only for deactivation and interrogation*/
    ss_params_ptr->sups_params.deactivate_ss_params.ccbs_indx =
                   (cm_ccbs_indx_type)sups_start_ptr->req.ccbsIndex;
  }

  return ret;
}
/*lint +esym(818, sups_start) */


/*===========================================================================

FUNCTION: cmutil_sups_fill_interrogate_params

DESCRIPTION
  Fills the sups command and structure of type cm_interr_ss_params_s_type
  from the generic sups structure.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_interr_ss_params_s_type structure, sups command
  in case of success. In case of failure an error value matching the error
  case will be returned.

SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_interrogate_params(

  cmutil_sups_params_s_type *ss_params_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/
  cm_num_s_type  sups_num
)
/*lint -esym(818, sups_start) */
{
  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;

  if(ss_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_params_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_params_ptr->sups_params.interrogate_ss_params.ss_ref = 0;
  ss_params_ptr->sups_params.interrogate_ss_params.ss_code =
                                          (uint8)sups_start_ptr->ss_code;
  /*BSG information*/
  ss_params_ptr->sups_params.interrogate_ss_params.basic_service.bs_code =
                            (byte) (sups_start_ptr->basic_service & 0xFF);
  ss_params_ptr->sups_params.interrogate_ss_params.basic_service.code_type =
                       (bsg_code_type) (sups_start_ptr->basic_service >> 16);
                            /*lint -save -e641 -e702 */
  ss_params_ptr->sups_params.interrogate_ss_params.basic_service.present =
  (ss_params_ptr->sups_params.interrogate_ss_params.basic_service.code_type \
    == NO_CODE) ? FALSE : TRUE;

  if(sups_start_ptr->ss_code == ccbs)
  {
    /*CCBS applicable only for deactivation and interrogation*/
    ss_params_ptr->sups_params.interrogate_ss_params.ccbs_indx =
                   (cm_ccbs_indx_type) sups_start_ptr->req.ccbsIndex;
  }

  return ret;
}
/*lint +esym(818, sups_start) */

/*===========================================================================

FUNCTION: cmutil_sups_fill_uss_params

DESCRIPTION
  Fills the sups command and structure cm_uss_params_s from the generic
  sups structure.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_uss_params_s structure, sups command in case
  of success. In case of failure an error value matching the error case
  will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_uss_params(

  cmutil_sups_params_s_type *ss_params_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/

   cm_num_s_type  sups_num
)
/*lint -esym(818, sups_start) */
{
  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;

  if(ss_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_params_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_params_ptr->sups_params.uss_params.ss_ref = 0;
  ss_params_ptr->sups_params.uss_params.uss_data.present = TRUE;
  ss_params_ptr->sups_params.uss_params.uss_data.uss_data_coding_scheme =
                                              (byte) CM_USSD_DCS_UNSPECIFIED;

  if(sups_start_ptr->req.ussd.data[0] == '\0')
  {
    CM_ERR_0 (" ussd data is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }
  memset(ss_params_ptr->sups_params.uss_params.uss_data.ussData,
         '\0', MAX_USS_CHAR_ASCII);

  memscpy( ss_params_ptr->sups_params.uss_params.uss_data.ussData,
           sizeof(ss_params_ptr->sups_params.uss_params.uss_data.ussData),
           sups_start_ptr->req.ussd.data, MAX_USS_CHAR_ASCII );

  ss_params_ptr->sups_params.uss_params.uss_data.size =
       (byte) sups_start_ptr->req.ussd.length;

  return ret;
}
/*lint +esym(818, sups_start) */

/*===========================================================================

FUNCTION: cmutil_sups_fill_reg_pwd_params

DESCRIPTION
  Fills the sups command and structure cm_reg_pwd_params_s from the generic
  sups structure. The sups string input was for changing/registering
  password.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled cm_reg_pwd_params_s structure, sups command in case
  of success. In case of failure an error value matching the error case
  will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_sups_fill_reg_pwd_params(

  cmutil_sups_params_s_type *ss_prms_ptr,
  /*OUT Sups Params*/

  const cmutil_sups_start_s_type *sups_start_ptr,
  /*Generic Sups Params*/

  cm_num_s_type  sups_num

)
/*lint -esym(818, sups_start) */
{
  cmutil_parse_sups_result_e_type ret = CMUTIL_PARSE_SUPS_RESULT_SUCCESS;
    int i;

  if(ss_prms_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  ss_prms_ptr->sups_cmd = sups_start_ptr->ss_operation;

  ss_prms_ptr->sups_params.reg_pwd_params.ss_ref = 0;
  ss_prms_ptr->sups_params.reg_pwd_params.ss_code.present = TRUE;
  ss_prms_ptr->sups_params.reg_pwd_params.ss_code.ss_code =
                                             (uint8)sups_start_ptr->ss_code;
  /*Initialize to FALSE by default*/
  ss_prms_ptr->sups_params.reg_pwd_params.ss_old_pwd.present = FALSE;
  ss_prms_ptr->sups_params.reg_pwd_params.ss_new_pwd.present = FALSE;
  ss_prms_ptr->sups_params.reg_pwd_params.ss_new_pwd_again.present = FALSE;

  /*OLD PASSWORD*/
  for (i = 0;
       (i<MAX_PWD_CHAR) && \
       ( sups_start_ptr->req.passwd.ss_password[i] != '\0'); i++)
  {
    ss_prms_ptr->sups_params.reg_pwd_params.ss_old_pwd.ss_password[i] =
                                  sups_start_ptr->req.passwd.ss_password[i];
  }

  /* Password is of MAX_PWD_CHAR chars by standard */
  if(MAX_PWD_CHAR == i)
  {
    ss_prms_ptr->sups_params.reg_pwd_params.ss_old_pwd.present = TRUE;
  }

  /*NEW PASSWORD*/
  for (i = 0;
      (i < MAX_PWD_CHAR) && \
      (sups_start_ptr->req.passwd.ss_new_password[i]!='\0'); i++)
  {
    ss_prms_ptr->sups_params.reg_pwd_params.ss_new_pwd.ss_password[i] =
                              sups_start_ptr->req.passwd.ss_new_password[i];
  }

  if(MAX_PWD_CHAR == i)
  {
    ss_prms_ptr->sups_params.reg_pwd_params.ss_new_pwd.present = TRUE;
  }

  /*NEW PASSWORD AGAIN*/
  for (i = 0;
       (i<MAX_PWD_CHAR) && \
       (sups_start_ptr->req.passwd.ss_new_password_again[i] != '\0'); i++)
  {
    ss_prms_ptr->sups_params.reg_pwd_params.ss_new_pwd_again.ss_password[i] \
        = sups_start_ptr->req.passwd.ss_new_password_again[i];
  }

  if(MAX_PWD_CHAR == i)
  {
    ss_prms_ptr->sups_params.reg_pwd_params.ss_new_pwd_again.present =
                                                                      TRUE;
  }

  return ret;
}
/*lint +esym(818, sups_start) */


/*===========================================================================

FUNCTION: cmutil_chk_ss

DESCRIPTION
  The sups string in the form of tokens will be checked for some special
  cases and the common sups structure will be filled. For example the sups
  string can be of the form as given below:
  One of **, ##, *#, *, #, **03* is say Sups Type (ST)
  Service Code is (SC). Sups string will be of form STSC*SI#
  Supplementary information (SI) is made of SIA, SIB, SIC
  Sups String constructed will be one among the many forms given below
  STSC*SIA*SIB*SIC#
  STSC*SIA*SIB#
  STSC*SIA**SIC#
  STSC*SIA#
  STSC**SIB*SIC#
  STSC**SIB#
  STSC***SIC#
  STSC#
  The input tokens for this function will be the actual string, SC, SIA,
  SIB, SIC

DEPENDENCIES
  None.

RETURN VALUE

  Returns the filled common sups structure in case of success. In case of
  failure an error value matching the error case will be returned.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_chk_ss(

  int num_tokens,
  /*Number of tokens as a result of sups string tokenize*/

  const void *tokens_ptr[],
  /*Tokens of the sups string*/

  cmutil_sups_start_s_type *sups_start_ptr
  /*Common sups structure to be filled*/

)
/*lint -esym(818, tokens_ptr) */
{
  char in_val[CMUTIL_SUPS_MAX_TOKENS][CM_MAX_NUMBER_CHARS];
  int i,j;
  const char *temp_ptr;
  const char *ss_str_ptr;
  int pos = 0;
  cmutil_sups_operation_s_type  *current_ss_operation_ptr =
                                     cmutil_sups_operation_input;
  uint8   str_len;
  boolean ss_match = TRUE;
  cm_sups_cmd_e_type ss_operation = CM_SUPS_CMD_NONE;
  char bsg[CMUTIL_SUPS_MAX_BSG_LEN + 1];
      /*container for basic service group info*/
  char sia[CMUTIL_SUPS_MAX_SIA_LEN + 1];
  char sib[CMUTIL_SUPS_MAX_SIB_LEN + 1];
  char sic[CMUTIL_SUPS_MAX_SIC_LEN + 1];
  char ss_code[CMUTIL_SUPS_MAX_SC_LEN + 1];
  char sups_str[CM_MAX_NUMBER_CHARS + 1];

  CM_MSG_HIGH_0("cmutil_chk_ss");

  if(sups_start_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  if((num_tokens <= 0) || (tokens_ptr[0] == NULL))
  {
     return CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_ILLEGAL_ENTRY;
  }

  for(i = 0;i < CMUTIL_SUPS_MAX_TOKENS;i++)
  {
    (void)memset(in_val[i],'\0',CM_MAX_NUMBER_CHARS);
  }

  temp_ptr = tokens_ptr[0];
  str_len = (uint8)strlen(temp_ptr);
  ss_str_ptr = tokens_ptr[0];

  for (i =0;!ISDIGIT(temp_ptr[i]);i++)
    in_val[0][i] = temp_ptr[i];

  for(j = 1;j < num_tokens;j++)
  {
    temp_ptr = tokens_ptr[j];
    for (i = 0;(temp_ptr[i] != '*') && (temp_ptr[i] != '#') && \
                                         i<CM_MAX_NUMBER_CHARS; i++)
    {
      in_val[j][i] = temp_ptr[i];
    }
  }

  /*Initialize the token containers to \0*/
  memset( sia, '\0', sizeof(sia) );
  memset( sib, '\0', sizeof(sib) );
  memset( sic, '\0', sizeof(sic) );
  memset(ss_code, '\0', sizeof(ss_code) );
  memset (bsg, '\0', sizeof(bsg));
  memset( sups_str, '\0', CM_MAX_NUMBER_CHARS);

  /*Initialize the input sups common structure to 0*/
  memset(sups_start_ptr, 0, sizeof(cmutil_sups_start_s_type));

  /*called number should be entered along with CLIR suppression, invocation
  i.e for eg. *31#<called number> as per 3GPP TS 22.030*/
  if( ( (memcmp(tokens_ptr[0], CLIR_INVOCATION, 4) == 0) ||
         (memcmp(tokens_ptr[0], CLIR_SUPRESSION, 4) == 0) ))
  {
     return CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_NO_MATCH;
  }

  /* STEP 1: Find the SS operation */
  while(current_ss_operation_ptr->sups_operation != NULL)
  {
    if( !strcmp( in_val[pos], current_ss_operation_ptr->sups_operation) )
    {
      ss_operation = current_ss_operation_ptr->sups_type;
      pos++;
      break;
    }
    current_ss_operation_ptr++;
  }

  if( !strcmp( in_val[pos], CMUTIL_SUPS_LOCAL_BSG_REG_PASSWD ) )
  {
    ss_operation = CM_SUPS_CMD_REG_PASSWORD;
    pos++;
  }

  /* By now the operation must have been identified */
  sups_start_ptr->ss_operation = ss_operation;

  if ( sups_start_ptr->ss_operation != CM_SUPS_CMD_NONE )
  {
    /* STEP 2: Find the SI fields */
    (void)memscpy( ss_code, sizeof(ss_code), in_val[pos++], CMUTIL_SUPS_MAX_SC_LEN );
    (void)memscpy( sia, sizeof(sia), in_val[pos++], CMUTIL_SUPS_MAX_SIA_LEN );
    (void)memscpy( sib, sizeof(sib), in_val[pos++], CMUTIL_SUPS_MAX_SIB_LEN );
    (void)memscpy( sic, sizeof(sic), in_val[pos++], CMUTIL_SUPS_MAX_SIC_LEN );

  /* by now SIA, SIB, SIC fields must have been separated out */
  /* STEP 3: Convert SC to network SC and update the
             sups_user_data field
  */
  {
    byte n = 0;
    while( n < CMUTIL_SUPS_SC_MAX_ENTRY )
    {
      if( !strncmp( cmutil_sups_sc_conversion_table[n].local_sc,
            ss_code, CMUTIL_SUPS_MAX_SC_LEN ) )
      {
        sups_start_ptr->ss_code =
             (ss_operation_code_T)cmutil_sups_sc_conversion_table[n].net_sc;
        break;
      }
      n++;
    }
    if (n >= CMUTIL_SUPS_SC_MAX_ENTRY)
    {
      ss_match = FALSE;
    }
  }

  /*Special Case - Only Interrogation and Deactivation are allowed on CCBS*/
   if(sups_start_ptr->ss_code == ccbs &&
     sups_start_ptr->ss_operation != CM_SUPS_CMD_INTERROGATE &&
     sups_start_ptr->ss_operation != CM_SUPS_CMD_DEACTIVATE)
   {
     ss_match = FALSE;
   }


   /* by now the translation of SC must be complete */
  /* STEP 4: Check for BSG . This may be in SIA, SIB or SIC */
  if (ss_match)
  {
    /*lint -e655 */
    /* 655: bit-wise operation uses (compatible) enum's */
    if( (sups_start_ptr->ss_operation != CM_SUPS_CMD_REG_PASSWORD)  &&
        ((((ss_operation_code_T)
           (sups_start_ptr->ss_code & MOST_SIG_NIBBLE)) == allForwardingSS )\
           || \
            /*lint !e641 */
          (((ss_operation_code_T)
           (sups_start_ptr->ss_code & MOST_SIG_NIBBLE)) == \
                                                 allCallRestrictionSS)
            /*lint !e641 */
         )
      )
    {
      /* BSG is in SIB for call forwarding and restriction*/
      if(strlcpy( bsg, sib, sizeof(bsg)) >= sizeof(bsg))
      {
        CM_ERR_0("Wrong sib received");
      }
    }
    else if( sups_start_ptr->ss_code == cw )
    {
      /* BSG is in  SIA for Call Waiting*/
      if(strlcpy( bsg, sia, sizeof(bsg)) >= sizeof(bsg))
      {
        CM_ERR_0("Wrong sia received");
      }	  
    }
    else
    {
      bsg[0] = '\0';
    }

    /* by now BSG must be identified */
    /* STEP 5: Convert BSG to net BSG */
    sups_start_ptr->basic_service = CMUTIL_SUPS_BSG_ALL_SERVICES;

    if ( sups_start_ptr->ss_operation != CM_SUPS_CMD_REG_PASSWORD)
    {
      /* special case */
       /*  BSG - Basic Service Group :
       BS  = Bearer Service
       TS  = Tele Service
       */
      byte n = 0;
      while( n < CMUTIL_SUPS_BSG_MAX_ENTRY )
      {
        if( !strncmp( cmutil_sups_bsg_conversion_table[n].local_bsg,
                bsg, CMUTIL_SUPS_MAX_BSG_LEN ) )
        {
          sups_start_ptr->basic_service  =
           (cmutil_sups_basic_service_e_type)
            cmutil_sups_bsg_conversion_table[n].net_bsg_code;
          break;
        }
        n++;
      }
      if(n == CMUTIL_SUPS_BSG_MAX_ENTRY  )
      {
        ss_match = FALSE;
      }
    }
  }

  /* STEP 6:  Convert DN, if any */
  if ( ss_match)
  {
    /*lint -e655 */
    /* 655: bit-wise operation uses (compatible) enum's */
    if ( (((ss_operation_code_T)
            sups_start_ptr->ss_code & MOST_SIG_NIBBLE)) == allForwardingSS )
            /*lint !e641 */
    {
      if (sups_start_ptr->ss_operation == CM_SUPS_CMD_REGISTER && \
                                                       sia[0] == '\0')
      {
        return CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_ILLEGAL_ENTRY;
      }

      if (sia[0] != '\0' &&
          (sups_start_ptr->ss_operation == CM_SUPS_CMD_REGISTER ||
           sups_start_ptr->ss_operation == CM_SUPS_CMD_ACTIVATE))
      {
        uint32 sia_len = strlen(sia);
        sups_start_ptr->req.reg.len = (uint32) \
                                      MIN(sia_len,CM_MAX_NUMBER_CHARS);

        if(strlcpy(sups_start_ptr->req.reg.data,
                   sia, sizeof(sups_start_ptr->req.reg.data))
                                      >= sizeof(sups_start_ptr->req.reg.data))
        {
            CM_ERR_0("Wrong sia received");
        }
      }
        /* else the length of the DN will be 0 */
    }
  }

  /* STEP 7: Check is activation is also registration?
  if registration data is not null or timer value is non zero it
  means we have to treat ** and * both as REGISTER*/
  if( sups_start_ptr->ss_operation == CM_SUPS_CMD_ACTIVATE )
  {
    if((sups_start_ptr->req.reg.len > 0) || \
          (sups_start_ptr->req.reg.nr_timer != 0))
    {
      sups_start_ptr->ss_operation = CM_SUPS_CMD_REGISTER;
    }
  }

  /* STEP 8: Set the NR timer */
  if( (sups_start_ptr->ss_operation == CM_SUPS_CMD_REGISTER )
                &&
      ( ( sups_start_ptr->ss_code == allForwardingSS ) ||
        ( sups_start_ptr->ss_code == allCondForwardingSS ) ||
        ( sups_start_ptr->ss_code == cfnry ) ) )
  {

    if( sic[0] != '\0' )
    {
      sups_start_ptr->req.reg.nr_timer = (uint8)atoi(sic);
    }
    else
    {
      sups_start_ptr->req.reg.nr_timer = 0;
    }
  }

  /* STEP 9: copy the password */
  if ( ss_match)
  {
    /*lint !e641 */
    if( (  (sups_start_ptr->ss_code & MOST_SIG_NIBBLE) == \
                 allCallRestrictionSS)
            /*lint !e641 */
            &&
           (sups_start_ptr->ss_operation != CM_SUPS_CMD_REG_PASSWORD) )
    {
      (void)strlcpy(sups_start_ptr->req.passwd.ss_password,
                    sia,
                    sizeof(sups_start_ptr->req.passwd.ss_password));

    }
    else if(sups_start_ptr->ss_code  == ccbs)
    {
     #ifdef FEATURE_CCBS
      if( sia[0] == '\0' )
      {
        sups_start_ptr->req.ccbsIndex = CM_CCBS_INDX_INVALID;
      }
      else
      {
        sups_start_ptr->req.ccbsIndex = (uint8)atoi(sia);
      }
      #else
        ss_match = FALSE;
      #endif
    }
  }

  /* copy old_pwd, new_pwd, new_pwd_again */
  if( sups_start_ptr->ss_operation == CM_SUPS_CMD_REG_PASSWORD)
  {
    if((sia[0] == '\0' || sib[0] == '\0' || sic[0] == '\0' ))
    {
       ss_match = FALSE;
    }
    else
    {
      if(strlcpy(sups_start_ptr->req.passwd.ss_password,
                       sia,
                 sizeof(sups_start_ptr->req.passwd.ss_password))
                 >= sizeof(sups_start_ptr->req.passwd.ss_password))
      {
        CM_ERR_0("Wrong sia received");        
      }


      /*New Passwd*/
      if(strlcpy(sups_start_ptr->req.passwd.ss_new_password,
                        sib,
                 sizeof(sups_start_ptr->req.passwd.ss_new_password))
                 >= sizeof(sups_start_ptr->req.passwd.ss_new_password))
      {
        CM_ERR_0("Wrong sib received");        
      }	  

      /*New Passwd Again*/
      if(strlcpy(sups_start_ptr->req.passwd.ss_new_password_again,
                        sic,
                 sizeof(sups_start_ptr->req.passwd.ss_new_password_again))
                 >= sizeof(sups_start_ptr->req.passwd.ss_new_password_again))
      {
        CM_ERR_0("Wrong sic received");        
      }	  
    }
  }
  }

  if ( !ss_match)
  /* check if it is an Unstructured SS data */
  {
    if( ( str_len <= MAX_USS_CHAR_ASCII ) &&
        ( ss_str_ptr[str_len - 1] == '#' ) )
    {
      sups_start_ptr->ss_operation = CM_SUPS_CMD_PROCESS_USS;
      ss_match = TRUE;

      (void)strlcpy (sups_start_ptr->req.ussd.data,
                          ss_str_ptr ,
                          (str_len+ 1) );
      sups_start_ptr->req.ussd.length = str_len;
    }
  }

  if((ss_str_ptr[str_len -1] != '#') || (!ss_match))
  {
    CM_ERR_0 (" Invalid Sups string");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_ILLEGAL_ENTRY;
  }

  return CMUTIL_PARSE_SUPS_RESULT_SUCCESS;
}
/*lint +esym(818, tokens_ptr) */

/*===========================================================================

FUNCTION: cmutil_reg_exp_chars_match

DESCRIPTION
  If the match char is special char then a range of digits are matched.
  otherwise just the char is matched against the input char.

DEPENDENCIES
  None.

RETURN VALUE

  Returns the result of the character match


SIDE EFFECTS
  None
===========================================================================*/
boolean cmutil_reg_exp_chars_match(

  boolean special_char,

  char match_char,

  char src
)
{
  if (special_char)     /* Do we treat this special */
  {
    switch (match_char)
    {
      case 'd': /* Currently, we only handle digits as special characters */
        return (src >= '0' && src <= '9');
      default:
        CM_ERR_0 (" Unknown special character in regex");
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

FUNCTION: cmutil_regex_to_match

DESCRIPTION
  Matches the part of the string against the given regex

DEPENDENCIES
  None.

RETURN VALUE

  Returns the result of the string match with regex


SIDE EFFECTS
  None
===========================================================================*/
boolean cmutil_regex_to_match(

  char **src_ptr,
  /*part of string to match*/

  char **regex_ptr
  /*Regex to compare with*/
)
{
  boolean special_char = FALSE; /* Are we processing a special character */
  boolean multi_char = FALSE;  /* Does this token have multiple characters */
  boolean choice_exists = FALSE; /* Is there a choice of characters */
  int min_num_chars_to_match;   /* Number of characters necessary (0 or 1) */
  char match_char;              /* The character we are trying to match. */
  char *start_of_regex_ptr = *regex_ptr; /* Save start of current regex */
  char *end_reg_ptr;            /* Save the end of the current regex. */
  char *tmp_reg_ptr;            /* Work variable. */
  int num_chars_matched = 0;    /* How long is the matching string. */

  end_reg_ptr = start_of_regex_ptr+1; /* End initially is the start + 1 */
  switch (*start_of_regex_ptr)
  {
    case '[':                   /* We are going to have a choice. */
      choice_exists = TRUE;
      start_of_regex_ptr++;         /* The characters to match start next */

      /* Find the end of the regular expression */
      while ((*end_reg_ptr != ']') && (*end_reg_ptr != '\0'))
      {
        end_reg_ptr++;
      }

      if (*end_reg_ptr != ']')
      {
        CM_ERR_0(" Error in regular expression; no closing ! ");
        return FALSE;
      }

      end_reg_ptr++;  /* End points to either the start of the next or the
                         repeat character */
      break;
    case '\\':          /* Escaped character? */
      end_reg_ptr++;  /* The regular expression takes an extra character. */
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
  }

  tmp_reg_ptr = start_of_regex_ptr;         /* Initialize work variable */
  do {
    match_char = *tmp_reg_ptr;        /* We are searching for this char */

    /* Check for escaped characters.*/
    if (match_char == '\\')             /* Unless it is escaped. */
    {
      match_char = *++tmp_reg_ptr;    /* Get the char after the backslash */
      switch (match_char)
      {
        case 'd':                    /* D is the only special char we know */
          special_char = TRUE;
          break;
        default:
          special_char = FALSE;
          break;
      }
    }

    /* Now check to see if the match_char from the regular expression matches
     * the current character in the input string. */
    if (cmutil_reg_exp_chars_match(special_char, match_char,
          *((*src_ptr) + num_chars_matched)))
    {
      /* We have a match, mark that we did. */
      num_chars_matched++;
      tmp_reg_ptr = start_of_regex_ptr;
    }
    else
    {
      /* It doesn't match.  Either we are done matching to this regular
       * expression or we have more choices. */
      if (choice_exists)
      {
        tmp_reg_ptr++;
        if (*tmp_reg_ptr == ']') /* No more choices, we are done. */
          break;
      }
      else
        break;
    }
  } while (multi_char || choice_exists);

  /* Now to see if we are actually matching what was specified. */
  if (num_chars_matched >= min_num_chars_to_match)
  {
    /* Move things beyond this regular expression */
    *src_ptr += num_chars_matched;
    *regex_ptr = end_reg_ptr;
    return TRUE;
  }
  else
  {
    /* no match.  */
    return FALSE;
  }
}


/*===========================================================================

FUNCTION: cmutil_tokenize_sups_string

DESCRIPTION
  Tokenize the sups string and invoke the function to fill the common
  sups structure. For instance a Supplementary service control string for
  interrogate will be of the form: *#SC*SI#. SI is the Supplementary
  Information and can be entered in any of the following formats:
  *SIA*SIB*SIC#
  *SIA*SIB#
  *SIA**SIC#
  *SIA#
  **SIB*SIC#
  **SIB#
  ***SIC#
  #
  where SIA, SIB, SIC are components of SI.


DEPENDENCIES
  None.

RETURN VALUE

  Returns a filled common structure and an error value of type
  cmutil_parse_sups_result_e_type


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type cmutil_tokenize_sups_string(

  char *in_str_ptr,
    /*Input string*/

  CMUTIL_TOKEN_INPUT *inputs_ptr,
    /*Regex input*/

  void *sups_start_ptr
    /*Generic structure to be filled after tokenize*/
)
{
  cmutil_parse_sups_result_e_type ret =
                        CMUTIL_PARSE_SUPS_RESULT_ERR_REGEX_NO_MATCH;
  int ntokens;          /* Number of tokens we have saved. */
  char *current_in_ptr;     /* Current place in the input string. */
  char *current_regex_ptr;  /* Current place of the regular expression */
  CMUTIL_TOKEN_INPUT *curr_ptr = inputs_ptr; /* Current regular express */
  boolean found_match = FALSE; /* Used to loop across multiple reg_exps */
  boolean optional_match = FALSE; /* Marks this part of regex as optional */
  boolean do_save = FALSE; /* Should we save this if it matches. */
  char *tmp_ptr;        /* Work variable when we may need to save it. */
  const void *tokens_ptr[CMUTIL_SUPS_MAX_TOKENS];

  if(in_str_ptr == NULL)
  {
    CM_ERR_0 (" input string pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(inputs_ptr == NULL)
  {
    CM_ERR_0 (" token inputs pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  if(sups_start_ptr == NULL)
  {
    CM_ERR_0 (" Sups start pointer passed is NULL");
    return CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  }

  /* While we need to search and haven't found it. */
  while ((found_match == FALSE) && (curr_ptr->reg_exp != NULL))
  {
    /* New expression, init our variables. */
    current_in_ptr = in_str_ptr;           /* Start at beginning of source */
    current_regex_ptr = curr_ptr->reg_exp; /* Start at beginning of regexp */
    found_match = TRUE;                 /* Assume we will match. */
    ntokens = 0;                        /* We haven't saved anything yet. */
    optional_match = FALSE;
    do_save = FALSE;

    /* While we are looking good. */
    while ((found_match) && (*current_regex_ptr != '\0'))
    {
      switch (*current_regex_ptr) /* Check if we handle the reg ex */
      {
        case '{':               /* this portion of the string is optional */
          optional_match = TRUE;
          current_regex_ptr++;
          break;
        case '}':               /* end the string is optional */
          optional_match = FALSE;
          current_regex_ptr++;
          break;
        case '(':               /* Save the current token. */
          do_save = TRUE;
          current_regex_ptr++;
          break;
        case ')':               /* Really a no-op since its up to the
            parsing function to mark the end. */
          current_regex_ptr++;
          break;
        default:
          /* We don't handle the character, so see if the token matches */
          tmp_ptr = current_in_ptr;

          found_match = cmutil_regex_to_match(&current_in_ptr, \
                                &current_regex_ptr);

          if (found_match)
          {
            if (do_save)
            {
              tokens_ptr[ntokens++] = (void *) tmp_ptr;
            }
          }
          else if (optional_match)
          {
            /* If we didn't find a match, it may mean we are in an
            optional part of the expression.  Skip to the end of
            the optional part now and resume comparing there.*/
            while ((*current_regex_ptr++ != '}') && \
                   (*current_regex_ptr != '\0'))
            {
              if (*current_regex_ptr == '\\') current_regex_ptr++;
              current_regex_ptr++;
            }
            found_match = TRUE;
          }
          do_save = FALSE;
          break;
      }
    }/*while(found_match && current_regex_ptr)*/

    /* Done checking regular expression... if everything matches... */
    if (found_match && (*current_in_ptr == '\0'))
    {
      /* Everything matched, call the function associated with it. */
      return ((*(curr_ptr->check_fn))(ntokens, tokens_ptr, sups_start_ptr));
    }

    /* We don't have a match, check next regular expression. */
    found_match = FALSE;
    curr_ptr++;
  }

  /*If we are here it means sups string didn't match the regex.*/
  CM_ERR_0 (" Sups String Tokenize Failed! ");
  /* Return an error value indicating that we did not find a match. */
  return ret;

}

/*===========================================================================

FUNCTION: cmutil_parse_sups_string

DESCRIPTION
  Parses call independent supplementary service control string.
  The input sups string passed should fall under one of the following
  categories: Register, Activate, Erase, Interrogate, Deactivate,
  Register Password, USSD.

DEPENDENCIES
  None.

RETURN VALUE

  returns a structure filled with sups params and sups operation of type
  cm_sups_cmd_e_type.


SIDE EFFECTS
  None
===========================================================================*/
cmutil_parse_sups_result_e_type   cmutil_parse_sups_string(

  cm_num_s_type  sups_num,
    /*Input Sups string*/

  uint8  sups_str_len,
    /*Input sups string length*/

  cmutil_sups_params_s_type* sups_params_ptr
    /*Output Sups params*/
)
{
  cmutil_parse_sups_result_e_type ret =
                            CMUTIL_PARSE_SUPS_RESULT_ERR_INVALID_INPUT;
  cmutil_sups_start_s_type sups_start;
  char  *sups_str_ptr = (char*)sups_num.buf;

  if(sups_str_ptr == NULL)
  {
    CM_ERR_0 (" Input Sups String is NULL ");
    return ret;
  }
  if(sups_params_ptr == NULL)
  {
    CM_ERR_0 (" Sups params pointer is NULL ");
    return ret;
  }
  if((sups_str_len > MAX_USS_CHAR_ASCII) || \
      (sups_str_len < CMUTIL_MIN_SUPS_DIGITS))
  {
    CM_ERR_0 (" Invalid Length! ");
    return ret;
  }
  /*check if the sups string is terminated by # and is valid.*/
  if((sups_str_ptr[sups_str_len-1] != '#') || \
      (!ISDIGIT(sups_str_ptr[sups_str_len-2])))
  {
    CM_MSG_HIGH_0 (" Invalid Input Sups String! ");
    return ret;
  }

  /*Initialize the sups start*/
  memset(&sups_start, 0, sizeof(cmutil_sups_start_s_type));
  /*Tokenize the sups string and fill the params in generic sups structure*/
  ret = cmutil_tokenize_sups_string(sups_str_ptr, cmutil_regex_input, \
          &sups_start);

  if(CMUTIL_PARSE_SUPS_RESULT_SUCCESS == ret)
  {
    if((sups_start.ss_operation >= CM_SUPS_CMD_REGISTER)&&
        (sups_start.ss_operation <= CM_SUPS_CMD_REG_PASSWORD))
    {
      /*lint -e{746}*/
      ret = ((*cmutil_sups_fill_fn[sups_start.ss_operation])\
                (sups_params_ptr, &sups_start,sups_num));
    }
    else
    {
      CM_ERR_1(" Unsupported Sups Command type %d! ", \
                       sups_start.ss_operation );
      return ret;
    }
  }
  return ret;
}
#endif /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================

FUNCTION: cm_util_cm_max_emergency_num_count

DESCRIPTION
  This function returns the max value of the cm_max_emergency_num_count

DEPENDENCIES
  FEATURE_GSM ||FEATURE_WCDMA

RETURN VALUE
 maximum no. of emergency count based on definition of feature

SIDE EFFECTS
  None
===========================================================================*/
uint8 cm_util_cm_max_emergency_num_count(void)
{
  #ifdef CM_GW_SUPPORTED
  return CM_MAX_EMERGENCY_NUM_COUNT;
  #else
  return CM_MIN_EMERGENCY_NUM_COUNT;
  #endif
} /* cm_util_cm_max_emergency_num_count() */

/*===========================================================================

FUNCTION: cmutil_num_of_subs_avail

DESCRIPTION
  This function returns the number of subscription available

DEPENDENCIES
  NONE

RETURN VALUE
  number of subs avail

SIDE EFFECTS
  None
===========================================================================*/
uint8 cmutil_num_of_subs_avail(void)
{
  cmph_s_type      *ph_ptr            = cmph_ptr();
  uint8             num_of_subs_avail = 0;

  if (ph_ptr->is_gwl_subscription_available || ph_ptr->is_cdma_subscription_available)
  {
    num_of_subs_avail++;
  }

  if (ph_ptr->is_hybr_gw_subscription_available)
  {
    num_of_subs_avail++;
  }

  
  CM_MSG_HIGH_4("oprt_mode_online: num_of_subs_avail %d is_gwl_sub_avail %d is_cdma_sub_avail %d, is_hybr_gw_sub_avail %d",
    num_of_subs_avail,
    ph_ptr->is_gwl_subscription_available,
    ph_ptr->is_cdma_subscription_available, 
    ph_ptr->is_hybr_gw_subscription_available);
    
  return num_of_subs_avail;
} /* cm_util_num_of_subs_avail() */

/*===========================================================================

FUNCTION: cmutil_all_subs_avail

DESCRIPTION
  This function returns true if all subscription is available

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean cmutil_all_subs_avail(void)
{
  sys_modem_device_mode_e_type device_mode = cmpmprx_get_device_mode();
  uint8             num_of_subs_avail = cmutil_num_of_subs_avail();
  
  CM_MSG_HIGH_2("oprt_mode_online: num_of_subs_avail %d, device_mode %d",
    num_of_subs_avail, device_mode);
    
  switch(device_mode)
  {
    case SYS_MODEM_DEVICE_MODE_SINGLE_SIM:
      if (num_of_subs_avail == 1)
      {
        return TRUE;
      }
      break;
    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY:
    case SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE:
      if (num_of_subs_avail == 2)
      {
        return TRUE;
      }
      break;
    default:
     CM_ERR_1("Incorrect device_mode configuration, device_mode %d",device_mode);
  }

  return FALSE;
} /* cm_util_num_of_subs_avail() */

/*===========================================================================

FUNCTION: cmutil_all_card_avail

DESCRIPTION
  This function returns true if all cards are available or not

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean cmutil_all_card_avail(void)
{
  uint8                          num_of_subs_avail = cmutil_num_of_subs_avail();
  uint16                         total_app_count  = 0;
  mmgsdi_available_apps_type     card_info;

  cmmmgsdi_get_card_info(&card_info);
  CM_MSG_HIGH_2("oprt_mode_online: cmutil_all_card_avail() card_info.card1_num_app %d, card_info.card2_num_app %d",
      card_info.card1_num_app, card_info.card2_num_app);

  if (card_info.card1_num_app)
  {
    total_app_count++;
  }
  if (card_info.card2_num_app)
  {
    total_app_count++;
  }

  CM_MSG_HIGH_2("oprt_mode_online: cmutil_all_card_avail() num_of_subs_avail %d, total_app_count %d",
    num_of_subs_avail, total_app_count);
  if (num_of_subs_avail == total_app_count)
  {
    return TRUE;
  }
  return FALSE;
} /* cmutil_all_card_avail() */

/*===========================================================================
FUNCTION cmutil_copy_char_array_to_str

DESCRIPTION
To copy a character array to a string that need to be terminated wiht '\0'

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
void cmutil_copy_char_array_to_str(char *pDest, size_t dest_size, char const *pSrc, size_t src_len)
{
  if(pSrc != NULL && pDest != NULL && src_len != 0 && dest_size != 0)
  {
    memscpy(pDest, dest_size-1, pSrc, src_len);
    pDest[MIN((dest_size - 1), src_len)] = '\0';
  }
}

/*===========================================================================

FUNCTION cmutil_read_policyman_ueMode

DESCRIPTION
  This function read the UE mode from policymanager


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
sys_ue_mode_e_type cmutil_read_policyman_ueMode( void )
{
  const policyman_item_t *pItem = NULL;
  const policyman_item_id_t id = POLICYMAN_ITEM_UE_MODE;
  sys_ue_mode_e_type   ueMode = SYS_UE_MODE_NONE;

  if(POLICYMAN_SUCCEEDED(policyman_get_items_block(&id, 1, &pItem)))
  {
    if(!POLICYMAN_SUCCEEDED(policyman_get_ue_mode(pItem,&ueMode)))
    {
      ueMode = SYS_UE_MODE_NONE;      
    }
  }
  
  policyman_item_release(pItem);

  return ueMode;
}

/*===========================================================================

FUNCTION cm_is_esr_allowed_in_srlte

DESCRIPTION
  This function returns if ESR can be sent ,
  while UE operating in 1XSRLTE mode .


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_esr_allowed_in_srlte( void )
{
  cmsds_s_type *sds_ptr = cmsds_ptr();
  if((sds_ptr->is_esr_supported || sds_ptr->supportfordualrxues)
     && cmph_is_oprting_in_1xsrlte_mode(SYS_MODEM_AS_ID_1)
#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))     
     && cmregprx_get_stack_info_ptr(SD_SS_HYBR_2)->state == CMREGPRX_STATE_ACTIVE    
#endif
     && cmss_ptr()->info.gw_srv_status == SYS_SRV_STATUS_SRV)
  {
    return TRUE;      
  }
  else
  {
    return FALSE;      
  }
}

/*===========================================================================

FUNCTION: cm_util_get_curr_systime

DESCRIPTION
  This function returns the current time with reference to start of time,
  sicne EPOCH by using API time_genoff_opr().
  However, time returned by it cannot be altered by network ovet the air.

DEPENDENCIES
  None

RETURN VALUE
 current system time

SIDE EFFECTS
  None
===========================================================================*/
uint32 cm_util_get_curr_systime(void)
{
  time_genoff_args_struct_type time_genoff_args;
  time_type		ts_val_secs; 
  uint32 		secs; 
  time_bases_type       time_base  = ATS_RTC;


  /** read time from time service */
  time_genoff_args.base        = time_base; 
  time_genoff_args.base_source = TIME_SCLK;
  time_genoff_args.ts_val      = (void *)&ts_val_secs;
  time_genoff_args.unit        = TIME_SECS;
  time_genoff_args.operation   = T_GET;

  time_genoff_opr( &time_genoff_args );
  /** time_genoff_opr returns time in secs in field time_genoff_args.ts_val, which
  ** is pointed to by local variable ts_val_secs. Hence, extract time in a temp_var-secs
  ** return to caller 
  */  
  secs  = ts_val_secs[0];

  return secs;
}

/*===========================================================================

FUNCTION cm_data_high_priority_sub

DESCRIPTION
  This function returns if ESR can be sent ,
  while UE operating in 1XSRLTE mode .


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_data_high_priority_sub( void )
{
  cmph_s_type      *ph_ptr            = cmph_ptr();
  short loop =SYS_MODEM_AS_ID_1;

  for(loop=SYS_MODEM_AS_ID_1; loop<=SYS_MODEM_AS_ID_3;loop++) 
  {
	if(ph_ptr->data_priority_per_sub[loop].priority_type == SYS_DATA_PRIORITY_HIGH)
	{
     CM_MSG_HIGH_0("SUBS with HIGH data priority Already Exsists");
	 return TRUE;
	}
  }
    return FALSE;      
}

/*===========================================================================

FUNCTION cmcall_is_there_vowlan_call_or_drvcc_in_prog

DESCRIPTION
  Tells if there is active VoWlan call or DRVCC in progress.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : if Vowlan call is in progress or DRVCC is in progress
  FALSE  otherwise
  
SIDE EFFECTS
  none

===========================================================================*/
boolean cm_is_there_vowlan_call_or_drvcc_in_prog(void)
{
  boolean ret_val = FALSE;

  if(cmcall_is_there_vowlan_call() != CM_CALL_ID_INVALID)
  {
    ret_val = TRUE;
  }
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  if(cmcall_drvcc_cache_get_drvcc_state() == CM_DRVCC_STATE_SETUP_IN_PROG)
  {
    ret_val = TRUE;
  }
  #endif
  CM_MSG_HIGH_1("vowlan_call_or_drvcc_in_prog returns %d",ret_val);
  return ret_val;
}
