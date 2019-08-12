/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   P A C K E R


GENERAL DESCRIPTION
  The provides utility function for packing various raw data into the
  corresponding tlv format


FUNCTIONS
  gstk_packer_address_tlv
    This function populates the passed in the address tlv pointer

  gstk_packer_alpha_tlv
    This function populates the passed in alpha tlv pointer

  gstk_packer_bc_repeat_ind_tlv
    This function populates the passed in the bc repeat indicator tlv pointer

  gstk_packer_bcch_list_tlv
    This function populates the passed in bcch list tlv pointer

  gstk_packer_cause_tlv
    This function populates the passed in the cause tlv pointer

  gstk_packer_cc_req_action_tlv
    This function populates the passed in the call control requested
    action tlv pointer

  gstk_packer_ccp_tlv
    This function populates the passed in the ccp tlv pointer

  gstk_packer_cmd_details_tlv
    This function populates the passed in command_details pointer with the
    info in the command details ref table after verifying the ref id for the
    command details ref table is valid

  gstk_packer_date_time_tlv
    This function populates the passed in the date/time/timezone tlv pointer

  gstk_packer_dev_id_tlv
    This function populates the passed in device_id pointer

  gstk_packer_duration_tlv
    This function populates the passed in duration tlv pointer

  gstk_packer_evt_list_tlv
    This function populates the passed in the event list tlv pointer

  gstk_packer_help_request_tlv
    This function populates the passed in the help request tlv pointer

  gstk_packer_imei_tlv
    This function populates the passed in imei tlv pointer

  gstk_packer_item_id_tlv
    This function populates the passed in the item id tlv pointer

  gstk_packer_lang_sel_tlv
    This function populates the passed in the language selection tlv pointer

  gstk_packer_location_info_tlv
    This function populates the passed in location info tlv pointer

  gstk_packer_location_status_tlv
    This function populates the passed in the location status tlv pointer

  gstk_packer_nmr_tlv
    This function populates the passed in nmr tlv pointer

  gstk_packer_result_tlv
    This function populates the passed in result tlv pointer

  gstk_packer_sms_tpdu_tlv
    This function populates the passed in the sms tpdu tlv pointer

  gstk_packer_loc_info_tlv
    This functions populates the passed in location info tlv pointer

  gstk_packer_subaddress_tlv
    This function populates the passed in the subaddress tlv pointer

  gstk_packer_text_string_tlv
    This function populates the passed in text tlv pointer

  gstk_packer_timing_advance_tlv
    This function populates the passed in timing advance tlv pointer

  gstk_packer_transaction_id_tlv
    This function populates the passed in the transaction id tlv pointer

  gstk_packer_browser_term_cause_tlv
    This function populates the passed in browser termination cause tlv pointer

  gstk_packer_timer_id_tlv
    This function populates the passed in timer id tlv pointer

  gstk_packer_timer_value_tlv
    This function populates the passed in timer value tlv pointer

  gstk_packer_cell_broadcast_page_tlv
    This function populates the passed in cell broadcast page tlv pointer

  gstk_packer_ch_status_tlv
    This function populates the passed in channel status tlv pointer

  gstk_packer_csd_bearer_param
    This function populates the csd bearer information.

  gstk_packer_gprs_bearer_param
    This function populates the gprs bearer information.

  gstk_packer_bearer_description_tlv
    This function populates the passed in bearer description tlv pointer

  gstk_packer_buffer_size_tlv
    This function populates the passed in buffer size tlv pointer

  gstk_packer_ch_data_len_tlv
    This function populates the passed in channel data length tlv pointer

  gstk_packer_ch_data_tlv
    This function populates the passed in channel data tlv pointer

  gstk_packer_nmr_utran_tlv
    This function packs the nmr data in the nmr utran tlv pointer

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstkpacker.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
01/19/15   kr      Fixed Compilation warnings
11/11/14   shr     Fixed compilation warnings
10/13/14   gs      Removal of gstk_packer_cdma_location_info_tlv
10/08/14   bd      Changes to control eHRPD loc info feature with NV
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
07/11/14   shr     Add Support for NV Refresh
06/30/14   gm      Not sending additional info in CSG cell sel event
05/28/14   gm      Support of CSG Rel-11 feature
05/06/14   gs      GSTK Featurization enhancements and clean up
04/16/14   dy      Fix loc info packer copying with incorrect cell id len
04/03/14   dy      Replace gstk_malloc() with macro
03/31/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/17/14   gm      Added support for IMS communication control
01/30/14   vr      SIM initiated call control handling
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Updated GSTK_NULL_CHECK_PTR macro
12/05/13   hn      Support DNS query and Channel reuse
10/04/13   sw      Reduced F3 messages
09/13/13   gm      Support for Recovery and hot-swap
08/22/13   vr      Klocwork fixes
07/26/13   hh      Support for E-UTRAN Inter-frequency measurements
07/26/13   gs      Fixed incorrect access tech packing
06/24/13   xz      Support IMS Registration event download
04/23/13   gm      Added support for negative latitude and longitude
04/09/13   vr      Use safer versions of memcpy() and memmove()
02/08/13   bd      Support for (e)HRPD access tech
10/15/12   bd      Support for Closed Subscriber Group feature
08/13/12   sg      Control with NV mandatory additional info byte
07/12/12   sg      Fix for spec update to bearer type 0xB for non-GBR
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Remove tmc.h inclusion
03/13/12   sg      Fix bug in packing of bearer param TLV
08/24/11   xz      Fix compiler warnings and LINT/KW errors
06/06/11   sg      Add support for mandatory TLV for result 0x20
05/18/11   xz      Use MMGSDI evt to track apps enabled
05/03/11   xz      Provide LTE only support
04/24/11   dd      SCWS support
03/24/11   js      Fix for LTE Call Control.
11/26/11   xz      Add support of HRPD access tech
01/14/11   xz      Fix buffer overflow issue
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
10/20/10   xz      Fix the issue of not send TR in eHRPD only mode
08/30/10   xz      Sanity check on location infor before packing
08/26/10   xz      Add support of EPS Call Control
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
05/28/10   yt      Lint fixes
05/11/10   xz      Lint fixes
04/13/10   yt      Fixed compilation warning
04/07/10   yt      Fixed compilation warnings
03/26/10   dd      Fix merge issue in date/time function
03/15/10   xz      Add support of LTE access tech and LTE BIP
02/25/10   adp     Arm Lint Fixes
02/26/10   sg      Complete fix for handle unknown TON which has '+'
02/17/10   sg      For MT call handle unknown TON which has '+'
02/15/10   nb      Moved Dual Slot Changes
02/10/10   dd      Provide local time and time zone in PLI
12/29/09   rn      Fixed klocwork errors.
11/23/09   dd      Use GMT time zone in PLI
11/16/09   bd      Fixed compiler warnings
09/26/09   kp      ZI memory reduction changes
07/21/09   dd      Adding Time Zone Information
06/20/09   dd      Enable SMS for CDMA
06/18/09   dd      Change GSTK_MAX_RAW_SMS_LEN to 255
05/18/09   sg      Changed gstk_packer_address_tlv function to ignore '+' for
                   international numbers.
02/06/08   sk      Support for PLI UTRAN NMR
09/05/07   sk      Fixed warning
08/29/07   sk      Removed additional info conversion
08/01/07   sk      Fixed lint error
07/03/07   tml     Added support for additional info when call mod by CC
06/26/07   sk      Removed warnings.
06/24/07   sk      Fixed potential buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
09/18/06   sk      Fixed lint errors.
08/21/06   sk      Added featurization for GPRS call control
07/03/06   sk      Added support for CCAT sms pp dl.
06/24/06   sk      Lint fixes.
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control
05/21/06   sk      Fixed lint errors.
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related to location information
04/04/06   sk      Added support for PLI - Battery status,ESN, MEID, IMEISV
11/21/05   sp      Fixed lint errors
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
08/08/05   tml     fixed memcpy issue in channel data packing
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added gstk_packer_access_technology_tlv()
05/11/05   sst     Lint fixes
12/04/04  sk/tml   Channel status fix
11/23/04  sk/tml   Added SAT/USAT data support
08/19/04   tml     Added cell broadcast support
08/03/05   tml     Fixed timer value format issue.  Added error messages
05/26/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports.  Added more debug messages
09/12/03   tml     Updated the address packing function
09/03/03   tml     Assigned Command detail tag correctly
07/08/03   tml     Added check such that only populate the address and TON/NPI
                   field in address TLV if the length is greater than 0
05/18/03   tml     linted
04/21/03   tml     Added NPI/TON support and cdma sms support
03/06/03   tml     Updated enum names
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

#include "err.h"
#include "string.h"
#include "uim_msg.h"

/*===========================================================================
FUNCTION gstk_packer_cmd_details_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  ref_id: [Input] Unique reference Id
  command_rsp_type: [Input] Specifies the type of terminal response, e.g.,
                            GSTK_DISPLAY_TEXT_CNF, etc,
  command_number: [Input] Command number for the proactive command that
                          this terminal response is responding to
  command_details: [Input/Output] Pointers to which the command details
                                  to be populated

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
gstk_status_enum_type gstk_packer_cmd_details_tlv(
  uint32                            ref_id,
  uint32                            command_rsp_type,
  uint8                             command_number,
  gstk_command_details_tlv_type *   command_details
)
{
    uint32 index = 0;
    UIM_MSG_HIGH_0("** Packing Cmd Details");

    if(command_details == NULL)
    {
       UIM_MSG_ERR_0("Cmd Detail Err: NULL");
       return GSTK_BAD_PARAM;
    }
    /* 4 checks to verify that the ref_id is a valid value/match to the information
    ** in the command_details_ref_table
    ** - reference_id check
    ** - whether the location in the table is still valid
    ** - command number check
    ** - response type and type of command match
    */
    index = GSTK_CMD_REF_TABLE_INDEX(ref_id);
    if (index  < GSTK_MAX_PROACTIVE_COMMAND) { /*ref_id within range */
        if (command_details_ref_table_ptr[index].free_slot != TRUE ) { /* this location is still used*/
          if ((int)command_details_ref_table_ptr[index].expected_cmd_rsp_type == command_rsp_type) {
              /* command matches -> valid ref_id */
              if (command_details_ref_table_ptr[index].command_details.command_number !=
                    command_number ) {
                /* wrong match */
                UIM_MSG_ERR_0("Cmd Detail Err: Command Num Mismatch");
                return GSTK_BAD_PARAM;
              }
          }
        }
        else {
          UIM_MSG_ERR_1("Cmd Detail Err: command_details_ref_table_ptr[0x%x] not in use",
                        ref_id);
          return GSTK_BAD_PARAM;
        }
    }
    else {
      UIM_MSG_ERR_0("Cmd Detail Err: ref_id 0x%x out of range");
      return GSTK_BAD_PARAM;
    }

    /* populate the command_details tlv for the response */
    command_details->tag = command_details_ref_table_ptr[index].command_details.tag;
    command_details->length = GSTK_COMMAND_DETAILS_LEN;
    command_details->command_number =
      command_details_ref_table_ptr[index].command_details.command_number;
    command_details->type_of_command =
      command_details_ref_table_ptr[index].command_details.type_of_command;
    command_details->qualifier =
      command_details_ref_table_ptr[index].command_details.qualifier;
    return GSTK_SUCCESS;
} /*gstk_packer_cmd_details_tlv*/


/*===========================================================================
FUNCTION gstk_packer_dev_id_tlv

DESCRIPTION

  This function populates the passed in device_id pointer

PARAMETERS
  source: [Input] Indicates source device
  destination: [Input] Indicates target device
  device_id: [Input/Output] Pointers to which the device id to be populated

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
gstk_status_enum_type gstk_packer_dev_id_tlv(
  gstk_device_identity_type                     source,
  gstk_device_identity_type                     destination,
  gstk_device_identities_tag_tlv_type * device_id
)
{

    UIM_MSG_HIGH_0("** Packing device ID");

    if(device_id == NULL)
    {
       UIM_MSG_ERR_0("Device Id Err: NULL");
       return GSTK_BAD_PARAM;
    }
    device_id->tag = (int)GSTK_DEVICE_IDENTITY_TAG;
    device_id->device_tag_length = GSTK_DEVICE_IDENTITY_LEN;
    device_id->source_device = (uint8)source;
    device_id->target_device = (uint8)destination;

    return GSTK_SUCCESS;
} /* gstk_packer_dev_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_loc_info_tlv

DESCRIPTION

  This function populates the passed in location info pointer

PARAMETERS
  loc_info: [Input/Output] Pointers to which the result is populated
  mcc_mnc: [Input] Indicates location information input

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
gstk_status_enum_type gstk_packer_loc_info_tlv(
  gstk_location_information_tag_tlv_type * loc_info,
  const uint8                            * mcc_mnc
)
{
    UIM_MSG_HIGH_0("** Packing Local Information");

    if(loc_info == NULL || mcc_mnc == NULL)
    {
       UIM_MSG_ERR_0("Local Information Err: NULL");
       return GSTK_BAD_PARAM;
    }

    loc_info->tag = (int)GSTK_LOCATION_INFORMATION_TAG;
    loc_info->location_info_length = GSTK_MAX_PLMN_LEN; /* MCC and MNC*/

    (void)gstk_memcpy(loc_info->mcc_and_mnc,
                   mcc_mnc,
                   GSTK_MAX_PLMN_LEN,
                   GSTK_MAX_PLMN_LEN,
                   GSTK_MAX_PLMN_LEN);

    return GSTK_SUCCESS;
} /* gstk_packer_dev_id_tlv */

/*===========================================================================
FUNCTION gstk_packer_result_tlv

DESCRIPTION

  This function populates the passed in result tlv pointer

PARAMETERS
  command_result: [Input] Indicates general result
  additional_info: [Input] Indicates any additional info for the result
  result: [Input/Output] Pointers to which the result to be populated

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
gstk_status_enum_type gstk_packer_result_tlv(
  gstk_general_result_enum_type         command_result,
  gstk_additional_info_ptr_type         additional_info,
  gstk_result_tag_tlv_type              * result
)
{

   UIM_MSG_HIGH_1("** Packing result, 0x%x", command_result);

   GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

   if(result == NULL)
   {
      UIM_MSG_ERR_0("Result Err: NULL");
      return GSTK_BAD_PARAM;
   }
   /* initialize additional result pointer to null */
   result->additional_info = NULL;

   result->result_tag = (int)GSTK_RESULT_TAG;
   /* variable length result */
   result->result_tag_length = additional_info.length + 1;
   result->result = (uint8)command_result;

   result->additional_info = NULL;
   /* can be multiple additional_info */
   if ((additional_info.length != 0) && (additional_info.additional_info_ptr != NULL)) {
     result->additional_info =
      (uint8*)GSTK_CALLOC(additional_info.length * sizeof(byte) );

     if (result->additional_info == NULL) {
       return GSTK_MEMORY_ERROR;
     }
     (void)memscpy(result->additional_info,
     	    additional_info.length,
            additional_info.additional_info_ptr,
            additional_info.length);
   }
   else if((gstk_nv_get_feature_status(
              GSTK_CFG_FEATURE_SLOT_TR_ADD_INFO_BYTE,
              gstk_curr_inst_ptr->slot_id) == TRUE))
   {
     /* If result is 0x20 and additional info not present - set additional info to
        No specific cause can be given as additional info is mandatory if terminal is
        unable to process command */
     if(result->result == GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND)
     {
       UIM_MSG_HIGH_0("TR Result is Unable to Process Cmd with No additional Info");
       result->additional_info =
         (uint8*)GSTK_CALLOC(sizeof(uint8));
       if (result->additional_info == NULL)
       {
         return GSTK_MEMORY_ERROR;
       }
       (result->result_tag_length)++;
       *(result->additional_info) = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
     }
   }
   return GSTK_SUCCESS;
} /* gstk_packer_result_tlv */


