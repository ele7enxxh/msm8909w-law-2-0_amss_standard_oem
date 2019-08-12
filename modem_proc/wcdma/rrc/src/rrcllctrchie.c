/*===========================================================================

                L3 - RRC Lower Layer Controller Transport Channel IEs

DESCRIPTION
  This file contains functions necessary for the RRC Lower Layer Controller
  Ordered Config Transport Channel Information Element Processing.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcllctrchie.c_v   1.33   07 Jun 2002 21:45:40   vnagpal  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcllctrchie.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
05/16/16   sg      Made changes for F3 reduction
05/09/16   sp      Made changes to add call end metrics - FR#30856
11/19/14   sg      Made changes to reject the configuration if SRBs are mapped to EDCH 
                   transport channels and EDCH transmission variable is FALSE.
11/05/14   aa      Added Message Highs for validation failures
10/31/14   sg      Made changes to revert CR 701950
09/02/14   sp      Changes are made to not add mac header size for TB size 0
08/01/14   sg      Made changes to skip EUL mapping validations if edch transmission variable is set to false
07/18/14   ad      Made changes to reduce F3s
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/25/14   sp      Made changes to set the last ctfc index as 0 when CTFCs are added for the first time so that last ctfc index is updated correclty
02/13/14   ad      Made changes to remove redundant F3s
12/13/13   db      Made changes to pass the correct R9 mac_ehs sizes 
10/21/13   vg      Made changed to Accept reconfiguration which asks UE to use (MACi/is, DCH) and reject reconfiguration only with (MACi/is, MAChs).
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
07/08/13   ad      Made changes to remove redundant F3s
07/01/13   vg      Added check wcdma_nv_hsupa_category == 0, when determining whether UE can support 2ms TTI.
06/14/13   as       Made changes to provide WL1 API to get the trch id for ctch
05/08/13   db      Multiple mapping support for EFACH
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
03/06/13   ad       Added prints to print CRNTI on PCH->FACH and FACH->PCH transition
                    Removed redundant F3s
02/05/13   ad       Provided an API which checks if SRBs are mapped to HSDSCH or not
01/23/13   vi       Made the change to check the eul is on while checking the ul_pdu_type
01/03/13   ad      Made changes to reduce the redundant F3s
12/14/12   mp      Fixing KW errors.
12/12/12   mp      Fixing KW errors
10/26/12   db      EFACH Redesign changes
10/20/12   sg       Changed "temp_ul_trch_add_reconfig_ptr" to "temp_ul_trch_delete_ptr" in 
                    function rrcllc_add_delete_and_arrange_ul_dch_trch_info_r7.
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
09/26/12   sg       Made changes to send FAILURE if the trch ids are same in 
                    add and delete information for edch.
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/12/12   md       Made changes to use L1 macros L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO and 
                    L1_DL_HS_DSCH_HARQ_MAX_PROC_MIMO inplace of L1_DL_HS_DSCH_HARQ_MAX_PROC
                    to validate the num of HARQ processes configured.
05/16/12   db       Made changes to ignore mapping check for data UM Rbs
05/10/12   md      Added code changes to support FEATURE_MAC_I
04/20/12   db       Fixed Compiler Warning
04/04/12   as       Changes to remove released Rb-Id from mapping validation check
03/15/12   db       Fixed Compilation Warnings
02/06/12   db       Made changes to reject the OTA when invalid NW reconfig from Mac-ehs to Mac-hs
01/10/11   db       Added code for 8.6.5.23, Mac-ehs Queue Id present in both Add/Reconfig 
                    and delete Trch Channel info list
12/27/11   sa       Added Null pointer checks
11/20/11   db       Made changes to backup and restore dedicated queue info
                    if queue id is same as in common queue
09/28/11   ad       Made changes to configure all multiplexing is valid only when
                    there is no multiplexing present in OC
08/26/11   ad       Made changes to pass the correct number of trchs if cell id in dch 
                    feature is ON. Initializing dl_dch_trch_info while deleting trch.
08/02/11   ad       Made changes to return failure if the same trch id is present in
                    the DL deleted transCh List and DL add reconf transCh List.
06/21/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/09/11   ad       Made changes to fill the holes in OC for mac-d flow index if 
                    EDCH trch is deleted before "Added or Reconfigured TrCH information list"
                    IE is processed
05/03/11   rl       Fixed the lint error
04/25/11   rl       Fixed the KW error
04/07/11   as       Added NULL pointer access check for e-mac-d flow list
03/28/11   rl       Merged the HS-FACH/PCH changes from mainline
03/14/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/09/11   su       Made changes to mainline code under FEATURE_WCDMA HSUPA 2MS TTI.
02/07/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl       Added the not operator for RRC_CHECK_COMMON_MSG_TYPE_PTR 
                    found during testing
01/28/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad       Added changes for RRC SW decoupling
01/06/11   as       Made changes to delete commonn queue info while moving
                    to non EFACH cell or DCH state
12/31/10   rl       Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl       Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/11/10   ss       ASN.1 migration to ASN1 9.3.0
10/04/10   prk      Merging latest e-fach fixes.
09/20/10   prk/kp   Added code to support FEATURE_WCDMA_HS_FACH
11/03/10   rl       Fixed compiler warnings
10/30/10   rl       Objective systems migration changes check in 
                    on code base VU_MODEM_WCDMA_RRC.01.80.00
08/02/10   kp       Added code to init e-tti & harq_rv_config which are trch i.e in 
                    L1 data base when previously EUL was not active. 
06/18/10   ss       Fixed compilation warnings
05/04/10   gkg      Made changes to initialize DL CTFC properly.
02/01/10   dm       Fixed compilation error
01/27/10   dm       Fixed Klockwork warnings
10/05/09   ps       Fixed Klockwork warnings
10/07/09   gkg      Changes for REL8 under FEATURE_WCDMA_REL8
06/09/09   ps       Fixed compiler warnings
06/03/09   gkg      Made changes so that the validation of SRB-Mapping isn't
                    failed if SRBs are mapped to MAC-ehs in DL.
05/25/09   kp       Added support for rejecting the configuration, If there is 
                    mismatch in E-DCH supported cat & N/W configuration. 
                    This check is only based on E-TTI.
05/22/09   ps       Suppressing lint warning when REL7 is defined
05/18/09   gkg      To fix compile warning, moved the definition of 
                    process_ul_deleted_trch_info_dch_usch() under R6 feature.
05/11/09   ps       Feature to dynamically allocate OC, CC, TOC  ptr
05/08/09   ss       Updated Copyright Information
04/29/09   ss       Made changes to compiler warnings on gcc compilers.
03/27/09   kp       Made changes to set all E-DCH active set DL RL's bit mask to reconfig, 
                    at TTI reconfiguration.in Rel7 leg.
03/18/09   gkg      Made changes so that UL TFCS is copied to DL TFCS using OC
                    instead of CC. This is needed when DL TFCS is set to sameAsUL
                    but OTA doesn't provide new UL TFCS.
03/09/09   ps       cleanup of feature flag 
                    FEATURE TFC RLC SIZE
                    FEATURE_DCH_FACH_STATE_TRANSITIONS
                    FEATURE TFC RLC SIZE
                    FEATURE_DCH_IE_IN_CELL_FACH
                    FEATURE_TFCSUBSET
                    FEATURE_ADJUST_TB_SIZE_WHEN_TRCH_INFO_NOT_PRESENT
02/12/09   gkg      Made changes towards optimizing PRACH working DB. 
                    Basically the data type of PRACH working DB is changed from 
                    ordered_config_type to prach_config_type.
02/13/09   gkg      If any OTA doesn't have TFC-subset List IE, return SUCCESS
                    by default instead of FALSE.
02/09/09   ps       Made changes to inform l1 if the Trch has Srb mapped to it 
                    or not through rrcllc_check_srb_mapped_to_trch()
01/28/09   dm       Fixed lint errors
01/19/08   ps       Made changes for ASn1 migration 25.331v7.9  
                    i.e.May 2008 version  (2008-05) 
21/11/08   gkg      Merged the changes that are needed for disabling HS on LCU build.
11/10/08   ps       Set the flag of hsdsch_info presence as TRUE if HArq info
                    is received from R7 ASU 
09/12/08   ps       Added code for REl7 under FEATURE_WCDMA_REL7
06/16/08    ps      Corrected featurisation issues for FEATURE HSDPA
06/13/08   gkg      Made changes so that if RLC PDU size changes for RBs through
                    Transport Channel IEs of OTA messages, those RBs are reconfigured
                    and therefore their RLC entity is notified for new RLC PDU size.
04/29/08    ps      Made changes to fix warnings reported by VeriSource tool
04/25/08    ps      Made changes to pass num_trch and num_tfci as 0 to L1
                    when num_dpdch in UL is 0
04/18/08    gkg     Fixed Klockwork errors.
03/06/08    da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                    Includes mobility and dynamic resource management.
03/06/08    da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
21/02/08    gkg     Fixed array index out-of-bound warnings reported by Klocwork.
02/13/08    kp      Added code changes to configure MAC when MAC-D flow is 
                    deleted for EUL.
02/11/08    ps      Corrected the print for the Restriched  Trch case through  
                    TFC subset list 
02/07/08    kp      Added support reject this configuration which try setting up 
                    2msec TTI when UE cat is either 1,3,5.    
02/04/08    kp      Added support to set all E-DCH active set DL RL's bit mask
                    to reconfig when TTI reconfiguration happens as L1 needs to reconfigure
                    all DL RL's in E-DCH active set.
01/11/08    ps      Fixed comipler warning when FEATURE_WCDMA_GPS_CELL_ID_READING
                    is defined
12/21/07    da      Support for FEATURE_MBMS
11/26/07    da      Corrected wrong bitmask check when receiving non-scheduled 
                    harq allocation info for EUL 2ms tti
11/14/07    rm      Made changes to set new_tfcs_list_present when processing the OTA
                    changes the common UL TrCh info
                    Added function to support FEATURE TFCC ENHNC
10/30/07    ps      Modified the prints of UL TFC subset list to show all the 
                    lists correctly.
09/26/07    ps      Corrected the handling of “resitricted Trch list” received in 
                    TFC Subset info in OTA msg.
09/11/07    vk      When multiple PDU Sizes are specified for a Queue/DfLOW,
                    corrected the assignment of MAC-d PDU Size to MAC-d flow
                    structure
08/31/07    ps      Added support for feature cell id reading in DCH state, 
                    under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
06/29/07    ps      Added code to reject a OTA message if the SRBs are not 
                    configured with any Trch configuration.
05/23/07    ps      Added support for "RB Info to Change" IE in RB-Setup msg.
                    Change are made under feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP.
04/20/07    da      Made changes needed to update ASN.1 version to R6 sept R6 2006
04/19/07    da      Put TFCS check when TrCH is added/deleted under feature
                    FEATURE_TFCS_SANITY_CHECK.  Also made change to enforce
                    this rule when destination state is CELL_DCH and relaxed
                    the checkf for other states.
04/10/07    ps      Made correction in way of checking the R5 nv rel flag 
03/29/07    rm      Fixed the issue of wrong calculation of number or tfcs when a
                    CTFC is added and the current config has a few TFCI`s removed.
03/02/06    ps      Made correction to traverse the link list 
                    restricted_trch_tfc_ptr for SRB5
02/21/07    ps      Made changes to handle failure/succes status when the
                    Standalone Rb-mapping info is restored into the ordered config
02/07/07    da      Setting flags ul_tfcs_rcvd when UL TFCS info rcvd and dl_tfcs_rcvd_flag 
                    when DL TFCS info rcvd. Setting flags ul_tfcs_needed if UL DCH is added 
                    or deleted and dl_tfcs_needed when DL DCH is added or deleted.
02/07/07    ps      Made changes to handle Standalone Rb-Mappings
02/02/07    da      Fixed buffer overrun vulnerability found in klocwork
01/22/07    da      Added check to make sure num_index doesn't read beyond the end 
                    of dch_trch_info_ptr_array in functions rrcllc_sort_dl_dch_trch_index_array
                    and rrcllc_sort_ul_dch_trch_index_array
11/30/06    da      Made change not to process dch quality target when the TrCH
                    is HS.  Also changed to use the right bitmask for R5 and R6 path.
11/09/06    da      Added check to not reject msg when NW tries to delete UL
                    non-existing trCH.
11/06/06    sgk     Removed check in set_hsdpa_l1_queue_info to return from
                    the function if the queue count in the DFLOW is 0 so that
                    the loop continues and copied the next valid DFLOW info to
                    the l1 structure.  
09/12/06   sm/da    Fix code review comments and general clean-up for REL6 and HSUPA
09/08/06    da      When BLER quality value is not signalled from NW, set it 
                    to -127.  This would result in 1% of BLER target. This is
                    good UE behaviour though the spec is ambiguous on what UE
                    behavior should be in this case.   
08/29/06   sm/da    Made changes required for R6 ASN.1 migration.  Also merged
                    FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/21/06    tkk     Added SRB#5 support.
06/05/06    sgk     Made changes to include rel5 non HSDPA functionality under 
                    FEATURE REL5 while retaining the HSDPA specifc processing 
                    under FEATURE HSDPA.
05/16/06    sgk     Fixed lint errors.
05/10/06    vk      When network indicates UE to delete a DFLOW Id that
                    does not exist at UE, ignore the error and proceed ahead
                    with the rest of the message
04/19/06    vk      When network indicates UE to delete a DL DCH channel that
                    does not exist at UE, ignore the error and proceed ahead
                    with the rest of the message
04/18/06   da       Modified code to support migration to Mar-06 Rel-5 ASN1 +
                    R6 Hacks
03/06/06    tkk     Added function "rrcllc_update_oc_with_tfc_subset" 
                    for TFC subset handling through reconfiguration/interRAT 
                    messages.
02/28/06  da/sm     Modified code to support migration to Dec-05 Rel-6 ASN1
12/06/05    vk      Made change to get the TFC information from Current Config
                    Database for the Downlink TFC when network indicates that
                    downlink common information is the same as that for uplink
                    and it does not specify the uplink TFC
08/12/05    da      Made change to increment TB size by 4 when multiple logical 
                    channels are mapped to the same transport channel.  This is to 
                    account for MAC header.  Change is for R. 99.  Functions affected
                    are get_dl_added_reconfig_trch and get_dl_added_reconfig_trch_2
08/10/05    vk      If network indicates that DL Trch has the same transport format
                    as the uplink TRch which was specified earlier, then adjust
                    transport block size if more than one logical channel is mapped
                    on the same transport channel for Release 5
07/21/05   vk       Changed some MSG_HIGH to MSG_MED as during HSDPA testing, lot
                    of F3 messages are getting dropped. This is part of the
                    overall effort to reduce un-necessary F3 messages
07/15/05    vk      Updated comments of function rrcllc_copy_pdu_info_per_queue()
06/17/05    vk      If queue information is specified again for an existing DFLOW,
                    configure MAC with new Release Timer and Window Size
06/09/05    da      Added passing in parameter of type boolean in func get_dl_tfcs_info
                    to indicates whether it's for sccpch with ctch or not.
06/07/05    vk      While processing downlink information common for all transport
                    channels for Release 5, added check to include the bit mask
                    for choice before processing it under FEATURE HSDPA
04/22/05    vk      Allow config to pass through if only HARQ is specified in new 
                    configuration. Do not delete the DFLOW on explicit delete and 
                    maintain the RB MApping Info. Just reinitialize the queues 
                    associated with the DFLOW
04/06/05    da      Added Initial support for BMC.  Handling of CTCH  and 
                    a second SCCPCH in IDLE, CELL_PCH and URA_PCH states
                    are now supported.  CTCH is not present in CONNECTING, CELL_FACH
                    and CELL_DCH states.
03/11/05    vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/02/05  ttl   Support RRC ASN1 Release 5.
10/28/04    vn      Corected check in get_dl_added_reconfig_trch to ensure when 
                    "Same as UL" TrCH ID is not found, a Failure is returned.
09/20/04    vn      Fill up TrCH ID in UL Semi Static Parameters going to L1.
8/30/04     sgk     Checked and assigned the right enum values for coding type
                    in function get_dl_added_reconfig_trch and 
                    get_dl_added_reconfig_trch_2 to fix lint error Type 
                    mismatch (assignment) (enum/enum). Wrong values were 
                    assigned earlier. 
                    Type cast tfs in mac_ul_tfs_type to uint8 in functions 
                    get_dl_added_reconfig_trch get_dl_added_reconfig_trch_2
                    to fix lint error Loss of precision (assignment) 
                    (16 bits to 8 bits).
                    Typecast num transport blocks in mac_ul_tf_type to uint8 
                    in function get_dl_added_reconfig_trch_2 to fix lint error
                    Loss of precision (assignment) (16 bits to 8 bits).
                    Removed break after return in function 
                    get_dl_added_reconfig_trch and get_dl_added_reconfig_trch_2
                    to fix lint error 'unreachable'.
                    Made check if (cur_ctfc_entry_index >= UE_MAX_TFC)
                    instead of just '>' in MACRO GET_DL_TFCS_EXPLICIT_CFG_RECFG
                    to fix multiple lint warnings Possible access of 
                    out-of-bounds pointer (1 beyond end of data) by operator '->'.
08/13/04    vn      Minor fix to align with Mar2004 ASN1
08/03/04    jh      Enabled sameAsUplink fix for both functions that
                    process the relevant IE.
07/09/04    jh      ASN.1 field name changes.
05/06/04    jh      Fixed to enable copying of existing UL TFS info to DL
                    when sameAsUplink is specified, but no UL TFS info is
                    present in the message.
02/13/04    sk      Added a trch id check for zero while sorting.
06/27/03    jh      Fixed lint errors.
04/29/03    vn      Made functions generic to populate the config database, a 
                    pointer to which is passed as a parameter. Functions that 
                    deal only with DPCH still are hard-coded for Ordered Config.
03/27/03    vn      Wrapped all RLC size for DCH processing with
                    FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH.
02/05/03    sk      March2002Rel specific changes 
                      Changed  rrc_complete to
                      rrc_ExplicitTFCS_Configuration_complete
                      Changed  rrc_modeSpecificInfo_present to
                      rrc_UL_CommonTransChInfo_modeSpecificInfo_present   
                      Changed rrc_tfs_SignallingMode_explicit to
                      rrc_tfs_SignallingMode_explicit_config
                      Changed explicit to explicit_config
                      Changed  rrc_tm_SignallingInfo_present to
                      rrc_DL_AddReconfTransChInformation_dummy_present 
11/15/02    jh      Changed get_ul_deleted_trch() and get_dl_deleted_trch() to
                    use the information in the UL/DL_DeletedTransChInfoList IE
                    to determine the TrCH type rather than relying on the
                    RRC state.
10/08/02    vn      In get_dl_info_common_for_all_trch, changed to ignore
                    if no UL TFCS included and DL TFCS is "same as UL".
10/04/02    jh      Added support for RLC size restriction: Added function
                    to extract Logical Channel List information from its IE.
                    Added code to process this IE in
                    get_ul_ded_trch_dynamic_tfs_info() and
                    get_ul_common_trch_dynamic_tfs_info().
06/07/02    vn      Fixed rrcllc_delete_ul_dch_trch_index to decrement TrCH
                    count in Uplink TFCS also.
06/03/02    vn      Pass on the Reference TFC ID in the signalled gain factors
                    when FEATURE_WCDMA_COMP_GF is defined. 
05/07/02    vn      Support for Transport Channel Reconfiguration: Update
                    Reconfig needed for DL and UL MAC and L1 whwnever TFCS
                    info is processed or TrCH is added or reconfigured. Changes
                    to ensure the holes created by removed TFCIs are remembered
                    in Ordered Config. Needed for procrssing removed TFCs.
04/22/02    vn      No separate common and dedicated TrCH info is maintained
                    anymore as either one or the other is valid. Changes in 
                    functions that accessed common and dedicated TrCH info.
04/04/02    vn      While calculating TB size, do not add MAC header length
                    for an RLC size of 0. Change due to new MAC header name. 
03/05/02    vn      Change due to change in macro name for MAC Header length.
03/04/02    vn      Added Cell_FACH support and Cleanup/Restructuring.
                    Initialize TFCS info when complete TFCS info comes in IE.
                    Process info common to all TrCHs (UL and DL)only if next
                    state is Cell_DCH. Pointer to ordered_config_type passed in
                    init functions. Uplink and downlink TrCH count is not 
                    maintained anymore as that can be computed from MAC info.
                    Set "allowed" in get_dl_tfs_common_trch to TRUE for a 
                    valid Transport format. Changed all MSG_ERRORs to ERRs.
                    Changes due to moving of all local structures to within 
                    ordered_config_type (rrcllc_tfcs, dl_dch_trch_info etc).
12/14/01    rj/kc   Fixed ARM Compilation errors.
12/04/01    kc      Changes for June spec conversion.  Mostly asn1 changes.
11/16/01    vn      Merged the following from branch -
                    Support for non-sequential Transport Channel IDs:
                    Changed logic in get_ul/dl_added_reconfig_trch and get_ul/
                    dl_deleted_trch. Added new functions to delete UL/DL TrCH
                    index and clean up all TrCH info in L1, MAC and local 
                    structures, copy MAC DL DCH TrCH info, copy MAC UL DCH TrCH
                    mapping and TFS info, copy all DL/UL DCH related info to/
                    from buffer and from one TrCH Index to another, sort TrCH
                    index array based on TrCH ID and functions to handle 
                    added/reconfig or deleted UL/DL DCH info and re-arrange 
                    TrCH info by TrCH IDs.
11/09/01    kmp     Added support to process BLER value received over the air
                    if FEATURE_L1_BLER_QUAL_CTL is defined.
11/07/01    vn      Fixed BLER comparison. Checks for NULL pointer in 
                    get_dl_transport_format_set. Checks correctly for the limit
                    of max transport formats in get_dl_tfs_dedicated_trch and 
                    get_dl_tfs_common_trch.
10/31/01    kmp     Fixed so that absence of TFC subset (MD IE) is not flagged
                    as error when processing UL Common Transport Channel info.
10/29/01    vn      Fixed a memory scribble associated with handling of 
                    Reference TFC ID for a signalled gain factor.
09/25/01    kmp     Added support for num_tfci in the uplink L1 CCTrCH info
                    in the GET_UL_CTFC_BIT_INFO macro. Correctly check the
                            tfc index against UE_MAX_TFC instead of UE_MAX_TRCH.
                    Replace all references of L1_MAX_TFC with UE_MAX_TFC. 
                    Replace all references of L1_MAX_TF with MAX_TF.
09/11/01    vn      Removed reference to ref_rfc_tab as that has been removed
                    from  l1_ul_cctrch_parms.
07/03/01    kmp     Fixed a bug in get_dl_deleted_trch() to correctly
                    subtract the number of DCHs or FACHs from the MAC DB.
06/28/01    kmp     get_ul_deleted_trch() was modified due to a change in
                    the rrcllc_init_rach_rb_mapping_info() parameter list.
                    get_dl_deleted_trch() fixed a bug where the RACH RB
                    mapping info was cleared, and the FACH RB mapping should
                    have been.
06/25/01    kmp     Fixed bugs in get_ul_deleted_trch() and
                    get_dl_deleted_trch().
06/20/01    kmp     Updated macro GET_UL_TRCH_DYNAMIC_TFS_IE_PTR to update
                    the layer 1 UL TTI type.
06/18/01    kmp     Fixed bugs in get_ul_deleted_trch() 
                    and get_dl_deleted_trch(). Added debug info to the same
                    functions.
06/15/01    kmp     Fixed a bug in get_ul_ded_trch_dynamic_tfs_info() and 
                    get_ul_com_trch_dynamic_tfs_info() which incorrectly
                    extracts the uplink TFS info. Fixed a bug in
                    get_ul_deleted_trch() where not all counts were
                    correctly cleared.
06/13/01    kmp     Added get_dl_deleted_trch() and get_ul_deleted_trch.
                    updated in numerous places to remove the fact that the
                    TrCH index is removed from rrcllcoc_trch_info_type
                    structure.
06/07/01    kmp     Fixed ARM compiler warning for an embedded comment.
06/06/01    kmp     Fixed bugs in GET_DL_TFCS_EXPLICIT_CFG_RECFG and
                    GET_DL_TFCS_EXPLICIT_CFG_ADD macros. Fixed bugs in 
                    get_dl_tfs_dedicated_trch(), get_dl_added_reconfig_trch() 
                    and get_dl_added_reconfig_trch_2(). Added more Debug
                    information for processing TrCH IEs. Fixed a bug while
                    processing the rlc_size and num_trblks in
                    get_ul_ded_trch_dynamic_tfs_info().
06/05/01    kmp     In get_ul_added_reconfig_trch() moved updating CCTrCH 
                    type to get_ul_dch_mapping_info() in rrcllcrbie.c.
                    In get_dl_added_reconfig_trch() and 
                    get_dl_added_reconfig_trch2() correctly set the 
                    multiple logical channel boolean in 
                    get_dl_transport_format_set(). get_dl_tfs_dedicated_trch()
                    replaced 4 with macro RRCLLC_CELL_DCH_MAC_HEADER_SIZE.
05/24/01    kmp     In get_ul_gain_factor_info() changed to a uint8 the 
                    reference TFC id for gain factor calculations instead of
                    using the l1_ul_ref_tfc_tab_struct_type which l1 nuked.
                    In get_ul_added_reconfig_trch() removed setting of the 
                    valid boolean indicating a good CCTrCH since l1 nuked
                    this field. Fixed bugs in get_dl_added_reconfig_trch() 
                    and get_ul_added_reconfig_trch() where the TrCH ID was 
                    inadvertantly written and where ndchs was improperly 
                    incremented.
05/21/00    kmp     In get_ul_common_trch_dynamic_tfs_info() updated to
                    correctly index looping through TF information. In
                    GET_UL_CTFC_BIT_INFO macro change ie_ptr to ##ie_ptr##.
                    In get_ul_ded_channel_dynamic_tfs_info() updated to 
                    correctly index looping through TF information.
05/17/01    kmp     In get_ul_semi_static_transport_format_info() fixed a
                    range check error. In GET_DL_TFCS_EXPLICIT_CFG_RECFG &
                    GET_DL_TFCS_EXPLICIT_CFG_ADD macros, changed the 
                    cur_ctfc_entry_index - 1 to cur_ctfc_entry_index. Fixed
                    a bug in get_dl_semistatic_tf_info().
05/16/01    kmp     Fixed the following: Fixed a ptr init problem in 
                    get_tfs_com_rlc_size(). Removed support for the ctfc_used
                    global which was used to fill in the ctfc table. Updated
                    get_ul_add_tfcs_info() and get_ul_normal_tfci_info() to
                    remove that info. In get_dl_tfs_common_trch(), change loop
                    until tf_ifo_list !=NULL. In get_dl_transport_format_set(),
                    change return type to be of uecomdef_status_e_type.
                    Changed get_tfs_ded_rlc_size() and get_tfs_com_rlc_size()
                    to return uint32 instead of uint16. Fixed compiler warnings.
05/09/01    kmp     Updated with DL TrCH info, and UL TrCH high level IE
                    processing functions.
04/13/01    kmp     Merged in the Uplink TFS and TFCS info. Also the common
                    functions shared between uplink and downlink processing.
03/02/01    kmp     Initial release, just as a holding in case something 
                    happened while on vacation.
===========================================================================*/


/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "err.h"
#include "l1dlcctrchcfg.h"
#include "l1dlphychancfg.h"
#include "l1sapcommon.h"
#include "l1rrcif.h"
#include "l1task_v.h"
#include "l1ulcfg.h"
#include "macrrcif_v.h"
  #include "macl1rrcif.h"
#include "msg.h"
#include "rlcrrcif_v.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrcllc.h"
#include "rrcllcoc.h"
#include "rrcsibdb.h"
#include "uecomdef.h"
#include "rrcscmgr.h"
#include "rrcmeasi.h"
#include "rrcnv.h"
#include "rrcrbcommon.h"
#include "rrclcm.h"
#ifdef FEATURE_WCDMA_HS_FACH
#include "rrcenhstatecfg.h"
#endif
/*===================================================================
                        DATA DECLARATIONS
=====================================================================*/

/* This is a temporary storage buffer for all downlink DCH specific info */
rrcllc_dl_dch_info_buffer_type  dl_dch_info_buffer; 

/* This is a temporary storage buffer for all uplink DCH info */
rrcllc_ul_dch_info_buffer_type  ul_dch_info_buffer;

#ifdef FEATURE_WCDMA_HS_FACH
rb_mapping_info_type_  *rrc_ordered_hanging_rb;
rb_mapping_info_type_  *rrc_current_hanging_rb;
#endif

rb_mapping_info_type *rrc_ordered_hanging_rb_mapping;
rb_mapping_info_type *rrc_current_hanging_rb_mapping;

extern boolean ul_tfcs_needed;
extern boolean dl_tfcs_needed;
extern boolean ul_tfcs_rcvd; 
extern boolean dl_tfcs_rcvd; 
/*===================================================================
                        CONSTANTS
=====================================================================*/

/*===================================================================
                        FORWARD DECLARATIONS
=====================================================================*/

/*===================================================================
*                       FUNCTION PROTOTYPES
====================================================================*/

/*===================================================================
                            FUNCTIONS
=====================================================================*/

/*===================================================================
                            MACROS AND FUNCTIONS
=====================================================================*/
/*===================================================================
MACRO       :   GET_UL_TRCH_DYNAMIC_TFS_IE_PTR 

DESCRIPTION :   Gets pointer to Transport Format Set information in the 
              received IE Saves TTI type in MAC Ordered Config Database
              Sets 'rv' to 'FAILURE' or 'SUCCESS' depending on whether
              the chosen TTI types is supported or not supported or 
              invalid.

DEPENDENCIES:   None

RETURN VALUE:   None.

SIDE EFFECTS:   None.

=====================================================================*/
#define     GET_UL_TRCH_DYNAMIC_TFS_IE_PTR(chan_type)\
\
    /* Pick TFS pointer based on the TTI type */ \
    switch (ie_ptr->tti.t){ \
\
        /* 10 msec TTI */ \
        case T_rrc_##chan_type##TransChTFS_tti_tti10: \
\
            /* Pointer to 10 msec TFC information */ \
            tfs_ptr = ie_ptr->tti.u.tti10; \
\
            /* Save TTI type info in UL MAC TFCS structure */ \
            oc_tfs_ptr->tti_info = L1_TTI_10MS;\
            oc_ssp_ptr->tti = L1_TTI_10MS;\
\
            /* Set Return value to SUCCESS */   \
            rv = SUCCESS;   \
\
            break;\
\
        /* 20 msec TTI */\
        case T_rrc_##chan_type##TransChTFS_tti_tti20:\
\
            /* Pointer to 10 msec TFC information */\
            tfs_ptr = ie_ptr->tti.u.tti20;\
\
            /* Save TTI type info in UL MAC TFCS structure */\
            oc_tfs_ptr->tti_info = L1_TTI_20MS;\
            oc_ssp_ptr->tti = L1_TTI_20MS;\
\
            /* Set Return value to SUCCESS */   \
            rv = SUCCESS;   \
\
            break;\
\
        /* 40 msec TTI */\
        case T_rrc_##chan_type##TransChTFS_tti_tti40:\
\
            /* Pointer to 10 msec TFC information */\
            tfs_ptr = ie_ptr->tti.u.tti40;\
\
            /* Save TTI type info in UL MAC TFCS structure */ \
            oc_tfs_ptr->tti_info = L1_TTI_40MS; \
            oc_ssp_ptr->tti = L1_TTI_40MS;\
\
            /* Set Return value to SUCCESS */   \
            rv = SUCCESS;   \
\
            break;\
\
        /* 80 msec TTI */\
        case T_rrc_##chan_type##TransChTFS_tti_tti80:\
\
            /* Pointer to 10 msec TFC information */ \
            tfs_ptr = ie_ptr->tti.u.tti80; \
\
            /* Save TTI type info in UL MAC TFCS structure */ \
            oc_tfs_ptr->tti_info = L1_TTI_80MS; \
            oc_ssp_ptr->tti = L1_TTI_80MS;\
\
            /* Set Return value to SUCCESS */   \
            rv = SUCCESS;   \
\
            break; \
\
        /* Dynamic TTI */ \
        case T_rrc_##chan_type##TransChTFS_tti_dynamic: \
\
            /* Log an error message */ \
             WRRC_MSG0_ERROR("Dynamic TTI's are not supported in FDD");\
\
            /* Set Return value to FAILURE */   \
            rv = FAILURE;   \
\
            break; \
\
        /* Invalid TTI type */ \
        default: \
\
            /* Log an error message */ \
            WRRC_MSG1_ERROR("Invalid TTI type %d rcv'd in TFS info",oc_tfs_ptr->tti_info); \
\
            /* Set Return value to Failure */   \
            rv = FAILURE;   \
            break; \
    }


/*===================================================================
MACRO       : GET_UL_CTFC_BIT_INFO

DESCRIPTION : Gets CTFC info from IE and saves it in UL MAC Data 
              Structure

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
=====================================================================*/
#define     GET_UL_CTFC_BIT_INFO(ie_ptr, ctfc_val)\
\
    while ((ie_ptr->n >idx) && (tfc_idx < L1_UL_TFC_MAX)){\
\
        /* Get TFC index */\
    if ((config_ptr->num_removed_tfcis > 0) &&  \
         (config_ptr->num_removed_tfcis < UE_MAX_TFC)){ \
\
        /* Get the first available position */ \
          tfc_idx = config_ptr->removed_tfci_list[0];   \
\
          /* Decrement the removed TFCI count */ \
          config_ptr->num_removed_tfcis--; \
\
          /* Adjust the removed tfc list */ \
\
          for (i=0; i < config_ptr->num_removed_tfcis; i++) \
              config_ptr->removed_tfci_list[i] = config_ptr->removed_tfci_list[i+1]; \
      } \
      /* There are no holes in the TFCS Table */ \
      else {    \
          /* TFC is added at the end */ \
          tfc_idx = config_ptr->ul_tfcs.ntfc;   \
      } \
\
        /* TFC Index is within maximum value */ \
        if (tfc_idx < L1_UL_TFC_MAX) { \
\
            /* Pointer to TFC info info OC data */\
            oc_tfc_ptr = &config_ptr->ul_tfcs.tfc_info[tfc_idx];\
\
            /* Set TFC Control Flag to allowed */\
            oc_tfc_ptr->control_flag = TFC_ALLOWED;\
\
            /* Save the CTFC value */\
            oc_tfc_ptr->ctfc = (uint32)(ie_ptr->elem[idx].ctfc_val); \
\
            /* Save the Power Offset Information */\
            if (ie_ptr->elem[idx].m.powerOffsetInformationPresent)\
                (void)get_ul_gain_factor_info (config_ptr->rrc_ul_cctrch_params.gain_factor_parms, &ie_ptr->elem[idx].powerOffsetInformation,(uint8)tfc_idx);\
\
            /* Get the pointer to the next TFC */\
            idx++;\
\
            /*Increment TFC count */\
            if(tfc_idx == config_ptr->ul_tfcs.ntfc) { \
\
                config_ptr->ul_tfcs.ntfc++;\
                config_ptr->rrc_ul_cctrch_params.num_tfci++; \
            }\
        }   \
        /* TFC index has exceeded the maximum value */  \
        else {  \
             WRRC_MSG0_ERROR("Invalid number of Transport Formats");   \
            rv = FAILURE;   \
            break;  \
        }   \
\
    }

/*====================================================================
MACRO: GET_DL_TFCS_EXPLICIT_CFG_RECFG

DESCRIPTION:
  This macro is gets the CTFC values from the TFCS information in
  case of the TFCS explicit complete reconfiguration.
  
  Input parameters:
    tfc_ptr : pointer to the TFC information.
    ctfc : CTFC value structure member name.
  
  Assmes:
    cur_ctfc_entry_index : CTFC entry array index where CTFC values
                           are to be stored.
    this_ctfc_entry : pointer to current CTFC entry structure.
    dl_tfcs_info : RRC DL TFCS info structure pointer
====================================================================*/
#define GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ptr, ctfc) \
          if (tfc_ptr == NULL) \
          { \
             WRRC_MSG0_ERROR("Min 1 TFC extected, Exp. cfg complete recfg");\
            return FAILURE; \
          } \
 \
          while (tfc_ptr->n > idx) \
          { \
            if (cur_ctfc_entry_index >= UE_MAX_TFC) \
            { \
               WRRC_MSG0_ERROR("Too many TFC, Exp cfg complete recfg");\
              return FAILURE; \
            } \
            this_ctfc_entry = &dl_tfcs_info->ctfc_entry_data[cur_ctfc_entry_index]; \
            this_ctfc_entry->state = ALLOWED; \
            this_ctfc_entry->ctfc_value = (uint32) (tfc_ptr->elem[idx].ctfc); \
            idx++; \
            cur_ctfc_entry_index++; \
          } \
 \
          dl_tfcs_info->last_ctfc_entry_index = (uint16) (cur_ctfc_entry_index);


/*====================================================================
MACRO: GET_DL_TFCS_EXPLICIT_CFG_ADD

DESCRIPTION:
  This macro is gets the CTFC values from the TFCS information in
  case of the TFCS explicit TFC ADD.
  
  Input parameters:
    tfc_ptr : pointer to the TFC information.
    ctfc : CTFC value structure member name.
  
  Assmes:
    cur_ctfc_entry_index : CTFC entry array index where CTFC values
                           are to be stored.
    this_ctfc_entry : pointer to current CTFC entry structure.
    dl_tfcs_info : RRC DL TFCS info structure pointer
====================================================================*/
#define GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ptr, ctfc) \
          if (tfc_ptr == NULL) \
          { \
             WRRC_MSG0_ERROR("Min 1 TFC extected, Exp. cfg complete recfg");\
            return FAILURE; \
          } \
 \
          while (tfc_ptr->n > idx) \
          { \
            if (cur_ctfc_entry_index >= UE_MAX_TFC) \
            { \
               WRRC_MSG0_ERROR("Not enough free TFCI, Exp cfg ADD");\
              return FAILURE; \
            } \
            while (dl_tfcs_info->ctfc_entry_data[cur_ctfc_entry_index].state != \
                   INVALID) \
            { \
              cur_ctfc_entry_index++; \
              if (cur_ctfc_entry_index >= UE_MAX_TFC) \
              { \
                 WRRC_MSG0_ERROR("Not enough free TFCI, Exp cfg ADD");\
                return FAILURE; \
              } \
            } \
\
            if (cur_ctfc_entry_index >= UE_MAX_TFC) \
            { \
               WRRC_MSG0_ERROR("Not enough free TFCI, Exp cfg ADD");\
              return FAILURE; \
            } \
\
            this_ctfc_entry = &dl_tfcs_info->ctfc_entry_data[cur_ctfc_entry_index]; \
            this_ctfc_entry->state = ALLOWED; \
            this_ctfc_entry->ctfc_value = (uint32) (tfc_ptr->elem[idx].ctfc); \
            idx++; \
            cur_ctfc_entry_index++; \
          } \
 \
          if ((cur_ctfc_entry_index > 0) && ( dl_tfcs_info->last_ctfc_entry_index == RRC_DL_INVALID_CTFC_VALUE))  \
          { \
            dl_tfcs_info->last_ctfc_entry_index = 0 ; \
          } \
          if ((cur_ctfc_entry_index - 1) > dl_tfcs_info->last_ctfc_entry_index) \
          { \
            dl_tfcs_info->last_ctfc_entry_index = (uint16) (cur_ctfc_entry_index); \
          }
/*===================================================================
MACRO       : GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB

DESCRIPTION : Gets CTFC info from IE and saves it in UL MAC Data 
              Structure

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
=====================================================================*/
#define     GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ptr, ctfc_val)\
\
    while ((ie_ptr->n > idx) && (tfc_idx < L1_UL_TFC_MAX)){\
          tfc_idx = prach_sel_info_ptr->ul_tfcs.ntfc;   \
\
        /* TFC Index is within maximum value */ \
        if (tfc_idx < L1_UL_TFC_MAX) { \
\
            /* Pointer to TFC info info OC data */\
            oc_tfc_ptr = &prach_sel_info_ptr->ul_tfcs.tfc_info[tfc_idx];\
\
            /* Set TFC Control Flag to allowed */\
            oc_tfc_ptr->control_flag = TFC_ALLOWED;\
\
            /* Save the CTFC value */\
            oc_tfc_ptr->ctfc = (uint32)(ie_ptr->elem[idx].ctfc_val); \
\
            /* Save the Power Offset Information */\
            if (ie_ptr->elem[idx].m.powerOffsetInformationPresent)\
                (void)get_ul_gain_factor_info (prach_sel_info_ptr->gain_factor_parms, &ie_ptr->elem[idx].powerOffsetInformation,(uint8)tfc_idx);\
\
            /* Get the pointer to the next TFC */\
            idx++;\
\
            /*Increment TFC count */\
            if(tfc_idx == prach_sel_info_ptr->ul_tfcs.ntfc) { \
\
                prach_sel_info_ptr->ul_tfcs.ntfc++;\
                prach_sel_info_ptr->l1_ul_cctrch_parms.num_tfci++; \
            }\
        }   \
        /* TFC index has exceeded the maximum value */  \
        else {  \
             WRRC_MSG0_ERROR("Invalid number of Transport Formats");   \
            rv = FAILURE;   \
            break;  \
        }   \
\
    }


/*====================================================================
FUNCTION: get_tfs_num_tb()

DESCRIPTION:
  This function gets the number of TB from the TF info. It is a
  utility function for getting the part of TFS information.
    
DEPENDENCIES:
  None

RETURN VALUE:
  number of transport blocks.
  0xFF in case of error.

SIDE EFFECTS:
  None.
====================================================================*/
uint8 get_tfs_num_tb
(
  /* RRC Dedicated TFS Dynamic TF info */
  rrc_NumberOfTransportBlocks *ie_ptr
)
{
  /* function local variables */
  /* ------------------------ */

  uint8 num_tb = 0;

  switch (ie_ptr->t)
  {
    case T_rrc_NumberOfTransportBlocks_zero:
      num_tb = 0;
      break;
    case T_rrc_NumberOfTransportBlocks_one: 
      num_tb = 1;
      break;
    case T_rrc_NumberOfTransportBlocks_small_:
      num_tb = (uint8)ie_ptr->u.small_;
      break;
    case T_rrc_NumberOfTransportBlocks_large_:
      num_tb = (uint8)ie_ptr->u.large_;
      break;
    default:
      WRRC_MSG1_ERROR("Invalid RRC DL number of TB type %d",
        ie_ptr->t);
      return RRCLLC_INVALID_NUM_TB;
  }

  return num_tb;
} /* END get_tfs_num_tb() */

/*====================================================================
FUNCTION: rrcllc_dch_usch_add_trch_info()

DESCRIPTION:
  

DEPENDENCIES:
  None

RETURN VALUE:


SIDE EFFECTS:
  None.
====================================================================*/
static uecomdef_status_e_type rrcllc_dch_usch_add_trch_info
(
  tr_ch_id_type trch_id,
  rrc_TransportFormatSet * transport_format_set
)
{
    /* local transport channel variables */
     uint8 trch_idx = RRCLLC_INVALID_TRCH_IDX;

    /*
    * Use the Transport Channel Index and next state to place the TFS in 
    * the correct slot in the MAC TFS array.
    */
    if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
    {
      /* Get the transport channel index for this transport channel ID.
       * Note: The index should already be there.
       */
      trch_idx = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr,
                                             trch_id);
      if((trch_idx == RRCLLC_INVALID_TRCH_IDX) ||
        (trch_idx == UE_MAX_TRCH)
        )
      {
        WRRC_MSG1_ERROR("UL DCH TrCH ID %d, no idx found",
          trch_id);
        return(FAILURE);
      }
      if(rrcllc_restore_ul_dch_rb_mapping_info(trch_idx) == FAILURE)
      {
        /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
        WRRC_MSG1_ERROR("UL DCH TrCH ID %d, info not correct in Standalone RB-Mapping",
          trch_id);
      }

      /* get the Transport Format Set for this TrCH */
      MSG_MED("Get UL TFS for TrCH ID %d", trch_id, 0, 0);
  
      if(get_ul_transport_format_set(ordered_config_ptr, 
                                     transport_format_set, 
                                     (uint8)trch_idx)
          == FAILURE)
      {
        return FAILURE;
      }
    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
 PDU size has changed.*/ 

      rrcllc_get_rb_for_rb_list_mapped_to_dch(trch_id, 
                ordered_config_ptr->rrc_state_indicator, UL);
    
      /* Indicate that Uplink MAC and L1 need to be reconfigured */
      ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
      ordered_config_ptr->reconfig_needed.uplink_l1 = TRUE;
    }
    else
    {
    WRRC_MSG1_ERROR("RRC State %d not supported for TrCH, return FAILURE",ordered_config_ptr->rrc_state_indicator);
    return FAILURE;
    }
    return SUCCESS;
  }
/*============================================================================
FUNCTION: rrc_convert_num_harq_process()

DESCRIPTION:
  
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

static uint8 rrc_convert_num_harq_process
(
 rrc_HARQ_Info_r7* harq_info
)
{
    uint8 num_harq_procs=0;
    switch(harq_info->numberOfProcesses)
    {
      case rrc_HARQ_Info_r7_numberOfProcesses_n1 :
        num_harq_procs= 1;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n2 :
        num_harq_procs= 2;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n3:
        num_harq_procs= 3;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n4 :
        num_harq_procs= 4;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n5:
        num_harq_procs= 5;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n6:
        num_harq_procs= 6;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n7:
        num_harq_procs= 7;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n8:
        num_harq_procs= 8;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n12:
        num_harq_procs= 12;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n14:
        num_harq_procs= 14;
        break;
      case rrc_HARQ_Info_r7_numberOfProcesses_n16:
        num_harq_procs= 16;
        break;
        
      default:
        WRRC_MSG1_ERROR("Invalid value for Num Harq processes %d",harq_info->numberOfProcesses);    
        break;
    }

    return num_harq_procs;
}
/*============================================================================
FUNCTION: set_harq_information_r7()

DESCRIPTION:
  This function sets the HARQ information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type set_harq_information_r7
(
  rrc_HARQ_Info_r7* harq_info_ptr
)
{
  /* Set HARQ Information */
  rrc_HARQ_Info_r7_memorySize* harq_head_ptr = NULL;

  uint8 cnt = 0;
  
  /*Indicate that HSDSCH Info is present which would be used later to set bit mask when HSDPA
      is already ACTIVE */
  hsdpa_msg_params.msg_params_present = TRUE;
  hsdpa_msg_params.hsdsch_info_present = TRUE;
  
  ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.num_proc = 
    rrc_convert_num_harq_process( harq_info_ptr);
  
  MSG_MED("RRCHS:No of processes %d",harq_info_ptr->numberOfProcesses,0,0); 

  if (RRC_CHECK_COMMON_MSG_TYPE(harq_info_ptr->memoryPartitioning,
   rrc_HARQ_Info_r7_memoryPartitioning_implicit))
  {
    ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_part_info_valid = FALSE;
    MSG_MED("RRCHS:Implicit Mem Partition",0,0,0); 
  }
  else
  {
    ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_part_info_valid = TRUE;
    
    MSG_MED("RRCHS:Explicit Mem Partition",0,0,0); 
    
    if ((harq_head_ptr = &harq_info_ptr->memoryPartitioning.u.explicit_->memorySize) == NULL)
    {
      WRRC_MSG0_ERROR("RRCHS:HARQ Mem Part Ptr NULL");
      return FAILURE;
    }
    while ((harq_head_ptr->n > cnt) && (cnt < L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO))
    {
      ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_size_type[cnt] = 
        (l1_dl_hs_dsch_harq_mem_sz_enum_type) harq_head_ptr->elem[cnt];
      cnt++;
    }
#ifdef FEATURE_WCDMA_MIMO
    if((harq_info_ptr->memoryPartitioning.u.explicit_->m.additionalMemorySizesForMIMOPresent) &&
         (harq_info_ptr->memoryPartitioning.u.explicit_->additionalMemorySizesForMIMO.n > 0 ))
    {
        uint32 idx =0;
        rrc_HARQ_Info_r7_additionalMemorySizesForMIMO * add_harq_info_head_ptr = NULL;
        add_harq_info_head_ptr = &harq_info_ptr->memoryPartitioning.u.explicit_->additionalMemorySizesForMIMO;

        ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mimo_additional_mem_part_info_valid = TRUE;
        while ((add_harq_info_head_ptr->n > idx) && (cnt < L1_DL_HS_DSCH_HARQ_MAX_PROC_MIMO))
        {
          ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_size_type[cnt++] = 
            (l1_dl_hs_dsch_harq_mem_sz_enum_type) add_harq_info_head_ptr->elem[idx];
          
          idx++;          
        }
    }
    else if(ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.num_proc > L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO )
    {
      WRRC_MSG1_ERROR("RRCHSPA+: Additional memory for Mimo not provided for num process %d > 8 ",
            ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.num_proc);
        return FAILURE;
    }
#else /*FEATURE_WCDMA_MIMO*/
    if(harq_info_ptr->memoryPartitioning.u.explicit_->m.additionalMemorySizesForMIMOPresent)
    {
      WRRC_MSG0_ERROR("RRCHSPA+: Additioanl memory for Mimo not supported");
    }
#endif /*FEATURE_WCDMA_MIMO*/
    WRRC_MSG2_HIGH("RRCHS:No of HARQ mem sizes %d num harq %d ",cnt,ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.num_proc);
  }
  return SUCCESS;

}


/*====================================================================
FUNCTION: get_tfs_ded_rlc_size()

DESCRIPTION:
  This function gets the RLC size for the deicated channel. The RLC size
  returned by this function is RLC PDU size. It is a utility function
  for getting the TFS info parts.
  
  See the 25.331 'Message and Information Element abstract syntax
  (with ASN.1)' PDU definition for TrCh IEs 2000-12 V3.5.0 Page 492-498
  for getting the RLC size for Bitmode or Octet mode parts.
    
DEPENDENCIES:
  None

RETURN VALUE:
  16 bit RLC size.
  0xFFFF in case of error

SIDE EFFECTS:
  None.
====================================================================*/
uint32 get_tfs_ded_rlc_size
(
  /* RRC Dedicated TFS Dynamic TF info */
  rrc_DedicatedDynamicTF_Info *ie_ptr
)
{
  /* function local variables */
  /* ------------------------ */

  /* initialize the RLC size to 0 */
  uint32 rlc_size = RRCLLC_INVALID_RLC_SIZE;

  switch (ie_ptr->rlc_Size.t)
  {
    case T_rrc_DedicatedDynamicTF_Info_rlc_Size_bitMode:
      switch (ie_ptr->rlc_Size.u.bitMode->t)
      {
        case T_rrc_BitModeRLC_SizeInfo_sizeType1:
          rlc_size = ie_ptr->rlc_Size.u.bitMode->u.sizeType1;
          break;
        case T_rrc_BitModeRLC_SizeInfo_sizeType2:
          rlc_size = (ie_ptr->rlc_Size.u.bitMode->u.sizeType2->part1 * 8) + 128;
          if (ie_ptr->rlc_Size.u.bitMode->u.sizeType2->m.part2Present)
          {
            rlc_size += ie_ptr->rlc_Size.u.bitMode->u.sizeType2->part2;
          }
          break;
        case T_rrc_BitModeRLC_SizeInfo_sizeType3:
          rlc_size = (ie_ptr->rlc_Size.u.bitMode->u.sizeType3->part1 * 16) + 256;
          if (ie_ptr->rlc_Size.u.bitMode->u.sizeType3->m.part2Present)
          {
            rlc_size += ie_ptr->rlc_Size.u.bitMode->u.sizeType3->part2;
          }
          break;
        case T_rrc_BitModeRLC_SizeInfo_sizeType4:
          rlc_size = (ie_ptr->rlc_Size.u.bitMode->u.sizeType4->part1 * 64) + 1024;
          if (ie_ptr->rlc_Size.u.bitMode->u.sizeType4->m.part2Present)
          {
            rlc_size += ie_ptr->rlc_Size.u.bitMode->u.sizeType4->part2;
          }
          break;
        default:
          WRRC_MSG1_ERROR("Invalid RRC TF type in BitMode %d", ie_ptr->rlc_Size.u.bitMode->t);
          break;
      }
      break;

    case T_rrc_DedicatedDynamicTF_Info_rlc_Size_octetModeType1:
      switch (ie_ptr->rlc_Size.u.octetModeType1->t)
      {
        case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
          rlc_size = (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType1 * 8) + 16;
          break;
        case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
          rlc_size = (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType2->part1 * 32) + 272;
          if (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType2->m.part2Present)
          {
            rlc_size += (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType2->part2 * 8);
          }
          break;
        case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
          rlc_size = (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType3->part1 * 64) + 1040;
          if (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType3->m.part2Present)
          {
            rlc_size += (ie_ptr->rlc_Size.u.octetModeType1->u.sizeType3->part2 * 8);
          }
          break;
        default:
          WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
            ie_ptr->rlc_Size.u.octetModeType1->t);
          break;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid RRC RLC size mode %d", ie_ptr->rlc_Size.t);
      break;
  }

  return rlc_size;
} /* END get_tfs_ded_rlc_size() */

/*====================================================================
FUNCTION: get_tfs_com_rlc_size()

DESCRIPTION:
  This function gets the RLC size for the common channel. The RLC size
  returned by this function is always equal to the TB size. It is a
  utility function for getting the TFS info parts.
  
  See the 25.331 'Message and Information Element abstract syntax
  (with ASN.1)' PDU definition for TrCh IEs 2000-12 V3.5.0 Page 492-498
  for getting the RLC size for Bitmode or Octet mode parts.
    
DEPENDENCIES:
  None

RETURN VALUE:
  16 bit RLC size (This is always the TB size)
  0xFFFF in case of error

SIDE EFFECTS:
  None.
====================================================================*/
uint32 get_tfs_com_rlc_size
(
  /* RRC Dedicated TFS Dynamic TF info */
  rrc_CommonDynamicTF_Info *ie_ptr
)
{
  /* function local variables */
  /* ------------------------ */
  
  uint32 rlc_size = RRCLLC_INVALID_RLC_SIZE;
  rrc_OctetModeRLC_SizeInfoType2 *rlc_size_info;

  rlc_size_info = &(ie_ptr->rlc_Size.u.fdd->octetModeRLC_SizeInfoType2);
  
  /* get RLC size baed on the FDD or TDD mode */
  switch (ie_ptr->rlc_Size.t)
  {
    case T_rrc_CommonDynamicTF_Info_rlc_Size_fdd:
      /* Get the RLC size in TDD mode */
      switch (ie_ptr->rlc_Size.u.fdd->octetModeRLC_SizeInfoType2.t)
      {
        case T_rrc_OctetModeRLC_SizeInfoType2_sizeType1:
          rlc_size = (rlc_size_info->u.sizeType1 * 8) + 48;
          break;
        case T_rrc_OctetModeRLC_SizeInfoType2_sizeType2:
          rlc_size = (rlc_size_info->u.sizeType2 * 16) + 312;
          break;
        case T_rrc_OctetModeRLC_SizeInfoType2_sizeType3:
          rlc_size = (rlc_size_info->u.sizeType3 * 64) + 1384;
          break;
        default:
          WRRC_MSG0_ERROR("RRC DL TFS cfg: Unknown TF size type");
          break;
      } /* End switch-case, size types for RLC size in FDD mode  */
      break;

    case T_rrc_CommonDynamicTF_Info_rlc_Size_tdd:
    default:
      WRRC_MSG1_ERROR("RRC DL TFS cfg: Com RLCsize in TDD mode/Unknown RLCsize Type %d",ie_ptr->rlc_Size.t);
      break;

  } /* End switch-case, RLC size mode TDD/FDD */

  return rlc_size;
} /* END get_tfs_com_rlc_size() */

/*====================================================================
FUNCTION: get_ul_gain_factor_info()

DESCRIPTION:
  This function get the UL gain factor information. The gain factor
  information is of 2 types.
  Signalled Type
  Computed Type
  
  In the Signalled Mode the gain factor information for this TFC Id
  (passed as the parameter to function) can be assigned a reference
  TFC Id. Note that this Reference TFC Id is different from the real
  TFC Id.
  This reference TFC Id is used in Computed Gain factor type in which
  rather than giving all the gain factors as in signalled mode only
  reference TFC Id is given. In this case the UL is supposed to use the
  Computed Gain factors from the TFC Id that had the reference TFC Id
  as already indicated in signalled mode TFC Id.
  There can be maximum 4 Reference TFC Id
  
  Example:
  
  -----------------------------------------------------
  TFCI Id   Mode    Gain Factors     Reference TFC Id
                     Beta C  Beta D
  -----------------------------------------------------
     0      SIG        X0     Y0            -
     1      SIG        X1     Y1            -
     2      SIG        X2     Y2            0
     ........
     25     SIG        X25    Y25           1
     ........
     37     COMPUTED    -      -            0 // Use gain factor of TFC Id 2
     ........
     41     SIG        X41    Y41           2
     ........
     62     COMPUTED    -      -            1 // Use gain factor of TFC Id 25
     63     COMPUTED    -      -            2 // Use gain factor of TFC Id 41
  -----------------------------------------------------
  
  
  Reference TFC Id list (MAX 4 Rreference TFC Id)
  
  -----------------------------------
  Ref Id     Index Valid   UL TFC Id
  -----------------------------------
     0         TRUE           2
     1         TRUE           25
     2         TRUE           41
     3         FALSE          -
  -----------------------------------
  
DEPENDENCIES:
  Needs function get_ul_gain_factor_info_struct_ptr to return the
  valid structure pointers to store the UL power offset info.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_ul_gain_factor_info
(
  l1_ul_gain_factor_parms_struct_type   *gain_factor_parms,
  /* RRC Power offset inforamtion IE struct pointer */
  rrc_PowerOffsetInformation *ie_ptr,
  /* This UL TFC Id */
  uint8 tfc_id
)
{
  /* function local variables */
  /* ------------------------ */

  /* UL power offset information */
  l1_ul_gain_factor_parms_struct_type *ul_power_offset_info;
  
  /* Gain factor structure pointer for signalled mode */
  rrc_SignalledGainFactors *sig_gain_fact;

  /* Reference TFC Id for signalled mode */
  rrc_ReferenceTFC_ID sig_mode_ref_tfc_id;

  /* Get the pointer to the UL gain factor information structures/arrays */
  ul_power_offset_info = &gain_factor_parms[tfc_id];
 
  /* Get the gain factor information based on the mode */
  switch (ie_ptr->gainFactorInformation.t)
  {
    case T_rrc_GainFactorInformation_signalledGainFactors:
      ul_power_offset_info->algo = L1_UL_GF_SIGNALLED;

      sig_gain_fact =
        (ie_ptr->gainFactorInformation.u.signalledGainFactors);

      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(sig_gain_fact,referenceTFC_ID))
      {
        sig_mode_ref_tfc_id =
          ie_ptr->gainFactorInformation.u.signalledGainFactors->referenceTFC_ID;

        if (sig_mode_ref_tfc_id > (L1_UL_REF_TFC_MAX - 1))
        {
          WRRC_MSG1_ERROR("RRC pwr ofst: Sig mode ref tfc id %d",
                    sig_mode_ref_tfc_id);
          return FAILURE;
        }
        else
        {
          ul_power_offset_info->ref_tfc_id = (uint8)sig_mode_ref_tfc_id; 
          ul_power_offset_info->ref_tfc_id_valid = TRUE;
        }
      }

      ul_power_offset_info->BetaD = (uint8) sig_gain_fact->gainFactorBetaD;
      switch (sig_gain_fact->modeSpecificInfo.t)
      {
        case T_rrc_SignalledGainFactors_modeSpecificInfo_fdd:
          ul_power_offset_info->BetaC = (uint8)
            sig_gain_fact->modeSpecificInfo.u.fdd->gainFactorBetaC;
          break;
        case T_rrc_SignalledGainFactors_modeSpecificInfo_tdd:
        default:
          WRRC_MSG1_ERROR("RRC Pwr ofst:Gain fact BetaC in TDD mode/Invalid gain fact mode %d",sig_gain_fact->modeSpecificInfo.t);
          return FAILURE;

      }

      break;
    case T_rrc_GainFactorInformation_computedGainFactors:
      ul_power_offset_info->algo = L1_UL_GF_CALCULATED;
      ul_power_offset_info->ref_tfc_id = (uint8)
        ie_ptr->gainFactorInformation.u.computedGainFactors;
      break;
    default:
      break;
  }

  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,powerOffsetPp_m))
  {
    ul_power_offset_info->valid_Pp_m = TRUE;
    ul_power_offset_info->pwr_offset_Pp_m = (int8) ie_ptr->powerOffsetPp_m;
  }
  else
    ul_power_offset_info->valid_Pp_m = FALSE;

  return SUCCESS;
} /* END get_ul_gain_factor_info() */
  
/*================================================================================================
FUNCTION    :   get_ul_semi_static_transport_format_info()

DESCRIPTION :   Processes the dynamic part of TFS IE of a transport channel
              If the IE contents are valid, saves the information in MAC Ordered Configuration 
              Database.

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE conenets are valid FAILURE otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
uecomdef_status_e_type get_ul_semi_static_transport_format_info
(
  rrc_SemistaticTF_Information        *ie_ptr,
  l1_ul_semi_static_parms_struct_type *oc_ssp_ptr
)
{
  /* get the semi-static Transport Format Information */
  switch(ie_ptr->channelCodingType.t)
  {
    case T_rrc_ChannelCodingType_noCoding:
      oc_ssp_ptr->coding = L1_UL_CODING_NONE;
      break;

    case T_rrc_ChannelCodingType_turbo:
      oc_ssp_ptr->coding = L1_UL_CODING_TURBO; 
      break;

    case T_rrc_ChannelCodingType_convolutional:
      if(ie_ptr->channelCodingType.u.convolutional == rrc_CodingRate_half)
      {
        oc_ssp_ptr->coding = L1_UL_CODING_CONV_ONE_HALF;
      }
      else if(ie_ptr->channelCodingType.u.convolutional == rrc_CodingRate_third)
      {
        oc_ssp_ptr->coding = L1_UL_CODING_CONV_ONE_THIRD;
      }
      else
      {
        WRRC_MSG1_ERROR("Convolutional code rate %d is not valid",
          ie_ptr->channelCodingType.u.convolutional);
        return FAILURE;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid Coding type %d rcv'd in TFS info",
        ie_ptr->channelCodingType.t);
      return FAILURE;
  } /* end switch channel coding type */
    
  /* Get the CRC information */
  switch(ie_ptr->crc_Size)
  {
    case rrc_CRC_Size_crc0:
      oc_ssp_ptr->crc = L1_UL_CRC_0_BITS;      
      break;

    case rrc_CRC_Size_crc8:
      oc_ssp_ptr->crc = L1_UL_CRC_8_BITS;
      break;

    case rrc_CRC_Size_crc12:
      oc_ssp_ptr->crc = L1_UL_CRC_12_BITS;
      break;

    case rrc_CRC_Size_crc16:
      oc_ssp_ptr->crc = L1_UL_CRC_16_BITS;
      break;

    case rrc_CRC_Size_crc24:
      oc_ssp_ptr->crc = L1_UL_CRC_24_BITS;
      break;

    default:
      WRRC_MSG1_ERROR("Invalid CRC %d rcv'd in TFS info",ie_ptr->crc_Size);
      return FAILURE;
  } /* end switch on crc type */

  /* Get the rate matching attribute */
  if((ie_ptr->rateMatchingAttribute <= RRCLLC_MAX_HI_RM) && 
     (ie_ptr->rateMatchingAttribute >= RRCLLC_MIN_HI_RM))
  {
    oc_ssp_ptr->staticRM = (uint16) ie_ptr->rateMatchingAttribute;
  }
  else
  { 
    WRRC_MSG1_ERROR("UL Rate Matching Attribute %d is out of range",
      ie_ptr->rateMatchingAttribute);
    return FAILURE;
  }

  /* Good things if made it this far */
  return SUCCESS;
} /* end get_ul_semi_static_transport_format_info() */

/*============================================================================
FUNCTION: get_logical_channel_list()

DESCRIPTION:
  This function takes a Logical Channel List IE and stores its data
  in the global rlc_size_restriction_info[] array.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  The rlc_size_restriction_info[] array is updated.
=============================================================================*/
static uecomdef_status_e_type get_logical_channel_list
(
  rrc_LogicalChannelList        *ie_ptr,
  rrcllcoc_logical_channel_list_type *rec
)
{
  uecomdef_status_e_type rv = SUCCESS;
  rrc_LogicalChannelList_explicitList *lc_list = NULL;

  switch(ie_ptr->t)
  {
    case T_rrc_LogicalChannelList_allSizes:
        rec->restriction_type = RLC_SIZES_ALL;
      break;

    case T_rrc_LogicalChannelList_configured:
        rec->restriction_type = RLC_SIZES_CONFIGURED;
      break;

    case T_rrc_LogicalChannelList_explicitList:
        rec->restriction_type = RLC_SIZES_EXPLICIT_LIST;
        rec->count = 0;
        lc_list = ie_ptr->u.explicitList;
        if(lc_list == NULL)
        {
          WRRC_MSG0_ERROR("Logical channel list is empty.");
          rv = FAILURE;
        }
        while((lc_list != NULL) && (lc_list->n > rec->count))
        {
          rec->rb_id[rec->count] = lc_list->elem[rec->count].rb_Identity;
          rec->count++;
        }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid CHOICE for logical channel list IE: %d", ie_ptr->t);
      rv = FAILURE;
      break;
  }

  return rv;
} /* get_logical_channel_list */


/*================================================================================================
FUNCTION:       get_ul_ded_trch_dynamic_tfs_info()

DESCRIPTION:    Processes the dynamic part of TFS IE of dedicated transport channel
                If the IE contents are valid, saves the information in MAC Ordered Configuration 
                Database.

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
static uecomdef_status_e_type get_ul_ded_trch_dynamic_tfs_info
(
  ordered_config_type                     *config_ptr,
  /* Pointer to the ASN.1 Dedicated Transport Channel TFS information */
  rrc_DedicatedTransChTFS                 *ie_ptr,

  /* Pointer to Transport Channel TFS information pointer in OC */
  mac_ul_tfs_type                         *oc_tfs_ptr,

  /* Pointer to the Semi static parms information pointer in OC */
  l1_ul_semi_static_parms_struct_type *oc_ssp_ptr,

  /* TrCH identity, used for storing Logical Channel List information */
  tr_ch_id_type                         trch_id
)
{
  /* Return value type needed by GET_UL_TRCH_DYNAMIC_TFS_IE_PTR */
  uecomdef_status_e_type           rv = FAILURE;

  /* Pointer to TFS information, initialized in MACRO call */
  rrc_DedicatedDynamicTF_InfoList  * tfs_ptr = NULL;

  /* Pointer to TF information in ordered config data */
  mac_ul_tf_type                      *oc_tf_ptr = NULL;

  /* Transport Format index */
  /* This ensures the previous Transport Format set is trashed
   * and the new one is stored.
   */
  uint16 tf_idx = 0;

  /* Pointer to Number of Transport Blocks Linked List */
  rrc_DedicatedDynamicTF_Info_numberOfTbSizeList * num_tb_ptr;
  uint32 idx =0,idy=0;

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

  rrcllcoc_logical_channel_list_type *rec;

#endif/*#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */
  /* Get the TFS pointer based on TTI type */
  GET_UL_TRCH_DYNAMIC_TFS_IE_PTR(Dedicated)

  /*
  * If the TTI type was invalid from the GET_UL_TRCH_DYNAMIC_TFS_IE_PTR 
  * macro, then bail, since probably have a NULL pointer
  */
  if(rv == FAILURE)
  {
    return FAILURE;
  }

  /* Get Dynamic Transport Format information */
  while((tfs_ptr->n > idy) && (tf_idx < UE_MAX_TF))
  {
    /* Get the Transport Block size pointer */
    num_tb_ptr = &tfs_ptr->elem[idy].numberOfTbSizeList;
 idx =0;
    /* Get the RLC size and Number of Transport blocks */
    while((num_tb_ptr->n  > idx) && (tf_idx < UE_MAX_TF))
    {
      /* TF info pointer in ordered config data */
      oc_tf_ptr = &oc_tfs_ptr->tf_info[tf_idx];

      /* Save Number of TBlks info in OC data structure */
      oc_tf_ptr->ntblks = get_tfs_num_tb(&num_tb_ptr->elem[idx]);

      /* Save RLC Size */
      oc_tf_ptr->tb_size = (uint16) get_tfs_ded_rlc_size(&tfs_ptr->elem[idy]);
#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH


      /* Save Logical Channel List */
      config_ptr->dch_rlc_size_restriction_info[trch_id].mac_update_needed = TRUE;
      rec = &(config_ptr->dch_rlc_size_restriction_info[trch_id].logical_channel_list[tf_idx]);
      if (get_logical_channel_list(&tfs_ptr->elem[idy].logicalChannelList,
                                   rec) == FAILURE)
      {
        return FAILURE;
      }
#endif

      /* Get the next item in Number of TBs list & increment TF index */
      idx++;
      tf_idx++;

    } /* end while still have TFs to process */

    /* Pointer to the next item in Dynamic TF info */
    idy++;
  } /* end of TF loop */

  /* Save the semi-static parameters */
  if(get_ul_semi_static_transport_format_info(&ie_ptr->semistaticTF_Information, oc_ssp_ptr)
    == FAILURE)
  {
    return FAILURE;
  }
  oc_ssp_ptr->trch_id = (uint8) trch_id;

    /* Save Number of Transport Formats info, and indicate all is well */
  oc_tfs_ptr->ntf = tf_idx;
    return SUCCESS;

} /* end get_ul_ded_trch_dynamic_tfs_info() */ /*lint !e715 */

/*================================================================================================
FUNCTION    :   get_ul_common_trch_dynamic_tfs_info()

DESCRIPTION :   Processes the dynamic part of TFS IE of common transport channel
                If the IE contents are valid, saves the information in MAC Ordered Configuration 
                Database.

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
static uecomdef_status_e_type get_ul_common_trch_dynamic_tfs_info
(
  ordered_config_type                 *config_ptr,
  /* Pointer to the ASN.1 Dedicated Transport Channel TFS information */
  rrc_CommonTransChTFS                *ie_ptr,

  /* Pointer to Transport Channel TFS information pointer in OC */
  mac_ul_tfs_type                     *oc_tfs_ptr,

  /* Pointer to the Semi static parms information pointer in OC */
  l1_ul_semi_static_parms_struct_type *oc_ssp_ptr
)
{
  /* Return value type needed by GET_UL_TRCH_DYNAMIC_TFS_IE_PTR */
  uecomdef_status_e_type        rv = FAILURE;

  /* Pointer to TFS information */
  rrc_CommonDynamicTF_InfoList * tfs_ptr = NULL;

  /* Pointer to TF information in ordered config data */
  mac_ul_tf_type               *oc_tf_ptr = NULL;

  /* Transport Format index */
  uint16 tf_idx = 0;

  /* Pointer to Number of Transport Blocks Linked List */
  rrc_CommonDynamicTF_Info_numberOfTbSizeList * num_tb_ptr;

  rrcllcoc_logical_channel_list_type *rec;
  uint32 idx =0, idy=0;


  /* Get the TFS pointer based on TTI type */
  GET_UL_TRCH_DYNAMIC_TFS_IE_PTR(Common)

  /*
  * If the TTI type was invalid from the GET_UL_TRCH_DYNAMIC_TFS_IE_PTR 
  * macro, then bail, since probably have a NULL pointer
  */
  if(rv == FAILURE)
  {
    return FAILURE;
  }

  /* Get Dynamic Transport Format information */
  while((tfs_ptr->n > idy) && (tf_idx < UE_MAX_TF))
  {
    /* Get the Transport Block size pointer */
    num_tb_ptr = &tfs_ptr->elem[idy].numberOfTbSizeList;
    idx=0;
    /* Get the RLC size and Number of Transport blocks */
    while((num_tb_ptr->n > idx) && (tf_idx < UE_MAX_TF))
    {
      /* TF info pointer in ordered config data */
      oc_tf_ptr = &oc_tfs_ptr->tf_info[tf_idx];

      /* Save Number of TBlks info in OC data structure */
      oc_tf_ptr->ntblks = get_tfs_num_tb(&num_tb_ptr->elem[idx]);

      /* Save RLC Size */
      oc_tf_ptr->tb_size = (uint16) get_tfs_com_rlc_size(&tfs_ptr->elem[idy]);

      if(config_ptr != NULL)
      {
        /* Save Logical Channel List */
        config_ptr->rach_rlc_size_restriction_info.mac_update_needed = TRUE;
        rec = &(config_ptr->rach_rlc_size_restriction_info.logical_channel_list[tf_idx]);
        if (get_logical_channel_list(&tfs_ptr->elem[idy].logicalChannelList,
                                   rec) == FAILURE)
        {
          return FAILURE;
        }
      }


      /* Get the next item in Number of TBs list & increment TF index */
      idx++;;
      tf_idx++;

    } /* end while still have TFs to process */

    /* Pointer to the next item in Dynamic TF info */
    idy++;
  } /* end of TF loop */

  /* Save the semi-static parameters */
  if(get_ul_semi_static_transport_format_info(&ie_ptr->semistaticTF_Information, oc_ssp_ptr) ==
    FAILURE)
  {
    rv = FAILURE;
  }

  /* Save Number of Transport Formats info, and indicate all is well */
  oc_tfs_ptr->ntf = tf_idx;
  return SUCCESS;

} /* end get_common_trch_dynamic_tfs_info() */

/*================================================================================================
FUNCTION    :   get_ul_transport_format_set()

DESCRIPTION :   Processes the TFS IE of a transport channel. If the IE contents are valid
              this function saves the information in MAC Ordered Configuration Database.

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE contents are valid, FAILURE otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
uecomdef_status_e_type  get_ul_transport_format_set
(
    ordered_config_type       *config_ptr,
    /* Transport Format Set IE Pointer */
    rrc_TransportFormatSet    *ie_ptr,

    /* Transport channel index (0 to 7) */
    uint8                     trch_idx
)
{
  uecomdef_status_e_type      rv = FAILURE;
  tr_ch_id_type trch_id;
  mac_ul_tfs_type     *oc_tfs_ptr;
  l1_ul_semi_static_parms_struct_type *oc_ssp_ptr;

  if(trch_idx >= UE_MAX_TRCH || trch_idx >= L1_UL_TRCH_MAX)
  {
    return (rv);
  }

  /* TrCH identity, used for storing Logical Channel List information for DCH */
  trch_id = config_ptr->ul_dch_trch_info[trch_idx].trch_id;

  /* Pointer to the TFS info in Ordered Config */
  oc_tfs_ptr = &config_ptr->ul_tfcs.tfs_info[trch_idx];

  /* Pointer to the Semi Static parms in Ordered Config */
  oc_ssp_ptr = &config_ptr->rrc_ul_cctrch_params.semi_static_parms[trch_idx];
  /* Get the pointer for this Transport Channel's Semi Static Parameters */
  config_ptr->l1_ul_cctrch_parms.semi_static_parms_ptr[trch_idx] =
    &config_ptr->rrc_ul_cctrch_params.semi_static_parms[trch_idx];


  /* Get TFS info based on channel type */
  switch (ie_ptr->t)
  {
    /* Dedicated Transport Channel */
    case T_rrc_TransportFormatSet_dedicatedTransChTFS:
        rv = get_ul_ded_trch_dynamic_tfs_info (config_ptr,
                                               ie_ptr->u.dedicatedTransChTFS,
                                               oc_tfs_ptr, oc_ssp_ptr, trch_id);
        break;

    /* Common Transport Channel */
    case T_rrc_TransportFormatSet_commonTransChTFS:
        rv = get_ul_common_trch_dynamic_tfs_info (config_ptr,
                                                  ie_ptr->u.commonTransChTFS,
                                                  oc_tfs_ptr, oc_ssp_ptr);
        break;

    /* Invalid Transport Channel */
    default:
        WRRC_MSG0_ERROR ("Rcv'd invalid TFS");
        rv = FAILURE;
        break;
  }

  return (rv);
} /* end get_ul_transport_format_set() */

/*================================================================================================
FUNCTION    :   get_ul_transport_format_set_for_prach_db

DESCRIPTION :   Processes the TFS IE of a transport channel. If the IE contents are valid
              this function saves the information in MAC Ordered Configuration Database.

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE contents are valid, FAILURE otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
uecomdef_status_e_type  get_ul_transport_format_set_for_prach_db
(
    rrcllc_prach_sel_info_type * prach_sel_info_ptr, /* Transport Format Set IE Pointer */
    rrc_TransportFormatSet    *ie_ptr, /* Transport channel index (0 to 7) */
    uint8  trch_idx
)
{
  uecomdef_status_e_type      rv = FAILURE;
  mac_ul_tfs_type     *oc_tfs_ptr;
  l1_ul_semi_static_parms_struct_type *oc_ssp_ptr;

  if(trch_idx >= UE_MAX_TRCH || trch_idx >= L1_UL_TRCH_MAX)
  {
    return (rv);
  }  
  /* Pointer to the TFS info in Ordered Config */
  oc_tfs_ptr = &prach_sel_info_ptr->ul_tfcs.tfs_info[trch_idx];

  /* Pointer to the Semi Static parms in Ordered Config */
  oc_ssp_ptr = &prach_sel_info_ptr->semi_static_parms[trch_idx];
  
  /* Get the pointer for this Transport Channel's Semi Static Parameters */
  prach_sel_info_ptr->l1_ul_cctrch_parms.semi_static_parms_ptr[trch_idx] =
    &prach_sel_info_ptr->semi_static_parms[trch_idx];

  /* Get TFS info based on channel type */
  switch (ie_ptr->t)
  {
    /* Common Transport Channel */
    case T_rrc_TransportFormatSet_commonTransChTFS:
        rv = get_ul_common_trch_dynamic_tfs_info (NULL,
                                                  ie_ptr->u.commonTransChTFS,
                                                  oc_tfs_ptr, oc_ssp_ptr);
        break;

    /* Invalid Transport Channel */
    default:
        WRRC_MSG0_ERROR ("Rcv'd invalid TFS");
        rv = FAILURE;
        break;
  }
  
  return (rv);  
  
}

/*================================================================================================
FUNCTION    :   get_ul_ctfc_info_for_prach_db ()

DESCRIPTION :   

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
uecomdef_status_e_type  get_ul_ctfc_info_for_prach_db
(
  rrcllc_prach_sel_info_type   * prach_sel_info_ptr,
  rrc_TFCS_ReconfAdd  *ie_ptr
)
{
    uecomdef_status_e_type  rv  =   SUCCESS;

    /* Pointer to the first item in the list */ \
    rrc_TFCS_ReconfAdd_ctfc2Bit     *ie_ctfc2_ptr;
    rrc_TFCS_ReconfAdd_ctfc4Bit     *ie_ctfc4_ptr;
    rrc_TFCS_ReconfAdd_ctfc6Bit     *ie_ctfc6_ptr;
    rrc_TFCS_ReconfAdd_ctfc8Bit     *ie_ctfc8_ptr;
    rrc_TFCS_ReconfAdd_ctfc12Bit    *ie_ctfc12_ptr;
    rrc_TFCS_ReconfAdd_ctfc16Bit   *ie_ctfc16_ptr;
    rrc_TFCS_ReconfAdd_ctfc24Bit    *ie_ctfc24_ptr;

    /* TFC index, needed by GET_UL_CTFC_BIT_INFO macro */
    uint32  tfc_idx = 0;
    uint32 idx=0;
    /* Pointer to TFC info in Ordered Config Data Structure */
    mac_ul_tfc_type     *oc_tfc_ptr;

    /* Get the info based on CTFC Size type */
    switch (ie_ptr->ctfcSize.t)
  {
        /* 2 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc2Bit:
            ie_ctfc2_ptr = ie_ptr->ctfcSize.u.ctfc2Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc2_ptr, ctfc2);
            break;

        /* 4 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc4Bit:
            ie_ctfc4_ptr = ie_ptr->ctfcSize.u.ctfc4Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc4_ptr, ctfc4);
            break;
            
        /* 6 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc6Bit:
            ie_ctfc6_ptr = ie_ptr->ctfcSize.u.ctfc6Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc6_ptr, ctfc6);
            break;
            
        /* 8 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc8Bit:
            ie_ctfc8_ptr = ie_ptr->ctfcSize.u.ctfc8Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc8_ptr, ctfc8);
            break;
            
        /* 12 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc12Bit:
            ie_ctfc12_ptr = ie_ptr->ctfcSize.u.ctfc12Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc12_ptr, ctfc12);
            break;
            
        /* 16 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc16Bit:
            ie_ctfc16_ptr = ie_ptr->ctfcSize.u.ctfc16Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc16_ptr, ctfc16);
            break;

        /* 24 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc24Bit:
            ie_ctfc24_ptr = ie_ptr->ctfcSize.u.ctfc24Bit;
            GET_UL_CTFC_BIT_INFO_FOR_PRACH_DB(ie_ctfc24_ptr, ctfc24);
            break;
            
        /* Invalid CTFC size */
        default:
            WRRC_MSG0_ERROR("Rcv'd invalid CTFC bit size");
            rv = FAILURE;
            break;
    }

    return (rv);

}

/*================================================================================================
FUNCTION: get_ul_tfcs_info_for_prach_db

DESCRIPTION:
  This function takes a pointer to the FDD only portion of the
  Transport Format Set IE and copies it to ORDERED_CONFIG.

  NOTE: This is on a per Transport Channel basis, and there can only
        be 1 TTI type per Transport Channel.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS: None.
===============================================================================================*/
uecomdef_status_e_type get_ul_tfcs_info_for_prach_db
(
    rrcllc_prach_sel_info_type  *prach_sel_info_ptr,
    /* Transport Format Combination Set IE Pointer */
    rrc_TFCS        *ie_ptr
)
{
  uecomdef_status_e_type  rv = SUCCESS;
  uint8 ix, iy;
    /* Get TFCS based on TFCI Signaling type */
  switch (ie_ptr->t)
  {
      /* Normal TFCI Signaling - Explicit TFCS Info */
      case T_rrc_TFCS_normalTFCI_Signalling:
           /* Get info based TFCS Representation type */
        switch (ie_ptr->u.normalTFCI_Signalling->t)
        {
          case T_rrc_ExplicitTFCS_Configuration_complete:
            
            /* Initialize the Uplink TFI table first */
            prach_sel_info_ptr->ul_tfcs.ntfc = 0;
            /* Clear out the uplink ctfc & tfis */
            for(ix = 0; ix < UE_MAX_TFC; ix++)
            {
              prach_sel_info_ptr->ul_tfcs.tfc_info[ix].control_flag = TFC_NOT_ALLOWED;
              prach_sel_info_ptr->ul_tfcs.tfc_info[ix].ctfc = 0;
              for(iy = 0; iy < UE_MAX_TRCH; iy++)
              {
                prach_sel_info_ptr->ul_tfcs.tfc_info[ix].tfi[iy] = 0;
              }
            }

            /* Get and save the CTFC chosen */
            if (get_ul_ctfc_info_for_prach_db(prach_sel_info_ptr, 
    ie_ptr->u.normalTFCI_Signalling->u.complete) == FAILURE)
            {
              rv = FAILURE;
            }
            break;

            default:
              WRRC_MSG0_ERROR("Rcvd invalid TFCS representation");
              rv = FAILURE;
        } /* end explicit config t() */
        break;
  
      /* Split TFCI Signaling */
      case T_rrc_TFCS_dummy:
      default:
        WRRC_MSG1_ERROR("Rcv'd Split TFCI info/invalid TFCI signaling %d ",ie_ptr->t);
          rv = FAILURE;
          break;
    }
  
    return (rv);
}

/*================================================================================================
FUNCTION    :   get_ul_ctfc_info ()

DESCRIPTION :   Processes CTFC info based on CTFC Bit Size. This is for a complete
              reconfiguration or an an addition or the addition portion of the
              replacement.

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
static uecomdef_status_e_type  get_ul_ctfc_info
(
  ordered_config_type  *config_ptr,
  rrc_TFCS_ReconfAdd  *ie_ptr
)
{
    uecomdef_status_e_type  rv  =   SUCCESS;

    /* Pointer to the first item in the list */ \
    rrc_TFCS_ReconfAdd_ctfc2Bit     *ie_ctfc2_ptr;
    rrc_TFCS_ReconfAdd_ctfc4Bit     *ie_ctfc4_ptr;
    rrc_TFCS_ReconfAdd_ctfc6Bit     *ie_ctfc6_ptr;
    rrc_TFCS_ReconfAdd_ctfc8Bit     *ie_ctfc8_ptr;
    rrc_TFCS_ReconfAdd_ctfc12Bit    *ie_ctfc12_ptr;
    rrc_TFCS_ReconfAdd_ctfc16Bit    *ie_ctfc16_ptr;
    rrc_TFCS_ReconfAdd_ctfc24Bit    *ie_ctfc24_ptr;

    /* TFC index, needed by GET_UL_CTFC_BIT_INFO macro */
    uint32  tfc_idx = 0;
     uint32  i = 0;
  uint32 idx=0;
    
    /* Pointer to TFC info in Ordered Config Data Structure */
    mac_ul_tfc_type     *oc_tfc_ptr;

    /* Get the info based on CTFC Size type */
    switch (ie_ptr->ctfcSize.t)
  {
        /* 2 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc2Bit:
            ie_ctfc2_ptr = ie_ptr->ctfcSize.u.ctfc2Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc2_ptr, ctfc2);
            break;

        /* 4 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc4Bit:
            ie_ctfc4_ptr = ie_ptr->ctfcSize.u.ctfc4Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc4_ptr, ctfc4);
            break;
            
        /* 6 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc6Bit:
            ie_ctfc6_ptr = ie_ptr->ctfcSize.u.ctfc6Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc6_ptr, ctfc6);
            break;
            
        /* 8 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc8Bit:
            ie_ctfc8_ptr = ie_ptr->ctfcSize.u.ctfc8Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc8_ptr, ctfc8);
            break;
            
        /* 12 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc12Bit:
            ie_ctfc12_ptr = ie_ptr->ctfcSize.u.ctfc12Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc12_ptr, ctfc12);
            break;
            
        /* 16 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc16Bit:
            ie_ctfc16_ptr = ie_ptr->ctfcSize.u.ctfc16Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc16_ptr, ctfc16);
            break;

        /* 24 Bit CTFC */
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc24Bit:
            ie_ctfc24_ptr = ie_ptr->ctfcSize.u.ctfc24Bit;
            GET_UL_CTFC_BIT_INFO(ie_ctfc24_ptr, ctfc24);
            break;
            
        /* Invalid CTFC size */
        default:
            WRRC_MSG0_ERROR("Rcv'd invalid CTFC bit size");
            rv = FAILURE;
            break;
    }

    return (rv);
} /* get_ul_ctfc_info() */

/*================================================================================================
FUNCTION  : get_ul_add_tfcs_info() 

DESCRIPTION : Adds TFC info to the MAC Ordered Config Database

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS: None.
===============================================================================================*/
static uecomdef_status_e_type get_ul_add_tfcs_info
(
  ordered_config_type *config_ptr,
  rrc_TFCS_ReconfAdd *ie_ptr
)
{
  /* CTFC size choice is same as the one used in complete configuration */
  return get_ul_ctfc_info(config_ptr, ie_ptr);

} /* end get_ul_tfcs_info() */

/*================================================================================================
FUNCTION  : get_ul_remove_tfcs_info

DESCRIPTION : This function removes a TFCI from MAC Ordered Config Database

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS: None.
===============================================================================================*/
static uecomdef_status_e_type get_ul_remove_tfcs_info
(
  ordered_config_type *config_ptr,
  rrc_TFCS_RemovalList * ie_ptr
)
{
  /* TFCI to be removed */
  uint32  removed_tfci = 0;
  uint32  index_for_sorting;
  uint32  to_be_compared_index =0;
  uint32 idx =0;
  /* Go over each element of the Removal List */
  while(ie_ptr->n > idx)
  {
    /* Get TFCI value */
    removed_tfci = ie_ptr->elem[idx].tfci;

    /* TFCI value is valid */
    if((removed_tfci < config_ptr->ul_tfcs.ntfc) && (removed_tfci < UE_MAX_TFC))
    {
      /* Mark the TFC as removed */
      config_ptr->ul_tfcs.tfc_info[removed_tfci].control_flag = TFC_REMOVED;

      /* Save the removed TFCI in a table and increment the removed TFCI count */
      config_ptr->removed_tfci_list[config_ptr->num_removed_tfcis] = removed_tfci;
      config_ptr->num_removed_tfcis ++;
    }
    /* TFCI value is invalid */
    else
    {
      WRRC_MSG0_ERROR("Invalid TFCI removal");
      return FAILURE;
    }

    /* Get pointer to the next item in the list */  \
    idx++;
  } /* end removed list loop */

  /* Now sort the Removed TFCI List here */
  for(index_for_sorting = 1; 
      ((index_for_sorting < config_ptr->num_removed_tfcis)&&
        (index_for_sorting < UE_MAX_TFC)); 
      index_for_sorting ++)
  {
    /* If the TFCI for the index for sort is NOT lower than the TFCI
     * for the index one lower than the one being sorted, do NOT do anything.
     * This is because all indices further below have already been sorted.
     */
    if(config_ptr->removed_tfci_list[index_for_sorting] < 
        config_ptr->removed_tfci_list[index_for_sorting-1])
    {
      /* The TFCI for sorted index is lower than TFCI for index before
       * This is time to move all indices below till a space is found for 
       * data at current sorted index.
       */
      removed_tfci = config_ptr->removed_tfci_list[index_for_sorting];

      to_be_compared_index = index_for_sorting;

      while( (to_be_compared_index > 0) && 
             (removed_tfci < 
              config_ptr->removed_tfci_list[to_be_compared_index-1])
           )
      {
         config_ptr->removed_tfci_list[to_be_compared_index] =
           config_ptr->removed_tfci_list[to_be_compared_index-1];
        
         to_be_compared_index --;
      }
      config_ptr->removed_tfci_list[to_be_compared_index] = removed_tfci;
    }
  } /* for(...) */

  /* everything good if made it here */
  return SUCCESS;
} /* end get_ul_remove_tfcs_info() */

/*================================================================================================
FUNCTION :  get_ul_normal_tfci_info() 

DESCRIPTION :   This function Processes NORMAL TFCI signaling mode

DEPENDENCIES:   None.

RETURN VALUE:   SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:   None.
===============================================================================================*/
static uecomdef_status_e_type get_ul_normal_tfci_info
(
  ordered_config_type  *config_ptr,
  rrc_ExplicitTFCS_Configuration * ie_ptr
)
{
  /* Get info based TFCS Representation type */
  switch (ie_ptr->t)
  {
    case T_rrc_ExplicitTFCS_Configuration_complete:
      /* Initialize the Uplink TFI table first */
      rrcllc_init_ul_tfi_table(config_ptr);

      /* Get and save the CTFC chosen */
      if (get_ul_ctfc_info(config_ptr, ie_ptr->u.complete) == FAILURE)
      {
        return FAILURE;
      }
      break;

    case T_rrc_ExplicitTFCS_Configuration_addition:
      if(get_ul_add_tfcs_info(config_ptr, ie_ptr->u.addition) == FAILURE)
      { 
        return FAILURE;
      } 
      break;

    case T_rrc_ExplicitTFCS_Configuration_removal:
      if(get_ul_remove_tfcs_info(config_ptr, ie_ptr->u.removal) == FAILURE)
      {
        return FAILURE;
      }
      break;

    case T_rrc_ExplicitTFCS_Configuration_replacement:
      /* First remove the selected TFC.... */
      if(get_ul_remove_tfcs_info(config_ptr, &ie_ptr->u.replacement->tfcsRemoval)
          == FAILURE)
      {
        return FAILURE;
      }

      /* Then add the selected TFC.... */
      if(get_ul_add_tfcs_info(config_ptr, &ie_ptr->u.replacement->tfcsAdd) == FAILURE)
      {
        return FAILURE;
      }
      break;

    default:
      WRRC_MSG0_ERROR("Rcvd invalid TFCS representation");
      return FAILURE;

  } /* end explicit config choice() */

  /* everything good if got here */
  /* Indicate that both Uplink MAC and L1 need a reconfig */
  config_ptr->reconfig_needed.uplink_mac = TRUE;
  config_ptr->reconfig_needed.uplink_l1 = TRUE;

  return SUCCESS;

} /* end get_ul_normal_tfci_info() */

/*================================================================================================
FUNCTION: get_ul_tfcs_info()

DESCRIPTION:
  This function takes a pointer to the FDD only portion of the
  Transport Format Set IE and copies it to ORDERED_CONFIG.

  NOTE: This is on a per Transport Channel basis, and there can only
        be 1 TTI type per Transport Channel.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS: None.
===============================================================================================*/
uecomdef_status_e_type get_ul_tfcs_info
(
    ordered_config_type  *config_ptr,
    /* Transport Format Combination Set IE Pointer */
    rrc_TFCS        *ie_ptr
)
{
  uecomdef_status_e_type  rv = SUCCESS;
  /* Set flag indicating that ul_tfcs has been rcvd*/
  ul_tfcs_rcvd = TRUE;
  /* Get TFCS based on TFCI Signaling type */
  switch (ie_ptr->t)
  {
      /* Normal TFCI Signaling - Explicit TFCS Info */
      case T_rrc_TFCS_normalTFCI_Signalling:
          rv = get_ul_normal_tfci_info(config_ptr, ie_ptr->u.normalTFCI_Signalling);
          break;
  
      /* Split TFCI Signaling */
      /* Invalid TFCI Signaling */
    case T_rrc_TFCS_dummy:
      default:
      WRRC_MSG1_ERROR("Rcv'd Split TFCI info/invalid TFCI signaling",ie_ptr->t);
          rv = FAILURE;
          break;
    }

    return (rv);
} /* end get_ul_tfcs_info() */

/*================================================================================================
FUNCTION: get_ul_info_common_for_all_trch()

DESCRIPTION:
  This function takes a pointer to the Uplink Common Information for all
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS: None.
===============================================================================================*/
uecomdef_status_e_type get_ul_info_common_for_all_trch
(
  rrc_UL_CommonTransChInfo *ie_ptr,
  rrc_state_e_type  next_rrc_state
)
{
  MSG_MED("Get UL Common info for all TrCH",0,0,0);

  /* Check if a new TFC Subset is required. */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_CommonTransChInfo,tfc_Subset))
  {
    WRRC_MSG0_HIGH("TFC subset IE is present: Using subset");
  }
  else
  {
    MSG_MED("Use existing UL TFC Subset",0,0,0);
  }

  /* Check if PRACH TFCS is presanet */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_CommonTransChInfo,prach_TFCS))
  {
    WRRC_MSG0_ERROR("Rcv'd PRACH TFCS info");
    return FAILURE;
  }

  if(next_rrc_state == RRC_STATE_CELL_DCH)
  {
    /* Check for the FDD specific info if it is included */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_CommonTransChInfo,modeSpecificInfo))
    {
      switch(ie_ptr->modeSpecificInfo.t)
      {
        case T_rrc_UL_CommonTransChInfo_modeSpecificInfo_fdd:
          MSG_MED("Get New UL DCH TFCS",0,0,0);
          if(get_ul_tfcs_info(ordered_config_ptr, &ie_ptr->modeSpecificInfo.u.fdd->ul_TFCS) == FAILURE)
          {
            return FAILURE;
          }
          WRRC_MSG0_HIGH("TFCC: TFCS list is Changed ");
          ordered_config_ptr->mac_ul_parms.new_tfcs_list_present = TRUE;
          break;
        case T_rrc_UL_CommonTransChInfo_modeSpecificInfo_tdd:
          WRRC_MSG0_ERROR("TDD Mode not supported for UL Common TRCH Info");
          break;
        default:
          WRRC_MSG1_ERROR("Invalid Mode %d rcv'd in UL Common TRCH Info IE",
                    ie_ptr->modeSpecificInfo.t);
          return FAILURE;
      } /* End mode switch */
    } /* end Mode specific info if present */
  }  /* Next RRC state is Cell_DCH */

  return SUCCESS;
} /* end get_ul_info_common_for_all_trch() */

/*================================================================================================
FUNCTION: get_ul_info_common_for_all_trch_r5()

DESCRIPTION:
  This function takes a pointer to the Uplink Common Information for all
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS: None.
===============================================================================================*/
uecomdef_status_e_type get_ul_info_common_for_all_trch_r5
(
  rrc_UL_CommonTransChInfo_r4 *ie_ptr,
  rrc_state_e_type  next_rrc_state
)
{

  /* Check if a new TFC Subset is required. */

  WRRC_MSG1_MED("TFC New subset IE presence:%d Use subset if present",
    RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_CommonTransChInfo_r4,tfc_Subset));

  /* Check if PRACH TFCS is presanet */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_CommonTransChInfo_r4,prach_TFCS))
  {
    WRRC_MSG0_ERROR("Rcv'd PRACH TFCS info");
    return FAILURE;
  }

  if(next_rrc_state == RRC_STATE_CELL_DCH)
  {
    /* Check for the FDD specific info if it is included */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_CommonTransChInfo_r4,modeSpecificInfo))
    {
      switch(ie_ptr->modeSpecificInfo.t)
      {
        case T_rrc_UL_CommonTransChInfo_r4_modeSpecificInfo_fdd:
          if(get_ul_tfcs_info(ordered_config_ptr, &ie_ptr->modeSpecificInfo.u.fdd->ul_TFCS) == FAILURE)
          {
            return FAILURE;
          }
          WRRC_MSG0_MED("TFCC: Get New UL DCH TFCS, TFCS list is Changed ");
          ordered_config_ptr->mac_ul_parms.new_tfcs_list_present = TRUE;
          break;
        case T_rrc_UL_CommonTransChInfo_r4_modeSpecificInfo_tdd:
          WRRC_MSG0_ERROR("TDD Mode not supported for UL Common TRCH Info");
          break;
        default:
          WRRC_MSG1_ERROR("Invalid Mode %d rcv'd in UL Common TRCH Info IE",
                    ie_ptr->modeSpecificInfo.t);
          return FAILURE;
      } /* End mode switch */
    } /* end Mode specific info if present */
  }  /* Next RRC state is Cell_DCH */

  return SUCCESS;
} /* end get_ul_info_common_for_all_trch_r5() */

/*================================================================================================
FUNCTION: process_ul_deleted_trch_info_dch_usch()

DESCRIPTION:
  This function process deleted trCh information provided for dch_usch.

  In the TFS it resets all information for this TrCH.

REUSABILITY: R6/R5/R99.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH deletions are the only item supported now.
===============================================================================================*/
static uecomdef_status_e_type process_ul_deleted_trch_info_dch_usch
(
  rrc_UL_TrCH_Type ul_TransportChannelType,
  rrc_TransportChannelIdentity ul_TransportChannelIdentity
)
{
  /* TrCH Index */
  uint8 ix;

  if(rrc_UL_TrCH_Type_dch == ul_TransportChannelType)
  {
    ix = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr,
                                     (tr_ch_id_type)ul_TransportChannelIdentity);
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("No index allocated for TrCH ID %d", ul_TransportChannelIdentity);
      return(SUCCESS);
    }
    rrcllc_backup_rbmapping_info(ix,UL_DCH_RB_MAPPING );
    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_ul_dch_rb_mapping_info(ordered_config_ptr, ix);


    /* Update UL TrCH counts now */
    /* MAC count */
    ordered_config_ptr->ul_cctrch.dch_info.ndchs--;
    /* Indicate that the old index is now available */
    if(rrcllc_deallocate_ul_dch_trch_idx(ix) == FAILURE)
    {
      return FAILURE;
    }

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

    /* Remove the RLC size restriction info */
    rrcllc_init_dch_rlc_size_info(ix);
#endif/*#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */
  }
  else
  {
    ix = rrcllc_find_ul_rach_trch_idx();

    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("No index allocated for TrCH ID %d", ul_TransportChannelIdentity);
      return(FAILURE);
    }
    rrcllc_backup_rbmapping_info(ix,RACH_RB_MAPPING);

    rrcllc_init_rach_rb_mapping_info(ordered_config_ptr);
  }

  /* RRCEUL-query why is this cleared ir-respective of DCH or RACH trch 
     delete information */
  /* Clear the TFS for this TrCH */
  rrcllc_init_ul_tfs_table(ordered_config_ptr, ix);

  ordered_config_ptr->ul_tfcs.nchan--;

  /* L1 count */
  ordered_config_ptr->rrc_ul_cctrch_params.num_trch--; 

  MSG_MED("Delete UL TrCH %d", ul_TransportChannelIdentity, 0,0);

  return(SUCCESS);
}

/*================================================================================================
FUNCTION: get_ul_deleted_trch_info_list_r6()

DESCRIPTION:
  This function takes a pointer to the R6 Uplink Deleted 
  Transport Channel IE and Deletes if from Ordered Config.

REUSABILITY: R6

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH deletions are the only item supported now.
===============================================================================================*/
static uecomdef_status_e_type get_ul_deleted_trch_info_list_r6
(
  rrc_UL_TransportChannelIdentity_r6 * ie_ptr
)
{
  if (RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_TransportChannelIdentity_r6_dch_usch))
  {
    /* set ul_tfcs_needed flag to true.  Later if common info is not present,
     * UE needs to reject the message 
     */
    ul_tfcs_needed = TRUE;
    return (process_ul_deleted_trch_info_dch_usch(
      ie_ptr->u.dch_usch->ul_TransportChannelType,
      ie_ptr->u.dch_usch->ul_TransportChannelIdentity));
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_TransportChannelIdentity_r6_e_dch))
  {
    uint8 e_mac_d_flow_idx = 0;
    e_mac_d_flow_idx =
      rrcllc_find_e_mac_d_flow_idx((uint8)ie_ptr->u.e_dch);

    if (e_mac_d_flow_idx >= UE_MAX_MAC_D_FLOW) 
    {
      WRRC_MSG1_ERROR("RRCEUL: NW deleting mac-d flow id %d that never existed",ie_ptr->u.e_dch);
      return(SUCCESS);
    }
    else
    {
      rrcllc_backup_rbmapping_info(e_mac_d_flow_idx,E_DCH_RB_MAPPING );
      rrcllc_init_e_dch_rbmapping_info(ordered_config_ptr, e_mac_d_flow_idx);
      rrcllc_update_mac_eul_action(TRUE);
    }
  }
    /* have this to be on safer side - in case later someone removes return 
     statement from one of the paths */
  return(SUCCESS);

} /* end get_ul_deleted_trch_info_list_r6() */

/*================================================================================================
FUNCTION: get_ul_deleted_trch()

DESCRIPTION:
  This function takes a pointer to the Uplink Deleted 
  Transport Channel IE and Deletes if from Ordered Config.

  In the TFS it resets all information for this TrCH.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH deletions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_ul_deleted_trch
(
  rrc_UL_TransportChannelIdentity * ie_ptr
)
{
  /* TrCH Index */
  uint8 ix;

  if(ie_ptr->ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
  {
    ix = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr,
                                     (tr_ch_id_type)ie_ptr->ul_TransportChannelIdentity);
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("No index allocated for TrCH ID %d, return SUCCESS",
          ie_ptr->ul_TransportChannelIdentity);
      return(SUCCESS);
    }
  }
  else
  {
    ix = rrcllc_find_ul_rach_trch_idx();
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("No index allocated for TrCH ID %d, return FAILURE",
          ie_ptr->ul_TransportChannelIdentity);
      return(FAILURE);
    }
  }
  /* Clear the RB Mapping info for this TrCH */
  if(ie_ptr->ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
  {

    rrcllc_backup_rbmapping_info(ix,UL_DCH_RB_MAPPING);

    rrcllc_init_ul_dch_rb_mapping_info(ordered_config_ptr, ix);
  }
  else /* must be CELL_FACH */
  {
    rrcllc_backup_rbmapping_info(ix,RACH_RB_MAPPING);

    rrcllc_init_rach_rb_mapping_info(ordered_config_ptr);
  }

  /* Clear the TFS for this TrCH */
  rrcllc_init_ul_tfs_table(ordered_config_ptr, ix);

  /* Update UL TrCH counts now */
  /* MAC count */
  if(ie_ptr->ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
  {
    ordered_config_ptr->ul_cctrch.dch_info.ndchs--;
    /* Indicate that the old index is now available */
    if(rrcllc_deallocate_ul_dch_trch_idx(ix) == FAILURE)
    {
      return FAILURE;
    }
#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

    /* Remove the RLC size restriction info */
    rrcllc_init_dch_rlc_size_info(ix);
#endif/*#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH*/

  }
  ordered_config_ptr->ul_tfcs.nchan--;
  /* L1 count */
  ordered_config_ptr->rrc_ul_cctrch_params.num_trch--; 
  
  MSG_MED("Delete UL TrCH %d", ie_ptr->ul_TransportChannelIdentity, 0,0);
  return(SUCCESS);
} /* end get_ul_deleted_trch() */

/*================================================================================================
FUNCTION: get_ul_added_reconfig_trch()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_ul_added_reconfig_trch
(
  rrc_UL_AddReconfTransChInformation * ie_ptr
)
{

  MSG_MED("Get UL Add/Reconf TrCH",0,0,0);


    if(rrcllc_dch_usch_add_trch_info((tr_ch_id_type)ie_ptr->transportChannelIdentity,
                              &ie_ptr->transportFormatSet) == FAILURE)
      {
        return FAILURE;
      }

  return SUCCESS;
} /* end get_ul_added_reconfig_trch() */

/*================================================================================================
FUNCTION    :   compute_ul_tfis_from_ctfc 

DESCRIPTION :   For each TFCI, compute TFIs from CTFC

                First compute "P" value for each Transport Channel (P[i]) using the following :

                P[i] = (Number of TFs of "i"th TrCh) * P[i-1], where i=1,2,3,...I

                where P[0] = 1, and "I" is the number of Transport Channels

                Then for each CTFC, compute the TFs using the following algorithm :

                m = CTFC;
                i = I;          (where "I" is number of Transport Channels)
                while (i >0) {
                    TFI[
                }

DEPENDENCIES:   None.

RETURN VALUE:   None.

SIDE EFFECTS:   None.
===============================================================================================*/
void    compute_ul_tfis_from_ctfc
(
    /* Pointer to TFCS Ordered Config Data */
    mac_ul_tfcs_type    *tfcs_ptr
)
{
    /* Transport Format Combination Index */
    uint8   tfc_idx;

    /* Transport Channel Index */
    int8    trch_idx;

    /* P - values -- one for each transport channel */
    uint32  p_value[UE_MAX_TRCH];

    /* M-Value */
    uint32  m_value;

    /* Pointer to TFC data in MAC Ordered Config Database */
    mac_ul_tfc_type     *tfc_ptr;

    /* P[0] = 1 */
    p_value[0] = 1;

    /* Compute P- vales of all the transport channels */
    /* P[i] = P[i-1] * L[i-1], where "L" is Number of TFs of (i-1) Transport Channel */
    for (trch_idx = 1; trch_idx <tfcs_ptr->nchan; trch_idx++){
        p_value[trch_idx] = (tfcs_ptr->tfs_info[trch_idx-1].ntf) * p_value[trch_idx-1];             
    }

    /* Derive Transport Formats from each CTFC */
    for (tfc_idx=0; tfc_idx < tfcs_ptr->ntfc; tfc_idx++){

        /* Pointer to Transport Format Combination in MAC ordered config database */
        tfc_ptr = &tfcs_ptr->tfc_info[tfc_idx];

        /* Initialy "m" value = CTFC value */
        m_value = tfc_ptr->ctfc;
    
        /* Compute TFI for each Transport channel */
    for (trch_idx = (int8)(tfcs_ptr->nchan-1); trch_idx >= 0; trch_idx--)
    {

            /* TFI value */
            tfc_ptr->tfi[trch_idx] = (uint8)(m_value / p_value[trch_idx]);
            
            /* Update m-value to be used to compute TFI for next Transport Channel */
            m_value = (m_value % p_value[trch_idx]);
        }
    }
} /* end compute_ul_tfis_from_ctfc() */

/*====================================================================
FUNCTION: get_dl_semistatic_tf_info()

DESCRIPTION:
  This is called from the transport format set info update function.
  This updates the Semistatic TF info for DL TrChs.
  
  References: 25.331 (2000-12) 10.3.5.11

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_dl_semistatic_tf_info
(
  /* RRC TF semistatic TF info IE struct pointer */
  rrc_SemistaticTF_Information *ie_ptr,
  /* DL TF semistatic param info struct pointer */
  l1_dl_trch_info_struct_type  *dl_semistatic_tf_info
)
{
  /* Get the channel coding type */
  switch (ie_ptr->channelCodingType.t)
  {
    case T_rrc_ChannelCodingType_noCoding:
      dl_semistatic_tf_info->coding_type = L1_DL_UNCODED;
      break;
    case T_rrc_ChannelCodingType_turbo:
      dl_semistatic_tf_info->coding_type = L1_DL_TURBO_CODING;
      break;
    case T_rrc_ChannelCodingType_convolutional:
      /* for convolutional coding get the DL coding type based on rate */
      if (ie_ptr->channelCodingType.u.convolutional == rrc_CodingRate_half)
        dl_semistatic_tf_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
      else
        dl_semistatic_tf_info->coding_type = L1_DL_CONV_1_BY_3_CODING;

      break;
    default:
      WRRC_MSG1_ERROR("Invalid RRC DL coding type %d",
                ie_ptr->channelCodingType.t);
      return FAILURE;
  } /* end switch-case, TrCh coding type */

  /* Get the rate matching attribute */
  dl_semistatic_tf_info->rate_match_attrib = 
    (uint16) (ie_ptr->rateMatchingAttribute);
    
  /* get the CRC size used for this TrCh */
  switch (ie_ptr->crc_Size)
  {
    case rrc_CRC_Size_crc0:
      dl_semistatic_tf_info->crc_length = L1_CRC_LENGTH_0;
      break;
    case rrc_CRC_Size_crc8:
      dl_semistatic_tf_info->crc_length = L1_CRC_LENGTH_8;
      break;
    case rrc_CRC_Size_crc12:
      dl_semistatic_tf_info->crc_length = L1_CRC_LENGTH_12;
      break;
    case rrc_CRC_Size_crc16:
      dl_semistatic_tf_info->crc_length = L1_CRC_LENGTH_16;
      break;
    case rrc_CRC_Size_crc24:
      dl_semistatic_tf_info->crc_length = L1_CRC_LENGTH_24;
      break;
    default:
      WRRC_MSG1_ERROR("Invalid RRC DL CRC value %d",
                ie_ptr->crc_Size);
      return FAILURE;
  } /* end switch-case, CRE size type */

  return SUCCESS;
} /* end get_dl_semistatic_tf_info() */

/*====================================================================
FUNCTION: get_dl_tfcs_explicit_configuration()

DESCRIPTION:
  This function get the CTFC information for the DL from the
  Explicit TFCS information IE in RRC. This function needs the
  DL CTFC information pointer passed as a paramater.
  The CTFC structure has array of CTFC entries. The elements of 
  the array are expected to be valid CTFC entry structure pointers
  up to MAX TFC as defined by macro UE_MAX_TFC. The caller of this
  function must initialize the CTFC entry array pointer with pointers
  to the valid structures.
  
  There are 4 type of CTFC explicit configurations:
  1. Complete reconfiguration
  2. Addtion of CTFC
  3. Removal of CTFC
  4. Replacement of CTFCs (Removal and addtion)
  
  In the addition configuration the least TFC index available (never
  used or freed in previous removal) is occupied.
  In the replace the TFCIs removed may not be equal to the TFCI to be
  added.
  
  References: 25.331 (2000-12) 10.3.5.13, 10.3.5.15, 10.3.5.16

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_dl_tfcs_explicit_configuration
(
  ordered_config_type  *config_ptr,
  /* RRC TFCS explicit configuration IE structure pointer */
  rrc_ExplicitTFCS_Configuration  *ie_ptr,
  boolean                          sccpch_with_ctch,
  /* DL CTFC structure pointer */
  rrcllc_dl_ctfc_info_struct_type *dl_tfcs_info
)
{
  /* function local variables */
  /* ------------------------ */
  
  /* This variable maintains the index of CTFC entry that is
    available next */
  uint8 cur_ctfc_entry_index;

  /* pointer to the individual CTFC entry (or TFCS info struct */
  rrcllc_dl_ctfc_entry_info_struct_type *this_ctfc_entry;

  rrc_TFCS_ReconfAdd *tfc_add_cfg_info;
  rrc_TFCS_RemovalList * tfc_remove_info;

  /* next TFC pointer */
  rrc_TFCS_ReconfAdd_ctfc2Bit  *tfc_ctfc2bit;
  rrc_TFCS_ReconfAdd_ctfc4Bit  *tfc_ctfc4bit;
  rrc_TFCS_ReconfAdd_ctfc6Bit  *tfc_ctfc6bit;
  rrc_TFCS_ReconfAdd_ctfc8Bit  *tfc_ctfc8bit;
  rrc_TFCS_ReconfAdd_ctfc12Bit *tfc_ctfc12bit;
  rrc_TFCS_ReconfAdd_ctfc16Bit *tfc_ctfc16bit;
  rrc_TFCS_ReconfAdd_ctfc24Bit *tfc_ctfc24bit;
  uint32 idx =0;
  /*** function code starts here ***/

  /* Get the CTFC information based on the type of CTFC update operation */
  switch (ie_ptr->t)
  {

    /* Complete reconfiguration */
    /* ------------------------ */
    case T_rrc_ExplicitTFCS_Configuration_complete:
      /* First initialize all DL CTFCs in L1 and local structures */ 
      {
        if (sccpch_with_ctch)
        {
          rrcllc_init_dl_ctfcs_for_sccpch_with_ctch(config_ptr);
        }
        else
        {
          rrcllc_init_dl_ctfcs(config_ptr);
        }
      }

      /* get the pointer to the complete reconfig of TFCS info.
        Initialize the CTFC entry (TFC) indedx to 0 */
      tfc_add_cfg_info = (ie_ptr->u.complete);
      cur_ctfc_entry_index = 0;

      /* get the all CTFC values (all TFC) based on the CTFC size */
      switch(ie_ptr->u.complete->ctfcSize.t)
      {
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc2Bit:
          tfc_ctfc2bit = tfc_add_cfg_info->ctfcSize.u.ctfc2Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc2bit, ctfc2)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc4Bit:
          tfc_ctfc4bit = tfc_add_cfg_info->ctfcSize.u.ctfc4Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc4bit, ctfc4)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc6Bit:
          tfc_ctfc6bit = tfc_add_cfg_info->ctfcSize.u.ctfc6Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc6bit, ctfc6)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc8Bit:
          tfc_ctfc8bit = tfc_add_cfg_info->ctfcSize.u.ctfc8Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc8bit, ctfc8)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc12Bit:
          tfc_ctfc12bit = tfc_add_cfg_info->ctfcSize.u.ctfc12Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc12bit, ctfc12)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc16Bit:
          tfc_ctfc16bit = tfc_add_cfg_info->ctfcSize.u.ctfc16Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc16bit, ctfc16)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc24Bit:
          tfc_ctfc24bit = tfc_add_cfg_info->ctfcSize.u.ctfc24Bit;
          GET_DL_TFCS_EXPLICIT_CFG_RECFG(tfc_ctfc24bit, ctfc24)
          break;
        default:
          WRRC_MSG0_ERROR("Invalid RRC DL Explicit TFCS cfg CTFC size");
          return FAILURE;
      }
      break;

    /* CTFC addition */
    /* ------------- */
    case T_rrc_ExplicitTFCS_Configuration_addition:
      /* get the pointer to the add TFCS info and initialize the
         CTFC entry (TFC) index to 0 */
      tfc_add_cfg_info = (ie_ptr->u.addition);
      cur_ctfc_entry_index = 0;
      
      /* find the least available TFCIs and update the TFC info at those
        indicies from the TFCS add info based on the CTFC value size */
      switch (ie_ptr->u.addition->ctfcSize.t)
      {
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc2Bit:
          tfc_ctfc2bit = tfc_add_cfg_info->ctfcSize.u.ctfc2Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc2bit, ctfc2)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc4Bit:
          tfc_ctfc4bit = tfc_add_cfg_info->ctfcSize.u.ctfc4Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc4bit, ctfc4)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc6Bit:
          tfc_ctfc6bit = tfc_add_cfg_info->ctfcSize.u.ctfc6Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc6bit, ctfc6)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc8Bit:
          tfc_ctfc8bit = tfc_add_cfg_info->ctfcSize.u.ctfc8Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc8bit, ctfc8)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc12Bit:
          tfc_ctfc12bit = tfc_add_cfg_info->ctfcSize.u.ctfc12Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc12bit, ctfc12)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc16Bit:
          tfc_ctfc16bit = tfc_add_cfg_info->ctfcSize.u.ctfc16Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc16bit, ctfc16)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc24Bit:
          tfc_ctfc24bit = tfc_add_cfg_info->ctfcSize.u.ctfc24Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc24bit, ctfc24)
          break;
        default:
          WRRC_MSG0_ERROR("Invalid RRC DL Explicit TFCS add CTFC size");
          return FAILURE;
      }
      break;

    /* CTFC or TFC is removed */
    /* ---------------------- */
    case T_rrc_ExplicitTFCS_Configuration_removal:
      /* get the TFCI to be removed value and mark its state to be INVALID */
      tfc_remove_info = ie_ptr->u.removal;
      while (tfc_remove_info->n > idx)
      {
        if (tfc_remove_info->elem[idx].tfci >= UE_MAX_TFC)
        {
          WRRC_MSG2_ERROR("RRC DL EXP TFCI RM idx %d > MAX idx %d",
                    tfc_remove_info->elem[idx].tfci, UE_MAX_TFC);
          return FAILURE;
        }

        dl_tfcs_info->ctfc_entry_data[tfc_remove_info->elem[idx].tfci].state = INVALID;
        idx++;
      }
      break;

    /* CTFC replacement */
    /* ---------------- */
    case T_rrc_ExplicitTFCS_Configuration_replacement:
      /* Remove the TFC and then ...... */
      tfc_remove_info = &ie_ptr->u.replacement->tfcsRemoval;
      while (tfc_remove_info->n > idx )
      {
        if (tfc_remove_info->elem[idx].tfci >= UE_MAX_TFC)
        {
          WRRC_MSG2_ERROR("RRC DL EXP TFCI RP idx %d > MAX idx %d",
                    tfc_remove_info->elem[idx].tfci, UE_MAX_TFC);
          return FAILURE;
        }

        dl_tfcs_info->ctfc_entry_data[tfc_remove_info->elem[idx].tfci].state = INVALID;
        idx++;
      }

      /* ...... Add the new TFC */
      tfc_add_cfg_info = &(ie_ptr->u.replacement->tfcsAdd);
      cur_ctfc_entry_index = 0;
      idx =0;
      switch (ie_ptr->u.replacement->tfcsAdd.ctfcSize.t)
      {
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc2Bit:
          tfc_ctfc2bit = tfc_add_cfg_info->ctfcSize.u.ctfc2Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc2bit, ctfc2)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc4Bit:
          tfc_ctfc4bit = tfc_add_cfg_info->ctfcSize.u.ctfc4Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc4bit, ctfc4)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc6Bit:
          tfc_ctfc6bit = tfc_add_cfg_info->ctfcSize.u.ctfc6Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc6bit, ctfc6)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc8Bit:
          tfc_ctfc8bit = tfc_add_cfg_info->ctfcSize.u.ctfc8Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc8bit, ctfc8)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc12Bit:
          tfc_ctfc12bit = tfc_add_cfg_info->ctfcSize.u.ctfc12Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc12bit, ctfc12)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc16Bit:
          tfc_ctfc16bit = tfc_add_cfg_info->ctfcSize.u.ctfc16Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc16bit, ctfc16)
          break;
        case T_rrc_TFCS_ReconfAdd_ctfcSize_ctfc24Bit:
          tfc_ctfc24bit = tfc_add_cfg_info->ctfcSize.u.ctfc24Bit;
          GET_DL_TFCS_EXPLICIT_CFG_ADD(tfc_ctfc24bit, ctfc24)
          break;
        default:
          WRRC_MSG0_ERROR("Invalid RRC DL Explicit TFCS add CTFC size");
          return FAILURE;
      }
      break;

    default:
      WRRC_MSG0_ERROR("Invalid RRC Explicit TFCS cfg req type");
      return FAILURE;
  } /* end ctfc action switch */

  return SUCCESS;
} /* end get_dl_tfcs_explicit_configuration() */

/*====================================================================
FUNCTION: get_dl_tfcs_info()

DESCRIPTION:
  This function get the CTFC information for the DL from the
  Explicit TFCS information IE in RRC. This function needs the
  DL CTFC information pointer passed as a paramater. This parameter
  is passed to the further lower level functions.
  For this parameter requirement see function:
    get_dl_tfcs_explicit_configuration
      
  References: 25.331 (2000-12) 10.3.5.20

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_dl_tfcs_info
(
  ordered_config_type  *config_ptr,
  /* RRC TFCS configuration IE structure pointer */
  rrc_TFCS                        *ie_ptr,
  boolean                          sccpch_with_ctch,
  /* DL CTFC structure pointer */
  rrcllc_dl_ctfc_info_struct_type *dl_tfcs_info
)
{
  /* Set flag indicating that dl_tfcs has been rcvd */
  dl_tfcs_rcvd = TRUE;
  switch (ie_ptr->t)
  {
    case T_rrc_TFCS_normalTFCI_Signalling:
      if(get_dl_tfcs_explicit_configuration(config_ptr,
                                            (ie_ptr->u.normalTFCI_Signalling),
                                             sccpch_with_ctch,
                                            dl_tfcs_info) == FAILURE)
      {
        return FAILURE;
      }
      break;

    case T_rrc_TFCS_dummy:
    default:
      WRRC_MSG1_ERROR("RRC DL cfg, Split mode cfg not supported/Unknown cfg type %d", ie_ptr->t);
      return FAILURE;
  }

  /* Indicate that Downlink L1 needs to be reconfigured */
  config_ptr->reconfig_needed.downlink_l1 = TRUE;
  /* Although no new parameters for MAC have been received, MAC 
   * still needs to be reconfigured so as to get the new CCTrCH
   * ID whenever Downlink L1 is reconfigured. This is due to the
   * interface requirements.
   */
  config_ptr->reconfig_needed.downlink_mac = TRUE;

  return SUCCESS;
} /* end get_dl_tfcs_info() */

/*====================================================================
FUNCTION: get_dl_tfs_dedicated_trch()

DESCRIPTION:
  This function get the transport format set information for the
  dedicated transport channel. It gets both semistaic and dynamic
  transport formart information. This function needs a parameter of type
  l1_dl_trch_info_struct_type that has array of pointers to TF information.
  Ther caller of this function is required to initialize the pointers
  of TF info structures to the valid ones up to the MAX TF per TrCh
  possible.
  
  The RLC size in the dedicated TrCh TFS info is RLC PDU. To get the TB
  size from RLC size (RLC PDU), the MAC header should be added to the
  RLC PDU size.
  TB Size = MAC Header + RLC PDU
  MAC header in case of Dedicated channel depends on the number of logical
  channel mapped to the TrCh. In case of multiple logical channel mapped
  to this TrCh (represented by this TFS info) the MAC header size is 4
  otherwise it is 0.
  
  In the Dynamic transport format info the information is in 2 loops.
  The first loop is for the RLC size (TB size info). For this TB size info
  there can be list of number of TBs for that size.
  Each pair (TB size, number of TB) constitute one TF. For example
  RLC Size = X
     Num TB size list [0, 1, 2]
  RLC Size = Y
     Num TB size list [3, 4]
  The TFI and list will be as following (each index has num_TB x TB size):
    TFI 0 : 0 x X
    TFI 1 : 1 x X
    TFI 2 : 2 x X
    TFI 3 : 3 x Y
    TFI 4 : 4 x Y
      
  References: 25.331 (2000-12) 10.3.5.23

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_dl_tfs_dedicated_trch
(
  /* RRC TFS configuration IE structure pointer */
  rrc_DedicatedTransChTFS     *ie_ptr,
  /* DL trch channel info pointer */
  l1_dl_trch_info_struct_type *dl_tfs_info,
  /* indicate if there are multiple logical channel in this TrCh */
  boolean                      multiple_logical_ch
)
{
  /* function local variables */
  /* ------------------------ */
  
  uint16 tb_size;
  uint32 tmp_tb_size;
  uint32 idx=0,idy=0;
  /* This ensures the previous TFS is trashed */
  uint8 cur_tf_idx = 0;

  rrc_DedicatedDynamicTF_InfoList * tf_info_list;
  rrc_DedicatedDynamicTF_Info_numberOfTbSizeList * num_tb_list;

  /* get the semistatic information */
  if(get_dl_semistatic_tf_info(&(ie_ptr->semistaticTF_Information), dl_tfs_info)
     == FAILURE)
  {
    return FAILURE;
  }
  
  /* get the TTI type and pointer to the Dynamic transport information */
  switch (ie_ptr->tti.t)
  {
    case T_rrc_DedicatedTransChTFS_tti_tti10:
      dl_tfs_info->tti_value = L1_TTI_10MS;
      tf_info_list = ie_ptr->tti.u.tti10;
      break;

    case T_rrc_DedicatedTransChTFS_tti_tti20:
      dl_tfs_info->tti_value = L1_TTI_20MS;
      tf_info_list = ie_ptr->tti.u.tti20;
      break;

    case T_rrc_DedicatedTransChTFS_tti_tti40:
      dl_tfs_info->tti_value = L1_TTI_40MS;
      tf_info_list = ie_ptr->tti.u.tti40;
      break;

    case T_rrc_DedicatedTransChTFS_tti_tti80:
      dl_tfs_info->tti_value = L1_TTI_80MS;
      tf_info_list = ie_ptr->tti.u.tti80;
      break;

    case T_rrc_DedicatedTransChTFS_tti_dynamic:
    default:
      tf_info_list = NULL;
      WRRC_MSG1_ERROR("RRC DL TFS cfg: Dyn. tti not expected/Unknown TTI type %d",ie_ptr->tti.t);
      return FAILURE;

  } /* TTI Type switch */

  /* Get the Dynamic transport format information */
  /* -------------------------------------------- */
  while (tf_info_list->n > idx)
  {
    /* Get the RLC size for this TF info and adjust that to TB size */
    tmp_tb_size = get_tfs_ded_rlc_size(&(tf_info_list->elem[idx]));
    if(tmp_tb_size == RRCLLC_INVALID_RLC_SIZE)
    {
      return FAILURE;
    }
    tb_size = (uint16)tmp_tb_size;

    /* tb size for MAC headers */
    if((tb_size != 0) && (multiple_logical_ch))
    {
      tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
    }
    idy =0;
    /* get all of the number of TB for this TB size */
    num_tb_list = &tf_info_list->elem[idx].numberOfTbSizeList;
    while (num_tb_list->n > idy)
    {
      /* Check if we have reached the max TRCH that we can accomodate */ 
      if (cur_tf_idx >= MAX_TF)
      {
        WRRC_MSG1_ERROR("RRC DL TFS Ded > %d TFs", cur_tf_idx);
        return FAILURE;
      }

      if ((dl_tfs_info->tf_info_array[cur_tf_idx]->num_tb =
           get_tfs_num_tb(&(num_tb_list->elem[idy]))) == 0xFF)
      {
        return FAILURE;
      }

      dl_tfs_info->tf_info_array[cur_tf_idx]->tb_size = tb_size;
      dl_tfs_info->tf_info_array[cur_tf_idx]->allowed = TRUE;
      cur_tf_idx++;
      idy++;

    } /* End while, get complete number of TB list for this TB size */

    idx++;
  } /* End while, Get complete TB size list */

  /* update the number of TF obtained in the above Dynamic transport
     format information. */
  dl_tfs_info->num_tf = cur_tf_idx;

  return SUCCESS;
} /* end get_dl_tfs_dedicated_trch() */

/*====================================================================
FUNCTION: get_dl_tfs_common_trch()

DESCRIPTION:
  This function get the transport format set information for the
  common transport channel. It gets both semistaic and dynamic
  transport formart information. This function needs a parameter of type
  l1_dl_trch_info_struct_type that has array of pointers to TF information.
  Ther caller of this function is required to initialize the pointers
  of TF info structures to the valid ones up to the MAX TF per TrCh
  possible.
  
  The RLC size in the comomn TrCh TFS info is alwaysRLC PDU TB size.
  
  For information on how the TFI are set in the Dunamic TF info see
  description of function get_dl_tfs_dedicated_trch.
  
  References: 25.331 (2000-12) 10.3.5.23

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_dl_tfs_common_trch
(
  /* RRC TFS configuration IE structure pointer */
  rrc_CommonTransChTFS        *ie_ptr,
  /* DL trch channel info pointer */
  l1_dl_trch_info_struct_type *dl_tfs_info
)
{
  /* function local variables */
  /* ------------------------ */
  
  uint16 tb_size;
  uint32 tmp_tb_size;

  /* This ensures the previous TFS is trashed */
  uint8 cur_tf_idx = 0;
  uint32 idx =0,idy=0;
  rrc_CommonDynamicTF_InfoList * tf_info_list;
  rrc_CommonDynamicTF_Info_numberOfTbSizeList *  num_tb_list;
  
  /* get the semistatic information */
  if(get_dl_semistatic_tf_info(&(ie_ptr->semistaticTF_Information), dl_tfs_info)
     == FAILURE)
  {
    return FAILURE;
  }

  /* get the TTI type and pointer to the Dynamic transport information */
  switch (ie_ptr->tti.t)
  {
    case T_rrc_CommonTransChTFS_tti_tti10:
      dl_tfs_info->tti_value = L1_TTI_10MS;
      tf_info_list = ie_ptr->tti.u.tti10;
      break;
    case T_rrc_CommonTransChTFS_tti_tti20:
      dl_tfs_info->tti_value = L1_TTI_20MS;
      tf_info_list = ie_ptr->tti.u.tti20;
      break;
    case T_rrc_CommonTransChTFS_tti_tti40:
      dl_tfs_info->tti_value = L1_TTI_40MS;
      tf_info_list = ie_ptr->tti.u.tti40;
      break;
    case T_rrc_CommonTransChTFS_tti_tti80:
      dl_tfs_info->tti_value = L1_TTI_80MS;
      tf_info_list = ie_ptr->tti.u.tti80;
      break;
    case T_rrc_CommonTransChTFS_tti_dynamic:
    default:
      tf_info_list = NULL;
      WRRC_MSG1_ERROR("RRC DL TFS cfg: Dyn. tti not expected/Unknown TTI type %d",ie_ptr->tti.t);
      return FAILURE;
  } /* end TTI type switch */

  /* Get the Dynamic transport format information */
  /* -------------------------------------------- */
  while (tf_info_list->n > idx)
  {
    /* Get TB size. Here RLC size = TB size */
    tmp_tb_size = get_tfs_com_rlc_size(&(tf_info_list->elem[idx]));
    if(tmp_tb_size == RRCLLC_INVALID_RLC_SIZE)
    {
      return FAILURE;
    }
    tb_size = (uint16)tmp_tb_size;
    idy=0;
    /* get all of the number of TB for this TB size */
    num_tb_list = &tf_info_list->elem[idx].numberOfTbSizeList;
    while (num_tb_list->n > idy)
    {
      if (cur_tf_idx >= MAX_TF)
      {
        WRRC_MSG1_ERROR("RRC DL TFS Common > %d TFs", cur_tf_idx);
        return (FAILURE);
      }

      if ((dl_tfs_info->tf_info_array[cur_tf_idx]->num_tb =
           get_tfs_num_tb(&(num_tb_list->elem[idy]))) == 0xFF)
      {
        return FAILURE;
      }

      dl_tfs_info->tf_info_array[cur_tf_idx]->tb_size = tb_size;
      dl_tfs_info->tf_info_array[cur_tf_idx]->allowed = TRUE;
      cur_tf_idx++;
      idy++;
    } /* End while, get complete number of TB list for this TB size */

    idx++;
  } /* End while, Get complete TB size list */

  /* update the number of TF obtained in the above Dynamic transport
     format information. */
  dl_tfs_info->num_tf = cur_tf_idx;

  return SUCCESS;
} /* end get_dl_tfs_common_trch() */

/*====================================================================
FUNCTION: get_dl_transport_format_set()

DESCRIPTION:
  This function get transport format set information for dedicated or
  common transport channel based on the choce of the transport channel
  type. It calls the appropriate function to get the information.
  
  References: 25.331 (2000-12) 10.3.5.23

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_dl_transport_format_set
(
  /* RRC TFS configuration IE structure pointer */
  rrc_TransportFormatSet      *ie_ptr,
  /* DL trch channel info pointer */
  l1_dl_trch_info_struct_type *dl_ded_tfs_info,
  /* indicate the multiple logical channel in this TrCh */
  boolean                      multiple_logical_ch,
  /* DL common trch channel info pointer */
  l1_dl_trch_info_struct_type *dl_com_tfs_info
)
{
  switch (ie_ptr->t)
  {
    case T_rrc_TransportFormatSet_dedicatedTransChTFS:
      if(dl_ded_tfs_info == NULL)
      {
        WRRC_MSG0_ERROR("NULL pointer for DL Ded TFS Info");
        return(FAILURE);
      }
      if(get_dl_tfs_dedicated_trch((ie_ptr->u.dedicatedTransChTFS),
                                   dl_ded_tfs_info, multiple_logical_ch)
         == FAILURE)
      {
        return FAILURE;
      }
      break;

    case T_rrc_TransportFormatSet_commonTransChTFS:
      if(dl_com_tfs_info == NULL)
      {
        WRRC_MSG0_ERROR("NULL pointer for DL Common TFS Info");
        return(FAILURE);
      }
      if(get_dl_tfs_common_trch((ie_ptr->u.commonTransChTFS), dl_com_tfs_info)
         == FAILURE)
      {
        return FAILURE;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid RRC DL TFS t %d", ie_ptr->t);
      return FAILURE;
  }

  return SUCCESS;
} /* end get_dl_transport_format_set() */

/*=============================================================================
FUNCTION: get_dl_info_common_for_all_trch()

DESCRIPTION:
  This function takes a pointer to the Uplink Common Information for all
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===============================================================================*/
uecomdef_status_e_type get_dl_info_common_for_all_trch
(
  rrc_DL_CommonTransChInfo *ie_ptr,
  rrc_UL_CommonTransChInfo *ul_ptr,
  rrc_state_e_type  next_rrc_state
)
{
  MSG_MED("Get DL Common info for all TrCH",0,0,0);

  /* Check if SCCPCH TFCS is present */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_CommonTransChInfo,sccpch_TFCS))
  {
    WRRC_MSG0_ERROR("Rcv'd SCCPCH TFCS info, Not applicable to CELL_DCH");
    return FAILURE;
  }

  if(next_rrc_state == RRC_STATE_CELL_DCH)
  {
    /* Check for the FDD specific info if it is included */
    switch(ie_ptr->modeSpecificInfo.t)
    {
      /*
      * If FDD TFCS is signalled, it will either be included here, or else grab
      * it from the Uplink TFCS
      */
      case T_rrc_DL_CommonTransChInfo_modeSpecificInfo_fdd:
        if(ie_ptr->modeSpecificInfo.u.fdd->m.dl_ParametersPresent)
        {
          switch(ie_ptr->modeSpecificInfo.u.fdd->dl_Parameters.t)
          {
            /* DL TFCS Included in this message */
            case T_rrc_DL_CommonTransChInfo_dl_Parameters_dl_DCH_TFCS:
              MSG_MED("Get New DL TFCS",0,0,0);
              if(get_dl_tfcs_info(ordered_config_ptr,
                                   ie_ptr->modeSpecificInfo.u.fdd->dl_Parameters.u.dl_DCH_TFCS,
                                   FALSE,
                                  &ordered_config_ptr->dl_ctfc) == FAILURE)
              {
                return FAILURE;
              }
              break;
              
            /* Get the TFCS from the Uplink IE */
            case T_rrc_DL_CommonTransChInfo_dl_Parameters_sameAsUL:
  
              /* Determine if Uplink Data was included */
              if((ul_ptr != NULL)&& (RRC_MSG_COMMON_BITMASK_IE_PTR(ul_ptr,
                    rrc_UL_CommonTransChInfo,modeSpecificInfo)))
              {
                switch(ie_ptr->modeSpecificInfo.t)
                {
                  /* Got a good UL TFCS */
                  case T_rrc_DL_CommonTransChInfo_modeSpecificInfo_fdd:
                    MSG_MED("Use UL TFCS as New DL TFCS",0,0,0);
                    if(get_dl_tfcs_info(ordered_config_ptr,
                                        &ul_ptr->modeSpecificInfo.u.fdd->ul_TFCS,
                                       FALSE,
                                        &ordered_config_ptr->dl_ctfc) == FAILURE)
                    {
                      return FAILURE;
                    }
                    break;
  
                /* Either TDD or invalid value */
                default:
                  WRRC_MSG0_ERROR("Rcv'd no UL TFCS; Cannot get DL TFCS");
                  return FAILURE;
                } /* End of uplink switch */
              } /* end of UL TFCS info */
              else
              {
                WRRC_MSG0_HIGH("Rcv'd no UL TFCS, DL same as UL");
                /* Copy the TFCS from CC to OC */
                rrcllc_copy_ul_tfcs_info_to_dl_tfcs();
              }
              break;
  
            default:
              WRRC_MSG1_ERROR("Invalid DL TFCS Signalled %d",
                        ie_ptr->modeSpecificInfo.u.fdd->dl_Parameters.t);
              return FAILURE;
  
          } /* end TFCS switch */
        } /* end TFCS signalling mode present check */
        else
        {
          MSG_MED("Using existing DL TFCS",0,0,0);
        }
        break; /* end FDD TFCS chosen */
  
      case T_rrc_DL_CommonTransChInfo_modeSpecificInfo_tdd:
        WRRC_MSG0_ERROR("TDD Mode not supported for DL Common TRCH Info");
        break;
  
      default:
        WRRC_MSG1_ERROR("Invalid Mode %d rcv'd in DL Common TRCH Info IE",
                  ie_ptr->modeSpecificInfo.t);
        return FAILURE;
    } /* End mode switch */
  }  /* next RRC state is Cell DCH */

  return SUCCESS;
} /* end get_dl_info_common_for_all_trch() */


/*=============================================================================
FUNCTION: get_dl_info_common_for_all_trch_r5()

DESCRIPTION:
  This function takes a pointer to the Uplink Common Information for all
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
===============================================================================*/
uecomdef_status_e_type get_dl_info_common_for_all_trch_r5
(
  rrc_DL_CommonTransChInfo_r4 *ie_ptr,
  rrc_UL_CommonTransChInfo_r4 *ul_ptr,
  rrc_state_e_type  next_rrc_state
)
{

  /* Check if SCCPCH TFCS is present */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
   rrc_DL_CommonTransChInfo_r4,sccpch_TFCS))
  {
    WRRC_MSG0_ERROR("Rcv'd SCCPCH TFCS info, Not applicable to CELL_DCH");
    return FAILURE;
  }

  if(next_rrc_state == RRC_STATE_CELL_DCH)
  {
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_CommonTransChInfo_r4,
  modeSpecificInfo))
    {
      WRRC_MSG1_MED("Mode %d rcv'd in DL Common TRCH Info IE",ie_ptr->modeSpecificInfo.t);

      /* Check for the FDD specific info if it is included */
      switch(ie_ptr->modeSpecificInfo.t)
      {
        /*
        * If FDD TFCS is signalled, it will either be included here, or else grab
        * it from the Uplink TFCS
        */
        case T_rrc_DL_CommonTransChInfo_r4_modeSpecificInfo_fdd:
          if(ie_ptr->modeSpecificInfo.u.fdd->m.dl_ParametersPresent)
          {
            switch(ie_ptr->modeSpecificInfo.u.fdd->dl_Parameters.t)
            {
              /* DL TFCS Included in this message */
              case T_rrc_DL_CommonTransChInfo_r4_dl_Parameters_dl_DCH_TFCS:

                /* For the time being, reject the config if tfcs is not present */

                if (ie_ptr->modeSpecificInfo.u.fdd->
                    dl_Parameters.u.dl_DCH_TFCS->m.tfcsPresent)
                {                
                if(get_dl_tfcs_info(ordered_config_ptr,
                    &ie_ptr->modeSpecificInfo.u.fdd->dl_Parameters.u.dl_DCH_TFCS->tfcs,
                                   FALSE,
                    &ordered_config_ptr->dl_ctfc) == FAILURE)
                {
                  return FAILURE;
                }

                }
                else
                {
                  WRRC_MSG0_ERROR("RRCHS:DL TFCS bit mask abs");
                  return FAILURE;
                }
                break;
                
              /* Get the TFCS from the Uplink IE */
              case T_rrc_DL_CommonTransChInfo_r4_dl_Parameters_sameAsUL:
    
                /* Determine if Uplink Data was included */
                if((ul_ptr != NULL) && (RRC_MSG_COMMON_BITMASK_IE_PTR(
       ul_ptr,rrc_UL_CommonTransChInfo_r4,modeSpecificInfo)))
                {
                  switch(ie_ptr->modeSpecificInfo.t)
                  {
                    /* Got a good UL TFCS */
                    case T_rrc_DL_CommonTransChInfo_r4_modeSpecificInfo_fdd:
                      if(get_dl_tfcs_info(ordered_config_ptr,
                                          &ul_ptr->modeSpecificInfo.u.fdd->ul_TFCS,
                                   FALSE,
                                          &ordered_config_ptr->dl_ctfc) == FAILURE)
                      {
                        return FAILURE;
                      }
                      break;
    
                  /* Either TDD or invalid value */
                  default:
                    WRRC_MSG0_ERROR("Rcv'd no UL TFCS; Cannot get DL TFCS");
                    return FAILURE;
                  } /* End of uplink switch */
                } /* end of UL TFCS info */
                else
                {
                  WRRC_MSG0_HIGH("Rcv'd no UL TFCS, DL same as UL");
                  rrcllc_copy_ul_tfcs_info_to_dl_tfcs();
                }
                break;
    
              default:
                WRRC_MSG1_ERROR("Invalid DL TFCS Signalled %d",
                          ie_ptr->modeSpecificInfo.u.fdd->dl_Parameters.t);
                return FAILURE;
    
            } /* end TFCS switch */
          } /* end TFCS signalling mode present check */
          else
          {
            WRRC_MSG0_HIGH("Using existing DL TFCS");
          }
          break; /* end FDD TFCS chosen */
    
        case T_rrc_DL_CommonTransChInfo_r4_modeSpecificInfo_tdd:
          WRRC_MSG0_ERROR("TDD Mode not supported for DL Common TRCH Info");
          break;
    
        default:
          WRRC_MSG1_ERROR("Invalid Mode %d rcv'd in DL Common TRCH Info IE",
                    ie_ptr->modeSpecificInfo.t);
          return FAILURE;
      } /* End mode switch */
    }
  }  /* next RRC state is Cell DCH */

  return SUCCESS;
} /* end get_dl_info_common_for_all_trch_r5() */

/*=============================================================================
FUNCTION: get_dl_deleted_trch()

DESCRIPTION:
  This function takes a pointer to the Downlink Deleted 
  Transport Channel IE and Deletes if from Ordered Config.
  In the TFS it resets all information for this TrCH.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
===============================================================================*/
uecomdef_status_e_type get_dl_deleted_trch
(
  rrc_DL_TransportChannelIdentity * ie_ptr
)
{
  /* TrCH Index */
  uint8 ix;

  if(ie_ptr->dl_TransportChannelType == rrc_DL_TrCH_Type_dch)
  {
    ix = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelIdentity);
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_HIGH("No DL DCH index for TrCH ID %d",
          ie_ptr->dl_TransportChannelIdentity);
      return(SUCCESS);
    }

    rrcllc_backup_rbmapping_info(ix,DL_DCH_RB_MAPPING );
    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_dl_dch_rb_mapping_info(ordered_config_ptr, ix);

    /* Clear the TFS for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, ix);

    /* Update MAC count */
    ordered_config_ptr->mac_dl_parms.num_dchs--;

    /* Initialize local data */
    ordered_config_ptr->dl_dch_trch_info[ix].dch_present = FALSE;
    ordered_config_ptr->dl_dch_trch_info[ix].trch_id     = 0;
  
    /* Indicate that the old index is now available */
    if(rrcllc_deallocate_dl_dch_trch_idx(ix) == FAILURE)
    {
      return FAILURE;
    }
  }
  else  /* Must be FACH */
  {
    // ix = rrcllc_find_dl_fach_trch_idx(ie_ptr->value.dl_TransportChannelIdentity); 
    ix = rrcllc_find_dl_fach_trch_idx();
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("No FACH index for TrCH ID %d",
          ie_ptr->dl_TransportChannelIdentity);
      return(FAILURE);
    }
    rrcllc_backup_rbmapping_info(ix,FACH_RB_MAPPING );

    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_fach_rb_mapping_info(ordered_config_ptr, ix);

    /* Clear the TFS for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, ix);

    /* Update MAC count */
    ordered_config_ptr->mac_dl_parms.num_fachs--;
  }

  /* Update L1 count */
  ordered_config_ptr->l1_dl_cctrch_parms.num_trch--;
  
  MSG_MED("Deleted DL TrCH %d",ie_ptr->dl_TransportChannelIdentity,0,0);

  return SUCCESS;
} /* end get_dl_deleted_trch() */

/*============================================================================
FUNCTION: rrcllc_delete_dl_dch_trch_index()

DESCRIPTION:
  This function takes a Transport channel index as a
  parameter and clears local, L1 and MAC data at the
  given index. It also decrements count of total number
  of transport channels in local, MAC and L1 data.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH deletions are the only item supported now.
=============================================================================*/
uecomdef_status_e_type rrcllc_delete_dl_dch_trch_index
(
  uint8   trch_idx
)
{
  if(trch_idx < UE_MAX_TRCH)
  {
    /* Clear the RB Mapping info (MAC) for this TrCH */
    rrcllc_init_dl_dch_rb_mapping_info(ordered_config_ptr, trch_idx);

    /* Clear the TFS (L1) for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, trch_idx);

    /* Initialize local data */
    ordered_config_ptr->dl_dch_trch_info[trch_idx].dch_present = FALSE;
    ordered_config_ptr->dl_dch_trch_info[trch_idx].trch_id     = 0;

    /* Free the TrCH index for further use */
    if(rrcllc_deallocate_dl_dch_trch_idx(trch_idx) == FAILURE)
    {
      return FAILURE;
    }

    /* One less TrCH - update local, MAC and L1 data structures */
    //ordered_config_ptr->dl_trch_cnt--;
    ordered_config_ptr->mac_dl_parms.num_dchs--;
    ordered_config_ptr->l1_dl_cctrch_parms.num_trch--;

    MSG_MED("Deleted DL TrCH Idx %d",trch_idx, 0, 0);
    return(SUCCESS);
  }
  else
  {
    return(FAILURE);
  }
} /* end rrcllc_delete_dl_dch_trch_index() */

/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_dl_added_reconfig_trch
(
  rrc_DL_AddReconfTransChInformation * ie_ptr,
  rrc_UL_AddReconfTransChInfoList *  ul_ptr,
  boolean                         tm_signalling_allowed
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = RRCLLC_INVALID_TRCH_IDX;
  tr_ch_id_type ul_trch_idx = RRCLLC_INVALID_TRCH_IDX;

  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  uint32 idx=0;
  /* Loop variables for copying UL TFS info */
  uint8 tf_idx = 0;
  /*lint -e578 Declaration of symbol 'rrc_dl_tf_info' hides symbol 'rrc_dl_tf_info'*/
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;

  /* Boolean used to indicate if multiple logical channels are mapped to
     a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;

  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;

  MSG_MED("Get DL Add/Reconf TrCH",0,0,0);

  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    /* Get the transport channel index for this transport channel ID.
     * Note: The index should already be there.
     */
    trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_transportChannelIdentity);

    if(trch_idx >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found",
          ie_ptr->dl_transportChannelIdentity);
      return(FAILURE);
    }
    if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
    {
      /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, info not correct in Standalone RB-Mapping",
          ie_ptr->dl_transportChannelIdentity);
    }
    
    /* Determine if multiple dedicated logical channels are present on this TrCH id */
    if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
    {
      multiple_ded_log_chan = TRUE;
    }

    /*
    * Get whether or not the TFS for a given TRCH is included or if it needs
    * to come from an uplink TrCH ID.*/
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation2_tfs_SignallingMode_explicit_config:
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
                                       &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
                                       multiple_ded_log_chan, /* mulitple logical channels? */
                                       &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
                                      ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */

      case T_rrc_DL_AddReconfTransChInformation2_tfs_SignallingMode_sameAsULTrCH:
        /*
        * Make sure the UL Info was included, and if it was get the TFS for 
        * the TrCH ID that was included in this message.
        */
        while(ul_ptr != NULL && ul_ptr->n > idx)
        {
          if(ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
             ul_TransportChannelIdentity == ul_ptr->elem[idx].transportChannelIdentity)
          { 
            /* Got a match with Uplink TrCH ID */
            MSG_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
                    ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
                    ul_ptr->elem[idx].transportChannelIdentity,0
                   );
            if(get_dl_transport_format_set(&ul_ptr->elem[idx].transportFormatSet,
                                           &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
                                           multiple_ded_log_chan, /* mulitple logical channels? */
                                           &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
                                          ) == FAILURE)
            {
              return FAILURE;
            }

            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */

          /* no match, keep looking */
          else 
          {
            idx++;
          }
        } /* end while */

        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
           * Note: The index should already be there.
           */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
              ordered_config_ptr,
              (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
                ul_TransportChannelIdentity);
          if(ul_trch_idx >= UE_MAX_TRCH)
          {
            WRRC_MSG2_ERROR("No DL TFS for TrCH %d, to use UL TrCH %d for TFS",
                ie_ptr->dl_transportChannelIdentity,
                ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
                  ul_TransportChannelIdentity);
            return(FAILURE);
          }

          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];

          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];

          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
            case L1_UL_CODING_NONE:
                    dl_tfs_info->coding_type = L1_DL_UNCODED;
                    break;
            case L1_UL_CODING_CONV_ONE_HALF:
                    dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
                    break;
            case L1_UL_CODING_CONV_ONE_THIRD:
                    dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
                    break;
            case L1_UL_CODING_TURBO:
                    dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
                    break;
            default:break; 
          }
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
            case L1_UL_CRC_0_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
              break;
            case L1_UL_CRC_8_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
              break;
            case L1_UL_CRC_12_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
              break;
            case L1_UL_CRC_16_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
              break;
            case L1_UL_CRC_24_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
              return FAILURE;
          }

          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8)ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];

            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;
            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);
            
            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if ((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }

            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 

      default:
        WRRC_MSG1_ERROR("No DL TFCS was included for TrCH ID %d",
                  ie_ptr->dl_transportChannelIdentity);
        return FAILURE;

    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
 PDU size has changed.*/ 
    rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_transportChannelIdentity, 
              ordered_config_ptr->rrc_state_indicator, DL); 
  } /* end CELL_DCH check */

  /* States not supported */
  else
  {
     WRRC_MSG1_ERROR("State %d not supported for TrCH",ordered_config_ptr->rrc_state_indicator);
     return FAILURE;
  }

  /***************************************************************/
  /* Transport Channel Count has been updated in RB Mapping info */
  /***************************************************************/

  /* Check for the BLER */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation,
   dch_QualityTarget))
  {
    /* Save the BLER if found, and is in range */
    if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
       (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER)
      )
    {
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
        (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
    }

    /* BLER is out of range */
    else
    {
      /* According to L1 I/F, must set an invalid value of -127 */
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
      WRRC_MSG1_ERROR("DL BLER %d is out of range",
                ie_ptr->dch_QualityTarget.bler_QualityValue
               );
      return FAILURE;
    }
  } /* end if BLER present */
  else
  {
    /* IE omitted. Set the target to -127 which would result 1% of BLER target.
     * This is good UE behaviour though the spec is ambiguous on what UE
     * behavior should be in this case.  
     */
    WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
    ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
  }

  /* Now check to see if allowed and is time to set up TM signalling info */
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation,dummy)) &&
     (tm_signalling_allowed)
    )
  {
    WRRC_MSG0_HIGH("TM Signalling on DCCH not currently supported");
    /* don't need to return a failure now */
  }
  else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation,dummy)) &&
          (!tm_signalling_allowed)
         ) 
  {
    WRRC_MSG0_ERROR("TM Signalling on DCCH not supported for this message");
    return FAILURE;  /* something is messed up in msg proccessing land */
  }
  else if(!(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation,dummy)) &&
          (tm_signalling_allowed)
         ) 
  {
    MSG_MED("TM Signalling on DCCH not required for this message config",0,0,0);
  }
  else if(!(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation,dummy)) &&
          !(tm_signalling_allowed)
         ) 
  {
    MSG_MED("No TM Signalling on DCCH",0,0,0);
  }
  else
  {
    ERR_FATAL("Something special happened(Bad)",0,0,0);
  }

  /* Indicate that DL L1 needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;

  return SUCCESS;
  /*lint +e578 Declaration of symbol 'rrc_dl_tf_info' hides symbol 'rrc_dl_tf_info'*/
} /* end get_dl_added_reconfig_trch() */


/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_2_r5()

DESCRIPTION:
  This function takes a pointer to the Rel 5 Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG. This function
  will not allow for the option of setting up a TM DCCH entity
  implicitly.
  NOTE: This function is similar to get_dl_added_reconfig_trch except
  that this function takes rrc_DL_AddReconfTransChInfo2List as 
  argument as against rrc_DL_AddReconfTransChInfoList taken by
  get_dl_added_reconfig_trch. There are subtle differences between
  rrc_DL_AddReconfTransChInfo2List and rrc_DL_AddReconfTransChInfoList.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_dl_added_reconfig_trch_2_r5
(
  rrc_DL_AddReconfTransChInformation_r4 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList  * ul_ptr
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = RRCLLC_INVALID_TRCH_IDX;
  tr_ch_id_type ul_trch_idx = RRCLLC_INVALID_TRCH_IDX;

  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  uint32 idx=0;
  /* Loop variables for copying UL TFS info */
  /*lint -e578 */
  uint8 tf_idx = 0;
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  /*lint +e578 */

  /* Boolean used to indicate if multiple logical channels are mapped to
     a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;

  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;

  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    /* Get the transport channel index for this transport channel ID.
     * Note: The index should already be there.
     */
    trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_transportChannelIdentity);
    if(trch_idx >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found",
          ie_ptr->dl_transportChannelIdentity);
      return(FAILURE);
    }
    if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
    {
      /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping",
          ie_ptr->dl_transportChannelIdentity);
    }
    
    /* Determine if multiple dedicated logical channels are present on this TrCH id */
    if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
    {
      multiple_ded_log_chan = TRUE;
    }

    /*
    * Get whether or not the TFS for a given TRCH is included or if it needs
    * to come from an uplink TrCH ID.
    */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation2_tfs_SignallingMode_explicit_config:
        
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
                                       &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
                                       multiple_ded_log_chan, /* mulitple logical channels? */
                                       &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
                                      ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */

      case T_rrc_DL_AddReconfTransChInformation2_tfs_SignallingMode_sameAsULTrCH:
        /*
        * Make sure the UL Info was included, and if it was get the TFS for 
        * the TrCH ID that was included in this message.
        */
        while(ul_ptr != NULL && ul_ptr->n > idx)
        {
          if(ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
             ul_TransportChannelIdentity == ul_ptr->elem[idx].transportChannelIdentity)
          { 
            /* Got a match with Uplink TrCH ID */
            MSG_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
                    ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
                    ul_ptr->elem[idx].transportChannelIdentity,0
                   );
            if(get_dl_transport_format_set(&ul_ptr->elem[idx].transportFormatSet,
                                           &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
                                           multiple_ded_log_chan, /* mulitple logical channels? */
                                           &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
                                          ) == FAILURE)
            {
              return FAILURE;
            }

            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */

          /* no match, keep looking */
          else 
          {
            idx++;
          }
        } /* end while */

        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
           * Note: The index should already be there.
           */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
              ordered_config_ptr,
              (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
                ul_TransportChannelIdentity);
          if(ul_trch_idx >= UE_MAX_TRCH)
          {
            WRRC_MSG1_ERROR("No DL TFS to use UL TrCH %d for TFS",
                ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
                  ul_TransportChannelIdentity);
            return(FAILURE);
          }
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
            case L1_UL_CODING_NONE:
                    dl_tfs_info->coding_type = L1_DL_UNCODED;
                    break;
            case L1_UL_CODING_CONV_ONE_HALF:
                    dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
                    break;
            case L1_UL_CODING_CONV_ONE_THIRD:
                    dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
                    break;
            case L1_UL_CODING_TURBO:
                    dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
                    break;
            default:break; 
          }
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
            case L1_UL_CRC_0_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
              break;
            case L1_UL_CRC_8_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
              break;
            case L1_UL_CRC_12_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
              break;
            case L1_UL_CRC_16_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
              break;
            case L1_UL_CRC_24_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
              return FAILURE;
          }
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8)ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;

            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if ((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 

      default:
        WRRC_MSG0_ERROR("No DL TFCS was included for TrCH ID");
        return FAILURE;

    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
 PDU size has changed.*/ 
    rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_transportChannelIdentity, 
              ordered_config_ptr->rrc_state_indicator, DL); 
  } /* end CELL_DCH check */

  /* States not supported */
  else
  {
     WRRC_MSG1_ERROR("State %d not supported for TrCH",ordered_config_ptr->rrc_state_indicator);
     return FAILURE;
  }

  /* Check for the BLER */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation_r4,
   dch_QualityTarget))
  {
    /* Save the BLER if found, and is in range */
    if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
       (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER)
      )
    {
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
        (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
    }

    /* BLER is out of range */
    else
    {
      /* According to L1 I/F, must set an invalid value of -127 */
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
      WRRC_MSG1_ERROR("DL BLER %d is out of range",
                ie_ptr->dch_QualityTarget.bler_QualityValue
               );
      return FAILURE;
    }
  } /* end if BLER present */
  else
  {
    /* IE omitted. Set the target to -127 which would result 1% of BLER target.
     * This is good UE behaviour though the spec is ambiguous on what UE
     * behavior should be in this case.  
     */
    WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
    ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
  }


  /* Indicate that DL L1 and MAC needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;

  return SUCCESS;
} /* end get_dl_added_reconfig_trch_2_r5() */



/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_2()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG. This function
  will not allow for the option of setting up a TM DCCH entity
  implicitly.
  NOTE: This function is similar to get_dl_added_reconfig_trch except
  that this function takes rrc_DL_AddReconfTransChInfo2List as 
  argument as against rrc_DL_AddReconfTransChInfoList taken by
  get_dl_added_reconfig_trch. There are subtle differences between
  rrc_DL_AddReconfTransChInfo2List and rrc_DL_AddReconfTransChInfoList.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_dl_added_reconfig_trch_2
(
  rrc_DL_AddReconfTransChInformation2 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList  * ul_ptr
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = RRCLLC_INVALID_TRCH_IDX;
  tr_ch_id_type ul_trch_idx = RRCLLC_INVALID_TRCH_IDX;

  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  uint32 idx=0;
  /* Loop variables for copying UL TFS info */
  /*lint -e578 */
  uint8 tf_idx = 0;
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  /*lint +e578 */

  /* Boolean used to indicate if multiple logical channels are mapped to
     a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;

  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;

  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    /* Get the transport channel index for this transport channel ID.
     * Note: The index should already be there.
     */
    trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->transportChannelIdentity);
    if(trch_idx >= UE_MAX_TRCH)
    {
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found",
          ie_ptr->transportChannelIdentity);
      return(FAILURE);
    }
    /* 
      * This will populate Ordered config with the info from Standlaone database, if
      *  the ordered config does not contain the Rb_mapping info 
      */

    if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
    {
      /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping",
          ie_ptr->transportChannelIdentity);
    } 
    
    /* Determine if multiple dedicated logical channels are present on this TrCH id */
    if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
    {
      multiple_ded_log_chan = TRUE;
    }

    /*
    * Get whether or not the TFS for a given TRCH is included or if it needs
    * to come from an uplink TrCH ID.
    */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation2_tfs_SignallingMode_explicit_config:
        MSG_MED("Get TFS for DL TrCH Id %d", ie_ptr->transportChannelIdentity,0,0);

        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
                                       &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
                                       multiple_ded_log_chan, /* mulitple logical channels? */
                                       &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
                                      ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */

      case T_rrc_DL_AddReconfTransChInformation2_tfs_SignallingMode_sameAsULTrCH:
        /*
        * Make sure the UL Info was included, and if it was get the TFS for 
        * the TrCH ID that was included in this message.
        */
        while(ul_ptr != NULL && ul_ptr->n > idx)
        {
          if(ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
             ul_TransportChannelIdentity == ul_ptr->elem[idx].transportChannelIdentity)
          { 
            /* Got a match with Uplink TrCH ID */
            MSG_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
                    ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
                    ul_ptr->elem[idx].transportChannelIdentity,0
                   );
            if(get_dl_transport_format_set(&ul_ptr->elem[idx].transportFormatSet,
                                           &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
                                           multiple_ded_log_chan, /* mulitple logical channels? */
                                           &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
                                          ) == FAILURE)
            {
              return FAILURE;
            }

            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */

          /* no match, keep looking */
          else 
          {
            idx++;
          }
        } /* end while */

        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
           * Note: The index should already be there.
           */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
              ordered_config_ptr,
              (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
                ul_TransportChannelIdentity);
      
          if(ul_trch_idx >= UE_MAX_TRCH)
          {
            WRRC_MSG1_ERROR("No DL TFS to use UL TrCH %d for TFS",
                ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
                  ul_TransportChannelIdentity);
            return(FAILURE);
          }
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
            case L1_UL_CODING_NONE:
                    dl_tfs_info->coding_type = L1_DL_UNCODED;
                    break;
            case L1_UL_CODING_CONV_ONE_HALF:
                    dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
                    break;
            case L1_UL_CODING_CONV_ONE_THIRD:
                    dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
                    break;
            case L1_UL_CODING_TURBO:
                    dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
                    break;
            default:break; 
          }
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
            case L1_UL_CRC_0_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
              break;
            case L1_UL_CRC_8_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
              break;
            case L1_UL_CRC_12_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
              break;
            case L1_UL_CRC_16_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
              break;
            case L1_UL_CRC_24_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
              return FAILURE;
          }
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8)ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;

            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);
            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if ((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }
            
            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 

      default:
        WRRC_MSG1_ERROR("No DL TFCS was included for TrCH ID %d",
                  ie_ptr->transportChannelIdentity);
        return FAILURE;

    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
 PDU size has changed.*/  
    rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->transportChannelIdentity, 
              ordered_config_ptr->rrc_state_indicator, DL); 
  } /* end CELL_DCH check */

  /* States not supported */
  else
  {
     WRRC_MSG1_ERROR("State %d not supported for TrCH",ordered_config_ptr->rrc_state_indicator);
     return FAILURE;
  }

  /* Check for the BLER */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,qualityTarget))
  {
    /* Save the BLER if found, and is in range */
    if((ie_ptr->qualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
       (ie_ptr->qualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER)
      )
    {
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
        (int8)ie_ptr->qualityTarget.bler_QualityValue;
    }

    /* BLER is out of range */
    else
    {
      /* According to L1 I/F, must set an invalid value of -127 */
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
      WRRC_MSG1_ERROR("DL BLER %d is out of range",
                ie_ptr->qualityTarget.bler_QualityValue
               );
      return FAILURE;
    }
  } /* end if BLER present */
  else
  {
    /* IE omitted. Set the target to -127 which would result 1% of BLER target.
     * This is good UE behaviour though the spec is ambiguous on what UE
     * behavior should be in this case.  
     */
    WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
    ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
  }


  /* Indicate that DL L1 and MAC needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;

  return SUCCESS;
} /* end get_dl_added_reconfig_trch_2() */

/*============================================================================
FUNCTION: rrcllc_copy_dl_dch_mac_info()

DESCRIPTION:
  This function copies all Downlink DCH transport channel related info 
  relevant to MAC from source data structure to destination data structure.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE.

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type
rrcllc_copy_dl_dch_mac_info
(
  mac_dl_dch_trch_config_type *dest_data,
  mac_dl_dch_trch_config_type *src_data
)
{
  int i;
  if((dest_data == NULL)||(src_data == NULL))
  {
    WRRC_MSG0_HIGH("NULL pointer passed");
    return(FAILURE);
  }

  dest_data->cctrch_id = src_data->cctrch_id;
  dest_data->trch_id   = src_data->trch_id;
  dest_data->ndlchan   = src_data->ndlchan;

  /* Copy data for all dedicated channels */
  for(i=0; i<MAX_DED_LOGCHAN_PER_TRCH; i++)
  {
    dest_data->dlchan_info[i] = src_data->dlchan_info[i];
  }

  return(SUCCESS);
}  /* rrcllc_copy_dl_dch_mac_info */

/*============================================================================
FUNCTION: rrcllc_copy_dl_dch_trch_info_to_buffer()

DESCRIPTION:
  This function copies all Downlink DCH transport channel related info to
  a local buffer from L1, MAC and local data structures indexed by the passed 
  index.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void  rrcllc_copy_dl_dch_trch_info_to_buffer(uint32 trch_idx)
{
  /* Copies all Downlink DCH Transport Channel related data to a 
   * temporary buffer
   */ 
  uint32  tf_count;
  /* Copy the L1 data first */
  /* Note: the mapping between transport channel data and the corresponding
   * transport format data is kept unchanged. ie a transport channel at index 
   * "i" will still point to transport format data at index "i".
   */
  dl_dch_info_buffer.l1_info = ordered_config_ptr->dl_trch_info[trch_idx];

  for(tf_count = 0; tf_count < MAX_TF; tf_count ++)
  {
    /* Copy each transport format data from src to dest index */
    dl_dch_info_buffer.l1_tf_info[tf_count] = 
                ordered_config_ptr->dl_tf_info[trch_idx][tf_count];
    /* Point L1 data at dest_trch_idx to corresponding transport format data */
    dl_dch_info_buffer.l1_info.tf_info_array[tf_count] = 
              &dl_dch_info_buffer.l1_tf_info[tf_count];
  }

  /* Copy MAC data next */
  (void)rrcllc_copy_dl_dch_mac_info(&dl_dch_info_buffer.mac_info, 
                              &ordered_config_ptr->dl_dch_info[trch_idx]);

  /* Copy local data now */
  dl_dch_info_buffer.local_info = ordered_config_ptr->dl_dch_trch_info[trch_idx];
} /* rrcllc_copy_dl_dch_trch_info_to_buffer */

/*============================================================================
FUNCTION: rrcllc_copy_dl_dch_trch_info_from_buffer()

DESCRIPTION:
  This function copies all Downlink DCH transport channel related info from
  a local buffer to L1, MAC and local data structures indexed by the passed 
  index.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void  rrcllc_copy_dl_dch_trch_info_from_buffer(uint32 trch_idx)
{
  /* Copies all Downlink DCH Transport Channel related data from
   * a buffer to the various local and non-local data structures.
   */
  uint32  tf_count;

  /* Copy the L1 data first */
  /* Note: the mapping between transport channel data and the corresponding
   * transport format data is kept unchanged. ie a transport channel at index 
   * "i" will still point to transport format data at index "i".
   */
  ordered_config_ptr->dl_trch_info[trch_idx] = dl_dch_info_buffer.l1_info;

  for(tf_count = 0; tf_count < MAX_TF; tf_count ++)
  {
    /* Copy each transport format data from src to dest index */
    ordered_config_ptr->dl_tf_info[trch_idx][tf_count] =
      dl_dch_info_buffer.l1_tf_info[tf_count];
    /* Point L1 data at dest_trch_idx to corresponding transport format data */
    ordered_config_ptr->dl_trch_info[trch_idx].tf_info_array[tf_count] =
      &ordered_config_ptr->dl_tf_info[trch_idx][tf_count];
  }

  /* Copy MAC data next */
  (void)rrcllc_copy_dl_dch_mac_info(&ordered_config_ptr->dl_dch_info[trch_idx],
                              &dl_dch_info_buffer.mac_info); 

  /* Copy local data now */
  ordered_config_ptr->dl_dch_trch_info[trch_idx] = dl_dch_info_buffer.local_info;
} /* rrcllc_copy_dl_dch_trch_info_from_buffer */


/*====================================================================
FUNCTION        rrcllc_copy_ul_tfcs_info_to_dl_tfcs()

DESCRIPTION     This function copied TFCS information from Uplink TFCS
                in Ordered Config to Downlink TFCS information in Ordered
                Config

DEPENDENCIES
RETURN VALUE    None

SIDE EFFECTS
====================================================================*/
void rrcllc_copy_ul_tfcs_info_to_dl_tfcs
(
  void
)
{
    /* local variables */
  uint8 ix=0, iy=0;


  /* Set flag indicating that dl_tfcs has been rcvd */
  dl_tfcs_rcvd = TRUE;

  rrcllc_init_dl_ctfcs(ordered_config_ptr);

  for(ix = 0; ix < UE_MAX_TFC; ix++)
  {
    /* Init the DL Transport Format Combination data pointers */
    ordered_config_ptr->l1_dl_ctfc_parms.ctfc_entry_array[ix] = &ordered_config_ptr->ctfc_info[ix];

    /* Initialize the pointer to tfi_table in CTFC Info */
    ordered_config_ptr->ctfc_info[ix].tfi_table = ordered_config_ptr->dl_ctfc.ctfc_entry_data[ix].tfi_table;

  }

  /* Now copy the ctfcs from UL to DL */

  if(ordered_config_ptr->ul_tfcs.ntfc != 0)
  {
    ordered_config_ptr->dl_ctfc.last_ctfc_entry_index = 
      ordered_config_ptr->l1_dl_ctfc_parms.last_ctfc_entry_index = 
      (ordered_config_ptr->ul_tfcs.ntfc - 1);
  }
  else
  {
    ordered_config_ptr->dl_ctfc.last_ctfc_entry_index = 
      ordered_config_ptr->l1_dl_ctfc_parms.last_ctfc_entry_index = RRC_DL_INVALID_CTFC_VALUE;  
  }


  WRRC_MSG2_HIGH("LAST CTFC L1_DL_CTFC_PARMS in OC %d NO OF UL TFCs %d",
           ordered_config_ptr->l1_dl_ctfc_parms.last_ctfc_entry_index,ordered_config_ptr->ul_tfcs.ntfc);


  /* Set TFCI Existence */
  ordered_config_ptr->l1_dl_ctfc_parms.tfci_existence = TRUE;
  ordered_config_ptr->l1_dl_ctfc_parms.pdsch_info_valid = FALSE;


  /* Get the tfc */
  for(ix = 0; ix < ordered_config_ptr->ul_tfcs.ntfc; ix++)
  {
  
    if (ordered_config_ptr->ul_tfcs.tfc_info[ix].control_flag == TFC_NOT_ALLOWED)
    {
      ordered_config_ptr->dl_ctfc.ctfc_entry_data[ix].state = RESTRICTED;
    }
    else if ((ordered_config_ptr->ul_tfcs.tfc_info[ix].control_flag == TFC_ALLOWED) ||
             (ordered_config_ptr->ul_tfcs.tfc_info[ix].control_flag == TFC_MANDATORY))
    {
      ordered_config_ptr->dl_ctfc.ctfc_entry_data[ix].state = ALLOWED;
      ordered_config_ptr->l1_dl_ctfc_parms.ctfc_entry_array[ix]->valid_ctfc = TRUE;

      ordered_config_ptr->dl_ctfc.ctfc_entry_data[ix].ctfc_value = 
        ordered_config_ptr->ul_tfcs.tfc_info[ix].ctfc;
    }
    else
    {
      ordered_config_ptr->dl_ctfc.ctfc_entry_data[ix].state = INVALID;
    }
    MSG_HIGH("ULTFC No %d,Flag [0:A,1NA,2M, 3RM] %d CTFC Value %d ", ix,
                      ordered_config_ptr->ul_tfcs.tfc_info[ix].control_flag,
                      ordered_config_ptr->dl_ctfc.ctfc_entry_data[ix].ctfc_value);
    /* Set in l1_dl_ctfc_parms*/

    ordered_config_ptr->l1_dl_ctfc_parms.ctfc_entry_array[ix]->pdsch_multicode_value = 0;

    ordered_config_ptr->l1_dl_ctfc_parms.ctfc_entry_array[ix]->pdsch_ovsf_code = 0;

    ordered_config_ptr->l1_dl_ctfc_parms.ctfc_entry_array[ix]->pdsch_spreading_factor = 
     L1_DL_SF_4;


    for(iy = 0; iy < UE_MAX_TRCH; iy++)
    {
      ordered_config_ptr->l1_dl_ctfc_parms.ctfc_entry_array[ix]->tfi_table[iy] = 
        ordered_config_ptr->ul_tfcs.tfc_info[ix].tfi[iy];
    }
  }

}


/*============================================================================
FUNCTION: rrcllc_copy_dl_dch_trch_info_from_one_to_next_index()

DESCRIPTION:
  This function copies all Downlink DCH transport channel related info from
  L1, MAC and local data structures indexed by the passed source index to
  L1, MAC and local data structures indexed by the passed destination index.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrcllc_copy_dl_dch_trch_info_from_one_to_next_index(
      uint8 dest_trch_idx, 
      uint8 src_trch_idx
)
{
  /* Copies all DL DCH related Transport Channel data from Source
   * Transport Channel Index to Destination Transport Channel Index.
   */
  uint32  tf_count;

  /* Copy the L1 data first */
  /* Note: the mapping between transport channel data and the corresponding
   * transport format data is kept unchanged. ie a transport channel at index 
   * "i" will still point to transport format data at index "i".
   */
  ordered_config_ptr->dl_trch_info[dest_trch_idx] = ordered_config_ptr->dl_trch_info[src_trch_idx];
  
  for(tf_count = 0; tf_count < MAX_TF; tf_count ++)
  {
    /* Copy each transport format data from src to dest index */
    ordered_config_ptr->dl_tf_info[dest_trch_idx][tf_count] = 
                ordered_config_ptr->dl_tf_info[src_trch_idx][tf_count];
    /* Point L1 data at dest_trch_idx to corresponding transport format data */
    ordered_config_ptr->dl_trch_info[dest_trch_idx].tf_info_array[tf_count] = 
              &ordered_config_ptr->dl_tf_info[dest_trch_idx][tf_count];
  }

  /* Copy MAC data next */
  (void)rrcllc_copy_dl_dch_mac_info(&ordered_config_ptr->dl_dch_info[dest_trch_idx], 
                              &ordered_config_ptr->dl_dch_info[src_trch_idx]);
  
  /* Copy local data now */
  ordered_config_ptr->dl_dch_trch_info[dest_trch_idx] = ordered_config_ptr->dl_dch_trch_info[src_trch_idx];
  
} /* rrcllc_copy_dl_dch_trch_info_from_one_to_next_index */


/*============================================================================
FUNCTION: rrcllc_sort_dl_dch_trch_index_array()

DESCRIPTION:
  It sorts and re-arranges all Transport channel related 
  downlink info based on Transport channel IDs.
      
DEPENDENCIES:
  This function presumes that there are no "holes" in transport channel
  index array. Any holes have to be removed before a call to this function.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
static void  rrcllc_sort_dl_dch_trch_index_array(
  rrcllcoc_trch_info_type *dch_trch_info_ptr, 
  uint32  num_index
)
{
  uint8  index_for_sorting;
  uint8  to_be_compared_index;

  /* Make sure num_index doesn't read beyond the end of dch_trch_info_ptr_array */
  if (num_index > UE_MAX_TRCH) 
  {
    ERR_FATAL("num_idx %d greater than UE_MAX_TRCH %d",num_index, UE_MAX_TRCH, 0);
  }

  for(index_for_sorting = 1; index_for_sorting < num_index; index_for_sorting ++)
  {
    /* If the Transport Channel ID for the index for sort is NOT lower than the Transport
     * Channel ID for the index one lower than the one being sorted, do NOT do anything.
     * This is because all indices further below have already been sorted.
     */
    if((dch_trch_info_ptr[index_for_sorting].trch_id < 
          dch_trch_info_ptr[index_for_sorting-1].trch_id) &&
       ( dch_trch_info_ptr[index_for_sorting].trch_id != 0))
    {
      /* The TrCH ID for sorted index is lower than TrCH ID for index before
       * This is time to move all indices below till a space is found for 
       * data at current sorted index.
       */
      rrcllc_copy_dl_dch_trch_info_to_buffer(index_for_sorting);
      to_be_compared_index = index_for_sorting;

      while( (to_be_compared_index > 0) && 
             (dl_dch_info_buffer.local_info.trch_id < 
               dch_trch_info_ptr[to_be_compared_index-1].trch_id)
           )
      {
        dch_trch_info_ptr[to_be_compared_index] = 
          dch_trch_info_ptr[to_be_compared_index - 1];
        rrcllc_copy_dl_dch_trch_info_from_one_to_next_index(
                                     to_be_compared_index,
                                     to_be_compared_index-1);
        to_be_compared_index --;
      }
  
      rrcllc_copy_dl_dch_trch_info_from_buffer(to_be_compared_index);
    }
  } /* for(...) */
} /* rrcllc_sort_dl_dch_trch_index_array */


/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_dl_dch_trch_info()

DESCRIPTION:
  This function takes up deleted and added DL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_dl_dch_trch_info
(
  /* local DL TrCH Add Reconfig pointer */
  rrc_DL_AddReconfTransChInfoList * dl_trch_add_reconfig_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList    * dl_trch_delete_ptr
)
{
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  uint32 idx=0, idy =0;
  rrc_DL_DeletedTransChInfoList *temp_dl_trch_delete_ptr = dl_trch_delete_ptr;
  rrc_DL_AddReconfTransChInfoList  *temp_dl_trch_add_reconfig_ptr = dl_trch_add_reconfig_ptr;
  
  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    while(temp_dl_trch_delete_ptr->n > idx)
    {
      if(rrc_DL_TrCH_Type_dch == temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType)
      {
        idy = 0;
        while(temp_dl_trch_add_reconfig_ptr->n > idy)
        {
          if(rrc_DL_TrCH_Type_dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType)
          {
            if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelIdentity == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
         temp_dl_trch_add_reconfig_ptr->elem[idy].dl_transportChannelIdentity,temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelIdentity);
              return FAILURE;
            }
          }
          idy++;
        }
      }   
      idx++;
    }
  }
  idx = 0;
  while((dl_trch_delete_ptr != NULL) && (dl_trch_delete_ptr->n > idx))
  {
    /* Remove this transport channel from MAC, L1 and local data.
     * and update transport channel count in L1, MAC and local data.
     */
    if(get_dl_deleted_trch(&dl_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* set dl_tfcs_needed flag to true.  Later if common info is not preset,
     * UE needs to reject the message 
     */
    dl_tfcs_needed = TRUE;
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the DL Added/Reconfigured Info */
  while((dl_trch_add_reconfig_ptr !=NULL) && (dl_trch_add_reconfig_ptr->n > idx) )
  {
    trch_id = (tr_ch_id_type)dl_trch_add_reconfig_ptr->elem[idx].dl_transportChannelIdentity;

    if((rrcllc_get_dl_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
      return(FAILURE);
    }
    idx++;
  } /* while(...) */

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->dl_trch_cnt indices.
   */
  rrcllc_fill_holes_in_dl_dch_trch_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */

  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                     ordered_config_ptr->mac_dl_parms.num_dchs
                                       );
  return(SUCCESS);
} /* rrcllc_add_delete_and_arrange_dl_dch_trch_info */

/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_dl_dch_trch_info2()

DESCRIPTION:
  This function takes up deleted and added DL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
  NOTE: This function is similar to 
  rrcllc_add_delete_and_arrange_dl_dch_trch_info except that this takes
  rrc_DL_AddReconfTransChInfo2List as a parameter as opposed to
  rrc_DL_AddReconfTransChInfoList. There are subtle differences between
  rrc_DL_AddReconfTransChInfo2List and rrc_DL_AddReconfTransChInfoList.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_dl_dch_trch_info2
(
  /* local DL TrCH Add Reconfig pointer */
  rrc_DL_AddReconfTransChInfo2List * dl_trch_add_reconfig2_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList    * dl_trch_delete_ptr
)
{
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  uint32 idx =0, idy = 0;
  rrc_DL_DeletedTransChInfoList *temp_dl_trch_delete_ptr = dl_trch_delete_ptr;
  rrc_DL_AddReconfTransChInfo2List  *temp_dl_trch_add_reconfig_ptr = dl_trch_add_reconfig2_ptr;
  
  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    while(temp_dl_trch_delete_ptr->n > idx)
    {
      if(rrc_DL_TrCH_Type_dch == temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType)
     {
       idy = 0;
        while(temp_dl_trch_add_reconfig_ptr->n > idy)
        {
          if(rrc_DL_TrCH_Type_dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType)
          {
            if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelIdentity == temp_dl_trch_add_reconfig_ptr->elem[idy].transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
          temp_dl_trch_add_reconfig_ptr->elem[idy].transportChannelIdentity,temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelIdentity);
              return FAILURE;
            }
          }
          idy++;
       }
     }   
      idx++;
    }
  }
  idx = 0;
  while((dl_trch_delete_ptr != NULL) && (dl_trch_delete_ptr->n > idx))
  {
    /* Remove this transport channel from MAC, L1 and local data.
     * and update transport channel count in L1, MAC and local data.
     */
    if(get_dl_deleted_trch(&dl_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* set dl_tfcs_needed flag to true.  Later if common info is not preset,
     * UE needs to reject the message 
     */
    dl_tfcs_needed = TRUE;
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the DL Added/Reconfigured Info */
  while((dl_trch_add_reconfig2_ptr !=NULL) && (dl_trch_add_reconfig2_ptr->n > idx) )
  {
    trch_id = (tr_ch_id_type)dl_trch_add_reconfig2_ptr->elem[idx].transportChannelIdentity;

    if((rrcllc_get_dl_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
      return(FAILURE);
    }
    idx++;
  } /* while(...) */

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->dl_trch_cnt indices.
   */
  rrcllc_fill_holes_in_dl_dch_trch_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */

  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                      ordered_config_ptr->mac_dl_parms.num_dchs
                                      );
  return(SUCCESS);
} /* rrcllc_add_delete_and_arrange_dl_dch_trch_info2 */




/*=============================================================================
FUNCTION: get_dl_deleted_trch_r5()

DESCRIPTION:
  This function takes a pointer to the Downlink Deleted 
  Transport Channel IE and Deletes if from Ordered Config.
  In the TFS it resets all information for this TrCH.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
===============================================================================*/
static uecomdef_status_e_type get_dl_deleted_trch_r5
(
  rrc_DL_TransportChannelIdentity_r5 * ie_ptr
)
{
  /* TrCH Index */
  uint8 ix;
  uint8 i;

  if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId2_r5_dch))
  {
    ix = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG1_HIGH("No DL DCH index for TrCH ID %d",
        ie_ptr->dl_TransportChannelType.u.dch);
      return(SUCCESS);
    }
    rrcllc_backup_rbmapping_info(ix,DL_DCH_RB_MAPPING );

    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_dl_dch_rb_mapping_info(ordered_config_ptr, ix);
    
    /* Clear the TFS for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, ix);
    
    /* Update MAC count */
    ordered_config_ptr->mac_dl_parms.num_dchs--;

    /* Initialize local data */
    ordered_config_ptr->dl_dch_trch_info[ix].dch_present = FALSE;
    ordered_config_ptr->dl_dch_trch_info[ix].trch_id     = 0;
  
    /* Indicate that the old index is now available */
    if(rrcllc_deallocate_dl_dch_trch_idx(ix) == FAILURE)
    {
      return FAILURE;
    }
    /* Update L1 count */
    ordered_config_ptr->l1_dl_cctrch_parms.num_trch--;
    /* set dl_tfcs_needed flag to true.  Later if common info is not preset,
     * UE needs to reject the message 
     */
    dl_tfcs_needed = TRUE;
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
    rrc_DL_TrCH_TypeId2_r5_hsdsch))
  {
    WRRC_MSG1_HIGH("RRCHS:Delete DFLOW %d", ie_ptr->dl_TransportChannelType.u.hsdsch);
    if ((ix = rrcllc_find_dflow_index((uint8)ie_ptr->dl_TransportChannelType.u.hsdsch)) >= UE_MAX_MAC_D_FLOW)
    {
      WRRC_MSG1_ERROR("RRCHS:Unable to index for DFLOW %d",ie_ptr->dl_TransportChannelType.u.hsdsch);
      /* Allow the Reconfig Msg to be processed since spec allows it implicitly */
      return(SUCCESS);
    }
    /* Clear the Queue Information associated with the DFLOW*/

    rrcllc_backup_rbmapping_info(ix,MAC_D_RB_MAPPING );
    
    WRRC_MSG1_HIGH("RRCHS:Initialize Queue for DFLOW %d", ie_ptr->dl_TransportChannelType.u.hsdsch);
    /* Initialize the queue info */
    rrcllc_initialize_queue_info(ix);

    hsdpa_msg_params.msg_params_present = TRUE;

    hsdpa_msg_params.hsdsch_info_present = TRUE;
   /* If the Flow id was actually not deleted, that is the Rb-Mapping for this flow id is still present in the 
    * ordered config then delete it from Backup info
    */
    if ((ix = rrcllc_find_dflow_index((uint8)ie_ptr->dl_TransportChannelType.u.hsdsch)) == UE_MAX_MAC_D_FLOW)
    {
      WRRC_MSG1_HIGH("Mac-d flow id %d got deleted ",(ie_ptr->dl_TransportChannelType.u.hsdsch));
    }
    else
    {
      for(i=0;i<ordered_config_ptr->mac_dflow_info[ix].ndlchan;i++)
      {
        rrcllc_free_hanging_rb_mapping_info_index(ordered_config_ptr->mac_dflow_info[ix].dlchan_info[i].rb_id, MAC_D_RB_MAPPING);
      }
    }

  }
  
  else  /* Must be FACH */
  {
    // ix = rrcllc_find_dl_fach_trch_idx(ie_ptr->value.dl_TransportChannelIdentity); 
    ix = rrcllc_find_dl_fach_trch_idx();
    if(ix >= UE_MAX_TRCH)
    {
      WRRC_MSG0_ERROR("No FACH index for TrCH ID");
      return(FAILURE);
    }
    rrcllc_backup_rbmapping_info(ix,FACH_RB_MAPPING );

    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_fach_rb_mapping_info(ordered_config_ptr, ix);
    
    /* Clear the TFS for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, ix);
    
    /* Update MAC count */
    ordered_config_ptr->mac_dl_parms.num_fachs--;

    /* Update L1 count */
    ordered_config_ptr->l1_dl_cctrch_parms.num_trch--;
  }
  
  return SUCCESS;
} /* end get_dl_deleted_trch_r5() */



/*============================================================================
FUNCTION: rrcllc_copy_mac_ul_dch_mapping_info()

DESCRIPTION:
  This function copies all Uplink DCH transport channel mapping info 
  relevant to MAC from source data structure to destination data structure.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
static void  rrcllc_copy_mac_ul_dch_mapping_info(
  mac_ul_dch_trch_config_type *ul_dch_dest_info_ptr, 
  mac_ul_dch_trch_config_type *ul_dch_src_info_ptr 
)
{
  uint32   i;
  /* First copy all mapping info */
  ul_dch_dest_info_ptr->cctrch_id = ul_dch_src_info_ptr->cctrch_id;

  ul_dch_dest_info_ptr->trch_id = ul_dch_src_info_ptr->trch_id;

  /* Copy all info about dedicated logical channels */
  ul_dch_dest_info_ptr->ndlchan = ul_dch_src_info_ptr->ndlchan;

  for(i = 0; i < MAX_DED_LOGCHAN_PER_TRCH; i ++)
  {
    ul_dch_dest_info_ptr->dlchan_info[i] = 
      ul_dch_src_info_ptr->dlchan_info[i];
  }
} /* rrcllc_copy_mac_ul_dch_mapping_info */

/*============================================================================
FUNCTION: rrcllc_copy_mac_ul_dch_tfs_info()

DESCRIPTION:
  This function copies all Uplink DCH transport channel transport format info 
  relevant to MAC from source data structure to destination data structure.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
static void rrcllc_copy_mac_ul_dch_tfs_info(
  mac_ul_tfs_type   *dest_tfs_ptr,
  mac_ul_tfs_type   *src_tfs_ptr
  )
{
  uint32  i;  

  dest_tfs_ptr->ntf = src_tfs_ptr->ntf;
  dest_tfs_ptr->tti_info  = src_tfs_ptr->tti_info;

  /* copy the Transport format set info */
  for(i = 0; i < UE_MAX_TF; i ++)
  {
    dest_tfs_ptr->tf_info[i] = src_tfs_ptr->tf_info[i];
  }
}  /* rrcllc_copy_mac_ul_dch_tfs_info */

/*============================================================================
FUNCTION: rrcllc_copy_ul_dch_trch_info_from_one_to_next_index()

DESCRIPTION:
  This function copies all Uplink DCH transport channel related info from
  L1, MAC and local data structures indexed by the passed source index to
  L1, MAC and local data structures indexed by the passed destination index.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void  rrcllc_copy_ul_dch_trch_info_from_one_to_next_index(
      uint8 dest_trch_idx, 
      uint8 src_trch_idx
      )      
{
  /* Copies all UL DCH related Transport Channel data from Source
   * Transport Channel Index to Destination Transport Channel Index.
   */
  /* Copy the L1 data first */
  ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[dest_trch_idx] = 
        ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[src_trch_idx];
  /* Copy MAC data next */
  rrcllc_copy_mac_ul_dch_mapping_info(&ordered_config_ptr->ul_cctrch.dch_info.dch_info[dest_trch_idx],
                                      &ordered_config_ptr->ul_cctrch.dch_info.dch_info[src_trch_idx]);

  rrcllc_copy_mac_ul_dch_tfs_info(&ordered_config_ptr->ul_tfcs.tfs_info[dest_trch_idx], 
                                  &ordered_config_ptr->ul_tfcs.tfs_info[src_trch_idx]);
  
  /* Copy local data now */
  ordered_config_ptr->ul_dch_trch_info[dest_trch_idx] = ordered_config_ptr->ul_dch_trch_info[src_trch_idx];
  
} /* rrcllc_copy_ul_dch_trch_info_from_one_to_next_index */

/*============================================================================
FUNCTION: rrcllc_delete_ul_dch_trch_index()

DESCRIPTION:
  This function takes a Transport channel index as a
  parameter and clears local, L1 and MAC data at the
  given index. It also decrements count of total number
  of transport channels in local, MAC and L1 data.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE.

SIDE EFFECTS:
  CELL_DCH deletions are the only item supported now.
=============================================================================*/
uecomdef_status_e_type rrcllc_delete_ul_dch_trch_index
(
  uint8   trch_idx
)
{
  if(trch_idx < UE_MAX_TRCH)
  {
    /* Clear the RB Mapping info (MAC) for this TrCH */
    rrcllc_init_ul_dch_rb_mapping_info(ordered_config_ptr, trch_idx);

    /* Clear the TFS (L1) for this TrCH */
    rrcllc_init_ul_tfs_table(ordered_config_ptr, trch_idx);

    /* Initialize local data */
    ordered_config_ptr->ul_dch_trch_info[trch_idx].dch_present = FALSE;
    ordered_config_ptr->ul_dch_trch_info[trch_idx].trch_id     = 0;

    /* Free the TrCH index for further use */
    if(rrcllc_deallocate_ul_dch_trch_idx(trch_idx) == FAILURE)
    {
      return FAILURE;
    }

    /* One less TrCH - update local, MAC and L1 data structures */
    ordered_config_ptr->mac_ul_parms.cctrch_ptr->dch_info.ndchs --;
    ordered_config_ptr->mac_ul_parms.tfcs_ptr->nchan --;

    ordered_config_ptr->rrc_ul_cctrch_params.num_trch--; 

    MSG_MED("Deleted UL TrCH Idx %d",trch_idx, 0, 0);
    return(SUCCESS);
  }
  else
  {
    return(FAILURE);
  }
} /* end rrcllc_delete_ul_dch_trch_index() */

/*============================================================================
FUNCTION: rrcllc_copy_ul_dch_trch_info_to_buffer()

DESCRIPTION:
  This function copies all Uplink DCH transport channel related info to
  a local buffer from L1, MAC and local data structures indexed by the passed 
  index.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void  rrcllc_copy_ul_dch_trch_info_to_buffer(uint32 trch_idx)
{
  /* Copies all Uplink DCH Transport Channel related data to a 
   * temporary buffer
   */ 
  /* Copy the L1 data first */
  ul_dch_info_buffer.l1_semi_static_parms = 
          ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[trch_idx];
  /* Copy MAC data next */
  rrcllc_copy_mac_ul_dch_mapping_info(&ul_dch_info_buffer.mac_dch_info,
                                      &ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx]);

  rrcllc_copy_mac_ul_dch_tfs_info(&ul_dch_info_buffer.mac_tfs_info, 
                                  &ordered_config_ptr->ul_tfcs.tfs_info[trch_idx]);

  /* Copy local data now */
  ul_dch_info_buffer.local_info = ordered_config_ptr->ul_dch_trch_info[trch_idx];
} /* rrcllc_copy_ul_dch_trch_info_to_buffer */

/*============================================================================
FUNCTION: rrcllc_copy_ul_dch_trch_info_from_buffer()

DESCRIPTION:
  This function copies all Uplink DCH transport channel related info from
  a local buffer to L1, MAC and local data structures indexed by the passed 
  index.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void  rrcllc_copy_ul_dch_trch_info_from_buffer(uint32 trch_idx)
{
  /* Copies all Downlink DCH Transport Channel related data from
   * a buffer to the various local and non-local data structures.
   */

  /* Copy the L1 data first */
  ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[trch_idx] = ul_dch_info_buffer.l1_semi_static_parms;
  /* Copy MAC data next */
  rrcllc_copy_mac_ul_dch_mapping_info(&ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx],
                                      &ul_dch_info_buffer.mac_dch_info);

  rrcllc_copy_mac_ul_dch_tfs_info(&ordered_config_ptr->ul_tfcs.tfs_info[trch_idx],
                                  &ul_dch_info_buffer.mac_tfs_info);

  /* Copy local data now */
  ordered_config_ptr->ul_dch_trch_info[trch_idx] = ul_dch_info_buffer.local_info;
} /* rrcllc_copy_ul_dch_trch_info_from_buffer */


/*============================================================================
FUNCTION: rrcllc_sort_ul_dch_trch_index_array()

DESCRIPTION:
  It sorts and re-arranges all Transport channel related 
  info based on Transport channel IDs.
      
DEPENDENCIES:
  This function presumes that there are no "holes" in transport channel
  index array. For correct results, all holes HAVE to be removed before
  calling this function.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
static void  rrcllc_sort_ul_dch_trch_index_array(
  rrcllcoc_trch_info_type *dch_trch_info_ptr, 
  uint32  num_index
)
{
  uint8  index_for_sorting;
  uint8  to_be_compared_index;

  /* Make sure num_index doesn't read beyond the end of dch_trch_info_ptr_array */
  if (num_index > UE_MAX_TRCH) 
  {
    ERR_FATAL("num_idx %d greater than UE_MAX_TRCH %d",num_index, UE_MAX_TRCH, 0);
  }

  for(index_for_sorting = 1; index_for_sorting < num_index; index_for_sorting ++)
  {
    /* If the Transport Channel ID for the index for sort is NOT lower than the Transport
     * Channel ID for the index one lower than the one being sorted, do NOT do anything.
     * This is because all indices further below have already been sorted.
     */
    if(dch_trch_info_ptr[index_for_sorting].trch_id < 
        dch_trch_info_ptr[index_for_sorting-1].trch_id)
    {
      /* The TrCH ID for sorted index is lower than TrCH ID for index before
       * This is time to move all indices below till a space is found for 
       * data at current sorted index.
       */
      rrcllc_copy_ul_dch_trch_info_to_buffer(index_for_sorting);

      to_be_compared_index = index_for_sorting;

      while( (to_be_compared_index > 0) && 
             (ul_dch_info_buffer.local_info.trch_id < 
                dch_trch_info_ptr[to_be_compared_index-1].trch_id)
           )
      {
         rrcllc_copy_ul_dch_trch_info_from_one_to_next_index(
                                       to_be_compared_index, 
                                       to_be_compared_index-1);
        to_be_compared_index --;
      }
  
      rrcllc_copy_ul_dch_trch_info_from_buffer(to_be_compared_index);
    }
  } /* for(...) */
} /* rrcllc_sort_ul_dch_trch_index_array */


/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_ul_dch_trch_info()

DESCRIPTION:
  This function takes up deleted and added UL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_ul_dch_trch_info
(
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList  * ul_trch_add_reconfig_ptr,
  rrc_UL_DeletedTransChInfoList   * ul_trch_delete_ptr
)
{

  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  uint32 idx =0, idy=0;

  rrc_UL_DeletedTransChInfoList* temp_ul_trch_delete_ptr = ul_trch_delete_ptr;
  rrc_UL_AddReconfTransChInfoList* temp_ul_trch_add_reconfig_ptr = ul_trch_add_reconfig_ptr;

  if((temp_ul_trch_delete_ptr != NULL) && (temp_ul_trch_add_reconfig_ptr != NULL))
  {
    while(temp_ul_trch_delete_ptr->n > idx)
    {
      if(rrc_UL_TrCH_Type_dch == temp_ul_trch_delete_ptr->elem[idx].ul_TransportChannelType)
     {
       idy = 0;
        while(temp_ul_trch_add_reconfig_ptr->n > idy)
        {
          if(rrc_UL_TrCH_Type_dch == temp_ul_trch_add_reconfig_ptr->elem[idy].ul_TransportChannelType)
          {
            if(temp_ul_trch_delete_ptr->elem[idx].ul_TransportChannelIdentity == temp_ul_trch_add_reconfig_ptr->elem[idy].transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
          temp_ul_trch_add_reconfig_ptr->elem[idy].transportChannelIdentity,temp_ul_trch_delete_ptr->elem[idx].ul_TransportChannelIdentity);
              return FAILURE;
            }
          }
          idy++;
       }
     }   
      idx++;
    }
  }
  idx = 0;
  while((ul_trch_delete_ptr !=NULL )&& (ul_trch_delete_ptr->n > idx))
  {
    /* Remove this transport channel from MAC, L1 and local data.
     * and update transport channel count in L1, MAC and local data.
     */
    if(get_ul_deleted_trch(&ul_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* set ul_tfcs_needed flag to true.  Later if common info is not present,
     * UE needs to reject the message 
     */
    ul_tfcs_needed = TRUE;
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the UL Added/Reconfigured Info */
  while((ul_trch_add_reconfig_ptr != NULL ) && (ul_trch_add_reconfig_ptr->n > idx) )
  {
    trch_id = (tr_ch_id_type)ul_trch_add_reconfig_ptr->elem[idx].transportChannelIdentity;

    if((rrcllc_get_ul_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
      return(FAILURE);
    }
    idx++;
  } /* while(...) */

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->ul_trch_cnt indices.
   */
  rrcllc_fill_holes_in_ul_dch_trch_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */
  rrcllc_sort_ul_dch_trch_index_array(&ordered_config_ptr->ul_dch_trch_info[0], 
                                      (RRCLLC_UPLINK_TRCH_COUNT(ordered_config_ptr)));
  return(SUCCESS);
} /* rrcllc_add_delete_and_arrange_ul_dch_trch_info */

/*============================================================================
FUNCTION: check_if_tfi_present_in_allowed_list()

DESCRIPTION:
  This function checks if the TFI for a Trch is present
  in the allowed list list in Restricted transport channel list
  It takes a Transport channel index and  TFCI index and the 
  OTA Restricted transport channel list as   parameters.
DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS : If the TFI is present in the Allowed list  or the Trch ID
            itself was not present in the Restricted transport channel list
  else   FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type 
check_if_tfi_present_in_allowed_list
(
  rrc_RestrictedTrChInfoList * restricted_trch_tfc_ptr,
  uint32 tfci_index,
  uint32 trch_index 
  
)
{
  rrc_AllowedTFI_List * temp_tfi_ptr;
  boolean trch_id_in_restricted_list = FALSE;
  uint8 tfi;
  uint32 trch_id ;
  uint32 idx=0;
  uint32 idy =0;
  while(restricted_trch_tfc_ptr->n > idx)
  {
    if(restricted_trch_tfc_ptr->elem[idx].ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
    {
      trch_id = restricted_trch_tfc_ptr->elem[idx].restrictedTrChIdentity;
      if((ordered_config_ptr->ul_dch_trch_info[trch_index].dch_present) &&
       ( (ordered_config_ptr->ul_dch_trch_info[trch_index].trch_id) == trch_id ))
      {
        /* The Trch id is present in the Restriched Trch List in OTA */
        trch_id_in_restricted_list = TRUE;
        if(restricted_trch_tfc_ptr->elem[idx].m.allowedTFI_ListPresent)
        {
          temp_tfi_ptr = &restricted_trch_tfc_ptr->elem[idx].allowedTFI_List;
          idy =0;
          while(temp_tfi_ptr->n > idy)
          {
            /* Since the TFI is present in the allowed TFI list in OTA, return success */
            if( ordered_config_ptr->ul_tfcs.tfc_info[tfci_index].tfi[trch_index] == temp_tfi_ptr->elem[idy])
            {
              return SUCCESS;
            }
            idy++;
          }
        }
        else
        {
          /* If the Allowed List for this Trch is not present, 
            * then TB size 0 shld be treated as ALLOWED, all other TFIs should be
            * NOT ALLOWED
            */
          tfi =ordered_config_ptr->ul_tfcs.tfc_info[tfci_index].tfi[trch_index];
                
          if(ordered_config_ptr->ul_tfcs.tfs_info[trch_index].tf_info[tfi].ntblks *
            ordered_config_ptr->ul_tfcs.tfs_info[trch_index].tf_info[tfi].tb_size == 0)
          { 
           /*No allowed list, but ALLOWED TFI as TB Size =0 */
            return SUCCESS;
          } 
        }
      }
    }
    else
    {
      WRRC_MSG2_ERROR("recieved  NON DCH type %d for trch id %d ",
    restricted_trch_tfc_ptr->elem[idx].ul_TransportChannelType,
    restricted_trch_tfc_ptr->elem[idx].restrictedTrChIdentity);
    }
    if(trch_id_in_restricted_list == TRUE )
    {
      break;
    }
    idx++;
  }
  /* We return success as, the Trch ID was not present in the 
    * Restriched Trch List in OTA, so all TFI for that Trch is allowed
    */  
  if(trch_id_in_restricted_list == FALSE)
  {
    return SUCCESS;
  }
  else
  {
    /* We return FAILURE as, the Trch ID was present in the 
      * Restriched Trch List in OTA, but not in the allowed TFI list
      */  
    WRRC_MSG0_ERROR("TrCh ID present in the restricted TrCh list");
    return FAILURE;
  }
}
/*============================================================================
FUNCTION: rrcllc_update_oc_with_tfc_subset()

DESCRIPTION:
  This function takes UL common Transport channel
  List and updates OC with TFC subset restrictions.
      
DEPENDENCIES:
  This function assumes that mode specific info is of type FDD.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_with_tfc_subset
(
  void                     *ie_ptr, 
  rrc_msg_ver_enum_type     rel_ver
)
{
  rrc_TFC_Subset             *tfc_subset_ptr = NULL;
  rrc_AllowedTFC_List        * allowed_tfc_ptr = NULL;
  rrc_Non_allowedTFC_List     * non_allowed_tfc_ptr = NULL;
  rrc_RestrictedTrChInfoList  * restricted_trch_tfc_ptr = NULL;
  uint32  tr_ch_indx[UE_MAX_TRCH] = {0};
  uint32  no_of_tr_ch_ids = 0;
  uint32 allowed_tfc_number = UE_MAX_TFC + 1;
  uint32 i = 0, j = 0, k = 0;
  boolean transport_channel_id_present = FALSE;
  rrc_AllowedTFI_List * temp_tfi_ptr;
  uint32 idx=0;
  /* As the IE's of our interest are same for Rel-99 & Rel-5,
     point a local pointer to the embedded TFC subset field */

  /*Doing a check for RRC NV rel indicator here doesn't make sense. Because it should
  have been done earlier itself. But keeping it as per the legacy code.*/
  if(rel_ver == MSG_REL99)
  {
    tfc_subset_ptr = &((rrc_UL_CommonTransChInfo *)ie_ptr)->tfc_Subset;
  }
  else if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5 && rel_ver == MSG_REL5)
    ||(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6 && rel_ver == MSG_REL6)
    ||(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7 && rel_ver == MSG_REL7)
    ||(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8 && rel_ver == MSG_REL8)
#ifdef FEATURE_WCDMA_REL9
    ||(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9 && rel_ver == MSG_REL9)

#endif /* FEATURE_WCDMA_REL9 */
#ifdef FEATURE_WCDMA_REL10
    #error code not present
#endif /* FEATURE_WCDMA_REL10 */

    )
  {
    tfc_subset_ptr = &((rrc_UL_CommonTransChInfo_r4 *)ie_ptr)->tfc_Subset;
  }
  else
  {
    WRRC_MSG0_ERROR("TFC subset restriction for unsupported release ... rejecting");
    return FAILURE;
  }

  /* Apply the TFC subset restrictions on top of the existing OC's TFCS table */
  WRRC_MSG1_HIGH("TFC subset restriction type=%d TFCS list is Changed ", tfc_subset_ptr->t);

  ordered_config_ptr->mac_ul_parms.new_tfcs_list_present = TRUE;

  switch(tfc_subset_ptr->t)
  {
  case T_rrc_TFC_Subset_minimumAllowedTFC_Number:

    /* all the TFC indices below & including this index are allowed for selection */
    allowed_tfc_number = tfc_subset_ptr->u.minimumAllowedTFC_Number;

    if(allowed_tfc_number < ordered_config_ptr->ul_tfcs.ntfc)
    {
      i = 0;
      while(i <= allowed_tfc_number)
      {
        /* Not even a single TFC should have been removed earlier */
        if(ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag 
                                                                == TFC_REMOVED)
        {
          WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED");
          return FAILURE;
        }
        i++;
      }

      /* Disallow all the combinations except the REMOVED ones
         & allow only the ones given by the network */
      i = 0;
      while(i < ordered_config_ptr->ul_tfcs.ntfc)
      {
        if(ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_NOT_ALLOWED;
        }
        i++;
      }
      
      i = 0;
      while(i <= allowed_tfc_number)
      {
        ordered_config_ptr->ul_tfcs.tfc_info[i++].control_flag = TFC_ALLOWED;
      }
    }
    else
    {
      ERR("Allowed TFC number=%d exceeds UE's capability=%d"
               "/current combinations=%d", allowed_tfc_number, 
               UE_MAX_TFC, ordered_config_ptr->ul_tfcs.ntfc);
      return FAILURE;
    }
    break;
  
  case T_rrc_TFC_Subset_allowedTFC_List:
    if(tfc_subset_ptr->u.allowedTFC_List != NULL)
    {
      allowed_tfc_ptr = tfc_subset_ptr->u.allowedTFC_List;
      
      while(allowed_tfc_ptr->n >  idx)
      {
        if((allowed_tfc_ptr->elem[idx] >= ordered_config_ptr->ul_tfcs.ntfc) ||
           (ordered_config_ptr->ul_tfcs.tfc_info[allowed_tfc_ptr->elem[idx]].control_flag == TFC_REMOVED))
        {
          WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED or allowed TFC number exceeded current combinations");
          return FAILURE;
        }
        idx++;
      }

      /* Disallow all the combinations except the REMOVED ones
         & allow only the ones given by the network */
      
      i = 0;
      while(i < ordered_config_ptr->ul_tfcs.ntfc)
      {
        if(ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_NOT_ALLOWED;
        }
        i++;
      }
      idx =0;
      allowed_tfc_ptr = tfc_subset_ptr->u.allowedTFC_List;
      while(allowed_tfc_ptr->n > idx)
      {
        ordered_config_ptr->ul_tfcs.tfc_info[allowed_tfc_ptr->elem[idx]].control_flag = TFC_ALLOWED;
        idx++;
      }
    }
    break;

    case T_rrc_TFC_Subset_non_allowedTFC_List:
      if(tfc_subset_ptr->u.non_allowedTFC_List != NULL)
      {
        non_allowed_tfc_ptr = tfc_subset_ptr->u.non_allowedTFC_List;
        
        while(non_allowed_tfc_ptr->n > idx)
        {
          if((non_allowed_tfc_ptr->elem[idx] >= ordered_config_ptr->ul_tfcs.ntfc) ||
             (ordered_config_ptr->ul_tfcs.tfc_info[non_allowed_tfc_ptr->elem[idx]].control_flag == TFC_REMOVED))
          {
            WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED or allowed TFC number exceeded current combinations");
            return FAILURE;
          }
          idx++;
        }

        /* Allow all the combinations except the REMOVED ones
           & allow only the ones given by the network */

        i = 0;
        while(i < ordered_config_ptr->ul_tfcs.ntfc)
        {
          if(ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
          {
            ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_ALLOWED;
          }
          i++;
        }
        idx =0;
        non_allowed_tfc_ptr = tfc_subset_ptr->u.non_allowedTFC_List;
        while(non_allowed_tfc_ptr->n > idx)
        {
          ordered_config_ptr->ul_tfcs.tfc_info[non_allowed_tfc_ptr->elem[idx]].control_flag = TFC_NOT_ALLOWED;
          idx++;
        }
      }
      break;
    
    case T_rrc_TFC_Subset_restrictedTrChInfoList:
      if(tfc_subset_ptr->u.restrictedTrChInfoList != NULL)
      {
        restricted_trch_tfc_ptr = tfc_subset_ptr->u.restrictedTrChInfoList;    

        /* First verify whether the restricted transport channel type & 
           transport channel identity exists in OC. If not, return FAILURE */
       
        while(restricted_trch_tfc_ptr->n > idx)
        {
          if(restricted_trch_tfc_ptr->elem[idx].ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
          {
            transport_channel_id_present = FALSE;
            
            i = 0;
            while(i < UE_MAX_TRCH)
            {
              if(restricted_trch_tfc_ptr->elem[idx].restrictedTrChIdentity == ordered_config_ptr->ul_dch_trch_info[i].trch_id)
              {
                if(ordered_config_ptr->ul_dch_trch_info[i].dch_present == TRUE)
                {
                  transport_channel_id_present = TRUE;

                  /* Record the transport channel IDs restricted in "tr_ch_ids" array */                  
                  tr_ch_indx[no_of_tr_ch_ids] = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr, ordered_config_ptr->ul_dch_trch_info[i].trch_id);

                  if(tr_ch_indx[no_of_tr_ch_ids] == RRCLLC_INVALID_TRCH_IDX)
                  {
                    WRRC_MSG0_ERROR("Found invalid TrCh index .. rejecting");
                  }

                  MSG_HIGH("Index=%d, DCH Index=%d, Restricting Transport Channel ID=%d", i, ordered_config_ptr->ul_dch_trch_idx_in_use[i], tr_ch_indx[no_of_tr_ch_ids]);
                  
                  if(restricted_trch_tfc_ptr->elem[idx].m.allowedTFI_ListPresent)
                  {
                    uint32 idy =0;
                    /* Record the TFI's pointers allowed for the restricted transport channel ID */
                    temp_tfi_ptr = &restricted_trch_tfc_ptr->elem[idx].allowedTFI_List;
                    
                    while(temp_tfi_ptr->n > idy)
                    {
                      if(temp_tfi_ptr->elem[idx]>= UE_MAX_TF)
                      {
                        WRRC_MSG1_ERROR("Found TFI value exceeding UE's capability for transport channel ID=%d.. rejecting", tr_ch_indx[no_of_tr_ch_ids]);
                        return FAILURE;
                      }
                      idy++;
                    }
                  }
                  no_of_tr_ch_ids++;
                  break;
                }
              }
              i++;
            }

            if(transport_channel_id_present == FALSE)
            {
              WRRC_MSG0_ERROR("Restricted transport channel Id not present ... rejecting");
              return FAILURE;
            }
          }
          else
          {
            WRRC_MSG0_ERROR("Restricted transport channel type non-dch choosen ... rejecting");
            return FAILURE;
          }

          idx++;
        }

        /* If we made this far, there is atleast one transport channel to apply restriction on. 
           Make all TFCs as NOT ALLOWED */
        /* j indicates the tfci index */
        for(j=0;j<ordered_config_ptr->ul_tfcs.ntfc && (j < UE_MAX_TFC);j++)
        {
          boolean tfci_allowed = TRUE;
          /* k represents each Trch index */
          for(k=0;k< ordered_config_ptr->ul_tfcs.nchan && (k < UE_MAX_TRCH); k++)
          {
            if(check_if_tfi_present_in_allowed_list(restricted_trch_tfc_ptr,j,k)
                 == FAILURE)
            {
              tfci_allowed = FALSE;
              break;
            }     
          }
            
          if(ordered_config_ptr->ul_tfcs.tfc_info[j].control_flag != TFC_REMOVED)
          { 
            if(tfci_allowed == FALSE )
            {
              ordered_config_ptr->ul_tfcs.tfc_info[j].control_flag = TFC_NOT_ALLOWED;  
            }
            else
            {
              ordered_config_ptr->ul_tfcs.tfc_info[j].control_flag = TFC_ALLOWED;
            }
          }    
        }
      }
      break;

    case T_rrc_TFC_Subset_fullTFCS:

      /* Relax all the restrictions if any.
         Allow all the combinations except the REMOVED ones */

      i = 0;
      while(i < ordered_config_ptr->ul_tfcs.ntfc)
      {
        if(ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_ALLOWED;
        }
        i++;
      }
      break;
    
    default:
      /* Ignoring this t */
      WRRC_MSG0_ERROR("Unsupported .. rejecting");
      return FAILURE;
  }

  j = 0;
  while(j < ordered_config_ptr->ul_tfcs.ntfc)
  {
    if(ordered_config_ptr->ul_tfcs.tfc_info[j].control_flag == TFC_ALLOWED)
    {
      WRRC_MSG2_HIGH("Allowed TFCI=%d, CTFC=%d", j, ordered_config_ptr->ul_tfcs.tfc_info[j].ctfc);      
    }
    j++;
  }
  return SUCCESS;
} /* rrcllc_update_oc_with_tfc_subset */


/*============================================================================
FUNCTION: rrcllc_update_oc_with_tfc_subset_list()

DESCRIPTION:
  This function takes UL common Transport channel
  List and maintains TFC subset list restrictions.
      
DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_with_tfc_subset_list(void  *ie_ptr)
{
  uint32 i, j , k;
  uint32 no_of_subset_lists = 0;
  rrcllc_oc_tfc_subset_list_type *ul_tfcsubsetlist_info_ptr = NULL;
  rrc_AllowedTFC_List         *allowed_tfc_ptr = NULL;
  rrc_Non_allowedTFC_List     *non_allowed_tfc_ptr = NULL;
  rrc_RestrictedTrChInfoList  * restricted_trch_tfc_ptr = NULL;
  uint32  tr_ch_indx[UE_MAX_TRCH] = {0};
  uint32  no_of_tr_ch_ids = 0;
  uint32 allowed_tfc_number = UE_MAX_TFC + 1;  
  boolean transport_channel_id_present = FALSE;
  rrc_AllowedTFI_List * temp_tfi_ptr;
  uecomdef_status_e_type status = FAILURE;
  uint32 idx =0;
  uint32 idy =0;
  uint32 idz =0;
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(((rrc_UL_CommonTransChInfo_r4 *)ie_ptr),
   tfc_SubsetList))
  {
    if(!(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) )
    {
        WRRC_MSG1_ERROR("RRC NV %d not REL5 capable so reject TFC subset list ",rrc_nv_rel_indicator);
        return status;
    }
  }
  if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) &&     
     (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(((rrc_UL_CommonTransChInfo_r4 *)ie_ptr),
     tfc_SubsetList)))
  { 
    rrc_TFC_SubsetList * tfc_subset_list_ptr = NULL;

    uint32 no_of_subset_list_index = 0;

    WRRC_MSG0_HIGH("TFCSubset List received .. TFCS list is Changed ");
    

    memset((void *)&ordered_config_ptr->ul_tfc_subset_list, 0, sizeof(ordered_config_ptr->ul_tfc_subset_list));

    ordered_config_ptr->mac_ul_parms.new_tfcs_list_present = TRUE;
    tfc_subset_list_ptr = &((rrc_UL_CommonTransChInfo_r4 *)ie_ptr)->tfc_SubsetList;

    ul_tfcsubsetlist_info_ptr = &ordered_config_ptr->ul_tfc_subset_list;


    /* All the information is basically stored as-is. NO checks performed.
       It is upto TFCC procedure (received on RLC-TM) to validate the data & use it */

    while(tfc_subset_list_ptr->n > idx)
    {
      if(RRC_CHECK_COMMON_MSG_TYPE(tfc_subset_list_ptr->elem[idx].modeSpecificInfo,
         rrc_TransportFormatCombinationControl_modeSpecificInfo_fdd))
      {
        if(no_of_subset_list_index >= UE_MAX_TFC_SUBSET) 
        {
          WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED");
          return status;
        }

        /* Please refer rrcllcoc.h for what each field of UL TFCSubsetList database [global var: ul_tfc_subset_list] means */
        switch(tfc_subset_list_ptr->elem[idx].tfc_Subset.t)
        {
        case T_rrc_TFC_Subset_minimumAllowedTFC_Number:          

          if(tfc_subset_list_ptr->elem[idx].tfc_Subset.u.minimumAllowedTFC_Number < 
     ordered_config_ptr->ul_tfcs.ntfc)
          {
            i = 0;
            while(i <= tfc_subset_list_ptr->elem[idx].tfc_Subset.u.minimumAllowedTFC_Number)
            {
              /* Not even a single TFC should have been removed earlier */
              if(ordered_config_ptr->ul_tfcs.tfc_info[i].control_flag 
                                                                      == TFC_REMOVED)
              {
                WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED");
                return status;
              }
              i++;
            }
          }
          else
          { 
            ERR("Allowed TFC number=%d exceeds UE's capability=%d"
               "/current combinations=%d", allowed_tfc_number, 
               UE_MAX_TFC, ordered_config_ptr->ul_tfcs.ntfc);
            return status;
          }

          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].restriction_choice = MINIMUM_ALLOWED_TFC;
          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].tfc_choice_info.minimum_allowed_tfc_number
     = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.minimumAllowedTFC_Number;
          break;

        case T_rrc_TFC_Subset_allowedTFC_List:
          idy =0;
          allowed_tfc_ptr = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.allowedTFC_List;

          while(allowed_tfc_ptr->n > idy)
          {
            if((allowed_tfc_ptr->elem[idy] >= ordered_config_ptr->ul_tfcs.ntfc) ||
               (ordered_config_ptr->ul_tfcs.tfc_info[allowed_tfc_ptr->elem[idy]].control_flag == TFC_REMOVED))
            {
              WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED or allowed TFC number exceeded current combinations");
              return status;
            }
            idy++;
          }

          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].restriction_choice = ALLOWED_TFC_LIST;
          allowed_tfc_ptr = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.allowedTFC_List;

          i = 0;
    idy =0;
          while(allowed_tfc_ptr->n > idy)
          {
            if(i < UE_MAX_TFC)
            {
              ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
      tfc_choice_info.allowed_tfc_list.allowed_tfc_number[i++] = 
      (uint16)allowed_tfc_ptr->elem[idy];
            }

            ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
    tfc_choice_info.allowed_tfc_list.total_allowed_tfc_count++;

            idy++;
          }
          break;

        case T_rrc_TFC_Subset_non_allowedTFC_List:
          idy =0;
          non_allowed_tfc_ptr = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.non_allowedTFC_List;
          while(non_allowed_tfc_ptr->n > idy)
          {
            if((non_allowed_tfc_ptr->elem[idy] >= ordered_config_ptr->ul_tfcs.ntfc) ||
               (ordered_config_ptr->ul_tfcs.tfc_info[non_allowed_tfc_ptr->elem[idy]].control_flag == TFC_REMOVED))
            {
              WRRC_MSG0_ERROR("Found a TFC which is set to TFC_REMOVED or allowed TFC number exceeded current combinations");
              return status;
            }
            idy++;
          }
        
          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].restriction_choice = NON_ALLOWED_TFC_LIST;
          non_allowed_tfc_ptr = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.non_allowedTFC_List;

    idy =0;
          i = 0;
          while(non_allowed_tfc_ptr->n > idy)
          {
            if(i < UE_MAX_TFC)
            {
              ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
      tfc_choice_info.non_allowed_tfc_list.non_allowed_tfc_number[i++]= 
      (uint16)non_allowed_tfc_ptr->elem[idy];
            }

            ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
    tfc_choice_info.non_allowed_tfc_list.total_non_allowed_tfc_count++;

            idy++;
          }
          break;

        case T_rrc_TFC_Subset_restrictedTrChInfoList:
    idy =0;

          restricted_trch_tfc_ptr = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.restrictedTrChInfoList;
          while(restricted_trch_tfc_ptr->n > idy)
          {
            transport_channel_id_present = FALSE;
            
            i = 0;
            while(i < UE_MAX_TRCH)
            {
              if(restricted_trch_tfc_ptr->elem[idy].restrictedTrChIdentity == ordered_config_ptr->ul_dch_trch_info[i].trch_id)
              {
                if(ordered_config_ptr->ul_dch_trch_info[i].dch_present == TRUE)
                {
                  transport_channel_id_present = TRUE;

                  if(rrcllc_find_ul_dch_trch_idx(ordered_config_ptr, 
       ordered_config_ptr->ul_dch_trch_info[i].trch_id) == RRCLLC_INVALID_TRCH_IDX)
                  {
                    WRRC_MSG0_ERROR("Found invalid TrCh index .. rejecting");
                    return status;
                  }

                  MSG_HIGH("Index=%d, DCH Index=%d, Restricting Transport Channel ID=%d", i, ordered_config_ptr->ul_dch_trch_idx_in_use[i], tr_ch_indx[no_of_tr_ch_ids]);
                  
                  if(restricted_trch_tfc_ptr->elem[idy].m.allowedTFI_ListPresent)
                  {
                    /* Record the TFI's pointers allowed for the restricted transport channel ID */
                    temp_tfi_ptr = &restricted_trch_tfc_ptr->elem[idy].allowedTFI_List;
                    idz =0;
                    while(temp_tfi_ptr->n > idz)
                    {
                      if(temp_tfi_ptr->elem[idz]>= UE_MAX_TF)
                      {
                        WRRC_MSG1_ERROR("Found TFI value exceeding UE's capability for transport channel ID=%d.. rejecting",
       tr_ch_indx[no_of_tr_ch_ids]);
                        return status;
                      }
                      idz++;
                    }
                  }
                  break;
                }
              }
              i++;
            }

            if(transport_channel_id_present == FALSE)
            {
              WRRC_MSG0_ERROR("Restricted transport channel Id not present ... rejecting");
              return status;
            }
            idy++;
          }

          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].restriction_choice = RESTRICTED_TRCH_LIST;
          restricted_trch_tfc_ptr = tfc_subset_list_ptr->elem[idx].tfc_Subset.u.restrictedTrChInfoList;

          i = 0;
    idy =0;
          while(restricted_trch_tfc_ptr->n > idy)
          {
            if(i < UE_MAX_TRCH)
            {
              if(restricted_trch_tfc_ptr->elem[idy].ul_TransportChannelType == rrc_UL_TrCH_Type_dch)
              {
                ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
     tfc_choice_info.restricted_trch_list.trch_info[i].dch_present = TRUE;
              }

              ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
      tfc_choice_info.restricted_trch_list.trch_info[i].trch_id = 
      restricted_trch_tfc_ptr->elem[idy].restrictedTrChIdentity;

              if(restricted_trch_tfc_ptr->elem[idy].m.allowedTFI_ListPresent)
              {
                rrc_AllowedTFI_List * allowed_tfi_list_ptr = NULL;
                uint32              m;

                ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].tfc_choice_info.restricted_trch_list.trch_info[i].allowed_tfi_info_present = TRUE;

                allowed_tfi_list_ptr = &restricted_trch_tfc_ptr->elem[idy].allowedTFI_List;
    idz =0;

                m = 0;
                while(allowed_tfi_list_ptr->n > idz)
                {
                  if(m < UE_MAX_TF)
                  {
                    ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
      tfc_choice_info.restricted_trch_list.trch_info[i].allowed_tfi_info.tfi[m++] = 
      (uint8)allowed_tfi_list_ptr->elem[idz];
                  }

                  ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
       tfc_choice_info.restricted_trch_list.trch_info[i].allowed_tfi_info.no_of_allowed_tfis++;

                  idz++;
                }
              }

              ++i;
            }
            ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].tfc_choice_info.restricted_trch_list.no_of_transport_channels++;

            idy++;
          }
          break;

        case T_rrc_TFC_Subset_fullTFCS:
          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
     restriction_choice = TFCS_FULL;
          break;

        default:
          ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_list_index].
     restriction_choice = TFC_CHOICE_NOT_VALID;
          break;
        } /* end of tfcsubset switch */

         no_of_subset_list_index++;

        idx++;
      }
      else
      {
        /* In case one of the sequences in the middle TFC subset list has TDD option, we may end up
           with inconsistent TFCSubsetlist database. So trash the message */

        WRRC_MSG0_ERROR("TFCSubset List received for non-FDD mode .. rejecting ");
        return status;
      }
    }

    ul_tfcsubsetlist_info_ptr = &ordered_config_ptr->ul_tfc_subset_list;  ul_tfcsubsetlist_info_ptr->no_of_subset_lists = no_of_subset_list_index;
    no_of_subset_lists = ul_tfcsubsetlist_info_ptr->no_of_subset_lists;
    while(no_of_subset_lists)
    {
      WRRC_MSG2_HIGH("Restriction choice at index %d is: %d", (no_of_subset_lists-1), ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].restriction_choice);

      switch(ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].restriction_choice)
      {
      case MINIMUM_ALLOWED_TFC:
        WRRC_MSG1_HIGH("Min. allowed:%d", ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.minimum_allowed_tfc_number);
        break;

      case ALLOWED_TFC_LIST:
        i = 0; 
        WRRC_MSG0_HIGH("Allowed TFC list is:");
        while(i < ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.allowed_tfc_list.total_allowed_tfc_count)
        {
          WRRC_MSG1_HIGH("%d", ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.allowed_tfc_list.allowed_tfc_number[i]);
          i++;
        }
        break;

      case NON_ALLOWED_TFC_LIST:
        i = 0; 
        WRRC_MSG0_HIGH("Non-allowed TFC list is:");
        while(i < ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.non_allowed_tfc_list.total_non_allowed_tfc_count)
        {
          WRRC_MSG1_HIGH("%d", ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.non_allowed_tfc_list.non_allowed_tfc_number[i]);
          i++;
        }
        break;

      case RESTRICTED_TRCH_LIST:
        i = ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.restricted_trch_list.no_of_transport_channels;
        if(i)
        {
          WRRC_MSG1_HIGH("No. of restricted transport channels are:%d Restricted transport channel IDs are/is", i);
          j = 0;
          while((j < i) && (j < UE_MAX_TRCH ))
          {
            WRRC_MSG1_HIGH("%d", ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.restricted_trch_list.trch_info[j].trch_id);
            if(ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.restricted_trch_list.trch_info[j].allowed_tfi_info_present)
            {
              k = 0;

              WRRC_MSG0_HIGH("Allowed TFIs for this Tr Channel are:");

              while(k < ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.restricted_trch_list.trch_info[j].allowed_tfi_info.no_of_allowed_tfis)
              {
                WRRC_MSG1_HIGH("%d", ul_tfcsubsetlist_info_ptr->tfc_subset_info[no_of_subset_lists-1].tfc_choice_info.restricted_trch_list.trch_info[j].allowed_tfi_info.tfi[k]);
                k++;
              }
            }
            j++;
          }
        }
        break;

      case TFCS_FULL:
        WRRC_MSG0_HIGH("All restrictions will be lifted for this Subset Id");
        break;

      default:
        WRRC_MSG0_ERROR("Invalid restriction type received");
      }

      no_of_subset_lists--;
    }
  }
   
  return SUCCESS;
} /* rrcllc_update_oc_with_tfc_subset_list */

/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_dl_dch_trch_info2()

DESCRIPTION:
  This function takes up deleted and added DL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
  NOTE: This function is similar to 
  rrcllc_add_delete_and_arrange_dl_dch_trch_info except that this takes
  rrc_DL_AddReconfTransChInfo2List as a parameter as opposed to
  rrc_DL_AddReconfTransChInfoList. There are subtle differences between
  rrc_DL_AddReconfTransChInfo2List and rrc_DL_AddReconfTransChInfoList.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_dl_dch_trch_info2_r5
(
  /* local DL TrCH Add Reconfig pointer */
  rrc_DL_AddReconfTransChInfoList_r4 * dl_trch_add_reconfig2_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList    * dl_trch_delete_ptr
)
{
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  uint32 idx =0, idy=0;
  rrc_DL_DeletedTransChInfoList *temp_dl_trch_delete_ptr = dl_trch_delete_ptr;
  rrc_DL_AddReconfTransChInfoList_r4  *temp_dl_trch_add_reconfig_ptr = dl_trch_add_reconfig2_ptr;

  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    while(temp_dl_trch_delete_ptr->n > idx)
    {
      if(rrc_DL_TrCH_Type_dch == temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType)
      {
        idy = 0;
        while(temp_dl_trch_add_reconfig_ptr->n > idy)
        {
          if(rrc_DL_TrCH_Type_dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType)
          {
            if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelIdentity == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
         temp_dl_trch_add_reconfig_ptr->elem[idy].dl_transportChannelIdentity,temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelIdentity);
              return FAILURE;
            }
          }
          idy++;
       }
      }     
      idx++;
    }
  }
  idx = 0;
  while((dl_trch_delete_ptr !=NULL) && (dl_trch_delete_ptr->n > idx))
  {
    /* Remove this transport channel from MAC, L1 and local data.
     * and update transport channel count in L1, MAC and local data.
     */
    if(get_dl_deleted_trch(&dl_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* set dl_tfcs_needed flag to true.  Later if common info is not preset,
     * UE needs to reject the message 
     */
    dl_tfcs_needed = TRUE;
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the DL Added/Reconfigured Info */
  while((dl_trch_add_reconfig2_ptr!=NULL) && (dl_trch_add_reconfig2_ptr->n > idx) )
  {
    trch_id = (tr_ch_id_type)dl_trch_add_reconfig2_ptr->elem[idx].dl_transportChannelIdentity;

    if((rrcllc_get_dl_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
      return(FAILURE);
    }
    idx++;
  } /* while(...) */

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->dl_trch_cnt indices.
   */
  rrcllc_fill_holes_in_dl_dch_trch_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */

  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                      ordered_config_ptr->mac_dl_parms.num_dchs
                                      );
  return(SUCCESS);
} /* rrcllc_add_delete_and_arrange_dl_dch_trch_info2_r5 */

/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_dl_dch_trch_info_r5()

DESCRIPTION:
  This function takes up deleted and added DL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs. For HSDSCH, it adds, deletes the DFLOW and rearranges
  the DFLOW so that missing holes because of deleted DFLOWs are
  removed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_dl_dch_trch_info_r5
(
  /* local DL TrCH Add Reconfig pointer */
  rrc_DL_AddReconfTransChInfoList_r5  * dl_trch_add_reconfig_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList_r5    * dl_trch_delete_ptr
)
{
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;

  uint8 dflow_index;
  rrc_MAC_hs_AddReconfQueue_List* dflow_queue_head_ptr = NULL;
  uint32 idx =0,idy =0;

  rrc_DL_DeletedTransChInfoList_r5 *temp_dl_trch_delete_ptr = dl_trch_delete_ptr;
  rrc_DL_AddReconfTransChInfoList_r5  *temp_dl_trch_add_reconfig_ptr = dl_trch_add_reconfig_ptr;

  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    while(temp_dl_trch_delete_ptr->n > idx)
    {
      if(T_rrc_DL_TrCH_TypeId2_r5_dch == temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.t)
      {
        idy = 0;
        while(temp_dl_trch_add_reconfig_ptr->n > idy)
        {
          if(T_rrc_DL_TrCH_TypeId1_r5_dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.t)
          {
            if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.u.dch)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
         temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.u.dch,temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.dch);
              return FAILURE;
            }
          }
          idy++;
       }
      }     
      idx++;
    }
  }
  idx = 0;
  idy = 0;
  while((dl_trch_delete_ptr != NULL) && (dl_trch_delete_ptr->n > idx))
  {
  /* Remove this transport channel from MAC, L1 and local data.
  * and update transport channel count in L1, MAC and local data.
    */
    if(get_dl_deleted_trch_r5(&dl_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the DL Added/Reconfigured Info */
  while((dl_trch_add_reconfig_ptr !=NULL) && (dl_trch_add_reconfig_ptr->n > idx) )
  {
    /* Check if channel type is HSDPA */
    if (RRC_CHECK_COMMON_MSG_TYPE(dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType, 
      rrc_DL_TrCH_TypeId1_r5_hsdsch))
    {
      /* At least one HARQ/DFLOW has to be present */
      if ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.t 
    == T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_hsdsch)
        && ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->m.addOrReconfMAC_dFlowPresent)
        || (dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->m.harqInfoPresent)))
      {
        MSG_LOW("RRCHS:Valid DFLOW specified",0,0,0);
      }
      else
      {
        WRRC_MSG0_ERROR("RRCHS:HARQ,DFLOW both ABSent ");
        return FAILURE;
      }
      
      /* Now find the DFLOW Index for ADD/RECONFIG.  */
      if ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.m.
             mac_hs_AddReconfQueue_ListPresent) &&
        (dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_AddReconfQueue_List.n > 0))
      {
        dflow_queue_head_ptr = 
          &dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
          addOrReconfMAC_dFlow.mac_hs_AddReconfQueue_List;
        idy=0;
        while (dflow_queue_head_ptr->n  > idy)
        {
          if ((dflow_queue_head_ptr->elem[idy].mac_dFlowId > UE_MAX_MAC_D_FLOW-1) ||
              (dflow_queue_head_ptr->elem[idy].mac_hsQueueId > UE_MAX_HS_QUEUE-1))
          {
            WRRC_MSG2_ERROR("RRCHS:DFLOW Id %d, Queue Id %d, Range 0..7", 
    dflow_queue_head_ptr->elem[idy].mac_dFlowId,
                dflow_queue_head_ptr->elem[idy].mac_hsQueueId);
            return FAILURE;
          }

          /* If DFLOW Index found then return it Else allocate a new index */
          if ((dflow_index = rrcllc_get_dflow_index(
     (uint8)dflow_queue_head_ptr->elem[idy].mac_dFlowId)) 
            == UE_MAX_MAC_D_FLOW)
          {
            WRRC_MSG1_HIGH("RRCHS:DFLOW index %d absent",dflow_index);
            return FAILURE;
          }
          else
          {
            idy++;
          }
        } /* End of while */       
        
      } /* End of if of add queue present */
    } /* End of if of rrc_DL_TrCH_TypeId1_r5_hsdsch chosen */
    else if (dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.t == 
      T_rrc_DL_TrCH_TypeId1_r5_dch)
    {
      trch_id = (tr_ch_id_type)dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.u.dch;
      
      if((rrcllc_get_dl_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
      {
        WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
        return(FAILURE);
      }
    }
    else
    {
      WRRC_MSG0_ERROR("DSCH unsupported");
      return FAILURE;
    }
    idx++;
  } /* while(...) */
  
    /* At this point, the transport channel list -
     *  - may have holes due to deleted channels.
     *  - transport channel info may not be sorted by TrCH IDs.
     */ 
  
  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->dl_trch_cnt indices.
   */

  /* Now Remove holes in DFLOW Index */
  rrcllc_rearrange_dflow_index();
  
  rrcllc_fill_holes_in_dl_dch_trch_idx();
  
  /* Now is the time and place for sorting all transport channel
  * info based on TrCH IDs. Call the right function.
  */

  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                     ordered_config_ptr->mac_dl_parms.num_dchs 
                                      );
  
  
  return(SUCCESS);
} /* rrcllc_add_delete_and_arrange_dl_dch_trch_info_r5 */


/*============================================================================
FUNCTION: set_harq_information()

DESCRIPTION:
  This function sets the HARQ information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type set_harq_information
(
  ordered_config_type *config_ptr,
  rrc_HARQ_Info* harq_info_ptr
)
{
  /* Set HARQ Information */
  rrc_HARQ_Info_explicit_* harq_head_ptr = NULL;
  uint32 idx =0;
  uint8 cnt = 0;
  
  
  config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.num_proc = (uint8)harq_info_ptr->numberOfProcesses;
  
  MSG_MED("RRCHS:No of processes %d",harq_info_ptr->numberOfProcesses,0,0); 
  if (RRC_CHECK_COMMON_MSG_TYPE(harq_info_ptr->memoryPartitioning,
   rrc_HARQ_Info_memoryPartitioning_implicit))
  {
    config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_part_info_valid = FALSE;
    MSG_MED("RRCHS:Implicit Mem Partition",0,0,0); 
  }
  else
  {
    config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_part_info_valid = TRUE;
    
    MSG_MED("RRCHS:Explicit Mem Partition",0,0,0); 
    
    if ((harq_head_ptr = harq_info_ptr->memoryPartitioning.u.explicit_) == NULL)
    {
      WRRC_MSG0_ERROR("RRCHS:HARQ Mem Part Ptr NULL");
      return FAILURE;
    }
    while ((harq_head_ptr->n > idx) && (cnt < L1_DL_HS_DSCH_HARQ_MAX_PROC_NO_MIMO))
    {
      config_ptr->l1_hsdpa_info.hs_dsch_info.harq_info.mem_size_type[cnt++] = 
        (l1_dl_hs_dsch_harq_mem_sz_enum_type) harq_head_ptr->elem[idx];
      
      idx++;
      
    }
    
    WRRC_MSG1_HIGH("RRCHS:No of HARQ mem sizes %d",cnt);
  }
  
  return SUCCESS;

}


/*============================================================================
FUNCTION: rrcllc_delete_queue_from_dflow()

DESCRIPTION:
  This function deletes the queue from a DFLOW. Once the queue has been
  deleted, a hole has been created which would be filled later by another
  function
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrcllc_delete_queue_from_dflow
(
  uint32 deleted_queue_id
)
{
  uint32 cnt = 0, queue_cnt = 0;

  boolean deleted_queue_found = FALSE;

  for (cnt = 0; cnt < ordered_config_ptr->mac_dl_parms.num_dflow; cnt++)
  {
    if ((ordered_config_ptr->dl_dflow_index_in_use[cnt] == TRUE) && 
      (ordered_config_ptr->mac_dflow_info[cnt].mac_dflow_id != INVALID_DFLOW_ID))
    {
      /* Check if it has queue to be deleted */
      for (queue_cnt = 0; queue_cnt < ordered_config_ptr->mac_dflow_info[cnt].no_of_queues; queue_cnt++)
      {
        if (ordered_config_ptr->mac_dflow_info[cnt].queue_info[queue_cnt].queue_id == deleted_queue_id)
        {

          ordered_config_ptr->mac_dflow_info[cnt].queue_info[queue_cnt].queue_id = INVALID_QUEUE_ID;
          
          ordered_config_ptr->mac_dflow_info[cnt].queue_info[queue_cnt].no_of_pdu_sizes = 0;

          /* Update the queue count */
          ordered_config_ptr->mac_dflow_info[cnt].no_of_queues--;

          MSG_HIGH("RRCHS :Queue %d del from Dflow %d No. of queues after deleting",  deleted_queue_id,
                   ordered_config_ptr->mac_dflow_info[cnt].mac_dflow_id,
            ordered_config_ptr->mac_dflow_info[cnt].no_of_queues);

          deleted_queue_found = TRUE;
        
          break;
        }
      } /* End of for of no_of_queues */
    } /* End of if of dflow found */
    if (deleted_queue_found == TRUE)
    {
      return SUCCESS;
    }
  } /* End of for of no of dflow */
  
  WRRC_MSG1_ERROR("RRCHS:No deleted queue %d found",deleted_queue_id);
  return FAILURE;
}


/*====================================================================
FUNCTION: get_hs_window_size()

DESCRIPTION:
  
DEPENDENCIES:
  Gets the window size associated with the queue.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
uint8 get_hs_window_size
(
  rrc_MAC_hs_WindowSize mac_hsWindowSize
)
{
  uint8 w_size;

  switch (mac_hsWindowSize)
  {
    case rrc_MAC_hs_WindowSize_mws4 : 
      w_size = 4;
      break;
      
    case rrc_MAC_hs_WindowSize_mws6 :
      w_size = 6;
      break;
      
    case rrc_MAC_hs_WindowSize_mws8:
      w_size = 8;
      break;
      
    case rrc_MAC_hs_WindowSize_mws12:
      w_size = 12;
      break;
      
    case rrc_MAC_hs_WindowSize_mws16:
      w_size = 16;
      break;
      
    case rrc_MAC_hs_WindowSize_mws24:
      w_size = 24;
      break;
      
    case rrc_MAC_hs_WindowSize_mws32:
      w_size = 32;
      break;
      
    default: /* Not Possible */
      w_size = 0;
      break;
      
  }
  MSG_MED("RRCHS:WindowSize %d",w_size,0,0);
  return w_size;
}

/*====================================================================
FUNCTION: get_hs_release_timer()

DESCRIPTION:
  
DEPENDENCIES:
  Gets the Release Timer associated with the queue.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
uint16 get_hs_release_timer
(
  rrc_T1_ReleaseTimer reorderingReleaseTimer
)
{
  uint16 release_timer;

  switch (reorderingReleaseTimer)
  {
    case rrc_T1_ReleaseTimer_rt10 : 
      release_timer = 10;
      break;
      
    case rrc_T1_ReleaseTimer_rt20 :
      release_timer = 20;
      break;
      
    case rrc_T1_ReleaseTimer_rt30:
      release_timer = 30;
      break;
      
    case rrc_T1_ReleaseTimer_rt40:
      release_timer = 40;
      break;
      
    case rrc_T1_ReleaseTimer_rt50:
      release_timer = 50;
      break;
      
    case rrc_T1_ReleaseTimer_rt60:
      release_timer = 60;
      break;
      
    case rrc_T1_ReleaseTimer_rt70:
      release_timer = 70;
      break;
      
    case rrc_T1_ReleaseTimer_rt80:
      release_timer = 80;
      break;
      
    case rrc_T1_ReleaseTimer_rt90:
      release_timer = 90;
      break;
      
    case rrc_T1_ReleaseTimer_rt100:
      release_timer = 100;
      break;
      
    case rrc_T1_ReleaseTimer_rt120:
      release_timer = 120;
      break;
      
    case rrc_T1_ReleaseTimer_rt140:
      release_timer = 140;
      break;
      
    case rrc_T1_ReleaseTimer_rt160:
      release_timer = 160;
      break;
      
    case rrc_T1_ReleaseTimer_rt200:
      release_timer = 200;
      break;
      
    case rrc_T1_ReleaseTimer_rt300:
      release_timer = 300;
      break;
      
    case rrc_T1_ReleaseTimer_rt400:
      release_timer = 400;
      break;    
      
    default: /* Not Possible */
      release_timer = 0;
      break;
      
  }
  MSG_MED("RRCHS:ReleaseTimer %d",release_timer,0,0);
  return release_timer;
}

/*====================================================================
FUNCTION: set_queue_info()

DESCRIPTION:
  
DEPENDENCIES:
  Sets the Release Timer, PDU Size, Release Timer, Window Size associated with this queue
  in MAC structure. The index passed is the new DFLOW index. The mapping between old DFLOW 
  index and this queue is removed.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
static uecomdef_status_e_type set_queue_info
(
  uint8  dflow_index,
  uint32 queue_index,
  rrc_MAC_hs_AddReconfQueue* rrc_queue_info_ptr,
  boolean queue_already_present
)
{
  /* Flow Id and Queue Id has already been stored */
  
  rrc_MAC_d_PDU_SizeInfo_List* pdu_list_ptr = NULL;
  
  uint8 pdu_size_count = 0, cnt = 0;
  uint32 idx =0;
  boolean pdu_index_present = FALSE;
  
  /* Check if the queue is already present. In this case don't look at queue index */

  MSG_HIGH("RRCHS:DFLOWId %d, Queue %d, Exists already[Yes/No] %d", 
        rrc_queue_info_ptr->mac_dFlowId,
        rrc_queue_info_ptr->mac_hsQueueId,
        queue_already_present);

  if (queue_already_present)
  {
    
    /* Check if PDU SIZE mapping provided */
    if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_queue_info_ptr,mac_d_PDU_SizeInfo_List)) && 
      (rrc_queue_info_ptr->mac_d_PDU_SizeInfo_List.n >0))
    {
      pdu_list_ptr = &rrc_queue_info_ptr->mac_d_PDU_SizeInfo_List;
      
      while ((pdu_list_ptr->n >idx) && (pdu_size_count < UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE)) 
      {
        /* Check if pdu size index exists. If yes, overwrite the pdu size in that index else store size in new index */
        for (pdu_size_count=0; pdu_size_count < 
          ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes;
        pdu_size_count++)
        {
          if (ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[pdu_size_count].mac_pdu_index
            == pdu_list_ptr->elem[idx].mac_d_PDU_Index)
          {
            MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHS:PDU Index %d existing in queue %d, Overwrite PDUSize %d with %d,PduIdx %d",
              pdu_list_ptr->elem[idx].mac_d_PDU_Index,
              rrc_queue_info_ptr->mac_hsQueueId,
              ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[pdu_size_count].mac_pdu_size, 
              pdu_list_ptr->elem[idx].mac_d_PDU_Size,
              pdu_list_ptr->elem[idx].mac_d_PDU_Index);
            
            ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[pdu_size_count].mac_pdu_size = 
              (uint16) pdu_list_ptr->elem[idx].mac_d_PDU_Size;
            
            pdu_index_present = TRUE;
          }
        }
        
        /* Now check if pdu index has already been ovewritten */
        if (pdu_index_present == FALSE)
        {
          if (ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes < 
              (UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE-1))
          {
            cnt = ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes;

            ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[cnt].mac_pdu_index = 
              (uint8) pdu_list_ptr->elem[idx].mac_d_PDU_Index;
            
            ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[cnt].mac_pdu_size = 
              (uint16) pdu_list_ptr->elem[idx].mac_d_PDU_Size;
            
            ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes++;
            
            MSG_HIGH("RRCHS:PDU size space available Set PDU index %d, Size %d No of PDU Sizes %d",
                       pdu_list_ptr->elem[idx].mac_d_PDU_Index,
                       pdu_list_ptr->elem[idx].mac_d_PDU_Size,
                     ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes);
          }
          else
          {
            WRRC_MSG1_ERROR("RRCHS:No of PDU sizes %d (MAX 8)",
              ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes);
            return FAILURE;
          }
        }
        else
        {
          /* Set index to FALSE forcibly */
          pdu_index_present = FALSE;
        }
        
        /* The PDU size has already been written */
        /* Get the next pdu size */
        idx++;
      } /* End of while */
    } /* End of if of PDU SIZE Info present */

    /* Set Window Size & Release Timer */
    ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].window_size = 
    
     get_hs_window_size(rrc_queue_info_ptr->mac_hsWindowSize);

    ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].release_timer = 
    
     get_hs_release_timer(rrc_queue_info_ptr->reorderingReleaseTimer);

    WRRC_MSG2_HIGH("rrchs:WindowSize %d, RelTimer %d ms", 
      ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].window_size,
      ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].release_timer);

  } /* End of if of queue already present */
  else
  {
    /* Queue not present. Queue Index has already been allocated. Set PDU info, Release Timer etc */
    
    pdu_list_ptr = &rrc_queue_info_ptr->mac_d_PDU_SizeInfo_List;
    
    while ((pdu_list_ptr->n > idx) && (pdu_size_count < UE_MAX_HS_NO_OF_PDU_SIZE_PER_QUEUE)) 
    {
      MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHS:DFLOWId %d, PDUIndex %d, Size %d set in Queue %d", 
        rrc_queue_info_ptr->mac_dFlowId,
        pdu_list_ptr->elem[idx].mac_d_PDU_Index,
        pdu_list_ptr->elem[idx].mac_d_PDU_Size,
        rrc_queue_info_ptr->mac_hsQueueId);
      
      ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[pdu_size_count].mac_pdu_size = 
        (uint16) pdu_list_ptr->elem[idx].mac_d_PDU_Size;
      
      ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].pdu_info[pdu_size_count].mac_pdu_index = 
        (uint8) pdu_list_ptr->elem[idx].mac_d_PDU_Index;
      
      pdu_size_count++;
      
      idx++;
    } /* End of while of pdu_list_ptr */
    
    ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes = pdu_size_count;
    
    ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].window_size = 

     get_hs_window_size(rrc_queue_info_ptr->mac_hsWindowSize);
    ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].release_timer = 

     get_hs_release_timer(rrc_queue_info_ptr->reorderingReleaseTimer);
    
    MSG_HIGH("RRCHS:No PduSizes %d, Queue %d No of queues are %d", pdu_size_count, rrc_queue_info_ptr->mac_hsQueueId, ordered_config_ptr->mac_dflow_info[dflow_index].no_of_queues);
  } /* End of else of queue not present */
  
  return SUCCESS;
} /* set_queue_info */
/*============================================================================
FUNCTION: set_dflow_info()

DESCRIPTION:
  This function sets the DFLOW information information in MAC structure in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type set_dflow_info(rrc_AddOrReconfMAC_dFlow * rrc_hsdsch)
{
    uint8 dflow_index;
  
  uint8 queue_index;
  
  boolean queue_present = FALSE;
  uint32 idx =0;
  rrc_MAC_hs_AddReconfQueue_List* queue_dflow_head_ptr = NULL;
  
  rrc_MAC_hs_DelQueue_List * deleted_queue_list_ptr = NULL;
  
  /* Get the deleted Queue Information */
  if ( (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_hsdsch,
   mac_hs_DelQueue_List)) &&
    (rrc_hsdsch->mac_hs_DelQueue_List.n > 0))
  {
    
    deleted_queue_list_ptr = &rrc_hsdsch->mac_hs_DelQueue_List;
    
    while (deleted_queue_list_ptr->n > idx)
    {

      /* Check which queue is being deleted and find dflow associated with it */
      if (rrcllc_delete_queue_from_dflow(deleted_queue_list_ptr->elem[idx].mac_hsQueueId) == SUCCESS)
      {
        idx++;
      }
      else
      {
        WRRC_MSG1_ERROR("RRCHS:Delete queue %d not found",deleted_queue_list_ptr->elem[idx].mac_hsQueueId);
        return FAILURE;
      }
    }
    /* Now rearrange queues again so as to fill out the holes */
    rrcllc_rearrange_queue_index();
  }

  /* Get the add/reconfig MAC D FLOW Info */
  if ( !((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_hsdsch,mac_hs_AddReconfQueue_List)) &&
	(rrc_hsdsch->mac_hs_AddReconfQueue_List.n > 0)))
  {
    return SUCCESS;
  }
  /* Parse the Queue associated to a DFLOW */
  queue_dflow_head_ptr = &rrc_hsdsch->mac_hs_AddReconfQueue_List;
  idx=0;
  /* DFLOW ID is already present in MAC structure & RRC structure */
  while (queue_dflow_head_ptr->n > idx)
  {
    /* Get the dflow index which has already been created */
    if ((dflow_index = rrcllc_find_dflow_index((uint8)queue_dflow_head_ptr->elem[idx].mac_dFlowId)) == UE_MAX_MAC_D_FLOW)
    {
      WRRC_MSG1_ERROR("RRCHS:Index not found for flow Id %d", queue_dflow_head_ptr->elem[idx].mac_dFlowId);
      return FAILURE;
    }
    if(rrcllc_restore_macd_rb_mapping_info(dflow_index) ==FAILURE)
    {
      /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
      WRRC_MSG1_ERROR("RRCHS:Index not correct for flow Id %d in Standalone Rb-mapping backup", queue_dflow_head_ptr->elem[idx].mac_dFlowId);
    }

    /* Now get the queue index as this queue may already be available. If not available, then store
    allocate a new queue index and store the index in it. Just store this queue and dflow id */

    if ((queue_index = rrcllc_get_queue_index(dflow_index, queue_dflow_head_ptr->elem[idx].mac_hsQueueId, &queue_present)) == UE_MAX_HS_QUEUE)
    {
      WRRC_MSG1_ERROR("RRCHS:Unable to process Queue with ID %d",queue_dflow_head_ptr->elem[idx].mac_hsQueueId);
      return FAILURE;
    }
    
    /* Set queue information in queue index */
    if (set_queue_info(dflow_index, queue_index, &queue_dflow_head_ptr->elem[idx], queue_present) == FAILURE)
    {
      WRRC_MSG0_ERROR("RRCHS:Invalid queue parameters");
      return FAILURE;
    }

    /* At this time, the queue and dflow have already been stored */
    /* Now check if this queue is available in some other dflow. If yes, then that old dflow association
      needs to be removed */

    if (queue_present == TRUE)
    {
      rrcllc_remove_dflow_queue_mapping(&queue_dflow_head_ptr->elem[idx]);
    }

    /* Get the next queue */
    idx++;
    
  }
  return SUCCESS;
}
/*====================================================================
FUNCTION: rrcllc_add_macehs()

DESCRIPTION:
  
DEPENDENCIES:


RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
static uecomdef_status_e_type rrcllc_add_macehs
(
 rrc_AddOrReconfMAC_ehs_ReordQ * mac_ehs_info
)
{
  rrc_MAC_ehs_AddReconfReordQ_List * macehs_queue_head_ptr;
  uint8 macehs_q_id;
  uint32 idx =0;
    /* Now find the MAC EHS Index for ADD/RECONFIG.  */
  if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mac_ehs_info,mac_ehs_AddReconfQueue_List))
     &&(mac_ehs_info->mac_ehs_AddReconfQueue_List.n > 0))
  {
    macehs_queue_head_ptr = &mac_ehs_info->mac_ehs_AddReconfQueue_List;
    while (macehs_queue_head_ptr->n > idx)
    {
      if (macehs_queue_head_ptr->elem[idx].mac_ehs_QueueId > UE_MAX_HS_QUEUE-1)
      {
        WRRC_MSG1_ERROR("MACEHS:MAC EHS Q Id %d, Range 0..7",
            macehs_queue_head_ptr->elem[idx].mac_ehs_QueueId);
        return FAILURE;
      }

      /* If Queue Index found then return it Else allocate a new index */
      if ((macehs_q_id = rrcllc_get_macehs_queue_index((uint8)macehs_queue_head_ptr->elem[idx].mac_ehs_QueueId)) 
        == UE_MAX_HS_QUEUE)
      {
        return FAILURE;
      }
      else
      {
      
        /* End of while of pdu_list_ptr */            
        ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].window_size = 
          get_hs_window_size(macehs_queue_head_ptr->elem[idx].mac_ehsWindowSize);
        
        ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].release_timer = 
          get_hs_release_timer(macehs_queue_head_ptr->elem[idx].reorderingReleaseTimer);
           /*PSR TBD  reorderingResetTimer   in 7.6       */
      }
#ifdef FEATURE_WCDMA_HS_FACH
        if(macehs_queue_head_ptr->elem[idx].m.reorderingResetTimerPresent)
        {
          ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].reset_timer = 
            macehs_queue_head_ptr->elem[idx].reorderingResetTimer + 1;
        }
        else
        {
          /*Default value is 2*/
          ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].reset_timer = 2;
        }
#endif 

        idx++;
    } /* End of while */       
    
  }
  return SUCCESS;
}

/*====================================================================
FUNCTION: rrcllc_set_macehs_or_machs_and_check_if_coexist()

DESCRIPTION:
  
DEPENDENCIES:


RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
boolean rrcllc_set_macehs_or_machs_and_check_if_coexist
(
  void
)
{
  uint32 i;
  boolean machs_present = FALSE;
  boolean mac_ehs_present = FALSE;
  
  for(i=0;i< ordered_config_ptr->mac_dl_parms.num_dflow ;i++)
  {
       /* Check that there is at least one MAC-D FLow */
    if ((ordered_config_ptr->mac_dflow_info[i].mac_dflow_id != INVALID_DFLOW_ID) &&
        (ordered_config_ptr->mac_dflow_info[i].ndlchan != 0) &&
        (ordered_config_ptr->mac_dflow_info[i].no_of_queues != 0)&&
        (ordered_config_ptr->dl_dflow_index_in_use[i] == TRUE ))
    {
       machs_present = TRUE;
       ordered_config_ptr->mac_dl_parms.mac_hs_or_ehs = MAC_HS;
       ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.mac_hs_or_ehs = MAC_HS;
       break;
    }
  }
  
  for(i=0;i< ordered_config_ptr->mac_dl_parms.num_mac_ehs_queue;i++)
  {
    /* Check that there is at least one MAC-D FLow */
    if ((ordered_config_ptr->dl_macehs_logchan_info[i].queue_id != INVALID_QUEUE_ID) &&
      (ordered_config_ptr->mac_ehs_lc_id_in_use[i] == TRUE ))
    {
      ordered_config_ptr->mac_dl_parms.mac_hs_or_ehs = MAC_EHS;
      ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.mac_hs_or_ehs = MAC_EHS;
      mac_ehs_present = TRUE;
      break;
    }
  }
  if(mac_ehs_present && machs_present )
  {
    WRRC_MSG0_ERROR("RRCHSPA+: Both MAC HS and MAC-Ehs co-exist");
     return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*============================================================================
FUNCTION: rrcllc_delete_queue_from_macehs()

DESCRIPTION:
  This function deletes the MACEHS queue . Once the queue has been
  deleted, a hole has been created which would be filled later by another
  function
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
  uecomdef_status_e_type rrcllc_delete_queue_from_macehs
(
  ordered_config_type *config_ptr,
  uint32 deleted_queue_id
)

{
  uint32 cnt = 0;

  for (cnt = 0; cnt < UE_MAX_HS_QUEUE; cnt++)
  {
    if ((config_ptr->mac_ehs_queue_index_in_use[cnt] == TRUE) && 
      (config_ptr->mac_ehs_queue_info[cnt].queue_id != INVALID_QUEUE_ID))
    {

      if (config_ptr->mac_ehs_queue_info[cnt].queue_id == deleted_queue_id)
      {
  
        config_ptr->mac_ehs_queue_info[cnt].queue_id  = INVALID_QUEUE_ID;
  
        config_ptr->mac_ehs_queue_index_in_use[cnt] = FALSE;
        /* Update the queue count */
        config_ptr->mac_dl_parms.num_mac_ehs_queue--;
  
        WRRC_MSG2_HIGH("RRCHS:MACEHS Queue %d del from Orig Queue cnt %d",
                  deleted_queue_id,config_ptr->mac_dl_parms.num_mac_ehs_queue);
      
        return SUCCESS;
      }
    } /* End of if of dflow found */
  } /* End of for of no of dflow */
  
  WRRC_MSG1_ERROR("RRCHS:No deleted queue %d found",deleted_queue_id);
  return FAILURE;
}
/*============================================================================
FUNCTION: set_macehs_q_info()

DESCRIPTION:
  
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type macehs_add_or_del_q_info
( 
  rrc_AddOrReconfMAC_ehs_ReordQ * mac_ehs_info
)
{  

  
  /* Get the deleted Queue Information */

  /* Get the add/reconfig MAC D FLOW Info */
  if ( (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mac_ehs_info,
    mac_ehs_AddReconfQueue_List)) &&
      (mac_ehs_info->mac_ehs_AddReconfQueue_List.n > 0))
  {
    if(rrcllc_add_macehs(mac_ehs_info) == FAILURE)
    {
      return FAILURE;
    }
  }
    
  return SUCCESS;
}

#ifdef FEATURE_WCDMA_HS_FACH


/*============================================================================
FUNCTION: rrcllc_check_if_macehs_queue_exists()

DESCRIPTION:
  
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_check_if_macehs_queue_exists
(
  ordered_config_type *config_ptr,
  uint8 queue_id
)
{
  uint8 idx =0;
  
  for (idx = 0; idx < UE_MAX_HS_QUEUE; idx++)
  {
    if ((config_ptr->mac_ehs_queue_index_in_use[idx] == TRUE) && 
      (config_ptr->mac_ehs_queue_info[idx].queue_id != INVALID_QUEUE_ID))
    {

      if (config_ptr->mac_ehs_queue_info[idx].queue_id == queue_id)
      {
        WRRC_MSG1_HIGH("EFACH:Queue-Id %d exists",queue_id); 
        return TRUE;
      }
    } /* End of if of dflow found */
  } /* End of for of no of dflow */  

  return FALSE;

}

/*============================================================================
FUNCTION: rrcllc_set_hsdpa_l1_queue_info_for_e_fach()

DESCRIPTION:
  This function expects that DB dl_macehs_logchan_info[] is updated. Also mac_dl_parms.ndlchan_macehs and 
  mac_dl_parms.mac_hs_or_ehs are updated.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_set_hsdpa_l1_queue_info_for_e_fach
(
  ordered_config_type  *config_ptr
)
{
  uint8 idx = 0;
  
  WRRC_MSG2_HIGH("EFACH:HS Type %d, No.of MAC-EHS Logical Channels %d",
    config_ptr->mac_dl_parms.mac_hs_or_ehs,
    config_ptr->mac_dl_parms.efach_ndlchans);

  if (config_ptr->mac_dl_parms.efach_ndlchans== 0)
  {
    return;
  }

  for(idx = 0; idx < L1_DL_MAX_LOGICAL_CHANNEL; idx++)
  {
    config_ptr->l1_hsdpa_info.hs_dsch_info.mac_ehs_dlchan_q_info[idx] = INVALID_QUEUE_ID;
  }

  if(config_ptr->mac_dl_parms.mac_hs_or_ehs == MAC_EHS)
  {
    for(idx = 0; idx < config_ptr->mac_dl_parms.efach_ndlchans; idx++)
    {
      config_ptr->l1_hsdpa_info.hs_dsch_info.mac_ehs_dlchan_q_info[config_ptr->efach_dl_logchan_info[idx].mac_id] 
            = config_ptr->efach_dl_logchan_info[idx].queue_id;

      WRRC_MSG2_HIGH("EFACH:Queue %d mapped to LCH %d",config_ptr->efach_dl_logchan_info[idx].queue_id,
                                                 config_ptr->efach_dl_logchan_info[idx].mac_id);
    }
  }

  return;
}

#endif /*FEATURE_WCDMA_HS_FACH*/





/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_r5()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_dl_added_reconfig_trch_r5
(
  rrc_DL_AddReconfTransChInformation_r5 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList * ul_ptr,
  boolean                         tm_signalling_allowed
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = 0;
  tr_ch_id_type ul_trch_idx = 0;
  
  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  uint32 idx =0,idy=0;
  /* Loop variables for copying UL TFS info */
  /*lint -e578 */
  uint8 tf_idx = 0;
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  /*lint +e578 */
  
  /* Boolean used to indicate if multiple logical channels are mapped to
  a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;
  
  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;
  
  rrc_MAC_hs_AddReconfQueue_List* add_queue_list_ptr = NULL;
  rrc_MAC_hs_DelQueue_List* delete_queue_list_ptr = NULL;
  
  MSG_MED("Get DL Add/Reconf TrCH",0,0,0);
  
  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
  /* Get the transport channel index for this transport channel ID.
    * Note: The index should already be there. */
    WRRC_MSG1_HIGH("RRCHS: TrCh type to be added %d",ie_ptr->dl_TransportChannelType.t);
    
    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId1_r5_dch))
      {
        
        trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
        
        if(trch_idx >= UE_MAX_TRCH)
        {
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found",
            ie_ptr->dl_TransportChannelType.u.dch);
          return(FAILURE);
        }

        if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
        {
          /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping",
              ie_ptr->dl_TransportChannelType.u.dch);
        }
      }
      
      /* Determine if multiple dedicated logical channels are present on this TrCH id */
      if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
      {
        multiple_ded_log_chan = TRUE;
      }

    /*
    * Get whether or not the TFS for a given TRCH is included or if it needs
    * to come from an uplink TrCH ID.
    */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_explicit_config:
        
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
          &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
          multiple_ded_log_chan, /* mulitple logical channels? */
          &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
          ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */
        
      case T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_sameAsULTrCH:
        
        while(ul_ptr != NULL && ul_ptr->n > idx)
        {
          if(ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
            ul_TransportChannelIdentity == ul_ptr->elem[idx].transportChannelIdentity)
          { 
            /* Got a match with Uplink TrCH ID */
            WRRC_MSG2_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
              ul_ptr->elem[idx].transportChannelIdentity);
            if(get_dl_transport_format_set(&ul_ptr->elem[idx].transportFormatSet,
              &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
              multiple_ded_log_chan, /* mulitple logical channels? */
              &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
              ) == FAILURE)
            {
              return FAILURE;
            }
            
            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */
          
          /* no match, keep looking */
          else 
          {
            idx++;
          }
        } /* end while */
        
        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
          * Note: The index should already be there.
          */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
            ordered_config_ptr,
            (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
            ul_TransportChannelIdentity);
          if(ul_trch_idx >= UE_MAX_TRCH)
          {
            WRRC_MSG2_ERROR("No DL TFS for TrCH %d, to use UL TrCH %d for TFS",
              ie_ptr->dl_TransportChannelType.u.dch,
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
              ul_TransportChannelIdentity);
            return(FAILURE);
          }
          
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
          case L1_UL_CODING_NONE:
            dl_tfs_info->coding_type = L1_DL_UNCODED;
            break;
          case L1_UL_CODING_CONV_ONE_HALF:
            dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
            break;
          case L1_UL_CODING_CONV_ONE_THIRD:
            dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
            break;
          case L1_UL_CODING_TURBO:
            dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
            break;
          default:
            break; 
          }
          
          
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
          case L1_UL_CRC_0_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
            break;
          case L1_UL_CRC_8_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
            break;
          case L1_UL_CRC_12_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
            break;
          case L1_UL_CRC_16_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
            break;
          case L1_UL_CRC_24_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
            break;
          default:
            WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
            return FAILURE;
          }
          
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8) ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;
            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);

            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }

            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 
        
    case T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_hsdsch:
      /* The DFLOW Index has already been added. So get the index */
      
      if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr->tfs_SignallingMode.u.hsdsch,
    addOrReconfMAC_dFlow))
      {
      /* Check if add & delete queue both are present and network specifies the same 
      queue id in both. This needs to be rejected */
      if ((ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.m.mac_hs_AddReconfQueue_ListPresent) &&
        (ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_AddReconfQueue_List.n > 0) &&
        (ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.m.mac_hs_DelQueue_ListPresent) &&
        (ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_DelQueue_List.n >0))
      {

        /* Check if UTRAN messed up by specifying the same queue id */
        delete_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_DelQueue_List;
        idx =0;
        while (delete_queue_list_ptr->n > idx)
        {
          add_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_AddReconfQueue_List;
          idy=0;
          while (add_queue_list_ptr->n > idy)
          {
            if (delete_queue_list_ptr->elem[idx].mac_hsQueueId == 
    add_queue_list_ptr->elem[idy].mac_hsQueueId)
            {
              /* Invalid Config */
              WRRC_MSG1_ERROR("RRCHS:Queue %d simul added & deleted",
              delete_queue_list_ptr->elem[idx].mac_hsQueueId);
              return FAILURE;
            }
            idy++;
          }
          
          idx++;
        }
      }

      if(RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
    rrc,addOrReconfMAC_dFlow) )
      {
        if (set_dflow_info(&ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow) == FAILURE)
        {
          WRRC_MSG0_ERROR("RRCHS:Invalid DFLOW info");
          return FAILURE;
        }
      }
      }
      
      /* SET HARQ info in L1 structure */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
    rrc_HSDSCH_Info,harqInfo))
      {
        MSG_MED("RRCHS:HARQ info present",0,0,0);
        if (set_harq_information(ordered_config_ptr,&ie_ptr->tfs_SignallingMode.u.hsdsch->harqInfo) == FAILURE)
        {
          WRRC_MSG0_ERROR("RRCHS:Unable to set HARQ information");
          return FAILURE;
        }
      }
      
      /* Indicate that HSDSCH Info is present which would be used later to set bit mask when HSDPA
      is already ACTIVE */
      hsdpa_msg_params.hsdsch_info_present = TRUE;
      
      hsdpa_msg_params.msg_params_present = TRUE;
      
      break;
    default:
      return FAILURE;
      
    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
 PDU size has changed.*/ 
    if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
  rrc_DL_TrCH_TypeId1_r5_dch))
    {
      rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch, 
                ordered_config_ptr->rrc_state_indicator, DL);  
    }  
  } /* end CELL_DCH check */
  
  /* States not supported */
  else
  {
    WRRC_MSG1_ERROR("State %d not supported for TrCH, return FAILURE",ordered_config_ptr->rrc_state_indicator);
    return FAILURE;
  }
  
  /***************************************************************/
  /* Transport Channel Count has been updated in RB Mapping info */
  /***************************************************************/

  if (!(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->tfs_SignallingMode,
    rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_hsdsch)) )
  {
    /* Check for the BLER */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
  rrc_DL_AddReconfTransChInformation_r5,dch_QualityTarget))
    {
      /* Save the BLER if found, and is in range */
      if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
         (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER))
      {
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
          (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
      }
    
    /* BLER is out of range */
      else
      {
      /* According to L1 I/F, must set an invalid value of -127 */
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
        WRRC_MSG1_ERROR("DL BLER %d is out of range",
            ie_ptr->dch_QualityTarget.bler_QualityValue
            );
        return FAILURE;
      }
    } /* end if BLER present */
    else
    {
     /* IE omitted. Set the target to -127 which would result 1% of BLER target.
      * This is good UE behaviour though the spec is ambiguous on what UE
      * behavior should be in this case.  
      */
      WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
    } 
  }
  else
  {
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
  rrc_DL_AddReconfTransChInformation_r5,dch_QualityTarget))
    {
      /*Ignoring dch quality target for HS*/
    }
  }
  
  
  /* Indicate that DL L1 needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
  
  return SUCCESS;
} /* end get_dl_added_reconfig_trch_r5() */


/*================================================================================================
FUNCTION: rrcllc_copy_pdu_info_per_queue()

DESCRIPTION:
  This function copies the PDU Information associated with a queue from MAC OC structure
  to L1 structure

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  
===============================================================================================*/
static void rrcllc_copy_pdu_info_per_queue(uint8 dflow_index, uint8 queue_index)
{
  uint8 index = 0;
  
  uint8 pdu_size_index = 0;
  /* Get the first available index in L1 structure and put the queue info there */
  
  for (index = 0; index < L1_DL_MAC_HS_Q_MAX_COUNT; index++)
  {
    if (ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.
      mac_hs_q_info[index].mac_hs_q_id == INVALID_QUEUE_ID)
    {
      /* Got the place to store index. First set Queue Id */
      ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.mac_hs_q_info[index].mac_hs_q_id = 
        ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].queue_id;
      
      ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.mac_hs_q_info[index].num_mac_d_pdu_sz = 
        ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes;
      
      /* Increment no of queues in L1 */
      ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.num_mac_hs_q++;
      
      
      WRRC_MSG1_HIGH("RRCHS:L1 queues after increment %d", 
        ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.num_mac_hs_q);
      
      for (pdu_size_index = 0; pdu_size_index < 
        ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].no_of_pdu_sizes; pdu_size_index++)
      {
        /* PDU SIZE INDEX */
        ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.
          mac_hs_q_info[index].mac_d_pdu_sz_info[pdu_size_index].pdu_sz_idx = 
          ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].
          pdu_info[pdu_size_index].mac_pdu_index;
        
        /* PDU SIZE */
        ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.
          mac_hs_q_info[index].mac_d_pdu_sz_info[pdu_size_index].pdu_sz = 
          ordered_config_ptr->mac_dflow_info[dflow_index].queue_info[queue_index].
          pdu_info[pdu_size_index].mac_pdu_size;
      }
      
      break;
    }
  }
}


/*================================================================================================
FUNCTION: rrcllc_check_if_queue_exists()

DESCRIPTION:
  This function checks if the passed queue id is already present in L1 structure.  

DEPENDENCIES:
  None.

RETURN VALUE:
  True if queue found else False

SIDE EFFECTS:
  
===============================================================================================*/
static boolean rrcllc_check_if_queue_exists(uint8 queue_id)
{
  uint8 index = 0;
  for (index = 0; index < UE_MAX_HS_QUEUE; index++)
  {
    if (ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.mac_hs_q_info[index].mac_hs_q_id == queue_id)
    {
      WRRC_MSG1_HIGH("RRCHS:Queue %d already present. Don't update",queue_id);
      return TRUE;
    }
  }
  return FALSE;
}

/*================================================================================================
FUNCTION: set_hsdpa_l1_queue_info()

DESCRIPTION:
  This function checks all the MAC D Flows in MAC structure and gets
  the index to MAC DFLOW and then sets the get queue info.  

DEPENDENCIES:
  None.

RETURN VALUE:
  Success/Failure.

SIDE EFFECTS:
  
===============================================================================================*/
uecomdef_status_e_type set_hsdpa_l1_queue_info
(
  void
)
{
  uint8 cnt = 0;
  uint8 queue_cnt = 0;
  
  if ((ordered_config_ptr->mac_dl_parms.num_dflow == 0)
    && (ordered_config_ptr->mac_dl_parms.ndlchan_macehs == 0)
    )
  {
    WRRC_MSG0_HIGH("No of Dflow and MAC EHS queue id are 0");
    return SUCCESS;
  }
  /* Initialize the queue in L1 structure */
  rrcllc_init_hsdpa_l1_queue(ordered_config_ptr);
  
  if(ordered_config_ptr->mac_dl_parms.mac_hs_or_ehs == MAC_EHS )
  {
    for(cnt = 0; cnt < ordered_config_ptr->mac_dl_parms.ndlchan_macehs; cnt++)
    {

      ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.mac_ehs_dlchan_q_info[ordered_config_ptr->dl_macehs_logchan_info[cnt].mac_id] 
            = ordered_config_ptr->dl_macehs_logchan_info[cnt].queue_id;
    }
  }
  else 
  {
  for (cnt = 0; cnt < ordered_config_ptr->mac_dl_parms.num_dflow && (cnt < UE_MAX_MAC_D_FLOW); cnt++)
  {
    if ((ordered_config_ptr->dl_dflow_index_in_use[cnt] == TRUE) && 
      (ordered_config_ptr->mac_dflow_info[cnt].mac_dflow_id != INVALID_DFLOW_ID))
    {
      /* Got the index and dflow id */
      /* Get the queue id */
      
      for (queue_cnt = 0; queue_cnt < ordered_config_ptr->mac_dflow_info[cnt].no_of_queues 
    && (queue_cnt < UE_MAX_HS_QUEUE); queue_cnt++)
      {
        if (ordered_config_ptr->mac_dflow_info[cnt].queue_info[queue_cnt].queue_id != INVALID_QUEUE_ID)
        {
          /* Preventive check : If queue is already existing in L1 structure. */
          if (rrcllc_check_if_queue_exists(ordered_config_ptr->mac_dflow_info[cnt].queue_info[queue_cnt].
            queue_id) == TRUE)
          {
            continue;
          }
          /* Call a function to store this queue value in L1 structure */
          rrcllc_copy_pdu_info_per_queue(cnt, queue_cnt);
          
          /* Preventive check */
          if (ordered_config_ptr->l1_hsdpa_info.hs_dsch_info.macd_info.num_mac_hs_q == 
            L1_DL_MAC_HS_Q_MAX_COUNT)
          {
            break;
          }
        }
      }
    }
    else
    {
      WRRC_MSG2_ERROR("RRCHS:Serious Error. DFLOW Id %d DFLOW Index %d ",
          ordered_config_ptr->mac_dflow_info[cnt].mac_dflow_id,
          ordered_config_ptr->dl_dflow_index_in_use[cnt]);

      return FAILURE;
    }
  }
  }
  return SUCCESS;
}


/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_ul_dch_trch_info_r6()

DESCRIPTION:
  This function takes up deleted and added UL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_add_delete_and_arrange_ul_dch_trch_info_r6
(
  OSS_UINT32 ul_add_recfg_trch_info_list_present,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r6  * ul_trch_add_reconfig_ptr,
  OSS_UINT32 ul_deleted_trch_info_list_present,
  rrc_UL_DeletedTransChInfoList_r6 * ul_deleted_trch_list_ptr
)
{

  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  uint32 idx =0,idy=0, idz=0;

  rrc_UL_DeletedTransChInfoList_r6 *temp_ul_trch_delete_ptr = ul_deleted_trch_list_ptr;
  rrc_UL_AddReconfTransChInfoList_r6 *temp_ul_trch_add_reconfig_ptr = ul_trch_add_reconfig_ptr;
  rrc_E_DCH_AddReconf_MAC_d_FlowList * temp_e_dch_mac_d_flow_list_ptr = NULL;

  if((temp_ul_trch_delete_ptr != NULL) && (temp_ul_trch_add_reconfig_ptr != NULL))
  {
    for(idx=0; idx<temp_ul_trch_delete_ptr->n; idx++)
    {
      if(T_rrc_UL_TransportChannelIdentity_r6_dch_usch == temp_ul_trch_delete_ptr->elem[idx].t)
      {
        for(idy=0; idy<temp_ul_trch_add_reconfig_ptr->n; idy++)
        {
          if(T_rrc_UL_AddReconfTransChInformation_r6_dch_usch == temp_ul_trch_add_reconfig_ptr->elem[idy].t)
          {
            if(temp_ul_trch_delete_ptr->elem[idx].u.dch_usch->ul_TransportChannelIdentity == temp_ul_trch_add_reconfig_ptr->elem[idy].u.dch_usch->transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
              temp_ul_trch_add_reconfig_ptr->elem[idy].u.dch_usch->transportChannelIdentity,temp_ul_trch_delete_ptr->elem[idx].u.dch_usch->ul_TransportChannelIdentity);
              return FAILURE;
            }
          }
        }
      }     
      else if(T_rrc_UL_TransportChannelIdentity_r6_e_dch == temp_ul_trch_delete_ptr->elem[idx].t)
      {
        for(idy=0; idy<temp_ul_trch_add_reconfig_ptr->n; idy++)
        {
          if(T_rrc_UL_AddReconfTransChInformation_r6_e_dch == temp_ul_trch_add_reconfig_ptr->elem[idy].t)
          {
            if(temp_ul_trch_add_reconfig_ptr->elem[idy].u.e_dch->m.addReconf_MAC_d_FlowListPresent)
            {           	
              temp_e_dch_mac_d_flow_list_ptr = &temp_ul_trch_add_reconfig_ptr->elem[idy].u.e_dch->addReconf_MAC_d_FlowList;
              for(idz = 0; idz <temp_e_dch_mac_d_flow_list_ptr->n; idz++)
              {
                if(temp_ul_trch_delete_ptr->elem[idx].u.e_dch == temp_e_dch_mac_d_flow_list_ptr->elem[idz].mac_d_FlowIdentity)
                {
                  WRRC_MSG2_ERROR("Same edch Trch ID found in add/reconfig list %d and deleted list %d ",
                  temp_e_dch_mac_d_flow_list_ptr->elem[idz].mac_d_FlowIdentity,temp_ul_trch_delete_ptr->elem[idx].u.e_dch);
                  return FAILURE;
                }
              }
            }
          }
        }
      }   
    }
  }

  idx = 0;
  idy = 0;
  if ((ul_deleted_trch_info_list_present) && (ul_deleted_trch_list_ptr != NULL))
  {
    while (ul_deleted_trch_list_ptr->n >idx)
    {
      /* Remove this transport channel from MAC, L1 and local data.
       * and update transport channel count in L1, MAC and local data.
       */
      if(get_ul_deleted_trch_info_list_r6(&ul_deleted_trch_list_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;
    } /* end while */
    idx = 0;
    /*Fill the holes only when Ul EDCH Trch is deleted*/
    while(ul_deleted_trch_list_ptr->n > idx) 
    {
      if (T_rrc_UL_TransportChannelIdentity_r6_e_dch == ul_deleted_trch_list_ptr->elem[idx].t)
      {
        rrcllc_fill_holes_in_e_mac_d_flow_idx();
        break;
      }
      idx++;
    }
  }
  idx =0;
  if ((ul_add_recfg_trch_info_list_present) && (ul_trch_add_reconfig_ptr != NULL))
  {
    /* Get the UL Added/Reconfigured Info */
    while(ul_trch_add_reconfig_ptr->n >idx ) 
    {
      if (T_rrc_UL_AddReconfTransChInformation_r6_dch_usch ==
          ul_trch_add_reconfig_ptr->elem[idx].t)
      {
        trch_id = (tr_ch_id_type)
                  ul_trch_add_reconfig_ptr->elem[idx].u.dch_usch->transportChannelIdentity;
  
        if((rrcllc_get_ul_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
        {
          WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
          return(FAILURE);
        }
      }
      else if ((T_rrc_UL_AddReconfTransChInformation_r6_e_dch == ul_trch_add_reconfig_ptr->elem[idx].t) && 
               (ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->m.addReconf_MAC_d_FlowListPresent)) 
      {
        rrc_E_DCH_AddReconf_MAC_d_FlowList * e_dch_mac_d_flow_list_ptr = NULL;

        e_dch_mac_d_flow_list_ptr = &ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->addReconf_MAC_d_FlowList;
        idy =0;
        while (e_dch_mac_d_flow_list_ptr->n > idy)
        {
          if((rrcllc_get_e_mac_dflow_idx((uint8)e_dch_mac_d_flow_list_ptr->elem[idy].
              mac_d_FlowIdentity)) == MAX_E_DCH_MAC_D_FLOW)
          {
            WRRC_MSG1_ERROR("RRCEUL: Couldn't find idx for e-dch MAC-D flow: %d",
              e_dch_mac_d_flow_list_ptr->elem[idy].mac_d_FlowIdentity);
            return(FAILURE);
          }

          idy++;
        }

        rrcllc_update_mac_eul_action(TRUE);
      }
      idx++;
    } /* while(...) */
  }

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->ul_trch_cnt indices.
   */
  rrcllc_fill_holes_in_ul_dch_trch_idx();

  rrcllc_fill_holes_in_e_mac_d_flow_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */
  rrcllc_sort_ul_dch_trch_index_array(&ordered_config_ptr->ul_dch_trch_info[0], 
                                      (RRCLLC_UPLINK_TRCH_COUNT(ordered_config_ptr)));
  return(SUCCESS);
}


/*============================================================================
FUNCTION: rrcllc_process_ul_common_trch_info_r5_r6()

DESCRIPTION:
  This function takes a pointer to the Uplink Common Information for all
  Transport Channels IE and copies it to ORDERED_CONFIG.

REUSABILITY: R5/R6

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_ul_common_trch_info_r5_r6
(
  OSS_UINT32 ul_common_trch_info_present,
  rrc_UL_CommonTransChInfo_r4 *ie_ptr
)
{
  if (ul_common_trch_info_present)
  {
    if(get_ul_info_common_for_all_trch_r5(ie_ptr,
      ordered_config_ptr->rrc_state_indicator)
      == FAILURE)
    {
      return FAILURE;
    } 
  }
  return(SUCCESS);
}

/*================================================================================================
FUNCTION: process_added_reconfig_e_dch_info()

DESCRIPTION:
  This function takes a pointer to the EDCH Added or Reconfigured 
  mac-d flow info and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:

===============================================================================================*/
static uecomdef_status_e_type process_added_reconfig_e_dch_info
(
  rrc_UL_AddReconfTransChInformation_r6 * ie_ptr
)
{

  /* local transport channel variables */
  uint8 e_mac_d_flow_idx = MAX_E_DCH_MAC_D_FLOW;
  e_tti_enum_type temp_tti;
  rrc_E_DCH_AddReconf_MAC_d_FlowList * e_mac_d_flow_list_ptr = NULL;
  uint32 idx =0;
  if ( !(RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r6_e_dch)))
  {
    WRRC_MSG1_ERROR("Un-wanted call to function to process e_dch_info. choice: %d",
      ie_ptr->t);
    return(FAILURE);
  }
  /*Added NULL check to bypass the KW error*/
  if(ie_ptr->u.e_dch == NULL)
  {
    return(FAILURE);
  }
#ifdef FEATURE_MAC_I
  ordered_config_ptr->mac_ul_parms.ul_pdu_type = MAC_E_PDU_HDR;
#endif /* FEATURE_MAC_I */	
	
  /*Update the TTI info in MAC and L1 struct. 10ms or 2ms */
  /* Check with stored TTI value before indicating reconfig to L1/MAC*/
  temp_tti = ((ie_ptr->u.e_dch->tti == rrc_E_DCH_TTI_tti10) ? E_TTI_10MS : E_TTI_2MS);

  if(temp_tti == E_TTI_2MS)
  {
    /* Check to see NV HSUPA Category.
     * For supporting 2msec TTI, Below are the valid cats
     * Cat - 2,4,6,7,8,9 from table Table 5.1g: FDD E-DCH physical layer categories
     * in 25.306.If N/W is trying to configure 2msec TTI with any other 
     * UE cat Bcoz of N/W issue, Reject this configuration
     */
    if(
       (wcdma_nv_hsupa_category == 0)||
       (wcdma_nv_hsupa_category == 1)||
       (wcdma_nv_hsupa_category == 3)||
       (wcdma_nv_hsupa_category == 5)
      )
    {
      WRRC_MSG1_ERROR("RRCEUL:Error N/W try to config 2msec TTI when UE cat = %d which is not 2 or 4 or 6", 
        wcdma_nv_hsupa_category);
      return(FAILURE);
    }
  }
  
  /*We store e-tti & harq_rv_config which are trch i.e in L1 data base,
  *hence init them when previously EUL was not active
  */
  if(current_config_ptr->e_dch_transmission == FALSE)
  {
    WRRC_MSG2_HIGH("RRCEUL:init e_tti in l1 db, prv value =%d,harq_rv_config=%d", 
     ordered_config_ptr->l1_e_info.e_ul_info.e_tti, 
     ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config);
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_NOT_PRESENT;
    ordered_config_ptr->l1_e_info.e_ul_info.e_tti = NUM_E_TTI;
    ordered_config_ptr->l1_e_info.e_dl_info.e_tti = NUM_E_TTI;
  }

  if (temp_tti != ordered_config_ptr->l1_e_info.e_ul_info.e_tti) 
  {
    rrcllc_update_mac_eul_action(TRUE);
    rrcllc_update_l1_e_req_mask_for_e_dl_info(TRUE);
    rrcllc_update_l1_e_req_mask_for_e_ul_info(TRUE);

    /*Set all E-DCH active set DL RL's bit mask to reconfig, 
     *This is used by L1 to reconfigure AGCH/RGCH/HICH
     *Channels at TTI reconfiguration
     */
    rrcllc_update_e_dl_channel_mask_for_e_active_set_rl_to_reconfig();
    
    ordered_config_ptr->mac_e_config.e_tti = temp_tti;
    ordered_config_ptr->l1_e_info.e_ul_info.e_tti = temp_tti;
    ordered_config_ptr->l1_e_info.e_dl_info.e_tti = temp_tti;
    if (current_config_ptr->mac_e_config.e_tti != NUM_E_TTI) 
    {
      WRRC_MSG1_HIGH("RRCEUL: Setting L1/MAC action cuz TTI RECONFIG (tti: %d)",
        temp_tti);
      ordered_config_ptr->mac_e_reset_indicator = TRUE;
    }
  }

  /* Update the harq info if present */
  if (ie_ptr->u.e_dch->harq_Info == rrc_UL_AddReconfTransChInformation_r6_harq_Info_rv0)
  {
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_0;
  }
  else
  {
    /* Default value is 'rvtable', in which case the UE shall use an RSN
       based RV index as specified in 25.212*/
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_TABLE;
  }

  if (ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config !=
      current_config_ptr->l1_e_info.e_ul_info.harq_rv_config) 
  {
    WRRC_MSG0_HIGH("RRCEUL: Setting UL-L1 action cuz Harq Info RECONFIG");
    rrcllc_update_l1_e_req_mask_for_e_ul_info(TRUE);
  }
  
  /* Update Add-Recong mac-d flow info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->u.e_dch,
   rrc_UL_AddReconfTransChInformation_r6_e_dch,addReconf_MAC_d_FlowList))
  {       
    e_mac_d_flow_list_ptr = &ie_ptr->u.e_dch->addReconf_MAC_d_FlowList;
    if(e_mac_d_flow_list_ptr != NULL)
    {
      /* Update Add-Recong mac-d flow info */
      while ((e_mac_d_flow_list_ptr != NULL) && (e_mac_d_flow_list_ptr->n > idx))
      {       
        if ((e_mac_d_flow_idx = rrcllc_find_e_mac_d_flow_idx(
               (uint8)e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity)) == MAX_E_DCH_MAC_D_FLOW)
        {
          WRRC_MSG1_ERROR("RRCEUL: E-DCH mac-d flow ID %d, no idx found",
              e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity);
          return FAILURE;
        }
        else
        {
          if(rrcllc_restore_ul_emac_rb_mapping_info(e_mac_d_flow_idx) == FAILURE)
          {
            /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
            WRRC_MSG1_ERROR("RRCEUL: E-DCH mac-d flow ID %d, not correct info found in Stanalone rb-mapping info",
                e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity);
          }
          rrcllc_update_mac_eul_action(TRUE);
          /* Only allowed to be absent when already defined for this E-DCH MAC-d flow*/
          if (RRC_MSG_COMMON_BITMASK_IE(e_mac_d_flow_list_ptr->elem[idx],
         rrc_E_DCH_AddReconf_MAC_d_Flow,mac_d_FlowPowerOffset))
          {
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
              e_mac_d_flow_power_offset = e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowPowerOffset;
          }
          else
          {
            if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                e_mac_d_flow_power_offset > RRCLLC_MAX_E_MAC_D_FLOW_PWR_OFFSET)
            {
              WRRC_MSG0_ERROR("RRCEUL: E-MAC-d flow pwr offset needs to be provided");
              return FAILURE;
            }
          }
          /* Only allowed to be absent when already defined for this E-DCH MAC-d flow */
          if (RRC_MSG_COMMON_BITMASK_IE(e_mac_d_flow_list_ptr->elem[idx],
        rrc_E_DCH_AddReconf_MAC_d_Flow,mac_d_FlowMaxRetrans))
          {
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
              e_mac_d_flow_max_num_of_retx = e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowMaxRetrans;
          }
          else
          {
            if(ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
               e_mac_d_flow_max_num_of_retx > RRCLLC_MAX_E_MAC_D_FLOW_NUM_RETX)
            {
              WRRC_MSG0_ERROR("RRCEUL: E-MAC-d max num of retx needs to be provided");
              return FAILURE;
            }
          }
          
          /* if no mux list is provided, use the previous one.  If there is no previous then allow multiplexing of 
             MAC-d PDU's from the E-DCH MAC-d flow indicated in the IE "E-DCH MAC-d flow identity" with MAC-d PDU's 
             from any other E-DCH MAC-d flow in the same MAC-e PDU.*/
          if (RRC_MSG_COMMON_BITMASK_IE(e_mac_d_flow_list_ptr->elem[idx],
        rrc_E_DCH_AddReconf_MAC_d_Flow,mac_d_FlowMultiplexingList))
          {
            /* Only look at first byte of bitstring as max e_mac_d flow is 8*/
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].e_mac_d_flow_mux_list =
              rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(
                &e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowMultiplexingList);
          }
          
          if (RRC_MSG_COMMON_BITMASK_IE(e_mac_d_flow_list_ptr->elem[idx],
        rrc_E_DCH_AddReconf_MAC_d_Flow,transmissionGrantType))
          {
            if (e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.t == 
                T_rrc_E_DCH_AddReconf_MAC_d_Flow_transmissionGrantType_non_ScheduledTransGrantInfo)
            {
              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                e_tx_grant_info = MAC_E_TX_GRANT_NON_SCHEDULED;

              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                non_sched_tx_grant.max_mac_e_pdu_size_in_bits = 
                  e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.
                    non_ScheduledTransGrantInfo->maxMAC_e_PDUContents;

              if (e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->m.
         ms2_NonSchedTransmGrantHARQAllocPresent)
              {
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 
                    rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(
                      &e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.
                         non_ScheduledTransGrantInfo->ms2_NonSchedTransmGrantHARQAlloc);
              }
              else /* Default value is: transmission in all HARQ processes is allowed. so set all to 1 */
              {
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 0xFF;
              }
            }
            else /* rrc_scheduledTransmissionGrantInfo */
            {
              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].e_tx_grant_info = MAC_E_TX_GRANT_SCHEDULED;
            }
          }
        }

        idx++;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("RRCEUL: E-DCH mac-d flow list is NULL");
      return FAILURE;
    }
  }
  return SUCCESS;
}

/*================================================================================================
FUNCTION: get_ul_added_reconfig_trch()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
static uecomdef_status_e_type get_ul_added_reconfig_trch_r6
(
  rrc_UL_AddReconfTransChInformation_r6 * ie_ptr
)
{
  MSG_MED("Get UL Add/Reconf TrCH",0,0,0);

  if (RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r6_dch_usch))
  {

    if(rrcllc_dch_usch_add_trch_info((tr_ch_id_type)ie_ptr->u.dch_usch->transportChannelIdentity,
                              &ie_ptr->u.dch_usch->transportFormatSet) == FAILURE)
      {
        return FAILURE;
      }
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r6_e_dch)) 
  {
    if (FAILURE == process_added_reconfig_e_dch_info(ie_ptr))
    {
      return FAILURE;
    }
  }
  return SUCCESS;
} /* end get_ul_added_reconfig_trch_r6() */

/*============================================================================
FUNCTION: rrcllc_store_ul_add_reconf_trch_info_list_r6()

DESCRIPTION:
  This function updates IE "ul added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_ul_add_reconf_trch_info_list_r6
(
  OSS_UINT32 ul_add_recfg_trch_info_list_present,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r6 * ul_trch_add_reconfig_ptr  
)
{
  if(ul_add_recfg_trch_info_list_present)
  {
    uint32 idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (ul_trch_add_reconfig_ptr->n > idx))
    {
      if(get_ul_added_reconfig_trch_r6(&ul_trch_add_reconfig_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
      
      /* get next TrCH Info */
      idx++;

    } /* end while */
  } /* end UL TrCH Add/Reconfig */

  /* Check to make sure UL TFS count is within UE Capabilities */
  if(rrcllc_check_ul_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}


/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_r6()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
static uecomdef_status_e_type get_dl_added_reconfig_trch_r6
(
  rrc_DL_AddReconfTransChInformation_r5 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList_r6 * ul_ptr
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = 0;
  tr_ch_id_type ul_trch_idx = 0;
  uint32 idx=0,idy=0;
  /* Pointers to UL and DL TFS info for use when copying existing config */
  /*lint -e578 */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  
  /* Loop variables for copying UL TFS info */
  uint8 tf_idx = 0;
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  /*lint +e578 */
  
  /* Boolean used to indicate if multiple logical channels are mapped to
  a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;
  
  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;
  
  rrc_MAC_hs_AddReconfQueue_List * add_queue_list_ptr = NULL;
  
  rrc_MAC_hs_DelQueue_List * delete_queue_list_ptr = NULL;
  
  
  MSG_MED("Get DL Add/Reconf TrCH",0,0,0);
  
  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
  /* Get the transport channel index for this transport channel ID.
    * Note: The index should already be there. */
    WRRC_MSG1_HIGH("RRCHS: TrCh to be added %d",ie_ptr->dl_TransportChannelType.t);
    
    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
    rrc_DL_TrCH_TypeId1_r5_dch))
      {
        
        trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
        
        if(trch_idx >= UE_MAX_TRCH)
        {
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found",
            ie_ptr->dl_TransportChannelType.u.dch);
          return(FAILURE);
        }

        if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
        {
          /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping",
               ie_ptr->dl_TransportChannelType.u.dch);
        } 
      }
      
      /* Determine if multiple dedicated logical channels are present on this TrCH id */
      if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
      {
        multiple_ded_log_chan = TRUE;
      }

    /*
    * Get whether or not the TFS for a given TRCH is included or if it needs
    * to come from an uplink TrCH ID.
    */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_explicit_config:
        
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
          &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
          multiple_ded_log_chan, /* mulitple logical channels? */
          &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
          ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */
        
      case T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_sameAsULTrCH:
        idx =0;
        while(ul_ptr != NULL && ul_ptr->n > idx)
        {
          /* check whether trCh is of type rrc_UL_AddReconfTransChInformation_r6_dch_usch */
          if((T_rrc_UL_AddReconfTransChInformation_r6_dch_usch == ul_ptr->elem[idx].t) &&
             (ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity == 
              ul_ptr->elem[idx].u.dch_usch->transportChannelIdentity))
          { 
            /* Got a match with Uplink TrCH ID */
            WRRC_MSG2_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
              ul_ptr->elem[idx].u.dch_usch->transportChannelIdentity);
            if(get_dl_transport_format_set(&ul_ptr->elem[idx].u.dch_usch->transportFormatSet,
              &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
              multiple_ded_log_chan, /* mulitple logical channels? */
              &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
              ) == FAILURE)
            {
              return FAILURE;
            }
            
            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */
          
          /* no match, keep looking */
          else 
          {
            idx++;
          }
        } /* end while */
        
        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
          * Note: The index should already be there.
          */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
            ordered_config_ptr,
            (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
            ul_TransportChannelIdentity);
          if(ul_trch_idx >= UE_MAX_TRCH)
          {
            WRRC_MSG2_ERROR("No DL TFS for TrCH %d, to use UL TrCH %d for TFS",
              ie_ptr->dl_TransportChannelType.u.dch,
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
              ul_TransportChannelIdentity);
            return(FAILURE);
          }
          
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
          case L1_UL_CODING_NONE:
            dl_tfs_info->coding_type = L1_DL_UNCODED;
            break;
          case L1_UL_CODING_CONV_ONE_HALF:
            dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
            break;
          case L1_UL_CODING_CONV_ONE_THIRD:
            dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
            break;
          case L1_UL_CODING_TURBO:
            dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
            break;
          default:
            break; 
          }
          
          
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
          case L1_UL_CRC_0_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
            break;
          case L1_UL_CRC_8_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
            break;
          case L1_UL_CRC_12_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
            break;
          case L1_UL_CRC_16_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
            break;
          case L1_UL_CRC_24_BITS:
            dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
            break;
          default:
            WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
            return FAILURE;
          }
          
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8) ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;
            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);

            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }

            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 
        
    case T_rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_hsdsch:
      /* The DFLOW Index has already been added. So get the index */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
    rrc,addOrReconfMAC_dFlow))
      {
      /* Check if add & delete queue both are present and network specifies the same 
      queue id in both. This needs to be rejected */
      if ((ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.m.
        mac_hs_AddReconfQueue_ListPresent) &&
        (ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_AddReconfQueue_List.n > 0) &&
        (ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.m.
        mac_hs_DelQueue_ListPresent) &&
        (ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_DelQueue_List.n >0 ))
      { 
        idx =0;
        /* Check if UTRAN messed up by specifying the same queue id */
        delete_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_DelQueue_List;
        
        while (delete_queue_list_ptr->n > idx)
        {
          add_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow.mac_hs_AddReconfQueue_List;
          idy =0;
          while (add_queue_list_ptr->n > idy)
          {
            if (delete_queue_list_ptr->elem[idx].mac_hsQueueId == 
    add_queue_list_ptr->elem[idy].mac_hsQueueId)
            {
              /* Invalid Config */
              WRRC_MSG1_ERROR("RRCHS:Queue %d simul added & deleted", delete_queue_list_ptr->elem[idx].mac_hsQueueId);
              return FAILURE;
            }
           idy++;
          }
          
          idx++;
        }
        
      }

      if(ie_ptr->tfs_SignallingMode.u.hsdsch->m.addOrReconfMAC_dFlowPresent )
      {
        if (set_dflow_info(&ie_ptr->tfs_SignallingMode.u.hsdsch->addOrReconfMAC_dFlow) == FAILURE)
        {
          WRRC_MSG0_ERROR("RRCHS:Invalid DFLOW info");
          return FAILURE;
        }
      }
      }
      
      /* SET HARQ info in L1 structure */
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->tfs_SignallingMode.u.hsdsch,
    rrc_HSDSCH_Info,harqInfo))
      {
        MSG_MED("RRCHS:HARQ info present",0,0,0);
        if (set_harq_information(ordered_config_ptr,&ie_ptr->tfs_SignallingMode.u.hsdsch->harqInfo) == FAILURE)
        {
          WRRC_MSG0_ERROR("RRCHS:Unable to set HARQ information");
          return FAILURE;
        }
      }
      
      /* Indicate that HSDSCH Info is present which would be used later to set bit mask when HSDPA
      is already ACTIVE */
      hsdpa_msg_params.hsdsch_info_present = TRUE;
      
      hsdpa_msg_params.msg_params_present = TRUE;
      break;
      
    default:
      return FAILURE;
      
    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
 PDU size has changed.*/ 
    if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
  rrc_DL_TrCH_TypeId1_r5_dch))
    {
      rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch, 
                ordered_config_ptr->rrc_state_indicator, DL);  
    }  
  } /* end CELL_DCH check */
  
  /* States not supported */
  else
  {
    WRRC_MSG0_ERROR("CELL_FACH/PCH/URA_PCH not supported for TrCH");
    return FAILURE;
  }
  
  /***************************************************************/
  /* Transport Channel Count has been updated in RB Mapping info */
  /***************************************************************/
  if (!(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->tfs_SignallingMode,rrc_DL_AddReconfTransChInformation_r5_tfs_SignallingMode_hsdsch)))
  {
    /* Check for the BLER */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
  rrc_DL_AddReconfTransChInformation_r5,dch_QualityTarget))
    {
      /* Save the BLER if found, and is in range */
      if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
        (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER))
      {
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
          (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
      }
      
      /* BLER is out of range */
      else
      {
        /* According to L1 I/F, must set an invalid value of -127 */
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
        WRRC_MSG1_ERROR("DL BLER %d is out of range",
          ie_ptr->dch_QualityTarget.bler_QualityValue
          );
        return FAILURE;
      }
    } /* end if BLER present */
    else
    {
      /* IE omitted. Set the target to -127 which would result 1% of BLER target.
       * This is good UE behaviour though the spec is ambiguous on what UE
       * behavior should be in this case.  
       */
      WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
    }
  }
  else
  {
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
  rrc_DL_AddReconfTransChInformation_r5,dch_QualityTarget))
    {
      WRRC_MSG0_HIGH("Ignoring dch quality target for HS");
    }
  }
  /* Indicate that DL L1 needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
  
  return SUCCESS;
} /* end get_dl_added_reconfig_trch_r6() */

/*============================================================================
FUNCTION: rrcllc_store_dl_add_reconf_trch_info_list_r6()

DESCRIPTION:
  This function updates IE "dl added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_dl_add_reconf_trch_info_list_r6
(
  OSS_UINT32 dl_add_reconf_trch_info_list_present,
  rrc_DL_AddReconfTransChInfoList_r5 * dl_trch_add_reconfig_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r6  * ul_trch_add_reconfig_ptr  
)
{
  uint32 idx, tf_idx;
  /* Get the DL Added or Reconfigured Transport Channel information */
  if(dl_add_reconf_trch_info_list_present)
  {

    idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (dl_trch_add_reconfig_ptr->n > idx))
    {
      if(get_dl_added_reconfig_trch_r6(&dl_trch_add_reconfig_ptr->elem[idx],
        ul_trch_add_reconfig_ptr) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;

    } /* end while */
  } 
  
  /* At this point store the Queue Information in L1 structure. To add later if HSDSCH config changed */
  
  /* Adjust TB size on every TrCH if necessary. */ 
  for (idx=0; idx<UE_MAX_TRCH; idx++)
  {
  /* The number of logical channel mapped to this transport channel 
  increases from 1 to 2. Needs to adjsut the tb_szie to
    reflect the adding of the MAC header.                         */
    if (ordered_config_ptr->dl_dch_trch_info[idx].dch_present &&
      (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up ||
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down))
    {
      /* TrCH IE is present and the TrCH ID is appear in the TrCH info table. */
      if (!(dl_add_reconf_trch_info_list_present &&
        find_trch_id_in_config_trch_list_r5(dl_trch_add_reconfig_ptr,
        ordered_config_ptr->dl_dch_trch_info[idx].trch_id)))
      {
        /* Added MAC head size to the current TB size.  */
        if (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          !ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Increasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if(ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size += 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
        /* Substract MAC head size from the current TB size.  */
        else if (!ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Decreasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size -= 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
      }
      WRRC_MSG0_HIGH("Clean TB size adjust flag in RBSetup");
      /* Reset lg_ch_cnt_up and lg_ch_cnt_down. */
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up = FALSE;
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down = FALSE;
    }
    else if (!ordered_config_ptr->dl_dch_trch_info[idx].dch_present)
    {
    /* Since the activated transport channels are sorted before stored in
      the memory, break the looping after reaching the 1st invalid block.  */
      break;
    }
  }
  
  /* Check to make sure DL TFS count is within UE Capabilities */
  if(rrcllc_check_dl_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}
/*====================================================================
FUNCTION        delete_unused_trch()

DESCRIPTION  This function deleted the TRCh which is not mapped to any RBs

DEPENDENCIES 
NONE

RETURN VALUE  
None

SIDE EFFECTS
====================================================================*/
void delete_unused_trch
(
  rrcllc_unmap_rb_e_type config_db
)
{
  uint8 trch_idx;
  uint8 pre_ndlchan = 0;


  uint8 dflow_index = 0;
  ordered_config_type   *tmp_config_ptr = NULL;
   /* start with OC */
  
  /* Doesnt delete MAC-HS and MAC-EHS at present */
  if (config_db == OC_ONLY)
  {
    tmp_config_ptr = ordered_config_ptr;
  }
  else if (config_db == TOC_ONLY)
  {
    if (transition_config.toc_usage != TOC_INVALID)
    {
      tmp_config_ptr = transition_config.toc_ptr;
    }
    else
    {
      return;
    }
  }
  else
  {
    WRRC_MSG1_ERROR(" Did not expect config_db %d value",config_db);
    return;
  }
     


    /* fach_info, dl_dch_info, dsch_info, and ul_cctrch.dch_info.dch_info are all
       arrays of size UE_MAX_TRCH, so we will iterate through them all with this
       outer loop. */
  for(trch_idx = 0; trch_idx < UE_MAX_TRCH; trch_idx++)
  {
  
    /* Should we check this for a FACH channel also */
  
  /* DL FACH */
    pre_ndlchan = tmp_config_ptr->fach_info[trch_idx].ndlchan;
    if(tmp_config_ptr->dl_fach_trch_idx_in_use[trch_idx] == TRUE)
    {
      if(pre_ndlchan == 0)
      {
         /* Clear the RB Mapping info for this TrCH */
         rrcllc_init_fach_rb_mapping_info(tmp_config_ptr, trch_idx);
        
         /* Clear the TFS for this TrCH */
         rrcllc_init_dl_tfs_table(tmp_config_ptr, trch_idx);
         
         /* Update MAC count */
         tmp_config_ptr->mac_dl_parms.num_fachs--;
     
         /* Update L1 count */
         tmp_config_ptr->l1_dl_cctrch_parms.num_trch--;
      }
    }

   /* DL DCH */
    pre_ndlchan = tmp_config_ptr->dl_dch_info[trch_idx].ndlchan;
   
    if((tmp_config_ptr->dl_dch_trch_info[trch_idx].dch_present)  &&
       (tmp_config_ptr->dl_dch_trch_idx_in_use[trch_idx] == TRUE))
    {
      if(pre_ndlchan == 0)
      {
          WRRC_MSG2_HIGH("delete DL Trch  index %d for trch id %d",trch_idx,tmp_config_ptr->dl_dch_trch_info[trch_idx].trch_id);
         /* Clear the RB Mapping info for this TrCH */
          rrcllc_init_dl_dch_rb_mapping_info(tmp_config_ptr, trch_idx);
      
          /* Clear the TFS for this TrCH */
          rrcllc_init_dl_tfs_table(tmp_config_ptr, trch_idx);
      
          /* Update MAC count */
          tmp_config_ptr->mac_dl_parms.num_dchs--;
          /* Indicate that the old index is now available */
          if(rrcllc_deallocate_dl_dch_trch_idx(trch_idx) == FAILURE)
          {
            return ;
          }
          /* Update L1 count */
          tmp_config_ptr->l1_dl_cctrch_parms.num_trch--;

              /* Initialize local data */
          tmp_config_ptr->dl_dch_trch_info[trch_idx].dch_present = FALSE;
          tmp_config_ptr->dl_dch_trch_info[trch_idx].trch_id     = 0;

        
          
      }
    }


    /* UL DCH */
    pre_ndlchan = tmp_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
   
    if((tmp_config_ptr->ul_dch_trch_info[trch_idx].dch_present) &&
        (tmp_config_ptr->ul_dch_trch_idx_in_use[trch_idx] == TRUE))
    {
      if(pre_ndlchan == 0)
      {
         WRRC_MSG2_HIGH("delete UL Trch  index %d for trch id %d",trch_idx,tmp_config_ptr->ul_dch_trch_info[trch_idx].trch_id);
         rrcllc_init_ul_dch_rb_mapping_info(tmp_config_ptr, trch_idx);
   
   
        /* Clear the TFS for this TrCH */
        rrcllc_init_ul_tfs_table(tmp_config_ptr, trch_idx);
      
        /* Update UL TrCH counts now */
        /* MAC count */
   
        tmp_config_ptr->ul_cctrch.dch_info.ndchs--;
        /* Indicate that the old index is now available */
        if(rrcllc_deallocate_ul_dch_trch_idx(trch_idx) == FAILURE)
        {
          return ;
        }
#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH
        /* Remove the RLC size restriction info */
        rrcllc_init_dch_rlc_size_info(trch_idx);
#endif
     
        /* Fill holes and put all Transport channels in the first
          * ordered_config_ptr->ul_trch_cnt indices.
          */
        tmp_config_ptr->ul_tfcs.nchan--;
        /* L1 count */

        tmp_config_ptr->rrc_ul_cctrch_params.num_trch--;  
        tmp_config_ptr->ul_dch_trch_info[trch_idx].dch_present = FALSE;
        tmp_config_ptr->ul_dch_trch_info[trch_idx].trch_id     = 0;
   
        /* One less TrCH - update local, MAC and L1 data structures */
   
        
      }
    }
  }

  for(trch_idx = 0; trch_idx < MAX_E_DCH_MAC_D_FLOW; trch_idx++)
  {
    pre_ndlchan = tmp_config_ptr->mac_e_config.e_mac_d_flow_info[trch_idx].num_logch;
    if(tmp_config_ptr->mac_e_config.e_mac_d_flow_info[trch_idx].e_mac_d_flow_id != INVALID_DFLOW_ID)
    {
      if(pre_ndlchan ==0 )
      {
        WRRC_MSG2_HIGH("delete E-DCH  index %d for flow id %d",trch_idx,tmp_config_ptr->mac_e_config.e_mac_d_flow_info[trch_idx].e_mac_d_flow_id);
        rrcllc_init_e_dch_rbmapping_info(tmp_config_ptr, trch_idx);
        
      }
    }
  }
  /* Fill holes and put all Transport channels in the first
    * ordered_config_ptr->dl_trch_cnt indices.
    */
          
  rrcllc_fill_holes_in_dl_dch_trch_idx();
  /* Now is the time and place for sorting all transport channel
    * info based on TrCH IDs. Call the right function.
    */
  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                     ordered_config_ptr->mac_dl_parms.num_dchs
                                      );


  /*  tmp_config_ptr->mac_ul_parms.tfcs_ptr->nchan --;*/
   
  rrcllc_fill_holes_in_ul_dch_trch_idx();
    
  /* Now is the time and place for sorting all transport channel
    * info based on TrCH IDs. Call the right function.
    */
  rrcllc_sort_ul_dch_trch_index_array(&tmp_config_ptr->ul_dch_trch_info[0], 
                                            (RRCLLC_UPLINK_TRCH_COUNT(tmp_config_ptr)));
        
  rrcllc_fill_holes_in_e_mac_d_flow_idx();
  
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) 
  {      
    for (dflow_index = 0; dflow_index < UE_MAX_MAC_D_FLOW; dflow_index++)
    {
      /* DL HSDSCH */
      pre_ndlchan = tmp_config_ptr->mac_dflow_info[dflow_index].ndlchan;
      if(tmp_config_ptr->dl_dflow_index_in_use[dflow_index] == TRUE)
      {
        if(pre_ndlchan ==0 )
        {
             WRRC_MSG2_HIGH("delete MAC DFLOW  index %d for trch id %d",dflow_index,tmp_config_ptr->mac_dflow_info[dflow_index].mac_dflow_id);
          /* Should we delete a MAC-DFLOW*/
        }
      }
      
    }
  }

  /* UL RACH  */
  /* ul_cctrch.rach_info, on the other had, is just a single structure, so we
     take care of it outside the loop above. */
  pre_ndlchan = tmp_config_ptr->ul_cctrch.rach_info.ndlchan;
  
   if(pre_ndlchan == 0 )
   {
     rrcllc_init_rach_rb_mapping_info(ordered_config_ptr);
   }

   
}


/*===========================================================================

FUNCTION    rrcllc_get_rb_id_list

DESCRIPTION This function gets the rb_ids given in the OTA(add/reconf and release included)
 
DEPENDENCIES
  None.

RETURN VALUE
 void

===========================================================================*/

void rrcllc_get_rb_id_list( uint8 *rb_id_list, rrc_RB_InformationReleaseList  *rb_release_ptr)
{
  uint8 index;
  uint8 rb_id = MAX_RB ;

  /*DL_UM*/
  for(index=0;index < ordered_config_ptr->rlc_dl_um_parms.nchan; index++)
  {
    rb_id = ordered_config_ptr->rlc_dl_um_parms.chan_info[index].rb_id;
    if((rb_id >= MAX_RB) || (rb_id <= CCCH_RADIO_BEARER_ID))
      continue;

    // We shall not consider data RB's , refer to CR-360712
    if(rb_id > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      continue;

    rb_id_list[rb_id] = TRUE;
  }

  /*AM*/
  for(index=0;index < ordered_config_ptr->rlc_am_parms.nentity; index++)
  {
    rb_id = ordered_config_ptr->rlc_am_parms.am_info[index]->common_config.rb_id;
    if((rb_id >= MAX_RB) || (rb_id <= CCCH_RADIO_BEARER_ID))
      continue;
    rb_id_list[rb_id] = TRUE;
  }

  /* DL TM*/
  for(index=0;index < ordered_config_ptr->rlc_dl_tm_parms.nchan; index++)
  {
    rb_id = rrclcm_get_dl_rb_id(ordered_config_ptr->rlc_dl_tm_parms.chan_info[index].lc_id);
    if((rb_id >= MAX_RB) || (rb_id <= CCCH_RADIO_BEARER_ID))
      continue;
    rb_id_list[rb_id] = TRUE;
  }
  
  if(rb_release_ptr == NULL )
    return;
  if(rb_release_ptr->n >=MAX_RB)
    return;


  for(index =0 ;((rb_release_ptr->n > index) && (index < MAX_RB));index++)
  {
    rb_id_list[rb_release_ptr->elem[index]] = FALSE;
  }
  
}

/*===========================================================================

FUNCTION    rrcllc_check_trch_mapped_to_rb_present

DESCRIPTION This function checks if the Trch mapped to RBs 
 
DEPENDENCIES
  None.

RETURN VALUE
 SUCCESS or FAILURE

===========================================================================*/
uecomdef_status_e_type rrcllc_check_trch_mapped_to_rb_present
(
  rrc_RB_InformationReleaseList  *rb_release_ptr
)
{
  uint8 cnt,trch_idx;
  uint8 index;
  boolean rb_ul[MAX_RB]={FALSE},rb_dl[MAX_RB]={FALSE};

  rrcllc_get_rb_id_list(rb_dl,rb_release_ptr);
  WCDMA_MEMCPY(rb_ul,
               sizeof(rb_ul),
               rb_dl,
               sizeof(rb_dl));
    
  for(trch_idx=0;trch_idx< UE_MAX_TRCH;trch_idx++)
  {
    /* DCH-DL*/
    if((ordered_config_ptr->dl_dch_trch_info[trch_idx].dch_present) &&
        (ordered_config_ptr->dl_dch_trch_idx_in_use[trch_idx] == TRUE) )
    {
      for (cnt = 0; cnt < ordered_config_ptr->dl_dch_info[trch_idx].ndlchan;cnt++) 
      {
        rb_dl[ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[cnt].rb_id] = FALSE;
      }
    }

    /* DCH-UL*/
    if((ordered_config_ptr->ul_dch_trch_info[trch_idx].dch_present) &&
        (ordered_config_ptr->ul_dch_trch_idx_in_use[trch_idx] == TRUE))
    {
      for (cnt = 0; cnt < ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan; cnt++) 
      {
        rb_ul[ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnt].rb_id]=FALSE;
      }
    }
  }
  
  /*MAC-HS*/
  for(index = 0; index < UE_MAX_MAC_D_FLOW;  index++)
  {
    if((ordered_config_ptr->dl_dflow_index_in_use[index] == TRUE) && 
       (ordered_config_ptr->mac_dflow_info[index].no_of_queues > 0))
    {
      for (cnt = 0; cnt < ordered_config_ptr->mac_dflow_info[index].ndlchan; cnt++) 
      {
        rb_dl[ordered_config_ptr->mac_dflow_info[index].dlchan_info[cnt].rb_id] = FALSE;
      }
    }
  }
  
  /*MAC-EHS*/
  for(index = 0; index < UE_MAX_DL_LOGICAL_CHANNEL;  index++)
  {
    if(ordered_config_ptr->mac_ehs_lc_id_in_use[index] != TRUE)
      continue;
    /*Now we got the DL logical channel for some SRB*/
    for (cnt = 0; cnt < UE_MAX_HS_QUEUE; cnt++) 
    {
      /*Check whether Q-ID for this RB is present or not.*/
      if((ordered_config_ptr->mac_ehs_queue_index_in_use[cnt] == TRUE) &&
         (ordered_config_ptr->dl_macehs_logchan_info[index].queue_id == 
           ordered_config_ptr->mac_ehs_queue_info[cnt].queue_id))
      {
        rb_dl[ordered_config_ptr->dl_macehs_logchan_info[index].rb_id] = FALSE;
      }
    }
  }

  /*EUL*/
  if(ordered_config_ptr->l1_e_info.e_ul_info.e_tti !=  NUM_E_TTI)
  {
    for(index = 0; index < ordered_config_ptr->mac_e_config.num_e_mac_d_flow;  index++)
    {
      for (cnt = 0; cnt < ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].num_logch; cnt++) 
      {
        if((ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[cnt].num_rlc_size_ddi_info)
#ifdef FEATURE_MAC_I
             || (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[cnt].rlc_pdu_type == RLC_FLEXIBLE_PDU_SIZE)
#endif
        	)
       	{
       	  /* If SRBs are mapped to EDCH and if EDCH transmission is FALSE, do not set rb_ul to FALSE */
	  /* If SRBs are mapped only to EDCH, rb_ul will be TRUE and we will fail the validation and reject the message */
       	  if((ordered_config_ptr->e_dch_transmission) || 
          (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[cnt].rb_id > DCCH_DT_HIGH_PRI_RADIO_BEARER_ID))
          {
            rb_ul[ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[cnt].rb_id]=FALSE;
          }
        }
      }
    }
  }

  for(index=0;index < MAX_RB ; index++)
  {
    if(rb_dl[index])
    {
      WRRC_MSG1_ERROR("Invalid RB configuration for RB_ID:%d in DL",index);
      return(FAILURE);
    }
    if(rb_ul[index])
    {
      WRRC_MSG1_ERROR("Invalid RB configuration for RB_ID:%d in UL",index);
      return(FAILURE);
    }
  }
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_check_if_tfi_present_in_allowed_list_in_tfcc()

DESCRIPTION:
  This function checks if the TFI for a Trch is present
  in the allowed list in Restricted transport channel list
  It takes a Transport channel index and  TFCI index and the 
  Restricted transport channel list from the TFCC message with which MAC UL is configured
DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS : If the TFI is present in the Allowed list  or the Trch ID
            itself was not present in the Restricted transport channel list
  else   FAILURE
=============================================================================*/
uecomdef_status_e_type rrcllc_check_if_tfi_present_in_allowed_list_in_tfcc
(
  ordered_config_type* config_ptr,
  mac_tfc_subset_restriction_info_type* restricted_info,
  int tfci_index,
  int trch_index
)
{
  int i,j;
  boolean trch_id_in_restricted_list = FALSE;
  uint8 tfi;
  i=j=0;
  while(i<restricted_info->restriction_info.trch_restriction_info_list.ntrch)
  {
    if((config_ptr->ul_dch_trch_info[trch_index].dch_present) &&
      (restricted_info->restriction_info.trch_restriction_info_list.trch_restriction_info[i].trch_type == DCH)&&
      (config_ptr->ul_dch_trch_info[trch_index].trch_id == 
         restricted_info->restriction_info.trch_restriction_info_list.trch_restriction_info[i].trch_id))
    {
      /* The Trch id is present in the Restriched Trch List in OTA */
      trch_id_in_restricted_list = TRUE;
      if(restricted_info->restriction_info.trch_restriction_info_list.trch_restriction_info[i].ntf)
      {
        j=0;
        while(j< restricted_info->restriction_info.trch_restriction_info_list.trch_restriction_info[i].ntf)
        {
          /* Since the TFI is present in the allowed TFI list in OTA, return success */
          if( config_ptr->ul_tfcs.tfc_info[tfci_index].tfi[trch_index] == 
              restricted_info->restriction_info.trch_restriction_info_list.trch_restriction_info[i].tfi[j])
          {
            MSG_HIGH("TFCC: Found the Allowed TF index %d for trch_idx %d in TFCI %d",j,trch_index,tfci_index);
            return SUCCESS;
          }
          j++;
        }
      }
      else
      {
        /* If the Allowed List for this Trch is not present, 
          * then TB size 0 shld be treated as ALLOWED, all other TFIs should be
          * NOT ALLOWED
          */
        tfi =config_ptr->ul_tfcs.tfc_info[tfci_index].tfi[trch_index];
              
        if(config_ptr->ul_tfcs.tfs_info[trch_index].tf_info[tfi].ntblks *
          config_ptr->ul_tfcs.tfs_info[trch_index].tf_info[tfi].tb_size == 0)
        { 
         /*No allowed list, but ALLOWED TFI as TB Size =0 */
         WRRC_MSG1_HIGH("TFCC: No allowed TF's for trch_idx %d, Allowed as zero",trch_index);
          return SUCCESS;
        } 
      }
    }
    if(trch_id_in_restricted_list == TRUE )
    {
      break;
    }
    i++;
  }
  if(trch_id_in_restricted_list == FALSE)
  {
    return SUCCESS;
  }
  else
  {
    /* We return FAILURE as, the Trch ID was present in the 
      * Restriched Trch List in OTA, but not in the allowed TFI list
      */  
    WRRC_MSG0_ERROR("TrCh ID present in the Restricted TrCh List");
    return FAILURE;
  }
}


/*===========================================================================

FUNCTION   RRCLLC_UPDATE_TFCC_RESTRICTION_IN_CC_OC

DESCRIPTION
  This function updates the TFCC restriction in RRC config db
  based on state.
  If RRC state is FACH, restriction info is updated in TOC
  If RRC state is DCH, restriction info is updated in CC
  If RRC state is DCH and OC is set for DCH and OC doesn't have the 
  new UL TFCS list from OTA, restriction info is also updated in OC
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/  
void rrcllc_update_tfcc_restriction_in_cc_oc
(
  mac_tfc_subset_restriction_info_type* restricted_info
)
{

  int i,j=0;
  ordered_config_type * config_ptr;
  if(rrc_get_state() == RRC_STATE_CELL_FACH)
  {
    /*TFCC message is received in FACH state*/
    if(transition_config.toc_usage == TOC_FOR_DCH)
    {
      WRRC_MSG0_HIGH("TFCC: Updating the TFCC restriction info to TOC");
      config_ptr= transition_config.toc_ptr;
    }
    else
    {
      WRRC_MSG1_HIGH("TFCC: TOC usage is %d, do not update in TOC",transition_config.toc_usage);
      return;
    }
  }
  else
  {
    /*TFCC message is received in DCH state*/
    WRRC_MSG0_HIGH("TFCC: Updating the TFCC restriction info to CC");
    config_ptr = current_config_ptr;
  }

  switch(restricted_info->tfci_restriction_choice)
  {
    case MIN_ALLOWED_TFCI:
      for(i=0;i<config_ptr->ul_tfcs.ntfc;i++)
      {
        if(config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_NOT_ALLOWED;
        }
      }
      for(i=0;i<=restricted_info->restriction_info.tfci;i++)
      {
        config_ptr->ul_tfcs.tfc_info[i].control_flag =TFC_ALLOWED;
      }
      break;
    case ALLOWED_TFCI:
      i = 0;
      while(i < config_ptr->ul_tfcs.ntfc)
      {
        if(config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_NOT_ALLOWED;
        }
        i++;
      }
      i=0;
      while(i<restricted_info->restriction_info.tfci_restriction_info.ntfc)
      {
        config_ptr->ul_tfcs.tfc_info[restricted_info->restriction_info.tfci_restriction_info.tfci_list[i]].
            control_flag = TFC_ALLOWED;
        i++;
      }
      break;
    case NOT_ALLOWED_TFCI:
      i = 0;
      while(i < config_ptr->ul_tfcs.ntfc)
      {
        if(config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_ALLOWED;
        }
        i++;
      }
      i=0;
      while(i<restricted_info->restriction_info.tfci_restriction_info.ntfc)
      {
        config_ptr->ul_tfcs.tfc_info[restricted_info->restriction_info.tfci_restriction_info.tfci_list[i]].
            control_flag = TFC_NOT_ALLOWED;
        i++;
      }
      break;
    case RESTRICTED_TFCI:
        /* i indicates the tfci index */
      for(i=0;i<config_ptr->ul_tfcs.ntfc && (i < UE_MAX_TFC);i++)
      {
        boolean tfci_allowed = TRUE;
        /* j represents each Trch index */
        for(j=0;j< config_ptr->ul_tfcs.nchan && (j < UE_MAX_TRCH); j++)
        {
          if(rrcllc_check_if_tfi_present_in_allowed_list_in_tfcc(config_ptr,restricted_info,i,j)
               == FAILURE)
          {
            tfci_allowed = FALSE;
            break;
          }     
        }
          
        if(config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        { 
          if(tfci_allowed == FALSE )
          {
            config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_NOT_ALLOWED;  
          }
          else
          {
            config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_ALLOWED;
          }
        }
      }
      break;
    case FULL_TFCI:
      i = 0;
      while(i < config_ptr->ul_tfcs.ntfc)
      {
        if(config_ptr->ul_tfcs.tfc_info[i].control_flag != TFC_REMOVED)
        {
          config_ptr->ul_tfcs.tfc_info[i].control_flag = TFC_ALLOWED;
        }
        i++;
      }
      break;
  }
  /*
    If a OTA is received during TFCC handling taking UE from DCH to DCH,
    and the OTA doesnt change TFCS, then update OC with the restrcition info of TFCC
  */
  if((config_ptr_in_use == ORDERED_CONFIG)&&
    (rrc_get_state()==RRC_STATE_CELL_DCH)&&
    (ordered_config.set_status == OC_SET_FOR_CELL_DCH) &&
    (!ordered_config_ptr->mac_ul_parms.new_tfcs_list_present))
  {
    WCDMA_MEMCPY(&ordered_config_ptr->ul_tfcs,
                 sizeof(mac_ul_tfcs_type),
                 &config_ptr->ul_tfcs, 
                 sizeof(mac_ul_tfcs_type));
  }
}

/*==========================================================
FUNCTION  :   rrcllc_update_l1_with_ul_trch()

DESCRIPTION : This function updates the L1 info in Config ptr with 
              UL Trch and UL TFCS info if UL DPDCH is present

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
void 
rrcllc_update_l1_with_ul_trch
(
  ordered_config_type * config_ptr
)
{
  if( config_ptr->l1_ul_chan_parms.dpch_cfg.num_dpdch > 0)
  {
    WRRC_MSG2_MED("Update l1 with UL Trch info num trch %d, numtfci %d",
        config_ptr->rrc_ul_cctrch_params.num_trch,
        config_ptr->rrc_ul_cctrch_params.num_tfci);
    
    config_ptr->l1_ul_cctrch_parms.num_tfci = config_ptr->rrc_ul_cctrch_params.num_tfci;
    config_ptr->l1_ul_cctrch_parms.num_trch = config_ptr->rrc_ul_cctrch_params.num_trch;
  }
  else
  {
     WRRC_MSG0_MED("NUM DPCH zero");
     config_ptr->l1_ul_cctrch_parms.num_tfci = 0;
     config_ptr->l1_ul_cctrch_parms.num_trch = 0;
  }
}

/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_r7()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
static uecomdef_status_e_type get_dl_added_reconfig_trch_r7
(
  rrc_DL_AddReconfTransChInformation_r7 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList_r7 * ul_ptr_r7
 
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = 0;
  tr_ch_id_type ul_trch_idx = 0;
  uint32 idx =0,idy=0;

  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  
  /* Loop variables for copying UL TFS info */
  uint8 tf_idx = 0;
  /*lint -e578 Declaration of symbol 'rrc_dl_tf_info' hides symbol 'rrc_dl_tf_info'*/
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  
  /* Boolean used to indicate if multiple logical channels are mapped to
  a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;
  
  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;
  
  rrc_MAC_hs_AddReconfQueue_List * add_queue_list_ptr = NULL;
  
  rrc_MAC_hs_DelQueue_List * delete_queue_list_ptr = NULL;
  
  
  MSG_MED("Get DL Add/Reconf TrCH",0,0,0);
  
  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    /* Get the transport channel index for this transport channel ID.
      * Note: The index should already be there. */
    WRRC_MSG1_HIGH("RRCHS:TrCh type to be added %d",ie_ptr->dl_TransportChannelType.t);
    
    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
    rrc_DL_TrCH_TypeId1_r5_dch))
      {
        
        trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
        
        if(trch_idx == RRCLLC_INVALID_TRCH_IDX)
        {
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found",
            ie_ptr->dl_TransportChannelType.u.dch);
          return(FAILURE);
        }

        if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
        {
          /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping",
               ie_ptr->dl_TransportChannelType.u.dch);
        } 
      }
      
      /* Determine if multiple dedicated logical channels are present on this TrCH id */
      if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
      {
        multiple_ded_log_chan = TRUE;
      }

    /*
      * Get whether or not the TFS for a given TRCH is included or if it needs
      * to come from an uplink TrCH ID.
      */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_explicit_config:
        
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
          &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
          multiple_ded_log_chan, /* mulitple logical channels? */
          &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
          ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */
        
      case T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_sameAsULTrCH:

        while(ul_ptr_r7 != NULL && ul_ptr_r7->n > idx)
        {
          /* check whether trCh is of type rrc_UL_AddReconfTransChInformation_r6_dch_usch */
          if((RRC_CHECK_LIST_MSG_TYPE(ul_ptr_r7,rrc_UL_AddReconfTransChInformation_r7_dch_usch)) &&
             (ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity == 
              ul_ptr_r7->elem[idx].u.dch_usch->transportChannelIdentity))
          { 
            /* Got a match with Uplink TrCH ID */
            WRRC_MSG2_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
              ul_ptr_r7->elem[idx].u.dch_usch->transportChannelIdentity);
            if(get_dl_transport_format_set(&ul_ptr_r7->elem[idx].u.dch_usch->transportFormatSet,
              &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
              multiple_ded_log_chan, /* mulitple logical channels? */
              &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
              ) == FAILURE)
            {
              return FAILURE;
            }
            
            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */
          
          /* no match, keep looking */
          else 
          {
            idx++;
          }
        }
               
        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
          * Note: The index should already be there.
          */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
            ordered_config_ptr,
            (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
            ul_TransportChannelIdentity);
          if(ul_trch_idx == RRCLLC_INVALID_TRCH_IDX)
          {
            WRRC_MSG2_ERROR("No DL TFS for TrCH %d, to use UL TrCH %d for TFS",
              ie_ptr->dl_TransportChannelType.u.dch,
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
              ul_TransportChannelIdentity);
            return(FAILURE);
          }
          
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
            case L1_UL_CODING_NONE:
              dl_tfs_info->coding_type = L1_DL_UNCODED;
              break;
            case L1_UL_CODING_CONV_ONE_HALF:
              dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
              break;
            case L1_UL_CODING_CONV_ONE_THIRD:
              dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
              break;
            case L1_UL_CODING_TURBO:
              dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
              break;
            default:
              break; 
          }
          
          
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
            case L1_UL_CRC_0_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
              break;
            case L1_UL_CRC_8_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
              break;
            case L1_UL_CRC_12_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
              break;
            case L1_UL_CRC_16_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
              break;
            case L1_UL_CRC_24_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
              return FAILURE;
          }
          
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8) ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;
            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);

            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if ((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }

            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 
        
      case T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_hsdsch:
        /* The DFLOW Index has already been added. So get the index */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
   rrc,dl_MAC_HeaderType))
        {
          if (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.t == T_rrc_HSDSCH_Info_r7_dl_MAC_HeaderType_mac_hs)
          {
  
            /* Check if add & delete queue both are present and network specifies the same 
            queue id in both. This needs to be rejected */
            if ((ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->m.
                mac_hs_AddReconfQueue_ListPresent) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List.n > 0) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->m. 
                mac_hs_DelQueue_ListPresent) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_DelQueue_List.n > 0))
            {

              /* Check if UTRAN messed up by specifying the same queue id */
              delete_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_DelQueue_List;
              idx =0;
              while (delete_queue_list_ptr->n > idx)
              {
                add_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List;
                idy =0;
                while (add_queue_list_ptr->n > idy)
                {
                  if (delete_queue_list_ptr->elem[idx].mac_hsQueueId == add_queue_list_ptr->elem[idy].mac_hsQueueId)
                  {
                    /* Invalid Config */
                    WRRC_MSG1_ERROR("RRCHS:Queue %d simul added & deleted", delete_queue_list_ptr->elem[idx].mac_hsQueueId);
                    return FAILURE;
                  }
                  idy++;
                }
                
                idx++;
              }
              
            }
            
            if (set_dflow_info(ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs) == FAILURE)
            {
              WRRC_MSG0_ERROR("RRCHS:Invalid DFLOW info");
              return FAILURE;
            }
          }
        }
        /* SET HARQ info in L1 structure */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
   rrc_HSDSCH_Info_r7,harqInfo))
        {
          MSG_MED("RRCHS:HARQ info present",0,0,0);
          if (set_harq_information_r7(&ie_ptr->tfs_SignallingMode.u.hsdsch->harqInfo) == FAILURE)
          {
            WRRC_MSG0_ERROR("RRCHS:Unable to set HARQ information");
            return FAILURE;
          }
        }
        
        /* Indicate that HSDSCH Info is present which would be used later to set bit mask when HSDPA
        is already ACTIVE */
        hsdpa_msg_params.hsdsch_info_present = TRUE;
        
        hsdpa_msg_params.msg_params_present = TRUE;
        
        break;
        
      default:
        return FAILURE;
      
    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
        PDU size has changed.*/ 
    if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId1_r5_dch))
    {
      rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch, 
                ordered_config_ptr->rrc_state_indicator, DL);  
    }
  } /* end CELL_DCH check */
  
  /* States not supported */
  else
  {
    WRRC_MSG1_ERROR("State %d not supported for TrCH",ordered_config_ptr->rrc_state_indicator);
    return FAILURE;
  }
  
  /***************************************************************/
  /* Transport Channel Count has been updated in RB Mapping info */
  /***************************************************************/
  if (!RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->tfs_SignallingMode,
   rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_hsdsch) )
  {
    /* Check for the BLER */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation_r7,dch_QualityTarget))
    {
      /* Save the BLER if found, and is in range */
      if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
        (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER))
      {
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
          (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
      }
      
      /* BLER is out of range */
      else
      {
        /* According to L1 I/F, must set an invalid value of -127 */
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
        WRRC_MSG1_ERROR("DL BLER %d is out of range",
          ie_ptr->dch_QualityTarget.bler_QualityValue
          );
        return FAILURE;
      }
    } /* end if BLER present */
    else
    {
      /* IE omitted. Set the target to -127 which would result 1% of BLER target.
       * This is good UE behaviour though the spec is ambiguous on what UE
       * behavior should be in this case.  
       */
      WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
    }
  }
  else
  {
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation_r7,dch_QualityTarget))
    {
      WRRC_MSG0_HIGH("Ignoring dch quality target for HS");
    }
  }
  /* Indicate that DL L1 needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
  
  return SUCCESS;
}
/*============================================================================
FUNCTION: rrcllc_store_dl_add_reconf_trch_info_list_r6()

DESCRIPTION:
  This function updates IE "dl added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_dl_add_reconf_trch_info_list_r7
(
  OSS_UINT32 dl_add_reconf_trch_info_list_present,
  rrc_DL_AddReconfTransChInfoList_r7 *dl_trch_add_reconfig_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r7  * ul_trch_add_reconfig_ptr  
)
{
  uint32 idx, tf_idx;
  /* Get the DL Added or Reconfigured Transport Channel information */
  if(dl_add_reconf_trch_info_list_present)
  {
      /* local UL TrCH Add Reconfig pointer */
  
    idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (dl_trch_add_reconfig_ptr->n > idx))
    {
      if(get_dl_added_reconfig_trch_r7(&dl_trch_add_reconfig_ptr->elem[idx],
          ul_trch_add_reconfig_ptr
        ) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;
    } /* end while */
  } 
  
  /* At this point store the Queue Information in L1 structure. To add later if HSDSCH config changed */
  
  /* Adjust TB size on every TrCH if necessary. */ 
  for (idx=0; idx<UE_MAX_TRCH; idx++)
  {
  /* The number of logical channel mapped to this transport channel 
  increases from 1 to 2. Needs to adjsut the tb_szie to
    reflect the adding of the MAC header.                         */
    if (ordered_config_ptr->dl_dch_trch_info[idx].dch_present &&
      (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up ||
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down))
    {
      /* TrCH IE is present and the TrCH ID is appear in the TrCH info table. */
      if (!(dl_add_reconf_trch_info_list_present &&
        find_trch_id_in_config_trch_list_r7(dl_trch_add_reconfig_ptr,
        ordered_config_ptr->dl_dch_trch_info[idx].trch_id)))
      {
        /* Added MAC head size to the current TB size.  */
        if (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          !ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Increasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size += 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
        /* Substract MAC head size from the current TB size.  */
        else if (!ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Decreasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size -= 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
      }
      WRRC_MSG0_HIGH("Clean TB size adjust flag in RBSetup");
      /* Reset lg_ch_cnt_up and lg_ch_cnt_down. */
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up = FALSE;
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down = FALSE;
    }
    else if (!ordered_config_ptr->dl_dch_trch_info[idx].dch_present)
    {
    /* Since the activated transport channels are sorted before stored in
      the memory, break the looping after reaching the 1st invalid block.  */
      break;
    }
  }
  
  /* Check to make sure DL TFS count is within UE Capabilities */
  if(rrcllc_check_dl_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}
  
/*=============================================================================
FUNCTION: get_dl_deleted_trch_r7()

DESCRIPTION:
  This function takes a pointer to the Downlink Deleted 
  Transport Channel IE and Deletes if from Ordered Config.
  In the TFS it resets all information for this TrCH.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
===============================================================================*/
static uecomdef_status_e_type get_dl_deleted_trch_r7
(
  rrc_DL_TransportChannelIdentity_r7* ie_ptr
)
{
  /* TrCH Index */
  uint8 ix;
  uint8 i;
  if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId2_r7_dch))
  {
    ix = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
    if(ix == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG1_HIGH("No DL DCH index for TrCH ID %d",
        ie_ptr->dl_TransportChannelType.u.dch);
      return(SUCCESS);
    }
    rrcllc_backup_rbmapping_info(ix,DL_DCH_RB_MAPPING );

    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_dl_dch_rb_mapping_info(ordered_config_ptr, ix);
    
    /* Clear the TFS for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, ix);
    
    /* Update MAC count */
    ordered_config_ptr->mac_dl_parms.num_dchs--;

    /* Initialize local data */
    ordered_config_ptr->dl_dch_trch_info[ix].dch_present = FALSE;
    ordered_config_ptr->dl_dch_trch_info[ix].trch_id     = 0;
  
    /* Indicate that the old index is now available */
    if(rrcllc_deallocate_dl_dch_trch_idx(ix) == FAILURE)
    {
      return FAILURE;
    }
    /* Update L1 count */
    ordered_config_ptr->l1_dl_cctrch_parms.num_trch--;
    /* set dl_tfcs_needed flag to true.  Later if common info is not preset,
     * UE needs to reject the message 
     */
    dl_tfcs_needed = TRUE;
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId2_r7_hsdsch))
  {
    if(ie_ptr->dl_TransportChannelType.u.hsdsch->t == T_rrc_DL_TrCH_TypeId2_r7_hsdsch_mac_hs )
    {
      if ((ix = rrcllc_find_dflow_index((uint8)ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_hs)) >= UE_MAX_MAC_D_FLOW)
      {
        WRRC_MSG1_ERROR("RRCHS:Unable to index for DFLOW %d",ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_hs);
        /* Allow the Reconfig Msg to be processed since spec allows it implicitly */
        return(SUCCESS);
      }
      /* Clear the Queue Information associated with the DFLOW*/
      rrcllc_backup_rbmapping_info(ix,MAC_D_RB_MAPPING );
      
      /* Initialize the queue info */
      rrcllc_initialize_queue_info(ix);
  
      hsdpa_msg_params.msg_params_present = TRUE;
  
      hsdpa_msg_params.hsdsch_info_present = TRUE;
     /* If the Flow id was actually not deleted, that is the Rb-Mapping for this flow id is still present in the 
      * ordered config then delete it from Backup info
      */
      if ((ix = rrcllc_find_dflow_index((uint8)ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_hs)) == UE_MAX_MAC_D_FLOW)
      {
        WRRC_MSG1_HIGH("Mac-d flow id %d got deleted ",(ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_hs));
      }
      else
      {
        for(i=0;i<ordered_config_ptr->mac_dflow_info[ix].ndlchan;i++)
        {
          rrcllc_free_hanging_rb_mapping_info_index(ordered_config_ptr->mac_dflow_info[ix].dlchan_info[i].rb_id, MAC_D_RB_MAPPING);
        }
      }
    }
    else if(ie_ptr->dl_TransportChannelType.u.hsdsch->t == T_rrc_DL_TrCH_TypeId2_r7_hsdsch_mac_ehs )
    {

       hsdpa_msg_params.msg_params_present = TRUE;
  
      hsdpa_msg_params.hsdsch_info_present = TRUE;
            
      if (rrcllc_delete_queue_from_macehs(ordered_config_ptr, ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_ehs) == FAILURE)
      {
        WRRC_MSG1_ERROR("MACEHS:Delete queue %d not found",ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_ehs);
        return SUCCESS;
      }
    }
  }  
  else  /* Must be FACH */
  {
    // ix = rrcllc_find_dl_fach_trch_idx(ie_ptr->value.dl_TransportChannelIdentity); 
    ix = rrcllc_find_dl_fach_trch_idx();
    if(ix == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG0_ERROR("No FACH index for TrCH ID");
      return(FAILURE);
    }
    rrcllc_backup_rbmapping_info(ix,FACH_RB_MAPPING );

    /* Clear the RB Mapping info for this TrCH */
    rrcllc_init_fach_rb_mapping_info(ordered_config_ptr, ix);
    
    /* Clear the TFS for this TrCH */
    rrcllc_init_dl_tfs_table(ordered_config_ptr, ix);
    
    /* Update MAC count */
    ordered_config_ptr->mac_dl_parms.num_fachs--;

    /* Update L1 count */
    ordered_config_ptr->l1_dl_cctrch_parms.num_trch--;
  }
  
  return SUCCESS;
}
/*================================================================================================
FUNCTION: process_added_reconfig_e_dch_info_r7()

DESCRIPTION:
  This function takes a pointer to the EDCH Added or Reconfigured 
  mac-d flow info and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:

===============================================================================================*/
static uecomdef_status_e_type process_added_reconfig_e_dch_info_r7
(
  rrc_UL_AddReconfTransChInformation_r7 * ie_ptr
)
{

  /* local transport channel variables */
  uint8 e_mac_d_flow_idx = MAX_E_DCH_MAC_D_FLOW;
  e_tti_enum_type temp_tti;
  rrc_E_DCH_AddReconf_MAC_d_FlowList_r7 * e_mac_d_flow_list_ptr = NULL;
  uint32 idx=0;
  if (!(RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r7_e_dch)))
  {
    WRRC_MSG1_ERROR("Un-wanted call to function to process e_dch_info. t: %d",
      ie_ptr->t);
    return(FAILURE);
  }
  /*Added NULL check to bypass the KW error*/
  if(ie_ptr->u.e_dch == NULL)
  {
    WRRC_MSG0_ERROR("UL TrCH info of EDCH is invalid (NULL)");
    return(FAILURE);
  }

#ifdef FEATURE_MAC_I
  ordered_config_ptr->mac_ul_parms.ul_pdu_type = MAC_E_PDU_HDR;
#endif /* FEATURE_MAC_I */	

  /*Update the TTI info in MAC and L1 struct. 10ms or 2ms */
  /* Check with stored TTI value before indicating reconfig to L1/MAC*/
  if(ie_ptr->u.e_dch->modeSpecific.t == T_rrc_UL_AddReconfTransChInformation_r7_modeSpecific_fdd )
  {
    temp_tti = ((ie_ptr->u.e_dch->modeSpecific.u.fdd->tti == rrc_E_DCH_TTI_tti10) ? E_TTI_10MS : E_TTI_2MS);
  
    if(temp_tti == E_TTI_2MS)
    {
      /* Check to see NV HSUPA Category.
       * For supporting 2msec TTI, Below are the valid cats
       * Cat - 2,4,6,7,8,9 from table Table 5.1g: FDD E-DCH physical layer categories
       * in 25.306.If N/W is trying to configure 2msec TTI with any other 
       * UE cat Bcoz of N/W issue, Reject this configuration
       */
      if(
         (wcdma_nv_hsupa_category == 0)||
         (wcdma_nv_hsupa_category == 1)||
         (wcdma_nv_hsupa_category == 3)||
         (wcdma_nv_hsupa_category == 5)
        )
      {
        WRRC_MSG1_ERROR("RRCEUL:Error N/W try to config 2msec TTI when UE cat = %d which is not 2 or 4 or 6", 
          wcdma_nv_hsupa_category);
        return(FAILURE);
      }
    }
  
    /*We store e-tti & harq_rv_config which are trch i.e in L1 data base,
    *hence init them when previously EUL was not active
    */
    if(current_config_ptr->e_dch_transmission == FALSE)
    {
      WRRC_MSG2_HIGH("RRCEUL:init e_tti in l1 db, prv value =%d,harq_rv_config=%d", 
       ordered_config_ptr->l1_e_info.e_ul_info.e_tti, 
       ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config);

      ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_NOT_PRESENT;
      ordered_config_ptr->l1_e_info.e_ul_info.e_tti = NUM_E_TTI;
      ordered_config_ptr->l1_e_info.e_dl_info.e_tti = NUM_E_TTI;
    }

    if (temp_tti != ordered_config_ptr->l1_e_info.e_ul_info.e_tti) 
    {
      rrcllc_update_mac_eul_action(TRUE);
      rrcllc_update_l1_e_req_mask_for_e_dl_info(TRUE);
      rrcllc_update_l1_e_req_mask_for_e_ul_info(TRUE);

      /*Set all E-DCH active set DL RL's bit mask to reconfig, 
       *This is used by L1 to reconfigure AGCH/RGCH/HICH
       *Channels at TTI reconfiguration
       */
      rrcllc_update_e_dl_channel_mask_for_e_active_set_rl_to_reconfig();

      ordered_config_ptr->mac_e_config.e_tti = temp_tti;
      ordered_config_ptr->l1_e_info.e_ul_info.e_tti = temp_tti;
      ordered_config_ptr->l1_e_info.e_dl_info.e_tti = temp_tti;
      if (current_config_ptr->mac_e_config.e_tti != NUM_E_TTI) 
      {
        WRRC_MSG1_HIGH("RRCEUL: Setting L1/MAC action cuz TTI RECONFIG (tti: %d)",
          temp_tti);
        ordered_config_ptr->mac_e_reset_indicator = TRUE;
      }
    }
  }
  /* Update the harq info if present */
  if (ie_ptr->u.e_dch->harq_Info == rrc_UL_AddReconfTransChInformation_r7_harq_Info_rv0)
  {
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_0;
  }
  else
  {
    /* Default value is 'rvtable', in which case the UE shall use an RSN
       based RV index as specified in 25.212*/
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_TABLE;
  }

  if (ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config !=
      current_config_ptr->l1_e_info.e_ul_info.harq_rv_config) 
  {
    WRRC_MSG0_HIGH("RRCEUL: Setting UL-L1 action cuz Harq Info RECONFIG");
    rrcllc_update_l1_e_req_mask_for_e_ul_info(TRUE);
  }
  
  /* Update Add-Recong mac-d flow info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->u.e_dch,rrc_UL_AddReconfTransChInformation_r7_e_dch,addReconf_MAC_d_FlowList))
  {       
    e_mac_d_flow_list_ptr = &ie_ptr->u.e_dch->addReconf_MAC_d_FlowList;

    idx=0;
    if(e_mac_d_flow_list_ptr != NULL)
    {
      /* Update Add-Recong mac-d flow info */
      while ((e_mac_d_flow_list_ptr != NULL) && (e_mac_d_flow_list_ptr->n > idx))
      {       
        /* As of now we do not support MAC-D Flow Retransimssion Timer */
        /* Check if we need to reurn a failure or an error print will do */
        if( e_mac_d_flow_list_ptr->elem[idx].m.mac_d_FlowRetransTimerPresent)
        {
            WRRC_MSG0_ERROR("RRCHSPA+ : rrc_mac_d_FlowRetransTimer_present not supported ");
            return FAILURE;
        }
        
        if ((e_mac_d_flow_idx = rrcllc_find_e_mac_d_flow_idx(
               (uint8)e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity)) == MAX_E_DCH_MAC_D_FLOW)
        {
          WRRC_MSG1_ERROR("RRCEUL: E-DCH mac-d flow ID %d, no idx found",
              e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity);
          return FAILURE;
        }
        else
        {
          if(rrcllc_restore_ul_emac_rb_mapping_info(e_mac_d_flow_idx) == FAILURE)
          {
            /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
            WRRC_MSG1_ERROR("RRCEUL: E-DCH mac-d flow ID %d, not correct info found in Stanalone rb-mapping info",
                e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity);
          }
          rrcllc_update_mac_eul_action(TRUE);
          /* Only allowed to be absent when already defined for this E-DCH MAC-d flow*/
          if (e_mac_d_flow_list_ptr->elem[idx].m.mac_d_FlowPowerOffsetPresent)
          {
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
              e_mac_d_flow_power_offset = e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowPowerOffset;
          }
          else
          {
            if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                e_mac_d_flow_power_offset > RRCLLC_MAX_E_MAC_D_FLOW_PWR_OFFSET)
            {
              WRRC_MSG0_ERROR("RRCEUL: E-MAC-d flow pwr offset needs to be provided");
              return FAILURE;
            }
          }
          /* Only allowed to be absent when already defined for this E-DCH MAC-d flow */
          if (e_mac_d_flow_list_ptr->elem[idx].m.mac_d_FlowMaxRetransPresent)
          {
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
              e_mac_d_flow_max_num_of_retx = e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowMaxRetrans;
          }
          else
          {
            if(ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
               e_mac_d_flow_max_num_of_retx > RRCLLC_MAX_E_MAC_D_FLOW_NUM_RETX)
            {
              WRRC_MSG0_ERROR("RRCEUL: E-MAC-d max num of retx needs to be provided");
              return FAILURE;
            }
          }
          
          /* if no mux list is provided, use the previous one.  If there is no previous then allow multiplexing of 
             MAC-d PDU's from the E-DCH MAC-d flow indicated in the IE "E-DCH MAC-d flow identity" with MAC-d PDU's 
             from any other E-DCH MAC-d flow in the same MAC-e PDU.*/
          if (e_mac_d_flow_list_ptr->elem[idx].m.mac_d_FlowMultiplexingListPresent)
          {
            /* Only look at first byte of bitstring as max e_mac_d flow is 8*/
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].e_mac_d_flow_mux_list =
              rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(
                &e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowMultiplexingList);
          }
          
          if (e_mac_d_flow_list_ptr->elem[idx].m.transmissionGrantTypePresent)
          {
            if (e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.t == 
                T_rrc_E_DCH_AddReconf_MAC_d_Flow_r7_transmissionGrantType_non_ScheduledTransGrantInfo)
            {
              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                e_tx_grant_info = MAC_E_TX_GRANT_NON_SCHEDULED;

              if(e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.t ==
                T_rrc_E_DCH_AddReconf_MAC_d_Flow_r7_modeSpecificInfo_fdd)
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.max_mac_e_pdu_size_in_bits = 
                    e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.u.fdd->maxMAC_e_PDUContents;

              if ((e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.t ==
                  T_rrc_E_DCH_AddReconf_MAC_d_Flow_r7_modeSpecificInfo_fdd) && 
                  (e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.u.fdd->m.
                  ms2_NonSchedTransmGrantHARQAllocPresent))
              {
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 
                    rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(
                      &e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->
                      modeSpecificInfo.u.fdd->ms2_NonSchedTransmGrantHARQAlloc);
              }
              else /* Default value is: transmission in all HARQ processes is allowed. so set all to 1 */
              {
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 0xFF;
              }
            }
            else /* rrc_scheduledTransmissionGrantInfo */
            {
              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].e_tx_grant_info = MAC_E_TX_GRANT_SCHEDULED;
            }
          }
        }

        idx++;  
      }
    }
    else
    {
      WRRC_MSG0_ERROR("RRCEUL: E-DCH mac-d flow list is NULL");
      return FAILURE;
    }
  }
  return SUCCESS;
}
/*================================================================================================
FUNCTION: get_ul_added_reconfig_trch_r7()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
static uecomdef_status_e_type 
get_ul_added_reconfig_trch_r7
(
  rrc_UL_AddReconfTransChInformation_r7 * ie_ptr
)
{
  MSG_MED("Get UL Add/Reconf TrCH",0,0,0);

  if(RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r7_dch_usch))
  {
      if(rrcllc_dch_usch_add_trch_info((tr_ch_id_type)ie_ptr->u.dch_usch->transportChannelIdentity,
                              &ie_ptr->u.dch_usch->transportFormatSet) == FAILURE)
      {
        return FAILURE;
      }
   
    }
  else if(RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r7_e_dch) )
  {
    if (FAILURE == process_added_reconfig_e_dch_info_r7(ie_ptr))
    {
      return FAILURE;
    }
  }
  return SUCCESS;
}
/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_ul_dch_trch_info_r7()

DESCRIPTION:
  This function takes up deleted and added UL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type 
rrcllc_add_delete_and_arrange_ul_dch_trch_info_r7
(
  OSS_UINT32 ul_add_recfg_trch_info_list_present,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r7 * ul_trch_add_reconfig_ptr,
  OSS_UINT32 ul_deleted_trch_info_list_present,
  rrc_UL_DeletedTransChInfoList_r6 * ul_deleted_trch_list_ptr
)
{
  uint32 idx =0,idy=0, idz=0;
  rrc_E_DCH_AddReconf_MAC_d_FlowList_r7 * temp_e_dch_mac_d_flow_list_ptr = NULL;
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  
  rrc_UL_DeletedTransChInfoList_r6 *temp_ul_trch_delete_ptr = ul_deleted_trch_list_ptr;
  rrc_UL_AddReconfTransChInfoList_r7  *temp_ul_trch_add_reconfig_ptr = ul_trch_add_reconfig_ptr;

  if((temp_ul_trch_delete_ptr != NULL) && (temp_ul_trch_add_reconfig_ptr != NULL))
  {
    for(idx=0; idx<temp_ul_trch_delete_ptr->n; idx++)
    {
      if(T_rrc_UL_TransportChannelIdentity_r6_dch_usch == temp_ul_trch_delete_ptr->elem[idx].t)
      {
        for(idy=0; idy < temp_ul_trch_add_reconfig_ptr->n; idy++)
        {
          if(T_rrc_UL_AddReconfTransChInformation_r7_dch_usch == temp_ul_trch_add_reconfig_ptr->elem[idy].t)
          {
            if(temp_ul_trch_delete_ptr->elem[idx].u.dch_usch->ul_TransportChannelIdentity == temp_ul_trch_add_reconfig_ptr->elem[idy].u.dch_usch->transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
              temp_ul_trch_add_reconfig_ptr->elem[idy].u.dch_usch->transportChannelIdentity,temp_ul_trch_delete_ptr->elem[idx].u.dch_usch->ul_TransportChannelIdentity);
              return FAILURE;
            }
          }
        }
      }   
      else if(T_rrc_UL_TransportChannelIdentity_r6_e_dch == temp_ul_trch_delete_ptr->elem[idx].t)
      {
        for(idy=0; idy<temp_ul_trch_add_reconfig_ptr->n; idy++)
        {
          if(T_rrc_UL_AddReconfTransChInformation_r7_e_dch == temp_ul_trch_add_reconfig_ptr->elem[idy].t)
          {
            if(temp_ul_trch_add_reconfig_ptr->elem[idy].u.e_dch->m.addReconf_MAC_d_FlowListPresent)
            {            	
              temp_e_dch_mac_d_flow_list_ptr = &temp_ul_trch_add_reconfig_ptr->elem[idy].u.e_dch->addReconf_MAC_d_FlowList;
              for(idz=0; idz<temp_e_dch_mac_d_flow_list_ptr->n; idz++)
              {
                if(temp_ul_trch_delete_ptr->elem[idx].u.e_dch == temp_e_dch_mac_d_flow_list_ptr->elem[idz].mac_d_FlowIdentity)
                {
                  WRRC_MSG2_ERROR("Same edch Trch ID found in add/reconfig list %d and deleted list %d ",
                  temp_e_dch_mac_d_flow_list_ptr->elem[idz].mac_d_FlowIdentity,temp_ul_trch_delete_ptr->elem[idx].u.e_dch);
                  return FAILURE;
                }
              }
            }
          }
        }
      }
    }
  }

  idx = 0;
  idy = 0;
  
  if ((ul_deleted_trch_info_list_present) && (ul_deleted_trch_list_ptr != NULL))
  {
    while (ul_deleted_trch_list_ptr->n > idx)
    {
      /* Remove this transport channel from MAC, L1 and local data.
       * and update transport channel count in L1, MAC and local data.
       */
      if(get_ul_deleted_trch_info_list_r6(&ul_deleted_trch_list_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;
    } /* end while */
    idx = 0;
    /*Fill the holes only when Ul EDCH Trch is deleted*/
    while(ul_deleted_trch_list_ptr->n > idx) 
    {
      if (T_rrc_UL_TransportChannelIdentity_r6_e_dch == ul_deleted_trch_list_ptr->elem[idx].t)
      {
        rrcllc_fill_holes_in_e_mac_d_flow_idx();
        break;
      }
      idx++;
    }
  }
  idx =0;
  if ((ul_add_recfg_trch_info_list_present) && (ul_trch_add_reconfig_ptr != NULL))
  {
    /* Get the UL Added/Reconfigured Info */
    while(ul_trch_add_reconfig_ptr->n > idx) 
    {
      if (T_rrc_UL_AddReconfTransChInformation_r7_dch_usch ==
          ul_trch_add_reconfig_ptr->elem[idx].t)
      {
        trch_id = (tr_ch_id_type)
                  ul_trch_add_reconfig_ptr->elem[idx].u.dch_usch->transportChannelIdentity;
  
        if((rrcllc_get_ul_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
        {
          WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
          return(FAILURE);
        }
      }
      else if ((T_rrc_UL_AddReconfTransChInformation_r7_e_dch == 
    ul_trch_add_reconfig_ptr->elem[idx].t) && 
               (ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->m.addReconf_MAC_d_FlowListPresent)) 
      {
        rrc_E_DCH_AddReconf_MAC_d_FlowList_r7 * e_dch_mac_d_flow_list_ptr = NULL;
  idy =0;

        e_dch_mac_d_flow_list_ptr = &ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->addReconf_MAC_d_FlowList;

        while (e_dch_mac_d_flow_list_ptr->n > idy)
        {
          if((rrcllc_get_e_mac_dflow_idx((uint8)e_dch_mac_d_flow_list_ptr->elem[idy].
              mac_d_FlowIdentity)) == MAX_E_DCH_MAC_D_FLOW)
          {
            WRRC_MSG1_ERROR("RRCEUL: Couldn't find idx for e-dch MAC-D flow: %d",
              e_dch_mac_d_flow_list_ptr->elem[idy].mac_d_FlowIdentity);
            return(FAILURE);
          }

          idy++;
        }

        rrcllc_update_mac_eul_action(TRUE);
      }
      idx++;
    } /* while(...) */
  }

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->ul_trch_cnt indices.
   */
  rrcllc_fill_holes_in_ul_dch_trch_idx();

  rrcllc_fill_holes_in_e_mac_d_flow_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */
  rrcllc_sort_ul_dch_trch_index_array(&ordered_config_ptr->ul_dch_trch_info[0], 
                                      (RRCLLC_UPLINK_TRCH_COUNT(ordered_config_ptr)));
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_store_ul_add_reconf_trch_info_list_r7()

DESCRIPTION:
  This function updates IE "ul added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_ul_add_reconf_trch_info_list_r7
(
  OSS_UINT32 ul_add_recfg_trch_info_list_present,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r7  * ul_trch_add_reconfig_ptr  
)
{
  if(ul_add_recfg_trch_info_list_present)
  {
    uint32 idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (ul_trch_add_reconfig_ptr->n > idx))
    {
      if(get_ul_added_reconfig_trch_r7(&ul_trch_add_reconfig_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
      
      /* get next TrCH Info */
      idx++;
    } /* end while */
  } /* end UL TrCH Add/Reconfig */

  /* Check to make sure UL TFS count is within UE Capabilities */
  if(rrcllc_check_ul_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_dl_dch_trch_info_r7()

DESCRIPTION:
  This function takes up deleted and added DL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs. For HSDSCH, it adds, deletes the DFLOW and rearranges
  the DFLOW so that missing holes because of deleted DFLOWs are
  removed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_dl_dch_trch_info_r7
(
  /* local DL TrCH Add Reconfig pointer */
  rrc_DL_AddReconfTransChInfoList_r7 * dl_trch_add_reconfig_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList_r7    * dl_trch_delete_ptr
)
{
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;

  uint8 dflow_index;
  rrc_MAC_hs_AddReconfQueue_List * dflow_queue_head_ptr = NULL;
  uint32 idx =0,idy =0,idz=0;

  rrc_DL_DeletedTransChInfoList_r7 *temp_dl_trch_delete_ptr = dl_trch_delete_ptr;
  rrc_DL_AddReconfTransChInfoList_r7  *temp_dl_trch_add_reconfig_ptr = dl_trch_add_reconfig_ptr;
  rrc_HSDSCH_Info_r7 *temp_hsdsch;
  rrc_MAC_ehs_AddReconfReordQ_List *temp_MAC_ehs_AddReconfReordQ_List;
  

  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    while(temp_dl_trch_delete_ptr->n > idx)
    {
      if(T_rrc_DL_TrCH_TypeId2_r7_dch == temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.t)
      {
        idy = 0;
        while(temp_dl_trch_add_reconfig_ptr->n > idy)
        {
          if(T_rrc_DL_TrCH_TypeId1_r5_dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.t)
          {
            if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.u.dch)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
              temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.u.dch,temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.dch);
              return FAILURE;
            }
          }
          idy++;
        }
      }
      idx++;
    }
  }


  /* 8.6.5.23  Added or reconfigured MAC-ehs reordering queue
  1>  if a MAC-ehs queue (identified by the IE "MAC-ehs queue Id") is included in both the IE "MAC-ehs queue to 
      add or reconfigure list" and the IE "DL HS-DSCH MAC-ehs reordering queue" within the IE "Deleted DL TrCH 
      information": 
      2>  the UE behaviour is unspecified. 
  */
  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    for(idx=0;idx < temp_dl_trch_delete_ptr->n ;idx++)
    {
      if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.t != 
          T_rrc_DL_TrCH_TypeId2_r7_hsdsch )
        continue;
      if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.hsdsch->t !=
          T_rrc_DL_TrCH_TypeId2_r7_hsdsch_mac_ehs)
        continue;

      for(idy = 0;temp_dl_trch_add_reconfig_ptr->n > idy ;idy++)
      {
        if(temp_dl_trch_add_reconfig_ptr->elem[idy].tfs_SignallingMode.t != 
            T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_hsdsch)
          continue;
        
        temp_hsdsch = temp_dl_trch_add_reconfig_ptr->elem[idy].tfs_SignallingMode.u.hsdsch;
        if(!temp_hsdsch->m.dl_MAC_HeaderTypePresent)
          continue;
        if(temp_hsdsch->dl_MAC_HeaderType.t != T_rrc_HSDSCH_Info_r7_dl_MAC_HeaderType_mac_ehs)
          continue;
        if(!temp_hsdsch->dl_MAC_HeaderType.u.mac_ehs->m.mac_ehs_AddReconfQueue_ListPresent)
          continue;
        temp_MAC_ehs_AddReconfReordQ_List = &temp_hsdsch->dl_MAC_HeaderType.u.mac_ehs->mac_ehs_AddReconfQueue_List;

        for(idz=0;temp_MAC_ehs_AddReconfReordQ_List->n > idz;idz++)
        {
          if(temp_MAC_ehs_AddReconfReordQ_List->elem[idz].mac_ehs_QueueId ==
             temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.hsdsch->u.mac_ehs)
          {
            WRRC_MSG1_ERROR("Same MAC-ehs queue Id found in add/reconfig list and deleted list ",
              temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.hsdsch->u.mac_ehs);
            return FAILURE;
          }
        }
      }
    }
  }


  idx =0;
  idy =0;
  while((dl_trch_delete_ptr !=NULL) &&( dl_trch_delete_ptr->n > idx))
  {
  /* Remove this transport channel from MAC, L1 and local data.
  * and update transport channel count in L1, MAC and local data.
    */
    if(get_dl_deleted_trch_r7(&dl_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the DL Added/Reconfigured Info */
  while((dl_trch_add_reconfig_ptr !=NULL) && (
    dl_trch_add_reconfig_ptr->n > idx) )
  {
    /* Check if channel type is HSDPA */
    if (dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.t == 
      T_rrc_DL_TrCH_TypeId1_r5_hsdsch)
    {
      /* At least one HARQ/DFLOW has to be present */
      if ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.t == 
    T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_hsdsch)
        && ((dl_trch_add_reconfig_ptr->elem[idx].
        tfs_SignallingMode.u.hsdsch->m.dl_MAC_HeaderTypePresent)
        || (dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.
        u.hsdsch->m.harqInfoPresent)))
      {
        
        MSG_LOW("RRCHSPA+:Valid DFLOW/MACEhs specified",0,0,0);
      }
      else
      {
        WRRC_MSG0_ERROR("RRCHSPA+:HARQ,DFLOW both ABS.");
        return FAILURE;
      }
      if(dl_trch_add_reconfig_ptr->elem[idx].
        tfs_SignallingMode.u.hsdsch->m.dl_MAC_HeaderTypePresent)
      {
        if(dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.
   hsdsch->dl_MAC_HeaderType.t == T_rrc_DL_TrCH_TypeId2_r7_hsdsch_mac_hs)
        {
          /* Now find the DFLOW Index for ADD/RECONFIG.  */
          if ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
     dl_MAC_HeaderType.u.mac_hs->m.mac_hs_AddReconfQueue_ListPresent) &&
            (dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
            dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List.n > 0))
          {
            dflow_queue_head_ptr = 
              &dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
              dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List;
            idy =0;
            while (dflow_queue_head_ptr->n > idy)
            {
              if ((dflow_queue_head_ptr->elem[idy].mac_dFlowId > UE_MAX_MAC_D_FLOW-1) ||
                  (dflow_queue_head_ptr->elem[idy].mac_hsQueueId > UE_MAX_HS_QUEUE-1))
              {
                WRRC_MSG2_ERROR("RRCHS:DFLOW Id %d, Queue Id %d, Range 0..7", dflow_queue_head_ptr->elem[idy].mac_dFlowId,
                    dflow_queue_head_ptr->elem[idy].mac_hsQueueId);
                return FAILURE;
              }
    
              /* If DFLOW Index found then return it Else allocate a new index */
              if ((dflow_index = rrcllc_get_dflow_index((uint8)dflow_queue_head_ptr->elem[idy].mac_dFlowId)) 
                == UE_MAX_MAC_D_FLOW)
              {
                WRRC_MSG1_HIGH("RRCHS:DFLOW index %d absent",dflow_index);
                return FAILURE;
              }
              else
              {
                idy++;
              }
            } /* End of while */       
            
          } /* End of if of add queue present */
        }
        else if(dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.
   hsdsch->dl_MAC_HeaderType.t == T_rrc_DL_TrCH_TypeId2_r7_hsdsch_mac_ehs)
        {
        if(macehs_add_or_del_q_info(dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_ehs) == FAILURE)            
           return FAILURE;
        }
      }
    } /* End of if of rrc_DL_TrCH_TypeId1_r5_hsdsch chosen */
    else if (dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.t == 
      T_rrc_DL_TrCH_TypeId1_r5_dch)
    {
      trch_id = (tr_ch_id_type)dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.u.dch;
      
      if((rrcllc_get_dl_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
      {
        WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
        return(FAILURE);
      }
    }
    else
    {
      WRRC_MSG0_ERROR("DSCH unsupported");
      return FAILURE;
    }
    idx++;
  } /* while(...) */
  
    /* At this point, the transport channel list -
     *  - may have holes due to deleted channels.
     *  - transport channel info may not be sorted by TrCH IDs.
     */ 
  
  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->dl_trch_cnt indices.
   */

  /* Now Remove holes in DFLOW Index */
  rrcllc_rearrange_dflow_index();
  rrcllc_rearrange_macehs_queue_index(ordered_config_ptr);
  rrcllc_fill_holes_in_dl_dch_trch_idx();
  
  /* Now is the time and place for sorting all transport channel
  * info based on TrCH IDs. Call the right function.
  */
  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                     ordered_config_ptr->mac_dl_parms.num_dchs
                                      );
  
  return(SUCCESS);
}
/*===========================================================================

FUNCTION    rrcllc_check_srb_mapped_to_hsdsch_present

DESCRIPTION This function checks if the HSDSCH Trch mapped to SRBs in 
             CELL DCH state exist
 
DEPENDENCIES
  None.

RETURN VALUE
 TRUE or FALSE

===========================================================================*/
boolean rrcllc_check_srb_mapped_to_hsdsch_present
(
void
)
{
  uint32 cnt;
  uint32 index;

  ordered_config_type * config_ptr;
  
  if(config_ptr_in_use == CURRENT_CONFIG )
  {
    config_ptr = current_config_ptr;
  }
  else
  {
    config_ptr = ordered_config_ptr;
  }

  /*We check for only mapping for MAC-hs for one SRB only.*/
    for(index = 0; index < UE_MAX_MAC_D_FLOW;  index++)
    {
      if(config_ptr->dl_dflow_index_in_use[index] == TRUE)
      {
        for (cnt = 0; cnt < config_ptr->mac_dflow_info[index].ndlchan; cnt++) 
        {
          if((config_ptr->mac_dflow_info[index].dlchan_info[cnt].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
          {
            /* this IE must have already been provided as part of mapping info. Just
               return success
             */
            return (TRUE);
          }          
        }
      }
    }

  /*We check for only mapping for MAC-ehs for one SRB only.*/
    /*Loop through all the DL logical channels and find out the ones that are for SRBs*/
    for(index = 0; index < UE_MAX_DL_LOGICAL_CHANNEL;  index++)
    {
      if((config_ptr->mac_ehs_lc_id_in_use[index] == TRUE) &&
        (config_ptr->dl_macehs_logchan_info[index].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID))
      {
        /*Now we got the DL logical channel for some SRB*/
        for (cnt = 0; cnt < UE_MAX_HS_QUEUE; cnt++) 
        {
          /*Check whether Q-ID for this SRB is present or not.*/
          if((config_ptr->mac_ehs_queue_index_in_use[cnt] == TRUE) &&
            (config_ptr->dl_macehs_logchan_info[index].queue_id == 
            config_ptr->mac_ehs_queue_info[cnt].queue_id))
          {
            return (TRUE);
          }          
        }
      }
    }
    return (FALSE);
  }
/*============================================================================
FUNCTION: rrcllc_check_srb_mapped_to_trch()

DESCRIPTION:
  This function checks if the TRCH id passed is mapped to SRB or not
  removed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE
      Returns True if the Trch is mapped to SRB
      else   FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_check_srb_mapped_to_trch
(
 tr_ch_id_type               trch_id
 )
{
  uint32 trch_idx;
  ordered_config_type * config_ptr;
  
  if(config_ptr_in_use == CURRENT_CONFIG )
  {
    config_ptr = current_config_ptr;
  }
  else
  {
    config_ptr = ordered_config_ptr;
  }
  
  for(trch_idx=0;trch_idx< UE_MAX_TRCH;trch_idx++)
  {
    if((config_ptr->dl_dch_trch_info[trch_idx].dch_present) &&
        (config_ptr->dl_dch_trch_idx_in_use[trch_idx] == TRUE) &&
        (config_ptr->dl_dch_trch_info[trch_idx].trch_id == trch_id))
    {
      if((config_ptr->dl_dch_info[trch_idx].ndlchan >0)&&
        (config_ptr->dl_dch_info[trch_idx].dlchan_info[0].rb_id < DCCH_DT_LOW_PRI_RADIO_BEARER_ID ))
      {
        /* this IE must have already been provided as part of mapping info. Just
             return success
           */
        return TRUE;
      }
      break;
    }
  }
  return FALSE;
}
/*============================================================================
FUNCTION: rrc_check_if_trch_is_configured_for_voice()

DESCRIPTION:
  This function checks if the TRCH id passed is mapped to voice RB
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE
      Returns True if the Trch is mapped to Voice RB
      else   FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrc_check_if_trch_is_configured_for_voice
(
  tr_ch_id_type               trch_id
)
{
  uint8 trch_idx;
  uint8 logical_chnl_idx;
  ordered_config_type * config_ptr;
  
  config_ptr = rrcllc_get_config_ptr_in_use();
  
  for(trch_idx=0;trch_idx < UE_MAX_TRCH;trch_idx++)
  {
    if((config_ptr->dl_dch_trch_info[trch_idx].dch_present) 
        &&
       (config_ptr->dl_dch_trch_idx_in_use[trch_idx] == TRUE)
        &&
       (config_ptr->dl_dch_trch_info[trch_idx].trch_id == trch_id))
    {
    
      for(logical_chnl_idx = 0; ((logical_chnl_idx < config_ptr->dl_dch_info[trch_idx].ndlchan) && (logical_chnl_idx < MAX_DED_LOGCHAN_PER_TRCH)); logical_chnl_idx++)
      {
         if((config_ptr->dl_dch_info[trch_idx].dlchan_info[logical_chnl_idx].rb_id > DCCH_DT_LOW_PRI_RADIO_BEARER_ID )
             &&
            (config_ptr->dl_dch_info[trch_idx].dlchan_info[logical_chnl_idx].rb_id <= MAX_RB)
             &&
            (config_ptr->dl_dch_info[trch_idx].dlchan_info[logical_chnl_idx].chan_type == UE_LOGCHAN_DTCH)
             &&
            (config_ptr->dl_dch_info[trch_idx].dlchan_info[logical_chnl_idx].rlc_mode== UE_MODE_TRANSPARENT))
        {
          return TRUE;
        }
      }
      break;
    }
  }
  return FALSE;
}

/*============================================================================
FUNCTION: rrcllc_get_ctch_trch_id()

DESCRIPTION:
  This function returns the transport channel id on which CTCH is mapped
      
DEPENDENCIES:
  None.

RETURN VALUE:
  Trch id
      Returns tr_ch_id_type if the FACH Trch is mapped with
      else   return invalid trch id

SIDE EFFECTS:
=============================================================================*/
tr_ch_id_type rrcllc_get_ctch_trch_id(void)
{
  uint32 trch_idx;
  ordered_config_type *config_ptr;

  if(config_ptr_in_use == CURRENT_CONFIG )
  {
    config_ptr = current_config_ptr;
  }
  else
  {
    config_ptr = ordered_config_ptr;
  }
  
  for(trch_idx = 0; trch_idx < config_ptr->mac_dl_parms.num_fachs; trch_idx++)
  {
    if(config_ptr->mac_dl_parms.fach_info[trch_idx]->ctch_enable == TRUE)
    {
      return config_ptr->mac_dl_parms.fach_info[trch_idx]->trch_id;
    }
  }
  return INVALID_TRCH_ID;
}
/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_ul_dch_trch_info_r8()

DESCRIPTION:
  This function takes up deleted and added UL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type 
rrcllc_add_delete_and_arrange_ul_dch_trch_info_r8
(
  OSS_UINT32 ul_add_recfg_trch_info_list_present,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r8 *  ul_trch_add_reconfig_ptr,
  OSS_UINT32 ul_deleted_trch_info_list_present,
  rrc_UL_DeletedTransChInfoList_r6 * ul_deleted_trch_list_ptr
)
{
  uint32 idx =0,idy=0, idz=0;
  rrc_E_DCH_AddReconf_MAC_d_FlowList_r7 * temp_e_dch_mac_d_flow_list_ptr = NULL;
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  
  rrc_UL_DeletedTransChInfoList_r6 *temp_ul_trch_delete_ptr = ul_deleted_trch_list_ptr;
  rrc_UL_AddReconfTransChInfoList_r8 *temp_ul_trch_add_reconfig_ptr = ul_trch_add_reconfig_ptr;


  if((temp_ul_trch_delete_ptr != NULL) && (temp_ul_trch_add_reconfig_ptr != NULL))
  {
    for(idx=0; idx<temp_ul_trch_delete_ptr->n; idx++)
    {
      if(T_rrc_UL_TransportChannelIdentity_r6_dch_usch == temp_ul_trch_delete_ptr->elem[idx].t)
      {
        for(idy=0; idy<temp_ul_trch_add_reconfig_ptr->n; idy++)
        {
          if(T_rrc_UL_AddReconfTransChInformation_r8_dch_usch == temp_ul_trch_add_reconfig_ptr->elem[idy].t)
          {
            if(temp_ul_trch_delete_ptr->elem[idx].u.dch_usch->ul_TransportChannelIdentity == temp_ul_trch_add_reconfig_ptr->elem[idy].u.dch_usch->transportChannelIdentity)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
              temp_ul_trch_add_reconfig_ptr->elem[idy].u.dch_usch->transportChannelIdentity,temp_ul_trch_delete_ptr->elem[idx].u.dch_usch->ul_TransportChannelIdentity);
              return FAILURE;
            }
          }
        }
      }   
      else if(T_rrc_UL_TransportChannelIdentity_r6_e_dch == temp_ul_trch_delete_ptr->elem[idx].t)
      {
        for(idy=0; idy<temp_ul_trch_add_reconfig_ptr->n; idy++)
        {
          if(T_rrc_UL_AddReconfTransChInformation_r7_e_dch == temp_ul_trch_add_reconfig_ptr->elem[idy].t)
          {
            if(temp_ul_trch_add_reconfig_ptr->elem[idy].u.e_dch->m.addReconf_MAC_d_FlowListPresent)
            {           	
              temp_e_dch_mac_d_flow_list_ptr = &temp_ul_trch_add_reconfig_ptr->elem[idy].u.e_dch->addReconf_MAC_d_FlowList;
              for(idz=0; idz<temp_e_dch_mac_d_flow_list_ptr->n; idz++)
              {
                if(temp_ul_trch_delete_ptr->elem[idx].u.e_dch == temp_e_dch_mac_d_flow_list_ptr->elem[idz].mac_d_FlowIdentity)
                {
                  WRRC_MSG2_ERROR("Same edch Trch ID found in add/reconfig list %d and deleted list %d ",
                  temp_e_dch_mac_d_flow_list_ptr->elem[idz].mac_d_FlowIdentity,temp_ul_trch_delete_ptr->elem[idx].u.e_dch);
                  return FAILURE;
                }
              }
            }
          }
        }
      }   
    }
  }

  idx =0;
  idy=0;
  if ((ul_deleted_trch_info_list_present) && (ul_deleted_trch_list_ptr != NULL))
  {
    while (ul_deleted_trch_list_ptr->n > idx)
    {
      /* Remove this transport channel from MAC, L1 and local data.
       * and update transport channel count in L1, MAC and local data.
       */
      if(get_ul_deleted_trch_info_list_r6(&ul_deleted_trch_list_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;
    } /* end while */
    idx = 0;
    /*Fill the holes only when Ul EDCH Trch is deleted*/
    while(ul_deleted_trch_list_ptr->n > idx) 
    {
      if (T_rrc_UL_TransportChannelIdentity_r6_e_dch == ul_deleted_trch_list_ptr->elem[idx].t)
      {
        rrcllc_fill_holes_in_e_mac_d_flow_idx();
        break;
      }
      idx++;
    }
  }
  idx =0;
  if ((ul_add_recfg_trch_info_list_present) && (ul_trch_add_reconfig_ptr != NULL))
  {
    /* Get the UL Added/Reconfigured Info */
    while(ul_trch_add_reconfig_ptr->n > idx) 
    {
      if (T_rrc_UL_AddReconfTransChInformation_r8_dch_usch ==
          ul_trch_add_reconfig_ptr->elem[idx].t)
      {
        trch_id = (tr_ch_id_type)
                  ul_trch_add_reconfig_ptr->elem[idx].u.dch_usch->transportChannelIdentity;
  
        if((rrcllc_get_ul_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
        {
          WRRC_MSG1_ERROR("REL8: Couldn't find idx, TrCH ID %d", trch_id);
          return(FAILURE);
        }
      }
      else if (RRC_CHECK_LIST_MSG_TYPE(ul_trch_add_reconfig_ptr,rrc_UL_AddReconfTransChInformation_r8_e_dch))
      {
        if(ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->m.ul_MAC_HeaderTypePresent)
        {
#ifdef FEATURE_MAC_I
          MSG_MED("MAC_I: ul_MAC_HeaderType: Mac-i/is ",0,0,0);
#else  /* FEATURE_MAC_I */		
          WRRC_MSG0_ERROR("REL8: IE ul_MAC_HeaderType not supported yet!");
          return FAILURE;
#endif /* FEATURE_MAC_I */
        }      
        if(ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->m.addReconf_MAC_d_FlowListPresent)
        {
          rrc_E_DCH_AddReconf_MAC_d_FlowList_r7 * e_dch_mac_d_flow_list_ptr = NULL;

          e_dch_mac_d_flow_list_ptr = &ul_trch_add_reconfig_ptr->elem[idx].u.e_dch->addReconf_MAC_d_FlowList;
          idy =0;
          while (e_dch_mac_d_flow_list_ptr->n > idy)
          {
            if((rrcllc_get_e_mac_dflow_idx((uint8)e_dch_mac_d_flow_list_ptr->elem[idy].
                mac_d_FlowIdentity)) == MAX_E_DCH_MAC_D_FLOW)
            {
              WRRC_MSG1_ERROR("REL8: Couldn't find idx for e-dch MAC-D flow: %d", 
                e_dch_mac_d_flow_list_ptr->elem[idy].mac_d_FlowIdentity);
              return(FAILURE);
            }
            idy++;
          }
          rrcllc_update_mac_eul_action(TRUE);
        }
      }
     idx++;
    } /* while(...) */
  }

  /* At this point, the transport channel list -
   *  - may have holes due to deleted channels.
   *  - transport channel info may not be sorted by TrCH IDs.
   */ 

  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->ul_trch_cnt indices.
   */
  rrcllc_fill_holes_in_ul_dch_trch_idx();

  rrcllc_fill_holes_in_e_mac_d_flow_idx();

  /* Now is the time and place for sorting all transport channel
   * info based on TrCH IDs. Call the right function.
   */
  rrcllc_sort_ul_dch_trch_index_array(&ordered_config_ptr->ul_dch_trch_info[0], 
                                      (RRCLLC_UPLINK_TRCH_COUNT(ordered_config_ptr)));
  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_store_ul_add_reconf_trch_info_list_r8()

DESCRIPTION:
  This function updates IE "ul added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_ul_add_reconf_trch_info_list_r8
(
  OSS_UINT32 ul_add_recfg_trch_info_list_present,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r8 * ul_trch_add_reconfig_ptr  
)
{
  if(ul_add_recfg_trch_info_list_present)
  {
    uint32 idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (ul_trch_add_reconfig_ptr->n > idx))
    {
      if(get_ul_added_reconfig_trch_r8(&ul_trch_add_reconfig_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
      
      /* get next TrCH Info */
      idx++;
      
    } /* end while */
  } /* end UL TrCH Add/Reconfig */

  /* Check to make sure UL TFS count is within UE Capabilities */
  if(rrcllc_check_ul_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}

/*================================================================================================
FUNCTION: get_ul_added_reconfig_trch_r8()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type 
get_ul_added_reconfig_trch_r8
(
  rrc_UL_AddReconfTransChInformation_r8 * ie_ptr
)
{
  MSG_MED("Get UL Add/Reconf TrCH",0,0,0);

  if (RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r8_dch_usch))
  {
      if(rrcllc_dch_usch_add_trch_info((tr_ch_id_type)ie_ptr->u.dch_usch->transportChannelIdentity,
                              &ie_ptr->u.dch_usch->transportFormatSet) == FAILURE)
      {
        return FAILURE;
      }
   
    }
  else if (RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r8_e_dch)) 
  {
    if (FAILURE == process_added_reconfig_e_dch_info_r8(ie_ptr))
    {
      return FAILURE;
    }
  }
  return SUCCESS;
}

/*================================================================================================
FUNCTION: process_added_reconfig_e_dch_info_r8()

DESCRIPTION:
  This function takes a pointer to the EDCH Added or Reconfigured 
  mac-d flow info and copies it to ORDERED_CONFIG.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS if the IE conenets are valid
                FALSE   otherwise

SIDE EFFECTS:

===============================================================================================*/
uecomdef_status_e_type process_added_reconfig_e_dch_info_r8
(
  rrc_UL_AddReconfTransChInformation_r8 * ie_ptr
)
{

  /* local transport channel variables */
  uint8 e_mac_d_flow_idx = MAX_E_DCH_MAC_D_FLOW;
  e_tti_enum_type temp_tti;
  rrc_E_DCH_AddReconf_MAC_d_FlowList_r7 * e_mac_d_flow_list_ptr = NULL;
  uint32 idx =0;
  if (!RRC_CHECK_COMMON_MSG_TYPE_PTR(ie_ptr,rrc_UL_AddReconfTransChInformation_r8_e_dch))
  {
    WRRC_MSG1_ERROR("Un-wanted call to function to process e_dch_info. t: %d",
      ie_ptr->t);
    return(FAILURE);
  }
  /*Added NULL check to bypass the KW error*/
  if(ie_ptr->u.e_dch == NULL)
  {
    return(FAILURE);
  }

  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr->u.e_dch,ul_MAC_HeaderType))
  {
#ifdef FEATURE_MAC_I
    ordered_config_ptr->mac_ul_parms.ul_pdu_type = MAC_I_PDU_HDR;
#else	
    WRRC_MSG0_ERROR("REL8: IE ul_MAC_HeaderType is not supported yet!");
    return FAILURE;
#endif /* FEATURE_MAC_I */
  }
#ifdef FEATURE_MAC_I
  else
  {
    ordered_config_ptr->mac_ul_parms.ul_pdu_type = MAC_E_PDU_HDR;
  }
#endif /* FEATURE_MAC_I */


  /*Update the TTI info in MAC and L1 struct. 10ms or 2ms */
  /* Check with stored TTI value before indicating reconfig to L1/MAC*/
  if(ie_ptr->u.e_dch->modeSpecific.t == T_rrc_UL_AddReconfTransChInformation_r8_modeSpecific_fdd )
  {
    temp_tti = ((ie_ptr->u.e_dch->modeSpecific.u.fdd->tti == rrc_E_DCH_TTI_tti10) ? E_TTI_10MS : E_TTI_2MS);
    if(temp_tti == E_TTI_2MS)
    {
      /* Check to see NV HSUPA Category.
       * For supporting 2msec TTI, Below are the valid cats
       * Cat - 2,4,6,7,8,9 from table Table 5.1g: FDD E-DCH physical layer categories
       * in 25.306.If N/W is trying to configure 2msec TTI with any other 
       * UE cat Bcoz of N/W issue, Reject this configuration
      */
      if(
         (wcdma_nv_hsupa_category == 0)||
         (wcdma_nv_hsupa_category == 1)||
         (wcdma_nv_hsupa_category == 3)||
         (wcdma_nv_hsupa_category == 5)
         )
      {
        WRRC_MSG1_ERROR("RRCEUL:Error N/W try to config 2msec TTI when UE cat = %d which is not 2 or 4 or 6", 
        wcdma_nv_hsupa_category);
        return(FAILURE);
      }
    }
    
    /*We store e-tti & harq_rv_config which are trch i.e in L1 data base,
    *hence init them when previously EUL was not active
    */
    if(current_config_ptr->e_dch_transmission == FALSE)
    {
      ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_NOT_PRESENT;
      ordered_config_ptr->l1_e_info.e_ul_info.e_tti = NUM_E_TTI;
      ordered_config_ptr->l1_e_info.e_dl_info.e_tti = NUM_E_TTI;
    }
  
    if (temp_tti != ordered_config_ptr->l1_e_info.e_ul_info.e_tti) 
    {
      rrcllc_update_mac_eul_action(TRUE);
      rrcllc_update_l1_e_req_mask_for_e_dl_info(TRUE);
      rrcllc_update_l1_e_req_mask_for_e_ul_info(TRUE);

      /*Set all E-DCH active set DL RL's bit mask to reconfig, 
       *This is used by L1 to reconfigure AGCH/RGCH/HICH
       *Channels at TTI reconfiguration
       */
      rrcllc_update_e_dl_channel_mask_for_e_active_set_rl_to_reconfig();

      ordered_config_ptr->mac_e_config.e_tti = temp_tti;
      ordered_config_ptr->l1_e_info.e_ul_info.e_tti = temp_tti;
      ordered_config_ptr->l1_e_info.e_dl_info.e_tti = temp_tti;
      if (current_config_ptr->mac_e_config.e_tti != NUM_E_TTI) 
      {
        WRRC_MSG1_HIGH("RRCEUL: Setting L1/MAC action cuz TTI RECONFIG (tti: %d)",
          temp_tti);
        ordered_config_ptr->mac_e_reset_indicator = TRUE;
      }
    }
  }
  /* Update the harq info if present */
  if (ie_ptr->u.e_dch->harq_Info == rrc_UL_AddReconfTransChInformation_r8_harq_Info_rv0)
  {
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_0;
  }
  else
  {
    /* Default value is 'rvtable', in which case the UE shall use an RSN
       based RV index as specified in 25.212*/
    ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config = L1_RV_TABLE;
  }

  if (ordered_config_ptr->l1_e_info.e_ul_info.harq_rv_config !=
      current_config_ptr->l1_e_info.e_ul_info.harq_rv_config) 
  {
    WRRC_MSG0_HIGH("REL8: Setting UL-L1 action cuz Harq Info RECONFIG");
    rrcllc_update_l1_e_req_mask_for_e_ul_info(TRUE);
  }

  /* Update Add-Recong mac-d flow info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->u.e_dch,
   rrc_UL_AddReconfTransChInformation_r8_e_dch,addReconf_MAC_d_FlowList))
  {       
    e_mac_d_flow_list_ptr = &ie_ptr->u.e_dch->addReconf_MAC_d_FlowList;

    idx =0;
    if(e_mac_d_flow_list_ptr != NULL)
    {
      /* Update Add-Recong mac-d flow info */
      while ((e_mac_d_flow_list_ptr != NULL) && (e_mac_d_flow_list_ptr->n > idx))
      {       
        /* As of now we do not support MAC-D Flow Retransimssion Timer */
        /* Check if we need to reurn a failure or an error print will do */
        if(RRC_MSG_LIST_BITMASK_IE_TYPE2(e_mac_d_flow_list_ptr,mac_d_FlowRetransTimer))
        {
            WRRC_MSG0_ERROR("RRCHSPA+ : rrc_mac_d_FlowRetransTimer_present not supported ");
            return FAILURE;
        }
        
        if ((e_mac_d_flow_idx = rrcllc_find_e_mac_d_flow_idx(
               (uint8)e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity)) == MAX_E_DCH_MAC_D_FLOW)
        {
          WRRC_MSG1_ERROR("RRCEUL: E-DCH mac-d flow ID %d, no idx found",
              e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity);
          return FAILURE;
        }
        else
        {
          if(rrcllc_restore_ul_emac_rb_mapping_info(e_mac_d_flow_idx) == FAILURE)
          {
            /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
            WRRC_MSG1_ERROR("RRCEUL: E-DCH mac-d flow ID %d, not correct info found in Stanalone rb-mapping info",
                e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowIdentity);
          }
          rrcllc_update_mac_eul_action(TRUE);
          /* Only allowed to be absent when already defined for this E-DCH MAC-d flow*/
          if (RRC_MSG_LIST_BITMASK_IE(e_mac_d_flow_list_ptr,
        rrc_E_DCH_AddReconf_MAC_d_Flow_r7,mac_d_FlowPowerOffset))
          {
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
              e_mac_d_flow_power_offset = e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowPowerOffset;
          }
          else
          {
            if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                e_mac_d_flow_power_offset > RRCLLC_MAX_E_MAC_D_FLOW_PWR_OFFSET)
            {
              WRRC_MSG0_ERROR("RRCEUL: E-MAC-d flow pwr offset needs to be provided");
              return FAILURE;
            }
          }
          /* Only allowed to be absent when already defined for this E-DCH MAC-d flow */
          if (RRC_MSG_LIST_BITMASK_IE(e_mac_d_flow_list_ptr,rrc_E_DCH_AddReconf_MAC_d_Flow_r7,
        mac_d_FlowMaxRetrans))
          {
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
              e_mac_d_flow_max_num_of_retx = e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowMaxRetrans;
          }
          else
          {
            if(ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
               e_mac_d_flow_max_num_of_retx > RRCLLC_MAX_E_MAC_D_FLOW_NUM_RETX)
            {
              WRRC_MSG0_ERROR("RRCEUL: E-MAC-d max num of retx needs to be provided");
              return FAILURE;
            }
          }
          
          /* if no mux list is provided, use the previous one.  If there is no previous then allow multiplexing of 
             MAC-d PDU's from the E-DCH MAC-d flow indicated in the IE "E-DCH MAC-d flow identity" with MAC-d PDU's 
             from any other E-DCH MAC-d flow in the same MAC-e PDU.*/
          if (RRC_MSG_LIST_BITMASK_IE(e_mac_d_flow_list_ptr,
            rrc_E_DCH_AddReconf_MAC_d_Flow_r7,mac_d_FlowMultiplexingList))
          {
            /* Only look at first byte of bitstring as max e_mac_d flow is 8*/
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].e_mac_d_flow_mux_list =
              rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(
                &e_mac_d_flow_list_ptr->elem[idx].mac_d_FlowMultiplexingList);
          }
          
          if (RRC_MSG_LIST_BITMASK_IE(e_mac_d_flow_list_ptr,
              rrc_E_DCH_AddReconf_MAC_d_Flow_r7,transmissionGrantType))
          {
            if (e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.t == 
                T_rrc_E_DCH_AddReconf_MAC_d_Flow_r7_transmissionGrantType_non_ScheduledTransGrantInfo)
            {
              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                e_tx_grant_info = MAC_E_TX_GRANT_NON_SCHEDULED;

              if(e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.t ==
                T_rrc_E_DCH_AddReconf_MAC_d_Flow_r7_modeSpecificInfo_fdd)
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.max_mac_e_pdu_size_in_bits = 
                    e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.u.fdd->maxMAC_e_PDUContents;

              if ((e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.t ==
                  T_rrc_E_DCH_AddReconf_MAC_d_Flow_r7_modeSpecificInfo_fdd) && 
                  (e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.u.fdd->m.
                  ms2_NonSchedTransmGrantHARQAllocPresent))
              {
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 
                    rrcllcpcie_convert_asn1_oss_bitstring_to_uint8(
                      &e_mac_d_flow_list_ptr->elem[idx].transmissionGrantType.u.non_ScheduledTransGrantInfo->modeSpecificInfo.u.fdd->ms2_NonSchedTransmGrantHARQAlloc);
              }
              else /* Default value is: transmission in all HARQ processes is allowed. so set all to 1 */
              {
                ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].
                  non_sched_tx_grant.non_sched_tx_grant_harq_allocation_2ms = 0xFF;
              }
            }
            else /* rrc_scheduledTransmissionGrantInfo */
            {
              ordered_config_ptr->mac_e_config.e_mac_d_flow_info[e_mac_d_flow_idx].e_tx_grant_info = MAC_E_TX_GRANT_SCHEDULED;
            }
          }
        }

        idx++;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("RRCEUL: E-DCH mac-d flow list is NULL");
      return FAILURE;
    }
  }
  return SUCCESS;
}

/*============================================================================
FUNCTION: rrcllc_store_dl_add_reconf_trch_info_list_r8()

DESCRIPTION:
  This function updates IE "dl added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_dl_add_reconf_trch_info_list_r8
(
  OSS_UINT32 dl_add_reconf_trch_info_list_present,
  rrc_DL_AddReconfTransChInfoList_r7 * dl_trch_add_reconfig_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r8  * ul_trch_add_reconfig_ptr  
)
{
  uint32 idx, tf_idx;
  /* Get the DL Added or Reconfigured Transport Channel information */
  if(dl_add_reconf_trch_info_list_present)
  {
      
    idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (dl_trch_add_reconfig_ptr->n > idx))
    {
      if(get_dl_added_reconfig_trch_r8(&dl_trch_add_reconfig_ptr->elem[idx],
          ul_trch_add_reconfig_ptr
        ) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;
    } /* end while */
  } 
  
  /* At this point store the Queue Information in L1 structure. To add later if HSDSCH config changed */
  
  /* Adjust TB size on every TrCH if necessary. */ 
  for (idx=0; idx<UE_MAX_TRCH; idx++)
  {
  /* The number of logical channel mapped to this transport channel 
  increases from 1 to 2. Needs to adjsut the tb_szie to
    reflect the adding of the MAC header.                         */
    if (ordered_config_ptr->dl_dch_trch_info[idx].dch_present &&
      (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up ||
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down))
    {
      /* TrCH IE is present and the TrCH ID is appear in the TrCH info table. */
      if (!(dl_add_reconf_trch_info_list_present &&
        find_trch_id_in_config_trch_list_r7(dl_trch_add_reconfig_ptr,
        ordered_config_ptr->dl_dch_trch_info[idx].trch_id)))
      {
        /* Added MAC head size to the current TB size.  */
        if (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          !ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Increasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size += 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
        /* Substract MAC head size from the current TB size.  */
        else if (!ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Decreasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size -= 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
      }
      WRRC_MSG0_HIGH("Clean TB size adjust flag in RBSetup");
      /* Reset lg_ch_cnt_up and lg_ch_cnt_down. */
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up = FALSE;
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down = FALSE;
    }
    else if (!ordered_config_ptr->dl_dch_trch_info[idx].dch_present)
    {
    /* Since the activated transport channels are sorted before stored in
      the memory, break the looping after reaching the 1st invalid block.  */
      break;
    }
  }
  
  /* Check to make sure DL TFS count is within UE Capabilities */
  if(rrcllc_check_dl_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}

/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_r8()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_dl_added_reconfig_trch_r8
(
  rrc_DL_AddReconfTransChInformation_r7 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList_r8 * ul_ptr_r8
 
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = 0;
  tr_ch_id_type ul_trch_idx = 0;
  uint32 idx=0,idy=0;


  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  
  /* Loop variables for copying UL TFS info */
  uint8 tf_idx = 0;
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  
  /* Boolean used to indicate if multiple logical channels are mapped to
  a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;
  
  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;
  
  rrc_MAC_hs_AddReconfQueue_List* add_queue_list_ptr = NULL;
  
  rrc_MAC_hs_DelQueue_List* delete_queue_list_ptr = NULL;
  
  
  MSG_MED("Get DL Add/Reconf TrCH",0,0,0);
  
  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    /* Get the transport channel index for this transport channel ID.
      * Note: The index should already be there. */
    WRRC_MSG1_MED("RRCHS:TrCh type to be added",ie_ptr->dl_TransportChannelType.t);
    
    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
    rrc_DL_TrCH_TypeId1_r5_dch))
      {
        
        trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
        
        if(trch_idx == RRCLLC_INVALID_TRCH_IDX)
        {
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found", 
            ie_ptr->dl_TransportChannelType.u.dch);
          return(FAILURE);
        }

        if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
        {
          /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping", 
               ie_ptr->dl_TransportChannelType.u.dch);
        } 
      }
      
      /* Determine if multiple dedicated logical channels are present on this TrCH id */
      if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
      {
        multiple_ded_log_chan = TRUE;
      }

    /*
      * Get whether or not the TFS for a given TRCH is included or if it needs
      * to come from an uplink TrCH ID.
      */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_explicit_config:
        
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
          &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
          multiple_ded_log_chan, /* mulitple logical channels? */
          &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
          ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */
        
      case T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_sameAsULTrCH:
        idx =0;
        while(ul_ptr_r8 != NULL && ul_ptr_r8->n > idx)
        {
          /* check whether trCh is of type rrc_UL_AddReconfTransChInformation_r6_dch_usch */
          if((RRC_CHECK_LIST_MSG_TYPE(ul_ptr_r8,rrc_UL_AddReconfTransChInformation_r8_dch_usch)) &&
             (ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity == 
              ul_ptr_r8->elem[idx].u.dch_usch->transportChannelIdentity))
          { 
            /* Got a match with Uplink TrCH ID */
            WRRC_MSG2_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
              ul_ptr_r8->elem[idx].u.dch_usch->transportChannelIdentity);
            if(get_dl_transport_format_set(&ul_ptr_r8->elem[idx].u.dch_usch->transportFormatSet,
              &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
              multiple_ded_log_chan, /* mulitple logical channels? */
              &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
              ) == FAILURE)
            {
              return FAILURE;
            }
            
            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */
          
          /* no match, keep looking */
          else 
          {
            idx++;
          }
        }
               
        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
          * Note: The index should already be there.
          */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
            ordered_config_ptr,
            (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
            ul_TransportChannelIdentity);
          if(ul_trch_idx == RRCLLC_INVALID_TRCH_IDX)
          {
            WRRC_MSG2_ERROR("No DL TFS for TrCH %d, to use UL TrCH %d for TFS",
              ie_ptr->dl_TransportChannelType.u.dch,
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity);
            return(FAILURE);
          }
          
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
            case L1_UL_CODING_NONE:
              dl_tfs_info->coding_type = L1_DL_UNCODED;
              break;
            case L1_UL_CODING_CONV_ONE_HALF:
              dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
              break;
            case L1_UL_CODING_CONV_ONE_THIRD:
              dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
              break;
            case L1_UL_CODING_TURBO:
              dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
              break;
            default:
              break; 
          }
          
          
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
            case L1_UL_CRC_0_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
              break;
            case L1_UL_CRC_8_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
              break;
            case L1_UL_CRC_12_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
              break;
            case L1_UL_CRC_16_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
              break;
            case L1_UL_CRC_24_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
              return FAILURE;
          }
          
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8) ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;
            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);

            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if ((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }

            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 
        
      case T_rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_hsdsch:
        /* The DFLOW Index has already been added. So get the index */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
   rrc,dl_MAC_HeaderType))
        {
          if (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.t == T_rrc_HSDSCH_Info_r7_dl_MAC_HeaderType_mac_hs)
          {
  
            /* Check if add & delete queue both are present and network specifies the same 
            queue id in both. This needs to be rejected */
            if ((ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->m.
               mac_hs_AddReconfQueue_ListPresent) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List.n > 0) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->m.
              mac_hs_DelQueue_ListPresent) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_DelQueue_List.n > 0))
            {
              /* Check if UTRAN messed up by specifying the same queue id */
              delete_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_DelQueue_List;
              idx =0;
              while (delete_queue_list_ptr->n > idx)
              {
                add_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List;
                idy =0;
                while (add_queue_list_ptr->n > idy)
                {
                  if (delete_queue_list_ptr->elem[idx].mac_hsQueueId == add_queue_list_ptr->elem[idy].mac_hsQueueId)
                  {
                    /* Invalid Config */
                    WRRC_MSG1_ERROR("RRCHS:Queue %d simul added & deleted", delete_queue_list_ptr->elem[idx].mac_hsQueueId);
                    return FAILURE;
                  }
                  idy++;
                }
                
                idx++;
              }
            }
            
            if (set_dflow_info(ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs) == FAILURE)
            {
              WRRC_MSG0_ERROR("RRCHS:Invalid DFLOW info");
              return FAILURE;
            }
          }
        }
        /* SET HARQ info in L1 structure */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
   rrc_HSDSCH_Info_r7,harqInfo))
        {
          if (set_harq_information_r7(&ie_ptr->tfs_SignallingMode.u.hsdsch->harqInfo) == FAILURE)
          {
            WRRC_MSG0_ERROR("RRCHS:Unable to set HARQ information");
            return FAILURE;
          }
        }
        
        /* Indicate that HSDSCH Info is present which would be used later to set bit mask when HSDPA
        is already ACTIVE */
        hsdpa_msg_params.hsdsch_info_present = TRUE;
        
        hsdpa_msg_params.msg_params_present = TRUE;
        
        break;
        
      default:
        return FAILURE;
      
    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
        PDU size has changed.*/ 
    if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId1_r5_dch))
    {
      rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch, 
                ordered_config_ptr->rrc_state_indicator, DL);  
    }
  } /* end CELL_DCH check */
  
  /* States not supported */
  else
  {
    WRRC_MSG1_ERROR("State %d not supported for TrCH",ordered_config_ptr->rrc_state_indicator);
    return FAILURE;
  }
  
  /***************************************************************/
  /* Transport Channel Count has been updated in RB Mapping info */
  /***************************************************************/
  if (!RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->tfs_SignallingMode,
   rrc_DL_AddReconfTransChInformation_r7_tfs_SignallingMode_hsdsch) )
  {
    /* Check for the BLER */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
  rrc_DL_AddReconfTransChInformation_r7,dch_QualityTarget))
    {
      /* Save the BLER if found, and is in range */
      if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
        (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER))
      {
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
          (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
      }
      
      /* BLER is out of range */
      else
      {
        /* According to L1 I/F, must set an invalid value of -127 */
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
        WRRC_MSG1_ERROR("DL BLER %d is out of range",
          ie_ptr->dch_QualityTarget.bler_QualityValue);
        return FAILURE;
      }
    } /* end if BLER present */
    else
    {
      /* IE omitted. Set the target to -127 which would result 1% of BLER target.
       * This is good UE behaviour though the spec is ambiguous on what UE
       * behavior should be in this case.  
       */
      WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
    }
  }
  else
  {
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation_r7,dch_QualityTarget))
    {
      WRRC_MSG0_HIGH("Ignoring dch quality target for HS");
    }
  }
  /* Indicate that DL L1 needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
  
  return SUCCESS;
}


#ifdef FEATURE_WCDMA_REL9
/*====================================================================
FUNCTION: get_hs_window_size_r9()

DESCRIPTION:
  
DEPENDENCIES:
  Gets the window size associated with the queue.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
uint8 get_hs_window_size_r9
(
  rrc_MAC_hs_WindowSize_r9 mac_hsWindowSize
)
{
  uint8 w_size;

  switch (mac_hsWindowSize)
  {
    case rrc_MAC_hs_WindowSize_r9_mws4 : 
      w_size = 4;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mws6 :
      w_size = 6;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mws8:
      w_size = 8;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mws12:
      w_size = 12;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mws16:
      w_size = 16;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mws24:
      w_size = 24;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mws32:
      w_size = 32;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mw64:
      w_size = 64;
      break;
      
    case rrc_MAC_hs_WindowSize_r9_mw128:
      w_size = 128;
      break;
      
    default: /* Not Possible */
      w_size = 0;
      break;
      
  }
  MSG_MED("RRCHS:WindowSize %d",w_size,0,0);
  return w_size;
}


/*====================================================================
FUNCTION: rrcllc_add_macehs_r9()

DESCRIPTION:
  
DEPENDENCIES:


RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  
====================================================================*/
static uecomdef_status_e_type rrcllc_add_macehs_r9
(
 rrc_AddOrReconfMAC_ehs_ReordQ_r9 * mac_ehs_info
)
{
  rrc_MAC_ehs_AddReconfReordQ_List_r9 * macehs_queue_head_ptr;
  uint8 macehs_q_id;
  uint32 idx =0;
    /* Now find the MAC EHS Index for ADD/RECONFIG.  */
  if ((RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mac_ehs_info,mac_ehs_AddReconfQueue_List))
     &&(mac_ehs_info->mac_ehs_AddReconfQueue_List.n > 0))
  {
    macehs_queue_head_ptr = &mac_ehs_info->mac_ehs_AddReconfQueue_List;
    while (macehs_queue_head_ptr->n > idx)
    {
      if (macehs_queue_head_ptr->elem[idx].mac_ehs_QueueId > UE_MAX_HS_QUEUE-1)
      {
        WRRC_MSG1_ERROR("MACEHS:MAC EHS Q Id %d, Range 0..7",
            macehs_queue_head_ptr->elem[idx].mac_ehs_QueueId);
        return FAILURE;
      }

      /* If Queue Index found then return it Else allocate a new index */
      if ((macehs_q_id = rrcllc_get_macehs_queue_index((uint8)macehs_queue_head_ptr->elem[idx].mac_ehs_QueueId)) 
        == UE_MAX_HS_QUEUE)
      {
        return FAILURE;
      }
      else
      {
      
        /* End of while of pdu_list_ptr */       
        ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].window_size = 
          get_hs_window_size_r9(macehs_queue_head_ptr->elem[idx].mac_ehsWindowSize);
        
        ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].release_timer = 
          get_hs_release_timer(macehs_queue_head_ptr->elem[idx].reorderingReleaseTimer);
           /*PSR TBD  reorderingResetTimer   in 7.6       */
      }
#ifdef FEATURE_WCDMA_HS_FACH
        if(macehs_queue_head_ptr->elem[idx].m.reorderingResetTimerPresent)
        {
          ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].reset_timer = 
            macehs_queue_head_ptr->elem[idx].reorderingResetTimer + 1;
        }
        else
        {
          /*Default value is 2*/
          ordered_config_ptr->mac_ehs_queue_info[macehs_q_id].reset_timer = 2;
        }
        
#endif 

        idx++;
    } /* End of while */       
    
  }
  return SUCCESS;
}


/*============================================================================
FUNCTION: set_macehs_q_info_r9()

DESCRIPTION:
  
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type macehs_add_or_del_q_info_r9
( 
  rrc_AddOrReconfMAC_ehs_ReordQ_r9 * mac_ehs_info
)
{  

  /* Get the add/reconfig MAC D FLOW Info */
  if ( (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(mac_ehs_info,
    mac_ehs_AddReconfQueue_List)) &&
      (mac_ehs_info->mac_ehs_AddReconfQueue_List.n > 0))
  {
    if(rrcllc_add_macehs_r9(mac_ehs_info) == FAILURE)
    {
      return FAILURE;
    }
  }
  else
  {
    return SUCCESS;
  }
    
  return SUCCESS;
}


/*================================================================================================
FUNCTION: get_dl_added_reconfig_trch_r9()

DESCRIPTION:
  This function takes a pointer to the Uplink Added or Reconfigured 
  Transport Channels IE and copies it to ORDERED_CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  CELL_DCH additions are the only item supported now.
===============================================================================================*/
uecomdef_status_e_type get_dl_added_reconfig_trch_r9
(
  rrc_DL_AddReconfTransChInformation_r9 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList_r8 * ul_ptr_r8
 
)
{
  /* local transport channel variables */
  tr_ch_id_type trch_idx = 0;
  tr_ch_id_type ul_trch_idx = 0;
  uint32 idx=0,idy=0;


  /* Pointers to UL and DL TFS info for use when copying existing config */
  l1_dl_trch_info_struct_type *dl_tfs_info = NULL;
  l1_ul_semi_static_parms_struct_type *ul_tfs_ss_info = NULL;
  mac_ul_tfs_type *ul_tfs_dyn_info = NULL;
  
  /* Loop variables for copying UL TFS info */
  uint8 tf_idx = 0;
  l1_dl_tf_info_struct_type *rrc_dl_tf_info = NULL;
  mac_ul_tf_type *ul_tf_info = NULL;
  
  /* Boolean used to indicate if multiple logical channels are mapped to
  a given TrCH. */
  boolean       multiple_ded_log_chan = FALSE;
  
  /* local variable indicating whether or not an uplink TrCH ID was found */
  boolean       got_one = FALSE;
  
  rrc_MAC_hs_AddReconfQueue_List* add_queue_list_ptr = NULL;
  
  rrc_MAC_hs_DelQueue_List* delete_queue_list_ptr = NULL;
  
  
  MSG_MED("Get DL Add/Reconf TrCH",0,0,0);
  
  /*
  * Use the Transport Channel Index and next state to place the TFS in 
  * the correct slot in the MAC TFS array.
  */
  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    WRRC_MSG1_MED("RRCHS:TrCh type to be added",ie_ptr->dl_TransportChannelType.t);

    /* Get the transport channel index for this transport channel ID.
      * Note: The index should already be there. */
    
    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,
    rrc_DL_TrCH_TypeId1_r5_dch))
      {
        
        trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch);
        
        if(trch_idx == RRCLLC_INVALID_TRCH_IDX)
        {
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found", 
            ie_ptr->dl_TransportChannelType.u.dch);
          return(FAILURE);
        }

        if(rrcllc_restore_dl_dch_rb_mapping_info(trch_idx) == FAILURE)
        {
          /* Validation was done prior to adding it to the Standalone RB-Mapping backup info */
          WRRC_MSG1_ERROR("DL DCH TrCH ID %d, not correct in Standalone RB-Mapping", 
               ie_ptr->dl_TransportChannelType.u.dch);
        } 
      }
      
      /* Determine if multiple dedicated logical channels are present on this TrCH id */
      if(ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt > 1)
      {
        multiple_ded_log_chan = TRUE;
      }
    /*
      * Get whether or not the TFS for a given TRCH is included or if it needs
      * to come from an uplink TrCH ID.
      */
    switch(ie_ptr->tfs_SignallingMode.t)
    {
      /* TFS explicitly given for this Transport Channel */
      case T_rrc_DL_AddReconfTransChInformation_r9_tfs_SignallingMode_explicit_config:
        
        /* get the Transport Format Set for this TrCH */
        if(get_dl_transport_format_set(ie_ptr->tfs_SignallingMode.u.explicit_config,
          &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
          multiple_ded_log_chan, /* mulitple logical channels? */
          &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
          ) == FAILURE)
        {
          return FAILURE;
        }
        break; /* end explicit TFS signalling for DL TrCH */
        
      case T_rrc_DL_AddReconfTransChInformation_r9_tfs_SignallingMode_sameAsULTrCH:
        idx =0;
        while(ul_ptr_r8 != NULL && ul_ptr_r8->n > idx)
        {
          /* check whether trCh is of type rrc_UL_AddReconfTransChInformation_r6_dch_usch */
          if((RRC_CHECK_LIST_MSG_TYPE(ul_ptr_r8,rrc_UL_AddReconfTransChInformation_r8_dch_usch)) &&
             (ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity == 
              ul_ptr_r8->elem[idx].u.dch_usch->transportChannelIdentity))
          { 
            /* Got a match with Uplink TrCH ID */
            WRRC_MSG2_MED("Get TFS for DL TrCH Id %d using UL TrCH ID %d", 
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity,
              ul_ptr_r8->elem[idx].u.dch_usch->transportChannelIdentity);
            if(get_dl_transport_format_set(&ul_ptr_r8->elem[idx].u.dch_usch->transportFormatSet,
              &ordered_config_ptr->dl_trch_info[trch_idx],  /* dedicated trch */
              multiple_ded_log_chan, /* mulitple logical channels? */
              &ordered_config_ptr->dl_trch_info[trch_idx]   /* common trch */
              ) == FAILURE)
            {
              return FAILURE;
            }
            
            /* Indicate that found an UL TrCH ID, and break the while loop */
            got_one = TRUE;
            break;
          } /* end UL TrCH match */
          
          /* no match, keep looking */
          else 
          {
            idx++;
          }
        }
               
        /* Make sure we found the UL TrCH, before continuing */
        if(!got_one)
        {
          WRRC_MSG0_HIGH("No matching UL TFS in msg, look up from OC");
          /* Get the transport channel index for the UL transport channel ID.
          * Note: The index should already be there.
          */
          ul_trch_idx = rrcllc_find_ul_dch_trch_idx(
            ordered_config_ptr,
            (tr_ch_id_type)ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->
            ul_TransportChannelIdentity);
          if(ul_trch_idx == RRCLLC_INVALID_TRCH_IDX)
          {
            WRRC_MSG2_ERROR("No DL TFS for TrCH %d, to use UL TrCH %d for TFS",
              ie_ptr->dl_TransportChannelType.u.dch,
              ie_ptr->tfs_SignallingMode.u.sameAsULTrCH->ul_TransportChannelIdentity);
            return(FAILURE);
          }
          
          /* Set up convenience pointers */
          dl_tfs_info = &ordered_config_ptr->dl_trch_info[trch_idx];
          ul_tfs_ss_info = &ordered_config_ptr->rrc_ul_cctrch_params.semi_static_parms[ul_trch_idx];
          ul_tfs_dyn_info = &ordered_config_ptr->ul_tfcs.tfs_info[ul_trch_idx];
          
          /* Copy semistatic info from uplink */
          switch(ul_tfs_ss_info->coding)
          {
            case L1_UL_CODING_NONE:
              dl_tfs_info->coding_type = L1_DL_UNCODED;
              break;
            case L1_UL_CODING_CONV_ONE_HALF:
              dl_tfs_info->coding_type = L1_DL_CONV_1_BY_2_CODING;
              break;
            case L1_UL_CODING_CONV_ONE_THIRD:
              dl_tfs_info->coding_type = L1_DL_CONV_1_BY_3_CODING;
              break;
            case L1_UL_CODING_TURBO:
              dl_tfs_info->coding_type = L1_DL_TURBO_CODING;
              break;
            default:
              break; 
          }
          
          
          dl_tfs_info->rate_match_attrib = ul_tfs_ss_info->staticRM;
          switch(ul_tfs_ss_info->crc)
          {
            case L1_UL_CRC_0_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_0;
              break;
            case L1_UL_CRC_8_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_8;
              break;
            case L1_UL_CRC_12_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_12;
              break;
            case L1_UL_CRC_16_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_16;
              break;
            case L1_UL_CRC_24_BITS:
              dl_tfs_info->crc_length = L1_CRC_LENGTH_24;
              break;
            default:
              WRRC_MSG1_ERROR("Invalid CRC length %d in UL", ul_tfs_ss_info->crc);
              return FAILURE;
          }
          
          /* Copy dynamic info from uplink */
          dl_tfs_info->tti_value = ul_tfs_dyn_info->tti_info;
          dl_tfs_info->num_tf = (uint8) ul_tfs_dyn_info->ntf;
          for(tf_idx = 0; tf_idx < dl_tfs_info->num_tf; tf_idx++)
          {
            rrc_dl_tf_info = dl_tfs_info->tf_info_array[tf_idx];
            ul_tf_info = &ul_tfs_dyn_info->tf_info[tf_idx];
            
            rrc_dl_tf_info->num_tb = (uint8)ul_tf_info->ntblks;
            rrc_dl_tf_info->tb_size = ul_tf_info->tb_size;
            WRRC_MSG2_HIGH("TB Size: %d, multiple_ded_log_chan: %d", 
                     rrc_dl_tf_info->tb_size, multiple_ded_log_chan);

            /* If number of logical channel is > 1, inc tb_size by 4 to accomodate MAC-header */
            if ((rrc_dl_tf_info->tb_size > 0) && (multiple_ded_log_chan == TRUE))
            {
              rrc_dl_tf_info->tb_size += RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
              WRRC_MSG1_HIGH("New TB Size %d", rrc_dl_tf_info->tb_size);
            }

            rrc_dl_tf_info->allowed = ul_tf_info->tf_flag;
          }
        }
        break; /* end use of previous signalled UL TrCH TFS for DL TrCH */ 
        
      case T_rrc_DL_AddReconfTransChInformation_r9_tfs_SignallingMode_hsdsch:
        /* The DFLOW Index has already been added. So get the index */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,rrc_HSDSCH_Info_r9,dl_MAC_HeaderType))
        {
          if (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.t == T_rrc_HSDSCH_Info_r9_dl_MAC_HeaderType_mac_hs)
          {
  
            /* Check if add & delete queue both are present and network specifies the same 
            queue id in both. This needs to be rejected */
            if ((ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->m.
               mac_hs_AddReconfQueue_ListPresent) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List.n > 0) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->m.
              mac_hs_DelQueue_ListPresent) &&
              (ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_DelQueue_List.n > 0))
            {
              /* Check if UTRAN messed up by specifying the same queue id */
              delete_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_DelQueue_List;
              idx =0;
              while (delete_queue_list_ptr->n > idx)
              {
                add_queue_list_ptr = &ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List;
                idy =0;
                while (add_queue_list_ptr->n > idy)
                {
                  if (delete_queue_list_ptr->elem[idx].mac_hsQueueId == add_queue_list_ptr->elem[idy].mac_hsQueueId)
                  {
                    /* Invalid Config */
                    WRRC_MSG1_ERROR("RRCHS:Queue %d simul added & deleted", delete_queue_list_ptr->elem[idx].mac_hsQueueId);
                    return FAILURE;
                  }
                  idy++;
                }
                
                idx++;
              }
            }
            
            if (set_dflow_info(ie_ptr->tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_hs) == FAILURE)
            {
              WRRC_MSG0_ERROR("RRCHS:Invalid DFLOW info");
              return FAILURE;
            }
          }
        }
        /* SET HARQ info in L1 structure */
        if (RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(ie_ptr->tfs_SignallingMode.u.hsdsch,
   rrc_HSDSCH_Info_r9,harqInfo))
        {
          if (set_harq_information_r7(&ie_ptr->tfs_SignallingMode.u.hsdsch->harqInfo) == FAILURE)
          {
            WRRC_MSG0_ERROR("RRCHS:Unable to set HARQ information");
            return FAILURE;
          }
        }
        
        /* Indicate that HSDSCH Info is present which would be used later to set bit mask when HSDPA
        is already ACTIVE */
        hsdpa_msg_params.hsdsch_info_present = TRUE;
        
        hsdpa_msg_params.msg_params_present = TRUE;
        
        break;
        
      default:
        return FAILURE;
      
    } /* end TFS selection switch */

    /*Find Rbs mapped to this DCH and fill them in rb_list_mapped_to_dch if their RLC
        PDU size has changed.*/ 
    if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_TransportChannelType,rrc_DL_TrCH_TypeId1_r5_dch))
    {
      rrcllc_get_rb_for_rb_list_mapped_to_dch((tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch, 
                ordered_config_ptr->rrc_state_indicator, DL);  
    }
  } /* end CELL_DCH check */
  
  /* States not supported */
  else
  {
    WRRC_MSG1_ERROR("State %d not supported for TrCH",ordered_config_ptr->rrc_state_indicator);
    return FAILURE;
  }
  
  /***************************************************************/
  /* Transport Channel Count has been updated in RB Mapping info */
  /***************************************************************/
  if (!RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->tfs_SignallingMode,
   rrc_DL_AddReconfTransChInformation_r9_tfs_SignallingMode_hsdsch) )
  {
    /* Check for the BLER */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,
  rrc_DL_AddReconfTransChInformation_r9,dch_QualityTarget))
    {
      /* Save the BLER if found, and is in range */
      if((ie_ptr->dch_QualityTarget.bler_QualityValue >= RRCLLC_MIN_BLER) &&
        (ie_ptr->dch_QualityTarget.bler_QualityValue <= RRCLLC_MAX_BLER))
      {
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val =
          (int8)ie_ptr->dch_QualityTarget.bler_QualityValue;
      }
      
      /* BLER is out of range */
      else
      {
        /* According to L1 I/F, must set an invalid value of -127 */
        ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
        WRRC_MSG1_ERROR("DL BLER %d is out of range",ie_ptr->dch_QualityTarget.bler_QualityValue);
        return FAILURE;
      }
    } /* end if BLER present */
    else
    {
      /* IE omitted. Set the target to -127 which would result 1% of BLER target.
       * This is good UE behaviour though the spec is ambiguous on what UE
       * behavior should be in this case.  
       */
      WRRC_MSG0_MED("DCH quality target not present. Setting to -127");
      ordered_config_ptr->dl_trch_info[trch_idx].bler_quality_val = RRCLLC_BLER_NA;
    }
  }
  else
  {
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_AddReconfTransChInformation_r9,dch_QualityTarget))
    {
      WRRC_MSG0_HIGH("Ignoring dch quality target for HS");
    }
  }
  /* Indicate that DL L1 needs to be reconfigured */
  ordered_config_ptr->reconfig_needed.downlink_l1 = TRUE;
  ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
  
  return SUCCESS;
}



/*============================================================================
FUNCTION: rrcllc_store_dl_add_reconf_trch_info_list_r9()

DESCRIPTION:
  This function updates IE "dl added reconfig trch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_store_dl_add_reconf_trch_info_list_r9
(
  OSS_UINT32 dl_add_reconf_trch_info_list_present,
  rrc_DL_AddReconfTransChInfoList_r9 * dl_trch_add_reconfig_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r8  * ul_trch_add_reconfig_ptr  
)
{
  uint32 idx, tf_idx;
  /* Get the DL Added or Reconfigured Transport Channel information */
  if(dl_add_reconf_trch_info_list_present)
  {
      
    idx = 0;
    
    /* Save info for each additional Transport Channel */
    while((idx <= UE_MAX_TRCH) && (dl_trch_add_reconfig_ptr->n > idx))
    {
      if(get_dl_added_reconfig_trch_r9(&dl_trch_add_reconfig_ptr->elem[idx],
          ul_trch_add_reconfig_ptr
        ) == FAILURE)
      {
        return FAILURE;
      }
      /* get next TrCH Info */
      idx++;
    } /* end while */
  } 
  
  /* At this point store the Queue Information in L1 structure. To add later if HSDSCH config changed */
  
  /* Adjust TB size on every TrCH if necessary. */ 
  for (idx=0; idx<UE_MAX_TRCH; idx++)
  {
  /* The number of logical channel mapped to this transport channel 
  increases from 1 to 2. Needs to adjsut the tb_szie to
    reflect the adding of the MAC header.                         */
    if (ordered_config_ptr->dl_dch_trch_info[idx].dch_present &&
      (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up ||
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down))
    {
      /* TrCH IE is present and the TrCH ID is appear in the TrCH info table. */
      if (!(dl_add_reconf_trch_info_list_present &&
        find_trch_id_in_config_trch_list_r9(dl_trch_add_reconfig_ptr,
        ordered_config_ptr->dl_dch_trch_info[idx].trch_id)))
      {
        /* Added MAC head size to the current TB size.  */
        if (ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          !ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Increasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size += 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
        /* Substract MAC head size from the current TB size.  */
        else if (!ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up &&
          ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down)
        {
          WRRC_MSG2_HIGH("Decreasing TrCH %d TB size, %d by MAC hdr size", 
                   ordered_config_ptr->dl_dch_trch_info[idx].trch_id, 
                   ordered_config_ptr->dl_trch_info[idx].tf_info_array[0]->tb_size);
          for (tf_idx=0; tf_idx<ordered_config_ptr->dl_trch_info[idx].num_tf; tf_idx++)
          {
            if (ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size > 0)
            {
              ordered_config_ptr->dl_trch_info[idx].tf_info_array[tf_idx]->tb_size -= 
                RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE;
            }
          }
        }
      }
      WRRC_MSG0_HIGH("Clean TB size adjust flag in RBSetup");
      /* Reset lg_ch_cnt_up and lg_ch_cnt_down. */
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_up = FALSE;
      ordered_config_ptr->dl_dch_trch_info[idx].lg_ch_cnt_down = FALSE;
    }
    else if (!ordered_config_ptr->dl_dch_trch_info[idx].dch_present)
    {
    /* Since the activated transport channels are sorted before stored in
      the memory, break the looping after reaching the 1st invalid block.  */
      break;
    }
  }
  
  /* Check to make sure DL TFS count is within UE Capabilities */
  if(rrcllc_check_dl_tfs_cnt() == FAILURE)
  {
    return FAILURE;
  }

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_add_delete_and_arrange_dl_dch_trch_info_r9()

DESCRIPTION:
  This function takes up deleted and added DL DCH Transport channel
  List and updates local, MAC and L1 data structures with transport
  channel IDs and count. It also re-arranges transport channel info
  in local, MAC and L1 structures so that it is arranged and sorted
  by TrCH IDs. For HSDSCH, it adds, deletes the DFLOW and rearranges
  the DFLOW so that missing holes because of deleted DFLOWs are
  removed
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type
rrcllc_add_delete_and_arrange_dl_dch_trch_info_r9
(
  /* local DL TrCH Add Reconfig pointer */
  rrc_DL_AddReconfTransChInfoList_r9 * dl_trch_add_reconfig_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList_r7    * dl_trch_delete_ptr
)
{
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;

  uint8 dflow_index;
  rrc_MAC_hs_AddReconfQueue_List * dflow_queue_head_ptr = NULL;
  uint32 idx =0,idy =0,idz=0;

  rrc_DL_DeletedTransChInfoList_r7 *temp_dl_trch_delete_ptr = dl_trch_delete_ptr;
  rrc_DL_AddReconfTransChInfoList_r9  *temp_dl_trch_add_reconfig_ptr = dl_trch_add_reconfig_ptr;
  rrc_HSDSCH_Info_r9 *temp_hsdsch;
  rrc_MAC_ehs_AddReconfReordQ_List_r9 *temp_MAC_ehs_AddReconfReordQ_List;
  

  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    while(temp_dl_trch_delete_ptr->n > idx)
    {
      if(T_rrc_DL_TrCH_TypeId2_r7_dch == temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.t)
      {
        idy = 0;
        while(temp_dl_trch_add_reconfig_ptr->n > idy)
        {
          if(T_rrc_DL_TrCH_TypeId1_r5_dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.t)
          {
            if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.dch == temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.u.dch)
            {
              WRRC_MSG2_ERROR("Same Trch ID found in add/reconfig list %d and deleted list %d ",
                temp_dl_trch_add_reconfig_ptr->elem[idy].dl_TransportChannelType.u.dch,temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.dch);
              return FAILURE;
            }
          }
          idy++;
        }
      }
      idx++;
    }
  }


  /* 8.6.5.23  Added or reconfigured MAC-ehs reordering queue
  1>  if a MAC-ehs queue (identified by the IE "MAC-ehs queue Id") is included in both the IE "MAC-ehs queue to 
      add or reconfigure list" and the IE "DL HS-DSCH MAC-ehs reordering queue" within the IE "Deleted DL TrCH 
      information": 
      2>  the UE behaviour is unspecified. 
  */
  if((temp_dl_trch_delete_ptr != NULL) && (temp_dl_trch_add_reconfig_ptr != NULL))
  {
    for(idx=0;idx < temp_dl_trch_delete_ptr->n ;idx++)
    {
      if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.t != 
          T_rrc_DL_TrCH_TypeId2_r7_hsdsch )
        continue;
      if(temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.hsdsch->t !=
          T_rrc_DL_TrCH_TypeId2_r7_hsdsch_mac_ehs)
        continue;

      for(idy = 0;temp_dl_trch_add_reconfig_ptr->n > idy ;idy++)
      {
        if(temp_dl_trch_add_reconfig_ptr->elem[idy].tfs_SignallingMode.t != 
            T_rrc_DL_AddReconfTransChInformation_r9_tfs_SignallingMode_hsdsch)
          continue;
        
        temp_hsdsch = temp_dl_trch_add_reconfig_ptr->elem[idy].tfs_SignallingMode.u.hsdsch;
        if(!temp_hsdsch->m.dl_MAC_HeaderTypePresent)
          continue;
        if(temp_hsdsch->dl_MAC_HeaderType.t != T_rrc_HSDSCH_Info_r9_dl_MAC_HeaderType_mac_ehs)
          continue;
        if(!temp_hsdsch->dl_MAC_HeaderType.u.mac_ehs->m.mac_ehs_AddReconfQueue_ListPresent)
          continue;
        temp_MAC_ehs_AddReconfReordQ_List = &temp_hsdsch->dl_MAC_HeaderType.u.mac_ehs->mac_ehs_AddReconfQueue_List;

        for(idz=0;temp_MAC_ehs_AddReconfReordQ_List->n > idz;idz++)
        {
          if(temp_MAC_ehs_AddReconfReordQ_List->elem[idz].mac_ehs_QueueId ==
             temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.hsdsch->u.mac_ehs)
          {
            WRRC_MSG1_ERROR("Same MAC-ehs queue Id found in add/reconfig list and deleted list ",
              temp_dl_trch_delete_ptr->elem[idx].dl_TransportChannelType.u.hsdsch->u.mac_ehs);
            return FAILURE;
          }
        }
      }
    }
  }


  idx =0;
  idy =0;
  while((dl_trch_delete_ptr !=NULL) &&( dl_trch_delete_ptr->n > idx))
  {
  /* Remove this transport channel from MAC, L1 and local data.
  * and update transport channel count in L1, MAC and local data.
    */
    if(get_dl_deleted_trch_r7(&dl_trch_delete_ptr->elem[idx]) == FAILURE)
    {
      return FAILURE;
    }
    /* get next TrCH Info */
    idx++;
  } /* end while */
  idx =0;
  /* Get the DL Added/Reconfigured Info */
  while((dl_trch_add_reconfig_ptr !=NULL) && (
    dl_trch_add_reconfig_ptr->n > idx) )
  {
    /* Check if channel type is HSDPA */
    if (dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.t == 
      T_rrc_DL_TrCH_TypeId1_r5_hsdsch)
    {
      /* At least one HARQ/DFLOW has to be present */
      if ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.t == 
    T_rrc_DL_AddReconfTransChInformation_r9_tfs_SignallingMode_hsdsch)
        && ((dl_trch_add_reconfig_ptr->elem[idx].
        tfs_SignallingMode.u.hsdsch->m.dl_MAC_HeaderTypePresent)
        || (dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.
        u.hsdsch->m.harqInfoPresent)))
      {
        
        MSG_LOW("RRCHSPA+:Valid DFLOW/MACEhs specified",0,0,0);
      }
      else
      {
        WRRC_MSG0_ERROR("RRCHSPA+:HARQ,DFLOW both ABS.");
        return FAILURE;
      }
      if(dl_trch_add_reconfig_ptr->elem[idx].
        tfs_SignallingMode.u.hsdsch->m.dl_MAC_HeaderTypePresent)
      {
        if(dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.
   hsdsch->dl_MAC_HeaderType.t == T_rrc_HSDSCH_Info_r9_dl_MAC_HeaderType_mac_hs)
        {
          /* Now find the DFLOW Index for ADD/RECONFIG.  */
          if ((dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
     dl_MAC_HeaderType.u.mac_hs->m.mac_hs_AddReconfQueue_ListPresent) &&
            (dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
            dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List.n > 0))
          {
            dflow_queue_head_ptr = 
              &dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->
              dl_MAC_HeaderType.u.mac_hs->mac_hs_AddReconfQueue_List;
            idy =0;
            while (dflow_queue_head_ptr->n > idy)
            {
              if ((dflow_queue_head_ptr->elem[idy].mac_dFlowId > UE_MAX_MAC_D_FLOW-1) ||
                  (dflow_queue_head_ptr->elem[idy].mac_hsQueueId > UE_MAX_HS_QUEUE-1))
              {
                WRRC_MSG2_ERROR("RRCHS:DFLOW Id %d, Queue Id %d, Range 0..7", dflow_queue_head_ptr->elem[idy].mac_dFlowId,
                    dflow_queue_head_ptr->elem[idy].mac_hsQueueId);
                return FAILURE;
              }
    
              /* If DFLOW Index found then return it Else allocate a new index */
              if ((dflow_index = rrcllc_get_dflow_index((uint8)dflow_queue_head_ptr->elem[idy].mac_dFlowId)) 
                == UE_MAX_MAC_D_FLOW)
              {
                WRRC_MSG1_HIGH("RRCHS:DFLOW index %d absent",dflow_index);
                return FAILURE;
              }
              else
              {
                idy++;
              }
            } /* End of while */       
            
          } /* End of if of add queue present */
        }
        else if(dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.
   hsdsch->dl_MAC_HeaderType.t == T_rrc_HSDSCH_Info_r9_dl_MAC_HeaderType_mac_ehs)
        {
          if(macehs_add_or_del_q_info_r9(dl_trch_add_reconfig_ptr->elem[idx].tfs_SignallingMode.u.hsdsch->dl_MAC_HeaderType.u.mac_ehs) == FAILURE)            
          {
             return FAILURE;
          }
        }
      }
    } /* End of if of rrc_DL_TrCH_TypeId1_r5_hsdsch chosen */
    else if (dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.t == 
      T_rrc_DL_TrCH_TypeId1_r5_dch)
    {
      trch_id = (tr_ch_id_type)dl_trch_add_reconfig_ptr->elem[idx].dl_TransportChannelType.u.dch;
      
      if((rrcllc_get_dl_dch_trch_idx(trch_id)) == RRCLLC_INVALID_TRCH_IDX)
      {
        WRRC_MSG1_ERROR("Couldn't find idx, TrCH ID %d", trch_id);
        return(FAILURE);
      }
    }
    else
    {
      WRRC_MSG0_ERROR("DSCH unsupported");
      return FAILURE;
    }
    idx++;
  } /* while(...) */
  
    /* At this point, the transport channel list -
     *  - may have holes due to deleted channels.
     *  - transport channel info may not be sorted by TrCH IDs.
     */ 
  
  /* Fill holes and put all Transport channels in the first
   * ordered_config_ptr->dl_trch_cnt indices.
   */

  /* Now Remove holes in DFLOW Index */
  rrcllc_rearrange_dflow_index();
  rrcllc_rearrange_macehs_queue_index(ordered_config_ptr);
  rrcllc_fill_holes_in_dl_dch_trch_idx();
  
  /* Now is the time and place for sorting all transport channel
  * info based on TrCH IDs. Call the right function.
  */
  rrcllc_sort_dl_dch_trch_index_array(&ordered_config_ptr->dl_dch_trch_info[0], 
                                     ordered_config_ptr->mac_dl_parms.num_dchs
                                      );
  
  return(SUCCESS);
}


#endif /* FEATURE_WCDMA_REL9 */

