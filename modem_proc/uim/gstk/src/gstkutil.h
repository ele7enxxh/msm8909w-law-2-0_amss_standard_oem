#ifndef gstkutil_h
#define gstkutil_h
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   U T I L   H E A D E R


GENERAL DESCRIPTION
  This is the header file that provides definition for GSTK internal utility
  as well as parsing and packing routines.


FUNCTIONS
  gstk_util_cmd_details_ref_table_get_next_free_slot
    This function returns the next available slot in the
    command_details_ref_table

  gstk_util_cmd_details_ref_table_free_slot
    This function marks the index location in the command_details_ref_table
    as indicated by the ref_id as available for upcoming use

  gstk_util_env_cmd_ref_table_get_next_free_slot
    This function returns the next available slot in the
    envelope_cmd_ref_table

  gstk_util_env_cmd_ref_table_free_slot
    This function marks the index location in the envelope_cmd_ref_table
    as indicated by the ref_id as available for upcoming use

  gstk_util_save_env_cmd_info
    This function get a free slot from the envelope_cmd_ref_table, and
    populate it with the user envelope command info.

  gstk_valid_tag
    Check for proactive command tag matches

  gstk_get_uim_buf
    This function returns a pointer to heap for buffer allocation for UIM
    Server interface.

  gstk_send_cmd_to_uim_server
    This function sends the uim cmd to the uim server

  gstk_util_populate_uim_buf_hdr_info
    This function gets a pointer for uim_cmd_ptr, and populates header info
    of the uim_cmd_type based on the uim_cmd_name_type

  gstk_find_length_of_length_value
    This function determines the size of the length field

  gstk_util_is_valid_client_id
    This function determines if the client_id is valid or not

  gstk_util_convert_cmd_type_to_reg_bitmask
    This function converts the command type into the registration bitmask

  gstk_util_find_client_with_registered_event
    This function goes through the gstk_client_table and see which client
    has registered for the event

  gstk_util_send_message_to_clients
    This function sends the message to the GSTK clients based on which clients
    have registered to receive that events.

  gstk_util_send_tr_to_clients
    This function sends the terminal response to the GSTK clients based on which clients
    have registered to receive TR.

  gstk_util_send_response_to_clients
    This function sends the Card Response to the GSTK clients based on which
    client registration callback function (used for Envelope Response)

  gstk_util_start_client_response_timer
    This function starts the terminal response timer for the proactive command
    that is just sent to the client

  gstk_util_release_upper_layer_cmd_memory
    This function frees all the allocated memory for the upper layer (U)SAT
    command response pointer input.

  gstk_util_check_evt_mask_reg_status
    This function check if the input bitmask has already been registered by
    any of the other clients.

  gstk_util_translate_qualifier_to_dcs
    This function translates the qualifier into the corresponding data
    coding scheme.

  gstk_util_build_evt_dl_ind
    This function populates the gstk_task_cmd_type based on the passed in
    event download indication

  gstk_util_check_mm_state_difference
    This function will get the cell id and determines whether there are any
    differences between the new location info/state with the old new.

  gstk_util_compare_curr_and_next_location_info
    This function will compare the previously sent location info with
    the one in the next available one

  gstk_packer_address_tlv
    This function populates the passed in the address tlv pointer

  gstk_packer_alpha_tlv
    This function populates the passed in alpha tlv pointer

  gstk_packer_bc_repeat_ind_tlv
    This function populates the passed in the bc repeat indicator tlv pointer

  gstk_packer_bcch_list_tlv
    This function populates the passed in bcch list tlv pointer

  gstk_packer_browser_term_cause_tlv
    This function populates the passed in browser termination cause tlv pointer

  gstk_packer_cause_tlv
    This function populates the passed in the cause tlv pointer

  gstk_packer_cc_req_action_tlv
    This function populates the passed in the call control requested
    action tlv pointer

  gstk_packer_ccp_tlv
    This function populates the passed in the ccp tlv pointer

  gstk_packer_cell_broadcast_page_tlv
    This function populates the passed in cell broadcast page tlv pointer

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

  gstk_packer_subaddress_tlv
    This function populates the passed in the subaddress tlv pointer

  gstk_packer_text_string_tlv
    This function populates the passed in text tlv pointer

  gstk_packer_timer_id_tlv
    This function populates the passed in timer id tlv pointer

  gstk_packer_timer_value_tlv
    This function populates the passed in timer value tlv pointer

  gstk_packer_timing_advance_tlv
    This function populates the passed in timing advance tlv pointer

  gstk_packer_transaction_id_tlv
    This function populates the passed in the transaction id tlv pointer

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

  gstk_parser_dtmf_tlv
    This function parses the dtmf TLV and populate the
    passed in parameter from the function call with the info
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

  gstk_parser_lang_tlv
    This function parses the lang TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

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

  gstk_parser_timer_id_tlv
    This function parses the timer id TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_timer_value_tlv
    This function parses the timer value TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_parser_tone_tlv
    This function parses the tone TLV and populate the gstk_tone_enum_type
    passed in the function call with the info in the TLV.

  gstk_parser_url_tlv
    This function parses the url TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.

  gstk_decode_text_string_body
    Stores decoded textual string in global text_string.text
    Used in text_string.length ^ text_string.data_coding_scheme

  gstk_util_pack_7_bit_chars
    The function packs ASCII char into SMS 7 bit format

  gstk_util_unpack_7_bit_chars
    The function unpacks SMS7 bit format into ASCII char

  gstk_util_check_proactive_card
    Check if the card has proactive command capability

  gstk_util_map_mm_to_gstk_location_status
    This function will map the location status from MM to GSTK enum type.

  gstk_util_is_valid_location_info
    This function will check if the MNC, MCC, LAC, CellID and location
    status info is valid combination.

  gstk_util_determine_new_pref_from_mode_chg
    This function determines the new preferred slot from the mode change new
    slot info.

  gstk_util_handle_mode_change
    This function determine new preferred slot based on new mode from mode
    change command, and process the start toolkit or mode switch process
    accordingly

  gstk_util_has_pending_timer_envelope
    This function determines if there is any pending envelope command for
    timer expiration that needs to be sent

  gstk_util_find_first_timer_expire_command
    This function find the first pending timer expire command available.

  gstk_util_has_pending_setup_call
    This function find if there is any pending setup call proactive command

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

  gstk_packer_ch_status_tlv
    This function populates the passed in channel status tlv pointer

  gstk_packer_bearer_description_tlv
    This function populates the passed in bearer description tlv pointer

  gstk_packer_buffer_size_tlv
    This function populates the passed in buffer size tlv pointer

  gstk_packer_ch_data_len_tlv
    This function populates the passed in channel data length tlv pointer

  gstk_packer_ch_data_tlv
    This function populates the passed in channel data tlv pointer

  gstk_util_is_valid_channel_id_range
    This function determines if the channel id provided by the SIM is within
    valid range

  gstk_util_find_apn_len
    This function calculates the no of bytes to be allocated to hold
    the APN after packing. This includes the extra bytes required for
    the label separtors '.' and the terminating null byte.

  gstk_util_compareString
    This function is used to compare two strings, substrings or to
    look for the presence of a particular character in the string.

  gstk_util_extract_label_len_value
    This function is used to extract the length and the value part
    of a label and pack in the format specified in the 23.003 specs
    for APN

  gstk_util_check_for_apn_net_op_id
    This function checks the labels in the APN to determine if
    the label correspomds to an Operator ID or a Network ID.

  gstk_util_dump_byte_array
    This function dumps byte array in the format of three item per line.

  gstk_util_is_valid_data_format_req
    Check if data format type is valid

  gstk_util_is_valid_func_req
    Check if functionality type is valid

  gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type
    This function converts an STK command to a GSTK command type

  gstk_util_call_client_evt_cb
    This function call client callback based on callback type

  gstk_util_free_client_id
    This function frees up clien id

  gstk_util_get_sim_slot_id
    This function returns SIM slot id assiciated with a client id

  gstk_util_is_client_id_internal
    This function checks if SIM slot id is valid

  gstk_util_get_rat_from_ds
    This function queries data services for RAT details

  gstk_util_send_tr_if_no_service
    This function sends error terminal response if NO SERVICE

  gstk_util_get_rat_from_ds
    This function sends error terminal response if the current RAT is NOT 3GPP

  gstk_util_config_io
    This function reads GSTK configuration item from corresponding EFS file

  gstk_util_is_envelope_rsp_loc_info
    This function will check the command id of a particular response is
    an for and location information envelope

  gstk_util_find_rat_priority
    The functions finds the priority of a given radio access technology
    based on rat priority table

  gstk_util_process_mm_or_mode_change
    The function compares the currently available mm or mode change events
    and decides which one should be processed. The API is called while
    processing a MM IDLE or MODE CHANGE command.

  gstk_parser_activate_descriptor_tlv
    Generic Function used by GSTK to parse the Active Descriptor TLV

  gstk_packer_csg_cell_selection_status_tlv
    This function populates the passed in CSG Cell Selection status

  gstk_packer_csg_id_tlv
    This function populates the passed in CSG ID

  gstk_packer_hnb_name_tlv
    This function populates the passed in HNB Name

  gstk_util_find_access_tech_priority
    This function finds the priority of given radio access technology
    based on rat priority table. This differes from another utility
    function gstk_util_find_rat_priority in argument type.

  gstk_util_is_vzw_mcc
    This function compares the inputted PLMN information with US MCC values

  gstk_util_find_terminal_rsp_in_queue
    This function returns true if there is a TR in the queue, used
    for hotswap scenarios

  gstk_util_send_sw_to_client
    This function calls gstk_util_find_terminal_rsp_in_queue and sends
    the SWs to GSTK clients

  gstk_util_compare_mm_to_nw_rej_cache
    This function checks the arbitrated PLMN and LAC against nw reject
    cache

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstkutil.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/15   vr      Don't send SW1 report for the clients which didn't send TR
04/27/15   vr      Non UI and non NW proactive command parallel processing
10/13/14   gs      Removal of gstk_packer_cdma_location_info_tlv
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
09/25/14   bd      Fixed compiler errors for 1x compiled out build 
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info 
09/09/14   vr      Reduce stack usage
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
07/31/14   dy      Fixing KW errors
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/11/14   shr     Add Support for NV Refresh
06/17/14   dy      Remove GSTK RAT use in ESTK setup call
05/28/14   gm      Support of CSG Rel-11 feature
05/23/14   gm      Verifying TR response report
05/06/14   gs      GSTK Featurization enhancements and clean up
03/24/14   sw      Add support for BIP Status DIAG event
03/17/14   gm      Added support for IMS communication control
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
02/20/14   dy      Verify TR response for raw TRs
02/19/14   gs      Fixed infinite IMSI refresh issue
12/16/13   bd      Added support for new CM SS event handling
12/05/13   hn      Support DNS query and Channel reuse
11/13/13   shr     Added support for BIP over WLAN
10/21/13   jd      Added slot parameter to gstkutil_update_plmn_lac_cellId_table()
09/13/13   gm      Support for Recovery and hot-swap
07/22/13   hn      Support MM Indications
07/01/13   hh      Added support for passing RAT through MO SMS CTRL API
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
06/24/13   xz      Support IMS Registration event download
05/01/13   hh      Removed duplicate declaration of gstk_util_config_io()
05/01/13   gm      Proper decoding of USSD string
04/29/13   gm      Removed unused GSTK function
04/23/13   gm      Removed unused function
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/08/13   bd      Support for (e)HRPD access tech
01/29/13   hn      Added BIP status events
01/17/13   hn      Support multiple sessions per subscription
11/13/12   bd      Support for Closed Subscriber Group feature
10/21/12   av      Update conf file only if necessary
03/28/12   av      Replaced feature flags with NVITEM
02/18/12   av      Cleaned up remaining ALWAYS OFF features
12/20/11   nb      GSDI Cleanup
10/31/11   sg      Class L and class M support
06/27/11   sg      1) Added API gstk_util_find_high_priority_rat
                   2) Added API gstk_util_process_mm_or_mode_change