/*===========================================================================
FUNCTION gstk_packer_text_string_tlv

DESCRIPTION

  This function populates the passed in text tlv pointer

PARAMETERS
  dcs: [Input] Indicates the text string coding scheme
  data_len: [Input] Indicates string length
  data: [Input] Indicates the string
  string_tlv: [Input/Output] Pointers to which the text string to be populated

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
gstk_status_enum_type gstk_packer_text_string_tlv(
       tags_enum_type         tag,
       uint8                  dcs,
       uint8                  data_len,
       const uint8            *data,
       gstk_string_tlv_type   *string_tlv
)
{

   gstk_status_enum_type gstk_status = GSTK_SUCCESS;

   UIM_MSG_HIGH_0("** Packing text string");

   if(string_tlv == NULL)
   {
      UIM_MSG_ERR_0("String TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   string_tlv->tag = (uint8)tag;
   string_tlv->length = data_len + 1;

   /* get data coding scheme */
   string_tlv->data_coding_scheme = dcs;

   if ((data_len > 0) && (data!= NULL)) {
     if (string_tlv->text != NULL) {
        (void)memscpy(string_tlv->text,
        	      data_len,
               data,
               data_len);
     }
     else {
       UIM_MSG_ERR_0("Text String Err: Null Text ptr");
       gstk_status = GSTK_MEMORY_ERROR;
     }
   }

   return gstk_status;
} /* gstk_packer_text_string_tlv */


/*===========================================================================
FUNCTION gstk_packer_alpha_tlv

DESCRIPTION

  This function populates the passed in alpha tlv pointer

PARAMETERS
  data: [Input] Indicates the alpha text
  string_tlv: [Input/Output] Pointers to which the alpha to be populated

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
gstk_status_enum_type gstk_packer_alpha_tlv(
       const gstk_string_type *data,
       gstk_alpha_tlv_type    *string_tlv
)
{

   gstk_status_enum_type gstk_status = GSTK_SUCCESS;

   UIM_MSG_HIGH_0("** Packing alpha");

   if(string_tlv == NULL)
   {
      UIM_MSG_ERR_0("String TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   string_tlv->tag = (int)GSTK_ALPHA_IDENTIFIER_TAG;
   /* check for data has null termination */
   if(data->text[data->length - 1] == 0)
   {
      string_tlv->length = data->length - 1;
   }
   else
   {
   string_tlv->length = data->length;
   }

   if (string_tlv->length > 0) {

     /* malloc alpha space */
     string_tlv->alpha = GSTK_CALLOC(string_tlv->length);

     if (string_tlv->alpha != NULL) {
        (void)gstk_memcpy(string_tlv->alpha,
                       data->text,
                       string_tlv->length,
                       data->length,
                       string_tlv->length);
     }
     else {
       gstk_status = GSTK_MEMORY_ERROR;
     }
   }

   return gstk_status;
} /* gstk_packer_alpha_tlv */

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_packer_3gpp2_location_info_tlv

DESCRIPTION

  This function populates the passed in location info tlv pointer

