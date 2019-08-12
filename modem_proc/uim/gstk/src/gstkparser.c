/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   P A R S E R


GENERAL DESCRIPTION
  This source file contains functions that parse the TLV from the card command
  and put into the GSTK format.


FUNCTIONS
  gstk_parser_address_ss_string_tlv
    This function parses the address or ss string TLV and populate the
    gstk_address_type passed in the function call with the info
    in the TLV.

  gstk_parser_alpha_identifier_tlv
    Generic Function used by GSTK to parse the Alpha Identifer TLV

  gstk_parser_bc_repeat_ind_tlv
    This function parses the bc repeat indicator TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_bearer_tlv
    This function parses the url TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_browser_id_tlv
    This function parses the browser identity TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_cap_config_tlv
    This function parses the ccp TLV and populate the
    gstk_cap_config_type passed in the function call with the info
    in the TLV.

  gstk_parser_duration_tlv
    This function parses the duration TLV and populate the gstk_duration_type
    passed in the function call with the info in the TLV.


  gstk_parser_evt_list_tlv
    This function parses the event list TLV and populate the
    gstk_evt_list_code_enum_type passed in the function call with the info
    in the TLV.

  gstk_parser_file_list_tlv
    This function parses the file list information provided.

  gstk_parser_icon_id_tlv
    This function parses the icon ID TLV, and populate the gstk_icon_type
    passed in the function call with the info in the TLV.

  gstk_parse_img_record
    Generic Function used to parse the Image

  gstk_parser_item_tlv
    Generic Function used by Populate Items for Menu Items or Select Items

  gstk_parser_item_icon_identifier_list_tlv
    Generic Function used to parse the Item Icon Identifier TLV .

  gstk_parser_item_identifier_tlv
    Generic Function used to parse the Item Icon Identifier TLV .

  gstk_parser_menu_num_items
    Generic Function used by GSTK to get the Characteristics for
    a Setup Menu Proactive Command

  gstk_parser_next_action_indicator_tlv
   Generic Function used to parse the Next Action Indicator TLV.

  gstk_parser_provisioning_ref_file_tlv
    This function parses the provisioning reference file TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_response_len_tlv
    This function parses the response length TLV and populate the max and min
    len passed in the function call with the info in the TLV.

  gstk_parser_sms_tpdu_tlv
    This function parses the sms tpdu TLV and populate the gstk_sms_tpdu_type
    passed in the function call with the info in the TLV.

  gstk_parser_subaddress_tlv
    This function parses the subaddress TLV and populate the
    gstk_subaddress_type passed in the function call with the info
    in the TLV.

  gstk_parser_text_string_tlv
    This function parses the text string/default text/ussd string TLV and
    populate the gstk_string_type passed in the function call with the info
    in the TLV.

  gstk_parser_tone_tlv
    This function parses the tone TLV and populate the gstk_tone_enum_type
    passed in the function call with the info in the TLV.

  gstk_parser_url_tlv
    This function parses the url TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_timer_id_tlv
    This function parses the timer id TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_timer_value_tlv
    This function parses the timer value TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_lang_tlv
    This function parses the lang TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_dtmf_tlv
    This function parses the dtmf TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_decode_text_string_body
    Stores decoded textual string in global text_string.text
    Used in text_string.length ^ text_string.data_coding_scheme


  gstk_gsdi_asynch_function_cb
    This callback function is being called when file access to mmgsdi returns
    to gstk task

  gstk_parser_is_orig_offset_valid
    This functions checks to see if the offset passed to any function is well
    within the range of the command pointer

  gstk_parser_get_next_tlv_offset
    This functions gets the offset of the next tlv if any. If there are more TLVs,
    more_tlv is set to true else it is set to false.

  gstk_parser_csd_bearer_param
    This function extracts the csd bearer information from the command data
    buffer.

  gstk_parser_gprs_bearer_param
    This function extracts the gprs bearer information from the command data
    buffer.

  gstk_parser_bearer_description_tlv
    Generic Function used by GSTK to parse the Bearer Description TLV

  gstk_parser_buffer_size_tlv
    Generic Function used by GSTK to parse the buffer size TLV

  gstk_convert_from_gsm_7bit_default
    Generic Function used by GSTK to convert a gsm 7 bit defualt buffer to a uint16

  gstk_decode_h80_usc2_alphabet
    Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer

  gstk_decode_h81_usc2_alphabet
    Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer

  gstk_decode_h82_usc2_alphabet
    Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer

  gstk_convert_gsm_default_to_gstk_alphabet
    Generic Function used by GSTK to convert a gsm default buffer to a uint16 buffer

  gstk_parser_alpha_identifier_tlv_ext
    This function parses the TLV that is coded according to ADN 11.11 format
    into gstk_alphabet_type format

  gstk_parser_icon_id_tlv_ext
    This function parses the icon ID TLV, and populate the gstk_icon_type
    passed in the function call with the info in the TLV.

  gstk_parser_other_address_tlv
    This function parses the other address TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_sim_me_interface_transport_level_tlv
    This function parses the SIM ME interface transport level TLV

  gstk_parser_ch_data_len_tlv
    This function parses the channel data length TLV

  gstk_parser_ch_data_tlv
    This function parses the channel data TLV

  gstk_parser_text_string_tlv_ext
    This function parses the TLV that is coded according to ADN 23.038 format
    into gstk_alphabet_type format

  gstk_parser_network_access_name_tlv
    This function parses the TLV that is coded according to 23.003 format
    into gstk_alphabet_type format

  gstk_parser_ussd_string_tlv
    This function parses the TLV that is coded according to ADN 23.038 format
    into gstk_ussd_string_type format

  gstk_decode_ussd_string_body
    Stores decoded textual string in global ussd_string.text

  gstk_parser_pli_nmr_utran_tlv
    This function parses the TLV that is coded as the UTRAN
    Measurement Qualifier in command Provide Local Information

  gstk_parser_activate_descriptor_tlv
    Generic Function used by GSTK to parse the Active Descriptor TLV


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstkparser.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/04/16   gm      Ignore "0xF" in the address string
11/12/15   vr      F3 Logging inprovements
04/27/15   vr      Non UI and non NW proactive command parallel processing
09/09/14   bd      Features clean up
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/14/14   vr      Use mmgsdi card slot session to read EF-IMG
07/31/14   dy      Fixing KW errors
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
05/06/14   gs      GSTK Featurization enhancements and clean up
03/27/14   vr      Increment item count only if item length is not 0 in case
                   of estk qmi interface
03/17/14   gm      Added support for IMS communication control
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
10/04/13   gs      F3 Message reduction
09/13/13   gm      Support for Recovery and hot-swap
06/24/13   xz      Support IMS Registration event download
05/01/13   gm      Proper decoding of USSD string
04/23/13   gm      Remove unused function
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/28/13   vr      Removed NULL termination for USSD string
01/17/13   hn      Support multiple sessions per subscription
11/13/12   bd      Support for Closed Subscriber Group feature
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Modify references to gstk_tcb
02/28/12   sg      Fix parsing of icon list identifier TLV
02/18/12   av      Cleaned up remaining ALWAYS OFF features
12/20/11   nb      GSDI Cleanup
10/31/11   sg      Class L and class M support
10/18/11   av      Removed features that are always OFF
10/16/11   nb      Add GSTK_PLMNWACT_LIST_TAG to known TAGs
10/04/11   sg      Use correct value for parsing IPv6 address
09/06/11   sg      Set icon size to 0 when not present
08/24/11   xz      Fix compiler warnings and LINT/KW errors
07/25/11   sg      Fix compiler warning
06/08/11   js      Fix for crash in gstk_parse_img_record()
04/24/11   dd      SCWS support
03/09/11   xz      Fix for EF-IMG file read
12/30/10   ssr     Fixed refresh request parsing
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
11/25/10   ssr     Fixed command detail parsing
08/09/10   bd      Changes to log Hex values of Alpha Id
08/30/10   xz      Fix compile warning
08/26/10   xz      Add support of EPS Call Control
05/23/10   xz      Add support of LTE NMR
05/11/10   xz      Add support of network rejection event
04/07/10   yt      Fixed compilation warnings
03/26/10   bd      Fixed parsing of image related EFs using MMGSDI session APIs
03/15/10   xz      Add support of LTE BIP
02/26/10   yt      Fixed KW errors
02/15/10   nb      Moved Dual Slot Changes
01/21/10   nb      Fix for skip_tlv API
11/06/09   dd      Check if the number of menu items exceeds the limit
10/28/09   nb      Release 7 Enhanced Network Selection
10/12/09   kp      Added const keyword for read only varibles
09/26/09   kp      ZI memory reduction changes
06/20/09   dd      Enable SMS for CDMA
05/20/09   dd      Fix the trailing characters in UCS2 0x82
05/18/09   sg      1) Added static keyword to gstk_gsdi_asynch_function_cb function
                   2) Added new variable rec_num_accessed_dummy_word of type
                      word to remove typecast warning.
03/25/09   yb      Removed Lint error due to redefinition of symbol OPTIONAL
                   in gstk.h
11/19/08   sk      Fixed warning
11/05/08   yb      Fixed partial comprehension error for CDMA SMS TPDU tag in
                   gstk_parser_sms_tpdu_tlv()
10/13/08   yb      Fixed documentation
09/14/08   sk      Fixed lint errors
04/04/08   sk      Fix access technology updates to the card
02/23/08   yb      Removed static return type for
                   gstk_parser_get_next_tlv_offset()
02/06/08   sk      Support for PLI UTRAN NMR
07/14/07   jar     Fixed memheap corruption which occurs while populating
                   the bearer list in gstk_parser_bearer_tlv()
06/12/07   sp      Removing static from gstk_is_tag_comprehension_req()
05/05/07   tml     Set icon present in item identifier icon list correctly
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
02/01/07   sk      Added Null ptr checks
01/29/07   tml     Added check if AID is greater than specification allowed
                   length
01/29/07   tml     Fixed compilation error
01/26/07   tml     Fixed text string parsing for gstk_alphabet_type
01/11/07   tml     Added rec_num for icon ID
10/23/06   sk      Fixed lint errors.
09/18/06   sk      Fixed lint errors.
08/28/06   sun     Fixed Warning
08/23/06   sun     Added support for NAA Refresh
08/21/06   sk      Added featurization for GPRS call control
06/07/06   sk      Lint fixes.
06/06/06   sk      Added support for network search mode
                   Lint fixes
05/23/06   tml     GPRS Call Control support and lint
05/21/06   sk      Fixed parsing problems for Open Channel APN.
05/03/06   tml     lint
11/21/05   sp      Fixed Lint errors
11/09/05   sk      Fixed Lint Errors
09/01/05   tml     Allow parsing for item identifier when tag comprehension
                   sets to false
08/25/05   tml     Fixed UCS2 0x81 and 0x82 support
08/10/05   sk      Updated offset value in gstk_parser_get_next_tlv_offset()
07/22/05   sk      Fixed potential data abort problems.
05/11/05   sst     Lint fixes
04/06/05   tml     Exposed utility functions for alpha GSM default unicode
                   conversion, removed 0xFF truncation for unicode
03/08/05  sk       Fix for STK empty calls
03/05/05   tml     Added tag comprehension support
02/02/05  sk       Updated function gstk_parser_network_access_name_tlv
02/02/05  sk       Added defintion for function
                   gstk_parser_network_access_name_ext_tlv
01/10/05  sk/tml   Added parser function for ussd string
12/27/04   jk      Fixed Compile Warnings.
12/04/04  sk/tml   Changed MSG_ERROR to MSG_HIGH for potentially optional
                   alpha ext parsing
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Return 7 bit format if number of input from cards exceed
                   256 number of bytes in 8 bit default format
08/18/04   tml     Decode USSD String accordingly
08/13/04   tml     Added memory free in error condition
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Fixed timer value parsing issue
07/14/04   tml     Change 7 bit to 8 bit dcs after decoding
06/10/04   tml     Added color coding scheme for icon
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports.  Added more debug messages
04/16/04   tml     linted
03/01/04   tml     Fixed address length issue
09/25/03   tml     Fixed Item ID comprehension issue
09/03/03   tml     Clean up string decoding method
07/25/03   tml     More appropriately handled icon id paring comprehension tag
07/07/03   tml     Fixed message priority
06/24/03   tml     Fixed tag validation issues
06/23/03   tml     Fixed decoding length
06/04/03   tml     Fixed alpha parsing
05/18/03   tml     linted
04/28/03   tml     Added icon support
04/21/03   tml     Added cdma sms support and TON/NPI support
03/19/03   tml     Icon ID/Icon List will return GSTK_UNSUPPORTED_COMMAND
                   until icon is fully supported by our software
03/06/03   tml     Updated enum names
02/26/03   jar     Added gstk_parser_file_list_tlv
02/25/03   tml     changed ERROR to ERROR_TONE to avoid conflict with atcop
02/13/03   tml     Added/Changed DM messages
02/07/03 tml/jar   Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "gstkutil.h"

#include "mmgsdisessionlib.h"
#include "err.h"
#include "string.h"
#include "task.h"
#include "uim_msg.h"

/* define index location for IMG EF */
#define GSTK_IMG_EF_WIDTH_INDEX 1
#define GSTK_IMG_EF_HEIGHT_INDEX 2
#define GSTK_IMG_EF_CODING_SCHEME_INDEX 3
#define GSTK_IMG_EF_FID_HIGH_BYTE_INDEX 4
#define GSTK_IMG_EF_FID_LOW_BYTE_INDEX 5
#define GSTK_IMG_EF_OFFSET_HIGH_BYTE_INDEX 6
#define GSTK_IMG_EF_OFFSET_LOW_BYTE_INDEX 7
#define GSTK_IMG_EF_SIZE_HIGH_BYTE_INDEX 8
#define GSTK_IMG_EF_SIZE_LOW_BYTE_INDEX 9

/* Max length of UTRAN Measurement Qualifier tlv */
#define GSTK_MAX_UTRAN_MSMT_QUAL_TLV_SIZE      0x03

static uint8                    *raw_img_data_p;
static mmgsdi_return_enum_type  gstk_gsdi_status = MMGSDI_SUCCESS;
static uint32                   returned_data_len;

/* The table index location should not be changed since
   the access to the table is index base */
static const uint16 gstk_gsmdefault_table[] =
{
  0x40, 0xA3, 0x24, 0xA5, 0xE8, 0xE9, 0xF9, 0xEC, 0xF2, 0xC7, 0x20, 0xD8, 0xF8, 0x20, 0xC5, 0xE5,
  0x394,0x5F, 0x3A6,0x393,0x39B,0x3A9,0x3A0,0x3A8,0x3A3,0x398,0x39E,0x20, 0xC6, 0xE6, 0xDF, 0xC9,
  0x20, 0x21, 0x22, 0x23, 0xA4, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0xA1, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0xC4, 0xD6, 0xD1, 0xDC, 0xA7,
  0xBF, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0xE4, 0xF6, 0xF1, 0xFC, 0xE0
};

/*===========================================================================
FUNCTION gstk_parser_is_orig_offset_valid

DESCRIPTION
  This functions checks to see if the offset passed to any function is well
  within the range of the command pointer

PARAMETERS
  int32 orig_offset  - Offset from which to start processing cmd
  int32 cmd_data_len - The command data length
  int spec_len       - The specified length of data to be considered for
                       processsing.

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => comprehension required
           FALSE => comprehension not required)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_parser_is_orig_offset_valid(int32 orig_offset, int32 cmd_data_len, int spec_len)
{
  if (orig_offset < 0) {
    UIM_MSG_ERR_2("Invalid Len: offset=0x%x, cmd_data_len=0x%x",
                  orig_offset, cmd_data_len);
    return FALSE;
  }
  if((orig_offset == 0) && (cmd_data_len == 0))
  {
    return TRUE;
  }

  if (orig_offset >= cmd_data_len) {
    UIM_MSG_ERR_2("Invalid Len: offset=0x%x, cmd_data_len=0x%x",
                  orig_offset, cmd_data_len);
    return FALSE;
  }
  return TRUE;
} /* gstk_parser_is_orig_offset_valid */ /*lint !e715 */


/*===========================================================================
FUNCTION gstk_parser_get_next_tlv_offset

DESCRIPTION
  This functions gets the offset of the next tlv if any. If there are more TLVs,
  more_tlv is set to true else it is set to false.

PARAMETERS
 int32 cmd_data_len     - command data length
 int32 count            - represents the last byte processed in the command
                          data buffer.
 int32* next_tlv_offset - pointer to the next tlv offset
 boolean* more_tlv      - indicates whether more TLVs are present or absent.

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => comprehension required
           FALSE => comprehension not required)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_get_next_tlv_offset(
  int32      cmd_data_len,
  int32      count,
  int32*     next_tlv_offset_ptr,
  boolean*   more_tlv_ptr)
{
  if ( (next_tlv_offset_ptr == NULL) || (more_tlv_ptr == NULL) ) {
    return GSTK_BAD_PARAM;
  }

  *more_tlv_ptr = TRUE;

  if ( (cmd_data_len - count) > 0 ) {
    /* more TLVs */
    /* if cmd_data_len - count == 1 -> only has Tag
       => Still return success and new offset since the next tlv
          parsing should catch whether this is a problem or not
          based on whether the TLV to be parsed is Mandatory
          or Optional */
    /* if cmd_data_len - count == 2 -> has both Tag and Length */
    /* if cmd_data-len - count > 2 -> has T, L and V */
    *next_tlv_offset_ptr = count;
  }
  else if ((cmd_data_len - count) == 0)
  { /* no more TLVs */
    *next_tlv_offset_ptr = count;
    *more_tlv_ptr = FALSE;
  }
  else {
    /* if cmd_data_len - count == 1 -> only have Tag */
    /* if cmd_data_len - count < 0 -> inconsistent length */
    UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x", cmd_data_len, count);
    return GSTK_INVALID_LENGTH;
  }
  return GSTK_SUCCESS;
} /* gstk_parser_get_next_tlv_offset */

/*===========================================================================
FUNCTION gstk_find_next_tlv_for_parsing

DESCRIPTION
  This functions gets the offset of the next tlv if any. If there are more TLVs,
  more_tlv is set to true else it is set to false.

PARAMETERS
  int32 tag_index       - tag index location
  uint8 tag             - tag
  uint8 *data_buf_ptr   - data buffer
  int32 data_buf_len    - data buffer len
  int32 *next_tag_index - next tag index location

DEPENDENCIES
  None

RETURN VALUE
  gstk_parsing_result_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_parsing_result_enum_type gstk_find_next_tlv_for_parsing(
  int32       tag_index,
  uint8       tag,
  const uint8 *data_buf_ptr,
  int32       data_buf_len,
                                                            int32 *next_tag_index )
{
  int i = tag_index;

  if(next_tag_index == NULL)
  {
    UIM_MSG_ERR_0("Next Tag Index ERR: NULL");
    return GSTK_TLV_PARSING_ERROR;
  }

  if(data_buf_ptr == NULL)
  {
    UIM_MSG_ERR_0("Data Buf PTR ERR: NULL");
    return GSTK_TLV_PARSING_ERROR;
  }
  *next_tag_index = i;

  /* i is located at the tag value */
  while (i <= data_buf_len) {
    /* Found another TLV, Update the next tag index */
    *next_tag_index = i;
    /* check if i still within bound */
    if (i == data_buf_len) {
      /* reach the end of the data, done parsing */
      return GSTK_TLV_PARSING_DONE;
    }
    if (i == data_buf_len - 1) {
      /* the data_buf_len indicates that there is only Tag, but no Len or Value
         for the next "TLV" */
      return GSTK_TLV_PARSING_ERROR;
    }

    /* Check if this tag is valid now */
    if (gstk_valid_tag(data_buf_ptr[i], (tags_enum_type)tag)) {
      return GSTK_TLV_PARSING_CONTINUE;
    }
    /* still not valid tag, continue with the next tag */
    /* go through the data buf */
    if (data_buf_ptr[*next_tag_index+1] > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT) {
      /* 2 bytes length */
      /* Check if the total len include the extra length byte */
      if (i == data_buf_len - 2) {
        /* The data_buf_len indicates that there is only Tag, 0x80, but
           doesn't contain the third byte that has the actual length */
        return GSTK_TLV_PARSING_ERROR;
      }
      i = i + data_buf_ptr[*next_tag_index+2] + 3;
    }
    else {
      i = i + data_buf_ptr[*next_tag_index+1] + 2;
    }
  }
  /* data length mismatch between BER TLV and the total Simple TLV length */
  return GSTK_TLV_PARSING_ERROR;
} /* gstk_find_next_tlv_for_parsing */


/*===========================================================================
FUNCTION gstk_continue_parsing

DESCRIPTION
  This function checks what gstk return status should be returned.

PARAMETERS
 uint8                         tag                 - tag
 gstk_parsing_result_enum_type tag_parsing_result  - result of the parsing
 boolean                       man_opt_specifier   - Mandatory or Optional
 gstk_status_enum_type*        gstk_status         - return status

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => Continue the parsing
           FALSE => Do not continue the parsing )

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static boolean gstk_continue_parsing(tags_enum_type tag, gstk_parsing_result_enum_type tag_parsing_result,
                              boolean man_opt_specifier, gstk_status_enum_type *gstk_status)
{
  if(gstk_status == NULL)
  {
    UIM_MSG_ERR_0("GSTK STATUS ERR: NULL");
    return FALSE;
  }

  switch (tag_parsing_result) {
    case GSTK_TLV_PARSING_ERROR:
      UIM_MSG_ERR_1("Invalid Command TLV w/ Tag 0x%x", tag);
      *gstk_status = GSTK_INVALID_COMMAND;
      return FALSE;

    case GSTK_TLV_PARSING_DONE:
      if (man_opt_specifier == GSTK_MANDATORY) {
        UIM_MSG_ERR_1("No Match for (M) TLV w/ Tag 0x%x", tag);
        *gstk_status = GSTK_INVALID_COMMAND;
      }
      else {
        UIM_MSG_HIGH_1("Done Parsing (O) TLV w/ Tag 0x%x", tag);
        *gstk_status = GSTK_SUCCESS;
      }
      return FALSE;

    case GSTK_TLV_PARSING_CONTINUE:
      /* continue with this TLV parsing */
      return TRUE;
  }
  return FALSE;
}

/*===========================================================================
FUNCTION gstk_is_tag_comprehension_req

DESCRIPTION
  This function checks if the tag has the comprehension required  flag set.

PARAMETERS
  tag: [Input] tag extracted from the card data buffer

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => comprehension required
           FALSE => comprehension not required)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_tag_comprehension_req(uint8 tag)
{

    if (tag > 0x80) { /* tag with CR */
      return TRUE;
    }
    else{ /* comprehension not required */
      return FALSE;
    }

} /* gstk_is_tag_comprehension_req*/

/*===========================================================================
FUNCTION gstk_is_tag_known

DESCRIPTION
  Check for proactive command tag is a known tag value or not

PARAMETERS
  tag: [Input] tag extracted from the card data buffer

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => this is a known tag
           FALSE => this is an unknown tag)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_is_tag_known(uint8 tag)
{
  /* Remove the CR bit */
  tag = tag & 0x7F;
  if (((tag >= (int)GSTK_COMMAND_DETAILS_TAG) && (tag <= (int)GSTK_CARD_READER_IDENTIFIER_TAG)) ||
      (tag == (int)GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG) ||
      (tag == (int)GSTK_OTHER_ADDRESS_TAG) ||
      (tag == (int)GSTK_NETWORK_ACCESS_NAME_TAG) ||
      (tag == (int)GSTK_CDMA_SMS_TPDU_TAG)       ||
      (tag == (int)GSTK_PLMNWACT_LIST_TAG))
  {
    return TRUE;
  }
  else {
    UIM_MSG_ERR_1("Unknown Tag value 0x%x", tag);
    return FALSE;
  }

} /* gstk_is_tag_known*/

/*===========================================================================
FUNCTION gstk_go_to_next_tag_index

DESCRIPTION
  Returned the next tag index location

PARAMETERS
  int32 *expected_next_tag_index
  int32 orig_tag_index
  int32 length_of_length_field
  int32 tlv_value_length

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => this is a known tag
           FALSE => this is an unknown tag)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_go_to_next_tag_index(
  int32 *expected_next_tag_index,
  int32 orig_tag_index,
  int32 length_of_length_field,
  int32 tlv_value_length)
{
  if(expected_next_tag_index == NULL)
  {
    UIM_MSG_ERR_0("expected_next_tag Index ERR: NULL");
    return;
  }

  if (*expected_next_tag_index <= (orig_tag_index + length_of_length_field + tlv_value_length + 1)) {
    *expected_next_tag_index = orig_tag_index + length_of_length_field + tlv_value_length + 1;
  }
} /* gstk_go_to_next_tag_index */