05/14/11   nb      ThreadSafe - Updated prototype of gstk_util_save_env_cmd_info()
05/12/11   xz      Fix an issue of infinite loop while process envelope cmd
04/28/11   nk      Fixed compiler warning for gstk_util_send_attach_ctrl_res()
                   prototype.
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/24/11   sg      Add gstk_util_is_envelope_rsp_loc_info
02/16/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
02/01/11   bd      Fixed lint error
01/13/11   dd      Remove estk_util_config_io
12/30/10   ssr     Fixed refresh request parsing
12/07/10   xz      Send error TR if no service or invalid RAT
11/25/10   ssr     Fixed command detail parsing
11/30/10   yt      Including private UIMDRV header
10/11/10   sg      Port IMSI_M enhancement
08/31/10   dd      Changes for compilation for LTE only build
08/30/10   xz      Add function gstk_util_check_zeros()
08/26/10   xz      Add support of EPS Call Control
07/26/10   xz      Add support of MM and EMM idle events simultaneously
06/29/10   nb      Merged changes for DS DS
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
03/19/10   sg      Fix compilation error
03/08/10   sg      Location information envelope command
02/19/10   xz      Fix compilation error
02/15/10   nb      Moved Dual Slot Changes
01/21/10   nb      Fix for skip_tlv API
10/28/09   nb      Release 7 Enhanced Netwrok Selection
07/28/09   bd      Fix lint issues
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
02/26/09   gg      Introduced Class B event registration
01/19/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
12/17/08   nb      Common Modem Interface
11/13/04   xz      Add gstk_util_free_client_id()
09/24/08   xz      Removed FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
08/28/08   sk      Removed gstk_util_calculate_log2l()
08/22/08   sk      Changed prototype for
                   gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type()
08/21/08   sk      Added support for GSTK enhanced registration.
08/08/08   xz      1) Added support for raw terminal response and raw envelope
                   response
                   2) function to dump byte array
05/06/08   sk      Added support for event download envelope retry
02/23/08   yb      Declare gstk_parser_get_next_tlv_offset() in header file
02/29/08   yb      Fixed Compilation warning in LTK
02/06/08   sk      Support for PLI UTRAN NMR
                   Lint fixes
07/23/07   sk      Fixed warning
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
06/24/07   sk      Fixed potential buffer overflow
06/12/07   sp      Declare gstk_is_tag_comprehension_req() in header file
08/23/06   sun     Added support for NAA Refresh
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM in LTK.
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
06/07/06   sk      Lint fixes.
06/06/06   sk      Added support for network search mode
                   Lint fixes
05/23/06   tml     GPRS control
06/01/06   sk      Increased evt_reg_bitmask size to uint64
05/21/06   sk      Fixed lint errors
05/03/06   sk      Removed cell id check function
05/03/06   tml     Fixed compilation issue
05/03/06   tml     compilation fix
05/03/06   tml     lint
04/04/06   sk      Added support for 1X related location information.
                   Made gstk_util_find_match_in_cmd_details_ref_table()
                   obsolete.
04/04/06   sk      Added packer function for battery status.
04/04/06   sk      Removed obsolete rrc cell id function.
10/10/05   tml     Added utility function to check and map rat
10/08/05   tml     Added function gstk_util_valid_cell_id
09/30/05   sk      Added funtion gstkutil_convert_rrc_plmn_id_to_nas_plmn_id()
                   and gstkutil_update_plmn_lac_cellId_table()
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added gstk_packer_access_technology_tlv()
                   and gstk_util_find_match_in_cmd_details_ref_table()
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
02/09/05   sk      Added prototype for ASCIIText_ConvertTo_GSMDefault()
02/02/05   sk      Added prototypes for functions
                   gstk_util_find_apn_len(),
                   gstk_util_compareString(),
                   gstk_util_extract_label_len_value(),
                   gstk_util_check_for_apn_net_op_id
01/10/05   sk/tml  Added parsing for ussd parsing
12/04/04   tml     move decode and unpack function to exp.h
11/23/04  sk/tml   Added SAT/USAT data support
08/19/04   tml     Added cell broadcast support
08/18/04   tml     Added gstk_util_decode_dcs_type function
08/10/04   tml     Added functions to support synchronizing Setup Call TR and
                   Call Connected Env
08/04/04   tml     Provide string length check in icon parsing API to correctly
                   determine if we can process the command or not
08/03/04   tml     Changed to use pointer for additioanl info
06/02/04   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/01/04   tml     Fixed location status env command issue
09/13/03   tml     changed slot_id_type to slot_id_enum_type
09/04/03   tml     Dual slot support
09/03/03   tml     Added packing and unpacking functions for ASCII and 7 bit
                   conversation
05/26/03   tml     linted
04/21/03   tml     cdma sms supports
03/07/03   tml     Added gstk_parser_file_list_tlv
02/26/03   tml     Changed gstk_util_save_env_cmd_info to take
                   gstk_evt_cb_funct_type instead of gstk_evt_cb_funct_type*
02/12/03   tml     Remove gstk_release_card_cmd_memory from this header
02/07/03 tml/jar   Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "gstk_nv_p.h"
#include "gstk.h"
#include "uim_p.h"
#include "dssocket.h"
#include "dss_iface_ioctl.h"
#include "dss_netpolicy.h"

#include "sys.h"

/*===========================================================================
  GSTK_COMMAND_DUPLICITY_MULTIPLIER: This value to multipled to the dupe count
  to obtain delay value, when card is repeating Proactive Command
===========================================================================*/
#define GSTK_COMMAND_DUPLICITY_MULTIPLIER 0x02

/*===========================================================================
  GSTK_COMMAND_DUPLICITY_RESET_COUNT: This value represent the max amount for
  which TR will delayed, when card is repeating Proactive Command
===========================================================================*/
#define GSTK_COMMAND_DUPLICITY_RESET_COUNT 120

/*===========================================================================
FUNCTION gstk_util_cmd_details_ref_table_get_next_free_slot

DESCRIPTION
  This function returns the next available slot in the
  command_details_ref_table

PARAMETERS
  None

DEPENDENCIES
    None

RETURN VALUE
    reference id for the next available slot

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint32 gstk_util_cmd_details_ref_table_get_next_free_slot( void );

/*===========================================================================
FUNCTION gstk_util_cmd_details_ref_table_free_slot

DESCRIPTION
  This function marks the index location in the command_details_ref_table
  as indicated by the ref_id as available for upcoming use

PARAMETERS
  ref_id: [Input] index location in the command_details_ref_table to be marked
                  free

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
extern void gstk_util_cmd_details_ref_table_free_slot(uint32 ref_id);

/*===========================================================================
FUNCTION gstk_util_env_cmd_ref_table_get_next_free_slot

DESCRIPTION
  This function returns the next available slot in the
  envelope_cmd_ref_table

PARAMETERS
  None

DEPENDENCIES
    None

RETURN VALUE
    reference id for the next available slot

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint32 gstk_util_env_cmd_ref_table_get_next_free_slot( void ) ;

/*===========================================================================
FUNCTION gstk_util_env_cmd_ref_table_free_slot

DESCRIPTION
  This function marks the index location in the envelope_cmd_ref_table
  as indicated by the ref_id as available for upcoming use

PARAMETERS
  ref_id: [Input] index location in the envelope_cmd_ref_table to be marked
                  free

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
extern void gstk_util_env_cmd_ref_table_free_slot(uint32 ref_id);

/*===========================================================================
FUNCTION gstk_util_save_env_cmd_info

DESCRIPTION
  This function get a free slot from the envelope_cmd_ref_table, and populate
  it with the user envelope command info.

PARAMETERS
  client_id: [Input] client id for referencing the client callback if the
                     envelope command is not sent by internal modules
  env_cmd_type: [Input] envelope command type
  cmd_ptr: [Input] command data
  env_ref_id_ptr: [Input/Output] envelope reference id in the
                             envelope_cmd_ref_table

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
extern gstk_status_enum_type gstk_util_save_env_cmd_info(
  gstk_client_id_type     client_id,
  gstk_cmd_enum_type      env_cmd_type,
  gstk_cmd_type         * cmd_ptr,
  uint32                * env_ref_id_ptr
);


/*===========================================================================
FUNCTION gstk_util_retrieve_env_cmd_info

DESCRIPTION
  This function get a free slot from the envelope_cmd_ref_table, and populate
  it with the user envelope command info.

PARAMETERS
  user_data_ptr: [Output] user data
  env_cmd_type: [Input] envelope command type
  env_cb: [Input] function to be called upon response from Card is received
                  for the envelope command
  env_ref_id: [Input] envelope reference id in the
                             envelope_cmd_ref_table

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
 gstk_status_enum_type gstk_util_retrieve_env_cmd_info(
     gstk_cmd_enum_type      env_cmd_type,
     uint32                  *user_data_ptr,
     gstk_evt_cb_funct_type  *env_cb_ptr,
     uint32                  env_ref_id);

/*===========================================================================
FUNCTION GSTK_VALID_TAG

DESCRIPTION
  Main wait routine for GSTK task.

DEPENDENCIES
  None

RETURN VALUE

===========================================================================*/
extern boolean gstk_valid_tag(uint8 tag,tags_enum_type type);

/*===========================================================================
FUNCTION GSTK_DECODE_TEXT_STRING_BODY

DESCRIPTION
    Stores decoded textual string in global text_string.text
    Used in text_string.length ^ text_string.data_coding_scheme

DEPENDENCIES
    If default is TRUE use default text, else use text_string

RETURN VALUE
    Updated Counter Value

===========================================================================*/
extern uint8 gstk_decode_text_string_body(
    tags_enum_type   tag,
    const uint8      *command_data,
    int32            offset,
    gstk_string_type *text_string);

