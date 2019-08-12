/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        G S T K     U T I L I T Y


GENERAL DESCRIPTION
  This is the GSTK internal utility file.  It contains function that will
  be called within GSTK module such basic util functionalities.


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

  gstk_send_cmd_to_uim_free_q
    This function sends the uim cmd back to uim free queue

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

  gstk_util_populate_card_cmd_memory
    This function allocates and copies memory for the command pointer input.

  gstk_util_release_card_cmd_memory
    This function frees all the allocated memory for the command pointer input.

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

  gstk_util_pack_7_bit_chars
    This function packs ASCII chars to SMS 7 bit format

  gstk_util_check_proactive_card
    Check if the card is has proactive commands capability

  gstk_util_map_mm_to_gstk_location_status
    This function maps the mm location status to gstk location status

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

  gstk_util_dereg_client_type
    This function dereg events for certain client type.  dereg_evt indicates
    the events to be removed from the client's list

  gstk_util_has_pending_setup_call
    This function find if there is any pending setup call proactive command

  gstk_util_decode_dcs_type
    This function determines if the dcs is 7 bit or 8 bit or ucs2 depending
    on whether it is a cell broadcast dcs or not

  gstk_util_is_valid_channel_id_range
    This function determines if the channel id provided by the SIM is within
    valid range

  gstk_util_unpack_7_bit_chars_ext
    This function unpacks the input data from SMS7 bit format into ASCII
    and returns the actual data length without the null terminated byte.

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
    This function frees up client id

  gstk_util_get_sim_slot_id
    This function returns SIM slot id assiciated with a client id

  gstk_util_is_client_id_internal
    This function checks if SIM slot id is valid

  gstk_util_preprocess_idle_evt
    This function pre-processes (E)MM IDLE event

  gstk_util_get_rat_from_ds
    This function queries data services for RAT details

  gstk_util_send_tr_if_no_service
    This function sends error terminal response if NO SERVICE

  gstk_util_get_rat_from_ds
    This function sends error terminal response if the current RAT is NOT 3GPP

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

  gstk_util_is_vzw_mcc
    This function compares the inputted PLMN information with US MCC values

  gstk_util_find_terminal_rsp_in_queue
    This function returns true if there is a TR in the queue, used
    for hotswap scenarios

  gstk_util_find_terminal_rsp
    This function is used by gstk_util_find_terminal_rsp_in_queue to
    find TRs in the queue

  gstk_util_send_sw_to_client
    This function calls gstk_util_find_terminal_rsp_in_queue and sends
    the SWs to GSTK clients

  gstk_util_compare_mm_to_nw_rej_cache
    This function checks the arbitrated PLMN and LAC against nw reject
    cache

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstkutil.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
05/31/16   shr     F3 reduction: Dumping Arrays
05/26/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/17/16   vr      F3 reduction
05/10/16   gs      F3 messages reduction 
05/06/16   gs      F3 logging improvements
01/08/16   vr      Reset usr_actvty and idl_scrn for raw envelope rsp evt dl
11/12/15   vr      F3 Logging improvements
10/28/15   vr      Don't send SW1 report for the clients which didn't send TR
09/25/15   vr      Check pending proactive cmd MMGSDI FETCH allowed is set
08/27/15   vr      Moved the logic to reset cache in respective event handlers
07/02/15   vr      Fix potential buffer overflow while building Envelope/TR
04/27/15   vr      Non UI and non NW proactive command parallel processing
01/27/15   gm      Retry call control after recovery
01/09/15   gm      Properly clear global variable when received REFRESH
01/08/15   vr      Retry envelope command after fetching the proactive command
12/19/14   gm      Added support to read envelope retry data from NV
11/12/14   gs      Remove gstk_state_tester files and dependencies
10/30/14   bd      Removed feature GSTK_CFG_FEATURE_ME_PLI_DATE_TIME_APPS
10/21/14   shr     Fix incorrect clean-up of Envelope retry data
10/08/14   bd      Changes to control eHRPD loc info feature with NV
10/07/14   vr      Fix SW1 SW2 TR REPORT failure for setup event list
09/25/14   bd      Fixed compiler errors for 1x compiled out build
09/23/14   hn      Support for 3GPP2 Advanced and HDR Location Info
09/23/14   gs      Make gstk_num_of_instances as part of gstk_shared_data_type
09/09/14   vr      Reduce stack usage
09/04/14   sw      Donot reset NAS and CM related variables for REFRESH_NAA_INIT
09/03/14   gs      Remove FEATURE_ESTK_BIP_USE_DATA_FILTER
08/27/14   vr      Move gstk_shared_data and gstk_instance_data to global
08/26/14   gs      Handle the Proactive command registration
08/14/14   sw      Fix bug in retry mechanism
08/14/14   gm      Free memory allocated for sending command to UIM
08/13/14   dy      Add feature to send limited svc to card only upon NW reject
08/06/14   dy      Fixing KW errors
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/29/14   vr      Offtarget support for mmgsdi fetch_allowed
07/29/14   shr     Fixed compiler error
07/23/14   vr      Cleanup GSTK_CLIENT_INIT_EXT function
07/23/14   vr      Add Rel99, Rel4 and Rel5 cat versions
07/21/14   vr      Check for client_id 0 in gstk_util_get_sim_slot_id and
                   corrected GSTK_LOC_INFO_ENVELOPE_IND handling in recovery
07/11/14   shr     Add Support for NV Refresh
07/02/14   sw      Donot set fetch_allowed flag in Test framework
06/30/14   gm      Remove unnecessary clear of timer
06/30/14   gm      Properly Reseting CSG info cache
06/30/14   dy      Fix high compiler warnings
06/20/14   gm      Dynamically disable TP related to LTE
06/18/14   sw      Call/SMS Control retry
06/22/14   gs      Return GSTK_NO_CLIENT_REG_FOUND when none of the client
                   registered for MODEM funcitonlity
06/17/14   dy      Remove GSTK RAT use in ESTK setup call
05/28/14   gm      Support of CSG Rel-11 feature
05/23/14   gm      Verifying TR response report
05/20/14   dy      Fixed handling of apps_enabled in mmgsdi_session_changed
05/06/14   gs      GSTK Featurization enhancements and clean up
04/21/14   vr      Handling efs_read failure
04/07/14   shr     Remove usage of MSG_* macro
04/03/14   dy      Replace gstk_malloc() with macro
02/27/14   sw      Extended retry mechanism
03/24/14   dy      Fixed gstk_byte_offset_copy and gstk_memcpy
03/28/14   sw      Add support for BIP Status DIAG event
03/27/14   gm      Reset ref_table_idx after recovery and hotswap
03/17/14   gm      Added support for IMS communication control
02/26/14   sw      Memory leak fixes
03/04/14   shr     Do not wait for 1x to become available on non-SVLTE targets
                   w.r.t. card activation via OTASP
03/04/14   shr     Fix Subscription Ready caching logic
02/27/14   dy      Add default_apn, bip_apn_mapping_tbl, rat_priority NV item
02/21/14   dy      Fixed GSTK EFS stub fct bug and added default_apn nv
02/20/14   dy      Verify TR response for raw TRs
01/30/14   vr      SIM initiated call control handling
02/20/14   gm      Clear globals after REFRESH
02/19/14   gs      Fixed infinite IMSI refresh issue
02/11/14   gm      Fix KW critical warning
01/31/14   vr      Allow the fetch command based on the fetch status from MMGSDI
01/16/14   hn      Replace MSG_* with UIM_MSG_* macros to save memory
01/09/14   kb      Cleanup global variables while handling STOP signal
01/03/14   bd      Fixed a log message
12/16/13   bd      Added support for new CM SS event handling
12/11/13   df      Remove unused memory paging feature
12/06/13   bd      Changes in GSTK to support MM IDLE events for SGLTE+G
12/05/13   hn      Support DNS query and Channel reuse
11/29/13   vr      Enable feature GSTK_CFG_FEATURE_ENH_RAT_AND_LOC_EVT in
                   recovery
11/12/13   sw      Removed FEATURE_GSTK_TEST_AUTOMATION code
11/13/13   shr     Added support for BIP over WLAN
11/01/13   vr      Compilation errors when removing WCDMA feature
10/21/13   jd      Added slot parameter to gstkutil_update_plmn_lac_cellId_table()
10/18/13   jd      Add missing code when merged CR524417/MPSS2.2 at gstk.c,gstkutil.c
10/04/13   gs      F3 Message reduction
09/06/13   gm      UIMDRV Memory De-Couple changes
09/13/13   gm      Support for Recovery and hot-swap
09/04/13   jd      Fix incorrect assumption w.r.t. Stack ID coming in with
                   SGLTE Merge and SGLTE Split MM indications
08/12/13   sw      Corrected logging of number of envelope bytes
08/28/13   yt      Deprecate VIRTUIM/VIRTSAT module
08/23/13   bd      Convert NAS RAT to GSTK RAT to get GSTK RAT priority
08/22/13   vr      Klocwork fixes
08/21/13   vr      Don't consider MODE CHANGE IND RAT in NO SERVICE for
                   RAT priority calculation
07/26/13   gs      Fixed incorrect log message
07/22/13   hn      Support MM Indications
07/22/13   gs      When a DL event is not set GSTK sends GSTK_UNSUPPORTED_COMMAND
07/05/13   xz      Don't write default RAT priority to EFS
07/01/13   hh      Added support for passing RAT through MO SMS CTRL API
07/01/13   bd      Added support for maintaining separate cache for cell
                   information from different RRCs
07/01/13   hn      Support SGLTE dual MMs
06/24/13   xz      Support IMS Registration event download
06/20/13   vr      Triton TSTS changes
06/19/13   hh      Send error response to clients when env ref table is full
06/12/13   vr      Fix for Task starvation due to GSTK infinite loop based on
                   queue race condition
05/24/13   hn      Does APN mapping based on NV item
05/17/13   hn      Support for Alpha for Refresh
05/17/13   abd     Added support for reading default lang from NV
05/01/13   vr      Fix command processing logic in GSTK
05/01/13   hh      Make gstk_util_config_io() take uint32 for 4th parameter "len"
                   Also check bytes_read < 0 first
05/01/13   gm      Proper decoding of USSD string
04/29/13   gm      Removed unused GSTK function.
04/23/13   gm      Remove unused function
04/09/13   vr      Use safer versions of memcpy() and memmove()
04/09/13   dd      Add RAT to checking if there is no service for n/w commands
03/29/13   bd      Do not remove ATTACH CTRL request from the cmd queue
03/27/13   gm      Handle error while writing to EFS
03/21/13   vr      Get the slot_id from gstk_current_instance_ptr client_id
                   before sending to client
02/28/13   bd      Support for call related evt download
02/28/13   vr      Don't free the userdata if userdata is equal to envelope
                   reference id for GSTK_CALL_CONNECTED_EVT_IND envelope
02/28/13   vr      Fix for 7-bit unpack returning extra byte
02/26/13   bd      Changes to default RAT priority configuration
02/21/13   hn      Enhancement of Loc Status and VPE retry
02/19/13   av      Combine mmgsdi,gstk,qmiuim/cat conf files into mmgsdi.conf
02/14/13   vs      Added default TAL config creation support
02/11/13   bd      Support for (e)HRPD access tech
01/29/13   hn      Added BIP status events
01/18/13   hn      Remove hardcoded index value
01/17/13   hn      Support multiple sessions per subscription
12/10/12   js      Fixed compiler warnings
11/29/12   bd      Clean up of redundant members from client table
10/15/12   bd      Support for Closed Subscriber Group feature
10/21/12   av      Update conf file only if necessary
10/12/12   vr      Save client callback for GSTK_CELL_BROADCAST_DOWNLOAD_IND
09/24/12   hk      Convert all ERRs to MSG_ERROR to save memory
09/20/12   yt      Manage concurrency between BIP and OTASP
09/18/12   hk      Support to read IMSI after getting VPE resp
08/15/12   sg      Read QMI CAT config
07/09/12   sg      Requeue commands to head of queue
06/06/12   sg      Fix compare latitude with longitude
04/17/12   bd      Fixed issue of command table getting full
03/28/12   av      Replaced feature flags with NVITEM
03/16/12   yt      Remove tmc.h inclusion
02/18/12   av      Cleaned up remaining ALWAYS ON features
02/07/12   sg      Optimize read for EFS config
02/06/12   bd      Clean up gstk_util_cmd_details_ref_table_free_all_slots()
                   for the functionality it is just expected to do
01/13/12   nb      Fix compilation warning
12/20/11   nb      GSDI Cleanup
11/29/11   dd      Add support for TDSCDMA RAT
11/11/11   nb      Remove AEESTD APIs
11/10/11   sg      Fix on-target compiler warning
10/31/11   sg      Class L and M support
10/21/11   sg      Fix issue in gstk_util_process_mm_or_mode_change
10/20/11   sg      Fix crash for envelopes and sms pp dl last byte missing
10/18/11   av      Removed features that are always OFF
10/13/11   nb      Full recovery support
10/05/11   xz      Fix the issue of memcmp() on SID
10/04/11   xz      Fix the issue of memcmp()
10/03/11   sg      Check invalid cell id and location area code in util
08/29/11   xz      Fix the issue of delaying successful terminal response
06/08/11   dd      Fix dangling pointers
06/02/11   js      Fixed compilation error
05/31/11   js      Fixed potential buffer overflow issue.
05/27/11   bd      Fixed memory leak when END proactive cmd is not read
05/14/11   nb      Thread Safe - Removed Usage of gstk_util_save_env_cmd_info()
05/12/11   xz      Fix an issue of infinite loop while process envelope cmd
05/06/11   sg      Fix crash for envelopes and sms pp dl last byte missing
05/03/11   dd      Fix a crash in SMS PP DL
05/03/11   xz      Provide LTE only support
04/25/11   bd      Fixed issue of proactive commands not being fetched
                   after REFRESH RESET
04/29/11   nk      Fixed KW error
04/27/11   yt      Included private GSDI header
04/24/11   dd      SCWS support
04/13/11   ea      Add proactive cmd enable / disable support
03/31/11   ea      Add STK redial support
04/11/11   js      Fix for envelope memcpy in TAL
04/05/11   xz      Changes to support VPE ATTACH CTRL
03/29/11   sg      Retry sending Location Information envelope
03/26/11   js      Fix for LTE Call Control.
03/23/11   bd      Fixed memory leak due to hotswap
03/11/11   nb      Reset proactive_command_in_progress flag when dismissing
                   pending proactive command for refresh
03/03/11   dd      Send PLI Date/time to clients
02/28/10   xz      Fix featurization issue in gstk_get_uim_buf()
02/22/11   js      Renaming fusion features
02/17/11   ea      Added FEATURE to enable/disable client response timer as per OEM need.
02/16/11   bd      Support for delaying the TR when card is repeating the
                   same proactive command continuously.
02/15/11   nb      Fixed Event Downloads
02/02/11   dd      Fix env_ref_id
02/02/11   sg      Fix envelope when offset is involved
01/28/11   bd      Fixed issue of wrong data structure passed to call back
01/20/11   adp     Reverting back changes made in #78 as it is causing compile
                   issues.
01/19/11   xz      Fix the issue of fail to send UIM cmd
01/14/11   xz      Fix TP DL issue due to efs_put() failure
01/03/10   xz      Fix the issue of LAC/TAC being 0xFFFF
12/22/10   yt      Removed FEATURE_UIM_ZI_MEMORY_REDUCTION
12/17/10   ps      UIM memory reduction changes
12/16/10   sg      Set Routeable flag to true while getting RAT from DS
12/08/10   sg      Use profile 107 for querying RAT mask from DS on eHRPD
12/07/10   xz      Send error TR if no service or invalid RAT
11/29/10   tkl     Removed RRC private header file
11/27/10   ssr     Fix to allow UIM driver to fetch only after it reports back
                   TR response
11/19/10   js      While populating uim cmd header using tal interface, if
                   there is error return the memory to heap than to
                   uim_free_cmd_q
11/10/10   vs      Fixed leak in send cmd to uim server on fusion msm
10/18/10   xz      Add support of QCN backup
10/11/10   sg      Port IMSI_M enhancement
09/16/10   bd      Fixed double free issue
08/31/10   dd      Changes for compilation for LTE only build
08/30/10   xz      Add function gstk_util_check_zeros()
08/26/10   xz      Add support of EPS Call Control
08/25/10   vs      Fixed crash in gstk_send_cmd_to_uim_server()
08/16/10   bd      Fixed wrong conversion of ASCII to 7-bit GSM
08/19/10   xz      Add NV support of GSTK feature bitmask
08/16/10   sg      Free UIM commands pointer when routed to TAL
08/12/10   sg      On Remote dont send TP to TAL - move to idle state
07/28/10   sg      TAL support for envelope commands
07/26/10   xz      Add support of MM and EMM idle events simultaneously
07/24/10   js      TAL command processing fixes
07/15/10   sg      TAL fixes for Remote client
06/21/10   sg      Route commands to TAL when present
06/29/10   nb      Merged changes for DS DS
06/18/10   xz      Moving FEATURE_GSTK_IMSI_M_PROVISION_ENH and
                   FEATURE_GSTK_LOC_INFO_ENVELOPE to config file
06/04/10   dd      Allow broadcast to GSTK_SLOT_AUTO clients
06/01/10   bd      Fixed lint issue
05/07/10   bd      Fixed padding issues in 7-bit unpacking
05/11/10   xz      Add support of network rejection event
04/29/10   dd      Check whether gstk_curr_inst_ptr is null
04/20/10   dd      Clear the proactive command in progress flag
04/20/10   xz      Add support of LTE-RRC LTE_RRC_SERVICE_IND
04/07/10   xz      Fix the issue of delay in FETCH after envelope rsp
03/08/10   sg      Location information envelope command
02/05/10   sg      Removed incorrect fix for index calc for class E commands
02/26/10   yt      Fixed KW errors
02/19/10   bd      Support for caching proactive commands
02/15/10   nb      Moved Dual Slot Changes
02/05/10   sg      Removed incorrect fix for index calc for class E commands
11/18/09   bd      Support for delaying sending of CC envelope to card
10/12/09   kp      Added const keyword for read only varibles
09/26/09   kp      ZI memory reduction changes
09/10/09   mib     Removed deprecated fs.h
09/07/09   bd      Fix KW issues
08/24/09   sg      Fixed index calculation for class E proactive commands
07/28/09   bd      Fix lint issues
07/20/09   mib     Fixed F3 log with string
04/20/09   yb      Implemented a mechanism to notify client when TP DL fails
04/15/09   xz      Support client id clean up of modem restart
03/11/09   yb      Fixed Warnings for using 'free' as variable name
03/02/09   xz      1) Fix typo of variable name
                   2) Check if callback is null before send msg to client
02/26/09   gg      Introduced Class B event registration
01/29/09   xz      Fix issue of not waiting for setup event list terminal responses
                   from multiple clients
01/23/09   xz      Fix the issue of failing to covert timer management
                   cmd to event mask
01/19/09   yb      Added secondary support for cell id information when
                   rr_get_cell_id returns NULL in gstk_mo_sms_ctrl_ind()
12/17/08   nb      Common Modem Interface
11/10/08   xz      Added gstk_util_free_client_id()
11/06/08   sk      Fixed registration when wrong class is provided.
10/13/08   yb      Fixed documentation
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/14/08   sk      Fixed lint errors
09/08/08   sk      Added support for FEATURE_ESTK
09/08/08   sk      Fixed warnings
09/05/08   xz      Rollback change of checking client_id in gstk_util_save_env_cmd_info()
09/02/08   sk      Removed Featurization FEATURE_GSTK_SINGLE_CLIENT_RAW_FMT
08/28/08   sk      Added fix for GSTK enhanced registration.
                   Removed gstk_util_calculate_log2l()
08/22/08   xz      Save expected envelope response of GSTK_RAW_ENVELOPE_IND
                   envelope_cmd_ref_table
                   Save client_id to envelope_cmd_ref_table in gstkutil.c
08/22/08   sk      Added support to send end session to only those clients
                   that are involved in the current proactive session.
                   Fixed compiler issue for using unsigned long long with
                   switch
08/21/08   sk      Added support for GSTK enhanced registration.
                   Added support to send end session to only those clients
                   that are involved in the current proactive session
                   Added support to differentiate the different types of
                   Provide Local Info Proactive command requests
08/14/08   sk      Fixed warning.
08/08/08   xz      1) Added support for raw terminal response and raw envelope
                   response
                   2) function to dump byte array
05/06/08   sk      Added support for event download envelope retry
03/11/08   jar     Wrapped memfree for PLI NMR UTRAN in function
                   gstk_util_release_upper_layer_cmd_memory() to avoid an
                   erroneous memfree.
02/06/08   sk      Support for PLI UTRAN NMR
11/20/07   tml     Added support to allow delay in sending TR until client
                   responded back for certain internal command that was originally
                   handled by GSTK
08/08/07   sk      Fixed warning
08/07/07   sk      Temporarily commented code that stores apdu byte transactions
                   in efs
08/01/07   sk      Fixed lint errors
07/23/07   sk      Fixed warning
07/14/07   sk      Added support to requeue sms pp envelope if there is a
                   pending send sms tr
07/14/07   sk      Fixed unpacking sms default for trailing zeroes
06/24/07   sk      Fixed potential buffer overflow
05/21/07   jk      Check to make see if UIM in recovery mode before sending
                   recovery command
04/23/07   nk      fixed possible buffer overflow
04/12/07   jk      replace mem_malloc & mem_free with gstk_malloc & gstk_free
12/01/06   sk      Corrected Feature Name
10/23/06   sk      Fixed lint errors.
10/19/06   sk      Fixed lint errors
10/08/06   sk      Added support for unknown unsupported proactive commands.
08/25/06   sk      Featurized uim recovery support
08/21/06   sk      Added featurization for GPRS call control
06/24/06   sk      Lint fixes, Envelope fixes for data available and
                   channel status.
06/17/06   jar     Added FEATURE_GSTK_FOR_NON_LTK_BUILD around RRC Includes
                   to support VSIM for OFFTARGET.  Compilation Warning Fix
06/14/06   sk      Removed warnings
06/13/06   sk      UIM recovery support
06/09/06   sk      Lint fixes
06/06/06   sk      Added support for network search mod
05/23/06   tml     GPRS Call Control support and lint
05/25/06   sk      Fixed Open Channel APN issues.
                   Added support for registration for Channel Status
                   and Data Available envelopes.
05/17/06   sk      Lint fixes
05/03/06   sk      Removed cell id check function
05/03/06   tml     Fixed compilation issue
05/03/06   tml     lint
04/04/06   sk      Added support for immediate digit response for get inkey
                   Made gstk_util_find_match_in_cmd_details_ref_table()
                   obsolete.
04/04/06   sk      Added support for PLI - battery status
04/04/06   sk      Removed obsolete function
                   gstk_util_rrc_get_serving_cell_id()
11/14/05   sk      Fixed lint errors
11/09/05   sk      Fixed Lint Errors
10/18/05   tml     Use GSDI protocol to determine which UIM protocol to
                   send command on
10/14/05   tml     Fixed compilation
10/14/05   tml     Fixed compilation
10/14/05   sun     Added utility function to log data
10/10/05   tml     Added utility function to check and map rat
10/08/05   tml     Valid Cell ID Check function
10/07/05   tml     Limited NO SERVICE LS event
09/30/05   sk      Added funtion gstkutil_convert_rrc_plmn_id_to_nas_plmn_id()
                   and gstkutil_update_plmn_lac_cellId_table()
07/26/05   tml     fixed DCS decoding function
07/22/05   tml     util function for rrc cell id suport
07/20/05   tml     Added extended cell id support
07/13/05   sk      Added support for Access Technology.
                   Added gstk_util_find_match_in_cmd_details_ref_table()
05/25/05   sk      Removed check for Mormal Service in function
                   gstk_util_check_mm_state_difference() in order to
                   support event downloads when changing between cells with
                   limited service.
05/11/05   sst     Lint fixes
03/05/05   tml     Added tag comprehension support
02/09/05   sk      Added defintion for ASCIIText_ConvertTo_GSMDefault()
                   amd also the corresponding conversion table.
02/02/05   sk      Changed gstk_util_compareString to return
                   gstk_status_enum_type.
02/02/05   sk      Added definctions for functions
                   gstk_util_find_apn_len(),
                   gstk_util_compareString(),
                   gstk_util_extract_label_len_value(),
                   gstk_util_check_for_apn_net_op_id
11/23/04  sk/tml   Added SAT/USAT data support
09/20/04   tml     Return correct number of bytes after 7 bit decoding
08/19/04   tml     Added cell broadcast support
08/18/04   tml     Added gstk_util_decode_dcs_type
08/13/04   tml     Enforce pointer to NULL after mem free
08/10/04   tml     Added gstk_util_find_first_non_timer_expire_postpone_env_command
                   and gstk_util_has_pending_setup_call functions
08/03/04   tml     Memory free for additional info pointer
06/28/04   tml     Added fake IMSI support
05/26/05   tml     Added lang notification, timer management, send dtmf,
                   timer expiration, browser term evt dl and lang selection
                   evt dl supports
04/16/04   tml     linted
03/04/04   tml     Compilation fix
03/01/04   tml     Added location status env cmd fixes and provide lang info
                   support
09/13/03   tml     Changed slot_id_type to slot_id_enum_type
09/03/03   tml     Added packing and unpacking functions
07/07/03   tml     Changed messages so that they are more meaningful
06/23/03   tml     removed compiler error for 1x/gsm
05/26/03   tml     linted
04/21/03   tml     Command qualifier fix
03/21/03   tml     Update featurization for WCDMA singlebuild
03/20/03   tml     Properly featurize local info for WCDMA and GSM as well as
                   virtual uim server
03/06/03   tml     Updated enum names, Added (FEATURE_GSTK_FOR_NON_LTK_BUILD)
02/27/03 jar/tml   Return gstk_status_enum_type in gstk_util_populate_card_
                   cmd_memory().  Removed Call Event:  MT,CALL_DISCONNECTED,
                   CALL_CONNECTED in gstk_util_build_evt_dl_ind().
02/26/03   tml     Change gstk_util_save_env_cmd_info to take
                   gstk_evt_cb_funct_type instead of pointer to the type
02/13/03   tml     Added/Changed DM messages
02/12/03   tml     Free icon type in the release
02/11/03   tml     Get uim queue from uim_cmd_q with the correct feature
                   Return valid client id when the queried id is
                   GSTK_CLIENT_ID
02/07/03  tml/jar  Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include <stringl/stringl.h>
#include "gstkutil.h"
#include "gstk_term_profile.h"
#include "gstk_terminal_rsp.h"
#include "gstk_envelope_cmd.h"
#include "gstk_envelope_rsp.h"
#include "gstk_state.h"
#include "gstk_exp.h"
#include "gstk_proactive_cmd.h"

#include "task.h"
#include "err.h"
#include "string.h"
#include "uim_msg.h"

/* For getting HW capability from MCS module */
#include "hwio_cap.h"

#include "sys.h"
#if defined (FEATURE_GSM)
#include "rr_usat_if.h"
#endif /*FEATURE_GSM */

#include "fs_lib.h"
#include "fs_public.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#include "event.h"

#define GSTK_SMS_8_BIT_GENERAL_DATA_CODING_DCS      0x04
#define GSTK_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x08
#define GSTK_SMS_GENERAL_DATA_CODING_MASK           0xCC

#define GSTK_SMS_8_BIT_DCS                          0xF4
#define GSTK_SMS_DATA_CODING_MASK                   0xFC

#define GSTK_CBS_8_BIT_DCS                          0xF4
#define GSTK_CBS_DATA_CODING_MASK                   0xFC

#define GSTK_CBS_8_BIT_GENERAL_DATA_CODING_DCS      0x44
#define GSTK_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS 0x48
#define GSTK_CBS_GENERAL_DATA_CODING_MASK           0xCC

#define GSTK_CBS_UCS2_7_BIT_DCS                     0x11

/* Queue to hold commands for gstk */
extern q_type gstk_task_cmd_q;

/* Conversion table for ASCII to SMS Default */
static const char ascii_to_gsmdefault_table[] =
{
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ',' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', '!',0x22, '#', '$', '%', '&',0x27, '(', ')', '*', '+', ',', '-', '.', '/',
 '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
 0x00, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', 0x11,
 ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}','~', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', 0x40, ' ', 0x01, 0x24, 0x03, ' ', 0x5F, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x0B, ' ', ' ', ' ', ' ', ' ', ' ', 0x60,
 ' ', ' ', ' ', ' ', 0x5B, 0x0E, 0x1C, 0x09, ' ', 0x1F, ' ', ' ', ' ', ' ', ' ', ' ',
 ' ', 0x5D, ' ', ' ', ' ', ' ', 0x5C, ' ', ' ', ' ', ' ', ' ', 0x5E, ' ', ' ', 0x1E,
 0x7F, ' ', ' ', ' ', 0x7B, 0x0F, 0x1D, ' ', 0x04, 0x05, ' ', ' ', 0x07, ' ', ' ', ' ',
 ' ', 0x7D, 0x08, ' ', ' ', ' ', 0x7C, ' ', 0x0C, 0x06, ' ', ' ', 0x7E, ' ', ' ', ' ',
};

/*===========================================================================

                         GLOBALS FOR MODULE

===========================================================================*/

/* define Max length and max no of substrings that need to be avoided in the APN */
#define GSTK_APN_MAXLEN 6
#define GSTK_APN_MAXNO  4

/* SPEC 11.14 - Substrings to be avoided in APN Network ID */
static const char gstk_apn_start_str_table[GSTK_APN_MAXNO][GSTK_APN_MAXLEN] = {"rac", "lac", "sgsn", "rnc"};
static char gstk_apn_end_str_table[GSTK_APN_MAXLEN] = ".gprs";

/* SPEC 11.14 - Count of substring to be avoided in APN Network ID */
static int gstk_start_str_cnt = 4;
static int gstk_end_str_cnt = 1;


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
uint32 gstk_util_cmd_details_ref_table_get_next_free_slot()
{
  uint32 i;

  /* check for Null ptr */
  GSTK_RETURN_ERROR_IF_NULL_PTR(command_details_ref_table_ptr, GSTK_CMD_DETAILS_REF_TABLE_FULL);
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_CMD_DETAILS_REF_TABLE_FULL);

  for (i = 0; i< GSTK_MAX_PROACTIVE_COMMAND; i++) {
    if ( command_details_ref_table_ptr[i].free_slot == TRUE) {
        command_details_ref_table_ptr[i].free_slot= FALSE;
        gstk_curr_inst_ptr->in_prog_cmd_ref_id = i;
        return i;
    }
  }
  UIM_MSG_HIGH_0("no more cmd details ref free slot");
  return GSTK_CMD_DETAILS_REF_TABLE_FULL;
} /* gstk_util_cmd_details_ref_table_get_next_free_slot */

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
void gstk_util_cmd_details_ref_table_free_slot(uint32 ref_id)
{
  gstk_slot_id_enum_type slot = GSTK_SLOT_ID_MAX;
  uint32                 index   = GSTK_CMD_REF_TABLE_INDEX(ref_id);

  if (index >= GSTK_MAX_PROACTIVE_COMMAND)
  {
    /*invalid ref id*/
    UIM_MSG_ERR_1("Invalid cmd details ref id (0x%x)!", ref_id);
    return;
  }
  /* check for Null ptr */
  GSTK_RETURN_IF_NULL_PTR(command_details_ref_table_ptr);

  slot = command_details_ref_table_ptr[index].sim_slot_id;
  if (GSTK_IS_VALID_SLOT_ID(slot))
  {
    gstk_instances_ptr[(uint8)slot - 1]->in_prog_cmd_ref_id =
      GSTK_MAX_PROACTIVE_COMMAND;
  }

  /* Set the free slot boolean to TRUE, so that the slot is avaliable next time */
  command_details_ref_table_ptr[index].free_slot = TRUE;
  /* Reset all the information in the table to 0 */
  command_details_ref_table_ptr[index].command_details.command_number = 0;
  command_details_ref_table_ptr[index].command_details.length = 0;
  command_details_ref_table_ptr[index].command_details.qualifier = 0;
  command_details_ref_table_ptr[index].command_details.tag = 0;
  command_details_ref_table_ptr[index].command_details.type_of_command = 0;
  command_details_ref_table_ptr[index].destination_dev_id = GSTK_NO_DEVICE;
  command_details_ref_table_ptr[index].expected_cmd_rsp_type = GSTK_CMD_ENUM_NOT_USE;
  command_details_ref_table_ptr[index].partial_comprehension = FALSE;
  command_details_ref_table_ptr[index].pending_responses = 0;
  command_details_ref_table_ptr[index].unique_ref_id = 0;

  if(command_details_ref_table_ptr[index].raw_cmd.data_buffer_ptr)
  {
    gstk_free(command_details_ref_table_ptr[index].raw_cmd.data_buffer_ptr);
    command_details_ref_table_ptr[index].raw_cmd.data_buffer_ptr = NULL;
  }
  command_details_ref_table_ptr[index].raw_cmd.data_len = 0;

  if (command_details_ref_table_ptr[index].cmd_ptr)
  {
    gstk_curr_inst_ptr->gstk_is_msg_cached = FALSE;
    gstk_util_release_card_cmd_memory(command_details_ref_table_ptr[index].cmd_ptr);
    gstk_free(command_details_ref_table_ptr[index].cmd_ptr);
    command_details_ref_table_ptr[index].cmd_ptr = NULL;
  }

  /* clear the timer */
  (void) rex_clr_timer( &(command_details_ref_table_ptr[index].tp_rpt_timer));
} /* gstk_util_cmd_details_ref_table_free_slot */

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
uint32 gstk_util_env_cmd_ref_table_get_next_free_slot()
{
  uint32 i;

  for (i = 0; i< GSTK_MAX_PROACTIVE_COMMAND; i++) {
    if ( gstk_shared_data.envelope_cmd_ref_table[i].is_free == TRUE) {
        gstk_shared_data.envelope_cmd_ref_table[i].is_free = FALSE;
        return i;
    }
  }
  if (i >= GSTK_MAX_PROACTIVE_COMMAND) {
    UIM_MSG_HIGH_0("no more env ref free slot" );
    return GSTK_CMD_DETAILS_REF_TABLE_FULL;
  }
  UIM_MSG_HIGH_0("no more env ref free slot");
  return GSTK_CMD_DETAILS_REF_TABLE_FULL;
} /* gstk_util_env_cmd_ref_table_get_next_free_slot */

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
void gstk_util_env_cmd_ref_table_free_slot(uint32 ref_id) {

  if (ref_id <= GSTK_MAX_PROACTIVE_COMMAND)
  {
    /* Set the free slot boolean to TRUE, so that the slot is avaliable next time */
    gstk_shared_data.envelope_cmd_ref_table[ref_id].is_free = TRUE;
    /* Reset all the information in the table to 0 */
    gstk_shared_data.envelope_cmd_ref_table[ref_id].client_id = 0;
    gstk_shared_data.envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type = GSTK_CMD_ENUM_NOT_USE; /* to indicate none */
    gstk_shared_data.envelope_cmd_ref_table[ref_id].evt_cb_type = GSTK_CLI_CB_NONE;
    gstk_shared_data.envelope_cmd_ref_table[ref_id].env_cb.client_env_cb = NULL;
    gstk_shared_data.envelope_cmd_ref_table[ref_id].user_data = 0;
    gstk_shared_data.envelope_cmd_ref_table[ref_id].unique_ref_id = 0;
    memset(&(gstk_shared_data.envelope_cmd_ref_table[ref_id].gstk_cc_cache_info), 0xFF,
       sizeof(gstk_shared_data.envelope_cmd_ref_table[ref_id].gstk_cc_cache_info));
    gstk_shared_data.envelope_cmd_ref_table[ref_id].evt_list_code = 0xFF;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid ref_id: 0x%x", ref_id);
  }
} /* gstk_util_env_cmd_ref_table_free_slot */