/*===========================================================================
FUNCTION gstk_is_tag_valid

DESCRIPTION
  Check for proactive command tag matches

PARAMETERS
  tag: [Input] tag extracted from the card data buffer

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => comprehension required
           FALSE => comprehension not required)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_parsing_result_enum_type gstk_is_tag_valid(
                              const uint32 *ref_id,
                              uint8         sim_tlv_tag,
                              uint8         tag,
                              boolean       man_opt_specifier,
                              uint8        *data,
                              int32        *count,
                              int32         data_len)
{
  int32 next_tag_index = 0;
  gstk_parsing_result_enum_type tag_parsing_result = GSTK_TLV_PARSING_CONTINUE;

  if ((ref_id != NULL) && (GSTK_CMD_REF_TABLE_INDEX(*ref_id) >= GSTK_MAX_PROACTIVE_COMMAND)) {
    return GSTK_TLV_PARSING_ERROR;
  }

  /* Check for input param */
  if ((data == NULL) || (count == NULL)) {
    UIM_MSG_ERR_0("Null Data ptr and/or count ptr");
    return GSTK_TLV_PARSING_ERROR;
  }

  /* Check for tag field index validity */
  if ((*count > data_len -1) || (*count < 0)) {
    UIM_MSG_ERR_0("Invalid Length for TLV");
    return GSTK_TLV_PARSING_ERROR;
  }
  /* Check if the tag matches */
  if (gstk_valid_tag(sim_tlv_tag, (tags_enum_type)tag)) {
    /* Check if the TLV consists of at least the "length" field" */
    if (*count == data_len -1) {
      /* the TLV contains only tag */
      if (man_opt_specifier == GSTK_MANDATORY) {
        return GSTK_TLV_PARSING_ERROR;
      }
      else {
        /* optional TLV, it is ok */
        return GSTK_TLV_PARSING_DONE;
      }
    }
    else if (*count == data_len -2) {
      /* the TLV contains 1 byte of length field, check to make sure
         the value is not 0x81 */
      if (data[*count + 1] == 0x81) {
        /* error since it indicates that it has a value == 0x81 => there should
           be at least 2 bytes of length fields */
        if (man_opt_specifier == GSTK_MANDATORY) {
          return GSTK_TLV_PARSING_ERROR;
        }
        else {
          /* optional TLV, it is ok */
          return GSTK_TLV_PARSING_DONE;
        }
      }
    }
    tag_parsing_result = GSTK_TLV_PARSING_CONTINUE;
  }
  else {
    UIM_MSG_ERR_3("sim tlv tag: 0x%x != exp tag: 0x%x (M/O = %x)",
                  sim_tlv_tag, tag, man_opt_specifier);
    if (gstk_is_tag_known(sim_tlv_tag)) {
      /* A known but unexpected tag */
      if (man_opt_specifier == GSTK_MANDATORY) {
        /* skip the tlv to next location */
        tag_parsing_result = gstk_find_next_tlv_for_parsing(*count, tag,
                                                            data, data_len,
                                                            &next_tag_index);
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
        *count = next_tag_index;
      }
      else { /* optional tag */
        /* return  with the same location of the count */
        tag_parsing_result = GSTK_TLV_PARSING_DONE;
      }
    }
    else { /* unknown tag */
      if (gstk_is_tag_comprehension_req(sim_tlv_tag)) {
        tag_parsing_result = GSTK_TLV_PARSING_ERROR;
      }
      else {
        /* comprehension not required */
        if (man_opt_specifier == GSTK_MANDATORY) {
          /* skip the tlv to next location */
          tag_parsing_result = gstk_find_next_tlv_for_parsing(*count, tag,
                                                            data, data_len,
                                                            &next_tag_index);
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
          }
          *count = next_tag_index;
        }
        else { /* optional tag */
          /* return  with the same location of the count */
          tag_parsing_result = GSTK_TLV_PARSING_DONE;
        }
      }
    }
  }
  return tag_parsing_result;
}/*lint !e818 *data suppression for externalized function */
/* gstk_is_tag_valid */

/*===========================================================================
FUNCTION gstk_decode_text_string_body

DESCRIPTION
    Stores decoded textual string in global text_string.text
    Used in text_string.length ^ text_string.data_coding_scheme

DEPENDENCIES
    If default is TRUE use default text, else use text_string

RETURN VALUE
    Updated Counter Value

===========================================================================*/
uint8 gstk_decode_text_string_body(tags_enum_type   tag,
                                   const uint8     *command_data,
                                   int32            offset,
                                   gstk_string_type *text_string)
{
    uint8              dcs = 0;
    int32              text_length = 0;
    int32              new_count = 0;
    int                additional_int = 0;
    int                additional_mod = 0;
    gstk_dcs_enum_type dcs_enum;

    new_count = offset;

    if(command_data == NULL || text_string == NULL)
    {
      UIM_MSG_ERR_2("In gstk_decode_text_string_body Command Data 0x%x, Text String 0x%x",
                    command_data, text_string);
      return int32touint8(new_count);
    }

    dcs = text_string->dcs;

    text_length = text_string->length; /*  length before decoding */
    UIM_MSG_HIGH_2("In gstk_decode_text_string_body(): dcs = %x, text_length = %x ",
                   dcs, text_length);

    if ((tag == GSTK_TEXT_STRING_TAG) || (tag == GSTK_DEFAULT_TEXT_TAG)) {
      dcs_enum = gstk_util_decode_dcs_type(FALSE, dcs);
    }
    else { /* USSD string => Cell Broadcast DCS */
      dcs_enum = gstk_util_decode_dcs_type(TRUE, dcs);
    }

    switch(dcs_enum) {
    case GSTK_DCS_7_BIT:
      gstk_util_dump_byte_array(" == command_data", &command_data[new_count], text_length);

      if (text_length > 223) {
        /* will exceed 256 after decoding, send to application so that it can decode */
        text_string->text = GSTK_CALLOC(int32touint32(text_length + 1) * sizeof(uint8));
        if (text_string->text == NULL)
        {
          return 0;
        }
        memset(text_string->text, 0, int32touint32(text_length + 1));
        (void)memscpy(text_string->text, int32touint32(text_length),
               &(command_data[new_count]), int32touint32(text_length));

        /* null terminate the string */
        text_string->text[text_length] = 0x00;

        new_count += text_length;
        /* increment to include null termination */
        text_length++;
      }
      else {
        /* allocate the memory */
        additional_int = text_length / 7;
        additional_mod = text_length % 7;
        if (additional_mod > 0) {
          additional_mod = 1;
        }
        text_string->text = GSTK_CALLOC(
            int32touint32(text_length + 1 + additional_mod + additional_int) * sizeof(uint8));

        if (text_string->text == NULL)
        {
          return 0;
        }

        memset(text_string->text, 0x00, int32touint32(text_length + 1 + additional_mod + additional_int));

        new_count += text_length;

        /* Convert to 8-bit text */
        text_length =  gstk_util_unpack_7_bit_chars(
          &command_data[new_count-text_length],
          int32touint8(text_length),
          text_string->text );

        /* add the null terminated char */
        text_string->text[text_length] = 0x00;

        /* increment the length to reflect the null terminated char */
        text_length++;

        text_string->dcs = text_string->dcs | 0x04;
        if ((tag == GSTK_TEXT_STRING_TAG) || (tag == GSTK_DEFAULT_TEXT_TAG)) {
          dcs_enum = gstk_util_decode_dcs_type(FALSE, text_string->dcs);
        }
        else {
          dcs_enum = gstk_util_decode_dcs_type(TRUE, text_string->dcs);
        }
        if (dcs_enum == GSTK_DCS_7_BIT) {
          /* default to data coding 8 bit = 0xF4 */
          text_string->dcs = 0xF4;
        }
      }

      break;

    case GSTK_DCS_8_BIT:
    case GSTK_DCS_UCS2_8_BIT:
    case GSTK_DCS_UCS2_7_BIT:
      /* Unpacked */
      /* Expanded default alphabet */
      text_string->text = GSTK_CALLOC(int32touint32(text_length + 1) * sizeof(uint8));
      if (text_string->text == NULL)
      {
        return 0;
      }
      memset(text_string->text, 0, int32touint32(text_length + 1));
      (void)memscpy(text_string->text, int32touint32(text_length),
      	       &(command_data[new_count]), int32touint32(text_length));

      /* null terminate the string */
      text_string->text[text_length] = 0x00;

      new_count += text_length;
      /* increment to include null termination */
      text_length++;

      /* new_count does not need to increase since it represents the raw TLV data count,
      ** which is not affected by unpacking or null char at the end of the text string */
      break;

    default:
      UIM_MSG_ERR_1("unable to determine dcs 0x%x", dcs);
      return 0;
    }

    gstk_util_dump_byte_array("TEXT", text_string->text, text_length);

    if(text_length != text_string->length)
    {
       text_string->length = int32touint8(text_length);
    }
    return int32touint8(new_count);
} /*gstk_decode_text_string_body*/

/*===========================================================================
FUNCTION gstk_decode_text_string_body_ext

DESCRIPTION
    Stores decoded text string format (for text string tlv) into
    gstk_alphabet_type

DEPENDENCIES
    If default is TRUE use default text, else use text_string

RETURN VALUE
    Updated Counter Value

===========================================================================*/
static int32 gstk_decode_text_string_body_ext(
  tags_enum_type      tag,
  const uint8        *command_data_ptr,
  int32               orig_offset,
  gstk_alphabet_type *text_string_ptr)
{
  uint8              dcs              = 0;
  int32              orig_text_length = 0;
  int32              new_count        = orig_offset;
  int                additional_int   = 0;
  int                additional_mod   = 0;
  int                i                = 0;
  gstk_dcs_enum_type dcs_enum;
  int32              new_text_length  = 0;
  int32              seven_bits_per_byte  = 7;

  if(command_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Command Data ERR: NULL");
    return new_count;
  }

  if(text_string_ptr == NULL)
  {
    UIM_MSG_ERR_0("Text string tag Index ERR: NULL");
    return new_count;
  }

  dcs = command_data_ptr[new_count];

  /* increment past DCS */
  new_count++;

  orig_text_length = text_string_ptr->length; /*  length before decoding (from the card) */
  UIM_MSG_HIGH_2(" In GSTK_DECODE_TEXT_STRING_BODY_EXT: dcs = 0x%x, text_length = 0x%x",
                 dcs, orig_text_length);

  if ((tag == GSTK_TEXT_STRING_TAG) || (tag == GSTK_DEFAULT_TEXT_TAG))
  {
    /* Non Cell Broadcast DCS */
    dcs_enum = gstk_util_decode_dcs_type(FALSE, dcs);
  }
  else
  {
    /* USSD string => Cell Broadcast DCS */
    dcs_enum = gstk_util_decode_dcs_type(TRUE, dcs);
  }

  gstk_util_dump_byte_array("Orig command_data", &command_data_ptr[new_count], orig_text_length);

  switch(dcs_enum)
  {
    case GSTK_DCS_7_BIT:
      /* allocate the memory */
      additional_int = orig_text_length / seven_bits_per_byte;
      additional_mod = orig_text_length % seven_bits_per_byte;
      if (additional_mod > 0)
      {
        /* To compensate the additional byte needed as a result of unpacking
           1 7-bit packed format into the unpacked format when the number of
           7-bits in the data is not divisible by 7 */
        additional_mod = 1;
      }
      /* with Null terminated byte */
      new_text_length = orig_text_length + 1 + additional_mod + additional_int;
      text_string_ptr->alphabet = GSTK_CALLOC(
        int32touint32(new_text_length) * sizeof(uint16));

      if (text_string_ptr->alphabet == NULL)
      {
        return 0;
      }

      memset(text_string_ptr->alphabet, 0x00, int32touint32(new_text_length)*sizeof(uint16));

      new_count += orig_text_length;

      /* Convert to 8-bit text */
      orig_text_length =  gstk_util_unpack_7_bit_chars_ext (
        &command_data_ptr[new_count-orig_text_length],
        int32touint8(orig_text_length),
        text_string_ptr->alphabet );
      orig_text_length++; /* Add null terminated byte */
      if (orig_text_length != new_text_length)
      {
        UIM_MSG_ERR_2("Length after 7bit unpack 0x%x != new text Len calculated 0x%x",
                      orig_text_length, new_text_length);
        new_text_length = orig_text_length;
      }
      break;

    case GSTK_DCS_8_BIT:
      new_text_length = orig_text_length + 1; /* Include Null terminated byte */
      text_string_ptr->alphabet = GSTK_CALLOC(int32touint32(new_text_length) * sizeof(uint16));
      if (text_string_ptr->alphabet == NULL)
      {
        return 0;
      }
      memset(text_string_ptr->alphabet, 0, int32touint32(new_text_length)*sizeof(uint16));
      gstk_util_dump_byte_array("command_data_ptr", &command_data_ptr[new_count], orig_text_length);
      for (i = 0; i < orig_text_length; i++)
      {
        text_string_ptr->alphabet[i] = (uint16)command_data_ptr[new_count+i];
      }
      new_count += orig_text_length;
      break;

    case GSTK_DCS_UCS2_8_BIT:
    case GSTK_DCS_UCS2_7_BIT:
      /* Unpacked */
      /* Expanded default alphabet */
      new_text_length = orig_text_length/2 + 1; /* with null terminated byte */
      text_string_ptr->alphabet = GSTK_CALLOC(int32touint32(new_text_length) * sizeof(uint16));
      if (text_string_ptr->alphabet == NULL)
      {
        return 0;
      }
      memset(text_string_ptr->alphabet, 0, int32touint32(new_text_length)*sizeof(uint16));

      for (i = 0; i < orig_text_length/2; i++)
      {
        text_string_ptr->alphabet[i] = (command_data_ptr[new_count+2*i] << 8) | command_data_ptr[new_count+2*i+1];
      }

      new_count += orig_text_length;
      break;

    default:
      UIM_MSG_ERR_1("unable to determine dcs 0x%x", dcs);
      return 0;
  }

  for (i = 0; i< new_text_length; i++)
  {
    UIM_MSG_HIGH_1(" TEXT : %c", (text_string_ptr->alphabet)[i]);
  }

  if(new_text_length != text_string_ptr->length)
  {
     text_string_ptr->length = new_text_length;
  }
  return new_count;
} /*gstk_decode_text_string_body_ext*/


/*===========================================================================
FUNCTION gstk_gsdi_asynch_function_cb

DESCRIPTION
    This callback function is being called when file access to gsdi returns
    to gstk task

PARAMETERS
  status : [Input] Indication of return status of MMGSDI
  cnf    : [Input] Indication of what type of command confirmation
  cnf_ptr: [Input] Pointer to the gsdi confirmation message

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  This function will set the signal in returning from gsdi task processing

SEE ALSO
  None
===========================================================================*/
void gstk_gsdi_asynch_function_cb(mmgsdi_return_enum_type status,
                                  mmgsdi_cnf_enum_type    cnf,
                                  const mmgsdi_cnf_type*  cnf_ptr)
{
  if(cnf_ptr == NULL)
  {
    UIM_MSG_ERR_0("cnf_ptr ERR: NULL");
    return;
  }

  gstk_gsdi_status = status;
  if(gstk_gsdi_status == MMGSDI_SUCCESS) {
    if(cnf == MMGSDI_READ_CNF) {
      returned_data_len = cnf_ptr->read_cnf.read_data.data_len;

      if (raw_img_data_p != NULL) {
        gstk_free(raw_img_data_p);
        raw_img_data_p = NULL;
      }
      /* get the memory location for the data */
      raw_img_data_p = GSTK_CALLOC(returned_data_len);
      if(raw_img_data_p == NULL) {
        returned_data_len = 0;
        gstk_gsdi_status = MMGSDI_ERROR;
      }
      else {
        memset(raw_img_data_p, 0x00, returned_data_len);
        (void)memscpy(raw_img_data_p,
               returned_data_len,
               cnf_ptr->read_cnf.read_data.data_ptr,
               cnf_ptr->read_cnf.read_data.data_len);
      }
    } /* MMGSDI_READ_CNF */
  }
  else {
    returned_data_len = 0;
  }
  (void)rex_set_sigs(UIM_GSTK_TCB, GSTK_GSDI_CMD_RSP_SIG);
} /* gstk_gsdi_asynch_function_cb */


/*===========================================================================
FUNCTION gstk_parser_text_string_tlv

DESCRIPTION
    This function parses the text string/default text/ussd string TLV and
    populate the gstk_string_type passed in the function call with the info
    in the TLV.
    It sets the DCS, length of text data and data.  Based on the DCS values,
    the data has been decode accordingly.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more_tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  tag: [Input] Indicate whether it is a Text String or Default Text or
               USSD string text TLV parsing
  text_string: [Input/Output] Pointer to gstk_string_type structure for populating
                              the parsed text string info
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_text_string_tlv(
                       uint8             *orig_cmd_data,
                       int32             cmd_data_len,
                       int32             orig_offset,
                       tags_enum_type    tag,
                       gstk_string_type  *text_string,
                       int32             *next_tlv_offset,
                       boolean           *more_tlvs,
                       boolean           man_opt_specifier,
                       const uint32      *ref_id)
{

    int32 count = 0;
    int32 count_after_decode = 0;
    int32 text_len_before_decode = 0;
    uint32 length_of_length_field = 0;
    boolean tag_comprehension_req = TRUE;
    gstk_status_enum_type         gstk_status = GSTK_SUCCESS;
    gstk_parsing_result_enum_type tag_parsing_result = GSTK_TLV_PARSING_CONTINUE;

    UIM_MSG_HIGH_0("** Parsing Text");

    if((!text_string) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }
    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* has to have at least tag and length field */

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len, 0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)tag, man_opt_specifier,
                                           orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(tag, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid Text String Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* check for length value */
    if(orig_cmd_data[count+length_of_length_field-1] == 0) {
        /* empty tlv */
        text_string->dcs = 0;
        text_string->length = 0;
        count = count + length_of_length_field;
    }
    else { /* at least 1 length */
        if(orig_cmd_data[count+length_of_length_field-1] == 1) {
          /* only have dcs field */
          text_string->length = 0;
        }
        else { /* has text */
          /* length of string */
          /* length in the Text String tlv includes length of text + DCS, thus,
          ** needs to -1 from the length obtained to get the actual length of the
          ** text string */
          text_string->length = orig_cmd_data[count+length_of_length_field-1] - 1;
        }

        count = count + length_of_length_field;

        /* DCS */
        text_string->dcs = orig_cmd_data[count++];
    }

    /* Text */
    if (text_string->length > 0) {

        text_len_before_decode = text_string->length;
        /* decode the text string according to the packing format
        ** add null char at the end */
        count_after_decode = gstk_decode_text_string_body(tag,
                                                          orig_cmd_data,
                                                          count, /* index location of the first byte of text */
                                                          text_string);

        /* calculate length based on raw TLV */
        count += text_len_before_decode;

        if (count != count_after_decode) {
            UIM_MSG_ERR_0("length doesn't match for raw TLV");
            if (text_string->text != NULL) {
              gstk_free(text_string->text);
              text_string->text = NULL;
              text_string->length = 0;
            }
          if (tag_comprehension_req) {
            return GSTK_INVALID_LENGTH;
          }
          else {
            if (ref_id != NULL) {
              command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
            }
          }
        }
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
      if (text_string->text != NULL) {
        gstk_free(text_string->text);
        text_string->text = NULL;
      }
      text_string->length = 0;
    }

    return gstk_status;
} /*gstk_parser_text_string_tlv*/

/*===========================================================================
FUNCTION gstk_parse_img_record

DESCRIPTION
    Generic Function used to parse the Image

PARAMETERS
   uint8                     rec_num -  Record number to access
   gstk_icon_type *          icon    -  Icon type

DEPENDENCIES

RETURN VALUE
  gstk_status_enum_type
    GSTK_FILE_ACCESS_FAIL - if the GSDI UIM_IMAGE File could not be read
    GSDI_SUCCESS          - Successful read of  GSDI UIM_IMAGE File

===========================================================================*/
gstk_status_enum_type gstk_parse_img_record(
    uint8                     rec_num,
    gstk_icon_type *          icon
)
{
    mmgsdi_return_enum_type         mmgsdi_status = MMGSDI_SUCCESS;
    mmgsdi_offset_type              img_instance_offset = 0;
    mmgsdi_len_type                 img_instance_size = 0;
    mmgsdi_access_type              file_access;
    uint8                           i = 0;
    mmgsdi_session_id_type          mmgsdi_session_id = MMGSDI_INVALID_SESSION_ID;
    rex_sigs_type                   gstk_sigs;
    mmgsdi_session_type_enum_type   session        = MMGSDI_MAX_SESSION_TYPE_ENUM;

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
    if(icon == NULL)
    {
      UIM_MSG_ERR_0("icon ERR: NULL");
      return GSTK_BAD_PARAM;
    }

    /* clear the sig first */
    (void) rex_clr_sigs( UIM_GSTK_TCB, GSTK_GSDI_CMD_RSP_SIG );

    switch(gstk_curr_inst_ptr->slot_id)
    {
      case GSTK_SLOT_1:
        session = MMGSDI_CARD_SESSION_SLOT_1;
        break;
      case GSTK_SLOT_2:
        session = MMGSDI_CARD_SESSION_SLOT_2;
        break;
      case GSTK_SLOT_3:
        session = MMGSDI_CARD_SESSION_SLOT_3;
        break;
      default:
        icon->present = FALSE;
        return GSTK_BAD_PARAM;
    }

    (void)gstk_util_validate_slot_sessions();

    /* Get the session id */
    for (i=0; i<GSTK_MMGSDI_SESS_INFO_SIZE;i++)
    {
      if ( gstk_shared_data.mmgsdi_sess_info[i].sess_type == session &&
           gstk_shared_data.mmgsdi_sess_info[i].slot ==
                gstk_curr_inst_ptr->slot_id )
      {
        mmgsdi_session_id = gstk_shared_data.mmgsdi_sess_info[i].sess_id;
        break;
      }
    }

    if(i == GSTK_MMGSDI_SESS_INFO_SIZE)
    {
      icon->present = FALSE;
      return GSTK_BAD_PARAM;
    }

    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = MMGSDI_IMAGE_FILE;

    mmgsdi_status = mmgsdi_session_read_record (
                      mmgsdi_session_id,
                      file_access,
                      (mmgsdi_rec_num_type)rec_num,
                      MMGSDI_MAX_DATA_BLOCK_LEN,
                      (mmgsdi_callback_type)gstk_gsdi_asynch_function_cb,
                      0);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
      gstk_sigs = gstk_wait(GSTK_GSDI_CMD_RSP_SIG | GSTK_TASK_STOP_SIG);
      if(gstk_sigs == GSTK_TASK_STOP_SIG)
      {
        /* Received TASK STOP signal, stop the GSTK processing */
        return GSTK_ERROR;
      }
      mmgsdi_status = gstk_gsdi_status;
    }

    if (mmgsdi_status != MMGSDI_SUCCESS) {
        icon->present = FALSE;
        /* free the raw_img_data_p from the previous image EF read allocation */
        if(raw_img_data_p != NULL) {
          gstk_free(raw_img_data_p);
          raw_img_data_p = NULL;
        }
        UIM_MSG_ERR_1("Cannot read GSDI UIM_IMAGE File 0x%x", mmgsdi_status);
        return GSTK_FILE_ACCESS_FAIL;
    }

    /* get the first image instance info */
    icon->width = raw_img_data_p[GSTK_IMG_EF_WIDTH_INDEX];
    icon->height = raw_img_data_p[GSTK_IMG_EF_HEIGHT_INDEX];
    switch (raw_img_data_p[GSTK_IMG_EF_CODING_SCHEME_INDEX]) {
      case 0x11:
        icon->image_coding_scheme = GSTK_BASIC_IMAGE_CODE_SCHEME;
        break;
      case 0x21:
        icon->image_coding_scheme = GSTK_COLOUR_IMAGE_CODE_SCHEME;
        break;
      default:
        UIM_MSG_ERR_1("Unknown Image Coding Scheme 0x%x",
                      raw_img_data_p[GSTK_IMG_EF_CODING_SCHEME_INDEX]);
        icon->image_coding_scheme = GSTK_UNKNOWN_IMAGE_CODE_SCHEME;
        break;
    }

    /* clear the sig first */
    (void) rex_clr_sigs( UIM_GSTK_TCB, GSTK_GSDI_CMD_RSP_SIG );

    img_instance_offset  = raw_img_data_p[GSTK_IMG_EF_OFFSET_HIGH_BYTE_INDEX] << 0x08;
    img_instance_offset |= raw_img_data_p[GSTK_IMG_EF_OFFSET_LOW_BYTE_INDEX];
    img_instance_size  = raw_img_data_p[GSTK_IMG_EF_SIZE_HIGH_BYTE_INDEX] << 0x08;
    img_instance_size |= raw_img_data_p[GSTK_IMG_EF_SIZE_LOW_BYTE_INDEX];
    icon->size = img_instance_size;
    file_access.access_method = MMGSDI_BY_PATH_ACCESS;
    file_access.file.path_type.path_len = 4;
    file_access.file.path_type.path_buf[0] = 0x3F00;
    file_access.file.path_type.path_buf[1] = UIM_TELECOM_DF;
    file_access.file.path_type.path_buf[2] = UIM_GRAPHICS_DF;
    file_access.file.path_type.path_buf[3] = raw_img_data_p[GSTK_IMG_EF_FID_HIGH_BYTE_INDEX] << 0x08
                          | raw_img_data_p[GSTK_IMG_EF_FID_LOW_BYTE_INDEX];
    /* free the raw_img_data_p from the previous image EF read */
    gstk_free(raw_img_data_p);
    raw_img_data_p = NULL;

    /* Transparent file */
    mmgsdi_status = mmgsdi_session_read_transparent(
                      mmgsdi_session_id,
                      file_access,
                      img_instance_offset,
                      img_instance_size,
                      (mmgsdi_callback_type) gstk_gsdi_asynch_function_cb,
                      0);
    if(mmgsdi_status == MMGSDI_SUCCESS)
    {
        gstk_sigs = gstk_wait(GSTK_GSDI_CMD_RSP_SIG | GSTK_TASK_STOP_SIG);
        if(gstk_sigs == GSTK_TASK_STOP_SIG)
        {
          /* Received TASK STOP signal, stop the GSTK processing */
          return GSTK_ERROR;
        }
        mmgsdi_status = gstk_gsdi_status;
    }


    if (mmgsdi_status != MMGSDI_SUCCESS) {
        icon->present = FALSE;
        /* free the static raw_img_data_p variable */
        if(raw_img_data_p != NULL) {
          gstk_free(raw_img_data_p);
          raw_img_data_p = NULL;
        }
        UIM_MSG_ERR_1("Cannot get icon image 0x%x", mmgsdi_status);
        return GSTK_FILE_ACCESS_FAIL;
    }

    /* Successfully Read The IMG File and */
    /* Successfully Read the IMG Instace File */
    /* OK to continue populating the icon structure passed in */
    icon->size = returned_data_len;
    icon->present = TRUE;

    /* Need to allocate Space to Store temporary Icon */
    icon->data = (uint8 *)GSTK_CALLOC(
               (sizeof(uint8) * icon->size ) );

    if ( (icon->data != NULL) && (raw_img_data_p != NULL) )
    {
        /*Initialize the Data */
        memset( icon->data,
                0xFF,
                icon->size);

        (void)memscpy(icon->data,
                icon->size,
                raw_img_data_p,
                icon->size);

        /* free the static raw_img_data_p variable */
        if(raw_img_data_p != NULL) {
          gstk_free(raw_img_data_p);
          raw_img_data_p = NULL;
        }
    }
    else
    {
        /* free the static raw_img_data_p variable */
        if(raw_img_data_p != NULL) {
          gstk_free(raw_img_data_p);
          raw_img_data_p = NULL;
        }
        /* free the static icon->data variable */
        if(icon->data != NULL)
        {
          gstk_free(icon->data);
          icon->data = NULL;
        }
        return GSTK_MEMORY_ERROR;
    }

    return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_parser_icon_id_tlv