/*===========================================================================
FUNCTION gstk_parser_text_string_tlv

DESCRIPTION
    This function parses the text string/default text/ussd string TLV and
    populate the gstk_string_type passed in the function call with the info
    in the TLV.
    It sets the DCS, length of text data and data.  Based on the DCS values,
    the data has been decode accordingly.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE

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
extern gstk_status_enum_type gstk_parser_text_string_tlv(
    uint8             *orig_cmd_data,
    int32             cmd_data_len,
    int32             orig_offset,
    tags_enum_type    tag,
    gstk_string_type  *text_string,
    int32             *next_tlv_offset,
    boolean           *more_tlvs,
    boolean           man_opt_specifier,
    const uint32      *ref_id);

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
    no more TLVs to be parsed, the more will be set to FALSE

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
extern gstk_status_enum_type gstk_parser_icon_id_tlv(
    const gstk_string_type   * string_tlv,
    uint8                    * orig_cmd_data,
    int32                    cmd_data_len,
    int32                    orig_offset,
    gstk_icon_type           * icon,
    int32                    * next_tlv_offset,
    boolean                  * more_tlvs,
    boolean                   man_opt_specifier,
    const uint32             * ref_id);

/*===========================================================================
FUNCTION gstk_parser_response_len_tlv

DESCRIPTION
    This function parses the response length TLV and populate the max and min
    len passed in the function call with the info in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

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
extern gstk_status_enum_type gstk_parser_response_len_tlv(
    uint8             *orig_cmd_data,
    int32             cmd_data_len,
    int32             orig_offset,
    uint8             *max_len,
    uint8             *min_len,
    int32             *next_tlv_offset,
    boolean           *more_tlvs,
    boolean            man_opt_specifier,
    const uint32      *ref_id);

/*===========================================================================
FUNCTION gstk_parser_tone_tlv

DESCRIPTION
    This function parses the tone TLV and populate the gstk_tone_enum_type
    passed in the function call with the info in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

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
extern gstk_status_enum_type gstk_parser_tone_tlv(
    uint8                 *orig_cmd_data,
    int32                 cmd_data_len,
    int32                 orig_offset,
    gstk_tone_enum_type   *tone,
    int32                 *next_tlv_offset,
    boolean               *more_tlvs,
    boolean                man_opt_specifier,
    const uint32          *ref_id);

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
extern gstk_status_enum_type gstk_parser_duration_tlv(
    uint8                 *orig_cmd_data,
    int32                 cmd_data_len,
    int32                 orig_offset,
    gstk_duration_type    *duration,
    int32                 *next_tlv_offset,
    boolean               *more_tlvs,
    boolean                man_opt_specifier,
    const uint32          *ref_id);

/*===========================================================================
FUNCTION gstk_parser_evt_list_tlv

DESCRIPTION
    This function parses the event list TLV and populate the
    gstk_evt_list_code_enum_type passed in the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to
    FALSE

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
extern gstk_status_enum_type gstk_parser_evt_list_tlv(
    uint8                           *orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    uint8                           *evt_list_mask,
    int32                           *next_tlv_offset,
    boolean                         *more_tlvs,
    boolean                          man_opt_specifier,
    const uint32                    *ref_id);

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
extern gstk_status_enum_type gstk_parser_address_ss_string_tlv(
    tags_enum_type                  tag,
    uint8                           *orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    gstk_address_type               *address,
    int32                           *next_tlv_offset,
    boolean                         *more_tlvs,
    boolean                          man_opt_specifier,
    const uint32                    *ref_id);

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
extern gstk_status_enum_type gstk_parser_subaddress_tlv(
    uint8                           *orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    gstk_subaddress_type            *subaddress,
    int32                           *next_tlv_offset,
    boolean                         *more_tlvs,
    boolean                          man_opt_specifier,
    const uint32                    *ref_id);

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
extern gstk_status_enum_type gstk_parser_cap_config_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_cap_config_type           *ccp,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_browser_id_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    uint8                          *browser_id,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_url_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_url_type                  *url,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

/*===========================================================================
FUNCTION gstk_parser_bearer_tlv

DESCRIPTION
    This function parses the url TLV and populate the
    passed in parameter from the function call with the info
    in the TLV.
    It also returns the index slot for next TLV's tag if available.  If there is
    no more TLVs to be parsed, the more tlv will be set to FALSE

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
extern gstk_status_enum_type gstk_parser_bearer_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_bearer_type               *bearer,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_provisioning_ref_file_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_file_list_type            *provisioning_file,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
  more_tlvs: [Input/Output] Pointer to boolean to indicate whether there are mo
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
extern gstk_status_enum_type gstk_parser_file_list_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_file_list_type            *file_list,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
    const uint32                   *ref_id);

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
  nmr_utran_qualifier_ptr:   [Output] Pointer to enum type for NMR UTRAN
                             Qualifier

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
extern gstk_status_enum_type gstk_parser_pli_nmr_utran_tlv(
    uint8                               *orig_cmd_data_ptr,
    int32                                cmd_data_len,
    int32                                orig_offset,
    tags_enum_type                       tag,
    int32                               *next_tlv_offset_ptr,
    boolean                             *more_tlvs_ptr,
    boolean                              man_opt_specifier,
    const uint32                        *ref_id_ptr,
    gstk_nmr_utran_qualifier_enum_type  *nmr_utran_qualifier_ptr);

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
  is_cdma_sms: [Input] Indicates if the tpdu is CDMA or GSM/WCDMA
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
extern gstk_status_enum_type gstk_parser_sms_tpdu_tlv(
    uint8                 *orig_cmd_data,
    int32                 cmd_data_len,
    int32                 orig_offset,
    gstk_sms_tpdu_type    *tpdu,
    boolean               *is_cdma_sms,
    int32                 *next_tlv_offset,
    boolean               *more_tlvs,
    boolean                man_opt_specifier,
    const uint32          *ref_id);

/*===========================================================================
FUNCTION GSTK_PARSER_MENU_NUM_ITEMS

DESCRIPTION
    Generic Function used by GSTK to get the Characteristics for
    a Setup Menu Proactive Command

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type gstk_parser_menu_num_items(
    const uint8   * cmd_data,
    const int32   * cmd_data_len,
    uint8         * num_menu_items);

/*===========================================================================
FUNCTION gstk_parser_alpha_identifier_tlv

DESCRIPTION
    Generic Function used by GSTK to parse the Alpha Identifer TLV

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type gstk_parser_alpha_identifier_tlv(
    uint8                     * orig_cmd_data,
    int32                       cmd_data_len,
    int32                       orig_offset,
    gstk_string_type          * alpha,
    int32                     * next_tlv_offset,
    boolean                   * more_tlvs,
    boolean                     man_opt_specifier,
    const uint32              * ref_id);

/*===========================================================================
FUNCTION GSTK_PARSER_STRIP_IDENTIFIER_TLV

DESCRIPTION
    Generic Function used by GSTK to remove Alpha Identifier from the TLV

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type gstk_parser_strip_alpha_identifer_tlv(
    uint8                 * orig_cmd_data,
    int32                 * cmd_data_len,
    uint8                 * cmd_data_no_alpha_id,
    uint8                 * cmd_data_no_alpha_id_len);

/*===========================================================================
FUNCTION GSTK_UTIL_GET_TEXT_BUFFER

DESCRIPTION
    Generic Function used by GSTK to remove Alpha Identifier from the TLV

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern uint8 *gstk_util_get_text_buffer (
    uint8 * num_bytes);

/*===========================================================================
FUNCTION GSTK_PARSER_POPULATE_ITEMS

DESCRIPTION
    Generic Function used by Populate Items for Menu Items or Select Items

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type  gstk_parser_item_tlv(
    uint8                     * orig_cmd_data,
    int32                       cmd_data_len,
    int32                       orig_offset,
    gstk_item_object_type     * item,
    int32                     * next_tlv_offset,
    boolean                   * more_tlvs,
    boolean                     man_opt_specifier,
    const uint32              * ref_id);

/*===========================================================================
FUNCTION GSTK_PARSER_NEXT_ACTION_INDICATOR_TLV

DESCRIPTION
    Generic Function used to parse the Next Action Indicator TLV.

    Next Action Indicator TLV Appear As:
        ITEMS NEXT ACTION
        INDICATOR TAG       - 1 BYTE
        LENGTH (X)          - 1 BYTE
        ITEMS NEXT ACTION
        INDICATOR LIST      - X BYTES (Determined by LENGTH (X) )

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type  gstk_parser_next_action_indicator_tlv(
    uint8                           * orig_cmd_data,
    int32                             cmd_data_len,
    int32                             orig_offset,
    gstk_next_item_ind_type         * next_action_ind_list,
    int32                           * next_tlv_offset,
    boolean                         * more_tlvs,
    boolean                           man_opt_specifier,
    const uint32                    *  ref_id);

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
extern gstk_status_enum_type  gstk_parser_item_icon_identifier_list_tlv(
    uint8                           * orig_cmd_data,
    int32                             cmd_data_len,
    int32                             orig_offset,
    gstk_icon_id_list_type          * icon_id_list,
    int32                           * next_tlv_offset,
    boolean                         * more_tlvs,
    boolean                           man_opt_specifier,
    const uint32                    * ref_id);


/*===========================================================================
FUNCTION gstk_parse_img_record

DESCRIPTION
    Generic Function used to parse the Image

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type gstk_parse_img_record(
    uint8               rec_num,
    gstk_icon_type      *icon);

/*===========================================================================
FUNCTION gstk_parser_bc_repeat_ind_tlv

DESCRIPTION
    This function parses the bc repeat indicator TLV and populate the
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
extern gstk_status_enum_type gstk_parser_bc_repeat_ind_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_bc_repeat_ind_enum_type   *bc_repeat,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_timer_id_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    uint8                          *timer_id,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_timer_value_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_timer_value_type          *timer_value,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_lang_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_lang_type                 *lang,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

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
extern gstk_status_enum_type gstk_parser_dtmf_tlv(
    uint8                          *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_dtmf_type                 *dtmf,
    int32                          *next_tlv_offset,
    boolean                        *more_tlvs,
    boolean                         man_opt_specifier,
    const uint32                   *ref_id);

/*===========================================================================
FUNCTION gstk_packer_cmd_details_tlv

DESCRIPTION
  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  ref_id: [Input] Index location in the command_Details_ref_table
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
extern gstk_status_enum_type gstk_packer_cmd_details_tlv(
    uint32                             ref_id,
    uint32                             command_rsp_type,
    uint8                              command_number,
    gstk_command_details_tlv_type *    command_details);

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
extern gstk_status_enum_type gstk_packer_dev_id_tlv(
    gstk_device_identity_type                     source,
    gstk_device_identity_type                     destination,
    gstk_device_identities_tag_tlv_type * device_id);

/*===========================================================================
FUNCTION gstk_packer_loc_info_tlv

DESCRIPTION
This function populates the passed in location info pointer

PARAMETERS
  loc_info: [Input/Output] Pointer to tlv
  mcc_mnc: [Input] Pointer to mcc mnc information

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
extern gstk_status_enum_type gstk_packer_loc_info_tlv(
    gstk_location_information_tag_tlv_type * loc_info,
    const uint8                            * mcc_mnc);

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
extern gstk_status_enum_type gstk_packer_result_tlv(
    gstk_general_result_enum_type         command_result,
    gstk_additional_info_ptr_type         additional_info,
    gstk_result_tag_tlv_type            * result);

/*===========================================================================
FUNCTION gstk_packer_text_string_tlv

DESCRIPTION
  This function populates the passed in result tlv pointer

PARAMETERS
  tag: [Input] Indicates TEXT_STRING_TAG or SS_STRING_TAG
  qualifier: [Input] Indicates the text string coding scheme
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
extern gstk_status_enum_type gstk_packer_text_string_tlv(
    tags_enum_type          tag,
    uint8                   qualifier,
    uint8                   data_len,
    const uint8             *data,
    gstk_string_tlv_type    *string_tlv );

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
extern gstk_status_enum_type gstk_packer_alpha_tlv(
    const gstk_string_type *data,
    gstk_alpha_tlv_type    *string_tlv);

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
extern gstk_status_enum_type gstk_packer_location_info_tlv(
    const gstk_location_info_rsp_type    *data,
    gstk_location_information_tlv_type   *location_info_tlv);

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_packer_3gpp2_location_info_tlv

DESCRIPTION

  This function populates the passed in location info tlv pointer

PARAMETERS
  data: [Input] Indicates the location info
  cache: [Input] the current GSTK location info cache
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
extern gstk_status_enum_type gstk_packer_3gpp2_location_info_tlv(
       const gstk_cdma_loci_ext_type             *data,
       gstk_cdma_loci_ext_cache_type             *cache,
       gstk_cdma_loci_ext_tlv_type               *location_info_tlv
);
#endif /*#ifdef FEATURE_CDMA */

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
extern gstk_status_enum_type gstk_packer_imei_tlv(
    const uint8           *data,
    gstk_imei_tlv_type    *imei_tlv);

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_packer_esn_tlv

DESCRIPTION
  This function populates the passed in imei tlv pointer

PARAMETERS
  data    : [Input] Indicates the esn
  esn_tlv : [Input/Output] Pointers to which the esn to be populated

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
extern gstk_status_enum_type gstk_packer_esn_tlv(
    const uint8         *data,
    gstk_esn_tlv_type   *esn_tlv);