/*===========================================================================
FUNCTION gstk_util_save_env_cmd_info

DESCRIPTION
  This function get a free slot from the envelope_cmd_ref_table, and populate
  it with the user envelope command info.

PARAMETERS
  client_id: [Input] client id for referencing the client callback if the
                     envelope command is not sent by internal modules
  env_cb: [Input] function to be called upon response from Card is received
                  for the envelope command
  env_cmd_type: [Input] envelope command type
  user_data: [Input] user data
  env_ref_id: [Input/Output] envelope reference id in the
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
gstk_status_enum_type gstk_util_save_env_cmd_info(
  gstk_client_id_type     client_id,
  gstk_cmd_enum_type      env_cmd_type,
  gstk_cmd_type         * cmd_ptr,
  uint32                * env_ref_ptr
)
{
  gstk_cmd_enum_type      env_rsp_type  = GSTK_CMD_ENUM_NOT_USE;
  gstk_evt_cb_funct_type  client_cb     = NULL;
  uint32                  user_data     = 0x00;
  gstk_status_enum_type   gstk_status   = GSTK_SUCCESS;
  gstk_nv_items_data_type nv_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if((NULL == cmd_ptr) || (NULL == env_ref_ptr))
  {
    UIM_MSG_ERR_2("Null Input Pointer: cmd_ptr = 0x%x, env_ref_ptr = 0x%x",
                  cmd_ptr, env_ref_ptr);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  /* get the expected rsp type and client_cb data */
  UIM_MSG_HIGH_2("In gstk_util_save_env_cmd_info(), Envelope Type is 0x%x, Cat Version 0x%x",
                 env_cmd_type, nv_data.cat_version);
  switch(env_cmd_type)
  {
    case GSTK_MENU_SELECTION_IND:
      client_cb    = cmd_ptr->menu_selection_envelope_ind.client_cb;
      env_rsp_type = GSTK_MENU_IND_RSP;
      break;
    case GSTK_SMS_PP_DOWNLOAD_IND:
      /* CC Envelope API does not accepts client_id as argument, use GSTKs instead
         should not matter anyway as callback is not NULL */
      client_cb    = cmd_ptr->sms_pp_envelope_ind.client_cb;
      env_rsp_type = GSTK_SMS_PP_DOWNLOAD_IND_RSP;
      break;
    case GSTK_MT_CALL_EVT_IND:
      env_rsp_type = GSTK_MT_CALL_EVT_IND_RSP;
      break;
    case GSTK_CALL_CONNECTED_EVT_IND:
      env_rsp_type = GSTK_CALL_CONNECTED_EVT_IND_RSP;
      break;
    case GSTK_CALL_DISCONNECTED_EVT_IND:
      env_rsp_type = GSTK_CALL_DISCONNECTED_EVT_IND_RSP;
      break;
    case GSTK_IDLE_SCRN_EVT_IND:
      env_rsp_type = GSTK_IDLE_SCRN_EVT_IND_RSP;
      break;
    case GSTK_LOCATION_STATUS_EVT_IND:
      env_rsp_type = GSTK_LOCATION_STATUS_EVT_IND_RSP;
      break;
    case GSTK_USER_ACTIVITY_EVT_IND:
      env_rsp_type = GSTK_USER_ACTIVITY_EVT_IND_RSP;
      break;
    case GSTK_BROWSER_TERMINATION_EVT_IND:
      env_rsp_type = GSTK_BROWSER_TERMINATION_EVT_IND_RSP;
      break;
    case GSTK_LANG_SELECTION_EVT_IND:
      env_rsp_type = GSTK_LANG_SELECTION_EVT_IND_RSP;
      break;
    case GSTK_MO_SMS_CTRL_IND:
      /* MO SMS API does not accepts client_id as argument, use GSTKs instead
         should not matter anyway as callback is not NULL */
      client_cb    = cmd_ptr->mo_sms_ctrl_envelope_ind.client_cb;
      env_rsp_type = GSTK_MO_SMS_CTRL_IND_RSP;
      break;
    case GSTK_CC_IND:
      /* CC Envelope API does not accepts client_id as argument, use GSTKs instead
         should not matter anyway as callback is not NULL */
      client_cb     = cmd_ptr->cc_envelope_ind.client_cb;
      env_rsp_type  = GSTK_CC_IND_RSP;
      break;
    case GSTK_TIMER_EXPIRE_IND:
      env_rsp_type = GSTK_TIMER_EXPIRE_IND_RSP;
      break;
    case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
      client_cb    = cmd_ptr->cell_broadcast_envelope_ind.client_cb;
      env_rsp_type = GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP;
      break;
    case GSTK_ACCESS_TECH_EVT_IND:
      env_rsp_type = GSTK_ACCESS_TECH_EVT_IND_RSP;
      break;
    case GSTK_DATA_AVAIL_EVT_IND:
      env_rsp_type = GSTK_DATA_AVAIL_EVT_IND_RSP;
      break;
    case GSTK_CH_STATUS_EVT_IND:
      env_rsp_type = GSTK_CH_STATUS_EVT_IND_RSP;
      break;
    case GSTK_RAW_ENVELOPE_IND:
      client_cb    = cmd_ptr->raw_envelope_ind.client_cb;
      env_rsp_type = GSTK_RAW_ENVELOPE_RSP;
      break;
    case GSTK_LOCATION_INFORMATION_IND:
      env_rsp_type = GSTK_LOC_INFO_ENVELOPE_RSP;
      break;
    case GSTK_NW_REJ_EVT_IND:
      env_rsp_type = GSTK_NW_REJ_EVT_IND_RSP;
      break;
    /* Allow fallthrough to Default if cat version != 6 */
    case GSTK_NW_SEARCH_MODE_EVT_IND:
      if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
      {
        env_rsp_type = GSTK_NW_SEARCH_MODE_EVT_IND_RSP;
      }
      break;
    case GSTK_CSG_CELL_SELECTION_EVT_IND:
      if (nv_data.cat_version >= GSTK_CFG_CAT_VER9)
      {
        env_rsp_type = GSTK_CSG_CELL_SELECTION_EVT_IND_RSP;
      }
      break;
    case GSTK_IMS_REG_EVT_IND:
      if (nv_data.cat_version >= GSTK_CFG_CAT_VER8)
      {
        env_rsp_type = GSTK_IMS_REG_EVT_IND_RSP;
      }
      break;
    default:
      break;
  }

  user_data = cmd_ptr->general_cmd.message_header.user_data;
  /* get a valid location from the env cmd reference table */
  *env_ref_ptr = gstk_util_env_cmd_ref_table_get_next_free_slot();
  UIM_MSG_HIGH_1("Env ref id is 0x%x", *env_ref_ptr);
  if ((GSTK_CMD_DETAILS_REF_TABLE_FULL == *env_ref_ptr)
      || (*env_ref_ptr > GSTK_MAX_PROACTIVE_COMMAND))
  {
    UIM_MSG_HIGH_0("envelope_cmd_ref_table full");
    gstk_status  = GSTK_MEMORY_ERROR;
    /* save env req info in last slot */
    *env_ref_ptr = GSTK_MAX_PROACTIVE_COMMAND;
    gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].is_free = FALSE;
  }

  /* populate the table */
  gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].user_data = user_data;
  if(client_cb != NULL)
  {
    gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].evt_cb_type =
      GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT;
    gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].env_cb.client_env_cb = client_cb;
  }
  else
  {
    if(gstk_util_is_valid_client_id(client_id))
    {
      if(gstk_shared_data.gstk_client_table[client_id - 1].client_cb != NULL)
      {
        gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].evt_cb_type =
          GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT;
        gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].env_cb.new_client_env_cb =
          (gstk_toolkit_evt_cb_func_type)gstk_shared_data.gstk_client_table[client_id - 1].client_cb;
      }
      else if(gstk_shared_data.gstk_client_table[client_id - 1].client_type == GSTK_GSTK_TYPE)
      {
        gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].evt_cb_type = GSTK_CLI_CB_NONE;
        gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].env_cb.new_client_env_cb = NULL;
      }
      else
      {
        UIM_MSG_HIGH_1("No matching callback format, cli 0x%x", client_id);
        gstk_util_env_cmd_ref_table_free_slot(*env_ref_ptr);
        return GSTK_CLIENT_NOT_REGISTERED;
      }
    }
  } /* more room for the envlope command */

  /* get the expected rsp type */
  UIM_MSG_HIGH_1("Env rsp type is 0x%x", env_rsp_type);
  gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].expected_cmd_rsp_type =
    env_rsp_type;
  gstk_shared_data.envelope_cmd_ref_table[*env_ref_ptr].client_id =
    client_id;
  return gstk_status;
} /* gstk_util_save_env_cmd_info */

/*===========================================================================
FUNCTION gstk_util_retrieve_env_cmd_info

DESCRIPTION
  This function will retrieve the envelope command info from the envelope table
  based on the envelope reference id and the command type.

PARAMETERS
  env_cmd_type:  [Input] envelope command type
  user_data_ptr: [Output] pointer to user data
  env_cb:        [Input] function to be called upon response from Card is received
                  for the envelope command
  env_ref_id:    [Input] envelope reference id in the envelope_cmd_ref_table

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
  uint32                  env_ref_id)
{
  if(user_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("user_data_ptr NULL");
    return GSTK_BAD_PARAM;
  }

  if(env_cb_ptr == NULL)
  {
    UIM_MSG_ERR_0("env_cb_ptr NULL");
    return GSTK_BAD_PARAM;
  }

  if ((env_ref_id == GSTK_CMD_DETAILS_REF_TABLE_FULL) ||
      (env_ref_id > (GSTK_MAX_PROACTIVE_COMMAND-1)))
  {
    UIM_MSG_ERR_0("Invalid env_ref_id");
    return GSTK_BAD_PARAM;
  }

  if(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].is_free == FALSE &&
     gstk_shared_data.envelope_cmd_ref_table[env_ref_id].expected_cmd_rsp_type == env_cmd_type)
  {
      *user_data_ptr = gstk_shared_data.envelope_cmd_ref_table[env_ref_id].user_data ;
      if(gstk_shared_data.envelope_cmd_ref_table[env_ref_id].evt_cb_type == GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT &&
         gstk_shared_data.envelope_cmd_ref_table[env_ref_id].env_cb.client_env_cb != NULL)
      {
        *env_cb_ptr = gstk_shared_data.envelope_cmd_ref_table[env_ref_id].env_cb.client_env_cb;
      }
      else
      {
        env_cb_ptr = NULL;
        UIM_MSG_HIGH_0("env cb ptr NULL");
      }
      return GSTK_SUCCESS;
  }
  else
  {
    UIM_MSG_ERR_0("Envelope could not be found");
    return GSTK_ERROR;
  }
} /* gstk_util_retrieve_env_cmd_info */


/*===========================================================================
FUNCTION gstk_valid_tag

DESCRIPTION
  Check for proactive command tag matches

PARAMETERS
  tag: [Input] tag extracted from the card data buffer
  type: [Input] the type that we are trying to match the tag with

DEPENDENCIES
  None

RETURN VALUE
  boolean (TRUE => match
           FALSE => does not match)

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_valid_tag(uint8 tag,tags_enum_type type)
{
    boolean valid = FALSE;

    if(tag == (int)type) { /* if tag of BER-TLV tag or command tag w/o CR */
        valid = TRUE;
    }
    if (!valid) { /* check if the command tag is passed in with CR */
      if (tag > 0x80) { /* tag with CR */
       tag -= 0x80;
      }
      if(tag == (int)type) {
        valid = TRUE;
      }
    }

    UIM_MSG_LOW_2("TAG = 0x%x, TYPE = 0x%x", tag, type);
    return(valid);
} /* gstk_valid_tag*/


/*===========================================================================
FUNCTION gstk_get_uim_buf

DESCRIPTION
  This function returns a pointer to heap for buffer allocation for UIM
  Server interface.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  uim_cmd_type pointer

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
uim_cmd_type *gstk_get_uim_buf (
  void
)
{
  uim_cmd_type *cmd;

    /* Allocate command buffer space */
    /* from TMC Heap                 */
  cmd = (uim_cmd_type*)GSTK_CALLOC(sizeof(uim_cmd_type));
  /* check for NULL cmd pointer before dereferencing */
  if (cmd != NULL)
  {
    cmd->hdr.cmd_hdr.done_q_ptr = NULL;
  }

  return cmd;
} /* gstk_get_uim_buf */


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
void gstk_send_cmd_to_uim_server (
  uim_cmd_type *cmd_ptr
)
{
#if defined (FEATURE_GSTK_FUSION_REMOTE) || defined (FEATURE_GSTK_TAL)
  gstk_tal_uim_cmd_type                     tal_uim_cmd;
#endif /* FEATURE_GSTK_FUSION_REMOTE || FEATURE_GSTK_TAL */
  unsigned long     seconds = 0;

  uim_return_type  uim_state = UIM_SUCCESS;
  uim_rpt_type     uim_rpt;
  memset((void *)&uim_rpt, 0, sizeof(uim_rpt_type));

  UIM_MSG_HIGH_0("In gstk_send_cmd_to_uim_server()");
  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr ERR:NULL");
    return;
  }
  /* Route all commands except UIM_POLLING_INTERVAL_F through TAL, since
     UIM_POLLING_INTERVAL_F is command that instructs UIM to change/(turn OFF)
     POLL interval and does not actually send any data to the card */

  if(cmd_ptr->hdr.command != UIM_POLLING_INTERVAL_F)
  {
#if defined (FEATURE_GSTK_FUSION_REMOTE) || defined (FEATURE_GSTK_TAL)
    memset((void *)&tal_uim_cmd, 0, sizeof(gstk_tal_uim_cmd_type));

    switch(cmd_ptr->hdr.command)
    {
    case UIM_TERMINAL_RESPONSE_F:
      tal_uim_cmd.cmd_type = GSTK_TERMINAL_RSP;
      tal_uim_cmd.user_data = 0;
      tal_uim_cmd.num_bytes = cmd_ptr->terminal_response.num_bytes;
      tal_uim_cmd.data_ptr = cmd_ptr->terminal_response.data;
      break;
    case UIM_TERMINAL_PROFILE_F:
#ifdef FEATURE_GSTK_FUSION_REMOTE
      memset((void *)&uim_rpt, 0, sizeof(uim_rpt_type));
      UIM_MSG_HIGH_0("Fake TP Response for Remote");
      uim_rpt.user_data   = 0;
      uim_rpt.sw1         = SW1_NORMAL_END;
      uim_rpt.sw2         = SW2_NORMAL_END;
      uim_rpt.rpt_status  = UIM_PASS;
      uim_rpt.slot        = UIM_SLOT_1;
      uim_rpt.rpt_type    = UIM_TERMINAL_PROFILE_R;
      gstk_uim_terminal_profile_report(&uim_rpt);
      gstk_free(cmd_ptr);
      return;
#else
      tal_uim_cmd.cmd_type = GSTK_TERMINAL_PROFILE_CMD;
      tal_uim_cmd.user_data = 0;
      tal_uim_cmd.num_bytes = cmd_ptr->terminal_profile.num_bytes;
      tal_uim_cmd.data_ptr = (uint8 *)GSTK_CALLOC(tal_uim_cmd.num_bytes);
      GSTK_RETURN_IF_NULL_PTR(tal_uim_cmd.data_ptr);
      (void)gstk_memcpy(tal_uim_cmd.data_ptr,
                     cmd_ptr->terminal_profile.data,
                     cmd_ptr->terminal_profile.num_bytes,
                     tal_uim_cmd.num_bytes,
                     cmd_ptr->terminal_profile.num_bytes);
      break;
#endif /* FEATURE_GSTK_FUSION_REMOTE */
    case UIM_ENVELOPE_F:
      tal_uim_cmd.cmd_type = GSTK_ENVELOPE_CMD;
      tal_uim_cmd.user_data = cmd_ptr->hdr.user_data;
      tal_uim_cmd.num_bytes = cmd_ptr->envelope.num_bytes + cmd_ptr->envelope.offset + 1;
      tal_uim_cmd.data_ptr = (uint8 *)GSTK_CALLOC(tal_uim_cmd.num_bytes);
      GSTK_RETURN_IF_NULL_PTR(tal_uim_cmd.data_ptr);
      tal_uim_cmd.data_ptr[0] = cmd_ptr->envelope.offset;
      (void)gstk_memcpy(
           tal_uim_cmd.data_ptr + 1,
           cmd_ptr->envelope.data,
           cmd_ptr->envelope.num_bytes+cmd_ptr->envelope.offset,
           cmd_ptr->envelope.num_bytes+cmd_ptr->envelope.offset,
           cmd_ptr->envelope.num_bytes+cmd_ptr->envelope.offset);
      break;
    default:
      UIM_MSG_ERR_0("Unsupported command to TAL");
      break;
    }
#ifdef FEATURE_GSTK_TAL
    (void)gstk_tal_send_cmd(GSTK_TAL_LOCAL_CLIENT, &tal_uim_cmd);
#else
    (void)gstk_tal_send_cmd(GSTK_TAL_REMOTE_CLIENT, &tal_uim_cmd);
#endif /* FEATURE_GSTK_TAL */
    if(tal_uim_cmd.cmd_type == GSTK_ENVELOPE_CMD ||
       tal_uim_cmd.cmd_type == GSTK_TERMINAL_PROFILE_CMD)
    {
      gstk_free(tal_uim_cmd.data_ptr);
    }
    gstk_send_cmd_to_uim_free_q(cmd_ptr);
    cmd_ptr = NULL;
    return;
#endif /* FEATURE_GSTK_FUSION_REMOTE || FEATURE_GSTK_TAL */
  } /* command != UIM_POLLING_INTERVAL_F */

  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  /* if cmd is envelope cmd, then replace the user data with uinque ref id,
     from the envelope command table which will be used to verify
     during the envelope rsp */
  if(cmd_ptr->hdr.command == UIM_ENVELOPE_F)
  {
    /* Generat unique ref id based on the env cmd table index */
    gstk_shared_data.envelope_cmd_ref_table[cmd_ptr->hdr.user_data].unique_ref_id =
                                                    gstk_util_generate_ref_id(cmd_ptr->hdr.user_data);
    /* pass this unique id along with uim cmd and gstk will get same in uim envelope report */
    cmd_ptr->hdr.user_data = gstk_shared_data.envelope_cmd_ref_table[cmd_ptr->hdr.user_data].unique_ref_id;
  }

  (void)q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);

  if(cmd_ptr->hdr.command == UIM_TERMINAL_RESPONSE_F)
  {
    /* Generate and pass the unique_ref_id to UIM driver for TR report */
    gstk_curr_inst_ptr->tr_unique_ref_id = gstk_util_generate_ref_id(cmd_ptr->hdr.user_data);
    cmd_ptr->hdr.user_data = gstk_curr_inst_ptr->tr_unique_ref_id;
    /* Check if card has sent same proactive cmd as last time */
    if (gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count != 0)
    { /* Card has repeated the cmd */
      if (GSTK_SUCCESS == gstk_util_cmp_with_cached_term_rsp (cmd_ptr))
      {
        seconds = (gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count * GSTK_COMMAND_DUPLICITY_MULTIPLIER);
        seconds %= GSTK_COMMAND_DUPLICITY_RESET_COUNT;
        UIM_MSG_HIGH_1 ("Starting the TR delay timer, dupe count: 0x%x",
                        gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count);
        /* TR is same as last time - start timer and send TR after timer is expired */
        (void)rex_clr_timer(&gstk_curr_inst_ptr->gstk_terminal_rsp_delay_timer);
        (void)rex_set_timer(&gstk_curr_inst_ptr->gstk_terminal_rsp_delay_timer,
                             seconds*1000);
        /* Free the buffer, as new will be allocated when Timer expires */
        gstk_send_cmd_to_uim_free_q(cmd_ptr);
        gstk_curr_inst_ptr->gstk_cmd_cache.tr_rcvd = TRUE;
        if (!gstk_nv_get_me_feature_status(GSTK_CFG_FEATURE_ME_DISABLE_DUP_ERR_TR_DELAY_PCMD_PARALLEL_PROCESSING) &&
            (gstk_shared_data.ui_nw_proactive_session_slot_in_progress == gstk_curr_inst_ptr->slot_id))
        {
          gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
          gstk_util_check_slot_with_fetch_rejected();
        }
        return;
      }
      else
      {
        UIM_MSG_HIGH_0 ("Card has repeated the cmd. TR is not same");
        /* Card has repeated the cmd but TR is not same */
        gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count = 0;
        /* Cache the TR */
        if(gstk_byte_offset_memcpy(gstk_curr_inst_ptr->gstk_cmd_cache.term_resp,
                                   cmd_ptr->terminal_response.data,
                                   0,
                                   cmd_ptr->terminal_response.num_bytes,
                                   sizeof(gstk_curr_inst_ptr->gstk_cmd_cache.term_resp),
                                   sizeof(cmd_ptr->terminal_response.data)) <
           cmd_ptr->terminal_response.num_bytes)
        {
          UIM_MSG_ERR_0 ("error: gstk_byte_offset_memcpy failed, could not cache TR");
          gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length = 0;
        }
        else
        {
          gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length =
          cmd_ptr->terminal_response.num_bytes;
        }
      }
    }
    else if (gstk_curr_inst_ptr->gstk_cmd_cache.dupe_count == 0)
    {
      /* Cache the TR */
      if(gstk_memcpy (gstk_curr_inst_ptr->gstk_cmd_cache.term_resp,
                      cmd_ptr->terminal_response.data,
                      cmd_ptr->terminal_response.num_bytes,
                      sizeof(gstk_curr_inst_ptr->gstk_cmd_cache.term_resp),
                      cmd_ptr->terminal_response.num_bytes) <
         (size_t)(cmd_ptr->terminal_response.num_bytes))
      {
        UIM_MSG_ERR_0 ("error: memscpy failed, could not cache TR");
        gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length = 0;
      }
      gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length =
      cmd_ptr->terminal_response.num_bytes;
    }
    else
    {
      UIM_MSG_ERR_0 ("error: Couldn't cache the TR");
    }
  } /* if(cmd_ptr->hdr.command == UIM_TERMINAL_RESPONSE_F) */

  uim_state = uim_cmd(cmd_ptr);
  if(uim_state != UIM_SUCCESS)
  {
     uim_rpt.user_data   = cmd_ptr->hdr.user_data;
     uim_rpt.sw1         = SW1_PROBLEM;
     uim_rpt.sw2         = SW2_NORMAL_END;
     uim_rpt.rpt_status  = UIM_FAIL;
     uim_rpt.slot        = cmd_ptr->hdr.slot;
     switch(cmd_ptr->hdr.command)
     {
     case UIM_TERMINAL_RESPONSE_F:
       uim_rpt.rpt_type    = UIM_TERMINAL_RESPONSE_R;
       gstk_uim_terminal_response_report(&uim_rpt);
       break;
     case UIM_TERMINAL_PROFILE_F:
       uim_rpt.rpt_type    = UIM_TERMINAL_PROFILE_R;
       gstk_uim_terminal_profile_report(&uim_rpt);
       break;
     case UIM_ENVELOPE_F:
       uim_rpt.rpt.envelope.get_resp_sw1 = SW1_PROBLEM;
       uim_rpt.rpt.envelope.get_resp_sw2 = SW2_NORMAL_END;
       uim_rpt.rpt_type    = UIM_ENVELOPE_R;
       gstk_uim_envelope_report(&uim_rpt);
       break;
     default:
       UIM_MSG_ERR_0("Unsupported command by GSTK");
       break;
     }
  }
  gstk_send_cmd_to_uim_free_q(cmd_ptr);
  cmd_ptr = NULL;
} /* gstk_send_cmd_to_uim_server */


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
void gstk_send_cmd_to_uim_free_q (
  uim_cmd_type *cmd_ptr
)
{
    if(cmd_ptr == NULL)
    {
      UIM_MSG_ERR_0("cmd_ptr ERR:NULL");
      return;
    }
    gstk_free(cmd_ptr);
} /* gstk_send_cmd_to_uim_free_qr */


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
gstk_status_enum_type gstk_util_populate_uim_buf_hdr_info(
  uim_cmd_name_type                 uim_cmd_name,
  uim_cmd_type                      **uim_cmd_ptr_ptr )
{
#ifndef FEATURE_GSTK_FUSION_REMOTE
    uim_cmd_type*            uim_cmd_ptr;
    gstk_status_enum_type    gstk_status = GSTK_SUCCESS;

    if(uim_cmd_ptr_ptr == NULL)
    {
        UIM_MSG_ERR_0("uim_cmd_ptr_ptr ERR:NULL");
        return GSTK_BAD_PARAM;
    }

    GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

    uim_cmd_ptr = *uim_cmd_ptr_ptr;

    if((uim_cmd_ptr) == NULL)
    {
        UIM_MSG_HIGH_0("No Buffer from UIM");
        return GSTK_MEMORY_ERROR;
    }

    uim_cmd_ptr->hdr.command                 = uim_cmd_name;

    uim_cmd_ptr->hdr.cmd_hdr.task_ptr        = NULL;
    uim_cmd_ptr->hdr.cmd_hdr.sigs            = 0;

     uim_cmd_ptr->hdr.slot                    =
       (uim_slot_type)gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
    uim_cmd_ptr->hdr.protocol                 = UIM_MAXIMUM_MODE;

    switch(uim_cmd_name)
    {
    case UIM_TERMINAL_RESPONSE_F:
#if !defined (FEATURE_GSTK_TAL) && !defined (FEATURE_GSTK_FUSION_REMOTE)
        uim_cmd_ptr->hdr.rpt_function         = gstk_uim_terminal_response_report;
#endif /*! (FEATURE_GSTK)TAL ) && ! (FEATURE_GSTK_FUSION_REMOTE)*/
        uim_cmd_ptr->hdr.options              = UIM_OPTION_ALWAYS_RPT;
        break;
    case UIM_ENVELOPE_F:
        uim_cmd_ptr->hdr.rpt_function         = gstk_uim_envelope_report;
        uim_cmd_ptr->hdr.options              = UIM_OPTION_ALWAYS_RPT;
        break;
    case UIM_TERMINAL_PROFILE_F:
#if ! defined (FEATURE_GSTK_TAL)  && !defined (FEATURE_GSTK_FUSION_REMOTE)
        uim_cmd_ptr->hdr.rpt_function         = gstk_uim_terminal_profile_report;
#endif /*! (FEATURE_GSTK_TAL) && ! (FEATURE_GSTK_FUSION_REMOTE)*/
        uim_cmd_ptr->hdr.options              = UIM_OPTION_ALWAYS_RPT;
        break;
    case UIM_POLLING_INTERVAL_F:
        uim_cmd_ptr->hdr.rpt_function         = gstk_uim_polling_report;
        uim_cmd_ptr->hdr.options              = UIM_OPTION_ALWAYS_RPT;
        break;
    default:
        UIM_MSG_ERR_1 ("Invalid UIM Cmd, %x", uim_cmd_name);
#ifndef FEATURE_GSTK_TAL
        gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
        *uim_cmd_ptr_ptr = NULL;
#endif /* ! FEATURE_GSTK_TAL */
        gstk_status = GSTK_BAD_PARAM;
        break;
    }
    return gstk_status;
#else
    (*uim_cmd_ptr_ptr)->hdr.command                 = uim_cmd_name;
    return GSTK_SUCCESS;
#endif /*!FEATURE_GSTK_FUSION_REMOTE*/
}  /* gstk_util_populate_uim_buf_hdr_info */


/*===========================================================================
FUNCTION gstk_find_length_of_length_value

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
uint32 gstk_find_length_of_length_value(const uint8* length_value)
{
    uint8 ret_val = 0;

    if(length_value == NULL)
    {
      UIM_MSG_ERR_0("length_value ERR:NULL");
      return ret_val;
    }

    /*
    ** This length field can either be 0x00-0x7F or
    ** 0x81 if the 2nd byte is used
    */

    UIM_MSG_LOW_1("IN gstk_find_length_of_length_value() and length_value[0] is 0x%x",
                  length_value[0]);
    if(length_value[0] <= 0x7F)
    {
        ret_val = 1;
    }
    else if(length_value[0] == 0x81)
    {
        ret_val = 2;
    }
    return(ret_val);
} /* gstk_find_length_of_length_value */


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
boolean gstk_util_is_valid_client_id(
    gstk_client_id_type  client_id )
{

  if (client_id > GSTK_MAX_CLIENT) {
    return FALSE;
  }
  if (client_id ==0) {
    return FALSE;
  }
  if (client_id == GSTK_CLIENT_ID) {
    return TRUE;
  }
  return (!gstk_shared_data.gstk_client_table[client_id-1].is_free);
} /* gstk_util_is_valid_client_id */

/*===========================================================================
FUNCTION gstk_util_is_valid_data_format_req

DESCRIPTION
  This function determines if the client has requested a valid data format ie
  RAW or GSTK.

PARAMETERS
  data_format_type: [Input] Data format type requested by the client

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE  - if data format type is valid (GSTK/RAW)
    FALSE - if data format type is invalid

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_valid_data_format_req(
  gstk_client_pro_cmd_data_format_enum_type   data_format_type)
{
  switch(data_format_type)
  {
  case GSTK_RAW_FORMAT:
  case GSTK_GSTK_FORMAT:
    return TRUE;
  default:
    return FALSE;
  }
} /* gstk_util_is_valid_data_format_req */

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
boolean gstk_util_is_valid_func_type(
  gstk_tk_functionality_enum_type func_type)
{
  switch(func_type)
  {
  case GSTK_HANDLE_DISPLAY_FUNC_ONLY:
  case GSTK_HANDLE_MODEM_FUNC_ONLY:
  case GSTK_HANDLE_ALL_FUNC:
    return TRUE;
  default:
    return FALSE;
  }
} /* gstk_util_is_valid_func_type */

/*===========================================================================
FUNCTION gstk_util_convert_cmd_type_to_reg_bitmask

DESCRIPTION
  This function converts the command type into the registration bitmask

PARAMETERS
  cmd_type: [Input] type of command that GSTK is going to search for in the
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
gstk_status_enum_type gstk_util_convert_cmd_type_to_reg_bitmask(
   gstk_cmd_enum_type   cmd_type,
   uint64               *cmd_bitmask_ptr,
   uint32               *class_bitmask_ptr)
{
  gstk_nv_items_data_type nv_data;

  UIM_MSG_HIGH_1("In gstk_util_convert_cmd_type_to_reg_bitmask cmd_type 0x%x",
                 cmd_type);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if (!cmd_bitmask_ptr || !class_bitmask_ptr)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  switch (cmd_type) {
  case GSTK_DISPLAY_TEXT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_DISPLAY_TEXT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_GET_INKEY_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_GET_INKEY_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_GET_INPUT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_GET_INPUT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_LAUNCH_BROWSER_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_LAUNCH_BROWSER_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_C_CMD;
    break;

  case GSTK_MORE_TIME_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_MORE_TIME_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PLAY_TONE_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PLAY_TONE_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_POLL_INTERVAL_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_POLL_INTERVAL_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_POLLING_OFF_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_POLLING_OFF_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PROVIDE_LOCAL_INFO_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_LOCAL_INFO_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PROVIDE_LANG_INFO_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_LANG_INFO_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_PROVIDE_BATTERY_STATUS_REQ:
    if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
    {
      *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_BATTERY_STATE_EVT;
      *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    }
    break;

  case GSTK_PROVIDE_DATE_TIME_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_PROVIDE_DATE_TIME_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_ACTIVATE_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_ACTIVATE_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_L_CMD;
    break;

  case GSTK_REFRESH_REQ:
    *cmd_bitmask_ptr = GSTK_REG_REFRESH_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_RUN_AT_CMDS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_RUN_AT_CMDS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_B_CMD;
    break;

  case GSTK_SELECT_ITEM_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SELECT_ITEM_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_SMS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_SMS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_SS_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_SS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_USSD_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_USSD_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_CALL_REQ:
    *cmd_bitmask_ptr = GSTK_REG_SETUP_CALL_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVENT_LIST_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVENT_LIST_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_MENU_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_MENU_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_IDLE_TEXT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_IDLE_TEXT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SEND_DTMF_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SEND_DTMF_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_LANG_NOTIFICATION_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_LANG_NOTIFY_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_TIMER_MANAGEMENT_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_TIMER_MANAGEMENT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_OPEN_CH_REQ:
  case GSTK_CLOSE_CH_REQ:
  case GSTK_SEND_DATA_REQ:
  case GSTK_RECEIVE_DATA_REQ:
  case GSTK_GET_CH_STATUS_REQ:
  case GSTK_OPEN_CH_CNF:
  case GSTK_CLOSE_CH_CNF:
    *cmd_bitmask_ptr   = GSTK_REG_DATA_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  case GSTK_SCWS_OPEN_CH_REQ:
  case GSTK_SCWS_CLOSE_CH_REQ:
  case GSTK_SCWS_SEND_DATA_REQ:
  case GSTK_SCWS_DATA_AVAIL_RSP_REQ:
    *cmd_bitmask_ptr   = GSTK_REG_SCWS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  /* Envelope Indication Responses */
  case GSTK_MT_CALL_EVT_IND_RSP:
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
    *cmd_bitmask_ptr   = GSTK_REG_EVT_DOWNLOAD_RSP_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
    *cmd_bitmask_ptr   = GSTK_REG_EVT_DOWNLOAD_RSP_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_C_CMD;
    break;

  /* Profile download response */
  case GSTK_PROFILE_DL_IND_RSP:
    *cmd_bitmask_ptr   = GSTK_REG_PROFILE_DL_RSP_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_IDLE_SCREEN_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_IDLE_MENU_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_USER_ACT_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_USER_ACT_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_BROWSER_TERM_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_BROWSER_TERM_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_C_CMD;
    break;

  case GSTK_SETUP_EVT_LANG_SEL_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_LANG_SEL_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_0_CMD;
    break;

  case GSTK_SETUP_EVT_DATA_AVAIL_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_DATA_AVAILABLE_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  case GSTK_SETUP_EVT_CH_STATUS_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_CH_STATUS_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_E_CMD;
    break;

  case GSTK_SETUP_EVT_HCI_CONNECT_NOTIFY:
    *cmd_bitmask_ptr   = GSTK_REG_SETUP_EVT_HCI_CONN_EVT;
    *class_bitmask_ptr = GSTK_REG_CATEGORY_M_CMD;
    break;

  default:
    return GSTK_UNSUPPORTED_COMMAND;
  }

  return GSTK_SUCCESS;
} /*gstk_util_convert_cmd_type_to_reg_bitmask*/

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
gstk_status_enum_type gstk_util_find_client_with_registered_event(
    gstk_client_id_type                        *client_id_ptr,
    gstk_cmd_enum_type                          cmd_type,
    gstk_client_pro_cmd_data_format_enum_type   format_type,
    gstk_tk_functionality_enum_type             cli_func_type
)
{
  uint64                cmd_bitmask    = 0;
  uint32                class_bitmask  = 0;
  uint32                index          = 0;
  uint32                l_index        = 0;
  uint32                c_index        = 0;
  uint64                mask           = 1;
  uint32                adj_offset     = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(client_id_ptr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  *client_id_ptr = GSTK_MAX_CLIENT + 1;

  if(gstk_util_convert_cmd_type_to_reg_bitmask(cmd_type,
                                               &cmd_bitmask,
                                               &class_bitmask) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_0("conversion to reg bitmask failed.");
    return GSTK_ERROR;
  }

  if(!gstk_util_is_valid_data_format_req(format_type))
  {
    UIM_MSG_ERR_1("Invalid data format 0x%x", format_type);
    return GSTK_BAD_PARAM;
  }

  if(!gstk_util_is_valid_func_type(cli_func_type))
  {
    UIM_MSG_ERR_1("Invalid client func type 0x%x", cli_func_type);
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_1("In gstk_util_find_client_with_registered_event, class_bitmask 0x%x",
                 class_bitmask);
  switch(class_bitmask)
  {
  case GSTK_REG_CATEGORY_0_CMD:
    l_index = GSTK_CLASS_0_BEGIN_INDEX;
    break;
  case GSTK_REG_CATEGORY_B_CMD:
    l_index      = GSTK_CLASS_B_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_B_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_C_CMD:
    l_index      = GSTK_CLASS_C_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_C_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_E_CMD:
    l_index      = GSTK_CLASS_E_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_E_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_L_CMD:
    l_index      = GSTK_CLASS_L_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_L_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_M_CMD:
    l_index      = GSTK_CLASS_M_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_M_ADJ_OFFSET;
    break;
  default:
    UIM_MSG_ERR_1("class bitmask unknown 0x%x", class_bitmask);
    return GSTK_ERROR;
  }

  /* Evt bitmask for different classes at present do not overlap due to backward
     compatibility. Hence the two lines below will avoiding checking bits for
     irrelevant classes and directly jump to the bit where a particular class's
     events begin
  */
  cmd_bitmask    = cmd_bitmask >> l_index;
  index          = index + l_index;

  while(cmd_bitmask)
  {
    if(cmd_bitmask & mask)
    {
      break;
    }
    cmd_bitmask = cmd_bitmask >> 1;
    index++;
  }/* End of while */

  c_index = l_index + index - adj_offset;

  if((c_index >= GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY) || ((int32) c_index < 0))
  {
    UIM_MSG_ERR_2("index out of range c-index 0x%x index 0x%x ",
                  c_index, index);
    return GSTK_ERROR;
  }

  switch(cli_func_type)
  {
  case GSTK_HANDLE_DISPLAY_FUNC_ONLY:
    if((gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.is_reg) &&
       (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.data_format_type == format_type))
    {
      *client_id_ptr =
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.client_id;
    }
    else
    {
      UIM_MSG_ERR_1("Registration lookup mismatch error 0x%x", c_index);
      return GSTK_NO_CLIENT_REG_FOUND;
    }
    break;
  case GSTK_HANDLE_MODEM_FUNC_ONLY:
    if((gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg) &&
       (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.data_format_type == format_type))
    {
      *client_id_ptr =
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.client_id;
    }
    else
    {
      UIM_MSG_ERR_1("Registration lookup mismatch error 0x%x", c_index);
      return GSTK_NO_CLIENT_REG_FOUND;
    }
    break;
  case GSTK_HANDLE_ALL_FUNC:
    if((gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.client_id ==
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.client_id ) &&
        (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.data_format_type ==
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.data_format_type) &&
        (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg) &&
        (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg) &&
        (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.data_format_type == format_type))
    {
      *client_id_ptr =
        gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.client_id;
      break;
    }
    else
    {
      UIM_MSG_ERR_1("Registration lookup mismatch error 0x%x", c_index);
      return GSTK_NO_CLIENT_REG_FOUND;
    }
  default:
    UIM_MSG_ERR_1("Unknown client functionality 0x%x", cli_func_type);
    return GSTK_NO_CLIENT_REG_FOUND;
  }/* End of switch */

  if (gstk_util_is_valid_client_id(*client_id_ptr))
  {
    return GSTK_SUCCESS;
  }
  else
  {
    UIM_MSG_ERR_1("Invalid client id (0x%x) in class evt reg table!",
                  *client_id_ptr);
    return GSTK_INVALID_CLIENT_TYPE;
  }
} /* gstk_util_find_client_with_registered_event*/

/*===========================================================================
FUNCTION gstk_util_send_message_to_clients

DESCRIPTION
  This function sends the message to the GSTK clients based on which clients
  have registered to receive that events.

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
gstk_status_enum_type gstk_util_send_message_to_clients(
    gstk_cmd_from_card_type * message_ptr )
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  gstk_client_id_type   client_id   = 0;
  uint32                i           = 0;

  if(message_ptr == NULL)
  {
    UIM_MSG_ERR_0("message_p ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  message_ptr->hdr_cmd.sim_slot_id = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);

  /* is this an end proactive session or toolkit switch event or mode change? */
  if ((message_ptr->hdr_cmd.command_id == GSTK_MORE_TIME_REQ)      ||
      (message_ptr->hdr_cmd.command_id == GSTK_PROFILE_DL_IND_RSP))
  {
    /* send to all client */
    for (i = 1; i< GSTK_MAX_CLIENT; i++) { /* 0 is GSTK */
      if((!gstk_shared_data.gstk_client_table[i].is_free)
         &&
         (gstk_util_get_sim_slot_id(i + 1) == message_ptr->hdr_cmd.sim_slot_id))
      {
        /* client is valid and cb ptr is not NULL
         * to send msg to client, we need call different
         * cb based on client's cb type.
         */
        UIM_MSG_HIGH_1("Send 0x%x msg ID to client",
                       message_ptr->hdr_cmd.command_id);
        gstk_util_call_client_evt_cb(i + 1, message_ptr);
      }
    }
    /* release all the memory allocation since client already copied it */
    gstk_util_release_card_cmd_memory(message_ptr);

    return GSTK_SUCCESS;
  }/* if (message_ptr->hdr_cmd.command_id == GSTK_MORE_TIME_REQ */
  if (message_ptr->hdr_cmd.command_id == GSTK_END_PROACTIVE_CMD_REQ)
  {
    /* send to relevant clients */
    for (i = 1; i< GSTK_MAX_CLIENT; i++)
    {
      if ((gstk_shared_data.gstk_client_table[i].is_free)
          ||
          (gstk_util_get_sim_slot_id(i + 1) != message_ptr->hdr_cmd.sim_slot_id))
      {
        continue;
      }
      if((gstk_shared_data.gstk_client_table[i].client_type == GSTK_UI_TYPE) &&
         (gstk_curr_inst_ptr->gstk_is_curr_session_ui_related == TRUE))
      {
        gstk_util_call_client_evt_cb(i+1, message_ptr);
      }
      else if(gstk_shared_data.gstk_client_table[i].client_type != GSTK_UI_TYPE)
      {
        gstk_util_call_client_evt_cb(i+1, message_ptr);
      }/* if(gstk_shared_data.gstk_client_table[i].client_type != GSTK_UI_TYPE) */
    }/* End of for */

    gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = FALSE;

    return gstk_status;

  }/* if (message_ptr->hdr_cmd.command_id == GSTK_END_PROACTIVE_CMD_REQ) */

  /* find the client who has registered for this event */
  gstk_status = gstk_util_find_client_with_registered_event(
                  &client_id,
                  message_ptr->hdr_cmd.command_id,
                  GSTK_GSTK_FORMAT,
                  GSTK_HANDLE_ALL_FUNC);
  if (gstk_status != GSTK_SUCCESS)
  {
    /* find the client who has registered for this event (modem) */
    gstk_status = gstk_util_find_client_with_registered_event(
                    &client_id,
                    message_ptr->hdr_cmd.command_id,
                    GSTK_GSTK_FORMAT,
                    GSTK_HANDLE_MODEM_FUNC_ONLY);
    if (gstk_status != GSTK_SUCCESS)
    {
      /* When there is no registered client for the MODEM functionlity for the below
         proactive commands, GSTK should return TR with Beyond ME Capabilities. */
      if(gstk_status == GSTK_NO_CLIENT_REG_FOUND)
      {
        switch(message_ptr->hdr_cmd.command_id)
        {
          case GSTK_SEND_SMS_REQ:
          case GSTK_SEND_SS_REQ:
          case GSTK_SEND_USSD_REQ:
          case GSTK_SETUP_CALL_REQ:
          case GSTK_SEND_DTMF_REQ:
          case GSTK_OPEN_CH_REQ:
          case GSTK_CLOSE_CH_REQ:
          case GSTK_SEND_DATA_REQ:
          case GSTK_RECEIVE_DATA_REQ:
          case GSTK_GET_CH_STATUS_REQ:
            return gstk_status;
          default:
            /* No action required */
            break;
        }
      }
      /* find the client who has registered for this event (display) */
      gstk_status = gstk_util_find_client_with_registered_event(
                      &client_id,
                      message_ptr->hdr_cmd.command_id,
                      GSTK_GSTK_FORMAT,
                      GSTK_HANDLE_DISPLAY_FUNC_ONLY);
      if(gstk_status != GSTK_SUCCESS)
      {
        /* Error or no client registered for this event */
        UIM_MSG_ERR_1("Error OR no client registered for event 0x%x",
                      gstk_status);
        return gstk_status;
      } /* UI functionality client */
    }/* Modem functionality client */
  } /* All functionality client */
  if (gstk_shared_data.gstk_client_table[client_id-1].client_cb)
  { /* valid callback function */
    gstk_util_call_client_evt_cb(client_id, message_ptr);
    if(gstk_shared_data.gstk_client_table[client_id-1].client_type == GSTK_UI_TYPE)
    {
      gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = TRUE;
    }
    /* release all the memory allocation since client already copied it */
    gstk_util_release_card_cmd_memory(message_ptr);
  }

  return gstk_status;
} /* gstk_util_send_message_to_clients */

/*===========================================================================
FUNCTION gstk_util_send_tr_to_clients

DESCRIPTION
  This function sends terminal response to the GSTK clients based on which clients
  have registered to receive TR.

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
gstk_status_enum_type gstk_util_send_tr_to_clients(
    gstk_cmd_from_card_type * message_ptr )
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  gstk_client_id_type   client_id   = 0;
  uint32                cmd_ref_id  = 0;

  if(message_ptr == NULL)
  {
    UIM_MSG_ERR_0("message_p ERR:NULL");
    return GSTK_BAD_PARAM;
  }

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  message_ptr->hdr_cmd.sim_slot_id =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(cmd_ref_id)].sim_slot_id;

  /* find the client who has registered for this event (display) */
  gstk_status = gstk_util_find_client_with_registered_event(
                  &client_id,
                  message_ptr->hdr_cmd.command_id,
                  GSTK_GSTK_FORMAT,
                  GSTK_HANDLE_DISPLAY_FUNC_ONLY);
  if(gstk_status != GSTK_SUCCESS)
  {
    /* Error or no client registered for this event */
    UIM_MSG_ERR_1("Error OR no client registered for event 0x%x", gstk_status);
    return gstk_status;
  } /* UI functionality client */

  if (gstk_shared_data.gstk_client_table[client_id-1].client_cb)
  { /* valid callback function */
    gstk_util_call_client_evt_cb(client_id, message_ptr);
    if(gstk_shared_data.gstk_client_table[client_id-1].client_type == GSTK_UI_TYPE)
    {
      gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = TRUE;
    }
    /* release all the memory allocation since client already copied it */
    gstk_util_release_card_cmd_memory(message_ptr);
  }

  return gstk_status;
} /* gstk_util_send_tr_to_clients */

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
gstk_status_enum_type gstk_util_send_response_to_clients(
    gstk_cmd_from_card_type* message_p, uint32 env_ref_id )
{
  gstk_status_enum_type             gstk_status = GSTK_SUCCESS;
  gstk_client_pro_cmd_reg_data_type pro_cmd_data;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
  {
    message_p->hdr_cmd.tag = GSTK_TAG_RECOVERY;
  }

  if (env_ref_id > GSTK_MAX_PROACTIVE_COMMAND )
  {
    UIM_MSG_ERR_1("Invalid env_ref_id: 0x%x", env_ref_id);
    gstk_status = GSTK_BAD_PARAM;
  }
  else
  {
    switch (gstk_shared_data.envelope_cmd_ref_table[env_ref_id].evt_cb_type)
    {
      case GSTK_CLI_CB_WITH_GSTK_FORMAT_SUPPORT:
        if (gstk_shared_data.envelope_cmd_ref_table[env_ref_id].env_cb.client_env_cb != NULL)
        {
          (gstk_shared_data.envelope_cmd_ref_table[env_ref_id].env_cb.client_env_cb)(message_p);
        }
        break;
      case GSTK_CLI_CB_WITH_MULTI_FORMAT_SUPPORT:
        memset(&pro_cmd_data, 0, sizeof(gstk_client_pro_cmd_reg_data_type));
        pro_cmd_data.data_format_type = GSTK_GSTK_FORMAT;
        pro_cmd_data.pro_cmd_reg_data.gstk_req_rsp_ptr = message_p;
        if (gstk_shared_data.envelope_cmd_ref_table[env_ref_id].env_cb.new_client_env_cb != NULL)
        {
          (gstk_shared_data.envelope_cmd_ref_table[env_ref_id].env_cb.new_client_env_cb)(&pro_cmd_data);
        }
        break;
      default:
        UIM_MSG_ERR_1("Invalid evt_cb_type with cli 0x%x",
                      gstk_shared_data.envelope_cmd_ref_table[env_ref_id].client_id);
        break;
    } /* switch */
  } /* else */
  /* release all the memory allocation since client already copied it */
  gstk_util_release_card_cmd_memory(message_p);
  /* free the slot */
  gstk_util_env_cmd_ref_table_free_slot(env_ref_id);

  return gstk_status;
} /* gstk_util_send_response_to_clients */



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
  rex_timer_cnt_type msecs)
{
  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_ME_GSTK_CUST_ENH0, GSTK_SLOT_ID_MAX) == FALSE)
  {
    /* first clear the timer */
    (void) rex_clr_timer( &(command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].tp_rpt_timer));

    /* then start the timer */
    (void) rex_set_timer( &(command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].tp_rpt_timer),
                          msecs );
  }
} /* gstk_util_start_client_response_timer */