DESCRIPTION
    This function parses the icon ID TLV, and populate the gstk_icon_type
    passed in the function call with the info in the TLV.
    It sets the in_use boolean variable in the gstk_icon_type if there is
    valid icon info.
    Based on the record number, it performs a gsdi read on the EF-IMG
    file to get the width, height info of the image.  In additional, it
    goes to the specific image instance file as specified in the
    ER-IMG to retrieve the actual image and size for the image.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE


PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  icon: [Input/Output] Pointer to gstk_icon_type structure for populating the
                       parsed icon info
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_icon_id_tlv(
        const gstk_string_type   * string_tlv,
        uint8                    * orig_cmd_data,
        int32                      cmd_data_len,
        int32                      orig_offset,
        gstk_icon_type           * icon,
        int32                    * next_tlv_offset,
        boolean                  * more_tlvs,
        boolean                    man_opt_specifier,
        const uint32             * ref_id)
{
    int32                           count                     = 0;
    uint32                          length_of_length_field    = 0;
    uint8                           rec_num;
    gstk_status_enum_type           gstk_status               = GSTK_SUCCESS;
    boolean                         tag_comprehension_req     = TRUE;
    gstk_parsing_result_enum_type   tag_parsing_result        = GSTK_TLV_PARSING_CONTINUE;
    int32                           tlv_length                = 0;
    uint32                          index                     = 0;

    UIM_MSG_HIGH_0("** Parsing Icon");
    if((!string_tlv) || (!next_tlv_offset) || (!more_tlvs) || (!icon))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len, 0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }
    if (ref_id != NULL) {
      index = GSTK_CMD_REF_TABLE_INDEX(*ref_id);
    }

    count = orig_offset;

    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_ICON_IDENTIFIER_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ICON_IDENTIFIER_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid Icon Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if ( tlv_length < GSTK_ICON_ID_LEN ) {
      UIM_MSG_ERR_1("Invalid icon id len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
         command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count += length_of_length_field;

      icon->present = TRUE;

      /* icon qualifier */
      icon->qualifier = orig_cmd_data[count++];

      /* get heigth, width, size, data and image from the EF-IMG file */
      /* record number in the EF-IMG */
      rec_num = orig_cmd_data[count++];
      if (rec_num == 0) {
        icon->present = FALSE;
        icon->size = 0;
        if (tag_comprehension_req){
          UIM_MSG_ERR_0("Invalid rec num = 0");
          return GSTK_INVALID_COMMAND;
        }
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
        }
      }
      else {
        if (string_tlv->length <= 1) {
          if (tag_comprehension_req) {
            UIM_MSG_ERR_0("Text string is null");
            return GSTK_INVALID_COMMAND;
          }
          icon->present = FALSE;
          icon->size = 0;
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
             command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
          }
        }
        icon->rec_num = rec_num;
        /* Now Populate the Icon Information */
        gstk_status = gstk_parse_img_record(rec_num,icon);
        if (gstk_status != GSTK_SUCCESS) {
          if (!tag_comprehension_req) {
            /* update the cmd_table info to indicate there was a parsing issue */
            if (ref_id != NULL) {
               command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
            }
            gstk_status = GSTK_SUCCESS;
            icon->present = FALSE;
            icon->size = 0;
          }
          else {
            UIM_MSG_ERR_0("Unable to read Img Record");
            return GSTK_INVALID_COMMAND;
          }
        }
      }
    }

    /* next TLV offset */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    if (gstk_status == GSTK_SUCCESS) {
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
    }
    else {
      (void)gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
    }

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
      if (icon->data != NULL) {
        gstk_free(icon->data);
        icon->data = NULL;
      }
      icon->rec_num = 0;
      icon->present = FALSE;
      icon->size = 0;
    }
    return gstk_status;

} /*gstk_parser_icon_id_tlv*/



/*===========================================================================
FUNCTION gstk_parser_response_len_tlv

DESCRIPTION
    This function parses the response length TLV and populate the max and min
    len passed in the function call with the info in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  max_len: [Input/Output] Pointer to the Max len
  min_len: [Input/Output] Pointer to the Min len
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_response_len_tlv(
                       uint8             *orig_cmd_data,
                       int32             cmd_data_len,
                       int32             orig_offset,
                       uint8             *max_len,
                       uint8             *min_len,
                       int32             *next_tlv_offset,
                       boolean           *more_tlvs,
                       boolean            man_opt_specifier,
                       const uint32      *ref_id)
{

    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           tlv_length             = 0;
    boolean                         tag_comprehension_req  = TRUE;

    UIM_MSG_HIGH_0("** Parsing Rsp Len");

    if((!max_len) || (!min_len) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_RESPONSE_LEN_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* Response Length TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_RESPONSE_LENGTH_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_RESPONSE_LENGTH_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid response len Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of response len tlv should be 2 */
    tlv_length = orig_cmd_data[count+length_of_length_field-1];
    if ( tlv_length < GSTK_RESPONSE_LEN_LEN ) {
      UIM_MSG_ERR_1("Invalid response len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count = count + length_of_length_field;
      *min_len = orig_cmd_data[count++];
      *max_len = orig_cmd_data[count++];

      UIM_MSG_HIGH_2("====== Min Len = 0x%x ====== Max Len = 0x%x",
                     *min_len, *max_len);
    }

    /* Check if TLV total length indicated by the Card is the same as the incremented value */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
} /*gstk_parser_response_len_tlv*/

/*===========================================================================
FUNCTION gstk_parser_tone_tlv

DESCRIPTION
    This function parses the tone TLV and populate the gstk_tone_enum_type
    passed in the function call with the info in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  tone: [Input/Output] Pointer to the tone
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_tone_tlv(
                       uint8                 *orig_cmd_data,
                       int32                 cmd_data_len,
                       int32                 orig_offset,
                       gstk_tone_enum_type   *tone,
                       int32                 *next_tlv_offset,
                       boolean               *more_tlvs,
                       boolean                man_opt_specifier,
                       const uint32          *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           tlv_length             = 0;
    boolean                         tag_comprehension_req  = TRUE;

    UIM_MSG_HIGH_0("** Parsing Tone");

    if((!tone) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }
    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_TONE_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* tone TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_TONE_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_TONE_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid tone Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of tone tlv should be 1 */
    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    if ( tlv_length < GSTK_TONE_LEN ) {
      UIM_MSG_ERR_1("Invalid tone len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {

      count = count + length_of_length_field;
      *tone = (gstk_tone_enum_type)orig_cmd_data[count++];
       UIM_MSG_HIGH_1("Tone = 0x%x", *tone);
    }

    /* Check if TLV total length indicated by the Card is the same as the incremented value */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_tone_tlv */



/*===========================================================================
FUNCTION gstk_parser_duration_tlv

DESCRIPTION
    This function parses the duration TLV and populate the gstk_duration_type
    passed in the function call with the info in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  duration: [Input/Output] Pointer to the duration
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_duration_tlv(
                       uint8                 *orig_cmd_data,
                       int32                 cmd_data_len,
                       int32                 orig_offset,
                       gstk_duration_type    *duration,
                       int32                 *next_tlv_offset,
                       boolean               *more_tlvs,
                       boolean                man_opt_specifier,
                       const uint32          *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing Duration");

    if((!duration) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_DURATION_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* duration TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_DURATION_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_DURATION_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid duration Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of duration tlv should be 2 */
    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    if ( tlv_length < GSTK_DURATION_LEN ) {
      UIM_MSG_ERR_1("Invalid duration len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count = count + length_of_length_field;

      duration->units = orig_cmd_data[count++];
      switch (duration->units) {
      case GSTK_DURATION_MINUTES:
      case GSTK_DURATION_SECONDS:
      case GSTK_DURATION_TENTHS_OF_SECONDS:
        duration->length = orig_cmd_data[count++];
        duration->present = TRUE;
        UIM_MSG_HIGH_1("  %d units", duration->length);
        break;
      default:
        duration->present = FALSE;
        UIM_MSG_ERR_0("Unknown Duration unit");
      }
      UIM_MSG_HIGH_1("Duration units = 0x%x ", duration->units);
    }

    /* Check if TLV total length indicated by the Card is the same as the incremented value */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_duration_tlv */




/*===========================================================================
FUNCTION gstk_parser_evt_list_tlv

DESCRIPTION
    This function parses the event list TLV and populate the
    gstk_shared_data.gstk_evt_list_code_enum_type passed in the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  evt_list_mask: [Input/Output] Pointer to the event list, the pointer should
                                point to at least MAX_EVENT_LIST_LEN
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_evt_list_tlv(
                       uint8                           *orig_cmd_data,
                       int32                           cmd_data_len,
                       int32                           orig_offset,
                       uint8                           *evt_list_mask,
                       int32                           *next_tlv_offset,
                       boolean                         *more_tlvs,
                       boolean                          man_opt_specifier,
                       const uint32                    *ref_id)
{
    int                             i                      = 0;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           tlv_length             = 0;
    uint32                          index                  = 0;
    gstk_nv_items_data_type         nv_data;

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

    if((!evt_list_mask) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* has to have at least the tag and the length field */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    /* Read the CAT version */
    (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                              gstk_curr_inst_ptr->slot_id,
                              &nv_data);

    count = orig_offset;
    if (ref_id != NULL) {
      index = GSTK_CMD_REF_TABLE_INDEX(*ref_id);
    }
    /* event list TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_EVENT_LIST_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_EVENT_LIST_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid event list Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of event list tlv data */
    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    count = count + length_of_length_field;

    for (i = 0; i< tlv_length; i++) {
      switch(orig_cmd_data[count]) {
        case GSTK_MT_CALL:
          evt_list_mask[GSTK_MT_CALL] = 0x01;
          UIM_MSG_HIGH_0("MT Call");
          break;
        case GSTK_CALL_CONNECTED:
          evt_list_mask[GSTK_CALL_CONNECTED] = 0x01;
          UIM_MSG_HIGH_0("Call connected");
          break;
        case GSTK_CALL_DISCONNECTED:
          evt_list_mask[GSTK_CALL_DISCONNECTED] = 0x01;
          UIM_MSG_HIGH_0("Call disconnected");
          break;
        case GSTK_LOCATION_STATUS:
          evt_list_mask[GSTK_LOCATION_STATUS] = 0x01;
          UIM_MSG_HIGH_0("Location Status");
          break;
        case GSTK_USER_ACTIVITY:
          evt_list_mask[GSTK_USER_ACTIVITY] = 0x01;
          UIM_MSG_HIGH_0("User Activity");
          break;
        case GSTK_IDLE_SCRN_AVAILABLE:
          evt_list_mask[GSTK_IDLE_SCRN_AVAILABLE] = 0x01;
          UIM_MSG_HIGH_0("Idle scrn avail");
          break;
        case GSTK_CARD_READER_STATUS:
          evt_list_mask[GSTK_CARD_READER_STATUS] = 0x01;
          UIM_MSG_HIGH_0("Card reader status");
          break;
        case GSTK_LANGUAGE_SELECTION:
          evt_list_mask[GSTK_LANGUAGE_SELECTION] = 0x01;
          UIM_MSG_HIGH_0("Language Sel");
          break;
        case GSTK_BROWSER_TERMINATION:
          evt_list_mask[GSTK_BROWSER_TERMINATION] = 0x01;
          UIM_MSG_HIGH_0("Browser Term");
          break;
        case GSTK_DATA_AVAILABLE:
          evt_list_mask[GSTK_DATA_AVAILABLE] = 0x01;
          UIM_MSG_HIGH_0("Data Avail");
          break;
        case GSTK_CHANNEL_STATUS:
          evt_list_mask[GSTK_CHANNEL_STATUS] = 0x01;
          UIM_MSG_HIGH_0("Channel status");
          break;
        case GSTK_NW_REJECTION:
          evt_list_mask[GSTK_NW_REJECTION] = 0x01;
          UIM_MSG_HIGH_0("Network Rejection");
          break;
        case GSTK_HCI_CONNECTIVITY:
          evt_list_mask[GSTK_HCI_CONNECTIVITY] = 0x01;
          UIM_MSG_HIGH_0("HCI Connectivity");
          break;
        case GSTK_ACCESS_TECH:
          if(nv_data.cat_version >= GSTK_CFG_CAT_VER4) {
            evt_list_mask[GSTK_ACCESS_TECH] = 0x01;
            UIM_MSG_HIGH_0("Access Technology");
          }
          else
          {
            if (tag_comprehension_req) {
              UIM_MSG_ERR_1("unknown event list, 0x%x", orig_cmd_data[count]);
              return GSTK_BAD_PARAM;
            }
            else {
              /* update the cmd_table info to indicate there was a parsing issue */
              if (ref_id != NULL) {
                command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
              }
              UIM_MSG_HIGH_1("Harmless unknown event 0x%x",
                             orig_cmd_data[count]);
            }
          }
          break;
        case GSTK_NW_SEARCH_MODE:
          if(nv_data.cat_version >= GSTK_CFG_CAT_VER6) {
            evt_list_mask[GSTK_NW_SEARCH_MODE] = 0x01;
            UIM_MSG_HIGH_0("Network Search mode");
          }
          else
          {
            if (tag_comprehension_req) {
              UIM_MSG_ERR_1("unknown event list, 0x%x", orig_cmd_data[count]);
              return GSTK_BAD_PARAM;
            }
            else {
              /* update the cmd_table info to indicate there was a parsing issue */
              if (ref_id != NULL) {
                command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
              }
              UIM_MSG_HIGH_1("Harmless unknown event 0x%x",
                             orig_cmd_data[count]);
            }
          }
          break;
        case GSTK_CSG_CELL_SELECTION:
          if(nv_data.cat_version >= GSTK_CFG_CAT_VER9) {
            evt_list_mask[GSTK_CSG_CELL_SELECTION] = 0x01;
            UIM_MSG_HIGH_0("CSG cell selection");
          }
          else
          {
            if (tag_comprehension_req) {
              UIM_MSG_ERR_1("unknown event list, 0x%x", orig_cmd_data[count]);
              return GSTK_BAD_PARAM;
            }
            else {
              /* update the cmd_table info to indicate there was a parsing issue */
              if (ref_id != NULL) {
                command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
              }
              UIM_MSG_HIGH_1("Harmless unknown event 0x%x",
                             orig_cmd_data[count]);
            }
          }
          break;
        case GSTK_IMS_REGISTRATION:
          evt_list_mask[GSTK_IMS_REGISTRATION] = 0x01;
          UIM_MSG_HIGH_0("IMS Registration");
          break;
        default:
          if (tag_comprehension_req) {
            UIM_MSG_ERR_1("unknown event list, 0x%x", orig_cmd_data[count]);
            return GSTK_BAD_PARAM;
          }
          else {
            /* update the cmd_table info to indicate there was a parsing issue */
            if (ref_id != NULL) {
              command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
            }
            UIM_MSG_HIGH_1("Harmless unknown event 0x%x",
                           orig_cmd_data[count]);
          }
        } /*end switch */
        count++;
      } /* end for loop */

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_evt_list_tlv */

/*===========================================================================
FUNCTION gstk_parser_address_ss_string_tlv

DESCRIPTION
    This function parses the address or ss string TLV and populate the
    gstk_address_type passed in the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  tag: [Input] ADDRESS_TAG or SS_STRING_TAG
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  address: [Input/Output] Pointer to the gstk address type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_address_ss_string_tlv(
                       tags_enum_type                  tag,
                       uint8                           *orig_cmd_data,
                       int32                           cmd_data_len,
                       int32                           orig_offset,
                       gstk_address_type               *address,
                       int32                           *next_tlv_offset,
                       boolean                         *more_tlvs,
                       boolean                          man_opt_specifier,
                       const uint32                    *ref_id)
{
    uint8                           bcd_address[GSTK_MAX_BCD_ADDRESS_LEN];
    int32                           bcd_addr_length        = 0;
    int                             i                      = 0;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing Addr/SS");
    if((!address) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* have to have at least tag and length field */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* address/ss_string TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)tag,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(tag, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid address/ss_string Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    count = count + length_of_length_field;

    /* check for length field */
    if(tlv_length == 0) {
      /* nothing! */
      bcd_addr_length = 0;
      address->TON = GSTK_RFU_TON;
      address->NPI = GSTK_RFU_NPI;
      if((man_opt_specifier == GSTK_MANDATORY) && (tag_comprehension_req == TRUE))
      {
        UIM_MSG_ERR_0("Mandatory Address not provided for this command");
        return GSTK_ERROR;
      }
    }
    else { /* at least has the TON */
      if(tlv_length == 1) {
        /* only TON */
        bcd_addr_length = 0;
      }
      else {
        /* length of address/ss_string tlv data */
        /* -1 for the TON */
        bcd_addr_length = tlv_length - 1;
      }

      /* copy the TON and NPI */
      if(gstk_parse_ton_npi(orig_cmd_data[count], &(address->TON), &(address->NPI))
          != GSTK_SUCCESS) {
        if (tag_comprehension_req) {
          UIM_MSG_ERR_0("Unable to Parse TON NPI Correctly");
          return GSTK_ERROR;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
             command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
          }
          address->TON = GSTK_RFU_TON;
          address->NPI = GSTK_RFU_NPI;
        }
      }

      count++;

      /* copy the address */
      if ((bcd_addr_length > GSTK_MAX_BCD_ADDRESS_LEN) || (bcd_addr_length < 1)) {
        if (tag_comprehension_req) {
          /* GSTK can't handle this big of an address */
          UIM_MSG_ERR_1("Address length out of bounds", bcd_addr_length);
          return GSTK_PARAM_EXCEED_BOUNDARY;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
          }
          address->length = 0;
        }
      }
      else {
        (void)memscpy(bcd_address, sizeof(bcd_address),
               &(orig_cmd_data[count]), int32touint32(bcd_addr_length));

        address->length = 0;
        /* Translate BCD address */
        for (i = 0; i < bcd_addr_length; i++) 
        {
          if ((address->length + 1) >= GSTK_MAX_ADDRESS_LEN)
          {
            UIM_MSG_ERR_1("address is too big! (0x%x)", address->length);
            return GSTK_PARAM_EXCEED_BOUNDARY;
          }
          /* No spec clarification on what to do if "F" is present in middle of the address string,
             So ignoring if any 'F' is present in string and continue parsing other characters */
          if ((bcd_address[i] & 0x0F) != 0x0F)
          {
            address->address[address->length] = (bcd_address[i] & 0x0F);
            address->length++;
          }
          if (((bcd_address[i] & 0xF0) >> 0x04) != 0x0F)
          {
            address->address[address->length] = (bcd_address[i] & 0xF0) >> 0x04;
            address->length++;
          }
        }

        gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], bcd_addr_length);

        gstk_util_dump_byte_array("Decoded address",
                                  address->address,
                                  GSTK_MIN(address->length, GSTK_MAX_ADDRESS_LEN));
      }

      /* position count to the end of this tlv */
      count += bcd_addr_length;

    } /* there is no TON or address */

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_address_ss_string_tlv */



/*===========================================================================
FUNCTION gstk_parser_subaddress_tlv

DESCRIPTION
    This function parses the subaddress TLV and populate the
    gstk_subaddress_type passed in the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  subaddress: [Input/Output] Pointer to the gstk subaddress type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_subaddress_tlv(
                       uint8                           *orig_cmd_data,
                       int32                           cmd_data_len,
                       int32                           orig_offset,
                       gstk_subaddress_type            *subaddress,
                       int32                           *next_tlv_offset,
                       boolean                         *more_tlvs,
                       boolean                          man_opt_specifier,
                       const uint32                    *ref_id)
{
    boolean                         tag_comprehension_req  = TRUE;
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("** Parsing Subaddr");
    if((!subaddress) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* have at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* subaddress TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_CALLED_PARTY_SUB_ADDRESS_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_CALLED_PARTY_SUB_ADDRESS_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid subaddress Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of subaddress tlv data */
    subaddress->length = orig_cmd_data[count+length_of_length_field-1];

    count = count + length_of_length_field;

    /* copy the subaddress */
    if (subaddress->length > GSTK_MAX_SUBADDR_LEN) {
      if (tag_comprehension_req) {
        /* GSTK can't handle this big of a subaddress */
        UIM_MSG_ERR_0("Subaddr too LONG");
        return GSTK_PARAM_EXCEED_BOUNDARY;
      }
      else {
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
        count += subaddress->length;
        subaddress->length = 0;
      }
    }
    else {
      (void)memscpy(subaddress->subaddress, sizeof(subaddress->subaddress),
      	       &orig_cmd_data[count], subaddress->length);

      gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], subaddress->length);

      gstk_util_dump_byte_array("Decoded subaddress", subaddress->subaddress, subaddress->length);

      /* position count to the end of this tlv */
      count += subaddress->length;
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_subaddress_tlv */


/*===========================================================================
FUNCTION gstk_parser_cap_config_tlv

DESCRIPTION
    This function parses the ccp TLV and populate the
    gstk_cap_config_type passed in the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  ccp: [Input/Output] Pointer to the gstk ccp type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_cap_config_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_cap_config_type           *ccp,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    boolean                         tag_comprehension_req  = TRUE;
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("** Paring C C P");
    if((!ccp) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* have at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* cap config TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid cap config Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of ccp tlv data */
    ccp->length = orig_cmd_data[count+length_of_length_field-1];

    count = count + length_of_length_field;

    /* copy the ccp */
    if (ccp->length > GSTK_MAX_CCP_LEN) {
      if (tag_comprehension_req) {
        /* GSTK can't handle this big of an ccp */
        UIM_MSG_ERR_0("Exceed CCP");
        return GSTK_PARAM_EXCEED_BOUNDARY;
      }
      else {
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
        count += ccp->length;
        ccp->length = 0;
      }
    }
    else {
      (void)memscpy(ccp->ccp, sizeof(ccp->ccp), &orig_cmd_data[count], ccp->length);

      gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], ccp->length);

      gstk_util_dump_byte_array("Decoded ccp", ccp->ccp, ccp->length);

      /* position count to the end of this tlv */
      count += ccp->length;
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_cap_config_tlv */



/*===========================================================================
FUNCTION gstk_parser_browser_id_tlv

DESCRIPTION
    This function parses the browser identity TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  browser_id: [Input/Output] Pointer to the browser identity,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_browser_id_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       uint8                          *browser_id,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    boolean                         tag_comprehension_req  = TRUE;
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing Browser ID");
    if((!browser_id) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }
    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_BROWSER_ID_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* browser id TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_BROWSER_IDENTITY_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_BROWSER_IDENTITY_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid browser id Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    if (tlv_length < GSTK_BROWSER_ID_LEN) {
      UIM_MSG_ERR_1("Invalid browser id len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      /* position count to the browser id location in the tlv */
      count = count + length_of_length_field;

      *browser_id = orig_cmd_data[count++];
      UIM_MSG_HIGH_1("  Browser ID: 0x%x", *browser_id);

      /* standard supports 0x00 as default browser value only */
      if(*browser_id != 0x00) {
        if (tag_comprehension_req) {
          /* if the Card passes any other values, return error */
          UIM_MSG_ERR_1("Unknown browser ID 0x%x", *browser_id);
          return GSTK_INVALID_COMMAND;
        }
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
      }
    }

    /* Check if TLV total length indicated by the Card is the same as the incremented value */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;

} /* gstk_parser_browser_id_tlv */

/*===========================================================================
FUNCTION gstk_parser_url_tlv

DESCRIPTION
    This function parses the url TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  url: [Input/Output] Pointer to the url,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_url_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_url_type                  *url,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("** Parsing URL");

    if((!url) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }
    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* have at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* url TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_URL_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_URL_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    /* valid url Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    url->length = orig_cmd_data[count + length_of_length_field -1];

    /* position count to the url location in the tlv */
    count = count + length_of_length_field;

    if (url->length > 0) { /* Card provides a url address */
      url->url = GSTK_CALLOC(url->length);
      if (url->url != NULL) {
        memset(url->url, 0x00, url->length);
        (void)memscpy(url->url, url->length, &orig_cmd_data[count], url->length);

        gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], url->length);

        gstk_util_dump_byte_array("Decoded url", url->url, url->length);

        count += url->length;
      }
      else {
        return GSTK_MEMORY_ERROR;
      }
    }
    /* if url->length == 0, application should use the default url */

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
} /* gstk_parser_url_tlv */



/*===========================================================================
FUNCTION gstk_parser_bearer_tlv

DESCRIPTION
    This function parses the url TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  bearer: [Input/Output] Pointer to the bearer.
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_bearer_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_bearer_type               *bearer,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int                             i                      = 0;
    boolean                         tag_comprehension_req  = TRUE;

    UIM_MSG_HIGH_0("** Parsing Bearer");
    if((!bearer) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

     /* have at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* bearer TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_BEARER_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_BEARER_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid bearer Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    bearer->length = orig_cmd_data[count + length_of_length_field -1];

    /* position count to the bearer location in the tlv */
    count = count + length_of_length_field;

    if (bearer->length > 0) { /* Card provides a bearer */
      bearer->bearer_list = (gstk_bearer_code_enum_type*)GSTK_CALLOC(
                                (bearer->length)*sizeof(gstk_bearer_code_enum_type));
      if (bearer->bearer_list != NULL) {
        memset(bearer->bearer_list, 0x00, (bearer->length)*sizeof(gstk_bearer_code_enum_type));

        gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], bearer->length);
        UIM_MSG_HIGH_0("============ Decoded bearer");
        for (i = 0; i< bearer->length; i++) {
          switch(orig_cmd_data[count]){
          case GSTK_SMS:
            bearer->bearer_list[i] = GSTK_SMS;
            UIM_MSG_HIGH_2("bearer[%d]: 0x%x, SMS", i, bearer->bearer_list[i]);
            break;
          case GSTK_CSD:
            bearer->bearer_list[i] = GSTK_CSD;
            UIM_MSG_HIGH_2("bearer[%d]: 0x%x, CSD", i, bearer->bearer_list[i]);
            break;
          case GSTK_USSD_BEARER_CODE:
            bearer->bearer_list[i] = GSTK_USSD_BEARER_CODE;
            UIM_MSG_HIGH_2("bearer[%d]: 0x%x, USSD",
                           i, bearer->bearer_list[i]);
            break;
          case GSTK_GPRS:
            bearer->bearer_list[i] = GSTK_GPRS;
            UIM_MSG_HIGH_2("bearer[%d]: 0x%x, GPRS",
                           i, bearer->bearer_list[i]);
            break;
          case GSTK_RFU:
            bearer->bearer_list[i] = GSTK_RFU;
            UIM_MSG_HIGH_2("bearer[%d]: 0x%x, RFU", i, bearer->bearer_list[i]);
            break;
          default:
            if (tag_comprehension_req) {
              bearer->bearer_list[i] = GSTK_RFU;
              UIM_MSG_ERR_1("known bearer, 0x%x", orig_cmd_data[i]);
              break;
            }
            else {
              /* update the cmd_table info to indicate there was a parsing issue */
              if (ref_id != NULL) {
                command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
              }
              bearer->bearer_list[i] = GSTK_RFU;
              UIM_MSG_HIGH_2("bearer[%d]: 0x%x, RFU",
                             i, bearer->bearer_list[i]);
              break;
            }
          }
          count++;
        }
      }
      else {
        return GSTK_MEMORY_ERROR;
      }
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        if (bearer->bearer_list != NULL) {
          gstk_free(bearer->bearer_list);
          bearer->bearer_list = NULL;
        }
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;

} /* gstk_parser_bearer_tlv */