/*===========================================================================
FUNCTION gstk_packer_meid_tlv

DESCRIPTION
  This function populates the passed in meid tlv pointer

PARAMETERS
  data    : [Input] Indicates the meid
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
extern gstk_status_enum_type gstk_packer_meid_tlv(
    const uint8          *data,
    gstk_meid_tlv_type   *meid_tlv);
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
extern gstk_status_enum_type gstk_packer_imeisv_tlv(
    const uint8            *data,
    gstk_imeisv_tlv_type   *imeisv_tlv);

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
extern gstk_status_enum_type gstk_packer_nmr_tlv(
    const uint8           *data,
    gstk_nmr_tlv_type     *nmr_tlv);

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
extern gstk_status_enum_type gstk_packer_bcch_list_tlv(
    uint8                         data_len,
    const uint8                   *data,
    gstk_bcch_ch_list_tlv_type    *bcch_list_tlv);

/*===========================================================================
FUNCTION gstk_packer_nmr_utran_tlv

DESCRIPTION
  This function packs the nmr data in the nmr utran tlv pointer

PARAMETERS
  length:            [Input] Length of data
  data_ptr:          [Input] Pointer from which nmr data is to be copied
  nmr_utran_tlv_ptr: [Input/Output] Pointers to which the nmr to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NULL_INPUT_PARAM:  Null input param
    GSTK_BAD_PARAM:         Bad input paramter
    GSTK_SUCCESS:           Success

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_packer_nmr_utran_tlv(
    uint32                        length,
    const uint8                   *data_ptr,
    gstk_nmr_utran_tlv_type       *nmr_utran_tlv_ptr);

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
extern gstk_status_enum_type gstk_packer_timing_advance_tlv(
    const gstk_time_advance_rsp_type     *data,
    gstk_timing_advance_tlv_type         *timing_adv_tlv);

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
extern gstk_status_enum_type gstk_packer_lang_sel_tlv(
    const uint8           *data,
    gstk_lang_tlv_type    *lang_tlv);

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
extern gstk_status_enum_type gstk_packer_date_time_tlv(
    const gstk_date_time_rsp_type   *data,
    gstk_date_time_tlv_type         *time_tlv);

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
extern gstk_status_enum_type gstk_packer_address_tlv(
    tags_enum_type            tag,
    const gstk_address_type   *data,
    gstk_address_tag_tlv_type *address_tlv);

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
extern gstk_status_enum_type gstk_packer_subaddress_tlv(
    const gstk_subaddress_type   *data,
    gstk_subaddress_tlv_type     *subaddress_tlv);

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
extern gstk_status_enum_type gstk_packer_ccp_tlv(
    const gstk_cap_config_type   *data,
    gstk_cap_config_tlv_type     *ccp_tlv);

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
extern gstk_status_enum_type gstk_packer_bc_repeat_ind_tlv(
    gstk_bc_repeat_ind_enum_type         data,
    gstk_bc_repeat_ind_tlv_type          *bc_repeat_ind_tlv);

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
extern gstk_status_enum_type gstk_packer_cc_req_action_tlv(
    const gstk_cc_req_action_type     *data,
    gstk_cc_req_action_tlv_type       *cc_req_action_tlv);

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
extern gstk_status_enum_type gstk_packer_duration_tlv(
    const gstk_duration_type    *data,
    gstk_duration_tlv_type      *duration_tlv);

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
extern gstk_status_enum_type gstk_packer_item_id_tlv(
    uint8                            chosen_item_identifier,
    gstk_item_identifier_tlv_type   *item_id_tlv);

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
extern gstk_status_enum_type gstk_packer_help_request_tlv(
    boolean                          data,
    gstk_help_request_tlv_type       *help_tlv);

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
extern gstk_status_enum_type gstk_packer_transaction_id_tlv(
    const gstk_transaction_id_type   *data,
    gstk_transaction_id_tlv_type     *transaction_id_tlv);

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
extern gstk_status_enum_type gstk_packer_evt_list_tlv(
    gstk_evt_list_code_enum_type       data,
    gstk_evt_list_tlv_type             *evt_list_tlv);

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
extern gstk_status_enum_type gstk_packer_cause_tlv(
    const gstk_cause_type   *data,
    gstk_cause_tlv_type     *cause_tlv);

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
extern gstk_status_enum_type gstk_packer_location_status_tlv(
    gstk_location_status_enum_type         data,
    gstk_location_status_tlv_type          *location_status_tlv);

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
extern gstk_status_enum_type gstk_packer_sms_tpdu_tlv(
    boolean                             is_cdma_tpdu,
    const gstk_sms_tpdu_type            *data,
    gstk_sms_tpdu_tag_tlv_type          *tpdu_tlv);

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
extern gstk_status_enum_type gstk_packer_browser_term_cause_tlv(
    const gstk_browser_term_cause_enum_type  *cause,
    gstk_browser_term_cause_tlv_type         *cause_tlv);

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
extern gstk_status_enum_type gstk_packer_timer_id_tlv(
    const uint8            *timer_id,
    gstk_timer_id_tlv_type *timer_id_tlv);

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
extern gstk_status_enum_type gstk_packer_timer_value_tlv(
    const gstk_timer_value_type *timer_value,
    gstk_timer_value_tlv_type   *timer_value_tlv);

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
extern gstk_status_enum_type  gstk_parser_item_identifier_tlv(
    uint8                               * orig_cmd_data,
    int32                                 cmd_data_len,
    int32                                 orig_offset,
    uint8                               * default_item,
    int32                               * next_tlv_offset,
    boolean                             * more_tlvs,
    boolean                               man_opt_specifier,
    const uint32                        * ref_id);

/*===========================================================================
FUNCTION gstk_get_uim_buf

DESCRIPTION
  This function returns a pointer to heap for buffer allocation for UIM
  Server interface.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.
===========================================================================*/
extern uim_cmd_type *gstk_get_uim_buf ( void );

/*===========================================================================
FUNCTION gstk_send_cmd_to_uim_server

DESCRIPTION

  This function sends the uim cmd to the uim server

PARAMETERS
  uim_cmd_ptr: [Input] Pointer to uim_cmd_type to be sent to uim server

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
extern void gstk_send_cmd_to_uim_server ( uim_cmd_type *cmd_ptr );

/*===========================================================================
FUNCTION gstk_send_cmd_to_uim_free_q

DESCRIPTION
  This function sends the uim cmd back to uim free queue

PARAMETERS
  uim_cmd_ptr: [Input] Pointer to uim_cmd_type to be sent to uim server

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
extern void gstk_send_cmd_to_uim_free_q (uim_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION gstk_util_populate_uim_buf_hdr_info

DESCRIPTION
  This function gets a pointer for uim_cmd_ptr, and populates header info
  of the uim_cmd_type based on the uim_cmd_name_type

PARAMETERS
  uim_cmd_type: [Input] Indicates which type of UIM Command
  uim_cmd_ptr: [Input/Output] Pointer to uim_cmd_type, which will be sent to
                              uim server eventually

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
extern gstk_status_enum_type gstk_util_populate_uim_buf_hdr_info(
    uim_cmd_name_type                 uim_cmd_name,
    uim_cmd_type                    ** uim_cmd_ptr );

/*===========================================================================
FUNCTION GSTK_FIND_LENGTH_OF_LENGTH_VALUE

DESCRIPTION
  This function determines the size of the length field:
  first byte of length field [0x00, 0x7F] -> length field size == 1
  first byte of length field == 0x81 -> length field size == 2

PARAMETERS
  length_value: [Input] pointer to the length value field

DEPENDENCIES
  None

RETURN VALUE
  byte -> length of the length field in TLV

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint32 gstk_find_length_of_length_value(const uint8* length_value);

/*===========================================================================
FUNCTION gstk_util_is_valid_client_id

DESCRIPTION
  This function determines if the client_id is valid or not

PARAMETERS
  client_id: [Input] Client ID to be checked

DEPENDENCIES
  None

RETURN VALUE
  boolean -> valid ID or not

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_valid_client_id(gstk_client_id_type  client_id );

/*===========================================================================
FUNCTION gstk_util_is_valid_data_format_req

DESCRIPTION
  This function determines if the client has requested a valid data format.

PARAMETERS
  data_format_type: [Input] Data format type requested by the client

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE  - if data format type is valid
    FALSE - if data format type is invalid

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_valid_data_format_req(
  gstk_client_pro_cmd_data_format_enum_type   data_format_type);

/*===========================================================================
FUNCTION gstk_util_is_valid_func_type

DESCRIPTION
  This function determines if the functionality type provided is valid

PARAMETERS
  func_type: [Input] Functionality specified by the client

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE  - if functionality type is valid
    FALSE - if functionality type is invalid

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_valid_func_type(
  gstk_tk_functionality_enum_type func_type);

/*===========================================================================
FUNCTION gstk_util_convert_cmd_type_to_reg_bitmask

DESCRIPTION
  This function converts the command type into the registration bitmask

PARAMETERS
  cmd_type:          [Input] type of command that GSTK is going to search for in the
                             client table
  cmd_bitmask_ptr:   [Input/Output] Pointer to bitmask value for the cmd_type
                              input
  class_bitmask_ptr: [Input/Output] Pointer to bitmask value for the class to
                                    which the cmd belongs

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NULL_INPUT_PARAM    -  Null Input Parameter
    GSTK_UNSUPPORTED_COMMAND -  Unsupported command
    GSTK_SUCCESS             -  Successful conversion of cmd type to
                                bitmask

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_convert_cmd_type_to_reg_bitmask(
    gstk_cmd_enum_type   cmd_type,
   uint64               *cmd_bitmask_ptr,
   uint32               *class_bitmask_ptr);

/*===========================================================================
FUNCTION gstk_util_find_client_with_registered_event

DESCRIPTION
  This function goes through the gstk_client_table and the class event
  registration table to see which client has registered for the event

PARAMETERS
  client_id_ptr:[Input] Pointer to client id
  cmd_type:     [Input] type of command that GSTK is going to search for in the
                        registration table
  format_type:  [Input] Format type (RAW/GSTK)
  cli_func_type:[Input] Client functionality type (MODEM/UI/ALL)

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type
    GSTK_NO_CLIENT_REG_FOUND - no client registered for the command.
    GSTK_SUCCESS             - registered client for the specific cmd found
    GSTK_ERROR               - error in finding a registered client
    GSTK_NULL_INPUT_PARAM    - null pointer provided as input
    GSTK_BAD_PARAM           - Bad parameter passed in
    GSTK_INVALID_CLIENT_TYPE - Invalid client id found in class evt reg table

COMMENTS
  There should only be 1 client registers for each command event for a
  specific functionality (MODEM/UI/ALL) at any given time

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_find_client_with_registered_event(
    gstk_client_id_type                        *client_id_ptr,
    gstk_cmd_enum_type                          cmd_type,
    gstk_client_pro_cmd_data_format_enum_type   format_type,
    gstk_tk_functionality_enum_type             cli_func_type);

/*===========================================================================
FUNCTION gstk_util_send_message_to_clients

DESCRIPTION
  This function sends the message to the GSTK clients based on which clients
  have registered to receive that events.  Upon returning from this function,
  the message_p is free

PARAMETERS
  message_ptr: [Input] Pointer to message to be sent to clients

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
extern gstk_status_enum_type gstk_util_send_message_to_clients(
    gstk_cmd_from_card_type * message_ptr );

/*===========================================================================
FUNCTION gstk_util_send_tr_to_clients

DESCRIPTION
  This function sends the terminal response to the GSTK clients based on which clients
  have registered to receive TR.  Upon returning from this function,
  the message_p is free

PARAMETERS
  message_ptr: [Input] Pointer to message to be sent to clients

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
extern gstk_status_enum_type gstk_util_send_tr_to_clients(
    gstk_cmd_from_card_type * message_ptr );

/*===========================================================================
FUNCTION gstk_util_send_response_to_clients

DESCRIPTION
  This function sends the Card Response to the GSTK clients based on which
  client registration callback function (used for Envelope Response)

PARAMETERS
  message_p: [Input] Pointer to message to be sent to clients
  env_ref_id: [Input] Envelope ref id in the envelope_cmd_ref_table

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
extern gstk_status_enum_type gstk_util_send_response_to_clients(
    gstk_cmd_from_card_type * message_p, uint32 env_ref_id );

/*===========================================================================
FUNCTION gstk_util_start_client_response_timer

DESCRIPTION
  This function starts the terminal response timer for the proactive command
  that is just sent to the client

PARAMETERS
  ref_id: [Input] The timer that needs to be started
  msecs:  [Input] Timer expiration value to be set in milliseconds

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_start_client_response_timer(
  uint32             ref_id,
  rex_timer_cnt_type msecs);

/*===========================================================================
FUNCTION gstk_util_release_upper_layer_cmd_memory

DESCRIPTION
  This function frees all the allocated memory for the upper layer (U)SAT
  command response pointer input.

PARAMETERS
  message_p: [Input] Pointer to message to which memory location is required
                     to be freed

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_release_upper_layer_cmd_memory(
    gstk_cmd_type * message_p );

/*===========================================================================
FUNCTION gstk_util_check_evt_mask_reg_status

DESCRIPTION
  This function check if the input bitmask has already been registered by
  any of the other clients.

PARAMETERS
  evt_reg_bitmask: [Input] Bitmask to be checked against the class evt
                           registration table
  class_bitmask:   [Input] Letter classes for ME hardware dependant commands
  func_type:       [Input] Client functionality type (MODEM/UI/ALL)

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
/*lint -e715 not referenced */
extern gstk_status_enum_type gstk_util_check_evt_mask_reg_status(
  uint64                                           evt_reg_bitmask,
  uint32                                           class_bit_mask,
  gstk_tk_functionality_enum_type                  func_type);