/*===========================================================================
FUNCTION gstk_util_release_card_cmd_memory

DESCRIPTION
  This function frees all the allocated memory for the command pointer input.

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
  The member fields in the gstk_cmd_from_card_type pointer that client passed
  in will be set to null upon the return of this function

SEE ALSO
  None
===========================================================================*/
void gstk_util_release_card_cmd_memory(
    gstk_cmd_from_card_type * message_p )
{
  int i = 0;

  if(message_p == NULL)
  {
    UIM_MSG_ERR_0("message_p ERR:NULL");
    return;
  }
  if (gstk_curr_inst_ptr->gstk_is_msg_cached == TRUE)
  {
    return;
  }

  UIM_MSG_HIGH_1("In gstk_util_release_card_cmd_memory(), command_id is 0x%x",
                 message_p->hdr_cmd.command_id);

  switch(message_p->hdr_cmd.command_id) {
  case GSTK_DISPLAY_TEXT_REQ:
    if (message_p->cmd.display_text_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.display_text_pro_cmd_req.text_string.text);
      message_p->cmd.display_text_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.display_text_pro_cmd_req.icon.present) &&
      (message_p->cmd.display_text_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.display_text_pro_cmd_req.icon.data);
      message_p->cmd.display_text_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.display_text_pro_cmd_req),
           0x00,
           sizeof(gstk_display_text_req_type));

    break;

  case GSTK_GET_INKEY_REQ:
    if (message_p->cmd.get_inkey_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.get_inkey_pro_cmd_req.text_string.text);
      message_p->cmd.get_inkey_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.get_inkey_pro_cmd_req.icon.present) &&
      (message_p->cmd.get_inkey_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.get_inkey_pro_cmd_req.icon.data);
      message_p->cmd.get_inkey_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.get_inkey_pro_cmd_req),
           0x00,
           sizeof(gstk_get_inkey_req_type));
    break;

  case GSTK_GET_INPUT_REQ:
    if (message_p->cmd.get_input_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.get_input_pro_cmd_req.text_string.text);
      message_p->cmd.get_input_pro_cmd_req.text_string.text = NULL;
    }
    if (message_p->cmd.get_input_pro_cmd_req.default_text.text != NULL) {
      gstk_free(message_p->cmd.get_input_pro_cmd_req.default_text.text);
      message_p->cmd.get_input_pro_cmd_req.default_text.text = NULL;
    }
    if ((message_p->cmd.get_input_pro_cmd_req.icon.present) &&
      (message_p->cmd.get_input_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.get_input_pro_cmd_req.icon.data);
      message_p->cmd.get_input_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.get_input_pro_cmd_req),
           0x00,
           sizeof(gstk_get_input_req_type));
    break;

  case GSTK_LAUNCH_BROWSER_REQ:
    if (message_p->cmd.launch_browser_pro_cmd_req.url.url != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.url.url);
      message_p->cmd.launch_browser_pro_cmd_req.url.url = NULL;
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list);
      message_p->cmd.launch_browser_pro_cmd_req.bearer.bearer_list = NULL;
    }
    for (i = 0; i< GSTK_MAX_PROVISIONING_FILE_REF; i++) {
      if (message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths != NULL) {
        gstk_free(message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths);
        message_p->cmd.launch_browser_pro_cmd_req.provisioning_file_ref[i].file_paths = NULL;
      }
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text);
      message_p->cmd.launch_browser_pro_cmd_req.gateway_proxy_text_string.text = NULL;
    }
    if (message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text != NULL) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text);
      message_p->cmd.launch_browser_pro_cmd_req.user_confirm_alpha.text = NULL;
    }
    if ((message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.present) &&
      (message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data != NULL)) {
      gstk_free(message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data);
      message_p->cmd.launch_browser_pro_cmd_req.user_confirm_icon.data = NULL;
    }
    memset(&(message_p->cmd.launch_browser_pro_cmd_req),
           0x00,
           sizeof(gstk_launch_browser_req_type));
    break;

  case GSTK_MORE_TIME_REQ:
    /* no dynamically allocated memory */
    break;

  case GSTK_PLAY_TONE_REQ:
    if (message_p->cmd.play_tone_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.play_tone_pro_cmd_req.alpha.text);
      message_p->cmd.play_tone_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.play_tone_pro_cmd_req.icon.present) &&
      (message_p->cmd.play_tone_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.play_tone_pro_cmd_req.icon.data);
      message_p->cmd.play_tone_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.play_tone_pro_cmd_req),
           0x00,
           sizeof(gstk_play_tone_req_type));
    break;

  case GSTK_POLL_INTERVAL_REQ:
  case GSTK_POLLING_OFF_REQ:
  case GSTK_PROVIDE_LOCAL_INFO_REQ:
    /* internal command */
    break;

  case GSTK_PROVIDE_LANG_INFO_REQ:
    break;

  case GSTK_REFRESH_REQ:
    if (message_p->cmd.refresh_pro_cmd_req.file_list.file_paths != NULL)
    {
      gstk_free(message_p->cmd.refresh_pro_cmd_req.file_list.file_paths);
      message_p->cmd.refresh_pro_cmd_req.file_list.file_paths = NULL;
    }

    if (message_p->cmd.refresh_pro_cmd_req.plmn_wact_list.data_buffer_ptr != NULL)
    {
      gstk_free(message_p->cmd.refresh_pro_cmd_req.plmn_wact_list.data_buffer_ptr);
      message_p->cmd.refresh_pro_cmd_req.plmn_wact_list.data_buffer_ptr = NULL;
    }
    if (message_p->cmd.refresh_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.refresh_pro_cmd_req.alpha.text);
      message_p->cmd.refresh_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.refresh_pro_cmd_req.icon.present) &&
      (message_p->cmd.refresh_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.refresh_pro_cmd_req.icon.data);
      message_p->cmd.refresh_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.refresh_pro_cmd_req),
           0x00,
           sizeof(gstk_refresh_req_type));
    break;

  case GSTK_RUN_AT_CMDS_REQ:
    if (message_p->cmd.run_at_cmd_pro_cmd_req.at_cmd.at_cmd != NULL) {
      gstk_free(message_p->cmd.run_at_cmd_pro_cmd_req.at_cmd.at_cmd );
      message_p->cmd.run_at_cmd_pro_cmd_req.at_cmd.at_cmd = NULL;
    }
    if (message_p->cmd.run_at_cmd_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.run_at_cmd_pro_cmd_req.alpha.text );
      message_p->cmd.run_at_cmd_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.run_at_cmd_pro_cmd_req.icon.present) &&
      (message_p->cmd.run_at_cmd_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.run_at_cmd_pro_cmd_req.icon.data);
      message_p->cmd.run_at_cmd_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.run_at_cmd_pro_cmd_req),
           0x00,
           sizeof(gstk_run_at_cmd_req_type));
    break;

  case GSTK_SELECT_ITEM_REQ:
    if (message_p->cmd.select_item_pro_cmd_req.title.text != NULL) {
      gstk_free(message_p->cmd.select_item_pro_cmd_req.title.text );
      message_p->cmd.select_item_pro_cmd_req.title.text = NULL;
    }
    for (i = 0; i< message_p->cmd.select_item_pro_cmd_req.num_items; i++) {
      if (message_p->cmd.select_item_pro_cmd_req.items[i].item_text != NULL) {
        gstk_free(message_p->cmd.select_item_pro_cmd_req.items[i].item_text );
        message_p->cmd.select_item_pro_cmd_req.items[i].item_text = NULL;
      }
    }
    if (message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list != NULL) {
      gstk_free(message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list);
      message_p->cmd.select_item_pro_cmd_req.next_action_indicator.next_action_list = NULL;
    }
    if(message_p->cmd.select_item_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< GSTK_MAX_MENU_ITEMS; i++) {
        /* array of pointer to icon type */
        if(message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] != NULL) {
           if((message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             gstk_free(
               message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data);
             message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]->data = NULL;
           }
           gstk_free(
             message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i]);
           message_p->cmd.select_item_pro_cmd_req.icon_id_list.icon_list[i] = NULL;
        }
      }
    }
    if ((message_p->cmd.select_item_pro_cmd_req.icon.present) &&
      (message_p->cmd.select_item_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.select_item_pro_cmd_req.icon.data);
      message_p->cmd.select_item_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.select_item_pro_cmd_req),
           0x00,
           sizeof(gstk_select_item_req_type));
    break;

  case GSTK_SEND_SMS_REQ:
    if (message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu != NULL) {
      gstk_free(message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu);
      message_p->cmd.send_sms_pro_cmd_req.sms_tpdu.tpdu = NULL;
    }
    if (message_p->cmd.send_sms_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_sms_pro_cmd_req.alpha.text);
      message_p->cmd.send_sms_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.send_sms_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_sms_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_sms_pro_cmd_req.icon.data);
      message_p->cmd.send_sms_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_sms_pro_cmd_req),
           0x00,
           sizeof(gstk_send_sms_req_type));
    break;

  case GSTK_SEND_SS_REQ:
    if (message_p->cmd.send_ss_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_ss_pro_cmd_req.alpha.text);
      message_p->cmd.send_ss_pro_cmd_req.alpha.text = NULL;
    }
    if ((message_p->cmd.send_ss_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_ss_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_ss_pro_cmd_req.icon.data);
      message_p->cmd.send_ss_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_ss_pro_cmd_req),
           0x00,
           sizeof(gstk_send_ss_req_type));
    break;

  case GSTK_SEND_USSD_REQ:
    if (message_p->cmd.send_ussd_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_ussd_pro_cmd_req.alpha.text);
      message_p->cmd.send_ussd_pro_cmd_req.alpha.text = NULL;
    }
    if (message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text != NULL) {
      gstk_free(message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text);
      message_p->cmd.send_ussd_pro_cmd_req.ussd_string.text = NULL;
    }
    if ((message_p->cmd.send_ussd_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_ussd_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_ussd_pro_cmd_req.icon.data);
      message_p->cmd.send_ussd_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_ussd_pro_cmd_req),
           0x00,
           sizeof(gstk_send_ussd_req_type));
    break;

  case GSTK_SETUP_CALL_REQ:
    if (message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text != NULL) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text);
      message_p->cmd.setup_call_pro_cmd_req.user_cnf_alpha.text = NULL;
    }
    if (message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text != NULL) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text);
      message_p->cmd.setup_call_pro_cmd_req.setup_call_display_alpha.text = NULL;
    }
    if ((message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.present) &&
      (message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.data);
      message_p->cmd.setup_call_pro_cmd_req.user_cnf_icon.data = NULL;
    }
    if ((message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.present) &&
      (message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data);
      message_p->cmd.setup_call_pro_cmd_req.setup_call_display_icon.data = NULL;
    }
    memset(&(message_p->cmd.setup_call_pro_cmd_req),
           0x00,
           sizeof(gstk_setup_call_req_type));
    break;

  case GSTK_SETUP_EVENT_LIST_REQ:
    /* internal */
    break;

  case GSTK_SETUP_MENU_REQ:
    if (message_p->cmd.setup_menu_pro_cmd_req.title.text != NULL) {
      gstk_free(message_p->cmd.setup_menu_pro_cmd_req.title.text );
      message_p->cmd.setup_menu_pro_cmd_req.title.text = NULL;
    }
    for (i = 0; i< message_p->cmd.setup_menu_pro_cmd_req.num_items; i++) {
      if (message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text != NULL) {
        gstk_free(message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text );
        message_p->cmd.setup_menu_pro_cmd_req.items[i].item_text = NULL;
      }
    }
    if (message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list != NULL) {
      gstk_free(message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list);
      message_p->cmd.setup_menu_pro_cmd_req.next_action_indicator.next_action_list = NULL;
    }
    if(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items > 0)
    {
      for(i = 0; i< message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.num_items; i++) {
        /* array of pointer to icon type */
        if(message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] != NULL) {
           if((message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->present) &&
              (message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data != NULL)) {
             gstk_free(
               message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data);
             message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]->data = NULL;
           }
           gstk_free(
             message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i]);
           message_p->cmd.setup_menu_pro_cmd_req.icon_id_list.icon_list[i] = NULL;
         }
      }
    }
    if ((message_p->cmd.setup_menu_pro_cmd_req.icon.present) &&
      (message_p->cmd.setup_menu_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_menu_pro_cmd_req.icon.data);
      message_p->cmd.setup_menu_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.setup_menu_pro_cmd_req),
           0x00,
           sizeof(gstk_setup_menu_req_type));
    break;

  case GSTK_SETUP_IDLE_TEXT_REQ:
    if (message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text != NULL) {
      gstk_free(message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text);
      message_p->cmd.setup_idle_mode_txt_pro_cmd_req.text_string.text = NULL;
    }
    if ((message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.present) &&
      (message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data);
      message_p->cmd.setup_idle_mode_txt_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.setup_idle_mode_txt_pro_cmd_req),
           0x00,
           sizeof(gstk_setup_idle_mode_text_req_type));
    break;

  case GSTK_SEND_DTMF_REQ:
    if (message_p->cmd.send_dtmf_pro_cmd_req.alpha.text != NULL) {
      gstk_free(message_p->cmd.send_dtmf_pro_cmd_req.alpha.text);
      message_p->cmd.send_dtmf_pro_cmd_req.alpha.text = NULL;
    }
    if (message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf != NULL) {
      gstk_free(message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf);
      message_p->cmd.send_dtmf_pro_cmd_req.dtmf.dtmf = NULL;
    }
    if ((message_p->cmd.send_dtmf_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_dtmf_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_dtmf_pro_cmd_req.icon.data);
      message_p->cmd.send_dtmf_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_dtmf_pro_cmd_req),
           0x00,
           sizeof(gstk_send_dtmf_req_type));
    break;


  case GSTK_OPEN_CH_REQ:
    if (message_p->cmd.open_ch_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.alpha.alphabet);
      message_p->cmd.open_ch_pro_cmd_req.alpha.alphabet = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.access_point_name.alphabet != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.access_point_name.alphabet);
      message_p->cmd.open_ch_pro_cmd_req.access_point_name.alphabet = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address);
      message_p->cmd.open_ch_pro_cmd_req.local_address.pkt_address.address = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address);
      message_p->cmd.open_ch_pro_cmd_req.data_dest_address.pkt_address.address = NULL;
    }
    if ((message_p->cmd.open_ch_pro_cmd_req.icon.present) &&
      (message_p->cmd.open_ch_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.icon.data);
      message_p->cmd.open_ch_pro_cmd_req.icon.data = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet);
      message_p->cmd.open_ch_pro_cmd_req.text_string_user_login.alphabet = NULL;
    }
    if (message_p->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet != NULL) {
      gstk_free(message_p->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet);
      message_p->cmd.open_ch_pro_cmd_req.text_string_user_passwd.alphabet = NULL;
    }
    memset(&(message_p->cmd.open_ch_pro_cmd_req),
           0x00,
           sizeof(gstk_open_ch_req_type));
    break;

  case GSTK_CLOSE_CH_REQ:
    if (message_p->cmd.close_ch_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.close_ch_pro_cmd_req.alpha.alphabet);
      message_p->cmd.close_ch_pro_cmd_req.alpha.alphabet = NULL;
    }
    if ((message_p->cmd.close_ch_pro_cmd_req.icon.present) &&
      (message_p->cmd.close_ch_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.close_ch_pro_cmd_req.icon.data);
      message_p->cmd.close_ch_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.close_ch_pro_cmd_req),
           0x00,
           sizeof(gstk_close_ch_req_type));
    break;

  case GSTK_SEND_DATA_REQ:
    if (message_p->cmd.send_data_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.send_data_pro_cmd_req.alpha.alphabet);
      message_p->cmd.send_data_pro_cmd_req.alpha.alphabet = NULL;
    }
    if (message_p->cmd.send_data_pro_cmd_req.ch_data.data != NULL) {
      gstk_free(message_p->cmd.send_data_pro_cmd_req.ch_data.data);
      message_p->cmd.send_data_pro_cmd_req.ch_data.data = NULL;
    }
    if ((message_p->cmd.send_data_pro_cmd_req.icon.present) &&
      (message_p->cmd.send_data_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.send_data_pro_cmd_req.icon.data);
      message_p->cmd.send_data_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.send_data_pro_cmd_req),
           0x00,
           sizeof(gstk_send_data_req_type));
    break;

  case GSTK_RECEIVE_DATA_REQ:
    if (message_p->cmd.receive_data_pro_cmd_req.alpha.alphabet != NULL) {
      gstk_free(message_p->cmd.receive_data_pro_cmd_req.alpha.alphabet);
      message_p->cmd.receive_data_pro_cmd_req.alpha.alphabet = NULL;
    }
    if ((message_p->cmd.receive_data_pro_cmd_req.icon.present) &&
      (message_p->cmd.receive_data_pro_cmd_req.icon.data != NULL)) {
      gstk_free(message_p->cmd.receive_data_pro_cmd_req.icon.data);
      message_p->cmd.receive_data_pro_cmd_req.icon.data = NULL;
    }
    memset(&(message_p->cmd.receive_data_pro_cmd_req),
           0x00,
           sizeof(gstk_receive_data_req_type));
    break;


  /* Envelope Indication Responses */
  case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
    if (message_p->cmd.sms_pp_download_rsp.response_data_present) {
      if (message_p->cmd.sms_pp_download_rsp.ack.ack != NULL) {
        gstk_free(message_p->cmd.sms_pp_download_rsp.ack.ack);
        message_p->cmd.sms_pp_download_rsp.ack.ack = NULL;
      }
    }
    break;
  case GSTK_RAW_ENVELOPE_RSP:
    if (message_p->cmd.raw_envelope_rsp.rsp_data_ptr)
    {
      gstk_free(message_p->cmd.raw_envelope_rsp.rsp_data_ptr);
      message_p->cmd.raw_envelope_rsp.rsp_data_ptr = NULL;
    }
    break;
  case GSTK_MO_SMS_CTRL_IND_RSP:
    if (message_p->cmd.mo_sms_cc_rsp.alpha.text != NULL) {
      gstk_free(message_p->cmd.mo_sms_cc_rsp.alpha.text);
      message_p->cmd.mo_sms_cc_rsp.alpha.text = NULL;
    }
    break;

  case GSTK_CC_IND_RSP:
    if (message_p->cmd.cc_rsp.cc_req_action.call_type == GSTK_USSD) {
      if (message_p->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
        gstk_free(message_p->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text);
        message_p->cmd.cc_rsp.cc_req_action.gstk_address_string.ussd_string.text = NULL;
      }
    }
    if (message_p->cmd.cc_rsp.cc_req_action.alpha.text != NULL) {
      gstk_free(message_p->cmd.cc_rsp.cc_req_action.alpha.text);
      message_p->cmd.cc_rsp.cc_req_action.alpha.text = NULL;
    }
    break;

  case GSTK_MT_CALL_EVT_IND_RSP:
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
  case GSTK_MENU_IND_RSP:
  case GSTK_TIMER_EXPIRE_IND_RSP:
    /* nothing to free */
    break;

  default:
    /* do nothing */
    break;
  }

} /* gstk_util_release_card_cmd_memory*/


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
void gstk_util_release_upper_layer_cmd_memory(
    gstk_cmd_type * message_p )
{
    uint32                        type_of_command = 0;
    gstk_additional_info_ptr_type *addi_info_ptr  = NULL;
    uint32                        i = 0;

   UIM_MSG_HIGH_0("gstk_util_release_upper_layer_cmd_memory");

    if(message_p == NULL)
    {
      UIM_MSG_ERR_0("message_p ERR:NULL");
      return;
    }

    /* Get the type of command */
    type_of_command = message_p->general_cmd.message_header.command_id;

    switch( type_of_command ) {

    case GSTK_SETUP_CALL_CNF:
      if (message_p->setup_call_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_call_term_rsp_cnf.result_additional_info;
      }
      /* all the mem alloc should be due to cc type change */
      if (message_p->setup_call_term_rsp_cnf.extra_param.has_cc_type_modification) {
        if (gstk_shared_data.cc_req_action.alpha.text != NULL) {
          gstk_free(gstk_shared_data.cc_req_action.alpha.text);
          gstk_shared_data.cc_req_action.alpha.text = NULL;
        }
        /* ussd_string and text_string2 are only valid when the calltype is changed to GSTK_USSD */
        if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD) {
          if (gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
            gstk_free(gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text);
            gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text = NULL;
          }
          if (message_p->setup_call_term_rsp_cnf.extra_param.text_string2.text != NULL) {
            gstk_free(message_p->setup_call_term_rsp_cnf.extra_param.text_string2.text);
            message_p->setup_call_term_rsp_cnf.extra_param.text_string2.text = NULL;
          }
        }
        if (message_p->setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
          gstk_free(message_p->setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
          message_p->setup_call_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
        }
        memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
      }
      break;

    case GSTK_SEND_SS_CNF:
      if (message_p->send_ss_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_call_term_rsp_cnf.result_additional_info;
      }
      /* all the mem alloc should be due to cc type change */
      if (message_p->send_ss_term_rsp_cnf.extra_param.has_cc_type_modification) {
        if (gstk_shared_data.cc_req_action.alpha.text != NULL) {
          gstk_free(gstk_shared_data.cc_req_action.alpha.text);
          gstk_shared_data.cc_req_action.alpha.text = NULL;
        }
        /* ussd_string and text_string2 are only valid when the calltype is changed to GSTK_USSD */
        if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD) {
          if (gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
            gstk_free(gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text);
            gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text = NULL;
          }
          if (message_p->send_ss_term_rsp_cnf.extra_param.text_string2.text != NULL) {
            gstk_free(message_p->send_ss_term_rsp_cnf.extra_param.text_string2.text);
            message_p->send_ss_term_rsp_cnf.extra_param.text_string2.text = NULL;
          }
        }
        if (message_p->send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
          gstk_free(message_p->send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr);
          message_p->send_ss_term_rsp_cnf.extra_param.additional_info2_ptr.additional_info_ptr = NULL;
        }
        memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
      }
      break;

    case GSTK_SEND_USSD_CNF:
      if (message_p->send_ussd_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_ussd_term_rsp_cnf.result_additional_info;
      }
      /* text is not dependent on whether there is a cc type change or not */
      if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.data.text != NULL) {
        gstk_free(message_p->send_ussd_term_rsp_cnf.ussd_extra_param.data.text);
        message_p->send_ussd_term_rsp_cnf.ussd_extra_param.data.text = NULL;
      }
      /* the rest are dependent on whether there is a cc type change or not */
      if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.has_cc_type_modification) {
        if (gstk_shared_data.cc_req_action.alpha.text != NULL) {
          gstk_free(gstk_shared_data.cc_req_action.alpha.text);
          gstk_shared_data.cc_req_action.alpha.text = NULL;
        }
        if (gstk_shared_data.cc_req_action.call_type == GSTK_USSD) {
          if (gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text != NULL) {
            gstk_free(gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text);
            gstk_shared_data.cc_req_action.gstk_address_string.ussd_string.text = NULL;
          }
        }
        if (message_p->send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr != NULL) {
          gstk_free(message_p->send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr);
          message_p->send_ussd_term_rsp_cnf.ussd_extra_param.additional_info2_ptr.additional_info_ptr = NULL;
        }
        memset(&gstk_shared_data.cc_req_action, 0x00, sizeof(gstk_cc_req_action_type));
      }
      break;

    case GSTK_GET_INPUT_CNF:
      if (message_p->get_input_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->get_input_term_rsp_cnf.result_additional_info;
      }
      if (message_p->get_input_term_rsp_cnf.data != NULL) {
        gstk_free(message_p->get_input_term_rsp_cnf.data);
        message_p->get_input_term_rsp_cnf.data = NULL;
      }
      break;

    case GSTK_PROVIDE_LOCAL_INFO_CNF:
    case GSTK_PROVIDE_LANG_INFO_CNF:
      if (message_p->provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->provide_local_info_term_rsp_cnf.result_additional_info;
      }
      if(message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_type == GSTK_NMR_UTRAN_INFO) {
        if(message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr){
          gstk_free(message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr);
          message_p->provide_local_info_term_rsp_cnf.local_info_rsp.info_data.nmr_utran.nmr_ptr = NULL;
        }
      }
      break;

    case GSTK_SEND_SMS_CNF:
      if (message_p->send_sms_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_sms_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_LAUNCH_BROWSER_CNF:
      if (message_p->launch_browser_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->launch_browser_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_PLAY_TONE_CNF:
      if (message_p->play_tone_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->play_tone_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_DISPLAY_TEXT_CNF:
      if (message_p->display_text_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->display_text_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_GET_INKEY_CNF:
      if (message_p->get_inkey_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->get_inkey_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SELECT_ITEM_CNF:
      if (message_p->select_item_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->select_item_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SETUP_MENU_CNF:
      if (message_p->setup_menu_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_menu_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SETUP_IDLE_TEXT_CNF:
      if (message_p->setup_idle_text_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_idle_text_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_REFRESH_CNF:
      if (message_p->refresh_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->refresh_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_MORE_TIME_CNF:
      if (message_p->more_time_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->more_time_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_POLL_INTERVAL_CNF:
    case GSTK_POLLING_OFF_CNF:
      if (message_p->poll_interval_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->poll_interval_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SETUP_EVENT_LIST_CNF:
      if (message_p->setup_evt_list_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->setup_evt_list_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SEND_DTMF_CNF:
      if (message_p->send_dtmf_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_dtmf_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_LANG_NOTIFICATION_CNF:
      if (message_p->lang_notification_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->lang_notification_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_TIMER_MANAGEMENT_CNF:
      if (message_p->timer_manage_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->timer_manage_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_OPEN_CH_CNF:
      if (message_p->open_ch_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->open_ch_term_rsp_cnf.result_additional_info;
      }
      if (message_p->open_ch_term_rsp_cnf.dns_addrs.addresses != NULL)
      {
        for (i = 0; i < message_p->open_ch_term_rsp_cnf.dns_addrs.num_addresses; i++)
        {
          if (message_p->open_ch_term_rsp_cnf.dns_addrs.addresses[i].address)
          {
            gstk_free(message_p->open_ch_term_rsp_cnf.dns_addrs.addresses[i].address);
            message_p->open_ch_term_rsp_cnf.dns_addrs.addresses[i].address = NULL;
          }
        }
        gstk_free(message_p->open_ch_term_rsp_cnf.dns_addrs.addresses);
        message_p->open_ch_term_rsp_cnf.dns_addrs.addresses = NULL;
      }
      break;

    case GSTK_CLOSE_CH_CNF:
      if (message_p->close_ch_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->close_ch_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_SEND_DATA_CNF:
      if (message_p->send_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->send_data_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_RECEIVE_DATA_CNF:
      if (message_p->receive_data_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->receive_data_term_rsp_cnf.result_additional_info;
      }
      if (message_p->receive_data_term_rsp_cnf.ch_data.data != NULL) {
        gstk_free(message_p->receive_data_term_rsp_cnf.ch_data.data);
        message_p->receive_data_term_rsp_cnf.ch_data.data = NULL;
      }
      break;

    case GSTK_GET_CH_STATUS_CNF:
      if (message_p->get_ch_status_term_rsp_cnf.result_additional_info.additional_info_ptr != NULL) {
        addi_info_ptr = &message_p->get_ch_status_term_rsp_cnf.result_additional_info;
      }
      break;

    case GSTK_RUN_AT_CMDS_CNF: /* later */
    case GSTK_UNSUPPORTED_UNKNOWN_CMD_CNF:
      /* nothing to free */
        break;

        /* Envelope command */
    case GSTK_MENU_SELECTION_IND:
    case GSTK_IDLE_SCRN_EVT_IND:
    case GSTK_USER_ACTIVITY_EVT_IND:
    case GSTK_BROWSER_TERMINATION_EVT_IND:
    case GSTK_LANG_SELECTION_EVT_IND:
    case GSTK_TIMER_EXPIRE_IND:
    case GSTK_MO_SMS_CTRL_IND:
      break;
    case GSTK_RAW_ENVELOPE_IND:
      if (message_p->raw_envelope_ind.env_data_ptr != NULL)
      {
        gstk_free(message_p->raw_envelope_ind.env_data_ptr);
        message_p->raw_envelope_ind.env_data_ptr = NULL;
      }
      break;
    case GSTK_SMS_PP_DOWNLOAD_IND:
      if (message_p->sms_pp_envelope_ind.tpdu.tpdu != NULL) {
        gstk_free(message_p->sms_pp_envelope_ind.tpdu.tpdu);
        message_p->sms_pp_envelope_ind.tpdu.tpdu = NULL;
      }
      break;
    case GSTK_MT_CALL_EVT_IND:
      if (message_p->mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list != NULL) {
        gstk_free(message_p->mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list);
        message_p->mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list = NULL;
      }
      break;
    case GSTK_CALL_CONNECTED_EVT_IND:
      if (message_p->call_connected_envelope_ind.transaction_id.transaction_list != NULL) {
        gstk_free(message_p->call_connected_envelope_ind.transaction_id.transaction_list);
        message_p->call_connected_envelope_ind.transaction_id.transaction_list = NULL;
      }
      break;
    case GSTK_CALL_DISCONNECTED_EVT_IND:
      if (message_p->call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list != NULL) {
        gstk_free(message_p->call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list);
        message_p->call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list = NULL;
      }
      if (message_p->call_disconnected_envelope_ind.call_disconnected_data.cause.cause != NULL) {
        gstk_free(message_p->call_disconnected_envelope_ind.call_disconnected_data.cause.cause);
        message_p->call_disconnected_envelope_ind.call_disconnected_data.cause.cause = NULL;
      }
      break;

    case GSTK_LOCATION_STATUS_EVT_IND: /* more codeeeeeee, based on MM/RR/RRC interface */
      break;

    case GSTK_IMS_REG_EVT_IND:
      if (message_p->ims_reg_evt_envelope_ind.ims_reg.impu_list != NULL) {
        gstk_free(message_p->ims_reg_evt_envelope_ind.ims_reg.impu_list);
        message_p->ims_reg_evt_envelope_ind.ims_reg.impu_list = NULL;
      }
      if (message_p->ims_reg_evt_envelope_ind.ims_reg.status_code != NULL) {
        gstk_free(message_p->ims_reg_evt_envelope_ind.ims_reg.status_code);
        message_p->ims_reg_evt_envelope_ind.ims_reg.status_code = NULL;
      }
      break;
    case GSTK_CC_IND:
      if (message_p->cc_envelope_ind.cc_data.call_enum_type == GSTK_USSD) {
        if (message_p->cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text != NULL) {
          gstk_free(message_p->cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text);
          message_p->cc_envelope_ind.cc_data.gstk_address_string.ussd_string.text = NULL;
        }
      }
      break;
    case GSTK_CELL_BROADCAST_DOWNLOAD_IND:
      if (message_p->cell_broadcast_envelope_ind.cb_page.cb_page_data != NULL) {
        gstk_free(message_p->cell_broadcast_envelope_ind.cb_page.cb_page_data);
        message_p->cell_broadcast_envelope_ind.cb_page.cb_page_data = NULL;
      }
      break;
    case GSTK_RAW_TERMIMAL_RESPONSE:
      if (message_p->client_raw_term_resp.raw_term_resp.data_buffer_ptr != NULL) {
        gstk_free(message_p->client_raw_term_resp.raw_term_resp.data_buffer_ptr);
        message_p->client_raw_term_resp.raw_term_resp.data_buffer_ptr = NULL;
      }
      break;
    default:
      break;
    }

    if (addi_info_ptr != NULL)
    {
      gstk_free(addi_info_ptr->additional_info_ptr);
      addi_info_ptr->additional_info_ptr = NULL;
      addi_info_ptr->length = 0;
    }
} /* gstk_util_release_upper_layer_cmd_memory*/

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
uint8 gstk_util_translate_qualifier_to_dcs(
   uint8                     qualifier )
{
  uint8 dcs;

  if ((qualifier & 0x02) == 0x02) {
     /* UCS2 */
     dcs = 0x08;
  }
  else {
     if (qualifier & 0x08) {
       /* packed sms */
       dcs = 0x00;
     }
     else {
       /* unpacked sms */
       dcs = 0x04;
     }
  }
  return dcs;
} /* gstk_util_translate_qualifier_to_dcs */


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
gstk_status_enum_type gstk_util_build_evt_dl_ind(
  gstk_task_cmd_type          * cmd,
  uint32                        env_ref_id,
  const gstk_evt_dl_ind_type  * evt_list)
{
  gstk_slot_id_enum_type slot      = GSTK_SLOT_ID_MAX;
  gstk_priv_data_type    *inst_ptr = NULL;

  UIM_MSG_HIGH_0("In build evt dl indication ");

  if (cmd == NULL || evt_list == NULL)
  {
    UIM_MSG_ERR_2("null ptr: 0x%x, 0x%x!", cmd, evt_list);
    return GSTK_NULL_INPUT_PARAM;
  }

  slot = cmd->cmd.general_cmd.message_header.sim_slot_id;
  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_BAD_PARAM;
  }

  inst_ptr = gstk_instances_ptr[(uint32)slot - 1];

  switch(evt_list->evt_type)
  {
  case GSTK_IDLE_SCRN_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_IDLE_SCRN_AVAILABLE] & 0x01) == 0x01) {
      cmd->cmd.idle_scrn_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.idle_scrn_evt_envelope_ind.message_header, GSTK_IDLE_SCRN_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.idle_scrn_evt_envelope_ind.message_header.user_data = env_ref_id;
    }
    else {
      UIM_MSG_ERR_0("Idle Screen Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_LOCATION_STATUS_EVT_IND:
    /* internal */
    cmd->cmd.location_st_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
    gstk_set_command_id(&cmd->cmd.location_st_envelope_ind.message_header, GSTK_LOCATION_STATUS_EVT_IND);
    /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
    cmd->cmd.location_st_envelope_ind.message_header.user_data = env_ref_id;
    break;

  case GSTK_USER_ACTIVITY_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_USER_ACTIVITY] & 0x01) == 0x01) {
      cmd->cmd.user_act_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.user_act_evt_envelope_ind.message_header, GSTK_USER_ACTIVITY_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.user_act_evt_envelope_ind.message_header.user_data = env_ref_id;
    }
    else {
      UIM_MSG_ERR_0("User Activity Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_LANG_SELECTION_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_LANGUAGE_SELECTION] & 0x01) == 0x01) {
      cmd->cmd.lang_sel_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.lang_sel_evt_envelope_ind.message_header, GSTK_LANG_SELECTION_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.lang_sel_evt_envelope_ind.message_header.user_data = env_ref_id;
      (void)memscpy(cmd->cmd.lang_sel_evt_envelope_ind.lang_code.lang_code,
      	    sizeof(cmd->cmd.lang_sel_evt_envelope_ind.lang_code.lang_code),
            evt_list->evt_ind.lang_sel_evt_ind.lang_code,
            2);
    }
    else {
      UIM_MSG_ERR_0("Language Sel Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_BROWSER_TERMINATION_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_BROWSER_TERMINATION] & 0x01) == 0x01) {
      if ((cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause != GSTK_BROWSER_USER_TERMINATED) &&
          (cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause != GSTK_BROWSER_ERROR_TERMINATED)) {
          UIM_MSG_ERR_1("Invalid Browser Term Cause 0x%x",
                        cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause);
          return GSTK_ERROR;
      }
      cmd->cmd.browser_term_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.browser_term_evt_envelope_ind.message_header, GSTK_BROWSER_TERMINATION_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.browser_term_evt_envelope_ind.message_header.user_data = env_ref_id;
      cmd->cmd.browser_term_evt_envelope_ind.browser_termination_cause =
        evt_list->evt_ind.browser_terminate_evt_ind;
    }
    else {
      UIM_MSG_ERR_0("Browser Term Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_DATA_AVAIL_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_DATA_AVAILABLE] & 0x01) == 0x01) {
      cmd->cmd.data_avail_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.data_avail_evt_envelope_ind.message_header, GSTK_DATA_AVAIL_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.data_avail_evt_envelope_ind.message_header.user_data = env_ref_id;
      /* Channel Status Info parameter checks */
      if((evt_list->evt_ind.data_avail_evt_ind.ch_status.ch_id < 0x01) ||
         (evt_list->evt_ind.data_avail_evt_ind.ch_status.ch_id > 0x07))
      {
        UIM_MSG_ERR_0("Invalid Channel Id");
        return GSTK_ERROR;
      }
      (void)memscpy(&(cmd->cmd.data_avail_evt_envelope_ind.ch_status_info),
      	sizeof(gstk_ch_status_type),
        &(evt_list->evt_ind.data_avail_evt_ind.ch_status),
        sizeof(gstk_ch_status_type));
      cmd->cmd.data_avail_evt_envelope_ind.ch_data_len =
        evt_list->evt_ind.data_avail_evt_ind.ch_len;
    }
    else {
      UIM_MSG_ERR_0("Data Available Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_CH_STATUS_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_CHANNEL_STATUS] & 0x01) == 0x01) {
      cmd->cmd.ch_status_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.ch_status_evt_envelope_ind.message_header, GSTK_CH_STATUS_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.ch_status_evt_envelope_ind.message_header.user_data = env_ref_id;
      /* Channel Status Info parameter checks */
      if((evt_list->evt_ind.ch_status_evt_ind.ch_id < 0x01) ||
         (evt_list->evt_ind.ch_status_evt_ind.ch_id  > 0x07))
      {
        UIM_MSG_ERR_0("Invalid Channel Id");
        return GSTK_ERROR;
      }
      (void)memscpy(&(cmd->cmd.ch_status_evt_envelope_ind.ch_status_info),
      	sizeof(gstk_ch_status_type),
        &(evt_list->evt_ind.ch_status_evt_ind),
        sizeof(gstk_ch_status_type));
    }
    else {
      UIM_MSG_ERR_0("Channel Status Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_HCI_CONNECTIVITY_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_HCI_CONNECTIVITY] & 0x01) == 0x01) {
      cmd->cmd.idle_scrn_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.idle_scrn_evt_envelope_ind.message_header, GSTK_HCI_CONNECTIVITY_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.idle_scrn_evt_envelope_ind.message_header.user_data = env_ref_id;
    }
    else {
      UIM_MSG_ERR_0("HCI Connectivity Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_MT_CALL_EVT_IND:
    if((inst_ptr->gstk_evt_list[GSTK_MT_CALL] & 0x01) == 0x01) {
      cmd->cmd.ch_status_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.data_avail_evt_envelope_ind.message_header, GSTK_MT_CALL_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.mt_evt_envelope_ind.message_header.user_data = env_ref_id;
      /* address */
      cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.TON =
        evt_list->evt_ind.mt_call_evt_ind.address.TON;
      cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.NPI =
        evt_list->evt_ind.mt_call_evt_ind.address.NPI;
      cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.length =
        evt_list->evt_ind.mt_call_evt_ind.address.length;
      UIM_MSG_HIGH_3("Address: TON: 0x%x NPI: 0x%x len: 0x%x",
                     evt_list->evt_ind.mt_call_evt_ind.address.TON,
                     evt_list->evt_ind.mt_call_evt_ind.address.NPI,
                     evt_list->evt_ind.mt_call_evt_ind.address.length);
      if(evt_list->evt_ind.mt_call_evt_ind.address.length > 0) {
        if(gstk_memcpy(cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.address,
                       evt_list->evt_ind.mt_call_evt_ind.address.address,
                       cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.length,
                       GSTK_MAX_ADDRESS_LEN,
                       GSTK_MAX_ADDRESS_LEN) <
           cmd->cmd.mt_evt_envelope_ind.mt_evt_data.address.length)
        {
          return GSTK_MEMORY_ERROR;
        }
      }
      /* subaddress */
      cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.length =
        evt_list->evt_ind.mt_call_evt_ind.subaddress.length;
      UIM_MSG_HIGH_1("Subaddress: len 0x%x",
                     evt_list->evt_ind.mt_call_evt_ind.subaddress.length);
      if(evt_list->evt_ind.mt_call_evt_ind.subaddress.length > 0) {
        if(gstk_memcpy(cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.subaddress,
                       evt_list->evt_ind.mt_call_evt_ind.subaddress.subaddress,
                       cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.length,
                       GSTK_MAX_SUBADDR_LEN,
                       GSTK_MAX_SUBADDR_LEN) <
           cmd->cmd.mt_evt_envelope_ind.mt_evt_data.subaddress.length)
        {
          return GSTK_MEMORY_ERROR;
        }
      }
      /* transaction id */
      cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.length =
        evt_list->evt_ind.mt_call_evt_ind.transaction_id.length;
      UIM_MSG_HIGH_2("Transaction ID: len 0x%x, ptr %p",
                     evt_list->evt_ind.mt_call_evt_ind.transaction_id.length,
                     evt_list->evt_ind.mt_call_evt_ind.transaction_id.transaction_list);
      if(evt_list->evt_ind.mt_call_evt_ind.transaction_id.length > 0 &&
            evt_list->evt_ind.mt_call_evt_ind.transaction_id.transaction_list != NULL) {
        cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list =
          GSTK_CALLOC(evt_list->evt_ind.mt_call_evt_ind.transaction_id.length);
        if(cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list != NULL)
        {
          (void)memscpy(cmd->cmd.mt_evt_envelope_ind.mt_evt_data.transaction_id.transaction_list,
                   evt_list->evt_ind.mt_call_evt_ind.transaction_id.length,
                   evt_list->evt_ind.mt_call_evt_ind.transaction_id.transaction_list,
                   evt_list->evt_ind.mt_call_evt_ind.transaction_id.length);
        }
        else{
          return GSTK_ERROR;
        }
      }
    }
    else {
      UIM_MSG_ERR_0("MT Call evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_CALL_CONNECTED_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_CALL_CONNECTED] & 0x01) == 0x01) {
      cmd->cmd.ch_status_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.data_avail_evt_envelope_ind.message_header, GSTK_CALL_CONNECTED_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.call_connected_envelope_ind.message_header.user_data = env_ref_id;
      /* transaction id */
      cmd->cmd.call_connected_envelope_ind.transaction_id.length = 1;
      cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list =
        GSTK_CALLOC(cmd->cmd.call_connected_envelope_ind.transaction_id.length);
      if (evt_list->evt_ind.call_connected_evt_ind.transaction_id.transaction_list != NULL &&
            cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list != NULL)
      {
        (void)memscpy(cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list,
                 cmd->cmd.call_connected_envelope_ind.transaction_id.length,
                 evt_list->evt_ind.call_connected_evt_ind.transaction_id.transaction_list,
                 cmd->cmd.call_connected_envelope_ind.transaction_id.length);
      }
      else {
        UIM_MSG_HIGH_2("Null ptr: dest %p src %p",
                       cmd->cmd.call_connected_envelope_ind.transaction_id.transaction_list,
                       evt_list->evt_ind.call_connected_evt_ind.transaction_id.transaction_list);
        return GSTK_ERROR;
      }
      /* MO call or MT call */
      cmd->cmd.call_connected_envelope_ind.near_end =
        evt_list->evt_ind.call_connected_evt_ind.near_end;
      UIM_MSG_HIGH_1("near_end",
                     evt_list->evt_ind.call_connected_evt_ind.near_end);
    }
    else {
      UIM_MSG_ERR_0("Call connected evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_CALL_DISCONNECTED_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_CALL_DISCONNECTED] & 0x01) == 0x01) {
      cmd->cmd.ch_status_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.data_avail_evt_envelope_ind.message_header, GSTK_CALL_DISCONNECTED_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.call_disconnected_envelope_ind.message_header.user_data = env_ref_id;
      /* Transaction ID*/
      UIM_MSG_HIGH_2("Transaction ID: len: 0x%x ptr %p",
                     evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.length,
                     evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.transaction_list);
      if(evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.length > 0 &&
           evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.transaction_list != NULL) {
        cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.length =
          evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.length;
        cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list =
          GSTK_CALLOC(evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.length);
        if(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list != NULL) {
          (void)memscpy(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list,
                   evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.length,
                   evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.transaction_list,
                   evt_list->evt_ind.call_disconnected_evt_ind.transaction_id.length);
        }
        else {
          return GSTK_ERROR;
        }
      }
      /*  Cause */
      UIM_MSG_HIGH_3("Cause: present 0x%x, len: 0x%x ptr %p",
                     evt_list->evt_ind.call_disconnected_evt_ind.cause.present,
                     evt_list->evt_ind.call_disconnected_evt_ind.cause.length,
                     evt_list->evt_ind.call_disconnected_evt_ind.cause.cause);
      if(evt_list->evt_ind.call_disconnected_evt_ind.cause.present &&
            evt_list->evt_ind.call_disconnected_evt_ind.cause.cause != NULL) {
        cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.present =
          evt_list->evt_ind.call_disconnected_evt_ind.cause.present;
        cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length =
          evt_list->evt_ind.call_disconnected_evt_ind.cause.length;
        cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.cause =
          GSTK_CALLOC(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length);
        if(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.cause != NULL)
        {
          (void)memscpy(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.cause,
                 cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length,
                 evt_list->evt_ind.call_disconnected_evt_ind.cause.cause,
                 cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.cause.length);
        }
        else {
          if(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list != NULL)
          {
            gstk_free(cmd->cmd.call_disconnected_envelope_ind.call_disconnected_data.transaction_id.transaction_list);
          }
          return GSTK_ERROR;
        }
      }
      /* Near end/Far end*/
      cmd->cmd.call_disconnected_envelope_ind.near_end =
        evt_list->evt_ind.call_disconnected_evt_ind.near_end;
      UIM_MSG_HIGH_1("near_end 0x%x",
                     evt_list->evt_ind.call_disconnected_evt_ind.near_end);
    }
    else {
      UIM_MSG_ERR_0("Call disconnected evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  case GSTK_IMS_REG_EVT_IND:
    if ((inst_ptr->gstk_evt_list[GSTK_IMS_REGISTRATION] & 0x01) == 0x01) {
      cmd->cmd.ims_reg_evt_envelope_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
      gstk_set_command_id(&cmd->cmd.ims_reg_evt_envelope_ind.message_header, GSTK_IMS_REG_EVT_IND);
      /* already save the user data in the gstk_shared_data.envelope_cmd_ref_table, set the user_data as the env_ref_id */
      cmd->cmd.ims_reg_evt_envelope_ind.message_header.user_data = env_ref_id;

      /* IMPU List*/
      gstk_util_dump_byte_array(
        "IMPU List",
        evt_list->evt_ind.ims_reg_evt_ind.impu_list,
        evt_list->evt_ind.ims_reg_evt_ind.impu_list_len);
      if(evt_list->evt_ind.ims_reg_evt_ind.impu_list_len > 0 &&
           evt_list->evt_ind.ims_reg_evt_ind.impu_list != NULL) {
        cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.impu_list_len =
          evt_list->evt_ind.ims_reg_evt_ind.impu_list_len;
        cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.impu_list =
          GSTK_CALLOC(evt_list->evt_ind.ims_reg_evt_ind.impu_list_len);
        if(cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.impu_list != NULL) {
          (void)memscpy(cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.impu_list,
                  evt_list->evt_ind.ims_reg_evt_ind.impu_list_len,
                  evt_list->evt_ind.ims_reg_evt_ind.impu_list,
                  evt_list->evt_ind.ims_reg_evt_ind.impu_list_len);
        }
        else {
          return GSTK_ERROR;
        }
      }

      /* Status Code */
      gstk_util_dump_byte_array(
        "IMS Status Code",
        evt_list->evt_ind.ims_reg_evt_ind.status_code,
        evt_list->evt_ind.ims_reg_evt_ind.status_code_len);
      if(evt_list->evt_ind.ims_reg_evt_ind.status_code_len > 0 &&
           evt_list->evt_ind.ims_reg_evt_ind.status_code != NULL) {
        cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.status_code_len =
          evt_list->evt_ind.ims_reg_evt_ind.status_code_len;
        cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.status_code =
          GSTK_CALLOC(evt_list->evt_ind.ims_reg_evt_ind.status_code_len);
        if(cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.status_code != NULL) {
          (void)memscpy(cmd->cmd.ims_reg_evt_envelope_ind.ims_reg.status_code,
                  evt_list->evt_ind.ims_reg_evt_ind.status_code_len,
                  evt_list->evt_ind.ims_reg_evt_ind.status_code,
                  evt_list->evt_ind.ims_reg_evt_ind.status_code_len);
        }
        else {
          return GSTK_ERROR;
        }
      }
    }
    else {
      UIM_MSG_ERR_0("IMS Registration Evt is not set");
      return GSTK_UNSUPPORTED_COMMAND;
    }
    break;

  default:
    UIM_MSG_MED_0("Unknown EVT Download");
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstk_util_build_evt_dl_ind */


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
gstk_status_enum_type gstk_util_map_mm_to_gstk_location_status(
  const gstk_mm_idle_state_type   * mm_idle_cmd,
  gstk_location_status_enum_type  * location_status)
{
  if(mm_idle_cmd == NULL || location_status == NULL)
  {
    UIM_MSG_ERR_2("mm_idle_cmd ERR: 0x%x, location_status Err: 0x%x",
                  mm_idle_cmd, location_status);
    return GSTK_BAD_PARAM;
  }

  switch(mm_idle_cmd->location_state) {
    case SYS_SRV_STATUS_NO_SRV:
      /* No service                             */
      *location_status = GSTK_NO_SERVICE;
      break;
    case SYS_SRV_STATUS_LIMITED:
      /* Limited service                        */
      *location_status = GSTK_LIMITED_SERVICE;
      break;
    case SYS_SRV_STATUS_SRV:
      /* Service available                      */
      *location_status = GSTK_NORMAL_SERVICE;
      break;
    default:
      return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_util_map_mm_to_gstk_location_status */

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
boolean gstk_util_is_valid_location_info(
  const uint8  * mcc_mnc,
  const uint8  * lac,
  const uint8  * cell_id, /*lint -esym(715, cell_id) */
                                        gstk_location_status_enum_type  location_status)
{
  uint8 gstk_bad_mcc_mnc[3];
  uint8 gstk_bad_lac[2];
  uint8 gstk_bad_cell_id[GSTK_MAX_CELL_ID_LEN];

  if((!mcc_mnc) || (!lac) || (!cell_id))
  {
    UIM_MSG_ERR_0("LAC/MCC_MNC/CELL_ID ptr ERR:NULL");
    return FALSE;
  }

  memset(gstk_bad_mcc_mnc, 0xFF, 3);
  memset(gstk_bad_lac, 0xFF, 2);
  memset(gstk_bad_cell_id, 0xFF, GSTK_MAX_CELL_ID_LEN);

  if (location_status == GSTK_NORMAL_SERVICE) {
    if (memcmp(gstk_bad_mcc_mnc, mcc_mnc, 3) == 0) {
      UIM_MSG_ERR_0("Normal Svc with MCC = 0xFF");
      return FALSE;
    }
    else if (memcmp(gstk_bad_lac, lac, 2) == 0) {
      UIM_MSG_ERR_0("Normal Svc with LAC = 0xFF");
      return FALSE;
    }
    else if (memcmp(gstk_bad_cell_id, cell_id, GSTK_MAX_CELL_ID_LEN) == 0) {
      UIM_MSG_ERR_0("Normal Svc with CELL ID = 0xFF");
      return FALSE;
    }
  }
  return TRUE;
} /* gstk_util_is_valid_location_info */

/*===========================================================================
FUNCTION gstk_util_check_mm_state_difference

DESCRIPTION
  This function will get the cell id and determines whether there are any
  differences between the new location info/state with the old new.  This
  function will return cell_id as a result of the operation.

PARAMETERS
  mm_idle_cmd: [Input] Pointer to gstk_mm_idle_state_type
  cell_id: [Input] pointer to cell id to be populated
  location_status: [Input] location status

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
boolean gstk_util_check_mm_state_difference(
  const uint8                     * mcc_mnc,
  const uint8                     * lac,
  const uint8                     * cell_id,
  const int32                       cell_len,
  gstk_location_status_enum_type    location_status)
{
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, FALSE);

  /* location_status:  New Location Status - Set by MM
  ** gstk_curr_inst_ptr->gstk_next_curr_location_status: Current Location Status set by GSTK (earlier)
  */
  UIM_MSG_HIGH_2("NEW LS: 0x%x OLD LS: 0x%x",
                 location_status,
                 gstk_curr_inst_ptr->gstk_next_curr_location_status);

  if ((mcc_mnc == NULL) ||
      (lac == NULL) ||
      (cell_id == NULL))
  {
    UIM_MSG_ERR_0("Null Input pointer");
    return FALSE;
  }

  /* compare location state */
  if(location_status != gstk_curr_inst_ptr->gstk_next_curr_location_status) {
    UIM_MSG_HIGH_2("NO MATCH LS:  NEW: 0x%x OLD: 0x%x",
                   location_status,
                   gstk_curr_inst_ptr->gstk_next_curr_location_status);
    return TRUE;
  }

  /* To avoid sending down multiple NO Server LS Events,
  ** check to see if status is GSTK_NO_SERVICE.  If it is
  ** there is no difference. */
  if (location_status == GSTK_NO_SERVICE) {
    UIM_MSG_HIGH_0("NEW: NO SVC  OLD: NO SVC - No LS EVT");
    return FALSE;
  }

  /* compare cell id */
  if (cell_len != gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len) {
    UIM_MSG_HIGH_2("NO MATCH CELL ID LEN: NEW: 0x%x OLD: 0x%x",
                   cell_len,
                   gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len);
    return TRUE;
  }

  /* Same cell Len */
  if (memcmp(cell_id,
             gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id,
             (uint32)cell_len) != 0)
  {
    UIM_MSG_HIGH_0("NO MATCH CELL ID");
    return TRUE;
  }

  /* compare mcc, mnc */
  if(memcmp(mcc_mnc, gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc, 3) != 0) {
    UIM_MSG_HIGH_0("NO MATCH MCC MNC");
    return TRUE; /* mcc mnc is different */
  }

  /* compare lac */
  if(memcmp(lac, gstk_curr_inst_ptr->gstk_next_curr_lac, 2) != 0) {
    UIM_MSG_HIGH_0("NO MATCH LAC");
    return TRUE; /* lac is different */
  }

  return FALSE;
} /* gstk_util_check_mm_state_difference */


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
boolean gstk_util_compare_curr_and_next_location_info( void )
{
  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, FALSE);

  /* compare location state */
  if (gstk_curr_inst_ptr->gstk_curr_location_status != gstk_curr_inst_ptr->gstk_next_curr_location_status) {
    return TRUE;
  }

  if (gstk_curr_inst_ptr->gstk_next_curr_location_status != GSTK_NORMAL_SERVICE) {
    /* no need to check further, no difference */
    return FALSE;
  }

  /* compare cell id */
  if(memcmp(&gstk_curr_inst_ptr->gstk_curr_cell_id,
            &gstk_curr_inst_ptr->gstk_next_curr_cell_id,
            sizeof(gstk_cell_id_type)) != 0)
  {
    return TRUE;
  }

  /* compare MCC, MNC */
  if(memcmp(gstk_curr_inst_ptr->gstk_curr_mcc_mnc,
            gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc,
            3) != 0)
  {
    return TRUE; /* mcc mnc is different */
  }

  /* compare lac */
  if(memcmp(gstk_curr_inst_ptr->gstk_curr_lac,
            gstk_curr_inst_ptr->gstk_next_curr_lac,
            2) != 0) {
    return TRUE; /* lac is different */
  }

  return FALSE;
} /* gstk_util_compare_curr_and_next_location_info */


/*===========================================================================
FUNCTION gstk_util_pack_7_bit_chars

DESCRIPTION
  This function pack the input data into SMS 7 bit format.

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
uint16 gstk_util_pack_7_bit_chars
(
  const uint8     * in,
  uint16          in_len, //number of chars
  uint8           * out
)
{
  uint16    i=0;
  uint16    pos   = 0;
  uint8     shift = 0;

  if(in == NULL || out == NULL)
  {
    UIM_MSG_ERR_2("in 0x%x, out 0x%x", in, out);
    return pos;
  }

  /* pack the ASCII characters
  */

  for( pos=0;
       i < in_len;
       pos++, i++ )
  {
    /* pack the low bits */
    out[pos] = in[i] >> shift;

    if( i+1 < in_len )
    {
      /* pack the high bits using the low bits of the next character */
      out[pos] |= (uint8) (in[i+1] << (7-shift));

      shift ++;

      if( shift == 7 )
      {
        shift = 0;
        i ++;
      }
    }
  }

  /* done */
  return pos;

} /* gstk_util_pack_7_bit_chars */

/*===========================================================================
FUNCTION gstk_byte_memmove

DESCRIPTION
  This function copies len bytes starting from s to d

PARAMETERS
  s: [Input] Pointer to Input data
  len: [Input] Input data length
  d: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  This doesn't check for invalid input

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_byte_memmove (uint8* d, uint8* s, uint8 len)
{
  uint8 i;
  for (i = 0; i < len; i++)
    *d++ = *s++;
}

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
uint8 gstk_util_unpack_7_bit_chars
(
  const uint8       * in,
  uint8             in_len,
  uint8             * out
)
{
  int      i=0;
  uint16   pos=0;
  uint16   shift = 0;
  uint8    j = 0;

  if(in == NULL || out == NULL)
  {
    UIM_MSG_ERR_2("In gstk_util_unpack_7_bit_chars In: 0x%x, Out 0x%x",
                  in, out);
    return (uint8)pos;
  }

  for( i=0;
       pos < in_len;
       i++, pos++ )
  {
    out[i] = ( in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      shift = 0;
      if ((pos+1 == in_len) && (in[pos] >> 1 == 0x0D))
        continue;
      /* a possible extra complete character is available */
      i ++;
      out[i] = in[pos] >> 1;

    }
  }

  while((i > 0) && (out[i-1] == 0x00))
  {
    i -= 1;
  }
  /* Strip extra carriage return(0x0D) when they occur consecutively */
  for (j=0; j<i; j++)
  {
    if (out[j] == 0x0D && out[j+1] == 0x0D)
    {
      gstk_byte_memmove(&out[j+1], &out[j+2], i-(j+1));
      out[i--] = 0x00;
    }
  }
  return(uint8)(i);

} /* gstk_util_unpack_7_bit_chars() */

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
boolean gstk_util_has_pending_timer_envelope(
  gstk_slot_id_enum_type slot
)
{
  int i = 0;

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return FALSE;
  }

  for (i = 0; i < GSTK_MAX_TIMER_SUPPORTED; i++) {
    if (gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[i].pending &&
        !gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[i].waiting_rsp) {
      return TRUE;
    }
  }
  return FALSE;
} /* gstk_util_has_pending_timer_envelope */



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
uint16 gstk_util_find_first_timer_expire_command(
  gstk_slot_id_enum_type slot
)
{
  uint16 i = 0;

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_MAX_TIMER_SUPPORTED;
  }
  for (i = 0; i < GSTK_MAX_TIMER_SUPPORTED; i++)
  {
    if (gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[i].pending &&
        !gstk_instances_ptr[(uint32)slot-1]->gstk_timer_management[i].waiting_rsp)
    {
      return i;
    }
  }
  return GSTK_MAX_TIMER_SUPPORTED;
} /* gstk_util_find_first_timer_expire_command */


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
boolean gstk_util_has_pending_setup_call(void)
{
  int i = 0;

  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++) {
    /* the location is not free */
    if(!(command_details_ref_table_ptr[i].free_slot)) {
      /* if the envelope is a call connected evt */
      if (command_details_ref_table_ptr[i].expected_cmd_rsp_type ==
          GSTK_SETUP_CALL_CNF) {
        return TRUE;
      }
    }
  }
  return FALSE;
} /* gstk_util_has_pending_setup_call */

/*===========================================================================
FUNCTION gstk_util_dereg_client_type

DESCRIPTION
  This function dereg events for certain client type.  dereg_evt indicates
  the events to be removed from the client's list

PARAMETERS
  client_type: [Input] Specifies the client type
  dereg_evt:   [Input] Specifies the events to be deregistered

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  When the client type has no more event that it is registered for, the
  client will be removed from the gstk_client_table

SEE ALSO
  None
===========================================================================*/
void gstk_util_dereg_client_type(gstk_client_type client_type, uint32 dereg_evt)
{
  int i = 0;

  for (i = 0; i<GSTK_MAX_CLIENT; i++) {
    if (gstk_shared_data.gstk_client_table[i].client_type == client_type) {
        gstk_shared_data.gstk_client_table[i].is_free = TRUE;
        gstk_shared_data.gstk_client_table[i].client_cb = NULL;
        gstk_shared_data.gstk_client_table[i].client_type = GSTK_NO_TYPE;
        gstk_shared_data.gstk_client_table[i].user_data = 0;
    }
  }
} /* gstk_util_dereg_client_type*/

/*===========================================================================
FUNCTION gstk_util_decode_dcs_type

DESCRIPTION
  This function determines if the dcs is 7 bit or 8 bit or ucs2 depending
  on whether it is a cell broadcast dcs or not

PARAMETERS
  is_cb_dcs - This flag is used for differentiating if the dcs is for
              ussd or not
  cs        - data coding scheme

DEPENDENCIES
  None

RETURN VALUE
  gstk_dcs_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_dcs_enum_type gstk_util_decode_dcs_type(boolean is_cb_dcs, uint8 dcs)
{
  if (!is_cb_dcs) {
    if ((dcs & GSTK_SMS_GENERAL_DATA_CODING_MASK) == GSTK_SMS_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_8_BIT;
    }
    if ((dcs & GSTK_SMS_GENERAL_DATA_CODING_MASK) == GSTK_SMS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_UCS2_8_BIT;
    }
    if ((dcs & GSTK_SMS_DATA_CODING_MASK) == GSTK_SMS_8_BIT_DCS) {
      return GSTK_DCS_8_BIT;
    }
  }
  else {
    /* Decode according to 23.038 */
    if ((dcs & GSTK_CBS_DATA_CODING_MASK) == GSTK_CBS_8_BIT_DCS) {
      return GSTK_DCS_8_BIT;
    }
    if ((dcs & GSTK_CBS_GENERAL_DATA_CODING_MASK) == GSTK_CBS_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_8_BIT;
    }
    if ((dcs & GSTK_CBS_GENERAL_DATA_CODING_MASK) == GSTK_CBS_UCS2_8_BIT_GENERAL_DATA_CODING_DCS) {
      return GSTK_DCS_UCS2_8_BIT;
    }
    if ((dcs & GSTK_CBS_UCS2_7_BIT_DCS) == GSTK_CBS_UCS2_7_BIT_DCS) {
      return GSTK_DCS_UCS2_7_BIT;
    }
  }
  return GSTK_DCS_7_BIT;
}

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
boolean gstk_util_is_valid_channel_id_range(gstk_device_identity_type device_id)
{
  if (((int)device_id > GSTK_CHANNEL_ID_BASE) && ((int)device_id <= GSTK_CHANNEL_ID_MAX_VALUE)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
} /* gstk_util_is_valid_channel_id_range */

/*===========================================================================
FUNCTION gstk_util_unpack_7_bit_chars_ext

DESCRIPTION
  This function unpacks the input data from SMS7 bit format into ASCII
  and returns the actual data length without the null terminated byte.

PARAMETERS
  in: [Input] Input data pointer
  in_len: [Input] Input data length
  out: [Input/Output] Pointer to output data

DEPENDENCIES
  None

RETURN VALUE
  int32:length of output data length
  If this function returns negative value, it should be considered as
  an error.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
int32 gstk_util_unpack_7_bit_chars_ext
(
  const uint8       * in,
  int32             in_len,
  uint16            * out
)
{
  int      i=0;
  uint16   pos=0;
  uint16   shift = 0;
  uint8    j = 0;

  if(in == NULL || out == NULL)
  {
    UIM_MSG_ERR_2("In gstk_util_unpack_7_bit_chars_ext() in 0x%x, out 0x%x",
                  in, out);
    return pos;
  }

  for( i=0;
       pos < in_len;
       i++, pos++ )
  {
    out[i] = ( in[pos] << shift ) & 0x7F;

    if( pos != 0 )
    {
      /* except the first byte, a character contains some bits
      ** from the previous byte.
      */
      out[i] |= in[pos-1] >> (8-shift);
    }

    shift ++;

    if( shift == 7 )
    {
      if ((pos+1 == in_len) && (in[pos] >> 1 == 0x0D))
        continue;
      shift = 0;

      /* a possible extra complete character is available */
      i ++;
      out[i] = in[pos] >> 1;
    }
  }

  while((i > 0) && (out[i-1] == 0x00))
  {
    i -= 1;
  }

  /* Strip extra carriage return(0x0D) when they occur consecutively */
  for (j=0; j<i; j++)
  {
    if (out[j] == 0x0D && out[j+1] == 0x0D)
    {
      gstk_byte_memmove((uint8*)&out[j+1], (uint8*)&out[j+2], i-(j+1));
      out[i--] = 0x00;
    }
  }

  return (i);
} /* gstk_util_unpack_7_bit_chars_ext() */

/*===========================================================================
FUNCTION gstk_util_find_apn_len

DESCRIPTION
  This function returns the total length of the APN. This is needed to determine
  the no of extra bytes that needs to be allocated for label separators '.'
  and the null bytes.


PARAMETERS
  uint8* apn_bytes - Byte array containing the APN
  int apn_length   - Length of the APN Byte Array
  int *cnt         - New length that includes the '.' and the nulll byte.


DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS
  GSTK_ERROR

===========================================================================*/
gstk_status_enum_type gstk_util_find_apn_len(
  const uint8 * apn_bytes,
  int           apn_length,
  int         * cnt)
{
  int i = 0;
  if(apn_bytes == NULL || cnt == NULL)
  {
    UIM_MSG_ERR_2("gstk_util_find_apn_len(): apn_bytes 0x%x, cnt 0x%x",
                  apn_bytes, cnt);
    return GSTK_ERROR;
  }

  *cnt = 0;
  while(i < apn_length)
  {
    /* Adding one byte for the '.'
    ** No need for the label_len to be taken into account.
    */
    *cnt = *cnt + apn_bytes[i] + 1;

    /* Adding an additional byte for the label_len itself */
    i = i + apn_bytes[i] + 1;
  }
  return GSTK_SUCCESS;
}

/*===========================================================================
FUNCTION gstk_util_check_for_apn_net_op_id

DESCRIPTION
  This function determines whether the label is part of the APN Network Id
  or APN Operator Id.

PARAMETERS
  char *apn_bytes - APN
  int apn_len     - Length
  int pos         - Start position
  int apn_val     - GSTK_APN_NET_ID/GSTK_APN_OP_ID

DEPENDENCIES
  None

RETURN VALUE
  GSTK_SUCCESS
  GSTK_ERROR


===========================================================================*/
gstk_status_enum_type gstk_util_check_for_apn_net_op_id(
  const char * apn_bytes,
  int          apn_len,
  int        * apn_val)
{

  unsigned int label_len = 0;
  int ind = 0;
  char tmp[5] = "gprs";
  char tmp1[4] = "mnc";
  char tmp2[4] = "mcc";
  char tmp3[5];

  if((apn_bytes == NULL) || (apn_val == NULL))
    return GSTK_ERROR;

  label_len = apn_bytes[ind];
  if(label_len < GSTK_MIN_OP_LABEL_LEN)
  {
    *apn_val = GSTK_APN_NET_ID;
    return GSTK_SUCCESS;
  }

  if(label_len < strlen(tmp1))
  {
    *apn_val = GSTK_APN_NET_ID;
    return GSTK_SUCCESS;
  }
  else
  {
    ind = ind + uint32toint32(label_len) + 1;

    if(ind > apn_len)
      return GSTK_ERROR;

    label_len = apn_bytes[ind];
    if(label_len  < strlen(tmp2))
    {
      *apn_val = GSTK_APN_NET_ID;
      return GSTK_SUCCESS;
    }
  }

  if(label_len >= GSTK_MIN_OP_LABEL_LEN && label_len <= GSTK_MAX_OP_LABEL_LEN)
  {
    ind = 0;
    label_len = apn_bytes[ind];
    ind++;
    if(ind > apn_len)
      return GSTK_ERROR;
    memset(tmp3,0x00,5*sizeof(char));

    (void)memscpy(tmp3,sizeof(tmp3),&apn_bytes[ind],3);
    tmp3[3] = '\0';
     /* No match with 'mnc' */
    if(strcmp(tmp3,tmp1) != 0)
    {
      *apn_val = GSTK_APN_NET_ID;
      return GSTK_SUCCESS;
    }

    ind = ind + uint32toint32(label_len);
    if(ind > apn_len)
      return GSTK_ERROR;

    label_len =  apn_bytes[ind];
    ind++;
    memset(tmp3,0x00,5*sizeof(char));
    (void)memscpy(tmp3,sizeof(tmp3),&apn_bytes[ind],3);
    tmp3[3] = '\0';

    /* No match with 'mcc' */
    if(strcmp(tmp3,tmp2) != 0)
    {
      *apn_val = GSTK_APN_NET_ID;
      return GSTK_SUCCESS;
    }

    ind = ind + uint32toint32(label_len);

    if(ind > apn_len)
      return GSTK_ERROR;

    label_len =  apn_bytes[ind];
    ind++;
    memset(tmp3,0x00,5*sizeof(char));
    (void)memscpy(tmp3,sizeof(tmp3),&apn_bytes[ind],4);
    tmp3[4] = '\0';

    /* No match with 'gprs' */
    if(strcmp(tmp3,tmp) == 0)
    {
      *apn_val = GSTK_APN_OP_ID;
      return GSTK_SUCCESS;
    }
    /* Match with 'mnc' and 'mcc' and 'gprs'*/
  }
  *apn_val = GSTK_APN_NET_ID;
  return GSTK_SUCCESS;
} /* gstk_util_check_for_apn_net_op_id */

/*===========================================================================
FUNCTION gstk_util_extract_label_len_value

DESCRIPTION
  Function used by GSTK to extract label and field value from the APN

PARAMETERS
  int32          *alphabet_in     - input buffer of type uint8
  uint16         *alphabet_out    - output buffer of type uint16

DEPENDENCIES

RETURN VALUE
  static int32

===========================================================================*/
gstk_status_enum_type gstk_util_extract_label_len_value(
  int32                apn,
  const uint8        * alphabet_in,
  uint16             * alphabet_out,
  int                * ind ,
  int32              * count,
  int                * iter
)
{
  int i = 0;
  int j = 0;
  int cnt = 0;
  int lb_len = 0;
  boolean match = 0;
  char tmp[5];
  int k = 0;
  uint8* temp;
  int init = 0;
  int val = 0;
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  if((alphabet_in == NULL)|| (alphabet_out == NULL) ||
    (count == NULL) || (ind == NULL) || (iter == NULL))
  {
    return GSTK_ERROR;
  }
  /*
  Extract the length portion of the label
  */
  j=0;
  lb_len = alphabet_in[j];

  memset(&(alphabet_out[*ind]), 0x00, sizeof(uint16)*int32touint32(lb_len));

  if(lb_len == 0)
  {
    *ind = *ind + alphabet_in[j] + 1;
    *iter = *iter + alphabet_in[j] + 1;
    *count = *count + alphabet_in[j] + 1;
    return GSTK_SUCCESS;
  }
  if(lb_len < 0)
  {
    return GSTK_ERROR;
  }
  /*
  If the length is greater than zero, depending on whether the APN_NET_OP
  is APN_NET/APN_OP, invoke the function to make a check for strings not to
  be used - rac, lac, sgsn,
  */
  temp = GSTK_CALLOC(int32touint32(lb_len+1));
  if(temp == NULL)
  {
    return GSTK_ERROR;
  }
  (void)memscpy(temp,int32touint32(lb_len),&alphabet_in[j+1],int32touint32(lb_len));
  temp[lb_len]= 0x00;

  /*
  ** APN - NETWORK IDENTIFIER
  */
  switch(apn)
  {
  case GSTK_APN_NET_ID:
    /*
    ** Make a check for strings like rac, lac, sgsn
    */
    for (cnt=0; cnt <gstk_start_str_cnt; cnt++ )
    {
      gstk_status = gstk_util_compareString((int)GSTK_SEARCH_BEGIN_STR,
                                            gstk_apn_start_str_table[cnt],
                                            (char *)temp,&match);
      if(gstk_status == GSTK_ERROR)
      {
        gstk_free(temp);
        temp = NULL;
        return GSTK_ERROR;
      }

      if(match)
      {
        gstk_free(temp);
        temp = NULL;
        UIM_MSG_ERR_1("APN NET ID Cannot contain string %s",
                      gstk_apn_start_str_table[cnt]);
        return GSTK_ERROR;
      }
    }
    for (cnt=0; cnt < gstk_end_str_cnt; cnt++ )
    {
      gstk_status = gstk_util_compareString((int)GSTK_SEARCH_END_STR,
                                            gstk_apn_end_str_table,
                                            (char *)temp,
                                            &match);
      if(gstk_status == GSTK_ERROR)
      {
        gstk_free(temp);
        temp = NULL;
        return GSTK_ERROR;
      }

      if(match)
      {
        gstk_free(temp);
        temp = NULL;
        UIM_MSG_ERR_1("APN OP ID Cannot contain string %s",
                      gstk_apn_end_str_table );
        return GSTK_ERROR;
      }
    }
    /* Check for the presence of a  * */
    gstk_status = gstk_util_compareString((int)GSTK_SEARCH_SINGLE_CHAR, "*", (char *)temp, &match);

    if(gstk_status == GSTK_ERROR)
    {
      gstk_free(temp);
      temp = NULL;
      return GSTK_ERROR;
    }

    if(match)
    {
      gstk_free(temp);
      temp = NULL;
      UIM_MSG_ERR_0("APN NET ID Cannot contain string *" );
      return GSTK_ERROR;
    }
    /* Copy the data from input to output */
    /*label_len */
    for( i = *ind,j=1; j <= alphabet_in[0] && i <= (*ind+lb_len);j++,i ++)
    {
      alphabet_out[i] =  alphabet_in[j];
    }
    alphabet_out[i] = 0x2e;

    *ind = *ind + alphabet_in[0] + 1;
    *iter = *iter + alphabet_in[0] + 1;
    *count = *count + alphabet_in[0] + 1;

    if(temp != NULL)
    {
    gstk_free(temp);
    temp = NULL;
    }
    return GSTK_SUCCESS;
    /*
    **  APN - OPERATOR IDENTIFIER
    */
  case GSTK_APN_OP_ID:
    /* Copy the data from input to output */
    /*label_len */
    while(val < 3)
    {
      memset(tmp,0x00,sizeof(char)*5);
      lb_len = alphabet_in[init];
      (void)memscpy(tmp,sizeof(tmp),&alphabet_in[init+1],3);
      tmp[3] = 0x00;
      if((strcmp(tmp,"mnc") == 0) || (strcmp(tmp,"mcc") == 0))
      {
        for( i = *ind, j = init+1; i <= (*ind+3) && j <= init+3; i ++, j++)
        {
          alphabet_out[i] =  alphabet_in[j];
        }
        for(j=*ind + GSTK_MAX_OP_LABEL_LEN, k = init+lb_len; (j> 0) && (k > init+3);k--,j--)
        {
          alphabet_out[j] = alphabet_in[k];
        }
        alphabet_out[*ind+7] = 0x2e;
        /* Add a byte for the '.' and another byte to move to the next location */
        *ind = *ind + GSTK_MAX_OP_LABEL_LEN + 1 + 1;
      }
      else /* If the string is gprs */
      {
        for( i = *ind,j=init; j <= init+lb_len && i <= (*ind+lb_len);j++,i ++)
        {
          alphabet_out[i] =  alphabet_in[j];
        }
        /* Add a byte for the label length */
        *ind = *ind + GSTK_MIN_OP_LABEL_LEN + 1;
      }
      *iter = *iter + alphabet_in[init] + 1;
      *count = *count + alphabet_in[init] + 1;
      val++;
      init = init + lb_len + 1;
    }/* end of while */
    gstk_free(temp);
    temp = NULL;
    return GSTK_SUCCESS;
  default:
    gstk_free(temp);
    temp = NULL;
    return GSTK_ERROR;
  }/* switch(apn) */
} /* gstk_util_extract_label_len_value */

/*===========================================================================
FUNCTION gstk_util_compareString

DESCRIPTION
  Generic Function used by GSTK to compare two strings/substrings

PARAMETERS
  int pos      - Start position
  char * src   - String1 to be compared.
  char * dest  - String2 to be compared.

DEPENDENCIES

RETURN VALUE
  static int32

===========================================================================*/
gstk_status_enum_type gstk_util_compareString(
    int          pos,
    const char * src,
    const char * dest,
    boolean    * match
                                              )
{
  int i = 0;
  int j = 0;
  int cnt = 0;
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;

  /* Case to match other strings like rac, lac, sgsn, gprs */
  if(src == NULL || dest == NULL || match == NULL)
    return GSTK_ERROR;

  if((strlen(dest) < strlen(src)) && (pos != (int)GSTK_SEARCH_SINGLE_CHAR))
  {
    *match = FALSE;
    return GSTK_SUCCESS;
  }

  switch(pos)
  {
  case GSTK_SEARCH_BEGIN_STR:
    for(i = 0; i<(int)strlen(src) && i < (int)strlen(dest); i++)
    {
      if(src[i] != dest[i])
      {
        *match = FALSE;
        gstk_status = GSTK_SUCCESS;
        break;
      }
    }
    if(j == uint32toint32(strlen(src)))
    {
      *match = TRUE;
      gstk_status = GSTK_SUCCESS;
    }
    else
    {
      *match = FALSE;
      gstk_status = GSTK_SUCCESS;
    }
    break;

  case GSTK_SEARCH_END_STR:
    cnt = 0;
    for(i = ((int)strlen(src))-1,j = ((int)strlen(dest))-1; i>=0 && j >= 0; i--, j--)
    {
      if(src[i] != dest[j])
      {
        *match = FALSE;
        gstk_status = GSTK_SUCCESS;
        break;
      }
      cnt = cnt + 1;
    }
    if(cnt == uint32toint32(strlen(src)))
    {
      *match = TRUE;
      gstk_status = GSTK_SUCCESS;
    }
    else
    {
      *match = FALSE;
      gstk_status = GSTK_SUCCESS;
    }
    break;

  case GSTK_SEARCH_SINGLE_CHAR:
    /* Case to match a '*' */
    if(strcmp(src,"*") == 0)
    {
      for(j = 0; j < (int)strlen(dest); j++)
      {
        if(dest[j] == '*')
        {
          *match = TRUE;
          gstk_status = GSTK_SUCCESS;
          break;
        }
      }
      *match = FALSE;
      gstk_status = GSTK_SUCCESS;
    }
    break;

  default:
    UIM_MSG_HIGH_0("INVALID Search Criteria");
    *match = FALSE;
    gstk_status = GSTK_ERROR;
  }
  return gstk_status;
} /* gstk_util_compareString */

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
void ASCIIText_ConvertTo_GSMDefault (const uint8   * def,
                                     uint32          len,
                                     uint8         * gsm_default
)
{
  uint32  i = 0;
  uint32  j = 0;

  if(def == NULL)
  {
    UIM_MSG_ERR_0("def ERR:NULL");
    return;
  }

  if(gsm_default == NULL)
  {
    UIM_MSG_ERR_0("gsm_default ERR:NULL");
    return;
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i = 0; i < len; i ++ )
  {
    j = def[i];
    gsm_default[i] = ascii_to_gsmdefault_table[j];
  }

} /* ASCIIText_ConvertTo_GSMDefault() */

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
gstk_status_enum_type gstkutil_convert_rrc_plmn_id_to_nas_plmn_id( gstk_rrc_plmn_identity_type plmn_id, byte *nas_plmn_id)
{

 if(nas_plmn_id == NULL)
 {
   return GSTK_ERROR;
 }

 memset(nas_plmn_id,0x00,3);

 if(plmn_id.num_mnc_digits == 2)
 {
    plmn_id.mnc[2] = 0x0F;
 }

 /* ----------------------------------------
 ** PLMN octet 1 = MCC digit 2 | MCC digit 1
 ** ---------------------------------------- */
 nas_plmn_id[0] = (byte) ((plmn_id.mcc[1] << 4) + plmn_id.mcc[0]);

 /* ----------------------------------------
 ** PLMN octet 2 = MNC digit 3 | MCC digit 3
 ** ---------------------------------------- */
 nas_plmn_id[1] = (byte) ((plmn_id.mnc[2] << 4) + plmn_id.mcc[2]);

 /* ----------------------------------------
 ** PLMN octet 3 = MNC digit 2 | MNC digit 1
 ** ---------------------------------------- */
 nas_plmn_id[2] = (byte) ((plmn_id.mnc[1] << 4) + plmn_id.mnc[0]);

 return GSTK_SUCCESS;

} /* end gstk_convert_rrc_plmn_id_to_nas_plmn_id() */
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
  gstk_slot_id_enum_type         slot)
{
  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    UIM_MSG_ERR_1("invalid slot id 0x%x!", slot);
    return GSTK_ERROR;
  }

  if(plmn_id_ptr == NULL || lac_ptr == NULL)
  {
    return GSTK_ERROR;
  }

  if(mm_or_rrc_update == MM_GSTK_LOCI_UPDATE)
  {
    (void)memscpy(gstk_instances_ptr[slot - 1]->gstk_loci_table.plmn_id,
    	          sizeof(gstk_instances_ptr[slot - 1]->gstk_loci_table.plmn_id),
    	          plmn_id_ptr,
                  3);
    (void)memscpy(gstk_instances_ptr[slot - 1]->gstk_loci_table.lac,
    	          sizeof(gstk_instances_ptr[slot - 1]->gstk_loci_table.lac),
    	          lac_ptr,
                  2);
    (void)memscpy(gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_id,
                  sizeof(gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_id),
                  cell_info.cell_id,
                  (uint32)cell_info.cell_len);
    gstk_instances_ptr[slot - 1]->gstk_loci_table.cell_info.cell_len = cell_info.cell_len;
    gstk_instances_ptr[slot - 1]->gstk_loci_table.loc_status = loc_status;

  } /* MM_GSTK_LOCI_UPDATE */
#if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
  else if(mm_or_rrc_update == RRC_LOCI_UPDATE)
  {
    uint8 i = GSTK_RAT_MAX_IDX;
    switch(rat)
    {
      case GSTK_ACCESS_TECH_UTRAN:
        i = GSTK_RAT_UMTS_IDX;
        break;
      case GSTK_ACCESS_TECH_LTE:
        i = GSTK_RAT_LTE_IDX;
        break;
      default:
        UIM_MSG_ERR_1("Invalid RAT 0x%x", rat);
        return GSTK_ERROR;
    }
    /* NOTE:- There is no location status field in the RRC table */
    (void)memscpy(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].plmn_id,
    	          sizeof(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].plmn_id),
    	          plmn_id_ptr,
                  3);
    (void)memscpy(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].lac,
    	          sizeof(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].lac),
    	          lac_ptr,
                  2);
    (void)memscpy(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].cell_info.cell_id,
    	          sizeof(gstk_instances_ptr[slot - 1]->rrc_loci_table[i].cell_info.cell_id),
                  cell_info.cell_id,
                  (uint32)cell_info.cell_len);
    gstk_instances_ptr[slot - 1]->rrc_loci_table[i].cell_info.cell_len = cell_info.cell_len;
  } /* RRC_LOCI_UPDATE */
#endif /* FEATURE_WCDMA || FEATURE_LTE */
  else
  {
    return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
} /* gstkutil_update_plmn_lac_cellId_table */
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE) */

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
boolean gstk_util_is_valid_rat(sys_radio_access_tech_e_type sys_rat,
                               gstk_access_technology_type *gstk_rat_ptr)
{

  if (gstk_rat_ptr == NULL) {
    UIM_MSG_ERR_0("Null gstk_rat pointer");
    return FALSE;
  }

  /* print out RAT info */
  switch(sys_rat)
  {
#if defined(FEATURE_GSM)
  case SYS_RAT_GSM_RADIO_ACCESS:
    *gstk_rat_ptr = GSTK_ACCESS_TECH_GSM;
    return TRUE;
#endif /* FEATURE_GSM */

#ifdef FEATURE_WCDMA
  case SYS_RAT_UMTS_RADIO_ACCESS:
    *gstk_rat_ptr = GSTK_ACCESS_TECH_UTRAN;
    return TRUE;

  case SYS_RAT_TDS_RADIO_ACCESS:
    *gstk_rat_ptr = GSTK_ACCESS_TECH_UTRAN;
    return TRUE;
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
  case SYS_RAT_LTE_RADIO_ACCESS:
    *gstk_rat_ptr = GSTK_ACCESS_TECH_LTE;
    return TRUE;
#endif /*FEATURE_LTE*/

  default:
    UIM_MSG_ERR_1("Unknown Rat 0x%x", sys_rat);
    /* Indicate not to send any LS;  However, if we are in No Service,
       the switch statement on location status below will reset the
       boolean to TRUE, so that we will still send down an envelope
       for NO SERVICE regardless of RAT */
    return FALSE;
  }
}/*lint !e818 "gstk_rat_ptr not const . This lint error is suppressed
  when FEATURE_GSM or FEATURE_WCDMA is not defined." */


#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_util_diff_cdma_ext_loc_info

DESCRIPTION
  This function will diff the old 3GPP2 cache with the new 3GPP2 cache

PARAMETERS
  cdma_loc_info [Input] CDMA location information to be compared

DEPENDENCIES
  None

RETURN VALUE 
  FALSE : There is no difference in CDMA location information
  TRUE  : There is difference in CDMA location information

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_diff_cdma_ext_loc_info(
  gstk_cdma_loci_ext_cache_type* old_cache,
  gstk_cdma_loci_ext_cache_type* new_cache)
{
  UIM_MSG_HIGH_0("gstk_util_diff_cdma_ext_loc_info()");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, FALSE);
  GSTK_RETURN_ERROR_IF_NULL_PTR(old_cache, FALSE);
  GSTK_RETURN_ERROR_IF_NULL_PTR(new_cache, FALSE);

  /* SID info */
  if(old_cache->data.cdma_loc_info.sid_info.id_len != new_cache->data.cdma_loc_info.sid_info.id_len
     ||
     memcmp(old_cache->data.cdma_loc_info.sid_info.sid,
            new_cache->data.cdma_loc_info.sid_info.sid,
            old_cache->data.cdma_loc_info.sid_info.id_len) != 0)
  {
    return TRUE;
  }

  /* NID info */
  if(old_cache->data.cdma_loc_info.nid_info.id_len != new_cache->data.cdma_loc_info.nid_info.id_len
     ||
     memcmp(old_cache->data.cdma_loc_info.nid_info.nid,
            new_cache->data.cdma_loc_info.nid_info.nid,
            old_cache->data.cdma_loc_info.nid_info.id_len) != 0)
  {
    return TRUE;
  }


  /* MCC info */
  if(memcmp(old_cache->data.cdma_loc_info.mcc,
            new_cache->data.cdma_loc_info.mcc, 
            GSTK_MCC_LEN) != 0)
  {
    return TRUE;
  }

    /* IMSI_11_12 */
  if(old_cache->data.cdma_loc_info.imsi_11_12
      != new_cache->data.cdma_loc_info.imsi_11_12)
  {
    return TRUE;
  }

  /* BASE_ID info */
  if(old_cache->data.cdma_loc_info.base_id_info.id_len != new_cache->data.cdma_loc_info.base_id_info.id_len
     ||
     memcmp(old_cache->data.cdma_loc_info.base_id_info.base_id,
            new_cache->data.cdma_loc_info.base_id_info.base_id,
            old_cache->data.cdma_loc_info.base_id_info.id_len) != 0)
  {
    return TRUE;
  }

  /* BASE Latitude */
  if(memcmp(old_cache->data.cdma_loc_info.base_lat,
            new_cache->data.cdma_loc_info.base_lat, 
            GSTK_BASE_LAT_LEN) != 0)
  {
    return TRUE;
  }

  /* BASE Longitude */
  if(memcmp(old_cache->data.cdma_loc_info.base_long,
            new_cache->data.cdma_loc_info.base_long, 
            GSTK_BASE_LONG_LEN) != 0)
  {
    return TRUE;
  }

  if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE)
  {
     /* Height */
     if(memcmp(old_cache->data.height,
               new_cache->data.height, GSTK_HEIGHT_LEN) != 0)
     {
       return TRUE;
     }

     /* Horizontal Uncertainty */
     if(old_cache->data.horizontal_uncertainty !=
        new_cache->data.horizontal_uncertainty)
     {
       return TRUE;
     }

     /* Country Code */
     if(memcmp(old_cache->data.country_code,
               new_cache->data.country_code, 
               GSTK_MCC_LEN) != 0)
     {
       return TRUE;
     }

     /* Sector ID */
     if(memcmp(old_cache->data.sector_id,
               new_cache->data.sector_id, 
               GSTK_HDR_SECTOR_ID_LEN) != 0)
     {
       return TRUE;
     }

     /* CDMA Channel */
     if(memcmp(old_cache->data.cdma_channel,
               new_cache->data.cdma_channel, 
               GSTK_CDMA_CHANNEL_LEN) != 0)
     {
       return TRUE;
     }

     /* Base Latitude */
     if(memcmp(old_cache->data.base_lat,
               new_cache->data.base_lat, 
               GSTK_BASE_LAT_LEN) != 0)
     {
       return TRUE;
     }

     /* Base Longitude */
     if(memcmp(old_cache->data.base_long,
               new_cache->data.base_long, 
               GSTK_BASE_LONG_LEN) != 0)
     {
       return TRUE;
     }
  }

  return FALSE;
} /* gstk_util_diff_cdma_ext_loc_info */
#endif /*#ifdef FEATURE_CDMA */


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
boolean gstk_util_find_match_in_cmd_details_ref_table(
    uint32                           * ref_id_ptr,
    gstk_cmd_enum_type               rsp_type
)
{
  int i = 0;

  if(ref_id_ptr == NULL)
  {
      UIM_MSG_HIGH_0("cmd_num ptr is NULL...");
      return FALSE;
  }

  for(i=0;i<GSTK_MAX_PROACTIVE_COMMAND;i++)
  {
    if (command_details_ref_table_ptr[i].expected_cmd_rsp_type == rsp_type)
    {
      UIM_MSG_HIGH_1("Found a match in the cmd_details_ref_table. Unique_ref_id : 0x%x",
                     	 	 command_details_ref_table_ptr[i].unique_ref_id);
      *ref_id_ptr = command_details_ref_table_ptr[i].unique_ref_id;
      return TRUE;
    }
  }
  UIM_MSG_HIGH_0("No match in the cmd_details_ref_table...");
  return FALSE;
} /* gstk_util_find_match_in_cmd_details_ref_table */


/*===========================================================================
FUNCTION gstk_util_find_tlv_len

DESCRIPTION
  This function finds the Len for the simple TLV.

PARAMETERS
  tag:               [Input] Simple TLV Tag
  orig_cmd_data_ptr: [Input] Original Command Data from which the length is
                     calculated from
  length_ptr:        [Input/Output] pointer to the length is returned
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
static gstk_status_enum_type gstk_util_find_tlv_len (
  tags_enum_type              tag,
  const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
  int32                      *length_ptr,
  gstk_cmd_group_enum_type cmd_group
)
{
  if ((orig_cmd_data_ptr == NULL) || (length_ptr == NULL))
    return GSTK_BAD_PARAM;

  *length_ptr = 0;
  switch(tag)
  {
  case GSTK_NO_TAG:
    return GSTK_SUCCESS;

  case GSTK_COMMAND_DETAILS_TAG:
    *length_ptr = GSTK_COMMAND_DETAILS_LEN;
    return GSTK_SUCCESS;

  case GSTK_DEVICE_IDENTITY_TAG:
    *length_ptr = GSTK_DEVICE_IDENTITY_LEN;
    return GSTK_SUCCESS;

  case GSTK_RESULT_TAG:
    *length_ptr = GSTK_GEN_RESULT_LEN +
      orig_cmd_data_ptr->result_info_data.result_additional_info.length;
    return GSTK_SUCCESS;

  case GSTK_EVENT_LIST_TAG:
    *length_ptr = GSTK_EVENT_LIST_LEN;
    return GSTK_SUCCESS;

  case GSTK_DURATION_TAG:
  case GSTK_ALPHA_IDENTIFIER_TAG:
  case GSTK_ADDRESS_TAG:
  case GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG:
  case GSTK_CALLED_PARTY_SUB_ADDRESS_TAG:
  case GSTK_SS_STRING_TAG:
  case GSTK_USSD_STRING_TAG:
  case GSTK_SMS_TPDU_TAG:
  case GSTK_CELL_BROADCAST_PAGE_TAG:
  case GSTK_TEXT_STRING_TAG:
  case GSTK_TONE_TAG:
  case GSTK_ITEM_TAG:
  case GSTK_ITEM_IDENTIFIER_TAG:
  case GSTK_RESPONSE_LENGTH_TAG:
  case GSTK_FILE_LIST_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_LOCATION_INFORMATION_TAG:
    if (orig_cmd_data_ptr->location_info_data.location_info_tech == GSTK_GSM_UMTS_LOCATION_INFO)
    {
      *length_ptr = GSTK_LOCATION_INFORMATION_LEN +
              orig_cmd_data_ptr->location_info_data.location_info.gsm_umts_loc_info.cell_id_len
              - GSTK_MIN_CELL_ID_LEN;
      return GSTK_SUCCESS;
    }
    else if (orig_cmd_data_ptr->location_info_data.location_info_tech == GSTK_GSM_UMTS_LOCATION_INFO)
    {
      *length_ptr = GSTK_CDMA_LOCATION_INFORMATION_LEN;
      return GSTK_SUCCESS;
    }
    return GSTK_ERROR;

  case GSTK_NW_SEARCH_MODE_TAG:
    *length_ptr = GSTK_NW_SEARCH_MODE_LEN;
    return GSTK_SUCCESS;

  case GSTK_IMEI_TAG:
  case GSTK_HELP_REQUEST_TAG:
  case GSTK_NETWORK_MEASUREMENT_RESULTS_TAG:
  case GSTK_DEFAULT_TEXT_TAG:
  case GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG:
  case GSTK_CAUSE_TAG:
  case GSTK_LOCATION_STATUS_TAG:
  case GSTK_TRANSACTION_IDENTIFIER_TAG:
  case GSTK_BCCH_CHANNEL_LIST_TAG:
  case GSTK_ICON_IDENTIFIER_TAG:
  case GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG:
  case GSTK_CARD_READER_STATUS_TAG:
  case GSTK_CARD_ATR_TAG:
  case GSTK_C_APDU_TAG:
  case GSTK_R_APDU_TAG:
  case GSTK_TIMER_IDENTIFIER_TAG:
  case GSTK_TIMER_VALUE_TAG:
  case GSTK_DATE_TIME_AND_TIME_ZONE_TAG:
  case GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG:
  case GSTK_AT_COMMAND_TAG:
  case GSTK_AT_RESPONSE_TAG:
  case GSTK_BC_REPEAT_INDICATOR_TAG:
  case GSTK_IMMEDIATE_RESPONSE_TAG:
  case GSTK_DTMF_STRING_TAG:
  case GSTK_LANGUAGE_TAG:
  case GSTK_TIMING_ADVANCE_TAG:
  case GSTK_AID_TAG:
  case GSTK_BROWSER_IDENTITY_TAG:
  case GSTK_BEARER_TAG:
  case GSTK_PROVISIONING_REF_FILE_TAG:
  case GSTK_BROWSER_TERMINATION_CAUSE_TAG:
  case GSTK_BEARER_DESCRIPTION_TAG:
  case GSTK_CHANNEL_DATA_TAG:
  case GSTK_CHANNEL_DATA_LEN_TAG:
  case GSTK_CHANNEL_STATUS_TAG:
  case GSTK_BUFFER_SIZE_TAG:
  case GSTK_CARD_READER_IDENTIFIER_TAG:
  case GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG:
  case GSTK_OTHER_ADDRESS_TAG:
  case GSTK_ACCESS_TECHNOLOGY_TAG:
  case GSTK_ESN_TAG:
  case GSTK_NETWORK_ACCESS_NAME_TAG:
  case GSTK_CDMA_SMS_TPDU_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG:
#ifdef FEATURE_GPRS_CALLCONTROL
    *length_ptr = orig_cmd_data_ptr->pdp_context_act_param_data.length;
    return GSTK_SUCCESS;
#else
    return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GPRS_CALLCONTROL */
  case GSTK_EPS_PDN_CONN_PARAM_TAG:
#ifdef FEATURE_CM_LTE
    *length_ptr = orig_cmd_data_ptr->pdn_conn_act_param_data.length;
    return GSTK_SUCCESS;
#else
    return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_CM_LTE */
  case GSTK_IMS_URI_TAG:
    if(cmd_group == GSTK_ENVELOPE_CMD)
    {
      *length_ptr = orig_cmd_data_ptr->ims_call_param_data.length;
      return GSTK_SUCCESS;
    }
    else
    {
      return GSTK_UNSUPPORTED_COMMAND;
    }
  case GSTK_CSG_CELL_SLECTION_STATUS_TAG:
  case GSTK_CSG_ID_TAG:
  case GSTK_HNB_NAME_TAG:
  case GSTK_IMEISV_TAG:
  case GSTK_BATTERY_STATUS_TAG:
  case GSTK_MEID_TAG:
  case GSTK_UTRAN_MEASUREMENT_TAG:
  case GSTK_TEXT_ATTRIBUTE_TAG:
  case GSTK_FRAME_IDENTIFIER_TAG:
  case GSTK_PLMNWACT_LIST_TAG:
  case GSTK_ROUTING_AREA_TAG:
  case GSTK_UPDATE_ATTACH_TYPE_TAG:
  case GSTK_REJECTION_CAUSE_TAG:
  case GSTK_TRACKING_AREA_TAG:
  case GSTK_ACTIVATE_DESCRIPTOR_TAG:
  case GSTK_IMPU_LIST_TAG:
  case GSTK_IMS_STATUS_CODE_TAG:
  case GSTK_DNS_ADDRESS_TAG:
  case GSTK_CSG_ID_LIST_TAG:
    return GSTK_UNSUPPORTED_COMMAND;
  }
  return GSTK_ERROR;
} /* gstk_util_find_tlv_len */

/*===========================================================================
FUNCTION gstk_util_populate_tlv_value

DESCRIPTION
  This function populates the simple TLV's Value.

PARAMETERS
  tag:               [Input] Simple TLV Tag
  orig_cmd_data_ptr: [Input] Original Command Data from which the info
                     is copied from
  new_data_ptr:      [Input/Output] pointer to the new buffer where the
                     data will be copied into
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
static gstk_status_enum_type gstk_util_populate_tlv_value (
  tags_enum_type              tag,
  const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
  uint8                      *new_data_ptr,
  int32                       dest_size,
  gstk_cmd_group_enum_type    cmd_group
)
{
  UIM_MSG_HIGH_1("Populate TLV Value 0x%x", tag);

  if ((orig_cmd_data_ptr == NULL) || (new_data_ptr == NULL))
  {
    UIM_MSG_ERR_2("Null Input Ptr: orig ptr 0x%x, new ptr 0x%x",
                  orig_cmd_data_ptr, new_data_ptr);
    return GSTK_BAD_PARAM;
  }

  switch(tag)
  {
  case GSTK_NO_TAG:
    return GSTK_SUCCESS;

  case GSTK_COMMAND_DETAILS_TAG:
    return gstk_packer_cmd_details_card_tlv(
             orig_cmd_data_ptr->cmd_info_data,
             new_data_ptr);

  case GSTK_DEVICE_IDENTITY_TAG:
    return gstk_packer_dev_id_card_tlv(
             &orig_cmd_data_ptr->dev_id_data,
             new_data_ptr);

  case GSTK_RESULT_TAG:
    return gstk_packer_result_card_tlv(
             orig_cmd_data_ptr->result_info_data,
             new_data_ptr);

  case GSTK_EVENT_LIST_TAG:
    return gstk_packer_evt_list_card_tlv(
             orig_cmd_data_ptr->evt_list_info_data,
             new_data_ptr);

  case GSTK_DURATION_TAG:
  case GSTK_ALPHA_IDENTIFIER_TAG:
  case GSTK_ADDRESS_TAG:
  case GSTK_CAPABILITY_CONFIGURATION_PARAMETERS_TAG:
  case GSTK_CALLED_PARTY_SUB_ADDRESS_TAG:
  case GSTK_SS_STRING_TAG:
  case GSTK_USSD_STRING_TAG:
  case GSTK_SMS_TPDU_TAG:
  case GSTK_CELL_BROADCAST_PAGE_TAG:
  case GSTK_TEXT_STRING_TAG:
  case GSTK_TONE_TAG:
  case GSTK_ITEM_TAG:
  case GSTK_ITEM_IDENTIFIER_TAG:
  case GSTK_RESPONSE_LENGTH_TAG:
  case GSTK_FILE_LIST_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_LOCATION_INFORMATION_TAG:
    return gstk_packer_location_info_card_tlv(
             &orig_cmd_data_ptr->location_info_data,
             new_data_ptr,
             dest_size);

  case GSTK_IMEI_TAG:
  case GSTK_HELP_REQUEST_TAG:
  case GSTK_NETWORK_MEASUREMENT_RESULTS_TAG:
  case GSTK_DEFAULT_TEXT_TAG:
  case GSTK_ITEMS_NEXT_ACTION_INDICATOR_TAG:
  case GSTK_CAUSE_TAG:
  case GSTK_LOCATION_STATUS_TAG:
  case GSTK_TRANSACTION_IDENTIFIER_TAG:
  case GSTK_BCCH_CHANNEL_LIST_TAG:
  case GSTK_ICON_IDENTIFIER_TAG:
  case GSTK_ITEM_ICON_IDENTIFIER_LIST_TAG:
  case GSTK_CARD_READER_STATUS_TAG:
  case GSTK_CARD_ATR_TAG:
  case GSTK_C_APDU_TAG:
  case GSTK_R_APDU_TAG:
  case GSTK_TIMER_IDENTIFIER_TAG:
  case GSTK_TIMER_VALUE_TAG:
  case GSTK_DATE_TIME_AND_TIME_ZONE_TAG:
  case GSTK_CALL_CONTROL_REQUESTED_ACTION_TAG:
  case GSTK_AT_COMMAND_TAG:
  case GSTK_AT_RESPONSE_TAG:
  case GSTK_BC_REPEAT_INDICATOR_TAG:
  case GSTK_IMMEDIATE_RESPONSE_TAG:
  case GSTK_DTMF_STRING_TAG:
  case GSTK_LANGUAGE_TAG:
  case GSTK_TIMING_ADVANCE_TAG:
  case GSTK_AID_TAG:
  case GSTK_BROWSER_IDENTITY_TAG:
  case GSTK_BEARER_TAG:
  case GSTK_PROVISIONING_REF_FILE_TAG:
  case GSTK_BROWSER_TERMINATION_CAUSE_TAG:
  case GSTK_BEARER_DESCRIPTION_TAG:
  case GSTK_CHANNEL_DATA_TAG:
  case GSTK_CHANNEL_DATA_LEN_TAG:
  case GSTK_CHANNEL_STATUS_TAG:
  case GSTK_BUFFER_SIZE_TAG:
  case GSTK_CARD_READER_IDENTIFIER_TAG:
  case GSTK_USIM_ME_INTERFACE_TRANSPORT_LEVEL_TAG:
  case GSTK_OTHER_ADDRESS_TAG:
  case GSTK_ACCESS_TECHNOLOGY_TAG:
  case GSTK_ESN_TAG:
  case GSTK_NETWORK_ACCESS_NAME_TAG:
  case GSTK_CDMA_SMS_TPDU_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_PDP_CONTEXT_ACTIVATION_PARAM_TAG:
#ifdef FEATURE_GPRS_CALLCONTROL
    return gstk_packer_pdp_context_act_param_card_tlv(
             &orig_cmd_data_ptr->pdp_context_act_param_data,
             new_data_ptr);
#else
    return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_GPRS_CALLCONTROL */

  case GSTK_EPS_PDN_CONN_PARAM_TAG:
#ifdef FEATURE_CM_LTE
    return gstk_packer_pdn_conn_act_param_card_tlv(
             &orig_cmd_data_ptr->pdn_conn_act_param_data,
             new_data_ptr);
#else
    return GSTK_UNSUPPORTED_COMMAND;
#endif /* FEATURE_CM_LTE */

  case GSTK_IMS_URI_TAG:
    if(cmd_group == GSTK_ENVELOPE_CMD)
    {
      return gstk_packer_ims_call_param_card_tlv(
               &orig_cmd_data_ptr->ims_call_param_data,
               new_data_ptr);
    }
    else
    {
      return GSTK_UNSUPPORTED_COMMAND;
    }
  case GSTK_IMEISV_TAG:
  case GSTK_BATTERY_STATUS_TAG:
  case GSTK_MEID_TAG:
  case GSTK_UTRAN_MEASUREMENT_TAG:
  case GSTK_TEXT_ATTRIBUTE_TAG:
  case GSTK_FRAME_IDENTIFIER_TAG:
  case GSTK_PLMNWACT_LIST_TAG:
  case GSTK_ROUTING_AREA_TAG:
  case GSTK_UPDATE_ATTACH_TYPE_TAG:
  case GSTK_REJECTION_CAUSE_TAG:
  case GSTK_TRACKING_AREA_TAG:
  case GSTK_ACTIVATE_DESCRIPTOR_TAG:
  case GSTK_CSG_CELL_SLECTION_STATUS_TAG:
  case GSTK_CSG_ID_TAG:
  case GSTK_HNB_NAME_TAG:
  case GSTK_IMPU_LIST_TAG:
  case GSTK_IMS_STATUS_CODE_TAG:
  case GSTK_DNS_ADDRESS_TAG:
  case GSTK_CSG_ID_LIST_TAG:
    return GSTK_UNSUPPORTED_COMMAND;

  case GSTK_NW_SEARCH_MODE_TAG:
    return gstk_packer_nw_search_mode_card_tlv(
             orig_cmd_data_ptr->nw_search_mode_data,
             new_data_ptr);

  }
  return GSTK_ERROR;
} /* gstk_util_populate_tlv_value */


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
gstk_status_enum_type gstk_util_populate_tlv (
  tags_enum_type              tag,
  int32                       orig_offset,
  const gstk_cmd_to_uim_type *orig_cmd_data_ptr,
  uint8                      *new_data_ptr,
  int32                      *new_offset_ptr,
  gstk_cmd_group_enum_type    cmd_group
)
{
  gstk_status_enum_type gstk_status = GSTK_SUCCESS;
  int32                 length = 0;
  int32                 new_offset = orig_offset;

  UIM_MSG_HIGH_1("Populate TLV 0x%x", tag);

  if ((new_data_ptr == NULL) ||
      (new_offset_ptr == NULL) ||
      (orig_cmd_data_ptr == NULL))
  {
    UIM_MSG_ERR_3("Null Input Pointers: new data=0x%x, new offset=0x%x, orig data=0x%x",
                  new_data_ptr, new_offset_ptr, orig_cmd_data_ptr);
    return GSTK_BAD_PARAM;
  }

  new_data_ptr[new_offset++] = (uint8)tag;

  gstk_status = gstk_util_find_tlv_len(tag, orig_cmd_data_ptr, &length, cmd_group);
  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Populate TLV Len fail 0x%x", gstk_status);
    return gstk_status;
  }

  if (length > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
  {
    new_data_ptr[new_offset++] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
    new_data_ptr[new_offset++] = int32touint8(length);
  }
  else
  {
    new_data_ptr[new_offset++] = int32touint8(length);
  }

  gstk_status = gstk_util_populate_tlv_value(
                              tag,
                              orig_cmd_data_ptr,
                              &new_data_ptr[new_offset],
                              UIM_MAX_TR_BYTES - new_offset,
                              cmd_group);
  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Populate TLV Value fail 0%x", gstk_status);
    return gstk_status;
  }

  if (length > 0)
  {
    new_offset += length;
  }
  *new_offset_ptr = new_offset;
  return GSTK_SUCCESS;
} /*gstk_util_populate_tlv */


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
gstk_status_enum_type gstk_util_populate_uim_cmd_total_data_len (
  int32         ber_tlv_tag,
  int32         total_data_len,
  uim_cmd_type *uim_cmd_ptr
)
{
  UIM_MSG_HIGH_2("Populate UIM Cmd Total Data Len BerTLV=0x%x, Len=0x%x",
                 ber_tlv_tag, total_data_len);

  if (uim_cmd_ptr == NULL)
    return GSTK_BAD_PARAM;

  switch (ber_tlv_tag)
  {
  case GSTK_PROACTIVE_SIM_COMMAND_TAG:
    return GSTK_ERROR;
  case GSTK_TERMINAL_RESPONSE_TAG:
    /* to do */
    return GSTK_SUCCESS;
  case GSTK_SMS_PP_DOWNLOAD_TAG:
  case GSTK_CELL_BROADCAST_DOWNLOAD_TAG:
  case GSTK_MENU_SELECTION_TAG:
  case GSTK_CALL_CONTROL_TAG:
  case GSTK_MO_SHORT_MESSAGE_CONTROL_TAG:
  case GSTK_EVENT_DOWNLOAD_TAG:
  case GSTK_TIMER_EXPIRY_TAG:
    if (total_data_len > GSTK_TLV_LENGTH_1_BYTE_OFFSET_LIMIT)
    {
      /* account for 0x80 */
      uim_cmd_ptr->envelope.offset = 0;
      uim_cmd_ptr->envelope.data[0] = int32touint8(ber_tlv_tag);
      uim_cmd_ptr->envelope.data[1] = GSTK_2_BYTE_LENGTH_FIRST_VALUE;
      uim_cmd_ptr->envelope.data[2] = int32touint8(total_data_len);
      uim_cmd_ptr->envelope.num_bytes = int32touint8(total_data_len + GSTK_TAG_LENGTH_LEN + 1);

    }
    else {
      uim_cmd_ptr->envelope.offset = 1;
      uim_cmd_ptr->envelope.data[1] = int32touint8(ber_tlv_tag);
      uim_cmd_ptr->envelope.data[2] = int32touint8(total_data_len);
      uim_cmd_ptr->envelope.num_bytes = int32touint8(total_data_len + GSTK_TAG_LENGTH_LEN);
    }
    return GSTK_SUCCESS;
  default:
    return GSTK_ERROR;
  }
} /* gstk_util_populate_uim_cmd_total_data_len */

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
gstk_status_enum_type gstk_util_populate_term_rsp_common_tlv(
  int32                         orig_offset,
  uint32                        *total_len_ptr,
  gstk_cmd_term_rsp_common_type gstk_tlv,
  byte                          *uim_cmd_ptr
)
{
  gstk_device_id_type                           dev_id;
  int32                                         offset = 0;
  gstk_status_enum_type                         gstk_status = GSTK_SUCCESS;

  offset = orig_offset;

  if(uim_cmd_ptr == NULL || total_len_ptr == NULL)
  {
    UIM_MSG_ERR_2("Bad Parameters : uim_cmd_ptr ptr is 0x%x, total_len_ptr is 0x%x",
                  uim_cmd_ptr, total_len_ptr);
    return GSTK_BAD_PARAM;
  }

  UIM_MSG_HIGH_1("In gstk_util_populate_term_rsp_common_tlv(), Initial total_len_ptr value is 0x%x",
                 *total_len_ptr);

  gstk_status = gstk_util_populate_tlv(
                  GSTK_COMMAND_DETAILS_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*)&(gstk_tlv.cmd_info_data),
                  uim_cmd_ptr,
                  &offset,
                  GSTK_TERMINAL_RSP); /*lint !e826 cmd_info_data suppress area too small */

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("Error in populating Command Details TLV");
    return GSTK_ERROR;
  }

  *total_len_ptr = *total_len_ptr +
               GSTK_COMMAND_DETAILS_LEN +
               GSTK_TAG_LENGTH_LEN;

  /* device ID */
  dev_id.src = GSTK_ME_DEVICE;
  dev_id.dest = GSTK_UICC_SIM_DEVICE;

  gstk_status = gstk_util_populate_tlv(
                  GSTK_DEVICE_IDENTITY_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*)&dev_id,
                  uim_cmd_ptr,
                  &offset,
                  GSTK_TERMINAL_RSP);/*lint !e826 area too small */

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("Error in populating Device Identity TLV");
    return GSTK_ERROR;
  }

  *total_len_ptr = *total_len_ptr +
               GSTK_DEVICE_IDENTITY_LEN +
               GSTK_TAG_LENGTH_LEN;

  /* result tag */
  gstk_status = gstk_util_populate_tlv(
                  GSTK_RESULT_TAG,
                  offset,
                  (gstk_cmd_to_uim_type*) &(gstk_tlv.result_info_data),
                  uim_cmd_ptr,
                  &offset,
                  GSTK_TERMINAL_RSP);/*lint !e826 area too small */

  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_HIGH_0("Error in populating Result TLV");
    return GSTK_ERROR;
  }

  *total_len_ptr = *total_len_ptr +
               GSTK_GEN_RESULT_LEN +
               gstk_tlv.result_info_data.result_additional_info.length +
               GSTK_TAG_LENGTH_LEN;

  return GSTK_SUCCESS;
} /* gstk_util_populate_term_rsp_common_tlv */

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
boolean gstk_util_is_envelope_rsp_evt_dl(uint32 command_id)
{
  UIM_MSG_HIGH_1("In gstk_util_is_envelope_rsp_evt_dl() command_id is 0x%x",
                 command_id);
  switch(command_id)
  {
  case GSTK_MT_CALL_EVT_IND_RSP:
  case GSTK_CALL_CONNECTED_EVT_IND_RSP:
  case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
  case GSTK_IDLE_SCRN_EVT_IND_RSP:
  case GSTK_LOCATION_STATUS_EVT_IND_RSP:
  case GSTK_USER_ACTIVITY_EVT_IND_RSP:
  case GSTK_LANG_SELECTION_EVT_IND_RSP:
  case GSTK_ACCESS_TECH_EVT_IND_RSP:
  case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
  case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
  case GSTK_DATA_AVAIL_EVT_IND_RSP:
  case GSTK_NW_REJ_EVT_IND_RSP:
  case GSTK_IMS_REG_EVT_IND_RSP:
    return TRUE;
  default:
    return FALSE;
  }
}/* gstk_util_is_envelope_rsp_evt_dl */

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
boolean gstk_util_is_envelope_rsp_loc_info(uint32 command_id)
{
  UIM_MSG_HIGH_1("In gstk_util_is_envelope_rsp_loc_info(), command_id 0x%x",
                 command_id);
  if(command_id == GSTK_LOC_INFO_ENVELOPE_RSP)
  {
    return TRUE;
  }
  return FALSE;
} /* gstk_util_is_envelope_rsp_loc_info */

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
  Currently this function will be called to retry event download, CC envelopes.
  The calling function should check the gstk envelope state to see if it is
  set to GSTK_SUB_ST_PENDING_ENV_IN_QUEUE before calling this
  command to retry the event download envelope.

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_util_retry_envelope_cmd(void)
{
  uim_cmd_type           *uim_cmd_ptr = NULL;
  gstk_status_enum_type   gstk_status = GSTK_SUCCESS;
  uint32                  copy_len    = 0;
  gstk_nv_items_data_type nv_data;

  memset(&nv_data, 0, sizeof(nv_data));

  UIM_MSG_HIGH_0("In gstk_util_retry_envelope_cmd()");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  (void)gstk_nv_access_read(GSTK_NV_SLOT_TOOLKIT_ENV_RETRY_EXT_DATA,
                          gstk_curr_inst_ptr->slot_id,
                          &nv_data);
                            
  /* Clear timer before sending the envelope */
  (void)rex_clr_timer(&gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_retry_timer);

  if(gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state !=
       GSTK_SUB_ST_PENDING_ENV_IN_QUEUE)
  {
    UIM_MSG_HIGH_0("Incorrect GSTK env state to retry envelope");
    return GSTK_ERROR;
  }
  if(!gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr ||
     gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_len > UIM_MAX_ENVELOPE_BYTES)
  {
    UIM_MSG_HIGH_0("env retry buffer ptr NULL or data len out of range");
    return GSTK_ERROR;
  }
  if ((gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id == GSTK_CMD_DETAILS_REF_TABLE_FULL) ||
      (gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id > (GSTK_MAX_PROACTIVE_COMMAND-1)))
  {
    UIM_MSG_ERR_0("Invalid env ref id");
    return GSTK_ERROR;
  }

  uim_cmd_ptr = gstk_get_uim_buf();
  if (uim_cmd_ptr == NULL )
  {
    return GSTK_ERROR;
  }
  gstk_status = gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
  if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))
  {
   /* set to 0x00 so that uim will get the data from index 0 */
   uim_cmd_ptr->envelope.offset    = 0x00;
   /* set user_data */
   uim_cmd_ptr->hdr.user_data      = gstk_curr_inst_ptr->gstk_curr_envelope_info.env_ref_id;
   /* populate the envelope command info */
   uim_cmd_ptr->envelope.num_bytes =
     int32touint8(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_len);
   
   copy_len = uim_cmd_ptr->envelope.num_bytes;

   if(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr[0] == 0x00)
   {
     uim_cmd_ptr->envelope.offset = 1;
     copy_len = uim_cmd_ptr->envelope.num_bytes + 1;
   }
   /* copy data */
   if(gstk_memcpy(uim_cmd_ptr->envelope.data,
                  gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr,
                  copy_len,
                  UIM_MAX_ENVELOPE_BYTES,
                  copy_len) <
      int32touint32(gstk_curr_inst_ptr->gstk_curr_envelope_info.env_data_bytes.data_len))
   {
     UIM_MSG_ERR_0("gstk_util_retry_envelope_cmd(): gstk_memcpy failed");
     gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
     return GSTK_MEMORY_ERROR;
   }

   gstk_util_dump_byte_array("Envelope Retry",
                             &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                             uim_cmd_ptr->envelope.num_bytes);   
   gstk_send_cmd_to_uim_server(uim_cmd_ptr); /* clear signal and call uim_cmd */
   if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_counter <
        nv_data.env_retry_ext.max_retries)
   {
     /* If retry_counter reaches max, don't increment and set it max to retry in other 
        cases like fetch and end of the proactive session and not to start the retry timer */
     gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_counter++;
   }
   gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
  }/* if ((gstk_status == GSTK_SUCCESS) && (uim_cmd_ptr != NULL))  */
  else
  {
     UIM_MSG_ERR_2("Fail to send retry envelope command gstk_status:0x%x, cmd_ptr:0x%x",
                    gstk_status, uim_cmd_ptr);
     return GSTK_ERROR;
  }
  return GSTK_SUCCESS;
}/* gstk_util_retry_envelope_cmd */

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
void gstk_util_clear_retry_envelope_data(gstk_slot_id_enum_type slot)
{
  UIM_MSG_HIGH_1("In gstk_util_clear_retry_envelope_data(): 0x%x", slot);

  if (!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  (void)rex_clr_timer(&gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.gstk_env_retry_timer);
  gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_NONE;
  gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.env_ref_id     = 0;
  gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.retry_info.retry_counter     = 0;
  gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.retry_info.cmd_type          = GSTK_CMD_ENUM_NOT_USE;
  if((gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.env_data_bytes.data_len != 0)
     &&
     gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr)
  {
    gstk_free((void*)gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr);
    gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.env_data_bytes.data_buffer_ptr = NULL;
    gstk_instances_ptr[(uint8)slot - 1]->gstk_curr_envelope_info.env_data_bytes.data_len        = 0;
  }
  else
  {
    UIM_MSG_HIGH_0("NULL ptr or len is zero ");
  }
}/* gstk_util_clear_retry_envelope_data */


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
gstk_status_enum_type gstk_cm_ph_reg_dereg(
  int32             evt_flg,
  cm_ph_event_e_type start_ph_evt,
  cm_ph_event_e_type end_ph_evt
)
{
  cm_client_status_e_type             cm_reg_return_status;
  gstk_status_enum_type               gstk_status = GSTK_SUCCESS;

  if (evt_flg == 0x01)
  {
    UIM_MSG_HIGH_2("... Register PH event with CM 0x%x, 0x%x",
                   start_ph_evt, end_ph_evt);
    cm_reg_return_status = cm_client_ph_reg(
      gstk_shared_data.gstk_client_id_from_cm,               /* pointer to the client struct */
      gstk_cm_ph_event_cb,                  /* pointer to a cb function for ss events */
      CM_CLIENT_EVENT_REG,                  /* event registration */
      start_ph_evt,                         /* from this ph event */
      end_ph_evt,                           /* to this ph event */
      NULL );                        /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_0("REGISTRATION UNSUCCESSFUL WITH CM");
      gstk_status = GSTK_ERROR;
    }
  }
  else
  {
    UIM_MSG_HIGH_2("... Deregister PH event with CM 0x%x, 0x%x",
                   start_ph_evt, end_ph_evt);
    cm_reg_return_status = cm_client_ph_reg(
      gstk_shared_data.gstk_client_id_from_cm,               /* pointer to the client struct */
      gstk_cm_ph_event_cb,                  /* pointer to a cb function for ss events */
      CM_CLIENT_EVENT_DEREG,                /* event registration */
      start_ph_evt,                         /* from this ph event */
      end_ph_evt,                           /* to this ph event */
      NULL );                        /* pointer to a cb function for errors */                               /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_0("DEREGISTRATION UNSUCCESSFUL WITH CM");
      gstk_status = GSTK_ERROR;
    }
  }
  return gstk_status;
} /* gstk_cm_ph_reg_dereg */


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
void gstk_util_cmd_details_ref_table_free_all_slots(void)
{
  int i = 0;

  for (i = 0; i < GSTK_MAX_PROACTIVE_COMMAND; i++)
  {
    if ( command_details_ref_table_ptr[i].free_slot == FALSE)
    {
      gstk_util_cmd_details_ref_table_free_slot(i);
    }
  }
} /* gstk_util_cmd_details_ref_table_free_all_slots */