/*===========================================================================
FUNCTION gstk_parser_provisioning_ref_file_tlv

DESCRIPTION
    This function parses the provisioning reference file TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  provisioning_file: [Input/Output] Pointer to the provisioning ref file,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_provisioning_ref_file_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_file_list_type            *provisioning_file,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int                             i                      = 0;

    UIM_MSG_HIGH_0("** Parsing Provision File");

    if((!provisioning_file) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least have tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* provisioning file TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_PROVISIONING_REF_FILE_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_PROVISIONING_REF_FILE_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    /* valid provisioning file Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    provisioning_file->length = orig_cmd_data[count + length_of_length_field - 1];

    /* position count to the provisioning file location in the tlv */
    count = count + length_of_length_field;

    if (provisioning_file->length != 0) {
      provisioning_file->file_paths = GSTK_CALLOC(
                provisioning_file->length);
      if (provisioning_file->file_paths == NULL) {
        return GSTK_MEMORY_ERROR;
      }
      memset(provisioning_file->file_paths,0x00, provisioning_file->length);
      for (i = 0; i < provisioning_file->length; i++) {
        provisioning_file->file_paths[i] = orig_cmd_data[count];
        if((orig_cmd_data[count] == 0x3F) && (orig_cmd_data[count+1] == 0x00)) { /* new file */
          provisioning_file->num_of_files++;
        }
        count++;
      }
      UIM_MSG_HIGH_1("===== # Files", provisioning_file->num_of_files);

      gstk_util_dump_byte_array("provisioning_file",
                                provisioning_file->file_paths,
                                provisioning_file->length);
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        if (provisioning_file->file_paths != NULL) {
          gstk_free(provisioning_file->file_paths);
          provisioning_file->file_paths = NULL;
        }
        provisioning_file->num_of_files = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;

} /* gstk_parser_provisioning_ref_file_tlv */




/*===========================================================================
FUNCTION gstk_parser_sms_tpdu_tlv

DESCRIPTION
    This function parses the sms tpdu TLV and populate the gstk_sms_tpdu_type
    passed in the function call with the info in the TLV.
    It also returns the index slot for next TLV's tag if available.
    If there is no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  tpdu: [Input/Output] Pointer to the tpdu
  is_cdma_sms: [Input] Indicates whether the TPDU is CDMA or GSM/WCDMA
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_sms_tpdu_tlv(
                       uint8                 *orig_cmd_data,
                       int32                 cmd_data_len,
                       int32                 orig_offset,
                       gstk_sms_tpdu_type    *tpdu,
                       boolean               *is_cdma_sms,
                       int32                 *next_tlv_offset,
                       boolean               *more_tlvs,
                       boolean                man_opt_specifier,
                       const uint32          *ref_id_ptr)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         partial_comp_temp      = FALSE;

    UIM_MSG_HIGH_0("** Parsing TPDU");
    if((!tpdu) || (!next_tlv_offset) || (!more_tlvs) || (!is_cdma_sms))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* have at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* initialize is_cmd_sms to FALSE */
    *is_cdma_sms = FALSE;
    if(ref_id_ptr != NULL)
    {
      partial_comp_temp =
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id_ptr)].partial_comprehension;
    }
    /* tpdu TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id_ptr, orig_cmd_data[count], (uint8)GSTK_SMS_TPDU_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_SMS_TPDU_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      /* is it cdma SMS TPDU ? */
      count = orig_offset;
      tag_parsing_result = gstk_is_tag_valid(ref_id_ptr, orig_cmd_data[count], (uint8)GSTK_CDMA_SMS_TPDU_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);
      /* Resetting the partial_comprehension flag to False if
         (no previous tag has set partial_comprehension flag to true)
          and  (current tag is GSTK_CDMA_SMS_TPDU_TAG) .
          The partial_comprehension tag was set to TRUE in previous call to gstk_is_tag_valid
          with GSTK_SMS_TPDU_TAG as third parameter */
      if((partial_comp_temp == FALSE) &&
          ((int)gstk_valid_tag(orig_cmd_data[orig_offset], GSTK_CDMA_SMS_TPDU_TAG)) &&
          (ref_id_ptr != NULL))
      {
         command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id_ptr)].partial_comprehension = FALSE;
      }
      if (!gstk_continue_parsing(GSTK_CDMA_SMS_TPDU_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
      {
        if (gstk_status == GSTK_SUCCESS) {
          /* Was optional tag, go through the parser_get_next_tlv_offset */
          gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
        }
        return gstk_status;
      }
      *is_cdma_sms = TRUE;
    }

    /* valid sms tpdu Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tpdu->length = orig_cmd_data[count + length_of_length_field -1];

    /* position count to the index for tpdu */
    count = count + length_of_length_field;

    /* tpdu */
    /* get memory */
    tpdu->tpdu =
        (uint8*)GSTK_CALLOC(tpdu->length);
    if(tpdu->tpdu == NULL) {
      return GSTK_MEMORY_ERROR;
    }
    memset(tpdu->tpdu, 0x00, tpdu->length);
    /* copy the tpdu info */
    (void)memscpy(tpdu->tpdu, tpdu->length, &orig_cmd_data[count], tpdu->length);

    gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], tpdu->length);

    gstk_util_dump_byte_array("Decoded Data", tpdu->tpdu, tpdu->length);

    /* position count to the next tlv starting location */
    count = count + tpdu->length;

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        if (tpdu->tpdu != NULL) {
          gstk_free(tpdu->tpdu);
          tpdu->tpdu = NULL;
        }
        tpdu->length = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;

} /* gstk_parser_sms_tpdu_tlv */

/*===========================================================================
FUNCTION gstk_parser_menu_num_items

DESCRIPTION
    Generic Function used by GSTK to get the Characteristics for
    a Setup Menu Proactive Command

DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type gstk_parser_menu_num_items(
    const uint8   * cmd_data,
    const int32   * cmd_data_len,
    uint8         * num_menu_items
)
{
    uint8                      num_of_items = 0;
    uint8                      index        = 0;
    uint8                      shift_index  = 0;
    gstk_nv_items_data_type    nv_data      = {0};

    /* Check Pointer */
    if ( ( cmd_data == NULL )        ||
         ( cmd_data_len == NULL )    ||
         ( num_menu_items == NULL ) )
    {
        /* Have a NULL Pointer */
        UIM_MSG_ERR_0("Null Input Params");
        return GSTK_ERROR;
    }

    UIM_MSG_HIGH_1("** Parsing Menu # Item, 0x%x", cmd_data[index]);

    /* Byte 0 should be Alpha Identifier in setup menu but
       in select item this is optional */
    if ( gstk_valid_tag( cmd_data[index],GSTK_ALPHA_IDENTIFIER_TAG ) )
    {
        /* Determine Length of Byte 0 to Skip */
        shift_index = cmd_data[++index];

        if (shift_index ==  GSTK_2_BYTE_LENGTH_FIRST_VALUE) {
            shift_index = cmd_data[++index];
        }

        /* Skip Alpha Identifier Text */
        index += shift_index;

        /* Now Set the Index to Next Item Tag */
        index++;
    }

    (void) gstk_nv_access_read(GSTK_NV_EXT_ME_QMI_CAT_MODE,
                               GSTK_SLOT_ID_MAX,
                               &nv_data);

    /* Now Count the Number of Items */
    /* the Rest of the Command       */
    while (index < *cmd_data_len)
    {
        if (gstk_valid_tag( cmd_data[index], GSTK_ITEM_TAG))
        {
          if((cmd_data[index+1] != 0) || (nv_data.qmi_cat_mode == 0))
          {
            /* Increment Number of Items Found */
            num_of_items++;
          }

          /* Now Shift the Index Past Text */
          shift_index = cmd_data[++index];
          if (shift_index ==  GSTK_2_BYTE_LENGTH_FIRST_VALUE) {
              shift_index = cmd_data[++index];
          }
          index += shift_index;
        }
        else
        {
          UIM_MSG_ERR_2("BAD ITEM TAG:%x INDEX:%x", cmd_data[index], index);
          break;
        }
        /* Place index to point to next Tag */
        index++;
    }

    /* Set the Number of Items Found */
    *num_menu_items = num_of_items;
    UIM_MSG_HIGH_1("Num of items 0x%x", *num_menu_items);
    if (*num_menu_items > GSTK_MAX_MENU_ITEMS)
    {
        return GSTK_BAD_PARAM;
    }

    return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_parser_alpha_identifier_tlv

DESCRIPTION
    Generic Function used by GSTK to parse the Alpha Identifer TLV

DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type gstk_parser_alpha_identifier_tlv(
    uint8                       * orig_cmd_data,
    int32                       cmd_data_len,
    int32                       orig_offset,
    gstk_string_type            * alpha,
    int32                       * next_tlv_offset,
    boolean                     * more_tlvs,
    boolean                       man_opt_specifier,
    const uint32                * ref_id
)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int                             i                      = 0;

    UIM_MSG_HIGH_0("** Parsing Alpha");

    if((!alpha) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* alpha TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_ALPHA_IDENTIFIER_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ALPHA_IDENTIFIER_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    /* valid alpha Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* length of string */
    alpha->length = orig_cmd_data[count+length_of_length_field-1];

    /* Increment count to beginning of Text */
    count = count + length_of_length_field;

    /* Allocate Memory for Alpha ID Text  */
    /* Add 1 to allow for the Null          */
    alpha->text =
        (uint8*)GSTK_CALLOC(( (alpha->length + 1) * sizeof(uint8) ) );

    if(alpha->text == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

    /* Initialize Allocted Memory */
    memset(alpha->text,
          0x00,
          alpha->length + 1);

    /* Alpha Identifier is not really      */
    /* a gstk_string_type so we need       */
    /* to default to 8 bit SMS default DCS */
    alpha->dcs = 0x04;

    /* MEMCPY Alpha ID Text */
    (void)memscpy(alpha->text,
          alpha->length,
          &orig_cmd_data[count],
          alpha->length);

    /* update dcs for unicode if required */
    if (alpha->length > 0) {
      if ((alpha->text[0] == 0x80) ||
          (alpha->text[0] == 0x81) ||
          (alpha->text[0] == 0x82)) {
        /* unicode */
        alpha->dcs = 0x08;
      }
    }

    /* calculate length based on raw TLV */
    /* Set the Count to check for       */
    /* Additional TLVs                  */
    count += alpha->length;

    /* Add 1 to the length to account for */
    /* the NULL that terminates the string*/
    alpha->length += 1;

    for(i = 0; (alpha->text != NULL) && (i < alpha->length); i++)
    {
      UIM_MSG_HIGH_3("    alpha[%d], 0x%x, %c",
                     i, alpha->text[i], alpha->text[i]);
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
       UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                     cmd_data_len, count);
        if (alpha->text != NULL) {
          gstk_free(alpha->text);
          alpha->text = NULL;
        }
        alpha->length = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;
}




/*===========================================================================
FUNCTION gstk_parser_item_tlv

DESCRIPTION
    Generic Function used by Populate Items for Menu Items or Select Items

    ITEM TLV Appears as:
        ITEM TAG    - 1 uint8
        LENGTH      - 2 to (Y-1)+2 uint8(s)
        IDENTIFIER  - (Y-1)+3 uint8(s)
        TEXT        - (Y-1)+4 to (Y-1)+X+2

DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type  gstk_parser_item_tlv(
    uint8                         * orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    gstk_item_object_type         * item,
    int32                         * next_tlv_offset,
    boolean                       * more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                  * ref_id
)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("** Parsing item");

    if((!item) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* ITEM TAG TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_ITEM_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ITEM_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    /* Item TLV Is Valid Proceed with Parsing */
      /* Increment Count to point to Length     */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    if (orig_cmd_data[count+length_of_length_field-1] == 0) {
      /* no items */
      item->length = 0;
      item->item_id = 0;
      count = count + length_of_length_field;
    }
    else {  /* at least have item id */
      if (orig_cmd_data[count+length_of_length_field-1] == 1) {
          /* only has item id */
          item->length = 0;
      }
      else {

        /* length of menu item or select item         */
        /* length of this item contains an Item ID    */
        /* followed by the Text.  Need to capture the */
        /* Item ID                                    */
        /* Need to subtract 1 because ITEM ID will be */
        /* stored as a structure Member               */
        item->length = orig_cmd_data[count+length_of_length_field-1] - 1;
      } /* length field is greater than 1 */

      count = count + length_of_length_field;

      /* Save off the Item ID   */
      item->item_id = orig_cmd_data[count];

      /* Copy the Contents of the Text */
      /* Increment Counter to point to */
      /* the beginning of the text     */
      count ++;

      /* First Allocate Space for the Text */
      item->item_text = (uint8*)GSTK_CALLOC(
                        ( item->length + 1 ) * sizeof ( uint8 ) );

      if(item->item_text == NULL)
      {
          return GSTK_MEMORY_ERROR;
      }

      memset(item->item_text,
            0x00,
            item->length + 1);

      /* OK to memcopy the Item Text */
      (void)memscpy(item->item_text,
            item->length,
            &orig_cmd_data[count],
            item->length);

      /* Set the Count to check for  */
      /* Additional TLVs             */
      count += item->length;

      /* Increment the length to     */
      /* account for the Null        */
      item->length +=1;

      gstk_util_dump_byte_array("text", item->item_text, item->length);

    } /* if there is no item id or text */

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        if (item->item_text != NULL) {
          gstk_free(item->item_text);
          item->item_text = NULL;
        }
        item->length = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;
}


/*===========================================================================
FUNCTION gstk_parser_next_action_indicator_tlv

DESCRIPTION
    Generic Function used to parse the Next Action Indicator TLV.

    Next Action Indicator TLV Appear As:
        ITEMS NEXT ACTION
        INDICATOR TAG       - 1 uint8
        LENGTH (X)          - 1 uint8
        ITEMS NEXT ACTION
        INDICATOR LIST      - X uint8S (Determined by LENGTH (X) )

DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type  gstk_parser_next_action_indicator_tlv(
    uint8                             * orig_cmd_data,
    int32                               cmd_data_len,
    int32                               orig_offset,
    gstk_next_item_ind_type           * next_action_ind_list,
    int32                             * next_tlv_offset,
    boolean                           * more_tlvs,
    boolean                             man_opt_specifier,
    const uint32                      * ref_id
)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;

    UIM_MSG_HIGH_0("** Parsing nxt action item");

    if((!next_action_ind_list) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }
    count = orig_offset;

    /* ITEM TAG TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* Item TLV Is Valid Proceed with Parsing */
    /* Increment Count to point to Length     */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(
                                        &orig_cmd_data[count]);

    /* Length of this item is the second uint8 of  */
    /* the TLV and is not follwed by any other    */
    /* TAGS.                                    */
    next_action_ind_list->num_items = orig_cmd_data[count+length_of_length_field-1];

    count += length_of_length_field;

    /* Verify the number of next action indicator */
    /* items doesn't succeed the number max number*/
    /* of items supported by ME                     */
    if ( next_action_ind_list->num_items > GSTK_MAX_MENU_ITEMS )
    {
      if (tag_comprehension_req) {

        UIM_MSG_HIGH_2("ITEMS_NEXT_ACTION_INDICATOR ITEMS EXCEEDS MAX %x > %x",
                       next_action_ind_list->num_items, GSTK_MAX_MENU_ITEMS);
        return GSTK_BAD_PARAM;
      }
      else {
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
        /* Increment to End of Items */
        count += next_action_ind_list->num_items;
        next_action_ind_list->num_items = 0;
      }
    }
    else {

      /* First Allocate Space for the Text */
      next_action_ind_list->next_action_list = (uint8*)GSTK_CALLOC(
                                     (next_action_ind_list->num_items));

      if(   next_action_ind_list->next_action_list == NULL)
      {
        return GSTK_MEMORY_ERROR;
      }

      memset( next_action_ind_list->next_action_list,
              0xFF,
              next_action_ind_list->num_items);

      /* OK to memcopy the Item Text */
      (void)memscpy(next_action_ind_list->next_action_list,
              next_action_ind_list->num_items,
              (void *)(&orig_cmd_data[count]),
              next_action_ind_list->num_items);

      /* Set the Count to check for  */
      /* Additional TLVs               */

      /* Increment to End of Items */
      count += next_action_ind_list->num_items;
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        if (next_action_ind_list->next_action_list != NULL) {
          gstk_free(next_action_ind_list->next_action_list);
          next_action_ind_list->next_action_list = NULL;
        }
        next_action_ind_list->num_items = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;
}


/*===========================================================================
FUNCTION gstk_parser_item_icon_identifier_list_tlv

DESCRIPTION
    Generic Function used to parse the Item Icon Identifier TLV .

    Item Icon Identifier TLV Appear As:
    ITEMS ICON
    IDENTIFIER TAG      - 1 uint8
    LENGTH (X)          - 1 uint8
    ICON LIST QUALIFIER - 1 uint8
                          Bit 1 --> 0 Icon is self Explanitory (replace Text)
                                    --> 1 Icon is not selx Explanitory (show Text
                                      & Icon)
    ICON IDENTIFIER
    LIST                - X uint8S (Determined by LENGTH (X) )

DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type  gstk_parser_item_icon_identifier_list_tlv(
    uint8                             * orig_cmd_data,
    int32                               cmd_data_len,
    int32                               orig_offset,
    gstk_icon_id_list_type            * icon_id_list,
    int32                             * next_tlv_offset,
    boolean                           * more_tlvs,
    boolean                             man_opt_specifier,
    const uint32                      * ref_id
)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           loop_control           = 0;
    uint8                           qualifier              = 0;

    UIM_MSG_HIGH_0("** Parsing icon id lst");

    if((!icon_id_list) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }
    count = orig_offset;

    /* ITEM TAG TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* Item TLV Is Valid Proceed with Parsing */
      /* Increment Count to point to Length     */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(
                                        &orig_cmd_data[count]);

    if (tag_comprehension_req) {
      if(orig_cmd_data[count+length_of_length_field-1] == 0) {
        /* no item id or anything */
        icon_id_list->num_items = 0;
        icon_id_list->display_icon_only = 0;
        count = count + length_of_length_field;
      }
      else { /* at least have display_icon qualifier */
        if(orig_cmd_data[count+length_of_length_field-1] == 1) {
          /* no icon info */
          icon_id_list->num_items = 0;
        }
        else { /* have icons */

          /* Length of this item is the second uint8 of  */
          /* the TLV and is not follwed by any other    */
          /* TAGS.                                    */
          /* Subtract 1 from length due to 1 uint8       */
          /* representing Icon List Qualifier           */
          icon_id_list->num_items = orig_cmd_data[count+length_of_length_field-1]
                                  - 1;
        }

        count = count + length_of_length_field;

        /* Save the Icon List Qualifier                 */
        /* 0x01 represent Icon List Qualifier Mask      */
        icon_id_list->display_icon_only = (orig_cmd_data[count] & 0x01) ? FALSE : TRUE;
        qualifier = orig_cmd_data[count];

        /* Each Subsequent uint8 after the Icon List   */
        /* Qualifier needs to be further parsed       */
        /* Increment Count to first item */
        count++;
        while ( ( loop_control < icon_id_list->num_items ) &&
                ( gstk_status == GSTK_SUCCESS ) )
        {
            if ( orig_cmd_data[count] != 0x00 )
            {
                /* Add Code uint8 is not 0x00 */
                /* Need to use uint8 to parse */
                /* IMG Record Accordingly     */

                /* Need to allocate memory for */
                /* icon type                   */
                icon_id_list->icon_list[loop_control] =
                  (gstk_icon_type*)GSTK_CALLOC(sizeof(gstk_icon_type));
                if ( icon_id_list->icon_list[loop_control] != NULL )
                {
                    memset(icon_id_list->icon_list[loop_control],
                            0x0,
                            sizeof(gstk_icon_type));
                }
                else
                {
                    return GSTK_MEMORY_ERROR;
                }

                icon_id_list->icon_list[loop_control]->rec_num = orig_cmd_data[count];
                icon_id_list->icon_list[loop_control]->qualifier = qualifier;
                gstk_status = gstk_parse_img_record(orig_cmd_data[count],
                                                    (gstk_icon_type *)icon_id_list->icon_list[loop_control]);
                if (gstk_status != GSTK_SUCCESS) {
                  break;
                }
            }
            else
            {
                /* Add Code uint8 is 0x00 */
                /* not Icon ID for item  */

            }

            loop_control++;
            count++;
        }
      } /* no icon list or display info */
    }
    else {
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + orig_cmd_data[count+length_of_length_field-1] +
               length_of_length_field;
      icon_id_list->num_items = 0;
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        for ( loop_control = 0; loop_control < icon_id_list->num_items; loop_control++ ) {
          if (icon_id_list->icon_list[loop_control] != NULL) {
            icon_id_list->icon_list[loop_control]->rec_num = 0;
            if (icon_id_list->icon_list[loop_control]->data != NULL) {
              gstk_free(icon_id_list->icon_list[loop_control]->data);
              icon_id_list->icon_list[loop_control]->data = NULL;
            }
            gstk_free(icon_id_list->icon_list[loop_control]);
            icon_id_list->icon_list[loop_control] = NULL;
          }
        }
        icon_id_list->num_items = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;
}




/*===========================================================================
FUNCTION gstk_parser_bc_repeat_ind_tlv

DESCRIPTION
    This function parses the bc repeat indicator TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more_tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  bc_repeat: [Input/Output] Pointer to the bc repeat indicator,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_bc_repeat_ind_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_bc_repeat_ind_enum_type   *bc_repeat,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing BC repeat ind");

    if((!bc_repeat) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_BC_REPEAT_IND_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* bc repeat indicator TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_BC_REPEAT_INDICATOR_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_BC_REPEAT_INDICATOR_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid bc repeat indicator Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    if(tlv_length < GSTK_BC_REPEAT_IND_LEN) {
      UIM_MSG_ERR_1("Invalid BC repeat indicator len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count += length_of_length_field;
      *bc_repeat = (gstk_bc_repeat_ind_enum_type)orig_cmd_data[count];

      UIM_MSG_HIGH_3("Raw Data[%d]: 0x%x, Decoded bc repeat 0x%x",
                     count, orig_cmd_data[count], *bc_repeat);

      if(*bc_repeat == GSTK_BC_REPEAT_IND_MAX)
      {
        if (tag_comprehension_req) {
          UIM_MSG_ERR_1("Unknown mode, 0x%x", *bc_repeat);
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
          }
          UIM_MSG_HIGH_0("Sequential mode");
          *bc_repeat = GSTK_SEQUENTIAL_MODE;
        }
      }
      count++;
    }

    /* next TLV offset */
    /* Check if TLV total length indicated by the Card is the same as the incremented value */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
}

/*===========================================================================
FUNCTION gstk_parser_item_identifier_tlv

DESCRIPTION
    Generic Function used to parse the Item Icon Identifier TLV .

    Item Identifier TLV Appear As:
    ITEMS IDENTIFIER TAG    - 1 Byte
    LENGTH (X)              - 1 Byte
    ITEM CHOSEN             - 1 Byte

DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type  gstk_parser_item_identifier_tlv(
    uint8                               * orig_cmd_data,
    int32                                 cmd_data_len,
    int32                                 orig_offset,
    uint8                               * default_item,
    int32                               * next_tlv_offset,
    boolean                             * more_tlvs,
    boolean                               man_opt_specifier,
    const uint32                        * ref_id
)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing item id");

    if((!default_item) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* ITEM TAG TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_ITEM_IDENTIFIER_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ITEM_IDENTIFIER_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* Item TLV Is Valid Proceed with Parsing */
    /* Increment Count to point to Length     */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(
                                        &orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count+length_of_length_field-1];

    /* Verify the Length indicated is     */
    /* exactly 0x01 byte                  */
    if (tlv_length < 0x01 )
    {
      UIM_MSG_ERR_1("Invalid Len 0x%x", orig_cmd_data[count]);
      return GSTK_INVALID_LENGTH;
    }

    count += length_of_length_field;
    /* Range check on the Default Item    */
    /* value                              */
    /* Range must be 0x01 - 0xFF          */
    if ( orig_cmd_data[count] >= 01 )
    {
        /* less than 0xFF check is not needed because orig_cmd_data is uint8
           array => data will never be greater than 0xFF */
        /* Save the Identifier of the item    */
        /* chosen                             */
        *default_item = orig_cmd_data[count];
    }
    else
    {
      /* Set to NLL.  Either the value is  */
      /* NULL or the value did not pass    */
      /* the range check specified         */
      *default_item = 0x00;
      if (!tag_comprehension_req)
      {
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
      }
    }
    /* Increment to next tlv, before increment, count is at the position of the
       default item ID; therefore, add the tlv_length to move to the tag position
       of the next tlv */
    count = count + tlv_length;

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
}