/*===========================================================================
FUNCTION gstk_util_check_evt_bitmask

DESCRIPTION
  This function check if the input bitmask has already been registered by
  any of the other clients.

PARAMETERS
  class_bit_mask:  [Input] Letter classes for ME hardware dependant commands
  evt_reg_bitmask: [Input] Bitmask to be checked against the
                           gstk_client_table

DEPENDENCIES
  None

RETURN VALUE
  boolean
    FALSE:  If command indicated by evt bitmask does not
            belong to the class specified by class bitmask.
    TRUE:   If command indicated by evt bitmask belongs to the
            class specified by the class bitmask.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_check_evt_bitmask(
  uint32   class_bit_mask,
  uint64   evt_reg_bitmask
);

/*===========================================================================
FUNCTION gstk_util_extract_class_specfic_evt_bitmask

DESCRIPTION
  This function extracts the event bitmask corresponding to a particular class

PARAMETERS
  client_id:       [Input]        client id
  evt_reg_bitmask: [Input]        event register bitmask
  bitmask_ptr:     [Input/Output] pointer to bitmask
  proactive_cmd_class:   [Input]        class bitmask

DEPENDENCIES
  None

RETURN VALUE
  GSTK_INVALID_CLIENT_TYPE - Invalid client id
  GSTK_NULL_INPUT_PARAM    - Null input paramater
  GSTK_SUCCESS             - successful extraction of events based
                             on class mask

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_extract_class_specfic_evt_bitmask(
  uint64                                     evt_reg_bitmask,
  uint64                                    *bitmask_ptr,
  uint32                                     proactive_cmd_class,
  uint64                                    *rem_bitmask_ptr);


/*===========================================================================
FUNCTION gstk_util_is_evt_in_category

DESCRIPTION
  This function check if the input event bitmask belongs to the class
  represented by class class_bitmask

PARAMETERS
  evt_reg_bitmask: [Input] Event Bitmask
  class_bitmask  : [Input] Class Bitmask

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE:  If any events in mask belongs to class_bitmask
    FALSE: If  event in mask do not belong to class class_bitmask

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_evt_in_category(
  uint64   evt_reg_bitmask,
  uint64   proactive_cmd_class);


/*===========================================================================
FUNCTION gstk_util_translate_qualifier_to_dcs

DESCRIPTION
  This function translates the qualifier into the corresponding data
  coding scheme.

PARAMETERS
  qualifier: [Input] qualifier from the (U)SAT command

DEPENDENCIES
  None

RETURN VALUE
  byte -> data coding scheme

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint8 gstk_util_translate_qualifier_to_dcs(
    uint8                     qualifier );

/*===========================================================================
FUNCTION gstk_util_build_evt_dl_ind

DESCRIPTION
  This function populates the gstk_task_cmd_type based on the passed in
  event download indication

PARAMETERS
  cmd: [Input/Output] Pointer to gstk_task_cmd_type to be populated
  env_ref_id: [Input] envelope_cmd_ref_table reference id
  evt_list: [Input] event download indication type from the client

DEPENDENCIES
  None

RETURN VALUE
  byte -> data coding scheme

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_build_evt_dl_ind(
    gstk_task_cmd_type          * cmd,
    uint32                        env_ref_id,
    const gstk_evt_dl_ind_type  * evt_list);

/*===========================================================================
FUNCTION gstk_util_map_mm_to_gstk_location_status

DESCRIPTION
  This function will map the location status from MM to GSTK enum type.

PARAMETERS
  mm_idle_cmd: [Input] Pointer to gstk_mm_idle_state_type
  location_status: [Input/Output] pointer to location status

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
extern gstk_status_enum_type gstk_util_map_mm_to_gstk_location_status(
    const gstk_mm_idle_state_type   * mm_idle_cmd,
    gstk_location_status_enum_type  * location_status);

/*===========================================================================
FUNCTION gstk_util_is_valid_location_info

DESCRIPTION
  This function will check if the MNC, MCC, LAC, CellID and location
  status info is valid combination.

PARAMETERS
  mcc_mnc: [Input]
  lac: [Input]
  cell_id: [Inputt]
  location_status: [Input]

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_valid_location_info(
    const uint8                     * mcc_mnc,
    const uint8                     * lac,
    const uint8                     * cell_id,
    gstk_location_status_enum_type    location_status);

/*===========================================================================
FUNCTION gstk_util_check_mm_state_difference

DESCRIPTION
  This function will get the cell id and determines whether there are any
  differences between the new location info/state with the old new.  This
  function will return cell_id as a result of the operation.

PARAMETERS
  mm_idle_cmd: [Input] Pointer to gstk_mm_idle_state_type
  cell_id: [Input/Output] pointer to cell id to be populated
  location_status: [Input] pointer to location status

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_check_mm_state_difference(
    const uint8                    * mcc_mnc,
    const uint8                    * lac,
    const uint8                    * cell_id,
    const int32                      cell_len,
    gstk_location_status_enum_type   location_status);

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_util_diff_cdma_ext_loc_info

DESCRIPTION
  This function will diff the old 3GPP2 cache with the new 3GPP2 cache

PARAMETERS
  cdma_ext_loc_info [Input] 3GPP2 location information to be compared

DEPENDENCIES
  None

RETURN VALUE 
  FALSE : There is no difference in 3GPP2 location information
  TRUE  : There is difference in 3GPP2 location information

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_diff_cdma_ext_loc_info(
  gstk_cdma_loci_ext_cache_type* old_cache,
  gstk_cdma_loci_ext_cache_type* new_cache);
#endif /* #ifdef FEATURE_CDMA */

/*===========================================================================
FUNCTION gstk_util_compare_curr_and_next_location_info

DESCRIPTION
  This function will compare the previously sent location info with
  the one in the next available one

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_compare_curr_and_next_location_info( void );

/*===========================================================================
FUNCTION gstk_util_pack_7_bit_chars

DESCRIPTION
  This function packs the input data from SMS7 bit format into ASCII.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  uint16:length of output data length

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint16 gstk_util_pack_7_bit_chars(
    const uint8     *in,
    uint16          in_len,
    uint8           *out );

/*===========================================================================
FUNCTION gstk_util_unpack_7_bit_chars

DESCRIPTION
  This function unpacks the input data from SMS7 bit format into ASCII.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  uint8:length of output data length

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint8 gstk_util_unpack_7_bit_chars(
    const uint8       *in,
    uint8             in_len,
    uint8             *out );

/*===========================================================================
FUNCTION gstk_util_has_pending_timer_envelope

DESCRIPTION
  This function based on the timer management table, determine if there is
  any pending timer expiration envelope that needs to be sent to the card

PARAMETERS
  slot [Input] The slot of which pending timer envelope to be checked.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_has_pending_timer_envelope(
  gstk_slot_id_enum_type slot
);

/*===========================================================================
FUNCTION gstk_util_find_first_timer_expire_command

DESCRIPTION
  This function find the first pending timer expire command available.

PARAMETERS
  slot [Input] the slot of which the timer expire command to be checked

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern uint16 gstk_util_find_first_timer_expire_command(
  gstk_slot_id_enum_type slot
);

/*===========================================================================
FUNCTION gstk_util_has_pending_setup_call

DESCRIPTION
  This function find if there is any pending setup call proactive command

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_has_pending_setup_call(void);

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
extern gstk_status_enum_type gstk_packer_cell_broadcast_page_tlv(
    const gstk_cb_page_type                *cb_page,
    gstk_cell_broadcast_page_tag_tlv_type  *cb_page_tlv);

/*===========================================================================
FUNCTION gstk_parser_bearer_description_tlv

DESCRIPTION
    Generic Function used by GSTK to parse the Bearer Description TLV
DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type gstk_parser_bearer_description_tlv(
    uint8                         * orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    gstk_bearer_description_type  * bearer_description,
    int32                         * next_tlv_offset,
    boolean                       * more_tlv,
    boolean                         man_opt_specifier,
    const uint32                  * ref_id);

/*===========================================================================
FUNCTION gstk_parser_buffer_size_tlv

DESCRIPTION
    Generic Function used by GSTK to parse the buffer size TLV

DEPENDENCIES

RETURN VALUE

===========================================================================*/
extern gstk_status_enum_type gstk_parser_buffer_size_tlv(
    uint8                         * orig_cmd_data,
    int32                           cmd_data_len,
    int32                           orig_offset,
    int32                         * buffer_size,
    int32                         * next_tlv_offset,
    boolean                       * more_tlv,
    boolean                         man_opt_specifier,
    const uint32                  * ref_id);

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
extern gstk_status_enum_type gstk_parser_alpha_identifier_tlv_ext(
    tags_enum_type                 tag,
    uint8                         *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_alphabet_type            *alpha,
    int32                         *next_tlv_offset,
    boolean                       *more_tlv,
    boolean                        man_opt_specifier,
    const uint32                  *ref_id);

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
extern gstk_status_enum_type gstk_parser_icon_id_tlv_ext(
    const gstk_alphabet_type * alpha_tlv,
    uint8                    * orig_cmd_data,
    int32                    cmd_data_len,
    int32                    orig_offset,
    gstk_icon_type           * icon,
    int32                    * next_tlv_offset,
    boolean                  * more_tlv,
    boolean                   man_opt_specifier,
    const uint32             * ref_id);

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
extern gstk_status_enum_type gstk_parser_other_address_tlv(
    uint8                         *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_pkt_data_address_type    *other_address,
    int32                         *next_tlv_offset,
    boolean                       *more_tlv,
    boolean                        man_opt_specifier,
    const uint32                  *ref_id);

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
extern gstk_status_enum_type gstk_parser_sim_me_interface_transport_level_tlv(
    uint8                                *orig_cmd_data,
    int32                                 cmd_data_len,
    int32                                 orig_offset,
    gstk_pkt_data_transport_level_type   *transport_level,
    int32                                *next_tlv_offset,
    boolean                              *more_tlv,
    boolean                               man_opt_specifier,
    const uint32                         *ref_id);

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
extern gstk_status_enum_type gstk_parser_ch_data_len_tlv(
    uint8                                *orig_cmd_data,
    int32                                 cmd_data_len,
    int32                                 orig_offset,
    int32                                *ch_data_len,
    int32                                *next_tlv_offset,
    boolean                              *more_tlv,
    boolean                               man_opt_specifier,
    const uint32                         *ref_id);

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
extern gstk_status_enum_type gstk_parser_ch_data_tlv(
    uint8                                *orig_cmd_data,
    int32                                 cmd_data_len,
    int32                                 orig_offset,
    gstk_ch_data_type                    *ch_data,
    int32                                *next_tlv_offset,
    boolean                              *more_tlv,
    boolean                               man_opt_specifier,
    const uint32                         *ref_id);

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
extern gstk_status_enum_type gstk_parser_text_string_tlv_ext(
    tags_enum_type                 tag,
    uint8                         *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_alphabet_type            *text_string,
    int32                         *next_tlv_offset,
    boolean                       *more_tlv,
    boolean                        man_opt_specifier,
    const uint32                  *ref_id);

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
extern gstk_status_enum_type gstk_packer_ch_status_tlv(
    const gstk_ch_status_type        *ch_status,
    gstk_ch_status_tlv_type          *ch_status_tlv);

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
extern gstk_status_enum_type gstk_packer_bearer_description_tlv(
    const gstk_bearer_description_type  *bearer_description,
    gstk_bearer_description_tlv_type    *bearer_description_tlv);

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
extern gstk_status_enum_type gstk_packer_buffer_size_tlv(
    int32                         buffer_size,
    gstk_buffer_size_tlv_type    *buffer_size_tlv);

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
extern gstk_status_enum_type gstk_packer_ch_data_len_tlv(
    int32                         ch_data_len,
    gstk_ch_data_len_tlv_type    *ch_data_len_tlv);

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
extern gstk_status_enum_type gstk_packer_ch_data_tlv(
    gstk_ch_data_type             ch_data,
    gstk_ch_data_tlv_type        *ch_data_tlv);

