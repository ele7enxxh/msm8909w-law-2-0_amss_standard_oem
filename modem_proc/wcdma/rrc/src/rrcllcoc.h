#ifndef RRCLLCOC_H
#define RRCLLCOC_H
/*===========================================================================

                    L3 - RRC Lower Layer Controller Ordered Config

DESCRIPTION
  This file contains data structures, variables, enums and function prototypes
  needed to be used within Lower Layer Controller and Ordered Config.


Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcllcoc.h_v   1.42   28 Jun 2002 18:13:32   vnagpal  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcllcoc.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/16   sp      Made changes to add call end metrics - FR#30856
05/09/16   nr      Made changes to remove irrelevant F3s
02/08/16   nr      CR973374: Made changes to not allow LTA if delay restriction IE is set
10/16/15   as      Added code to support LTA durting activation time
09/11/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
03/18/15   ad      Made changes to pick to ac to asc info from sib5 if missing in sib6.
04/14/15   sp      Made changes to indicate Multi rab status to L1
02/05/15   aa      Made changes to exclude the structure to store new CM list from the internal database of config pointer
10/11/14   sg      Made changes to add validations for 3C window size
10/01/14   sp      Made changes to set HSPA RNTI flag to false so that UE triggers CU instead of MR so that start value is sent to the network
07/23/14   sp      Made changes to remove F3s as part of FR19999
06/26/14   ad      Made changes to implement IE "E-DCH reconfiguration information same serving cell" in ASU
06/20/14   sa      Made changes to update UL 16QAM
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
05/26/14   db      Made changes to include the correct diverstiy value when the optiona IE 
                   'Primary PCCPCH info'is omitted
05/15/14   sp      Made changes to avoid accessing null pointer while accessing hsdpa_l1_info.
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/06/14   db      Made changes to handle SIB5 change during PCH to FACH transition and in common EFACH mode
18/04/14   ad      Made changes to adaptively calcualte T304 for GCF TCs
02/10/14   sg      Made changes for including revert_old_config flag in cphy setup req
02/04/14   sg      Made code changes for latest 3c interface 
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
02/13/14   ad      Made changes to remove redundant F3s
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
12/19/13   db      Made changes to configure lower layers properly and detect size re-establishment for SIB5 change
10/12/13   sa      Made changes for reseting ETFCI boost values
12/03/13   sg      Made changes to update review comments for 3C 
11/13/13   sa      Mainlined the FEATURE_MVS_UNDER_GHDI
11/07/13   db      Made changes to check for mapping and ERNTI in Connection setup for HSRACH
10/30/13   as      Made changes to add feature code for FEATURE_WCDMA_3C_HSDPA
10/21/13   vg      Made changed to Accept reconfiguration which asks UE to use (MACi/is, DCH) and reject reconfiguration only with (MACi/is, MAChs).
10/17/13   ad      Changes for DC HSDPA feature with 3C interface
09/16/13   mp      Added code for REL10 signalling under feature FEATURE_WCDMA_REL10
08/26/13   ad      Made changes to provide CTCH indicator on the correct FACH index to MAC
08/27/13   sp      Made changes to rename r9 function rrcllcpcie_process_dc_hsupa_params_r9
07/30/13   ad      Added code changes to support DB-DC feature
07/16/13   vi      Made changes to read GPS-cell id on HS serving cell if HS is active
07/08/13   ad      Made changes to remove redundant F3s
06/20/13   sa      Made changes for Default config in CELL_FACH
06/12/13   ad      Made changes to configure from OC when cell update confirm takes to 
                   PCH state and OC is set.This will remove the use of 
                   RRCLLC_CHAN_CFG_EPCH_HRNTI_ONLY cause.
06/13/13   db       Made changes to ignore mapping check for released RB's
05/21/13   db      Made changes to check for flexible size in OC for DCH to FACH scenario
05/22/13   sa      Added code changes to support FEATURE_WCDMA_DC_HSUPA
05/15/13   as      Made changes to pass whether PO3 is signalled or pre-set
05/08/13   db      Multiple mapping support for EFACH
04/25/13   sg      Added code for REL9 under FEATURE_WCDMA_REL9
04/24/13   ad       Made changes to configure hsrach dedicated mode during reselection.
                    Made changes not to reset hsrach actions in OC if OC is set when RRCLLC_CHAN_CFG_EPCH_HRNTI_ONLY
                    cause is used.
04/16/13   db      Made changes to set sttd indicator to FALSE when IE is not included in SIB5
01/25/13   db      Made changes to not configure BCCH FACH in connecting state
01/03/13   ad      Made changes to reduce the redundant F3s
11/28/12   db       Made changes to determine the hspa_rnti_stored_cell_pch flag based 
                    on if CRNTI is provided in the OTA message or not
10/26/12   db      EFACH Redesign changes
10/25/12   ad      Made changes to deactivate the CM patterns which are not configured
                   in the message which does a timing re-initialized HHO
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/03/12   mp      Made changes to configure lower layers properly when
                   SIB5 changes to support EFACH and vice-versa
09/28/12   mp      Made the changes to detected the duplicate RL entry
09/07/12   jhl     Removed function rrcllc_check_srb_mapped_to_trch, and moved it to
                   l1rrcif.h
09/02/12   sg      Made changes to provide an API for dl amr mode value.
08/29/12   sa      Made changed to detect missing rb mapping info in CELL_FACH state
08/22/12   ad       Made changes to handle HSCFG if N/W configures HS-DPCCH 
                    without CQI Meas FB info
08/17/12   sa    Made thec changes to MAX_HANGING_RB_MAPPING_INFO 13 for HS_FACH
08/14/12   as      Made changes to restore rlc size list for rach
07/20/12   ad      Made changes to do rlc-reestablishment during reselections.
08/13/12   md      Made changes to take the SCCPCH config from SIB5 for CTCH setup if not present in SIB6
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
07/11/12   ad      Changes for including the hsdpcch bitmask in 
                   cell_transition_req for a hsrach cell.Made changes 
                   to handle ERNTI correctly. Changes to avoid starting MAC
                   for a HSRACH to HSRACH SIB5 change.Increased the handing rb size.
11/07/12   ad       Made changes to handle ERNTI correctly. Changes to avoid starting MAC
                    for a HSRACH to HSRACH SIB5 change.Increased the handing rb size.
06/29/12   ad      Made changes for triggering CU if E-RNTI is not given.
                   Skipped the de-registration of WMs if HS-RACH is ON and RRC is 
                   triggering MR while coming out of PCH. Fixed bug to updated E-RNTI
                   in OC correctly. Added code to trigger CU if R99 or R5 CUCNF is received. 
                   Added code to keep L1 and MAC in sync wrt HS-RACH actions.
04/06/12   ad      Changes for HS-RACH feature
05/10/12   md      Added code changes to support FEATURE_MAC_I
04/04/12   as      Changes to remove released Rb-Id from mapping validation check
02/06/12   db      Made changes to reject the OTA when invalid NW reconfig from Mac-ehs to Mac-hs
02/17/12   db      Fixed Compiler errors and warnings
12/21/11   ad      Made changes to bring the mvs code outside of qchat feature
11/10/11   ad      Added changes to handle UL RLC size change from DCH to EDCH
12/14/11   md      Removed extern declaration for ghdi api funs and added NAS Api i.e gsvoiceif.h 
12/08/11   db      Made changes to handle EFACH/EPCH support change in SIB5
12/08/11   db      Made changes to delete BCCH/PCCH mac-ehs mapping while moving to DCH
12/08/11   db      Made changes to trigger cell update if H-RNTI is missing 
12/08/11   db      Made changes to not invalidate hrnti status during dch to fach transtion
11/20/11   db      Made changes to backup and restore dedicated queue info
                   if queue id is same as in common queue
10/31/11   db      Updated persistence value formula
09/14/11   ad      Added code for FEATURE CPC DTX HS SCCH REVERT HHO RECOVERY feature
09/08/11   as      Added E-DRX changes for R8
08/16/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.104.00
08/05/11   as      Made changes to backup fach and rach mapping for EFACH
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
20/07/11   as      Made changes to fill correct L1 params for EFACH-DRX
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
07/29/11   as      Made changes to reset rlc size list properly during unmap rb
06/21/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.100.50
06/20/11   as      Made changes to backup E-FACH mapping when UE camped on to Legacy cell
06/09/11   ad      Added changes to ignore the duplicate RB IDs in chan_config_req
                   when releasing RBs in RBR
05/24/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.98.50
05/18/11   ad      Added changes to include UM RBs also in channel config request 
                   from established rabs
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/18/11   as      Added code to support feature EFACH-DRX 
04/13/11   ad      Changes to address the rate change issue for voice call
                   Qchat media concurrency scenarios
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
03/14/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.93.50
03/09/11   su      Made changes to mainline code under FEATURE_WCDMA HSUPA 2MS TTI.
03/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.92.50
02/18/11   ad      Added changes for one sided UL RLC reestablishment
02/16/11   as      Fixed compilation error
02/15/11   kp      Added function to restore eFACH mapping. 
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/03/11   as      Made changes to do rnti validation for dch to fach/pch
                   transition when efach is enabled 
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/06/11   as      Made changes to delete commonn queue info while moving
                   to non EFACH cell or DCH state
01/12/11   rl      Migrated to VU_MODEM_WCDMA_RRC.01.88.50
12/30/10   prk     In e-fach mode PCCH and BCCH RBs will have same LC ID, to
                   differentiate made changes to pass rb-id as argument to function.
01/04/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.87.50
12/06/10   prk     Changed function prototype.
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/06/10   prk     Added function prototype.
11/22/10   prk     Chagned function prototype.
12/22/10   su      Made changes tofix compilation issue under FEATURE UMTS BMC.
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   prk     Changed function prototypes.
10/11/10   ss      ASN.1 migration to ASN1 9.3.0
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
10/04/10   prk     Merging latest e-fach fixes.
10/01/10   su      Fixed compiler warnings
09/23/10   kp      Added code to maintain status of Chan_config_req posted by CSP for sib5 change,
                   the status life time will be from where CSP post the cmd till LLC starts processing
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
09/17/10   su      Featurizing MVS related code
11/03/10   rl      Fixed compiler warnings
                   Fixed review comments
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/24/10   kp      Added code for feature CPC-DRX under feature flag FEATURE_WCDMA_CPC_DRX
05/07/10   prk     Fixed compiler warnings.
05/04/10   gkg     Made changes to initialize DL CTFC properly.
05/03/10   gkg     Made changes to fix compile errors when enabling FEATURE_WCDMA_FDPCH_STTD.
04/15/10   gkg     Made changes for processing MIMO Codebook Restriction.
04/07/10   gkg     ASN.1 migration to 8.a.0
04/01/10   gkg     Un-featurize function rrcllc_get_config_ptr_in_use().
03/18/10   gkg     Added #define definitions for DC-HSDPA validations.
03/17/10   gkg     Made changes to populate Secondary C-PICH Power Offset 
                   from R7 and R8 OTAs for MIMO UEs.
03/17/10   as      Resolved Errors due to ASN.1 8.9 Migration
03/15/10   as      Updated RRC Code to support ASN.1 8.9 version
03/15/10   gkg     Made changes to put changes for "M ultiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
02/01/10   dm      Made changes for returning DRX cycle lenth for multiple DRX support feature.
02/11/10   gkg     Made changes for Opt Timer for CSoHS.
12/24/09   gkg     Made changes so that Standalone DCHs are checked while performing
                   the validation for UL/DL TFCS against added/deleted UL/DL DCH TrCHs.
01/18/10   kp      Added proto type for rrcllcpcie_get_inact_thrshd_dtx_cycle2.
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/01/09   dm      Added code to send Schedule Period Offset to BMC as part of 
                   FEATURE_UMTS_BMC_ENH_SCHEDULE. Made changes to ignore CTCH configuration present in SIB6 
                   and use the info contained in SIB5 in connected mode.
10/21/09   gkg     Fixed the compile errors that popped-up when feature E-FDPCH is defined.
10/20/09   kp      Made changes as part of CMI to include comptask_api.h
10/12/09   kp      If N/W is providing AGCH info for non EUL serving RL then don’t 
                   indicate add/reconfig action to L1, since AGCH makes sense 
                   only for serving RL.
10/07/09   gkg/kp  Changes for REL8 under FEATURE_WCDMA_REL8 and CPC-DTX
09/03/09   ps      Made changes for EDPCCH power Boost under 
                   FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
09/03/09   ps      Made changes as part of CMI to include pdcp.h
                   instead of pdpcrrcif.h and comptask.h
07/30/09   gkg     Made changes to handle the corner scenario when L1 triggers OOS 
                   IND just after UE has recovered from OOS due to RLF/RLC unrecoverable 
                   error.
07/16/09   kp      Added code to change the enum member name CM_NONE to RRC_CM_NONE in 
                   rrcllc_cm_status_e_type as it was conflicting with MS VS 2005.NET 
                   which is needed for RRC UTF.
06/17/09   ps      Made changes to not send a CTCH drop/ADD if CTCH is not
                   supported in the cell
05/25/09   ps      Made changes for EDPCH interpolation support
05/11/09   ps      Made chahges to dynamically allocate memory for CC/OC/TOC
05/06/09   gkg     Made changes for ZI memory optimization for global prach_sel_info.
                   If feature FEATURE MODEM HEAP is defined and FEATURE_WCDMA_ZI_OPT_DISABLED
                   is not defined, then global prach_sel_info is initialized using modem heap,
                   otherwise it's initialized statically.
05/08/09   ss      Updated Copyright Information
05/04/09   ps      Made changes to update the TFRI table status for Non-R7 OTA 
                   also
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/08/09   gkg     In case of Cell_FACH->Cell_DCH transition through Reconfig OTA, 
                   if RB-Mapping for any RB is provided again, TOC will be not be 
                   updated for RB-Mapping.
03/24/09   ps      Made changes that once OTA provides RLC size, then it
                   can be changed only through OTA, only in case of R3 OTA
                   RLC size will be calculated from the TF of the Trch
03/09/09   ps      cleanup of feature flag 
                   FEATURE TFC RLC SIZE
                   FEATURE_DCH_FACH_STATE_TRANSITIONS
                   FEATURE TFC RLC SIZE
                   FEATURE_DCH_IE_IN_CELL_FACH
                   FEATURE_TFCSUBSET
                   FEATURE_ADJUST_TB_SIZE_WHEN_TRCH_INFO_NOT_PRESENT
03/06/09   gkg     Added prototype for rrcllc_reset_mac_and_l1_for_hsdpa().
02/24/09   ps      Made changes to move the declaration of 
                   rrcllc_check_srb_mapped_to_trch()  to l1rrcif.h
02/18/09   gkg     Defined a macro for invalid URA_ID to 0xFFFFFFFF.
02/12/09   gkg     Made changes towards optimizing PRACH working DB. 
                   Basically the data type of PRACH working DB is changed from 
                   ordered_config_type to prach_config_type.
02/09/09   ps      Made changes to inform l1 if the Trch has Srb mapped to it 
                   or not through rrcllc_check_srb_mapped_to_trch()    
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/18/08   ps      Made changes to send an add and drop for DPCH , if the tau DPCH
                   changes by 1 under FEATURE WCDMA REL7 HACK CHG TAU DPCH
12/16/08   gkg     To ensure that variable amr_cb_set has always valid values, made
                   changes to make it global variable.
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
24/10/08   gkg     Removed the signature of function rrc_turbo_coding_provided_in_ota().
10/10/08   gkg     Added code for updating UM RBs in OC first and then making them 
                   available to LLC to configure RLC while doing Channel Config Req. 
                   Also added code for Alt E Bit that is used for UM RBs.
10/10/08   ps      Made changes to support Delay restriction IE
09/22/08   gkg     Added the signature for function rrc_turbo_coding_provided_in_ota().
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/23/08   ps      Added changes for FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION 
                   which was missing in R6 path
06/19/08   ps      Made changes to store a RB mapping for EDCH which has no 
                   EDCH info yet in the Standalone RB Mapping Database 
                   instead of OC
06/16/08   ps      Corrected featurisation issues for FEATURE HSDPA
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state.   
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of OTA messages, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
05/07/08   ps      Added defination of RRC_EUL_MAX_RL
04/25/08   ps      Made changes to pass num_trch and num_tfci as 0 to L1
                   when num_dpdch in UL is 0
04/17/08   ps      Made changes to move get_srb_info_to_setup_preconfig_r5() out 
                   of feature FEATURE_DEFAULT_CONFIG_IN_RB_SETUP
04/14/08   gkg     Modified prototype of functions rrc_configure_amr_mode_param()
                   and rrc_change_code_configuration().
04/07/08   da      Fixed compilation warning
01/04/08   gkg     Modified the declaration of rrcllc_derive_dl_amr_mode().
20/03/08   gkg     Added declaration of function rrcllc_derive_dl_amr_mode().
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
02/04/08   kp      Added prototype for function which will will set all E-DCH active set DL RL's bit mask
                   to reconfig, This is currenlty used in TTI reconfiguration, where L1 needs to reconfigure
                   all DL RL's in E-DCH active set.
12/21/07   da      Support for FEATURE_MBMS
12/03/07   ps      Made changes to support Additional Transport format for CCCH
                   received in SIB5/SIB6.Changes are made under feature 
                   flag FEATURE_CCCH_IE_ENH.
26/11/07   gkg     Added support for VT->Voice fallback and vice versa
                   by RB-Reconfig msg for SCUDIF.Also added changes to 
                   support VT->Voice fallback using RB-Setup msg.
                   Changes under feature flag FEATURE_SCUDIF_ENHANCEMENT
11/14/07   rm      Added prototypes to support FEATURE TFCC ENHNC
10/30/07   ps      Added support for DC 14. Also added support to extract 
                   SRB5 contents received in Rel6 Handover to utran message.
10/12/07   vk      Added fn rrcllc_init_fdpch_info() to initialize F-DPCH parameters
                   under FEATURE_WCDMA_FDPCH
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
08/28/07   da      Added prototype of set_harq_information()
07/13/07   da      Added support for HSPA call status indication.
06/29/07   ps      Added code to reject a OTA message if the SRBs are not 
                   configured with any Trch configuration.
05/23/07   ps      Added support for "RB Info to Change" IE in RB-Setup msg.
                   Change are made under feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP.
05/08/07   sm      Added prototype of rrcllc_init_before_msg_processing()
04/19/07   da      Put TFCS check when TrCH is added/deleted under feature
                   FEATURE_TFCS_SANITY_CHECK.  Also made change to enforce
                   this rule when destination state is CELL_DCH and relaxed
                   the checkf for other states. 
04/10/07   ps      Added support for default configs sent through Rb-Setup msg
04/03/07   da      Prototype for rrcllcpcie_indicate_drop_and_add_of_e_dch_active_set_rl()
03/08/06   da      Prototype for rrc_is_cm_active_with_eul() and 
                   rrc_is_eul_active()
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/28/07   sgk     Added support for HSDPA ASU repointing with prototype for 
                   function rrcllc_process_serving_hsdsch_cell_info under 
                   FEATURE_HSDPA_ASU_REPOINTING. 
02/26/07   sm      Modified ENUM rrcllc_cm_status_e_type for rejection of EUL+CM config
                   for early R6 UEs
02/26/07   da      Defined Macro RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT and prototype
                   for rrcllc_validate_rb_mapping_info()
02/21/07   ps      Made changes to return a failure/succes status for all the functions
                   that restore the Standalone Rb-mapping info into the ordered config
02/12/07   sgk     Added prototype for function rrcllc_update_cm_params.
02/12/07   sgk     Added prototype for function rrcllc_update_cm_params.
02/09/07   ps      Made changes to re-initializes Re-establish entity either 
                   only in OC, or in CC only or both 
02/07/07   da      Added prototypes for rrcllc_init_tfcs_needed_flags() and
                   rrcllc_invalid_tfcs_info()
02/07/07   ps      Made changes to handle Standalone Rb-Mappings
02/02/07   da      Added prototype rrcllc_get_dch_fach_act_time_in_ms()
01/29/07   ps      Added changes to ignore timingMaintainedSynchInd IE when 
                   Frequency info IE is not present or in Timing Initialised case
01/22/07   da      Passing new boolean param to fn rrcllc_init_rach_info
                   as indication on whether to init rb-mapping info or not.
01/19/07   rm      Added support for SIB7 update in Cell FACH state.
                   The code is under feature flag FEATURE_UPDATE_SIB7_IN_FACH.
                   Update CC,OC & TOC with the updated Up link interference value from SIB-7
01/12/07   vr/sm   Added prototype of function rrcllc_timing_init_in_progress()
12/22/06   ps      Made changes to handle postVerificationPeriod and the
                           timingMaintainedSynchInd IE received in OTA messages
12/19/06   sm      Added prototype of function rrcllc_init_mac_ul_edch_info_ie()
11/15/06   sm      Added EUL HHO and reverting back to old config support
10/16/06   vk      Added prototype of fn rrcllc_init_reestablish_entity() under
                   FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/11/06   vk      Declared rlc_size_change_in_progress structure. Also
                   added prototype of function to compare RLC Size.
                   Changes have been featurized under FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/08/06   da      Defined RRCLLC_BLER_NA as -127
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/21/06   tkk     Added SRB#5 support.
08/02/06   tkk     Added new structure with fields needed for PRACH selection.
07/13/06   da      Added new flag is_hho as part of OC. Prototypes for func 
                   rrcllcpcie_is_hho and rrcllcpcie_is_hho_r5
06/05/06   sgk     Made changes to include rel5 non HSDPA functionality under 
                   FEATURE REL5 while retaining the HSDPA specifc processing 
                   under FEATURE HSDPA.
05/01/06   vk      Declared new functions to determine if beta table and measurement
                   feedback information has changed. Changes have ben featurized under 
                   FEATURE_HSDPA_PERFORMANCE_OPTIMZATION
04/25/06   da      Removed prototype of rrcllc_new_sccpch_selection_required
04/18/06   vk      Added prototype of new fn rrcllc_get_cm_hsdpa_nv_item()
04/14/06   vk      Added a new enum rrcllc_cm_status_e_type under FEATURE HSDPA
03/28/06   vk      Added the prototype of a new function rrcllc_is_cm_active()
                   that indicates if CM is active under FEATURE HSDPA
03/17/06   da      Added uint8 call_type_mask that indicates the types of 
                   active calls in ordered_config_type 
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
03/06/06   tkk     Added function prototype "rrcllc_update_oc_with_tfc_subset" 
                   for TFC subset handling through reconfiguration/interRAT 
                   messages.
02/24/06   tkk     Added prototype for "rrcllc_get_dl_amr_mode_for_rb" & 
                   "rrcllc_get_dl_amr_mode"
12/18/05   da      Added prototype for fn rrcllc_new_sccpch_selection_required()
12/06/05   vk      Added protype for fn rrcllc_copy_ul_tfcs_info_to_dl_tfcs()
11/18/05   da      Added prototype for rrcllc_dequeue_cmd()
11/10/05   tkk     Added prototype for "get_dl_info_for_each_rl_r5" to support
                   Rel-5 Interrat HO To UTRAN.
10/27/05   da      Defined RRCLLC_SCR_CODE_MULTIPLICATION_FACTOR as 16
09/26/05   ss      Added the prototypes get_mac_d_hfn_cucnf
                   and get_mac_d_hfn usedby procedures
09/02/05   da      Added prototype for following funcs: 1.rrcllc_get_dl_ded_rate() 
                   and 2. rrcllc_get_ul_ded_rate()
08/10/05   vk      Declared a new function rrcllc_set_r99_hsdpa_action() for 
                   setting HSDPA Action based on contents of Release 99 Message
06/14/05   kc      Added prototype to check and see if the received PRACH/SCCPCH info has atleast
                   one AICH/PICH info respectively
06/03/05   da      Added prototype for rrcllc_print_trans_chnl_rate()
05/26/05   vk      Added declaration for function rrcllc_allocate_dflow_index()
05/05/05   svk     Added declaration for function rrcllc_update_oc_in_fach_with_sibs().
04/29/05   da      Added prototype for func get_tfcs_and_fach_pch_info()
04/22/05   vk      Declared new fn rrcllc_copy_harq_info() to copy HARQ Information
                   Also declared new fn rrcllc_init_hspdsch_info() to initialize
                   HS-PDSCH Information under FEATURE HSDPA
04/13/05    sk      Changed prototype for func unmap_rb()
04/06/05    da      Added Initial support for BMC.  Handling of CTCH  and
                    a second SCCPCH in IDLE, CELL_PCH and URA_PCH states
                    are now supported.  CTCH is not present in CONNECTING, CELL_FACH
                    and CELL_DCH states.
03/22/05    da     Define RRCLLC_TAU_DPCH_MULTIPLICATION_FACTOR as 256
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
10/11/04    sk     Removed Lint errors
                   Added prototype for rrcllc_check_dcch_fach_mapping() and
                   rrcllc_update_oc_in_fach_with_toc_for_rl_failure()
                   Moved protypes for rrcllc_copy_rlc_parms() and
                   rrcllc_copy_cell_fach_rlc_size_list() to rrcllcoc.h file
08/19/04   sgk     Added prototype for function rrcllc_check_dcch_fach_mapping
                   defined in rrclcmsgie.c
                   Added the prototype for function rrcllc_cell_update_confirm_ie
                   defined in rrcllcmsgie.c ot f ix lint error in rrcllcoc.c
                   Added prototypes for rrcllc_print_mac_info and
                   rrcllc_print_l1_info
                   Added prototype for function
                   rrcllc_update_oc_in_fach_with_toc_for_rl_failure
                   Changed the return type of the functions
                   rrcllc_update_oc_from_pch_to_fach,
                   rrcllc_update_oc_from_fach_to_cell_pch_or_ura_pch,
                   rrcllc_update_oc_from_dch_to_pch_with_sibs,
                   rrcllc_update_oc_from_dch_to_fach_with_sibs
                   from rrcllc_oc_status_e_type to uecomdef_status_e_type
                   to correct lint errors 'bad type' in file rrcllc.c where
                   the functions were called.
                   Removed redundant declarations for the following functions
                   rrcllc_update_oc_from_dch_to_pch_with_msg
                   rrcllc_update_oc_from_dch_to_pch_with_sibs
                   rrcllc_update_oc_from_fach_to_cell_pch_or_ura_pch
                   rrcllc_update_oc_from_fach_to_pch_with_msg
                   rrcllc_update_oc_in_pch_with_toc
                   to correct lint errors
07/30/04   vn      Added a new function prototype for rrcllc_init_dl_common_info()
07/15/04   jh      Added prototypes for get_frequency_info() and
                   get_dl_info_per_rl().
06/25/04   ttl     Added lg_ch_cnt_up.
06/21/04   vk      Added prototype for rrcllc_initialize_cm_parms()
03/12/04   jh      Added prototype for remove_rlc_size_list_entry().
01/14/04   jh      Added utran_drx_cycle_length to ordered config.  Added
                   function prototypes for
                   rrcllc_update_oc_from_dch_to_pch_with_msg(),
                   rrcllc_update_oc_from_fach_to_cell_pch_or_ura_pch(),
                   rrcllc_update_oc_in_pch_with_toc(), and
                   rrcllc_chan_config_dch_to_pch().
02/03/04   ttl     Added a new field lg_ch_cnt_down in rrcllcoc_trch_info_type.
12/04/03   vn      Added a new field sdu_time in Ordered Config.
11/13/03   jh      Added flag reconfig_in_progress to indicate that a reconfig
                   message is being processed.  This is to support processing
                   ASU messages while a reconfig is in progress.
07/15/03   jh      Added parameter to get_dl_dpch_info_for_each_rl() and
                   get_dl_info_for_each_rl() to indicate whether the RL is a
                   new addition.
06/04/03   vn      Added fn prototype for verify_sccpch_exists().
04/29/03   vn      Consolidated all Ordered Config status/states into one
                   structure, added ordered config processing state to the
                   Ordered Config complete status structure to isolate race
                   conditions. Prototype for rrcllc_clear_ordered_config_now,
                   rrcllc_is_cmd_under_process_for_solo_bch_operation and
                   rrcllc_copy_bch_info_from_cc_to_oc.
03/26/03   vn      Added prototype for rrcllc_handover_to_utran_ie and
                   for rrcllc_get_timers_and_const_in_conn_mode_for_handover.
                   Wrapped all RLC size for DCH related data structures with
                   FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH
02/18/03   vn      Added storage for destination frequency and primary
                   scrambling code in Ordered Config. This is to support
                   DCH->FACH directed cell.
02/14/03   kc      Changed feature defn from INTERRAT_HANDOVER_GTOW_RRC to
                   INTERRAT_HANDOVER_GTOW
02/13/03   jh      Added prototype for rrcllc_update_oc_from_connecting_to_dch()
                   function.
11/22/02   jh      Changed prototype of seg_rl_add_info() to accomodate
                   reporting of a duplicate RL add.
11/15/02   jh      Changed get_ul_deleted_trch() and get_dl_deleted_trch()
                   function prototypes to accept the entire
                   UL/DL_DeletedTransChInfoList IE instead of just the TrCH
                   id.
10/04/02   vn      Added support for FACH to DCH state transitions:
                   new function prototypes :
                    rrcllc_copy_cell_fach_rb_mapping_info,
                    rrcllc_discard_oc and
                    and rrcllc_update_oc_from_fach_to_dch.
10/04/02   jh      Added data structures and function prototypes for RLC size
                   restriction.
                   New types:
                     rrcllcoc_rlc_size_list_type
                     rrcllcoc_logical_channel_list_type
                     rrcllcoc_rlc_size_restriction_info_type
                   New fields in ordered_config_type:
                     dch_rlc_size_restriction_info[UE_MAX_TRCH]
                     rach_rlc_size_restriction_info
                   New function prototypes:
                     rrcllc_populate_mac_rlc_size_restriction_info
                     rrcllc_init_dch_rlc_size_info
                     rrcllc_init_fach_rlc_size_info
09/24/02   vn      Support for DCH->FACH transitions: Added a new structure
                   cell_fach_rb_mapping_info for storing RB Mapping Info meant
                   for CELL_FACH in the ordered_config_type. Added
                   rrcllc_transition_config_type to store TOC data. Added
                   TRANSITION_CONFIG to rrcllc_config_e_type. Function prototypes
                   for new functions:
                   rrcllc_init_config_data, rrcllc_init_ordered_config_state_and_data,
                   rrcllc_init_ordered_config_mac_info,
                   rrcllc_update_oc_in_fach_with_msg,
                   rrcllc_update_oc_from_dch_to_fach_with_msg,
                   rrcllc_update_oc_from_dch_to_fach_with_sibs,
                   rrcllc_chan_config_dch_to_fach,
                   rrcllc_update_cell_fach_dl_mapping_info_with_log_chl_id,
                   rrcllc_update_cell_fach_ul_mapping_info_with_log_chl_id,
                   rrcllc_update_toc_dl_dch_info_with_log_chl_id,
                   rrcllc_update_toc_ul_dch_info_with_log_chl_id.
09/20/02   vn      Added support for cell reselection in Connecting & CELL_FACH.
                   Added prototype for
                   rrcllc_update_oc_for_serving_neighbor_bch_in_connected().
09/04/02   bu      Added prototype for rrcllc_register_for_reconfig_complete.
08/21/02   vk      Declared function rrcllc_send_compressed_mode_run_time_error_ind(...)
                   to send compressed mode run time error indication to Physical
                   Channel Reconfiguration Failure by posting RRC_CPHY_COMPRESSED_MODE_IND.
                   Also declared rrcllc_modify_cm_info_in_current_config(...) to
                   delete the tgpsi from current config on getting a compressed
                   mode run time error indication from L1. Changes are under
                   FEATURE CM SUPPORTED
08/05/02   vk      Declared function set_tgpsi_function(...) to indicate to
                   L1 if the compressed mode gap pattern has to be modified.
07/31/02   vk      Defined the compressed mode structure in ordered config
                   database. Declared 2 functions set_compressed_mode_info(..)
                   and find_tgpsi_index to process compressed mode parameters.
                   Changes have been put under FEATURE CM SUPPORTED
06/28/02   vn      get_dl_info_for_each_rl now returns a new enum to indicate
                   when the RL Info is Ignored. Added a new enum
                   rrcllcoc_dl_info_per_rl_status_e_type.
06/13/02   vn      Provided prototypes for new functions to clear UL and DL
                   TM and UM Info.
06/07/02   vn      Removed rrcllc_dl_trch_reconfig and rrcllc_dl_trch_reconfig2
                   as they are not being used anymore. Added the new function
                   prototypes for rrcllc_copy_dl_dch_trch_info_to_buffer,
                   rrcllc_copy_dl_dch_trch_info_from_buffer, rrcllc_copy_ul_dch_
                   trch_info_to_buffer, rrcllc_copy_ul_dch_trch_info_from_buffer
                   and rrcllc_delete_rlc_am_info.
06/03/02   jh      Added declaration for unmap_rb().
05/16/02   jh      Added support for reading Dynamic Persistence values from
                   SIB 7.
05/15/02   vn      Merged the following from cell reselection branch:
           vn      Added declaration for rrcllc_semi_permanent_data.
           vn      Added Neighbor BCH fields to rrcllc_dl_phy_chan_data_type
                   for cell reselection support. Added function prototype for
                   rrcllc_update_oc_for_resel_in_discon.
05/07/02   vn      Added removed_tfci_list and num_removed_tfcis to ordered_
                   config_type. Support for TFCS removal Information.
05/01/02   vn      Added function protype for rrcllc_copy_oc_to_cc.
04/23/02   vn      Added structure rrcllc_lower_layer_reconfig_need_type to
                   ordered_config_type to indicate need for reconfiguring
                   lower layers. Added prototype for rrcllc_init_reconfig_needed
04/22/02   vn      No separate common and dedicated TrCH info is maintained
                   anymore as either one or the other is valid. Removed ded
                   and com_trch_info, ded and com_tf_info and replaced with
                   dl_trch_info and dl_tf_info. New function prototypes that
                   got affected with this change.
04/22/02   vn      Removed rrcllc_semi_permanent_data from ordered_config_type
04/15/02   vn      Added function prototype for rrcllc_init_ordered_and_current
                    _config_ptrs. General cleanup and added function headers.
03/22/02   vn      Updates to MAC Header size macros. Updated prototypes of
                   functions used to determine RLC sizes.
03/11/02   vn      Added prototypes for rrcllc_init_activation_time and
                   rrcllc_copy_ccch_info_from_cc_to_oc.
03/08/02   vn      Updated prototype for rrcllc_init_fach_info to include new
                   parameter for indicating if RB Mapping info needs to be init
03/05/02   vn      Added new macro RRCLLC_DED_RACH_MAC_HEADER_SIZE for the case
                   when dedicated logical channels are mapped on RACH. Changed
                   names of other MAC Header macros for clarity. Updated proto-
                   -type for rrcllc_get_dl_um_dcch_rlc_size to have RRC state.
03/04/02   vn      Added Cell_FACH support and Cleanup/Restructuring.
                   New macros RRCLLC_DOWNLINK_TRCH_COUNT and RRCLLC_UPLINK_
                   TRCH_COUNT added to count number of TrCHs in DL and UL resp.
                   New enums rrcllc_bch_op_e_type, rrcllc_config_e_type defined
                   rrcllc_dl_ctfc_info_struct_type now has an array of CTFC
                   info data instead of array of pointers to CTFC data. rrcllc_
                   semi_permanent_data now moved to OC. data structs for TrCH
                   index and info, ordered_config_type now has new data structs
                   for FACH, DL-DCH, DL CTFC, CCTrCH and CTFC info, RM Restrict,
                   ded/common TrCH and TF info, semi static and gain factor parms
                   info, ASC info and RLC AM Config info. Pointer to ordered_
                   config_type now passed to all init functions. New fn
                   prototypes for set_dl_rm_restricted_info_to_invalid, fns to
                   init MAC UL-DL, L1 UL-DL and RLC pointers. New fn prototypes
                   for updating OC in disconnected, from discon to connecting,
                   from connecting to DCH/FACH, in DCH/FACH states.New fns to
                   copy SCCPCH info from SIB to OC in connected mode, copy BCH
                   info from CC to OC etc.
12/09/01   kc/rj   Added linked list head pointer as additional formal
                   parameter to functions get_srb_info_to_setup and
                   get_rb_info_to_setup for June conversion.
11/16/01   vn      Merged the following from branch -
                   Provided non-sequential transport channel handling: Added
                   new structure prototypes for storing DL/UL DCH TrCH info
                   relevant to L1, MAC and OC. Added prototypes of functions
                   to get, delete, allocate, deallocate and find TrCH indices
                   for UL/DL DCH. Also for filling holes in TrCH indices
                   allocation, copying TrCH data from one to another index
                   and to add, delete and sort TrCH indices for DL/UL DCHs.
11/07/01   vn      Defined Doff ASN1 max value and multiplication factor.
                   Defined default persistence scaling factor value. Added
                   function prototypes - get_persist_scaling_default_factors
                   and select_asc_from_ac. Defined default PRACH parameters
                   for Access Service Class 0.
11/03/01   vn      Defined min and max values for CN domain specific DRX cycle
                   length, AICH power offset, UL interference, TrCH IDs, and
                   renamed min and max values for UTRAN DRX cycle length.
09/25/01   kmp     Removed get_pich_pwr_offset() and get_drx_cycle() prototypes.
09/13/01   kmp     Changed prototype for get_rl_remove_info().
08/27/01   kmp     Replaced utran_drx_cycle_length_coef with
                   cs_drx_cycle_length and ps_drx_cycle_length in
                   ordered_config_type. Added rrcllc_check_ul_tfs_cnt() and
                   rrcllc_check_dl_tfs_cnt() prototypes.
07/17/01   kmp     Added prototype for rrcllc_set_pch_included(). Added
                   prototype for rrcllc_active_set_update_ie(). Added
                   prototypes for get_rl_remove_info(), get_rl_add_info(),
                   and rrcllc_init_dl_rl_info(). Updated
                   rrcllc_init_ordered_config() to now initilize the DL
                   Radio Link structures. Added RRCLLC_INVALID_CPICH_SCR_CODE,
                   RRCLLC_INVALID_SEC_SCR_CODE, & RRCLLC_INVALID_CHAN_CODE
                   macros. Added prototype for get_tx_diversity_mode_info().
                   Added macros RRCLLC_MIN_CPICH_SCR_CODE, and
                   RRCLLC_MAX_CPICH_SCR_CODE. Added prototype for
                   get_rb_info_to_reconfig(). Added prototypes for
                   rrcllc_rb_reconfig_ie(), rrcllc_phychan_reconfig_ie(),
                   and rrcllc_trch_reconfig_ie().
06/28/01   kmp     Change prototype of rrcllc_copy_sib_to_oc() and
                   rrcllc_init_rach_rb_mapping_info() to not take any
                   parameters.
06/19/01   kmp     Added 2 macros RRCLLC_MAX_AVAILABLE_SIGNATURE, and
                   RRCLLC_MAX_AVAILABLE_SUBCHAN.
06/13/01   kmp     Added prototypes for rrcllc_get_dl_um_rlc_size() and
                   rrcllc_get_dl_um_rlc_size(). These names were wrong during
                   an earlier checkin.
06/13/01   kmp     Modified rrcllc_init_dl_ded_tfs_table(),
                   rrcllc_init_dl_ded_tfs_table(), and rrcllc_init_ul_tfs_table(),
                   to take the TrCH Index as a parameter. Removed trch_idx from
                   rrcllcoc_trch_info_type structure. Moved ul_trch_cnt and
                   dl_trch_cnt into ordered config structure. Make the DL TF array
                   a 2 dimensional array indexed by TrCH Index, and TF index.
                   Updated prototype for get_sccpch_info().
06/06/01   kmp     Added rrcllc_init_dl_tfi_table(), rrcllc_init_ul_tfi_table(),
                   rrcllc_init_ul_tfs_table(), rrcllc_init_dl_com_tfs_table(),
                   and rrcllc_init_dl_ded_tfs_table() prototypes. Added an
                   extern uint16 num_removed_tfcis.
06/04/01   kmp     Added prototypes for get_rb_info_to_release(),
                   get_rb_info_affected(), get_dl_deleted_trch(),
                   get_ul_deleted_trch(). In rrcllc_copy_sib_to_oc() added
                   sib3 ptr as a parameter. This SIB contains the Max UL
                   Transmit data needed by the UL Ordered Config. Added
                   RRCLLC_UL_DPCCH_PWR_OFFSET_MIN & RRCLLC_UL_DPCCH_PWR_OFFSET_MAX
                   macros. Moved MAC and RLC header macros here from rrcllcoc.c.
                   Added rrcllc_ded_tf_info and changed rrcllc_tf_info to
                   rrcllc_com_tf_info.
05/29/01   kmp     Updated for new AM interface with RLC that was missed
                   earlier. Removed crnti and urnti elements from the
                   ordered_config structure and just use those values from
                   the mac_ul_config_type structure.
05/24/01   kmp     Updated Ordered Config for new RLC I/F.
05/24/01   vn      Removed declarations for pcch and ccch and added one for
                   rrcllc_set_fach_included().
05/23/01   kmp     Added rrcllc_get_dl_um_rlc_size() and rrcllc_get_ul_um_rlc_size()
                   function prototypes.
05/17/01   kmp     Changed RRCLLC_MAX_CONSTANT_VALUE from -10 to 10. Changed
                   RRCLLC_MAX_HI_RM from 2 to 256.
05/16/01   kmp     Changed prototypes of get_tfs_ded_rlc_size() and
                   get_tfs_com_rlc_size to return uint32 instead of uint16
05/14/01   kmp     Updated to remove prototypes to get_c_rnti() and
                   get_u_rnti().
05/09/01   kmp     Updated to add support for boundary checking constants, for
                   ie's rcv'd over the air. Added new prototypes for UL and DL
                   Transport channel IEs. Added rrcllc_dl_ctfc_info_struct_type
                   and rrcllc_rm_restrict_info_struct_type structures sued for
                   uplink TFCS calculations. Added counts for UL TM, UL UM,
                   DL TM, DL UM logical channels, and AM entities.
04/13/01   kmp     Updated to add constants from L1 DL Phycan Activity, and
                   new constants for error checking. Added additional function
                   prototypes.
04/04/01   kmp     Updated the prototype for rrcllc_copy_sib_to_oc().
04/03/01   kmp     Updated for the new RLC interfaces.
04/02/01   kmp     Updated to add uplink power data structure to Ordered
                   Config.
03/29/01   kmp     Initial Release.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "l1dlcctrchcfg.h"
#include "l1dlphychancfg.h"
#include "l1sapcommon.h"
#include "l1ulcfg.h"
#include "macrrcif_v.h"
#include "rlcrrcif_v.h"
#include "rrcllci.h"
#include "uecomdef.h"
#include "rrciho.h"
#include "rrccu.h"
#include "rrcrbe.h"
#include "rrcllc.h"
#include "rrccspi.h"
#ifdef FEATURE_UMTS_PDCP

#ifdef FEATURE_CMI
#include "pdcp.h"
#include "comptask_api.h"
#else/*FEATURE_CMI*/
#include "pdcprrcif.h"
#include "comptask.h"
#endif /*FEATURE_CMI*/

#endif /* FEATURE_UMTS_PDCP */

#ifdef FEATURE_MVS
#include "mvs.h"
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
#include "dldec.h"
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
#endif  /* FEATURE_MVS */
#include "modem_mem.h"

/*Added for ghdi api fun prototypes*/
#include "gsvoiceif.h"
/*===================================================================
                        CONSTANTS
=====================================================================*/
#define RRC_PS_MRAB    2

/* Not used currently */
#define L1_EUL_RECFG_GRANT_CONFIG_DL 0x10

#define RRC_INVALID_TPC_INDEX 0xFF
#ifdef FEATURE_MAC_I
  /*Invalid size for UL flexible size parameters*/
#define RRC_UL_INVALID_RLC_SIZE     0xFFFF
  /*Min and Max values when flexible size is configured*/
#define RRC_UL_MAX_FLEXIBLE_PDU_SIZE 12040
#define RRC_UL_MIN_FLEXIBLE_PDU_SIZE 16
#endif /* FEATURE_MAC_I */


/* This defines the Activation Time min and max and now values. */
/* Now is an implementation specific value. */
#define RRCLLC_ACTIVATION_TIME_NOW       0xFFFF
#define RRCLLC_MIN_ACTIVATION_TIME       0x0000
#define RRCLLC_MAX_ACTIVATION_TIME       0x00FF

/* This defines an that no valid C_RNTI has been received. */
/* Note that the UE must at least have been assigned a U_RNTI
   in the RRC Connection Establishment procedure. */
#define RRCLLC_NO_VALID_C_RNTI           0xFFFFFFFF

/* Serving RNC ID bit length */
#define RRCLLC_SRNC_ID_BIT_LENGTH        12

/* Serving RNTI bit length */
#define RRCLLC_SRNTI_BIT_LENGTH          20

/* This defines the UTRAN DRX Cylce Length min and max values. */
#define RRCLLC_UTRAN_MIN_DRX_CYCLE_LENGTH      3
#define RRCLLC_UTRAN_MAX_DRX_CYCLE_LENGTH      9

/* This defines the CN domain specific DRX cycle length min and max values */
#define RRCLLC_CN_DOMAIN_MIN_DRX_CYCLE_LENGTH   6
#define RRCLLC_CN_DOMAIN_MAX_DRX_CYCLE_LENGTH   9

/* This defines the Max Allowed UL TX Power min and max values. */
#define RRCLLC_MIN_ALLOW_UL_TX_PWR       (-50)
#define RRCLLC_MAX_ALLOW_UL_TX_PWR       (33)

/* This defines the PICH power offset min and max values. */
#define RRCLLC_MIN_PICH_PWR_OFFSET       (-10)
#define RRCLLC_MAX_PICH_PWR_OFFSET       (5)

/* This defines the AICH power offset min and max values. */
#define RRCLLC_MIN_AICH_PWR_OFFSET       (-22)
#define RRCLLC_MAX_AICH_PWR_OFFSET       (5)

/* This defines the DRX Cycle range min and max values. */
#define RRCLLC_MIN_DRX_CYCLE             3
#define RRCLLC_MAX_DRX_CYCLE             12

/* This defines the Puncturing Limit range min and max values
   multiplied by 100. */
#define RRCLLC_MIN_PUNCTURING_LIMIT      40
#define RRCLLC_MAX_PUNCTURING_LIMIT      100

/* This defines the Max available signiture bit string size */
#define RRCLLC_MAX_AVAILABLE_SIGNATURE   16

/* This defines the Max available sub-channel # bit string size */
#define RRCLLC_MAX_AVAILABLE_SUBCHAN     12

/* This defines the ASC range min and max values */
#define RRCLLC_MIN_AC_TO_ASC_MAPPING     0
#define RRCLLC_MAX_AC_TO_ASC_MAPPING     7

/* This defines the Primary CPICH Tx Power range min and max values */
#define RRCLLC_MIN_PRI_CPICH_TX_PWR      (-10)
#define RRCLLC_MAX_PRI_CPICH_TX_PWR      (50)

/* This defines the UL Interference min and max values */
#define RRCLLC_MIN_UL_INTERFERENCE    (-110)
#define RRCLLC_MAX_UL_INTERFERENCE    (-70)

/* This defines the CONSTANT_VALUE range min and max values */
#define RRCLLC_MIN_CONSTANT_VALUE        (-35)
#define RRCLLC_MAX_CONSTANT_VALUE        (10)

/* This defines the Power Ramp Step range min and max values */
#define RRCLLC_MIN_PWR_RAMP_STEP         1
#define RRCLLC_MAX_PWR_RAMP_STEP         8

/* This defines the minimum and maximum values for Transport Channel IDs */
#define RRCLLC_TRCH_ID_MIN_VALUE    1
#define RRCLLC_TRCH_ID_MAX_VALUE    32

/* This defines the Preamble Retransmission Maximum range min
   and max values */
#define RRCLLC_MIN_PREAMBLE_RETRANS_MAX  1
#define RRCLLC_MAX_PREAMBLE_RETRANS_MAX  64

/* This defines the Maximum number of Preamble cycles range min
   and max values */
#define RRCLLC_MIN_MMAX                  1
#define RRCLLC_MAX_MMAX                  32

/* This defines the Lower Bound for random backoff range min
   and max values */
#define RRCLLC_MIN_NB01                  0
#define RRCLLC_MAX_NB01                  50

/* This defines the channelization code range min and max values */
#define RRCLLC_MIN_CHANNELIZATION_CODE   0
#define RRCLLC_MAX_CHANNELIZATION_CODE   255

/* This defines the ul scrambling code range min and max values */
#define RRCLLC_MIN_UL_SCR_CODE           0
#define RRCLLC_MAX_UL_SCR_CODE           16777215

/* Minimun number of DPDCH allowed */
#define RRCLLC_MIN_NUM_UL_DPDCH_ALLOWED  2
#define RRCLLC_DEFAULT_NUM_DPDCH_ALLOWED 1

#define RRCLLC_EUL_MIN_NUM_UL_DPDCH_ALLOWED 0

#define RRCLLC_UL_DPCCH_PWR_OFFSET_MIN   (-82)
#define RRCLLC_UL_DPCCH_PWR_OFFSET_MAX   (-3)

#ifdef FEATURE_WCDMA_DC_HSUPA
#define RRCLLC_SEC_UL_DPCCH_PWR_OFFSET_MIN   (0)
#define RRCLLC_SEC_UL_DPCCH_PWR_OFFSET_MAX   (7)

#endif

/* This defines the minimum and maximum rate matching attributes */
#define RRCLLC_MIN_HI_RM                 1
#define RRCLLC_MAX_HI_RM                 256

/*
* This defines the BLER range min and max values Value is (x10)
* Example: MIN value is -6.3; therefore, MIN value is -6.3*10 = -63
* This is per a note in ASN.1 source
*/
#define RRCLLC_MIN_BLER                  (-63)
#define RRCLLC_MAX_BLER                  (0)
#define RRCLLC_BLER_NA                   (-127)

/* This define indicates an invalid Number of Transport Blocks */
#define  RRCLLC_INVALID_NUM_TB           0xFF

/* This define indicates when there is an invalid UL/DL TrCH Index */
#define  RRCLLC_INVALID_TRCH_IDX         0xFF

/* This define indicates an invalid SCCPCH index condition */
#define  RRCLLC_INVALID_SCCPCH_IDX       0x00

/* This defines the maximum preamble scrambling code */
#define RRCLLC_MAX_PREAMBLE_SCRAMBLING_CODE 15

/* Defines an invalid window size */
#define RRCLLC_INVALID_WINDOW_SIZE       0xFFFF

/* Defines an invalid Discard value for all RLC discard types */
#define RRCLLC_INVALID_RLC_DISCARD       0xFFFF

/* This defines the minimum and maximum rate matching attributes */
#define RRCLLC_MIN_CPICH_SCR_CODE        0
#define RRCLLC_MAX_CPICH_SCR_CODE        511

/* Defines an invalid Primary Scrambling code for Downlink. */
#define RRCLLC_INVALID_CPICH_SCR_CODE    0xFFFF

/* Defines an invalid Channelization code for Downlink. */
#define RRCLLC_INVALID_CHAN_CODE         0xFFFF

/* Defines an invalid Channelization code for Downlink. */
#define RRCLLC_INVALID_SEC_SCR_CODE      0xFFFF

/* CQI & Chan Code and Power Offset values */
#define RRCLLC_MAX_SCCH_CHAN_CODE   127
#define RRCLLC_CQI_MIN_REP_FACTOR   1
#define RRCLLC_CQI_MAX_REP_FACTOR   4
#define RRCLLC_DELTA_CQI_MAX   8
#define RRCLLC_MIN_MEAS_POWER_OFFSET   -12
#define RRCLLC_MAX_MEAS_POWER_OFFSET   26
#define RRCLLC_DELTA_ACK_MAX   8
#define RRCLLC_DELTA_NACK_MAX   8
#define RRCLLC_DELTA_ACK_NACK_REPITITION_FACTOR_MAX   4
#define RRCLLC_DELTA_ACK_NACK_REPITITION_FACTOR_MIN   1

/* For 2 MS, the default value is 50 TTI ms, so it is 50*2 = 100 ms*/
#define  RRCEUL_NO_GRANT_PERIODICITY_DEFAULT_VALUE_2MS 100

#ifdef FEATURE_WCDMA_MIMO
#define CPHY_MIMO_ACTION_PRESENT(mask) \
( ((mask) & (L1_MIMO_START )) ||\
  ((mask) & (L1_MIMO_RECFG ))||\
  ((mask) & (L1_MIMO_STOP ))\
)
#endif

#ifdef FEATURE_WCDMA_DC_HSDPA

#define MAX_HS_SCCH_CODES_WITH_DUAL_CELL 6
#define RRCLLC_INVALID_MEAS_PWR_OFFSET 0xFF
#define RRCLLC_INVALID_DL_FREQ 0xFFFF

#define RRC_MAX_HARQ_PROC_FOR_DC_HSDPA 8
#define RRC_MIN_HARQ_PROC_FOR_DC_HSDPA 6
#ifdef FEATURE_WCDMA_3C_HSDPA
#define RRC_DEFAULT_3C_HSDPA_CATEGORY 29
#define MAX_HS_SCCH_CODES_WITH_3_CELL 9
#endif
#ifdef FEATURE_WCDMA_DB_DC_HSDPA
/*Macros to add validation for DC HSDPA validation*/
#define HS_DC_FREQ_KHZ_MIN_DIFF   3800
#define HS_DC_FREQ_KHZ_MAX_DIFF   5200
#endif

#endif /*FEATURE_WCDMA_DC_HSDPA*/



#define RRCLLC_MAX_E_MAC_D_FLOW_PWR_OFFSET 6
#define RRCLLC_MAX_E_MAC_D_FLOW_NUM_RETX   15
#define RRCLLC_MAX_NUM_OF_RLC_PDU_SIZE_PER_LOGCHAN 32

/*
* The following define the MAC Header sizes.
*/
/* Common logical channel mapped on a RACH */
#define RRCLLC_COM_LOG_CHL_RACH_MAC_HDR_SIZE               2
/* Multiple dedicated logical channels on a RACH */
#define RRCLLC_DED_MULTI_LOG_CHL_RACH_MAC_HDR_SIZE         24
/* Single dedicated logical channels on a RACH */
#define RRCLLC_DED_SINGLE_LOG_CHL_RACH_MAC_HDR_SIZE         20

/* Common logical channel on a FACH */
#define RRCLLC_COM_LOG_CHL_FACH_MAC_HDR_SIZE               8U

/* Multiple Dedicated logical channels on a FACH with CRNTI */
#define RRCLLC_DED_MULTI_LOG_CHL_FACH_CRNTI_MAC_HDR_SIZE   24
/* Single Dedicated logical channel on a FACH with CRNTI */
#define RRCLLC_DED_SINGLE_LOG_CHL_FACH_CRNTI_MAC_HDR_SIZE  20

/* Multiple dedicated logical channels on a FACH with URNTI */
#define RRCLLC_DED_MULTI_LOG_CHL_FACH_URNTI_MAC_HDR_SIZE   40
/* Single dedicated logical channels on a FACH with URNTI */
#define RRCLLC_DED_SINGLE_LOG_CHL_FACH_URNTI_MAC_HDR_SIZE  40

/* Multiple dedicated logical channels on a DL DCH */
#define RRCLLC_DED_MULTI_LOG_CHL_DL_DCH_MAC_HDR_SIZE         4
/* Single dedicated logical channel on a DL DCH */
#define RRCLLC_DED_SINGLE_LOG_CHL_DL_DCH_MAC_HDR_SIZE        0

/* Default values as given in the spec */
/* All persistence scaling factor values in RRC as actual value * 10.
 * So a default value of 1 becomes 10.
 */
#define RRCLLC_DEFAULT_PERSISTENCE_SCALING_FACTOR_VAL 10

/* Defines multiplication factor for Persistence values */
#define RRCLLC_PVAL_MULTIPLICATION_FACTOR   0xFFFF

/* Defines multiplication factor for default DPCH Offset value */
#define RRCLLC_DOFF_MULTIPLICATION_FACTOR   512

/* Defines multiplication factor for TAU DPCH */
#define RRCLLC_TAU_DPCH_MULTIPLICATION_FACTOR   256

/* Defines the scr code multi factor (L1 representation) */
#define RRCLLC_SCR_CODE_MULTIPLICATION_FACTOR     16

/* Max Doff value in ASN1 */
#define RRCLLC_ASN1_DOFF_MAX_VAL  599

/* Default value for ASC0 PRACH Signature End Index */
#define  RRCLLC_ASC0_DEFAULT_SIGNATURE_END_INDEX    15
/* Default value for ASC0 PRACH Signature Start Index */
#define  RRCLLC_ASC0_DEFAULT_SIGNATURE_START_INDEX   0
/* Default value for ASC0 Assigned subchannel mask */
#define RRCLLC_ASC0_DEFAULT_ASSIGNED_SUBCHAN_MASK    0x0f

#ifdef FEATURE_UMTS_PDCP
/* definition of default values for IEs related to RFC 2507 */

#define RFC_2507_F_MAX_PERIOD     256
#define RFC_2507_F_MAX_TIME       5
#define RFC_2507_MAX_HEADER       168
#define RFC_2507_TCP_SPACE        15
#define RFC_2507_NON_TCP_SPACE    15

/* definition of default values for IEs related to RFC 3095 [ROHC] */

#define RFC_3095_MAX_CID          15
#define RFC_3095_REVERSE_DECOMPRESSION_DEPTH   0
#define RFC_3095_MAX_ROHC_PROFILES    3
#define RFC_3095_MAX_PCKT_SIZE_LIST      3
#endif /* FEATURE_UMTS_PDCP*/

#define RRCLLC_MAC_LOG_CHAN_ID_NOT_PRESENT 0
/* MAX EDCH RL in Active set allowed */
#define RRC_EUL_MAX_RL 4
/* This macro computes the totalk number of Downlink transport channels
 * active in the configuration pointed to by the passed pointer. The
 * number is the sum of number of BCHs, PCHs, FACHs and DCHs.
 * Note: Number of DCHs in any state other than Cell_DCH should be 0
 *       Number of FACHs in any state other than Cell_FACH and Connecting
 *       should be 0.
 */
#ifdef FEATURE_WCDMA_HS_FACH
/*Introduced to check if the common mac-ehs queue in sib5
  is same as that in OTA*/
#define UE_MAX_COMMON_HS_QUEUE 2
#endif
#define  RRCLLC_DOWNLINK_TRCH_COUNT(config_ptr)  \
         (                                        \
            config_ptr->mac_dl_parms.num_bchs +   \
            config_ptr->mac_dl_parms.num_pchs +   \
            config_ptr->mac_dl_parms.num_fachs +  \
            config_ptr->mac_dl_parms.num_dchs     \
         )
/* This macro computes the total number of Uplink Transport Channels
 * active in the Configuration, a pointer to which is passed. If the
 * CCTrCH type is invalid, number of transport channels is 0, for RACH
 * it is fixed to 1 (only 1 RACH supported) and for DCH, the number is the
 * total number of DCHs present in MAC Uplink parameter structure
 */
#define  RRCLLC_UPLINK_TRCH_COUNT(config_ptr)     \
         (                                        \
            (config_ptr->mac_ul_parms.cctrch_ptr->cctrch_type           \
               == INVALID_UL_CCTRCH) ? 0 :        \
              (                                   \
               (config_ptr->mac_ul_parms.cctrch_ptr->cctrch_type == RACH) ? 1 : \
                (                                 \
                  config_ptr->mac_ul_parms.cctrch_ptr->dch_info.ndchs     \
                )                                 \
              )                                   \
         )

#define RRCLLC_INVALID_RB_ID                  0xFF

/* Bitmask to indicate if the Freqency info is present in OTA message */
#define RRCLLC_FREQ_INFO_PRESENT_IN_OTA 0x01
/* Bitmask to indicate if the IE "Timing Indication" is set to "Maintained". */
#define RRCLLC_TIMING_MAINTAIN_IND_PRESENT_OTA 0x10

#define RRCLLC_SIB6_ADD_PRACH_TF_EXT_PRESENT(sib6_ptr)  \
    ( \
    (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6 ) && \
    (sib6_ptr->m.v4b0NonCriticalExtensionsPresent) && \
    (sib6_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && \
    (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650nonCriticalExtensionsPresent) &&  \
    (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.m.v690nonCriticalExtensionsPresent)\
    && (sib6_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650nonCriticalExtensions.v690nonCriticalExtensions. \
      sysInfoType6_v690ext.m.additionalPRACH_TF_and_TFCS_CCCH_ListPresent))


#define RRCLLC_SIB5_ADD_PRACH_TF_EXT_PRESENT(sib5_ptr)  \
	(  \
	(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6 ) && \
	( sib5_ptr->m.v4b0NonCriticalExtensionsPresent) &&  \
	(sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && \
	(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent )&& \
	(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) && \
	(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) && \
	(sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions. \
           v680NonCriticalExtensions.v690NonCriticalExtensions.sysInfoType5_v690ext.m.additionalPRACH_TF_and_TFCS_CCCH_ListPresent))     


#define INVALID_TB_SIZE 0xFFFF



#define RRCLLC_SIB5_DRX_LEVEL1_INFO_EXT_PRESENT(sib5_ptr)  \
	(  \
	(rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL6 ) && \
	( sib5_ptr->m.v4b0NonCriticalExtensionsPresent ) &&  \
      (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && \
       (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent)  && \
      (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions. \
       m.v680NonCriticalExtensionsPresent) && \
       (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions. \
       v680NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) && \
       (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.  \
           v680NonCriticalExtensions.v690NonCriticalExtensions.sysInfoType5_v690ext.m.cBS_DRX_Level1Information_extensionPresent))

#define UE_MAX_DTX_DRX_OFFSET 159

#define RRC_DL_INVALID_CTFC_VALUE 0xFFFF
#ifdef FEATURE_WCDMA_DC_HSDPA
#define RRC_DEFAULT_DC_HSDPA_CATEGORY 24
#endif
/*-------------------------------------------------------------------
ENUM: rrcllc_dl_ctfc_entry_state_enum_type

TFC restriction state enum type. This is based on the RM restriction
information.
--------------------------------------------------------------------*/
typedef enum
{
  INVALID, /* available to be used */
  ALLOWED, /* Allowed and should be accounted for rate matching */
  RESTRICTED /* Some TFI in this TFC is not allowed. Hence should
                should not be used in RM calculation */
} rrcllc_dl_ctfc_entry_state_enum_type;

typedef enum
{
  ORDERED_CONFIG,
  CURRENT_CONFIG,
  TRANSITION_CONFIG
} rrcllc_config_e_type;


typedef enum
{
  RRCLLC_ADD_BCH,
  RRCLLC_DROP_BCH,
  RRCLLC_NO_OP_BCH
} rrcllc_bch_op_e_type;

typedef enum
{
  RRCLLC_ADD_NBCH,
  RRCLLC_DROP_NBCH,
  RRCLLC_NO_OP_NBCH
}rrcllc_nbch_op_e_type;

typedef enum
{
  RRCLLC_ADD_PCH,
  RRCLLC_RECONFIG_PCH,
  RRCLLC_NO_OP_PCH
} rrcllc_pch_op_e_type;

typedef enum
{
  RRCLLC_ADD_CCCH,
  RRCLLC_RECONFIG_CCCH,
  RRCLLC_NO_OP_CCCH
} rrcllc_ccch_op_e_type;

/* This enumerates the possible status values that
 * processing of Downlink Info per RL can assume
 */
typedef enum
{
  RRCLLCOC_DL_INFO_PER_RL_SUCCESS,
  RRCLLCOC_DL_INFO_PER_RL_FAILURE,
  RRCLLCOC_DL_INFO_PER_RL_IGNORED
}rrcllcoc_dl_info_per_rl_status_e_type;

typedef enum
{
  RRCLLC_NO_CTCH, /* no CTCH, one SCCPCH with paging */
  RRCLLC_CTCH_WITH_PCCH, /*  Only one SCCPCH with CTCH and PCCH*/
  RRCLLC_CTCH_ON_DIF_SCCPCH /*CTCH is on different SCCPCH */
} rrcllc_ctch_status_e_type;

typedef enum
{
  RRC_CM_NONE,
  DEACTIVATE_CM_1, /* Msg is trying to deactivate one CM  pattern */
  DEACTIVATE_CM_2, /* Msg is trying to deactivate two CM pattern */
  DEACTIVATE_CM_3, /* Msg is trying to deactivate three CM pattern */
  DEACTIVATE_CM_4, /* Msg is trying to deactivate four CM pattern */
  DEACTIVATE_CM_5, /* Msg is trying to deactivate five CM  pattern */
  DEACTIVATE_CM_6, /* Msg is trying to deactivate six CM pattern */
  DEACTIVATE_CM_7, /* Msg is trying to deactivate seven CM pattern */
  DEACTIVATE_CM_8, /* Msg is trying to deactivate eight CM pattern */
  ACTIVATE_CM  /* Msg is trying to activate CM for at least one pattern */
} rrcllc_cm_status_e_type;


#define RRC_INVALID_URA_ID   0xFFFFFFFF

#ifdef FEATURE_WCDMA_MIMO
#ifdef FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT
extern int8 rrc_mimo_s_cpich_po;
#endif /*FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT*/
#endif /*FEATURE_WCDMA_MIMO*/

extern rrcllc_rrc_cmd_process_data_type  rrcllc_cmd_under_process;

/*===================================================================
                        DATA STRUCTURES
====================================================================*/
/*___________________________________________________________________
STRUCTURE: rrcllcoc_pdcp_param_type

This stucture contains PDCP related info needed by the PDCP layer for 
a particular RB.
--------------------------------------------------------------------*/

#ifdef FEATURE_UMTS_PDCP
#define RRC_INVALID_MAX_CS_DELAY 0xFF
typedef enum
{
  CPDCP_SETUP,
  CPDCP_RECONFIG,
  CPDCP_RELEASE
} rrcllcoc_pdcp_action_etype;

typedef struct
{
  rrc_RB_Identity rb_id;
  boolean pdcp_info_changed;
  boolean in_use;
  pdcp_cfg_req_data_type pdcp_cfg_req;
}rrcllcoc_pdcp_config_req_type;

typedef struct
{
  uint32 num_rbs;
  rrcllcoc_pdcp_config_req_type pdcp_config_req[MAX_RAB_TO_SETUP];
  //pdcp_sn_info_type pdcp_sn_info;
  /*Since only one PDCP entity will have AMR Rate associated with it. So keeping
  ul_amr_rate variable outside so that it's easier to retrieve.*/
}rrcllcoc_pdcp_parms_type;

#endif /* FEATURE_UMTS_PDCP */


/*-------------------------------------------------------------------
STRUCTURE: rrcllcoc_l1_ul_init_pwr_type

This structure defines the parameters needed by L1 to calculate
initial openloop uplink transmit power.
--------------------------------------------------------------------*/
typedef struct
{
  /* Power info, from SIB 5 */
  int primary_cpich_tx_power;
  int constant_value;

  /* Uplink Interference info, from SIB 7 */
  int ul_interference;

#ifdef FEATURE_WCDMA_HS_RACH
    /*Primary CPICH TX power 10.3.6.61.Default value is the value of "Primary CPICH TX power" in the first occurrence in list "PRACH system information list" in SIB5 or SIB5bis.*/
    int8 cpich_tx_pwr;

    /*Constant value 10.3.6.11..Default value is the value of "Constant value" in the first occurrence in list "PRACH system information list" in SIB5 or SIB5bis*/
    int8 const_val;
	
      /*UL_interference 10.3.6.87.*/
    int8 ul_interference_commonedch;
#endif
} rrcllcoc_l1_ul_init_pwr_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllcoc_trch_info_type

This structure is used to store Transport Channel information.
--------------------------------------------------------------------*/
typedef struct
{
  /* The Transport Channel(TrCH) ID */
  tr_ch_id_type               trch_id;

  /* Number of Transport Formats(TF) supported */
  uint32                      tf_cnt;

  /* Number of Logical Channels supported */
  uint32                      lg_ch_cnt;

  /* Set to TRUE when a DCH is mapped to a TrCH */
  boolean                     dch_present;

  /* TRUE when the logical channel mapped to this transport channel
     dropped from 2(multiple) to 1. */
  boolean                     lg_ch_cnt_down;

  /* TRUE when the lc mapped to this TrCH increased from 1 to 2. */
  boolean                     lg_ch_cnt_up;

} rrcllcoc_trch_info_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_dl_ctfc_entry_info_struct_type

This strcuture is for each TFC information
--------------------------------------------------------------------*/
typedef struct
{
  /* CTFC entry state valid/allowed/restricted */
  rrcllc_dl_ctfc_entry_state_enum_type state;

  /* CTFC value representing the TFI for this TFC */
  uint32 ctfc_value;

  /* TFI for each TrCh in this TrCh */
  uint8 tfi_table[UE_MAX_TRCH];
} rrcllc_dl_ctfc_entry_info_struct_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_dl_ctfc_info_struct_type

This structure definition is for storing all the TFC information in
the TFCS
--------------------------------------------------------------------*/
typedef struct
{
  /* last CTFC or TFC index. */
  uint16  last_ctfc_entry_index;

  /* Pointer to the array of TFC or CTFC entries. */
  rrcllc_dl_ctfc_entry_info_struct_type  ctfc_entry_data[UE_MAX_TFC];

} rrcllc_dl_ctfc_info_struct_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_rm_restricted_trch_info_struct_type

This structure definition is for storing the RM restriction
information for a given TrCh
--------------------------------------------------------------------*/
typedef struct
{
  /* TrCh Id of the restricted TrCh */
  uint8 trch_id;

  /* number of TF info entries */
  uint8 num_tf;

  /* allowed TF indicies per TrCH */
  boolean tf_allowed[MAX_TF];
} rrcllc_rm_restricted_trch_info_struct_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_rm_restrict_info_struct_type

This structure definition is for storing the RM restriction
information for all given TrChs
--------------------------------------------------------------------*/
typedef struct
{
  /* indicate if the RM restriction info exists or NOT */
  boolean restriction_info_exist;

  /* number of TrCh in rm restriction info */
  uint8 num_trch;

  /* restrcited TrCh info */
  rrcllc_rm_restricted_trch_info_struct_type restricted_trch_info[UE_MAX_TRCH];
} rrcllc_rm_restrict_info_struct_type;


/*-------------------------------------------------------------------
STRUCTURE: rrcllc_dl_dch_info_buffer_type

This structure holds all DL DCH transport channel specific data that
is of use to L1 or MAC. Note that this is a temporary storage for
ordered config operations.
--------------------------------------------------------------------*/
typedef struct
{
  l1_dl_trch_info_struct_type   l1_info;
  l1_dl_tf_info_struct_type     l1_tf_info[MAX_TF];
  mac_dl_dch_trch_config_type   mac_info;
  rrcllcoc_trch_info_type       local_info;
}rrcllc_dl_dch_info_buffer_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_ul_dch_info_buffer_type

This structure holds all UL DCH transport channel specific data that
is of use to L1 or MAC. Note that this is a temporary storage for
ordered config operations.
--------------------------------------------------------------------*/
typedef struct
{
  l1_ul_semi_static_parms_struct_type   l1_semi_static_parms;
  mac_ul_tfs_type                       mac_tfs_info;
  mac_ul_dch_trch_config_type           mac_dch_info;
  rrcllcoc_trch_info_type               local_info;
}rrcllc_ul_dch_info_buffer_type;


/* This stores data needed by LLC */
typedef struct
{
  /* Indicates if PCCPCH is up currently */
  boolean   pccpch_is_up;
  /* Indicates that N_PCCPCH is up currently */
  boolean   n_pccpch_is_up;
  /* Indicates if SCCPCH is up currently */
  boolean   sccpch_is_up;
  /* Indicates if DL DPCH is up currently */
  boolean   dl_dpch_is_up;
  /* CCTrCH info for PCCPCH */
  l1_dl_cctrch_info_struct_type   pccpch;
  /* CCTrCH info for N_PCCPCH */
  l1_dl_cctrch_info_struct_type   n_pccpch;
  /* PCCPCH parameters */
  l1_pccpch_parms_struct_type     pccpch_parms;
  /* N_PCCPCH parameters */
  l1_pccpch_parms_struct_type     n_pccpch_parms;
  /* CCTrCH info for SCCPCH */
  l1_dl_cctrch_info_struct_type   sccpch;
  /* CCTrCH info for DPCH */
  l1_dl_cctrch_info_struct_type   dl_dpch;
  /* Indicates if FACH transport blocks are present on SCCPCH */
  boolean   fach_included;
  /* Indicates if PCH transport blocks are present on SCCPCH */
  boolean   pch_included;


  /* Indicates if SCCPCH is up currently */
  boolean   sccpch_with_ctch_is_up;

  /* Indicates if SCCPCH with ctch is up currently */
  rrcllc_ctch_status_e_type  ctch_status;

  /* Indicates if PCH transport blocks are present on SCCPCH that
  has been selected for  CTCH */
  boolean   pch_included_for_sccpch_with_ctch;

  /* CCTrCH info for SCCPCH with CTCH*/
  l1_dl_cctrch_info_struct_type   sccpch_with_ctch;

}rrcllc_dl_phy_chan_data_type;


typedef struct
{
  /* Indicates if PRACH is up currently */
  boolean   prach_is_up;
  /* Indicates if UL DPCH is up currently */
  boolean   ul_dpch_is_up;

  cctrch_id_type  ul_dpch_cctrch_id;
  cctrch_id_type  prach_cctrch_id;
}rrcllc_ul_phy_chan_data_type;


/*-------------------------------------------------------------------
This structure holds semi-permanent data pertaining to LLC Uplink
and Downlink Physical channels as well as Uplink MAC Config flag
--------------------------------------------------------------------*/
typedef struct
{
  rrcllc_dl_phy_chan_data_type  dl_phy_chan;
  rrcllc_ul_phy_chan_data_type  ul_phy_chan;
  boolean                       send_mac_uplink_config;
}rrcllc_semi_permanent_data_type;



/*-------------------------------------------------------------------
This structure indicates need for reconfiguring UL and DL MAC and L1.
This is determined based on presence of corresponding IEs.
--------------------------------------------------------------------*/
typedef struct
{
  boolean  uplink_mac;
  boolean  downlink_mac;
  boolean  uplink_l1;
  boolean  downlink_l1;
#ifdef FEATURE_WCDMA_DC_HSUPA
//  boolean  sec_downlink_l1;
#endif /* FEATURE_WCDMA_DC_HSUPA */
}rrcllc_lower_layer_reconfig_need_type;

typedef struct
{
  /* Number of dedicated RLC logical channels mapped to TrCh */
  uint8             dl_ndlchan;
  uint8             ul_ndlchan;
  /* Info of each dedicated logical channel mapped */
  mac_ul_ded_logch_config_type    ul_dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];
  /* Configuration of each dedicated logical channel mapped to */
  /* this transport channel */
  mac_dl_ded_logch_config_type    dl_dlchan_info[MAX_DED_LOGCHAN_PER_TRCH];

  /* Future place for storing RLC size restrictions */

}rrcllc_rb_mapping_info;

#ifdef FEATURE_WCDMA_HS_FACH

typedef struct
{
  /*Num of User RBs are stored in this var*/
  uint8 num_of_uRBs;

  /*The index 0 to 3 are reserved for SRBs*/
  rb_id_type rb_id[MAX_RB];
  boolean e_fach_mapping_valid[MAX_RB];
  uint8 mac_ehs_queue_id[MAX_RB];
}rrcllc_e_fach_rb_mapping_info_type;

/*Introduced to check if the common mac-ehs queue in sib5
  is same as that in OTA*/
typedef struct 
{
  boolean in_use[UE_MAX_COMMON_HS_QUEUE];
  mac_ehs_queue_struct_type    mac_ehs_queue_info[UE_MAX_COMMON_HS_QUEUE];
}rrc_ded_mac_ehs_queue_info;

typedef enum 
{
  CELL_CHANGE_INACTIVE,
  CELL_CHANGE_ACTIVE,  /* To represent the transient state between CSP sending Channel Config till LLC takes over*/
  NO_CHANGE_IN_CELL_CAP,
  CELL_CAP_CHANGED,
#ifdef FEATURE_WCDMA_HS_RACH
  SIB5CHANGE_HSRACH_DELAY_CU
#endif
}rrcllc_sib5_change_type;


#endif /*FEATURE_WCDMA_HS_FACH*/


/*-------------------------------------------------------------------
STRUCTURE: rrcllc_rb_list_mapped_to_dch_type

This structure holds num of RBs in rrc_rb_mapped_to_dch. 
rrc_rb_mapped_to_dch contains those RBs for which RLC PDU size has changed.
--------------------------------------------------------------------*/
typedef struct {
  uint8 num_of_RBs;
  rrc_RB_Identity rrc_rb_mapped_to_dch[MAX_RB];
} rrcllc_rb_list_mapped_to_dch_type;

typedef enum {
  UL,
  DL
} rrc_direction_enum_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_rlc_size_list_type

This structure holds data extracted from an RLC Size List IE.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_RB_Identity rb_id;
  rrc_rlc_size_restriction_config_e_type restriction_type;
  uint8 count;
  uint16 size_idx[UE_MAX_TF];
}rrcllcoc_rlc_size_list_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_logical_channel_list_type

This structure holds data extracted from a Logical Channel List IE.
--------------------------------------------------------------------*/
typedef struct
{
  rrc_rlc_size_restriction_config_e_type restriction_type;
  uint8 count;
  rrc_RB_Identity rb_id[15];
}rrcllcoc_logical_channel_list_type;

/*-------------------------------------------------------------------
STRUCTURE: rrcllc_rlc_size_restriction_info_type

This structure contains the raw RLC size restriction info for a
particular channel.  This data is later validated and copied into
the MAC's data structures.
--------------------------------------------------------------------*/
typedef struct
{
  rrcllcoc_rlc_size_list_type rlc_size_list[MAX_DED_LOGCHAN_PER_TRCH];
  rrcllcoc_logical_channel_list_type logical_channel_list[UE_MAX_TF];
  boolean mac_update_needed;
}rrcllcoc_rlc_size_restriction_info_type;




typedef enum restriction_choice_e {
  TFC_CHOICE_NOT_VALID   = -1,
  MINIMUM_ALLOWED_TFC  = 0,
  ALLOWED_TFC_LIST     = 1,
  NON_ALLOWED_TFC_LIST = 2,
  RESTRICTED_TRCH_LIST = 3,
  TFCS_FULL            = 4
} restriction_choice_enum_type;

typedef struct allowed_tfc_list_s {
  uint16 total_allowed_tfc_count; /* Total count of allowed TFCs; can be more than UE_MAX_TFC */
  uint16 allowed_tfc_number[UE_MAX_TFC]; /* List of allowed TFC numbers */
} allowed_tfc_list_type;

typedef struct non_allowed_tfc_list_s {
  uint16 total_non_allowed_tfc_count; /* Total count of non allowed TFCs; can be more than UE_MAX_TFC */
  uint16 non_allowed_tfc_number[UE_MAX_TFC]; /* List of disallowed TFC numbers */
} non_allowed_tfc_list_type;

typedef struct allowed_tfi_info_s {
  uint16 no_of_allowed_tfis; /* Total number of allowed TFIs; can be more than UE_MAX_NUM_TF_64 */
  uint8 tfi[UE_MAX_TF]; /* List of TFIs which are allowed for TFC selection */
} allowed_tfi_info_type;

typedef struct trch_info_s {
  allowed_tfi_info_type  allowed_tfi_info; 
  uint32                 trch_id;     /* Transport channel ID signalled */
  boolean                dch_present; /* TRUE if DCH transport channel type is signalled */
  boolean                allowed_tfi_info_present; /* TRUE indicates presence */
} trch_info_type;

typedef struct restricted_trch_list_s {
  uint8                 no_of_transport_channels; /* Number of restricted transport channels; can be more than UE_MAX_TRCH */
  trch_info_type         trch_info[UE_MAX_TRCH]; /* Info related to each restricteed transport channel */
} restricted_trch_list_type;

typedef union tfc_choice_info_u {
  uint32                     minimum_allowed_tfc_number; /* Minimum number below which all the TFCs are valid */
  allowed_tfc_list_type      allowed_tfc_list; /* Info on list of allowed TFCs */
  non_allowed_tfc_list_type  non_allowed_tfc_list; /* Info on list of disallowed TFCs */
  restricted_trch_list_type  restricted_trch_list; /* Info on list of restricted TFCs based on transport channels */
} tfc_choice_info_u_type;

typedef struct tfc_subset_info_s {
  restriction_choice_enum_type restriction_choice; /* TFC restriction choice */
  tfc_choice_info_u_type       tfc_choice_info;    /* Info related to the selected restriction choice */
} tfc_subset_info_type;


typedef struct rrcllc_oc_tfc_subset_list_s {
  uint32                no_of_subset_lists; /* The number of TFCsubset lists stored; can be more than UE_MAX_TFC_SUBSET */
  tfc_subset_info_type  tfc_subset_info[UE_MAX_TFC_SUBSET]; /* Each subsets' list info */
} rrcllc_oc_tfc_subset_list_type;

typedef struct {
  boolean srb5_rb_exists;  /* TRUE indicates presence of SRB#5 */
  rrc_RB_Identity rb_id;           /* SRB#5 RB ID */
  boolean srb5_trch_exists;/* TRUE indicates Transport Channel for this RB exists*/
  uint32  trch_id;         /* Transport channel ID on which SRB#5 is mapped */
  uint16  lc_id;           /* Logical channel ID that is allocated for this */
} rrcllc_srb5_info_type;

typedef struct
{
  l1_ul_prach_cfg_struct_type prach_cfg; /*PRACH config parameters*/
  rrcllcoc_trch_info_type   ul_rach_trch_info[UE_MAX_TRCH];  /*RACH TrCH Info*/
  boolean                        ul_rach_trch_idx_in_use[UE_MAX_TRCH];
  l1_ul_asc_struct_type                 asc_info[MAX_ASC];  
} prach_config_type;

typedef struct
{

  /* number of transport channels valid for this CCTrCH */
  uint8                                 num_trch;
  /* number of TFCIs */
   uint8                                 num_tfci;
  /* Semi-static parameters for Uplink Transport channels */
  l1_ul_semi_static_parms_struct_type   semi_static_parms[UE_MAX_TRCH];
   /* Map of Gain factor parameters for Uplink */
  l1_ul_gain_factor_parms_struct_type   gain_factor_parms[L1_UL_TFC_MAX];
}rrc_ul_cctrch_struct_type;


typedef struct {
  rlc_lc_id_type lc_id;          /* Unique identifier for the Downlink RLC logical channel */
  rrc_RB_Identity rb_id;               /*rb-id provided in OTA*/
  uecomdef_logchan_e_type lc_type;      /* Type of Downlink UM logical channel */
  
  rlc_li_e_type li_size;        /* Network can explicitly specify li size in Release 5 */
  boolean alternate_e_bit;              /* TRUE - Alternate E-bit interpretation */
                                        /* FALSE- Normal E-bit interpretation    */
} rrc_rlc_dl_um_channel_config_type;

typedef struct {
  uint8  nchan;          /* Number of transparent mode DL logical channels to configure */ 
  rrc_rlc_dl_um_channel_config_type chan_info[UE_MAX_DL_UM_CHANNEL];
                                        /* Downlink UM RLC configuration for each channel*/ 
} rrc_rlc_dl_um_config_type;

typedef struct {
  rlc_lc_id_type lc_id;      /* Unique identifier for the Uplink RLC logical channel */
  rrc_RB_Identity rb_id;          /*rb-id provided in OTA*/
  uecomdef_logchan_e_type lc_type;   /* Type of Downlink UM logical channel */
  uint16 timer_discard;       /* Elapsed time in milliseconds before an SDU is discarded at the transmitter*/
  boolean alternate_e_bit;              /* TRUE - Alternate E-bit interpretation */
                                        /* FALSE- Normal E-bit interpretation    */
} rrc_rlc_ul_um_channel_config_type;

typedef struct {
  uint8  nchan;    /* Number of transparent mode UL logical channels to configure */ 
  rrc_rlc_ul_um_channel_config_type chan_info[UE_MAX_UL_UM_CHANNEL];
                                        /* Uplink UM RLC configuration for each channel*/
} rrc_rlc_ul_um_config_type;
/* Indicates if the RLC is provided in OTA or not.
   And if provided ,if it is fixed or flexible*/
typedef enum
{
  RRC_PDU_SIZE_NOT_SET,
  RRC_FIXED_PDU_SIZE_SET,
  RRC_FLEXIBLE_PDU_SIZE_SET
}rrc_am_rlc_pdu_enum_type;
/* Stores if the RLC is provided in OTA or not . 
And stores the rlc size indicated by the OTA */
typedef struct {

 rrc_am_rlc_pdu_enum_type rrc_pdu_type;

 rrc_RB_Identity  rb_id;
 uint32 dl_rlc_am_pdu_size;
}   rrc_rb_dl_am_rlc_size_info;
#ifdef FEATURE_MAC_I
/*------------------------------------------------------------------*/
/*      Re-configuration from MAC-e/es <-> MAC-i/is                 */
/*------------------------------------------------------------------*/
typedef enum 
{
  RRC_MAC_E_INVALID_TRANSITION,
  RRC_MAC_E_TO_I,
  RRC_MAC_I_TO_E,
  RRC_MAC_E_NO_TRANSITION
}rrc_mac_e_transition_type;

#endif /* FEATURE_MAC_I */

#define MAX_NO_USER_RB (MAX_RB - (DCCH_DT_LOW_PRI_RADIO_BEARER_ID+1))

typedef struct 
{
  rlc_pdu_size_type ul_rlc_pdu_type;    /* Fixed/Flexible PDU size         */
  uint32 ul_fixed_rlc_size;                   /* Fixed PDU size.                  */
  rlc_li_e_type ul_flex_li_size;          /* LI size                         */
}ul_rlc_size_info;

/*Since this is an index we start from 0 to 27 where we will store from RB ID 5 to 32*/
#define RRC_GET_OLD_BACKUP_SIZE_INDEX(S,idx)  \
  idx = (S-(DCCH_DT_LOW_PRI_RADIO_BEARER_ID+1));\
  if(idx >=MAX_NO_USER_RB)\
    rv = FAILURE;\
  else\
    rv = SUCCESS;\

extern ul_rlc_size_info rrc_old_ul_rlc_size_backup[MAX_NO_USER_RB];
extern boolean ul_reestab_for_pch_trans;

extern rrc_cell_capability_type last_active_cell_capability;

#ifdef FEATURE_WCDMA_HS_RACH
typedef struct {
    /* Access Service Class Info per ASC */
  l1_ul_asc_struct_type                 asc_info[MAX_ASC];
/*
  * Persistence Scaling factors used to calculate the Access
  * Service Class Establishment
  */
  uint32                          psf[MAX_ASC_PERSIST];

  /* Access Class to Access Service Class Mapping value */
  uint8                           ac_to_asc_mapping[MAX_ASC_MAP];
}   rrc_hsrach_info_type;
#endif
#ifdef FEATURE_WCDMA_HS_FACH
typedef enum 
{
  INVALID_HRNTI,
  COMMON_HRNTI,
  DEDICATED_HRNTI
} efach_hrnti_type_enum;
#endif

/*-------------------------------------------------------------------
STRUCTURE: rrc_ordered_config_type

This structure defines the variable ORDERED_CONFIG as defined in
the RRC spec 25.331.
--------------------------------------------------------------------*/
typedef struct
{
  /* Physical Layer Ordered Config Data */
  /* CCTrCH info */
  l1_dl_cctrch_info_struct_type   l1_dl_cctrch_parms;
  l1_dl_ctfc_info_struct_type     l1_dl_ctfc_parms;
  l1_ul_cctrch_cfg_struct_type    l1_ul_cctrch_parms;

  /* CCTrCH info for sccpch carrying CTCH*/
  l1_dl_cctrch_info_struct_type   l1_dl_cctrch_parms_for_sccpch_with_ctch;
  l1_dl_ctfc_info_struct_type     l1_dl_ctfc_parms_for_sccpch_with_ctch;

  /* Phychan info */
  l1_dl_phychan_db_struct_type    l1_dl_chan_parms;
  l1_ul_phychan_cfg_struct_type   l1_ul_chan_parms;


  /* This is RRC information to indicate the queue/s used by dflow */
  boolean dl_dflow_index_in_use[UE_MAX_MAC_D_FLOW];

  /* L1 HSDPA Information */
  l1_hsdpa_info_struct_type l1_hsdpa_info;

  /* MAC storage space for HS flow Id */
  mac_dflow_struct_type    mac_dflow_info[UE_MAX_MAC_D_FLOW];

  rrc_hsdpa_hrnti_enum_type hrnti_status;

  uint16 hrnti;

#ifdef FEATURE_WCDMA_HS_FACH

  efach_hrnti_type_enum                                        efach_hrnti_status;
  uint16                                                       efach_hrnti;
  boolean                                                      efach_dl_lc_id_in_use[UE_MAX_DL_LOGICAL_CHANNEL];
  mac_dl_ehs_logch_config_type                                 efach_dl_logchan_info[UE_MAX_DL_LOGICAL_CHANNEL];
  mac_ehs_queue_struct_type                                    efach_common_queue[UE_MAX_COMMON_HS_QUEUE];

#endif

  /* This is RRC information to indicate the queue/s used */
  boolean mac_ehs_lc_id_in_use[UE_MAX_DL_LOGICAL_CHANNEL];

  /* This is RRC information to indicate the queue/s used */
  boolean mac_ehs_queue_index_in_use[UE_MAX_HS_QUEUE];

  /* Info of dedicated logical channels mapped */
  mac_dl_ehs_logch_config_type dl_macehs_logchan_info[UE_MAX_DL_LOGICAL_CHANNEL];
  
  /* MAC storage space for EHS Q Id */
  mac_ehs_queue_struct_type    mac_ehs_queue_info[UE_MAX_HS_QUEUE];

#ifdef FEATURE_WCDMA_HS_FACH
  boolean hs_status_in_e_fach;
  hsdpa_action_enum_type hs_action_in_e_fach;
#endif /*FEATURE_WCDMA_HS_FACH*/

  /* E_DCH_TRANSMISSION variable to be evaluated after processing config message */
  boolean e_dch_transmission;

  /* set to MAC_EUL_REVERT_TO_OLD_TSN whenever UE has to reverting back to old
     config. MAC-E should process this information first(before mac-e-reset
     indicator). 
     If it is set to MAC_EUL_REVERT_TO_OLD_TSN then MAC-E is expected to
     revert to the old TSN information which was stored on receipt of last
     CMAC_UL_CONFIG_REQ.
     If it is set to MAC_EUL_REVERT_TO_OLD_CONFIG_BACKUP_NEEDED then back up of
     current config(like TSN) is needed. */
  mac_e_tsn_action_e_type   mac_e_tsn_action;
  
 /* Indicates L1 to flush all HARQs.
    RRC will set this to TRUE under following conditions:
      - NW explicitly indicate in the reconfig message
      - whenever TTI reconfiguration happens 

    When this variable is set to TRUE, then all the HARQ processes
    will be flushed in Firmware. MAC will use this variable to flush all
    HARQ process related information that is maintained internally.
    Thsi information needs to be flushed so that happy bit is computed
    correctly in MAC */
 boolean mac_e_reset_indicator;

 /* req mask for EUL configuration */
 /* This request mask will outline the action related to HSUPA UL and DL */
 /* bitmask:
      L1_EUL_NOOP                 0x00
      L1_EUL_START                0x01
      L1_EUL_RECFG_UL             0x02
      L1_EUL_RECFG_DL             0x04
      L1_EUL_STOP                 0x08 */
 uint8 l1_e_req_mask;
 
 l1_e_info_struct_type l1_e_info;

 mac_eul_action_e_type               mac_eul_action;

 /* MAC E-DCH config info - MAC should look into this pointer only if
    eul_action is MAC_EUL_CONFIG */
 mac_e_config_s_type                 mac_e_config;

#ifdef FEATURE_WCDMA_DC_HSUPA

/* E_DCH_TRANSMISSION variable to be evaluated after processing config message */
  boolean sec_e_dch_transmission;

/* req mask for EUL configuration */
/* This request mask will outline the action related to HSUPA UL and DL */
/* bitmask:
     L1_EUL_START                0x01
     L1_EUL_RECFG_UL             0x02
     L1_EUL_RECFG_DL             0x04
     L1_EUL_STOP                 0x08 */

  uint8 l1_sec_e_req_mask;

  l1_sec_e_info_struct_type           l1_sec_e_info;

  mac_sec_eul_action_e_type           mac_sec_eul_action;

/* When secondary mac eul is configured tsn filed exten should be activated at MAC 
   When secondary mac eul is deconfigured mac should stop using tsn field extn */
  mac_sec_eul_info_s_type             mac_sec_eul_config;

#endif /* FEATURE_WCDMA_DC_HSUPA */

#ifdef FEATURE_WCDMA_HS_RACH
  l1_e_hs_rach_info_struct_type l1_hs_rach_e_info;
/* This request mask will outline the action related to HS_RACH */
/* bitmask:
	 L1_HS_RACH_NO_OP			  0x00
	 L1_HS_RACH_START			  0x01
	 L1_HS_RACH_RECFG			  0x02
	 L1_HS_RACH_STOP			  0x04 */
  uint8 l1_hs_rach_req_mask;

/*boolean to indicate for FACH to DCH transition*/
   boolean transition_to_dch;
   
   /* e-dch common resource list*/
  l1_e_dch_common_list_info_struct_type common_e_dch_resource_list;
/* MAC */
  mac_hs_rach_action_e_type mac_hs_rach_action;

  /*hs_rach_to_eul_trans will be set to TRUE by RRC at hs_rach to eul transistion*/
  boolean hs_rach_to_eul_trans;
  
  mac_hs_rach_config_s_type mac_hs_rach_config;
  
   /*  When this variable is set to TRUE, then all the HARQ processes
     will be flushed in Firmware. MAC will use this variable to flush all
     HARQ process related information that is maintained internally.
     Thsi information needs to be flushed so that happy bit is computed
     correctly in MAC */
  boolean mac_hsrach_e_reset_indicator;

  /*   13.4.20oo READY_FOR_COMMON_EDCH
     In CELL_FACH state or Idle mode, this variable indicates whether E-DPDCH and
     E-DPCCH transmission procedures for FDD */
  boolean ready_for_common_edch;
  
  /* 13.4.1a COMMON_E_DCH_TRANSMISSION 
     This variable indicates whether E-DPDCH and E-DPCCH transmission procedures 
      for FDD   in CELL_FACH state or Idle mode are ongoing. */
  boolean common_edch_transmission;
  
  /*   13.4.8d HSPA_RNTI_STORED_CELL_PCH
      This variable indicates whether variables H_RNTI, C_RNTI and E_RNTI are stored in CELL_PCH state. */
  boolean hspa_rnti_stored_cell_pch;

  /*This variable indicates whether IE ack_nack_support_on_hs_dpcch is present in SIB5 for the current cell  */
  boolean ack_nack_support_on_hs_dpcch;	

#endif

/* DTX_DRX_STATUS variable to be evaluated after processing config message */
/*L1 to not look at this */
boolean cpc_dtx_drx_status;

/* req mask for DTX-DRX configuration */
/* This request mask will outline the action related to DTX*/
/* bitmask:
  L1_CPC_DTX_NO_OP                      0x00
  L1_CPC_DTX_START                      0x01
  L1_CPC_DTX_RECONFIG                   0x02
  L1_CPC_DTX_STOP                       0x04*/
uint8 l1_cpc_dtx_req_mask;


/* DRX_STATUS variable to be evaluated after processing config message */
/*L1 to not look at this */
boolean cpc_drx_status;

/* This request mask will outline the action related to DRX*/
/* bitmask:
  L1_CPC_DRX_NO_OP                      0x00
  L1_CPC_DRX_START                      0x01
  L1_CPC_DRX_RECONFIG                   0x02
  L1_CPC_DRX_STOP                       0x04*/
uint8 l1_cpc_drx_req_mask;


/* This enum indicate what action L1 needs to take on received HS-SCCH orders*/
l1_dtx_drx_hs_scch_order_info_enum_type l1_hs_scch_order_action;

/*Timing info needed to start DTX/DRX at L1 for CPC*/
l1_dtx_drx_timing_info_struct_type dtx_drx_timing_info;

/*All the info needed for DTX operation at L1 for CPC*/
l1_dtx_info_struct_type dtx_info;

/*All the info needed for DRX operation at L1 for CPC*/
  l1_drx_info_struct_type drx_info;



  /*
  * This is local data that L1 wants RRC to store. It is accessed
  * via a function call.
  */
  rrcllcoc_l1_ul_init_pwr_type    rrc_ul_pwr_init_parms;
  /* RLC size restriction data */
#ifdef FEATURE_RRC_TFC_RLC_SIZE_FOR_DCH
  rrcllcoc_rlc_size_restriction_info_type dch_rlc_size_restriction_info[UE_MAX_TRCH];
#endif
  rrcllcoc_rlc_size_restriction_info_type rach_rlc_size_restriction_info;


  /* MAC Ordered Config Data */
  mac_dl_config_type              mac_dl_parms;
  mac_ul_config_type              mac_ul_parms;

  /* RLC Ordered Config Data */
  /* TM and UM Uplink */
  rlc_ul_tm_config_type           rlc_ul_tm_parms;
  rrc_rlc_ul_um_config_type       rlc_ul_um_parms;

  /* TM & UM Downlink */
  rlc_dl_tm_config_type           rlc_dl_tm_parms;
  rrc_rlc_dl_um_config_type       rlc_dl_um_parms;

  /* AM mode only has one type for both uplink and downlink */
  rlc_am_config_req_type          rlc_am_parms;

#ifdef FEATURE_UMTS_PDCP
  rrcllcoc_pdcp_parms_type        pdcp_parms;
#endif /* FEATURE_UMTS_PDCP */

  /* Start: Storage space for data needed by L1 */
  /* Downlink Transport channel data */
  l1_dl_trch_info_struct_type           dl_trch_info[UE_MAX_TRCH];
  l1_dl_trch_info_struct_type           dl_trch_info_for_sccpch_with_ctch[UE_MAX_TRCH];

  /* Downlink Dedicate Transport Format (TF) data per Transport Channel */
  l1_dl_tf_info_struct_type             dl_tf_info[UE_MAX_TRCH][MAX_TF];
  l1_dl_tf_info_struct_type             dl_tf_info_for_sccpch_with_ctch[UE_MAX_TRCH][MAX_TF];

  /* Downlink Transport Format Combinations (TFC) */
  l1_dl_ctfc_entry_info_struct_type     ctfc_info[UE_MAX_TFC];
  l1_dl_ctfc_entry_info_struct_type     ctfc_info_for_sccpch_with_ctch[UE_MAX_TFC];
  rrc_ul_cctrch_struct_type rrc_ul_cctrch_params;
  /* Access Service Class Info per ASC */
  l1_ul_asc_struct_type                 asc_info[MAX_ASC];
#ifdef FEATURE_WCDMA_HS_RACH
  rrc_hsrach_info_type                 rrc_hsrach_info;
#endif
  /* Needed for calculations - also store parameters in a slightly
   * different form.
   */
  /* CTFC management data used to calculate the TFCS */
  rrcllc_dl_ctfc_info_struct_type       dl_ctfc;

  /* Rate Matching info for DL Blind Rate Detection */
  rrcllc_rm_restrict_info_struct_type   rm_restrict;

 /* CTFC management data used to calculate the TFCS
    for sccpch that carries CTCH */
  rrcllc_dl_ctfc_info_struct_type       dl_ctfc_for_sccpch_with_ctch;

  /* Rate Matching info for DL Blind Rate Detection */
  rrcllc_rm_restrict_info_struct_type   rm_restrict_for_sccpch_with_ctch;


  /* End: Storage space for data needed by L1 */

  /* Start: Storage space for data needed by MAC */
  /* FACH transport channel info */
  mac_dl_fach_trch_config_type          fach_info[UE_MAX_TRCH];

  /* DCH Transport channel information */
  mac_dl_dch_trch_config_type           dl_dch_info[UE_MAX_TRCH];

  /* DSCH Transport channel information */
  mac_dl_dsch_trch_config_type          dsch_info[UE_MAX_TRCH];

  /* MAC Uplink TFCS information */
  mac_ul_tfcs_type                      ul_tfcs;

  /* List of removed TFCIs, sorted at all times based on TFCI value */
  uint32                                removed_tfci_list[UE_MAX_TFC];

  /* The number of removed TFCIs is */
  /* - set to zero initially  and also when UE receives complete TFCS configuration */
  /* - incremented by one for each TFCI removal and the TFCI is stored in the table */
  /* - decremented by one (if number of removed TFCIs > zero) when a TFCI is added */
  uint16                                num_removed_tfcis;

  /* MAC uplink CCTrCH information */
  mac_ul_cctrch_config_type             ul_cctrch;

  /* End: Storage space for data needed by MAC */

  /* Start: Storage space for data needed by RLC */
  /* RLC-AM information */
  rlc_am_config_type                    am_config[UE_MAX_AM_ENTITY];
  /* End: Storage space for data needed by RLC */

  /*
  * Persistence Scaling factors used to calculate the Access
  * Service Class Establishment
  */
  uint32                          psf[MAX_ASC_PERSIST];
  /*
  * Dynamic Persistence level used to calculate the Access
  * Service Class Establishment
  */
  rrc_DynamicPersistenceLevel     dpl;

    /* Total count of all active UL and DL TrCHs */
  uint8                           dl_trch_cnt;
  uint8                           ul_trch_cnt;

  /* Data structures for Transport channel indexing */
  /* This is RRC information to indicate which TrCH indicies are in use. */
  boolean                         dl_pch_trch_idx_in_use;
  boolean                         dl_fach_trch_idx_in_use[UE_MAX_TRCH];
  boolean                         dl_dch_trch_idx_in_use[UE_MAX_TRCH];
  boolean                         ul_rach_trch_idx_in_use[UE_MAX_TRCH];
  boolean                         ul_dch_trch_idx_in_use[UE_MAX_TRCH];

  boolean                         dl_pch_trch_idx_in_use_for_sccpch_with_ctch;
  boolean                         dl_fach_trch_idx_in_use_for_sccpch_with_ctch[UE_MAX_TRCH];
  /* This stores the TrCH ID to TrCH Idx mapping info.*/
  rrcllcoc_trch_info_type         dl_pch_trch_info;
  rrcllcoc_trch_info_type         dl_fach_trch_info[UE_MAX_TRCH];
  rrcllcoc_trch_info_type         dl_fach_trch_info_for_sccpch_with_ctch[UE_MAX_TRCH];
  rrcllcoc_trch_info_type         dl_dch_trch_info[UE_MAX_TRCH];
  rrcllcoc_trch_info_type         ul_rach_trch_info[UE_MAX_TRCH];
  rrcllcoc_trch_info_type         ul_dch_trch_info[UE_MAX_TRCH];

  /* End: Data structures for Transport channel indexing */

  /* This structure indicates the need for reconfiguring
   * Uplink and Downlink MAC and L1.
   */
  rrcllc_lower_layer_reconfig_need_type  reconfig_needed;

  /* Access Class to Access Service Class Mapping value */
  uint8                           ac_to_asc_mapping[MAX_ASC_MAP];

  /* Store the Activation Time and DRX Cycle Lengths. */
    /* the type of action time.  NONE, SFN or CFN */
  l1_act_time_enum_type           act_time_type;
  uint16                          activation_time;
  uint16                          sdu_time;
  rrc_state_e_type                rrc_state_indicator;
  uint32                          cs_drx_cycle_length;
  uint32                          ps_drx_cycle_length;
  uint32                          utran_drx_cycle_length;
  rrcllc_rb_mapping_info          cell_fach_rb_mapping_info;

  boolean        dest_freq_present;    /* Indicates if Destination frequency is present */
  rrc_freq_type  dest_freq;            /* Destination Frequency
                                        */

#ifdef FEATURE_WCDMA_DC_HSUPA
//  boolean        sec_eul_dest_freq_present;    /* Indicates if Destination frequency is present ..this may not be required */
  rrc_freq_type  sec_eul_dest_freq;            /* Destination Frequency  this will be present always if DC_HSUA is up..
                                                                                       think that we are ging to include this in L1 configreq only if its START and/or START+STOp i.e handover cases*/  
  boolean        sec_eul_is_hho;               /* this can be true only if secondary needs to set to START+STOp */
  boolean        sec_ul_asu;
#endif
                                        
  boolean        dest_psc_present;     /* Indicates if Destination PSC is present */
  rrc_scr_code_type  dest_psc;         /* Destination Primray Scrambling Code - relevant
                                        * only for DCH->FACH transition.
                                        */


  /* Compressed Mode Info */
  l1_cm_info_struct_type cm_info;

  /* Gap Parameters */
  l1_cm_tgp_seq_info_struct_type  tgp_seq_info[L1_CM_MAX_TGP_SEQ_COUNT];

  /*Count of CM TGPSIs which are provided in OTA*/
  uint8 cm_tgpsi_ota_cnt;
  /*List of CM TGPSIs which are provided in OTA*/
  uint8 cm_tgpsi_ota_changed[L1_CM_MAX_TGP_SEQ_COUNT];
  
  
 /* RRC will set the bit based on what type of call is active and 
     what type of call is being setup.
  bit7(MSB) bit6 bit5 .........bit0(LSB)
  bit0 ...................1 Voice call  
  bit1 ...................1 Release 99 PS data call
  bit2 ...................1 HS call 
  bit3-7 ...................reserved */
  uint8 call_type_mask;

  /* boolean flag to indicate that hard handover is in progress */
  boolean is_hho;
  rrcllc_oc_tfc_subset_list_type ul_tfc_subset_list;
  rrcllc_srb5_info_type  srb5_info;
  /* Timing maintained sync Indicator applicable or not*/
  uint8 tm_sync_ind_applicable;
  boolean predef_po3_status;
   rrc_rb_dl_am_rlc_size_info rb_am_rlc_size_ota[UE_MAX_AM_ENTITY];
#ifdef FEATURE_MAC_I
  /*Structure to maintain the rlc_OneSidedReEst per AM entity*/
  boolean rlc_one_sided_reest[UE_MAX_AM_ENTITY];
#endif /* FEATURE_MAC_I */
} ordered_config_type;

typedef struct {
 rrcllcoc_l1_ul_init_pwr_type    rrc_ul_pwr_init_parms;
  /* MAC Uplink TFCS information */
  mac_ul_tfcs_type                      ul_tfcs;
  l1_ul_cctrch_cfg_struct_type    l1_ul_cctrch_parms;

  /* Semi-static parameters for Uplink Transport channels */
  l1_ul_semi_static_parms_struct_type   semi_static_parms[UE_MAX_TRCH];

    /* Map of Gain factor parameters for Uplink */
  l1_ul_gain_factor_parms_struct_type   gain_factor_parms[L1_UL_TFC_MAX];
  rrc_PersistenceScalingFactorList  *persistence_scaling_factor_list;  
  rrc_AC_To_ASC_MappingTable        *ac_to_asc_mapping_table; 
  rrc_TFCS                                 *tfcs;
  rrc_PRACH_RACH_Info                      *rach_info;
  rrc_TransportChannelIdentity              transport_channel_id;
  rrc_TransportFormatSet                   *rach_tfs;  
  rrc_PRACH_Partitioning                   *prach_partitioning;
  rrc_PrimaryCPICH_TX_Power                primary_cpich_tx_power; 
  rrc_ConstantValue                        constant_value;  
  rrc_PRACH_PowerOffset                    *prach_pow_offset; 
  rrc_RACH_TransmissionParameters          *rach_transmission_parms;  
  rrc_AICH_Info                            *aich_info; 
  rrc_AdditionalPRACH_TF_and_TFCS_CCCH_IEs * rrc_additional_prach_TF_list;
} rrcllc_prach_sel_info_type;

#ifdef FEATURE_WCDMA_HS_FACH
typedef enum {
  RRC_E_FACH_COMMON,
  RRC_E_FACH_DEDICATED,
  RRC_E_FACH_INVALID
} rrc_e_fach_operation_mode_e_type;
/* This enum defines the RB-Mapping for FACH/RACH or mac-ehs/RACH Trch channel used */
typedef enum
{
  INVALID_RACH_RB_MAPPING,
  FACH_RACH_RB_MAPPING,
  EFACH_RACH_RB_MAPPING
} rach_rb_mapping_enum_type;

#endif /*FEATURE_WCDMA_HS_FACH*/

typedef struct
{
  rrcllc_toc_usage_e_type   toc_usage;
  ordered_config_type       *toc_ptr;
}rrcllc_transition_config_type;

extern rrc_state_e_type  rrc_ordered_state;

/* This structure has all data to provide complete status of OC */
typedef struct
{
  /*This enum indicates the status of Ordered Config */
  rrcllc_oc_set_status_e_type   set_status;
  /* This records which one of the RRC procedures last set the OC */
  rrc_proc_e_type               set_by_proc;
  /* This records the current OC processing state if OC is set
   * Note that process_state is valid only when set_status != OC_NOT_SET
   */
  rrcllc_oc_process_state_e_type  process_state;
  /* This variable indicates that SMC needs to be notified when
   * a reconfiguration is completed.
   */
  boolean              notify_smc;
  /* This variable indicates that a reconfiguration message is currently
   * being processed.
   */
  boolean reconfig_in_progress;

}rrcllc_oc_complete_status_type;


/* NOT USED CURRENTLY */

typedef struct e_dch_transmission_s
{
  boolean curr_value;                /* EDCH transmission current value */
  boolean next_value;                /* EDCH transmission value after processing OTA msg */
  boolean rnti_info_stored;          /* Indicates whether Primary or Secondary is present*/
  boolean edpcch_info_stored;        /* Indicates whether E-DPCCH info stored */
  boolean edpdch_info_stored;        /* Indicates whether E-DPDCH info stored */
  boolean serving_cell_info_stored;  /* Indicates whether serving cell info stored */
  boolean log_chan_to_edch;          /* Indicates whether logical channel to RB mapping exists */
  boolean e_mac_info_changed;        /* Indicates whether mac info is included in OTA msg */
  boolean e_l1_dl_info_changed;      /* Indicates whether L1 dl info is included in OTA msg */
  boolean e_l1_ul_info_changed;      /* Indicates whether L1 ul info is included in OTA msg */
}e_dch_transmission_type;

#ifdef FEATURE_WCDMA_HS_FACH
#define MAX_HANGING_RB_MAPPING_INFO 13
#else
#define MAX_HANGING_RB_MAPPING_INFO 5
#endif
/* This enum defines the RB-Mapping for the  type of Trch channel used */
typedef enum
{
  INVALID_RB_MAPPING,
  UL_DCH_RB_MAPPING,
  DL_DCH_RB_MAPPING,
  RACH_RB_MAPPING,
  FACH_RB_MAPPING,
  E_DCH_RB_MAPPING,
  MAC_D_RB_MAPPING,
  MAC_EHS_RB_MAPPING,
  ALL_RB_MAPPING  
} rb_mapping_chan_enum_type;

/* RB-Mapping info for the UL DCH Trch channel */
typedef struct
{
  boolean valid; /* indicates if the database is valid */
  tr_ch_id_type tr_id; /* Trch channel id for the UL DCH for which the RB-Mapping is stored*/
  uint8 log_channel_id;/* Logical channel id received in the OTA message in RB-Mapping  */
  rrcllcoc_rlc_size_list_type size_rest_info;/* RLC size info */
  uint8 mac_log_priority;/* MAC Priority received in the OTA message in RB-Mapping*/
}ul_rb_dch_mapping_info_type;

/* RB-Mapping info for the RACH Trch channel */
typedef struct
{
  boolean valid; /* indicates if the database is valid */
  tr_ch_id_type tr_id; /*none for now */
  uint8 log_channel_id;/* Logical channel id received in the OTA message in RB-Mapping  */
  rrcllcoc_rlc_size_list_type size_rest_info;/* RLC size info */
  uint8 mac_log_priority;/* MAC Priority received in the OTA message in RB-Mapping*/
}ul_rb_rach_mapping_info_type;


  /* RB-Mapping info for the HSUPA channel */
typedef struct
{
  boolean valid;/* indicates if the database is valid */
  uint8 mac_e_flow_id;
  uint8 mac_log_priority;/* MAC Priority received in the OTA message in RB-Mapping*/
   /* Mapping of rlc-pdu-size  to ddi. Valid number of fields in following array is
     equal to 'num_rlc_size_ddi_info' */
       /* logical channel identifier (1-15) - as received from NW */
  /* To be used while sending Scheduling Information to NW */
  uint8      log_channel_id;/* Logical channel id received in the OTA message in RB-Mapping  */
  boolean in_sched_info;
  uint8                              num_rlc_size_ddi_info; /* 0..32, 0 is valid only when action is NOOP or STOP */
  /* Mapping of rlc-pdu-size  to ddi. Valid number of fields in following array is
     equal to 'num_rlc_size_ddi_info' */
  mac_e_rlc_size_ddi_info_s_type     rlc_size_ddi_info[MAX_RLC_PDU_SIZE_PER_LOGCHAN];  
#ifdef FEATURE_MAC_I
  rlc_pdu_size_type rlc_pdu_type;/* indicates if ul flexible pdu size is configured */
  rlc_li_e_type      ul_li_size;           /* LI size, only for AM       */
  uint16             ul_flex_min_pdu_size; /* Minimum PDU size           */
  uint16             ul_flex_max_pdu_size; /* Minimum PDU size           */
#endif /* FEATURE_MAC_I */
}ul_rb_mac_e_mapping_info_type;

typedef struct
{

  ul_rb_dch_mapping_info_type  ul_dch_info;
  ul_rb_rach_mapping_info_type  rach_info;
  ul_rb_mac_e_mapping_info_type ul_mace;

}ul_mapping_info_type;



typedef struct
{
  boolean valid;/* indicates if the database is valid */
  tr_ch_id_type tr_id; 
      /* MAC logical channel identifier (1-15) */
  /* Distinguishes dedicated logical channels when more than one */
  /* is mapped onto same transport channel */
  uint8               log_channel_id;/* Logical channel id received in the OTA message in RB-Mapping  */
   
}dl_rb_dch_mapping_info_type;

typedef struct
{
  boolean valid;/* indicates if the database is valid */
  tr_ch_id_type fach_id;
      /* MAC logical channel identifier (1-15) */
  /* Distinguishes dedicated logical channels when more than one */
  /* is mapped onto same transport channel */
  uint8               log_channel_id;/* Logical channel id received in the OTA message in RB-Mapping  */

}dl_rb_fach_mapping_info_type;

typedef struct
{
  boolean valid;/* indicates if the database is valid */
  uint8 mac_d_flow_id;
  /* MAC logical channel identifier (1-15) */
  /* Distinguishes dedicated logical channels when more than one */
  /* is mapped onto same transport channel */
  uint8               log_channel_id; /* Logical channel id received in the OTA message in RB-Mapping  */
    /* Radio Bearer Identifier (0-31) */

}dl_rb_macd_mapping_info_type;
typedef struct
{
  boolean  valid;/* indicates if the database is valid */
  uint8  queue_id; /* Queue id received in the OTA message in RB-Mapping  */
  uint8  log_channel_id; /* Logical channel id received in the OTA message in RB-Mapping  */

}dl_rb_macehs_mapping_info_type;


typedef struct
{
  dl_rb_dch_mapping_info_type dch_map_info;
  dl_rb_fach_mapping_info_type fach_map_info;
  dl_rb_macd_mapping_info_type macd_map_info;
  dl_rb_macehs_mapping_info_type macehs_map_info;

}dl_mapping_info_type;


typedef struct
{
  boolean in_use; /*TRUE for being in use and FASLE otherwise */
  rrc_RB_Identity rb_id;/*rb id for which mapping is maintained */
  uecomdef_logch_mode_e_type    logch_mode;
  ul_mapping_info_type ul_mapping_info;
  dl_mapping_info_type dl_mapping_info;
}rb_mapping_info_type;

typedef enum
{
  REL_99,
  REL_5,
  REL_6,
  REL_7,
  REL_8,
}mapping_ie_release_version;
#ifdef FEATURE_WCDMA_HS_FACH

#define MAX_MAPPING_COMBO 8

/* Use uint8 to represent the mask*/

// High bits for UL and low bits for DL
#define UL_DCH_MAPPING         0x0010
#define UL_EDCH_MAPPING        0x0020
#define UL_RACH_MAPPING        0x0040
#define INVALID_MAPPING        0x0080

#define DL_DCH_MAPPING         0x0001
#define DL_MAC_D_MAPPING       0x0002
#define DL_MAC_EHS_MAPPING     0x0004
#define DL_FACH_MAPPING        0x0008

#define DL_STUB                 0x000F
#define UL_STUB                 0x00F0

typedef union
{
  ul_rb_dch_mapping_info_type  dch_map_info;
  ul_rb_rach_mapping_info_type  rach_map_info;
  ul_rb_mac_e_mapping_info_type edch_map_info;
  
}ul_mapping_info_type_;

typedef union
{
  dl_rb_dch_mapping_info_type dch_map_info;
  dl_rb_fach_mapping_info_type fach_map_info;
  dl_rb_macd_mapping_info_type macd_map_info;
  dl_rb_macehs_mapping_info_type macehs_map_info;

}dl_mapping_info_type_;


typedef struct
{
  uint8 combo_type;
  ul_mapping_info_type_ ul_map;
  dl_mapping_info_type_ dl_map;
}mapping_info_type;

typedef struct 
{
  rrc_RB_Identity rb_id;/*rb id for which mapping is maintained */
  uecomdef_logch_mode_e_type    logch_mode;
  mapping_info_type mapping_info[MAX_MAPPING_COMBO]; // Can be made pointer 
}rb_mapping_info_type_ ;

#endif


/* defines that max no of DCh channels required for one voice rab */
#define MAX_DCH_VOICE_CHANNEL 3
#define MIN_DCH_VOICE_CHANNEL 2
/*-------------------------------------------------------------------
STRUCTURE: rrcllc_amr_mapping_info_type

This structure defines the uplink and downlink RLC Logical Channel IDs
mapping for the AMR Mode commands.
--------------------------------------------------------------------*/
typedef struct
{
  rlc_lc_id_type     ul_amr_a_id;
  rlc_lc_id_type     ul_amr_b_id;
  rlc_lc_id_type     ul_amr_c_id;
  rlc_lc_id_type     dl_amr_a_id;
  rlc_lc_id_type     dl_amr_b_id;
  rlc_lc_id_type     dl_amr_c_id;
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
  tr_ch_id_type      dl_amr_a_trch_id;
  tr_ch_id_type      dl_amr_b_trch_id;
  tr_ch_id_type      dl_amr_c_trch_id;
  cctrch_id_type     dl_amr_a_cctrch_id;
  cctrch_id_type     dl_amr_b_cctrch_id;
  cctrch_id_type     dl_amr_c_cctrch_id;
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
#ifdef FEATURE_MVS
  mvs_amr_mode_type  amr_mode;
  mvs_scr_mode_type   scr_mode;
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type mode_type; /* contains the codec mode that mvs needs to be configured with*/
#endif /*FEATURE_VOC_AMR_WB*/
  mvs_amr_mode_type  dl_amr_mode; /* contains DL AMR mode */
#endif

} rrcllc_amr_mapping_info_type;

/*This stucture is used for internal use*/
/* This structure conatins the transport channel if, RB-ID , RLC -ID and the Transport block sizes
  * associated with each voice class.
  */
typedef struct
{
  uint8   transport_channel_index; 
  uint16  amr_class_size;
  /* Radio Bearer Identifier (0-31) */
  rrc_RB_Identity                          rb_id;
  /* RLC logical channel buffer identifier (0-20) */
  rlc_lc_id_type                  rlc_id;
}dch_id_mapping;

#if defined(FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING)
typedef struct
{
  tr_ch_id_type      dl_amr_a_trch_id;
  tr_ch_id_type      dl_amr_b_trch_id;
  tr_ch_id_type      dl_amr_c_trch_id;
  cctrch_id_type     dl_amr_a_cctrch_id;
  cctrch_id_type     dl_amr_b_cctrch_id;
  cctrch_id_type     dl_amr_c_cctrch_id;
  mvs_amr_mode_type  dl_amr_mode; /* contains DL AMR mode */
#ifdef FEATURE_VOC_AMR_WB
  mvs_mode_type mode_type; /* contains the codec mode that mvs needs to be configured with*/
#endif /*FEATURE_VOC_AMR_WB*/
} rrc_amr_info_type;
extern rrc_amr_info_type rrc_amr_info; /* storing MVS conf values to be used W Acquires the VOC */
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING*/


#ifdef FEATURE_WCDMA_MIMO

typedef enum
{
  RRC_MIMO_INACTIVE,
  RRC_MIMO_ACTIVE
}rrc_mimo_status_enum_type;
#endif

/* Stores parameters required to setup HSDPA Action */
extern rrc_hsdpa_msg_struct_type hsdpa_msg_params;

extern rlc_size_per_rb_struct_type rlc_size_per_rb;
extern rlc_size_change_struct_type rlc_size_change_in_progress;

#ifdef FEATURE_WCDMA_HS_FACH
extern rb_mapping_info_type_ *rrc_ordered_hanging_rb;
extern rb_mapping_info_type_ *rrc_current_hanging_rb;
#endif

extern rb_mapping_info_type *rrc_ordered_hanging_rb_mapping;
extern rb_mapping_info_type *rrc_current_hanging_rb_mapping;

  /*Variable to store the UTRAN DRX CYCLE INFO*/
  extern utran_drx_cycle_type utran_drx_cycle_info;
/*===================================================================
                        DATA DECLARATIONS
=====================================================================*/
extern  ordered_config_type  config_data[2];

/********************************************************************
* ORDERED_CONFIG definition
*********************************************************************/
extern ordered_config_type *ordered_config_ptr;
extern ordered_config_type *current_config_ptr;
extern ordered_config_type *reselection_config_ptr;

extern rrcllc_oc_complete_status_type ordered_config;

extern rrcllc_config_e_type config_ptr_in_use;
extern rrcllc_transition_config_type  transition_config;


/********************************************************************
* End ORDERED_CONFIG definition
*********************************************************************/

/********************************************************************
* Transport Channel Mapping info
*********************************************************************/
extern rrcllcoc_trch_info_type dl_pch_trch_info;
extern rrcllcoc_trch_info_type dl_fach_trch_info[UE_MAX_TRCH];
extern rrcllcoc_trch_info_type dl_dch_trch_info[UE_MAX_TRCH];
extern rrcllcoc_trch_info_type ul_rach_trch_info[UE_MAX_TRCH];
extern rrcllcoc_trch_info_type ul_dch_trch_info[UE_MAX_TRCH];

/********************************************************************
* End Transport Channel Mapping info
*********************************************************************/

/********************************************************************
* Local TFS/Rate Matching Info for TFCS calc in RRCLLCOC
*********************************************************************/
/* Uplink */
extern uint16                                num_removed_tfcis;
/********************************************************************
* End Local TFS/Rate Matching Info for TFCS calc in RRCLLCOC
*********************************************************************/
/* This stores semi permamnent data for various Physical channels */
extern  rrcllc_semi_permanent_data_type  rrcllc_semi_permanent_data;

extern rrcllc_rb_list_mapped_to_dch_type rrc_rb_list_mapped_to_dch;

/* ESTABLISHED_RABS */
extern rrc_established_rabs_type rrc_est_rabs;

/* Flag to indicate that RRC is in charge of the Vocoder */
extern boolean rrciho_mvs_control_flag ;


extern rrcllc_prach_sel_info_type *  prach_sel_info[UE_MAX_PRACH];
#ifdef FEATURE_WCDMA_DC_HSDPA
#define NUM_OF_SEC_CARRIERS_2C 1
#define NUM_OF_SEC_CARRIERS_3C 2
#endif


/*===================================================================
*                       FUNCTION PROTOTYPES
====================================================================*/
/*====================================================================
FUNCTION: rrcllc_dequeue_cmd()

DESCRIPTION:
  This function dequeues commands fromLLC command queue
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.

=====================================================================*/
void rrcllc_dequeue_cmd
(
  void
);
/*====================================================================
FUNCTION       get_mac_d_hfn_cucnf()

DESCRIPTION    This function takes a pointer to the cu_confirm_ptr for R5
                         and extract mac_d_hfn field.

DEPENDENCIES   None

RETURN VALUE   value of mac_d_hfn if present else, invalid value

====================================================================*/
uint32 get_mac_d_hfn_cucnf(rrc_CellUpdateConfirm_r5_IEs *cu_confirm_ptr);

/*====================================================================
FUNCTION       get_mac_d_hfn()

DESCRIPTION    This function takes a pointer to the dl_dpch_info
                         and extract mac_d_hfn field.

DEPENDENCIES   None

RETURN VALUE   Success or failure if validation fails or succeds,
                        allocation of pointer is the responsibility of caller.

====================================================================*/

uecomdef_status_e_type  get_mac_d_hfn(rrc_DL_DPCH_InfoCommon_r4 *dpch_ie_ptr, 
                                      uint32 *mac_d_val);



/*
* SCCPCH TrBlk delivery enable functions.
*/
/*====================================================================
FUNCTION: rrcllc_set_fach_included()

DESCRIPTION:
  This function sets the fach_included flag in
  rrcllc_semi_permanent_data to the boolean value as indicated
  by the passed parameter.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
extern  void  rrcllc_set_fach_included(boolean status);

/*====================================================================
FUNCTION: rrcllc_set_pch_included()

DESCRIPTION:
  This function sets the pch_included flag in
  rrcllc_semi_permanent_data to the boolean value as indicated
  by the passed parameter.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
extern  void  rrcllc_set_pch_included
(
  boolean status
);


 /*====================================================================
FUNCTION: rrcllc_set_ctch_included()

DESCRIPTION:
  This function sets the pch_included flag in
  rrcllc_semi_permanent_data to the rrcllc_ctch_status_e_type value as indicated
  by the passed parameter.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_set_ctch_included
(
  rrcllc_ctch_status_e_type status
);

/*====================================================================
FUNCTION: rrcllc_set_pch_included_for_sccpch_with_ctch()

DESCRIPTION:
  This function sets the pch_included_for_sccpch_with_ctch flag in
  rrcllc_semi_permanent_data to the boolean value as indicated
  by the passed parameter.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_set_pch_included_for_sccpch_with_ctch
(
  boolean status
);



#ifdef FEATURE_UMTS_PDCP

/*====================================================================
FUNCTION: rrcllc_init_pdcp_parms()

DESCRIPTION:
  This function initializes all PDCP related information.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_pdcp_parms(ordered_config_type *config_ptr);

#endif /* FEATURE_UMTS_PDCP*/

/*
* Ordered Config functions.
*/
/*====================================================================
FUNCTION: rrcllc_init_ordered_and_current_config_ptrs()

DESCRIPTION:
  This function initializes the ordered_config_ptr and current_config_ptr
  to the two Configuration data structures.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  This function should only be called at poserup. For all other times,
  set_ordered_config and clear_ordered_config will update the
  ordered_config_ptr and current_config_ptrs.
====================================================================*/
void rrcllc_init_ordered_and_current_config_ptrs(void);

void  rrcllc_init_config_data(
  ordered_config_type *config_ptr,
  rrc_state_e_type  rrc_state
  );

/*====================================================================
FUNCTION: rrcllc_init_ordered_config_status_and_data()

DESCRIPTION:
  This function initializes the ordered_config database. It sets up
  the ordered_config_ptr and current_config_ptr, as well as
  initializing all data.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_ordered_config_status_and_data(
  rrc_state_e_type  rrc_state
);

void rrcllc_init_ordered_config_mac_info(
  rrcllc_config_e_type config_type
);

/*====================================================================
FUNCTION: rrcllc_init_before_msg_processing()

DESCRIPTION:
  This function initializes global variables which need to be initialized before message
  processing.

DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_before_msg_processing (void);

/*====================================================================
FUNCTION: rrcllc_copy_cc_to_oc

DESCRIPTION:
  This function copies the current config and places it in ordered
  config. It also updates the ordered_config pointer.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_cc_to_oc
(
  void
);

/*====================================================================
FUNCTION: rrcllc_copy_oc_to_cc

DESCRIPTION:
  This function copies the Ordered config and places it in Current
  config. It also updates the current_config pointer.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_oc_to_cc
(
  void
);

/*====================================================================
FUNCTION: rrcllc_copy_sib_to_oc

DESCRIPTION:
  This function takes a pointer to the serving cell data base for
  SIB 5 and copies all applicable IEs for RLC, MAC and L1 and places
  it into an internal format from the format received and processed
  over-the-air.

DEPENDENCIES:
  SIB 6 is currently not supported.

RETURN VALUE:
  None.

SIDE EFFECTS:
  If SIB 6 is available, SIB 6 data will instead be used in place of
  SIB 5 data. In other words, the correct SIB data will be used.
====================================================================*/
uecomdef_status_e_type rrcllc_copy_sib_to_oc(rrc_state_e_type next_rrc_state);

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
  void *ie_ptr, 
  rrc_msg_ver_enum_type rel_ver
);


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
rrcllc_update_oc_with_tfc_subset_list(void  *ie_ptr);

/*====================================================================
FUNCTION: rrcllc_update_oc_with_srb5()

DESCRIPTION:
  This function updates ordered_config with SRB#5 information such as 
  RB ID & transport channel ID.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_with_srb5
(
  rrc_SRB_InformationSetup_r5 * srb_setup_r5_ptr,
  rrc_RB_Identity                  rb_id
);


/*====================================================================
FUNCTION: rrcllc_update_oc_with_srb5_rel6_ie()

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
rrcllc_update_oc_with_srb5_rel6_ie
(
  rrc_SRB_InformationSetup_r6 * srb_setup_r6_ptr, 
  rrc_RB_Identity rb_id
);

/*============================================================================
FUNCTION: rrcllc_get_predefined_dl_common_info_srb5

DESCRIPTION: This function updates the DL common info received in Handover to 
             Utran message and the SRB5 Default config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type 
rrcllc_get_predefined_dl_common_info_srb5
(
  rrc_DL_CommonInformation        *dl_common_info_ptr,
  rrc_DL_CommonInformationPost    *dl_common_info_post_ptr,
  rrc_DL_DPCH_InfoCommon_r4 *predef_phych,
  rrc_U_RNTI_Short *new_urnti_ptr
);  

/*============================================================================
FUNCTION: rrcllc_get_predefined_ul_dpch_info_srb5

DESCRIPTION: This function updates the UL DPCH info received in Handover to 
             Utran message and the SRB5 Default config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type 
rrcllc_get_predefined_ul_dpch_info_srb5
(
  rrc_UL_DPCH_Info *ul_dpch_info_ptr,
  rrc_UL_DPCH_InfoPostFDD *ul_dpch_info_post_ptr,
  rrc_UL_DPCH_Info_r5 *predef_phych
) ;


/*====================================================================
FUNCTION: rrcllc_copy_sib_to_oc_for_sccpch_with_ctch()

DESCRIPTION:
  This function is responsible for getting the SCCPCH info from SIB
  and updating of relevant Ordered Config data structures.

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS/FAILURE
  (For now, SUCCESS is always returned)

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_copy_sib_to_oc_for_sccpch_with_ctch
(
  rrc_state_e_type  next_rrc_state
);

#ifdef FEATURE_UPDATE_SIB7_IN_FACH
/*====================================================================
FUNCTION: rrcllc_update_configs_with_sib7_info_in_fach

DESCRIPTION:
  This is called when the OC/CC/TOC's Uplink interference variable 
  needs to be updated with the latest value received by SIB.

DEPENDENCIES:
  None

RETURN VALUE:
  FAILURE -> If update of SIB7 interference is unsuccessful
  SUCCESS -> Otherwise.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_configs_with_sib7_info_in_fach
(
  void
);
#endif  /*FEATURE_UPDATE_SIB7_IN_FACH*/


/*====================================================================
FUNCTION: rrcllc_copy_oc_with_msg_ie

DESCRIPTION:
  This function figures out which RRC message was received and
  updates ordered_config with the latest IEs recv'd by one of
  the RRC procedures (i.e. an over-the-air message). This is valid
  only for Downlink DCCH and CCCH messages.

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS if everything is valid, FAILURE otherwise.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_copy_msg_ie_to_oc
(
  uint32 dl_sdu_num,
  void *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_get_dl_um_ccch_rlc_size()

DESCRIPTION:
  This is called when the RLC size for an downlink Unacknowledged Mode
  CCCH Logical Channel is required.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The size of the RLC UM channel.

SIDE EFFECTS:
  Note that currently all logical channels mapped to a Transport
  channel must have the same TrBLK size. Therefore, by extracting
  the RLC size of the first Transport Format, this will apply to
  all Transport Formats for all logical channels and types mapped to
  this TrCH, except for PCH/FACH, where it is the second index.
====================================================================*/
uint32 rrcllc_get_dl_um_ccch_rlc_size(void);

/*====================================================================
FUNCTION: rrcllc_print_trans_chnl_rate()

DESCRIPTION:
  This is called to print the rate of each transport channel in CC and OC
  in order to see how rate has changed.

DEPENDENCIES:
  None.

RETURN VALUE:
  void.
====================================================================*/
void rrcllc_print_trans_chnl_rate(void);

/*====================================================================
FUNCTION: rrcllc_get_ctch_drx_info()

DESCRIPTION:
  This is called when DRX info for CTCH is required.

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
void rrcllc_get_ctch_drx_info
(
  uint8 *frame_offset,
  uint16 *alloc_period
);

/*====================================================================
FUNCTION: rrcllc_get_ctch_rlc_size()

DESCRIPTION:
  This is called when the RLC size for an downlink Unacknowledged Mode
  CTCH Logical Channel is required.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The size of the RLC UM channel.

SIDE EFFECTS:
  Note that currently all logical channels mapped to a Transport
  channel must have the same TrBLK size. Therefore, by extracting
  the RLC size of the first Transport Format, this will apply to
  all Transport Formats for all logical channels and types mapped to
  this TrCH, except for PCH/FACH, where it is the second index.
====================================================================*/
uint32 rrcllc_get_ctch_rlc_size(void);


/*====================================================================
FUNCTION: rrcllc_get_ctch_enabled_status()

DESCRIPTION:
  Ckecks whether CTCH is enabled in mac info and returns boolean
  value based on that.
DEPENDENCIES:
  None.

RETURN VALUE:
  boolean

SIDE EFFECTS:
====================================================================*/
boolean rrcllc_get_ctch_enabled_status(void);
/*====================================================================
FUNCTION: rrcllc_get_bmc_schedul_info()

DESCRIPTION:
  This is called when the period of BMC scheduling message is required.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_get_bmc_schedul_info(uint16 *schedul_msg_period);

/*====================================================================
FUNCTION: rrcllc_get_dl_ded_rlc_size()

DESCRIPTION:
  This is called when the RLC size for a downlink dedicated RLC
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
uint32 rrcllc_get_dl_ded_rlc_size
(
  rrc_state_e_type  rrc_state,
  rrc_RB_Identity  rb_id
);

/*====================================================================
FUNCTION: rrcllc_get_ul_ded_rlc_size()

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
uint32 rrcllc_get_ul_ded_rlc_size
(
  rrc_state_e_type  rrc_state,
  rrc_RB_Identity  rb_id
);
/*====================================================================
FUNCTION: rrcllc_get_dl_ded_rate()

DESCRIPTION:
  This is called when dl rate for a particular rb is needed

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The downlink rate of the rb passed in.

SIDE EFFECTS:
  None
===================================================================*/
uint32 rrcllc_get_dl_ded_rate
(
  rrc_state_e_type  rrc_state,
  rrc_RB_Identity  rb_id
);
/*====================================================================
FUNCTION: rrcllc_get_ul_ded_rate()

DESCRIPTION:
  This is called when ul rate for a particular rb is needed

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The uplink rate of the rb passed in.

SIDE EFFECTS:
  None
===================================================================*/
uint32 rrcllc_get_ul_ded_rate
(
  rrc_state_e_type  rrc_state,
  rrc_RB_Identity  rb_id
);
/*
* The following prototypes are for Downlink OTA messages. These
* functions will update the ORDERED_CONFIG database with the new IEs.
*/
uecomdef_status_e_type rrcllc_active_set_update_ie
(
  rrc_ActiveSetUpdate_r3_IEs *msg_ptr
);
uecomdef_status_e_type rrcllc_phychan_reconfig_ie
(
  rrc_DL_DCCH_Message *dcch_ptr 
);
uecomdef_status_e_type rrcllc_rb_reconfig_ie
(
  rrc_DL_DCCH_Message *dcch_ptr 
);
uecomdef_status_e_type rrcllc_rb_release_ie
(
  rrc_RadioBearerRelease_r3_IEs *msg_ptr
);
uecomdef_status_e_type rrcllc_rb_setup_ie
(
  rrc_RadioBearerSetup_r3_IEs *msg_ptr
);
uecomdef_status_e_type rrcllc_rrc_conn_setup_ie
(
  rrc_RRCConnectionSetup_r3_IEs *msg_ptr
);
uecomdef_status_e_type rrcllc_trch_reconfig_ie
(
  rrc_DL_DCCH_Message *dcch_ptr 
);

/*
* The following prototypes don't have the rrcllc prefix, since they
* are simply to copy IE's from messages into the L1, MAC and RLC
* data structures. These functions get IEs out of SIBs 5 and 6.
* They may call other IE processign functions listed below.
*/

/*
* Physical Channel IE processing functions
*/

/* Uplink */
/*====================================================================
FUNCTION       get_prach_sys_info()

DESCRIPTION    This function takes a pointer to the PRACH system
               information list and copies all applicable IEs for
               MAC, RLC, and L1 into the ORDERED_CONFIG database
               (internal format).

DEPENDENCIES   None

RETURN VALUE   SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS   The current contents of ORDERED_CONFIG will be
               overwritten.
====================================================================*/
uecomdef_status_e_type get_prach_sys_info
(
  ordered_config_type *config_ptr,
  rrc_PRACH_SystemInformationList *prach_sys_info_ptr
   ,rrc_AdditionalPRACH_TF_and_TFCS_CCCH_List * rrc_additional_prach_TF_list 
   ,boolean is_sib6_in_use
);

/*====================================================================
FUNCTION        get_prach_rach_info()

DESCRIPITON     This function takes a pointer to the PRACH to RACH
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    The current Ordered Config data is overwritten.
====================================================================*/
uecomdef_status_e_type get_prach_rach_info
(
  l1_ul_prach_cfg_struct_type * prach_cfg_ptr,
  rrc_PRACH_RACH_Info *ie_ptr
);

/*====================================================================
FUNCTION        get_prach_partitioning()

DESCRIPITON     This function takes a pointer to the PRACH Partitioning
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    The current Ordered Config data is overwritten.
                The num_of_asc is updated to reflect the number of
                asc's rcv'd over the air.
====================================================================*/
uecomdef_status_e_type get_prach_partitioning
(
  l1_ul_asc_struct_type  *asc_info,
  l1_ul_prach_cfg_struct_type * prach_cfg_ptr,
  rrc_PRACH_Partitioning   *ie_ptr,
  uint8                    *num_of_asc
);

/*====================================================================
FUNCTION        get_persist_scaling_factors()

DESCRIPITON     This function takes a pointer to the Persistence Scaling
                Factor info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Dynamic Persistence Values from SIB 7 are then needed
                to correctly calculate the Persistence Value needed
                by MAC.
====================================================================*/
uecomdef_status_e_type get_persist_scaling_factors
(
  ordered_config_type  *config_ptr,
  rrc_PersistenceScalingFactorList  *ie_ptr
);

/*====================================================================
FUNCTION        get_ac_to_asc_mapping()

DESCRIPITON     This function takes a pointer to the AC to ASC mapping
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
uecomdef_status_e_type get_ac_to_asc_mapping
(
  ordered_config_type  *config_ptr,
  rrc_AC_To_ASC_MappingTable        *ie_ptr
);

/*====================================================================
FUNCTION        get_primary_cpich_tx_pwr()

DESCRIPITON     This function takes a pointer to the primary CPICH Tx
                Power info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
uecomdef_status_e_type get_primary_cpich_tx_pwr
(
  rrcllcoc_l1_ul_init_pwr_type  *  rrc_ul_pwr_init_parms_ptr,
  rrc_PrimaryCPICH_TX_Power pri_cpich_tx_pwr
);

/*====================================================================
FUNCTION        get_constant_value()

DESCRIPITON     This function takes a pointer to the Constant Value
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
uecomdef_status_e_type get_constant_value
(
  rrcllcoc_l1_ul_init_pwr_type  *  rrc_ul_pwr_init_parms_ptr,
  rrc_ConstantValue const_val
);

/*====================================================================
FUNCTION        get_prach_pwr_offset()

DESCRIPITON     This function takes a pointer to the PRACH Power Offset
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
uecomdef_status_e_type get_prach_pwr_offset
(
  ordered_config_type  *config_ptr,
  rrc_PRACH_PowerOffset *prach_pwr_offset
);

/*====================================================================
FUNCTION        get_rach_xmit_parms()

DESCRIPITON     This function takes a pointer to the RACH Xmit parms
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
uecomdef_status_e_type get_rach_xmit_parms
(
  ordered_config_type  *config_ptr,
  rrc_RACH_TransmissionParameters *rach_xmit_parms
);

/*====================================================================
FUNCTION        get_aich_info()

DESCRIPITON     This function takes a pointer to the AICH
                info IE and copies it to ORDERED_CONFIG.

DEPENDENCIES    None

RETURN VALUE    SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS    Previous values in Ordered Config are overwritten.
====================================================================*/
uecomdef_status_e_type get_aich_info
(
  ordered_config_type  *config_ptr,
  rrc_AICH_Info *aich_info_ptr
);

uecomdef_status_e_type get_max_allow_ul_tx_pwr
(
  rrc_MaxAllowedUL_TX_Power max_ul_tx_pwr
);
uecomdef_status_e_type get_ul_dpch_info
(
  rrc_UL_DPCH_Info *ie_ptr
);
uecomdef_status_e_type get_ul_dpch_pwr_ctrl_info
(
  rrc_UL_DPCH_PowerControlInfo *ie_ptr
);
/* Downlink */

/*============================================================================
FUNCTION: rrcllcpcie_is_hho()

DESCRIPTION:
  This function returns boolean value based on whether inter-freq or intra-freq
  HHO is ongoing. 

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - HHO in progress
  FALSE - No HHO

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_is_hho
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);
/*============================================================================
FUNCTION: rrcllcpcie_is_hho_r5()

DESCRIPTION:
  This function returns boolean value based on whether inter-freq or intra-freq
  HHO is ongoing. 

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - HHO in progress
  FALSE - No HHO

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_is_hho_r5
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r5 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);


/*====================================================================
FUNCTION       verify_sccpch_exists()

DESCRIPTION    This function takes a pointer to the SCCPCH system
               information list and RRC state, checks
               to see if at least one SCCPCH exists that has a FACH
               (for RRC states CELL_FACH and CONNECTING) or a PCH
               (for RRC states DISCONNECTED, CELL_PCH or URA_PCH)
               mapped to it.

DEPENDENCIES   None

RETURN VALUE   TRUE: SCCPCH with right type of TrCH exists.
               FALSE: SCCPCH with right kind of TrCH does NOT exist.

SIDE EFFECTS   None
====================================================================*/
boolean verify_sccpch_exists
(
  rrc_SCCPCH_SystemInformationList *sccpch_sys_info_ptr,
  rrc_state_e_type  next_rrc_state
);

/*====================================================================
FUNCTION       get_sys_info_for_sccpch_with_ctch()

DESCRIPTION    This function takes a pointer to the SCCPCH system
               information list for SIB 5, determines the state, checks
               to see if SIB 6 is also available, and selects the
               applicable SCCPCH information to use, and copies all
               applicable IEs for MAC, RLC, and L1 into the
               ORDERED_CONFIG database (internal format).

               SIB 5 must be there or else the UE cannot get pages or
               enter connected mode. This SIB contains the Inormation
               necessary to setup the SCCPCH an PRACH physical channels,
               and the PCH, FACH and RACH transport channels.

               SIB 6 is optional, and if present is only used to get
               SCCPCH information while in connected mode, i.e.
               CELL_FACH, CELL_PCH and URA_PCH.

DEPENDENCIES   rrcllc_init_ordered_config should always be called
               before a call to this function.

RETURN VALUE   SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS   This function presumes that Ordered Config is initialized
               before a call to this function. It updates all ordered
               config structures based on this assumption.
====================================================================*/
uecomdef_status_e_type get_sys_info_for_sccpch_with_ctch
(
  ordered_config_type  *config_ptr,
  rrc_SCCPCH_SystemInformationList *sccpch_sys_info_ptr,
  rrc_state_e_type  next_rrc_state
);

/*====================================================================
FUNCTION       get_sccpch_sys_info()

DESCRIPTION    This function takes a pointer to the SCCPCH system
               information list for SIB 5, determines the state, checks
               to see if SIB 6 is also available, and selects the
               applicable SCCPCH information to use, and copies all
               applicable IEs for MAC, RLC, and L1 into the
               ORDERED_CONFIG database (internal format).

               SIB 5 must be there or else the UE cannot get pages or
               enter connected mode. This SIB contains the Inormation
               necessary to setup the SCCPCH an PRACH physical channels,
               and the PCH, FACH and RACH transport channels.

               SIB 6 is optional, and if present is only used to get
               SCCPCH information while in connected mode, i.e.
               CELL_FACH, CELL_PCH and URA_PCH.

DEPENDENCIES   rrcllc_init_ordered_config should always be called
               before a call to this function.

RETURN VALUE   SUCCESS if everything is valid, FAILURE otherwise

SIDE EFFECTS   This function presumes that Ordered Config is initialized
               before a call to this function. It updates all ordered
               config structures based on this assumption.
====================================================================*/
uecomdef_status_e_type get_sccpch_sys_info
(
  ordered_config_type  *config_ptr,
  rrc_SCCPCH_SystemInformationList *sccpch_sys_info_ptr,
  rrc_state_e_type  next_rrc_state,
  boolean is_sib6_in_use
);

/*====================================================================
FUNCTION        get_tfcs_and_fach_pch_info()

DESCRIPTION     This function will return the tfcs and fach_pch
                info list to use for the sccpch identitfied by the
                index.

RETURN VALUE    SUCCESS or FAILURE

====================================================================*/
uecomdef_status_e_type get_tfcs_and_fach_pch_info
(
  rrc_SCCPCH_SystemInformationList *ie_ptr,
  rrc_FACH_PCH_InformationList **fach_pch_infolist_to_use,
  rrc_TFCS *tfcs_to_use,
  uint8 index
);

/*====================================================================
FUNCTION        get_sccpch_index()

DESCRIPTION     This function will determine which SCCPCH to read from
                SIB 5, and returns this index.

DEPENDENCIES    The U-RNTI should have been read and filled in
                Ordered Config

RETURN VALUE    The index (starting at 1) of the SCCPCH to read. An
                index of zero indicates a problem reading the
                SCCPCH_SYS_INFO IE from Sib 5 or Sib 6.

SIDE EFFECTS    This function also sets the imsi_div_pch_cnt(IMSI/K)
                parameter for L1 to perform the DRX calculations.
====================================================================*/
int get_sccpch_index
(
  ordered_config_type  *config_ptr,
  rrc_SCCPCH_SystemInformationList *ie_ptr,
  rrc_state_e_type                next_rrc_state
);

/*====================================================================
FUNCTION        get_sccpch_index_for_sccpch_with_ctch()

DESCRIPTION     This function will determine which SCCPCH to read for
                ctch from SIB 5/6, and returns this index.

DEPENDENCIES
RETURN VALUE    The index (starting at 1) of the SCCPCH to read. An
                index of zero indicates a problem reading the
                SCCPCH_SYS_INFO IE from Sib 5 or Sib 6.

SIDE EFFECTS
====================================================================*/
uint8 get_sccpch_index_for_sccpch_with_ctch
(
  rrc_SCCPCH_SystemInformationList *ie_ptr
);
uecomdef_status_e_type get_sccpch_info
(
  rrc_SecondaryCCPCH_Info       *ie_ptr,
  l1_sccpch_parms_struct_type   *psccpch,
  l1_dl_ctfc_info_struct_type   *ctfc_info,
  l1_dl_cctrch_info_struct_type *cctrch_info
);
/*====================================================================
FUNCTION        rrcllc_copy_ul_tfcs_info_to_dl_tfcs()

DESCRIPTION     This function copied TFCS information from Uplink TFCS
                in Current Config to Downlink TFCS information in Ordered
                Config

DEPENDENCIES
RETURN VALUE    None

SIDE EFFECTS
====================================================================*/
void rrcllc_copy_ul_tfcs_info_to_dl_tfcs
(
  void
);


uecomdef_status_e_type get_pich_info
(
  rrc_PICH_Info *ie_ptr,
  l1_pich_parms_struct_type *ppich
);
uecomdef_status_e_type get_tx_diversity_mode_info
(
  rrc_TX_DiversityMode tx_div_mode,
  l1_dl_common_info_struct_type *dl_common
);
uecomdef_status_e_type get_dl_dpch_common_info
(
  rrc_DL_DPCH_InfoCommon *ie_ptr,
  l1_dl_dpch_common_info_struct_type *common_info,
  l1_dl_cctrch_info_struct_type *cctrch_info,
  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);
uecomdef_status_e_type get_dl_dpch_info_for_each_rl
(
  rrc_DL_DPCH_InfoPerRL *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan,
  uint8 rl_idx,
  boolean is_new_rl
);
uecomdef_status_e_type get_dl_info_common_for_all_rl
(
  rrc_DL_CommonInformation *ie_ptr,
  l1_dl_common_info_struct_type *dl_common,
  l1_dl_cctrch_info_struct_type *l1_dl_cctrch_parms,
  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);
/*====================================================================
FUNCTION        get_dl_rl_idx_to_update()

DESCRIPTION     This function retrieves the RL index for a given
                scrambling code in a Downlink information for each
                radio link IE

DEPENDENCIES    NONE

RETURN VALUE    RL index if found, UE_MAX_RL otherwise.

SIDE EFFECTS
====================================================================*/

uint8 get_dl_rl_idx_to_update
(
  rrc_PrimaryScramblingCode pscr,
  OSS_UINT32 modeSpecificInfo_fdd_info_present,
  l1_dl_phychan_db_struct_type *dl_phychan
);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*====================================================================
FUNCTION        get_dl_rl_idx_to_update()

DESCRIPTION     This function retrieves the RL index for a given
                scrambling code in a Downlink information for each
                radio link IE

DEPENDENCIES    NONE

RETURN VALUE    RL index if found, UE_MAX_RL otherwise.

SIDE EFFECTS
====================================================================*/
uint8 get_sec_dl_rl_idx_to_update
(
  rrc_PrimaryScramblingCode pscr,
  l1_dl_phychan_db_struct_type *dl_phychan
);
#endif /* FEATURE_WCDMA_DC_HSUPA */
/*====================================================================
FUNCTION: rrc_get_rl_idx_for_psc

DESCRIPTION:
  This function returns RL index in OC for a given PSC

DEPENDENCIES:
  None.

RETURN VALUE:
  RL index in OC.

SIDE EFFECTS:
  None.
====================================================================*/
uint8 rrc_get_rl_idx_for_psc
(
  uint16 psc
);

/*====================================================================
FUNCTION: get_cell_id_per_rl_list

DESCRIPTION:
  This function fills cell_id_per_rl for given Cell_Ids and their PSCs.
  This is used by all procedures except ASU procedure.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_cell_id_per_rl_list
(
  rrc_CellIdentity_PerRL_List *ie_ptr_cell_id_PerRL_List,
  rrc_DL_InformationPerRL_List *ie_ptr_dl_info_perRL_List
);

/*====================================================================
FUNCTION: get_cell_id_per_rl_list_for_asu

DESCRIPTION:
  This function fills cell_id_per_rl for given Cell_Ids and their PSCs.
  This is used by ASU procedure only.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type get_cell_id_per_rl_list_for_asu
(
  rrc_CellIdentity_PerRL_List *ie_ptr_cell_id_PerRL_List,
  rrc_RL_AdditionInformationList *ie_ptr_RL_add_info_List
);

rrcllcoc_dl_info_per_rl_status_e_type get_dl_info_for_each_rl
(
  rrc_DL_InformationPerRL *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan,
  uint8 rl_idx,
  boolean is_new_rl
);
uecomdef_status_e_type get_radio_link_addition_info
(
  rrc_RL_AdditionInformationList *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan
);
uecomdef_status_e_type get_dl_rm_restriction_info
(
  rrc_Dl_rate_matching_restriction *ie_ptr,
  rrcllc_rm_restrict_info_struct_type *rm_restrict_info
);
void update_rm_restriction_info
(
  l1_dl_cctrch_info_struct_type *cctrch_info,
  rrcllc_dl_ctfc_info_struct_type *dl_tfcs_info,
  rrcllc_rm_restrict_info_struct_type *rm_restrict_info,
  l1_dl_ctfc_info_struct_type *tfcs_info_rrc_l1_inf
);
uecomdef_status_e_type get_rl_remove_info
(
  rrc_PrimaryCPICH_Info *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan
);
#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION        get_sec_eul_rl_remove_info()

DESCRIPTION    This function translates radio link removal information of sec Eul  from
                a radio link removal list into the RRC config database. This
                function is only called from an Active Set Update Message.

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    The RRC config database as pointed to by the input parameters
                will be updated.
===========================================================================*/
uecomdef_status_e_type get_sec_eul_rl_remove_info
(
  rrc_PrimaryCPICH_Info *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan
);
#endif /* FEATURE_WCDMA_DC_HSUPA */
uecomdef_status_e_type get_rl_add_info
(
  rrc_RL_AdditionInformation *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan,
  boolean *duplicate_add_attempted
);
uecomdef_status_e_type get_frequency_info
(
  rrc_FrequencyInfo *ie_ptr,
  rrc_RRC_StateIndicator next_state
);
uecomdef_status_e_type get_dl_info_per_rl
(
  rrc_DL_InformationPerRL_List *ie_ptr,
  rrc_RRC_StateIndicator next_state
);
void rrcllc_init_dl_rl_info
(
  ordered_config_type *config_ptr,
  uint8 idx
#ifdef FEATURE_WCDMA_DC_HSUPA
  ,  boolean is_sec_eul
#endif /* FEATURE_WCDMA_DC_HSUPA */

);

/*
* Transport Channel IE processing functions
*/
/* Uplink */
uint8 get_tfs_num_tb
(
  rrc_NumberOfTransportBlocks *ie_ptr
);
uint32 get_tfs_ded_rlc_size
(
  rrc_DedicatedDynamicTF_Info *ie_ptr
);
uint32 get_tfs_com_rlc_size
(
    rrc_CommonDynamicTF_Info    *ie_ptr
);
uecomdef_status_e_type get_ul_info_common_for_all_trch
(
  rrc_UL_CommonTransChInfo *ie_ptr,
  rrc_state_e_type  next_rrc_state
);
uecomdef_status_e_type get_ul_added_reconfig_trch
(
  rrc_UL_AddReconfTransChInformation * ie_ptr
);
uecomdef_status_e_type get_ul_deleted_trch
(
  rrc_UL_TransportChannelIdentity * ie_ptr
);

/*====================================================================
FUNCTION: FIND_TRCH_ID_IN_CONFIG_TRCH_LIST_R5

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean find_trch_id_in_config_trch_list_r5
(
  rrc_DL_AddReconfTransChInfoList_r5 * dl_trch_add_reconfig_ptr,
  tr_ch_id_type trch_id
);

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
);

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
);

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
);

uecomdef_status_e_type get_ul_semi_static_transport_format_info
(
  rrc_SemistaticTF_Information        *ie_ptr,
  l1_ul_semi_static_parms_struct_type *oc_ssp_ptr
);
void    compute_ul_tfis_from_ctfc
(
    mac_ul_tfcs_type    *tfcs_ptr
);
void rrcllc_init_ul_tfi_table
(
  ordered_config_type *config_ptr
);
void rrcllc_init_ul_tfs_table
(
  ordered_config_type *config_ptr,
  uint8 index
);
void rrcllc_init_rlc_size_info
(
  tr_ch_id_type trch_id
);
/* Downlink */
uecomdef_status_e_type get_dl_semistatic_tf_info
(
  rrc_SemistaticTF_Information *ie_ptr,
  l1_dl_trch_info_struct_type  *dl_semistatic_tf_info
);

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
);

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
);

uecomdef_status_e_type get_dl_tfs_dedicated_trch
(
  rrc_DedicatedTransChTFS     *ie_ptr,
  l1_dl_trch_info_struct_type *dl_tfs_info,
  boolean                      multiple_logical_ch
);
uecomdef_status_e_type get_dl_tfs_common_trch
(
  rrc_CommonTransChTFS        *ie_ptr,
  l1_dl_trch_info_struct_type *dl_tfs_info
);
uecomdef_status_e_type get_dl_transport_format_set
(
  rrc_TransportFormatSet      *ie_ptr,
  l1_dl_trch_info_struct_type *dl_ded_tfs_info,
  boolean                      multiple_logical_ch,
  l1_dl_trch_info_struct_type *dl_com_tfs_info
);
uecomdef_status_e_type get_dl_info_common_for_all_trch
(
  rrc_DL_CommonTransChInfo *ie_ptr,
  rrc_UL_CommonTransChInfo *ul_ptr,
  rrc_state_e_type  next_rrc_state
);
uecomdef_status_e_type get_dl_added_reconfig_trch
(
  rrc_DL_AddReconfTransChInformation * ie_ptr,
  rrc_UL_AddReconfTransChInfoList * ul_ptr,
  boolean                         tm_signalling_allowed
);
uecomdef_status_e_type get_dl_added_reconfig_trch_2
(
  rrc_DL_AddReconfTransChInformation2 * ie_ptr,
  rrc_UL_AddReconfTransChInfoList * ul_ptr
);
uecomdef_status_e_type get_dl_deleted_trch
(
  rrc_DL_TransportChannelIdentity * ie_ptr
);
void rrcllc_init_dl_tfs_table
(
  ordered_config_type  *config_ptr,
  uint8 index
);

/*
* Transport channel mapping prototypes.
*/

/*====================================================================
FUNCTION: rrcllc_init_dl_pch_trch_idx()

DESCRIPTION:
  This function initializes the RRC DL PCH TRCH Data base. All DL
  PCH TRCH IDs are marked as available for use.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_pch_trch_idx(ordered_config_type* config_ptr);

/*====================================================================
FUNCTION: rrcllc_get_dl_trch_idx()

DESCRIPTION:
  This function manages DL PCH TrCH Indexes for the UE.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL PCH TrCH Index that can be used.

SIDE EFFECTS:
  None.
====================================================================*/
uint8 rrcllc_get_dl_pch_trch_idx(
  ordered_config_type  *config_ptr,
  tr_ch_id_type transportChannelIdentity);


/*====================================================================
FUNCTION: rrcllc_get_dl_pch_trch_idx_for_sccpch_with_ctch()

DESCRIPTION:
  This function manages DL PCH TrCH Indexes for the UE.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL PCH TrCH Index that can be used.

SIDE EFFECTS:
  None.
====================================================================*/
uint8 rrcllc_get_dl_pch_trch_idx_for_sccpch_with_ctch(
  ordered_config_type  *config_ptr,
  tr_ch_id_type transportChannelIdentity);


/*====================================================================
FUNCTION: rrcllc_clear_dl_pch_trch_idx()

DESCRIPTION:
  This function clears a DL PCH TrCH Index for later UE use.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates a DL PCH TrCH Index was cleared
                             FAILURE indicates a DL PCH TrCH Index was not cleared

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_clear_dl_pch_trch_idx(void);

/* FACH */
/*====================================================================
FUNCTION: rrcllc_init_dl_fach_trch_idx()

DESCRIPTION:
  This function initializes the RRC DL FACH TRCH Data base. All DL
  FACH TRCH IDs are marked as available for use.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_fach_trch_idx_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_get_dl_fach_trch_idx()

DESCRIPTION:
  This function gets a DL FACH TrCH Index for a given TrCH ID. If an
  index already exists, it is returned. Otherwise a new one is
  allocated for this TrCH ID.
  If a new TrCH index is allocated, TrCH count in MAC, L1 and local
  data for TrCH ID and reference type is updated. The number of
  DL transport channels is also incremented in local, L1 and MAC
  structures.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL FACH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no DL FACH TrCH Indexes are available.
====================================================================*/
uint8 rrcllc_get_dl_fach_trch_idx(
  ordered_config_type  *config_ptr,
  tr_ch_id_type  trch_id,
  uint8 pch_trch_idx
);

/*====================================================================
FUNCTION: rrcllc_get_dl_fach_trch_idx_for_sccpch_with_ctch()

DESCRIPTION:
  This function gets a DL FACH TrCH Index for a given TrCH ID. If an
  index already exists, it is returned. Otherwise a new one is
  allocated for this TrCH ID.
  If a new TrCH index is allocated, TrCH count in MAC, L1 and local
  data for TrCH ID and reference type is updated. The number of
  DL transport channels is also incremented in local, L1 and MAC
  structures.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL FACH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no DL FACH TrCH Indexes are available.
====================================================================*/
uint8 rrcllc_get_dl_fach_trch_idx_for_sccpch_with_ctch(
  ordered_config_type  *config_ptr,
  tr_ch_id_type  trch_id,
  uint8 pch_trch_idx,
  boolean ctch_indicator
);

/*====================================================================
FUNCTION: rrcllc_clear_dl_fach_trch_idx()

DESCRIPTION:
  This function clears a DL FACH TrCH Index for later UE use.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates a DL FACH TrCH Index was cleared
                             FAILURE indicates a DL FACH TrCH Index was not cleared

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_clear_dl_fach_trch_idx(uint8 trch_idx);

/* RACH */
/*====================================================================
FUNCTION: rrcllc_init_ul_rach_trch_idx()

DESCRIPTION:
  This function initializes the RRC UL RACH TRCH Data base. All UL
  RACH TRCH IDs are marked as available for use.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_ul_rach_trch_idx_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_get_ul_rach_trch_idx()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
====================================================================*/
uint8 rrcllc_get_ul_rach_trch_idx_for_prach_db
(
  prach_config_type *prach_config_ptr,
  rrcllc_prach_sel_info_type * prach_sel_info_ptr,
  tr_ch_id_type  trch_id
);

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
    uint8                     trch_idx
);

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
    rrcllc_prach_sel_info_type  *prach_sel_info_ptr, /* Transport Format Combination Set IE Pointer */
    rrc_TFCS        *ie_ptr
);

/*====================================================================
FUNCTION: rrcllc_get_ul_rach_trch_idx()

DESCRIPTION:
  This function gets a UL DCH TrCH Index for a given TrCH ID. If an
  index already exists, it is returned. Otherwise a new one is
  allocates for this TrCH ID.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A UL RACH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no DL RACH TrCH Indexes are available.
  If a new TrCH index is allocated, TrCH count in MAC, L1 and local
  data is incremented.
====================================================================*/
uint8 rrcllc_get_ul_rach_trch_idx
(
  ordered_config_type *config_ptr,
  tr_ch_id_type  trch_id
);

/*====================================================================
FUNCTION: rrcllc_clear_ul_rach_trch_idx()

DESCRIPTION:
  This function clears a UL RACH TrCH Index for later UE use.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates a UL RACH TrCH Index was cleared
                             FAILURE indicates a UL RACH TrCH Index was not cleared

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_clear_ul_rach_trch_idx(uint8 trch_idx);

/* DL DCH */
/*====================================================================
FUNCTION: rrcllc_init_dl_dch_trch_idx()

DESCRIPTION:
  This function initializes the RRC DL DCH TRCH Data base. All DL
  DCH TRCH IDs are marked as available for use.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_dch_trch_idx_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_allocate_dl_dch_trch_idx()

DESCRIPTION:
  This function allocates a DL DCH TrCH Index.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL DCH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no DL DCH TrCH Indexes are available.
====================================================================*/
uint8 rrcllc_allocate_dl_dch_trch_idx(void);

/*====================================================================
FUNCTION: rrcllc_deallocate_dl_dch_trch_idx()

DESCRIPTION:
  This function deallocates a DL DCH TrCH Index for later UE use.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates a DL DCH TrCH Index
                             was cleared
                             FAILURE indicates a DL DCH TrCH Index
                             was not cleared
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_deallocate_dl_dch_trch_idx(uint8 trch_idx);

/* UL DCH */
/*====================================================================
FUNCTION: rrcllc_init_ul_dch_trch_idx()

DESCRIPTION:
  This function initializes the RRC UL RACH TRCH Data base. All UL
  RACH TRCH IDs are marked as available for use.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_ul_dch_trch_idx_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_allocate_ul_dch_trch_idx()

DESCRIPTION:
  This function manages UL DCH TrCH Indexes for the UE.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A UL DCH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no UL DCH TrCH Indexes are available.
====================================================================*/
uint8 rrcllc_allocate_ul_dch_trch_idx(void);

/*====================================================================
FUNCTION: rrcllc_deallocate_ul_dch_trch_idx()

DESCRIPTION:
  This function clears a UL DCH TrCH Index for later UE use.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates a UL DCH TrCH Index was cleared
                             FAILURE indicates a UL DCH TrCH Index was not cleared

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_deallocate_ul_dch_trch_idx(uint8 trch_idx);

/*====================================================================
FUNCTION: rrcllc_init_rlc_parms()

DESCRIPTION:
  This function initializes the Number of channels and Number of
  entities in RLC parameters to 0.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_rlc_parms(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_rnti_info()

DESCRIPTION:
  This function initializes the RNTI info in Uplink and Downlink
  MAC parameter data to NO RNTI VALID.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_rnti_info(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_l1_dl_dpch_chan_info()

DESCRIPTION:
  This function initializes the Downlink DPCH related channel info
  that goes to L1.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_l1_dl_dpch_chan_info(ordered_config_type  *config_ptr);

#ifdef FEATURE_MAC_I
/*====================================================================
FUNCTION: rrcllc_check_edch_fixed_ul_size_configured()

DESCRIPTION:
  This function checks if edch mapping and fixed size is configured
  the rb id specified

DEPENDENCIES:
  None.

RETURN VALUE:
  boolean


SIDE EFFECTS:

====================================================================*/
boolean rrcllc_check_edch_fixed_ul_size_configured
(
  uint32 rb_id
);
/*====================================================================
FUNCTION: rrcllc_check_edch_flexible_ul_size_configured_dch_fach_transition()

DESCRIPTION:
  This function checks if edch mapping and flexible size is configured
  the rb id. For DCH to FACH case on a hsrach cell 
  it checks in OC instead of config ptr.
  This is because TOC doesnt have HSRACH mappings.

DEPENDENCIES:
  None.

RETURN VALUE:
  boolean

SIDE EFFECTS:

====================================================================*/
boolean rrcllc_check_edch_flexible_ul_size_configured_est_rabs
(
  ordered_config_type *config_ptr,
  uint32 rb_id
);
/*====================================================================
FUNCTION: rrcllc_check_edch_flexible_ul_size_configured()

DESCRIPTION:
  This function checks if edch mapping and flexible size is configured
  the rb id specified

DEPENDENCIES:
  None.

RETURN VALUE:
  boolean

SIDE EFFECTS:

====================================================================*/
boolean rrcllc_check_edch_flexible_ul_size_configured
(
  ordered_config_type *config_ptr,
  uint32 rb_id
);
/*====================================================================
FUNCTION: rrcllc_get_edch_ul_rlc_flexible_params()

DESCRIPTION:
  This function returns the UL RLC parameters configured for 
  flexible size for the specified rb id

DEPENDENCIES:
  None.

RETURN VALUE:
  void

SIDE EFFECTS:

====================================================================*/
void rrcllc_get_edch_ul_rlc_flexible_params
(
  ordered_config_type *config_ptr,
  uint32 rb_id,
  rlc_ul_flexi_size_info_type *rlc_flexi_ul_size
);
#endif /* FEATURE_MAC_I */
/*===========================================================================

FUNCTION          rrcllcpcie_convert_asn1_oss_bitstring_to_uint8

DESCRIPTION       
  This function translates oss-bitstring to uint8 and retruned the computed uint8 value. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
uint8 rrcllcpcie_convert_asn1_oss_bitstring_to_uint8
(
  ASN1BitStr32  *bitstring_ptr
);
/*============================================================================
FUNCTION: rrcllc_sort_ref_e_tfci_list()

DESCRIPTION:
  This function sort reference E-TFCIs in ascending order
      
DEPENDENCIES:
  NONE

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
 void  rrcllc_sort_ref_e_tfci_list(
   l1_e_dpdch_info_struct_type *e_dpdch_info_ptr
);
/*============================================================================
FUNCTION: rrcllcpcie_convert_asn1_e_dch_max_ch_codes()

DESCRIPTION:
  This function converts the ENUM type of max-channelisation codes in the 
  received in reconfig message to actual internally defined enumration.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  happy bit delay condition in ms

SIDE EFFECTS:
=============================================================================*/
 uecomdef_status_e_type rrcllcpcie_convert_asn1_e_dch_max_ch_codes
(
  rrc_E_DPDCH_MaxChannelisationCodes rcvd_max_ch_codes,
  l1_e_dpdch_info_struct_type *e_dpdch_info_ptr    
);
/*============================================================================
FUNCTION: rrcllcpcie_convert_asn1_minreduced_e_dpdch_gainfactor()

DESCRIPTION:
  This function converts the ENUM type of minReduced_E_DPDCH_GainFactor 
  received in reconfig message to actual internally defined enumration.

DEPENDENCIES:
  None.

RETURN VALUE:
  minReduced_E_DPDCH_GainFactor
  
SIDE EFFECTS:
=============================================================================*/
l1_e_min_bed_enum_type rrcllcpcie_convert_asn1_minreduced_e_dpdch_gainfactor
(
  rrc_MinReduced_E_DPDCH_GainFactor minReduced_E_DPDCH_GainFactor
);
/*============================================================================
FUNCTION: rrcllcpcie_convert_asn1_happy_bit_delay_condn_to_ms()

DESCRIPTION:
  This function converts the ENUM type received in reconfig message to actual ms
  delay in happyBitDelayCondition.

DEPENDENCIES:
  None.

RETURN VALUE:
  happy bit delay condition in ms

SIDE EFFECTS:
=============================================================================*/
 uint16 rrcllcpcie_convert_asn1_happy_bit_delay_condn_to_ms
(
    rrc_HappyBit_DelayCondition happy_bit_delay_condn
);
/*============================================================================
FUNCTION: rrcllc_update_l1_e_req_mask_for_e_ul_info()

DESCRIPTION:
  This function updates the e_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_e_req_mask_for_e_ul_info
(
  boolean e_ul_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_mac_eul_action()

DESCRIPTION:
  This function updates the mac_eul_action to be sent to MAC.
  This function is called whenever any of the EUL related parameters used by MAC 
  is modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_mac_eul_action
(
  boolean mac_e_es_info_present
);


#ifdef FEATURE_WCDMA_DC_HSUPA

/*============================================================================
FUNCTION: rrcllc_update_mac_sec_eul_action()

DESCRIPTION:
  This function updates the mac_sec_eul_action to be sent to MAC.
  This function is called whenever any of the SEC EUL related parameters used by MAC 
  is modified by reconfig message.


DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_mac_sec_eul_action
(
  boolean mac_e_es_info_present
);

/*====================================================================
FUNCTION: rrcllc_print_cmac_ul_config_dchsupa()

DESCRIPTION:
  This function prints  cmac config of dc hsupa 

DEPENDENCIES:
  None.
RETURN VALUE:
  None
SIDE EFFECTS:
  None
====================================================================*/

void rrcllc_print_cmac_ul_config_dchsupa
(
  mac_ul_config_type *mac_config_info
);

#endif /* FEATURE_WCDMA_DC_HSUPA */
/*====================================================================
FUNCTION: rrc_print_cphy_setup()

DESCRIPTION:
  This function prints  cphy config of 3c hsdpa 

DEPENDENCIES:
  None.
RETURN VALUE:
  None
SIDE EFFECTS:
  None
====================================================================*/
void rrc_print_cphy_setup(l1_setup_cmd_type *l1_cmd_ptr);
/*============================================================================
FUNCTION: rrcllc_update_l1_e_req_mask_for_e_dl_info()

DESCRIPTION:
  This function updates the e_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related DL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_e_req_mask_for_e_dl_info
(
  boolean e_dl_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_e_dl_channel_mask_for_e_active_set_rl_to_reconfig()

DESCRIPTION:
  This function will set all E-DCH active set DL RL's bit mask to reconfig, This is 
  currenlty used in TTI reconfiguration, where L1 needs to reconfigure all DL RL's 
  in E-DCH active set.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_e_dl_channel_mask_for_e_active_set_rl_to_reconfig
(
  void
);
/*====================================================================
FUNCTION: rrcllc_init_mac_e_info()

DESCRIPTION:
  This function initializes mac e info stored in rrc data-base
  
DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_e_info(ordered_config_type *config_ptr);

#ifdef FEATURE_WCDMA_DC_HSUPA

/*====================================================================
FUNCTION: rrcllc_init_mac_sec_eul_e_info()

DESCRIPTION:
  This function initializes sec mac e info stored in rrc data-base
  
DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_sec_eul_e_info(ordered_config_type *config_ptr);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*====================================================================
FUNCTION: rrcllc_init_l1_e_dl_info()

DESCRIPTION:
  This function initializes l1_e_dl_info variables.
  Some of the variables needs to be touched only if UE is in idle state or
  transitioning to idle state.
  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_e_dl_info
(
  boolean is_idle,
  boolean is_hho,
  ordered_config_type *config_ptr  
);
#ifdef FEATURE_WCDMA_DC_HSUPA

/*====================================================================
FUNCTION: rrcllc_init_l1_sec_e_dl_info()

DESCRIPTION:
  This function initializes l1_sec_e_dl_info variables.
  Some of the variables needs to be touched only if UE is in idle state or
  transitioning to idle state.
  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_sec_e_dl_info
(
  boolean is_idle,
  boolean is_hho,
  ordered_config_type *config_ptr  
);
#endif /* FEATURE_WCDMA_DC_HSUPA */
/*====================================================================
FUNCTION: rrcllc_init_mac_ul_edch_info_ie()

DESCRIPTION:
  This function initializes 'mac_e_config.e_dch_info' variables (i.e. UL-EDCH
  physical layer parameters).
  This function should be called whenever UE leaves DCH state.  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_ul_edch_info_ie
(
  ordered_config_type *config_ptr
);

#ifdef FEATURE_WCDMA_DC_HSUPA


/*====================================================================
FUNCTION: rrcllc_reset_mac_and_l1_e_info_on_llc_cfg_cnf()

DESCRIPTION:
  This function resets the sec eul  MAC and L1 variables on receipt of LLC CONFIG CNF 
  by any procedure

  
DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_reset_mac_and_l1_sec_e_info_on_llc_cfg_cnf
(
void
);

/*====================================================================
FUNCTION: rrcllc_init_mac_sec_ul_edch_info_ie()

DESCRIPTION:
  This function initializes 'mac_sec_eul_config' variables (i.e. UL-EDCH
  physical layer parameters).
  This function should be called whenever UE leaves DCH state.  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_sec_ul_edch_info_ie
(
  ordered_config_type *config_ptr
);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*====================================================================
FUNCTION: rrcllc_init_l1_e_ul_info()

DESCRIPTION:
  This function initializes l1_e_ul_info variables
  Some of the variables needs to be touched only if UE is in idle state or
  transitioning to idle state.
  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_e_ul_info
(
  boolean is_idle,
  ordered_config_type *config_ptr  
);
#ifdef FEATURE_WCDMA_DC_HSUPA

/*====================================================================
FUNCTION: rrcllc_init_l1_sec_e_ul_info()

DESCRIPTION:
  This function initializes l1_sec_e_ul_info variables.
  Some of the variables needs to be touched only if UE is in idle state or
  transitioning to idle state.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_sec_e_ul_info
(
  ordered_config_type *config_ptr  
);



/*====================================================================
FUNCTION: rrcllc_init_l1_dl_dpch_chan_info()

DESCRIPTION:
  This function initializes the Downlink sec FDPCH related channel info
  that goes to L1.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_l1_dl_sec_fdpch_chan_info(ordered_config_type  *config_ptr);

#endif /* FEATURE_WCDMA_DC_HSUPA */
/*====================================================================
FUNCTION: rrcllc_reset_mac_and_l1_e_info_on_llc_cfg_cnf()

DESCRIPTION:
  This function resets the MAC and L1 variables on receipt of LLC CONFIG CNF 
  by any procedure
  
DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_reset_mac_and_l1_e_info_on_llc_cfg_cnf(void);
/*====================================================================
FUNCTION: rrcllc_init_e_mac_dflow()

DESCRIPTION:
  This function initializes E-DCH mac-d flow information.  To initialize
  all info, pass second parameter as true.  Else, pass index to be
  initialized

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:

====================================================================*/
void rrcllc_init_e_dch_rbmapping_info
(
  ordered_config_type  *config_ptr,
  uint8  e_mac_d_flow_idx
);
/*====================================================================
FUNCTION: rrcllc_get_e_mac_dflow_idx()

DESCRIPTION:
  This function gets a EDCH mac-d flow Index for the specified mac-d flow. 
  If the the mac-d flow already existed, its index is returned. Otherwise a 
  new index is allocated for the mac-d ID. If a new mac-d flowindex is allocated,
  e_mac_d_flow count in MAC structure is updated.

DEPENDENCIES:
  None.

RETURN VALUE:
  Index value if successful else
  UE_MAX_MAC_D_FLOW if no index is available

SIDE EFFECTS:

====================================================================*/
uint8 rrcllc_get_e_mac_dflow_idx
(
  uint8  e_mac_d_flow_id
);
/*====================================================================
FUNCTION: rrcllc_update_e_dch_mapping_info_with_log_chl_id()

DESCRIPTION:
  This function updates the Uplink Mapping Info in EDCH RB Mapping
  Info with the passed Logical Channel ID.
  The correct entity is selected based on the passed RB ID.
  The EDCH RB mapping info in the passed Config data is updated.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_e_dch_mapping_info_with_log_chl_id
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity  rb_id,
  rlc_lc_id_type   rlc_ul_log_chl_id
);

/*====================================================================
FUNCTION: rrcllc_find_e_mac_d_flow_idx()

DESCRIPTION:
  This function finds the edch mac-d flow index for the E-MAC-d FLOW. 
  If not found, it allocates a new index and return to the index number.

DEPENDENCIES:
  None.

RETURN VALUE:
  E-MAC-d FLOW Index if found
  Else UE_MAX_MAC_D_FLOW.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_find_e_mac_d_flow_idx
(
  uint8  e_mac_dflow_id
);

/*====================================================================
FUNCTION: rrcllc_allocate_e_mac_d_flow_idx()

DESCRIPTION:
  Allocates a EDCH MAC-d flow Id.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A EDCH MAC-d flow Index.
  UE_MAX_MAC_D_FLOW -> Given EDCH Mac-d flow does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_allocate_e_mac_d_flow_index(void);


/*====================================================================
FUNCTION: rrcllc_fill_holes_in_e_mac_d_flow_idx()

DESCRIPTION:
  This function deletes any holes in the MAC edch mac-d flow structure.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_fill_holes_in_e_mac_d_flow_idx
(
  void
);

/*====================================================================
FUNCTION: rrcllc_get_e_dch_log_chan_mode()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified

DEPENDENCIES:
  None.

RETURN VALUE:
  Index value if successful else
  UE_MAX_MAC_D_FLOW if no index is available

SIDE EFFECTS:

====================================================================*/
uecomdef_logch_mode_e_type rrcllc_get_e_dch_log_chan_mode(uint32 rb_id);

/*============================================================================
FUNCTION: rrcllcpcie_process_ul_e_dch_information()

DESCRIPTION:
  This function updates IE "ul edch informaion" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_e_dch_information
(
  OSS_UINT32 e_dch_info_present,
  rrc_UL_EDCH_Information_r6 *e_dch_info_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*====================================================================
FUNCTION: rrcllc_init_e_dch_info_before_msg_processing()

DESCRIPTION:
  This function resets needed E-DCH variables before processing any config message

DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_e_dch_info_before_msg_processing
(
  ordered_config_type *config_ptr
);
#ifdef FEATURE_WCDMA_3C_HSDPA
/*============================================================================
FUNCTION: rrcllc_verify_contiguous_carriers

DESCRIPTION:
  This function checks whether the provided frequencies of the carriers are contiguous or not
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllc_verify_contiguous_carriers(uint32 *carrier_freq, uint8 num_carriers);
#endif
#ifdef FEATURE_WCDMA_DC_HSUPA

/*============================================================================
FUNCTION: rrcllcpcie_process_sec_e_dch_reconfig_info_r9()

DESCRIPTION:
  This function processes secondary EUL serving cell info and other cells info of secondary EUL active set
  if given by network.


RE-USABILITY: 

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_sec_e_dch_reconfig_info_r9
(
  OSS_UINT32 e_dch_reconfig_info_present,
  rrc_E_DCH_ReconfigurationInfo_SecULFrequency *e_dch_reconfig_info_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_process_sec_eul_rl_addition_info_list_r9()

DESCRIPTION:
  This function updates IE "rrc_RL_AdditionInformationList_r9 " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_sec_eul_rl_addition_info_list_r9
(
  OSS_UINT32 rl_additon_info_list_present,
  rrc_RL_AdditionInformationList_SecULFreq *rl_add_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_process_rl_removal_information_list_on_sec_eul()

DESCRIPTION:
  This function processes rl removal list and updates sec_e_dch active set information
  and adds removed rl to e_rl_removed list
      
RE-USABILITY: R99/R5/R6/R7/R8

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_rl_removal_information_list_on_sec_eul
(
  OSS_UINT32 rl_removal_info_list_present,
  rrc_RL_RemovalInformationList_SecULFreq *rl_remove_ptr,
  OSS_UINT32 rl_addition_info_list_present  
);




/*====================================================================
FUNCTION: rrcllc_evaluate_sec_e_dch_transmission_variable()

DESCRIPTION:
  This function evaluates the value of variable e_dch_transmission.

DEPENDENCIES:
  This function evaluates the value of variable sec_e_dch_transmission.

RETURN VALUE:
  boolean.

SIDE EFFECTS:
  None.
====================================================================*/
 boolean rrcllc_evaluate_sec_e_dch_transmission_variable
(
  OSS_UINT32 dc_hudpa_params_present_ota,
  rrc_RRC_StateIndicator rcvd_state_indicator
);


/*====================================================================
FUNCTION: rrcllc_update_mac_eul_action_and_l1_e_req_mask()

DESCRIPTION:
  This function updates the mac_sec_eul_action for MAC and l1_sec_e_req_mask for L1.

DEPENDENCIES:
Function is only called after processing all the IEs in a config message.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_mac_sec_eul_action_and_l1_e_req_mask
(
  void
);


/*====================================================================
FUNCTION: rrcllc_update_l1_sec_e_info_for_new_e_dch_transmission()

DESCRIPTION:
  This function updates the various masks in l1 and other information based on 
  evaluated value of sec_e_dch_transmission variable.


DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_l1_sec_e_info_for_new_e_dch_transmission
(
  rrc_RRC_StateIndicator rcvd_state_indicator
);


/*====================================================================
FUNCTION: rrcllc_init_sec_e_dch_info_before_msg_processing()

DESCRIPTION:
  This function resets needed Sec E-DCH variables before processing any config message


DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_sec_e_dch_info_before_msg_processing
(
  ordered_config_type *config_ptr
);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*============================================================================
FUNCTION: rrcllcpcie_process_rl_removal_information_list()

DESCRIPTION:
  This function processes rl removal list and updates e_dch active set information
  and adds removed rl to e_rl_removed list
      
RE-USABILITY: R99/R5/R6

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_rl_removal_information_list
(
  OSS_UINT32 rl_removal_info_list_present,
  rrc_RL_RemovalInformationList *rl_remove_ptr,
  OSS_UINT32 rl_addition_info_list_present  
);

/*============================================================================
FUNCTION: rrcllc_reset_mac_and_l1_for_hsdpa()

DESCRIPTION:
  This funtion resets MAC-HS actions. It also resets the hs_req_mask for L1.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
=============================================================================*/
void rrcllc_reset_mac_and_l1_for_hsdpa
(
  void
);
/*============================================================================
FUNCTION: rrcllc_process_serving_hsdsch_cell_info()

DESCRIPTION:
  This function updates IE "rrc_Serving_HSDSCH_CellInformation " in ordered config
  and update HS-DSCH parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllc_process_serving_hsdsch_cell_info
(
  rrc_ActiveSetUpdate_r6_IEs *msg_ptr
);

/*====================================================================
FUNCTION      rrcllc_check_if_meas_feedback_changed()

DESCRIPTION   This function checks if Meas Feedback Info has changed.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None.
====================================================================*/
void rrcllc_check_if_meas_feedback_changed
(
  rrc_Measurement_Feedback_Info* meas_feedback_ptr,
  boolean* beta_table_changed,
  boolean* dpcch_changed
);
/*====================================================================
FUNCTION      rrcllc_check_if_hsscch_changed()

DESCRIPTION   This function checks if HS-SCCH Info has changed.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
boolean rrcllc_check_if_hsscch_changed
(
  void* scch_info_ptr,
  rrc_msg_ver_enum_type msg_version
);


/*====================================================================
FUNCTION: rrcllc_is_cm_active()

DESCRIPTION:
  Determines if Compressed Mode is Active

DEPENDENCIES:
  None.

RETURN VALUE:
  True if CM is active/going to be active
  False if CM is inactive/going to be inactive

SIDE EFFECTS:
====================================================================*/
boolean rrcllc_is_cm_active(ordered_config_type  *config_ptr);

/*===========================================================================

FUNCTION rrcllc_get_cm_hsdpa_nv_item

DESCRIPTION
  This  function gets NV item NV_HSDPA_COMPRESSED_MODE_ENABLED_I and stores it in the global variable

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcllc_get_cm_hsdpa_nv_item
(
sys_modem_as_id_e_type wrrc_as_id
);


/*====================================================================
FUNCTION: rrcllc_allocate_dflow_index()

DESCRIPTION:
  Allocates a Dflow Id.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL DCH TrCH Index that was already allocated to
           given transport channel ID.
  RRCLLC_INVALID_TRCH_IDX -> Given Transport channel ID does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_allocate_dflow_index(void);

/*====================================================================
FUNCTION: rrcllc_init_hspdsch_info()

DESCRIPTION:
  This function initializes the HSDPA Information in the configuration
  database.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hspdsch_info(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_hsdpa_mac_info()

DESCRIPTION:
  This function initializes the HSDPA Information in the configuration
  database.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hsdpa_mac_info(ordered_config_type  *config_ptr);


/*====================================================================
FUNCTION: rrcllc_init_hsdpa_msg_params_struct()

DESCRIPTION:
  This function initializes the HSDPA Message Parameter structure

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hsdpa_msg_params_struct
(
  void
);


/*===========================================================================

FUNCTION rrc_set_hrnti_status

DESCRIPTION

 Sets HRNTI status in the passed pointer

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrc_set_hrnti_status
(
  ordered_config_type* config_ptr,
  rrc_hsdpa_hrnti_enum_type hrnti_status
);


/*===========================================================================

FUNCTION rrc_get_hrnti_status

DESCRIPTION

 Gets HRNTI status from CC

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/
rrc_hsdpa_hrnti_enum_type rrc_get_hrnti_status
(
  void
);


/*===========================================================================

FUNCTION rrc_set_hrnti

DESCRIPTION

 Sets HRNTI in the passed pointer

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/
void rrc_set_hrnti
(
  ordered_config_type* config_ptr,
  uint16 new_hrnti
);


/*===========================================================================

FUNCTION rrc_get_hrnti

DESCRIPTION

 Gets HRNTI from CC

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

uint16 rrc_get_hrnti
(
  void
);

/*====================================================================
FUNCTION: rrcllc_check_if_rb_mapped_on_dflow()

DESCRIPTION:
  This function checks if a RB is mapped to a DFLOW in the passed
  config ptr.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if RB is found to be mapped on a DFLOW else FALSE

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_if_rb_mapped_on_dflow
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity  rb_id
);

/*====================================================================
FUNCTION: rrcllc_check_if_hsdpa_can_be_started_reconfigured()

DESCRIPTION:
  This function checks if HSDPA can be started/reconfigured by looking at the fact that
  at least one HS Queue, one DFLOW, a HS Radio Link, a PS RAB is mapped onto
  one DFLOW and H-RNTI is present

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrcllc_check_if_hsdpa_can_be_started_reconfigured
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_set_hsdpa_l1_info_in_cmd_under_process()

DESCRIPTION:
  This function sets HSDPA Information in L1 struct in rrcllc_cmd_under_process

DEPENDENCIES:
  None.

RETURN VALUE:
void

SIDE EFFECTS:
  None.
====================================================================*/

void rrcllc_set_hsdpa_l1_info_in_cmd_under_process
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_copy_harq_info()

DESCRIPTION:
  This function copies the HARQ information.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_harq_info
(
  ordered_config_type *dest_config,
  ordered_config_type *src_config
);

/*====================================================================
FUNCTION: rrcllc_is_maci_compatible_with_hsdpa()

DESCRIPTION:
  This function checks if  MACi/is is compatible with HSDPA being configured

DEPENDENCIES:
  rrc_set_hsdpa_action must be called before calling this function.

RETURN VALUE:
  SUCCESS if MACi/is and HSDPA being configured are compatible
  FAILURE Otherwise

SIDE EFFECTS:  
====================================================================*/
uecomdef_status_e_type rrcllc_is_maci_compatible_with_hsdpa
(
  void
);

/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action()

DESCRIPTION:

  This function determines the HSDPA action and sets the parameters in L1
  structure accordingly.

DEPENDENCIES:
  None.

RETURN VALUE:
  Sucess if HSDPA action and parameters have been set
  Failure Otherwise

SIDE EFFECTS:

====================================================================*/
uecomdef_status_e_type rrcllc_determine_hsdpa_action
(
  rrc_DL_HSPDSCH_Information* hspdsch_info_ptr
);

/*====================================================================
FUNCTION      set_hsscch_info()

DESCRIPTION   This function sets the Channelization code required
              for HSPDSCH in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
uecomdef_status_e_type set_hsscch_info
(
  rrc_HS_SCCH_Info* hsscch_info_ptr
);


/*====================================================================
FUNCTION      set_meas_feedback_info()

DESCRIPTION   This function sets the Meas Feedback Information in
              L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/

uecomdef_status_e_type set_meas_feedback_info
(
  rrc_Measurement_Feedback_Info* meas_feedback_ptr
);


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
);


/*====================================================================
FUNCTION: rrcllc_set_r99_hsdpa_action()

DESCRIPTION:

  This function sets the HSDPA Action based on the contents of R99 Message.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  
====================================================================*/
void rrcllc_set_r99_hsdpa_action
(
  void
);

/*====================================================================
FUNCTION: rrc_print_emac_dflow_info_in_oc

DESCRIPTION:
  Prints the updated OC content

DEPENDENCIES:

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

void rrc_print_emac_dflow_info_in_oc(void);


/*====================================================================
FUNCTION: rrcllc_find_dflow_idx()

DESCRIPTION:
  This function finds the dflow index for the DFLOW. If not found, it allocates
  a new index and return to the index number.

DEPENDENCIES:
  None.

RETURN VALUE:
  DFLOW Index if found
  Else UE_MAX_MAC_D_FLOW.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_find_dflow_index
(
  uint8  dflow_id
);


/*====================================================================
FUNCTION: rrcllc_init_dflow_rb_mapping_info()

DESCRIPTION:
  This function initializes the Downlink RB Mapping Info table for that
  particular MAC-D Flow Identifier in MAC D Flow structure

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dflow_rb_mapping_info
(
  ordered_config_type  *config_ptr,
  uint8  ix
);


/*====================================================================
FUNCTION: rrcllc_deallocate_dflow_index()

DESCRIPTION:
  This function deallocates a DFLOW Index specified by passed pointer
  so that it can be used Later

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS/FAILURE
SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_deallocate_dflow_index
(
  uint8 dflow_index
);


/*====================================================================
FUNCTION: rrcllc_initialize_queue_info()

DESCRIPTION:
  This function deallocates a DFLOW Index specified by passed pointer
  so that it can be used Later

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS/FAILURE
SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_initialize_queue_info
(
  uint8 dflow_index
);


/*====================================================================
FUNCTION: rrcllc_rearrange_dflow_index()

DESCRIPTION:
  This function deletes any hoes in dl_dflow_index_in_use structure
  and the MAC DFLOW structure.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

void rrcllc_rearrange_dflow_index
(
  void
);


/*====================================================================
FUNCTION: rrcllc_get_dflow_index()

DESCRIPTION:
  This function gets a DFLOW Index for the specified DFLOW ID. If the
  DFLOW already existed, its index is returned. Otherwise a new index is
  allocated for the DFLOW ID. If a new DFLOW index is allocated,
  DFLOW count in MAC structure is updated.

DEPENDENCIES:
  None.

RETURN VALUE:
  Index value if successful else
  UE_MAX_MAC_D_FLOW if no index is available

SIDE EFFECTS:

====================================================================*/

uint8 rrcllc_get_dflow_index
(
  uint8  dflow_id
);


/*============================================================================
FUNCTION: rrcllc_rearrange_queue_index()

DESCRIPTION:
  This function rearranges the queue for a particular DFLOW so that the hole
  which may have been created earlier is filled up

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
void rrcllc_rearrange_queue_index
(
  void
);

/*====================================================================
FUNCTION: rrcllc_get_queue_index()

DESCRIPTION:

  This function gets a Queue Index for the specified DFLOW ID. If the
  Queue already existed, its index is returned. Otherwise a new queue index is
  allocated for the DFLOW ID. The queue count for that index is also
  incremented. The fn also fills the passed queue already present indicator
  if the queue is already present.

DEPENDENCIES:
  None.

RETURN VALUE:
  Queue Index if successfully allocated else UE_MAX_HS_QUEUE

SIDE EFFECTS:

====================================================================*/
uint8 rrcllc_get_queue_index
(
  uint8  dflow_index,
  uint32 queue_id,
  boolean* queue_present_ptr
);

/*====================================================================
FUNCTION: rrcllc_remove_dflow_queue_mapping()

DESCRIPTION:

  This function removes the association of former dflow associated
  with the queue. The dflow associated with this queue in the passed
  ptr is the new DFLOW

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:

====================================================================*/
void rrcllc_remove_dflow_queue_mapping
(
  rrc_MAC_hs_AddReconfQueue* queue_ptr
);


/*====================================================================
FUNCTION: rrcllc_init_hsdpa_l1_queue()

DESCRIPTION:
  This function initializes the HSDPA Information in the configuration
  database.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hsdpa_l1_queue
(
  ordered_config_type  *config_ptr
);

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
);
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
);

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
);


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
  rrc_DL_AddReconfTransChInfoList_r5 * dl_trch_add_reconfig_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList_r5    * dl_trch_delete_ptr
);


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
);


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
);


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
);


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
);


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
);


/*====================================================================
FUNCTION      get_ul_dpch_info_r5()

DESCRIPTION   This function translates the Uplink DPCH info common for
              all RL (radio links) IE into the RRC config database.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire translation operation is successful,
              FAILURE otherwise

SIDE EFFECTS  The RRC config database page pointed to by the input
              parameter ord_cfg is overwritten with all IE elements
              that are present.  Those IE elements that are not present
              do NOT overwrite the database values with, say, default
              values.  Any optional parameters that are present also
              cause parameter presence bits in the database to be set.
====================================================================*/
uecomdef_status_e_type get_ul_dpch_info_r5
(
  rrc_UL_DPCH_Info_r5 *ie_ptr
);


/*====================================================================
FUNCTION        get_dl_info_common_for_all_rl_r5()

DESCRIPTION     This function translates the Downlink information common
                for all radio links IE into the RRC config database.

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
uecomdef_status_e_type get_dl_info_common_for_all_rl_r5
(
  /* Pointer to the DL CommonInformation IE */
  rrc_DL_CommonInformation_r5 *ie_ptr,

  /* Pointer to the desirec DL PhyChan common information structure */
  l1_dl_common_info_struct_type *dl_common,

  /* Pointer to the DL CCTRCH parameters structure */
  l1_dl_cctrch_info_struct_type *l1_dl_cctrch_parms,

  /* Pointer to the DL CTFC parameters structure */
  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);





/*===========================================================================
FUNCTION        get_dl_info_per_rl_r5()

DESCRIPTION     This function validates and stores Downlink Information Per
                Radio Link for reconfiguration messages.

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    Ordered config is updated with the new RL info.
===========================================================================*/
uecomdef_status_e_type get_dl_info_per_rl_r5
(
  rrc_DL_InformationPerRL_List_r5 *ie_ptr,
  rrc_RRC_StateIndicator next_state
);


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
  rrc_SRB_InformationSetupList_r5 * srb_setup_head_ptr,
  rrc_RB_Identity                   rb_id
);


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
  rrc_RB_InformationSetupList_r5   * rb_setup_head_ptr
);




/*====================================================================
FUNCTION: rrcllc_rb_reconfig_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Radio Bearer Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_reconfig_ie_r5
(
  rrc_DL_DCCH_Message *dcch_ptr
);


/*====================================================================
FUNCTION: rrcllc_rb_release_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Release 5 Radio Bearer Release Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_release_ie_r5
(
  rrc_RadioBearerRelease_r5_IEs *msg_ptr
);


/*====================================================================
FUNCTION: rrcllc_rb_setup_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Radio Bearer Setup Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_setup_ie_r5
(
  rrc_RadioBearerSetup_r5_IEs *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_rrc_conn_setup_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in R5 RRC Connection Setup Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rrc_conn_setup_ie_r5
(
  rrc_RRCConnectionSetup_r5_IEs *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_trch_reconfig_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Release 5 Transport Channel Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_trch_reconfig_ie_r5
(
  rrc_DL_DCCH_Message *dcch_ptr
);

/*====================================================================
FUNCTION: rrcllc_phychan_reconfig_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Physical Channel Reconfiguration Message for Rel 5

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_phychan_reconfig_ie_r5
(
  rrc_DL_DCCH_Message *dcch_ptr
);

/*====================================================================
FUNCTION: rrcllc_cell_update_confirm_ie_r5()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Cell Update Confirm Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_cell_update_confirm_ie_r5
(
  rrc_CellUpdateConfirm_r5_IEs *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_process_non_critcal_phys_reconfig_ie_r5

DESCRIPTION:
  This function acts on the NON Critical IE for Phys Reconfig R5

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_process_non_critcal_phys_reconfig_ie_r5
(
  rrc_DL_DCCH_Message *dcch_ptr
);

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
  rrc_DL_AddReconfTransChInfoList_r4 *  dl_trch_add_reconfig2_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList    * dl_trch_delete_ptr
);


/*====================================================================
FUNCTION: rrcllc_init_hsdpa_info_before_msg_processing()

DESCRIPTION:
  This function initializes the HSDPA Action in MAC structure, forces
  the MAC Reset variable to FALSE. Also, makes sure that the HSDPA
  Action variable is set to NOOP and also initializes the HSDPA
  message param structure and rlc size per rb structure

DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hsdpa_info_before_msg_processing
(
  void
);

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
);

/*====================================================================
FUNCTION      get_ul_dpch_info_per_r4_rrc_connection()

DESCRIPTION   This function translates the Uplink DPCH info common for
              all RL (radio links) IE into the RRC config database
              for RRC Connection Setup Message.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire translation operation is successful,
              FAILURE otherwise

SIDE EFFECTS  The RRC config database page pointed to by the input
              parameter ord_cfg is overwritten with all IE elements
              that are present.  Those IE elements that are not present
              do NOT overwrite the database values with, say, default
              values.  Any optional parameters that are present also
              cause parameter presence bits in the database to be set.
====================================================================*/
uecomdef_status_e_type get_ul_dpch_info_per_r4_rrc_connection
(
  rrc_UL_DPCH_Info_r4 *ie_ptr
);


/*====================================================================
FUNCTION        get_dl_info_common_for_all_rl_per_r4_rrc_connection()

DESCRIPTION     This function translates the Downlink information common
                for all radio links IE into the RRC config database.

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
uecomdef_status_e_type get_dl_info_common_for_all_rl_per_r4_rrc_connection
(
  /* Pointer to the DL CommonInformation IE */
  rrc_DL_CommonInformation_r4 *ie_ptr,

  /* Pointer to the desirec DL PhyChan common information structure */
  l1_dl_common_info_struct_type *dl_common,

  /* Pointer to the DL CCTRCH parameters structure */
  l1_dl_cctrch_info_struct_type *l1_dl_cctrch_parms,

  /* Pointer to the DL CTFC parameters structure */
  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);

/*====================================================================
FUNCTION        get_dl_info_for_each_rl_r5_per_rrc_connection()

DESCRIPTION     This function translates the Downlink information for
                each radio link IE to the RRC config database

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
rrcllcoc_dl_info_per_rl_status_e_type get_dl_info_for_each_rl_r5_per_rrc_connection
(
  rrc_DL_InformationPerRL_r5bis *ie_ptr,

  l1_dl_phychan_db_struct_type *dl_phychan,

  uint8 rl_idx,

  boolean is_new_rl
);

/*===========================================================================
FUNCTION        get_dl_info_per_rl_r5_per_rrc_connection()

DESCRIPTION     This function validates and stores Downlink Information Per
                Radio Link for reconfiguration messages.

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    Ordered config is updated with the new RL info.
===========================================================================*/
uecomdef_status_e_type get_dl_info_per_rl_r5_per_rrc_connection
(
  rrc_DL_InformationPerRL_List_r5bis *ie_ptr,
  rrc_RRC_StateIndicator next_state
);





/*====================================================================
FUNCTION: rrcllc_process_rb_info_release_list()

DESCRIPTION:
  This function the IE rrc_RB_InformationReleaseList and updates the ordered
  config accordingly

REUSABILITY: R99/R5/R6

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_process_rb_info_release_list 
(
  rrc_RB_InformationReleaseList * rb_info_rel_list_ptr
);

/*====================================================================
FUNCTION: rrcllc_rb_release_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 5 Radio Bearer Release Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_release_ie_r6
(
  rrc_RadioBearerRelease_r6_IEs *msg_ptr
  ,rrc_RadioBearerRelease_r6 *rb_rel_r6_non_crit_ext

);

/*====================================================================
FUNCTION: rrcllc_rb_setup_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Setup Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_setup_ie_r6
(
  rrc_RadioBearerSetup_r6_IEs *msg_ptr
  ,rrc_RadioBearerSetup_r6 *rb_setup_r6_non_crit_ext

);
/*====================================================================
FUNCTION: rrcllc_rb_reconfig_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_reconfig_ie_r6
(
  rrc_RadioBearerReconfiguration_r6_IEs *msg_ptr
  ,rrc_RadioBearerReconfiguration_r6 *rb_reconfig_r6_non_crit_ext

);
/*====================================================================
FUNCTION: rrcllc_trch_reconfig_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 6 Transport Channel Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_trch_reconfig_ie_r6
(
  rrc_TransportChannelReconfiguration_r6_IEs *msg_ptr
  ,rrc_TransportChannelReconfiguration_r6 *trch_chan_reconfig_r6_non_crit_ext

);
/*====================================================================
FUNCTION: rrcllc_phychan_reconfig_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Physical Channel Reconfiguration Message for Rel 6

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_phychan_reconfig_ie_r6
(
  rrc_PhysicalChannelReconfiguration_r6_IEs *msg_ptr
  , rrc_PhysicalChannelReconfiguration_r6 *phys_chan_reconfig_r6_non_critical_ext

);
/*====================================================================
FUNCTION: rrcllc_cell_update_confirm_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Cell Update Confirm Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_cell_update_confirm_ie_r6
(
  rrc_CellUpdateConfirm_r6_IEs *msg_ptr
  ,rrc_DL_InformationPerRL_List_v6b0ext *dl_InformationPerRL_List_v6b0ext

);
/*====================================================================
FUNCTION: rrcllc_rrc_conn_setup_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in R5 RRC Connection Setup Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rrc_conn_setup_ie_r6
(
  rrc_RRCConnectionSetup_r6_IEs *msg_ptr
  ,rrc_RRCConnectionSetup_r6 * rrc_conn_setup_r6_non_crit_ext

);

/*====================================================================
FUNCTION: rrcllc_active_set_update_ie()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Active Set Update Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_active_set_update_ie_r6
(
  rrc_ActiveSetUpdate_r6_IEs *msg_ptr
  ,rrc_ActiveSetUpdate_r6 *asu_r6_non_crit_ext

);

/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action_r6()

DESCRIPTION:

  This function determines the HSDPA action and sets the parameters in L1
  structure accordingly.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  Sucess if HSDPA action and parameters have been set
  Failure Otherwise

SIDE EFFECTS:
  
====================================================================*/
uecomdef_status_e_type rrcllc_determine_hsdpa_action_r6
(
  rrc_DL_HSPDSCH_Information_r6* hspdsch_info_ptr
);

/*====================================================================
FUNCTION      set_hsscch_info_r6()

DESCRIPTION   This function sets the Channelization code required
              for HSPDSCH in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
uecomdef_status_e_type set_hsscch_info_r6
(
  rrc_HS_SCCH_Info_r6* hsscch_info_ptr
);

/*====================================================================
FUNCTION      get_ul_dpch_pwr_ctrl_info_r6()

DESCRIPTION   This function translates the Uplink DPCH Power Control
              info common for all RL (radio links) IE into the RRC
              config database.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire translation operation is successful,
              FAILURE otherwise

SIDE EFFECTS  The RRC config database page pointed to by the input
              parameter ord_cfg is overwritten with all IE elements
              that are present.  Those IE elements that are not present
              do NOT overwrite the database values with, say, default
              values.  Any optional parameters that are present also
              cause parameter presence bits in the database to be set.
====================================================================*/
uecomdef_status_e_type get_ul_dpch_pwr_ctrl_info_r6
(
  rrc_UL_DPCH_PowerControlInfo_r6_fdd * fdd_ul_pwrcntrl_info
);

/*====================================================================
FUNCTION      get_ul_dpch_info_r6()

DESCRIPTION   This function translates the Uplink DPCH info IE into the RRC config database.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire translation operation is successful,
              FAILURE otherwise

SIDE EFFECTS  The RRC config database page pointed to by the input
              parameter ord_cfg is overwritten with all IE elements
              that are present.  Those IE elements that are not present
              do NOT overwrite the database values with, say, default
              values.  Any optional parameters that are present also
              cause parameter presence bits in the database to be set.
====================================================================*/
uecomdef_status_e_type get_ul_dpch_info_r6
(
  rrc_UL_DPCH_Info_r6 *ie_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_process_ul_dpch_info_r6()

DESCRIPTION:
  This function updates IE "ul dpch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_dpch_info_r6
(
  OSS_UINT32 ul_dpch_info_present,
  rrc_UL_DPCH_Info_r6 *ul_dpch_info_ptr,
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_validate_transition_to_cell_dch()

DESCRIPTION:
  This function validates whether DL common Info and DL per RL info is present or not,
  when UE is transitioned from non-cell DCH to cell-DCH in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_validate_transition_to_cell_dch
(
  OSS_UINT32 ul_dpch_info_present,
  OSS_UINT32 dl_common_info_present,
  OSS_UINT32 dl_per_rl_info_list_present,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*====================================================================
FUNCTION      get_dl_dpch_common_info_r6()

DESCRIPTION   This function translates the Downlink DPCH info common for
              all RL (radio links) IE into the RRC config database.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire translation operation is successful,
              FAILURE otherwise

SIDE EFFECTS  The RRC config database page pointed to by the input
              parameter ord_cfg is overwritten with all IE elements
              that are present.  Those IE elements that are not present
              do NOT overwrite the database values with, say, default
              values.  Any optional parameters that are present also
              cause parameter presence bits in the database to be set.
====================================================================*/
uecomdef_status_e_type get_dl_dpch_common_info_r6
(
  /* pointer to the IE */
  rrc_DL_DPCH_InfoCommon_r6 *ie_ptr,

  /* pointer to the desired config page */
  l1_dl_dpch_common_info_struct_type *common_info,

  /* pointer to the CCTrCH info structure */
  l1_dl_cctrch_info_struct_type *cctrch_info,

  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);

/*====================================================================
FUNCTION        get_dl_info_common_for_all_rl_r6()

DESCRIPTION     This function translates the Downlink information common
                for all radio links IE into the RRC config database.

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
uecomdef_status_e_type get_dl_info_common_for_all_rl_r6
(
  /* Pointer to the DL CommonInformation IE */
  rrc_DL_CommonInformation_r6 *ie_ptr,

  /* Pointer to the desirec DL PhyChan common information structure */
  l1_dl_common_info_struct_type *dl_common,

  /* Pointer to the DL CCTRCH parameters structure */
  l1_dl_cctrch_info_struct_type *l1_dl_cctrch_parms,

  /* Pointer to the DL CTFC parameters structure */
  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_common_info_r6()

DESCRIPTION:
  This function updates IE "dl common info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dl_common_info_r6
(
  OSS_UINT32 dl_common_info_present,
  rrc_DL_CommonInformation_r6 *dl_common_info_ptr,
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_info_per_rl_list_r6()

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dl_info_per_rl_list_r6
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r6 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
  ,struct rrc_DL_InformationPerRL_List_v6b0ext *dl_InformationPerRL_List_v6b0ext
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_hspdsch_info_and_update_hsdpa_msg_params()

DESCRIPTION:
  This function updates IE "dl hspdsch info " in ordered config and update
  hsdpa_msg_param variable
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_hspdsch_info_and_upd_hsdpa_msg_params
(
  OSS_UINT32 dl_hspdsch_info_present,
  rrc_DL_HSPDSCH_Information_r6 *dl_hspdsch_info_ptr,
  OSS_UINT32 is_ifhho_involved,
  rrc_RRC_StateIndicator rcvd_state_indicator
);
/*============================================================================
FUNCTION: rrcllcpcie_is_hho()

DESCRIPTION:
  This function returns boolean value based on whether inter-freq or intra-freq
  HHO is ongoing. 

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - HHO in progress
  FALSE - No HHO

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_is_hho_r6
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r6 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_rl_addition_info_list_r6()

DESCRIPTION:
  This function updates IE "rrc_RL_AdditionInformationList_r6 " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_rl_addition_info_list_r6
(
  OSS_UINT32 rl_additon_info_list_present,
  rrc_RL_AdditionInformationList_r6 *rl_add_ptr
  ,struct rrc_RL_AdditionInformation_list_v6b0ext *dl_InformationPerRL_List_v6b0ext
);

/*============================================================================
FUNCTION: rrcllcpcie_process_e_dch_reconfig_info()

DESCRIPTION:
  This function processes rl removal list and updates e_dch active set information
  and adds removed rl to e_rl_removed list
      
RE-USABILITY: R99/R5/R6

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_e_dch_reconfig_info
(
  OSS_UINT32 e_dch_reconfig_info_present,
  rrc_E_DCH_ReconfigurationInfo *e_dch_reconfig_info_ptr
);

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
);

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
  rrc_SRB_InformationSetupList_r6 * src_info_setup_list_ptr
);

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
  rrc_RAB_InformationSetupList_r6 *rab_info_setup_list_ptr
);

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
  rrc_RB_InformationAffectedList_r6 * rb_info_affected_list_ptr
) ;

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
  rrc_RB_InformationReconfigList_r6 * rb_info_reconfig_list_ptr
);
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
);

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
);

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
  rrc_DL_AddReconfTransChInfoList_r5 * dl_add_reconf_trch_info_list_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r6  * ul_add_reconf_trch_info_list_ptr  
);

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
  rrc_SRB_InformationSetupList_r6 * srb_setup_head_ptr,
  rrc_RB_Identity                   rb_id
);

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
);

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
);

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
rrc_RB_InformationSetupList_r6   * rb_setup_head_ptr
);

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
  struct rrc_RAB_InformationSetupList *rab_setup_ptr
); 
/*============================================================================
FUNCTION: rrcllc_get_predefined_ul_dpch_info

DESCRIPTION:

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_get_predefined_ul_dpch_info
(
  rrc_UL_DPCH_Info *ul_dpch_info_ptr,
  rrc_UL_DPCH_InfoPostFDD *ul_dpch_info_post_ptr,
  rrc_PreDefPhyChConfiguration *predef_phych
);
/*============================================================================
FUNCTION: rrcllc_get_predefined_dl_common_info

DESCRIPTION:

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_get_predefined_dl_common_info
(
  rrc_DL_CommonInformation        *dl_common_info_ptr,
  rrc_DL_CommonInformationPost    *dl_common_info_post_ptr,
  rrc_PreDefPhyChConfiguration    *predef_phych,
  rrc_U_RNTI_Short *new_urnti_ptr
);
/*============================================================================
FUNCTION: rrcllc_get_predefined_dl_rl_list

DESCRIPTION:

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_get_predefined_dl_rl_list
(
  struct rrc_DL_InformationPerRL_List    **dl_rl_ptr,
  rrc_DL_InformationPerRL_ListPostFDD  *dl_rl_post_ptr,
  rrc_DL_CommonInformation        dl_common_info
);  
/*==========================================================
FUNCTION  : get_srb_info_to_setup_preconfig_r5()

DESCRIPTION : Get Signalling Radio Bearer Information to 
              setup for Rb-Setup with preconfiguration info.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type get_srb_info_to_setup_preconfig_r5
(
  rrc_SRB_InformationSetup_r5     *ie_ptr,
  rrc_SRB_InformationSetupList_r5 * srb_setup_head_ptr,
  rrc_RB_Identity                   rb_id
);


/*====================================================================
FUNCTION     delete_unused_trch()

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
);





/*====================================================================
FUNCTION: rrcllc_get_trch_id_mapped_to_rb()

DESCRIPTION:
  This function returns the Transport chanel id mapped to a RB

DEPENDENCIES:
  None

RETURN VALUE:
  tr_ch_id_type

SIDE EFFECTS:
  None.
====================================================================*/
tr_ch_id_type rrcllc_get_trch_id_mapped_to_rb
(
  rrc_RB_Identity  rb_id
);



/*====================================================================
FUNCTION: rrcllc_init_l1_sccpch_chan_info()

DESCRIPTION:
  This function initializes the SCCPCH related channel info
  that goes to L1.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_l1_sccpch_chan_info(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_l1_sccpch_for_ctch_chan_info()

DESCRIPTION:
  This function initializes the SCCPCH (used for ctch) related channel info
  that goes to L1.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_l1_sccpch_for_ctch_chan_info(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_bch_info()

DESCRIPTION:
  This function initializes the BCH transport channel related info.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_bch_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_pch_info()

DESCRIPTION:
  This function initializes the PCH transport channel related info.
  This also includes releasing the PCH TrCH index and setting "PCH
  Included" to FALSE for LLC.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_pch_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_fach_info()

DESCRIPTION:
  This function initializes the FACH transport channel related info.
  This also includes releasing all FACH TrCH indices, setting
  "FACH Included" to FALSE for LLC and initializing the FACH RB
  Mapping Info.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_fach_info(ordered_config_type *config_ptr, boolean init_rb_mapping_info);

/*====================================================================
FUNCTION: rrcllc_init_dl_dch_info()

DESCRIPTION:
  This function initializes the DL DCH transport channel related info.
  This also includes releasing all DL DCH TrCH indices and
  initializing the DL DCH RB Mapping Info.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_dch_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_dl_cctrch_info_for_sccpch_with_ctch()

DESCRIPTION:
  This function initializes the DL CCTrCH Info for L1.
  This includes setting the number of transport channels for this
  CCTrCH to 0 and initializing the Transport Format Set.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_cctrch_info_for_sccpch_with_ctch(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_cbs_info()

DESCRIPTION:
  This function initializes the CBS related info.  Sets the flag that
  indicated to L1 that ctch info is present to FALSE.


DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_cbs_info(ordered_config_type *config_ptr);


/*====================================================================
FUNCTION: rrcllc_init_dl_ctfcs_for_sccpch_with_ctch()

DESCRIPTION:
  This function initializes the Downlink TFI table. This is
  called whenever the DL CCTrCH type changes (i.e. a state change),
  or when a TrCH is deleted..

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_ctfcs_for_sccpch_with_ctch(ordered_config_type *config_ptr);


/*====================================================================
FUNCTION: rrcllc_init_dl_cctrch_info()

DESCRIPTION:
  This function initializes the DL CCTrCH Info for L1.
  This includes setting the number of transport channels for this
  CCTrCH to 0 and initializing the Transport Format Set.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_cctrch_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_dl_ctfcs()

DESCRIPTION:
  This function initializes the Global Downlink TFI table. This is
  called whenever the DL CCTrCH type changes (i.e. a state change),
  or when a TrCH is deleted..

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_ctfcs(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_l1_ul_dpch_chan_info()

DESCRIPTION:
  This function initializes the UL DPCH Physical Channel Related Info.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_l1_ul_dpch_chan_info(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_rach_info()

DESCRIPTION:
  This function initializes the RACH Transport Channel Related Info.
  Since there can be only one Uplink Physical Channel at any time,
  initializing RACH info also results in initialization of:
  - MAC UL CCTrCH type to INVALID
  - Number of TrCHs in Uplink TFCS
  - Number of TrCHs in L1 Uplink CCTrCH parameters
  - Uplink TFCS table
  - Uplink TFS for all TrCHs.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_rach_info(ordered_config_type *config_ptr, boolean init_rb_map_info);

/*====================================================================
FUNCTION: rrcllc_init_ul_dch_info()

DESCRIPTION:
  This function initializes the UL DCH Transport Channel Related Info.
  Since there can be only one Uplink Physical Channel at any time,
  initializing UL DCH info also results in initialization of:
  - MAC UL CCTrCH type to INVALID
  - Number of TrCHs in Uplink TFCS
  - Number of TrCHs in L1 Uplink CCTrCH parameters
  - Uplink TFCS table
  - Uplink TFS for all TrCHs.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_ul_dch_info(ordered_config_type *config_ptr);

/* TrCH Reconfig */
/*====================================================================
FUNCTION: rrcllc_ul_trch_reconfig()

DESCRIPTION:
  This checks for UL TrCH reconfigurations, and then executes them.

DEPENDENCIES:
  This uses the Add Reconfig Info List structure.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates either no reconfig,
                             or reconfig is successful
                             FAILURE indicates an unsuccessful reconfig

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_ul_trch_reconfig
(
  rrc_UL_AddReconfTransChInfoList ie_ptr
);


/* TFS Ue Capability Check */
/*====================================================================
FUNCTION: rrcllc_check_ul_tfs_cnt()

DESCRIPTION:
  This function verifies that the number of Transport Format sets
  received in an OTA Message is within the UE Capabilities.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates # of UL TFS < UE Cap
                             FAILURE indicates # of UL TFS > UE Cap

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_check_ul_tfs_cnt(void);

/*====================================================================
FUNCTION: rrcllc_check_dl_tfs_cnt()

DESCRIPTION:
  This function verifies that the number of Transport Format sets
  received in an OTA Message is within the UE Capabilities.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates # of DL TFS < UE Cap
                             FAILURE indicates # of DL TFS > UE Cap

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_check_dl_tfs_cnt(void);

/*
* Radio Bearer IE processing functions
*/
uecomdef_status_e_type get_srb_info_to_setup
(
  rrc_SRB_InformationSetup   *ie_ptr,
  rrc_SRB_InformationSetupList2 * srb_setup_head_ptr,
  rrc_RB_Identity                  rb_id
);

uecomdef_status_e_type get_rb_info_to_setup
(
  rrc_RB_InformationSetup    *ie_ptr,
#ifdef FEATURE_UMTS_PDCP
  rrc_CN_DomainIdentity cn_Domain,
#endif /* FEATURE_UMTS_PDCP*/
  rrc_RB_InformationSetupList * rb_setup_head_ptr
);

uecomdef_status_e_type get_rb_info_affected
(
  rrc_RB_InformationAffected *ie_ptr
);
uecomdef_status_e_type get_rb_info_to_release
(
  rrc_RB_Identity            *ie_ptr
);
uecomdef_status_e_type get_rb_info_to_reconfig
(
  rrc_RB_InformationReconfig *ie_ptr
);
void rrcllc_init_fach_rb_mapping_info
(
  ordered_config_type  *config_ptr,
  uint8 index
);
void rrcllc_init_dl_dch_rb_mapping_info
(
  ordered_config_type  *config_ptr,
  uint8 index
);
void rrcllc_init_rach_rb_mapping_info
(
  ordered_config_type  *config_ptr
);
void rrcllc_init_ul_dch_rb_mapping_info
(
  ordered_config_type  *config_ptr,
  uint8 index
);

#ifdef FEATURE_WCDMA_HS_FACH
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
);
#endif

void unmap_rb
(
  rrc_RB_Identity rb_id,
  rrcllc_unmap_rb_e_type config_db
);
void remove_dl_ded_logch_entry
(
  mac_dl_ded_logch_config_type *ded_logch_array,
  uint8                         entry_to_remove
);
void remove_ul_ded_logch_entry
(
  mac_ul_ded_logch_config_type *ded_logch_array,
  uint8                         entry_to_remove
);
void remove_rlc_size_list_entry
(
  rrcllcoc_rlc_size_list_type *rlc_size_list_array,
  uint8                        entry_to_remove
);

/*====================================================================
FUNCTION: rrcllc_reset_rach_rlc_size_lists()

DESCRIPTION:
  This function initializes the RACH rlc size lists in the RLC
  size restriction info of the given database.
  Pointer to the database is passed as a parameter.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_reset_rach_rlc_size_lists(
  rrcllcoc_rlc_size_list_type *rec,
  uint8                        entry_to_remove,
  boolean remove_all
);

/*====================================================================
FUNCTION        select_asc_from_ac()

DESCRIPITON     This function selects the ASC from the UE's access
                service class. It uses the AC to ASC mapping to come up
                with the chosen ASC.

DEPENDENCIES    AC to ASC mapping should be present.
                UE's Access class should be available.

RETURN VALUE    None

SIDE EFFECTS    None
====================================================================*/
void  select_asc_from_ac(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION        get_persist_scaling_default_factors()

DESCRIPITON     This function sets default values of persistence
                scaling factors for all ASCs.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
====================================================================*/
void  get_persist_scaling_default_factors(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_get_dl_dch_trch_idx()

DESCRIPTION:
  This function gets a DL DCH TrCH Index for a given TrCH ID. If an
  index already exists, it is returned. Otherwise a new one is
  allocates for this TrCH ID.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL DCH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no DL DCH TrCH Indexes are available.
  If a new TrCH index is allocated, TrCH count in MAC, L1 and local
  data is incremented.
====================================================================*/
uint8 rrcllc_get_dl_dch_trch_idx(tr_ch_id_type  trch_id);


/*====================================================================
FUNCTION: rrcllc_get_ul_dch_trch_idx()

DESCRIPTION:
  This function gets a DL DCH TrCH Index for a given TrCH ID. If an
  index already exists, it is returned. Otherwise a new one is
  allocates for this TrCH ID.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A DL DCH TrCH Index that can be used.

SIDE EFFECTS:
  trch_idx = 0xFF -> indicates no DL DCH TrCH Indexes are available.
  If a new TrCH index is allocated, TrCH count in MAC, L1 and local
  data is incremented.
====================================================================*/
uint8 rrcllc_get_ul_dch_trch_idx(tr_ch_id_type  trch_id);


void  rrcllc_fill_holes_in_dl_dch_trch_idx(void);

void rrcllc_copy_dl_dch_trch_info_from_one_to_next_index(
      uint8 dest_trch_idx,
      uint8 src_trch_idx
);

uecomdef_status_e_type rrcllc_delete_dl_dch_trch_index
(
  uint8   trch_idx
);

uint8  rrcllc_find_dl_fach_trch_idx(void);
uint8  rrcllc_find_dl_dch_trch_idx(tr_ch_id_type  trch_id);

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
  rrc_DL_DeletedTransChInfoList   * dl_trch_delete_ptr
);

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
  rrc_DL_AddReconfTransChInfo2List  *dl_trch_add_reconfig2_ptr,
    /* local DL TrCH Drop Pointer */
  rrc_DL_DeletedTransChInfoList  *  dl_trch_delete_ptr
);

void  rrcllc_fill_holes_in_ul_dch_trch_idx(void);

/*====================================================================
FUNCTION: rrcllc_check_if_rlc_mode_is_am()

DESCRIPTION:
  This function checks whether given rb_id is of AM RLC type or not.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - rb_id is of AM RLC type
  FALSE - rb_id isn't of AM RLC type

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_if_rlc_mode_is_am
(
  rrc_RB_Identity rb_id
);

/*====================================================================
FUNCTION: rrcllc_check_if_rlc_mode_is_um()

DESCRIPTION:
 This function queries CC and finds out whether a given RB rb_id is of 
 UM RLC mode or not.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - RB rb_id is of UM RLC mode.
  FALSE - RB rb_id isn't of UM RLC mode.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_if_rlc_mode_is_um
(
  rrc_RB_Identity rb_id
);
/*====================================================================
FUNCTION: rrcllc_init_rb_list_mapped_to_dch()

DESCRIPTION:
  This function initializes global variables rrc_rb_list_mapped_to_dch

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_rb_list_mapped_to_dch
(
  void
);

/*====================================================================
FUNCTION: rrcllc_fill_rb_list_mapped_to_dch()

DESCRIPTION:
  This function fills the given RB-id in the global structure 
  rrc_rb_list_mapped_to_dch. It does so only if the given RB-id isn't 
  already present in rrc_rb_list_mapped_to_dch.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_fill_rb_list_mapped_to_dch
(
  rrc_RB_Identity rb_id
#ifdef FEATURE_MAC_I
 ,rrc_direction_enum_type  direction
#endif /* FEATURE_MAC_I */
);

/*====================================================================
FUNCTION: rrcllc_get_rb_for_rb_list_mapped_to_dch()

DESCRIPTION:
  This function calls rrcllc_fill_rb_list_mapped_to_dch() to fill the 
  given RB in rrc_rb_list_mapped_to_dch, if RLC PDU size for that RB has 
  changed. The third parameter is to notify whether RLC PDU size will be 
  checked for UL or DL.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_get_rb_for_rb_list_mapped_to_dch
(
  tr_ch_id_type trch_id, 
  rrc_state_e_type next_state, 
  rrc_direction_enum_type direction
);

/*====================================================================
FUNCTION: rrcllc_get_am_rlc_size_for_rb()

DESCRIPTION:
  This function returns RLC PDU size for a given rb_id and direction.
  It used OC database for it.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32  - The size of the RLC channel.

SIDE EFFECTS:
  None
===================================================================*/
uint32 rrcllc_get_am_rlc_size_for_rb
(
  rrc_RB_Identity  rb_id, 
  rrc_direction_enum_type direction
);

void rrcllc_copy_ul_dch_trch_info_from_one_to_next_index(
      uint8 dest_trch_idx,
      uint8 src_trch_idx
);

uecomdef_status_e_type rrcllc_delete_ul_dch_trch_index
(
  uint8   trch_idx
);

/*====================================================================
FUNCTION: rrcllc_find_ul_dch_trch_idx()

DESCRIPTION:
  This function looks for and returns a transport channel index for a
  given UL DCH transport channel ID. It returns a valid index only
  if it was already assigned to the given DCH transport channel ID.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A UL DCH TrCH Index that was already allocated to
           given transport channel ID.
  RRCLLC_INVALID_TRCH_IDX -> Given Transport channel ID does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_find_ul_dch_trch_idx(ordered_config_type *config_ptr,
                                   tr_ch_id_type  trch_id);

uint8  rrcllc_find_ul_rach_trch_idx(void);

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
  rrc_UL_AddReconfTransChInfoList * ul_trch_add_reconfig_ptr,
  rrc_UL_DeletedTransChInfoList   * ul_trch_delete_ptr
);

/*====================================================================
FUNCTION: set_dl_rm_restriction_info_to_invalid

DESCRIPTION:
  This function invalidates the passes RM restriction info. This is
  needed in all Non-DCH states.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  set_dl_rm_restriction_info_to_invalid(
    /* RM restriction info structure pointer */
  rrcllc_rm_restrict_info_struct_type *rm_restrict_info
);

/*====================================================================
FUNCTION: rrcllc_init_mac_dl_ptrs()

DESCRIPTION:
  This function is responsible for initializing pointers to FACH, DCH
  and DSCH info in MAC DL Parameter structure to the storage
  space for FACH, DCH and DSCH info respectively.
  NOTE: Once mac_dl_config_type is changed to have the data
  structures in place of pointers, this function will not be
  needed anymore.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_mac_dl_ptrs(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_mac_ul_ptrs()

DESCRIPTION:
  This function is responsible for initializing pointers to
  TFCS and CCTrCH configuration in MAC UL Parameter structure to the
  TFCS and CCTrCH Config info in ordered_config_type respectively.
  NOTE: Once mac_ul_config_type is changed to have the data
  structures in place of pointers, this function will not be
  needed anymore.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_mac_ul_ptrs(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_l1_dl_ptrs()

DESCRIPTION:
  This function is responsible for initializing pointers to
  transport channel info (for each TrCH) in l1_dl_cctrch_parms
  to dedicated or common TrCH info in ordered config type
  depending on the passed RRC state. It also initializes the
  pointers to Transport format info inside dedicated or common TrCH info
  to dedicated or common transport format info respectively.
  This also initializes pointers to CTFC info in l1_dl_ctfc_parms
  to ctfc_info in ordered_config_type for all CTFCs. The pointers
  to TFI table inside ctfc_info are initialized to TFI table inside
  the local dl_ctfc in ordered_config_type.

  NOTE: Once l1_dl_cctrch_parms, l1_dl_trch_info_struct_type,
  l1_dl_ctfc_parms and l1_dl_ctfc_entry_info_struct_type are changed
  to have the data structures in place of pointers, this function will
  not be needed anymore.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_l1_dl_ptrs
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_init_l1_ul_ptrs()

DESCRIPTION:
  This function is responsible for initializing pointers to
  semi static and gain factor info in L1 UL CCTrCH Parameter structure
  to the info stored locally in ordered_config_type. It also
  initializes pointers to ASC Info in L1 UL Channel Parameters.

  NOTE: Once l1_ul_cctrch_parms and l1_ul_chan_parms are changed
  to have the data structures in place of pointers, this function will
  not be needed anymore.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_l1_ul_ptrs(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_rlc_ptrs()

DESCRIPTION:
  This function is responsible for initializing pointers to
  AM Info in RLC AM parameter structure to the AM Configuration
  stored locally in ordered_config_type.

  NOTE: Once rlc_am_config_req_type is changed to have the data
  structures in place of pointers, this function will not be
  needed anymore.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_rlc_ptrs(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_activation_time()

DESCRIPTION:
  This function initializes the Activation Time to Avtivation Time
  NONE. This is equivalent to saying the Avtivation Time is "Now".

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_init_activation_time(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_init_reconfig_needed()

DESCRIPTION:
  This function initializes the Reconfig Needed structure.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_reconfig_needed(ordered_config_type  *config_ptr);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_fach_with_msg()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  in Cell_FACH state.
  It starts with copying Current Config to Ordered Config and then
  updating Ordered Config data with Message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a NOT_SUPPORTED
  status is returned.
  On successful updation, ordered_config_status is set to TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_in_fach_with_msg(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

void  rrcllc_update_oc_from_fach_to_disconnected(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_dch_to_fach_with_msg()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_DCH to Cell_FACH state.
  This function updates OC with the message contents of the message
  that transitions UE from Cell_DCH to Cell_FACH.
  The updated OC is copied to TOC to be used later in Cell_FACH and
  when coming back to Cell_DCH from Cell_FACH.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_dch_to_fach_with_msg(
  uint32            dl_sdu_num,
  void             *msg_ptr
);


/*====================================================================
FUNCTION: rrcllc_update_oc_from_connecting_to_dch()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  moving from Connecting to Cell_DCH state.
  It starts with initializing Ordered Config and then
  updating Ordered Config data with Message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a NOT_SUPPORTED
  status is returned.
  On successful updation, ordered_config_set is set to TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_connecting_to_dch(
  uint32            dl_sdu_num,
  void             *msg_ptr
);


/*====================================================================
FUNCTION: rrcllc_update_oc_from_fach_to_dch()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_FACH to Cell_DCH state.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_fach_to_dch(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_cell_pch_to_fach()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_PCH to Cell_FACH state.
  This function reads the SIBs and updates OC with SIB info.
  The BCH info is preserved from the CC to OC.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_cell_pch_to_fach(void);


/*====================================================================
FUNCTION: rrcllc_update_oc_from_dch_to_fach_with_sibs()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_DCH to Cell_FACH state.
  This function reads the SIBs and updates OC with SIB info.
  The BCH info is preserved from the CC to OC.

DEPENDENCIES:
  None

RETURN VALUE:
  uecomdef_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_from_dch_to_fach_with_sibs(void);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_fach_with_toc()

DESCRIPTION:
  This function is responsible for updating OC with TOC parameters
  immediately after transitioning to CELL_FACH.
  The CC is sopied to OC and then OC is updated with RLC and RB Mapping
  Info.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_oc_in_fach_with_toc(
  void
);


/*====================================================================
FUNCTION: rrcllc_check_dcch_fach_mapping(ordered_config_type *config_ptr)

DESCRIPTION:
  This function checks whether DCCH mapping on FACH exists in
  ordered config.

DEPENDENCIES:
  None
RETURN VALUE:
  booean
SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_dcch_fach_mapping(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_update_oc_for__fach_with_toc_for_rl_failure()

DESCRIPTION:
  This function is responsible for updating OC with TOC parameters
  immediately after transitioning to CELL_FACH.
  The CC is copied to OC and then OC is updated with RLC and RB Mapping
  Info.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_oc_in_fach_with_toc_for_rl_failure(
  void
);

/*====================================================================
FUNCTION: rrcllc_update_cc_in_fach_with_toc_for_oos_ind()

DESCRIPTION:
  This function is responsible for updating CC with TOC parameters
  immediately after transitioning to CELL_FACH.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_cc_in_fach_with_toc_for_oos_ind
(
  void
);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_dch()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  in Cell_DCH state.
  It starts with copying Current Config to Ordered Config and then
  updating Ordered Config data with Message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a NOT_SUPPORTED
  status is returned.
  On successful updation, ordered_config_set is set to TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_in_dch(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*===========================================================================
FUNCTION   rrcllc_chan_config_dch_to_fach

DESCRIPTION
  This function is resposible for determining all actions on RLC, MAC
  and L1 when going from Cell_DCH to Cell_FACH state.
  Note: This function will only setup CCCH and the rest of RLC channels
  will be hooked to the pipes later, thru' explicit commands.
  This also expects a BCCH Drop request in Channel Config Request.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
uecomdef_status_e_type
rrcllc_chan_config_dch_to_fach(rrc_cmd_type *cmd_ptr);


/*===========================================================================
FUNCTION   rrcllc_chan_config_dch_to_pch

DESCRIPTION
  This function is resposible for determining all actions on RLC, MAC
  and L1 when going from Cell_DCH to Cell_PCH or URA_PCH state.
  Note: This function will only setup CCCH and the rest of RLC channels
  will be hooked to the pipes later, thru' explicit commands.
  This also expects a BCCH Drop request in Channel Config Request.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
uecomdef_status_e_type
rrcllc_chan_config_dch_to_pch(rrc_cmd_type *cmd_ptr);


/*====================================================================
FUNCTION: rrcllc_update_oc_in_fach()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  in Cell_FACH state.
  It starts with copying Current Config to Ordered Config and then
  updating Ordered Config data with Message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a NOT_SUPPORTED
  status is returned.
  On successful updation, ordered_config_set is set to TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_in_fach(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_fach_with_sibs

DESCRIPTION:


DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type  rrcllc_update_oc_in_fach_with_sibs
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_connecting_to_fach()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  going from Connecting to Cell_FACH state.
  It starts with initializing Ordered Config and then
  updating Ordered Config data with Message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a NOT_SUPPORTED
  status is returned.
  On successful updation, ordered_config_set is set to TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_connecting_to_fach(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_copy_sccpch_info_from_sib_to_oc_in_connected_mode()

DESCRIPTION:
  This function is responsible for getting the SCCPCH info from SIB
  and updating of relevant Ordered Config data structures.

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS/FAILURE
  (For now, SUCCESS is always returned)

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_copy_sccpch_info_from_sib_to_oc_in_connected_mode
(
  rrc_state_e_type  curr_rrc_state,
  rrc_state_e_type  next_rrc_state
);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_disconnected_to_connecting()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  mving from Disconnected to Connecting state.
  It starts with initializing Ordered Config and then
  updating Ordered Config data with SIBs
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a FAILURE status is returned.

DEPENDENCIES:
  All SIBs should be read before calling this function.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_from_disconnected_to_connecting(void);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_disconnected_to_dch()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  moving from Disconnected to Cell DCH state.
  It starts with initializing Ordered Config and then
  updating Ordered Config data with message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a FAILURE status is returned.

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_disconnected_to_dch(
  uint32            dl_sdu_num,
  void             *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_init_oc_cc_in_diconnected()

DESCRIPTION:
  This function is responsible for initializing the Ordered Config
  and Current Config in Disconnecting state.

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS/FAILURE
  (For now, SUCCESS is always returned)

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_init_oc_cc_in_diconnected(void);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_pch_for_pcch_reconfig()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  in cell_pch or URA pch state.

DEPENDENCIES:
  For a PCH add/reconfig operation, all SIBs should be read before
  calling this function.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcllc_update_oc_in_pch_for_pcch_reconfig
(
  ordered_config_type *config_ptr,
  rrc_state_e_type state
);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_disconnected()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  in Disconnected state. It is based on the PCH and BCH operation type
  that is passed as a parameter.
  It starts with copying Current Config to Ordered Config and then
  updating Ordered Config data. For a PCH add/reconfig, OC is updated
  with SIBs and in the event of a failure, the Ordered Config data is
  reverted back to Current Config data.

DEPENDENCIES:
  For a PCH add/reconfig operation, all SIBs should be read before
  calling this function.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcllc_update_oc_in_disconnected
(
  ordered_config_type *config_ptr,
  rrcllc_bch_op_e_type  bch_op,
  rrcllc_nbch_op_e_type  nbch_op,
  rrcllc_pch_op_e_type  pch_op
);

/*====================================================================
FUNCTION: rrcllc_update_oc_for_serving_neighbor_bch_in_connected()

DESCRIPTION:
  This function is responsible for updating the Ordered Config
  for Serving and/or Neighbor BCH in Connected mode states.
  It is based on the BCH and NBCH operation type
  that is passed as a parameter.

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcllc_update_oc_for_serving_neighbor_bch_in_connected
(
  ordered_config_type  *config_ptr,
  rrcllc_bch_op_e_type  bch_op,
  rrcllc_nbch_op_e_type  nbch_op
);

/*====================================================================
FUNCTION: rrcllc_copy_ccch_info_from_cc_to_oc()

DESCRIPTION:
  This function is responsible for copying all CCCH related info from
  Current Config to Ordered Config.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_ccch_info_from_cc_to_oc(void);

/*====================================================================
FUNCTION: rrcllc_update_oc_for_resel()

DESCRIPTION:
  This function is responsible for updating Ordered Config due to a
  Cell reselection in all RRC states.
  It initializes all SCCPCH Info, CTFCs and L1 CCTrCH Info.
  The PCH info is not initialized so that the PCCH logical channel ID
  could be maintained. In Connecting and Connected modes, FACH
  info is not initialized
  The SCCPCH Info on new cell is read and all
  info is updated. The PCH Transport Channel ID is also updated.

DEPENDENCIES:
  Serving Cell index in SIB should be initialized before calling this
  function. Otherwise this may read SIBs from old cell.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_for_resel
(
  rrc_state_e_type  curr_rrc_state
);

/*==========================================================
FUNCTION  : get_lc_type_by_rb_id()

DESCRIPTION :   
  This function returns logical channel type based on RB id.

DEPENDENCIES: None.

RETURN VALUE: 
  Logical Channel Type.

SIDE EFFECTS: None.
============================================================*/
uecomdef_logchan_e_type get_lc_type_by_um_rb_id
(
  rrc_RB_Identity rb_id
);

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
);

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
);

/*==========================================================
FUNCTION  : get_rlc_dl_um_info ()

DESCRIPTION : 
  Processes RLC Info IE for DL UM

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
);

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
);

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
);
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
);

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
);

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_am_info()

DESCRIPTION : Deletes RLC AM info and re-arranges the RLC
              AM Info array to cover any holes.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_am_info(rrc_RB_Identity rb_id);

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_dl_tm_info()

DESCRIPTION : Decrements the count of DL TM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_dl_tm_info(rlc_lc_id_type lc_id);

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_dl_um_info()

DESCRIPTION : Decrements the count of DL UM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_dl_um_info(rlc_lc_id_type lc_id);

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_ul_tm_info()

DESCRIPTION : Decrements the count of UL TM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_ul_tm_info(rlc_lc_id_type lc_id);

/*==========================================================
FUNCTION  : rrcllc_delete_rlc_ul_um_info()

DESCRIPTION : Decrements the count of UL UM logical channels.

DEPENDENCIES: None.

RETURN VALUE: None.

SIDE EFFECTS: None.
============================================================*/
void  rrcllc_delete_rlc_ul_um_info(rlc_lc_id_type lc_id);

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
void  rrcllc_copy_ul_dch_trch_info_from_buffer(uint32 trch_idx);

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
void  rrcllc_copy_ul_dch_trch_info_to_buffer(uint32 trch_idx);

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
void  rrcllc_copy_dl_dch_trch_info_from_buffer(uint32 trch_idx);

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
void  rrcllc_copy_dl_dch_trch_info_to_buffer(uint32 trch_idx);

/*====================================================================
FUNCTION: rrcllc_update_cell_fach_dl_mapping_info_with_log_chl_id()

DESCRIPTION:
  This function updates the Downlink Mapping Info in Cell FACH RB Mapping
  Info with the passed Logical Channel ID.
  The correct entity is selected based on the passed RB ID.
  The Cell FACH RB mapping info in the passed Config data is updated.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_cell_fach_dl_mapping_info_with_log_chl_id
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity  rb_id,
  rlc_lc_id_type   rlc_dl_log_chl_id
);

/*====================================================================
FUNCTION: rrcllc_update_cell_fach_ul_mapping_info_with_log_chl_id()

DESCRIPTION:
  This function updates the Uplink Mapping Info in Cell FACH RB Mapping
  Info with the passed Logical Channel ID.
  The correct entity is selected based on the passed RB ID.
  The Cell FACH RB mapping info in the passed Config data is updated.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_cell_fach_ul_mapping_info_with_log_chl_id
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity  rb_id,
  rlc_lc_id_type   rlc_ul_log_chl_id
);

/*====================================================================
FUNCTION: rrcllc_update_toc_ul_dch_info_with_log_chl_id()

DESCRIPTION:
  This function updates the MAC UL Info in TOC with LOgical Channel ID.
  The correct entity is selected based on the passed RB ID.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_update_toc_ul_dch_info_with_log_chl_id
(
  rrc_RB_Identity  rb_id,
  rlc_lc_id_type  rlc_ul_log_chl_id
);

/*====================================================================
FUNCTION: rrcllc_update_toc_dl_dch_info_with_log_chl_id()

DESCRIPTION:
  This function updates the MAC DL Info in TOC with LOgical Channel ID.
  The correct entity is selected based on the passed RB ID.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_update_toc_dl_dch_info_with_log_chl_id
(
  rrc_RB_Identity  rb_id,
  rlc_lc_id_type  rlc_dl_log_chl_id
);


/*============================================================================
FUNCTION: set_compressed_mode_info()

DESCRIPTION:
  This function copies the compressed mode parameters from UTRAN message
  in the format required by L1

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type set_compressed_mode_info
(
  rrc_DPCH_CompressedModeInfo* rrc_compressed_mode_ptr
);


/*============================================================================
FUNCTION: find_tgpsi_index()

DESCRIPTION:
  This function finds the tgpsi index for a given compressed mode Meas

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE.

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type find_tgpsi_index
(
  uint8* utran_tgpsi,
  uint8* tgpsi_index,
  boolean* tgpsi_modify
);


/*============================================================================
FUNCTION: set_tgpsi_location

DESCRIPTION:
  This function sets the tgpsi location in action_needed array so that L1 knows
  which TGPSI's are being modified by UTRAN. If any location in this array is set
  to TRUE, then that means that L1 needs to take care of that TGPSI. If that TGPSI
  is not existing, then L1 adds it else L1 modifies it

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void set_tgpsi_location
(
  uint8* tgpsi_location
);



/*====================================================================
FUNCTION: rrcllc_modify_cm_info_in_current_config()

DESCRIPTION:
  This function modifies the compressed mode gap sequence structure
  in current config by deleting the tgpsi given by L1 on compressed
  mode run time error indication.

DEPENDENCIES:
  None

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_modify_cm_info_in_current_config
(
  l1_compressed_mode_ind_type* cm_ind_ptr
);


/*====================================================================
FUNCTION: rrcllc_send_compressed_mode_run_time_error_ind()

DESCRIPTION:
  This function constructs the RRC_CPHY_COMPRESSED_MODE_IND primitive and
  sends it to Physical Channel Reconfiguration procedure.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_send_compressed_mode_run_time_error_ind
(
 l1_compressed_mode_ind_type* cm_ptr
);


/*====================================================================
FUNCTION: rrcllc_populate_mac_rlc_size_restriction_info()

DESCRIPTION:
  This function populates the MAC ordered config structures with the
  RLC size restriction data already stored outside ordered config.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  Updates MAC data structures in ordered config.
====================================================================*/
uecomdef_status_e_type rrcllc_populate_mac_rlc_size_restriction_info
(
  ordered_config_type *config_ptr,
  rrc_state_e_type state
);


/*====================================================================
FUNCTION: rrcllc_init_dch_rlc_size_info()

DESCRIPTION:
  This function initializes the DCH RLC size restriction info for the
  given transport channel identity.  Any existing RLC size restriction
  information is cleared out.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dch_rlc_size_info(tr_ch_id_type trch_id);

/*====================================================================
FUNCTION: rrcllc_init_fach_rlc_size_info()

DESCRIPTION:
  This function initializes the FACH RLC size restriction info.
  Any existing RLC size restriction information is cleared out.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_fach_rlc_size_info(void);



/*============================================================================
FUNCTION: rrcllc_register_for_reconfig_complete()

DESCRIPTION:
  This function is called by SMC if Ordered Config is set for reconfiguration.
  This function sets a boolean to notify SMC when the reconfiguration is
  completed.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrcllc_register_for_reconfig_cnf(void);

/*============================================================================
FUNCTION: rrcllc_discard_oc()

DESCRIPTION:
  This function is called to discard the Ordered Config. This in effect
  sets the config_ptr_in_use to CURRENT CONFIG.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
void rrcllc_discard_oc(void);

#ifdef FEATURE_WCDMA_HS_FACH
/*====================================================================
FUNCTION: rrcllc_copy_cell_rach_rb_mapping_info()

DESCRIPTION:
  This function is responsible for copying all RB Mapping Info stored
  for CELL_FACH state in the Source Config, to the RB Mapping Info for
  RACH in the Destination Config.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_copy_cell_rach_rb_mapping_info(
  ordered_config_type  *dest_config,
  ordered_config_type  *src_config
  );

/*====================================================================
FUNCTION: rrcllc_fill_directed_cell_info()

DESCRIPTION:
  Function fills directed cell info from config pointer 

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_fill_directed_cell_info
(
  ordered_config_type *config_ptr,
  rrcrb_directed_cell_info_type  *directed_cell_info
);

#endif


/*====================================================================
FUNCTION: rrcllc_copy_cell_fach_rb_mapping_info()

DESCRIPTION:
  This function is responsible for copying all RB Mapping Info stored
  for CELL_FACH state in the Source Config, to the RB Mapping Info for
  FACH and RACH in the Destination Config.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_copy_cell_fach_rb_mapping_info(
  ordered_config_type  *dest_config,
  ordered_config_type  *src_config
  );



/*====================================================================
FUNCTION: rrcllc_handover_to_utran_ie()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Handover to UTRAN Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_handover_to_utran_ie
(
  rrc_HandoverToUTRANCommand *msg_ptr
);

/*====================================================================
FUNCTION: rrcllc_get_timers_and_const_in_conn_mode_for_handover

DESCRIPTION:
  This function initializes default connected mode timers and
  constants for GtoW handover.  Defaults are used here because the
  UE does not have access to SIB1 at this point.

DEPENDENCIES:
  None

RETURN VALUE:
  None.

SIDE EFFECTS:
====================================================================*/
void
rrcllc_get_timers_and_const_in_conn_mode_for_handover(void);


/*====================================================================
FUNCTION: rrcllc_clear_ordered_config_now

DESCRIPTION:
  This is called when the ORDERED_CONFIG variable needs to be cleared.
  That means the Ordered Config and Current Config pointers are swapped.

DEPENDENCIES:
  Make sure no command is under process when this is called.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_clear_ordered_config_now
(
  void
);

boolean  rrcllc_is_cmd_under_process_for_solo_bch_operation(void);

/*====================================================================
FUNCTION: rrcllc_copy_bch_info_from_cc_to_oc()

DESCRIPTION:
  This function is responsible for copying all BCH related info from
  Current Config to Ordered Config.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void  rrcllc_copy_bch_info_from_cc_to_oc(void);




/*====================================================================
FUNCTION: rrcllc_update_oc_from_dch_to_pch_with_msg()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_DCH to Cell_PCH or URA_PCH state.
  This function updates OC with the message contents of the message
  that transitions UE from Cell_DCH to Cell_PCH or URA_PCH.
  The updated OC is copied to TOC to be used later when coming back to
  Cell_FACH from Cell_PCH or URA_PCH.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_dch_to_pch_with_msg(
  uint32            dl_sdu_num,
  void             *msg_ptr,
  rrc_state_e_type  next_rrc_state
);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_dch_to_pch_with_sibs()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_DCH to Cell_PCH or URA_PCH state.
  This function reads the SIBs and updates OC with SIB info.
  The BCH info is preserved from the CC to OC.

DEPENDENCIES:
  None

RETURN VALUE:
  uecomdef_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_from_dch_to_pch_with_sibs
(
  rrc_state_e_type next_rrc_state
);

 /*====================================================================
FUNCTION: rrcllc_update_oc_from_fach_to_cell_pch_or_ura_pch()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_FACH to Cell_PCH or URA_PCH state.
  This function reads the SIBs and updates OC with SIB info.
  The BCH info is preserved from the CC to OC.

DEPENDENCIES:
  None

RETURN VALUE:
  uecomdef_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type
rrcllc_update_oc_from_fach_to_cell_pch_or_ura_pch
(
  rrc_state_e_type next_rrc_state
);

/*====================================================================
FUNCTION: rrcllc_update_oc_from_fach_to_pch_with_msg()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_FACH to Cell_PCH or URA_PCH state.
  It starts with copying Current Config to Ordered Config and then
  updating Ordered Config data with Message IEs.
  In the event of a failure, the Ordered Config data is
  reverted back to Current Config data and a NOT_SUPPORTED
  status is returned.
  On successful updation, ordered_config.set_status is set to TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  rrcllc_oc_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
rrcllc_oc_status_e_type
rrcllc_update_oc_from_fach_to_pch_with_msg(
  uint32            dl_sdu_num,
  void             *msg_ptr,
  rrc_state_e_type  next_rrc_state
);

/*====================================================================
FUNCTION: rrcllc_update_oc_for_pch_to_fach_with_toc()

DESCRIPTION:
  This function is responsible for updating OC with TOC parameters
  immediately after transitioning to CELL_FACH.
  The CC is copied to OC and then OC is updated with RLC and RB Mapping
  Info.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_oc_for_pch_to_fach_with_toc
(
  void
);

 /*====================================================================
FUNCTION: rrcllc_update_oc_from_pch_to_fach()

DESCRIPTION:
  This function is responsible for updating the Ordered Config when
  transitioning from Cell_PCH to Cell_FACH state.
  This function reads the SIBs and updates OC with SIB info.
  The BCH info is preserved from the CC to OC.

DEPENDENCIES:
  None

RETURN VALUE:
  uecomdef_status_e_type

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_update_oc_from_pch_to_fach
(
  void
);

/*====================================================================
FUNCTION: rrcllc_update_oc_in_pch_with_toc()

DESCRIPTION:
  This function is responsible for updating OC with TOC parameters
  immediately after transitioning to CELL_PCH or URA_PCH.
  The CC is copied to OC and then OC is updated with RLC info.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_oc_in_pch_with_toc
(
  void
);




/*===========================================================================

FUNCTION rrcllc_update_cm_parms

DESCRIPTION

 Updates CM params in Transition Config during DCH->FACH and DCH->PCH when 
 transition is due to a reconfiguration message to a UTRAN directed cell. 

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcllc_update_cm_parms
(
  void
);

/*===========================================================================

FUNCTION rrcllc_initialize_cm_parms

DESCRIPTION

 Initializes CM params in Transition Config

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcllc_initialize_cm_parms
(
  void
);
/*===========================================================================

FUNCTION rrcllc_init_rrc_cm_ota_parms

DESCRIPTION

 Checks and deactivates the CM params in config ptr

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcllc_init_rrc_cm_ota_parms
(
  ordered_config_type *config_ptr
);
/*===========================================================================

FUNCTION rrcllc_find_and_deact_cm_pattern

DESCRIPTION

 Checks and deactivates the CM params in config ptr

DEPENDENCIES
 None

RETURN VALUE

 None

SIDE EFFECTS

 None

===========================================================================*/

void rrcllc_find_and_deact_cm_pattern
(
  ordered_config_type *config_ptr
);
/*====================================================================
FUNCTION: rrcllc_init_dl_common_info()

DESCRIPTION:
  This function is responsible for initializing DL Common Info
  for DPCH. The only field that needs to be init is the Timing
  Initialization type. It is set to "Timing Maintained".

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dl_common_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_timing_init_in_progress()
DESCRIPTION:
  This function returns true if timing init HHO is ongoing.
DEPENDENCIES:
  None
RETURN VALUE:
  None
SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_timing_init_in_progress(void);

#ifdef FEATURE_UMTS_PDCP
/*====================================================================
FUNCTION: rrcllc_print_pdcp_info()

DESCRIPTION:
  This function prints the PDCP information in the ordered config pointer

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None

====================================================================*/
void rrcllc_print_pdcp_info(ordered_config_type *config_ptr);
#endif /* FEATURE_UMTS_PDCP */

/*====================================================================
FUNCTION: rrcllc_print_mac_info()

DESCRIPTION:
  This function prints the MAC information in the ordered config pointer

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None

====================================================================*/
void rrcllc_print_mac_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_print_l1_info()

DESCRIPTION:
  This function prints the L1 information in the ordered config pointer

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None

====================================================================*/
void rrcllc_print_l1_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION: rrcllc_cell_update_confirm_ie()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd
  in the Cell Update Confirm Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_cell_update_confirm_ie
(
  rrc_CellUpdateConfirm_r3_IEs *msg_ptr
);


#ifdef FEATURE_UMTS_PDCP

/*====================================================================
FUNCTION: rrcllc_copy_pdcp_parms()

DESCRIPTION:
  This function is responsible for copying the PDCP
  parms from Source to Destination Config data.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_pdcp_parms(
  ordered_config_type  *dest_config,
  ordered_config_type  *src_config
  );

#endif /* FEATURE_UMTS_PDCP */

/*====================================================================
FUNCTION: rrcllc_copy_rlc_parms()

DESCRIPTION:
  This function is responsible for copying the RLC AM, UM and TM
  parms from Source to Destination Config data.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

void rrcllc_copy_rlc_parms(
  ordered_config_type  *dest_config,
  ordered_config_type  *src_config
  );


/*====================================================================
FUNCTION: rrcllc_copy_cell_fach_rlc_size_list()

DESCRIPTION:
  This function is responsible for copying all RLC Size Restriction Info
  stored for CELL_FACH state in the Source Config, to the RLC Size
  Restriction Info for FACH and RACH in the Destination Config.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

void  rrcllc_copy_cell_fach_rlc_size_list(
  ordered_config_type  *dest_config,
  ordered_config_type  *src_config
  );

/******************************/
/* End of Function prototypes */
/******************************/

/*============================================================================
FUNCTION: rrcllc_validate_t_barred_criteria_for_aich_info

DESCRIPTION:
  This validates PRACH info from SIB5/6 and checks to see if it meets
  T-Barred criteria.

DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS: Indicates that Cell contains atleast AICH info hence don't bar it
FAILURE: Indicates that none of Prach info contains AICH.  Hence bar the cell

SIDE EFFECTS:
  None.
==============================================================================*/
uecomdef_status_e_type rrcllc_validate_t_barred_criteria_for_aich_info
(
  rrc_PRACH_SystemInformationList   *prach_sys_info_ptr
);
/*============================================================================
FUNCTION: rrcllc_validate_t_barred_criteria_for_pich_info

DESCRIPTION:
  This validates SCCPCH info from SIB5/6 and checks to see if it meets
  T-Barred criteria.

DEPENDENCIES:
  None.

RETURN VALUE:
SUCCESS: Indicates that Cell contains atleast PICH info hence don't bar it
FAILURE: Indicates that none of SCCPCH info contains PICH. Hence bar the cell

SIDE EFFECTS:
  None.
==============================================================================*/
uecomdef_status_e_type rrcllc_validate_t_barred_criteria_for_pich_info
(
  rrc_SCCPCH_SystemInformationList *sccpch_sys_info_ptr
);

/*====================================================================
FUNCTION        get_dl_info_for_each_rl_r5()

DESCRIPTION     This function translates the Downlink information for
                each radio link IE to the RRC config database

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
rrcllcoc_dl_info_per_rl_status_e_type get_dl_info_for_each_rl_r5
(
  rrc_DL_InformationPerRL_r5 *ie_ptr,

  l1_dl_phychan_db_struct_type *dl_phychan,

  uint8 rl_idx,

  boolean is_new_rl
);
#ifdef FEATURE_UMTS_PDCP
/*==========================================================
FUNCTION  : get_pdcp_info_later_than_r3()

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
);
#endif
/*==========================================================
FUNCTION  : get_am_index()

DESCRIPTION : Returns the index of the am_config info
              associated with the given RB id, or
              UE_MAX_AM_ENTITY if no entry exists.

DEPENDENCIES: None.

RETURN VALUE: This function returns AM index in OC for a given
              rb_id.

SIDE EFFECTS: None.
============================================================*/
uint8 get_am_index
(
  rrc_RB_Identity rb_id
);

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
);
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
);

#ifdef FEATURE_MVS
/*===========================================================================

FUNCTION rrcllc_get_dl_amr_mode

DESCRIPTION

 Given the TB size, this function returns the AMR mode associated with this 
 TB size

DEPENDENCIES
 None

RETURN VALUE

 mvs_amr_mode_type: AMR mode

SIDE EFFECTS

 None

===========================================================================*/
mvs_amr_mode_type rrcllc_get_dl_amr_mode(uint32 tb_size);
/*============================================================================
FUNCTION: rrc_mvsssup_get_dl_amr_mode()

DESCRIPTION:
This function gets the DL AMR mode

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
mvs_amr_mode_type rrc_mvssup_get_dl_amr_mode(void);
/*===========================================================================

FUNCTION rrcllc_get_dl_amr_mode_for_rb

DESCRIPTION

 Given RB Id, returns the AMR mode associated with it

DEPENDENCIES
 None

RETURN VALUE

 mvs_amr_mode_type: AMR mode

SIDE EFFECTS

 None

===========================================================================*/
mvs_amr_mode_type rrcllc_get_dl_amr_mode_for_rb
(
  rrc_RB_Identity  rb_id, 
  ordered_config_type *config_ptr
);
#endif /* FEATURE_MVS */

/*====================================================================
FUNCTION        rrcllc_init_srb5_info()

DESCRIPTION     This function initializes SRB#5 data structures 
                with default values.

DEPENDENCIES    NONE

RETURN VALUE    void

SIDE EFFECTS
====================================================================*/
void rrcllc_init_srb5_info(ordered_config_type *config_ptr);

/*====================================================================
FUNCTION        rrcllc_has_config_srb5()

DESCRIPTION     This function tells if the passed RB Id maps to SRB#5 
                or not.

DEPENDENCIES    NONE

RETURN VALUE    boolean
                TRUE: If RB Id is same as SRB#5 Id.
                FALSE: otherwise

SIDE EFFECTS
====================================================================*/
boolean rrcllc_has_config_srb5
(
  rrcllc_config_e_type config_type, 
  uint16 rb_id
);

/*====================================================================
FUNCTION: rrcllc_get_rlc_size_change_status()

DESCRIPTION:
  This function checks if RLC Size Change is in progress from OC and returns
  TRUE or FALSE accordingly

DEPENDENCIES:
  None

RETURN VALUE:
  RLC Size

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_get_rlc_size_change_status(void);

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
);
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
);
#endif /*FEATURE_MAC_I*/

/*====================================================================
FUNCTION: rrcllc_init_reestablish_entity()

DESCRIPTION:
  This function re-initializes Re-establish entity with in OC/CC.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if RLC Size Changed

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_reestablish_entity
(
  rrcllc_reestab_config_e_type config_type
);

/*====================================================================
FUNCTION: rrcllc_compare_rlc_size_change()

DESCRIPTION:
  This function compares the new with old RLC Size

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if RLC Sioze Changed

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrcllc_compare_rlc_size_change(uint32 old_size, uint32 new_size);


/*====================================================================
FUNCTION        rrcllcpcie_update_sync_a_post_veri_info()

DESCRIPTION     This function updates ordered config with the 
                postVerificationPeriod and the timingMaintainedSynchInd 
                IE values received in the OTA message

DEPENDENCIES    NONE

RETURN VALUE    None

SIDE EFFECTS
====================================================================*/
void rrcllcpcie_update_sync_a_post_veri_info
(
  void *msg_ptr,
   uint32 dl_sdu_num
);

/*====================================================================
FUNCTION        rrcllcpcie_initialise_sync_a_post_veri_info()

DESCRIPTION     This function intialises the postVerificationPeriod 
                and the timingMaintainedSynchInd values in ordered config 
                to default values

DEPENDENCIES    NONE

RETURN VALUE    None

SIDE EFFECTS
====================================================================*/
void  rrcllcpcie_initialise_sync_a_post_veri_info
(
  void
);
/*====================================================================
FUNCTION        rrcllcpcie_validate_timing_maintained_sync_ind()

DESCRIPTION     This function validates if the timingMaintainedSynchInd IE is valid for the OTA message

DEPENDENCIES    NONE

RETURN VALUE    None

SIDE EFFECTS
====================================================================*/
void rrcllcpcie_validate_timing_maintained_sync_ind
(
  rrc_proc_e_type proc_id
);

/*====================================================================
FUNCTION: rrcllc_get_dch_fach_act_time_in_ms()

DESCRIPTION:
  This function returns activation time in ms for dch->fach transitions.
  returns 0 if no act time or if this function is called during non dch-fach
  transitions.

DEPENDENCIES:
  None

RETURN VALUE:
  uin16.

SIDE EFFECTS:
  None.
====================================================================*/
uint16 rrcllc_get_dch_fach_act_time_in_ms(void);
/*==========================================================
FUNCTION  : rrcllc_check_if_rbid_present_in_chan_config_within_index()

DESCRIPTION : This function checks if the RB-Id specified 
              is already present in Chan_config within the index specified.

DEPENDENCIES: None.

RETURN VALUE: 
boolean: returns TRUE if Rb Id is presrn in  Channel 
         Config Req.It also returns the index at which 
         the RB-Id is present in Chan Config else returns 
         FALSE.

SIDE EFFECTS: None
============================================================*/
uint8 rrcllc_check_if_rbid_present_in_chan_config_within_index
(
  rrc_channel_config_req_type * chan_config_req, 
  rrc_RB_Identity rb_id,
  uint8 count
);

/*==========================================================
FUNCTION  : rrcllc_check_if_rbid_present_in_chan_config()

DESCRIPTION : This function checks if the RB-Id specified is already present in Chan_config

DEPENDENCIES: None.

RETURN VALUE: boolean: returns TRUE if Rb Id is presrn in  Channel Config Req. It also 
     returns the index at which the RB-Id is present in Chan Config
     else returns FALSE

SIDE EFFECTS: None
============================================================*/
uint8 rrcllc_check_if_rbid_present_in_chan_config
(
  rrc_channel_config_req_type    * chan_config_req, 
  rrc_RB_Identity rb_id 
);



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
boolean rrcllc_check_ordered_current_mapping_same
(
  uint8 index
);
/*==========================================================
FUNCTION  : rrcllc_init_ordered_hanging_rb_mapping_info()

DESCRIPTION : initialise the rrc_ordered_hanging_rb_mapping info
DEPENDENCIES: None.

RETURN VALUE: None                        

SIDE EFFECTS: None
============================================================*/
extern void rrcllc_init_ordered_hanging_rb_mapping_info
(
  void
 );

/*==========================================================
FUNCTION  : rrcllc_copy_ordered_hanging_rb_mapping_to_current()

DESCRIPTION : copies the rrc_ordered_hanging_rb_mapping to rrc_current_hanging_rb_mapping
DEPENDENCIES: None.

RETURN VALUE: None                        

SIDE EFFECTS: None
============================================================*/
 void rrcllc_copy_ordered_hanging_rb_mapping_to_current
 ( 
   void
 );
/*==========================================================
FUNCTION  : rrcllc_copy_current_hanging_rb_mapping_to_orderedt()

DESCRIPTION : copies the rrc_ordered_hanging_rb_mapping to rrc_current_hanging_rb_mapping
DEPENDENCIES: None.

RETURN VALUE: None                        

SIDE EFFECTS: None
============================================================*/
void rrcllc_copy_current_hanging_rb_mapping_to_ordered(void);

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
);
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
);
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
 );
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
   uint8 trch_index,
   rb_mapping_chan_enum_type chan_type 
 );

/*==========================================================
FUNCTION  :   rrcllc_restore_ul_dch_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info from
              the RB-mapping backup info to the Ordered config
              for UL DCh.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              returns  SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_restore_ul_dch_rb_mapping_info
(
   uint8  index 
);

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
uecomdef_status_e_type rrcllc_restore_dl_dch_rb_mapping_info
( 
  tr_ch_id_type index
);
/*==========================================================
FUNCTION  : rrcllc_restore_macd_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info 
              from the RB-mapping backup info to the 
              Ordered config for MAC-D flow id.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping backup info is invalid
              else 
              returns  SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_restore_macd_rb_mapping_info
( 
  uint8 index
);

/*==========================================================
FUNCTION  : rrcllc_restore_ul_emac_rb_mapping_info()

DESCRIPTION : This function restores the Rb_mapping info from 
              the RB-mapping backup info to the Ordered config 
              for E-DCH MAC-d flowid.

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
                       backup info is invalid
              else 
              returns SUCCESS

SIDE EFFECTS: None
============================================================*/
uecomdef_status_e_type rrcllc_restore_ul_emac_rb_mapping_info
( 
  uint8 index
);
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
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
);
/*===========================================================================

FUNCTION          rrc_is_cm_active_with_eul


DESCRIPTION       

  This function returns whether EUL is active with compressed mode of not.
  This function doesnt checks the condition when CM is going to de-activate before
  activation time is reached. Its a known limitation from the design phase.

DEPENDENCIES


RETURN VALUE
                  
  TRUE/FALSE

SIDE EFFECTS
                  
  None

===========================================================================*/
boolean rrc_is_cm_active_with_eul
(
  ordered_config_type *config_ptr
);
/*===========================================================================

FUNCTION          rrc_is_eul_active


DESCRIPTION       

  This function returns whether HSUPA / EUL is active or not at a given moment.
  for input, if OC is not set then use current_config_ptr, else use ordered_config_ptr.

DEPENDENCIES


RETURN VALUE
                  
  TRUE/FALSE

SIDE EFFECTS
                  
  None

===========================================================================*/
boolean rrc_is_eul_active
(
  void
);
/*============================================================================
FUNCTION: rrcllcpcie_indicate_add_and_drop_of_active_set_rl()

DESCRIPTION:
  This function takes care of setting the e_dl_Channel_mask for all RLs in
  the add_reconfig list to ADD.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_indicate_drop_and_add_of_e_dch_active_set_rl
(
#ifdef FEATURE_WCDMA_DC_HSUPA

boolean is_sec_eul
#else
  void
#endif /* FEATURE_WCDMA_DC_HSUPA */

);

/*====================================================================
FUNCTION: rrcllc_ul_dchs_not_standalone()

DESCRIPTION:
  This function returns TRUE if there's at least one UL DCH TrCH that's not standalone. 
  Essentially, it means that a UL DCH is in use.

DEPENDENCIES:
  None

RETURN VALUE:
  boolean

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_ul_dchs_not_standalone
(
  void
);

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
  rrc_UL_LogicalChannelMapping_r6 *ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
);
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
);
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
);
/*====================================================================
FUNCTION: rrcllc_init_tfcs_needed_flags()

DESCRIPTION:
  This functions initializes flags ul_tfcs_needed and dl_tfcs_needed.

DEPENDENCIES:
  None

RETURN VALUE:
  uin16.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_tfcs_needed_flags(void);
/*====================================================================
FUNCTION: rrcllc_is_tfcs_provided_on_add_delete_trch()

DESCRIPTION:
  This function returns TRUE if UE was expecting to receive some TFCS
  info, but never received it.  Else returns FALSE

DEPENDENCIES:
  None

RETURN VALUE:
  boolean

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_is_tfcs_provided_on_add_delete_trch(rrc_RRC_StateIndicator dest_state);
/*==========================================================
FUNCTION  : rrcllc_validate_rb_mapping_info()

DESCRIPTION : This functions validates the RB mapping information
              for R99, R5 and R6.

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE.

SIDE EFFECTS: 
============================================================*/
uecomdef_status_e_type rrcllc_validate_rb_mapping_info(void);
/*====================================================================
FUNCTION: rrcllc_derive_dl_amr_mode()

DESCRIPTION:

  This function returns MVS AMR mode for DL. This function uses DL CTFCs 
  to derive the AMR mode.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  
====================================================================*/
void rrcllc_derive_dl_amr_mode
(
  rrcllc_amr_mapping_info_type * amr_info
);

#ifdef FEATURE_VOC_AMR_WB
/*===========================================================================
FUNCTION rrc_configure_amr_wb_mode_param

DESCRIPTION Configures the MVS  for WB- AMR
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_configure_amr_wb_mode_param
(
  rrcllc_amr_mapping_info_type * amr_wb_info
);

/*===========================================================================

FUNCTION rrc_change_codec_configuration

DESCRIPTION This function is called if there is a codec change in a ongoing call.
                     In this case, the old codec is released. And then the MVS is configured 
                     with the new codec parameters. And then the new is acquired.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_change_codec_configuration
(
  uint8 rab_idx
);

/*===========================================================================

FUNCTION rrcllcoc_update_oc_orderd_codec_mode

DESCRIPTION This function updates the OC with the codec mode which needs to be
                      applied.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrcllcoc_update_oc_orderd_codec_mode
(
  mvs_mode_type codec_mode_ordered
);

#endif /*FEATURE_VOC_AMR_WB*/

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
);
/*============================================================================
FUNCTION: rrcllcpcie_process_e_dch_reconfig_info_samecell()

DESCRIPTION:
  This function processes edch reconfig info same cell IE and 
  populates the L1 structures and updates the req mask
      
RE-USABILITY: 

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_e_dch_reconfig_info_samecell
(
  OSS_UINT32 e_dch_reconfig_info_samecell_present,
  rrc_E_DCH_RL_InfoSameServingCell *e_dch_reconfig_info_samecell_ptr
);
/*============================================================================
FUNCTION: rrcllcpcie_process_e_dch_reconfig_info_r7()

DESCRIPTION:
  This function processes rl removal list and updates e_dch active set information
  and adds removed rl to e_rl_removed list
      
RE-USABILITY: 

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_e_dch_reconfig_info_r7
(
  OSS_UINT32 e_dch_reconfig_info_present,
  rrc_E_DCH_ReconfigurationInfo_r7 *e_dch_reconfig_info_ptr
);
#ifdef FEATURE_WCDMA_16_QAM
/*============================================================================
FUNCTION: rrcllcpcie_process_ul_16qam_config()

DESCRIPTION:
  This function processes 16 QAM config
      
RE-USABILITY: 

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_16qam_config
(
  OSS_UINT32 ul_16qam_config_present,
  rrc_UL_16QAM_Config *ul_16qam_config_ptr
);
#endif
/*====================================================================
FUNCTION      set_hsscch_info_r7()

DESCRIPTION   This function sets the Channelization code required
              for HSPDSCH in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
uecomdef_status_e_type set_hsscch_info_r7
(
  rrc_HS_SCCH_Info_r7* hsscch_info_ptr
);
/*====================================================================
FUNCTION      set_meas_feedback_info_r7()

DESCRIPTION   This function sets the Meas Feedback Information in
              L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/

uecomdef_status_e_type set_meas_feedback_info_r7
(
  rrc_Measurement_Feedback_Info_r7* meas_feedback_ptr
);
/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action_r7()

DESCRIPTION:

  This function determines the HSDPA action and sets the parameters in L1
  structure accordingly.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  Sucess if HSDPA action and parameters have been set
  Failure Otherwise

SIDE EFFECTS:
  
====================================================================*/
uecomdef_status_e_type rrcllc_determine_hsdpa_action_r7
(
  rrc_DL_HSPDSCH_Information_r7* hspdsch_info_ptr
);


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
  rrc_RB_InformationReconfigList_r7 * rb_info_reconfig_list_ptr
)  ;
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
  rrc_RB_InformationAffectedList_r7 * rb_info_affected_list_ptr
)  ;
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
  rrc_SRB_InformationSetupList_r7 * src_info_setup_list_ptr
);

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
);

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
  rrc_DL_AddReconfTransChInfoList_r7 *dl_add_reconf_trch_info_list_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r7  *ul_add_reconf_trch_info_list_ptr  
);
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
);
/*====================================================================
FUNCTION: FIND_TRCH_ID_IN_CONFIG_TRCH_LIST_R7

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean find_trch_id_in_config_trch_list_r7
(
  rrc_DL_AddReconfTransChInfoList_r7 * dl_trch_add_reconfig_ptr,
  tr_ch_id_type trch_id
);

#ifdef FEATURE_WCDMA_REL9

/*====================================================================
FUNCTION: FIND_TRCH_ID_IN_CONFIG_TRCH_LIST_R9

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean find_trch_id_in_config_trch_list_r9
(
  rrc_DL_AddReconfTransChInfoList_r9 * dl_trch_add_reconfig_ptr,
  tr_ch_id_type trch_id
);
#endif
/*====================================================================
FUNCTION: rrcllc_rb_release_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 7 Radio Bearer Release Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_release_ie_r7
(
  rrc_RadioBearerRelease_r7 * rb_release_r7_ota
);

/*====================================================================
FUNCTION: rrcllc_rb_setup_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Setup Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_setup_ie_r7
(
  rrc_RadioBearerSetup_r7 * rb_setup_r7_ota
);
/*====================================================================
FUNCTION: rrcllc_rb_reconfig_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_reconfig_ie_r7
(
  rrc_RadioBearerReconfiguration_r7 * rb_reconfig_r7_ota
);
/*====================================================================
FUNCTION: rrcllc_trch_reconfig_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 6 Transport Channel Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_trch_reconfig_ie_r7
(
  rrc_TransportChannelReconfiguration_r7 * trch_reconfig_r7_ota
);
/*====================================================================
FUNCTION: rrcllc_phychan_reconfig_ie_r6()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Physical Channel Reconfiguration Message for Rel 6

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_phychan_reconfig_ie_r7
(
 rrc_PhysicalChannelReconfiguration_r7 * phys_chan_reconfig_r7_ota
);
/*====================================================================
FUNCTION: rrcllc_cell_update_confirm_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Cell Update Confirm Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_cell_update_confirm_ie_r7
(
  uint32 dl_sdu_num,
  void * cell_update_r7_msg_ptr
);
/*====================================================================
FUNCTION: rrcllc_rrc_conn_setup_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in R5 RRC Connection Setup Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rrc_conn_setup_ie_r7
(
  rrc_RRCConnectionSetup_r7 *rrc_conn_setup_r7_ota
);

/*====================================================================
FUNCTION: rrcllc_active_set_update_ie_r7()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Active Set Update Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_active_set_update_ie_r7
(
  rrc_ActiveSetUpdate_r7 * active_set_update_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_rl_addition_info_list_r7()

DESCRIPTION:
  This function updates IE "rrc_RL_AdditionInformationList_r6 " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_rl_addition_info_list_r7
(
  OSS_UINT32 rl_additon_info_list_present,
  rrc_RL_AdditionInformationList_r7 *rl_add_ptr
);
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
  rrc_UL_AddReconfTransChInfoList_r7 *  ul_trch_add_reconfig_ptr  
);
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
);

/*============================================================================
FUNCTION: rrcllcpcie_process_ul_e_dch_information_r7()

DESCRIPTION:
  This function updates IE "ul e dch information" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_e_dch_information_r7
(
  OSS_UINT32 e_dch_info_present,
  rrc_UL_EDCH_Information_r7 *e_dch_info_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);
/*============================================================================
FUNCTION: rrcllcpcie_process_ul_dpch_info_r7()

DESCRIPTION:
  This function updates IE "ul dpch info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_dpch_info_r7
(
  OSS_UINT32 ul_dpch_info_present,
  rrc_UL_DPCH_Info_r7 *ul_dpch_info_ptr,
  rrc_RRC_StateIndicator rcvd_state_indicator
);

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
  rrc_RAB_InformationSetupList_r7 * rab_info_setup_list_ptr
);
/*====================================================================
FUNCTION      rrcllc_check_if_meas_feedback_changed_r7()

DESCRIPTION   This function checks if Meas Feedback Info has changed.

DEPENDENCIES  None

RETURN VALUE  True if FDD specifed in Meas Feedback Cycle. False otherwise

SIDE EFFECTS  None.
====================================================================*/
void rrcllc_check_if_meas_feedback_changed_r7
(
  rrc_Measurement_Feedback_Info_r7* meas_feedback_ptr,
  boolean* beta_table_changed,
  boolean* dpcch_changed
);

/*============================================================================
FUNCTION: rrcllc_process_serving_hsdsch_cell_info_r7()

DESCRIPTION:
  This function updates IE "rrc_Serving_HSDSCH_CellInformation " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllc_process_serving_hsdsch_cell_info_r7
(
  rrc_ActiveSetUpdate_r7_IEs *msg_ptr 
);
/*============================================================================
FUNCTION: rrcllcpcie_is_hho_r7()

DESCRIPTION:
  This function returns boolean value based on whether inter-freq or intra-freq
  HHO is ongoing. 

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - HHO in progress
  FALSE - No HHO

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_is_hho_r7
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r7 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_common_info_r7()

DESCRIPTION:
  This function updates IE "dl common info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dl_common_info_r7
(
  OSS_UINT32 dl_common_info_present,
  rrc_DL_CommonInformation_r7 *dl_common_info_ptr,
  rrc_RRC_StateIndicator rcvd_state_indicator
);
/*============================================================================
FUNCTION: rrcllcpcie_process_dl_info_per_rl_list_r7)

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dl_info_per_rl_list_r7
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r7 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_hspdsch_info_and_update_hsdpa_msg_params_r7()

DESCRIPTION:
  This function updates IE "dl hspdsch info " in ordered config and update
  hsdpa_msg_param variable
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_hspdsch_info_and_upd_hsdpa_msg_params_r7
(
  OSS_UINT32 dl_hspdsch_info_present,
  rrc_DL_HSPDSCH_Information_r7 *dl_hspdsch_info_ptr,
  /* is inter freq handover involved */
  OSS_UINT32 is_ifhho_involved,
  rrc_RRC_StateIndicator rcvd_state_indicator
);
/*====================================================================
FUNCTION: rrcllc_update_oc_with_srb5_rel7_ie()

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
rrcllc_update_oc_with_srb5_rel7_ie
(
  rrc_SRB_InformationSetup_r7 * srb_setup_r7_ptr, 
  rrc_RB_Identity rb_id
);

/*============================================================================
FUNCTION: rrcllcpcie_update_hs_tfri_table_type_non_r7_ota()

DESCRIPTION:
  This function updates the TRFI table to be used for MAC-HS or MAC-EHS for Non-R7 OTA
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_hs_tfri_table_type_non_r7_ota
(
  void
);
/*============================================================================
FUNCTION: rrcllcpcie_update_hs_tfri_table_type()

DESCRIPTION:
  This function updates the TRFI table to be used for MAC-HS or MAC-EHS
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_hs_tfri_table_type
(
  boolean hs_tb_size_octet_aligned
);
/*====================================================================
FUNCTION: rrcllc_get_macehs_queue_index()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uint8  rrcllc_allocate_lc_index_macehs
( 
  ordered_config_type *config_ptr,
  rrc_LogicalChannelIdentity logicalChannelIdentity
);
/*===========================================================================

FUNCTION rrc_set_hsdpa_action_in_machs_macehs

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/
void rrc_set_hsdpa_action_in_machs_macehs
(
  void
);
/*====================================================================
FUNCTION: rrcllc_get_macehs_queue_index()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uint8 rrcllc_find_lc_index_for_macehs
( 
  ordered_config_type *config_ptr,
  rrc_LogicalChannelIdentity logicalChannelIdentity,
  uint16 rb_id
);
/*===========================================================================

FUNCTION rrcllc_set_machs_ehs_transition

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/
void rrcllc_set_machs_ehs_transition
(
void
);
/*===========================================================================

FUNCTION rrc_reset_mac_ehs_params

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/

void rrc_reset_mac_ehs_params
(
  ordered_config_type * config_ptr
);
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
);
/*====================================================================
FUNCTION: rrcllc_rearrange_macehs_queue_index()

DESCRIPTION:
  This function deletes any holes
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

void rrcllc_rearrange_macehs_queue_index
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_allocate_macehs_q_index()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A EDCH MAC-d flow Index.
  UE_MAX_MAC_D_FLOW -> Given EDCH Mac-d flow does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_allocate_macehs_q_index
(
  uint8  mac_ehs_q_id
);

/*====================================================================
FUNCTION: rrcllc_get_macehs_queue_index()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uint8 rrcllc_get_macehs_queue_index
(
  uint8  mac_ehs_q_id
);


/*====================================================================
FUNCTION: rrcllc_get_ehs_log_chan_mode()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified if the rb_id is mapped to EHS
  queues

DEPENDENCIES:
  None.

RETURN VALUE:
  Logical Channel mode of the rb_id    if rb_id is found mapped to a EHS queue
  UE_MODE_MAX_NUM     otherwise

SIDE EFFECTS:

====================================================================*/
uecomdef_logch_mode_e_type rrcllc_get_ehs_log_chan_mode
(
  rrc_RB_Identity rb_id
);

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
);



#ifdef FEATURE_WCDMA_MIMO
/*============================================================================
FUNCTION: rrcllc_init_mimo_params()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_init_mimo_params
(
  ordered_config_type *config_ptr
);

/*============================================================================
FUNCTION: rrcllc_get_mimo_action_in_config()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
l1_dl_hs_mimo_action_enum_type 
  rrcllc_get_mimo_action_in_config
(
  void
);

/*============================================================================
FUNCTION: rrcllc_update_mimo_status()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_mimo_status
(
  rrc_mimo_status_enum_type mimo_status
);
/*============================================================================
FUNCTION: rrcllc_get_mimo_status()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
rrc_mimo_status_enum_type rrcllc_get_mimo_status
(
   void
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params()

DESCRIPTION:
  This function updates IE "Mimo Params" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type    rrcllcpcie_process_mimo_params
(
  rrc_MIMO_Parameters_r7 * mimo_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 mimo_params_present_ota
);

#ifdef FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT
/*============================================================================
FUNCTION: rrcllcpcie_update_mimo_scpich_po()

DESCRIPTION:
  This function updates IE "s-cpich-PowerOffset-Mimo" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_mimo_scpich_po
(
  rrc_MIMO_Parameters_v7f0ext * mimo_params_ptr
);
#endif /*FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT*/

#ifdef FEATURE_WCDMA_MIMO_CODEBOOK_RESTRICTION
/*============================================================================
FUNCTION: rrcllcpcie_update_mimo_codebook_restriction()

DESCRIPTION:
  This function updates IE "precodingWeightSetRestriction" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_mimo_codebook_restriction
(
  boolean val
);
#endif /*FEATURE_WCDMA_MIMO_CODEBOOK_RESTRICTION*/

/*============================================================================
FUNCTION: rrcllc_check_if_mimo_can_be_started()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_check_if_mimo_can_be_started
(
  rrc_RRC_StateIndicator rrc_state
);
#endif

#if defined(FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT) || defined(FEATURE_WCDMA_MIMO_CODEBOOK_RESTRICTION)
/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_asu_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 ASU.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_asu_r7
(
  rrc_ActiveSetUpdate_r7 * active_set_update_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_asu_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 ASU.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_asu_r8
(
  rrc_ActiveSetUpdate_r8 * active_set_update_r8_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_cu_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 CU.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_cu_r7
(
  uint32 dl_sdu_num,
  void * cell_update_r7_msg_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_cu_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 CU.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_cu_r8
(
  uint32 dl_sdu_num,
  void * cell_update_r8_msg_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_tcrc_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 TCRC.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_tcrc_r7
(
  rrc_TransportChannelReconfiguration_r7 * trch_reconfig_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_tcrc_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 TCRC.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_tcrc_r8
(
  rrc_TransportChannelReconfiguration_r8 * trch_reconfig_r8_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_rbrc_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 RBRC.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_rbrc_r7
(
  rrc_RadioBearerReconfiguration_r7 * rb_reconfig_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_rbrc_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 RBRC.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_rbrc_r8
(
  rrc_RadioBearerReconfiguration_r8 * rb_reconfig_r8_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_rbr_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 RBR.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_rbr_r7
(
  rrc_RadioBearerRelease_r7 * rb_release_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_rbr_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 RBR.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_rbr_r8
(
  rrc_RadioBearerRelease_r8 * rb_release_r8_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_rbe_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 RBE.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_rbe_r7
(
  rrc_RadioBearerSetup_r7 * rb_setup_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_rbe_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 RBE.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_rbe_r8
(
  rrc_RadioBearerSetup_r8 * rb_setup_r8_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_pcrc_r7()

DESCRIPTION:
  This function updates the addional MIMO params for R7 PCRC.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_pcrc_r7
(
  rrc_PhysicalChannelReconfiguration_r7 * phys_chan_reconfig_r7_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_ext_pcrc_r8()

DESCRIPTION:
  This function updates the addional MIMO params for R8 PCRC.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_process_mimo_params_ext_pcrc_r8
(
  rrc_PhysicalChannelReconfiguration_r8 * phys_chan_reconfig_r8_ota
);

#endif /*FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT || FEATURE_WCDMA_MIMO_CODEBOOK_RESTRICTION*/

/*===========================================================================
FUNCTION    rrcllc_get_rb_id_list

DESCRIPTION This function gets the rb_ids given in the OTA(add/reconf and release included)
 
DEPENDENCIES
  None.

RETURN VALUE
 void

===========================================================================*/

void rrcllc_get_rb_id_list
(
  uint8 *rb_id_list, rrc_RB_InformationReleaseList  *rb_release_ptr
);


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
);

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
);

/*===========================================================================

FUNCTION rrcllcoc_update_toc_with_oc_from_fach


DESCRIPTION This function updates the TOC with the OC value. 
                      It also sets the TOC.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void  rrcllcoc_update_toc_with_oc_from_fach
(
  void
);


/*===========================================================================

FUNCTION rrcllc_init_fdpch_info


DESCRIPTION This function initializes the F-DPCH parameters in Config DB. 
                     
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void  rrcllc_init_fdpch_info
(
  ordered_config_type *config_ptr
);


/*====================================================================
FUNCTION: rrcllc_dl_dchs_not_standalone()

DESCRIPTION:
  This function returns TRUE if there's at least one DL DCH TrCH that's not standalone. 
  Essentially, it means that a DL DCH is in use.

DEPENDENCIES:
  None

RETURN VALUE:
  boolean

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_dl_dchs_not_standalone
(
  void
);

/*============================================================================
FUNCTION: rrcllc_check_if_tfi_present_in_allowed_list_in_tfcc()

DESCRIPTION:
  This function checks if the TFI for a Trch is present
  in the allowed list list in Restricted transport channel list
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
);

/*============================================================================
FUNCTION: rrcllc_check_if_new_tfcs_present()

DESCRIPTION:
  This Function returns TRUE if UL TFCs is set in ordered config,
  FALSE otherwise
  
DEPENDENCIES:
  None

RETURN VALUE:
TRUE    if new UL TFCS is present in ordered config
FALSE  otherwise
=============================================================================*/
boolean rrcllc_check_if_new_tfcs_present
(
  void
);

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
);

/*====================================================================
FUNCTION       rrcllc_compute_additional_prach_tf()

DESCRIPTION    Gets  the ATF size from SIb5/SIB6 and updates the TF and TFC list
               with it. Also update the atf_present, and atf_ccch_tf_index to indicate to 
               MAC the ATF size.

DEPENDENCIES   None

RETURN VALUE  None

SIDE EFFECTS   None.
====================================================================*/

void  
rrcllc_compute_additional_prach_tf
(
   rrc_AdditionalPRACH_TF_and_TFCS_CCCH_IEs * rrc_additional_prach_TF_list ,
   ordered_config_type *config_ptr,
   uint8 trch_idx
);

/*====================================================================
FUNCTION: rrcllc_get_cctrch_id()

DESCRIPTION:
  This function manages CCTrCH Id's for the UE.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A CCTrCH ID that can be used.

SIDE EFFECTS:
  cctrch_id = 0xFF -> indicates no CCTrCHs are available.
====================================================================*/
uint8 rrcllc_get_cctrch_id(void);

/*====================================================================
FUNCTION: rrcllc_clear_cctrch_id()

DESCRIPTION:
  This function clears a CCTrCH Id for later UE use.

DEPENDENCIES:
  None.

RETURN VALUE:
  ue_comdef_status_e_type -> SUCCESS indicates a CCTrCH ID was cleared
                             FAILURE indicates a CCTrCH ID was not cleared

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_clear_cctrch_id(uint8 cctrch_id);

/*==========================================================
FUNCTION  :   RRCLLC_GET_CONFIG_PTR_IN_USE()

DESCRIPTION : This function checks

DEPENDENCIES: None.

RETURN VALUE: See description.

SIDE EFFECTS: None.
============================================================*/
ordered_config_type *rrcllc_get_config_ptr_in_use(void);

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
);

/*====================================================================
FUNCTION: rrcllc_get_amr_mapping()

DESCRIPTION:
  This function returns takes a pointer to a data structure of type
  rrcllc_amr_mapping_info_type, fills in the RLC LC IDs of the Uplink
  and Downlink AMR Modes (A, B, C), as well as the AMR and SRC modes\
  in use.

DEPENDENCIES:
  None.

RETURN VALUE:
  FAILURE: If either the AMR mode is not understood (based on TFs)
           Or info on Class A and B is not available. For 12.2 and
           10.2 Kbps AMR modes, Class C info is also needed.
  SUCCESS: If info on AMR mode and all classes is available.         
           
SIDE EFFECTS:
  The uplink and downlink TrCH IDs and RLC Sizes are assumed to be
  the same.

  Since the UE only supports Logical Channels mapped to a TrCH
  where all Logical Channels mapped support all Transport Formats,
  and since this is not the case for AMR Modes, there can only be
  1 Logical channel on this TrCH.
====================================================================*/
uecomdef_status_e_type
rrcllc_get_amr_mapping( rrcllc_amr_mapping_info_type *ptr );

#ifdef FEATURE_VOC_AMR_WB

/*====================================================================
FUNCTION: rrcllc_get_amr_wb_mapping()

DESCRIPTION:
  This function returns takes a pointer to a data structure of type
  rrcllc_amr_wb_mapping_info_type, fills in the RLC LC IDs of the Uplink
  and Downlink AMR Modes (A, B, C), as well as the AMR and SRC modes\
  in use.

DEPENDENCIES:
  None.

RETURN VALUE:
  FAILURE: If either the AMR mode is not understood (based on TFs)
           Or info on Class A and B is not available. 
  SUCCESS: If info on AMR mode and all classes is available.

SIDE EFFECTS:
  The uplink and downlink TrCH IDs and RLC Sizes are assumed to be
  the same.

  Since the UE only supports Logical Channels mapped to a TrCH
  where all Logical Channels mapped support all Transport Formats,
  and since this is not the case for AMR Modes, there can only be
  1 Logical channel on this TrCH.
====================================================================*/

uecomdef_status_e_type rrcllc_get_amr_wb_mapping
(
  rrcllc_amr_mapping_info_type *ptr 
);

/*====================================================================
FUNCTION: rrcllc_derive_amr_rate()

DESCRIPTION:
 
DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:
====================================================================*/
mvs_amr_mode_type rrcllc_derive_amr_rate
(
  mvs_mode_type * codec_info ,
  dch_id_mapping * dch_mapping
);

#endif/*FEATURE_VOC_AMR_WB*/

/*===========================================================================
FUNCTION rrc_deregister_mvs

DESCRIPTION
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
void rrc_deconfigure_mvs
(
  void
);

/*===========================================================================
FUNCTION rrc_configure_mvs

DESCRIPTION
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_configure_mvs
(
  rrcllc_amr_mapping_info_type * amr_info,
  uint8 rab_idx
);


/*===========================================================================
FUNCTION rrc_register_ul_mvs_through_ghdi()

DESCRIPTION
  This function calls GHDI to register UL RLC queues for NB-AMR call.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_register_ul_mvs_through_ghdi
(
  rrcllc_amr_mapping_info_type * amr_info,
  uint8 num_of_RBs
);

/*===========================================================================
FUNCTION rrc_register_dl_mvs_through_ghdi()

DESCRIPTION
  This function calls GHDI to register DL RLC queues for NB-AMR call.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_register_dl_mvs_through_ghdi
(
  rrcllc_amr_mapping_info_type * amr_info
);

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
/*===========================================================================

FUNCTION rrc_save_rrc_amr_info_for_callback()

DESCRIPTION
  This function saves global rrc_amr_info that stores the TrCH ids and 
  CCTrCH ids for DL TrCHs.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_save_rrc_amr_info_for_mvs_callback
(
  rrcllc_amr_mapping_info_type * amr_info
);

#endif /*FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING*/


/*====================================================================
FUNCTION: rrcllc_get_hs_log_chan_mode()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified if the rb_id is mapped to HS dflow

DEPENDENCIES:
  None.

RETURN VALUE:
  Logical Channel mode of the rb_id    if rb_id is found mapped to a HS dflow
  UE_MODE_MAX_NUM     otherwise

SIDE EFFECTS:

====================================================================*/
uecomdef_logch_mode_e_type rrcllc_get_hs_log_chan_mode
(
  rrc_RB_Identity rb_id
);

/*====================================================================
FUNCTION: rrcllc_check_if_dflow_exist_and_update_rlc_id()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified if the rb_id is mapped to dflow,
  if mapped to dflow, updates the dflow info the logicah channel id

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE   if RB id is mapped to dflow
  FALSE     otherwise

SIDE EFFECTS:

====================================================================*/
boolean rrcllc_check_if_dflow_exist_and_update_rlc_id
(
  rrc_RB_Identity rb_id,
  rlc_lc_id_type ul_lc_id
);

/*====================================================================
FUNCTION: rrcllc_check_if_ul_dch_exist_and_update_rlc_id()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified if the rb_id is mapped to UL DCH,
  if mapped to DCH, updates the UL DCH info the logicah channel id

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE   if RB id is mapped to UL DCH
  FALSE     otherwise

SIDE EFFECTS:

====================================================================*/
boolean rrcllc_check_if_ul_dch_exist_and_update_rlc_id
(
  rrc_RB_Identity rb_id,
  rlc_lc_id_type ul_lc_id,
  uecomdef_logchan_e_type log_chl_type
);

/*====================================================================
FUNCTION: rrcllc_check_if_dl_dch_exist_and_update_rlc_id()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified if the rb_id is mapped to UL DCH,
  if mapped to DCH, updates the UL DCH info the logicah channel id

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE   if RB id is mapped to UL DCH
  FALSE     otherwise

SIDE EFFECTS:

====================================================================*/
boolean rrcllc_check_if_dl_dch_exist_and_update_rlc_id
(
  rrc_RB_Identity rb_id,
  rlc_lc_id_type dl_lc_id,
  uecomdef_logchan_e_type log_chl_type
);

/*====================================================================
FUNCTION: rrcllc_get_log_chan_mode_ul_dch()

DESCRIPTION:
  This function gets a the log_chan_mode for the rb id specified if the rb_id is UL DCH

DEPENDENCIES:
  None.

RETURN VALUE:
  Logical Channel mode of the rb_id    if rb_id is found mapped to a UL DCH
  UE_MODE_MAX_NUM     otherwise

SIDE EFFECTS:

====================================================================*/
uecomdef_logch_mode_e_type rrcllc_get_log_chan_mode_ul_dch
(
  rrc_RB_Identity rb_id
);

/*===========================================================================
FUNCTION rrcllc_update_delay_restriction
DESCRIPTION
This function updates the activation time based on delay restriction flag received in the OTA

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcllc_update_delay_restriction
(
  OSS_UINT32 act_time_present, 
  OSS_UINT32 delay_restriction_present,
  rrc_ActivationTime act_time,
  rrc_RRC_StateIndicator rcvd_state_indicator
);


/*====================================================================
FUNCTION: rrcllc_allocate_mem_for_prach_sel_info()

DESCRIPTION:
  This function initializes global prach_sel_info dynamically/statically based on whether feature 
  FEATURE MODEM HEAP is defined or not.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.
  
SIDE EFFECTS:
  None.

=====================================================================*/
void rrcllc_deallocate_mem_for_prach_sel_info
(
  uint8 rrc_num_of_prach
);

/*====================================================================
FUNCTION: rrcllc_allocate_mem_for_prach_sel_info()

DESCRIPTION:
  This function resets global prach_sel_info.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.
  
SIDE EFFECTS:
  None.

=====================================================================*/
void rrcllc_allocate_mem_for_prach_sel_info
(
  uint8 rrc_num_of_prach
);


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
  rrc_RB_InformationSetupList_r7  * rb_setup_head_ptr
);
/*============================================================================
FUNCTION: rrcllcpcie_process_edpch_interpolation_info()

DESCRIPTION:
  This function updates EDPCH power interpolation info
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
 void rrcllcpcie_process_edpch_interpolation_info
 (
   rrc_E_DPDCH_PowerInterpolation  e_dpch_pwr_interpolation_info,
   OSS_UINT32 e_dpch_pwr_interpolation_info_present
 );

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST

/*====================================================================
FUNCTION: rrcllcpcie_process_edpcch_boost_info()

DESCRIPTION:
   This function updates EDPCH Boost info

DEPENDENCIES:
  None.

RETURN VALUE:
   SUCCESS/FAILURE

SIDE EFFECTS:

====================================================================*/
uecomdef_status_e_type rrcllcpcie_process_edpcch_boost_info
(
  rrc_E_TFC_Boost_Info_r7 *e_tfc_boost_info_ptr,
  OSS_UINT32 e_tfc_boost_info_present
);
#endif
/*====================================================================
FUNCTION: rrcllc_update_drx_coef_length()

DESCRIPTION:
  This function is called when T319 timer expires(or is stopped) and updates the UTRAN DRX Cycle Coef Length

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS : If process is successfull
  FAILURE : Otherwise
  
SIDE EFFECTS:

====================================================================*/
void rrcllc_update_drx_coef_length
(
  void
);
/*====================================================================
FUNCTION: rrcllc_get_drx_info_for_multiple_drx()

DESCRIPTION:
  This function returns the shorter of the PS or CS CN DRX Cycle
  Length for multiple DRX support feature.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint32 of the DRX Cycle length, per 25.304.

SIDE EFFECTS:
  None.
====================================================================*/
uint32 rrcllc_get_drx_info_for_multiple_drx
(
  void
);

#ifdef FEATURE_WCDMA_HS_FACH
/*====================================================================
FUNCTION: rrcenhstate_camped_cell_supports_e_fach()

DESCRIPTION:
This function checks whether the currently camped cell supports eFACH operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_camped_cell_supports_e_fach
(
  void
);
/*====================================================================
FUNCTION: rrcenhstate_set_hrnti_status_for_transitions()

DESCRIPTION:
This function set hrnti status as valid
This funcion should be called during fach to fach redirection and
during dch to fach transitions

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_set_hrnti_status_for_transitions
(
  void
);

#ifdef FEATURE_WCDMA_HS_FACH_DRX

/*====================================================================
FUNCTION: rrcenhstate_get_e_fach_drx_status()

DESCRIPTION:
This function returns the EFACH DRX status 

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_get_e_fach_drx_status
(
  l1_hsdpa_info_struct_type *hsdpa_l1_info
);
/*====================================================================
FUNCTION: rrcenhstate_camped_cell_supports_e_fach_drx()

DESCRIPTION:
This function checks whether the currently camped cell supports eFACH drx operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_camped_cell_supports_e_fach_drx
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_nv_supports_e_fach_drx()

DESCRIPTION:
This function checks whether the NV supports eFACH drx operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_nv_supports_e_fach_drx
(
  void
);

#endif

/*====================================================================
FUNCTION: rrcenhstate_nv_supports_e_fach()

DESCRIPTION:
This function checks whether the NV supports eFACH operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_nv_supports_e_fach
(
  void
);


/*====================================================================
FUNCTION: rrcllc_efach_update_dedicated_params()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_efach_update_dedicated_params
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_set_hs_status_in_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_set_hs_status_in_e_fach
(
  ordered_config_type *config_ptr,
  boolean status
);

/*====================================================================
FUNCTION: rrcllc_get_hs_status_in_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_get_hs_status_in_e_fach
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_set_hs_action_in_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_set_hs_action_in_e_fach
(
  ordered_config_type *config_ptr,
  hsdpa_action_enum_type action
);

/*====================================================================
FUNCTION: rrcllc_get_hs_status_in_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
hsdpa_action_enum_type rrcllc_get_hs_action_in_e_fach
(
  ordered_config_type *config_ptr
);


/*====================================================================
FUNCTION: rrcllc_efach_mappings_are_given_for_rbs()

DESCRIPTION:
This function checks whether all the RBs are mapped to eFACH mappings i.e. RACH in UL and HS-DSCH in DL

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_efach_mappings_are_given_for_rbs
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_check_if_hs_can_be_started_reconfigured_in_e_fach()

DESCRIPTION:
  This function checks if HSDPA can be started/reconfigured in e-FACH state by looking at the following
  Check whether C-RNTI is present
  Dedicated H-RNTI is present
  The MAC-ehs Re-ordering Queues are present on which RBs are mapped.

  see function definion for checkStatus
DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_if_hs_can_be_started_reconfigured_in_e_fach
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type efach_mode
);

/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action_for_e_fach()

DESCRIPTION:
  This function sets HSDPA action in global hsdpa_action.

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_determine_hsdpa_action_for_e_fach
(
  void
);

/*============================================================================
FUNCTION: set_macehs_q_info()

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
);

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
);


/*====================================================================
FUNCTION: rrcllc_copy_sib_to_oc_for_efach()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS
  FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_sib_to_oc_for_efach(rrc_state_e_type next_rrc_state ,
               rrc_e_fach_operation_mode_e_type rrc_e_fach_operation_mode);

/*====================================================================
FUNCTION: rrcllc_process_drop_sccpch()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS
  FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcllc_process_drop_sccpch
(
  void
);
/*===========================================================================
FUNCTION   rrcllc_build_chan_config_req_cmd_for_est_rbs

DESCRIPTION
  This function is resposible for determining all actions on RLC, MAC
  and L1 when re-entering service area in Cell_FACH state.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
void rrcllc_build_chan_config_req_cmd_for_est_rbs
(
  rrc_cmd_type *cmd_ptr
);

#endif /*FEATURE_WCDMA_HS_FACH*/



/*====================================================================
FUNCTION: rrcllc_set_am_dl_rlc_size_ota()

DESCRIPTION:
  This function sets the PDU type and size for AM RB .
  If the PDU tpe is 
    RRC_PDU_SIZE_NOT_SET then the OTA has not provided the RLC size explicitly, and
     it should be calculated from the TF sizes of Trch
  RRC_FIXED_PDU_SIZE_SET the OTA had explicitly given the rlc size 
  RRC_FLEXIBLE_PDU_SIZE_SET OTA has set the RB as Flexible PDU
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrcllc_set_am_dl_rlc_size_ota
(
  rrc_RB_Identity rb_id,
  uint32 dl_rlc_am_pdu_size,
rrc_am_rlc_pdu_enum_type pdu_type

);

/*====================================================================
FUNCTION: rrcllc_get_am_dl_rlc_size_ota()

DESCRIPTION:
  This function returns  the RLC size if it is provided by the N\W  for AM RB .
 
  
DEPENDENCIES:
  None.

RETURN VALUE:
  RLC size if the OTA provided the rlcc size, 
  0 if it is flexible pdu,
  RRCLLC_INVALID_RLC_SIZE if rlc size is not provided in ota
  
SIDE EFFECTS:

====================================================================*/
uint32 rrcllc_get_am_dl_rlc_size_ota
(
  rrc_RB_Identity rb_id
);

/*====================================================================
FUNCTION: rrcllc_init_am_dl_rlc_size()

DESCRIPTION:
  This function inits the db which contains rlc size from ota .
 
  
DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrcllc_init_am_dl_rlc_size
(
  ordered_config_type *config_ptr,
  uint32 entity_idx
);


/*====================================================================
FUNCTION: rrcllc_print_rlc_info()

DESCRIPTION:
  This function prints RLC information.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:

====================================================================*/
void rrcllc_print_rlc_info(ordered_config_type *config_ptr);

void rrcllc_init_config_ptrs
(
  void
);


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
);

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
);

/*============================================================================
FUNCTION: rrcllc_shld_ctch_be_setup()

DESCRIPTION:
  This function indicates if the present cell support CTCH or not.
   CSP calls this function to find out if CTCH can be setup or not after BPLMN search is done
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FASLE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_shld_ctch_be_setup
(
  void
);

/*====================================================================
FUNCTION: rrcllc_check_and_stop_dtx()

DESCRIPTION:

  This function will be called by all the pre-Rel7 messages which are capable of DTX operation 
  It will check if DTX operation is ongoing, 
    - if yes, then it will stop DTX 
    - otherwise, it will not do anything.
    
DEPENDENCIES:
  Function should be called only by the pre-Rel7 messages which are capable of DTX operation.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_check_and_stop_dtx
(
  rrc_RRC_StateIndicator rcvd_state_indicator,
  rrc_msg_ver_enum_type     rel_ver
);

/*============================================================================
FUNCTION: rrcllc_update_l1_dtx_req_mask_for_stop()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_dtx_req_mask_for_stop
(
  boolean dtx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_dtx_req_mask_for_reconfig()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_dtx_req_mask_for_reconfig
(
  boolean dtx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_dtx_req_mask_for_start()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_dtx_req_mask_for_start
(
  boolean dtx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_dtx_req_mask_for_stop()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_stop
(
 ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_drx_req_mask_for_reconfig()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_reconfig
(
  ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_drx_req_mask_for_start()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_start
(
 ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*====================================================================
FUNCTION: rrcllc_reset_l1_cpc_dtx_info_on_llc_cfg_cnf()

DESCRIPTION:
  This function resets the L1 variables related to CPC-DTX on receipt of LLC CONFIG CNF 
  by any procedure
  
DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_reset_l1_cpc_dtx_info_on_llc_cfg_cnf
(
  void
);
/*====================================================================
FUNCTION: rrcllc_init_cpc_dtx_info_before_msg_processing()

DESCRIPTION:
  This function resets needed CPC-DTX variables before processing any config message

DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_cpc_dtx_info_before_msg_processing
(
  ordered_config_type *config_ptr
);
/*============================================================================
FUNCTION: rrcllcpcie_calculate_dpcch_slot_format()

DESCRIPTION:
  This function calculate dpcch_slot_format
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uint8  rrcllcpcie_calculate_dpcch_slot_format
(
  void
);
/*====================================================================
FUNCTION: rrcllc_init_dtx_params()

DESCRIPTION:
  This function evaluates the value of variable rrcllc_is_dtx_params_variable_set.

DEPENDENCIES:
  Function is only called after processing of all the IEs in a config message is completed.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_dtx_params
(
  ordered_config_type *config_ptr  
);
/*============================================================================
FUNCTION: rrcllcpcie_process_dtx_drx_params()

DESCRIPTION:
  This function processes 
  1) DTX-DRX timing info 
  2) dtx_drx_Info &
  3) evaluates DTX-DRX status in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type    rrcllcpcie_process_dtx_drx_params
(
  rrc_DTX_DRX_TimingInfo_r7 *dtx_drx_timingInfo,
  OSS_UINT32 dtx_drx_timingInfo_present_ota,
  rrc_DTX_DRX_Info_r7 *dtx_drx_Info,
  OSS_UINT32 dtx_drx_Info_present_ota,
  rrc_RRC_StateIndicator rrc_state
);
/*============================================================================
FUNCTION: rrcllc_update_l1_dtx_req_mask_for_no_op()

DESCRIPTION:
  This function updates the l1_cpc_dtx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_dtx_req_mask_for_no_op
(
  boolean dtx_info_present
);
l1_ue_dtx_cycle1_info_enum_type  rrcllcpcie_get_dtx_cycle1
(
  void    *ue_dtx_cycle1,
  e_tti_enum_type e_tti
);
/*============================================================================
FUNCTION: rrcllcpcie_get_dtx_cycle2()

DESCRIPTION:
  This function processes DTX info in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
l1_ue_dtx_cycle2_info_enum_type  rrcllcpcie_get_dtx_cycle2
(
  void    *ue_dtx_cycle2,
  e_tti_enum_type e_tti
);
/*============================================================================
FUNCTION: rrcllcpcie_get_mac_dtx_cycle()

DESCRIPTION:
  This function converts MAC DTX info to internal form 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
l1_mac_dtx_cycle_info_enum_type rrcllcpcie_get_mac_dtx_cycle
(
  void    *mac_dtx_cycle,
  e_tti_enum_type e_tti
);
/*============================================================================
FUNCTION: rrcllcpcie_get_ue_dpcch_burst_1_info()

DESCRIPTION:
  This function converts MAC DTX info to internal form 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
l1_ue_dpcch_burst_1_info_enum_type rrcllcpcie_get_ue_dpcch_burst_1_info
(
    rrc_UE_DPCCH_Burst *ue_dpcch_burst_1_info
);
/*============================================================================
FUNCTION: rrcllcpcie_get_ue_dpcch_burst_2_info()

DESCRIPTION:
  This function converts MAC DTX info to internal form 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
l1_ue_dpcch_burst_2_info_enum_type rrcllcpcie_get_ue_dpcch_burst_2_info
(
    rrc_UE_DPCCH_Burst *ue_dpcch_Burst2
);
/*============================================================================
FUNCTION: rrcllcpcie_get_ue_dtx_long_preamble_length()

DESCRIPTION:
  This function converts MAC DTX info to internal form 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
l1_dtx_long_preamble_length_info_enum_type rrcllcpcie_get_ue_dtx_long_preamble_length
(
  rrc_UE_DTX_long_preamble_length *ue_dtx_long_preamble_length
);
/*============================================================================
FUNCTION: rrcllcpcie_get_inact_thrshd_dtx_cycle2()

DESCRIPTION:
  This function converts MAC DTX info to internal form 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
l1_inactivity_threshold_for_ue_dtx_cycle2_info_enum_type rrcllcpcie_get_inact_thrshd_dtx_cycle2
(
  rrc_UE_DTX_Cycle2InactivityThreshold *ue_dtx_cycle2_inactiv_thrshd
);

/*====================================================================
FUNCTION: rrcllc_evaluate_dtx_drx_status_var_and_set_lower_layers_action()

DESCRIPTION:

  This function calls 
    - the function that evaluates the value of variable dtx_drx_status.
    - the function that updates the l1_cpc_dtx_req_mask for L1
    - the function updates the various masks in l1 and other information based on 
      evaluated value of dtx_drx_status variable

DEPENDENCIES:
  Function is only called after processing of all the IEs in a config message is 
  completed.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_evaluate_dtx_drx_status_var_and_set_lower_layers_action
(
  rrc_RRC_StateIndicator rcvd_state_indicator,
  OSS_UINT32 dtx_drx_timingInfo_present_ota
);


/*====================================================================
FUNCTION: rrcllc_active_set_update_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Active Set Update Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_active_set_update_ie_r8
(
  rrc_ActiveSetUpdate_r8 * active_set_update_r8_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_rl_addition_info_list_r8()

DESCRIPTION:
  This function updates IE "rrc_RL_AdditionInformationList_r8 " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_rl_addition_info_list_r8
(
  OSS_UINT32 rl_additon_info_list_present,
  rrc_RL_AdditionInformationList_r8 *rl_add_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_is_hho_r8()

DESCRIPTION:
  This function returns boolean value based on whether inter-freq or intra-freq
  HHO is ongoing. 

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - HHO in progress
  FALSE - No HHO

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_is_hho_r8
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r8 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_common_info_r8()

DESCRIPTION:
  This function updates IE "dl common info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dl_common_info_r8
(
  OSS_UINT32 dl_common_info_present,
  rrc_DL_CommonInformation_r8 *dl_common_info_ptr,
  rrc_RRC_StateIndicator rcvd_state_indicator
);

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

/*====================================================================
FUNCTION        get_dl_info_common_for_all_rl_r8()

DESCRIPTION     This function translates the Downlink information common
                for all radio links IE into the RRC config database.

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
uecomdef_status_e_type get_dl_info_common_for_all_rl_r8
(
  /* Pointer to the DL CommonInformation IE */
  rrc_DL_CommonInformation_r8 *ie_ptr,

  /* Pointer to the desirec DL PhyChan common information structure */
  l1_dl_common_info_struct_type *dl_common,

  /* Pointer to the DL CCTRCH parameters structure */
  l1_dl_cctrch_info_struct_type *l1_dl_cctrch_parms,

  /* Pointer to the DL CTFC parameters structure */
  l1_dl_ctfc_info_struct_type *l1_dl_ctfc_info
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_info_per_rl_list_r8

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dl_info_per_rl_list_r8
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r8 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*===========================================================================
FUNCTION        get_dl_info_per_rl_r8()

DESCRIPTION     This function validates and stores Downlink Information Per
                Radio Link for reconfiguration messages.

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    Ordered config is updated with the new RL info.
===========================================================================*/
uecomdef_status_e_type get_dl_info_per_rl_r8
(
  rrc_DL_InformationPerRL_List_r8 *ie_ptr,
  rrc_RRC_StateIndicator next_state
);

#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION        get_sec_dl_info_per_rl()

DESCRIPTION     This function validates and stores Downlink Information Per
                Radio Link for reconfiguration messages.

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    Ordered config is updated with the new RL info.
===========================================================================*/
uecomdef_status_e_type get_sec_dl_info_per_rl
(
  rrc_DL_InformationPerSecondaryRL_List *ie_ptr,
  rrc_RRC_StateIndicator next_state
);

#endif /* FEATURE_WCDMA_DC_HSUPA */
/*===========================================================================
FUNCTION        is_current_camped_psc_in_rl_info_r8()

DESCRIPTION     This function determines whether the current camped psc is
                included in the dl-per-rl-list or not.

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    Ordered config is updated with the new RL info.
===========================================================================*/
boolean is_current_camped_psc_in_rl_info_r8(rrc_DL_InformationPerRL_List_r8 *ie_ptr);

/*====================================================================
FUNCTION        get_dl_info_for_each_rl_r8()

DESCRIPTION     This function translates the Downlink information for
                each radio link IE to the RRC config database

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
rrcllcoc_dl_info_per_rl_status_e_type get_dl_info_for_each_rl_r8
(
  rrc_DL_InformationPerRL_r8 *ie_ptr,

  l1_dl_phychan_db_struct_type *dl_phychan,

  uint8 rl_idx,

  boolean is_new_rl
);
#ifdef FEATURE_WCDMA_DC_HSUPA
/*====================================================================
FUNCTION        get_dl_fdpch_info_for_each_rl()

DESCRIPTION     This function translates the Downlink information for
                each radio link IE to the RRC config database

DEPENDENCIES    NONE

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise.

SIDE EFFECTS
====================================================================*/
rrcllcoc_dl_info_per_rl_status_e_type get_sec_fdpch_info_for_each_rl
(
  rrc_DL_InformationPerSecondaryRL *ie_ptr,

  l1_dl_phychan_db_struct_type *dl_phychan,

  uint8 rl_idx,

  boolean is_new_rl
);
#endif /* FEATURE_WCDMA_DC_HSUPA */

/*============================================================================
FUNCTION: rrcllcpcie_process_e_info_in_dl_info_per_rl_list_r8()

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_e_info_in_dl_info_per_rl_list_r8
(
  OSS_UINT32 dl_info_per_rl_list_present,
  rrc_DL_InformationPerRL_List_r8 *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_e_agch_info_r8()

DESCRIPTION:
  This function processes e-AGCH information and updates the information
  in a given e_rl_index
      
DEPENDENCIES:
  This function needs to be called only after calling function to process E-HICH info.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_e_agch_info_r8
(
  OSS_UINT32 e_agch_info_present,
  rrc_E_AGCH_Information_r8 *e_agch_info_ptr,
  uint32 psc
);

/*============================================================================
FUNCTION: rrcllcpcie_process_ul_e_dch_information_r8()

DESCRIPTION:
  This function updates IE "ul e dch information" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_e_dch_information_r8
(
  OSS_UINT32 e_dch_info_present,
  rrc_UL_EDCH_Information_r8 *e_dch_info_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dl_hspdsch_info_and_update_hsdpa_msg_params_r8()

DESCRIPTION:
  This function updates IE "dl hspdsch info " in ordered config and update
  hsdpa_msg_param variable
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_hspdsch_info_and_upd_hsdpa_msg_params_r8
(
  OSS_UINT32 dl_hspdsch_info_present,
  rrc_DL_HSPDSCH_Information_r8 *dl_hspdsch_info_ptr,
  /* is inter freq handover involved */
  OSS_UINT32 is_ifhho_involved,
  rrc_RRC_StateIndicator rcvd_state_indicator
);

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
  rrc_UL_AddReconfTransChInfoList_r8 * ul_trch_add_reconfig_ptr,
  OSS_UINT32 ul_deleted_trch_info_list_present,
  rrc_UL_DeletedTransChInfoList_r6 * ul_deleted_trch_list_ptr
);

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
  rrc_SRB_InformationSetupList_r8 * src_info_setup_list_ptr
);

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
  rrc_SRB_InformationSetupList_r8 * srb_setup_head_ptr,
  rrc_RB_Identity                   rb_id
);

/*==========================================================
FUNCTION  : get_rb_mapping_info_r8()

DESCRIPTION : Get Signalling Radio Bearer Information to setup.

REUSABILITY: R6

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
);
/*==========================================================
FUNCTION  : rrcllc_validate_srb_id()

DESCRIPTION : Get Signalling Radio Bearer Information to setup.

REUSABILITY: R6

DEPENDENCIES: None.

RETURN VALUE: SUCCESS or FAILURE

SIDE EFFECTS: None.
============================================================*/
uecomdef_status_e_type rrcllc_validate_srb_id
(
  rb_id_type                   rb_id
);

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
);

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
);

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
  rrc_UL_AddReconfTransChInformation_r8 *ie_ptr
);

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
  rrc_RAB_InformationSetupList_r8 * rab_info_setup_list_ptr
);

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
  rrc_RB_InformationReconfigList_r8 * rb_info_reconfig_list_ptr
);

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
);

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
  rrc_RB_InformationSetupList_r8  * rb_setup_head_ptr
);

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
  rrc_RB_InformationAffectedList_r8 * rb_info_affected_list_ptr
);

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
);

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
  rrc_DL_AddReconfTransChInfoList_r7* dl_add_reconf_trch_info_list_ptr,
  /* local UL TrCH Add Reconfig pointer */
  rrc_UL_AddReconfTransChInfoList_r8 * ul_add_reconf_trch_info_list_ptr  
);

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
 
);

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
);

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
);

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
);

/*============================================================================
FUNCTION: rrcllcpcie_process_e_dpdch_info_r8()

DESCRIPTION:
  This function updates IE "dl per rl info" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_e_dpdch_info_r8
(
  OSS_UINT32 e_dpdch_info_present,
  rrc_E_DPDCH_Info_r8 *e_dpdch_info_ptr
);

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
uecomdef_status_e_type rrcllc_backup_eul_from_ota_r8
(
  rrc_UL_LogicalChannelMapping_r8*ie_ptr,  
  rrc_RB_Identity                    rb_id,
  uecomdef_logch_mode_e_type    logch_mode
);

/*===========================================================================
FUNCTION        get_rl_add_info_r8()

DESCRIPTION     This function translates radio link addition information from
                a radio link addition list into the RRC config database. This
                function is only called from an Active Set Update Message

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    The RRC config database as pointed to by the input parameters
                will be updated.
===========================================================================*/
uecomdef_status_e_type get_rl_add_info_r8
(
  rrc_RL_AdditionInformation_r8 *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan,
  boolean *duplicate_add_attempted
);

#ifdef FEATURE_WCDMA_MIMO
/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_r8()

DESCRIPTION:
  This function updates IE "Mimo Params" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type    rrcllcpcie_process_mimo_params_r8
(
  rrc_MIMO_Parameters_r8 * mimo_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 mimo_params_present_ota
);
#endif /*FEATURE_WCDMA_MIMO*/

/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action_r8()

DESCRIPTION:

  This function determines the HSDPA action and sets the parameters in L1
  structure accordingly.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  Sucess if HSDPA action and parameters have been set
  Failure Otherwise

SIDE EFFECTS:
  
====================================================================*/
uecomdef_status_e_type rrcllc_determine_hsdpa_action_r8
(
  rrc_DL_HSPDSCH_Information_r8* hspdsch_info_ptr
);

/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action_for_hsdpa_active_r8()

DESCRIPTION:
  This function determines the HSDPA action when HSDPA ia already ACTIVE. 
  
DEPENDENCIES:
  None.

RETURN VALUE:
  Sucess if HSDPA action and parameters have been set
  Failure Otherwise

SIDE EFFECTS:
====================================================================*/
uecomdef_status_e_type rrcllc_determine_hsdpa_action_for_hsdpa_active_r8
(
  rrc_DL_HSPDSCH_Information_r8* hspdsch_info_ptr
);

/*============================================================================
FUNCTION: rrcllc_process_serving_hsdsch_cell_info_r8()

DESCRIPTION:
  This function updates IE "rrc_Serving_HSDSCH_CellInformation " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllc_process_serving_hsdsch_cell_info_r8
(
  rrc_ActiveSetUpdate_r8_IEs *msg_ptr 
);

#ifdef FEATURE_WCDMA_DC_HSDPA
/*============================================================================
FUNCTION: rrcllcpcie_update_sec_hs_ie_info_into_oc()

DESCRIPTION:
  This function processes IE "DL_SecondaryCellInfoFDD" content in ordered config for R8 OTAs. Basically,
  this function updates the OC with the IE content and does some valiadation.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_sec_hs_ie_info_into_oc
(
  rrc_DL_SecondaryCellInfoFDD * dc_hsdpa_params,
  uint8 index
);

/*============================================================================
FUNCTION: rrcllcpcie_update_sec_hs_ie_info_into_oc_r8()

DESCRIPTION:
  This function processes IE "DL_SecondaryCellInfoFDD" content in ordered config for R8 OTAs. Basically,
  this function updates the OC with the IE content and does some valiadation.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_update_sec_hs_ie_info_into_oc_r8
(
  rrc_DL_SecondaryCellInfoFDD * dc_hsdpa_params,
  uint8 index
);
/*============================================================================
FUNCTION: rrcllcpcie_update_sec_hs_ie_info_into_oc()

DESCRIPTION:
  This function processes IE "DL_SecondaryCellInfoFDD" content in ordered config for R8 OTAs. Basically,
  this function updates the OC with the IE content and does some valiadation.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_sec_hs_ie_info_into_oc_r9
(
  rrc_DL_SecondaryCellInfoFDD_r9_configurationInfo * dc_hsdpa_params,
  uint8 index
);

/*==============
FUNCTION: rrcllcpcie_update_sec_hs_ie_info_into_oc()

DESCRIPTION:
  This function processes IE "DL_SecondaryCellInfoFDD" content in ordered config for R8 OTAs. Basically,
  this function updates the OC with the IE content and does some valiadation.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
void rrcllcpcie_update_sec_hs_ie_info_into_oc_r10
(
  rrc_DL_SecondaryCellInfoFDD_r10_configurationInfo * dc_hsdpa_params,
  uint8 index
);

/*============================================================================
FUNCTION: rrcllc_init_sec_hsdpa_params()

DESCRIPTION: This function resets DC-HSDPA parameters in Config DB (CC/OC). It also
  resets global status of DC-HSDPA.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_init_sec_hsdpa_params
(
  ordered_config_type *config_ptr
);

/*============================================================================
FUNCTION: rrcllc_check_if_dc_hsdpa_can_be_started()

DESCRIPTION: When this function returns FALSE, the Reconfig Failure isn't sent in the OTA. 
  This function does the following validations:
  1. UE is going to Cell_DCH state.
  2. HSDPA will be active with DC-HSDPA.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_check_if_dc_hsdpa_can_be_started
(
  rrc_RRC_StateIndicator rrc_state
);

/*============================================================================
FUNCTION: rrcllcpcie_process_dc_hsdpa_params()

DESCRIPTION:
  This function updates IE "DL_SecondaryCellInfoFDD" in ordered config for R8 OTAs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dc_hsdpa_params
(
  rrc_DL_SecondaryCellInfoFDD * dc_hsdpa_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 dc_hsdpa_params_present_ota
);

/*============================================================================
FUNCTION: rrcllc_validate_sec_hsdpa_config()

DESCRIPTION: When this function returns FALSE, the Reconfug Failure is sent in the OTA.
  The following validations are done here.
  1. Make sure MIMO is not active with DC-HSDPA.
  2. Total no. of HS-SCCH codes doesn't exceed 6

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_validate_sec_hsdpa_config
(
  OSS_UINT32 dc_hsdpa_params_present_ota,
  rrc_RRC_StateIndicator rrc_rcvd_state

);

#ifdef FEATURE_WCDMA_REL9

/*============================================================================
FUNCTION: rrcllcpcie_process_dc_hsdpa_params_r9()

DESCRIPTION:
  This function processes IE "DL_SecondaryCellInfoFDD" content in ordered config for R9 OTAs. Basically,
  this function updates the OC with the IE content and does some valiadation.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dc_hsdpa_params_r9
(
  rrc_DL_SecondaryCellInfoFDD_r9 * dc_hsdpa_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 dc_hsdpa_params_present_ota
);
#endif /*FEATURE_WCDMA_REL9*/

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif /*FEATURE_WCDMA_REL10*/



#endif /*FEATURE_WCDMA_DC_HSDPA*/


/*====================================================================
FUNCTION: validate_fdpch_information_r8()

DESCRIPTION:
  This function verifies that there are no mixed RLs with DPCH and FDPCH
  after ASU is processed.

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS if validation is successful.
  FAILURE if there are mixed RLs with FDPCH and DPCH

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type validate_fdpch_information_r8
(
  rrc_RL_AdditionInformationList_r8 *rl_AdditionInformationList_ptr
);

/*====================================================================
FUNCTION: rrcllc_rrc_conn_setup_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in R8 RRC Connection Setup Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rrc_conn_setup_ie_r8
(
  rrc_RRCConnectionSetup_r8 *rrc_conn_setup_r8_ota
);

/*====================================================================
FUNCTION: rrcllc_cell_update_confirm_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Cell Update Confirm Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_cell_update_confirm_ie_r8
(
  uint32 dl_sdu_num,
  void * cu_msg_ptr

);

/*====================================================================
FUNCTION: rrcllc_phychan_reconfig_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Physical Channel Reconfiguration Message for Rel 8

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_phychan_reconfig_ie_r8
(
  rrc_PhysicalChannelReconfiguration_r8 * phys_chan_reconfig_r8_ota
);

/*====================================================================
FUNCTION: rrcllc_rb_reconfig_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_reconfig_ie_r8
(
  rrc_RadioBearerReconfiguration_r8 * rb_reconfig_r8_ota
);

/*====================================================================
FUNCTION: rrcllc_rb_setup_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Setup Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_setup_ie_r8
(
  rrc_RadioBearerSetup_r8 * rb_setup_r8_ota
);

/*====================================================================
FUNCTION: rrcllc_rb_release_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 8 Radio Bearer Release Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_release_ie_r8
(
  rrc_RadioBearerRelease_r8 * rb_release_r8_ota
);

/*====================================================================
FUNCTION: rrcllc_trch_reconfig_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 8 Transport Channel Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_trch_reconfig_ie_r8
(
  rrc_TransportChannelReconfiguration_r8 * trch_reconfig_r8_ota
);

/*====================================================================
FUNCTION: rrcllc_handover_to_utran_ie_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Handover to UTRAN Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_handover_to_utran_ie_r8
(
 rrc_HandoverToUTRANCommand_r8 *msg_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_dl_common_mode_specific_info_r8()

DESCRIPTION:
 
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS or FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type
rrcllcpcie_dl_common_mode_specific_info_r8
(
  struct  rrc_DL_CommonInformation_r8_fdd * ie_ptr,
  l1_dl_common_info_struct_type *dl_common
);

/*============================================================================
FUNCTION: set_compressed_mode_info_r8()

DESCRIPTION:
  This function sets the compressed mode parameters from UTRAN message
  in the format required by L1. It validates the parameters to check if they
  are in range and sets in Ordered Config the compressed mode list. L1 is provided
  the entire list everytime with no of elements in it.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type set_compressed_mode_info_r8
(
  rrc_DPCH_CompressedModeInfo_r8 * rrc_compressed_mode_ptr
);

/*====================================================================
FUNCTION: rrcllc_handover_to_utran_process_preconfig_r8()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Handover to UTRAN Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_handover_to_utran_process_preconfig_r8
(
  rrc_HandoverToUTRANCommand_r8_IEs *msg_ptr
);


/*====================================================================
FUNCTION: rrcllc_clear_agch_chan_mask_for_non_serving_rl()

DESCRIPTION:
  This function clear agch chan mask for non serving EUL rl.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_clear_agch_chan_mask_for_non_serving_rl
(
  ordered_config_type  *config_ptr
#ifdef FEATURE_WCDMA_DC_HSUPA
 , boolean is_sec_eul
#endif /* FEATURE_WCDMA_DC_HSUPA */

);

/*===========================================================================
FUNCTION rrcllc_update_utran_drx_cycle_length_coeff_in_oc

DESCRIPTION
This function updates the utran DRX cycle length coefficient to OC received in
any reconfig message.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS or FAILURE
===========================================================================*/

uecomdef_status_e_type rrcllc_update_utran_drx_cycle_length_coeff_in_oc_r7
(
  OSS_UINT32 utran_drx_cycle_len_coeff_present, 
  rrc_UTRAN_DRX_CycleLengthCoefficient_r7 * utran_drx_cycle_len_coeff
);

/*====================================================================
FUNCTION: rrcllc_init_cpc_drx_info_before_msg_processing()

DESCRIPTION:
  This function resets needed CPC-DRX variables before processing any config message

DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_cpc_drx_info_before_msg_processing
(
  ordered_config_type *config_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_init_drx_info_before_processing()

DESCRIPTION:
  This function will init stored DRX info before processing 
  DRX_info I.E
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
void  rrcllcpcie_init_drx_info_before_processing
(
  ordered_config_type *config_ptr
);
/*============================================================================
FUNCTION: rrcllc_update_l1_drx_req_mask_for_stop()

DESCRIPTION:
  This function updates the l1_cpc_drx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_stop
(
  ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_drx_req_mask_for_reconfig()

DESCRIPTION:
  This function updates the l1_cpc_drx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_reconfig
(
  ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_drx_req_mask_for_start()

DESCRIPTION:
  This function updates the l1_cpc_drx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_start
(
  ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*============================================================================
FUNCTION: rrcllc_update_l1_drx_req_mask_for_no_op()

DESCRIPTION:
  This function updates the l1_cpc_drx_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_drx_req_mask_for_no_op
(
 ordered_config_type *config_ptr,
  boolean drx_info_present
);
/*============================================================================
FUNCTION: rrcllc_get_l1_cpc_drx_status()

DESCRIPTION:
  This function retuens the l1_cpc_drx_status.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_get_l1_cpc_drx_status
(
 ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_is_drx_params_variable_set()

DESCRIPTION:
  This function evaluates the value of variable rrcllc_is_drx_params_variable_set.

DEPENDENCIES:
  Function is only called after processing of all the IEs in a config message is completed.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_is_drx_params_variable_set
(
  void
);
/*====================================================================
FUNCTION: rrcllc_is_drx_params_variable_set()

DESCRIPTION:
  This function evaluates the value of variable rrcllc_is_drx_params_variable_set.

DEPENDENCIES:
  Function is only called after processing of all the IEs in a config message is completed.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_is_drx_params_variable_set
(
  void
);

/*====================================================================
FUNCTION: rrcllc_clear_sib5_change_in_progress_flag()

DESCRIPTION:
  This function set sib5_change_in_progress_flag.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_set_sib5_change_in_progress_flag
(
  void
);

/*====================================================================
FUNCTION: rrcllc_set_sib5_change_in_progress_flag_to_active()

DESCRIPTION:
  This function set sib5_change_in_progress_flag.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_set_sib5_change_in_progress_flag_to_active
(
  void
);
/*====================================================================
FUNCTION: rrcllc_clear_sib5_change_in_progress_flag()

DESCRIPTION:
  This function clear sib5_change_in_progress_flag.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_clear_sib5_change_in_progress_flag
(
  void
);
/*====================================================================
FUNCTION: rrcllc_get_sib5_change_in_progress_flag()

DESCRIPTION:
  This function gets sib5_change_in_progress_flag.

DEPENDENCIES:
  None

RETURN VALUE:
  boolean : TRUE=sib5_change_in_progress_flag set FALSE otherwise

SIDE EFFECTS:
  None.
====================================================================*/
#ifdef FEATURE_QSH_DUMP
__attribute__((section(".uncompressible.text")))
#endif
rrcllc_sib5_change_type rrcllc_get_sib5_change_in_progress_flag
(
  void
);

/*===========================================================================
FUNCTION   rrcllc_get__pccpch_tx_diversity

DESCRIPTION
  This function gives the TxDiversity value for the serving cell.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcllc_get_pccpch_tx_diversity
(
  rrc_state_e_type next_state
);


#ifdef FEATURE_UPDATE_SIB7_IN_FACH

/*===========================================================================
FUNCTION   rrcllc_get_pccpch_tx_diversity_from_sibs

DESCRIPTION
  This function gives the TxDiversity value for the cell for which PCCPCH is setup.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if TxDiversity is to be enabled
 else FALSE
 Default value is TRUE.
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcllc_get_pccpch_tx_diversity_from_sibs
(
  rrc_state_e_type next_state
);
#endif

/*===========================================================================
FUNCTION   rrcllc_get_correct_pccpch_tx_diversity_from_sibs

DESCRIPTION
  This function gives the TxDiversity value for the cell for which PCCPCH is 
setup.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if TxDiversity is to be enabled
 else FALSE
 Default value is TRUE.
SIDE EFFECTS
  None.
===========================================================================*/
boolean rrcllc_get_correct_pccpch_tx_diversity_from_sibs
(
  rrc_state_e_type next_state
);


#ifdef FEATURE_MAC_I

/*====================================================================
FUNCTION: rrcllc_find_mac_d_flow_idx_and_logch_idx()

DESCRIPTION:
  This function finds the edch mac-d flow id for a particular RB ID.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_find_mac_d_flow_idx_and_logch_idx
(
  mac_e_config_s_type *mac_e_config,
  rrc_RB_Identity  rb_id,
  uint8 *prev_mac_dflow_index,
  uint8 *prev_logch_idx
);
#endif /* FEATURE_MAC_I */

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
);

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================

FUNCTION rrcllc_establish_srb_rab_in_efach

DESCRIPTION
  This function sends RRC_CHANNEL_CONFIG_REQ to RRC LLC to establish RB0
  and RB1 to RB4 if the RB mapping info of these signalling RBs on FACH
  is available. rrcllc will decide whether to setup RB1 to RB4 along with RB0
  
DEPENDENCIES


RETURN VALUE
  rrccu_status_e_type: returns RRCCU_SUCCESS if it successfully sends
  RRC_CHANNEL_CONFIG_REQ to the RRC LLC. Otherwise it returns
  RRCCU_FAILURE_MAJOR.


SIDE EFFECTS

  None

===========================================================================*/
void rrcllc_establish_srb_rab_in_efach
( 
  rrc_e_fach_operation_mode_e_type efach_mode,
  rrc_proc_e_type       procedure,
  boolean is_oc_set
);


/*====================================================================
FUNCTION: rrcllc_check_for_efach_mapping()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_for_efach_mapping
(
  ordered_config_type  *config_ptr,
  rb_id_type  rb_id
);

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
);

/*====================================================================
FUNCTION: rrcllc_store_mapping_in_hanging_rb()

DESCRIPTION:
Stores into HRB

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_store_mapping_in_hanging_rb
(
  uint8 hrb_index,
  uint8 combo_type,
  ul_mapping_info_type_ *ul_map,
  dl_mapping_info_type_ *dl_map
);

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
);

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

uint8 rrcllc_get_rb_index_in_hanging_rb
(
  rrc_RB_Identity rb_id
);


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

/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R5()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE: SUCCESS / FAILURE

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging
(
  void *ie_ptr,
  mapping_ie_release_version rel_ver,
  rrc_RB_Identity               rb_id,
  uecomdef_logch_mode_e_type    logch_mode
);


/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R99()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE: SUCCESS / FAILURE

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R99
(
  rrc_RB_MappingInfo         * ie_ptr,
  uint8 hrb_index
);

/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R5()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE: SUCCESS / FAILURE

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R5
(
  rrc_RB_MappingInfo_r5  *ie_ptr,
  uint8 hrb_index
);

/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R6()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE: SUCCESS / FAILURE

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R6
(
  rrc_RB_MappingInfo_r6  *ie_ptr,
  uint8 hrb_index
);

/*====================================================================
FUNCTION: rrcllc_backup_mapping_info_into_hanging_R7()

DESCRIPTION:
Backs up Mapping info into hangin RB

DEPENDENCIES:
  None.

RETURN VALUE: SUCCESS / FAILURE

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_backup_mapping_info_into_hanging_R7
(
  rrc_RB_MappingInfo_r7  *ie_ptr,
  uint8 hrb_index
);

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
);
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
);

/*==========================================================
FUNCTION  :   rrcllc_restore_macehs_mapping()

DESCRIPTION : This function restores the MACEHS mapping for a particular RB

DEPENDENCIES: None.

RETURN VALUE: FAILURE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              returns  SUCCESS

SIDE EFFECTS: None
============================================================*/

boolean rrcllc_restore_macehs_mapping
(
  ordered_config_type * config_ptr,
  uint8 rb_index,
  uint8 hrb_index
);
/*==========================================================
FUNCTION  :   rrcllc_restore_rach_rb_mapping()

DESCRIPTION : This function restores the rach mapping for a particular RB

DEPENDENCIES: None.

RETURN VALUE: FALSE: if the Rb-mapping info in the Rb-Mapping 
              backup info is invalid
              else 
              returns  TRUE

SIDE EFFECTS: None
============================================================*/
boolean rrcllc_restore_rach_mapping
(
  ordered_config_type * config_ptr,
  uint8 rb_index,
  uint8 hrb_index
);
/*==========================================================
FUNCTION  :   rrcllc_restore_fach_rb_mapping()

DESCRIPTION : This function restores thefach mapping for a particular RB

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
);

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

int print_ul_type(uint8 combo);

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

int print_dl_type(uint8 combo);


/*====================================================================
FUNCTION: rrcllc_check_edch_mapping_in_ordered_hanging_rb()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_edch_mapping_in_ordered_hanging_rb
(
  rrc_RB_Identity rb_id
);
/*====================================================================
FUNCTION: rrcllc_check_macehs_mapping_in_ordered_hanging_rb()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_macehs_mapping_in_ordered_hanging_rb
(
  rrc_RB_Identity rb_id
);

/*====================================================================
FUNCTION: rrcllc_check_rach_mapping_in_ordered_hanging_rb()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_rach_mapping_in_ordered_hanging_rb
(
  rrc_RB_Identity rb_id
);

/*====================================================================
FUNCTION: rrcllc_check_hsfach_mapping_in_ordered_hanging_rb()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_hsfach_mapping_in_ordered_hanging_rb
(
  rrc_RB_Identity rb_id
);
/*====================================================================
FUNCTION: rrcllc_check_hsfach_mappings_in_channel_config_req()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_hsfach_mappings_in_channel_config_req
(
rrc_channel_config_req_type *channel_config_req_ptr
);

#endif

#ifdef FEATURE_WCDMA_HS_RACH
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_prach_info()

DESCRIPTION:
  This function initializes hs-rach l1 prach variables.

DEPENDENCIES:

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_prach_info
(
  l1_ul_prach_cfg_struct_type *hs_prach_cfg_ptr  
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_ul_dpch_info()

DESCRIPTION:
  This function initializes hs-rach ul dpch structure.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_ul_dpch_info
(
  l1_ul_dpch_cfg_struct_type *ul_dpch_cfg_ptr  
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_common_edch_resource_list()

DESCRIPTION:
  This function initializes hs-rach common edch resource list .

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_common_edch_resource_list
(
  l1_e_dch_common_list_info_struct_type *common_e_dch_resource_list_ptr  
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_aich_info()

DESCRIPTION:
  This function initializes hs-rach l1 aich variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_aich_info
(
  l1_aich_parms_struct_type *hs_rach_aich_ptr  
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_fdpch_info()

DESCRIPTION:
  This function initializes hs-rach l1 fdpch variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_fdpch_info
(
  l1_hs_rach_f_dpch_info_struct_type *hs_rach_dl_fdpch_info_ptr  
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_hsdpcch_info()

DESCRIPTION:
  This function initializes l1 hs dpcch variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_hsdpcch_info
(
  l1_ul_hs_dpcch_info_struct_type *hs_dpcch_info_ptr,
  rrc_state_e_type next_rrc_state
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_eul_info()

DESCRIPTION:
  This function initializes hs-rach l1_e_ul_info variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_eul_info
(
  l1_e_ul_info_struct_type *e_ul_info_ptr  
);
/*====================================================================
FUNCTION: rrcllc_init_l1_hsrach_edl_info()

DESCRIPTION:
  This function initializes hs-rach l1_e_dl_info variables.
  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_l1_hsrach_edl_info
(
  l1_e_dl_hs_rach_info_struct_type *e_dl_info_ptr
);
/*====================================================================
FUNCTION: rrcllc_init_mac_hsrach_info()

DESCRIPTION:
  This function initializes hs-rach mac structures in rrc data-base
  
DEPENDENCIES:

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_hsrach_info
(
mac_hs_rach_config_s_type *mac_hs_rach_config_ptr
);
/*====================================================================
FUNCTION: rrcllc_init_hsrach_info()

DESCRIPTION:
  This function initializes l1_e_ul_info variables.
  Some of the variables needs to be touched only if UE is in idle state or
  transitioning to idle state.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hsrach_info
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state
);
/*====================================================================
FUNCTION: rrcllc_init_mac_hsrach_edch_info_ie()

DESCRIPTION:
  This function initializes 'mac_e_config.e_dch_info' variables (i.e. UL-EDCH
  physical layer parameters).
  This function should be called whenever UE leaves DCH state.  
DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_hsrach_edch_info_ie
(
  ordered_config_type *config_ptr
);
/*====================================================================
FUNCTION: rrcllc_init_hs_rach_info()

DESCRIPTION:
  This function initializes all the HS-RACH variables.

DEPENDENCIES:


RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_hs_rach_info
(
  ordered_config_type *config_ptr  
);
/*====================================================================
FUNCTION: rrcllc_update_ready_for_common_edch()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_determine_and_set_ready_for_common_edch
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state
);

/*====================================================================
FUNCTION: rrcllc_update_hspa_rnti_stored_cell_pch()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_determine_and_set_hspa_rnti_stored_cell_pch
(
  ordered_config_type *config_ptr,
  boolean action
);
/*====================================================================
FUNCTION: rrcllc_get_hspa_rnti_stored_cell_pch()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_get_hspa_rnti_stored_cell_pch
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_copy_sib_to_oc_for_hsrach()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS
  FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_sib_to_oc_for_hsrach(rrc_state_e_type next_rrc_state);

/*====================================================================
FUNCTION: rrcllc_copy_sib_to_oc_for_hsrach()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS
  FAILURE

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_copy_sib_to_oc_for_hsrach(rrc_state_e_type next_rrc_state);

/*====================================================================
FUNCTION: rrchsrach_camped_cell_supports_hsrach()

DESCRIPTION:
This function checks whether the currently camped cell supports HSRACH or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_camped_cell_supports_hsrach
(
  void
);

/*====================================================================
FUNCTION: rrcllc_evaluate_hs_rach_and_set_lower_layers_action()

DESCRIPTION:
This function evalutate mac and l1 actions

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcllc_evaluate_hs_rach_and_set_lower_layers_action
(
  rrc_state_e_type next_rrc_state,
  ordered_config_type* config_ptr
);
/*====================================================================
FUNCTION: rrcllc_check_and_update_hsrach_params()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_check_and_update_hsrach_params
(
  rrc_RRC_StateIndicator rrc_StateIndicator,
  boolean validate_directed_cell_info,
  boolean validation_check,
  boolean new_ernti_present
);


/*====================================================================
FUNCTION: rrchsrach_set_common_edch_transmission()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrchsrach_set_common_edch_transmission
(
  ordered_config_type *config_ptr,
  boolean status
);

/*====================================================================
FUNCTION: rrchsrach_get_common_edch_transmission()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_get_common_edch_transmission
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrchsrach_get_common_edch_transmission_wof3()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_get_common_edch_transmission_wof3
(
  ordered_config_type *config_ptr
);


/*====================================================================
FUNCTION: rrchsrach_set_ready_for_common_edch()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrchsrach_set_ready_for_common_edch
(
  ordered_config_type *config_ptr,
  boolean status
);
/*====================================================================
FUNCTION: rrchsrach_set_hspa_rnti_stored_cell_pch()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrchsrach_set_hspa_rnti_stored_cell_pch
(
  ordered_config_type *config_ptr,
  boolean status
);
/*====================================================================
FUNCTION: rrchsrach_get_ready_for_common_edch()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_get_ready_for_common_edch
(
  ordered_config_type *config_ptr
);


/*====================================================================
FUNCTION: rrcllc_determine_hsrach_action_for_e_fach()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_determine_hsrach_action
(
  void
);
/*====================================================================
FUNCTION: rrcllc_determine_hsrach_action_for_reselection()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_determine_hsrach_action_for_reselection
(
  ordered_config_type* , ordered_config_type*
);
/*============================================================================
DESCRIPTION:
  This funtion resets MAC and L1 hsrach actions

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
=============================================================================*/
void rrcllc_reset_mac_and_l1_for_hsrach_config_ptr
(
  ordered_config_type* config_ptr 
);
/*============================================================================
DESCRIPTION:
  This funtion resets MAC and L1 hsrach actions

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
=============================================================================*/
void rrcllc_reset_mac_and_l1_for_hsrach
(
  boolean clear_all_config_pointers
);
/*====================================================================
FUNCTION: rrcllc_update_hs_rach_params()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_update_hs_rach_params
(
  ordered_config_type* config_ptr
);
/*====================================================================
FUNCTION: rrcllc_set_hs_rach_action()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_set_hs_rach_action
  (
    ordered_config_type *config_ptr,
    mac_hs_rach_action_e_type action
  );
/*====================================================================
FUNCTION: rrcllc_get_hs_rach_action()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  mac_hs_rach_action_e_type.

SIDE EFFECTS:
  None.
====================================================================*/
mac_hs_rach_action_e_type rrcllc_get_hs_rach_action
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_check_if_hs_rach_can_be_started_reconfigured()

DESCRIPTION:
  This function checks if HSDPA can be started/reconfigured in e-FACH state by looking at the following
  Check whether C-RNTI is present
  Dedicated H-RNTI is present
  The MAC-ehs Re-ordering Queues are present on which RBs are mapped.

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

boolean rrcllc_check_if_hs_rach_can_be_started_reconfigured(ordered_config_type* config_ptr);
/*====================================================================
FUNCTION: rrcllc_check_hsrach_mappings_are_given_for_rbs()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_hsrach_mappings_are_given_for_rbs
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcllc_check_hsrach_mappings_for_all_rbs_present()

DESCRIPTION:
This function checks if hsrach mappings are available for all
srbs and rbs in established rabs

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_hsrach_mappings_for_all_rbs_present(void);

/*====================================================================
FUNCTION: rrcllc_check_and_invalidate_hspa_rnti_stored_cell_pch()

DESCRIPTION:
  This function checks if mappings and hrnti and ernti are available
  while going to PCH state and invalidates hspa_rnti_stored_cell_pch 
  variable

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_check_and_invalidate_hspa_rnti_stored_cell_pch
(
  ordered_config_type *config_ptr, 
  rrc_state_e_type rrc_state
);

/*====================================================================
FUNCTION: rrcllc_check_hsrach_mapping_in_ordered_hanging_rb()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_hsrach_mapping_in_ordered_hanging_rb
(
  rrc_RB_Identity rb_id
);
/*====================================================================
FUNCTION: rrcllc_check_hsrach_mappings_in_channel_config_req()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_mappings_in_channel_config_req_and_srb
(
  rrc_channel_config_req_type *channel_config_req_ptr,
  uint8 ul_dl_mapping_combo
);

/*====================================================================
FUNCTION: rrcllc_check_for_edch_hsrach_mapping()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_for_edch_hsrach_mapping
(
  ordered_config_type *config_ptr,
  rb_id_type rb_id
);

/*====================================================================
FUNCTION: rrcllc_update_hsrach_e_dch_mapping_info_with_log_chl_id()

DESCRIPTION:
  This function updates the Uplink Mapping Info in EDCH RB Mapping
  Info with the passed Logical Channel ID.
  The correct entity is selected based on the passed RB ID.
  The EDCH RB mapping info in the passed Config data is updated.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_update_hsrach_e_dch_mapping_info_with_log_chl_id
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity  rb_id,
  rlc_lc_id_type   rlc_ul_log_chl_id
);
/*====================================================================
FUNCTION: rrchsrach_set_hsrach_action_to_stop_if_active_ota()

DESCRIPTION:

   reset the mac and l1 action if hsrach is active

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

void rrchsrach_set_hsrach_action_to_stop_if_active_ota(void);
/*====================================================================
FUNCTION: rrchsrach_set_hsrach_action_to_stop_if_active()

DESCRIPTION:

   reset the mac and l1 action if hsrach is active

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/

void rrchsrach_set_hsrach_action_to_stop_if_active
(
  boolean clear_all_config_pointers
);

/*====================================================================
FUNCTION: rrcllc_init_mac_hsrach_info()

DESCRIPTION:
  This function initializes hs-rach mac mapping structures in rrc data-base
  
DEPENDENCIES:

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_mac_hsrach_mapping_info
(
  mac_hs_rach_config_s_type *mac_hs_rach_config_ptr
);
/*====================================================================
FUNCTION: rrcllc_check_if_rbid_exists_in_config_ptr()

DESCRIPTION:
  This function finds if a RB ID exists in database 

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_if_rbid_exists_in_config_ptr
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity  rb_id,
  uint8    e_mac_d_flow_idx
);
/*====================================================================
FUNCTION: rrchsrach_check_ernti_status()

DESCRIPTION:
This function checks if ernti is valid in hsrach cell 
This funcion should be called during fach to fach redirection and
during dch to fach transitions only.

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_check_ernti_status
(
  void
);

/*====================================================================
FUNCTION: rrcllc_get_ul_rlc_size_in_config_ptr()

DESCRIPTION:
This function gets the UL rlc size in fach state.

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
uint32 rrcllc_get_ul_rlc_size_in_config_ptr
(
  ordered_config_type  *config_ptr,
  rrc_RB_Identity rb_id
);
#endif

/*===========================================================================

FUNCTION          rrcllcpcie_check_for_duplicate_rl_entry

DESCRIPTION       This function checks for duplicate RL entries in OC

DEPENDENCIES      None

RETURN VALUE      None

SIDE EFFECTS      None
===========================================================================*/

uecomdef_status_e_type rrcllcpcie_check_for_duplicate_rl_entry(void);
#ifdef FEATURE_WCDMA_REL9

#ifdef FEATURE_WCDMA_DC_HSUPA

/*============================================================================
FUNCTION: rrcllcpcie_process_sec_serv_edch_info()

DESCRIPTION: This fnction processes information specific to secondary eul serving cell.

      
DEPENDENCIES:


RETURN VALUE:


SIDE EFFECTS:
=============================================================================*/


void rrcllcpcie_process_sec_serv_edch_info
(
  rrc_SecondaryServingEDCHCell_Info *sec_serv_edch_info
);

/*============================================================================
FUNCTION: rrcllcpcie_process_sec_edch_comm_info()

DESCRIPTION:This function processes common edch info of secondary EUL. If the secondary EUL
configuration is 'continue' this function will not get hit.


      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllcpcie_process_sec_edch_comm_info
(
  rrc_SecondaryEDCH_Info_Common *sec_edch_comm_info
);



/*============================================================================
FUNCTION: rrcllcpcie_process_dc_hsupa_params()

DESCRIPTION:
  This function processes IE "UL_SecondaryCellInfoFDD" content in ordered config for R9 OTAs. Basically,
  this function updates the OC with the IE content and does some valiadation.

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_dc_hsupa_params
(
  rrc_UL_SecondaryCellInfoFDD * dc_hsupa_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 dc_hudpa_params_present_ota
);

/*============================================================================
FUNCTION: rrcllcpcie_process_sec_e_dl_info_per_rl()

DESCRIPTION:This function processes dl info of each RL of secondary EUL active set.

      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_sec_e_dl_info_per_rl
(
  rrc_DL_InformationPerSecondaryRL_List *dl_info_per_rl_list_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

/*============================================================================
FUNCTION: rrcllcpcie_validate_dc_hsupa_params()

DESCRIPTION:This function validates whether DC HSUPA and DC HSDPA frequency is same or not.

      
DEPENDENCIES:


RETURN VALUE:
E

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllcpcie_validate_dc_hsupa_params
(
void
);

#if 0

/*====================================================================
FUNCTION: rrcllc_init_sec_e_dch_info_before_msg_processing()

DESCRIPTION:
  This function resets needed sec E-DCH variables before processing any config message

DEPENDENCIES:
  Function is only called before processing the DL OTA Msg.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_init_sec_e_dch_info_before_msg_processing
(
  ordered_config_type *config_ptr
);

#endif
#endif /* FEATURE_WCDMA_DC_HSUPA */


/*====================================================================
FUNCTION: rrcllc_handover_to_utran_process_preconfig_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Handover to UTRAN Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_handover_to_utran_process_preconfig_r9
(
  rrc_HandoverToUTRANCommand_r9_IEs *msg_ptr
);


/*====================================================================
FUNCTION: rrcllc_handover_to_utran_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Handover to UTRAN Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_handover_to_utran_ie_r9
(
  rrc_HandoverToUTRANCommand_r9 *msg_ptr
);


/*====================================================================
FUNCTION: rrcllc_rb_reconfig_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Reconfiguration Message R9

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_reconfig_ie_r9
(
  rrc_RadioBearerReconfiguration_r9 * rb_reconfig_r9_ota
);


/*====================================================================
FUNCTION: rrcllc_phychan_reconfig_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Physical Channel Reconfiguration Message for Rel 9

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_phychan_reconfig_ie_r9
(
  rrc_PhysicalChannelReconfiguration_r9 * phys_chan_reconfig_r9_ota
);


/*====================================================================
FUNCTION: rrcllc_rb_release_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 8 Radio Bearer Release Message R8.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_release_ie_r9
(
  rrc_RadioBearerRelease_r9 * rb_release_r9_ota
);


/*====================================================================
FUNCTION: rrcllc_trch_reconfig_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Release 9 Transport Channel Reconfiguration Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_trch_reconfig_ie_r9
(
  rrc_TransportChannelReconfiguration_r9 * trch_reconfig_r9_ota
);


/*====================================================================
FUNCTION: rrcllc_cell_update_confirm_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Cell Update Confirm Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_cell_update_confirm_ie_r9
(
  uint32 dl_sdu_num,
  void * cell_update_r9_msg_ptr

);


/*====================================================================
FUNCTION: rrcllc_rb_setup_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Radio Bearer Setup Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rb_setup_ie_r9
(
  rrc_RadioBearerSetup_r9 * rb_setup_r9_ota
);


/*============================================================================
FUNCTION: rrcllcpcie_process_dl_hspdsch_info_and_update_hsdpa_msg_params_r9()

DESCRIPTION:
  This function updates IE "dl hspdsch info " in ordered config and update
  hsdpa_msg_param variable
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_hspdsch_info_and_upd_hsdpa_msg_params_r9
(
  OSS_UINT32 dl_hspdsch_info_present,
  rrc_DL_HSPDSCH_Information_r9 *dl_hspdsch_info_ptr,
  /* is inter freq handover involved */
  OSS_UINT32 is_ifhho_involved,
  rrc_RRC_StateIndicator rcvd_state_indicator
);


/*============================================================================
FUNCTION: rrcllcpcie_process_ul_e_dch_information_r9()

DESCRIPTION:
  This function updates IE "ul e dch information" in ordered config
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_ul_e_dch_information_r9
(
  OSS_UINT32 e_dch_info_present,
  rrc_UL_EDCH_Information_r9 *e_dch_info_ptr,  
  rrc_RRC_StateIndicator rcvd_state_indicator
);

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
);


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
 
);

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
);

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
);



/*====================================================================
FUNCTION: rrcllc_rrc_conn_setup_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in R8 RRC Connection Setup Message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_rrc_conn_setup_ie_r9
(
  rrc_RRCConnectionSetup_r9 *rrc_conn_setup_r9_ota
);

#ifdef FEATURE_WCDMA_REL10
#error code not present
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA

/*============================================================================
FUNCTION: rrcllcpcie_update_lower_layer_action_for_dc_hsupa_params_r9()

DESCRIPTION:This function determines SEC EDCH actions for both MAC and L1


DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllcpcie_update_lower_layer_action_for_dc_hsupa_params_r9
(
  rrc_UL_SecondaryCellInfoFDD * dc_hsupa_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 dc_hudpa_params_present_ota

);

#endif /* FEATURE_WCDMA_DC_HSUPA */


/*====================================================================
FUNCTION      set_hsscch_info_r9()

DESCRIPTION   This function sets the Channelization code required
              for HSPDSCH in L1 structure in OC.

DEPENDENCIES  None

RETURN VALUE  SUCCESS if the entire operation is successful,
              FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
uecomdef_status_e_type set_hsscch_info_r9
(
  rrc_HS_SCCH_Info_r9* hsscch_info_ptr
);
#ifdef FEATURE_WCDMA_DC_HSUPA
/*============================================================================
FUNCTION: rrcllc_update_l1_sec_e_req_mask_for_e_ul_info()

DESCRIPTION:
  This function updates the sec_e_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related UL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_sec_e_req_mask_for_e_ul_info
(
  boolean e_ul_info_present
);

/*============================================================================
FUNCTION: rrcllc_update_l1_sec_e_req_mask_for_e_dl_info()

DESCRIPTION:
  This function updates the e_req_mask to be sent to layer1.
  This function is called whenever any of the EUL related DL physical layer 
  parameters are modified by reconfig message.

DEPENDENCIES:
  None.

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcllc_update_l1_sec_e_req_mask_for_e_dl_info
(
  boolean e_dl_info_present
);
#endif /* FEATURE_WCDMA_DC_HSUPA */
/*====================================================================
FUNCTION: rrcllc_determine_hsdpa_action_r9()

DESCRIPTION:
  This function determines the HSDPA action and sets the parameters in L1
  structure accordingly.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  Sucess if HSDPA action and parameters have been set
  Failure Otherwise

SIDE EFFECTS:
====================================================================*/
uecomdef_status_e_type rrcllc_determine_hsdpa_action_r9
(
  rrc_DL_HSPDSCH_Information_r9* hspdsch_info_ptr
);


/*===========================================================================
FUNCTION        get_rl_add_info_r9()

DESCRIPTION     This function translates radio link addition information from
                a radio link addition list into the RRC config database. This
                function is only called from an Active Set Update Message

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    The RRC config database as pointed to by the input parameters
                will be updated.
===========================================================================*/
uecomdef_status_e_type get_rl_add_info_r9
(
  rrc_RL_AdditionInformation_r9 *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan,
  boolean *duplicate_add_attempted
);
#ifdef FEATURE_WCDMA_DC_HSUPA
/*===========================================================================
FUNCTION        get_sec_eul_rl_add_info_r9()

DESCRIPTION     This function translates radio link addition information from
                a radio link addition list into the RRC config database. This
                function is only called from an Active Set Update Message

RETURN VALUE    SUCCESS if all goes well, FAILURE otherwise

SIDE EFFECTS    The RRC config database as pointed to by the input parameters
                will be updated.
===========================================================================*/
uecomdef_status_e_type get_sec_eul_rl_add_info_r9
(
  rrc_RL_AdditionInformation_SecULFreq *ie_ptr,
  l1_dl_phychan_db_struct_type *dl_phychan,
  boolean *duplicate_add_attempted
)
;
#endif /* FEATURE_WCDMA_DC_HSUPA */
/*====================================================================
FUNCTION: validate_fdpch_information_r9()

DESCRIPTION:
  This function verifies that there are no mixed RLs with DPCH and FDPCH
  after ASU is processed.

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS if validation is successful.
  FAILURE if there are mixed RLs with FDPCH and DPCH

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type validate_fdpch_information_r9
(
  rrc_RL_AdditionInformationList_r9 *rl_AdditionInformationList_ptr
);

#ifdef FEATURE_WCDMA_MIMO

/*============================================================================
FUNCTION: rrcllcpcie_process_mimo_params_r9()

DESCRIPTION:
  This function updates IE "Mimo Params" in ordered config for R8 OTAs.
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type    rrcllcpcie_process_mimo_params_r9
(
  rrc_MIMO_Parameters_r9 * mimo_params,
  rrc_RRC_StateIndicator rrc_state,
  OSS_UINT32 mimo_params_present_ota
);

#endif/*FEATURE_WCDMA_MIMO*/
/*============================================================================
FUNCTION: rrcllc_process_serving_hsdsch_cell_info_r9()

DESCRIPTION:
  This function updates IE "rrc_Serving_HSDSCH_CellInformation " in ordered config

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_process_serving_hsdsch_cell_info_r9
(
  rrc_Serving_HSDSCH_CellInformation_r9 *hs_srv_cell_ptr
);

/*============================================================================
FUNCTION: rrcllcpcie_process_rl_addition_info_list_ext_asu_r9()

DESCRIPTION:
  This function updates IE "rrc_RL_AdditionInformationList_v950ext " in ordered config

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllcpcie_process_rl_addition_info_list_ext_asu_r9
(
  rrc_RL_AdditionInformationList_v950ext rl_add_ptr
);


/*============================================================================
FUNCTION: rrcllcpcie_process_rl_addition_info_list_r9()

DESCRIPTION:
  This function updates IE "rrc_RL_AdditionInformationList_r9 " in ordered config
  and update e_dch parameters

DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllcpcie_process_rl_addition_info_list_r9
(
  OSS_UINT32 rl_additon_info_list_present,
  rrc_RL_AdditionInformationList_r9 *rl_add_ptr
);

/*====================================================================
FUNCTION: rrcllc_active_set_update_ie_r9()

DESCRIPTION:
  This function updates ordered_config with the latest IEs recv'd 
  in the Active Set Update Message

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcllc_active_set_update_ie_r9
(
  rrc_ActiveSetUpdate_r9 * active_set_update_r8_ota
);

#endif /*FEATURE_WCDMA_REL9*/
/*===========================================================================
FUNCTION rrc_set_predefined_po3_status

DESCRIPTION
  This function sets the predefined po3 status
 

DEPENDENCIES
  This function should be called if OC is set

RETURN VALUE
 None
SIDE EFFECTS
  None
===========================================================================*/
void rrc_set_predefined_po3_status(boolean status);
/*===========================================================================
FUNCTION rrc_get_predefined_po3_status

DESCRIPTION
  This function returns TRUE if prefined PO3 is configured by NW especially for inter RAT HO cases.
  This function is valid for DCH state only.
 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: Predefined PO3 is configured
  FALSE: If PO3 is signalled from NW OTA

SIDE EFFECTS
  None
===========================================================================*/
boolean rrc_get_predefined_po3_status(void);

/*====================================================================
FUNCTION: rrcllc_is_hs_repointing_happening()

DESCRIPTION:
  This function returns if hs repointing is happening or not.
  
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE : HS-repointing is happening
  FALSE HS repointing is not happening 

SIDE EFFECTS:
====================================================================*/
boolean rrcllc_is_hs_repointing_happening(void);

/*============================================================================
FUNCTION: 
uecomdef_status_e_type rrcllcpcie_validate_dbdc_band_combination()

DESCRIPTION:
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/

uecomdef_status_e_type rrcllcpcie_validate_dbdc_band_combination(void);
/*============================================================================
FUNCTION: 
void rrcllcpcie_return_num_sec_hsdpa_carriers(ordered_config_ptr)

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
=============================================================================*/
uint8 rrcllcpcie_return_num_sec_hsdpa_carriers(ordered_config_type* config_ptr);
/*===========================================================================

FUNCTION RRCLLC_CONVERT_TTI_TO_MS

DESCRIPTION
  This function converts the l1_tti_enums to milliseconds

DEPENDENCIES
  None

RETURN VALUE
  uint32

SIDE EFFECTS
  None
===========================================================================*/
uint32 rrc_convert_tti_to_ms
(
  l1_tti_enum_type tti                    
);
/*====================================================================
FUNCTION: rrcllc_validate_hs_ehs_window_size_for_sec()

DESCRIPTION:
  This function checks if the window size is greater than 32.

DEPENDENCIES:
  None

RETURN VALUE:
  boolean

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_validate_hs_ehs_window_size_for_sec
(
    rrc_RRC_StateIndicator rrc_rcvd_state
);
#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION rrc_qsh_log_overall_ota_params
  
DESCRIPTION
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_qsh_log_overall_ota_params(rrc_proc_e_type proc_id);
/*===========================================================================
FUNCTION rrc_qsh_log_ota_params
  
DESCRIPTION
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_qsh_log_ota_params(rrc_proc_e_type proc_id);
/*===========================================================================
FUNCTION rrc_qsh_log_ota_params
DESCRIPTION
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_qsh_log_ul_failure_ota();
/*===========================================================================
FUNCTION rrc_qsh_log_dl_ota_msg_id
  
DESCRIPTION
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void rrc_qsh_log_dl_ota_msg_id(rrc_cmd_type *cmd_ptr, rrc_proc_e_type rrc_proc_name);

/*===========================================================================
FUNCTION rrc_qsh_log_dl_ota_msg_id
  
DESCRIPTION
DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

boolean rrc_qsh_prevent_dl_ota_param_log(rrc_cmd_type *cmd_ptr);


#endif
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
);
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*===========================================================================

FUNCTION: rrcllc_post_reconfig_complete_ind_for_failure

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
TRUE/FALSE

SIDE EFFECTS:
  None
===========================================================================*/

void rrcllc_post_reconfig_complete_ind_for_failure(void);
/*===========================================================================

FUNCTION  rrctfcc_is_reconfig_in_progress

DESCRIPTION

  This function will check if TFCC Procedure substate is reconfig in progress
    
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE: if TFCC procedure is wating for reconfig in progress
  FALSE: othewise.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrctfcc_is_reconfig_in_progress
(
  void
);

#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

/*====================================================================
FUNCTION: rrcllc_check_delay_restriction_ie_trch_reconfig

DESCRIPTION:
  This function returns TRUE if delay restriction is applicable

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_delay_restriction_ie_trch_reconfig
(
  rrc_DL_DCCH_Message *dcch_ptr
);

/*====================================================================
FUNCTION: rrcllc_check_delay_restriction_ie_trch_reconfig_r5

DESCRIPTION:
  This function returns TRUE if delay restriction is applicable

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_delay_restriction_ie_trch_reconfig_r5
(
  rrc_DL_DCCH_Message *dcch_ptr
);

/*====================================================================
FUNCTION: rrcllc_check_delay_restriction_ie_phys_reconfig

DESCRIPTION:
  This function returns TRUE if delay restriction is applicable

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_delay_restriction_ie_phys_reconfig
(
  rrc_DL_DCCH_Message *dcch_ptr
);


/*====================================================================
FUNCTION: rrcllc_check_delay_restriction_ie_phys_reconfig_r5

DESCRIPTION:
  This function returns TRUE if delay restriction is applicable

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_delay_restriction_ie_phys_reconfig_r5
(
  rrc_DL_DCCH_Message *dcch_ptr
);

/*====================================================================
FUNCTION: rrcllc_check_delay_restriction_ie_rb_reconfig

DESCRIPTION:
  This function returns TRUE if delay restriction is applicable

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_delay_restriction_ie_rb_reconfig
(
  rrc_DL_DCCH_Message *dcch_ptr
);

/*===============================================
FUNCTION: rrcllc_check_delay_restriction_ie_rb_reconfig_r5

DESCRIPTION:
  This function returns TRUE if delay restriction is applicable

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_delay_restriction_ie_rb_reconfig_r5 
(
  rrc_DL_DCCH_Message *dcch_ptr
);
/*===========================================================================
 FUNCTION     rrc_log_feature_state_variables_status

 DESCRIPTION
   This functions logs the status of all connected mode feature configuration status

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None

===========================================================================*/
void rrc_log_feature_state_variables_status(ordered_config_type* config_ptr);

/*====================================================================
FUNCTION: rrc_print_cmac_ul_config()

DESCRIPTION:
  This function prints  cmac config of dc hsupa 

DEPENDENCIES:
  None.
RETURN VALUE:
  None
SIDE EFFECTS:
  None
====================================================================*/

void rrc_print_cmac_ul_config(wcdma_l2_ul_cmd_type* l2_ul_cmd_ptr);

/*===========================================================================
 FUNCTION     rrc_log_rb_mapping_info

 DESCRIPTION
   This functions logs all the rbs established and its lch, trch mapping

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None

===========================================================================*/
void rrc_log_rb_mapping_info(ordered_config_type* config_ptr);

/*===========================================================================
 FUNCTION     rrc_log_rlc_am_info

 DESCRIPTION
   This functions logs important AM info

 DEPENDENCIES
   None.

 PARAMETERS
   None.

 RETURN VALUE
   None.

 SIDE EFFECTS
   None

===========================================================================*/

void rrc_log_rlc_am_info();

#endif /* RRCLLCOC_H */