/*===========================================================================
FUNCTION gstk_parser_file_list_tlv

DESCRIPTION
    This function parses the file list information provided as optional
    data in commands like REFRESH.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  file_list: [Input/Output] Pointer to file list buffer,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_file_list_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_file_list_type            *file_list,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int                             i                      = 0;

    UIM_MSG_HIGH_0("Parsing File List");

    if((!file_list) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least have tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* provisioning file TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_FILE_LIST_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_FILE_LIST_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid provisioning file Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    file_list->length = orig_cmd_data[count + length_of_length_field - 1];

    /* position count to the provisioning file location in the tlv */
    count = count + length_of_length_field;

    if (file_list->length != 0)
    {
      file_list->file_paths = GSTK_CALLOC(file_list->length);
      if (file_list->file_paths == NULL)
      {
        if (tag_comprehension_req) {
          return GSTK_MEMORY_ERROR;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
          }
          count = count + file_list->length;
          file_list->num_of_files = 0;
        }
      }
      else 
      {
        memset(file_list->file_paths, 0x00, file_list->length);
        for (i = 0; i < file_list->length; i++)
        {
          file_list->file_paths[i] = orig_cmd_data[count];
          if((orig_cmd_data[count] == 0x3F) && (orig_cmd_data[count+1] == 0x00)) { /* new file */
            file_list->num_of_files++;
          }
          count++;
        }

        UIM_MSG_HIGH_1("# Files: 0x%x, ============ Decoded file",
                       file_list->num_of_files);
        gstk_util_dump_byte_array("file_list", file_list->file_paths, file_list->length);
      }
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                    cmd_data_len, count);
        if (file_list->file_paths != NULL) {
          gstk_free(file_list->file_paths);
          file_list->file_paths = NULL;
        }
        file_list->num_of_files = 0;
        return GSTK_INVALID_LENGTH;
    }

    return gstk_status;

} /* gstk_parser_file_list_tlv */



/*===========================================================================
FUNCTION gstk_parser_timer_id_tlv

DESCRIPTION
    This function parses the timer id, If there is no more TLVs to be parsed,
    the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  timer_id: [Input/Output] Pointer to timer id
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_timer_id_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       uint8                          *timer_id,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int                             tlv_length             = 0;

    UIM_MSG_HIGH_0("Parsing timer id");

    if((!timer_id) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least have tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_TIMER_IDENTITY_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* timer value TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_TIMER_IDENTIFIER_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_TIMER_IDENTIFIER_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid timer id Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if(tlv_length < GSTK_TIMER_IDENTITY_LEN) {
      UIM_MSG_ERR_1("Invalid timer id len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count += length_of_length_field;
      *timer_id = orig_cmd_data[count];

      UIM_MSG_HIGH_1("Timer ID %d ", *timer_id);

      /* increment for next TLV */
      count++;

      /* invalid timer id */
      if ((*timer_id > GSTK_MAX_TIMER_SUPPORTED) || (*timer_id < 1)) {
        if (tag_comprehension_req) {
          UIM_MSG_ERR_1("Unknown timer_id 0x%x", *timer_id);
          return GSTK_ERROR;
        }
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        }
      }
    }

    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
} /* gstk_parser_timer_id_tlv */



/*===========================================================================
FUNCTION gstk_parser_timer_value_tlv

DESCRIPTION
    This function parses the timer value, If there is no more TLVs to be parsed,
    the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  timer_value: [Input/Output] Pointer to timer value
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_timer_value_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_timer_value_type          *timer_value,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;
    byte                            temp_time              = 0;

    UIM_MSG_HIGH_0("Parsing timer value");

    if((!timer_value) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least have tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_TIMER_IDENTITY_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* timer value TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_TIMER_VALUE_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_TIMER_VALUE_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid timer value Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if(tlv_length < GSTK_TIMER_VALUE_LEN) {
      UIM_MSG_ERR_1("Invalid timer value len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count += length_of_length_field;
      /* hour */
      temp_time = orig_cmd_data[count++];
      timer_value->hour =  ((temp_time & 0x0F)*10) + ((temp_time >> 0x04) & 0x0F);
      temp_time = orig_cmd_data[count++];
      timer_value->minute =  ((temp_time & 0x0F)*10) + ((temp_time >> 0x04) & 0x0F);
      temp_time = orig_cmd_data[count++];
      timer_value->second =  ((temp_time & 0x0F)*10) + ((temp_time >> 0x04) & 0x0F);

      UIM_MSG_HIGH_3("Time value: hr = %d, min = %d, sec = 0x%x",
                     timer_value->hour,
                     timer_value->minute,
                     timer_value->second);
    }

    /* next TLV offset */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
} /* gstk_parser_timer_value_tlv */


/*===========================================================================
FUNCTION gstk_parser_lang_tlv

DESCRIPTION
    This function parses the language tlv, If there is no more TLVs to be parsed,
    the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  lang: [Input/Output] Pointer to lang code
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_lang_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_lang_type                 *lang,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("Parsing lang");

    if((!lang) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* at least have tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,GSTK_LANGUAGE_LEN))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* lang TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_LANGUAGE_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_LANGUAGE_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid lang Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if(tlv_length < GSTK_LANGUAGE_LEN) {
      UIM_MSG_ERR_1("Invalid lang len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count++;

      /* lang */
      lang->lang_code[0] = orig_cmd_data[count++];
      lang->lang_code[1] = orig_cmd_data[count++];

      UIM_MSG_HIGH_2("Lang 0x%x, 0x%x",
                     lang->lang_code[0], lang->lang_code[1]);
    }

    /* next TLV offset */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    return gstk_status;
} /* gstk_parser_lang_tlv */

/*===========================================================================
FUNCTION gstk_parser_dtmf_tlv

DESCRIPTION
    This function parses the dtmf tlv, If there is no more TLVs to be parsed,
    the more tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  dtmf: [Input/Output] Pointer to dtmf
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_dtmf_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_dtmf_type                 *dtmf,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("Parsing dtmf");

    if((!dtmf) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* have at least tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* dtmf TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_DTMF_STRING_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_DTMF_STRING_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    /* valid dtmf Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    dtmf->length = orig_cmd_data[count + length_of_length_field -1];

    /* position count to the index for tpdu */
    count = count + length_of_length_field;

    /* dtmf */
    /* get memory */
    if (dtmf->length > 0) {
      dtmf->dtmf =
          (uint8*)GSTK_CALLOC(dtmf->length);
      if(dtmf->dtmf == NULL) {
        return GSTK_MEMORY_ERROR;
      }
      memset(dtmf->dtmf, 0x00, dtmf->length);
      /* copy the dtmf info */
      (void)memscpy(dtmf->dtmf, dtmf->length, &orig_cmd_data[count], dtmf->length);

      gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], dtmf->length);
    }

    /* position count to the next tlv starting location */
    count = count + dtmf->length;

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
    if (gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                    cmd_data_len, count);
      if (dtmf->dtmf != NULL) {
        gstk_free(dtmf->dtmf);
        dtmf->dtmf = NULL;
      }
      dtmf->length = 0;
      return GSTK_INVALID_LENGTH;
    }
    return gstk_status;

} /* gstk_parser_dtmf_tlv */

/*===========================================================================
FUNCTION gstk_parser_csd_bearer_param

DESCRIPTION
    This function extracts the csd bearer information from the command data
    buffer.

PARAMETERS
  uint8                      *cmd_data   - pointer to command data buffer
  gstk_csd_bearer_param_type *csd_bearer - csd bearer information

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_parser_csd_bearer_param(
  const uint8                *cmd_data,
  gstk_csd_bearer_param_type *csd_bearer)
{
  if ((csd_bearer == NULL) || (cmd_data == NULL)) {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_MEMORY_ERROR;
  }

  UIM_MSG_HIGH_3("CSD Bearer Speed 0x%x, CSD Bearer Name 0x%x, CSD Bearer Connectino Element 0x%x",
                 cmd_data[0], cmd_data[1], cmd_data[2]);

  switch (cmd_data[0]) {
  case 0:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_AUTO;
    break;
  case 1:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_300_V21;
    break;
  case 2:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_1200_V22;
    break;
  case 3:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_1200_75_V23;
    break;
  case 4:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_2400_V22;
    break;
  case 5:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_2400_V26;
    break;
  case 6:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_4800_V32;
    break;
  case 7:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_9600_V32;
    break;
  case 12:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_9600_V34;
    break;
  case 14:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_14400_V34;
    break;
  case 15:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_19200_V34;
    break;
  case 16:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_28800_V34;
    break;
  case 17:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_33600_V34;
    break;
  case 34:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_1200_V120;
    break;
  case 36:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_2400_V120;
    break;
  case 38:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_4800_V120;
    break;
  case 39:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_9600_V120;
    break;
  case 43:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_14400_V120;
    break;
  case 47:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_19200_V120;
    break;
  case 48:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_28800_V120;
    break;
  case 49:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_38400_V120;
    break;
  case 50:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_48000_V120;
    break;
  case 51:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_56000_V120;
    break;
  case 65:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_300_V110;
    break;
  case 66:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_1200_V110;
    break;
  case 68:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_2400_V110_X31;
    break;
  case 70:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_4800_V110_X31;
    break;
  case 71:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_9600_V110_X31;
    break;
  case 75:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_14400_V110_X31;
    break;
  case 79:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_19200_V110_X31;
    break;
  case 80:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_28800_V110_X31;
    break;
  case 81:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_38400_V110_X31;
    break;
  case 82:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_48000_V110_X31;
    break;
  case 83:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_56000_V110_X31;
    break;
  case 84:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_64000_V110_X31;
    break;
  case 115:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT;
    break;
  case 116:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT;
    break;
  case 120:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_32000_PIAFS32K;
    break;
  case 121:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_64000_PIAFS32K;
    break;
  case 130:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_28800_MULTIMEDIA;
    break;
  case 131:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_32000_MULTIMEDIA;
    break;
  case 132:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_33600_MULTIMEDIA;
    break;
  case 133:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_56000_MULTIMEDIA;
    break;
  case 134:
    csd_bearer->speed = GSTK_CSD_BEARER_SPEED_64000_MULTIMEDIA;
    break;
  default:
    UIM_MSG_ERR_1("Unknown CSD Bearer Speed 0x%x", cmd_data[0]);
    return GSTK_INVALID_COMMAND;
  }
  switch (cmd_data[1]) {
  case 0:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ;
    break;
  case 1:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ;
    break;
  case 2:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI;
    break;
  case 3:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI;
    break;
  case 4:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI;
    break;
  case 5:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI;
    break;
  case 6:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI;
    break;
  case 7:
    csd_bearer->name = GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI;
    break;
  default:
    UIM_MSG_ERR_1("Unknown CSD Bearer Name 0x%x", cmd_data[1]);
    return GSTK_INVALID_COMMAND;
  }
  switch (cmd_data[2]) {
  case 0:
    csd_bearer->connection_element = GSTK_CSD_BEARER_CE_TRANSPARENT;
    break;
  case 1:
    csd_bearer->connection_element = GSTK_CSD_BEARER_CE_NON_TRANSPARENT;
    break;
  case 2:
    csd_bearer->connection_element = GSTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF;
    break;
  case 3:
    csd_bearer->connection_element = GSTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF;
    break;
  default:
    UIM_MSG_ERR_1("Unknown CSD Bearer Connection Element 0x%x", cmd_data[1]);
    return GSTK_INVALID_COMMAND;
  }
  return GSTK_SUCCESS;

} /* gstk_parser_csd_bearer_param */

/*===========================================================================
FUNCTION gstk_parser_eutran_mapped_utran_ps_bearer_param

DESCRIPTION
    This function extracts the EUTRAN mapped UTRAN PS bearer information
    from the command data buffer.

PARAMETERS
  uint8                      *cmd_data     - pointer to command data buffer
  gstk_gprs_bearer_param_type *gprs_bearer - gprs bearer information

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_parser_eutran_mapped_utran_ps_bearer_param(
  const uint8                             *cmd_data,
  gstk_eutran_mapped_utran_ps_param_type  *eutran_mapped_utran_ps_bearer)
{
  if ((eutran_mapped_utran_ps_bearer == NULL) || (cmd_data == NULL)) {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_MEMORY_ERROR;
  }

  eutran_mapped_utran_ps_bearer->qci = cmd_data[0];
  eutran_mapped_utran_ps_bearer->max_bitrate_ul = cmd_data[1];
  eutran_mapped_utran_ps_bearer->max_bitrate_dl = cmd_data[2];
  eutran_mapped_utran_ps_bearer->guaranteed_bitrate_ul = cmd_data[3];
  eutran_mapped_utran_ps_bearer->guaranteed_bitrate_dl = cmd_data[4];
  eutran_mapped_utran_ps_bearer->max_bitrate_ul_ext = cmd_data[5];
  eutran_mapped_utran_ps_bearer->max_bitrate_dl_ext = cmd_data[6];
  eutran_mapped_utran_ps_bearer->guaranteed_bitrate_ul_ext = cmd_data[7];
  eutran_mapped_utran_ps_bearer->guaranteed_bitrate_dl_ext = cmd_data[8];
  eutran_mapped_utran_ps_bearer->pdp_type = (gstk_packet_data_protocol_enum_type)cmd_data[9];

  UIM_MSG_HIGH_3("qci = 0x%x, max_bitrate_ul = 0x%x, max_bitrate_dl = 0x%x",
                 eutran_mapped_utran_ps_bearer->qci,
                 eutran_mapped_utran_ps_bearer->max_bitrate_ul,
                 eutran_mapped_utran_ps_bearer->max_bitrate_dl);
  UIM_MSG_HIGH_3("guaranteed_bitrate_ul = 0x%x, guaranteed_bitrate_dl = 0x%x, max_bitrate_ul_ext = 0x%x",
                 eutran_mapped_utran_ps_bearer->guaranteed_bitrate_ul,
                 eutran_mapped_utran_ps_bearer->guaranteed_bitrate_dl,
                 eutran_mapped_utran_ps_bearer->max_bitrate_ul_ext);
  UIM_MSG_HIGH_3("max_bitrate_dl_ext = 0x%x, guaranteed_bitrate_ul_ext = 0x%x, guaranteed_bitrate_dl_ext = 0x%x",
                 eutran_mapped_utran_ps_bearer->max_bitrate_dl_ext,
                 eutran_mapped_utran_ps_bearer->guaranteed_bitrate_ul_ext,
                 eutran_mapped_utran_ps_bearer->guaranteed_bitrate_dl_ext);
  UIM_MSG_HIGH_1("pdp_type = 0x%x", eutran_mapped_utran_ps_bearer->pdp_type);

  return GSTK_SUCCESS;
} /*gstk_parser_eutran_mapped_utran_ps_bearer_param*/

/*===========================================================================
FUNCTION gstk_parser_eutran_ext_bearer_param

DESCRIPTION
    This function extracts the EUTRAN extended bearer information from the
    command data buffer.

PARAMETERS
  uint8                      *cmd_data     - pointer to command data buffer
  gstk_gprs_bearer_param_type *gprs_bearer - gprs bearer information

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_parser_eutran_ext_bearer_param(
  const uint8                 *cmd_data,
  gstk_eutran_ext_param_type  *eutran_ext_bearer)
{
  if ((eutran_ext_bearer == NULL) || (cmd_data == NULL)) {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_MEMORY_ERROR;
  }

  eutran_ext_bearer->traffic_class = cmd_data[0];
  eutran_ext_bearer->max_bitrate_ul =
    ((uint16)cmd_data[1]) | (((uint16)cmd_data[2]) << 8);
  eutran_ext_bearer->max_bitrate_dl =
    ((uint16)cmd_data[3]) | (((uint16)cmd_data[4]) << 8);
  eutran_ext_bearer->guaranteed_bitrate_ul =
    ((uint16)cmd_data[5]) | (((uint16)cmd_data[6]) << 8);
  eutran_ext_bearer->guaranteed_bitrate_dl =
    ((uint16)cmd_data[7]) | (((uint16)cmd_data[8]) << 8);;
  eutran_ext_bearer->delivery_order = cmd_data[9];
  eutran_ext_bearer->max_sdu_size   = cmd_data[10];
  eutran_ext_bearer->sdu_error_ratio = cmd_data[11];
  eutran_ext_bearer->residual_bit_error_ratio = cmd_data[12];
  eutran_ext_bearer->delivery_of_err_sdu = cmd_data[13];
  eutran_ext_bearer->transfer_delay = cmd_data[14];
  eutran_ext_bearer->traffic_handling_pri = cmd_data[15];
  eutran_ext_bearer->pdp_type = (gstk_packet_data_protocol_enum_type)cmd_data[16];

  UIM_MSG_HIGH_3("traffic_class = 0x%x, max_bitrate_ul = 0x%x, max_bitrate_dl = 0x%x",
                 eutran_ext_bearer->traffic_class,
                 eutran_ext_bearer->max_bitrate_ul,
                 eutran_ext_bearer->max_bitrate_dl);
  UIM_MSG_HIGH_3("guaranteed_bitrate_ul = 0x%x, guaranteed_bitrate_dl = 0x%x, delivery_order = 0x%x",
                 eutran_ext_bearer->guaranteed_bitrate_ul,
                 eutran_ext_bearer->guaranteed_bitrate_dl,
                 eutran_ext_bearer->delivery_order);
  UIM_MSG_HIGH_3("max_sdu_size = 0x%x, sdu_error_ratio = 0x%x, residual_bit_error_ratio = 0x%x",
                 eutran_ext_bearer->max_sdu_size,
                 eutran_ext_bearer->sdu_error_ratio,
                 eutran_ext_bearer->residual_bit_error_ratio);
  UIM_MSG_HIGH_3("delivery_of_err_sdu = 0x%x, transfer_delay = 0x%x, traffic_handling_pri = 0x%x",
                 eutran_ext_bearer->delivery_of_err_sdu,
                 eutran_ext_bearer->transfer_delay,
                 eutran_ext_bearer->traffic_handling_pri);
  UIM_MSG_HIGH_1("pdp_type = 0x%x", eutran_ext_bearer->pdp_type);
  return GSTK_SUCCESS;
} /*gstk_parser_eutran_ext_bearer_param*/

/*===========================================================================
FUNCTION gstk_parser_gprs_bearer_param

DESCRIPTION
    This function extracts the gprs bearer information from the command data
    buffer.

PARAMETERS
  uint8                      *cmd_data     - pointer to command data buffer
  gstk_gprs_bearer_param_type *gprs_bearer - gprs bearer information

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static gstk_status_enum_type gstk_parser_gprs_bearer_param(
  const uint8                 *cmd_data,
  gstk_gprs_bearer_param_type *gprs_bearer)
{
  if ((gprs_bearer == NULL) || (cmd_data == NULL)) {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_MEMORY_ERROR;
  }

  gprs_bearer->precedence_cls = cmd_data[0];
  gprs_bearer->delay_cls = cmd_data[1];
  gprs_bearer->reliability_cls = cmd_data[2];
  gprs_bearer->peak_throughput = cmd_data[3];
  gprs_bearer->mean_throughput = cmd_data[4];
  UIM_MSG_HIGH_2("GPRS Bearer precedence 0x%x  :  0x%x",
                 cmd_data[0], gprs_bearer->precedence_cls);
  UIM_MSG_HIGH_2("GPRS Bearer delay 0x%x  :  0x%x",
                 cmd_data[1], gprs_bearer->delay_cls);
  UIM_MSG_HIGH_2("GPRS Bearer reliability 0x%x  :  0x%x",
                 cmd_data[2], gprs_bearer->reliability_cls);
  UIM_MSG_HIGH_2("GPRS Bearer peak throughput 0x%x  :  0x%x",
                 cmd_data[3], gprs_bearer->peak_throughput);
  UIM_MSG_HIGH_2("GPRS Bearer mean throughput 0x%x  :  0x%x",
                 cmd_data[4], gprs_bearer->mean_throughput);
  UIM_MSG_HIGH_1("GPRS Bearer pkt data protocol 0x%x", cmd_data[5]);

  switch(cmd_data[5]) {
  case 0x02:
    gprs_bearer->pkt_data_protocol = GSTK_PKT_DATA_IP;
    return GSTK_SUCCESS;
  default:
    return GSTK_INVALID_COMMAND;
  }

} /* gstk_parser_gprs_bearer_param */

/*===========================================================================
FUNCTION gstk_parser_bearer_description_tlv

DESCRIPTION
    Generic Function used by GSTK to parse the Bearer Description TLV

PARAMETERS
  uint8                         * orig_cmd_data        - command data buffer
  int32                           cmd_data_len         - the length of the
                                                         command buffer
  int32                           orig_offset          - the offset to start
                                                         reading bearer from
  gstk_bearer_description_type  * bearer_description   - bearer details
  int32                         * next_tlv_offset      - pointer to next TLV
  boolean                       * more_tlv             - Indicates if there are
                                                         more TLVs

DEPENDENCIES

RETURN VALUE
  gstk_status_enum_type

===========================================================================*/
gstk_status_enum_type gstk_parser_bearer_description_tlv(
    uint8                         * orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    gstk_bearer_description_type  * bearer_description,
    int32                         * next_tlv_offset,
    boolean                       * more_tlv,
    boolean                         man_opt_specifier,
    const uint32                  * ref_id
)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           len_bearer             = 0;
    boolean                         tag_comprehension_req  = TRUE;
    uint32                          index                  = 0;

    UIM_MSG_HIGH_0("** Parsing bearer description");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
      else
        return GSTK_SUCCESS;
    }

    if (bearer_description == NULL) {
      UIM_MSG_ERR_0("Null Input Param");
      return GSTK_MEMORY_ERROR;
    }

    if (ref_id != NULL) {
      index = GSTK_CMD_REF_TABLE_INDEX(*ref_id);
    }

    /* Initialize Count */
    count = orig_offset;

    /* bearer description TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_BEARER_DESCRIPTION_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_BEARER_DESCRIPTION_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid bearer description Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* Increment count to beginning of description */
    count = count + length_of_length_field;

    len_bearer = orig_cmd_data[count-1] - 1;  /* length of the bearer param is the len of TLV - bearer type */

    switch(orig_cmd_data[count]) {
    case 0x01:
      if (len_bearer < GSTK_CSD_BEARER_PARAM_LEN) {
        if (tag_comprehension_req) {
          return GSTK_INVALID_LENGTH;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
          }
        }
      }
      else {
        bearer_description->bearer_type = GSTK_CSD;
        UIM_MSG_HIGH_0("Bearer Type: CSD");
        count++;
        gstk_status = gstk_parser_csd_bearer_param(&orig_cmd_data[count], &bearer_description->bearer_params.csd_bearer);
      }
      count+=len_bearer;
      break;
    case 0x02:
      if (len_bearer < GSTK_GPRS_BEARER_PARAM_LEN) {
        if (tag_comprehension_req) {
          return GSTK_INVALID_LENGTH;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
          }
        }
      }
      else {
        bearer_description->bearer_type = GSTK_GPRS;
        UIM_MSG_HIGH_0("Bearer Type: GPRS");
        count++;
        gstk_status = gstk_parser_gprs_bearer_param(&orig_cmd_data[count], &bearer_description->bearer_params.gprs_bearer);
      }
      count+=len_bearer;
      break;
    case 0x03:
      bearer_description->bearer_type = GSTK_BEARER_DEFAULT;
      UIM_MSG_HIGH_0("Bearer Type: DEFAULT");
      count++;
      count+=len_bearer;
      break;
    case 0x09:
      if (len_bearer < GSTK_EUTRAN_EXT_PARAM_LEN) {
        if (tag_comprehension_req) {
          return GSTK_INVALID_LENGTH;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
          }
        }
      }
      else {
        bearer_description->bearer_type = GSTK_EUTRAN_EXT_PARAM;
        UIM_MSG_HIGH_0("Bearer Type: EUTRAN EXT PARAM");
        count++;
        gstk_status = gstk_parser_eutran_ext_bearer_param(
                        &orig_cmd_data[count],
                        &bearer_description->bearer_params.eutran_ext_bearer);
      }
      count+=len_bearer;
      break;
    case 0x0B:
      if (len_bearer < GSTK_EUTRAN_MAPPED_UTRAN_PS_PARAM_LEN) {
        if (tag_comprehension_req) {
          return GSTK_INVALID_LENGTH;
        }
        else {
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
          }
        }
      }
      else {
        bearer_description->bearer_type = GSTK_EUTRAN_MAPPED_UTRAN_PS;
        UIM_MSG_HIGH_0("Bearer Type: EUTRAN MAPPED UTRAN PS PARAM");
        count++;
        gstk_status = gstk_parser_eutran_mapped_utran_ps_bearer_param(
                        &orig_cmd_data[count],
                        &bearer_description->bearer_params.eutran_mapped_utran_ps_bearer);
      }
      count+=len_bearer;
      break;
    default:
      UIM_MSG_ERR_1("Reserved bearer type, 0x%x", orig_cmd_data[count]);
      if (tag_comprehension_req) {
        return GSTK_INVALID_COMMAND;
      }
      else {
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
        }
      }
      count+=len_bearer;
    }

    if (gstk_status != GSTK_SUCCESS) { /* bearer param parsing failed */
      /* update to next tlv, and keep the gstk_status */
      (void)gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }
    else {
      /* next TLV offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
    }
    return gstk_status;
}

/*===========================================================================
FUNCTION gstk_parser_buffer_size_tlv

DESCRIPTION
    Generic Function used by GSTK to parse the buffer size TLV

PARAMETERS
  uint8                         * orig_cmd_data        - command data buffer
  int32                           cmd_data_len         - the length of the
                                                         command buffer
  int32                           orig_offset          - the offset to start
                                                         reading bearer from
  int32                         * buffer_size          - buffer size
  int32                         * next_tlv_offset      - pointer to next TLV
  boolean                       * more_tlv             - Indicates if there are
                                                         more TLVs

DEPENDENCIES

RETURN VALUE
  gstk_status_enum_type

===========================================================================*/
gstk_status_enum_type gstk_parser_buffer_size_tlv(
    uint8                         * orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    int32                         * buffer_size,
    int32                         * next_tlv_offset,
    boolean                       * more_tlv,
    boolean                         man_opt_specifier,
    const uint32                  * ref_id )
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing buffer size");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
      else
        return GSTK_SUCCESS;
    }

    if (buffer_size == NULL) {
      UIM_MSG_ERR_0("Null Input Param");
      return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* buffer size TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_BUFFER_SIZE_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_BUFFER_SIZE_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid buffer size Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if ( tlv_length < GSTK_BUFFER_SIZE_LEN ) {
      UIM_MSG_ERR_1("Invalid icon id len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      /* Increment count to beginning of data */
      count = count + length_of_length_field;

      /* Extract two bytes of buffer size from orig_cmd_data */
      *buffer_size = (orig_cmd_data[count] << 0x08) | orig_cmd_data[count+1];

      UIM_MSG_HIGH_3("Raw Data 0x%x, 0x%x: Buffer size is 0x%x",
                     orig_cmd_data[count],
                     orig_cmd_data[count+1],
                     *buffer_size);

      count+=2;
    }

    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
    }
    return gstk_status;
}

