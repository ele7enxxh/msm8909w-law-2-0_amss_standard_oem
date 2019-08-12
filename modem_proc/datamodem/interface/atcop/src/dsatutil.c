/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D
                        
                        U T I L I T Y   S E R V I C E S

GENERAL DESCRIPTION
  This module contains utility functions that are used in executing/processing
  AT commands.

EXTERNALIZED FUNCTIONS
  dsatutil_fmt_num_val
    This function formats the result of a value inquiry "?" for a normal
    numeric variable.  The return string is a null terminated 3 - digit
    decimal number with leading zeros.

  dsatutil_fmt_num_val_ex
    This function formats the value of an extended format numeric
    variable.  The value may be formatted as the result of a READ
    "?" command, or in the form of a SET command for use in uploading
    the configuration.

   dsatutil_fmt_mixed_param_val
    This function formats the values of a mixed parameter.
    The values may be formatted as the result of a READ "?" command,
    or in the form of a SET command for use in uploading the configuration.

  dsatutil_fmt_num_range
    This function formats the result of a range inquiry "=?" command
    for a numeric variable.

  dsatutil_fmt_string_range
    This function formats the result of a range inquiry "=?" for a string
    variable.

  dsatutil_fmt_string_val
    This function formats the value of an extended format string
    variable.  The value may be formatted as the result of a READ
    "?" command, or in the form of a SET command for use in uploading
    the configuration.

  dsatutil_put_num_arg
    This function checks the value and number of numeric arguments for
    a command, and, if they are correct, stores the value(s) in the
    parameter.

  dsatutil_put_string_arg
    This checks an extended format string WRITE command for correct
    syntax and string length.

  dsatutil_strip_quotes_out
    This function strips the quotes out from the quoted_str, the results
    are in out_str as a null-terminated string.  str_len is the max length 
    of the out_str string, the applocated space for out_str.

  dsatutil_append_dsm_item
    This function checks the buffer length against the passed threshold
    value.  If the threshold is exceeded, a new DSM item is requested and
    appended to the buffer chain.  An error is generated if no dsm_items
    are available.

  dsatutil_validate_dial_char
    This function examines the passed dial string charater and returns a
    validation result.  It is up to the caller to act upon the result as
    approppriate within its context.

  dsatutil_srch_list
    This function searchs a list of strings, and returns the index
    to the item found.
      
  dsatutil_init_pkt_dial_string_from_nv
    This function reads Packet Dial Sting lookup table from the NV during
    initialization. If these items were never written in NV, default
    values are loaded and stored to NV.

  dsatutil_convert_chset
    This function performs conversion of a string in one character set 
    to another supported character set.

  dsatutil_init_timers
    This function registers timers with the opeating system.

  dsatutil_swapn_word_msb_lsb
    This function is used to swap the msb and lsb of the given word array

  dsatutil_reverse_word_arr
    This function reverses the given word array by the 
    specified number of words

  dsatutil_reversen_byte
    This function reverses the given byte array by the specified 
    number of bytes.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsatutil.c_v   1.7   11 Nov 2002 14:02:48   sbandaru  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatutil.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ------------------------------------------------------------------
06/09/16   skc     Fixed PLMN display issue.
12/18/15   skc     Fixed KW issues
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/26/14   tk      Fixed issues in SUPS events handling.
08/12/14   pg      Modified DS Profile APIs to per subs.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
06/17/14   sc      Fixed KW errors.
06/11/14   sc      Fixed PLMN name issue having '@'.
04/17/14   sc      Fixed ATCoP KW errors.
04/14/14   tk      Replaced __FILE__ with __FILENAME__ in DSM API wrappers.
03/14/14   tk      Optimized debug macros usage in ATCoP.
01/22/14   sc      Fixed compilation errors when FEATURE_SECONDARY_PDP  is undefined.
12/30/13   sc      Fixed static code bugs.
12/30/13   sc      Used cm_user* api instead of sys_sel* api
12/25/13   tk      Enhanced $QCSIMAPP command for better user experience.
11/18/13   sc      Fixed ATCoP KW errors.
11/13/13   sc      Added support for $QCCSGCOPS command.
10/15/13   sc      Fixed compilation errors when FEATURE_WCDMA is removed.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
09/13/13   sc      Fixed KW errors.
08/01/13   sc      Fixed KW errors.
07/16/13   pg      Fixed no ^MODE URC when UE goes out of service and comes back
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
07/01/13   tk      Fixed issue in memory allocation for SGLTE second stack.
04/08/13   tk      Refactored the code in dsatutil_convert_tuple().
01/28/12   sk      SE13 table updated to 26-Nov-2012 version released by GSMA.
01/08/12   sk      Encoding type MMGSDI_EONS_ENC_UCS2_PACKED added.
01/23/13   sk      DS command buffer memory reduction.
11/07/12   sk      SE13:Fixed parenthesis issue & Updated version to 05-OCT-12. 
11/30/12   tk      ATCoP changes for Triton DSDA.
11/09/12   sk      Modified plmn_by_rat api to report name even if RAT not macthes.
10/16/12   sk      uinetwk_network_table updated to latest SE13 list.
08/22/12   sk      Mainlining Nikel Changes.
07/26/12   sb      Fixed KW Errors.
07/25/12   tk      Added new entries in SE13 table as per Carrier requirements.
07/16/12   tk      Added RAT input to lookup SE13 table.
06/01/12   sk      TDS mode changes.
05/18/12   tk      Migrated to MSG 2.0 macros
05/18/12   sk      Fixed ATD command issue.
04/20/12   sk      DSAT_NIKEL featurization changes.
02/20 12   sb      Fixed Compiler Warnings.
02/17/12   sb      Fixed Compiler and KW Warnings.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/25/12   nd      Deprecating API cm_get_pref_srv_type_for_packet_orig.
01/19/12   sk      Feature cleanup.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
02/10/11   ad      Fixed +CGDCONT issue.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
12/07/10   ad      Added Dual IMEI support  for DSDS.
11/22/10   nc      Fixed an unaligned access crash in 'dsatutil_convert_ipaddr'.
10/25/10   ad      Init Changes for DSDS. 
05/31/10   ad      Added support for S7 command.
05/10/10   kk      Mainlining pending GOBI changes.
01/04/10   vk      Removing certain macros from PDP reg API header file
                   that cause problems with autogenerated RPC stub code
11/16/09   ca      Added utility function dsatutil_reversen_byte().
12/15/09   nc      Featurisation changes for LTE.
10/12/09   vk      PDP profile changes for IPv4v6 support
09/22/09   vrk	   Merged LTE changes.
06/29/09   vg	   Replacing MSG_XXX with MSG_SPRINTF_xxx.
06/25/09   vk      Removing use of certain deprecated macros in pdp reg api
05/19/09   vk      Defeaturization of PDP registry for CMI
04/29/09   ua      Fixed Lint Critical Errors.
04/29/09   ua      Fixed compiler warnings. 
02/02/09   bs      Added support for extended IRA table.
12/29/08   nc      Added support for +VTS Command
11/28/08   cs      Off-Target build Compiler and Lint warning fixes.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/03/08   bs      Fixed Alignment issues for MDM8200.
07/16/08   ua      Added utility function dsatutil_strncmp_ig_sp_case
07/24/08   nc      Added util functions dsatutil_swap_msb_lsb_word_arr() and
                   dsatutil_reverse_word_arr()
03/24/08   bs      Fixed Klocwork criticals.
11/24/07   ua      Fixed Lint-lows. 
07/05/07   ss      Full UCS2 character support for phone book commands.
05/02/07   ua      Fixed lint medium errors. 
03/13/07   sa      Correcting the usage of strlcpy.
02/26/07   ua      Correcting Lint fixes. 
02/15/07   ss      Fixed lint high errors
02/14/07   ph      Multiple out-of-bound checks
02/14/07   pkp     Fixed Klocwork (NULL dereference) errors.
02/13/07   pkp     Fixed RVCT compiler warnings.
01/30/07   ss      Replaced banned string API calls.
01/29/07   ua      Added support for GSM 7bit '@' character (0x00).
06/28/06   au      Correct PS data feature wrappings.
03/30/06   snb     Correct PS data feature wrappings.
02/28/06   snb     Fix +CGTFT TOS/Mask conversion.
01/12/06   snb     Support PBM 8bit alphabet conversions.
01/06/06   snb     Correctly NULL terminate password on reset.
12/08/05   snb     Add IPV6 support and Lint corrections
11/09/05   snb     Lint errors.
10/05/05   snb     Add timer to wait for dsm items to be freed by USB.
04/20/05    sb     Library function to send URC.
02/07/05   tkk     Renamed function "is_date_valid" according to the coding 
                   conventions.
12/29/04   snb     Further changes to allow use of character set conversion 
                   function with 8bit DCS SMSes and fix for packet dial string.
11/22/04    rr     Set higher order byte of profile number to DS_UMTS_PDP_
                   ATCOP_PROFILE_FAMILY to access profiles from 
                   "pdp_profile/atcop_profiles" directory
11/18/04   snb     Add support for GSM and UCS2 character sets to +CSCS command
10/27/04   tkk     Added dsatutil_strtok/is_date_valid/is_leap_year for +CCLK
                   command processing.  
10/21/04   snb     Correct parsing of packet dial strings containing
                   multiple CIDs.
10/18/04   ar      Add generic timer support
04/09/04   snb     Fix for problem with dsat_pkt_dial_string_table size.
11/19/03   ar      Added dsatutil_convert_tuple() routine, replaces
                   obsolete dsatutil_format_ipaddr().
07/09/03   ar      Use DSMGR mode for pkt dial str lookup if CM mode invalid.
06/12/03   ar      Add packet dial string lookup table support. Some cleanup.
04/17/03   sb      Allow 0-1 options in *98 dial string  and 0-3 options in 
                   *99 dial string.
04/02/03   ar      Correct null parameter handling for BROKEN_RANGE attribute
03/26/03   ar      Fix formatting in diagnostic messages
03/24/03   ar      Enforce cid option position on packet dial string
03/17/03   ar      Correct test response formatting for BROKEN_RANGE logic.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/20/03   wx      Return (x) instead of (x-x) for test command when a
                   command's numerical range is only one value
02/20/03   wx      Accept leading 0's for numerical arguments
01/24.03   ar      Migrated context cmds to persistent PDP profile support 
01/20/03   ar      Wrapped dsat_cgdcont_contexts under ETSI Packet FEATURE
01/17/03   wx      Added dsatutil_fill_modifier_info()
12/31/02   wx      Add function dsatutil_strcmp_pkt_dial_str and 
                   dsatutil_check_opt_dial_str
01/07/03   wx      Import dsat707_pkt_orig_str
11/11/02   sb      Definition of dsatutil_srch_list() function
09/18/02   jay     Removed casting to word in calls to itoa.
09/09/02   ar      Added dsatutil_validate_dial_char routine
06/27/02   ar      Added dsatutil_append_dsm_item routine
02/15/02   ar      Added dsatutil_format_ipaddr routine
10/09/01   wx      Modify function dsatutil_put_string_list_arg so that it
                   supports double quoted arguments
08/10/01   sjd     Initial release to MSM5200 archives.
05/15/01   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */
#include "dstaski.h"
#include "nv.h"
#include "dsati.h"
#include "err.h"
#include "msg.h"
#include "amssassert.h"
#include "ps_utils.h"
#include <string.h>
#include <stdio.h>
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatvoice.h"
#include "ds3gmgr.h"

#include "dsatact.h"

#ifdef FEATURE_DATA_IS707 
#include  "dsat707extctab.h"
#include  "dsat707faxctab.h"
#include  "dsat707hdrctab.h"
#include  "dsat707mipctab.h"
#include  "dsat707pstatsctab.h"
#include  "dsat707smsctab.h"
#include  "dsat707util.h"
#include  "dsat707vendctab.h"
#endif /* FEATURE_DATA_IS707   */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsicmif.h"
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_MODE */
#include "dsat707mip.h"
#include "dsatetsipkt.h"
#include "dsatvend.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/

/*-------------------------------------
   Packet Dial String lookup table 
---------------------------------------*/
LOCAL dsati_pkt_dial_string_table_s_type dsat_pkt_dial_string_table[DSAT_SYS_MODEM_AS_ID_MAX];

LOCAL boolean pkt_mode_override;

/*-------------------------------------
  Table to convert IRA characters to GSM 7 bit default alphabets
  Refer to 3G TS 27.005 Annex A for more details                 
---------------------------------------*/
const uint8 ira_to_gsm7[256] =
{
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  10, 255, 255,  13, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  32,  33,  34,  35,   2,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  32,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 255, 255, 255, 255,  17,
  255,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 64, 255, 1, 36, 3, 255, 95, 255, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 96,
  255, 255, 255, 255, 91, 14, 28, 9, 255, 31, 255, 255, 255, 255, 255, 255,
  255, 93, 255, 255, 255, 255, 92, 255, 11, 255, 255, 255, 94, 255, 255, 30,
  127, 255, 255, 255, 123, 15, 29, 255, 4, 5, 255, 255, 7, 255, 255, 255,
  255, 125, 8, 255, 255, 255, 124, 255, 12, 6, 255, 255, 126, 255, 255, 255
};

/*-------------------------------------
  Table to convert GSM 7 bit default alphabets to IRA characters 
---------------------------------------*/
const uint8 gsm7_to_ira[128] =
{
  64, 163,  36, 165, 232, 233, 249, 236, 242, 199,  10, 216, 248,  13, 197, 229,
  255,  95, 255, 255, 255, 255, 255, 255, 255, 255, 255, 32, 198, 230, 223, 201,
  32,  33,  34,  35, 164,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  161,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 196, 214, 209,  220, 167,
  191,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 228, 246, 241, 252, 224
};

/* Escape character to enter GSM alphabet extension table of 23.038 */
#define GSM_EXT_TBL_ESC_CHAR 0x1B
/* minimum and maximum acceptable value for characters in these alphabets:
   0 value cannot be accepted for NULL terminated strings used throughout
   ATCOP and 0x7F is max possible in 7 bit alphabet */
#define MIN_IRA_GSM_CHAR 0x01
#define MAX_GSM_CHAR     0x7F
#define MAX_IRA_CHAR     0xFF
#define MAX_8BIT_CHAR    0xFF

LOCAL const char hex_chars[] = "0123456789ABCDEF";

/* Characters in the "C0 control and Basic Latin" code chart of UTF-16 that 
   aren't common to IRA T.50(C0+G0) and GSM 7bit alphabets */
LOCAL const char basic_latin_not_common[] = 
{
  0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0B,0x0C,0x0E,0x0F,0x10,0x11,0x12,
  0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x24,0x40,0x5B,
  0x5C,0x5D,0x5E,0x5F,0x60,0x7B,0x7C,0x7D,0x7E,0x7F,0x00
};

/* Complementary arrays for converting between PBM 8bit and UCS2 hex 4-tuples:
   for those values that aren't common in the range supported by both */
LOCAL const char pbm8_to_ucs2_not_common[] = 
  "\x90\x92\x93\x94\x95\x96\x97\x98\x99\x9A";
LOCAL const byte ucs2_to_pbm8_not_common[][8] = 
{
  "0394", "03A6", "0393", "039B", "03A9", 
  "03A0", "03A8", "03A3", "0398", "039E",
  ""
};
 
LOCAL const byte ucs2_to_pbm8_not_supported[][8] =
{
  "0090", "0092", "0093", "0094", "0095", 
  "0096", "0097", "0098", "0099", "009A",
  ""
};

LOCAL const char pbm8_to_gsm7_exttbl[] = "\x5B\x5C\x5D\x5E\x7B\x7C\x7D\x7E";
LOCAL const char gsm7_to_pbm8_exttbl[] = "\x3C\x2F\x3E\x14\x28\x40\x29\x3D";

dsat_timer_ex_s_type dsatutil_timer_table_ex[] =
{
#ifdef FEATURE_DSAT_ETSI_MODE
   { DSAT_INBAND_DTMF_RING,
     dsatetsicall_inband_dtmf_timer_expired_handler }
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_PS_EAP
  ,{ DSAT_TIMER_CEAP_REPORT_TIMEOUT,
     dsatetsime_ceap_timer_expired_handler }

  ,{ DSAT_TIMER_CEAP_EAP_TIMEOUT,
     dsatetsime_ceap_process_timer_expired_handler }
#endif /* FEATURE_DATA_PS_EAP */

  ,{ DSAT_TIMER_FREE_ITEM,
     dsati_free_item_timer_expired_handler }
};

LOCAL const uint8 dsatutil_timer_table_size_ex =
  sizeof(dsatutil_timer_table_ex) / sizeof(dsat_timer_ex_s_type);

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL byte * itoa_zfill
(
  word v,
  byte *rb_ptr
);

LOCAL action_quote_e_type determine_quote_action(uint32 attrib);

LOCAL boolean check_operating_mode_override
(
  dsati_mode_e_type            *mode_ptr,         /* Mode of Operation */
  nv_operating_mode_enum_type  modes_permitted    /* Allowed modes     */
);

LOCAL void pbm_char_to_gsm
(
  char pbm_in, 
  char **gsm_out
);

LOCAL void gsm_char_to_pbm
(
  char **gsm_in, 
  char *pbm_out
);

LOCAL int is_leap_year
(
  int year
);

LOCAL boolean dsatutil_hex_to_uint8
(
  const char *in_octet,
  uint8      *out_uint8
);

LOCAL boolean dsatutil_hex_to_uint16
(
  const char *in_str,
  uint16     *out_uint16
);

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL

DESCRIPTION
  This function formats the result of a value inquiry "?" for a normal
  numeric variable.  The return string is a null terminated 3 - digit
  decimal number with leading zeros.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_num_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr                          /*  point to buffer for result  */
)
{
  unsigned int result = 0;

  /* Abort formatting if no display attribute set */
  if ((tab_entry->attrib & NO_DISPLAY) == NO_DISPLAY)
  {
    *rb_ptr = '\0';
  }
  else
  {
     dsat_num_item_type *val_ptr = (dsat_num_item_type *)EXEC_VAL_PTR(tab_entry);
     if (NULL != val_ptr)
     {
       result = (unsigned int)( itoa_zfill((word)(0xFF & *(val_ptr)),rb_ptr) - rb_ptr );
     }
  }
  return result;
} /*  dsatutil_fmt_num_val  */

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL_EX