PARAMETERS
  data: [Input] Indicates the location info
  location_info_tlv: [Input/Output] Pointers to which the location info
                                    to be populated

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
gstk_status_enum_type gstk_packer_3gpp2_location_info_tlv(
       const gstk_cdma_loci_ext_type             *data,
       gstk_cdma_loci_ext_cache_type             *cache,
       gstk_cdma_loci_ext_tlv_type               *location_info_tlv
)
{
   int offset = 0;

   UIM_MSG_HIGH_0("** Packing 3GPP2 location info");

   if(data == NULL || location_info_tlv == NULL || cache == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   location_info_tlv->tag = int32touint8(GSTK_LOCATION_INFORMATION_TAG);
   location_info_tlv->length = GSTK_CDMA_LOCATION_INFORMATION_LEN;

   if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE)
   {
     if (cache->is_advance_info_valid)
     {
       location_info_tlv->length += GSTK_1X_ADV_LOCI_LEN;
     }
     if (cache->is_hdr_info_valid)
     {
       location_info_tlv->length += GSTK_HDR_LOCI_LEN;
     }
   }
   location_info_tlv->loc_info = (uint8 *)GSTK_CALLOC(location_info_tlv->length);

   if (location_info_tlv->loc_info == NULL)
   {
     return GSTK_MEMORY_ERROR;
   }

   /* Initialize location info to all 0xFFs */
   memset(location_info_tlv->loc_info, 0xFF, location_info_tlv->length);

   if ((offset + sizeof(data->cdma_loc_info.mcc)) <= location_info_tlv->length)
   {
     (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                       data->cdma_loc_info.mcc,
                       GSTK_MCC_LEN,
                       location_info_tlv->length - offset,
                       sizeof(data->cdma_loc_info.mcc));
     offset += sizeof(data->cdma_loc_info.mcc);
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if ((offset + 1) <= location_info_tlv->length)
   {
     location_info_tlv->loc_info[offset] =
        data->cdma_loc_info.imsi_11_12;
     offset++;
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if ((offset + sizeof(data->cdma_loc_info.sid_info.sid)) <= location_info_tlv->length)
   {
     (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                       data->cdma_loc_info.sid_info.sid,
                       data->cdma_loc_info.sid_info.id_len,
                       location_info_tlv->length - offset,
                       sizeof(data->cdma_loc_info.sid_info.sid));
     offset += sizeof(data->cdma_loc_info.sid_info.sid);
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if ((offset + sizeof(data->cdma_loc_info.nid_info.nid)) <= location_info_tlv->length)
   {
     (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                       data->cdma_loc_info.nid_info.nid,
                       data->cdma_loc_info.nid_info.id_len,
                       location_info_tlv->length - offset,
                       sizeof(data->cdma_loc_info.nid_info.nid));
     offset += sizeof(data->cdma_loc_info.nid_info.nid);
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if ((offset + sizeof(data->cdma_loc_info.base_id_info.base_id)) <= location_info_tlv->length)
   {
     (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                       data->cdma_loc_info.base_id_info.base_id,
                       data->cdma_loc_info.base_id_info.id_len,
                       location_info_tlv->length - offset,
                       sizeof(data->cdma_loc_info.base_id_info.base_id));
     offset += sizeof(data->cdma_loc_info.base_id_info.base_id);
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if ((offset + sizeof(data->cdma_loc_info.base_lat)) <= location_info_tlv->length)
   {
     (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                       data->cdma_loc_info.base_lat,
                       GSTK_BASE_LAT_LEN,
                       location_info_tlv->length - offset,
                       sizeof(data->cdma_loc_info.base_lat));
     offset += GSTK_BASE_LAT_LEN;
     location_info_tlv->loc_info[offset - 1] &= 0x3F;
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if ((offset + sizeof(data->cdma_loc_info.base_long)) <= location_info_tlv->length)
   {
     (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                       data->cdma_loc_info.base_long,
                       GSTK_BASE_LONG_LEN,
                       location_info_tlv->length - offset,
                       sizeof(data->cdma_loc_info.base_long));
     offset += GSTK_BASE_LONG_LEN;
     location_info_tlv->loc_info[offset - 1] &= 0x7F;
   }
   else
   {
     gstk_util_location_info_tlv_cleanup(location_info_tlv);
     return GSTK_ERROR;
   }

   if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE)
   {
      if (!cache->is_cdma_info_valid &&
          (cache->is_advance_info_valid || cache->is_hdr_info_valid))
      {
        memset(location_info_tlv->loc_info, 0xFF, location_info_tlv->length);
      }

      if (cache->is_advance_info_valid)
      {
        if ((offset + sizeof(cache->data.height)) <= location_info_tlv->length)
        {
          (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                            cache->data.height,
                            GSTK_HEIGHT_LEN,
                            location_info_tlv->length - offset,
                            sizeof(cache->data.height));
          offset += GSTK_HEIGHT_LEN;
        }
        else
        {
           gstk_util_location_info_tlv_cleanup(location_info_tlv);
           return GSTK_ERROR;
        }

        if ((offset + 1) <= location_info_tlv->length) {
           location_info_tlv->loc_info[offset] =
              cache->data.horizontal_uncertainty;
           offset++;
        }
        else
        {
           gstk_util_location_info_tlv_cleanup(location_info_tlv);
           return GSTK_ERROR;
        }
      }

      if (cache->is_hdr_info_valid)
      {
         if ((offset + sizeof(cache->data.country_code)) <= location_info_tlv->length)
         {
            (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                              cache->data.country_code,
                              GSTK_MCC_LEN,
                              location_info_tlv->length - offset,
                              sizeof(cache->data.country_code));
            offset += GSTK_MCC_LEN;
         }
         else
         {
            gstk_util_location_info_tlv_cleanup(location_info_tlv);
            return GSTK_ERROR;
         }

         if ((offset + sizeof(cache->data.sector_id)) <= location_info_tlv->length)
         {
            (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                              cache->data.sector_id,
                              GSTK_HDR_SECTOR_ID_LEN,
                              location_info_tlv->length - offset,
                              sizeof(cache->data.sector_id));
            offset += GSTK_HDR_SECTOR_ID_LEN;
         }
         else
         {
            gstk_util_location_info_tlv_cleanup(location_info_tlv);
            return GSTK_ERROR;
         }

         if ((offset + sizeof(cache->data.cdma_channel)) <= location_info_tlv->length)
         {
            (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                              cache->data.cdma_channel,
                              GSTK_CDMA_CHANNEL_LEN,
                              location_info_tlv->length - offset,
                              sizeof(cache->data.cdma_channel));
            offset += GSTK_CDMA_CHANNEL_LEN;
         }
         else
         {
            gstk_util_location_info_tlv_cleanup(location_info_tlv);
            return GSTK_ERROR;
         }

         if ((offset + sizeof(cache->data.base_lat)) <= location_info_tlv->length)
         {
            (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                              cache->data.base_lat,
                              GSTK_BASE_LAT_LEN,
                              location_info_tlv->length - offset,
                              sizeof(cache->data.base_lat));
            offset += GSTK_BASE_LAT_LEN;
            location_info_tlv->loc_info[offset - 1] &= 0x3F;
         }
         else
         {
            gstk_util_location_info_tlv_cleanup(location_info_tlv);
            return GSTK_ERROR;
         }

         if ((offset + sizeof(cache->data.base_long)) <= location_info_tlv->length)
         {
            (void)gstk_memcpy(&(location_info_tlv->loc_info[offset]),
                              cache->data.base_long,
                              GSTK_BASE_LONG_LEN,
                              location_info_tlv->length - offset,
                              sizeof(cache->data.base_long));
            offset += GSTK_BASE_LONG_LEN;
            location_info_tlv->loc_info[offset - 1] &= 0x7F;
         }
         else
         {
            gstk_util_location_info_tlv_cleanup(location_info_tlv);
            return GSTK_ERROR;
         }
      }
   } /* if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE) */

   return GSTK_SUCCESS;
} /* gstk_packer_3gpp2_location_info_tlv */
#endif /*#ifdef FEATURE_CDMA */


/*===========================================================================
FUNCTION gstk_packer_location_info_tlv

DESCRIPTION

  This function populates the passed in location info tlv pointer

PARAMETERS
  data: [Input] Indicates the location info
  location_info_tlv: [Input/Output] Pointers to which the location info
                                    to be populated

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
gstk_status_enum_type gstk_packer_location_info_tlv(
       const gstk_location_info_rsp_type    *data,
       gstk_location_information_tlv_type   *location_info_tlv
)
{

   UIM_MSG_HIGH_0("** Packing location info");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(location_info_tlv == NULL)
   {
      UIM_MSG_ERR_0("Location Info TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   location_info_tlv->tag = (int)GSTK_LOCATION_INFORMATION_TAG;
   location_info_tlv->length = GSTK_LOCATION_INFORMATION_LEN;

   (void)memscpy(location_info_tlv->location_info.cell_id,
          sizeof(location_info_tlv->location_info.cell_id),
          data->cell_id,
          2);
   (void)memscpy(location_info_tlv->location_info.loc_area_code,
          sizeof(location_info_tlv->location_info.loc_area_code),
          data->loc_area_code,
          2);
   (void)memscpy(location_info_tlv->location_info.mcc_and_mnc,
          sizeof(location_info_tlv->location_info.mcc_and_mnc),
          data->mcc_and_mnc,
          3);

   if (data->cell_id_len == 4)
   {
     (void)memscpy(location_info_tlv->location_info.ext_cell_id,
          sizeof(location_info_tlv->location_info.ext_cell_id),
          &data->cell_id[2],
          2);
     location_info_tlv->length += 2;
   }

   return GSTK_SUCCESS;
} /* gstk_packer_location_info_tlv */


/*===========================================================================
FUNCTION gstk_packer_imei_tlv

DESCRIPTION

  This function populates the passed in imei tlv pointer

PARAMETERS
  data: [Input] Indicates the imei
  imei_tlv: [Input/Output] Pointers to which the imei to be populated

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
gstk_status_enum_type gstk_packer_imei_tlv(
       const uint8          *data,
       gstk_imei_tlv_type   *imei_tlv
)
{
   UIM_MSG_HIGH_0("** Packing imei");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }
   if(imei_tlv == NULL)
   {
      UIM_MSG_ERR_0("IMEI TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   imei_tlv->tag = (int)GSTK_IMEI_TAG;
   imei_tlv->length = GSTK_IMEI_LEN;

   (void)memscpy(imei_tlv->imei,
          sizeof(imei_tlv->imei),
          data,
          GSTK_IMEI_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_imei_tlv */


#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_packer_esn_tlv

DESCRIPTION

  This function populates the passed in esn tlv pointer

PARAMETERS
  data: [Input] Indicates the esn
  esn_tlv: [Input/Output] Pointers to which the esn to be populated

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
gstk_status_enum_type gstk_packer_esn_tlv(
       const uint8         *data,
       gstk_esn_tlv_type   *esn_tlv
)
{
   UIM_MSG_HIGH_0("** Packing esn");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }
   if(esn_tlv == NULL)
   {
      UIM_MSG_ERR_0("ESN TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   esn_tlv->tag = (int)GSTK_ESN_TAG;
   esn_tlv->length = GSTK_ESN_LEN;

   (void)memscpy(esn_tlv->esn,
          sizeof(esn_tlv->esn),
          data,
          GSTK_ESN_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_esn_tlv */


/*===========================================================================
FUNCTION gstk_packer_meid_tlv

DESCRIPTION

  This function populates the passed in meid tlv pointer

PARAMETERS
  data: [Input] Indicates the meid
  meid_tlv: [Input/Output] Pointers to which the meid to be populated

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
gstk_status_enum_type gstk_packer_meid_tlv(
       const uint8          *data,
       gstk_meid_tlv_type   *meid_tlv
)
{
   UIM_MSG_HIGH_0("** Packing meid");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }
   if(meid_tlv == NULL)
   {
      UIM_MSG_ERR_0("MEID TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   meid_tlv->tag = (int)GSTK_MEID_TAG;
   meid_tlv->length = GSTK_MEID_LEN;

   (void)memscpy(meid_tlv->meid,
          sizeof(meid_tlv->meid),
          data,
          GSTK_MEID_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_meid_tlv */
#endif /* FEATURE_CDMA */


/*===========================================================================
FUNCTION gstk_packer_imeisv_tlv

DESCRIPTION

  This function populates the passed in imeisv tlv pointer

PARAMETERS
  data: [Input] Indicates the imeisv
  imeisv_tlv: [Input/Output] Pointers to which the imeisv to be populated

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
gstk_status_enum_type gstk_packer_imeisv_tlv(
       const uint8            *data,
       gstk_imeisv_tlv_type   *imeisv_tlv
)
{
   UIM_MSG_HIGH_0("** Packing imeisv");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }
   if(imeisv_tlv == NULL)
   {
      UIM_MSG_ERR_0("IMEISV TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   imeisv_tlv->tag = (int)GSTK_IMEISV_TAG;
   imeisv_tlv->length = GSTK_IMEI_SV_LEN;

   (void)memscpy(imeisv_tlv->imeisv,
          sizeof(imeisv_tlv->imeisv),
          data,
          GSTK_IMEI_SV_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_imeisv_tlv */


/*===========================================================================
FUNCTION gstk_packer_access_technology_tlv

DESCRIPTION

  This function populates the passed in access technology tlv pointer

PARAMETERS
  data                  : [Input] Indicates the iccess technology
  iccess_technology_tlv: [Input/Output] Pointers to which the  access technology needs to be populated

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
gstk_status_enum_type gstk_packer_access_technology_tlv(
  const uint8                       *data_ptr,
  gstk_access_technology_tlv_type   *access_tech_tlv
)
{
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Data_ptr is NULL");
    return GSTK_ERROR;
  }

  if(access_tech_tlv == NULL)
  {
    UIM_MSG_ERR_0("access_tech_tlv is NULL");
    return GSTK_ERROR;
  }

  UIM_MSG_HIGH_1("Packing Access Tech 0x%x",
                 *data_ptr);

  access_tech_tlv->tag    = (int)GSTK_ACCESS_TECHNOLOGY_TAG;
  access_tech_tlv->length =  GSTK_ACCESS_TECHNOLOGY_LEN;
  access_tech_tlv->access_technology = *data_ptr;
  /* If it's VZW, use 0x08 for access tech */
  if((access_tech_tlv->access_technology == GSTK_ACCESS_TECH_EHRPD) &&
     ((gstk_nv_get_feature_status(
        GSTK_CFG_FEATURE_SLOT_IMSI_M_ENH,
        gstk_curr_inst_ptr->slot_id) == TRUE)||
      (gstk_nv_get_feature_status(
         GSTK_CFG_FEATURE_SLOT_PROP_LOC_STATUS,
         gstk_curr_inst_ptr->slot_id) == TRUE)))
  {
    access_tech_tlv->access_technology = 0x08;
  }

  return GSTK_SUCCESS;
}


/*===========================================================================
FUNCTION gstk_packer_battery_status_tlv

DESCRIPTION

  This function populates the passed in battery status tlv pointer

PARAMETERS
  data                  : [Input] Indicates the battery status
  battery_status_tlv: [Input/Output] Pointers to which the battery status needs to be populated

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
gstk_status_enum_type gstk_packer_battery_status_tlv(
  const uint8                    *data_ptr,
  gstk_battery_status_tlv_type   *battery_status_tlv
)
{
  UIM_MSG_HIGH_0("** Packing Battery Status");

  if(data_ptr == NULL)
  {
    UIM_MSG_ERR_0("Data_ptr is NULL");
    return GSTK_BAD_PARAM;
  }

  if(battery_status_tlv == NULL)
  {
    UIM_MSG_ERR_0("battery_status_tlv is NULL");
    return GSTK_BAD_PARAM;
  }

  battery_status_tlv->tag            = (int)GSTK_BATTERY_STATUS_TAG;
  battery_status_tlv->length         =  GSTK_BATTERY_STATUS_LEN;
  battery_status_tlv->battery_status = *data_ptr;

  return GSTK_SUCCESS;
}


/*===========================================================================
FUNCTION gstk_packer_nmr_tlv

DESCRIPTION

  This function populates the passed in nmr tlv pointer

PARAMETERS
  data: [Input] Indicates the nmr
  nmr_tlv: [Input/Output] Pointers to which the nmr to be populated

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
gstk_status_enum_type gstk_packer_nmr_tlv(
       const uint8          *data,
       gstk_nmr_tlv_type    *nmr_tlv
)
{

   UIM_MSG_HIGH_0("** Packing nmr");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(nmr_tlv == NULL)
   {
      UIM_MSG_ERR_0("NMR TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   nmr_tlv->tag = (int)GSTK_NETWORK_MEASUREMENT_RESULTS_TAG;
   nmr_tlv->length = GSTK_NMR_LEN;

   (void)memscpy(nmr_tlv->nmr,
          sizeof(nmr_tlv->nmr),
          data,
          GSTK_NMR_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_nmr_tlv */


/*===========================================================================
FUNCTION gstk_packer_bcch_list_tlv

DESCRIPTION

  This function populates the passed in bcch list tlv pointer

PARAMETERS
  data: [Input] Indicates the bcch list
  bcch_list_tlv: [Input/Output] Pointers to which the bcch list to be populated

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
gstk_status_enum_type gstk_packer_bcch_list_tlv(
       uint8                        data_len,
       const uint8                  *data,
       gstk_bcch_ch_list_tlv_type   *bcch_list_tlv
)
{
   UIM_MSG_HIGH_0("** Packing bcch");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(bcch_list_tlv == NULL)
   {
      UIM_MSG_ERR_0("BCCH LIST TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   bcch_list_tlv->tag = (int)GSTK_BCCH_CHANNEL_LIST_TAG;
   bcch_list_tlv->length = data_len;

   if(data_len > 0) {
     if (bcch_list_tlv->channel_list != NULL) {
       (void)memscpy(bcch_list_tlv->channel_list,
       	      data_len,
              data,
              data_len);
       return GSTK_SUCCESS;
     }
     else {
       UIM_MSG_ERR_0("BCCH List Err: Null Channel List ptr");
       return GSTK_NULL_INPUT_PARAM;
     }
   }
   return GSTK_SUCCESS;
} /* gstk_packer_bcch_list_tlv */


/*===========================================================================
FUNCTION gstk_packer_nmr_utran_tlv

DESCRIPTION
  This function packs the nmr data in the nmr utran tlv pointer

PARAMETERS
  length:            [Input] Length of data
  data_ptr:          [Input] Pointer from which nmr data is to be copied
  nmr_utran_tlv_ptr: [Input/Output] Pointers to which the nmr is to be
                     populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NULL_INPUT_PARAM:  Null input param
    GSTK_BAD_PARAM:         Bad input paramter
    GSTK_SUCCESS:           Success

COMMENTS
  dest buffer length field ie nmr_utran_tlv_ptr->length is assigned
  by the caller function

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_nmr_utran_tlv(
         uint32                        length,
         const uint8                   *data_ptr,
         gstk_nmr_utran_tlv_type       *nmr_utran_tlv_ptr
)
{
  UIM_MSG_HIGH_3("gstk_packer_nmr_utran_tlv: length=0x%x, data_ptr=%p, "
                 "nmr_utran_tlv_ptr=%p", length, data_ptr, nmr_utran_tlv_ptr);

  if(data_ptr == NULL)
  {
     UIM_MSG_ERR_0("NMR TLV Err: data_ptr NULL ptr ");
     return GSTK_NULL_INPUT_PARAM;
  }
  if(nmr_utran_tlv_ptr == NULL)
  {
     UIM_MSG_ERR_0("NMR TLV Err: nmr_utran_tlv_ptr NULL");
     return GSTK_NULL_INPUT_PARAM;
  }
  if(length == 0 || length > 0xFF)
  {
     UIM_MSG_ERR_0("NMR TLV Err: length is 0 or > 0xFF");
     return GSTK_BAD_PARAM;
  }

  /* nmr_utran_tlv_ptr->length value has already been assigned by the caller function */
  if(nmr_utran_tlv_ptr->length < length)
  {
     UIM_MSG_ERR_0("NMR TLV Err: deat length is <  src length");
     return GSTK_BAD_PARAM;
  }

  nmr_utran_tlv_ptr->tag = (int)GSTK_NETWORK_MEASUREMENT_RESULTS_TAG;

  if (nmr_utran_tlv_ptr->measurement_report_ptr == NULL)
  {
      UIM_MSG_ERR_0("NMR UTRAN Err: Measurement report ptr");
      return GSTK_NULL_INPUT_PARAM;
  }
  (void)memscpy(nmr_utran_tlv_ptr->measurement_report_ptr,
         length,
         data_ptr,
         length);
  return GSTK_SUCCESS;
}/* gstk_packer_nmr_utran_tlv */

/*===========================================================================
FUNCTION gstk_packer_timing_advance_tlv

DESCRIPTION

  This function populates the passed in timing advance tlv pointer

PARAMETERS
  data: [Input] Indicates the timing advance list
  timing_adv_tlv: [Input/Output] Pointers to which the timing advance to be
                                 populated

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
gstk_status_enum_type gstk_packer_timing_advance_tlv(
       const gstk_time_advance_rsp_type     *data,
       gstk_timing_advance_tlv_type         *timing_adv_tlv
)
{
   UIM_MSG_HIGH_0("** Packing timing advance");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(timing_adv_tlv == NULL)
   {
      UIM_MSG_ERR_0("TIMING ADV TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   timing_adv_tlv->tag = (int)GSTK_TIMING_ADVANCE_TAG;
   timing_adv_tlv->length = GSTK_TIMING_ADVANCE_LEN;

   timing_adv_tlv->timing_advance.me_status = data->me_status;
   timing_adv_tlv->timing_advance.timing_advance = data->timing_advance;

   return GSTK_SUCCESS;

} /* gstk_packer_timing_advance_tlv */


/*===========================================================================
FUNCTION gstk_packer_lang_sel_tlv

DESCRIPTION

  This function populates the passed in the language selection tlv pointer

PARAMETERS
  data: [Input] Indicates the language selection
  imei_tlv: [Input/Output] Pointers to which the language selection to be
                           populated

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
gstk_status_enum_type gstk_packer_lang_sel_tlv(
       const uint8          *data,
       gstk_lang_tlv_type   *lang_tlv
)
{

   UIM_MSG_HIGH_0("** Packing lang selection");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(lang_tlv == NULL)
   {
      UIM_MSG_ERR_0("Lang TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }
   lang_tlv->tag = (int)GSTK_LANGUAGE_TAG;
   lang_tlv->length = GSTK_LANGUAGE_LEN;

   (void)memscpy(lang_tlv->lang_code,
          sizeof(lang_tlv->lang_code),
          data,
          GSTK_LANGUAGE_LEN);

   return GSTK_SUCCESS;
} /* gstk_packer_lang_sel_tlv */


/*===========================================================================
FUNCTION gstk_packer_date_time_tlv

DESCRIPTION

  This function populates the passed in the date/time/timezone tlv pointer

PARAMETERS
  data: [Input] Indicates the date, time and time zone
  time_tlv: [Input/Output] Pointers to which the time related info to be
                           populated

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
gstk_status_enum_type gstk_packer_date_time_tlv(
       const gstk_date_time_rsp_type   *data,
       gstk_date_time_tlv_type         *time_tlv
)
{
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
    int8 gstk_gw_tz = 0;
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/
#if defined(FEATURE_CDMA)
   int8 ltm_off = 0;
#endif /* FEATURE_CDMA */
   UIM_MSG_HIGH_0("** Packing date time");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(time_tlv == NULL)
   {
      UIM_MSG_ERR_0("Time TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   time_tlv->tag = (int)GSTK_DATE_TIME_AND_TIME_ZONE_TAG;
   time_tlv->length = GSTK_DATE_TIME_AND_TIME_ZONE_LEN;

   time_tlv->date_time.day = data->day;
   time_tlv->date_time.hour = data->hour;
   time_tlv->date_time.minute = data->minute;
   time_tlv->date_time.month = data->month;
   time_tlv->date_time.second = data->second;
   time_tlv->date_time.year = data->year;
   time_tlv->date_time.zone = data->zone;

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
    if (((gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_GSM)
         ||
         (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_UTRAN)
         ||
         (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_LTE))
        &&
        (data->zone != 0xFF))
    {
       gstk_gw_tz = (int8)data->zone;
       if (gstk_gw_tz < 0)
       {
         gstk_gw_tz = -gstk_gw_tz;
         time_tlv->date_time.zone = ((uint8)(gstk_gw_tz) / 10);
         time_tlv->date_time.zone += (((uint8)(gstk_gw_tz) % 10) << 4);
         time_tlv->date_time.zone |= 0x08;
       }
       else
       {
         time_tlv->date_time.zone = ((uint8)(gstk_gw_tz) / 10);
         time_tlv->date_time.zone += (((uint8)(gstk_gw_tz) % 10) << 4);
       }
       UIM_MSG_HIGH_1("GSM/WCDMA TP-Service-Centre-Time-Stamp is 0x%x",
                      time_tlv->date_time.zone);
    }
#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE */

#if defined(FEATURE_CDMA)
 /* Since the time zone is in 30 min intervals, it has to be converted to 15 min intervals
  * according to spec. Also the spec says that in the first of the two semi-octets,
  * the first bit (bit 3 of the seventh octet of TP Service Center Time stamp field
  * represents whether it is positive or negative (0 if positive, 1 if negative */
    if (((gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_CDMA)
         ||
         (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_HRPD)
         ||
         (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_EHRPD))
        &&
        (data->zone != 0xFF))
    {
      ltm_off = (int8)(data->zone & 0x3F);
      if (ltm_off >= 32)
      {
        ltm_off -= 64;
        ltm_off = (int8)(-(ltm_off * 2));
        time_tlv->date_time.zone = ((uint8)(ltm_off) / 10);
        time_tlv->date_time.zone += (((uint8)(ltm_off) % 10) << 4);
        time_tlv->date_time.zone |= 0x08;
      }
      else
      {
        ltm_off = (int8)(ltm_off*2);
        time_tlv->date_time.zone = ((uint8)(ltm_off) / 10);
        time_tlv->date_time.zone += (((uint8)(ltm_off) % 10) << 4);
      }
      UIM_MSG_HIGH_1("CDMA TP-Service-Centre-Time-Stamp is 0x%x",
                     time_tlv->date_time.zone);
    }
#endif /* FEATURE_CDMA */
  return GSTK_SUCCESS;

} /* gstk_packer_date_time_tlv */


/*===========================================================================
FUNCTION gstk_packer_address_tlv

DESCRIPTION

  This function populates the passed in the address tlv pointer

PARAMETERS
  tag: [Input] Indicates: ADDRESS_TAG or SS_STRING_TAG
  data: [Input] Indicates the address
  address_tlv: [Input/Output] Pointers to which the
                              gstk_address_tag_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_address_tlv(
       tags_enum_type            tag,
       const gstk_address_type   *data,
       gstk_address_tag_tlv_type *address_tlv)
{
  uint8 i        = 0;
  uint8 temp_ton = 0;
  uint8 lsb      = 0;
  uint8 msb      = 0;

  if(data == NULL)
  {
     UIM_MSG_ERR_0("** Packing address: Data Err: NULL");
     return GSTK_BAD_PARAM;
  }

  if(address_tlv == NULL)
  {
     UIM_MSG_ERR_0("** Packing address: Address TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }

  address_tlv->tag = (uint8)tag;
  address_tlv->length = 0;
  UIM_MSG_HIGH_3("** Packing address: TON=0x%x , NPI=0x%x, data->address[0]=0x%x",
                 data->TON, data->NPI, data->address[0]);

  if (data->length > 0) {
    temp_ton = int32touint8((int)data->TON << GSTK_TON_SHIFT_VALUE);
    i = 0;
    /* If international number then ignore '+'*/

    if(data->address[i] == 0x2B)
    {
      if(data->TON == GSTK_INTERNATIONAL_NUM || data->TON == GSTK_UNKNOWN_TON)
      {
        i++;

        if(data->TON == GSTK_UNKNOWN_TON)
        {
          temp_ton = int32touint8((int)GSTK_INTERNATIONAL_NUM << GSTK_TON_SHIFT_VALUE);
        }
      }
      else
      {
        UIM_MSG_ERR_0("Invalid Phone Number");
        return GSTK_ERROR;
      }
    }
    address_tlv->ton = temp_ton | ((int)data->NPI & GSTK_NPI_MASK_VALUE) | 0x80 ;

    while((i < data->length)
          &&
          (i < GSTK_MAX_ADDRESS_LEN)
          &&
          (address_tlv->length < GSTK_MAX_BCD_ADDRESS_LEN))
    {
      if((i + 1) < GSTK_MAX_ADDRESS_LEN && (i + 1) < data->length)
      {
        if ((data->address[i+1] >= 0x30) && (data->address[i+1] < 0x40))
        {
          msb = (uint8)((int32touint32(data->address[i+1]-0x30) << 0x04) & 0xF0);
        }
        else {
          switch (data->address[i+1]) {
            case 0x2A: /* * */
              msb = 0xA0;
              break;
            case 0x23: /* # */
              msb = 0xB0;
              break;
            default: /* passed in as 0x01 e.g. */
              msb = (uint8)(((data->address[i+1]) << 0x04) & 0xF0);
              break;
          }
        }
        if ((data->address[i] >= 0x30) && (data->address[i] < 0x40))
        {
          lsb = (uint8)((data->address[i]-0x30) & 0x0F);
        }
        else {
          switch (data->address[i]) {
            case 0x2A: /* * */
              lsb = 0x0A;
              break;
            case 0x23: /* # */
              lsb = 0x0B;
              break;
            default: /* passed in as 0x01 e.g. */
              lsb = (uint8)((data->address[i]) & 0x0F);
              break;
          }
        }
        address_tlv->address[address_tlv->length] =
            (uint8)(msb | lsb);
      }
      else { /* last digit, pad 0x0F */
        if ((data->address[i] >= 0x30) && (data->address[i] < 0x40))
        {
          lsb = (uint8)((data->address[i]-0x30) & 0x0F);
        }
        else {
          switch (data->address[i]) {
            case 0x2A: /* * */
              lsb = 0x0A;
              break;
            case 0x23: /* # */
              lsb = 0x0B;
              break;
            default: /* passed in as 0x01 e.g. */
              lsb = (uint8)((data->address[i]) & 0x0F);
              break;
          }
        }
        address_tlv->address[address_tlv->length] =
          0xF0 | lsb;
      }
      i+=2;
      address_tlv->length++;
    }
    if (address_tlv->length == GSTK_MAX_BCD_ADDRESS_LEN)
    {
      return GSTK_ERROR;
    }

    address_tlv->length++; /* add the TON byte */
  }
  return GSTK_SUCCESS;
} /* gstk_packer_address_tlv */


/*===========================================================================
FUNCTION gstk_packer_subaddress_tlv

DESCRIPTION

  This function populates the passed in the subaddress tlv pointer

PARAMETERS
  data: [Input] Indicates the subaddress
  subaddress_tlv: [Input/Output] Pointers to which the
                              gstk_subaddress_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_subaddress_tlv(
       const gstk_subaddress_type   *data,
       gstk_subaddress_tlv_type     *subaddress_tlv)
{

  UIM_MSG_HIGH_0("** Packing subaddress");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(subaddress_tlv == NULL)
   {
      UIM_MSG_ERR_0("Subaddress TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

  subaddress_tlv->tag = (int)GSTK_CALLED_PARTY_SUB_ADDRESS_TAG;
  subaddress_tlv->length = data->length;
  (void)memscpy(subaddress_tlv->subaddress,
         sizeof(subaddress_tlv->subaddress),
         data->subaddress, data->length);
  return GSTK_SUCCESS;
} /* gstk_packer_subaddress_tlv */


/*===========================================================================
FUNCTION gstk_packer_ccp_tlv

DESCRIPTION

  This function populates the passed in the ccp tlv pointer

PARAMETERS
  data: [Input] Indicates the ccp
  ccp_tlv: [Input/Output] Pointers to which the ccp tlv info to be
                          populated

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
gstk_status_enum_type gstk_packer_ccp_tlv(
       const gstk_cap_config_type   *data,
       gstk_cap_config_tlv_type     *ccp_tlv)
{

  UIM_MSG_HIGH_0("** Packing ccp");

  if(data == NULL)
  {
     UIM_MSG_ERR_0("Data Err: NULL");
     return GSTK_BAD_PARAM;
  }

  if(ccp_tlv == NULL)
  {
      UIM_MSG_ERR_0("CCP_TLV Err: NULL");
      return GSTK_BAD_PARAM;
  }

  ccp_tlv->tag = (int)GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG;
  ccp_tlv->length = data->length;
  (void)memscpy(ccp_tlv->ccp, sizeof(ccp_tlv->ccp), data->ccp, data->length);
  return GSTK_SUCCESS;
} /* gstk_packer_ccp_tlv */


/*===========================================================================
FUNCTION gstk_packer_bc_repeat_ind_tlv

DESCRIPTION

  This function populates the passed in the bc repeat indicator tlv pointer

PARAMETERS
  data: [Input] Indicates the bc repeat indicator
  ccp_tlv: [Input/Output] Pointers to which the bc repeat indicator tlv info
                          to be populated

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
gstk_status_enum_type gstk_packer_bc_repeat_ind_tlv(
       gstk_bc_repeat_ind_enum_type         data,
       gstk_bc_repeat_ind_tlv_type          *bc_repeat_ind_tlv)
{

  UIM_MSG_HIGH_0("** Packing bc repeat ind");

  if(bc_repeat_ind_tlv == NULL)
  {
     UIM_MSG_ERR_0("BC_REPEAT_IND TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }

  bc_repeat_ind_tlv->tag = (int)GSTK_BC_REPEAT_INDICATOR_TAG;
  bc_repeat_ind_tlv->length = GSTK_BC_REPEAT_IND_LEN;
  bc_repeat_ind_tlv->bc_repeat_indicator = (byte)data;

  return GSTK_SUCCESS;
} /* gstk_packer_bc_repeat_ind_tlv */


/*===========================================================================
FUNCTION gstk_packer_cc_req_action_tlv

DESCRIPTION

  This function populates the passed in the call control requested
  action tlv pointer

PARAMETERS
  data: [Input] Indicates the call control requested action
  cc_req_action_tlv: [Input/Output] Pointers to which the
                                    gstk_cc_req_action_tlv_type info to be
                                    populated

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
gstk_status_enum_type gstk_packer_cc_req_action_tlv(
       const gstk_cc_req_action_type     *data,
       gstk_cc_req_action_tlv_type       *cc_req_action_tlv)
{

  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  if(data == NULL)
  {
     UIM_MSG_ERR_0("** Packing CC req action: Data Err: NULL");
     return GSTK_BAD_PARAM;
  }
  if(cc_req_action_tlv == NULL)
  {
     UIM_MSG_ERR_0("** Packing CC req action: CC_REQ_ACTION TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }

  cc_req_action_tlv->tag = (int)GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG;
  cc_req_action_tlv->length = 0;

  UIM_MSG_HIGH_1("** Packing CC req action: CallType=0x%x", data->call_type);
  switch(data->call_type) {
  case GSTK_VOICE:
    if (data->gstk_address_string.voice_info.address.length != 0) {
      gstk_status = gstk_packer_address_tlv(GSTK_ADDRESS_TAG,
                               &data->gstk_address_string.voice_info.address,
                               &cc_req_action_tlv->gstk_address_string_tlv.address);
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("CC Req Err: Address error 0x%x", gstk_status);
        cc_req_action_tlv->length = 0;
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->gstk_address_string_tlv.address.length + 2;
    }
    if (data->gstk_address_string.voice_info.ccp1.length != 0) {
      gstk_status = gstk_packer_ccp_tlv(&data->gstk_address_string.voice_info.ccp1,
                             &cc_req_action_tlv->ccp1);
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("CC Req Err: CCP 1 error 0x%x", gstk_status);
        cc_req_action_tlv->length = 0;
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->ccp1.length
                                  + 2;
    }
    if (data->gstk_address_string.voice_info.ccp2.length != 0) {
      gstk_status = gstk_packer_ccp_tlv(&data->gstk_address_string.voice_info.ccp2,
                             &cc_req_action_tlv->ccp2);
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("CC Req Err: CCP 2 error 0x%x", gstk_status);
        cc_req_action_tlv->length = 0;
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->ccp2.length
                                  + 2;
      /* bc repeat indicator is present if cpp2 is presented */
      gstk_status = gstk_packer_bc_repeat_ind_tlv(data->bc_repeat_ind,
                                       &cc_req_action_tlv->bc_repeat_ind);
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("CC Req Err: BC Repeat Ind error 0x%x", gstk_status);
        cc_req_action_tlv->length = 0;
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->bc_repeat_ind.length
                                  + 2;
    }
    if (data->gstk_address_string.voice_info.subaddress.length != 0) {
      gstk_status = gstk_packer_subaddress_tlv(&data->gstk_address_string.voice_info.subaddress,
                                &cc_req_action_tlv->subaddress);
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("CC Req Err: Subaddress error 0x%x", gstk_status);
        cc_req_action_tlv->length = 0;
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->subaddress.length
                                  + 2;
    }
    break;
  case GSTK_USSD:
    if (data->gstk_address_string.ussd_string.length != 0) {
      uint8                 length     = data->gstk_address_string.ussd_string.length;
      uint8                 dcs        = data->gstk_address_string.ussd_string.orig_dcs_from_sim;
      uint8                *ussd_str   = data->gstk_address_string.ussd_string.text;
      gstk_dcs_enum_type    gstk_dcs   = gstk_util_decode_dcs_type(TRUE, dcs);

      if(gstk_dcs == GSTK_DCS_7_BIT)
      {
        ussd_str  = (uint8 *)GSTK_CALLOC(data->gstk_address_string.ussd_string.length);
        if(ussd_str == NULL)
        {
          return GSTK_MEMORY_ERROR;
        }
        length = (uint8)gstk_util_pack_7_bit_chars(data->gstk_address_string.ussd_string.text,
                                                                    data->gstk_address_string.ussd_string.length,
                                                                    ussd_str);
      }
      /* malloc the space for ussd string */
      cc_req_action_tlv->gstk_address_string_tlv.ussd_string.text =
          GSTK_CALLOC(length);
      if (cc_req_action_tlv->gstk_address_string_tlv.ussd_string.text != NULL) {
        gstk_status = gstk_packer_text_string_tlv(GSTK_USSD_STRING_TAG,
                                     dcs,
                                     length,
                                     ussd_str,
                                     &cc_req_action_tlv->gstk_address_string_tlv.ussd_string);
        if(gstk_dcs == GSTK_DCS_7_BIT && ussd_str != NULL)
        {
          gstk_free(ussd_str);
        }
        if(gstk_status != GSTK_SUCCESS) {
          UIM_MSG_ERR_1("CC Req Err: USSD Text error 0x%x", gstk_status);
          cc_req_action_tlv->length = 0;
          return gstk_status;
        }
        /* length for CC request action */
        cc_req_action_tlv->length = cc_req_action_tlv->length +
                                    cc_req_action_tlv->gstk_address_string_tlv.ussd_string.length
                                    + 2;
      }
      else {
        if(gstk_dcs == GSTK_DCS_7_BIT && ussd_str != NULL)
        {
          gstk_free(ussd_str);
        }
        return GSTK_MEMORY_ERROR;
      }
    }
    break;
  case GSTK_SS:
    if (data->gstk_address_string.ss_string.length != 0) {
      gstk_status = gstk_packer_address_tlv(GSTK_SS_STRING_TAG,
                               &data->gstk_address_string.ss_string,
                               &cc_req_action_tlv->gstk_address_string_tlv.ss_string);
      if(gstk_status != GSTK_SUCCESS) {
        UIM_MSG_ERR_1("CC Req Err: SS String error 0x%x", gstk_status);
        cc_req_action_tlv->length = 0;
        return gstk_status;
      }
      /* length for CC request action */
      cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->gstk_address_string_tlv.ss_string.length
                                  + 2;
    }
    break;
  default:
    UIM_MSG_ERR_0("CC Req Err: unkown call type");
    return GSTK_ERROR;
  }

  /* alpha */
  if (data->alpha.length != 0) {
    gstk_status  = gstk_packer_alpha_tlv(&data->alpha,
                             &cc_req_action_tlv->alpha);
    if(gstk_status != GSTK_SUCCESS) {
      UIM_MSG_ERR_1("CC Req Err: Alpha error 0x%x", gstk_status);
      cc_req_action_tlv->length = 0;
      return gstk_status;
    }
    /* length for CC request action */
    cc_req_action_tlv->length = cc_req_action_tlv->length +
                                  cc_req_action_tlv->alpha.length
                                  + 2;
  }
  return GSTK_SUCCESS;
} /* gstk_packer_cc_req_action_tlv*/


/*===========================================================================
FUNCTION gstk_packer_duration_tlv

DESCRIPTION

  This function populates the passed in duration tlv pointer

PARAMETERS
  data: [Input] Indicates the duration
  duration_tlv: [Input/Output] Pointers to which the duration
                                    to be populated

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
gstk_status_enum_type gstk_packer_duration_tlv(
       const gstk_duration_type    *data,
       gstk_duration_tlv_type      *duration_tlv
)
{

   UIM_MSG_HIGH_0("** Packing duration");

   if(data == NULL)
   {
      UIM_MSG_ERR_0("Data Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if(duration_tlv == NULL)
   {
      UIM_MSG_ERR_0("Duration TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   duration_tlv->tag = (int)GSTK_DURATION_TAG;
   duration_tlv->length = GSTK_DURATION_LEN;

   duration_tlv->time_interval = data->length;
   duration_tlv->time_unit = data->units;

   return GSTK_SUCCESS;
} /* gstk_packer_duration_tlv */


/*===========================================================================
FUNCTION gstk_packer_item_id_tlv

DESCRIPTION

  This function populates the passed in the item id tlv pointer

PARAMETERS
  chosen_item_identifier: [Input] Indicates the item id
  item_id_tlv: [Input/Output] Pointers to which the item id info to be
                           populated

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
gstk_status_enum_type gstk_packer_item_id_tlv(
       uint8                            chosen_item_identifier,
       gstk_item_identifier_tlv_type   *item_id_tlv
)
{
   UIM_MSG_HIGH_0("** Packing item id");

   if(item_id_tlv == NULL)
   {
      UIM_MSG_ERR_0("ITEM ID TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }
   item_id_tlv->tag = (int)GSTK_ITEM_IDENTIFIER_TAG;
   item_id_tlv->length = GSTK_ITEM_IDENTIFIER_LEN;

   item_id_tlv->identifier = chosen_item_identifier;


   return GSTK_SUCCESS;
} /* gstk_packer_item_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_help_request_tlv

DESCRIPTION

  This function populates the passed in the help request tlv pointer

PARAMETERS
  data: [Input] Indicates the help requested info
  help_tlv: [Input/Output] Pointers to which the help request related info
                           to be populated

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
gstk_status_enum_type gstk_packer_help_request_tlv(
       boolean                          data,
       gstk_help_request_tlv_type       *help_tlv
)
{
   UIM_MSG_HIGH_0("** Packing help request");

   if(help_tlv == NULL)
   {
      UIM_MSG_ERR_0("Help TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

   if (data) {
     help_tlv->tag = (int)GSTK_HELP_REQUEST_TAG;
     help_tlv->length = GSTK_HELP_REQUEST_LEN;
   }
   else {
     help_tlv->length = 0;
   }

   return GSTK_SUCCESS;
} /* gstk_packer_help_request_tlv */


/*===========================================================================
FUNCTION gstk_packer_transaction_id_tlv

DESCRIPTION

  This function populates the passed in the transaction id tlv pointer

PARAMETERS
  data: [Input] Indicates the transaction id
  transaction_id_tlv: [Input/Output] Pointers to which the
                              gstk_transaction_id_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_transaction_id_tlv(
       const gstk_transaction_id_type   *data,
       gstk_transaction_id_tlv_type     *transaction_id_tlv)
{

  UIM_MSG_HIGH_0("** Packing transaction id");

   if(data == NULL)
   {
     UIM_MSG_ERR_0("Data Err: NULL");
     return GSTK_BAD_PARAM;
   }

   if(transaction_id_tlv == NULL)
   {
      UIM_MSG_ERR_0("TRANSACTION ID TLV Err: NULL");
      return GSTK_BAD_PARAM;
   }

  transaction_id_tlv->tag = (int)GSTK_TRANSACTION_IDENTIFIER_TAG;
  transaction_id_tlv->length = data->length;
  if(data->length > 0) {
    if(transaction_id_tlv->transaction_list != NULL) {
      (void)memscpy(transaction_id_tlv->transaction_list,
              data->length,
              data->transaction_list, data->length);
    }
    else {
      UIM_MSG_ERR_0("Trans ID Err: Trans List Null ptr");
      return GSTK_MEMORY_ERROR;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_packer_transaction_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_evt_list_tlv

DESCRIPTION

  This function populates the passed in the event list tlv pointer

PARAMETERS
  data: [Input] Indicates one event list
  evt_list_tlv: [Input/Output] Pointers to which the
                              gstk_evt_list_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_evt_list_tlv(
       gstk_evt_list_code_enum_type    data,
       gstk_evt_list_tlv_type*         evt_list_tlv)
{

  UIM_MSG_HIGH_0("** Packing evt list");

  if(evt_list_tlv == NULL)
  {
     UIM_MSG_ERR_0("EVT LIST TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }

  evt_list_tlv->tag = (int)GSTK_EVENT_LIST_TAG;
  evt_list_tlv->length = 1;
  /* only 1 event */
  evt_list_tlv->event_list[0] = (uint8)data;

  return GSTK_SUCCESS;
} /* gstk_packer_evt_list_tlv */


/*===========================================================================
FUNCTION gstk_packer_cause_tlv

DESCRIPTION

  This function populates the passed in the cause tlv pointer

PARAMETERS
  data: [Input] Indicates the cause
  cause_tlv: [Input/Output] Pointers to which the
                              gstk_cause_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_cause_tlv(
       const gstk_cause_type   *data,
       gstk_cause_tlv_type     *cause_tlv)
{

  UIM_MSG_HIGH_0("** Packing cause");

  if(data == NULL)
  {
     UIM_MSG_ERR_0("Data Err: NULL");
     return GSTK_BAD_PARAM;
  }

  if(cause_tlv == NULL)
  {
     UIM_MSG_ERR_0("CAUSE TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }
  cause_tlv->tag = (int)GSTK_CAUSE_TAG;
  cause_tlv->length = data->length;
  if(data->length > 0) {
    if(cause_tlv->cause != NULL) {
      (void)memscpy(cause_tlv->cause, data->length, data->cause, data->length);
    }
    else {
      UIM_MSG_ERR_0("Cause Err: Cause Null ptr");
      return GSTK_MEMORY_ERROR;
    }
  }
  return GSTK_SUCCESS;
} /* gstk_packer_cause_tlv */


/*===========================================================================
FUNCTION gstk_packer_location_status_tlv

DESCRIPTION

  This function populates the passed in the location status tlv pointer

PARAMETERS
  data: [Input] Indicates the location status
  location_status_tlv: [Input/Output] Pointers to which the
                              gstk_location_status_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_location_status_tlv(
       gstk_location_status_enum_type         data,
       gstk_location_status_tlv_type          *location_status_tlv)
{
  UIM_MSG_HIGH_0("** Packing location status");

  if(location_status_tlv == NULL)
  {
     UIM_MSG_ERR_0("Location status TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }
  location_status_tlv->tag = (int)GSTK_LOCATION_STATUS_TAG;
  location_status_tlv->length = GSTK_LOCATION_STATUS_LEN;

  location_status_tlv->location_status = (uint8)data;

  return GSTK_SUCCESS;
} /* gstk_packer_location_status_tlv */


/*===========================================================================
FUNCTION gstk_packer_sms_tpdu_tlv

DESCRIPTION

  This function populates the passed in the sms tpdu tlv pointer

PARAMETERS
  is_cdma_tpdu: [Input] Indicates whether the tag should be CDMA or GSM/WCDMA
                        TPDU tag
  data: [Input] Indicates the sms tpdu
  tpdu_tlv: [Input/Output] Pointers to which the
                              gstk_sms_tpdu_tag_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_sms_tpdu_tlv(
       boolean                             is_cdma_tpdu,
       const gstk_sms_tpdu_type            *data,
       gstk_sms_tpdu_tag_tlv_type          *tpdu_tlv)
{
  UIM_MSG_HIGH_0("** Packing sms tpdu");

  if(data == NULL)
  {
     UIM_MSG_ERR_0("Data Err: NULL");
     return GSTK_BAD_PARAM;
  }

  if(tpdu_tlv == NULL)
  {
     UIM_MSG_ERR_0("TPDU TLV Err: NULL");
     return GSTK_BAD_PARAM;
  }

  if(is_cdma_tpdu)
  {
    tpdu_tlv->tag = (uint8)GSTK_CDMA_SMS_TPDU_TAG;
  }
  else
  {
    tpdu_tlv->tag = (int)GSTK_SMS_TPDU_TAG;
  }
  tpdu_tlv->length = data->length;

  if(gstk_memcpy(tpdu_tlv->sms_tpdu,
                 data->tpdu,
                 data->length,
                 GSTK_MAX_RAW_SMS_LEN,
                 data->length) <
     data->length)
  {
    UIM_MSG_ERR_0("gstk_packer_sms_tpdu_tlv(): gstk_memcpy failed");
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_packer_sms_tpdu_tlv */


/*===========================================================================
FUNCTION gstk_packer_browser_term_cause_tlv

DESCRIPTION

  This function populates the passed in browser termination cause tlv pointer

PARAMETERS
  cause: [Input] Indicates the termination cause
  cause_tlv: [Input/Output] Pointers to which the
                              gstk_browser_term_cause_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_browser_term_cause_tlv(
         const gstk_browser_term_cause_enum_type  *cause,
         gstk_browser_term_cause_tlv_type         *cause_tlv)
{
  UIM_MSG_HIGH_0("** Packing browser termination cause");

  if ((cause_tlv == NULL) || (cause == NULL)) {
    UIM_MSG_ERR_0("Browser Term Cause Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  cause_tlv->tag = (int)GSTK_BROWSER_TERMINATION_CAUSE_TAG;
  cause_tlv->length = GSTK_BROWSER_TERMINATION_CAUSE_LEN;
  if ((*cause != GSTK_BROWSER_USER_TERMINATED) && (*cause != GSTK_BROWSER_ERROR_TERMINATED))
  {
    UIM_MSG_ERR_1("Browser Term Cause Err: unknown cause 0x%x", *cause);
    return GSTK_ERROR;
  }
  cause_tlv->cause = (uint8)(*cause);
  return GSTK_SUCCESS;
} /* gstk_packer_browser_term_cause_tlv */


/*===========================================================================
FUNCTION gstk_packer_timer_id_tlv

DESCRIPTION

  This function populates the passed in timer id tlv pointer

PARAMETERS
  timer_id: [Input] Indicates the timer id
  timer_id_tlv: [Input/Output] Pointers to which the
                              gstk_timer_id_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_timer_id_tlv(
         const uint8            *timer_id,
         gstk_timer_id_tlv_type *timer_id_tlv)
{
  UIM_MSG_HIGH_0("** Packing timer id");
  if ((timer_id_tlv == NULL) || (timer_id == NULL)) {
    UIM_MSG_ERR_0("Timer ID Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  timer_id_tlv->tag = (int)GSTK_TIMER_IDENTIFIER_TAG;
  timer_id_tlv->length = GSTK_TIMER_IDENTITY_LEN;
  timer_id_tlv->timer_id = (uint8)(*timer_id);
  return GSTK_SUCCESS;
} /* gstk_packer_timer_id_tlv */


/*===========================================================================
FUNCTION gstk_packer_timer_value_tlv

DESCRIPTION

  This function populates the passed in timer value tlv pointer

PARAMETERS
  timer_value: [Input] Indicates the timer value
  timer_value_tlv: [Input/Output] Pointers to which the
                              gstk_timer_value_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_timer_value_tlv(
         const gstk_timer_value_type *timer_value,
         gstk_timer_value_tlv_type   *timer_value_tlv)
{
  UIM_MSG_HIGH_0("** Packing timer value");
  if ((timer_value == NULL) || (timer_value_tlv == NULL))
  {
    UIM_MSG_ERR_0("Timer ID Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  timer_value_tlv->tag = (int)GSTK_TIMER_VALUE_TAG;
  timer_value_tlv->length = GSTK_TIMER_VALUE_LEN;

  timer_value_tlv->timer_value.hour = (timer_value->hour / 10) |
                                      ((timer_value->hour % 10) << 0x04);
  timer_value_tlv->timer_value.minute = (timer_value->minute / 10) |
                                        ((timer_value->minute % 10) << 0x04);
  timer_value_tlv->timer_value.second = (timer_value->second / 10) |
                                        ((timer_value->second % 10) << 0x04);
  return GSTK_SUCCESS;
} /* gstk_packer_timer_value_tlv */


/*===========================================================================
FUNCTION gstk_packer_cell_broadcast_page_tlv

DESCRIPTION

  This function populates the passed in cell broadcast page tlv pointer

PARAMETERS
  cell cb_page: [Input] Indicates the cell broadcast page value
  cb_page_tlv: [Input/Output] Pointers to which the
                              gstk_cell_broadcast_page_tag_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_cell_broadcast_page_tlv(
         const gstk_cb_page_type                *cb_page,
         gstk_cell_broadcast_page_tag_tlv_type  *cb_page_tlv)
{

  UIM_MSG_HIGH_0("** Packing cell broadcast page");
  if ((cb_page == NULL) || (cb_page_tlv == NULL))
  {
    UIM_MSG_ERR_0("Cell Broadcast Page Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  cb_page_tlv->tag = (int)GSTK_CELL_BROADCAST_PAGE_TAG;
  cb_page_tlv->length = GSTK_CELL_BROADCAST_PAGE_LEN;

  cb_page_tlv->cb_page = GSTK_CALLOC(GSTK_CELL_BROADCAST_PAGE_LEN);

  if (cb_page_tlv->cb_page == NULL) {
    return GSTK_MEMORY_ERROR;
  }
  (void)memscpy(cb_page_tlv->cb_page, GSTK_CELL_BROADCAST_PAGE_LEN, cb_page->cb_page_data, GSTK_CELL_BROADCAST_PAGE_LEN);

  return GSTK_SUCCESS;
} /* gstk_packer_timer_value_tlv */


/*===========================================================================
FUNCTION gstk_packer_ch_status_tlv

DESCRIPTION

  This function populates the passed in channel status tlv pointer

PARAMETERS
  ch_status: [Input] Indicates the channel status value
  ch_status_tlv: [Input/Output] Pointers to which the
                              gstk_ch_status_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_ch_status_tlv(
         const gstk_ch_status_type        *ch_status,
         gstk_ch_status_tlv_type          *ch_status_tlv)
{

  UIM_MSG_HIGH_0("** Packing Channel Status");
  if ((ch_status == NULL) || (ch_status_tlv == NULL))
  {
    UIM_MSG_ERR_0("Channel Status Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }

  /* Channel Status TLV Tag */
  ch_status_tlv->tag = (int)GSTK_CHANNEL_STATUS_TAG;

  /* Channel Status TLV Length */
  ch_status_tlv->ch_status_length = GSTK_CHANNEL_STATUS_LEN;

  /* Populate the first byte of ch_status_tlv->ch_status with channel
     id and link established boolean */
  if (ch_status->is_scws_ch)
  {
    ch_status_tlv->ch_status[0] = (((uint8)ch_status->scws_ch_status << 6) & 0xC0) | (((uint8)ch_status->ch_id) & 0x7);
  }
  else if(ch_status->is_link_est) {
    ch_status_tlv->ch_status[0] = 0x80 | (((uint8)ch_status->ch_id) & 0x7);
  }
  else {
    ch_status_tlv->ch_status[0] = ((uint8)ch_status->ch_id) & 0x7;
  }

  /* Populate the second byte of ch_status_tlv->ch_status with
     additional information */
  switch (ch_status->ch_status_additional_info) {
    case GSTK_CH_STATUS_NO_FURTHER_INFO:
      ch_status_tlv->ch_status[1] = 0;
      break;
    case GSTK_CH_STATUS_LINK_DROPPED:
      ch_status_tlv->ch_status[1] = 5;
      break;
    default:
      ch_status_tlv->ch_status[1] = 1;
      break;
  }
  return GSTK_SUCCESS;
} /* gstk_packer_ch_status_tlv */