/*===========================================================================
FUNCTION GSTK_FIND_RECOVERY_END_CMD

DESCRIPTION
  This function is called by q_linear_delete_new. This function will return TRUE only when the
  command id is that of a GSTK_RECOVERY_END_CMD.

PARAMETER
  param_ptr [Input]:  ptr to seq num

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int gstk_find_recovery_end_cmd(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type     *task_cmd_ptr	 = NULL;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_ID_MAX;

  if(item_ptr == NULL)
  {
    UIM_MSG_HIGH_0("item_ptr is NULL");
    return 0;
  }

  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);

  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_RECOVERY_CMD &&
     task_cmd_ptr->cmd.general_cmd.message_header.command_id == GSTK_RECOVERY_END_CMD &&
     task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id == slot )
  {
    UIM_MSG_HIGH_0("GSTK_RECOVERY_END_CMD found for deletion");
    return 1;
  }
  else
  {
    return 0;
  }
} /* gstk_find_recovery_end_cmd */

/*===========================================================================
FUNCTION GSTK_FIND_AND_DELETE_RECOVERY_END_CMD

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
void gstk_util_find_and_delete_recovery_end_cmd(void)
{
  gstk_task_cmd_type	   *task_cmd_ptr	 = NULL;

  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  while( (task_cmd_ptr = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                 gstk_find_recovery_end_cmd,
                                                 NULL,
                                                 NULL,
                                                 NULL)) != NULL)
  {
    gstk_task_free_cmd_buf(task_cmd_ptr);
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* Leaving gstk critical section */
} /* gstk_util_find_and_delete_recovery_end_cmd */