DESCRIPTION
  This function formats the value of an extended format numeric
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <value>                            for a simple variable, or
      +NAME: <value1>, <value2>, ... , <valueN> for a compound variable.

  For the SET command, it is in the form

      +NAME=<value>;                            for a simple variable, or
      +NAME=<value1>, <value2>, ... , <valueN>; for a compound variable.


  The value will be in decimal, unless the parameter is specified with
  a HEX attribute in the parse table entry.  Leading zeros are suppressed.

  ".0" is added to the result for +FCLASS=2.0

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_num_val_ex
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr,                         /*  point to buffer for result  */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
)
{
  unsigned int i, j;
  boolean print_name;
  uint16 radix;
  byte c, *w_ptr;
  const byte *s, *l_ptr;
  boolean skip_fmt = FALSE;
  dsat_num_item_type *val_ptr;

  w_ptr = rb_ptr;
  radix = ( (tab_entry->attrib & HEX) ? 16 : 10);

  print_name = ( ((tab_entry->attrib & FCLASS2P0) == 0)
        && ((tab_entry->special) != SPECIAL_FCLASS))
      || fmt_sw == SET
      || fmt_sw == RESPONSE_ALL;

#if 0
  if (tab_entry->attrib & EXTENDED) /*  Since this function is also */
  {                               /*  used to SET CONFIG for the  */
    *w_ptr++ = '+';                 /*  S-registers.                */
  }
#endif /* if 0 */

  if (print_name)
    {
      s = (const byte *)tab_entry->name;

      while ( (c = *s++) !=0 )
        {
          *w_ptr++ = c;
        }
    }

  switch (fmt_sw)
    {
      case RESPONSE:
      case RESPONSE_ALL:
        if (print_name)
        {
          *w_ptr++ = ':';
          *w_ptr++ = ' ';
        }
        /* Abort further formatting if no display attribute set */
        if ((tab_entry->attrib & NO_DISPLAY) == NO_DISPLAY)
        {
          skip_fmt = TRUE;
        }
        break;

    case SET:
        *w_ptr++ = '=';
        break;

    default:
        /*  ERROR */
        ;
    }
  val_ptr = (dsat_num_item_type *) EXEC_VAL_PTR(tab_entry);
  if ( (NULL != val_ptr) && (!skip_fmt))
  {
    if ((tab_entry->attrib & LIST) != 0)
    {
      for (j=0; j < tab_entry->compound; ++j)
      {
        i = val_ptr[j];
        l_ptr = ((def_list_type *) tab_entry->def_lim_ptr)[j].list_v[i];
      
        if(tab_entry->attrib & YES_QUOTE)
        {
          *w_ptr++ = '"';
        }

        while ( (c = *l_ptr++) != '\0')
        {
          *w_ptr++ = c;
        }

        if(tab_entry->attrib & YES_QUOTE)
        {
          *w_ptr++ = '"';
        }
        *w_ptr++ = ',';
      }
      --w_ptr;
    }
    else if (tab_entry->compound > 1)
    {
      for (j=0; j < tab_entry->compound; ++j)
      {
        w_ptr = dsatutil_itoa(val_ptr[j], w_ptr, radix );
        *w_ptr++ = ',';
      }
      --w_ptr;
    }
    else
    {
      w_ptr = dsatutil_itoa(*val_ptr, w_ptr, radix );
    }
  
    if ( (tab_entry->special) == SPECIAL_FCLASS && *(val_ptr) == 2)
    {
      *w_ptr++ = '.';
      *w_ptr++ = '0';
    }

    if (fmt_sw == SET && tab_entry->attrib & EXTENDED)
    {
      *w_ptr++ = ';';
    }
  }
  *w_ptr = '\0';

  return (unsigned int) (w_ptr - rb_ptr);
} /*  dsatutil_fmt_num_val_ex */

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" command
  for a numeric variable.

  The formatted value is returned as a null terminated string.
DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.


SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_num_range
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr                          /*  point to buffer for result  */
)
{
  unsigned int j;
  unsigned int offset = 0;
  word radix;
  byte c, *w_ptr;
  const byte *r_ptr, *s;
  uint32 brtest = 0;

  w_ptr = rb_ptr;

  radix = ( (tab_entry->attrib & HEX) ? 16 : 10);

  s = (const byte *)tab_entry->name;

  if ((tab_entry->attrib & FCLASS2P0) == 0)
  {
    while ( (c = *s++) !=0 )
    {
      *w_ptr++ = c;
    }

    *w_ptr++ = ':';
    *w_ptr++ = ' ';
  }

  if ( (tab_entry->attrib & LIST) == 0)
  {
    /* Loop over number of parameters */
    for (j=0; j < tab_entry->compound; ++j)
    {
      brtest = (tab_entry->attrib & BROKEN_RANGE);
      *w_ptr++ = '(';

      /* Loop over BROKEN_RANGE segments (if any) */
      do
      {
        /* if the lower and upper are the same value, such as 7
           we should return (7) instead of (7-7) */
        if(((dflm_type *) tab_entry->def_lim_ptr)[j+offset].lower ==
           ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].upper)
        {
          w_ptr = dsatutil_itoa (
                                 ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].lower,
                                 w_ptr,
                                 radix );
        }
        else
        {
          w_ptr = dsatutil_itoa (
                                 ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].lower,
                                 w_ptr,
                                 radix );
          *w_ptr++ = '-';
          w_ptr = dsatutil_itoa (
                                 ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].upper,
                                 w_ptr,
                                 radix );

        }
        if (BROKEN_RANGE == brtest)
        {
          *w_ptr++ = ',';
          offset++;
        }
      } while (brtest &&
               (MAX_BROKEN_RANGE !=
                ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].default_v));

      /* Remove last comma if broken range */
      if (BROKEN_RANGE == brtest)
      {
        w_ptr--;
      }

      *w_ptr++ = ')';
      *w_ptr++ = ',';
    }
    *(w_ptr-1) = '\0';
  }
  else
  {
    for (j=0; j < tab_entry->compound; ++j)
    {
      r_ptr = ((def_list_type *) tab_entry->def_lim_ptr)[j].list_t;
      while ( (c = *r_ptr++) != '\0')
      {
        *w_ptr++ = c;
      }
      *w_ptr++ = ',';
    }
    *(w_ptr-1) = '\0';
  }

  return (unsigned int) (w_ptr - rb_ptr);

} /*  dsatutil_fmt_num_range  */

/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" for a string
  variable.  The function always returns
      +NAME: (20,21,23-7E)
  which is the range of printable ASCII characters.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void dsatutil_fmt_string_range
(
  const dsati_cmd_type *tab_entry,          /*  entry in parse table  */
  byte *rb_ptr,                             /*  pointer to buffer for result  */
  const dsat_string_item_type *range_ptr    /*  pointer to range  */
)
{
  dsat_string_item_type c;
  const dsat_string_item_type *s_ptr;

  if ((tab_entry->attrib & FCLASS2P0) == 0)
  {
    s_ptr = (const byte *)tab_entry->name;

    while ( (c = *s_ptr++) !=0 )
    {
      *rb_ptr++ = c;
    }

    *rb_ptr++ = ':';
    *rb_ptr++ = ' ';
  }

  s_ptr = range_ptr;
  while ( (*rb_ptr++ = *s_ptr++) != 0)
    ;
} /*  dsatutil_fmt_string_range */

/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_VAL

DESCRIPTION
  This function formats the value of an extended format string
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <string>

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_string_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr,                         /*  point to buffer for result  */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
)
{
  dsat_string_item_type c, *w_ptr;
  const dsat_string_item_type *s;
  boolean print_name;
  boolean skip_fmt = FALSE;

  w_ptr = rb_ptr;

  print_name = ( ((tab_entry->attrib & FCLASS2P0) == 0)
        || fmt_sw == SET
        || fmt_sw == RESPONSE_ALL);

  if (print_name)
  {
    s = (const byte *)tab_entry->name;

    while ( (c = *s++) != 0 )
    {
      *w_ptr++ = c;
    }
  }

  switch (fmt_sw)
  {
    case RESPONSE:
    case RESPONSE_ALL:
      if (print_name)
      {
        *w_ptr++ = ':';
        *w_ptr++ = ' ';
      }
      
      /* Abort further formatting if no display attribute set */
      if ((tab_entry->attrib & NO_DISPLAY) == NO_DISPLAY)
      {
        skip_fmt = TRUE;
      }
      break;

    case SET:
      *w_ptr++ = '=';
      break;

    default:
      /*  ERROR */
      ;

  }
    s = (dsat_string_item_type *)EXEC_VAL_PTR(tab_entry);
  if( (NULL != s) && (!skip_fmt) )
  {
    if ((tab_entry->attrib & NO_QUOTE) == 0)
    {
      *w_ptr++ = '"';
    }

    while ( (c = *s++) != 0)
    {
      *w_ptr++ = c;
    }

    if ((tab_entry->attrib & NO_QUOTE) == 0)
    {
      *w_ptr++ = '"';
    }

    if (fmt_sw == SET)
    {
      *w_ptr++ = ';';
    }
  }

  *w_ptr = '\0';

  return (unsigned int) (w_ptr - rb_ptr);
} /*  dsatutil_fmt_string_val */

/*===========================================================================

FUNCTION DSATUTIL_FMT_MIXED_PARAM_VAL

DESCRIPTION
  This function formats the values of a mixed parameter.
  The values may be formatted as the result of a READ "?" command,
  or in the form of a SET command for use in uploading the configuration.
  For the READ command, the returned string is of the form:

      +NAME: <p1>,<p2>,...

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_mixed_param_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  const dsm_item_type *res_buff_ptr,    /*  Place to put response       */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
)
{
  dsat_string_item_type c, *w_ptr;
  const dsat_string_item_type *s;
  boolean print_name;
  int index = 0;
  uint16 buff_len = 0;
  
  dsat_mixed_param_val_type * val_list = 
    (dsat_mixed_param_val_type *) EXEC_VAL_PTR(tab_entry);

  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) tab_entry->def_lim_ptr;

  w_ptr = res_buff_ptr->data_ptr + res_buff_ptr->used;
  buff_len = res_buff_ptr->size - res_buff_ptr->used;
  
  print_name = ( ((tab_entry->attrib & FCLASS2P0) == 0)
                 || fmt_sw == SET
                 || fmt_sw == RESPONSE_ALL);

  if (print_name)
  {
    s = (const byte *)tab_entry->name;

    while ( (c = *s++) != 0 )
    {
      *w_ptr++ = c;
    }
  }

  switch (fmt_sw)
  {
    case RESPONSE:
    case RESPONSE_ALL:
      if (print_name)
      {
        *w_ptr++ = ':';
        *w_ptr++ = ' ';
      }
      break;

    case SET:
      *w_ptr++ = '=';
      break;

    default:
      /*  ERROR */
      ;

  }
  if(NULL != val_list)
  {
  for (index = 0; index < tab_entry->compound; index++)
  {
    /* the index-th parameter is not to be displayed  */
    if( (mixed_def_list[index]->attrib & (NO_DISPLAY)) == 
        (NO_DISPLAY) )
    {
      /* Skip formatting this parameter */
    }
    
    /* the index-th parameter is of type STRING, 
       val_ptr points to its real value */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG | STRING) )
    {
      dsat_string_item_type * val_string = val_list[index].string_item;
        
      if(mixed_def_list[index]->attrib & YES_QUOTE )
      {
         w_ptr += (word) snprintf((char *)w_ptr,buff_len,
                                        "\"%s\",", val_string);
      }
      else
      {
         w_ptr += (word) snprintf((char *)w_ptr,buff_len,
                                        "%s,", val_string);
      }
    }

    /* the index-th parameter is a string within a string list,
         we use val_ptr to index into the string list to get the string */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG | LIST) )
    {
      const dsat_string_item_type * val_string = 
        mixed_def_list[index]->def->def_list.list_v[val_list[index].num_item];
        
      if(mixed_def_list[index]->attrib & YES_QUOTE )
      {
        w_ptr += (word) snprintf((char *)w_ptr,buff_len,
                                       "\"%s\",", val_string);
      }
      else
      {
        w_ptr += (word) snprintf((char *)w_ptr, buff_len,
                                       "%s,", val_string);
      }
    }

    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG) )
    {

      w_ptr += (word) snprintf((char *)w_ptr,buff_len, "%u,",
                                     val_list[index].num_item);
    }

    /* the argument is not anticipated */
    else
    {
      DS_AT_MSG3_ERROR("Mixed parameter setting action %d, %dth param has wrong attrib %d",
          tab_entry->name, index, mixed_def_list[index]->attrib);
      return 0;
    }
  }
  }	

  /* get rid of the last ',' & terminate string */
  *(--w_ptr) = '\0';

  return (unsigned int) (w_ptr - (res_buff_ptr->data_ptr + res_buff_ptr->used));
} /*  dsatutil_fmt_mixed_param_val */

/*===========================================================================

FUNCTION DSATUTIL_PUT_NUM_ARG

DESCRIPTION
  This function checks the value and number of numeric arguments for
  a command, and, if they are correct, stores the value(s) in the
  parameter.

DEPENDENCIES
  If command uses the BROKEN_RANGE attribute, all parameters in the
  default/limits array must be configured with MAX_BROKEN_RANGE delimiters.

RETURN VALUE
  Returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS
  Describe here any side effects that may occur as a result of calling this
  function. This includes such things as global vars that are modified, etc.

===========================================================================*/
boolean dsatutil_put_num_arg
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table        */
  const tokens_struct_type *tok_ptr     /*  Command tokens from parser  */
)
{
  dsat_num_item_type scr_val [MAX_ARG];
  byte flag [MAX_ARG]; /* used to flag that there is match in the val list */
  atoi_enum_type e;
  param_srch_enum_type srchrc;
  word radix;
  uint32 j,i;
  uint32 range_offset = 0;
  uint32 brtest = (tab_entry->attrib & BROKEN_RANGE);
  dsat_num_item_type *val_ptr;

  for (i=0;i<MAX_ARG;i++)
  {
    flag[i]=0;
    scr_val[i]=0;
  }

  radix = ( (tab_entry->attrib & HEX) ? 16 : 10);

  if (tok_ptr->args_found > tab_entry->compound)
  {
    return (FALSE);
  }

  for (j = 0; j < tok_ptr->args_found; ++j)
  {
    flag [j] = 0;
    
    if ((tab_entry->attrib & LIST) == 0)
    {
      e = dsatutil_atoi (&scr_val [j], tok_ptr->arg[j], radix);
      if (e == ATOI_OK)
      {
        /* Adjust for broken ranges */
        i = j + range_offset;

        if (0 == brtest)
        {
          /* Continuous range */
          if (scr_val [j] < ((dflm_type *) tab_entry->def_lim_ptr)[i].lower ||
              scr_val [j] > ((dflm_type *) tab_entry->def_lim_ptr)[i].upper)
          {
            /* The argument is out of range */
            return FALSE;
          }
          else
          {
            flag [j] = 1;
          }
        } 
        else
        {
          /* Discontinuous range so loop over each segment */
          while (MAX_BROKEN_RANGE !=
                 ((dflm_type *) tab_entry->def_lim_ptr)[i].default_v)
          {
            if ((scr_val [j] >= ((dflm_type *) tab_entry->def_lim_ptr)[i].lower) &&
                (scr_val [j] <= ((dflm_type *) tab_entry->def_lim_ptr)[i].upper))
            {
              flag [j] = 1;
            }
            i++;
          }
          /* See if one segment worked */
          if (0 == flag [j])
          {
            /* The argument is out of range */
            return FALSE;
          }
          range_offset = i - j;
        }
      } 
      else if (e == ATOI_OUT_OF_RANGE)
      {
        /* the char code of is out of range */
        return FALSE;
      }
      else if (0 != brtest)
      {
        /* Advance to next discontinuous range segment */
        i = j + range_offset;
        while (MAX_BROKEN_RANGE !=
               ((dflm_type *) tab_entry->def_lim_ptr)[i].default_v) 
        { 
          i++; 
        }
        range_offset = i - j;
      }
      /*else if (e == ATOI_NO_ARG) do nothing: flag[j] remains unset  */
      /*    and the value is not stored                               */
    }
    else  /*  LIST  */
    {
      srchrc = dsatutil_srch_list (
                 &scr_val [j], 
                 ((def_list_type *) tab_entry->def_lim_ptr)[j].list_v,
                 tok_ptr->arg[j],
                 determine_quote_action(tab_entry->attrib) );
      
      /* Continue if paramter detected */
      if (PARAM_NULL != srchrc)
      {
        if (PARAM_HIT == srchrc)
        {
          flag [j] = 1;
        }
        else
        {
          /* not found in list */
          return FALSE;
        }
      }
    }
  }
  
  val_ptr = (dsat_num_item_type *)EXEC_VAL_PTR(tab_entry);

  if(NULL == val_ptr)
  {
    return FALSE;
  }

  for (j = 0; j < tok_ptr->args_found; ++j)
  {
    if (flag[j])
    {
      * (val_ptr + j) = scr_val [j];
    }
  }
  return TRUE;
} /*  dsatutil_put_num_arg  */

/*===========================================================================

FUNCTION DSATUTIL_PUT_STRING_ARG

DESCRIPTION
  This checks an extended format string WRITE command for correct
  syntax and string length.  If everthing is OK, the string is copied
  into the specified parameter.  Otherwise, the global dsat_result" is
  set to ERROR. Non space characters outside of the '"' are considered
  an error.

DEPENDENCIES


RETURN VALUE
  returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS


===========================================================================*/
boolean dsatutil_put_string_arg
(
  const dsati_cmd_type *tab_entry,   /*  entry in parse table        */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  put_string_enum_type action     /*  Store or concatenate  */
)
{
  byte c, *s_ptr, *t_ptr;
  unsigned int arg_cnt, total_cnt;
  unsigned int n_args;

  if ((tok_ptr->args_found != 1) &&         /*  One and only one argument */
      (tab_entry->attrib & NO_QUOTE) == 0)  /*  allowed, unless NOQUOTE   */
  {
     return ( FALSE );
  }

  s_ptr = tok_ptr->arg[0];
  arg_cnt = 0;
  total_cnt = 0;

  if ((tab_entry->attrib & NO_QUOTE) == 0)
  {
    while ( ((c = *s_ptr++) !=0 ) && c != '"')/* No characters except spaces */
    {                                         /* allowed between '=' and '"' */
      if (c != ' ')
      {
         return ( FALSE );
      }
    }

    t_ptr = s_ptr;                        /*  Points to 1st char after '"'*/
    while ( (c = *s_ptr++) != '"')        /*  Count characters inside the */
    {                                     /*  '"'. Non-print not allowed. */
      ++arg_cnt;
      if (arg_cnt > tab_entry->compound || c <' ' || c > '\x7E')
      {
         return ( FALSE );
      }
    }

    total_cnt = arg_cnt;

    while ( (c = *s_ptr++) != 0 )         /*  No trailing characters before */
    {                                     /*  ';' except spaces             */
      if (c != ' ')
      {
          return ( FALSE );
      }
    }
  }
  else                                    /*  NO_QUOTE: count all chars in  */
  {                                       /*  all args to end of command.   */
    n_args = 0;
    t_ptr = s_ptr;
    for ( ;; )
    {
      if ((c = *s_ptr++) == '\0')
      {
        if( (++n_args < tok_ptr->args_found) && ( MAX_ARG > n_args ) )
        {
          s_ptr = tok_ptr->arg[n_args];
          c = ',';
        }
        else
        {
          break;
        }
      }
      ++arg_cnt;
      if (arg_cnt > tab_entry->compound || c <' ' || c > '\x7E')
      {
         return ( FALSE );
      }
    }
  }

  /* Update the parameter value only if non-null string */
  if ( 0 != arg_cnt )
  {
    s_ptr = (dsat_string_item_type *) EXEC_VAL_PTR(tab_entry);
    if( NULL == s_ptr)
      return FALSE;
    
    /*----------------------------------------------------------  
    If concatenate requested, and argument is not a NULL string
    advance the store point to the end of the current value, and
    increment total character count.  Compare count to allowable
    for the parameter.
    
    If concatenate requested, and argument is the NULL string,
    leave pointers alone. This will cause a NULL character to
    be stored in the paramter.
    ----------------------------------------------------------*/
    
    if (action == CAT)
    {
      while (*s_ptr)                      /*  Count current length and add  */
      {                                   /*  to total count                */
        ++s_ptr;
        ++total_cnt;
      }
      if (total_cnt > tab_entry->compound)
      {
        return ( FALSE );
      }
    }
    
    n_args = 0;
    while (arg_cnt--)                   /*  Copy arg to end to parameter  */
    {
      c = *t_ptr++;
      if (c == '\0')
      {
        if ( (++n_args < tok_ptr->args_found) && ( MAX_ARG > n_args ) )
        {
          c = ',';
          t_ptr = tok_ptr->arg[n_args];
        }
      }
      *s_ptr++ = c;
    }
    
    /* Verify string length */
    if (s_ptr >
        ((dsat_string_item_type *) EXEC_VAL_PTR(tab_entry) + tab_entry->compound))
    {
      ERR_FATAL ("OVERFLOW", 0, 0, 0);
    }
    
    *s_ptr = '\0';
  }
  return ( TRUE );

} /*  dsatutil_put_string_arg */