/*===========================================================================
FUNCTION gstk_convert_from_gsm_7bit_default

DESCRIPTION
  Generic Function used by GSTK to convert a gsm 7 bit defualt buffer to a uint16

PARAMETERS
  const uint8   * input_data - gsm 7 bit default buffer
  int32           len        - length of the input buffer
  uint16        * asc        - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static void

===========================================================================*/
static void gstk_convert_from_gsm_7bit_default
(
  const uint8   * input_data,
  int32           len,
  uint16        * asc
)
{
  uint32  j;
  int32  i;

  if((!input_data) || (!asc))
  {
    UIM_MSG_ERR_0("NULL PTR");
    return;
  }

  for( i = 0; i < len; i ++ )
  {
    j = input_data[i];
    asc[i] = gstk_gsmdefault_table[j];
  }

} /* gstk_convert_from_gsm_7bit_default() */

/*===========================================================================
FUNCTION gstk_decode_h80_usc2_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer

PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *ucs2_default_in - ucs2 buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32 : Length of NULL terminated output buffer.
  If this function returns negative value, it should be considered as
  an error.

===========================================================================*/
int32 gstk_decode_h80_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in,
                                    uint16             *alphabet_out)
{
  int i;
  int usc2_len = 0;
  boolean is_odd = FALSE;
  if(ucs2_default_in == NULL)
    return 0;

  if((in_len > 0) && (ucs2_default_in != NULL)) /*lint !e774 */
  {
    i = in_len;

    /* return length of decode data + terminated null byte */
    if ((i%2) != 0) {
      usc2_len = i/2 + 2;
      is_odd = TRUE;
    }
    else {
      usc2_len = i/2 + 1;
    }

    if (alphabet_out == NULL) {
      return usc2_len;
    }
    memset(alphabet_out, 0x00, sizeof(uint16)*int32touint32(usc2_len));

    for (i = 0; i < usc2_len-2; i++) { /* for loop to the second last uint16 */
      alphabet_out[i] = (ucs2_default_in[2*i] << 8) | ucs2_default_in[2*i+1];
    }
    /* take care of the last uint16 */
    if (is_odd) {
      alphabet_out[i] = (ucs2_default_in[2*i] << 8) | 0x00;
    }
    else {
      alphabet_out[i] = (ucs2_default_in[2*i] << 8) | ucs2_default_in[2*i+1];
    }
  }
  else
  {
    if (alphabet_out != NULL) {
      /* null terminated */
      alphabet_out[0] = 0;
    }
  }
  return usc2_len;
}/*lint !e818 *ucs2_default_in suppression for externalized function */
/* gstk_decode_h80_usc2_alphabet */

/*===========================================================================
FUNCTION gstk_decode_h81_usc2_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer

PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *ucs2_default_in - ucs2 buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32 : Length of NULL terminated output buffer.
  If this function returns negative value, it should be considered as
  an error.

===========================================================================*/
int32 gstk_decode_h81_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in,
                                    uint16             *alphabet_out)
{
  int i;
  int usc2_len = 0;
  uint16 base_value = 0;
  if(ucs2_default_in == NULL)
    return 0;

  if((in_len > 0) && (ucs2_default_in != NULL)) /*lint !e774 */
  {
    usc2_len = ucs2_default_in[0]; /* number of characters */
    if ((usc2_len+2) > in_len) {
      UIM_MSG_ERR_2("Pre parsing: Mismatch 0x81 type USC2 len 0x%x, 0x%x",
                    in_len, usc2_len);
      /* limit the data len to in_len-2 */
      usc2_len = in_len-2;
    }
    for(i = 0;  i < usc2_len; i++)
    {
      if(0xFF == ucs2_default_in[i])
      {
        ucs2_default_in[i] = 0;
        break;
      }
    }
    /* increment by 1 to reflect the new length,
       which include the "null" byte */
    usc2_len = ++i;

    if (alphabet_out == NULL) {
      return usc2_len;
    }
    memset(alphabet_out, 0x00, sizeof(uint16)*int32touint32(usc2_len));

    /* base value */
    base_value = ucs2_default_in[1] << 7;

    for (i = 0; i < usc2_len-1; i++) { /* for loop to the last byte in input array */
      if ((ucs2_default_in[i+2] & 0x80) == 0) {
        /* GSM default */
        gstk_convert_from_gsm_7bit_default(&ucs2_default_in[i+2], 1, &alphabet_out[i]);
      }
      else {
        alphabet_out[i] = (ucs2_default_in[i+2] - 0x80) + base_value;
      }
    }
  }
  else
  {
    if (alphabet_out != NULL) {
      /* null terminated */
      alphabet_out[0] = 0;
    }
  }
  return usc2_len;
} /* gstk_decode_h81_usc2_alphabet */

/*===========================================================================
FUNCTION gstk_decode_h82_usc2_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a usc2 buffer to a uint16 buffer

PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *ucs2_default_in - ucs2 buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32 : Length of NULL terminated output buffer.
  If this function returns negative value, it should be considered as
  an error.

===========================================================================*/
int32 gstk_decode_h82_usc2_alphabet(int32               in_len,
                                    uint8              *ucs2_default_in,
                                    uint16             *alphabet_out)
{
  int i = 0;
  int usc2_len = 0;
  uint16 base_value = 0;
  if(ucs2_default_in == NULL)
    return 0;

  if((in_len > 0) && (ucs2_default_in != NULL)) /*lint !e774 */
  {
    usc2_len = ucs2_default_in[0]; /* number of characters */
    if ((usc2_len+3) > in_len) {
      UIM_MSG_ERR_2("Pre parsing: Mismatch 0x81 type USC2 len 0x%x, 0x%x",
                    in_len, usc2_len);
      /* limit the data len to in_len-2 */
      usc2_len = in_len-3;
    }
    i = in_len;
    /* increment by 1 to reflect the new length,
       which include the "null" byte */
    usc2_len = ++i;

    if (alphabet_out == NULL) {
      return usc2_len;
    }
    memset(alphabet_out, 0x00, sizeof(uint16)*int32touint32(usc2_len));

    /* base value */
    base_value = (ucs2_default_in[1] << 8) + ucs2_default_in[2];

    for (i = 0; i < usc2_len-1; i++) { /* for loop to the last byte in input array */
      if ((ucs2_default_in[i+3] & 0x80) == 0) {
        /* GSM default */
        gstk_convert_from_gsm_7bit_default(&ucs2_default_in[i+3], 1, &alphabet_out[i]);
      }
      else {
        alphabet_out[i] = (ucs2_default_in[i+3] - 0x80) + base_value;
      }
    }
  }
  else
  {
    if (alphabet_out != NULL) {
      /* null terminated */
      alphabet_out[0] = 0;
    }
  }
  return usc2_len;
}/*lint !e818 *ucs2_default_in suppression for externalized function */
/* gstk_decode_h82_usc2_alphabet */

/*===========================================================================
FUNCTION gstk_convert_gsm_default_to_gstk_alphabet

DESCRIPTION
  Generic Function used by GSTK to convert a gsm default buffer to a uint16 buffer

PARAMETERS
  int32           in_len          - length of the input buffer
  uint8          *gsm_default_in  - gsm default buffer
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32

===========================================================================*/
static int32 gstk_convert_gsm_default_to_gstk_alphabet(int32               in_len,
                                                       uint8              *gsm_default_in,
                                                       uint16             *alphabet_out)
{
  int i=0;
  if(gsm_default_in == NULL)
    return 0;

  /* Remove all tarinling 0xFFs */
  if((in_len > 0) && (gsm_default_in != NULL)) /*lint !e774 */
  {
    for(i = 0;  i < in_len; i++)
    {
      if(0xFF == gsm_default_in[i])
      {
        gsm_default_in[i] = 0;
        break;
      }
    }
    /* increment by 1 to reflect the new length,
       which include the "null" byte */
    i++;
    if (alphabet_out == NULL) {
      /* return length of decode data + terminated null byte */
      return i;
    }
    memset(alphabet_out, 0x00, sizeof(uint16)*int32touint32(i));
    gstk_convert_from_gsm_7bit_default(gsm_default_in, i-1, alphabet_out);
  }
  else
  {
    if (alphabet_out != NULL) {
      /* null terminated */
      alphabet_out[0] = 0;
    }
  }
  return i;
}


/*===========================================================================
FUNCTION gstk_parser_alpha_identifier_tlv_ext

DESCRIPTION
    This function parses the TLV that is coded according to ADN 11.11 format
    into gstk_alphabet_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  alpha: [Input/Output] Pointer to the alpha of gstk_alphabet_type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_alpha_identifier_tlv_ext(
                       tags_enum_type                 tag,
                       uint8                         *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_alphabet_type            *alpha,
                       int32                         *next_tlv_offset,
                       boolean                       *more_tlv,
                       boolean                        man_opt_specifier,
                       const uint32                  *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    int32                           tlv_length             = 0;
    int                             i                      = 0;
    int32                           orig_alpha_len         = 0;

    UIM_MSG_HIGH_1("** Parsing 0x%x into GSTK_ALPHABET_TYPE", tag);

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
      else
        return GSTK_SUCCESS;
    }

    if (alpha == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* Initialize alpha */
    alpha->alphabet_given = FALSE;
    alpha->length = 0;

    /* check tag of the TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)tag,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(tag, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    alpha->alphabet_given = TRUE;

    /* valid alpha Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    orig_alpha_len = tlv_length;

    /* position count to the url location in the tlv */
    count = count + length_of_length_field;

    if (orig_alpha_len > 0) { /* Card provides a url address */
      /* Check first byte to see if the data is UCSII or GSM 7 bit default */
      switch(orig_cmd_data[count]) {
      case 0x80:
        orig_alpha_len--; /* the actual number of bytes that are data */
        count++; /* move to the starting of actual UCS2 data */
        /* get the actual length of the data after truncating 0xFF, the length include the null pointer byte */
        alpha->length = gstk_decode_h80_usc2_alphabet(orig_alpha_len, &orig_cmd_data[count], NULL);
        alpha->alphabet = GSTK_CALLOC(sizeof(uint16) * int32touint32(alpha->length));
        if (alpha->alphabet != NULL) {
          (void)gstk_decode_h80_usc2_alphabet(orig_alpha_len, &orig_cmd_data[count], alpha->alphabet);

          gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], orig_alpha_len);

          for (i = 0; i< alpha->length; i++) {
            UIM_MSG_HIGH_3(" Decoded alpha[%d]: 0x%x, %c",
                           i, alpha->alphabet[i], alpha->alphabet[i]);
          }
          count += orig_alpha_len;
        }
        else {
          /* memory allocation failed */
          return GSTK_MEMORY_ERROR;
        }
        break;
      case 0x81:
        orig_alpha_len--; /* the actual number of bytes that are data */
        count++; /* move to the starting of actual UCS2 data */
        /* get the actual length of the data after truncating 0xFF, the length include the null pointer byte */
        alpha->length = gstk_decode_h81_usc2_alphabet(orig_alpha_len, &orig_cmd_data[count], NULL);
        alpha->alphabet = GSTK_CALLOC(sizeof(uint16) * int32touint32(alpha->length));
        if (alpha->alphabet != NULL) {
          (void)gstk_decode_h81_usc2_alphabet(orig_alpha_len, &orig_cmd_data[count], alpha->alphabet);

          gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], orig_alpha_len);

          for (i = 0; i< alpha->length; i++) {
            UIM_MSG_HIGH_3("Decoded alpha[%d]: 0x%x, %c",
                           i, alpha->alphabet[i], alpha->alphabet[i]);
          }
          count += orig_alpha_len;
        }
        else {
          /* memory allocation failed */
          return GSTK_MEMORY_ERROR;
        }
        break;
      case 0x82:
        orig_alpha_len--; /* the actual number of bytes that are data */
        count++; /* move to the starting of actual UCS2 data */
        /* get the actual length of the data after truncating 0xFF, the length include the null pointer byte */
        alpha->length = gstk_decode_h82_usc2_alphabet(orig_alpha_len, &orig_cmd_data[count], NULL);
        alpha->alphabet = GSTK_CALLOC(sizeof(uint16) * int32touint32(alpha->length));
        if (alpha->alphabet != NULL) {
          (void)gstk_decode_h82_usc2_alphabet(orig_alpha_len, &orig_cmd_data[count], alpha->alphabet);

          gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], orig_alpha_len);

          for (i = 0; i< alpha->length; i++) {
            UIM_MSG_HIGH_3("Decoded alpha[%d]: 0x%x, %c",
                           i, alpha->alphabet[i], alpha->alphabet[i]);
          }
          count += orig_alpha_len;
        }
        else {
          /* memory allocation failed */
          return GSTK_MEMORY_ERROR;
        }
        break;
      default: /* GSM 7 bit default with bit 8 == 0 */

        /* get the actual length of the data after truncating 0xFF, the length include the null pointer byte */
        alpha->length = gstk_convert_gsm_default_to_gstk_alphabet(orig_alpha_len, &orig_cmd_data[count], NULL);

        if (alpha->length > 0) {
          alpha->alphabet = GSTK_CALLOC(sizeof(uint16) * int32touint32(alpha->length));
          if (alpha->alphabet != NULL) {
            (void)gstk_convert_gsm_default_to_gstk_alphabet(orig_alpha_len, &orig_cmd_data[count], alpha->alphabet);

            gstk_util_dump_byte_array("Raw Data", &orig_cmd_data[count], orig_alpha_len);

            for (i = 0; i< alpha->length; i++) {
              UIM_MSG_HIGH_3("Decoded alpha[%d]: 0x%x, %c",
                             i, alpha->alphabet[i], alpha->alphabet[i]);
            }
            count += orig_alpha_len;
          }
          else {
            /* memory allocation failed */
            return GSTK_MEMORY_ERROR;
          }
        }
        break;
      } /* switch on coding type */
    }
    /* if alpha->length == 0, application should use the default url */

    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
      if (alpha->alphabet != NULL) {
        gstk_free(alpha->alphabet);
        alpha->alphabet = NULL;
      }
      alpha->alphabet_given = FALSE;
      alpha->length = 0;
    }
    return gstk_status;

} /* gstk_parser_alpha_tlv_ext */


/*===========================================================================
FUNCTION gstk_parser_icon_id_tlv_ext

DESCRIPTION
    This function parses the icon ID TLV, and populate the gstk_icon_type
    passed in the function call with the info in the TLV.
    It sets the in_use boolean variable in the gstk_icon_type if there is
    valid icon info.
    Based on the record number, it performs a gsdi read on the EF-IMG
    file to get the width, height info of the image.  In additional, it
    goes to the specific image instance file as specified in the
    ER-IMG to retrieve the actual image and size for the image.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE


PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  icon: [Input/Output] Pointer to gstk_icon_type structure for populating the
                       parsed icon info
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_icon_id_tlv_ext(
        const gstk_alphabet_type * alpha_tlv,
        uint8                    * orig_cmd_data,
        int32                      cmd_data_len,
        int32                      orig_offset,
        gstk_icon_type            * icon,
        int32                     * next_tlv_offset,
        boolean                   * more_tlv,
        boolean                     man_opt_specifier,
        const uint32              * ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    uint8                           rec_num                = 0;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;
    uint32                          index                  = 0;

    UIM_MSG_HIGH_0("** Parsing Icon Ext");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to true */
    *more_tlv = TRUE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
    }
    if (orig_cmd_data == NULL) {
      return GSTK_SUCCESS;
    }


    if (icon == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    if (alpha_tlv == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    if (ref_id != NULL) {
      index = GSTK_CMD_REF_TABLE_INDEX(*ref_id);
    }

    /* initialize icon */
    memset(icon, 0x00, sizeof(gstk_icon_type));

    /* Icon TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (int)GSTK_ICON_IDENTIFIER_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_ICON_IDENTIFIER_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid Icon Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if ( tlv_length < GSTK_ICON_ID_LEN ) {
      UIM_MSG_ERR_1("Invalid icon id len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      count = count + length_of_length_field;

      icon->present = TRUE;

      /* icon qualifier */
      icon->qualifier = orig_cmd_data[count++];

      /* get heigth, width, size, data and image from the EF-IMG file */
      /* record number in the EF-IMG */
      rec_num = orig_cmd_data[count++];
      if (rec_num == 0) {
        icon->present = FALSE;
        if (tag_comprehension_req){
          UIM_MSG_ERR_0("Invalid rec num = 0");
          return GSTK_INVALID_COMMAND;
        }
        /* update the cmd_table info to indicate there was a parsing issue */
        if (ref_id != NULL) {
          command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
        }
      }
      else {
        if (((alpha_tlv->alphabet_given) && (alpha_tlv->length <= 1)) ||
            (!alpha_tlv->alphabet)) {
          if (tag_comprehension_req) {
            UIM_MSG_ERR_0("Text string was null");
            return GSTK_INVALID_COMMAND;
          }
          icon->present = FALSE;
          /* update the cmd_table info to indicate there was a parsing issue */
          if (ref_id != NULL) {
            command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
          }
        }
        icon->rec_num = rec_num;
        /* Now Populate the Icon Information */
        gstk_status = gstk_parse_img_record(rec_num,icon);
        if (gstk_status != GSTK_SUCCESS) {
          if (!tag_comprehension_req) {
            gstk_status = GSTK_SUCCESS;
            icon->present = FALSE;
            /* update the cmd_table info to indicate there was a parsing issue */
            if (ref_id != NULL) {
              command_details_ref_table_ptr[index].partial_comprehension |= TRUE;
            }
          }
        }
      }
    }

    /* next TLV offset */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    if (gstk_status == GSTK_SUCCESS) {
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }
    else {
      (void)gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }

    if (gstk_status != GSTK_SUCCESS) {
      icon->rec_num = 0;
      /* any mem free of clean up if required */
      if (icon->data != NULL) {
        gstk_free(icon->data);
        icon->data = NULL;
      }
      icon->present = FALSE;
    }

    return gstk_status;

} /*gstk_parser_icon_id_tlv_ext */

/*===========================================================================
FUNCTION gstk_parser_other_address_tlv

DESCRIPTION
    This function parses the other address TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.  The destination parameter is in the gstk_pkt_data_address_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  other_address: [Input/Output] Pointer to the url of gstk_pkt_data_address_type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_other_address_tlv(
                       uint8                         *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_pkt_data_address_type    *other_address,
                       int32                         *next_tlv_offset,
                       boolean                       *more_tlv,
                       boolean                        man_opt_specifier,
                       const uint32                  *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("** Parsing other address");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
      else
        return GSTK_SUCCESS;
    }

    if (other_address == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* Initialize other address */
    other_address->address_type = GSTK_PKT_DATA_NO_ADDRESS_GIVEN;
    other_address->pkt_address.length = 0;

    /* other address TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (int)GSTK_OTHER_ADDRESS_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_OTHER_ADDRESS_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    /* valid other address Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* It is possible to have address length == 0 => in dynamic data address */
    /* address length = tlv length - 1 byte of adderss type */
    other_address->pkt_address.length = orig_cmd_data[count + length_of_length_field -1];
    if (other_address->pkt_address.length > 0) {
      other_address->pkt_address.length -= 1;
    }
    UIM_MSG_HIGH_1("Other Addr Length 0x%x",
                   other_address->pkt_address.length);

    /* position count to the address type location in the tlv */
    count = count + length_of_length_field;

    if (other_address->pkt_address.length > 0) {
      /* address type */
      UIM_MSG_HIGH_1("Address Type 0x%x", orig_cmd_data[count]);
      switch (orig_cmd_data[count]) {
      case 0x21:
        other_address->address_type = GSTK_PKT_DATA_IPV4_ADDRESS;
        break;
      case 0x57:
        other_address->address_type = GSTK_PKT_DATA_IPV6_ADDRESS;
        break;
      default:
        other_address->address_type = GSTK_PKT_DATA_DYNAMIC_ADDRESS;
        break;
      }
      count++;

      if (other_address->pkt_address.length > 0) {
        /* get the actual length of the data after truncating 0xFF, the length include the null pointer byte */
        other_address->pkt_address.address = GSTK_CALLOC(int32touint32(other_address->pkt_address.length));
        if (other_address->pkt_address.address != NULL) {
          (void)memscpy(other_address->pkt_address.address, int32touint32(other_address->pkt_address.length),
          	        &orig_cmd_data[count], int32touint32(other_address->pkt_address.length));

          gstk_util_dump_byte_array("Raw Data",
                                    other_address->pkt_address.address,
                                    other_address->pkt_address.length);

          count += other_address->pkt_address.length;
        }
        else {
          /* memory allocation failed */
          return GSTK_MEMORY_ERROR;
        }
      }
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
      if (other_address->pkt_address.address != NULL) {
        gstk_free(other_address->pkt_address.address);
        other_address->pkt_address.address = NULL;
      }
      other_address->pkt_address.length = 0;
      other_address->address_type = GSTK_PKT_DATA_NO_ADDRESS_GIVEN;
    }
    return gstk_status;

} /* gstk_parser_other_address_tlv */



/*===========================================================================
FUNCTION gstk_parser_sim_me_interface_transport_level_tlv

DESCRIPTION
    This function parses the SIM ME interface transport level TLV
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  transport_level: [Input/Output] Pointer to the sim transport level
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlv: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_sim_me_interface_transport_level_tlv(
                       uint8                                *orig_cmd_data,
                       int32                                 cmd_data_len,
                       int32                                 orig_offset,
                       gstk_pkt_data_transport_level_type   *transport_level,
                       int32                                *next_tlv_offset,
                       boolean                              *more_tlv,
                       boolean                               man_opt_specifier,
                       const uint32                         *ref_id )
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing sim me interface transport lvl");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
      else
        return GSTK_SUCCESS;
    }

    if (transport_level == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* Initialize transport level info */
    transport_level->transport_protocol = GSTK_PKT_DATA_NOT_PRESENT;
    transport_level->port_number = 0;

    /* sim me transport level TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid sim me transport level Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if ( tlv_length < GSTK_SIM_ME_INTERFACE_TRANSPORT_LVL_LEN ) {
      UIM_MSG_ERR_1("Invalid SIM/ME transport lvl len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      /* position count to the transport protocol type in the tlv */
      count = count + length_of_length_field;

     UIM_MSG_HIGH_1(" Transport Protocol Type 0x%x", orig_cmd_data[count]);
      /* transport protocol type */
      switch (orig_cmd_data[count]) {
        case 0x01:
          transport_level->transport_protocol = GSTK_PKT_DATA_UDP;
          break;
        case 0x02:
          transport_level->transport_protocol = GSTK_PKT_DATA_TCP;
          break;
        case 0x03:
          transport_level->transport_protocol = GSTK_PKT_DATA_TCP_UICC_SERVER_MODE;
          break;
        default:
          return GSTK_INVALID_COMMAND;
      }

      count++;

      /* port number */
      transport_level->port_number = (orig_cmd_data[count] << 0x08) | orig_cmd_data[count+1];

      UIM_MSG_HIGH_3("Raw Data 0x%x, 0x%x ::: Port Number 0x%x",
                     orig_cmd_data[count],
                     orig_cmd_data[count+1],
                     transport_level->port_number);

      count+=2;
    }

    /* next TLV offset */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
    }
    return gstk_status;

} /* gstk_parser_sim_me_interface_transport_level_tlv */

/*===========================================================================
FUNCTION gstk_parser_ch_data_len_tlv

DESCRIPTION
    This function parses the channel data length TLV
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  ch_data_len: [Input/Output] Pointer to the channel data length
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlv: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_ch_data_len_tlv(
                       uint8                                *orig_cmd_data,
                       int32                                 cmd_data_len,
                       int32                                 orig_offset,
                       int32                                *ch_data_len,
                       int32                                *next_tlv_offset,
                       boolean                              *more_tlv,
                       boolean                               man_opt_specifier,
                       const uint32                         *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           tlv_length             = 0;

    UIM_MSG_HIGH_0("** Parsing Channel Data Length");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
        *more_tlv = FALSE;
        if(man_opt_specifier == GSTK_MANDATORY)
        {
          return GSTK_INVALID_COMMAND;
        }
        else
          return GSTK_SUCCESS;
    }

    if (ch_data_len == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* channel data length TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_CHANNEL_DATA_LEN_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_CHANNEL_DATA_LEN_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid channel data length Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    tlv_length = orig_cmd_data[count + length_of_length_field -1];
    if ( tlv_length < GSTK_CHANNEL_DATA_LEN_LEN ) {
      UIM_MSG_ERR_1("Invalid ch data len len 0x%x", tlv_length);
      if (tag_comprehension_req){
        return GSTK_INVALID_LENGTH;
      }
      /* update the cmd_table info to indicate there was a parsing issue */
      if (ref_id != NULL) {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
      }
      count = count + length_of_length_field + tlv_length;
    }
    else {
      /* position count to the channel data length in the tlv */
      count = count + length_of_length_field;

      /* channel data length */
      *ch_data_len = orig_cmd_data[count];
      UIM_MSG_HIGH_1("Channel Data Length", *ch_data_len);

      count++;
    }

    /* next TLV offset */
    gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
    }
    return gstk_status;

} /* gstk_parser_ch_data_len_tlv */


/*===========================================================================
FUNCTION gstk_parser_ch_data_tlv

DESCRIPTION
    This function parses the channel data TLV
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  ch_data: [Input/Output] Pointer to the channel data
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlv: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_ch_data_tlv(
                       uint8                                *orig_cmd_data,
                       int32                                 cmd_data_len,
                       int32                                 orig_offset,
                       gstk_ch_data_type                    *ch_data,
                       int32                                *next_tlv_offset,
                       boolean                              *more_tlv,
                       boolean                               man_opt_specifier,
                       const uint32                         *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

    UIM_MSG_HIGH_0("** Parsing Channel Data");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
      return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
        *more_tlv = FALSE;
        if(man_opt_specifier == GSTK_MANDATORY)
        {
          return GSTK_INVALID_COMMAND;
        }
        else
          return GSTK_SUCCESS;
    }

    if (ch_data == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* channel data TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_CHANNEL_DATA_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_CHANNEL_DATA_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    /* valid channel data Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* position count to the channel data in the tlv */

    /* channel data */
    ch_data->data_len = orig_cmd_data[count + length_of_length_field - 1];

    /* position count to the channel data in the tlv */
    count = count + length_of_length_field;

    UIM_MSG_HIGH_1("Channel Data Length", ch_data->data_len);

    ch_data->data = GSTK_CALLOC(int32touint32(ch_data->data_len));
    if (ch_data->data == NULL) {
      /* memory allocation failed */
      return GSTK_MEMORY_ERROR;
    }

    /* copy the data */
    (void)memscpy(ch_data->data, int32touint32(ch_data->data_len), &orig_cmd_data[count], int32touint32(ch_data->data_len));

    count += ch_data->data_len;

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
      if (ch_data->data != NULL) {
        gstk_free(ch_data->data);
        ch_data->data = NULL;
      }
      ch_data->data_len = 0;
    }
    return gstk_status;

} /* gstk_parser_ch_data_tlv */