/*===========================================================================
FUNCTION GSTK_FIND_END_PROACTIVE_CMD

DESCRIPTION
  This function is called by q_linear_delete_new. This function will return TRUE only when the
  command id is that of a GSTK_RECOVERY_END_CMD.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete_new()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int gstk_find_end_proactive_cmd(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type	   *task_cmd_ptr	 = NULL;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_ID_MAX;

  if(item_ptr == NULL)
  {
    UIM_MSG_HIGH_0("item_ptr is NULL");
    return 0;
  }
  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_PROACTIVE_CMD &&
     task_cmd_ptr->cmd.general_cmd.message_header.command_id == GSTK_END_PROACTIVE_CMD_REQ &&
     task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id == slot)
  {
    return 1;
  }
  else
  {
    return 0;
  }
} /* gstk_find_end_proactive_cmd */


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
void gstk_util_find_and_build_proactive_end_session(
  gstk_tag_enum_type gstk_tag
)
{
  gstk_proactive_cmd_type  cmd;
  gstk_task_cmd_type      *cmd_p = NULL;

  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* find end proactive session in the gstk_cmd_q, if no simulate one */
  cmd_p = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                gstk_find_end_proactive_cmd,
                                                NULL,
                                                NULL,
                                                NULL);
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* Leaving gstk critical section */

  if(cmd_p == NULL)
  {
    cmd.data[GSTK_GENERATED_END_SES_INDEX] = GSTK_GENERATED_END_SES;
    (void)gstk_end_proactive_session(&cmd,gstk_tag);
  }
  else
  {
    if(gstk_process_state(&(cmd_p->cmd)) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Err in processing END Proactive Session Cmd during recovery or card error");
    }
    gstk_task_free_cmd_buf(cmd_p);
  }
} /* gstk_util_find_and_build_proactive_end_session */