/*===========================================================================

FUNCTION DSATUTIL_MIXED_PARAM_SET_NUM_ARG

DESCRIPTION
  This function checks the value of a string argument for
  a command in the def list.  It determines the string argument's index 
  into the string list.  Use this index to set the val_ptr.

DEPENDENCIES


RETURN VALUE
  returns a param_srch_enum_type:
    PARAM_MISS - Not found in values list 
    PARAM_HIT  - Found in values list            
    PARAM_NULL - No parameter specified          

SIDE EFFECTS
  None

===========================================================================*/
param_srch_enum_type dsatutil_mixed_param_set_num_arg
(
  dsat_num_item_type * val_num_ptr, /* the index number to be returned */
  const def_list_type * def_list_ptr, /* the def list */
  const byte * arg,                   /* the token argument */
  uint32 attrib                       /* the attrib of the MIXED_PARAM */
)
{
  return dsatutil_srch_list(val_num_ptr, 
                   def_list_ptr->list_v,
                   arg,
                   determine_quote_action(attrib));
}


/*===========================================================================

FUNCTION DSATUTIL_XITOA

DESCRIPTION
  Formats an unsigned long to hexadecimal.
  Supplies leading zeros.

DEPENDENCIES

RETURN VALUE
  A pointer to the null terminator is returned.


SIDE EFFECTS

===========================================================================*/
unsigned char * dsatutil_xitoa
(
  unsigned long val,  /*  value to convert  */
  byte * a_ptr        /*  pointer to null terminator of result  */
)
{
  byte c, *s_ptr ;
  int count = 8;
  
  ASSERT(a_ptr != NULL);
  s_ptr = a_ptr + 8;

  *s_ptr-- = '\0';

  while (--count >= 0)
  {
    c = (byte) (val & 0x0F);
    val >>= 4;

    *s_ptr-- = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');
  }

  return a_ptr+8;
} /* dsatutil_xitoa() */


/*===========================================================================
FUNCTION DSATUTIL_STRIP_QUOTES_OUT

DESCRIPTION
  This function strips the quotes out from the quoted_str, the results
  are in out_str as a null-terminated string.  str_len is the max length 
  of the out_str string, the applocated space for out_str.

  The quoted_str is a null terminated string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the action succeeds.
  FALSE if the action fails.
        The reasons will output to ERR.  The reasons could be 
        1. the str_len is less than the length of
           quoted_str -2, ie the out_str is too small to hold the result.
        2. the quoted_str is not quoted

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatutil_strip_quotes_out
( 
  const byte * quoted_str, byte * out_str, word str_len
)
{
  int index;
  boolean end_quote_found = FALSE;

  if(quoted_str[0] != '"')
  {
    /* This error is caused by user input, not a system error.
       Use MSG_MED */
    DS_AT_MSG0_MED("The in_str must be quoted");
    return FALSE;
  }
  
  for (index = 0; 
       (index < str_len) && (quoted_str[index + 1] != '\0') 
         && !end_quote_found; 
       index++)
  {
    if(quoted_str[index + 1] == '"')
    {
      end_quote_found = TRUE;
      out_str[index] = '\0';
    }
    else
    {
      out_str[index] = quoted_str[index + 1];
    }
  }

  if(!end_quote_found)
  {
    /* This error is caused by user input, not a system error.
       Use MSG_MED */
    DS_AT_MSG1_MED("The in_str has opened quotes, or out_str is small: %d",
            str_len);
    return FALSE;
  }
  return TRUE;
} /* dsatutil_strip_quotes_out */

/*===========================================================================
FUNCTION DSATUTIL_SRCH_LIST

DESCRIPTION
  This function searchs a list of strings, and returns the index to the
  item found.  (The first item index is 0).  Index set to NULL if 
  parameter is null, as with empty commas entered, 

  If ignore_quote is true, then srch_list will find quoted args in 
  unquoted lists.  For instance, \"ME\" will find itself as the first 
  item in list ME, MT, SM

DEPENDENCIES
  None

RETURN VALUE
  PARAM_HIT if found.
  PARAM_MISS if no match.
  PARAM_NULL if no parameter specified.

SIDE EFFECTS
  None
===========================================================================*/
param_srch_enum_type dsatutil_srch_list
(
  dsat_num_item_type *val_arg_ptr,  /*  value returned  */
  const byte l[][8],
  const byte * arg,
  action_quote_e_type action_quote
)
{
  unsigned int i = 0;
  const byte * use_arg = arg; /* the argument used to search through arg list */
  byte stripped_arg[MAX_LINE_SIZE];
  dsat_num_item_type arg_num, list_ele_num;

  if(arg[0] == '\0')
  {
    val_arg_ptr = NULL;
    return PARAM_NULL;
  }
  else
  {
    /* In ignore_quote case, if the arg has double quote 
       surrounding it, strip the quotes out */
    switch (action_quote)
    {
      case ACTION_QUOTE_BOTH_OK:
        /* don't care about the quote */
        if(arg[0] == '"')
        {
          (void) dsatutil_strip_quotes_out(arg, stripped_arg, MAX_LINE_SIZE);
          use_arg = stripped_arg;
        }
        break;

      case ACTION_QUOTE_YES_QUOTE:
        /* strip out the quotes */
        if(!dsatutil_strip_quotes_out(arg, stripped_arg, MAX_LINE_SIZE))
        {
          return PARAM_MISS;
        }
        use_arg = stripped_arg;
        break;
        
      case ACTION_QUOTE_NO_QUOTE:
        /* don't strip quote */
        break;
       
      default:
        /*lint -save -e641 */
        ERR_FATAL("No such case: %d",action_quote,0,0);
        /*lint -restore */
        break;
    }
  }

  /* Abort if parameter null */  
  if (0 == *use_arg)
  {
    val_arg_ptr = NULL;
    return PARAM_NULL;
  }
  
  /* Search values list for match */  
  while (*l[i] != 0)
  {
    /* if both the argument and l[i] are valid numbers, do 
       numerical comparision */
    if(dsatutil_atoi(&arg_num, use_arg, 10)   == ATOI_OK &&
       dsatutil_atoi(&list_ele_num, l[i], 10) == ATOI_OK)
    {
      if(arg_num == list_ele_num)
      {
        *val_arg_ptr = i;
        return PARAM_HIT;
      }
    }
    /* do string comparision */
    else if (dsatutil_strcmp_ig_sp_case (l[i], use_arg) == 0)
    {
      *val_arg_ptr = i;
      return PARAM_HIT;
    }
    ++i;
  }

  /* if the execution come here, there is no match */
  return PARAM_MISS;
} /* dsatutil_srch_list ()*/


/*===========================================================================

FUNCTION ITOA_ZFILL

DESCRIPTION
  This function formats a word variable.  The return string is a null
  terminated 3 - digit decimal number with leading zeros.

DEPENDENCIES

RETURN VALUE
  Pointer to the NULL terminator.

SIDE EFFECTS

===========================================================================*/
LOCAL byte * itoa_zfill
(
  word v,
  byte *rb_ptr
)
{
  int n;
  byte c, *ptr;

  ptr = rb_ptr + 3;
  *ptr = '\0';

  for (n = 0; n < 3; ++n)
  {
    c = (byte) (v % 10);
    v /= 10;
    *--ptr = (c + '0');
  }
  
  return rb_ptr + 3;
} /*  itoa_zfill */

/*===========================================================================
FUNCTION DETERMINE_QUOTE_ACTION

DESCRIPTION
  This function determines the action_quote from a parameter's attrib.

DEPENDENCIES
  None

RETURN VALUE
  ACTION_QUOTE_YES_QUOTE if attrib has YES_QUOTE 
  ACTION_QUOTE_NO_QUOTE  if attrib has NO_QUOTE 
  ACTION_QUOTE_BOTH_OK if none of the above

SIDE EFFECTS
  None
===========================================================================*/
LOCAL action_quote_e_type determine_quote_action(uint32 attrib)
{
  /* if the command table does not say this command's 
     arguments do not contain quotes, we ignore the 
     surrounding quotes */
  action_quote_e_type action_quote = ACTION_QUOTE_BOTH_OK;

  if(attrib & NO_QUOTE)
  {
    action_quote = ACTION_QUOTE_NO_QUOTE;
  }
  else if(attrib & YES_QUOTE)
  {
    action_quote = ACTION_QUOTE_YES_QUOTE;
  }
  
  return action_quote;
} /* determine_quote_action */


/*===========================================================================

FUNCTION DSATUTIL_APPEND_DSM_ITEM

DESCRIPTION
  This function checks the buffer length against the passed threshold
  value.  If the threshold is exceeded, a new DSM item is requested and
  appended to the buffer chain.  An error is generated if no dsm_items
  are available. A pointer to the new DSM item is returned as a parameter;
  it equals the input DSM item if no action taken.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to current(appended) DSM item 

SIDE EFFECTS
  A new dsm_item may be allocated.
  
===========================================================================*/
dsm_item_type * dsatutil_append_dsm_item
(
  dsm_item_type * dsm_item_ptr,          /* Pointer to head DSM item */
  word threshold                         /* Threshold length of buffer */
)
{
  dsm_item_type *overflow_buff_ptr;
  dsm_item_type *cur_buff_ptr;
  byte x;
  byte y;
  word * used_ptr;
  byte * buff_ptr;

  cur_buff_ptr = dsm_item_ptr;
  buff_ptr = &dsm_item_ptr->data_ptr[dsm_item_ptr->used];
  used_ptr = &dsm_item_ptr->used;
  
  /*  If the data item is full, acquire a new data item. ERROR if none  */
  /*  available.                                                        */
  /*  Remove the last two characters from the old data item and put     */
  /*  them on the front of the new item, so they can be over written    */
  /*  if need be.                                                       */
  /*  Use dsm_append to append the two buffers                          */
  if ( *used_ptr > threshold )
  {
    y = *(buff_ptr-1);
    x = *(buff_ptr-2);

    overflow_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);

    (*used_ptr) -= 2;

    /* Now the buffer ptr should point to the new dsm item ptr */
    buff_ptr = overflow_buff_ptr->data_ptr;
    used_ptr = &overflow_buff_ptr->used;
    cur_buff_ptr = overflow_buff_ptr;

    dsm_append( &dsm_item_ptr, &overflow_buff_ptr );
      
    buff_ptr[(*used_ptr)++] = x;
    buff_ptr[(*used_ptr)++] = y;
  }
  return cur_buff_ptr;
} /* dsatutil_append_dsm_item */


/*===========================================================================

FUNCTION DSATUTIL_VALIDATE_DIAL_CHAR

DESCRIPTION
  This function examines the passed dial string charater and returns a
  validation result.  It is up to the caller to act upon the result as
  approppriate within its context.

  Refer to 3GPP TS 27.007 section 6.2 for details on the valid dialing 
  digits.  The supported set are summarized as follows:
    0 - 9 
    A - C
    + * ; #           
    D - E             (ETSI SMS PDU mode)
    W , $ T P ! @ D   (accepted but flagged for strip)
  
DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating validation result.

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatutil_validate_dial_char
(
  const byte * c_ptr                 /*  Input character   */
)
{
  dsat_dial_val_e_type result =  DSAT_DIAL_EMPTY;

  /* Characters that are ignored in a dial string */
  const byte d_strip        [] = "W,$TP!@D";

  /* Test for empty input */
  if (*c_ptr != '\0')
  {
    byte c = UPCASE (*c_ptr);

    /* Catagorize character */
    if (c >= '0' && c <= '9')
    {
      result = DSAT_DIAL_DIGIT;
    }
    else if (c >= 'A' && c <= 'C')
    {
      result = DSAT_DIAL_ASCII_ABC;
    }
    else if (c == 'D')
    {
      result = DSAT_DIAL_ASCII_D;
    }
    else if (c == 'E')
    {
      result = DSAT_DIAL_ASCII_E;
    }
    else if (c == '*')
    {
      result = DSAT_DIAL_STAR;
    }
    else if (c == '+')
    {
      result = DSAT_DIAL_PLUS;
    }
    else if (c == '#')
    {
      result = DSAT_DIAL_POUND;
    }
    else if (c == ';')
    {
      result = DSAT_DIAL_SEMICOLON;
    }
    else if (0 != strchr ((char*)d_strip, (char)c))
    {
      result = DSAT_DIAL_STRIP;
    }
    else
    {
      result = DSAT_DIAL_IGNORE;
    }
  }
  return result;
} /* dsatutil_validate_dial_char () */


/*===========================================================================

FUNCTION DSATUTIL_STRCMP_PKT_DIAL_STR

DESCRIPTION
  This function performs a lookup in the Packet call dial string registry
  to see if the input string prefix is found.  Further validation is
  done base on call type mode.   Input mode may be overridden for some
  call types.  Returns flag indicating valid packet call type detected.

DEPENDENCIES
  Lookup table must have been initialized from NV prior to use.

RETURN VALUE
  Returns TRUE if match in lookup table, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsatutil_strcmp_pkt_dial_str
(
  byte               *orig_dial_str,   /*  Pointer to the dial string   */
  dsati_mode_e_type  *mode_ptr         /*  Pointer to Mode of Operation */
)
{
  #define MAX_PKT_DIAL_STRING_LEN 32
  boolean result = FALSE;
  boolean hit = FALSE;
  uint8 prefix_len = 0;
  uint8 i = 0;
  byte *s_ptr, *p_ptr = NULL;
  byte pdsbuf[MAX_PKT_DIAL_STRING_LEN+1];
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  ASSERT(NULL != orig_dial_str);

  /* Perform aggressive strip on dial string to remove unwanted characters. */
  /* We do this assuming packet dial strings are limited to range [0-9#*].  */
  /* Avoids issue where characters allowed in the spec are before or        */
  /* after the token searched for against dial string table.                */
  p_ptr = orig_dial_str;
  s_ptr = pdsbuf;
  while (('\0' != *p_ptr) &&
         (MAX_PKT_DIAL_STRING_LEN > (s_ptr - pdsbuf)))
  {
    /* Test range */
    if ( (('0' <= *p_ptr) && ('9' >= *p_ptr)) ||
         ('#' == *p_ptr) ||
         ('*' == *p_ptr) )
    {
      /* Save wanted character */
      *s_ptr++ = *p_ptr;
    }
    p_ptr++;
  }
  *s_ptr = '\0';
  
  
  /* Perform lookup for packet call prefix */
  while (i < dsat_pkt_dial_string_table[subs_id].size)
  {
     byte * prefix = dsat_pkt_dial_string_table[subs_id].dial_string_info[i].dial_string;
     prefix_len = (uint8) strlen((char*) prefix);
     if( prefix_len > MAX_DIAL_STRING_SIZE )
     {
       DS_AT_MSG1_ERROR("Invalid prefix length : %d", prefix_len);
       return result;
     }

     /* Test packet prefix within dial string buffer */
     if ( 0 == memcmp(pdsbuf, prefix, prefix_len) ) 
     {
        /* Exit loop on hit */
        hit = TRUE;
        break;
     }
     i++;
  }

  /* Mode specific validation */
  if (hit)
  {
     dsati_pkt_dial_string_s_type * pds_ptr =
        &dsat_pkt_dial_string_table[subs_id].dial_string_info[i];
     pkt_mode_override = FALSE;
     /* CDMA mode validation */
     if ( 0 == ((NV_OPMODE_GPRS | NV_OPMODE_WCDMA) & pds_ptr->call_type) )
     {
        /* Ensure prefix matches input string exactly */
        if ('\0' == pdsbuf[prefix_len])
        {
           /* Verify CM preferred mode allowed for dial string */
           result = pkt_mode_override =
              check_operating_mode_override (mode_ptr,
                                             pds_ptr->modes_permitted);
        }
     }
     else
     {
        /* ETSI mode validation */
        /* Check to see if the dial string is terminated by a '#' */
        if( s_ptr > pdsbuf )
        {
           s_ptr = pdsbuf;
           while (*s_ptr != 0 )
           {
              s_ptr++;
           }
           if ( '#' == *(s_ptr-1) )
           {
             result = TRUE;
           }
        }
      }
    }
  
  DS_AT_MSG1_HIGH("strcmp_pkt_dial_str result = %d", result);
  return result;
} /*  dsatutil_strcmp_pkt_dial_str() */



/*===========================================================================

FUNCTION CHECK_OPERATING_MODE_OVERRIDE

DESCRIPTION
  This function checks the Call Manager preferred packet call mode against
  a list of permitted modes.  If CM mode is permitted, the ATCOP current
  operating mode is changed to match.  This affects later processing and
  call type invocation.  If CM mode not recognized, the current mode set
  by DSMGR is used instead.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if operating mode was changed, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean check_operating_mode_override
(
  dsati_mode_e_type            *mode_ptr,         /* Mode of Operation */
  nv_operating_mode_enum_type  modes_permitted    /* Allowed modes     */
)
{
   boolean result = FALSE;
   nv_operating_mode_enum_type preferred_mode = 0;
   sys_sys_mode_e_type cm_pref_mode;

   /* Query DS3G for preferred operating mode */
   /* Currently only single Data Subscription, so no need to verify with subs. */ 
   cm_pref_mode = ds3g_get_current_network_mode_ex(dsat_get_current_subs_id(FALSE));

   /* Map mode to NV enum */
   switch (cm_pref_mode)
   {
   case SYS_SYS_MODE_CDMA:
   case SYS_SYS_MODE_HDR:
      preferred_mode = NV_OPMODE_CDMA;
      break;
   case SYS_SYS_MODE_GSM:
      preferred_mode = NV_OPMODE_GPRS;
      break;
   case SYS_SYS_MODE_TDS:
   case SYS_SYS_MODE_WCDMA:
      preferred_mode = NV_OPMODE_WCDMA;
      break;
   default:
      /* If CM preferred mode not recognized, use current mode set by
         DSMGR instead */
      DS_AT_MSG1_HIGH("Unsupported CM pref_srv_type_for_packet_orig mode: %d",
               cm_pref_mode);
      
      switch (*mode_ptr)
      {
      case DSAT_MODE_GSM:
        preferred_mode = NV_OPMODE_GPRS;
        break;
      case DSAT_MODE_TDS:
      case DSAT_MODE_WCDMA:
        preferred_mode = NV_OPMODE_WCDMA;
        break;
      case DSAT_MODE_CDMA:
        preferred_mode = NV_OPMODE_CDMA;
        break;
      default:
        DS_AT_MSG1_HIGH("Unsupported DSMGR mode: %d",*mode_ptr);
      }
   }
           
   if (0 != preferred_mode)
   {
      /* Verify preferred mode is permitted */
      if ( 0 != (preferred_mode & modes_permitted) )
      {
         /* Change current mode to preferred mode */
        switch(preferred_mode)
        {
        case NV_OPMODE_GPRS:
          *mode_ptr = DSAT_MODE_GSM;
          break;
        case NV_OPMODE_WCDMA:
          *mode_ptr = DSAT_MODE_WCDMA;
          break;
        case NV_OPMODE_CDMA:
          *mode_ptr = DSAT_MODE_CDMA;
          break;
        default:
          DS_AT_MSG1_HIGH("Unsupported preferred mode: %d",preferred_mode);
        }
         result = TRUE;
      }
   }
   return result;
} /* check_operating_mode_override() */