/*===========================================================================
FUNCTION gstk_parser_text_string_tlv_ext

DESCRIPTION
    This function parses the TLV that is coded according to ADN 23.038 format
    into gstk_alphabet_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  alpha: [Input/Output] Pointer to the string of gstk_alphabet_type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_text_string_tlv_ext(
                       tags_enum_type                 tag,
                       uint8                         *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_alphabet_type            *text_string,
                       int32                         *next_tlv_offset,
                       boolean                       *more_tlv,
                       boolean                        man_opt_specifier,
                       const uint32                  *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    int32                           orig_txt_len           = 0;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    boolean                         tag_comprehension_req  = TRUE;
    int32                           count_after_decode     = 0;

    UIM_MSG_HIGH_1("** Parsing 0x%x into GSTK_ALPHABET_TYPE", tag);

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
        return GSTK_INVALID_LENGTH;
    }

    if (orig_cmd_data == NULL) {
       *more_tlv = FALSE;
        if(man_opt_specifier == GSTK_MANDATORY)
        {
          return GSTK_INVALID_COMMAND;
        }
        else
          return GSTK_SUCCESS;
    }

    if (text_string == NULL) {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* Initialize text_string */
    text_string->alphabet_given = FALSE;
    text_string->length         = 0;

    /* check tag of the TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)tag,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(tag, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    text_string->alphabet_given = TRUE;

    /* valid alpha Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* The actual number of bytes that form the data */
    orig_txt_len = orig_cmd_data[count + length_of_length_field -1] - 1;

    /* position count to the text location in the tlv */
    count = count + length_of_length_field;

    if (orig_txt_len > 0)
    { /* Card provides a text string */
      text_string->length = orig_txt_len;

      /* decode the text string according to the packing format
      ** add null char at the end */
      count_after_decode = gstk_decode_text_string_body_ext(tag,
                                                            orig_cmd_data,
                                                            count, /* index location of the first byte of text */
                                                            text_string);

      /* calculate length based on raw TLV */
      count += (orig_txt_len+1); /* text len + dcs byte */

      if (count != count_after_decode)
      {
        UIM_MSG_ERR_0("length doesn't match for raw TLV");
        if (text_string->alphabet != NULL)
        {
          gstk_free(text_string->alphabet);
          text_string->alphabet = NULL;
          text_string->length = 0;
        }
        if (tag_comprehension_req)
        {
          return GSTK_INVALID_LENGTH;
        }
        else
        {
          if (ref_id != NULL)
          {
            command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
          }
        }
      }
    }/* if text_string->length == 0, application should use the default url */

    /* next TLV offset */
    if (gstk_status != GSTK_SUCCESS)
    {
      (void)gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }
    else
    {
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }

    if (gstk_status != GSTK_SUCCESS)
    {
      /* any mem free of clean up if required */
      if (text_string->alphabet != NULL)
      {
        gstk_free(text_string->alphabet);
        text_string->alphabet = NULL;
      }
      text_string->alphabet_given = FALSE;
    }
    return gstk_status;

} /* gstk_parser_text_string_tlv_ext */

/*===========================================================================
FUNCTION gstk_decode_ussd_string_body

DESCRIPTION
    Stores decoded textual string in global text_string.text
    Used in text_string.length ^ text_string.data_coding_scheme

DEPENDENCIES
    If default is TRUE use default text, else use text_string

RETURN VALUE
    Updated Counter Value

===========================================================================*/
static uint8 gstk_decode_ussd_string_body(
  const uint8           *command_data,
  int32                  offset,
  gstk_ussd_string_type *text_string)
{
    uint8              dcs = 0;
    int32              text_length = 0;
    int32              new_count = 0;
    int                additional_int = 0;
    int                additional_mod = 0;
    gstk_dcs_enum_type dcs_enum;

    new_count = offset;

    if((!text_string) || (!command_data))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return int32touint8(new_count);
    }

    dcs = text_string->dcs;

    text_length = text_string->length; /*  length before decoding */
    UIM_MSG_HIGH_2("In gstk_decode_ussd_string_body() dcs = %x, text_length = %x",
                   dcs, text_length);

    /* USSD string => Cell Broadcast DCS */
    dcs_enum = gstk_util_decode_dcs_type(TRUE, dcs);

    switch(dcs_enum) {
    case GSTK_DCS_7_BIT:
      gstk_util_dump_byte_array(" == command_data", &command_data[new_count], text_length);

      if (text_length > 223) {
        /* will exceed 256 after decoding, send to application so that it can decode */
        text_string->text = GSTK_CALLOC(int32touint32(text_length) * sizeof(uint8));
        if (text_string->text == NULL)
        {
          return 0;
        }
        memset(text_string->text, 0, int32touint32(text_length));
        (void)memscpy(text_string->text, int32touint32(text_length),
        	      &(command_data[new_count]), int32touint32(text_length));

        new_count += text_length;
      }
      else {
        /* allocate the memory */
        additional_int = text_length / 7;
        additional_mod = text_length % 7;
        if (additional_mod > 0) {
          additional_mod = 1;
        }
        text_string->text = GSTK_CALLOC(
            int32touint32(text_length + additional_mod + additional_int) * sizeof(uint8));

        if (text_string->text == NULL)
        {
          return 0;
        }

        memset(text_string->text, 0x00, int32touint32(text_length + additional_mod + additional_int));

        new_count += text_length;

        /* Convert to 8-bit text */
        text_length =  gstk_util_unpack_7_bit_chars(
          &command_data[new_count-text_length],
          int32touint8(text_length),
          text_string->text );

        text_string->dcs = text_string->dcs | 0x04;
        dcs_enum = gstk_util_decode_dcs_type(TRUE, text_string->dcs);

        if (dcs_enum == GSTK_DCS_7_BIT) {
          /* default to data coding 8 bit = 0xF4 */
          text_string->dcs = 0xF4;
        }
      }

      break;

    case GSTK_DCS_8_BIT:
    case GSTK_DCS_UCS2_8_BIT:
    case GSTK_DCS_UCS2_7_BIT:
      /* Unpacked */
      /* Expanded default alphabet */
      text_string->text = GSTK_CALLOC(int32touint32(text_length) * sizeof(uint8));
      if (text_string->text == NULL)
      {
        return 0;
      }
      memset(text_string->text, 0, int32touint32(text_length));
      (void)memscpy(text_string->text, int32touint32(text_length),
      	       &(command_data[new_count]), int32touint32(text_length));

      new_count += text_length;

      /* new_count does not need to increase since it represents the raw TLV data count,
      ** which is not affected by unpacking or null char at the end of the text string */
      break;

    default:
      UIM_MSG_ERR_1("unable to determine dcs 0x%x", dcs);
      return 0;
    }

    gstk_util_dump_byte_array("TEXT", text_string->text, text_length);

    if(text_length != text_string->length)
    {
       text_string->length = int32touint8(text_length);
    }
    return int32touint8(new_count);
} /*gstk_decode_ussd_string_body*/

/*===========================================================================
FUNCTION gstk_parser_ussd_string_tlv

DESCRIPTION
    This function parses the ussd string TLV and
    populate the gstk_string_type passed in the function call with the info
    in the TLV.
    It sets the DCS, length of text data and data.  Based on the DCS values,
    the data has been decode accordingly.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more_tlv will be set to FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  ussd_string: [Input/Output] Pointer to gstk_ussd_string_type structure for populating
                              the parsed text string info
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_ussd_string_tlv(
                       uint8                  *orig_cmd_data,
                       int32                   cmd_data_len,
                       int32                   orig_offset,
                       gstk_ussd_string_type  *ussd_string,
                       int32                  *next_tlv_offset,
                       boolean                *more_tlvs,
                       boolean                 man_opt_specifier,
                       const uint32           *ref_id)
{

    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    int32                           text_len_before_decode = 0;
    int32                           count_after_decode     = 0;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    boolean                         tag_comprehension_req  = TRUE;

    UIM_MSG_HIGH_0("** Parsing USSD");

    if((!ussd_string) || (!next_tlv_offset) || (!more_tlvs))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }
    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    /* has to have at least tag and length field */

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len, 0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    count = orig_offset;

    /* TEXT String TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_USSD_STRING_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_USSD_STRING_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
        tag_comprehension_req = FALSE;
    }

    /* valid USSD String Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /* check for length value */
    if(orig_cmd_data[count+length_of_length_field-1] == 0) {
        /* empty tlv */
        ussd_string->orig_dcs_from_sim = 0;
        ussd_string->dcs = 0;
        ussd_string->length = 0;
        count = count + length_of_length_field;
    }
    else { /* at least 1 length */
        if(orig_cmd_data[count+length_of_length_field-1] == 1) {
          /* only have dcs field */
          ussd_string->length = 0;
        }
        else { /* has text */
          /* length of string */
          /* length in the Text String tlv includes length of text + DCS, thus,
          ** needs to -1 from the length obtained to get the actual length of the
          ** text string */
          ussd_string->length = orig_cmd_data[count+length_of_length_field-1] - 1;
        }
        count = count + length_of_length_field;

        /* DCS */
        ussd_string->orig_dcs_from_sim = orig_cmd_data[count++];
        ussd_string->dcs = ussd_string->orig_dcs_from_sim;
    }

    /* Text */
    if (ussd_string->length > 0) {

        text_len_before_decode = ussd_string->length;
        /* decode the text string according to the packing format
        ** add null char at the end */
        count_after_decode = gstk_decode_ussd_string_body(orig_cmd_data,
                                                          count, /* index location of the first byte of text */
                                                          ussd_string);

        /* calculate length based on raw TLV */
        count += text_len_before_decode;

        if (count != count_after_decode) {
            UIM_MSG_ERR_0("length doesn't match for raw TLV");
            if (ussd_string->text != NULL) {
              gstk_free(ussd_string->text);
              ussd_string->text = NULL;
            }
            if (tag_comprehension_req){
              return GSTK_INVALID_COMMAND;
            }
            else {
              /* update the cmd_table info to indicate there was a parsing issue */
              if (ref_id != NULL) {
                command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
              }
            }
        }
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
    if (gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                      cmd_data_len, count);
        if (ussd_string->text != NULL) {
          gstk_free(ussd_string->text);
          ussd_string->text = NULL;
        }
        ussd_string->length = 0;
        return GSTK_INVALID_LENGTH;
    }

    return GSTK_SUCCESS;
} /*gstk_parser_ussd_string_tlv*/


/*===========================================================================
FUNCTION gstk_parser_network_access_name_tlv

DESCRIPTION
    This function parses the TLV that is coded according to 23.003 format
    into gstk_alphabet_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  nw_access_name: [Input/Output] Pointer to the string of gstk_alphabet_type,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_network_access_name_tlv(
                       uint8                         *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_alphabet_type            *nw_access_name,
                       int32                         *next_tlv_offset,
                       boolean                       *more_tlv,
                       boolean                        man_opt_specifier,
                       const uint32                  *ref_id)
{
    int32                           count                      = 0;
    uint32                          length_of_length_field     = 0;
    gstk_status_enum_type           gstk_status                = GSTK_SUCCESS;
    boolean                         tag_comprehension_req      = TRUE;
    int                             alpha_len                  = 0;
    int                             nLen                       = 0;
    int                             apn_val                    = 0;
    int                             iter                       = 0;
    gstk_parsing_result_enum_type   tag_parsing_result         = GSTK_TLV_PARSING_CONTINUE;
    int                             tlv_length                 = 0;

    UIM_MSG_HIGH_0("** Parsing Network access name into GSTK_ALPHABET_TYPE");

    if((!next_tlv_offset) || (!more_tlv))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* initialize more tlvs to false */
    *more_tlv = FALSE;

    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len, 0))
    {
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlv = FALSE;
      if(man_opt_specifier == GSTK_MANDATORY)
      {
        return GSTK_INVALID_COMMAND;
      }
      else
        return GSTK_SUCCESS;
    }

    if (nw_access_name == NULL)
    {
        UIM_MSG_ERR_0("Null Input Param");
        return GSTK_MEMORY_ERROR;
    }

    /* Initialize Count */
    count = orig_offset;

    /* Initialize text_string */
    nw_access_name->alphabet_given = FALSE;
    nw_access_name->length         = 0;

    /* check tag of the TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (int)GSTK_NETWORK_ACCESS_NAME_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_NETWORK_ACCESS_NAME_TAG,
                               tag_parsing_result,
                               man_opt_specifier,
                               &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count]))
    {
      tag_comprehension_req = FALSE;
    }

    nw_access_name->alphabet_given = TRUE;

    /* valid network access name Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    UIM_MSG_LOW_2("COUNT: %x Len: %x", count, length_of_length_field);

    /* The actual number of bytes that form the data */
    /* The length field of each label accounts for the space needed for a "."
       The last label's length field will be used for a NULL byte
    */
    nw_access_name->length = orig_cmd_data[count + length_of_length_field -1];
    tlv_length = nw_access_name->length;

    /* position count to the text location in the tlv */
    count = count + length_of_length_field;

    /* Card provides the network access name
       APN consists of :
       mandatory - APN Network Identifier
       optional  - APN Operator Identifier
    */
    if (nw_access_name->length < GSTK_MIN_APN_LEN + 1)
    {
      UIM_MSG_ERR_1("Insufficient length, 0x%x", nw_access_name->length-1);
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
      if(tag_comprehension_req)
      {
        return GSTK_ERROR;
      }
      else
      {
        /* next TLV offset */
        gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
        return gstk_status;
      }
    }

    /* Check for the maximum octets possible in an APN
    */
    if (nw_access_name->length > GSTK_MAX_APN_LEN)
    {
      UIM_MSG_ERR_0("Maximum length is 100 octets");
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
      if(tag_comprehension_req)
      {
        return GSTK_ERROR;
      }
      else
      {
        /* next TLV offset */
        gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
        return gstk_status;
      }
    }
    gstk_status = gstk_util_find_apn_len(&orig_cmd_data[count], nw_access_name->length, &nLen);

    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Null bytes ! ");
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
      if(tag_comprehension_req)
      {
        return GSTK_ERROR;
      }
      else
      {
        /* next TLV offset */
        gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
        return gstk_status;
      }
    }

    /* Allocate memory for the APN */
    nw_access_name->alphabet = (uint16*)GSTK_CALLOC(int32touint32(nLen)*sizeof(uint16));
    if (nw_access_name->alphabet == NULL)
    {
      nw_access_name->alphabet_given = FALSE;
      nw_access_name->length = 0;
      return GSTK_MEMORY_ERROR;
    }
    memset(nw_access_name->alphabet, 0x00, int32touint32(nLen) * sizeof(uint16));

    alpha_len = 0;

    /* Find out if the label is an APN Network ID or APN Operator ID */
    gstk_status = gstk_util_check_for_apn_net_op_id((char *)&orig_cmd_data[count],
                                                    nw_access_name->length,
                                                    &apn_val);

    if(apn_val != GSTK_APN_NET_ID)
    {
      UIM_MSG_ERR_0("First octet shoud be an APN Network Identifier");
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
      gstk_free(nw_access_name->alphabet);
      nw_access_name->alphabet = NULL;
      if(tag_comprehension_req)
      {
        return GSTK_ERROR;
      }
      else
      {
        /* next TLV offset */
        gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
        return gstk_status;
      }
    }

    gstk_status = gstk_util_extract_label_len_value(GSTK_APN_NET_ID, &orig_cmd_data[count],
                                                    nw_access_name->alphabet, &alpha_len,
                                                    &count, &iter);

    if(gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("First octet shoud be an APN Network Identifier");
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
      gstk_free(nw_access_name->alphabet);
      nw_access_name->alphabet = NULL;
      if(tag_comprehension_req)
      {
        return GSTK_ERROR;
      }
      else
      {
        /* next TLV offset */
        gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
        return gstk_status;
      }
    }

    while(iter < nw_access_name->length)
    {
      /* Find out is the label is an APN Network ID or APN Operator ID */
      gstk_status = gstk_util_check_for_apn_net_op_id((char *)&orig_cmd_data[count],
                                                      nw_access_name->length,
                                                      &apn_val);

      if(gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Neither a APN NETWORK ID NOR A OP ID");
        nw_access_name->length = 0;
        nw_access_name->alphabet_given = FALSE;
        gstk_free(nw_access_name->alphabet);
        nw_access_name->alphabet = NULL;
        if(tag_comprehension_req)
        {
          return GSTK_ERROR;
        }
        else
        {
          /* next TLV offset */
          gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
          gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
          return gstk_status;
        }
      }

      gstk_status = gstk_util_extract_label_len_value(apn_val, &orig_cmd_data[count],
                                                      nw_access_name->alphabet,
                                                      &alpha_len,
                                                      &count,
                                                      &iter);

      if(gstk_status != GSTK_SUCCESS)
      {
        UIM_MSG_ERR_0("Neither a Network APN or Operator APN ");
        nw_access_name->length = 0;
        nw_access_name->alphabet_given = FALSE;
        gstk_free(nw_access_name->alphabet);
        nw_access_name->alphabet = NULL;
        if(tag_comprehension_req)
        {
          return GSTK_ERROR;
        }
        else
        {
          /* next TLV offset */
          gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);
          gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
          return gstk_status;
        }
      }
    }
    /* The APN needs to be null terminated according to the specs */

    if((alpha_len != nLen) || (nw_access_name->length != nLen))
    {
      UIM_MSG_HIGH_0("Error in parsing APN");
      /* any mem free of clean up if required */
      if (nw_access_name->alphabet != NULL)
      {
        gstk_free(nw_access_name->alphabet);
        nw_access_name->alphabet = NULL;
      }
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
    }
    else
    {
      nw_access_name->alphabet[alpha_len-1] = 0x00;
      nw_access_name->length = nLen;
    }

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

    if (gstk_status != GSTK_SUCCESS) {
      /* any mem free of clean up if required */
      if (nw_access_name->alphabet != NULL) {
        gstk_free(nw_access_name->alphabet);
        nw_access_name->alphabet = NULL;
      }
      nw_access_name->length = 0;
      nw_access_name->alphabet_given = FALSE;
    }
    return gstk_status;
} /* gstk_parser_network_access_name_tlv */

/*lint -e715 orig_cmd_data_ptr not being used */
/*lint -e818 orig_cmd_data_ptr not const */
/*lint -e715 cmd_data_len not being used */
/*lint -e715 orig_offset not being used */
/*lint -e715 pdp_context_act_param_ptr not being used */
/*lint -e818 pdp_context_act_param_ptr not const */
/*lint -e715 more_tlv_ptr not being used */
/*lint -e818 more_tlv_ptr not const */
/*lint -e715 man_opt_specifier not being used */
/*lint -e715 ref_id_ptr not being used */
/*lint -e715 next_tlv_offset_ptr not being used */
/*lint -e818 next_tlv_offset_ptr not const */
/*===========================================================================
FUNCTION gstk_parser_pdp_context_act_param_tlv

DESCRIPTION
    This function parses the TLV that is coded as the
    ACTIVATE PDP CONTEXT REQUEST message according to 24.008
    into gstk_pdp_context_act_param_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data_ptr:         [Input] Pointer to the original command
                             data to be parse
  cmd_data_len:              [Input] Valid length of the original command
                             data pointer
  orig_offset:               [Input] Offset in the original command data
                             where the parsing should begin
  pdp_context_act_param_ptr: [Input/Output] Pointer to the string of
                             gstk_pdp_context_act_param_type
  next_tlv_offset_ptr:       [Input/Output] Pointer to the index
                             offset for the next tlv if applicable.
  more_tlvs_ptr:             [Input/Output] Pointer to boolean to
                             indicate whether there are more
                             TLV to be parse after exiting this function
  man_opt_specifier:         [Input] Mandatory or Optional Parsing required
  ref_id_ptr:                [Input] Pointer containing index to the command
                             detail reference global table

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_pdp_context_act_param_tlv(
  uint8                                *orig_cmd_data_ptr,
  int32                                 cmd_data_len,
  int32                                 orig_offset,
  gstk_pdp_context_act_param_type      *pdp_context_act_param_ptr,
  int32                                *next_tlv_offset_ptr,
  boolean                              *more_tlv_ptr,
  boolean                               man_opt_specifier,
  const uint32                         *ref_id_ptr)
{
#ifdef FEATURE_GPRS_CALLCONTROL
  int32                           count                  = 0;
  uint32                          length_of_length_field = 0;
  gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
  gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

  UIM_MSG_HIGH_0("** Parsing PDP Conext Activation Parameter");

  if((!next_tlv_offset_ptr) || (!more_tlv_ptr))
  {
    UIM_MSG_ERR_2("NULL PTR: next tlv offset 0x%x, more tlv 0x%x",
                  next_tlv_offset_ptr, more_tlv_ptr);
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to false */
  *more_tlv_ptr = FALSE;

  if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
    return GSTK_INVALID_LENGTH;
  }

  if (orig_cmd_data_ptr == NULL) {
    *more_tlv_ptr = FALSE;
    if(man_opt_specifier == GSTK_MANDATORY)
    {
      return GSTK_INVALID_COMMAND;
    }
    else
      return GSTK_SUCCESS;
  }

  if (pdp_context_act_param_ptr == NULL) {
    UIM_MSG_ERR_0("Null Input Param: pdp context act param");
    return GSTK_MEMORY_ERROR;
  }

  /* Initialize Count */
  count = orig_offset;

  /* channel data TLV */
  tag_parsing_result = gstk_is_tag_valid(ref_id_ptr,
                                         orig_cmd_data_ptr[count],
                                        (uint8)GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG,
                                         man_opt_specifier,
                                         orig_cmd_data_ptr,
                                         &count,
                                         cmd_data_len);

  if (!gstk_continue_parsing(GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG,
                             tag_parsing_result,
                             man_opt_specifier,
                             &gstk_status))
  {
    if (gstk_status == GSTK_SUCCESS) {
      /* Was optional tag, go through the parser_get_next_tlv_offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                    count,
                                                    next_tlv_offset_ptr,
                                                    more_tlv_ptr);
    }
    return gstk_status;
  }

  /* valid channel data Tag */
  count++;

  length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data_ptr[count]);

  /* pdp context activation parm length */
  pdp_context_act_param_ptr->length = orig_cmd_data_ptr[count + length_of_length_field - 1];

  /* position count to the pdp context activation parm in the tlv */
  count = count + length_of_length_field;

  UIM_MSG_HIGH_1("PDP Context Act Param Length 0x%x",
                 pdp_context_act_param_ptr->length);

  /* copy the data */
  (void)memscpy(pdp_context_act_param_ptr->pdp_act_param,
         sizeof(pdp_context_act_param_ptr->pdp_act_param),
         &orig_cmd_data_ptr[count],
         int32touint32(pdp_context_act_param_ptr->length));

  count += pdp_context_act_param_ptr->length;

  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                count,
                                                next_tlv_offset_ptr,
                                                more_tlv_ptr);

  if (gstk_status != GSTK_SUCCESS) {
    pdp_context_act_param_ptr->length = 0;
  }
  return gstk_status;
#else
  UIM_MSG_ERR_0("Feature GPRS Call control not supported");
  return GSTK_ERROR;
#endif /* #ifdef FEATURE_GPRS_CALLCONTROL */
}
/*lint +e715 orig_cmd_data_ptr not being used */
/*lint +e818 orig_cmd_data_ptr not const */
/*lint +e715 cmd_data_len not being used */
/*lint +e715 orig_offset not being used */
/*lint +e715 pdp_context_act_param_ptr not being used */
/*lint +e818 pdp_context_act_param_ptr not const */
/*lint +e715 more_tlv_ptr not being used */
/*lint +e818 more_tlv_ptr not const */
/*lint +e715 man_opt_specifier not being used */
/*lint +e715 ref_id_ptr not being used */
/*lint +e715 next_tlv_offset_ptr not being used */
/*lint +e818 next_tlv_offset_ptr not const */

/*===========================================================================
FUNCTION gstk_parser_pdn_conn_act_param_tlv

DESCRIPTION
    This function parses the TLV that is coded as the
    PDN CONNECTION REQUEST message according to 24.310
    into gstk_eps_pdn_conn_act_param_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data_ptr:         [Input] Pointer to the original command
                             data to be parse
  cmd_data_len:              [Input] Valid length of the original command
                             data pointer
  orig_offset:               [Input] Offset in the original command data
                             where the parsing should begin
  pdn_conn_act_param_ptr:    [Input/Output] Pointer to the string of
                             gstk_eps_pdn_conn_act_param_type
  next_tlv_offset_ptr:       [Input/Output] Pointer to the index
                             offset for the next tlv if applicable.
  more_tlvs_ptr:             [Input/Output] Pointer to boolean to
                             indicate whether there are more
                             TLV to be parse after exiting this function
  man_opt_specifier:         [Input] Mandatory or Optional Parsing required
  ref_id_ptr:                [Input] Pointer containing index to the command
                             detail reference global table

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_pdn_conn_act_param_tlv(
  uint8                                *orig_cmd_data_ptr,
  int32                                 cmd_data_len,
  int32                                 orig_offset,
  gstk_eps_pdn_conn_act_param_type     *pdn_conn_act_param_ptr,
  int32                                *next_tlv_offset_ptr,
  boolean                              *more_tlv_ptr,
  boolean                               man_opt_specifier,
  const uint32                         *ref_id_ptr)
{
#ifdef FEATURE_GPRS_CALLCONTROL
  int32                           count                  = 0;
  uint32                          length_of_length_field = 0;
  gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
  gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

  UIM_MSG_HIGH_0("** Parsing PDN CONN REQUEST Parameter");

  if((!next_tlv_offset_ptr) || (!more_tlv_ptr))
  {
    UIM_MSG_ERR_2("NULL PTR: next tlv offset 0x%x, more tlv 0x%x",
                  next_tlv_offset_ptr, more_tlv_ptr);
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to false */
  *more_tlv_ptr = FALSE;

  if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
    return GSTK_INVALID_LENGTH;
  }

  if (orig_cmd_data_ptr == NULL) {
    *more_tlv_ptr = FALSE;
    if(man_opt_specifier == GSTK_MANDATORY)
    {
      return GSTK_INVALID_COMMAND;
    }
    else
      return GSTK_SUCCESS;
  }

  if (pdn_conn_act_param_ptr == NULL) {
    UIM_MSG_ERR_0("Null Input Param: pdn_conn_act_param_ptr");
    return GSTK_MEMORY_ERROR;
  }

  /* Initialize Count */
  count = orig_offset;

  /* channel data TLV */
  tag_parsing_result = gstk_is_tag_valid(ref_id_ptr,
                                         orig_cmd_data_ptr[count],
                                        (uint8)GSTK_EPS_PDN_CONN_PARAM_TAG,
                                         man_opt_specifier,
                                         orig_cmd_data_ptr,
                                         &count,
                                         cmd_data_len);

  if (!gstk_continue_parsing(GSTK_EPS_PDN_CONN_PARAM_TAG,
                             tag_parsing_result,
                             man_opt_specifier,
                             &gstk_status))
  {
    if (gstk_status == GSTK_SUCCESS) {
      /* Was optional tag, go through the parser_get_next_tlv_offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                    count,
                                                    next_tlv_offset_ptr,
                                                    more_tlv_ptr);
    }
    return gstk_status;
  }

  /* valid channel data Tag */
  count++;

  length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data_ptr[count]);

  /* pdp context activation parm length */
  pdn_conn_act_param_ptr->length = orig_cmd_data_ptr[count + length_of_length_field - 1];

  /* position count to the pdp context activation parm in the tlv */
  count = count + length_of_length_field;

  UIM_MSG_HIGH_1("PDN Conn Act Param Length 0x%x",
                 pdn_conn_act_param_ptr->length);

  /* copy the data */
  (void)memscpy(pdn_conn_act_param_ptr->eps_act_param,
         sizeof(pdn_conn_act_param_ptr->eps_act_param),
         &orig_cmd_data_ptr[count],
         int32touint32(pdn_conn_act_param_ptr->length));

  count += pdn_conn_act_param_ptr->length;

  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                count,
                                                next_tlv_offset_ptr,
                                                more_tlv_ptr);

  if (gstk_status != GSTK_SUCCESS) {
    pdn_conn_act_param_ptr->length = 0;
  }
  return gstk_status;