/*===========================================================================
FUNCTION gstk_util_is_valid_channel_id_range

DESCRIPTION
  This function determines if the channel id provided by the SIM is within
  valid range

PARAMETERS
  device_id

DEPENDENCIES
  None

RETURN VALUE
  boolean

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_valid_channel_id_range(gstk_device_identity_type device_id);

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
extern gstk_status_enum_type gstk_parser_network_access_name_tlv(
    uint8                         *orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_alphabet_type            *nw_access_name,
    int32                         *next_tlv_offset,
    boolean                       *more_tlv,
    boolean                        man_opt_specifier,
    const uint32                  *ref_id);

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
extern gstk_status_enum_type gstk_parser_ussd_string_tlv(
    uint8                  *orig_cmd_data,
    int32                   cmd_data_len,
    int32                   orig_offset,
    gstk_ussd_string_type  *ussd_string,
    int32                  *next_tlv_offset,
    boolean                *more_tlvs,
    boolean                 man_opt_specifier,
   const uint32           *ref_id);

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
extern gstk_status_enum_type gstk_parser_pdp_context_act_param_tlv(
    uint8                                *orig_cmd_data_ptr,
    int32                                 cmd_data_len,
    int32                                 orig_offset,
    gstk_pdp_context_act_param_type      *pdp_context_act_param_ptr,
    int32                                *next_tlv_offset_ptr,
    boolean                              *more_tlv_ptr,
    boolean                               man_opt_specifier,
    const uint32                         *ref_id_ptr);

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
  const uint32                         *ref_id_ptr);

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
  const uint32                         *ref_id_ptr);

/*===========================================================================
FUNCTION gstk_util_find_apn_len

DESCRIPTION


PARAMETERS


DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type gstk_util_find_apn_len(
    const uint8 * apn_bytes,
    int           apn_length,
    int         * cnt);

/*===========================================================================
FUNCTION gstk_util_compareString

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
gstk_status_enum_type gstk_util_compareString(
    int             pos,
    const char    * str1,
    const char    * str2 ,
    boolean       * match);

/*===========================================================================
FUNCTION gstk_util_extract_label_len_value

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
gstk_status_enum_type gstk_util_extract_label_len_value(
    int32               apn,
    const uint8        *alphabet_in,
    uint16             *alphabet_out,
    int                *ind ,
    int32              *count,
    int                *iter);

/*===========================================================================
FUNCTION gstk_util_check_for_apn_net_op_id

DESCRIPTION


PARAMETERS


DEPENDENCIES

RETURN VALUE

===========================================================================*/
gstk_status_enum_type gstk_util_check_for_apn_net_op_id(
    const char * apn_bytes,
    int          apn_len,
    int        * apn_val);

/*=========================================================================
FUNCTION
  ASCIIText_ConvertTo_GSMDefault

DESCRIPTION
  Convert an ASCII string to a GSM 7-bit Default alphabet string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void ASCIIText_ConvertTo_GSMDefault (
    const uint8   *def,
    uint32        len,
    uint8         *gsm_default);

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
extern gstk_status_enum_type gstk_packer_access_technology_tlv(
    const uint8                       *data,
    gstk_access_technology_tlv_type   *access_tech_tlv);

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
extern gstk_status_enum_type gstk_packer_battery_status_tlv(
    const uint8                    *data_ptr,
    gstk_battery_status_tlv_type   *battery_status_tlv);

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
extern gstk_status_enum_type gstk_packer_nw_search_mode_tlv(
  gstk_nw_search_mode_enum_type  nw_search_mode,
  gstk_nw_search_mode_tlv_type   *nw_search_mode_tlv
);

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
extern gstk_status_enum_type gstk_packer_csg_cell_tlv(
  sys_detailed_plmn_list_s_type     csg_info,
  gstk_csg_info_tlv_type        *csg_cell_tlv
);

#if (defined(FEATURE_WCDMA) || defined(FEATURE_LTE))
/*===========================================================================

FUNCTION GSTK_CONVERT_RRC_PLMN_ID_TO_NAS_PLMN_ID

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to the NAS PLMN_id_T format (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to PLMN_id_T format (3 byte array - 2 BCD digits/byte)

SIDE EFFECTS
  None

===========================================================================*/
extern gstk_status_enum_type gstkutil_convert_rrc_plmn_id_to_nas_plmn_id(
    gstk_rrc_plmn_identity_type plmn_id, byte *nas_plmn_id);
#endif /* FEATURE_WCDMA || FEATURE_LTE*/

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
/*===========================================================================
FUNCTION gstkutil_update_plmn_lac_cellId_table

DESCRIPTION
  This function is used to update the RRC -location informationn table and the
  MM-GSTK location information table.

DEPENDENCIES
  None

RETURN VALUE
  GSTK_ERROR   - In case of error
  GSTK_SUCCESS - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
gstk_status_enum_type gstkutil_update_plmn_lac_cellId_table(
  int                            mm_or_rrc_update,
  const uint8*                   plmn_id_ptr,
  const uint8*                   lac_ptr,
  gstk_cell_id_type              cell_info,
  gstk_location_status_enum_type loc_status,
  gstk_access_technology_type    rat,
  gstk_slot_id_enum_type         slot);
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)*/

/*===========================================================================

FUNCTION gstk_util_is_valid_rat

DESCRIPTION
  This function determines if the rat is valid or not, and map it to GSTK
  access tech type

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean gstk_util_is_valid_rat(
    sys_radio_access_tech_e_type sys_rat,
    gstk_access_technology_type *gstk_rat_ptr);

/*===========================================================================
FUNCTION gstk_map_card_local_info_to_gstk_local_info

DESCRIPTION
  This function maps the local info type request from the card to the
  gstk local info type

PARAMETERS
   local_info_type_req_from_card - info type request from the card
   *info_type - gstk info type request

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_map_card_local_info_to_gstk_local_info(
    gstk_local_info_enum_type      local_info_type_req_from_card,
    gstk_local_info_gen_enum_type *info_type,
    const gstk_generic_data_type  *data_ptr);

/*===========================================================================
FUNCTION gstk_util_populate_tlv

DESCRIPTION
  This function populates the simple TLV.
  It takes the original offset into the passed in data buffer to which
  the simple TLV will be populated.
  It also returns the new offset as a result of populating the simple TLV

PARAMETERS
  tag:               [Input] Simple TLV Tag
  orig_offset:       [Input] Original Offset into the orig_cmd_data_ptr
  orig_cmd_data_ptr: [Input] Original Command Data from which the info
                     is copied from
  new_data_ptr:      [Input/Output] pointer to the new buffer where the
                     data will be copied into
  new_offset_ptr:    [Input/Output] pointer to which the new offset will
                     be returned
  cmd_group:         [Input] Command group type of caller of this function

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
extern gstk_status_enum_type gstk_util_populate_tlv (
    tags_enum_type              tag,
    int32                       orig_offset,
    const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
    uint8                      *new_data_ptr,
    int32                      *new_offset_ptr,
    gstk_cmd_group_enum_type    cmd_group);

/*===========================================================================
FUNCTION gstk_util_populate_uim_cmd_total_data_len

DESCRIPTION
  This function populates the data_len for the data in the uim_cmd_type
  according to the type of command to be sent to the card.
  It accounts for the case where > 0x7F total len by adding the extra
  byte for the 0x81 byte.
  If it is an envelope command, the Envelope Tag will be filled in as well

PARAMETERS
  ber_tlv_tag:    [Input] BER TLV Tag
  total_data_len: [Input] Total number of bytes for all the simple TLVs
  uim_cmd_ptr:    [Input/Output] pointer to the uim_cmd_type

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
extern gstk_status_enum_type gstk_util_populate_uim_cmd_total_data_len (
    int32         ber_tlv_tag,
    int32         total_data_len,
    uim_cmd_type *uim_cmd_ptr);

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
extern gstk_status_enum_type gstk_packer_dev_id_card_tlv(
    const gstk_device_id_type          *dev_id_orig_ptr,
    uint8                              *dev_id_new_value_ptr);

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
extern gstk_status_enum_type gstk_packer_location_info_card_tlv(
    const gstk_location_info_gen_type  *location_info_orig_ptr,
    uint8                              *location_info_new_value_ptr,
    int32                               dest_size);

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
extern gstk_status_enum_type gstk_packer_pdp_context_act_param_card_tlv(
    const gstk_pdp_context_act_param_type  *pdp_context_act_param_orig_ptr,
    uint8                                  *pdp_context_act_param_new_value_ptr);

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
);

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
);

/*=========================================================================
FUNCTION
  gstk_util_find_match_in_cmd_details_ref_table

DESCRIPTION
  This function is used to find a match for a request in the command details
  reference table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE   - if a match is found.
  FALSE  - if no match is found.

SIDE EFFECTS
  None
=========================================================================*/
extern boolean gstk_util_find_match_in_cmd_details_ref_table(
    uint32                           * ref_id_ptr,
    gstk_cmd_enum_type               rsp_type);