#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_ETSI_DATA
/*===========================================================================

FUNCTION DSATUTIL_CHECK_OPT_DIAL_STR

DESCRIPTION
  Parses a Packet data dial string. Only options {*<cid>,***<cid>}
  are supported.  If cid is specified but not defined (via +CGDCONT),
  an error is returned.  The cid is passed back to caller.
  
  Validation is skipped if packet call mode override is set, indicating
  a non-ETSI mode dial string is being processed in ETSI mode.

DEPENDENCIES
  The calling function is responsible for cid parameter initialization.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatutil_check_opt_dial_str
(
  const byte * in_ptr,                /*  Input string, null terminated   */
  byte * cid_ptr                /*  Connection ID return value      */
)
{
#define MAX_OPTIONS 4
#define MAX_OPTION_SIZE 8
#define SC98_OPTION 1
#define SC99_OPTION 3
  byte options [MAX_OPTIONS][MAX_OPTION_SIZE];
  byte c;
  byte * o_ptr;
  byte curropt = 0;
  dsat_num_item_type gprs_sc, cid;
  dsat_result_enum_type result = DSAT_OK;
          
  /* Bypass validation if packet mode override occurred */
  if (pkt_mode_override)
  {
     return DSAT_OK;
  }

  /* Initialize options array */
  memset((void*)options, 0, sizeof(options));
  curropt = 0;

  /* Process first '*' */
  c = *in_ptr++;
  o_ptr = options[curropt];

  /* Scan input string, storing options as discovered */
  while ((c = *in_ptr++) != '\0')
  {
    /* Stop when hash seen */
    if (c == '#')
    {
      *o_ptr = '\0';
      break;
    }
    
    /* Option delimiter found; increment option field.
       Added ',' as option delimiter to allow processing of multiple
       <cid>s in dial string, which is not currently supported and should
       generate ERROR */
    if( c == '*' ||
        c == ',' )
    {
      if (curropt++ < (MAX_OPTIONS-1))
      {  
        *o_ptr = '\0';
        o_ptr = options[curropt];
      }
      else
      {
        DS_AT_MSG0_HIGH("Unsupported number of options");
        return DSAT_ERROR;
      }
    }
    /* Store option character */
    else
    {
      if ((o_ptr - options[curropt]) < (MAX_OPTION_SIZE-1))
      {
        *o_ptr++ = c;
      }
      else
      {
        *o_ptr = '\0';
        DS_AT_MSG0_MED("Option exceeds expected length");
      }
    }
  }
  *o_ptr = '\0';

  /* First option is actually GPRS Service Center, used to drive validation */
  if (ATOI_OK != dsatutil_atoi(&gprs_sc, options[0], 10))
  {
     DS_AT_MSG_SPRINTF_1_HIGH( 
      "Cannot convert to integer: %s", options[0]);
     return DSAT_ERROR;
  }

  /* Validate options based on SC */
  switch (gprs_sc)
  {
    case 98:
      /* Verify at most one option given (ATD*98*<cid>#) */
      if ((0 < curropt) && (SC98_OPTION != curropt))
      {
         DS_AT_MSG1_HIGH("Incorrect options for SC98: %d",curropt);
         result = DSAT_ERROR;
      }

      /* Check for valid conection id, if specified as first option */
      else if ('\0' != options[SC98_OPTION][0])
      {
        if (ATOI_OK == dsatutil_atoi(&cid, options[SC98_OPTION], 10))
        {
          if ( cid > MAX_BYTE_VALUE ||
               TRUE != 
                 dsatetsipkt_is_profile_context_valid( (uint16)cid,
                                                        DS_PROFILE_TECH_3GPP,
                                                        dsat_get_current_ds_profile_subs_id()))
          {
            DS_AT_MSG1_HIGH("Undefined PDP context cid: %d",cid);
            result = DSAT_ERROR;
          }
          else
          {
            /* Set CID return value */
            *cid_ptr = (byte)cid;
          }
        }
        else
        {
          DS_AT_MSG_SPRINTF_1_HIGH( "Cannot convert to \
            integer: %s", options[SC98_OPTION]);
          result = DSAT_ERROR;
        }
      }
      break;
      
    case 99:
      /* Verify at most 3 options given (ATD*99*<called_address>*<L2P>*<cid>#) */
      if ((0 < curropt) && SC99_OPTION < curropt)
      {
         DS_AT_MSG1_HIGH("Incorrect options for SC99: %d",curropt);
         result = DSAT_ERROR;
      }
      
      /* Check for valid conection id, if specified as third option */
      if ('\0' != options[SC99_OPTION][0])
      {
        if (ATOI_OK == dsatutil_atoi(&cid, options[SC99_OPTION], 10))
        {
          if (TRUE !=
              dsatetsipkt_is_profile_context_valid(cid,
                                                   DS_PROFILE_TECH_3GPP,
                                                   dsat_get_current_ds_profile_subs_id()))
          {

            DS_AT_MSG1_HIGH("Undefined PDP context cid: %d",cid);
            result = DSAT_ERROR;
          }
          else
          {
            /* Set CID return value */
            *cid_ptr = (byte)cid;
          }
        }
        else
        {
           DS_AT_MSG_SPRINTF_1_HIGH("Cannot convert to \
             integer: %s", options[SC99_OPTION]);
           result = DSAT_ERROR;
        }
      }
      break;
      
    default:
      DS_AT_MSG1_HIGH("Unsupport GPRS Service Center: %d", gprs_sc);
      return DSAT_ERROR;
  }
  
  return result;
} /* dsatutil_check_opt_dial_str () */
#endif /* FEATURE_DSAT_ETSI_DATA */


/*===========================================================================

FUNCTION DSATUTIL_FILL_MODIFIER_INFO

DESCRIPTION
  This function fill out the struct pointed by mod_info_ptr according
  to attribute of dial_ptr and <index> and <info> of +CCUG command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None.

===========================================================================*/
dsat_result_enum_type dsatutil_fill_modifier_info
(
  /* input, contains properties of dial string */
  const dsati_dial_str_type     * dial_ptr,

  /* output, contains modifier info of the dial string */
  dsat_dial_modifier_info_type  * mod_info_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  mod_info_ptr->modifier_present = FALSE;

  if(dial_ptr->attrib & DIAL_ATTRIB_PKT)
  {
#ifdef FEATURE_DSAT_ETSI_DATA
    byte cid = 0;
    
    /* Check to see if the dial string contains optional characters */
    /* Refer to 3GPP TS 27.007 sections 10.2.1.1 & 10.2.1.2         */
    if (DSAT_OK == dsatutil_check_opt_dial_str ( dial_ptr->num, &cid ))
    {
      if (0 != cid)
      {
        /* this is a packet call */
        mod_info_ptr->modifier_present = TRUE;
        mod_info_ptr->modifier = DSAT_CID_DIAL_MODIFIER;

        mod_info_ptr->modifier_info.cid.cid_val = cid;
      }
    }
    else
    {
      /* Problem decoding connection ID */
      result = DSAT_ERROR;
    }
#endif /* FEATURE_DSAT_ETSI_DATA */
  }
  else
  {
#ifdef FEATURE_DSAT_ETSI_MODE
    /* fill out CLIR related fields */
    if(dial_ptr->attrib & DIAL_ATTRIB_UP_I)
    {
      mod_info_ptr->modifier_present = TRUE;
      mod_info_ptr->modifier = DSAT_CLI_DIAL_MODIFIER;
      mod_info_ptr->modifier_info.cli.cli_enabled = TRUE;
    }
    else if(dial_ptr->attrib & DIAL_ATTRIB_DN_I)
    {
      mod_info_ptr->modifier_present = TRUE;
      mod_info_ptr->modifier = DSAT_CLI_DIAL_MODIFIER;
      mod_info_ptr->modifier_info.cli.cli_enabled = FALSE;
    }

    /* fill out +CCUG related fields */
    if(
       /* dial modifier enables CUG temporary mode */
       dial_ptr->attrib & DIAL_ATTRIB_UP_G ||

       /* NOT disable CUG, and +CCUG <n> enables CUG temporary mode */
       ( (!(dial_ptr->attrib & DIAL_ATTRIB_DN_G)) && 
         ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,0,NUM_TYPE) == 1))
      )
    {
      mod_info_ptr->modifier_present = TRUE;
      if(dial_ptr->attrib & (DIAL_ATTRIB_UP_I | DIAL_ATTRIB_DN_I) )
      {
        mod_info_ptr->modifier = DSAT_CLI_AND_CUG_MODIFIER;
      }
      else
      {
        mod_info_ptr->modifier = DSAT_CUG_DIAL_MODIFIER;
      }

      mod_info_ptr->modifier_info.cug.cug_enabled = TRUE;

      /* see if CUG index is present */
      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,1,NUM_TYPE) != 10)
      {
	if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,1,NUM_TYPE) > 9)
        {
          ASSERT(0);
          return DSAT_ERROR;
        }
        mod_info_ptr->modifier_info.cug.cug_index_enabled = TRUE;
        mod_info_ptr->modifier_info.cug.cug_index_val = (byte)
          (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,1,NUM_TYPE);
      }
      else
      {
        mod_info_ptr->modifier_info.cug.cug_index_enabled = FALSE;
      }

      /* fill out fields related to <info> of +CCUG */
      switch((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,2,NUM_TYPE))
      {
        case 0:
          mod_info_ptr->modifier_info.cug.suppress_pref_cug = FALSE;
          mod_info_ptr->modifier_info.cug.suppress_outgoing_access = FALSE;
          break;

        case 1:
          /* +CCUG cmd <info>: suppress OA */
          mod_info_ptr->modifier_info.cug.suppress_pref_cug = FALSE;
          mod_info_ptr->modifier_info.cug.suppress_outgoing_access = TRUE;
          break;

        case 2:
          /* +CCUG cmd <info>: suppress preferential CUG */
          mod_info_ptr->modifier_info.cug.suppress_pref_cug = TRUE;
          mod_info_ptr->modifier_info.cug.suppress_outgoing_access = FALSE;
          break;
      
        case 3:
          /* +CCUG cmd <info>: suppress OA and preferential CUG */
          mod_info_ptr->modifier_info.cug.suppress_outgoing_access = TRUE;
          mod_info_ptr->modifier_info.cug.suppress_pref_cug = TRUE;
          break;

        default:
          DS_AT_MSG1_HIGH("incorrect dsat_ccug_val[2]: %d", (dsat_num_item_type)
            dsatutil_get_val(DSATETSI_EXT_ACT_CCUG_ETSI_IDX,0,2,NUM_TYPE));
          mod_info_ptr->modifier_info.cug.suppress_pref_cug = FALSE;
          mod_info_ptr->modifier_info.cug.suppress_outgoing_access = FALSE;
          result = DSAT_ERROR;
          break;
      }
    }
#endif /* FEATURE_DSAT_ETSI_MODE */
  }

  return result;
} /* dsatutil_fill_modifier_info */

#endif /* FEATURE_DSAT_ETSI_MODE */



/*===========================================================================

FUNCTION DSATUTIL_INIT_PKT_DIAL_STRING_FROM_NV

DESCRIPTION
  This function reads Packet Dial Sting lookup table from the NV during
  initialization. If these items were never written in NV, default
  values are loaded and stored to NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_pkt_dial_string_from_nv 
( 
  sys_modem_as_id_e_type  subs_id
)
{
  nv_stat_enum_type    nv_status;          
  nv_item_type         ds_nv_item;                /* actual data item     */

  memset ((void*)&ds_nv_item, 0, sizeof(ds_nv_item));
  /* Read the packet dial sting list from the NV */
  nv_status = dsatutil_get_nv_item_per_subs( NV_PKT_DIAL_STRINGS_I,
                                             &ds_nv_item,
                                             subs_id);
  
  /* Check if the NV has valid data */
  if( nv_status == NV_DONE_S)
  {
    /* i is index into NV items read, j is index into dial string table
       being populated. Allows for NV to have null entries at beginning,
       end or middle */
    uint8 i,j;
     
    /* Create local table from NV results */
    memset ((void*)&dsat_pkt_dial_string_table[subs_id], 0,sizeof(dsati_pkt_dial_string_table_s_type));
     
    for ( i=0, j = 0; i < NV_PKT_DIAL_STRING_LIST_SIZE; i++ )
    {
      if ( '\0' != ds_nv_item.pkt_dial_strings[i].dial_string[0] )
      {
        dsati_pkt_dial_string_s_type * pds_ptr =
          &dsat_pkt_dial_string_table[subs_id].dial_string_info[j];
        /* Transfer data items */
        pds_ptr->call_type = ds_nv_item.pkt_dial_strings[i].call_type;
        pds_ptr->modes_permitted = ds_nv_item.pkt_dial_strings[i].modes_permitted;
        (void) dsatutil_memscpy((void*)pds_ptr->dial_string,
                MAX_DIAL_STRING_SIZE,
                (void*)ds_nv_item.pkt_dial_strings[i].dial_string,
                NV_PKT_DIAL_STRING_SIZE);
        j++;
      }
    }
       dsat_pkt_dial_string_table[subs_id].size = j;
  }
  else
  {
    /* NV not available so use default table */
    dsati_pkt_dial_string_table_s_type default_pkt_dial_string_table =
    {
      3,
      {
        /* String,  Call Type,                         Permitted Modes                 */
        { "*98",    (NV_OPMODE_GPRS|NV_OPMODE_WCDMA),  (NV_OPMODE_GPRS|NV_OPMODE_WCDMA) },
        { "*99",    (NV_OPMODE_GPRS|NV_OPMODE_WCDMA),  (NV_OPMODE_GPRS|NV_OPMODE_WCDMA) },
        { "#777",   NV_OPMODE_CDMA,                    NV_OPMODE_CDMA                   },
        { "",       0,                                 0                                }
      }
    };
     
    (void) dsatutil_memscpy((void*)&dsat_pkt_dial_string_table[subs_id],
            sizeof(dsati_pkt_dial_string_table_s_type),
            (void*)&default_pkt_dial_string_table,
            sizeof(default_pkt_dial_string_table));

    /* Update NV if never written */
    if( nv_status == NV_NOTACTIVE_S )
    {
      uint8 i = 0;
      memset ((void*)&ds_nv_item, 0, sizeof(ds_nv_item));
     
      while (i < dsat_pkt_dial_string_table[subs_id].size)
      {
        dsati_pkt_dial_string_s_type * pds_ptr =
          &dsat_pkt_dial_string_table[subs_id].dial_string_info[i];
        /* Transfer data items */
        ds_nv_item.pkt_dial_strings[i].call_type = pds_ptr->call_type;
        ds_nv_item.pkt_dial_strings[i].modes_permitted =
          pds_ptr->modes_permitted;
        (void) dsatutil_memscpy((void*)ds_nv_item.pkt_dial_strings[i].dial_string,
                NV_PKT_DIAL_STRING_SIZE,
                (void*)pds_ptr->dial_string,
               sizeof(ds_nv_item.pkt_dial_strings[i].dial_string));
        i++;
      }
       
      /* Post to NV */
      (void)dsatutil_put_nv_item_per_subs( NV_PKT_DIAL_STRINGS_I, 
                                           &ds_nv_item,
                                           subs_id);
    }
  }
  return;
}

/*===========================================================================

FUNCTION DSATUTIL_INIT_TIMERS

DESCRIPTION
  This function initializes timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_init_timers ( void )
{
  uint8 idx1, idx2;

  for (idx1 = 0; idx1 < dsatutil_timer_table_size_ex; idx1++)
  {
    for (idx2 = 0; idx2 < (uint8) DSAT_TIMER_MAX; idx2++)
    {
      if (dsatutil_timer_table_ex[idx1].timer_id ==
          dsatutil_timer_table[idx2].timer_id)
      {
        dsatutil_timer_table[idx2].hdlr_func =
          dsatutil_timer_table_ex[idx1].hdlr_func;
      }
    }
  }

  return;
} /* dsatutil_init_timers */

/*===========================================================================

FUNCTION DSATUTIL_DEINIT_TIMERS

DESCRIPTION
  This function deinitializes timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void dsatutil_deinit_timers ( void )
{
  uint8 idx1, idx2;

  for (idx1 = 0; idx1 < dsatutil_timer_table_size_ex; idx1++)
  {
    for (idx2 = 0; idx2 < (uint8) DSAT_TIMER_MAX; idx2++)
    {
      if (dsatutil_timer_table_ex[idx1].timer_id ==
          dsatutil_timer_table[idx2].timer_id)
      {
        dsatutil_timer_table[idx2].hdlr_func = NULL;
      }
    }
  }

  return;
} /* dsatutil_deinit_timers */

/*===========================================================================

FUNCTION  DSATUTIL_ALLOC_CMD_MEM_PTR

DESCRIPTION
  This function can be used to allocate command memory pointer to maintain the
  info required during commands execution.

DEPENDENCIES
  None

RETURNS
  Command memory pointer.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_alloc_cmd_mem_ptr
(
  void *data_ptr
)
{
  dsat_global_state *state_ptr = &dsat_pending_state[PORT_ZERO];

  ASSERT(NULL == state_ptr->cmd_mem_ptr);

  state_ptr->cmd_mem_ptr = data_ptr;

  return state_ptr->cmd_mem_ptr;
} /* dsatutil_alloc_cmd_mem_ptr */

/*===========================================================================

FUNCTION  DSATUTIL_GET_CMD_MEM_PTR

DESCRIPTION
  This function can be used to get the command memory pointer which maintains the
  info required during commands execution.

DEPENDENCIES
  None

RETURNS
  Command memory pointer.

SIDE EFFECTS
  None

===========================================================================*/
void *dsatutil_get_cmd_mem_ptr(void)
{
  dsat_global_state *state_ptr = &dsat_pending_state[PORT_ZERO];

  ASSERT(NULL != state_ptr->cmd_mem_ptr);

  return state_ptr->cmd_mem_ptr;
} /* dsatutil_get_cmd_mem_ptr */