/*===========================================================================
FUNCTION gstk_packer_csd_bearer_param

DESCRIPTION
  This function populates the csd bearer information.

PARAMETERS
  gstk_csd_bearer_param_type gstk_bearer -  conatins bearer parameters
  uint8                     *csd_bearer  -  pointer holds csd bearer
                                            information.

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
static gstk_status_enum_type gstk_packer_csd_bearer_param(
  gstk_csd_bearer_param_type gstk_bearer,
  uint8                     *csd_bearer)
{

  /* Allocate memory for csd bearer pointer */
  if (csd_bearer == NULL) {
    UIM_MSG_ERR_0("csd_bearer is NULL");
    return GSTK_BAD_PARAM;
  }

  /* Determine bearer speed information */
  switch (gstk_bearer.speed) {
  case GSTK_CSD_BEARER_SPEED_AUTO:
    csd_bearer[0] = 0;
    break;
  case GSTK_CSD_BEARER_SPEED_300_V21:
    csd_bearer[0] = 1;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_V22:
    csd_bearer[0] = 2;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_75_V23:
    csd_bearer[0] = 3;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V22:
    csd_bearer[0] = 4;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V26:
    csd_bearer[0] = 5;
    break;
  case GSTK_CSD_BEARER_SPEED_4800_V32:
    csd_bearer[0] = 6;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V32:
    csd_bearer[0] = 7;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V34:
    csd_bearer[0] = 12;
    break;
  case GSTK_CSD_BEARER_SPEED_14400_V34:
    csd_bearer[0] = 14;
    break;
  case GSTK_CSD_BEARER_SPEED_19200_V34:
    csd_bearer[0] = 15;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_V34:
    csd_bearer[0] = 16;
    break;
  case GSTK_CSD_BEARER_SPEED_33600_V34:
    csd_bearer[0] = 17;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_V120:
    csd_bearer[0] = 34;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V120:
    csd_bearer[0] = 36;
    break;
  case GSTK_CSD_BEARER_SPEED_4800_V120:
    csd_bearer[0] = 38;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V120:
    csd_bearer[0] = 39;
    break;
  case GSTK_CSD_BEARER_SPEED_14400_V120:
    csd_bearer[0] = 43;
    break;
  case GSTK_CSD_BEARER_SPEED_19200_V120:
    csd_bearer[0] = 47;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_V120:
    csd_bearer[0] = 48;
    break;
  case GSTK_CSD_BEARER_SPEED_38400_V120:
    csd_bearer[0] = 49;
    break;
  case GSTK_CSD_BEARER_SPEED_48000_V120:
    csd_bearer[0] = 50;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_V120:
    csd_bearer[0] = 51;
    break;
  case GSTK_CSD_BEARER_SPEED_300_V110:
    csd_bearer[0] = 65;
    break;
  case GSTK_CSD_BEARER_SPEED_1200_V110:
    csd_bearer[0] = 66;
    break;
  case GSTK_CSD_BEARER_SPEED_2400_V110_X31:
    csd_bearer[0] = 68;
    break;
  case GSTK_CSD_BEARER_SPEED_4800_V110_X31:
    csd_bearer[0] = 70;
    break;
  case GSTK_CSD_BEARER_SPEED_9600_V110_X31:
    csd_bearer[0] = 71;
    break;
  case GSTK_CSD_BEARER_SPEED_14400_V110_X31:
    csd_bearer[0] = 75;
    break;
  case GSTK_CSD_BEARER_SPEED_19200_V110_X31:
    csd_bearer[0] = 79;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_V110_X31:
    csd_bearer[0] = 80;
    break;
  case GSTK_CSD_BEARER_SPEED_38400_V110_X31:
    csd_bearer[0] = 81;
    break;
  case GSTK_CSD_BEARER_SPEED_48000_V110_X31:
    csd_bearer[0] = 82;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_V110_X31:
    csd_bearer[0] = 83;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_V110_X31:
    csd_bearer[0] = 84;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_BIT_TRANSPARENT:
    csd_bearer[0] = 115;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_BIT_TRANSPARENT:
    csd_bearer[0] = 116;
    break;
  case GSTK_CSD_BEARER_SPEED_32000_PIAFS32K:
    csd_bearer[0] = 120;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_PIAFS32K:
    csd_bearer[0] = 121;
    break;
  case GSTK_CSD_BEARER_SPEED_28800_MULTIMEDIA:
    csd_bearer[0] = 130;
    break;
  case GSTK_CSD_BEARER_SPEED_32000_MULTIMEDIA:
    csd_bearer[0] = 131;
    break;
  case GSTK_CSD_BEARER_SPEED_33600_MULTIMEDIA:
    csd_bearer[0] = 132;
    break;
  case GSTK_CSD_BEARER_SPEED_56000_MULTIMEDIA:
    csd_bearer[0] = 133;
    break;
  case GSTK_CSD_BEARER_SPEED_64000_MULTIMEDIA:
    csd_bearer[0] = 134;
    break;
  default:
    UIM_MSG_ERR_1("CSD_Bearer speed from Client 0x%x", gstk_bearer.speed);
    csd_bearer[0] = 0xFF;
    break;
  }

  /* Determine bearer name */
  switch(gstk_bearer.name) {
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_UDI_3_1KHZ:
    csd_bearer[1] = 0;
    break;
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_UDI_3_1KHZ:
    csd_bearer[1] = 1;
    break;
  case GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_UDI:
    csd_bearer[1] = 2;
    break;
  case GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNCH_UDI:
    csd_bearer[1] = 3;
    break;
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_ASYNCH_RDI:
    csd_bearer[1] = 4;
    break;
  case GSTK_CSD_BEARER_NAME_DATA_CIRCUIT_SYNCH_RDI:
    csd_bearer[1] = 5;
    break;
  case GSTK_CSD_BEARER_NAME_PAD_ACCESS_ASYNC_RDI:
    csd_bearer[1] = 6;
    break;
  case GSTK_CSD_BEARER_NAME_PACKET_ACCESS_SYNC_RDI:
    csd_bearer[1] = 7;
    break;
  default:
    UIM_MSG_ERR_1("CSD_Bearer name from Client 0x%x", gstk_bearer.name);
    csd_bearer[1] = 0xFF;
    break;
  }

  /* Determine bearer connection element */
  switch(gstk_bearer.connection_element) {
  case GSTK_CSD_BEARER_CE_TRANSPARENT:
    csd_bearer[2] = 0;
    break;
  case GSTK_CSD_BEARER_CE_NON_TRANSPARENT:
    csd_bearer[2] = 1;
    break;
  case GSTK_CSD_BEARER_CE_BOTH_TRANSPARENT_PREF:
    csd_bearer[2] = 2;
    break;
  case GSTK_CSD_BEARER_CE_BOTH_NON_TRANSPARENT_PREF:
    csd_bearer[2] = 3;
    break;
  default:
    UIM_MSG_ERR_1("CSD_Bearer connection element from Client 0x%x",
                  gstk_bearer.connection_element);
    csd_bearer[2] = 0xFF;
    break;
  }
  return GSTK_SUCCESS;
}  /* gstk_packer_csd_bearer_param */