/*===========================================================================
FUNCTION GSTK_FIND_PENDING_RSP

DESCRIPTION
  This function is called by q_linear_delete_new. This function will return TRUE only when the
  command id is that of a GSTK_RECOVERY_END_CMD.

PARAMETER
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete_new()

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if there is a match
  FALSE - if there is no match

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static int gstk_find_pending_rsp(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type      *task_cmd_ptr	 = NULL;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_ID_MAX;
  if(item_ptr == NULL)
  {
    UIM_MSG_HIGH_0("item_ptr is NULL");
    return 0;
  }
  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  if(task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id == slot)
  {
    switch (task_cmd_ptr->cmd.general_cmd.message_header.command_group)
    {
      case GSTK_ENVELOPE_RSP:
      case GSTK_TR_REPORT:
        return 1;
        break;
      default:
        return 0;
        break;
    }
  }
  else
  {
    return 0;
  }
} /* gstk_find_pending_rsp */


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
void gstk_util_find_and_process_pending_responses(void)
{
  gstk_task_cmd_type   *cmd_p = NULL;

  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* find end proactive session in the gstk_cmd_q, if no simulate one */
  while((cmd_p = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                     gstk_find_pending_rsp,
                                                     NULL,
                                                     NULL,
                                                     NULL)) != NULL)
  {
    if(gstk_process_state(&(cmd_p->cmd)) != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Err in processing envelope response during recovery or card error");
    }
    gstk_task_free_cmd_buf(cmd_p);
  }
  /* Leaving gstk critical section */
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
}/* gstk_util_find_and_process_pending_responses */

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
gstk_status_enum_type gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
  uint8               stk_cmd_type,
  uint8               stk_cmd_qual,
  gstk_cmd_enum_type *cmd_type_ptr)
{
  gstk_nv_items_data_type nv_data;

  UIM_MSG_HIGH_0("In gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type()");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(cmd_type_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_type_ptr is NULL!");
    return GSTK_NULL_INPUT_PARAM;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);

  switch(stk_cmd_type)
  {
  case GSTK_CMD_STK_POLL_INTERVAL:
    *cmd_type_ptr = GSTK_POLL_INTERVAL_REQ;
    break;

  case GSTK_CMD_STK_POLLING_OFF:
    *cmd_type_ptr = GSTK_POLLING_OFF_REQ;
    break;

  case GSTK_CMD_STK_SET_UP_EVENT_LIST:
    *cmd_type_ptr = GSTK_SETUP_EVENT_LIST_REQ;
    break;

  case GSTK_CMD_STK_SET_UP_CALL:
    *cmd_type_ptr = GSTK_SETUP_CALL_REQ;
    break;

  case GSTK_CMD_STK_SEND_SS:
    *cmd_type_ptr = GSTK_SEND_SS_REQ;
    break;

  case GSTK_CMD_STK_USSD:
    *cmd_type_ptr = GSTK_SEND_USSD_REQ;
    break;

  case GSTK_CMD_STK_LAUNCH_BROWSER:
    *cmd_type_ptr = GSTK_LAUNCH_BROWSER_REQ;
    break;

  case GSTK_CMD_STK_SEND_SHORT_MESSAGE:
    *cmd_type_ptr = GSTK_SEND_SMS_REQ;
    break;

  case GSTK_CMD_STK_PLAY_TONE:
    *cmd_type_ptr = GSTK_PLAY_TONE_REQ;
    break;

  case GSTK_CMD_STK_DISPLAY_TEXT:
    *cmd_type_ptr = GSTK_DISPLAY_TEXT_REQ;
    break;

  case GSTK_CMD_STK_GET_INKEY:
    *cmd_type_ptr = GSTK_GET_INKEY_REQ;
    break;

  case GSTK_CMD_STK_GET_INPUT:
    *cmd_type_ptr = GSTK_GET_INPUT_REQ;
    break;

  case GSTK_CMD_STK_SELECT_ITEM:
    *cmd_type_ptr = GSTK_SELECT_ITEM_REQ;
    break;

  case GSTK_CMD_STK_SET_UP_MENU:
    *cmd_type_ptr = GSTK_SETUP_MENU_REQ;
    break;

  case GSTK_CMD_STK_PROVIDE_LOCAL_INFO:
    switch (stk_cmd_qual)
    {
      case GSTK_LANGUAGE_SETTING:
        /* Change expected cmd req type for Language provide local info */
        *cmd_type_ptr = GSTK_PROVIDE_LANG_INFO_REQ;
        break;
      case GSTK_BATTERY_STATUS:
        if(nv_data.cat_version >= GSTK_CFG_CAT_VER6)
        {
          /* Change expected cmd req type for Battery Status */
          *cmd_type_ptr = GSTK_PROVIDE_BATTERY_STATUS_REQ;
        }
        else
        {
          UIM_MSG_HIGH_1("GSTK_BATTERY_STATUS: CAT version is pre-VER6 : 0x%x",
                         nv_data.cat_version);
        }
        break;
      case GSTK_DATE_TIME_ZONE:
        *cmd_type_ptr = GSTK_PROVIDE_DATE_TIME_REQ;
        break;
      default:
        *cmd_type_ptr =  GSTK_PROVIDE_LOCAL_INFO_REQ;
        break;
    }/* switch (stk_cmd_qual) */
    break;

  case GSTK_CMD_STK_SET_UP_IDLE_MODE_TEXT:
    *cmd_type_ptr = GSTK_SETUP_IDLE_TEXT_REQ;
    break;

  case GSTK_CMD_STK_REFRESH:
    *cmd_type_ptr = GSTK_REFRESH_REQ ;
    break;

  case GSTK_CMD_STK_MORE_TIME:
    *cmd_type_ptr = GSTK_MORE_TIME_REQ;
    break;

  case GSTK_CMD_STK_TIMER_MANAGEMENT:
    *cmd_type_ptr =  GSTK_TIMER_MANAGEMENT_REQ;
    break;

  case GSTK_CMD_STK_LANG_NOTIFICATION:
    *cmd_type_ptr = GSTK_LANG_NOTIFICATION_REQ;
    break;

  case GSTK_CMD_STK_SEND_DTMF:
    *cmd_type_ptr = GSTK_SEND_DTMF_REQ;
    break;

  case GSTK_CMD_STK_OPEN_CHANNEL:
    *cmd_type_ptr = GSTK_OPEN_CH_REQ;
    break;

  case GSTK_CMD_STK_CLOSE_CHANNEL:
    *cmd_type_ptr = GSTK_CLOSE_CH_REQ;
    break;

  case GSTK_CMD_STK_RECEIVE_DATA:
    *cmd_type_ptr = GSTK_RECEIVE_DATA_REQ;
    break;

  case GSTK_CMD_STK_SEND_DATA:
    *cmd_type_ptr = GSTK_SEND_DATA_REQ;
    break;

  case GSTK_CMD_STK_GET_CHANNEL_STATUS:
    *cmd_type_ptr = GSTK_GET_CH_STATUS_REQ;
    break;

  case GSTK_CMD_STK_RUN_AT_COMMAND:
  case GSTK_CMD_STK_PERFORM_CARD_APDU:
  case GSTK_CMD_STK_POWER_ON_CARD:
  case GSTK_CMD_STK_POWER_OFF_CARD:
  case GSTK_CMD_STK_GET_READER_STATUS:
    *cmd_type_ptr = GSTK_CMD_ENUM_NOT_USE;
    return GSTK_ERROR;

  case GSTK_CMD_STK_ACTIVATE:
    *cmd_type_ptr = GSTK_ACTIVATE_REQ;
    break;

  case GSTK_CMD_STK_END_OF_PROACTIVE_SES:
    *cmd_type_ptr = GSTK_END_PROACTIVE_CMD_REQ;
    break;

  default:
    UIM_MSG_ERR_1("STK cmd (0x%x) is unknown!", stk_cmd_type);
    *cmd_type_ptr = GSTK_CMD_ENUM_NOT_USE;
    return GSTK_ERROR;
  }

  return GSTK_SUCCESS;
} /* gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type */

/*===========================================================================
FUNCTION gstk_util_call_client_evt_cb

DESCRIPTION
  Based on the type of the client, this function packs the right format of the
  arguments and invokes the event callback function registerd by the client.

  This function is only applicable to GSTK messages. Raw proactive commands are
  handled in gstk_process_proactive_command().

PARAMETERS
  client_id:  [Input] The ID of the client whose event call back function will
                      be invoked.
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
void gstk_util_call_client_evt_cb(
  gstk_client_id_type      client_id,
  gstk_cmd_from_card_type *message_ptr
)
{
  gstk_client_pro_cmd_reg_data_type pro_cmd_data;
  uint32                            i           = client_id - 1;
  uint32                            cmd_ref_index  = 0;

  GSTK_RETURN_IF_NULL_PTR(command_details_ref_table_ptr)

  if (message_ptr == NULL)
  {
    UIM_MSG_ERR_0("null message_ptr in gstk_util_call_client_evt_cb().");
    return;
  }

  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("invalid client id 0x%x", client_id);
    return;
  }

  message_ptr->hdr_cmd.user_data = gstk_shared_data.gstk_client_table[client_id-1].user_data;

  pro_cmd_data.data_format_type = GSTK_GSTK_FORMAT;
  pro_cmd_data.pro_cmd_reg_data.gstk_req_rsp_ptr = message_ptr;
  if (gstk_shared_data.gstk_client_table[i].client_cb != NULL)
  {
    (gstk_shared_data.gstk_client_table[i].client_cb)(&pro_cmd_data);
    /* Cache the proactive command if the client type is GSTK_WAKEUP_TYPE*/
    if (gstk_shared_data.gstk_client_table[i].client_type == GSTK_WAKEUP_TYPE)
    {
      cmd_ref_index = GSTK_CMD_REF_TABLE_INDEX(message_ptr->hdr_cmd.cmd_detail_reference);
      if (message_ptr->hdr_cmd.command_id != GSTK_END_PROACTIVE_CMD_REQ)
      {
        /* Allocate memory for cmd_ptr in command_details_ref_table */
        command_details_ref_table_ptr[cmd_ref_index].cmd_ptr =
          GSTK_CALLOC(sizeof(gstk_cmd_from_card_type));

        if (command_details_ref_table_ptr[cmd_ref_index].cmd_ptr != NULL)
        {
          /* Shallow copy */
          (void)memscpy (command_details_ref_table_ptr[cmd_ref_index].cmd_ptr,
                  sizeof (gstk_cmd_from_card_type),
                  message_ptr,
                  sizeof (gstk_cmd_from_card_type) );
          /* To make sure pointers are not freed in gstk_util_release_card_cmd_memory*/
          gstk_curr_inst_ptr->gstk_is_msg_cached = TRUE;
        }
      }
    }
  }
  else
  {
    UIM_MSG_ERR_1("gstk_shared_data.gstk_client_table[%d].client_cb == NULL!",
                  i);
  }
} /* gstk_util_call_client_evt_cb */

/*===========================================================================
FUNCTION gstk_util_dump_byte_array

DESCRIPTION
  This function dumps byte array in the format of three item per line.

PARAMETERS
  array_name_ptr:  [Input] The name of the array to be dumped
  byte_array_ptr:  [Input] The pointer to the byte array to be dumped
  array_size:      [Input] The size of the byte array

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
void gstk_util_dump_byte_array(
  const char  *array_name_ptr,
  const byte  *byte_array_ptr,
  uint32      array_size
)
{
  uint32  i       = 0;
  uint32  idx     = 0;

  if (array_name_ptr == NULL || byte_array_ptr == NULL)
  {
    UIM_MSG_ERR_0("null ptr pass into gstk_util_dump_byte_array().");
    return;
  }

  MSG_SPRINTF_2(
    MSG_SSID_RUIM,
    MSG_LEGACY_HIGH,
    "Dumping array %s array size 0x%x",
    array_name_ptr, array_size);

  while (i < (array_size / 9))
  {
    idx = i * 9;
    UIM_MSG_HIGH_9("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx+1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3],
                   byte_array_ptr[idx + 4],
                   byte_array_ptr[idx + 5],
                   byte_array_ptr[idx + 6],
                   byte_array_ptr[idx + 7],
                   byte_array_ptr[idx + 8]);
    ++ i;
  }

  idx = i * 9;
  switch( array_size - idx )
  {
  case 9:
    /* have 9 items left in last line */
    UIM_MSG_HIGH_9("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3],
                   byte_array_ptr[idx + 4],
                   byte_array_ptr[idx + 5],
                   byte_array_ptr[idx + 6],
                   byte_array_ptr[idx + 7],
                   byte_array_ptr[idx + 8]);
    break;
  case 8:
    /* have 8 items left in last line */
    UIM_MSG_HIGH_8("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3],
                   byte_array_ptr[idx + 4],
                   byte_array_ptr[idx + 5],
                   byte_array_ptr[idx + 6],
                   byte_array_ptr[idx + 7]);
    break;
  case 7:
    /* have 7 items left in last line */
    UIM_MSG_HIGH_7("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3],
                   byte_array_ptr[idx + 4],
                   byte_array_ptr[idx + 5],
                   byte_array_ptr[idx + 6]);
    break;
  case 6:
    /* have 6 items left in last line */
    UIM_MSG_HIGH_6("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3],
                   byte_array_ptr[idx + 4],
                   byte_array_ptr[idx + 5]);
    break;
  case 5:
    /* have 5 items left in last line */
    UIM_MSG_HIGH_5("0x%x 0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3],
                   byte_array_ptr[idx + 4]);
    break;
  case 4:
    /* have 4 items left in last line */
    UIM_MSG_HIGH_4("0x%x 0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1],
                   byte_array_ptr[idx + 2],
                   byte_array_ptr[idx + 3]);
    break;
  case 3:
    /* have 3 items left in last line */
    UIM_MSG_HIGH_3("0x%x 0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx+1],
                   byte_array_ptr[idx + 2]);
    break;
  case 2:
    /* have 2 items left in last line */
    UIM_MSG_HIGH_2("0x%x 0x%x",
                   byte_array_ptr[idx],
                   byte_array_ptr[idx + 1]);
    break;
  case 1:
    /* have 1 item left in last line */
    UIM_MSG_HIGH_1("0x%x",
                   byte_array_ptr[idx]);
    break;
  default:
    break;
  }
} /* gstk_util_dump_byte_array */

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
gstk_status_enum_type gstk_util_check_evt_mask_reg_status(
  uint64                                           evt_reg_bitmask,
  uint32                                           class_bit_mask,
  gstk_tk_functionality_enum_type                  func_type)
{
  uint64    single_bitmask = evt_reg_bitmask;
  uint64    mask           = 1;
  uint32    l_index        = 0;
  uint32    index          = 0;
  uint32    c_index        = 0;
  uint32    adj_offset     = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  /* check for valid func type */
  if(!gstk_util_is_valid_func_type(func_type))
  {
    UIM_MSG_ERR_1("Invalid func type 0x%x", func_type);
    return GSTK_BAD_PARAM;
  }

  /* Check if the class bitmask and evt bit mask match */
  if(gstk_util_check_evt_bitmask(class_bit_mask,evt_reg_bitmask) == FALSE)
  {
    UIM_MSG_ERR_2("Class - Evt mask did not match class 0x%x evt 0x%x",
                   class_bit_mask, evt_reg_bitmask);
    return GSTK_ERROR;
  }

  switch(class_bit_mask)
  {
  case GSTK_REG_CATEGORY_0_CMD:
    l_index = GSTK_CLASS_0_BEGIN_INDEX;
    break;
  case GSTK_REG_CATEGORY_B_CMD:
    l_index      = GSTK_CLASS_B_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_B_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_C_CMD:
    l_index      = GSTK_CLASS_C_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_C_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_E_CMD:
    l_index      = GSTK_CLASS_E_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_E_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_L_CMD:
    l_index      = GSTK_CLASS_L_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_L_ADJ_OFFSET;
    break;
  case GSTK_REG_CATEGORY_M_CMD:
    l_index      = GSTK_CLASS_M_BEGIN_INDEX;
    adj_offset   = GSTK_CATEGORY_M_ADJ_OFFSET;
    break;
  default:
    UIM_MSG_ERR_1("Invalid class_bit_mask 0x%x", class_bit_mask);
    return GSTK_ERROR;
  }

  /* Evt bitmask for different classes at present do not overlap due to backward
     compatibility. Hence the two lines below will avoiding checking bits for
     irrelevant classes and directly jump to the bit where a particular class's
     events begin
  */
  single_bitmask = single_bitmask >> l_index;
  index          = index + l_index;

  while(single_bitmask)
  {
    if (single_bitmask & mask)
    {
      c_index = l_index + index - adj_offset;

      if((c_index >= GSTK_TOTAL_ELEMENTS_IN_CLASS_ARRAY) || ((int32) c_index < 0))
      {
        UIM_MSG_ERR_2("index out of range c-index 0x%x index 0x%x ",
                      c_index, index);
        return GSTK_ERROR;
      }

      switch(func_type)
      {
      case GSTK_HANDLE_DISPLAY_FUNC_ONLY:
        if(gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.is_reg == TRUE)
        {
          UIM_MSG_ERR_0("Invalid GSTK_DUPLICATE_REGISTRATION ");
          return GSTK_DUPLICATE_REGISTRATION;
        }
        break;
      case GSTK_HANDLE_MODEM_FUNC_ONLY:
        if(gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg == TRUE)
        {
          UIM_MSG_ERR_0("GSTK_DUPLICATE_REGISTRATION ");
          return GSTK_DUPLICATE_REGISTRATION;
        }
        break;
      case GSTK_HANDLE_ALL_FUNC:
        if((gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_ui_func_client.is_reg == TRUE) ||
           (gstk_curr_inst_ptr->gstk_class_evt_reg_table[c_index].gstk_modem_func_client.is_reg == TRUE))
        {
          UIM_MSG_ERR_0("GSTK_DUPLICATE_REGISTRATION ");
          return GSTK_DUPLICATE_REGISTRATION;
        }
        break;
      default:
        break;
      }/* End of switch */
    }/*  if (single_bitmask & mask) */
    single_bitmask = single_bitmask >> 1;
    index++;
  }/* End of while */

  return GSTK_SUCCESS;

} /* gstk_util_check_evt_mask_reg_status */

/*lint +e715 */
/*===========================================================================
FUNCTION gstk_util_check_evt_bitmask

DESCRIPTION
  This function check if the input event bitmask belongs to a particular
  class of ME hardware dependant commands.

PARAMETERS
  proactive_cmd_class :  [Input] Letter classes for ME hardware dependant commands
  evt_reg_bitmask: [Input] Event Bitmask

DEPENDENCIES
  None

RETURN VALUE
  boolean
    FALSE: If command indicated by evt bitmask does not
     belong to the class specified by class bitmask.
    TRUE:  If command indicated by evt bitmask belongs to the
     class specified by the class bitmask.

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_check_evt_bitmask(
  uint32   proactive_cmd_class,
  uint64   evt_reg_bitmask
)
{
  boolean is_evt_in_class = FALSE;

  switch(proactive_cmd_class)
  {
  case GSTK_REG_CATEGORY_0_CMD:
    is_evt_in_class = gstk_util_is_evt_in_category(evt_reg_bitmask,
                                                   (uint64)GSTK_CATEGORY_0_EVT_BITMASK);
    break;
  case GSTK_REG_CATEGORY_B_CMD:
    is_evt_in_class = gstk_util_is_evt_in_category(evt_reg_bitmask,
                                                   (uint64)GSTK_CATEGORY_B_EVT_BITMASK);
    break;
  case GSTK_REG_CATEGORY_C_CMD:
    is_evt_in_class = gstk_util_is_evt_in_category(evt_reg_bitmask,
                                                   (uint64)GSTK_CATEGORY_C_EVT_BITMASK);
    break;
  case GSTK_REG_CATEGORY_D_CMD:
    UIM_MSG_ERR_0("ERROR - No specific commands present for class D");
    break;
  case GSTK_REG_CATEGORY_E_CMD:
    is_evt_in_class = gstk_util_is_evt_in_category(evt_reg_bitmask,
                                                   (uint64)GSTK_CATEGORY_E_EVT_BITMASK);
    break;
  case GSTK_REG_CATEGORY_L_CMD:
    is_evt_in_class = gstk_util_is_evt_in_category(evt_reg_bitmask,
                                                   (uint64)GSTK_CATEGORY_L_EVT_BITMASK);
    break;
  case GSTK_REG_CATEGORY_M_CMD:
    is_evt_in_class = gstk_util_is_evt_in_category(evt_reg_bitmask,
                                                   (uint64)GSTK_CATEGORY_M_EVT_BITMASK);
    break;
  default:
    UIM_MSG_ERR_1("Invalid proactive command class 0x%x", proactive_cmd_class);
    break;
  }
  return is_evt_in_class;
} /* gstk_util_check_evt_bitmask */

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
    TRUE:  If any event in mask belongs to class_bitmask
    FALSE: If events in mask does not belong to class class_bitmask

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
boolean gstk_util_is_evt_in_category(
  uint64   evt_reg_bitmask,
  uint64   class_bitmask
)
{
  return ((evt_reg_bitmask & class_bitmask) != 0) ? TRUE : FALSE;
}/* gstk_util_is_evt_in_category */