/*===========================================================================

FUNCTION  DSATUTIL_FREE_CMD_MEM_PTR

DESCRIPTION
  This function can be used to free the command memory pointer which maintains the
  info required during commands execution.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatutil_free_cmd_mem_ptr(void)
{
  dsat_global_state *state_ptr = &dsat_pending_state[PORT_ZERO];

  if (NULL != state_ptr->cmd_mem_ptr)
  {
    dsatutil_free_memory(state_ptr->cmd_mem_ptr);
    state_ptr->cmd_mem_ptr = NULL;
  }

  return;
} /* dsatutil_free_cmd_mem_ptr */

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_ALPHA_GSM_WITH_AT_TO_ALPHA_GSM

DESCRIPTION
This function converts a GSM string which could contain a GSM '@' character (0x00)
to a GSM string with replacing '@' (0x00) to a special character (0xe6) which is known to 
ATCOP module alone. 

DEPENDENCIES
  This function should be invoked when the string (input to ATCOP) is comming from other
  than TE (eg: network or some gsdi modules etc)

  
RETURN VALUE
  possible values:
    TRUE: Succesfull conversion. The out_str is a NULL terminated string.
    FALSE: could not convert the string. 
SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatutil_convert_alpha_gsm_with_at_to_alpha_gsm
(
  const char *in_str, 
  char *out_str,
  uint16 in_len
)
{
  uint16 index = 0;
  
  if( (in_str == NULL)|| (out_str == NULL)|| in_len==0 )
  {
    DS_AT_MSG0_MED("Invalid len/string passed ");
    return FALSE;
  }
  while( index < in_len )
  {
    if ( in_str[index] == GSM_AT_CHAR )
    {
      out_str[index] = GSM_AT_CHAR_REPLACEMENT;
    }
    else
    {
      out_str[index]= in_str[index];
    }
    ++index;
  }
  out_str[index] = '\0';

  DS_AT_MSG2_MED("in_len = %d, index = %d", in_len, index);

  return TRUE;
}/* dsatutil_convert_alpha_gsm_with_at_to_alpha_gsm */