/*===========================================================================
FUNCTION gstk_packer_gprs_bearer_param

DESCRIPTION
  This function populates the gprs bearer information.

PARAMETERS
  gstk_gprs_bearer_param_type gstk_bearer - conatins bearer parameters
  uint8                     *csd_bearer  -  pointer holds csd bearer
                                            information.

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
static gstk_status_enum_type gstk_packer_gprs_bearer_param(
  gstk_gprs_bearer_param_type gstk_bearer,
  uint8                       *gprs_bearer)
{
  /* Allocate memory for the gprs_bearer pointer */
  if (gprs_bearer == NULL) {
    UIM_MSG_ERR_0("gprs_bearer is NULL");
    return GSTK_BAD_PARAM;
  }

  /* Fill in Bearer Description information
  1. Precedence class
  2. Delay class
  3. Reliability class
  4. Peak throughput class
  5. Mean throughput class
  6. Packet Data Protocol type
  */

  gprs_bearer[0] = int32touint8(gstk_bearer.precedence_cls);
  gprs_bearer[1] = int32touint8(gstk_bearer.delay_cls);
  gprs_bearer[2] = int32touint8(gstk_bearer.reliability_cls);
  gprs_bearer[3] = int32touint8(gstk_bearer.peak_throughput);
  gprs_bearer[4] = int32touint8(gstk_bearer.mean_throughput);
  switch(gstk_bearer.pkt_data_protocol) {
  case GSTK_PKT_DATA_IP:
    gprs_bearer[5] = 0x02;
    break;
  default:
    UIM_MSG_ERR_1("GPRS_Bearear from Client 0x%x",
                  gstk_bearer.pkt_data_protocol);
    gprs_bearer[5] = 0xFF;
    break;
  }
  return GSTK_SUCCESS;
}  /* gstk_packer_gprs_bearer_param */