#else
  UIM_MSG_ERR_0("Feature GPRS Call control not supported");
  return GSTK_ERROR;
#endif /* #ifdef FEATURE_GPRS_CALLCONTROL */
} /* gstk_parser_pdn_conn_act_param_tlv */

/*===========================================================================
FUNCTION gstk_parser_ims_call_param_tlv

DESCRIPTION
    This function parses the TLV that is coded as the
    IMS URI according to rfc3261
    into gstk_ims_call_param_type format
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data_ptr:         [Input] Pointer to the original command
                             data to be parse
  cmd_data_len:              [Input] Valid length of the original command
                             data pointer
  orig_offset:               [Input] Offset in the original command data
                             where the parsing should begin
  ims_call_param_ptr:        [Input/Output] Pointer to the string of
                             gstk_eps_pdn_conn_act_param_type
  next_tlv_offset_ptr:       [Input/Output] Pointer to the index
                             offset for the next tlv if applicable.
  more_tlvs_ptr:             [Input/Output] Pointer to boolean to
                             indicate whether there are more
                             TLV to be parse after exiting this function
  man_opt_specifier:         [Input] Mandatory or Optional Parsing required
  ref_id_ptr:                [Input] Pointer containing index to the command
                             detail reference global table

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_ims_call_param_tlv(
  uint8                                *orig_cmd_data_ptr,
  int32                                 cmd_data_len,
  int32                                 orig_offset,
  gstk_ims_call_param_type             *ims_call_param_ptr,
  int32                                *next_tlv_offset_ptr,
  boolean                              *more_tlv_ptr,
  boolean                               man_opt_specifier,
  const uint32                         *ref_id_ptr)
{
  int32                           count                  = 0;
  uint32                          length_of_length_field = 0;
  gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
  gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

  UIM_MSG_HIGH_0("** Parsing IMS REQUEST URI Parameter");

  if((!next_tlv_offset_ptr) || (!more_tlv_ptr))
  {
    UIM_MSG_ERR_2("NULL PTR: next tlv offset 0x%x, more tlv 0x%x",
      next_tlv_offset_ptr, more_tlv_ptr);
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to false */
  *more_tlv_ptr = FALSE;

  if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
    return GSTK_INVALID_LENGTH;
  }

  if (orig_cmd_data_ptr == NULL) {
    *more_tlv_ptr = FALSE;
    if(man_opt_specifier == GSTK_MANDATORY)
    {
      return GSTK_INVALID_COMMAND;
    }
    else
      return GSTK_SUCCESS;
  }

  if (ims_call_param_ptr == NULL) {
    UIM_MSG_ERR_0("Null Input Param: ims_call_param_ptrr");
    return GSTK_MEMORY_ERROR;
  }

  /* Initialize Count */
  count = orig_offset;

  /* channel data TLV */
  tag_parsing_result = gstk_is_tag_valid(ref_id_ptr,
                                         orig_cmd_data_ptr[count],
                                        (uint8)GSTK_IMS_URI_TAG,
                                         man_opt_specifier,
                                         orig_cmd_data_ptr,
                                         &count,
                                         cmd_data_len);

  if (!gstk_continue_parsing(GSTK_IMS_URI_TAG,
                             tag_parsing_result,
                             man_opt_specifier,
                             &gstk_status))
  {
    if (gstk_status == GSTK_SUCCESS) {
      /* Was optional tag, go through the parser_get_next_tlv_offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                    count,
                                                    next_tlv_offset_ptr,
                                                    more_tlv_ptr);
    }
    return gstk_status;
  }

  /* valid channel data Tag */
  count++;

  length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data_ptr[count]);

  /* ims call param length */
  ims_call_param_ptr->length = orig_cmd_data_ptr[count + length_of_length_field - 1];

  /* position count to the ims call parm in the tlv */
  count = count + length_of_length_field;

  UIM_MSG_HIGH_1("IMS CALL Param Length 0x%x", ims_call_param_ptr->length);

  memset(ims_call_param_ptr->ims_uri, 0, sizeof(ims_call_param_ptr->ims_uri));

  /* copy the data */
  (void)memscpy(ims_call_param_ptr->ims_uri,
         sizeof(ims_call_param_ptr->ims_uri),
         &orig_cmd_data_ptr[count],
         int32touint32(ims_call_param_ptr->length));

  count += ims_call_param_ptr->length;

  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                count,
                                                next_tlv_offset_ptr,
                                                more_tlv_ptr);

  if (gstk_status != GSTK_SUCCESS) {
    ims_call_param_ptr->length = 0;
  }
  return gstk_status;
}

/*===========================================================================
FUNCTION gstk_parser_aid_tlv

DESCRIPTION
    This function parses the aid information provided as optional
    data in commands like REFRESH.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

PARAMETERS
  orig_cmd_data: [Input] Pointer to the original command data to be parse
  cmd_data_len: [Input] Valid length of the original command data pointer
  orig_offset: [Input] Offset in the original command data where the parsing
                       should begin
  aid: [Input/Output] Pointer to aid buffer,
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are more
                            TLV to be parse after exiting this function
DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_aid_tlv(
                       uint8                          *orig_cmd_data,
                       int32                          cmd_data_len,
                       int32                          orig_offset,
                       gstk_app_id_type               *aid,
                       int32                          *next_tlv_offset,
                       boolean                        *more_tlvs,
                       boolean                         man_opt_specifier,
                       const uint32                   *ref_id)
{
    int32                           count                  = 0;
    uint32                          length_of_length_field = 0;
    gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
    gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
    boolean                         tag_comprehension_req  = TRUE;

    UIM_MSG_HIGH_0("Parsing AID");

    if((!aid) || (!next_tlv_offset) || (!more_tlvs) || (!ref_id))
    {
      UIM_MSG_ERR_0("NULL PTR");
      return GSTK_BAD_PARAM;
    }

    /* at least have tag and length */
    if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len,0))
    {
      *more_tlvs = FALSE;
      return GSTK_INVALID_LENGTH;
    }

    if(orig_cmd_data == NULL)
    {
      *more_tlvs = FALSE;
      return GSTK_MEMORY_ERROR;
    }

    /* initialize more tlvs to true */
    *more_tlvs = TRUE;

    count = orig_offset;

    /* AID TLV */
    tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)GSTK_AID_TAG,
                                           man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

    if (!gstk_continue_parsing(GSTK_AID_TAG, tag_parsing_result, man_opt_specifier, &gstk_status))
    {
      if (gstk_status == GSTK_SUCCESS) {
        /* Was optional tag, go through the parser_get_next_tlv_offset */
        gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);
      }
      return gstk_status;
    }

    if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
      tag_comprehension_req = FALSE;
    }

    /* valid provisioning aid Tag */
    count++;

    length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

    /*Access the i- 1 array element */
    if (orig_cmd_data[count + length_of_length_field - 1] > GSTK_MAX_APP_ID_LEN)
    {
      if (tag_comprehension_req)
      {
        UIM_MSG_ERR_1("Invalid AID Length 0x%x",
                      orig_cmd_data[count + length_of_length_field - 1]);
        return GSTK_ERROR;
      }
      else
      {
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
        aid->aid_len = 0;
      }
    }
    else
    {
      aid->aid_len = MIN(orig_cmd_data[count + length_of_length_field - 1],
                       GSTK_MAX_APP_ID_LEN);
    }
    /* position count to the aid itself in the tlv */
    count = count + length_of_length_field;

    if (aid->aid_len != 0)
    {
      (void)memscpy(aid->aid_data, sizeof(aid->aid_data), &orig_cmd_data[count],aid->aid_len );
    }
    count += aid->aid_len;

    /* next TLV offset */
    gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlvs);

    if (gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                    cmd_data_len, count);
      return GSTK_INVALID_LENGTH;
    }

    return gstk_status;

} /* gstk_parser_aid_tlv */


/*===========================================================================
FUNCTION gstk_parser_pli_nmr_utran_tlv

DESCRIPTION
  This function parses the TLV that is coded as the
  UTRAN Measurement Qualifier in command Provide Local Information

PARAMETERS
  orig_cmd_data_ptr:         [Input] Pointer to the original command
                             data to be parse
  cmd_data_len:              [Input] Valid length of the original command
                             data pointer
  orig_offset:               [Input] Offset in the original command data where the parsing
                             should begin
  tag:                       [Input] PLI UTRAN NMR tag
  next_tlv_offset_ptr:       [Input/Output] Pointer to the index offset for the next tlv
                             if applicable.
  more_tlvs_ptr:             [Input/Output] Pointer to boolean to indicate whether there are mo
                             TLV to be parse after exiting this function
  man_opt_specifier:         [Input] Specifies whether tlv is optional or mandatory
  ref_id_ptr:                [Input] Pointer tp Reference id from the cmd_details_ref table
  nmr_utran_qualifier_ptr:   [Output] Pointer to enum type for NMR UTRAN Qualifier

DEPENDENCIES
    None

RETURN VALUE
    gstk_status_enum_type
    GSTK_BAD_PARAM:          Bad input paramter provided
    GSTK_INVALID_LENGTH:     Invalid length field of tlv
    GSTK_MEMORY_ERROR:       Memory error
    GSTK_SUCCESS:            Successful parsing
    GSTK_ERROR:              Error in parsing

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_pli_nmr_utran_tlv(
    uint8                               *orig_cmd_data_ptr,
    int32                                cmd_data_len,
    int32                                orig_offset,
    tags_enum_type                       tag,
    int32                               *next_tlv_offset_ptr,
    boolean                             *more_tlvs_ptr,
    boolean                              man_opt_specifier,
    const uint32                        *ref_id_ptr,
    gstk_nmr_utran_qualifier_enum_type  *nmr_utran_qualifier_ptr)
{
  int32                           count                  = 0;
  gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
  gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;

  if((!next_tlv_offset_ptr) || (!more_tlvs_ptr) || (!nmr_utran_qualifier_ptr))
  {
    UIM_MSG_ERR_0("NULL PTR");
    return GSTK_BAD_PARAM;
  }

  if(orig_cmd_data_ptr == NULL)
  {
    *more_tlvs_ptr = FALSE;
    UIM_MSG_ERR_0("orig_cmd_data_ptr NULL PTR");
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to true */
  *more_tlvs_ptr = TRUE;

  /* has to have at least tag and length field */
  if (!gstk_parser_is_orig_offset_valid(orig_offset,cmd_data_len, 0))
  {
    UIM_MSG_ERR_1("Invalid offset: orig_offset 0x%x", orig_offset);
    return GSTK_INVALID_LENGTH;
  }

  if(cmd_data_len != GSTK_MAX_UTRAN_MSMT_QUAL_TLV_SIZE)
  {
    UIM_MSG_ERR_1("Invalid UTRAN msmt. qualifier Length 0x%x", cmd_data_len);
    return GSTK_INVALID_LENGTH;
  }

  count = orig_offset;

  tag_parsing_result = gstk_is_tag_valid(ref_id_ptr,
                                         orig_cmd_data_ptr[count],
                                         (uint8)tag, man_opt_specifier,
                                         orig_cmd_data_ptr,
                                         &count,
                                         cmd_data_len);

  if (!gstk_continue_parsing(tag, tag_parsing_result, man_opt_specifier, &gstk_status))
  {
    if (gstk_status == GSTK_SUCCESS)
    {
      /* Was optional tag, go through the parser_get_next_tlv_offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset_ptr, more_tlvs_ptr);
    }
    return gstk_status;
  }

  /* valid UTRAN measurement qualifier Tag */
  count++;

  if(orig_cmd_data_ptr[count] != GSTK_UTRAN_NMR_QUAL_LEN)
  {
    UIM_MSG_ERR_1("Invalid UTRAN msmt. qualifier Type Length 0x%x",
                  orig_cmd_data_ptr[count]);
    return GSTK_INVALID_LENGTH;
  }
  count++;

  switch(orig_cmd_data_ptr[count])
  {
    case GSTK_NMR_INTRA_FREQ_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_INTRA_FREQ_MEAS;
      break;
    case GSTK_NMR_INTER_FREQ_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_INTER_FREQ_MEAS;
      break;
    case GSTK_NMR_INTER_RAT_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_INTER_RAT_MEAS;
      break;
    case GSTK_NMR_INTER_RAT_LTE_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_INTER_RAT_LTE_MEAS;
      break;
    case GSTK_NMR_LTE_INTRA_FREQ_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_LTE_INTRA_FREQ_MEAS;
      break;
    case GSTK_NMR_LTE_INTER_FREQ_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_LTE_INTER_FREQ_MEAS;
      break;
    case GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_LTE_INTER_RAT_GERAN_MEAS;
      break;
    case GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS:
      *nmr_utran_qualifier_ptr = GSTK_NMR_LTE_INTER_RAT_UTRAN_MEAS;
      break;
    default:
      /* This TLV is being considered as mandatory if the PLI is of type NMR. Therefore
         return GSTK_INVALID_COMMAND regardless of the comprehension req flag */
      UIM_MSG_ERR_1("Invalid UTRAN Measurement Qualifier 0x%x tag_comprehension_req not set",
                    orig_cmd_data_ptr[count]);
      *nmr_utran_qualifier_ptr = GSTK_NMR_NONE_MEAS;
      return GSTK_INVALID_COMMAND;
    }

  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                count,
                                                next_tlv_offset_ptr,
                                                more_tlvs_ptr);

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x", cmd_data_len, count);
    return GSTK_INVALID_LENGTH;
  }

  return gstk_status;
}

/*===========================================================================
FUNCTION gstk_parser_plmn_list_tlv

DESCRIPTION
  This function parses the plmn list information provided as optional
  data in a REFRESH command.
  If there is no more TLVs to be parsed, the more tlv will be set to
  FALSE

PARAMETERS
  orig_cmd_data_ptr  : [Input] Pointer to the original command data to be parse
  cmd_data_len       : [Input] Valid length of the original command data pointer
  orig_offset        : [Input] Offset in the original command data where the parsing
                       should begin
  plmn_list_ptr      : [Input/Output] Pointer to file list buffer,
  next_tlv_offset_ptr: [Input/Output] Pointer to the index offset for the next tlv
                       if applicable.
  more_tlvs_ptr      : [Input/Output] Pointer to boolean to indicate whether there
                       are more TLV to be parse after exiting this function
  ref_id_ptr         : [Input] Pointer to reference id

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_plmn_list_tlv(
    uint8                        * orig_cmd_data_ptr,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_generic_data_type       * plmn_list_ptr,
    int32                        * next_tlv_offset_ptr,
    boolean                      * more_tlvs_ptr,
    boolean                        man_opt_specifier,
    const uint32                 * ref_id_ptr)
{
  int32                           count                  = 0;
  uint32                          length_of_length_field = 0;
  gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
  gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
  boolean                         tag_comprehension_req  = TRUE;

  UIM_MSG_HIGH_0("Parsing PLMN List");

  if((!plmn_list_ptr) || (!next_tlv_offset_ptr) || (!more_tlvs_ptr) ||
     (!orig_cmd_data_ptr) || (!ref_id_ptr))
  {
    UIM_MSG_ERR_3("Null Input Pointer: plmn_list_ptr=0x%x, next_tlv_offset_ptr=0x%x, more_tlvs_ptr=0x%x",
                  plmn_list_ptr, next_tlv_offset_ptr, more_tlvs_ptr);
    UIM_MSG_ERR_2("Null Input Pointer: orig_cmd_data_ptr=0x%x, ref_id_ptr=0x%x",
                  orig_cmd_data_ptr, ref_id_ptr);
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to true */
  *more_tlvs_ptr = TRUE;

  /* at least have tag and length */
  if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0))
  {
    return GSTK_INVALID_LENGTH;
  }

  count = orig_offset;

  /* provisioning plmn TLV */
  tag_parsing_result = gstk_is_tag_valid(ref_id_ptr,
                                         orig_cmd_data_ptr[count],
                                         (uint8)GSTK_PLMNWACT_LIST_TAG,
                                         man_opt_specifier,
                                         orig_cmd_data_ptr,
                                         &count,
                                         cmd_data_len);

  if(!gstk_continue_parsing(GSTK_PLMNWACT_LIST_TAG, tag_parsing_result,
                            man_opt_specifier, &gstk_status))
  {
    if(gstk_status == GSTK_SUCCESS)
    {
      /* Was optional tag, go through the parser_get_next_tlv_offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                    count,
                                                    next_tlv_offset_ptr,
                                                    more_tlvs_ptr);
    }
    return gstk_status;
  }

  if(!gstk_is_tag_comprehension_req(orig_cmd_data_ptr[count]))
  {
    tag_comprehension_req = FALSE;
  }

  /* valid provisioning file Tag */
  count++;

  length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data_ptr[count]);

  plmn_list_ptr->data_len = orig_cmd_data_ptr[count + length_of_length_field - 1];

  /* position count to the provisioning file location in the tlv */
  count = count + length_of_length_field;

  if(plmn_list_ptr->data_len != 0)
  {
    plmn_list_ptr->data_buffer_ptr = GSTK_CALLOC(plmn_list_ptr->data_len);
    if(plmn_list_ptr->data_buffer_ptr == NULL)
    {
      if(tag_comprehension_req)
      {
        return GSTK_MEMORY_ERROR;
      }
      else
      {
        /* update the cmd_table info to indicate there was a parsing issue */
        command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id_ptr)].partial_comprehension |= TRUE;
        count = count + plmn_list_ptr->data_len;
        plmn_list_ptr->data_len = 0;
      }
    }
    else
    {
      (void)memscpy(plmn_list_ptr->data_buffer_ptr,
             plmn_list_ptr->data_len,
             &orig_cmd_data_ptr[count],
             plmn_list_ptr->data_len);

      count = count + plmn_list_ptr->data_len;
    }
  }

  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                count,
                                                next_tlv_offset_ptr,
                                                more_tlvs_ptr);

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x", cmd_data_len, count);

    if (plmn_list_ptr->data_buffer_ptr != NULL)
    {
      gstk_free(plmn_list_ptr->data_buffer_ptr);
      plmn_list_ptr->data_buffer_ptr = NULL;
    }
    plmn_list_ptr->data_len = 0;
    return GSTK_INVALID_LENGTH;
  }

  return gstk_status;
} /* gstk_parser_plmn_list_tlv */

/*===========================================================================
FUNCTION gstk_parser_skip_tlv

DESCRIPTION
  This function skips over a TLV object

PARAMETERS
  cmd_data_len   : [Input] Valid length of the original command data pointer
  orig_offset    : [Input] Offset in the original command data where the parsing
                           should begin
  next_tlv_offset: [Input/Output] Pointer to the index offset for the next tlv
                                  if applicable.
  more_tlvs      : [Input/Output] Pointer to boolean to indicate whether there
                                  are more TLV to be parse after exiting this function
DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_parser_skip_tlv(
    const uint8 * orig_cmd_data_ptr,
    int32         cmd_data_len,
    int32         orig_offset,
    int32       * next_tlv_offset_ptr,
    boolean     * more_tlvs_ptr)
{
  int32                 count                  = 0;
  uint32                length_of_length_field = 0;
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  UIM_MSG_HIGH_0("gstk_parser_skip_tlv");

  if((next_tlv_offset_ptr == NULL) || (more_tlvs_ptr == NULL))
  {
    UIM_MSG_ERR_2("NULL Input Pointer:next_tlv_offset_ptr=0x%x, more_tlvs_ptr=0x%x",
                  next_tlv_offset_ptr, more_tlvs_ptr);
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to true */
  *more_tlvs_ptr = TRUE;

  /* at least have tag and length */
  if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0))
  {
    return GSTK_INVALID_LENGTH;
  }

  /* Let count point to Length field */
  count = orig_offset + 1;

  length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data_ptr[count]);

  count = count + length_of_length_field + orig_cmd_data_ptr[count + length_of_length_field - 1];
  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len,
                                                count,
                                                next_tlv_offset_ptr,
                                                more_tlvs_ptr);

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_2("cmd_data_len=0x%x, final count=0x%x",
                  cmd_data_len, orig_offset);
    return GSTK_INVALID_LENGTH;
  }

  return GSTK_SUCCESS;
} /* gstk_parser_skip_tlv */

/*===========================================================================
FUNCTION gstk_parser_activate_descriptor_tlv

DESCRIPTION
  Generic Function used by GSTK to parse the Active Descriptor TLV

PARAMETERS
  uint8                         * orig_cmd_data        - command data buffer
  int32                           cmd_data_len         - the length of the
                                                         command buffer
  int32                           orig_offset          - the offset to start
                                                         reading bearer from
  gstk_bearer_description_type  * target_description   - target details
  int32                         * next_tlv_offset      - pointer to next TLV
  boolean                       * more_tlv             - Indicates if there are
                                                         more TLVs

DEPENDENCIES

RETURN VALUE
  gstk_status_enum_type

===========================================================================*/
gstk_status_enum_type gstk_parser_activate_descriptor_tlv(
    uint8                         * orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    tags_enum_type                  tag,
    gstk_target_enum_type         * target_description,
    int32                         * next_tlv_offset,
    boolean                       * more_tlv,
    boolean                         man_opt_specifier,
    const uint32                  * ref_id
)
{
  int32                           count                  = 0;
  uint32                          length_of_length_field = 0;
  gstk_parsing_result_enum_type   tag_parsing_result     = GSTK_TLV_PARSING_CONTINUE;
  gstk_status_enum_type           gstk_status            = GSTK_SUCCESS;
  boolean                         tag_comprehension_req  = TRUE;
  int32                           tlv_length             = 0;

  UIM_MSG_HIGH_0("** Parsing activate descriptor");

  if((!next_tlv_offset) || (!more_tlv))
  {
    UIM_MSG_ERR_0("NULL PTR");
    return GSTK_BAD_PARAM;
  }

  /* initialize more tlvs to false */
  *more_tlv = FALSE;

  if (!gstk_parser_is_orig_offset_valid(orig_offset, cmd_data_len, 0)) {
    return GSTK_INVALID_LENGTH;
  }

  if (orig_cmd_data == NULL) {
    *more_tlv = FALSE;
    if(man_opt_specifier == GSTK_MANDATORY)
    {
      return GSTK_INVALID_COMMAND;
    }
    else
      return GSTK_SUCCESS;
  }

  if (target_description == NULL) {
    UIM_MSG_ERR_0("Null Input Param");
    return GSTK_MEMORY_ERROR;
  }

  /* Initialize Count */
  count = orig_offset;

  /* buffer size TLV */
  tag_parsing_result = gstk_is_tag_valid(ref_id, orig_cmd_data[count], (uint8)tag,
                                         man_opt_specifier, orig_cmd_data, &count, cmd_data_len);

  if (!gstk_continue_parsing(tag, tag_parsing_result, man_opt_specifier, &gstk_status))
  {
    if (gstk_status == GSTK_SUCCESS) {
      /* Was optional tag, go through the parser_get_next_tlv_offset */
      gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);
    }
    return gstk_status;
  }

  if(!gstk_is_tag_comprehension_req(orig_cmd_data[count])) {
    tag_comprehension_req = FALSE;
  }

  /* valid buffer size Tag */
  count++;

  length_of_length_field = gstk_find_length_of_length_value(&orig_cmd_data[count]);

  tlv_length = orig_cmd_data[count + length_of_length_field -1];
  if ( tlv_length < GSTK_ACTIVATE_DESCRIPTION_LEN ) {
    UIM_MSG_ERR_1("Invalid activate descriptor len 0x%x", tlv_length);
    if (tag_comprehension_req){
      return GSTK_INVALID_LENGTH;
    }
    /* update the cmd_table info to indicate there was a parsing issue */
    if (ref_id != NULL) {
      command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*ref_id)].partial_comprehension |= TRUE;
    }
    count = count + length_of_length_field + tlv_length;
  }
  else {
    /* Increment count to beginning of data */
    count = count + length_of_length_field;

    *target_description = (gstk_target_enum_type)orig_cmd_data[count];

    UIM_MSG_HIGH_1("Target Description 0x%x", *target_description);

    count+=1;
  }

  gstk_go_to_next_tag_index(&count, orig_offset, length_of_length_field, tlv_length);

  /* next TLV offset */
  gstk_status = gstk_parser_get_next_tlv_offset(cmd_data_len, count, next_tlv_offset, more_tlv);

  if (gstk_status != GSTK_SUCCESS) {
    /* any mem free of clean up if required */
  }
  return gstk_status;
} /*gstk_parser_activate_descriptor_tlv*/