/*===========================================================================

FUNCTION DSATUTIL_CONVERT_CHSET

DESCRIPTION
  This function performs conversion of a given null terminated string in one 
  supported character set to another supported character set, writing it into
  an output buffer provided and null terminating it. It will handle 
  inconvertible characters as indicated by drop_inconvertible, simply not 
  copying them if TRUE or stopping conversion and returning NULL if FALSE. 
  The character with value 0x00 is not supported in the GSM 7 bit or IRA 
  alphabet as NULL is used for terminating strings, neither will the function 
  output this character value except as a termination. When converting from 
  HEX/UCS2 for instance this counts as an inconvertible character.
  It will stop the conversion if indicated maximum output buffer length 
  in bytes is reached.

DEPENDENCIES
  None.

RETURN VALUE
  possible values:
    NULL : if drop_inconvertible FALSE and conversion fails
    pointer to terminating NULL in output buffer : if conversion successful

SIDE EFFECTS
  None
  
===========================================================================*/
char *dsatutil_convert_chset
(
  const char *in_str, 
  dsati_chset_type in_chset,
  const char *out_str,
  dsati_chset_type out_chset,
  uint16 out_max,
  boolean drop_inconvertible
)
{
  #define LWR_BYTE 0x00FF
  /* Macro to convert a character to a NULL-terminated pair of hex characters
     at the location pointed to by out_str_dup, which is left pointing to the 
     terminating NULL */
  #define CVT_TO_HEX_PAIR(ch)\
    if (ch < 16) *out_str_dup++ = '0';\
    out_str_dup = (char *)dsatutil_itoa((uint32)ch, (byte *)out_str_dup, 16);

  char *in_str_dup = (char *)in_str;
  char *out_str_dup = (char *)out_str;
  uint8 in_char_len = 0, out_char_len = 0;
  char   conv_char;
  dsat_num_item_type conv_word;
  char   conv_buf[5] = {0};
  dsat_num_item_type in_char_val;
  char *tmp_chr_ptr = NULL;
  /* Mark the end of output buffer */
  char *last_char_lim;
  uint16 in_str_max_len; /* max length of the data area that can be accessed from in_str */

  ASSERT(( in_str != NULL ) && ( out_str != NULL ));

  DS_AT_MSG3_HIGH("in_char = %d, out_char = %d, out_max = %d",
                  in_chset, out_chset, out_max);

  if ( in_chset >= ALPHA_MAX ||
       out_chset >= ALPHA_MAX )
  {
    return NULL;
  }

  /* Save character lengths */
  switch ( in_chset )
  {
    case ALPHA_UCS2:
      in_char_len = UCS2_CHAR_LEN;
      break;
    case ALPHA_HEX:
      in_char_len = HEX_CHAR_LEN;
      break;
    case ALPHA_GSM:
    case ALPHA_GSM_WITH_AT:
      in_char_len = GSM_CHAR_LEN;
      break;
    default:
    case ALPHA_IRA:
    case ALPHA_PBM_8BIT:
      in_char_len = IRA_CHAR_LEN;
      break;
    case ALPHA_PBM_UCS2:
      in_char_len = PBM_UCS2_CHAR_LEN;
      break;
  }
  switch ( out_chset )
  {
    case ALPHA_UCS2:
      out_char_len = UCS2_CHAR_LEN;
      break;
    case ALPHA_HEX:
      out_char_len = HEX_CHAR_LEN;
      break;
    case ALPHA_GSM:
    case ALPHA_GSM_WITH_AT:
      out_char_len = GSM_CHAR_LEN;
      break;
    default:
    case ALPHA_IRA:
    case ALPHA_PBM_8BIT:
      out_char_len = IRA_CHAR_LEN;
      break;
    case ALPHA_PBM_UCS2:
      out_char_len = PBM_UCS2_CHAR_LEN;
      break;
  }

  /* Adjust last possible char position to save room for 
     the character size and a terminating NULL */
  if ( out_chset == ALPHA_PBM_UCS2 )
  {
   last_char_lim = (out_str_dup + out_max) - (2 * out_char_len );
  }
  else
  {
    last_char_lim = (out_str_dup + out_max) - (out_char_len+1);
  }
  /* compute max size of in_str */
  if (in_chset == ALPHA_8BIT)
  { 
    /* in the case of ALPHA_8BIT let the output buffer determine how much to read from in_str.  This 
    is because NULL characters can be expected in the input stream for conversion and hence can't rely
    on strlen() */
    in_str_max_len = (uint16)(((out_max-1)/out_char_len) * in_char_len); 
  }
  else if (in_chset == ALPHA_PBM_UCS2 )
  {
    in_str_max_len = (uint16)(dsatutil_ucs2_strlen(in_str) * 2);
  }
  else
  {
    /* For all other character sets, we're guaranteed to get NULL-terminated strings */
    in_str_max_len = (uint16)strlen(in_str_dup);
  }

  if ((in_str_max_len % in_char_len) != 0 )
  {
    /* Return with error condition if not a 
       'whole' number of input chars in input string */
    DS_AT_MSG2_ERROR("incorrect input string len %d for char set size %d",in_str_max_len-1,in_char_len);
    return NULL;
  }

  /* Go until there are characters still to be read AND 
  there is room left in out_str for converted output */
  while ( ((in_str_dup - in_str) < in_str_max_len) &&
          (out_str_dup <= last_char_lim) )
  {
    /* sanity check for internal error */
   if ((in_str_dup - in_str) % in_char_len) /* we didn't consume the right number of chars */
   {
     ERR_FATAL("Internal Error: in_char_len %d processed %d",in_char_len, in_str_dup-in_str,0);
   }

   switch ( in_chset )
    {
      /* Input character set IRA */
      case ALPHA_IRA:
        switch ( out_chset )
        {
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT:
            if ( *in_str_dup == 0x40 ) /* ASCII @ */
            {
              if ( out_chset == ALPHA_GSM )
              {
                *out_str_dup++= GSM_AT_CHAR_REPLACEMENT; 
              }
              else
              {
                *out_str_dup++= GSM_AT_CHAR;
              }
            }
            else
            {
              conv_char = ira_to_gsm7[(uint8)(*in_str_dup)];
              if ( conv_char != MAX_8BIT_CHAR && conv_char >= MIN_IRA_GSM_CHAR )
              {
                *out_str_dup++ = conv_char;
              }
              else if ( !drop_inconvertible )
              {
                return NULL;
              }
              else
              {
                /* Replace inconvertible char with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
            }
            break;
          case ALPHA_UCS2:
            memset( out_str_dup, '0', 2 );
            out_str_dup += 2;
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          case ALPHA_HEX:
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          case ALPHA_IRA:
          case ALPHA_8BIT:
          case ALPHA_PBM_8BIT:
            *out_str_dup++ = *in_str_dup;
            break;
          case ALPHA_PBM_UCS2:
            *out_str_dup++ = *in_str_dup;
            *out_str_dup++ ='\0';
            break;
          /* If unknown output character set specified */
          default:
            return NULL;
        }
        /* Advance by one input octet */
        in_str_dup += IRA_CHAR_LEN;
        break;

      /* Input character set GSM */
      case ALPHA_GSM:
        if ( (*in_str_dup > MAX_GSM_CHAR )&& (*in_str_dup != GSM_AT_CHAR_REPLACEMENT ))
        {
          return NULL;
        }
        switch ( out_chset )
        {
          case ALPHA_IRA:
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT)
            {
              *out_str_dup++ = 0x40; /* 0x40 is IRA '@' character */
            }
            else if ( (conv_char = gsm7_to_ira[(uint8)(*in_str_dup)]) != MAX_8BIT_CHAR ) 
            {
              *out_str_dup++ = conv_char;
            }
            else if ( !drop_inconvertible )
            {
              return NULL;
            }
            else
            {
              /* Replace inconvertible char with a space */
              *out_str_dup++ = (char)REPLACEMENT_CHAR;
            }
            break;
          case ALPHA_UCS2:
            /* First convert to PBM 8bit alphabet, which is nearly identical to
               UCS2: if escape character is found in_str_dup will be advanced 
               by one and left pointing at character following it */
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              memset( out_str_dup, '0', 2 );
              out_str_dup += 2;
              /* 0x40 is UCS2 equivalent for @ */
              out_str_dup = 
                 (char *)dsatutil_itoa( 0x40, (byte *)out_str_dup, 16);
            }
            else
            {
              gsm_char_to_pbm( &in_str_dup, &conv_char );
              /* If the PBM 8bit character is one not common to UCS2... */
              if ( NULL != (tmp_chr_ptr = 
                          (char *)strchr( pbm8_to_ucs2_not_common, conv_char )) )
              {
                /* Replace it with the proper UCS2 value as hex 4-tuple */
                (void)strlcpy( out_str_dup, 
                                  (char *)ucs2_to_pbm8_not_common[tmp_chr_ptr - 
                                                        pbm8_to_ucs2_not_common],
                                   UCS2_CHAR_LEN + 1 );
                out_str_dup += UCS2_CHAR_LEN;
              }
              else
              {
                *out_str_dup++ = '0';
                out_str_dup = (char *)dsatutil_itoa( 0, (byte *)out_str_dup, 16);
                CVT_TO_HEX_PAIR( conv_char );
              }
            }
            break;
          case ALPHA_HEX:
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              /* 0x40 is UCS2 equivalent for @ */
              out_str_dup = 
                 (char *)dsatutil_itoa( 0x40, (byte *)out_str_dup, 16);
            }
            else
            {
              CVT_TO_HEX_PAIR( *in_str_dup );
            }
            break;
          case ALPHA_8BIT:
          case ALPHA_GSM:
            *out_str_dup++ = *in_str_dup;
            break;
          case ALPHA_GSM_WITH_AT:
           if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
           {
             *out_str_dup++ = GSM_AT_CHAR;
           }
           else
           {
             *out_str_dup++ = *in_str_dup;
           }
           break; 
          case ALPHA_PBM_8BIT:
            /* if escape character is found in_str_dup will be advanced by one
               and left pointing at character following it */
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              *out_str_dup++=0x40; /* 0x40 is PBM8bit '@' character */
            }
            else
            {
              gsm_char_to_pbm( &in_str_dup, out_str_dup++ );
            }
            break;
          case ALPHA_PBM_UCS2:
            /* if escape character is found in_str_dup will be advanced by one
               and left pointing at character following it */
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              *out_str_dup++=0x40; /* 0x40 is PBM8bit '@' character */
            }
            else
            {
              gsm_char_to_pbm( &in_str_dup, out_str_dup++ );
            }
            *out_str_dup++ ='\0';
            break;
          /* If unknown output character set specified */
          default:
            return NULL;
        }
        /* Advance by one input octet */
        in_str_dup += GSM_CHAR_LEN;
        break;

      /* Input character set UCS2 */
      case ALPHA_UCS2:
        /* Input UCS2 character must be composed completely of HEX chars */
        if ( strchr(hex_chars, *in_str_dup) == NULL ||
             strchr(hex_chars, *(in_str_dup + 1)) == NULL ||
             strchr(hex_chars, *(in_str_dup + 2)) == NULL ||
             strchr(hex_chars, *(in_str_dup + 3)) == NULL ) 
        {
          return NULL;
        }

        switch ( out_chset )
        {
          case ALPHA_IRA:
          case ALPHA_8BIT:
          case ALPHA_HEX:
            /* If first 2 chars are not '0's */
            if ( *in_str_dup != '0' ||
                 *(in_str_dup + 1) != '0' )
            {
              if ( drop_inconvertible )
              {
                /* Replace with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
              else
              {
                return NULL;
              }
            }
            else
            {
              if ( ALPHA_HEX == out_chset )
              {
                (void) dsatutil_memscpy((void*)out_str_dup,
                        HEX_CHAR_LEN,(void*)(in_str_dup+2),HEX_CHAR_LEN);             
                out_str_dup += HEX_CHAR_LEN;
              }
              else /* ALPHA_IRA or ALPHA_8BIT */
              {
                /* First convert to a char... */
                (void)strlcpy( conv_buf, in_str_dup+2, HEX_CHAR_LEN + 1 );
                /* Already verified input is all HEX chars 
                   so safe to ignore return value */
                (void)dsatutil_atoi( &in_char_val, 
                                     (const byte *)&conv_buf[0], 16 );

                /* Extended IRA is 8 bit alphabet and character 
                   with value 0x00 is not allowed*/
                if ( out_chset == ALPHA_IRA &&
                     ( in_char_val < MIN_IRA_GSM_CHAR ||
                       in_char_val > MAX_GSM_CHAR ) )
                {
                  if ( !drop_inconvertible )
                  {
                    return NULL;
                  }
                  else
                  {
                    /* Replace inconvertible char with a space */
                    *out_str_dup++ = (char)REPLACEMENT_CHAR;
                  }
                }
                else
                {
                  *out_str_dup++ = (uint8)in_char_val;
                }
              }
            }
            break;
          case ALPHA_GSM:
          case ALPHA_PBM_8BIT:
          case ALPHA_GSM_WITH_AT:
            /* Copy UCS2 character (as hex 4-tuple) into temporary buffer */
            (void)strlcpy( conv_buf, in_str_dup, UCS2_CHAR_LEN + 1 );
            /* If found in array of characters not common to both 
               PBM8 and UCS2... */
            if ( PARAM_HIT == dsatutil_srch_list( 
                                     &conv_word,  
                                     ucs2_to_pbm8_not_common,     
                                     (const byte *)conv_buf,
                                     ACTION_QUOTE_NO_QUOTE ) )
            {
              /* Get word from same offset of complementary array */
              in_char_val = pbm8_to_ucs2_not_common[conv_word];
            }
            else if ( PARAM_HIT == dsatutil_srch_list( 
                                          &conv_word,
                                          ucs2_to_pbm8_not_supported,     
                                          (const byte *)conv_buf,
                                          ACTION_QUOTE_NO_QUOTE ) )
            {
              in_char_val = (char)REPLACEMENT_CHAR;
            }
            else
            {
              /* Convert to a word:
                 Already verified input is all HEX chars 
                 so safe to ignore return value */
              (void)dsatutil_atoi( &in_char_val, 
                                   (const byte *)conv_buf, 16 );
            }

            /* If upper byte is not zero */
            if ( in_char_val > MAX_8BIT_CHAR )
            {
              if ( drop_inconvertible )
              {
                /* Replace with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;         
              }
              else
              {
                return NULL;
              }
            }
            else
            {
              if ( ALPHA_GSM == out_chset || ALPHA_GSM_WITH_AT == out_chset)
              {
                if ( (in_char_val & LWR_BYTE) == 0x40 ) /* UCS2 @ char */
                {
                  if ( ALPHA_GSM == out_chset )
                  {
                    *out_str_dup = GSM_AT_CHAR_REPLACEMENT; 
                  }
                  else
                  {
                    *out_str_dup = GSM_AT_CHAR;
                  }
                }
                else
                {
                  pbm_char_to_gsm( (char)(in_char_val & LWR_BYTE), &out_str_dup );
                }
              }
              else
              {
                *out_str_dup = (char)(in_char_val & LWR_BYTE);
              }
              out_str_dup += GSM_CHAR_LEN;
            }
            break;
          case ALPHA_UCS2:
            (void) dsatutil_memscpy((void*)out_str_dup,
                    UCS2_CHAR_LEN,(void*)in_str_dup,UCS2_CHAR_LEN);
            out_str_dup += UCS2_CHAR_LEN;
            break;
          case ALPHA_PBM_UCS2:
            /* Convert the lower 16 bits of the input and add to output then 
             * the upper 16 bits */
            (void)strlcpy( conv_buf, in_str_dup + 2, PBM_UCS2_CHAR_LEN + 1 );
            (void)dsatutil_atoi( &in_char_val, 
                                 (const byte *)conv_buf, 16 );
            *out_str_dup++ = (char)in_char_val;
            (void)strlcpy( conv_buf, in_str_dup, PBM_UCS2_CHAR_LEN + 1 );
            (void)dsatutil_atoi( &in_char_val, 
                                 (const byte *)conv_buf, 16 );
            *out_str_dup++ = (char)in_char_val;
            break;
          /* If unknown output character set specified */
          default:
            return NULL;
        }
        in_str_dup += UCS2_CHAR_LEN;
        break;

      /* Input character set HEX */
      case ALPHA_HEX:
        /* Input HEX character pair must be composed completely of HEX chars */
        if ( strchr(hex_chars, *in_str_dup) == NULL ||
             strchr(hex_chars, *(in_str_dup + 1)) == NULL )
        {
          return NULL;
        }

        switch ( out_chset )
        {
          case ALPHA_IRA:
          case ALPHA_GSM:
          case ALPHA_8BIT:
          case ALPHA_GSM_WITH_AT:
            (void)strlcpy( conv_buf, in_str_dup, (HEX_CHAR_LEN + 1) );

            /* Already verified input is all HEX chars 
               so safe to ignore return value */
            (void)dsatutil_atoi( &in_char_val,
                           (const byte *)&conv_buf[0], 16 );


            /* GSM and IRA are 7 bit & 8 bits alphabets respectively 
               and character with value 0x00 is not allowed */
            if ( out_chset != ALPHA_8BIT &&
                 ( in_char_val < MIN_IRA_GSM_CHAR ||
                   in_char_val > MAX_GSM_CHAR ) )
            {
              if ( !drop_inconvertible )
              {
                return NULL;
              }
              else
              {
                /* Replace inconvertible char with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
            }
            else
            { /* Check for HEX @ character */
              if ( ( in_char_val == 0x40 ) && ( out_chset == ALPHA_GSM ))
              {
                *out_str_dup++ = GSM_AT_CHAR_REPLACEMENT;
              }
              else if ( ( in_char_val == 0x40 ) && ( out_chset == ALPHA_GSM_WITH_AT ))
              {
                *out_str_dup++ = GSM_AT_CHAR;
              }
              else
              {
                *out_str_dup++ = (uint8)in_char_val;
              }
            }
            break;
          case ALPHA_UCS2:
            memset( out_str_dup, '0', HEX_CHAR_LEN );
            out_str_dup += HEX_CHAR_LEN;
            break;
          case ALPHA_HEX:
            (void) dsatutil_memscpy((void*)out_str_dup,
                                HEX_CHAR_LEN,(void*)in_str_dup,HEX_CHAR_LEN);
            out_str_dup += HEX_CHAR_LEN;
            break;
          /* If unsupported output character set specified */
          default:
            return NULL;
        }
        in_str_dup += HEX_CHAR_LEN;
        break;

      /* Input character set 8BIT */
      case ALPHA_8BIT:
        switch ( out_chset )
        {
          case ALPHA_IRA:
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT:
            if ( *in_str_dup < MIN_IRA_GSM_CHAR ||
                 *in_str_dup > MAX_GSM_CHAR )
            {
              if ( !drop_inconvertible )
              {
               return NULL;
              }
              else
              {
                /* Replace inconvertible char with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
            }
            else
            {  /* check for ALPHA 8 BIT @ character */
              if ( ( *in_str_dup == 0x40 ) && ( out_chset == ALPHA_GSM ))
              {
                *out_str_dup++ = GSM_AT_CHAR_REPLACEMENT;
              }
              else if ( ( *in_str_dup == 0x40 ) && ( out_chset == ALPHA_GSM_WITH_AT ) )
              {
                *out_str_dup++ = GSM_AT_CHAR;
              }
              else
              {
                *out_str_dup++ = (uint8)*in_str_dup;
              }
            }
            break;
          case ALPHA_UCS2:
            memset( out_str_dup, '0', 2 );
            out_str_dup += 2;
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          case ALPHA_HEX:
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          /* If unsupported output character set specified */
          default:
            return NULL;
        }
        in_str_dup += EIGHT_BIT_CHAR_LEN;
        break;

      /* Input character set PBM 8BIT */
      case ALPHA_PBM_8BIT:
        switch ( out_chset )
        {
          case ALPHA_IRA:
            /* PBM 8bit and IRA T.50 are the same over the range 0x00-0x7F */
            *out_str_dup++ = ( *in_str_dup <= MAX_GSM_CHAR ) ?
                               *in_str_dup : REPLACEMENT_CHAR;
            break;
          case ALPHA_UCS2:
            /* If the PBM 8bit character is one not common to UCS2... */
            if ( NULL != (tmp_chr_ptr = 
                      (char *)strchr( pbm8_to_ucs2_not_common, *in_str_dup )) )
            {
              /* Replace it with the proper UCS2 value as hex 4-tuple */
              (void)strlcpy( out_str_dup, 
                                (char *)ucs2_to_pbm8_not_common[tmp_chr_ptr - 
                                                      pbm8_to_ucs2_not_common],
                                 UCS2_CHAR_LEN + 1);
              out_str_dup += UCS2_CHAR_LEN;
            }
            else
            {
              /* Converting from 8bit alphabet upper 16 bits always 0 */
              memset( out_str_dup, '0', 2 );
              out_str_dup += 2;
              CVT_TO_HEX_PAIR( *in_str_dup );
            }
            break;
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT: /* first check for PBM 8 bit @ character */
            if ( ( out_chset == ALPHA_GSM ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR_REPLACEMENT; 
            }
            else if ( ( out_chset == ALPHA_GSM_WITH_AT ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR; 
            }
            else
            {
              pbm_char_to_gsm( *in_str_dup, &out_str_dup );
            }
            out_str_dup++;
            break;
          /* If unsupported output character set specified */
          default:
            DS_AT_MSG0_ERROR("Conversion from PBM alpha limited to IRA, GSM, UCS2");
            return NULL;
        }
        in_str_dup += PBM_8BIT_CHAR_LEN;
        break;
      /* Input character set PBM UCS2 */
      case ALPHA_PBM_UCS2:
        switch ( out_chset )
        {
          case ALPHA_IRA:
            /* Conversion not possible if in the range 0x0100-0xFFFF */
            if ( *(in_str_dup + 1) != '\0' )
            {
              *out_str_dup++ = REPLACEMENT_CHAR;
              break;
            }
            /* PBM UCS2 and IRA T.50 are the same over the range 0x0000-0x007F */
            *out_str_dup++ = ( *in_str_dup <= MAX_GSM_CHAR ) ?
                               *in_str_dup : REPLACEMENT_CHAR;
            break;
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT: 
            /* Conversion not possible if in the range 0x0100-0xFFFF */
            if ( *(in_str_dup + 1) != '\0' )
            {
              *out_str_dup++ = REPLACEMENT_CHAR;
              break;
            }
            /* first check for PBM UCS2 bit @ character */
            if ( ( out_chset == ALPHA_GSM ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR_REPLACEMENT; 
            }
            else if ( ( out_chset == ALPHA_GSM_WITH_AT ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR; 
            }
            else
            {
              pbm_char_to_gsm( *in_str_dup, &out_str_dup );
            }
            out_str_dup++;
            break;
          case ALPHA_UCS2:
              CVT_TO_HEX_PAIR( *(in_str_dup + 1) );
              CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          /* If unknown output character set specified */
          default:
            DS_AT_MSG0_ERROR("Conversion from ALPHA_PBM_UCS2 is limited to IRA, GSM, UCS2");
            return NULL;
        }
        in_str_dup += PBM_UCS2_CHAR_LEN;
        break ;
      /* If unknown input character set specified */
      default:
        DS_AT_MSG1_ERROR("Invalid input character set specified %d",
                   in_chset);
        return NULL;
    }
  }
  
  if (out_chset == ALPHA_PBM_UCS2)
  {
    *out_str_dup++ = '\0';
  }
  *out_str_dup = '\0';
  return out_str_dup;
} /* dsatutil_convert_chset */

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_GSM_UCS2_TO_PBM_UCS2

DESCRIPTION
  This function performs the special conversion of one of the three GSM UCS2
  codings specified in 3GPP GSM TS 11.11 Annex B into PBM UCS2 format.  Sets 
  the terminating NULL (0x0000) character if there is enough room in out_ary
  for it; if there isn't then no error is thrown.

DEPENDENCIES
  None.

RETURN VALUE
  Error value enum

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_cme_error_e_type dsatutil_convert_gsm_ucs2_to_pbm_ucs2
(
  const char  *in_str,
  uint16      *out_ary,
  uint16       out_max /* size of out_ary (in uint16's, NOT bytes) */
)
{
  uint16  i;
  uint16  page_offset = 0;
  uint16  out_idx = 0;
  uint16  in_len;
  uint16  ucs2_char_tmp;
  uint8   octet_tmp;
  uint8   coding_scheme = 0;
  uint8   num_chars = 0;

  /* Coding scheme identifiers */
  #define GSM_UCS2_CODING_SCHEME_ONE    0x80
  #define GSM_UCS2_CODING_SCHEME_TWO    0x81
  #define GSM_UCS2_CODING_SCHEME_THREE  0x82

  in_len = strlen(in_str);
  if ( (in_len % HEX_CHAR_LEN) )
  {
    DS_AT_MSG1_ERROR("invalid input length %d",in_len);
    return DSAT_CME_UNKNOWN;
  }

  for (i = 0; i < in_len; i += HEX_CHAR_LEN)
  {
    /* First, determine that each octet is within the valid range for hex chars */
    if ( strchr(hex_chars, UPCASE(in_str[i])) == NULL ||
         strchr(hex_chars, UPCASE(in_str[i + 1])) == NULL )
    {
      return DSAT_CME_INVALID_CHAR_IN_TEXT;
    }

    /*--------------------------------------------------------------------------
     *  Process special meaning octets
     *    Note: each branch has a continue at the end so if a special header
     *          octet is processed, it will skip normal processing below
     *------------------------------------------------------------------------*/
    if ( i == 0 )
    {
      /* First octet determines which coding scheme we use */
      (void) dsatutil_hex_to_uint8( in_str, &coding_scheme );
      if ( (coding_scheme < GSM_UCS2_CODING_SCHEME_ONE) ||
           (coding_scheme > GSM_UCS2_CODING_SCHEME_THREE) )
      {
        DS_AT_MSG1_ERROR("invalid GSM coding scheme 0x%02x", coding_scheme);
        return DSAT_CME_INVALID_CHAR_IN_TEXT;
      }
      continue;
    }
    else if ( (i == 2) &&
              ( (coding_scheme == GSM_UCS2_CODING_SCHEME_TWO) ||
                (coding_scheme == GSM_UCS2_CODING_SCHEME_THREE) ) )
    {
      /* For coding schemes 2 & 3, the second octet is the number of characters 
         in the string */
      (void) dsatutil_hex_to_uint8( &in_str[i], &num_chars );
      if ( num_chars == 0 )
      {
        DS_AT_MSG0_ERROR("number of characters in coding scheme is 0");
        return DSAT_CME_UNKNOWN;
      }
      else if ( num_chars > out_max )
      {
        DS_AT_MSG2_ERROR("too many characters specified %d - max is %d",
                  num_chars, out_max);
        return DSAT_CME_TEXT_STRING_TOO_LONG;
      }
      continue;
    }
    else if ( i == 4 )
    {
      if ( coding_scheme == GSM_UCS2_CODING_SCHEME_TWO )
      {
        /* The 3rd octet in coding scheme two contains an 8 bit number which 
           defines bits 15 to 8 of a 16 bit base pointer, where bit 16 is set to 
           zero, and bits 7 to 1 are also set to zero */
        (void) dsatutil_hex_to_uint8( &in_str[i], &octet_tmp );
        page_offset = (octet_tmp << 7);
        continue;
      }
      else if ( coding_scheme == GSM_UCS2_CODING_SCHEME_THREE )
      {
        /* For coding scheme 3, the 2nd & 3rd octet specify the page offset,
           so make sure we have a 3rd octet then parse it with the 2nd octet */
        if (in_len < 6)
        {
          DS_AT_MSG1_ERROR("not enough characters (%d) for coding scheme 3",in_len);
          return DSAT_CME_UNKNOWN;
        }
        (void) dsatutil_hex_to_uint16( &in_str[i], &page_offset );
        i += HEX_CHAR_LEN;
        continue;
      }
    }

    /*--------------------------------------------------------------------------
     * Process normal characters
     *------------------------------------------------------------------------*/
    switch ( coding_scheme )
    {
      /* All octets now represent UCS2 characters.  Unused fields are set to 0xFFFF */
      case GSM_UCS2_CODING_SCHEME_ONE:
      {
        /* Make sure we have at least 2 octets left then parse */
        if ( (in_len - i) < UCS2_CHAR_LEN )
        {
          DS_AT_MSG2_ERROR("input character stream incomplete (len = %d i = %d)",in_len,i);
          return DSAT_CME_UNKNOWN;
        }
        (void) dsatutil_hex_to_uint16( &in_str[i], &ucs2_char_tmp );

        /* Ignore unused characters (0xFFFF) */
        if ( ucs2_char_tmp != 0xFFFF )
        {
          /* Make sure we have enough room in the output array for another character */
          if ( out_idx >= out_max )
          {
            DS_AT_MSG0_ERROR("not enough room in output");
            return DSAT_CME_TEXT_STRING_TOO_LONG;
          }
          out_ary[out_idx++] = ucs2_char_tmp;
        }
        i += HEX_CHAR_LEN; /* Add 2 to i since we processed 4 hex characters */
        break;
      } /* case GSM_UCS2_CODING_SCHEME_ONE */

      case GSM_UCS2_CODING_SCHEME_TWO:
      case GSM_UCS2_CODING_SCHEME_THREE:
      {
        /* Make sure we have enough room in the output array for another character */
        if ( out_idx >= out_max )
        {
          DS_AT_MSG0_ERROR("not enough room in output");
          return DSAT_CME_TEXT_STRING_TOO_LONG;
        }

        /* If bit 8 of the octet is set to zero, the remaining 7 bits of the 
           octet contain a GSM Default Alphabet character, whereas if bit 8 of 
           the octet is set to one, then the remaining seven bits are an offset 
           value */
        (void) dsatutil_hex_to_uint8( &in_str[i], &octet_tmp );
        if ( octet_tmp & 0x80 )
        {
          out_ary[out_idx++] = page_offset + (octet_tmp & 0x7F);
        }
        else
        {
          if ( octet_tmp == GSM_AT_CHAR )
          {
            out_ary[out_idx++] = 0x0040; /* '@' character */
          }
          else
          {
            char  conv_tmp;
            char  tmp_chr_ary[3];
            char *tmp_chr_ptr;

            /* Need to do special setup if using an escape character, but don't
               need to worry about GSM_AT_CHAR_REPLACEMENT) */
            tmp_chr_ary[0] = (char) octet_tmp;
            if ( octet_tmp == GSM_EXT_TBL_ESC_CHAR )
            {
              /* Need another octet since this is an escape sequence */
              i += HEX_CHAR_LEN;
              if ( (in_len - i) < HEX_CHAR_LEN )
              {
                DS_AT_MSG0_ERROR("input character stream incomplete for GSM escape sequence");
                return DSAT_CME_INVALID_CHAR_IN_TEXT;
              }
              (void) dsatutil_hex_to_uint8( &in_str[i], (uint8 *) &tmp_chr_ary[1] );
              tmp_chr_ary[2] = '\0';
            }
            else
            {
              tmp_chr_ary[1] = '\0';
            }
            tmp_chr_ptr = tmp_chr_ary;
            gsm_char_to_pbm( &tmp_chr_ptr, &conv_tmp );

            if ( NULL != (tmp_chr_ptr = 
                          (char *) strchr( pbm8_to_ucs2_not_common, conv_tmp )) )
            {
              /* GSM->PBM conversion doesn't match GSM->UCS2, so use the proper
                 UCS2 value */
              (void) dsatutil_hex_to_uint16( 
                        (const char *) ucs2_to_pbm8_not_common[tmp_chr_ptr - pbm8_to_ucs2_not_common],
                        &out_ary[out_idx++]);
            }
            else
            {
              /* Can safely use the PBM 8bit value as UCS2 */
              out_ary[out_idx++] = conv_tmp;
            }
          }
        }
        break;
      } /* case GSM_UCS2_CODING_SCHEME_TWO or THREE */
    } /* switch( coding_scheme ) */

    /* Break out of loop early if we hit the expected number of characters 
       (this is per spec) */
    if ( (num_chars != 0) && (out_idx == num_chars) )
    {
      break;
    }
  } /* for( each octet in input string )*/

  /* If the coding scheme specified the number of characters, make sure it
     matches what we processed */
  if ( (num_chars != 0) && (out_idx != num_chars) )
  {
    DS_AT_MSG2_ERROR("did not process expected number of characters; expected %d got %d",
              num_chars, out_idx);
    return DSAT_CME_UNKNOWN;
  }

  /* Set the terminating NULL character if we can */
  if ( out_idx < out_max )
  {
    out_ary[out_idx] = 0;
  }
  return DSAT_CME_NO_ERROR;
} /* dsatutil_convert_gsm_ucs2_to_pbm_ucs2() */

/*===========================================================================
FUNCTION DSATUTIL_HEX_TO_UINT8

DESCRIPTION
  Converts the first two characters of a hex string into uint8 
  representation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success, FALSE if dsatutil_atoi failed (i.e. invalid character
  in string)

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL boolean dsatutil_hex_to_uint8
(
  const char *in_octet,
  uint8      *out_uint8
)
{
  char tmp[3];
  dsat_num_item_type out_tmp;

  (void) dsatutil_memscpy((void*)tmp, sizeof(tmp),(void*)in_octet,2);
  tmp[2] = '\0';

  if ( ATOI_OK == dsatutil_atoi( &out_tmp, (const byte *) tmp, 16 ) )
  {
    *out_uint8 = (uint8) out_tmp;
    return TRUE;
  }
  return FALSE;
} /* dsatutil_hex_to_uint8() */

/*===========================================================================
FUNCTION DSATUTIL_HEX_TO_UINT16

DESCRIPTION
  Converts the first four characters of a hex string into uint16 
  representation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success, FALSE if dsatutil_atoi failed (i.e. invalid character
  in string)

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL boolean dsatutil_hex_to_uint16
(
  const char *in_str,
  uint16     *out_uint16
)
{
  char tmp[5];
  dsat_num_item_type out_tmp;
  (void) dsatutil_memscpy((void*)tmp, sizeof(tmp),(void*)in_str,4); 
  tmp[4] = '\0';
  if ( ATOI_OK == dsatutil_atoi( &out_tmp, (const byte *) tmp, 16 ) )
  {
    *out_uint16 = (uint16) out_tmp;
    return TRUE;
  }
  return FALSE;
} /* dsatutil_hex_to_uint16() */

/*===========================================================================
FUNCTION dsatutil_ucs2_strlen

DESCRIPTION
  This function counts the number of UCS2 characters in a string.
  The  string must be terminated by 0x0000.

DEPENDENCIES
  None

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.
===========================================================================*/
 uint16 dsatutil_ucs2_strlen
(
  const char* str
)
{
  uint16 i = 0;
  while( str[i] || str[i + 1])
  {
    i += 2;
  }
  return i/2;
}

/*===========================================================================

FUNCTION pbm_char_to_gsm

DESCRIPTION
  This function performs conversion of a single PBM 8bit character into GSM
  7bit alphabet. On return **gsm_out will point to the last converted character 
  written and the pointer *gsm_out may have been advanced by one place encode 
  a GSM character of the alphabet extension table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void pbm_char_to_gsm
(
  char pbm_in, 
  char **gsm_out
)
{
  char *tmp_chr_ptr = NULL;
  /* Complementary arrays for converting between PBM 8bit and GSM 7bit:
     for those values that aren't common to both.
     Used separate arrays for pbm_char_to_gsm() and gsm_char_to_pbm() to handle
     the conversion of GSM character '@'. */
  const char pbm8_to_gsm7_single[] = 
  {
    0x24,0x40,0x5F,0x90,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA1,0xA3,0xA4,
    0xA5,0xA7,0xBF,0xC4,0xC5,0xC6,0xC7,0xC9,0xD1,0xD6,0xD8,0xDC,0xDF,0xE0,0xE4,0xE5,
    0xE6,0xE8,0xE9,0xEC,0xF1,0xF2,0xF6,0xF8,0xF9,0xFC,0x00
  };
  const char gsm7_to_pbm8_single[] = 
  {
    0x02,0x20,0x11,0x10,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x40,0x01,0x24,
    0x03,0x5F,0x60,0x5B,0x0E,0x1C,0x09,0x1F,0x5D,0x5C,0x0B,0x5E,0x1E,0x7F,0x7B,0x0F,
    0x1D,0x04,0x05,0x07,0x7D,0x08,0x7C,0x0C,0x06,0x7E,0x00
  };

  ASSERT( NULL != gsm_out && NULL != *gsm_out );

  /* If pbm_in character is in the range 0x00-0x7F and
     is in the group of Basic Latin characters common to PBM 8bit and 
     GSM 7bit alphabets... */
  if ( pbm_in <= MAX_GSM_CHAR &&
       NULL == strchr( basic_latin_not_common, pbm_in ) )
  {
    **gsm_out = pbm_in;
  }
  /* If convertible to GSM 7bit as a single character... */
  else if ( NULL != 
            ( tmp_chr_ptr = (char *)strchr( pbm8_to_gsm7_single, pbm_in ) ) )
  {
    /* Output character at same offset in complementary array */
    **gsm_out = gsm7_to_pbm8_single[tmp_chr_ptr - pbm8_to_gsm7_single];
  }
  /* If convertible to GSM 7bit as two characters:
     part of the GSM 7bit alphabet extension table of 23.038... */
  else if ( NULL != 
            ( tmp_chr_ptr = (char *)strchr( pbm8_to_gsm7_exttbl, pbm_in ) ) )
  {
    /* Output escape character, advancing pointer... */
    *(*gsm_out)++ = (char)GSM_EXT_TBL_ESC_CHAR;
    /* Then output character at same offset in complementary array
       implementing extension table */
    **gsm_out = gsm7_to_pbm8_exttbl[tmp_chr_ptr - pbm8_to_gsm7_exttbl];
  }
  else
  {
    /* Replace inconvertible character with a space */
    **gsm_out = (char)REPLACEMENT_CHAR;
  }
}



/*===========================================================================

FUNCTION gsm_char_to_pbm

DESCRIPTION
  This function performs conversion of a one or two GSM 7bit characters
  into the PBM 8bit alphabet. If the escape character is encountered the
  function will advance gsm_in by one character and attempt to find the next
  character in the alphabet extension table. If found this is conversion 
  character, if not found it will use the replacement character. 
  On return *pbm_out will have been set to the converted character but 
  pointer will not have been advanced.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void gsm_char_to_pbm
(
  char **gsm_in, 
  char *pbm_out
)
{
  char *tmp_chr_ptr = NULL;
  /* Complementary arrays for converting between PBM 8bit and GSM 7bit:
     for those values that aren't common to both.
     Used separate arrays for pbm_char_to_gsm() and gsm_char_to_pbm() to handle
     the conversion of GSM character '@'. */
  const char pbm8_to_gsm7_single[] = 
  {
    0x24,0x5F,0x90,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA1,0xA3,0xA4,
    0xA5,0xA7,0xBF,0xC4,0xC5,0xC6,0xC7,0xC9,0xD1,0xD6,0xD8,0xDC,0xDF,0xE0,0xE4,
    0xE5,0xE6,0xE8,0xE9,0xEC,0xF1,0xF2,0xF6,0xF8,0xF9,0xFC,0x00
  };
  const char gsm7_to_pbm8_single[] = 
  {
    0x02,0x11,0x10,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x40,0x01,0x24,
    0x03,0x5F,0x60,0x5B,0x0E,0x1C,0x09,0x1F,0x5D,0x5C,0x0B,0x5E,0x1E,0x7F,0x7B,
    0x0F,0x1D,0x04,0x05,0x07,0x7D,0x08,0x7C,0x0C,0x06,0x7E,0x00
  };

  ASSERT( NULL != gsm_in && NULL != *gsm_in && NULL != pbm_out );

  /* If gsm_in character is in the range 0x00-0x7F and
     is in the group of Basic Latin characters common to PBM 8bit and 
     GSM 7bit alphabets... */
  if ( **gsm_in <= MAX_GSM_CHAR &&
       NULL == strchr( basic_latin_not_common, **gsm_in ) )
  {
    *pbm_out = **gsm_in;
  }
  /* If a single non-escape character is convertible to PBM 8bit... */
  else if ( GSM_EXT_TBL_ESC_CHAR != **gsm_in &&
            NULL != ( tmp_chr_ptr = 
                        (char *)strchr( gsm7_to_pbm8_single, **gsm_in ) ) )
  {
    /* Output character at same offset in complementary array */
    *pbm_out = pbm8_to_gsm7_single[tmp_chr_ptr - gsm7_to_pbm8_single];
  }
  /* If using the GSM 7bit alphabet extension table of 23.038... */
  else if ( GSM_EXT_TBL_ESC_CHAR == **gsm_in )
  {
    /* Advance to next character after escape character... */
    (*gsm_in)++;

    /* If that character is found in the extension table... */
    if ( NULL != ( tmp_chr_ptr = 
                     (char *)strchr( gsm7_to_pbm8_exttbl, **gsm_in ) ) )
    {
      /* Then output character at same offset in complementary array
         implementing extension table */
      *pbm_out = pbm8_to_gsm7_exttbl[tmp_chr_ptr - gsm7_to_pbm8_exttbl];
    }
    else
    {
      *pbm_out = (char)REPLACEMENT_CHAR;
    }
  }
  else
  {
    /* Replace inconvertible character with a space */
    *pbm_out = (char)REPLACEMENT_CHAR;
  }
}


/*===========================================================================

FUNCTION dsatutil_strtok

DESCRIPTION
  This function is used to return the tokens in tok_str string passed

DEPENDENCIES

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR : if there was any problem in execution
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_strtok
(
  const byte *str,
  uint32 str_len,
  byte delimiter,
  byte *tok_str
)
{
  uint32 i = 0;

  while(*str != delimiter)
  {
    if( i < str_len)
    {
      *(tok_str + i) = *str++;
      i++;
    }
    else
    {
      return DSAT_ERROR;
    }
  }
  return DSAT_OK;
} /* dsatutil_strtok */


/*===========================================================================

FUNCTION is_leap_year

DESCRIPTION
  This function is used to return whether the year passed is a leap year or not

DEPENDENCIES

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL int is_leap_year(int year)
{
  int result;

  /* year is not divisible by 4 */
  if((year % 4) != 0 )
  {
    result = FALSE;
  }
  /* year is divisible by 400 */
  else if ((year % 400) == 0 )
  {
    result = TRUE;
  }
  /* year is divisible by 100 */
  else if ((year % 100) == 0 )
  {
    result = FALSE;
  }
  else
  {
    result = TRUE;
  }

  return result;
} /* is_leap_year */


/*===========================================================================

FUNCTION dsatutil_is_date_valid

DESCRIPTION
  This function is used to check whether the year/day/month 
  combination is correct or not

DEPENDENCIES

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
int dsatutil_is_date_valid(int year, int month, int day)
{
  int valid = TRUE;
  int month_length[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

   /* 29 days in February in a leap year (including year 2000) */
  if(is_leap_year(year) )
  {
    month_length[2] = 29;
  }

  if(month < 1 || month > 12)
  {
    valid = FALSE;
  }
  else if(day < 1 || day > month_length[month])
  {
    valid = FALSE;
  }

  return valid;
} /* dsatutil_is_date_valid */