/*===========================================================================
FUNCTION gstk_packer_bearer_description_tlv

DESCRIPTION

  This function populates the passed in bearer description tlv pointer

PARAMETERS
  bearer_description: [Input] Indicates the bearer description value
  bearer_description_tlv: [Input/Output] Pointers to which the
                              gstk_bearer_description_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_bearer_description_tlv(
         const gstk_bearer_description_type  *bearer_description,
         gstk_bearer_description_tlv_type    *bearer_description_tlv)
{
  gstk_status_enum_type gstk_status = GSTK_ERROR;
  uint16                val         = 0;
  uint8                 bearer_param_len = 0;

  UIM_MSG_HIGH_0("** Packing Bearer Description");
  if ((bearer_description == NULL) || (bearer_description_tlv == NULL))
  {
    UIM_MSG_ERR_0("Bearer Description Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  bearer_description_tlv->tag = (int)GSTK_BEARER_DESCRIPTION_TAG;

  switch(bearer_description->bearer_type) {
    case GSTK_CSD:
      bearer_description_tlv->bearer_type = 0x01;
      bearer_description_tlv->bearer_description_length = GSTK_CSD_BEARER_PARAM_LEN + 1;
      bearer_description_tlv->bearer_param = GSTK_CALLOC(bearer_description_tlv->bearer_description_length-1);
      gstk_status = gstk_packer_csd_bearer_param(bearer_description->bearer_params.csd_bearer, bearer_description_tlv->bearer_param);
      break;
    case GSTK_GPRS:
      bearer_description_tlv->bearer_type = 0x02;
      bearer_description_tlv->bearer_description_length = GSTK_GPRS_BEARER_PARAM_LEN + 1;
      bearer_description_tlv->bearer_param = GSTK_CALLOC(bearer_description_tlv->bearer_description_length-1);
      gstk_status = gstk_packer_gprs_bearer_param(bearer_description->bearer_params.gprs_bearer, bearer_description_tlv->bearer_param);
      break;
    case GSTK_BEARER_DEFAULT:
      bearer_description_tlv->bearer_type = 0x03;
      bearer_description_tlv->bearer_description_length = 1;
      bearer_description_tlv->bearer_param = NULL;
      gstk_status = GSTK_SUCCESS;
      break;
    case GSTK_EUTRAN_EXT_PARAM:
      bearer_description_tlv->bearer_type = 0x09;
      bearer_description_tlv->bearer_description_length =
        GSTK_EUTRAN_EXT_PARAM_LEN + 1;
      bearer_description_tlv->bearer_param =
        GSTK_CALLOC(GSTK_EUTRAN_EXT_PARAM_LEN);
      if (bearer_description_tlv->bearer_param)
      {
        gstk_status = GSTK_SUCCESS;
        bearer_description_tlv->bearer_param[0] =
          bearer_description->bearer_params.eutran_ext_bearer.traffic_class;
        val =
          uint32touint16(bearer_description->bearer_params.eutran_ext_bearer.max_bitrate_ul);
        bearer_description_tlv->bearer_param[1] =
          uint16touint8(val & 0x00FF);
        bearer_description_tlv->bearer_param[2] =
          uint16touint8((val & 0xFF00) >> 8);
        val =
          uint32touint16(bearer_description->bearer_params.eutran_ext_bearer.max_bitrate_dl);
        bearer_description_tlv->bearer_param[3] =
          uint16touint8(val & 0x00FF);
        bearer_description_tlv->bearer_param[4] =
          uint16touint8((val & 0xFF00) >> 8);
        val =
          uint32touint16(bearer_description->bearer_params.eutran_ext_bearer.guaranteed_bitrate_ul);
        bearer_description_tlv->bearer_param[5] =
          uint16touint8(val & 0x00FF);
        bearer_description_tlv->bearer_param[6] =
          uint16touint8((val & 0xFF00) >> 8);
        val =
          uint32touint16(bearer_description->bearer_params.eutran_ext_bearer.guaranteed_bitrate_dl);
        bearer_description_tlv->bearer_param[7] =
          uint16touint8(val & 0x00FF);
        bearer_description_tlv->bearer_param[8] =
          uint16touint8((val & 0xFF00) >> 8);
        bearer_description_tlv->bearer_param[9] =
          bearer_description->bearer_params.eutran_ext_bearer.delivery_order;
        bearer_description_tlv->bearer_param[10] =
          uint32touint8(bearer_description->bearer_params.eutran_ext_bearer.max_sdu_size);
        bearer_description_tlv->bearer_param[11] =
          bearer_description->bearer_params.eutran_ext_bearer.sdu_error_ratio;
        bearer_description_tlv->bearer_param[12] =
          bearer_description->bearer_params.eutran_ext_bearer.residual_bit_error_ratio;
        bearer_description_tlv->bearer_param[13] =
          bearer_description->bearer_params.eutran_ext_bearer.delivery_of_err_sdu;
        bearer_description_tlv->bearer_param[14] =
          uint32touint8(bearer_description->bearer_params.eutran_ext_bearer.transfer_delay);
        bearer_description_tlv->bearer_param[15] =
          uint32touint8(bearer_description->bearer_params.eutran_ext_bearer.traffic_handling_pri);
        bearer_description_tlv->bearer_param[16] =
          bearer_description->bearer_params.eutran_ext_bearer.pdp_type;
      }
      else
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      break;
    case GSTK_EUTRAN_MAPPED_UTRAN_PS:
      bearer_description_tlv->bearer_type = 0x0B;
      UIM_MSG_HIGH_1("QCI is %d",
                     bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.qci);
      if(bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.qci >= GSTK_EUTRAN_MAPPED_UTRAN_QCI_5_NON_GBR
         &&  bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.qci <= GSTK_EUTRAN_MAPPED_UTRAN_QCI_9_NON_GBR)
      {
        bearer_param_len = 2; /* Only QCI and PDP Type */
      }
      else
      {
        bearer_param_len = (uint8)GSTK_EUTRAN_MAPPED_UTRAN_PS_PARAM_LEN;
      }
      bearer_description_tlv->bearer_description_length =
        bearer_param_len + 1; /* Bearer Param + Bearer Type */
      bearer_description_tlv->bearer_param =
        GSTK_CALLOC(bearer_param_len);
      if (bearer_description_tlv->bearer_param)
      {
        gstk_status = GSTK_SUCCESS;
        bearer_description_tlv->bearer_param[0] =
          bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.qci;

        if(bearer_param_len == GSTK_EUTRAN_MAPPED_UTRAN_PS_PARAM_LEN) /* GBR */
        {
          bearer_description_tlv->bearer_param[1] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_ul;
          bearer_description_tlv->bearer_param[2] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_dl;
          bearer_description_tlv->bearer_param[3] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_ul;
          bearer_description_tlv->bearer_param[4] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_dl;
          bearer_description_tlv->bearer_param[5] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_ul_ext;
          bearer_description_tlv->bearer_param[6] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.max_bitrate_dl_ext;
          bearer_description_tlv->bearer_param[7] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_ul_ext;
          bearer_description_tlv->bearer_param[8] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.guaranteed_bitrate_dl_ext;
          bearer_description_tlv->bearer_param[9] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.pdp_type;
        }
        else /* non-GBR */
        {
          bearer_description_tlv->bearer_param[1] =
            bearer_description->bearer_params.eutran_mapped_utran_ps_bearer.pdp_type;
        }
      }
      else
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      break;
    default:
      UIM_MSG_ERR_1("Bearer Description invalid bearer type 0x%x",
                    bearer_description->bearer_type);
      gstk_status = GSTK_ERROR;
  }
  return gstk_status;
} /* gstk_packer_bearer_description_tlv */


/*===========================================================================
FUNCTION gstk_packer_buffer_size_tlv

DESCRIPTION

  This function populates the passed in buffer size tlv pointer

PARAMETERS
  buffer_size: [Input] Indicates the buffer size value
  buffer_size_tlv: [Input/Output] Pointers to which the
                              gstk_buffer_size_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_buffer_size_tlv(
         int32                         buffer_size,
         gstk_buffer_size_tlv_type    *buffer_size_tlv)
{

  UIM_MSG_HIGH_0("** Packing buffer size");
  if (buffer_size_tlv == NULL)
  {
    UIM_MSG_ERR_0("Buffer size Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  buffer_size_tlv->tag = (int)GSTK_BUFFER_SIZE_TAG;

  buffer_size_tlv->buffer_size_length = GSTK_BUFFER_SIZE_LEN;

  buffer_size_tlv->buffer_size[0] = int32touint8((buffer_size >> 8) & 0xFF);
  buffer_size_tlv->buffer_size[1] = (uint8)buffer_size & 0xFF;
  return GSTK_SUCCESS;
} /* gstk_packer_buffer_size_tlv */


/*===========================================================================
FUNCTION gstk_packer_ch_data_len_tlv

DESCRIPTION

  This function populates the passed in channel data length tlv pointer

PARAMETERS
  ch_data_len: [Input] Indicates the channel data length
  ch_data_len_tlv: [Input/Output] Pointers to which the
                              gstk_ch_data_len_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_ch_data_len_tlv(
         int32                         ch_data_len,
         gstk_ch_data_len_tlv_type    *ch_data_len_tlv)
{

  UIM_MSG_HIGH_0("** Packing Channel Data Length");
  if (ch_data_len_tlv == NULL)
  {
    UIM_MSG_ERR_0("Channel Data Length Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  ch_data_len_tlv->tag = (int)GSTK_CHANNEL_DATA_LEN_TAG;

  ch_data_len_tlv->ch_data_len_length = GSTK_CHANNEL_DATA_LEN_LEN;

  /* The max channel data length requested cannot be more than 0xFF */
  if(ch_data_len > 0xFF)
  {
    ch_data_len_tlv->ch_data_len = 0xFF;
  }
  else
  {
    ch_data_len_tlv->ch_data_len = (uint8)ch_data_len;
  }
  return GSTK_SUCCESS;
} /* gstk_packer_ch_data_len_tlv */


/*===========================================================================
FUNCTION gstk_packer_ch_data_tlv

DESCRIPTION

  This function populates the passed in channel data tlv pointer

PARAMETERS
  ch_data: [Input] Indicates the channel data length
  ch_data_tlv: [Input/Output] Pointers to which the
                              gstk_ch_data_tlv_type info to be
                              populated

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
gstk_status_enum_type gstk_packer_ch_data_tlv(
         gstk_ch_data_type             ch_data,
         gstk_ch_data_tlv_type        *ch_data_tlv)
{

  UIM_MSG_HIGH_0("** Packing Channel Data");
  if (ch_data_tlv == NULL)
  {
    UIM_MSG_ERR_0("Channel Data Err: Null ptr");
    return GSTK_NULL_INPUT_PARAM;
  }
  ch_data_tlv->tag = (int)GSTK_CHANNEL_DATA_TAG;

  ch_data_tlv->ch_data_length = int32touint8(ch_data.data_len);

  /* initialize ch_data to NULL */
  ch_data_tlv->ch_data = NULL;

  if ((ch_data.data_len > 0) && (ch_data.data != NULL)) {
    ch_data_tlv->ch_data = GSTK_CALLOC(ch_data_tlv->ch_data_length);
    if (ch_data_tlv->ch_data != NULL) {
      (void)memscpy(ch_data_tlv->ch_data, ch_data_tlv->ch_data_length, ch_data.data, ch_data_tlv->ch_data_length);
    }
  }
  return GSTK_SUCCESS;
} /* gstk_packer_ch_data_tlv */