/*===========================================================================
FUNCTION gstk_util_populate_term_rsp_common_tlv

DESCRIPTION
  This function populates the command details, device identities and the
  result values into the uim buffer when building the terminal response.

PARAMETERS
  orig_offset       - [Input]original offset
  *total_len_ptr    - [Input/Output] Total length ptr
  gstk_tlv          - [Input] contains command details, device id details and result values.
  *uim_cmd_ptr      - [Input/Output] uim Cmd ptr

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
extern gstk_status_enum_type gstk_util_populate_term_rsp_common_tlv(
    int32                         orig_offset,
    uint32                        *total_len_ptr,
    gstk_cmd_term_rsp_common_type gstk_tlv,
    byte                          *uim_cmd_ptr);

/*===========================================================================
FUNCTION gstk_packer_cmd_details_tlv

DESCRIPTION

  This function populates the passed in command_details pointer with the
  info in the command details ref table after verifying the ref id for the
  command details ref table is valid


PARAMETERS
  ref_id: [Input] Index location in the command_Details_ref_table
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
extern gstk_status_enum_type gstk_packer_cmd_details_card_tlv(
    gstk_cmd_info_type             cmd_info,
    uint8                         *cmd_details_new_value_ptr);

/*===========================================================================
FUNCTION gstk_packer_result_card_tlv

DESCRIPTION
  This function populates the passed in result value

PARAMETERS

  result_info           [Input] data to be copied
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
extern gstk_status_enum_type gstk_packer_result_card_tlv(
    gstk_result_type              result_info,
    uint8                         *result_new_value_ptr);

/*===========================================================================
FUNCTION gstk_packer_evt_list_card_tlv

DESCRIPTION
  This function populates the data uint8 array with event ID value info

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
extern gstk_status_enum_type gstk_packer_evt_list_card_tlv(
    gstk_evt_list_code_enum_type      evt_list_info_data,
    uint8                            *evt_list_new_value_ptr);

/*===========================================================================
FUNCTION gstk_packer_nw_search_mode_card_tlv

DESCRIPTION

  This function populates the data uint8 array with network search mode  info

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
extern gstk_status_enum_type gstk_packer_nw_search_mode_card_tlv(
    gstk_nw_search_mode_enum_type    nw_search_mode,
    uint8                            *nw_search_mode_new_value_ptr);

/*===========================================================================
FUNCTION gstk_cm_ph_reg_dereg

DESCRIPTION
  This function is used to register or deregister with CM for specific
  phone events.

PARAMETERS
  evt_flg:               [Input] flg indicates where Reg or Dereg is required.
  start_ph_evt:          [Input] phone event lower range
  end_ph_evt:            [Input] phone event upper range

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
extern gstk_status_enum_type gstk_cm_ph_reg_dereg(
   int32              evt_flg,
   cm_ph_event_e_type start_ph_evt,
    cm_ph_event_e_type end_ph_evt);

/*===========================================================================
FUNCTION gstk_util_cmd_details_ref_table_free_all_slots

DESCRIPTION
  This function is only used during internal sim reset during uim recovery. It
  frees up used slots in the command details ref table after notifying clients
  that the proactive sim session has ended.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_cmd_details_ref_table_free_all_slots(void);

/*===========================================================================
FUNCTION gstk_util_find_and_build_proactive_end_session

DESCRIPTION
  This function finds the end proactive in the queue if not present then builds the proactive end session and sends it
  to the client. This is different than the proactive end session that
  comes from the card.

PARAMETERS
  gstk_tag_enum_type - Tag that sent to the client

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_find_and_build_proactive_end_session(gstk_tag_enum_type gstk_tag);

/*===========================================================================
FUNCTION gstk_util_find_and_process_pending_responses

DESCRIPTION
  This function finds the envelope responses in the gstk_cmd_q and process them first during recovery and card error
  scenerios.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_find_and_process_pending_responses(void);

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
boolean gstk_is_tag_comprehension_req(
    uint8 tag);

/*===========================================================================
FUNCTION gstk_parser_get_next_tlv_offset

DESCRIPTION
  This functions gets the offset of the next tlv if any. If there are more TLVs,
  more_tlv is set to true else it is set to false.

PARAMETERS
 cmd_data_len     - command data length
 count            - represents the last byte processed in the command
                          data buffer.
 next_tlv_offset  - pointer to the next tlv offset
 more_tlv         - indicates whether more TLVs are present or absent.

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
extern gstk_status_enum_type gstk_parser_get_next_tlv_offset(
  int32      cmd_data_len,
  int32      count,
  int32*     next_tlv_offset_ptr,
  boolean*   more_tlv_ptr);

/*===========================================================================
FUNCTION gstk_util_retry_envelope_cmd

DESCRIPTION
  This function will retry sending the last unsuccessful delivered envelope
  to the card

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  Currently this function will be called to retry only event download envelopes.
  The calling function should check the gstk envelope state to see if it is
  set to GSTK_SUB_ST_PENDING_EVT_DL_IN_QUEUE before calling this
  command to retry the event download envelope.

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_retry_envelope_cmd(void);

/*===========================================================================
FUNCTION gstk_util_clear_retry_envelope_data

DESCRIPTION
  This function will clear the envelope retry information stored in a gstk
  global variable

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_clear_retry_envelope_data(gstk_slot_id_enum_type slot);

/*===========================================================================
FUNCTION gstk_util_is_envelope_rsp_evt_dl

DESCRIPTION
  This function will check the command id of a particular response is
  an for and event download envelope

PARAMETERS
  command_id: command identifier

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE:   if the command is an event download envelope
    FALSE:  if the command is not an event download envelope

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_envelope_rsp_evt_dl(uint32 command_id);

/*===========================================================================
FUNCTION gstk_util_is_envelope_rsp_loc_info

DESCRIPTION
  This function will check the command id of a particular response is
  an for and location information envelope

PARAMETERS
  command_id: command identifier

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE:   if the command is a location information envelope
    FALSE:  if the command is not a location information envelope

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern boolean gstk_util_is_envelope_rsp_loc_info(uint32 command_id);

/*===========================================================================
FUNCTION gstk_util_call_client_evt_cb

DESCRIPTION
  Based on the type of the client, this function packs the right format of the
  arguments and invokes the event callback function registerd by the client.

PARAMETERS
  client_id:  [Input] The ID of the client whose event call back function will
                      be invoked.s
  message_ptr:[Input] The pointer to the message to be passed to the client

DEPENDENCIES
  None

RETURN VALUE
  Void
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_call_client_evt_cb(
  gstk_client_id_type      client_id,
  gstk_cmd_from_card_type *message_ptr
);

/*===========================================================================
FUNCTION gstk_util_dump_byte_array

DESCRIPTION
  This function dumps byte array in the format of three item per line.

PARAMETERS
  array_name_ptr:  [Input] The name of the array to be dumped
  byte_array_ptr:  [Input] The pointer to the byte array to be dumped
  array_size:  [Input] The size of the byte array

DEPENDENCIES
  None

RETURN VALUE
  Void
COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_dump_byte_array(
  const char  *array_name_ptr,
  const byte  *byte_array_ptr,
  uint32      array_size
);

/*===========================================================================
FUNCTION gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type

DESCRIPTION
  This function converts an STK command to a GSTK command type

PARAMETERS
  stk_cmd_type: [Input]        STK command type. This confirms to the 3GPP spec
  cmd_type_ptr: [Input/Output] GSTK command type

DEPENDENCIES
  None

RETURN VALUE
  GSTK_BAD_PARAM - if input parameters are incorrect
  GSTK_SUCCESS   - if cmd is successfully queued to the clients.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
  uint8               stk_cmd_type,
  uint8               stk_cmd_qual,
  gstk_cmd_enum_type *cmd_type_ptr
);

/*===========================================================================
FUNCTION gstk_util_free_client_id

DESCRIPTION
  This function frees up client id and all event registered by this client

PARAMETERS
  client_id: the client id to be released

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_free_client_id(gstk_client_id_type client_id);

/*===========================================================================
FUNCTION gstk_util_notify_tp_dl_fail

DESCRIPTION
  This function notifies clients that terminal profile download failed

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_notify_tp_dl_fail(void);

/*===========================================================================
FUNCTION gstk_util_notify_tp_dl_fail_specific_client

DESCRIPTION
  This function notifies specific client that terminal profile download failed

PARAMETERS
  client_id : gstk client id type

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_notify_tp_dl_fail_specific_client(gstk_client_id_type client_id);

/*===========================================================================
FUNCTION gstk_util_send_err_to_client

DESCRIPTION
  This function sends error report to client for sending the command in wrong
  GSTK state

PARAMETERS
  cmd: pointer to gstk command type

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_send_err_to_client(
  const gstk_cmd_type* cmd_ptr );

/*===========================================================================
FUNCTION gstk_parser_plmn_list_tlv

DESCRIPTION
  This function parses the plmn list information provided as optional
  data in a REFRESH command.
  If there is no more TLVs to be parsed, the more tlv will be set to
  FALSE

PARAMETERS
  orig_cmd_data  : [Input] Pointer to the original command data to be parse
  cmd_data_len   : [Input] Valid length of the original command data pointer
  orig_offset    : [Input] Offset in the original command data where the parsing
                           should begin
  plmn_list      : [Input/Output] Pointer to file list buffer,
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
extern gstk_status_enum_type gstk_parser_plmn_list_tlv(
    uint8                        * orig_cmd_data,
    int32                          cmd_data_len,
    int32                          orig_offset,
    gstk_generic_data_type       * file_list,
    int32                        * next_tlv_offset,
    boolean                      * more_tlvs,
    boolean                        man_opt_specifier,
    const uint32                 * ref_id);

/*===========================================================================
FUNCTION gstk_parser_skip_tlv

DESCRIPTION
  This function skips over a TLV object

PARAMETERS
  orig_cmd_data_ptr : [Input] Command Buffer
  cmd_data_len      : [Input] Valid length of the original command data pointer
  orig_offset       : [Input] Offset in the original command data where the parsing
                              should begin
  next_tlv_offset   : [Input/Output] Pointer to the index offset for the next tlv
                                     if applicable.
  more_tlvs         : [Input/Output] Pointer to boolean to indicate whether there
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
extern gstk_status_enum_type gstk_parser_skip_tlv(
    const uint8                  * orig_cmd_data_ptr,
    int32                          cmd_data_len,
    int32                          orig_offset,
    int32                        * next_tlv_offset,
    boolean                      * more_tlvs);

/*===========================================================================
FUNCTION gstk_util_get_sim_slot_id

DESCRIPTION
  This function returns SIM slot id assiciated with a client id

PARAMETERS
  client_id: the client id of which SIM slot id will be returned

DEPENDENCIES
  None

RETURN VALUE
  SIM slot id assiciated with specified client id

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_slot_id_enum_type gstk_util_get_sim_slot_id(
  gstk_client_id_type client_id
);

/*===========================================================================
FUNCTION gstk_util_is_client_id_internal

DESCRIPTION
  This function checks if SIM slot id is valid

PARAMETERS
  client_id: the client id of which SIM slot id will be returned

DEPENDENCIES
  None

RETURN VALUE
  TRUE if valid; FALSE if not valid

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_client_id_internal(
  gstk_client_id_type client_id
);

/*===========================================================================
FUNCTION gstk_util_check_bytes

DESCRIPTION
  This function checks if the values of in a byte array are all equal to a
  certain byte value

PARAMETERS
  array_ptr: [in] the pointer to the byte array
  array_size:[in] the size of the byte array

DEPENDENCIES
  None

RETURN VALUE
  return TRUE if all values in the byte array are equal to given byte value;
  otherwise, return FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_check_bytes(
  const byte* array_ptr,
  uint32      array_size,
  uint8       byte_value
);

/*===========================================================================
FUNCTION gstk_util_get_rat_from_ds

DESCRIPTION
  This function queries data services for RAT details

PARAMETERS
  rat_ptr: [output] the structure of RAT

DEPENDENCIES
  None

RETURN VALUE
  TRUE if succeeds. Otherwise, return FALSE

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_get_rat_from_ds(
  dss_iface_ioctl_bearer_tech_type *rat_ptr
);

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
                              int32         data_len);

/*===========================================================================
FUNCTION gstk_util_send_tr_if_no_service

DESCRIPTION
  This function sends error terminal response if the current service status
  is NO SERVICE

PARAMETERS
  None

DEPENDENCIES
  cmd_detail_ref_id:     the ref ID of command details
  command_number:        the command number
  terminl_response_enum: the type of terminal response

RETURN VALUE
  TRUE if error terminal response is sent

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_send_tr_if_no_service(
  uint32             cmd_detail_ref_id,
  uint8              command_number,
  gstk_cmd_enum_type terminal_response_enum
);

/*===========================================================================
FUNCTION gstk_util_send_tr_if_not_3gpp

DESCRIPTION
  This function sends error terminal response if the current access technology
  is NOT 3GPP

PARAMETERS
  None

DEPENDENCIES
  cmd_detail_ref_id:     the ref ID of command details
  command_number:        the command number
  terminl_response_enum: the type of terminal response

RETURN VALUE
  TRUE if error terminal response is sent

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_send_tr_if_not_3gpp(
  uint32             cmd_detail_ref_id,
  uint8              command_number,
  gstk_cmd_enum_type terminal_response_enum
);

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
boolean gstk_is_tag_known(uint8 tag);

/*===========================================================================
FUNCTION gstk_util_cmp_with_cached_term_rsp

DESCRIPTION

  This function compares the cached terminal response with terminal response
  being sent for proactive command in progress.

PARAMETERS
  uim_cmd_ptr: [Input] Pointer to the terminal response to be compared

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
gstk_status_enum_type gstk_util_cmp_with_cached_term_rsp(const uim_cmd_type *uim_cmd_ptr);

/*===========================================================================
FUNCTION gstk_util_send_attach_ctrl_res

DESCRIPTION
  This function send the result of attach query to the querying task

PARAMETERS
  status: [input] the status to be sent

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_send_attach_ctrl_res(void);

/*===========================================================================
FUNCTION gstk_util_find_rat_priority

DESCRIPTION
  The functions finds the priority of a given radio access technology
  based on rat priority table

PARAMETERS
  rat: [Input] The Radio access tech for which need to find priority

DEPENDENCIES
  None

RETURN VALUE
  uint8 - The priority of RAT in GSTK global table

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint8 gstk_util_find_rat_priority(gstk_rat_enum_type rat);

/*===========================================================================
FUNCTION gstk_util_process_mm_or_mode_change

DESCRIPTION
  The function compares the currently available mm or mode change events
  and decides which one should be processed. The API is called while
  processing a MM IDLE or MODE CHANGE command.

PARAMETERS
  cmd_orig: [Input]Inform if MM IDLE or MODE CHANGE originated the call

DEPENDENCIES
  None

RETURN VALUE
  Boolean:
  TRUE - if the current event invoking API has to continue with the event
  FALSE - if the current event involing API has to ignore event

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_process_mm_or_mode_change(
  gstk_cmd_check_orig_enum_type         cmd_origin);

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
);

/*===========================================================================
FUNCTION gstk_util_compare_and_update_file

DESCRIPTION
  This function opens the file, compares its contents against the passed
  buffer and if the two are different, empties the file and writes back
  the buffer data to the file. It also writes the buffer data to the file
  if file couldn't be read.

PARAMETERS
  file_path_ptr: File path
  buf_ptr:       Buffer to compare against the file data
  buf_len:       Buffer length

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_compare_and_update_file(
  const char*                file_path_ptr,
  const char*                buf_ptr,
  uint16                     buf_len
);
/*===========================================================================
FUNCTION gstk_packer_csg_cell_selection_status_tlv

DESCRIPTION
  This function populates CSG cell selection information in to required
  TLV structure

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
);

/*===========================================================================
FUNCTION gstk_packer_csg_id_tlv

DESCRIPTION
  This function populates CSG ID information in to required TLV structure

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION gstk_util_validate_slot_sessions

DESCRIPTION
  This function simply checks GSTK subscription and session tables to make sure
  the primary 1X/GW sessions belong to one single slot. Same with secondary
  sessions.

PARAMETERS

DEPENDENCIES
  modem_as_info
  mmgsdi_sess_info

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_validate_slot_sessions(void);

/*===========================================================================
FUNCTION gstk_util_find_access_tech_priority

DESCRIPTION
  The functions finds the priority of a given radio access technology
  based on rat priority table

PARAMETERS
  access_tech: [Input] The access tech for which need to find priority

DEPENDENCIES
  None

RETURN VALUE
  uint8 - The priority of RAT in GSTK global table

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uint8 gstk_util_find_access_tech_priority(gstk_access_technology_type access_tech);

/*===========================================================================
FUNCTION gstk_util_remove_pending_env

DESCRIPTION
  This function will remove any pending Location Status and Verizon
  Proprietary Envelope cmd from command queue

PARAMETERS
  cmd_id [Input]: The command ID of cmd to be removed from cmd queue

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_remove_pending_env(
  gstk_cmd_enum_type cmd_id
);

/*===========================================================================

FUNCTION gstk_util_convert_impu_list

DESCRIPTION
  This function converts IMPU list seperated by '\0 as delimiter to the format
  defined in section 8.111 of 131 111 specification.

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if conversion succeeds.

SIDE EFFECTS
  the original buffer is freed if the function returns TRUE
===========================================================================*/
boolean gstk_util_convert_impu_list(uint8 **impu_list_ptr, uint8 *len_ptr);