/*===========================================================================

FUNCTION DSATUTIL_SWAPN_WORD_MSB_LSB

DESCRIPTION
  This function is used to swap the msb and lsb of the given word array
DEPENDENCIES
  None.
RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatutil_swapn_word_msb_lsb
(
  const word * source_arr ,                       /* Pointer to Source array */
  word * dest_arr,                          /* Pointer to Destination array */
  dsat_num_item_type source_length,         /* No of words in source */
  dsat_num_item_type dest_length            /* No of words in destination */
)
{
  dsat_num_item_type index;
  if ( ( dest_length == 0 ) || ( source_length == 0 ) )
  {
    DS_AT_MSG0_HIGH("Invalid length in dsatutil_swapn_word_msb_lsb");
    return FALSE;
  }

  if( (source_arr == NULL) || (dest_arr == NULL) )
  {
    DS_AT_MSG0_HIGH("Invalid source or dest pointer");
    return FALSE;
  }
  
  /* Begining from the source add each word */
  /* by shifting it by 8 bits right and left.*/
  for(index = 0; index < MIN(source_length,dest_length) ;index++)
  {
    *dest_arr = (word)(( source_arr[index]>>8 ) | ( source_arr[index]<<8 ));
                  dest_arr++;
  }
  return TRUE;
  
  

}/* dsatutil_swap_msb_lsb_word_arr */

/*===========================================================================

FUNCTION DSATUTIL_REVERSEN_WORD

DESCRIPTION
  This function reverses the given word array by the specified number of words
DEPENDENCIES
  None.
RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/

boolean dsatutil_reversen_word
(
  word * wrd_arry,                        /* Word Array to be reversed */
  dsat_num_item_type len                 /* Number of words to be reversed */
)
{
  dsat_num_item_type index;
  word temp;

  if( (wrd_arry != NULL) && ( len > 0 ) )
  {
    /* Reverse the contents of the array */
    for(index = 0; index < (len/2) ;index++)
    {
      temp = wrd_arry[index];
      wrd_arry[index] = wrd_arry[(len - index) - 1];
      wrd_arry[(len - index) -1] = temp;
    }
    return TRUE;
  }
  else
  {
    DS_AT_MSG0_HIGH(" Invalid params in dsatutil_reverse_word_arr ");
    return FALSE;
  }
}/* dsatutil_reverse_word_arr */

/*===========================================================================

FUNCTION DSATUTIL_REVERSEN_BYTE

DESCRIPTION
  This function reverses the given byte array by the specified number of bytes.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    FALSE : if there was any problem in execution
    TRUE : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean dsatutil_reversen_byte
(
  byte * byte_arry,                      /*  Byte Array to be reversed */
  dsat_num_item_type len                 /* Number of bytes to be reversed */
)
{
  dsat_num_item_type index;
  byte temp;

  if( (byte_arry != NULL) && ( len > 0 ) )
  {
    /* Reverse the contents of the array */
    for(index = 0; index < (len/2) ;index++)
    {
      temp = byte_arry[index];
      byte_arry[index] = byte_arry[(len - index) - 1];
      byte_arry[(len - index) -1] = temp;
    }
    return TRUE;
  }
  else
  {
    DS_AT_MSG0_HIGH(" Invalid params in dsatutil_reverse_byte_arr ");
    return FALSE;
  }
}/* dsatutil_reversen_byte */

#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_ADDR_MASK

DESCRIPTION
  This function converts between an address mask string as used by +CGTFT and 
  ds_umts_address_mask_type. It determines the type of conversion needed and
  passes the work on to dsatutil_convert_tuple(). In the IPV6 mask conversion
  creates a dummy address representing the mask first.

DEPENDENCIES
  String buffer pointed to should be MAX_ADDR_SUBNET_STRING_LEN+1 bytes 
  in length to hold NULL terminated, maximum length IP address and mask for 
  INT_TO_STRING conversion.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_OK : if validation successful
    DSAT_ERROR : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatutil_convert_addr_mask
(
  tuple_convert_enum_type mode,             /* Mode of conversion      */
  ds_profile_3gpp_address_mask_type *addr_mask_num, /* Number representations  */
  ds_profile_3gpp_ip_version_enum_type* ip_vsn,
  byte *addr_mask_str                       /* NULL terminated string  */
)
{
#define ADDR_MASK_SEP_IPV4  4
#ifdef FEATURE_DATA_PS_IPV6
#define DELIMITERS_PER_IPV6 31
#define ADDR_MASK_SEP_IPV6  16
#define NUM_BITS_LESS_THAN_8  3
#define MASK_BITS_LESS_THAN_8 0x07
#endif

  uint8 num_delimiters = 0;
  byte *curr_char = addr_mask_str;
  dsat_num_item_type   cgpiaf_add_format = 0;
  dsat_num_item_type   cgpiaf_subnet_format = 0;
  tuple_type_enum_type tuple_type        = TUPLE_TYPE_MAX;
  /* word aligned temporary address */
  typedef union addr_u
  {
    uint32 addr32[4];
#ifdef FEATURE_DATA_PS_IPV6
    uint8  addr8[16];
#endif
  } addr_u_type;

  addr_u_type addr;

  ASSERT( ( NULL != addr_mask_num ) && ( NULL != addr_mask_str ) );

  if ( STRING_TO_INT == mode )
  {
#ifdef FEATURE_DATA_PS_IPV6
    /* for string to number conversion count the number of '.' delimiters in
       address mask string to determine IP version */
    while ( NULL !=
            (curr_char = (byte *)strchr((char *)curr_char, IPV4_DELIMITER)) )
    {
      num_delimiters++;
      curr_char++;
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    memset( addr.addr32, 0, (sizeof(uint32) * 4) );
  }
  else
  {
    /* for number to string conversion first copy address to 
       local word aligned structure */
    if ( DS_PROFILE_3GPP_IP_V6 == *ip_vsn )
    {
      addr.addr32[0] = addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[0];
      addr.addr32[1] = addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[1];
      addr.addr32[2] = addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[2];
      addr.addr32[3] = addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[3];
    }
    else
    {
      addr.addr32[0] = addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv4 ;
    }
  }

#ifdef FEATURE_DATA_PS_IPV6
  /* For IPV6 address / mask conversion in either mode... */
  if ( (STRING_TO_INT == mode && DELIMITERS_PER_IPV6 == num_delimiters ) ||
       (INT_TO_STRING == mode && DS_PROFILE_3GPP_IP_V6 == *ip_vsn) )
  {
    /* First convert between address and dot separated 8 bit decimal values 
       or the reverse. */
    /* Decides IPv6 address representation based on +CGPIAF setting*/
    
     tuple_type = TUPLE_TYPE_IPv6_OCTETS;
    
     if(INT_TO_STRING == mode)
     {
       cgpiaf_add_format = (dsat_num_item_type)dsatutil_get_val(
                                     DSATETSI_EXT_CGPIAF_IDX,0,0,NUM_TYPE);
       /* "1" Use IPv6-like colon-notation. IP address */
       if(1 == cgpiaf_add_format)
       {
          tuple_type = TUPLE_TYPE_IPv6;
       }
       else
       {
          tuple_type = TUPLE_TYPE_IPv6_OCTETS;
       }
     }
    if ( DSAT_OK != dsatutil_convert_tuple(
                        mode,
                        tuple_type,
                        addr.addr32,
                        addr_mask_str,
                        MAX_ADDR_SUBNET_STRING_LEN+1,
                        DSAT_CONVERT_IP) )
    {
      DS_AT_MSG0_ERROR("Failed IPV6 address conversion");
      return DSAT_ERROR;
    }

    if ( STRING_TO_INT == mode )
    {
      /* for string to number conversion copy address from word aligned 
         address back to  PROFILE struct */
      addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[0] = addr.addr32[0];
      addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[1] = addr.addr32[1];
      addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[2] = addr.addr32[2];
      addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[3] = addr.addr32[3];

    }

    /* Then convert Mask... */
    if ( INT_TO_STRING == mode )
    {
    /* for subnet mask to string conversion create a dummy address representing
       mask, The mask should have some number of contiguous MS bits set and the
       rest zeroed */
      /* starting at MS byte of array representing address */
      int index = sizeof(addr) - 1;
      uint8 num_bytes_set = 
        addr_mask_num->mask.prefix_len >> NUM_BITS_LESS_THAN_8;

      /* zero all bytes first, */
      memset( addr.addr32, 0, (sizeof(uint32) * 4) );

      /* then set whole bytes with all bits set, */
      while ( num_bytes_set && index >= 0 )
      {
        addr.addr8[index--] = 0xFF;
        num_bytes_set--;
      }

      /* then set the byte with only some of its upper bits set, if any */
      if ( index >= 0 )
      {
        addr.addr8[index] = (uint8)
          (0xFF << (8 - (addr_mask_num->mask.prefix_len & MASK_BITS_LESS_THAN_8)));
      }

      /* add '.' delimiter between address and mask parts */
      curr_char = addr_mask_str + strlen((char *)addr_mask_str);
      /* Decides IPv6 address representation based on +CGPIAF setting*/
      cgpiaf_add_format     = (dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_CGPIAF_IDX,0,0,NUM_TYPE);
      cgpiaf_subnet_format  = (dsat_num_item_type)dsatutil_get_val(
                                     DSATETSI_EXT_CGPIAF_IDX,0,1,NUM_TYPE);
      if(1 == cgpiaf_add_format && 1 == cgpiaf_subnet_format)
      {
        *curr_char++ = '/'; /* Subnet 1 ,Applying (forward slash) /CIDR */
      }else if(1 == cgpiaf_add_format && 0 == cgpiaf_subnet_format)
      {
        *curr_char++ = ' '; /* Subnet 0 ,Both IP Address and subnet mask are separated by a space.*/
      }
      else
      {
        *curr_char++ = '.'; /* Default setting when address format is 0 */
      }
      *curr_char = '\0';
    }
    else
    {
      /* for string to subnet mask number conversion... */
      num_delimiters = 0;
      curr_char = addr_mask_str;

      /* Leave current character pointing to 1st character of mask string */
      while ((num_delimiters < ADDR_MASK_SEP_IPV6) && 
             (NULL != (curr_char = (byte *)strchr((char *)curr_char, IPV4_DELIMITER))))
      {
        num_delimiters++;
        curr_char++;
      }
    }

    /* convert between mask and dot separated 8 bit decimal values 
       or the reverse conversion, */

    /* sanity check before passing buffer to dsatutil_convert_tuple */
    if (curr_char == NULL)
    {
      DS_AT_MSG0_ERROR("Invalid address mask string");
      return DSAT_ERROR;
    }

    if(INT_TO_STRING == mode)
    {
      /* Decides IPv6 address representation based on +CGPIAF setting*/
      cgpiaf_add_format     = (dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_CGPIAF_IDX,0,0,NUM_TYPE);
      cgpiaf_subnet_format  = (dsat_num_item_type)dsatutil_get_val(
                                     DSATETSI_EXT_CGPIAF_IDX,0,1,NUM_TYPE);
      if(1 == cgpiaf_add_format && 1 == cgpiaf_subnet_format)
      {
        /* Convert to integer */
         dsatutil_itoa ((uint32)addr_mask_num->mask.prefix_len,(byte *) curr_char, 10);
      }
      else
      {
        if(1 == cgpiaf_add_format)
        {
          tuple_type = TUPLE_TYPE_IPv6; 
        }
        else
        {
          tuple_type = TUPLE_TYPE_IPv6_OCTETS; 
        }
  
        if ( DSAT_OK != dsatutil_convert_tuple(
                             mode,
                             tuple_type,
                             addr.addr32,
                             curr_char,
                             (uint8)((MAX_ADDR_SUBNET_STRING_LEN+1) - 
                             (curr_char-addr_mask_str)),
                             DSAT_CONVERT_MASK) )
        {
          DS_AT_MSG0_ERROR("Failed IPV6 address conversion");
          return DSAT_ERROR;
        }
      }
    }
    else
    {
      if ( DSAT_OK != dsatutil_convert_tuple(
                    mode,
                    TUPLE_TYPE_IPv6_OCTETS,
                    addr.addr32,
                    curr_char,
                    (uint8)((MAX_ADDR_SUBNET_STRING_LEN+1) - 
                      (curr_char-addr_mask_str)),
                      DSAT_CONVERT_MASK) )
      {
        DS_AT_MSG0_ERROR("Failed IPV6 address conversion");
        return DSAT_ERROR;
      }
    }
    if ( STRING_TO_INT == mode )
    {
      /* for string to mask number conversion count number of bits set in 
         temp address: start from MS byte of array representing address... */
      int index = sizeof(addr) - 1;

      *ip_vsn = DS_PROFILE_3GPP_IP_V6;

      while ( index >= 0 )
      {
        if ( 0xFF == addr.addr8[index] )
        {
          /* count number of whole bytes set, */
          addr_mask_num->mask.prefix_len+= 8;
        }
        else if ( 0 != addr.addr8[index] )
        {
          /* and number of bits in the byte partially set */
          uint8 bit_mask = 0x80;

          while ( addr.addr8[index] & bit_mask )
          {
            addr_mask_num->mask.prefix_len++;
            bit_mask = bit_mask >> 1;
          }

          break;
        }
        else
        {
          break;
        }

        index--;
      }
    }
  }
  else 
#endif /* FEATURE_DATA_PS_IPV6 */
  {
    byte *mask_start = NULL;

    /* Convert IPV4 address to string or vv. */
    if ( DSAT_OK != dsatutil_convert_tuple(
                               mode,
                               TUPLE_TYPE_IPv4,
                               addr.addr32,
                               addr_mask_str,
                               MAX_IPADDR_STR_LEN,
                               DSAT_CONVERT_IP) )
    {
      DS_AT_MSG0_ERROR("Failed IPV4 address conversion");
      return DSAT_ERROR;
    }

    if ( INT_TO_STRING == mode )
    {
      /* add '.' delimiter between address and mask parts */
      curr_char = addr_mask_str + strlen((char *)addr_mask_str);
      *curr_char++ = '.';
      *curr_char = '\0';
      mask_start = curr_char;

    /* copy mask from DSUMTS  struct to local word aligned mask */
      addr.addr32[0] = addr_mask_num->mask.mask;
    }
    else /* STRING_TO_INT */
    {
      num_delimiters = 0;
      curr_char = addr_mask_str;

    /* copy local word aligned address back to DS PROFILE  struct */
      *ip_vsn = DS_PROFILE_3GPP_IP_V4;
      addr_mask_num->address.ds_profile_3gpp_tft_addr_ipv4 = addr.addr32[0];

      /* Leave current character pointing to 1st character of mask string */
      while ((num_delimiters < ADDR_MASK_SEP_IPV4) && 
             (NULL != (curr_char = (byte *)strchr((char *)curr_char,IPV4_DELIMITER))))
      {
        num_delimiters++;
        curr_char++; 
      }

      mask_start = curr_char;
    }

    /* sanity check before passing buffer to dsatutil_convert_tuple */
    if (mask_start == NULL)
    {
      DS_AT_MSG0_ERROR("Invalid address mask string");
      return DSAT_ERROR;
    }
    /* Convert mask */
    if ( DSAT_OK != dsatutil_convert_tuple(
                          mode,
                          TUPLE_TYPE_IPv4,
                          addr.addr32,
                          mask_start,
                          (uint8)(MAX_IPADDR_STR_LEN - strlen((char *)addr_mask_str)),
                          DSAT_CONVERT_MASK) )
    {
      DS_AT_MSG0_ERROR("Failed IPV4 address conversion");
      return DSAT_ERROR;
    }

    if ( STRING_TO_INT == mode )
    {
      /* copy from local word aligned mask back to packed DSUMTS struct */
      addr_mask_num->mask.mask = addr.addr32[0];
    }
  }
  return DSAT_OK;
} /* dsatutil_convert_addr_mask */

#endif /* FEATURE_DSAT_ETSI_DATA */


#ifdef FEATURE_DSAT_GOBI_MAINLINE
/*===========================================================================

FUNCTION DSATUTIL_GET_UQCN_VERSION

DESCRIPTION
  This command reads the appropriate NV item containing the UQCN version 
  information for UMTS / CDMA.

DEPENDENCIES
  NONE

RETURN VALUE
  uint32 - UQCN Version

SIDE EFFECTS
  None
  
===========================================================================*/
uint32 dsatutil_get_uqcn_version()
{
  static uint32 uqcn_version = 0;
  nv_item_type  ds_nv_item;                /* actual data item     */

  if (!uqcn_version) 
  {
    memset(&ds_nv_item, 0, sizeof(ds_nv_item));
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
    /* Fetch NV item #4995 */
    if (NV_DONE_S == dsatutil_get_nv_item(NV_CUST_UQCN_C2K_VERSION_I, &ds_nv_item))
    {
      uqcn_version = ds_nv_item.cust_uqcn_c2k_version;
    }
#elif (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
    /* Fetch NV item #4996 */
    if (NV_DONE_S == dsatutil_get_nv_item(NV_CUST_UQCN_UMTS_VERSION_I, &ds_nv_item))
    {
      uqcn_version = ds_nv_item.cust_uqcn_umts_version;
    }
#endif
  }

  return uqcn_version;
} /* dsatutil_get_uqcn_version */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

/*===========================================================================
FUNCTION UI_STRNICMP

DESCRIPTION
Case in-sensitive string compare.  This function returns a number greater
than 0 if s1 is greater than s2, a number less than 0 if s1 is less than
s2, and 0 if s1 is the same as s2.  The comparison continues until both
strings reach the null character or len characters are reached.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int ui_strnicmp(const char *s1, const char *s2, int len)
{
  int ret_val = 0;
  /* While there are still characters to test AND
   * *s1 and *s2 are not both NULL characters AND
   * *s1 and *s2 are the same character (when in upper case)
   * check the next character.
   * Otherwise, return the result from the last comparison which has the
   * difference or, if there is no difference, it is set to the value 0.
   */
  while (len-- && (*s1 || *s2) && ((ret_val = UPCASE(*s1) - UPCASE(*s2)) == 0))
  {
    s1++;
    s2++; 
  }
  return ret_val;
}