/*===========================================================================
FUNCTION gstk_packer_dev_id_card_tlv

DESCRIPTION

  This function populates the data uint8 array with Device ID value info

PARAMETERS
  dev_id_orig_ptr       [Input] data pointer from which the information
                        if to be copied
  dev_id_new_value_ptr  [Input/Output] data pointer to which the information
                        is to be populated

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
gstk_status_enum_type gstk_packer_dev_id_card_tlv(
  const gstk_device_id_type          *dev_id_orig_ptr,
  uint8                              *dev_id_new_value_ptr
)
{

  UIM_MSG_HIGH_0("** Packing device ID");

  if((dev_id_orig_ptr == NULL) || (dev_id_new_value_ptr == NULL))
  {
    UIM_MSG_ERR_0("Device Id Err: NULL");
    UIM_MSG_ERR_2("Device Id Err: orig ptr 0x%x, new ptr 0x%x",
                  dev_id_orig_ptr, dev_id_new_value_ptr);
    return GSTK_BAD_PARAM;
  }
  dev_id_new_value_ptr[0] = (uint8)dev_id_orig_ptr->src;
  dev_id_new_value_ptr[1] = (uint8)dev_id_orig_ptr->dest;

  return GSTK_SUCCESS;
} /* gstk_packer_dev_id_card_tlv */


/*===========================================================================
FUNCTION gstk_packer_location_info_card_tlv

DESCRIPTION

   This function populates the data uint8 array with Location Info value
   according to the technology

PARAMETERS
  location_info_orig_ptr        [Input] data pointer from which the information
                                if to be copied
  location_info_new_value_ptr   [Input/Output] data pointer to which the
                                information is to be populated

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
gstk_status_enum_type gstk_packer_location_info_card_tlv(
  const gstk_location_info_gen_type  *location_info_orig_ptr,
  uint8                              *location_info_new_value_ptr,
  int32                               dest_size
)
{
  size_t new_dest_size        =  int32touint32(dest_size);

  UIM_MSG_HIGH_0("** Packing location info");

  if((location_info_orig_ptr == NULL) || (location_info_new_value_ptr == NULL))
  {
    UIM_MSG_ERR_2("Location Info Err: orig ptr 0x%x, new ptr 0x%x",
                  location_info_orig_ptr, location_info_new_value_ptr);
    return GSTK_BAD_PARAM;
  }

  if(dest_size < 0)
  {
    UIM_MSG_ERR_0("dest size less than 0");
    return GSTK_BAD_PARAM;
  }

  if (location_info_orig_ptr->location_info_tech == GSTK_GSM_UMTS_LOCATION_INFO)
  {
    (void)gstk_memcpy(
         location_info_new_value_ptr,
         location_info_orig_ptr->location_info.gsm_umts_loc_info.mcc_and_mnc,
         GSTK_MAX_PLMN_LEN, /*copy_size */
         new_dest_size,     /* dest max size */
         sizeof(location_info_orig_ptr->location_info.gsm_umts_loc_info.mcc_and_mnc));

    new_dest_size = int32touint32(dest_size) - GSTK_MAX_PLMN_LEN;

    (void)gstk_memcpy(
         &location_info_new_value_ptr[GSTK_MAX_PLMN_LEN],
         location_info_orig_ptr->location_info.gsm_umts_loc_info.loc_area_code,
         GSTK_MAX_LAC_LEN, /*copy_size */
         new_dest_size,    /* dest max size */
         sizeof(location_info_orig_ptr->location_info.gsm_umts_loc_info.loc_area_code));
    
    new_dest_size = int32touint32(dest_size) - (GSTK_MAX_PLMN_LEN + GSTK_MAX_LAC_LEN);

    if(gstk_memcpy(
         &location_info_new_value_ptr[GSTK_MAX_PLMN_LEN + GSTK_MAX_LAC_LEN],
         location_info_orig_ptr->location_info.gsm_umts_loc_info.cell_id,
         int32touint32(location_info_orig_ptr->location_info.gsm_umts_loc_info.cell_id_len),    /*copy_size */
         new_dest_size,           /* dest max size */
         sizeof(location_info_orig_ptr->location_info.gsm_umts_loc_info.cell_id)) < /* src max size */
       int32touint32(location_info_orig_ptr->location_info.gsm_umts_loc_info.cell_id_len))
    {
      return GSTK_MEMORY_ERROR;
    }

    return GSTK_SUCCESS;
  }
  if (location_info_orig_ptr->location_info_tech == GSTK_CDMA_LOCATION_INFO)
  {
    return GSTK_UNSUPPORTED_COMMAND;
  }
  return GSTK_ERROR;

} /* gstk_packer_location_info_card_tlv */


/*lint -e715 pdp_context_act_param_orig_ptr not being used */
/*lint -e715 pdp_context_act_param_new_value_ptr not being used */
/*lint -e818 pdp_context_act_param_new_value_ptr not const */
/*===========================================================================
FUNCTION gstk_packer_pdp_context_act_param_card_tlv

DESCRIPTION

   This function populates the data uint8 array with PDP Context Activation
   Parameter value

PARAMETERS
  pdp_context_act_param_orig_ptr      [Input] data pointer from which the
                                      information if to be copied
  pdp_context_act_param_new_value_ptr [Input/Output] data pointer to which the
                                      information is to be populated

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
gstk_status_enum_type gstk_packer_pdp_context_act_param_card_tlv(
  const gstk_pdp_context_act_param_type  *pdp_context_act_param_orig_ptr,
  uint8                                  *pdp_context_act_param_new_value_ptr
)
{
#ifdef FEATURE_GPRS_CALLCONTROL
  UIM_MSG_HIGH_0("** Packing PDP Context Act Parameter");

  if(pdp_context_act_param_orig_ptr == NULL)
  {
    UIM_MSG_ERR_1("PDP Context Act Parameter orig ptr 0x%x",
                  pdp_context_act_param_orig_ptr);
    return GSTK_BAD_PARAM;
  }
  if(pdp_context_act_param_orig_ptr->length == 0)
  {
    UIM_MSG_ERR_0("PDP Context Act Parameter Len = 0");
    return GSTK_SUCCESS;
  }
  if(pdp_context_act_param_new_value_ptr == NULL)
  {
    UIM_MSG_ERR_1("PDP Context Act Parameter new ptr 0x%x",
                  pdp_context_act_param_new_value_ptr);
    return GSTK_BAD_PARAM;
  }

  (void)memscpy(pdp_context_act_param_new_value_ptr,
         int32touint32(pdp_context_act_param_orig_ptr->length),
         pdp_context_act_param_orig_ptr->pdp_act_param,
         int32touint32(pdp_context_act_param_orig_ptr->length));

  return GSTK_SUCCESS;
#else
  UIM_MSG_ERR_0("No GPRS Call Control Support");
  return GSTK_ERROR;
#endif /* #ifdef FEATURE_GPRS_CALLCONTROL */

} /* gstk_packer_pdp_context_act_param_card_tlv */
/*lint +e715 pdp_context_act_param_orig_ptr not being used */
/*lint +e715 pdp_context_act_param_new_value_ptr not being used */
/*lint +e818 pdp_context_act_param_new_value_ptr not const */

/*===========================================================================
FUNCTION gstk_packer_pdn_conn_act_param_card_tlv

DESCRIPTION

   This function populates the data uint8 array with PDP Context Activation
   Parameter value

PARAMETERS
  pdn_conn_act_param_orig_ptr      [Input] data pointer from which the
                                      information if to be copied
  pdn_conn_act_param_new_value_ptr [Input/Output] data pointer to which the
                                      information is to be populated

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
gstk_status_enum_type gstk_packer_pdn_conn_act_param_card_tlv(
  const gstk_eps_pdn_conn_act_param_type *pdn_conn_act_param_orig_ptr,
  uint8                                  *pdn_conn_act_param_new_value_ptr
)
{
#ifdef FEATURE_CM_LTE
  UIM_MSG_HIGH_0("** Packing PDN Conn Act Parameter");

  if(pdn_conn_act_param_orig_ptr == NULL)
  {
    UIM_MSG_ERR_1("PDN Conn Act Parameter orig ptr 0x%x",
                  pdn_conn_act_param_orig_ptr);
    return GSTK_BAD_PARAM;
  }
  if(pdn_conn_act_param_orig_ptr->length == 0)
  {
    UIM_MSG_ERR_0("PDN Conn Act Parameter Len = 0");
    return GSTK_SUCCESS;
  }
  if(pdn_conn_act_param_new_value_ptr == NULL)
  {
    UIM_MSG_ERR_1("PDN Conn Act Parameter new ptr 0x%x",
                  pdn_conn_act_param_new_value_ptr);
    return GSTK_BAD_PARAM;
  }

  (void)memscpy(pdn_conn_act_param_new_value_ptr,
         int32touint32(pdn_conn_act_param_orig_ptr->length),
         pdn_conn_act_param_orig_ptr->eps_act_param,
         int32touint32(pdn_conn_act_param_orig_ptr->length));

  return GSTK_SUCCESS;
#else
  UIM_MSG_ERR_0("No EPS Call Control Support");
  return GSTK_ERROR;
#endif /* FEATURE_CM_LTE */

} /* gstk_packer_pdn_conn_act_param_card_tlv */

/*===========================================================================
FUNCTION gstk_packer_ims_call_param_card_tlv

DESCRIPTION

   This function populates the data uint8 array with IMS request URI
   Parameter value

PARAMETERS
  ims_call_param_orig_ptr      [Input] data pointer from which the
                                      information if to be copied
  ims_call_param_orig_ptr      [Input/Output] data pointer to which the
                                      information is to be populated

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
gstk_status_enum_type gstk_packer_ims_call_param_card_tlv(
  const gstk_ims_call_param_type                *ims_call_param_orig_ptr,
  uint8                                  *ims_call_param_new_value_ptr
)
{
  UIM_MSG_HIGH_0("** Packing IMS request URI");

  if(ims_call_param_orig_ptr == NULL)
  {
    UIM_MSG_ERR_1("PDN Conn Act Parameter orig ptr 0x%x",
      ims_call_param_orig_ptr);
    return GSTK_BAD_PARAM;
  }
  if(ims_call_param_orig_ptr->length == 0)
  {
    UIM_MSG_ERR_0("PDN Conn Act Parameter Len = 0");
    return GSTK_SUCCESS;
  }
  if(ims_call_param_new_value_ptr == NULL)
  {
    UIM_MSG_ERR_1("PDN Conn Act Parameter new ptr 0x%x",
      ims_call_param_new_value_ptr);
    return GSTK_BAD_PARAM;
  }

  (void)memscpy(ims_call_param_new_value_ptr,
         int32touint32(ims_call_param_orig_ptr->length),
         ims_call_param_orig_ptr->ims_uri,
         int32touint32(ims_call_param_orig_ptr->length));

  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_cmd_details_card_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  ref_id: [Input] Unique reference Id
  command_rsp_type: [Input] Specifies the type of terminal response, e.g.,
                            GSTK_DISPLAY_TEXT_CNF, etc,
  command_number: [Input] Command number for the proactive command that
                          this terminal response is responding to
  command_details: [Input/Output] Pointers to which the command details
                                  to be populated

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
gstk_status_enum_type gstk_packer_cmd_details_card_tlv(
  gstk_cmd_info_type             cmd_info,
  uint8                         *cmd_details_new_value_ptr
)
{
    int32 offset = 0;
    uint32 index = 0;

    UIM_MSG_HIGH_0("** Packing Cmd Details");

    if(cmd_details_new_value_ptr == NULL)
    {
       UIM_MSG_ERR_0("Cmd Detail Err: NULL");
       return GSTK_BAD_PARAM;
    }
    /* 4 checks to verify that the ref_id is a valid value/match to the information
    ** in the command_details_ref_table
    ** - reference_id check
    ** - whether the location in the table is still valid
    ** - command number check
    ** - response type and type of command match
    */
    index = GSTK_CMD_REF_TABLE_INDEX(cmd_info.ref_id);
    if (index < GSTK_MAX_PROACTIVE_COMMAND) { /*ref_id within range */
        if (command_details_ref_table_ptr[index].free_slot != TRUE ) { /* this location is still used*/
          if ((int)command_details_ref_table_ptr[index].expected_cmd_rsp_type == cmd_info.command_rsp_type) {
              /* command matches -> valid ref_id */
              if (command_details_ref_table_ptr[index].command_details.command_number !=
                    cmd_info.command_number ) {
                /* wrong match */
                UIM_MSG_ERR_0("Cmd Detail Err: Command Num Mismatch");
                return GSTK_BAD_PARAM;
              }
          }
          else
          {
             UIM_MSG_ERR_0("Cmd Detail Err: Command Rsp Mismatch");
             return GSTK_BAD_PARAM;
          }
        }
        else {
          UIM_MSG_ERR_1("Cmd Detail Err: command_details_ref_table_ptr[0x%x] not in use",
                        index);
          return GSTK_BAD_PARAM;
        }
    }
    else {
      UIM_MSG_ERR_0("Cmd Detail Err: ref_id 0x%x out of range");
      return GSTK_BAD_PARAM;
    }

    /* populate the command_details tlv for the response */

    cmd_details_new_value_ptr[offset] =
      command_details_ref_table_ptr[index].command_details.command_number;
    offset ++;

    cmd_details_new_value_ptr[offset] =
      command_details_ref_table_ptr[index].command_details.type_of_command;
    offset ++;

    cmd_details_new_value_ptr[offset] =
      command_details_ref_table_ptr[index].command_details.qualifier;
    offset ++;

    return GSTK_SUCCESS;
} /*gstk_packer_cmd_details_card_tlv*/


/*===========================================================================
FUNCTION gstk_packer_result_card_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  result_info           [Input] data pointer from which the information
                        if to be copied
  result_new_value_ptr  [Input/Output] data pointer to which the information
                        is to be populated

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
gstk_status_enum_type gstk_packer_result_card_tlv(
  gstk_result_type              result_info,
  uint8                         *result_new_value_ptr
)
{
  UIM_MSG_HIGH_0("** Packing Result");

  if(result_new_value_ptr == NULL)
  {
    UIM_MSG_ERR_0("Result new value ptr Err: NULL");
    return GSTK_BAD_PARAM;
  }
  result_new_value_ptr[0] = (uint8)result_info.command_result;
  if((result_info.result_additional_info.length > 0) &&
     (result_info.result_additional_info.additional_info_ptr != NULL)
    )
  {
    (void)memscpy(&(result_new_value_ptr[1]),
           result_info.result_additional_info.length,
           result_info.result_additional_info.additional_info_ptr,
           result_info.result_additional_info.length);
  }
  return GSTK_SUCCESS;

}


/*===========================================================================
FUNCTION gstk_packer_nw_search_mode_card_tlv

DESCRIPTION

  This function populates the data uint8 array with network search mode value info

PARAMETERS
  nw_search_mode                [Input] data pointer from which the information
                                if to be copied
  nw_search_mode_new_value_ptr  [Input/Output] data pointer to which the information
                                is to be populated

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
gstk_status_enum_type gstk_packer_nw_search_mode_card_tlv(
  gstk_nw_search_mode_enum_type    nw_search_mode,
  uint8                            *nw_search_mode_new_value_ptr
)
{

  UIM_MSG_HIGH_0("** Packing Network Search Mode");

  if(nw_search_mode_new_value_ptr == NULL)
  {
    UIM_MSG_ERR_0("nw_search_mode_new_value_ptr Err: NULL");
    return GSTK_BAD_PARAM;
  }
  nw_search_mode_new_value_ptr[0] = int32touint8((int32)nw_search_mode);
  return GSTK_SUCCESS;
} /* gstk_packer_nw_search_mode_card_tlv */
/*===========================================================================
FUNCTION gstk_packer_evt_list_card_tlv

DESCRIPTION

  This function populates the data uint8 array with event id value info

PARAMETERS
  evt_list_info_data       [Input] data pointer from which the information
                           if to be copied
  evt_list_new_value_ptr   [Input/Output] data pointer to which the information
                           is to be populated

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
gstk_status_enum_type gstk_packer_evt_list_card_tlv(
  gstk_evt_list_code_enum_type      evt_list_info_data,
  uint8                            *evt_list_new_value_ptr
)
{

  UIM_MSG_HIGH_0("** Packing Event List");

  if(evt_list_new_value_ptr == NULL)
  {
    UIM_MSG_ERR_0("evt_list_new_value_ptr Err: NULL");
    return GSTK_BAD_PARAM;
  }
  evt_list_new_value_ptr[0] = int32touint8((int32)evt_list_info_data);
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_csg_cell_selection_status_tlv

DESCRIPTION
  This function populates CSG cell selection information in to required
  structure

PARAMETERS
  csg_cell_selection_status      [INPUT] Data pointer from which information
                                 to be copied
  csg_cell_selection_status_tlv  [INPUT/OUTPUT] Data pointer to which information
                                 to be copied

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
gstk_status_enum_type gstk_packer_csg_cell_selection_status_tlv(
  const gstk_cell_selection_status_type*      csg_cell_selection_status,
  gstk_csg_cell_selection_status_tlv_type*    csg_cell_selection_status_tlv
)
{
  UIM_MSG_HIGH_0("** Packing CSG Cell Selection Status");

  csg_cell_selection_status_tlv->tag = GSTK_CSG_CELL_SLECTION_STATUS_TAG;
    csg_cell_selection_status_tlv->selection_status_len =
      GSTK_CSG_CELL_SELECTION_STATUS_LEN;
  csg_cell_selection_status_tlv->selection_status[0] =
    csg_cell_selection_status->general_info.is_camped ? 0x01 : 0x00;
  /* As per spec. CSG cell selection status may include additional info and since
  CM/NAS don't have support to provide selection mode for CSG, Additional information
  will be pass as "00" till support for CSG seleciton mode is added in CM/NAS */
  csg_cell_selection_status_tlv->selection_status[1] = 0x00;
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_csg_id_tlv