/*===========================================================================
FUNCTION gstk_util_extract_class_specfic_evt_bitmask

DESCRIPTION
  This function extracts the event bitmask corresponding to a particular class

PARAMETERS
  client_id:             [Input]        client id
  evt_reg_bitmask:       [Input]        event register bitmask
  bitmask_ptr:           [Input/Output] pointer to bitmask
  proactive_cmd_class:   [Input]        Letter classes for ME hardware dependant
                                        commands

DEPENDENCIES
  None

RETURN VALUE
  GSTK_ERROR               - Error extracting events from class mask
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
gstk_status_enum_type gstk_util_extract_class_specfic_evt_bitmask(
  uint64                                     evt_reg_bitmask,
  uint64                                    *bitmask_ptr,
  uint32                                     proactive_cmd_class,
  uint64                                    *rem_bitmask_ptr)
{
  if((bitmask_ptr == NULL) || (rem_bitmask_ptr == NULL))
  {
    UIM_MSG_ERR_0("bitmask_ptr NULL");
    return GSTK_NULL_INPUT_PARAM;
  }

  *bitmask_ptr     = 0x00;
  *rem_bitmask_ptr = 0x00;

  switch(proactive_cmd_class)
  {
  case GSTK_REG_CATEGORY_0_CMD:
    *bitmask_ptr = evt_reg_bitmask & GSTK_CATEGORY_0_EVT_BITMASK;
    *rem_bitmask_ptr = evt_reg_bitmask & ~GSTK_CATEGORY_0_EVT_BITMASK;
    break;
  case GSTK_REG_CATEGORY_B_CMD:
    *bitmask_ptr = evt_reg_bitmask & GSTK_CATEGORY_B_EVT_BITMASK;
    *rem_bitmask_ptr = evt_reg_bitmask & ~GSTK_CATEGORY_B_EVT_BITMASK;
    break;
  case GSTK_REG_CATEGORY_C_CMD:
    *bitmask_ptr = evt_reg_bitmask & GSTK_CATEGORY_C_EVT_BITMASK;
    *rem_bitmask_ptr = evt_reg_bitmask & ~GSTK_CATEGORY_C_EVT_BITMASK;
    break;
  case GSTK_REG_CATEGORY_E_CMD:
    *bitmask_ptr = evt_reg_bitmask & GSTK_CATEGORY_E_EVT_BITMASK;
    *rem_bitmask_ptr = evt_reg_bitmask & ~GSTK_CATEGORY_E_EVT_BITMASK;
    break;
  case GSTK_REG_CATEGORY_L_CMD:
    *bitmask_ptr = evt_reg_bitmask & GSTK_CATEGORY_L_EVT_BITMASK;
    *rem_bitmask_ptr = evt_reg_bitmask & ~GSTK_CATEGORY_L_EVT_BITMASK;
    break;
  case GSTK_REG_CATEGORY_M_CMD:
    *bitmask_ptr = evt_reg_bitmask & GSTK_CATEGORY_M_EVT_BITMASK;
    *rem_bitmask_ptr = evt_reg_bitmask & ~GSTK_CATEGORY_M_EVT_BITMASK;
    break;
  default:
    UIM_MSG_ERR_1("Invalid proactive command class 0x%x", proactive_cmd_class);
    return GSTK_ERROR;
  }

  return GSTK_SUCCESS;

}/* gstk_util_extract_class_specfic_evt_bitmask */

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
void gstk_util_free_client_id(gstk_client_id_type client_id)
{
  if (! gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("Invalid client id 0x%x", client_id);
    return;
  }

  /* delete client from the class event registration table */
  gstk_delete_client_from_class_evt_reg_table(client_id);

  gstk_shared_data.gstk_client_table[client_id-1].is_free          = TRUE;
  gstk_shared_data.gstk_client_table[client_id-1].client_cb        = NULL;
  gstk_shared_data.gstk_client_table[client_id-1].client_type      = GSTK_NO_TYPE;
  gstk_shared_data.gstk_client_table[client_id-1].user_data        = 0;
  gstk_shared_data.gstk_client_table[client_id-1].sim_slot_id      = GSTK_SLOT_ID_MAX;
} /* gstk_util_free_client_id */

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
void gstk_util_notify_tp_dl_fail(void)
{
  gstk_cmd_from_card_type    terminal_profile_rsp;

  memset(&terminal_profile_rsp, 0x00, sizeof(gstk_cmd_from_card_type));

  terminal_profile_rsp.hdr_cmd.command_id = GSTK_PROFILE_DL_IND_RSP;
  /* command_number, cmd_ref_id are ignore */
  terminal_profile_rsp.hdr_cmd.cmd_detail_reference = 0;
  terminal_profile_rsp.hdr_cmd.command_number = 0;
  terminal_profile_rsp.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_FAIL;
  (void)gstk_util_send_message_to_clients(&terminal_profile_rsp);
} /* gstk_util_notify_tp_dl_fail */

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
void gstk_util_notify_tp_dl_fail_specific_client(gstk_client_id_type client_id)
{
  gstk_cmd_from_card_type    terminal_profile_rsp;

  memset(&terminal_profile_rsp, 0x00, sizeof(gstk_cmd_from_card_type));

  terminal_profile_rsp.hdr_cmd.command_id = GSTK_PROFILE_DL_IND_RSP;
  /* command_number, cmd_ref_id are ignore */
  terminal_profile_rsp.hdr_cmd.cmd_detail_reference = 0;
  terminal_profile_rsp.hdr_cmd.command_number = 0;
  terminal_profile_rsp.cmd.terminal_profile_rsp = GSTK_PROFILE_RSP_FAIL;
  if((!gstk_shared_data.gstk_client_table[client_id-1].is_free) &&
     (gstk_shared_data.gstk_client_table[client_id-1].client_cb != NULL))
  {
    gstk_util_call_client_evt_cb(client_id, &terminal_profile_rsp);
  }
} /* gstk_util_notify_tp_dl_fail_specific_client */

/*===========================================================================
FUNCTION gstk_util_send_err_to_client

DESCRIPTION
  This function sends error report to client for sending the command in wrong
  GSTK state

PARAMETERS
  cmd_ptr: pointer to gstk command type

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
  const gstk_cmd_type* cmd_ptr )
{
  gstk_cmd_group_enum_type     type_of_command = GSTK_MAX_CMD;

  if(cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("cmd_ptr ERR:NULL");
    return;
  }

  /* Get the type of command */
  type_of_command = cmd_ptr->general_cmd.message_header.command_group;
  switch( type_of_command )
  {
    case GSTK_TERMINAL_PROFILE_CMD:
      UIM_MSG_HIGH_0(" Rec'd Terminal Profile Download in wrong state");
      gstk_util_notify_tp_dl_fail_specific_client(cmd_ptr->terminal_profile_cmd.message_header.client_id);
      break;
    default:
      UIM_MSG_HIGH_0(" Rec'd unknown command");
      break;
  }
} /* gstk_util_send_err_to_client */

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
)
{
  if (client_id == 0 || client_id > GSTK_MAX_CLIENT)
  {
    UIM_MSG_ERR_1("invalid client id 0x%x", client_id);
    return GSTK_SLOT_ID_MAX;
  }
  return gstk_shared_data.gstk_client_table[client_id - 1].sim_slot_id;
} /*gstk_util_get_sim_slot_id*/

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
)
{
  uint8 i = 0;
  for (i = 0; i < gstk_shared_data.gstk_num_of_instances; ++ i)
  {
    if (gstk_instances_ptr[i]->client_id == client_id)
    {
      return TRUE;
    }
  }
  return FALSE;
} /*gstk_util_is_client_id_internal*/

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
)
{
  uint32 i = 0;

  for (i = 0; i < array_size; ++i)
  {
    if (array_ptr[i] != byte_value)
    {
      return FALSE;
    }
  }

  return TRUE;
} /*gstk_util_check_bytes*/

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
)
{
#ifndef TEST_FRAMEWORK
  sint15                              nethandle;
  dss_iface_id_type                   iface_id;
  dss_net_policy_info_type            policy_info;
  sint15                              ds_errno;
  boolean                             ret       = TRUE;

  if (rat_ptr == NULL)
  {
    UIM_MSG_ERR_0("NULL ptr!");
    return FALSE;
  }

  dss_init_net_policy_info(&policy_info);
  policy_info.iface.kind = DSS_IFACE_NAME;
  policy_info.iface.info.name = DSS_IFACE_CDMA_SN;

#ifdef FEATURE_EHRPD
  policy_info.cdma.data_session_profile_id = 107;
#else
  policy_info.cdma.data_session_profile_id = 0;
#endif

  policy_info.is_routeable = TRUE;

  /* Initialize the network library  to get network handle */
  nethandle = dss_open_netlib2(NULL, NULL, NULL, NULL, &policy_info, &ds_errno);
  if (nethandle == DSS_ERROR)
  {
    UIM_MSG_ERR_1("Could not open network library, error=%d", ds_errno);
    return FALSE;
  }

  if ((iface_id = dss_get_iface_id(nethandle)) == DSS_IFACE_INVALID_ID)
  {
    UIM_MSG_ERR_1("Failed to get the iface id for nethandle=%d", nethandle);
    ret = FALSE;
    goto GSTK_GET_RAT_FROM_DS_CLEANUP_AND_RETURN;
  }

  if(dss_iface_ioctl(iface_id, DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
                  (void *)rat_ptr, &ds_errno) < 0)
  {
    UIM_MSG_ERR_0("Getting RAT technology from DS failed");
    ret = FALSE;
    goto GSTK_GET_RAT_FROM_DS_CLEANUP_AND_RETURN;
  }
  goto GSTK_GET_RAT_FROM_DS_CLEANUP_AND_RETURN;
GSTK_GET_RAT_FROM_DS_CLEANUP_AND_RETURN:
  if (dsnet_release_handle(nethandle, &ds_errno) != DSS_SUCCESS)
  {
    UIM_MSG_ERR_1("Failed to release net handle 0x%x!", ds_errno);
  }
  return ret;
#else
  #error code not present
#endif /*TEST_FRAMEWORK*/
} /* gstk_util_get_rat_from_ds */

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
)
{
  gstk_additional_info_type          addi_info;
  gstk_terminal_rsp_extra_param_type other_info;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, FALSE);

  memset(&addi_info, 0x00, sizeof(addi_info));
  memset(&other_info, 0x00, sizeof(other_info));

  /* check service status */
  if ((gstk_curr_inst_ptr->gstk_next_curr_location_status == GSTK_NO_SERVICE) ||
      (gstk_curr_inst_ptr->gstk_next_curr_rat ==  GSTK_ACCESS_NONE))
  {
    UIM_MSG_HIGH_0("Send error TR with NO SERVICE!");

    addi_info.length = 1;
    addi_info.additional_info[0] = GSTK_NO_SERVICE_AVAILABLE;
    if (gstk_send_terminal_response(
          gstk_curr_inst_ptr->client_id,
          0,
          cmd_detail_ref_id,
          command_number,
          terminal_response_enum,
          GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
          &addi_info,
          &other_info) == GSTK_SUCCESS)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* gstk_util_send_tr_if_no_service */

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
)
{
  gstk_additional_info_type          addi_info;
  gstk_terminal_rsp_extra_param_type other_info;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, FALSE);

  memset(&addi_info, 0x00, sizeof(addi_info));
  memset(&other_info, 0x00, sizeof(other_info));

  /* check service status */
  if (gstk_curr_inst_ptr->gstk_next_curr_rat == GSTK_ACCESS_TECH_CDMA)
  {
    UIM_MSG_HIGH_0("Send error TR - invalid RAT!");

    addi_info.length = 1;
    addi_info.additional_info[0] = GSTK_NO_SPECIFIC_CAUSE_CAN_BE_GIVEN;
    if (gstk_send_terminal_response(
          gstk_curr_inst_ptr->client_id,
          0,
          cmd_detail_ref_id,
          command_number,
          terminal_response_enum,
          GSTK_NETWORK_CURRENTLY_UNABLE_TO_PROCESS_COMMAND,
          &addi_info,
          &other_info) == GSTK_SUCCESS)
    {
      return TRUE;
    }
  }
  return FALSE;
} /* gstk_util_send_tr_if_not_3gpp */

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
gstk_status_enum_type  gstk_util_cmp_with_cached_term_rsp(const uim_cmd_type *uim_cmd_ptr)
{
  uint32 gen_res = 0;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  gen_res = uim_cmd_ptr->terminal_response.data[11] & 0xF0;
  if (gen_res != 0x30 && gen_res != 0x20)
  {
    // only send delay TR for duplicated pro cmd if general result is 0x3X or 0x2X
    return GSTK_ERROR;
  }

  /* Compare lengths of cached & current TRs */
  if (uim_cmd_ptr->terminal_response.num_bytes == gstk_curr_inst_ptr->gstk_cmd_cache.term_resp_length)
  {
    /* Compare cached TR and current TR */
    if(0 == memcmp ((void *)gstk_curr_inst_ptr->gstk_cmd_cache.term_resp,
                   (void *)uim_cmd_ptr->terminal_response.data,
                   uim_cmd_ptr->terminal_response.num_bytes))
    {
      return GSTK_SUCCESS;
    }
  }
  return GSTK_ERROR;
} /* gstk_cmp_with_cached_term_rsp */

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
void gstk_util_send_attach_ctrl_res()
{
  GSTK_RETURN_IF_NULL_PTR(gstk_curr_inst_ptr);

  UIM_MSG_HIGH_2("gstk_util_send_attach_ctrl_res(): 0x%x 0x%x",
                 gstk_curr_inst_ptr->attach_ctrl_res,
                 gstk_curr_inst_ptr->attach_ctrl_p);

  if ((gstk_curr_inst_ptr->attach_ctrl_p != NULL)
      &&
      (gstk_curr_inst_ptr->attach_ctrl_res != GSTK_ATTACH_CTRL_INVALID))
  {
    (void)rex_set_sigs(
            (rex_tcb_type *)gstk_curr_inst_ptr->attach_ctrl_p->task_tcb,
            (rex_sigs_type)gstk_curr_inst_ptr->attach_ctrl_p->sig_to_wait);
    gstk_free(gstk_curr_inst_ptr->attach_ctrl_p);
    (void)rex_clr_timer(&gstk_curr_inst_ptr->attach_ctrl_timer);
    gstk_curr_inst_ptr->attach_ctrl_p = NULL;
  }
} /*gstk_util_send_attach_ctrl_res*/

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
uint8 gstk_util_find_rat_priority(gstk_rat_enum_type rat)
{
  uint8                   index = 0;
  gstk_nv_items_data_type nv_data;

  UIM_MSG_HIGH_1("gstk_util_find_rat_priority for rat 0x%x", rat);

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_MAX_NUM_RATS);

  if(rat == GSTK_RAT_INVALID)
  {
    UIM_MSG_ERR_0("Invalid RAT passed");
    return GSTK_MAX_NUM_RATS;
  }

  (void)gstk_nv_access_read(GSTK_NV_SLOT_RAT_PRIORITY_CONFIG,
                            gstk_curr_inst_ptr->slot_id,
                            &nv_data);


  for(index=0; index<GSTK_MAX_NUM_RATS; index++)
  {
    if(nv_data.rat_prio[index] == rat)
    {
      return index;
    }
  }

  UIM_MSG_ERR_0("RAT priority not found in table");
  return GSTK_MAX_NUM_RATS;
} /*gstk_util_find_high_priority_rat*/

/*===========================================================================
FUNCTION gstk_util_process_mm_or_mode_change

DESCRIPTION
  The function compares the currently available mm or mode change events
  and decides which one should be processed. The API is called while
  processing a MM IDLE or MODE CHANGE command.

PARAMETERS
  None

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
  gstk_cmd_check_orig_enum_type         cmd_origin)
{
  gstk_location_status_enum_type    mm_idle_ls              = GSTK_NO_SERVICE;
  gstk_location_status_enum_type    mode_change_ls          = GSTK_NO_SERVICE;
  gstk_task_cmd_type*               task_cmd_ptr            = NULL;
  uint8                             mm_idle_priority        = GSTK_MAX_NUM_RATS;
  uint8                             mode_change_priority    = GSTK_MAX_NUM_RATS;
  gstk_access_technology_type       mode_change_access_tech = GSTK_ACCESS_NONE;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, FALSE);

  /*If currently GSTK has only MM IDLE or Mode Change then
    process it anyway by returning TRUE */
  if(gstk_curr_inst_ptr->mm_idle_pri_ptr == NULL
     || gstk_curr_inst_ptr->mode_change_cmd_ptr == NULL)
  {
    UIM_MSG_HIGH_2("MM IDLE PTR 0x%x or MODE CHANGE PTR 0x%x is NULL",
                   gstk_curr_inst_ptr->mm_idle_pri_ptr,
                   gstk_curr_inst_ptr->mode_change_cmd_ptr);
    return TRUE;
  }

  /* Compare the Location Status (Normal, Limited, No between MM IDLE and
     MODE CHANGE IND */
  if(gstk_util_map_mm_to_gstk_location_status(
      &(gstk_curr_inst_ptr->mm_idle_pri_ptr->mm_idle_state),
      &mm_idle_ls) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Bad location status from MM: 0x%x",
                  gstk_curr_inst_ptr->mm_idle_pri_ptr->mm_idle_state.location_state);
    return TRUE;
  }
  
  /* Get the priority RAT among all stacks that have 3GPP2 system */
  if (GSTK_SUCCESS != gstk_util_find_priority_mode_change_rat(&mode_change_access_tech, &mode_change_ls,
                        &gstk_curr_inst_ptr->mode_change_cmd_ptr->mode_change_ind))
  {
    return TRUE;
  }

  UIM_MSG_HIGH_3("Lowest = Highest MM IDLE LS 0x%x Mode Change LS 0x%x MM LS 0x%x",
                 mm_idle_ls,
                 mode_change_ls,
                 gstk_curr_inst_ptr->mm_idle_pri_ptr->mm_idle_state.location_state);

  /* Note Normal (0), Limited(1), No(2) Service */
  if(mm_idle_ls > mode_change_ls)
  {
    if(cmd_origin == GSTK_MM_IDLE_ORIG)
    {
      goto GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN;
    }
    else
    {
      return TRUE;
    }
  }
  else if(mm_idle_ls < mode_change_ls)
  {
    if(cmd_origin == GSTK_MODE_CHANGE_ORIG)
    {
      goto GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN;
    }
    else
    {
      return TRUE;
    }
  }
  else
  {
    /* Look at prioirty of access tech to decide which one to
    process - mm idle or mode change */

    /* Get MM IDLE access tech priority */
    switch(gstk_curr_inst_ptr->mm_idle_pri_ptr->mm_idle_state.rat)
    {
      case SYS_RAT_GSM_RADIO_ACCESS:
        mm_idle_priority = gstk_util_find_rat_priority(GSTK_RAT_GSM);
        break;

      case SYS_RAT_UMTS_RADIO_ACCESS:
      case SYS_RAT_TDS_RADIO_ACCESS:
        mm_idle_priority = gstk_util_find_rat_priority(GSTK_RAT_UMTS);
        break;

      case SYS_RAT_LTE_RADIO_ACCESS:
        mm_idle_priority = gstk_util_find_rat_priority(GSTK_RAT_LTE);
        break;

      default:
        UIM_MSG_ERR_0("Unhandled MM IDLE RAT in GSTK");
        return TRUE;
    }

    /* Get priority of prioritized access tech from 3GPP2 stacks of MODE CHANGE IND */
    mode_change_priority = gstk_util_find_access_tech_priority(mode_change_access_tech);

    UIM_MSG_HIGH_2("Lowest = Highest MM IDLE RAT priority 0x%x  MODE CH RAT Priority 0x%x",
                   mm_idle_priority, mode_change_priority);

    if(mm_idle_priority == GSTK_MAX_NUM_RATS && mode_change_priority != GSTK_MAX_NUM_RATS)
    {
      if(cmd_origin == GSTK_MM_IDLE_ORIG)
      {
        goto GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN;
      }
      else
      {
        return TRUE;
      }
    }
    else if(mm_idle_priority != GSTK_MAX_NUM_RATS && mode_change_priority == GSTK_MAX_NUM_RATS)
    {
      if(cmd_origin == GSTK_MODE_CHANGE_ORIG)
      {
        goto GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN;
      }
      else
      {
        return TRUE;
      }
    }
    else if(mm_idle_priority == GSTK_MAX_NUM_RATS && mode_change_priority == GSTK_MAX_NUM_RATS)
    {
      return TRUE;
    }

    /* Note: Lower number is higher priority */
    if(mm_idle_priority < mode_change_priority)
    {
      if(cmd_origin == GSTK_MODE_CHANGE_ORIG)
      {
        goto GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN;
      }
      else
      {
        return TRUE;
      }
    }
    else if(mm_idle_priority > mode_change_priority)
    {
      if(cmd_origin == GSTK_MM_IDLE_ORIG)
      {
        goto GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN;
      }
      else
      {
        return TRUE;
      }
    }
    else
    {
      return TRUE;
    }
  }

GSTK_UTIL_PROCESS_MM_OR_MODE_CHANGE_RETURN:
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if(task_cmd_ptr == NULL)
  {
    return FALSE;
  }

  if(cmd_origin == GSTK_MM_IDLE_ORIG)
  {
    (void)gstk_memcpy(&(task_cmd_ptr->cmd.mode_change_ind),
                   gstk_curr_inst_ptr->mode_change_cmd_ptr,
                   sizeof(gstk_cmd_type),
                   sizeof(gstk_cmd_type),
                   sizeof(gstk_cmd_type));
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_id =
                                      GSTK_MODE_CHANGE_FROM_REQUEUE;
  }
  else
  {
    (void)gstk_memcpy(&(task_cmd_ptr->cmd.mm_idle_state),
                   gstk_curr_inst_ptr->mm_idle_pri_ptr,
                   sizeof(gstk_cmd_type),
                   sizeof(gstk_cmd_type),
                   sizeof(gstk_cmd_type));
    task_cmd_ptr->cmd.mm_idle_state.message_header.command_id =
                                    GSTK_MM_IDLE_FROM_REQUEUE;
    task_cmd_ptr->cmd.mm_idle_state.message_header.sim_slot_id =
                                    gstk_curr_inst_ptr->slot_id;
  }
  gstk_task_put_cmd_buf_to_head(task_cmd_ptr);
  return FALSE;
} /*gstk_util_process_mm_or_mode_change*/

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
boolean gstk_util_validate_slot_sessions()
{
  uint8 i = 0;
  uint8 j = 0;
  uint8 k = 0;
  gstk_slot_id_enum_type slot[GSTK_AS_MAX_SESSION];

  /* Initialization */
  for (j = 0; j < GSTK_AS_MAX_SESSION; j++)
  {
    slot[j] = GSTK_SLOT_ID_MAX;
  }

  /* For each subscription, check that its sessions belong to the same slot */
  for (i = 0; i < GSTK_MODEM_AS_INFO_SIZE; i++)
  {
    for (j = 0; j < GSTK_AS_MAX_SESSION; j++)
    {
      /* Find out slot for each of this subscription's session */
      for (k = 0; k < GSTK_MMGSDI_SESS_INFO_SIZE; k++)
      {
        if (gstk_shared_data.modem_as_info[i].sess_types[j] ==
            gstk_shared_data.mmgsdi_sess_info[k].sess_type)
        {
          slot[j] = gstk_shared_data.mmgsdi_sess_info[k].slot;
        }
      }
    }

    /* if the respective slot of each of this subscription's session are different,
       print out error */
    for (j = 1; j < GSTK_AS_MAX_SESSION; j++)
    {
      if (slot[j - 1] < GSTK_SLOT_ID_MAX &&
          slot[j] < GSTK_SLOT_ID_MAX &&
          slot[j] != slot[j -1])
      {
        UIM_MSG_ERR_3("Sessions of the same subscription belong to different slot: subscription %x, slot %x, slot %x!",
                      i, slot[j - 1], slot[j]);
        return FALSE;
      }

      /* carry forward the most significant slot, for comparison's sake */
      if (slot[j] == GSTK_SLOT_ID_MAX)
      {
        slot[j] = slot[j - 1];
      }
    }
  }

  return TRUE;
}

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
uint8 gstk_util_find_access_tech_priority(gstk_access_technology_type access_tech)
{
  UIM_MSG_HIGH_1("gstk_util_find_access_tech_priority for rat 0x%x",
                 access_tech);

  if(access_tech == GSTK_ACCESS_NONE)
  {
    UIM_MSG_ERR_0("Invalid RAT passed");
    return GSTK_MAX_NUM_RATS;
  }

  switch(access_tech)
  {
    case GSTK_ACCESS_TECH_GSM:
      return gstk_util_find_rat_priority(GSTK_RAT_GSM);
    case GSTK_ACCESS_TECH_UTRAN:
      return gstk_util_find_rat_priority(GSTK_RAT_UMTS);
    case GSTK_ACCESS_TECH_CDMA:
      return gstk_util_find_rat_priority(GSTK_RAT_1X);
    case GSTK_ACCESS_TECH_HRPD:
      return gstk_util_find_rat_priority(GSTK_RAT_HRPD);
    case GSTK_ACCESS_TECH_LTE:
      return gstk_util_find_rat_priority(GSTK_RAT_LTE);
    case GSTK_ACCESS_TECH_EHRPD:
      return gstk_util_find_rat_priority(GSTK_RAT_EHRPD);
    default:
      UIM_MSG_ERR_0("RAT priority not found in table");
  }
  return GSTK_MAX_NUM_RATS;
} /*gstk_util_find_access_tech_priority*/

/*===========================================================================
FUNCTION gstk_util_is_env_to_remove

DESCRIPTION
  This function decides what cmds to be removed from cmd queue

PARAMETERS
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete()

DEPENDENCIES
  None

RETURN VALUE
  1 - cmd will be removed
  0 -  cmd will not be removed

===========================================================================*/
static int gstk_util_is_env_to_remove(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type *task_cmd_ptr  = NULL;
  gstk_cmd_enum_type  cmd_id        = GSTK_CMD_ENUM_NOT_USE;
  if(item_ptr == NULL)
  {
    UIM_MSG_HIGH_0("item_ptr is NULL");
    return 0;
  }

  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  cmd_id       = *((gstk_cmd_enum_type *)param_ptr);

  UIM_MSG_HIGH_3("gstk_util_is_env_to_remove(): 0x%x 0x%x 0x%x",
                 task_cmd_ptr->cmd.general_cmd.message_header.command_group,
                 task_cmd_ptr->cmd.general_cmd.message_header.command_id,
                 cmd_id);
  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group ==
       GSTK_ENVELOPE_CMD
     &&
     (uint32)cmd_id ==
       task_cmd_ptr->cmd.general_cmd.message_header.command_id)
  {
    /* Do not delete the command if it is ATTACH CTRL request */
    /* We need it to be processed further to unblock MM task */
    if(task_cmd_ptr->cmd.general_cmd.message_header.command_id ==
         GSTK_LOCATION_INFORMATION_IND &&
       task_cmd_ptr->cmd.general_cmd.message_header.user_data ==
         GSTK_MAX_PROACTIVE_COMMAND)
    {
      return 0;
    }
    return 1;
  }

  return 0;
} /*gstk_util_is_env_to_remove*/

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
)
{
  gstk_task_cmd_type     *cmd_ptr  = NULL;
  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  while (NULL != (cmd_ptr = q_linear_delete_new(&gstk_task_cmd_q,
                                                gstk_util_is_env_to_remove,
                                                &cmd_id,
                                                NULL,
                                                NULL)))
  {
    gstk_util_release_upper_layer_cmd_memory(&cmd_ptr->cmd);
    gstk_task_free_cmd_buf(cmd_ptr);
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /*Leaving gstk critical section */
}/*gstk_util_remove_pending_env*/

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
boolean gstk_util_convert_impu_list(uint8 **impu_list_ptr, uint8 *len_ptr)
{
  uint8 *orig_buff  = NULL;
  uint8 *new_buff   = NULL;
  uint8 orig_len    = 0;
  uint8 new_len     = 0;
  uint8 num_of_impu = 0;
  uint8 i           = 0;
  uint8 j           = 0;
  uint8 impu_beg    = 0;
  uint8 impu_end    = 0;

  if (impu_list_ptr == NULL || len_ptr == NULL)
  {
    UIM_MSG_ERR_0("Null Ptr!");
    return FALSE;
  }

  orig_buff = *impu_list_ptr;
  orig_len = *len_ptr;

  if (orig_buff == NULL || orig_len == 0)
    return TRUE;

  gstk_util_dump_byte_array(
    "=== IMPU list before ===",
    orig_buff,
    orig_len);

  for (i = 0; i < orig_len; ++ i)
  {
    if (orig_buff[i] == '\0')
      ++ num_of_impu;
  }
  if (orig_buff[i - 1] != '\0')
    ++ num_of_impu;

  new_len = num_of_impu + orig_len;
  new_buff = (uint8 *)GSTK_CALLOC(new_len);
  if (new_buff == NULL)
  {
    return FALSE;
  }

  for (j = 0, i = 0; j < new_len && i < orig_len; ++ i)
  {
    impu_beg = impu_end = i;
    while (orig_buff[i] != '\0' && i < orig_len)
      ++ i;
    impu_end = i;

    if (j < new_len)
      new_buff[j++] = 0x80;
    if (j < new_len)
      new_buff[j++] = impu_end - impu_beg;
    while (impu_beg < impu_end && j < new_len)
    {
      new_buff[j++] = orig_buff[impu_beg++];
    }
  }

  gstk_free(orig_buff);
  *impu_list_ptr = new_buff;
  *len_ptr = new_len;

  return TRUE;
}

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
boolean gstk_util_requeue_prio_mm(gstk_mm_idle_state_type* new_mm_ptr)
{
  int32 prev_index                             = 0;
  int32 new_index                              = 0;
  uint8 prev_rat_pri                           = 0;
  uint8 new_rat_pri                            = 0;
  gstk_location_status_enum_type prev_stack_ls = GSTK_NO_SERVICE;
  gstk_location_status_enum_type new_stack_ls  = GSTK_NO_SERVICE;
  gstk_task_cmd_type* task_cmd_ptr             = NULL;
  gstk_access_technology_type    prev_rat      = GSTK_ACCESS_NONE;
  gstk_access_technology_type    new_rat       = GSTK_ACCESS_NONE;

  if (new_mm_ptr == NULL)
  {
    UIM_MSG_ERR_0("In gstk_util_requeue_prio_mm() new_mm_ptr NULL!");
    return FALSE;
  }
  /* MOD indexing scheme to keep arbitration solution common to
     SGLTE, non-SGLTE, DSDS, TSTS */
  new_index = new_mm_ptr->stack_id % NUM_OF_MM_PER_GSTK_INST;
  UIM_MSG_HIGH_2("In gstk_util_requeue_prio_mm(): New MM stack_id 0x%x. New index: %d",
                 new_mm_ptr->stack_id, new_index);

  /* Cache new MM IDLE evt into proper index */
  if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index] == NULL)
  {
    gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index] =
      (gstk_cmd_type *)GSTK_CALLOC(sizeof(gstk_cmd_type));

    if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index] == NULL)
    {
      return FALSE;
    }
  }
  (void)gstk_memcpy(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index],
                 new_mm_ptr,
                 sizeof(gstk_cmd_type),
                 sizeof(gstk_cmd_type),
                 sizeof(gstk_cmd_type));

  /* If this is the first MM IDLE, update primary MM IDLE immediately */
  if (gstk_curr_inst_ptr->mm_idle_pri_ptr == NULL)
  {
    UIM_MSG_HIGH_0("First MM IDLE! Skip MM IDLE arbitration");
    return FALSE;
  }

  /* Find out what the previous stack index is (if it was active)
     This logic only works for NUM_OF_MM_PER_GSTK_INST = 2 */
  prev_index = (new_index + 1) % NUM_OF_MM_PER_GSTK_INST;

  /* if there is currently one stack active, also update primary MM IDLE immediately */
  if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index] == NULL)
  {
    UIM_MSG_HIGH_0("Only 1 MM IDLE stack active, skip MM IDLE arbitration");
    return FALSE;
  }

  /* Arbitration logic below applies only when there are 2 active MM IDLE stacks */
  /* Compare LS */
  UIM_MSG_HIGH_4("MM IDLE arbitration, prev stack RAT: 0x%x, new stack RAT: 0x%x, MM prev LS: 0x%x MM new LS",
                 gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state.rat,
                 gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index]->mm_idle_state.rat,
				 gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state.location_state,
				 gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index]->mm_idle_state.location_state);

  if(gstk_util_map_mm_to_gstk_location_status(
       &(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state),
       &prev_stack_ls) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Bad location status from MM prev stack: 0x%x",
                  gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state.location_state);
    return FALSE;
  }
   
  if(gstk_util_map_mm_to_gstk_location_status(
      &(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index]->mm_idle_state),
      &new_stack_ls) != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Bad location status from MM new stack: 0x%x",
                  gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index]->mm_idle_state.location_state);
    return FALSE;
  }

  UIM_MSG_HIGH_2("Prev stack LS: 0x%x, New stack LS: 0x%x",
                 prev_stack_ls, new_stack_ls);

  /* Compare LS */
  if (prev_stack_ls < new_stack_ls)
  {
    goto GSTK_UTIL_REQUEUE_PRIO_MM;
  }
  else if (prev_stack_ls > new_stack_ls)
  {
    return FALSE;
  }
  else
  {
    /* If LS is equal, pick MM IDLE based on RAT priority config */
    if(!gstk_util_is_valid_rat((sys_radio_access_tech_e_type)gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state.rat,
                               &prev_rat))
    {
      UIM_MSG_ERR_1("Unknown Sys Rat 0x%x",
                    gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state.rat);
      return FALSE;
    }
    if(!gstk_util_is_valid_rat((sys_radio_access_tech_e_type)gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index]->mm_idle_state.rat,
                               &new_rat))
    {
      UIM_MSG_ERR_1("Unknown Sys Rat 0x%x",
                    gstk_curr_inst_ptr->mm_idle_cmd_ptrs[new_index]->mm_idle_state.rat);
      goto GSTK_UTIL_REQUEUE_PRIO_MM;
    }
    prev_rat_pri = gstk_util_find_access_tech_priority(prev_rat);
    new_rat_pri = gstk_util_find_access_tech_priority(new_rat);

    UIM_MSG_HIGH_2("Prev stack RAT prio: 0x%x, New stack RAT prio: 0x%x",
                   prev_rat_pri, new_rat_pri);
    if (prev_rat_pri < new_rat_pri)
    {
      goto GSTK_UTIL_REQUEUE_PRIO_MM;
    }
    else if (prev_rat_pri > new_rat_pri)
    {
      return FALSE;
    }
    else
    {
      return FALSE;
    }
  }

GSTK_UTIL_REQUEUE_PRIO_MM:
  task_cmd_ptr = gstk_task_get_cmd_buf();

  if(task_cmd_ptr == NULL)
  {
    return FALSE;
  }

  (void)gstk_memcpy(&(task_cmd_ptr->cmd.mm_idle_state),
                 gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index],
                 sizeof(gstk_cmd_type),
                 sizeof(gstk_cmd_type),
                 sizeof(gstk_cmd_type));
  task_cmd_ptr->cmd.mm_idle_state.message_header.command_id = GSTK_MM_IDLE_FROM_REQUEUE;
  task_cmd_ptr->cmd.mm_idle_state.message_header.sim_slot_id = gstk_curr_inst_ptr->slot_id;
  gstk_task_put_cmd_buf_to_head(task_cmd_ptr);
  return TRUE;
}

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
boolean gstk_util_process_mm_indication(gstk_mm_idle_state_type *mm_ind_ptr)
{
  int32 prev_index                             = 0;
  int32 index                              = 0;

  UIM_MSG_HIGH_0("In gstk_util_process_mm_indication()");

  if (mm_ind_ptr == NULL)
  {
    UIM_MSG_ERR_0("In gstk_util_process_mm_indication() mm_ind_ptr NULL!");
    return FALSE;
  }

  /* Cache the MM indication received */
  gstk_shared_data.mm_ind_cache =
    (gstk_io_send_mm_ind_enum_type)mm_ind_ptr->message_header.user_data;

  /* MOD indexing scheme to keep arbitration solution common to
     SGLTE, non-SGLTE, DSDS, TSTS */
  index = mm_ind_ptr->stack_id % NUM_OF_MM_PER_GSTK_INST;
  UIM_MSG_HIGH_3("MM Indication stack_id 0x%x. Index: %d, Ind type 0x%x",
                 mm_ind_ptr->stack_id,
                 index,
                 gstk_shared_data.mm_ind_cache);

  /* Find out what the previous stack index is
     This logic only works for NUM_OF_MM_PER_GSTK_INST = 2 */
  prev_index = (index + 1) % NUM_OF_MM_PER_GSTK_INST;

  switch(gstk_shared_data.mm_ind_cache)
  {
    case GSTK_IO_MM_SGLTE_IND:
    case GSTK_IO_MM_CSFB_IND:
      /* for Mode Change, which will come with main stack ID, acquisition will
         start over so MM IDLEs for both stacks will arrive again. It is therefore
         safe to clear cache for hybrid stack in both cases.
         In CSFB Mode, hybrid stack will be deactivated for good. */
      if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index])
      {
        UIM_MSG_HIGH_1("Invalidate MM Index 0x%x", prev_index);
        gstk_free(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]);
        gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index] = NULL;
      }
      break;
    case GSTK_IO_MM_SGLTE_MERGE_IND:
    case GSTK_IO_MM_SGLTE_SPLIT_IND:
      /* MERGE will come with main stack ID
         SPLIT will come with hybrid stack ID
         For either case, we need to move MM Idle from this stack to the other stack */
      UIM_MSG_HIGH_2("mm_idle_cmd_ptrs: 0x%x, MM Index: 0x%x",
                     gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index],
                     prev_index);
      if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index])
      {
        (void)gstk_memcpy(mm_ind_ptr,
                       gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index],
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type),
                       sizeof(gstk_cmd_type));
        /* Moving stack: this logic only works for NUM_OF_MM_PER_GSTK_INST = 2 */
        mm_ind_ptr->stack_id =
          (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]->mm_idle_state.stack_id + 1) % NUM_OF_MM_PER_GSTK_INST;
        mm_ind_ptr->message_header.command_id = GSTK_MM_IDLE_FROM_IND;

        /* Deactivate the cache for this stack after moving it over */
        UIM_MSG_HIGH_1("Invalidate MM Index 0x%x", prev_index);
        gstk_free(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index]);
        gstk_curr_inst_ptr->mm_idle_cmd_ptrs[prev_index] = NULL;

        /* Have GSTK processes the MM IDLE as if it has moved stack */
        return TRUE;
      }
      break;
    default:
      return GSTK_ERROR;
  }

  return FALSE;
}

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
void gstk_util_log_gwl_info(const gstk_gwl_loc_info *gwl_info_ptr)
{
  if (!gwl_info_ptr)
  {
    return;
  }

  UIM_MSG_HIGH_1("gwl: cell_id_len=0x%x", gwl_info_ptr->cell_id_len);
  gstk_util_dump_byte_array("gwl info",
                            (const byte*)gwl_info_ptr,
                            sizeof(gstk_gwl_loc_info));
}/* gstk_util_log_gwl_info */


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
void gstk_util_log_cdma_info(const gstk_cdma_loc_info *cdma_info_ptr)
{
  if (!cdma_info_ptr)
  {
    return;
  }

  UIM_MSG_HIGH_7("cdma: mcc=0x%x, imsi_11_12=0x%x, sid=0x%x, nid=0x%x, base_id=0x%x, base_lat=0x%x, base_long=0x%x",
                 cdma_info_ptr->mcc,
                 cdma_info_ptr->imsi_11_12,
                 cdma_info_ptr->sid,
                 cdma_info_ptr->nid,
                 cdma_info_ptr->base_id,
                 cdma_info_ptr->base_lat,
                 cdma_info_ptr->base_long);
}/* gstk_util_log_cdma_info */


/*===========================================================================
FUNCTION gstk_util_get_unique_num

DESCRIPTION
  This function generate 32bit unique number.

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  Return 32bit random number

===========================================================================*/
uint32 gstk_util_get_unique_num(void)
{
  static uint16 seq_num = 0;
  return( (++seq_num) ? seq_num : (++seq_num));
}