/*===========================================================================
FUNCTION  DSATUTIL_DECODE_UCS2_80_81_82_ENCODING

DESCRIPTION
  Extracts the UCS2 80, 81, or 82 encoded buffer and decodes into UCS2 little
  endian.

DEPENDENCIES
  None.

RETURN VALUE
  size_t len: Length of the output buffer, number of uint16 characters.
  
SIDE EFFECTS
  None
==============================================================================*/
size_t dsatutil_decode_ucs2_80_81_82_encoding( uint8 *buf_in, 
                                             size_t buf_in_len,
                                             uint16 *buf_out, 
                                             size_t buf_out_max_len)
{
  uint8 i, j;
  uint8 *in_p = buf_in;
  uint16 *out_p16 = buf_out;
  uint16 ucs2_base = 0;
  uint8 ucs2_len = 0;
  size_t buf_len = 0;

#define DSATUTIL_MMGSDI_UCS2_80 0x80
#define DSATUTIL_MMGSDI_UCS2_81 0x81
#define DSATUTIL_MMGSDI_UCS2_82 0x82

  if ((buf_in == NULL) || (buf_out == NULL)) {
    return buf_len;
  }

  in_p++;
  /* The first octet, if 0x80, 0x81, 0x82 denotes the UCS2 encoding, else
   * the GSM encoding is used.
   */
  DS_AT_MSG1_HIGH("In dsatutil_decode_ucs2_80_81_82_encoding : Incoming buffer encoding used 0x%02x", buf_in[0]);
  switch (buf_in[0]) 
  {
    case DSATUTIL_MMGSDI_UCS2_80:
      for (i=0,j=0; (i<(buf_out_max_len-1)) && (j < (buf_in_len-1)); i+=1, j+=2, in_p+=2, out_p16++) 
      {
        if((in_p == NULL)||(in_p+1 == NULL) || (out_p16 == NULL) ||(*in_p == 0xFF))
        {
          break;
        }
        else 
        {
          *out_p16 = *(in_p);
          *out_p16 <<= 8;
          *out_p16 |= *(in_p+1);
        }
      }
      buf_len = i;
      break;

    case DSATUTIL_MMGSDI_UCS2_81:
      /* the second-octet is length of UCS2 chars in the EF-SPN */
      ucs2_len = *(in_p++);
      if ((ucs2_len > 0) && (ucs2_len < buf_out_max_len+1) && (in_p != NULL))
      {
        /* the third octet contains an 8-bit number which defines bits
         * 15 - 8 of a 16-base pointer, where bit 16 is set to 0, and bits 7 - 1 
         * are also set to zero. These 16 bits consitute a base pointer to a 
         * half-page in the UCS2 code space, to be used with some or all of the 
         * remaining octets in the string. 
         */
        ucs2_base = *(in_p++);
        ucs2_base <<= 7;
        
        for (i=0, j=0; (i < ucs2_len) && (i < buf_in_len ) && (j < buf_out_max_len); i++, j++, in_p++, out_p16++) 
        {
          if((in_p == NULL) || (out_p16 == NULL))
          {
            break;
          }
          /*                                        UNICODE              :  GSM-7bit */
          *out_p16 = (*(in_p) & 0x80) ? ( ucs2_base + (*(in_p) & 0x7F) ) : *(in_p);
        }
        buf_len = i;
      }
      break;

    case DSATUTIL_MMGSDI_UCS2_82:
      ucs2_len = *(in_p++);
      if ((ucs2_len > 0) && (ucs2_len < buf_out_max_len) && (in_p != NULL) && (in_p+1 != NULL)) 
      {
        /* the third and fourth octets contain a 16-bit number which defines the complete
         * 16-bit base pointer to a 'half-page' in the UCS2 code space, for use with some
         * or all of the remaining octets in the string. 
         */
        ucs2_base = *(in_p++);
        ucs2_base <<= 8;
        ucs2_base |= *(in_p++);

        for (i=0, j=0; (i < ucs2_len) && (j < buf_out_max_len); i+=1, j+=1, in_p++, out_p16++) 
        {
          if((in_p == NULL) || (out_p16 == NULL))
          {
            break;
          }
          /*                                            UNICODE          :  GSM-7bit */
          *out_p16 = (*(in_p) & 0x80) ? ( ucs2_base + (*(in_p) & 0x7F) ) : *(in_p);
        }
        buf_len  = i;
      }
      break;

    default:
      DS_AT_MSG0_ERROR("Input buffer not encoded in UCS2 80,81 or 82");
      break;
  }

  return buf_len;
} /* dsatutil_decode_ucs2_80_81_82_encoding */

/*===========================================================================
 FUNCTION DSAT_PROCESS_EFS_DATA
 
 DESCRIPTION
   This function sends the data to the TE. Responsible for parsing the
   data handling special cases as well.
 
 DEPENDENCIES
   None.

 RETURN VALUE
   boolean: - TRUE in success case, FALSE otherwise
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
boolean dsat_process_efs_data
(
  dsat_string_item_type *buffer,
  dsat_num_item_type     size,
  dsm_item_type         *res_buff_ptr,
  dsat_efs_data_e_type   efs_type,
  void                  *user_data
)
{
  char                    local_buffer[30];
  word                    data_size = 0;
  dsat_string_item_type   *ptr = NULL;
  dsat_num_item_type      count = 0;
  dsat_num_item_type      line_count = 0;
  dsat_num_item_type      user_index = 0;
  dsat_bandpref_e_type   *result_list = NULL;
  boolean                  print_flag  = TRUE;

  if (buffer == NULL || res_buff_ptr == NULL || size <= 0)
  {
    return FALSE;
  }
  if (efs_type == DSAT_EFS_PREFMODE_QUERY && user_data == NULL) 
  {
    return FALSE;
  }

  memset(local_buffer, 0 ,sizeof(local_buffer));
  ptr = buffer;

  while (count < size && ptr != NULL)
  {
    count++;
    if (*ptr == ',' || count == size)
    {
       line_count++;

       /*Special case for BANDPREF, check which bands are valid*/
       /*Result List is sorted list index of bands present given by CM*/
      if (efs_type == DSAT_EFS_PREFMODE_QUERY) 
      {
        result_list = (dsat_bandpref_e_type   *)user_data;

        if (line_count != result_list[user_index]) 
        {
          print_flag = FALSE;
        }
      }
      /*Special case for MRU, insert the comma in the end before flushing the buffer*/
      if(efs_type == DSAT_EFS_MRU_TEST)
      {
        local_buffer[data_size] = *ptr;
        data_size++;
      }
      /*print flag by default is true, only used to handle special case 
        of PREFMODE_QUERY*/
      if (print_flag) 
      {
        user_index++;
        /*Flush the buffer to TE here once the token is parsed*/
        dsat_dsm_pushdown_tail(&res_buff_ptr,
                               local_buffer,
                               data_size,
                               DSM_ITEM_POOL(res_buff_ptr),
                               FALSE);
      }
      ptr++;
      memset(local_buffer, 0 ,sizeof(local_buffer));
      data_size = 0;
      print_flag = TRUE;
      continue;
    }
    local_buffer[data_size] = *ptr;
    data_size++;
    ptr++;
  }
  return TRUE;
} /*dsat_process_efs_data*/

/*===========================================================================
 FUNCTION DSAT_EFS_FILE_OPEN
 
 DESCRIPTION
   Open a given EFS file . Currently we support two types of efs_files
   1) Command Names: ALl command names are in EFS
   2) Strings: Over 500 bytes strings in EFS. Read it Dynamically here
 
 DEPENDENCIES
   None.
 RETURN VALUE
   fs_handle: -1 for error and fs_handle>=0 in success case
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
int dsat_efs_file_open
(
  char                *file_name,
  dsat_efs_file_e_type file_type
)
{
  #define         EFS_MAX_PATH_LEN  128
  char            file_path[EFS_MAX_PATH_LEN];
  const char      ATCOP_STR_DIR[]    = "/ds/atcop/strings/";
  const char      ATCOP_CMD_DIR[]    = "/ds/atcop/cmds/";

  const char       *sub_dir_path = NULL;
  dsat_err_e_type  err_info      = DSAT_ERR_NO_ERROR;
  int              fs_handle     = 0; 

  if (file_name == NULL)
  {
    err_info = DSAT_ERR_VALIDATION_FAIL;
    goto send_error;
  }

  if (file_type == DSAT_FILE_STR_TYPE)
  {
    sub_dir_path = ATCOP_STR_DIR;
  }
  else if (file_type == DSAT_FILE_CMD_TYPE)
  {
    sub_dir_path = ATCOP_CMD_DIR;
  }
  else
  {
    err_info = DSAT_ERR_VALIDATION_FAIL;
    goto send_error;
  }

  memset(file_path,0, EFS_MAX_PATH_LEN);

  if (strlcpy(file_path,  sub_dir_path, sizeof(file_path)) >= sizeof(file_path))
  {
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    goto send_error;
  }

  if (strlcat(file_path,  file_name, sizeof(file_path)) >= sizeof(file_path))
  {
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    goto send_error;
  }

  fs_handle = efs_open(file_path, O_RDONLY, DEFFILEMODE);
  
  if (-1 == fs_handle)
  {
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_error;
  }

  return fs_handle;

send_error:
  DS_AT_MSG_SPRINTF_1_HIGH ("File_Path:%s", file_path);
  DS_AT_MSG2_HIGH ("dsat_efs_file_open() file_name invalid len, err:%d, efs_erno:", 
                   err_info, efs_errno);
  return -1;
} /*dsat_efs_file_open*/

/*===========================================================================
 FUNCTION DSAT_GET_STRING_FILE_SIZE
 
 DESCRIPTION
   This function gets the file_size of the file in EFS. 
 
 DEPENDENCIES
   None.
 
 RETURN VALUE
   ret: Returns the number of bytes read for a given file
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
dsat_num_item_type dsat_get_string_efs_file_size
(
  int fs_handle
)
{
  struct  fs_stat   fs_buf;
  int     ret = 0;

  if (fs_handle < 0)
  {
    return 0;
  }

  ret = efs_fstat (fs_handle, &fs_buf);

  if (-1 == ret)
  {
    DS_AT_MSG1_ERROR("dsat_get_string_efs_file_size() Invalid file, efs_erno:%d", efs_errno);
    return 0;
  }

  DS_AT_MSG1_HIGH ("dsat_get_string_efs_file_size EFS File Size:%d", fs_buf.st_size);

  return (dsat_num_item_type)fs_buf.st_size;

} /*dsat_get_string_efs_file_size*/

/*===========================================================================
 FUNCTION DSAT_READ_EFS_DATA
 
 DESCRIPTION
   This function gets the file_size of the file in EFS. Currently we support two
   types of efs_files
   1) Command Names: ALl command names are in EFS
   2) Strings: Strings in EFS. Read it Dynamically here
 
 DEPENDENCIES
   This function will handle to free memory in case of FAILURES,
   in SUCCESS case, the caller is responsible for freeing the memory
 
 RETURN VALUE
   ret: Returns the number of bytes read for a given file
 
 SIDE EFFECTS
   None.
 ===========================================================================*/
dsat_num_item_type dsat_read_efs_data
(
  char                * file_name,
  dsat_efs_file_e_type  file_type,
  void                ** buffer
)
{
  dsat_err_e_type              err_info    = DSAT_ERR_NO_ERROR;
  int                          ret         = 0;
  dsat_num_item_type           local_size  = 0;
  dsat_num_item_type           fs_handle   = 0;

  /*Buffer passed to this function should be NULL*/
  if (buffer == NULL || *buffer != NULL || file_name == NULL)
  {
    err_info = DSAT_ERR_VALIDATION_FAIL;
    goto send_error;
  }

  /*open the efs_file here*/
  fs_handle = dsat_efs_file_open(file_name , file_type);

  /*Assuming file is open here already as valid file_desc is being passed 
    else we'll fail here and send_Error below*/
  local_size = dsat_get_string_efs_file_size(fs_handle);

  /*Comapare the size here*/
  if(local_size <= 0)
  {
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    goto send_error;
  }

  /* This function will allocate the buffer and caller function
     will free the buffer. Allocate buffer here */
  *buffer = (void *) dsat_alloc_memory (local_size , FALSE);

  /*Now file is open, read the contents and copy to buffer being passed*/
  ret = efs_read(fs_handle, *buffer, local_size);

  DS_AT_MSG1_HIGH ("EFS Bytes read:%d", ret);

  if (-1 == ret)
  {
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_error;
  }
  
  /*Close the file if its opened, and return result*/
  (void)efs_close(fs_handle);

  return ret;

send_error:
  DS_AT_MSG2_HIGH ("dsat_read_efs_data() file_name invalid length, err:%d, efs_erno:%d", 
                   err_info, efs_errno);

  /* Returning 0 bytes if any error occurs and free in the 
    buffer only in error case */
  dsatutil_free_memory_ext((void **) buffer);

  /*Close the file if its opened, and return result*/
  (void)efs_close(fs_handle);
  return 0;
} /*dsat_read_efs_data*/

/*===========================================================================
  FUNCTION dsatutil_convert_endianness16()

  DESCRIPTION
    Convert endianness of 16bit character string by swapping first and second
    byte of each character

  PARAMETERS
   buf: ptr to buffer that will be converted.
   size: size of 'buf' in terms of number of bytes. 'size' is expected to be
          an even number. if not, last byte at the end will be left untouched
  
  DEPENDENCIES
    None.

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void dsatutil_convert_endianness16
(
  unsigned char *buf,
  unsigned int size
)
{
  unsigned char tmp;
  unsigned int  i;

  if ( buf )
  {
    for ( i=0; i<size-1; i+=2 )
    {
      tmp        = *(buf+i);
      *(buf+i)   = *(buf+i+1);
      *(buf+i+1) = tmp;
    }
  }
}