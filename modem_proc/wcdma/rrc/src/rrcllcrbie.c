/*===========================================================================

             L3 - RRC Lower Layer Controller Radio Bearer IEs

DESCRIPTION
  This file contains functions necessary for the RRC Lower Layer Controller
  Ordered Config Radio Bearer Information Element processing.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcllcrbie.c_v   1.33   13 Jun 2002 16:05:00   vnagpal  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcllcrbie.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/20/16   sp      Made changes to reduce F3s
05/09/16   nr      Made changes to remove irrelevant F3s
06/12/15   sp      Made changes to validate the radio bearer identity used in the array index
02/05/15   aa      Made changes to add the default switch case to fix errors
01/08/14   sg      Made changes to mainline early implementable rel 10 spec CRs
11/05/14   aa      Added Message Highs for validation failures
10/08/14   sp    Made changes to update F3 message related to downlink re-establishment
10/01/14   sp    Made changes not to configure edch mac-d flow ID 7 for dedicated SRB hsrach mappings
08/04/14   sp      Made changes to fix compilation errors
07/25/14   sa      Made changes to check array bounday check for updating SRB mapping info
07/18/14   ad      Made changes to reduce F3s
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
05/23/14   db      Made changes to not reject OTA when PDU size is >126 and special_value_he_config is set
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/22/14   sp      Made changes to put rel-10 spec cr 4375 under release-9 NV as this is applicable from rel-9 onwards
03/29/14   sg     Made changes to update proper feature flag for rel 10 spec CRs
04/10/14   db      Made changes to detect UL size change during FACH to FACH redirection
04/10/14   sp      Fixed KW error. Added index check for acessin am_config array
04/10/14   sg     Made changes to correct the feature flag used for rel 10 spec CRs
04/01/14   sp      Fixed KW errors
03/13/14   db      Made changes to skip uplink size detection when UE transitions out of DCH state
02/13/14   ad      Made changes to remove redundant F3s
01/22/14   ad      Made changes to reestablish DL and UL both even if one-sided reestablishment
                   flag is FALSE
01/15/14   sp      Fixed compiler warnings
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/19/13   db      Made changes to use the correct parameter to fill lc-id and queue association
11/25/13   db      Made changes to detect fixed to fixed size change in EDCH size
11/06/13   sg      Made changes to reject the config if for a AM RLC enity, network configures flexible size 
                   with LI 7 where 
                    1. largest PDU size is >126 octets without HE field or 
                    2. both the minimum and maximum UL RLC PDU size is >126 octets
11/01/13   sp      Made changes to implement Rel-10 Spec CR 4375
10/07/13   db      Made changes to configure downlink properly upon PCH to FACH thorugh MR
09/29/13   db       Made changes to check re-establishment within HSRACH and out of HSRACH
07/23/13   db      Made changes to ignore flow-ID check for SRB's and user RB when cell update is triggered
06/13/13   db       Made changes to ignore mapping check for released RB's
05/22/13   sa      Made the changes for spec CR4633
05/15/13   vg       Removed code under compiler flag FEATURE_WCDMA_RLC_DL_SIZE_CHECK.
05/15/13   mp      Fixing KW errors
05/08/13   db      Multiple mapping support for EFACH
04/26/13   ad       Made changes to detect size change from DCH to EDCH transport channels
03/19/13   sa      backout the CR403803
02/04/13   ad      Made changes to correct the debug prints in 
                   rrcllc_check_if_rbid_present_in_chan_config function
01/10/13  sa        Made changes to set the mac_eul_action to reconfig on deleting the dedicated 
                    logical channel from R8 to R3 msg.
01/03/13   ad      Made changes to reduce the redundant F3s
10/26/12   db      EFACH Redesign changes
09/18/12   gv       Fixed the compilation warnings
10/04/12   md       Made changes to detect RLC size change during state transitions via CELL PCH state.
10/02/12   sa       Fixed compilation warnings
09/26/12   mp       Made changes to update log_ch mode form hanging rb database
08/29/12   ad       Made changes to detect rlc-reestablishment when
                    mapping changes from one mac-d flow to another mac-d flow
08/17/12   sa       Made the changes to check ul edch flexible or fixed size only for edch is active
08/14/12   as       Made changes to restore rlc size list for rach
07/23/12   sa       Made the changes to update the oc on transition to PCH from FACH
13/07/12   ad       Made changes to handle one-sided RLC restablishment flag correctly
                    in flexbible to fixed change
07/24/12   geg      Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
11/07/12   ad       Made changes to handle ERNTI correctly. Changes to avoid starting MAC
                    for a HSRACH to HSRACH SIB5 change.Increased the handing rb size.
04/06/12   ad       Changes for HS-RACH feature
05/31/12   md       Merged MAC_I fixes.
05/10/12   md       Added code changes to support FEATURE_MAC_I
04/20/12   db       Fixed Compiler Warning
05/04/12   ad       Made changes to reject if either of UL or DL RLC mode info is not present for AM mode.
03/20/12   mn       Corrected bad format log prints
03/12/12   db       Mainlined FEATURE_RRC_UM_RLC_INFO, OTA should be rejected if it tries to modify 
                    existing RB RLC mode
01/18/12   md       Made changes to reject rb setup if either of UL or DL RLC mode info is not present.
12/30/11   ad       Fixed compilation warnings
12/26/11   ad       Added changes to handle UL RLC size change from DCH to EDCH
12/08/11   db       Made changes to delete BCCH/PCCH mac-ehs mapping while moving to DCH
11/03/11   rl       Fixed the osys coding issue
09/08/11   as       Added E-DRX changes for R8
08/16/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.104.00
08/05/11   as       Made changes to backup fach and rach mapping for EFACH
07/19/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/13/11   as       Made changes to adjust mac-LogicalChannelPriorities if
                    SRB2 prioirity is lower than than SRB3
07/19/11   rl       Added condition to check the bitmask before accessing the IE pointer
07/14/11   rl       Added the checks for the optional IE's and also correcting the indexing issue 
                    with dl_LogicalChannelMappingList.
07/08/11   gv       Fixed KW errors
07/05/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.101.00
07/29/11   as       Made changes to reset rlc size list properly during unmap rb
06/21/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/20/11   as       Made changes to backup E-FACH mapping when UE camped on to Legacy cell
06/09/11   ad       Added changes to ignore the duplicate RB IDs in chan_config_req
                    when releasing RBs in RBR
06/08/11   as       Made changes to backup mac-ehs mapping only if choice is mac-ehs
06/08/11   as       Made changes to reset no of ul ndlchan in rrcllc_restore_rach_rb_mapping
                    only if restoring all rach mapping is TRUE
06/14/11   sks      Fixed a bug to use correct bit-mask for rlc_InfoChoice.
05/24/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.98.50
05/18/11   ad       Added changes to include UM RBs also in channel config request 
                    from established rabs
05/03/11   rl       Fixed the lint error
04/15/11   su       Made changes to correct the if condition check in order to
                    avoid NULL pointer access.
04/12/11   ad       Made changes to correct the condition check for loop index
04/08/11   as       Made changes to use correct pointer while accessing rlc-info
                    when rlc-info-choice is same_as_rb
03/29/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/16/11   ad       Removed the changes added in efach to check for 
                    duplication of rbid/logical channel id in database   
03/28/11   rl       Merged the HS-FACH/PCH changes from mainline
03/04/11   sk       Fixed compilation warning.   
03/01/11   ad       Storing the RB Mapping info fin EFACH code
                    when N/W uses pre Rel 7 OTA's
03/02/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/24/11   ad       Fixed compilation warnings
02/18/11   ad       Added changes for one sided UL RLC reestablishment 
02/15/11   kp       Modified rrcllc_restore_rach_rb_mapping to restore SRB1 only info. 
02/15/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/14/11   as       Made changes to use memmove instead of memcpy when source
                    and destination buffers overlap
02/07/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad       Added changes for RRC SW decoupling
01/05/10   as       Made changes to update rb mapping for EFACH
01/12/11   rl       Migrated to VU_MODEM_WCDMA_RRC.01.88.50
12/30/10   prk      In e-fach mode PCCH and BCCH RBs will have same LC ID, to
                    differentiate made changes to pass rb-id as argument to function.
01/04/11   rl       Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/17/10   prk      Made changes to indicate RLC if network configures DL PDU 
                    sizes for AM.
12/10/10   prk      Made changes to restore mappping from ordered hanging RB.
12/31/10   rl       Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/01/10   prk      Featurization of code, this avoids rejecting dch-ehs mapping
                    if EHS feature is enabled.
11/23/10   kp       Set mac_id in l1 db while restoring mac-ehs q info
11/22/10   prk      Modified function definition such that it restore a given RB,
                    from Hanging rb to OC.
11/29/10   rl       Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/02/10   prk      Handle multiple RACH mappping on non-efach cell.
11/09/10   rl       Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   prk      Copy FACH mapping on all configured FACHs.
10/21/10   prk      Copy RB mapping from OTA into OC if next state is PCH.
10/21/10   prk      Copy FACH mapping on all configured FACHs.
10/20/10   su       Fixed Lint warnings.
10/12/10   su       Fixed klockwork warnings
10/04/10   prk      Fixed compilation warning.
10/04/10   prk      Merging latest e-fach fixes.
10/01/10   su       Fixed compiler warnings
09/28/10   kp       Made changes to avoid buffer over flow problem in 
                    get_pdcp_info_later_than_r3 & get_pdcp_info.
09/27/10   rm       Replaced the usage of FEATURE_SRB5 with FEATURE SRB5 SUPPORT
09/20/10   prk/kp   Added code to support FEATURE_WCDMA_HS_FACH
09/16/10   rm       Added code to validate the SRB IDs received from the OTA and
                    fail the OTA on the validation failure
11/03/10   rl       Fixed compiler warnings
10/30/10   rl       Objective systems migration changes check in 
                    on code base VU_MODEM_WCDMA_RRC.01.80.00
08/25/10   prk      Featurizing the changes made to set SRB4 priority. 
07/20/10   prk      Made changes to set SRB4 priority to a lower priority if
                    both SRB3 and SRB4 have equal priority.
05/07/10   prk      Fixed compiler warnings.
03/10/10   gkg      Featurize the Opt Timer changes under feature FEATURE_CS_VOICE_OVER_HSPA.
02/11/10   gkg      Made changes for Opt Timer for CSoHS.
12/21/09   gkg      Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
11/17/09   rmsd     Fixed lint error.
11/04/09   kp       While adding/deleting MAC-ehs logical channels, made changes so 
                    that MAC-ehs info is reset properly in rrcllc_rearrange_macehs_lc_index()
10/30/09   as       Fixed klockwork warnings
10/23/09   dm       Fixed Klockwork warnings
10/05/09   ps       Fixed Klockwork warnings
10/07/09   gkg      Changes for REL8 under FEATURE_WCDMA_REL8
05/06/09   gkg      Made changes so that if FEATURE_WCDMA_FDPCH is defined,
                    and SRBs are mapped to HS-DSCH, RB-Mapping validation passes.
                    This change is done for R7 Signaling path only.
05/18/09   gkg      Made the changes to fix compiler warning. Basically, moved
                    the definiton for get_dl_rlc_um_info_r6() under R6 feature.
05/11/09   ps       Feature to dynamically allocate OC, CC, TOC ptr
05/08/09   ss       Updated Copyright Information
04/29/09   ss       Made changes to compiler warnings on gcc compilers.
04/08/09   gkg      In case of Cell_FACH->Cell_DCH transition through Reconfig OTA, 
                    if RB-Mapping for any RB is provided again, TOC will be not be 
                    updated for RB-Mapping.
03/30/09   ps       Removed changes to enable/disable HE field usingthe NV flag
                    NV_WCDMA_TEST_NV_1_I
03/24/09   ps       Made changes that once OTA provides RLC size, then it
                    can be changed only through OTA, only in case of R3 OTA
                    RLC size will be calculated from teh TF of the Trch
03/16/09   ps       Made changes to reject OTA if GCF is not ebale and N\W
                    sends ALT-E bit field
03/09/09   ps       cleanup of feature flag 
                    FEATURE TFC RLC SIZE
                    FEATURE_DCH_FACH_STATE_TRANSITIONS
                    FEATURE TFC RLC SIZE
                    FEATURE_DCH_IE_IN_CELL_FACH
                    FEATURE_TFCSUBSET
                    FEATURE_ADJUST_TB_SIZE_WHEN_TRCH_INFO_NOT_PRESENT
02/09/09    ps      Made changes to disbale enable HE field on UL based on
                    NV flag NV_WCDMA_TEST_NV_1_I
02/06/09    ps      Set HSDSCH bitmask if MAC-EHS mapping is provided
01/30/09    ps      Added the hack of Disabling HE field on UL again under
                    FEATURE_WCDMA REL7 DISABLE UL HE FIELD
01/21/09    ps      Removed hack of Disabling HE field on UL as, E// N/W 
                    now supports it.
12/19/08    ps      Disabling HE field on UL till N/W starts supporting it under 
                    FEATURE WCDMA REL7 HACK
11/27/08    gkg     Featurized the use of CTCH_RADIO_BEARER_ID under feature FEATURE UMTS BMC.
                    The change is in function get_lc_type_by_um_rb_id().
10/30/08    ps      Corrected the return type for get_dl_rlc_um_info_r6() in 
                    R6 OTA path
10/10/08    gkg     Added code for updating UM RBs in OC first and then making them 
                    available to LLC to configure RLC while doing Channel Config Req. 
                    Also added code for Alt E Bit that is used for UM RBs.
10/16/08    ps      Made correction to the check that the discard mode is 
                    No Discard for the support for SNRS relocation IE to be
                    mandatorily be present
09/12/08    ps      Added code for REl7 under FEATURE_WCDMA_REL7
07/25/08    ps      Made changes to loop through MAX AM entity while checking 
                    for re-establish flag in AM config
06/19/08    ps      Made changes to store a RB mapping for EDCH which has no 
                    EDCH info yet in the Standalone RB Mapping Database 
                    instead of OC
06/16/08    ps      Corrected featurisation issues for FEATURE HSDPA
06/13/08    gkg     Made changes so that if RLC PDU size changes for RBs through
                    Transport Channel IEs of OTA messages, those RBs are reconfigured
                    and therefore their RLC entity is notified for new RLC PDU size.
05/28/08    ps      Fixed compilation erro, need to pass if Unicast Call or a Multi-Cast Call
                    to pdcp_validate_params()
05/26/08    ps      Made changes to incate to PDCP, if an RB for a
                    Unicast Call or a Multi-Cast Call
04/18/08    gkg     Fixed Klockwork errors.
04/17/08    ps      Made changes to move get_srb_info_to_setup_preconfig_r5() out 
                    of feature FEATURE_DEFAULT_CONFIG_IN_RB_SETUP
03/06/08    da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
21/02/08    gkg     Fixed array index out-of-bound warnings reported by Klocwork.
01/18/07    ps      Made modification to update RLC LC ID for RACH whenever the 
                    RB Mapping for an RB is modified
12/27/07    rm      Modified ERR print when deleting the RLC parameters in config db
12/21/07    da      Support for FEATURE_MBMS
11/08/07    da      Fixed R6 RRC connection setup issue when FEATURE SRB5 SUPPORT
                    is not defined.
10/30/07    ps      Added support for DC 14. Also added support to extract 
                    SRB5 contents received in Rel6 Handover to utran message.
09/17/07    da      Made change to return failure when unable to get empty RLC 
                    AM Info slot.  The CFG complete will be sent to NW with failure.
09/13/07    vk      In fn get_dl_logical_channel_info_r5(), if the associated
                    transport channel with a radio bearer as specified by the
                    network in DL Reconmfoiguration Msg is HS-DSCH and F-DPCH
                    has been configured, then allow the configuration to proceed.
                    Changes have been featurized under FEATURE_WCDMA_FDPCH
09/10/07    da      When getting new EDCH mapping info from NW, always get the LC 
                    info from LCM if it is present for that rb_id.
06/11/07    vm      Fixed a warning
06/08/07    da      Change to initialize the rohc profiles to uncompressed as 
                    uncompressed should always be supported by the UE.  Removal
                    of IE processing that have become dummy in the latest ASN.1
05/23/07    ps      Added support for "RB Info to Change" IE in RB-Setup msg.
                    Change are made under feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP.
04/20/07    da      Made changes needed to update ASN.1 version to R6 sept R6 2006
04/10/07    ps      Added support for default configs sent through Rb-Setup msg
03/29/07    da      Fixed bug when handling IE SRBinfotosetup in R6 path and the 
                    rlc_infochoice is set to same_as_RB_Chosen. 
03/08/07    da      7200 compiler warning fix.
02/26/07    da      Added function rrcllc_validate_rb_mapping_info().
02/21/07    ps      Made changes to return a failure/succes status for all the functions
                    that restore the Standalone Rb-mapping info into the ordered config
02/07/07    ps      Made changes to handle Standalone Rb-Mappings
12/19/06    sgk     Added check to verify if the rb id was same before rejecting
                    the mapping when the logical channel identity is not present
                    and the mapping is repeated in the the same message. 
11/20/06    da      Made change to always restore oc_ptr before returning
                    when oc is pointing to toc.
11/20/06    da      Added check to reject configuration if IPHC is on top of
                    RLC config with out of sequence delivery.  Out of sequence
                    delivery is not supported by the compression engine.
10/24/06    da      Added a check to reject configuration if MAX_CID is greater
                    than max number of ROHC context sessions advertized in PDCP
                    capability.
10/03/06    sm      Made changes to store lgChIdentity rcvd from NW in E-DCH MAC D 
                    flow information to be sent to MAC
09/11/06    vk      Modified fn get_dl_am_rlc_info_r5() to include RLC Size Change
                    parameters. Also added code to determine 1) if new size
                    specified by network in DL Msg is different from the old
                    size 2) Process Re-establishment indication in the Reconfig
                    Message. Changes have been featurized under FEATURE_RLC_ONE_
                    SIDE_RE_ESTABLISHMENT
08/29/06   sm/da    Made changes required for R6 ASN.1 migration.  Also merged
                    FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
06/13/06    da      Added check to reject msg if UL and DL logical chan mode 
                    aren't matching for a given RB.  For example - UL mode AM and DL
                    mode UM for the same RB is not a supported configuration.
06/05/06   sgk      Made changes to include rel5 non HSDPA functionality under 
                    FEATURE REL5 while retaining the HSDPA specifc processing 
                    under FEATURE HSDPA.
05/08/06    vr      Fixed lint warnings
04/20/06    da      Added check to reject msg if RB mapping info maps multiple 
                    logical channels to the same trch without providing logical 
                    channel identity for each.  Also reject if the same logical
                    channel id is used for more than 1 logical channel mapped 
                    to a given trans channel.
04/18/06    da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                    R6 Hacks
03/15/06    tkk     Lint warnings/errors cleanup
03/10/06    da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
02/28/06  da/sm     Modified code to support migration to Dec-05 Rel-6 ASN1
01/11/06    sgk     Corrected checks for srns relocation to support rel 99
                    and rel5 asn1 
01/10/06    sgk     Added additional checks to verify if any of the internal 
                    ies were present if pdcp info is included, before returning
                    unsupported configuration to the network.  
01/04/05    kc      Fixed compilation errors for constant defines being different between
                    rel-99 and rel-5 ASN1 versions
12/22/05    da      Return failure if PDCP info is present in rb_info_to_setup and 
                    rb_info_to_reconfig.
11/10/05    da      Checking global flag rrc_nv_rel_indicator before doing R5 related
                    processing.
08/17/05    da      Added support for timer based explicit discard.  change is under
                    feature FEATURE TIMER BASED SDU DISCARD.
07/21/05    vk      Changed some MSG_HIGH to MSG_MED as during HSDPA testing, lot
                    of F3 messages are getting dropped. This is part of the
                    overall effort to reduce un-necessary F3 messages
07/15/05    vk      In fn get_dl_logical_channel_info_r5(), modified the check
                    from < to <= so that if network maps SRB4 on HS-DSCH, 
                    then configuration is rejected
06/23/05    vk      In case rrc_noDiscard_chosen is chosen and maxdat is set to 1,
                    send failure message as this is an invalid configuration
05/26/05    vk      In case a RB is mapped onto a DFLOW which does not exist, then
                    create a new DFLOW and map the RB on that
04/22/05    vk      If UL RLC Info is specified for a RB which is
                    already existing, ignore the RLC Info
04/15/05    sk      Modified Functin unmap_rb() under HSDPA definition.
04/13/05    sk      Functin unmap_rb() is taking one more parameter that indicated 
                    which config_db (oc or toc or both) to unmap RBs from.
03/11/05    vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
03/02/05    ttl     Support RRC ASN1 Release 5.
11/12/04    ttl     Fixed the bug in increasing the number of nchan of an AM entity.
11/09/04    ttl     Fixed lint error.
10/29/04    ttl     Fix the RLC AM config memory pointing bug for M-PDP by leaving
                    the hole w/o replacing w/ other existing RLC entities when release
                    an RLC entity.
10/12/04    vn      While releasing DL TM channels, ensure they are released 
                    from Current Config if BCCH is the only channel being released.
08/30/04    sgk     Added the function get_rx_window_size to fix lint error 
                    Type mismatch (arg. no. 1) (enum/enum) in function 
                    get_dl_am_rlc_info 
                    Initialized rlc_lc_id to 0 in function 
                    get_fach_mapping_info and get_rach_mapping_info to fix 
                    lint warning 'may not have been initialized'      
                    Type cast timerPollProhibit and timer_poll in function
                    get_polling_info and timerStatusProhibit in function
                    get_rlc_dl_status_info to uint16 to fix lint warnings
                    Converting enum to int and Arithmetic operation uses
                    (compatible) enum's.   
08/26/04    ttl     Changed messages for TB size adjustment.
08/24/04    jh      Fixed lint warning in unmap_rb().
07/19/04    ttl     Moved the TB size adjusting to llcmsgie.c.
07/09/04    jh      Added additional logging for channel counts.
07/09/04    jh      Removed obsolete TimerEPC IE processing.
06/25/04    ttl     Set lg_ch_cnt_up to TRUE when the number of mapping LC
                    increases from 1 to 2 for a TrCH. 
                    Reset lg_ch_cnt_down after adjusted the TB size.
05/11/04    jh      Fixed get_rlc_status_info() to disable RLC features when
                    corresponding optional IEs are absent.
04/28/04    jh      Changed get_srb_info_to_setup() to copy RLC Info from
                    existing configuration if same_as_RB is specified but
                    the RB is not included in the message.
04/13/04    jh      Extended unmap_rb() to also remove RB mappings from TOC
                    if it is in a valid state.
04/02/04    jh      Fixed get_fach/rach_mapping_info() functions to update
                    rlc_id and dlchan_info when the next state is a PCH state.
03/12/04    jh      Added function remove_rlc_size_list_entry() for use in
                    unmap_rb() when an existing radio bearer is unmapped.
02/26/04    jh      Added support for storing the logical channel ID for new
                    UM and TM RLC channels in addition to incrementing the
02/06/04    sk      Fixed decrement from zero of number of channels in ordered
                    config RLC TM and UM info.
02/03/04    ttl     Fixed the problem of not update the tb_size when its number
                    of mapping logical channels reduce to 1 from multiple.
                    Update lg_ch_cnt when ndlchan changed.
01/19/04    ttl     Fixed TimerStatusProhibit calculating error.
10/08/03    jh      Disabled processing of unsupported RLC Info IEs.
10/01/03    jh      Added check to get_rb_info_to_reconfig() to treat empty
                    RLC Info the same as absent RLC Info instead of rejecting
                    it.
04/29/03    vn      Additional parameter passed to find_ul_dch_trch_idx
04/10/03    jh      Added support for temporarily pointing ordered_config_ptr
                    at transition_config.toc in order to process and store DCH
                    IEs in Cell_FACH state.
03/27/03    vn      Wrapped all RLC size for DCH processing with
                    FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH.
02/05/03    sk      March2002Rel specific changes 
                    Changed rrc_UL_TransportChannelType_rach_chosen to
                    rrc_rach_chosen
10/30/02    vn      Do not process UL UM Mode RLC Info if "Timer Discard" is 
                    not present. Do not process DL UM Mode Info if a new RLC
                    entity is not to be added.
10/07/02    vn      Code cleanup. Added function headers, removed redundant code.
10/04/02    jh      Added support for RLC size restrictions: Added new functions
                    to extract RLC Size List information from its IE.  Added
                    calls to process this IE in get_ul_dch_mapping_info() and
                    get_rach_mapping_info().
09/24/02    vn      Support for DCH->FACH transitions: Update logical channel mode
                    and ID while processing RB Mapping Info for UL/DL DCH, FACH
                    and RACH for RBs in "Affected List". This is so that when
                    "Affected list" is included in message that transitions from
                    DCH to FACH, the log chl mode and ID are available in TOC
                    after transition to CELL_FACH. Always update cell_fach_rb_
                    mapping_info in OC while processing get_fach/rach_mapping_
                    info. This is to make CEll FACH mappings up-to-date in both 
                    CELL_FACH and CELL_DCH states. unmap_rb also unmaps RB Mapping
                    info in cell_fach_rb_mapping_info struct. Added new fns:
                    rrcllc_update_dl/ul_ded_log_chl_info. Added next RRC state 
                    as a parm to get_rach/fach_mapping_info.
08/01/02    vn      Accept Timer based no Explicit setting for UL TM RLC entity.
                    Even if UE-RLC does not actually implement this discard mode, 
                    it does not affect UTRAN-UE communication.
06/13/02    vn      Added new functions to delete RLC UL and DL UM and TM Info.
06/10/02    vn      Fixed a typo in remove_dl_ded_logch_entry (changed UL to DL)
06/07/02    vn      Indicate if MAC Uplink or Downlink needs to be reconfigured
                    when processing RB Mapping Info or RB Info to Release list.
                    Changed get_rb_info_to_release to call unmap_rb() to remove
                    all RB mapping info, delete RLC AM Info (new function).
06/06/02    jh      Added ability to overwrite existing RLC data.
06/03/02    jh      Added unmap_rb() function to allow remapping of already
                    mapped radio bearers.
05/08/02    vn      Ignore Mapping info not meant for the upcoming RRC state.
04/15/02    vn      Changed so that access to Ordered Config data is only 
                    through ordered_config_ptr.
03/22/02    vn      Missing Optional parameters in Polling Info are now
                    indicated to RLC.
03/18/02    vn      In get_fach_mapping_info, map all logical channels to 
                    all available FACHes.
03/12/02    kc      Fixed incorrect timerStatusProhibit check. The check is
                    corrected to be between rrc_tsp10 and rrc_tsp_1000.
03/06/02    kc      Fix for CR20954.  Fixed incorrect timerStatusProhibit check
03/05/02    vn      Updated get_rach_mapping_info to correctly initialize MAC
                    ID to the Logical Channel ID value in RB Mapping Info IE.
03/04/02    vn      Added Cell_FACH support and changes due to OC cleanup.
                    In get_srb_info_to_setup and get_rb_info_to_setup, 
                    always use the SRB Setup Info pointer as passed in 
                    parameter to get RB Mapping Info. In get_fach_mapping_info
                    and get_rach_mapping_info, call find_dl_fach_trch_idx
                    and find_ul_rach_trch_idx to get FACH and RACH TrCH index.
                    Update for only the 1st TrCH index since an RB/SRB could
                    be mapped to all FACHs simultaneously. Changes due to 
                    moving of structures to ordered_config_type. 
12/21/01    kc      Fixed CR19919.  Typo extra ";" after if stmt 
12/18/01    vn      In get_rlc_info, while updating RLC Info for AM entity, 
                    also update the RB ID (in RLC Info) with the one passed
                    as parameter.
12/04/01    rj      Updated function get_srb_info_to_setup with correct 
                    linked list type.
12/04/01    kc      Changes pertaining to June spec conversion.  Mainly asn1
                    changes and rlcrrcif changes.
11/28/01    vn      For Uplink UM RLC Info, accept Timer based no Explicit
                    as a valid SDU Discard mode setting. In get_rlc_info
                    corrected comparisons with MAX number of supported channels.
11/16/01    vn      Merged the following from branch -
                    Provided non-sequential transport channel handling: Call
                    rrcllc_find_dl/ul_dch_trch_idx from get_dl/ul_dch_mapping
                    _info since transport channel index should have already 
                    been allocated.
07/17/01    kmp     Added get_rb_info_to_reconfig().
07/03/01    ram     Added temporary code to check downlink RLC mode as well
                    as uplink RLC mode. This is not required once the scripts
                    have been corrected.
06/30/01    kmp     Fixed a bug in get_rlc_info where the DL UM channel count
                    was not correctly updated.
06/28/01    kmp     Fixed a bug in get_polling_info(), where the 
                    poll_info_incl flag was not properly set.
06/28/01    kmp     Fixed a bug in get_rlc_mode() where logch_mode may be
                    uninitialized. Also removed checking the DL RLC mode
                    since all RABs and SRBs have same UL and DL RLC modes.
06/18/01    kmp     Added debug info to get_rb_info_to_release().
06/13/01    kmp     Updated to remove trch_idx from local rrcllcoc TrCH
                    mapping structure. Updated to make ul_trch_cnt and 
                    dl_trch_cnt part of the ordered_config_data structure.
                    Added get_rb_info_affected().
06/07/01    kmp     Fix ARM compiler warnings and errors in 
                    get_rb_info_to_release() and get_xmit_rlc_discard().
06/06/01    kmp     Fixed a bug in get_dl_dch_mapping_info() and 
                    get_fach_mapping_info() to correctly increment the number
                    of transport channels found.
06/05/01    kmp     Updated MSG_LOW to MSG_MED debug output. Fixed bugs in
                    get_ul_dch_mapping_info and get_rach_mapping_info() to
                    update nchan for tfcs calculation.
06/05/01    kmp     Added get_rb_info_to_release(). Updated 
                    get_ul_dch_mapping_info() to also update the CCTrCH type.
                    Updated get_dl_dch_mapping_info(), get_fach_mapping_info(),
                    get_ul_dch_mapping_info(), and get_prach_mapping_info() to 
                    now accurately contain the number of dedicate logical channels
                    in the array.
05/29/01    kmp     Updated get_dl_dch_mapping_info(), get_fach_mapping_info(),
                    get_ul_dch_mapping_info(), and get_prach_mapping_info()
                    to correctly determine if there are assigned TrCHs already
                    present in the existing database. Updated the RLC Channel
                    count info, instead of using local data. Correctly selected
                    the Logical Channel (DCCH .vs. DTCH) type. In 
                    get_rb_mapping_info() updated the debug output. In 
                    get_ul_am_rlc_info(), get_ul_um_rlc_info(), get_ul_tm_rlc_info(),
                    and get_dl_am_rlc_info() get the correct pointer value, and use
                    the new RLC nentity for array indexing. In get_rlc_info() use
                    the new RLC I/F for channel and AM entity indexing. Updated
                    debug output in get_rlc_info().
05/24/01    kmp     In get_ul_tm_rlc_info(), get_ul_tm_rlc_info(), and in
                    get_rlc_info() updated for the new RLC I/F.
05/16/01    kmp     Change return types to uint32 for get_maxmrw() & for
                    get_max_dat(). Changed return type of get_rlc_mode() to
                    a uecomdef_status_e_type. Fixed various other compiler
                    warnings.
05/10/01    kmp     Fixed some additional compiler warnings. Removed support
                    for 2 RLC Logical channels per AM entity, now return an
                    error, in RB mapping info IE.
05/09/01    kmp     Merge in the RLC information and RB Mapping info. Added
                    High level IE's SRB info to setup and RAB info to setup.
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
#include "msg.h"
#include "rlcrrcif_v.h"
#include "rrcasn1.h"
#include "rrcdata_v.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcllcoc.h"
#include "rrcscmgr.h"
#include "rrcsibdb.h"
#include "uecomdef.h"

#ifdef FEATURE_WCDMA_HS_FACH
#include "rrcenhstatecfg.h"
#endif

#ifdef FEATURE_WCDMA_HS_RACH
#include "rrchsrachcfg.h"
#endif

/*===================================================================
                        DATA DECLARATIONS
====================================================================*/
#ifdef FEATURE_UMTS_PDCP
extern boolean rrc_pdcp_enabled;
#endif /* FEATURE_UMTS_PDCP */

extern boolean nv_gcf_test_flag;

/*===================================================================
                        CONSTANTS
====================================================================*/

/*===================================================================
                        FORWARD DECLARATIONS
====================================================================*/

/*===================================================================
*                       FUNCTION PROTOTYPES
====================================================================*/
#ifdef FEATURE_CHANGE_SRB4_PRIRORITY
void rrcllc_check_and_set_srb3_srb4_priority(void);
#endif
/*===================================================================
*                            FUNCTIONS
====================================================================*/

/*-------------------------------------------------------------------
This macro returns true if passed log chan type is DCCH or DTCH.  Else
returns false.
--------------------------------------------------------------------*/
#define RRCLLC_LC_TYPE_DCCH_OR_DCTCH(log_chan_type) \
    ((log_chan_type == UE_LOGCHAN_DCCH) || \
     (log_chan_type == UE_LOGCHAN_DTCH) || \
     (log_chan_type == UE_LOGCHAN_TM_DCCH_DL)) ? TRUE : FALSE 

/*lint -e818 */

/*====================================================================
FUNCTION: rrcllc_get_ul_rlc_size_for_eul()

DESCRIPTION:
  This is called when the RLC size for an uplink dedicated RLC
  entity is required. This is needed since the RLC sap requires the
  rlc size for channel setups. The input parameter is the next
  state.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The size of the RLC channel.

SIDE EFFECTS:
  None
===================================================================*/
uint32 rrcllc_get_ul_rlc_size_for_eul
(
  ordered_config_type  *config_ptr,
  rb_id_type  rb_id
)
{
  uint32 rlc_size = 0;
  uint8 ix,iy,iz;
  boolean  rb_found = FALSE;

  mac_e_config_s_type *mac_e_config_ptr = &config_ptr->mac_e_config;
    
#ifdef FEATURE_WCDMA_HS_RACH
    if(TRUE == rrchsrach_get_ready_for_common_edch(config_ptr))
    {
      mac_e_config_ptr = &config_ptr->mac_hs_rach_config.hs_rach_config;
    }
#endif

  /* Check in EDCH MAC structure if there is a higher RLC size */
  rb_found = FALSE;
  for (ix = 0; (!rb_found) && (ix < MAX_E_DCH_MAC_D_FLOW) ; ix++) 
  {
    for (iy = 0; (!rb_found) && (iy < mac_e_config_ptr->e_mac_d_flow_info[ix].num_logch); iy++)
    {
      if (mac_e_config_ptr->e_mac_d_flow_info[ix].e_ul_logch_info[iy].rb_id == rb_id)
      { 
        rb_found = TRUE;
        for (iz = 0; iz < mac_e_config_ptr->e_mac_d_flow_info[ix].e_ul_logch_info[iy].num_rlc_size_ddi_info; iz++) 
        {
          if (rlc_size < mac_e_config_ptr->e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_size_ddi_info[iz].rlc_pdu_size_in_bits) 
          {
            rlc_size = mac_e_config_ptr->e_mac_d_flow_info[ix].e_ul_logch_info[iy].rlc_size_ddi_info[iz].rlc_pdu_size_in_bits;
          }
        }
      }
    }
  }

#ifdef FEATURE_MAC_I
  /* If mac_d flow is not configred for a particular RB return INVLID RLC SIZE*/
  if(rlc_size == 0)
  {
    rlc_size = RRCLLC_INVALID_RLC_SIZE;
  }
#endif /* FEATURE_MAC_I */	
  WRRC_MSG2_HIGH("RRCEUL:RLC Size for RB id %d is %d",rb_id,rlc_size);
  return rlc_size;
}

/*==========================================================
FUNCTION  : get_lc_type_by_rb_id()

DESCRIPTION :   
  This function returns logical channel type based on UM RB id.

DEPENDENCIES: None.

RETURN VALUE: 
  Logical Channel Type.

SIDE EFFECTS: None.
============================================================*/
uecomdef_logchan_e_type get_lc_type_by_um_rb_id
(
  rrc_RB_Identity rb_id
)
{
  if(rb_id == CCCH_RADIO_BEARER_ID)
  {
    return UE_LOGCHAN_CCCH;
  }

  else if(rb_id == CTCH_RADIO_BEARER_ID)
  {
    return UE_LOGCHAN_CTCH;
  }
  else if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    return UE_LOGCHAN_DCCH;
  }
  else
  {
    return UE_LOGCHAN_DTCH;  	
  }
}

/*==========================================================
FUNCTION  : get_rlc_size_list()

DESCRIPTION : Processes the RLC Size List IE contents and saves 
              them in the RLC size restriction database.  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: The RLC size restriction database is updated.
============================================================*/
static uecomdef_status_e_type get_rlc_size_list
(
  rrc_UL_LogicalChannelMapping *ie_ptr,
  rrcllcoc_rlc_size_list_type  *rlc_size_rec,
  rrc_RB_Identity                    rb_id
)
{
  rrc_RLC_SizeExplicitList *rlc_size_list = NULL;

  rlc_size_rec->rb_id = rb_id;
  switch(ie_ptr->rlc_SizeList.t)
  {
    case T_rrc_UL_LogicalChannelMapping_rlc_SizeList_allSizes:
      rlc_size_rec->restriction_type = RLC_SIZES_ALL;
      break;

    case T_rrc_UL_LogicalChannelMapping_rlc_SizeList_configured:
      rlc_size_rec->restriction_type = RLC_SIZES_CONFIGURED;
      break;

    case T_rrc_UL_LogicalChannelMapping_rlc_SizeList_explicitList:
      rlc_size_rec->restriction_type = RLC_SIZES_EXPLICIT_LIST;
      rlc_size_list = ie_ptr->rlc_SizeList.u.explicitList;
      if(rlc_size_list == NULL)
      {
        WRRC_MSG0_ERROR("RLC Size List empty");
        return(FAILURE);
      }
      rlc_size_rec->count = 0;
      while(rlc_size_rec->count < rlc_size_list->n)
      {
        rlc_size_rec->size_idx[rlc_size_rec->count] = (uint16)rlc_size_list->elem[rlc_size_rec->count].rlc_SizeIndex;
		rlc_size_rec->count++;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid RLC Size List CHOICE: %d", ie_ptr->rlc_SizeList.t);
      return(FAILURE);
  }

  return SUCCESS;
} /* end get_rlc_size_list() */


/*===========================================================================
FUNCTION rrcllc_validate_srb_id

DESCRIPTION
This function validates the SRBID against the RLC mode of the RBID

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS or FAILURE
===========================================================================*/
uecomdef_status_e_type rrcllc_validate_srb_id
(
  rb_id_type rb_id
)
{
  uecomdef_status_e_type status = SUCCESS;
  switch(rb_id)
  {
    case DCCH_UM_RADIO_BEARER_ID:
      /*Check the RB ID against UM*/
      if((UE_MAX_DL_UM_CHANNEL == get_dl_um_index(rb_id,ordered_config_ptr)) ||
         (UE_MAX_UL_UM_CHANNEL == get_ul_um_index(rb_id)))
      {
        status = FAILURE;
      }
      break;
    case DCCH_AM_RADIO_BEARER_ID:
    case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
    case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
      if(UE_MAX_AM_ENTITY == get_am_index(rb_id))
      {
        status = FAILURE;
      }
      break;
    default:
      /*If the RB mode is AM or UM or UL TM (TBD: Check for UL TM), then it is not SRB5 too, so return failure*/
      if((UE_MAX_DL_UM_CHANNEL != get_dl_um_index(rb_id,ordered_config_ptr)) ||
         (UE_MAX_UL_UM_CHANNEL != get_ul_um_index(rb_id)) ||
         (UE_MAX_AM_ENTITY != get_am_index(rb_id)))
      {
        status = FAILURE;
      }
      break;
  }
  if(status == FAILURE)
  {
    WRRC_MSG1_ERROR("Wrong Mode for RB ID %d, return FAILURE",rb_id);
  }
  return status;
}

/*==========================================================
FUNCTION  : rrcllc_update_dl_dch_ota()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type rrcllc_update_dl_dch_ota
(
 rrc_RB_Identity    rb_id,
 uecomdef_logch_mode_e_type       logch_mode,
 tr_ch_id_type   trch_id,
  rrc_LogicalChannelIdentity logical_channel_id
)

{
      /* transport Channel Index */
  uint8         trch_idx = RRCLLC_INVALID_TRCH_IDX;
  
  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  uint8 cnt;

      /* Get the transport channel index for this transport channel ID.
    * Note: The index should already be there.
  */
  trch_idx = rrcllc_find_dl_dch_trch_idx(trch_id);
  
  if (trch_idx == RRCLLC_INVALID_TRCH_IDX)
  {
   /* Store the RB-Mapping info */
    return(rrcllc_backup_dl_dch_from_ota( trch_id,rb_id,logch_mode,(uint8) logical_channel_id));
  }
  
  /* Get the Logical Channel Index */
  logch_idx = ordered_config_ptr->dl_dch_info[trch_idx].ndlchan;

  for (cnt = 0; cnt < ordered_config_ptr->dl_dch_info[trch_idx].ndlchan; cnt++) 
  {
    if (rb_id == ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[cnt].rb_id)
    {
      return (SUCCESS);
    }
  } 

  /* If Logical Channel ID is present, save MAC Logical Channel Identity */
  ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
      (uint8)logical_channel_id;
  

  /* Save Radio Bearer ID */
  ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].rb_id = rb_id;
  
  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
  } 
  else
  {
    ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
  }
  
  if (logch_mode == UE_MODE_MAX_NUM)
  {
  /* This is the case when RB mapping info is coming 
  * in "Affected List". At this point, we need to update the Logical Channel mode 
  * and Logical Channel ID in Cell FACH RB Mapping Info - 
  * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
    */
    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if (rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
    
    if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
    /* Also update the Logical Channel ID here */
    ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
  }
  
  /* Save the logical channel mode, if a setup */
  ordered_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].rlc_mode = logch_mode;
  
  /* Increment number of logical channels */
  ordered_config_ptr->dl_dch_info[trch_idx].ndlchan++;
  /* The logic channel mapped to this TrCH increases from 1 to 2. */
  /* Need to update the TB size to reflect this change.           */
  if (ordered_config_ptr->dl_dch_info[trch_idx].ndlchan == 2 &&
    ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt == 1)
  {
    WRRC_MSG0_HIGH("Need to adjust TB size");
    ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt_up = TRUE;
  }
  ordered_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt = 
    ordered_config_ptr->dl_dch_info[trch_idx].ndlchan;
  
  rrcllc_free_hanging_rb_mapping_info_index(rb_id,DL_DCH_RB_MAPPING);

  return SUCCESS;
}

/*==========================================================
FUNCTION  : rrcllc_process_dl_pdu_size()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type rrcllc_process_dl_pdu_size
(
   rrc_OctetModeRLC_SizeInfoType1 * rrc_dl_rlc_pdu_sz,
   uint8               entity_idx,
   boolean       allow_new_channels,
  rlc_reestablish_e_type reestablish_entity
)
{
  uint32 rlc_size = RRCLLC_INVALID_RLC_SIZE;
  uecomdef_status_e_type status_var = SUCCESS;
    
  switch (rrc_dl_rlc_pdu_sz->t)
  {
  case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
    
    /* -- Actual size = (8 * sizeType1) + 16   sizeType1 INTEGER (0..31) */
    if (rrc_dl_rlc_pdu_sz->u.sizeType1 > 31)
    {
      status_var = FAILURE;
    }
    rlc_size = (rrc_dl_rlc_pdu_sz->u.sizeType1* 8) + 16;
    break;
    
  case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
  /* -- Actual size = (32 * part1) + 272 + (part2 * 8) part1 INTEGER (0..23),
    part2 INTEGER (1..3) */
    if (rrc_dl_rlc_pdu_sz->u.sizeType2->part1 > 23)
    {
      status_var = FAILURE;
    }
    rlc_size = (rrc_dl_rlc_pdu_sz->u.sizeType2->part1 * 32) + 272;
    if (rrc_dl_rlc_pdu_sz->u.sizeType2->m.part2Present)
    {
      if (!((rrc_dl_rlc_pdu_sz->u.sizeType2->part2 > 0) && (rrc_dl_rlc_pdu_sz->u.sizeType2->part2 <4)))
      {
        status_var = FAILURE;
      }
      rlc_size += (rrc_dl_rlc_pdu_sz->u.sizeType2->part2 * 8);
    }
    break;
  case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
  /* -- Actual size = (64 * part1) + 1040 + (part2 * 8) part1 INTEGER (0..61),
    part2 INTEGER (1..7) */
    if (rrc_dl_rlc_pdu_sz->u.sizeType3->part1 > 61)
    {
      status_var = FAILURE;
    }
    
    rlc_size = (rrc_dl_rlc_pdu_sz->u.sizeType3->part1 * 64) + 1040;
    if (rrc_dl_rlc_pdu_sz->u.sizeType3->m.part2Present)
    {
      
      if (!((rrc_dl_rlc_pdu_sz->u.sizeType3->part2 > 0) && (rrc_dl_rlc_pdu_sz->u.sizeType3->part2 <8)))
      {
        status_var = FAILURE;
      }
      rlc_size += (rrc_dl_rlc_pdu_sz->u.sizeType3->part2 * 8);
    }
    break;
  default:
    status_var = FAILURE;
  }
  
  if(status_var == FAILURE)
  {
    WRRC_MSG1_ERROR("Invalid RLC DL PDU Size params given in %d, return FAILURE",rrc_dl_rlc_pdu_sz->t);
    return FAILURE;
  }
  
  WRRC_MSG2_MED("rlcsizeindicated: RLC Size %d, NewCh %d", rlc_size,allow_new_channels);
  ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size = rlc_size;
  ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile = FALSE; 

  if (allow_new_channels == FALSE)
  {
 
    if(current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile == TRUE ) 
    {
      ordered_config_ptr->am_config[entity_idx].reestab_entity = reestablish_entity;
      WRRC_MSG2_MED("MACEHS: Re-establish dl am enity %d PDU type changed from  flexible to fixed for rb-id %d ",
      entity_idx,ordered_config_ptr->am_config[entity_idx].common_config.rb_id);
    }
    else
    if (rrcllc_compare_rlc_size_change(ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size,
      current_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size) == TRUE)
    {
      WRRC_MSG3_MED("rlcsizeindicated: OldSize %d,NewSize %d,Reestab[0:B/1:DL] %d ", current_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size,
        ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size,reestablish_entity);
      
      ordered_config_ptr->am_config[entity_idx].reestab_entity = reestablish_entity;
    }
    else
    {
      WRRC_MSG2_MED("rlcsizeindicated: Old Size %d, New Size %d", current_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size,
        ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size);
      ordered_config_ptr->am_config[entity_idx].reestab_entity = RLC_RE_ESTABLISH_NONE;
    }
  }
  
  return SUCCESS;
  
}

/*==========================================================
FUNCTION  : get_dl_dch_mapping_info()

DESCRIPTION : Processes DL DCH Mapping IE contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_dch_mapping_info
(
  rrc_DL_LogicalChannelMapping *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{


  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  /* Get the DL DCH TrCH ID if it exists */
  switch(ie_ptr->dl_TransportChannelType.t)
  {
    case T_rrc_DL_TransportChannelType_dch:
      trch_id = (tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch;
      break;

    case T_rrc_DL_TransportChannelType_fach:
    case T_rrc_DL_TransportChannelType_dsch:
    default:
      WRRC_MSG0_ERROR("Expecting DL DCH TrCH ID");
      return FAILURE;
  } /* end DL DCH TrCH ID switch */


  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,& rrc_DL_LogicalChannelMapping,logicalChannelIdentity))
  {
    return rrcllc_update_dl_dch_ota(rb_id,logch_mode,trch_id, ie_ptr->logicalChannelIdentity);
  }
  else
  {
    return rrcllc_update_dl_dch_ota(rb_id,logch_mode,trch_id, RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT);
  }
} /* end get_dl_dch_mapping_info() */

/*==========================================================
FUNCTION  : rrcllc_update_dl_ded_log_chl_info_r5()

DESCRIPTION : Processes DL Logical channel info and fills it
              in passed pointer to MAC I/F struct.

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
static void  rrcllc_update_dl_ded_log_chl_info_r5
(
rrc_DL_LogicalChannelMapping_r5 *ie_ptr, 
mac_dl_ded_logch_config_type  *dlchan_info,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode
)
{
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_LogicalChannelMapping_r5,logicalChannelIdentity))
  {
    dlchan_info->mac_id =
      (uint8)ie_ptr->logicalChannelIdentity;
  }
  else
  {
    dlchan_info->mac_id =
      RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
  }
  /* Save Radio Bearer ID */
  dlchan_info->rb_id = rb_id;
  
  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    dlchan_info->chan_type = UE_LOGCHAN_DCCH;
  } 
  else
  {
    dlchan_info->chan_type = UE_LOGCHAN_DTCH;
  }
  
  /* Save the logical channel mode, if a setup */
  if (logch_mode != UE_MODE_MAX_NUM)
  {
    dlchan_info->rlc_mode = logch_mode;
  }
}  /* rrcllc_update_dl_ded_log_chl_info_r5 */


/*==========================================================
FUNCTION  : rrcllc_update_dl_ded_log_chl_info()

DESCRIPTION : Processes DL Logical channel info and fills it
              in passed pointer to MAC I/F struct.

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
static void  rrcllc_update_dl_ded_log_chl_info
 (
  rrc_LogicalChannelIdentity logicalChannelIdentity,
  mac_dl_ded_logch_config_type  *dlchan_info,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode

 )
{

  dlchan_info->mac_id =
      (uint8)logicalChannelIdentity;
  /* Save Radio Bearer ID */
  dlchan_info->rb_id = rb_id;

  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    dlchan_info->chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    dlchan_info->chan_type = UE_LOGCHAN_DTCH;
  }

  /* Save the logical channel mode, if a setup */
  if(logch_mode != UE_MODE_MAX_NUM)
  {
    dlchan_info->rlc_mode = logch_mode;
  }
}  /* rrcllc_update_dl_ded_log_chl_info */


/*==========================================================
FUNCTION  : get_fach_mapping_info()

DESCRIPTION : Processes FACH mapping IE contents and saves 
              it in MAC Ordered Configuration Database.
              If the next state is CELL_DCH, the data is 
              stored in local data structure in OC.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_fach_mapping_info
(
  rrc_LogicalChannelIdentity logical_chan_id,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode,
  rrc_state_e_type             next_rrc_state
)
{
  /* transport Channel Index */
  uint8   trch_idx  = RRCLLC_INVALID_TRCH_IDX;

  /* Get the logical channel index */
  uint8   logch_idx, i;
 
  /* Temporary storage for TrCH ID */
  tr_ch_id_type  tmp_trch_id;

  mac_dl_ded_logch_config_type  *dlchan_info;
  rlc_lc_id_type    rlc_lc_id = (uint8)RRCLCM_RLC_LC_ID_NOT_FOUND;

  /* TBD: Although all existing FACHs would be mapped to each of the RBs,
   * fill up only for the 1st FACH - till MAC interface is clarified/updated
   */

  boolean update_fach_rlc_id = FALSE;

  uint8 cnt;

  /* Start with updating Cell FACH RB Mapping Info when the next state is
   * either CELL_DCH or CELL_FACH
   */
  logch_idx = ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan;
  dlchan_info = &ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx];

  if(logch_mode == UE_MODE_MAX_NUM)
  {
    /* This is the case when RB mapping info applicable to CELL_FACH is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
    */
    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
      return(FAILURE);
    }

    if(rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
    /* Also update the Logical Channel ID here */
    dlchan_info->rlc_id = rlc_lc_id;
    update_fach_rlc_id = TRUE;
  }

  for (cnt = 0; cnt < ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan ; cnt++) 
  {            
    if(rb_id == ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cnt].rb_id)
    {
      return (SUCCESS);
    }    
  }

/* If Logical Channel ID is present, save MAC Logical Channel Identity */ 

  rrcllc_update_dl_ded_log_chl_info(logical_chan_id,dlchan_info,rb_id,logch_mode);
  ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan ++;
  
   
  /* Now update the FACH Info ONLY IF the next state is CELL_FACH */
  if(next_rrc_state == RRC_STATE_CELL_FACH ||
     next_rrc_state == RRC_STATE_CELL_PCH ||
     next_rrc_state == RRC_STATE_URA_PCH)
  {
    if((trch_idx = rrcllc_find_dl_fach_trch_idx()) >= UE_MAX_TRCH)
    {
      return FAILURE;
    }
    logch_idx = ordered_config_ptr->fach_info[trch_idx].ndlchan;
    dlchan_info = &ordered_config_ptr->fach_info[trch_idx].dlchan_info[logch_idx];
    if(update_fach_rlc_id)
    {
      dlchan_info->rlc_id = rlc_lc_id;
    }

    /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 

    rrcllc_update_dl_ded_log_chl_info(logical_chan_id,dlchan_info,rb_id,logch_mode);

    ordered_config_ptr->fach_info[trch_idx].ndlchan++;
    ordered_config_ptr->dl_fach_trch_info[trch_idx].lg_ch_cnt = 
      ordered_config_ptr->fach_info[trch_idx].ndlchan;
      
    /* Since each available FACH is mapped to all RBs, copy the FACH info
    *  from the 1st FACH to all FACHs. Copy everything except for the
    * Transport Channel ID.
    */
    for(i = 1; i < ordered_config_ptr->mac_dl_parms.num_fachs; i ++)
    {
      tmp_trch_id = ordered_config_ptr->mac_dl_parms.fach_info[i]->trch_id;
      
      *ordered_config_ptr->mac_dl_parms.fach_info[i] = 
      *ordered_config_ptr->mac_dl_parms.fach_info[0];
      
      ordered_config_ptr->mac_dl_parms.fach_info[i]->trch_id = tmp_trch_id;
      
      ordered_config_ptr->dl_fach_trch_info[i].lg_ch_cnt =
      ordered_config_ptr->dl_fach_trch_info[0].lg_ch_cnt;
    }
  }  /* If next state is CELL_FACH, update the FACH Info */
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
    if ( rb_id && (rb_id <= NUM_SRB_DC_CELLFACH))
    {
      default_cfg_ota_mapping[rb_id-1] |= FACH_MAPPING_PRESENT;
    }
#endif   
  return SUCCESS;

} /* end get_fach_mapping_info() */


/*==========================================================
FUNCTION  : get_ul_dch_mapping_info()

DESCRIPTION : Process UL DCH Mapping IE's contents and saves 
              them in the MAC Ordered Configuration Database.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_dch_mapping_info
(
  rrc_UL_LogicalChannelMapping *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  /* transport Channel Index */
  uint8         trch_idx = RRCLLC_INVALID_TRCH_IDX;

  /* Transport Channel ID */
  tr_ch_id_type   trch_id = 0;

  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  uint8 cnt = 0;

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH
  /* Temporary pointer for processing the RLC Size List */
  rrcllcoc_rlc_size_list_type *rlc_size_rec = NULL;
#endif /* #ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */

  /* Get the UL DCH TrCH ID if it exists */
  switch(ie_ptr->ul_TransportChannelType.t)
  {
    case T_rrc_UL_TransportChannelType_dch:
      trch_id = (tr_ch_id_type)ie_ptr->ul_TransportChannelType.u.dch;
      break;

    case T_rrc_UL_TransportChannelType_rach:
    case T_rrc_UL_TransportChannelType_dummy:
    case T_rrc_UL_TransportChannelType_usch:
    default:
      WRRC_MSG0_ERROR("Expecting UL DCH TrCH ID");
      return FAILURE;
  } /* end UL DCH TrCH ID switch */

  /* Get the transport channel index for this transport channel ID.
   * Note: The index should already be there.
   */
  trch_idx = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr, trch_id);

  if(trch_idx >= UE_MAX_TRCH)
  {
    /* Store the RB-Mapping info */
    return(rrcllc_backup_ul_dch_from_ota(ie_ptr,rb_id,logch_mode));
  }

  /* Get the Logical Channel Index */
  logch_idx = ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;

  /* If network specifies same UL DCH for the RB, do not store it again */
  for (cnt = 0; cnt < ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan; cnt++)
  {
    if (ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnt].rb_id == rb_id)
    {
      WRRC_MSG2_HIGH("RRC:Ignore UL DCH Info for rb %d UL RLC Mapping for UL DCH %d not overwritten",rb_id, trch_id);
      return SUCCESS;
    }
    
  }

  /* If Logical Channel ID is present, save MAC Logical Channel Identity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_LogicalChannelMapping,logicalChannelIdentity))
  {
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
      (uint8)ie_ptr->logicalChannelIdentity;
  }
  else
  {
     ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
       RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
  }
#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

  /* Save the RLC Size List information */
  ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].mac_update_needed = TRUE;
  rlc_size_rec = &(ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].rlc_size_list[logch_idx]);
  if(get_rlc_size_list(ie_ptr, rlc_size_rec, rb_id) == FAILURE)
  {
    return FAILURE;
  }
#else
  /* Initialize the MAC data structures to allow all RLC sizes, just in case. */
  ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_size_info.all_tfi_valid = TRUE;
#endif /* FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */




  /* MAC Logical Channel Priority */
  ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].priority =
    (uint8) ie_ptr->mac_LogicalChannelPriority;

  /* Save Radio Bearer ID */
  ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rb_id = rb_id;

  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
  }

  if(logch_mode == UE_MODE_MAX_NUM)
  {
    /* This is the case when RB mapping info applicable to CELL_FACH is coming 
     * in "Affected List". At this point, we need to update the Logical Channel mode 
     * and Logical Channel ID in Cell FACH RB Mapping Info - 
     * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
     */
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
  
    if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
    }

    /* Also update the Logical Channel ID here */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
  }
  
  /* Save the logical channel mode, if a setup */
  ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_mode = logch_mode;

  /* other wise it is just a reconfig of RB Mapping info */

  /* Increment number of logical channels */
  ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan++;
  ordered_config_ptr->ul_dch_trch_info[trch_idx].lg_ch_cnt = ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;

  rrcllc_free_hanging_rb_mapping_info_index(rb_id,UL_DCH_RB_MAPPING);

  return SUCCESS;

} /* end get_ul_dch_mapping_info() */


/*==========================================================
FUNCTION  : rrcllc_update_ul_ded_log_chl_info()

DESCRIPTION : Processes UL Logical channel info and fills it
              in passed pointer to MAC I/F struct.

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
static void  rrcllc_update_ul_ded_log_chl_info
 (
  rrc_UL_LogicalChannelMapping *ie_ptr, 
  mac_ul_ded_logch_config_type  *dlchan_info,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
 )
{
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_LogicalChannelMapping,logicalChannelIdentity))
  {
    dlchan_info->mac_id =
      (uint8)ie_ptr->logicalChannelIdentity;
  }
  else
  {
    dlchan_info->mac_id = 
      RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
  }
  /* MAC Logical Channel Priority */
  dlchan_info->priority =
    (uint8)ie_ptr->mac_LogicalChannelPriority;

  /* Save Radio Bearer ID */
  dlchan_info->rb_id = rb_id;

  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    dlchan_info->chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    dlchan_info->chan_type = UE_LOGCHAN_DTCH;
  }

  /* Save the logical channel mode, if a setup */
  if(logch_mode != UE_MODE_MAX_NUM)
  {
    dlchan_info->rlc_mode = logch_mode;
  }
}  /* rrcllc_update_ul_ded_log_chl_info */


/*==========================================================
FUNCTION  : get_rach_mapping_info()

DESCRIPTION : Process RACH Mapping IE's contents and saves 
              them in the MAC Ordered Configuration Database.
              If next RRC state is CELL_DCH, save the RACH
              mapping contents in a local data structure in
              OC.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rach_mapping_info
(
  rrc_UL_LogicalChannelMapping *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode,
  rrc_state_e_type             next_rrc_state
)
{
  /* transport Channel Index */
  uint8   trch_idx  = RRCLLC_INVALID_TRCH_IDX;

  /* Get the logical channel index */
  uint8   logch_idx;
 
  mac_ul_ded_logch_config_type  *dlchan_info;
  rlc_lc_id_type   rlc_lc_id = (uint8)RRCLCM_RLC_LC_ID_NOT_FOUND;
  /* TBD: Although all existing FACHs would be mapped to each of the RBs,
   * fill up only for the 1st FACH - till MAC interface is clarified/updated
   */

  boolean update_rach_rlc_id = FALSE;

  uint8 cnt;

  /* Temporary pointer for processing the RLC Size List */
  rrcllcoc_rlc_size_list_type *rlc_size_rec = NULL;


  /* Start with updating Cell FACH RB Mapping Info when the next state is
   * either CELL_DCH or CELL_FACH
   */
  logch_idx = ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan;
  dlchan_info = &ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx];


  /* Save the RLC Size List information */
  ordered_config_ptr->rach_rlc_size_restriction_info.mac_update_needed = TRUE;
  rlc_size_rec =
    &(ordered_config_ptr->rach_rlc_size_restriction_info.rlc_size_list[logch_idx]);
  if(get_rlc_size_list(ie_ptr, rlc_size_rec, rb_id) == FAILURE)
  {
    return FAILURE;
  }


  if(logch_mode == UE_MODE_MAX_NUM)
  {
    /* This is the case when RB mapping info applicable to CELL_FACH is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
    */
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
    
    if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
    }
    /* Also update the Logical Channel ID here */
    dlchan_info->rlc_id = rlc_lc_id;
    update_rach_rlc_id = TRUE;
  }
  else
  {
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  != RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      /* Also update the Logical Channel ID here */
      dlchan_info->rlc_id = rlc_lc_id;
      update_rach_rlc_id = TRUE;
    }
  }
  
  for (cnt = 0; cnt < ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ; cnt++) 
  {
    if(rb_id == ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cnt].rb_id)
    {
      return (SUCCESS); 
    }
  }


  rrcllc_update_ul_ded_log_chl_info(ie_ptr, dlchan_info, rb_id, logch_mode);
  ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ++;  
   
  /* Now update the FACH Info ONLY IF the next state is CELL_FACH */
  if(next_rrc_state == RRC_STATE_CELL_FACH ||
     next_rrc_state == RRC_STATE_CELL_PCH ||
     next_rrc_state == RRC_STATE_URA_PCH)
  {
    if((trch_idx = rrcllc_find_ul_rach_trch_idx()) >= UE_MAX_TRCH)
    {
      return FAILURE;
    }
    logch_idx = ordered_config_ptr->ul_cctrch.rach_info.ndlchan;
    dlchan_info = &ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx];
    if(update_rach_rlc_id)
    {
      dlchan_info->rlc_id = rlc_lc_id;
    }
    rrcllc_update_ul_ded_log_chl_info(ie_ptr, dlchan_info, rb_id, logch_mode);

    ordered_config_ptr->ul_cctrch.rach_info.ndlchan++;
    ordered_config_ptr->ul_rach_trch_info[trch_idx].lg_ch_cnt = 
      ordered_config_ptr->ul_cctrch.rach_info.ndlchan;

  }  /* If next state is CELL_FACH, update the FACH Info */
 
  return SUCCESS;

} /* end get_rach_mapping_info() */


/*==========================================================
FUNCTION  : get_ul_single_logical_channel_info()

DESCRIPTION : Process one UL Logical Channel Mapping IE's 
              contents and saves them in the MAC Ordered 
              Configuration Database.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_single_logical_channel_info
(
  rrc_UL_LogicalChannelMapping *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  ordered_config_type *tmp_config_ptr = NULL;


  /* Transport channel type */
  switch (ie_ptr->ul_TransportChannelType.t)
  {
    case T_rrc_UL_TransportChannelType_dch:
      if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
      {
        if (get_ul_dch_mapping_info(ie_ptr, rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      }
      else
      {
          if(transition_config.toc_usage == TOC_INVALID)
          {
            transition_config.toc_usage = TOC_FOR_DCH;
          }

          tmp_config_ptr = ordered_config_ptr;
          ordered_config_ptr = transition_config.toc_ptr;

          /* Processing functions must think we are going to Cell_DCH state. */
          ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;

          if(get_ul_dch_mapping_info(ie_ptr, rb_id, logch_mode) == FAILURE)
          {
            ordered_config_ptr = tmp_config_ptr;
            tmp_config_ptr = NULL;
            return FAILURE;
          }

          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
      }
      break;

    case T_rrc_UL_TransportChannelType_rach:
      if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_FACH ||
         ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_PCH ||
         ordered_config_ptr->rrc_state_indicator == RRC_STATE_URA_PCH)
      {
        if (get_rach_mapping_info(ie_ptr, rb_id, logch_mode,
                            ordered_config_ptr->rrc_state_indicator) == FAILURE)
        {
          return FAILURE;
        }
      }
      else
      {
        if (get_rach_mapping_info(ie_ptr, rb_id, logch_mode, RRC_STATE_CELL_DCH) == FAILURE)
        {
          return FAILURE;
        }
      }
      break;

    case T_rrc_UL_TransportChannelType_dummy:
    case T_rrc_UL_TransportChannelType_usch:
    default:
      WRRC_MSG1_ERROR("Rcvd invalid/unsupported UL TrCh type %d", ie_ptr->ul_TransportChannelType.t);
      return FAILURE;
  }

  return SUCCESS;

} /* end get_ul_single_logical_channel_info() */

/*==========================================================
FUNCTION  : get_ul_logical_channel_info()

DESCRIPTION : Get UL logical Channel info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_logical_channel_info
(
  rrc_UL_LogicalChannelMappings * ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{

  /* Process IE info based on number of UL Logical Channels */
  switch (ie_ptr->t)
  {
    /* One UL Logical Channel */
    case T_rrc_UL_LogicalChannelMappings_oneLogicalChannel:
      if(get_ul_single_logical_channel_info(ie_ptr->u.oneLogicalChannel, rb_id, logch_mode)
         == FAILURE)
      {
        return FAILURE;
      }
      break;

    /* Two logical channels, currently not supported by RRC-RLC I/F */
    case T_rrc_UL_LogicalChannelMappings_twoLogicalChannels:
    default:
      WRRC_MSG1_ERROR("Recvd Two/Invalid number of UL logical channels", ie_ptr->t );

      /* TRUE indicates that first logical channel is for Data PDUs and second  
         logical channel is for control PDUs */
      /* FALSE indicates that Data and Control PDUs can be sent on either of them */

      /* Invalid number of logical channels */
        return FAILURE;
  } /* end switch */

  return SUCCESS;

} /* end get_ul_logical_channel_info() */

/*==========================================================
FUNCTION  : get_dl_logical_channel_info()

DESCRIPTION : Get DL logical Channel info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_logical_channel_info
(
  rrc_DL_LogicalChannelMappingList * ie_ptr,
  rrc_RB_Identity                       rb_id,
  uecomdef_logch_mode_e_type       logch_mode
)
{
  /*
  * Count to make sure that don't exceed number of Logical
  * channels that can be Mapped to a Transport Channel.
  */
  uint8 lc_tc_cnt = 0;

  ordered_config_type *tmp_config_ptr = NULL;


  /* Process each item of the list */
  while((ie_ptr->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH))
  {
    /* Transport channel type */
    switch (ie_ptr->elem[lc_tc_cnt].dl_TransportChannelType.t)
    {
      case T_rrc_DL_TransportChannelType_dch:
        if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
        {
          if(get_dl_dch_mapping_info(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {

          if(transition_config.toc_usage == TOC_INVALID)
          {
            transition_config.toc_usage = TOC_FOR_DCH;
          }

          tmp_config_ptr = ordered_config_ptr;
          ordered_config_ptr = transition_config.toc_ptr;

          /* Processing functions must think we are going to Cell_DCH state. */
          ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;

          if(get_dl_dch_mapping_info(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
          {
            ordered_config_ptr = tmp_config_ptr;
            tmp_config_ptr = NULL;
            return FAILURE;
          }

          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
        }
        break;

      case T_rrc_DL_TransportChannelType_fach:
        if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_FACH ||
           ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_PCH ||
           ordered_config_ptr->rrc_state_indicator == RRC_STATE_URA_PCH)
        {

         /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
           if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[lc_tc_cnt],rrc_DL_LogicalChannelMapping,logicalChannelIdentity))
           {
             if(get_fach_mapping_info(ie_ptr->elem[lc_tc_cnt].logicalChannelIdentity, rb_id, logch_mode,
                                      ordered_config_ptr->rrc_state_indicator) == FAILURE)
             {
               return FAILURE;
             }
           }
           else
           {
             if(get_fach_mapping_info(RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT, rb_id, logch_mode,
                                      ordered_config_ptr->rrc_state_indicator) == FAILURE)
             {
               return FAILURE;
             }
           }

          
        }
        else
        {
          /* FACH mapping info can come in DCH state. Accept and update
           * local mapping info structure.
           */
         /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
           if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[lc_tc_cnt],rrc_DL_LogicalChannelMapping,logicalChannelIdentity))
           {
             if(get_fach_mapping_info(ie_ptr->elem[lc_tc_cnt].logicalChannelIdentity, rb_id, logch_mode,
                                      RRC_STATE_CELL_DCH) == FAILURE)
             {
               return FAILURE;
             }
           }
           else
           {
             if(get_fach_mapping_info(RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT, rb_id, logch_mode,
                                      RRC_STATE_CELL_DCH) == FAILURE)
             {
               return FAILURE;
             }
           }
        }
        break;

      case T_rrc_DL_TransportChannelType_dsch:
      default:
        WRRC_MSG1_ERROR("Rcv'd DSCH/invalid DL TrCh type",ie_ptr->elem[lc_tc_cnt].dl_TransportChannelType.t);
        return FAILURE;
    } /* end Logical channel type switch */

    /* Get pointer to the next item in the list */
    lc_tc_cnt++;
  } /* end while ie_ptr != NULL */

  return SUCCESS;

} /* end get_dl_logical_channel_info() */

/*==========================================================
FUNCTION  : get_rb_mapping_info()

DESCRIPTION : Get Radio Bearer Mapping Info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rb_mapping_info
(
  rrc_RB_MappingInfo         * ie_ptr,
  rrc_RB_Identity                 rb_id,
  uecomdef_logch_mode_e_type logch_mode
)
{
  /*
  * Count to make sure that don't exceed the maximum number 
  * of RB Mux options
  */
  uint8 rb_mux_cnt = 0;

  /* Process the RB Mux option IE in the linked list */
  while((ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS))
  {
    /* Check for DL Channel Mapping List */
    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,dl_LogicalChannelMappingList))
    {
      MSG_MED("Get RB %d Mapping Info for DL", rb_id,0,0);
      if(get_dl_logical_channel_info(&ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList,
                                     rb_id, logch_mode) == FAILURE)
      {
        return FAILURE;
      }
      /* Here indicate that MAC Downlink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
    } /* end dl Log Ch mapping list */

    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,ul_LogicalChannelMappings))
    {
      MSG_MED("Get RB %d Mapping Info for UL", rb_id,0,0);
      if(get_ul_logical_channel_info(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,
                                     rb_id, logch_mode) == FAILURE)
      {
        return FAILURE;
      }
      /* Here indicate that MAC Uplink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
    } /* end ul Log Ch mapping list */

    /* Pointer to the next item in the IE list */
    rb_mux_cnt++;
  } /* end loop of RB Mapping info list */

#ifdef FEATURE_WCDMA_HS_FACH
  rrcllc_backup_mapping_info_into_hanging(ie_ptr,REL_99,rb_id,logch_mode);
#endif
  return SUCCESS;

} /* end get_rb_mapping_info() */

/*==========================================================
FUNCTION  : get_maxmrw

DESCRIPTION : 
  SDU Discard -- Max_MRW infomation interpretation.
  See 10.3.4.25 in 25.331.

DEPENDENCIES:
  None.

RETURN VALUE:
  The max value for the number of retransmissions of a MRW 
  command for RLC.

SIDE EFFECTS:
  None.
============================================================*/
static uint32 get_maxmrw
(
  rrc_MaxMRW maxmrw
)
{
  /* Init MaxMRW */
  uint32 rlc_maxmrw = RRCLLC_INVALID_RLC_DISCARD;

  switch (maxmrw)
  {
    case rrc_MaxMRW_mm1:
      rlc_maxmrw = 1;
      break;
    case rrc_MaxMRW_mm4:
      rlc_maxmrw = 4;
      break;
    case rrc_MaxMRW_mm6:
      rlc_maxmrw = 6;
      break;
    case rrc_MaxMRW_mm8:
      rlc_maxmrw = 8;
      break;
    case rrc_MaxMRW_mm12:
      rlc_maxmrw = 12;
      break;
    case rrc_MaxMRW_mm16:
      rlc_maxmrw = 16;
      break;
    case rrc_MaxMRW_mm24:
      rlc_maxmrw = 24;
      break;
    case rrc_MaxMRW_mm32:
      rlc_maxmrw = 32;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown Max_MRW type", maxmrw);
      break;
  } /* end MaxMRW choice */

  return (rlc_maxmrw);

} /* end get_maxmrw() */

/*==========================================================
FUNCTION  : get_timer_mrw()

DESCRIPTION : 
  SDU discard -- Timer MRW infomation interpretation.
  See 10.3.4.25 in 25.331.

DEPENDENCIES:
  None.

RETURN VALUE:
  Timer MRW in SDU discard information for RLC.

SIDE EFFECTS:
  None.
============================================================*/
static uint16 get_timer_mrw
(
  rrc_TimerMRW timer_mrw
)
{
  /* Init Timer_MRW */
  uint16 rlc_timer_mrw = (uint16)RRCLLC_INVALID_RLC_DISCARD;

  switch (timer_mrw)
  {
    case rrc_TimerMRW_te50:
      rlc_timer_mrw = 50;
      break;
    case rrc_TimerMRW_te60:
      rlc_timer_mrw = 60;
      break;
    case rrc_TimerMRW_te70:
      rlc_timer_mrw = 70;
      break;
    case rrc_TimerMRW_te80:
      rlc_timer_mrw = 80;
      break;
    case rrc_TimerMRW_te90:
      rlc_timer_mrw = 90;
      break;
    case rrc_TimerMRW_te100:
      rlc_timer_mrw = 100;
      break;
    case rrc_TimerMRW_te120:
      rlc_timer_mrw = 120;
      break;
    case rrc_TimerMRW_te140:
      rlc_timer_mrw = 140;
      break;
    case rrc_TimerMRW_te160:
      rlc_timer_mrw = 160;
      break;
    case rrc_TimerMRW_te180:
      rlc_timer_mrw = 180;
      break;
    case rrc_TimerMRW_te200:
      rlc_timer_mrw = 200;
      break;
    case rrc_TimerMRW_te300:
      rlc_timer_mrw = 300;
      break;
    case rrc_TimerMRW_te400:
      rlc_timer_mrw = 400;
      break;
    case rrc_TimerMRW_te500:
      rlc_timer_mrw = 500;
      break;
    case rrc_TimerMRW_te700:
      rlc_timer_mrw = 700;
      break;
    case rrc_TimerMRW_te900:
      rlc_timer_mrw = 900;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown Timer MRW type", timer_mrw);
      break;
  } /* end Timer_MRW choice */

  return (rlc_timer_mrw);

} /* end get_timer_mrw() */

/*==========================================================
FUNCTION  : get_max_dat()

DESCRIPTION : 
  SDU discard -- Max_DAT infomation interpretation.
  See 10.3.4.25 in 25.331.

DEPENDENCIES:
  None.

RETURN VALUE:
  Number of retransmissions of a PU before a SDU is discarded 
  for RLC.

SIDE EFFECTS:
  None.
============================================================*/
static uint32 get_max_dat
(
  rrc_MaxDAT max_dat
)
{
  /* Init Max_DAT */
  uint32 rlc_max_dat = RRCLLC_INVALID_RLC_DISCARD;

  switch (max_dat)
  {
    case rrc_MaxDAT_dat1:
      rlc_max_dat = 1;
      break;
    case rrc_MaxDAT_dat2:
      rlc_max_dat = 2;
      break;
    case rrc_MaxDAT_dat3:
      rlc_max_dat = 3;
      break;
    case rrc_MaxDAT_dat4:
      rlc_max_dat = 4;
      break;
    case rrc_MaxDAT_dat5:
      rlc_max_dat = 5;
      break;
    case rrc_MaxDAT_dat6:
      rlc_max_dat = 6;
      break;
    case rrc_MaxDAT_dat7:
      rlc_max_dat = 7;
      break;
    case rrc_MaxDAT_dat8:
      rlc_max_dat = 8;
      break;
    case rrc_MaxDAT_dat9:
      rlc_max_dat = 9;
      break;
    case rrc_MaxDAT_dat10:
      rlc_max_dat = 10;
      break;
    case rrc_MaxDAT_dat15:
      rlc_max_dat = 15;
      break;
    case rrc_MaxDAT_dat20:
      rlc_max_dat = 20;
      break;
    case rrc_MaxDAT_dat25:
      rlc_max_dat = 25;
      break;
    case rrc_MaxDAT_dat30:
      rlc_max_dat = 30;
      break;
    case rrc_MaxDAT_dat35:
      rlc_max_dat = 35;
      break;
    case rrc_MaxDAT_dat40:
      rlc_max_dat = 40;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown Max_DAT %d", max_dat);
      break;
  } /* end Max_DAT choice */

  return (rlc_max_dat);

} /* end get_max_dat() */

/*==========================================================
FUNCTION  : get_timer_discard()

DESCRIPTION : 
  SDU discard -- timer discard infomation interpretation.
  See 10.3.4.25 in 25.331.

DEPENDENCIES:
  None.

RETURN VALUE:
  Time, in MS, that should elapse before an SDU is discarded for RLC.

SIDE EFFECTS:
  None.
============================================================*/
static uint32 get_timer_discard
(
  rrc_TimerDiscard timerDiscard
)
{
  /* Init Max_DAT */
  uint32 rlc_timer_discard = RRCLLC_INVALID_RLC_DISCARD;

  switch (timerDiscard)
  {
    case rrc_TimerDiscard_td0_1:
      rlc_timer_discard = 100;
      break;
    case rrc_TimerDiscard_td0_25:
      rlc_timer_discard = 250;
      break;
    case rrc_TimerDiscard_td0_5:
      rlc_timer_discard = 500;
      break;
    case rrc_TimerDiscard_td0_75:
      rlc_timer_discard = 750;
      break;
    case rrc_TimerDiscard_td1:
      rlc_timer_discard = 1000;
      break;
    case rrc_TimerDiscard_td1_25:
      rlc_timer_discard = 1250;
      break;
    case rrc_TimerDiscard_td1_5:
      rlc_timer_discard = 1500;
      break;
    case rrc_TimerDiscard_td1_75:
      rlc_timer_discard = 1750;
      break;
    case rrc_TimerDiscard_td2:
      rlc_timer_discard = 2000;
      break;
    case rrc_TimerDiscard_td2_5:
      rlc_timer_discard = 2500;
      break;
    case rrc_TimerDiscard_td3:
      rlc_timer_discard = 3000;
      break;
    case rrc_TimerDiscard_td3_5:
      rlc_timer_discard = 3500;
      break;
    case rrc_TimerDiscard_td4:
      rlc_timer_discard = 4000;
      break;
    case rrc_TimerDiscard_td4_5:
      rlc_timer_discard = 4500;
      break;
    case rrc_TimerDiscard_td5:
      rlc_timer_discard = 5000;
      break;
    case rrc_TimerDiscard_td7_5:
      rlc_timer_discard = 7500;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown TimerDiscard %d", timerDiscard);
      break;
  } /* end TimerDiscard choice */

  return (rlc_timer_discard);

} /* end get_timer_discard() */

/*==========================================================
FUNCTION  : rrc_get_ul_um_timer_discard_for_rb_id()

DESCRIPTION : 
  This function returns UL UM SDU Discard Timer value for a given RB-Id.

DEPENDENCIES:
  None.

RETURN VALUE:
  Time, in MS, that should elapse before an SDU is discarded for RLC.

SIDE EFFECTS:
  None.
============================================================*/
uint16 rrc_get_ul_um_timer_discard_for_rb_id
(
  rrc_RB_Identity rb_id
)
{
  uint8 ul_um_idx = 0;
  ordered_config_type * config_ptr = NULL;
  
  if(config_ptr_in_use == ORDERED_CONFIG)
  {
    config_ptr = ordered_config_ptr;
  }
  else
  {
    config_ptr = current_config_ptr;
  }
  
  for(ul_um_idx = 0; ul_um_idx < config_ptr->rlc_ul_um_parms.nchan; ul_um_idx++)
  {
    if(config_ptr->rlc_ul_um_parms.chan_info[ul_um_idx].rb_id == rb_id)
    {
      return config_ptr->rlc_ul_um_parms.chan_info[ul_um_idx].timer_discard;
    }
  }
  WRRC_MSG0_ERROR("UL UM SDU Discard not found in OC");
  return 0;
}

/*==========================================================
FUNCTION  : get_ul_um_timer_discard()

DESCRIPTION : 
  SDU discard -- timer discard infomation interpretation.
  See 9.7.3.2 in 25.322 for the details of the bahaviour of this timer.
  See 10.3.4.25 in 25.331 for the details of the value (in ms)

DEPENDENCIES:
  None.

RETURN VALUE:
  Time, in MS, that should elapse before an SDU is discarded for RLC.

SIDE EFFECTS:
  None.
============================================================*/
uint16 get_ul_um_timer_discard
(
  rrc_NoExplicitDiscard noExplicitDiscard
)
{
  /* Init with zero. Basically, if we set it to zero, RLC will not start timer for UL UM SDU.*/
  uint16 rlc_timer_discard = 0;

  switch (noExplicitDiscard)
  {
    case rrc_NoExplicitDiscard_dt10:
      rlc_timer_discard = 10;
      break;
    case rrc_NoExplicitDiscard_dt20:
      rlc_timer_discard = 20;
      break;
    case rrc_NoExplicitDiscard_dt30:
      rlc_timer_discard = 30;
      break;
    case rrc_NoExplicitDiscard_dt40:
      rlc_timer_discard = 40;
      break;
    case rrc_NoExplicitDiscard_dt50:
      rlc_timer_discard = 50;
      break;
    case rrc_NoExplicitDiscard_dt60:
      rlc_timer_discard = 60;
      break;
    case rrc_NoExplicitDiscard_dt70:
      rlc_timer_discard = 70;
      break;
    case rrc_NoExplicitDiscard_dt80:
      rlc_timer_discard = 80;
      break;
    case rrc_NoExplicitDiscard_dt90:
      rlc_timer_discard = 90;
      break;
    case rrc_NoExplicitDiscard_dt100:
      rlc_timer_discard = 100;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown NoExplicitDiscard %d", noExplicitDiscard);
      break;
  } /* end rrc_NoExplicitDiscard choice */
  return (rlc_timer_discard);
} /* end get_ul_um_timer_discard() */

/*==========================================================
FUNCTION  : get_xmit_rlc_discard

DESCRIPTION : 
  Config the SDU discard informtion for RLC.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_xmit_rlc_discard
(
  rlc_ul_info_type            *rlc_ul_info,
  rrc_TransmissionRLC_Discard *ie_ptr
)
{
  /* temporary storage for various IE values */
  uint32 temp_val = RRCLLC_INVALID_RLC_DISCARD;

  /* indicated that there is information for RLC */
  rlc_ul_info->discard_info.tx_discard_incl = TRUE;

  switch (ie_ptr->t)
  {
    case T_rrc_TransmissionRLC_Discard_timerBasedExplicit:
      /* Save the Discard Mode */
      rlc_ul_info->discard_info.discard_mode = RLC_TBE;
      
      /* Get the timer discard value */
      temp_val = get_timer_discard(ie_ptr->u.timerBasedExplicit->timerDiscard);
      if(temp_val == RRCLLC_INVALID_RLC_DISCARD)
      {
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.timer_discard = (uint16)temp_val;
      }

      /* Get the Timer_MRW */
      temp_val = get_timer_mrw(ie_ptr->u.timerBasedExplicit->timerMRW);
      if(temp_val == RRCLLC_INVALID_RLC_DISCARD)
      {
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.timer_mrw = (uint16)temp_val;
      }

      /* Get the MaxMRW */
      temp_val = get_maxmrw(ie_ptr->u.timerBasedExplicit->maxMRW);
      if(temp_val == RRCLLC_INVALID_RLC_DISCARD)
      {
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.maxmrw = (uint8)temp_val;
      }

      break;
    case T_rrc_TransmissionRLC_Discard_maxDAT_Retransmissions:
      /* Save the Discard Mode */
      rlc_ul_info->discard_info.discard_mode = RLC_MAXDAT;
      
      /* Get the Max_DAT */
      temp_val = get_max_dat(ie_ptr->u.maxDAT_Retransmissions->maxDAT);
      if(temp_val == RRCLLC_INVALID_RLC_DISCARD)
      {
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.maxdat = (uint8)temp_val;
      }

      /* Get the Timer_MRW */
      temp_val = get_timer_mrw(ie_ptr->u.maxDAT_Retransmissions->timerMRW);
      if(temp_val == RRCLLC_INVALID_RLC_DISCARD)
      {
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.timer_mrw = (uint16)temp_val;
      }

      /* Get the MaxMRW */
      temp_val = get_maxmrw(ie_ptr->u.maxDAT_Retransmissions->maxMRW);
      if(temp_val == RRCLLC_INVALID_RLC_DISCARD)
      {
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.maxmrw = (uint8)temp_val;
      }
    
      break; /* End Max DAT retransmissions case */

    case T_rrc_TransmissionRLC_Discard_noDiscard:
      /* Save the Discard Mode */
      rlc_ul_info->discard_info.discard_mode = RLC_NO_DISCARD;
      
      /* Get the Max_DAT */
      temp_val = get_max_dat(ie_ptr->u.noDiscard);
      if ((temp_val == RRCLLC_INVALID_RLC_DISCARD) || (temp_val == 1))
      {
        WRRC_MSG0_ERROR("Number of maximum retransmissions is invalid or set to 1");
        return FAILURE;
      }
      else
      {
        rlc_ul_info->discard_info.maxdat = (uint8)temp_val;
      }
      break; /* End No Discard case */
    

    case T_rrc_TransmissionRLC_Discard_timerBasedNoExplicit:
    default:
      WRRC_MSG1_ERROR("No Explicit Timer Discard/ Unknown Discard mode %d, return FAILURE", ie_ptr->t);
      return FAILURE;
  }

  /* everything is okay */
  return SUCCESS;

} /* end get_xmit_rlc_discard() */
 
/*==========================================================
FUNCTION  : get_polling_info()

DESCRIPTION : 
  Config the polling information for RLC.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_polling_info
(
  rlc_ul_info_type *rlc_ul_info,
  rrc_PollingInfo  *ie_ptr
)
{
  /* indicate that polling info is included */
  rlc_ul_info->poll_info.poll_info_incl = TRUE;
              
  /*------------------------------------------------- 
    Mandatory parameters.
  ---------------------------------------------------*/
  
  /* assign the boolean value of last_tx_pu_poll  */
  rlc_ul_info->poll_info.last_tx_pdu_poll = ie_ptr->lastTransmissionPDU_Poll;
              
  /* assign the boolean value of last_retx_pu_poll  */
  rlc_ul_info->poll_info.last_retx_pdu_poll = ie_ptr->lastRetransmissionPDU_Poll;

  /*------------------------------------------------- 
    Optional parameters
  ---------------------------------------------------*/

  /*
  * Timer Poll Prohibit. If the timer poll prohibit value is between 10 and 500, 
  * the step size is 10. If the Timer poll prohibit value is between 600 and 1000
  * the step size is 50. All other values are out of range.
  */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,timerPollProhibit))
  {
    /* Get the minimum time between polls in ms, for RLC UL AM */
    if(ie_ptr->timerPollProhibit <= rrc_TimerPollProhibit_tpp550)
    {
      rlc_ul_info->poll_info.poll_prohibit = ((((uint16)ie_ptr->timerPollProhibit) + 1) * 10);
    }
    /*lint -save -e685 */
    else if((ie_ptr->timerPollProhibit >= rrc_TimerPollProhibit_tpp550) && (ie_ptr->timerPollProhibit <= rrc_TimerPollProhibit_tpp1000))
    {
      rlc_ul_info->poll_info.poll_prohibit = (600 + (((uint16)ie_ptr->timerPollProhibit - (uint16)rrc_TimerPollProhibit_tpp600)) * 50);
    }
    /*lint -restore */
    else
    {
      WRRC_MSG1_ERROR("Timer poll prohibit is out of bounds %d", ie_ptr->timerPollProhibit);
      return FAILURE;
    }
  } /* end timer poll prohibit check */
  else
  {
    /* A value of 0 indicates that Timer Poll Prohibit is absent */
    rlc_ul_info->poll_info.poll_prohibit = 0;
  }

  /*
  * Timer Poll. If the timer poll  value is between 10 and 500, the step size
  * is 10. If the Timer poll value is between 600 and 1000, the step size is 50.
  * All other values are out of range.
  */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,timerPoll))
  {
    if(ie_ptr->timerPoll <= rrc_TimerPoll_tp550)
    {
      rlc_ul_info->poll_info.timer_poll = (((uint16)ie_ptr->timerPoll + 1) * 10);
    }
    /*lint -save -e685 */
    else if((ie_ptr->timerPoll >= rrc_TimerPoll_tp550) && (ie_ptr->timerPoll <= rrc_TimerPoll_tp1000))
    {
      rlc_ul_info->poll_info.timer_poll = (uint16)(600 + ((uint16)ie_ptr->timerPoll - (uint16)rrc_TimerPoll_tp600) * 50);
    }
    /*lint -restore */
    else
    {
      WRRC_MSG1_ERROR("Timer poll is out of bound", ie_ptr->timerPoll);
      return FAILURE;
    }
  } /* end timer poll check */
  else
  {
    /* A value of 0 indicates Timer Poll is absent */
    rlc_ul_info->poll_info.timer_poll = 0;
  }

  /* Number of PU's interval between polling */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,poll_PDU))
  {
    switch (ie_ptr->poll_PDU)
    {
      case rrc_Poll_PDU_pdu1:
        rlc_ul_info->poll_info.poll_pdu = 1;
        break;
      case rrc_Poll_PDU_pdu2:
        rlc_ul_info->poll_info.poll_pdu = 2;
        break;
      case rrc_Poll_PDU_pdu4:
        rlc_ul_info->poll_info.poll_pdu = 4;
        break;
      case rrc_Poll_PDU_pdu8:
        rlc_ul_info->poll_info.poll_pdu = 8;
        break;
      case rrc_Poll_PDU_pdu16:
        rlc_ul_info->poll_info.poll_pdu = 16;
        break;
      case rrc_Poll_PDU_pdu32:
        rlc_ul_info->poll_info.poll_pdu = 32;
        break;
      case rrc_Poll_PDU_pdu64:
        rlc_ul_info->poll_info.poll_pdu = 64;
        break;
      case rrc_Poll_PDU_pdu128:
        rlc_ul_info->poll_info.poll_pdu = 128;
        break;
      default:
        WRRC_MSG1_ERROR("Unknown Pull_PU type %d", ie_ptr->poll_PDU);
        return FAILURE;
    } /* end poll pu choice */
  } /* end poll pu check */
  else
  {
    /* A value of 0 indicates Poll PDU is absent */
    rlc_ul_info->poll_info.poll_pdu = 0;
  }

  /* # of SDUs interval between polling  */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,poll_SDU)) 
  {
    switch (ie_ptr->poll_SDU)
    {
      case rrc_Poll_SDU_sdu1:
        rlc_ul_info->poll_info.poll_sdu = 1;
        break;
      case rrc_Poll_SDU_sdu4:
        rlc_ul_info->poll_info.poll_sdu = 4;
        break;
      case rrc_Poll_SDU_sdu16:
        rlc_ul_info->poll_info.poll_sdu = 16;
        break;
      case rrc_Poll_SDU_sdu64:
        rlc_ul_info->poll_info.poll_sdu = 64;
        break;
      default:
        WRRC_MSG1_ERROR("Unknown Pull_SDU type %d", ie_ptr->poll_SDU);
        return FAILURE;
    } /* end poll sdu choice */
  } /* end poll sdu check */
  else
  {
    /* A value of 0 indicates Poll SDU is absent */
    rlc_ul_info->poll_info.poll_sdu = 0;
  }

  /* % of transmission window for polling */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,pollWindow)) 
  {
    switch (ie_ptr->pollWindow)
    {
      case rrc_PollWindow_pw50:
        rlc_ul_info->poll_info.poll_window = 50;
        break;
      case rrc_PollWindow_pw60:
        rlc_ul_info->poll_info.poll_window = 60;
        break;
      case rrc_PollWindow_pw70:
        rlc_ul_info->poll_info.poll_window = 70;
        break;
      case rrc_PollWindow_pw80:
        rlc_ul_info->poll_info.poll_window = 80;
        break;
      case rrc_PollWindow_pw85:
        rlc_ul_info->poll_info.poll_window = 85;
        break;
      case rrc_PollWindow_pw90:
        rlc_ul_info->poll_info.poll_window = 90;
        break;
      case rrc_PollWindow_pw95:
        rlc_ul_info->poll_info.poll_window = 95;
        break;
      case rrc_PollWindow_pw99:
        rlc_ul_info->poll_info.poll_window = 99;
        break;
      default:
        WRRC_MSG1_ERROR("Unknown pollWindow type %d", ie_ptr->pollWindow);
        return FAILURE;
    } /* end poll window choice */
  } /* end poll window check */
  else
  {
    rlc_ul_info->poll_info.poll_window = 0;
  }

  /* Tiem in ms timer for Timer for Periodic polling */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,timerPollPeriodic)) 
  {
    switch (ie_ptr->timerPollPeriodic)
    {
      case rrc_TimerPollPeriodic_tper100:
        rlc_ul_info->poll_info.timer_poll_periodic = 100;
        break;
      case rrc_TimerPollPeriodic_tper200:
        rlc_ul_info->poll_info.timer_poll_periodic = 200;
        break;
      case rrc_TimerPollPeriodic_tper300:
        rlc_ul_info->poll_info.timer_poll_periodic = 300;
        break;
      case rrc_TimerPollPeriodic_tper400:
        rlc_ul_info->poll_info.timer_poll_periodic = 400;
        break;
      case rrc_TimerPollPeriodic_tper500:
        rlc_ul_info->poll_info.timer_poll_periodic = 500;
        break;
      case rrc_TimerPollPeriodic_tper750:
        rlc_ul_info->poll_info.timer_poll_periodic = 750;
        break;
      case rrc_TimerPollPeriodic_tper1000:
        rlc_ul_info->poll_info.timer_poll_periodic = 1000;
        break;
      case rrc_TimerPollPeriodic_tper2000:
        rlc_ul_info->poll_info.timer_poll_periodic = 2000;
        break;
      default:
        WRRC_MSG1_ERROR("Unknown Timer Poll Periodic type %d",
                   ie_ptr->timerPollPeriodic);
        return FAILURE;
    } /* end timer poll periodic choice */
  } /* end timer poll periodic check */
  else
  {
    rlc_ul_info->poll_info.timer_poll_periodic = 0;
  }

  /* If made it this far, then all things are a go */
  return SUCCESS;

} /* end get_polling_info() */

/*==========================================================
FUNCTION  : get_rx_window_size()

DESCRIPTION : 
  Return the Window Size for receiving.

DEPENDENCIES: 
  None.

RETURN VALUE:
  Window size for receiving for RLC.

SIDE EFFECTS:
  None.
============================================================*/
uint16 get_rx_window_size
(
  rrc_ReceivingWindowSize rrc_rx_window_size
)
{
  /* Initialize the window size  */
  uint16 rlc_rx_win_size = (uint16)RRCLLC_INVALID_WINDOW_SIZE;

  switch (rrc_rx_window_size)
  {
    case rrc_ReceivingWindowSize_rw1:
      rlc_rx_win_size = 1;
      break;
    case rrc_ReceivingWindowSize_rw8:
      rlc_rx_win_size = 8;
      break;
    case rrc_ReceivingWindowSize_rw16:
      rlc_rx_win_size = 16;
      break;
    case rrc_ReceivingWindowSize_rw32:
      rlc_rx_win_size = 32;
      break;
    case rrc_ReceivingWindowSize_rw64:
      rlc_rx_win_size = 64;
      break;
    case rrc_ReceivingWindowSize_rw128:
      rlc_rx_win_size = 128;
      break;
    case rrc_ReceivingWindowSize_rw256:
      rlc_rx_win_size = 256;
      break;
    case rrc_ReceivingWindowSize_rw512:
      rlc_rx_win_size = 512;
      break;
    case rrc_ReceivingWindowSize_rw768:
      rlc_rx_win_size = 768;
      break;
    case rrc_ReceivingWindowSize_rw1024:
      rlc_rx_win_size = 1024;
      break;
    case rrc_ReceivingWindowSize_rw1536:
      rlc_rx_win_size = 1536;
      break;
    case rrc_ReceivingWindowSize_rw2047:
      rlc_rx_win_size = 2047;
      break;
    case rrc_ReceivingWindowSize_rw2560:
      rlc_rx_win_size = 2560;
      break;
    case rrc_ReceivingWindowSize_rw3072:
      rlc_rx_win_size = 3072;
      break;
    case rrc_ReceivingWindowSize_rw3584:
      rlc_rx_win_size = 3584;
      break;
    case rrc_ReceivingWindowSize_rw4095:
      rlc_rx_win_size = 4095;
      break;
    default:
      WRRC_MSG1_ERROR("Invalid Window Size %d", rrc_rx_window_size); 
      break;
  } /* end window size choice */

  return rlc_rx_win_size;

} /* end get_rx_window_size() */

/*==========================================================
FUNCTION  : get_window_size()

DESCRIPTION : 
  Return the Window Size for transmitting.

DEPENDENCIES: 
  None.

RETURN VALUE:
  Window size for transmitting RLC.

SIDE EFFECTS:
  None.
============================================================*/
static uint16 get_window_size
(
  rrc_TransmissionWindowSize rrc_tx_window_size
)
{
  /* Initialize the window size  */
  uint16 rlc_tx_win_size = (uint16)RRCLLC_INVALID_WINDOW_SIZE;

  switch (rrc_tx_window_size)
  {
    case rrc_TransmissionWindowSize_tw1:
      rlc_tx_win_size = 1;
      break;
    case rrc_TransmissionWindowSize_tw8:
      rlc_tx_win_size = 8;
      break;
    case rrc_TransmissionWindowSize_tw16:
      rlc_tx_win_size = 16;
      break;
    case rrc_TransmissionWindowSize_tw32:
      rlc_tx_win_size = 32;
      break;
    case rrc_TransmissionWindowSize_tw64:
      rlc_tx_win_size = 64;
      break;
    case rrc_TransmissionWindowSize_tw128:
      rlc_tx_win_size = 128;
      break;
    case rrc_TransmissionWindowSize_tw256:
      rlc_tx_win_size = 256;
      break;
    case rrc_TransmissionWindowSize_tw512:
      rlc_tx_win_size = 512;
      break;
    case rrc_TransmissionWindowSize_tw768:
      rlc_tx_win_size = 768;
      break;
    case rrc_TransmissionWindowSize_tw1024:
      rlc_tx_win_size = 1024;
      break;
    case rrc_TransmissionWindowSize_tw1536:
      rlc_tx_win_size = 1536;
      break;
    case rrc_TransmissionWindowSize_tw2047:
      rlc_tx_win_size = 2047;
      break;
    case rrc_TransmissionWindowSize_tw2560:
      rlc_tx_win_size = 2560;
      break;
    case rrc_TransmissionWindowSize_tw3072:
      rlc_tx_win_size = 3072;
      break;
    case rrc_TransmissionWindowSize_tw3584:
      rlc_tx_win_size = 3584;
      break;
    case rrc_TransmissionWindowSize_tw4095:
      rlc_tx_win_size = 4095;
      break;
    default:
      WRRC_MSG1_ERROR("Invalid Window Size %d", rrc_tx_window_size); 
      break;
  } /* end window size choice */

  return rlc_tx_win_size;

} /* end get_window_size() */

/*==========================================================
FUNCTION  : rrcllc_get_1st_hole_for_rlc_am

DESCRIPTION : 
  Find the first am_config block that is not used.
  
DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uint8 rrcllc_get_1st_hole_for_rlc_am( void )
{
  uint8 am_entity_idx = UE_MAX_AM_ENTITY;
  int i;

  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    /* Since we reset the am_config to 0 once it is not used, 
       the ul_nchan and dl_nchan should be 0 if it is a hole.
     */
    if(ordered_config_ptr->rlc_am_parms.am_info[i]->ul_nchan == 0 && 
       ordered_config_ptr->rlc_am_parms.am_info[i]->dl_nchan == 0)
    {
      am_entity_idx = (uint8)i;
     ordered_config_ptr->rlc_am_parms.am_info[i]->rlc_dl_am_config.am_pdu_size_flexbile = FALSE;

      WRRC_MSG1_MED("Got empty RLC AM Info slot %d", i);
      break;
    }
  }
  return am_entity_idx;
}

/*==========================================================
FUNCTION  : rrcllc_get_1st_hole_for_rlc_ul_um

DESCRIPTION : 
  Find the first unused rlc_ul_um_parms block in OC.
  
DEPENDENCIES: 
  None.

RETURN VALUE:
  entity index

SIDE EFFECTS:
  None.
============================================================*/
uint8 rrcllc_get_1st_hole_for_rlc_ul_um
(
  void
)
{
  uint8 ul_um_entity_idx = UE_MAX_UL_UM_CHANNEL;
  int i;

  for(i = 0; i < UE_MAX_UL_UM_CHANNEL; i++)
  {
    if(ordered_config_ptr->rlc_ul_um_parms.chan_info[i].rb_id == RRCLLC_INVALID_RB_ID )
    {
      ul_um_entity_idx = (uint8)i;
      WRRC_MSG1_MED("Got empty RLC UL UM Info slot %d. (8 = UE_MAX_UL_UM_CHANNEL)", i);
      break;
    }
  }
  if(ul_um_entity_idx == UE_MAX_UL_UM_CHANNEL)
  {
    WRRC_MSG0_ERROR("Empty UL UM idx not found");
  }
  return ul_um_entity_idx;
}

/*==========================================================
FUNCTION  : rrcllc_get_1st_hole_for_rlc_dl_um

DESCRIPTION : 
  Find the first unused rlc_dl_um_parms block in Config DB.
  
DEPENDENCIES: 
  None.

RETURN VALUE:
  entity index

SIDE EFFECTS:
  None.
============================================================*/
uint8 rrcllc_get_1st_hole_for_rlc_dl_um
(
  ordered_config_type * config_ptr
)
{
  uint8 dl_um_entity_idx = UE_MAX_DL_UM_CHANNEL;
  int i;
  for(i = 0; i < UE_MAX_DL_UM_CHANNEL; i++)
  {
    if(config_ptr->rlc_dl_um_parms.chan_info[i].rb_id == RRCLLC_INVALID_RB_ID )
    {
      dl_um_entity_idx = (uint8)i;
      WRRC_MSG1_MED("Got empty RLC DL UM Info slot %d. (8 = UE_MAX_DL_UM_CHANNEL)", i);
      break;
    }
  }
  if(dl_um_entity_idx == UE_MAX_DL_UM_CHANNEL)
  {
    WRRC_MSG0_ERROR("Empty DL UM idx not found");
  }
  return dl_um_entity_idx;
}

/*==========================================================
FUNCTION  : get_ul_am_rlc_info()

DESCRIPTION : 
  Read the information for Uplink AM RLC from the received 
  structure and write them into one of the pre-allocated 
  AM memory block.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_ul_am_rlc_info
(
  rrc_UL_AM_RLC_Mode *ie_ptr,
  uint8               entity_idx
)
{
  rlc_ul_info_type *rlc_ul_info;

  /* No am_config index being assigned. */
  if(entity_idx == UE_MAX_AM_ENTITY)
  {
    WRRC_MSG0_ERROR("Invalid AM config is passed");
    return FAILURE;
  }

  rlc_ul_info = 
    &ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;

  /* Turn on the information included flag  */
  rlc_ul_info->rlc_info_incl = TRUE;

  /* config the SDU/PDU discard scheme parameters */
  if(get_xmit_rlc_discard(rlc_ul_info, &ie_ptr->transmissionRLC_Discard) == FAILURE)
  {
    return FAILURE;
  }



  /* Get the maximum number of RLC PDUs sent without requiring an ack  */
  if((rlc_ul_info->tx_window_size = get_window_size(ie_ptr->transmissionWindowSize))
    == RRCLLC_INVALID_WINDOW_SIZE)
  {
    return FAILURE;
  }

  /* Get the timer value used to detect the loss of reset ack pdu */
  switch(ie_ptr->timerRST)
  {
    case rrc_TimerRST_tr50:
      rlc_ul_info->timer_rst = 50;
      break;
    case rrc_TimerRST_tr100:
      rlc_ul_info->timer_rst = 100;
      break;
    case rrc_TimerRST_tr150:
      rlc_ul_info->timer_rst = 150;
      break;
    case rrc_TimerRST_tr200:
      rlc_ul_info->timer_rst = 200;
      break;
    case rrc_TimerRST_tr250:
      rlc_ul_info->timer_rst = 250;
      break;
    case rrc_TimerRST_tr300:
      rlc_ul_info->timer_rst = 300;
      break;
    case rrc_TimerRST_tr350:
      rlc_ul_info->timer_rst = 350;
      break;
    case rrc_TimerRST_tr400:
      rlc_ul_info->timer_rst = 400;
      break;
    case rrc_TimerRST_tr450:
      rlc_ul_info->timer_rst = 450;
      break;
    case rrc_TimerRST_tr500:
      rlc_ul_info->timer_rst = 500;
      break;
    case rrc_TimerRST_tr550:
      rlc_ul_info->timer_rst = 550;
      break;
    case rrc_TimerRST_tr600:
      rlc_ul_info->timer_rst = 600;
      break;
    case rrc_TimerRST_tr700:
      rlc_ul_info->timer_rst = 700;
      break;
    case rrc_TimerRST_tr800:
      rlc_ul_info->timer_rst = 800;
      break;
    case rrc_TimerRST_tr900:
      rlc_ul_info->timer_rst = 900;
      break;
    case rrc_TimerRST_tr1000:
      rlc_ul_info->timer_rst = 1000;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown Timer RST type %d", ie_ptr->timerRST);
      return FAILURE;
  } /* end Timer_RST */

  /* Get the Maximum number of retransmissions of the the RESET_PDU */
  switch (ie_ptr->max_RST)
  {
    case rrc_MaxRST_rst1:
      rlc_ul_info->maxrst = 1;
      break;
    case rrc_MaxRST_rst4:
      rlc_ul_info->maxrst = 4;
      break;
    case rrc_MaxRST_rst6:
      rlc_ul_info->maxrst = 6;
      break;
    case rrc_MaxRST_rst8:
      rlc_ul_info->maxrst = 8;
      break;
    case rrc_MaxRST_rst12:
      rlc_ul_info->maxrst = 12;
      break;
    case rrc_MaxRST_rst16:
      rlc_ul_info->maxrst = 16;
      break;
    case rrc_MaxRST_rst24:
      rlc_ul_info->maxrst = 24;
      break;
    case rrc_MaxRST_rst32:
      rlc_ul_info->maxrst = 32;
      break;
    default:
      WRRC_MSG1_ERROR("Unknown MAX RST type %d", ie_ptr->max_RST);
      return FAILURE;
  } /* end Max_RST */

  /* Configure Poll scheme parameters  */
  if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,pollingInfo))
  {
    if(get_polling_info(rlc_ul_info, &ie_ptr->pollingInfo) == FAILURE)
    {
      return FAILURE;
    }
  }
  else
  {
    rlc_ul_info->poll_info.poll_info_incl = FALSE; 
  }

  return SUCCESS;
} /* end get_ul_am_rlc_info() */

/*==========================================================
FUNCTION  : get_ul_um_rlc_info

DESCRIPTION : 
  Read the information for Uplink UM RLC from the received 
  structure and write them into one of the pre-allocated 
  UM memory block.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_ul_um_rlc_info
(
  rrc_UL_UM_RLC_Mode *ie_ptr,
  uint8               entity_idx
)
{
  /* local copy of TM pointer */
  if(entity_idx == UE_MAX_UL_UM_CHANNEL)
  {
    entity_idx = ordered_config_ptr->rlc_ul_um_parms.nchan;
  }

  /*
  * For RLC UL UM only Timer based no explicit discard is allowed. This is not
  * supported by the UE.
  */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_UM_RLC_Mode,transmissionRLC_Discard))
  {
    switch (ie_ptr->transmissionRLC_Discard.t)
    {
      case T_rrc_TransmissionRLC_Discard_timerBasedNoExplicit:
        {
          ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].timer_discard = 
              get_ul_um_timer_discard(ie_ptr->transmissionRLC_Discard.u.timerBasedNoExplicit);
          WRRC_MSG1_HIGH("RLC UL UM: Timer based no explicit %d", 
              ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].timer_discard);
        }
        break;

      case T_rrc_TransmissionRLC_Discard_timerBasedExplicit:
      case T_rrc_TransmissionRLC_Discard_maxDAT_Retransmissions:
      case T_rrc_TransmissionRLC_Discard_noDiscard:
      default:

        WRRC_MSG1_ERROR("UL RLC UM %d Retrans not allowed/unknown, return FAILURE",
                  ie_ptr->transmissionRLC_Discard.t);
        return FAILURE;
    } /* end rlc discard choice */
  } /* end timer discard check */
  else
  {
    ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].timer_discard = 0;
  }

  /* everything okay if here */
  return SUCCESS;

} /* end get_ul_um_rlc_info() */

#ifdef FEATURE_RRC_TM_RLC_INFO
/*==========================================================
FUNCTION  : get_ul_tm_rlc_info()

DESCRIPTION : 
  Read the information for UpLink TM RLC from the received 
  structure and write them into one of the pre-allocated 
  TM memory block.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
/*lint -esym(714,get_ul_tm_rlc_info) */
static uecomdef_status_e_type get_ul_tm_rlc_info
(
  rrc_UL_TM_RLC_Mode *ie_ptr,
  uint8               entity_idx
)
{
  rlc_ul_tm_channel_config_type *ul_tm_chan_config;

  /* local copy of TM pointer */
  if(entity_idx == UE_MAX_UL_TM_CHANNEL)
  {
    entity_idx = ordered_config_ptr->rlc_ul_tm_parms.nchan;
  }
  ul_tm_chan_config = 
    &ordered_config_ptr->rlc_ul_tm_parms.chan_info[entity_idx];

  /* Set up the segmentation indication */
  ul_tm_chan_config->seg_ind = ie_ptr->segmentationIndication;

  /*
  * For RLC UL TM only Timer based no explicit discard is allowed. This is not
  * supported by the UE.
  */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_TM_RLC_Mode,transmissionRLC_Discard))
  {
    switch (ie_ptr->transmissionRLC_Discard.t)
    {
      case T_rrc_TransmissionRLC_Discard_timerBasedNoExplicit:
        break;

      case T_rrc_TransmissionRLC_Discard_timerBasedExplicit:
      case T_rrc_TransmissionRLC_Discard_maxDAT_Retransmissions:
      case T_rrc_TransmissionRLC_Discard_noDiscard:
      default:

        WRRC_MSG1_ERROR("UL RLC TM %d Retrans not allowed/Unknown type, return FAILURE",
                  ie_ptr->transmissionRLC_Discard.t);
        return FAILURE;

    } /* end rlc discard choice */
  } /* end timer discard check */

  /* everything okay if here */
  return SUCCESS;

} /* end get_ul_tm_rlc_info() */

#endif

/*==========================================================
FUNCTION  : get_dl_rlc_status_info

DESCRIPTION : 
  Config the AM status information in Downlink.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_dl_rlc_status_info
(
  rlc_dl_info_type      *rlc_dl_info,
  rrc_DL_RLC_StatusInfo *ie_ptr 
)
{
  /* indicate that have some info for DL status */
  rlc_dl_info->status_info.status_info_incl = TRUE;

 /*-------------------------------------------------------- 
    Mandatory parameters.
  ---------------------------------------------------------*/

  /* True indicates that the UE should send a status report for each detected missing PU */
  rlc_dl_info->status_info.missing_pdu = ie_ptr->missingPDU_Indicator;
  
 /*-------------------------------------------------------- 
    Optional parameters.
  ---------------------------------------------------------*/

  /* Optional parameters that are not included indicate that the corresponding
   * function should be disabled in RLC, so initialize those functions to
   * off here. (25.331 8.6.4.9)
   */

  rlc_dl_info->status_info.status_prohibit = 0;
  rlc_dl_info->status_info.timer_epc = 0;
  rlc_dl_info->status_info.status_periodic = 0;

  /*
  * Get the minimum time in ms between status reports. The values range from 10 to
  * 50, in step sizes of 10. Note that the ASN.1 enum allows for for up to 10,000.
  */
  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,timerStatusProhibit))
  {
    if(ie_ptr->timerStatusProhibit <= rrc_TimerStatusProhibit_tsp550)
    {
      rlc_dl_info->status_info.status_prohibit = (((uint16)ie_ptr->timerStatusProhibit + 1) * 10);
    }
    /*lint -save -e685 */
    else if((ie_ptr->timerStatusProhibit > rrc_TimerStatusProhibit_tsp550) && 
            (ie_ptr->timerStatusProhibit <= rrc_TimerStatusProhibit_tsp1000))
    {
      rlc_dl_info->status_info.status_prohibit = (550 + 
                                                          (((uint16)ie_ptr->timerStatusProhibit - (uint16)rrc_TimerStatusProhibit_tsp550) * 50));
    }
    /*lint -restore */
    else
    {
      WRRC_MSG1_ERROR("Invalid Timer Status Prohibit %d", ie_ptr->timerStatusProhibit);
      return FAILURE;
    }
  }
  
  /* Periodic Timer status, in ms */
  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,timerStatusPeriodic))
  {
    switch (ie_ptr->timerStatusPeriodic)
    {
      case rrc_TimerStatusPeriodic_tsp100:
        rlc_dl_info->status_info.status_periodic = 100;
        break;
      case rrc_TimerStatusPeriodic_tsp200:
        rlc_dl_info->status_info.status_periodic = 200;
        break;
      case rrc_TimerStatusPeriodic_tsp300:
        rlc_dl_info->status_info.status_periodic = 300;
        break;
      case rrc_TimerStatusPeriodic_tsp400:
        rlc_dl_info->status_info.status_periodic = 400;
        break;
      case rrc_TimerStatusPeriodic_tsp500:
        rlc_dl_info->status_info.status_periodic = 500;
        break;
      case rrc_TimerStatusPeriodic_tsp750:
        rlc_dl_info->status_info.status_periodic = 750;
        break;
      case rrc_TimerStatusPeriodic_tsp1000:
        rlc_dl_info->status_info.status_periodic = 1000;
        break;
      case rrc_TimerStatusPeriodic_tsp2000:
        rlc_dl_info->status_info.status_periodic = 2000;
        break;
      default:
        WRRC_MSG1_ERROR("Unknown Timer status Periodic type %d",
                   ie_ptr->timerStatusPeriodic);
        return FAILURE;
    } /* end periodic timer status choice */
  } /* end periodic timer status check */
  
  /* if got here, everything is okay */
  return SUCCESS;

} /* end get_rlc_dl_status_info() */


/*==========================================================
FUNCTION  : get_dl_am_rlc_info_r5()

DESCRIPTION : 
  Read the information for Downlink AM RLC from the 
  received structure and write them into one of the 
  pre-allocated AM memory block.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_dl_am_rlc_info_r5
(
rrc_DL_AM_RLC_Mode_r5 *ie_ptr,
uint8               entity_idx,
rrc_RB_Identity          rb_id,
boolean       allow_new_channels,
rlc_reestablish_e_type reestablish_entity
)
{
  rlc_dl_info_type *rlc_dl_info;
  
  /* local copy of poitner */
  if (entity_idx == UE_MAX_AM_ENTITY)
  {
    entity_idx = ordered_config_ptr->rlc_am_parms.nentity;
  }
  rlc_dl_info = 
    &ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
  
  /* Turn on the information included flag  */
  rlc_dl_info->rlc_info_incl = TRUE;
  
  /* indicates if in sequence delivery is required */
  rlc_dl_info->in_sequence = ie_ptr->inSequenceDelivery;
  
  /* Get the maximum number of RLC PUs allowed to be rcv'd  */
  if ((rlc_dl_info->rx_window_size = get_rx_window_size(ie_ptr->receivingWindowSize))
    == RRCLLC_INVALID_WINDOW_SIZE)
  {
    return FAILURE;
  }
  
  /* Get the downlink status info */
  if (get_dl_rlc_status_info(rlc_dl_info, &ie_ptr->dl_RLC_StatusInfo) == FAILURE)
  {
    return FAILURE;
  }
  

  if(rrcllc_process_dl_pdu_size(&ie_ptr->dl_RLC_PDU_size, entity_idx,allow_new_channels,reestablish_entity)
    == SUCCESS)
  {
      /* Set this RLC Size in the global structure rlc_size_per_rb */
    if (rrc_set_dl_am_rlc_pdu_size(rb_id, ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size) == FAILURE)
    {
      WRRC_MSG0_ERROR("Unable to set DL AM PDU Size Info in rlc_per_rb, return FAILURE");
      return FAILURE;
    }
  }
  else
  {
    return FAILURE;
  }
  
  /* everything is a go */
  return SUCCESS;

} /* end get_dl_am_rlc_info_r5() */

/*==========================================================
FUNCTION  : get_dl_am_rlc_info()

DESCRIPTION : 
  Read the information for Downlink AM RLC from the 
  received structure and write them into one of the 
  pre-allocated AM memory block.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_dl_am_rlc_info
(
  rrc_DL_AM_RLC_Mode *ie_ptr,
  uint8               entity_idx
)
{
  rlc_dl_info_type *rlc_dl_info;

  /* LLC processes the UL AM info first, the entity idx should be available
     when processing the DL AM info.
   */
  if(entity_idx == UE_MAX_AM_ENTITY)
  {
     WRRC_MSG0_ERROR("Invalid AM config is passed");
    return FAILURE;
  }
  rlc_dl_info = &ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;

  /* Turn on the information included flag  */
  rlc_dl_info->rlc_info_incl = TRUE;
    ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile = FALSE;  
    if(current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile == TRUE ) 
    {
      ordered_config_ptr->am_config[entity_idx].reestab_entity = RLC_RE_ESTABLISH_DL_UL;
      WRRC_MSG2_HIGH("MACEHS: Re-establish dl am enity %d PDU type changed from  flexible to fixed for rb-id %d ",
      entity_idx,ordered_config_ptr->am_config[entity_idx].common_config.rb_id);
    }

  /* indicates if in sequence delivery is required */
  rlc_dl_info->in_sequence = ie_ptr->inSequenceDelivery;

  /* Get the maximum number of RLC PUs allowed to be rcv'd  */
  if((rlc_dl_info->rx_window_size = get_rx_window_size(ie_ptr->receivingWindowSize))
    == RRCLLC_INVALID_WINDOW_SIZE)
  {
    return FAILURE;
  }
  if((ordered_config_ptr->rb_am_rlc_size_ota[entity_idx].rrc_pdu_type
 == RRC_PDU_SIZE_NOT_SET )
     ||(current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile == TRUE )
      )
  {
    if(current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile)
    {
      WRRC_MSG0_HIGH("Change from Flexible PDU to fixed using r3 OTA, so calculate size from Trch config");
    }
    rrcllc_set_am_dl_rlc_size_ota(ordered_config_ptr->am_config[entity_idx].common_config.rb_id,
           RRCLLC_INVALID_RLC_SIZE,RRC_PDU_SIZE_NOT_SET);     
  }

  /* Get the downlink status info */
  if(get_dl_rlc_status_info(rlc_dl_info, &ie_ptr->dl_RLC_StatusInfo) == FAILURE)
  {
    return FAILURE;
  } 
  
  /* everything is a go */
  return SUCCESS;

} /* end get_dl_am_rlc_info() */

/*==========================================================
FUNCTION  : get_am_index_in_toc()

DESCRIPTION : Returns the index of the am_config info
              associated with the given RB id, or
              UE_MAX_AM_ENTITY if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_am_index_in_toc
(
  rrc_RB_Identity rb_id
)
{
  uint8 i;
  uint8 result = UE_MAX_AM_ENTITY;
  ordered_config_type * config_ptr = transition_config.toc_ptr;
  
  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    /* This function will be called by get_rlc_info() when fills in RLC UL info and
       RLC DL info. Since RRC fills in UL first, the ul_nchan will be non-zero when
       fills RLC DL info and calls this function. 
       Otherwise, for an empty am_info block, the ul_nchan and dl_nchan should be zero.
     */
    if((config_ptr->rlc_am_parms.am_info[i]->common_config.rb_id == rb_id) &&
       (config_ptr->rlc_am_parms.am_info[i]->ul_nchan ||
          config_ptr->rlc_am_parms.am_info[i]->dl_nchan)
       )
    {
      result = i;
      WRRC_MSG2_HIGH("Found RLC AM for RB %d, idx %d in TOC", rb_id, i);
      break;
    }
  }

  return result;
}

/*==========================================================
FUNCTION  : get_am_index()

DESCRIPTION : Returns the index of the am_config info
              associated with the given RB id, or
              UE_MAX_AM_ENTITY if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_am_index
(
  rrc_RB_Identity rb_id
)
{
  uint8 i;
  uint8 result = UE_MAX_AM_ENTITY;

  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    /* This function will be called by get_rlc_info() when fills in RLC UL info and
       RLC DL info. Since RRC fills in UL first, the ul_nchan will be non-zero when
       fills RLC DL info and calls this function. 
       Otherwise, for an empty am_info block, the ul_nchan and dl_nchan should be zero.
     */
    if((ordered_config_ptr->rlc_am_parms.am_info[i]->common_config.rb_id == rb_id) &&
       (ordered_config_ptr->rlc_am_parms.am_info[i]->ul_nchan ||
          ordered_config_ptr->rlc_am_parms.am_info[i]->dl_nchan)
       )
    {
      result = i;
      WRRC_MSG2_MED("Found RLC AM for RB %d, idx %d", rb_id, i);
      break;
    }
  }

  return result;
} /* end get_am_index */
/*==========================================================
FUNCTION  : get_am_index_wo_f3()

DESCRIPTION : Returns the index of the am_config info
              associated with the given RB id, or
              UE_MAX_AM_ENTITY if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_am_index_wo_f3
(
  rrc_RB_Identity rb_id
)
{
  uint8 i;
  uint8 result = UE_MAX_AM_ENTITY;

  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    /* This function will be called by get_rlc_info() when fills in RLC UL info and
       RLC DL info. Since RRC fills in UL first, the ul_nchan will be non-zero when
       fills RLC DL info and calls this function. 
       Otherwise, for an empty am_info block, the ul_nchan and dl_nchan should be zero.
     */
    if((ordered_config_ptr->rlc_am_parms.am_info[i]->common_config.rb_id == rb_id) &&
       (ordered_config_ptr->rlc_am_parms.am_info[i]->ul_nchan ||
          ordered_config_ptr->rlc_am_parms.am_info[i]->dl_nchan)
       )
    {
      result = i;
      break;
    }
  }

  return result;
} /* end get_am_index */
/*==========================================================
FUNCTION  : get_ul_um_index()

DESCRIPTION : Returns the index of the UL UM channel config
              info associated with the given logical channel id,
              or UE_MAX_UL_UM_CHANNEL if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_ul_um_index
(
  rrc_RB_Identity rb_id
)
{
  uint8 i;
  uint8 result = UE_MAX_UL_UM_CHANNEL;

  if(rb_id != RRCLLC_INVALID_RB_ID)
  {
    for(i = 0; i < UE_MAX_UL_UM_CHANNEL; i++)
    {
      if(ordered_config_ptr->rlc_ul_um_parms.chan_info[i].rb_id== rb_id)
      {
        result = i;
        break;
      }
    }
  }
  return result;
} /* end get_ul_um_index */
/*==========================================================
FUNCTION  : get_um_ul_index()

DESCRIPTION : Returns the index of the UL UM channel config
              info associated with the given logical channel id,
              or UE_MAX_UL_UM_CHANNEL if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_um_ul_index
(
  rrc_RB_Identity rb_id,
  ordered_config_type * config_ptr
)
{
  uint8 i;
  uint8 result = UE_MAX_UL_UM_CHANNEL;

  if(rb_id != RRCLLC_INVALID_RB_ID)
  {
    for(i = 0; i < UE_MAX_UL_UM_CHANNEL; i++)
    {
      if(config_ptr->rlc_ul_um_parms.chan_info[i].rb_id == rb_id)
      {
        result = i;
        break;
      }
    }
  }
  return result;
} 

/*==========================================================
FUNCTION  : get_ul_tm_index_in_config_ptr()

DESCRIPTION : Returns the index of the UL TM channel config
              info associated with the given logical channel id,
              or UE_MAX_UL_TM_CHANNEL if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
static uint8 get_ul_tm_index_in_config_ptr
(
  rlc_lc_id_type lc_id,
  ordered_config_type * config_ptr
)
{
  uint8 i;
  uint8 result = UE_MAX_UL_TM_CHANNEL;

  if(lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    for(i = 0; i < config_ptr->rlc_ul_tm_parms.nchan; i++)
    {
      if(config_ptr->rlc_ul_tm_parms.chan_info[i].lc_id == lc_id)
      {
        result = i;
        break;
      }
    }
  }

  return result;
} /* end get_ul_tm_index */
/*==========================================================
FUNCTION  : get_ul_tm_index()

DESCRIPTION : Returns the index of the UL TM channel config
              info associated with the given logical channel id,
              or UE_MAX_UL_TM_CHANNEL if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
static uint8 get_ul_tm_index
(
  rlc_lc_id_type lc_id
)
{
  uint8 i;
  uint8 result = UE_MAX_UL_TM_CHANNEL;

  if(lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    for(i = 0; i < ordered_config_ptr->rlc_ul_tm_parms.nchan; i++)
    {
      if(ordered_config_ptr->rlc_ul_tm_parms.chan_info[i].lc_id == lc_id)
      {
        result = i;
        break;
      }
    }
  }

  return result;
} /* end get_ul_tm_index */

/*==========================================================
FUNCTION  : get_dl_um_index()

DESCRIPTION : Returns the index of the DL UM channel config
              info associated with the given logical channel id,
              or UE_MAX_DL_UM_CHANNEL if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_dl_um_index
(
  rrc_RB_Identity rb_id,
  ordered_config_type * config_ptr
)
{
  uint8 i;
  uint8 result = UE_MAX_DL_UM_CHANNEL;

  if(rb_id != RRCLLC_INVALID_RB_ID)
  {
    for(i = 0; i < UE_MAX_DL_UM_CHANNEL; i++)
    {
      if(config_ptr->rlc_dl_um_parms.chan_info[i].rb_id == rb_id)
      {
        result = i;
        break;
      }
    }
  }
  return result;
} /* end get_dl_um_index */

/*==========================================================
FUNCTION  : get_dl_tm_index()

DESCRIPTION : Returns the index of the DL TM channel config
              info associated with the given logical channel id,
              or UE_MAX_DL_TM_CHANNEL if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
static uint8 get_dl_tm_index
(
  rlc_lc_id_type lc_id
)
{
  uint8 i;
  uint8 result = UE_MAX_DL_TM_CHANNEL;

  if(lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    for(i = 0; i < ordered_config_ptr->rlc_dl_tm_parms.nchan; i++)
    {
      if(ordered_config_ptr->rlc_dl_tm_parms.chan_info[i].lc_id == lc_id)
      {
        result = i;
        break;
      }
    }
  }

  return result;
} /* end get_dl_tm_index */

/*==========================================================
FUNCTION  : get_rlc_ul_um_info ()

DESCRIPTION : 
  Processes RLC Info IE for UL UM RBs.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
uecomdef_status_e_type get_rlc_ul_um_info
(
  rrc_RB_Identity    rb_id,
  boolean       allow_new_channels 
)
{
  uint8 entity_idx;
  rlc_lc_id_type lc_id;
  uecomdef_status_e_type status_var = SUCCESS;
  entity_idx = get_ul_um_index(rb_id);
      
  if(entity_idx >= UE_MAX_UL_UM_CHANNEL)
  { 
    if (!allow_new_channels)
    {
      status_var = FAILURE;
    }
    else
    {
      entity_idx = rrcllc_get_1st_hole_for_rlc_ul_um();
  
      if (entity_idx >= UE_MAX_UL_UM_CHANNEL)
      {
        status_var = FAILURE;
      }

      if(status_var == SUCCESS)
      {
        ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].rb_id = rb_id;
        ordered_config_ptr->rlc_ul_um_parms.nchan++;
      }
      
      /* Make sure UE has not exceeded the maximum number of UL UM Log CH's */
      if (ordered_config_ptr->rlc_ul_um_parms.nchan > UE_MAX_UL_UM_CHANNEL)
      {
        status_var = FAILURE;
      }
      }
  
  
    if(status_var == FAILURE)
    {
       MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Wrong UL UM params for RB-ID %d. allow_new_channels %d(FALSE->Not allowed to add), entity_idx %d (8 is invalid),rlc_ul_um_parms.nchan %d(>8 is invalid)",
                      rb_id,
                      allow_new_channels,
                      entity_idx,
                      ordered_config_ptr->rlc_ul_um_parms.nchan);
  
      return FAILURE;
    }
  
  }

  ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].lc_type = 
      get_lc_type_by_um_rb_id(rb_id);
    
  lc_id = rrclcm_get_ul_rlc_lc_id(rb_id);
      
  
  if (lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
      WRRC_MSG2_MED("LC id for RB id %d = %d",rb_id,lc_id);
    ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].lc_id =  lc_id;
  }
  return SUCCESS;
}

/*==========================================================
FUNCTION  : get_rlc_dl_um_info ()

DESCRIPTION : 
  Processes RLC Info IE for DL UM RBs.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
uecomdef_status_e_type get_rlc_dl_um_info
(
  rrc_RB_Identity    rb_id,
  boolean       allow_new_channels,
  ordered_config_type * config_ptr  
)
{
  uint8 entity_idx;
  rlc_lc_id_type lc_id;
  uecomdef_status_e_type status_var = SUCCESS;
  
  entity_idx = get_dl_um_index(rb_id, config_ptr);
      
  if(entity_idx >= UE_MAX_DL_UM_CHANNEL)
  {

    if (!allow_new_channels)
    {
      status_var =  FAILURE;
    }
    else
    {
      entity_idx = rrcllc_get_1st_hole_for_rlc_dl_um(config_ptr);
  
      if (entity_idx >= UE_MAX_DL_UM_CHANNEL)
      {
        status_var =  FAILURE;
      }

      if(status_var == SUCCESS)
      {
      config_ptr->rlc_dl_um_parms.chan_info[entity_idx].rb_id = rb_id;
      config_ptr->rlc_dl_um_parms.nchan++;
      }
      /* Make sure UE has not exceeded the maximum number of DL UM Log CH's */

      if(config_ptr->rlc_dl_um_parms.nchan > UE_MAX_DL_UM_CHANNEL)
      {
        status_var =  FAILURE;
      }
    }
  
    if(status_var == FAILURE)
    {
       MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Wrong DL UM params for RB-ID %d. allow_new_channels %d(FALSE->Not allowed to add), entity_idx %d (8 is invalid),rlc_dl_um_parms.nchan %d(>8 is invalid)",
                      rb_id,
                      allow_new_channels,
                      entity_idx,
                      ordered_config_ptr->rlc_dl_um_parms.nchan);
  
      return FAILURE;
    }
  
  }
  
  config_ptr->rlc_dl_um_parms.chan_info[entity_idx].lc_type = 
      get_lc_type_by_um_rb_id(rb_id);

  lc_id = rrclcm_get_dl_rlc_lc_id(rb_id);
      
  WRRC_MSG2_MED("LC id for RB id %d = %d",rb_id,lc_id);
  
  if (lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
    config_ptr->rlc_dl_um_parms.chan_info[entity_idx].lc_id =  lc_id;
  }
  return SUCCESS;
}

/*==========================================================
FUNCTION  : get_ul_rlc_info()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_rlc_info
(
  rrc_UL_RLC_Mode *rrc_ul_RLC_Mode,
  rrc_RB_Identity    rb_id,
  boolean       allow_new_channels,
  boolean * new_rlc_am_entity
)
{
      uint8 entity_idx;
  rlc_lc_id_type lc_id;

    switch (rrc_ul_RLC_Mode->t)
    {
    case T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode:
      MSG_MED("Get UL AM RLC Info for RB %d", rb_id,0,0);
      
      entity_idx = get_am_index(rb_id);
      if(entity_idx == UE_MAX_AM_ENTITY)
      {
        if (!allow_new_channels)
        {
          WRRC_MSG0_ERROR("Not allowed to add new UL AM RLC entity");
          return FAILURE;
        }
        else
        {
          /* Find an unused am_config block.  */
          entity_idx = rrcllc_get_1st_hole_for_rlc_am();

          if (entity_idx == UE_MAX_AM_ENTITY)
          {
            WRRC_MSG0_ERROR("unable to get empty RLC AM Info slot");
            return FAILURE;
          }
          WRRC_MSG2_MED("Allocate RLC AM slot %d for RB %d", entity_idx, rb_id);
          *new_rlc_am_entity = TRUE;
          
          /* It always assume to be the shared channel (Control&Data) 
          as we support now. */  
          
          ordered_config_ptr->rlc_am_parms.am_info[entity_idx]->ul_nchan = 1;
        }
      }
      
      if (get_ul_am_rlc_info(rrc_ul_RLC_Mode->u.ul_AM_RLC_Mode, entity_idx) == FAILURE)
      {
        return FAILURE;
      }
      
      /* It's no harm to re-fill in the rb_id again, if it does exist.  */
      ordered_config_ptr->rlc_am_parms.am_info[entity_idx]->common_config.rb_id = rb_id;
      
      if((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL9))
      { 
           /*For SRB2 set Max_RST to 1 - Rel 10 Spec CR 4375*/
        if(rb_id == DCCH_AM_RADIO_BEARER_ID && (rrc_ul_RLC_Mode->u.ul_AM_RLC_Mode->max_RST > rrc_MaxRST_rst1))
        {
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info.maxrst = 1;
          WRRC_MSG1_MED("Setting Max_RST to 1 for SRB%d",rb_id);
        }
      }
      break;
      
    case T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode:

      if(get_rlc_ul_um_info(rb_id, allow_new_channels) == FAILURE)
      {
        return FAILURE;
      }

      entity_idx = get_ul_um_index(rb_id);
    
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rrc_ul_RLC_Mode->u.ul_UM_RLC_Mode,rrc_UL_UM_RLC_Mode,transmissionRLC_Discard))
      {
        if (get_ul_um_rlc_info(rrc_ul_RLC_Mode->u.ul_UM_RLC_Mode, entity_idx) == FAILURE)
        {
          return FAILURE;
        }
      }
      break;
      
    case T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode: 
      MSG_MED("Get UL TM RLC Info for RB %d", rb_id,0,0);
      
      lc_id = rrclcm_get_ul_rlc_lc_id(rb_id);
      entity_idx = get_ul_tm_index(lc_id);
      
#ifdef FEATURE_RRC_TM_RLC_INFO
      if (entity_idx == UE_MAX_UL_TM_CHANNEL && !allow_new_channels)
      {
        WRRC_MSG0_ERROR("Not allowed to add new UL TM RLC entity");
        return FAILURE;
      }
      
      if (get_ul_tm_rlc_info(rrc_ul_RLC_Mode->u.ul_TM_RLC_Mode, entity_idx) == FAILURE)
      {
        return FAILURE;
      }
#endif /* FEATURE_RRC_TM_RLC_INFO */
      
      if (entity_idx == UE_MAX_UL_TM_CHANNEL)
      {
        if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
        {
        /* There was no logical channel ID for the given radio bearer,
          so we need to add a new RLC channel. */
          ordered_config_ptr->rlc_ul_tm_parms.nchan++;
          WRRC_MSG1_MED("nchan: UL TM %d in get_rlc_info() after alloc",
            ordered_config_ptr->rlc_ul_tm_parms.nchan);
        } 
        else
        {
        /* We found a logical channel ID for the given radio bearer, so
          make a note of that in the RLC info. */
          entity_idx = ordered_config_ptr->rlc_ul_tm_parms.nchan;
          ordered_config_ptr->rlc_ul_tm_parms.chan_info[entity_idx].lc_id =
            lc_id;
        }
        
        /* Make sure UE has not exceeded the maximum number of UL TM Log CH's */
        if (ordered_config_ptr->rlc_ul_tm_parms.nchan > UE_MAX_UL_TM_CHANNEL)
        {
          WRRC_MSG1_ERROR("Too Many UL TM Log Ch %d", ordered_config_ptr->rlc_ul_tm_parms.nchan);
          return FAILURE;
        }
      }
      break;
      
    case T_rrc_UL_RLC_Mode_spare:
    default:
      WRRC_MSG1_ERROR("Unknown UL RLC mode %d", rrc_ul_RLC_Mode->t);
      return FAILURE;
    } /* end UL RLC choice */

	return SUCCESS;
  }

/*==========================================================
FUNCTION  : get_dl_rlc_tm_info()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_rlc_tm_info
 (
  boolean     rrc_segmentationIndication,
  rrc_RB_Identity    rb_id,
  boolean       allow_new_channels
 )
{
  uint8 entity_idx;
  rlc_lc_id_type lc_id;
  uecomdef_status_e_type ret_var = SUCCESS;

  MSG_MED("Get DL TM RLC Info for RB %d", rb_id,0,0);
  
  lc_id = rrclcm_get_dl_rlc_lc_id(rb_id);
  entity_idx = get_dl_tm_index(lc_id);
  
  #ifdef FEATURE_RRC_TM_RLC_INFO
  if (entity_idx == UE_MAX_DL_TM_CHANNEL && !allow_new_channels)
  {
    ret_var = FAILURE;
  }
  #endif /* FEATURE_RRC_TM_RLC_INFO */
  if(entity_idx > UE_MAX_DL_TM_CHANNEL)
  {
    ret_var = FAILURE;
  }

  if(ret_var == FAILURE)
  {
    WRRC_MSG2_ERROR("Not allowed to add new DL TM RLC entity/Invalid entity_idx %d allow_new_channels %d",
          entity_idx,
          allow_new_channels);
    return FAILURE;
  }
  /*
  * Indicate if segmentation is performed
  * Segmentation is currently not supported in RLC
  */
  if (entity_idx == UE_MAX_DL_TM_CHANNEL)
  {
    
#ifdef FEATURE_RRC_TM_RLC_INFO
    ordered_config_ptr->rlc_dl_tm_parms.chan_info[ordered_config_ptr->rlc_dl_tm_parms.nchan].seg_ind = 
     rrc_segmentationIndication;
#endif /* FEATURE_RRC_TM_RLC_INFO */
    
    if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      /* There was no logical channel ID for the given radio bearer,
       so we need to add a new RLC channel. */
       ordered_config_ptr->rlc_dl_tm_parms.nchan++;
    } 
    else
    {
     /* We found a logical channel ID for the given radio bearer, so
      make a note of that in the RLC info. */
      entity_idx = ordered_config_ptr->rlc_dl_tm_parms.nchan;
      ordered_config_ptr->rlc_dl_tm_parms.chan_info[entity_idx].lc_id =
        lc_id;
    }
    
    /* Make sure UE has not exceeded the maximum number of DL TM Log CH's */
    if (ordered_config_ptr->rlc_dl_tm_parms.nchan > UE_MAX_DL_TM_CHANNEL)
    {
      WRRC_MSG1_ERROR("Too Many DL TM Log Ch %d", ordered_config_ptr->rlc_dl_tm_parms.nchan);
      return FAILURE;
    }
  } 
  else
  {
    ordered_config_ptr->rlc_dl_tm_parms.chan_info[entity_idx].seg_ind = rrc_segmentationIndication;
  }

  return SUCCESS;
}

/*==========================================================
FUNCTION  : get_dl_rlc_um_info_r6()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_rlc_um_info_r6
 (
    rrc_DL_UM_RLC_Mode_r6 * rrc_dl_UM_RLC_Mode,
  rrc_RB_Identity    rb_id,
  boolean       allow_new_channels,
  boolean       alt_e_bit_present

)
{
  uint8 entity_idx;
 
  if(get_rlc_dl_um_info(rb_id, allow_new_channels, ordered_config_ptr) == FAILURE)
  {
    return FAILURE;
  }
        
  if (rrc_set_dl_um_li(rb_id, 
              rrc_dl_UM_RLC_Mode->dl_UM_RLC_LI_size) == FAILURE)
  {
    return FAILURE;
  }

  entity_idx = get_dl_um_index(rb_id, ordered_config_ptr);
  if(entity_idx == UE_MAX_DL_UM_CHANNEL)
  {
    WRRC_MSG0_ERROR("The corresponding DL UM info not present");
    return FAILURE;
  }
  ordered_config_ptr->rlc_dl_um_parms.chan_info[entity_idx].alternate_e_bit = alt_e_bit_present;

  if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(rrc_dl_UM_RLC_Mode,dl_Reception_Window_Size))
  {
    WRRC_MSG0_ERROR("RRCHS+: R6-ASN1 DL UM reception window size not yet supported");
  }

  return SUCCESS;
}

/*==========================================================
FUNCTION  : rrcllc_update_machs_ota()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type rrcllc_update_machs_ota
(
rrc_LogicalChannelIdentity logicalChannelIdentity,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode,
  uint32         dflow_id
)
{
  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  
  uint8 mac_dflow_index;
  uint8 cnt;
  
    mac_dflow_index = rrcllc_find_dflow_index((uint8)dflow_id);
    /* If DFLOW Index does not exist, then create a new one */
    if (mac_dflow_index == UE_MAX_MAC_D_FLOW)
    {
      /* Create a new one */
      mac_dflow_index = rrcllc_allocate_dflow_index();
      if(mac_dflow_index == UE_MAX_MAC_D_FLOW)
      {
        /* Failure to allocate a new DFLOW Index */
        WRRC_MSG0_ERROR("RRCHS:DFLOW Index not available");
        return FAILURE;
      }
      else  /* A valid new DFLOW index has been allocated */
      {
        /* Now Save DFLOW ID in MAC structure */
        ordered_config_ptr->mac_dflow_info[mac_dflow_index].mac_dflow_id = (uint8) dflow_id;
        
        /* Increment no of DFLOWs */
        ordered_config_ptr->mac_dl_parms.num_dflow++;
        
      } /* A valid new DFLOW index allocated */
    }
    
    /* Get the Logical Channel Index */
    logch_idx = ordered_config_ptr->mac_dflow_info[mac_dflow_index].ndlchan;
  
  
    for (cnt = 0; cnt < ordered_config_ptr->mac_dflow_info[mac_dflow_index].ndlchan; cnt++) 
    {
      if (rb_id == ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[cnt].rb_id)
      {  
        return (SUCCESS);     
      } 
    } 
  
    /* If Logical Channel ID is present, save MAC Logical Channel Identity */

   ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[logch_idx].mac_id =
        (uint8)logicalChannelIdentity;
   
    /* Save Radio Bearer ID */
    ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[logch_idx].rb_id = rb_id;
    
    /* Determine the type of Logical Channel mapped to DCH TrCH */
    if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
    } 
    else
    {
      ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
    }
    
    if (logch_mode == UE_MODE_MAX_NUM)
    {
    /* This is the case when RB mapping info is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
      */
      rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
      if (rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
        return(FAILURE);
      }
      
      if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
      {
        WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
        return(FAILURE);
      }
      /* Also update the Logical Channel ID here */
      ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
    }
    
    /* Save the logical channel mode, if a setup */
    ordered_config_ptr->mac_dflow_info[mac_dflow_index].dlchan_info[logch_idx].rlc_mode = logch_mode;
    
    /* Increment number of logical channels */
    ordered_config_ptr->mac_dflow_info[mac_dflow_index].ndlchan++;
    
    WRRC_MSG2_HIGH("RRCHS:DFLOW %d, No of dedicated Ch %d",
      ordered_config_ptr->mac_dflow_info[mac_dflow_index].mac_dflow_id,
      ordered_config_ptr->mac_dflow_info[mac_dflow_index].ndlchan);
    rrcllc_free_hanging_rb_mapping_info_index(rb_id,MAC_D_RB_MAPPING);

	return SUCCESS;
  }



/*==========================================================
FUNCTION  : get_rlc_info ()

DESCRIPTION : 
  Processes RLC Info IE

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_rlc_info
(
  rrc_RLC_Info *ie_ptr,
  rrc_RB_Identity    rb_id,
  boolean       allow_new_channels
)
{

  uint8 entity_idx;
  boolean new_rlc_am_entity = FALSE;

  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info,ul_RLC_Mode))
  {
    if(get_ul_rlc_info(&ie_ptr->ul_RLC_Mode,rb_id,allow_new_channels,&new_rlc_am_entity) == FAILURE)
    {
     return FAILURE;
    }

    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode )
    {
      entity_idx = get_ul_um_index(rb_id);

      WRRC_MSG0_HIGH("IE altE-bitInterpretation is not present");
      if(entity_idx < UE_MAX_UL_UM_CHANNEL)
      {
        ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].alternate_e_bit = FALSE;
      }
    }

    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode )
    {
      entity_idx = get_am_index_wo_f3(rb_id);

      if(entity_idx == UE_MAX_AM_ENTITY)
      {
        WRRC_MSG0_ERROR("The corresponding UL AM info not present");
        return FAILURE;
      }
      
      WRRC_MSG0_HIGH("R3 does not support, Special HE field,set to FALSE");        
      ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config = FALSE;
    }
  } /* end if UL RLC present */

  /* Downlink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_dl_AM_RLC_Mode:
        MSG_MED("Get DL AM RLC Info for RB %d", rb_id,0,0);

        entity_idx = get_am_index_wo_f3(rb_id);

        /* LLC processes the UL AM info first, it should find the corresponding
           entity index for DL AM info to be filled in.
         */  
        if(entity_idx == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG0_ERROR("The corresponding UL AM info not present");
          return FAILURE;
        }

        if(get_dl_am_rlc_info(ie_ptr->dl_RLC_Mode.u.dl_AM_RLC_Mode, entity_idx) == FAILURE)
        {
          return FAILURE;
        }

        /* This flag is set when processing UL AM info. */
        if(new_rlc_am_entity)
        {
          /* Indicate that the AM entity has been filled up. LLC does not increase this
             number of entities when process UL AM info.
           */
          ordered_config_ptr->rlc_am_parms.nentity++;

          /* Make sure UE has not exceeded the maximum number of AM Entities */
          if(ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY)
          {
            WRRC_MSG1_ERROR("Too Many AM Entities %d", ordered_config_ptr->rlc_am_parms.nentity);
            return FAILURE;
          }
          
          /* It always assume to be the shared channel (Control&Data) 
             as we support now.
           */  
          ordered_config_ptr->rlc_am_parms.am_info[entity_idx]->dl_nchan = 1;
          
        }

        ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.special_value_he_config = FALSE;
        break;  /* DL AM */
 
      case T_rrc_DL_RLC_Mode_dl_UM_RLC_Mode:
          
        if(get_rlc_dl_um_info(rb_id, allow_new_channels, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }

        entity_idx = get_dl_um_index(rb_id, ordered_config_ptr);
    
        if(entity_idx < UE_MAX_DL_UM_CHANNEL)
        {
          ordered_config_ptr->rlc_dl_um_parms.chan_info[entity_idx].alternate_e_bit = FALSE;
        }
 
        break;

      case T_rrc_DL_RLC_Mode_dl_TM_RLC_Mode:
        if(get_dl_rlc_tm_info(ie_ptr->dl_RLC_Mode.u.dl_TM_RLC_Mode->segmentationIndication,rb_id,allow_new_channels)
             == FAILURE)
        {
          return FAILURE;
        }
        break;

      default:
        WRRC_MSG1_ERROR("Unknown DL RLC mode %d", ie_ptr->dl_RLC_Mode.t);
        return FAILURE;
    } /* end DL RLC choice */
  } /* end if DL RLC present */

  /* Got here, everything is a go */
  return SUCCESS;

} /* end get_rlc_info() */

/*====================================================================
FUNCTION: rrcllc_get_rlc_size_change_status()

DESCRIPTION:
  This function gets the RLC Size Change Status from Ordered Config

DEPENDENCIES:
  None

RETURN VALUE:
  RLC Size

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_get_rlc_size_change_status(void)
{
  uint8 i;
  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    if ((ordered_config_ptr->rlc_am_parms.am_info[i]->ul_nchan ||
         ordered_config_ptr->rlc_am_parms.am_info[i]->dl_nchan)
      && ((ordered_config_ptr->rlc_am_parms.am_info[i]->reestab_entity == 
      RLC_RE_ESTABLISH_DL) ||
      (ordered_config_ptr->rlc_am_parms.am_info[i]->reestab_entity 
      == RLC_RE_ESTABLISH_DL_UL)||
      (ordered_config_ptr->rlc_am_parms.am_info[i]->reestab_entity == 
       RLC_RE_ESTABLISH_UL)))
    {
      return TRUE;
    }
  }
  return FALSE;  
}

#ifdef FEATURE_MAC_I
/*==========================================================
FUNCTION  : get_am_index_in_config_ptr()

DESCRIPTION : Returns the index of the am_config info
              associated with the given RB id, or
              UE_MAX_AM_ENTITY if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_am_index_in_config_ptr
(
  rrc_RB_Identity rb_id,
  ordered_config_type *config_ptr
)
{
  uint8 i;
  uint8 result = UE_MAX_AM_ENTITY;
  
  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    /* This function will be called by get_rlc_info() when fills in RLC UL info and
       RLC DL info. Since RRC fills in UL first, the ul_nchan will be non-zero when
       fills RLC DL info and calls this function. 
       Otherwise, for an empty am_info block, the ul_nchan and dl_nchan should be zero.
     */
    if((config_ptr->rlc_am_parms.am_info[i]->common_config.rb_id == rb_id) &&
       (config_ptr->rlc_am_parms.am_info[i]->ul_nchan ||
          config_ptr->rlc_am_parms.am_info[i]->dl_nchan)
       )
    {
      result = i;
      WRRC_MSG2_MED("Found RLC AM for RB %d, idx %d", rb_id, i);
      break;
    }
  }

  return result;
}
/*==========================================================
FUNCTION  : get_am_index_in_config_ptr_wo_f3()

DESCRIPTION : Returns the index of the am_config info
              associated with the given RB id, or
              UE_MAX_AM_ENTITY if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
uint8 get_am_index_in_config_ptr_wo_f3
(
  rrc_RB_Identity rb_id,
  ordered_config_type *config_ptr
)
{
  uint8 i;
  uint8 result = UE_MAX_AM_ENTITY;
  
  for(i = 0; i < UE_MAX_AM_ENTITY; i++)
  {
    /* This function will be called by get_rlc_info() when fills in RLC UL info and
       RLC DL info. Since RRC fills in UL first, the ul_nchan will be non-zero when
       fills RLC DL info and calls this function. 
       Otherwise, for an empty am_info block, the ul_nchan and dl_nchan should be zero.
     */
    if((config_ptr->rlc_am_parms.am_info[i]->common_config.rb_id == rb_id) &&
       (config_ptr->rlc_am_parms.am_info[i]->ul_nchan ||
          config_ptr->rlc_am_parms.am_info[i]->dl_nchan)
       )
    {
      result = i;
      break;
    }
  }

  return result;
}
#endif /*FEATURE_MAC_I*/
/*==========================================================
FUNCTION  : get_rlc_info_r5 ()

DESCRIPTION : 
  Processes RLC Info IE

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_rlc_info_r5
(
rrc_RLC_Info_r5 *ie_ptr,
rrc_RB_Identity    rb_id,
boolean       allow_new_channels
)
{
  uint8 entity_idx;
  boolean new_rlc_am_entity = FALSE;
  rlc_reestablish_e_type reestablish_entity = RLC_RE_ESTABLISH_NONE;
  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r5,ul_RLC_Mode))
  {
    if(get_ul_rlc_info(&ie_ptr->ul_RLC_Mode,rb_id,allow_new_channels,&new_rlc_am_entity) == FAILURE)
    {
        return FAILURE;
    }
    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode )
    {
      entity_idx = get_ul_um_index(rb_id);

      if(entity_idx < UE_MAX_UL_UM_CHANNEL)
      {
        ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].alternate_e_bit = FALSE;
      }
    }
    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode)
    {
      entity_idx = get_am_index_wo_f3(rb_id);
      if(entity_idx == UE_MAX_AM_ENTITY)
      {
        WRRC_MSG0_ERROR("The corresponding UL AM info not present");
        return FAILURE;
      }

      WRRC_MSG0_HIGH("R5 does not support, Special HE field,set to FALSE.");        
      ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config = FALSE;
    }
  } /* end if UL RLC present */
  
  /* Downlink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r5,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_r5_dl_AM_RLC_Mode: /* rrc_dl_AM_RLC_Mode_r5_chosen */
        WRRC_MSG1_HIGH("RRCHS:Get DL AM RLC Info for RB %d", rb_id);
        
        entity_idx = get_am_index_wo_f3(rb_id);
        
        /* LLC processes the UL AM info first, it should find the corresponding
        entity index for DL AM info to be filled in. */
        if (entity_idx == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG0_ERROR("Not allowed to add new DL AM RLC entity");
          return FAILURE;
        }
        if (ie_ptr->rlc_OneSidedReEst == TRUE)
        {
          reestablish_entity = RLC_RE_ESTABLISH_DL;
        }
        else
        {
          reestablish_entity = RLC_RE_ESTABLISH_DL_UL;
        }
        WRRC_MSG2_MED("One-Sided Re-est from OTA %d,so set downlink local_reestablish_entity to [DL_UL/DL/UL/None]%d",ie_ptr->rlc_OneSidedReEst,reestablish_entity);

        if (get_dl_am_rlc_info_r5(ie_ptr->dl_RLC_Mode.u.dl_AM_RLC_Mode, 
              entity_idx, rb_id, allow_new_channels, reestablish_entity) == FAILURE)
        {
          return FAILURE;
        }
        
        /* This flag is set when processing UL AM info. */
        if(new_rlc_am_entity)
        {
          /* Indicate that the AM entity has been filled up. LLC does not increase this
          number of entities when process UL AM info.
          */
          ordered_config_ptr->rlc_am_parms.nentity++;
          
          /* Make sure UE has not exceeded the maximum number of AM Entities */
          if(ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY)
          {
            WRRC_MSG1_ERROR("Too Many AM Entities %d", ordered_config_ptr->rlc_am_parms.nentity);
            return FAILURE;
          }
          
          /* It always assume to be the shared channel (Control&Data) 
          as we support now.
          */  
          ordered_config_ptr->rlc_am_parms.am_info[entity_idx]->dl_nchan = 1;
          
        }
      
        ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.special_value_he_config = FALSE;
      
      break;  /* DL AM */
      
    case T_rrc_DL_RLC_Mode_r5_dl_UM_RLC_Mode: /* rrc_dl_UM_RLC_r5_Mode_chosen NULL */

      if(get_rlc_dl_um_info(rb_id, allow_new_channels, ordered_config_ptr) == FAILURE)
      {
        return FAILURE;
      }

      if (rrc_set_dl_um_li(rb_id, 
                           ie_ptr->dl_RLC_Mode.u.dl_UM_RLC_Mode->dl_UM_RLC_LI_size) == FAILURE)
      {
        WRRC_MSG2_ERROR("RRC:Unable to set LI %d for RB %d",
            ie_ptr->dl_RLC_Mode.u.dl_UM_RLC_Mode->dl_UM_RLC_LI_size,rb_id);
        return FAILURE;
      }

      entity_idx = get_dl_um_index(rb_id, ordered_config_ptr);   

      if(entity_idx < UE_MAX_DL_UM_CHANNEL)
      {
        ordered_config_ptr->rlc_dl_um_parms.chan_info[entity_idx].alternate_e_bit = FALSE;
      }
      break;
      
    case T_rrc_DL_RLC_Mode_r5_dl_TM_RLC_Mode:
      if(get_dl_rlc_tm_info(ie_ptr->dl_RLC_Mode.u.dl_TM_RLC_Mode->segmentationIndication,rb_id,allow_new_channels)
        == FAILURE)
      {
        return FAILURE;
      }
      break;
      
    default:
      WRRC_MSG1_ERROR("Unknown DL RLC mode %d", ie_ptr->dl_RLC_Mode.t);
      return FAILURE;
    } /* end DL RLC choice */
  } /* end if DL RLC present */
  
  /* Got here, everything is a go */
  return SUCCESS;

} /* end get_rlc_info_r5() */

/*==========================================================
FUNCTION  : get_rlc_mode()

DESCRIPTION : Gets the RLC mode for a given logical channel.

DEPENDENCIES: None.

RETURN VALUE: uecomdef_logch_mode_e_type indicate whether or 
              not it is TM, UM or AM.

SIDE EFFECTS: None.
============================================================*/
static uecomdef_logch_mode_e_type get_rlc_mode
(
  rrc_RLC_Info *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  uecomdef_logch_mode_e_type dl_logch_mode = UE_MODE_MAX_NUM;

  /* Uplink RLC info */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info,ul_RLC_Mode))
  {
    switch (ie_ptr->ul_RLC_Mode.t)
    {
      case T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode:
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;

      case T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode:
        logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;

      case T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode: 
        logch_mode = UE_MODE_TRANSPARENT;
        break;

      case T_rrc_UL_RLC_Mode_spare:
      default:
        logch_mode = UE_MODE_MAX_NUM;
        break;
    } /* end UL RLC choice */
  } /* end if UL RLC present */

  /* Downlink RLC info */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_dl_AM_RLC_Mode:
        dl_logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;

      case T_rrc_DL_RLC_Mode_dl_UM_RLC_Mode:
        dl_logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;

      case T_rrc_DL_RLC_Mode_dl_TM_RLC_Mode: 
        dl_logch_mode = UE_MODE_TRANSPARENT;
        break;

      default:
        dl_logch_mode = UE_MODE_MAX_NUM;
        break;
    } /* end DL RLC choice */

    /* If both DL and UL mode is present, check to make sure they are the same */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info,ul_RLC_Mode))
    {
      if (logch_mode != dl_logch_mode) 
      {
        WRRC_MSG2_ERROR("UL %d and DL %d lc mode not a match.  Invalid config",logch_mode,dl_logch_mode);
        logch_mode = UE_MODE_MAX_NUM;
      }
    }
    else
    {
      logch_mode = dl_logch_mode;
    }
  } /* end if DL RLC present */

  /* Need to know RLC type in either UL or DL */
  if (!((ie_ptr->m.ul_RLC_ModePresent) ||
                            (ie_ptr->m.dl_RLC_ModePresent)))
  {
    WRRC_MSG0_ERROR("Neither DL nor UL RLC mode present!");
  }

  return logch_mode;

} /* end get_rlc_mode */

/*====================================================================
FUNCTION: rrcllc_rearrange_macehs_lc_index()

DESCRIPTION:
  This function deletes any holes
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

static void rrcllc_rearrange_macehs_lc_index
(
  ordered_config_type   *config_ptr
)
{
  uint8  ix, iy;

  if (config_ptr->mac_dl_parms.ndlchan_macehs == 0)
  {
    return;
  }
 /* Put all the values in tmp structure. No there are no holes in tmp structure */
  for(ix = UE_MAX_DL_LOGICAL_CHANNEL; ix > 0; ix--)
  {
    /* Check if valid data */
    if(config_ptr->dl_macehs_logchan_info[ix-1].queue_id!= INVALID_QUEUE_ID)
    {
      for(iy = 0; iy < (ix - 1); iy++)
      {
        /* Check if valid data */
        if(config_ptr->dl_macehs_logchan_info[iy].queue_id== INVALID_QUEUE_ID)
        {
          config_ptr->mac_ehs_lc_id_in_use[iy] =  
                    config_ptr->mac_ehs_lc_id_in_use[ix-1];
          WCDMA_MEMCPY(&config_ptr->dl_macehs_logchan_info[iy], 
                       sizeof(config_ptr->dl_macehs_logchan_info[iy]),
                       &config_ptr->dl_macehs_logchan_info[ix-1], 
                       sizeof(config_ptr->dl_macehs_logchan_info[iy]));
          config_ptr->dl_macehs_logchan_info[ix-1].queue_id= INVALID_QUEUE_ID;
          config_ptr->dl_macehs_logchan_info[ix-1].rb_id = RRC_INVALID_RB_ID;
          config_ptr->dl_macehs_logchan_info[ix-1].rlc_id = 0;
          config_ptr->dl_macehs_logchan_info[ix-1].mac_id = INVALID_LOGICAL_CHANNEL_ID;
          config_ptr->dl_macehs_logchan_info[ix-1].chan_type = UE_LOGCHAN_NONE;
          config_ptr->dl_macehs_logchan_info[ix-1].rlc_mode = UE_MODE_MAX_NUM;
          config_ptr->mac_ehs_lc_id_in_use[ix-1] = FALSE;  
          break;
        }
      }
      if (iy == (ix-1)) 
      {
        MSG_MED("MACEHS: Rearranging MAC EHS Q complete", 0,0,0);
        break;
      }
    }
  } /* End of for (...) */

}
  /*==========================================================
FUNCTION  : rrcllc_process_flexible_pdu_dl_am()

DESCRIPTION :

DEPENDENCIES: None.

RETURN VALUE: 

SIDE EFFECTS: None.
============================================================*/
static void rrcllc_process_flexible_pdu_dl_am
(
   rlc_li_e_type  rrc_dl_rlc_pdu,
   uint8               entity_idx,
   boolean       allow_new_channels,
rlc_reestablish_e_type reestablish_entity
)
{

  ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile = TRUE;
  ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_li_type = rrc_dl_rlc_pdu;

  ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size = RRCLLC_INVALID_RLC_SIZE;
  
  if(allow_new_channels == FALSE )
  {
    if(current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_pdu_size_flexbile == FALSE ) 
    {
       if(rrc_dl_rlc_pdu == LI_15_BITS )
       {
         ordered_config_ptr->am_config[entity_idx].reestab_entity = reestablish_entity;
         WRRC_MSG2_HIGH("MACEHS: Re-establish dl am enity %d as PDU type changed from fixed to flexible for rb-id %d with LI 15 bits",
          entity_idx,ordered_config_ptr->am_config[entity_idx].common_config.rb_id);
       }
       else
       {
         WRRC_MSG2_HIGH("MACEHS: MACEHS: DL AM am enity modified %d, PDU type changed from fixed to flexible for rb-id %d with LI 7 bits",
			           entity_idx,ordered_config_ptr->am_config[entity_idx].common_config.rb_id);
          ordered_config_ptr->am_config[entity_idx].reestab_entity = RLC_RE_ESTABLISH_NONE;
       }
    }
    else if(((current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_li_type == LI_7_BITS)
           && (rrc_dl_rlc_pdu == LI_15_BITS )) )
    {
      ordered_config_ptr->am_config[entity_idx].reestab_entity = reestablish_entity;
      WRRC_MSG2_HIGH("MACEHS: Re-establish dl am enity %d as LI size changes from 7 bits to 15 bitsfor rb-id %d ",
            entity_idx,ordered_config_ptr->am_config[entity_idx].common_config.rb_id);
    }
    else if(((current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_li_type == LI_15_BITS)
           && rrc_dl_rlc_pdu == LI_7_BITS ) )
    {
       ordered_config_ptr->am_config[entity_idx].reestab_entity = reestablish_entity;
       WRRC_MSG2_HIGH("MACEHS: Re-establish dl am enity %d as LI sizes changes from 15 bits to 7 bitsfor rb-id %d ",
        entity_idx,ordered_config_ptr->am_config[entity_idx].common_config.rb_id);
    }
    else
    {
        WRRC_MSG2_HIGH("MACEHS: L1 size indicated: Old Size %d, New Size %d",
            current_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_li_type,
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.am_li_type);
        ordered_config_ptr->am_config[entity_idx].reestab_entity = RLC_RE_ESTABLISH_NONE;
    }
  }
}



/*==========================================================
FUNCTION  : get_rlc_mode_r5()

DESCRIPTION : Gets the RLC mode for a given logical channel.

DEPENDENCIES: None.

RETURN VALUE: uecomdef_logch_mode_e_type indicate whether or 
              not it is TM, UM or AM.

SIDE EFFECTS: None.
============================================================*/
static uecomdef_logch_mode_e_type get_rlc_mode_r5
(
rrc_RLC_Info_r5 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  uecomdef_logch_mode_e_type dl_logch_mode = UE_MODE_MAX_NUM;

  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r5,ul_RLC_Mode))
  {
    switch (ie_ptr->ul_RLC_Mode.t)
    {
      case T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode:
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;

      case T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode:
        logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;

      case T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode: 
        logch_mode = UE_MODE_TRANSPARENT;
        break;

      case T_rrc_UL_RLC_Mode_spare:
      default:
        logch_mode = UE_MODE_MAX_NUM;
        break;
    }/* end UL RLC choice */
  }/* end if UL RLC present */

  /* Downlink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r5,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_r5_dl_AM_RLC_Mode:
        dl_logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;

      case T_rrc_DL_RLC_Mode_r5_dl_UM_RLC_Mode:
        dl_logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;

      case T_rrc_DL_RLC_Mode_r5_dl_TM_RLC_Mode: 
        dl_logch_mode = UE_MODE_TRANSPARENT;
        break;

      default:
        dl_logch_mode = UE_MODE_MAX_NUM;
        break;
    } /* end DL RLC choice */

    /* If both DL and UL mode is present, check to make sure they are the same */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r5,ul_RLC_Mode))
    {
      if (logch_mode != dl_logch_mode) 
      {
        WRRC_MSG2_ERROR("UL %d and DL %d lc mode not a match.  Invalid config",logch_mode,dl_logch_mode);
        logch_mode = UE_MODE_MAX_NUM;
      }
    }
    else
    {
      logch_mode = dl_logch_mode;
    }
  }/* end if DL RLC present */

  /* Need to know RLC type in either UL or DL */
  if (!((ie_ptr->m.ul_RLC_ModePresent) ||
                            (ie_ptr->m.dl_RLC_ModePresent)))
  {
    WRRC_MSG0_ERROR("Neither DL nor UL RLC mode present!");
  }

  return logch_mode;

} /* end get_rlc_mode_r5 */



/*==========================================================
FUNCTION  : get_dl_dch_mapping_info_r5()

DESCRIPTION : Processes DL DCH Mapping IE contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_dch_mapping_info_r5
(
rrc_DL_LogicalChannelMapping_r5 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode
)
{

  
  /* Transport Channel ID */
  tr_ch_id_type   trch_id;
  /* Get the DL DCH TrCH ID if it exists */
  switch (ie_ptr->dl_TransportChannelType.t)
  {
  case T_rrc_DL_TransportChannelType_r5_dch:
    trch_id = (tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch;
    break;

  case T_rrc_DL_TransportChannelType_r5_dch_and_hsdsch:
    trch_id = (tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->dch_transport_ch_id;
    break;
    
  case T_rrc_DL_TransportChannelType_fach:
  case T_rrc_DL_TransportChannelType_dsch:
  default:
    WRRC_MSG0_ERROR("Expecting DL DCH TrCH ID");
    return FAILURE;
  } /* end DL DCH TrCH ID switch */
  

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_LogicalChannelMapping,logicalChannelIdentity))
  {
    return rrcllc_update_dl_dch_ota(rb_id,logch_mode,trch_id, ie_ptr->logicalChannelIdentity);
  }
  else
  {
    return rrcllc_update_dl_dch_ota(rb_id,logch_mode,trch_id, RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT);
  }
} /* end get_dl_dch_mapping_info_r5() */

/*==========================================================
FUNCTION  : get_hsdsch_mapping_info_r5()

DESCRIPTION : Processes HSDSCH Mapping IE contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_hsdsch_mapping_info_r5
(
rrc_DL_LogicalChannelMapping_r5 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode
)
{
  /* DFLOW   Id */
  uint32         dflow_id;
  switch (ie_ptr->dl_TransportChannelType.t)
  {
  case T_rrc_DL_TransportChannelType_r5_dch_and_hsdsch:
    MSG_MED("RRCHS:DFLOW Id is %d",
      ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->hsdsch_mac_d_flow_id,0,0); 

    dflow_id = ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->hsdsch_mac_d_flow_id;
    break;

  case T_rrc_DL_TransportChannelType_r5_hsdsch:
    MSG_MED("RRCHS:DFLOW Id is %d",
      ie_ptr->dl_TransportChannelType.u.hsdsch,0,0); 

    dflow_id = ie_ptr->dl_TransportChannelType.u.hsdsch;
    break;

  default:
    WRRC_MSG1_HIGH("RRCHS:Invalid  Channel Type %d",ie_ptr->dl_TransportChannelType.t);
    return FAILURE;
  }

 /* If Logical Channel ID is present, save MAC Logical Channel Identity */
   if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_LogicalChannelMapping_r5,logicalChannelIdentity))
   {
    return rrcllc_update_machs_ota(ie_ptr->logicalChannelIdentity,rb_id,logch_mode,dflow_id);
   }
   else 
   {
    return  rrcllc_update_machs_ota(RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT,rb_id,logch_mode,dflow_id);
   }
}

/*==========================================================
FUNCTION  : get_fach_mapping_info_r5()

DESCRIPTION : Processes FACH mapping IE contents and saves 
              it in MAC Ordered Configuration Database.
              If the next state is CELL_DCH, the data is 
              stored in local data structure in OC.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_fach_mapping_info_r5
(
rrc_DL_LogicalChannelMapping_r5 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode,
rrc_state_e_type             next_rrc_state
)
{
  /* transport Channel Index */
  uint8   trch_idx  = RRCLLC_INVALID_TRCH_IDX;

  /* Get the logical channel index */
  uint8   logch_idx, i;

  /* Temporary storage for TrCH ID */
  tr_ch_id_type  tmp_trch_id;

  mac_dl_ded_logch_config_type  *dlchan_info;
  rlc_lc_id_type    rlc_lc_id = (uint8)RRCLCM_RLC_LC_ID_NOT_FOUND;

  /* TBD: Although all existing FACHs would be mapped to each of the RBs,
   * fill up only for the 1st FACH - till MAC interface is clarified/updated
   */

  boolean update_fach_rlc_id = FALSE;

  uint8 cnt;

  /* Start with updating Cell FACH RB Mapping Info when the next state is
   * either CELL_DCH or CELL_FACH
   */
  logch_idx = ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan;
  dlchan_info = &ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx];

  if (logch_mode == UE_MODE_MAX_NUM)
  {
    /* This is the case when RB mapping info applicable to CELL_FACH is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
    */
    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if (rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
      return(FAILURE);
    }

    if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
    /* Also update the Logical Channel ID here */
    dlchan_info->rlc_id = rlc_lc_id;
    update_fach_rlc_id = TRUE;
  }

  for (cnt = 0; cnt < ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan ; cnt++) 
  {      
    if (rb_id == ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cnt].rb_id ) 
    {
      WRRC_MSG1_MED("RB_ID %d already present",rb_id);
      return (SUCCESS);        
    }
  }

  rrcllc_update_dl_ded_log_chl_info_r5(ie_ptr, dlchan_info, rb_id, logch_mode);
  ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan ++;
  

  /* Now update the FACH Info ONLY IF the next state is CELL_FACH */
  if (next_rrc_state == RRC_STATE_CELL_FACH ||
      next_rrc_state == RRC_STATE_CELL_PCH ||
      next_rrc_state == RRC_STATE_URA_PCH)
  {
    if ((trch_idx = rrcllc_find_dl_fach_trch_idx()) >= UE_MAX_TRCH)
    {
      return FAILURE;
    }
    logch_idx = ordered_config_ptr->fach_info[trch_idx].ndlchan;
    dlchan_info = &ordered_config_ptr->fach_info[trch_idx].dlchan_info[logch_idx];
    if (update_fach_rlc_id)
    {
      dlchan_info->rlc_id = rlc_lc_id;
    }
    rrcllc_update_dl_ded_log_chl_info_r5(ie_ptr, dlchan_info, rb_id, logch_mode);
    
    ordered_config_ptr->fach_info[trch_idx].ndlchan++;
    ordered_config_ptr->dl_fach_trch_info[trch_idx].lg_ch_cnt = 
      ordered_config_ptr->fach_info[trch_idx].ndlchan;

    /* Since each available FACH is mapped to all RBs, copy the FACH info
    *  from the 1st FACH to all FACHs. Copy everything except for the
    * Transport Channel ID.
    */
    for (i = 1; i < ordered_config_ptr->mac_dl_parms.num_fachs; i ++)
    {
      tmp_trch_id = ordered_config_ptr->mac_dl_parms.fach_info[i]->trch_id;
      
      *ordered_config_ptr->mac_dl_parms.fach_info[i] = 
      *ordered_config_ptr->mac_dl_parms.fach_info[0];

      ordered_config_ptr->mac_dl_parms.fach_info[i]->trch_id = tmp_trch_id;

      ordered_config_ptr->dl_fach_trch_info[i].lg_ch_cnt =
      ordered_config_ptr->dl_fach_trch_info[0].lg_ch_cnt;
    }
  } /* If next state is CELL_FACH, update the FACH Info */

  return SUCCESS;

} /* end get_fach_mapping_info_r5() */

/*==========================================================
FUNCTION  : get_dl_logical_channel_info_r5()

DESCRIPTION : Get DL logical Channel info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_logical_channel_info_r5
(
rrc_DL_LogicalChannelMappingList_r5 * ie_ptr,
rrc_RB_Identity                       rb_id,
uecomdef_logch_mode_e_type       logch_mode
)
{
  /*
  * Count to make sure that don't exceed number of Logical
  * channels that can be Mapped to a Transport Channel.
  */
  uint8 lc_tc_cnt = 0;
  
  ordered_config_type *tmp_config_ptr = NULL;

  
  /* Process each item of the list */
  while ((ie_ptr->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH))
  {
    /* Transport channel type */
    switch (ie_ptr->elem[lc_tc_cnt].dl_TransportChannelType.t)
    {

    case T_rrc_DL_TransportChannelType_r5_hsdsch:
      /* Check if logical channel is not TM */
      if (logch_mode == UE_MODE_TRANSPARENT)
      {
        WRRC_MSG0_ERROR("RRCHS:TM Log Channel mapped on HSDSCH");
        return FAILURE;
      }
      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
      {
        if (get_hsdsch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {
        if (transition_config.toc_usage == TOC_INVALID)
        {
          transition_config.toc_usage = TOC_FOR_DCH;
        }
        
        tmp_config_ptr = ordered_config_ptr;
        ordered_config_ptr = transition_config.toc_ptr;
        
        /* Processing functions must think we are going to Cell_DCH state. */
        ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
        
        if (get_hsdsch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;
      }
      break;
    case T_rrc_DL_TransportChannelType_r5_dch:
      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
      {
        if (get_dl_dch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {
        if (transition_config.toc_usage == TOC_INVALID)
        {
          transition_config.toc_usage = TOC_FOR_DCH;
        }
        
        tmp_config_ptr = ordered_config_ptr;
        ordered_config_ptr = transition_config.toc_ptr;
        
        /* Processing functions must think we are going to Cell_DCH state. */
        ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
        
        if (get_dl_dch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;
      }
      break;
      
    case T_rrc_DL_TransportChannelType_r5_fach:
      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_FACH ||
        ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_PCH ||
        ordered_config_ptr->rrc_state_indicator == RRC_STATE_URA_PCH)
      {
        if (get_fach_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode,
          ordered_config_ptr->rrc_state_indicator) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {
      /* FACH mapping info can come in DCH state. Accept and update
      * local mapping info structure.
        */
        if (get_fach_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode, RRC_STATE_CELL_DCH) == FAILURE)
        {
          return FAILURE;
        }
      }
      break;
      

      
    case T_rrc_DL_TransportChannelType_r5_dch_and_hsdsch:
      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
      {
        if (get_dl_dch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
        if (get_hsdsch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {
        if (transition_config.toc_usage == TOC_INVALID)
        {
          transition_config.toc_usage = TOC_FOR_DCH;
        }
        
        tmp_config_ptr = ordered_config_ptr;
        ordered_config_ptr = transition_config.toc_ptr;
        
        /* Processing functions must think we are going to Cell_DCH state. */
        ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
        
        if (get_dl_dch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        if (get_hsdsch_mapping_info_r5(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;
      }
      break;

      case T_rrc_DL_TransportChannelType_r5_dsch:
      case T_rrc_DL_TransportChannelType_r5_dch_and_dsch:
    default:

        WRRC_MSG0_ERROR("Recv'd Invalid TrCh type/DSCH TrCH not supported, return FAILURE");
      return FAILURE;

    } /* end Logical channel type switch */
    
    /* Get pointer to the next item in the list */
    lc_tc_cnt++;
  } /* end while ie_ptr != NULL */
  
  return SUCCESS;

} /* end get_dl_logical_channel_info_r5() */

/*==========================================================
FUNCTION  : get_rb_mapping_info_r5()

DESCRIPTION : Get Radio Bearer Mapping Info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rb_mapping_info_r5
(
rrc_RB_MappingInfo_r5      * ie_ptr,
rrc_RB_Identity                 rb_id,
uecomdef_logch_mode_e_type logch_mode
)
{
  /*
  * Count to make sure that don't exceed the maximum number 
  * of RB Mux options
  */
  uint8 rb_mux_cnt = 0;

  /* Process the RB Mux option IE in the linked list */
  while ((ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5))
  {
    /* Check for DL Channel Mapping List */
    if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption_r5,dl_LogicalChannelMappingList))
    {
      MSG_MED("Get RB %d Mapping Info for DL", rb_id,0,0);
      if (get_dl_logical_channel_info_r5(&ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList,
                                         rb_id, logch_mode) == FAILURE)
      {
        return FAILURE;
      }
      /* Here indicate that MAC Downlink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
    } /* end dl Log Ch mapping list */

    if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption_r5,ul_LogicalChannelMappings))
    {
      MSG_MED("Get RB %d Mapping Info for UL", rb_id,0,0);
      if (get_ul_logical_channel_info(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,
                                      rb_id, logch_mode) == FAILURE)
      {
        return FAILURE;
      }

      /* Here indicate that MAC Uplink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
    } /* end ul Log Ch mapping list */

    /* Pointer to the next item in the IE list */
    rb_mux_cnt++;

  } /* end loop of RB Mapping info list */
#ifdef FEATURE_WCDMA_HS_FACH
  rrcllc_backup_mapping_info_into_hanging(ie_ptr,REL_5,rb_id,logch_mode);
#endif

  return SUCCESS;

} /* end get_rb_mapping_info_r5() */

#ifdef FEATURE_UMTS_PDCP
/*==========================================================
FUNCTION  : init_rfc_3095_profiles()

DESCRIPTION : initializes the rohc profiles to uncompressed as 
              uncompressed should always be supported by the UE

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
static void init_rfc_3095_profiles(rohc_profiles_e_type *profile_list)
{
  uint32 index = 0;

  for (index = 0; index < RFC_3095_MAX_ROHC_PROFILES; index++) 
  {
    profile_list[index] = ROHC_PROFILE_UNCOMPRESSED;
  }
}

/*==========================================================
FUNCTION  : get_pdcp_info()

DESCRIPTION : Get PDCP info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_pdcp_info
(
rrc_PDCP_Info           *ie_ptr,
rrc_RB_Identity                 rb_id,
rrcllcoc_pdcp_action_etype pdcp_action
)
{
  uint32 idx = 0;      
  uint32 index = 0;

  rrc_HeaderCompressionInfoList *hdrCmprssnInfoList = {0};  

  /*Get the next free index in pdcp_config_req */
  if (pdcp_action == CPDCP_SETUP) 
  {
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use == FALSE) 
      {
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
        /* Get the RB_ID information */
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id = rb_id;
        ordered_config_ptr->pdcp_parms.num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
  }
  else if (pdcp_action == CPDCP_RECONFIG) 
  {
    while (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id != rb_id) 
    {
      idx++;
    }
  }

  WRRC_MSG1_HIGH("PDCP_RRC config req index is %d",idx);

  if (idx == MAX_RAB_TO_SETUP) 
  {
    WRRC_MSG0_ERROR("PDCP_RRC This shouldn't happen");
    return FAILURE;
  }

  /* save the PDCP action */
  WRRC_MSG2_HIGH("PDCP_RRC action for rb_id %d pdcp_action %d",rb_id,pdcp_action);
  if (pdcp_action == CPDCP_SETUP) 
  {
    ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
  }
  else if (pdcp_action == CPDCP_RECONFIG) 
  {
    /* initialize pdcp params as PDCP_INFO provides a snapshot */

    ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_info_changed = TRUE;
    memset(&(ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info), 0, sizeof(pdcp_info_type)); 

    ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = RECONFIG_PDCP;
  }
  else
  {
    WRRC_MSG0_ERROR("PDCP_RRC Shouldn't come here");
    return FAILURE;
  }

  ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.is_mcast_call = FALSE;
  
  /* IE support for SNRS relocation is manditory present when logch_mode is AM, 
   * in sequence delivery and no discard.
   */
  index = get_am_index_wo_f3(rb_id);
  if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info,losslessSRNS_RelocSupport)) )
  {
    if (index!= UE_MAX_AM_ENTITY && 
        ((ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_ul_am_config.rlc_info.discard_info.discard_mode == RLC_NO_DISCARD) 
         &&
         (ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_dl_am_config.rlc_info.in_sequence == TRUE)))
    {
      WRRC_MSG0_ERROR("PDCP_RRC SRNS reloc IE needs to be present");
      return FAILURE;
    }
  }
 
  ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_info_valid = TRUE;

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info,losslessSRNS_RelocSupport)) 
  {
    WRRC_MSG1_HIGH("PDCP IE SRNS reloc supported status %d",
      RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported));
    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)) 
    {
      ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.support_for_lossless = TRUE;
      switch(ie_ptr->losslessSRNS_RelocSupport.u.supported)
      {
        case rrc_MaxPDCP_SN_WindowSize_sn255:
          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_sn_win_size = SN_255;
          break;

        case rrc_MaxPDCP_SN_WindowSize_sn65535:
          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_sn_win_size = SN_65535;
          break;
      }
      WRRC_MSG1_HIGH("PDCP SN win size %d", ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].
               pdcp_cfg_req.pdcp_info.pdcp_sn_win_size);
    }
    else
    {
      ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.support_for_lossless = FALSE;
    }
  }

  /* Populate wheter PDCP PDU header is existent or not */
  if(ie_ptr->pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
  {
    ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_pdu_hdr = PRESENT;
  }
  else
  {
    ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_pdu_hdr = ABSENT;
  }
  WRRC_MSG1_HIGH("PDCP pdu header present:0/ absent:1 %d",
   ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_pdu_hdr);

  /* IF IPHC is configured on AM RLC , make sure in-sequence delivery and no-discard is configured.  
   * No need to have same check for UM as UM is always in sequence.
   */
   //?? why loop through??
  if (index!= UE_MAX_AM_ENTITY && 
      (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info,headerCompressionInfoList)))
  {
    uint32 ix =0;
    hdrCmprssnInfoList = &ie_ptr->headerCompressionInfoList;
    while (hdrCmprssnInfoList->n > ix) 
    {
      if ((ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_ul_am_config.rlc_info.discard_info.discard_mode != RLC_NO_DISCARD) 
          ||
          (ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_dl_am_config.rlc_info.in_sequence == FALSE)) 
      {
        WRRC_MSG2_ERROR("PDCP_RRC Bad IPHC config discard mode: %d in sequence %d ",
            ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_ul_am_config.rlc_info.discard_info.discard_mode,
            ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_dl_am_config.rlc_info.in_sequence);
        return FAILURE;
      }
      ix++;
    }
  }

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info,headerCompressionInfoList)) 
  {
    hdrCmprssnInfoList = &ie_ptr->headerCompressionInfoList;

    index = 0;
    while (hdrCmprssnInfoList->n > index && (index < MAX_COMP_ENG_TYPES))
    {
      /* Process RFC 2507 related info*/
      if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.t == 
          T_rrc_AlgorithmSpecificInfo_rfc2507_Info)
      {
        /* Indicates that RFC 2507 info is present */
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
          iphc_info.comp_eng_type = PDCP_IPHC;

        /* Largest number of compressed non-TCP headers that may be sent without sending a full header. */
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_f_max_period = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->f_MAX_PERIOD;
         /* default RFC_2507_F_MAX_PERIOD;*/
        
        /* Compressed headers may not be sent more than F_MAX_TIME seconds after sending last full header. */

          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_f_max_time = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->f_MAX_TIME;
        
        /* default RFC_2507_F_MAX_TIME;*/
        
        /* The largest header size in octets that may be compressed */
      
          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_max_header = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->max_HEADER;
         /* default RFC_2507_MAX_HEADER;*/

        /* Maximum CID value for TCP connections.*/
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_tcp_space = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->tcp_SPACE;
        
        /* default RFC_2507_TCP_SPACE;*/
        
       /* Maximum CID value for non-TCP connections. */
        
          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_non_tcp_space = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->non_TCP_SPACE;
        /* default  RFC_2507_NON_TCP_SPACE;*/
        
        /* Whether the algorithm shall reorder PDCP SDUs or not.*/
        if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->expectReordering == rrc_ExpectReordering_reorderingNotExpected) 
        {
          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.expect_reorder = REORDERING_NOT_EXPECTED;
        }
        else if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->expectReordering == rrc_ExpectReordering_reorderingExpected) 
        {
          ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.expect_reorder = REORDERING_EXPECTED;
        }
        MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RFC 2507 info present ind %d F_MAX_PERIOD %d F_MAX_TIME %d F_MAX_HEADER %d TCP_SPACE %d NON_TCP_SPACE %d EXPECTED_REORDERING %d",index,
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_f_max_period,
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_f_max_time,
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_max_header,
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_tcp_space,
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_non_tcp_space,
                 ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.expect_reorder);
      } /*if (hdrCmprssnInfoList->value.algorithmSpecificInfo.choice & rrc_AlgorithmSpecificInfo_r4_rfc2507_Info_chosen)*/
      else
      {
        WRRC_MSG0_ERROR("Should not come here");
        return FAILURE;
      }
      index++;
      
    } /* while (hdrCmprssnInfoList) */

    /* Call PDCP function to futher validate PDCP info */
    if (! pdcp_validate_params(ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info,FALSE))
    {
      WRRC_MSG0_ERROR("pdcp validation failed");
      return FAILURE;
    }
  }
  else
  {
    WRRC_MSG0_HIGH("PDCP header compression info list not present");
  }
  return SUCCESS;

} /* end get_pdcp_info() */


/*==========================================================
FUNCTION  : get_pdcp_info_r5()

DESCRIPTION : Get PDCP info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_pdcp_info_later_than_r3
(
rrcllcoc_pdcp_parms_type   *pdcp_parms_ptr,
rrc_PDCP_Info_r4           *ie_ptr,
rrc_RB_Identity                 rb_id,
rrcllcoc_pdcp_action_etype pdcp_action
)
{
  uint32 idx = 0;      
  uint32 index = 0;
  uint32 count = 0;

  rrc_HeaderCompressionInfoList_r4 *hdrCmprssnInfoList = {0};  
  rrc_ROHC_ProfileList_r4 *rohcProflList;

/*If RBE is setting up CS over HSPA RAB, then make sure to do the validation of following checks.*/
/*From 25.331 v.8.4, clause 8.6.4.10, the following IEs shouldn't be present for CS over HSPA RAB.*/
  /*Get the next free index in pdcp_config_req */
  if (pdcp_action == CPDCP_SETUP) 
  {
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (pdcp_parms_ptr->pdcp_config_req[idx].in_use == FALSE) 
      {
        pdcp_parms_ptr->pdcp_config_req[idx].in_use = TRUE;
        /* Get the RB_ID information */
        pdcp_parms_ptr->pdcp_config_req[idx].rb_id = rb_id;
        pdcp_parms_ptr->num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
  }
  else if (pdcp_action == CPDCP_RECONFIG) 
  {
    while ((pdcp_parms_ptr->pdcp_config_req[idx].rb_id != rb_id) && 
           (idx < pdcp_parms_ptr->num_rbs)) 
    {
      idx++;
    }
    
    if (idx == pdcp_parms_ptr->num_rbs) 
    {
      WRRC_MSG1_ERROR("PDCP info to reconfig not found %d: fail",rb_id);
      return FAILURE;
    }
  }

  WRRC_MSG1_HIGH("PDCP_RRC config req index is %d",idx);

  if (idx == MAX_RAB_TO_SETUP) 
  {
    WRRC_MSG0_ERROR("PDCP_RRC This shouldn't happen");
    return FAILURE;
  }

  /* save the PDCP action */
  WRRC_MSG2_HIGH("PDCP_RRC action for rb_id %d pdcp_action %d",rb_id,pdcp_action);

  if (pdcp_action == CPDCP_SETUP) 
  {
    pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
  }
  else if (pdcp_action == CPDCP_RECONFIG) 
  {
    /* initialize pdcp params as PDCP_INFO provides a snapshot */

    pdcp_parms_ptr->pdcp_config_req[idx].pdcp_info_changed = TRUE;
    memset(&(ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info), 0, sizeof(pdcp_info_type)); 
    pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.action = RECONFIG_PDCP;
  }
  else
  {
    WRRC_MSG0_ERROR("PDCP_RRC Shouldn't come here");
  }
  pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.is_mcast_call = FALSE;

  {
    /* If IE support for SNRS relocation is set, make sure that logch_mode is AM, 
     * in sequence delivery and no discard.
     */
    index = get_am_index_wo_f3(rb_id);
    if (!(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)))
    {
      if (index!= UE_MAX_AM_ENTITY && 
          ((ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_ul_am_config.rlc_info.discard_info.discard_mode == RLC_NO_DISCARD) 
           &&
           (ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_dl_am_config.rlc_info.in_sequence == TRUE)))
      {
        WRRC_MSG0_HIGH("PDCP_RRC SRNS reloc IE needs to be present");
        return FAILURE;
      }
    }
    /* IF ROCH is configured on AM RLC , make sure in-sequence delivery and no-discard is configured.  
     * No need to have same check for UM as UM is always in sequence.
     */
    if (index!= UE_MAX_AM_ENTITY && 
        (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info_r4,headerCompressionInfoList)))
    {
       
      {
        uint32 ix =0;
        hdrCmprssnInfoList = &ie_ptr->headerCompressionInfoList;
        while (hdrCmprssnInfoList->n > ix) 
        {
          if ((ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_ul_am_config.rlc_info.discard_info.discard_mode != RLC_NO_DISCARD) 
              ||
              (ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_dl_am_config.rlc_info.in_sequence == FALSE)) 
          {
            WRRC_MSG2_ERROR("PDCP_RRC Bad ROHC IPHC config discard mode: %d in sequence %d ",
                ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_ul_am_config.rlc_info.discard_info.discard_mode,
                ordered_config_ptr->rlc_am_parms.am_info[index]->rlc_dl_am_config.rlc_info.in_sequence);
            return FAILURE;
          }
          ix++;
        }
      }
    }
  }
 
  pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_info_valid = TRUE;

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) 
  {
    WRRC_MSG1_HIGH("PDCP IE SRNS reloc supported status %d",
      RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported));

    if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)) 
    {
      pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.support_for_lossless = TRUE;
      switch(ie_ptr->losslessSRNS_RelocSupport.u.supported)
      {
        case rrc_MaxPDCP_SN_WindowSize_sn255:
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_sn_win_size = SN_255;
          break;

        case rrc_MaxPDCP_SN_WindowSize_sn65535:
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_sn_win_size = SN_65535;
          break;
        default:
          WRRC_MSG0_ERROR("Unexpected SN win size value");
          break;
      }
      WRRC_MSG1_HIGH("PDCP SN win size %d", pdcp_parms_ptr->pdcp_config_req[idx].
               pdcp_cfg_req.pdcp_info.pdcp_sn_win_size);
    }
    else
    {
      pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.support_for_lossless = FALSE;
    }
  }
  /* Populate wheter PDCP PDU header is existent or not */
  if(ie_ptr->pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
  {
    pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_pdu_hdr = PRESENT;
  }
  else
  {
    pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_pdu_hdr = ABSENT;
  }
  WRRC_MSG1_HIGH("PDCP pdu header present:0/ absent:1 %d",
              pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.pdcp_pdu_hdr);

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_PDCP_Info_r4,headerCompressionInfoList)) 
  {
    hdrCmprssnInfoList = &ie_ptr->headerCompressionInfoList;

    index = 0;
    while (hdrCmprssnInfoList->n > index && (index < MAX_COMP_ENG_TYPES))
    {
      /* Process RFC 2507 related info*/
      if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.t == 
          T_rrc_AlgorithmSpecificInfo_r4_rfc2507_Info)
      {
        /* Indicates that RFC 2507 info is present */
        pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
          iphc_info.comp_eng_type = PDCP_IPHC;

        /* Largest number of compressed non-TCP headers that may be sent without sending a full header. */
    
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_f_max_period = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->f_MAX_PERIOD;
       /* defau;t  RFC_2507_F_MAX_PERIOD;*/
        
        /* Compressed headers may not be sent more than F_MAX_TIME seconds after sending last full header. */
      
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_f_max_time = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->f_MAX_TIME;
         /*  default       RFC_2507_F_MAX_TIME */

        /* The largest header size in octets that may be compressed */
      
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_max_header = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->max_HEADER;
       /* default   RFC_2507_MAX_HEADER;*/
       

        /* Maximum CID value for TCP connections.*/
    
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_tcp_space = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->tcp_SPACE;

        /* Maximum CID value for non-TCP connections. */
       
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.iphc_non_tcp_space = (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->non_TCP_SPACE;
        /*  default  RFC_2507_NON_TCP_SPACE;*/

        /* Whether the algorithm shall reorder PDCP SDUs or not.*/
        if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->expectReordering == rrc_ExpectReordering_reorderingNotExpected) 
        {
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.expect_reorder = REORDERING_NOT_EXPECTED;
        }
        else if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc2507_Info->expectReordering == rrc_ExpectReordering_reorderingExpected) 
        {
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            iphc_info.expect_reorder = REORDERING_EXPECTED;
        }

        MSG_7(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RFC 2507 info present ind %d F_MAX_PERIOD %d F_MAX_TIME %d F_MAX_HEADER %d TCP_SPACE %d NON_TCP_SPACE %d EXPECTED_REORDERING %d",index,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_f_max_period,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_f_max_time,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_max_header,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_tcp_space,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.iphc_non_tcp_space,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].iphc_info.expect_reorder);

      } /*if (hdrCmprssnInfoList->value.algorithmSpecificInfo.choice & rrc_AlgorithmSpecificInfo_r4_rfc2507_Info_chosen)*/

      else if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.t == T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info) 
      {
        WRRC_MSG1_HIGH("RFC 3095 info present ind %d",index);
        /* Indicates that RFC 3095 info is present */
        pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
          rohc_info.comp_eng_type = PDCP_ROHC;


        /* Profiles supported by both compressor and decompressor in both UE and UTRAN. Profile 0 shall always be supported. */
        init_rfc_3095_profiles(pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.
                               comp_eng_info[index].rohc_info.rohc_profiles);

        rohcProflList = &hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc3095_Info->rohcProfileList;
        count = 0;
        while((rohcProflList->n > count) && count < RFC_3095_MAX_ROHC_PROFILES)
        {
          if(rohcProflList->elem[count] == 1)
          {
            pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_profiles[count] = ROHC_PROFILE_RTP;
          }
          else if(rohcProflList->elem[count] == 2)
          {
            pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_profiles[count] = ROHC_PROFILE_UDP;
          }
          MSG_HIGH("RFC_3095 rohc profile count %d value %d = %d",count ,rohcProflList->elem[count],pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].rohc_info.rohc_profiles[count]);
          count++;
        } // while(rohcProflList)
        /* Always set the last profile to uncompressed as uncompressed should 
         * always be supported by the UE
         */
        pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
          rohc_info.rohc_profiles[count] = ROHC_PROFILE_UNCOMPRESSED;
        
        /* Process Uplink ROHC info if present*/
        if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc3095_Info->m.ul_RFC3095Present) 
        {
          WRRC_MSG0_HIGH("RFC 3095 UL info present");
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            rohc_info.rohc_ul_info_present = TRUE;

          /* Highest context ID number to be used by the UE compressor */

            pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_ul_max_cid =
               (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID;
          /* defau;t  RFC_3095_MAX_CID;*/
          

          /*  as per section 8.6.4.10, for RFC 3095:
              the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as
              indicated in the IE "PDCP Capability".
          */
          if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_ul_max_cid > 4) 
          {
            WRRC_MSG1_ERROR("Invalid config.  MAX_CID is > than advertized max num ROHC context sessions",
                ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
                rohc_info.rohc_ul_max_cid);
            return FAILURE;
          }

        }

        /* Process downlink ROHC info if present*/
        if (hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc3095_Info->m.dl_RFC3095Present) 
        {
          WRRC_MSG0_HIGH("RFC 3095 DL info present");
          pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
            rohc_info.rohc_dl_info_present = TRUE;

          /* Highest context ID number to be used by the UE compressor */

            pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_dl_max_cid =
               (uint16)hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID;
          /* defaULT  RFC_3095_MAX_CID;*/
          

          /*  as per section 8.6.4.10, for RFC 3095:
              the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as
              indicated in the IE "PDCP Capability".
          */
          if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_dl_max_cid > 4) 
          {
            WRRC_MSG1_ERROR("Invalid config.  MAX_CID is > than advertized max num ROHC context sessions",
                ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
                rohc_info.rohc_ul_max_cid);
            return FAILURE;
          }

          /* Determines whether reverse decompression should be used or not and the maximum number of packets that can be reverse
             decompressed by the UE decompressor.*/
       
            pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].
              rohc_info.rohc_dl_rev_decomp_depth = 
              hdrCmprssnInfoList->elem[index].algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.reverseDecompressionDepth;
         /*  DEFAULT RFC_3095_REVERSE_DECOMPRESSION_DEPTH;*/
          
        }


        MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RFC_3095 rohc profile count %d to uncompressed,RFC 3095 ul max_cid %d,RFC 3095 dl max_cidk, RFC 3095 reverse decompression depth %d",
                 count,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].rohc_info.rohc_ul_max_cid,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].rohc_info.rohc_dl_max_cid,
                 pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info.comp_eng_info[index].rohc_info.rohc_dl_rev_decomp_depth);

      } /*else if (hdrCmprssnInfoList->value.algorithmSpecificInfo.choice & rrc_rfc3095_Info_chosen) */
      else
      {
        WRRC_MSG0_ERROR("Should not come here");
      }
      index++;
     
    }/* while (hdrCmprssnInfoList) */

    /* Call PDCP function to futher validate PDCP info */
    if (!pdcp_validate_params(pdcp_parms_ptr->pdcp_config_req[idx].pdcp_cfg_req.pdcp_info,FALSE))
    {
      WRRC_MSG0_ERROR("pdcp validation failed");
      return FAILURE;
    }
  }
  else
  {
    WRRC_MSG0_HIGH("PDCP header compression info list not present");
  }

  return SUCCESS;

} /* end get_pdcp_info_later_than_r3() */

#endif /* FEATURE_UMTS_PDCP */

/*==========================================================
FUNCTION  : get_rb_info_to_setup_r5()

DESCRIPTION : Get Radio Bearer Information to setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_setup_r5
(
rrc_RB_InformationSetup_r5       *ie_ptr,
#ifdef FEATURE_UMTS_PDCP
rrc_CN_DomainIdentity cn_Domain,
#endif /* FEATURE_UMTS_PDCP*/
rrc_RB_InformationSetupList_r5   * rb_setup_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type    logch_mode = UE_MODE_MAX_NUM;
  
  /*initialize a counter to check rb count*/
  uint8                         rb_cnt  = 0;
  
#ifdef FEATURE_UMTS_PDCP
  uint8 idx = 0;
#endif /* FEATURE_UMTS_PDCP */
 
 
  
  /*place holder for rb_id we get originally*/
  rrc_RB_Identity  local_rb_id = ie_ptr->rb_Identity;
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_RB_InformationSetup_r5     *temp_ie_ptr = ie_ptr;

  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the rb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r5_same_as_RB))
  {
    while ((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
    {
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == 
        rb_setup_ptr->elem[rb_cnt].rb_Identity)
      {
        temp_ie_ptr = &(rb_setup_ptr->elem[rb_cnt]);
        break;
      }
      
      rb_cnt++;
    } /* end RB per RAB setup */
    
    if ((rb_setup_ptr->n == rb_cnt) || (rb_cnt >= MAX_RB_PER_RAB))
    {
       WRRC_MSG0_ERROR("Unable to match rb_id to same_as_RB"); 
      return FAILURE;
    }
  } /* if RLC info choice is "Same as RB" type */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r5_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)&&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", local_rb_id);
      return FAILURE;
    }
  }
  
    /* Now we have either the RLC Info explicitly meant for this RB OR
    * the RLC Info that is the same as another RB.Get the RLC mode */
  
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r5_rlc_Info) &&
       ((logch_mode = get_rlc_mode_r5(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
     )
  {
    return FAILURE;
  }
  
  /* Save off the RLC Info */
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r5_rlc_Info) &&
      (get_rlc_info_r5(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, 
        local_rb_id, TRUE) == FAILURE))
  {
    return FAILURE;
  }
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r5(&ie_ptr->rb_MappingInfo, local_rb_id, logch_mode)
    == FAILURE)
  {
    return FAILURE;
  }
  
#ifdef FEATURE_UMTS_PDCP

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r5,pdcp_Info))
  {
  if (rrc_pdcp_enabled)  
  { 
    WRRC_MSG0_HIGH("PDPC info present in rb setup");
    if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,
                                    &ie_ptr->pdcp_Info,local_rb_id,CPDCP_SETUP ) == FAILURE)
    {
      return FAILURE;
    }
    return SUCCESS;
  }
  else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r5,pdcp_Info)) &&
      ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
       (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
       ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
        (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)))))
  {
    WRRC_MSG0_ERROR("PDCP NV not enabled.  PDCP info not handled");
    return FAILURE;
  }
  }

  /* if fall-thru happens, PDCP layer has to be setup in transparent [pass thru] mode */
  if (cn_Domain == rrc_CN_DomainIdentity_ps_domain) 
  {
    WRRC_MSG1_HIGH("Setting up PDCP in transparent mode rb_id %d",local_rb_id);
    /*Get the next free index in pdcp_config_req */
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use == FALSE) 
      {
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id = local_rb_id;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.is_mcast_call = FALSE;

        ordered_config_ptr->pdcp_parms.num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
    /* CPDCP action will be setup, but pdcp info will be null */
    //ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
  }
#else
  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r5,pdcp_Info)) &&
      ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
       (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
       ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
        (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)))))
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }
#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_setup_r5() */


/*==========================================================
FUNCTION  : get_srb_info_to_setup_r5()

DESCRIPTION : Get Signalling Radio Bearer Information to 
              setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_srb_info_to_setup_r5
(
rrc_SRB_InformationSetup_r5     *ie_ptr,
rrc_SRB_InformationSetupList_r5 * srb_setup_ptr,
rrc_RB_Identity                   rb_id
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  
  /* Local count of # of SRB's to setup */
  uint8                           srb_cnt = 0;
  
  /* temp rb id which we shall compute if rb_id is not part of
  * srb_setup_ptr when we loop thru the list to get a match
  */
  rrc_RB_Identity                      temp_rb_id = 0;
  

  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_SRB_InformationSetup_r5     *temp_ie_ptr = ie_ptr;
  
  /* Entity indexes in case we need to look up RLC info from
  * our internal database instead of processing an IE.
  */
  uint8 entity_idx, new_entity_idx;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the srb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r5_same_as_RB))
  {
    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        temp_rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        temp_rb_id++;
      }
      
      /*check if the rb_id matches*/
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == temp_rb_id)
      {
        temp_ie_ptr = &(srb_setup_ptr->elem[srb_cnt]);
        break;
      }
      
      
      srb_cnt++;
    } /* end while... */
    
    if ((srb_setup_ptr->n == srb_cnt) || (srb_cnt >= UE_MAX_SRB_SETUP))
    {
      WRRC_MSG1_HIGH("same_as_RB %d not in msg, lookup req'd",
        ie_ptr->rlc_InfoChoice.u.same_as_RB);
      temp_ie_ptr = NULL;
      temp_rb_id = ie_ptr->rlc_InfoChoice.u.same_as_RB;
      
      if ((entity_idx = get_am_index(temp_rb_id)) != UE_MAX_AM_ENTITY)
      {
      /* This RB is in AM mode. And its reference config is found in OC.
        */
        if ((new_entity_idx = rrcllc_get_1st_hole_for_rlc_am()) == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG1_ERROR("Can't find space for new rb %d", rb_id);
          return FAILURE;
        }
        ordered_config_ptr->am_config[new_entity_idx].rlc_ul_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->common_config.rb_id = rb_id;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan++;
        ordered_config_ptr->am_config[new_entity_idx].rlc_dl_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan++;

        ordered_config_ptr->rlc_am_parms.nentity++;

        if((ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan >
                                                       UE_MAX_UL_AM_DATA_CHANNEL) || 
           (ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan >
                                                       UE_MAX_DL_AM_DATA_CHANNEL) ||
           (ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY))
        {
          ERR("Too many UL/DL AM channels or entities [DL,UL,entities] [%d,%d,%d]",
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.nentity);
          return FAILURE;
        }
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      if ((entity_idx = get_ul_um_index(temp_rb_id))
        != UE_MAX_UL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_ul_um_info(rb_id, TRUE) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      if ((entity_idx = get_dl_um_index(temp_rb_id, ordered_config_ptr))     
        != UE_MAX_DL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_dl_um_info(rb_id, TRUE, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      /* SRBs cannot be TM.  If we haven't found it by now, we have
      * a problem. */
      if (logch_mode == UE_MODE_MAX_NUM)
      {
        WRRC_MSG1_ERROR("same_as_RB %d not found in msg or OC",
          ie_ptr->rlc_InfoChoice.u.same_as_RB);
        return FAILURE;
      }
    }
  } /* end RLC choice == Same as RB */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r5_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)) ||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", rb_id);
      return FAILURE;
    }
  }
  
  
  /* Now we have either the RLC Info explicitly meant for this RB OR
  * the RLC Info that is the same as another RB.  If tmp_ie_ptr is NULL,
  * then we've already copied the RLC Info from our internal database.
  */
  if (temp_ie_ptr != NULL)
  {
    /* Get the RLC mode */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r5_rlc_Info) && 
         ((logch_mode = get_rlc_mode_r5(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
       )
    {
      return FAILURE;
    }
    
    /* Get the RLC info */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r5_rlc_Info) && 
        (get_rlc_info_r5(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, rb_id, TRUE) == FAILURE))
    {
      return FAILURE;
    }
  }
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r5(&ie_ptr->rb_MappingInfo, rb_id, logch_mode) == FAILURE)
  {
    return FAILURE;
  }
#ifdef FEATURE_CHANGE_SRB4_PRIRORITY
  rrcllc_check_and_set_srb3_srb4_priority(); 
#endif 

  if(FAILURE == rrcllc_validate_srb_id(rb_id))
  {
    return FAILURE;
  }

  /* everything is configured */
  return SUCCESS;

} /* end get_srb_info_to_setup_r5() */



/*==========================================================
FUNCTION  : rrcllc_update_ul_ded_log_chl_info_r6()

DESCRIPTION : Processes UL Logical channel info and fills it
              in passed pointer to MAC I/F struct.

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
static void  rrcllc_update_ul_ded_log_chl_info_r6
 (
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr, 
  mac_ul_ded_logch_config_type  *dlchan_info,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
 )
{
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
  if(ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
  {
    dlchan_info->mac_id =
      (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
  }
  else
  {
    dlchan_info->mac_id = 
      RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
  }

  /* MAC Logical Channel Priority */
  dlchan_info->priority =
    (uint8)ie_ptr->mac_LogicalChannelPriority;

  /* Save Radio Bearer ID */
  dlchan_info->rb_id = rb_id;

  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    dlchan_info->chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    dlchan_info->chan_type = UE_LOGCHAN_DTCH;
  }

  /* Save the logical channel mode, if a setup */
  if(logch_mode != UE_MODE_MAX_NUM)
  {
    dlchan_info->rlc_mode = logch_mode;
  }
}  /* rrcllc_update_ul_ded_log_chl_info */


/*==========================================================
FUNCTION  : get_rlc_size_list_r6()

DESCRIPTION : Processes the RLC Size List IE contents and saves 
              them in the RLC size restriction database.  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: The RLC size restriction database is updated.
============================================================*/
static uecomdef_status_e_type get_rlc_size_list_r6
(
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr,
  rrcllcoc_rlc_size_list_type  *rlc_size_rec,
  rrc_RB_Identity                    rb_id
)
{
  rrc_RLC_SizeExplicitList *rlc_size_list = NULL;

  rlc_size_rec->rb_id = rb_id;
  switch(ie_ptr->ul_TrCH_Type.u.dch_rach_usch->rlc_SizeList.t)
  {
    case T_rrc_UL_LogicalChannelMapping_r6_rlc_SizeList_allSizes:
      rlc_size_rec->restriction_type = RLC_SIZES_ALL;
      break;

    case T_rrc_UL_LogicalChannelMapping_r6_rlc_SizeList_configured:
      rlc_size_rec->restriction_type = RLC_SIZES_CONFIGURED;
      break;

    case T_rrc_UL_LogicalChannelMapping_r6_rlc_SizeList_explicitList:
      rlc_size_rec->restriction_type = RLC_SIZES_EXPLICIT_LIST;
      rlc_size_list = ie_ptr->ul_TrCH_Type.u.dch_rach_usch->rlc_SizeList.u.explicitList;
      if(rlc_size_list == NULL)
      {
        WRRC_MSG0_ERROR("RLC Size List empty");
        return(FAILURE);
      }
      rlc_size_rec->count = 0;
      while(rlc_size_list->n > rlc_size_rec->count)
      {
        rlc_size_rec->size_idx[rlc_size_rec->count] = 
			(uint16)rlc_size_list->elem[rlc_size_rec->count].rlc_SizeIndex;
        rlc_size_rec->count++;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid RLC Size List CHOICE: %d", ie_ptr->ul_TrCH_Type.u.dch_rach_usch->rlc_SizeList.t);
      return(FAILURE);
  }

  return SUCCESS;
} /* end get_rlc_size_list_r6() */

/*==========================================================
FUNCTION  : get_rach_mapping_info_r6()

DESCRIPTION : Process RACH Mapping IE's contents and saves 
              them in the MAC Ordered Configuration Database.
              If next RRC state is CELL_DCH, save the RACH
              mapping contents in a local data structure in
              OC.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rach_mapping_info_r6
(
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode,
  rrc_state_e_type             next_rrc_state
)
{
  /* transport Channel Index */
  uint8   trch_idx  = RRCLLC_INVALID_TRCH_IDX;

  /* Get the logical channel index */
  uint8   logch_idx, cnt;
 
  mac_ul_ded_logch_config_type  *dlchan_info;
  rlc_lc_id_type   rlc_lc_id = (uint8)RRCLCM_RLC_LC_ID_NOT_FOUND;
  /* TBD: Although all existing FACHs would be mapped to each of the RBs,
   * fill up only for the 1st FACH - till MAC interface is clarified/updated
   */

  boolean update_rach_rlc_id = FALSE;

  /* Temporary pointer for processing the RLC Size List */
  rrcllcoc_rlc_size_list_type *rlc_size_rec = NULL;


  /* Start with updating Cell FACH RB Mapping Info when the next state is
   * either CELL_DCH or CELL_FACH
   */
  logch_idx = ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan;
  dlchan_info = &ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx];

  /* Save the RLC Size List information */
  ordered_config_ptr->rach_rlc_size_restriction_info.mac_update_needed = TRUE;
  rlc_size_rec =
    &(ordered_config_ptr->rach_rlc_size_restriction_info.rlc_size_list[logch_idx]);
  if(get_rlc_size_list_r6(ie_ptr, rlc_size_rec, rb_id) == FAILURE)
  {
    return FAILURE;
  }


  if(logch_mode == UE_MODE_MAX_NUM)
  {
    /* This is the case when RB mapping info applicable to CELL_FACH is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
    */
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
    
    if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
    }
    /* Also update the Logical Channel ID here */
    dlchan_info->rlc_id = rlc_lc_id;
    update_rach_rlc_id = TRUE;
  }
  else
  {
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  != RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      /* Also update the Logical Channel ID here */
      dlchan_info->rlc_id = rlc_lc_id;
      update_rach_rlc_id = TRUE;
    }
  }
  #ifdef FEATURE_WCDMA_HS_FACH
  for (cnt = 0; cnt < ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ; cnt++) 
  {            
    if(rb_id == ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cnt].rb_id)
    {
      WRRC_MSG1_MED("RB_ID %d already present",rb_id);
      return (SUCCESS);
    }    
  }
#endif
  
  rrcllc_update_ul_ded_log_chl_info_r6(ie_ptr, dlchan_info, rb_id, logch_mode);
  ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ++;  
   
  /* Now update the FACH Info ONLY IF the next state is CELL_FACH */
  if(next_rrc_state == RRC_STATE_CELL_FACH ||
     next_rrc_state == RRC_STATE_CELL_PCH ||
     next_rrc_state == RRC_STATE_URA_PCH)
  {
    if((trch_idx = rrcllc_find_ul_rach_trch_idx()) >= UE_MAX_TRCH)
    {
      return FAILURE;
    }
    logch_idx = ordered_config_ptr->ul_cctrch.rach_info.ndlchan;
    dlchan_info = &ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx];
    if(update_rach_rlc_id)
    {
      dlchan_info->rlc_id = rlc_lc_id;
    }
    rrcllc_update_ul_ded_log_chl_info_r6(ie_ptr, dlchan_info, rb_id, logch_mode);

    ordered_config_ptr->ul_cctrch.rach_info.ndlchan++;
    ordered_config_ptr->ul_rach_trch_info[trch_idx].lg_ch_cnt = 
      ordered_config_ptr->ul_cctrch.rach_info.ndlchan;

  }  /* If next state is CELL_FACH, update the FACH Info */
 
  return SUCCESS;

} /* end get_rach_mapping_info_r6() */

/*==========================================================
FUNCTION  : get_ul_dch_mapping_info_r6()

DESCRIPTION : Process UL DCH Mapping IE's contents and saves 
              them in the MAC Ordered Configuration Database.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_dch_mapping_info_r6
(
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  /* transport Channel Index */
  uint8         trch_idx = RRCLLC_INVALID_TRCH_IDX;

  /* Transport Channel ID */
  tr_ch_id_type   trch_id = 0;

  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  uint8 cnt = 0;

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

  /* Temporary pointer for processing the RLC Size List */
  rrcllcoc_rlc_size_list_type *rlc_size_rec = NULL;
#endif/*#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */


  if(RRC_CHECK_COMMON_MSG_TYPE( ie_ptr->ul_TrCH_Type,rrc_UL_LogicalChannelMapping_r6_ul_TrCH_Type_dch_rach_usch))
  {
    /* Get the UL DCH TrCH ID if it exists */
    switch (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t)
    {
      case T_rrc_UL_TransportChannelType_dch:
        trch_id = (tr_ch_id_type)
          ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.u.dch;
        break;
  
      case T_rrc_UL_TransportChannelType_rach:
      case T_rrc_UL_TransportChannelType_dummy:
      case T_rrc_UL_TransportChannelType_usch:
      default:
        WRRC_MSG0_ERROR("Invalid UL DCH TrCH ID Type");
        return FAILURE;
    } /* end UL DCH TrCH ID switch */
  
    /* Get the transport channel index for this transport channel ID.
     * Note: The index should already be there.
     */
    trch_idx = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr, trch_id);
    if(trch_idx >= UE_MAX_TRCH)
    {
      return(rrcllc_backup_ul_dch_from_ota_r6(ie_ptr,rb_id,logch_mode));
    }
  
    /* Get the Logical Channel Index */
    logch_idx = ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
  
    for (cnt = 0; cnt < ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan; cnt++) 
    {
      if (rb_id == ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnt].rb_id)
      {
        WRRC_MSG1_MED("RB_ID %d already present", rb_id);
        return (SUCCESS); 
      }
    }

    /* If Logical Channel ID is present, save MAC Logical Channel Identity */
    if (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
    {
      
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
        (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
    }
    else 
    {
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
        RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
    }

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

    /* Save the RLC Size List information */
    ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].mac_update_needed = TRUE;
    rlc_size_rec = &(ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].rlc_size_list[logch_idx]);
    if(get_rlc_size_list_r6(ie_ptr, rlc_size_rec, rb_id) == FAILURE)
    {
      return FAILURE;
    }
#else
    /* Initialize the MAC data structures to allow all RLC sizes, just in case. */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_size_info.all_tfi_valid = TRUE;
#endif /* FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */

    /* MAC Logical Channel Priority */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].priority =
      (uint8) ie_ptr->mac_LogicalChannelPriority;
  
    /* Save Radio Bearer ID */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rb_id = rb_id;
  
    /* Determine the type of Logical Channel mapped to DCH TrCH */
    if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
    }
    else
    {
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
    }
  
    if(logch_mode == UE_MODE_MAX_NUM)
    {
      /* This is the case when RB mapping info applicable to CELL_FACH is coming 
       * in "Affected List". At this point, we need to update the Logical Channel mode 
       * and Logical Channel ID in Cell FACH RB Mapping Info - 
       * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
       */
      rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
      if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rb_id);
        return(FAILURE);
      }
    
      if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
      {
        WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
      }
  
      /* Also update the Logical Channel ID here */
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
    }
    
    /* Save the logical channel mode, if a setup */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_mode = logch_mode;
  
    /* other wise it is just a reconfig of RB Mapping info */
  
    /* Increment number of logical channels */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan++;
    ordered_config_ptr->ul_dch_trch_info[trch_idx].lg_ch_cnt = ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
  }
  else /* rrc_ul_TrCH_Type_e_dch_chosen */
  {
    WRRC_MSG0_ERROR("RRCEUL: R6 ASN1 rrc_ul_TrCH_Type_e_dch_chosen not yet supported");
  }
  rrcllc_free_hanging_rb_mapping_info_index(rb_id,UL_DCH_RB_MAPPING);
  return SUCCESS;

} /* end get_ul_dch_mapping_info() */
/*==========================================================
FUNCTION  : get_edch_mapping_info_r6()

DESCRIPTION : Processes EDCH RB Mapping info contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_edch_mapping_info_r6
(
rrc_UL_LogicalChannelMapping_r6 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode,
boolean                       one_sided_reest_present,
boolean                       one_sided_reest,
boolean allow_new_channels
#ifdef FEATURE_MAC_I
,ordered_config_type *backup_ordered_config_ptr
#endif /* FEATURE_MAC_I */

)
{
  /* EDCH MAC-D FLOW Id */
  uint32         dflow_id;
  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  uint8 mac_dflow_index;

  rrc_RLC_PDU_SizeList *rlc_pdu_sizelist;
  uint8 ddi = 0;
  uint32 cnt = 0;
  uint16 rlc_size = 0;
  uint32 index = 0;

  dflow_id = ie_ptr->ul_TrCH_Type.u.e_dch->e_DCH_MAC_d_FlowIdentity;
  
  mac_dflow_index = rrcllc_find_e_mac_d_flow_idx((uint8)dflow_id);
  /* If DFLOW Index does not exist, then create a new one */
  if (mac_dflow_index == UE_MAX_MAC_D_FLOW)
  {
    
    WRRC_MSG1_HIGH("EDCH Mac-d FLOW Index for DFLOW %d, not found", dflow_id);
    return(rrcllc_backup_eul_from_ota(ie_ptr,rb_id,logch_mode));
  }
  else
  {
    for (index=0;
      index < ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch; index++)
    {
      if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[index].rb_id == rb_id)
      {
        WRRC_MSG3_MED("RB_ID %d, logical channel identity %d, mac_id: %d already present",
          rb_id, ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity,
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_mac_d_flow_id);      
        return(SUCCESS);
      }
    }
  }

  /* Get the Logical Channel Index */
  logch_idx = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch;
  rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);

  if (rlc_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {

    /*This is the case where RB-mapping info is present in rb-reconfig-info list or affected list*/
    if (rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
  }
    /* Also update the Logical Channel ID here */
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_id = rlc_lc_id;
#ifdef FEATURE_MAC_I		
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_pdu_type = RLC_FIXED_PDU_SIZE;
#endif /* FEATURE_MAC_I */
  /* Save the and RLC info DDI info*/
  rlc_pdu_sizelist = &ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_SizeList;

  /* If more than 1 UL RLC PDU size is configured for this RB, the different sizes will use subsequent DDI values starting
    from this DDI value.*/
  ddi = ie_ptr->ul_TrCH_Type.u.e_dch->ddi;
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].num_rlc_size_ddi_info = 0;

  while ((rlc_pdu_sizelist->n > cnt) && (cnt < RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN)) 
  {
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].
      rlc_size_ddi_info[cnt].ddi = ddi;

    switch (rlc_pdu_sizelist->elem[cnt].t)
    {
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType1 * 8) + 16;
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part1 * 32) + 272;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType2->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part2 * 8);
        }
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part1 * 64) + 1040;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType3->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part2 * 8);
        }
        break;
      default:
        WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
            rlc_pdu_sizelist->elem[cnt].t);
        break;
    }
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].
      rlc_size_ddi_info[cnt].rlc_pdu_size_in_bits = rlc_size;

    /* Increment the number of valid fields in rlc_size_ddi_info[] */
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].num_rlc_size_ddi_info++;
    cnt++;
    /* Increment the DDI */
    ddi++;
   
  }


  /* Ignore the logical channel identity signalled by NW.  Not needed by MAC*/ 
  
  /* Save Radio Bearer ID */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rb_id = rb_id;

  /* Get the priority between a user's different RBs (or logical channels). */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].priority =
    ie_ptr->mac_LogicalChannelPriority;
  
  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
  } 
  else
  {
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
  }

  /* If set include scheduling info is set to false then dont consider this mac_id while sending schedulingInformation */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].include_in_scheduling_info = 
    ie_ptr->ul_TrCH_Type.u.e_dch->includeInSchedulingInfo;

  /* Set the logical channel Id recieved from the NW */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].lc_id = 
    ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity;
  
  /* Save the logical channel mode, if a setup */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_mode = logch_mode;
  
  /* Increment number of logical channels */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch++;  

  WRRC_MSG3_MED("RRCEUL:EDCH E-Mac-d FLOW %d, No of dedicated Ch %d, rlc mode: %d(1:UM, >1:AM)",
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_mac_d_flow_id,
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch,
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_mode);
  rrcllc_free_hanging_rb_mapping_info_index(rb_id,E_DCH_RB_MAPPING);
  return SUCCESS;

}


/*==========================================================
FUNCTION  : get_ul_single_logical_channel_info_r6()

DESCRIPTION : Process one UL Logical Channel Mapping IE's 
              contents and saves them in the MAC Ordered 
              Configuration Database.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_single_logical_channel_info_r6
(
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
  ,boolean one_sided_reest_present
  ,boolean one_sided_reest
  ,boolean allow_new_channels
)
{
  ordered_config_type *tmp_config_ptr = NULL;


  if (RRC_CHECK_COMMON_MSG_TYPE( ie_ptr->ul_TrCH_Type,rrc_UL_LogicalChannelMapping_r6_ul_TrCH_Type_dch_rach_usch))
  {
    /* Transport channel type */
    switch (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t)
    {
      case T_rrc_UL_TransportChannelType_dch:
        if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
        {
          if (get_ul_dch_mapping_info_r6(ie_ptr, rb_id, logch_mode) == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {
            if(transition_config.toc_usage == TOC_INVALID)
            {
              transition_config.toc_usage = TOC_FOR_DCH;
            }
  
            tmp_config_ptr = ordered_config_ptr;
            ordered_config_ptr = transition_config.toc_ptr;
  
            /* Processing functions must think we are going to Cell_DCH state. */
            ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
  
            if(get_ul_dch_mapping_info_r6(ie_ptr, rb_id, logch_mode) == FAILURE)
            {
              ordered_config_ptr = tmp_config_ptr;
              tmp_config_ptr = NULL;
              return FAILURE;
            }
  
            ordered_config_ptr = tmp_config_ptr;
            tmp_config_ptr = NULL;
        }
        break;
  
      case T_rrc_UL_TransportChannelType_rach:

        if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_FACH ||
           ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_PCH ||
           ordered_config_ptr->rrc_state_indicator == RRC_STATE_URA_PCH)
        {
          if (get_rach_mapping_info_r6(ie_ptr, rb_id, logch_mode,
                              ordered_config_ptr->rrc_state_indicator) == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {
          if (get_rach_mapping_info_r6(ie_ptr, rb_id, logch_mode, RRC_STATE_CELL_DCH) == FAILURE)
          {
            return FAILURE;
          }
        }
        break;
  
      case T_rrc_UL_TransportChannelType_dummy:
      case T_rrc_UL_TransportChannelType_usch:
      default:
        WRRC_MSG1_ERROR("Rcvd Invalid/DUMMY/USCH UL TrCh type %d, return FAILURE", ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t);
        return FAILURE;
    }
  }
  else if(RRC_CHECK_COMMON_MSG_TYPE( ie_ptr->ul_TrCH_Type, rrc_UL_LogicalChannelMapping_r6_ul_TrCH_Type_e_dch))/* rrc_ul_TrCH_Type_e_dch_chosen */
  {
    if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
    {
      if (logch_mode != UE_MODE_TRANSPARENT) 
      {
        if (get_edch_mapping_info_r6(ie_ptr, rb_id, logch_mode
             , one_sided_reest_present, one_sided_reest,allow_new_channels
#ifdef FEATURE_MAC_I            
             ,NULL
#endif /* FEATURE_MAC_I */            
             ) == FAILURE)
        {
          return FAILURE;
        }
        rrcllc_update_mac_eul_action(TRUE);
      }
      else
      {
        WRRC_MSG0_ERROR("Cannot map TM RB on EDCH");
        return FAILURE;
      }
    }
    else
    {
      if(transition_config.toc_usage == TOC_INVALID)
      {
        transition_config.toc_usage = TOC_FOR_DCH;
      }
      
      tmp_config_ptr = ordered_config_ptr;
      ordered_config_ptr = transition_config.toc_ptr;
      
      /* Processing functions must think we are going to Cell_DCH state. */
      ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
      
      if (logch_mode != UE_MODE_TRANSPARENT) 
      {
        if (get_edch_mapping_info_r6(ie_ptr, rb_id, logch_mode
             , one_sided_reest_present, one_sided_reest,allow_new_channels
#ifdef FEATURE_MAC_I           
             ,tmp_config_ptr
#endif /* FEATURE_MAC_I */
             ) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;  
          return FAILURE;
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Cannot map TM RB on EDCH");
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;  
        return FAILURE;
      }
      
      ordered_config_ptr = tmp_config_ptr;
      tmp_config_ptr = NULL;  
    }
#ifdef FEATURE_WCDMA_HS_RACH
/*Always backup the mapping in hanging rb independent of state*/
    //if(rrc_ordered_state  != RRC_STATE_CELL_DCH)
    {
      rrcllc_backup_eul_from_ota(ie_ptr,rb_id,logch_mode);
    }
#endif
  }

  return SUCCESS;

} /* end get_ul_single_logical_channel_info_r6() */

/*==========================================================
FUNCTION  : get_ul_logical_channel_info_r6()

DESCRIPTION : Get UL logical Channel info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_logical_channel_info_r6
(
  rrc_UL_LogicalChannelMappings_r6 * ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
  ,boolean one_sided_reest_present
  ,boolean one_sided_reest
  ,boolean allow_new_channels
)
{

  /* Process IE info based on number of UL Logical Channels */
  switch (ie_ptr->t)
  {
    /* One UL Logical Channel */
    case T_rrc_UL_LogicalChannelMappings_r6_oneLogicalChannel:
      if(get_ul_single_logical_channel_info_r6(
         ie_ptr->u.oneLogicalChannel, rb_id, logch_mode
         ,one_sided_reest_present,one_sided_reest,allow_new_channels) == FAILURE)
      {
        return FAILURE;
      }
      break;

    /* Two logical channels, currently not supported by RRC-RLC I/F */
    case T_rrc_UL_LogicalChannelMappings_r6_twoLogicalChannels:
      /* TRUE indicates that first logical channel is for Data PDUs and second  
         logical channel is for control PDUs */
      /* FALSE indicates that Data and Control PDUs can be sent on either of them */

      /* Invalid number of logical channels */
      default:
      WRRC_MSG1_ERROR("Support only 1 Log CH per AM/ Invalid number of UL logical channels, return FAILURE", ie_ptr->t );
        return FAILURE;
  } /* end switch */

  return SUCCESS;

} /* end get_ul_logical_channel_info() */

/*==========================================================
FUNCTION  : get_rlc_mode_r6()

DESCRIPTION : Gets the RLC mode for a given logical channel.

DEPENDENCIES: None.

RETURN VALUE: uecomdef_logch_mode_e_type indicate whether or 
              not it is TM, UM or AM.

SIDE EFFECTS: None.
============================================================*/
static uecomdef_logch_mode_e_type get_rlc_mode_r6
(
  rrc_RLC_Info_r6 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  uecomdef_logch_mode_e_type dl_logch_mode = UE_MODE_MAX_NUM;

  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,ul_RLC_Mode))
  {
    switch (ie_ptr->ul_RLC_Mode.t)
    {
      case T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode:
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;
  
      case T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode:
        logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;
  
      case T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode: 
        logch_mode = UE_MODE_TRANSPARENT;
        break;
  
      case T_rrc_UL_RLC_Mode_spare:
      default:
        WRRC_MSG1_ERROR("Unknown UL RLC mode %d", ie_ptr->ul_RLC_Mode.t);
        logch_mode = UE_MODE_MAX_NUM;
        break;
    }  /* end UL RLC choice */
  }  /* end if UL RLC present */

  /* Downlink RLC info */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_r6_dl_AM_RLC_Mode:
        dl_logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;
  
      case T_rrc_DL_RLC_Mode_r6_dl_UM_RLC_Mode:
        dl_logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;
  
      case T_rrc_DL_RLC_Mode_r6_dl_TM_RLC_Mode: 
        dl_logch_mode = UE_MODE_TRANSPARENT;
        break;
  
      default:
        WRRC_MSG1_ERROR("Unknown DL RLC mode %d", ie_ptr->ul_RLC_Mode.t);
        dl_logch_mode = UE_MODE_MAX_NUM;
        break;
    }  /* end DL RLC choice */

    /* If both DL and UL mode is present, check to make sure they are the same */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,ul_RLC_Mode))
    {
      if (logch_mode != dl_logch_mode) 
      {
        WRRC_MSG2_ERROR("UL %d and DL %d lc mode not a match.  Invalid config",logch_mode,dl_logch_mode);
        logch_mode = UE_MODE_MAX_NUM;
      }
    }
    else
    {
      logch_mode = dl_logch_mode;
    }

  }  /* end if DL RLC present */

  /* Need to know RLC type in either UL or DL */
  if (!((ie_ptr->m.ul_RLC_ModePresent) ||
                            (ie_ptr->m.dl_RLC_ModePresent)))
  {
    WRRC_MSG0_ERROR("Neither DL nor UL RLC mode present!");
  }


  return logch_mode;

} /* end get_rlc_mode_r6 */

/*==========================================================
FUNCTION  : get_rlc_info_r6 ()

DESCRIPTION : 
  Processes RLC Info IE

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_rlc_info_r6
(
rrc_RLC_Info_r6 *ie_ptr,
rrc_RB_Identity    rb_id,
boolean       allow_new_channels
)
{
  uint8 entity_idx;
  boolean new_rlc_am_entity = FALSE;
  rlc_reestablish_e_type reestablish_entity = RLC_RE_ESTABLISH_NONE;

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,altE_bitInterpretation))
  {
    if(((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,ul_RLC_Mode)) &&
        (ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode ))
        ||((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,dl_RLC_Mode)) &&
           (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_RLC_Mode,rrc_DL_RLC_Mode_r6_dl_UM_RLC_Mode))))
    {
      if(nv_gcf_test_flag == FALSE)
      {
        WRRC_MSG0_ERROR("N\\W send ALT E Bit, ALT E Bit not supported");
        return FAILURE;
      }
    }
  }

  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,ul_RLC_Mode))
  {
    if(get_ul_rlc_info(&ie_ptr->ul_RLC_Mode,rb_id,allow_new_channels,&new_rlc_am_entity) == FAILURE)
    {
        return FAILURE;
    }
	
    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode )
    {
      entity_idx = get_ul_um_index(rb_id);
      if(entity_idx < UE_MAX_UL_UM_CHANNEL)
      {
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,altE_bitInterpretation))
        {
          ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].alternate_e_bit = TRUE;
        }
        else
        {
          ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].alternate_e_bit = FALSE;
        }
      }
    }
    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode )
    {
      entity_idx = get_am_index_wo_f3(rb_id);
      if(entity_idx == UE_MAX_AM_ENTITY)
      {
        WRRC_MSG0_ERROR("The corresponding UL AM info not present");
        return FAILURE;
      }
      WRRC_MSG0_HIGH("R6 does not support Special HE field, set to FALSE");        
      ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config = FALSE;
    }
  } /* end if UL RLC present */
  
  /* Downlink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_r6_dl_AM_RLC_Mode: /* rrc_dl_AM_RLC_Mode_r5_chosen */
        
        entity_idx = get_am_index_wo_f3(rb_id);
        
        /* LLC processes the UL AM info first, it should find the corresponding
        entity index for DL AM info to be filled in. */
        if (entity_idx == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG0_ERROR("Not allowed to add new DL AM RLC entity");
          return FAILURE;
        }
        if (ie_ptr->rlc_OneSidedReEst == TRUE)
        {
          reestablish_entity = RLC_RE_ESTABLISH_DL;
        }
        else
        {
          reestablish_entity = RLC_RE_ESTABLISH_DL_UL;
        }
        WRRC_MSG2_MED("One-Sided Re-est from OTA %d,so set downlink local_reestablish_entity to [DL_UL/DL/UL/None]%d",ie_ptr->rlc_OneSidedReEst,reestablish_entity);
        if (get_dl_am_rlc_info_r5(ie_ptr->dl_RLC_Mode.u.dl_AM_RLC_Mode, entity_idx, rb_id
          , allow_new_channels, reestablish_entity) == FAILURE)
        {
          return FAILURE;
        }
        
        /* This flag is set when processing UL AM info. */
        if(new_rlc_am_entity)
        {
        /* Indicate that the AM entity has been filled up. LLC does not increase this
        number of entities when process UL AM info.
          */
          ordered_config_ptr->rlc_am_parms.nentity++;
          
          /* Make sure UE has not exceeded the maximum number of AM Entities */
          if(ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY)
          {
            WRRC_MSG1_ERROR("Too Many AM Entities %d", ordered_config_ptr->rlc_am_parms.nentity);
            return FAILURE;
          }
          
          /* It always assume to be the shared channel (Control&Data) 
          as we support now.
          */  
          ordered_config_ptr->rlc_am_parms.am_info[entity_idx]->dl_nchan = 1;
          
        }
        
        ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.special_value_he_config = FALSE;
        
        break;  /* DL AM */
        
      case T_rrc_DL_RLC_Mode_r6_dl_UM_RLC_Mode: /* rrc_dl_UM_RLC_r6_Mode_chosen NULL */
      
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r6,altE_bitInterpretation))
        {
          if(get_dl_rlc_um_info_r6(ie_ptr->dl_RLC_Mode.u.dl_UM_RLC_Mode,rb_id,allow_new_channels,TRUE)
              == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {
          if(get_dl_rlc_um_info_r6(ie_ptr->dl_RLC_Mode.u.dl_UM_RLC_Mode,rb_id,allow_new_channels,FALSE)
              == FAILURE)
          {
            return FAILURE;
          }
        }
      
        break;
        
      case T_rrc_DL_RLC_Mode_r6_dl_TM_RLC_Mode:
        if(get_dl_rlc_tm_info(ie_ptr->dl_RLC_Mode.u.dl_TM_RLC_Mode->segmentationIndication,rb_id,allow_new_channels)
            == FAILURE)
        {
          return FAILURE;
        }
        break;
        
      default:
        WRRC_MSG1_ERROR("Unknown DL RLC mode %d", ie_ptr->dl_RLC_Mode.t);
        return FAILURE;
    } /* end DL RLC choice */
  } /* end if DL RLC present */
  
  /* Got here, everything is a go */
  return SUCCESS;

} /* end get_rlc_info_r6() */

/*==========================================================
FUNCTION  : get_rb_mapping_info_r6()

DESCRIPTION : Get Radio Bearer Mapping Info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rb_mapping_info_r6
(
rrc_RB_MappingInfo_r6      * ie_ptr,
rrc_RB_Identity                 rb_id,
uecomdef_logch_mode_e_type logch_mode,
boolean                    one_sided_reest_present,
boolean                    one_sided_reest,
boolean allow_new_channels
)
{
  /*
  * Count to make sure that don't exceed the maximum number 
  * of RB Mux options
  */
  uint8 rb_mux_cnt = 0;

  /* Process the RB Mux option IE in the linked list */
  while ((ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5))
  {
    /* Check for DL Channel Mapping List */
    if (ie_ptr->elem[rb_mux_cnt].m.dl_LogicalChannelMappingListPresent)
    {
      MSG_MED("Get RB %d Mapping Info for DL", rb_id,0,0);
      if (get_dl_logical_channel_info_r5(&ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList,
                                         rb_id, logch_mode) == FAILURE)
      {
        return FAILURE;
      }
      /* Here indicate that MAC Downlink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
    }  /* end dl Log Ch mapping list */

    if (ie_ptr->elem[rb_mux_cnt].m.ul_LogicalChannelMappingsPresent)
    {
      MSG_MED("Get RB %d Mapping Info for UL", rb_id,0,0);
      if (get_ul_logical_channel_info_r6(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,
                                      rb_id, logch_mode
           ,one_sided_reest_present,one_sided_reest,allow_new_channels) == FAILURE)
      {
        return FAILURE;
      }

      /* Here indicate that MAC Uplink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
    }  /* end ul Log Ch mapping list */
    rb_mux_cnt++;
  }  /* end loop of RB Mapping info list */
#ifdef FEATURE_WCDMA_HS_FACH
  rrcllc_backup_mapping_info_into_hanging(ie_ptr,REL_6,rb_id,logch_mode);
#endif

  return SUCCESS;
} /* end get_rb_mapping_info_r6() */

/*==========================================================
FUNCTION  : get_srb_info_to_setup_r6()

DESCRIPTION : Get Signalling Radio Bearer Information to setup.

REUSABILITY: R6

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_srb_info_to_setup_r6
(
  rrc_SRB_InformationSetup_r6     *ie_ptr,
  rrc_SRB_InformationSetupList_r6 * srb_setup_ptr,
  rrc_RB_Identity                   rb_id
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  
  /* Local count of # of SRB's to setup */
  uint8                           srb_cnt = 0;
  
  /* temp rb id which we shall compute if rb_id is not part of
  * srb_setup_ptr when we loop thru the list to get a match
  */
  rrc_RB_Identity                      temp_rb_id = 0;
    
 /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_SRB_InformationSetup_r6     *temp_ie_ptr = ie_ptr;
  
  /* Entity indexes in case we need to look up RLC info from
  * our internal database instead of processing an IE.
  */
  uint8 entity_idx, new_entity_idx;
  boolean one_sided_reest_present=FALSE;
  boolean one_sided_reest=FALSE;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the srb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r6_same_as_RB))
  {
    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        temp_rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        temp_rb_id++;
      }
      
      /*check if the rb_id matches*/
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == temp_rb_id)
      {
        temp_ie_ptr = &(srb_setup_ptr->elem[srb_cnt]);
        break;
      }
      
      srb_cnt++;
    } /* end while... */
    
    if ((srb_setup_ptr->n == srb_cnt) || (srb_cnt >= UE_MAX_SRB_SETUP))
    {
      WRRC_MSG1_HIGH("same_as_RB %d not in msg, lookup req'd",
        ie_ptr->rlc_InfoChoice.u.same_as_RB);
      temp_ie_ptr = NULL;
      temp_rb_id = ie_ptr->rlc_InfoChoice.u.same_as_RB;
      
      if ((entity_idx = get_am_index(temp_rb_id)) != UE_MAX_AM_ENTITY)
      {
      /* This RB is in AM mode. And its reference config is found in OC.
        */
        if ((new_entity_idx = rrcllc_get_1st_hole_for_rlc_am()) == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG1_ERROR("Can't find space for new rb %d", rb_id);
          return FAILURE;
        }
        ordered_config_ptr->am_config[new_entity_idx].rlc_ul_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->common_config.rb_id = rb_id;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan++;
        ordered_config_ptr->am_config[new_entity_idx].rlc_dl_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan++;
        ordered_config_ptr->rlc_am_parms.nentity++;
        if((ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan >
                                                       UE_MAX_UL_AM_DATA_CHANNEL) || 
           (ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan >
                                                       UE_MAX_DL_AM_DATA_CHANNEL) ||
           (ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY))
        {
          ERR("Too many UL/DL AM channels or entities [DL,UL,entities] [%d,%d,%d]",
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.nentity);
          return FAILURE;
        }
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      if ((entity_idx = get_ul_um_index(temp_rb_id))      
        != UE_MAX_UL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_ul_um_info(rb_id, TRUE) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      if ((entity_idx = get_dl_um_index(temp_rb_id, ordered_config_ptr))
        != UE_MAX_DL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_dl_um_info(rb_id, TRUE, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      /* SRBs cannot be TM.  If we haven't found it by now, we have
      * a problem. */
      if (logch_mode == UE_MODE_MAX_NUM)
      {
        WRRC_MSG1_ERROR("same_as_RB %d not found in msg or OC",
          ie_ptr->rlc_InfoChoice.u.same_as_RB);
        return FAILURE;
      }
    }
  } /* end RLC choice == Same as RB */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r6_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", rb_id);
      return FAILURE;
    }
  }  


  /* Now we have either the RLC Info explicitly meant for this RB OR
   * the RLC Info that is the same as another RB.  If tmp_ie_ptr is NULL,
   * then we've already copied the RLC Info from our internal database.
   */
  if (temp_ie_ptr != NULL)
  {
    /* Get the RLC mode */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r6_rlc_Info) && 
         ((logch_mode = get_rlc_mode_r6(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
       )
    {
      return FAILURE;
    }
      
    /* Get the RLC info */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r6_rlc_Info) && 
         (get_rlc_info_r6(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, rb_id, TRUE) == FAILURE))
    {
      return FAILURE;
    }
    if (temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r6_rlc_Info)
    {
      one_sided_reest_present=TRUE;
      one_sided_reest=temp_ie_ptr->rlc_InfoChoice.u.rlc_Info->rlc_OneSidedReEst;
    }
  }
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r6(&ie_ptr->rb_MappingInfo, rb_id, logch_mode
      ,one_sided_reest_present,one_sided_reest,TRUE) == FAILURE)
  {
    return FAILURE;
  }
  
  if(FAILURE == rrcllc_validate_srb_id(rb_id))
  {
    return FAILURE;
  }

  /* everything is configured */
  return SUCCESS;

}

/*==========================================================
FUNCTION  : get_rb_info_to_reconfig_r6()

DESCRIPTION : Get Radio Bearer Information to reconfig.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_reconfig_r6
(
rrc_RB_InformationReconfig_r6 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  boolean one_sided_reest_present=FALSE;
  boolean one_sided_reest=FALSE;
#ifdef FEATURE_MAC_I
  uint8 entity_idx=UE_MAX_AM_ENTITY;
#endif /* FEATURE_MAC_I */
  
  /* Check to see if the RLC info is present */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,rlc_Info)
    && ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode)) ||
    (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode))))
  {
    /* Get the RLC mode */
    if ((logch_mode = get_rlc_mode_r6(&ie_ptr->rlc_Info)) == UE_MODE_MAX_NUM)
    {
      return FAILURE;
    }
    
    /* Save off the RLC Info */
    if (get_rlc_info_r6(&ie_ptr->rlc_Info, ie_ptr->rb_Identity, FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end RLC Info Check */
  

#ifdef FEATURE_MAC_I
  /*Copy rlc one side re-establishment flag to OC*/
if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,rlc_Info)) && (ie_ptr->rb_Identity >= DCCH_AM_RADIO_BEARER_ID))
{
  if (ie_ptr->rlc_Info.rlc_OneSidedReEst == TRUE)
  {
  
    entity_idx = get_am_index_wo_f3(ie_ptr->rb_Identity);
    if (entity_idx != UE_MAX_AM_ENTITY)
    {
      ordered_config_ptr->rlc_one_sided_reest[entity_idx] = TRUE;
    }
  }
}
#endif /* FEATURE_MAC_I */



  /* Save off the RB Mapping info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,rb_MappingInfo))
  {
    /* Remove any existing mapping for the RB */
    unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);
    if(ie_ptr->m.rlc_InfoPresent)
    {
      one_sided_reest_present = TRUE;
      one_sided_reest = ie_ptr->rlc_Info.rlc_OneSidedReEst;
    }
    
    if (get_rb_mapping_info_r6(&ie_ptr->rb_MappingInfo, (rb_id_type)ie_ptr->rb_Identity, logch_mode
         ,one_sided_reest_present,one_sided_reest,FALSE ) == FAILURE)
    {
      return FAILURE;
    }
  } /* end of RB Mapping info check */
  
#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,pdcp_Info))
  {
    if (rrc_pdcp_enabled)
    {
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,
                                      &ie_ptr->pdcp_Info.pdcp_Info, 
                                      ie_ptr->rb_Identity, 
                                      CPDCP_RECONFIG) == FAILURE)
      {
        return FAILURE;
      }
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
    {
      WRRC_MSG0_ERROR("PDCP Info not supported");
      return FAILURE;
    }
  }
#else

  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r6,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }

#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_reconfig_r6() */

/*==========================================================
FUNCTION  : get_rb_info_affected_r6()

DESCRIPTION : Get Radio Bearer Information to be affected.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_affected_r6
(
rrc_RB_InformationAffected_r6 *ie_ptr
)
{
  /* Remove any existing mapping for the RB */
  unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);


  /* Get the RB Mapping info */
  return(get_rb_mapping_info_r6(&ie_ptr->rb_MappingInfo, 
                                ie_ptr->rb_Identity, 
                                UE_MODE_MAX_NUM
                                ,FALSE,FALSE,FALSE));

}  /* end get_rb_info_affected_r6() */

/*==========================================================
FUNCTION  : get_rb_info_to_setup_r6()

DESCRIPTION : Get Radio Bearer Information to setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_setup_r6
(
  rrc_RB_InformationSetup_r6       *ie_ptr,
#ifdef FEATURE_UMTS_PDCP
  rrc_CN_DomainIdentity cn_Domain,
#endif /* FEATURE_UMTS_PDCP*/
  rrc_RB_InformationSetupList_r6   * rb_setup_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type    logch_mode = UE_MODE_MAX_NUM;
  
  /*initialize a counter to check rb count*/
  uint8                         rb_cnt  = 0;
  
  
  /*place holder for rb_id we get originally*/
  rrc_RB_Identity  local_rb_id = ie_ptr->rb_Identity;
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_RB_InformationSetup_r6     *temp_ie_ptr = ie_ptr;

#ifdef FEATURE_UMTS_PDCP
  uint8 idx = 0;
#endif /* FEATURE_UMTS_PDCP */  
  
  boolean  one_sided_reest_present = FALSE;
  boolean  one_sided_reest = FALSE;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the rb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r6_same_as_RB))
  {
    while ((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
    {
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == 
        rb_setup_ptr->elem[rb_cnt].rb_Identity)
      {
        temp_ie_ptr = &(rb_setup_ptr->elem[rb_cnt]);
        break;
      }
  
      rb_cnt++;
    } /* end RB per RAB setup */
    
    if ((rb_setup_ptr->n == rb_cnt) || (rb_cnt >= MAX_RB_PER_RAB))
    {
       WRRC_MSG0_ERROR("Unable to match rb_id to same_as_RB"); 
      return FAILURE;
    }
  } /* if RLC info choice is "Same as RB" type */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r6_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r6,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", local_rb_id);
      return FAILURE;
    }
  }  
  
    /* Now we have either the RLC Info explicitly meant for this RB OR
    * the RLC Info that is the same as another RB.Get the RLC mode */
  
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r6_rlc_Info) && 
       ((logch_mode = get_rlc_mode_r6(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
     )
  {
    return FAILURE;
  }
  
  /* Save off the RLC Info */
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r6_rlc_Info) &&
      (get_rlc_info_r6(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, 
       local_rb_id, TRUE) == FAILURE))
  {
    return FAILURE;
  }
  
  if (temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r6_rlc_Info)
  {
    one_sided_reest_present = TRUE;
    one_sided_reest = temp_ie_ptr->rlc_InfoChoice.u.rlc_Info->rlc_OneSidedReEst;
  }

  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r6(&ie_ptr->rb_MappingInfo, local_rb_id, logch_mode
       ,one_sided_reest_present,one_sided_reest,TRUE) == FAILURE)
  {
    return FAILURE;
  }

#ifdef FEATURE_UMTS_PDCP

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r6,pdcp_Info))
  {
    if (rrc_pdcp_enabled)  
    { 
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,
                                      &ie_ptr->pdcp_Info, 
                                      local_rb_id, 
                                      CPDCP_SETUP) == FAILURE)
      {
        return FAILURE;
      }
      return SUCCESS;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r6,pdcp_Info)) &&
            ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
            (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
            ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
            (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,
            rrc_LosslessSRNS_RelocSupport_supported)))))
    {
      WRRC_MSG0_ERROR("PDCP NV not enabled.  PDCP info not handled");
      return FAILURE;
    }
  }

  /* if fall-thru happens, PDCP layer has to be setup in transparent [pass thru] mode */
  if (cn_Domain == rrc_CN_DomainIdentity_ps_domain) 
  {
    WRRC_MSG1_MED("Setting up PDCP in transparent mode rb_id %d",local_rb_id);
    /*Get the next free index in pdcp_config_req */
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use == FALSE) 
      {
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id = local_rb_id;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.is_mcast_call = FALSE;
        ordered_config_ptr->pdcp_parms.num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
    /* CPDCP action will be setup, but pdcp info will be null */
    //ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
  }
#else
  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r6,pdcp_Info)) &&
            ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
            (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
            ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
            (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,
            rrc_LosslessSRNS_RelocSupport_supported)))))
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }
#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_setup_r6() */


/*============================================================================
FUNCTION: rrcllc_process_srb_info_setup_list_r6()

DESCRIPTION:
  This function process IE rrc_SRB_InformationSetupList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_srb_info_setup_list_r6
(
  OSS_UINT32 srb_info_setup_list_present,
  rrc_SRB_InformationSetupList_r6 * srb_setup_r6_ptr
)
{
  rrc_SRB_InformationSetup_r6 * srb5_setup_ptr =NULL;
  rrc_RB_Identity                          srb_id = RRCLLC_INVALID_RB_ID;

  if (srb_info_setup_list_present)
  {
    uint8                           srb_cnt =0;
    rrc_RB_Identity                          rb_id = 0;

  
  
    
    while((srb_setup_r6_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if(srb_setup_r6_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_r6_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        rb_id++;
      }
      
      /* Save off the RB Mapping info for each SRB */
      if(get_srb_info_to_setup_r6(
        &srb_setup_r6_ptr->elem[srb_cnt], 
        (srb_setup_r6_ptr), 
        rb_id)
        == FAILURE)
      {
        return FAILURE;
      }
      if(rb_id > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        /* Pick that pointer that has RB Id > 4 & 
         assume it to be SRB#5 pointer for the time being */

        WRRC_MSG0_HIGH("Probable SRB#5 IE in the message .. processing");
        srb5_setup_ptr = &srb_setup_r6_ptr->elem[srb_cnt];

        srb_id = rb_id;
      }

      srb_cnt++;
    } /* end SRB setup */
    /* We are just validating one more SRB, incase 
       SRB#5 is given by n/w */
    if(srb5_setup_ptr != NULL)
    {

      if(rrcllc_update_oc_with_srb5_rel6_ie(srb5_setup_ptr, srb_id) == FAILURE)
      {
        WRRC_MSG0_ERROR("Rejecting SRB#5 configuration given by the network");
        return FAILURE;
      }
    }
#ifdef FEATURE_CHANGE_SRB4_PRIRORITY
    rrcllc_check_and_set_srb3_srb4_priority(); 
#endif
  }

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_process_rab_info_setup_list_r6()

DESCRIPTION:
  This function process IE rrc_RB_InformationAffectedList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rab_info_setup_list_r6
(
  OSS_UINT32 rab_info_setup_list_present,
  rrc_RAB_InformationSetupList_r6 * rab_setup_ptr
)
{
  if(rab_info_setup_list_present)
  {
    /* Local count of # of RAB's & RB's to setup */
    uint32                             rab_cnt = 0;
    uint32                             rb_cnt  = 0;    
    /* local RAB to setup list pointer */
   
    /* local RB per RAB to setup list pointer */
    rrc_RB_InformationSetupList_r6     * rb_setup_ptr;
    /* Local varible to store head pointer */
   

    
    while((rab_setup_ptr->n > rab_cnt)&& (rab_cnt < MAX_RAB_TO_SETUP))
    {
      rb_setup_ptr  = &rab_setup_ptr->elem[rab_cnt].rb_InformationSetupList;
      rb_cnt =0;
            
      while((rb_setup_ptr->n > rb_cnt ) && (rb_cnt < MAX_RB_PER_RAB))
      {
        if(get_rb_info_to_setup_r6(
           &rb_setup_ptr->elem[rb_cnt],
#ifdef FEATURE_UMTS_PDCP
           rab_setup_ptr->elem[rab_cnt].rab_Info.cn_DomainIdentity,
#endif /* FEATURE_UMTS_PDCP */           
           rb_setup_ptr) == FAILURE)
        {
          return FAILURE;
        }
        
        rb_cnt++;		
      } /* end RB per RAB setup */
      rab_cnt++;
    } /* end rab setup */
  } /* end RAB setup */

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_process_rb_info_reconfig_list_r6()

DESCRIPTION:
  This function process IE rrc_RB_InformationReconfigList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_reconfig_list_r6
(
  uint32 rb_info_reconfig_list_present,
  rrc_RB_InformationReconfigList_r6 * rb_reconfig_ptr
)  
{

  if (rb_info_reconfig_list_present)
  {
  
    uint32 idx = 0;
   
    idx = 0;
    while((rb_reconfig_ptr->n > idx) && (idx < MAX_RB))
    {
      if(get_rb_info_to_reconfig_r6(&rb_reconfig_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
    
      idx++;
    } /* end RB reconfig */
  }

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_process_rb_info_affected_list_r6()

DESCRIPTION:
  This function process IE rrc_RB_InformationAffectedList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_affected_list_r6
(
  OSS_UINT32 rb_info_affected_list_present,
  rrc_RB_InformationAffectedList_r6 * rb_affected_ptr
)  
{
  /* Get the affected RB list if present */
  if(rb_info_affected_list_present)
  {
   
    uint32 idx = 0;
    
    while((rb_affected_ptr->n > idx) && (idx < MAX_RB))
    {
      if(get_rb_info_affected_r6(&rb_affected_ptr->elem[idx]) == FAILURE)
      {
        return (FAILURE);
      }
      
      idx++;
    }
  } /* end Affected RB */

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_get_predefined_rab_info

DESCRIPTION:

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_get_predefined_rab_info
(
  rrc_RAB_Info_Post rab_Info,
  rrc_PredefinedRB_Configuration  *predef_rb, 
  rrc_RAB_InformationSetupList  *rab_setup_ptr
)  
{
  rab_setup_ptr->n = 1;
  memset (&rab_setup_ptr->elem[0].rab_Info.m, 0, sizeof(rab_setup_ptr->elem[0].rab_Info.m));
    rab_setup_ptr->elem[0].rab_Info.cn_DomainIdentity =
      rab_Info.cn_DomainIdentity;

    if(RRC_MSG_COMMON_BITMASK_IE(rab_Info,rrc_RAB_Info_Post,nas_Synchronisation_Indicator))
    {
      rab_setup_ptr->elem[0].rab_Info.nas_Synchronisation_Indicator =
        rab_Info.nas_Synchronisation_Indicator;
      rab_setup_ptr->elem[0].rab_Info.m.nas_Synchronisation_IndicatorPresent =1;
    }
    rab_setup_ptr->elem[0].rab_Info.rab_Identity = rab_Info.rab_Identity;
    rab_setup_ptr->elem[0].rab_Info.re_EstablishmentTimer =
      predef_rb->re_EstablishmentTimer;
    rab_setup_ptr->elem[0].rb_InformationSetupList =
      predef_rb->rb_InformationList;

  return (SUCCESS);
}


/*==========================================================
FUNCTION  : get_srb_info_to_setup()

DESCRIPTION : Get Signalling Radio Bearer Information to 
              setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_srb_info_to_setup
(
  rrc_SRB_InformationSetup     *ie_ptr,
  rrc_SRB_InformationSetupList2 * srb_setup_ptr,
  rrc_RB_Identity                   rb_id
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;

  /* Local count of # of SRB's to setup */
  uint8                           srb_cnt = 0;

  /* temp rb id which we shall compute if rb_id is not part of
   * srb_setup_ptr when we loop thru the list to get a match
   */
  rrc_RB_Identity                      temp_rb_id = 0;

 
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
   * RLC Info for another RB, IE Pointer is made to point to 
   * SRB Info Setup for the same_as RB. 
   * For RB Mapping info, IE Pointer should always point to
   * SRB Info Setup for the RB that came as a parameter.
   */
  rrc_SRB_InformationSetup     *temp_ie_ptr = ie_ptr;
  
  /* Entity indexes in case we need to look up RLC info from
   * our internal database instead of processing an IE.
   */
  uint8 entity_idx, new_entity_idx;

  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
   *rlc_Info for the rb_id specified.  Walk thru the srb_setup_head_ptr
   *list until we find a rb_id that matches
   */
  if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_same_as_RB))
  {
    while((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {

      /* Check to see if the RB ID is there ... */
      if(srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        temp_rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }

      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        temp_rb_id++;
      }

      /*check if the rb_id matches*/
      if(ie_ptr->rlc_InfoChoice.u.same_as_RB == temp_rb_id)
      {
        temp_ie_ptr = &(srb_setup_ptr->elem[srb_cnt]);
        break;
      }

      
      srb_cnt++;
    } /* end while... */

    if((srb_setup_ptr->n == srb_cnt) || (srb_cnt >= UE_MAX_SRB_SETUP))
    {
      WRRC_MSG1_HIGH("same_as_RB %d not in msg, lookup req'd",
               ie_ptr->rlc_InfoChoice.u.same_as_RB);
      temp_ie_ptr = NULL;
      temp_rb_id = ie_ptr->rlc_InfoChoice.u.same_as_RB;
      
      if((entity_idx = get_am_index(temp_rb_id)) != UE_MAX_AM_ENTITY)
      {
        /* This RB is in AM mode. And its reference config is found in OC.
         */
        if ((new_entity_idx = rrcllc_get_1st_hole_for_rlc_am()) == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG1_ERROR("Can't find space for new rb %d", rb_id);
          return FAILURE;
        }
        ordered_config_ptr->am_config[new_entity_idx].rlc_ul_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->common_config.rb_id = rb_id;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan++;
        ordered_config_ptr->am_config[new_entity_idx].rlc_dl_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan++;
        ordered_config_ptr->rlc_am_parms.nentity++;
        if((ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan >
                                                           UE_MAX_UL_AM_DATA_CHANNEL) || 
           (ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan >
                                                           UE_MAX_DL_AM_DATA_CHANNEL) ||
           (ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY))
        {
          ERR("Too many UL/DL AM channels or entities [DL,UL,entities] [%d,%d,%d]",
                ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
                ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
                ordered_config_ptr->rlc_am_parms.nentity);
          return FAILURE;
        }
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
       
      if((entity_idx = get_ul_um_index(temp_rb_id))
              != UE_MAX_UL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_ul_um_info(rb_id, TRUE) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      if((entity_idx = get_dl_um_index(temp_rb_id, ordered_config_ptr))
              != UE_MAX_DL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_dl_um_info(rb_id, TRUE, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      /* SRBs cannot be TM.  If we haven't found it by now, we have
       * a problem. */
      if(logch_mode == UE_MODE_MAX_NUM)
      {
        WRRC_MSG1_ERROR("same_as_RB %d not found in msg or OC",
            ie_ptr->rlc_InfoChoice.u.same_as_RB);
        return FAILURE;
      }
    }
  } /* end RLC t == Same as RB */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", rb_id);
      return FAILURE;
    }
  }  


  /* Now we have either the RLC Info explicitly meant for this RB OR
   * the RLC Info that is the same as another RB.  If tmp_ie_ptr is NULL,
   * then we've already copied the RLC Info from our internal database.
   */
  if(temp_ie_ptr != NULL)
  {
    /* Get the RLC mode */
    if((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_rlc_Info) && 
        ((logch_mode = get_rlc_mode(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
      )
    {
      return FAILURE;
    }

    /* Get the RLC info */
    if((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_rlc_Info) && 
        (get_rlc_info(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, rb_id, TRUE) == FAILURE))
    {
      return FAILURE;
    }
  }

  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if(get_rb_mapping_info(&ie_ptr->rb_MappingInfo, rb_id, logch_mode) == FAILURE)
  {
    return FAILURE;
  }
#ifdef FEATURE_CHANGE_SRB4_PRIRORITY
  rrcllc_check_and_set_srb3_srb4_priority(); 
#endif

  if(FAILURE == rrcllc_validate_srb_id(rb_id))
  {
    return FAILURE;
  }

  /* everything is configured */
  return SUCCESS;

} /* end get_srb_info_to_setup() */

/*==========================================================
FUNCTION  : get_rb_info_to_setup()

DESCRIPTION : Get Radio Bearer Information to setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_setup
(
  rrc_RB_InformationSetup       *ie_ptr,
#ifdef FEATURE_UMTS_PDCP
  rrc_CN_DomainIdentity cn_Domain,
#endif /* FEATURE_UMTS_PDCP*/
  rrc_RB_InformationSetupList   * rb_setup_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type    logch_mode = UE_MODE_MAX_NUM;
  
  /*initialize a counter to check rb count*/
  uint8                         rb_cnt  = 0;
  
  /*place holder for rb_id we get originally*/
  rrc_RB_Identity      local_rb_id = ie_ptr->rb_Identity;

  /* Local IE pointer - if RLC Info for this RB is the same as 
   * RLC Info for another RB, IE Pointer is made to point to 
   * SRB Info Setup for the same_as RB. 
   * For RB Mapping info, IE Pointer should always point to
   * SRB Info Setup for the RB that came as a parameter.
   */
  rrc_RB_InformationSetup     *temp_ie_ptr = ie_ptr;

#ifdef FEATURE_UMTS_PDCP
  uint8 idx = 0;
#endif /* FEATURE_UMTS_PDCP */

  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
   *rlc_Info for the rb_id specified.  Walk thru the rb_setup_head_ptr
   *list until we find a rb_id that matches
   */
  if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_same_as_RB))
  {
    while((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
    {
      if(ie_ptr->rlc_InfoChoice.u.same_as_RB == 
         rb_setup_ptr->elem[rb_cnt].rb_Identity)
      {
        temp_ie_ptr = &(rb_setup_ptr->elem[rb_cnt]);
        break;
      }
     
      rb_cnt++;
    } /* end RB per RAB setup */

    if((rb_setup_ptr->n == rb_cnt) || (rb_cnt >= MAX_RB_PER_RAB))
    {
       WRRC_MSG0_ERROR("Unable to match rb_id to same_as_RB"); 
      return FAILURE;
    }
  } /* if RLC info t is "Same as RB" type */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", local_rb_id);
      return FAILURE;
    }
  
  }  

  /* Now we have either the RLC Info explicitly meant for this RB OR
   * the RLC Info that is the same as another RB.
   */
  /* Get the RLC mode */
  if((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_rlc_Info) && 
      ((logch_mode = get_rlc_mode(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
    )
  {
    return FAILURE;
  }

  /* Save off the RLC Info */
  if((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_rlc_Info) && 
      (get_rlc_info(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, 
                  local_rb_id, TRUE) == FAILURE))
  {
    return FAILURE;
  }

  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if(get_rb_mapping_info(&ie_ptr->rb_MappingInfo, local_rb_id, logch_mode)
     == FAILURE)
  {
    return FAILURE;
  }
  

#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup,pdcp_Info))
  {
    if (rrc_pdcp_enabled)
    {
      WRRC_MSG0_HIGH("PDCP info present in rb setup");
      if (get_pdcp_info(&ie_ptr->pdcp_Info, local_rb_id, CPDCP_SETUP) == FAILURE)
      {
        return FAILURE;
      }
    }
    else  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup,pdcp_Info)) &&
      ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
       (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info,headerCompressionInfoList)) ||
       ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info,losslessSRNS_RelocSupport)) &&
        (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported))))) 
    {
      WRRC_MSG0_ERROR("PDCP nv turned off.  Not handling pdcp");
      return FAILURE;
    }
  }
  /* PDCP layer has to be setup in transparent [pass thru] mode */
  if (cn_Domain == rrc_CN_DomainIdentity_ps_domain) 
  {
    WRRC_MSG1_MED("Setting up PDCP in transparent mode rb_id %d",local_rb_id);
    /*Get the next free index in pdcp_config_req */
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use == FALSE) 
      {
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id = local_rb_id;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.is_mcast_call = FALSE;

        ordered_config_ptr->pdcp_parms.num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
    /* CPDCP action will be setup, but pdcp info will be null for transparent mode */
  }
#else
  /* Ignore the PDCP info for now */
  /********************************/

  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup,pdcp_Info)) &&
      ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
       (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info,headerCompressionInfoList)) ||
       ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info,losslessSRNS_RelocSupport)) &&
        (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,rrc_LosslessSRNS_RelocSupport_supported)))))                
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }
#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_setup() */

/*==========================================================
FUNCTION  : get_rb_info_to_relase()

DESCRIPTION : Get Radio Bearer Information to release.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_release
(
  rrc_RB_Identity *ie_ptr
)
{

  if(*ie_ptr < DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    WRRC_MSG1_ERROR("RB ID %d cannot be released", *ie_ptr);
    return FAILURE;
  }

  unmap_rb(*ie_ptr, OC_ONLY);

  MSG_MED("Removed RB Mapping Info for RB%d", *ie_ptr, 0, 0);

  /* Now delete the AM Info for this RB (if this is an AM RB) 
  * in the RLC Info.
  */
  return SUCCESS;
} /* end get_rb_info_to_release() */


/*==========================================================
FUNCTION  : rrcllc_delete_rlc_am_info()

DESCRIPTION : Deletes RLC AM info and re-arranges the RLC
              AM Info array to cover any holes.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_am_info(rrc_RB_Identity rb_id)
{
  uint8  count;
  
  for(count = 0; count < UE_MAX_AM_ENTITY; count ++)
  {
    if((ordered_config_ptr->rlc_am_parms.am_info[count]->common_config.rb_id 
        == rb_id) && 
       (ordered_config_ptr->rlc_am_parms.am_info[count]->dl_nchan &&
          ordered_config_ptr->rlc_am_parms.am_info[count]->ul_nchan)
      )
    {
      /* Reset this RLC AM entity memory and leave it there for the 
         next adding RLC AM entity.
       */
      memset(ordered_config_ptr->rlc_am_parms.am_info[count], 
             0,
             sizeof(rlc_am_config_type)); 
      WRRC_MSG2_HIGH("Release/reset RLC AM, RB%d, config%d", rb_id, count);
      /* This is acceptable since we are breaking immediately after */
      ordered_config_ptr->rlc_am_parms.nentity --;
      rrcllc_init_am_dl_rlc_size(ordered_config_ptr, count);
      break;
    }
  }  /* for (...) */

  if (count == UE_MAX_AM_ENTITY)
  {
    WRRC_MSG1_ERROR("can't locate rb_id %d", rb_id);
  }
}  /* rrcllc_delete_rlc_am_info */

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_dl_tm_info()

DESCRIPTION : Decrements the count of DL TM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_dl_tm_info(rlc_lc_id_type lc_id)
{
  ordered_config_type *config_ptr;
  /* Use Current Config when in Solo BCH Operation */
  if(rrcllc_is_cmd_under_process_for_solo_bch_operation())
  {
    config_ptr = current_config_ptr;
  }
  else
  {
    config_ptr = ordered_config_ptr;
  }

  /* Just decrement the count now since other parameters are not stored/used */
  if (config_ptr->rlc_dl_tm_parms.nchan > 0)
  {
    config_ptr->rlc_dl_tm_parms.nchan --;
  }
  else
  {
    WRRC_MSG0_ERROR("No DL TM channels to delete");
  }
} /*lint !e715 */

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_dl_um_info()

DESCRIPTION : Deletes the UM RB from OC and decrements the count 
              of DL UM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_dl_um_info(rlc_lc_id_type lc_id)
{
  uint8 count;
 
  for(count = 0; count < UE_MAX_DL_UM_CHANNEL; count ++)
  {
    if((ordered_config_ptr->rlc_dl_um_parms.chan_info[count].lc_id == lc_id) && 
        (ordered_config_ptr->rlc_dl_um_parms.chan_info[count].rb_id != RRCLLC_INVALID_RB_ID))
    {
      /* Reset this RLC DL UM entity memory and leave it there for the 
         next adding RLC DL UM entity.
       */
      memset(&ordered_config_ptr->rlc_dl_um_parms.chan_info[count], 
             0, sizeof(rrc_rlc_dl_um_channel_config_type)); 
      ordered_config_ptr->rlc_dl_um_parms.nchan --;
      ordered_config_ptr->rlc_dl_um_parms.chan_info[count].rb_id = RRCLLC_INVALID_RB_ID;

      WRRC_MSG2_HIGH("Release/reset RLC DL UM for LC id %d, config%d", lc_id, count);
      break;
    }
  }  /* for (...) */

  if (count == UE_MAX_DL_UM_CHANNEL)
  {
    WRRC_MSG1_ERROR("can't locate lc_id %d", lc_id);
  }
} /*lint !e715 */

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_ul_tm_info()

DESCRIPTION : Decrements the count of UL TM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_ul_tm_info(rlc_lc_id_type lc_id)
{
  /* Just decrement the count now since other parameters are not stored/used */
  if (ordered_config_ptr->rlc_ul_tm_parms.nchan > 0)
  {
    ordered_config_ptr->rlc_ul_tm_parms.nchan --;
    WRRC_MSG1_MED("nchan: UL TM %d after delete",
        ordered_config_ptr->rlc_ul_tm_parms.nchan);
  }
  else
  {
    WRRC_MSG0_ERROR("No UL TM channels to delete");
  }
} /*lint !e715 */

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_ul_um_info()

DESCRIPTION : Deletes the UM RB and decrements the count of 
              UL UM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_ul_um_info(rlc_lc_id_type lc_id)
{
  uint8 count;

  for(count = 0; count < UE_MAX_UL_UM_CHANNEL; count ++)
  {
    if((ordered_config_ptr->rlc_ul_um_parms.chan_info[count].lc_id == lc_id) && 
        (ordered_config_ptr->rlc_ul_um_parms.chan_info[count].rb_id != RRCLLC_INVALID_RB_ID))
    {
      /* Reset this RLC UL UM entity memory and leave it there for the 
         next adding RLC UL UM entity.
       */
      memset(&ordered_config_ptr->rlc_ul_um_parms.chan_info[count], 
             0, sizeof(rrc_rlc_ul_um_channel_config_type)); 
      ordered_config_ptr->rlc_ul_um_parms.nchan --;
      ordered_config_ptr->rlc_ul_um_parms.chan_info[count].rb_id = RRCLLC_INVALID_RB_ID;

      WRRC_MSG2_HIGH("Release/reset RLC UL UM for LC id %d, config%d", lc_id, count);
      break;
    }
  }  /* for (...) */

  if (count == UE_MAX_UL_UM_CHANNEL)
  {
    WRRC_MSG1_ERROR("can't locate lc_id %d", lc_id);
  }
} /*lint !e715 */

/*==========================================================
FUNCTION  : get_rb_info_affected()

DESCRIPTION : Get Radio Bearer Information to be affected.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_affected
(
  rrc_RB_InformationAffected *ie_ptr
)
{
  /* Remove any existing mapping for the RB */
  unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);

  /* Get the RB Mapping info */
  return (get_rb_mapping_info(&ie_ptr->rb_MappingInfo, 
          ie_ptr->rb_Identity, 
          UE_MODE_MAX_NUM));

} /* end get_rb_info_affected() */

/*==========================================================
FUNCTION  : get_rb_info_affected_r5()

DESCRIPTION : Get Radio Bearer Information to be affected.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_affected_r5
(
rrc_RB_InformationAffected_r5 *ie_ptr
)
{
  /* Remove any existing mapping for the RB */
  unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);


  /* Get the RB Mapping info */
  return(get_rb_mapping_info_r5(&ie_ptr->rb_MappingInfo, 
                                ie_ptr->rb_Identity, 
                                UE_MODE_MAX_NUM));

} /* end get_rb_info_affected_r5() */



/*==========================================================
FUNCTION  : get_rb_info_to_reconfig()

DESCRIPTION : Get Radio Bearer Information to reconfig.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_reconfig
(
  rrc_RB_InformationReconfig *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;

  /* Check to see if the RLC info is present */
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,rlc_Info))
     && ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info,ul_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info,dl_RLC_Mode))))
  {
    /* Get the RLC mode */
    if((logch_mode = get_rlc_mode(&ie_ptr->rlc_Info)) == UE_MODE_MAX_NUM)
    {
      return FAILURE;
    }
  
    /* Save off the RLC Info */
    if(get_rlc_info(&ie_ptr->rlc_Info, ie_ptr->rb_Identity, FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end RLC Info Check */

  /* Save off the RB Mapping info */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,rb_MappingInfo))
  {
    /* Remove any existing mapping for the RB */
    unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);

    if(get_rb_mapping_info(&ie_ptr->rb_MappingInfo, ie_ptr->rb_Identity, logch_mode)
      == FAILURE)
    {
      return FAILURE;
    }
  } /* end of RB Mapping info check */
  
  /* Check to see if the RB supports stop/continue */
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,rb_StopContinue))
  {
    WRRC_MSG0_HIGH("RB Stop/Continue IE recvd");
  }

#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,pdcp_Info))
  {
  if (rrc_pdcp_enabled)
  { 
    WRRC_MSG0_HIGH("PDPC info present in rb setup");
    if (get_pdcp_info(&ie_ptr->pdcp_Info.pdcp_Info, ie_ptr->rb_Identity, CPDCP_RECONFIG) == FAILURE)
    {
      return FAILURE;
    }
  }
  else  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }
  }
#else

  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
  {
  WRRC_MSG0_ERROR("PDCP Info not supported");
  return FAILURE;
  }
#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_reconfig() */


/*==========================================================
FUNCTION  : get_rb_info_to_reconfig_r5()

DESCRIPTION : Get Radio Bearer Information to reconfig.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_reconfig_r5
(
rrc_RB_InformationReconfig_r5 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
#ifdef FEATURE_MAC_I
  uint8 entity_idx=UE_MAX_AM_ENTITY;
#endif /* FEATURE_MAC_I */

  
  /* Check to see if the RLC info is present */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,rlc_Info))
     && ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode))))
  {
    /* Get the RLC mode */
    if ((logch_mode = get_rlc_mode_r5(&ie_ptr->rlc_Info)) == UE_MODE_MAX_NUM)
    {
      return FAILURE;
    }
    
    /* Save off the RLC Info */
    if (get_rlc_info_r5(&ie_ptr->rlc_Info, ie_ptr->rb_Identity, FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end RLC Info Check */
  
#ifdef FEATURE_MAC_I
/*Copy rlc one side re-establishment flag to OC*/
if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,rlc_Info)) && (ie_ptr->rb_Identity >= DCCH_AM_RADIO_BEARER_ID))
{
  if (ie_ptr->rlc_Info.rlc_OneSidedReEst == TRUE)
  {
    entity_idx = get_am_index_wo_f3(ie_ptr->rb_Identity);
    if (entity_idx != UE_MAX_AM_ENTITY)
    {
      ordered_config_ptr->rlc_one_sided_reest[entity_idx] = TRUE;
    }
  }
}
#endif /* FEATURE_MAC_I */



  /* Save off the RB Mapping info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,rb_MappingInfo))
  {
    /* Remove any existing mapping for the RB */
    unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);
    
    if (get_rb_mapping_info_r5(&ie_ptr->rb_MappingInfo, ie_ptr->rb_Identity, logch_mode)
      == FAILURE)
    {
      return FAILURE;
    }
  } /* end of RB Mapping info check */
  
#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,pdcp_Info))
  {
    if (rrc_pdcp_enabled)
    {
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,
                                      &ie_ptr->pdcp_Info.pdcp_Info, 
                                      ie_ptr->rb_Identity, 
                                      CPDCP_RECONFIG) == FAILURE)
      {
        return FAILURE;
      }
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
    {
      WRRC_MSG0_ERROR("PDCP Info not supported");
      return FAILURE;
    }
  }
#else
  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }

#endif /* FEATURE_UMTS_PDCP */
  /* Check to see if the RB supports stop/continue */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r5,rb_StopContinue))
  {
    WRRC_MSG0_HIGH("RB Stop/Continue IE specified");
  }
  
  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_reconfig_r5() */

/*==========================================================
FUNCTION  : remove_e_ul_ded_logch_entry()

DESCRIPTION : Removes the specified entry in the array of
              mac_e_ul_logch_info_s_type entries indicated
              by the supplied pointer.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: The specified entry in the provided array is
              removed, and the rest of the array is moved down
              to fill in the hole.
============================================================*/
static void remove_e_ul_ded_logch_entry
(
  mac_e_ul_logch_info_s_type *e_ul_ded_logch_array,
  uint8                         entry_to_remove
)
{
  if(entry_to_remove == MAX_DED_LOGCHAN_PER_TRCH - 1)
  {
    memset(e_ul_ded_logch_array + entry_to_remove, 0,
           sizeof(mac_e_ul_logch_info_s_type));
  }
  else
  {
    WCDMA_MEMMOVE(e_ul_ded_logch_array + entry_to_remove,
                  sizeof(mac_e_ul_logch_info_s_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove),
                  e_ul_ded_logch_array + entry_to_remove + 1,
                  sizeof(mac_e_ul_logch_info_s_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove - 1));
    
    memset(e_ul_ded_logch_array + (MAX_DED_LOGCHAN_PER_TRCH - 1), 0,
           sizeof(mac_e_ul_logch_info_s_type));
  }
}

/*==========================================================
FUNCTION  : unmap_rb()

DESCRIPTION : Remove the mapping for the specified RB if it
              exists.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: If a mapping for the specified RB is found in
              ordered_config, it is removed and the
              ded_logch_config array it was in is rearranged
              to fill the hole. This function also indicates
              if MAC Uplink or Downlink needs to be reconfigured.
============================================================*/
void unmap_rb
(
  rrc_RB_Identity rb_id,
  rrcllc_unmap_rb_e_type config_db
)
{
  uint8 trch_idx;
  uint8 logch_idx;
  uint8 ndlchan;
  uint8 pre_ndlchan = 0;

  uint32 iterations = 1;

  uint8 dflow_index = 0;
uint8 mac_ehs_queue_index =0;
  ordered_config_type   *tmp_config_ptr = NULL;

  /* start with OC */
  tmp_config_ptr = ordered_config_ptr;

  if ((config_db == OC_AND_TOC) && (transition_config.toc_usage != TOC_INVALID))
  {
    /*In case of Cell_FACH -> Cell_DCH state transition, if TOC is valid, 
    don't remove RB-mapping from TOC.*/
    if((rrc_get_state() == RRC_STATE_CELL_FACH) && 
      (rrc_ordered_state == RRC_STATE_CELL_DCH))
    {
      WRRC_MSG1_MED("Clean only OC for RB-Mapping for RB %d",rb_id);
    }
    else
    {
      WRRC_MSG0_HIGH("Need to clean both OC & TOC");
      iterations = 2;
    }
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
     
  if( config_db == OC_ONLY || config_db == OC_AND_TOC)
  {
    rrcllc_free_hanging_rb_mapping_info_index(rb_id,ALL_RB_MAPPING);
#ifdef FEATURE_WCDMA_HS_FACH
    rrcllc_remove_rb_from_hanging(rb_id);
#endif
  }
  while (iterations)
  {
    /* fach_info, dl_dch_info, dsch_info, and ul_cctrch.dch_info.dch_info are all
       arrays of size UE_MAX_TRCH, so we will iterate through them all with this
       outer loop. */
    for(trch_idx = 0; trch_idx < UE_MAX_TRCH; trch_idx++)
    {
      /* DL FACH */
      ndlchan = tmp_config_ptr->fach_info[trch_idx].ndlchan;
      for(logch_idx = 0; logch_idx < ndlchan; logch_idx++)
      {
        if(tmp_config_ptr->fach_info[trch_idx].dlchan_info[logch_idx].rb_id == rb_id)
        {
          remove_dl_ded_logch_entry(tmp_config_ptr->fach_info[trch_idx].dlchan_info,
                                    logch_idx);
          tmp_config_ptr->fach_info[trch_idx].ndlchan--;
          tmp_config_ptr->dl_fach_trch_info[trch_idx].lg_ch_cnt = 
                              tmp_config_ptr->fach_info[trch_idx].ndlchan;

          /* Here indicate that MAC Downlink need to be reconfigured */
          tmp_config_ptr->reconfig_needed.downlink_mac = TRUE;
        }
        /* No break here - all RBs are mapped to all available FACHes. */
      }

      /* DL DCH */
      ndlchan = tmp_config_ptr->dl_dch_info[trch_idx].ndlchan;
      for(logch_idx = 0; logch_idx < ndlchan; logch_idx++)
      {
        if(tmp_config_ptr->dl_dch_info[trch_idx].dlchan_info[logch_idx].rb_id == rb_id)
        {
          remove_dl_ded_logch_entry(tmp_config_ptr->dl_dch_info[trch_idx].dlchan_info,
                                    logch_idx);
          pre_ndlchan = tmp_config_ptr->dl_dch_info[trch_idx].ndlchan;
          tmp_config_ptr->dl_dch_info[trch_idx].ndlchan--;
          tmp_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt = 
                              tmp_config_ptr->dl_dch_info[trch_idx].ndlchan;

          /* When the number of logical channel mapped to this transport channel 
             going down from 2/multiple to 1, we need to update the tb_size due 
             to the change of MAC header. */
          if (pre_ndlchan > 1 && tmp_config_ptr->dl_dch_info[trch_idx].ndlchan == 1)
          {
            WRRC_MSG1_HIGH("TrCH %d Need to reduce TB size", 
                     ordered_config_ptr->dl_dch_info[trch_idx].trch_id);
            tmp_config_ptr->dl_dch_trch_info[trch_idx].lg_ch_cnt_down = TRUE;
          }
          /* Here indicate that MAC Downlink needs to be reconfigured */
          tmp_config_ptr->reconfig_needed.downlink_mac = TRUE;
          break;
        }
      }

      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) 
      {      
        for (dflow_index = 0; dflow_index < UE_MAX_MAC_D_FLOW; dflow_index++)
        {
          /* DL HSDSCH */
          ndlchan = tmp_config_ptr->mac_dflow_info[dflow_index].ndlchan;
          for (logch_idx = 0; logch_idx < ndlchan; logch_idx++)
          {
            if (tmp_config_ptr->mac_dflow_info[dflow_index].dlchan_info[logch_idx].rb_id == rb_id)
            {
              MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRCHS:Delete mapping on DFLOW Id %d RB %d,L.ChIndex %d,RLCID %d",
                                    tmp_config_ptr->mac_dflow_info[dflow_index].mac_dflow_id,
                                    rb_id,dflow_index,logch_idx);
              
              remove_dl_ded_logch_entry(tmp_config_ptr->mac_dflow_info[dflow_index].dlchan_info,
                                        logch_idx);
              
              pre_ndlchan = tmp_config_ptr->mac_dflow_info[dflow_index].ndlchan;
              
              if (pre_ndlchan > 0)
              {
                tmp_config_ptr->mac_dflow_info[dflow_index].ndlchan--;
              }
              
              /* Here indicate that MAC Downlink needs to be reconfigured */
              tmp_config_ptr->reconfig_needed.downlink_mac = TRUE;
              break;
            }
          }
        }
      }

      for (mac_ehs_queue_index = 0; mac_ehs_queue_index < UE_MAX_DL_LOGICAL_CHANNEL; mac_ehs_queue_index++)
      {
        if (tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].rb_id == rb_id)
        {
          WRRC_MSG4_MED("MACEHS:Delete mapping on Queue Id %d RB %d,Queue index %d,LC ID %d",
                             tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].queue_id,
                             rb_id,mac_ehs_queue_index,
                             tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].mac_id);
  
          tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].rb_id = RRC_INVALID_RB_ID;
          tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].rlc_id = 0;
          tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].queue_id = INVALID_QUEUE_ID;
          tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].mac_id = INVALID_LOGICAL_CHANNEL_ID;
          tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].chan_type = UE_LOGCHAN_NONE;
          tmp_config_ptr->dl_macehs_logchan_info[mac_ehs_queue_index].rlc_mode = UE_MODE_MAX_NUM;
  
          tmp_config_ptr->mac_dl_parms.ndlchan_macehs --;
  
          tmp_config_ptr->mac_ehs_lc_id_in_use[mac_ehs_queue_index] = FALSE;
          
          /* Here indicate that MAC Downlink needs to be reconfigured */
          tmp_config_ptr->reconfig_needed.downlink_mac = TRUE;
          break;
        }
      }



      /* DL DSCH  */
      ndlchan = tmp_config_ptr->dsch_info[trch_idx].ndlchan;
      for(logch_idx = 0; logch_idx <  ndlchan; logch_idx++)
      {
        if(tmp_config_ptr->dsch_info[trch_idx].dlchan_info[logch_idx].rb_id == rb_id)
        {
          remove_dl_ded_logch_entry(tmp_config_ptr->dsch_info[trch_idx].dlchan_info,
                                    logch_idx);
          tmp_config_ptr->dsch_info[trch_idx].ndlchan--;
          /* Here indicate that MAC Downlink needs to be reconfigured */
          tmp_config_ptr->reconfig_needed.downlink_mac = TRUE;

          break;
        }
      }

      /* UL DCH */
      ndlchan = tmp_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
      for(logch_idx = 0; logch_idx < ndlchan; logch_idx++)
      {
        if(tmp_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rb_id == rb_id)
        {
          remove_ul_ded_logch_entry(tmp_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info,
                                    logch_idx);
          tmp_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan--;
          tmp_config_ptr->ul_dch_trch_info[trch_idx].lg_ch_cnt = 
                              tmp_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
          /* Here indicate that MAC Uplink needs to be reconfigured */
          tmp_config_ptr->reconfig_needed.uplink_mac = TRUE;
          break;
        }
      }
    }

    for (dflow_index = 0; dflow_index < MAX_E_DCH_MAC_D_FLOW; dflow_index++)
    {
      ndlchan = tmp_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_index].num_logch;
      for(logch_idx = 0; logch_idx < ndlchan; logch_idx++)
      {
        if(tmp_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_index].e_ul_logch_info[logch_idx].rb_id == rb_id)
        {
          remove_e_ul_ded_logch_entry(tmp_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_index].
                                      e_ul_logch_info, logch_idx);
          tmp_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_index].num_logch--;
          /* Here indicate that MAC Uplink needs to be reconfigured */
          tmp_config_ptr->reconfig_needed.uplink_mac = TRUE;
          
          break;
        }
      }
    }


    /* UL RACH  */
    /* ul_cctrch.rach_info, on the other had, is just a single structure, so we
       take care of it outside the loop above. */
    ndlchan = tmp_config_ptr->ul_cctrch.rach_info.ndlchan;
    for(logch_idx = 0; logch_idx < ndlchan; logch_idx++)
    {
      if(tmp_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].rb_id == rb_id)
      {
        remove_ul_ded_logch_entry(tmp_config_ptr->ul_cctrch.rach_info.dlchan_info,
                                  logch_idx);
        tmp_config_ptr->ul_cctrch.rach_info.ndlchan--;
        for(trch_idx = 0; trch_idx < UE_MAX_TRCH; trch_idx++)
        {
          if(tmp_config_ptr->ul_rach_trch_info[trch_idx].trch_id ==
              tmp_config_ptr->ul_cctrch.rach_info.trch_id)
          {
            tmp_config_ptr->ul_rach_trch_info[trch_idx].lg_ch_cnt = 
                               tmp_config_ptr->ul_cctrch.rach_info.ndlchan;
          }
        }
        /* Here indicate that MAC Uplink needs to be reconfigured */
        tmp_config_ptr->reconfig_needed.uplink_mac = TRUE;

        break;
      }
    }

    /* Delete the RB Mapping info for the RB as stored in Cell FACH RB Mapping Info */
    /* Note: This info is stored for later use when transition to CELL_FACH is made.
     */
    /* Remove the Downlink Dedicated Logical Channel Info */
    ndlchan = tmp_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan;
    for(logch_idx = 0; logch_idx <  ndlchan; logch_idx++)
    {
      if(tmp_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].rb_id == rb_id)
      {
        remove_dl_ded_logch_entry(tmp_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info,
                                  logch_idx);
        tmp_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan --;
        break;
      }
    }

    /* Remove the Uplink Dedicated Logical Channel Info */
    ndlchan = tmp_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan;
    for(logch_idx = 0; logch_idx <  ndlchan; logch_idx++)
    {
        if(tmp_config_ptr->rach_rlc_size_restriction_info.rlc_size_list[logch_idx].rb_id == rb_id)
      {
        remove_rlc_size_list_entry(tmp_config_ptr->rach_rlc_size_restriction_info.rlc_size_list,
                                   logch_idx);
        break;
      }
    }

    for(logch_idx = 0; logch_idx <  ndlchan; logch_idx++)
    {
      if(tmp_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].rb_id == rb_id)
      {
        remove_ul_ded_logch_entry(tmp_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info,
                                  logch_idx);
        tmp_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan --;
        break;
      }
    }
    iterations --;
    rrcllc_rearrange_macehs_lc_index(tmp_config_ptr);
    /* in the next iteration clean the transition config database also */
    tmp_config_ptr = transition_config.toc_ptr;
  } /*while iterations */


} /* unmap_rb */

/*==========================================================
FUNCTION  : remove_dl_ded_logch_entry()

DESCRIPTION : Removes the specified entry in the array of
              mac_dl_ded_logch_config_type entries indicated
              by the supplied pointer.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: The specified entry in the provided array is
              removed, and the rest of the array is moved down
              to fill in the hole.
============================================================*/
void remove_dl_ded_logch_entry
(
  mac_dl_ded_logch_config_type *ded_logch_array,
  uint8                         entry_to_remove
)
{
  if(entry_to_remove == MAX_DED_LOGCHAN_PER_TRCH - 1)
  {
    memset(ded_logch_array + entry_to_remove, 0,
           sizeof(mac_dl_ded_logch_config_type));
  }
  else
  {
    WCDMA_MEMMOVE(ded_logch_array + entry_to_remove,
                  sizeof(mac_dl_ded_logch_config_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove),
                  ded_logch_array + entry_to_remove + 1,
                  sizeof(mac_dl_ded_logch_config_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove - 1));
    
    memset(ded_logch_array + (MAX_DED_LOGCHAN_PER_TRCH - 1), 0,
           sizeof(mac_dl_ded_logch_config_type));
  }
}

/*==========================================================
FUNCTION  : remove_ul_ded_logch_entry()

DESCRIPTION : Removes the specified entry in the array of
              mac_ul_ded_logch_config_type entries indicated
              by the supplied pointer.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: The specified entry in the provided array is
              removed, and the rest of the array is moved down
              to fill in the hole.
============================================================*/
void remove_ul_ded_logch_entry
(
  mac_ul_ded_logch_config_type *ded_logch_array,
  uint8                         entry_to_remove
)
{
  if(entry_to_remove == MAX_DED_LOGCHAN_PER_TRCH - 1)
  {
    memset(ded_logch_array + entry_to_remove, 0,
           sizeof(mac_ul_ded_logch_config_type));
  }
  else
  {
    WCDMA_MEMMOVE(ded_logch_array + entry_to_remove,
                  sizeof(mac_ul_ded_logch_config_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove),
                  ded_logch_array + entry_to_remove + 1,
                  sizeof(mac_ul_ded_logch_config_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove - 1));
    
    memset(ded_logch_array + (MAX_DED_LOGCHAN_PER_TRCH - 1), 0,
           sizeof(mac_ul_ded_logch_config_type));
  }
}


/*==========================================================
FUNCTION  : remove_rlc_size_list_entry()

DESCRIPTION : Removes the specified entry in the array of
              rrcllcoc_rlc_size_list_type entries indicated
              by the supplied pointer.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: The specified entry in the provided array is
              removed, and the rest of the array is moved down
              to fill in the hole.
============================================================*/
void remove_rlc_size_list_entry
(
  rrcllcoc_rlc_size_list_type *rlc_size_list_array,
  uint8                        entry_to_remove
)
{
  if(entry_to_remove == MAX_DED_LOGCHAN_PER_TRCH - 1)
  {
    rrcllc_reset_rach_rlc_size_lists(rlc_size_list_array, entry_to_remove, FALSE);
  }
  else
  {
    WCDMA_MEMMOVE(rlc_size_list_array + entry_to_remove,
                  sizeof(rrcllcoc_rlc_size_list_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove),
                  rlc_size_list_array + entry_to_remove + 1,
                  sizeof(rrcllcoc_rlc_size_list_type) * (MAX_DED_LOGCHAN_PER_TRCH - entry_to_remove - 1));
  }
}

/*==========================================================
FUNCTION  : rrcllc_check_if_rbid_present_in_chan_config_within_index()

DESCRIPTION : This function checks if the RB-Id specified 
              is already present in Chan_config within the index specified.

DEPENDENCIES: None.

RETURN VALUE: 
uint8:   It returns the index at which 
         the RB-Id is present in Chan Config else returns 
         MAX_RB.

SIDE EFFECTS: None
============================================================*/
uint8 rrcllc_check_if_rbid_present_in_chan_config_within_index
(
  rrc_channel_config_req_type * chan_config_req, 
  rrc_RB_Identity rb_id,
  uint8 count
)
{
  uint8 j;
  uint8 rb_id_index = MAX_RB;
   
  for(j=0;j<count;j++)
  {
    if(chan_config_req->rb[j].rb_id == rb_id )
    {
      rb_id_index = j;
      break;
    }
  }
  WRRC_MSG2_HIGH( "rb_id %d found in chan config at index %d",rb_id,rb_id_index);
  return rb_id_index;
}

/*==========================================================
FUNCTION  : rrcllc_check_if_rbid_present_in_chan_config()

DESCRIPTION : This function checks if the RB-Id specified 
              is already present in Chan_config.

DEPENDENCIES: None.

RETURN VALUE: 
boolean: returns TRUE if Rb Id is presrn in  Channel 
         Config Req.It also returns the index at which 
         the RB-Id is present in Chan Config else returns 
         FALSE.

SIDE EFFECTS: None
============================================================*/
uint8 rrcllc_check_if_rbid_present_in_chan_config
(
  rrc_channel_config_req_type * chan_config_req, 
  rrc_RB_Identity rb_id 
)
{
  uint8 j;
   
  for(j=0;j<chan_config_req->num_rb_to_config;j++)
  {
    if(chan_config_req->rb[j].rb_id == rb_id )
    {
      return j;
    }
  }
  WRRC_MSG1_HIGH("RB ID not present in channel config req rb_id %d ",rb_id);
  return MAX_RB;
}

/*==========================================================
FUNCTION  : rrcllc_copy_ordered_hanging_rb_mapping_to_current()

DESCRIPTION : copies the rrc_ordered_hanging_rb_mapping to rrc_current_hanging_rb_mapping
DEPENDENCIES: None.

RETURN VALUE: None                        

SIDE EFFECTS: None
============================================================*/
void rrcllc_copy_ordered_hanging_rb_mapping_to_current(void)
{
  WRRC_MSG0_HIGH("copied Standalone OC Database  to Standalone CC Database ");
  WCDMA_MEMCPY(rrc_current_hanging_rb_mapping,
               sizeof(rb_mapping_info_type) * MAX_HANGING_RB_MAPPING_INFO, 
               rrc_ordered_hanging_rb_mapping,
               sizeof(rb_mapping_info_type) * MAX_HANGING_RB_MAPPING_INFO);
  WCDMA_MEMCPY(rrc_current_hanging_rb, 
               sizeof(rb_mapping_info_type_) * MAX_HANGING_RB_MAPPING_INFO,
               rrc_ordered_hanging_rb,
               sizeof(rb_mapping_info_type_) * MAX_HANGING_RB_MAPPING_INFO);
}

/*==========================================================
FUNCTION  : rrcllc_copy_current_hanging_rb_mapping_to_orderedt()

DESCRIPTION : copies the rrc_ordered_hanging_rb_mapping to rrc_current_hanging_rb_mapping
DEPENDENCIES: None.

RETURN VALUE: None                        

SIDE EFFECTS: None
============================================================*/
void rrcllc_copy_current_hanging_rb_mapping_to_ordered(void)
{
  WRRC_MSG0_HIGH("copied Standalone CC Database  to Standalone OC Database ");
  WCDMA_MEMCPY(rrc_ordered_hanging_rb_mapping,
               sizeof(rb_mapping_info_type) * MAX_HANGING_RB_MAPPING_INFO, 
               rrc_current_hanging_rb_mapping,
               sizeof(rb_mapping_info_type) * MAX_HANGING_RB_MAPPING_INFO);
  WCDMA_MEMCPY(rrc_ordered_hanging_rb,
               sizeof(rb_mapping_info_type_) * MAX_HANGING_RB_MAPPING_INFO, 
               rrc_current_hanging_rb,
               sizeof(rb_mapping_info_type_) * MAX_HANGING_RB_MAPPING_INFO);
}

/*==========================================================
FUNCTION  : rrcllc_init_ordered_hanging_rb_mapping_info()

DESCRIPTION : initialise the rrc_ordered_hanging_rb_mapping info
DEPENDENCIES: None.

RETURN VALUE: None                        

SIDE EFFECTS: None
============================================================*/
void rrcllc_init_ordered_hanging_rb_mapping_info(void)
{
  WRRC_MSG0_MED("Init  Standalone OC Database ");
  memset(rrc_ordered_hanging_rb_mapping,0,
              (sizeof(rb_mapping_info_type)*MAX_HANGING_RB_MAPPING_INFO));

#ifdef FEATURE_WCDMA_HS_FACH
  rrcllc_init_hanging_rb();
#endif
}

/*==========================================================
FUNCTION  : rrcllc_find_free_hanging_rb_mapping_info()

DESCRIPTION : Finds a free slot in the rrc_ordered_hanging_rb_mapping database

DEPENDENCIES: None.

RETURN VALUE: SUCCESS, if it succefully finds a free slot in the Backup database
                        In this case it also, return the index of the free slot in the database
                        else FAILURE is there is no more any free slot.                   

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_find_free_hanging_rb_mapping_info
(
  uint8 *index
)
{
  uint8 i;
  for(i=0; i < MAX_HANGING_RB_MAPPING_INFO; i++)
  {
    if(rrc_ordered_hanging_rb_mapping[i].in_use == FALSE)
    {
      *index = i;
      WRRC_MSG1_MED("Allocating index %d in  Standalone OC Database",i);
      rrc_ordered_hanging_rb_mapping[i].in_use = TRUE;
      return SUCCESS;
    }
  }
  WRRC_MSG0_ERROR("Free slot unavailable in hanging rb mapping ");
  return FAILURE;
}


/*==========================================================
FUNCTION  : rrcllc_free_hanging_rb_mapping_info_index()

DESCRIPTION : this function frees the rb_mapping info for the RB-ID passed in 
                        the Rb- mapping backup info

DEPENDENCIES: None.

RETURN VALUE: None       

SIDE EFFECTS: None
============================================================*/
void rrcllc_free_hanging_rb_mapping_info_index
(
   rrc_RB_Identity rb_id, 
   rb_mapping_chan_enum_type chan_type
)
{
  uint8 i;

  for(i=0; i < MAX_HANGING_RB_MAPPING_INFO; i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE)
        && (rrc_ordered_hanging_rb_mapping[i].rb_id == rb_id)) 
    { 
      WRRC_MSG3_MED("rb_id %d is removed  from Standalone OC Database  at index %d for chan type %d",
                          rb_id,i,chan_type);
      if( chan_type == ALL_RB_MAPPING)
      {
        memset(&rrc_ordered_hanging_rb_mapping[i],0,sizeof(rb_mapping_info_type));
        rrc_ordered_hanging_rb_mapping[i].in_use = FALSE;
        return;
      }
      else
      {
        if( chan_type == UL_DCH_RB_MAPPING )
        {
          if(rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.valid == TRUE )
          {
            memset(&rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info,
                           0,sizeof(ul_rb_dch_mapping_info_type));
            rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.valid = FALSE;
          }
        }
        else if( chan_type == DL_DCH_RB_MAPPING )
        {
          if(rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.valid == TRUE )
          {
            memset(&rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info,
                          0,sizeof(dl_rb_dch_mapping_info_type));
            rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.valid = FALSE;
          }
        }
        else  if( chan_type == E_DCH_RB_MAPPING )  
        {
          if(rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.valid == TRUE )
          {
            memset(&rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace,
                          0,sizeof(ul_rb_mac_e_mapping_info_type));
            rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.valid= FALSE;
          }
        }
        else if( chan_type == MAC_D_RB_MAPPING )  
        {
          if(rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.valid == TRUE )
          {
            memset(&rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info,
                           0,sizeof(dl_rb_macd_mapping_info_type));
            rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.valid = FALSE;
          }
        }
        /* 
          * If UL DCH, DL DCH, E-DCH, MAC-D  mappings are deleted for a RB, 
          * we can free the RB from the Stadalone Database
          */
        if((rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.valid == FALSE )
             && (rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.valid == FALSE )
             &&( rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.valid == FALSE) 
             &&( rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.valid == FALSE)
   #ifdef FEATURE_WCDMA_HS_FACH
     &&( rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.rach_info.valid == FALSE)
     &&( rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.fach_map_info.valid == FALSE)
     &&( rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macehs_map_info.valid == FALSE)
   #endif
          )
        {
          WRRC_MSG2_HIGH("RB id from Standalone OC Database , and freeing  index %d  ",
                            rrc_ordered_hanging_rb_mapping[i].rb_id,i);
          memset(&rrc_ordered_hanging_rb_mapping[i],0,sizeof(rb_mapping_info_type));
           rrc_ordered_hanging_rb_mapping[i].in_use = FALSE;
        }
      }      
    }
  }
}

/*==========================================================
FUNCTION  : rrcllc_check_rb_id_exist_rb_mapping()

DESCRIPTION : this function checks if the RB_Mapping info for the RB-ID passed is present in
                       the RB-Mapping Backup info.

DEPENDENCIES: None.

RETURN VALUE: Of type boolean . Returns true if the RB-ID exists in the RB-mappign Backup info
                       else, returns FALSE

SIDE EFFECTS: None
============================================================*/
boolean rrcllc_check_rb_id_exist_rb_mapping
(
  rrc_RB_Identity rb_id , 
  uint8 *index
)
{
  uint8 i;
  for(i=0;i<MAX_HANGING_RB_MAPPING_INFO;i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE) &&
        (rrc_ordered_hanging_rb_mapping[i].rb_id == rb_id ))
    {
      *index = i;
      return TRUE;
    }
  }
  return FALSE;
}

/*==========================================================
FUNCTION  : rrcllc_backup_rbmapping_info()

DESCRIPTION : This function stores the Rb_mapping info for the transport channel id passed 
                         in the RB-mapping Backup info

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None
============================================================*/
void rrcllc_backup_rbmapping_info
(
  uint8 index,
  rb_mapping_chan_enum_type chan_type 
)
{
  uint8 idx;
  uint8 i;
  uint8 j;
   
  if( chan_type == UL_DCH_RB_MAPPING )
  {
    /* At present we consider deletion of RB, 
      * so assumption that only one RB is mapped to this Transport channel
      */
    for(i=0;i<ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].ndlchan;i++ )
    {
      if( rrcllc_check_rb_id_exist_rb_mapping(
         ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[i].rb_id,
         &idx)   == FALSE )
      {
        if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
        {
          WRRC_MSG0_ERROR("No space for RB Mapping backup for UL DCH ");
          return;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].rb_id =
         ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[i].rb_id;
      rrc_ordered_hanging_rb_mapping[idx].logch_mode =
         ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[i].rlc_mode;
   
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.valid = TRUE;
      /* Storing the UL DCH mapping info to Standalone database */
        
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id = 
         ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].trch_id;
   
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =
         ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[i].mac_id;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.mac_log_priority =
          ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[i].priority;
#if defined (FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH)

      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.size_rest_info =
          ordered_config_ptr->dch_rlc_size_restriction_info[ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].trch_id].rlc_size_list[i];
#endif
      WRRC_MSG3_MED("Backup UL DCH data for RB_id %d at  index %d for trch_id %d ",
                       rrc_ordered_hanging_rb_mapping[idx].rb_id,idx,
                       rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id);
      
    }
  }
  else  if( chan_type == DL_DCH_RB_MAPPING )
  {
      /* At present we consider deletion of RB, 
        * so assumption that only one RB is mapped to this Transport channel
        */
    for(i=0;i<ordered_config_ptr->dl_dch_info[index].ndlchan;i++ )
    {
      if( rrcllc_check_rb_id_exist_rb_mapping(
           ordered_config_ptr->dl_dch_info[index].dlchan_info[i].rb_id, &idx) == FALSE )
      {
        if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
        {
          WRRC_MSG0_ERROR("No space for RB Mapping backup for DL DCH ");
          return;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.valid = TRUE;
      /* Storing the DL DCH mapping info to Standalone database */   
      rrc_ordered_hanging_rb_mapping[idx].rb_id =
        ordered_config_ptr->dl_dch_info[index].dlchan_info[i].rb_id;
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.tr_id = 
        ordered_config_ptr->dl_dch_info[index].trch_id;
      rrc_ordered_hanging_rb_mapping[idx].logch_mode =
        ordered_config_ptr->dl_dch_info[index].dlchan_info[i].rlc_mode;
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.log_channel_id =
        ordered_config_ptr->dl_dch_info[index].dlchan_info[i].mac_id;
      MSG_HIGH("Backup DL DCH data for RB_id %d at  index %d for DL trch_id %d ",
                        rrc_ordered_hanging_rb_mapping[idx].rb_id,idx,
                       rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.tr_id);
    }
  }
  else if( chan_type == RACH_RB_MAPPING )
  {
    for(i=0;i<ordered_config_ptr->ul_cctrch.rach_info.ndlchan;i++ )
    {
      if( rrcllc_check_rb_id_exist_rb_mapping(
          ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[i].rb_id, 
           &idx) == FALSE )
      {
        if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
        {
          WRRC_MSG0_ERROR("No space for RB Mapping backup for RACH");
          return;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.rach_info.valid = TRUE;
      /* Storing the RACH mapping info to Standalone database */   
      rrc_ordered_hanging_rb_mapping[idx].rb_id =
             ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[i].rb_id;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.rach_info.tr_id= 0;
  
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.rach_info.log_channel_id =
             ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[i].mac_id;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.rach_info.mac_log_priority=
             ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[i].priority;
  
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.rach_info.size_rest_info =
             ordered_config_ptr->rach_rlc_size_restriction_info.rlc_size_list[i];
  
      MSG_HIGH("Backup RACH data for RB_id %d at  index %d for trch_id %d ",
                         rrc_ordered_hanging_rb_mapping[idx].rb_id,idx,
                         rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.rach_info.tr_id);
  
    }
  }
  else if( chan_type == FACH_RB_MAPPING )
  {
    for(i=0;i<ordered_config_ptr->fach_info[index].ndlchan;i++ )
    {
      if( rrcllc_check_rb_id_exist_rb_mapping(
           ordered_config_ptr->dl_dch_info[index].dlchan_info[i].rb_id, 
           &idx) == FALSE )
      {
        if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
        {
          WRRC_MSG0_ERROR("No space for RB Mapping backup for FACH ");
          return;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].rb_id =
          ordered_config_ptr->fach_info[index].dlchan_info[i].rb_id;
     
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.fach_map_info.valid = TRUE;
      /* Storing the FACH mapping info to Standalone database */      
         /* FACH doesnt ve a Transport Channel ID in the OTA msg */
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.fach_map_info.fach_id= 0;
    
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.fach_map_info.log_channel_id =
               ordered_config_ptr->fach_info[index].dlchan_info[i].mac_id;
      MSG_HIGH("Backup FACH data for RB_id %d at  index %d for trch_id %d ",
                        rrc_ordered_hanging_rb_mapping[idx].rb_id,idx,
                       rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.fach_map_info.fach_id);
    }
  }
  else  if(( chan_type == E_DCH_RB_MAPPING )&& (index < MAX_E_DCH_MAC_D_FLOW ))
  {
    for(i=0;i<ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].num_logch ;i++ )
    {
      if( rrcllc_check_rb_id_exist_rb_mapping(
         ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].rb_id ,
         &idx) == FALSE )
      {
        if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
        {
          WRRC_MSG0_ERROR("No space for RB Mapping backup for E- DCH ");
          return;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].rb_id = 
             ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].rb_id ;
      rrc_ordered_hanging_rb_mapping[idx].logch_mode= 
             ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].rlc_mode;
      
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.valid = TRUE;
          /* Storing the E-DCH mapping info to Standalone database */   
     
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_e_flow_id = 
         ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_mac_d_flow_id;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_log_priority =
         ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].priority;
         
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.log_channel_id=
       ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].lc_id;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.in_sched_info =
         ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].include_in_scheduling_info;
#ifdef FEATURE_MAC_I
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_pdu_type = 
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].rlc_pdu_type;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_min_pdu_size = 
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].ul_flex_min_pdu_size;					
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_max_pdu_size = 
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].ul_flex_max_pdu_size;
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_li_size = 
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].ul_li_size;
      
#endif /* FEATURE_MAC_I */

      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.num_rlc_size_ddi_info =
         ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].num_rlc_size_ddi_info;
      for(j=0;j<rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.num_rlc_size_ddi_info;j++)
      {
        rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_size_ddi_info[j] =
            ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[i].rlc_size_ddi_info[j];
      }
      WRRC_MSG3_MED("Backup MAC_E data for RB_id %d at  index %d for flow id %d ",
                        rrc_ordered_hanging_rb_mapping[idx].rb_id,idx,
                       rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_e_flow_id); 
    }
  }
  else  if( (chan_type == MAC_D_RB_MAPPING )&& (index < UE_MAX_MAC_D_FLOW ))
  {
    for(i=0;i<ordered_config_ptr->mac_dflow_info[index].ndlchan ;i++ )
    {
      if( rrcllc_check_rb_id_exist_rb_mapping(
           ordered_config_ptr->mac_dflow_info[index].dlchan_info[i].rb_id,
           &idx) == FALSE )
      {
        if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
        {
          WRRC_MSG0_ERROR("No space for RB Mapping backup for MAC-D ");
          return;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].rb_id = 
            ordered_config_ptr->mac_dflow_info[index].dlchan_info[i].rb_id;
      rrc_ordered_hanging_rb_mapping[idx].logch_mode= 
            ordered_config_ptr->mac_dflow_info[index].dlchan_info[i].rlc_mode;
    
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.macd_map_info.valid = TRUE;
      /* Storing theMAC-D mapping info to Standalone database */  
        
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.macd_map_info.mac_d_flow_id =
        ordered_config_ptr->mac_dflow_info[index].mac_dflow_id;
      rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.macd_map_info.log_channel_id =
        ordered_config_ptr->mac_dflow_info[index].dlchan_info[i].mac_id;
      MSG_HIGH("Backup MAC_Ddata for RB_id %d at  index %d for flow id %d ",
                    rrc_ordered_hanging_rb_mapping[idx].rb_id,idx,
                        rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.macd_map_info.mac_d_flow_id);
    }
  }
  else
  {
    WRRC_MSG2_HIGH("Invalid info receive for Backup, unkown chan type %d index %d ",chan_type,index);
  }
}

/*==========================================================
FUNCTION  : rrcllc_restore_ul_dch_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info from
              the RB-mapping backup info to the 
              Ordered config for UL DCh.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              return SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_restore_ul_dch_rb_mapping_info
(
  uint8  index 
)
{
  uint8 i,logch_idx;
  tr_ch_id_type trch_id;
  uecomdef_logch_mode_e_type logch_mode;
  rlc_lc_id_type rlc_lc_id;
  uecomdef_status_e_type ret_val = SUCCESS;;
 
  trch_id = ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].trch_id;
  
  for(i=0;i<MAX_HANGING_RB_MAPPING_INFO;i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE ) && 
       (rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.valid == TRUE ))
    {
      if(rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.tr_id == trch_id )
      {
       
        logch_idx = ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].ndlchan;
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].rb_id = 
            rrc_ordered_hanging_rb_mapping[i].rb_id;
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].mac_id=
           rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.log_channel_id;
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].priority =
          rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.mac_log_priority;
#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

        /* Save the RLC Size List information */
        ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].mac_update_needed = TRUE;
        ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].rlc_size_list[logch_idx] =
           rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.size_rest_info;
  #else
        /* Initialize the MAC data structures to allow all RLC sizes, just in case. */
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].rlc_size_info.all_tfi_valid = TRUE;
#endif /* FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */


  /* Determine the type of Logical Channel mapped to DCH TrCH */
        if(rrc_ordered_hanging_rb_mapping[i].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
        }
        else
        {
          ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
        }
        
        
        rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id);
        if((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND) && 
            (rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM ))
        {
          WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rrc_ordered_hanging_rb_mapping[i].rb_id);
          ret_val= FAILURE;
        }
        
        if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
        {  
          if(get_am_index_wo_f3(rrc_ordered_hanging_rb_mapping[i].rb_id) != UE_MAX_AM_ENTITY)
          {
            logch_mode = UE_MODE_ACKNOWLEDGED_DATA;            
          }
          else if(get_ul_um_index((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id )!= 
            UE_MAX_UL_UM_CHANNEL)         
          {
            logch_mode = UE_MODE_UNACKNOWLEDGED;
          }
          else if(get_ul_tm_index(rlc_lc_id)!= UE_MAX_UL_TM_CHANNEL)
          {
            logch_mode = UE_MODE_TRANSPARENT;
          }
          else
          {
            if(rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM )
            {
              WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
              ret_val =  FAILURE;
            }
          }
        }
  
        /* Also update the Logical Channel ID here */
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
       
        /* Save the logical channel mode, if a setup */
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].dlchan_info[logch_idx].rlc_mode = logch_mode;
       
        /* other wise it is just a reconfig of RB Mapping info */
       
        /* Increment number of logical channels */
        ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].ndlchan++;
        ordered_config_ptr->ul_dch_trch_info[index].lg_ch_cnt = 
             ordered_config_ptr->ul_cctrch.dch_info.dch_info[index].ndlchan;
        ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
        MSG_HIGH("Restore UL DCH data for RB_id %d at  index %d for trch_id %d ",
                            rrc_ordered_hanging_rb_mapping[i].rb_id,i,
                            rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_dch_info.tr_id);
        rrcllc_free_hanging_rb_mapping_info_index(rrc_ordered_hanging_rb_mapping[i].rb_id, UL_DCH_RB_MAPPING);
      }
    }
  }
  return ret_val;
}

/*==========================================================
FUNCTION  :   rrcllc_restore_dl_dch_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info from
              the RB-mapping backup info to the Ordered config
              for DL DCh.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              returns  SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_restore_dl_dch_rb_mapping_info( tr_ch_id_type index)
{
  uint8 i,logch_idx;
  tr_ch_id_type trch_id;
  uecomdef_logch_mode_e_type logch_mode;
  rlc_lc_id_type rlc_lc_id;
  uecomdef_status_e_type ret_val= SUCCESS;
 
  trch_id = ordered_config_ptr->dl_dch_info[index].trch_id;
  
  for(i=0;i<MAX_HANGING_RB_MAPPING_INFO;i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE ) && 
       (rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.valid == TRUE ))
    {
      if(rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.tr_id == trch_id )
      {       
        
        logch_idx = ordered_config_ptr->dl_dch_info[index].ndlchan;
        ordered_config_ptr->dl_dch_info[index].dlchan_info[logch_idx].rb_id = 
               rrc_ordered_hanging_rb_mapping[i].rb_id;
        ordered_config_ptr->dl_dch_info[index].dlchan_info[logch_idx].mac_id =
               rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.log_channel_id;
        /* Determine the type of Logical Channel mapped to DCH TrCH */
        if(rrc_ordered_hanging_rb_mapping[i].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          ordered_config_ptr->dl_dch_info[index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
        }
        else
        {
          ordered_config_ptr->dl_dch_info[index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
        }
   
        rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id);
      
        if((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND) && 
            (rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM ))
        {
          
          WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rrc_ordered_hanging_rb_mapping[i].rb_id);
          ret_val= FAILURE;
          
        }
     
        if(rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
        {
          if(get_am_index_wo_f3(rrc_ordered_hanging_rb_mapping[i].rb_id) != UE_MAX_AM_ENTITY)
          {
            logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
          }
          else if(get_dl_um_index((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id, 
              ordered_config_ptr)!= UE_MAX_DL_UM_CHANNEL) 
          {
            logch_mode = UE_MODE_UNACKNOWLEDGED;
          }
          else if(get_dl_tm_index(rlc_lc_id)!= UE_MAX_DL_TM_CHANNEL)
          {
            logch_mode = UE_MODE_TRANSPARENT;
          }
          else
          {
            if(rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM )
            {
              WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
              ret_val= FAILURE;
            }
          }
        }
        /* Also update the Logical Channel ID here */
        ordered_config_ptr->dl_dch_info[index].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
      
        /* Save the logical channel mode, if a setup */
        ordered_config_ptr->dl_dch_info[index].dlchan_info[logch_idx].rlc_mode = logch_mode;
      
        /* Increment number of logical channels */
        ordered_config_ptr->dl_dch_info[index].ndlchan++;
        /* The logic channel mapped to this TrCH increases from 1 to 2. */
        /* Need to update the TB size to reflect this change.           */
        if (ordered_config_ptr->dl_dch_info[index].ndlchan == 2 &&
          ordered_config_ptr->dl_dch_trch_info[index].lg_ch_cnt == 1)
        {
          WRRC_MSG1_HIGH("TrCH %d Need to increase TB size", 
                            ordered_config_ptr->dl_dch_info[index].trch_id);
          ordered_config_ptr->dl_dch_trch_info[index].lg_ch_cnt_up = TRUE;
        }
        ordered_config_ptr->dl_dch_trch_info[index].lg_ch_cnt = 
                ordered_config_ptr->dl_dch_info[index].ndlchan;
        
        ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
        MSG_HIGH("Restore DL DCH data for RB_id %d at  index %d for trch_id %d ",
                            rrc_ordered_hanging_rb_mapping[i].rb_id,i,
                           rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.dch_map_info.tr_id);
        rrcllc_free_hanging_rb_mapping_info_index(rrc_ordered_hanging_rb_mapping[i].rb_id, DL_DCH_RB_MAPPING);
      }
    } 
  }
  return ret_val;
}

/*==========================================================
FUNCTION  :   rrcllc_restore_macd_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info from the RB-mapping backup info
               to the Ordered config for MAC-D flow id.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              return SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type  rrcllc_restore_macd_rb_mapping_info( uint8 index)
{
  uint8 i,logch_idx;
  uint8 mac_dflow_id;
  uecomdef_logch_mode_e_type logch_mode;
  rlc_lc_id_type rlc_lc_id;
  uecomdef_status_e_type ret_val= SUCCESS;
    
  mac_dflow_id = ordered_config_ptr->mac_dflow_info[index].mac_dflow_id;
  
  for(i=0;i<MAX_HANGING_RB_MAPPING_INFO;i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE ) && 
         (rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.valid == TRUE ))
    {
      if(rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.mac_d_flow_id == mac_dflow_id )
      {
                  
        logch_idx = ordered_config_ptr->mac_dflow_info[index].ndlchan;
        ordered_config_ptr->mac_dflow_info[index].dlchan_info[logch_idx].rb_id = 
           rrc_ordered_hanging_rb_mapping[i].rb_id;
        ordered_config_ptr->mac_dflow_info[index].dlchan_info[logch_idx].mac_id =
           rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.log_channel_id;
   
        /* Determine the type of Logical Channel mapped to DCH TrCH */
        if (rrc_ordered_hanging_rb_mapping[i].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          ordered_config_ptr->mac_dflow_info[index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
        } 
        else
        {
          ordered_config_ptr->mac_dflow_info[index].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
        }
          
        rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id);
        if ((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)&&
            (rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM ))
        {
          WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rrc_ordered_hanging_rb_mapping[i].rb_id);
          ret_val= FAILURE;
        }
          
        if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
        {
          if(get_am_index_wo_f3(rrc_ordered_hanging_rb_mapping[i].rb_id) != UE_MAX_AM_ENTITY)
          {
            logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
          }           
          else if(get_dl_um_index((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id, 
            ordered_config_ptr)!= UE_MAX_DL_UM_CHANNEL)         
          {
            logch_mode = UE_MODE_UNACKNOWLEDGED;
          }
          else if(get_dl_tm_index(rlc_lc_id)!= UE_MAX_DL_TM_CHANNEL)
          {
            logch_mode = UE_MODE_TRANSPARENT;
          }
          else
          {
            if(rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM )
            {
              WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
              ret_val= FAILURE;
            }
          }
        }
        /* Also update the Logical Channel ID here */
        ordered_config_ptr->mac_dflow_info[index].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
      
        /* Save the logical channel mode, if a setup */
        ordered_config_ptr->mac_dflow_info[index].dlchan_info[logch_idx].rlc_mode = logch_mode;
      
        /* Increment number of logical channels */
        ordered_config_ptr->mac_dflow_info[index].ndlchan++;
   
        ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
   
        rrcllc_free_hanging_rb_mapping_info_index(rrc_ordered_hanging_rb_mapping[i].rb_id, 
                                                                                    MAC_D_RB_MAPPING);
        MSG_HIGH("Restored MAC_D data for RB_id %d at  index %d for flow id %d ",
                           rrc_ordered_hanging_rb_mapping[i].rb_id,i,
                           rrc_ordered_hanging_rb_mapping[i].dl_mapping_info.macd_map_info.mac_d_flow_id);
      }
    }
  }
  return ret_val;
}



/*==========================================================
FUNCTION  : rrcllc_backup_eul_from_ota()

DESCRIPTION : This function backs up the Rb-Mapping info 
              received in the OTA message for EULH. 
              This function is called when the OTA message 
              provides the RB-Mapping info, but the Trch 
              info mapped to that RB is not provided.

DEPENDENCIES: None.

RETURN VALUE: boolean

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_backup_eul_from_ota
(
  rrc_UL_LogicalChannelMapping_r6*ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  uint8 idx;
  rrc_RLC_PDU_SizeList *rlc_pdu_sizelist;
  uint8 ddi = 0;
  uint32 cnt = 0;
  uint16 rlc_size = 0;
  rlc_lc_id_type rlc_lc_id;
  
  /* Store the RB-Mapping info */
  if( rrcllc_check_rb_id_exist_rb_mapping(rb_id, &idx) == FALSE )
  {
    if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
    {
      WRRC_MSG0_ERROR("No space for RB Mapping backup for UL DCH ");
      return FAILURE;
    }
  }
  rrc_ordered_hanging_rb_mapping[idx].rb_id = rb_id;
  rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
    
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.valid = TRUE;
  
#ifdef FEATURE_MAC_I
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_pdu_type = RLC_FIXED_PDU_SIZE;		
#endif /* FEATURE_MAC_I */	  	
   
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_e_flow_id= 
                 (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->e_DCH_MAC_d_FlowIdentity;
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_log_priority  = 
        (uint8) ie_ptr->mac_LogicalChannelPriority;

  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.log_channel_id =
       (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity;
  /* Save the and RLC info DDI info*/
  
  rlc_pdu_sizelist = &ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_SizeList;

  /* If more than 1 UL RLC PDU size is configured for this RB, the different sizes will use subsequent DDI values starting
    from this DDI value.*/
  ddi = ie_ptr->ul_TrCH_Type.u.e_dch->ddi;
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.num_rlc_size_ddi_info = 0;

  /*Affected info given in OTA doesnt provide rlc info, due to which logical channel mode in hanging RB gets 
    overwritten. This causes issue when update hsrach params is done as config ptr is not updated with the 
    correct LI size*/
  if(rrc_ordered_hanging_rb_mapping[idx].logch_mode == UE_MODE_MAX_NUM)
  {
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id(rb_id);
    if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
      return FAILURE;
    }
    if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      if(get_am_index_wo_f3(rb_id) != UE_MAX_AM_ENTITY)
      {
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      else if(get_ul_um_index(rb_id)!= UE_MAX_UL_UM_CHANNEL) 
      {
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      else if(get_ul_tm_index(rlc_lc_id)!= UE_MAX_UL_TM_CHANNEL)
      {
        logch_mode = UE_MODE_TRANSPARENT;
      }
      else
      {
        WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
        return FAILURE;
      }
    }
    rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
  }

  while ((rlc_pdu_sizelist->n > cnt) && (cnt < RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN)) 
  {
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_size_ddi_info[cnt].ddi = ddi;

    switch (rlc_pdu_sizelist->elem[cnt].t)
    {
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType1 * 8) + 16;
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part1 * 32) + 272;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType2->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part2 * 8);
        }
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part1 * 64) + 1040;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType3->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part2 * 8);
        }
        break;
      default:
        WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
            rlc_pdu_sizelist->elem[cnt].t);
        break;
    }
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_size_ddi_info[cnt].rlc_pdu_size_in_bits = rlc_size;

    /* Increment the number of valid fields in rlc_size_ddi_info[] */
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.num_rlc_size_ddi_info++;
    cnt++;
    /* Increment the DDI */
    ddi++;

  }

  /* If set include scheduling info is set to false then dont consider this mac_id while sending schedulingInformation */
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.in_sched_info = 
    ie_ptr->ul_TrCH_Type.u.e_dch->includeInSchedulingInfo;

 
  MSG_HIGH(" Adding into Standalone Database for R6 E_DCH for RB-ID %d for FLOW ID %d  at idx %d",
                             rrc_ordered_hanging_rb_mapping[idx].rb_id,
                             rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_e_flow_id,
                             idx);
  
  
      
  return (SUCCESS);
}
/*==========================================================
FUNCTION  : rrcllc_restore_ul_emac_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info from the RB-mapping backup info
               to the Ordered config for E-DCH MAC-d flowid.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              return SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_restore_ul_emac_rb_mapping_info( uint8 index)
{
  uint8 i,logch_idx,j;
  uint8 mac_eflow_id;
  uecomdef_logch_mode_e_type logch_mode;
  rlc_lc_id_type rlc_lc_id;
  uecomdef_status_e_type ret_val= SUCCESS;
 
  mac_eflow_id = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_mac_d_flow_id;
  
  for(i=0;i<MAX_HANGING_RB_MAPPING_INFO;i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE ) && 
         (rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.valid == TRUE))
    {
      if((rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id == mac_eflow_id ) 
#ifdef FEATURE_WCDMA_HS_RACH
       /*Check if the RB ID already exists in OC. Issue is happenning in HS-RACH because 
        we have decided to always keep the mapping in hanging rb. This is to avoid adding the the same rb id
        in OC twice  */
        && (FALSE == rrcllc_check_if_rbid_exists_in_config_ptr(ordered_config_ptr,rrc_ordered_hanging_rb_mapping[i].rb_id,index))
#endif
        )
      {            
        logch_idx = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].num_logch;
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rb_id = 
                   rrc_ordered_hanging_rb_mapping[i].rb_id;
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].lc_id =
                  rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.log_channel_id;
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].priority =
                 rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_log_priority;
      
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].include_in_scheduling_info =
               rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.in_sched_info;           
#ifdef FEATURE_MAC_I
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_pdu_type = 
                   rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.rlc_pdu_type;				
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].ul_li_size = 
                   rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.ul_li_size;
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].ul_flex_min_pdu_size= 
                   rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.ul_flex_min_pdu_size;
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].ul_flex_max_pdu_size= 
                   rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.ul_flex_max_pdu_size;
#endif		  
 				
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].num_rlc_size_ddi_info =
              rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.num_rlc_size_ddi_info;
        for( j=0;j<rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.num_rlc_size_ddi_info;j++)
        {
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_size_ddi_info[j]=
              rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.rlc_size_ddi_info[j];
        }
   
        /* Determine the type of Logical Channel mapped to DCH TrCH */
        if (rrc_ordered_hanging_rb_mapping[i].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
        } 
        else
        {
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
        }
   
   
        rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id);
        if ((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)&&
            (rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM ))
        {
          WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rrc_ordered_hanging_rb_mapping[i].rb_id);
          ret_val= FAILURE;
        }
   
        if (rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
        {
          
          if(get_am_index_wo_f3(rrc_ordered_hanging_rb_mapping[i].rb_id) != UE_MAX_AM_ENTITY)
          {
            logch_mode = UE_MODE_ACKNOWLEDGED_DATA;            
          }           
          else if(get_ul_um_index( (rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id )!= 
            UE_MAX_UL_UM_CHANNEL)         
          {
            logch_mode = UE_MODE_UNACKNOWLEDGED;
          }
          else if(get_ul_tm_index(rlc_lc_id)!= UE_MAX_UL_TM_CHANNEL)
          {
            logch_mode = UE_MODE_TRANSPARENT;
          }
          else
          {
            if(rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM )
            {
              WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
              ret_val= FAILURE;
            }
          }
        }
   
        /* Also update the Logical Channel ID here */
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_id = rlc_lc_id;
   
        /* Save the logical channel mode, if a setup */
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_mode = logch_mode;
      
        /* Increment number of logical channels */
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[index].num_logch++;  
   
        /* Here indicate that MAC Uplink needs to be reconfigured */
        ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
    
        MSG_HIGH("Restored MAC_E data for RB_id %d at  index %d for flow id %d ",
                          rrc_ordered_hanging_rb_mapping[i].rb_id,i,
                         rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id);
/* Don't free the Hanging RB */
#ifndef FEATURE_WCDMA_HS_RACH
        rrcllc_free_hanging_rb_mapping_info_index(rrc_ordered_hanging_rb_mapping[i].rb_id,
                                                                                 E_DCH_RB_MAPPING);
#endif
      }
    }
  }
  return ret_val;
}
/*==========================================================
FUNCTION  : rrcllc_check_ordered_current_mapping_same()

DESCRIPTION : This function checks if the RB Mapping is same in the current RB-Mapping 
 database, and the Ordered RB-Mapping database.

DEPENDENCIES: None.

RETURN VALUE: boolean: returns TRUE if  RB Mapping is same in the current RB-Mapping 
 database and the Ordered RB-Mapping database.
 else returns FALSE

SIDE EFFECTS: None
============================================================*/
boolean rrcllc_check_ordered_current_mapping_same(uint8 index )
{

  boolean ret_val = TRUE;
  
  if(rrc_ordered_hanging_rb_mapping[index].rb_id == rrc_current_hanging_rb_mapping[index].rb_id )
  {
    if((rrc_current_hanging_rb_mapping[index].ul_mapping_info.ul_dch_info.valid == TRUE ) &&
      ( rrc_ordered_hanging_rb_mapping[index].ul_mapping_info.ul_dch_info.valid == FALSE ))
    {
	  ret_val=FALSE;
    }
    if((rrc_current_hanging_rb_mapping[index].ul_mapping_info.ul_mace.valid == TRUE ) &&
       ( rrc_ordered_hanging_rb_mapping[index].ul_mapping_info.ul_mace.valid == FALSE ))
    {
	  ret_val=FALSE;
    }
    if((rrc_current_hanging_rb_mapping[index].dl_mapping_info.dch_map_info.valid == TRUE ) &&
      ( rrc_ordered_hanging_rb_mapping[index].dl_mapping_info.dch_map_info.valid == FALSE ))
    {
	  ret_val=FALSE;
    }
    if((rrc_current_hanging_rb_mapping[index].dl_mapping_info.macd_map_info.valid == TRUE ) &&
       ( rrc_ordered_hanging_rb_mapping[index].dl_mapping_info.macd_map_info.valid == FALSE ))
    {
	  ret_val=FALSE;
    }
  }
  else
  {
    ret_val=FALSE;
  }
  if(rrc_current_hanging_rb_mapping[index].rb_id != rrc_ordered_hanging_rb_mapping[index].rb_id)
  {
  MSG_HIGH("CC/OC rb-d [%d/%d] at index %d ",rrc_current_hanging_rb_mapping[index].rb_id,
                                             rrc_ordered_hanging_rb_mapping[index].rb_id,
                                             index);
  }
  WRRC_MSG8_MED("CC/OC ul_dch_info.valid[%d/%d] ul_mace.valid[%d/%d] dch_map_info.valid[%d/%d] macd_map_info.valid[%d/%d]",
                                        rrc_current_hanging_rb_mapping[index].ul_mapping_info.ul_dch_info.valid,
                                        rrc_ordered_hanging_rb_mapping[index].ul_mapping_info.ul_dch_info.valid,
                                        rrc_current_hanging_rb_mapping[index].ul_mapping_info.ul_mace.valid,
                                        rrc_ordered_hanging_rb_mapping[index].ul_mapping_info.ul_mace.valid,
                                        rrc_current_hanging_rb_mapping[index].dl_mapping_info.dch_map_info.valid,
                                        rrc_ordered_hanging_rb_mapping[index].dl_mapping_info.dch_map_info.valid,
                                        rrc_current_hanging_rb_mapping[index].dl_mapping_info.macd_map_info.valid,
                                        rrc_ordered_hanging_rb_mapping[index].dl_mapping_info.macd_map_info.valid);
  return ret_val;
}

/*==========================================================
FUNCTION  : rrcllc_backup_dl_dch_from_ota()

DESCRIPTION : This function backs up the Rb-Mapping info 
              received in the OTA message for DL DCH. 
              This function is called when the OTA message 
              provides the RB-Mapping info, but the Trch 
              info mapped to that RB is not provided.

DEPENDENCIES: None.

RETURN VALUE: boolean

SIDE EFFECTS: None
============================================================*/
 uecomdef_status_e_type rrcllc_backup_dl_dch_from_ota
(
  tr_ch_id_type               trch_id,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode,
  uint8 mac_id
)
{
  uint8 idx;
  /* Store the RB-Mapping info */
  if( rrcllc_check_rb_id_exist_rb_mapping(rb_id, &idx) == FALSE )
  {
    if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
    {
      WRRC_MSG0_ERROR("No space for RB Mapping backup for DL DCH ");
      return FAILURE;
    }
  }
  rrc_ordered_hanging_rb_mapping[idx].rb_id = rb_id;
  rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
     
  rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.valid = TRUE;
     
   rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.tr_id = trch_id;
    
  rrc_ordered_hanging_rb_mapping[idx].dl_mapping_info.dch_map_info.log_channel_id = mac_id; 
   
  MSG_HIGH(" Adding into Standalone Database for DL DCH for RB-ID  %d for Trch id %d  at idx %d",
                 rrc_ordered_hanging_rb_mapping[idx].rb_id,trch_id,idx);
  return (SUCCESS);

}
/*==========================================================
FUNCTION  : rrcllc_backup_ul_dch_from_ota_r6()

DESCRIPTION : This function backs up the Rb-Mapping info 
              received in the REL6 OTA message for UL DCH.
              This function is called when the OTA message 
              provides the RB-Mapping info, 
              but the Trch info mapped to that RB is not provided.

DEPENDENCIES: None.

RETURN VALUE: boolean

SIDE EFFECTS: None
============================================================*/
 uecomdef_status_e_type rrcllc_backup_ul_dch_from_ota_r6
(
  rrc_UL_LogicalChannelMapping_r6*ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  uint8 idx;
  /* Store the RB-Mapping info */
  if( rrcllc_check_rb_id_exist_rb_mapping(rb_id, &idx) == FALSE )
  {
    if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
    {
      WRRC_MSG0_ERROR("No space for RB Mapping backup for UL DCH ");
      return FAILURE;
    }
  }
  rrc_ordered_hanging_rb_mapping[idx].rb_id = rb_id;
  rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
    
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.valid = TRUE;
  
   
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id = 
                (tr_ch_id_type) ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.u.dch;
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.mac_log_priority  = 
        (uint8) ie_ptr->mac_LogicalChannelPriority;
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */
  if (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
  {
     rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =
       (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
  }
  else
  {
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =0;
  }
  MSG_HIGH(" Adding into Standalone Database for R6 UL DCH for RB-ID  %d for Trch id %d  at idx %d",
                             rrc_ordered_hanging_rb_mapping[idx].rb_id,
                             rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id,
                             idx);
  /* Save the RLC Size List information */
#if defined(FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH)
  if(get_rlc_size_list_r6(ie_ptr, &rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.size_rest_info, rb_id) == FAILURE)
  {
    return FAILURE;
  }
#endif
      
  return (SUCCESS);
}
/*==========================================================
FUNCTION  : rrcllc_backup_ul_dch_from_ota()

DESCRIPTION : This function backs up the Rb-Mapping info received
              in the REL5/R99 OTA message for UL DCH. This function 
              is called when the OTA message provides the RB-Mapping 
              info, but the Trch info mapped to that RB is not provided.


DEPENDENCIES: None.

RETURN VALUE: boolean

SIDE EFFECTS: None
============================================================*/
 uecomdef_status_e_type rrcllc_backup_ul_dch_from_ota
(
   rrc_UL_LogicalChannelMapping *ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{

  uint8 idx;
  /* Store the RB-Mapping info */
  if( rrcllc_check_rb_id_exist_rb_mapping(rb_id, &idx) == FALSE )
  {
    if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
    {
      WRRC_MSG0_ERROR("No space for RB Mapping backup for UL DCH ");
      return FAILURE;
    }
  }
  rrc_ordered_hanging_rb_mapping[idx].rb_id = rb_id;
  rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
  
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.valid = TRUE;
  
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id = 
     (tr_ch_id_type)ie_ptr->ul_TransportChannelType.u.dch;
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.mac_log_priority
      = (uint8)ie_ptr->mac_LogicalChannelPriority;
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_UL_LogicalChannelMapping,logicalChannelIdentity))
  {
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =
         (uint8)ie_ptr->logicalChannelIdentity;
  }
  else
  {
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =0;    
  } 
  MSG_HIGH(" Adding into Standalone Database for UL DCH for RB-ID  %d for Trch id %d  at idx %d",
                             rrc_ordered_hanging_rb_mapping[idx].rb_id,
                             rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id,
                             idx);
    #if defined(FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH)
  /* Save the RLC Size List information */
   if(get_rlc_size_list(ie_ptr, &rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.size_rest_info, rb_id) == FAILURE)
  {
    return FAILURE;
  }
#endif

  return (SUCCESS);
}

/*==========================================================
FUNCTION  : rrcllc_validate_rb_mapping_info()

DESCRIPTION : This functions validates the RB mapping information
              for R99, R5 and R6.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE.

SIDE EFFECTS: 
============================================================*/
uecomdef_status_e_type rrcllc_validate_rb_mapping_info(void)
{
  uint8 cntx = 0;
  uint8 cnty = 0;
  uint8 trch_idx = 0;
  ordered_config_type *config_ptr;

  if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
  {
    config_ptr = ordered_config_ptr;
  }
  else
  {
    if(transition_config.toc_usage != TOC_INVALID)
    {
      config_ptr = transition_config.toc_ptr;
    }
    else
    {
      WRRC_MSG0_HIGH("TOC usage is invalid.");
      return SUCCESS;
    }
  }
  /*Validate DL DCH mapping info */
  for (trch_idx = 0; trch_idx < UE_MAX_TRCH; trch_idx++) 
  {
    if((config_ptr->dl_dch_trch_info[trch_idx].dch_present) &&
       (config_ptr->dl_dch_trch_idx_in_use[trch_idx] == TRUE))
    {
      if (config_ptr->dl_dch_info[trch_idx].ndlchan > 1) 
      {
        for (cntx = 0; (cntx < config_ptr->dl_dch_info[trch_idx].ndlchan) && (cntx <MAX_DED_LOGCHAN_PER_TRCH); cntx++) 
        {
          if (RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->dl_dch_info[trch_idx].dlchan_info[cntx].chan_type)) 
          {
            if (config_ptr->dl_dch_info[trch_idx].dlchan_info[cntx].mac_id == 
                RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT) 
            {
              WRRC_MSG1_ERROR("Logical Chan ID not present for rb_id %d",
                  config_ptr->dl_dch_info[trch_idx].dlchan_info[cntx].rb_id);
              return FAILURE;
            }
            for (cnty = (cntx+1); ((cnty < config_ptr->dl_dch_info[trch_idx].ndlchan) 
                && (cnty < MAX_DED_LOGCHAN_PER_TRCH)); cnty++) 
            {
              if ((config_ptr->dl_dch_info[trch_idx].dlchan_info[cnty].mac_id  == 
                   config_ptr->dl_dch_info[trch_idx].dlchan_info[cntx].mac_id) &&
                  (RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->dl_dch_info[trch_idx].dlchan_info[cnty].chan_type)) ) 
              {
                ERR("Cannot assign same log chl ID %d to 2 dif rbs: %d %d: fail",
                    config_ptr->dl_dch_info[trch_idx].dlchan_info[cnty].mac_id,
                    config_ptr->dl_dch_info[trch_idx].dlchan_info[cnty].rb_id,
                    config_ptr->dl_dch_info[trch_idx].dlchan_info[cntx].rb_id);
                return (FAILURE);
                
              }
            } 
          }
        }
      } /* if ndlchan > 1*/
    }
  } /* For (trch_idx ...*/

  /* Validate FACH mapping info.  Look at OC always */
  if ((ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan > 1) 
      && (ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan <  MAX_DED_LOGCHAN_PER_TRCH))
  {
    for (cntx = 0; cntx < ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan; cntx++) 
    { 
      if (RRCLLC_LC_TYPE_DCCH_OR_DCTCH(ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cntx].chan_type))
      {
        if (ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cntx].mac_id ==
            RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT)
        {
          WRRC_MSG1_ERROR("Logical Chan ID not present for rb_id %d",
              ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cntx].rb_id);
          return FAILURE;
        }
        for (cnty = (cntx+1); cnty < ordered_config_ptr->cell_fach_rb_mapping_info.dl_ndlchan ; cnty++) 
        {
          if ((ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cntx].mac_id == 
              ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cnty].mac_id) &&
              RRCLLC_LC_TYPE_DCCH_OR_DCTCH(ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cnty].chan_type)) 
          {
            WRRC_MSG1_ERROR("Cannot assign same log chl ID %d to 2 dif log chans: fail",
                ordered_config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[cntx].mac_id);
            return (FAILURE);
          }
        } 
      }
    } /*for cntx=0 ... */
  } /* if dl_ndlchan >= 1*/

  /* validate mapping to HSDPA */
  if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5) 
  {
    for(trch_idx = 0; trch_idx < UE_MAX_MAC_D_FLOW; trch_idx++)
    {
      if((config_ptr->dl_dflow_index_in_use[trch_idx] == TRUE) &&
         (config_ptr->mac_dflow_info[trch_idx].ndlchan > 1))
      {
        for (cntx = 0; cntx < config_ptr->mac_dflow_info[trch_idx].ndlchan; cntx++) 
        {
          if (RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->mac_dflow_info[trch_idx].dlchan_info[cntx].chan_type)) 
          {
            if (config_ptr->mac_dflow_info[trch_idx].dlchan_info[cntx].mac_id ==
                RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT) 
            {
              WRRC_MSG1_ERROR("Logical Chan ID not present for rb_id %d",
                  config_ptr->mac_dflow_info[trch_idx].dlchan_info[cntx].rb_id);
              return FAILURE;
            }
            
            for (cnty = (cntx+1); ((cnty < config_ptr->mac_dflow_info[trch_idx].ndlchan)
                && (cnty < UE_MAX_LOGCHAN_PER_DFLOW)); cnty++) 
            {
              if ((config_ptr->mac_dflow_info[trch_idx].dlchan_info[cntx].mac_id == 
                   config_ptr->mac_dflow_info[trch_idx].dlchan_info[cnty].mac_id) && 
                  RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->mac_dflow_info[trch_idx].dlchan_info[cnty].chan_type))
              {
                ERR("Cannot assign same log chl ID %d to 2 dif rbs: %d %d: fail",
                    config_ptr->mac_dflow_info[trch_idx].dlchan_info[cntx].mac_id,
                    config_ptr->mac_dflow_info[trch_idx].dlchan_info[cntx].rb_id,
                    config_ptr->mac_dflow_info[trch_idx].dlchan_info[cnty].rb_id);
                return (FAILURE);
              }
            } 
          }
        } /* for (cntx = 0; ...*/
      }
    }/*for(trch_idx = 0 ...*/  

    for(cntx = 0;cntx < config_ptr->mac_dl_parms.ndlchan_macehs; cntx++)
    {
      if((config_ptr->mac_ehs_lc_id_in_use[cntx] == TRUE)
        &&(RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->dl_macehs_logchan_info[cntx].chan_type)))
      {
        if (config_ptr->dl_macehs_logchan_info[cntx].mac_id ==
            INVALID_LOGICAL_CHANNEL_ID) 
        {
          WRRC_MSG1_ERROR("Logical Chan ID not present for rb_id %d",
              config_ptr->dl_macehs_logchan_info[cntx].rb_id);
          return FAILURE;
        }
        
        for (cnty = (cntx+1); ((cnty < config_ptr->mac_dl_parms.ndlchan_macehs)
            &&(cnty < UE_MAX_DL_LOGICAL_CHANNEL)); cnty++) 
        {
          if ((config_ptr->dl_macehs_logchan_info[cntx].mac_id == 
               config_ptr->dl_macehs_logchan_info[cnty].mac_id) && 
              RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->dl_macehs_logchan_info[cnty].chan_type))
          {
            ERR("Cannot assign same log chl ID %d to 2 dif rbs: %d %d: fail",
                config_ptr->dl_macehs_logchan_info[cntx].mac_id,
                config_ptr->dl_macehs_logchan_info[cntx].rb_id,
                config_ptr->dl_macehs_logchan_info[cnty].rb_id);
            return (FAILURE);
          }
        } 
      }
    }
  }/*if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)*/
   

  /* Validate UL DCH mapping info */
  for(trch_idx = 0; trch_idx < UE_MAX_TRCH; trch_idx++)
  {
    if((config_ptr->ul_dch_trch_info[trch_idx].dch_present) &&
       (config_ptr->ul_dch_trch_idx_in_use[trch_idx] == TRUE))
    {
      if (config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan > 1)
      {
        for (cntx = 0; cntx < config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan; cntx++) 
        {
          if (RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cntx].chan_type))
          {
            if (config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cntx].mac_id == 
                RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT) 
            {
              WRRC_MSG1_ERROR("Logical Chan ID not present for rb_id %d",
                  config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cntx].rb_id);
              return FAILURE;
            }
            
            for (cnty = (cntx+1); (cnty < config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan)
                                  && (cnty < MAX_DED_LOGCHAN_PER_TRCH); cnty++) 
            {
              if ((config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnty].mac_id ==
                   config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cntx].mac_id) &&
                  RRCLLC_LC_TYPE_DCCH_OR_DCTCH(config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnty].chan_type))
              {
                ERR("Cannot assign same log chl ID %d to 2 dif rbs %d %d: fail",
                    config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cntx].mac_id,
                    config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cntx].rb_id,
                    config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnty].rb_id);
                return (FAILURE);
              }
            }
          }
        }/* for (cntx = 0; ...*/
      }
    }
  }/* for(trch_idx = 0; ...*/

  /* Validate PRACH mapping info.  Look at OC always */
  if ((ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan > 1) 
    &&(ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan < MAX_DED_LOGCHAN_PER_TRCH))
  {
    for (cntx = 0; cntx < ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ; cntx++) 
    {
      if (RRCLLC_LC_TYPE_DCCH_OR_DCTCH(ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cntx].chan_type))
      {
        if (ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cntx].mac_id == 
            RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT) 
        {
          WRRC_MSG1_ERROR("Logical Chan ID not present for rb_id %d",
              ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cntx].rb_id);
          return FAILURE;
        }
        
        for (cnty = (cntx+1); cnty < ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ; cnty++) 
        {
          if ((ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cntx].mac_id == 
               ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cnty].mac_id ) &&
              RRCLLC_LC_TYPE_DCCH_OR_DCTCH(ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cnty].chan_type))
          {
            WRRC_MSG1_ERROR("Cannot assign same log chl ID %d to 2 dif log chans: fail",
                ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cntx].mac_id);
            return FAILURE;
          }
        }
      }
    }
  }

  return SUCCESS;

} /* rrcllc_validate_rb_mapping_info() */

/*==========================================================
FUNCTION  : get_srb_info_to_setup_default_r5()

DESCRIPTION : Get Signalling Radio Bearer Information to 
              setup for Rb-Setup with preconfiguration info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_srb_info_to_setup_preconfig_r5
(
rrc_SRB_InformationSetup_r5     *ie_ptr,
rrc_SRB_InformationSetupList_r5 * srb_setup_ptr,
rrc_RB_Identity                   rb_id
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  
  /* Local count of # of SRB's to setup */
  uint8                           srb_cnt = 0;
  
  /* temp rb id which we shall compute if rb_id is not part of
  * srb_setup_ptr when we loop thru the list to get a match
  */
  rrc_RB_Identity                      temp_rb_id = 0;
  

  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_SRB_InformationSetup_r5     *temp_ie_ptr = ie_ptr;
  
  /* Entity indexes in case we need to look up RLC info from
  * our internal database instead of processing an IE.
  */
  uint8 entity_idx, new_entity_idx;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
   *rlc_Info for the rb_id specified.  Walk thru the srb_setup_head_ptr
   *list until we find a rb_id that matches
   */
  if(RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r5_same_as_RB))
  {
    while((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {

      /* Check to see if the RB ID is there ... */
      if(srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        temp_rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }

      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        temp_rb_id++;
      }

      /*check if the rb_id matches*/
      if(ie_ptr->rlc_InfoChoice.u.same_as_RB == temp_rb_id)
      {
        temp_ie_ptr = &(srb_setup_ptr->elem[srb_cnt]);
        break;
      }

      
      srb_cnt++;
    } /* end while... */

    if((srb_setup_ptr->n == srb_cnt) || (srb_cnt >= UE_MAX_SRB_SETUP))
    {
      WRRC_MSG1_HIGH("same_as_RB %d not in msg, lookup req'd",
               ie_ptr->rlc_InfoChoice.u.same_as_RB);
      temp_ie_ptr = NULL;
      temp_rb_id = ie_ptr->rlc_InfoChoice.u.same_as_RB;
      
      if((entity_idx = get_am_index(temp_rb_id)) != UE_MAX_AM_ENTITY)
      {
        /* This RB is in AM mode. And its reference config is found in OC.
         */
        if( (new_entity_idx = get_am_index_wo_f3(rb_id)) == UE_MAX_AM_ENTITY)
        {
          if ((new_entity_idx = rrcllc_get_1st_hole_for_rlc_am()) == UE_MAX_AM_ENTITY)
          {
            WRRC_MSG1_ERROR("Can't find space for new rb %d", rb_id);
            return FAILURE;
          }
          ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan++;
          ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan++;
          ordered_config_ptr->rlc_am_parms.nentity++;
                  
        }
        ordered_config_ptr->am_config[new_entity_idx].rlc_ul_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->common_config.rb_id = rb_id;

        ordered_config_ptr->am_config[new_entity_idx].rlc_dl_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
   
        if((ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan >
                                                       UE_MAX_UL_AM_DATA_CHANNEL) || 
           (ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan >
                                                       UE_MAX_DL_AM_DATA_CHANNEL) ||
           (ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY))
        {
          ERR("Too many UL/DL AM channels or entities [DL,UL,entities] [%d,%d,%d]",
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.nentity);
          return FAILURE;
        }
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
             
      if(((entity_idx = get_ul_um_index(temp_rb_id))
              != UE_MAX_UL_UM_CHANNEL) && 
              ((entity_idx = get_ul_um_index(rb_id))
              == UE_MAX_UL_UM_CHANNEL))             
      {
        /* This RB is in UM mode */
        if(get_rlc_ul_um_info(rb_id, TRUE) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
           
      if(((entity_idx = get_dl_um_index(temp_rb_id, ordered_config_ptr))
              != UE_MAX_DL_UM_CHANNEL) &&
              ((entity_idx = get_dl_um_index(rb_id, ordered_config_ptr))
              == UE_MAX_DL_UM_CHANNEL))
      {
        /* This RB is in UM mode */
        if(get_rlc_dl_um_info(rb_id, TRUE, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      /* SRBs cannot be TM.  If we haven't found it by now, we have
       * a problem. */
      if(logch_mode == UE_MODE_MAX_NUM)
      {
        WRRC_MSG1_ERROR("same_as_RB %d not found in msg or OC",
            ie_ptr->rlc_InfoChoice.u.same_as_RB);
        return FAILURE;
      }
    }
  } /* end RLC choice == Same as RB */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r5_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,ul_RLC_Mode)&&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r5,dl_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", rb_id);
      return FAILURE;
    }
  
  }

  
  /* Now we have either the RLC Info explicitly meant for this RB OR
  * the RLC Info that is the same as another RB.  If tmp_ie_ptr is NULL,
  * then we've already copied the RLC Info from our internal database.
  */
  if (temp_ie_ptr != NULL)
  {
    /* Get the RLC mode */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r5_rlc_Info) && 
         ((logch_mode = get_rlc_mode_r5(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
       )
    {
      return FAILURE;
    }
    
    /* Get the RLC info */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r5_rlc_Info) && 
         (get_rlc_info_r5(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, rb_id, TRUE) == FAILURE))
    {
      return FAILURE;
    }
  }
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r5(&ie_ptr->rb_MappingInfo, rb_id, logch_mode) == FAILURE)
  {
    return FAILURE;
  }
  
  /* everything is configured */
  return SUCCESS;

} /* end get_srb_info_to_setup_r5() */


/*============================================================================
FUNCTION: rrcllc_update_mac_ehs_ota()

DESCRIPTION:

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
static uecomdef_status_e_type rrcllc_update_mac_ehs_ota
(
rrc_LogicalChannelIdentity logicalChannelIdentity,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode,
uint32         macehs_queue_id
)
{
  uint8 lc_index;
  rlc_lc_id_type   rlc_lc_id;

  lc_index = rrcllc_find_lc_index_for_macehs(ordered_config_ptr, logicalChannelIdentity,rb_id);
  if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
  {
     /* Allocate LC index */
     lc_index = rrcllc_allocate_lc_index_macehs(ordered_config_ptr, logicalChannelIdentity);
 
     if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
     {
       /* Failure to allocate a new LC Index Index */
       WRRC_MSG0_ERROR("MACEHS: Failure to allocate new Lc iD index");
       return(FAILURE);
     }
  }         

  if (logch_mode == UE_MODE_MAX_NUM)
  {
   /* This is the case when RB mapping info is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
     */
  
    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if (rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("MACEHS: Invalid DL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
    
    if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("MACEHS: Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
    /* Also update the Logical Channel ID here */
    ordered_config_ptr->dl_macehs_logchan_info[lc_index].rlc_id = rlc_lc_id;
  }

  hsdpa_msg_params.hsdsch_info_present =TRUE; 
  
  ordered_config_ptr->dl_macehs_logchan_info[lc_index].rlc_mode= logch_mode;
  ordered_config_ptr->dl_macehs_logchan_info[lc_index].rb_id = rb_id;
  
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID )
  {
    ordered_config_ptr->dl_macehs_logchan_info[lc_index].chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    ordered_config_ptr->dl_macehs_logchan_info[lc_index].chan_type = UE_LOGCHAN_DTCH;
  }
  ordered_config_ptr->dl_macehs_logchan_info[lc_index].queue_id =   macehs_queue_id;

#ifdef FEATURE_WCDMA_HS_FACH
  lc_index = rrcllc_find_lc_index_for_efach(transition_config.toc_ptr, logicalChannelIdentity,rb_id);
  if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
  {
     /* Allocate LC index */
     lc_index = rrcllc_allocate_lc_index_efach(transition_config.toc_ptr, logicalChannelIdentity);
 
     if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
     {
       /* Failure to allocate a new LC Index Index */
       WRRC_MSG0_ERROR("EFACH: Failure to allocate new Lc iD index");
       return(FAILURE);
     }
  }
  if (logch_mode == UE_MODE_MAX_NUM)
  {
    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if (rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("MACEHS: Invalid DL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
    if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("MACEHS: Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
    /* Also update the Logical Channel ID here */
    transition_config.toc_ptr->efach_dl_logchan_info[lc_index].rlc_id = rlc_lc_id;
  }
  
  transition_config.toc_ptr->efach_dl_logchan_info[lc_index].rlc_mode= logch_mode;    
  transition_config.toc_ptr->efach_dl_logchan_info[lc_index].rb_id = rb_id;
  
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID )
  {
    transition_config.toc_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    transition_config.toc_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_DTCH;
  }
  transition_config.toc_ptr->efach_dl_logchan_info[lc_index].queue_id =   macehs_queue_id;
#endif

   return(SUCCESS);
}

/*==========================================================
FUNCTION  : get_dl_dch_mapping_info_r7()

DESCRIPTION : Processes DL DCH Mapping IE contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_dch_mapping_info_r7
(
rrc_DL_LogicalChannelMapping_r7 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode
)
{
  
  /* Transport Channel ID */
  tr_ch_id_type   trch_id=0;

  /* Get the DL DCH TrCH ID if it exists */
  switch (ie_ptr->dl_TransportChannelType.t)
  {
  case T_rrc_DL_TransportChannelType_r7_dch:
    trch_id = (tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch;
    break;

  case T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch:
    if(ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->t == T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch_mac_hs )
    {
      trch_id = (tr_ch_id_type)ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->u.mac_hs->dch_transport_ch_id;
    }
    break;
    
  case T_rrc_DL_TransportChannelType_r7_fach:
  case T_rrc_DL_TransportChannelType_r7_dsch:
  default:
    WRRC_MSG0_ERROR("Expecting DL DCH TrCH ID");
    return FAILURE;
  } /* end DL DCH TrCH ID switch */
  
    /* Get the transport channel index for this transport channel ID.
    * Note: The index should already be there.
  */

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_LogicalChannelMapping_r7,logicalChannelIdentity))
  {
    return rrcllc_update_dl_dch_ota(rb_id,logch_mode,trch_id, ie_ptr->logicalChannelIdentity);
  }
  else
  {
    return rrcllc_update_dl_dch_ota(rb_id,logch_mode,trch_id, RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT);
  }
  

}
/*==========================================================
FUNCTION  : get_hsdsch_mapping_info_r7()

DESCRIPTION : Processes HSDSCH Mapping IE contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_hsdsch_mapping_info_r7
(
rrc_DL_LogicalChannelMapping_r7 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode
)
{
  /* DFLOW   Id */
  uint32         dflow_id= INVALID_DFLOW_ID;

  boolean mac_hs_chosen = FALSE;
  boolean mac_ehs_chosen = FALSE;
  uint32 macehs_queue_id = INVALID_QUEUE_ID;

  switch (ie_ptr->dl_TransportChannelType.t)
  {
  case T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch:
     
    if( ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->t == T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch_mac_hs)
    {
      dflow_id = ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->u.mac_hs->hsdsch_mac_d_flow_id;
      mac_hs_chosen = TRUE;
    }
    else if( ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->t == T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch_mac_ehs)
    {

      mac_ehs_chosen = TRUE;
      macehs_queue_id = ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->u.mac_ehs->hsdsch_mac_ehs_QueueId;
    }
    break;

  case T_rrc_DL_TransportChannelType_r7_hsdsch:
    if( ie_ptr->dl_TransportChannelType.u.hsdsch->t == T_rrc_DL_TransportChannelType_r7_hsdsch_mac_hs )
    {
      mac_hs_chosen = TRUE;
      dflow_id = ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_hs;
    }
    else if( ie_ptr->dl_TransportChannelType.u.hsdsch->t == T_rrc_DL_TransportChannelType_r7_hsdsch_mac_ehs )
    {
      mac_ehs_chosen = TRUE;
      macehs_queue_id = ie_ptr->dl_TransportChannelType.u.hsdsch->u.mac_ehs;
    }
    break;

  default:
    WRRC_MSG1_ERROR("RRCHS:Invalid  Channel Type %d",ie_ptr->dl_TransportChannelType.t);
    return FAILURE;
  }

  WRRC_MSG4_MED("RRCHS:mac_hs_chosen %d mac_ehs_chosen %d dflow_id %d  macehs_queue_id %d",
                      mac_hs_chosen,mac_ehs_chosen,dflow_id,macehs_queue_id);
  
  if(mac_hs_chosen == TRUE )
  {
    /* If Logical Channel ID is present, save MAC Logical Channel Identity */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_LogicalChannelMapping_r7,logicalChannelIdentity))
    {
      return rrcllc_update_machs_ota(ie_ptr->logicalChannelIdentity,rb_id,logch_mode,dflow_id);
    }
    else 
    {
      return rrcllc_update_machs_ota(RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT,rb_id,logch_mode,dflow_id);
    }
  }
  else if (mac_ehs_chosen == TRUE )
  {

    /* If Logical Channel ID is present, save MAC Logical Channel Identity */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_DL_LogicalChannelMapping_r7,logicalChannelIdentity))
    {
      return rrcllc_update_mac_ehs_ota(ie_ptr->logicalChannelIdentity,rb_id,logch_mode,macehs_queue_id);
    }
    else
    {
      WRRC_MSG0_HIGH(" Logical ID mandatory for MAC-Ehs ");
      return FAILURE;
    }
  }
  return SUCCESS;

}

/*==========================================================
FUNCTION  : get_dl_logical_channel_info_r7()

DESCRIPTION : Get DL logical Channel info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_dl_logical_channel_info_r7
(
rrc_DL_LogicalChannelMappingList_r7 * ie_ptr,
rrc_RB_Identity                       rb_id,
uecomdef_logch_mode_e_type       logch_mode
)
{
  /*
  * Count to make sure that don't exceed number of Logical
  * channels that can be Mapped to a Transport Channel.
  */
  uint8 lc_tc_cnt = 0;
  
  ordered_config_type *tmp_config_ptr = NULL;

  
  /* Process each item of the list */
  while ((ie_ptr->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH))
  {
    /* Transport channel type */
    switch (ie_ptr->elem[lc_tc_cnt].dl_TransportChannelType.t)
    {

    case T_rrc_DL_TransportChannelType_r7_hsdsch:
      /* Check if logical channel is not TM */
      if (logch_mode == UE_MODE_TRANSPARENT)
      {
        WRRC_MSG0_ERROR("RRCHS:TM Log Channel mapped on HSDSCH");
        return FAILURE;
      }
      if ( ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH )
      {
        if (get_hsdsch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {

        /*TBD PSR: Why will ever fo here as the rrc_state_indicator in OC is always DCH
          * But then in that case the mapping info will always be in OC and not in TOC?? 
          */
        if (transition_config.toc_usage == TOC_INVALID)
        {
          transition_config.toc_usage = TOC_FOR_DCH;
        }
        
        tmp_config_ptr = ordered_config_ptr;
        ordered_config_ptr = transition_config.toc_ptr;
        
        /* Processing functions must think we are going to Cell_DCH state. */
        ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
        
        if (get_hsdsch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;
      }
      break;
    case T_rrc_DL_TransportChannelType_r7_dch:
      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
      {
        if (get_dl_dch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {
        if (transition_config.toc_usage == TOC_INVALID)
        {
          transition_config.toc_usage = TOC_FOR_DCH;
        }
        
        tmp_config_ptr = ordered_config_ptr;
        ordered_config_ptr = transition_config.toc_ptr;
        
        /* Processing functions must think we are going to Cell_DCH state. */
        ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
        
        if (get_dl_dch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;
      }
      break;
      
    case T_rrc_DL_TransportChannelType_r7_fach:
    /*TODO: No need to backup fach mapping. we can delete it later as we don't
        need to restore the mapping*/ 
      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_FACH ||
        ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_PCH ||
        ordered_config_ptr->rrc_state_indicator == RRC_STATE_URA_PCH)
      {
         /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
           if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[lc_tc_cnt],rrc_DL_LogicalChannelMapping_r7,logicalChannelIdentity))
           {
             if(get_fach_mapping_info(ie_ptr->elem[lc_tc_cnt].logicalChannelIdentity, rb_id, logch_mode,
                                      ordered_config_ptr->rrc_state_indicator) == FAILURE)
             {
               return FAILURE;
             }
           }
           else
           {
             if(get_fach_mapping_info(RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT, rb_id, logch_mode,
                                      ordered_config_ptr->rrc_state_indicator) == FAILURE)
             {
               return FAILURE;
             }
           }
      } 
      else
      {
      /* FACH mapping info can come in DCH state. Accept and update
      * local mapping info structure.
        */
         /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
           if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[lc_tc_cnt],rrc_DL_LogicalChannelMapping_r7,logicalChannelIdentity))
           {
             if(get_fach_mapping_info(ie_ptr->elem[lc_tc_cnt].logicalChannelIdentity, rb_id, logch_mode,
                                      RRC_STATE_CELL_DCH) == FAILURE)
             {
               return FAILURE;
             }
           }
           else
           {
             if(get_fach_mapping_info(RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT, rb_id, logch_mode,
                                      RRC_STATE_CELL_DCH) == FAILURE)
             {
               return FAILURE;
             }
           }
      }
      break;
      
    case T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch:
      if (rb_id < DCCH_DT_LOW_PRI_RADIO_BEARER_ID )
      {
        /* Insane network. Mapping SRBs on HSDSCH */
        WRRC_MSG1_ERROR("RRCHS:RB %d cannot be mapped on HSDSCH",rb_id);
        return FAILURE;
      }

      if (ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
      {
        if (get_dl_dch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
        if (get_hsdsch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          return FAILURE;
        }
      } 
      else
      {
        if (transition_config.toc_usage == TOC_INVALID)
        {
          transition_config.toc_usage = TOC_FOR_DCH;
        }
        
        tmp_config_ptr = ordered_config_ptr;
        ordered_config_ptr = transition_config.toc_ptr;
        
        /* Processing functions must think we are going to Cell_DCH state. */
        ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
        
        if (get_dl_dch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        if (get_hsdsch_mapping_info_r7(&ie_ptr->elem[lc_tc_cnt], rb_id, logch_mode) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;
          return FAILURE;
        }
        
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;
      }
      break;

    case T_rrc_DL_TransportChannelType_r7_dsch:
    case T_rrc_DL_TransportChannelType_r5_dch_and_dsch:
    default:
      WRRC_MSG1_ERROR("DSCH TrCH not supported/Rcv'd invalid DL TrCh type %d, return FAILURE",
        ie_ptr->elem[lc_tc_cnt].dl_TransportChannelType.t);
      return FAILURE;
    } /* end Logical channel type switch */
    
    /* Get pointer to the next item in the list */
    
    lc_tc_cnt++;
  } /* end while ie_ptr != NULL */
  
  return SUCCESS;

}
/*==========================================================
FUNCTION  : get_rb_mapping_info_r7()

DESCRIPTION : Get Radio Bearer Mapping Info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rb_mapping_info_r7
(
rrc_RB_MappingInfo_r7      * ie_ptr,
rrc_RB_Identity                 rb_id,
uecomdef_logch_mode_e_type logch_mode,
boolean one_sided_reest_present,
boolean one_sided_reest,
boolean allow_new_channels
)
{
  /*
  * Count to make sure that don't exceed the maximum number 
  * of RB Mux options
  */
  uint8 rb_mux_cnt = 0;


  /* Process the RB Mux option IE in the linked list */
  while ((ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5))
  {

    /* Check for DL Channel Mapping List */
    if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption_r7,dl_LogicalChannelMappingList))
    {
      MSG_MED("Get RB %d Mapping Info for DL", rb_id,0,0);
      if (get_dl_logical_channel_info_r7(&ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList,
                                         rb_id, logch_mode
                                  ) == FAILURE)
      {
        return FAILURE;
      }
      /* Here indicate that MAC Downlink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
    }  /* end dl Log Ch mapping list */

    if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption_r7,ul_LogicalChannelMappings))
    {
      MSG_MED("Get RB %d Mapping Info for UL", rb_id,0,0);
      if (get_ul_logical_channel_info_r6(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,
                                      rb_id, logch_mode
                                      , one_sided_reest_present, one_sided_reest,allow_new_channels
                                       ) == FAILURE)
      {
        return FAILURE;
      }

      /* Here indicate that MAC Uplink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
    }  /* end ul Log Ch mapping list */
    rb_mux_cnt++;
  }  /* end loop of RB Mapping info list */
#ifdef FEATURE_WCDMA_HS_FACH
  rrcllc_backup_mapping_info_into_hanging(ie_ptr,REL_7,rb_id,logch_mode);
#endif

  return SUCCESS;
}

/*==========================================================
FUNCTION  : get_dl_am_rlc_info_r7()

DESCRIPTION : 
  Read the information for Downlink AM RLC from the 
  received structure and write them into one of the 
  pre-allocated AM memory block.

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_dl_am_rlc_info_r7
(
rrc_DL_AM_RLC_Mode_r7 *ie_ptr,
uint8               entity_idx,
rrc_RB_Identity          rb_id,
boolean       allow_new_channels,
rlc_reestablish_e_type reestablish_entity
)
{
  rlc_dl_info_type *rlc_dl_info;
  
  /* local copy of poitner */
  if (entity_idx == UE_MAX_AM_ENTITY)
  {
    entity_idx = ordered_config_ptr->rlc_am_parms.nentity;
  }
  rlc_dl_info = 
    &ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
  
  /* Turn on the information included flag  */
  rlc_dl_info->rlc_info_incl = TRUE;
  
  /* indicates if in sequence delivery is required */
  rlc_dl_info->in_sequence = ie_ptr->inSequenceDelivery;
  
  /* Get the maximum number of RLC PUs allowed to be rcv'd  */
  if ((rlc_dl_info->rx_window_size = get_rx_window_size(ie_ptr->receivingWindowSize))
    == RRCLLC_INVALID_WINDOW_SIZE)
  {
    return FAILURE;
  }
  
  /* Get the downlink status info */
  if (get_dl_rlc_status_info(rlc_dl_info, &ie_ptr->dl_RLC_StatusInfo) == FAILURE)
  {
    return FAILURE;
  }
  
  
  if(ie_ptr->dl_RLC_PDU_size.t == T_rrc_DL_AM_RLC_Mode_r7_dl_RLC_PDU_size_fixedSize)
  {

    if(rrcllc_process_dl_pdu_size(ie_ptr->dl_RLC_PDU_size.u.fixedSize, entity_idx,allow_new_channels,reestablish_entity)
     == SUCCESS)
    {
      /* Set this RLC Size in the global structure rlc_size_per_rb */
      if (rrc_set_dl_am_rlc_pdu_size(rb_id, ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.dl_rlc_size) == FAILURE)
      {
        WRRC_MSG0_ERROR("Unable to set DL AM PDU Size Info in rlc_per_rb");
        return FAILURE;
      }
    }
    else
    {
      return FAILURE;
    }
  }
  else if(ie_ptr->dl_RLC_PDU_size.t == T_rrc_DL_AM_RLC_Mode_r7_dl_RLC_PDU_size_flexibleSize)
  {
   rrcllc_set_am_dl_rlc_size_ota(
     rb_id,RRCLLC_INVALID_RLC_SIZE ,RRC_FLEXIBLE_PDU_SIZE_SET

      );

    if(ie_ptr->dl_RLC_PDU_size.u.flexibleSize == rrc_DL_AM_RLC_Mode_r7_flexibleSize_size15)
    {
      rrcllc_process_flexible_pdu_dl_am(LI_15_BITS, entity_idx,allow_new_channels,reestablish_entity);
    }
    else if (ie_ptr->dl_RLC_PDU_size.u.flexibleSize == rrc_DL_AM_RLC_Mode_r7_flexibleSize_size7)
    {
      rrcllc_process_flexible_pdu_dl_am(LI_7_BITS, entity_idx,allow_new_channels,reestablish_entity);
    }
    else
    {
      WRRC_MSG1_ERROR("RRCHSPA+: Invalid value %d  for Li size for flexible PDU  ",ie_ptr->dl_RLC_PDU_size.u.flexibleSize);
      return FAILURE;
    }
   
  }

  /* everything is a go */
  return SUCCESS;

}
/*==========================================================
FUNCTION  : get_rlc_mode_r7()

DESCRIPTION : Gets the RLC mode for a given logical channel.

DEPENDENCIES: None.

RETURN VALUE: uecomdef_logch_mode_e_type indicate whether or 
              not it is TM, UM or AM.

SIDE EFFECTS: None.
============================================================*/
uecomdef_logch_mode_e_type get_rlc_mode_r7
(
  rrc_RLC_Info_r7 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  uecomdef_logch_mode_e_type dl_logch_mode = UE_MODE_MAX_NUM;

  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,ul_RLC_Mode))
  {
    switch (ie_ptr->ul_RLC_Mode.t)
    {
      case T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode:
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;
  
      case T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode:
        logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;
  
      case T_rrc_UL_RLC_Mode_ul_TM_RLC_Mode: 
        logch_mode = UE_MODE_TRANSPARENT;
        break;
  
      case T_rrc_UL_RLC_Mode_spare:
      default:
        WRRC_MSG1_ERROR("Unknown UL RLC mode %d", ie_ptr->ul_RLC_Mode.t);
        logch_mode = UE_MODE_MAX_NUM;
        break;
    }  /* end UL RLC t */
  }  /* end if UL RLC present */

  /* Downlink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_r7_dl_AM_RLC_Mode:
        dl_logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        break;
  
      case T_rrc_DL_RLC_Mode_r7_dl_UM_RLC_Mode:
        dl_logch_mode = UE_MODE_UNACKNOWLEDGED;
        break;
  
      case T_rrc_DL_RLC_Mode_r7_dl_TM_RLC_Mode: 
        dl_logch_mode = UE_MODE_TRANSPARENT;
        break;
  
      default:
        WRRC_MSG1_ERROR("Unknown DL RLC mode %d", ie_ptr->ul_RLC_Mode.t);
        dl_logch_mode = UE_MODE_MAX_NUM;
        break;
    }  /* end DL RLC choice */

    /* If both DL and UL mode is present, check to make sure they are the same */
    if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,ul_RLC_Mode))
    {
      if (logch_mode != dl_logch_mode) 
      {
        WRRC_MSG2_ERROR("UL %d and DL %d lc mode not a match.  Invalid config",logch_mode,dl_logch_mode);
        logch_mode = UE_MODE_MAX_NUM;
      }
    }
    else
    {
      logch_mode = dl_logch_mode;
    }

  }  /* end if DL RLC present */

  /* Need to know RLC type in either UL or DL */
  if (!((ie_ptr->m.ul_RLC_ModePresent) ||
                            (ie_ptr->m.dl_RLC_ModePresent)))
  {
    WRRC_MSG0_ERROR("Neither DL nor UL RLC mode present!");
  }


  return logch_mode;

} /* end get_rlc_mode_r6 */

/*==========================================================
FUNCTION  : get_rlc_info_r7 ()

DESCRIPTION : 
  Processes RLC Info IE

DEPENDENCIES: 
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
  None.
============================================================*/
static uecomdef_status_e_type get_rlc_info_r7
(
rrc_RLC_Info_r7 *ie_ptr,
rrc_RB_Identity    rb_id,
boolean       allow_new_channels
)
{
  uint8 entity_idx;
  boolean new_rlc_am_entity = FALSE;
  rlc_reestablish_e_type reestablish_entity = RLC_RE_ESTABLISH_NONE;

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,altE_bitInterpretation))
  {
    if(((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,ul_RLC_Mode)) &&
        (ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode ))
        ||((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,dl_RLC_Mode)) &&
           (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->dl_RLC_Mode,rrc_DL_RLC_Mode_r7_dl_UM_RLC_Mode))))
    {
      if(nv_gcf_test_flag == FALSE)
      {
        WRRC_MSG0_ERROR("N\\W send ALT E Bit, ALT E Bit not supported");
        return FAILURE;
      }
    }
  }


  /* Uplink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,ul_RLC_Mode))
  {
    if(get_ul_rlc_info(&ie_ptr->ul_RLC_Mode,rb_id,allow_new_channels,
        &new_rlc_am_entity) == FAILURE)
    {
      return FAILURE;
    }

    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_UM_RLC_Mode )
    {
       entity_idx = get_ul_um_index(rb_id);

      if(entity_idx < UE_MAX_UL_UM_CHANNEL)
      {
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,altE_bitInterpretation))
        {
          ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].alternate_e_bit = TRUE;
        }
        else
        {
          ordered_config_ptr->rlc_ul_um_parms.chan_info[entity_idx].alternate_e_bit = FALSE;
        }
      }
    }

    if(ie_ptr->ul_RLC_Mode.t == T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode )
    {
      entity_idx = get_am_index_wo_f3(rb_id);
  
      if(entity_idx >= UE_MAX_AM_ENTITY)
      {
        WRRC_MSG0_ERROR("The corresponding UL AM info not present");
        return FAILURE;
      }
      else
      {
        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(ie_ptr,useSpecialValueOfHEField))
        {
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config = TRUE;
        }
        else
        {
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config = FALSE;
        }    
      }
    }
  }/* end if UL RLC present */
  
  /* Downlink RLC info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,dl_RLC_Mode))
  {
    switch (ie_ptr->dl_RLC_Mode.t)
    {
      case T_rrc_DL_RLC_Mode_r7_dl_AM_RLC_Mode: /* rrc_dl_AM_RLC_Mode_r7_chosen */
        
        entity_idx = get_am_index_wo_f3(rb_id);
        
        /* LLC processes the UL AM info first, it should find the corresponding
        entity index for DL AM info to be filled in. */
        if (entity_idx == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG0_ERROR("Not allowed to add new DL AM RLC entity");
          return FAILURE;
        }
        if (ie_ptr->rlc_OneSidedReEst == TRUE)
        {
          reestablish_entity = RLC_RE_ESTABLISH_DL;
        }
        else
        {
          reestablish_entity = RLC_RE_ESTABLISH_DL_UL;
        }
        WRRC_MSG2_MED("One-Sided Re-est from OTA %d,so set downlink local_reestablish_entity to [DL_UL/DL/UL/None]%d",ie_ptr->rlc_OneSidedReEst,reestablish_entity);
        
        if (get_dl_am_rlc_info_r7(ie_ptr->dl_RLC_Mode.u.dl_AM_RLC_Mode, entity_idx, rb_id
          , allow_new_channels, reestablish_entity) == FAILURE)
        {
          return FAILURE;
        }
        if(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7)
        {
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.special_value_he_config = TRUE;
        }
        else
        {
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.special_value_he_config = FALSE;
        }
        
        /* This flag is set when processing UL AM info. */
        if(new_rlc_am_entity)
        {
        /* Indicate that the AM entity has been filled up. LLC does not increase this
        number of entities when process UL AM info.
          */
          ordered_config_ptr->rlc_am_parms.nentity++;
          
          /* Make sure UE has not exceeded the maximum number of AM Entities */
          if(ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY)
          {
            WRRC_MSG1_ERROR("Too Many AM Entities %d", ordered_config_ptr->rlc_am_parms.nentity);
            return FAILURE;
          }
          
          /* It always assume to be the shared channel (Control&Data) 
          as we support now.
          */  
          ordered_config_ptr->rlc_am_parms.am_info[entity_idx]->dl_nchan = 1;
          
        }
        
        break;  /* DL AM */
        
      case T_rrc_DL_RLC_Mode_r7_dl_UM_RLC_Mode: /* rrc_dl_UM_RLC_r6_Mode_chosen NULL */   

        if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RLC_Info_r7,altE_bitInterpretation))
        {
          if(get_dl_rlc_um_info_r6(ie_ptr->dl_RLC_Mode.u.dl_UM_RLC_Mode,rb_id,allow_new_channels,TRUE)
              == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {
          if(get_dl_rlc_um_info_r6(ie_ptr->dl_RLC_Mode.u.dl_UM_RLC_Mode,rb_id,allow_new_channels,FALSE)
              == FAILURE)
          {
            return FAILURE;
          }
        }
        break;
        
      case T_rrc_DL_RLC_Mode_r7_dl_TM_RLC_Mode:
        if(get_dl_rlc_tm_info(ie_ptr->dl_RLC_Mode.u.dl_TM_RLC_Mode->segmentationIndication,
            rb_id,allow_new_channels) == FAILURE)
        {
          return FAILURE;
        }
        break;
        
      default:
        WRRC_MSG1_ERROR("Unknown DL RLC mode %d", ie_ptr->dl_RLC_Mode.t);
        return FAILURE;
    } /* end DL RLC choice */
  } /* end if DL RLC present */

  /* Got here, everything is a go */
  return SUCCESS;

} /* end get_rlc_info_r6() */

/*==========================================================
FUNCTION  : get_rb_info_to_reconfig_r7()

DESCRIPTION : Get Radio Bearer Information to reconfig.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rb_info_to_reconfig_r7
(
rrc_RB_InformationReconfig_r7 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  boolean  one_sided_reest_present=FALSE;
  boolean  one_sided_reest=FALSE;
#ifdef FEATURE_MAC_I
  uint8 entity_idx=UE_MAX_AM_ENTITY;
#endif

  /* Check to see if the RLC info is present */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,rlc_Info))
     && ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode))))
  {
    /* Get the RLC mode */
    if ((logch_mode = get_rlc_mode_r7(&ie_ptr->rlc_Info)) == UE_MODE_MAX_NUM)
    {
      return FAILURE;
    }
    
    /* Save off the RLC Info */
    if (get_rlc_info_r7(&ie_ptr->rlc_Info, ie_ptr->rb_Identity, FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end RLC Info Check */
  

#ifdef FEATURE_MAC_I
/*Copy rlc one side re-establishment flag to OC*/
if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,rlc_Info)) &&(ie_ptr->rb_Identity >= DCCH_AM_RADIO_BEARER_ID))
{
  if (ie_ptr->rlc_Info.rlc_OneSidedReEst == TRUE)
  {
  
    entity_idx = get_am_index_wo_f3(ie_ptr->rb_Identity);
    if (entity_idx != UE_MAX_AM_ENTITY)
    {
      ordered_config_ptr->rlc_one_sided_reest[entity_idx] = TRUE;
    }
  }
}
#endif /* FEATURE_MAC_I */

  /* Save off the RB Mapping info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,rb_MappingInfo))
  {
    /* Remove any existing mapping for the RB */
    unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);
    
    if(ie_ptr->m.rlc_InfoPresent)
    {
      one_sided_reest_present=TRUE;
      one_sided_reest =ie_ptr->rlc_Info.rlc_OneSidedReEst;
    }
    
    if (get_rb_mapping_info_r7(&ie_ptr->rb_MappingInfo, (rb_id_type)ie_ptr->rb_Identity, logch_mode
        ,one_sided_reest_present,one_sided_reest,FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end of RB Mapping info check */
  
#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,pdcp_Info))
  {
    if (rrc_pdcp_enabled)
    {
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,
        &ie_ptr->pdcp_Info.pdcp_Info, 
        ie_ptr->rb_Identity, 
        CPDCP_RECONFIG) == FAILURE)
      {
        return FAILURE;
      }
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
    {
      WRRC_MSG0_ERROR("PDCP Info not supported");
      return FAILURE;
    }
  }
#else

  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r7,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }

#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_reconfig_r6() */

/*==========================================================
FUNCTION  : get_rb_info_affected_r7()

DESCRIPTION : Get Radio Bearer Information to be affected.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rb_info_affected_r7
(
rrc_RB_InformationAffected_r7*ie_ptr
)
{

  /* Remove any existing mapping for the RB */
  unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);


  /* Get the RB Mapping info */
  return(get_rb_mapping_info_r7(&ie_ptr->rb_MappingInfo, 
                                ie_ptr->rb_Identity, 
                                UE_MODE_MAX_NUM
                                ,FALSE,FALSE,FALSE));

}  /* end get_rb_info_affected_r6() */

/*==========================================================
FUNCTION  : get_rb_info_to_setup_r7()

DESCRIPTION : Get Radio Bearer Information to setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_setup_r7
(
  rrc_RB_InformationSetup_r7       *ie_ptr,
#ifdef FEATURE_UMTS_PDCP
  rrc_CN_DomainIdentity cn_Domain,
#endif /* FEATURE_UMTS_PDCP*/
  rrc_RB_InformationSetupList_r7   * rb_setup_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type    logch_mode = UE_MODE_MAX_NUM;
  
  /*initialize a counter to check rb count*/
  uint8                         rb_cnt  = 0;
  
  
  
  /*place holder for rb_id we get originally*/
  rrc_RB_Identity                    local_rb_id = ie_ptr->rb_Identity;
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_RB_InformationSetup_r7     *temp_ie_ptr = ie_ptr;

#ifdef FEATURE_UMTS_PDCP
  uint8 idx = 0;
#endif /* FEATURE_UMTS_PDCP */  
  
  boolean  one_sided_reest_present=FALSE;
  boolean one_sided_reest =FALSE;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the rb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_same_as_RB))
  {
    while ((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
    {
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == 
        rb_setup_ptr->elem[rb_cnt].rb_Identity)
      {
        temp_ie_ptr = &(rb_setup_ptr->elem[rb_cnt]);
        break;
      }
     
      rb_cnt++;
    } /* end RB per RAB setup */
    
    if ((rb_setup_ptr->n == rb_cnt) || (rb_cnt >= MAX_RB_PER_RAB))
    {
       WRRC_MSG0_ERROR("Unable to match rb_id to same_as_RB"); 
      return FAILURE;
    }
  } /* if RLC info choice is "Same as RB" type */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r7_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", local_rb_id);
      return FAILURE;
    }
  } 
  
    /* Now we have either the RLC Info explicitly meant for this RB OR
    * the RLC Info that is the same as another RB.Get the RLC mode */
  
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
       ((logch_mode = get_rlc_mode_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
     )
  {
    return FAILURE;
  }
  
  /* Save off the RLC Info */
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
      (get_rlc_info_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, local_rb_id, TRUE) == FAILURE))
  {
    return FAILURE;
  }
  
  if (temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info)
  {
    one_sided_reest_present=TRUE;
    one_sided_reest =temp_ie_ptr->rlc_InfoChoice.u.rlc_Info->rlc_OneSidedReEst;
  }

  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r7(&ie_ptr->rb_MappingInfo, local_rb_id, logch_mode
       ,one_sided_reest_present,one_sided_reest,TRUE) == FAILURE)
  {
    return FAILURE;
  }

#ifdef FEATURE_UMTS_PDCP

  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r7,pdcp_Info))
  {
    if (rrc_pdcp_enabled)  
    { 
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,&ie_ptr->pdcp_Info, 
        local_rb_id, CPDCP_SETUP) == FAILURE)
      {
        return FAILURE;
      }
      return SUCCESS;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r7,pdcp_Info)) &&
            ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
            (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
            ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
            (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,
            rrc_LosslessSRNS_RelocSupport_supported)))))
    {
      WRRC_MSG0_ERROR("PDCP NV not enabled.  PDCP info not handled");
      return FAILURE;
    }
  }

/*If CS over HSPA is being setup and PDCP Info IE is missing for this RB, return failure.*/  

  /* if fall-thru happens, PDCP layer has to be setup in transparent [pass thru] mode */
  if (cn_Domain == rrc_CN_DomainIdentity_ps_domain) 
  {
    WRRC_MSG1_MED("Setting up PDCP in transparent mode rb_id %d",local_rb_id);
    /*Get the next free index in pdcp_config_req */
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use == FALSE) 
      {
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id = local_rb_id;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
        ordered_config_ptr->pdcp_parms.num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
    /* CPDCP action will be setup, but pdcp info will be null */
    //ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
  }
#else
  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r7,pdcp_Info)) &&
            ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
            (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
            ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
            (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,
            rrc_LosslessSRNS_RelocSupport_supported)))))
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }
#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

}
/*============================================================================
FUNCTION: rrcllc_process_rb_info_reconfig_list_r7()

DESCRIPTION:
  This function process IE rrc_RB_InformationReconfigList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_reconfig_list_r7
(
  uint32 rb_info_reconfig_list_present,
  rrc_RB_InformationReconfigList_r7 * rb_reconfig_ptr
)  
{

  if (rb_info_reconfig_list_present)
  {
   
    uint32 idx = 0;
   
    idx = 0;
    while((rb_reconfig_ptr->n > idx) && (idx < MAX_RB))
    {
      if(get_rb_info_to_reconfig_r7(&rb_reconfig_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
     
      idx++;
    } /* end RB reconfig */
  }

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_process_rb_info_affected_list_r7()

DESCRIPTION:
  This function process IE rrc_RB_InformationAffectedList_r7 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_affected_list_r7
(
  OSS_UINT32 rb_info_affected_list_present,
  rrc_RB_InformationAffectedList_r7 * rb_affected_ptr
)  
{
  /* Get the affected RB list if present */
  if(rb_info_affected_list_present)
  {
    
    uint32 idx = 0;
  
   
    
    while((rb_affected_ptr->n > idx) && (idx < MAX_RB))
    {
      if(get_rb_info_affected_r7(&rb_affected_ptr->elem[idx]) == FAILURE)
      {
        return (FAILURE);
      }
     
      idx++;
    }
  } /* end Affected RB */

  return(SUCCESS);
}



/*==========================================================
FUNCTION  : get_srb_info_to_setup_r7()

DESCRIPTION : Get Signalling Radio Bearer Information to setup.

REUSABILITY: R6

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_srb_info_to_setup_r7
(
  rrc_SRB_InformationSetup_r7     *ie_ptr,
  rrc_SRB_InformationSetupList_r7  *srb_setup_ptr,
  rrc_RB_Identity                   rb_id
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  
  /* Local count of # of SRB's to setup */
  uint8                           srb_cnt = 0;
  
  /* temp rb id which we shall compute if rb_id is not part of
  * srb_setup_ptr when we loop thru the list to get a match
  */
  rrc_RB_Identity                      temp_rb_id = 0;
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_SRB_InformationSetup_r7     *temp_ie_ptr = ie_ptr;
  
  /* Entity indexes in case we need to look up RLC info from
  * our internal database instead of processing an IE.
  */
  uint8 entity_idx, new_entity_idx;
  boolean  one_sided_reest_present=FALSE;
  boolean  one_sided_reest =FALSE;

  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the srb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_same_as_RB))
  {
    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        temp_rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        temp_rb_id++;
      }
      
      /*check if the rb_id matches*/
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == temp_rb_id)
      {
        temp_ie_ptr = &(srb_setup_ptr->elem[srb_cnt]);
        break;
      }
      
      srb_cnt++;
    } /* end while... */
    
    if ((srb_setup_ptr->n == srb_cnt) || (srb_cnt >= UE_MAX_SRB_SETUP))
    {
      WRRC_MSG1_HIGH("same_as_RB %d not in msg, lookup req'd",
        ie_ptr->rlc_InfoChoice.u.same_as_RB);
      temp_ie_ptr = NULL;
      temp_rb_id = ie_ptr->rlc_InfoChoice.u.same_as_RB;
      
      if ((entity_idx = get_am_index(temp_rb_id)) != UE_MAX_AM_ENTITY)
      {
      /* This RB is in AM mode. And its reference config is found in OC.
        */
        if ((new_entity_idx = rrcllc_get_1st_hole_for_rlc_am()) == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG1_ERROR("Can't find space for new rb %d", rb_id);
          return FAILURE;
        }
        ordered_config_ptr->am_config[new_entity_idx].rlc_ul_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->common_config.rb_id = rb_id;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan++;
        
        ordered_config_ptr->am_config[new_entity_idx].rlc_dl_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan++;
        
        ordered_config_ptr->rlc_am_parms.nentity++;
        if((ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan >
                                                       UE_MAX_UL_AM_DATA_CHANNEL) || 
           (ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan >
                                                       UE_MAX_DL_AM_DATA_CHANNEL) ||
           (ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY))
        {
          ERR("Too many UL/DL AM channels or entities [DL,UL,entities] [%d,%d,%d]",
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.nentity);
          return FAILURE;
        }
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      
      if ((entity_idx = get_ul_um_index(rrclcm_get_ul_rlc_lc_id(temp_rb_id)))
        != UE_MAX_UL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_ul_um_info(rb_id, TRUE) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      if ((entity_idx = get_dl_um_index(temp_rb_id, ordered_config_ptr))
        != UE_MAX_DL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_dl_um_info(rb_id, TRUE, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      /* SRBs cannot be TM.  If we haven't found it by now, we have
      * a problem. */
      if (logch_mode == UE_MODE_MAX_NUM)
      {
        WRRC_MSG1_ERROR("same_as_RB %d not found in msg or OC",
          ie_ptr->rlc_InfoChoice.u.same_as_RB);
        return FAILURE;
      }
    }
  } /* end RLC choice == Same as RB */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r7_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", rb_id);
      return FAILURE;
    }
  } 


  /* Now we have either the RLC Info explicitly meant for this RB OR
   * the RLC Info that is the same as another RB.  If tmp_ie_ptr is NULL,
   * then we've already copied the RLC Info from our internal database.
   */
  if (temp_ie_ptr != NULL)
  {
    /* Get the RLC mode */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
         ((logch_mode = get_rlc_mode_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
       )
    {
      return FAILURE;
    }
      
    /* Get the RLC info */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
         (get_rlc_info_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, rb_id, TRUE) == FAILURE))
    {
      return FAILURE;
    }
  }

  if ((temp_ie_ptr != NULL) && (temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info))
  {
    one_sided_reest_present=TRUE;
    one_sided_reest=temp_ie_ptr->rlc_InfoChoice.u.rlc_Info->rlc_OneSidedReEst;
  }
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r7(&ie_ptr->rb_MappingInfo, rb_id, logch_mode
       ,one_sided_reest_present,one_sided_reest,TRUE) == FAILURE)
  {
    return FAILURE;
  }
  
  if(FAILURE == rrcllc_validate_srb_id(rb_id))
  {
    return FAILURE;
  }

  /* everything is configured */
  return SUCCESS;

}
/*============================================================================
FUNCTION: rrcllc_process_srb_info_setup_list_r7()

DESCRIPTION:
  This function process IE rrc_SRB_InformationSetupList_r7 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_srb_info_setup_list_r7
(
  OSS_UINT32 srb_info_setup_list_present,
  rrc_SRB_InformationSetupList_r7 * srb_setup_r7_ptr
)
{
  rrc_SRB_InformationSetup_r7 * srb5_setup_ptr = NULL;
  rrc_RB_Identity                          srb_id = RRCLLC_INVALID_RB_ID;

  if (srb_info_setup_list_present)
  {
    uint8                           srb_cnt =0;
    rrc_RB_Identity                          rb_id = 0;
    
    while((srb_setup_r7_ptr->n > srb_cnt ) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if(srb_setup_r7_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_r7_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        rb_id++;
      }
      
      /* Save off the RB Mapping info for each SRB */
      if(get_srb_info_to_setup_r7(
        &srb_setup_r7_ptr->elem[srb_cnt], 
        (srb_setup_r7_ptr), 
        rb_id)
        == FAILURE)
      {
        return FAILURE;
      }
      if(rb_id > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        /* Pick that pointer that has RB Id > 4 & 
         assume it to be SRB#5 pointer for the time being */

        WRRC_MSG0_HIGH("Probable SRB#5 IE in the message .. processing");
        srb5_setup_ptr = &srb_setup_r7_ptr->elem[srb_cnt];

        srb_id = rb_id;
      }
      srb_cnt++;
    } /* end SRB setup */

  }
  /* We are just validating one more SRB, incase 
     SRB#5 is given by n/w */
  if(srb5_setup_ptr != NULL)
  {
  
    if(rrcllc_update_oc_with_srb5_rel7_ie(srb5_setup_ptr, srb_id) == FAILURE)
    {
      WRRC_MSG0_ERROR("Rejecting SRB#5 configuration given by the network");
      return FAILURE;
    }
  }
#ifdef FEATURE_CHANGE_SRB4_PRIRORITY
  rrcllc_check_and_set_srb3_srb4_priority(); 
#endif

  return(SUCCESS);
}

/*============================================================================
FUNCTION: rrcllc_process_rab_info_setup_list_r7()

DESCRIPTION:
  This function process IE rrc_RB_InformationAffectedList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rab_info_setup_list_r7
(
  OSS_UINT32 rab_info_setup_list_present,
  rrc_RAB_InformationSetupList_r7 * rab_setup_ptr
)
{
  if(rab_info_setup_list_present)
  {
    /* Local count of # of RAB's & RB's to setup */
    uint32                             rab_cnt = 0;
    uint32                             rb_cnt  = 0;    
   
    rrc_RB_InformationSetupList_r7     * rb_setup_ptr;
  
    
    while((rab_setup_ptr->n > rab_cnt) && (rab_cnt < MAX_RAB_TO_SETUP))
    {
      rb_setup_ptr  = &rab_setup_ptr->elem[rab_cnt].rb_InformationSetupList;
      
      rb_cnt =0;
      
      while((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
      {
        if(get_rb_info_to_setup_r7(
           &rb_setup_ptr->elem[rb_cnt],
#ifdef FEATURE_UMTS_PDCP
           rab_setup_ptr->elem[rab_cnt].rab_Info.cn_DomainIdentity,
#endif /* FEATURE_UMTS_PDCP */           
           rb_setup_ptr) == FAILURE)
        {
          return FAILURE;
        }
       
        rb_cnt++;
      } /* end RB per RAB setup */
      
      
      rab_cnt++;
    } /* end rab setup */
  } /* end RAB setup */

  return(SUCCESS);
}

/*==========================================================
FUNCTION  : rrcllc_update_ul_ded_log_chl_info_r8()

DESCRIPTION : Processes UL Logical channel info and fills it
              in passed pointer to MAC I/F struct.

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None.
============================================================*/
static void  rrcllc_update_ul_ded_log_chl_info_r8
 (
  rrc_UL_LogicalChannelMapping_r8 *ie_ptr, 
  mac_ul_ded_logch_config_type  *dlchan_info,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
 )
{
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */ 
  if(ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
  {
    dlchan_info->mac_id =
      (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
  }
  else
  {
    dlchan_info->mac_id = 
      RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
  }

  /* MAC Logical Channel Priority */
  dlchan_info->priority =
    (uint8)ie_ptr->mac_LogicalChannelPriority;

  /* Save Radio Bearer ID */
  dlchan_info->rb_id = rb_id;

  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    dlchan_info->chan_type = UE_LOGCHAN_DCCH;
  }
  else
  {
    dlchan_info->chan_type = UE_LOGCHAN_DTCH;
  }

  /* Save the logical channel mode, if a setup */
  if(logch_mode != UE_MODE_MAX_NUM)
  {
    dlchan_info->rlc_mode = logch_mode;
  }
}  /* rrcllc_update_ul_ded_log_chl_info */

/*==========================================================
FUNCTION  : get_rlc_size_list_r8()

DESCRIPTION : Processes the RLC Size List IE contents and saves 
              them in the RLC size restriction database.  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: The RLC size restriction database is updated.
============================================================*/
static uecomdef_status_e_type get_rlc_size_list_r8
(
  rrc_UL_LogicalChannelMapping_r8 *ie_ptr,
  rrcllcoc_rlc_size_list_type  *rlc_size_rec,
  rrc_RB_Identity                    rb_id
)
{
  rrc_RLC_SizeExplicitList *rlc_size_list = NULL;

  rlc_size_rec->rb_id = rb_id;
  switch(ie_ptr->ul_TrCH_Type.u.dch_rach_usch->rlc_SizeList.t)
  {
    case T_rrc_UL_LogicalChannelMapping_r8_rlc_SizeList_allSizes:
      rlc_size_rec->restriction_type = RLC_SIZES_ALL;
      break;

    case T_rrc_UL_LogicalChannelMapping_r8_rlc_SizeList_configured:
      rlc_size_rec->restriction_type = RLC_SIZES_CONFIGURED;
      break;

    case T_rrc_UL_LogicalChannelMapping_r8_rlc_SizeList_explicitList:
      rlc_size_rec->restriction_type = RLC_SIZES_EXPLICIT_LIST;
      rlc_size_list = ie_ptr->ul_TrCH_Type.u.dch_rach_usch->rlc_SizeList.u.explicitList;
      if(rlc_size_list == NULL)
      {
        WRRC_MSG0_ERROR("RLC Size List empty");
        return(FAILURE);
      }
      rlc_size_rec->count = 0;
      while(rlc_size_list->n > rlc_size_rec->count)
      {
        rlc_size_rec->size_idx[rlc_size_rec->count] = 
			(uint16)rlc_size_list->elem[rlc_size_rec->count].rlc_SizeIndex;
        rlc_size_rec->count++;
      }
      break;

    default:
      WRRC_MSG1_ERROR("Invalid RLC Size List CHOICE: %d", ie_ptr->ul_TrCH_Type.u.dch_rach_usch->rlc_SizeList.t);
      return(FAILURE);
  }

  return SUCCESS;
}

/*==========================================================
FUNCTION  : get_rach_mapping_info_r8()

DESCRIPTION : Process RACH Mapping IE's contents and saves 
              them in the MAC Ordered Configuration Database.
              If next RRC state is CELL_DCH, save the RACH
              mapping contents in a local data structure in
              OC.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_rach_mapping_info_r8
(
  rrc_UL_LogicalChannelMapping_r8 *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode,
  rrc_state_e_type             next_rrc_state
)
{
  /* transport Channel Index */
  uint8   trch_idx  = RRCLLC_INVALID_TRCH_IDX;

  /* Get the logical channel index */
  uint8   logch_idx, cnt;
 
  mac_ul_ded_logch_config_type  *dlchan_info;
  rlc_lc_id_type   rlc_lc_id = (uint8)RRCLCM_RLC_LC_ID_NOT_FOUND;
  /* TBD: Although all existing FACHs would be mapped to each of the RBs,
   * fill up only for the 1st FACH - till MAC interface is clarified/updated
   */

  boolean update_rach_rlc_id = FALSE;

  /* Temporary pointer for processing the RLC Size List */
  rrcllcoc_rlc_size_list_type *rlc_size_rec = NULL;


  /* Start with updating Cell FACH RB Mapping Info when the next state is
   * either CELL_DCH or CELL_FACH
   */
  logch_idx = ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan;
  dlchan_info = &ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx];

  /* Save the RLC Size List information */
  ordered_config_ptr->rach_rlc_size_restriction_info.mac_update_needed = TRUE;
  rlc_size_rec =
    &(ordered_config_ptr->rach_rlc_size_restriction_info.rlc_size_list[logch_idx]);
  if(get_rlc_size_list_r8(ie_ptr, rlc_size_rec, rb_id) == FAILURE)
  {
    return FAILURE;
  }


  if(logch_mode == UE_MODE_MAX_NUM)
  {
    /* This is the case when RB mapping info applicable to CELL_FACH is coming 
    * in "Affected List". At this point, we need to update the Logical Channel mode 
    * and Logical Channel ID in Cell FACH RB Mapping Info - 
    * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
    */
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
    if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rb_id);
      return(FAILURE);
    }
    
    if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
    }
    /* Also update the Logical Channel ID here */
    dlchan_info->rlc_id = rlc_lc_id;
    update_rach_rlc_id = TRUE;
  }
  else
  {
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id(rb_id);
    if(rlc_lc_id  != RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      /* Also update the Logical Channel ID here */
      dlchan_info->rlc_id = rlc_lc_id;
      update_rach_rlc_id = TRUE;
    }
  }
#ifdef FEATURE_WCDMA_HS_FACH
  for (cnt = 0; cnt < ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ; cnt++) 
  {            
    if(rb_id == ordered_config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[cnt].rb_id)
    {
      WRRC_MSG1_MED("RB_ID %d already present",rb_id);
      return (SUCCESS);
    }    
  }
#endif
  
  rrcllc_update_ul_ded_log_chl_info_r8(ie_ptr, dlchan_info, rb_id, logch_mode);
  ordered_config_ptr->cell_fach_rb_mapping_info.ul_ndlchan ++;  
   
  /* Now update the FACH Info ONLY IF the next state is CELL_FACH */
  if(next_rrc_state == RRC_STATE_CELL_FACH ||
     next_rrc_state == RRC_STATE_CELL_PCH ||
     next_rrc_state == RRC_STATE_URA_PCH)
  {
    if((trch_idx = rrcllc_find_ul_rach_trch_idx()) >= UE_MAX_TRCH)
    {
      return FAILURE;
    }
    logch_idx = ordered_config_ptr->ul_cctrch.rach_info.ndlchan;
    dlchan_info = &ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx];
    if(update_rach_rlc_id)
    {
      dlchan_info->rlc_id = rlc_lc_id;
    }
    rrcllc_update_ul_ded_log_chl_info_r8(ie_ptr, dlchan_info, rb_id, logch_mode);

    ordered_config_ptr->ul_cctrch.rach_info.ndlchan++;
    ordered_config_ptr->ul_rach_trch_info[trch_idx].lg_ch_cnt = 
      ordered_config_ptr->ul_cctrch.rach_info.ndlchan;

  }  /* If next state is CELL_FACH, update the FACH Info */
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
    if ((rb_id > 0) && (FACH_MAPPING_PRESENT & default_cfg_ota_mapping[rb_id-1]) && (rb_id <= NUM_SRB_DC_CELLFACH))
    {
      default_cfg_ota_mapping[rb_id-1]|= FACH_RACH_MAPPING_PRESENT;
    }
#endif
  return SUCCESS;

}



/*==========================================================
FUNCTION  : get_ul_dch_mapping_info_r8()

DESCRIPTION : Process UL DCH Mapping IE's contents and saves 
              them in the MAC Ordered Configuration Database.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_dch_mapping_info_r8
(
  rrc_UL_LogicalChannelMapping_r8 *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  /* transport Channel Index */
  uint8         trch_idx = RRCLLC_INVALID_TRCH_IDX;

  /* Transport Channel ID */
  tr_ch_id_type   trch_id = 0;

  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  uint8 cnt = 0;

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

  /* Temporary pointer for processing the RLC Size List */
  rrcllcoc_rlc_size_list_type *rlc_size_rec = NULL;
#endif/*#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */


  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->ul_TrCH_Type,
  	  rrc_UL_LogicalChannelMapping_r8_ul_TrCH_Type_dch_rach_usch))
  {
    /* Get the UL DCH TrCH ID if it exists */
    switch (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t)
    {
      case T_rrc_UL_TransportChannelType_dch:
        trch_id = (tr_ch_id_type)
          ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.u.dch;
        break;
  
      case T_rrc_UL_TransportChannelType_rach:
      case T_rrc_UL_TransportChannelType_dummy:
      case T_rrc_UL_TransportChannelType_usch:
      default:
        WRRC_MSG0_ERROR("Expecting UL DCH TrCH ID");
        return FAILURE;
    } /* end UL DCH TrCH ID switch */
  
    /* Get the transport channel index for this transport channel ID.
     * Note: The index should already be there.
     */
    trch_idx = rrcllc_find_ul_dch_trch_idx(ordered_config_ptr, trch_id);
    if(trch_idx >= UE_MAX_TRCH)
    {
      return(rrcllc_backup_ul_dch_from_ota_r8(ie_ptr,rb_id,logch_mode));
    }
  
    /* Get the Logical Channel Index */
    logch_idx = ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
  
    for (cnt = 0; cnt < ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan; cnt++) 
    {
      if (rb_id == ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[cnt].rb_id)
      {
        WRRC_MSG1_MED("RB_ID %d already present", rb_id);
        return (SUCCESS); 
      }
    }

    /* If Logical Channel ID is present, save MAC Logical Channel Identity */
    if (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
    {
      
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
        (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
    }
    else 
    {
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].mac_id =
        RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT;
    }

#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH

    /* Save the RLC Size List information */
    ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].mac_update_needed = TRUE;
    rlc_size_rec = &(ordered_config_ptr->dch_rlc_size_restriction_info[trch_id].rlc_size_list[logch_idx]);
    if(get_rlc_size_list_r8(ie_ptr, rlc_size_rec, rb_id) == FAILURE)
    {
      return FAILURE;
    }
#else
    /* Initialize the MAC data structures to allow all RLC sizes, just in case. */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_size_info.all_tfi_valid = TRUE;
#endif /* FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH */

    /* MAC Logical Channel Priority */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].priority =
      (uint8) ie_ptr->mac_LogicalChannelPriority;
  
    /* Save Radio Bearer ID */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rb_id = rb_id;
  
    /* Determine the type of Logical Channel mapped to DCH TrCH */
    if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
    }
    else
    {
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
    }
  
    if(logch_mode == UE_MODE_MAX_NUM)
    {
      /* This is the case when RB mapping info applicable to CELL_FACH is coming 
       * in "Affected List". At this point, we need to update the Logical Channel mode 
       * and Logical Channel ID in Cell FACH RB Mapping Info - 
       * so find the real mode from DCH Info and send that in place of UE_MODE_MAX_NUM
       */
      rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
      if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        WRRC_MSG1_ERROR("Invalid UL LC ID for RB %d", rb_id);
        return(FAILURE);
      }
    
      if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
      {
        WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
      }
  
      /* Also update the Logical Channel ID here */
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_id = rlc_lc_id;
    }
    
    /* Save the logical channel mode, if a setup */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].dlchan_info[logch_idx].rlc_mode = logch_mode;
  
    /* other wise it is just a reconfig of RB Mapping info */
  
    /* Increment number of logical channels */
    ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan++;
    ordered_config_ptr->ul_dch_trch_info[trch_idx].lg_ch_cnt = ordered_config_ptr->ul_cctrch.dch_info.dch_info[trch_idx].ndlchan;
  }
  else /* rrc_ul_TrCH_Type_e_dch_chosen */
  {
    WRRC_MSG0_ERROR("RRCEUL: R6 ASN1 rrc_ul_TrCH_Type_e_dch_chosen not yet supported");
  }
  rrcllc_free_hanging_rb_mapping_info_index(rb_id,UL_DCH_RB_MAPPING);

  return SUCCESS;

}

/*==========================================================
FUNCTION  : get_ul_single_logical_channel_info_r8()

DESCRIPTION : Process one UL Logical Channel Mapping IE's 
              contents and saves them in the MAC Ordered 
              Configuration Database.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_single_logical_channel_info_r8
(
  rrc_UL_LogicalChannelMapping_r8 *ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
 ,boolean one_sided_reest_present
 ,boolean one_sided_reest
 ,boolean allow_new_channels
)
{
  ordered_config_type *tmp_config_ptr = NULL;

  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->ul_TrCH_Type,
  	  rrc_UL_LogicalChannelMapping_r8_ul_TrCH_Type_dch_rach_usch))
  {
    /* Transport channel type */
    switch (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t)
    {
      case T_rrc_UL_TransportChannelType_dch:
        if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
        {
          if (get_ul_dch_mapping_info_r8(ie_ptr, rb_id, logch_mode) == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {
            if(transition_config.toc_usage == TOC_INVALID)
            {
              transition_config.toc_usage = TOC_FOR_DCH;
            }
  
            tmp_config_ptr = ordered_config_ptr;
            ordered_config_ptr = transition_config.toc_ptr;
  
            /* Processing functions must think we are going to Cell_DCH state. */
            ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
  
            if(get_ul_dch_mapping_info_r8(ie_ptr, rb_id, logch_mode) == FAILURE)
            {
              ordered_config_ptr = tmp_config_ptr;
              tmp_config_ptr = NULL;
              return FAILURE;
            }
  
            ordered_config_ptr = tmp_config_ptr;
            tmp_config_ptr = NULL;
        }
        break;
  
      case T_rrc_UL_TransportChannelType_rach:
        if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_FACH ||
           ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_PCH ||
           ordered_config_ptr->rrc_state_indicator == RRC_STATE_URA_PCH)
        {
          if (get_rach_mapping_info_r8(ie_ptr, rb_id, logch_mode,
                              ordered_config_ptr->rrc_state_indicator) == FAILURE)
          {
            return FAILURE;
          }
        }
        else
        {
          if (get_rach_mapping_info_r8(ie_ptr, rb_id, logch_mode, RRC_STATE_CELL_DCH) == FAILURE)
          {
            return FAILURE;
          }
        }
        break;
  
      case T_rrc_UL_TransportChannelType_dummy:
      case T_rrc_UL_TransportChannelType_usch:
      default:
        WRRC_MSG1_ERROR("Rcvd USCH/DUMMY/Invalid UL TrCh type %d", ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t);
        return FAILURE;
    }
  }
  else if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->ul_TrCH_Type,
  	rrc_UL_LogicalChannelMapping_r8_ul_TrCH_Type_e_dch))/* rrc_ul_TrCH_Type_e_dch_chosen */
  {
    if(ordered_config_ptr->rrc_state_indicator == RRC_STATE_CELL_DCH)
    {
      if (logch_mode != UE_MODE_TRANSPARENT) 
      {
        if (get_edch_mapping_info_r8(ie_ptr, rb_id, logch_mode
            ,one_sided_reest_present,one_sided_reest,allow_new_channels
#ifdef FEATURE_MAC_I 
            ,NULL
#endif /* FEATURE_MAC_I */
            ) == FAILURE)
        {
          return FAILURE;
        }
        rrcllc_update_mac_eul_action(TRUE);
      }
      else
      {
        WRRC_MSG0_ERROR("Cannot map TM RB on EDCH");
        return FAILURE;
      }
    }
    else
    {
      if(transition_config.toc_usage == TOC_INVALID)
      {
        transition_config.toc_usage = TOC_FOR_DCH;
      }
      
      tmp_config_ptr = ordered_config_ptr;
      ordered_config_ptr = transition_config.toc_ptr;
      
      /* Processing functions must think we are going to Cell_DCH state. */
      ordered_config_ptr->rrc_state_indicator = RRC_STATE_CELL_DCH;
      
      if (logch_mode != UE_MODE_TRANSPARENT) 
      {
        if (get_edch_mapping_info_r8(ie_ptr, rb_id, logch_mode
            , one_sided_reest_present,one_sided_reest,allow_new_channels
#ifdef FEATURE_MAC_I         
            ,tmp_config_ptr
#endif /* FEATURE_MAC_I */            
            ) == FAILURE)
        {
          ordered_config_ptr = tmp_config_ptr;
          tmp_config_ptr = NULL;  
          return FAILURE;
        }
      }
      else
      {
        WRRC_MSG0_ERROR("Cannot map TM RB on EDCH");
        ordered_config_ptr = tmp_config_ptr;
        tmp_config_ptr = NULL;  
        return FAILURE;
      }
      
      ordered_config_ptr = tmp_config_ptr;
      tmp_config_ptr = NULL;  
    }
#ifdef FEATURE_WCDMA_HS_RACH
/*Always backup the mapping in hanging rb independent of state*/
    {
      rrcllc_backup_eul_from_ota_r8(ie_ptr,rb_id,logch_mode);
    }
#endif
  }
  return SUCCESS;

}

/*==========================================================
FUNCTION  : get_ul_logical_channel_info_r8()

DESCRIPTION : Get UL logical Channel info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
static uecomdef_status_e_type get_ul_logical_channel_info_r8
(
  rrc_UL_LogicalChannelMappings_r8 * ie_ptr,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
 ,boolean one_sided_reest_present
 ,boolean one_sided_reest
 ,boolean allow_new_channels
)
{

  /* Process IE info based on number of UL Logical Channels */
  switch (ie_ptr->t)
  {
    /* One UL Logical Channel */
    case T_rrc_UL_LogicalChannelMappings_r8_oneLogicalChannel:
      if(get_ul_single_logical_channel_info_r8(
         ie_ptr->u.oneLogicalChannel, rb_id, logch_mode
         , one_sided_reest_present, one_sided_reest,allow_new_channels) == FAILURE)
      {
        return FAILURE;
      }
      break;

    /* Two logical channels, currently not supported by RRC-RLC I/F */
    case T_rrc_UL_LogicalChannelMappings_r8_twoLogicalChannels:
      /* TRUE indicates that first logical channel is for Data PDUs and second  
         logical channel is for control PDUs */
      /* FALSE indicates that Data and Control PDUs can be sent on either of them */
      /* Invalid number of logical channels */
      default:
      WRRC_MSG1_ERROR("Support only 1 Log CH per AM/Invalid number of UL logical channels", ie_ptr->t );
        return FAILURE;
  } /* end switch */

  return SUCCESS;

}

/*============================================================================
FUNCTION: rrcllc_process_srb_info_setup_list_r8()

DESCRIPTION:
  This function process IE rrc_SRB_InformationSetupList_r8 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_srb_info_setup_list_r8
(
  OSS_UINT32 srb_info_setup_list_present,
  rrc_SRB_InformationSetupList_r8 * srb_setup_r8_ptr
)
{
  rrc_SRB_InformationSetup_r8 * srb5_setup_ptr = NULL;
  rrc_RB_Identity                          srb_id = RRCLLC_INVALID_RB_ID;

  if (srb_info_setup_list_present)
  {
    uint8                           srb_cnt =0;
    rrc_RB_Identity                          rb_id = 0;
   
    
    while((srb_setup_r8_ptr->n > srb_cnt ) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      /* Check to see if the RB ID is there ... */
      if(srb_setup_r8_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        rb_id = srb_setup_r8_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        rb_id++;
      }
      
      /* Save off the RB Mapping info for each SRB */
      if(get_srb_info_to_setup_r8(
        &srb_setup_r8_ptr->elem[srb_cnt], 
        (srb_setup_r8_ptr), 
        rb_id)
        == FAILURE)
      {
        return FAILURE;
      }
      if(rb_id > DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        /* Pick that pointer that has RB Id > 4 & 
         assume it to be SRB#5 pointer for the time being */

        WRRC_MSG0_HIGH("Probable SRB#5 IE in the message .. processing");
        srb5_setup_ptr = &srb_setup_r8_ptr->elem[srb_cnt];

        srb_id = rb_id;
      }
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
      if ((srb_setup_r8_ptr->elem[srb_cnt].rb_Identity) && (srb_setup_r8_ptr->elem[srb_cnt].rb_Identity <= NUM_SRB_DC_CELLFACH))
      {
         default_cfg_recfg_list_srb_present[srb_setup_r8_ptr->elem[srb_cnt].rb_Identity-1] = TRUE;
      }
#endif   
      srb_cnt++;
    } /* end SRB setup */

  }
  /* We are just validating one more SRB, incase 
     SRB#5 is given by n/w */
  if(srb5_setup_ptr != NULL)
  {
  
    if(rrcllc_update_oc_with_srb5_rel8_ie(srb5_setup_ptr, srb_id) == FAILURE)
    {
      WRRC_MSG0_ERROR("Rejecting SRB#5 configuration given by the network");
      return FAILURE;
    }
  }

  return(SUCCESS);
}
/*==========================================================
FUNCTION  : get_rb_mapping_info_r8()

DESCRIPTION : Get Radio Bearer Mapping Info IE

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_mapping_info_r8
(
rrc_RB_MappingInfo_r8      * ie_ptr,
rrc_RB_Identity                 rb_id,
uecomdef_logch_mode_e_type logch_mode
,boolean one_sided_reest_present
,boolean one_sided_reest
,boolean allow_new_channels
)
{
  /*
  * Count to make sure that don't exceed the maximum number 
  * of RB Mux options
  */
  uint8 rb_mux_cnt = 0;

  /* Process the RB Mux option IE in the linked list */
  while ((ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5))
  {

    /* Check for DL Channel Mapping List */
    if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],
		rrc_RB_MappingOption_r8,dl_LogicalChannelMappingList))
    {
      MSG_MED("Get RB %d Mapping Info for DL", rb_id,0,0);
      if (get_dl_logical_channel_info_r7(&ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList,
                                         rb_id, logch_mode
                                    ) == FAILURE)
      {
        return FAILURE;
      }
      /* Here indicate that MAC Downlink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.downlink_mac = TRUE;
    }  /* end dl Log Ch mapping list */

    if (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],
		rrc_RB_MappingOption_r8,ul_LogicalChannelMappings))
    {
      MSG_MED("Get RB %d Mapping Info for UL", rb_id,0,0);
      if (get_ul_logical_channel_info_r8(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,
                                      rb_id, logch_mode
                                    , one_sided_reest_present,one_sided_reest,allow_new_channels) == FAILURE)
      {
        return FAILURE;
      }

      /* Here indicate that MAC Uplink needs to be reconfigured */
      ordered_config_ptr->reconfig_needed.uplink_mac = TRUE;
    }  /* end ul Log Ch mapping list */

    /* Pointer to the next item in the IE list */
    rb_mux_cnt++;

  }  /* end loop of RB Mapping info list */
#ifdef FEATURE_WCDMA_HS_FACH
  rrcllc_backup_mapping_info_into_hanging(ie_ptr,REL_8,rb_id,logch_mode);
#endif

  return SUCCESS;
}

/*==========================================================
FUNCTION  : get_srb_info_to_setup_r8()

DESCRIPTION : Get Signalling Radio Bearer Information to setup.

REUSABILITY: R6

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_srb_info_to_setup_r8
(
  rrc_SRB_InformationSetup_r8     *ie_ptr,
  rrc_SRB_InformationSetupList_r8 * srb_setup_ptr,
  rrc_RB_Identity                   rb_id
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  
  /* Local count of # of SRB's to setup */
  uint8                           srb_cnt = 0;
  
  /* temp rb id which we shall compute if rb_id is not part of
  * srb_setup_ptr when we loop thru the list to get a match
  */
  rrc_RB_Identity                      temp_rb_id = 0;
  
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_SRB_InformationSetup_r8     *temp_ie_ptr = ie_ptr;
  
  /* Entity indexes in case we need to look up RLC info from
  * our internal database instead of processing an IE.
  */
  uint8 entity_idx, new_entity_idx;
  boolean one_sided_reest_present=FALSE;
  boolean one_sided_reest=FALSE;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the srb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,
    rrc_RLC_InfoChoice_r7_same_as_RB))
  {
    while ((srb_setup_ptr->n > srb_cnt) && (srb_cnt < UE_MAX_SRB_SETUP))
    {
      
      /* Check to see if the RB ID is there ... */
      if (srb_setup_ptr->elem[srb_cnt].m.rb_IdentityPresent)
      {
        temp_rb_id = srb_setup_ptr->elem[srb_cnt].rb_Identity;
      }
      
      /*
      * ... if not, then start with the first SRB and get the next SRB ID
      * each time one is not included in the IE.
      */
      else
      {
        /* Get the next SRB, note this increments to 1 the first time */
        temp_rb_id++;
      }
      
      /*check if the rb_id matches*/
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == temp_rb_id)
      {
        temp_ie_ptr = &(srb_setup_ptr->elem[srb_cnt]);
        break;
      }
     
      srb_cnt++;
    } /* end while... */
    
    if ((srb_setup_ptr->n == srb_cnt) || (srb_cnt >= UE_MAX_SRB_SETUP))
    {
      WRRC_MSG1_HIGH("same_as_RB %d not in msg, lookup req'd",
        ie_ptr->rlc_InfoChoice.u.same_as_RB);
      temp_ie_ptr = NULL;
      temp_rb_id = ie_ptr->rlc_InfoChoice.u.same_as_RB;
      
      if ((entity_idx = get_am_index(temp_rb_id)) != UE_MAX_AM_ENTITY)
      {
      /* This RB is in AM mode. And its reference config is found in OC.
        */
        if ((new_entity_idx = rrcllc_get_1st_hole_for_rlc_am()) == UE_MAX_AM_ENTITY)
        {
          WRRC_MSG1_ERROR("Can't find space for new rb %d", rb_id);
          return FAILURE;
        }
        ordered_config_ptr->am_config[new_entity_idx].rlc_ul_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->common_config.rb_id = rb_id;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan++;
        ordered_config_ptr->am_config[new_entity_idx].rlc_dl_am_config.rlc_info =
          ordered_config_ptr->am_config[entity_idx].rlc_dl_am_config.rlc_info;
        ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan++;
        ordered_config_ptr->rlc_am_parms.nentity++;
        if((ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan >
                                                       UE_MAX_UL_AM_DATA_CHANNEL) || 
           (ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->dl_nchan >
                                                       UE_MAX_DL_AM_DATA_CHANNEL) ||
           (ordered_config_ptr->rlc_am_parms.nentity > UE_MAX_AM_ENTITY))
        {
          ERR("Too many UL/DL AM channels or entities [DL,UL,entities] [%d,%d,%d]",
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.am_info[new_entity_idx]->ul_nchan, 
            ordered_config_ptr->rlc_am_parms.nentity);
          return FAILURE;
        }
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      
      if ((entity_idx = get_ul_um_index(rrclcm_get_ul_rlc_lc_id(temp_rb_id)))
        != UE_MAX_UL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_ul_um_info(rb_id, TRUE) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      if ((entity_idx = get_dl_um_index(temp_rb_id, ordered_config_ptr))
        != UE_MAX_DL_UM_CHANNEL)
      {
        /* This RB is in UM mode */
        if(get_rlc_dl_um_info(rb_id, TRUE, ordered_config_ptr) == FAILURE)
        {
          return FAILURE;
        }
		
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      
      /* SRBs cannot be TM.  If we haven't found it by now, we have
      * a problem. */
      if (logch_mode == UE_MODE_MAX_NUM)
      {
        WRRC_MSG1_ERROR("same_as_RB %d not found in msg or OC",
          ie_ptr->rlc_InfoChoice.u.same_as_RB);
        return FAILURE;
      }
    }
  } /* end RLC choice == Same as RB */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r7_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", rb_id);
      return FAILURE;
    }
  } 


  /* Now we have either the RLC Info explicitly meant for this RB OR
   * the RLC Info that is the same as another RB.  If tmp_ie_ptr is NULL,
   * then we've already copied the RLC Info from our internal database.
   */
  if (temp_ie_ptr != NULL)
  {
    /* Get the RLC mode */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
         ((logch_mode = get_rlc_mode_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
       )
    {
      return FAILURE;
    }
      
    /* Get the RLC info */
    if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
        (get_rlc_info_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, rb_id, TRUE) == FAILURE))
    {
      return FAILURE;
    }

    if(temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info)
    {
      one_sided_reest_present = TRUE;
      one_sided_reest =temp_ie_ptr->rlc_InfoChoice.u.rlc_Info->rlc_OneSidedReEst;
    }
  }
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r8(&ie_ptr->rb_MappingInfo, rb_id, logch_mode
       , one_sided_reest_present,one_sided_reest,TRUE) == FAILURE)
  {
    return FAILURE;
  }
  
  if(FAILURE == rrcllc_validate_srb_id(rb_id))
  {
    return FAILURE;
  }

  /* everything is configured */
  return SUCCESS;

}



 /* end get_ul_logical_channel_info() */

 /* end get_ul_single_logical_channel_info_r6() */

 /* end get_ul_dch_mapping_info() */


 /* end get_rlc_size_list_r6() */

/*============================================================================
FUNCTION: rrcllc_process_rab_info_setup_list_r8()

DESCRIPTION:
  This function process IE rrc_RB_InformationAffectedList_r8 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rab_info_setup_list_r8
(
  OSS_UINT32 rab_info_setup_list_present,
  rrc_RAB_InformationSetupList_r8 * rab_setup_ptr
)
{
  if(rab_info_setup_list_present)
  {
    /* Local count of # of RAB's & RB's to setup */
    uint32                             rab_cnt = 0;
    uint32                             rb_cnt  = 0;    
   
    rrc_RB_InformationSetupList_r8     * rb_setup_ptr;
   
    
    while((rab_setup_ptr->n > rab_cnt) && (rab_cnt < MAX_RAB_TO_SETUP))
    {
      rb_setup_ptr  = &rab_setup_ptr->elem[rab_cnt].rb_InformationSetupList;
      rb_cnt =0;
 
      
      while((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
      {
        if(get_rb_info_to_setup_r8(
           &rb_setup_ptr->elem[rb_cnt],
#ifdef FEATURE_UMTS_PDCP
           rab_setup_ptr->elem[rab_cnt].rab_Info.cn_DomainIdentity,
#endif /* FEATURE_UMTS_PDCP */           
           rb_setup_ptr) == FAILURE)
        {
          return FAILURE;
        }
        
        rb_cnt++;
      } /* end RB per RAB setup */
      
      rab_cnt++;
    } /* end rab setup */
  } /* end RAB setup */

  return(SUCCESS);
}

/*==========================================================
FUNCTION  : get_rb_info_to_setup_r8()

DESCRIPTION : Get Radio Bearer Information to setup.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_setup_r8
(
  rrc_RB_InformationSetup_r8       *ie_ptr,
#ifdef FEATURE_UMTS_PDCP
  rrc_CN_DomainIdentity cn_Domain,
#endif /* FEATURE_UMTS_PDCP*/
  rrc_RB_InformationSetupList_r8   * rb_setup_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type    logch_mode = UE_MODE_MAX_NUM;
  
  /*initialize a counter to check rb count*/
  uint8                         rb_cnt  = 0;
  
  
  /*place holder for rb_id we get originally*/
  rrc_RB_Identity                    local_rb_id = ie_ptr->rb_Identity;
  
  /* Local IE pointer - if RLC Info for this RB is the same as 
  * RLC Info for another RB, IE Pointer is made to point to 
  * SRB Info Setup for the same_as RB. 
  * For RB Mapping info, IE Pointer should always point to
  * SRB Info Setup for the RB that came as a parameter.
  */
  rrc_RB_InformationSetup_r8     *temp_ie_ptr = ie_ptr;

#ifdef FEATURE_UMTS_PDCP
  uint8 idx = 0;
#endif /* FEATURE_UMTS_PDCP */  
  boolean one_sided_reest_present=FALSE;
  boolean one_sided_reest=FALSE;
  
  /*check if rlc_InfoChoice is same_as_RB_Chosen and then get the
  *rlc_Info for the rb_id specified.  Walk thru the rb_setup_head_ptr
  *list until we find a rb_id that matches
  */
  if (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->rlc_InfoChoice,rrc_RLC_InfoChoice_r7_same_as_RB))
  {
    while ((rb_setup_ptr->n > rb_cnt) && (rb_cnt < MAX_RB_PER_RAB))
    {
      if (ie_ptr->rlc_InfoChoice.u.same_as_RB == 
        rb_setup_ptr->elem[rb_cnt].rb_Identity)
      {
        temp_ie_ptr = &(rb_setup_ptr->elem[rb_cnt]);
        break;
      }
      rb_cnt++;
    } /* end RB per RAB setup */
    
    if ((rb_setup_ptr->n == rb_cnt) || (rb_cnt >= MAX_RB_PER_RAB))
    {
       WRRC_MSG0_ERROR("Unable to match rb_id to same_as_RB"); 
      return FAILURE;
    }
  } /* if RLC info choice is "Same as RB" type */
  else
  {
    /*Reject the configuration If (UL RLC info present && Mode is AM && DL RLC info not present)
    Or (DL RLC info present && Mode is AM && UL RLC info not present)
    Or (Both DL and UL RLC info not present) */
    if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->ul_RLC_Mode.t ==T_rrc_UL_RLC_Mode_ul_AM_RLC_Mode) &&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
        (ie_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t ==T_rrc_DL_RLC_Mode_r7_dl_AM_RLC_Mode) &&
      !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode))||
      (!RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode)&&
         !RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)))
    {
      WRRC_MSG1_ERROR("DL or UL or Both RLC mode info not provided for rb id %d", local_rb_id);
      return FAILURE;
    }
  } 

  
    /* Now we have either the RLC Info explicitly meant for this RB OR
    * the RLC Info that is the same as another RB.Get the RLC mode */
  
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
       ((logch_mode = get_rlc_mode_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info)) == UE_MODE_MAX_NUM)
     )
  {
    return FAILURE;
  }
  
  /* Save off the RLC Info */
  if ((temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info) && 
      (get_rlc_info_r7(temp_ie_ptr->rlc_InfoChoice.u.rlc_Info, 
        local_rb_id, TRUE) == FAILURE))
  {
    return FAILURE;
  }
  
  if(temp_ie_ptr->rlc_InfoChoice.t == T_rrc_RLC_InfoChoice_r7_rlc_Info)
  {
    one_sided_reest_present = TRUE;
    one_sided_reest =temp_ie_ptr->rlc_InfoChoice.u.rlc_Info->rlc_OneSidedReEst;
  }  
  
  /* Use the IE Pointer that came as a parameter to get the RB Mapping info */
  if (get_rb_mapping_info_r8(&ie_ptr->rb_MappingInfo, local_rb_id, logch_mode
     , one_sided_reest_present,one_sided_reest,TRUE) == FAILURE)
  {
    return FAILURE;
  }

#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r8,pdcp_Info))
  {
    if (rrc_pdcp_enabled)  
    { 
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,&ie_ptr->pdcp_Info, 
        local_rb_id, CPDCP_SETUP) == FAILURE)
      {
        return FAILURE;
      }
      return SUCCESS;
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r8,pdcp_Info)) &&
            ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
            (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
            ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
            (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,
            rrc_LosslessSRNS_RelocSupport_supported)))))
    {
      WRRC_MSG0_ERROR("PDCP NV not enabled.  PDCP info not handled");
      return FAILURE;
    }
  }

  /* if fall-thru happens, PDCP layer has to be setup in transparent [pass thru] mode */
  if (cn_Domain == rrc_CN_DomainIdentity_ps_domain) 
  {
    WRRC_MSG1_MED("Setting up PDCP in transparent mode rb_id %d",local_rb_id);
    /*Get the next free index in pdcp_config_req */
    while (idx < MAX_RAB_TO_SETUP) 
    {
      if (ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use == FALSE) 
      {
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].rb_id = local_rb_id;
        ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].pdcp_cfg_req.action = SETUP_PDCP;
        ordered_config_ptr->pdcp_parms.num_rbs++;
        break;
      }
      else
      {
        idx++;
      }
    }
    /* CPDCP action will be setup, but pdcp info will be null */
    //ordered_config_ptr->pdcp_parms.pdcp_config_req[idx].in_use = TRUE;
  }
#else
  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationSetup_r8,pdcp_Info)) &&
            ((ie_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
            (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
            ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
            (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.losslessSRNS_RelocSupport,
            rrc_LosslessSRNS_RelocSupport_supported)))))
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }
#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

}

/*============================================================================
FUNCTION: rrcllc_process_rb_info_reconfig_list_r8()

DESCRIPTION:
  This function process IE rrc_RB_InformationReconfigList_r6 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_reconfig_list_r8
(
  uint32 rb_info_reconfig_list_present,
  rrc_RB_InformationReconfigList_r8 * rb_reconfig_ptr
)  
{

  if (rb_info_reconfig_list_present)
  {
   
    uint32 idx = 0;
    
    idx = 0;
    while((rb_reconfig_ptr->n > idx) && (idx < MAX_RB))
    {
      if(get_rb_info_to_reconfig_r8(&rb_reconfig_ptr->elem[idx]) == FAILURE)
      {
        return FAILURE;
      }
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
      if ((rb_reconfig_ptr->elem[idx].rb_Identity) && (rb_reconfig_ptr->elem[idx].rb_Identity <= NUM_SRB_DC_CELLFACH))
      	{default_cfg_recfg_list_srb_present[rb_reconfig_ptr->elem[idx].rb_Identity-1] = TRUE;}
#endif
      idx++;
    } /* end RB reconfig */
  }

  return(SUCCESS);
}

/*==========================================================
FUNCTION  : get_rb_info_to_reconfig_r8()

DESCRIPTION : Get Radio Bearer Information to reconfig.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_to_reconfig_r8
(
rrc_RB_InformationReconfig_r8 *ie_ptr
)
{
  /* local variable for log channel mode */
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  boolean one_sided_reest_present=FALSE;
  boolean one_sided_reest=FALSE;
#ifdef FEATURE_MAC_I
  uint8 entity_idx=UE_MAX_AM_ENTITY;
#endif
  /* Check to see if the RLC info is present */
  if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,rlc_Info))
     && ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r7,ul_RLC_Mode)) ||
         (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->rlc_Info,rrc_RLC_Info_r7,dl_RLC_Mode))))
  {
    /* Get the RLC mode */
    if ((logch_mode = get_rlc_mode_r7(&ie_ptr->rlc_Info)) == UE_MODE_MAX_NUM)
    {
      return FAILURE;
    }
    
    /* Save off the RLC Info */
    if (get_rlc_info_r7(&ie_ptr->rlc_Info, ie_ptr->rb_Identity, FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end RLC Info Check */
#ifdef FEATURE_MAC_I
/*Copy rlc one side re-establishment flag to OC*/
if ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,rlc_Info)) && (ie_ptr->rb_Identity >= DCCH_AM_RADIO_BEARER_ID))
{
  if (ie_ptr->rlc_Info.rlc_OneSidedReEst == TRUE)
  {
  
    entity_idx = get_am_index_wo_f3(ie_ptr->rb_Identity);
    if (entity_idx != UE_MAX_AM_ENTITY)
    {
      ordered_config_ptr->rlc_one_sided_reest[entity_idx] = TRUE;
    }
  }
}
#endif
  /* Save off the RB Mapping info */
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,rb_MappingInfo))
  {
    /* Remove any existing mapping for the RB */
    unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);
    
    if(ie_ptr->m.rlc_InfoPresent)
    {
      one_sided_reest_present = TRUE;
      one_sided_reest =ie_ptr->rlc_Info.rlc_OneSidedReEst;
    }
    
    if (get_rb_mapping_info_r8(&ie_ptr->rb_MappingInfo, (rb_id_type)ie_ptr->rb_Identity, logch_mode
       , one_sided_reest_present,one_sided_reest,FALSE) == FAILURE)
    {
      return FAILURE;
    }
  } /* end of RB Mapping info check */
  
#ifdef FEATURE_UMTS_PDCP
  if (RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,pdcp_Info))
  {
    if (rrc_pdcp_enabled)
    {
      WRRC_MSG0_HIGH("PDPC info present in rb setup");
      if (get_pdcp_info_later_than_r3(&ordered_config_ptr->pdcp_parms,
        &ie_ptr->pdcp_Info.pdcp_Info, 
        ie_ptr->rb_Identity, 
        CPDCP_RECONFIG) == FAILURE)
      {
        return FAILURE;
      }
    }
    else if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
    {
      WRRC_MSG0_ERROR("PDCP Info not supported");
      return FAILURE;
    }
  }
#else

  /* Ignore the PDCP info for now */
  /********************************/
  if((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,pdcp_SN_Info)) ||
            ((RRC_MSG_COMMON_BITMASK_IE_PTR(ie_ptr,rrc_RB_InformationReconfig_r8,pdcp_Info)) &&
             ((ie_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) ||
              (RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,headerCompressionInfoList)) ||
              ((RRC_MSG_COMMON_BITMASK_IE(ie_ptr->pdcp_Info.pdcp_Info,
                rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
               (RRC_CHECK_COMMON_MSG_TYPE(ie_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
                rrc_LosslessSRNS_RelocSupport_supported))))))          
  {
    WRRC_MSG0_ERROR("PDCP Info not supported");
    return FAILURE;
  }

#endif /* FEATURE_UMTS_PDCP */

  /* All is well with this IE */
  return SUCCESS;

} /* end get_rb_info_to_reconfig_r6() */

/*============================================================================
FUNCTION: rrcllc_process_rb_info_affected_list_r8()

DESCRIPTION:
  This function process IE rrc_RB_InformationAffectedList_r8 and store the 
  received information in OC
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_affected_list_r8
(
  OSS_UINT32 rb_info_affected_list_present,
  rrc_RB_InformationAffectedList_r8 * rb_affected_ptr
)  
{
  /* Get the affected RB list if present */
  if(rb_info_affected_list_present)
  {
   
    uint32 idx = 0;
  
    while((rb_affected_ptr->n > idx) && (idx < MAX_RB))
    {
      if(get_rb_info_affected_r8(&rb_affected_ptr->elem[idx]) == FAILURE)
      {
        return (FAILURE);
      }
#ifdef FEATURE_WCDMA_DEFAULT_CFG_CELLFACH
      if ((rb_affected_ptr->elem[idx].rb_Identity) && (rb_affected_ptr->elem[idx].rb_Identity <= NUM_SRB_DC_CELLFACH))
      	{default_cfg_recfg_list_srb_present[rb_affected_ptr->elem[idx].rb_Identity-1] = TRUE;}
#endif	
      idx++;
    }
  } /* end Affected RB */

  return(SUCCESS);
}

/*==========================================================
FUNCTION  : get_rb_info_affected_r8()

DESCRIPTION : Get Radio Bearer Information to be affected.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_rb_info_affected_r8
(
rrc_RB_InformationAffected_r8*ie_ptr
)
{
  /* Remove any existing mapping for the RB */
  unmap_rb(ie_ptr->rb_Identity, OC_AND_TOC);


  /* Get the RB Mapping info */
  return(get_rb_mapping_info_r8(&ie_ptr->rb_MappingInfo, 
                                ie_ptr->rb_Identity, 
                                UE_MODE_MAX_NUM
                                ,FALSE,FALSE,FALSE));

}  /* end get_rb_info_affected_r6() */

/*==========================================================
FUNCTION  : rrcllc_backup_ul_dch_from_ota_r8()

DESCRIPTION : This function backs up the Rb-Mapping info 
              received in the REL8 OTA message for UL DCH.
              This function is called when the OTA message 
              provides the RB-Mapping info, 
              but the Trch info mapped to that RB is not provided.

DEPENDENCIES: None.

RETURN VALUE: boolean

SIDE EFFECTS: None
============================================================*/
 uecomdef_status_e_type rrcllc_backup_ul_dch_from_ota_r8
(
  rrc_UL_LogicalChannelMapping_r8*ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  uint8 idx;
  /* Store the RB-Mapping info */
  if( rrcllc_check_rb_id_exist_rb_mapping(rb_id, &idx) == FALSE )
  {
    if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
    {
      WRRC_MSG0_ERROR("No space for RB Mapping backup for UL DCH ");
      return FAILURE;
    }
  }
  rrc_ordered_hanging_rb_mapping[idx].rb_id = rb_id;
  rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
    
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.valid = TRUE;
  
   
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id = 
                (tr_ch_id_type) ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.u.dch;
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.mac_log_priority  = 
        (uint8) ie_ptr->mac_LogicalChannelPriority;
  /* If Logical Channel ID is present, save MAC Logical Channel Identity */
  if (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
  {
     rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =
       (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
  }
  else
  {
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.log_channel_id =0;
  }
  MSG_HIGH(" Adding into Standalone Database for R6 UL DCH for RB-ID  %d for Trch id %d  at idx %d",
                             rrc_ordered_hanging_rb_mapping[idx].rb_id,
                             rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.tr_id,
                             idx);
  /* Save the RLC Size List information */
#if defined(FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH)
  if(get_rlc_size_list_r8(ie_ptr, &rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_dch_info.size_rest_info, rb_id) == FAILURE)
  {
    return FAILURE;
  }
#endif
      
  return (SUCCESS);
}

/*==========================================================
FUNCTION  : get_edch_mapping_info_r8()

DESCRIPTION : Processes EDCH RB Mapping info contents and saves 
              them in MAC Ordered Configuration Data  

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_edch_mapping_info_r8
(
rrc_UL_LogicalChannelMapping_r8 *ie_ptr,
rrc_RB_Identity                    rb_id,
uecomdef_logch_mode_e_type    logch_mode,
boolean one_sided_reest_present,
boolean one_sided_reest,
boolean allow_new_channels
#ifdef FEATURE_MAC_I
,ordered_config_type *backup_ordered_config_ptr
#endif /* FEATURE_MAC_I */
)
{
  /* EDCH MAC-D FLOW Id */
  uint32         dflow_id;
  /* Get the logical channel index */
  uint8         logch_idx;
  rlc_lc_id_type   rlc_lc_id;
  uint8 mac_dflow_index;

  rrc_RLC_PDU_SizeList *rlc_pdu_sizelist;
  uint8 ddi = 0;
  uint32 cnt = 0;
  uint16 rlc_size = 0;
  uint32 index = 0;
  uint8 entity_idx;
  ordered_config_type *oc_config_ptr = ordered_config_ptr;
  uint16 largestRLC_PDU_Size = 0xFFFF;
  uint16 minRLC_PDU_Size = 0xFFFF;
  boolean special_value_he_config = FALSE;
  if(backup_ordered_config_ptr != NULL)
  {
    oc_config_ptr = backup_ordered_config_ptr;
  }
  dflow_id = ie_ptr->ul_TrCH_Type.u.e_dch->e_DCH_MAC_d_FlowIdentity;
  
  mac_dflow_index = rrcllc_find_e_mac_d_flow_idx((uint8)dflow_id);
  /* If DFLOW Index does not exist, then create a new one */
  if (mac_dflow_index == UE_MAX_MAC_D_FLOW)
  {
    
    WRRC_MSG1_MED("EDCH Mac-d FLOW Index for DFLOW %d, not found", dflow_id);
    return(rrcllc_backup_eul_from_ota_r8(ie_ptr,rb_id,logch_mode));
  }
  else
  {
    for (index=0;index < ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch; index++)
    {
      if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[index].rb_id == rb_id)
      {
        WRRC_MSG3_MED("RB_ID %d, logical channel identity %d, mac_id: %d already present",
          rb_id, ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity,
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_mac_d_flow_id);      
        return(SUCCESS);
      }
    }
  }

  /* Get the Logical Channel Index */
  logch_idx = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch;
  rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);

  if (rlc_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
  {

    /*This is the case where RB-mapping info is present in rb-reconfig-info list or affected list*/
    if (rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
      return(FAILURE);
    }
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_id = rlc_lc_id;
  }
  /* Save the and RLC info DDI info*/
  if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == 
  	T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_fixedSize)
  {
    rlc_pdu_sizelist = &ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.fixedSize->rlc_PDU_SizeList;
    ddi = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.fixedSize->ddi;
#ifdef FEATURE_MAC_I
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_pdu_type = RLC_FIXED_PDU_SIZE;
#endif /* FEATURE_MAC_I */
  /* If more than 1 UL RLC PDU size is configured for this RB, the different sizes will use subsequent DDI values starting
    from this DDI value.*/

  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].num_rlc_size_ddi_info = 0;

  while ((rlc_pdu_sizelist->n > cnt) && (cnt < RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN)) 
  {
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].
      rlc_size_ddi_info[cnt].ddi = ddi;

    switch (rlc_pdu_sizelist->elem[cnt].t)
    {
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType1 * 8) + 16;
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part1 * 32) + 272;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType2->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part2 * 8);
        }
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part1 * 64) + 1040;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType3->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part2 * 8);
        }
        break;
      default:
        WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
            rlc_pdu_sizelist->elem[cnt].t);
        break;
    }
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].
      rlc_size_ddi_info[cnt].rlc_pdu_size_in_bits = rlc_size;

    /* Increment the number of valid fields in rlc_size_ddi_info[] */
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].num_rlc_size_ddi_info++;
    cnt++;
    /* Increment the DDI */
    ddi++;
 
  }

  }
  else if (ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_flexibleSize)
  {
#ifdef FEATURE_MAC_I
      /*Flexible size shouldnt be configured for SRBs*/
    if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      WRRC_MSG1_ERROR("MAC_I:RB ID %d configured with flexible UL PDU size",rb_id);
      return FAILURE;
    }
			
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_pdu_type = RLC_FLEXIBLE_PDU_SIZE;
		
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_flex_min_pdu_size = 
      (ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->minRLC_PDU_Size*8)+16;
	
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_flex_max_pdu_size= 
      (ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->largestRLC_PDU_Size*8)+16;

    if(ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_flex_max_pdu_size < 
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_flex_min_pdu_size)
    {
      WRRC_MSG2_ERROR("MAC_I:MAX size %d is less than Min size %d ",
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_flex_max_pdu_size,
        ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_flex_min_pdu_size);
      return FAILURE;
    }
    
    if(logch_mode == UE_MODE_ACKNOWLEDGED_DATA)
    {
      if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->m.lengthIndicatorSizePresent)
      {
        if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->lengthIndicatorSize == rrc_RLC_PDU_SizeConstraint_lengthIndicatorSize_size7)
        {
        /*  NOTE1:  In  uplink, if the UTRAN has configured "Flexible size" RLC PDUs and 7-bit "Length indicator size" field 
            and	"Largest UL RLC PDU size" > 126 octets, but the UTRAN has not configured "Use special value of HE field", then the UE behaviour is unspecified. 

            NOTE2:  In uplink, if the UTRAN has configured "Flexible size" RLC PDUs and 7-bit "Length indicator size" field,
            but the UTRAN has also configured "Minimum UL RLC PDU" > 126 octets, then the UE behaviour is unspecified.

            largestRLC_PDU_Size and minRLC_PDU_Size are in bytes, +2 is for + 16bits mentioned in the below comment from spec
            So that the total RLC PDU size will not exceed 126 octets

            the actual values are (IE value * 8 ) + 16 
            minRLC-PDU-Size INTEGER (0 . . 1503 ) , 
            largestRLC-PDU-Size INTEGER (0 . . 1503 )
        */
          entity_idx = get_am_index_in_config_ptr(rb_id,oc_config_ptr);
          largestRLC_PDU_Size = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->largestRLC_PDU_Size + 2;
          minRLC_PDU_Size = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->minRLC_PDU_Size + 2;

          if(entity_idx < UE_MAX_AM_ENTITY)
          {
            special_value_he_config = oc_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config;

            if( ( (largestRLC_PDU_Size > 126) && (special_value_he_config == FALSE) ) || 
                (minRLC_PDU_Size > 126) )
            {
              ERR("LI is 7, special_value_he_config %d largestRLC_PDU_Size: %d octets, minRLC_PDU_Size: %d octets ", 
               special_value_he_config,largestRLC_PDU_Size,minRLC_PDU_Size);
              return FAILURE;
            }
          }
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_li_size= LI_7_BITS;
        }
        else if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->lengthIndicatorSize == rrc_RLC_PDU_SizeConstraint_lengthIndicatorSize_size15)
        {
          ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].ul_li_size= LI_15_BITS;	
        }			
      }
      else
      {
         WRRC_MSG1_ERROR("MAC_I:LI not given for a AM RLC RB ID %d ",rb_id);
         return FAILURE;
      }
     
    }
#else
    WRRC_MSG0_ERROR("REL8: rrc_rlc_PDU_Size_flexibleSize not supported");
    return FAILURE;
#endif	
  }

  /* Ignore the logical channel identity signalled by NW.  Not needed by MAC*/ 
  
  /* Save Radio Bearer ID */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rb_id = rb_id;

  /* Get the priority between a user's different RBs (or logical channels). */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].priority =
    ie_ptr->mac_LogicalChannelPriority;
  
  /* Determine the type of Logical Channel mapped to DCH TrCH */
  if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
  {
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
  } 
  else
  {
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
  }

  /* If set include scheduling info is set to false then dont consider this mac_id while sending schedulingInformation */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].include_in_scheduling_info = 
    ie_ptr->ul_TrCH_Type.u.e_dch->includeInSchedulingInfo;

  /* Set the logical channel Id recieved from the NW */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].lc_id = 
    ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity;
  
  /* Save the logical channel mode, if a setup */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_mode = logch_mode;
  
  /* Increment number of logical channels */
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch++;  

  if (ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_flexibleSize)
  {
#ifdef FEATURE_MAC_I
#else
    WRRC_MSG0_ERROR("REL8: rrc_rlc_PDU_Size_flexibleSize not supported");
    return FAILURE;
#endif
  }

      
  WRRC_MSG3_MED("RRCEUL:EDCH E-Mac-d FLOW %d, No of dedicated Ch %d, rlc mode: %d(1:UM, >1:AM)",
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_mac_d_flow_id,
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].num_logch,
  ordered_config_ptr->mac_e_config.e_mac_d_flow_info[mac_dflow_index].e_ul_logch_info[logch_idx].rlc_mode);
  rrcllc_free_hanging_rb_mapping_info_index(rb_id,E_DCH_RB_MAPPING);

  return SUCCESS;

}

/*==========================================================
FUNCTION  : rrcllc_backup_eul_from_ota_r8()

DESCRIPTION : This function backs up the Rb-Mapping info 
              received in the OTA message for EULH. 
              This function is called when the OTA message 
              provides the RB-Mapping info, but the Trch 
              info mapped to that RB is not provided.

DEPENDENCIES: None.

RETURN VALUE: boolean

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_backup_eul_from_ota_r8
(
  rrc_UL_LogicalChannelMapping_r8*ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  uint8 idx;
  rrc_RLC_PDU_SizeList *rlc_pdu_sizelist;
  uint8 ddi = 0;
  uint32 cnt = 0;
  uint16 rlc_size = 0;
  rlc_lc_id_type rlc_lc_id;
  uint8 entity_idx;
  uint16 largestRLC_PDU_Size = 0xFFFF;
  uint16 minRLC_PDU_Size = 0xFFFF;
  boolean special_value_he_config = FALSE;
  /* Store the RB-Mapping info */
  if( rrcllc_check_rb_id_exist_rb_mapping(rb_id, &idx) == FALSE )
  {
    if(rrcllc_find_free_hanging_rb_mapping_info(&idx) == FAILURE)
    {
      WRRC_MSG0_ERROR("No space for RB Mapping backup for UL DCH ");
      return FAILURE;
    }
  }
  rrc_ordered_hanging_rb_mapping[idx].rb_id = rb_id;
  rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
    
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.valid = TRUE;
  
   
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_e_flow_id= 
                 (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->e_DCH_MAC_d_FlowIdentity;
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_log_priority  = 
        (uint8) ie_ptr->mac_LogicalChannelPriority;

  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.log_channel_id =
       (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity;
  /* Save the and RLC info DDI info*/
  

  if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_flexibleSize)	
  {
#ifdef FEATURE_MAC_I
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_pdu_type =  RLC_FLEXIBLE_PDU_SIZE ;
		
      /*Flexible size shouldnt be configured for SRBs*/
    if (rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      WRRC_MSG1_ERROR("MAC_I:SRB %d configured with flexible UL PDU size",rb_id);
      return FAILURE;
    }
					
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_min_pdu_size = 
			(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->minRLC_PDU_Size*8)+16;
	
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_max_pdu_size= 
			(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->largestRLC_PDU_Size*8)+16;

    if(rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_max_pdu_size< 
      rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_min_pdu_size)
    {
      WRRC_MSG2_ERROR("MAC_I:MAX size %d is less than Min size %d ",
        rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_max_pdu_size,
        rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_flex_min_pdu_size);
      return FAILURE;
    }

    /*Affected info given in OTA doesnt provide rlc info, due to which logical channel mode in hanging RB gets 
    overwritten. This causes issue when update hsrach params is done as config ptr is not updated with the 
    correct LI size*/
    if(rrc_ordered_hanging_rb_mapping[idx].logch_mode == UE_MODE_MAX_NUM)
    {
      rlc_lc_id = rrclcm_get_ul_rlc_lc_id(rb_id);
      if(rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
        return FAILURE;
      }
      if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
      {
        if(get_am_index_wo_f3(rb_id) != UE_MAX_AM_ENTITY)
        {
          logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
        }
        else if(get_ul_um_index(rb_id)!= UE_MAX_UL_UM_CHANNEL) 
        {
          logch_mode = UE_MODE_UNACKNOWLEDGED;
        }
        else if(get_ul_tm_index(rlc_lc_id)!= UE_MAX_UL_TM_CHANNEL)
        {
          logch_mode = UE_MODE_TRANSPARENT;
        }
        else
        {
          WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
          return FAILURE;
        }
      }
      rrc_ordered_hanging_rb_mapping[idx].logch_mode = logch_mode;
    }


    if	(logch_mode == UE_MODE_ACKNOWLEDGED_DATA)	
    {
      if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->m.lengthIndicatorSizePresent)
      {
        if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->lengthIndicatorSize == rrc_RLC_PDU_SizeConstraint_lengthIndicatorSize_size7)
        {
          /*  NOTE1:  In  uplink, if the UTRAN has configured "Flexible size" RLC PDUs and 7-bit "Length indicator size" field 
              and "Largest UL RLC PDU size" > 126 octets, but the UTRAN has not configured "Use special value of HE field", then the UE behaviour is unspecified. 
          
              NOTE2:  In uplink, if the UTRAN has configured "Flexible size" RLC PDUs and 7-bit "Length indicator size" field,
              but the UTRAN has also configured "Minimum UL RLC PDU" > 126 octets, then the UE behaviour is unspecified.
          
              largestRLC_PDU_Size and minRLC_PDU_Size are in bytes, +2 is for + 16bits mentioned in the below comment from spec
              So that the total RLC PDU size will not exceed 126 octets
          
              the actual values are (IE value * 8 ) + 16 
              minRLC-PDU-Size INTEGER (0 . . 1503 ) , 
              largestRLC-PDU-Size INTEGER (0 . . 1503 )
          */

          entity_idx = get_am_index_wo_f3(rb_id);
          largestRLC_PDU_Size = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->largestRLC_PDU_Size + 2;
          minRLC_PDU_Size = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->minRLC_PDU_Size + 2;

          if(entity_idx < UE_MAX_AM_ENTITY)
          {
            special_value_he_config = ordered_config_ptr->am_config[entity_idx].rlc_ul_am_config.special_value_he_config;

            if( ( (largestRLC_PDU_Size > 126) && (special_value_he_config == FALSE) ) || 
                (minRLC_PDU_Size > 126) )
            {
              ERR("LI is 7, special_value_he_config %d largestRLC_PDU_Size: %d octets, minRLC_PDU_Size: %d octets ", 
               special_value_he_config,largestRLC_PDU_Size,minRLC_PDU_Size);
              return FAILURE;
            }
          }
          rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_li_size= LI_7_BITS;
        }
        else if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->lengthIndicatorSize == rrc_RLC_PDU_SizeConstraint_lengthIndicatorSize_size15)
        {
          rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.ul_li_size= LI_15_BITS;	
        }	        
  }
  else
  {
        WRRC_MSG1_ERROR("MAC_I:LI not given for a AM RLC RB ID %d ",rb_id);
        return FAILURE;
      }
    }
#else
    WRRC_MSG0_ERROR("REL8: rrc_rlc_PDU_Size_flexibleSize not supported");
    return FAILURE;
#endif /* FEATURE_MAC_I */
  }  
  else if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_fixedSize)	
  { 

      rlc_pdu_sizelist = &ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.fixedSize->rlc_PDU_SizeList;
      ddi = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.fixedSize->ddi;
#ifdef FEATURE_MAC_I
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_pdu_type = RLC_FIXED_PDU_SIZE;		
#endif /* FEATURE_MAC_I */
  /* If more than 1 UL RLC PDU size is configured for this RB, the different sizes will use subsequent DDI values starting
    from this DDI value.*/
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.num_rlc_size_ddi_info = 0;

  while ((rlc_pdu_sizelist->n > cnt) && (cnt < RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN)) 
  {
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_size_ddi_info[cnt].ddi = ddi;

    switch (rlc_pdu_sizelist->elem[cnt].t)
    {
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType1 * 8) + 16;
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part1 * 32) + 272;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType2->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part2 * 8);
        }
        break;
      case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
        rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part1 * 64) + 1040;
        if (rlc_pdu_sizelist->elem[cnt].u.sizeType3->m.part2Present)
        {
          rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part2 * 8);
        }
        break;
      default:
        WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
            rlc_pdu_sizelist->elem[cnt].t);
        break;
    }
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.rlc_size_ddi_info[cnt].rlc_pdu_size_in_bits = rlc_size;

    /* Increment the number of valid fields in rlc_size_ddi_info[] */
    rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.num_rlc_size_ddi_info++;
    cnt++;
    /* Increment the DDI */
    ddi++;

  }
}
  /* If set include scheduling info is set to false then dont consider this mac_id while sending schedulingInformation */
  rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.in_sched_info = 
    ie_ptr->ul_TrCH_Type.u.e_dch->includeInSchedulingInfo;

 
  WRRC_MSG3_MED(" Adding into Standalone Database for R6 E_DCH for RB-ID %d for FLOW ID %d  at idx %d",
                             rrc_ordered_hanging_rb_mapping[idx].rb_id,
                             rrc_ordered_hanging_rb_mapping[idx].ul_mapping_info.ul_mace.mac_e_flow_id,
                             idx);
  return (SUCCESS);
}

/*====================================================================
FUNCTION: rrcllc_update_oc_with_srb5_rel8_ie()

DESCRIPTION:
  This function updates ordered_config with SRB#5 information such as 
  RB ID & transport channel ID.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE if SRB#5 is found & OC is successful cases.
  FALSE otherwise

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_with_srb5_rel8_ie
(
  rrc_SRB_InformationSetup_r8 * srb_setup_r8_ptr, 
  rrc_RB_Identity rb_id
)
{
  uint32 srb5_trch_id = 0;
  rrc_RB_Identity srb5_id = 0;
  uint8         trch_idx = RRCLLC_INVALID_TRCH_IDX;
  uecomdef_status_e_type result = FAILURE;

  if(srb_setup_r8_ptr != NULL)
  {
    WRRC_MSG0_HIGH("Probable SRB5 INFO .. processing");
    /* Probably SRB5 exists  */
    if(RRC_MSG_COMMON_BITMASK_IE_PTR(srb_setup_r8_ptr,
		 rrc_SRB_InformationSetup_r8,rb_Identity))
    {
      srb5_id = srb_setup_r8_ptr->rb_Identity;
    }
    else
    {
      srb5_id = rb_id;
    }

    /* Verify RLC mode - accept iff DL TM mode */
    if(RRC_CHECK_COMMON_MSG_TYPE(srb_setup_r8_ptr->rlc_InfoChoice,
		rrc_RLC_InfoChoice_r7_rlc_Info))
    {
      if(RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(
	  	srb_setup_r8_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,
	  	ul_RLC_Mode))
      {
        WRRC_MSG0_ERROR("SRB#5 can't have UL mode .. rejecting");
        return result;
      }
      else if(RRC_MSG_COMMON_BITMASK_IE_PTR_FOR_OBYS_ONLY(
	  	srb_setup_r8_ptr->rlc_InfoChoice.u.rlc_Info,rrc_RLC_Info_r7,
	  	dl_RLC_Mode))
      {
        if(srb_setup_r8_ptr->rlc_InfoChoice.u.rlc_Info->dl_RLC_Mode.t !=
			T_rrc_DL_RLC_Mode_r7_dl_TM_RLC_Mode)
        {
          WRRC_MSG0_ERROR("SRB#5 can't be in other than DL TM mode .. rejecting");
          return result;
        }
      }
      else
      {
        WRRC_MSG0_ERROR("SRB#5 can't be in other than DL .. rejecting");
        return result;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("No other SRB can have the characteristics of SRB#5 .. rejecting");
      return result;
    }

    /* Verify RLC mapping */
    if(RRC_MSG_COMMON_BITMASK_IE(srb_setup_r8_ptr->rb_MappingInfo.elem[0],
		rrc_RB_MappingOption_r8,ul_LogicalChannelMappings))
    {
      WRRC_MSG0_ERROR("SRB#5 can't have UL mapping .. rejecting");
      return result;
    }
    else if(RRC_MSG_COMMON_BITMASK_IE(srb_setup_r8_ptr->rb_MappingInfo.elem[0],
		rrc_RB_MappingOption_r8,dl_LogicalChannelMappingList))
    {
      if(RRC_CHECK_COMMON_MSG_TYPE(srb_setup_r8_ptr->rb_MappingInfo.elem[0].dl_LogicalChannelMappingList.
          elem[0].dl_TransportChannelType,rrc_DL_TransportChannelType_r7_dch))
      {
        /* Remember the mapped TRCH ID for SRB5 */
        srb5_trch_id = srb_setup_r8_ptr->rb_MappingInfo.elem[0].dl_LogicalChannelMappingList.elem[0].dl_TransportChannelType.u.dch;
      }
      else
      {
        WRRC_MSG0_ERROR("SRB#5 can't DL mapping other than DCH .. rejecting");
        return result;
      }
    }
    else
    {
      WRRC_MSG0_ERROR("SRB#5 can't DL mapping other than DCH .. rejecting");
      return result;
    }

    trch_idx = rrcllc_find_dl_dch_trch_idx((tr_ch_id_type)srb5_trch_id);

    if (trch_idx == RRCLLC_INVALID_TRCH_IDX)
    {
      WRRC_MSG1_ERROR("DL DCH TrCH ID %d, no idx found", srb5_trch_id);
      return result;
    }

    /* If we came this far, we found SRB5. Update OC with SRB5 info */
    ordered_config_ptr->srb5_info.srb5_rb_exists   = TRUE;
    ordered_config_ptr->srb5_info.rb_id            = srb5_id;
    ordered_config_ptr->srb5_info.srb5_trch_exists = TRUE;
    ordered_config_ptr->srb5_info.trch_id          = srb5_trch_id;


    WRRC_MSG2_HIGH("Updated OC with SRB5 INFO->rb id=%d, tchid=%d", srb5_id, srb5_trch_id);
    result = SUCCESS;
  }
  else
  {
    WRRC_MSG0_ERROR("One of the SRB pointers is NULL ... rejecting");
    result = FAILURE;
  }

  return result;
}



#ifdef FEATURE_CHANGE_SRB4_PRIRORITY
/*====================================================================
FUNCTION: rrcllc_check_and_set_srb3_srb4_priority()

DESCRIPTION:
  This function checks the priorities of SRB3 and SRB4 and set the priority of SR4
  priority if both the SRBs have equal priority. SRB4 should have lower priority.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.


SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_check_and_set_srb3_srb4_priority(void)
{
  uint8 dflow_idx,logch_idx,num_dflow,num_logch,srb3_pri=0,srb4_pri=0,srb2_pri=0,
        srb4_dflow_idx=0,srb4_logch_idx=0,srb3_dflow_idx=0,srb3_logch_idx=0,loop_break=0;

  if(ordered_config_ptr->ul_cctrch.cctrch_type == RACH)
  {
    //SRBs are mapped on RACH
    num_logch = ordered_config_ptr->ul_cctrch.rach_info.ndlchan;
    for (logch_idx = 0; logch_idx < num_logch; logch_idx++)
    {
      if (ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].rb_id == 
          DCCH_DT_HIGH_PRI_RADIO_BEARER_ID )
      {
        srb3_pri = ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].priority;
        srb3_logch_idx = logch_idx;
      }
      if (ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].rb_id == 
          DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        srb4_pri = ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].priority;
        srb4_logch_idx = logch_idx;
      }
      if(ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].rb_id == 
          DCCH_AM_RADIO_BEARER_ID)
      {
        srb2_pri = ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx].priority;
      }
    }
    
    if((srb3_pri == srb4_pri) || (srb3_pri == srb2_pri))
    {
      srb3_pri = srb2_pri + 1;
      srb4_pri = srb3_pri + 1;
      ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[srb4_logch_idx].priority = srb4_pri;
      ordered_config_ptr->ul_cctrch.rach_info.dlchan_info[srb3_logch_idx].priority = srb3_pri;
      WRRC_MSG2_HIGH("Lowered RACH SRB4 %d & SRB3 priority: %d",srb4_pri,srb3_pri);
    }
  }

  if(ordered_config_ptr->ul_cctrch.cctrch_type == DCH)
  {
    //SRBs are mapped on DCH
    num_dflow = ordered_config_ptr->ul_cctrch.dch_info.ndchs;
    for(dflow_idx =0;dflow_idx < num_dflow;num_dflow++)
    {
      num_logch = ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].ndlchan;
      for (logch_idx = 0; logch_idx < num_logch; logch_idx++)
      {
        if (ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].dlchan_info[logch_idx].rb_id == 
            DCCH_DT_HIGH_PRI_RADIO_BEARER_ID )
        {
          srb3_pri = ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].dlchan_info[logch_idx].priority;
          srb3_dflow_idx = dflow_idx;
          srb3_logch_idx = logch_idx;
        }
        if (ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].dlchan_info[logch_idx].rb_id == 
            DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
        {
          srb4_pri = ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].dlchan_info[logch_idx].priority;
          srb4_dflow_idx = dflow_idx;
          srb4_logch_idx = logch_idx;
        }
        if (ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].dlchan_info[logch_idx].rb_id == 
            DCCH_AM_RADIO_BEARER_ID)
        {
          srb2_pri = ordered_config_ptr->ul_cctrch.dch_info.dch_info[dflow_idx].dlchan_info[logch_idx].priority;
        }
      }
      if((srb2_pri != 0) && (srb3_pri != 0) && (srb4_pri != 0))
      {
        break;
      }
    }
    
    if((srb3_pri == srb4_pri) || (srb3_pri == srb2_pri))
    {
      srb3_pri = srb2_pri + 1;
      srb4_pri = srb3_pri + 1;
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[srb3_dflow_idx].dlchan_info[srb3_logch_idx].priority = srb3_pri;
      ordered_config_ptr->ul_cctrch.dch_info.dch_info[srb4_dflow_idx].dlchan_info[srb4_logch_idx].priority = srb4_pri;
      WRRC_MSG2_HIGH("Lowered RACH SRB4 %d & SRB3 priority: %d",srb4_pri,srb3_pri);
    }
  }

  //SRBs are mapped on E-DCH
  srb2_pri = srb3_pri = srb4_pri = 0;
  num_dflow = ordered_config_ptr->mac_e_config.num_e_mac_d_flow;
  for(dflow_idx =0,loop_break = 0;dflow_idx < num_dflow;num_dflow++)
  {
    num_logch = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].num_logch;
    for (logch_idx = 0; logch_idx < num_logch; logch_idx++)
    {
      if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].e_ul_logch_info[logch_idx].rb_id == 
          DCCH_DT_HIGH_PRI_RADIO_BEARER_ID )
      {
        srb3_pri = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].e_ul_logch_info[logch_idx].priority;
        srb3_dflow_idx = dflow_idx;
        srb3_logch_idx = logch_idx;
      }
      if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].e_ul_logch_info[logch_idx].rb_id == 
          DCCH_DT_LOW_PRI_RADIO_BEARER_ID )
      {
        srb4_pri = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].e_ul_logch_info[logch_idx].priority;
        srb4_dflow_idx = dflow_idx;
        srb4_logch_idx = logch_idx;
      }
      if (ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].e_ul_logch_info[logch_idx].rb_id == 
          DCCH_AM_RADIO_BEARER_ID )
      {
        srb2_pri = ordered_config_ptr->mac_e_config.e_mac_d_flow_info[dflow_idx].e_ul_logch_info[logch_idx].priority;
      }
    }
    if((srb2_pri != 0) && (srb3_pri != 0) && (srb4_pri != 0))
    {
      break;
    }

  }
  
  if((srb3_pri == srb4_pri) || (srb3_pri == srb2_pri))
  {
    srb3_pri = srb2_pri + 1;
    srb4_pri = srb3_pri + 1;
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[srb3_dflow_idx].e_ul_logch_info[srb3_logch_idx].priority = srb3_pri;
    ordered_config_ptr->mac_e_config.e_mac_d_flow_info[srb4_dflow_idx].e_ul_logch_info[srb4_logch_idx].priority = srb4_pri;
    WRRC_MSG2_HIGH("Lowered RACH SRB4 %d & SRB3 priority: %d",srb4_pri,srb3_pri);
  }
}
#endif/*FEATURE_CHANGE_SRB4_PRIRORITY*/
#ifdef FEATURE_WCDMA_HS_RACH
/*====================================================================
FUNCTION: rrcllc_update_hs_rach_params()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_update_hs_rach_params(ordered_config_type* config_ptr)
{

  uint8 i,logch_idx = 0,j;
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  rlc_lc_id_type rlc_lc_id = 0xFF;
  uecomdef_status_e_type ret_val= SUCCESS;
  uint8 index = MAX_E_DCH_MAC_D_FLOW;
 
  rrcllc_init_mac_hsrach_mapping_info(&config_ptr->mac_hs_rach_config);
 
  for(i=0;i<MAX_HANGING_RB_MAPPING_INFO;i++)
  {
    if((rrc_ordered_hanging_rb_mapping[i].in_use == TRUE ) && 
         (rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.valid == TRUE))
    {

        /* Do not update Mappings to config_ptr if CCCH mac_d flow id is configured */
        /* From section 10.3.5.3b 
                  NOTE 1:   For FDD, E-DCH MAC-d flow identity 7 is reserved for CCCH transmission. */
      if(rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id == HS_RACH_CCCH_D_FLOW_ID)
      {
        WRRC_MSG0_ERROR("Mac-d flow ID 7 cannot be configured");
        continue;        
      }
      /* Get index for macd flow from new hs rach structures which are updated from SIB5  */

      if((index = rrchsrach_find_e_mac_d_flow_idx(config_ptr,(uint8)rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id)) == MAX_E_DCH_MAC_D_FLOW)
      {
        WRRC_MSG2_HIGH("RRCHSRACH: failed to find e_mac-d flow %d for RB-ID %d, skip restore into OC",
                             (uint8)rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id,
                             rrc_ordered_hanging_rb_mapping[i].rb_id);
        continue;
      }

      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_mac_d_flow_id =
                 rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id;
      logch_idx = config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].num_logch;
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rb_id = 
                 rrc_ordered_hanging_rb_mapping[i].rb_id;
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].lc_id =
                rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.log_channel_id;
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].priority =
               rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_log_priority;
    
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].include_in_scheduling_info =
             rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.in_sched_info;           

#ifdef FEATURE_MAC_I
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_pdu_type = 
      rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.rlc_pdu_type;       
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].ul_li_size = 
       rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.ul_li_size;
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].ul_flex_min_pdu_size= 
                 rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.ul_flex_min_pdu_size;
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].ul_flex_max_pdu_size= 
                 rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.ul_flex_max_pdu_size;
#endif		  

      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].num_rlc_size_ddi_info =
            rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.num_rlc_size_ddi_info;
      for( j=0;j<rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.num_rlc_size_ddi_info;j++)
      {
        config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_size_ddi_info[j]=
            rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.rlc_size_ddi_info[j];
      }
 
      /* Determine the type of Logical Channel mapped to DCH TrCH */
      if (rrc_ordered_hanging_rb_mapping[i].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
      } 
      else
      {
        config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
      }
 
      rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id);

      if ((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)&&
          (rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM ))
      {
        WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rrc_ordered_hanging_rb_mapping[i].rb_id);
        ret_val= FAILURE;
      }
 
          /* Also update the Logical Channel ID here */
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_id = rlc_lc_id;
   
        /* Save the logical channel mode, if a setup */
      logch_mode = config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_mode = rrc_ordered_hanging_rb_mapping[i].logch_mode;
        
  
      WRRC_MSG3_MED("RRCHSRACH  rlc_lc_id %d rrc_ordered_hanging_rb_mapping[i].rb_id %d for logch_idx %d ",
                          rlc_lc_id,rrc_ordered_hanging_rb_mapping[i].rb_id,
                         logch_idx);

      if (UE_MODE_MAX_NUM == logch_mode)
      {
        if (rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
        {
          if(get_am_index_in_config_ptr(rrc_ordered_hanging_rb_mapping[i].rb_id,config_ptr) != UE_MAX_AM_ENTITY)
        {
          logch_mode = UE_MODE_ACKNOWLEDGED_DATA;            
          }           
          else if(get_um_ul_index( (rrc_RB_Identity)rrc_ordered_hanging_rb_mapping[i].rb_id,config_ptr)!= 
            UE_MAX_UL_UM_CHANNEL)         
          {
            logch_mode = UE_MODE_UNACKNOWLEDGED;
          }
          else if(get_ul_tm_index_in_config_ptr(rlc_lc_id,config_ptr)!= UE_MAX_UL_TM_CHANNEL)
          {
            logch_mode = UE_MODE_TRANSPARENT;
          }
          else
          {
            if(rrc_ordered_hanging_rb_mapping[i].logch_mode == UE_MODE_MAX_NUM )
            {
              WRRC_MSG1_ERROR("Failure to get UL Log Chl mode for LC %d", rlc_lc_id);
              ret_val= FAILURE;
            }
          }
        }

        config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].e_ul_logch_info[logch_idx].rlc_mode = logch_mode;
  
      }

      /* Increment number of logical channels */
      config_ptr->mac_hs_rach_config.hs_rach_config.e_mac_d_flow_info[index].num_logch++;  
 
      /* Here indicate that MAC Uplink needs to be reconfigured */
      config_ptr->reconfig_needed.uplink_mac = TRUE;
  
      WRRC_MSG3_MED("RRCHSRACH Restore MAC_E data for RB_id %d at  index %d for flow id %d ",
                        rrc_ordered_hanging_rb_mapping[i].rb_id,i,
                       rrc_ordered_hanging_rb_mapping[i].ul_mapping_info.ul_mace.mac_e_flow_id);
    }

  logch_idx = 0;
  logch_mode= UE_MODE_MAX_NUM;
  rlc_lc_id = 0xFF;
  index = MAX_E_DCH_MAC_D_FLOW;
  }
  return ret_val;

}

#endif

#ifdef FEATURE_WCDMA_HS_FACH

/*====================================================================
FUNCTION: print_ul_type()

DESCRIPTION: Prints UL mapping type

DEPENDENCIES:
  None.

RETURN VALUE:
#define UL_DCH_MAPPING         0x0010
#define UL_EDCH_MAPPING        0x0020
#define UL_RACH_MAPPING        0x0040
#define INVALID_MAPPING        0x0080


SIDE EFFECTS:
  None.
====================================================================*/

int print_ul_type(uint8 combo)
{
  return (combo & UL_STUB);
}

/*====================================================================
FUNCTION: print_dl_type()

DESCRIPTION: Prints DL mapping type

DEPENDENCIES:
  None.

RETURN VALUE:
#define DL_DCH_MAPPING         0x0001
#define DL_MAC_D_MAPPING       0x0002
#define DL_MAC_EHS_MAPPING     0x0004
#define DL_FACH_MAPPING        0x0008


SIDE EFFECTS:
  None.
====================================================================*/

int print_dl_type(uint8 combo)
{
  return (combo & DL_STUB);
}

/*====================================================================
FUNCTION: get_ul_mapping_r99_hrb()

DESCRIPTION: Updates uplink mapping info from OTA into ul_map structure

DEPENDENCIES:
  None.

RETURN VALUE: UL mapping type
#define UL_DCH_MAPPING         0x0010
#define UL_EDCH_MAPPING        0x0020
#define UL_RACH_MAPPING        0x0040
#define INVALID_MAPPING        0x0080

SIDE EFFECTS:
  None.
====================================================================*/

uint8 get_ul_mapping_r99_hrb
(
  rrc_UL_LogicalChannelMappings *ul_logChan_mapping,
  ul_mapping_info_type_ *ul_map,
  uint8 hrb_index
)
{
  rrc_UL_LogicalChannelMapping *ie_ptr =NULL;

  if(ul_logChan_mapping->t != T_rrc_UL_LogicalChannelMappings_oneLogicalChannel)
  {
    return INVALID_MAPPING;
  }
  ie_ptr = ul_logChan_mapping->u.oneLogicalChannel;
  switch (ie_ptr->ul_TransportChannelType.t)
  {
    case T_rrc_UL_TransportChannelType_dch:
      /*Do nothing for now*/
      return UL_DCH_MAPPING;

    case T_rrc_UL_TransportChannelType_rach:
      ul_map->rach_map_info.valid = TRUE;
      ul_map->rach_map_info.mac_log_priority = (uint8)ie_ptr->mac_LogicalChannelPriority;
      if(ie_ptr->m.logicalChannelIdentityPresent)
        ul_map->rach_map_info.log_channel_id = (uint8)ie_ptr->logicalChannelIdentity;
      else
        ul_map->rach_map_info.log_channel_id = 0;
      if(get_rlc_size_list(ie_ptr,&ul_map->rach_map_info.size_rest_info,
                           rrc_ordered_hanging_rb[hrb_index].rb_id) == FAILURE)
      {
        memset(ul_map,0,sizeof(ul_mapping_info_type_));
        return INVALID_MAPPING;
      }
      return UL_RACH_MAPPING;

    case T_rrc_UL_TransportChannelType_dummy:
    case T_rrc_UL_TransportChannelType_usch:
    default:
      WRRC_MSG1_ERROR("Invalid Value received %d", ie_ptr->ul_TransportChannelType.t);
      return INVALID_MAPPING;
  }
}


/*====================================================================
FUNCTION: get_ul_mapping_r6_hrb()

DESCRIPTION: Updates uplink mapping info from OTA into ul_map structure

DEPENDENCIES:
  None.

RETURN VALUE: UL mapping type
#define UL_DCH_MAPPING         0x0010
#define UL_EDCH_MAPPING        0x0020
#define UL_RACH_MAPPING        0x0040
#define INVALID_MAPPING        0x0080

SIDE EFFECTS:
  None.
====================================================================*/

uint8 get_ul_mapping_r6_hrb
(
  rrc_UL_LogicalChannelMappings_r6 *ul_logChan_mapping,
  ul_mapping_info_type_ *ul_map,
  uint8 hrb_index
)
{
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr =NULL;
  rrc_RLC_PDU_SizeList *rlc_pdu_sizelist  =NULL;
  uint8 ddi = 0;
  uint32 cnt = 0;
  uint16 rlc_size = 0;

  if(ul_logChan_mapping->t != T_rrc_UL_LogicalChannelMappings_r6_oneLogicalChannel)
  {
    return INVALID_MAPPING;
  }
  ie_ptr = ul_logChan_mapping->u.oneLogicalChannel;
  if(ie_ptr->ul_TrCH_Type.t == T_rrc_UL_LogicalChannelMapping_r6_ul_TrCH_Type_dch_rach_usch)
  {
    switch (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t)
    {
      case T_rrc_UL_TransportChannelType_dch:
        /*Do nothing for now*/
        return UL_DCH_MAPPING;
  
      case T_rrc_UL_TransportChannelType_rach:
        ul_map->rach_map_info.valid = TRUE;
        ul_map->rach_map_info.mac_log_priority = (uint8)ie_ptr->mac_LogicalChannelPriority;
        if(ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
          ul_map->rach_map_info.log_channel_id = (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
        else
          ul_map->rach_map_info.log_channel_id = 0;
        if(get_rlc_size_list_r6(ie_ptr,&ul_map->rach_map_info.size_rest_info,
                             rrc_ordered_hanging_rb[hrb_index].rb_id) == FAILURE)
        {
          memset(ul_map,0,sizeof(ul_mapping_info_type_));
          return INVALID_MAPPING;
        }
        return UL_RACH_MAPPING;

      case T_rrc_UL_TransportChannelType_dummy:
      case T_rrc_UL_TransportChannelType_usch:
      default:
        WRRC_MSG1_ERROR("Invalid Value received %d", ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t);
        return INVALID_MAPPING;
    }
  }
  if(ie_ptr->ul_TrCH_Type.t == T_rrc_UL_LogicalChannelMapping_r6_ul_TrCH_Type_e_dch)
  {
    /*Backup EUL */
    ul_map->edch_map_info.valid = TRUE;
#ifdef FEATURE_MAC_I
    ul_map->edch_map_info.rlc_pdu_type = RLC_FIXED_PDU_SIZE;
#endif
    ul_map->edch_map_info.mac_e_flow_id =     (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->e_DCH_MAC_d_FlowIdentity;
    ul_map->edch_map_info.mac_log_priority  = (uint8) ie_ptr->mac_LogicalChannelPriority;
    ul_map->edch_map_info.log_channel_id =    (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity;

    rlc_pdu_sizelist = &ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_SizeList;
    /* If more than 1 UL RLC PDU size is configured for this RB, the different sizes will use subsequent DDI values starting
      from this DDI value.*/
    ddi = ie_ptr->ul_TrCH_Type.u.e_dch->ddi;
    ul_map->edch_map_info.num_rlc_size_ddi_info = 0;
    while ((rlc_pdu_sizelist->n > cnt) && (cnt < RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN)) 
    {
      ul_map->edch_map_info.rlc_size_ddi_info[cnt].ddi = ddi;
      switch (rlc_pdu_sizelist->elem[cnt].t)
      {
        case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
          rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType1 * 8) + 16;
          break;
        case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
          rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part1 * 32) + 272;
          if (rlc_pdu_sizelist->elem[cnt].u.sizeType2->m.part2Present)
          {
            rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part2 * 8);
          }
          break;
        case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
          rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part1 * 64) + 1040;
          if (rlc_pdu_sizelist->elem[cnt].u.sizeType3->m.part2Present)
          {
            rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part2 * 8);
          }
          break;
        default:
          WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
              rlc_pdu_sizelist->elem[cnt].t);
          break;
      }
      ul_map->edch_map_info.rlc_size_ddi_info[cnt].rlc_pdu_size_in_bits = rlc_size;

      /* Increment the number of valid fields in rlc_size_ddi_info[] */
      ul_map->edch_map_info.num_rlc_size_ddi_info++;
      cnt++;
      /* Increment the DDI */
      ddi++;
    }

  /* If set include scheduling info is set to false then dont consider this mac_id while sending schedulingInformation */
    ul_map->edch_map_info.in_sched_info = ie_ptr->ul_TrCH_Type.u.e_dch->includeInSchedulingInfo;

    WRRC_MSG2_HIGH("RRC_HRB: R6 E_DCH for RB-ID %d for FLOW ID %d",
                               rrc_ordered_hanging_rb[hrb_index].rb_id,
                               ul_map->edch_map_info.mac_e_flow_id);
    return UL_EDCH_MAPPING;
  }
  return INVALID_MAPPING;
}


/*====================================================================
FUNCTION: get_ul_mapping_r8_hrb()

DESCRIPTION: Updates uplink mapping info from OTA into ul_map structure

DEPENDENCIES:
  None.

RETURN VALUE: UL mapping type
#define UL_DCH_MAPPING         0x0010
#define UL_EDCH_MAPPING        0x0020
#define UL_RACH_MAPPING        0x0040
#define INVALID_MAPPING        0x0080

SIDE EFFECTS:
  None.
====================================================================*/

uint8 get_ul_mapping_r8_hrb
(
  rrc_UL_LogicalChannelMappings_r8 *ul_logChan_mapping,
  ul_mapping_info_type_ *ul_map,
  uint8 hrb_index
)
{
  rrc_UL_LogicalChannelMapping_r8 *ie_ptr =NULL;
  rrc_RLC_PDU_SizeList *rlc_pdu_sizelist  =NULL;
  uint8 ddi = 0;
  uint32 cnt = 0;
  uint16 rlc_size = 0;

  if(ul_logChan_mapping->t != T_rrc_UL_LogicalChannelMappings_r8_oneLogicalChannel)
  {
    return INVALID_MAPPING;
  }
  ie_ptr = ul_logChan_mapping->u.oneLogicalChannel;
  if(ie_ptr->ul_TrCH_Type.t == T_rrc_UL_LogicalChannelMapping_r8_ul_TrCH_Type_dch_rach_usch)
  {
    switch (ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t)
    {
      case T_rrc_UL_TransportChannelType_dch:
        /*Do nothing for now*/
        return UL_DCH_MAPPING;
  
      case T_rrc_UL_TransportChannelType_rach:
        ul_map->rach_map_info.valid = TRUE;
        ul_map->rach_map_info.mac_log_priority = (uint8)ie_ptr->mac_LogicalChannelPriority;
        if(ie_ptr->ul_TrCH_Type.u.dch_rach_usch->m.logicalChannelIdentityPresent)
          ul_map->rach_map_info.log_channel_id = (uint8)ie_ptr->ul_TrCH_Type.u.dch_rach_usch->logicalChannelIdentity;
        else
          ul_map->rach_map_info.log_channel_id = 0;
        if(get_rlc_size_list_r8(ie_ptr,&ul_map->rach_map_info.size_rest_info,
                             rrc_ordered_hanging_rb[hrb_index].rb_id) == FAILURE)
        {
          memset(ul_map,0,sizeof(ul_mapping_info_type_));
          return INVALID_MAPPING;
        }
        return UL_RACH_MAPPING;
  
      case T_rrc_UL_TransportChannelType_dummy:
      case T_rrc_UL_TransportChannelType_usch:
      default:
        WRRC_MSG1_ERROR("Invalid Value received %d", ie_ptr->ul_TrCH_Type.u.dch_rach_usch->ul_TransportChannelType.t);
        return INVALID_MAPPING;
    }
  }
  else if(ie_ptr->ul_TrCH_Type.t == T_rrc_UL_LogicalChannelMapping_r8_ul_TrCH_Type_e_dch)
  {
    /*Backup EUL */
    
    ul_map->edch_map_info.valid = TRUE;
    ul_map->edch_map_info.mac_e_flow_id =     (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->e_DCH_MAC_d_FlowIdentity;
    ul_map->edch_map_info.mac_log_priority  = (uint8) ie_ptr->mac_LogicalChannelPriority;
    ul_map->edch_map_info.log_channel_id =    (uint8)ie_ptr->ul_TrCH_Type.u.e_dch->logicalChannelIdentity;

    if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_flexibleSize)
    {
#ifdef FEATURE_MAC_I
      ul_map->edch_map_info.rlc_pdu_type =  RLC_FLEXIBLE_PDU_SIZE ;

      /*Flexible size shouldnt be configured for SRBs*/
      if (rrc_ordered_hanging_rb[hrb_index].rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
      {
        WRRC_MSG1_HIGH("MAC_I:RB ID %d configured with flexible UL PDU size",rrc_ordered_hanging_rb[hrb_index].rb_id);
        return INVALID_MAPPING;
      }

      ul_map->edch_map_info.ul_flex_min_pdu_size = 
        (ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->minRLC_PDU_Size*8)+16;
      ul_map->edch_map_info.ul_flex_max_pdu_size= 
        (ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->largestRLC_PDU_Size*8)+16;

      if(rrc_ordered_hanging_rb[hrb_index].logch_mode == UE_MODE_ACKNOWLEDGED_DATA)
      {
        if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->m.lengthIndicatorSizePresent)
        {
          if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->lengthIndicatorSize == rrc_RLC_PDU_SizeConstraint_lengthIndicatorSize_size7)
          {
            ul_map->edch_map_info.ul_li_size= LI_7_BITS;
          }
          else if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.flexibleSize->lengthIndicatorSize == rrc_RLC_PDU_SizeConstraint_lengthIndicatorSize_size15)
          {
            ul_map->edch_map_info.ul_li_size= LI_15_BITS;
          }
        }
        else
        {
          WRRC_MSG1_HIGH("MAC_I:LI not given for a AM RLC RB ID %d ",rrc_ordered_hanging_rb[hrb_index].rb_id);
        }
      }
#else
      WRRC_MSG0_ERROR("REL8: rrc_rlc_PDU_Size_flexibleSize not supported");
      return INVALID_MAPPING;
#endif
    }  
    else if(ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.t == T_rrc_UL_LogicalChannelMapping_r8_rlc_PDU_Size_fixedSize)	
    {
      rlc_pdu_sizelist = &ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.fixedSize->rlc_PDU_SizeList;
      ddi = ie_ptr->ul_TrCH_Type.u.e_dch->rlc_PDU_Size.u.fixedSize->ddi;
#ifdef FEATURE_MAC_I
      ul_map->edch_map_info.rlc_pdu_type = RLC_FIXED_PDU_SIZE;
#endif /* FEATURE_MAC_I */
    /* If more than 1 UL RLC PDU size is configured for this RB, the different sizes will use subsequent DDI values starting
      from this DDI value.*/
      ul_map->edch_map_info.num_rlc_size_ddi_info = 0;
      while ((rlc_pdu_sizelist->n > cnt) && (cnt < RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN)) 
      {
        ul_map->edch_map_info.rlc_size_ddi_info[cnt].ddi = ddi;
        switch (rlc_pdu_sizelist->elem[cnt].t)
        {
         case T_rrc_OctetModeRLC_SizeInfoType1_sizeType1:
           rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType1 * 8) + 16;
           break;
         case T_rrc_OctetModeRLC_SizeInfoType1_sizeType2:
           rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part1 * 32) + 272;
           if (rlc_pdu_sizelist->elem[cnt].u.sizeType2->m.part2Present)
           {
             rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType2->part2 * 8);
           }
           break;
         case T_rrc_OctetModeRLC_SizeInfoType1_sizeType3:
           rlc_size = (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part1 * 64) + 1040;
           if (rlc_pdu_sizelist->elem[cnt].u.sizeType3->m.part2Present)
           {
             rlc_size += (rlc_pdu_sizelist->elem[cnt].u.sizeType3->part2 * 8);
           }
           break;
         default:
           WRRC_MSG1_ERROR("Invalid RRC TF type in OctetMode %d",
               rlc_pdu_sizelist->elem[cnt].t);
           break;
        }
        ul_map->edch_map_info.rlc_size_ddi_info[cnt].rlc_pdu_size_in_bits = rlc_size;
        /* Increment the number of valid fields in rlc_size_ddi_info[] */
        ul_map->edch_map_info.num_rlc_size_ddi_info++;
        cnt++;
        /* Increment the DDI */
        ddi++;
      }
    }
    WRRC_MSG2_MED("REL8: E_DCH for RB-ID %d for FLOW ID %d",
                           rrc_ordered_hanging_rb[hrb_index].rb_id,
                           ul_map->edch_map_info.mac_e_flow_id);
    return UL_EDCH_MAPPING;
  }
  return INVALID_MAPPING;
}



/*====================================================================
FUNCTION: get_dl_mapping_r99_hrb()

DESCRIPTION: Updates downlink mapping info from OTA into dl_map structure

DEPENDENCIES:
  None.

RETURN VALUE: UL mapping type

#define DL_DCH_MAPPING         0x0001
#define DL_MAC_D_MAPPING       0x0002
#define DL_MAC_EHS_MAPPING     0x0004
#define DL_FACH_MAPPING        0x0008

SIDE EFFECTS:
  None.
====================================================================*/

uint8 get_dl_mapping_r99_hrb
(
  rrc_DL_LogicalChannelMapping * ie_ptr,
  dl_mapping_info_type_ *dl_map,
  uint8 hrb_index
)
{
  switch (ie_ptr->dl_TransportChannelType.t)
  {
    case T_rrc_DL_TransportChannelType_dch:
      return DL_DCH_MAPPING;

    case T_rrc_DL_TransportChannelType_fach:
      dl_map->fach_map_info.valid = TRUE;
      if(ie_ptr->m.logicalChannelIdentityPresent)
        dl_map->fach_map_info.log_channel_id = ie_ptr->logicalChannelIdentity;
      else
        dl_map->fach_map_info.log_channel_id = 0;
      return DL_FACH_MAPPING;

    case T_rrc_DL_TransportChannelType_dsch:
    case T_rrc_DL_TransportChannelType_dch_and_dsch:
    default:
      WRRC_MSG1_ERROR("Invalid Value received %d", ie_ptr->dl_TransportChannelType.t);
      return INVALID_MAPPING;
  }
}


/*====================================================================
FUNCTION: get_dl_mapping_r5_hrb()

DESCRIPTION: Updates downlink mapping info from OTA into dl_map structure

DEPENDENCIES:
  None.

RETURN VALUE: UL mapping type

#define DL_DCH_MAPPING         0x0001
#define DL_MAC_D_MAPPING       0x0002
#define DL_MAC_EHS_MAPPING     0x0004
#define DL_FACH_MAPPING        0x0008

SIDE EFFECTS:
  None.
====================================================================*/

uint8 get_dl_mapping_r5_hrb
(
  rrc_DL_LogicalChannelMapping_r5 * ie_ptr,
  dl_mapping_info_type_ *dl_map,
  uint8 hrb_index
)
{
  switch (ie_ptr->dl_TransportChannelType.t)
  {
    case T_rrc_DL_TransportChannelType_r5_hsdsch:
      return DL_MAC_D_MAPPING;
    case T_rrc_DL_TransportChannelType_r5_dch:
    case T_rrc_DL_TransportChannelType_r5_dsch:
    case T_rrc_DL_TransportChannelType_r5_dch_and_dsch:
      return  DL_DCH_MAPPING;

    case T_rrc_DL_TransportChannelType_r5_fach:
      dl_map->fach_map_info.valid = TRUE;
      if(ie_ptr->m.logicalChannelIdentityPresent)
        dl_map->fach_map_info.log_channel_id = ie_ptr->logicalChannelIdentity;
      else
        dl_map->fach_map_info.log_channel_id = 0;
      return DL_FACH_MAPPING;
  }
  return INVALID_MAPPING;
}



/*====================================================================
FUNCTION: get_dl_mapping_r7_hrb()

DESCRIPTION: Updates downlink mapping info from OTA into dl_map structure

DEPENDENCIES:
  None.

RETURN VALUE: UL mapping type

#define DL_DCH_MAPPING         0x0001
#define DL_MAC_D_MAPPING       0x0002
#define DL_MAC_EHS_MAPPING     0x0004
#define DL_FACH_MAPPING        0x0008

SIDE EFFECTS:
  None.
====================================================================*/

uint8 get_dl_mapping_r7_hrb
(
  rrc_DL_LogicalChannelMapping_r7 * ie_ptr,
  dl_mapping_info_type_ *dl_map,
  uint8 hrb_index
)
{
  rrc_DL_TransportChannelType_r7_hsdsch *hsdsch = NULL;
  switch (ie_ptr->dl_TransportChannelType.t)
  {
    case  T_rrc_DL_TransportChannelType_r7_dch :
    case  T_rrc_DL_TransportChannelType_r7_dsch :
    case  T_rrc_DL_TransportChannelType_r7_dch_and_dsch :
      return  DL_DCH_MAPPING;

    case T_rrc_DL_TransportChannelType_r7_fach :
      dl_map->fach_map_info.valid = TRUE;
      if(ie_ptr->m.logicalChannelIdentityPresent)
        dl_map->fach_map_info.log_channel_id = ie_ptr->logicalChannelIdentity;
      else
        dl_map->fach_map_info.log_channel_id = 0;
      return DL_FACH_MAPPING;

    case  T_rrc_DL_TransportChannelType_r7_hsdsch :
      hsdsch = ie_ptr->dl_TransportChannelType.u.hsdsch;
      if(hsdsch->t == T_rrc_DL_TransportChannelType_r7_hsdsch_mac_ehs)
      {
        dl_map->macehs_map_info.valid = TRUE;

        if(ie_ptr->m.logicalChannelIdentityPresent)
          dl_map->macehs_map_info.log_channel_id = ie_ptr->logicalChannelIdentity;
        else
          dl_map->macehs_map_info.log_channel_id = 0;
        dl_map->macehs_map_info.queue_id = hsdsch->u.mac_ehs;
        return DL_MAC_EHS_MAPPING;
      }
      else return DL_MAC_D_MAPPING;
      break;
    case  T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch :
      if(ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->t == 
          T_rrc_DL_TransportChannelType_r7_dch_and_hsdsch_mac_ehs)
      {
        dl_map->macehs_map_info.valid = TRUE;
        if(ie_ptr->m.logicalChannelIdentityPresent)
          dl_map->macehs_map_info.log_channel_id = ie_ptr->logicalChannelIdentity;
        else
          dl_map->macehs_map_info.log_channel_id = 0;
        dl_map->macehs_map_info.queue_id = 
         ie_ptr->dl_TransportChannelType.u.dch_and_hsdsch->u.mac_ehs->hsdsch_mac_ehs_QueueId;
      }
      else
      {
        return DL_MAC_D_MAPPING;
      }
      break;
  }
  return INVALID_MAPPING;
}


/*==========================================================
FUNCTION  :   rrcllc_restore_macehs_mapping()

DESCRIPTION : This function restores the Rb_mapping info from the RB-mapping backup info
               to the Ordered config for MAC-D flow id.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              return SUCCESS

SIDE EFFECTS: None
============================================================*/
boolean rrcllc_restore_macehs_mapping
(
  ordered_config_type * config_ptr,
  uint8 rb_index,
  uint8 hrb_index
)
{
  uint8 i;
  uecomdef_logch_mode_e_type logch_mode = UE_MODE_MAX_NUM;
  rlc_lc_id_type rlc_lc_id;
  uint8 lc_index,rb_id = rrc_ordered_hanging_rb[rb_index].rb_id,logicalChannelIdentity;


  if(rrc_ordered_hanging_rb[rb_index].mapping_info[hrb_index].dl_map.macehs_map_info.valid == TRUE )
  {

    logicalChannelIdentity = rrc_ordered_hanging_rb[rb_index].mapping_info[hrb_index].dl_map.macehs_map_info.log_channel_id;
    lc_index = rrcllc_find_lc_index_for_efach(config_ptr, logicalChannelIdentity,rb_id);
    if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
    {
       /* Allocate LC index */
       lc_index = rrcllc_allocate_lc_index_efach(config_ptr, logicalChannelIdentity);
   
       if(lc_index == UE_MAX_DL_LOGICAL_CHANNEL)
       {
         /* Failure to allocate a new LC Index Index */
         WRRC_MSG0_ERROR("MACEHS: Failure to allocate new Lc iD index");
         return(FAILURE);
       }
    }
    /*It means that Th info for LCH-ID logicalChannelIdentity is aready present in DB*/
    else
    {
      if(config_ptr->efach_dl_logchan_info[lc_index].rb_id != rb_id)
      {
        MSG_ERROR("EFACH:LCH-ID %d already present for RB %d and duplicated for Rb %d",
          logicalChannelIdentity,config_ptr->efach_dl_logchan_info[lc_index].rb_id,rb_id);
        return(FAILURE);
      }
    }
    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if (rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      WRRC_MSG1_HIGH("MACEHS: LC ID not allocated for RB %d", rb_id);
    }
    else if (rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      WRRC_MSG1_HIGH("MACEHS: Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
    }
    /* Also update the Logical Channel ID here */

    if (rlc_lc_id  != RRCLCM_RLC_LC_ID_NOT_FOUND)
    {
      config_ptr->efach_dl_logchan_info[lc_index].rlc_id = rlc_lc_id;
      config_ptr->efach_dl_logchan_info[lc_index].rlc_mode= logch_mode;
    }
    else
    {
      /*LC id allocation and filling happens in config dcch dtch. Update only logical channel mode*/
      config_ptr->efach_dl_logchan_info[lc_index].rlc_mode = rrc_ordered_hanging_rb[rb_index].logch_mode;
    }
    config_ptr->efach_dl_logchan_info[lc_index].rb_id = rb_id;
    if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID )
    {
      config_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_DCCH;
    }
    else
    {
      config_ptr->efach_dl_logchan_info[lc_index].chan_type = UE_LOGCHAN_DTCH;
    }
    config_ptr->efach_dl_logchan_info[lc_index].queue_id =
      rrc_ordered_hanging_rb[rb_index].mapping_info[hrb_index].dl_map.macehs_map_info.queue_id;

    WRRC_MSG3_HIGH("EFACH:Restore EHS mapping of RB-ID:%d, MACEHS: Queue ID %d mapped to LC id %d",
    rb_id, config_ptr->efach_dl_logchan_info[lc_index].queue_id,config_ptr->efach_dl_logchan_info[lc_index].mac_id);

  }
  else return FALSE;


  rrcllc_init_hsdpa_l1_queue(config_ptr);
  for(i = 0; i < config_ptr->mac_dl_parms.efach_ndlchans; i++)
  {
    config_ptr->l1_hsdpa_info.hs_dsch_info.mac_ehs_dlchan_q_info[config_ptr->efach_dl_logchan_info[i].mac_id] 
           = config_ptr->efach_dl_logchan_info[i].queue_id;
  }

  return TRUE;
}

/*==========================================================
FUNCTION  :   rrcllc_restore_rach_rb_mapping()

DESCRIPTION : This function restores the Rb_mapping info from
              the RB-mapping backup info to the Ordered config
              for DL DCh.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              returns  SUCCESS

SIDE EFFECTS: None
============================================================*/
boolean rrcllc_restore_rach_mapping
(
  ordered_config_type * config_ptr,
  uint8 rb_index,
  uint8 hrb_index
)
{
  uint8 logch_idx,trch_idx;
  uecomdef_logch_mode_e_type logch_mode;
  rlc_lc_id_type rlc_lc_id;
  boolean ret_val= TRUE;
  ul_rb_rach_mapping_info_type  *rach_mapping = NULL;
  rrcllcoc_rlc_size_list_type     *rlc_size_rec = NULL;
  mac_ul_ded_logch_config_type    *dlchan_info =  NULL;
  uint8 rb_id = rrc_ordered_hanging_rb[rb_index].rb_id;

  rach_mapping = &rrc_ordered_hanging_rb[rb_index].mapping_info[hrb_index].ul_map.rach_map_info;

  if((rach_mapping !=NULL) && (rach_mapping->valid == TRUE))
  {
    logch_idx = config_ptr->cell_fach_rb_mapping_info.ul_ndlchan;
    if(logch_idx >= MAX_DED_LOGCHAN_PER_TRCH)
    {
      WRRC_MSG1_HIGH("logch_idx is %d greater than MAX_DED_LOGCHAN_PER_TRCH ",logch_idx);
      return FAILURE;
    }
    config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].rb_id = rb_id;
    config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].mac_id =
               rach_mapping->log_channel_id;
    config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].priority =
               rach_mapping->mac_log_priority;
    /* Determine the type of Logical Channel mapped to DCH TrCH */
    if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
    }
    else
    {
      config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
    }
   
    rlc_lc_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rb_id);
      
    if((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND) && 
       (rrc_ordered_hanging_rb[rb_index].logch_mode == UE_MODE_MAX_NUM ))
    {
       WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
       ret_val= FALSE;
    }
     
    if(rrclcm_get_ul_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      if(get_am_index_wo_f3(rb_id) != UE_MAX_AM_ENTITY)
      {
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      else if(get_ul_um_index((rrc_RB_Identity)rb_id) != UE_MAX_DL_UM_CHANNEL) 
      {
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      else if(get_ul_tm_index(rlc_lc_id)!= UE_MAX_DL_TM_CHANNEL)
      {
        logch_mode = UE_MODE_TRANSPARENT;
      }
      else
      {
        if(rrc_ordered_hanging_rb[rb_index].logch_mode == UE_MODE_MAX_NUM )
        {
           WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
           ret_val= FALSE;
        }
        else
        {
          logch_mode = rrc_ordered_hanging_rb[rb_index].logch_mode;
        }
      }
    }
    /* Also update the Logical Channel ID here */
    config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].rlc_id = rlc_lc_id;
      
    /* Save the logical channel mode, if a setup */
    config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx].rlc_mode = logch_mode;
      
    /* Increment number of logical channels */
    //config_ptr->cell_fach_rb_mapping_info.ul_ndlchan++;

    config_ptr->reconfig_needed.uplink_mac = TRUE;


    if((trch_idx = rrcllc_find_ul_rach_trch_idx()) < UE_MAX_TRCH)
    {
      logch_idx = config_ptr->ul_cctrch.rach_info.ndlchan;
      if(logch_idx >= MAX_DED_LOGCHAN_PER_TRCH)
      {
        WRRC_MSG1_HIGH("logch_idx is %d greater than MAX_DED_LOGCHAN_PER_TRCH ",logch_idx);
        return FAILURE;
      }
      dlchan_info = &config_ptr->ul_cctrch.rach_info.dlchan_info[logch_idx];
      /*Assuming logch_idx doesnt go out of sync between  ul_cctrch.rach_info.dlchan_info[logch_idx] and
                                                                                  cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx]*/
      WCDMA_MEMCPY(dlchan_info, 
                   sizeof(mac_ul_ded_logch_config_type),
                   &config_ptr->cell_fach_rb_mapping_info.ul_dlchan_info[logch_idx],
                   sizeof(mac_ul_ded_logch_config_type));
      
      rlc_size_rec = &(config_ptr->rach_rlc_size_restriction_info.rlc_size_list[logch_idx]);
      WCDMA_MEMCPY(rlc_size_rec,
                   sizeof(rrcllcoc_rlc_size_list_type),
                   &rach_mapping->size_rest_info,
                   sizeof(rrcllcoc_rlc_size_list_type));
    
      config_ptr->ul_cctrch.rach_info.ndlchan++;
      config_ptr->cell_fach_rb_mapping_info.ul_ndlchan++;
       
      config_ptr->ul_rach_trch_info[trch_idx].lg_ch_cnt = 
        config_ptr->ul_cctrch.rach_info.ndlchan;
    }
    else
    {
      ret_val = FALSE;
      WRRC_MSG0_HIGH("RRC_HRB: RACH mapping not completely restored!!!");
    }

  }
  else ret_val = FALSE;

  return ret_val;
}

/*==========================================================
FUNCTION  :   rrcllc_restore_fach_rb_mapping()

DESCRIPTION : This function restores the Rb_mapping info from
              the RB-mapping backup info to the Ordered config
              for DL DCh.

DEPENDENCIES: None.

RETURN VALUE: FALSE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              returns  TRUE

SIDE EFFECTS: None
============================================================*/
boolean rrcllc_restore_fach_rb_mapping
(
  ordered_config_type * config_ptr,
  uint8 rb_index,
  uint8 hrb_index
)
{
  uint8 i =0,logch_idx;
  uecomdef_logch_mode_e_type logch_mode;
  rlc_lc_id_type rlc_lc_id;
  boolean ret_val= TRUE;
  dl_rb_fach_mapping_info_type *fach_map_info =NULL;

  uint8 rb_id = rb_id = rrc_ordered_hanging_rb[rb_index].rb_id;

  fach_map_info = &rrc_ordered_hanging_rb[rb_index].mapping_info[hrb_index].dl_map.fach_map_info;

  if(fach_map_info->valid == TRUE)
  {
    logch_idx = config_ptr->cell_fach_rb_mapping_info.dl_ndlchan;
    config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].rb_id = rb_id;
    config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].mac_id = fach_map_info->log_channel_id;
    /* Determine the type of Logical Channel mapped to DCH TrCH */
    if(rb_id <= DCCH_DT_LOW_PRI_RADIO_BEARER_ID)
    {
      config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DCCH;
    }
    else
    {
      config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].chan_type = UE_LOGCHAN_DTCH;
    }

    rlc_lc_id = rrclcm_get_dl_rlc_lc_id((rrc_RB_Identity)rb_id);
    if((rlc_lc_id  == RRCLCM_RLC_LC_ID_NOT_FOUND) && 
        (rrc_ordered_hanging_rb[rb_index].logch_mode == UE_MODE_MAX_NUM ))
    {
      WRRC_MSG1_ERROR("Invalid DL LC ID for RB %d", rb_id);
      ret_val = FALSE;
    }

    if(rrclcm_get_dl_rlc_mode(rlc_lc_id, &logch_mode) == RRCLCM_FAILURE)
    {
      if(get_am_index_wo_f3(rb_id) != UE_MAX_AM_ENTITY)
      {
        logch_mode = UE_MODE_ACKNOWLEDGED_DATA;
      }
      else if(get_dl_um_index((rrc_RB_Identity)rb_id, 
          config_ptr)!= UE_MAX_DL_UM_CHANNEL) 
      {
        logch_mode = UE_MODE_UNACKNOWLEDGED;
      }
      else if(get_dl_tm_index(rlc_lc_id)!= UE_MAX_DL_TM_CHANNEL)
      {
        logch_mode = UE_MODE_TRANSPARENT;
      }
      else
      {
        /*LC id allocation and filling happens in config dcch dtch. Update only logical channel mode*/
        logch_mode = rrc_ordered_hanging_rb[rb_index].logch_mode;
        if(rrc_ordered_hanging_rb[rb_index].logch_mode == UE_MODE_MAX_NUM )
        {
          WRRC_MSG1_ERROR("Failure to get DL Log Chl mode for LC %d", rlc_lc_id);
          ret_val = FALSE;
        }
      }
    }
    /* Also update the Logical Channel ID here */
    config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].rlc_id = rlc_lc_id;
  
    /* Save the logical channel mode, if a setup */
    config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx].rlc_mode = logch_mode;
  
    /* Increment number of logical channels */
    config_ptr->cell_fach_rb_mapping_info.dl_ndlchan++;

    config_ptr->reconfig_needed.downlink_mac = TRUE;

    /* Copy for all FACHes */
    for( i= 0; i < UE_MAX_TRCH; i ++)
    {
      /* Copy number of DL DEdicated logical channels on FACH */
      config_ptr->fach_info[i].ndlchan = config_ptr->cell_fach_rb_mapping_info.dl_ndlchan;

      /* Copy mapping info for DL dedicated logical channels */
      config_ptr->fach_info[i].dlchan_info[logch_idx] =
          config_ptr->cell_fach_rb_mapping_info.dl_dlchan_info[logch_idx];
    }
  } 
  else ret_val = FALSE;

  return ret_val;
}

/*====================================================================
FUNCTION: rrcllc_store_mapping_in_hanging_rb()

DESCRIPTION:
Stores into HRB

DEPENDENCIES:
  None.

RETURN VALUE: SUCCESS /FAILURE

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_store_mapping_in_hanging_rb
(
  uint8 hrb_index,
  uint8 combo_type,
  ul_mapping_info_type_ *ul_map,
  dl_mapping_info_type_ *dl_map
)
{
  uint8 comboCounter =0,ignore_mapping = TRUE;

  switch(combo_type)
  {
    case  (UL_RACH_MAPPING|DL_MAC_EHS_MAPPING):
    case  (UL_RACH_MAPPING|DL_FACH_MAPPING):
    case  (INVALID_MAPPING|DL_FACH_MAPPING):
    case  (UL_RACH_MAPPING|INVALID_MAPPING):
    case  (UL_EDCH_MAPPING|DL_MAC_EHS_MAPPING):
     ignore_mapping = FALSE;
     break;
  }
     
    if(ignore_mapping == TRUE)
  {
    /*Ignore for these types*/
    WRRC_MSG3_MED("RRC_HRB: UL:%d \tDL:%d \tRB-ID %d \tignore for now",
                                            print_ul_type(combo_type),
                                            print_dl_type(combo_type),
                                            rrc_ordered_hanging_rb[hrb_index].rb_id);
    return SUCCESS;
  }

  for(;comboCounter<MAX_MAPPING_COMBO;comboCounter++)
  {
    if((rrc_ordered_hanging_rb[hrb_index].mapping_info[comboCounter].combo_type != 0)&&
       (rrc_ordered_hanging_rb[hrb_index].mapping_info[comboCounter].combo_type != combo_type))
      continue;

    /*MSG_5(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRC_HRB: UL:%d \tDL:%d \tRB-ID %d \thrb_index %d and combo_index %d",
               print_ul_type(combo_type),
               print_dl_type(combo_type),
               rrc_ordered_hanging_rb[hrb_index].rb_id,
               hrb_index,comboCounter);*/
    
    /*Strip the INVALID_MAPPING if present, happens in case of loopback mode*/
    if(combo_type & INVALID_MAPPING)
      combo_type &= ~INVALID_MAPPING; 

    rrc_ordered_hanging_rb[hrb_index].mapping_info[comboCounter].combo_type = combo_type;
    WCDMA_MEMCPY(&rrc_ordered_hanging_rb[hrb_index].mapping_info[comboCounter].ul_map, 
                 sizeof(ul_mapping_info_type_),
                 ul_map,
                 sizeof(ul_mapping_info_type_));
    WCDMA_MEMCPY(&rrc_ordered_hanging_rb[hrb_index].mapping_info[comboCounter].dl_map,
                 sizeof(dl_mapping_info_type_),
                 dl_map,
                 sizeof(dl_mapping_info_type_));
    return SUCCESS;
  }
  WRRC_MSG0_ERROR("RRC_HRB:Run out of space, should not happen !!!");
  return FAILURE;
}


/*====================================================================
FUNCTION: rrcllc_get_hanging_mapping_index()

DESCRIPTION:
Checks if the particular combo type exists for RB-ID given by index in hanging RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uint8 rrcllc_check_mapping_in_hanging_rb
(
  uint8 combo_type,
  uint8 hanging_rb_index
)
{

  uint8 mapping_cntr_combo =0;
  if((rrc_ordered_hanging_rb[hanging_rb_index].rb_id < DCCH_UM_RADIO_BEARER_ID) ||
     (rrc_ordered_hanging_rb[hanging_rb_index].rb_id >= MAX_RB))
  {
    WRRC_MSG1_HIGH("RRC_HRB: Wrong RB %d passed for mapping check, should not happen!!!",
                rrc_ordered_hanging_rb[hanging_rb_index].rb_id);
    return MAX_MAPPING_COMBO;
  }
  for(;mapping_cntr_combo < MAX_MAPPING_COMBO; mapping_cntr_combo++)
  {
    if(rrc_ordered_hanging_rb[hanging_rb_index].mapping_info[mapping_cntr_combo].combo_type != combo_type)
    {
      continue;
    }
    /*Check for queue and flow accociation */
    if(combo_type & DL_MAC_EHS_MAPPING)
    {
      if(rrcenhstate_check_for_queue_id(transition_config.toc_ptr,
                                        rrc_ordered_hanging_rb[hanging_rb_index].
                                          mapping_info[mapping_cntr_combo].dl_map.macehs_map_info.queue_id) == FALSE)
      {
        WRRC_MSG1_HIGH("EFACH: Queue not found in TOC for RB-ID %d",rrc_ordered_hanging_rb[hanging_rb_index].rb_id);
        return MAX_MAPPING_COMBO;
      }
    }
    if(combo_type & UL_EDCH_MAPPING)
    {
      if(rrchsrach_check_for_edch_flow_in_sib5(rrc_ordered_hanging_rb[hanging_rb_index].mapping_info[mapping_cntr_combo].ul_map.edch_map_info.mac_e_flow_id) == FALSE)
      {
        WRRC_MSG1_HIGH("HSRACH: Flow not found in SIB5 for for RB-ID %d",rrc_ordered_hanging_rb[hanging_rb_index].rb_id);
        return MAX_MAPPING_COMBO;
      }
    }
    return mapping_cntr_combo;
  }
  return MAX_MAPPING_COMBO;

}


/*====================================================================
FUNCTION: rrcllc_get_rb_index_in_hanging_rb()

DESCRIPTION:
Returns the rb_index in hanging RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uint8 rrcllc_get_rb_index_in_hanging_rb(rrc_RB_Identity rb_id)
{

  uint8 rb_index =0;

  for(rb_index = 0;rb_index < MAX_HANGING_RB_MAPPING_INFO ;rb_index++)
  {
    if(rrc_ordered_hanging_rb[rb_index].rb_id == rb_id)
    {
      break;
    }
  }
  return rb_index;
}




/*==========================================================
FUNCTION  :   rrcllc_find_or_allocate_index_hanging()

DESCRIPTION : 

DEPENDENCIES: None.

RETURN VALUE: index for the RB

SIDE EFFECTS: None
============================================================*/

uint8  rrcllc_find_or_allocate_index_hanging
(
  rrc_RB_Identity rb_id
)
{
  uint8 hrb_index = 0, first_free_index = MAX_HANGING_RB_MAPPING_INFO;
  for(;hrb_index < MAX_HANGING_RB_MAPPING_INFO;hrb_index++)
  {
    if(rrc_ordered_hanging_rb[hrb_index].rb_id == rb_id)
    {
      WRRC_MSG2_HIGH("RRC_HRB: Found entry for RB-ID %d at index %d",rb_id,hrb_index);
      return hrb_index;
    }
    else if((rrc_ordered_hanging_rb[hrb_index].rb_id == MAX_RB) &&
            (first_free_index ==MAX_HANGING_RB_MAPPING_INFO))
    {
      first_free_index = hrb_index;
    }
  }
  if(first_free_index != MAX_HANGING_RB_MAPPING_INFO)
  {
    WRRC_MSG2_MED("RRC_HRB: Allocating index %d for RB-ID %d",first_free_index,rb_id);
    return first_free_index;
  }
  else
  {
    WRRC_MSG0_HIGH("RRC_HRB: Run out of space in HRB!!!");
    return MAX_HANGING_RB_MAPPING_INFO;
  }
}

/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/


uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging
(
  void *ie_ptr,
  mapping_ie_release_version rel_ver,
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
)
{
  uint8 hrb_index = MAX_HANGING_RB_MAPPING_INFO;
  uecomdef_status_e_type status = FAILURE;
  
  hrb_index = rrcllc_find_or_allocate_index_hanging(rb_id);

  if(hrb_index == MAX_HANGING_RB_MAPPING_INFO)
  {
    return FAILURE;
  }

    rrc_ordered_hanging_rb[hrb_index].rb_id = rb_id;
    rrc_ordered_hanging_rb[hrb_index].logch_mode = logch_mode;
    switch(rel_ver)
    {
      case REL_99:
        status= rrcllc_backup_mapping_info_into_hanging_R99(ie_ptr,hrb_index);
        break;
      case REL_5:
        status= rrcllc_backup_mapping_info_into_hanging_R5(ie_ptr,hrb_index);
        break;
      case REL_6:
        status= rrcllc_backup_mapping_info_into_hanging_R6(ie_ptr,hrb_index);
        break;
      case REL_7:
        status= rrcllc_backup_mapping_info_into_hanging_R7(ie_ptr,hrb_index);
        break;
      case REL_8:
        status= rrcllc_backup_mapping_info_into_hanging_R8(ie_ptr,hrb_index);
        break;
      default:
        status = FAILURE;
        WRRC_MSG0_ERROR("RRC_HRB: Invalid Version Number Passed");
    };
    return status;

}



/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R99()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R99
(
  rrc_RB_MappingInfo         * ie_ptr,
  uint8 hrb_index
)
{
  uint8 rb_mux_cnt = 0;
  uint8 ul_mapping_type = 0,dl_mapping_type =0,lc_tc_cnt =0;
  rrc_DL_LogicalChannelMappingList *dl_list = NULL;
  ul_mapping_info_type_ ul_map ;
  dl_mapping_info_type_ dl_map ;
  memset(&ul_map,0,sizeof(ul_mapping_info_type_));
  memset(&dl_map,0,sizeof(dl_mapping_info_type_));

  /* Process the RB Mux option IE in the linked list */
  for(;(ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS);rb_mux_cnt++)
  {
    ul_mapping_type = 0;
    memset(&ul_map,0,sizeof(ul_mapping_info_type_));

    if(! RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,ul_LogicalChannelMappings))
    {
      ul_mapping_type = INVALID_MAPPING;
    }
    else 
    {
      ul_mapping_type |= get_ul_mapping_r99_hrb(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,&ul_map,hrb_index);
    }
    
    /*DL might have more than one logical chan info, so loop through it*/
    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,dl_LogicalChannelMappingList))
    {
      dl_list = &ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList;
      for(lc_tc_cnt =0 ;(dl_list->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH);lc_tc_cnt++)
      {
        dl_mapping_type =0;
        memset(&dl_map,0,sizeof(dl_mapping_info_type_));
        
        dl_mapping_type |= get_dl_mapping_r99_hrb(&dl_list->elem[lc_tc_cnt],&dl_map,hrb_index);
        rrcllc_store_mapping_in_hanging_rb(hrb_index,(dl_mapping_type|ul_mapping_type),&ul_map,&dl_map);
        MSG_MED("RRC_HRB: DL mapping info not provided !!!", 0,0,0);
      }
    }
  } /* end loop of RB Mapping info list */
  return SUCCESS;
}


/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R5()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R5
(
  rrc_RB_MappingInfo_r5  *ie_ptr,
  uint8 hrb_index
)
{
  uint8 rb_mux_cnt = 0;
  uint8 ul_mapping_type = 0,dl_mapping_type =0,lc_tc_cnt =0;
  rrc_DL_LogicalChannelMappingList_r5 *dl_list = NULL;
  ul_mapping_info_type_ ul_map ;
  dl_mapping_info_type_ dl_map ;
  memset(&ul_map,0,sizeof(ul_mapping_info_type_));
  memset(&dl_map,0,sizeof(dl_mapping_info_type_));

  /* Process the RB Mux option IE in the linked list */
  for(;(ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5);rb_mux_cnt++)
  {
    ul_mapping_type = 0;
    memset(&ul_map,0,sizeof(ul_mapping_info_type_));
    
    if(! RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,ul_LogicalChannelMappings))
    {
      ul_mapping_type = INVALID_MAPPING;
    }
    else
    {
      ul_mapping_type = get_ul_mapping_r99_hrb(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,&ul_map,hrb_index);
    }

    /*DL might have more than one logical chan info, so loop through it*/
    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,dl_LogicalChannelMappingList))
    {
      dl_list = &ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList;
      for(lc_tc_cnt =0 ;(dl_list->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH);lc_tc_cnt++)
      {
        dl_mapping_type =0;
        memset(&dl_map,0,sizeof(dl_mapping_info_type_));
        
        dl_mapping_type |= get_dl_mapping_r5_hrb(&dl_list->elem[lc_tc_cnt],&dl_map,hrb_index);
        rrcllc_store_mapping_in_hanging_rb(hrb_index,(dl_mapping_type|ul_mapping_type),&ul_map,&dl_map);
        MSG_MED("RRC_HRB: DL mapping info not provided !!!", 0,0,0);
      }
    }
  } /* end loop of RB Mapping info list */
  return SUCCESS;
}


/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R6()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R6
(
  rrc_RB_MappingInfo_r6  *ie_ptr,
  uint8 hrb_index
)
{
  uint8 rb_mux_cnt = 0;
  uint8 ul_mapping_type = 0,dl_mapping_type =0,lc_tc_cnt =0;
  rrc_DL_LogicalChannelMappingList_r5 *dl_list = NULL;
  ul_mapping_info_type_ ul_map ;
  dl_mapping_info_type_ dl_map ;
  memset(&ul_map,0,sizeof(ul_mapping_info_type_));
  memset(&dl_map,0,sizeof(dl_mapping_info_type_));

  /* Process the RB Mux option IE in the linked list */
  for(;(ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5);rb_mux_cnt++)
  {
    ul_mapping_type = 0;
    memset(&ul_map,0,sizeof(ul_mapping_info_type_));
    
    if(! RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,ul_LogicalChannelMappings))
    {
      ul_mapping_type = INVALID_MAPPING;
    }
    else
    {
      ul_mapping_type = get_ul_mapping_r6_hrb(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,&ul_map,hrb_index);
    }

    /*DL might have more than one logical chan info, so loop through it*/
    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,dl_LogicalChannelMappingList))
    {
      dl_list = &ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList;
      for(lc_tc_cnt =0 ;(dl_list->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH);lc_tc_cnt++)
      {
        dl_mapping_type =0;
        memset(&dl_map,0,sizeof(dl_mapping_info_type_));
        
        dl_mapping_type |= get_dl_mapping_r5_hrb(&dl_list->elem[lc_tc_cnt],&dl_map,hrb_index);
        rrcllc_store_mapping_in_hanging_rb(hrb_index,(dl_mapping_type|ul_mapping_type),&ul_map,&dl_map);
        MSG_MED("RRC_HRB: DL mapping info not provided !!!", 0,0,0);
      }
    }
  } /* end loop of RB Mapping info list */
  return SUCCESS;
}

/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R7()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R7
(
  rrc_RB_MappingInfo_r7  *ie_ptr,
  uint8 hrb_index
)
{
  uint8 rb_mux_cnt = 0;
  uint8 ul_mapping_type = 0,dl_mapping_type =0,lc_tc_cnt =0;
  rrc_DL_LogicalChannelMappingList_r7 *dl_list = NULL;
  ul_mapping_info_type_ ul_map ;
  dl_mapping_info_type_ dl_map ;
  memset(&ul_map,0,sizeof(ul_mapping_info_type_));
  memset(&dl_map,0,sizeof(dl_mapping_info_type_));

  /* Process the RB Mux option IE in the linked list */
  for(;(ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5);rb_mux_cnt++)
  {
    ul_mapping_type = 0;
    memset(&ul_map,0,sizeof(ul_mapping_info_type_));
    
    if(! RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,ul_LogicalChannelMappings))
    {
      ul_mapping_type = INVALID_MAPPING;
    }
    else
    {
      ul_mapping_type = get_ul_mapping_r6_hrb(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,&ul_map,hrb_index);
    }

    /*DL might have more than one logical chan info, so loop through it*/
    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,dl_LogicalChannelMappingList))
    {
      dl_list = &ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList;
      for(lc_tc_cnt =0 ;(dl_list->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH);lc_tc_cnt++)
      {
        dl_mapping_type =0;
        memset(&dl_map,0,sizeof(dl_mapping_info_type_));
        
        dl_mapping_type |= get_dl_mapping_r7_hrb(&dl_list->elem[lc_tc_cnt],&dl_map,hrb_index);
        rrcllc_store_mapping_in_hanging_rb(hrb_index,(dl_mapping_type|ul_mapping_type),&ul_map,&dl_map);
        MSG_MED("RRC_HRB: DL mapping info not provided !!!", 0,0,0);
      }
    }
  } /* end loop of RB Mapping info list */
  return SUCCESS;
}


/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R8()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R8
(
  rrc_RB_MappingInfo_r8  *ie_ptr,
  uint8 hrb_index
)
{
  uint8 rb_mux_cnt = 0;
  uint8 ul_mapping_type = 0,dl_mapping_type =0,lc_tc_cnt =0;
  rrc_DL_LogicalChannelMappingList_r7 *dl_list = NULL;
  ul_mapping_info_type_ ul_map ;
  dl_mapping_info_type_ dl_map ;
  memset(&ul_map,0,sizeof(ul_mapping_info_type_));
  memset(&dl_map,0,sizeof(dl_mapping_info_type_));

  /* Process the RB Mux option IE in the linked list */
  for(;(ie_ptr->n > rb_mux_cnt) && (rb_mux_cnt < MAX_RB_MUX_OPTIONS_REL5);rb_mux_cnt++)
  {
    ul_mapping_type = 0;
    memset(&ul_map,0,sizeof(ul_mapping_info_type_));
    
    if(! RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,ul_LogicalChannelMappings))
    {
      ul_mapping_type = INVALID_MAPPING;
    }
    else
    {
      ul_mapping_type = get_ul_mapping_r8_hrb(&ie_ptr->elem[rb_mux_cnt].ul_LogicalChannelMappings,&ul_map,hrb_index);
    }

    /*DL might have more than one logical chan info, so loop through it*/
    if(RRC_MSG_COMMON_BITMASK_IE(ie_ptr->elem[rb_mux_cnt],rrc_RB_MappingOption,dl_LogicalChannelMappingList))
    {
      dl_list = &ie_ptr->elem[rb_mux_cnt].dl_LogicalChannelMappingList;
      for(lc_tc_cnt =0 ;(dl_list->n > lc_tc_cnt) && (lc_tc_cnt < MAX_DED_LOGCHAN_PER_TRCH);lc_tc_cnt++)
      {
        dl_mapping_type =0;
        memset(&dl_map,0,sizeof(dl_mapping_info_type_));
        
        dl_mapping_type |= get_dl_mapping_r7_hrb(&dl_list->elem[lc_tc_cnt],&dl_map,hrb_index);
        rrcllc_store_mapping_in_hanging_rb(hrb_index,(dl_mapping_type|ul_mapping_type),&ul_map,&dl_map);
        
      }
    }
    else WRRC_MSG0_ERROR("RRC_HRB: DL mapping info not provided !!!");
  } /* end loop of RB Mapping info list */
  return SUCCESS;
}


/*==========================================================
FUNCTION  :   rrcllc_restore_from_hanging()

DESCRIPTION : This function restores the mapping for all RB's based on the combo.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              return SUCCESS

SIDE EFFECTS: None
============================================================*/

boolean rrcllc_restore_from_hanging
(
  ordered_config_type * config_ptr,
  uint8 combo
)
{
  uint8 rb_index=0,hrb_index=0,rb_id = RRC_INVALID_RB_ID;
  boolean ret_val = TRUE;

  /*Some initializations*/
  /*Do not call rrcllc_init_rach_rb_mapping_info(config_ptr) or rrcllc_init_fach_rb_mapping_info
      as it will reset the trch id read from SIBs*/
  if(combo & UL_RACH_MAPPING)
  {
    config_ptr->ul_cctrch.rach_info.ndlchan = 0;
    config_ptr->cell_fach_rb_mapping_info.ul_ndlchan = 0;
  }
  if(combo & DL_FACH_MAPPING)
  {
    config_ptr->cell_fach_rb_mapping_info.dl_ndlchan = 0;
  }
  else if(combo & DL_MAC_EHS_MAPPING)
  {
    rrcenhstate_init_efach_mapping(config_ptr);
  }
  
  for(rb_index=0;rb_index<MAX_HANGING_RB_MAPPING_INFO;rb_index++)
  {
    rb_id = rrc_ordered_hanging_rb[rb_index].rb_id;
    if(rb_id == MAX_RB)
      continue;
    for(hrb_index=0;hrb_index<MAX_MAPPING_COMBO;hrb_index++)
    {
      if(rrc_ordered_hanging_rb[rb_index].mapping_info[hrb_index].combo_type == combo)
      {
        switch(combo & 0xF0)
        {
          case UL_RACH_MAPPING:
            ret_val = rrcllc_restore_rach_mapping(config_ptr,rb_index,hrb_index);
            break;
          default:
            break;
        }
        switch(combo & 0xF)
        {
          case DL_MAC_EHS_MAPPING:
            ret_val = rrcllc_restore_macehs_mapping(config_ptr,rb_index,hrb_index);
            break;
          case DL_FACH_MAPPING:
            ret_val = rrcllc_restore_fach_rb_mapping(config_ptr,rb_index,hrb_index);
          default:
            break;
        }
      }
    }

    WRRC_MSG4_MED("RRC_HRB:%d and %d restore sucessful(TRUE/FALSE) %d for RB %d ",
            print_ul_type(combo),
            print_dl_type(combo),
            ret_val,
            rb_id);

  }
  return ret_val;
}


/*==========================================================
FUNCTION  :   rrcllc_remove_rb_from_hanging()

DESCRIPTION : This function removes the mapping forRB 

DEPENDENCIES: None.

RETURN VALUE: TRUE if found, else FALSE

SIDE EFFECTS: None
============================================================*/

boolean rrcllc_remove_rb_from_hanging
(
  rrc_RB_Identity rb_id
)
{
  int index =0;

  for(;index<MAX_HANGING_RB_MAPPING_INFO;index++)
  {
    if(rrc_ordered_hanging_rb[index].rb_id != rb_id)
      continue;

    rrc_ordered_hanging_rb[index].rb_id = MAX_RB;
    rrc_ordered_hanging_rb[index].logch_mode = UE_MODE_MAX_NUM;
    memset(&rrc_ordered_hanging_rb[index].mapping_info,0,MAX_MAPPING_COMBO*sizeof(mapping_info_type));
    return TRUE;
  }
  WRRC_MSG1_HIGH("RRC_HRB: RB-ID %d to be release not found !!",rb_id);
  return FALSE;
}





/*==========================================================
FUNCTION  :   rrcllc_init_hanging_rb()

DESCRIPTION : This function initializes the new hanging RB's 

DEPENDENCIES: None.

RETURN VALUE: None

SIDE EFFECTS: None
============================================================*/

void rrcllc_init_hanging_rb
(
  void
)
{
  uint8 rb_index=0;

 for(rb_index=0;rb_index<MAX_HANGING_RB_MAPPING_INFO;rb_index++)
  {
    rrc_ordered_hanging_rb[rb_index].rb_id = MAX_RB;
    rrc_ordered_hanging_rb[rb_index].logch_mode = UE_MODE_MAX_NUM;
    memset(&rrc_ordered_hanging_rb[rb_index].mapping_info,0,MAX_MAPPING_COMBO*sizeof(mapping_info_type));
  }

}



#endif
/*lint +e818 */