/*===========================================================================
FUNCTION gstk_util_generate_ref_id

DESCRIPTION
  This function generate the reference id by appending unique number to 32bit input parameter

PARAMETERS
  ref_num [Input]: The reference number to be prefixed with random number

DEPENDENCIES
  None

RETURN VALUE
  Return 32 bit unique reference id

===========================================================================*/
uint32 gstk_util_generate_ref_id(uint32 ref_num)
{
  return (ref_num | (gstk_util_get_unique_num() << 16));
}


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
void gstk_util_get_idx(uint32 ref_id, uint32 *index, uint32 *u_num)
{
  *index = GSTK_CMD_REF_TABLE_INDEX(ref_id);
  if(u_num != NULL)
  {
    *u_num = GSTK_UNIQUE_REF_ID_MASK(ref_id);
  }
}


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
gstk_status_enum_type gstk_util_compare_ref_id(uint32 ref_id)
{
  if(command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].unique_ref_id != ref_id)
  {
    UIM_MSG_HIGH_2("Ref Ids are not matching From client : 0x%x In ref table : 0x%x",
                   ref_id,
                   		 command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(ref_id)].unique_ref_id);
    return(GSTK_BAD_PARAM);
  }
  return(GSTK_SUCCESS);
}

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
                                         gstk_tag_enum_type     tag)
{
  uint32 i = 0;

  switch(tag)
  {
    case GSTK_TAG_CARD_ERROR:
    case GSTK_TAG_REFRESH_RESET:
    case GSTK_TAG_RECOVERY:
    case GSTK_TAG_POWER_DOWN:
      /* Delete the cached proactive command and reset the proactive command
         flags and TR delay timers */
      memset(&gstk_curr_inst_ptr->gstk_cmd_cache, 0x00, sizeof(gstk_cmd_cache_type));
      (void)rex_clr_timer(&gstk_curr_inst_ptr->gstk_terminal_rsp_delay_timer);
      gstk_curr_inst_ptr->gstk_proactive_cmd_in_progress  = FALSE;
      if(gstk_shared_data.ui_nw_proactive_session_slot_in_progress == slot)
      {
        gstk_shared_data.ui_nw_proactive_session_slot_in_progress = GSTK_SLOT_ID_MAX;
        gstk_util_check_slot_with_fetch_rejected();
      }
      if(gstk_shared_data.fetch_is_in_progress == (uint8)slot)
      {
        gstk_shared_data.fetch_is_in_progress = 0;
      }
      gstk_curr_inst_ptr->gstk_is_curr_session_ui_related = FALSE;
      gstk_curr_inst_ptr->in_prog_cmd_ref_id              = GSTK_MAX_PROACTIVE_COMMAND;
      gstk_curr_inst_ptr->gstk_is_msg_cached              = FALSE;
      gstk_shared_data.is_open_ch_ok                 = TRUE;
      gstk_curr_inst_ptr->cdma_loc_st_disabled            = FALSE;
      gstk_curr_inst_ptr->is_fetch_ignored                = FALSE;

      /* Cleanup envelope related data and flags */
      gstk_util_clear_retry_envelope_data(gstk_curr_inst_ptr->slot_id);

      gstk_curr_inst_ptr->gstk_pending_location_evt_envelope = FALSE;
      gstk_curr_inst_ptr->gstk_bip_env_in_progress           = FALSE;
      gstk_curr_inst_ptr->gstk_block_cc_env                  = 0;
      gstk_curr_inst_ptr->bip_blocked                        = FALSE;
      gstk_curr_inst_ptr->otasp_blocked                      = FALSE;
      gstk_curr_inst_ptr->tr_unique_ref_id                   = 0;

      /* Clean up GSTK timer management timers */
      for (i = 0; i< GSTK_MAX_TIMER_SUPPORTED; i++)
      {
        (void)rex_clr_timer( &(gstk_curr_inst_ptr->gstk_timer_management[i].timer) );
        (void)rex_clr_timer( &(gstk_curr_inst_ptr->gstk_timer_management[i].timer_elapsed) );
        gstk_curr_inst_ptr->gstk_timer_management[i].time          = 0;
        gstk_curr_inst_ptr->gstk_timer_management[i].timer_started = FALSE;
        gstk_curr_inst_ptr->gstk_timer_management[i].pending       = FALSE;
        gstk_curr_inst_ptr->gstk_timer_management[i].waiting_rsp   = FALSE;
      }

      /* Cleanup Setup event list */
      memset(gstk_curr_inst_ptr->gstk_evt_list, 0x00, GSTK_MAX_EVENT_LIST_LEN);
      gstk_util_release_upper_layer_cmd_memory(
               (gstk_cmd_type *)&gstk_curr_inst_ptr->setup_evt_list_cnf_cache);
      memset(&gstk_curr_inst_ptr->setup_evt_list_cnf_cache, 0x00,
                         sizeof(gstk_curr_inst_ptr->setup_evt_list_cnf_cache));

      /* Cleanup NW search mode Info */
      gstk_curr_inst_ptr->is_nw_search_mode_sent = FALSE;
      gstk_curr_inst_ptr->nw_search_mode = GSTK_NW_SEARCH_MODE_MAX;

      /* Cleanup current access technology group */
      gstk_curr_inst_ptr->gstk_curr_tech_group = GSTK_ACCESS_TECH_NONE;
      break;
    default:
      /*Do nothing*/
      break;
  }

  switch(tag)
  {
    case GSTK_TAG_CARD_ERROR:
    case GSTK_TAG_POWER_DOWN:
    case GSTK_TAG_REFRESH_RESET:
    case GSTK_TAG_REFRESH_2G_INIT:
      /* Cleanup GSTK location info */
      memset(gstk_curr_inst_ptr->gstk_last_loc_info_sent, 0xFF, 3);
      gstk_curr_inst_ptr->gstk_mm_param_received          = FALSE;
      gstk_curr_inst_ptr->gstk_mm_state_is_idle           = FALSE;

      memset(gstk_curr_inst_ptr->gstk_curr_mcc_mnc, 0xFF, 3);
      memset(gstk_curr_inst_ptr->gstk_curr_lac, 0xFF, 2);
      memset(gstk_curr_inst_ptr->gstk_curr_cell_id.cell_id,
             0xFF, GSTK_MAX_CELL_ID_LEN);
      gstk_curr_inst_ptr->gstk_curr_cell_id.cell_len      = 0;
      gstk_curr_inst_ptr->gstk_curr_location_status       = GSTK_NO_SERVICE;
      gstk_curr_inst_ptr->gstk_curr_rat                   = GSTK_ACCESS_NONE;

      memset(gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc, 0xFF, 3);
      memset(gstk_curr_inst_ptr->gstk_next_curr_lac, 0xFF, 2);
      memset(gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_id,
               0xFF, GSTK_MAX_CELL_ID_LEN);
      gstk_curr_inst_ptr->gstk_next_curr_cell_id.cell_len = 0;
      gstk_curr_inst_ptr->gstk_next_curr_location_status  = GSTK_NO_SERVICE;
      gstk_curr_inst_ptr->gstk_next_curr_rat              = GSTK_ACCESS_NONE;
      gstk_curr_inst_ptr->nas_rat                         = GSTK_ACCESS_NONE;

      /* Clean up call related flags */
      gstk_curr_inst_ptr->gstk_cm_call_is_connected       = FALSE;

      /* Cleanup MM IDLE, Mode change and GSTK Loci table info */
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
      memset(gstk_curr_inst_ptr->gstk_loci_table.plmn_id,
             0xFF, GSTK_MAX_PLMN_LEN);
      memset(gstk_curr_inst_ptr->gstk_loci_table.lac,0xFF,GSTK_MAX_LAC_LEN);
      gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_len = GSTK_MAX_CELL_ID_LEN;
      memset(gstk_curr_inst_ptr->gstk_loci_table.cell_info.cell_id,
             0xFF, GSTK_MAX_CELL_ID_LEN);
      gstk_curr_inst_ptr->gstk_loci_table.loc_status = GSTK_NO_SERVICE;
  #endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_LTE*/

      (void)rex_clr_timer(&(gstk_curr_inst_ptr->gstk_mm_idle_no_service_timer));
      if(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer != NULL)
      {
        gstk_free(gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer);
        gstk_curr_inst_ptr->gstk_mm_idle_no_srv_from_timer = NULL;
      }

      for (i = 0; i < NUM_OF_MM_PER_GSTK_INST; ++i)
      {
        if (gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i] != NULL)
        {
           gstk_free(gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i]);
           gstk_curr_inst_ptr->mm_idle_cmd_ptrs[i] = NULL;
        }
      }

      if(gstk_curr_inst_ptr->mm_idle_pri_ptr != NULL)
      {
        gstk_free(gstk_curr_inst_ptr->mm_idle_pri_ptr);
        gstk_curr_inst_ptr->mm_idle_pri_ptr     = NULL;
      }

      if(gstk_curr_inst_ptr->mode_change_cmd_ptr != NULL)
      {
        gstk_free(gstk_curr_inst_ptr->mode_change_cmd_ptr);
        gstk_curr_inst_ptr->mode_change_cmd_ptr = NULL;
      }

      /* Cleanup RRC loci table */
  #if defined(FEATURE_WCDMA) || defined(FEATURE_LTE)
      for (i = 0; i < GSTK_RAT_MAX_IDX; i++)
      {
        gstk_curr_inst_ptr->is_rrc_cell_id_received[i] = FALSE;
        memset(gstk_curr_inst_ptr->rrc_loci_table[i].plmn_id, 0xFF, GSTK_MAX_PLMN_LEN);
        memset(gstk_curr_inst_ptr->rrc_loci_table[i].lac, 0xFF, GSTK_MAX_LAC_LEN);
        memset(gstk_curr_inst_ptr->rrc_loci_table[i].cell_info.cell_id,
             0xFF, GSTK_MAX_CELL_ID_LEN);
        gstk_curr_inst_ptr->rrc_loci_table[i].cell_info.cell_len = 0;
      }
  #endif /* FEATURE_WCDMA || FEATURE_LTE */

      /* Cleanup RR data */
      memset(gstk_shared_data.rr_mcc_mnc, 0xFF,
             sizeof(gstk_shared_data.rr_mcc_mnc));
      memset(gstk_shared_data.rr_lac, 0xFF,
             sizeof(gstk_shared_data.rr_lac));
      memset(gstk_shared_data.rr_cell_id, 0xFF,
             sizeof(gstk_shared_data.rr_cell_id));

      /* Cleanup CSG info */
      memset(&gstk_curr_inst_ptr->csg_info, 0x00,
             sizeof(gstk_curr_inst_ptr->csg_info));
      gstk_curr_inst_ptr->csg_info.access_tech = GSTK_ACCESS_NONE;
      gstk_curr_inst_ptr->is_csg_info_sent = FALSE;

      /* Cleanup NW reject cache */
      gstk_curr_inst_ptr->nw_rej.access_tech = GSTK_ACCESS_NONE;
      memset(gstk_curr_inst_ptr->nw_rej.mcc_mnc, 0xFF, GSTK_MAX_PLMN_LEN);
      memset(gstk_curr_inst_ptr->nw_rej.lac, 0xFF, GSTK_MAX_LAC_LEN);
      UIM_MSG_HIGH_0("Clear 3GPP2 LOCI cache");
#ifdef FEATURE_CDMA
      memset(&gstk_curr_inst_ptr->cdma_ext_loci_cache,
             0xFF,
             sizeof(gstk_curr_inst_ptr->cdma_ext_loci_cache));
      gstk_curr_inst_ptr->cdma_ext_loci_cache.is_cdma_info_valid = FALSE;
      gstk_curr_inst_ptr->cdma_ext_loci_cache.is_advance_info_valid = FALSE;
      gstk_curr_inst_ptr->cdma_ext_loci_cache.is_hdr_info_valid = FALSE;
#endif /* FEATURE_CDMA */

      if(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr != NULL)
      {
        gstk_free(gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr);
        gstk_curr_inst_ptr->gstk_cdma_loc_status_cmd_ptr = NULL;
      }

      /* Clean up tp releated data and flags */
      gstk_curr_inst_ptr->is_first_loc_info_envelope_sent   = FALSE;
      gstk_curr_inst_ptr->is_tp_dl_done                     = TRUE;
      gstk_curr_inst_ptr->is_tp_rsp_received                = FALSE;
      if (GSTK_IS_VALID_SLOT_ID(slot))
      {
        gstk_shared_data.fetch_allowed[slot - 1]  = FALSE;
      }
      /* Reset Setup evt list count */
      gstk_curr_inst_ptr->setup_evt_list_cnt           = 0;
      /* Cleanup Setup event list */
      memset(gstk_curr_inst_ptr->gstk_evt_list, 0x00, GSTK_MAX_EVENT_LIST_LEN);
      gstk_curr_inst_ptr->recovery_enable_ls_act       = FALSE;

      /* Cleanup MMGSDI related data and flags */
      gstk_curr_inst_ptr->apps_enabled                 = GSTK_APP_BMSK_NONE;
      gstk_curr_inst_ptr->is_subs_ready.subs_ready_gw  = FALSE;
      gstk_curr_inst_ptr->is_subs_ready.subs_ready_1x  = FALSE;
      gstk_curr_inst_ptr->card_protocol                = MMGSDI_NO_PROTOCOL;
      gstk_curr_inst_ptr->read_imsi                    = FALSE;
      gstk_curr_inst_ptr->imsi_switched                = FALSE;
      gstk_shared_data.gstk_is_imsi_m_provisioned = FALSE;

      /* Clean up OTASP data */
      gstk_curr_inst_ptr->otasp_call_id     = -1;
      gstk_curr_inst_ptr->otasp_failure_cnt = 0;
      gstk_curr_inst_ptr->recv_otasp_commit = FALSE;
      if(gstk_curr_inst_ptr->open_ch_imsi_m_ptr != NULL)
      {
        gstk_free((void *)gstk_curr_inst_ptr->open_ch_imsi_m_ptr);
        gstk_curr_inst_ptr->open_ch_imsi_m_ptr = NULL;
      }

      for (i = 0; i < GSTK_CM_NUM_STACKS; ++i)
      {
        gstk_curr_inst_ptr->tz_info[i].access_tech = GSTK_ACCESS_NONE;
        gstk_curr_inst_ptr->tz_info[i].tz_avail = GSTK_TZ_NOT_AVAILABLE;
        gstk_curr_inst_ptr->tz_info[i].tz = 0xFF;
      }
      break;
    
    default:
      /*Do nothing*/
      break;
  }
} /* gstk_util_clean_up_gstk_global_data */


/*===========================================================================
FUNCTION gstk_util_build_error_envelope_response

DESCRIPTION
  This function build error envelope response for pending envelope commands
  in recovery or card error cases.

PARAMETERS
  ref_id: Envelope reference table index

DEPENDENCIES
  gstk_status_enum_type
  GSTK_ERROR  : if command could not be built successfully or no need to
                construct error response
  GSTK_SUCCESS: if command was built successfully

RETURN VALUE
  None

===========================================================================*/
gstk_status_enum_type gstk_util_build_error_envelope_response(uint32 ref_id)
{
  gstk_general_envelope_rsp_enum_type rsp_type          = GSTK_ENVELOPE_CMD_FAIL;
  gstk_status_enum_type               gstk_status       = GSTK_SUCCESS;
  uint32                              command_id        = 0;
  uint8                               timer_table_index = GSTK_MAX_TIMER_SUPPORTED;

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);

  if(ref_id <= GSTK_MAX_PROACTIVE_COMMAND)
  {
    command_id = gstk_shared_data.envelope_cmd_ref_table[ref_id].expected_cmd_rsp_type;
    switch(command_id)
    {
      case GSTK_MENU_IND_RSP:
        rsp_type = GSTK_ENVELOPE_CMD_FAIL;
        break;
      case GSTK_SMS_PP_DOWNLOAD_IND_RSP:
      case GSTK_IDLE_SCRN_EVT_IND_RSP:
      case GSTK_MO_SMS_CTRL_IND_RSP:
      case GSTK_CC_IND_RSP:
      case GSTK_USER_ACTIVITY_EVT_IND_RSP:
      case GSTK_CELL_BROADCAST_DOWNLOAD_IND_RSP:
      case GSTK_RAW_ENVELOPE_RSP:
        if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
        {
          rsp_type = GSTK_ENVELOPE_CMD_CARD_BUSY;
        }
        else
        {
          rsp_type = GSTK_ENVELOPE_CMD_FAIL;
        }
        break;
      case GSTK_MT_CALL_EVT_IND_RSP:
      case GSTK_CALL_CONNECTED_EVT_IND_RSP:
      case GSTK_CALL_DISCONNECTED_EVT_IND_RSP:
      case GSTK_LOCATION_STATUS_EVT_IND_RSP:
      case GSTK_LANG_SELECTION_EVT_IND_RSP:
      case GSTK_ACCESS_TECH_EVT_IND_RSP:
      case GSTK_NW_SEARCH_MODE_EVT_IND_RSP:
      case GSTK_BROWSER_TERMINATION_EVT_IND_RSP:
      case GSTK_DATA_AVAIL_EVT_IND_RSP:
      case GSTK_CH_STATUS_EVT_IND_RSP:
      case GSTK_NW_REJ_EVT_IND_RSP:
      case GSTK_CSG_CELL_SELECTION_EVT_IND_RSP:
      case GSTK_IMS_REG_EVT_IND_RSP:
        /* no need to build error evelope report for these evt dl, cleanup
           from ref table */
        gstk_status = GSTK_ERROR;
        break;
      case GSTK_TIMER_EXPIRE_IND_RSP:
        timer_table_index = uint32touint8(gstk_shared_data.envelope_cmd_ref_table[ref_id].user_data);
        if (timer_table_index >= GSTK_MAX_TIMER_SUPPORTED)
        {
          UIM_MSG_ERR_1("Invalid Timer Table Index 0x%x", timer_table_index);
        }
        else
        {
          UIM_MSG_ERR_0("Timer Expire Envelope Err");
          gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_started = FALSE;
          gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].pending       = FALSE;
          gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].time          = 0;
          (void)rex_clr_timer( &(gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].timer_elapsed));
          gstk_curr_inst_ptr->gstk_timer_management[timer_table_index].waiting_rsp = FALSE;
        }
        gstk_status = GSTK_ERROR;
        break;
      case GSTK_LOC_INFO_ENVELOPE_RSP:
        if(gstk_curr_inst_ptr->gstk_sub_state == GSTK_RECOVERY_SUB_S)
        {
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_ALLOWED;
          gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_ALLOWED;
        }
        else
        {
          gstk_curr_inst_ptr->attach_ctrl_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
          gstk_curr_inst_ptr->loc_info_env_res = GSTK_ATTACH_CTRL_NOT_ALLOWED;
        }
        gstk_util_send_attach_ctrl_res();
        gstk_status = GSTK_ERROR;
        break;
      default:
        UIM_MSG_ERR_1("In Env Rsp Process, unknown cmd type 0x%x", command_id);
        gstk_status = GSTK_ERROR;
        break;
    }/* End of switch */
    if(gstk_status == GSTK_SUCCESS &&
       gstk_build_error_envelope_report(ref_id, command_id, rsp_type ) != GSTK_SUCCESS)
    {
      gstk_status = GSTK_ERROR;
    }
  }
  else
  {
    gstk_status = GSTK_ERROR;
  }

  if(gstk_status == GSTK_ERROR)
  {
    /* Clean-up retry Data */
    gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id));
  }
  return gstk_status;
}

/*===========================================================================
FUNCTION gstk_util_find_priority_mode_change_rat

DESCRIPTION
  This function finds the priority 3GPP2 access tech based on location status
  and RAT

PARAMETER
  access_tech     : Result of Access tech due to arbitration
  location_status : Result of Loc Status due to arbitration
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
   const gstk_mode_change_ind_type* mode_change_ind)
{
  int i = 0;

  if(access_tech == NULL || loc_status == NULL || mode_change_ind == NULL)
  {
    UIM_MSG_HIGH_3("NULL Input Param 0x%x 0x%x 0x%x",
                   access_tech, loc_status, mode_change_ind);
    return GSTK_NULL_INPUT_PARAM;
  }

  /* initialize the input variables to default values */
  *loc_status = GSTK_NO_SERVICE;
  *access_tech = GSTK_ACCESS_NONE;

  for (i = 0; i < GSTK_CM_NUM_STACKS; i++)
  {
    switch (mode_change_ind->cm_stack[i].access_tech)
    {
      case GSTK_ACCESS_TECH_CDMA:
      case GSTK_ACCESS_TECH_HRPD:
      case GSTK_ACCESS_TECH_EHRPD:
        break;
      default:
       continue;
    }

    /* Lowest value of location status is highest priority */
    if(*loc_status < mode_change_ind->cm_stack[i].loc_status)
    {
      continue;
    }
    else if(*loc_status > mode_change_ind->cm_stack[i].loc_status)
    {
      *access_tech = mode_change_ind->cm_stack[i].access_tech;
      *loc_status = mode_change_ind->cm_stack[i].loc_status;
    }
    else if(mode_change_ind->cm_stack[i].loc_status == *loc_status)
    {
      uint8 prev_rat_pri = gstk_util_find_access_tech_priority(
                             *access_tech);
      uint8 curr_rat_pri  = gstk_util_find_access_tech_priority(
                             mode_change_ind->cm_stack[i].access_tech);

      if (*loc_status == GSTK_NO_SERVICE)
      {
        /* Just "continue" as the previous location status and current
           location status are both no service */
        continue;
      }
      if (prev_rat_pri > curr_rat_pri)
      {
        *access_tech = mode_change_ind->cm_stack[i].access_tech;
      }
    }
  }
  UIM_MSG_HIGH_2("Priority Access Tech = 0x%x Loc Status = 0x%x",
                 *access_tech, *loc_status);

  return GSTK_SUCCESS;
} /* gstk_util_find_priority_mode_change_rat */

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
  FALSE : Otherwise

===========================================================================*/
boolean gstk_util_is_vzw_mcc(const uint8* loc_info_ptr)
{
  if (loc_info_ptr[0] == 0x13 && (loc_info_ptr[1] == 0x00 || loc_info_ptr[1] == 0x01))
  {
    UIM_MSG_HIGH_2("US VzW MCC - enable CDMA Loc Status envelope 0x%x 0x%x",
             loc_info_ptr[0], loc_info_ptr[1]);
    return TRUE;
  }
  else
  {
    UIM_MSG_HIGH_2("Non-US MCC - disable CDMA Loc Status envelope 0x%x 0x%x",
             loc_info_ptr[0], loc_info_ptr[1]);
    return FALSE;
  }
} /* gstk_util_is_vzw_mcc */

/*===========================================================================
FUNCTION gstk_util_find_terminal_rsp

DESCRIPTION
  This function returns true if it finds a TR in the queue, used by
  gstk_util_find_terminal_rsp_in_queue. This is used during hotswap recovery scenarios to
  ensure status words are sent to clients due to GSTK being unable to get GSTK_TR_REPORT

PARAMETERS
  item_ptr  [Input]:  ptr to queue item
  param_ptr [Input]:  no use as of now. This will be populated as NULL
                      by q_linear_delete_new()

DEPENDENCIES
  None

RETURN VALUE
  1== Found the TR, 0 otherwise

COMMENTS
  None

SIDE EFFECTS

SEE ALSO
  None
===========================================================================*/
static int gstk_util_find_terminal_rsp(
   void *item_ptr,
   void *param_ptr
)
{
  gstk_task_cmd_type      *task_cmd_ptr  = NULL;
  gstk_slot_id_enum_type  slot           = GSTK_SLOT_ID_MAX;
  if(item_ptr == NULL)
  {
    UIM_MSG_HIGH_0("item_ptr is NULL");
    return 0;
  }
  task_cmd_ptr = (gstk_task_cmd_type*)item_ptr;
  slot = gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  if(task_cmd_ptr->cmd.general_cmd.message_header.command_group == GSTK_TERMINAL_RSP &&
     task_cmd_ptr->cmd.general_cmd.message_header.sim_slot_id == slot)
  {
    return 1;
  }
  else
  {
    return 0;
  }
} /* gstk_util_find_terminal_rsp */

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
  TRUE == Found the TR, FALSE otherwise

COMMENTS
  None

SIDE EFFECTS
  Removes the TR from the queue if found

SEE ALSO
  None
===========================================================================*/
gstk_task_cmd_type* gstk_util_find_terminal_rsp_in_queue(void) {
  gstk_task_cmd_type   *cmd_p = NULL;

  /* Entering gstk critical section */
  rex_enter_crit_sect(&gstk_shared_data.gstk_crit_sect);
  /* find TR in the gstk_cmd_q, if none return false */
  while((cmd_p = (gstk_task_cmd_type*) q_linear_delete_new (&gstk_task_cmd_q,
                                                     gstk_util_find_terminal_rsp,
                                                     NULL,
                                                     NULL,
                                                     NULL)) != NULL)
  {
    rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
    return cmd_p;
  }
  rex_leave_crit_sect(&gstk_shared_data.gstk_crit_sect);
  return NULL;
  /* Leaving gstk critical section */
} /* gstk_util_find_terminal_rsp_in_queue */

/*===========================================================================
FUNCTION gstk_util_send_sw_to_client

DESCRIPTION
  This function checks if GSTK should send status words to client during
  recovery/hotswap scenarios

PARAMETERS
  sw1_sw2_ptr : SW1 SW2 report

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
void gstk_util_send_sw_to_client(gstk_cmd_from_card_type *sw1_sw2_ptr) {
  gstk_cmd_from_card_type                                 sw1_sw2_rpt;
  gstk_status_enum_type                                   gstk_status;
  gstk_cmd_enum_type                                      cmd_type = GSTK_CMD_ENUM_MAX;
  gstk_client_id_type                                     client_id = 0x00;

  /*
    Because this function is triggered when gstk_proactive_cmd_in_progress == true
    or when GSTK has received the TR report, the cached proactive cmd is still
    valid
  */

  UIM_MSG_HIGH_1("sw1_sw2_ptr 0x%x", sw1_sw2_ptr);
  sw1_sw2_rpt.hdr_cmd.command_id = GSTK_TERM_RSP_SW1_SW2_RPT;
  /* Received TR report and SW1 and SW2 not null */
  if(sw1_sw2_ptr != NULL)
  {
    sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw1 = sw1_sw2_ptr->cmd.term_rsp_sw1_sw2_rpt.sw1;
    sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw2 = sw1_sw2_ptr->cmd.term_rsp_sw1_sw2_rpt.sw2;
    sw1_sw2_rpt.hdr_cmd.sim_slot_id = sw1_sw2_ptr->hdr_cmd.sim_slot_id;
  }
  else
  {
    sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw1 = 0x90;
    sw1_sw2_rpt.cmd.term_rsp_sw1_sw2_rpt.sw2 = 0x00;
    sw1_sw2_rpt.hdr_cmd.sim_slot_id = gstk_curr_inst_ptr->slot_id;
  }

  if(gstk_curr_inst_ptr->gstk_cmd_cache.cmd[5] == GSTK_CMD_STK_SET_UP_EVENT_LIST)
  { 
    
    for(client_id = 1; client_id <= GSTK_MAX_CLIENT && gstk_util_is_valid_client_id(client_id); client_id++)
    {
      if(gstk_shared_data.gstk_client_table[client_id - 1].send_setup_event_sw1_sw2)
      {
         gstk_shared_data.gstk_client_table[client_id -1].send_setup_event_sw1_sw2 = FALSE;     
         gstk_util_call_client_evt_cb(client_id, &sw1_sw2_rpt);
      }
    }
  }
    
  gstk_status = gstk_util_convert_from_stk_cmd_type_to_gstk_cmd_type(
                                          gstk_curr_inst_ptr->gstk_cmd_cache.cmd[5],
                                          gstk_curr_inst_ptr->gstk_cmd_cache.cmd[6],
                                          &cmd_type);
  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("cached proactive cmd gave has invalid cmd_type 0x%x",
                  cmd_type);
    return;
  }

  gstk_status = gstk_util_find_client_with_registered_event(&client_id,
                                                            cmd_type,
                                                            GSTK_RAW_FORMAT,
                                                            GSTK_HANDLE_ALL_FUNC);
  if (gstk_status != GSTK_SUCCESS)
  {
    UIM_MSG_ERR_1("Error finding client for cmd_type 0x%x, GSTK_RAW_FORMAT, GSTK_HANDLE_ALL_FUNC",
                  cmd_type);
    return;
  }
  if (!gstk_util_is_valid_client_id(client_id))
  {
    UIM_MSG_ERR_1("obtained invalid client id 0x%x", client_id);
    return;
  }

  gstk_util_call_client_evt_cb(client_id,
                               &sw1_sw2_rpt);
} /* gstk_util_send_sw_to_client */

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
)
{
  uint8 event_payload_len = 0;

  PACKED struct PACKED_POST
  {
    uint8 channel_id;
    uint8 slot_id;
    uint8 session_status;
  } bip_status;

  UIM_MSG_HIGH_3("Preparing BIP Status DIAG Event: 0x%x, Channel ID: 0x%x, Slot ID: 0x%x",
           bip_session_status, bip_channel_id, slot_id);

  if (!GSTK_IS_VALID_SLOT_ID(slot_id))
  {
    return;
  }

  event_payload_len = sizeof(bip_status);

  bip_status.channel_id = (uint8)bip_channel_id;
  bip_status.slot_id = (uint8)slot_id;

  switch (bip_session_status)
  {
    case GSTK_BIP_SESSION_IN_PROGRESS:
      bip_status.session_status = 0;
      break;
    case GSTK_BIP_SESSION_END:
      bip_status.session_status = 1;
      break;
    default:
      return;
  }

  /* Send out the DIAG Event */
  event_report_payload(EVENT_GSTK_BIP_STATUS,
                       event_payload_len,
                       (void *)&bip_status);
}/* gstk_util_send_bip_status_diag_event */

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
  sys_detailed_plmn_list_s_type *srs_plmn_csg_info)
{
  uint8 i, j, k;

  if(dest_plmn_csg_info == NULL || srs_plmn_csg_info == NULL)
    return;

  for(i=0; i < srs_plmn_csg_info->length; i++)
  {
    if(srs_plmn_csg_info->info[i].csg_info.csg_id != SYS_CSG_ID_INVALID)
    {
      for(j=0; j < dest_plmn_csg_info->no_of_plmn_entry; j++)
      {
        if(memcmp(&srs_plmn_csg_info->info[i].plmn.identity,
                  &dest_plmn_csg_info->available_plmn_info[j],
                  3) == 0)
        {
          break;
        }
      }
      if(j == dest_plmn_csg_info->no_of_plmn_entry)
      {
        dest_plmn_csg_info->no_of_plmn_entry++;
        if(gstk_memcpy(dest_plmn_csg_info->available_plmn_info[j].plmn,
                  srs_plmn_csg_info->info[i].plmn.identity,
                  3,
                  3,
                  3) <
           3)
        {
          /* Partial data copied */
        }
      }
      k = dest_plmn_csg_info->available_plmn_info[j].no_of_csg_entry++;
      /* Copy CSG ID */
      dest_plmn_csg_info->available_plmn_info[j].csg_info[k].csg_id =
        srs_plmn_csg_info->info[i].csg_info.csg_id;

       /* Copy Home (e)NodeB Name */
      dest_plmn_csg_info->available_plmn_info[j].csg_info[k].len =
        gstk_memcpy(dest_plmn_csg_info->available_plmn_info[j].csg_info[k].hnb_name,
                       srs_plmn_csg_info->info[i].csg_info.hnb_name.name,
                       srs_plmn_csg_info->info[i].csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       GSTK_HNB_NAME_MAX_LEN);

        /* Copy access tech */
       dest_plmn_csg_info->available_plmn_info[j].csg_info[k].access_tech =
            srs_plmn_csg_info->info[i].rat;

       /* Calculate CSG cell selection status */
       if(dest_plmn_csg_info->available_plmn_info[j].csg_info[k].access_tech == GSTK_ACCESS_NONE ||
         (int)(dest_plmn_csg_info->available_plmn_info[j].csg_info[k].access_tech) == SYS_CSG_ID_INVALID)
       {
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.general_info.is_camped = FALSE;
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.addi_info.is_addi_info_present = FALSE;
       }
       else
       {
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.general_info.is_camped = TRUE;
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.addi_info.is_addi_info_present = TRUE;
       }
    }
  }
}/* gstk_util_extract_plmn_csg_info */

/*===========================================================================
FUNCTION gstk_util_find_modem_capability

DESCRIPTION
  This function find out the modem capability using MCS API.

PARAMETERS
  capability [input]: RAT type for which capability is needed.

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
boolean gstk_util_find_modem_capability(gstk_modem_cap_enum_type capability)
{
  static mcs_modem_cap_return_enum lte_available = MCS_MODEM_CAP_UNKNOWN;

  if(lte_available == MCS_MODEM_CAP_UNKNOWN)
  {
    switch(capability)
    {
      case GSTK_MODEM_CAPABILITY_FEATURE_LTE:
        lte_available = mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_LTE);
        break;

      case GSTK_MODEM_UNKNOWN:
      default:
        return FALSE;
     }
  }

  switch (lte_available)
  {
    case MCS_MODEM_CAP_AVAILABLE:
      return TRUE;

    case MCS_MODEM_CAP_UNAVAILABLE:
    case MCS_MODEM_CAP_UNKNOWN:
    default:
      return FALSE;
  }
}

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
)
{
  uint8 index = 0;

  if(sub_id_ptr == NULL)
  {
    UIM_MSG_ERR_0("sub_id_ptr is NULL");
    return GSTK_BAD_PARAM;
  }

  if(!GSTK_IS_VALID_SLOT_ID(slot_id))
  {
    UIM_MSG_HIGH_1("Invalid slot id 0x%x", slot_id);
    return GSTK_BAD_PARAM;
  }

  for (index = 0; index < GSTK_MMGSDI_SESS_INFO_SIZE; index++)
  {
    if(gstk_shared_data.mmgsdi_sess_info[index].slot == slot_id)
    {
      switch(gstk_shared_data.mmgsdi_sess_info[index].sess_type)
      {
        case MMGSDI_GW_PROV_PRI_SESSION:
        case MMGSDI_1X_PROV_PRI_SESSION:
          *sub_id_ptr = SYS_MODEM_AS_ID_1;
          return GSTK_SUCCESS;
        case MMGSDI_GW_PROV_SEC_SESSION:
        case MMGSDI_1X_PROV_SEC_SESSION:
          *sub_id_ptr = SYS_MODEM_AS_ID_2;
          return GSTK_SUCCESS;
        case MMGSDI_GW_PROV_TER_SESSION:
        case MMGSDI_1X_PROV_TER_SESSION:
          *sub_id_ptr = SYS_MODEM_AS_ID_3;
          return GSTK_SUCCESS;
        default:
          break;
      }
    }
  }

  UIM_MSG_ERR_1("Could not find Valid MMGSDI Session corresponding to slot: 0x%x",
                slot_id);

  return GSTK_ERROR;
} /* gstk_util_get_sub_id_from_slot */

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
boolean gstk_util_compare_mm_to_nw_rej_cache(void)
{
  UIM_MSG_HIGH_0("In gstk_util_compare_mm_to_nw_rej_cache()");
  /*
    Check cache to see if we received a NW reject (limited service)
    previously, if we have not, return false
  */
  if (gstk_curr_inst_ptr->nw_rej.access_tech == GSTK_ACCESS_NONE)
  {
    UIM_MSG_HIGH_0("Did not receive NW rej yet");
    return FALSE;
  }
  /*
    Check if NW reject cached PLMN matches PLMN sent from NAS, if there
    is a mis match, then do not sent limited svc LS to card
  */
  else if (memcmp(gstk_curr_inst_ptr->nw_rej.mcc_mnc,
                  gstk_curr_inst_ptr->gstk_next_curr_mcc_mnc,
                  GSTK_MAX_PLMN_LEN))
  {
    UIM_MSG_HIGH_0("PLMN received from NAS different from NW reject cache");
    UIM_MSG_HIGH_3("NW reject cache PLMN 0x%x 0x%x 0x%x",
                   gstk_curr_inst_ptr->nw_rej.mcc_mnc[0],
                   gstk_curr_inst_ptr->nw_rej.mcc_mnc[1],
                   gstk_curr_inst_ptr->nw_rej.mcc_mnc[2]);
    return FALSE;
  }
  /*
    Check if NW reject cached PLMN matches LAC sent from NAS, if there
    is a mis match, then do not sent limited svc LS to card
  */
  else if (memcmp(gstk_curr_inst_ptr->nw_rej.lac,
                  gstk_curr_inst_ptr->gstk_next_curr_lac,
                  GSTK_MAX_LAC_LEN))
  {
    UIM_MSG_HIGH_0("LAC received from NAS different from NW reject cache");
    UIM_MSG_HIGH_2("NW reject cache LAC 0x%x 0x%x 0x%x",
                   gstk_curr_inst_ptr->nw_rej.lac[0],
                   gstk_curr_inst_ptr->nw_rej.lac[1]);
    return FALSE;
  }
  return TRUE;
}

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
void gstk_util_location_info_tlv_cleanup(gstk_cdma_loci_ext_tlv_type *loc_info_tlv)
{
  UIM_MSG_HIGH_0("gstk_util_location_info_tlv_cleanup()");

  if (loc_info_tlv != NULL && loc_info_tlv->loc_info != NULL) 
  {
    gstk_free(loc_info_tlv->loc_info);
    loc_info_tlv->loc_info = NULL;
  }
}

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
boolean gstk_util_merge_cm_stacks(gstk_cdma_loci_ext_cache_type *cdma_ext_cache,
                                  gstk_mode_change_ind_type     *mode_change_ind)
{
  int i = 0;

  if(cdma_ext_cache == NULL || mode_change_ind == NULL)
  {
    UIM_MSG_HIGH_2("NULL Input Param 0x%x 0x%x",
                   cdma_ext_cache, mode_change_ind);
    return FALSE;
  }

  /* Initialize Location Info to all 0xFFs */
  memset(cdma_ext_cache, 0xFF, sizeof(gstk_cdma_loci_ext_type));
  cdma_ext_cache->is_advance_info_valid = FALSE;
  cdma_ext_cache->is_cdma_info_valid = FALSE;
  cdma_ext_cache->is_hdr_info_valid = FALSE;

  for (i = 0; i < GSTK_CM_NUM_STACKS; i++)
  {
    if (mode_change_ind->cm_stack[i].access_tech == GSTK_ACCESS_TECH_CDMA &&
        mode_change_ind->cm_stack[i].loc_status != GSTK_NO_SERVICE)
    {
      gstk_cache_1x_location_info(cdma_ext_cache, mode_change_ind->cm_stack[i].loc_info.cdma_ext.cdma_loc_info);
    }

    if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, gstk_curr_inst_ptr->slot_id) == TRUE)
    {
       if (mode_change_ind->cm_stack[i].advance_info_changed)
       {
          gstk_cache_advance_location_info(cdma_ext_cache, mode_change_ind->cm_stack[i].loc_info.cdma_ext);
       }

       if ((mode_change_ind->cm_stack[i].access_tech == GSTK_ACCESS_TECH_HRPD ||
            mode_change_ind->cm_stack[i].access_tech == GSTK_ACCESS_TECH_EHRPD) &&
           mode_change_ind->cm_stack[i].loc_status != GSTK_NO_SERVICE)
       {
          gstk_cache_hdr_location_info(cdma_ext_cache, mode_change_ind->cm_stack[i].loc_info.cdma_ext);
       }
    } /* if (gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, slot) == TRUE) */
  } /* for (i = 0; i < GSTK_CM_NUM_STACKS; i++) */

  UIM_MSG_HIGH_3("Mode Change LOCI: 0x%x 0x%x 0x%x",
                 cdma_ext_cache->is_cdma_info_valid,
                 cdma_ext_cache->is_advance_info_valid,
                 cdma_ext_cache->is_hdr_info_valid);

  gstk_util_dump_byte_array(
    "CDMA Data",
    (const byte*)&cdma_ext_cache->data,
    sizeof(cdma_ext_cache->data));

  return TRUE;
}
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
void gstk_util_check_slot_with_fetch_rejected(void)
{
  uint8 slot_id = 0;

  for(slot_id = 0; slot_id < gstk_shared_data.gstk_num_of_instances; slot_id++)
  {
    if(gstk_instances_ptr[slot_id] != NULL && 
       gstk_instances_ptr[slot_id]->is_fetch_ignored &&
       gstk_shared_data.fetch_allowed[slot_id])
    {
      UIM_MSG_HIGH_1("Check for pending proactive command on slot 0x%x", slot_id+1);
      /* call uimdrv API to check pending proactive command */
      (void)uim_check_pending_proactive_command((uim_slot_type)(slot_id+1));
    }
  }
}