DESCRIPTION
  This function populates CSG ID information in to required structure

PARAMETERS
  csg_id      [INPUT] Data pointer from which information to be copied
  csg_id_tlv  [INPUT/OUTPUT] Data pointer to which information to be copied

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
gstk_status_enum_type gstk_packer_csg_id_tlv(
  const uint32*            csg_id,
  gstk_csg_id_tlv_type*    csg_id_tlv
)
{
  uint32 temp_csg_id = 0;

  temp_csg_id = *csg_id;

  UIM_MSG_HIGH_0("** Packing CSG ID");

  csg_id_tlv->tag = GSTK_CSG_ID_TAG;
  csg_id_tlv->csg_id_len = GSTK_CSG_ID_LEN;
  /*CSG ID can be of max lenght 27 bits and as per TS 23.003 setting unused bit to 1*/
  temp_csg_id = temp_csg_id << 5;
  temp_csg_id = temp_csg_id | 0x1F;
  csg_id_tlv->csg_id[3] = temp_csg_id & 0xFF;
  csg_id_tlv->csg_id[2] = (temp_csg_id >> 8) & 0xFF;
  csg_id_tlv->csg_id[1] = (temp_csg_id >> 16) & 0xFF;
  csg_id_tlv->csg_id[0] = (temp_csg_id >> 24) & 0xFF;
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_hnb_name_tlv

DESCRIPTION
  This function populates HNB Name into required structure

PARAMETERS
  hnb_name      [INPUT] Data pointer from which information to be copied
  hnb_name_len  [INPUT] Length of input HNB name
  hnb_name_tlv  [INPUT/OUTPUT] Data pointer to which information to be copied

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
gstk_status_enum_type gstk_packer_hnb_name_tlv(
  const uint8*               hnb_name,
  uint8                      hnb_name_len,
  gstk_hnb_name_tlv_type*    hnb_name_tlv
)
{
  UIM_MSG_HIGH_0("** Packing HNB Name");

  hnb_name_tlv->tag = GSTK_HNB_NAME_TAG;
  hnb_name_tlv->hnb_name_len = hnb_name_len;
  if(gstk_memcpy(hnb_name_tlv->hnb_name,
                 hnb_name,
                 hnb_name_len,
                 GSTK_HNB_NAME_LEN,
                 hnb_name_len) <
     hnb_name_len)
  {
    UIM_MSG_ERR_0("gstk_packer_hnb_name_tlv(): gstk_memcpy failed");
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_impu_list_tlv

DESCRIPTION
  This function populates IMPU List into required structure

PARAMETERS
  impu_list      [INPUT] Data pointer from which information to be copied
  impu_list_len  [INPUT] Length of input IMPU List
  impu_list_tlv  [INPUT/OUTPUT] Data pointer to which information to be copied

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
gstk_status_enum_type gstk_packer_impu_list_tlv(
  const uint8*                     impu_list,
  uint8                            impu_list_len,
  gstk_ims_reg_impu_list_tlv_type* impu_list_tlv
)
{
  UIM_MSG_HIGH_0("** Packing IMPU List");

  impu_list_tlv->tag = GSTK_IMPU_LIST_TAG;
  impu_list_tlv->impu_list_len = impu_list_len;
  impu_list_tlv->impu_list = NULL;

  if (impu_list_len != 0 && impu_list == NULL)
  {
    UIM_MSG_ERR_0("IMPU List Err: impu_list_len != 0 but impu_list == NULL!");
    return GSTK_BAD_PARAM;
  }

  if (impu_list_len != 0)
  {
    impu_list_tlv->impu_list =
      (uint8*)GSTK_CALLOC(impu_list_len * sizeof(byte) );

    if (impu_list_tlv->impu_list == NULL) {
      return GSTK_MEMORY_ERROR;
    }

    memscpy(impu_list_tlv->impu_list,
            impu_list_len,
            impu_list,
            impu_list_len);
  }

  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_ims_status_code_tlv

DESCRIPTION
  This function populates IMS Status Code into required structure

PARAMETERS
  status_code     [INPUT] Data pointer from which information to be copied
  status_code_len [INPUT] Length of input Status Code
  status_code_tlv [INPUT/OUTPUT] Data pointer to which information to be copied

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
gstk_status_enum_type gstk_packer_ims_status_code_tlv(
  const uint8*                       status_code,
  uint8                              status_code_len,
  gstk_ims_reg_status_code_tlv_type* status_code_tlv
)
{
  UIM_MSG_HIGH_0("** Packing IMS Status Code");

  if (status_code_len > 0)
  {
    status_code_tlv->tag = GSTK_IMS_STATUS_CODE_TAG;
    status_code_tlv->status_code_len = status_code_len;

    status_code_tlv->status_code =
    (uint8*)GSTK_CALLOC(status_code_len * sizeof(byte) );

    if (status_code_tlv->status_code == NULL) {
      return GSTK_MEMORY_ERROR;
    }

    if (status_code != NULL)
    {
      memscpy(status_code_tlv->status_code,
              status_code_len,
              status_code,
              status_code_len);
    }
    else
    {
      UIM_MSG_ERR_0("IMS Status Code Err: status_code NULL!");
      return GSTK_BAD_PARAM;
    }
  }
  else
  {
    UIM_MSG_ERR_0("IMS Status Code Err: status_code_len cannot be 0!");
    return GSTK_BAD_PARAM;
  }

  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_dns_addr_tlv

DESCRIPTION
  This function populates DNS addresses into required structure

PARAMETERS
  dns_addrs: [Input] The DNS addresses returned by network
  dns_addrs_tlv: [Input/Output] Pointers to which information to be copied

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
gstk_status_enum_type gstk_packer_dns_addr_tlv(
  gstk_data_address_type        *dns_addr_ptr,
  gstk_dns_addr_tlv_type        *dns_addr_tlv
)
{
  UIM_MSG_HIGH_0("** Packing DNS IP Address");

  if (dns_addr_tlv == NULL || dns_addr_ptr == NULL)
  {
    UIM_MSG_ERR_0("DNS Address Err: dns_addr_tlv or dns_addr_ptr == NULL!");
    return GSTK_BAD_PARAM;
  }

  dns_addr_tlv->tag = GSTK_DNS_ADDRESS_TAG;
  dns_addr_tlv->length = dns_addr_ptr->length + 1; /* 1 extra byte for address type */

  /* address type */
  if (dns_addr_ptr->length == GSTK_IPV4_ADDRESS_LEN)
  {
    dns_addr_tlv->type = 0x21; /* IPv4 address */
  }
  else if (dns_addr_ptr->length == GSTK_IPV6_ADDRESS_LEN)
  {
    dns_addr_tlv->type = 0x57; /* IPv6 address */
  }
  else
  {
    UIM_MSG_HIGH_0("gstk_packer_dns_addr_tlv: Empty or invalid DNS address");
    dns_addr_tlv->length = 0;
    dns_addr_tlv->type = 0x00;
  }

  if (dns_addr_ptr->length > 0)
  {
    dns_addr_tlv->addr = (uint8*)GSTK_CALLOC(dns_addr_ptr->length);

    if (dns_addr_tlv->addr == NULL) {
      return GSTK_MEMORY_ERROR;
    }

    memscpy(dns_addr_tlv->addr,
            dns_addr_ptr->length,
            dns_addr_ptr->address,
            dns_addr_ptr->length);
  }

  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_csg_cell_tlv

DESCRIPTION

  This function populates the passed in csg cell tlv pointer

PARAMETERS
  data                  : [Input] Indicates the battery status
  csg_cell_tlv          : [Input/Output] Pointers to which the battery status needs to be populated

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
gstk_status_enum_type gstk_packer_csg_cell_tlv(
  sys_detailed_plmn_list_s_type     csg_info,
  gstk_csg_info_tlv_type        *csg_cell_tlv
)
{
  uint8 i, j;
  uint8 length                                = 0;
  uint32 temp_csg_id                          = 0;
  gstk_available_csg_list_type *temp_csg_info = NULL;

  UIM_MSG_HIGH_0("** Packing CSG cell information");

  if(csg_cell_tlv == NULL)
  {
    UIM_MSG_ERR_0("csg_cell_tlv is NULL");
    return GSTK_BAD_PARAM;
  }

  csg_cell_tlv->csg_info = NULL;

  UIM_MSG_HIGH_1("no of available network 0x%x",csg_info.length);

  if(csg_info.length == 0)
  {
    csg_cell_tlv->csg_info = (uint8*)GSTK_CALLOC(UIM_MAX_TR_BYTES * sizeof(byte));
    if( csg_cell_tlv->csg_info == NULL)
    {
      UIM_MSG_ERR_0("csg_cell_tlv->csg_info is NULL");
      return GSTK_MEMORY_ERROR;
    }

    csg_cell_tlv->csg_info[length++] = 0x80;
    csg_cell_tlv->csg_info[length++] = 0x03;

    length = length + gstk_byte_offset_memcpy(csg_cell_tlv->csg_info,
                                            gstk_curr_inst_ptr->csg_info.plmn_info,
                                            length,
                                            3,
                                            UIM_MAX_TR_BYTES,
                                            3);

    csg_cell_tlv->csg_info[length++] = 0x81;
    csg_cell_tlv->csg_info[length++] = gstk_curr_inst_ptr->csg_info.len + GSTK_CSG_ID_LEN;
    temp_csg_id = gstk_curr_inst_ptr->csg_info.csg_id;
    temp_csg_id = temp_csg_id << 5;
    temp_csg_id = temp_csg_id | 0x1F;
    csg_cell_tlv->csg_info[length + 3] = temp_csg_id & 0xFF;
    csg_cell_tlv->csg_info[length + 2] = (temp_csg_id >> 8) & 0xFF;
    csg_cell_tlv->csg_info[length + 1] = (temp_csg_id >> 16) & 0xFF;
    csg_cell_tlv->csg_info[length]     = (temp_csg_id >> 24) & 0xFF;
    length = length + GSTK_CSG_ID_LEN;

    length = length + gstk_byte_offset_memcpy(csg_cell_tlv->csg_info,
                                            gstk_curr_inst_ptr->csg_info.hnb_name,
                                            length,
                                            gstk_curr_inst_ptr->csg_info.len,
                                            UIM_MAX_TR_BYTES,
                                            GSTK_HNB_NAME_MAX_LEN);

    csg_cell_tlv->tag            = (int)GSTK_CSG_ID_LIST_TAG;
    csg_cell_tlv->length         =  length;
    return GSTK_SUCCESS;
  }

  temp_csg_info = GSTK_CALLOC(sizeof(gstk_available_csg_list_type));
  if(temp_csg_info == NULL)
  {
    UIM_MSG_ERR_0("temp_csg_info is NULL");
    return GSTK_MEMORY_ERROR;
  }

  gstk_util_extract_plmn_csg_info(temp_csg_info, &csg_info);

  csg_cell_tlv->csg_info = (uint8*)GSTK_CALLOC(UIM_MAX_TR_BYTES * sizeof(byte));
  if(csg_cell_tlv->csg_info == NULL)
  {
    UIM_MSG_ERR_0("csg_cell_tlv->csg_info is NULL");
    return GSTK_MEMORY_ERROR;
  }

  for(i = 0; i < temp_csg_info->no_of_plmn_entry; i++)
  {
    csg_cell_tlv->csg_info[length++] = 0x80;
    csg_cell_tlv->csg_info[length++] = 0x03;
    gstk_memcpy(&csg_cell_tlv->csg_info[length],
          temp_csg_info->available_plmn_info[i].plmn,
          3,
          3,
          3);
    length = length + 3;
    for(j = 0; j < temp_csg_info->available_plmn_info[i].no_of_csg_entry; j++)
    {
      csg_cell_tlv->csg_info[length++] = 0x81;
      csg_cell_tlv->csg_info[length++] = temp_csg_info->available_plmn_info[i].csg_info[j].len + GSTK_CSG_ID_LEN;
      temp_csg_id = temp_csg_info->available_plmn_info[i].csg_info[j].csg_id;
      temp_csg_id = temp_csg_id << 5;
      temp_csg_id = temp_csg_id | 0x1F;
      csg_cell_tlv->csg_info[length + 3] = temp_csg_id & 0xFF;
      csg_cell_tlv->csg_info[length + 2] = (temp_csg_id >> 8) & 0xFF;
      csg_cell_tlv->csg_info[length + 1] = (temp_csg_id >> 16) & 0xFF;
      csg_cell_tlv->csg_info[length]     = (temp_csg_id >> 24) & 0xFF;

      length = length + GSTK_CSG_ID_LEN;
      length = length + gstk_byte_offset_memcpy(csg_cell_tlv->csg_info,
               temp_csg_info->available_plmn_info[i].csg_info[j].hnb_name,
                                                length,
               temp_csg_info->available_plmn_info[i].csg_info[j].len,
                                                UIM_MAX_TR_BYTES,
                                                GSTK_HNB_NAME_MAX_LEN);
    }
  }

  csg_cell_tlv->tag            = (int)GSTK_CSG_ID_LIST_TAG;
  csg_cell_tlv->length         =  length;
  gstk_free(temp_csg_info);
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_packer_nw_search_mode_tlv

DESCRIPTION

  This function populates the passed in network search mode tlv pointer

PARAMETERS
  data_ptr                  : [Input] Indicates the network search mode
  nw_search_mode_tlv: [Input/Output] Pointers to which the battery status needs to be populated

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
gstk_status_enum_type gstk_packer_nw_search_mode_tlv(
  gstk_nw_search_mode_enum_type  nw_search_mode,
  gstk_nw_search_mode_tlv_type   *nw_search_mode_tlv
)
{
  UIM_MSG_HIGH_0("** Packing Battery Status");

  if(nw_search_mode_tlv == NULL)
  {
    UIM_MSG_ERR_0("battery_status_tlv is NULL");
    return GSTK_BAD_PARAM;
  }

  nw_search_mode_tlv->tag            = (int)GSTK_NW_SEARCH_MODE_TAG;
  nw_search_mode_tlv->length         =  GSTK_NW_SEARCH_MODE_LEN;
  nw_search_mode_tlv->nw_search_mode = (uint8)nw_search_mode;

  return GSTK_SUCCESS;
}