/*===========================================================================
FUNCTION gstk_util_requeue_prio_mm

DESCRIPTION
  This function compares the new MM IDLE evt with old MM IDLE evt on other
  stack(s) to see if previous MM IDLE evt can be reused (for better service)

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if an old MM IDLE evt is requeued

===========================================================================*/
boolean gstk_util_requeue_prio_mm(gstk_mm_idle_state_type *new_mm_ptr);

/*===========================================================================
FUNCTION gstk_util_process_mm_indication

DESCRIPTION
  This function processes the MM Indication

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if there is an MM IDLE to be processed.
  Return FALSE if there is no MM IDLE to be processed.

===========================================================================*/
boolean gstk_util_process_mm_indication(gstk_mm_idle_state_type *mm_ind_ptr);

/*===========================================================================
FUNCTION gstk_util_log_gwl_info

DESCRIPTION
  This function log detailed info of gstk_gwl_loc_info

PARAMETERS
  gwl_info_ptr [Input]: The gstk_gwl_loc_info to be logged

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_log_gwl_info(const gstk_gwl_loc_info *gwl_info_ptr);


/*===========================================================================
FUNCTION gstk_util_log_cdma_info

DESCRIPTION
  This function log detailed info of gstk_cdma_loc_info

PARAMETERS
  cdma_info_ptr [Input]: The gstk_cdma_loc_info to be logged

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_log_cdma_info(const gstk_cdma_loc_info *cdma_info_ptr);


/*===========================================================================
FUNCTION gstk_util_get_unique_num

DESCRIPTION
  This function generate 32bit unique number.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Return 32bit random number

===========================================================================*/
uint32 gstk_util_get_unique_num(void);


/*===========================================================================
FUNCTION gstk_util_generate_ref_id

DESCRIPTION
  This function generate the reference id by appending unique number to 32bit input parameter.

PARAMETERS
  ref_num [Input]: The reference number to be prefixed with random number

DEPENDENCIES
  None

RETURN VALUE
  Return 32 bit Unique number

===========================================================================*/
uint32 gstk_util_generate_ref_id(uint32 ref_num);


/*===========================================================================
FUNCTION gstk_util_get_idx

DESCRIPTION
  This function estract index and unique number from ref_id

PARAMETERS
  ref_id [Input]: Unique reference number
  index [Output]: Index in command reference table
  u_num [Output]: Unique Number

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_get_idx(uint32 ref_id, uint32* index, uint32 *u_num);

/*===========================================================================
FUNCTION gstk_util_compare_ref_id

DESCRIPTION
  Compare the unique reference id given as input with the unique reference id in reference table.

PARAMETERS
  ref_id [Input]: Unique reference number

DEPENDENCIES
  None

RETURN VALUE
  return success if unique reference id is matching.

===========================================================================*/
gstk_status_enum_type gstk_util_compare_ref_id(uint32 ref_id);

/*===========================================================================
FUNCTION GSTK_UTIL_FIND_AND_DELETE_RECOVERY_END_CMD

DESCRIPTION
  This function is to find and delete the recovery end cmd of old recovery, if recovery cmd comes recursively.

PARAMETER
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_util_find_and_delete_recovery_end_cmd(void);

/*===========================================================================
FUNCTION gstk_util_clean_up_gstk_global_data

DESCRIPTION
  This function cleans/reset the global data when there is a card error

PARAMETERS
  Slot - Slot for which data need to cleanup.
  tag  - Cleanup/error tag

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_clean_up_gstk_global_data(gstk_slot_id_enum_type slot,
                                         gstk_tag_enum_type     tag);


/*===========================================================================
FUNCTION gstk_util_build_error_envelope_response

DESCRIPTION
  This function cleans/reset the global data when there is a card error

PARAMETERS
  ref_id: Envelope reference table index

DEPENDENCIES
  gstk_status_enum_type
  GSTK_ERROR  : if command could not be built successfully
  GSTK_SUCCESS: if command was built successfully

RETURN VALUE
  None

===========================================================================*/
gstk_status_enum_type gstk_util_build_error_envelope_response(uint32 ref_id);

/*===========================================================================
FUNCTION gstk_util_find_priority_mode_change_rat

DESCRIPTION
  This function finds the priority 3GPP2 access tech based on location status
  and RAT

PARAMETER
  access_tech     : Access tech result due to RAT prioritization
  location_status : Loc Status result due to RAT prioritization
  mode_change_ind : CM mode change command

DEPENDENCIES
  None

RETURN VALUE
  GSTK_NULL_INPUT_PARAM : When one of the inputs is NULL
  GSTK_SUCCESS          : Otherwise

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_find_priority_mode_change_rat(
   gstk_access_technology_type*     access_tech,
   gstk_location_status_enum_type*  loc_status,
   const gstk_mode_change_ind_type* mode_change_ind);

/*===========================================================================
FUNCTION gstk_util_is_vzw_mcc

DESCRIPTION
  This function checks the input MCC value to US MCC values

PARAMETERS
  loc_info_ptr [Input]: PLMN information

DEPENDENCIES
  None

RETURN VALUE
  TRUE  : When the inputted PLMN has MCC that belongs of US
  FLASE : Otherwise

===========================================================================*/
boolean gstk_util_is_vzw_mcc(const uint8* loc_info_ptr);

/*===========================================================================
FUNCTION gstk_util_find_terminal_rsp_in_queue

DESCRIPTION
  This function finds if there is a TR in GSTK queue when card error evt is
  received from MMGSDI. This is used during hotswap recovery scenarios to
  ensure status words are sent to clients due to GSTK being unable to get GSTK_TR_REPORT
  evt

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  True == Found the TR, false otherwise

COMMENTS
  None

SIDE EFFECTS
  Removes the TR from the queue if found

SEE ALSO
  None
===========================================================================*/
extern gstk_task_cmd_type* gstk_util_find_terminal_rsp_in_queue(void);

/*===========================================================================
FUNCTION gstk_util_send_sw_to_client

DESCRIPTION
  This function checks if GSTK should send status words to client during
  recovery/hotswap scenarios

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  Removes the TR from the queue if found

SEE ALSO
  None
===========================================================================*/
extern void gstk_util_send_sw_to_client(gstk_cmd_from_card_type *sw1_sw2_ptr);

/*===========================================================================
FUNCTION gstk_util_send_bip_status_diag_event

DESCRIPTION
  This function sends out a DIAG event w.r.t current BIP Status

PARAMETERS
  bip_session_status [Input]: Status of BIP Session
  bip_channel_id     [Input]: BIP Channel ID
  slot_id            [Input]: Slot ID

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_send_bip_status_diag_event(
  gstk_bip_status_data_type bip_session_status,
  uint32                    bip_channel_id,
  gstk_slot_id_enum_type    slot_id
);

/*===========================================================================
FUNCTION gstk_util_extract_plmn_csg_info

DESCRIPTION
  This function sends out a DIAG event w.r.t current BIP Status

PARAMETERS
  dest_plmn_csg_info [output]: PLMN and CSG list
  srs_plmn_csg_info   [input]: available network list

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_extract_plmn_csg_info(
  gstk_available_csg_list_type *dest_plmn_csg_info,
  sys_detailed_plmn_list_s_type *srs_plmn_csg_info
);

/*===========================================================================
FUNCTION gstk_util_get_sub_id_from_slot

DESCRIPTION

  This function returns subscription ID corresponding to the slot requested

PARAMETERS
  slot_id:   [Input]  Slot ID
  sub_id_ptr [Output] Sub ID corresponding to the slot

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
gstk_status_enum_type gstk_util_get_sub_id_from_slot(
  const gstk_slot_id_enum_type  slot_id,
  sys_modem_as_id_e_type       *sub_id_ptr
);

/*===========================================================================
FUNCTION gstk_util_compare_mm_to_nw_rej_cache

DESCRIPTION
  This function checks the arbitrated PLMN and LAC against nw reject
  cache

PARAMETERS

DEPENDENCIES
  feature bmsk must enable GSTK_CFG_FEATURE_SLOT_LTD_SVC_WITH_NW_REJ_ONLY

RETURN VALUE
  True - Arbitrated MM cache same as NW reject cache
  False - Arbitrated MM cache different from NW reject cache

===========================================================================*/
boolean gstk_util_compare_mm_to_nw_rej_cache(void);

/*===========================================================================
FUNCTION gstk_util_location_info_tlv_cleanup

DESCRIPTION
  This function cleans the dynamically-allocated location info TLV

PARAMETERS 
  loc_info_tlv   [input]: Location info TLV

DEPENDENCIES
  None 
 
RETURN VALUE 
  None 

===========================================================================*/
extern void gstk_util_location_info_tlv_cleanup(
  gstk_cdma_loci_ext_tlv_type *loc_info_tlv
);

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_util_merge_cm_stacks

DESCRIPTION
  This function merges CM stacks into a GSTK structure

PARAMETERS 
  mode_change_ind   [input]: CM Mode Change Indication
  cdma_ext         [output]: GSTK CDMA Ext Location Info

DEPENDENCIES
  None 
 
RETURN VALUE 
  None 
===========================================================================*/
extern boolean gstk_util_merge_cm_stacks(
  gstk_cdma_loci_ext_cache_type *cdma_ext_cache, 
  gstk_mode_change_ind_type *mode_change_ind
);
#endif /* FEATURE_CDMA */

/*===========================================================================
FUNCTION gstk_util_check_slot_with_fetch_rejected

DESCRIPTION
 This function will check for the slot with ignored fetch request and calls
 uimdrv API to check for 91 xx that slot.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_check_slot_with_fetch_rejected(void);
#endif /* gstkutil_h */
